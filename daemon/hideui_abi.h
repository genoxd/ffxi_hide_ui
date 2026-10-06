// hideui_abi.h - the contract between hideui_daemon.dll, the immortal detour
// service, and any engine image that asks it for hooks.
//
// Both sides compile this one file. Everything in it has plain C shape --
// explicit calling conventions, uint32_t rather than bool, 4-byte packing, no
// virtuals -- because the two sides ship in different addons, are built months
// apart and must still agree byte for byte. Two rules make that hold:
//
//   - HuDaemonApi is append-only. Fields are never reordered or removed, and a
//     reader checks `size` before touching anything past it.
//   - The version test is a range. An engine needs resident.abi_version >= its
//     own minimum, never equality: the resident daemon cannot be replaced while
//     the client runs, so equality would kill every addon not shipping the same
//     release.
//
// Sizes and offsets are asserted below, so an engine and a daemon that disagree
// about this file fail to build rather than fail in a client.

#ifndef HIDEUI_ABI_H_
#define HIDEUI_ABI_H_

#include <windows.h>
#include <stdint.h>
#include <stddef.h>

#if defined(__cplusplus)
#define HU_STATIC_ASSERT(condition, message) static_assert(condition, message)
#elif defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201112L
#define HU_STATIC_ASSERT(condition, message) _Static_assert(condition, message)
#else
#error "hideui_abi.h needs C++11 or C11: its size and offset asserts are the ABI"
#endif

// Bumped only when a field is appended. An engine asks for the oldest daemon it
// can work with; a newer resident daemon satisfies it silently.
#define HU_DAEMON_ABI 1u

// Bumped whenever the daemon binary changes, ABI or not. Two addons can ship
// different daemon builds at one ABI and whoever pins first holds the process
// for the session, so without this the resident build cannot be named.
#define HU_DAEMON_BUILD "1.0.0"

// The wire size of HuDaemonRecord::winner_build. Frozen: it is a fixed field in
// a shared structure, so it can never change without changing the record.
#define HU_BUILD_MAX 64

// Written into the shared record last, after a barrier, so a reader that sees
// it sees a complete record. Any other value -- zero included -- means nobody
// has published, which is what makes a mid-election crash recoverable.
#define HU_MAGIC 0x48554B31u

// The section the record lives in is always created and opened at this fixed
// size, never at sizeof(HuDaemonRecord): Windows documents ERROR_ACCESS_DENIED
// for opening an existing section with a larger requested size, and a fixed
// page means two daemon builds can never disagree about it.
#define HU_MAPPING_BYTES 4096

// The daemon's one export, and the names of the two objects the election runs
// on. Local\ is session-scoped -- a mapping made in one client would be read by
// another -- so both names carry GetCurrentProcessId().
#define HU_DAEMON_ACQUIRE_NAME "hu_daemon_acquire"
#define HU_MAPPING_NAME_FORMAT "Local\\hideui_daemon_v1_%08X"
#define HU_MUTEX_NAME_FORMAT "Local\\hideui_daemon_lock_v1_%08X"
#define HU_NAME_MAX 64

// A detour replaces the first five bytes of the target with a jump, so the
// relocated prologue is at least five bytes. The upper bound sizes the saved
// copy in HuSiteInfo and is ABI.
#define HU_PROLOGUE_MIN 5u
#define HU_PROLOGUE_MAX 32u

// The stub copies the caller's stack arguments with one push per dword, so the
// count is bounded by what fits in a stub page with room to spare.
#define HU_ARG_BYTES_MAX 256u

enum {
    HU_OK = 0,
    HU_E_ARG = -1,        /* a null or malformed argument */
    HU_E_PROLOGUE = -2,   /* prologue_bytes outside HU_PROLOGUE_MIN..HU_PROLOGUE_MAX */
    HU_E_TARGET = -3,     /* target is not committed executable memory */
    HU_E_PATCHED = -4,    /* the target's first bytes are someone else's patch */
    HU_E_MEMORY = -5,     /* an allocation failed; nothing was changed */
    HU_E_BUSY = -6,       /* a handler was still running when the wait ran out */
    HU_E_THREADS = -7,    /* a thread would not leave the prologue; nothing was written */
    HU_E_SITE = -8,       /* not a site id this daemon handed out */
    HU_E_PROTECT = -9     /* VirtualProtect refused the target page */
};

