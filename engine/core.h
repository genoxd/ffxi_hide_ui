// core.h - the parts of the hideui engine that read no game memory: the menu
// inventory, name and key lookup, hold accounting, the session memory of
// positions and sizes, the two queues between the Lua thread and the game
// thread, the errors the game thread reports to a handle, the signature
// scanner, the glyph decoder and the reply writer. The engine and the offline
// test compile this one file.
//
// Every object here is plain data with no constructor or destructor: the
// engine keeps them at namespace scope, where anything non-trivial would run
// at image load or process exit.

#ifndef HIDEUI_CORE_H_
#define HIDEUI_CORE_H_

#include <windows.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef HU_HEAPCHECK
#include <stdarg.h>
#endif

#include "hideui_engine_abi.h"

namespace hu {

// ---------------------------------------------------------------------------
// Breadcrumbs, in a build made with -DHU_BREADCRUMBS alone: each thread's
// steps through the engine, entered and not yet left, and its last marks,
// in the exported hideui_crumbs, so a frozen client's memory names the step
// each thread is in. A mark is the shared counter's value then; a step's
// name is a string in this image. Lua bindings never nest but through a
// __gc, so a binding entered from Lua drops what a Lua error's longjmp left
// open.

#ifdef HU_BREADCRUMBS
const int kCrumbThreads = 8;
const int kCrumbDepth = 12;
const int kCrumbTrail = 64;

struct CrumbMark {
    volatile LONG count;
    const char* volatile step;
    volatile LONG detail;
    volatile LONG left;         // 1: the step was left at this mark
};

struct CrumbThread {
    volatile LONG thread;       // GetCurrentThreadId(); 0 for a free slot
    volatile LONG depth;        // steps entered and not left
    volatile LONG last;         // the counter at this thread's last mark
    volatile LONG marks;
    const void* stack_base;     // the thread's stack, from its TEB
    const void* stack_limit;
    CrumbMark open[kCrumbDepth];    // the steps entered and not left, outermost first
    volatile LONG trail_at;         // marks written to `trail`, the last at (trail_at - 1) % kCrumbTrail
    CrumbMark trail[kCrumbTrail];
};

struct Crumbs {
    char magic[16];
    volatile LONG counter;
    int32_t thread_bytes;       // sizeof(CrumbThread), for a reader
    CrumbThread thread[kCrumbThreads];
};
#endif

// ---------------------------------------------------------------------------
// Heap checks, in a build made with -DHU_HEAPCHECK and -DHU_BREADCRUMBS:
// every heap of the process validated around the engine's game calls and
// once a second on the game thread, the first failure kept in the exported
// hideui_heapcheck with the last check that passed before it and every
// thread's crumbs as the failure found them. The process keeps the labels
// and the failure in a section of its own that outlives this image, so the
// next image names an earlier one's failure, or its last check that passed.

#ifdef HU_HEAPCHECK
#ifndef HU_BREADCRUMBS
#error "-DHU_HEAPCHECK needs -DHU_BREADCRUMBS"
#endif
const size_t kHeapLabel = 128;
const DWORD kHeapMax = 512;             // heaps a check walks
const DWORD kHeapBackgroundMs = 1000;
const char kHeapRecordName[] = "Local\\hideui_heapcheck_v1_%08X";
const uint32_t kHeapRecordMagic = 0x48554850u;
const int32_t kEveryHandle = -2;        // an ErrorRecord's slot that every handle reads

// The process's section: images number themselves from it, 1 the first.
struct HeapRecord {
    uint32_t magic;
    volatile LONG images;
    volatile LONG failed;
    LONG fail_image;
    LONG fail_tick;
    LONG good_image;
    LONG good_tick;
    char fail[kHeapLabel];
    char good[kHeapLabel];
};

// read_crumbs.py reads the fields up to `snapshot` by offset; `magic` is
// written at the first check. `failed` 2: an earlier image's check failed,
// and this image runs none.
struct HeapCheck {
    char magic[16];
    volatile LONG failed;
    volatile LONG checks;
    volatile LONG busy;                 // a check runs
    LONG heaps;                         // the last check's; above kHeapMax, none were walked
    LONG last_us;
    LONG max_us;
    LONG total_us;
    LONG image;
    LONG good_image;                    // the last check that passed: its image, GetTickCount(), crumb counter
    LONG good_tick;
    LONG good_counter;
    LONG fail_image;
    LONG fail_tick;
    LONG fail_counter;
    LONG fail_thread;
    LONG fail_heap;                     // GetProcessHeaps()'s index, 0 the process heap
    void* fail_handle;
    LONG command_tick;
    const char* outside_step;           // the game thread's last step outside the drain
    LONG outside_left;
    LONG outside_tick;
    LONG background_tick;
    char good[kHeapLabel];
    char fail[kHeapLabel];
    char command[48];                   // the last command the drain ran: its op and target
    CrumbThread snapshot[kCrumbThreads];

    LONG frame_end;                     // the crumb counter as the drain last ended
    LONG drain_thread;
    volatile LONG report_due;           // the drain posts `report` once GetTickCount() reaches report_at
    LONG report_at;
    char report[256];
    HeapRecord* record;
    LONGLONG frequency;
};

static_assert(offsetof(HeapCheck, good) == 104 && offsetof(HeapCheck, snapshot) == 408,
    "read_crumbs.py reads HeapCheck by these offsets");
#endif

}  // namespace hu

#ifdef HU_BREADCRUMBS
extern "C" __declspec(dllexport) hu::Crumbs hideui_crumbs;
#endif
#ifdef HU_HEAPCHECK
extern "C" __declspec(dllexport) hu::HeapCheck hideui_heapcheck;
#endif

namespace hu {

#ifdef HU_BREADCRUMBS
// This thread's slot, claimed on its first step; NULL once every slot is
// taken.
inline CrumbThread* crumb_thread() {
    const LONG id = static_cast<LONG>(GetCurrentThreadId());
    for (int i = 0; i < kCrumbThreads; ++i) {
        if (hideui_crumbs.thread[i].thread == id) {
            return &hideui_crumbs.thread[i];
        }
    }
    for (int i = 0; i < kCrumbThreads; ++i) {
        CrumbThread& t = hideui_crumbs.thread[i];
        if (InterlockedCompareExchange(&t.thread, id, 0) == 0) {
            const NT_TIB* tib;
            __asm__("movl %%fs:0x18, %0" : "=r"(tib));
            t.stack_base = tib->StackBase;
            t.stack_limit = tib->StackLimit;
            return &t;
        }
    }
    return NULL;
}

// `trail`: the mark goes on the thread's trail too; the draw and input
// hooks, run hundreds of times a frame, leave it to the other steps.
inline LONG crumb_mark(CrumbThread* t, const char* step, LONG detail, LONG left, bool trail) {
    const LONG count = InterlockedIncrement(&hideui_crumbs.counter);
    if (trail) {
        CrumbMark& m = t->trail[static_cast<uint32_t>(t->trail_at) % kCrumbTrail];
        m.count = count;
        m.step = step;
        m.detail = detail;
        m.left = left;
        ++t->trail_at;
    }
    ++t->marks;
    t->last = count;
    return count;
}

struct CrumbScope {
    CrumbThread* t;
    const char* step;
    LONG at;
    bool trail;

    // `top`: a Lua binding entered from Lua.
    CrumbScope(const char* name, LONG detail = 0, bool top = false, bool on_trail = true)
        : t(crumb_thread()), step(name), at(0), trail(on_trail) {
        if (!t) {
            return;
        }
        if (top) {
            t->depth = 0;
        }
        at = t->depth;
        if (at < kCrumbDepth) {
            t->open[at].step = name;
            t->open[at].detail = detail;
            t->open[at].left = 0;
        }
        t->depth = at + 1;
        const LONG count = crumb_mark(t, name, detail, 0, trail);
        if (at < kCrumbDepth) {
            t->open[at].count = count;
        }
    }

    ~CrumbScope() {
        if (t) {
            t->depth = at;
            crumb_mark(t, step, 0, 1, trail);
        }
    }
};

#define HU_CRUMB_JOIN2(a, b) a##b
#define HU_CRUMB_JOIN(a, b) HU_CRUMB_JOIN2(a, b)
#define HU_STEP(name) ::hu::CrumbScope HU_CRUMB_JOIN(hu_crumb_, __LINE__)(name)
#define HU_STEP_AT(name, detail) ::hu::CrumbScope HU_CRUMB_JOIN(hu_crumb_, __LINE__)(name, static_cast<LONG>(detail))
#define HU_LUA_STEP(name) ::hu::CrumbScope HU_CRUMB_JOIN(hu_crumb_, __LINE__)(name, 0, true)
#define HU_HOT_STEP(name) ::hu::CrumbScope HU_CRUMB_JOIN(hu_crumb_, __LINE__)(name, 0, false, false)
#else
#define HU_STEP(name) ((void)0)
#define HU_STEP_AT(name, detail) ((void)0)
#define HU_LUA_STEP(name) ((void)0)
#define HU_HOT_STEP(name) ((void)0)
#endif

#ifdef HU_HEAPCHECK
// The failed check's report: its image when an earlier one's, then the last
// check that passed, its image when another, and how long before.
inline void heap_describe(HeapCheck& h) {
    char lead[48];
    if (h.fail_image == h.image) {
        snprintf(lead, sizeof(lead), "heap check failed: ");
    } else {
        snprintf(lead, sizeof(lead), "heap check failed in engine image %ld: ", static_cast<long>(h.fail_image));
    }
    char good[kHeapLabel + 48];
    const unsigned long before = static_cast<DWORD>(h.fail_tick - h.good_tick);
    if (!h.good[0]) {
        snprintf(good, sizeof(good), "none");
    } else if (h.good_image != h.fail_image) {
        snprintf(good, sizeof(good), "%s, engine image %ld, %lu ms before", h.good, static_cast<long>(h.good_image),
            before);
    } else {
        snprintf(good, sizeof(good), "%s, %lu ms before", h.good, before);
    }
    snprintf(h.report, sizeof(h.report), "%s%s (last good: %s)", lead, h.fail, good);
}

// The magic, this image's number and the process's section, at its first
// check. The section stays mapped for the life of the process, as the
// election's does. An earlier image's failure becomes this one's (failed 2),
// reported by the drain a second on, once the handle new() is making can
// read it.
inline void heap_open(HeapCheck& h) {
    memcpy(h.magic, "hideui heap 1", 14);
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    h.frequency = frequency.QuadPart > 0 ? frequency.QuadPart : 1;
    h.image = 1;
    char name[64];
    snprintf(name, sizeof(name), kHeapRecordName, static_cast<unsigned>(GetCurrentProcessId()));
    const HANDLE mapping = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(HeapRecord),
        name);
    HeapRecord* r = mapping
        ? static_cast<HeapRecord*>(MapViewOfFile(mapping, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(HeapRecord))) : NULL;
    if (!r) {
        if (mapping) {
            CloseHandle(mapping);
        }
        return;
    }
    h.record = r;
    r->magic = kHeapRecordMagic;
    h.image = InterlockedIncrement(&r->images);
    if (r->good[0]) {
        memcpy(h.good, r->good, kHeapLabel);
        h.good[kHeapLabel - 1] = '\0';
        h.good_image = r->good_image;
        h.good_tick = r->good_tick;
    }
    if (r->failed) {
        memcpy(h.fail, r->fail, kHeapLabel);
        h.fail[kHeapLabel - 1] = '\0';
        h.fail_image = r->fail_image;
        h.fail_tick = r->fail_tick;
        heap_describe(h);
        h.report_at = static_cast<LONG>(GetTickCount() + kHeapBackgroundMs);
        InterlockedExchange(&h.report_due, 1);
        h.failed = 2;
    }
}

// Every heap of the process validated, `context` following the label in
// parentheses on a failure. HeapValidate takes each heap's own lock, so a
// check waits out an allocation in progress; a check on another thread is
// spun out. The first failure is reported by the drain: at once when it
// came on the drain's thread, a second on from any other.
inline void heap_run(const char* label, const char* context) {
    HeapCheck& h = hideui_heapcheck;
    while (InterlockedCompareExchange(&h.busy, 1, 0) != 0) {
        SwitchToThread();
    }
    if (!h.image) {
        heap_open(h);
    }
    if (!h.failed) {
        LARGE_INTEGER start;
        QueryPerformanceCounter(&start);
        HANDLE heaps[kHeapMax];
        const DWORD count = GetProcessHeaps(kHeapMax, heaps);
        DWORD bad = count;
        for (DWORD i = 0; count <= kHeapMax && i < count; ++i) {
            if (!HeapValidate(heaps[i], 0, NULL)) {
                bad = i;
                break;
            }
        }
        LARGE_INTEGER end;
        QueryPerformanceCounter(&end);
        const LONG us = static_cast<LONG>((end.QuadPart - start.QuadPart) * 1000000 / h.frequency);
        h.heaps = static_cast<LONG>(count);
        h.last_us = us;
        h.max_us = us > h.max_us ? us : h.max_us;
        h.total_us += us;
        InterlockedIncrement(&h.checks);
        const DWORD now = GetTickCount();
        const LONG counter = hideui_crumbs.counter;
        HeapRecord* r = h.record;
        if (bad == count) {
            snprintf(h.good, sizeof(h.good), "%s", label);
            h.good_image = h.image;
            h.good_tick = static_cast<LONG>(now);
            h.good_counter = counter;
            if (r) {
                memcpy(r->good, h.good, kHeapLabel);
                r->good_image = h.image;
                r->good_tick = h.good_tick;
            }
        } else {
            if (context) {
                snprintf(h.fail, sizeof(h.fail), "%s (%s)", label, context);
            } else {
                snprintf(h.fail, sizeof(h.fail), "%s", label);
            }
            h.fail_image = h.image;
            h.fail_tick = static_cast<LONG>(now);
            h.fail_counter = counter;
            h.fail_thread = static_cast<LONG>(GetCurrentThreadId());
            h.fail_heap = static_cast<LONG>(bad);
            h.fail_handle = heaps[bad];
            memcpy(h.snapshot, hideui_crumbs.thread, sizeof(h.snapshot));
            heap_describe(h);
            if (r) {
                memcpy(r->fail, h.fail, kHeapLabel);
                r->fail_image = h.image;
                r->fail_tick = h.fail_tick;
                InterlockedExchange(&r->failed, 1);
            }
            const bool drain = h.fail_thread == h.drain_thread;
            h.report_at = static_cast<LONG>(now + (drain ? 0 : kHeapBackgroundMs));
            InterlockedExchange(&h.report_due, 1);
            h.failed = 1;
        }
    }
    InterlockedExchange(&h.busy, 0);
}

// A check named by a printf format. None runs once one has failed: the heap
// stays corrupt, and the first failure is the one that says where.
inline void heap_check(const char* format, ...) {
    if (hideui_heapcheck.failed) {
        return;
    }
    char label[kHeapLabel];
    va_list args;
    va_start(args, format);
    vsnprintf(label, sizeof(label), format, args);
    va_end(args);
    label[sizeof(label) - 1] = '\0';
    heap_run(label, NULL);
}

#define HU_HEAP_CHECK(...) ::hu::heap_check(__VA_ARGS__)
#else
#define HU_HEAP_CHECK(...) ((void)0)
#endif

// ---------------------------------------------------------------------------
// The inventory the live registry is verified against.

struct RowSpec {
    const char* type;
    const char* name;
    uint8_t layer;
    uint32_t policy;
    int16_t avail;      // -1: not known
    int32_t overlap;    // -1: not known
};

const RowSpec kRowSpecs[] = {
#include "menu_table.inc"
};

const int kRowCount = static_cast<int>(sizeof(kRowSpecs) / sizeof(kRowSpecs[0]));
const size_t kKeyLen = 16;
const int kMaxNames = 384;
// A registry name is at most 8 characters; the names on no row run longer.
const size_t kNameMax = 11;

const size_t kRowStride = 0x2C;
const size_t kRowSlot = 0x20;
const size_t kRowPolicy = 0x24;
const size_t kRowAvail = 0x28;
const size_t kRowLayer = 0x29;
const size_t kRowOverlap = 0x2A;

// The registry's dock bits. 0x10000000 is set at runtime by iteminfo alone
// and is never cleared or restored by the engine.
const uint32_t kDockMask = 0x1000 | 0x2000 | 0x4000;

inline char lower_ascii(char c) {
    return (c >= 'A' && c <= 'Z') ? static_cast<char>(c - 'A' + 'a') : c;
}

struct NameEntry {
    char name[kNameMax + 1];
    char key[kKeyLen];      // the row's type + name, space-padded: the registry's own form
    char alias[kKeyLen];    // an alias row's key, all 0 for none
    int16_t rows[2];
    uint8_t row_count;

    // The key a live menu's resource record holds names this window.
    bool keyed(const void* k) const {
        return memcmp(key, k, kKeyLen) == 0 || (alias[0] && memcmp(alias, k, kKeyLen) == 0);
    }
};

// Registry names that are another window under a second name, folded into
// it: the game's open and close of mcr1edit and mcr2edit rewrite the name to
// mcr1edlo and mcr2edlo under one interface layout, one controller each, and
// a Japanese client opens the job list as jobcsel where every other opens
// jobcselu. Every verb takes either name, and the events, reads and lists
// carry the first.
struct AliasSpec {
    const char* name;
    const char* alias;
};

const AliasSpec kAliases[] = {
    {"mcr1edit", "mcr1edlo"},
    {"mcr2edit", "mcr2edlo"},
    {"jobcselu", "jobcsel"},
};
const int kAliasCount = static_cast<int>(sizeof(kAliases) / sizeof(kAliases[0]));

inline const char* alias_of(const char* nm) {
    for (int i = 0; i < kAliasCount; ++i) {
        if (strcmp(kAliases[i].alias, nm) == 0) {
            return kAliases[i].name;
        }
    }
    return NULL;
}

struct GroupSpec {
    const char* name;
    uint32_t mask;
    const char* anchor;
    int carries;            // the group a move of this one takes along, or -1
};

// The chat log carries the party list: the game moves partywin with the log
// only when the log's own edge changes, so a SetPosition of the log leaves it
// behind.
const GroupSpec kGroups[] = {
    {"chat_log", 0x1000, "logwindo", 1},
    {"party_list", 0x2000, "partywin", -1},
    {"target_window", 0x4000, "targetwi", -1},
};
const int kGroupCount = 3;

// Names that may be hidden but never blocked or closed, by any engine abi's
// slots, each with the reason a refusal gives. The game reads query's and
// comyn's windows right after their opens with no null check, and frees
// gmtell's controller on a refused open before writing into it; while the
// KO window (dead) is down the game declines every raise offered, and a
// refused open of it or of Monstrosity's (mp_dead) acts on the choice an
// earlier KO left in its controller; a refused open of the raise question
// (the resyn family) keeps it from the player, and the game asks it again
// every frame; an NPC dialogue box (rem4line, rem4li2) with no window has
// nothing advance its dialogue, its timed pages and the script's own advance
// included, and the NPC's script waits on it forever; the outgoing box gives
// up when its name entry (stringdl) cannot open, which ends the delivery
// session. cancel('query') closes query once the answer is written, as the
// event's wait does on its next tick.
struct HideOnlySpec {
    const char* name;
    const char* reason;
};

const HideOnlySpec kHideOnly[] = {
    {"query", "query cannot be blocked or closed; answer it with answer('query', value) or cancel('query')"},
    {"gmtell", "gmtell cannot be blocked or closed: the game writes a GM's question into it as it opens, so a"
               " refused open crashes the client, and a close leaves the question unanswered; hide it instead"},
    {"comyn", "comyn cannot be blocked or closed: the game reads the window right after asking some of its"
              " questions, so a refused open crashes the client, and a close drops the question unanswered; hide it"
              " instead"},
    {"dead", "dead cannot be blocked or closed: closing the KO window during a raise offer declines the raise, and"
             " a refused open declines it too and can send the player to their home point at once; hide it"
             " instead"},
    {"mp_dead", "mp_dead cannot be blocked or closed: a refused open repeats the choice of the previous KO at once,"
                " and a close leaves the player no way to choose; hide it instead"},
    {"resyn", "resyn cannot be blocked or closed: blocked, the raise question is never shown, so the player cannot"
              " accept a raise, and the game asks again every frame; hide it instead"},
    {"resynus", "resynus cannot be blocked or closed: blocked, the raise question is never shown, so the player"
                " cannot accept a raise, and the game asks again every frame; hide it instead"},
    {"resynde", "resynde cannot be blocked or closed: blocked, the raise question is never shown, so the player"
                " cannot accept a raise, and the game asks again every frame; hide it instead"},
    {"resynfr", "resynfr cannot be blocked or closed: blocked, the raise question is never shown, so the player"
                " cannot accept a raise, and the game asks again every frame; hide it instead"},
    {"rem4line", "rem4line cannot be blocked or closed: without its window nothing advances the dialogue, not even"
                 " its timed pages or the script's own advance, and the NPC's script waits forever; hide it instead,"
                 " and answer('rem4line') or cancel('rem4line')"},
    {"rem4li2", "rem4li2 cannot be blocked or closed: without its window nothing advances the dialogue, not even"
                " its timed pages or the script's own advance, and the NPC's script waits forever; hide it instead,"
                " and answer('rem4li2') or cancel('rem4li2')"},
    {"stringdl", "stringdl cannot be blocked or closed: the outgoing box gives up when its name entry cannot open,"
                 " which ends the delivery session; hide it instead, and answer('stringdl') or cancel('stringdl')"},
};
const int kHideOnlyCount = static_cast<int>(sizeof(kHideOnly) / sizeof(kHideOnly[0]));

inline const char* hide_only_reason(const char* name) {
    for (int i = 0; i < kHideOnlyCount; ++i) {
        if (strcmp(kHideOnly[i].name, name) == 0) {
            return kHideOnly[i].reason;
        }
    }
    return NULL;
}

inline bool hide_only(const char* name) {
    return hide_only_reason(name) != NULL;
}

// Names no engine abi's close slots close, beyond the hide-only ones, each
// with the reason a refusal gives: the game's error screen waits on ok's
// acknowledgment, which a closed ok never gives. A block still holds them.
const HideOnlySpec kCloseRefused[] = {
    {"ok", "ok cannot be closed: the game waits until it is acknowledged, and closed, it waits forever;"
           " answer('ok') acknowledges it"},
};
const int kCloseRefusedCount = static_cast<int>(sizeof(kCloseRefused) / sizeof(kCloseRefused[0]));

// Why a close of the name is refused in every slot, or NULL.
inline const char* close_refusal(const char* name) {
    const char* reason = hide_only_reason(name);
    for (int i = 0; !reason && i < kCloseRefusedCount; ++i) {
        if (strcmp(kCloseRefused[i].name, name) == 0) {
            reason = kCloseRefused[i].reason;
        }
    }
    return reason;
}

// Names open refuses, each from the engine abi whose open slots first
// refuse it, with the reason a refusal gives. From abi 1, every slot: an
// open by name alone can crash the client, or the game opens the window
// only for a message from the server or an NPC's script, and opened alone
// it sends the server a request made of nothing, closes on its first
// frame, or cannot be dismissed. A window "has no controller" when its
// registry slot holds none until the game's own opener makes one: the
// open binds the new instance to that controller with no test for NULL.
// From abi 9: it is a set's main window, which opens only with its set, or
// it opens empty or with what an earlier open of the game's left in it.
// The windows kRoutes names open through their routes instead.
struct OpenRefusedSpec {
    const char* name;
    uint32_t since;
    const char* reason;
};

