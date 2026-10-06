// hideui_daemon - the immortal detour service.
//
// This image never unloads. Every site it patches is a five-byte jump into a
// stub that lives in this image's memory, and a thread may be inside that stub
// at any moment, so unmapping it is never safe. Everything that changes --
// which functions are hooked for what, and what the handlers do -- lives in
// the engine image, which stays swappable because the daemon copies nothing
// out of it and calls nothing of it once clear_handlers has returned.
//
// Because it can never be updated without the player restarting the client,
// these rules bind every change to this file:
//
//   - the ABI it publishes is a range and append-only (see hideui_abi.h).
//   - a site is never uninstalled: a stub whose address is on some thread's
//     stack must stay. With no handlers a site is a passthrough.
//   - the stub allocates nothing, takes no lock and touches no per-thread
//     state: the copy of the arguments it makes lives on the stack it was
//     entered on, so recursion and concurrency need no bookkeeping.
//   - it runs no code at process exit: no namespace-scope object here has a
//     non-trivial destructor and DllMain does nothing.

#define WIN32_LEAN_AND_MEAN
#include "hideui_abi.h"

#include <tlhelp32.h>
#include <cstdio>
#include <cstring>

namespace {

const DWORD election_wait_ms_ = 5000;
const SIZE_T stub_page_bytes_ = 4096;
const int suspend_attempts_ = 50;

// Handlers are published as a pointer to one immutable triple, swapped as a
// unit, so a reader never sees a pre from one engine and a post from another.
// Triples are never freed: a reader may hold one across a handler call, and a
// published pointer is only ever replaced, not reused.
struct Handlers {
    HuPreFn pre;
    HuPostFn post;
    void* user;
};

// One patched function. The stub embeds the Site's address, so the hot path
// never walks a table. A Site is one allocation, never freed, never moved.
struct Site {
    int32_t id;
    const uint8_t* target;
    uint32_t prologue_bytes;
    uint32_t stack_arg_bytes;
    uint32_t callee_pops;
    uint8_t original[HU_PROLOGUE_MAX];
    uint8_t jump[5];
    uint8_t* stub;
    uint8_t* trampoline;
    Handlers* volatile handlers;
    volatile LONG inflight;
};

// Plain objects with no constructor and no destructor, zero-initialized into
// .bss by the loader: nothing here may run at image load or at process exit.
Site** g_sites;
LONG g_site_count;
LONG g_site_capacity;

CRITICAL_SECTION g_lock;
volatile LONG g_lock_state;

// g_api_valid is set last by publish() and by adopt() and is the only record of
// whether an election succeeded. There must be no second flag for "an election
// was attempted": latching on the attempt makes every transient failure
// permanent in an image that can never be reloaded.
HuDaemonApi g_public_api;
uint32_t g_resident_abi;
uint32_t g_api_valid;

char g_winner_path[MAX_PATH];
char module_anchor_;

void lock() {
    if (g_lock_state != 2) {
        if (InterlockedCompareExchange(&g_lock_state, 1, 0) == 0) {
            InitializeCriticalSection(&g_lock);
            MemoryBarrier();
            g_lock_state = 2;
        } else {
            while (g_lock_state != 2) {
                SwitchToThread();
            }
        }
    }
    EnterCriticalSection(&g_lock);
}

void unlock() {
    LeaveCriticalSection(&g_lock);
}

void* alloc_zeroed(SIZE_T bytes) {
    return HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, bytes);
}

// ---------------------------------------------------------------------------
// The hot path. Both run inside the stub on whatever thread called the hooked
// function, with the registers it had saved by pushad at `block`:
//   block[0..7]  edi esi ebp esp ebx edx ecx eax
//   block[8]     eflags
//   block[9]     the handler triple this call captured (pre writes it)
//   block[10]    the caller's return address
//   block[11..]  the caller's stack arguments
// A call that captured a triple holds `inflight` from its pre to the end of
// its post, so a drain covers the original too and a post always pairs with
// its own pre.

