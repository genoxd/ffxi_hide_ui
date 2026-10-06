// daemon_harness - proves hideui_daemon.dll across a real DLL boundary, under
// wine, on functions whose prologues are written by hand so that the bytes the
// daemon relocates are known exactly. One of them has the game's menu-open
// routine's own prologue.

#define WIN32_LEAN_AND_MEAN
#include "../../daemon/hideui_abi.h"

#include <cstdio>
#include <cstring>

namespace {

int g_failures;
int g_checks;

void check(bool ok, const char* what) {
    ++g_checks;
    if (!ok) {
        ++g_failures;
    }
    std::printf("%s  %s\n", ok ? "PASS" : "FAIL", what);
}

}  // namespace

// ---------------------------------------------------------------------------
// Hooked test functions. Each prologue is exactly the bytes the test names.

extern "C" {

// Prologue 8b 44 24 04 83 ec 20 (mov eax,[esp+4]; sub esp,0x20): the menu-open
// routine's shape. thiscall(this, a, b, c), ret 0xC, returns a+b+c+*this.
__attribute__((naked)) void open_shape() {
    asm volatile(
        "movl 4(%esp), %eax\n\t"
        "subl $0x20, %esp\n\t"
        "addl 0x28(%esp), %eax\n\t"
        "addl 0x2c(%esp), %eax\n\t"
        "addl (%ecx), %eax\n\t"
        "addl $0x20, %esp\n\t"
        "ret $12\n\t");
}

// Prologue 55 8b ec 56 57 (push ebp; mov ebp,esp; push esi; push edi), five
// bytes exactly. thiscall(this, a, b), ret 8, returns a*b+*this.
__attribute__((naked)) void mul_shape() {
    asm volatile(
        "pushl %ebp\n\t"
        "movl %esp, %ebp\n\t"
        "pushl %esi\n\t"
        "pushl %edi\n\t"
        "movl 8(%ebp), %eax\n\t"
        "imull 12(%ebp), %eax\n\t"
        "addl (%ecx), %eax\n\t"
        "popl %edi\n\t"
        "popl %esi\n\t"
        "popl %ebp\n\t"
        "ret $8\n\t");
}

// Same prologue as open_shape; thiscall(this, n), ret 4; returns n+(n-1)+..+0
// by calling itself through its own first bytes, so every level runs through
// the hook.
__attribute__((naked)) void rec_shape() {
    asm volatile(
        "movl 4(%esp), %eax\n\t"
        "subl $0x20, %esp\n\t"
        "testl %eax, %eax\n\t"
        "jz 1f\n\t"
        "pushl %eax\n\t"
        "decl %eax\n\t"
        "pushl %eax\n\t"
        "call _rec_shape\n\t"
        "popl %ecx\n\t"
        "addl %ecx, %eax\n\t"
        "1:\n\t"
        "addl $0x20, %esp\n\t"
        "ret $4\n\t");
}

// Prologue 55 8b ec 56 57; cdecl(a, b), plain ret, returns a-b.
__attribute__((naked)) void cdecl_shape() {
    asm volatile(
        "pushl %ebp\n\t"
        "movl %esp, %ebp\n\t"
        "pushl %esi\n\t"
        "pushl %edi\n\t"
        "movl 8(%ebp), %eax\n\t"
        "subl 12(%ebp), %eax\n\t"
        "popl %edi\n\t"
        "popl %esi\n\t"
        "popl %ebp\n\t"
        "ret\n\t");
}

// call3(fn, self, a, b, c, &delta): pushes c, b, a, sets ecx = self, calls fn,
// and reports how many bytes the callee left on the stack. A callee with
// `ret N` leaves 12-N; an unbalanced stub shows up as anything else.
__attribute__((naked)) uint32_t call3(const void*, void*, uint32_t, uint32_t, uint32_t, uint32_t*) {
    asm volatile(
        "pushl %ebp\n\t"
        "movl %esp, %ebp\n\t"
        "pushl %esi\n\t"
        "movl %esp, %esi\n\t"
        "pushl 24(%ebp)\n\t"
        "pushl 20(%ebp)\n\t"
        "pushl 16(%ebp)\n\t"
        "movl 12(%ebp), %ecx\n\t"
        "call *8(%ebp)\n\t"
        "movl %esi, %edx\n\t"
        "subl %esp, %edx\n\t"
        "movl 28(%ebp), %ecx\n\t"
        "movl %edx, (%ecx)\n\t"
        "movl %esi, %esp\n\t"
        "popl %esi\n\t"
        "popl %ebp\n\t"
        "ret\n\t");
}

}  // extern "C"