const OpenRefusedSpec kOpenRefused[] = {
    // crash: what the window reads, only its own opener sets
    {"inventor", 1, "inventor cannot be opened alone: it reads a list only the game's own openers set, and opened"
                    " without one it can crash the client; open_set('equipment') opens it with the equipment"},
    {"bank", 1, "bank cannot be opened alone: its open reads a list only the game's own opener sets, and can crash"
                " the client; the game opens it at the Mog House"},
    {"itmstora", 1, "itmstora cannot be opened alone: its open reads the Mog Safe's list, which is not there until"
                    " the Mog Safe has been used, and can crash the client; the game opens it from the Mog Safe"},
    {"shop", 1, "shop cannot be opened alone: its open reads a list only the game's own opener sets, and can crash"
                " the client; the game opens it at a shop's NPC"},
    {"bluinven", 1, "bluinven cannot be opened alone: its open reads a list only the game's own opener sets, and can"
                    " crash the client; open_set('set_spells'), open_set('attachments') and open_set('tabula') open it"
                    " with theirs"},
    {"bluequip", 1, "bluequip cannot be opened alone: its first frame writes rows by a count only the game's own"
                    " opener sets, past the end of its buffer, and can crash the client; open_set('set_spells') opens"
                    " it with its set"},
    {"cmbhlst", 1, "cmbhlst cannot be opened alone: it reads a list only the game's own opener sets, which can be"
                   " freed by then, and can crash the client; the game opens it from the synthesis menu"},
    {"mp_invnt", 1, "mp_invnt cannot be opened alone: it reads a list only the game's own opener sets, which can be"
                    " freed by then, and can crash the client; the game opens it from the Monstrosity menu"},
    {"holdtime", 1, "holdtime cannot be opened alone: its first frame calls the confirm only a yes/no question's"
                    " hold sets, and can crash the client; the game opens it for a question confirmed by holding"},
    {"mcrledit", 1, "mcrledit cannot be opened alone: its confirm acts on the macro line only the macro editor"
                    " sets, and can crash the client; the game opens it from the macro editor"},
    {"mcresed", 1, "mcresed cannot be opened alone: it reads the equipment set only its own opener sets up, and can"
                   " crash the client; the game opens it from the equipment set list"},
    {"loot", 1, "loot cannot be opened alone: it reads lnowin's controller, which only the game's own opener hands"
                " it, and can crash the client; the game opens it for the treasure pool"},
    {"lootope", 1, "lootope cannot be opened alone: its close reads what only loot's own open sets, and can crash"
                   " the client; the game opens it from loot"},
    {"prvdungp", 1, "prvdungp cannot be opened alone: its draw reads entries only the game's own opener sets, and"
                    " can crash the client; open_set('tabula', index) opens it for the Maze Tabula at that index"},
    {"conquer1", 1, "conquer1 cannot be opened alone: its draw reads by an index only the game's own opener sets,"
                    " and can crash the client; the game opens it over the conquest map"},
    {"beseige2", 1, "beseige2 cannot be opened alone: its draw reads by an index only the game's own opener sets,"
                    " and can crash the client; the game opens it over the Besieged map"},
    {"campsan", 1, "campsan cannot be opened alone: its draw reads by an index only the game's own opener sets, and"
                   " can crash the client; the game opens it over the Campaign map"},
    {"camparea", 1, "camparea cannot be opened alone: its draw reads by an index only the game's own opener sets,"
                    " and can crash the client; the game opens it over the Campaign map"},
    {"auc4", 1, "auc4 cannot be opened alone: its open reads the row selected in auclist, and with none it crashes"
                " the client; the game opens it from the auction house's list"},
    {"faqmain", 1, "faqmain cannot be opened alone: its draw reads a document only the game's own opener builds, and"
                   " can crash the client; the game opens it from the help menu"},
    {"textcol1", 1, "textcol1 cannot be opened alone: its open reads a table by a category only the game's own"
                    " opener sets, and can crash the client; open_set('text_colors', category) opens it with its"
                    " set"},
    {"textcol3", 1, "textcol3 cannot be opened alone: it reads a color only the game's own opener sets, and the"
                    " player's OK on it would crash the client; the game opens it from the configuration menu"},
    {"conf11s", 1, "conf11s cannot be opened alone: its open reads a table by an index only the game's own opener"
                   " sets, and can crash the client; the game opens it from the configuration menu"},
    {"rmlo1", 1, "rmlo1 cannot be opened alone: it reads the Mog House's layout, which is there only while the game"
                 " lays out the room, and can crash the client"},
    {"rmlo2", 1, "rmlo2 cannot be opened alone: its open reads the Mog House's layout, which is there only while the"
                 " game lays out the room, and can crash the client"},
    {"rmlo4", 1, "rmlo4 cannot be opened alone: its open reads the Mog House's layout, which is there only while the"
                 " game lays out the room, and can crash the client"},
    {"plnt1", 1, "plnt1 cannot be opened alone: it has no pot's record, and the player's Enter on it would crash the"
                 " client; the game opens it at a flowerpot"},
    {"plnt2", 1, "plnt2 cannot be opened alone: it has no pot's record, and the player's Enter on it would crash the"
                 " client; the game opens it at a flowerpot"},
    {"plnt3", 1, "plnt3 cannot be opened alone: it has no pot's record, and the player's Enter on it would crash the"
                 " client; the game opens it at a flowerpot"},
    {"plnt4", 1, "plnt4 cannot be opened alone: it has no pot's record, and the player's Enter on it would crash the"
                 " client; the game opens it at a flowerpot"},
    {"plnt5", 1, "plnt5 cannot be opened alone: it has no pot's record, and the player's Enter on it would crash the"
                 " client; the game opens it at a flowerpot"},
    {"plnt6", 1, "plnt6 cannot be opened alone: it has no pot's record, and the player's Enter on it would crash the"
                 " client; the game opens it at a flowerpot"},
    // crash: no controller until the game's own opener makes one
    {"netwait", 1, "netwait cannot be opened alone: it has no controller until the game's own opener makes one,"
                   " and the open would crash the client; the game opens it while it waits on the server"},
    {"netbar", 1, "netbar cannot be opened alone: it has no controller until the game's own opener makes one, and"
                  " the open would crash the client; the game opens it while it waits on the server"},
    {"keypad2", 1, "keypad2 cannot be opened alone: it has no controller but on a Japanese client, and the open"
                   " would crash the client"},
    {"keyselec", 1, "keyselec cannot be opened alone: it has no controller but on a Japanese client, and the open"
                    " would crash the client"},
    {"keypad3", 1, "keypad3 cannot be opened alone: it has no controller but on a Japanese client, and the open"
                   " would crash the client"},
    {"roomlist", 1, "roomlist cannot be opened alone: it has no controller outside the Mog House, and the open would"
                    " crash the client; the game opens it in the Mog House"},
    {"mogdoor", 1, "mogdoor cannot be opened alone: it has no controller until the game's own opener makes one, and"
                   " the open would crash the client; the game opens it at the Mog House's door"},
    {"bankmenu", 1, "bankmenu cannot be opened alone: it has no controller until the game's own opener makes one,"
                    " and the open would crash the client; the game opens it for its Mog Safe question"},
    // the server's or an NPC's: a request made of nothing, a close on the
    // first frame, or a window that cannot be dismissed
    {"chocobor", 1, "chocobor cannot be opened alone: it would ask the server for a race made of nothing; the game"
                    " opens it for an NPC's race"},
    {"racecard", 1, "racecard cannot be opened alone: its open asks the server for a race made of nothing; the game"
                    " opens it for an NPC's race"},
    {"mes1rcv", 1, "mes1rcv cannot be opened alone: its actions would send requests for a message made of nothing;"
                   " the game opens it for a message read from msglist"},
    {"mes2frnd", 1, "mes2frnd cannot be opened alone: its actions would send requests for a message made of nothing;"
                    " the game opens it for a message read from msglist"},
    {"auc1", 1, "auc1 cannot be opened alone: it opens for the server's answer at an auction counter, and alone it"
                " closes on its first frame; the game opens it at the auction house"},
    {"auclist", 1, "auclist cannot be opened alone: it opens for the server's answer to a search, and alone it"
                   " opens iteminfo on every frame; the game opens it at the auction house"},
    {"auchisto", 1, "auchisto cannot be opened alone: it opens for the server's price history, and alone it opens"
                    " iteminfo on every frame; the game opens it at the auction house"},
    {"ranking", 1, "ranking cannot be opened alone: it opens for an event's ranking, which only the event's script"
                   " gives it; the game opens it at the event's NPC"},
    {"shopmain", 1, "shopmain cannot be opened alone: it opens for a shop's message from the server, and alone it"
                    " closes on its first frame; the game opens it at a shop's NPC"},
    {"guildsho", 1, "guildsho cannot be opened alone: it opens for a guild shop's message from the server, and alone"
                    " it closes on its first frame; the game opens it at a guild's NPC"},
    {"ok", 1, "ok cannot be opened alone: it opens with no notice in it and cannot be closed until the player"
              " leaves the area; the game opens it for its own notices"},
    {"mp_dead", 1, "mp_dead cannot be opened alone: it opens for a Monstrosity KO, and opened without one it acts on"
                   " what an earlier KO left in it; the game opens it when the monster is KO'd"},
    // abi 9: a set's main window, or it opens empty or stale
    {"equip", 9, "equip opens only with its set: use open_set('equipment')"},
    {"map0", 9, "map0 opens only with its set: use open_set('map')"},
    {"automato", 9, "automato opens only with its set: use open_set('attachments')"},
    {"mp_mncst", 9, "mp_mncst opens only with its set: use open_set('instincts')"},
    {"tskill1", 9, "tskill1 opens only with its set: use open_set('synthesis', index)"},
    {"mcr1edit", 9, "mcr1edit opens only with its set: use open_set('macros')"},
    {"mcr2edit", 9, "mcr2edit opens only with its set: use open_set('macros')"},
    {"mp_stat", 9, "mp_stat opens only with its set: use open_set('status')"},
    {"cnqframe", 9, "cnqframe opens only with its set: use open_set('region', kind)"},
    {"mapframe", 9, "mapframe cannot be opened alone: it opens empty; it is the map's frame, which"
                    " open_set('map') opens with the map"},
    {"evitem", 9, "evitem cannot be opened alone: it opens with what the game left in it; the game opens it from"
                  " the key items menu"},
    {"emote", 9, "emote cannot be opened alone: it opens empty; the game opens it from the emote menu"},
};
const int kOpenRefusedCount = static_cast<int>(sizeof(kOpenRefused) / sizeof(kOpenRefused[0]));

// A window every close slot refuses while another window, `other`, is open:
// the other reads it through its menu with no test for it, and the client
// crashes.
struct PairSpec {
    const char* name;
    const char* other;
    const char* reason;
};

// The windows that act on another window's selection or state: each opens
// only while one of its parents is open, as the game opens it from there,
// and every open slot refuses it otherwise with "<name> needs <parents>
// open" and, where opening it alone would crash the client, why.
struct ChildSpec {
    const char* name;
    const char* parents[9];
    const char* why;
};

const ChildSpec kChildren[] = {
    {"mcrselop", {"mcrselec", NULL}, "its open reads mcrselec's window, and with mcrselec closed it would crash the"
                                     " client"},
    {"mcresmn", {"mcres20", NULL}, "its open reads mcres20's window, and with mcres20 closed it would crash the"
                                   " client"},
    {"gift", {"trade", NULL}, "its input moves trade's cursor through trade's window, and with trade closed the"
                              " player's keys on it would crash the client"},
    {"auc2", {"auc1", NULL}, "its rows read the auction house's categories, which are there only then, and the"
                             " player's pick on it would crash the client"},
    {"aucweapo", {"auc1", NULL}, "its rows read the auction house's categories, which are there only then, and the"
                                 " player's pick on it would crash the client"},
    {"aucammo", {"auc1", NULL}, "its rows read the auction house's categories, which are there only then, and the"
                                " player's pick on it would crash the client"},
    {"aucarmor", {"auc1", NULL}, "its rows read the auction house's categories, which are there only then, and the"
                                 " player's pick on it would crash the client"},
    {"aucmagic", {"auc1", NULL}, "its rows read the auction house's categories, which are there only then, and the"
                                 " player's pick on it would crash the client"},
    {"aucmater", {"auc1", NULL}, "its rows read the auction house's categories, which are there only then, and the"
                                 " player's pick on it would crash the client"},
    {"aucfood", {"auc1", NULL}, "its rows read the auction house's categories, which are there only then, and the"
                                " player's pick on it would crash the client"},
    {"aucmeals", {"auc1", NULL}, "its rows read the auction house's categories, which are there only then, and the"
                                 " player's pick on it would crash the client"},
    {"aucitem", {"auc1", NULL}, "its rows read the auction house's categories, which are there only then, and the"
                                " player's pick on it would crash the client"},
    {"auc3", {"auc1", NULL}, "it reads the auction house's list, which is there only then"},
    {"wepsort", {"auc1", NULL}, "its rows read the auction house's categories, which are there only then, and the"
                                " player's pick on it would crash the client"},
    {"armsort", {"auc1", NULL}, "its rows read the auction house's categories, which are there only then, and the"
                                " player's pick on it would crash the client"},
    {"itmsortw", {"inventor", "bank", NULL}, "its rows act on the list of the one it opens over, and the player's"
                                             " pick on it alone can crash the client"},
    {"itmsort2", {"inventor", "bank", NULL}, "its rows act on the list of the one it opens over, and the player's"
                                             " pick on it alone can crash the client"},
    // a submenu or a panel of the selection in its parent
    {"link1", {"link5", NULL}, NULL},
    {"link2", {"link5", NULL}, NULL},
    {"link3", {"link5", NULL}, NULL},
    {"link4", {"link5", NULL}, NULL},
    {"link13", {"link5", NULL}, NULL},
    {"lscswin", {"link5", NULL}, NULL},
    {"link7", {"link12", NULL}, NULL},
    {"link8", {"link12", NULL}, NULL},
    {"link9", {"link12", NULL}, NULL},
    {"link10", {"link12", NULL}, NULL},
    {"flmes", {"friend", NULL}, NULL},
    {"blkmain", {"blklist", NULL}, NULL},
    {"merit3", {"meritcat", NULL}, NULL},
    {"meritinf", {"meritcat", NULL}, NULL},
    {"levmeri2", {"merit2ca", NULL}, NULL},
    {"guide01", {"guide00", NULL}, NULL},
    {"scoption", {"scresult", NULL}, NULL},
    {"acoption", {"scresult", NULL}, NULL},
    {"bluehelp", {"bluequip", "automato", "prvdungp", "meritcat", "merit2ca", "fp_cat", "fp_cat2", "mp_mncst",
                  NULL}, NULL},
    {"bluepoin", {"bluequip", NULL}, NULL},
    {"abisortw", {"ability", "emote", "mount", NULL}, NULL},
    {"mgcsortw", {"magic", NULL}, NULL},
    {"blusortw", {"magic", NULL}, NULL},
    {"subwindo", {"chatctrl", "ability", "mount", "magic", NULL}, NULL},
    {"iuse", {"inventor", NULL}, NULL},
    {"lnowin", {"loot", NULL}, NULL},
    {"mcrbedit", {"mcr1edit", "mcr2edit", NULL}, NULL},
    {"mapv3", {"map0", NULL}, NULL},
    {"mapv2", {"mapv3", NULL}, NULL},
    {"scanlist", {"map0", NULL}, NULL},
    {"mapscan", {"scanlist", NULL}, NULL},
    {"textcol2", {"textcol1", NULL}, NULL},
    {"mcrmogc", {"mcresed", NULL}, NULL},
    {"cmbhwin", {"cmbhlst", NULL}, NULL},
    {"mnstorag", {"equip", NULL}, NULL},
    {"mp_cstm", {"mp_mncst", NULL}, NULL},
    {"mp_mnnm", {"mp_invnt", NULL}, NULL},
    {"mp_mntp2", {"mp_invnt", NULL}, NULL},
    {"mp_kncst", {"mp_invnt", NULL}, NULL},
    {"quest01", {"quest00", NULL}, NULL},
    {"scsibori", {"scresult", NULL}, NULL},
    {"sccompar", {"comgenre", "scresult", NULL}, NULL},
    {"sccomite", {"comgenre", "scresult", NULL}, NULL},
    {"sccomls", {"comgenre", "scresult", NULL}, NULL},
    {"coloresu", {"cnqframe", NULL}, NULL},
    {"itemxinf", {"iteminfo", NULL}, NULL},
};
const int kChildCount = static_cast<int>(sizeof(kChildren) / sizeof(kChildren[0]));
const int kChildDepth = 4;      // deeper than the longest chain, map0, mapv3, mapv2

inline const ChildSpec* child_spec(const char* name) {
    for (int i = 0; i < kChildCount; ++i) {
        if (strcmp(kChildren[i].name, name) == 0) {
            return &kChildren[i];
        }
    }
    return NULL;
}

// The kChildren pairs whose parent's close, as close() runs it, closes the
// child itself: its begin-close (vtable +0x08) by name or by instance, or
// its Escape where close() runs that (map0, the palettes, textcol1,
// mp_mncst, cnqframe). ability closes abisortw and subwindo only while it
// shows its sort window and its pane; close() closes the rest of a closed
// parent's children itself, each before the parent where no other parent of
// it is open.
struct ParentCloses {
    const char* parent;
    const char* children[13];
};

const ParentCloses kParentCloses[] = {
    {"auc1", {"auc2", "aucweapo", "aucammo", "aucarmor", "aucmagic", "aucmater", "aucfood", "aucmeals", "aucitem",
              "auc3", "wepsort", "armsort", NULL}},
    {"inventor", {"itmsortw", "itmsort2", "iuse", NULL}},
    {"link5", {"link1", "link2", "link3", "link4", "link13", "lscswin", NULL}},
    {"link12", {"link7", "link8", "link9", "link10", NULL}},
    {"friend", {"flmes", NULL}},
    {"blklist", {"blkmain", NULL}},
    {"meritcat", {"meritinf", NULL}},
    {"merit2ca", {"levmeri2", NULL}},
    {"scresult", {"scoption", "acoption", NULL}},
    {"bluequip", {"bluepoin", NULL}},
    {"ability", {"abisortw", "subwindo", NULL}},
    {"emote", {"abisortw", NULL}},
    {"mount", {"abisortw", "subwindo", NULL}},
    {"magic", {"mgcsortw", "blusortw", "subwindo", NULL}},
    {"chatctrl", {"subwindo", NULL}},
    {"scanlist", {"mapscan", NULL}},
    {"map0", {"mapv3", "scanlist", NULL}},
    {"mcr1edit", {"mcrbedit", NULL}},
    {"mcr2edit", {"mcrbedit", NULL}},
    {"textcol1", {"textcol2", NULL}},
    {"mp_mncst", {"mp_cstm", NULL}},
    {"cnqframe", {"coloresu", NULL}},
};
const int kParentClosesCount = static_cast<int>(sizeof(kParentCloses) / sizeof(kParentCloses[0]));

inline bool parent_closes(const char* parent, const char* child) {
    for (int i = 0; i < kParentClosesCount; ++i) {
        if (strcmp(kParentCloses[i].parent, parent) != 0) {
            continue;
        }
        for (int k = 0; kParentCloses[i].children[k]; ++k) {
            if (strcmp(kParentCloses[i].children[k], child) == 0) {
                return true;
            }
        }
    }
    return false;
}

inline bool child_of(const ChildSpec& c, const char* parent) {
    for (int i = 0; c.parents[i]; ++i) {
        if (strcmp(c.parents[i], parent) == 0) {
            return true;
        }
    }
    return false;
}

// "<name> needs <parents> open", and why where opening it alone would
// crash the client.
inline void child_reason(const ChildSpec& c, char* out, size_t size) {
    size_t used = static_cast<size_t>(snprintf(out, size, "%s needs ", c.name));
    for (int i = 0; c.parents[i] && used < size; ++i) {
        const char* between = i == 0 ? "" : c.parents[i + 1] ? ", " : " or ";
        used += static_cast<size_t>(snprintf(out + used, size - used, "%s%s", between, c.parents[i]));
    }
    if (used < size) {
        snprintf(out + used, size - used, " open%s%s", c.why ? ": " : "", c.why ? c.why : "");
    }
}

const PairSpec kCloseWhileOpen[] = {
    {"gift", "trade", "gift cannot be closed while trade is open: trade's input moves gift's cursor through gift's"
                      " window, and with gift closed the player's keys on trade would crash the client;"
                      " cancel('trade') ends the trade"},
};
const int kCloseWhileOpenCount = static_cast<int>(sizeof(kCloseWhileOpen) / sizeof(kCloseWhileOpen[0]));

inline const PairSpec* pair_spec(const PairSpec* table, int count, const char* name) {
    for (int i = 0; i < count; ++i) {
        if (strcmp(table[i].name, name) == 0) {
            return &table[i];
        }
    }
    return NULL;
}

// Names no block slot blocks, beyond the hide-only ones, each with the
// reason a refusal gives: trade's input moves gift's cursor through gift's
// window, and a blocked gift does not open with the next trade.
const HideOnlySpec kBlockRefused[] = {
    {"gift", "gift cannot be blocked: it would not open with the next trade, whose input moves gift's cursor"
             " through gift's window, and the player's keys on trade would crash the client; hide it instead"},
};
const int kBlockRefusedCount = static_cast<int>(sizeof(kBlockRefused) / sizeof(kBlockRefused[0]));

// Why a block of the name is refused in every slot, or NULL.
inline const char* block_refusal(const char* name) {
    const char* reason = hide_only_reason(name);
    for (int i = 0; !reason && i < kBlockRefusedCount; ++i) {
        if (strcmp(kBlockRefused[i].name, name) == 0) {
            reason = kBlockRefused[i].reason;
        }
    }
    return reason;
}

// Why an open slot of engine abi `abi` refuses the name, or NULL.
inline const char* open_refusal(const char* name, uint32_t abi) {
    for (int i = 0; i < kOpenRefusedCount; ++i) {
        if (kOpenRefused[i].since <= abi && strcmp(kOpenRefused[i].name, name) == 0) {
            return kOpenRefused[i].reason;
        }
    }
    return NULL;
}

// The windows the game opens with the open routine's last argument 1: its
// reopen of the registry rows of policy bit 0x400 passes it, and so do the
// target window's and the action menus' own openers. With it a window opens
// dormant while another window of its overlap group shows, and comes out
// when that one goes. Every other window's every opener passes 0, and an
// open by name passes what the game passes.
const char* const kOpenParked[] = {"gaugewin", "targetwi", "playermo", "mp_pmode"};

inline int32_t open_overlap(const char* name) {
    for (size_t i = 0; i < sizeof(kOpenParked) / sizeof(kOpenParked[0]); ++i) {
        if (strcmp(kOpenParked[i], name) == 0) {
            return 1;
        }
    }
    return 0;
}

// The lists the ability window and the magic window show, by the category
// their openers take: the one name each has in every verb and event. The
// game checks no category; one past a window's table reads and writes past
// its controller's arrays. Ability's 0, 5 and 7 name no list.
struct ListName {
    int32_t category;
    const char* name;
};

const ListName kAbilityLists[] = {
    {1, "job_abilities"}, {2, "pet_commands"}, {3, "weapon_skills"}, {4, "job_traits"},
    {6, "blood_pact_rage"}, {8, "phantom_roll"}, {9, "quick_draw"}, {10, "blood_pact_ward"},
    {11, "sambas"}, {12, "waltzes"}, {13, "steps"}, {14, "flourishes_i"}, {15, "stratagems"},
    {16, "jigs"}, {17, "flourishes_ii"}, {18, "ready"}, {19, "flourishes_iii"}, {20, "monstrosity"},
    {21, "rune_enchantment"}, {22, "ward"}, {23, "effusion"},
};
const int kAbilityListCount = static_cast<int>(sizeof(kAbilityLists) / sizeof(kAbilityLists[0]));
const int32_t kAbilityListMax = 23;

const ListName kMagicLists[] = {
    {1, "white_magic"}, {2, "black_magic"}, {3, "summoning"}, {4, "ninjutsu"},
    {5, "songs"}, {6, "blue_magic"}, {7, "geomancy"}, {8, "trust"},
};
const int kMagicListCount = static_cast<int>(sizeof(kMagicLists) / sizeof(kMagicLists[0]));
const int32_t kMagicListMax = 8;
const int32_t kBlueMagic = 6;   // the list whose sort window is blusortw, not mgcsortw

inline const char* list_name(const ListName* lists, int count, int32_t category) {
    for (int i = 0; i < count; ++i) {
        if (lists[i].category == category) {
            return lists[i].name;
        }
    }
    return NULL;
}

// Whether `name` is `low`, which is already lowercase, in any case: the
// game's own names keep their capitals.
inline bool same_lowercase(const char* name, const char* low) {
    for (; *name && *low; ++name, ++low) {
        if (lower_ascii(*name) != *low) {
            return false;
        }
    }
    return *name == *low;
}

// A list by its name or its number in digits, already lowercase: the
// category, or -1 for a name or number no list of the table has.
inline int32_t list_category(const ListName* lists, int count, const char* text) {
    if (!text || !text[0]) {
        return -1;
    }
    int32_t value = 0;
    bool digits = true;
    for (const char* d = text; *d && digits; ++d) {
        digits = *d >= '0' && *d <= '9' && value < 1000;
        value = value * 10 + (*d - '0');
    }
    for (int i = 0; i < count; ++i) {
        if (digits ? lists[i].category == value : same_lowercase(lists[i].name, text)) {
            return lists[i].category;
        }
    }
    return -1;
}

inline const char* ability_list_name(int32_t category) {
    return list_name(kAbilityLists, kAbilityListCount, category);
}

// An open's or a set's argument as hideui.lua hands it to the engine: a
// word (a name, or a whole number in digits, a leading '-' allowed), or a
// table's fields as "key=value;key=value", each value a word. Names are
// compared lowercase.
const int kArgFields = 4;
const size_t kArgText = 32;     // the game's longest choice name is 24

struct OpenArg {
    int count;                  // words given: 0 none, 1 for a word
    bool table;
    char key[kArgFields][kArgText];
    char value[kArgFields][kArgText];
};

// `text` (NULL or "" for none) into *out; false when it is no word and no
// table of words.
inline bool parse_open_arg(const char* text, OpenArg* out) {
    memset(out, 0, sizeof(*out));
    if (!text || !text[0]) {
        return true;
    }
    out->table = strchr(text, '=') != NULL;
    const char* at = text;
    while (*at) {
        if (out->count >= kArgFields) {
            return false;
        }
        const char* end = strchr(at, ';');
        const size_t length = end ? static_cast<size_t>(end - at) : strlen(at);
        const char* equals = static_cast<const char*>(memchr(at, '=', length));
        if (out->table != (equals != NULL)) {
            return false;
        }
        const char* word = equals ? equals + 1 : at;
        const size_t key_length = equals ? static_cast<size_t>(equals - at) : 0;
        const size_t word_length = length - (equals ? key_length + 1 : 0);
        if ((out->table && (key_length == 0 || key_length >= kArgText)) || word_length == 0
            || word_length >= kArgText) {
            return false;
        }
        for (size_t i = 0; i < key_length; ++i) {
            out->key[out->count][i] = lower_ascii(at[i]);
        }
        for (size_t i = 0; i < word_length; ++i) {
            out->value[out->count][i] = lower_ascii(word[i]);
        }
        ++out->count;
        at += length;
        if (*at == ';') {
            ++at;
            if (!*at || !out->table) {
                return false;
            }
        }
    }
    return out->table || out->count == 1;
}

// A table field's word, or NULL.
inline const char* arg_field(const OpenArg& a, const char* key) {
    for (int i = 0; a.table && i < a.count; ++i) {
        if (strcmp(a.key[i], key) == 0) {
            return a.value[i];
        }
    }
    return NULL;
}

// Whether the table's fields are exactly `keys` (NULL-ended), in any order.
inline bool arg_fields_are(const OpenArg& a, const char* const* keys) {
    int n = 0;
    for (; keys[n]; ++n) {
        if (!arg_field(a, keys[n])) {
            return false;
        }
    }
    return a.table && a.count == n;
}

