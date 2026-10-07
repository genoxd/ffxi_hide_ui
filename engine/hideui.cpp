// _HideUI - the hideui engine. Resolves FFXiMain by signature, hooks six of
// its menu routines, the compass draw and the macro key gate through
// hideui_daemon.dll, and serves hideui.lua.
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

#include <stdlib.h>

extern "C" {
#if defined(_WIN32)
#define LUA_BUILD_AS_DLL
#endif
#include "lua.h"
#include "lauxlib.h"
}

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
const DWORD kDrainWaitMs = 1500;
const uint32_t kClearWaitMs = 1000;
const DWORD kElectionWaitMs = 5000;
const int kMaxElements = 128;
const size_t kElementText = 128;
const uint32_t kAbi = HU_ENGINE_ABI_BUILT;
const size_t kWhyBytes = 512;

enum State { kIdle = 0, kInstalled, kFailed };

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
    int32_t sites[kSiteCount];
    bool pinned_busy;
    HMODULE self_reference;
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
    g_rt.lock.enter();
}

void unlock() {
    g_rt.lock.leave();
}

void bind_lock() {
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
    int reads = 0;
    const size_t left = static_cast<size_t>(text + size - hit_test);
    const size_t span = left < kHitTestBytes ? left : kHitTestBytes;
    for (size_t i = 0; i + sizeof(kHitTestMouseRead) <= span; ++i) {
        if (memcmp(hit_test + i, kHitTestMouseRead, sizeof(kHitTestMouseRead)) == 0) {
            mouse_read = hit_test + i;
            ++reads;
        }
    }
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
    const int link5 = g_engine.inv.find_exact("link5");
    const uint8_t* const* link5_slot = link5 >= 0 && g_engine.inv.names[link5].row_count
        ? g_engine.game.slot[g_engine.inv.names[link5].rows[0]] : NULL;
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
    memcpy(&g.template_swap, &g_rt.calls[kCallTemplateSwap], sizeof(g.template_swap));
    memcpy(&g.set_frame_rect, &g_rt.calls[kCallSetFrameRect], sizeof(g.set_frame_rect));
    memcpy(&g.set_cursor, &set_cursor, sizeof(g.set_cursor));
    memcpy(&g.glyph_convert, &glyph_convert, sizeof(g.glyph_convert));
    g.row_hit_test = hit_test;
    g.mouse_read = mouse_read;
    g.menu_routing = routing;
    g.macro_object = macro_object;
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
        && info->callee_pops == 1 && memcmp(info->original, prologue, spec.prologue) == 0;
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
    desc.callee_pops = 1;
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

// The word the macro key gate's prologue reads, pinned at +2 before the
// site is scanned for: the engine knows it from nowhere else, so it is read
// out of the one hit, or, on a hit the daemon has already patched (the jump
// covers +2), out of the original the daemon saved there, which the site's
// acceptance compares against the pinned prologue in turn.
bool pin_macro_gate(const uint8_t* text, size_t size, const SiteSpec& spec, Sig& s, char* why, size_t len) {
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
        desc.callee_pops = 1;
        const int32_t id = g_rt.daemon->install(&desc);
        memset(&info, 0, sizeof(info));
        info.size = sizeof(info);
        if (id < 0 || g_rt.daemon->site_info(id, &info) != HU_OK || info.prologue_bytes < spec.prologue
            || memcmp(info.original, s.bytes, kMacroGateGlobal) != 0) {
            snprintf(why, len, "signature %s: not found (client patched, or another hook owns it)", spec.name);
            return false;
        }
        source = info.original;
    }
    memcpy(s.bytes + kMacroGateGlobal, source + kMacroGateGlobal, 4);
    memset(s.mask + kMacroGateGlobal, 'x', 4);
    return true;
}