void fill_frame(HuFrame* frame, const Site* site, const Handlers* handlers, const uint32_t* block) {
    frame->edi = block[0];
    frame->esi = block[1];
    frame->ebp = block[2];
    frame->esp = block[3];
    frame->ebx = block[4];
    frame->edx = block[5];
    frame->ecx = block[6];
    frame->eax = block[7];
    frame->eflags = block[8];
    frame->return_address = block[10];
    frame->args = block + 11;
    frame->result = 0;
    frame->site = site->id;
    frame->user = handlers->user;
}

int __cdecl hu_dispatch_pre(Site* site, uint32_t* block) {
    InterlockedIncrement(&site->inflight);
    const Handlers* handlers = site->handlers;
    block[9] = reinterpret_cast<uint32_t>(handlers);
    if (!handlers) {
        InterlockedDecrement(&site->inflight);
        return 0;
    }
    if (handlers->pre) {
        HuFrame frame;
        fill_frame(&frame, site, handlers, block);
        if (handlers->pre(&frame) != 0) {
            block[7] = frame.result;
            InterlockedDecrement(&site->inflight);
            return 1;
        }
    }
    return 0;
}

void __cdecl hu_dispatch_post(Site* site, uint32_t* block) {
    const Handlers* handlers = reinterpret_cast<const Handlers*>(block[9]);
    if (!handlers) {
        return;
    }
    if (handlers->post) {
        HuFrame frame;
        fill_frame(&frame, site, handlers, block);
        frame.result = block[7];
        handlers->post(&frame);
        block[7] = frame.result;
    }
    InterlockedDecrement(&site->inflight);
}

// ---------------------------------------------------------------------------
// Stub generation. One page per site holds the stub and, after it, the
// trampoline: the relocated prologue followed by a jump to the rest of the
// target. The stub:
//
//   push 0                          ; the slot the captured triple lives in
//   pushfd; pushad; push esp; push site; call hu_dispatch_pre; add esp,8
//   test eax,eax; jnz skip
//   popad; popfd
//   push [esp+N+4] (N/4 times)      ; a copy of the arguments, in order
//   call trampoline                 ; the original runs on the copy
//   add esp,N                       ; only when the callee did not pop it
//   pushfd; pushad; push esp; push site; call hu_dispatch_post; add esp,8
//   popad; popfd
//   add esp,4                       ; drop the triple slot
//   ret N / ret                     ; eax carries the result
// skip:
//   popad; popfd                    ; pre wrote the result into eax's slot
//   add esp,4
//   ret N / ret
//
// The caller's return address and arguments are never moved, so the post
// handler reads the arguments as the caller passed them.

struct Emitter {
    uint8_t* base;
    uint32_t at;
};

void emit8(Emitter* e, uint8_t byte) {
    e->base[e->at++] = byte;
}

void emit32(Emitter* e, uint32_t value) {
    std::memcpy(e->base + e->at, &value, 4);
    e->at += 4;
}

void emit_rel32(Emitter* e, uint8_t opcode, const void* destination) {
    emit8(e, opcode);
    const uint32_t next = reinterpret_cast<uint32_t>(e->base + e->at + 4);
    emit32(e, reinterpret_cast<uint32_t>(destination) - next);
}

void emit_add_esp(Emitter* e, uint32_t bytes) {
    if (bytes == 0) {
        return;
    }
    emit8(e, 0x81); emit8(e, 0xC4);                   // add esp, imm32
    emit32(e, bytes);
}

void emit_dispatch(Emitter* e, const Site* site, const void* function) {
    emit8(e, 0x9C);                                   // pushfd
    emit8(e, 0x60);                                   // pushad
    emit8(e, 0x54);                                   // push esp
    emit8(e, 0x68); emit32(e, reinterpret_cast<uint32_t>(site));  // push site
    emit_rel32(e, 0xE8, function);                    // call
    emit8(e, 0x83); emit8(e, 0xC4); emit8(e, 0x08);   // add esp, 8
}