// A whole number in digits, a leading '-' allowed, into *out.
inline bool arg_number(const char* text, int32_t* out) {
    const bool negative = text && text[0] == '-';
    const char* d = text ? text + (negative ? 1 : 0) : NULL;
    if (!d || !*d) {
        return false;
    }
    int64_t value = 0;
    for (; *d; ++d) {
        if (*d < '0' || *d > '9' || value > 0x7FFFFFFF) {
            return false;
        }
        value = value * 10 + (*d - '0');
    }
    if (value > 0x7FFFFFFF) {
        return false;
    }
    *out = static_cast<int32_t>(negative ? -value : value);
    return true;
}

const int32_t kTimerTicksPerSecond = 60;     // the timers count down in 1/60 seconds
const int32_t kTimerTicksMax = 0x7FFFFFFF;  // their openers take the count as an int32

// Seconds in decimal, a fraction and an exponent allowed, as the count of
// 1/60 seconds nearest them, into *ticks; false for anything else or a count
// outside 0..kTimerTicksMax.
inline bool arg_ticks(const char* text, int32_t* ticks) {
    if (!text || !*text) {
        return false;
    }
    const char* d = text;
    const bool negative = *d == '-';
    if (*d == '-' || *d == '+') {
        ++d;
    }
    double value = 0;
    int digits = 0;
    for (; *d >= '0' && *d <= '9'; ++d, ++digits) {
        value = value * 10 + (*d - '0');
    }
    if (*d == '.') {
        double place = 0.1;
        for (++d; *d >= '0' && *d <= '9'; ++d, ++digits, place /= 10) {
            value += (*d - '0') * place;
        }
    }
    if (!digits) {
        return false;
    }
    if (*d == 'e' || *d == 'E') {
        ++d;
        const bool down = *d == '-';
        if (*d == '-' || *d == '+') {
            ++d;
        }
        int exponent = 0;
        int exponent_digits = 0;
        for (; *d >= '0' && *d <= '9' && exponent < 400; ++d, ++exponent_digits) {
            exponent = exponent * 10 + (*d - '0');
        }
        if (!exponent_digits) {
            return false;
        }
        for (int i = 0; i < exponent; ++i) {
            value = down ? value / 10 : value * 10;
        }
    }
    if (*d) {
        return false;
    }
    const double count = (negative ? -value : value) * kTimerTicksPerSecond + 0.5;
    if (!(count >= 0) || count >= static_cast<double>(kTimerTicksMax) + 1) {
        return false;
    }
    *ticks = static_cast<int32_t>(count);
    return true;
}

// "'a', 'b' or 'c'" from a choice table, into `out`.
inline void choice_names(const ListName* choices, int count, char* out, size_t size) {
    size_t used = 0;
    out[0] = '\0';
    for (int i = 0; i < count && used < size; ++i) {
        const char* between = i == 0 ? "" : i + 1 == count ? " or " : ", ";
        const bool number = choices[i].name[strspn(choices[i].name, "0123456789")] == '\0';
        used += static_cast<size_t>(
            snprintf(out + used, size - used, number ? "%s%s" : "%s'%s'", between, choices[i].name));
    }
}

// The choices the argument windows and sets take, each by its name or its
// number in the table. Each table's numbers count from 1, as the player
// sees the rows, but the bags', which are Windower's bag ids, and the jobs',
// which are Windower's job ids.
const ListName kMogpostModes[] = {{1, "mog_house"}, {2, "nomad"}};
const ListName kJobChangeRows[] = {{1, "main"}, {2, "support"}};
const ListName kJobs[] = {
    {1, "war"}, {2, "mnk"}, {3, "whm"}, {4, "blm"}, {5, "rdm"}, {6, "thf"}, {7, "pld"}, {8, "drk"},
    {9, "bst"}, {10, "brd"}, {11, "rng"}, {12, "sam"}, {13, "nin"}, {14, "drg"}, {15, "smn"}, {16, "blu"},
    {17, "cor"}, {18, "pup"}, {19, "dnc"}, {20, "sch"}, {21, "geo"}, {22, "run"},
};
// quest00's modes 2 and 3 by the titles the game gives them (its message
// category 12, 0x18B and 0x18C); mode 1 has no title in the game's text.
// quest01's lists go by number: lists 1 and 3 both read "Current".
const ListName kQuestModes[] = {{1, "1"}, {2, "Unity Information"}, {3, "Ranking Information"}};
const ListName kQuestLists[] = {{1, "1"}, {2, "2"}, {3, "3"}, {4, "4"}, {5, "5"}, {6, "6"}, {7, "7"}};
// The mission logs by the names the game's mission list takes from its
// message category 13 into its table of them (FFXiMain.dll+0x22D7A0).
const ListName kMissionLogs[] = {
    {1, "San d'Oria"}, {2, "Bastok"}, {3, "Windurst"}, {4, "Rise of the Zilart"}, {5, "Chains of Promathia"},
    {6, "Assault"}, {7, "Treasures of Aht Urhgan"}, {8, "Campaign"}, {9, "Wings of the Goddess"},
    {10, "Seekers of Adoulin"}, {11, "Rhapsodies of Vana'diel"}, {12, "The Voracious Resurgence"},
    {13, "A Crystalline Prophecy"}, {14, "A Moogle Kupo d'Etat"}, {15, "A Shantotto Ascension"},
};
const int32_t kMissionNation = 16;      // no argument: the player's nation's log, as the Missions row opens it
const ListName kSearchModes[] = {{1, "new"}, {2, "refine"}};
const ListName kComgenreModes[] = {{1, "comment"}, {2, "search"}, {3, "refine"}};
const ListName kCommentPurposes[] = {{1, "comment"}, {2, "refine"}};
const ListName kCastStyles[] = {{1, "bar"}, {2, "countdown"}};
const ListName kPalettes[] = {{1, "ctrl"}, {2, "alt"}};
const ListName kBags[] = {
    {0, "inventory"}, {1, "safe"}, {2, "storage"}, {3, "temporary"}, {4, "locker"}, {5, "satchel"},
    {6, "sack"}, {7, "case"}, {8, "wardrobe"}, {9, "safe2"}, {10, "wardrobe2"}, {11, "wardrobe3"},
    {12, "wardrobe4"}, {13, "wardrobe5"}, {14, "wardrobe6"}, {15, "wardrobe7"}, {16, "wardrobe8"},
    {17, "recycle"},
};
// The Region Info map's kinds by the titles its setup gives each points
// window (FFXiMain.dll+0x1E7DC8: message category 13, 0x8E, 0x8C, 0x8F, 0xA2).
const ListName kRegionKinds[] = {{1, "Conquest"}, {2, "Besieged"}, {3, "Campaign"}, {4, "Colonization"}};
#define HU_CHOICES(table) table, static_cast<int>(sizeof(table) / sizeof(table[0]))

const int kItemSlotMax = 80;            // a bag's last record; the game.h route checks the bag's size
const int32_t kItemIdMax = 0xFFFE;      // 0xFFFF is the gil's
const int32_t kCastKindBar = 0;         // the spell's bar, as casting shows it
const int32_t kCastKindCountdown = 4;   // the seconds left, as an NPC's event shows them
const int32_t kCommentModeComment = 0;  // comgenre's and its sub-lists' modes: the player's own comment
const int32_t kCommentModeRefine = 2;   // a narrowing of the search on screen
const int32_t kCommentBaseStep = 16;    // a comment category's base in the sub-lists: category x 16

// How the windows that take an argument open: each through the game's own
// opener of it, or what the game's own route writes before its open. A
// plain open of one with a default runs its route with it in every open
// slot; one with none (kArgRequired) is refused in every slot. A choice
// reaches the route as its number in its table.
enum {
    kRouteStorage = 1,  // the storage list's opener; x 0 the Mog House's, 1 the Nomad Moogle's
    kRouteMogpost,      // the post menu's opener with the mode
    kRouteFriend,       // the friend list's opener, titlehan held off
    kRouteMsglist,      // the mail list's opener with (0, 0, 0), titlehan held off
    kRouteBlklist,      // the blacklist's opener
    kRouteMount,        // the mount list's opener as the ability list runs it, subwindo held off
    kRouteJobChange,    // the row (1 main, 2 support) written, then the open by the language's key
    kRouteConf11l,      // the set written, then the open
    kRouteJobPoints,    // the job: the game's test, then the opener
    kRouteFpCat2,       // the category: the opener
    kRouteQuest00,      // the mode written, then the open
    kRouteQuest01,      // the list, of quest00's mode: quest00's own confirm
    kRouteMiss00,       // the log, or the player's nation's, written, then the open
    kRouteSearch,       // new: the search reset first; refine: refused with no search run
    kRouteComgenre,     // the mode: search as kRouteSearch's new, refine as its refine, then the opener
    kRouteCommentSub,   // value the mode, y the category: the opener with the category's base
    kRouteScsibori,     // refused with no search run
    kRouteItemPanel,    // x the bag, -1 for an id alone; y the slot; value the id
    kRouteTimer,        // value the count in 1/60 seconds, x the kind
    kRoutePersona,      // the equipment layout written, then the open
    kRouteMacro,        // x the palette, value the row from 0: the palette switch when it is the other one,
                        // then the pick
    kRouteScanlist,     // while the map tracks, its Wide Scan row's question on comyn; else the opener
    kRouteColoresu,     // the region written, then the open, colorank closed
};

const uint8_t kArgNone = 0;         // no argument
const uint8_t kArgChoice = 1;       // one of `choices`, by name or number
const uint8_t kArgNumber = 2;       // a whole number, low..high
const uint8_t kArgItem = 3;         // {bag = , slot = } or {item = }
const uint8_t kArgTimer = 4;        // seconds, or {seconds = , style = }: the count in 1/60 seconds
const uint8_t kArgComment = 5;      // a purpose, or with a category {purpose = , category = }
const uint8_t kArgMacro = 6;        // {palette = , row = }
const int32_t kArgRequired = -0x7FFFFFFF;

struct RouteSpec {
    const char* name;
    uint8_t route;
    uint8_t takes;
    const ListName* choices;
    int choice_count;
    int32_t low;                // kArgNumber's range, and a choice's
    int32_t high;
    int32_t fallback;           // a plain open's value, or kArgRequired
    const char* what;           // what the argument is, in a refusal
    const char* example;        // an argument in a refusal
};

const RouteSpec kRoutes[] = {
    {"storage", kRouteStorage, kArgNone, NULL, 0, 0, 0, 0, NULL, NULL},
    {"storage2", kRouteStorage, kArgNone, NULL, 0, 0, 0, 1, NULL, NULL},
    {"mogpost", kRouteMogpost, kArgChoice, HU_CHOICES(kMogpostModes), 1, 2, 1, "mode", "'mog_house'"},
    {"friend", kRouteFriend, kArgNone, NULL, 0, 0, 0, 0, NULL, NULL},
    {"msglist", kRouteMsglist, kArgNone, NULL, 0, 0, 0, 0, NULL, NULL},
    {"blklist", kRouteBlklist, kArgNone, NULL, 0, 0, 0, 0, NULL, NULL},
    {"mount", kRouteMount, kArgNone, NULL, 0, 0, 0, 0, NULL, NULL},
    {"jobcselu", kRouteJobChange, kArgChoice, HU_CHOICES(kJobChangeRows), 1, 2, kArgRequired, "job to change",
     "'main'"},
    {"conf11l", kRouteConf11l, kArgNumber, NULL, 0, 1, 2, 1, "set", "1"},
    {"jbpcat", kRouteJobPoints, kArgChoice, HU_CHOICES(kJobs), 1, 22, kArgRequired, "job", "'thf'"},
    {"fp_cat2", kRouteFpCat2, kArgNumber, NULL, 0, 1, 7, kArgRequired, "category", "2"},
    {"quest00", kRouteQuest00, kArgChoice, HU_CHOICES(kQuestModes), 1, 3, 1, "mode", "'Unity Information'"},
    {"quest01", kRouteQuest01, kArgChoice, HU_CHOICES(kQuestLists), 1, 7, 1, "list", "1"},
    {"miss00", kRouteMiss00, kArgChoice, HU_CHOICES(kMissionLogs), 1, 15, kMissionNation, "log",
     "'Rise of the Zilart'"},
    {"searchjo", kRouteSearch, kArgChoice, HU_CHOICES(kSearchModes), 1, 2, 1, "mode", "'refine'"},
    {"searchna", kRouteSearch, kArgChoice, HU_CHOICES(kSearchModes), 1, 2, 1, "mode", "'refine'"},
    {"searchra", kRouteSearch, kArgChoice, HU_CHOICES(kSearchModes), 1, 2, 1, "mode", "'refine'"},
    {"searchle", kRouteSearch, kArgChoice, HU_CHOICES(kSearchModes), 1, 2, 1, "mode", "'refine'"},
    {"scchar", kRouteSearch, kArgChoice, HU_CHOICES(kSearchModes), 1, 2, 1, "mode", "'refine'"},
    {"comgenre", kRouteComgenre, kArgChoice, HU_CHOICES(kComgenreModes), 1, 3, kArgRequired, "mode",
     "'search'"},
    {"sccompar", kRouteCommentSub, kArgComment, HU_CHOICES(kCommentPurposes), 1, 3, kArgRequired,
     "purpose and category", "{purpose = 'comment', category = 2}"},
    {"sccomite", kRouteCommentSub, kArgComment, HU_CHOICES(kCommentPurposes), 4, 4, kArgRequired, "purpose",
     "'comment'"},
    {"sccomls", kRouteCommentSub, kArgComment, HU_CHOICES(kCommentPurposes), 5, 5, kArgRequired, "purpose",
     "'comment'"},
    {"scsibori", kRouteScsibori, kArgNone, NULL, 0, 0, 0, 0, NULL, NULL},
    {"iteminfo", kRouteItemPanel, kArgItem, NULL, 0, 0, 0, kArgRequired, "item",
     "{bag = 'inventory', slot = 5} or {item = 16555}"},
    {"itemxinf", kRouteItemPanel, kArgItem, NULL, 0, 0, 0, kArgRequired, "item",
     "{bag = 'inventory', slot = 5} or {item = 16555}"},
    {"casttime", kRouteTimer, kArgTimer, HU_CHOICES(kCastStyles), 1, kTimerTicksMax, kArgRequired, "length",
     "3 or {seconds = 3, style = 'countdown'}"},
    {"eventtim", kRouteTimer, kArgTimer, NULL, 0, 1, kTimerTicksMax, kArgRequired, "length", "30"},
    {"persona", kRoutePersona, kArgNone, NULL, 0, 0, 0, 0, NULL, NULL},
    {"macro", kRouteMacro, kArgMacro, HU_CHOICES(kPalettes), 1, 10, kArgRequired, "macro",
     "{palette = 'ctrl', row = 3}"},
    {"scanlist", kRouteScanlist, kArgNone, NULL, 0, 0, 0, 0, NULL, NULL},
    {"coloresu", kRouteColoresu, kArgNumber, NULL, 0, 2, 9, kArgRequired, "region", "3"},
};
const int kRouteCount = static_cast<int>(sizeof(kRoutes) / sizeof(kRoutes[0]));

inline const RouteSpec* route_spec(const char* name) {
    for (int i = 0; name && i < kRouteCount; ++i) {
        if (strcmp(kRoutes[i].name, name) == 0) {
            return &kRoutes[i];
        }
    }
    return NULL;
}

// What a route runs with: kRoutes' meaning for each.
struct RouteArgs {
    int16_t x;
    int16_t y;
    uint32_t value;
};

// One choice of `choices` by its name or number: its number, or -1.
inline int32_t arg_choice(const ListName* choices, int count, const char* word) {
    return word ? list_category(choices, count, word) : -1;
}

// {bag = , slot = } or {item = }: the bag (-1 for an id alone), the slot
// and the id; false with `why` filled.
inline bool parse_item_arg(const char* window, const OpenArg& a, RouteArgs* out, char* why, size_t size) {
    static const char* const by_slot[] = {"bag", "slot", NULL};
    static const char* const by_id[] = {"item", NULL};
    int32_t number = 0;
    if (arg_fields_are(a, by_id)) {
        if (!arg_number(arg_field(a, "item"), &number) || number < 1 || number > kItemIdMax) {
            snprintf(why, size, "no such item id for %s: %s (1 to %d)", window, arg_field(a, "item"), kItemIdMax);
            return false;
        }
        out->x = -1;
        out->y = 0;
        out->value = static_cast<uint32_t>(number);
        return true;
    }
    if (!arg_fields_are(a, by_slot)) {
        snprintf(why, size, "%s takes an item as {bag = 'inventory', slot = 5} or {item = 16555}", window);
        return false;
    }
    const int32_t bag = arg_choice(HU_CHOICES(kBags), arg_field(a, "bag"));
    if (bag < 0) {
        char names[320];
        choice_names(HU_CHOICES(kBags), names, sizeof(names));
        snprintf(why, size, "no such bag for %s: %s (%s, or Windower's bag id, 0 to 17)", window,
            arg_field(a, "bag"), names);
        return false;
    }
    if (!arg_number(arg_field(a, "slot"), &number) || number < 1 || number > kItemSlotMax) {
        snprintf(why, size, "no such slot for %s: %s (1 to %d, the slot get_items reports)", window,
            arg_field(a, "slot"), kItemSlotMax);
        return false;
    }
    out->x = static_cast<int16_t>(bag);
    out->y = static_cast<int16_t>(number);
    out->value = 0;
    return true;
}

// The route's values for `spec` from `a`, its default for none; false with
// `why` filled when the argument is not one the window takes, or none is
// given to one that needs it.
inline bool route_args(const RouteSpec& spec, const OpenArg& a, RouteArgs* out, char* why, size_t size) {
    memset(out, 0, sizeof(*out));
    char names[400];
    if (spec.takes == kArgNone) {
        if (a.count) {
            snprintf(why, size, "%s takes no argument", spec.name);
            return false;
        }
        out->x = static_cast<int16_t>(spec.fallback);
        return true;
    }
    if (!a.count) {
        if (spec.fallback == kArgRequired) {
            snprintf(why, size, "%s needs a %s: open('%s', %s)", spec.name, spec.what, spec.name, spec.example);
            return false;
        }
        out->value = static_cast<uint32_t>(spec.fallback);
        return true;
    }
    const char* word = a.table ? NULL : a.value[0];
    int32_t number = 0;
    switch (spec.takes) {
    case kArgChoice: {
        const int32_t choice = arg_choice(spec.choices, spec.choice_count, word);
        if (choice < spec.low || choice > spec.high) {
            choice_names(spec.choices, spec.choice_count, names, sizeof(names));
            snprintf(why, size, "no such %s of %s: %s (%s)", spec.what, spec.name, word ? word : "a table", names);
            return false;
        }
        out->value = static_cast<uint32_t>(choice);
        return true;
    }
    case kArgNumber:
        if (!word || !arg_number(word, &number) || number < spec.low || number > spec.high) {
            snprintf(why, size, "no such %s of %s: %s (%d to %d)", spec.what, spec.name, word ? word : "a table",
                spec.low, spec.high);
            return false;
        }
        out->value = static_cast<uint32_t>(number);
        return true;
    case kArgItem:
        return parse_item_arg(spec.name, a, out, why, size);
    case kArgTimer: {
        const char* seconds = word;
        int32_t style = 1;
        if (a.table) {
            const char* styled = arg_field(a, "style");
            seconds = spec.choices && a.count == (styled ? 2 : 1) ? arg_field(a, "seconds") : NULL;
            style = seconds && styled ? arg_choice(spec.choices, spec.choice_count, styled) : 1;
            if (!seconds || style < 0) {
                snprintf(why, size, "%s takes seconds%s", spec.name, spec.choices
                    ? ", or {seconds = 3, style = 'bar' or 'countdown'}" : "");
                return false;
            }
        }
        if (!arg_ticks(seconds, &number) || number < spec.low || number > spec.high) {
            snprintf(why, size, "no such length of %s: %s (seconds, 1/60 to %d.%02d, in steps of 1/60 as the game"
                " counts)", spec.name, seconds, spec.high / kTimerTicksPerSecond,
                spec.high % kTimerTicksPerSecond * 100 / kTimerTicksPerSecond);
            return false;
        }
        out->value = static_cast<uint32_t>(number);
        out->x = static_cast<int16_t>(style == 2 ? kCastKindCountdown : kCastKindBar);
        return true;
    }
    case kArgComment: {
        static const char* const both[] = {"purpose", "category", NULL};
        const bool categories = spec.low != spec.high;
        const char* purpose = categories ? (arg_fields_are(a, both) ? arg_field(a, "purpose") : NULL) : word;
        const int32_t choice = arg_choice(spec.choices, spec.choice_count, purpose);
        if (choice < 1) {
            choice_names(spec.choices, spec.choice_count, names, sizeof(names));
            if (categories) {
                snprintf(why, size, "%s takes {purpose = %s, category = %d to %d}", spec.name, names, spec.low,
                    spec.high);
            } else {
                snprintf(why, size, "%s takes a purpose: %s", spec.name, names);
            }
            return false;
        }
        number = spec.low;
        if (categories && (!arg_number(arg_field(a, "category"), &number) || number < spec.low
                           || number > spec.high)) {
            snprintf(why, size, "no such category of %s: %s (%d to %d)", spec.name, arg_field(a, "category"),
                spec.low, spec.high);
            return false;
        }
        out->value = static_cast<uint32_t>(choice == 1 ? kCommentModeComment : kCommentModeRefine);
        out->y = static_cast<int16_t>(number);
        return true;
    }
    case kArgMacro: {
        static const char* const both[] = {"palette", "row", NULL};
        const int32_t palette = arg_fields_are(a, both)
            ? arg_choice(spec.choices, spec.choice_count, arg_field(a, "palette")) : -1;
        if (palette < 1) {
            snprintf(why, size, "%s takes {palette = 'ctrl' or 'alt', row = %d to %d}", spec.name, spec.low,
                spec.high);
            return false;
        }
        if (!arg_number(arg_field(a, "row"), &number) || number < spec.low || number > spec.high) {
            snprintf(why, size, "no such row of %s: %s (%d to %d)", spec.name, arg_field(a, "row"), spec.low,
                spec.high);
            return false;
        }
        out->x = static_cast<int16_t>(palette);
        out->value = static_cast<uint32_t>(number - 1);
        return true;
    }
    }
    snprintf(why, size, "%s takes no argument", spec.name);
    return false;
}

// The windows open_set opens together, by the set's name: the game's own
// opener for them, run as one call. `windows` are those a block of any
// refuses the set for up front, with the one of `by_choice` the choice
// opens beside them; `opens` the one that must be open once the opener
// returns. A set takes nothing, one list of its window, the inventory
// index of the item its opener is run for (the crystal of a synthesis, the
// tabula of a maze), an item as the item panel takes one, or one of its
// choices.
const uint8_t kTakesNothing = 0;
const uint8_t kTakesList = 1;
const uint8_t kTakesItem = 2;
const uint8_t kTakesItemPanel = 3;
const uint8_t kTakesChoice = 4;

struct SetSpec {
    const char* name;
    const char* opens;
    const char* windows[6];
    uint8_t takes;
    const ListName* choices;
    int choice_count;
    const char* const* by_choice;   // by the choice's number from 1
    const char* what;
};

const char* const kRegionPoints[] = {"conq3pts", "imperial", "allied", "colopoin"};
const ListName kTextColorCategories[] = {{1, "1"}, {2, "2"}, {3, "3"}, {4, "4"}};

const int kSetEquipment = 0;
const int kSetAbility = 1;
const int kSetMagic = 2;
const int kSetMap = 3;
const int kSetSpells = 4;
const int kSetAttachments = 5;
const int kSetLinkshell = 6;
const int kSetInstincts = 7;
const int kSetSynthesis = 8;
const int kSetTabula = 9;
const int kSetMacros = 10;
const int kSetFriend = 11;
const int kSetMail = 12;
const int kSetMount = 13;
const int kSetCompare = 14;
const int kSetStatus = 15;
const int kSetRegion = 16;
const int kSetTextColors = 17;
const SetSpec kSets[] = {
    {"equipment", "equip", {"equip", "inventor", "persona", "money", "iteminfo", NULL}, kTakesNothing, NULL, 0, NULL,
     NULL},
    {"ability", "ability", {"ability", "abisortw", "subwindo", NULL}, kTakesList, NULL, 0, NULL, NULL},
    {"magic", "magic", {"magic", "mgcsortw", "subwindo", NULL}, kTakesList, NULL, 0, NULL, NULL},
    {"map", "map0", {"mapframe", "map0", NULL}, kTakesNothing, NULL, 0, NULL, NULL},
    {"set_spells", "bluequip", {"persona", "bluehelp", "bluinven", "bluepoin", "bluequip", NULL}, kTakesNothing,
     NULL, 0, NULL, NULL},
    {"attachments", "automato", {"bluehelp", "bluinven", "statpup", "automato", NULL}, kTakesNothing, NULL, 0, NULL,
     NULL},
    {"linkshell", "link5", {"link5", "link6", NULL}, kTakesNothing, NULL, 0, NULL, NULL},
    {"instincts", "mp_mncst", {"mp_stat", "level", "mp_cstm", "mp_mncst", NULL}, kTakesNothing, NULL, 0, NULL, NULL},
    {"synthesis", "tskill1", {"inventor", "iteminfo", "tskill1", NULL}, kTakesItem, NULL, 0, NULL, NULL},
    {"tabula", "prvdungp", {"prvdungp", "bluehelp", "bluinven", NULL}, kTakesItem, NULL, 0, NULL, NULL},
    {"macros", "mcrbedit", {"mcrbedit", "mcr1edit", "mcr2edit", NULL}, kTakesNothing, NULL, 0, NULL, NULL},
    {"friend", "friend", {"friend", "titlehan", NULL}, kTakesNothing, NULL, 0, NULL, NULL},
    {"mail", "msglist", {"msglist", "titlehan", NULL}, kTakesNothing, NULL, 0, NULL, NULL},
    {"mount", "mount", {"mount", "abisortw", "subwindo", NULL}, kTakesNothing, NULL, 0, NULL, NULL},
    {"compare", "itemxinf", {"itemxinf", "itemyinf", NULL}, kTakesItemPanel, NULL, 0, NULL, NULL},
    {"status", "persona", {"persona", "level", "statcom2", NULL}, kTakesNothing, NULL, 0, NULL, NULL},
    {"region", "cnqframe", {"cnqframe", NULL}, kTakesChoice, HU_CHOICES(kRegionKinds), kRegionPoints, "kind"},
    {"text_colors", "textcol1", {"textcol2", "textcol1", NULL}, kTakesChoice, HU_CHOICES(kTextColorCategories),
     NULL, "category"},
};
const int kSetCount = static_cast<int>(sizeof(kSets) / sizeof(kSets[0]));