bool resolve_sites(const uint8_t* text, size_t size, char* why, size_t len) {
    for (int i = 0; i < kSiteCount; ++i) {
        const SiteSpec& spec = kSites[i];
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
        if (i == kSiteMacroGate && !pin_macro_gate(text, size, spec, s, why, len)) {
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
    return true;
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

bool install_engine() {
    if (g_rt.state == kInstalled) {
        return true;
    }
    if (g_rt.state == kFailed) {
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
        g_rt.resolved = true;
    }
    if (!g_rt.claim && !claim_engine(why, sizeof(why))) {
        return fail(why, kPlayerOldEngine);
    }
    if (!take_self_reference()) {
        release_claim();
        return fail("pin: this image could not take a reference on itself", kPlayerRestart);
    }
    for (int i = 0; i < kSiteCount; ++i) {
        const SiteSpec& spec = kSites[i];
        HuSiteDesc desc;
        desc.size = sizeof(desc);
        desc.target = g_rt.targets[i];
        desc.prologue_bytes = spec.prologue;
        desc.stack_arg_bytes = spec.arg_bytes;
        desc.callee_pops = 1;
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
                    " (prologue %u, %u argument bytes, pops %u; this build's %u, %u, 1)",
                    spec.name, static_cast<unsigned>(info.prologue_bytes),
                    static_cast<unsigned>(info.stack_arg_bytes), static_cast<unsigned>(info.callee_pops),
                    static_cast<unsigned>(spec.prologue), static_cast<unsigned>(spec.arg_bytes));
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
    // The compass as the install finds it: its first opened or closed event
    // is a change from here.
    g_engine.compass_shown = compass_open(compass_guarded()) ? 1 : 0;
    g_rt.pinned_busy = false;
    g_rt.transient = false;
    g_rt.state = kInstalled;
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
        if (!res || !readable(res + kResKey, kKeyLen) || memcmp(res + kResKey, ne.key, kKeyLen) != 0) {
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

// Open as every reader reports it: a window's live instance with the marks
// clear, the compass's state byte.
bool open_of(int n) {
    return is_compass(g_engine, n) ? compass_open(compass_guarded()) : open_menu_guarded(n, NULL) != NULL;
}

// The registry's words for a name; the compass, on no row, reads as zeros.
const RowSpec& spec_of(const NameEntry& ne) {
    static const RowSpec none = {"", "", 0, 0, 0, 0};
    return ne.row_count ? kRowSpecs[ne.rows[0]] : none;
}

// Whether a block may hold the name: never query.
bool blockable(int n) {
    return !hide_only(g_engine.inv.names[n].name);
}

// A string the game owns, copied up to `size` - 1 bytes; a page is checked
// before its first byte is read.
void copy_text(const char* p, char* out, size_t size) {
    size_t i = 0;
    for (; p && i + 1 < size; ++i) {
        const bool page_start = i == 0 || (reinterpret_cast<uintptr_t>(p + i) & 0xFFF) == 0;
        if ((page_start && !readable(p + i, 1)) || p[i] == '\0') {
            break;
        }
        out[i] = p[i];
    }
    out[i] = '\0';
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

const char kQueryHideOnly[] =
    "query cannot be blocked or closed; answer it with answer('query', value) or cancel('query')";

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
// carried it out. Engine abi 3 refuses a second one while one is queued; the
// earlier slots queue it as they always did.
bool enqueue_reply(Command c, bool refuse_queued, char* why, size_t size) {
    const int n = c.target;
    if (n < 0 || n >= g_engine.inv.count) {
        snprintf(why, size, "no window takes this reply");
        return false;
    }
    if (refuse_queued && g_engine.replies[n] > 0) {
        snprintf(why, size, "a reply to %s is already queued", g_engine.inv.names[n].name);
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
    if (g_rt.state == kInstalled) {
        const int slot = g_rt.handles.claim(name, g_engine.events.position());
        if (slot >= 0) {
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
    } else if (g_rt.state == kInstalled) {
        snprintf(why, size, "out of memory for another handle");
    } else {
        snprintf(why, size, "the hideui engine is shut down");
    }
    unlock();
}

void __stdcall op_release(const HuEngineHandle* ref) {
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
    bool ok = false;
    lock();
    Handle* h = usable(ref, why, size);
    if (h) {
        const int n = lookup(name, windows, why, size);
        if (n >= 0) {
            // The compass takes a block as a window does, hidden by the want
            // bit its draw hook reads; no `blocked` event ever follows, the
            // game never attempts an open of it.
            if (bit == kHoldBlock && on && hide_only(g_engine.inv.names[n].name)) {
                snprintf(why, size, "%s", kQueryHideOnly);
            } else {
                g_engine.holds.set(*h, n, bit, on);
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

// The windows the game opens for an event or a session and waits on:
// answered or cancelled, never opened by name.
bool event_window(const char* nm) {
    static const char* const names[] = {"query", "passinpu", "prtyjoin", "delivery", "post1", "post2",
                                        "link5", "arealist"};
    for (size_t i = 0; i < sizeof(names) / sizeof(names[0]); ++i) {
        if (strcmp(nm, names[i]) == 0) {
            return true;
        }
    }
    return false;
}

// `prompts`: engine abi 4's close, which refuses the eight event windows.
int32_t name_command(const HuEngineHandle* ref, const char* name, uint8_t op, uint8_t verb, bool windows,
                     bool prompts, char* why, uint32_t size) {
    bool ok = false;
    lock();
    Handle* h = usable(ref, why, size);
    if (h) {
        const int n = lookup(name, windows, why, size);
        if (n >= 0) {
            const char* nm = g_engine.inv.names[n].name;
            if ((op == kOpOpen || op == kOpClose) && is_compass(g_engine, n)) {
                // No window to open or close: close is this handle's hide
                // hold and open drops it, here; nothing queued, no event.
                g_engine.holds.set(*h, n, kHoldHide, op == kOpClose);
                ok = true;
            } else if (op == kOpOpen && (g_engine.holds.want[n] & kWantBlocked)) {
                snprintf(why, size, "%s is blocked", nm);
            } else if (op == kOpOpen && windows && event_window(nm)) {
                snprintf(why, size, "%s opens only for the game's own event; answer or cancel it instead", nm);
            } else if (op == kOpClose && prompts && event_window(nm)) {
                snprintf(why, size, "%s is a prompt the game waits on; use cancel('%s')", nm, nm);
            } else if (op == kOpClose && hide_only(nm)) {
                snprintf(why, size, "%s", kQueryHideOnly);
            } else {
                ok = enqueue(command(op, n, 0, 0, ref, 0, verb), why, size);
            }
        }
    }
    unlock();
    return ok;
}

int32_t __stdcall op_reset(const HuEngineHandle* ref, const char* name, char* why, uint32_t size) {
    return name_command(ref, name, kOpReset, kVerbReset, false, false, why, size);
}

// Engine abi 5's block: the hold, and the game's own close of the window
// when it is open now, queued with it; the prompt windows close() refuses
// are left open. A full queue refuses the block whole.
int32_t __stdcall op_block5(const HuEngineHandle* ref, const char* name, char* why, uint32_t size) {
    bool ok = false;
    lock();
    Handle* h = usable(ref, why, size);
    const int n = h ? window_index(name, why, size) : -1;
    if (n >= 0) {
        const char* nm = g_engine.inv.names[n].name;
        if (hide_only(nm)) {
            snprintf(why, size, "%s", kQueryHideOnly);
        } else if (is_compass(g_engine, n) || event_window(nm) || !open_menu_guarded(n, NULL)
                   || enqueue(command(kOpBlockClose, n, 0, 0, ref, 0, kVerbBlock), why, size)) {
            // The compass has no window to close: its block is the hidden
            // want bit its draw hook reads, and no `blocked` event ever
            // follows, the game never attempts an open of it.
            g_engine.holds.set(*h, n, kHoldBlock, true);
            ok = true;
        }
    }
    unlock();
    return ok;
}

int32_t __stdcall op_open(const HuEngineHandle* ref, const char* name, char* why, uint32_t size) {
    return name_command(ref, name, kOpOpen, kVerbOpen, false, false, why, size);
}

int32_t __stdcall op_close(const HuEngineHandle* ref, const char* name, char* why, uint32_t size) {
    return name_command(ref, name, kOpClose, kVerbClose, false, false, why, size);
}

int32_t __stdcall op_reset3(const HuEngineHandle* ref, const char* name, char* why, uint32_t size) {
    return name_command(ref, name, kOpReset, kVerbReset, true, false, why, size);
}

int32_t __stdcall op_open3(const HuEngineHandle* ref, const char* name, char* why, uint32_t size) {
    return name_command(ref, name, kOpOpen, kVerbOpen, true, false, why, size);
}

int32_t __stdcall op_close3(const HuEngineHandle* ref, const char* name, char* why, uint32_t size) {
    return name_command(ref, name, kOpClose, kVerbClose, true, false, why, size);
}

int32_t __stdcall op_close4(const HuEngineHandle* ref, const char* name, char* why, uint32_t size) {
    return name_command(ref, name, kOpClose, kVerbClose, true, true, why, size);
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
    if (!ctl || !rdptr(ctl, kPassCallback)) {
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
    return ctl && ctl[kAreaLatch] && area_event_mode(ctl[kAreaMode]) && !rdptr(ctl, kCtlMenu) ? ctl : NULL;
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
    if (!ctl || !ctl[kAreaLatch]) {
        refusal(why, size, prefix, "no area choice is pending");
        return NULL;
    }
    if (!area_event_mode(ctl[kAreaMode])) {
        refusal(why, size, prefix, "%s", kAreaNotAsking);
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
// come since the caller read the prompt's id, or with kCmdSession, once
// another invite or post-box session has.
bool current_prompt(const Command& c, char* why, size_t size) {
    if (!(c.flags & kCmdInstance)) {
        return true;
    }
    const int session = (c.flags & kCmdSession) ? session_window(g_engine.inv.names[c.target].name) : -1;
    const uint32_t current = session >= 0 ? session_now(session) : static_cast<uint32_t>(g_engine.instance[c.target]);
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

// answer(name, value[, id]): the reply the player would give in the window,
// each window taking the value of one Lua type. A value of another type, or
// an id that is no whole number, is misuse (-1); a value the window does not
// list is refused (0), and so is an id another open of the window has
// replaced.
int32_t answer_reply(const HuEngineHandle* ref, const char* name, int32_t kind, double number, const char* text,
                     int32_t has_id, double id, bool sessions, char* why, uint32_t size) {
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
        take_id(&c, has_id, id, sessions);
        return queue_checked(ref, c, true, why, size, [](char* w, size_t s) { return party_pending(NULL, w, s); });
    }
    char text_why[kWhyBytes];
    snprintf(text_why, sizeof(text_why),
        "%s takes no answer; answer() replies to query, link5, arealist, passinpu and prtyjoin",
        g_engine.inv.names[n].name);
    return refusal_result(ref, why, size, text_why);
}

int32_t __stdcall op_answer(const HuEngineHandle* ref, const char* name, int32_t kind, double number,
                            const char* text, char* why, uint32_t size) {
    return answer_reply(ref, name, kind, number, text, 0, 0, false, why, size);
}

int32_t __stdcall op_answer4(const HuEngineHandle* ref, const char* name, int32_t kind, double number,
                             const char* text, int32_t has_id, double id, char* why, uint32_t size) {
    return answer_reply(ref, name, kind, number, text, has_id, id, false, why, size);
}

int32_t __stdcall op_answer5(const HuEngineHandle* ref, const char* name, int32_t kind, double number,
                             const char* text, int32_t has_id, double id, char* why, uint32_t size) {
    return answer_reply(ref, name, kind, number, text, has_id, id, true, why, size);
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
// the session for the post box. With `end_box`, engine abi 4's: the
// incoming box ends as its window's own cancel does, where abi 3 refuses
// while a box window is live.
int32_t cancel_reply(const HuEngineHandle* ref, const char* name, int32_t has_id, double id, bool end_box,
                     bool sessions, char* why, uint32_t size) {
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
                    snprintf(w, s, "query cannot be cancelled now: the event allows no cancel");
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
        take_id(&c, has_id, id, sessions);
        return queue_checked(ref, c, true, why, size, [](char* w, size_t s) { return party_pending(NULL, w, s); });
    }
    if (end_box && (strcmp(nm, "post1") == 0 || strcmp(nm, "post2") == 0)) {
        c = command(kOpPostEnd, n, 0, 0, ref, 0, kVerbCancel);
        take_id(&c, has_id, id, sessions);
        return queue_checked(ref, c, true, why, size,
            [=](char* w, size_t s) { return box_cancellable(nm, w, s); });
    }
    if (strcmp(nm, "delivery") == 0 || strcmp(nm, "post1") == 0 || strcmp(nm, "post2") == 0) {
        c = command(kOpPostClose, n, 0, 0, ref, 0, kVerbCancel);
        take_id(&c, has_id, id, sessions);
        return queue_checked(ref, c, true, why, size, [=](char* w, size_t s) { return post_open(nm, NULL, w, s); });
    }
    char text_why[kWhyBytes];
    snprintf(text_why, sizeof(text_why),
        "%s has nothing to cancel; cancel() takes query, passinpu, link5, arealist, prtyjoin, delivery, post1"
        " and post2", nm);
    return refusal_result(ref, why, size, text_why);
}

int32_t __stdcall op_cancel(const HuEngineHandle* ref, const char* name, char* why, uint32_t size) {
    return cancel_reply(ref, name, 0, 0, false, false, why, size);
}

int32_t __stdcall op_cancel4(const HuEngineHandle* ref, const char* name, int32_t has_id, double id, char* why,
                             uint32_t size) {
    return cancel_reply(ref, name, has_id, id, true, false, why, size);
}

int32_t __stdcall op_cancel5(const HuEngineHandle* ref, const char* name, int32_t has_id, double id, char* why,
                             uint32_t size) {
    return cancel_reply(ref, name, has_id, id, true, true, why, size);
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
            } else if (is_compass(g_engine, n) && (two ? (a >= 1 && a <= 0x7FFF && b >= 1 && b <= 0x7FFF) : a >= 1)) {
                // Accepted and ignored: the compass has no rows or frame to
                // size; nothing is queued or remembered.
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
            if (!g_engine.errors.read(static_cast<uint32_t>(name), &r) || r.slot != ref->slot
                || r.gen != ref->generation) {
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
            if (!g_engine.errors.read(static_cast<uint32_t>(name), &r) || r.slot != ref->slot
                || r.gen != ref->generation) {
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

// One event for engine abi 5's poll, as a table on the reply; false for one
// that is not this handle's or names nothing. Under the lock.
bool write_event(ReplyWriter& w, const HuEngineHandle* ref, uint32_t raw) {
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
    if (type == kEvPending) {
        w.table();
        w.set_string("event", "pending");
        w.set_string("what", pending_what(name) == kPendingPost ? "post" : "invite");
        w.set_bool("pending", pending_on(name));
        return true;
    }
    if (type == kEvError) {
        ErrorRecord r;
        if (!g_engine.errors.read(static_cast<uint32_t>(name), &r) || r.slot != ref->slot
            || r.gen != ref->generation) {
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
    if (type == kEvBlocked) {
        w.set_bool("mine", write_holders(w, "by", name, kHoldBlock, ref));
    }
    return true;
}

// Lua-thread only, under the lock.
uint32_t g_poll_raw[kEventCapacity];

// Engine abi 5's poll: every event past the handle's cursor as a list of
// tables, {event, name} and each type's own fields, closed by {event =
// 'resync', dropped} when events were lost; then the count lost. The
// cursor moves only when the reply fits, so a retry with `used` bytes
// reads the same events.
void __stdcall op_poll5(const HuEngineHandle* ref, HuEngineReply* reply) {
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
        if (write_event(w, ref, g_poll_raw[i])) {
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
    int16_t items;
    ElementInfo elements[kMaxElements];
    int element_count;
    bool truncated;
    const char* layout;
    MemEntry mem;
    bool mem_ok;
    char owner[48];
    char size_owner[48];
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

// What info() reports of window `n`, read from this thread.
void collect_info(int n, InfoData* d) {
    memset(d, 0, sizeof(*d));
    d->n = n;
    d->want = g_engine.holds.want[n];
    if (is_compass(g_engine, n)) {
        collect_compass(d);
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

void __stdcall op_info(const HuEngineHandle* ref, const char* name, HuEngineReply* reply) {
    ReplyWriter w = {reply};
    const int n = read_target(ref, name, false, w);
    if (n < 0) {
        return;
    }
    InfoData d;
    collect_info(n, &d);

    const NameEntry& ne = g_engine.inv.names[n];
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

// The compass's info: what every window has at the top but the layer, which
// it has none of, its object's words under `detail`.
void write_compass_info(const HuEngineHandle* ref, int n, const InfoData& d, ReplyWriter& w, bool v4) {
    const NameEntry& ne = g_engine.inv.names[n];
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
    if (d.open) {
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
    if (d.menu) {
        set_hex(w, "address", d.menu);
        w.set_number("state", d.state);
        w.set_number("x", d.origin[0]);
        w.set_number("y", d.origin[1]);
        w.set_number("height", d.rect[3] - d.rect[1]);
    }
    w.field("detail");
}

// Engine abi 3's info, and with `v4` abi 4's: what an addon draws by at the
// top, the engine's and the game's own words under `detail`. Abi 4 has
// `blockable` for abi 3's `hide_only`, the handles holding each hide and
// block, `resize` for `sizes`, and `elements_truncated` beside `elements`.
void write_info(const HuEngineHandle* ref, const char* name, HuEngineReply* reply, bool v4) {
    ReplyWriter w = {reply};
    const int n = read_target(ref, name, true, w);
    if (n < 0) {
        return;
    }
    InfoData d;
    collect_info(n, &d);

    if (is_compass(g_engine, n)) {
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
    if (v4) {
        w.set_bool("blockable", !hide_only(ne.name));
        const bool hiding = write_holders(w, "hidden_by", n, kHoldHide, ref);
        const bool blocking = write_holders(w, "blocked_by", n, kHoldBlock, ref);
        w.table();
        w.set_bool("hidden", hiding);
        w.set_bool("blocked", blocking);
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
        if (!is_compass(g_engine, n)) {
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

// A decoded title or option: the plain text under `text_key`, its runs under
// `segments_key` as {text, color, escape}, and its '?' count under
// `undecoded_key`. Shift-JIS, the game's own encoding.
void write_glyph_text(ReplyWriter& w, const GlyphText& t, const char* text_key, const char* segments_key,
        const char* undecoded_key) {
    w.set_string(text_key, t.text);
    w.table();
    for (int i = 0; i < t.run_count; ++i) {
        const GlyphRun& r = t.runs[i];
        char part[sizeof(t.text)];
        memcpy(part, t.text + r.start, r.length);
        part[r.length] = '\0';
        w.table();
        w.set_string("text", part);
        w.set_number("color", r.color);
        w.set_number("escape", r.escape);
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

// What the player could pick in prompt n: query's choice and whether it may
// be cancelled, link5's slots, arealist's rows (engine abi 3 and 4: its
// zones), passinpu's limit, and from engine abi 4 on prtyjoin's invite.
// False, with nil and the reason written, when the prompt has nothing to
// offer.
bool write_options(ReplyWriter& w, int n, int version) {
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
    } else {
        snprintf(why, sizeof(why), v4
            ? "%s has no options; options() reads query, link5, arealist, passinpu and prtyjoin"
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
// window, or from engine abi 5 on for prtyjoin and the post boxes the
// session's id; either read before the prompt is.
uint32_t prompt_id(int n, int version) {
    const int session = version >= 5 ? session_window(g_engine.inv.names[n].name) : -1;
    return session >= 0 ? session_now(session) : static_cast<uint32_t>(g_engine.instance[n]);
}

// As op_options, with abi 4's or 5's shapes and the prompt's `id`, which
// answer and cancel take back. A read the prompt changed during is made
// again, so the id never names a prompt newer than what was read. Abi 5's
// names the prompt in `name`.
void write_prompt(const HuEngineHandle* ref, const char* name, HuEngineReply* reply, int version) {
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

// What the server waits on with no window open: a party invite, and a
// post-box session. Empty when neither is. From abi 4 on the session's state
// word is under `detail`; abi 5's name each the prompt a reply to it takes,
// prtyjoin or the box, and give the session's id.
void write_pending(const HuEngineHandle* ref, HuEngineReply* reply, int version) {
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

// Every open window's frame, {x, y, w, h}, keyed by name.
void __stdcall op_rects5(const HuEngineHandle* ref, HuEngineReply* reply) {
    ReplyWriter w = {reply};
    if (!readable_for(ref, w)) {
        return;
    }
    w.table();
    for (int n = 0; n < g_engine.inv.count; ++n) {
        int16_t r[4];
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
    set_hex(w, "macro_object", g_engine.game.macro_object);
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
    return state == kInstalled ? "installed" : state == kFailed ? "failed" : "idle";
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

// Restores everything the engine changed, waits for the game thread to do
// it, then unhooks. Refuses while any handle in the client is open (kind
// "handles"); a game thread that does not drain in time leaves the engine
// installed ("drain"), and a hooked call still running leaves it pinned
// ("busy"). Past the unpin this image runs only because every caller holds a
// reference on it: its own Lua state, or a forwarder.
int32_t shutdown_locked(char* why, uint32_t why_size, char* kind, uint32_t kind_size) {
    char text[256];
    lock();
    if (g_rt.handles.open > 0) {
        snprintf(text, sizeof(text), "%d hideui handle(s) still open", g_rt.handles.open);
        unlock();
        return refusal_out(why, why_size, text, kind, kind_size, "handles");
    }
    if (g_rt.state == kInstalled) {
        if (!g_engine.commands.push(command(kOpRestoreAll, 0, 0, 0, NULL, 0, kVerbNone))) {
            unlock();
            return refusal_out(why, why_size, "the command queue is full; the engine stays installed",
                kind, kind_size, "drain");
        }
        const uint32_t target = g_engine.commands.pushed();
        unlock();
        const DWORD start = GetTickCount();
        while (static_cast<int32_t>(static_cast<uint32_t>(g_engine.completed) - target) < 0
               && GetTickCount() - start < kDrainWaitMs) {
            Sleep(1);
        }
        lock();
        if (g_rt.handles.open > 0) {
            unlock();
            return refusal_out(why, why_size, "a handle was opened during shutdown", kind, kind_size,
                "handles");
        }
        if (static_cast<int32_t>(static_cast<uint32_t>(g_engine.completed) - target) < 0) {
            unlock();
            return refusal_out(why, why_size,
                "the game thread did not drain in time; the engine stays installed", kind, kind_size, "drain");
        }
        g_rt.state = kIdle;
        unhook();
    } else if (g_rt.pinned_busy && g_rt.daemon) {
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
    // resident between its check and the retire; taking it again is
    // recursive.
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
const size_t kNewestSlot = HU_SLOT(macros);

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

int l_hide(lua_State* L) { return window_verb(L, "hide", HU_SLOT(hide3)); }
int l_unhide(lua_State* L) { return window_verb(L, "unhide", HU_SLOT(unhide3)); }
int l_block(lua_State* L) { return window_verb(L, "block", HU_SLOT(block5)); }
int l_unblock(lua_State* L) { return window_verb(L, "unblock", HU_SLOT(unblock3)); }
int l_open(lua_State* L) { return window_verb(L, "open", HU_SLOT(open3)); }
int l_close(lua_State* L) { return window_verb(L, "close", HU_SLOT(close4)); }
int l_reset_group(lua_State* L) { return window_verb(L, "reset_group", HU_SLOT(reset_group5)); }
int l_move(lua_State* L) { return place_verb(L, "move", HU_SLOT(move5)); }
int l_move_group(lua_State* L) { return place_verb(L, "move_group", HU_SLOT(move_group5)); }

// reset(name[, aspect]).
int l_reset(lua_State* L) {
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
    HandleRef* ref = ref_at(L);
    const char* name = check_string(L, 2);
    double id = 0;
    const int32_t has_id = optional_id(L, 3, &id);
    int pushed = 0;
    const HuEngineApi* api = table_for(L, ref, "cancel", HU_SLOT(cancel5), &pushed);
    if (!api) {
        return pushed;
    }
    char why[kWhyBytes];
    why[0] = '\0';
    return verb_result(L, api->cancel5(&ref->h, name, has_id, id, why, sizeof(why)), why);
}

// A verb of no window.
int handle_verb(lua_State* L, const char* verb, size_t offset) {
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

// answer(name, value[, id]): the resident says which type each window
// takes, and a value of another type raises.
int l_answer(lua_State* L) {
    HandleRef* ref = ref_at(L);
    const char* name = check_string(L, 2);
    double id = 0;
    const int32_t has_id = optional_id(L, 4, &id);
    int32_t kind = HU_ANSWER_OTHER;
    double number = 0;
    const char* text = NULL;
    switch (lua_type(L, 3)) {
    case LUA_TNONE:
    case LUA_TNIL:
        kind = HU_ANSWER_NONE;
        break;
    case LUA_TNUMBER:
        kind = HU_ANSWER_NUMBER;
        number = lua_tonumber(L, 3);
        break;
    case LUA_TSTRING:
        kind = HU_ANSWER_STRING;
        text = lua_tostring(L, 3);
        break;
    case LUA_TBOOLEAN:
        kind = HU_ANSWER_BOOLEAN;
        number = lua_toboolean(L, 3) ? 1 : 0;
        break;
    default:
        break;
    }
    int pushed = 0;
    const HuEngineApi* api = table_for(L, ref, "answer", HU_SLOT(answer5), &pushed);
    if (!api) {
        return pushed;
    }
    char why[kWhyBytes];
    why[0] = '\0';
    return verb_result(L, api->answer5(&ref->h, name, kind, number, text, has_id, id, why, sizeof(why)), why);
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
    release_ref(ref_at(L));
    return push_true(L);
}

int l_gc(lua_State* L) {
    release_ref(static_cast<HandleRef*>(lua_touserdata(L, 1)));
    return 0;
}

// poll() -> the events since the last poll, and how many were lost: the
// resident's reply as it is. Each event is {event, name}; an error also has
// verb and reason, blocked by and mine, cursor row, and pending {event,
// what, pending} names no window. Lost events end the list with {event =
// 'resync', dropped}: what came before it is not the whole story.
int l_poll(lua_State* L) {
    HandleRef* ref = ref_at(L);
    int pushed = 0;
    const HuEngineApi* api = table_for(L, ref, "poll", HU_SLOT(poll5), &pushed);
    if (!api) {
        return pushed;
    }
    const HuEngineHandle* h = &ref->h;
    const HuEngineRead read = api->poll5;
    return read_reply(L, [=](HuEngineReply* out) { read(h, out); });
}

int handle_read(lua_State* L, const char* verb, size_t offset) {
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
int l_options(lua_State* L) { return named_read(L, "options", HU_SLOT(options5)); }
int l_list(lua_State* L) { return handle_read(L, "list", HU_SLOT(list4)); }
int l_opened(lua_State* L) { return handle_read(L, "opened", HU_SLOT(opened)); }
int l_focused(lua_State* L) { return handle_read(L, "focused", HU_SLOT(focused4)); }
int l_remembered(lua_State* L) { return handle_read(L, "remembered", HU_SLOT(positions3)); }
int l_groups(lua_State* L) { return handle_read(L, "groups", HU_SLOT(groups4)); }
int l_layout(lua_State* L) { return handle_read(L, "layout", HU_SLOT(layout)); }
int l_pending(lua_State* L) { return handle_read(L, "pending", HU_SLOT(pending5)); }
int l_rects(lua_State* L) { return handle_read(L, "rects", HU_SLOT(rects5)); }
int l_macros(lua_State* L) { return handle_read(L, "macros", HU_SLOT(macros)); }

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
    lua_pushstring(L, "hideui " HU_ENGINE_BUILD);
    return 1;
}

// Goes to the resident, wherever it is: it refuses while any copy holds a
// handle, and on success leaves no resident behind.
int l_shutdown(lua_State* L) {
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
    {"close", l_close},
    {"move", l_move},
    {"move_group", l_move_group},
    {"resize", l_resize},
    {"reset", l_reset},
    {"reset_group", l_reset_group},
    {"reset_all", l_reset_all},
    {"answer", l_answer},
    {"cancel", l_cancel},
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