void emit_restore(Emitter* e) {
    emit8(e, 0x61);                                   // popad
    emit8(e, 0x9D);                                   // popfd
}

void emit_return(Emitter* e, const Site* site) {
    emit_add_esp(e, 4);                               // the triple slot
    const uint32_t pops = site->callee_pops ? site->stack_arg_bytes : 0;
    if (pops == 0) {
        emit8(e, 0xC3);                               // ret
    } else {
        emit8(e, 0xC2);                               // ret imm16
        emit8(e, static_cast<uint8_t>(pops & 0xFF));
        emit8(e, static_cast<uint8_t>(pops >> 8));
    }
}

// Returns the trampoline's address, or NULL when the page cannot hold it.
uint8_t* emit_site(Site* site, uint8_t* page) {
    Emitter e;
    e.base = page;
    e.at = 0;

    emit8(&e, 0x6A); emit8(&e, 0x00);                 // push 0
    emit_dispatch(&e, site, reinterpret_cast<const void*>(&hu_dispatch_pre));
    emit8(&e, 0x85); emit8(&e, 0xC0);                 // test eax, eax
    emit8(&e, 0x0F); emit8(&e, 0x85);                 // jnz rel32 (patched below)
    const uint32_t skip_fixup = e.at;
    emit32(&e, 0);
    emit_restore(&e);

    // push dword [esp+N+4], N/4 times: each push shifts the originals up by
    // four, so the same displacement reaches the next-lower argument every
    // time; the +4 steps over the triple slot.
    for (uint32_t i = 0; i < site->stack_arg_bytes / 4; ++i) {
        emit8(&e, 0xFF); emit8(&e, 0xB4); emit8(&e, 0x24);
        emit32(&e, site->stack_arg_bytes + 4);
    }

    const uint32_t call_fixup = e.at;
    emit_rel32(&e, 0xE8, NULL);                       // call trampoline (patched below)
    if (!site->callee_pops) {
        emit_add_esp(&e, site->stack_arg_bytes);
    }
    emit_dispatch(&e, site, reinterpret_cast<const void*>(&hu_dispatch_post));
    emit_restore(&e);
    emit_return(&e, site);

    const uint32_t skip_at = e.at;
    emit_restore(&e);
    emit_return(&e, site);

    // 16-byte align the trampoline so a debugger reads it as its own thing.
    while (e.at % 16 != 0) {
        emit8(&e, 0xCC);
    }
    uint8_t* trampoline = page + e.at;
    std::memcpy(page + e.at, site->original, site->prologue_bytes);
    e.at += site->prologue_bytes;
    emit_rel32(&e, 0xE9, site->target + site->prologue_bytes);

    if (e.at > stub_page_bytes_) {
        return NULL;
    }

    uint32_t rel = static_cast<uint32_t>(skip_at - (skip_fixup + 4));
    std::memcpy(page + skip_fixup, &rel, 4);
    rel = static_cast<uint32_t>(trampoline - (page + call_fixup + 5));
    std::memcpy(page + call_fixup + 1, &rel, 4);
    return trampoline;
}

// ---------------------------------------------------------------------------
// Writing the jump. Another thread may be executing the bytes being replaced,
// so every other thread is suspended and checked to be outside the prologue
// before the write; one that is inside is given a moment and the check is
// repeated. Nothing here allocates while threads are suspended: the handle
// array is sized from a snapshot beforehand.

struct Suspended {
    HANDLE* handles;
    DWORD count;
    DWORD capacity;
};

void resume_all(Suspended* s) {
    for (DWORD i = 0; i < s->count; ++i) {
        ResumeThread(s->handles[i]);
        CloseHandle(s->handles[i]);
    }
    s->count = 0;
}