// The item types the game's item use runs a set's opener for: a crystal's
// synthesis, and the M and the R Maze Tabulae, whose editor opens in mode 0
// and mode 1. The item panel reads the extra data of an item of the types
// from kItemPanelExtraFirst to kItemPanelExtraLast with no test for none:
// one shown by its id alone, which has none, crashes the client.
const uint16_t kItemCrystal = 8;
const uint16_t kItemTabulaM = 0x16;
const uint16_t kItemTabulaR = 0x17;
const uint16_t kItemPanelExtraFirst = 20;
const uint16_t kItemPanelExtraLast = 23;

inline int find_set(const char* name) {
    for (int s = 0; name && s < kSetCount; ++s) {
        if (strcmp(kSets[s].name, name) == 0) {
            return s;
        }
    }
    return -1;
}

// The windows that ask the player something and wait on the reply, each
// from the engine abi whose verbs first count it: open refuses them (the
// game opens each for its own question), close refuses the ones cancel ends
// and a hide-only or close-refused one with its own reason (confyn and
// comment, which have no cancel and are neither, close as any window), and a
// block leaves an open one to its reply. While the engine hides one that
// asks now (game.h's prompt_asks()), the player's keys and the menu mouse
// mode are held off it; any other hidden window keeps its keys, a prompt
// that asks nothing now among them. `yesno` marks the game's yes/no class,
// `cancels` a prompt cancel() ends, `boolean` one answered true or false
// whose default is the row it opens on.
struct PromptSpec {
    const char* name;
    uint32_t since;
    bool yesno;
    bool cancels;
    bool boolean;
};

const PromptSpec kPrompts[] = {
    {"query", 3, false, true, false},
    {"passinpu", 3, false, true, false},
    {"prtyjoin", 3, false, true, false},
    {"delivery", 3, false, true, false},
    {"post1", 3, false, true, false},
    {"post2", 3, false, true, false},
    {"link5", 3, false, true, false},
    {"arealist", 3, false, true, false},
    {"comyn", 8, true, true, true},
    {"merityn", 8, true, true, true},
    {"resyn", 8, true, true, true},
    {"resynus", 8, true, true, true},
    {"resynde", 8, true, true, true},
    {"resynfr", 8, true, true, true},
    {"shopbuy", 8, true, true, true},
    {"shopsell", 8, true, true, true},
    {"dead", 8, false, false, false},
    {"moneyctr", 8, false, true, false},
    {"itemctrl", 8, false, true, false},
    {"sortyn", 8, false, true, true},
    {"evitem01", 8, false, true, true},
    {"confyn", 8, false, false, true},
    {"bankmenu", 8, false, true, false},
    {"maplist", 8, false, true, false},
    {"gmtell", 8, false, true, false},
    {"mp_dead", 8, false, false, false},
    {"ok", 8, false, false, false},
    {"rem4line", 8, false, true, false},
    {"rem4li2", 8, false, true, false},
    {"mogdoor", 8, false, true, false},
    {"stringdl", 8, false, true, false},
    {"comment", 8, false, false, false},
    {"msgline", 8, false, true, false},
    {"scname", 8, false, true, false},
    {"link11", 8, false, true, false},
    {"trade", 8, false, true, false},
    {"handover", 8, false, true, false},
};
const int kPromptCount = static_cast<int>(sizeof(kPrompts) / sizeof(kPrompts[0]));

// The number boxes among the prompts: moneyctr asks for gil, itemctrl for
// an item count.
const int kBoxCount = 2;
const int kBoxMoney = 0;
const int kBoxItem = 1;
const char* const kBoxNames[kBoxCount] = {"moneyctr", "itemctrl"};

inline int box_named(const char* name) {
    for (int b = 0; b < kBoxCount; ++b) {
        if (strcmp(kBoxNames[b], name) == 0) {
            return b;
        }
    }
    return -1;
}

// The other prompts of engine abi 8, each answered by its own reply: the
// item and sort confirmations (sortyn), the key item confirmation
// (evitem01), the config's "restore defaults" (confyn), the Mog Safe
// picker (bankmenu), the map list (maplist), a GM's question (gmtell), the
// Monstrosity KO window (mp_dead), the error notice (ok) and the Mog House
// exit (mogdoor).
enum Asked {
    kAskedSortyn,
    kAskedEvitem,
    kAskedConfyn,
    kAskedBank,
    kAskedMaplist,
    kAskedGmtell,
    kAskedMpDead,
    kAskedOk,
    kAskedDoor,
    kAskedCount
};
const char* const kAskedNames[kAskedCount] = {"sortyn", "evitem01", "confyn", "bankmenu", "maplist", "gmtell",
                                              "mp_dead", "ok", "mogdoor"};

inline int asked_named(const char* name) {
    for (int a = 0; a < kAskedCount; ++a) {
        if (strcmp(kAskedNames[a], name) == 0) {
            return a;
        }
    }
    return -1;
}

// The NPC dialogue boxes among the prompts, each showing the pages of its
// own text object: rem4line over the chat log, rem4li2 over the second.
// They show one dialogue, the same page in both: a hold or a reply on
// either is one on both (Inventory::dialogue_twin()), a move or a size each
// box's own.
const int kDialogueCount = 2;
const char* const kDialogueNames[kDialogueCount] = {"rem4line", "rem4li2"};

inline int dialogue_named(const char* name) {
    for (int d = 0; d < kDialogueCount; ++d) {
        if (strcmp(kDialogueNames[d], name) == 0) {
            return d;
        }
    }
    return -1;
}

// The text-entry windows among the prompts, each answered with what the
// player would type: the outgoing box's recipient (stringdl), the bazaar,
// search and linkshell messages (comment), a PlayOnline friend message
// (msgline), the player search's name (scname) and a new linkshell's name
// and colors (link11).
enum Text {
    kTextStringdl,
    kTextComment,
    kTextMsgline,
    kTextScname,
    kTextLink11,
    kTextCount
};
const char* const kTextNames[kTextCount] = {"stringdl", "comment", "msgline", "scname", "link11"};

inline int text_named(const char* name) {
    for (int t = 0; t < kTextCount; ++t) {
        if (strcmp(kTextNames[t], name) == 0) {
            return t;
        }
    }
    return -1;
}

// What each takes, in bytes as the game holds them: the recipient's name,
// letters only, as the outgoing box's check takes it; the comment's three
// lines; msgline's text, by the field its mode gives it, of which the
// long field keeps one byte less: the commit zeroes its last; the searched
// name; the linkshell's name, which its encoder takes below 20 bytes, and
// each of its three colors.
const int kStringdlMin = 3;
const int kStringdlMax = 15;
const int kCommentLines = 3;
const int kCommentLine = 0x28;
const int kMsglineLong = 0x12C;
const int kMsglineKept = kMsglineLong - 1;
const int kMsglineShort = 15;
const int kScnameMax = 15;
const int kLink11Max = 19;
const int kLink11ColorMax = 15;

// A text window's answer as the Lua thread checked it, handed to the game
// thread through the engine's slot for that window: a reply to a window
// counts against it until the drain carries it out, and another is refused
// meanwhile, so the slot is never written while a queued reply reads it.
struct TextAnswer {
    int32_t yes;                            // stringdl's confirm: 1 yes, 0 no; -1 its name
    char text[kMsglineLong + 1];            // stringdl's, msgline's, scname's or link11's text
    char lines[kCommentLines][kCommentLine + 1];
    uint8_t color[3];                       // link11's colors
    uint8_t colored;                        // link11: 0 keeps the colors the window holds
};

// The windows an item is offered in: the trade with another player (trade)
// and the trade with an NPC (handover). offer() and withdraw() reply to them
// beside answer() and cancel().
const int kOfferCount = 2;
const int kOfferTrade = 0;
const int kOfferHandover = 1;
const char* const kOfferNames[kOfferCount] = {"trade", "handover"};

inline int offer_named(const char* name) {
    for (int o = 0; o < kOfferCount; ++o) {
        if (strcmp(kOfferNames[o], name) == 0) {
            return o;
        }
    }
    return -1;
}

inline const PromptSpec* prompt_spec(const char* name) {
    for (int i = 0; i < kPromptCount; ++i) {
        if (strcmp(kPrompts[i].name, name) == 0) {
            return &kPrompts[i];
        }
    }
    return NULL;
}

// A prompt to the verbs of an engine abi `abi` slot.
inline bool prompt_window(const char* name, uint32_t abi) {
    const PromptSpec* p = prompt_spec(name);
    return p && p->since <= abi;
}

inline bool yesno_window(const char* name) {
    const PromptSpec* p = prompt_spec(name);
    return p && p->yesno;
}

inline bool prompt_cancels(const char* name) {
    const PromptSpec* p = prompt_spec(name);
    return p && p->cancels;
}

inline bool boolean_window(const char* name) {
    const PromptSpec* p = prompt_spec(name);
    return p && p->boolean;
}

// The classes whose open hook places the window with a constant after the
// default rect is written, so the default rect is not their home.
inline bool open_hook_places(const char* name) {
    static const char* const names[] = {
        "persona", "mp_stat", "joblevel", "stringdl", "inventor",
        "iteminfo", "itemxinf", "level", "levmerit",
    };
    for (size_t i = 0; i < sizeof(names) / sizeof(names[0]); ++i) {
        if (strcmp(name, names[i]) == 0) {
            return true;
        }
    }
    return false;
}

// A frame the game hangs from the default rect's bottom edge: its controller
// keeps the bottom at origin.y + the default height and moves the top with
// its content (log lines, the roster), and keeps whatever bottom a
// SetPosition or SetFrameRect leaves. A template window's frame is its
// default rect.
inline bool hangs_from_bottom(int top, int bottom, int default_top, int default_bottom) {
    return bottom == default_bottom && top != default_top;
}

// The windows measured to hang from the bottom. At exactly their default
// height (a six-member party, a 134-px log) their frame is their default
// rect, which the shape cannot tell from a template window's.
inline bool bottom_anchored_class(const char* name) {
    return strcmp(name, "logwindo") == 0 || strcmp(name, "logwin2") == 0 || strcmp(name, "partywin") == 0;
}

// The class, else the frame as the game left it.
inline bool reads_bottom_anchored(const char* name, int top, int bottom, int default_top, int default_bottom) {
    return bottom_anchored_class(name) || hangs_from_bottom(top, bottom, default_top, default_bottom);
}

// The compass is no registry row: the game draws it outside the menu
// system. The engine names it all the same, on an entry with no row and a
// key of 16 NUL bytes, which no open key (space-padded, cut at the first
// NUL) and no live resource key can equal. targetarrow, the arrows the
// target window draws over the target, is named the same way: they are
// pieces of that window's drawing, not a window.
const char kCompassName[] = "compass";
const char kTargetArrowName[] = "targetarrow";

struct Inventory {
    NameEntry names[kMaxNames];
    int16_t name_of_row[kRowCount];
    int count;
    int16_t compass;        // the compass entry's index; -1 until built
    int16_t targetarrow;    // the target arrows' entry's index; -1 until built
    int16_t targetwi;       // the target window's, whose drawing carries the arrows; -1 until built
    int16_t ability;        // the ability entry's index, whose events and blocks carry a category; -1 until built
    uint8_t prompt[kMaxNames];  // kPrompts by index, for the hook handlers
    uint8_t first_row[kMaxNames];   // a boolean prompt: its default is the row its open shows first
    int16_t box_window[kBoxCount];  // each number box's index
    int16_t asked_window[kAskedCount];
    int16_t dialogue_window[kDialogueCount];
    int16_t text_window[kTextCount];
    int16_t offer_window[kOfferCount];

    // Every record, whatever its type, then the compass and the target
    // arrows; rows share an entry only when type and name both match. Fails
    // on an inventory the engine cannot index, which only a broken
    // menu_table.inc can produce: one name under two types could not be told
    // apart by name.
    bool build() {
        count = 0;
        compass = -1;
        targetarrow = -1;
        targetwi = -1;
        ability = -1;
        // The aliases' rows last, each its window's second.
        for (int pass = 0; pass < 2; ++pass) {
            for (int r = 0; r < kRowCount; ++r) {
                const char* tp = kRowSpecs[r].type;
                const char* nm = kRowSpecs[r].name;
                const char* canonical = alias_of(nm);
                if ((canonical != NULL) != (pass == 1)) {
                    continue;
                }
                name_of_row[r] = -1;
                const size_t len = strlen(nm);
                if (strlen(tp) != 8 || len == 0 || len > 8) {
                    return false;
                }
                char key[kKeyLen];
                memcpy(key, tp, 8);
                memset(key + 8, ' ', 8);
                memcpy(key + 8, nm, len);
                int n = find_exact(canonical ? canonical : nm);
                if (canonical) {
                    if (n < 0 || (names[n].alias[0] && memcmp(names[n].alias, key, kKeyLen) != 0)) {
                        return false;
                    }
                    memcpy(names[n].alias, key, kKeyLen);
                } else if (n >= 0 && memcmp(names[n].key, key, kKeyLen) != 0) {
                    return false;
                }
                if (n < 0) {
                    if (count >= kMaxNames) {
                        return false;
                    }
                    n = count++;
                    NameEntry& e = names[n];
                    memset(&e, 0, sizeof(e));
                    memcpy(e.name, nm, len);
                    memcpy(e.key, key, kKeyLen);
                }
                NameEntry& e = names[n];
                if (e.row_count >= 2) {
                    return false;
                }
                e.rows[e.row_count++] = static_cast<int16_t>(r);
                name_of_row[r] = static_cast<int16_t>(n);
            }
        }
        if (count == 0 || count + 2 > kMaxNames || find_exact(kCompassName) >= 0
            || find_exact(kTargetArrowName) >= 0) {
            return false;
        }
        ability = static_cast<int16_t>(find_exact("ability"));
        targetwi = static_cast<int16_t>(find_exact("targetwi"));
        if (targetwi < 0 || names[targetwi].row_count != 1) {
            return false;
        }
        memset(prompt, 0, sizeof(prompt));
        memset(first_row, 0, sizeof(first_row));
        for (int i = 0; i < kPromptCount; ++i) {
            const int n = find_exact(kPrompts[i].name);
            if (n < 0) {
                return false;
            }
            prompt[n] = 1;
            first_row[n] = kPrompts[i].boolean ? 1 : 0;
        }
        for (int b = 0; b < kBoxCount; ++b) {
            box_window[b] = static_cast<int16_t>(find_exact(kBoxNames[b]));
            if (box_window[b] < 0) {
                return false;
            }
        }
        for (int a = 0; a < kAskedCount; ++a) {
            asked_window[a] = static_cast<int16_t>(find_exact(kAskedNames[a]));
            if (asked_window[a] < 0 || !prompt[asked_window[a]]) {
                return false;
            }
        }
        for (int d = 0; d < kDialogueCount; ++d) {
            dialogue_window[d] = static_cast<int16_t>(find_exact(kDialogueNames[d]));
            if (dialogue_window[d] < 0 || !prompt[dialogue_window[d]]) {
                return false;
            }
        }
        for (int t = 0; t < kTextCount; ++t) {
            text_window[t] = static_cast<int16_t>(find_exact(kTextNames[t]));
            if (text_window[t] < 0 || !prompt[text_window[t]]) {
                return false;
            }
        }
        for (int o = 0; o < kOfferCount; ++o) {
            offer_window[o] = static_cast<int16_t>(find_exact(kOfferNames[o]));
            if (offer_window[o] < 0 || !prompt[offer_window[o]]) {
                return false;
            }
        }
        NameEntry& c = names[count];
        memset(&c, 0, sizeof(c));
        memcpy(c.name, kCompassName, strlen(kCompassName));
        compass = static_cast<int16_t>(count++);
        NameEntry& t = names[count];
        memset(&t, 0, sizeof(t));
        memcpy(t.name, kTargetArrowName, strlen(kTargetArrowName));
        targetarrow = static_cast<int16_t>(count++);
        return true;
    }

    // The number box name index n is, or -1.
    int box_of(int n) const {
        for (int b = 0; b < kBoxCount; ++b) {
            if (n >= 0 && box_window[b] == n) {
                return b;
            }
        }
        return -1;
    }

    // The Asked window name index n is, or -1.
    int asked_of(int n) const {
        for (int a = 0; a < kAskedCount; ++a) {
            if (n >= 0 && asked_window[a] == n) {
                return a;
            }
        }
        return -1;
    }

    // The dialogue box name index n is, or -1.
    int dialogue_of(int n) const {
        for (int d = 0; d < kDialogueCount; ++d) {
            if (n >= 0 && dialogue_window[d] == n) {
                return d;
            }
        }
        return -1;
    }

    // The other dialogue box's name index when n is one, or -1.
    int dialogue_twin(int n) const {
        const int d = dialogue_of(n);
        return d < 0 ? -1 : dialogue_window[kDialogueCount - 1 - d];
    }

    // The text window name index n is, or -1.
    int text_of(int n) const {
        for (int t = 0; t < kTextCount; ++t) {
            if (n >= 0 && text_window[t] == n) {
                return t;
            }
        }
        return -1;
    }

    // The offer window name index n is, or -1.
    int offer_of(int n) const {
        for (int o = 0; o < kOfferCount; ++o) {
            if (n >= 0 && offer_window[o] == n) {
                return o;
            }
        }
        return -1;
    }

    int find_exact(const char* nm) const {
        const char* canonical = nm ? alias_of(nm) : NULL;
        const char* want = canonical ? canonical : nm;
        for (int n = 0; want && n < count; ++n) {
            if (strcmp(names[n].name, want) == 0) {
                return n;
            }
        }
        return -1;
    }

    // A window by its registry name in any case; never a group name.
    int find_window(const char* text) const {
        char low[16];
        return lower_name(text, low) && strlen(low) <= kNameMax ? find_exact(low) : -1;
    }

    // A player-facing name as engine abi 1 and 2 take it: the registry name in
    // any case, or one of the dock-group names standing for that group's
    // anchor.
    int find(const char* text) const {
        if (!text) {
            return -1;
        }
        char low[16];
        size_t len = strlen(text);
        if (len == 0 || len >= sizeof(low)) {
            return -1;
        }
        for (size_t i = 0; i <= len; ++i) {
            low[i] = lower_ascii(text[i]);
        }
        for (int g = 0; g < kGroupCount; ++g) {
            if (strcmp(low, kGroups[g].name) == 0) {
                return find_exact(kGroups[g].anchor);
            }
        }
        if (len > kNameMax) {
            return -1;
        }
        return find_exact(low);
    }

    // `text` lower-cased into `low`; false for a text that cannot be a name.
    static bool lower_name(const char* text, char low[16]) {
        if (!text) {
            return false;
        }
        const size_t len = strlen(text);
        if (len == 0 || len >= 16) {
            return false;
        }
        for (size_t i = 0; i <= len; ++i) {
            low[i] = lower_ascii(text[i]);
        }
        return true;
    }

    // A key as a caller hands it to the open routine: the type and the name,
    // NUL- or space-padded to 16. Stops at the first NUL, so it never reads
    // past a short string.
    int find_key(const char* raw) const {
        char key[kKeyLen];
        size_t i = 0;
        for (; i < kKeyLen && raw[i] != '\0'; ++i) {
            key[i] = lower_ascii(raw[i]);
        }
        for (; i < kKeyLen; ++i) {
            key[i] = ' ';
        }
        return find_key16(key);
    }

    // A key exactly as the registry and a live menu's resource record hold it.
    int find_key16(const void* key) const {
        for (int n = 0; n < count; ++n) {
            if (names[n].keyed(key)) {
                return n;
            }
        }
        return -1;
    }
};

inline int find_group(const char* text) {
    if (!text) {
        return -1;
    }
    for (int g = 0; g < kGroupCount; ++g) {
        if (strcmp(text, kGroups[g].name) == 0) {
            return g;
        }
    }
    return -1;
}

// A group name in any case, as engine abi 3 takes it.
inline int find_group_any(const char* text) {
    char low[16];
    return Inventory::lower_name(text, low) ? find_group(low) : -1;
}

// Every piece a move of group `g` carries: its anchor, each piece whose
// policy has the group's bit, the anchor of the group it carries, and the
// same again for every group whose anchor is in. `policy[n]` is the dock word
// to go by: the live menu's, 0 for one that is not open, for a move; the
// registry's, for the list of members.
inline void group_closure(const Inventory& inv, int g, const uint32_t* policy, uint8_t* in_set) {
    memset(in_set, 0, kMaxNames);
    const int anchor = inv.find_exact(kGroups[g].anchor);
    if (anchor >= 0) {
        in_set[anchor] = 1;
    }
    uint8_t expanded[kGroupCount];
    memset(expanded, 0, sizeof(expanded));
    bool grew = true;
    while (grew) {
        grew = false;
        for (int gi = 0; gi < kGroupCount; ++gi) {
            const int a = inv.find_exact(kGroups[gi].anchor);
            if (expanded[gi] || a < 0 || !in_set[a]) {
                continue;
            }
            expanded[gi] = 1;
            grew = true;
            for (int n = 0; n < inv.count; ++n) {
                if (policy[n] & kGroups[gi].mask) {
                    in_set[n] = 1;
                }
            }
            const int carried = kGroups[gi].carries;
            const int c = carried >= 0 ? inv.find_exact(kGroups[carried].anchor) : -1;
            if (c >= 0) {
                in_set[c] = 1;
            }
        }
    }
}

// The group DockReset re-docks a window carrying these bits in, in the show
// path's order; group 2's window goes to group 1 while its anchor, the
// target window, is not open. -1 for a window that does not dock.
inline int dock_reset_group(uint32_t policy, bool target_open) {
    if (policy & 0x1000) {
        return 0;
    }
    if (policy & 0x4000) {
        return target_open ? 2 : 1;
    }
    if (policy & 0x2000) {
        return 1;
    }
    if (policy & 0x10000000) {
        return 3;
    }
    return -1;
}

// ---------------------------------------------------------------------------
// Resize families: the templates a window's own controller swaps between by
// row count. Keys are the template names the game passes, cut to the eight
// characters its 16-byte compare reads.

const char* const kActionKeys[] = {
    "actionm1", "actionm2", "actionm3", "actionm4", "actionm5",
    "actionm6", "actionm7", "actionm8", "actionm9", "actiom10",
};
const char* const kPartyKeys[] = {"ptw1", "ptw2", "ptw3", "ptw4", "ptw5", "ptw6"};
const char* const kItemKeys[] = {
    "iteminfo", "item4inf", "item5inf", "item6inf", "item7inf",
    "item8inf", "item9inf", "item10in", "item11in", "item12in",
};
const char* const kItemxKeys[] = {
    "itemxinf", "itemx4in", "itemx5in", "itemx6in", "itemx7in",
    "itemx8in", "itemx9in", "itemx10i", "itemx11i", "itemx12i",
};

struct Family {
    const char* name;
    const char* const* keys;    // keys[rows - min]
    int min;
    int max;
    bool counts;                // the owner sets the item count around its swap
    const char* holds;          // when the owner sizes the window again
};

// How long a size the engine puts on holds: until the window closes, or
// until its owner swaps again -- on its trigger (a row or roster change), or
// every frame from its content.
const char kHoldsReopen[] = "reopen";
const char kHoldsTrigger[] = "trigger";
const char kHoldsFrame[] = "frame";

// mp_pmode's controller swaps through its own table of eight. partywin's
// owner reads the item count as its cursor's modulus and the item panes' owner
// leaves it alone; neither writes it around a swap.
const Family kFamilies[] = {
    {"playermo", kActionKeys, 1, 10, true, kHoldsTrigger},
    {"mp_pmode", kActionKeys, 1, 8, true, kHoldsTrigger},
    {"partywin", kPartyKeys, 1, 6, false, kHoldsTrigger},
    {"iteminfo", kItemKeys, 3, 12, false, kHoldsFrame},
    {"itemxinf", kItemxKeys, 3, 12, false, kHoldsFrame},
};
const int kFamilyCount = static_cast<int>(sizeof(kFamilies) / sizeof(kFamilies[0]));

inline const Family* family_of(const char* name) {
    for (int i = 0; i < kFamilyCount; ++i) {
        if (strcmp(kFamilies[i].name, name) == 0) {
            return &kFamilies[i];
        }
    }
    return NULL;
}

inline const char* size_holds(const char* name) {
    const Family* f = family_of(name);
    return f ? f->holds : kHoldsReopen;
}

// The template key for `rows`, space-padded to 16 like a registry key, in a
// buffer zeroed past it as the game's own key records are. False outside
// the family.
inline bool family_key(const Family& f, int rows, char out[32]) {
    if (rows < f.min || rows > f.max) {
        return false;
    }
    const char* stem = f.keys[rows - f.min];
    memset(out, 0, 32);
    memcpy(out, "menu    ", 8);
    memset(out + 8, ' ', 8);
    memcpy(out + 8, stem, strlen(stem));
    return true;
}