#pragma pack(push, 4)

// What a handler sees. Registers are as pushad stored them at the moment the
// hooked function was entered (pre) or returned (post); esp is the value before
// the pushad. `args` points at the caller's stack arguments, which the stub
// leaves untouched: the original runs on a copy. A pre handler that returns
// nonzero makes the stub return `result` without running the original; a post
// handler may rewrite `result`.
typedef struct HuFrame {
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t eflags;
    uint32_t return_address;
    const uint32_t* args;
    uint32_t result;
    int32_t site;
    void* user;
} HuFrame;

typedef int (__cdecl* HuPreFn)(HuFrame*);
typedef void (__cdecl* HuPostFn)(HuFrame*);

// The caller sets `size` to its own sizeof. `stack_arg_bytes` is how many
// bytes of stack arguments the function takes -- the stub copies that many
// for the original to run on. `callee_pops` is 1 when the function removes
// them itself with `ret N` (stdcall, thiscall) and 0 when the caller does
// (cdecl). The engine guarantees the first `prologue_bytes` of the target are
// whole instructions that mean the same thing anywhere in memory; the daemon
// copies them verbatim and cannot check that.
typedef struct HuSiteDesc {
    uint32_t size;
    const void* target;
    uint32_t prologue_bytes;
    uint32_t stack_arg_bytes;
    uint32_t callee_pops;
} HuSiteDesc;

// `original` holds the target's first prologue_bytes as they were before the
// patch. `displaced` is 1 when the five bytes at the target no longer hold the
// daemon's jump.
typedef struct HuSiteInfo {
    uint32_t size;
    const void* target;
    const void* trampoline;
    uint32_t prologue_bytes;
    uint32_t stack_arg_bytes;
    uint32_t callee_pops;
    uint8_t original[HU_PROLOGUE_MAX];
    uint32_t displaced;
    uint32_t in_flight;
} HuSiteInfo;

// Append-only, never reordered. install returns a site id >= 0, the same id for
// the same target, or a negative error having changed nothing. A site is never
// uninstalled: with no handlers it is a passthrough for the life of the client.
// A call that ran a pre handler runs the post handler of the same registration,
// whatever set_handlers did in between. clear_handlers publishes "no handlers"
// and then waits, up to timeout_ms, for every such call to finish -- the
// original included; HU_E_BUSY means one was still inside and the engine must
// not unload.
// in_flight is one-way if an exception unwinds through the stub: clear_handlers stays busy for the session.
typedef struct HuDaemonApi {
    uint32_t abi_version;
    uint32_t size;
    int32_t (__stdcall* install)(const HuSiteDesc*);
    int32_t (__stdcall* set_handlers)(int32_t site, HuPreFn pre, HuPostFn post, void* user);
    int32_t (__stdcall* clear_handlers)(int32_t site, uint32_t timeout_ms);
    uint32_t (__stdcall* in_flight)(int32_t site);
    int32_t (__stdcall* site_info)(int32_t site, HuSiteInfo* out);
    const char* (__stdcall* build_id)(void);   /* the resident daemon's build */
} HuDaemonApi;

// What the election publishes into the pid-scoped mapping. The api travels by
// value, so a reader never dereferences a pointer into another image. It comes
// last and must stay last, so that appending to it moves nothing an older
// reader depends on.
typedef struct HuDaemonRecord {
    uint32_t magic;
    uint32_t record_size;
    uint32_t abi_version;
    char winner_path[MAX_PATH];
    char winner_build[HU_BUILD_MAX];
    HuDaemonApi api;
} HuDaemonRecord;

#pragma pack(pop)

// Called with LoadLibraryW's module, never from DllMain: the election takes a
// lock and may LoadLibrary, which under the loader lock deadlocks. Returns NULL
// when the resident daemon is older than min_abi, or when this image could not
// pin itself.
typedef const HuDaemonApi* (__stdcall* HuDaemonAcquire)(uint32_t min_abi);

HU_STATIC_ASSERT(sizeof(void*) == 4, "hideui's ABI is 32-bit; the sizes below assume it");