DWORD count_threads() {
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (snap == INVALID_HANDLE_VALUE) {
        return 0;
    }
    DWORD count = 0;
    THREADENTRY32 entry;
    entry.dwSize = sizeof(entry);
    if (Thread32First(snap, &entry)) {
        do {
            if (entry.th32OwnerProcessID == GetCurrentProcessId()) {
                ++count;
            }
        } while (Thread32Next(snap, &entry));
    }
    CloseHandle(snap);
    return count;
}

// Suspends every other thread of the process. Returns 0 when a thread could
// not be suspended or more threads exist than there is room for; the caller
// then resumes what was suspended and tries again with a bigger array.
int suspend_others(Suspended* s) {
    HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (snap == INVALID_HANDLE_VALUE) {
        return 0;
    }
    const DWORD self = GetCurrentThreadId();
    int ok = 1;
    THREADENTRY32 entry;
    entry.dwSize = sizeof(entry);
    if (Thread32First(snap, &entry)) {
        do {
            if (entry.th32OwnerProcessID != GetCurrentProcessId() || entry.th32ThreadID == self) {
                continue;
            }
            if (s->count == s->capacity) {
                ok = 0;
                break;
            }
            HANDLE thread = OpenThread(THREAD_SUSPEND_RESUME | THREAD_GET_CONTEXT | THREAD_QUERY_INFORMATION,
                FALSE, entry.th32ThreadID);
            if (!thread) {
                continue;   // already gone
            }
            if (SuspendThread(thread) == static_cast<DWORD>(-1)) {
                CloseHandle(thread);
                continue;   // exited between the snapshot and here
            }
            s->handles[s->count++] = thread;
        } while (Thread32Next(snap, &entry));
    }
    CloseHandle(snap);
    return ok;
}

int any_inside(const Suspended* s, const uint8_t* lo, const uint8_t* hi) {
    for (DWORD i = 0; i < s->count; ++i) {
        CONTEXT context;
        std::memset(&context, 0, sizeof(context));
        context.ContextFlags = CONTEXT_CONTROL;
        if (!GetThreadContext(s->handles[i], &context)) {
            return 1;   // unknown is inside
        }
        const uint8_t* eip = reinterpret_cast<const uint8_t*>(context.Eip);
        if (eip >= lo && eip < hi) {
            return 1;
        }
    }
    return 0;
}

int write_bytes(uint8_t* at, const uint8_t* bytes, SIZE_T count) {
    DWORD old = 0;
    if (!VirtualProtect(at, count, PAGE_EXECUTE_READWRITE, &old)) {
        return HU_E_PROTECT;
    }
    std::memcpy(at, bytes, count);
    DWORD ignored = 0;
    VirtualProtect(at, count, old, &ignored);
    FlushInstructionCache(GetCurrentProcess(), at, count);
    return HU_OK;
}

int write_jump(Site* site) {
    uint8_t* target = const_cast<uint8_t*>(site->target);
    const uint8_t* lo = site->target;
    const uint8_t* hi = site->target + site->prologue_bytes;

    Suspended s;
    s.count = 0;
    s.capacity = count_threads() + 16;
    s.handles = static_cast<HANDLE*>(alloc_zeroed(sizeof(HANDLE) * s.capacity));
    if (!s.handles) {
        return HU_E_MEMORY;
    }

    int result = HU_E_THREADS;
    for (int attempt = 0; attempt < suspend_attempts_; ++attempt) {
        if (!suspend_others(&s)) {
            resume_all(&s);
            HeapFree(GetProcessHeap(), 0, s.handles);
            s.capacity *= 2;
            s.handles = static_cast<HANDLE*>(alloc_zeroed(sizeof(HANDLE) * s.capacity));
            if (!s.handles) {
                return HU_E_MEMORY;
            }
            continue;
        }
        if (!any_inside(&s, lo, hi)) {
            result = write_bytes(target, site->jump, sizeof(site->jump));
            resume_all(&s);
            break;
        }
        resume_all(&s);
        Sleep(1);
    }
    HeapFree(GetProcessHeap(), 0, s.handles);
    return result;
}