// ---------------------------------------------------------------------------
// Registry verification. The live table must be the expected one, row for
// row: the inventory gives the engine every original byte it restores.

inline bool verify_registry(const uint8_t* table, int rows, char* error, size_t size) {
    if (rows < kRowCount + 1) {
        snprintf(error, size, "menu table: only %d rows readable, %d expected", rows, kRowCount);
        return false;
    }
    for (int r = 0; r < kRowCount; ++r) {
        const uint8_t* row = table + r * kRowStride;
        const RowSpec& s = kRowSpecs[r];
        char name[8];
        memset(name, ' ', 8);
        memcpy(name, s.name, strlen(s.name));
        const char* what = NULL;
        if (memcmp(row, s.type, 8) != 0) {
            what = "type";
        } else if (memcmp(row + 8, name, 8) != 0) {
            what = "name";
        } else if (row[kRowLayer] != s.layer) {
            what = "draw layer";
        } else {
            uint32_t policy;
            memcpy(&policy, row + kRowPolicy, 4);
            uint16_t overlap;
            memcpy(&overlap, row + kRowOverlap, 2);
            if (policy != s.policy) {
                what = "policy";
            } else if (s.avail >= 0 && row[kRowAvail] != static_cast<uint8_t>(s.avail)) {
                what = "availability";
            } else if (s.overlap >= 0 && overlap != static_cast<uint16_t>(s.overlap)) {
                what = "overlap group";
            }
        }
        if (what) {
            snprintf(error, size,
                "menu table row %d (%s): %s differs from the expected inventory"
                " -- the client was patched, or another tool changed the table",
                r, s.name, what);
            return false;
        }
    }
    static const uint8_t zero[8] = {0};
    if (memcmp(table + kRowCount * kRowStride, zero, 8) != 0) {
        snprintf(error, size, "menu table: row %d is not the terminator", kRowCount);
        return false;
    }
    return true;
}

// ---------------------------------------------------------------------------
// Signatures. Mask 'x' is an exact byte, anything else a wildcard.

inline int hex_digit(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    return -1;
}

// "8b 44 ?? 04" as signatures.h writes it, into bytes and a mask.
// Returns the length; 0 for a malformed text or one longer than `max`.
inline size_t parse_signature(const char* text, uint8_t* bytes, char* mask, size_t max) {
    size_t n = 0;
    for (const char* p = text; *p;) {
        if (*p == ' ') {
            ++p;
            continue;
        }
        if (n + 1 >= max || !p[1]) {
            return 0;
        }
        if (p[0] == '?' && p[1] == '?') {
            bytes[n] = 0;
            mask[n] = '?';
        } else {
            const int hi = hex_digit(p[0]);
            const int lo = hex_digit(p[1]);
            if (hi < 0 || lo < 0) {
                return 0;
            }
            bytes[n] = static_cast<uint8_t>(hi * 16 + lo);
            mask[n] = 'x';
        }
        ++n;
        p += 2;
    }
    mask[n] = '\0';
    return n;
}

struct ScanResult {
    const uint8_t* hits[4];
    bool jumped[4];
    int count;              // may exceed 4; only the first 4 are stored
};

inline bool match_from(const uint8_t* p, const uint8_t* sig, const char* mask, size_t from) {
    for (size_t i = from; mask[i]; ++i) {
        if (mask[i] == 'x' && p[i] != sig[i]) {
            return false;
        }
    }
    return true;
}

// Every place the signature matches. With `jumps`, a place whose first byte
// is a jump and whose bytes from 5 on match is a candidate too: it is a
// function some detour already patched, and the caller decides whose detour.
inline void scan(const uint8_t* base, size_t size, const uint8_t* sig, const char* mask,
                 bool jumps, ScanResult* out) {
    const size_t len = strlen(mask);
    out->count = 0;
    if (len < 5 || size < len) {
        return;
    }
    for (size_t i = 0; i + len <= size; ++i) {
        const uint8_t* p = base + i;
        bool jumped = false;
        if (!match_from(p, sig, mask, 0)) {
            if (!jumps || p[0] != 0xE9 || !match_from(p, sig, mask, 5)) {
                continue;
            }
            jumped = true;
        }
        if (out->count < 4) {
            out->hits[out->count] = p;
            out->jumped[out->count] = jumped;
        }
        ++out->count;
    }
}

// ---------------------------------------------------------------------------
// Answers given as tables.

// A table answer as the binding encodes it (hideui_engine_abi.h's
// HU_ANSWER_TABLE), its entries in the order they came: each key's parts,
// a whole number's as '#' and its digits, and the value's type ('s', 'n',
// 'b', 't' or 'o'); a string's first kTableBytes bytes and its length, a
// number or a boolean in `number`.
const int kTableEntries = 16;
const int kTableBytes = 128;

struct TableEntry {
    char key[2][24];
    int parts;
    char type;
    double number;
    int length;
    char bytes[kTableBytes + 1];
};

struct AnswerTable {
    TableEntry entry[kTableEntries];
    int count;
    bool too_large;         // the binding could not encode it, or it has more than kTableEntries entries
};

// One key part, up to the '/', '=' or end: false for an empty part or one
// too long to keep.
inline bool table_key_part(const char** p, char* out, size_t size) {
    size_t n = 0;
    while (**p && **p != '/' && **p != '=' && **p != ';') {
        if (n + 1 >= size) {
            return false;
        }
        out[n++] = *(*p)++;
    }
    out[n] = '\0';
    return n > 0;
}

// False for a text no binding of this engine encodes.
inline bool parse_answer_table(const char* text, AnswerTable* out) {
    memset(out, 0, sizeof(*out));
    if (!text) {
        return false;
    }
    if (strcmp(text, "!") == 0) {
        out->too_large = true;
        return true;
    }
    const char* p = text;
    while (*p) {
        if (out->count >= kTableEntries) {
            out->too_large = true;
            return true;
        }
        TableEntry& e = out->entry[out->count];
        if (!table_key_part(&p, e.key[0], sizeof(e.key[0]))) {
            return false;
        }
        e.parts = 1;
        if (*p == '/') {
            ++p;
            if (!table_key_part(&p, e.key[1], sizeof(e.key[1]))) {
                return false;
            }
            e.parts = 2;
        }
        if (*p++ != '=') {
            return false;
        }
        e.type = *p++;
        if (e.type == 's') {
            while (*p && *p != ';') {
                const int hi = hex_digit(p[0]);
                const int lo = p[1] ? hex_digit(p[1]) : -1;
                if (hi < 0 || lo < 0) {
                    return false;
                }
                if (e.length < kTableBytes) {
                    e.bytes[e.length] = static_cast<char>(hi * 16 + lo);
                }
                ++e.length;
                p += 2;
            }
            e.bytes[e.length < kTableBytes ? e.length : kTableBytes] = '\0';
        } else if (e.type == 'n') {
            char* end = NULL;
            e.number = strtod(p, &end);
            if (end == p) {
                return false;
            }
            p = end;
        } else if (e.type == 'b') {
            if (*p != '0' && *p != '1') {
                return false;
            }
            e.number = *p++ == '1' ? 1 : 0;
        } else if (e.type != 't' && e.type != 'o') {
            return false;
        }
        if (*p++ != ';') {
            return false;
        }
        ++out->count;
    }
    return true;
}

// ---------------------------------------------------------------------------
// Settings: each control the player finds on a page of the game's Config
// menu (a toggle, a group of choice rows, a slider, a color, a key), in the
// menu's order, named after its label there: an ON/OFF row a bool, true for
// ON; a row of other buttons a choice of them in the page's order. Each is
// kept in the game's config store, character_information in the player's
// flags the server keeps; game.h reads and sets them as the pages do, and
// the game saves the store as the player leaves the game.

enum SettingValue { kValueNumber, kValueBool, kValueChoice, kValueColor, kValueKey };

// Where a setting is kept. kStoreWord: the store word `id`. kStoreBit: bit
// `bit` of the words from `id` on, 32 to a word. kStoreFilter: a row of the
// chat filter page, its bits the game's row table gives. kStoreLock,
// kStoreIdle and kStoreFlag: set through the routines their pages call (the
// equipment lock's request, the idle timer, the player flag's request).
enum SettingStore { kStoreWord, kStoreBit, kStoreFilter, kStoreLock, kStoreIdle, kStoreFlag };

// The game's table whose list `list` holds the setting at `row`, its text
// `label`: the text colors' rows give their store word, the chat filter's,
// the battle effects filter's and the chat log routing's their bits.
enum SettingFrom { kFromNone, kFromColors, kFromFilter, kFromEffects, kFromRouting, kFromKeys };

const uint8_t kInverted = 1;    // a bool, true while its word or bit is 0
const char* const kKeyPages[2] = {"k1assign", "k2assign"};     // kFromKeys's lists' pages
const uint8_t kCommits = 2;     // its page edits a copy and writes the copy back as it closes

struct SettingChoice {
    const char* name;
    int32_t value;
};

struct SettingSpec {
    const char* name;
    const char* page;               // the Config window that shows it
    uint8_t value;
    uint8_t store;
    uint8_t id;
    int8_t bit;
    uint8_t flags;
    uint8_t from;
    uint8_t list;
    uint8_t row;
    int16_t label;                  // its text in the game's string table kLabelTable, -1 for none
    const SettingChoice* choices;   // in the page's row order, ended by a NULL name
};

const int32_t kLabelTable = 0x0B;

const SettingChoice kTimestampChoices[] = {{"minutes", 1}, {"seconds", 2}, {"off", 0}, {NULL, 0}};
const SettingChoice kShadowChoices[] = {{"high", 2}, {"normal", 0}, {"off", 1}, {NULL, 0}};
const SettingChoice kTypeChoices[] = {{"type1", 0}, {"type2", 1}, {NULL, 0}};
const SettingChoice kLogMultiWindowChoices[] = {{"vertical", 1}, {"horizontal", 2}, {"off", 0}, {NULL, 0}};
const SettingChoice kDamageDisplayChoices[] = {{"log", 0}, {"both", 1}, {"screen", 2}, {NULL, 0}};
const SettingChoice kShowHideChoices[] = {{"show", 0}, {"hide", 1}, {NULL, 0}};
const SettingChoice kIconTypeChoices[] = {{"type1", 1}, {"type2", 0}, {NULL, 0}};
const SettingChoice kNormalStaticChoices[] = {{"normal", 0}, {"static", 1}, {NULL, 0}};
const SettingChoice kKeyboardSizeChoices[] = {{"full", 0}, {"compact1", 1}, {"compact2", 2}, {NULL, 0}};
const SettingChoice kPaletteSizeChoices[] = {{"full", 0}, {"compact", 1}, {NULL, 0}};
const SettingChoice kPalettePositionChoices[] = {{"dynamic", 0}, {"static", 1}, {NULL, 0}};
const SettingChoice kCameraViewChoices[] = {{"normal", 0}, {"chase_cam", 1}, {NULL, 0}};
const SettingChoice kAxisChoices[] = {{"normal", 0}, {"inverted", 1}, {NULL, 0}};
const SettingChoice kSoftwareKeyboardChoices[] = {{"off", 0}, {"full", 1}, {"compact", 2}, {NULL, 0}};
const SettingChoice kTermFilterChoices[] = {{"on", 1}, {"display_name_only", 2}, {"off", 0}, {NULL, 0}};