namespace {

// ---------------------------------------------------------------------------
// Handlers record what they saw into these.

struct Seen {
    volatile LONG pre_calls;
    volatile LONG post_calls;
    HuFrame last_pre;
    HuFrame last_post;
    uint32_t pre_args[3];
    uint32_t post_args[3];
    uint32_t skip_with;        // nonzero: pre skips the original and returns this
    uint32_t override_result;  // nonzero: post replaces the result with this
    uint32_t sleep_ms;         // pre sleeps this long (drain test)
};

int __cdecl on_pre(HuFrame* frame) {
    Seen* seen = static_cast<Seen*>(frame->user);
    InterlockedIncrement(&seen->pre_calls);
    seen->last_pre = *frame;
    for (int i = 0; i < 3; ++i) {
        seen->pre_args[i] = frame->args[i];
    }
    if (seen->sleep_ms) {
        Sleep(seen->sleep_ms);
    }
    if (seen->skip_with) {
        frame->result = seen->skip_with;
        return 1;
    }
    return 0;
}

void __cdecl on_post(HuFrame* frame) {
    Seen* seen = static_cast<Seen*>(frame->user);
    InterlockedIncrement(&seen->post_calls);
    seen->last_post = *frame;
    for (int i = 0; i < 3; ++i) {
        seen->post_args[i] = frame->args[i];
    }
    if (seen->override_result) {
        frame->result = seen->override_result;
    }
}

const HuDaemonApi* load_daemon(const char* path, uint32_t min_abi, HMODULE* module_out) {
    HMODULE module = LoadLibraryA(path);
    if (module_out) {
        *module_out = module;
    }
    if (!module) {
        std::printf("  LoadLibrary(%s) failed: %lu\n", path, GetLastError());
        return NULL;
    }
    HuDaemonAcquire acquire = reinterpret_cast<HuDaemonAcquire>(
        reinterpret_cast<void*>(GetProcAddress(module, HU_DAEMON_ACQUIRE_NAME)));
    if (!acquire) {
        std::printf("  %s has no %s\n", path, HU_DAEMON_ACQUIRE_NAME);
        return NULL;
    }
    return acquire(min_abi);
}

int32_t install(const HuDaemonApi* api, const void* target, uint32_t prologue, uint32_t arg_bytes,
                uint32_t callee_pops = 1) {
    HuSiteDesc desc;
    desc.size = sizeof(desc);
    desc.target = target;
    desc.prologue_bytes = prologue;
    desc.stack_arg_bytes = arg_bytes;
    desc.callee_pops = callee_pops;
    return api->install(&desc);
}

// ---------------------------------------------------------------------------
// The hammer: two threads calling mul_shape as fast as they can while the main
// thread installs the hook and swaps handlers underneath them.

volatile LONG g_hammer_stop;

// Runs until told to stop, checking every result against the formula as it
// goes, so the hook can be installed and its handlers swapped at any moment
// of the run.
struct Hammer {
    const void* fn;
    uint32_t self_value;
    uint32_t calls;
    uint32_t wrong;
    uint32_t imbalanced;
};

DWORD WINAPI hammer_thread(LPVOID arg) {
    Hammer* h = static_cast<Hammer*>(arg);
    for (uint32_t i = 0; !g_hammer_stop; ++i) {
        uint32_t delta = 0;
        const uint32_t a = i & 0xFF;
        const uint32_t b = (i >> 8) & 0xFF;
        const uint32_t got = call3(h->fn, &h->self_value, a, b, 0, &delta);
        ++h->calls;
        if (got != a * b + h->self_value) {
            ++h->wrong;
        }
        if (delta != 4) {
            ++h->imbalanced;
        }
    }
    return 0;
}

struct OneCall {
    const void* fn;
    uint32_t self_value;
    uint32_t result;
};

DWORD WINAPI one_call_thread(LPVOID arg) {
    OneCall* c = static_cast<OneCall*>(arg);
    uint32_t delta = 0;
    c->result = call3(c->fn, &c->self_value, 2, 3, 0, &delta);
    return 0;
}

bool overwrite_code(const void* at, const uint8_t* bytes, SIZE_T count) {
    DWORD old = 0;
    if (!VirtualProtect(const_cast<void*>(at), count, PAGE_EXECUTE_READWRITE, &old)) {
        return false;
    }
    std::memcpy(const_cast<void*>(at), bytes, count);
    VirtualProtect(const_cast<void*>(at), count, old, &old);
    FlushInstructionCache(GetCurrentProcess(), at, count);
    return true;
}

}  // namespace