// ---------------------------------------------------------------------------

int target_is_code(const void* target, SIZE_T bytes) {
    MEMORY_BASIC_INFORMATION info;
    if (VirtualQuery(target, &info, sizeof(info)) != sizeof(info)) {
        return 0;
    }
    if (info.State != MEM_COMMIT) {
        return 0;
    }
    const DWORD protect = info.Protect & 0xFF;
    const int executable = protect == PAGE_EXECUTE || protect == PAGE_EXECUTE_READ
        || protect == PAGE_EXECUTE_READWRITE || protect == PAGE_EXECUTE_WRITECOPY;
    if (!executable) {
        return 0;
    }
    const uint8_t* end = static_cast<const uint8_t*>(info.BaseAddress) + info.RegionSize;
    return static_cast<const uint8_t*>(target) + bytes <= end;
}

// A first byte that is already a jump, a call or a breakpoint is someone
// else's patch. Chaining over it would make its removal unrecoverable.
int looks_patched(const uint8_t* bytes) {
    if (bytes[0] == 0xE9 || bytes[0] == 0xE8 || bytes[0] == 0xCC) {
        return 1;
    }
    return bytes[0] == 0xFF && bytes[1] == 0x25;
}

Site* site_by_id(int32_t id) {
    if (id < 0 || id >= g_site_count) {
        return NULL;
    }
    return g_sites[id];
}

Site* site_by_target(const void* target) {
    for (LONG i = 0; i < g_site_count; ++i) {
        if (g_sites[i]->target == target) {
            return g_sites[i];
        }
    }
    return NULL;
}

int append_site(Site* site) {
    if (g_site_count == g_site_capacity) {
        const LONG capacity = g_site_capacity ? g_site_capacity * 2 : 8;
        Site** grown = static_cast<Site**>(alloc_zeroed(sizeof(Site*) * capacity));
        if (!grown) {
            return 0;
        }
        if (g_sites) {
            std::memcpy(grown, g_sites, sizeof(Site*) * g_site_count);
            HeapFree(GetProcessHeap(), 0, g_sites);
        }
        g_sites = grown;
        g_site_capacity = capacity;
    }
    site->id = g_site_count;
    g_sites[g_site_count++] = site;
    return 1;
}

int32_t install_locked(const HuSiteDesc* desc) {
    const uint8_t* target = static_cast<const uint8_t*>(desc->target);

    Site* existing = site_by_target(target);
    if (existing) {
        if (std::memcmp(target, existing->jump, sizeof(existing->jump)) == 0) {
            return existing->id;
        }
        // Our jump is gone. The original bytes back in place is a restore we
        // can redo; anything else is a foreign patch.
        if (std::memcmp(target, existing->original, sizeof(existing->jump)) != 0) {
            return HU_E_PATCHED;
        }
        const int written = write_jump(existing);
        return written == HU_OK ? existing->id : written;
    }

    if (!target_is_code(target, desc->prologue_bytes)) {
        return HU_E_TARGET;
    }
    if (looks_patched(target)) {
        return HU_E_PATCHED;
    }

    Site* site = static_cast<Site*>(alloc_zeroed(sizeof(Site)));
    if (!site) {
        return HU_E_MEMORY;
    }
    site->target = target;
    site->prologue_bytes = desc->prologue_bytes;
    site->stack_arg_bytes = desc->stack_arg_bytes;
    site->callee_pops = desc->callee_pops ? 1 : 0;
    std::memcpy(site->original, target, desc->prologue_bytes);

    uint8_t* page = static_cast<uint8_t*>(VirtualAlloc(NULL, stub_page_bytes_,
        MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));
    if (!page) {
        HeapFree(GetProcessHeap(), 0, site);
        return HU_E_MEMORY;
    }
    site->stub = page;
    site->trampoline = emit_site(site, page);
    DWORD old = 0;
    if (!site->trampoline || !VirtualProtect(page, stub_page_bytes_, PAGE_EXECUTE_READ, &old)) {
        VirtualFree(page, 0, MEM_RELEASE);
        HeapFree(GetProcessHeap(), 0, site);
        return HU_E_MEMORY;
    }

    site->jump[0] = 0xE9;
    const uint32_t rel = reinterpret_cast<uint32_t>(page) - (reinterpret_cast<uint32_t>(target) + 5);
    std::memcpy(site->jump + 1, &rel, 4);

    // The table entry comes first: the write makes the stub live, and the stub
    // must be able to be looked up by id from the moment it can run.
    if (!append_site(site)) {
        VirtualFree(page, 0, MEM_RELEASE);
        HeapFree(GetProcessHeap(), 0, site);
        return HU_E_MEMORY;
    }
    const int written = write_jump(site);
    if (written != HU_OK) {
        // The site stays in the table, unpatched: a second install of the same
        // target finds the original bytes in place and writes again.
        return written;
    }
    return site->id;
}