const SettingSpec kSettings[] = {
    {"auto_target", "conf2win", kValueBool, kStoreWord, 0x07, -1, kInverted, kFromNone, 0, 0, -1, NULL},
    {"sound_effects_volume", "conf2win", kValueNumber, kStoreWord, 0x09, -1, 0, kFromNone, 0, 0, -1, NULL},
    {"music_volume", "conf2win", kValueNumber, kStoreWord, 0x0A, -1, 0, kFromNone, 0, 0, -1, NULL},
    {"gamma_red", "conf2win", kValueNumber, kStoreWord, 0x37, -1, 0, kFromNone, 0, 0, -1, NULL},
    {"gamma_green", "conf2win", kValueNumber, kStoreWord, 0x38, -1, 0, kFromNone, 0, 0, -1, NULL},
    {"gamma_blue", "conf2win", kValueNumber, kStoreWord, 0x39, -1, 0, kFromNone, 0, 0, -1, NULL},
    {"inventory_sort", "conf2win", kValueBool, kStoreWord, 0x9A, -1, 0, kFromNone, 0, 0, -1, NULL},
    {"inventory_type", "conf2win", kValueChoice, kStoreWord, 0xCA, -1, 0, kFromNone, 0, 0, -1, kTypeChoices},
    {"filter_say", "cfilter", kValueNumber, kStoreFilter, 0x00, -1, kCommits, kFromFilter, 0, 0, 0x00, NULL},
    {"filter_tell", "cfilter", kValueNumber, kStoreFilter, 0x00, -1, kCommits, kFromFilter, 0, 1, 0x02, NULL},
    {"filter_party", "cfilter", kValueNumber, kStoreFilter, 0x00, -1, kCommits, kFromFilter, 0, 2, 0x03, NULL},
    {"filter_linkshell", "cfilter", kValueNumber, kStoreFilter, 0x00, -1, kCommits, kFromFilter, 0, 3, 0x04, NULL},
    {"filter_linkshell_2", "cfilter", kValueNumber, kStoreFilter, 0x00, -1, kCommits, kFromFilter, 0, 4, 0x05, NULL},
    {"filter_assist_j", "cfilter", kValueNumber, kStoreFilter, 0x00, -1, kCommits, kFromFilter, 0, 5, 0x06, NULL},
    {"filter_assist_e", "cfilter", kValueNumber, kStoreFilter, 0x00, -1, kCommits, kFromFilter, 0, 6, 0x07, NULL},
    {"filter_unity", "cfilter", kValueNumber, kStoreFilter, 0x00, -1, kCommits, kFromFilter, 0, 7, 0x08, NULL},
    {"filter_emotes", "cfilter", kValueNumber, kStoreFilter, 0x00, -1, kCommits, kFromFilter, 0, 8, 0x09, NULL},
    {"filter_shout", "cfilter", kValueNumber, kStoreFilter, 0x00, -1, kCommits, kFromFilter, 0, 9, 0x01, NULL},
    {"filter_yell", "cfilter", kValueBool, kStoreFilter, 0x00, -1, kCommits, kFromFilter, 0, 10, 0xC3, NULL},
    {"filter_special_actions_started_on_by_you", "cfilter", kValueBool, kStoreFilter,
     0x00, -1, kCommits, kFromFilter, 0, 11, 0x0B, NULL},
    {"filter_special_action_effects_on_by_you", "cfilter", kValueBool, kStoreFilter,
     0x00, -1, kCommits, kFromFilter, 0, 12, 0x0C, NULL},
    {"filter_attacks_by_you", "cfilter", kValueBool, kStoreFilter, 0x00, -1, kCommits, kFromFilter, 0, 13, 0x0D, NULL},
    {"filter_missed_attacks_by_you", "cfilter", kValueBool, kStoreFilter,
     0x00, -1, kCommits, kFromFilter, 0, 14, 0x0E, NULL},
    {"filter_attacks_you_evade", "cfilter", kValueBool, kStoreFilter,
     0x00, -1, kCommits, kFromFilter, 0, 15, 0x0F, NULL},
    {"filter_damage_you_take", "cfilter", kValueBool, kStoreFilter,
     0x00, -1, kCommits, kFromFilter, 0, 16, 0x10, NULL},
    {"filter_special_actions_started_on_by_party", "cfilter", kValueBool, kStoreFilter,
     0x00, -1, kCommits, kFromFilter, 0, 17, 0x1E, NULL},
    {"filter_special_action_effects_on_by_party", "cfilter", kValueBool, kStoreFilter,
     0x00, -1, kCommits, kFromFilter, 0, 18, 0x14, NULL},
    {"filter_attacks_by_party", "cfilter", kValueBool, kStoreFilter,
     0x00, -1, kCommits, kFromFilter, 0, 19, 0x15, NULL},
    {"filter_missed_attacks_by_party", "cfilter", kValueBool, kStoreFilter,
     0x00, -1, kCommits, kFromFilter, 0, 20, 0x16, NULL},
    {"filter_attacks_evaded_by_party", "cfilter", kValueBool, kStoreFilter,
     0x00, -1, kCommits, kFromFilter, 0, 21, 0x17, NULL},
    {"filter_damage_taken_by_party", "cfilter", kValueBool, kStoreFilter,
     0x00, -1, kCommits, kFromFilter, 0, 22, 0x18, NULL},
    {"filter_special_actions_started_on_by_allies", "cfilter", kValueBool, kStoreFilter,
     0x00, -1, kCommits, kFromFilter, 0, 23, 0x1F, NULL},
    {"filter_special_action_effects_on_by_allies", "cfilter", kValueBool, kStoreFilter,
     0x00, -1, kCommits, kFromFilter, 0, 24, 0x19, NULL},
    {"filter_attacks_by_allies", "cfilter", kValueBool, kStoreFilter,
     0x00, -1, kCommits, kFromFilter, 0, 25, 0x1A, NULL},
    {"filter_missed_attacks_by_allies", "cfilter", kValueBool, kStoreFilter,
     0x00, -1, kCommits, kFromFilter, 0, 26, 0x1B, NULL},
    {"filter_attacks_evaded_by_allies", "cfilter", kValueBool, kStoreFilter,
     0x00, -1, kCommits, kFromFilter, 0, 27, 0x1C, NULL},
    {"filter_damage_taken_by_allies", "cfilter", kValueBool, kStoreFilter,
     0x00, -1, kCommits, kFromFilter, 0, 28, 0x1D, NULL},
    {"filter_special_actions_started_on_by_foes", "cfilter", kValueBool, kStoreFilter,
     0x00, -1, kCommits, kFromFilter, 0, 29, 0xB1, NULL},
    {"filter_special_action_effects_on_by_foes", "cfilter", kValueBool, kStoreFilter,
     0x00, -1, kCommits, kFromFilter, 0, 30, 0xB2, NULL},
    {"filter_attacks_by_foes", "cfilter", kValueBool, kStoreFilter,
     0x00, -1, kCommits, kFromFilter, 0, 31, 0xB3, NULL},
    {"filter_missed_attacks_by_foes", "cfilter", kValueBool, kStoreFilter,
     0x00, -1, kCommits, kFromFilter, 0, 32, 0xB4, NULL},
    {"filter_attacks_evaded_by_foes", "cfilter", kValueBool, kStoreFilter,
     0x00, -1, kCommits, kFromFilter, 0, 33, 0xB5, NULL},
    {"filter_damage_taken_by_foes", "cfilter", kValueBool, kStoreFilter,
     0x00, -1, kCommits, kFromFilter, 0, 34, 0xB6, NULL},
    {"filter_special_actions_started_on_by_others", "cfilter", kValueBool, kStoreFilter,
     0x00, -1, kCommits, kFromFilter, 0, 35, 0xAB, NULL},
    {"filter_special_action_effects_on_by_others", "cfilter", kValueBool, kStoreFilter,
     0x00, -1, kCommits, kFromFilter, 0, 36, 0xAC, NULL},
    {"filter_attacks_by_others", "cfilter", kValueBool, kStoreFilter,
     0x00, -1, kCommits, kFromFilter, 0, 37, 0xAD, NULL},
    {"filter_missed_attacks_by_others", "cfilter", kValueBool, kStoreFilter,
     0x00, -1, kCommits, kFromFilter, 0, 38, 0xAE, NULL},
    {"filter_attacks_evaded_by_others", "cfilter", kValueBool, kStoreFilter,
     0x00, -1, kCommits, kFromFilter, 0, 39, 0xAF, NULL},
    {"filter_damage_taken_by_others", "cfilter", kValueBool, kStoreFilter,
     0x00, -1, kCommits, kFromFilter, 0, 40, 0xB0, NULL},
    {"filter_special_actions_started_on_by_npcs", "cfilter", kValueBool, kStoreFilter,
     0x00, -1, kCommits, kFromFilter, 0, 41, 0x20, NULL},
    {"filter_special_action_effects_on_by_npcs", "cfilter", kValueBool, kStoreFilter,
     0x00, -1, kCommits, kFromFilter, 0, 42, 0x11, NULL},
    {"filter_attacks_by_npcs", "cfilter", kValueBool, kStoreFilter,
     0x00, -1, kCommits, kFromFilter, 0, 43, 0x12, NULL},
    {"filter_missed_attacks_by_npcs", "cfilter", kValueBool, kStoreFilter,
     0x00, -1, kCommits, kFromFilter, 0, 44, 0x13, NULL},
    {"filter_others_synthesis_and_fishing_results", "cfilter", kValueBool, kStoreFilter,
     0x00, -1, kCommits, kFromFilter, 0, 45, 0x58, NULL},
    {"filter_lot_results", "cfilter", kValueBool, kStoreFilter, 0x00, -1, kCommits, kFromFilter, 0, 46, 0x90, NULL},
    {"filter_campaign_related_data", "cfilter", kValueBool, kStoreFilter,
     0x00, -1, kCommits, kFromFilter, 0, 47, 0xBF, NULL},
    {"filter_tell_messages_deemed_spam", "cfilter", kValueBool, kStoreFilter,
     0x00, -1, kCommits, kFromFilter, 0, 48, 0xC0, NULL},
    {"filter_shout_yell_messages_deemed_spam", "cfilter", kValueBool, kStoreFilter,
     0x00, -1, kCommits, kFromFilter, 0, 49, 0xC1, NULL},
    {"filter_job_specific_emote", "cfilter", kValueBool, kStoreFilter,
     0x00, -1, kCommits, kFromFilter, 0, 50, 0xC2, NULL},
    {"filter_messages_from_alter_egos", "cfilter", kValueNumber, kStoreFilter,
     0x00, -1, kCommits, kFromFilter, 0, 51, 0x0A, NULL},
    {"filter_system_level", "cfilter", kValueNumber, kStoreWord, 0x35, -1, kCommits, kFromFilter, 0, 52, -1, NULL},
    {"color_say", "conftxtc", kValueColor, kStoreWord, 0x1B, -1, 0, kFromColors, 0, 0, 0x24, NULL},
    {"color_tell", "conftxtc", kValueColor, kStoreWord, 0x1D, -1, 0, kFromColors, 0, 1, 0x26, NULL},
    {"color_party", "conftxtc", kValueColor, kStoreWord, 0x1E, -1, 0, kFromColors, 0, 2, 0x27, NULL},
    {"color_linkshell", "conftxtc", kValueColor, kStoreWord, 0x1F, -1, 0, kFromColors, 0, 3, 0x28, NULL},
    {"color_linkshell_2", "conftxtc", kValueColor, kStoreWord, 0xB3, -1, 0, kFromColors, 0, 4, 0x29, NULL},
    {"color_assist_j", "conftxtc", kValueColor, kStoreWord, 0xCB, -1, 0, kFromColors, 0, 5, 0x2A, NULL},
    {"color_assist_e", "conftxtc", kValueColor, kStoreWord, 0xCC, -1, 0, kFromColors, 0, 6, 0x2B, NULL},
    {"color_unity", "conftxtc", kValueColor, kStoreWord, 0xB0, -1, 0, kFromColors, 0, 7, 0x2C, NULL},
    {"color_emotes", "conftxtc", kValueColor, kStoreWord, 0x20, -1, 0, kFromColors, 0, 8, 0x2D, NULL},
    {"color_message", "conftxtc", kValueColor, kStoreWord, 0x21, -1, 0, kFromColors, 0, 9, 0x2E, NULL},
    {"color_npc_conversations", "conftxtc", kValueColor, kStoreWord, 0x22, -1, 0, kFromColors, 0, 10, 0x2F, NULL},
    {"color_shout", "conftxtc", kValueColor, kStoreWord, 0x1C, -1, 0, kFromColors, 0, 11, 0x25, NULL},
    {"color_yell", "conftxtc", kValueColor, kStoreWord, 0x9B, -1, 0, kFromColors, 0, 12, 0xC4, NULL},
    {"color_hp_mp_you_recover", "conftxtc", kValueColor, kStoreWord, 0x23, -1, 0, kFromColors, 1, 0, 0x30, NULL},
    {"color_hp_mp_you_lose", "conftxtc", kValueColor, kStoreWord, 0x24, -1, 0, kFromColors, 1, 1, 0x31, NULL},
    {"color_beneficial_effects_you_are_granted", "conftxtc", kValueColor, kStoreWord,
     0x25, -1, 0, kFromColors, 1, 2, 0x32, NULL},
    {"color_detrimental_effects_you_receive", "conftxtc", kValueColor, kStoreWord,
     0x26, -1, 0, kFromColors, 1, 3, 0x33, NULL},
    {"color_effects_you_resist", "conftxtc", kValueColor, kStoreWord, 0x27, -1, 0, kFromColors, 1, 4, 0x34, NULL},
    {"color_actions_you_evade", "conftxtc", kValueColor, kStoreWord, 0x28, -1, 0, kFromColors, 1, 5, 0x35, NULL},
    {"color_hp_mp_others_recover", "conftxtc", kValueColor, kStoreWord, 0x29, -1, 0, kFromColors, 2, 0, 0x36, NULL},
    {"color_hp_mp_others_lose", "conftxtc", kValueColor, kStoreWord, 0x2A, -1, 0, kFromColors, 2, 1, 0x37, NULL},
    {"color_beneficial_effects_others_are_granted", "conftxtc", kValueColor, kStoreWord,
     0x2B, -1, 0, kFromColors, 2, 2, 0x38, NULL},
    {"color_detrimental_effects_others_receive", "conftxtc", kValueColor, kStoreWord,
     0x2C, -1, 0, kFromColors, 2, 3, 0x39, NULL},
    {"color_effects_others_resist", "conftxtc", kValueColor, kStoreWord, 0x2D, -1, 0, kFromColors, 2, 4, 0x3A, NULL},
    {"color_actions_others_evade", "conftxtc", kValueColor, kStoreWord, 0x2E, -1, 0, kFromColors, 2, 5, 0x3B, NULL},
    {"color_standard_battle_messages", "conftxtc", kValueColor, kStoreWord,
     0x2F, -1, 0, kFromColors, 3, 0, 0x3C, NULL},
    {"color_calls_for_help", "conftxtc", kValueColor, kStoreWord, 0x30, -1, 0, kFromColors, 3, 1, 0x3D, NULL},
    {"color_basic_system_messages", "conftxtc", kValueColor, kStoreWord, 0x31, -1, 0, kFromColors, 3, 2, 0x3E, NULL},
    {"log_multi_window", "conf5win", kValueChoice, kStoreWord,
     0xB4, -1, 0, kFromNone, 0, 0, -1, kLogMultiWindowChoices},
    {"timestamp", "conf5win", kValueChoice, kStoreWord, 0xB5, -1, 0, kFromNone, 0, 0, -1, kTimestampChoices},
    {"window_type", "conf5win", kValueNumber, kStoreWord, 0x12, -1, 0, kFromNone, 0, 0, -1, NULL},
    {"window_effect", "conf5win", kValueBool, kStoreWord, 0xA4, -1, kInverted, kFromNone, 0, 0, -1, NULL},
    {"log_window1_reactive_sizing", "conf5w1", kValueBool, kStoreWord, 0x0E, -1, kInverted, kFromNone, 0, 0, -1, NULL},
    {"log_window1_max_lines", "conf5w1", kValueNumber, kStoreWord, 0x0F, -1, 0, kFromNone, 0, 0, -1, NULL},
    {"log_window1_min_lines", "conf5w1", kValueNumber, kStoreWord, 0x10, -1, 0, kFromNone, 0, 0, -1, NULL},
    {"log_window1_width", "conf5w1", kValueNumber, kStoreWord, 0x9C, -1, 0, kFromNone, 0, 0, -1, NULL},
    {"log_window1_resize_time", "conf5w1", kValueNumber, kStoreWord, 0x11, -1, 0, kFromNone, 0, 0, -1, NULL},
    {"log_window2_reactive_sizing", "conf5w2", kValueBool, kStoreWord, 0xB6, -1, kInverted, kFromNone, 0, 0, -1, NULL},
    {"log_window2_max_lines", "conf5w2", kValueNumber, kStoreWord, 0xB7, -1, 0, kFromNone, 0, 0, -1, NULL},
    {"log_window2_min_lines", "conf5w2", kValueNumber, kStoreWord, 0xB8, -1, 0, kFromNone, 0, 0, -1, NULL},
    {"log_window2_width", "conf5w2", kValueNumber, kStoreWord, 0xC5, -1, 0, kFromNone, 0, 0, -1, NULL},
    {"log_window2_resize_time", "conf5w2", kValueNumber, kStoreWord, 0xC6, -1, 0, kFromNone, 0, 0, -1, NULL},
    {"damage_display", "conf3win", kValueChoice, kStoreWord, 0x13, -1, 0, kFromNone, 0, 0, -1, kDamageDisplayChoices},
    {"character_information", "conf3win", kValueChoice, kStoreFlag,
     0x00, -1, kCommits, kFromNone, 0, 0, -1, kShowHideChoices},
    {"shadows", "conf3win", kValueChoice, kStoreWord, 0x3A, -1, 0, kFromNone, 0, 0, -1, kShadowChoices},
    {"weather_effects", "conf3win", kValueBool, kStoreWord, 0x3B, -1, 0, kFromNone, 0, 0, -1, NULL},
    {"character_models_displayed", "conf3win", kValueNumber, kStoreWord, 0x3C, -1, 0, kFromNone, 0, 0, -1, NULL},
    {"icon_type", "conf3win", kValueChoice, kStoreBit, 0x99, 2, 0, kFromNone, 0, 0, -1, kIconTypeChoices},
    {"pc_armor_display", "conf3win", kValueChoice, kStoreWord, 0xA5, -1, 0, kFromNone, 0, 0, -1, kNormalStaticChoices},
    {"clipping_plane", "conf6win", kValueNumber, kStoreWord, 0x3E, -1, 0, kFromNone, 0, 0, -1, NULL},
    {"footstep_effects", "conf6win", kValueBool, kStoreWord, 0x3F, -1, kInverted, kFromNone, 0, 0, -1, NULL},
    {"animation_frame_rate", "conf6win", kValueNumber, kStoreWord, 0x40, -1, 0, kFromNone, 0, 0, -1, NULL},
    {"keyboard_size", "conf6win", kValueChoice, kStoreWord, 0x42, -1, 0, kFromNone, 0, 0, -1, kKeyboardSizeChoices},
    {"background_aspect_ratio", "conf6win", kValueNumber, kStoreWord, 0x43, -1, 0, kFromNone, 0, 0, -1, NULL},
    {"macro_palette_size", "conf6win", kValueChoice, kStoreWord,
     0x9D, -1, 0, kFromNone, 0, 0, -1, kPaletteSizeChoices},
    {"macro_palette_position", "conf6win", kValueChoice, kStoreWord,
     0x9E, -1, 0, kFromNone, 0, 0, -1, kPalettePositionChoices},
    {"key1_move_up", "k1assign", kValueKey, kStoreWord, 0x4A, -1, kCommits, kFromKeys, 0, 0, 0x59, NULL},
    {"key1_move_down", "k1assign", kValueKey, kStoreWord, 0x4B, -1, kCommits, kFromKeys, 0, 1, 0x5A, NULL},
    {"key1_move_left", "k1assign", kValueKey, kStoreWord, 0x4C, -1, kCommits, kFromKeys, 0, 2, 0x5C, NULL},
    {"key1_move_right", "k1assign", kValueKey, kStoreWord, 0x4D, -1, kCommits, kFromKeys, 0, 3, 0x5B, NULL},
    {"key1_menu_cursor_camera_up", "k1assign", kValueKey, kStoreWord, 0x4E, -1, kCommits, kFromKeys, 0, 4, 0x65, NULL},
    {"key1_menu_cursor_camera_down", "k1assign", kValueKey, kStoreWord,
     0x4F, -1, kCommits, kFromKeys, 0, 5, 0x66, NULL},
    {"key1_menu_cursor_camera_left", "k1assign", kValueKey, kStoreWord,
     0x50, -1, kCommits, kFromKeys, 0, 6, 0x68, NULL},
    {"key1_menu_cursor_camera_right", "k1assign", kValueKey, kStoreWord,
     0x51, -1, kCommits, kFromKeys, 0, 7, 0x67, NULL},
    {"key1_move_camera_up", "k1assign", kValueKey, kStoreWord, 0x52, -1, kCommits, kFromKeys, 0, 8, 0x5D, NULL},
    {"key1_move_camera_down", "k1assign", kValueKey, kStoreWord, 0x53, -1, kCommits, kFromKeys, 0, 9, 0x5E, NULL},
    {"key1_move_camera_left", "k1assign", kValueKey, kStoreWord, 0x54, -1, kCommits, kFromKeys, 0, 10, 0x60, NULL},
    {"key1_move_camera_right", "k1assign", kValueKey, kStoreWord, 0x55, -1, kCommits, kFromKeys, 0, 11, 0x5F, NULL},
    {"key1_zoom_in", "k1assign", kValueKey, kStoreWord, 0x56, -1, kCommits, kFromKeys, 0, 12, 0x89, NULL},
    {"key1_zoom_out", "k1assign", kValueKey, kStoreWord, 0x57, -1, kCommits, kFromKeys, 0, 13, 0x88, NULL},
    {"key1_menu_target_cursor_up", "k1assign", kValueKey, kStoreWord,
     0x58, -1, kCommits, kFromKeys, 0, 14, 0x61, NULL},
    {"key1_menu_target_cursor_down", "k1assign", kValueKey, kStoreWord,
     0x59, -1, kCommits, kFromKeys, 0, 15, 0x62, NULL},
    {"key1_menu_target_cursor_left", "k1assign", kValueKey, kStoreWord,
     0x5A, -1, kCommits, kFromKeys, 0, 16, 0x64, NULL},
    {"key1_menu_target_cursor_right", "k1assign", kValueKey, kStoreWord,
     0x5B, -1, kCommits, kFromKeys, 0, 17, 0x63, NULL},
    {"key1_confirm", "k1assign", kValueKey, kStoreWord, 0x5C, -1, kCommits, kFromKeys, 0, 18, 0x69, NULL},
    {"key1_cancel", "k1assign", kValueKey, kStoreWord, 0x5D, -1, kCommits, kFromKeys, 0, 19, 0x6A, NULL},
    {"key1_change_view", "k1assign", kValueKey, kStoreWord, 0x5E, -1, kCommits, kFromKeys, 0, 20, 0x6B, NULL},
    {"key1_autorun", "k1assign", kValueKey, kStoreWord, 0x5F, -1, kCommits, kFromKeys, 0, 21, 0x6C, NULL},
    {"key1_heal", "k1assign", kValueKey, kStoreWord, 0x60, -1, kCommits, kFromKeys, 0, 22, 0x6D, NULL},
    {"key1_select_window", "k1assign", kValueKey, kStoreWord, 0x61, -1, kCommits, kFromKeys, 0, 23, 0x6E, NULL},
    {"key1_open_main_menu", "k1assign", kValueKey, kStoreWord, 0x62, -1, kCommits, kFromKeys, 0, 24, 0x6F, NULL},
    {"key1_toggle_walk_run", "k1assign", kValueKey, kStoreWord, 0x63, -1, kCommits, kFromKeys, 0, 25, 0x8A, NULL},
    {"key1_hide_show_window", "k1assign", kValueKey, kStoreWord, 0x64, -1, kCommits, kFromKeys, 0, 26, 0x8E, NULL},
    {"key1_take_screenshot", "k1assign", kValueKey, kStoreWord, 0x65, -1, kCommits, kFromKeys, 0, 27, 0x8D, NULL},
    {"key1_turn_right", "k1assign", kValueKey, kStoreWord, 0x66, -1, kCommits, kFromKeys, 0, 28, 0xC6, NULL},
    {"key1_turn_left", "k1assign", kValueKey, kStoreWord, 0x67, -1, kCommits, kFromKeys, 0, 29, 0xC7, NULL},
    {"key2_attack", "k2assign", kValueKey, kStoreWord, 0x6A, -1, kCommits, kFromKeys, 1, 0, 0x70, NULL},
    {"key2_items", "k2assign", kValueKey, kStoreWord, 0x6B, -1, kCommits, kFromKeys, 1, 1, 0x71, NULL},
    {"key2_magic", "k2assign", kValueKey, kStoreWord, 0x6C, -1, kCommits, kFromKeys, 1, 2, 0x72, NULL},
    {"key2_abilities", "k2assign", kValueKey, kStoreWord, 0x6D, -1, kCommits, kFromKeys, 1, 3, 0x73, NULL},
    {"key2_weapon_skills", "k2assign", kValueKey, kStoreWord, 0x6E, -1, kCommits, kFromKeys, 1, 4, 0x74, NULL},
    {"key2_pet_commands", "k2assign", kValueKey, kStoreWord, 0x6F, -1, kCommits, kFromKeys, 1, 5, 0x85, NULL},
    {"key2_confirm", "k2assign", kValueKey, kStoreWord, 0x70, -1, kCommits, kFromKeys, 1, 6, 0x86, NULL},
    {"key2_cancel", "k2assign", kValueKey, kStoreWord, 0x71, -1, kCommits, kFromKeys, 1, 7, 0x87, NULL},
    {"key2_open_main_menu", "k2assign", kValueKey, kStoreWord, 0x72, -1, kCommits, kFromKeys, 1, 8, 0x8C, NULL},
    {"key2_select_window", "k2assign", kValueKey, kStoreWord, 0x73, -1, kCommits, kFromKeys, 1, 9, 0x8B, NULL},
    {"key2_hide_show_window", "k2assign", kValueKey, kStoreWord, 0x74, -1, kCommits, kFromKeys, 1, 10, 0x8E, NULL},
    {"key2_take_screenshot", "k2assign", kValueKey, kStoreWord, 0x75, -1, kCommits, kFromKeys, 1, 11, 0x8D, NULL},
    {"key2_toggle_walk_run", "k2assign", kValueKey, kStoreWord, 0x76, -1, kCommits, kFromKeys, 1, 12, 0x8A, NULL},
    {"key2_zoom_in", "k2assign", kValueKey, kStoreWord, 0x77, -1, kCommits, kFromKeys, 1, 13, 0x89, NULL},
    {"key2_zoom_out", "k2assign", kValueKey, kStoreWord, 0x78, -1, kCommits, kFromKeys, 1, 14, 0x88, NULL},
    {"key2_ranged_attack", "k2assign", kValueKey, kStoreWord, 0x79, -1, kCommits, kFromKeys, 1, 15, 0x75, NULL},
    {"key2_call_for_help", "k2assign", kValueKey, kStoreWord, 0x7A, -1, kCommits, kFromKeys, 1, 16, 0x76, NULL},
    {"key2_equipment", "k2assign", kValueKey, kStoreWord, 0x7B, -1, kCommits, kFromKeys, 1, 17, 0x77, NULL},
    {"key2_change_view", "k2assign", kValueKey, kStoreWord, 0x7C, -1, kCommits, kFromKeys, 1, 18, 0x78, NULL},
    {"key2_heal", "k2assign", kValueKey, kStoreWord, 0x7D, -1, kCommits, kFromKeys, 1, 19, 0x79, NULL},
    {"key2_check", "k2assign", kValueKey, kStoreWord, 0x7E, -1, kCommits, kFromKeys, 1, 20, 0x7A, NULL},
    {"key2_chat_say", "k2assign", kValueKey, kStoreWord, 0x7F, -1, kCommits, kFromKeys, 1, 21, 0x7B, NULL},
    {"key2_chat_party", "k2assign", kValueKey, kStoreWord, 0x80, -1, kCommits, kFromKeys, 1, 22, 0x7C, NULL},
    {"key2_chat_tell", "k2assign", kValueKey, kStoreWord, 0x81, -1, kCommits, kFromKeys, 1, 23, 0x7D, NULL},
    {"key2_chat_linkshell", "k2assign", kValueKey, kStoreWord, 0x82, -1, kCommits, kFromKeys, 1, 24, 0x7E, NULL},
    {"key2_chat_reply_to_tell", "k2assign", kValueKey, kStoreWord, 0x83, -1, kCommits, kFromKeys, 1, 25, 0x82, NULL},
    {"key2_chat_unity", "k2assign", kValueKey, kStoreWord, 0x84, -1, kCommits, kFromKeys, 1, 26, 0x81, NULL},
    {"key2_chat_linkshell2", "k2assign", kValueKey, kStoreWord, 0x85, -1, kCommits, kFromKeys, 1, 27, 0x7F, NULL},
    {"key2_chat_assiste_or_assistj", "k2assign", kValueKey, kStoreWord,
     0x86, -1, kCommits, kFromKeys, 1, 28, 0x80, NULL},
    {"effect_filter_all_effects_during_battle", "fxfilter", kValueBool, kStoreBit,
     0x92, -1, kCommits, kFromEffects, 0, 0, 0x99, NULL},
    {"effect_filter_you_to_you", "fxfilter", kValueBool, kStoreBit,
     0x92, -1, kCommits, kFromEffects, 0, 1, 0x9A, NULL},
    {"effect_filter_you_to_monster", "fxfilter", kValueBool, kStoreBit,
     0x92, -1, kCommits, kFromEffects, 0, 2, 0x9B, NULL},
    {"effect_filter_you_to_party", "fxfilter", kValueBool, kStoreBit,
     0x92, -1, kCommits, kFromEffects, 0, 3, 0x9C, NULL},
    {"effect_filter_you_to_non_party_pc", "fxfilter", kValueBool, kStoreBit,
     0x92, -1, kCommits, kFromEffects, 0, 4, 0x9D, NULL},
    {"effect_filter_monster_to_you", "fxfilter", kValueBool, kStoreBit,
     0x92, -1, kCommits, kFromEffects, 0, 5, 0x9E, NULL},
    {"effect_filter_monster_to_monster", "fxfilter", kValueBool, kStoreBit,
     0x92, -1, kCommits, kFromEffects, 0, 6, 0x9F, NULL},
    {"effect_filter_monster_to_party", "fxfilter", kValueBool, kStoreBit,
     0x92, -1, kCommits, kFromEffects, 0, 7, 0xA0, NULL},
    {"effect_filter_monster_to_non_party_pc", "fxfilter", kValueBool, kStoreBit,
     0x92, -1, kCommits, kFromEffects, 0, 8, 0xA1, NULL},
    {"effect_filter_party_to_you", "fxfilter", kValueBool, kStoreBit,
     0x92, -1, kCommits, kFromEffects, 0, 9, 0xA2, NULL},
    {"effect_filter_party_to_monster", "fxfilter", kValueBool, kStoreBit,
     0x92, -1, kCommits, kFromEffects, 0, 10, 0xA3, NULL},
    {"effect_filter_party_to_party", "fxfilter", kValueBool, kStoreBit,
     0x92, -1, kCommits, kFromEffects, 0, 11, 0xA4, NULL},
    {"effect_filter_party_to_non_party_pc", "fxfilter", kValueBool, kStoreBit,
     0x92, -1, kCommits, kFromEffects, 0, 12, 0xA5, NULL},
    {"effect_filter_non_party_pc_to_you", "fxfilter", kValueBool, kStoreBit,
     0x92, -1, kCommits, kFromEffects, 0, 13, 0xA6, NULL},
    {"effect_filter_non_party_pc_to_monster", "fxfilter", kValueBool, kStoreBit,
     0x92, -1, kCommits, kFromEffects, 0, 14, 0xA7, NULL},
    {"effect_filter_non_party_pc_to_party", "fxfilter", kValueBool, kStoreBit,
     0x92, -1, kCommits, kFromEffects, 0, 15, 0xA8, NULL},
    {"effect_filter_non_party_pc_to_non_party_pc", "fxfilter", kValueBool, kStoreBit,
     0x92, -1, kCommits, kFromEffects, 0, 16, 0xA9, NULL},
    {"effect_filter_screen_shaking", "fxfilter", kValueBool, kStoreBit,
     0x92, -1, kCommits, kFromEffects, 0, 17, 0xAA, NULL},
    {"mouse_control", "conf7", kValueChoice, kStoreWord, 0x90, -1, 0, kFromNone, 0, 0, -1, kTypeChoices},
    {"screen_edge_panning", "conf7", kValueBool, kStoreWord, 0x8F, -1, kInverted, kFromNone, 0, 0, -1, NULL},
    {"camera_view", "conf7", kValueChoice, kStoreWord, 0x91, -1, 0, kFromNone, 0, 0, -1, kCameraViewChoices},
    {"third_person_y_axis", "conf7", kValueChoice, kStoreWord, 0x8C, -1, 0, kFromNone, 0, 0, -1, kAxisChoices},
    {"third_person_x_axis", "conf7", kValueChoice, kStoreWord, 0x8B, -1, 0, kFromNone, 0, 0, -1, kAxisChoices},
    {"first_person_y_axis", "conf7", kValueChoice, kStoreWord, 0x8E, -1, 0, kFromNone, 0, 0, -1, kAxisChoices},
    {"first_person_x_axis", "conf7", kValueChoice, kStoreWord, 0x8D, -1, 0, kFromNone, 0, 0, -1, kAxisChoices},
    {"chat_language_filter", "conf4", kValueBool, kStoreWord, 0x18, -1, kInverted, kFromNone, 0, 0, -1, NULL},
    {"auto_disconnect", "conf4", kValueNumber, kStoreIdle, 0x41, -1, 0, kFromNone, 0, 0, -1, NULL},
    {"log1_say", "conf11s", kValueBool, kStoreBit, 0xB9, -1, kCommits, kFromRouting, 0, 0, 0x24, NULL},
    {"log1_tell", "conf11s", kValueBool, kStoreBit, 0xB9, -1, kCommits, kFromRouting, 0, 1, 0x26, NULL},
    {"log1_party", "conf11s", kValueBool, kStoreBit, 0xB9, -1, kCommits, kFromRouting, 0, 2, 0x27, NULL},
    {"log1_linkshell", "conf11s", kValueBool, kStoreBit, 0xB9, -1, kCommits, kFromRouting, 0, 3, 0x28, NULL},
    {"log1_linkshell_2", "conf11s", kValueBool, kStoreBit, 0xB9, -1, kCommits, kFromRouting, 0, 4, 0x29, NULL},
    {"log1_assist_j", "conf11s", kValueBool, kStoreBit, 0xB9, -1, kCommits, kFromRouting, 0, 5, 0x2A, NULL},
    {"log1_assist_e", "conf11s", kValueBool, kStoreBit, 0xB9, -1, kCommits, kFromRouting, 0, 6, 0x2B, NULL},
    {"log1_unity", "conf11s", kValueBool, kStoreBit, 0xB9, -1, kCommits, kFromRouting, 0, 7, 0x2C, NULL},
    {"log1_emotes", "conf11s", kValueBool, kStoreBit, 0xB9, -1, kCommits, kFromRouting, 0, 8, 0x2D, NULL},
    {"log1_message", "conf11s", kValueBool, kStoreBit, 0xB9, -1, kCommits, kFromRouting, 0, 9, 0x2E, NULL},
    {"log1_npc_conversations", "conf11s", kValueBool, kStoreBit, 0xB9, -1, kCommits, kFromRouting, 0, 10, 0x2F, NULL},
    {"log1_shout", "conf11s", kValueBool, kStoreBit, 0xB9, -1, kCommits, kFromRouting, 0, 11, 0x25, NULL},
    {"log1_yell", "conf11s", kValueBool, kStoreBit, 0xB9, -1, kCommits, kFromRouting, 0, 12, 0xC4, NULL},
    {"log1_hp_mp_you_recover", "conf11s", kValueBool, kStoreBit, 0xB9, -1, kCommits, kFromRouting, 1, 0, 0x30, NULL},
    {"log1_hp_mp_you_lose", "conf11s", kValueBool, kStoreBit, 0xB9, -1, kCommits, kFromRouting, 1, 1, 0x31, NULL},
    {"log1_beneficial_effects_you_are_granted", "conf11s", kValueBool, kStoreBit,
     0xB9, -1, kCommits, kFromRouting, 1, 2, 0x32, NULL},
    {"log1_detrimental_effects_you_receive", "conf11s", kValueBool, kStoreBit,
     0xB9, -1, kCommits, kFromRouting, 1, 3, 0x33, NULL},
    {"log1_effects_you_resist", "conf11s", kValueBool, kStoreBit, 0xB9, -1, kCommits, kFromRouting, 1, 4, 0x34, NULL},
    {"log1_actions_you_evade", "conf11s", kValueBool, kStoreBit, 0xB9, -1, kCommits, kFromRouting, 1, 5, 0x35, NULL},
    {"log1_hp_mp_others_recover", "conf11s", kValueBool, kStoreBit,
     0xB9, -1, kCommits, kFromRouting, 2, 0, 0x36, NULL},
    {"log1_hp_mp_others_lose", "conf11s", kValueBool, kStoreBit, 0xB9, -1, kCommits, kFromRouting, 2, 1, 0x37, NULL},
    {"log1_beneficial_effects_others_are_granted", "conf11s", kValueBool, kStoreBit,
     0xB9, -1, kCommits, kFromRouting, 2, 2, 0x38, NULL},
    {"log1_detrimental_effects_others_receive", "conf11s", kValueBool, kStoreBit,
     0xB9, -1, kCommits, kFromRouting, 2, 3, 0x39, NULL},
    {"log1_effects_others_resist", "conf11s", kValueBool, kStoreBit,
     0xB9, -1, kCommits, kFromRouting, 2, 4, 0x3A, NULL},
    {"log1_actions_others_evade", "conf11s", kValueBool, kStoreBit,
     0xB9, -1, kCommits, kFromRouting, 2, 5, 0x3B, NULL},
    {"log1_battle_messages", "conf11s", kValueBool, kStoreBit, 0xB9, -1, kCommits, kFromRouting, 3, 0, 0xCA, NULL},
    {"log1_calls_for_help", "conf11s", kValueBool, kStoreBit, 0xB9, -1, kCommits, kFromRouting, 3, 1, 0xCB, NULL},
    {"log1_basic_system_messages", "conf11s", kValueBool, kStoreBit,
     0xB9, -1, kCommits, kFromRouting, 3, 2, 0x3E, NULL},
    {"log2_say", "conf11s", kValueBool, kStoreBit, 0xBB, -1, kCommits, kFromRouting, 0, 0, 0x24, NULL},
    {"log2_tell", "conf11s", kValueBool, kStoreBit, 0xBB, -1, kCommits, kFromRouting, 0, 1, 0x26, NULL},
    {"log2_party", "conf11s", kValueBool, kStoreBit, 0xBB, -1, kCommits, kFromRouting, 0, 2, 0x27, NULL},
    {"log2_linkshell", "conf11s", kValueBool, kStoreBit, 0xBB, -1, kCommits, kFromRouting, 0, 3, 0x28, NULL},
    {"log2_linkshell_2", "conf11s", kValueBool, kStoreBit, 0xBB, -1, kCommits, kFromRouting, 0, 4, 0x29, NULL},
    {"log2_assist_j", "conf11s", kValueBool, kStoreBit, 0xBB, -1, kCommits, kFromRouting, 0, 5, 0x2A, NULL},
    {"log2_assist_e", "conf11s", kValueBool, kStoreBit, 0xBB, -1, kCommits, kFromRouting, 0, 6, 0x2B, NULL},
    {"log2_unity", "conf11s", kValueBool, kStoreBit, 0xBB, -1, kCommits, kFromRouting, 0, 7, 0x2C, NULL},
    {"log2_emotes", "conf11s", kValueBool, kStoreBit, 0xBB, -1, kCommits, kFromRouting, 0, 8, 0x2D, NULL},
    {"log2_message", "conf11s", kValueBool, kStoreBit, 0xBB, -1, kCommits, kFromRouting, 0, 9, 0x2E, NULL},
    {"log2_npc_conversations", "conf11s", kValueBool, kStoreBit, 0xBB, -1, kCommits, kFromRouting, 0, 10, 0x2F, NULL},
    {"log2_shout", "conf11s", kValueBool, kStoreBit, 0xBB, -1, kCommits, kFromRouting, 0, 11, 0x25, NULL},
    {"log2_yell", "conf11s", kValueBool, kStoreBit, 0xBB, -1, kCommits, kFromRouting, 0, 12, 0xC4, NULL},
    {"log2_hp_mp_you_recover", "conf11s", kValueBool, kStoreBit, 0xBB, -1, kCommits, kFromRouting, 1, 0, 0x30, NULL},
    {"log2_hp_mp_you_lose", "conf11s", kValueBool, kStoreBit, 0xBB, -1, kCommits, kFromRouting, 1, 1, 0x31, NULL},
    {"log2_beneficial_effects_you_are_granted", "conf11s", kValueBool, kStoreBit,
     0xBB, -1, kCommits, kFromRouting, 1, 2, 0x32, NULL},
    {"log2_detrimental_effects_you_receive", "conf11s", kValueBool, kStoreBit,
     0xBB, -1, kCommits, kFromRouting, 1, 3, 0x33, NULL},
    {"log2_effects_you_resist", "conf11s", kValueBool, kStoreBit, 0xBB, -1, kCommits, kFromRouting, 1, 4, 0x34, NULL},
    {"log2_actions_you_evade", "conf11s", kValueBool, kStoreBit, 0xBB, -1, kCommits, kFromRouting, 1, 5, 0x35, NULL},
    {"log2_hp_mp_others_recover", "conf11s", kValueBool, kStoreBit,
     0xBB, -1, kCommits, kFromRouting, 2, 0, 0x36, NULL},
    {"log2_hp_mp_others_lose", "conf11s", kValueBool, kStoreBit, 0xBB, -1, kCommits, kFromRouting, 2, 1, 0x37, NULL},
    {"log2_beneficial_effects_others_are_granted", "conf11s", kValueBool, kStoreBit,
     0xBB, -1, kCommits, kFromRouting, 2, 2, 0x38, NULL},
    {"log2_detrimental_effects_others_receive", "conf11s", kValueBool, kStoreBit,
     0xBB, -1, kCommits, kFromRouting, 2, 3, 0x39, NULL},
    {"log2_effects_others_resist", "conf11s", kValueBool, kStoreBit,
     0xBB, -1, kCommits, kFromRouting, 2, 4, 0x3A, NULL},
    {"log2_actions_others_evade", "conf11s", kValueBool, kStoreBit,
     0xBB, -1, kCommits, kFromRouting, 2, 5, 0x3B, NULL},
    {"log2_battle_messages", "conf11s", kValueBool, kStoreBit, 0xBB, -1, kCommits, kFromRouting, 3, 0, 0xCA, NULL},
    {"log2_calls_for_help", "conf11s", kValueBool, kStoreBit, 0xBB, -1, kCommits, kFromRouting, 3, 1, 0xCB, NULL},
    {"log2_basic_system_messages", "conf11s", kValueBool, kStoreBit,
     0xBB, -1, kCommits, kFromRouting, 3, 2, 0x3E, NULL},
    {"weapon_effect", "conf12wi", kValueBool, kStoreWord, 0xBE, -1, kInverted, kFromNone, 0, 0, -1, NULL},
    {"style_lock", "conf12wi", kValueChoice, kStoreLock, 0xBF, -1, 0, kFromNone, 0, 0, -1, kNormalStaticChoices},
    {"area_display", "conf12wi", kValueBool, kStoreWord, 0xC0, -1, 0, kFromNone, 0, 0, -1, NULL},
    {"target_expressions", "conf12wi", kValueBool, kStoreBit, 0x99, 7, kInverted, kFromNone, 0, 0, -1, NULL},
    {"party_icon_display", "conf12wi", kValueBool, kStoreWord, 0xC2, -1, 0, kFromNone, 0, 0, -1, NULL},
    {"status_icon_timer_display", "conf12wi", kValueBool, kStoreWord, 0xC3, -1, 0, kFromNone, 0, 0, -1, NULL},
    {"furniture_camera_collision", "conf12wi", kValueBool, kStoreBit, 0x99, 6, kInverted, kFromNone, 0, 0, -1, NULL},
    {"software_keyboard", "conf13wi", kValueChoice, kStoreWord,
     0xC9, -1, 0, kFromNone, 0, 0, -1, kSoftwareKeyboardChoices},
    {"term_filter", "conf13wi", kValueChoice, kStoreWord, 0xCE, -1, 0, kFromNone, 0, 0, -1, kTermFilterChoices},
};
const int kSettingCount = static_cast<int>(sizeof(kSettings) / sizeof(kSettings[0]));

