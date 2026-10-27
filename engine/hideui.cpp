// _HideUI - the hideui engine. Resolves FFXiMain by signature, hooks six of
// its menu routines, the ability opener, the compass draw, the macro key
// gate, the key test, the template swap, the per-menu and sprite draws and
// the draw primitive through hideui_daemon.dll, and serves hideui.lua.
//
// Every addon ships its own copy. The first copy to install in a client is
// the resident and publishes a table of its operations; every later copy
// forwards its Lua calls through that table (hideui_engine_abi.h). One image
// holds the hooks, one table holds every handle, and the Lua bindings at the
// bottom are the same code in both kinds of copy.
//
// Lua calls validate, queue and return; the game is changed only by the
// per-frame drain in game.h. The calls that answer from game memory directly
// (info, list, options, pending, ...) read from the Lua thread and guard
// every pointer they follow.
//
// Never make a Lua API call while holding a lock or the election mutex: a
// Lua error longjmps past the release. Lock order: the bind lock, the
// election mutex, the engine lock.

#include "signatures.h"

#include <math.h>
#include <stdlib.h>

extern "C" {
#if defined(_WIN32)
#define LUA_BUILD_AS_DLL
#endif
#include "lua.h"
#include "lauxlib.h"
}

#ifdef HU_BREADCRUMBS
extern "C" {
__declspec(dllexport) hu::Crumbs hideui_crumbs = {"hideui crumbs 1", 0, sizeof(hu::CrumbThread), {}};
}
#endif
#ifdef HU_HEAPCHECK
extern "C" {
__declspec(dllexport) hu::HeapCheck hideui_heapcheck;
}
#endif

// The table this copy publishes when resident. The tests build one copy that
// publishes engine abi 2's, to stand for an older resident.
#ifndef HU_ENGINE_ABI_BUILT
#define HU_ENGINE_ABI_BUILT HU_ENGINE_ABI
#endif
#ifndef HU_ENGINE_PUBLISHED_SIZE
#define HU_ENGINE_PUBLISHED_SIZE sizeof(HuEngineApi)
#endif

namespace {

using namespace hu;

const char kHandleType[] = "hideui.handle";
const uint32_t kClearWaitMs = 1000;
const DWORD kElectionWaitMs = 5000;
const int kMaxElements = 128;
const size_t kElementText = 128;
const uint32_t kAbi = HU_ENGINE_ABI_BUILT;
const size_t kWhyBytes = 512;

// kClosing: a shutdown has queued the restore and its thread waits for the
// game thread to run it; the hooks are still set.
enum State { kIdle = 0, kInstalled, kFailed, kClosing };

// What new() answers when the engine cannot install, as a player can act on
// it; status().detail.error has the failure itself.
const char kPlayerVersion[] = "hideui can't work with this version of FFXI; update the addon";
const char kPlayerRestart[] = "hideui can't work until FFXI is restarted";
const char kPlayerReinstall[] = "hideui can't work: a file it needs is missing; reinstall the addon";
const char kPlayerOlderDaemon[] =
    "hideui can't work: another addon loaded an older hideui first; update that addon and restart FFXI";
const char kPlayerOldEngine[] = "hideui can't work while an addon with hideui 0.1.0 is loaded; unload it";
const char kPlayerRetry[] = "hideui could not hook the game just now; try again";

const uint32_t kClaimMagic = 0x48554531u;

struct ClaimRecord {
    uint32_t magic;
    char path[MAX_PATH];
};

// Made on first use: nothing here may run at image load.
struct LazyLock {
    CRITICAL_SECTION section;
    volatile LONG state;

    void enter() {
        if (state != 2) {
            if (InterlockedCompareExchange(&state, 1, 0) == 0) {
                InitializeCriticalSection(&section);
                MemoryBarrier();
                state = 2;
            } else {
                while (state != 2) {
                    SwitchToThread();
                }
            }
        }
        EnterCriticalSection(&section);
    }

    void leave() { LeaveCriticalSection(&section); }
};

struct Runtime {
    int state;
    char failure[256];
    const char* player;             // the failure as new() reports it
    bool transient;                 // idle after a daemon error that can pass; the next new() retries
    uint32_t dropped;               // events every handle has lost
    bool inventory_ready;
    bool resolved;
    const HuDaemonApi* daemon;
    const uint8_t* targets[kSiteCount];
    uint8_t prologues[kSiteCount][HU_PROLOGUE_MAX];  // each site's first bytes, as resolved
    const uint8_t* calls[kCallCount];
    const uint8_t* quad;            // the quad routine, whose call names the draw primitive
    int32_t sites[kSiteCount];
    bool pinned_busy;
    HMODULE self_reference;
    volatile LONG closing;          // the generation of the latest shutdown's thread; an older one leaves
    uint32_t closing_target;        // where that shutdown's restore sits in the queue
    HANDLE claim_mapping;
    ClaimRecord* claim;
    HandleTable handles;
    LazyLock lock;
};

// A resident this copy forwards to: its table, copied out of the record, and
// one reference on its image. Kept, unmoved, until the last Lua state of this
// copy closes, so nothing made through it can reach an unmapped image.
struct Adopted {
    HMODULE module;
    HuEngineApi api;            // `size` is what the resident published, at most ours
    char path[MAX_PATH];
};

// The election. The mutex and the record view are kept for the life of the
// process, like the daemon's: the record outlives every image that reads it.
struct Binding {
    HMODULE self;
    HANDLE mutex;
    HANDLE mapping;
    HuEngineRecord* record;
    Adopted** adopted;
    int adopted_count;
    int adopted_capacity;
    int users;                  // Lua states that opened this image
    char refusal[kWhyBytes];
    LazyLock lock;
};

Engine g_engine;
Runtime g_rt;
Binding g_bind;
const char module_anchor_ = 'h';

void lock() {
    HU_STEP("engine lock");
    g_rt.lock.enter();
}

void unlock() {
    g_rt.lock.leave();
}

void bind_lock() {
    HU_STEP("bind lock");
    g_bind.lock.enter();
}

void bind_unlock() {
    g_bind.lock.leave();
}

// Only the first failure is kept: it is the one status() names.
bool fail(const char* why, const char* player) {
    if (g_rt.state != kFailed) {
        g_rt.state = kFailed;
        snprintf(g_rt.failure, sizeof(g_rt.failure), "%s", why);
        g_rt.player = player;
        g_rt.transient = false;
    }
    return false;
}

// A daemon error that can pass -- a thread inside a prologue, an allocation
// -- latches nothing: the engine stays idle and the next new() installs
// again. status() names it until then.
bool retry_later(const char* why) {
    snprintf(g_rt.failure, sizeof(g_rt.failure), "%s", why);
    g_rt.player = kPlayerRetry;
    g_rt.transient = true;
    return false;
}

bool daemon_error_passes(int32_t error) {
    return error == HU_E_THREADS || error == HU_E_MEMORY;
}


// ---------------------------------------------------------------------------
// This image, the daemon beside it, and the claim on the hooks.

// From an address inside the image, never by name: every addon ships its own
// copy of this basename. UNCHANGED_REFCOUNT, or each call leaks a reference.
HMODULE self_module(WCHAR* path, DWORD length) {
    HMODULE self = NULL;
    if (!GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS
                | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
            &module_anchor_, &self) || !self) {
        return NULL;
    }
    if (path) {
        const DWORD copied = GetModuleFileNameW(self, path, length);
        if (copied == 0 || copied >= length) {
            return NULL;
        }
    }
    return self;
}

bool path_beside_self(const WCHAR* name, WCHAR* out, DWORD length) {
    if (!self_module(out, length)) {
        return false;
    }
    DWORD cut = 0;
    for (DWORD i = 0; out[i] != L'\0'; ++i) {
        if (out[i] == L'\\' || out[i] == L'/') {
            cut = i + 1;
        }
    }
    DWORD i = 0;
    for (; name[i] != L'\0'; ++i) {
        if (cut + i + 1 >= length) {
            return false;
        }
        out[cut + i] = name[i];
    }
    out[cut + i] = L'\0';
    return true;
}

void narrow(const WCHAR* wide, char* out, int size) {
    if (WideCharToMultiByte(CP_UTF8, 0, wide, -1, out, size, NULL, NULL) == 0) {
        out[0] = '\0';
    }
}

// What the election published, for the two refusals acquire() can make.
bool read_daemon_record(HuDaemonRecord* out) {
    char name[HU_NAME_MAX];
    snprintf(name, sizeof(name), HU_MAPPING_NAME_FORMAT, static_cast<unsigned>(GetCurrentProcessId()));
    HANDLE mapping = OpenFileMappingA(FILE_MAP_READ, FALSE, name);
    if (!mapping) {
        return false;
    }
    const void* view = MapViewOfFile(mapping, FILE_MAP_READ, 0, 0, HU_MAPPING_BYTES);
    bool ok = false;
    if (view) {
        const HuDaemonRecord* record = static_cast<const HuDaemonRecord*>(view);
        if (record->magic == HU_MAGIC && record->record_size >= offsetof(HuDaemonRecord, api)) {
            memcpy(out, record, offsetof(HuDaemonRecord, api));
            out->winner_path[MAX_PATH - 1] = '\0';
            out->winner_build[HU_BUILD_MAX - 1] = '\0';
            ok = true;
        }
        UnmapViewOfFile(view);
    }
    CloseHandle(mapping);
    return ok;
}

// Never from DllMain: the election takes a lock and may LoadLibrary.
bool acquire_daemon(char* why, size_t size, const char** player) {
    if (g_rt.daemon) {
        return true;
    }
    *player = kPlayerReinstall;
    WCHAR path[MAX_PATH];
    if (!path_beside_self(L"hideui_daemon.dll", path, MAX_PATH)) {
        snprintf(why, size, "daemon: this image could not resolve its own path");
        return false;
    }
    char shown[MAX_PATH * 3];
    narrow(path, shown, sizeof(shown));
    const HMODULE module = LoadLibraryW(path);
    if (!module) {
        snprintf(why, size, "daemon: hideui_daemon.dll is missing beside _HideUI.dll (%s)", shown);
        return false;
    }
    const HuDaemonAcquire acquire = reinterpret_cast<HuDaemonAcquire>(
        reinterpret_cast<void (*)()>(GetProcAddress(module, HU_DAEMON_ACQUIRE_NAME)));
    if (!acquire) {
        snprintf(why, size, "daemon: %s is not the hideui daemon", shown);
        return false;
    }
    *player = kPlayerRestart;
    const HuDaemonApi* api = acquire(HU_DAEMON_ABI);
    if (!api || api->size < sizeof(HuDaemonApi) || !api->install || !api->set_handlers
        || !api->clear_handlers || !api->site_info) {
        HuDaemonRecord record;
        if (read_daemon_record(&record) && record.abi_version < HU_DAEMON_ABI) {
            *player = kPlayerOlderDaemon;
            snprintf(why, size,
                "daemon: an older hideui_daemon.dll (abi %u) is resident from %s;"
                " exit FFXI and replace it", static_cast<unsigned>(record.abi_version),
                record.winner_path);
        } else {
            snprintf(why, size, "daemon: acquire refused (the daemon could not pin itself)");
        }
        return false;
    }
    g_rt.daemon = api;
    return true;
}

// The claim engines from before forwarding (0.1.0) check. The daemon holds
// one handler set per site, so a second image setting handlers would take the
// hooks from the first; the resident holds this claim too, so neither kind
// ever does.
bool claim_engine(char* why, size_t size) {
    char name[64];
    snprintf(name, sizeof(name), "Local\\hideui_engine_v1_%08X",
        static_cast<unsigned>(GetCurrentProcessId()));
    HANDLE mapping = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0,
        sizeof(ClaimRecord), name);
    const DWORD error = GetLastError();
    if (!mapping) {
        snprintf(why, size, "claim: could not create %s (%lu)", name, error);
        return false;
    }
    ClaimRecord* record = static_cast<ClaimRecord*>(
        MapViewOfFile(mapping, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(ClaimRecord)));
    if (!record) {
        CloseHandle(mapping);
        snprintf(why, size, "claim: could not map %s", name);
        return false;
    }
    if (error == ERROR_ALREADY_EXISTS) {
        char owner[MAX_PATH];
        if (record->magic == kClaimMagic) {
            memcpy(owner, record->path, MAX_PATH);
            owner[MAX_PATH - 1] = '\0';
        } else {
            snprintf(owner, sizeof(owner), "(an engine still starting)");
        }
        UnmapViewOfFile(record);
        CloseHandle(mapping);
        snprintf(why, size, "claim: another hideui engine holds the hooks in this client: %s"
            " (one that does not forward; unload it)", owner);
        return false;
    }
    WCHAR path[MAX_PATH];
    if (self_module(path, MAX_PATH)) {
        narrow(path, record->path, MAX_PATH);
    }
    MemoryBarrier();
    record->magic = kClaimMagic;
    g_rt.claim_mapping = mapping;
    g_rt.claim = record;
    return true;
}

void release_claim() {
    if (!g_rt.claim) {
        return;
    }
    g_rt.claim->magic = 0;
    UnmapViewOfFile(g_rt.claim);
    CloseHandle(g_rt.claim_mapping);
    g_rt.claim = NULL;
    g_rt.claim_mapping = NULL;
}

// One reference on this image while handlers are set, so the daemon can never
// call into an unmapped image. Released only past a drain that succeeded.
bool take_self_reference() {
    if (g_rt.self_reference) {
        return true;
    }
    WCHAR path[MAX_PATH];
    if (!self_module(path, MAX_PATH)) {
        return false;
    }
    g_rt.self_reference = LoadLibraryW(path);
    return g_rt.self_reference != NULL;
}

void release_self_reference() {
    if (!g_rt.self_reference) {
        return;
    }
    const HMODULE module = g_rt.self_reference;
    g_rt.self_reference = NULL;
    FreeLibrary(module);
}

// This image's base. Cached: it cannot change while the image is mapped.
const void* self_base() {
    if (!g_bind.self) {
        g_bind.self = self_module(NULL, 0);
    }
    return g_bind.self;
}

bool published_as_self() {
    const HuEngineRecord* rec = g_bind.record;
    return rec && self_base() && rec->magic == HU_ENGINE_MAGIC && rec->module == self_base();
}

// Under the election mutex, once the handlers are out: the next copy to
// elect finds no resident and installs itself.
void retire_record() {
    if (published_as_self()) {
        MemoryBarrier();
        g_bind.record->magic = 0;
    }
}

// ---------------------------------------------------------------------------
// Resolution. Every signature must match exactly once in the mapped image.

bool text_section(const uint8_t** out, size_t* size) {
    const HMODULE module = GetModuleHandleA("FFXiMain.dll");
    if (!module) {
        return false;
    }
    const uint8_t* base = reinterpret_cast<const uint8_t*>(module);
    const IMAGE_DOS_HEADER* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(base);
    if (!readable(dos, sizeof(*dos)) || dos->e_magic != IMAGE_DOS_SIGNATURE) {
        return false;
    }
    const IMAGE_NT_HEADERS32* nt = reinterpret_cast<const IMAGE_NT_HEADERS32*>(base + dos->e_lfanew);
    if (!readable(nt, sizeof(*nt)) || nt->Signature != IMAGE_NT_SIGNATURE) {
        return false;
    }
    const IMAGE_SECTION_HEADER* section = IMAGE_FIRST_SECTION(nt);
    for (unsigned i = 0; i < nt->FileHeader.NumberOfSections; ++i, ++section) {
        if (memcmp(section->Name, ".text", 5) != 0) {
            continue;
        }
        *out = base + section->VirtualAddress;
        *size = section->Misc.VirtualSize ? section->Misc.VirtualSize : section->SizeOfRawData;
        return readable(*out, *size);
    }
    return false;
}

struct Sig {
    uint8_t bytes[64];
    char mask[65];
    size_t length;
};

const uint8_t* find_one(const uint8_t* text, size_t size, const char* name, const char* signature,
                        char* why, size_t len) {
    Sig s;
    s.length = parse_signature(signature, s.bytes, s.mask, sizeof(s.bytes));
    if (!s.length) {
        snprintf(why, len, "signature %s: malformed", name);
        return NULL;
    }
    ScanResult r;
    scan(text, size, s.bytes, s.mask, false, &r);
    if (r.count != 1) {
        if (r.count == 0) {
            snprintf(why, len, "signature %s: not found (client patched?)", name);
        } else {
            snprintf(why, len, "signature %s: matched %d times", name, r.count);
        }
        return NULL;
    }
    return r.hits[0];
}

// The routine the call at `at` in a hit reaches: the rel32 after its e8.
const uint8_t* call_target(const uint8_t* hit, size_t at) {
    int32_t rel;
    memcpy(&rel, hit + at + 1, 4);
    return hit + at + 5 + rel;
}

// How often `bytes` occur inside the first `span` bytes from `at`, the
// text's end cutting the span short; the last one found in `*last`.
int count_within(const uint8_t* text, size_t size, const uint8_t* at, size_t span, const uint8_t* bytes,
                 size_t length, const uint8_t** last) {
    const size_t left = static_cast<size_t>(text + size - at);
    const size_t end = left < span ? left : span;
    int found = 0;
    for (size_t i = 0; i + length <= end; ++i) {
        if (memcmp(at + i, bytes, length) == 0) {
            *last = at + i;
            ++found;
        }
    }
    return found;
}

// The global holding window nm's controller: its registry row's slot.
uint8_t** registry_slot(const char* nm) {
    const int n = g_engine.inv.find_exact(nm);
    return n >= 0 && g_engine.inv.names[n].row_count ? g_engine.game.slot[g_engine.inv.names[n].rows[0]] : NULL;
}

bool in_text(const uint8_t* p, const uint8_t* text, size_t size) {
    return p >= text && p < text + size;
}

// The yes/no class's confirm keeps its question where the replies read and
// write it; the KO window's confirm asks through comyn's controller with the
// class's ask and a callback that marks the KO window's controller, and its
// tick keeps the countdown options('dead') reads. Any of them otherwise
// fails the install.
bool resolve_prompts(const uint8_t* text, size_t size, char* why, size_t len) {
    const uint8_t* confirm = find_one(text, size, "yesno_confirm", kSigYesNoConfirm, why, len);
    if (!confirm) {
        return false;
    }
    const struct {
        const uint8_t* bytes;
        size_t length;
        const char* what;
    } fields[] = {
        {kConfirmChainedRead, sizeof(kConfirmChainedRead), "read of the chained flag +0x3A"},
        {kConfirmCallbackClear, sizeof(kConfirmCallbackClear), "clear of the callback +0x14"},
        {kConfirmChainedDefault, sizeof(kConfirmChainedDefault), "read of the chained default +0x3B"},
    };
    for (size_t i = 0; i < sizeof(fields) / sizeof(fields[0]); ++i) {
        const uint8_t* at = NULL;
        const int found = count_within(text, size, confirm, kYesNoConfirmBytes, fields[i].bytes, fields[i].length,
            &at);
        if (found != 1) {
            snprintf(why, len, "yesno_confirm: its %s is %s", fields[i].what,
                found ? "there more than once" : "not there (client patched?)");
            return false;
        }
    }

    const uint8_t* hit = find_one(text, size, "dead_confirm", kSigDeadConfirm, why, len);
    if (!hit) {
        return false;
    }
    uint8_t** comyn = registry_slot("comyn");
    if (rdptr(hit, kDeadAskerGlobal) != reinterpret_cast<const uint8_t*>(comyn)) {
        snprintf(why, len, "dead_confirm: its asker's global 0x%08X is not the registry's comyn slot 0x%08X",
            static_cast<unsigned>(rd32(hit, kDeadAskerGlobal)),
            static_cast<unsigned>(reinterpret_cast<uintptr_t>(comyn)));
        return false;
    }
    const uint8_t* decider = call_target(hit, kDeadDeciderCall);
    const uint8_t* message = call_target(hit, kDeadMessageCall);
    const uint8_t* ask = call_target(hit, kDeadAskCall);
    const uint8_t* callback = rdptr(hit, kDeadAskCallback);
    if (!in_text(decider, text, size) || !in_text(message, text, size) || !in_text(ask, text, size)
        || !in_text(callback, text, size)) {
        snprintf(why, len, "dead_confirm: a routine it calls or names is not in FFXiMain's code");
        return false;
    }
    if (!readable(ask, sizeof(kYesNoAskHead)) || memcmp(ask, kYesNoAskHead, sizeof(kYesNoAskHead)) != 0) {
        snprintf(why, len, "dead_confirm: the ask it calls, 0x%08X, is not the yes/no class's (client patched?)",
            static_cast<unsigned>(reinterpret_cast<uintptr_t>(ask)));
        return false;
    }
    uint8_t** dead = registry_slot("dead");
    const size_t head = sizeof(kDeadChosenHead);
    if (!readable(callback, head + 4 + sizeof(kDeadChosenTail)) || memcmp(callback, kDeadChosenHead, head) != 0
        || rdptr(callback, head) != reinterpret_cast<const uint8_t*>(dead)
        || memcmp(callback + head + 4, kDeadChosenTail, sizeof(kDeadChosenTail)) != 0) {
        snprintf(why, len, "dead_confirm: its callback 0x%08X does not mark the home point chosen in dead's"
            " controller (client patched?)", static_cast<unsigned>(rd32(hit, kDeadAskCallback)));
        return false;
    }
    const uint8_t* tick = find_one(text, size, "dead_tick", kSigDeadTick, why, len);
    if (!tick) {
        return false;
    }
    if (call_target(tick, kDeadTickDeciderCall) != decider) {
        snprintf(why, len, "dead_tick: its decider is not the one the KO window's confirm calls");
        return false;
    }

    Game& g = g_engine.game;
    g.yesno_confirm = confirm;
    g.dead_tick = tick;
    memcpy(&g.yesno_ask, &ask, sizeof(g.yesno_ask));
    DeadConfirm& d = g.dead;
    memcpy(&d.decider, &decider, sizeof(d.decider));
    d.decider_first = static_cast<int8_t>(hit[kDeadDeciderFirst]);
    d.decider_second = static_cast<int8_t>(hit[kDeadDeciderSecond]);
    memcpy(&d.message, &message, sizeof(d.message));
    d.category = static_cast<int8_t>(hit[kDeadCategory]);
    d.text_if_set = static_cast<int32_t>(rd32(hit, kDeadTextIfSet));
    d.text_otherwise = static_cast<int8_t>(hit[kDeadTextOtherwise]);
    d.callback = callback;
    d.context = static_cast<int8_t>(hit[kDeadAskContext]);
    d.default_yes = static_cast<int8_t>(hit[kDeadAskDefault]);
    d.help = static_cast<int8_t>(hit[kDeadAskHelp]);
    d.cancellable = static_cast<int8_t>(hit[kDeadAskCancellable]);
    d.mode = static_cast<int8_t>(hit[kDeadAskMode]);
    return true;
}

bool resolve_plain(const uint8_t* text, size_t size, char* why, size_t len) {
    const uint8_t* hit = find_one(text, size, "menu_registry", kSigRegistry, why, len);
    if (!hit) {
        return false;
    }
    if (rd32(hit, 1) != rd32(hit, 19)) {
        snprintf(why, len, "signature menu_registry: its two table addresses disagree");
        return false;
    }
    uint8_t* table = rdptr(hit, 1);
    if (!readable(table, (kRowCount + 1) * kRowStride)) {
        snprintf(why, len, "menu table at 0x%08X is not readable", static_cast<unsigned>(rd32(hit, 1)));
        return false;
    }
    if (!verify_registry(table, kRowCount + 1, why, len)) {
        return false;
    }
    for (int r = 0; r < kRowCount; ++r) {
        uint8_t** slot = reinterpret_cast<uint8_t**>(rdptr(table + r * kRowStride, kRowSlot));
        // One row (hnbackwi) carries no controller slot at all; it has no
        // controller to find, not an unreadable one.
        if (!slot) {
            g_engine.game.slot[r] = NULL;
            continue;
        }
        if (!readable(slot, 4)) {
            snprintf(why, len, "menu table row %d (%s): controller slot is not readable",
                r, kRowSpecs[r].name);
            return false;
        }
        g_engine.game.slot[r] = slot;
    }

    hit = find_one(text, size, "menu_manager", kSigManager, why, len);
    if (!hit) {
        return false;
    }
    uint8_t* mcb = rdptr(hit, 5);
    if (!readable(mcb, kMcbBytes)) {
        snprintf(why, len, "menu manager at 0x%08X is not readable", static_cast<unsigned>(rd32(hit, 5)));
        return false;
    }

    hit = find_one(text, size, "dock_masks", kSigDockMasks, why, len);
    if (!hit) {
        return false;
    }
    const uint8_t* masks = rdptr(hit, 3);
    if (!readable(masks, sizeof(kDockMaskTable)) || memcmp(masks, kDockMaskTable, sizeof(kDockMaskTable)) != 0) {
        snprintf(why, len, "dock mask table is not 1000 2000 4000 10000000 0");
        return false;
    }

    const uint8_t* set_position = find_one(text, size, "set_position", kSigSetPosition, why, len);
    if (!set_position) {
        return false;
    }
    const uint8_t* close = find_one(text, size, "close_by_name", kSigCloseByName, why, len);
    if (!close) {
        return false;
    }
    const uint8_t* set_cursor = find_one(text, size, "set_cursor", kSigSetCursor, why, len);
    if (!set_cursor) {
        return false;
    }
    const uint8_t* glyph_convert = find_one(text, size, "glyph_convert", kSigGlyphConvert, why, len);
    if (!glyph_convert) {
        return false;
    }
    // The mouse block leans on the hit test's own miss for a menu whose
    // kMenuMouse byte is 0; without that read a hidden menu would take clicks.
    const uint8_t* hit_test = find_one(text, size, "row_hit_test", kSigRowHitTest, why, len);
    if (!hit_test) {
        return false;
    }
    const uint8_t* mouse_read = NULL;
    const int reads = count_within(text, size, hit_test, kHitTestBytes, kHitTestMouseRead, sizeof(kHitTestMouseRead),
        &mouse_read);
    if (reads != 1) {
        snprintf(why, len, "row_hit_test: its test of the menu's mouse byte +0x%02X is %s",
            static_cast<unsigned>(kMenuMouse), reads ? "there more than once" : "not there (client patched?)");
        return false;
    }

    const uint8_t* routing = find_one(text, size, "menu_routing", kSigMenuRouting, why, len);
    if (!routing) {
        return false;
    }

    for (int i = 0; i < kCallCount; ++i) {
        g_rt.calls[i] = find_one(text, size, kCalls[i].name, kCalls[i].signature, why, len);
        if (!g_rt.calls[i]) {
            return false;
        }
    }
    uint8_t* pending = rdptr(g_rt.calls[kCallPartyClear], kPartyClearFlag);
    if (!readable(pending, 1)) {
        snprintf(why, len, "prtyjoin pending flag at 0x%08X is not readable",
            static_cast<unsigned>(rd32(g_rt.calls[kCallPartyClear], kPartyClearFlag)));
        return false;
    }
    hit = find_one(text, size, "link5_cache", kSigLink5Cache, why, len);
    if (!hit) {
        return false;
    }
    uint8_t* cache = rdptr(hit, kLink5CacheImm);
    if (!readable(cache, kLink5Slots * kLink5Entry)) {
        snprintf(why, len, "link5 concierge cache at 0x%08X is not readable",
            static_cast<unsigned>(rd32(hit, kLink5CacheImm)));
        return false;
    }
    hit = find_one(text, size, "query_cancel_allowed", kSigQueryCancelAllowed, why, len);
    if (!hit) {
        return false;
    }
    const uint8_t* cancel_allowed = rdptr(hit, kQueryCancelImm);
    if (!readable(cancel_allowed, 1)) {
        snprintf(why, len, "query cancel-allowed byte at 0x%08X is not readable",
            static_cast<unsigned>(rd32(hit, kQueryCancelImm)));
        return false;
    }
    hit = find_one(text, size, "link5_latch", kSigLink5Latch, why, len);
    if (!hit) {
        return false;
    }
    const uint8_t* link5_latch = rdptr(hit, kLink5LatchImm);
    if (!readable(link5_latch, 1)) {
        snprintf(why, len, "link5 pending latch at 0x%08X is not readable",
            static_cast<unsigned>(rd32(hit, kLink5LatchImm)));
        return false;
    }
    hit = find_one(text, size, "macro_object", kSigMacroObject, why, len);
    if (!hit) {
        return false;
    }
    if (rd32(hit, kMacroObjectImm) != rd32(hit, kMacroObjectImm2)) {
        snprintf(why, len, "macro_object: its two globals disagree");
        return false;
    }
    uint8_t** macro_object = reinterpret_cast<uint8_t**>(rdptr(hit, kMacroObjectImm));
    if (!readable(macro_object, 4)) {
        snprintf(why, len, "macro object global at 0x%08X is not readable",
            static_cast<unsigned>(rd32(hit, kMacroObjectImm)));
        return false;
    }
    hit = find_one(text, size, "link5_open", kSigLink5Open, why, len);
    if (!hit) {
        return false;
    }
    // The handler opens link5 on the controller in this global; the registry
    // row must name the same one, or the pending test reads another menu.
    uint8_t** link5_slot = registry_slot("link5");
    if (rdptr(hit, kLink5OpenGlobal) != reinterpret_cast<const uint8_t*>(link5_slot)) {
        snprintf(why, len, "link5 open site: its controller global 0x%08X is not the registry's link5 slot 0x%08X",
            static_cast<unsigned>(rd32(hit, kLink5OpenGlobal)),
            static_cast<unsigned>(reinterpret_cast<uintptr_t>(link5_slot)));
        return false;
    }
    const uint8_t* link5_callback = rdptr(hit, kLink5OpenCallback);
    if (link5_callback < text || link5_callback >= text + size) {
        snprintf(why, len, "link5 open site: its callback 0x%08X is not in FFXiMain's code",
            static_cast<unsigned>(rd32(hit, kLink5OpenCallback)));
        return false;
    }
    hit = find_one(text, size, "yesno_class", kSigYesNoClass, why, len);
    if (!hit) {
        return false;
    }
    const uint8_t* yesno_vtable = rdptr(hit, kYesNoClassVtable);
    if (!readable(yesno_vtable, 4) || (yesno_vtable >= text && yesno_vtable < text + size)) {
        snprintf(why, len, "yes/no class: its vtable 0x%08X is not readable data",
            static_cast<unsigned>(rd32(hit, kYesNoClassVtable)));
        return false;
    }
    // cancel() refuses a question whose cancellable byte is clear: the
    // class's own cancel must test that byte before it calls back.
    const uint8_t* yesno_input = readable(yesno_vtable, (kYesNoInputSlot + 1) * 4)
        ? rdptr(yesno_vtable, kYesNoInputSlot * 4) : NULL;
    const uint8_t* yesno_cancel = NULL;
    const int tests = in_text(yesno_input, text, size) ? count_within(text, size, yesno_input, kYesNoInputBytes,
        kYesNoCancelTest, sizeof(kYesNoCancelTest), &yesno_cancel) : 0;
    if (tests != 1 || !readable(yesno_cancel + kYesNoCancelCall, sizeof(kYesNoCancelCallback))
        || memcmp(yesno_cancel + kYesNoCancelCall, kYesNoCancelCallback, sizeof(kYesNoCancelCallback)) != 0) {
        snprintf(why, len, "yes/no class: its cancel's test of the cancellable byte +0x%02X is %s",
            static_cast<unsigned>(kYesNoCancellable), tests > 1 ? "there more than once" : "not there (client patched?)");
        return false;
    }
    if (!resolve_prompts(text, size, why, len)) {
        return false;
    }

    Game& g = g_engine.game;
    g.registry = table;
    g.mcb = mcb;
    memcpy(&g.set_position, &set_position, sizeof(set_position));
    memcpy(&g.close, &close, sizeof(close));
    memcpy(&g.passinpu_reset, &g_rt.calls[kCallPassinpuReset], sizeof(g.passinpu_reset));
    memcpy(&g.party_send, &g_rt.calls[kCallPartySend], sizeof(g.party_send));
    memcpy(&g.party_clear, &g_rt.calls[kCallPartyClear], sizeof(g.party_clear));
    g.party_pending = pending;
    memcpy(&g.link5_clear, &g_rt.calls[kCallLink5Clear], sizeof(g.link5_clear));
    g.link5_cache = cache;
    g.link5_latch = link5_latch;
    g.link5_callback = link5_callback;
    g.query_cancel_allowed = cancel_allowed;
    memcpy(&g.arealist_close, &g_rt.calls[kCallArealistClose], sizeof(g.arealist_close));
    memcpy(&g.arealist_latch, &g_rt.calls[kCallArealistLatch], sizeof(g.arealist_latch));
    memcpy(&g.post_request, &g_rt.calls[kCallPostRequest], sizeof(g.post_request));
    memcpy(&g.post_request_close, &g_rt.calls[kCallPostRequestClose], sizeof(g.post_request_close));
    memcpy(&g.dock_reset, &g_rt.calls[kCallDockReset], sizeof(g.dock_reset));
    memcpy(&g.set_frame_rect, &g_rt.calls[kCallSetFrameRect], sizeof(g.set_frame_rect));
    memcpy(&g.set_cursor, &set_cursor, sizeof(g.set_cursor));
    memcpy(&g.glyph_convert, &glyph_convert, sizeof(g.glyph_convert));
    g.row_hit_test = hit_test;
    g.mouse_read = mouse_read;
    g.menu_routing = routing;
    g.macro_object = macro_object;
    g.yesno_vtable = yesno_vtable;
    g.yesno_cancel = yesno_cancel;
    memcpy(&g.select_row, &g_rt.calls[kCallSelectRow], sizeof(g.select_row));
    return true;
}

// The number boxes' confirms and setups, and every asker's callback; after
// the sites, for the open routine each setup must call. Each confirm closes
// through the manager; each setup opens its own key through the manager
// after writing the question; each asker's globals are the registry slots
// of its box and its own window, and a site's call reaches its box's setup.
bool resolve_boxes(const uint8_t* text, size_t size, char* why, size_t len) {
    static const char* const confirm_names[kBoxCount] = {"moneyctr_confirm", "itemctrl_confirm"};
    static const char* const confirm_sigs[kBoxCount] = {kSigMoneyConfirm, kSigItemConfirm};
    static const char* const setup_names[kBoxCount] = {"moneyctr_setup", "itemctrl_setup"};
    static const char* const setup_sigs[kBoxCount] = {kSigMoneySetup, kSigItemSetup};
    const uint8_t* open = g_rt.targets[kSiteOpen];
    const uint8_t* confirm[kBoxCount];
    const uint8_t* setup[kBoxCount];
    for (int b = 0; b < kBoxCount; ++b) {
        confirm[b] = find_one(text, size, confirm_names[b], confirm_sigs[b], why, len);
        if (!confirm[b]) {
            return false;
        }
        if (rdptr(confirm[b], kBoxConfirmManager[b]) != g_engine.game.mcb) {
            snprintf(why, len, "%s: its close does not go through the menu manager", confirm_names[b]);
            return false;
        }
        setup[b] = find_one(text, size, setup_names[b], setup_sigs[b], why, len);
        if (!setup[b]) {
            return false;
        }
        const uint8_t* key = rdptr(setup[b], kBoxSetupKey[b]);
        const int n = g_engine.inv.box_window[b];
        if (!readable(key, kKeyLen) || memcmp(key, g_engine.inv.names[n].key, kKeyLen) != 0) {
            snprintf(why, len, "%s: its open does not name the registry's %s key", setup_names[b], kBoxNames[b]);
            return false;
        }
        if (rdptr(setup[b], kBoxSetupManager[b]) != g_engine.game.mcb || setup[b][kBoxSetupOpen[b]] != 0xE8
            || call_target(setup[b], kBoxSetupOpen[b]) != open) {
            snprintf(why, len, "%s: it does not open through the menu manager's open routine", setup_names[b]);
            return false;
        }
    }
    if (memcmp(setup[kBoxMoney] + kMoneySetupTailAt, kMoneySetupTail, sizeof(kMoneySetupTail)) != 0) {
        snprintf(why, len, "moneyctr_setup: its writes of the max, the context and the extra callback before its"
            " open are not there (client patched?)");
        return false;
    }
    const void* callbacks[kAskerCount];
    for (int i = 0; i < kAskerCount; ++i) {
        const AskerSite& site = kAskerSites[i];
        const uint8_t* hit = find_one(text, size, site.name, site.signature, why, len);
        if (!hit) {
            return false;
        }
        const AskerSpec& a = kAskers[i];
        const uint8_t* callback = site.callback ? rdptr(hit, site.callback) : hit;
        if (!in_text(callback, text, size)) {
            snprintf(why, len, "%s: its callback 0x%08X is not in FFXiMain's code", site.name,
                static_cast<unsigned>(reinterpret_cast<uintptr_t>(callback)));
            return false;
        }
        if (site.setup_call && (hit[site.setup_call] != 0xE8 || call_target(hit, site.setup_call) != setup[a.box])) {
            snprintf(why, len, "%s: its call does not reach %s's setup", site.name, kBoxNames[a.box]);
            return false;
        }
        if (site.box_global
            && rdptr(hit, site.box_global) != reinterpret_cast<const uint8_t*>(registry_slot(kBoxNames[a.box]))) {
            snprintf(why, len, "%s: its box global is not the registry's %s slot", site.name, kBoxNames[a.box]);
            return false;
        }
        if (site.owner_global
            && rdptr(hit, site.owner_global) != reinterpret_cast<const uint8_t*>(registry_slot(a.owner))) {
            snprintf(why, len, "%s: its global is not the registry's %s slot", site.name, a.owner);
            return false;
        }
        callbacks[i] = callback;
    }
    for (int i = 0; i < kAskerCount; ++i) {
        for (int k = 0; k < i; ++k) {
            if (callbacks[i] == callbacks[k]) {
                snprintf(why, len, "%s and %s ask with one callback", kAskerSites[k].name, kAskerSites[i].name);
                return false;
            }
        }
    }
    Game& g = g_engine.game;
    for (int b = 0; b < kBoxCount; ++b) {
        memcpy(&g.box_confirm[b], &confirm[b], sizeof(g.box_confirm[b]));
        g.box_setup[b] = setup[b];
    }
    memcpy(g.askers, callbacks, sizeof(g.askers));
    return true;
}

// FFXiMain's read-only data sections, at most kDataSections of them: where
// its vtables are.
bool data_sections(AskedInputs* in) {
    const HMODULE module = GetModuleHandleA("FFXiMain.dll");
    const uint8_t* base = reinterpret_cast<const uint8_t*>(module);
    const IMAGE_DOS_HEADER* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(base);
    if (!module || !readable(dos, sizeof(*dos)) || dos->e_magic != IMAGE_DOS_SIGNATURE) {
        return false;
    }
    const IMAGE_NT_HEADERS32* nt = reinterpret_cast<const IMAGE_NT_HEADERS32*>(base + dos->e_lfanew);
    if (!readable(nt, sizeof(*nt)) || nt->Signature != IMAGE_NT_SIGNATURE) {
        return false;
    }
    in->data_count = 0;
    const IMAGE_SECTION_HEADER* section = IMAGE_FIRST_SECTION(nt);
    for (unsigned i = 0; i < nt->FileHeader.NumberOfSections; ++i, ++section) {
        const DWORD flags = section->Characteristics;
        const DWORD bytes = section->Misc.VirtualSize ? section->Misc.VirtualSize : section->SizeOfRawData;
        if (!(flags & IMAGE_SCN_MEM_READ) || (flags & (IMAGE_SCN_MEM_WRITE | IMAGE_SCN_MEM_EXECUTE))
            || in->data_count >= kDataSections || !readable(base + section->VirtualAddress, bytes)) {
            continue;
        }
        in->data[in->data_count] = base + section->VirtualAddress;
        in->data_size[in->data_count] = bytes;
        ++in->data_count;
    }
    return in->data_count > 0;
}

// The Asked windows', the dialogue boxes', the text windows' and the offer
// windows' routines and data, the openers of the sets and the magic list,
// and the routes of the windows open takes an argument for
// (signatures.h's resolve_asked(), resolve_dialogue(), resolve_text(),
// resolve_offer(), resolve_openers() and resolve_routes()), after the
// sites for the open routine and the key test; close by instance is the
// call close by name ends in.
bool resolve_asked_windows(const uint8_t* text, size_t size, char* why, size_t len) {
    const uint8_t* close;
    memcpy(&close, &g_engine.game.close, sizeof(close));
    const uint8_t* close_instance = close[kCloseByNameInstanceCall] == 0xE8
        ? call_target(close, kCloseByNameInstanceCall) : NULL;
    if (!in_text(close_instance, text, size)) {
        snprintf(why, len, "close_by_name: its call of close by instance is not in FFXiMain's code");
        return false;
    }
    AskedInputs in;
    memset(&in, 0, sizeof(in));
    if (!data_sections(&in)) {
        snprintf(why, len, "FFXiMain.dll has no readable read-only data section");
        return false;
    }
    in.text = text;
    in.size = size;
    in.inv = &g_engine.inv;
    in.registry = g_engine.game.registry;
    in.mcb = g_engine.game.mcb;
    in.open = g_rt.targets[kSiteOpen];
    in.close = close;
    memcpy(&in.message, &g_engine.game.dead.message, sizeof(in.message));
    in.key_test = g_rt.targets[kSiteKeyTest];
    in.cancel_allowed = g_engine.game.query_cancel_allowed;
    if (!resolve_asked(in, close_instance, &g_engine.game.asked, why, len)
        || !resolve_dialogue(in, close_instance, &g_engine.game.dialogue, why, len)
        || !resolve_text(in, close_instance, &g_engine.game.text, why, len)
        || !resolve_offer(in, close_instance, g_engine.game.box_setup[kBoxMoney], g_engine.game.askers,
            &g_engine.game.offer, why, len)
        || !resolve_openers(in, &g_engine.game.openers, why, len)
        || !resolve_routes(in, g_engine.game.offer, g_engine.game.openers.item_type, close_instance,
            reinterpret_cast<const uint8_t*>(g_engine.game.yesno_ask), &g_engine.game.routes, why, len)
        || !resolve_settings(in, g_engine.game.asked.bank_data, &g_engine.game.settings, why, len)) {
        return false;
    }
    memcpy(&g_engine.game.close_instance, &close_instance, sizeof(g_engine.game.close_instance));
    return true;
}

// The daemon's site `id` has this build's shape: the prologue it relocated
// and the bytes it saved, the stack it copies, the callee popping it. The
// daemon returns an existing site for a target whatever shape was asked
// for, and these handlers can run only behind the shape they were built for.
bool site_matches(int32_t id, const SiteSpec& spec, const uint8_t* prologue, HuSiteInfo* info) {
    memset(info, 0, sizeof(*info));
    info->size = sizeof(*info);
    if (g_rt.daemon->site_info(id, info) != HU_OK) {
        return false;
    }
    return info->prologue_bytes == spec.prologue && info->stack_arg_bytes == spec.arg_bytes
        && info->callee_pops == spec.callee_pops && memcmp(info->original, prologue, spec.prologue) == 0;
}

// A candidate whose first bytes are already a jump is ours only if the daemon
// says it patched exactly this prologue there. install() on such a target
// never writes: it returns the existing site or refuses.
bool daemon_owns(const uint8_t* at, const SiteSpec& spec, const Sig& s) {
    HuSiteDesc desc;
    desc.size = sizeof(desc);
    desc.target = at;
    desc.prologue_bytes = spec.prologue;
    desc.stack_arg_bytes = spec.arg_bytes;
    desc.callee_pops = spec.callee_pops;
    const int32_t id = g_rt.daemon->install(&desc);
    HuSiteInfo info;
    return id >= 0 && site_matches(id, spec, s.bytes, &info);
}

// The compass global, read out of the draw entry's intact copy at +31 and
// pinned at +2, the prologue's imm32, before the site is scanned for: a jump
// the daemon has written there covers +2. On an unpatched entry the two
// copies must agree; on a patched one the daemon's saved prologue says so
// when the site is accepted.
bool pin_compass_global(const uint8_t* text, size_t size, Sig& s, char* why, size_t len) {
    ScanResult r;
    scan(text, size, s.bytes, s.mask, true, &r);
    if (r.count == 0) {
        snprintf(why, len, "signature compass_draw: not found (client patched, or another hook owns it)");
        return false;
    }
    if (r.count != 1) {
        snprintf(why, len, "signature compass_draw: matched %d times", r.count);
        return false;
    }
    const uint8_t* hit = r.hits[0];
    const uint32_t global = rd32(hit, kCompassDrawGlobal2);
    if (!r.jumped[0] && rd32(hit, kCompassDrawGlobal) != global) {
        snprintf(why, len, "compass_draw: its two compass globals disagree");
        return false;
    }
    uint8_t** at = reinterpret_cast<uint8_t**>(rdptr(hit, kCompassDrawGlobal2));
    if (!readable(at, 4)) {
        snprintf(why, len, "compass global at 0x%08X is not readable", static_cast<unsigned>(global));
        return false;
    }
    memcpy(s.bytes + kCompassDrawGlobal, &global, 4);
    memset(s.mask + kCompassDrawGlobal, 'x', 4);
    g_engine.game.compass = at;
    return true;
}

// An imm32 a site's prologue reads that the engine knows from nowhere else
// (the macro key gate's word at +2, the ability opener's global at +1),
// pinned at `at` before the site is scanned for: read out of the one hit,
// or, on a hit the daemon has already patched (the jump covers it), out of
// the original the daemon saved there, which the site's acceptance compares
// against the pinned prologue in turn. `*hit_out` is that hit.
bool pin_prologue_imm(const uint8_t* text, size_t size, const SiteSpec& spec, Sig& s, size_t at,
                      const uint8_t** hit_out, char* why, size_t len) {
    ScanResult r;
    scan(text, size, s.bytes, s.mask, true, &r);
    if (r.count == 0) {
        snprintf(why, len, "signature %s: not found (client patched, or another hook owns it)", spec.name);
        return false;
    }
    if (r.count != 1) {
        snprintf(why, len, "signature %s: matched %d times", spec.name, r.count);
        return false;
    }
    const uint8_t* hit = r.hits[0];
    const uint8_t* source = hit;
    HuSiteInfo info;
    if (r.jumped[0]) {
        HuSiteDesc desc;
        desc.size = sizeof(desc);
        desc.target = hit;
        desc.prologue_bytes = spec.prologue;
        desc.stack_arg_bytes = spec.arg_bytes;
        desc.callee_pops = spec.callee_pops;
        const int32_t id = g_rt.daemon->install(&desc);
        memset(&info, 0, sizeof(info));
        info.size = sizeof(info);
        if (id < 0 || g_rt.daemon->site_info(id, &info) != HU_OK || info.prologue_bytes < spec.prologue
            || memcmp(info.original, s.bytes, at) != 0) {
            snprintf(why, len, "signature %s: not found (client patched, or another hook owns it)", spec.name);
            return false;
        }
        source = info.original;
    }
    memcpy(s.bytes + at, source + at, 4);
    memset(s.mask + at, 'x', 4);
    *hit_out = hit;
    return true;
}

// The ability opener: its global pinned, then the imm32 of its second key
// push, past the daemon's jump either way, must point at the registry's
// ability key: the open that routine makes is the one the engine's events
// and category blocks go by.
bool pin_ability_open(const uint8_t* text, size_t size, const SiteSpec& spec, Sig& s, char* why, size_t len) {
    const uint8_t* hit = NULL;
    if (!pin_prologue_imm(text, size, spec, s, kAbilityOpenGlobal, &hit, why, len)) {
        return false;
    }
    const int ability = g_engine.inv.ability;
    const uint8_t* key = rdptr(hit, kAbilityOpenKeyImm);
    if (ability < 0 || !readable(key, kKeyLen) || memcmp(key, g_engine.inv.names[ability].key, kKeyLen) != 0) {
        snprintf(why, len, "ability_open: its key push at +%u does not name the registry's ability key",
            static_cast<unsigned>(kAbilityOpenKeyImm));
        return false;
    }
    return true;
}

// The key test, from the UI update's call at kUpdateKeyTestCall: its
// prologue read out of the routine, or on a site the daemon patched for
// this shape before, out of the original it saved. The bytes past the
// prologue are never patched.
bool resolve_key_test(const uint8_t* text, size_t size, const SiteSpec& spec, char* why, size_t len) {
    const uint8_t* update = g_rt.targets[kSiteUpdate];
    const uint8_t* at = update[kUpdateKeyTestCall] == 0xE8 ? call_target(update, kUpdateKeyTestCall) : NULL;
    if (!in_text(at, text, size) || !readable(at, spec.prologue + sizeof(kKeyTestBody))) {
        snprintf(why, len, "ui_update: its call at +%u does not reach a routine in FFXiMain's code",
            static_cast<unsigned>(kUpdateKeyTestCall));
        return false;
    }
    uint8_t* prologue = g_rt.prologues[kSiteKeyTest];
    memcpy(prologue, at, spec.prologue);
    if (at[0] == 0xE9) {
        HuSiteDesc desc;
        desc.size = sizeof(desc);
        desc.target = at;
        desc.prologue_bytes = spec.prologue;
        desc.stack_arg_bytes = spec.arg_bytes;
        desc.callee_pops = spec.callee_pops;
        const int32_t id = g_rt.daemon->install(&desc);
        HuSiteInfo info;
        memset(&info, 0, sizeof(info));
        info.size = sizeof(info);
        if (id < 0 || g_rt.daemon->site_info(id, &info) != HU_OK || info.prologue_bytes != spec.prologue
            || info.stack_arg_bytes != spec.arg_bytes || info.callee_pops != spec.callee_pops) {
            snprintf(why, len, "key_test: another hook owns it");
            return false;
        }
        memcpy(prologue, info.original, spec.prologue);
    }
    if (memcmp(prologue, kKeyTestHead, sizeof(kKeyTestHead)) != 0
        || memcmp(at + spec.prologue, kKeyTestBody, sizeof(kKeyTestBody)) != 0) {
        snprintf(why, len, "key_test: the routine the UI update calls is not the key test (client patched?)");
        return false;
    }
    g_rt.targets[kSiteKeyTest] = at;
    return true;
}

// The target window's two arrow draws: each a call of the sprite draw the
// site hooks, the second just after the first. The sprite draw's pre tells
// an arrow's draw by the address each call returns to.
bool resolve_arrows(const uint8_t* text, size_t size, char* why, size_t len) {
    const uint8_t* target = find_one(text, size, "target_arrow", kSigTargetArrow, why, len);
    const uint8_t* pick = target ? find_one(text, size, "pick_arrow", kSigPickArrow, why, len) : NULL;
    if (!pick) {
        return false;
    }
    const uint8_t* sprite = g_rt.targets[kSiteSpriteDraw];
    if (call_target(target, kTargetArrowCall) != sprite || call_target(pick, kPickArrowCall) != sprite
        || pick <= target || pick > target + kArrowSpan) {
        snprintf(why, len, "target_arrow: the target window's two arrow draws are not its calls of sprite_draw"
            " (client patched?)");
        return false;
    }
    g_engine.game.arrow_return[0] = reinterpret_cast<uintptr_t>(target + kTargetArrowCall + 5);
    g_engine.game.arrow_return[1] = reinterpret_cast<uintptr_t>(pick + kPickArrowCall + 5);
    return true;
}

// The quad routine, its first five bytes as found or another hook's jump,
// must call the draw primitive the site found at kQuadPrimitiveCall: the
// draw every quad of the interface takes.
bool resolve_quad(const uint8_t* text, size_t size, char* why, size_t len) {
    Sig s;
    s.length = parse_signature(kSigQuadDraw, s.bytes, s.mask, sizeof(s.bytes));
    if (!s.length) {
        snprintf(why, len, "signature quad_draw: malformed");
        return false;
    }
    ScanResult r;
    scan(text, size, s.bytes, s.mask, true, &r);
    if (r.count != 1) {
        if (r.count == 0) {
            snprintf(why, len, "signature quad_draw: not found (client patched?)");
        } else {
            snprintf(why, len, "signature quad_draw: matched %d times", r.count);
        }
        return false;
    }
    const uint8_t* quad = r.hits[0];
    const uint8_t* primitive = g_rt.targets[kSitePrimitiveDraw];
    if (quad[kQuadPrimitiveCall] != 0xE8 || call_target(quad, kQuadPrimitiveCall) != primitive) {
        snprintf(why, len, "quad_draw: its call at +0x%03X does not reach primitive_draw (client patched?)",
            static_cast<unsigned>(kQuadPrimitiveCall));
        return false;
    }
    g_rt.quad = quad;
    return true;
}

bool resolve_sites(const uint8_t* text, size_t size, char* why, size_t len) {
    for (int i = 0; i < kSiteCount; ++i) {
        const SiteSpec& spec = kSites[i];
        if (!spec.signature) {
            if (!resolve_key_test(text, size, spec, why, len)) {
                return false;
            }
            continue;
        }
        Sig s;
        s.length = parse_signature(spec.signature, s.bytes, s.mask, sizeof(s.bytes));
        if (s.length < spec.prologue) {
            snprintf(why, len, "signature %s: malformed", spec.name);
            return false;
        }
        // The manager's address, resolved before the sites, in its imm32.
        if (spec.manager_imm) {
            if (spec.manager_imm + 4 > s.length) {
                snprintf(why, len, "signature %s: malformed", spec.name);
                return false;
            }
            memcpy(s.bytes + spec.manager_imm, &g_engine.game.mcb, 4);
            memset(s.mask + spec.manager_imm, 'x', 4);
        }
        if (i == kSiteCompassDraw && !pin_compass_global(text, size, s, why, len)) {
            return false;
        }
        const uint8_t* pinned = NULL;
        if (i == kSiteMacroGate
            && !pin_prologue_imm(text, size, spec, s, kMacroGateGlobal, &pinned, why, len)) {
            return false;
        }
        if (i == kSiteAbilityOpen && !pin_ability_open(text, size, spec, s, why, len)) {
            return false;
        }
        for (uint32_t k = 0; k < spec.prologue; ++k) {
            if (s.mask[k] != 'x') {
                snprintf(why, len, "signature %s: its prologue is not exact bytes", spec.name);
                return false;
            }
        }
        ScanResult r;
        scan(text, size, s.bytes, s.mask, true, &r);
        if (r.count > 4) {
            snprintf(why, len, "signature %s: matched %d times", spec.name, r.count);
            return false;
        }
        const uint8_t* found = NULL;
        int accepted = 0;
        for (int k = 0; k < r.count; ++k) {
            if (!r.jumped[k] || daemon_owns(r.hits[k], spec, s)) {
                found = r.hits[k];
                ++accepted;
            }
        }
        memcpy(g_rt.prologues[i], s.bytes, spec.prologue);
        if (accepted != 1) {
            if (accepted == 0) {
                snprintf(why, len, "signature %s: not found (client patched, or another hook owns it)",
                    spec.name);
            } else {
                snprintf(why, len, "signature %s: matched %d times", spec.name, accepted);
            }
            return false;
        }
        g_rt.targets[i] = found;
    }
    // The sink's pre tells the player's keys by this call's return address,
    // so the call must reach the sink hooked.
    const uint8_t* call = g_engine.game.menu_routing + kRoutingSinkCall;
    int32_t rel;
    memcpy(&rel, call + 1, 4);
    if (call + 5 + rel != g_rt.targets[kSiteMenuInput]) {
        snprintf(why, len, "menu_routing: its call at +0x%02X does not reach menu_input",
            static_cast<unsigned>(kRoutingSinkCall));
        return false;
    }
    g_engine.game.routing_return = reinterpret_cast<uintptr_t>(call + 5);
    memcpy(&g_engine.game.open, &g_rt.targets[kSiteOpen], sizeof(g_engine.game.open));
    memcpy(&g_engine.game.template_swap, &g_rt.targets[kSiteTemplateSwap], sizeof(g_engine.game.template_swap));
    memcpy(&g_engine.game.ability_list, &g_rt.targets[kSiteAbilityOpen], sizeof(g_engine.game.ability_list));
    return resolve_quad(text, size, why, len) && resolve_arrows(text, size, why, len);
}

// Clears every handler this image set. False when a hooked call was still
// running: the handlers are gone, but the image must stay mapped.
bool clear_handlers() {
    bool all = true;
    for (int i = 0; i < kSiteCount; ++i) {
        if (g_rt.daemon->clear_handlers(g_rt.sites[i], kClearWaitMs) != HU_OK) {
            all = false;
        }
    }
    g_rt.pinned_busy = !all;
    return all;
}

uint8_t* compass_guarded();
bool targetarrow_guarded();

uint8_t* live_menu_guarded(int n, uint8_t** ctl_out);

bool install_engine() {
    HU_STEP("install_engine");
    if (g_rt.state == kInstalled) {
        return true;
    }
    // A closing engine's handlers are still set; only a handle takes it back
    // (op_new).
    if (g_rt.state == kFailed || g_rt.state == kClosing) {
        return false;
    }
    char why[256];
    if (!g_rt.inventory_ready) {
        if (!g_engine.inv.build()) {
            return fail("inventory: menu_table.inc could not be indexed", kPlayerVersion);
        }
        g_rt.inventory_ready = true;
    }
    if (!g_rt.resolved) {
        const uint8_t* text = NULL;
        size_t size = 0;
        if (!text_section(&text, &size)) {
            return fail("FFXiMain.dll is not loaded, or its .text is not readable", kPlayerVersion);
        }
        if (!resolve_plain(text, size, why, sizeof(why))) {
            return fail(why, kPlayerVersion);
        }
        const char* player = kPlayerRestart;
        if (!acquire_daemon(why, sizeof(why), &player)) {
            return fail(why, player);
        }
        if (!resolve_sites(text, size, why, sizeof(why))) {
            return fail(why, kPlayerVersion);
        }
        if (!resolve_boxes(text, size, why, sizeof(why))) {
            return fail(why, kPlayerVersion);
        }
        if (!resolve_asked_windows(text, size, why, sizeof(why))) {
            return fail(why, kPlayerVersion);
        }
        g_rt.resolved = true;
    }
    if (!g_rt.claim && !claim_engine(why, sizeof(why))) {
        return fail(why, kPlayerOldEngine);
    }
    if (!take_self_reference()) {
        release_claim();
        return fail("pin: this image could not take a reference on itself", kPlayerRestart);
    }
    HU_HEAP_CHECK("install: before the hooks");
    for (int i = 0; i < kSiteCount; ++i) {
        const SiteSpec& spec = kSites[i];
        HuSiteDesc desc;
        desc.size = sizeof(desc);
        desc.target = g_rt.targets[i];
        desc.prologue_bytes = spec.prologue;
        desc.stack_arg_bytes = spec.arg_bytes;
        desc.callee_pops = spec.callee_pops;
        const int32_t id = g_rt.daemon->install(&desc);
        HuSiteInfo info;
        memset(&info, 0, sizeof(info));
        const bool matches = id >= 0 && site_matches(id, spec, g_rt.prologues[i], &info);
        if (!matches) {
            const bool passes = daemon_error_passes(id);
            if (passes) {
                snprintf(why, sizeof(why), "hook %s: the daemon could not patch it just now (error %d)",
                    spec.name, static_cast<int>(id));
            } else if (id < 0) {
                snprintf(why, sizeof(why), "hook %s: the daemon refused it (error %d)",
                    spec.name, static_cast<int>(id));
            } else if (info.prologue_bytes == 0) {
                snprintf(why, sizeof(why), "hook %s: the daemon could not describe its site there", spec.name);
            } else {
                snprintf(why, sizeof(why), "hook %s: the daemon's site there has another shape"
                    " (prologue %u, %u argument bytes, pops %u; this build's %u, %u, %u)",
                    spec.name, static_cast<unsigned>(info.prologue_bytes),
                    static_cast<unsigned>(info.stack_arg_bytes), static_cast<unsigned>(info.callee_pops),
                    static_cast<unsigned>(spec.prologue), static_cast<unsigned>(spec.arg_bytes),
                    static_cast<unsigned>(spec.callee_pops));
            }
            release_claim();
            if (!g_rt.pinned_busy) {
                release_self_reference();
            }
            return passes ? retry_later(why) : fail(why, kPlayerRestart);
        }
        g_rt.sites[i] = id;
    }
    for (int i = 0; i < kSiteCount; ++i) {
        const int32_t set = g_rt.daemon->set_handlers(g_rt.sites[i], kSites[i].pre, kSites[i].post, &g_engine);
        if (set != HU_OK) {
            const bool passes = daemon_error_passes(set);
            snprintf(why, sizeof(why), "hook %s: the daemon %s the handlers (error %d)", kSites[i].name,
                passes ? "could not set" : "refused", static_cast<int>(set));
            if (clear_handlers()) {
                release_claim();
                release_self_reference();
            }
            return passes ? retry_later(why) : fail(why, kPlayerRestart);
        }
    }
    // The compass and the target arrows as the install finds them: their
    // first opened or closed events are changes from here.
    g_engine.compass_shown = compass_open(compass_guarded()) ? 1 : 0;
    g_engine.targetarrow_shown = targetarrow_guarded() ? 1 : 0;
    // The windows open at install: their first event is their close.
    for (int n = 0; n < g_engine.inv.count; ++n) {
        g_engine.reported[n] = synthetic(g_engine, n) ? NULL : live_menu_guarded(n, NULL);
    }
    g_rt.pinned_busy = false;
    g_rt.transient = false;
    g_rt.state = kInstalled;
    HU_HEAP_CHECK("install: hooks set");
    return true;
}

// ---------------------------------------------------------------------------
// Reads from the Lua thread. The game may retire a menu between two hops, so
// every pointer is checked before it is followed.

uint8_t* live_menu_guarded(int n, uint8_t** ctl_out) {
    const NameEntry& ne = g_engine.inv.names[n];
    for (int i = 0; i < ne.row_count; ++i) {
        uint8_t** slot = g_engine.game.slot[ne.rows[i]];
        uint8_t* ctl = slot ? *slot : NULL;
        if (!ctl || !readable(ctl, kCtlMenu + 4)) {
            continue;
        }
        uint8_t* menu = rdptr(ctl, kCtlMenu);
        if (!menu || !readable(menu, kMenuBytes)) {
            continue;
        }
        const uint8_t* res = rdptr(menu, kMenuRes);
        if (!res || !readable(res + kResKey, kKeyLen) || !ne.keyed(res + kResKey)) {
            continue;
        }
        if (ctl_out) {
            *ctl_out = ctl;
        }
        return menu;
    }
    return NULL;
}

// live_menu_guarded() when the window is open (game.h's menu_is_open): what
// every reader and verb reports, from the frame the closed event is posted.
uint8_t* open_menu_guarded(int n, uint8_t** ctl_out) {
    uint8_t* ctl = NULL;
    uint8_t* menu = n >= 0 ? live_menu_guarded(n, &ctl) : NULL;
    if (!menu_is_open(menu)) {
        return NULL;
    }
    if (ctl_out) {
        *ctl_out = ctl;
    }
    return menu;
}

// A controller through its registry row's slot, with `bytes` of it readable.
uint8_t* controller_guarded(const char* nm, size_t bytes) {
    const int n = g_engine.inv.find_exact(nm);
    if (n < 0 || g_engine.inv.names[n].row_count == 0) {
        return NULL;
    }
    uint8_t** slot = g_engine.game.slot[g_engine.inv.names[n].rows[0]];
    uint8_t* ctl = slot ? *slot : NULL;
    return ctl && readable(ctl, bytes) ? ctl : NULL;
}

bool open_guarded(const char* nm) {
    return open_menu_guarded(g_engine.inv.find_exact(nm), NULL) != NULL;
}

// An instance of the window, open or closing.
bool live_guarded(const char* nm) {
    const int n = g_engine.inv.find_exact(nm);
    return n >= 0 && live_menu_guarded(n, NULL) != NULL;
}

// The compass object while its global names one this thread can read.
uint8_t* compass_guarded() {
    uint8_t* c = compass_object(g_engine);
    return c && readable(c, kCompassBytes) ? c : NULL;
}

// The draw walk's gates as this thread can read them: the manager, and
// rem4line's controller.
const uint8_t* mcb_guarded() {
    const uint8_t* mcb = g_engine.game.mcb;
    return mcb && readable(mcb, kMcbBytes) ? mcb : NULL;
}

const uint8_t* modal_guarded() {
    return controller_guarded("rem4line", kBoxBytes);
}

// The target window's draw painting an arrow, as the drain reads it.
bool targetarrow_guarded() {
    uint8_t* ctl = NULL;
    const uint8_t* menu = live_menu_guarded(g_engine.inv.targetwi, &ctl);
    return menu && readable(ctl, kTargetBytes) && arrows_drawn(menu, ctl, mcb_guarded(), modal_guarded());
}

// Open as every reader reports it: a window's live instance with the marks
// clear, the compass's state byte, the target arrows drawn.
bool open_of(int n) {
    if (is_compass(g_engine, n)) {
        return compass_open(compass_guarded());
    }
    if (is_targetarrow(g_engine, n)) {
        return targetarrow_guarded();
    }
    return open_menu_guarded(n, NULL) != NULL;
}

// The registry's words for a name; one on no row reads as zeros.
const RowSpec& spec_of(const NameEntry& ne) {
    static const RowSpec none = {"", "", 0, 0, 0, 0};
    return ne.row_count ? kRowSpecs[ne.rows[0]] : none;
}

// Whether a block may hold the name: never a hide-only or block-refused one.
bool blockable(int n) {
    return !block_refusal(g_engine.inv.names[n].name);
}

struct ElementInfo {
    int16_t type;
    bool placed;            // x and y are fields of this type
    bool sized;             // w and h are fields of this type
    int16_t x;
    int16_t y;
    int16_t w;
    int16_t h;
    char text[kElementText];
};

// Where each type the draw knows keeps its box: the frame (0) at
// +0x1A/+0x1C sized +0x22/+0x24, as SetFrameRect writes them; a list item
// (1) at +0x2A/+0x2C sized +0x2E/+0x30, as its re-lay writes them and its hit
// test reads them (its +0x1C and +0x20 are the sprite's float scales); the
// cursor (2) at +0x20/+0x22, with no size. 0: the type has no such field.
struct ElementLayout {
    size_t x;
    size_t y;
    size_t w;
    size_t h;
};
const ElementLayout kElementLayouts[] = {
    {0x1A, 0x1C, 0x22, 0x24},
    {0x2A, 0x2C, 0x2E, 0x30},
    {0x20, 0x22, 0, 0},
};
const int kElementTypes = static_cast<int>(sizeof(kElementLayouts) / sizeof(kElementLayouts[0]));
const size_t kElementBytes = 0x32;

// A list item's template text: the string at +0x40, else the one at +0x44,
// the order the game reads them in. Frames and the cursor carry none.
const size_t kItemText = 0x40;
const size_t kItemTextAlt = 0x44;

// The menu's element list: node+0 next, +0x10 item, +0x14 nonzero when the
// game has tombstoned the node.
int read_elements(const uint8_t* menu, ElementInfo* out, int max, bool* truncated) {
    *truncated = false;
    if (!readable(menu + kMenuList, 4)) {
        return 0;
    }
    const uint8_t* node = rdptr(menu, kMenuList);
    int stored = 0;
    for (int steps = 0; node && steps < 512; ++steps) {
        if (!readable(node, 0x15)) {
            break;
        }
        if (node[0x14] == 0) {
            const uint8_t* item = rdptr(node, 0x10);
            if (item && readable(item, kElementBytes)) {
                if (stored < max) {
                    ElementInfo& e = out[stored++];
                    memset(&e, 0, sizeof(e));
                    e.type = rd16(item, 0x00);
                    if (e.type >= 0 && e.type < kElementTypes) {
                        const ElementLayout& l = kElementLayouts[e.type];
                        e.placed = true;
                        e.x = rd16(item, l.x);
                        e.y = rd16(item, l.y);
                        if (l.w) {
                            e.sized = true;
                            e.w = rd16(item, l.w);
                            e.h = rd16(item, l.h);
                        }
                    }
                    if (e.type == 1 && readable(item, kItemTextAlt + 4)) {
                        const char* text = reinterpret_cast<const char*>(rdptr(item, kItemText));
                        if (!text) {
                            text = reinterpret_cast<const char*>(rdptr(item, kItemTextAlt));
                        }
                        copy_text(text, e.text, sizeof(e.text));
                    }
                } else {
                    *truncated = true;
                }
            }
        }
        node = rdptr(node, 0);
    }
    return stored;
}

const char* dock_group_of(uint32_t policy) {
    for (int g = 0; g < kGroupCount; ++g) {
        if (policy & kGroups[g].mask) {
            return kGroups[g].name;
        }
    }
    return NULL;
}

void set_hex(ReplyWriter& w, const char* key, const void* p) {
    char text[16];
    snprintf(text, sizeof(text), "0x%08X", static_cast<unsigned>(reinterpret_cast<uintptr_t>(p)));
    w.set_string(key, text);
}

// ---------------------------------------------------------------------------
// The operations. Every copy's Lua bindings call them: this image's own when
// it is the resident, the resident's through its table when it forwards.
// Plain C in and out; none calls into Lua.

const char* category_name(int32_t category);

// Under the lock. NULL with `why` filled when the call cannot go ahead.
Handle* usable(const HuEngineHandle* ref, char* why, size_t size) {
    if (g_rt.state == kFailed) {
        snprintf(why, size, "%s", g_rt.failure);
        return NULL;
    }
    if (g_rt.state != kInstalled) {
        snprintf(why, size, "the hideui engine is shut down");
        return NULL;
    }
    Handle* h = g_rt.handles.get(ref->slot, ref->generation);
    if (!h) {
        snprintf(why, size, "this hideui handle is released");
    }
    return h;
}

// A name as engine abi 1 and 2 take it, a group name standing for its
// anchor; fills `why` when it names no menu.
int name_index(const char* name, char* why, size_t size) {
    const int n = name ? g_engine.inv.find(name) : -1;
    if (n < 0) {
        snprintf(why, size, "no such menu: %s", name ? name : "(not a string)");
    }
    return n;
}

// A window name as engine abi 3 takes it: never a group's.
int window_index(const char* name, char* why, size_t size) {
    if (find_group_any(name) >= 0) {
        snprintf(why, size, "%s is a group name; only move_group and groups take one", name);
        return -1;
    }
    const int n = name ? g_engine.inv.find_window(name) : -1;
    if (n < 0) {
        snprintf(why, size, "no such window: %s", name ? name : "(not a string)");
    }
    return n;
}

int lookup(const char* name, bool windows, char* why, size_t size) {
    return windows ? window_index(name, why, size) : name_index(name, why, size);
}

bool enqueue(const Command& c, char* why, size_t size) {
    if (!g_engine.commands.push(c)) {
        snprintf(why, size, "the game thread has not taken the last %u commands",
            static_cast<unsigned>(kCommandCapacity));
        return false;
    }
    return true;
}

Command command(uint8_t op, int target, int x, int y, const HuEngineHandle* ref, uint32_t value, uint8_t verb) {
    Command c;
    memset(&c, 0, sizeof(c));
    c.op = op;
    c.verb = verb;
    c.target = static_cast<int16_t>(target);
    c.x = static_cast<int16_t>(x);
    c.y = static_cast<int16_t>(y);
    c.slot = ref ? ref->slot : -1;
    c.gen = ref ? ref->generation : 0;
    c.value = value;
    return c;
}

// Under the lock: a reply counts against its window until the drain has
// carried it out. Engine abi 3 refuses a second one while one is queued, to
// the window or, for a dialogue box, to its twin, the two being one
// dialogue; the earlier slots queue it as they always did.
bool enqueue_reply(Command c, bool refuse_queued, char* why, size_t size) {
    HU_STEP_AT("enqueue_reply", c.op);
    const int n = c.target;
    if (n < 0 || n >= g_engine.inv.count) {
        snprintf(why, size, "no window takes this reply");
        return false;
    }
    const int twin = g_engine.inv.dialogue_twin(n);
    const int queued = g_engine.replies[n] > 0 ? n : twin >= 0 && g_engine.replies[twin] > 0 ? twin : -1;
    if (refuse_queued && queued >= 0) {
        snprintf(why, size, "a reply to %s is already queued", g_engine.inv.names[queued].name);
        return false;
    }
    c.flags |= kCmdReply;
    InterlockedIncrement(&g_engine.replies[n]);
    if (!enqueue(c, why, size)) {
        InterlockedDecrement(&g_engine.replies[n]);
        return false;
    }
    return true;
}

// A whole number in lo..hi. A Lua value that was not a number arrives as NaN
// and fails the range test.
bool int_value(double v, int lo, int hi, int* out) {
    if (!(v >= lo && v <= hi)) {
        return false;
    }
    const int i = static_cast<int>(v);
    if (static_cast<double>(i) != v) {
        return false;
    }
    *out = i;
    return true;
}

// `why` as "prefix: reason", or the reason alone: engine abi 1 and 2 named
// the verb in every refusal, abi 3's caller knows it.
void refusal(char* why, size_t size, const char* prefix, const char* format, ...) {
    char text[kWhyBytes];
    va_list args;
    va_start(args, format);
    vsnprintf(text, sizeof(text), format, args);
    va_end(args);
    if (prefix && prefix[0]) {
        snprintf(why, size, "%s: %s", prefix, text);
    } else {
        snprintf(why, size, "%s", text);
    }
}

void __stdcall op_new(const char* name, HuEngineHandle* out) {
    HU_STEP("op_new");
    out->slot = -1;
    out->generation = 0;
    if (!name) {
        return;
    }
    lock();
    // A busy shutdown leaves the resident idle but published; the next handle
    // installs it again, from whichever copy it comes.
    if (g_rt.state == kIdle && published_as_self()) {
        install_engine();
    }
    // A handle made while closing -- a reload's new() before the frame its
    // unload's shutdown waits for -- cancels that shutdown: the restore it
    // queued still runs, ahead of anything this handle queues, and its
    // thread leaves without unhooking.
    if (g_rt.state == kInstalled || g_rt.state == kClosing) {
        const int slot = g_rt.handles.claim(name, g_engine.events.position());
        if (slot >= 0) {
            g_rt.state = kInstalled;
            out->slot = slot;
            out->generation = g_rt.handles.slots[slot].generation;
        }
    }
    unlock();
}

// op_new, saying why when it makes no handle. Events start here: the
// handle's cursor is the ring's position now.
void __stdcall op_handle_open(const char* name, HuEngineHandle* out, char* why, uint32_t size) {
    op_new(name, out);
    if (out->slot >= 0) {
        return;
    }
    lock();
    if (!name) {
        snprintf(why, size, "new(name) needs a string");
    } else if (g_rt.state == kFailed || g_rt.transient) {
        snprintf(why, size, "%s", g_rt.player ? g_rt.player : g_rt.failure);
    } else if (g_rt.state == kInstalled || g_rt.state == kClosing) {
        snprintf(why, size, "out of memory for another handle");
    } else {
        snprintf(why, size, "the hideui engine is shut down");
    }
    unlock();
}

void __stdcall op_release(const HuEngineHandle* ref) {
    HU_STEP("op_release");
    lock();
    Handle* h = g_rt.handles.get(ref->slot, ref->generation);
    if (h) {
        g_engine.holds.release(*h, g_engine.inv.count);
        // A full queue leaves the handle's moves and sizes where they are;
        // status().detail.drain_errors counts it.
        if (g_rt.state == kInstalled
            && !g_engine.commands.push(command(kOpResetOwned, 0, 0, 0, ref, 0, kVerbResetAll))) {
            InterlockedIncrement(&g_engine.drain_errors);
        }
        g_rt.handles.free_slot(ref->slot);
    }
    unlock();
}

int32_t hold(const HuEngineHandle* ref, const char* name, uint8_t bit, bool on, bool windows, char* why,
             uint32_t size) {
    HU_STEP_AT("hold", bit);
    bool ok = false;
    lock();
    Handle* h = usable(ref, why, size);
    if (h) {
        const int n = lookup(name, windows, why, size);
        if (n >= 0) {
            // The compass and the target arrows take a block as a window
            // does, hidden by the want bit their draw hooks read; no
            // `blocked` event ever follows, the game never attempts an open
            // of them.
            if (bit == kHoldBlock && on && block_refusal(g_engine.inv.names[n].name)) {
                snprintf(why, size, "%s", block_refusal(g_engine.inv.names[n].name));
            } else {
                // A dialogue box's hold is its twin's too: the two show one
                // dialogue. Both are hide-only, so a block of either is
                // refused as one of both.
                g_engine.holds.set(*h, n, bit, on);
                const int twin = g_engine.inv.dialogue_twin(n);
                if (twin >= 0) {
                    g_engine.holds.set(*h, twin, bit, on);
                }
                ok = true;
            }
        }
    }
    unlock();
    return ok;
}

int32_t __stdcall op_hide(const HuEngineHandle* ref, const char* name, char* why, uint32_t size) {
    return hold(ref, name, kHoldHide, true, false, why, size);
}

int32_t __stdcall op_unhide(const HuEngineHandle* ref, const char* name, char* why, uint32_t size) {
    return hold(ref, name, kHoldHide, false, false, why, size);
}

int32_t __stdcall op_block(const HuEngineHandle* ref, const char* name, char* why, uint32_t size) {
    return hold(ref, name, kHoldBlock, true, false, why, size);
}

int32_t __stdcall op_unblock(const HuEngineHandle* ref, const char* name, char* why, uint32_t size) {
    return hold(ref, name, kHoldBlock, false, false, why, size);
}

int32_t __stdcall op_hide3(const HuEngineHandle* ref, const char* name, char* why, uint32_t size) {
    return hold(ref, name, kHoldHide, true, true, why, size);
}

int32_t __stdcall op_unhide3(const HuEngineHandle* ref, const char* name, char* why, uint32_t size) {
    return hold(ref, name, kHoldHide, false, true, why, size);
}

int32_t __stdcall op_block3(const HuEngineHandle* ref, const char* name, char* why, uint32_t size) {
    return hold(ref, name, kHoldBlock, true, true, why, size);
}

int32_t __stdcall op_unblock3(const HuEngineHandle* ref, const char* name, char* why, uint32_t size) {
    return hold(ref, name, kHoldBlock, false, true, why, size);
}

// With `frame`, engine abi 5's: x, y are the frame's top-left for every
// window, which the drain converts for a bottom-anchored one.
int32_t place_command(const HuEngineHandle* ref, const char* name, double x, double y, bool windows, bool frame,
                      char* why, uint32_t size) {
    int ix = 0;
    int iy = 0;
    const bool coords = int_value(x, -32768, 32767, &ix) && int_value(y, -32768, 32767, &iy);
    bool ok = false;
    lock();
    if (usable(ref, why, size)) {
        const int n = lookup(name, windows, why, size);
        if (n >= 0) {
            if (!coords) {
                snprintf(why, size, "usage: move(name, x, y) with whole-number x and y");
            } else if (is_targetarrow(g_engine, n)) {
                // Accepted and ignored: the arrows follow the target; nothing
                // is queued or remembered.
                ok = true;
            } else {
                Command c = command(kOpMove, n, ix, iy, ref, 0, kVerbMove);
                if (frame) {
                    c.flags |= kCmdFrame;
                }
                ok = enqueue(c, why, size);
            }
        }
    }
    unlock();
    return ok;
}

int32_t __stdcall op_move(const HuEngineHandle* ref, const char* name, double x, double y,
                          char* why, uint32_t size) {
    return place_command(ref, name, x, y, false, false, why, size);
}

int32_t __stdcall op_move3(const HuEngineHandle* ref, const char* name, double x, double y,
                           char* why, uint32_t size) {
    return place_command(ref, name, x, y, true, false, why, size);
}

int32_t __stdcall op_move5(const HuEngineHandle* ref, const char* name, double x, double y,
                           char* why, uint32_t size) {
    return place_command(ref, name, x, y, true, true, why, size);
}

// Engine abi 1 and 2 shift the group by dx, dy; abi 3 and 4 put its
// anchor's origin at x, y and the rest by as much, abi 5 its frame's
// top-left. Abi 4's and 5's wait for the anchor's next open when it is
// closed; the others refuse.
int32_t group_command(const HuEngineHandle* ref, const char* group, double x, double y, bool absolute, bool wait,
                      bool frame, char* why, uint32_t size) {
    int ix = 0;
    int iy = 0;
    const bool coords = int_value(x, -32768, 32767, &ix) && int_value(y, -32768, 32767, &iy);
    bool ok = false;
    lock();
    if (usable(ref, why, size)) {
        const int g = absolute ? find_group_any(group) : find_group(group);
        if (g < 0) {
            snprintf(why, size, "no such group: %s (chat_log, party_list or target_window)",
                group ? group : "(not a string)");
        } else if (!coords) {
            snprintf(why, size, absolute ? "usage: move_group(group, x, y) with whole-number x and y"
                                         : "usage: move_group(group, dx, dy) with whole-number dx and dy");
        } else if (!wait && !open_menu_guarded(g_engine.inv.find_exact(kGroups[g].anchor), NULL)) {
            snprintf(why, size, "%s: its anchor %s is not open", kGroups[g].name, kGroups[g].anchor);
        } else {
            Command c = command(absolute ? kOpGroupTo : kOpGroup, g, ix, iy, ref, 0, kVerbMoveGroup);
            if (wait) {
                c.flags |= kCmdWait;
            }
            if (frame) {
                c.flags |= kCmdFrame;
            }
            ok = enqueue(c, why, size);
        }
    }
    unlock();
    return ok;
}

int32_t __stdcall op_move_group(const HuEngineHandle* ref, const char* group, double dx, double dy,
                                char* why, uint32_t size) {
    return group_command(ref, group, dx, dy, false, false, false, why, size);
}

int32_t __stdcall op_move_group3(const HuEngineHandle* ref, const char* group, double x, double y,
                                 char* why, uint32_t size) {
    return group_command(ref, group, x, y, true, false, false, why, size);
}

int32_t __stdcall op_move_group4(const HuEngineHandle* ref, const char* group, double x, double y,
                                 char* why, uint32_t size) {
    return group_command(ref, group, x, y, true, true, false, why, size);
}

int32_t __stdcall op_move_group5(const HuEngineHandle* ref, const char* group, double x, double y,
                                 char* why, uint32_t size) {
    return group_command(ref, group, x, y, true, true, true, why, size);
}

// The reason of the pair in `table` naming nm while its other window is
// open (`open` true) or not open (false), read from the Lua thread; NULL
// when no pair names nm or the other window is not so.
const char* pair_refusal(const PairSpec* table, int count, const char* nm, bool open) {
    const PairSpec* p = pair_spec(table, count, nm);
    return p && open_guarded(p->other) == open ? p->reason : NULL;
}

// game.h's parent_open(), read from the Lua thread: why child window nm
// cannot open now, in `why`, or NULL.
const char* child_refusal(const char* nm, char* why, size_t size) {
    const ChildSpec* c = child_spec(nm);
    for (int i = 0; c && c->parents[i]; ++i) {
        if (open_guarded(c->parents[i])) {
            return NULL;
        }
    }
    if (!c) {
        return NULL;
    }
    child_reason(*c, why, size);
    return why;
}

// game.h's controller_missing(), read from the Lua thread.
bool controller_missing_guarded(int n) {
    if (n < 0 || n >= g_engine.inv.count || g_engine.inv.names[n].row_count == 0) {
        return false;
    }
    uint8_t** slot = g_engine.game.slot[g_engine.inv.names[n].rows[0]];
    return slot && readable(slot, 4) && !*slot;
}

// A route's command: kOpOpenRoute on window n with the route's values.
Command route_command(int n, const RouteArgs& args, const HuEngineHandle* ref) {
    Command c = command(kOpOpenRoute, n, args.x, args.y, ref, args.value, kVerbOpen);
    return c;
}

// ability and magic, the windows whose open takes a list.
bool list_window(int n) {
    return n >= 0 && (n == g_engine.inv.ability || n == g_engine.inv.find_exact("magic"));
}

// `abi`: the engine abi of the slot. From 3 on it takes a window's name
// alone and open refuses the prompts that abi counts (core.h's kPrompts);
// from 4 on close refuses those cancel() ends too, pointing at it: a
// hide-only one here, any other on the game thread while it asks
// (kCmdPrompt), as only the game's state says whether it does. Every
// slot's close refuses a hide-only or close-refused window with its own
// reason, and every slot's open the windows core.h's kOpenRefused
// gives that abi and a window whose registry slot holds no controller
// (game.h's controller_missing()); from 9 on open takes ability and magic
// only with a list.
int32_t name_command(const HuEngineHandle* ref, const char* name, uint8_t op, uint8_t verb, uint32_t abi,
                     char* why, uint32_t size) {
    HU_STEP_AT("name_command", op);
    bool ok = false;
    lock();
    Handle* h = usable(ref, why, size);
    if (h) {
        const int n = lookup(name, abi >= 3, why, size);
        if (n >= 0) {
            const char* nm = g_engine.inv.names[n].name;
            if ((op == kOpOpen || op == kOpClose) && synthetic(g_engine, n)) {
                // No window to open or close: close is this handle's hide
                // hold and open drops it, here; nothing queued, no event.
                g_engine.holds.set(*h, n, kHoldHide, op == kOpClose);
                ok = true;
            } else if (op == kOpOpen && (g_engine.holds.want[n] & kWantBlocked)) {
                snprintf(why, size, "%s is blocked", nm);
            } else if (op == kOpOpen && abi >= 3 && prompt_window(nm, abi)) {
                snprintf(why, size, "%s opens only for the game's own event; %s it instead", nm,
                    prompt_cancels(nm) ? "answer or cancel" : "answer");
            } else if (op == kOpOpen && open_refusal(nm, abi)) {
                snprintf(why, size, "%s", open_refusal(nm, abi));
            } else if (op == kOpOpen && child_refusal(nm, why, size)) {
                // why filled
            } else if (op == kOpOpen && controller_missing_guarded(n)) {
                snprintf(why, size, "%s has no controller now, and the game's open would crash the client", nm);
            } else if (op == kOpOpen && route_spec(nm)) {
                OpenArg none;
                parse_open_arg(NULL, &none);
                RouteArgs args;
                if (route_args(*route_spec(nm), none, &args, why, size)) {
                    ok = enqueue(route_command(n, args, ref), why, size);
                }
            } else if (op == kOpClose && pair_refusal(kCloseWhileOpen, kCloseWhileOpenCount, nm, true)) {
                snprintf(why, size, "%s", pair_refusal(kCloseWhileOpen, kCloseWhileOpenCount, nm, true));
            } else if (op == kOpOpen && abi >= 9 && list_window(n)) {
                snprintf(why, size, "%s opens one list at a time: name the list, as in open('%s', '%s')", nm, nm,
                    n == g_engine.inv.ability ? kAbilityLists[0].name : kMagicLists[0].name);
            } else if (op == kOpClose && abi >= 4 && prompt_window(nm, abi) && prompt_cancels(nm)
                       && close_refusal(nm)) {
                snprintf(why, size, "%s is a prompt the game waits on; use cancel('%s')", nm, nm);
            } else if (op == kOpClose && close_refusal(nm)) {
                snprintf(why, size, "%s", close_refusal(nm));
            } else {
                Command c = command(op, n, 0, 0, ref, 0, verb);
                if (op == kOpClose && abi >= 4 && prompt_window(nm, abi) && prompt_cancels(nm)) {
                    c.flags |= kCmdPrompt;
                }
                ok = enqueue(c, why, size);
            }
        }
    }
    unlock();
    return ok;
}

int32_t __stdcall op_reset(const HuEngineHandle* ref, const char* name, char* why, uint32_t size) {
    return name_command(ref, name, kOpReset, kVerbReset, 1, why, size);
}

// A block from engine abi 5 on: the hold, and the game's own close of the
// window when it is open now, queued with it; an open prompt the slot's
// engine abi `abi` counts is left to its reply. A full queue refuses the
// block whole.
int32_t block_window(const HuEngineHandle* ref, const char* name, uint32_t abi, char* why, uint32_t size) {
    bool ok = false;
    lock();
    Handle* h = usable(ref, why, size);
    const int n = h ? window_index(name, why, size) : -1;
    if (n >= 0) {
        const char* nm = g_engine.inv.names[n].name;
        if (block_refusal(nm)) {
            snprintf(why, size, "%s", block_refusal(nm));
        } else if (synthetic(g_engine, n) || prompt_window(nm, abi) || !open_menu_guarded(n, NULL)
                   || enqueue(command(kOpBlockClose, n, 0, 0, ref, 0, kVerbBlock), why, size)) {
            // The compass and the target arrows have no window to close:
            // their block is the hidden want bit their draw hooks read, and
            // no `blocked` event ever follows, the game never attempts an
            // open of them.
            g_engine.holds.set(*h, n, kHoldBlock, true);
            ok = true;
        }
    }
    unlock();
    return ok;
}

int32_t __stdcall op_block5(const HuEngineHandle* ref, const char* name, char* why, uint32_t size) {
    return block_window(ref, name, 5, why, size);
}

// The lists of ability as block7 and unblock7 name them, as a bit mask: a
// list's name (core.h's kAbilityLists) takes its category; a number
// 1..kCategoryBits-1 in digits takes that one. 0 with `why` filled for
// anything else.
uint32_t ability_categories(const char* text, char* why, size_t size) {
    char low[24];
    if (text && strlen(text) < sizeof(low)) {
        for (size_t i = 0; i <= strlen(text); ++i) {
            low[i] = lower_ascii(text[i]);
        }
        for (int i = 0; i < kAbilityListCount; ++i) {
            if (strcmp(kAbilityLists[i].name, low) == 0) {
                return 1u << kAbilityLists[i].category;
            }
        }
        int value = 0;
        bool digits = low[0] != '\0';
        for (const char* d = low; *d && digits; ++d) {
            digits = *d >= '0' && *d <= '9';
            value = value * 10 + (*d - '0');
            digits = digits && value < kCategoryBits;
        }
        if (digits && value >= 1) {
            return 1u << value;
        }
    }
    snprintf(why, size, "no such category: %s (a list's name, such as job_abilities or pet_commands, or a number"
        " 1..31)", text ? text : "(not a string)");
    return 0;
}

// With a category, engine abi 7's block: the hold on that list of ability
// alone, and the game's own close of the window when it shows that list
// now, queued with it as block5 queues the whole window's. A full queue
// refuses the block whole.
int32_t block_category(const HuEngineHandle* ref, const char* name, const char* category, char* why,
                       uint32_t size) {
    bool ok = false;
    lock();
    Handle* h = usable(ref, why, size);
    const int n = h ? window_index(name, why, size) : -1;
    if (n >= 0) {
        if (n != g_engine.inv.ability) {
            snprintf(why, size, "only ability has categories");
        } else {
            const uint32_t mask = ability_categories(category, why, size);
            uint8_t* ctl = NULL;
            const int32_t shown = open_menu_guarded(n, &ctl) && readable(ctl, kAbilityCategory + 4)
                ? static_cast<int32_t>(rd32(ctl, kAbilityCategory)) : -1;
            const bool showing = shown >= 0 && shown < kCategoryBits && (mask & (1u << shown));
            if (mask && (!showing || enqueue(command(kOpBlockClose, n, 0, 0, ref, 0, kVerbBlock), why, size))) {
                for (int c = 1; c < kCategoryBits; ++c) {
                    if (mask & (1u << c)) {
                        g_engine.holds.set_ability(*h, c, true);
                    }
                }
                ok = true;
            }
        }
    }
    unlock();
    return ok;
}

// Engine abi 7's block: with a category, block_category; without one,
// block5.
int32_t __stdcall op_block7(const HuEngineHandle* ref, const char* name, const char* category, char* why,
                            uint32_t size) {
    if (!category || !category[0]) {
        return op_block5(ref, name, why, size);
    }
    return block_category(ref, name, category, why, size);
}

// Engine abi 8's block: abi 7's, and without a category the yes/no windows
// among the prompts it leaves open.
int32_t __stdcall op_block8(const HuEngineHandle* ref, const char* name, const char* category, char* why,
                            uint32_t size) {
    if (!category || !category[0]) {
        return block_window(ref, name, 8, why, size);
    }
    return block_category(ref, name, category, why, size);
}

// Engine abi 7's unblock: with a category, that list's hold dropped and
// nothing else; without one, unblock3, the whole-window hold alone.
int32_t __stdcall op_unblock7(const HuEngineHandle* ref, const char* name, const char* category, char* why,
                              uint32_t size) {
    if (!category || !category[0]) {
        return op_unblock3(ref, name, why, size);
    }
    bool ok = false;
    lock();
    Handle* h = usable(ref, why, size);
    const int n = h ? window_index(name, why, size) : -1;
    if (n >= 0) {
        if (n != g_engine.inv.ability) {
            snprintf(why, size, "only ability has categories");
        } else {
            const uint32_t mask = ability_categories(category, why, size);
            if (mask) {
                for (int c = 1; c < kCategoryBits; ++c) {
                    if (mask & (1u << c)) {
                        g_engine.holds.set_ability(*h, c, false);
                    }
                }
                ok = true;
            }
        }
    }
    unlock();
    return ok;
}

int32_t __stdcall op_open(const HuEngineHandle* ref, const char* name, char* why, uint32_t size) {
    return name_command(ref, name, kOpOpen, kVerbOpen, 1, why, size);
}

// One list of ability or magic, n, by its name or its number in the
// window's table (core.h's kAbilityLists, kMagicLists): the category, or
// -1 with `why` filled. The game checks no category, and one past a
// window's table writes past its controller.
int32_t window_list(int n, const char* text, char* why, size_t size) {
    const bool ability = n == g_engine.inv.ability;
    char low[24];
    const size_t length = text ? strlen(text) : 0;
    if (text && length < sizeof(low)) {
        for (size_t i = 0; i <= length; ++i) {
            low[i] = lower_ascii(text[i]);
        }
        const int32_t category = ability ? list_category(kAbilityLists, kAbilityListCount, low)
                                         : list_category(kMagicLists, kMagicListCount, low);
        if (category >= 0) {
            return category;
        }
    }
    snprintf(why, size, "no such list of %s: %s (%s)", ability ? "ability" : "magic", text ? text : "(not a string)",
        ability ? "a list's name, such as job_abilities or pet_commands, or its number, 1 to 23 but 5 and 7"
                : "a list's name, such as white_magic or songs, or its number, 1 to 8");
    return -1;
}

// Under the lock: why list `category` of window n cannot open now, or NULL.
const char* list_refusal(int n, int32_t category, char* why, size_t size) {
    const char* nm = g_engine.inv.names[n].name;
    if (g_engine.holds.want[n] & kWantBlocked) {
        snprintf(why, size, "%s is blocked", nm);
        return why;
    }
    if (n == g_engine.inv.ability && category < kCategoryBits && (g_engine.holds.ability_want & (1L << category))) {
        snprintf(why, size, "%s's list %s is blocked", nm, ability_list_name(category));
        return why;
    }
    return NULL;
}

// Engine abi 9's open: with no argument, open8's, also refusing the windows
// core.h's kOpenRefused gives abi 9 and the windows of kRoutes that need
// one, and running the route of the rest; with one, the list of ability or
// magic alone through the game's own opener of it, with no sort window or
// detail pane, or the route of a window of kRoutes with what it takes. A
// window that takes an argument is refused up front as its plain open is.
int32_t __stdcall op_open9(const HuEngineHandle* ref, const char* name, const char* list, char* why,
                           uint32_t size) {
    if (!list || !list[0]) {
        return name_command(ref, name, kOpOpen, kVerbOpen, 9, why, size);
    }
    bool ok = false;
    lock();
    Handle* h = usable(ref, why, size);
    const int n = h ? window_index(name, why, size) : -1;
    if (n >= 0) {
        const char* nm = g_engine.inv.names[n].name;
        const RouteSpec* spec = route_spec(nm);
        OpenArg arg;
        RouteArgs args;
        if (list_window(n)) {
            const int32_t category = window_list(n, list, why, size);
            if (category >= 0 && !list_refusal(n, category, why, size)) {
                ok = enqueue(command(kOpOpenList, n, 0, 0, ref, static_cast<uint32_t>(category), kVerbOpen), why,
                    size);
            }
        } else if (!spec) {
            snprintf(why, size, "%s takes no argument: only ability, magic and the windows the README lists as"
                " taking one do", nm);
        } else if (!parse_open_arg(list, &arg)) {
            snprintf(why, size, "%s: the argument is not one hideui reads (%s)", nm, list);
        } else if (g_engine.holds.want[n] & kWantBlocked) {
            snprintf(why, size, "%s is blocked", nm);
        } else if (child_refusal(nm, why, size)) {
            // why filled
        } else if (controller_missing_guarded(n)) {
            snprintf(why, size, "%s has no controller now, and the game's open would crash the client", nm);
        } else if (route_args(*spec, arg, &args, why, size)) {
            ok = enqueue(route_command(n, args, ref), why, size);
        }
    }
    unlock();
    return ok;
}

// An inventory index in digits, 1..kInventoryRecords - 1, for a set that
// opens for an item; -1 with `why` filled.
int32_t set_item_index(const SetSpec& spec, const char* text, char* why, size_t size) {
    int32_t value = 0;
    bool digits = text && text[0];
    for (const char* d = text; digits && *d; ++d) {
        digits = *d >= '0' && *d <= '9' && value < kInventoryRecords;
        value = value * 10 + (*d - '0');
    }
    if (digits && value >= 1 && value < kInventoryRecords) {
        return value;
    }
    snprintf(why, size, "no such inventory index: %s (the %s's index in the inventory, 1 to %d)",
        text ? text : "(not a string)", strcmp(spec.name, "synthesis") == 0 ? "crystal" : "Maze Tabula",
        kInventoryRecords - 1);
    return -1;
}

// Engine abi 9's open_set: the windows of a set (core.h's kSets) opened
// together by the game's own opener of them, the ability and magic sets
// with their list, synthesis and tabula with the inventory index of their
// crystal or tabula, compare with an item, region and text_colors with
// their choice. Refused up front when a window of the set, or the list, is
// blocked; for blue magic the sort window is blusortw, and for region the
// kind's points window is one of the set's.
int32_t __stdcall op_open_set9(const HuEngineHandle* ref, const char* set, const char* list, char* why,
                               uint32_t size) {
    bool ok = false;
    lock();
    if (usable(ref, why, size)) {
        char low[16];
        const int s = Inventory::lower_name(set, low) ? find_set(low) : -1;
        const bool given = list && list[0];
        OpenArg arg;
        if (s < 0) {
            char names[256];
            names[0] = '\0';
            for (int k = 0; k < kSetCount; ++k) {
                const size_t used = strlen(names);
                snprintf(names + used, sizeof(names) - used, "%s%s", k == 0 ? "" : k + 1 == kSetCount ? " or " : ", ",
                    kSets[k].name);
            }
            snprintf(why, size, "no such set: %s (%s)", set ? set : "(not a string)", names);
        } else if (kSets[s].takes == kTakesNothing && given) {
            snprintf(why, size, "the %s set takes no argument", kSets[s].name);
        } else if (kSets[s].takes == kTakesList && !given) {
            snprintf(why, size, "the %s set opens one list: name it, as in open_set('%s', '%s')", kSets[s].name,
                kSets[s].name, s == kSetAbility ? kAbilityLists[0].name : kMagicLists[0].name);
        } else if (kSets[s].takes == kTakesItem && !given) {
            snprintf(why, size, "the %s set opens for one %s: give its index in the inventory, as in open_set('%s', 5)",
                kSets[s].name, s == kSetSynthesis ? "crystal" : "Maze Tabula", kSets[s].name);
        } else if (kSets[s].takes == kTakesItemPanel && !given) {
            snprintf(why, size, "the %s set opens for one item: open_set('%s', {bag = 'inventory', slot = 5}) or"
                " open_set('%s', {item = 16555})", kSets[s].name, kSets[s].name, kSets[s].name);
        } else if (kSets[s].takes == kTakesChoice && !given) {
            char names[128];
            choice_names(kSets[s].choices, kSets[s].choice_count, names, sizeof(names));
            snprintf(why, size, "the %s set opens for one %s: %s", kSets[s].name, kSets[s].what, names);
        } else if (given && !parse_open_arg(list, &arg)) {
            snprintf(why, size, "the %s set: the argument is not one hideui reads (%s)", kSets[s].name, list);
        } else {
            const SetSpec& spec = kSets[s];
            const int n = g_engine.inv.find_exact(spec.opens);
            RouteArgs item;
            memset(&item, 0, sizeof(item));
            int32_t value = 0;
            if (spec.takes == kTakesList) {
                value = window_list(n, list, why, size);
            } else if (spec.takes == kTakesItem) {
                value = set_item_index(spec, list, why, size);
            } else if (spec.takes == kTakesItemPanel) {
                value = parse_item_arg(spec.name, arg, &item, why, size) ? static_cast<int32_t>(item.value) : -1;
            } else if (spec.takes == kTakesChoice) {
                value = arg.table ? -1 : arg_choice(spec.choices, spec.choice_count, arg.value[0]);
                if (value < 0) {
                    char names[128];
                    choice_names(spec.choices, spec.choice_count, names, sizeof(names));
                    snprintf(why, size, "no such %s of the %s set: %s (%s)", spec.what, spec.name, list, names);
                }
            }
            bool refused = value < 0;
            for (int i = 0; !refused && spec.windows[i]; ++i) {
                const char* member = s == kSetMagic && value == kBlueMagic && strcmp(spec.windows[i], "mgcsortw") == 0
                    ? "blusortw" : spec.windows[i];
                const int m = g_engine.inv.find_exact(member);
                if (m >= 0 && (g_engine.holds.want[m] & kWantBlocked)) {
                    snprintf(why, size, "%s is blocked; the %s set opens it", member, spec.name);
                    refused = true;
                }
            }
            if (!refused && spec.by_choice) {
                const char* member = spec.by_choice[value - 1];
                const int m = g_engine.inv.find_exact(member);
                if (m >= 0 && (g_engine.holds.want[m] & kWantBlocked)) {
                    snprintf(why, size, "%s is blocked; the %s set opens it", member, spec.name);
                    refused = true;
                }
            }
            refused = refused || (spec.takes == kTakesList && list_refusal(n, value, why, size));
            refused = refused || (spec.takes == kTakesItemPanel && child_refusal(spec.opens, why, size) != NULL);
            if (!refused) {
                Command c = command(kOpOpenSet, n, s, item.y, ref, static_cast<uint32_t>(value), kVerbOpenSet);
                c.z = item.x;
                ok = enqueue(c, why, size);
            }
        }
    }
    unlock();
    return ok;
}

int32_t __stdcall op_close(const HuEngineHandle* ref, const char* name, char* why, uint32_t size) {
    return name_command(ref, name, kOpClose, kVerbClose, 1, why, size);
}

int32_t __stdcall op_reset3(const HuEngineHandle* ref, const char* name, char* why, uint32_t size) {
    return name_command(ref, name, kOpReset, kVerbReset, 3, why, size);
}

int32_t __stdcall op_open3(const HuEngineHandle* ref, const char* name, char* why, uint32_t size) {
    return name_command(ref, name, kOpOpen, kVerbOpen, 3, why, size);
}

int32_t __stdcall op_close3(const HuEngineHandle* ref, const char* name, char* why, uint32_t size) {
    return name_command(ref, name, kOpClose, kVerbClose, 3, why, size);
}

int32_t __stdcall op_close4(const HuEngineHandle* ref, const char* name, char* why, uint32_t size) {
    return name_command(ref, name, kOpClose, kVerbClose, 4, why, size);
}

int32_t __stdcall op_open8(const HuEngineHandle* ref, const char* name, char* why, uint32_t size) {
    return name_command(ref, name, kOpOpen, kVerbOpen, 8, why, size);
}

int32_t __stdcall op_close8(const HuEngineHandle* ref, const char* name, char* why, uint32_t size) {
    return name_command(ref, name, kOpClose, kVerbClose, 8, why, size);
}

int32_t __stdcall op_reset_all(const HuEngineHandle* ref, char* why, uint32_t size) {
    bool ok = false;
    lock();
    if (usable(ref, why, size)) {
        ok = enqueue(command(kOpResetOwned, 0, 0, 0, ref, 0, kVerbResetAll), why, size);
    }
    unlock();
    return ok;
}

// The macro keys blocked: the hold on this handle, and with the first hold
// in the client, the close of a bar the handler has up, queued with it. A
// full queue refuses the block whole.
int32_t __stdcall op_block_macros(const HuEngineHandle* ref, char* why, uint32_t size) {
    bool ok = false;
    lock();
    Handle* h = usable(ref, why, size);
    if (h) {
        const bool first = !h->macros_hold && g_engine.holds.macros_count == 0;
        if (!first || enqueue(command(kOpMacrosBlocked, -1, 0, 0, ref, 0, kVerbBlockMacros), why, size)) {
            g_engine.holds.set_macros(*h, true);
            ok = true;
        }
    }
    unlock();
    return ok;
}

int32_t __stdcall op_unblock_macros(const HuEngineHandle* ref, char* why, uint32_t size) {
    bool ok = false;
    lock();
    Handle* h = usable(ref, why, size);
    if (h) {
        g_engine.holds.set_macros(*h, false);
        ok = true;
    }
    unlock();
    return ok;
}

bool written_by(int32_t slot, uint32_t gen, const HuEngineHandle* ref);
void owner_name(int32_t slot, uint32_t gen, char* out, size_t size);

// Under the lock: "<what> was placed by <owner>" into `why`, the owner the
// handle another than `ref` that wrote slot, gen.
void placed_by(const char* what, int32_t slot, uint32_t gen, char* why, size_t size) {
    char owner[48];
    owner_name(slot, gen, owner, sizeof(owner));
    snprintf(why, size, "%s was placed by %s", what, owner[0] ? owner : "another handle");
}

// Engine abi 5's reset(name[, aspect]): only what this handle placed, the
// position and the size, or the one aspect named. Refused whole when
// another handle placed an aspect it would reset; one nobody placed is
// left as it is.
int32_t __stdcall op_reset5(const HuEngineHandle* ref, const char* name, const char* aspect, char* why,
                            uint32_t size) {
    uint32_t aspects = kAspectPosition | kAspectSize;
    if (aspect && aspect[0]) {
        if (strcmp(aspect, "position") == 0) {
            aspects = kAspectPosition;
        } else if (strcmp(aspect, "size") == 0) {
            aspects = kAspectSize;
        } else {
            snprintf(why, size, "reset(name[, aspect]): aspect is 'position' or 'size'");
            return -1;
        }
    }
    bool ok = false;
    lock();
    const int n = usable(ref, why, size) ? window_index(name, why, size) : -1;
    if (n >= 0) {
        const char* nm = g_engine.inv.names[n].name;
        MemEntry m;
        if (!g_engine.memory.read(n, &m)) {
            snprintf(why, size, "the game thread is placing %s; try again", nm);
        } else if ((aspects & kAspectPosition) && m.active && !written_by(m.owner_slot, m.owner_gen, ref)) {
            placed_by(nm, m.owner_slot, m.owner_gen, why, size);
        } else if ((aspects & kAspectSize) && m.size_kind && !written_by(m.size_slot, m.size_gen, ref)) {
            placed_by(nm, m.size_slot, m.size_gen, why, size);
        } else {
            ok = enqueue(command(kOpResetMine, n, 0, 0, ref, aspects, kVerbReset), why, size);
        }
    }
    unlock();
    return ok;
}

// Engine abi 5's reset_group(group): this handle's move of the group, the
// one waiting for its anchor or every position it wrote. Refused while
// another handle's move of the group waits, or placed the anchor.
int32_t __stdcall op_reset_group5(const HuEngineHandle* ref, const char* group, char* why, uint32_t size) {
    bool ok = false;
    lock();
    if (usable(ref, why, size)) {
        const int g = find_group_any(group);
        GroupWait wait;
        MemEntry a;
        const int anchor = g >= 0 ? g_engine.inv.find_exact(kGroups[g].anchor) : -1;
        if (g < 0) {
            snprintf(why, size, "no such group: %s (chat_log, party_list or target_window)",
                group ? group : "(not a string)");
        } else if (!g_engine.waiting.read(g, &wait) || !g_engine.memory.read(anchor, &a)) {
            snprintf(why, size, "the game thread is placing %s; try again", kGroups[g].name);
        } else if (wait.active && !written_by(wait.slot, wait.gen, ref)) {
            placed_by(kGroups[g].name, wait.slot, wait.gen, why, size);
        } else if (a.active && a.group == g + 1 && !written_by(a.owner_slot, a.owner_gen, ref)) {
            placed_by(kGroups[g].name, a.owner_slot, a.owner_gen, why, size);
        } else {
            ok = enqueue(command(kOpResetGroupMine, g, 0, 0, ref, 0, kVerbResetGroup), why, size);
        }
    }
    unlock();
    return ok;
}

// ---------------------------------------------------------------------------
// Replies. Each check runs under the lock, reads the game from this thread
// with every pointer guarded, and fills `why` when the reply cannot go
// ahead; the drain checks again on the game thread. `prefix` is the engine
// abi 1 or 2 verb its refusals start with, empty for abi 3.

int window_named(const char* nm) {
    return g_engine.inv.find_exact(nm);
}

// The open choice's controller, or NULL with `why` filled.
uint8_t* query_open(const char* prefix, char* why, size_t size) {
    const int n = window_named("query");
    uint8_t* ctl = NULL;
    if (n < 0 || !open_menu_guarded(n, &ctl) || !readable(ctl, kQueryResult + 2)) {
        refusal(why, size, prefix, "query is not open");
        return NULL;
    }
    return ctl;
}

// The byte query's input handler tests before it writes the cancel answer;
// the game buzzes instead while it is not 1.
bool query_cancellable() {
    return *static_cast<const volatile uint8_t*>(g_engine.game.query_cancel_allowed) == 1;
}

// game.h's query_index_of() with every hop guarded: true when an option
// the player can pick has this value.
bool query_lists_guarded(const uint8_t* ctl, int value) {
    const uint8_t* node = rdptr(ctl, kQueryOptions);
    for (int steps = 0; node && steps < kQueryListMax && readable(node, 0x15); ++steps) {
        const uint8_t* item = rdptr(node, 0x10);
        if (node[0x14] == 0 && readable(item, kOptionBytes)
            && static_cast<uint16_t>(rd16(item, kOptionValue)) == value) {
            return true;
        }
        node = rdptr(node, 0);
    }
    return false;
}

// The pending entry's controller, or NULL with `why` filled.
uint8_t* passinpu_pending(const char* prefix, char* why, size_t size) {
    uint8_t* ctl = controller_guarded("passinpu", kPassContext + 4);
    if (!passinpu_asks(ctl)) {
        refusal(why, size, prefix, "no text entry is pending");
        return NULL;
    }
    return ctl;
}

bool passinpu_fits(const uint8_t* ctl, size_t length, const char* prefix, char* why, size_t size) {
    const int max = static_cast<int>(rd32(ctl, kPassMax));
    if (static_cast<int>(length) > max) {
        refusal(why, size, prefix, "%u bytes, the entry takes at most %d", static_cast<unsigned>(length), max);
        return false;
    }
    return true;
}

bool party_pending(const char* prefix, char* why, size_t size) {
    if (!*static_cast<const volatile uint8_t*>(g_engine.game.party_pending)) {
        refusal(why, size, prefix, "no party invite is pending");
        return false;
    }
    return true;
}

// A controller's window instance with its bytes readable, or NULL: what the
// post-box replies take for the box's window, open, closing or covered.
const uint8_t* window_of(const uint8_t* ctl) {
    const uint8_t* menu = ctl ? rdptr(ctl, kCtlMenu) : NULL;
    return menu && readable(menu, kMenuBytes) ? menu : NULL;
}

// game.h's post_close() checks with every hop guarded.
bool post_open(const char* nm, const char* prefix, char* why, size_t size) {
    if (strcmp(nm, "delivery") == 0) {
        const uint8_t* ctl = controller_guarded("delivery", kDeliveryNext + 2);
        const uint8_t* menu = window_of(ctl);
        const char* reason = menu ? delivery_row_refusal(ctl, menu) : NULL;
        const int state = ctl ? rd16(ctl, kDeliveryState) : 0;
        if (reason) {
            refusal(why, size, prefix, "%s", reason);
            return false;
        }
        if (menu) {
            return true;
        }
        if (state == 0) {
            refusal(why, size, prefix, "no outgoing post-box session is open");
            return false;
        }
        if (state == kDeliveryClosing) {
            refusal(why, size, prefix, "its close is already waiting for the server");
            return false;
        }
        return true;
    }
    if (live_guarded("post1") || live_guarded("post2")) {
        refusal(why, size, prefix, "%s", open_guarded("post1") || open_guarded("post2") ? kBoxOpen : kBoxClosing);
        return false;
    }
    const uint8_t* ctl = controller_guarded("post1", kPostState + 4);
    const uint32_t state = ctl ? rd32(ctl, kPostState) : 0;
    if (state == 0) {
        refusal(why, size, prefix, "no incoming post-box session is open");
        return false;
    }
    if (state == kPostClosing) {
        refusal(why, size, prefix, "its close is already waiting for the server");
        return false;
    }
    return true;
}

// game.h's link5_pending() with every hop guarded: link5's controller, or
// NULL with `why` filled.
const uint8_t* link5_pending(const char* prefix, char* why, size_t size) {
    const uint8_t* ctl = controller_guarded("link5", kLink5Callback + 4);
    const bool pending = ctl && *static_cast<const volatile uint8_t*>(g_engine.game.link5_latch) == 1
        && rd32(ctl, kLink5Mode) == 1 && rdptr(ctl, kLink5Callback) == g_engine.game.link5_callback;
    if (!pending) {
        refusal(why, size, prefix, "no linkshell choice is pending");
        return NULL;
    }
    return ctl;
}

// The concierge cache slot a row's record is, or -1.
int link5_slot_of(const uint8_t* record) {
    const uint8_t* cache = g_engine.game.link5_cache;
    if (record < cache || record >= cache + kLink5Slots * kLink5Entry) {
        return -1;
    }
    const size_t at = static_cast<size_t>(record - cache);
    return at % kLink5Entry == 0 ? static_cast<int>(at / kLink5Entry) : -1;
}

// link5's rows past the header and its name buffer, guarded; 0 rows when
// either is unreadable.
int link5_rows(const uint8_t* ctl, const uint8_t** rows, const uint8_t** names) {
    *rows = rdptr(ctl, kLink5Rows);
    *names = rdptr(ctl, kLink5Names);
    const int count = static_cast<int>(rd32(ctl, kLink5Count));
    if (count <= 1 || count > 256 || !readable(*rows, count * kLink5RowStride)) {
        return 0;
    }
    if (*names && !readable(*names, count * kLink5NameStride)) {
        *names = NULL;
    }
    return count;
}

// game.h's link5_lists() with every hop guarded.
bool link5_lists_guarded(const uint8_t* ctl, int slot) {
    const uint8_t* rows = NULL;
    const uint8_t* names = NULL;
    const int count = link5_rows(ctl, &rows, &names);
    for (int i = 1; i < count; ++i) {
        if (link5_slot_of(rdptr(rows + i * kLink5RowStride, kLink5RowRecord)) == slot) {
            return true;
        }
    }
    return false;
}

// arealist's controller while its list is open and its latch set, which
// every open sets: what options() reads; NULL with `why` filled.
const uint8_t* area_pending(const char* prefix, char* why, size_t size) {
    const int n = window_named("arealist");
    uint8_t* ctl = NULL;
    if (n < 0 || !open_menu_guarded(n, &ctl) || !readable(ctl, kAreaBytes) || !ctl[kAreaLatch]) {
        refusal(why, size, prefix, "no area choice is pending");
        return NULL;
    }
    return ctl;
}

// arealist's controller while an NPC event's choice waits with no window
// instance to show it (its open refused): the latch set in mode 1 or 2, as
// the event's opener sets them before the open. NULL otherwise.
const uint8_t* area_blocked() {
    const uint8_t* ctl = controller_guarded("arealist", kAreaBytes);
    return !area_refusal(ctl) && !rdptr(ctl, kCtlMenu) ? ctl : NULL;
}

// arealist's rows, guarded: game.h's area_count() of them with the array
// readable, else 0. Every rebuild moves the array, so it is read each time.
int area_rows_guarded(const uint8_t* ctl, const uint8_t** rows) {
    *rows = rdptr(ctl, kAreaRows);
    const int count = area_count(ctl);
    return count && readable(*rows, count * kAreaRowStride) ? count : 0;
}

// Row i's id, from its entry; false when the entry is unreadable, which ends
// the rows a read takes.
bool area_id_guarded(const uint8_t* rows, int i, int* id) {
    const uint8_t* entry = rdptr(rows + i * kAreaRowStride, kAreaRowEntry);
    if (!readable(entry, 4)) {
        return false;
    }
    *id = static_cast<int32_t>(rd32(entry, 0));
    return true;
}

// game.h's area_row_of() with every hop guarded: true when a row holds `id`.
bool area_lists_guarded(const uint8_t* ctl, int id) {
    const uint8_t* rows = NULL;
    const int count = area_rows_guarded(ctl, &rows);
    int at = 0;
    for (int i = 0; i < count && area_id_guarded(rows, i, &at); ++i) {
        if (at == id) {
            return true;
        }
    }
    return false;
}

// game.h's area_event_pending() with every hop guarded: an NPC event's
// choice, the window open (`*window`) or blocked; NULL with `why` filled.
const uint8_t* area_event_guarded(const char* prefix, char* why, size_t size, bool* window) {
    const uint8_t* ctl = controller_guarded("arealist", kAreaBytes);
    const char* reason = area_refusal(ctl);
    if (reason) {
        refusal(why, size, prefix, "%s", reason);
        return NULL;
    }
    *window = open_guarded("arealist");
    return ctl;
}

// The zone an answer to that choice names: one of the open list's zone rows,
// or with the list blocked, any zone id.
bool area_zone_ok(const uint8_t* ctl, bool window, int zone, const char* prefix, char* why, size_t size) {
    if (!window && (zone < 0 || zone > kZoneMax)) {
        refusal(why, size, prefix, "the area list is blocked, so there are no rows to check %d against; the answer"
            " takes a zone id, 0..%d", zone, kZoneMax);
        return false;
    }
    if (window && !area_lists_guarded(ctl, zone)) {
        refusal(why, size, prefix, "no row of the list has the id %d (options('arealist') lists them)", zone);
        return false;
    }
    if (window && zone < 1) {
        refusal(why, size, prefix, "the row with the id %d is not a zone; the answer takes a zone row's id", zone);
        return false;
    }
    return true;
}

// game.h's yesno_pending() with every hop guarded: window n's yes/no
// controller while it asks, or NULL with `why` filled.
const uint8_t* yesno_pending_guarded(int n, char* why, size_t size) {
    const uint8_t* ctl = controller_guarded(g_engine.inv.names[n].name, kYesNoBytes);
    return yesno_asks(g_engine, n, ctl, why, size) ? ctl : NULL;
}

// A number box's question as a reply or a read finds it, every hop
// guarded: its question kept from its window while its record stands
// (game.h's box_record_gone), else its window up with its callback set.
struct BoxQuestion {
    int box;
    bool blocked;
    const uint8_t* ctl;
    int asker;
    uint32_t id;
    BoxRecord record;           // blocked: the record, and its asker's window and controller now
    const uint8_t* owner;
    const uint8_t* owner_ctl;
};

bool box_question_guarded(int b, BoxQuestion* q, char* why, size_t size) {
    memset(q, 0, sizeof(*q));
    q->box = b;
    q->asker = -1;
    const char* nm = kBoxNames[b];
    const uint8_t* ctl = controller_guarded(nm, kBoxLayouts[b].bytes);
    BoxRecord r;
    if (!ctl || !g_engine.boxes[b].read(&r)) {
        memset(&r, 0, sizeof(r));
    }
    const AskerSpec* a = r.active && r.asker >= 0 ? &kAskers[r.asker] : NULL;
    const uint8_t* owner = r.active ? open_menu_guarded(r.owner_n, NULL) : NULL;
    const uint8_t* owner_ctl = a ? controller_guarded(a->owner, static_cast<size_t>(a->field) + a->field_bytes)
                                 : NULL;
    const int n = g_engine.inv.box_window[b];
    if (!box_asks(r, b, ctl, owner, owner_ctl, open_menu_guarded(n, NULL) != NULL, &q->blocked)) {
        snprintf(why, size, "no number is pending in %s", nm);
        return false;
    }
    q->ctl = ctl;
    if (q->blocked) {
        q->asker = r.asker;
        q->id = r.id;
        q->record = r;
        q->owner = owner;
        q->owner_ctl = owner_ctl;
    } else {
        q->asker = asker_of(g_engine, box_callback(ctl, b));
        q->id = static_cast<uint32_t>(g_engine.instance[n]);
    }
    return true;
}

// game.h's box_blocked_refusal() or box_window_refusal(), for the question
// read.
const char* box_refusal_guarded(const BoxQuestion& q, bool answer, char* buf, size_t size) {
    if (q.blocked) {
        return box_blocked_refusal(q.record, q.box, q.owner, q.owner_ctl, answer, buf, size);
    }
    const bool owner_open = q.asker >= 0 && open_guarded(kAskers[q.asker].owner);
    return box_window_refusal(q.asker, owner_open, answer, buf, size);
}

// An Asked window's question as a reply or a read finds it, every hop
// guarded (game.h's asked_now() and asked_pending()): its record's copy, the
// window as it stands, whether a block kept it from its window, and the
// prompt's id.
struct AskedQuestion {
    int asked;
    bool blocked;
    uint32_t id;
    AskedNow now;
    AskedRecord record;
};

bool asked_question_guarded(int a, AskedQuestion* q, char* why, size_t size) {
    memset(q, 0, sizeof(*q));
    q->asked = a;
    const int n = g_engine.inv.asked_window[a];
    AskedRecord& r = q->record;
    if (!asked_records(a) || !g_engine.asked[a].read(&r)) {
        memset(&r, 0, sizeof(r));
    }
    AskedNow& now = q->now;
    now.ctl = controller_guarded(kAskedNames[a], asked_size(a));
    now.menu = now.ctl ? open_menu_guarded(n, NULL) : NULL;
    if (r.active && r.owner_n >= 0) {
        now.owner = open_menu_guarded(r.owner_n, NULL);
    }
    if (a == kAskedEvitem) {
        now.list = controller_guarded("evitem", kEvitemListBytes);
    }
    if (a == kAskedBank) {
        now.safe2 = bank_safe2(g_engine);
    }
    if (a == kAskedDoor) {
        door_now(g_engine, &now);
    }
    char buf[kWhyBytes];
    const char* reason = asked_pending(g_engine, a, r, now, &q->blocked, buf, sizeof(buf));
    if (reason) {
        snprintf(why, size, "%s", reason);
        return false;
    }
    q->id = q->blocked && asked_records(a) ? r.id : static_cast<uint32_t>(g_engine.instance[n]);
    return true;
}

// Dialogue box d's text object and the chat log it hands its lines back
// to, every hop guarded, NULL for either it has not.
void dialogue_box_guarded(int d, const uint8_t** text, const uint8_t** log) {
    const uint8_t* ctl = controller_guarded(kDialogueNames[d], kBoxBytes);
    const uint8_t* t = ctl ? rdptr(ctl, kBoxText) : NULL;
    *text = t && readable(t, kTextBytes) ? t : NULL;
    const uint8_t* log_ctl = ctl ? rdptr(ctl, kBoxLog) : NULL;
    *log = log_ctl && readable(log_ctl + kLogObject, 4) ? rdptr(log_ctl, kLogObject) : NULL;
}

// game.h's dialogue_pair_refusal() for dialogue box d with every hop
// guarded: false, with `why` filled, when it and its twin take no answer
// (`answer`) or cancel now.
bool dialogue_question_guarded(int d, bool answer, char* why, size_t size) {
    const uint8_t* text = NULL;
    const uint8_t* log = NULL;
    const uint8_t* twin_text = NULL;
    const uint8_t* twin_log = NULL;
    dialogue_box_guarded(d, &text, &log);
    dialogue_box_guarded(kDialogueCount - 1 - d, &twin_text, &twin_log);
    const uint8_t* allowed = g_engine.game.query_cancel_allowed;
    char buf[kWhyBytes];
    const char* reason = dialogue_pair_refusal(d, text, log, twin_text, twin_log,
        readable(allowed, 1) ? *allowed : 0, answer, buf, sizeof(buf));
    if (reason) {
        snprintf(why, size, "%s", reason);
        return false;
    }
    return true;
}

// A text window's question as a reply or a read finds it, every hop
// guarded (game.h's text_now() and text_asks()): the window as it stands,
// msgline's record's copy, and the prompt's id. `answer`: for an answer,
// which comment's view of another player's message takes none of.
struct TextQuestion {
    int text;
    uint32_t id;
    TextNow now;
    TextRecord record;
};

bool text_question_guarded(int t, bool answer, TextQuestion* q, char* why, size_t size) {
    memset(q, 0, sizeof(*q));
    q->text = t;
    const int n = g_engine.inv.text_window[t];
    TextRecord& r = q->record;
    if (t != kTextMsgline || !g_engine.msgline.read(&r)) {
        memset(&r, 0, sizeof(r));
    }
    TextNow& now = q->now;
    now.ctl = controller_guarded(kTextNames[t], text_size(t));
    now.menu = now.ctl ? open_menu_guarded(n, NULL) : NULL;
    const TextSites& s = g_engine.game.text;
    switch (t) {
    case kTextStringdl:
        now.owner = controller_guarded("delivery", kDeliveryNext + 2);
        break;
    case kTextScname: {
        uint8_t* search = s.search && readable(s.search, 4) ? *s.search : NULL;
        now.owner = search && readable(search, kSearchCriteria + 4) ? search : NULL;
        break;
    }
    case kTextLink11:
        now.owner = controller_guarded("link5", kLink5Callback + 4);
        now.selected = link5_selected(now.owner);
        break;
    case kTextMsgline:
        now.busy = s.busy && readable(s.busy, 1) ? *s.busy : 0;
        now.player = s.player && readable(s.player, 4) ? (*s.player ? 1 : 0) : -1;
        now.pol = s.pol && readable(s.pol, 4) ? (*s.pol && readable(*s.pol, kPolWord + 4) ? 1 : 0) : -1;
        now.blocked = !msgline_record_gone(r, now.ctl);
        break;
    default:
        break;
    }
    now.owner_menu = now.owner && t != kTextScname ? window_of(now.owner) : NULL;
    char buf[kWhyBytes];
    const char* reason = text_asks(t, now, r, answer, buf, sizeof(buf));
    if (reason) {
        snprintf(why, size, "%s", reason);
        return false;
    }
    q->id = now.blocked ? r.id : static_cast<uint32_t>(g_engine.instance[n]);
    return true;
}

// A text window's question asked now and the answer `a` (or its cancel)
// one the game would take now, else `why` filled.
bool text_takes_guarded(int t, bool answer, const TextAnswer& a, char* why, size_t size) {
    TextQuestion q;
    if (!text_question_guarded(t, answer, &q, why, size)) {
        return false;
    }
    char buf[kWhyBytes];
    const char* reason = text_refusal(t, q.now, answer, a, buf, sizeof(buf));
    if (reason) {
        snprintf(why, size, "%s", reason);
        return false;
    }
    return true;
}

// An offer window as a reply or a read finds it from this thread (game.h's
// offer_now() and offer_view(), every hop guarded), the trade's offers on
// their way among it.
void offer_view_guarded(int w, OfferView* v) {
    const OfferSites& s = g_engine.game.offer;
    OfferNow now;
    memset(&now, 0, sizeof(now));
    now.ctl = controller_guarded(kOfferNames[w], offer_bytes(w));
    now.menu = now.ctl ? open_menu_guarded(g_engine.inv.offer_window[w], NULL) : NULL;
    const uint8_t* data = s.player && readable(s.player, 4) ? *s.player : NULL;
    now.data = offer_data_ok(s, data) ? data : NULL;
    const uint8_t* intervals = s.intervals && readable(s.intervals, 4) ? *s.intervals : NULL;
    now.intervals = offer_intervals_ok(s, intervals) ? intervals : NULL;
    now.clock = clock_read(s, &now.now);
    TradePending p;
    const bool pending = w == kOfferTrade && g_engine.trade_pending.read(&p);
    offer_view(g_engine, w, now, pending ? &p : NULL, v);
}

// The player's data as the offer windows read it from this thread, NULL
// while any table they read is not readable.
const uint8_t* offer_data_guarded() {
    const OfferSites& s = g_engine.game.offer;
    const uint8_t* data = s.player && readable(s.player, 4) ? *s.player : NULL;
    return offer_data_ok(s, data) ? data : NULL;
}

// Inventory item `index` as the player's data holds it now, read from this
// thread; nothing for one the data does not hold.
HeldItem held_guarded(int index) {
    return held_item(g_engine.game.offer, offer_data_guarded(), index);
}

// Offer window w's open now, as the drain's gray test records it.
uint32_t offer_instance(int w) {
    return static_cast<uint32_t>(g_engine.instance[g_engine.inv.offer_window[w]]);
}

// The lines of the page dialogue box `ctl` showed last, oldest first, at
// most kPageLines: the last +0x4E nodes of its list, each line's codes and
// their count into `glyphs` and `counts`; how many were read.
int page_lines_guarded(const uint8_t* ctl, const uint8_t** glyphs, int* counts) {
    HU_STEP("page_lines_guarded");
    int want = rd16(ctl, kBoxPageLines);
    want = want < 0 ? 0 : want > kPageLines ? kPageLines : want;
    const uint8_t* lines[kPageLines];
    int found = 0;
    const uint8_t* node = rdptr(ctl, kBoxLinesTail);
    for (int steps = 0; node && found < want && steps < 512 && readable(node, kNodeRemoved + 1); ++steps) {
        const uint8_t* line = rdptr(node, kNodeItem);
        if (!node[kNodeRemoved] && line && readable(line, kLineBytes)) {
            lines[found++] = line;
        }
        node = rdptr(node, kNodePrev);
    }
    for (int i = 0; i < found; ++i) {
        const uint8_t* line = lines[found - 1 - i];
        glyphs[i] = line + kLineGlyphCodes;
        counts[i] = static_cast<uint16_t>(rd16(line, kLineCount));
    }
    return found;
}

// game.h's dead_refusal() with every hop guarded: why the KO window's own
// confirm would not ask now, or NULL.
const char* dead_refusal_guarded() {
    const uint8_t* menu = open_menu_guarded(window_named("dead"), NULL);
    return dead_refusal(g_engine, menu, controller_guarded("comyn", kYesNoBytes));
}

// game.h's session_live() with every hop guarded.
bool session_live_guarded(int which) {
    if (which == kSessionInvite) {
        const uint8_t* flags = g_engine.game.party_pending;
        return readable(flags, 1) && flags[0] != 0;
    }
    const uint8_t* delivery = controller_guarded("delivery", kDeliveryState + 2);
    const uint8_t* post1 = controller_guarded("post1", kPostState + 4);
    return (delivery && rd16(delivery, kDeliveryState) != 0) || (post1 && rd32(post1, kPostState) != 0);
}

// The id of the invite or post-box session up now, as the drain will
// number it.
uint32_t session_now(int which) {
    const LONG watch = which == kSessionInvite ? g_engine.invite_watch : g_engine.post_watch;
    MemoryBarrier();
    return session_id(watch, session_live_guarded(which));
}

// A reply naming its prompt: refused once another open of the window has
// come since the caller read the prompt's id (of either dialogue box for
// one of them: game.h's open_count()), or with kCmdSession, once another
// invite or post-box session has.
bool current_prompt(const Command& c, char* why, size_t size) {
    if (!(c.flags & kCmdInstance)) {
        return true;
    }
    const int session = (c.flags & kCmdSession) ? session_window(g_engine.inv.names[c.target].name) : -1;
    const uint32_t current = session >= 0 ? session_now(session) : open_count(g_engine, c.target);
    if (current != c.instance) {
        snprintf(why, size, "that prompt is gone");
        return false;
    }
    return true;
}

// Under the lock: the handle, the prompt, then `check`, then the queue,
// counted as a reply to the command's window.
template <typename Check>
int32_t queue_checked(const HuEngineHandle* ref, const Command& c, bool refuse_queued, char* why, uint32_t size,
                      Check check) {
    bool ok = false;
    lock();
    if (usable(ref, why, size) && current_prompt(c, why, size) && check(why, static_cast<size_t>(size))) {
        ok = enqueue_reply(c, refuse_queued, why, size);
    }
    unlock();
    return ok;
}

// queue_checked() for a text window's reply, its answer handed over in the
// engine's slot for the window, written only while no reply to the window
// is queued: the drain reads the slot until it has carried that one out.
template <typename Check>
int32_t queue_text(const HuEngineHandle* ref, const Command& c, int t, const TextAnswer& a, char* why,
                   uint32_t size, Check check) {
    bool ok = false;
    lock();
    if (usable(ref, why, size) && current_prompt(c, why, size) && check(why, static_cast<size_t>(size))) {
        if (g_engine.replies[c.target] > 0) {
            snprintf(why, size, "a reply to %s is already queued", kTextNames[t]);
        } else {
            g_engine.text_answers[t] = a;
            MemoryBarrier();
            ok = enqueue_reply(c, true, why, size);
        }
    }
    unlock();
    return ok;
}

int32_t refusal_result(const HuEngineHandle* ref, char* why, uint32_t size, const char* text) {
    lock();
    const bool ok = usable(ref, why, size) != NULL;
    unlock();
    if (ok) {
        snprintf(why, size, "%s", text);
    }
    return 0;
}

int32_t __stdcall op_query_answer(const HuEngineHandle* ref, double value, char* why, uint32_t size) {
    int v = 0;
    if (!int_value(value, 1, 0xFE, &v)) {
        return refusal_result(ref, why, size,
            "usage: query_answer(value), value = the option's value, 1..254");
    }
    const int n = window_named("query");
    return queue_checked(ref, command(kOpQueryAnswer, n, 0, 0, ref, static_cast<uint32_t>(v), kVerbAnswer),
        false, why, size, [](char* w, size_t s) { return query_open(NULL, w, s) != NULL; });
}

int32_t __stdcall op_query_cancel(const HuEngineHandle* ref, char* why, uint32_t size) {
    const int n = window_named("query");
    return queue_checked(ref, command(kOpQueryCancel, n, 0, 0, ref, 0, kVerbCancel), false, why, size,
        [](char* w, size_t s) {
            if (!query_open("query_cancel", w, s)) {
                return false;
            }
            if (!query_cancellable()) {
                snprintf(w, s, "query_cancel: this event does not allow a cancel right now");
                return false;
            }
            return true;
        });
}

int32_t __stdcall op_passinpu_submit(const HuEngineHandle* ref, const char* text, char* why, uint32_t size) {
    const size_t length = text ? strlen(text) : 0;
    if (!text || length > kPassinpuText) {
        return refusal_result(ref, why, size, "usage: passinpu_submit(text), text a string of at most 16 bytes");
    }
    Command c = command(kOpPassinpuSubmit, window_named("passinpu"), 0, 0, ref, static_cast<uint32_t>(length),
        kVerbAnswer);
    memcpy(c.text, text, length);
    return queue_checked(ref, c, false, why, size, [=](char* w, size_t s) {
        const uint8_t* ctl = passinpu_pending("passinpu_submit", w, s);
        return ctl && passinpu_fits(ctl, length, "passinpu_submit", w, s);
    });
}

int32_t __stdcall op_passinpu_cancel(const HuEngineHandle* ref, char* why, uint32_t size) {
    return queue_checked(ref, command(kOpPassinpuCancel, window_named("passinpu"), 0, 0, ref, 0, kVerbCancel),
        false, why, size, [](char* w, size_t s) { return passinpu_pending("passinpu_cancel", w, s) != NULL; });
}

int32_t party_reply(const HuEngineHandle* ref, bool accept, char* why, uint32_t size) {
    return queue_checked(ref, command(kOpPartyReply, window_named("prtyjoin"), 0, 0, ref, accept ? 1 : 0,
            accept ? kVerbAnswer : kVerbCancel), false, why, size,
        [=](char* w, size_t s) { return party_pending(accept ? "prtyjoin_accept" : "prtyjoin_decline", w, s); });
}

int32_t __stdcall op_prtyjoin_accept(const HuEngineHandle* ref, char* why, uint32_t size) {
    return party_reply(ref, true, why, size);
}

int32_t __stdcall op_prtyjoin_decline(const HuEngineHandle* ref, char* why, uint32_t size) {
    return party_reply(ref, false, why, size);
}

int32_t __stdcall op_post_close(const HuEngineHandle* ref, const char* name, char* why, uint32_t size) {
    const int n = name ? g_engine.inv.find(name) : -1;
    const char* nm = n >= 0 ? g_engine.inv.names[n].name : "";
    if (strcmp(nm, "delivery") != 0 && strcmp(nm, "post1") != 0 && strcmp(nm, "post2") != 0) {
        return refusal_result(ref, why, size, "usage: post_close(name), name delivery, post1 or post2");
    }
    return queue_checked(ref, command(kOpPostClose, n, 0, 0, ref, 0, kVerbCancel), false, why, size,
        [=](char* w, size_t s) {
            char prefix[32];
            snprintf(prefix, sizeof(prefix), "post_close %s", nm);
            return post_open(nm, prefix, w, s);
        });
}

int32_t __stdcall op_link5_confirm(const HuEngineHandle* ref, double slot, char* why, uint32_t size) {
    int s = 0;
    if (!int_value(slot, 0, kLink5Slots - 1, &s)) {
        return refusal_result(ref, why, size, "usage: link5_confirm(slot), slot the concierge cache slot 0..15");
    }
    return queue_checked(ref, command(kOpLink5Confirm, window_named("link5"), 0, 0, ref, static_cast<uint32_t>(s),
            kVerbAnswer), false, why, size,
        [](char* w, size_t n) { return link5_pending("link5_confirm", w, n) != NULL; });
}

int32_t __stdcall op_link5_cancel(const HuEngineHandle* ref, char* why, uint32_t size) {
    return queue_checked(ref, command(kOpLink5Cancel, window_named("link5"), 0, 0, ref, 0, kVerbCancel), false,
        why, size, [](char* w, size_t n) { return link5_pending("link5_cancel", w, n) != NULL; });
}

int32_t __stdcall op_arealist_confirm(const HuEngineHandle* ref, double zone, char* why, uint32_t size) {
    int z = 0;
    if (!int_value(zone, 0, 0x7FFF, &z)) {
        return refusal_result(ref, why, size, "usage: arealist_confirm(zone), zone a listed zone id");
    }
    return queue_checked(ref, command(kOpArealistAnswer, window_named("arealist"), 0, 0, ref,
            static_cast<uint32_t>(z), kVerbAnswer), false, why, size,
        [=](char* w, size_t s) {
            bool window = false;
            const uint8_t* ctl = area_event_guarded("arealist_confirm", w, s, &window);
            return ctl && area_zone_ok(ctl, window, z, "arealist_confirm", w, s);
        });
}

int32_t __stdcall op_arealist_cancel(const HuEngineHandle* ref, char* why, uint32_t size) {
    return queue_checked(ref, command(kOpArealistCancel, window_named("arealist"), 0, 0, ref, 0, kVerbCancel),
        false, why, size, [](char* w, size_t s) {
            bool window = false;
            return area_event_guarded("arealist_cancel", w, s, &window) != NULL;
        });
}

// The prompt's id into `c`, when the caller gave one. False for an id that
// is no whole number from 0 up: misuse. With `sessions`, engine abi 5's: a
// reply to prtyjoin or a post box names the session, not an open.
bool take_id(Command* c, int32_t has_id, double id, bool sessions = false) {
    if (!has_id) {
        return true;
    }
    int v = 0;
    if (!int_value(id, 0, 0x7FFFFFFF, &v)) {
        return false;
    }
    c->flags |= kCmdInstance;
    c->instance = static_cast<uint32_t>(v);
    if (sessions && c->target >= 0 && c->target < g_engine.inv.count
        && session_window(g_engine.inv.names[c->target].name) >= 0) {
        c->flags |= kCmdSession;
    }
    return true;
}

// Only the letters A to Z, either case.
bool letters_only(const char* text) {
    for (const char* p = text; *p; ++p) {
        if (!((*p >= 'A' && *p <= 'Z') || (*p >= 'a' && *p <= 'z'))) {
            return false;
        }
    }
    return true;
}

// An answer to text window t, of Lua type `kind`, as *a: 1; 0 with `why`
// the limit the game holds it to, or -1 with `why` the misuse. The game's
// limits: stringdl's name as the outgoing box's check takes it, comment's
// lines and msgline's text as their fields hold them, scname's and link11's
// names as their fields take them (letters only) and link11's colors as its
// rows step them.
int32_t text_answer_of(int t, int32_t kind, double number, const char* text, TextAnswer* a, char* why,
                       size_t size) {
    memset(a, 0, sizeof(*a));
    a->yes = -1;
    const size_t length = text ? strlen(text) : 0;
    switch (t) {
    case kTextStringdl:
        if (kind == HU_ANSWER_BOOLEAN) {
            a->yes = number != 0 ? 1 : 0;
            return 1;
        }
        if (kind != HU_ANSWER_STRING) {
            snprintf(why, size, "answer('stringdl', name) takes the recipient's name, a string, or true or false"
                " while it asks about a delivery slot");
            return -1;
        }
        if (length < static_cast<size_t>(kStringdlMin) || length > static_cast<size_t>(kStringdlMax)
            || !letters_only(text)) {
            snprintf(why, size, "the recipient's name is %d to %d letters, A to Z, and nothing else; the game sets"
                " their case itself", kStringdlMin, kStringdlMax);
            return 0;
        }
        memcpy(a->text, text, length);
        return 1;
    case kTextMsgline:
        if (kind != HU_ANSWER_STRING) {
            snprintf(why, size, "answer('msgline', text) takes the message, a string");
            return -1;
        }
        if (length == 0) {
            snprintf(why, size, "empty text sends nothing; cancel('msgline') instead");
            return 0;
        }
        if (length > static_cast<size_t>(kMsglineKept)) {
            snprintf(why, size, "%u bytes; msgline takes at most %d (%d in its modes 2 and 7)",
                static_cast<unsigned>(length), kMsglineKept, kMsglineShort);
            return 0;
        }
        memcpy(a->text, text, length);
        return 1;
    case kTextScname:
        if (kind != HU_ANSWER_STRING) {
            snprintf(why, size, "answer('scname', name) takes the name to search for, a string");
            return -1;
        }
        if (length > static_cast<size_t>(kScnameMax) || !letters_only(text)) {
            snprintf(why, size, "the name to search for is at most %d letters, A to Z, and nothing else", kScnameMax);
            return 0;
        }
        memcpy(a->text, text, length);
        return 1;
    default:
        break;
    }
    const bool comment = t == kTextComment;
    const char* usage = comment ? "answer('comment', lines) takes a table of up to 3 strings, the message's lines"
                                : "answer('link11', {name = name, color = {r, g, b}}) takes the linkshell's name and,"
                                  " if they change, its colors";
    AnswerTable table;
    if (kind != HU_ANSWER_TABLE || !parse_answer_table(text, &table) || table.too_large) {
        snprintf(why, size, "%s", usage);
        return -1;
    }
    bool named = false;
    int colors = 0;
    double color[3] = {0, 0, 0};
    for (int i = 0; i < table.count; ++i) {
        const TableEntry& e = table.entry[i];
        const int index = e.key[e.parts - 1][0] == '#' ? atoi(e.key[e.parts - 1] + 1) : 0;
        if (comment && e.parts == 1 && e.type == 's' && index >= 1 && index <= kCommentLines) {
            if (e.length > kCommentLine) {
                snprintf(why, size, "line %d is %d bytes; comment takes at most %d a line", index, e.length,
                    kCommentLine);
                return 0;
            }
            if (static_cast<int>(strlen(e.bytes)) != e.length) {
                snprintf(why, size, "line %d holds a NUL byte, which would end it there", index);
                return 0;
            }
            memcpy(a->lines[index - 1], e.bytes, e.length);
        } else if (!comment && e.parts == 1 && e.type == 's' && strcmp(e.key[0], "name") == 0) {
            if (e.length < 1 || e.length > kLink11Max || static_cast<int>(strlen(e.bytes)) != e.length
                || !letters_only(e.bytes)) {
                snprintf(why, size, "the linkshell's name is 1 to %d letters, A to Z, and nothing else", kLink11Max);
                return 0;
            }
            memcpy(a->text, e.bytes, e.length);
            named = true;
        } else if (!comment && e.parts == 2 && e.type == 'n' && strcmp(e.key[0], "color") == 0 && index >= 1
                   && index <= 3) {
            color[index - 1] = e.number;
            colors |= 1 << (index - 1);
        } else {
            snprintf(why, size, "%s", usage);
            return -1;
        }
    }
    if (comment) {
        return 1;
    }
    if (!named || (colors != 0 && colors != 7)) {
        snprintf(why, size, "%s", usage);
        return -1;
    }
    for (int k = 0; colors && k < 3; ++k) {
        int v = 0;
        if (!int_value(color[k], 0, kLink11ColorMax, &v)) {
            snprintf(why, size, "each color is a whole number from 0 to %d", kLink11ColorMax);
            return 0;
        }
        a->color[k] = static_cast<uint8_t>(v);
    }
    a->colored = colors ? 1 : 0;
    return 1;
}

// answer(name, value[, id]): the reply the player would give in the window,
// each window taking the value of one Lua type. A value of another type, or
// an id that is no whole number, is misuse (-1); a value the window does not
// list is refused (0), and so is an id another open of the window has
// replaced. `abi`, the slot's engine abi: from 5 on a reply to prtyjoin
// names the invite's session, from 8 on the yes/no windows take one, dead
// takes none, its answer being its own confirm, the number boxes take a
// whole number in the range they ask and the text windows their text, a
// table for comment and link11.
int32_t answer_reply(const HuEngineHandle* ref, const char* name, int32_t kind, double number, const char* text,
                     int32_t has_id, double id, uint32_t abi, char* why, uint32_t size) {
    if (!name) {
        snprintf(why, size, "answer(name, value) needs a window name");
        return -1;
    }
    char low[16];
    const char* nm = Inventory::lower_name(name, low) ? low : "";
    struct Expect {
        const char* window;
        int32_t kind;
        const char* usage;
    };
    static const Expect kinds[] = {
        {"query", HU_ANSWER_NUMBER, "answer('query', value) takes the option's value, a number"},
        {"link5", HU_ANSWER_NUMBER, "answer('link5', slot) takes the slot options('link5') lists, a number"},
        {"arealist", HU_ANSWER_NUMBER, "answer('arealist', id) takes a zone's id, a number"},
        {"passinpu", HU_ANSWER_STRING, "answer('passinpu', text) takes the text, a string"},
        {"prtyjoin", HU_ANSWER_BOOLEAN, "answer('prtyjoin', accept) takes true to accept or false to decline"},
    };
    for (size_t i = 0; i < sizeof(kinds) / sizeof(kinds[0]); ++i) {
        if (strcmp(nm, kinds[i].window) == 0 && kind != kinds[i].kind) {
            snprintf(why, size, "%s", kinds[i].usage);
            return -1;
        }
    }
    const bool yesno = abi >= 8 && yesno_window(nm);
    if (abi >= 8 && boolean_window(nm) && kind != HU_ANSWER_BOOLEAN) {
        snprintf(why, size, "answer('%s', yes) takes true for yes or false for no", nm);
        return -1;
    }
    const bool dead = abi >= 8 && strcmp(nm, "dead") == 0;
    if (dead && kind != HU_ANSWER_NONE && !(kind == HU_ANSWER_BOOLEAN && number != 0)) {
        snprintf(why, size, "answer('dead') takes no value, or true: it asks the KO window's question through comyn,"
            " which answer('comyn', yes) then answers");
        return -1;
    }
    const int box = abi >= 8 ? box_named(nm) : -1;
    if (box >= 0 && kind != HU_ANSWER_NUMBER) {
        snprintf(why, size, "answer('%s', number) takes a number", nm);
        return -1;
    }
    const int asked = abi >= 8 ? asked_named(nm) : -1;
    if (asked == kAskedOk && kind != HU_ANSWER_NONE && !(kind == HU_ANSWER_BOOLEAN && number != 0)) {
        snprintf(why, size, "answer('ok') takes no value, or true: it acknowledges the notice");
        return -1;
    }
    const int dialogue = abi >= 8 ? dialogue_named(nm) : -1;
    if (dialogue >= 0 && kind != HU_ANSWER_NONE && !(kind == HU_ANSWER_BOOLEAN && number != 0)) {
        snprintf(why, size, "answer('%s') takes no value, or true: it shows the next page", nm);
        return -1;
    }
    static const struct {
        int asked;
        const char* usage;
    } numbers[] = {
        {kAskedBank, "answer('bankmenu', container) takes the container's id options('bankmenu') lists, a number"},
        {kAskedMaplist, "answer('maplist', value) takes the value of a row options('maplist') lists, a number"},
        {kAskedGmtell, "answer('gmtell', value) takes the value of an option options('gmtell') lists, a number"},
        {kAskedMpDead, "answer('mp_dead', choice) takes the value of a choice options('mp_dead') lists, a number"},
        {kAskedDoor, "answer('mogdoor', exit) takes the value of an exit options('mogdoor') lists, a number"},
    };
    for (size_t i = 0; i < sizeof(numbers) / sizeof(numbers[0]); ++i) {
        if (asked == numbers[i].asked && kind != HU_ANSWER_NUMBER) {
            snprintf(why, size, "%s", numbers[i].usage);
            return -1;
        }
    }
    Command probe;
    memset(&probe, 0, sizeof(probe));
    if (!take_id(&probe, has_id, id)) {
        snprintf(why, size, "answer(name, value, id): id is the id options(name) gave, a whole number");
        return -1;
    }
    char found[kWhyBytes];
    const int n = window_index(name, found, sizeof(found));
    if (n < 0) {
        return refusal_result(ref, why, size, found);
    }
    if (strcmp(nm, "query") == 0) {
        int v = 0;
        if (!int_value(number, 1, 0xFE, &v)) {
            return refusal_result(ref, why, size, "answer('query', value): value is a whole number, 1..254");
        }
        Command c = command(kOpQueryAnswer, n, 0, 0, ref, static_cast<uint32_t>(v), kVerbAnswer);
        c.flags = kCmdListed;
        take_id(&c, has_id, id);
        return queue_checked(ref, c, true, why, size, [=](char* w, size_t s) {
            const uint8_t* ctl = query_open(NULL, w, s);
            if (!ctl) {
                return false;
            }
            if (!query_lists_guarded(ctl, v)) {
                snprintf(w, s, "no option in the list has the value %d (options('query') lists them)", v);
                return false;
            }
            return true;
        });
    }
    if (strcmp(nm, "link5") == 0) {
        int slot = 0;
        if (!int_value(number, 0, kLink5Slots - 1, &slot)) {
            return refusal_result(ref, why, size, "answer('link5', slot): slot is a whole number, 0..15");
        }
        Command c = command(kOpLink5Confirm, n, 0, 0, ref, static_cast<uint32_t>(slot), kVerbAnswer);
        c.flags = kCmdListed;
        take_id(&c, has_id, id);
        return queue_checked(ref, c, true, why, size, [=](char* w, size_t s) {
            const uint8_t* ctl = link5_pending(NULL, w, s);
            if (!ctl) {
                return false;
            }
            if (!link5_lists_guarded(ctl, slot)) {
                snprintf(w, s, "slot %d is not in the list (options('link5') lists them)", slot);
                return false;
            }
            return true;
        });
    }
    if (strcmp(nm, "arealist") == 0) {
        int row_id = 0;
        if (!int_value(number, -0x8000, 0x7FFF, &row_id)) {
            return refusal_result(ref, why, size, "answer('arealist', id): id is a whole number, -32768..32767");
        }
        Command c = command(kOpArealistAnswer, n, 0, 0, ref, static_cast<uint32_t>(row_id), kVerbAnswer);
        take_id(&c, has_id, id);
        return queue_checked(ref, c, true, why, size, [=](char* w, size_t s) {
            bool window = false;
            const uint8_t* ctl = area_event_guarded(NULL, w, s, &window);
            return ctl && area_zone_ok(ctl, window, row_id, NULL, w, s);
        });
    }
    if (strcmp(nm, "passinpu") == 0) {
        const size_t length = text ? strlen(text) : 0;
        if (length > kPassinpuText) {
            return refusal_result(ref, why, size, "answer('passinpu', text): text is at most 16 bytes");
        }
        Command c = command(kOpPassinpuSubmit, n, 0, 0, ref, static_cast<uint32_t>(length), kVerbAnswer);
        memcpy(c.text, text, length);
        take_id(&c, has_id, id);
        return queue_checked(ref, c, true, why, size, [=](char* w, size_t s) {
            const uint8_t* ctl = passinpu_pending(NULL, w, s);
            return ctl && passinpu_fits(ctl, length, NULL, w, s);
        });
    }
    if (strcmp(nm, "prtyjoin") == 0) {
        const bool accept = number != 0;
        Command c = command(kOpPartyReply, n, 0, 0, ref, accept ? 1 : 0, kVerbAnswer);
        take_id(&c, has_id, id, abi >= 5);
        return queue_checked(ref, c, true, why, size, [](char* w, size_t s) { return party_pending(NULL, w, s); });
    }
    if (yesno) {
        Command c = command(kOpYesNoAnswer, n, 0, 0, ref, number != 0 ? 1 : 0, kVerbAnswer);
        take_id(&c, has_id, id);
        return queue_checked(ref, c, true, why, size,
            [=](char* w, size_t s) { return yesno_pending_guarded(n, w, s) != NULL; });
    }
    if (box >= 0) {
        int v = 0;
        if (!int_value(number, -0x7FFFFFFF, 0x7FFFFFFF, &v)) {
            char text[kWhyBytes];
            snprintf(text, sizeof(text), "answer('%s', number): the number is a whole number", nm);
            return refusal_result(ref, why, size, text);
        }
        Command c = command(kOpBoxAnswer, n, 0, 0, ref, static_cast<uint32_t>(v), kVerbAnswer);
        take_id(&c, has_id, id);
        return queue_checked(ref, c, true, why, size, [=](char* w, size_t s) {
            BoxQuestion q;
            if (!box_question_guarded(box, &q, w, s)) {
                return false;
            }
            const int32_t max = box_max(q.ctl, box);
            if (v < kBoxLayouts[box].min || v > max) {
                snprintf(w, s, "%d is outside %d..%d, what %s takes now", v, static_cast<int>(kBoxLayouts[box].min),
                    static_cast<int>(max), kBoxNames[box]);
                return false;
            }
            char buf[kWhyBytes];
            const char* refused = box_refusal_guarded(q, true, buf, sizeof(buf));
            if (refused) {
                snprintf(w, s, "%s", refused);
                return false;
            }
            return true;
        });
    }
    if (dead) {
        Command c = command(kOpDeadConfirm, n, 0, 0, ref, 0, kVerbAnswer);
        take_id(&c, has_id, id);
        return queue_checked(ref, c, true, why, size, [](char* w, size_t s) {
            const char* reason = dead_refusal_guarded();
            if (reason) {
                snprintf(w, s, "%s", reason);
            }
            return reason == NULL;
        });
    }
    if (asked >= 0) {
        int v = 1;
        if (kind == HU_ANSWER_BOOLEAN) {
            v = number != 0 ? 1 : 0;
        } else if (kind == HU_ANSWER_NUMBER) {
            const bool gm = asked == kAskedGmtell;
            if (!int_value(number, gm ? 1 : -0x7FFFFFFF, gm ? 0xFE : 0x7FFFFFFF, &v)) {
                char text[kWhyBytes];
                snprintf(text, sizeof(text), gm ? "answer('gmtell', value): value is a whole number, 1..254"
                                                : "answer('%s', number): the number is a whole number", nm);
                return refusal_result(ref, why, size, text);
            }
        }
        Command c = command(kOpAskedAnswer, n, 0, 0, ref, static_cast<uint32_t>(v), kVerbAnswer);
        if (asked == kAskedGmtell) {
            c.flags = kCmdListed;
        }
        take_id(&c, has_id, id);
        return queue_checked(ref, c, true, why, size, [=](char* w, size_t s) {
            AskedQuestion q;
            if (!asked_question_guarded(asked, &q, w, s)) {
                return false;
            }
            char buf[kWhyBytes];
            const char* refused = asked_refusal(g_engine, asked, q.now, q.record, q.blocked, true, v, buf,
                sizeof(buf));
            if (!refused && asked == kAskedGmtell && !query_lists_guarded(q.now.ctl, v)) {
                snprintf(buf, sizeof(buf), "no option in the list has the value %d (options('gmtell') lists them)", v);
                refused = buf;
            }
            if (refused) {
                snprintf(w, s, "%s", refused);
                return false;
            }
            return true;
        });
    }
    if (dialogue >= 0) {
        Command c = command(kOpDialogueAnswer, n, 0, 0, ref, 0, kVerbAnswer);
        take_id(&c, has_id, id);
        return queue_checked(ref, c, true, why, size,
            [=](char* w, size_t s) { return dialogue_question_guarded(dialogue, true, w, s); });
    }
    const int offer = abi >= 8 ? offer_named(nm) : -1;
    if (offer >= 0) {
        if (kind != HU_ANSWER_NONE && !(kind == HU_ANSWER_BOOLEAN && number != 0)) {
            snprintf(why, size, "answer('%s') takes no value, or true: it is the window's OK; cancel('%s') cancels"
                " it", nm, nm);
            return -1;
        }
        Command c = command(kOpOfferAnswer, n, 0, 0, ref, 1, kVerbAnswer);
        take_id(&c, has_id, id);
        return queue_checked(ref, c, true, why, size, [=](char* w, size_t s) {
            OfferView v;
            offer_view_guarded(offer, &v);
            char buf[kWhyBytes];
            const char* refused = offer_reply_refusal(v, true, buf, sizeof(buf));
            if (refused) {
                snprintf(w, s, "%s", refused);
                return false;
            }
            return true;
        });
    }
    const int text_t = abi >= 8 ? text_named(nm) : -1;
    if (text_t >= 0) {
        TextAnswer a;
        char reason[kWhyBytes];
        const int32_t taken = text_answer_of(text_t, kind, number, text, &a, reason, sizeof(reason));
        if (taken < 0) {
            snprintf(why, size, "%s", reason);
            return -1;
        }
        if (taken == 0) {
            return refusal_result(ref, why, size, reason);
        }
        Command c = command(kOpTextAnswer, n, 0, 0, ref, 0, kVerbAnswer);
        take_id(&c, has_id, id);
        return queue_text(ref, c, text_t, a, why, size,
            [=](char* w, size_t s) { return text_takes_guarded(text_t, true, a, w, s); });
    }
    char text_why[kWhyBytes];
    snprintf(text_why, sizeof(text_why), abi >= 8
        ? "%s takes no answer; answer() replies to query, link5, arealist, passinpu, prtyjoin, comyn, merityn, the"
          " resyn windows, shopbuy, shopsell, dead, moneyctr, itemctrl, sortyn, evitem01, confyn, bankmenu,"
          " maplist, gmtell, mp_dead, ok, mogdoor, rem4line, rem4li2, stringdl, comment, msgline, scname, link11,"
          " trade and handover"
        : "%s takes no answer; answer() replies to query, link5, arealist, passinpu and prtyjoin",
        g_engine.inv.names[n].name);
    return refusal_result(ref, why, size, text_why);
}

int32_t __stdcall op_answer(const HuEngineHandle* ref, const char* name, int32_t kind, double number,
                            const char* text, char* why, uint32_t size) {
    return answer_reply(ref, name, kind, number, text, 0, 0, 3, why, size);
}

int32_t __stdcall op_answer4(const HuEngineHandle* ref, const char* name, int32_t kind, double number,
                             const char* text, int32_t has_id, double id, char* why, uint32_t size) {
    return answer_reply(ref, name, kind, number, text, has_id, id, 4, why, size);
}

int32_t __stdcall op_answer5(const HuEngineHandle* ref, const char* name, int32_t kind, double number,
                             const char* text, int32_t has_id, double id, char* why, uint32_t size) {
    return answer_reply(ref, name, kind, number, text, has_id, id, 5, why, size);
}

int32_t __stdcall op_answer8(const HuEngineHandle* ref, const char* name, int32_t kind, double number,
                             const char* text, int32_t has_id, double id, char* why, uint32_t size) {
    return answer_reply(ref, name, kind, number, text, has_id, id, 8, why, size);
}

// A whole number a Lua number holds exactly; NaN, a fraction and anything
// past 2^53 are not.
bool whole_number(double v, int64_t* out) {
    if (!(v >= -9007199254740992.0 && v <= 9007199254740992.0)) {
        return false;
    }
    const int64_t i = static_cast<int64_t>(v);
    if (static_cast<double>(i) != v) {
        return false;
    }
    *out = i;
    return true;
}

// The offer window `name` names for offer() and withdraw(); -1 with `why`
// filled, the result offer8 or withdraw8 returns in *result.
int offer_window_of(const HuEngineHandle* ref, const char* name, const char* verb, char* why, uint32_t size,
                    int32_t* result) {
    char found[kWhyBytes];
    const int n = window_index(name, found, sizeof(found));
    if (n < 0) {
        *result = refusal_result(ref, why, size, found);
        return -1;
    }
    const int w = g_engine.inv.offer_of(n);
    if (w < 0) {
        char text[kWhyBytes];
        snprintf(text, sizeof(text), "%s takes no %s; %s() takes trade and handover", g_engine.inv.names[n].name,
            verb, verb);
        *result = refusal_result(ref, why, size, text);
    }
    return w;
}

// offer(name, item, count): `count` of inventory item `item`, or with item
// "gil" `count` gil, into the window's offer, refused up front for all the
// game would refuse, its gray test of the item as the drain last ran it on
// this open and this item; the game thread runs that test again. A count
// that is no whole number, or an item that is neither an index nor "gil",
// is misuse.
int32_t __stdcall op_offer8(const HuEngineHandle* ref, const char* name, double item, const char* item_text,
                            double count, char* why, uint32_t size) {
    int64_t index = 0;
    const bool gil = item_text && strcmp(item_text, "gil") == 0;
    if (!gil && (item_text || !whole_number(item, &index))) {
        snprintf(why, size, "offer(name, item, count) takes the item's inventory index, a whole number, or 'gil'");
        return -1;
    }
    int64_t n = 0;
    if (!whole_number(count, &n)) {
        snprintf(why, size, gil ? "offer(name, 'gil', amount): the amount is a whole number"
                                : "offer(name, item, count): the count is a whole number");
        return -1;
    }
    int32_t result = 0;
    const int w = offer_window_of(ref, name, "offer", why, size, &result);
    if (w < 0) {
        return result;
    }
    if (!gil && (index < 1 || index >= kInventoryRecords)) {
        char text[kWhyBytes];
        snprintf(text, sizeof(text), "the inventory has no slot %lld", static_cast<long long>(index));
        return refusal_result(ref, why, size, text);
    }
    if (n > 0xFFFFFFFFLL) {
        n = 0xFFFFFFFFLL;
    }
    const int x = gil ? 0 : static_cast<int>(index);
    Command c = command(kOpOffer, g_engine.inv.offer_window[w], x, 0, ref,
        static_cast<uint32_t>(n < 0 ? 0 : n), kVerbOffer);
    return queue_checked(ref, c, true, why, size, [=](char* t, size_t ts) {
        OfferView v;
        offer_view_guarded(w, &v);
        char buf[kWhyBytes];
        int slot = 0;
        const HeldItem held = x ? held_guarded(x) : HeldItem();
        const char* refused = offer_refusal(v, x, held, n, &slot, buf, sizeof(buf));
        GrayRows rows;
        if (!refused && x && g_engine.gray_table.read(&rows)) {
            refused = gray_rows_refusal(rows, w, offer_instance(w), x, held, buf, sizeof(buf));
        }
        if (refused) {
            snprintf(t, ts, "%s", refused);
            return false;
        }
        return true;
    });
}

// withdraw(name, item): inventory item `item` back out of the window's
// offer, as its own row's confirm takes it out.
int32_t __stdcall op_withdraw8(const HuEngineHandle* ref, const char* name, double item, char* why,
                               uint32_t size) {
    int64_t index = 0;
    if (!whole_number(item, &index)) {
        snprintf(why, size, "withdraw(name, item) takes the item's inventory index, a whole number; offer(name,"
            " 'gil', 0) takes the gil back");
        return -1;
    }
    int32_t result = 0;
    const int w = offer_window_of(ref, name, "withdraw", why, size, &result);
    if (w < 0) {
        return result;
    }
    if (index < 1 || index >= kInventoryRecords) {
        return refusal_result(ref, why, size, "that item is not offered");
    }
    const int x = static_cast<int>(index);
    Command c = command(kOpWithdraw, g_engine.inv.offer_window[w], x, 0, ref, 0, kVerbWithdraw);
    return queue_checked(ref, c, true, why, size, [=](char* t, size_t ts) {
        OfferView v;
        offer_view_guarded(w, &v);
        char buf[kWhyBytes];
        int slot = 0;
        const char* refused = withdraw_refusal(v, x, &slot, buf, sizeof(buf));
        if (refused) {
            snprintf(t, ts, "%s", refused);
            return false;
        }
        return true;
    });
}

// game.h's post_end() checks with every hop guarded: with post1's window
// instance live, the gates the player's cancel passes, else post_close's.
bool box_cancellable(const char* nm, char* why, size_t size) {
    const uint8_t* menu = window_of(controller_guarded("post1", kCtlMenu + 4));
    if (!menu) {
        return post_open(nm, NULL, why, size);
    }
    const char* reason = box_cancel_refusal(menu);
    if (reason) {
        snprintf(why, size, "%s", reason);
        return false;
    }
    return true;
}

// cancel(name[, id]): the way out the window itself offers, or the end of
// the session for the post box. `abi`, the slot's engine abi: from 4 on the
// incoming box ends as its window's own cancel does, where abi 3 refuses
// while a box window is live; from 5 on a reply to prtyjoin or a box names
// its session; from 8 on the yes/no windows, the number boxes and the text
// windows take one, and dead's and comment's refusals say why they have
// none.
int32_t cancel_reply(const HuEngineHandle* ref, const char* name, int32_t has_id, double id, uint32_t abi,
                     char* why, uint32_t size) {
    Command probe;
    memset(&probe, 0, sizeof(probe));
    if (!take_id(&probe, has_id, id)) {
        snprintf(why, size, "cancel(name, id): id is the id options(name) gave, a whole number");
        return -1;
    }
    char found[kWhyBytes];
    const int n = window_index(name, found, sizeof(found));
    if (n < 0) {
        return refusal_result(ref, why, size, found);
    }
    const char* nm = g_engine.inv.names[n].name;
    Command c;
    if (strcmp(nm, "query") == 0) {
        c = command(kOpQueryCancel, n, 0, 0, ref, 0, kVerbCancel);
        take_id(&c, has_id, id);
        return queue_checked(ref, c, true, why, size,
            [](char* w, size_t s) {
                if (!query_open(NULL, w, s)) {
                    return false;
                }
                if (!query_cancellable()) {
                    snprintf(w, s, "query cannot be canceled now: the event allows no cancel");
                    return false;
                }
                return true;
            });
    }
    if (strcmp(nm, "passinpu") == 0) {
        c = command(kOpPassinpuCancel, n, 0, 0, ref, 0, kVerbCancel);
        take_id(&c, has_id, id);
        return queue_checked(ref, c, true, why, size,
            [](char* w, size_t s) { return passinpu_pending(NULL, w, s) != NULL; });
    }
    if (strcmp(nm, "link5") == 0) {
        c = command(kOpLink5Cancel, n, 0, 0, ref, 0, kVerbCancel);
        take_id(&c, has_id, id);
        return queue_checked(ref, c, true, why, size,
            [](char* w, size_t s) { return link5_pending(NULL, w, s) != NULL; });
    }
    if (strcmp(nm, "arealist") == 0) {
        c = command(kOpArealistCancel, n, 0, 0, ref, 0, kVerbCancel);
        take_id(&c, has_id, id);
        return queue_checked(ref, c, true, why, size, [](char* w, size_t s) {
            bool window = false;
            return area_event_guarded(NULL, w, s, &window) != NULL;
        });
    }
    if (strcmp(nm, "prtyjoin") == 0) {
        c = command(kOpPartyReply, n, 0, 0, ref, 0, kVerbCancel);
        take_id(&c, has_id, id, abi >= 5);
        return queue_checked(ref, c, true, why, size, [](char* w, size_t s) { return party_pending(NULL, w, s); });
    }
    if (abi >= 4 && (strcmp(nm, "post1") == 0 || strcmp(nm, "post2") == 0)) {
        c = command(kOpPostEnd, n, 0, 0, ref, 0, kVerbCancel);
        take_id(&c, has_id, id, abi >= 5);
        return queue_checked(ref, c, true, why, size,
            [=](char* w, size_t s) { return box_cancellable(nm, w, s); });
    }
    if (strcmp(nm, "delivery") == 0 || strcmp(nm, "post1") == 0 || strcmp(nm, "post2") == 0) {
        c = command(kOpPostClose, n, 0, 0, ref, 0, kVerbCancel);
        take_id(&c, has_id, id, abi >= 5);
        return queue_checked(ref, c, true, why, size, [=](char* w, size_t s) { return post_open(nm, NULL, w, s); });
    }
    if (abi >= 8 && strcmp(nm, "dead") == 0) {
        return refusal_result(ref, why, size,
            "dead has no cancel: the game's own Escape there only hands focus to the chat log");
    }
    if (abi >= 8 && box_named(nm) >= 0) {
        const int box = box_named(nm);
        c = command(kOpBoxCancel, n, 0, 0, ref, 0, kVerbCancel);
        take_id(&c, has_id, id);
        return queue_checked(ref, c, true, why, size, [=](char* w, size_t s) {
            BoxQuestion q;
            if (!box_question_guarded(box, &q, w, s)) {
                return false;
            }
            char buf[kWhyBytes];
            const char* refused = box_refusal_guarded(q, false, buf, sizeof(buf));
            if (refused) {
                snprintf(w, s, "%s", refused);
                return false;
            }
            return true;
        });
    }
    if (abi >= 8 && asked_named(nm) >= 0) {
        const int asked = asked_named(nm);
        if (asked_cancel_refusal(asked)) {
            return refusal_result(ref, why, size, asked_cancel_refusal(asked));
        }
        c = command(kOpAskedCancel, n, 0, 0, ref, 0, kVerbCancel);
        take_id(&c, has_id, id);
        return queue_checked(ref, c, true, why, size, [=](char* w, size_t s) {
            AskedQuestion q;
            if (!asked_question_guarded(asked, &q, w, s)) {
                return false;
            }
            char buf[kWhyBytes];
            const char* refused = asked_refusal(g_engine, asked, q.now, q.record, q.blocked, false, 0, buf,
                sizeof(buf));
            if (refused) {
                snprintf(w, s, "%s", refused);
                return false;
            }
            return true;
        });
    }
    if (abi >= 8 && dialogue_named(nm) >= 0) {
        const int dialogue = dialogue_named(nm);
        c = command(kOpDialogueCancel, n, 0, 0, ref, 0, kVerbCancel);
        take_id(&c, has_id, id);
        return queue_checked(ref, c, true, why, size,
            [=](char* w, size_t s) { return dialogue_question_guarded(dialogue, false, w, s); });
    }
    if (abi >= 8 && yesno_window(nm)) {
        c = command(kOpYesNoCancel, n, 0, 0, ref, 0, kVerbCancel);
        take_id(&c, has_id, id);
        return queue_checked(ref, c, true, why, size, [=](char* w, size_t s) {
            const uint8_t* ctl = yesno_pending_guarded(n, w, s);
            if (ctl && !ctl[kYesNoCancellable]) {
                snprintf(w, s, "this question cannot be canceled; answer it");
                return false;
            }
            return ctl != NULL;
        });
    }
    if (abi >= 8 && offer_named(nm) >= 0) {
        const int offer = offer_named(nm);
        c = command(kOpOfferCancel, n, 0, 0, ref, 0, kVerbCancel);
        take_id(&c, has_id, id);
        return queue_checked(ref, c, true, why, size, [=](char* w, size_t s) {
            OfferView v;
            offer_view_guarded(offer, &v);
            char buf[kWhyBytes];
            const char* refused = offer_reply_refusal(v, false, buf, sizeof(buf));
            if (refused) {
                snprintf(w, s, "%s", refused);
                return false;
            }
            return true;
        });
    }
    if (abi >= 8 && text_named(nm) >= 0) {
        const int t = text_named(nm);
        if (t == kTextComment) {
            return refusal_result(ref, why, size,
                "comment has no cancel: closing it sends the text, as the game's Escape does; answer it");
        }
        TextAnswer a;
        memset(&a, 0, sizeof(a));
        a.yes = -1;
        c = command(kOpTextCancel, n, 0, 0, ref, 0, kVerbCancel);
        take_id(&c, has_id, id);
        return queue_text(ref, c, t, a, why, size,
            [=](char* w, size_t s) { return text_takes_guarded(t, false, a, w, s); });
    }
    char text_why[kWhyBytes];
    snprintf(text_why, sizeof(text_why), abi >= 8
        ? "%s has nothing to cancel; cancel() takes query, passinpu, link5, arealist, prtyjoin, delivery, post1,"
          " post2, comyn, merityn, the resyn windows, shopbuy, shopsell, moneyctr, itemctrl, sortyn, evitem01,"
          " bankmenu, maplist, gmtell, mogdoor, rem4line, rem4li2, stringdl, msgline, scname, link11, trade and"
          " handover"
        : "%s has nothing to cancel; cancel() takes query, passinpu, link5, arealist, prtyjoin, delivery, post1"
          " and post2", nm);
    return refusal_result(ref, why, size, text_why);
}

int32_t __stdcall op_cancel(const HuEngineHandle* ref, const char* name, char* why, uint32_t size) {
    return cancel_reply(ref, name, 0, 0, 3, why, size);
}

int32_t __stdcall op_cancel4(const HuEngineHandle* ref, const char* name, int32_t has_id, double id, char* why,
                             uint32_t size) {
    return cancel_reply(ref, name, has_id, id, 4, why, size);
}

int32_t __stdcall op_cancel5(const HuEngineHandle* ref, const char* name, int32_t has_id, double id, char* why,
                             uint32_t size) {
    return cancel_reply(ref, name, has_id, id, 5, why, size);
}

int32_t __stdcall op_cancel8(const HuEngineHandle* ref, const char* name, int32_t has_id, double id, char* why,
                             uint32_t size) {
    return cancel_reply(ref, name, has_id, id, 8, why, size);
}

// "rows" or "WxH", as the Lua binding writes the numbers it was given.
bool parse_size(const char* text, long* a, long* b, bool* two) {
    if (!text) {
        return false;
    }
    char* end = NULL;
    *a = strtol(text, &end, 10);
    if (end == text) {
        return false;
    }
    *two = *end == 'x';
    if (!*two) {
        return *end == '\0';
    }
    const char* rest = end + 1;
    *b = strtol(rest, &end, 10);
    return end != rest && *end == '\0';
}

int32_t resize_command(const HuEngineHandle* ref, const char* name, const char* size_text, bool windows,
                       char* why, uint32_t size) {
    long a = 0;
    long b = 0;
    bool two = false;
    const bool parsed = parse_size(size_text, &a, &b, &two);
    bool ok = false;
    lock();
    if (usable(ref, why, size)) {
        const int n = lookup(name, windows, why, size);
        if (n >= 0) {
            const char* nm = g_engine.inv.names[n].name;
            const Family* f = family_of(nm);
            if (!parsed) {
                snprintf(why, size, "usage: resize(name, rows) or resize(name, w, h) with whole numbers");
            } else if (synthetic(g_engine, n) && (two ? (a >= 1 && a <= 0x7FFF && b >= 1 && b <= 0x7FFF) : a >= 1)) {
                // Accepted and ignored: the compass and the target arrows
                // have no size to set; nothing is queued or remembered.
                ok = true;
            } else if (!two && !f) {
                snprintf(why, size, "resize %s: it has no template family; resize(name, w, h) sets any size", nm);
            } else if (!two && (a < f->min || a > f->max)) {
                snprintf(why, size, "resize %s: its templates take %d..%d rows", nm, f->min, f->max);
            } else if (two && (a < 1 || a > 0x7FFF || b < 1 || b > 0x7FFF)) {
                snprintf(why, size, "resize %s: width and height are 1..32767", nm);
            } else if (two) {
                ok = enqueue(command(kOpResizeRect, n, static_cast<int>(a), static_cast<int>(b), ref, 0, kVerbResize),
                    why, size);
            } else {
                ok = enqueue(command(kOpResizeRows, n, 0, 0, ref, static_cast<uint32_t>(a), kVerbResize), why, size);
            }
        }
    }
    unlock();
    return ok;
}

int32_t __stdcall op_resize(const HuEngineHandle* ref, const char* name, const char* size_text,
                            char* why, uint32_t size) {
    return resize_command(ref, name, size_text, false, why, size);
}

int32_t __stdcall op_resize3(const HuEngineHandle* ref, const char* name, const char* size_text,
                             char* why, uint32_t size) {
    return resize_command(ref, name, size_text, true, why, size);
}

// ---------------------------------------------------------------------------
// Events.

// Engine abi 1 and 2's names: an open and a return from under another
// window were both `show`, a close and a cover both `close`; errors go to
// abi 3's poll alone.
const char* old_event_name(int type, int32_t* fresh) {
    *fresh = -1;
    switch (type) {
    case kEvOpened: *fresh = 1; return "show";
    case kEvUncovered: *fresh = 0; return "show";
    case kEvClosed:
    case kEvCovered: return "close";
    case kEvBlocked: return "blocked";
    default: return NULL;
    }
}

const char* event_name_of(int type) {
    switch (type) {
    case kEvOpened: return "opened";
    case kEvClosed: return "closed";
    case kEvCovered: return "covered";
    case kEvUncovered: return "uncovered";
    case kEvBlocked: return "blocked";
    case kEvError: return "error";
    default: return NULL;
    }
}

// Whether error record r is another handle's. A heap check's report is
// every handle's.
#ifdef HU_HEAPCHECK
#define HU_ERROR_ELSEWHERE(r, ref) \
    ((r).slot != kEveryHandle && ((r).slot != (ref)->slot || (r).gen != (ref)->generation))
#else
#define HU_ERROR_ELSEWHERE(r, ref) ((r).slot != (ref)->slot || (r).gen != (ref)->generation)
#endif

// The reason an error record gives. A group move another handle's replaced
// names that handle while its slot has not been claimed again.
void error_reason(const ErrorRecord& r, char* out, size_t size) {
    const char* by = NULL;
    char name[48];
    if (r.by_slot >= 0) {
        lock();
        const char* held = g_rt.handles.name_of(r.by_slot, r.by_gen);
        if (held) {
            snprintf(name, sizeof(name), "%s", held);
            by = name;
        }
        unlock();
    }
    if (by) {
        snprintf(out, size, "replaced by %s's move", by);
    } else {
        snprintf(out, size, "%s", r.reason);
    }
}

// Up to `max` raw events past the handle's cursor, under the lock.
int take_events(const HuEngineHandle* ref, uint32_t* raw, int max, uint32_t* dropped, char* why, uint32_t size) {
    HU_STEP("take_events");
    lock();
    Handle* h = usable(ref, why, size);
    if (!h) {
        unlock();
        return -1;
    }
    const int n = g_engine.events.read(&h->cursor, raw, max, &h->dropped);
    *dropped = h->dropped;
    g_rt.dropped += h->dropped;
    h->dropped = 0;
    unlock();
    return n;
}

// Takes up to `capacity` events off the ring; `count` says how many, so a
// caller that got a full batch asks again.
int32_t __stdcall op_poll(const HuEngineHandle* ref, HuEngineEvent* out, uint32_t capacity,
                          uint32_t* count, uint32_t* dropped, char* why, uint32_t size) {
    *count = 0;
    *dropped = 0;
    uint32_t raw[256];
    const int n = take_events(ref, raw, capacity < 256 ? static_cast<int>(capacity) : 256, dropped, why, size);
    if (n < 0) {
        return 0;
    }
    for (int i = 0; i < n; ++i) {
        HuEngineEvent& e = out[i];
        memset(&e, 0, sizeof(e));
        e.fresh = -1;
        const int name = event_name(raw[i]);
        const char* event = old_event_name(event_type(raw[i]), &e.fresh);
        if (!event || name < 0 || name >= g_engine.inv.count) {
            continue;
        }
        snprintf(e.event, sizeof(e.event), "%s", event);
        snprintf(e.name, sizeof(e.name), "%s", g_engine.inv.names[name].name);
    }
    *count = static_cast<uint32_t>(n);
    return 1;
}

// As op_poll, with abi 3's names; an error reaches only the handle whose
// command the game thread refused.
int32_t __stdcall op_poll3(const HuEngineHandle* ref, HuEngineEvent3* out, uint32_t capacity,
                           uint32_t* count, uint32_t* dropped, char* why, uint32_t size) {
    *count = 0;
    *dropped = 0;
    uint32_t raw[256];
    const int n = take_events(ref, raw, capacity < 256 ? static_cast<int>(capacity) : 256, dropped, why, size);
    if (n < 0) {
        return 0;
    }
    for (int i = 0; i < n; ++i) {
        HuEngineEvent3& e = out[i];
        memset(&e, 0, sizeof(e));
        const int type = event_type(raw[i]);
        const int name = event_name(raw[i]);
        const char* event = event_name_of(type);
        if (!event) {
            continue;
        }
        if (type == kEvError) {
            ErrorRecord r;
            if (!g_engine.errors.read(static_cast<uint32_t>(name), &r) || HU_ERROR_ELSEWHERE(r, ref)) {
                continue;
            }
            snprintf(e.name, sizeof(e.name), "%s", r.name);
            snprintf(e.verb, sizeof(e.verb), "%s", r.verb);
            error_reason(r, e.reason, sizeof(e.reason));
        } else if (name < 0 || name >= g_engine.inv.count) {
            continue;
        } else {
            snprintf(e.name, sizeof(e.name), "%s", g_engine.inv.names[name].name);
        }
        snprintf(e.event, sizeof(e.event), "%s", event);
    }
    *count = static_cast<uint32_t>(n);
    return 1;
}

// As op_poll3, and `pending` with what changed (invite or post) and whether
// it is pending now. Abi 3's poll skips those.
int32_t __stdcall op_poll4(const HuEngineHandle* ref, HuEngineEvent4* out, uint32_t capacity,
                           uint32_t* count, uint32_t* dropped, char* why, uint32_t size) {
    *count = 0;
    *dropped = 0;
    uint32_t raw[256];
    const int n = take_events(ref, raw, capacity < 256 ? static_cast<int>(capacity) : 256, dropped, why, size);
    if (n < 0) {
        return 0;
    }
    for (int i = 0; i < n; ++i) {
        HuEngineEvent4& e = out[i];
        memset(&e, 0, sizeof(e));
        const int type = event_type(raw[i]);
        const int name = event_name(raw[i]);
        if (type == kEvPending) {
            // A number box's question is engine abi 8's to report.
            if (pending_what(name) >= kPendingBox) {
                continue;
            }
            snprintf(e.event, sizeof(e.event), "pending");
            snprintf(e.what, sizeof(e.what), "%s", pending_what(name) == kPendingPost ? "post" : "invite");
            e.pending = pending_on(name) ? 1 : 0;
            continue;
        }
        const char* event = event_name_of(type);
        if (!event) {
            continue;
        }
        if (type == kEvError) {
            ErrorRecord r;
            if (!g_engine.errors.read(static_cast<uint32_t>(name), &r) || HU_ERROR_ELSEWHERE(r, ref)) {
                continue;
            }
            snprintf(e.name, sizeof(e.name), "%s", r.name);
            snprintf(e.verb, sizeof(e.verb), "%s", r.verb);
            error_reason(r, e.reason, sizeof(e.reason));
        } else if (name < 0 || name >= g_engine.inv.count) {
            continue;
        } else {
            snprintf(e.name, sizeof(e.name), "%s", g_engine.inv.names[name].name);
        }
        snprintf(e.event, sizeof(e.event), "%s", event);
    }
    *count = static_cast<uint32_t>(n);
    return 1;
}

bool write_holders(ReplyWriter& w, const char* key, int n, uint8_t bit, const HuEngineHandle* ref);
bool write_blockers(ReplyWriter& w, const char* key, int n, int category, const HuEngineHandle* ref);

// One event for engine abi 5's poll, as a table on the reply; false for one
// that is not this handle's or names nothing. Under the lock. A blocked
// event's `by` names the handles whose hold refused the open: the window's
// whole-window holders, and on ability the holders of the list it carries.
// From `version` 8 on, a pending event also comes for a number box's
// question, `what` the box's name, but not to the handle whose reply ended
// it; and a page event when a dialogue box shows a new page.
bool write_event(ReplyWriter& w, const HuEngineHandle* ref, uint32_t raw, int version) {
    const int type = event_type(raw);
    if (type == kEvCursor) {
        const int n = cursor_name(raw);
        if (n >= g_engine.inv.count) {
            return false;
        }
        w.table();
        w.set_string("event", "cursor");
        w.set_string("name", g_engine.inv.names[n].name);
        w.set_number("row", cursor_row(raw));
        return true;
    }
    const int name = event_name(raw);
    if (type == kEvPage) {
        if (version < 8 || name < 0 || name >= g_engine.inv.count) {
            return false;
        }
        w.table();
        w.set_string("event", "page");
        w.set_string("name", g_engine.inv.names[name].name);
        return true;
    }
    if (type == kEvPending) {
        const int what = pending_what(name);
        if (what >= kPendingBox && (version < 8 || pending_from(raw, ref->slot, ref->generation))) {
            return false;
        }
        w.table();
        w.set_string("event", "pending");
        w.set_string("what", pending_name(what));
        w.set_bool("pending", pending_on(name));
        return true;
    }
    if (type == kEvError) {
        ErrorRecord r;
        if (!g_engine.errors.read(static_cast<uint32_t>(name), &r) || HU_ERROR_ELSEWHERE(r, ref)) {
            return false;
        }
        char reason[kErrorReason + 64];
        r.verb[sizeof(r.verb) - 1] = '\0';
        r.name[sizeof(r.name) - 1] = '\0';
        r.reason[sizeof(r.reason) - 1] = '\0';
        error_reason(r, reason, sizeof(reason));
        w.table();
        w.set_string("event", "error");
        w.set_string("name", r.name);
        w.set_string("verb", r.verb);
        w.set_string("reason", reason);
        return true;
    }
    const char* event = event_name_of(type);
    if (!event || name < 0 || name >= g_engine.inv.count) {
        return false;
    }
    w.table();
    w.set_string("event", event);
    w.set_string("name", g_engine.inv.names[name].name);
    int category = -1;
    if (event_has_category(raw)) {
        category = event_category(raw);
        w.set_number("category", category);
        if (const char* named = category_name(category)) {
            w.set_string("category_name", named);
        }
    }
    if (type == kEvBlocked) {
        w.set_bool("mine", write_blockers(w, "by", name, category, ref));
    }
    return true;
}

// Lua-thread only, under the lock.
uint32_t g_poll_raw[kEventCapacity];

// Engine abi 5's poll: every event past the handle's cursor as a list of
// tables, {event, name} and each type's own fields (on ability, the list
// the event is about as category and category_name), closed by {event =
// 'resync', dropped} when events were lost; then the count lost. The
// cursor moves only when the reply fits, so a retry with `used` bytes
// reads the same events. Engine abi 8's adds the pending events of the
// questions a block keeps from their windows, and the dialogue boxes' page
// events.
void poll_reply(const HuEngineHandle* ref, HuEngineReply* reply, int version) {
    HU_STEP_AT("poll_reply", version);
    ReplyWriter w = {reply};
    char why[256];
    lock();
    Handle* h = usable(ref, why, sizeof(why));
    if (!h) {
        unlock();
        w.fail(why);
        return;
    }
    uint32_t cursor = h->cursor;
    uint32_t dropped = h->dropped;
    const int n = g_engine.events.read(&cursor, g_poll_raw, static_cast<int>(kEventCapacity), &dropped);
    w.table();
    int k = 0;
    for (int i = 0; i < n; ++i) {
        if (write_event(w, ref, g_poll_raw[i], version)) {
            w.index(++k);
        }
    }
    if (dropped > 0) {
        w.table();
        w.set_string("event", "resync");
        w.set_number("dropped", dropped);
        w.index(++k);
    }
    w.number(dropped);
    if (reply->used <= reply->capacity) {
        h->cursor = cursor;
        g_rt.dropped += dropped;
        h->dropped = 0;
    }
    unlock();
}

void __stdcall op_poll5(const HuEngineHandle* ref, HuEngineReply* reply) {
    poll_reply(ref, reply, 5);
}

void __stdcall op_poll8(const HuEngineHandle* ref, HuEngineReply* reply) {
    poll_reply(ref, reply, 8);
}

// ---------------------------------------------------------------------------
// Reads.

bool remembered(const MemEntry& m) {
    return m.active || m.size_kind;
}

bool written_by(int32_t slot, uint32_t gen, const HuEngineHandle* ref) {
    return ref && slot == ref->slot && gen == ref->generation;
}

// Under the lock: the handle may read. A read that cannot writes nil and
// the reason.
bool readable_for(const HuEngineHandle* ref, ReplyWriter& w) {
    char why[256];
    lock();
    const bool ok = usable(ref, why, sizeof(why)) != NULL;
    unlock();
    if (!ok) {
        w.fail(why);
    }
    return ok;
}

void write_size(ReplyWriter& w, const MemEntry& m, const HuEngineHandle* ref, const char* size_owner) {
    w.table();
    if (m.size_kind == kSizeRows) {
        w.set_number("rows", m.size_a);
    } else {
        w.set_number("w", m.size_a);
        w.set_number("h", m.size_b);
    }
    if (size_owner) {
        w.set_string("owner", size_owner);
    }
    w.set_bool("mine", written_by(m.size_slot, m.size_gen, ref));
    w.field("size");
}

// The remembered position and size of one window, each with its own last
// writer, as engine abi 1 and 2 give it: x, y, docked, owner, mine when a
// position is remembered, and size {rows} or {w, h} with its owner and mine
// when a size is.
void write_memory(ReplyWriter& w, const MemEntry& m, const HuEngineHandle* ref, const char* owner,
                  const char* size_owner) {
    w.table();
    if (m.active) {
        w.set_number("x", m.x);
        w.set_number("y", m.y);
        w.set_bool("docked", m.keep_dock != 0);
        if (owner) {
            w.set_string("owner", owner);
        }
        w.set_bool("mine", written_by(m.owner_slot, m.owner_gen, ref));
    }
    if (m.size_kind) {
        write_size(w, m, ref, size_owner);
    }
}

// The same as engine abi 3 gives it: {position = {x, y, group, owner, mine},
// size = {rows | w, h, owner, mine}}, either part absent when nothing is
// remembered for it. `group` names the group whose move wrote the position.
void write_memory3(ReplyWriter& w, const MemEntry& m, const HuEngineHandle* ref, const char* owner,
                   const char* size_owner) {
    w.table();
    if (m.active) {
        w.table();
        w.set_number("x", m.x);
        w.set_number("y", m.y);
        if (m.group > 0 && m.group <= kGroupCount) {
            w.set_string("group", kGroups[m.group - 1].name);
        }
        if (owner) {
            w.set_string("owner", owner);
        }
        w.set_bool("mine", written_by(m.owner_slot, m.owner_gen, ref));
        w.field("position");
    }
    if (m.size_kind) {
        write_size(w, m, ref, size_owner);
    }
}

// A writer's name, copied under the lock; empty when that handle is gone.
void owner_name(int32_t slot, uint32_t gen, char* out, size_t size) {
    out[0] = '\0';
    Handle* h = g_rt.handles.get(slot, gen);
    if (h) {
        snprintf(out, size, "%s", h->name);
    }
}

typedef void (*MemoryWriter)(ReplyWriter&, const MemEntry&, const HuEngineHandle*, const char*, const char*);

// Every window with a remembered position or size, keyed by name.
void write_positions(const HuEngineHandle* ref, HuEngineReply* reply, MemoryWriter write) {
    ReplyWriter w = {reply};
    if (!readable_for(ref, w)) {
        return;
    }
    w.table();
    for (int n = 0; n < g_engine.inv.count; ++n) {
        MemEntry m;
        if (!g_engine.memory.read(n, &m) || !remembered(m)) {
            continue;
        }
        char owner[48];
        char size_owner[48];
        lock();
        owner_name(m.owner_slot, m.owner_gen, owner, sizeof(owner));
        owner_name(m.size_slot, m.size_gen, size_owner, sizeof(size_owner));
        unlock();
        write(w, m, ref, owner[0] ? owner : NULL, size_owner[0] ? size_owner : NULL);
        w.field(g_engine.inv.names[n].name);
    }
}

void __stdcall op_positions(const HuEngineHandle* ref, HuEngineReply* reply) {
    write_positions(ref, reply, &write_memory);
}

void __stdcall op_positions3(const HuEngineHandle* ref, HuEngineReply* reply) {
    write_positions(ref, reply, &write_memory3);
}

struct InfoData {
    int n;
    LONG want;
    bool open;
    bool focused;
    const uint8_t* menu;
    const uint8_t* ctl;
    uint32_t state;
    uint32_t policy;
    uint32_t registry_policy;       // the row as the next open copies it
    uint8_t layer;
    int16_t rect[4];
    int16_t deflt[4];
    int16_t origin[2];
    bool has_cursor;
    int cursor;                     // the row; query's and arealist's, the option or row under it
    int top;                        // query's and arealist's first shown; 0 for every other window
    bool has_category;
    int32_t category;               // ability's kAbilityCategory, the list its window shows
    int16_t items;
    ElementInfo elements[kMaxElements];
    int element_count;
    bool truncated;
    const char* layout;
    MemEntry mem;
    bool mem_ok;
    char owner[48];
    char size_owner[48];
    int arrow_count;                // the target arrows': those drawn now, each its point
    int16_t arrows[2][2];
};

// What info() reports of the compass: its object's words while the global
// names one, its box, its anchor as the origin, and as the default box the
// one on the anchor the game had before the engine moved it, else the box
// it has.
void collect_compass(InfoData* d) {
    const uint8_t* c = compass_guarded();
    d->open = compass_open(c);
    if (!c) {
        return;
    }
    d->menu = c;
    d->state = c[kCompassState];
    compass_frame(c, d->rect);
    d->origin[0] = rd16(c, kCompassX);
    d->origin[1] = rd16(c, kCompassY);
    memcpy(d->deflt, d->rect, sizeof(d->deflt));
    if (g_engine.compass_home_valid) {
        const LONG home = g_engine.compass_home;
        d->deflt[0] = static_cast<int16_t>(home_x(home) - kCompassWidth);
        d->deflt[1] = static_cast<int16_t>(home_y(home) - rd16(c, kCompassHeight));
        d->deflt[2] = home_x(home);
        d->deflt[3] = home_y(home);
    }
}

// What info() reports of the target arrows: whether the target window's
// draw paints one, and while its window is live, its controller and the
// point of each arrow painted. They have no box: each is a sprite at a
// point the game takes from the target.
void collect_targetarrow(InfoData* d) {
    uint8_t* ctl = NULL;
    const uint8_t* menu = live_menu_guarded(g_engine.inv.targetwi, &ctl);
    if (!menu || !readable(ctl, kTargetBytes)) {
        return;
    }
    d->menu = ctl;
    d->open = arrows_drawn(menu, ctl, mcb_guarded(), modal_guarded());
    if (!d->open) {
        return;
    }
    if (rd16(ctl, kTargetArrowX) != 0) {
        d->arrows[d->arrow_count][0] = rd16(ctl, kTargetArrowX);
        d->arrows[d->arrow_count++][1] = rd16(ctl, kTargetArrowY);
    }
    if (ctl[kTargetPicking] != 0) {
        d->arrows[d->arrow_count][0] = rd16(ctl, kTargetPickX);
        d->arrows[d->arrow_count++][1] = rd16(ctl, kTargetPickY);
    }
}

// What info() reports of window `n`, read from this thread.
void collect_info(int n, InfoData* d) {
    memset(d, 0, sizeof(*d));
    d->n = n;
    d->want = g_engine.holds.want[n];
    if (synthetic(g_engine, n)) {
        if (is_compass(g_engine, n)) {
            collect_compass(d);
        } else {
            collect_targetarrow(d);
        }
        d->mem_ok = g_engine.memory.read(n, &d->mem) && remembered(d->mem);
        if (d->mem_ok) {
            lock();
            owner_name(d->mem.owner_slot, d->mem.owner_gen, d->owner, sizeof(d->owner));
            unlock();
        }
        return;
    }
    d->registry_policy = rd32(g_engine.game.registry + g_engine.inv.names[n].rows[0] * kRowStride, kRowPolicy);
    uint8_t* ctl = NULL;
    uint8_t* menu = open_menu_guarded(n, &ctl);
    d->open = menu != NULL;
    if (menu) {
        d->menu = menu;
        d->ctl = ctl;
        d->state = rd32(menu, kMenuState);
        d->policy = rd32(menu, kMenuPolicy);
        d->layer = menu[kMenuLayer];
        for (int i = 0; i < 4; ++i) {
            d->rect[i] = rd16(menu, kMenuRect + 2 * i);
            d->deflt[i] = rd16(menu, kMenuDefault + 2 * i);
        }
        d->origin[0] = rd16(menu, kMenuOrigin);
        d->origin[1] = rd16(menu, kMenuOrigin + 2);
        const char* nm = g_engine.inv.names[n].name;
        if (strcmp(nm, "query") == 0) {
            d->has_cursor = readable(ctl, kQueryTop + 2) && query_position(ctl, &d->cursor, &d->top);
        } else if (strcmp(nm, "arealist") == 0) {
            d->has_cursor = readable(ctl, kAreaBytes) && area_position(ctl, menu, area_count(ctl), &d->cursor, &d->top);
        } else {
            d->cursor = rd16(menu, kMenuCursor);
            d->has_cursor = true;
        }
        if (strcmp(nm, "ability") == 0 && readable(ctl, kAbilityCategory + 4)) {
            d->has_category = true;
            d->category = static_cast<int32_t>(rd32(ctl, kAbilityCategory));
        }
        d->items = rd16(menu, kMenuItems);
        d->element_count = read_elements(menu, d->elements, kMaxElements, &d->truncated);
        d->focused = rdptr(g_engine.game.mcb, kMcbActive) == menu;
        const void* set_position;
        memcpy(&set_position, &g_engine.game.set_position, sizeof(set_position));
        // Before the engine's first touch, the reading that touch would make.
        const LONG anchor = g_engine.anchor[n];
        if (anchor == kAnchorBottom || (anchor == kAnchorUnknown
                && reads_bottom_anchored(g_engine.inv.names[n].name, d->rect[1], d->rect[3], d->deflt[1],
                    d->deflt[3]))) {
            d->layout = "bottom";
        } else if (refreshes_itself(ctl)) {
            d->layout = "self";
        } else {
            d->layout = relayout_is_safe(ctl, set_position) ? "driven" : "refused";
        }
    }
    d->mem_ok = g_engine.memory.read(n, &d->mem) && remembered(d->mem);
    if (d->mem_ok) {
        lock();
        owner_name(d->mem.owner_slot, d->mem.owner_gen, d->owner, sizeof(d->owner));
        owner_name(d->mem.size_slot, d->mem.size_gen, d->size_owner, sizeof(d->size_owner));
        unlock();
    }
}

// The window `name` names, under the lock, or -1 with the reason written.
int read_target(const HuEngineHandle* ref, const char* name, bool windows, ReplyWriter& w) {
    char why[256];
    lock();
    int n = -1;
    if (usable(ref, why, sizeof(why))) {
        n = lookup(name, windows, why, sizeof(why));
    }
    unlock();
    if (n < 0) {
        w.fail(why);
    }
    return n;
}

void write_rect(ReplyWriter& w, const char* key, const int16_t* r) {
    w.table();
    w.set_number("x", r[0]);
    w.set_number("y", r[1]);
    w.set_number("right", r[2]);
    w.set_number("bottom", r[3]);
    w.field(key);
}

// {x, y, w, h}, and right and bottom with them.
void write_rect3(ReplyWriter& w, const char* key, const int16_t* r) {
    w.table();
    w.set_number("x", r[0]);
    w.set_number("y", r[1]);
    w.set_number("w", r[2] - r[0]);
    w.set_number("h", r[3] - r[1]);
    w.set_number("right", r[2]);
    w.set_number("bottom", r[3]);
    w.field(key);
}

void write_rows(ReplyWriter& w, const NameEntry& ne) {
    w.table();
    for (int i = 0; i < ne.row_count; ++i) {
        w.number(ne.rows[i]);
        w.index(i + 1);
    }
    w.field("rows");
}

// The points of the target arrows drawn now, as a list under `arrows`.
void write_arrows(ReplyWriter& w, const InfoData& d) {
    w.table();
    for (int i = 0; i < d.arrow_count; ++i) {
        w.table();
        w.set_number("x", d.arrows[i][0]);
        w.set_number("y", d.arrows[i][1]);
        w.index(i + 1);
    }
    w.field("arrows");
}

void __stdcall op_info(const HuEngineHandle* ref, const char* name, HuEngineReply* reply) {
    ReplyWriter w = {reply};
    const int n = read_target(ref, name, false, w);
    if (n < 0) {
        return;
    }
    InfoData d;
    collect_info(n, &d);

    const NameEntry& ne = g_engine.inv.names[n];
    if (is_targetarrow(g_engine, n)) {
        w.table();
        w.set_string("name", ne.name);
        w.set_bool("open", d.open);
        w.set_bool("hidden", (d.want & kWantHidden) != 0);
        w.set_bool("blocked", (d.want & kWantBlocked) != 0);
        w.set_bool("hide_only", !blockable(n));
        w.set_number("layer", 0);
        w.set_number("policy", 0);
        write_rows(w, ne);
        w.set_bool("focused", false);
        if (d.menu) {
            set_hex(w, "controller", d.menu);
            write_arrows(w, d);
        }
        return;
    }
    if (is_compass(g_engine, n)) {
        w.table();
        w.set_string("name", ne.name);
        w.set_bool("open", d.open);
        w.set_bool("hidden", (d.want & kWantHidden) != 0);
        w.set_bool("blocked", (d.want & kWantBlocked) != 0);
        w.set_bool("hide_only", !blockable(n));
        w.set_number("layer", 0);
        w.set_number("policy", 0);
        write_rows(w, ne);
        w.set_bool("focused", false);
        if (d.mem_ok) {
            write_memory(w, d.mem, ref, d.owner[0] ? d.owner : NULL, NULL);
            w.field("memory");
        }
        if (d.menu) {
            set_hex(w, "address", d.menu);
            w.set_number("state", d.state);
        }
        if (d.open) {
            write_rect(w, "rect", d.rect);
            write_rect(w, "default", d.deflt);
            w.table();
            w.set_number("x", d.origin[0]);
            w.set_number("y", d.origin[1]);
            w.field("origin");
        }
        return;
    }
    const RowSpec& spec = kRowSpecs[ne.rows[0]];
    w.table();
    w.set_string("name", ne.name);
    w.set_bool("open", d.open);
    w.set_bool("hidden", (d.want & kWantHidden) != 0);
    w.set_bool("blocked", (d.want & kWantBlocked) != 0);
    w.set_bool("hide_only", hide_only(ne.name));
    w.set_number("layer", spec.layer);
    w.set_number("policy", spec.policy);
    write_rows(w, ne);
    if (const char* dock = dock_group_of(spec.policy)) {
        w.set_string("dock", dock);
    }
    w.set_bool("focused", d.focused);
    if (const Family* f = family_of(ne.name)) {
        w.table();
        w.set_number("min", f->min);
        w.set_number("max", f->max);
        w.field("sizes");
    }
    if (d.mem_ok) {
        write_memory(w, d.mem, ref, d.owner[0] ? d.owner : NULL, d.size_owner[0] ? d.size_owner : NULL);
        w.field("memory");
    }
    if (d.open) {
        set_hex(w, "address", d.menu);
        set_hex(w, "controller", d.ctl);
        w.set_number("state", d.state);
        w.set_bool("dormant", d.state == kStateDormant);
        w.set_number("live_layer", d.layer);
        w.set_number("live_policy", d.policy);
        w.set_bool("docked", (d.policy & kDockMask) != 0);
        write_rect(w, "rect", d.rect);
        write_rect(w, "default", d.deflt);
        w.table();
        w.set_number("x", d.origin[0]);
        w.set_number("y", d.origin[1]);
        w.field("origin");
        if (d.has_cursor) {
            w.set_number("cursor", d.cursor);
        }
        if (d.top) {
            w.set_number("top", d.top);
        }
        w.set_number("items", d.items);
        w.set_string("layout", d.layout);
        w.table();
        for (int i = 0; i < d.element_count; ++i) {
            w.table();
            const ElementInfo& el = d.elements[i];
            w.set_number("type", el.type);
            if (el.placed) {
                w.set_number("x", el.x);
                w.set_number("y", el.y);
            }
            if (el.sized) {
                w.set_number("w", el.w);
                w.set_number("h", el.h);
            }
            w.set_string("text", el.text);
            w.index(i + 1);
        }
        w.field("elements");
        w.set_bool("elements_truncated", d.truncated);
    }
}

const char* element_type_name(int type) {
    switch (type) {
    case 0: return "frame";
    case 1: return "item";
    case 2: return "cursor";
    default: return "other";
    }
}

// The names of the handles `holds` says hold, as a list under `key`; true
// when `ref` is one of them.
template <typename Holding>
bool write_holding(ReplyWriter& w, const char* key, const HuEngineHandle* ref, Holding holds) {
    bool mine = false;
    w.table();
    int k = 0;
    lock();
    for (int i = 0; i < g_rt.handles.capacity; ++i) {
        const Handle& h = g_rt.handles.slots[i];
        if (h.active && holds(h)) {
            w.string(h.name);
            w.index(++k);
            mine = mine || (i == ref->slot && h.generation == ref->generation);
        }
    }
    unlock();
    w.field(key);
    return mine;
}

// The handles holding `bit` on window n.
bool write_holders(ReplyWriter& w, const char* key, int n, uint8_t bit, const HuEngineHandle* ref) {
    return write_holding(w, key, ref, [=](const Handle& h) { return (h.holds[n] & bit) != 0; });
}

// The handles blocking window n whole, and, for `category` 0 or more on
// ability, those blocking that list of it.
bool write_blockers(ReplyWriter& w, const char* key, int n, int category, const HuEngineHandle* ref) {
    const uint32_t bit = n == g_engine.inv.ability && category >= 0 && category < kCategoryBits
        ? 1u << category : 0u;
    return write_holding(w, key, ref, [=](const Handle& h) {
        return (h.holds[n] & kHoldBlock) != 0 || (h.ability_block & bit) != 0;
    });
}

// The macro keys: {blocked, blocked_by = the handles holding the block,
// mine}.
void __stdcall op_macros(const HuEngineHandle* ref, HuEngineReply* reply) {
    ReplyWriter w = {reply};
    if (!readable_for(ref, w)) {
        return;
    }
    w.table();
    w.set_bool("blocked", g_engine.holds.macros_want != 0);
    w.set_bool("mine", write_holding(w, "blocked_by", ref, [](const Handle& h) { return h.macros_hold != 0; }));
}

// The compass's and the target arrows' info: what every window has at the
// top but the layer, which they have none of, and the compass's box; the
// game's words under `detail`.
void write_compass_info(const HuEngineHandle* ref, int n, const InfoData& d, ReplyWriter& w, bool v4) {
    const NameEntry& ne = g_engine.inv.names[n];
    const bool arrows = is_targetarrow(g_engine, n);
    w.table();
    w.set_string("name", ne.name);
    w.set_bool("open", d.open);
    w.set_bool("hidden", (d.want & kWantHidden) != 0);
    w.set_bool("blocked", (d.want & kWantBlocked) != 0);
    if (v4) {
        w.set_bool("blockable", blockable(n));
        const bool hiding = write_holders(w, "hidden_by", n, kHoldHide, ref);
        const bool blocking = write_holders(w, "blocked_by", n, kHoldBlock, ref);
        w.table();
        w.set_bool("hidden", hiding);
        w.set_bool("blocked", blocking);
        w.field("mine");
    } else {
        w.set_bool("hide_only", !blockable(n));
    }
    w.set_bool("docked", false);
    w.set_bool("covered", false);
    w.set_bool("focused", false);
    w.table();
    w.set_string("holds", "none");
    w.field(v4 ? "resize" : "sizes");
    if (d.mem_ok) {
        write_memory3(w, d.mem, ref, d.owner[0] ? d.owner : NULL, NULL);
        w.field("memory");
    }
    if (d.open && !arrows) {
        write_rect3(w, "rect", d.rect);
        write_rect3(w, "default", d.deflt);
        w.table();
        w.set_number("x", d.origin[0]);
        w.set_number("y", d.origin[1]);
        w.field("origin");
    }
    w.table();
    w.set_number("policy", 0);
    write_rows(w, ne);
    if (d.menu && arrows) {
        set_hex(w, "controller", d.menu);
        write_arrows(w, d);
    } else if (d.menu) {
        set_hex(w, "address", d.menu);
        w.set_number("state", d.state);
        w.set_number("x", d.origin[0]);
        w.set_number("y", d.origin[1]);
        w.set_number("height", d.rect[3] - d.rect[1]);
    }
    w.field("detail");
}

// The list ability's window shows, by its controller's category (core.h's
// kAbilityLists); NULL for a value no list has.
const char* category_name(int32_t category) {
    return ability_list_name(category);
}

// The lists of ability in `mask`, each by its name, or by its number where
// no list has it, as a list under `key`.
void write_categories(ReplyWriter& w, const char* key, uint32_t mask) {
    w.table();
    int k = 0;
    for (int c = 0; c < kCategoryBits; ++c) {
        if (!(mask & (1u << c))) {
            continue;
        }
        if (const char* named = category_name(c)) {
            w.string(named);
        } else {
            w.number(c);
        }
        w.index(++k);
    }
    w.field(key);
}

// The lists of ability the handle blocks, as a mask; 0 for a released one.
uint32_t own_categories(const HuEngineHandle* ref) {
    lock();
    const Handle* h = g_rt.handles.get(ref->slot, ref->generation);
    const uint32_t mask = h ? h->ability_block : 0;
    unlock();
    return mask;
}

// Engine abi 3's info, and with `v4` abi 4's: what an addon draws by at the
// top, the engine's and the game's own words under `detail`. Abi 4 has
// `blockable` for abi 3's `hide_only`, the handles holding each hide and
// block, `resize` for `sizes`, and `elements_truncated` beside `elements`.
// On ability, `blocked_categories` lists the lists any handle blocks, and
// mine.blocked_categories this handle's.
void write_info(const HuEngineHandle* ref, const char* name, HuEngineReply* reply, bool v4) {
    HU_STEP("write_info");
    ReplyWriter w = {reply};
    const int n = read_target(ref, name, true, w);
    if (n < 0) {
        return;
    }
    InfoData d;
    collect_info(n, &d);

    if (synthetic(g_engine, n)) {
        write_compass_info(ref, n, d, w, v4);
        return;
    }
    const NameEntry& ne = g_engine.inv.names[n];
    const RowSpec& spec = kRowSpecs[ne.rows[0]];
    const Family* f = family_of(ne.name);
    w.table();
    w.set_string("name", ne.name);
    w.set_bool("open", d.open);
    w.set_bool("hidden", (d.want & kWantHidden) != 0);
    w.set_bool("blocked", (d.want & kWantBlocked) != 0);
    const bool lists = n == g_engine.inv.ability;
    if (lists) {
        write_categories(w, "blocked_categories", static_cast<uint32_t>(g_engine.holds.ability_want));
    }
    if (v4) {
        w.set_bool("blockable", blockable(n));
        const bool hiding = write_holders(w, "hidden_by", n, kHoldHide, ref);
        const bool blocking = write_holders(w, "blocked_by", n, kHoldBlock, ref);
        w.table();
        w.set_bool("hidden", hiding);
        w.set_bool("blocked", blocking);
        if (lists) {
            write_categories(w, "blocked_categories", own_categories(ref));
        }
        w.field("mine");
    } else {
        w.set_bool("hide_only", hide_only(ne.name));
    }
    w.set_number("layer", spec.layer);
    w.set_bool("docked", ((d.open ? d.policy : d.registry_policy) & kDockMask) != 0);
    w.set_bool("covered", d.open && d.state == kStateDormant);
    w.set_bool("focused", d.focused);
    w.table();
    w.set_string("holds", size_holds(ne.name));
    if (f) {
        w.set_number(v4 ? "min_rows" : "min", f->min);
        w.set_number(v4 ? "max_rows" : "max", f->max);
    }
    w.field(v4 ? "resize" : "sizes");
    if (d.mem_ok) {
        write_memory3(w, d.mem, ref, d.owner[0] ? d.owner : NULL, d.size_owner[0] ? d.size_owner : NULL);
        w.field("memory");
    }
    if (d.open) {
        write_rect3(w, "rect", d.rect);
        write_rect3(w, "default", d.deflt);
        w.table();
        w.set_number("x", d.origin[0]);
        w.set_number("y", d.origin[1]);
        w.field("origin");
        if (d.has_cursor) {
            w.set_number("cursor", d.cursor);
        }
        if (d.top) {
            w.set_number("top", d.top);
        }
        if (d.has_category) {
            w.set_number("category", d.category);
            if (const char* named = category_name(d.category)) {
                w.set_string("category_name", named);
            }
        }
        w.set_number("items", d.items);
        w.table();
        for (int i = 0; i < d.element_count; ++i) {
            w.table();
            const ElementInfo& el = d.elements[i];
            w.set_string("type", element_type_name(el.type));
            if (el.placed) {
                w.set_number("x", el.x);
                w.set_number("y", el.y);
            }
            if (el.sized) {
                w.set_number("w", el.w);
                w.set_number("h", el.h);
                w.set_number("right", el.x + el.w);
                w.set_number("bottom", el.y + el.h);
            }
            w.set_string("text", el.text);
            w.index(i + 1);
        }
        w.field("elements");
        if (v4) {
            w.set_bool("elements_truncated", d.truncated);
        }
    }
    w.table();
    w.set_number("policy", spec.policy);
    write_rows(w, ne);
    if (const char* dock = dock_group_of(spec.policy)) {
        w.set_string("dock", dock);
    }
    if (d.open) {
        set_hex(w, "address", d.menu);
        set_hex(w, "controller", d.ctl);
        w.set_number("state", d.state);
        w.set_number("live_layer", d.layer);
        w.set_number("live_policy", d.policy);
        w.set_string("layout", d.layout);
        if (!v4) {
            w.set_bool("elements_truncated", d.truncated);
        }
    }
    w.field("detail");
}

void __stdcall op_info3(const HuEngineHandle* ref, const char* name, HuEngineReply* reply) {
    write_info(ref, name, reply, false);
}

void __stdcall op_info4(const HuEngineHandle* ref, const char* name, HuEngineReply* reply) {
    write_info(ref, name, reply, true);
}

void __stdcall op_list(const HuEngineHandle* ref, HuEngineReply* reply) {
    ReplyWriter w = {reply};
    if (!readable_for(ref, w)) {
        return;
    }
    w.table();
    for (int n = 0; n < g_engine.inv.count; ++n) {
        const NameEntry& ne = g_engine.inv.names[n];
        const RowSpec& spec = spec_of(ne);
        const LONG want = g_engine.holds.want[n];
        MemEntry m;
        const bool read = g_engine.memory.read(n, &m);
        const bool open = open_of(n);
        w.table();
        w.set_string("name", ne.name);
        w.set_number("layer", spec.layer);
        w.set_number("policy", spec.policy);
        w.set_number("avail", spec.avail);
        w.set_number("overlap", spec.overlap);
        w.set_bool("open", open);
        w.set_bool("hidden", (want & kWantHidden) != 0);
        w.set_bool("blocked", (want & kWantBlocked) != 0);
        w.set_bool("moved", read && m.active);
        w.set_bool("resized", read && m.size_kind != 0);
        w.set_bool("hide_only", !blockable(n));
        if (const char* dock = dock_group_of(spec.policy)) {
            w.set_string("dock", dock);
        }
        w.index(n + 1);
    }
}

// Every window keyed by name: its state at the top, the registry's words
// under `detail`. Abi 4 has `blockable` for abi 3's `hide_only`.
void write_list(const HuEngineHandle* ref, HuEngineReply* reply, bool v4) {
    HU_STEP("write_list");
    ReplyWriter w = {reply};
    if (!readable_for(ref, w)) {
        return;
    }
    w.table();
    for (int n = 0; n < g_engine.inv.count; ++n) {
        const NameEntry& ne = g_engine.inv.names[n];
        const RowSpec& spec = spec_of(ne);
        const LONG want = g_engine.holds.want[n];
        MemEntry m;
        const bool read = g_engine.memory.read(n, &m);
        w.table();
        w.set_string("name", ne.name);
        w.set_bool("open", open_of(n));
        w.set_bool("hidden", (want & kWantHidden) != 0);
        w.set_bool("blocked", (want & kWantBlocked) != 0);
        if (v4) {
            w.set_bool("blockable", blockable(n));
        } else {
            w.set_bool("hide_only", !blockable(n));
        }
        if (!synthetic(g_engine, n)) {
            w.set_number("layer", spec.layer);
        }
        w.set_bool("moved", read && m.active);
        w.set_bool("resized", read && m.size_kind != 0);
        w.table();
        w.set_number("policy", spec.policy);
        w.set_number("avail", spec.avail);
        w.set_number("overlap", spec.overlap);
        if (const char* dock = dock_group_of(spec.policy)) {
            w.set_string("dock", dock);
        }
        w.field("detail");
        w.field(ne.name);
    }
}

void __stdcall op_list3(const HuEngineHandle* ref, HuEngineReply* reply) {
    write_list(ref, reply, false);
}

void __stdcall op_list4(const HuEngineHandle* ref, HuEngineReply* reply) {
    write_list(ref, reply, true);
}

// The names of the open windows, in the registry's order.
void __stdcall op_opened(const HuEngineHandle* ref, HuEngineReply* reply) {
    ReplyWriter w = {reply};
    if (!readable_for(ref, w)) {
        return;
    }
    w.table();
    int k = 0;
    for (int n = 0; n < g_engine.inv.count; ++n) {
        if (open_of(n)) {
            w.string(g_engine.inv.names[n].name);
            w.index(++k);
        }
    }
}

// The manager's active menu (mcb+0x54) by name while it is open; when it is
// none of the registry's, closing, or none at all, nil (abi 3) or false
// (abi 4).
void write_focused(const HuEngineHandle* ref, HuEngineReply* reply, bool v4) {
    ReplyWriter w = {reply};
    if (!readable_for(ref, w)) {
        return;
    }
    const uint8_t* menu = rdptr(g_engine.game.mcb, kMcbActive);
    const uint8_t* res = menu && readable(menu, kMenuBytes) && menu_is_open(menu) ? rdptr(menu, kMenuRes) : NULL;
    const int n = res && readable(res + kResKey, kKeyLen) ? g_engine.inv.find_key16(res + kResKey) : -1;
    if (n >= 0) {
        w.string(g_engine.inv.names[n].name);
    } else if (v4) {
        w.boolean(false);
    } else {
        w.nil();
    }
}

void __stdcall op_focused(const HuEngineHandle* ref, HuEngineReply* reply) {
    write_focused(ref, reply, false);
}

void __stdcall op_focused4(const HuEngineHandle* ref, HuEngineReply* reply) {
    write_focused(ref, reply, true);
}

void write_members(ReplyWriter& w, int g, const uint32_t* policy, int anchor) {
    uint8_t in_set[kMaxNames];
    group_closure(g_engine.inv, g, policy, in_set);
    w.table();
    int k = 0;
    for (int n = 0; n < g_engine.inv.count; ++n) {
        if (in_set[n] && n != anchor) {
            w.string(g_engine.inv.names[n].name);
            w.index(++k);
        }
    }
    w.field("members");
}

// `version` 1 for the table's first slot (name, index and mask too), 3, or
// 4: the anchor's origin while it is open, and a move waiting for it.
void write_groups(const HuEngineHandle* ref, HuEngineReply* reply, int version) {
    ReplyWriter w = {reply};
    if (!readable_for(ref, w)) {
        return;
    }
    uint32_t policy[kMaxNames];
    for (int n = 0; n < g_engine.inv.count; ++n) {
        policy[n] = spec_of(g_engine.inv.names[n]).policy;
    }
    w.table();
    for (int g = 0; g < kGroupCount; ++g) {
        const GroupSpec& spec = kGroups[g];
        const int anchor = g_engine.inv.find_exact(spec.anchor);
        const uint8_t* menu = open_menu_guarded(anchor, NULL);
        w.table();
        if (version < 3) {
            w.set_string("name", spec.name);
            w.set_number("index", g);
            w.set_number("mask", spec.mask);
        }
        w.set_string("anchor", spec.anchor);
        w.set_bool("anchor_open", menu != NULL);
        if (version >= 4 && menu) {
            w.table();
            w.set_number("x", rd16(menu, kMenuOrigin));
            w.set_number("y", rd16(menu, kMenuOrigin + 2));
            w.field("origin");
        }
        write_members(w, g, policy, anchor);
        GroupWait wait;
        if (version >= 4 && g_engine.waiting.read(g, &wait) && wait.active) {
            char owner[48];
            lock();
            owner_name(wait.slot, wait.gen, owner, sizeof(owner));
            unlock();
            w.table();
            w.set_number("x", wait.x);
            w.set_number("y", wait.y);
            if (owner[0]) {
                w.set_string("owner", owner);
            }
            w.set_bool("mine", written_by(wait.slot, wait.gen, ref));
            w.field("waiting");
        }
        w.field(spec.name);
    }
}

void __stdcall op_groups(const HuEngineHandle* ref, HuEngineReply* reply) {
    write_groups(ref, reply, 1);
}

void __stdcall op_groups3(const HuEngineHandle* ref, HuEngineReply* reply) {
    write_groups(ref, reply, 3);
}

void __stdcall op_groups4(const HuEngineHandle* ref, HuEngineReply* reply) {
    write_groups(ref, reply, 4);
}

// Everything this handle wrote last, in the form apply() replays: a group
// move as its anchor's position, taken from the anchor's entry (the members
// it carried follow it again) or from the move waiting for the anchor to
// open, which replaces it; a move of a window as its position, and each
// size. Only string keys and numbers, and no empty table, so Windower's
// config library saves it as it is.
void __stdcall op_layout(const HuEngineHandle* ref, HuEngineReply* reply) {
    ReplyWriter w = {reply};
    if (!readable_for(ref, w)) {
        return;
    }
    GroupWait wait[kGroupCount];
    bool waiting[kGroupCount];
    bool any_group = false;
    for (int g = 0; g < kGroupCount; ++g) {
        waiting[g] = g_engine.waiting.read(g, &wait[g]) && wait[g].active;
        any_group = any_group || (waiting[g] && written_by(wait[g].slot, wait[g].gen, ref));
    }
    MemEntry mem[kMaxNames];
    bool read[kMaxNames];
    bool any_position = false;
    bool any_size = false;
    for (int n = 0; n < g_engine.inv.count; ++n) {
        read[n] = g_engine.memory.read(n, &mem[n]);
        const MemEntry& m = mem[n];
        if (!read[n]) {
            continue;
        }
        if (m.active && written_by(m.owner_slot, m.owner_gen, ref)) {
            const bool by_group = m.group > 0 && m.group <= kGroupCount;
            if (!by_group) {
                any_position = true;
            } else if (!waiting[m.group - 1] && n == g_engine.inv.find_exact(kGroups[m.group - 1].anchor)) {
                any_group = true;
            }
        }
        any_size = any_size || (m.size_kind && written_by(m.size_slot, m.size_gen, ref));
    }
    w.table();
    if (any_group) {
        w.table();
        for (int g = 0; g < kGroupCount; ++g) {
            const int a = g_engine.inv.find_exact(kGroups[g].anchor);
            if (waiting[g]) {
                if (written_by(wait[g].slot, wait[g].gen, ref)) {
                    w.table();
                    w.set_number("x", wait[g].x);
                    w.set_number("y", wait[g].y);
                    w.field(kGroups[g].name);
                }
            } else if (a >= 0 && read[a]) {
                const MemEntry& m = mem[a];
                if (m.active && m.group == g + 1 && written_by(m.owner_slot, m.owner_gen, ref)) {
                    w.table();
                    w.set_number("x", m.x);
                    w.set_number("y", m.y);
                    w.field(kGroups[g].name);
                }
            }
        }
        w.field("groups");
    }
    if (any_position) {
        w.table();
        for (int n = 0; n < g_engine.inv.count; ++n) {
            const MemEntry& m = mem[n];
            if (read[n] && m.active && m.group == 0 && written_by(m.owner_slot, m.owner_gen, ref)) {
                w.table();
                w.set_number("x", m.x);
                w.set_number("y", m.y);
                w.field(g_engine.inv.names[n].name);
            }
        }
        w.field("positions");
    }
    if (any_size) {
        w.table();
        for (int n = 0; n < g_engine.inv.count; ++n) {
            const MemEntry& m = mem[n];
            if (read[n] && m.size_kind && written_by(m.size_slot, m.size_gen, ref)) {
                w.table();
                if (m.size_kind == kSizeRows) {
                    w.set_number("rows", m.size_a);
                } else {
                    w.set_number("w", m.size_a);
                    w.set_number("h", m.size_b);
                }
                w.field(g_engine.inv.names[n].name);
            }
        }
        w.field("sizes");
    }
}

// A decoded title, option or page: the plain text under `text_key`, its
// runs under `segments_key` as {text, color, escape}, and its '?' count
// under `undecoded_key`. Shift-JIS, the game's own encoding.
template <typename Text>
void write_glyph_text(ReplyWriter& w, const Text& t, const char* text_key, const char* segments_key,
        const char* undecoded_key) {
    w.set_string(text_key, t.text);
    w.table();
    for (int i = 0; i < t.run_count; ++i) {
        char part[sizeof(t.text)];
        memcpy(part, t.text + t.runs[i].start, t.runs[i].length);
        part[t.runs[i].length] = '\0';
        w.table();
        w.set_string("text", part);
        w.set_number("color", t.runs[i].color);
        w.set_number("escape", t.runs[i].escape);
        w.index(i + 1);
    }
    w.field(segments_key);
    w.set_number(undecoded_key, t.undecoded);
}

// The open NPC choice as query's controller parsed it out of the event:
// {title, title_segments, title_undecoded, options = {{text, segments,
// undecoded, value}, ...}} in list order, the nodes the game has tombstoned
// left out; with `title_table`, abi 4's title = {text, segments, undecoded}.
// Each text is its glyph codes decoded by their count, two-byte characters
// through the table the game thread built; `value` is the option's word at
// +0x104, the one the answer writes.
void write_query(ReplyWriter& w, const uint8_t* ctl, bool title_table) {
    GlyphText text;
    decode_glyphs(ctl + kQueryTitle, ctl[kQueryTitleCount], &g_engine.glyphs, &text);
    w.table();
    if (title_table) {
        w.table();
        write_glyph_text(w, text, "text", "segments", "undecoded");
        w.field("title");
    } else {
        write_glyph_text(w, text, "title", "title_segments", "title_undecoded");
    }
    w.table();
    int k = 0;
    const uint8_t* node = rdptr(ctl, kQueryOptions);
    for (int steps = 0; node && steps < kQueryListMax && readable(node, 0x15); ++steps) {
        const uint8_t* item = rdptr(node, 0x10);
        if (node[0x14] == 0 && readable(item, kOptionBytes)) {
            decode_glyphs(item + kOptionGlyphs, item[kOptionCount], &g_engine.glyphs, &text);
            w.table();
            write_glyph_text(w, text, "text", "segments", "undecoded");
            w.set_number("value", static_cast<uint16_t>(rd16(item, kOptionValue)));
            w.index(++k);
        }
        node = rdptr(node, 0);
    }
    w.field("options");
}

// query's controller with its title readable, or NULL.
const uint8_t* query_parsed() {
    const int n = window_named("query");
    uint8_t* ctl = NULL;
    if (n < 0 || !open_menu_guarded(n, &ctl) || !readable(ctl, kQueryTitle + 2 * kGlyphMax)) {
        return NULL;
    }
    return ctl;
}

void __stdcall op_query_options(const HuEngineHandle* ref, HuEngineReply* reply) {
    ReplyWriter w = {reply};
    if (!readable_for(ref, w)) {
        return;
    }
    const uint8_t* ctl = query_parsed();
    if (!ctl) {
        w.fail("query is not open");
        return;
    }
    write_query(w, ctl, false);
}

// The pending invite as prtyjoin's controller holds it: the inviter's 16
// bytes, and whether it is to an alliance. Engine abi 5 keeps `name` for
// the prompt's and has the inviter as `inviter`.
void write_invite(ReplyWriter& w, const uint8_t* flags, bool v5) {
    char inviter[kPartyNameBytes + 1];
    inviter[0] = '\0';
    const uint8_t* ctl = controller_guarded("prtyjoin", kPartyName + kPartyNameBytes);
    if (ctl) {
        memcpy(inviter, ctl + kPartyName, kPartyNameBytes);
        inviter[kPartyNameBytes] = '\0';
    }
    w.set_string(v5 ? "inviter" : "name", inviter);
    w.set_bool("alliance", flags[kPartyKind] == 0);
}

// The kind of arealist row i, by its id and the list it is in: the top of
// mode 0's and mode 3's lists opens on the current area; mode 4's rows are
// no areas.
const char* area_kind(int mode, int level, int i, int id) {
    if (mode == kAreaModeOther) {
        return "other";
    }
    if (i == 0 && level == 0 && (mode == 0 || mode == 3)) {
        return "current_area";
    }
    if (id == -1) {
        return "current_region";
    }
    if (id == 0) {
        return "all";
    }
    return id <= -2 ? "region" : "zone";
}

const size_t kAreaText = 64;

// arealist's rows in order, each {text, id, kind} and column 1's value: its
// text as `label`, or a region's count of zones as `count`; the mode, the
// level (0 the top, a region's -id inside it) and pending, true. With
// `blocked`, an event's choice with no window: no rows. Engine abi 3 and 4
// have the zone rows alone, each {zone}, under `zones`.
void write_area(ReplyWriter& w, const uint8_t* ctl, bool v5, bool blocked) {
    const uint8_t* rows = NULL;
    const int count = blocked ? 0 : area_rows_guarded(ctl, &rows);
    const int mode = ctl[kAreaMode];
    const int level = rd16(ctl, kAreaLevel);
    w.table();
    if (v5) {
        w.set_number("mode", mode);
        w.set_number("level", level);
        w.set_bool("pending", true);
    }
    w.table();
    int k = 0;
    int id = 0;
    for (int i = 0; i < count && area_id_guarded(rows, i, &id); ++i) {
        const uint8_t* row = rows + i * kAreaRowStride;
        const char* kind = area_kind(mode, level, i, id);
        if (!v5) {
            if (strcmp(kind, "zone") == 0) {
                w.table();
                w.set_number("zone", id);
                w.index(++k);
            }
            continue;
        }
        char text[kAreaText];
        copy_text(reinterpret_cast<const char*>(rdptr(row, kAreaRowValue)), text, sizeof(text));
        w.table();
        w.set_string("text", text);
        w.set_number("id", id);
        w.set_string("kind", kind);
        const uint8_t format = row[kAreaRowFormat + 1];
        const uint8_t* value = rdptr(row, kAreaRowValue + 4);
        if ((format == kFormatText || format == kFormatTextParens) && value) {
            copy_text(reinterpret_cast<const char*>(value), text, sizeof(text));
            w.set_string("label", text);
        } else if (format == kFormatCount) {
            w.set_number("count", static_cast<int32_t>(rd32(row, kAreaRowValue + 4)));
        }
        w.index(++k);
    }
    w.field(v5 ? "rows" : "zones");
}

const size_t kYesNoTextBytes = 512;

// The answer yes/no window n offers first: the row the drain saw its cursor
// on at this open, else the cursor now; with no window, the default of a
// question asked again, the one default the controller keeps.
bool yesno_default_guarded(int n, const uint8_t* ctl) {
    const uint8_t* menu = open_menu_guarded(n, NULL);
    if (!menu) {
        return ctl[kYesNoChainedYes] != 0;
    }
    const LONG first = g_engine.yesno_first[n];
    MemoryBarrier();
    if (first_row_is(first, g_engine.instance[n])) {
        return first_row_yes(first);
    }
    return rd16(menu, kMenuCursor) == kRowYes;
}

// The question yes/no window n asks: {text, segments, undecoded} decoded
// from the game's own text as decode_text reads it, default, cancellable,
// hold (a confirm the player holds through holdtime), and the mode under
// `detail`.
void write_yesno(ReplyWriter& w, int n, const uint8_t* ctl) {
    char raw[kYesNoTextBytes];
    copy_text(reinterpret_cast<const char*>(rdptr(ctl, kYesNoText)), raw, sizeof(raw));
    GlyphText text;
    decode_text(reinterpret_cast<const uint8_t*>(raw), strlen(raw), &text);
    const int32_t mode = static_cast<int32_t>(rd32(ctl, kYesNoMode));
    w.table();
    write_glyph_text(w, text, "text", "segments", "undecoded");
    w.set_bool("default", yesno_default_guarded(n, ctl));
    w.set_bool("cancellable", ctl[kYesNoCancellable] != 0);
    w.set_bool("hold", mode != 0);
    w.table();
    w.set_number("mode", mode);
    w.field("detail");
}

// The KO window's countdown, in whole seconds as the window draws it, and
// whether answer('dead') would ask now; the countdown's raw fields and the
// home-point byte under `detail`.
void write_dead(ReplyWriter& w, const uint8_t* ctl) {
    const int32_t remaining = static_cast<int32_t>(rd32(ctl, kDeadRemaining));
    w.table();
    w.set_number("seconds", remaining > 0 ? remaining / kDeadTicks : 0);
    w.set_bool("askable", dead_refusal_guarded() == NULL);
    w.table();
    w.set_number("remaining", remaining);
    w.set_number("deadline", rd32(ctl, kDeadDeadline));
    w.set_bool("chosen", ctl[kDeadChosen] != 0);
    w.field("detail");
}

// A number box's question: the number its window shows, the numbers it
// takes, whether a block kept its window from opening, and for an item
// count asked for an item picked from the inventory that item's id; the
// callback, its context and the asker under `detail`.
void write_box(ReplyWriter& w, const BoxQuestion& q) {
    const int b = q.box;
    const void* callback = box_callback(q.ctl, b);
    const uint8_t* context = rdptr(q.ctl, kBoxLayouts[b].context);
    w.table();
    w.set_number("value", box_value(q.ctl, b));
    w.set_number("min", kBoxLayouts[b].min);
    w.set_number("max", box_max(q.ctl, b));
    w.set_bool("blocked", q.blocked);
    if (q.asker >= 0 && kAskers[q.asker].item && readable(context, 2)) {
        w.set_number("item", static_cast<uint16_t>(rd16(context, 0)));
    }
    w.table();
    set_hex(w, "callback", callback);
    set_hex(w, "context", context);
    w.set_string("asker", q.asker >= 0 ? kAskers[q.asker].name : "unknown");
    w.field("detail");
}

// The game's text at `p`, at most `bytes` of it, decoded as decode_text
// reads it.
void decode_game_text(const char* p, size_t bytes, GlyphText* out) {
    char raw[kYesNoTextBytes];
    copy_text(p, raw, bytes < sizeof(raw) ? bytes + 1 : sizeof(raw));
    decode_text(reinterpret_cast<const uint8_t*>(raw), strlen(raw), out);
}

// {text, segments, undecoded} of the game's text at `p` as a table under
// `key`, or onto the table being written when `key` is NULL.
void write_game_text(ReplyWriter& w, const char* p, size_t bytes, const char* key) {
    GlyphText text;
    decode_game_text(p, bytes, &text);
    if (key) {
        w.table();
    }
    write_glyph_text(w, text, "text", "segments", "undecoded");
    if (key) {
        w.field(key);
    }
}

// The answer a boolean prompt's window offers first: the row the drain saw
// its cursor on at this open, else the cursor now.
bool first_row_yes_guarded(int n, const uint8_t* menu) {
    const LONG first = g_engine.yesno_first[n];
    MemoryBarrier();
    if (first_row_is(first, g_engine.instance[n])) {
        return first_row_yes(first);
    }
    return rd16(menu, kMenuCursor) == kRowYes;
}

// The text a window's open hook gave its list row `row`, the list's nodes
// and items guarded; NULL for none.
const char* row_text_guarded(const uint8_t* menu, int row) {
    const uint8_t* node = readable(menu + kMenuList, 4) ? rdptr(menu, kMenuList) : NULL;
    for (int steps = 0; node && steps < 512 && readable(node, 0x15); ++steps) {
        const uint8_t* item = rdptr(node, 0x10);
        if (node[0x14] == 0 && readable(item, kItemRowText + 4) && rd16(item, 0) == 1 && rd16(item, kItemRowId) == row) {
            return reinterpret_cast<const char*>(rdptr(item, kItemRowText));
        }
        node = rdptr(node, 0);
    }
    return NULL;
}

// maplist's rows in order, each {text, value, kind} and column 1's text as
// `label` or count as `count`: kind 'map' for a zone's map, with its zone
// and map; 'list' for a row whose answer opens a list of its own; 'close'
// for the row that closes the list. value is the row's place, what answer()
// takes.
void write_map_rows(ReplyWriter& w, const uint8_t* ctl) {
    const uint8_t* rows = rdptr(ctl, kMapRows);
    const int count = map_count(ctl);
    w.table();
    for (int i = 0; i < count; ++i) {
        const uint8_t* entry = map_entry(ctl, i);
        if (!entry) {
            break;
        }
        const uint8_t* row = rows + i * kAreaRowStride;
        const int32_t id = static_cast<int32_t>(rd32(entry, 0));
        char text[kAreaText];
        copy_text(reinterpret_cast<const char*>(rdptr(row, kAreaRowValue)), text, sizeof(text));
        w.table();
        w.set_string("text", text);
        w.set_number("value", i + 1);
        w.set_string("kind", id == kMapCloseRow ? "close" : id >= 0 ? "map" : "list");
        if (id >= 0 && id != kMapCloseRow) {
            w.set_number("zone", id);
            w.set_number("map", static_cast<int32_t>(rd32(entry, 4)));
        }
        const uint8_t format = row[kAreaRowFormat + 1];
        const uint8_t* value = rdptr(row, kAreaRowValue + 4);
        if ((format == kFormatText || format == kFormatTextParens) && value) {
            copy_text(reinterpret_cast<const char*>(value), text, sizeof(text));
            w.set_string("label", text);
        } else if (format == kFormatCount) {
            w.set_number("count", static_cast<int32_t>(rd32(row, kAreaRowValue + 4)));
        }
        w.index(i + 1);
    }
    w.field("rows");
}

// One of mogdoor's rows: its text, its kind, its exit's value unless it
// has none (-1: the stay row, cancel()'s, and the row that opens the list
// of areas, which the values of `areas` stand for), a zone, and whether the
// window takes it now.
void write_door_row(ReplyWriter& w, const char* text, const char* kind, int32_t value, int32_t zone, bool enabled,
                    int index) {
    w.table();
    write_game_text(w, text, kDoorTextBytes - 1, NULL);
    w.set_string("kind", kind);
    if (value >= 0) {
        w.set_number("value", value);
    }
    if (zone >= 0) {
        w.set_number("zone", zone);
    }
    w.set_bool("enabled", enabled);
    w.index(index);
}

// mogdoor's question: its title; level, 'top' or 'areas', the one it
// shows (the top for a question its block kept from opening); `options`,
// the rows of that level; and `areas`, the list's, each value an answer at
// either level. The texts are the draw's, read on the game thread for this
// prompt.
void write_door(ReplyWriter& w, const AskedQuestion& q) {
    const uint8_t* ctl = q.now.ctl;
    DoorTexts t;
    memset(&t, 0, sizeof(t));
    const bool texts = g_engine.door_texts.read(&t) && t.ready && t.id == q.id;
    uint16_t zones[kDoorListMax];
    const int count = door_areas(g_engine, ctl, !q.blocked, q.record.row & 0xFF, zones);
    const bool top = q.blocked || ctl[kDoorLevel] == kDoorTop;
    const int32_t garden = g_engine.game.asked.door.garden;
    write_game_text(w, texts ? t.title : "", kDoorTextBytes - 1, "title");
    w.set_string("level", top ? "top" : "areas");
    for (int list = 0; list < 2; ++list) {
        w.table();
        if (list == 0 && top) {
            const int32_t floor = door_floor_exit(q.now);
            write_door_row(w, texts ? t.rows[0] : "", "stay", -1, -1, true, 1);
            write_door_row(w, texts ? t.rows[1] : "", "back", kDoorBack, -1, true, 2);
            write_door_row(w, texts ? t.rows[2] : "", "floor", floor, -1, floor >= 0 && ctl[kDoorFloor] != 0, 3);
            write_door_row(w, texts ? t.rows[3] : "", "areas", -1, -1, true, 4);
        } else {
            for (int i = 0; i < count; ++i) {
                const bool is_garden = zones[i] == garden;
                write_door_row(w, texts ? t.areas[i] : "", is_garden ? "garden" : "area",
                    door_area_exit(g_engine, zones[i], i), zones[i], !is_garden || ctl[kDoorGarden] != 0, i + 1);
            }
        }
        w.field(list == 0 ? "options" : "areas");
    }
}

// An Asked window's question, in plain fields: its text as the game's
// readers decode it, its choices, whether a block kept its window from
// opening; the controller's words under `detail`.
void write_asked(ReplyWriter& w, const AskedQuestion& q) {
    const int a = q.asked;
    const int n = g_engine.inv.asked_window[a];
    const uint8_t* ctl = q.now.ctl;
    const uint8_t* menu = q.now.menu;
    const bool blocked_kind = asked_records(a) || a == kAskedBank;
    if (a == kAskedGmtell) {
        write_query(w, ctl, true);
    } else {
        w.table();
    }
    switch (a) {
    case kAskedSortyn:
        write_game_text(w, reinterpret_cast<const char*>(rdptr(ctl, kSortynText)), kYesNoTextBytes - 1, NULL);
        write_game_text(w, reinterpret_cast<const char*>(rdptr(ctl, kSortynTitle)), kYesNoTextBytes - 1, "title");
        break;
    case kAskedEvitem:
        write_game_text(w, reinterpret_cast<const char*>(ctl + kEvitemText), kEvitemTextBytes - 1, NULL);
        break;
    case kAskedBank: {
        w.table();
        const int32_t ids[2] = {kBankSafe, kBankSafe2};
        for (int i = 0; i < 2; ++i) {
            w.table();
            w.set_number("id", ids[i]);
            w.set_bool("available", i == 0 || q.now.safe2 == 1);
            w.index(i + 1);
        }
        w.field("containers");
        break;
    }
    case kAskedMaplist: {
        static const char* const levels[] = {"top", "region", "zone"};
        write_map_rows(w, ctl);
        w.set_bool("top", map_top(ctl));
        w.set_string("level", levels[map_level(ctl)]);
        break;
    }
    case kAskedGmtell: {
        char gm[kGmTellNameBytes + 1];
        gm[0] = '\0';
        const AskedSites& s = g_engine.game.asked;
        const int32_t slot = readable(s.gm_slot, 4) ? *s.gm_slot : -1;
        if (slot >= 0 && slot < kGmTellSlots) {
            copy_text(reinterpret_cast<const char*>(s.gm_queue + slot * kGmTellSlotBytes), gm, sizeof(gm));
        }
        w.set_string("gm", gm);
        w.set_bool("cancellable", true);
        break;
    }
    case kAskedMpDead: {
        w.table();
        for (int row = 1; row <= 2; ++row) {
            w.table();
            write_game_text(w, row_text_guarded(menu, row), kYesNoTextBytes - 1, NULL);
            w.set_number("value", row);
            w.index(row);
        }
        w.field("options");
        const int32_t remaining = static_cast<int32_t>(rd32(ctl, kMpDeadRemaining));
        w.set_number("seconds", remaining > 0 ? remaining / kDeadTicks : 0);
        break;
    }
    case kAskedOk:
        write_game_text(w, reinterpret_cast<const char*>(ctl + kOkText), kOkTextBytes - 1, NULL);
        break;
    case kAskedDoor:
        write_door(w, q);
        break;
    default:
        break;
    }
    if (boolean_window(kAskedNames[a]) && menu) {
        w.set_bool("default", first_row_yes_guarded(n, menu));
    }
    if (a == kAskedSortyn || a == kAskedEvitem) {
        w.set_bool("cancellable", true);
    }
    if (blocked_kind) {
        w.set_bool("blocked", q.blocked);
    }
    w.table();
    switch (a) {
    case kAskedSortyn:
        set_hex(w, "callback", rdptr(ctl, kSortynCallback));
        set_hex(w, "context", rdptr(ctl, kSortynContext));
        if (q.blocked && q.record.owner_n >= 0) {
            w.set_string("asker", g_engine.inv.names[q.record.owner_n].name);
        }
        break;
    case kAskedEvitem:
        set_hex(w, "callback", rdptr(ctl, kEvitemCallback));
        break;
    case kAskedConfyn:
        w.set_number("mode", static_cast<int32_t>(rd32(ctl, kConfynMode)));
        break;
    case kAskedBank:
        set_hex(w, "callback", rdptr(ctl, kBankCallback));
        break;
    case kAskedMaplist:
        set_hex(w, "callback", rdptr(ctl, kMapCallback));
        w.set_number("region", rd16(ctl, kMapRegion));
        w.set_number("zone", rd16(ctl, kMapZone));
        break;
    case kAskedGmtell: {
        const AskedSites& s = g_engine.game.asked;
        w.set_number("slot", readable(s.gm_slot, 4) ? *s.gm_slot : -1);
        break;
    }
    case kAskedMpDead:
        w.set_number("remaining", static_cast<int32_t>(rd32(ctl, kMpDeadRemaining)));
        w.set_number("deadline", rd32(ctl, kMpDeadDeadline));
        break;
    case kAskedOk:
        set_hex(w, "callback", rdptr(ctl, kOkCallback));
        set_hex(w, "context", rdptr(ctl, kOkContext));
        break;
    case kAskedDoor:
        w.set_number("nation", q.blocked ? q.record.row & 0xFF : ctl[kDoorNation]);
        w.set_number("kept", q.blocked ? (q.record.row >> 8) & 0xFF : ctl[kDoorKept]);
        w.set_number("exit", static_cast<int8_t>(ctl[kDoorExit]));
        w.set_number("floor", q.now.floor);
        set_hex(w, "house", q.now.house);
        set_hex(w, "door", q.now.door);
        break;
    default:
        break;
    }
    w.field("detail");
}

// The page dialogue box d shows: {text, segments, undecoded} of its lines
// decoded and joined, waits ('key', 'timer' or 'key_or_timer'), seconds
// left on a timed page, auto while the script confirms each page itself,
// more while a page of the message follows, queued (the messages behind
// it) and cancellable (the event allows a cancel); the text object's words
// under `detail`. False, with nil and the reason written, while the box
// shows no message.
bool write_dialogue(ReplyWriter& w, int d) {
    HU_STEP_AT("write_dialogue", d);
    const uint8_t* ctl = controller_guarded(kDialogueNames[d], kBoxBytes);
    const uint8_t* text = ctl ? rdptr(ctl, kBoxText) : NULL;
    text = text && readable(text, kTextBytes) ? text : NULL;
    const uint8_t* message = dialogue_message(text);
    if (!message || !readable(message, kMessageBytes)) {
        char why[kWhyBytes];
        snprintf(why, sizeof(why), "no dialogue is shown in %s", kDialogueNames[d]);
        w.fail(why);
        return false;
    }
    const uint8_t* glyphs[kPageLines];
    int counts[kPageLines];
    const int lines = page_lines_guarded(ctl, glyphs, counts);
    PageText page;
    decode_page(glyphs, counts, lines, &g_engine.glyphs, &page);
    const uint32_t flags = rd32(text, kTextFlags);
    int64_t timer;
    memcpy(&timer, text + kTextTimer, 8);
    const int32_t held = static_cast<int32_t>(rd32(text, kTextQueued));
    const uint8_t* allowed = g_engine.game.query_cancel_allowed;
    w.table();
    write_glyph_text(w, page, "text", "segments", "undecoded");
    const bool key = (flags & kPageKey) != 0;
    const bool timed = (flags & kPageTimed) != 0;
    if (key || timed) {
        w.set_string("waits", key && timed ? "key_or_timer" : key ? "key" : "timer");
    }
    if (timed) {
        w.set_number("seconds", timer > 0 ? static_cast<double>(timer) / kDeadTicks : 0.0);
    }
    w.set_bool("auto", rd16(ctl, kBoxAuto) != 0);
    w.set_bool("more", more_pages(message));
    w.set_number("queued", held > 1 ? held - 1 : 0);
    w.set_bool("cancellable", readable(allowed, 1) && *allowed == 1);
    w.table();
    w.set_number("flags", flags);
    w.set_number("timer", static_cast<double>(timer));
    w.set_number("page_lines", rd16(ctl, kBoxPageLines));
    w.set_number("lines_read", lines);
    w.set_bool("truncated", page.truncated);
    w.set_number("held", held);
    w.set_number("mode", message[kMessageMode]);
    set_hex(w, "message", message);
    set_hex(w, "text_object", text);
    w.field("detail");
    return true;
}

// One item of an offer: {slot, item, id, count}, item the inventory index
// (none for the partner's), and pending while it is on its way.
void write_offer_item(ReplyWriter& w, const OfferEntry& m, bool own, int k) {
    w.table();
    w.set_number("slot", m.slot);
    if (own) {
        w.set_number("item", m.index);
    }
    w.set_number("id", m.id);
    w.set_number("count", m.count);
    if (m.pending) {
        w.set_bool("pending", true);
    }
    w.index(k);
}

// The inventory indexes the game grays out for offer window `which` now, as
// the drain's gray test last found them on this open, each still holding the
// item the test ran on; nil until the test has run on this open.
void write_gray(ReplyWriter& w, int which) {
    GrayRows rows;
    if (!g_engine.gray_table.read(&rows) || !rows.open || rows.which != which
        || rows.instance != offer_instance(which)) {
        return;
    }
    const uint8_t* data = offer_data_guarded();
    w.table();
    int k = 0;
    for (int i = 1; i < kInventoryRecords; ++i) {
        const HeldItem held = held_item(g_engine.game.offer, data, i);
        if (rows.code[i] == kGrayGrayed && held.present && held.id == rows.id[i]) {
            w.number(i);
            w.index(++k);
        }
    }
    w.field("gray");
}

// An offer window's offer, with its window open. trade: {partner, mine =
// {gil, items}, theirs = {gil, items}, ok_mine, ok_theirs, next_change,
// gray}, mine's items in slot order, this engine's offers on their way after
// them, next_change the seconds until the game sends another change;
// handover: {npc, items, gil, gray, next_ok}, next_ok the seconds until the
// game sends the NPC trade. Each wait 0 when it would go now, nil while the
// clock cannot be read; gray as write_gray(). The partner's and the NPC's
// names as the actor table holds them, nil when it holds none. detail: the
// controller, the target's index and id, and the trade's states.
void write_offer(ReplyWriter& w, const OfferView& v) {
    const OfferSites& s = g_engine.game.offer;
    const bool trade = v.which == kOfferTrade;
    char name[kActorNameBytes + 1];
    w.table();
    if (actor_name(s, v.target, name, sizeof(name))) {
        w.set_string(trade ? "partner" : "npc", name);
    }
    if (trade) {
        w.table();
    }
    w.set_number("gil", v.gil);
    w.table();
    int k = 0;
    for (int slot = 1; slot < kOfferTable; ++slot) {
        if (v.mine[slot].id != 0) {
            write_offer_item(w, v.mine[slot], true, ++k);
        }
    }
    for (int i = 0; i < v.pending_count; ++i) {
        write_offer_item(w, v.pending[i], true, ++k);
    }
    w.field("items");
    if (trade) {
        w.field("mine");
        w.table();
        w.set_number("gil", v.their_gil);
        w.table();
        k = 0;
        for (int slot = 1; slot < kOfferTable; ++slot) {
            if (v.theirs[slot].id != 0) {
                write_offer_item(w, v.theirs[slot], false, ++k);
            }
        }
        w.field("items");
        w.field("theirs");
        w.set_bool("ok_mine", v.state == kTradeOkSent);
        w.set_bool("ok_theirs", v.their_state == kTradeOkSent);
    }
    if (v.wait >= 0) {
        w.set_number(trade ? "next_change" : "next_ok", v.wait / 1000.0);
    }
    write_gray(w, v.which);
    w.table();
    set_hex(w, "controller", controller_guarded(kOfferNames[v.which], offer_bytes(v.which)));
    w.set_number(trade ? "partner_index" : "npc_index", v.target);
    w.set_number(trade ? "partner_id" : "npc_id", v.target_id);
    if (trade) {
        w.set_number("state", v.state);
        w.set_number("their_state", v.their_state);
    }
    w.field("detail");
}

// A text window's question: stringdl's mode ('name' or 'confirm'), the
// name's lengths and the line shown over the field (`prompt`); comment's
// kind of message, its lines as the slot holds them and the search's
// category; msgline's mode, the longest text it takes, the recipient's
// name where the mode has one and whether its block kept it from opening;
// scname's and link11's longest name and the name they hold (`text`),
// link11's colors. The words the replies read under `detail`.
void write_text(ReplyWriter& w, const TextQuestion& q) {
    const uint8_t* ctl = q.now.ctl;
    w.table();
    switch (q.text) {
    case kTextStringdl: {
        const bool name = ctl[kStringdlMode] == kStringdlAsksName;
        w.set_string("mode", name ? "name" : "confirm");
        if (name) {
            w.set_number("min_length", kStringdlMin);
            w.set_number("max_length", kStringdlMax);
        }
        char prompt[kStringdlPromptBytes + 1];
        copy_text(reinterpret_cast<const char*>(ctl + kStringdlPrompt), prompt, sizeof(prompt));
        if (prompt[0]) {
            w.set_string("prompt", prompt);
        }
        w.table();
        w.set_number("reply", rd16(ctl, kStringdlReply));
        w.set_bool("editing", ctl[kStringdlEditing] != 0);
        w.set_number("delivery_state", q.now.owner ? rd16(q.now.owner, kDeliveryState) : 0);
        w.field("detail");
        break;
    }
    case kTextComment: {
        static const char* const modes[4] = {"bazaar", "view", "search", "linkshell"};
        const int mode = static_cast<int8_t>(ctl[kCommentMode]);
        w.set_string("mode", modes[mode]);
        w.table();
        for (int l = 0; l < kCommentLines; ++l) {
            char line[kCommentLineBytes];
            copy_text(reinterpret_cast<const char*>(ctl + kCommentSlots + (mode * 3 + l) * kCommentLineBytes), line,
                sizeof(line));
            w.table();
            w.set_string("text", line);
            w.index(l + 1);
        }
        w.field("lines");
        if (mode == 2) {
            w.set_number("category", ctl[kCommentCategory]);
        }
        w.table();
        w.set_number("editing_line", static_cast<int8_t>(ctl[kCommentEditing]));
        w.field("detail");
        break;
    }
    case kTextMsgline: {
        const int mode = rd16(ctl, kMsglineMode);
        w.set_number("mode", mode);
        w.set_number("max_length", msgline_max(mode));
        char name[kMsglineNameBytes + 1];
        copy_text(reinterpret_cast<const char*>(ctl + kMsglineName), name, sizeof(name));
        if ((mode == 0 || mode == 2 || mode == 7) && name[0]) {
            w.set_string("recipient", name);
        }
        w.set_bool("blocked", q.now.blocked);
        w.table();
        w.set_bool("busy", q.now.busy != 0);
        w.set_bool("editing", rdptr(ctl, kMsglineField) != NULL);
        w.set_number("word1", rd32(ctl, kMsglineWord1));
        w.set_number("word2", rd32(ctl, kMsglineWord2));
        w.field("detail");
        break;
    }
    case kTextScname: {
        char name[kScnameMax + 2];
        copy_text(reinterpret_cast<const char*>(ctl + kScnameName), name, sizeof(name));
        w.set_number("max_length", kScnameMax);
        w.set_string("text", name);
        w.table();
        w.set_bool("editing", rdptr(ctl, kScnameField) != NULL);
        w.field("detail");
        break;
    }
    case kTextLink11: {
        char name[kLink11Max + 1];
        copy_text(reinterpret_cast<const char*>(ctl + kLink11Name), name, sizeof(name));
        w.set_number("max_length", kLink11Max);
        w.set_string("text", name);
        w.table();
        for (int k = 0; k < 3; ++k) {
            w.number(ctl[kLink11Colors + k]);
            w.index(k + 1);
        }
        w.field("color");
        w.table();
        w.set_number("editing", static_cast<int32_t>(rd32(ctl, kLink11Editing)));
        set_hex(w, "selected", q.now.selected);
        w.field("detail");
        break;
    }
    default:
        break;
    }
}

// What the player could pick in prompt n: query's choice and whether it may
// be canceled, link5's slots, arealist's rows (engine abi 3 and 4: its
// zones), passinpu's limit, from engine abi 4 on prtyjoin's invite, and from
// engine abi 8 on a yes/no window's question, the KO window's countdown, a
// number box's question, the Asked windows' and a dialogue box's page, and
// what a text window asks for. False, with nil and the reason written, when
// the prompt has nothing to offer.
bool write_options(ReplyWriter& w, int n, int version) {
    HU_STEP_AT("write_options", n);
    const bool v4 = version >= 4;
    const char* nm = g_engine.inv.names[n].name;
    char why[kWhyBytes];
    if (strcmp(nm, "query") == 0) {
        const uint8_t* ctl = query_parsed();
        if (!ctl) {
            w.fail("query is not open");
            return false;
        }
        write_query(w, ctl, v4);
        w.set_bool("cancellable", query_cancellable());
    } else if (strcmp(nm, "link5") == 0) {
        const uint8_t* ctl = link5_pending(NULL, why, sizeof(why));
        if (!ctl) {
            w.fail(why);
            return false;
        }
        const uint8_t* rows = NULL;
        const uint8_t* names = NULL;
        const int count = link5_rows(ctl, &rows, &names);
        w.table();
        w.table();
        int k = 0;
        for (int i = 1; i < count; ++i) {
            const int slot = link5_slot_of(rdptr(rows + i * kLink5RowStride, kLink5RowRecord));
            if (slot < 0) {
                continue;
            }
            char text[kLink5NameBytes + 1];
            copy_text(names ? reinterpret_cast<const char*>(names + i * kLink5NameStride + kLink5NameAt) : NULL,
                text, sizeof(text));
            w.table();
            w.set_number("slot", slot);
            w.set_string("name", text);
            w.index(++k);
        }
        w.field("slots");
    } else if (strcmp(nm, "arealist") == 0) {
        const uint8_t* ctl = area_pending(NULL, why, sizeof(why));
        const uint8_t* blocked = !ctl && version >= 5 ? area_blocked() : NULL;
        if (!ctl && !blocked) {
            w.fail(why);
            return false;
        }
        write_area(w, ctl ? ctl : blocked, version >= 5, blocked != NULL);
    } else if (strcmp(nm, "passinpu") == 0) {
        const uint8_t* ctl = passinpu_pending(NULL, why, sizeof(why));
        if (!ctl) {
            w.fail(why);
            return false;
        }
        const int max = static_cast<int>(rd32(ctl, kPassMax));
        w.table();
        w.set_number("max_length", max < static_cast<int>(kPassinpuText) ? max : static_cast<int>(kPassinpuText));
    } else if (v4 && strcmp(nm, "prtyjoin") == 0) {
        const uint8_t* flags = g_engine.game.party_pending;
        if (!readable(flags, kPartyKind + 1) || !flags[0]) {
            w.fail("no party invite is pending");
            return false;
        }
        w.table();
        write_invite(w, flags, version >= 5);
    } else if (version >= 8 && yesno_window(nm)) {
        const uint8_t* ctl = yesno_pending_guarded(n, why, sizeof(why));
        if (!ctl) {
            w.fail(why);
            return false;
        }
        write_yesno(w, n, ctl);
    } else if (version >= 8 && strcmp(nm, "dead") == 0) {
        uint8_t* ctl = NULL;
        if (!open_menu_guarded(n, &ctl) || !readable(ctl, kDeadBytes)) {
            w.fail("dead is not open");
            return false;
        }
        write_dead(w, ctl);
    } else if (version >= 8 && box_named(nm) >= 0) {
        BoxQuestion q;
        if (!box_question_guarded(box_named(nm), &q, why, sizeof(why))) {
            w.fail(why);
            return false;
        }
        write_box(w, q);
    } else if (version >= 8 && asked_named(nm) >= 0) {
        AskedQuestion q;
        if (!asked_question_guarded(asked_named(nm), &q, why, sizeof(why))) {
            w.fail(why);
            return false;
        }
        write_asked(w, q);
    } else if (version >= 8 && dialogue_named(nm) >= 0) {
        return write_dialogue(w, dialogue_named(nm));
    } else if (version >= 8 && text_named(nm) >= 0) {
        TextQuestion q;
        if (!text_question_guarded(text_named(nm), false, &q, why, sizeof(why))) {
            w.fail(why);
            return false;
        }
        write_text(w, q);
    } else if (version >= 8 && offer_named(nm) >= 0) {
        OfferView v;
        offer_view_guarded(offer_named(nm), &v);
        if (!v.open) {
            snprintf(why, sizeof(why), "%s is not open", nm);
            w.fail(why);
            return false;
        }
        write_offer(w, v);
    } else {
        snprintf(why, sizeof(why), version >= 8
            ? "%s has no options; options() reads query, link5, arealist, passinpu, prtyjoin, comyn, merityn, the"
              " resyn windows, shopbuy, shopsell, dead, moneyctr, itemctrl, sortyn, evitem01, confyn, bankmenu,"
              " maplist, gmtell, mp_dead, ok, mogdoor, rem4line, rem4li2, stringdl, comment, msgline, scname,"
              " link11, trade and handover"
            : v4 ? "%s has no options; options() reads query, link5, arealist, passinpu and prtyjoin"
                 : "%s has no options; options() reads query, link5, arealist and passinpu", nm);
        w.fail(why);
        return false;
    }
    return true;
}

void __stdcall op_options(const HuEngineHandle* ref, const char* name, HuEngineReply* reply) {
    ReplyWriter w = {reply};
    const int n = read_target(ref, name, true, w);
    if (n >= 0) {
        write_options(w, n, 3);
    }
}

// The prompt's id for version 4 and 5: the count of the game's opens of the
// window, both dialogue boxes' for either (game.h's open_count()), or from
// engine abi 5 on for prtyjoin and the post boxes the session's id; either
// read before the prompt is.
uint32_t prompt_id(int n, int version) {
    const int session = version >= 5 ? session_window(g_engine.inv.names[n].name) : -1;
    return session >= 0 ? session_now(session) : open_count(g_engine, n);
}

// As op_options, with abi 4's or 5's shapes and the prompt's `id`, which
// answer and cancel take back. A read the prompt changed during is made
// again, so the id never names a prompt newer than what was read. Abi 5's
// names the prompt in `name`.
void write_prompt(const HuEngineHandle* ref, const char* name, HuEngineReply* reply, int version) {
    HU_STEP_AT("write_prompt", version);
    ReplyWriter w = {reply};
    const int n = read_target(ref, name, true, w);
    if (n < 0) {
        return;
    }
    const uint32_t start = reply->used;
    for (int attempt = 0; attempt < 4; ++attempt) {
        reply->used = start;
        const uint32_t id = prompt_id(n, version);
        MemoryBarrier();
        const bool wrote = write_options(w, n, version);
        MemoryBarrier();
        if (wrote) {
            if (version >= 5) {
                w.set_string("name", g_engine.inv.names[n].name);
            }
            w.set_number("id", id);
        }
        if (prompt_id(n, version) == id) {
            break;
        }
    }
}

void __stdcall op_options4(const HuEngineHandle* ref, const char* name, HuEngineReply* reply) {
    write_prompt(ref, name, reply, 4);
}

void __stdcall op_options5(const HuEngineHandle* ref, const char* name, HuEngineReply* reply) {
    write_prompt(ref, name, reply, 5);
}

void __stdcall op_options8(const HuEngineHandle* ref, const char* name, HuEngineReply* reply) {
    write_prompt(ref, name, reply, 8);
}

// What waits on a reply with no window open: a party invite, a post-box
// session, and from abi 8 on a number box's, an Asked window's or msgline's
// question its block kept from opening, under the window's name. Empty when
// none does. From abi 4 on the session's state word is under `detail`; abi
// 5's name each the prompt a reply to it takes, prtyjoin or the box, and
// give the session's id.
void write_pending(const HuEngineHandle* ref, HuEngineReply* reply, int version) {
    HU_STEP_AT("write_pending", version);
    const bool v4 = version >= 4;
    ReplyWriter w = {reply};
    if (!readable_for(ref, w)) {
        return;
    }
    w.table();
    const uint8_t* flags = g_engine.game.party_pending;
    if (readable(flags, kPartyKind + 1) && flags[0]) {
        const uint32_t id = session_now(kSessionInvite);
        w.table();
        write_invite(w, flags, version >= 5);
        if (version >= 5) {
            w.set_string("name", "prtyjoin");
            w.set_number("id", id);
        }
        w.field("invite");
    }
    const uint8_t* delivery = controller_guarded("delivery", kDeliveryState + 2);
    const uint8_t* post1 = controller_guarded("post1", kPostState + 4);
    const int out_state = delivery ? static_cast<uint16_t>(rd16(delivery, kDeliveryState)) : 0;
    const uint32_t in_state = post1 ? rd32(post1, kPostState) : 0;
    if (out_state || in_state) {
        const double state = out_state ? static_cast<double>(out_state) : static_cast<double>(in_state);
        const char* box = out_state ? "delivery" : "post1";
        w.table();
        w.set_string("box", box);
        if (version >= 5) {
            w.set_string("name", box);
            w.set_number("id", session_now(kSessionPost));
        }
        if (v4) {
            w.table();
            w.set_number("state", state);
            w.field("detail");
        } else {
            w.set_number("state", state);
        }
        w.field("post");
    }
    for (int b = 0; version >= 8 && b < kBoxCount; ++b) {
        BoxQuestion q;
        char why[kWhyBytes];
        if (!box_question_guarded(b, &q, why, sizeof(why)) || !q.blocked) {
            continue;
        }
        write_box(w, q);
        w.set_string("name", kBoxNames[b]);
        w.set_number("id", q.id);
        w.field(kBoxNames[b]);
    }
    for (int a = 0; version >= 8 && a < kAskedCount; ++a) {
        AskedQuestion q;
        char why[kWhyBytes];
        if ((!asked_records(a) && a != kAskedBank) || !asked_question_guarded(a, &q, why, sizeof(why)) || !q.blocked) {
            continue;
        }
        write_asked(w, q);
        w.set_string("name", kAskedNames[a]);
        w.set_number("id", q.id);
        w.field(kAskedNames[a]);
    }
    TextQuestion message;
    char why[kWhyBytes];
    if (version >= 8 && text_question_guarded(kTextMsgline, false, &message, why, sizeof(why))
        && message.now.blocked) {
        write_text(w, message);
        w.set_string("name", kTextNames[kTextMsgline]);
        w.set_number("id", message.id);
        w.field(kTextNames[kTextMsgline]);
    }
}

void __stdcall op_pending(const HuEngineHandle* ref, HuEngineReply* reply) {
    write_pending(ref, reply, 3);
}

void __stdcall op_pending4(const HuEngineHandle* ref, HuEngineReply* reply) {
    write_pending(ref, reply, 4);
}

void __stdcall op_pending5(const HuEngineHandle* ref, HuEngineReply* reply) {
    write_pending(ref, reply, 5);
}

void __stdcall op_pending8(const HuEngineHandle* ref, HuEngineReply* reply) {
    write_pending(ref, reply, 8);
}

// Every open window's frame, {x, y, w, h}, keyed by name, and the compass's
// box; the target arrows have none.
void __stdcall op_rects5(const HuEngineHandle* ref, HuEngineReply* reply) {
    ReplyWriter w = {reply};
    if (!readable_for(ref, w)) {
        return;
    }
    w.table();
    for (int n = 0; n < g_engine.inv.count; ++n) {
        int16_t r[4];
        if (is_targetarrow(g_engine, n)) {
            continue;
        }
        if (is_compass(g_engine, n)) {
            const uint8_t* c = compass_guarded();
            if (!compass_open(c)) {
                continue;
            }
            compass_frame(c, r);
        } else {
            const uint8_t* menu = open_menu_guarded(n, NULL);
            if (!menu) {
                continue;
            }
            for (int i = 0; i < 4; ++i) {
                r[i] = rd16(menu, kMenuRect + 2 * i);
            }
        }
        w.table();
        w.set_number("x", r[0]);
        w.set_number("y", r[1]);
        w.set_number("w", r[2] - r[0]);
        w.set_number("h", r[3] - r[1]);
        w.field(g_engine.inv.names[n].name);
    }
}

enum Listed { kListHidden, kListBlocked, kListMoved, kListResized };

void write_names(ReplyWriter& w, const char* key, Listed what) {
    w.table();
    int k = 0;
    for (int n = 0; n < g_engine.inv.count; ++n) {
        MemEntry m;
        bool listed = false;
        if (what == kListHidden || what == kListBlocked) {
            listed = (g_engine.holds.want[n] & (what == kListHidden ? kWantHidden : kWantBlocked)) != 0;
        } else if (g_engine.memory.read(n, &m)) {
            listed = what == kListMoved ? m.active != 0 : m.size_kind != 0;
        }
        if (listed) {
            w.string(g_engine.inv.names[n].name);
            w.index(++k);
        }
    }
    w.field(key);
}

struct StatusData {
    int state;
    char failure[256];
    bool transient;
    bool pinned;
    bool busy;
    int handles;
    uint32_t queued;
    uint32_t dropped;
    const HuDaemonApi* daemon;
    bool resolved;
    char drain_error[sizeof(g_engine.error)];
};

void collect_status(StatusData* s) {
    HU_STEP("collect_status");
    lock();
    s->state = g_rt.state;
    memcpy(s->failure, g_rt.failure, sizeof(s->failure));
    s->transient = g_rt.transient;
    s->pinned = g_rt.self_reference != NULL;
    s->busy = g_rt.pinned_busy;
    s->handles = g_rt.handles.open;
    s->queued = g_engine.commands.pushed() - g_engine.commands.taken();
    s->dropped = g_rt.dropped;
    s->daemon = g_rt.daemon;
    s->resolved = g_rt.resolved;
    unlock();

    s->drain_error[0] = '\0';
    for (int attempt = 0; attempt < 16; ++attempt) {
        const LONG before = g_engine.error_seq;
        if (before & 1) {
            continue;
        }
        memcpy(s->drain_error, g_engine.error, sizeof(s->drain_error));
        MemoryBarrier();
        if (g_engine.error_seq == before) {
            break;
        }
        s->drain_error[0] = '\0';
    }
    s->drain_error[sizeof(s->drain_error) - 1] = '\0';
}

// The game's UI size, as the manager holds it.
void write_ui(ReplyWriter& w) {
    w.table();
    w.set_number("w", static_cast<uint16_t>(rd16(g_engine.game.mcb, kMcbUiW)));
    w.set_number("h", static_cast<uint16_t>(rd16(g_engine.game.mcb, kMcbUiH)));
    w.field("ui");
}

// The engine's own words: what failed, the daemon, the queues, and once the
// game is resolved the addresses of everything it found.
void write_internals(ReplyWriter& w, const StatusData& s) {
    if (s.state == kFailed || (s.state == kIdle && s.transient)) {
        w.set_string("error", s.failure);
    }
    if (s.daemon) {
        w.table();
        w.set_number("abi", s.daemon->abi_version);
        w.set_string("build", s.daemon->build_id ? s.daemon->build_id() : "?");
        w.field("daemon");
    }
    w.set_bool("pinned", s.pinned);
    w.set_bool("busy", s.busy);
    w.set_number("queued", s.queued);
    w.set_number("events", g_engine.events.position());
    w.set_number("unmatched_open_keys", g_engine.unmatched_keys);
    w.set_number("open_stack_overflow", g_engine.opens.overflow);
    w.set_number("drain_errors", g_engine.drain_errors);
    if (s.drain_error[0]) {
        w.set_string("drain_error", s.drain_error);
    }
    if (!s.resolved) {
        return;
    }
    set_hex(w, "registry", g_engine.game.registry);
    w.set_number("rows", kRowCount);
    w.set_number("names", g_engine.inv.count);
    set_hex(w, "manager", g_engine.game.mcb);
    write_ui(w);
    w.table();
    for (int i = 0; i < kSiteCount; ++i) {
        set_hex(w, kSites[i].name, g_rt.targets[i]);
    }
    set_hex(w, "quad_draw", g_rt.quad);
    const void* set_position;
    memcpy(&set_position, &g_engine.game.set_position, sizeof(set_position));
    set_hex(w, "set_position", set_position);
    const void* close;
    memcpy(&close, &g_engine.game.close, sizeof(close));
    set_hex(w, "close_by_name", close);
    const void* set_cursor;
    memcpy(&set_cursor, &g_engine.game.set_cursor, sizeof(set_cursor));
    set_hex(w, "set_cursor", set_cursor);
    const void* glyph_convert;
    memcpy(&glyph_convert, &g_engine.game.glyph_convert, sizeof(glyph_convert));
    set_hex(w, "glyph_convert", glyph_convert);
    set_hex(w, "row_hit_test", g_engine.game.row_hit_test);
    set_hex(w, "mouse_read", g_engine.game.mouse_read);
    set_hex(w, "menu_routing", g_engine.game.menu_routing);
    for (int i = 0; i < kCallCount; ++i) {
        set_hex(w, kCalls[i].name, g_rt.calls[i]);
    }
    set_hex(w, "prtyjoin_pending", g_engine.game.party_pending);
    set_hex(w, "link5_cache", g_engine.game.link5_cache);
    set_hex(w, "link5_latch", g_engine.game.link5_latch);
    set_hex(w, "link5_callback", g_engine.game.link5_callback);
    set_hex(w, "query_cancel_allowed", g_engine.game.query_cancel_allowed);
    set_hex(w, "compass_global", g_engine.game.compass);
    set_hex(w, "target_arrow_return", reinterpret_cast<const void*>(g_engine.game.arrow_return[0]));
    set_hex(w, "pick_arrow_return", reinterpret_cast<const void*>(g_engine.game.arrow_return[1]));
    set_hex(w, "macro_object", g_engine.game.macro_object);
    set_hex(w, "yesno_vtable", g_engine.game.yesno_vtable);
    set_hex(w, "yesno_confirm", g_engine.game.yesno_confirm);
    const void* yesno_ask;
    memcpy(&yesno_ask, &g_engine.game.yesno_ask, sizeof(yesno_ask));
    set_hex(w, "yesno_ask", yesno_ask);
    set_hex(w, "dead_tick", g_engine.game.dead_tick);
    const DeadConfirm& dead = g_engine.game.dead;
    const void* dead_decider;
    memcpy(&dead_decider, &dead.decider, sizeof(dead_decider));
    set_hex(w, "dead_decider", dead_decider);
    const void* message;
    memcpy(&message, &dead.message, sizeof(message));
    set_hex(w, "message", message);
    set_hex(w, "dead_callback", dead.callback);
    set_hex(w, "yesno_cancel", g_engine.game.yesno_cancel);
    for (int b = 0; b < kBoxCount; ++b) {
        char key[32];
        const void* confirm;
        memcpy(&confirm, &g_engine.game.box_confirm[b], sizeof(confirm));
        snprintf(key, sizeof(key), "%s_confirm", kBoxNames[b]);
        set_hex(w, key, confirm);
        snprintf(key, sizeof(key), "%s_setup", kBoxNames[b]);
        set_hex(w, key, g_engine.game.box_setup[b]);
    }
    w.table();
    for (int i = 0; i < kAskerCount; ++i) {
        set_hex(w, kAskerSites[i].name, g_engine.game.askers[i]);
    }
    w.field("askers");
    const void* close_instance;
    memcpy(&close_instance, &g_engine.game.close_instance, sizeof(close_instance));
    set_hex(w, "close_instance", close_instance);
    const AskedSites& as = g_engine.game.asked;
    const void* evitem_callback;
    memcpy(&evitem_callback, &as.evitem_callback, sizeof(evitem_callback));
    const void* confyn_restore;
    memcpy(&confyn_restore, &as.confyn_restore, sizeof(confyn_restore));
    const void* bank_clear;
    memcpy(&bank_clear, &as.bank_clear, sizeof(bank_clear));
    const void* maplist[4];
    memcpy(&maplist[0], &as.maplist_close, sizeof(maplist[0]));
    memcpy(&maplist[1], &as.maplist_rebuild, sizeof(maplist[1]));
    memcpy(&maplist[2], &as.maplist_select, sizeof(maplist[2]));
    memcpy(&maplist[3], &as.maplist_cursor, sizeof(maplist[3]));
    const void* map_close;
    memcpy(&map_close, &as.map_close, sizeof(map_close));
    w.table();
    set_hex(w, "sortyn_setup", as.sortyn_setup);
    set_hex(w, "sortyn_confirm", as.sortyn_confirm);
    set_hex(w, "evitem_ask", as.evitem_ask);
    set_hex(w, "evitem_setup", as.evitem_setup);
    set_hex(w, "evitem_callback", evitem_callback);
    set_hex(w, "evitem_input", as.evitem_input);
    set_hex(w, "confyn_input", as.confyn_input);
    set_hex(w, "confyn_restore", confyn_restore);
    set_hex(w, "bank_open", as.bank_open);
    set_hex(w, "bank_clear", bank_clear);
    set_hex(w, "bank_data", as.bank_data);
    w.set_number("bank_safe2", as.bank_safe2);
    set_hex(w, "bank_input", as.bank_input);
    set_hex(w, "maplist_close", maplist[0]);
    set_hex(w, "maplist_rebuild", maplist[1]);
    set_hex(w, "maplist_select", maplist[2]);
    set_hex(w, "maplist_cursor", maplist[3]);
    set_hex(w, "map0_input", as.map0_input);
    set_hex(w, "map_close", map_close);
    set_hex(w, "gmtell_poll", as.gm_poll);
    set_hex(w, "gmtell_slot", as.gm_slot);
    set_hex(w, "gmtell_queue", as.gm_queue);
    set_hex(w, "mp_dead_input", as.mp_dead_input);
    set_hex(w, "ok_input", as.ok_input);
    const DoorSites& door = as.door;
    const void* door_fns[6];
    memcpy(&door_fns[0], &door.gate, 4);
    memcpy(&door_fns[1], &door.floor, 4);
    memcpy(&door_fns[2], &door.condition, 4);
    memcpy(&door_fns[3], &door.tick, 4);
    memcpy(&door_fns[4], &door.open_door, 4);
    door_fns[5] = door.opened;
    set_hex(w, "door_input", door.input);
    set_hex(w, "door_vtable", door.vtable);
    set_hex(w, "door_open_hook", door.open_hook);
    set_hex(w, "door_draw", door.draw);
    set_hex(w, "door_handler", door.handler);
    set_hex(w, "door_gate", door_fns[0]);
    set_hex(w, "door_floor", door_fns[1]);
    w.set_number("door_floor_word", door.floor_word);
    set_hex(w, "door_condition", door_fns[2]);
    set_hex(w, "door_tick", door_fns[3]);
    set_hex(w, "door_open", door_fns[4]);
    set_hex(w, "door_opened", door_fns[5]);
    set_hex(w, "door_house", door.house);
    for (int i = 0; i < 3; ++i) {
        char key[32];
        snprintf(key, sizeof(key), "door_exiting_%d", i + 1);
        set_hex(w, key, door.exiting[i]);
    }
    set_hex(w, "door_table", door.table);
    w.set_number("door_nations", door.nations);
    w.set_number("door_garden", door.garden);
    w.field("asked");
    const DialogueSites& dialogue = g_engine.game.dialogue;
    const void* dialogue_fns[3];
    memcpy(&dialogue_fns[0], &dialogue.dispatch, 4);
    memcpy(&dialogue_fns[1], &dialogue.release, 4);
    memcpy(&dialogue_fns[2], &dialogue.clear, 4);
    w.table();
    set_hex(w, "input", dialogue.input);
    set_hex(w, "handler", dialogue.handler);
    set_hex(w, "dispatch", dialogue_fns[0]);
    set_hex(w, "text_first", dialogue.texts[0]);
    set_hex(w, "text_second", dialogue.texts[1]);
    set_hex(w, "release", dialogue_fns[1]);
    set_hex(w, "clear", dialogue_fns[2]);
    set_hex(w, "wait", dialogue.wait);
    set_hex(w, "tick", dialogue.tick);
    set_hex(w, "click_return", reinterpret_cast<const void*>(dialogue.click_return));
    w.field("dialogue");
    const TextSites& text = g_engine.game.text;
    const struct {
        const char* name;
        const void* const* at;
    } text_fns[] = {
        {"stringdl_stop", reinterpret_cast<const void* const*>(&text.stringdl_stop)},
        {"focus", reinterpret_cast<const void* const*>(&text.focus)},
        {"comment_stop", reinterpret_cast<const void* const*>(&text.comment_stop)},
        {"comment_start", reinterpret_cast<const void* const*>(&text.comment_start)},
        {"sanitize", reinterpret_cast<const void* const*>(&text.sanitize)},
        {"trim", reinterpret_cast<const void* const*>(&text.trim)},
        {"msgline_stop", reinterpret_cast<const void* const*>(&text.msgline_stop)},
        {"msgline_start", reinterpret_cast<const void* const*>(&text.msgline_start)},
        {"pol_number", reinterpret_cast<const void* const*>(&text.pol_number)},
        {"clock_text", reinterpret_cast<const void* const*>(&text.clock_text)},
        {"send_tell", reinterpret_cast<const void* const*>(&text.send_tell)},
        {"send_reply", reinterpret_cast<const void* const*>(&text.send_reply)},
        {"send_short", reinterpret_cast<const void* const*>(&text.send_short)},
        {"send_answer", reinterpret_cast<const void* const*>(&text.send_answer)},
        {"send_dated", reinterpret_cast<const void* const*>(&text.send_dated)},
        {"mark", reinterpret_cast<const void* const*>(&text.mark)},
        {"scname_stop", reinterpret_cast<const void* const*>(&text.scname_stop)},
        {"criteria_clear", reinterpret_cast<const void* const*>(&text.criteria_clear)},
        {"criteria_name", reinterpret_cast<const void* const*>(&text.criteria_name)},
        {"search_start", reinterpret_cast<const void* const*>(&text.search_start)},
        {"link11_stop", reinterpret_cast<const void* const*>(&text.link11_stop)},
        {"slot_taken", reinterpret_cast<const void* const*>(&text.slot_taken)},
        {"selected", reinterpret_cast<const void* const*>(&text.selected)},
        {"encode", reinterpret_cast<const void* const*>(&text.encode)},
        {"create", reinterpret_cast<const void* const*>(&text.create)},
        {"item", reinterpret_cast<const void* const*>(&text.item)},
        {"passinpu_stop", reinterpret_cast<const void* const*>(&text.passinpu_stop)},
        {"passinpu_start", reinterpret_cast<const void* const*>(&text.passinpu_start)},
    };
    w.table();
    set_hex(w, "stringdl_input", text.stringdl_input);
    set_hex(w, "stringdl_name", text.stringdl_name);
    set_hex(w, "stringdl_slot", text.stringdl_slot);
    set_hex(w, "comment_input", text.comment_input);
    set_hex(w, "comment_vtable", text.comment_vtable);
    set_hex(w, "comment_close", text.comment_close);
    set_hex(w, "msgline_input", text.msgline_input);
    set_hex(w, "scname_input", text.scname_input);
    set_hex(w, "link11_input", text.link11_input);
    set_hex(w, "passinpu_input", text.passinpu_input);
    for (size_t i = 0; i < sizeof(text_fns) / sizeof(text_fns[0]); ++i) {
        set_hex(w, text_fns[i].name, *text_fns[i].at);
    }
    set_hex(w, "player", text.player);
    set_hex(w, "pol", text.pol);
    set_hex(w, "sent_1", text.sent[0]);
    set_hex(w, "sent_2", text.sent[1]);
    set_hex(w, "sending", text.sending);
    set_hex(w, "busy", text.busy);
    set_hex(w, "search", text.search);
    for (int i = 0; i < 4; ++i) {
        char key[16];
        snprintf(key, sizeof(key), "made_%d", i + 1);
        set_hex(w, key, text.made[i]);
    }
    w.field("text");
    const OfferSites& offer = g_engine.game.offer;
    const struct {
        const char* name;
        const void* const* at;
    } offer_fns[] = {
        {"trade_stage", reinterpret_cast<const void* const*>(&offer.stage[kOfferTrade])},
        {"trade_ok", reinterpret_cast<const void* const*>(&offer.ok[kOfferTrade])},
        {"trade_cancel", reinterpret_cast<const void* const*>(&offer.trade_cancel)},
        {"trade_close", reinterpret_cast<const void* const*>(&offer.trade_close)},
        {"handover_stage", reinterpret_cast<const void* const*>(&offer.stage[kOfferHandover])},
        {"handover_ok", reinterpret_cast<const void* const*>(&offer.ok[kOfferHandover])},
        {"handover_close", reinterpret_cast<const void* const*>(&offer.handover_close)},
        {"clock", reinterpret_cast<const void* const*>(&offer.clock)},
        {"item", reinterpret_cast<const void* const*>(&offer.item)},
        {"item_data", reinterpret_cast<const void* const*>(&offer.item_data)},
        {"describe", reinterpret_cast<const void* const*>(&offer.describe)},
    };
    w.table();
    set_hex(w, "trade_input", offer.trade_input);
    set_hex(w, "trade_open", offer.trade_open);
    set_hex(w, "handover_input", offer.handover_input);
    set_hex(w, "rows", offer.rows);
    set_hex(w, "send", offer.send);
    set_hex(w, "allocator", offer.allocator);
    for (size_t i = 0; i < sizeof(offer_fns) / sizeof(offer_fns[0]); ++i) {
        set_hex(w, offer_fns[i].name, *offer_fns[i].at);
    }
    set_hex(w, "trade_rows", offer.trade_rows);
    set_hex(w, "trade_mode", offer.mode[kOfferTrade]);
    set_hex(w, "handover_mode", offer.mode[kOfferHandover]);
    set_hex(w, "player", offer.player);
    w.set_number("own", offer.own);
    w.set_number("theirs", offer.theirs);
    w.set_number("inventory", offer.inventory);
    w.set_number("last_send", offer.last_send);
    set_hex(w, "intervals", offer.intervals);
    w.set_number("interval", offer.interval);
    w.set_number("packets", offer.packets);
    set_hex(w, "clock_object", offer.clock_object);
    set_hex(w, "cache", offer.cache);
    w.set_number("lock", offer.lock);
    set_hex(w, "status_mode", offer.status_mode);
    set_hex(w, "actors", offer.actors);
    w.set_number("actor_titled", offer.actor_titled);
    w.set_number("actor_title", offer.actor_title);
    w.set_number("actor_name", offer.actor_name);
    w.field("offer");
    const OpenerSites& openers = g_engine.game.openers;
    const void* opener_fns[14];
    memcpy(&opener_fns[0], &openers.equipment, 4);
    memcpy(&opener_fns[1], &openers.monstrosity, 4);
    memcpy(&opener_fns[2], &openers.magic, 4);
    memcpy(&opener_fns[3], &openers.map, 4);
    memcpy(&opener_fns[4], &openers.set_spells, 4);
    memcpy(&opener_fns[5], &openers.attachments, 4);
    memcpy(&opener_fns[6], &openers.linkshell_check, 4);
    memcpy(&opener_fns[7], &openers.linkshell, 4);
    memcpy(&opener_fns[8], &openers.instincts, 4);
    memcpy(&opener_fns[9], &openers.item_data, 4);
    memcpy(&opener_fns[10], &openers.use_check, 4);
    memcpy(&opener_fns[11], &openers.synthesis, 4);
    memcpy(&opener_fns[12], &openers.tabula, 4);
    memcpy(&opener_fns[13], &openers.macros, 4);
    w.table();
    set_hex(w, "equipment", opener_fns[0]);
    set_hex(w, "equipment_shortcut", openers.shortcut);
    set_hex(w, "monstrosity", opener_fns[1]);
    set_hex(w, "magic", opener_fns[2]);
    set_hex(w, "map", opener_fns[3]);
    set_hex(w, "set_spells", opener_fns[4]);
    set_hex(w, "attachments", opener_fns[5]);
    set_hex(w, "linkshell_row", openers.linkshell_row);
    set_hex(w, "linkshell_check", opener_fns[6]);
    set_hex(w, "linkshell", opener_fns[7]);
    set_hex(w, "instincts", opener_fns[8]);
    set_hex(w, "item_use", openers.item_use);
    set_hex(w, "item_cache", openers.item_cache);
    set_hex(w, "item_data", opener_fns[9]);
    w.set_number("item_type", openers.item_type);
    set_hex(w, "use_check", opener_fns[10]);
    set_hex(w, "synthesis", opener_fns[11]);
    w.set_number("inventor_list", openers.inventor_list);
    set_hex(w, "tabula", opener_fns[12]);
    set_hex(w, "macro_edit", openers.macro_edit);
    set_hex(w, "macro_editor", openers.macro_editor);
    set_hex(w, "macros", opener_fns[13]);
    w.field("openers");
    const RouteSites& routes = g_engine.game.routes;
    const struct {
        const char* name;
        const void* const* at;
    } route_fns[] = {
        {"storage", reinterpret_cast<const void* const*>(&routes.storage)},
        {"mogpost", reinterpret_cast<const void* const*>(&routes.mogpost)},
        {"language_key", reinterpret_cast<const void* const*>(&routes.language_key)},
        {"friend", reinterpret_cast<const void* const*>(&routes.friend_list)},
        {"msglist", reinterpret_cast<const void* const*>(&routes.msglist)},
        {"blklist", reinterpret_cast<const void* const*>(&routes.blklist)},
        {"mount", reinterpret_cast<const void* const*>(&routes.mount)},
        {"job_points_test", reinterpret_cast<const void* const*>(&routes.job_points_test)},
        {"job_points", reinterpret_cast<const void* const*>(&routes.job_points)},
        {"fp_cat2", reinterpret_cast<const void* const*>(&routes.fp_cat2)},
        {"search_reset", reinterpret_cast<const void* const*>(&routes.search_reset)},
        {"comgenre", reinterpret_cast<const void* const*>(&routes.comgenre)},
        {"sccompar", reinterpret_cast<const void* const*>(&routes.comment_sub[0])},
        {"sccomite", reinterpret_cast<const void* const*>(&routes.comment_sub[1])},
        {"sccomls", reinterpret_cast<const void* const*>(&routes.comment_sub[2])},
        {"item_panel", reinterpret_cast<const void* const*>(&routes.item_panel)},
        {"casttime", reinterpret_cast<const void* const*>(&routes.timer[0])},
        {"eventtim", reinterpret_cast<const void* const*>(&routes.timer[1])},
        {"mp_stat", reinterpret_cast<const void* const*>(&routes.mp_stat)},
        {"quest00_dim_1", reinterpret_cast<const void* const*>(&routes.quest00_dim[0])},
        {"quest00_dim_2", reinterpret_cast<const void* const*>(&routes.quest00_dim[1])},
        {"quest01", reinterpret_cast<const void* const*>(&routes.quest01)},
        {"miss00_menu", reinterpret_cast<const void* const*>(&routes.miss00_menu)},
        {"expansion", reinterpret_cast<const void* const*>(&routes.expansion)},
        {"tracking", reinterpret_cast<const void* const*>(&routes.tracking)},
        {"scanlist", reinterpret_cast<const void* const*>(&routes.scanlist)},
        {"region", reinterpret_cast<const void* const*>(&routes.region)},
        {"macro_pick", reinterpret_cast<const void* const*>(&routes.macro_pick)},
        {"macro_close", reinterpret_cast<const void* const*>(&routes.macro_close)},
        {"title", reinterpret_cast<const void* const*>(&routes.title)},
        {"items_view", reinterpret_cast<const void* const*>(&routes.items_view)},
        {"inventor_open", reinterpret_cast<const void* const*>(&routes.inventor_open)},
    };
    w.table();
    for (size_t i = 0; i < sizeof(route_fns) / sizeof(route_fns[0]); ++i) {
        set_hex(w, route_fns[i].name, *route_fns[i].at);
    }
    set_hex(w, "mog_house", routes.mog_house);
    set_hex(w, "nomad", routes.nomad);
    set_hex(w, "mission_bits", routes.mission_bits);
    set_hex(w, "player", routes.player);
    set_hex(w, "items_object", routes.items_object);
    set_hex(w, "inventor_policy", routes.inventor_policy);
    w.set_number("job_row", routes.job_row);
    w.set_number("search_mode", routes.search_mode);
    w.set_number("item_extra", routes.item_extra);
    w.set_number("persona_layout", routes.persona_layout);
    w.set_number("conf11l_set", routes.conf11l_set);
    w.set_number("quest00_mode", routes.quest00_mode);
    w.set_number("miss00_log", routes.miss00_log);
    w.set_number("area", routes.area);
    w.set_number("region_kind", routes.region_kind);
    w.set_number("coloresu_region", routes.coloresu_region);
    w.set_number("textcol1", routes.textcol[0]);
    w.set_number("textcol2", routes.textcol[1]);
    w.set_number("textcol3", routes.textcol[2]);
    w.set_number("macro_palette", routes.macro_palette);
    w.set_number("inventor_items", routes.inventor_items);
    w.set_number("summary_type", routes.summary_type);
    w.field("routes");
    const SettingSites& settings = g_engine.game.settings;
    const void* setting_fns[5];
    memcpy(&setting_fns[0], &settings.set, 4);
    memcpy(&setting_fns[1], &settings.lock, 4);
    memcpy(&setting_fns[2], &settings.idle, 4);
    memcpy(&setting_fns[3], &settings.flag, 4);
    memcpy(&setting_fns[4], &settings.lock_clock, 4);
    w.table();
    set_hex(w, "store", settings.store);
    set_hex(w, "set", setting_fns[0]);
    set_hex(w, "method", settings.method);
    set_hex(w, "save", settings.save);
    set_hex(w, "lock", setting_fns[1]);
    set_hex(w, "lock_clock", setting_fns[4]);
    set_hex(w, "lock_stamp", settings.lock_stamp);
    w.set_number("lock_wait", settings.lock_wait);
    set_hex(w, "idle", setting_fns[2]);
    set_hex(w, "flag", setting_fns[3]);
    set_hex(w, "player", settings.player);
    w.set_number("flags", settings.flags);
    w.set_number("anonymous", settings.anonymous);
    set_hex(w, "filter_rows", settings.filter_rows);
    set_hex(w, "effect_rows", settings.effect_rows);
    for (int k = 0; k < 4; ++k) {
        char key[24];
        snprintf(key, sizeof(key), "routing_%d", k + 1);
        set_hex(w, key, settings.routing[k]);
        snprintf(key, sizeof(key), "colors_%d", k + 1);
        set_hex(w, key, settings.colors[k]);
    }
    set_hex(w, "keys_1", settings.keys[0]);
    set_hex(w, "keys_2", settings.keys[1]);
    w.set_bool("loaded", store_loaded(settings));
    w.set_bool("labels", g_engine.setting_labels.ready != 0);
    w.field("settings");
    w.field("functions");
    const GlyphTable& glyphs = g_engine.glyphs;
    if (glyphs.ready) {
        MemoryBarrier();
        w.table();
        w.set_number("size", glyphs.size);
        w.set_number("cp932_defined", glyphs.cp932_defined);
        w.set_number("cp932_undefined", glyphs.size - glyphs.cp932_defined);
        w.set_number("gaiji", glyphs.gaiji_named);
        w.set_number("build_ms", glyphs.build_ms);
        w.field("glyph_table");
    }
}

void write_held(ReplyWriter& w) {
    write_names(w, "hidden", kListHidden);
    write_names(w, "blocked", kListBlocked);
    write_names(w, "moved", kListMoved);
    write_names(w, "resized", kListResized);
}

const char* state_name(int state) {
    switch (state) {
    case kInstalled: return "installed";
    case kClosing: return "closing";
    case kFailed: return "failed";
    default: return "idle";
    }
}

void __stdcall op_status(HuEngineReply* reply) {
    ReplyWriter w = {reply};
    StatusData s;
    collect_status(&s);
    w.table();
    w.set_bool("ok", s.state == kInstalled);
    w.set_string("state", state_name(s.state));
    w.set_string("engine", HU_ENGINE_BUILD);
    w.set_number("handles", s.handles);
    write_internals(w, s);
    if (s.resolved) {
        write_held(w);
    }
}

// Engine abi 3's: what an addon acts on at the top, the engine's own words
// under `detail`; the copy asked adds its role, and its image and the
// resident's to `detail`. Abi 4's also has the game's UI size at the top
// once the game is resolved; from 0.9.0 both say whether the macro keys are
// blocked.
void write_status(HuEngineReply* reply, bool v4) {
    ReplyWriter w = {reply};
    StatusData s;
    collect_status(&s);
    w.table();
    w.set_bool("ok", s.state == kInstalled);
    w.set_string("state", state_name(s.state));
    w.set_string("engine", HU_ENGINE_BUILD);
    w.set_number("handles", s.handles);
    w.set_number("dropped", s.dropped);
    write_held(w);
    w.set_bool("macros_blocked", g_engine.holds.macros_want != 0);
    if (v4 && s.resolved) {
        write_ui(w);
    }
    w.table();
    write_internals(w, s);
    w.field("detail");
}

void __stdcall op_status3(HuEngineReply* reply) {
    write_status(reply, false);
}

void __stdcall op_status4(HuEngineReply* reply) {
    write_status(reply, true);
}

void __stdcall op_version(char* out, uint32_t size) {
    lock();
    const HuDaemonApi* daemon = g_rt.daemon;
    unlock();
    if (daemon) {
        snprintf(out, size, "hideui engine %s, daemon abi %u build %s", HU_ENGINE_BUILD,
            static_cast<unsigned>(daemon->abi_version), daemon->build_id ? daemon->build_id() : "?");
    } else {
        snprintf(out, size, "hideui engine %s, no daemon acquired", HU_ENGINE_BUILD);
    }
}

int32_t refusal_out(char* why, uint32_t why_size, const char* text, char* kind, uint32_t kind_size,
                    const char* which) {
    snprintf(why, why_size, "%s", text);
    snprintf(kind, kind_size, "%s", which);
    return 0;
}

// The handlers come out, and the claim and the record go with them: from
// then on another copy may install. A hooked call still running keeps all
// three, and the pin, until a later shutdown.
void unhook() {
    if (clear_handlers()) {
        release_claim();
        retire_record();
        release_self_reference();
    }
}

// Whether the shutdown that started the thread of `generation` still stands.
// That thread polls it without the lock.
bool still_closing(LONG generation) {
    return *static_cast<volatile int*>(&g_rt.state) == kClosing && g_rt.closing == generation;
}

// The thread a shutdown starts, so that the Lua thread never waits for the
// game thread: once a frame has run the restore, it unhooks as unhook() does,
// but takes the image's pin for itself and gives it back as its last act. A
// new() before that cancels it. The reference the shutdown took for it keeps
// the image mapped until it has left: a canceled thread can still be on its
// way out when a later one gives the pin back.
DWORD WINAPI finish_shutdown(LPVOID param) {
    const LONG generation = static_cast<LONG>(reinterpret_cast<intptr_t>(param));
    const HMODULE self = self_module(NULL, 0);
    lock();
    const uint32_t target = g_rt.closing_target;
    unlock();
    while (still_closing(generation)
           && static_cast<int32_t>(static_cast<uint32_t>(g_engine.completed) - target) < 0) {
        Sleep(1);
    }
    const HANDLE mutex = g_bind.mutex;
    const DWORD wait = mutex ? WaitForSingleObject(mutex, INFINITE) : WAIT_OBJECT_0;
    const bool held = wait == WAIT_OBJECT_0 || wait == WAIT_ABANDONED;
    HMODULE unpinned = NULL;
    lock();
    if (!held) {
        if (still_closing(generation)) {
            g_rt.state = kInstalled;
        }
    } else if (still_closing(generation) && g_rt.handles.open == 0) {
        g_rt.state = kIdle;
        HU_HEAP_CHECK("shutdown thread: before unhook");
        if (clear_handlers()) {
            release_claim();
            retire_record();
            unpinned = g_rt.self_reference;
            g_rt.self_reference = NULL;
        }
        HU_HEAP_CHECK("shutdown thread: after unhook");
    }
    unlock();
    if (held && mutex) {
        ReleaseMutex(mutex);
    }
    if (unpinned) {
        FreeLibrary(self);
        FreeLibraryAndExitThread(unpinned, 0);
    }
    FreeLibraryAndExitThread(self, 0);
}

// Under the lock, with the restore queued at `target`: the engine is closing
// and finish_shutdown() runs on a reference of its own. False, the engine
// installed as before, when the thread could not start.
bool start_closing(uint32_t target) {
    HMODULE pin = NULL;
    if (!GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, &module_anchor_, &pin) || !pin) {
        return false;
    }
    g_rt.closing_target = target;
    const LONG generation = InterlockedIncrement(&g_rt.closing);
    g_rt.state = kClosing;
    const HANDLE thread = CreateThread(NULL, 0, &finish_shutdown,
        reinterpret_cast<LPVOID>(static_cast<intptr_t>(generation)), 0, NULL);
    if (!thread) {
        g_rt.state = kInstalled;
        FreeLibrary(pin);
        return false;
    }
    CloseHandle(thread);
    return true;
}

// Restores everything the engine changed and unhooks, never waiting for the
// game thread: the restore is queued, and finish_shutdown() unhooks once a
// frame has run it, the engine closing until then. Refuses while any handle
// in the client is open (kind "handles"); a full queue, or a thread that
// could not start, leaves the engine installed ("drain"). A hooked call still
// running when the handlers come out leaves the engine idle and pinned; the
// next shutdown clears them here, and refuses ("busy") while it still runs.
// Past that unpin this image runs only because every caller holds a
// reference on it: its own Lua state, or a forwarder.
int32_t shutdown_locked(char* why, uint32_t why_size, char* kind, uint32_t kind_size) {
    HU_STEP("shutdown_locked");
    char text[256];
    lock();
    if (g_rt.handles.open > 0) {
        snprintf(text, sizeof(text), "%d hideui handle(s) still open", g_rt.handles.open);
        unlock();
        return refusal_out(why, why_size, text, kind, kind_size, "handles");
    }
    if (g_rt.state == kClosing) {
        unlock();
        return 1;
    }
    if (g_rt.state == kInstalled) {
        HU_HEAP_CHECK("shutdown: before queuing restore_all");
        if (!g_engine.commands.push(command(kOpRestoreAll, 0, 0, 0, NULL, 0, kVerbNone))) {
            unlock();
            return refusal_out(why, why_size, "the command queue is full; the engine stays installed",
                kind, kind_size, "drain");
        }
        const bool started = start_closing(g_engine.commands.pushed());
        unlock();
        if (!started) {
            return refusal_out(why, why_size, "the engine could not start its shutdown; it stays installed",
                kind, kind_size, "drain");
        }
        return 1;
    }
    if (g_rt.pinned_busy && g_rt.daemon) {
        unhook();
    }
    const bool busy = g_rt.pinned_busy;
    unlock();
    if (busy) {
        return refusal_out(why, why_size,
            "a hooked call was still running; the engine stays pinned for this session", kind, kind_size, "busy");
    }
    return 1;
}

int32_t __stdcall op_shutdown(char* why, uint32_t why_size, char* kind, uint32_t kind_size) {
    // Every caller already holds the election mutex, so no copy adopts this
    // resident between its check and a retire here; taking it again is
    // recursive. finish_shutdown() takes it for its own.
    const HANDLE mutex = g_bind.mutex;
    if (mutex) {
        const DWORD wait = WaitForSingleObject(mutex, kElectionWaitMs);
        if (wait != WAIT_OBJECT_0 && wait != WAIT_ABANDONED) {
            return refusal_out(why, why_size,
                "another hideui engine held the election lock; the engine stays installed", kind, kind_size,
                "election");
        }
    }
    const int32_t done = shutdown_locked(why, why_size, kind, kind_size);
    if (mutex) {
        ReleaseMutex(mutex);
    }
    return done;
}

// ---------------------------------------------------------------------------
// Settings (core.h's kSettings): read from the game's store on this thread,
// set on the game's by game.h's setting_command().

const char* const kValueNames[] = {"number", "bool", "choice", "color", "key"};
const char kNotInGame[] = "the game's settings are not loaded: the player is not in the game";
const char kNotStored[] = "the game's store does not hold this setting";

// A setting as Lua sees it: a number, a boolean, a choice's name (its
// number for a value no choice names), a color's {r, g, b}.
void write_setting_value(ReplyWriter& w, const SettingSpec& spec, int32_t raw) {
    switch (spec.value) {
    case kValueBool:
        w.boolean((raw != 0) != ((spec.flags & kInverted) != 0));
        return;
    case kValueChoice:
        for (const SettingChoice* c = spec.choices; c->name; ++c) {
            if (c->value == raw) {
                w.string(c->name);
                return;
            }
        }
        w.number(raw);
        return;
    case kValueColor: {
        const uint32_t v = static_cast<uint32_t>(raw);
        w.table();
        w.set_number("r", color_red(v));
        w.set_number("g", color_green(v));
        w.set_number("b", color_blue(v));
        return;
    }
    default:
        w.number(raw);
        return;
    }
}

// settings(): every setting in the Config menu's order, each {name, page,
// kind, value, default, min, max, choices, label}: min and max for a number
// and a key, a color's for each channel; choices for a choice, in its
// page's row order; default nil where the game keeps none; label the game's
// text, nil where it has none.
void __stdcall op_settings9(const HuEngineHandle* ref, HuEngineReply* reply) {
    HU_STEP("op_settings9");
    ReplyWriter w = {reply};
    if (!readable_for(ref, w)) {
        return;
    }
    const SettingSites& s = g_engine.game.settings;
    if (!store_loaded(s)) {
        w.fail(kNotInGame);
        return;
    }
    const SettingLabels& labels = g_engine.setting_labels;
    const bool labeled = labels.ready != 0;
    MemoryBarrier();
    w.table();
    for (int i = 0; i < kSettingCount; ++i) {
        const SettingSpec& spec = kSettings[i];
        w.table();
        w.set_string("name", spec.name);
        w.set_string("page", spec.page);
        w.set_string("kind", kValueNames[spec.value]);
        int32_t raw = 0;
        if (setting_raw(s, i, false, &raw)) {
            write_setting_value(w, spec, raw);
            w.field("value");
        }
        if (setting_raw(s, i, true, &raw)) {
            write_setting_value(w, spec, raw);
            w.field("default");
        }
        int32_t lo = 0;
        int32_t hi = 0;
        if ((spec.value == kValueNumber || spec.value == kValueKey || spec.value == kValueColor)
            && setting_range(s, i, &lo, &hi)) {
            w.set_number("min", lo);
            w.set_number("max", hi);
        }
        if (spec.value == kValueChoice) {
            w.table();
            for (int k = 0; spec.choices[k].name; ++k) {
                w.string(spec.choices[k].name);
                w.index(k + 1);
            }
            w.field("choices");
        }
        if (labeled && labels.text[i][0]) {
            w.set_string("label", labels.text[i]);
        }
        w.index(i + 1);
    }
}

// setting(name): its value.
void __stdcall op_setting9(const HuEngineHandle* ref, const char* name, HuEngineReply* reply) {
    HU_STEP("op_setting9");
    ReplyWriter w = {reply};
    if (!readable_for(ref, w)) {
        return;
    }
    const int i = find_setting(name);
    int32_t raw = 0;
    if (i < 0) {
        char why[kWhyBytes];
        snprintf(why, sizeof(why), "no such setting: %s (settings() lists them)", name ? name : "(not a string)");
        w.fail(why);
    } else if (!store_loaded(g_engine.game.settings)) {
        w.fail(kNotInGame);
    } else if (!setting_raw(g_engine.game.settings, i, false, &raw)) {
        w.fail(kNotStored);
    } else {
        write_setting_value(w, kSettings[i], raw);
    }
}

// The value set_setting(name, value) takes for setting i, of Lua type
// `kind` (hideui_engine_abi.h's HU_ANSWER_*), as setting_raw() reads it:
// -1 with `why` the misuse, a value of another type than its kind takes;
// else 1, and *raw within lo..hi, or 0 with `why` the values it takes.
int32_t setting_value_of(int i, int32_t kind, double number, const char* text, int32_t lo, int32_t hi, int32_t* raw,
                         char* why, size_t size) {
    const SettingSpec& spec = kSettings[i];
    int v = 0;
    switch (spec.value) {
    case kValueBool:
        if (kind != HU_ANSWER_BOOLEAN) {
            snprintf(why, size, "set_setting('%s', value): %s takes true or false", spec.name, spec.name);
            return -1;
        }
        *raw = (number != 0) != ((spec.flags & kInverted) != 0) ? 1 : 0;
        return 1;
    case kValueChoice: {
        if (kind != HU_ANSWER_STRING) {
            snprintf(why, size, "set_setting('%s', value): %s takes one of its choices' names, a string", spec.name,
                spec.name);
            return -1;
        }
        char names[96];
        names[0] = '\0';
        for (int k = 0; spec.choices[k].name; ++k) {
            if (same_lowercase(text, spec.choices[k].name)) {
                *raw = spec.choices[k].value;
                return 1;
            }
            const size_t used = strlen(names);
            snprintf(names + used, sizeof(names) - used, "%s%s", k == 0 ? "" : spec.choices[k + 1].name ? ", " : " or ",
                spec.choices[k].name);
        }
        snprintf(why, size, "no such choice of %s: %s (%s)", spec.name, text, names);
        return 0;
    }
    case kValueColor: {
        AnswerTable t;
        double channel[3] = {0, 0, 0};
        int given = 0;
        bool shaped = kind == HU_ANSWER_TABLE && parse_answer_table(text, &t) && !t.too_large;
        for (int k = 0; shaped && k < t.count; ++k) {
            const TableEntry& e = t.entry[k];
            const int c = e.parts != 1 || e.type != 'n' ? -1 : strcmp(e.key[0], "r") == 0 ? 0
                : strcmp(e.key[0], "g") == 0 ? 1 : strcmp(e.key[0], "b") == 0 ? 2 : -1;
            shaped = c >= 0 && !(given & (1 << c));
            if (shaped) {
                channel[c] = e.number;
                given |= 1 << c;
            }
        }
        if (!shaped || given != 7) {
            snprintf(why, size, "set_setting('%s', value): %s takes a color, {r = red, g = green, b = blue}, each a"
                " number", spec.name, spec.name);
            return -1;
        }
        int rgb[3];
        for (int k = 0; k < 3; ++k) {
            if (!int_value(channel[k], lo, hi, &rgb[k])) {
                snprintf(why, size, "each of r, g and b is a whole number from %d to %d", static_cast<int>(lo),
                    static_cast<int>(hi));
                return 0;
            }
        }
        *raw = static_cast<int32_t>(pack_color(rgb[0], rgb[1], rgb[2]));
        return 1;
    }
    default:
        if (kind != HU_ANSWER_NUMBER) {
            snprintf(why, size, "set_setting('%s', value): %s takes %s, a number", spec.name, spec.name,
                spec.value == kValueKey ? "a DirectInput key code" : "a whole number");
            return -1;
        }
        if (!int_value(number, lo, hi, &v)) {
            snprintf(why, size, "%s takes a whole number from %d to %d", spec.name, static_cast<int>(lo),
                static_cast<int>(hi));
            return 0;
        }
        *raw = v;
        return 1;
    }
}

// set_setting(name, value): misuse (-1) for a value of another type than
// the setting's kind takes; refused up front for an unknown name, a value
// outside the setting's range (the store's, read now), and while the
// player's values are not loaded; refused by an error event while a page
// that writes its own copy back as it closes is open.
int32_t __stdcall op_set_setting9(const HuEngineHandle* ref, const char* name, int32_t kind, double number,
                                  const char* text, char* why, uint32_t size) {
    HU_STEP("op_set_setting9");
    if (!name) {
        snprintf(why, size, "set_setting(name, value) needs a setting's name");
        return -1;
    }
    const int i = find_setting(name);
    if (i < 0) {
        snprintf(why, size, "no such setting: %s (settings() lists them)", name);
        return 0;
    }
    int32_t probe = 0;
    const int32_t shape = setting_value_of(i, kind, number, text, INT_MIN, INT_MAX, &probe, why, size);
    if (shape < 0) {
        return -1;
    }
    bool ok = false;
    lock();
    if (usable(ref, why, size)) {
        const SettingSites& s = g_engine.game.settings;
        int32_t lo = 0;
        int32_t hi = 0;
        int32_t raw = 0;
        if (!store_loaded(s)) {
            snprintf(why, size, "%s", kNotInGame);
        } else if (!setting_range(s, i, &lo, &hi)) {
            snprintf(why, size, "%s", kNotStored);
        } else if (setting_value_of(i, kind, number, text, lo, hi, &raw, why, size) == 1) {
            ok = enqueue(command(kOpSetSetting, i, 0, 0, ref, static_cast<uint32_t>(raw), kVerbSetSetting), why, size);
        }
    }
    unlock();
    return ok;
}

const HuEngineApi kOwnApi = {
    kAbi,
    sizeof(HuEngineApi),
    &op_new,
    &op_release,
    &op_hide,
    &op_unhide,
    &op_block,
    &op_unblock,
    &op_move,
    &op_move_group,
    &op_reset,
    &op_reset_all,
    &op_open,
    &op_close,
    &op_resize,
    &op_query_answer,
    &op_query_cancel,
    &op_passinpu_submit,
    &op_passinpu_cancel,
    &op_prtyjoin_accept,
    &op_prtyjoin_decline,
    &op_post_close,
    &op_link5_confirm,
    &op_link5_cancel,
    &op_arealist_confirm,
    &op_arealist_cancel,
    &op_poll,
    &op_info,
    &op_positions,
    &op_list,
    &op_groups,
    &op_status,
    &op_version,
    &op_shutdown,
    &op_query_options,
    &op_handle_open,
    &op_hide3,
    &op_unhide3,
    &op_block3,
    &op_unblock3,
    &op_open3,
    &op_close3,
    &op_reset3,
    &op_move3,
    &op_move_group3,
    &op_resize3,
    &op_answer,
    &op_cancel,
    &op_poll3,
    &op_info3,
    &op_options,
    &op_list3,
    &op_opened,
    &op_focused,
    &op_positions3,
    &op_groups3,
    &op_layout,
    &op_pending,
    &op_status3,
    &op_move_group4,
    &op_close4,
    &op_answer4,
    &op_cancel4,
    &op_poll4,
    &op_info4,
    &op_options4,
    &op_list4,
    &op_focused4,
    &op_groups4,
    &op_pending4,
    &op_status4,
    &op_move5,
    &op_move_group5,
    &op_reset5,
    &op_reset_group5,
    &op_answer5,
    &op_cancel5,
    &op_poll5,
    &op_options5,
    &op_pending5,
    &op_rects5,
    &op_block5,
    &op_block_macros,
    &op_unblock_macros,
    &op_macros,
    &op_block7,
    &op_unblock7,
    &op_open8,
    &op_close8,
    &op_block8,
    &op_answer8,
    &op_cancel8,
    &op_options8,
    &op_poll8,
    &op_pending8,
    &op_offer8,
    &op_withdraw8,
    &op_open9,
    &op_open_set9,
    &op_settings9,
    &op_setting9,
    &op_set_setting9,
};

// ---------------------------------------------------------------------------
// The election: the first copy to install publishes its table into the
// pid-scoped record, and every later copy reads it and forwards.

enum Role { kRoleNone = 0, kRoleResident, kRoleForwarder };

struct Route {
    const HuEngineApi* api;     // NULL: this copy refuses; g_bind.refusal says why
    int role;
    bool have_resident;
    char path[MAX_PATH];        // the resident's
    char build[HU_BUILD_MAX];
    uint32_t abi;
};

bool open_election(char* why, size_t size) {
    if (g_bind.record) {
        return true;
    }
    const unsigned pid = static_cast<unsigned>(GetCurrentProcessId());
    char mutex_name[HU_NAME_MAX];
    char mapping_name[HU_NAME_MAX];
    snprintf(mutex_name, sizeof(mutex_name), HU_ENGINE_MUTEX_NAME_FORMAT, pid);
    snprintf(mapping_name, sizeof(mapping_name), HU_ENGINE_MAPPING_NAME_FORMAT, pid);
    HANDLE mutex = CreateMutexA(NULL, FALSE, mutex_name);
    if (!mutex) {
        snprintf(why, size, "election: could not create %s (%lu)", mutex_name, GetLastError());
        return false;
    }
    HANDLE mapping = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0,
        HU_ENGINE_MAPPING_BYTES, mapping_name);
    if (!mapping) {
        snprintf(why, size, "election: could not create %s (%lu)", mapping_name, GetLastError());
        CloseHandle(mutex);
        return false;
    }
    void* view = MapViewOfFile(mapping, FILE_MAP_ALL_ACCESS, 0, 0, HU_ENGINE_MAPPING_BYTES);
    if (!view) {
        snprintf(why, size, "election: could not map %s (%lu)", mapping_name, GetLastError());
        CloseHandle(mapping);
        CloseHandle(mutex);
        return false;
    }
    g_bind.mutex = mutex;
    g_bind.mapping = mapping;
    g_bind.record = static_cast<HuEngineRecord*>(view);
    return true;
}

void refuse(Route* r, const char* why) {
    r->api = NULL;
    r->role = kRoleNone;
    snprintf(g_bind.refusal, sizeof(g_bind.refusal), "%s", why);
}

// A fixed-size text field of the record, which a writer that died may have
// left unterminated.
void copy_field(char* out, size_t size, const char* field, size_t field_size) {
    size_t n = 0;
    for (; n < field_size && n + 1 < size && field[n]; ++n) {
        out[n] = field[n];
    }
    out[n] = '\0';
}

void note_resident(Route* r, const HuEngineRecord* rec) {
    r->have_resident = true;
    r->abi = rec->abi_version;
    copy_field(r->path, sizeof(r->path), rec->resident_path, sizeof(rec->resident_path));
    copy_field(r->build, sizeof(r->build), rec->resident_build, sizeof(rec->resident_build));
}

void image_path(char* out, size_t size) {
    WCHAR wide[MAX_PATH];
    out[0] = '\0';
    if (self_module(wide, MAX_PATH)) {
        narrow(wide, out, static_cast<int>(size));
    }
}

enum Adoption { kStale, kAdopted, kRefused };

// Any resident, older or newer: what it published, up to this copy's table,
// is copied; a slot past its `size` reads as missing, and the call answers
// that it needs a newer resident.
Adoption adopt(const HuEngineRecord* rec, Route* r) {
    note_resident(r, rec);
    char why[kWhyBytes];
    size_t published = rec->api.size;
    const size_t room = rec->record_size > offsetof(HuEngineRecord, api)
        ? rec->record_size - offsetof(HuEngineRecord, api) : 0;
    if (published > room) {
        published = room;
    }
    if (published > sizeof(HuEngineApi)) {
        published = sizeof(HuEngineApi);
    }
    if (published < offsetof(HuEngineApi, handle_release) + sizeof(void*)) {
        snprintf(why, sizeof(why), "forward: the resident hideui engine %s from %s published no table",
            r->build, r->path);
        refuse(r, why);
        return kRefused;
    }
    HuEngineApi api;
    memset(&api, 0, sizeof(api));
    memcpy(&api, &rec->api, published);
    api.size = static_cast<uint32_t>(published);
    for (size_t at = offsetof(HuEngineApi, handle_new); at + sizeof(void*) <= published; at += sizeof(void*)) {
        const void* fn;
        memcpy(&fn, reinterpret_cast<const uint8_t*>(&api) + at, sizeof(fn));
        if (!fn) {
            snprintf(why, sizeof(why), "forward: the resident hideui engine %s from %s published an incomplete table",
                r->build, r->path);
            refuse(r, why);
            return kRefused;
        }
    }

    // The resident must still be the image that published. The reference
    // keeps it mapped while this copy has Lua states: a forwarded shutdown
    // releases the resident's own pin from inside the resident.
    const void* entry;
    memcpy(&entry, &api.handle_new, sizeof(entry));
    HMODULE module = NULL;
    if (!GetModuleHandleExA(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS, static_cast<LPCSTR>(entry), &module)
        || !module) {
        r->have_resident = false;
        return kStale;
    }
    if (static_cast<const void*>(module) != rec->module) {
        FreeLibrary(module);
        r->have_resident = false;
        return kStale;
    }
    for (int i = 0; i < g_bind.adopted_count; ++i) {
        if (g_bind.adopted[i]->module == module) {
            FreeLibrary(module);
            r->api = &g_bind.adopted[i]->api;
            r->role = kRoleForwarder;
            return kAdopted;
        }
    }
    if (g_bind.adopted_count == g_bind.adopted_capacity) {
        const int grown = g_bind.adopted_capacity ? g_bind.adopted_capacity * 2 : 4;
        Adopted** bigger = static_cast<Adopted**>(
            HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(Adopted*) * grown));
        if (!bigger) {
            FreeLibrary(module);
            refuse(r, "forward: out of memory");
            return kRefused;
        }
        if (g_bind.adopted) {
            memcpy(bigger, g_bind.adopted, sizeof(Adopted*) * g_bind.adopted_count);
            HeapFree(GetProcessHeap(), 0, g_bind.adopted);
        }
        g_bind.adopted = bigger;
        g_bind.adopted_capacity = grown;
    }
    Adopted* a = static_cast<Adopted*>(HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(Adopted)));
    if (!a) {
        FreeLibrary(module);
        refuse(r, "forward: out of memory");
        return kRefused;
    }
    a->module = module;
    a->api = api;
    snprintf(a->path, sizeof(a->path), "%s", r->path);
    g_bind.adopted[g_bind.adopted_count++] = a;
    r->api = &a->api;
    r->role = kRoleForwarder;
    return kAdopted;
}

// With this copy installed. The magic goes last, behind a barrier: a reader
// that sees it sees a complete record.
void publish(HuEngineRecord* rec) {
    HuEngineRecord fresh;
    memset(&fresh, 0, sizeof(fresh));
    fresh.record_size = sizeof(fresh);
    fresh.abi_version = kAbi;
    fresh.module = self_base();
    image_path(fresh.resident_path, sizeof(fresh.resident_path));
    memcpy(fresh.resident_build, HU_ENGINE_BUILD, sizeof(HU_ENGINE_BUILD));
    fresh.api = kOwnApi;
    fresh.api.size = HU_ENGINE_PUBLISHED_SIZE;
    rec->magic = 0;
    MemoryBarrier();
    memcpy(reinterpret_cast<uint8_t*>(rec) + sizeof(rec->magic),
           reinterpret_cast<const uint8_t*>(&fresh) + sizeof(fresh.magic), sizeof(fresh) - sizeof(fresh.magic));
    MemoryBarrier();
    rec->magic = HU_ENGINE_MAGIC;
}

// Under the bind lock. Returns true holding the election mutex, which the
// caller releases once its call through the route is done. A record whose
// magic is not set, or whose image is gone, is free to publish over. With
// may_install, a client with no resident gets this copy installed and
// published; without, no resident.
bool elect(bool may_install, Route* r) {
    HU_STEP("elect");
    memset(r, 0, sizeof(*r));
    r->api = &kOwnApi;
    char why[kWhyBytes];
    if (!self_base()) {
        refuse(r, "election: this image could not resolve its own module");
        return false;
    }
    if (!open_election(why, sizeof(why))) {
        refuse(r, why);
        return false;
    }
    const DWORD wait = WaitForSingleObject(g_bind.mutex, kElectionWaitMs);
    if (wait != WAIT_OBJECT_0 && wait != WAIT_ABANDONED) {
        refuse(r, "election: another hideui engine held the election lock for 5 seconds");
        return false;
    }
    HuEngineRecord* rec = g_bind.record;
    if (rec->magic == HU_ENGINE_MAGIC && rec->record_size >= offsetof(HuEngineRecord, api)) {
        if (rec->module == self_base()) {
            note_resident(r, rec);
            r->role = kRoleResident;
            return true;
        }
        if (adopt(rec, r) != kStale) {
            return true;
        }
    }
    if (may_install) {
        lock();
        const bool installed = install_engine();
        unlock();
        if (installed) {
            publish(rec);
            note_resident(r, rec);
            r->role = kRoleResident;
        }
    }
    return true;
}

// The last Lua state of this copy is closing, so no handle made through a
// forwarded table is left: the references on those residents go.
void release_adopted() {
    for (int i = 0; i < g_bind.adopted_count; ++i) {
        Adopted* a = g_bind.adopted[i];
        g_bind.adopted[i] = NULL;
        FreeLibrary(a->module);
        HeapFree(GetProcessHeap(), 0, a);
    }
    g_bind.adopted_count = 0;
}

// ---------------------------------------------------------------------------
// Lua. The same bindings serve a resident and a forwarder: a handle calls
// through the table it was made with. A call whose slot that table lacks --
// a resident older than this copy -- answers that it needs a newer one.
// Misuse, an argument of the wrong type or a missing one, raises; a call the
// engine cannot carry out returns nil and why.

struct HandleRef {
    HuEngineHandle h;
    const HuEngineApi* api;     // NULL: made while this copy refused
};

// One per Lua state that opened this image, finalized after every handle.
struct Sentinel {
    int counted;
};

HandleRef* ref_at(lua_State* L) {
    return static_cast<HandleRef*>(luaL_checkudata(L, 1, kHandleType));
}

int push_fail(lua_State* L, const char* why) {
    lua_pushnil(L);
    lua_pushstring(L, why);
    return 2;
}

int push_true(lua_State* L) {
    lua_pushboolean(L, 1);
    return 1;
}

int push_refusal(lua_State* L, const char* why, const char* kind) {
    lua_pushboolean(L, 0);
    lua_pushstring(L, why);
    lua_pushstring(L, kind);
    return 3;
}

void set_number(lua_State* L, const char* key, double value) {
    lua_pushnumber(L, value);
    lua_setfield(L, -2, key);
}

void set_bool(lua_State* L, const char* key, bool value) {
    lua_pushboolean(L, value ? 1 : 0);
    lua_setfield(L, -2, key);
}

void set_string(lua_State* L, const char* key, const char* value) {
    lua_pushstring(L, value);
    lua_setfield(L, -2, key);
}

const char* check_string(lua_State* L, int index) {
    luaL_checktype(L, index, LUA_TSTRING);
    return lua_tostring(L, index);
}

double check_number(lua_State* L, int index) {
    luaL_checktype(L, index, LUA_TNUMBER);
    return lua_tonumber(L, index);
}

int push_refused(lua_State* L) {
    char why[kWhyBytes];
    bind_lock();
    snprintf(why, sizeof(why), "%s", g_bind.refusal);
    bind_unlock();
    return push_fail(L, why);
}

#define HU_SLOT(field) offsetof(HuEngineApi, field)

bool has_slot(const HuEngineApi* api, size_t offset) {
    if (offset + sizeof(void*) > api->size) {
        return false;
    }
    const void* fn;
    memcpy(&fn, reinterpret_cast<const uint8_t*>(api) + offset, sizeof(fn));
    return fn != NULL;
}

template <typename Fn>
Fn slot_of(const HuEngineApi* api, size_t offset) {
    Fn fn;
    memcpy(&fn, reinterpret_cast<const uint8_t*>(api) + offset, sizeof(fn));
    return fn;
}

// The first build whose table had the slot at `offset`.
const char* first_build(size_t offset) {
    if (offset >= HU_SLOT(open8)) {
        return "0.11.0";
    }
    if (offset >= HU_SLOT(block7)) {
        return "0.10.0";
    }
    if (offset >= HU_SLOT(block_macros)) {
        return "0.9.0";
    }
    if (offset >= HU_SLOT(move5)) {
        return "0.7.0";
    }
    if (offset >= HU_SLOT(move_group4)) {
        return "0.5.1";
    }
    if (offset >= HU_SLOT(handle_open)) {
        return "0.5.0";
    }
    return offset >= HU_SLOT(query_options) ? "0.4.2" : "0.2.0";
}

// The last slot this copy's calls use. A handle is made only through a
// resident whose table reaches it: an older one would serve some of the
// calls and refuse the rest.
const size_t kNewestSlot = HU_SLOT(set_setting9);

// "<verb> needs hideui <first build> or newer; the resident copy is <path>".
void needs_text(const char* verb, const HuEngineApi* api, size_t offset, char* out, size_t size) {
    char path[MAX_PATH];
    path[0] = '\0';
    bind_lock();
    for (int i = 0; i < g_bind.adopted_count; ++i) {
        if (&g_bind.adopted[i]->api == api) {
            snprintf(path, sizeof(path), "%s", g_bind.adopted[i]->path);
        }
    }
    bind_unlock();
    if (!path[0]) {
        image_path(path, sizeof(path));
    }
    snprintf(out, size, "%s needs hideui %s or newer; the resident copy is %s", verb, first_build(offset), path);
}

int push_needs(lua_State* L, const char* verb, const HuEngineApi* api, size_t offset) {
    char why[kWhyBytes + MAX_PATH];
    needs_text(verb, api, offset, why, sizeof(why));
    return push_fail(L, why);
}

// The handle's table when it has the slot; else NULL with nil and the
// reason pushed, `*pushed` their count.
const HuEngineApi* table_for(lua_State* L, HandleRef* ref, const char* verb, size_t offset, int* pushed) {
    if (!ref->api) {
        *pushed = push_refused(L);
        return NULL;
    }
    if (!has_slot(ref->api, offset)) {
        *pushed = push_needs(L, verb, ref->api, offset);
        return NULL;
    }
    return ref->api;
}

// Pushes the values a reply encodes and returns how many. A reply this copy
// cannot read pushes nil and the reason instead.
int replay(lua_State* L, const uint8_t* data, uint32_t used) {
    const int kDepth = 32;
    const int base = lua_gettop(L);
    bool is_table[kDepth];
    int depth = 0;
    uint32_t at = 0;
    bool ok = lua_checkstack(L, kDepth + 4) != 0;
    while (ok && at < used) {
        const uint8_t op = data[at++];
        const bool pushes = op >= HU_REPLY_NIL && op <= HU_REPLY_TABLE;
        if (pushes && depth == kDepth) {
            ok = false;
            break;
        }
        uint32_t length = 0;
        if (op == HU_REPLY_STRING || op == HU_REPLY_FIELD) {
            if (used - at < 4) {
                ok = false;
                break;
            }
            memcpy(&length, data + at, 4);
            at += 4;
            if (used - at < length) {
                ok = false;
                break;
            }
        }
        switch (op) {
        case HU_REPLY_NIL:
            lua_pushnil(L);
            is_table[depth++] = false;
            break;
        case HU_REPLY_FALSE:
        case HU_REPLY_TRUE:
            lua_pushboolean(L, op == HU_REPLY_TRUE);
            is_table[depth++] = false;
            break;
        case HU_REPLY_NUMBER: {
            if (used - at < 8) {
                ok = false;
                break;
            }
            double v;
            memcpy(&v, data + at, 8);
            at += 8;
            lua_pushnumber(L, v);
            is_table[depth++] = false;
            break;
        }
        case HU_REPLY_STRING:
            lua_pushlstring(L, reinterpret_cast<const char*>(data + at), length);
            at += length;
            is_table[depth++] = false;
            break;
        case HU_REPLY_TABLE:
            lua_newtable(L);
            is_table[depth++] = true;
            break;
        case HU_REPLY_FIELD:
            if (depth < 2 || !is_table[depth - 2]) {
                ok = false;
                break;
            }
            lua_pushlstring(L, reinterpret_cast<const char*>(data + at), length);
            at += length;
            lua_insert(L, -2);
            lua_rawset(L, -3);
            --depth;
            break;
        case HU_REPLY_INDEX: {
            if (used - at < 4 || depth < 2 || !is_table[depth - 2]) {
                ok = false;
                break;
            }
            int32_t i;
            memcpy(&i, data + at, 4);
            at += 4;
            lua_rawseti(L, -2, i);
            --depth;
            break;
        }
        default:
            ok = false;
            break;
        }
    }
    if (!ok) {
        lua_settop(L, base);
        return push_fail(L, "the hideui engine sent a reply this copy cannot read");
    }
    return depth;
}

// Runs a read into a buffer that is a Lua userdata, so a Lua error frees
// it, growing it to what the resident counted when it did not fit.
template <typename Read>
int read_reply(lua_State* L, Read read) {
    uint32_t capacity = 16384;
    for (int attempt = 0; attempt < 8; ++attempt) {
        uint8_t* data = static_cast<uint8_t*>(lua_newuserdata(L, capacity));
        HuEngineReply out;
        out.size = sizeof(out);
        out.data = data;
        out.capacity = capacity;
        out.used = 0;
        read(&out);
        if (out.used <= capacity) {
            const int buffer = lua_gettop(L);
            const int n = replay(L, data, out.used);
            lua_remove(L, buffer);
            return n;
        }
        lua_pop(L, 1);
        if (out.used > (16u << 20)) {
            break;
        }
        capacity = out.used + out.used / 4;
    }
    return push_fail(L, "the hideui engine's reply did not fit");
}

int verb_result(lua_State* L, int32_t result, const char* why) {
    if (result < 0) {
        return luaL_error(L, "%s", why);
    }
    return result ? push_true(L) : push_fail(L, why);
}

int window_verb(lua_State* L, const char* verb, size_t offset) {
    HU_LUA_STEP(verb);
    HandleRef* ref = ref_at(L);
    const char* name = check_string(L, 2);
    int pushed = 0;
    const HuEngineApi* api = table_for(L, ref, verb, offset, &pushed);
    if (!api) {
        return pushed;
    }
    char why[kWhyBytes];
    why[0] = '\0';
    return verb_result(L, slot_of<HuEngineNameVerb>(api, offset)(&ref->h, name, why, sizeof(why)), why);
}

int place_verb(lua_State* L, const char* verb, size_t offset) {
    HU_LUA_STEP(verb);
    HandleRef* ref = ref_at(L);
    const char* name = check_string(L, 2);
    const double x = check_number(L, 3);
    const double y = check_number(L, 4);
    int pushed = 0;
    const HuEngineApi* api = table_for(L, ref, verb, offset, &pushed);
    if (!api) {
        return pushed;
    }
    char why[kWhyBytes];
    why[0] = '\0';
    return verb_result(L, slot_of<HuEnginePlaceVerb>(api, offset)(&ref->h, name, x, y, why, sizeof(why)), why);
}

// unblock(name[, category]): with a category, engine abi 7's slot; without
// one, the slot the call has had, so a resident older than 0.10.0 still
// serves the whole-window form.
int category_verb(lua_State* L, const char* verb, size_t plain, size_t with) {
    HU_LUA_STEP(verb);
    if (lua_isnoneornil(L, 3)) {
        return window_verb(L, verb, plain);
    }
    HandleRef* ref = ref_at(L);
    const char* name = check_string(L, 2);
    const char* category = check_string(L, 3);
    int pushed = 0;
    const HuEngineApi* api = table_for(L, ref, verb, with, &pushed);
    if (!api) {
        return pushed;
    }
    char why[kWhyBytes];
    why[0] = '\0';
    return verb_result(L, slot_of<HuEngineCategoryVerb>(api, with)(&ref->h, name, category, why, sizeof(why)),
                       why);
}

int l_hide(lua_State* L) { return window_verb(L, "hide", HU_SLOT(hide3)); }
int l_unhide(lua_State* L) { return window_verb(L, "unhide", HU_SLOT(unhide3)); }
int l_unblock(lua_State* L) { return category_verb(L, "unblock", HU_SLOT(unblock3), HU_SLOT(unblock7)); }
// open(name[, list]) and open_set(set[, list]): engine abi 9's slots, each
// taking both forms.
int list_verb(lua_State* L, const char* verb, size_t offset) {
    HU_LUA_STEP(verb);
    HandleRef* ref = ref_at(L);
    const char* name = check_string(L, 2);
    const char* list = lua_isnoneornil(L, 3) ? NULL : check_string(L, 3);
    int pushed = 0;
    const HuEngineApi* api = table_for(L, ref, verb, offset, &pushed);
    if (!api) {
        return pushed;
    }
    char why[kWhyBytes];
    why[0] = '\0';
    return verb_result(L, slot_of<HuEngineCategoryVerb>(api, offset)(&ref->h, name, list, why, sizeof(why)), why);
}

int l_open(lua_State* L) { return list_verb(L, "open", HU_SLOT(open9)); }
int l_open_set(lua_State* L) { return list_verb(L, "open_set", HU_SLOT(open_set9)); }
int l_close(lua_State* L) { return window_verb(L, "close", HU_SLOT(close8)); }

// block(name[, category]): engine abi 8's slot takes both forms.
int l_block(lua_State* L) {
    HU_LUA_STEP("block");
    HandleRef* ref = ref_at(L);
    const char* name = check_string(L, 2);
    const char* category = lua_isnoneornil(L, 3) ? NULL : check_string(L, 3);
    int pushed = 0;
    const HuEngineApi* api = table_for(L, ref, "block", HU_SLOT(block8), &pushed);
    if (!api) {
        return pushed;
    }
    char why[kWhyBytes];
    why[0] = '\0';
    return verb_result(L, api->block8(&ref->h, name, category, why, sizeof(why)), why);
}
int l_reset_group(lua_State* L) { return window_verb(L, "reset_group", HU_SLOT(reset_group5)); }
int l_move(lua_State* L) { return place_verb(L, "move", HU_SLOT(move5)); }
int l_move_group(lua_State* L) { return place_verb(L, "move_group", HU_SLOT(move_group5)); }

// reset(name[, aspect]).
int l_reset(lua_State* L) {
    HU_LUA_STEP("reset");
    HandleRef* ref = ref_at(L);
    const char* name = check_string(L, 2);
    const char* aspect = lua_isnoneornil(L, 3) ? NULL : check_string(L, 3);
    int pushed = 0;
    const HuEngineApi* api = table_for(L, ref, "reset", HU_SLOT(reset5), &pushed);
    if (!api) {
        return pushed;
    }
    char why[kWhyBytes];
    why[0] = '\0';
    return verb_result(L, api->reset5(&ref->h, name, aspect, why, sizeof(why)), why);
}

// An optional prompt id at `index`: absent or nil is none, anything but a
// number raises.
int32_t optional_id(lua_State* L, int index, double* id) {
    *id = 0;
    if (lua_isnoneornil(L, index)) {
        return 0;
    }
    *id = check_number(L, index);
    return 1;
}

// cancel(name[, id]).
int l_cancel(lua_State* L) {
    HU_LUA_STEP("cancel");
    HandleRef* ref = ref_at(L);
    const char* name = check_string(L, 2);
    double id = 0;
    const int32_t has_id = optional_id(L, 3, &id);
    int pushed = 0;
    const HuEngineApi* api = table_for(L, ref, "cancel", HU_SLOT(cancel8), &pushed);
    if (!api) {
        return pushed;
    }
    char why[kWhyBytes];
    why[0] = '\0';
    return verb_result(L, api->cancel8(&ref->h, name, has_id, id, why, sizeof(why)), why);
}

// A verb of no window.
int handle_verb(lua_State* L, const char* verb, size_t offset) {
    HU_LUA_STEP(verb);
    HandleRef* ref = ref_at(L);
    int pushed = 0;
    const HuEngineApi* api = table_for(L, ref, verb, offset, &pushed);
    if (!api) {
        return pushed;
    }
    char why[kWhyBytes];
    why[0] = '\0';
    return verb_result(L, slot_of<HuEngineVerb>(api, offset)(&ref->h, why, sizeof(why)), why);
}

int l_reset_all(lua_State* L) { return handle_verb(L, "reset_all", HU_SLOT(reset_all)); }
int l_block_macros(lua_State* L) { return handle_verb(L, "block_macros", HU_SLOT(block_macros)); }
int l_unblock_macros(lua_State* L) { return handle_verb(L, "unblock_macros", HU_SLOT(unblock_macros)); }

// resize(name, rows) or resize(name, w, h). The numbers travel to the
// resident as the text "rows" or "WxH", which it parses and checks.
int l_resize(lua_State* L) {
    HU_LUA_STEP("resize");
    HandleRef* ref = ref_at(L);
    const char* name = check_string(L, 2);
    const double a = check_number(L, 3);
    char text[80];
    if (lua_isnoneornil(L, 4)) {
        snprintf(text, sizeof(text), "%.17g", a);
    } else {
        snprintf(text, sizeof(text), "%.17gx%.17g", a, check_number(L, 4));
    }
    int pushed = 0;
    const HuEngineApi* api = table_for(L, ref, "resize", HU_SLOT(resize3), &pushed);
    if (!api) {
        return pushed;
    }
    char why[kWhyBytes];
    why[0] = '\0';
    return verb_result(L, api->resize3(&ref->h, name, text, why, sizeof(why)), why);
}

// A table onto `out` as hideui_engine_abi.h's HU_ANSWER_TABLE encodes it:
// the table at stack index `index`, its keys after `prefix`; one level of
// tables under it, any deeper one a 't'. False when `out` is full.
struct TableText {
    char* out;
    size_t size;
    size_t used;

    bool put(const char* text, size_t n) {
        if (used + n >= size) {
            return false;
        }
        memcpy(out + used, text, n);
        used += n;
        out[used] = '\0';
        return true;
    }
    bool put(const char* text) { return put(text, strlen(text)); }
};

bool encode_table(lua_State* L, int index, const char* prefix, TableText* t) {
    lua_pushnil(L);
    while (lua_next(L, index)) {
        char key[40];
        key[0] = '?';
        key[1] = '\0';
        if (lua_type(L, -2) == LUA_TNUMBER) {
            const lua_Number k = lua_tonumber(L, -2);
            if (k == static_cast<lua_Number>(static_cast<int>(k))) {
                snprintf(key, sizeof(key), "#%d", static_cast<int>(k));
            }
        } else if (lua_type(L, -2) == LUA_TSTRING) {
            size_t n = 0;
            const char* k = lua_tolstring(L, -2, &n);
            bool name = n > 0 && n < sizeof(key);
            for (size_t i = 0; name && i < n; ++i) {
                name = (k[i] >= 'a' && k[i] <= 'z') || (k[i] >= 'A' && k[i] <= 'Z') || (k[i] >= '0' && k[i] <= '9')
                    || k[i] == '_';
            }
            if (name) {
                memcpy(key, k, n);
                key[n] = '\0';
            }
        }
        char path[96];
        snprintf(path, sizeof(path), "%s%s%s=", prefix, prefix[0] ? "/" : "", key);
        bool ok = true;
        switch (lua_type(L, -1)) {
        case LUA_TSTRING: {
            size_t n = 0;
            const char* v = lua_tolstring(L, -1, &n);
            ok = t->put(path) && t->put("s");
            for (size_t i = 0; ok && i < n; ++i) {
                char hex[3];
                snprintf(hex, sizeof(hex), "%02x", static_cast<unsigned char>(v[i]));
                ok = t->put(hex, 2);
            }
            ok = ok && t->put(";");
            break;
        }
        case LUA_TNUMBER: {
            char number[40];
            snprintf(number, sizeof(number), "n%.17g;", static_cast<double>(lua_tonumber(L, -1)));
            ok = t->put(path) && t->put(number);
            break;
        }
        case LUA_TBOOLEAN:
            ok = t->put(path) && t->put(lua_toboolean(L, -1) ? "b1;" : "b0;");
            break;
        case LUA_TTABLE:
            if (prefix[0]) {
                ok = t->put(path) && t->put("t;");
            } else {
                path[strlen(path) - 1] = '\0';
                ok = encode_table(L, lua_gettop(L), path, t);
            }
            break;
        default:
            ok = t->put(path) && t->put("o;");
            break;
        }
        lua_pop(L, 1);
        if (!ok) {
            lua_pop(L, 1);
            return false;
        }
    }
    return true;
}

// The Lua value at `index` as answer() and set_setting() hand it to the
// resident: its type (HU_ANSWER_*), a number or a boolean (1 or 0) in
// *number, a string, or a table encoded into `encoded`, in *text.
struct AnswerValue {
    int32_t kind;
    double number;
    const char* text;
    char encoded[2048];
};

void answer_value(lua_State* L, int index, AnswerValue* v) {
    v->kind = HU_ANSWER_OTHER;
    v->number = 0;
    v->text = NULL;
    switch (lua_type(L, index)) {
    case LUA_TNONE:
    case LUA_TNIL:
        v->kind = HU_ANSWER_NONE;
        break;
    case LUA_TNUMBER:
        v->kind = HU_ANSWER_NUMBER;
        v->number = lua_tonumber(L, index);
        break;
    case LUA_TSTRING:
        v->kind = HU_ANSWER_STRING;
        v->text = lua_tostring(L, index);
        break;
    case LUA_TBOOLEAN:
        v->kind = HU_ANSWER_BOOLEAN;
        v->number = lua_toboolean(L, index) ? 1 : 0;
        break;
    case LUA_TTABLE: {
        TableText t = {v->encoded, sizeof(v->encoded), 0};
        v->encoded[0] = '\0';
        v->kind = HU_ANSWER_TABLE;
        v->text = encode_table(L, index, "", &t) ? v->encoded : "!";
        break;
    }
    default:
        break;
    }
}

// answer(name, value[, id]): the resident says which type each window
// takes, and a value of another type raises. A table travels encoded.
int l_answer(lua_State* L) {
    HU_LUA_STEP("answer");
    HandleRef* ref = ref_at(L);
    const char* name = check_string(L, 2);
    double id = 0;
    const int32_t has_id = optional_id(L, 4, &id);
    AnswerValue value;
    answer_value(L, 3, &value);
    int pushed = 0;
    const HuEngineApi* api = table_for(L, ref, "answer", HU_SLOT(answer8), &pushed);
    if (!api) {
        return pushed;
    }
    char why[kWhyBytes];
    why[0] = '\0';
    return verb_result(L, api->answer8(&ref->h, name, value.kind, value.number, value.text, has_id, id, why,
                                       sizeof(why)), why);
}

// set_setting(name, value): the resident says which type each setting
// takes, and a value of another type raises. A color's table travels
// encoded, as answer's does.
int l_set_setting(lua_State* L) {
    HU_LUA_STEP("set_setting");
    HandleRef* ref = ref_at(L);
    const char* name = check_string(L, 2);
    AnswerValue value;
    answer_value(L, 3, &value);
    int pushed = 0;
    const HuEngineApi* api = table_for(L, ref, "set_setting", HU_SLOT(set_setting9), &pushed);
    if (!api) {
        return pushed;
    }
    char why[kWhyBytes];
    why[0] = '\0';
    return verb_result(L, api->set_setting9(&ref->h, name, value.kind, value.number, value.text, why, sizeof(why)),
                       why);
}

// offer(name, item, count): the item an inventory index, or 'gil'. Any
// other type of item, or of count, reaches the resident as NaN or NULL, which
// it refuses as misuse.
int l_offer(lua_State* L) {
    HU_LUA_STEP("offer");
    HandleRef* ref = ref_at(L);
    const char* name = check_string(L, 2);
    double item = NAN;
    const char* item_text = NULL;
    if (lua_type(L, 3) == LUA_TNUMBER) {
        item = lua_tonumber(L, 3);
    } else if (lua_type(L, 3) == LUA_TSTRING) {
        item_text = lua_tostring(L, 3);
    }
    const double count = lua_type(L, 4) == LUA_TNUMBER ? lua_tonumber(L, 4) : NAN;
    int pushed = 0;
    const HuEngineApi* api = table_for(L, ref, "offer", HU_SLOT(offer8), &pushed);
    if (!api) {
        return pushed;
    }
    char why[kWhyBytes];
    why[0] = '\0';
    return verb_result(L, api->offer8(&ref->h, name, item, item_text, count, why, sizeof(why)), why);
}

// withdraw(name, item).
int l_withdraw(lua_State* L) {
    HU_LUA_STEP("withdraw");
    HandleRef* ref = ref_at(L);
    const char* name = check_string(L, 2);
    const double item = lua_type(L, 3) == LUA_TNUMBER ? lua_tonumber(L, 3) : NAN;
    int pushed = 0;
    const HuEngineApi* api = table_for(L, ref, "withdraw", HU_SLOT(withdraw8), &pushed);
    if (!api) {
        return pushed;
    }
    char why[kWhyBytes];
    why[0] = '\0';
    return verb_result(L, api->withdraw8(&ref->h, name, item, why, sizeof(why)), why);
}

HandleRef* new_ref(lua_State* L) {
    HandleRef* ref = static_cast<HandleRef*>(lua_newuserdata(L, sizeof(HandleRef)));
    ref->h.slot = -1;
    ref->h.generation = 0;
    ref->api = NULL;
    luaL_getmetatable(L, kHandleType);
    lua_setmetatable(L, -2);
    return ref;
}

// new(name): a handle, or nil and why. Made under the election mutex, so the
// resident cannot retire between the election and the claim.
int l_new(lua_State* L) {
    HU_LUA_STEP("new");
    const char* name = check_string(L, 1);
    HandleRef* ref = new_ref(L);
    char why[kWhyBytes + MAX_PATH];
    why[0] = '\0';
    Route r;
    bind_lock();
    const bool held = elect(true, &r);
    if (!r.api) {
        snprintf(why, sizeof(why), "%s", g_bind.refusal);
    } else if (!has_slot(r.api, kNewestSlot)) {
        needs_text("new", r.api, kNewestSlot, why, sizeof(why));
    } else {
        r.api->handle_open(name, &ref->h, why, sizeof(why));
        if (ref->h.slot >= 0) {
            ref->api = r.api;
        }
    }
    if (held) {
        ReleaseMutex(g_bind.mutex);
    }
    bind_unlock();
    if (ref->h.slot < 0) {
        return push_fail(L, why[0] ? why : "the hideui engine made no handle");
    }
    return 1;
}

void release_ref(HandleRef* ref) {
    if (ref->api && ref->h.slot >= 0) {
        ref->api->handle_release(&ref->h);
    }
    ref->h.slot = -1;
}

int l_release(lua_State* L) {
    HU_LUA_STEP("release");
    release_ref(ref_at(L));
    return push_true(L);
}

int l_gc(lua_State* L) {
    HU_STEP("handle __gc");
    release_ref(static_cast<HandleRef*>(lua_touserdata(L, 1)));
    return 0;
}

// poll() -> the events since the last poll, and how many were lost: the
// resident's reply as it is. Each event is {event, name}; an error also has
// verb and reason, blocked by and mine, cursor row, and pending {event,
// what, pending} names no window. Lost events end the list with {event =
// 'resync', dropped}: what came before it is not the whole story.
int l_poll(lua_State* L) {
    HU_LUA_STEP("poll");
    HandleRef* ref = ref_at(L);
    int pushed = 0;
    const HuEngineApi* api = table_for(L, ref, "poll", HU_SLOT(poll8), &pushed);
    if (!api) {
        return pushed;
    }
    const HuEngineHandle* h = &ref->h;
    const HuEngineRead read = api->poll8;
    return read_reply(L, [=](HuEngineReply* out) { read(h, out); });
}

int handle_read(lua_State* L, const char* verb, size_t offset) {
    HU_LUA_STEP(verb);
    HandleRef* ref = ref_at(L);
    int pushed = 0;
    const HuEngineApi* api = table_for(L, ref, verb, offset, &pushed);
    if (!api) {
        return pushed;
    }
    const HuEngineHandle* h = &ref->h;
    const HuEngineRead read = slot_of<HuEngineRead>(api, offset);
    return read_reply(L, [=](HuEngineReply* out) { read(h, out); });
}

int named_read(lua_State* L, const char* verb, size_t offset) {
    HU_LUA_STEP(verb);
    HandleRef* ref = ref_at(L);
    const char* name = check_string(L, 2);
    int pushed = 0;
    const HuEngineApi* api = table_for(L, ref, verb, offset, &pushed);
    if (!api) {
        return pushed;
    }
    const HuEngineHandle* h = &ref->h;
    const HuEngineNamedRead read = slot_of<HuEngineNamedRead>(api, offset);
    return read_reply(L, [=](HuEngineReply* out) { read(h, name, out); });
}

int l_info(lua_State* L) { return named_read(L, "info", HU_SLOT(info4)); }
int l_options(lua_State* L) { return named_read(L, "options", HU_SLOT(options8)); }
int l_list(lua_State* L) { return handle_read(L, "list", HU_SLOT(list4)); }
int l_opened(lua_State* L) { return handle_read(L, "opened", HU_SLOT(opened)); }
int l_focused(lua_State* L) { return handle_read(L, "focused", HU_SLOT(focused4)); }
int l_remembered(lua_State* L) { return handle_read(L, "remembered", HU_SLOT(positions3)); }
int l_groups(lua_State* L) { return handle_read(L, "groups", HU_SLOT(groups4)); }
int l_layout(lua_State* L) { return handle_read(L, "layout", HU_SLOT(layout)); }
int l_pending(lua_State* L) { return handle_read(L, "pending", HU_SLOT(pending8)); }
int l_rects(lua_State* L) { return handle_read(L, "rects", HU_SLOT(rects5)); }
int l_macros(lua_State* L) { return handle_read(L, "macros", HU_SLOT(macros)); }
int l_settings(lua_State* L) { return handle_read(L, "settings", HU_SLOT(settings9)); }
int l_setting(lua_State* L) { return named_read(L, "setting", HU_SLOT(setting9)); }

const char* role_name(int role) {
    switch (role) {
    case kRoleResident: return "resident";
    case kRoleForwarder: return "forwarder";
    default: return "none";
    }
}

void set_copy(lua_State* L, const char* key, const char* path, const char* build, uint32_t abi) {
    lua_newtable(L);
    set_string(L, "path", path);
    set_string(L, "build", build);
    set_number(L, "abi", abi);
    lua_setfield(L, -2, key);
}

// The route for a call that installs nothing, and the last refusal, copied
// out so no Lua call runs under the bind lock.
void route_for_read(Route* r, char* refusal, size_t size) {
    bind_lock();
    if (elect(false, r)) {
        ReleaseMutex(g_bind.mutex);
    }
    snprintf(refusal, size, "%s", g_bind.refusal);
    bind_unlock();
}

// The resident's status, as whichever copy is asked, with this copy's part
// in it: role at the top, image and resident in `detail`.
int l_status(lua_State* L) {
    HU_LUA_STEP("status");
    Route r;
    char refusal[kWhyBytes];
    route_for_read(&r, refusal, sizeof(refusal));
    const HuEngineApi* api = r.api ? r.api : &kOwnApi;
    if (!has_slot(api, HU_SLOT(status4))) {
        return push_needs(L, "status", api, HU_SLOT(status4));
    }
    char path[MAX_PATH];
    image_path(path, sizeof(path));
    const int n = read_reply(L, [=](HuEngineReply* out) { api->status4(out); });
    if (n != 1 || !lua_istable(L, -1)) {
        return n;
    }
    if (!r.api) {
        set_bool(L, "ok", false);
        set_string(L, "state", "failed");
    }
    set_string(L, "role", role_name(r.role));
    lua_getfield(L, -1, "detail");
    if (lua_istable(L, -1)) {
        if (!r.api) {
            set_string(L, "error", refusal);
        }
        set_copy(L, "image", path, HU_ENGINE_BUILD, kAbi);
        if (r.have_resident) {
            set_copy(L, "resident", r.path, r.build, r.abi);
        }
    }
    lua_pop(L, 1);
    return 1;
}

int l_version(lua_State* L) {
    HU_LUA_STEP("version");
    lua_pushstring(L, "hideui " HU_ENGINE_BUILD);
    return 1;
}

// Goes to the resident, wherever it is: it refuses while any copy holds a
// handle, and on success leaves no resident behind once the game's next
// frame has run.
int l_shutdown(lua_State* L) {
    HU_LUA_STEP("shutdown");
    Route r;
    char why[kWhyBytes];
    char kind[32];
    why[0] = '\0';
    kind[0] = '\0';
    bind_lock();
    const bool held = elect(false, &r);
    const HuEngineApi* api = r.api ? r.api : &kOwnApi;
    const int32_t done = api->shutdown(why, sizeof(why), kind, sizeof(kind));
    if (held) {
        ReleaseMutex(g_bind.mutex);
    }
    bind_unlock();
    return done ? push_true(L) : push_refusal(L, why, kind);
}

int l_image_gc(lua_State* L) {
    HU_STEP("image __gc");
    Sentinel* s = static_cast<Sentinel*>(lua_touserdata(L, 1));
    if (!s || !s->counted) {
        return 0;
    }
    s->counted = 0;
    bind_lock();
    if (--g_bind.users == 0) {
        release_adopted();
    }
    bind_unlock();
    return 0;
}

const luaL_Reg kHandleMethods[] = {
    {"hide", l_hide},
    {"unhide", l_unhide},
    {"block", l_block},
    {"unblock", l_unblock},
    {"open", l_open},
    {"open_set", l_open_set},
    {"close", l_close},
    {"move", l_move},
    {"move_group", l_move_group},
    {"resize", l_resize},
    {"reset", l_reset},
    {"reset_group", l_reset_group},
    {"reset_all", l_reset_all},
    {"answer", l_answer},
    {"cancel", l_cancel},
    {"offer", l_offer},
    {"withdraw", l_withdraw},
    {"info", l_info},
    {"options", l_options},
    {"list", l_list},
    {"opened", l_opened},
    {"focused", l_focused},
    {"remembered", l_remembered},
    {"groups", l_groups},
    {"layout", l_layout},
    {"pending", l_pending},
    {"rects", l_rects},
    {"block_macros", l_block_macros},
    {"unblock_macros", l_unblock_macros},
    {"macros", l_macros},
    {"settings", l_settings},
    {"setting", l_setting},
    {"set_setting", l_set_setting},
    {"poll", l_poll},
    {"release", l_release},
    {NULL, NULL},
};

const luaL_Reg kFunctions[] = {
    {"new", l_new},
    {"status", l_status},
    {"shutdown", l_shutdown},
    {"version", l_version},
    {NULL, NULL},
};

}  // namespace

extern "C" __declspec(dllexport) int luaopen__HideUI(lua_State* L) {
    HU_LUA_STEP("luaopen");
    // Counted only once it is anchored in the registry, so a Lua error on the
    // way leaves nothing to uncount. Made before any handle, so lua_close
    // finalizes it after every handle.
    Sentinel* sentinel = static_cast<Sentinel*>(lua_newuserdata(L, sizeof(Sentinel)));
    sentinel->counted = 0;
    lua_newtable(L);
    lua_pushcfunction(L, l_image_gc);
    lua_setfield(L, -2, "__gc");
    lua_setmetatable(L, -2);
    lua_pushlightuserdata(L, const_cast<char*>(&module_anchor_));
    lua_insert(L, -2);
    lua_rawset(L, LUA_REGISTRYINDEX);
    bind_lock();
    ++g_bind.users;
    sentinel->counted = 1;
    bind_unlock();

    luaL_newmetatable(L, kHandleType);
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    lua_pushcfunction(L, l_gc);
    lua_setfield(L, -2, "__gc");
    luaL_register(L, NULL, kHandleMethods);
    lua_pop(L, 1);

    // Returned to the loader in hideui.lua, not published as a global.
    lua_newtable(L);
    luaL_register(L, NULL, kFunctions);
    return 1;
}