HU_STATIC_ASSERT(sizeof(HuFrame) == 56, "HuFrame size is ABI");
HU_STATIC_ASSERT(offsetof(HuFrame, edi) == 0, "HuFrame layout is ABI");
HU_STATIC_ASSERT(offsetof(HuFrame, eax) == 28, "HuFrame layout is ABI");
HU_STATIC_ASSERT(offsetof(HuFrame, eflags) == 32, "HuFrame layout is ABI");
HU_STATIC_ASSERT(offsetof(HuFrame, return_address) == 36, "HuFrame layout is ABI");
HU_STATIC_ASSERT(offsetof(HuFrame, args) == 40, "HuFrame layout is ABI");
HU_STATIC_ASSERT(offsetof(HuFrame, result) == 44, "HuFrame layout is ABI");
HU_STATIC_ASSERT(offsetof(HuFrame, site) == 48, "HuFrame layout is ABI");
HU_STATIC_ASSERT(offsetof(HuFrame, user) == 52, "HuFrame layout is ABI");

HU_STATIC_ASSERT(sizeof(HuSiteDesc) == 20, "HuSiteDesc size is ABI");
HU_STATIC_ASSERT(sizeof(HuSiteInfo) == 64, "HuSiteInfo size is ABI");
HU_STATIC_ASSERT(offsetof(HuSiteInfo, original) == 24, "HuSiteInfo layout is ABI");
HU_STATIC_ASSERT(offsetof(HuSiteInfo, displaced) == 56, "HuSiteInfo layout is ABI");

HU_STATIC_ASSERT(sizeof(HuDaemonApi) == 32, "HuDaemonApi size is ABI");
HU_STATIC_ASSERT(offsetof(HuDaemonApi, abi_version) == 0, "HuDaemonApi layout is ABI");
HU_STATIC_ASSERT(offsetof(HuDaemonApi, size) == 4, "HuDaemonApi layout is ABI");
HU_STATIC_ASSERT(offsetof(HuDaemonApi, install) == 8, "HuDaemonApi layout is ABI");
HU_STATIC_ASSERT(offsetof(HuDaemonApi, set_handlers) == 12, "HuDaemonApi layout is ABI");
HU_STATIC_ASSERT(offsetof(HuDaemonApi, clear_handlers) == 16, "HuDaemonApi layout is ABI");
HU_STATIC_ASSERT(offsetof(HuDaemonApi, in_flight) == 20, "HuDaemonApi layout is ABI");
HU_STATIC_ASSERT(offsetof(HuDaemonApi, site_info) == 24, "HuDaemonApi layout is ABI");
HU_STATIC_ASSERT(offsetof(HuDaemonApi, build_id) == 28, "HuDaemonApi layout is ABI");

HU_STATIC_ASSERT(sizeof(HuDaemonRecord) == 368, "HuDaemonRecord size is ABI");
HU_STATIC_ASSERT(offsetof(HuDaemonRecord, winner_path) == 12, "HuDaemonRecord layout is ABI");
HU_STATIC_ASSERT(offsetof(HuDaemonRecord, winner_build) == 272, "HuDaemonRecord layout is ABI");
HU_STATIC_ASSERT(offsetof(HuDaemonRecord, api) == 336, "the api comes last so appending to it moves nothing");
HU_STATIC_ASSERT(sizeof(HU_DAEMON_BUILD) <= HU_BUILD_MAX, "the build string must fit the record");
HU_STATIC_ASSERT(MAX_PATH == 260, "HuDaemonRecord::winner_path is a fixed 260 bytes on the wire");
HU_STATIC_ASSERT(sizeof(HuDaemonRecord) <= HU_MAPPING_BYTES, "the record must fit the fixed section");

// %08X becomes eight characters; sizeof() already counts the four it replaces
// and the terminator, so this is an upper bound on the formatted name.
HU_STATIC_ASSERT(sizeof(HU_MAPPING_NAME_FORMAT) + 8 <= HU_NAME_MAX, "the mapping name must fit for every pid");
HU_STATIC_ASSERT(sizeof(HU_MUTEX_NAME_FORMAT) + 8 <= HU_NAME_MAX, "the mutex name must fit for every pid");

#endif  // HIDEUI_ABI_H_