int main(int argc, char** argv) {
    const char* dir_a = argc > 1 ? argv[1] : "build\\a\\hideui_daemon.dll";
    const char* dir_b = argc > 2 ? argv[2] : "build\\b\\hideui_daemon.dll";

    // 1. election
    HMODULE module_a = NULL;
    HMODULE module_b = NULL;
    const HuDaemonApi* a = load_daemon(dir_a, HU_DAEMON_ABI, &module_a);
    const HuDaemonApi* b = load_daemon(dir_b, HU_DAEMON_ABI, &module_b);
    check(a != NULL, "copy A acquires");
    check(b != NULL, "copy B acquires");
    if (!a || !b) {
        std::printf("%d of %d checks failed\n", g_failures, g_checks);
        return g_failures;
    }
    check(a->install == b->install && a->site_info == b->site_info,
        "copy B hands out the resident copy's functions");
    check(std::strcmp(a->build_id(), HU_DAEMON_BUILD) == 0, "build id is this build");
    {
        HMODULE again = NULL;
        const HuDaemonApi* too_new = load_daemon(dir_b, HU_DAEMON_ABI + 1, &again);
        check(too_new == NULL, "min_abi above the resident refuses");
    }

    // 2. passthrough
    const int32_t open_site = install(a, reinterpret_cast<const void*>(&open_shape), 7, 12);
    check(open_site >= 0, "install on the open-routine prologue");
    {
        bool ok = open_site >= 0;
        uint32_t self_value = 1000;
        for (uint32_t i = 0; i < 1000 && ok; ++i) {
            uint32_t delta = 0;
            const uint32_t got = call3(reinterpret_cast<const void*>(&open_shape), &self_value, i, i * 2, i * 3, &delta);
            ok = got == i * 6 + 1000 && delta == 0;
        }
        check(ok, "passthrough with no handlers keeps results and stack");
    }

    // 3. pre/post observation, result override, skip
    Seen seen;
    std::memset(&seen, 0, sizeof(seen));
    check(a->set_handlers(open_site, &on_pre, &on_post, &seen) == HU_OK, "set_handlers");
    {
        uint32_t self_value = 7;
        uint32_t delta = 0;
        const uint32_t got = call3(reinterpret_cast<const void*>(&open_shape), &self_value, 10, 20, 30, &delta);
        check(got == 67 && delta == 0, "hooked call returns the original's result");
        check(seen.pre_calls == 1 && seen.post_calls == 1, "pre and post each ran once");
        check(seen.last_pre.ecx == reinterpret_cast<uint32_t>(&self_value), "pre sees ecx = this");
        check(seen.pre_args[0] == 10 && seen.pre_args[1] == 20 && seen.pre_args[2] == 30, "pre sees the arguments");
        check(seen.post_args[0] == 10 && seen.post_args[1] == 20 && seen.post_args[2] == 30, "post sees the arguments");
        check(seen.last_post.result == 67, "post sees the original's result");
        check(seen.last_pre.return_address == seen.last_post.return_address
            && seen.last_pre.return_address != 0, "pre and post see the same return address");
        check(seen.last_pre.site == open_site, "frame carries the site id");

        seen.override_result = 4242;
        const uint32_t overridden = call3(reinterpret_cast<const void*>(&open_shape), &self_value, 1, 2, 3, &delta);
        check(overridden == 4242 && delta == 0, "post can replace the result");
        seen.override_result = 0;

        seen.skip_with = 0xBEEF;
        const LONG posts_before = seen.post_calls;
        const uint32_t skipped = call3(reinterpret_cast<const void*>(&open_shape), &self_value, 1, 2, 3, &delta);
        check(skipped == 0xBEEF && delta == 0, "pre skip returns the chosen value with a balanced stack");
        check(seen.post_calls == posts_before, "a skipped call runs no post handler");
        seen.skip_with = 0;
    }

    // 4. recursion through the hook
    const int32_t rec_site = install(a, reinterpret_cast<const void*>(&rec_shape), 7, 4);
    check(rec_site >= 0, "install on the recursive function");
    {
        Seen rec_seen;
        std::memset(&rec_seen, 0, sizeof(rec_seen));
        a->set_handlers(rec_site, &on_pre, &on_post, &rec_seen);
        uint32_t self_value = 0;
        uint32_t delta = 0;
        const uint32_t got = call3(reinterpret_cast<const void*>(&rec_shape), &self_value, 5, 0, 0, &delta);
        check(got == 15 && delta == 8, "five-deep recursion unwinds to the right result");
        check(rec_seen.pre_calls == 6 && rec_seen.post_calls == 6, "every level ran pre and post");
        check(rec_seen.post_args[0] == 5 && rec_seen.last_post.result == 15,
            "the outermost post sees its own argument and result");
        a->clear_handlers(rec_site, 1000);
    }

    // cdecl: zero stack-arg bytes
    const int32_t cdecl_site = install(a, reinterpret_cast<const void*>(&cdecl_shape), 5, 8, 0);
    check(cdecl_site >= 0, "install on a cdecl function");
    {
        Seen c_seen;
        std::memset(&c_seen, 0, sizeof(c_seen));
        a->set_handlers(cdecl_site, &on_pre, &on_post, &c_seen);
        uint32_t self_value = 0;
        uint32_t delta = 0;
        const uint32_t got = call3(reinterpret_cast<const void*>(&cdecl_shape), &self_value, 50, 8, 0, &delta);
        check(got == 42 && delta == 12 && c_seen.pre_calls == 1 && c_seen.post_calls == 1
            && c_seen.pre_args[0] == 50 && c_seen.post_args[1] == 8,
            "cdecl: callee pops nothing, result, arguments and handlers intact");
        a->clear_handlers(cdecl_site, 1000);
    }

    // 5. two threads under install and handler swaps
    {
        Hammer h1;
        Hammer h2;
        std::memset(&h1, 0, sizeof(h1));
        std::memset(&h2, 0, sizeof(h2));
        h1.fn = h2.fn = reinterpret_cast<const void*>(&mul_shape);
        h1.self_value = 3;
        h2.self_value = 5;
        HANDLE t1 = CreateThread(NULL, 0, &hammer_thread, &h1, 0, NULL);
        HANDLE t2 = CreateThread(NULL, 0, &hammer_thread, &h2, 0, NULL);
        Sleep(5);
        const int32_t mul_site = install(a, reinterpret_cast<const void*>(&mul_shape), 5, 8);
        check(mul_site >= 0, "install while two threads are inside the target");
        Seen m_seen;
        std::memset(&m_seen, 0, sizeof(m_seen));
        bool drained = mul_site >= 0;
        for (int round = 0; round < 20 && mul_site >= 0; ++round) {
            a->set_handlers(mul_site, &on_pre, &on_post, &m_seen);
            Sleep(5);
            drained = drained && a->clear_handlers(mul_site, 5000) == HU_OK;
        }
        check(drained, "twenty set/clear rounds under load all drained");
        InterlockedExchange(&g_hammer_stop, 1);
        WaitForSingleObject(t1, INFINITE);
        WaitForSingleObject(t2, INFINITE);
        CloseHandle(t1);
        CloseHandle(t2);
        char hammer_label[96];
        std::snprintf(hammer_label, sizeof(hammer_label),
            "%lu hammered calls, every result right, stack balanced",
            static_cast<unsigned long>(h1.calls + h2.calls));
        check(h1.calls > 0 && h2.calls > 0 && h1.wrong == 0 && h2.wrong == 0
            && h1.imbalanced == 0 && h2.imbalanced == 0, hammer_label);
        char label[96];
        std::snprintf(label, sizeof(label), "handlers saw calls, pre and post in equal number (%ld / %ld)",
            static_cast<long>(m_seen.pre_calls), static_cast<long>(m_seen.post_calls));
        check(m_seen.pre_calls > 0 && m_seen.pre_calls == m_seen.post_calls, label);

        // drain: a handler asleep inside the site holds clear_handlers
        m_seen.sleep_ms = 300;
        a->set_handlers(mul_site, &on_pre, &on_post, &m_seen);
        OneCall one;
        one.fn = reinterpret_cast<const void*>(&mul_shape);
        one.self_value = 1;
        one.result = 0;
        HANDLE t3 = CreateThread(NULL, 0, &one_call_thread, &one, 0, NULL);
        Sleep(50);
        check(a->in_flight(mul_site) == 1, "in_flight counts the thread asleep in the handler");
        check(a->clear_handlers(mul_site, 20) == HU_E_BUSY, "clear_handlers times out while a handler runs");
        check(a->clear_handlers(mul_site, 5000) == HU_OK, "clear_handlers returns once the handler has left");
        WaitForSingleObject(t3, INFINITE);
        CloseHandle(t3);
        check(one.result == 7 && a->in_flight(mul_site) == 0, "the slow call still returned the right result");
    }

    // 6. idempotent install, site_info, displaced
    {
        const int32_t again = install(a, reinterpret_cast<const void*>(&open_shape), 7, 12);
        check(again == open_site, "install on the same target returns the same site");
        HuSiteInfo info;
        std::memset(&info, 0, sizeof(info));
        info.size = sizeof(info);
        check(b->site_info(open_site, &info) == HU_OK, "site_info through copy B");
        static const uint8_t open_prologue[7] = {0x8B, 0x44, 0x24, 0x04, 0x83, 0xEC, 0x20};
        check(std::memcmp(info.original, open_prologue, 7) == 0 && info.prologue_bytes == 7,
            "site_info reports the original prologue bytes");
        check(info.displaced == 0, "site_info: not displaced");
        check(info.trampoline != NULL && std::memcmp(info.trampoline, open_prologue, 7) == 0,
            "the trampoline starts with the relocated prologue");

        check(overwrite_code(reinterpret_cast<const void*>(&open_shape), open_prologue, 5), "restore the original bytes by hand");
        b->site_info(open_site, &info);
        check(info.displaced == 1, "site_info: displaced after the overwrite");
        const LONG pre_before = seen.pre_calls;
        uint32_t self_value = 0;
        uint32_t delta = 0;
        call3(reinterpret_cast<const void*>(&open_shape), &self_value, 1, 1, 1, &delta);
        check(seen.pre_calls == pre_before && delta == 0, "a displaced site runs the original unhooked");
        const int32_t re = install(a, reinterpret_cast<const void*>(&open_shape), 7, 12);
        b->site_info(open_site, &info);
        check(re == open_site && info.displaced == 0, "install re-applies the jump over restored bytes");
        call3(reinterpret_cast<const void*>(&open_shape), &self_value, 1, 1, 1, &delta);
        check(seen.pre_calls == pre_before + 1, "hooked again after the re-install");

        static const uint8_t foreign[5] = {0xE9, 0x00, 0x00, 0x00, 0x00};
        overwrite_code(reinterpret_cast<const void*>(&open_shape), foreign, 5);
        check(install(a, reinterpret_cast<const void*>(&open_shape), 7, 12) == HU_E_PATCHED,
            "install refuses a foreign patch");
        overwrite_code(reinterpret_cast<const void*>(&open_shape), open_prologue, 5);
        check(install(a, reinterpret_cast<const void*>(&open_shape), 7, 12) == open_site, "recovered after the foreign patch was removed");

        HuSiteDesc bad;
        bad.size = sizeof(bad);
        bad.target = reinterpret_cast<const void*>(&mul_shape);
        bad.prologue_bytes = 3;
        bad.stack_arg_bytes = 8;
        bad.callee_pops = 1;
        check(a->install(&bad) == HU_E_PROLOGUE, "install refuses a prologue under five bytes");
        bad.target = &seen;
        bad.prologue_bytes = 5;
        check(a->install(&bad) == HU_E_TARGET, "install refuses a target that is not code");
        check(a->set_handlers(999, &on_pre, NULL, NULL) == HU_E_SITE, "set_handlers refuses an unknown site");
    }

    // 7. FreeLibrary does not unmap a daemon
    {
        char path_a[MAX_PATH];
        char path_b[MAX_PATH];
        GetModuleFileNameA(module_a, path_a, MAX_PATH);
        GetModuleFileNameA(module_b, path_b, MAX_PATH);
        FreeLibrary(module_a);
        FreeLibrary(module_b);
        FreeLibrary(module_a);
        FreeLibrary(module_b);
        check(GetModuleHandleA(path_a) != NULL, "the winning copy stays mapped after FreeLibrary");
        check(GetModuleHandleA(path_b) != NULL, "the losing copy stays mapped after FreeLibrary");
        uint32_t self_value = 100;
        uint32_t delta = 0;
        const LONG pre_before = seen.pre_calls;
        const uint32_t got = call3(reinterpret_cast<const void*>(&open_shape), &self_value, 1, 2, 3, &delta);
        check(got == 106 && delta == 0 && seen.pre_calls == pre_before + 1, "sites still work after FreeLibrary");
        check(a->clear_handlers(open_site, 1000) == HU_OK, "clear_handlers on the open site");
    }

    std::printf("%d of %d checks failed\n", g_failures, g_checks);
    return g_failures;
}