int32_t __stdcall api_install(const HuSiteDesc* desc) {
    if (!desc || desc->size < sizeof(HuSiteDesc) || !desc->target) {
        return HU_E_ARG;
    }
    if (desc->prologue_bytes < HU_PROLOGUE_MIN || desc->prologue_bytes > HU_PROLOGUE_MAX) {
        return HU_E_PROLOGUE;
    }
    if (desc->stack_arg_bytes > HU_ARG_BYTES_MAX || desc->stack_arg_bytes % 4 != 0) {
        return HU_E_ARG;
    }
    if (desc->callee_pops > 1) {
        return HU_E_ARG;
    }
    lock();
    const int32_t result = install_locked(desc);
    unlock();
    return result;
}

int32_t __stdcall api_set_handlers(int32_t id, HuPreFn pre, HuPostFn post, void* user) {
    lock();
    Site* site = site_by_id(id);
    if (!site) {
        unlock();
        return HU_E_SITE;
    }
    const Handlers* next = NULL;
    if (pre || post) {
        Handlers* made = static_cast<Handlers*>(alloc_zeroed(sizeof(Handlers)));
        if (!made) {
            unlock();
            return HU_E_MEMORY;
        }
        made->pre = pre;
        made->post = post;
        made->user = user;
        next = made;
    }
    InterlockedExchangePointer(reinterpret_cast<void* volatile*>(&site->handlers), const_cast<Handlers*>(next));
    unlock();
    return HU_OK;
}

int32_t __stdcall api_clear_handlers(int32_t id, uint32_t timeout_ms) {
    lock();
    Site* site = site_by_id(id);
    if (!site) {
        unlock();
        return HU_E_SITE;
    }
    InterlockedExchangePointer(reinterpret_cast<void* volatile*>(&site->handlers), NULL);
    unlock();

    // A thread that incremented inflight before the exchange may still be
    // inside the old handlers; one that increments after it reads NULL.
    const DWORD start = GetTickCount();
    while (site->inflight != 0) {
        if (GetTickCount() - start > timeout_ms) {
            return HU_E_BUSY;
        }
        Sleep(1);
    }
    return HU_OK;
}

uint32_t __stdcall api_in_flight(int32_t id) {
    lock();
    Site* site = site_by_id(id);
    const uint32_t count = site ? static_cast<uint32_t>(site->inflight) : 0;
    unlock();
    return count;
}