// The setting by its name, in any case; -1 for none.
inline int find_setting(const char* name) {
    for (int i = 0; name && i < kSettingCount; ++i) {
        const char* a = kSettings[i].name;
        const char* b = name;
        while (*a && lower_ascii(*b) == *a) {
            ++a;
            ++b;
        }
        if (!*a && !*b) {
            return i;
        }
    }
    return -1;
}

// A color as the store keeps it: 0x80, then red, green and blue, as the
// game's color editor packs it.
inline uint32_t pack_color(uint32_t r, uint32_t g, uint32_t b) {
    return 0x80000000u | (r & 0xFF) << 16 | (g & 0xFF) << 8 | (b & 0xFF);
}
inline uint32_t color_red(uint32_t v) { return (v >> 16) & 0xFF; }
inline uint32_t color_green(uint32_t v) { return (v >> 8) & 0xFF; }
inline uint32_t color_blue(uint32_t v) { return v & 0xFF; }

// The word and mask of kStoreBit's bit `bit` from word `id`.
inline uint8_t bit_word(uint8_t id, int bit) {
    return static_cast<uint8_t>(id + bit / 32);
}
inline uint32_t bit_mask(int bit) {
    return 1u << (bit % 32);
}

// The chat filter page's words: two masks of kFilterBitsPerWord bits each,
// the second states' mask, the system level and Tell's flag, written back in
// this order as the page closes. A row of its table is a mask bit (none:
// kFilterNoBit), then a second state's bit (none: kFilterOneState; a level
// row's minus its level), its states 0 (neither), 1 (the mask bit), 2 (the
// second's); the row whose second state is kFilterFlagged's and that has no
// mask bit cycles through four, the second's bit and the flag word.
const uint8_t kFilterWords[5] = {0x32, 0x33, 0x34, 0x35, 0x47};
const int kFilterBitsPerWord = 30;
const uint8_t kFilterNoBit = 0x33;
const int8_t kFilterOneState = 0x0B;
const int8_t kFilterFlagged = 2;
const int kFilterRows = 55;

struct FilterRow {
    int8_t mask;                // the mask bit, 0..59; -1 for none
    int8_t second;              // the second state's bit, 0..31; -1 for none
    uint8_t flagged;
};

inline FilterRow filter_row(uint8_t bit, int8_t sub) {
    FilterRow r;
    r.mask = bit != kFilterNoBit ? static_cast<int8_t>(bit) : -1;
    r.second = sub != kFilterOneState && sub >= 0 ? sub : -1;
    r.flagged = r.mask < 0 && sub == kFilterFlagged ? 1 : 0;
    return r;
}

// The states a row cycles through.
inline int filter_states(const FilterRow& r) {
    return r.flagged ? 4 : r.mask >= 0 && r.second >= 0 ? 3 : 2;
}

struct FilterWords {
    uint32_t masks[2];
    uint32_t second;
    uint32_t flag;
};

inline bool filter_mask_set(const FilterRow& r, const FilterWords& w) {
    return r.mask >= 0 && (w.masks[r.mask / kFilterBitsPerWord] >> (r.mask % kFilterBitsPerWord) & 1);
}
inline bool filter_second_set(const FilterRow& r, const FilterWords& w) {
    return r.second >= 0 && (w.second >> r.second & 1);
}

// The row's state as its page reads it, numbered in the order its confirm
// cycles from 0.
inline int filter_state(const FilterRow& r, const FilterWords& w) {
    if (r.flagged) {
        return (filter_second_set(r, w) ? 1 : 0) + (w.flag ? 2 : 0);
    }
    if (filter_mask_set(r, w)) {
        return 1;
    }
    return filter_second_set(r, w) ? (r.mask >= 0 ? 2 : 1) : 0;
}

// The words with the row in `state`, every other bit kept.
inline void filter_compose(const FilterRow& r, int state, FilterWords* w) {
    bool mask = false;
    bool second = false;
    if (r.flagged) {
        second = (state & 1) != 0;
        w->flag = state >> 1 ? 1 : 0;
    } else if (r.mask >= 0) {
        mask = state == 1;
        second = state == 2;
    } else {
        second = state == 1;
    }
    if (r.mask >= 0) {
        uint32_t& m = w->masks[r.mask / kFilterBitsPerWord];
        const uint32_t bit = 1u << (r.mask % kFilterBitsPerWord);
        m = mask ? m | bit : m & ~bit;
    }
    if (r.second >= 0) {
        const uint32_t bit = 1u << r.second;
        w->second = second ? w->second | bit : w->second & ~bit;
    }
}

// ---------------------------------------------------------------------------
// Holds: which handles hide or block which names. Lua-thread state; the game
// thread reads only the published want words.

const uint8_t kHoldHide = 1;
const uint8_t kHoldBlock = 2;
const LONG kWantHidden = 1;
const LONG kWantBlocked = 2;

// The ability window's lists a block can name one of: the category its
// controller stores (game.h's kAbilityCategory), a bit each.
const int kCategoryBits = 32;

struct Handle {
    uint32_t generation;
    uint8_t active;
    char name[48];
    uint8_t holds[kMaxNames];
    uint8_t macros_hold;    // this handle blocks the macro keys
    uint32_t ability_block; // bit k: this handle blocks ability's list k
    uint32_t cursor;        // event ring position this handle has read up to
    uint32_t dropped;
};

struct HandleTable {
    Handle* slots;
    int capacity;
    int open;

    // Returns the slot, or -1 when the table could not grow.
    int claim(const char* name, uint32_t cursor) {
        int slot = -1;
        for (int i = 0; i < capacity; ++i) {
            if (!slots[i].active) {
                slot = i;
                break;
            }
        }
        if (slot < 0) {
            const int grown = capacity ? capacity * 2 : 4;
            Handle* bigger = static_cast<Handle*>(
                HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, sizeof(Handle) * grown));
            if (!bigger) {
                return -1;
            }
            if (slots) {
                memcpy(bigger, slots, sizeof(Handle) * capacity);
                HeapFree(GetProcessHeap(), 0, slots);
            }
            slot = capacity;
            slots = bigger;
            capacity = grown;
        }
        Handle& h = slots[slot];
        const uint32_t generation = h.generation + 1;
        memset(&h, 0, sizeof(h));
        h.generation = generation;
        h.active = 1;
        const size_t len = strlen(name);
        const size_t keep = len < sizeof(h.name) - 1 ? len : sizeof(h.name) - 1;
        memcpy(h.name, name, keep);
        h.cursor = cursor;
        ++open;
        return slot;
    }

    Handle* get(int slot, uint32_t generation) {
        if (slot < 0 || slot >= capacity) {
            return NULL;
        }
        Handle& h = slots[slot];
        return (h.active && h.generation == generation) ? &h : NULL;
    }

    // The handle's name, released or not, until its slot is claimed again;
    // NULL after.
    const char* name_of(int slot, uint32_t generation) const {
        if (slot < 0 || slot >= capacity || slots[slot].generation != generation) {
            return NULL;
        }
        return slots[slot].name;
    }

    void free_slot(int slot) {
        if (slot >= 0 && slot < capacity && slots[slot].active) {
            slots[slot].active = 0;
            --open;
        }
    }
};

struct Holds {
    uint32_t hide_count[kMaxNames];
    uint32_t block_count[kMaxNames];
    volatile LONG want[kMaxNames];
    uint32_t macros_count;          // handles blocking the macro keys
    volatile LONG macros_want;      // 1 while any does
    uint32_t ability_block_count[kCategoryBits];    // handles blocking each list of ability
    volatile LONG ability_want;     // bit k while any blocks list k

    LONG want_of(int n) const {
        LONG w = 0;
        if (hide_count[n] || block_count[n]) {
            w |= kWantHidden;
        }
        if (block_count[n]) {
            w |= kWantBlocked;
        }
        return w;
    }

    void publish(int n) {
        InterlockedExchange(&want[n], want_of(n));
    }

    // Adds or drops one handle's hold. The union is what is published.
    void set(Handle& h, int n, uint8_t bit, bool on) {
        const bool had = (h.holds[n] & bit) != 0;
        if (had == on) {
            return;
        }
        uint32_t* counter = (bit == kHoldHide) ? hide_count : block_count;
        if (on) {
            h.holds[n] |= bit;
            ++counter[n];
        } else {
            h.holds[n] &= static_cast<uint8_t>(~bit);
            --counter[n];
        }
        publish(n);
    }

    // The macro keys: a hold under no name, counted once per handle.
    void set_macros(Handle& h, bool on) {
        if ((h.macros_hold != 0) == on) {
            return;
        }
        h.macros_hold = on ? 1 : 0;
        if (on) {
            ++macros_count;
        } else {
            --macros_count;
        }
        InterlockedExchange(&macros_want, macros_count ? 1 : 0);
    }

    // One list of ability, `category` 0..kCategoryBits-1: a hold under the
    // name's category bit, counted once per handle.
    void set_ability(Handle& h, int category, bool on) {
        const uint32_t bit = 1u << category;
        if (((h.ability_block & bit) != 0) == on) {
            return;
        }
        if (on) {
            h.ability_block |= bit;
            ++ability_block_count[category];
        } else {
            h.ability_block &= ~bit;
            --ability_block_count[category];
        }
        uint32_t want = 0;
        for (int k = 0; k < kCategoryBits; ++k) {
            if (ability_block_count[k]) {
                want |= 1u << k;
            }
        }
        InterlockedExchange(&ability_want, static_cast<LONG>(want));
    }

    void release(Handle& h, int names) {
        for (int n = 0; n < names; ++n) {
            if (h.holds[n] & kHoldHide) {
                set(h, n, kHoldHide, false);
            }
            if (h.holds[n] & kHoldBlock) {
                set(h, n, kHoldBlock, false);
            }
        }
        set_macros(h, false);
        for (int k = 0; k < kCategoryBits; ++k) {
            if (h.ability_block & (1u << k)) {
                set_ability(h, k, false);
            }
        }
    }
};

// ---------------------------------------------------------------------------
// Session memory of positions and sizes. Written only by the game thread;
// the Lua thread reads a consistent copy through the sequence word.

// A size is either form of resize; each has its own last writer.
const uint8_t kSizeRows = 1;
const uint8_t kSizeRect = 2;

struct MemEntry {
    volatile LONG seq;      // odd while the game thread is writing
    int16_t x;
    int16_t y;
    uint8_t active;         // a position is remembered
    uint8_t keep_dock;      // a group move: y belongs to docking while the window is docked
    uint8_t group;          // the group whose move wrote it, + 1; 0 for a move of the window
    uint8_t by_frame;       // x, y are the frame's top-left, as engine abi 5 takes them; else the origin
    uint8_t pinned;         // a bottom-anchored window's move by the frame: pinned_y is set
    int16_t pinned_y;       // the origin y its first placement gave it, which every later one keeps
    int32_t owner_slot;
    uint32_t owner_gen;
    uint8_t size_kind;      // kSizeRows or kSizeRect; 0 when no size is remembered
    int16_t size_a;         // the rows, or the width
    int16_t size_b;         // the height
    int32_t size_slot;
    uint32_t size_gen;
};

struct Memory {
    MemEntry e[kMaxNames];

    // `group` -1 for a move of the window itself.
    void write(int n, int x, int y, int group, int32_t slot, uint32_t gen, bool by_frame = false) {
        MemEntry& m = e[n];
        InterlockedIncrement(&m.seq);
        m.x = static_cast<int16_t>(x);
        m.y = static_cast<int16_t>(y);
        m.active = 1;
        m.keep_dock = group >= 0 ? 1 : 0;
        m.group = static_cast<uint8_t>(group + 1);
        m.by_frame = by_frame ? 1 : 0;
        m.pinned = 0;
        m.owner_slot = slot;
        m.owner_gen = gen;
        InterlockedIncrement(&m.seq);
    }

    void pin(int n, int y) {
        MemEntry& m = e[n];
        InterlockedIncrement(&m.seq);
        m.pinned = 1;
        m.pinned_y = static_cast<int16_t>(y);
        InterlockedIncrement(&m.seq);
    }

    void write_size(int n, uint8_t kind, int a, int b, int32_t slot, uint32_t gen) {
        MemEntry& m = e[n];
        InterlockedIncrement(&m.seq);
        m.size_kind = kind;
        m.size_a = static_cast<int16_t>(a);
        m.size_b = static_cast<int16_t>(b);
        m.size_slot = slot;
        m.size_gen = gen;
        InterlockedIncrement(&m.seq);
    }

    void clear(int n) {
        MemEntry& m = e[n];
        if (!m.active) {
            return;
        }
        InterlockedIncrement(&m.seq);
        m.active = 0;
        InterlockedIncrement(&m.seq);
    }

    void clear_size(int n) {
        MemEntry& m = e[n];
        if (!m.size_kind) {
            return;
        }
        InterlockedIncrement(&m.seq);
        m.size_kind = 0;
        InterlockedIncrement(&m.seq);
    }

    bool owned_by(int n, int32_t slot, uint32_t gen) const {
        return e[n].active && e[n].owner_slot == slot && e[n].owner_gen == gen;
    }

    bool size_owned_by(int n, int32_t slot, uint32_t gen) const {
        return e[n].size_kind && e[n].size_slot == slot && e[n].size_gen == gen;
    }

    // A copy from another thread. False when the writer kept it busy.
    bool read(int n, MemEntry* out) const {
        const MemEntry& m = e[n];
        for (int attempt = 0; attempt < 64; ++attempt) {
            const LONG before = m.seq;
            if (before & 1) {
                continue;
            }
            MemoryBarrier();
            out->x = m.x;
            out->y = m.y;
            out->active = m.active;
            out->keep_dock = m.keep_dock;
            out->group = m.group;
            out->by_frame = m.by_frame;
            out->pinned = m.pinned;
            out->pinned_y = m.pinned_y;
            out->owner_slot = m.owner_slot;
            out->owner_gen = m.owner_gen;
            out->size_kind = m.size_kind;
            out->size_a = m.size_a;
            out->size_b = m.size_b;
            out->size_slot = m.size_slot;
            out->size_gen = m.size_gen;
            MemoryBarrier();
            if (m.seq == before) {
                out->seq = before;
                return true;
            }
        }
        return false;
    }
};

// A group move made while its anchor was closed, kept until the game opens
// the anchor. Written only by the game thread; the Lua thread reads a
// consistent copy through the sequence word, as Memory's.
struct GroupWait {
    volatile LONG seq;
    int16_t x;
    int16_t y;
    uint8_t active;
    uint8_t by_frame;       // x, y are the anchor frame's top-left
    int32_t slot;
    uint32_t gen;
};

struct Waiting {
    GroupWait g[kGroupCount];

    void write(int group, int x, int y, int32_t slot, uint32_t gen, bool by_frame = false) {
        GroupWait& w = g[group];
        InterlockedIncrement(&w.seq);
        w.x = static_cast<int16_t>(x);
        w.y = static_cast<int16_t>(y);
        w.active = 1;
        w.by_frame = by_frame ? 1 : 0;
        w.slot = slot;
        w.gen = gen;
        InterlockedIncrement(&w.seq);
    }

    void clear(int group) {
        GroupWait& w = g[group];
        if (!w.active) {
            return;
        }
        InterlockedIncrement(&w.seq);
        w.active = 0;
        InterlockedIncrement(&w.seq);
    }

    bool owned_by(int group, int32_t slot, uint32_t gen) const {
        return g[group].active && g[group].slot == slot && g[group].gen == gen;
    }

    // A copy from another thread. False when the writer kept it busy.
    bool read(int group, GroupWait* out) const {
        const GroupWait& w = g[group];
        for (int attempt = 0; attempt < 64; ++attempt) {
            const LONG before = w.seq;
            if (before & 1) {
                continue;
            }
            MemoryBarrier();
            out->x = w.x;
            out->y = w.y;
            out->active = w.active;
            out->by_frame = w.by_frame;
            out->slot = w.slot;
            out->gen = w.gen;
            MemoryBarrier();
            if (w.seq == before) {
                out->seq = before;
                return true;
            }
        }
        return false;
    }
};

// ---------------------------------------------------------------------------
// Commands: Lua thread -> game thread. One producer (the caller holds the
// engine's Lua lock), one consumer (the per-frame drain).

enum Op {
    kOpMove = 1,
    kOpGroup,
    kOpReset,
    kOpResetOwned,
    kOpOpen,
    kOpClose,
    kOpQueryAnswer,
    kOpRestoreAll,
    kOpQueryCancel,
    kOpPassinpuSubmit,
    kOpPassinpuCancel,
    kOpPartyReply,          // value 1 accept, 0 decline
    kOpPostClose,           // target delivery, post1 or post2
    kOpLink5Cancel,
    kOpArealistAnswer,      // value the zone, as an int32
    kOpResizeRows,          // value the row count
    kOpResizeRect,          // x, y the width and height
    kOpLink5Confirm,        // value the concierge cache slot
    kOpGroupTo,             // the group's anchor to x, y, the rest by the same amount
    kOpPostEnd,             // end the incoming post-box session as its live box's cancel does, else as kOpPostClose
    kOpResetMine,           // value the aspects (kAspect*) of the window the command's handle placed
    kOpResetGroupMine,      // target the group: its moves the command's handle made
    kOpBlockClose,          // the game's close of a window a block found open
    kOpArealistCancel,
    kOpMacrosBlocked,       // the macro keys' first block: a bar the handler has up closed, no target
    kOpYesNoAnswer,         // value 1 yes, 0 no
    kOpYesNoCancel,
    kOpDeadConfirm,         // the KO window's confirm: its question asked
    kOpBoxAnswer,           // value the number, as an int32
    kOpBoxCancel,
    kOpAskedAnswer,         // target an Asked window; value its answer, as an int32
    kOpAskedCancel,
    kOpDialogueAnswer,      // target a dialogue box: one page on
    kOpDialogueCancel,
    kOpTextAnswer,          // target a text window; its answer in the engine's slot for it
    kOpTextCancel,
    kOpOffer,               // target an offer window; x the item's inventory index, 0 the gil; value the count
    kOpWithdraw,            // target an offer window; x the item's inventory index
    kOpOfferAnswer,         // target an offer window: its OK
    kOpOfferCancel,
    kOpOpenList,            // target ability or magic; value the list's category: the list alone
    kOpOpenSet,             // x the set (kSet*), target the window it opens; value the set's list, index or
                            // choice; for an item, z the bag (-1 an id alone in value), y the slot
    kOpOpenRoute,           // target a window of kRoutes: its route with x, y and value (RouteArgs)
    kOpSetSetting,          // target a setting (kSettings); value its value as the store or its route takes it
};

// What a kOpResetMine resets.
const uint32_t kAspectPosition = 1;
const uint32_t kAspectSize = 2;

// The public verb a command carries out, named in the error it may post.
enum Verb {
    kVerbNone = 0,
    kVerbMove,
    kVerbMoveGroup,
    kVerbReset,
    kVerbResetAll,
    kVerbOpen,
    kVerbClose,
    kVerbResize,
    kVerbAnswer,
    kVerbCancel,
    kVerbResetGroup,
    kVerbBlock,
    kVerbBlockMacros,
    kVerbOffer,
    kVerbWithdraw,
    kVerbOpenSet,
    kVerbSetSetting,
};

inline const char* verb_name(int verb) {
    static const char* const names[] = {"", "move", "move_group", "reset", "reset_all", "open", "close",
                                        "resize", "answer", "cancel", "reset_group", "block", "block_macros",
                                        "offer", "withdraw", "open_set", "set_setting"};
    return verb >= 0 && verb < static_cast<int>(sizeof(names) / sizeof(names[0])) ? names[verb] : "";
}

#ifdef HU_HEAPCHECK
inline const char* op_name(int op) {
    static const char* const names[] = {"", "move", "group", "reset", "reset_owned", "open", "close",
        "query_answer", "restore_all", "query_cancel", "passinpu_submit", "passinpu_cancel", "party_reply",
        "post_close", "link5_cancel", "arealist_answer", "resize_rows", "resize_rect", "link5_confirm", "group_to",
        "post_end", "reset_mine", "reset_group_mine", "block_close", "arealist_cancel", "macros_blocked",
        "yesno_answer", "yesno_cancel", "dead_confirm", "box_answer", "box_cancel", "asked_answer", "asked_cancel",
        "dialogue_answer", "dialogue_cancel", "text_answer", "text_cancel", "offer", "withdraw", "offer_answer",
        "offer_cancel", "open_list", "open_set", "open_route", "set_setting"};
    static_assert(sizeof(names) / sizeof(names[0]) == kOpSetSetting + 1, "a name for every op");
    return op >= 0 && op <= kOpSetSetting ? names[op] : "?";
}
#endif