int32_t __stdcall api_site_info(int32_t id, HuSiteInfo* out) {
    if (!out || out->size < sizeof(HuSiteInfo)) {
        return HU_E_ARG;
    }
    lock();
    Site* site = site_by_id(id);
    if (!site) {
        unlock();
        return HU_E_SITE;
    }
    HuSiteInfo info;
    std::memset(&info, 0, sizeof(info));
    info.size = sizeof(info);
    info.target = site->target;
    info.trampoline = site->trampoline;
    info.prologue_bytes = site->prologue_bytes;
    info.stack_arg_bytes = site->stack_arg_bytes;
    info.callee_pops = site->callee_pops;
    std::memcpy(info.original, site->original, sizeof(info.original));
    info.displaced = std::memcmp(site->target, site->jump, sizeof(site->jump)) != 0;
    info.in_flight = static_cast<uint32_t>(site->inflight);
    unlock();
    std::memcpy(out, &info, sizeof(info));
    return HU_OK;
}

const char* __stdcall api_build_id() {
    return HU_DAEMON_BUILD;
}

// ---------------------------------------------------------------------------
// The election, as worlddraw runs it: the first copy to publish into the
// pid-scoped mapping serves every later copy, which hands out its own copy of
// the winner's api. Function pointers point into the winner, which is pinned.

// Pinned by two independent means, because hooking from an image that can unmap
// is the one thing this design cannot survive. Pin defeats FreeLibrary; the
// extra LoadLibraryW covers a loader that refused the pin. Either alone is
// enough, and neither leaves anything to balance.
bool pin_self() {
    HMODULE self = NULL;
    const bool pinned = GetModuleHandleExA(
        GET_MODULE_HANDLE_EX_FLAG_PIN | GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
        &module_anchor_, &self) != FALSE;

    if (!self) {
        GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS
                | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            &module_anchor_, &self);
    }
    if (!self) {
        return false;
    }

    const DWORD copied = GetModuleFileNameA(self, g_winner_path, MAX_PATH);
    if (copied == 0 || copied >= MAX_PATH) {
        g_winner_path[0] = '\0';
    }
    g_winner_path[MAX_PATH - 1] = '\0';

    WCHAR wide_path[MAX_PATH];
    const DWORD wide = GetModuleFileNameW(self, wide_path, MAX_PATH);
    bool loaded = false;
    if (wide != 0 && wide < MAX_PATH) {
        loaded = LoadLibraryW(wide_path) != NULL;
    }

    return pinned || loaded;
}

void fill_api(HuDaemonApi* api) {
    std::memset(api, 0, sizeof(*api));
    api->abi_version = HU_DAEMON_ABI;
    api->size = sizeof(HuDaemonApi);
    api->install = &api_install;
    api->set_handlers = &api_set_handlers;
    api->clear_handlers = &api_clear_handlers;
    api->in_flight = &api_in_flight;
    api->site_info = &api_site_info;
    api->build_id = &api_build_id;
}

void publish(HuDaemonRecord* record) {
    std::memset(record, 0, sizeof(*record));

    HuDaemonApi api;
    fill_api(&api);

    record->record_size = sizeof(HuDaemonRecord);
    record->abi_version = HU_DAEMON_ABI;
    record->api = api;
    std::memcpy(record->winner_path, g_winner_path, sizeof(record->winner_path));
    record->winner_path[MAX_PATH - 1] = '\0';
    std::memcpy(record->winner_build, HU_DAEMON_BUILD, sizeof(HU_DAEMON_BUILD));
    record->winner_build[HU_BUILD_MAX - 1] = '\0';

    // Written last, behind a barrier: magic is the only thing that says the
    // rest of the record is there, including to an image that arrives after a
    // crash mid-publish and finds it still zero.
    MemoryBarrier();
    record->magic = HU_MAGIC;

    g_public_api = api;
    g_resident_abi = HU_DAEMON_ABI;
    g_api_valid = 1;
}

// A loser copies the api out of the record by value and hands out a pointer to
// that copy, so nothing it returns points into the record.
void adopt(const HuDaemonRecord* record) {
    if (record->magic != HU_MAGIC || record->record_size < sizeof(HuDaemonRecord)) {
        return;
    }
    if (record->api.size < sizeof(HuDaemonApi) || record->abi_version < 1) {
        return;
    }

    HuDaemonApi api;
    std::memset(&api, 0, sizeof(api));
    std::memcpy(&api, &record->api, sizeof(api));

    // Never advertise more than was copied: a longer api in the record is a
    // newer daemon, and the fields past ours were not brought across.
    api.size = sizeof(HuDaemonApi);

    if (!api.install || !api.set_handlers || !api.clear_handlers
        || !api.in_flight || !api.site_info || !api.build_id) {
        return;
    }

    g_public_api = api;
    g_resident_abi = record->abi_version;
    g_api_valid = 1;
}

void run_election() {
    // Pin before anything else. An image that cannot pin itself publishes
    // nothing and patches nothing.
    if (!pin_self()) {
        return;
    }

    const DWORD pid = GetCurrentProcessId();
    char mapping_name[HU_NAME_MAX];
    char mutex_name[HU_NAME_MAX];
    std::snprintf(mapping_name, sizeof(mapping_name), HU_MAPPING_NAME_FORMAT, static_cast<unsigned>(pid));
    std::snprintf(mutex_name, sizeof(mutex_name), HU_MUTEX_NAME_FORMAT, static_cast<unsigned>(pid));

    HANDLE mutex = CreateMutexA(NULL, FALSE, mutex_name);
    if (!mutex) {
        return;
    }

    // WAIT_ABANDONED means the previous holder died holding it, which is the
    // mid-election crash the publish predicate below recovers. Anything else
    // means we do not hold it, and proceeding would race two images through the
    // publish.
    const DWORD wait = WaitForSingleObject(mutex, election_wait_ms_);
    if (wait != WAIT_OBJECT_0 && wait != WAIT_ABANDONED) {
        CloseHandle(mutex);
        return;
    }

    HANDLE mapping = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0,
        HU_MAPPING_BYTES, mapping_name);
    // Read immediately: any call in between overwrites it.
    const DWORD create_error = GetLastError();
    if (!mapping) {
        ReleaseMutex(mutex);
        CloseHandle(mutex);
        return;
    }

    const bool created = create_error != ERROR_ALREADY_EXISTS;
    void* view = MapViewOfFile(mapping, FILE_MAP_ALL_ACCESS, 0, 0, HU_MAPPING_BYTES);
    if (!view) {
        CloseHandle(mapping);
        ReleaseMutex(mutex);
        CloseHandle(mutex);
        return;
    }

    HuDaemonRecord* const record = static_cast<HuDaemonRecord*>(view);

    // Evaluated under the mutex. A magic that is not ours proves nobody finished
    // publishing, so taking over is safe -- without this, one image dying
    // mid-election poisons the mapping for the life of the process.
    if (created || record->magic != HU_MAGIC) {
        publish(record);
    } else {
        adopt(record);
    }

    ReleaseMutex(mutex);

    // The view, the section handle and the mutex handle are kept for the life of
    // the process: the record has to outlive every image that reads it.
}

}  // namespace

// Called after LoadLibraryW returns, never from DllMain: the election takes a
// lock and may LoadLibrary, which deadlocks under the loader lock. The version
// test is a range -- an engine works with any daemon at least as new as the one
// it was built for.
//
// g_api_valid is the election's result, never the fact that it was attempted,
// so a transient failure is retried by the next caller. One attempt per call.
extern "C" const HuDaemonApi* __stdcall hu_daemon_acquire(uint32_t min_abi) {
    lock();
    if (!g_api_valid) {
        run_election();
    }
    unlock();

    if (!g_api_valid || g_resident_abi < min_abi) {
        return NULL;
    }

    return &g_public_api;
}

// Nothing, for every reason code. Work here runs under the loader lock, and the
// only work this image could want to do -- draining, unpatching, freeing -- is
// exactly what must never happen there.
extern "C" BOOL WINAPI DllMain(HINSTANCE, DWORD, LPVOID) {
    return TRUE;
}