// Command flags. A reply is counted against its window until the drain has
// carried it out; a listed reply is checked again on the game thread against
// the list the player would pick from; a group move that may wait is kept
// for the anchor's next open when the anchor is closed; a reply naming its
// prompt is refused once another open of the window has come, or with
// kCmdSession once another invite or post-box session has; a move's x, y
// are the frame's top-left with kCmdFrame, else the origin; a close of a
// prompt cancel() ends is refused while the prompt asks with kCmdPrompt.
const uint8_t kCmdReply = 1;
const uint8_t kCmdListed = 2;
const uint8_t kCmdWait = 4;
const uint8_t kCmdInstance = 8;
const uint8_t kCmdSession = 16;
const uint8_t kCmdFrame = 32;
const uint8_t kCmdPrompt = 64;

// The prompts that open no window of their own: a party invite, and a
// post-box session. A reply to one names the session, counted by the drain
// as it sees each appear.
const int kSessionInvite = 0;
const int kSessionPost = 1;

inline int session_window(const char* nm) {
    if (strcmp(nm, "prtyjoin") == 0) {
        return kSessionInvite;
    }
    if (strcmp(nm, "delivery") == 0 || strcmp(nm, "post1") == 0 || strcmp(nm, "post2") == 0) {
        return kSessionPost;
    }
    return -1;
}

// A session watch word is the sessions counted << 1 | 1 while the drain has
// seen the current one. The current session's id: the count, or the next
// one while a session is up that the drain has not seen yet, so a reader
// between frames and the drain after it agree.
inline LONG session_watch(uint32_t count, bool seen) {
    return static_cast<LONG>(count << 1 | (seen ? 1u : 0u));
}
inline uint32_t session_count(LONG watch) { return static_cast<uint32_t>(watch) >> 1; }
inline bool session_seen(LONG watch) { return (watch & 1) != 0; }
inline uint32_t session_id(LONG watch, bool live) {
    return session_count(watch) + (live && !session_seen(watch) ? 1u : 0u);
}

// The passinpu callback copies exactly this many bytes.
const size_t kPassinpuText = 16;

struct Command {
    uint8_t op;
    uint8_t verb;
    uint8_t flags;
    int16_t target;         // name index, or group index for kOpGroup and kOpGroupTo
    int16_t x;
    int16_t y;
    int16_t z;
    int32_t slot;
    uint32_t gen;
    uint32_t value;
    uint32_t instance;          // kCmdInstance: the window's open count the reply was made for
    char text[kPassinpuText];   // kOpPassinpuSubmit: `value` bytes, no NUL
};

const uint32_t kCommandCapacity = 4096;

struct CommandRing {
    Command buf[kCommandCapacity];
    volatile LONG head;     // commands pushed
    volatile LONG tail;     // commands taken

    bool push(const Command& c) {
        const uint32_t h = static_cast<uint32_t>(head);
        const uint32_t t = static_cast<uint32_t>(tail);
        if (h - t >= kCommandCapacity) {
            return false;
        }
        buf[h % kCommandCapacity] = c;
        MemoryBarrier();
        InterlockedIncrement(&head);
        return true;
    }

    bool pop(Command* out) {
        const uint32_t t = static_cast<uint32_t>(tail);
        if (t == static_cast<uint32_t>(head)) {
            return false;
        }
        MemoryBarrier();
        *out = buf[t % kCommandCapacity];
        MemoryBarrier();
        InterlockedIncrement(&tail);
        return true;
    }

    uint32_t pushed() const { return static_cast<uint32_t>(head); }
    uint32_t taken() const { return static_cast<uint32_t>(tail); }
};

// ---------------------------------------------------------------------------
// Events: game thread -> Lua thread. Any thread may post; every handle reads
// the same ring with its own cursor. A reader that falls a lap behind loses
// the oldest events and is told how many.

// kEvError carries the position of its ErrorRecord in place of a name,
// kEvPending what changed and whether it is pending now, kEvCursor the
// window and its row packed by pack_cursor. The other five on the ability
// window carry its list, packed by pack_event_category. kEvPage, a dialogue
// box showing a new page, reaches engine abi 8's poll alone.
enum EventType {
    kEvOpened = 1,
    kEvClosed = 2,
    kEvBlocked = 3,
    kEvCovered = 4,
    kEvUncovered = 5,
    kEvError = 6,
    kEvPending = 7,
    kEvCursor = 8,
    kEvPage = 9,
};

const int kPendingInvite = 0;
const int kPendingPost = 1;
const int kPendingBox = 2;      // + the box: a number box's question whose open was refused
const int kPendingAsked = kPendingBox + kBoxCount;  // + the Asked window: its question waiting with no window
const int kPendingMsgline = kPendingAsked + kAskedCount;    // msgline's message waiting with no window
const int kPendingCount = kPendingMsgline + 1;

// A pending event's name bits: what changed and whether it is pending now,
// then, for a question a reply ended, the replying handle's slot + 1 above
// them, with its generation's low byte in the category bits: that handle
// knows of its own reply, and its poll skips the event.
const int kPendingWhatBits = 4;
inline int pending_payload(int what, bool on) { return what << 1 | (on ? 1 : 0); }
inline int pending_what(int payload) { return (payload >> 1) & ((1 << kPendingWhatBits) - 1); }
inline bool pending_on(int payload) { return (payload & 1) != 0; }
const int kPendingSlotShift = kPendingWhatBits + 1;
const int32_t kPendingSlots = (1 << (16 - kPendingSlotShift)) - 1;

// What a pending event names: an invite, a post-box session, or the window
// whose question waits.
inline const char* pending_name(int what) {
    if (what == kPendingInvite || what == kPendingPost) {
        return what == kPendingInvite ? "invite" : "post";
    }
    if (what >= kPendingBox && what < kPendingAsked) {
        return kBoxNames[what - kPendingBox];
    }
    if (what == kPendingMsgline) {
        return kTextNames[kTextMsgline];
    }
    return what >= kPendingAsked && what < kPendingMsgline ? kAskedNames[what - kPendingAsked] : "";
}

inline uint32_t pack_event(int type, int name) {
    return static_cast<uint32_t>(type & 0xFF) | (static_cast<uint32_t>(name & 0xFFFF) << 16);
}
inline int event_type(uint32_t e) { return static_cast<int>(e & 0xFF); }
inline int event_name(uint32_t e) { return static_cast<int>(e >> 16); }

// An opened, closed, covered, uncovered or blocked event carrying a
// category: the category + 1 in bits 8..15, 0 for none, so 0..254 travels
// and any other value goes without one.
inline uint32_t pack_event_category(int type, int name, int category) {
    const uint32_t c = category >= 0 && category <= 254 ? static_cast<uint32_t>(category) + 1u : 0u;
    return pack_event(type, name) | (c << 8);
}
inline bool event_has_category(uint32_t e) { return ((e >> 8) & 0xFF) != 0; }
inline int event_category(uint32_t e) { return static_cast<int>((e >> 8) & 0xFF) - 1; }

// A pending event; `slot` and `gen` name the handle whose reply ended the
// question, slot -1 for none. A slot past kPendingSlots goes unnamed.
inline uint32_t pack_pending(int what, bool on, int32_t slot = -1, uint32_t gen = 0) {
    uint32_t name = static_cast<uint32_t>(pending_payload(what, on));
    uint32_t low = 0;
    if (slot >= 0 && slot < kPendingSlots) {
        name |= static_cast<uint32_t>(slot + 1) << kPendingSlotShift;
        low = gen & 0xFF;
    }
    return static_cast<uint32_t>(kEvPending) | (low << 8) | (name << 16);
}

// Whether a pending event's question was ended by the reply of the handle
// (slot, gen).
inline bool pending_from(uint32_t e, int32_t slot, uint32_t gen) {
    const uint32_t named = static_cast<uint32_t>(event_name(e)) >> kPendingSlotShift;
    return named != 0 && slot >= 0 && named == static_cast<uint32_t>(slot) + 1
        && ((e >> 8) & 0xFF) == (gen & 0xFF);
}

// A cursor event: the name in 9 bits (kMaxNames is under 512), the row in
// the 15 above it, clamped to 0..0x7FFF.
inline uint32_t pack_cursor(int name, int row) {
    const uint32_t r = row < 0 ? 0u : row > 0x7FFF ? 0x7FFFu : static_cast<uint32_t>(row);
    return static_cast<uint32_t>(kEvCursor) | (static_cast<uint32_t>(name & 0x1FF) << 8) | (r << 17);
}
inline int cursor_name(uint32_t e) { return static_cast<int>((e >> 8) & 0x1FF); }
inline int cursor_row(uint32_t e) { return static_cast<int>(e >> 17); }

const uint32_t kEventCapacity = 4096;

struct EventSlot {
    volatile LONG seq;      // position + 1 once published; 0 while being written
    volatile LONG payload;
};

struct EventRing {
    EventSlot slot[kEventCapacity];
    volatile LONG claimed;

    // Never blocks and never allocates: safe from a hook handler.
    void post(uint32_t payload) {
        const uint32_t pos = static_cast<uint32_t>(InterlockedIncrement(&claimed)) - 1;
        EventSlot& s = slot[pos % kEventCapacity];
        InterlockedExchange(&s.seq, 0);
        InterlockedExchange(&s.payload, static_cast<LONG>(payload));
        InterlockedExchange(&s.seq, static_cast<LONG>(pos + 1));
    }

    uint32_t position() const { return static_cast<uint32_t>(claimed); }

    // Copies up to `max` events past *cursor into `out`. Stops at an event
    // still being written; it is read on the next call.
    int read(uint32_t* cursor, uint32_t* out, int max, uint32_t* dropped) const {
        uint32_t c = *cursor;
        const uint32_t w = static_cast<uint32_t>(claimed);
        if (w - c > kEventCapacity) {
            *dropped += (w - c) - kEventCapacity;
            c = w - kEventCapacity;
        }
        int n = 0;
        while (c != w && n < max) {
            const EventSlot& s = slot[c % kEventCapacity];
            const LONG want = static_cast<LONG>(c + 1);
            const LONG before = s.seq;
            MemoryBarrier();
            if (before == want) {
                const LONG payload = s.payload;
                MemoryBarrier();
                if (s.seq == before) {
                    out[n++] = static_cast<uint32_t>(payload);
                } else {
                    ++*dropped;
                }
                ++c;
                continue;
            }
            if (before != 0 && static_cast<LONG>(static_cast<uint32_t>(before) - (c + 1)) > 0) {
                ++*dropped;
                ++c;
                continue;
            }
            break;
        }
        *cursor = c;
        return n;
    }
};

// ---------------------------------------------------------------------------
// Errors: what the game thread refused of a queued command, kept for the
// handle that queued it. The game thread alone writes; the event that names a
// record follows it, and a reader that finds the record overwritten, or
// mid-write, drops it.

const uint32_t kErrorCapacity = 256;    // divides 0x10000, the event's 16 bits
const size_t kErrorName = 48;           // the longest window, group, set or setting name, whole
const size_t kErrorReason = 200;

struct ErrorRecord {
    volatile LONG seq;      // position + 1 once written; 0 while being written
    int32_t slot;
    uint32_t gen;
    int32_t by_slot;        // the handle whose move replaced this one's, named when read; else -1
    uint32_t by_gen;
    char verb[16];
    char name[kErrorName];
    char reason[kErrorReason];
};

struct ErrorLog {
    ErrorRecord r[kErrorCapacity];
    volatile LONG written;

    // Returns the position, for the event.
    uint32_t add(int32_t slot, uint32_t gen, const char* verb, const char* name, const char* reason,
                 int32_t by_slot = -1, uint32_t by_gen = 0) {
        const uint32_t pos = static_cast<uint32_t>(InterlockedIncrement(&written)) - 1;
        ErrorRecord& e = r[pos % kErrorCapacity];
        InterlockedExchange(&e.seq, 0);
        e.slot = slot;
        e.gen = gen;
        e.by_slot = by_slot;
        e.by_gen = by_gen;
        snprintf(e.verb, sizeof(e.verb), "%s", verb);
        snprintf(e.name, sizeof(e.name), "%s", name);
        snprintf(e.reason, sizeof(e.reason), "%s", reason);
        MemoryBarrier();
        InterlockedExchange(&e.seq, static_cast<LONG>(pos + 1));
        return pos;
    }

    // The record an event's 16 bits name. False once it is overwritten.
    bool read(uint32_t pos16, ErrorRecord* out) const {
        const ErrorRecord& e = r[pos16 % kErrorCapacity];
        const LONG before = e.seq;
        MemoryBarrier();
        if (before == 0 || ((static_cast<uint32_t>(before) - 1) & 0xFFFF) != pos16) {
            return false;
        }
        memcpy(out, &e, sizeof(*out));
        MemoryBarrier();
        return e.seq == before;
    }
};

// ---------------------------------------------------------------------------
// Open calls in progress, so a post handler finds what its pre decided and a
// show nested inside an open is not reported twice. Only the main thread and
// the lobby worker open menus, and they run in lockstep, so the spin lock is
// never contended.

struct OpenFrame {
    const void* args;
    DWORD thread;
    int16_t name;
};

struct OpenStack {
    OpenFrame f[64];
    int depth;
    volatile LONG lock;
    volatile LONG overflow;

    void enter() {
        while (InterlockedCompareExchange(&lock, 1, 0) != 0) {
        }
    }
    void leave() { InterlockedExchange(&lock, 0); }

    // A frame on this thread whose arguments sit at or below `args` belongs to
    // a call that has already returned without its post running.
    void drop_dead(const void* args, DWORD thread) {
        int keep = 0;
        for (int i = 0; i < depth; ++i) {
            const bool dead = f[i].thread == thread
                && reinterpret_cast<uintptr_t>(f[i].args) <= reinterpret_cast<uintptr_t>(args);
            if (!dead) {
                f[keep++] = f[i];
            }
        }
        depth = keep;
    }

    bool push(const void* args, DWORD thread, int16_t name) {
        enter();
        drop_dead(args, thread);
        const bool ok = depth < static_cast<int>(sizeof(f) / sizeof(f[0]));
        if (ok) {
            f[depth].args = args;
            f[depth].thread = thread;
            f[depth].name = name;
            ++depth;
        } else {
            InterlockedIncrement(&overflow);
        }
        leave();
        return ok;
    }

    // Also drops any deeper frame of this thread: returning here means those
    // calls have returned too.
    bool pop(const void* args, DWORD thread, int16_t* name) {
        enter();
        bool found = false;
        int keep = 0;
        for (int i = 0; i < depth; ++i) {
            const bool mine = f[i].thread == thread;
            const uintptr_t at = reinterpret_cast<uintptr_t>(f[i].args);
            if (mine && f[i].args == args) {
                *name = f[i].name;
                found = true;
                continue;
            }
            if (mine && at < reinterpret_cast<uintptr_t>(args)) {
                continue;
            }
            f[keep++] = f[i];
        }
        depth = keep;
        leave();
        return found;
    }

    bool contains(DWORD thread, int16_t name) {
        enter();
        bool found = false;
        for (int i = 0; i < depth; ++i) {
            if (f[i].thread == thread && f[i].name == name) {
                found = true;
                break;
            }
        }
        leave();
        return found;
    }
};

// ---------------------------------------------------------------------------
// Glyph text: a parsed title or option as the game's text converter leaves it,
// int16 codes (the choice list's parsed options).
// 0..0x5F is the character 0x20 more, so a space is 0 and the codes are read
// by their count; 0x60 and up is a two-byte Shift-JIS character, given back
// as its two bytes through the table below, or as '?', counted, where the
// table has none; -0x1FF..-0x100 is color 1E nn with nn = -0x100 - g, -0x200
// and below color 1F nn with nn = -0x200 - g; -0xFF..-1 is not drawn.

const int kGlyphMax = 127;              // a title or option holds at most this many, then a 0
const int16_t kGlyphTwoByte = 0x60;     // the first code of a two-byte character
const uint8_t kColorTable = 0x1E;       // the game's color table: 1 default, 2 green
const uint8_t kColorSecond = 0x1F;
const uint16_t kColorDefault = 1;

// Two-byte characters by glyph: lead << 8 | trail, 0 where none. Nothing in
// the client converts glyphs back, so the game thread builds this once from
// the converter itself (game.h) and never changes it after `ready`.
struct GlyphTable {
    uint16_t sjis[0x8000];
    uint8_t cp932[0x8000 / 8];      // a bit per glyph: its pair is one code page 932 defines
    uint8_t gaiji[0x8000 / 8];      // a bit per glyph: its pair has lead 0xEF
    int32_t size;                   // glyphs with a pair
    int32_t cp932_defined;          // of them, those whose pair code page 932 defines
    int32_t gaiji_named;            // of them, those whose pair has lead 0xEF
    double build_ms;
    volatile LONG ready;

    bool defined(int g) const { return (cp932[g >> 3] >> (g & 7)) & 1; }
    bool from_gaiji(int g) const { return (gaiji[g >> 3] >> (g & 7)) & 1; }

    uint16_t lookup(int16_t g) const {
        if (!ready || g < kGlyphTwoByte) {
            return 0;
        }
        MemoryBarrier();
        return sjis[g];
    }
};

// A run of text in one color, named by the game's escape pair for it.
struct GlyphRun {
    uint8_t start;          // into GlyphText::text
    uint8_t length;
    uint8_t escape;
    uint16_t color;
};

struct GlyphText {
    char text[2 * kGlyphMax + 1];   // the plain text, Shift-JIS: colors left out
    GlyphRun runs[kGlyphMax];
    int run_count;
    int undecoded;                  // two-byte characters the table lacks, each a '?' in text
};

// A page of an NPC dialogue box: its lines, each at most kLineGlyphs codes,
// decoded and joined by '\n' as the box draws one line under the next.
const int kPageLines = 16;
const int kLineGlyphs = 0x100;

struct PageRun {
    uint16_t start;         // into PageText::text
    uint16_t length;
    uint8_t escape;
    uint16_t color;
};

struct PageText {
    char text[kPageLines * (2 * kLineGlyphs + 1) + 1];
    PageRun runs[2 * kPageLines * 16];
    int run_count;
    int undecoded;
    bool truncated;                 // runs ran out, the rest left off
};

// One drawn character, `width` bytes, onto `out`'s text, in a new run when
// its color is not the last run's; false, with nothing written, when the
// text or the runs are full.
template <typename Text>
inline bool glyph_append(Text* out, int* length, uint8_t escape, uint16_t color, const char* c, int width) {
    const int text_max = static_cast<int>(sizeof(out->text)) - 1;
    const int run_max = static_cast<int>(sizeof(out->runs) / sizeof(out->runs[0]));
    if (*length + width > text_max) {
        return false;
    }
    int last = out->run_count - 1;
    if (last < 0 || out->runs[last].escape != escape || out->runs[last].color != color) {
        if (out->run_count >= run_max) {
            return false;
        }
        last = out->run_count++;
        out->runs[last].start = static_cast<decltype(out->runs[last].start)>(*length);
        out->runs[last].length = 0;
        out->runs[last].escape = escape;
        out->runs[last].color = color;
    }
    memcpy(out->text + *length, c, width);
    *length += width;
    out->runs[last].length = static_cast<decltype(out->runs[last].length)>(out->runs[last].length + width);
    return true;
}

// `count` little-endian int16 codes onto `out` from *length, unaligned,
// from the default color; false once `out` is full. `table` may be NULL or
// not yet built: every two-byte character is then a '?'.
template <typename Text>
inline bool decode_codes(const uint8_t* glyphs, int count, const GlyphTable* table, Text* out, int* length) {
    uint8_t escape = kColorTable;
    uint16_t color = kColorDefault;
    for (int i = 0; i < count; ++i) {
        int16_t g;
        memcpy(&g, glyphs + 2 * i, 2);
        if (g <= -0x200) {
            escape = kColorSecond;
            color = static_cast<uint16_t>(-0x200 - g);
            continue;
        }
        if (g <= -0x100) {
            escape = kColorTable;
            color = static_cast<uint16_t>(-0x100 - g);
            continue;
        }
        if (g < 0) {
            continue;
        }
        char c[2] = {static_cast<char>(g + 0x20), 0};
        int width = 1;
        bool missing = false;
        if (g >= kGlyphTwoByte) {
            const uint16_t sjis = table ? table->lookup(g) : 0;
            if (sjis) {
                c[0] = static_cast<char>(sjis >> 8);
                c[1] = static_cast<char>(sjis & 0xFF);
                width = 2;
            } else {
                c[0] = '?';
                missing = true;
            }
        }
        if (!glyph_append(out, length, escape, color, c, width)) {
            return false;
        }
        out->undecoded += missing ? 1 : 0;
    }
    return true;
}

// `glyphs`: `count` little-endian int16 codes, unaligned; a count past
// kGlyphMax reads kGlyphMax.
inline void decode_glyphs(const uint8_t* glyphs, int count, const GlyphTable* table, GlyphText* out) {
    int length = 0;
    out->run_count = 0;
    out->undecoded = 0;
    decode_codes(glyphs, count > kGlyphMax ? kGlyphMax : count, table, out, &length);
    out->text[length] = '\0';
}

// A dialogue box's page: `lines` lines of glyph codes, line i's `counts[i]`
// codes at `glyphs[i]` (a count past kLineGlyphs reads kLineGlyphs), each
// from the default color, a '\n' between two lines.
inline void decode_page(const uint8_t* const* glyphs, const int* counts, int lines, const GlyphTable* table,
                        PageText* out) {
    int length = 0;
    out->run_count = 0;
    out->undecoded = 0;
    out->truncated = false;
    for (int i = 0; i < lines && !out->truncated; ++i) {
        const int count = counts[i] < 0 ? 0 : counts[i] > kLineGlyphs ? kLineGlyphs : counts[i];
        out->truncated = (i > 0 && !glyph_append(out, &length, kColorTable, kColorDefault, "\n", 1))
            || !decode_codes(glyphs[i], count, table, out, &length);
    }
    out->text[length] = '\0';
}

// The bytes a 7F escape takes in the converter, the 7F among them.
inline size_t text_escape_bytes(uint8_t kind) {
    switch (kind) {
    case 0x31: case 0x32: case 0x33: case 0x37: case 0xFB: case 0xFC:
        return 2;
    case 0x34: case 0x35: case 0x36:
        return 3;
    case 0x38:
        return 4;
    default:
        return 1;
    }
}

// The bytes the converter reads as the lead of a two-byte character.
inline bool text_lead_byte(uint8_t b) {
    return (b >= 0x81 && b <= 0x9F) || (b >= 0xE0 && b <= 0xEF) || (b >= 0xFA && b <= 0xFC);
}

// The game's own text, NUL-terminated within `bytes`, as its text-to-glyph
// converter reads it into codes and decode_glyphs decodes those: 1E nn and
// 1F nn switch the color; a 7F escape is skipped, drawn as nothing but the
// space a 7F 00 makes; a lead byte and the byte after it are a two-byte
// character, kept as the text's own bytes, so none is undecoded; any other
// byte is drawn from 0x20 to 0x7E alone. At most kGlyphMax codes, as
// decode_glyphs reads. A sequence the terminator cuts short ends the text.
inline void decode_text(const uint8_t* text, size_t bytes, GlyphText* out) {
    uint8_t escape = kColorTable;
    uint16_t color = kColorDefault;
    int length = 0;
    out->run_count = 0;
    out->undecoded = 0;
    size_t i = 0;
    for (int codes = 0; codes < kGlyphMax && i < bytes && text[i]; ++codes) {
        const uint8_t b = text[i];
        const uint8_t next = i + 1 < bytes ? text[i + 1] : 0;
        if (b == kColorTable || b == kColorSecond) {
            if (!next) {
                break;
            }
            escape = b;
            color = next;
            i += 2;
            continue;
        }
        if (b == 0x7F) {
            const size_t take = text_escape_bytes(next);
            if (take == 1 && !next) {
                glyph_append(out, &length, escape, color, " ", 1);
            }
            size_t k = 1;
            while (k < take && i + k < bytes && text[i + k]) {
                ++k;
            }
            if (k < take) {
                break;
            }
            i += take;
            continue;
        }
        if (text_lead_byte(b)) {
            if (!next) {
                break;
            }
            const char c[2] = {static_cast<char>(b), static_cast<char>(next)};
            glyph_append(out, &length, escape, color, c, 2);
            i += 2;
            continue;
        }
        if (b >= 0x20 && b < 0x7F) {
            const char c = static_cast<char>(b);
            glyph_append(out, &length, escape, color, &c, 1);
        }
        ++i;
    }
    out->text[length] = '\0';
}

// ---------------------------------------------------------------------------
// Replies: what a read returns to Lua, in the format hideui_engine_abi.h
// defines, written the way the Lua calls that push it would run.

struct ReplyWriter {
    HuEngineReply* out;

    // Past the capacity it only counts, so the caller learns what it needs.
    void bytes(const void* p, uint32_t n) {
        if (out->used <= out->capacity && n <= out->capacity - out->used) {
            memcpy(out->data + out->used, p, n);
        }
        out->used += n;
    }
    void code(uint8_t c) { bytes(&c, 1); }
    void counted(uint8_t c, const char* text) {
        const uint32_t n = static_cast<uint32_t>(strlen(text));
        code(c);
        bytes(&n, 4);
        bytes(text, n);
    }

    void nil() { code(HU_REPLY_NIL); }
    void boolean(bool v) { code(v ? HU_REPLY_TRUE : HU_REPLY_FALSE); }
    void number(double v) {
        code(HU_REPLY_NUMBER);
        bytes(&v, 8);
    }
    void string(const char* v) { counted(HU_REPLY_STRING, v); }
    void table() { code(HU_REPLY_TABLE); }
    void field(const char* key) { counted(HU_REPLY_FIELD, key); }
    void index(int32_t i) {
        code(HU_REPLY_INDEX);
        bytes(&i, 4);
    }

    void set_number(const char* key, double v) { number(v); field(key); }
    void set_bool(const char* key, bool v) { boolean(v); field(key); }
    void set_string(const char* key, const char* v) { string(v); field(key); }
    void fail(const char* why) { nil(); string(why); }
};

}  // namespace hu

#endif  // HIDEUI_CORE_H_
