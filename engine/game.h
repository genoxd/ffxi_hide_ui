// game.h - what the engine does to the game: the hook handlers, the per-frame
// drain, and the hides, moves and resets they carry out. The handlers and the
// drain run on the thread that called the hooked routine -- the game's main
// thread, or the lobby worker that runs in lockstep with it -- and never call
// into Lua, never allocate and never wait.
//
// No Lua and no daemon calls: the offline test drives this file against
// fabricated menus, the engine against the game.

#ifndef HIDEUI_GAME_H_
#define HIDEUI_GAME_H_

#include "core.h"
#include "../daemon/hideui_abi.h"

#include <stdarg.h>

namespace hu {

// FFXiMain's own routines. All are __thiscall; MinGW's __fastcall with a
// dummy edx is the same call, callee popping the stack arguments.
typedef void (__fastcall* SetPositionFn)(void* menu, void* edx, int x, int y);
typedef void* (__fastcall* OpenFn)(void* mcb, void* edx, const char* key, int activate, int overlap);
typedef int (__fastcall* CloseFn)(void* mcb, void* edx, const char* key);
typedef void (__fastcall* LayoutFn)(void* controller, void* edx);
typedef void (__fastcall* DockResetFn)(void* mcb, void* edx, int group, void* menu);
typedef void (__fastcall* SwapFn)(void* menu, void* edx, const char* key);
typedef void (__fastcall* FrameRectFn)(void* menu, void* edx, int x, int y, int w, int h,
                                       int one, int keep, int b9a);
typedef void (__fastcall* SetCursorFn)(void* menu, void* edx, int row, int warp);

// The reply routines. Those returning a bool set only al.
typedef uint32_t (__fastcall* ControllerFn)(void* controller, void* edx);
typedef uint32_t (__cdecl* PartySendFn)(int accept);
typedef void (__cdecl* PartyClearFn)();
typedef uint32_t (__cdecl* PostRequestFn)(int command);
typedef void (__cdecl* TextCallbackFn)(void* context, const char* text);
typedef void (__cdecl* Link5CallbackFn)(int slot, const void* entry);

// Text to glyphs: at most max_glyphs - 1 codes, then a 0; returns the count.
typedef uint32_t (__cdecl* GlyphConvertFn)(const char* text, int16_t* glyphs, uint32_t max_glyphs, char* raw,
                                           int raw_bytes);

const size_t kMenuBytes = 0xD4;
const size_t kMenuRes = 0x04;
const size_t kResKey = 0x46;
const size_t kMenuList = 0x14;
const size_t kMenuState = 0x10;
const size_t kMenuPolicy = 0x34;
const size_t kMenuRect = 0x3A;
const size_t kMenuDefault = 0x42;
const size_t kMenuCursor = 0x4C;
const size_t kMenuOrigin = 0x52;
const size_t kMenuInputWait = 0x56;     // int16: the input dispatcher drops every event while it is above 0
const size_t kMenuItems = 0x58;
const size_t kMenuBusy = 0x5D;          // the input dispatcher passes nothing but a cancel while it is set
const size_t kMenuLayer = 0x60;
const size_t kMenuClosing = 0x6E;       // close by instance sets it before its staged close
const size_t kMenuPendingDestroy = 0x70; // close by instance sets it on a dormant menu, deferring its destruction
const size_t kMenuMouse = 0x77;         // 0: the row hit test misses the menu; its constructor sets 1
const size_t kMenuFrameByte = 0x9A;     // SetFrameRect's last argument lands here
const size_t kCtlMenu = 0x08;
const size_t kQueryResult = 0x548;
const size_t kQueryOptions = 0x14;      // the parsed options: a list, as a menu's element list
const size_t kQueryCount = 0x24;        // int32: the options in that list
const size_t kQueryCursor = 0x30;       // int16: the option under the cursor, 0-based in that list
const size_t kQueryTop = 0x32;          // int16: the first option shown; +0x4C = cursor - top + 1
const int kQueryListMax = 512;          // the most nodes a walk of that list follows
const int kQueryRowsShown = 3;          // the rows the cursor setter keeps the cursor among
const size_t kQueryTitleCount = 0x36;   // the title's glyph count, read as a u8 (at most kGlyphMax)
const size_t kQueryTitle = 0x38;        // the title's int16 glyph codes, inline
const size_t kOptionBytes = 0x108;
const size_t kOptionGlyphs = 0x04;      // int16 glyph codes, inline
const size_t kOptionValue = 0x104;      // u16: what an answer to query writes
const size_t kOptionCount = 0x106;      // u8: the glyph count
const size_t kPassMax = 0x18;
const size_t kPassCallback = 0x1C;
const size_t kPassContext = 0x20;
const size_t kLink5Mode = 0x68;        // dword: 1 when the event's 0x71 sub 0x40 handler opened it
const size_t kLink5Callback = 0x6C;
const int kLink5Slots = 16;
const size_t kLink5Entry = 0x20;
const size_t kLink5Count = 0x50;        // rows, the header row 0 included
const size_t kLink5Rows = 0x54;
const size_t kLink5Names = 0x58;        // a buffer: row r's name at +r*0x17+3
const size_t kLink5RowStride = 0x54;
const size_t kLink5RowRecord = 0x48;    // the row's concierge cache entry
const size_t kLink5NameStride = 0x17;
const size_t kLink5NameAt = 3;
const size_t kLink5NameBytes = 20;
const size_t kListScroll = 0x1E;        // int16: a list controller's first row shown, 0-based
const size_t kAreaCapacity = 0x50;      // int32: the rows arealist's array holds
const size_t kAreaCount = 0x54;         // int32: the rows filled; kept when a close frees the array
const size_t kAreaLevel = 0x58;         // int16: 0 the top, -id of the region whose zones are shown
const size_t kAreaRows = 0x60;          // the row array, reallocated by every rebuild
const size_t kAreaMode = 0x6C;
const size_t kAreaLatch = 0x6D;         // set by every open; cleared by the close
const size_t kAreaZone = 0x6E;          // int16: what the event reads, -1 until an answer
const size_t kAreaBytes = 0x74;
const size_t kAreaRowStride = 0x54;
const size_t kAreaRowFormat = 0x00;     // u8 per column: how the column's value is drawn
const size_t kAreaRowValue = 0x28;      // dword per column; column 0 is the row's text
const size_t kAreaRowEntry = 0x48;      // the row's {int32 id; u8 flag}
const int kAreaRowMax = 0x102;          // the rows a rebuild allocates
const uint8_t kAreaModeOther = 4;       // a list whose rows are no areas
const int kZoneMax = 0x1FF;              // Windower's zone ids: what a blocked list's answer may name
const uint8_t kFormatText = 0x01;
const uint8_t kFormatTextParens = 0x21; // text, drawn "(%s)"
const uint8_t kFormatCount = 0x09;      // a number, drawn "[%d]"
const size_t kDeliveryState = 0x1B4;
const size_t kDeliveryNext = 0x1B6;     // u16: the state the closing states hand over to when done
const size_t kPostState = 0x14;         // post1's controller; request-close reads it
const size_t kPartyName = 0x14;         // prtyjoin's controller: the inviter, 16 bytes
const size_t kPartyNameBytes = 16;
const size_t kPartyKind = 1;            // the byte after the pending flag: 1 party, 0 alliance
const size_t kMcbUiW = 0x80;
const size_t kMcbUiH = 0x82;
const size_t kMcbActive = 0x54;
const size_t kMcbBytes = 0xB8;
const size_t kMouseMode = 0x4D;         // the mouse object's: 1 world, 2 menu
const uint8_t kMouseWorld = 1;
// The compass object, reached only through the game's global pointer. The
// manager lays its box out as 88 wide by +0x2C high, its anchor the box's
// bottom-right: the clock and the dial draw relative to (x, y - height).
const size_t kCompassState = 0x0D;      // u8: 0 hidden, 1 show requested, 2 fading in, 3 shown, 4 fading out
const size_t kCompassX = 0x28;          // int16: the anchor
const size_t kCompassY = 0x2A;
const size_t kCompassHeight = 0x2C;     // int16
const size_t kCompassBytes = 0x3C;
const int kCompassWidth = 88;
// The macro key object, reached only through the game's global pointer. The
// handler records the bar it has up and the set its keys dispatch to; its
// own close of a bar is the bar closed by name, then +0x0C and +0x18 zeroed,
// +0x0D left alone.
const size_t kMacroBar = 0x0C;          // u8: 0 none, 1 the Ctrl bar, 2 the Alt bar up
const size_t kMacroSet = 0x0D;          // u8: the set the number keys dispatch to
const size_t kMacroBarUp = 0x18;        // u8: set while a bar is up
const size_t kMacroObjectBytes = 0x24;  // the constructor's allocation

const uint32_t kStateDormant = 0x0E;
const int kQueryCancelled = 0xFF;
const int kPostCloseCommand = 0x0F;
// The outgoing box's closing states: the staged rows taken back, the box-2
// slots asked back, their replies awaited, request 15 sent, its reply
// awaited.
const int kDeliveryUnstage = 0x15;
const int kDeliveryReturn = 0x16;
const int kDeliveryWait = 0x17;
const int kDeliveryRequest = 0x18;
const int kDeliveryClosing = 0x19;
const uint32_t kPostClosing = 0x12;

inline int16_t rd16(const uint8_t* p, size_t off) {
    int16_t v;
    memcpy(&v, p + off, 2);
    return v;
}
inline uint32_t rd32(const uint8_t* p, size_t off) {
    uint32_t v;
    memcpy(&v, p + off, 4);
    return v;
}
inline uint8_t* rdptr(const uint8_t* p, size_t off) {
    uint8_t* v;
    memcpy(&v, p + off, 4);
    return v;
}
inline void wr16(uint8_t* p, size_t off, int v) {
    const int16_t s = static_cast<int16_t>(v);
    memcpy(p + off, &s, 2);
}
inline void wr32(uint8_t* p, size_t off, uint32_t v) {
    memcpy(p + off, &v, 4);
}

// Committed, readable, not a guard page. For pointers this thread does not
// own: everything the Lua thread reads, and code it scans.
inline bool readable(const void* p, size_t bytes) {
    if (!p) {
        return false;
    }
    MEMORY_BASIC_INFORMATION mbi;
    const uint8_t* cur = static_cast<const uint8_t*>(p);
    const uint8_t* end = cur + bytes;
    while (cur < end) {
        if (VirtualQuery(cur, &mbi, sizeof(mbi)) != sizeof(mbi)) {
            return false;
        }
        if (mbi.State != MEM_COMMIT) {
            return false;
        }
        const DWORD ok = PAGE_READONLY | PAGE_READWRITE | PAGE_WRITECOPY | PAGE_EXECUTE_READ
            | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY;
        if (!(mbi.Protect & ok) || (mbi.Protect & PAGE_GUARD)) {
            return false;
        }
        cur = static_cast<const uint8_t*>(mbi.BaseAddress) + mbi.RegionSize;
    }
    return true;
}

// ---------------------------------------------------------------------------
// Controller classes, read from their code.

// 249 of 312 classes point vtable+0x14 at a shared bare `ret`; the rest
// re-cache their widget positions when SetPosition asks them to.
inline bool refreshes_itself(const uint8_t* ctl) {
    const uint8_t* vt = rdptr(ctl, 0);
    if (!readable(vt, 6 * 4)) {
        return false;
    }
    const uint8_t* fn = rdptr(vt, 5 * 4);
    if (!readable(fn, 1)) {
        return false;
    }
    return fn[0] != 0xC3;
}

// The class code below is read byte by byte, not decoded, so a byte counts
// only where the compiler's own forms put it. A ret (C3, or C2 and its
// count) ends the routine when it is the routine's first byte or follows an
// epilogue: pop ebx, ebp, esi or edi, leave, or add esp. Any other C3 or C2
// is part of an operand.
inline bool ends_routine(const uint8_t* fn, size_t i) {
    if (fn[i] != 0xC3 && fn[i] != 0xC2) {
        return false;
    }
    if (i == 0) {
        return true;
    }
    const uint8_t before = fn[i - 1];
    if (before == 0x5B || before == 0x5D || before == 0x5E || before == 0x5F || before == 0xC9) {
        return true;
    }
    if (i >= 3 && fn[i - 3] == 0x83 && fn[i - 2] == 0xC4) {
        return true;
    }
    return i >= 6 && fn[i - 6] == 0x81 && fn[i - 5] == 0xC4;
}

// The open hook's tail jump `jmp *NN(%eax)` (FF 60 NN), in the two forms the
// game's classes end on: straight after the vtable load (mov eax,[ecx] or
// mov eax,[eax]), or after setting ecx for the callee and popping a saved
// register (mov ecx,r32; pop ebx/ebp/esi/edi). Any other FF 60 is part of an
// operand.
inline bool vtable_tail_jump(const uint8_t* fn, size_t i) {
    if (fn[i] != 0xFF || fn[i + 1] != 0x60) {
        return false;
    }
    if (i >= 2 && fn[i - 2] == 0x8B && (fn[i - 1] == 0x00 || fn[i - 1] == 0x01)) {
        return true;
    }
    if (i < 3 || fn[i - 3] != 0x8B || (fn[i - 2] & 0xF8) != 0xC8) {
        return false;
    }
    const uint8_t pop = fn[i - 1];
    return pop == 0x5B || pop == 0x5D || pop == 0x5E || pop == 0x5F;
}

// A direct call to `target` before the function's ret.
inline bool calls_directly(const uint8_t* fn, const void* target) {
    if (!readable(fn, 0x200)) {
        return true;
    }
    for (size_t i = 0; i + 5 <= 0x200; ++i) {
        if (fn[i] == 0xE8) {
            int32_t rel;
            memcpy(&rel, fn + i + 1, 4);
            if (fn + i + 5 + rel == target) {
                return true;
            }
        }
        if (ends_routine(fn, i)) {
            break;
        }
    }
    return false;
}

// Driving a class's layout is safe only when its open hook, and the layout
// its tail jump reaches, never call SetPosition themselves: persona's places
// itself at a constant and would be yanked home.
inline bool relayout_is_safe(const uint8_t* ctl, const void* set_position) {
    const uint8_t* vt = rdptr(ctl, 0);
    if (!readable(vt, 0x50)) {
        return false;
    }
    const uint8_t* open = rdptr(vt, 1 * 4);
    if (!readable(open, 0x200)) {
        return false;
    }
    for (size_t i = 0; i + 5 <= 0x200; ++i) {
        if (open[i] == 0xE8) {
            int32_t rel;
            memcpy(&rel, open + i + 1, 4);
            if (open + i + 5 + rel == set_position) {
                return false;
            }
        }
        if (ends_routine(open, i)) {
            break;
        }
        if (vtable_tail_jump(open, i)) {
            const size_t slot = open[i + 2] / 4;
            if (slot < 0x14 && readable(vt + slot * 4, 4)) {
                const uint8_t* sub = rdptr(vt, slot * 4);
                if (readable(sub, 0x200) && calls_directly(sub, set_position)) {
                    return false;
                }
            }
            break;
        }
    }
    return true;
}

// Re-runs a class's own layout so its cached widget positions follow the
// window. The open hook's tail jump names the layout slot and the layout's
// opening `cmp %bp,NN(%esi)` its one-shot guard. Never the close/open slots:
// they put the window back at its default.
inline bool run_layout(uint8_t* ctl, const void* set_position) {
    if (!relayout_is_safe(ctl, set_position)) {
        return false;
    }
    const uint8_t* vt = rdptr(ctl, 0);
    const uint8_t* open = rdptr(vt, 1 * 4);
    if (!readable(open, 0x80)) {
        return false;
    }
    size_t slot = 0;
    for (size_t i = 0; i + 3 <= 0x80; ++i) {
        if (vtable_tail_jump(open, i)) {
            slot = open[i + 2] / 4;
            break;
        }
        if (ends_routine(open, i)) {
            break;
        }
    }
    if (!slot || slot >= 0x20 || !readable(vt + slot * 4, 4)) {
        return false;
    }
    const uint8_t* layout = rdptr(vt, slot * 4);
    if (!readable(layout, 0x40)) {
        return false;
    }
    size_t guard = 0;
    for (size_t i = 0; i + 4 <= 0x40; ++i) {
        if (layout[i] == 0x66 && layout[i + 1] == 0x39 && layout[i + 2] == 0x6E) {
            guard = layout[i + 3];
            break;
        }
    }
    if (!guard || !readable(ctl + guard, 2)) {
        return false;
    }
    wr16(ctl, guard, 0);
    LayoutFn fn;
    memcpy(&fn, &layout, sizeof(fn));
    fn(ctl, NULL);
    return true;
}

// ---------------------------------------------------------------------------
// The engine's state. One instance, zero-initialized.

struct Game {
    uint8_t* registry;
    uint8_t** slot[kRowCount];      // the global holding each row's controller
    uint8_t* mcb;
    SetPositionFn set_position;
    OpenFn open;
    CloseFn close;
    DockResetFn dock_reset;
    SwapFn template_swap;
    FrameRectFn set_frame_rect;
    SetCursorFn set_cursor;
    ControllerFn passinpu_reset;
    PartySendFn party_send;
    PartyClearFn party_clear;
    uint8_t* party_pending;         // the byte party_clear zeroes
    ControllerFn link5_clear;
    uint8_t* link5_cache;           // kLink5Slots entries of kLink5Entry bytes
    const uint8_t* link5_latch;     // 1 while the event waits on a choice
    const void* link5_callback;     // the event's, as its handler opens link5
    const uint8_t* query_cancel_allowed;
    ControllerFn arealist_close;
    ControllerFn arealist_latch;
    PostRequestFn post_request;
    ControllerFn post_request_close;
    GlyphConvertFn glyph_convert;   // called once, by build_glyph_table
    const uint8_t* row_hit_test;    // never called: resolved for its kMenuMouse test
    const uint8_t* mouse_read;      // that test, inside it
    const uint8_t* menu_routing;    // never called: resolved for its call to the sink
    uintptr_t routing_return;       // that call's return address
    uint8_t** compass;              // the game's global: the compass object, or NULL
    uint8_t** macro_object;         // the game's global: the macro key object, or NULL
};

// Game-thread bookkeeping per name. Never read by the Lua thread.
struct Placement {
    uint8_t applied_hidden;
    uint8_t own_mouse;              // kMenuMouse of `mouse_menu` before the hide wrote 0
    const uint8_t* mouse_menu;      // the instance the hide found live; NULL once shown
    uint8_t registry_undocked;
    uint8_t touched;                // the engine placed the window it has now
    uint8_t last_keep_dock;
    uint8_t home_valid;
    int16_t last_x;
    int16_t last_y;
    int16_t home_x;
    int16_t home_y;
    uint8_t size_kind;              // the form of the size the engine put on the window
    uint8_t swapped;                // a rows resize swapped its template
    int16_t own_w;                  // the frame before the engine first sized it, while size_kind is set
    int16_t own_h;
    int16_t own_items;              // +0x58 then
};

// How a window is placed, read off its frame at the engine's first touch and
// kept until the engine forgets the window.
const LONG kAnchorUnknown = 0;
const LONG kAnchorTop = 1;
const LONG kAnchorBottom = 2;

struct Engine {
    Game game;
    Inventory inv;
    Holds holds;
    Memory memory;
    CommandRing commands;
    EventRing events;
    OpenStack opens;
    Placement place[kMaxNames];
    volatile LONG anchor[kMaxNames];    // written by the game thread; info reads it
    GlyphTable glyphs;                  // built by the game thread; query_options reads it
    ErrorLog errors;
    volatile LONG replies[kMaxNames];   // replies queued per window and not yet carried out
    uint8_t covered[kMaxNames];         // game thread only: sent dormant under another window
    volatile LONG instance[kMaxNames];  // the game's opens of each window, refused ones included
    Waiting waiting;                    // group moves waiting for their anchor's open

    // What pending() reads, as the drain last saw it: the session watch
    // words (core.h's session_watch), written by the game thread alone.
    int16_t watch_delivery;
    int16_t watch_post1;
    uint8_t watch_ready;
    volatile LONG invite_watch;
    volatile LONG post_watch;

    // Each open window's cursor row (query's option) as the drain last read
    // it, and the instance it read it from: game thread only.
    int16_t last_row[kMaxNames];
    const uint8_t* row_menu[kMaxNames];

    // The compass as the drain last saw it: shown (its state byte not 0),
    // and the anchor the game had on it when a move first took it, x in the
    // low word and y in the high, published for info's default box. Written
    // by the game thread alone.
    uint8_t compass_shown;
    volatile LONG compass_home;
    volatile LONG compass_home_valid;

    const Command* current;         // the command the drain is carrying out, on `current_thread`
    DWORD current_thread;
    volatile LONG completed;        // commands carried out, not merely taken
    volatile LONG unmatched_keys;
    volatile LONG drain_errors;
    volatile LONG error_seq;        // odd while the game thread writes `error`
    char error[256];
};

inline bool is_compass(const Engine& e, int n) {
    return n >= 0 && n == e.inv.compass;
}

inline uint8_t* compass_object(const Engine& e) {
    return e.game.compass ? *e.game.compass : NULL;
}

inline uint8_t* macro_object(const Engine& e) {
    return e.game.macro_object ? *e.game.macro_object : NULL;
}

// Open, as every reader and event reports it: the game is drawing it, fading
// in or out included.
inline bool compass_open(const uint8_t* c) {
    return c && c[kCompassState] != 0;
}

// The box the manager lays the compass out in, as {left, top, right,
// bottom} from its anchor.
inline void compass_frame(const uint8_t* c, int16_t out[4]) {
    const int x = rd16(c, kCompassX);
    const int y = rd16(c, kCompassY);
    out[0] = static_cast<int16_t>(x - kCompassWidth);
    out[1] = static_cast<int16_t>(y - rd16(c, kCompassHeight));
    out[2] = static_cast<int16_t>(x);
    out[3] = static_cast<int16_t>(y);
}

inline LONG pack_home(int x, int y) {
    return static_cast<LONG>((static_cast<uint32_t>(y) & 0xFFFFu) << 16 | (static_cast<uint32_t>(x) & 0xFFFFu));
}
inline int16_t home_x(LONG home) { return static_cast<int16_t>(home & 0xFFFF); }
inline int16_t home_y(LONG home) { return static_cast<int16_t>(static_cast<uint32_t>(home) >> 16); }

// The window or group a command names; empty for one that names none.
inline const char* command_target(const Engine& e, const Command& c) {
    if (c.op == kOpGroup || c.op == kOpGroupTo || c.op == kOpResetGroupMine) {
        return c.target >= 0 && c.target < kGroupCount ? kGroups[c.target].name : "";
    }
    if (c.op == kOpResetOwned || c.op == kOpRestoreAll) {
        return "";
    }
    return c.target >= 0 && c.target < e.inv.count ? e.inv.names[c.target].name : "";
}

// What the game thread could not do: status() keeps the last, and inside a
// queued command an error event goes to the handle that queued it, named by
// the command's verb and target.
inline void drain_error(Engine& e, const char* format, ...) {
    char reason[kErrorReason];
    va_list args;
    va_start(args, format);
    vsnprintf(reason, sizeof(reason), format, args);
    va_end(args);
    reason[sizeof(reason) - 1] = '\0';
    const Command* c = e.current && e.current_thread == GetCurrentThreadId() ? e.current : NULL;
    const char* verb = c ? verb_name(c->verb) : "";
    const char* name = c ? command_target(e, *c) : "";
    InterlockedIncrement(&e.error_seq);
    if (verb[0]) {
        snprintf(e.error, sizeof(e.error), "%s%s%s: %s", verb, name[0] ? " " : "", name, reason);
    } else {
        snprintf(e.error, sizeof(e.error), "%s", reason);
    }
    InterlockedIncrement(&e.error_seq);
    InterlockedIncrement(&e.drain_errors);
    if (c && c->slot >= 0) {
        const uint32_t pos = e.errors.add(c->slot, c->gen, verb, name, reason);
        e.events.post(pack_event(kEvError, static_cast<int>(pos & 0xFFFF)));
    }
}

// ---------------------------------------------------------------------------
// Finding menus. On this thread the game's pointers are consistent: a
// non-null controller or menu is live.

inline int name_of_menu(const Engine& e, const uint8_t* menu) {
    const uint8_t* res = rdptr(menu, kMenuRes);
    return res ? e.inv.find_key16(res + kResKey) : -1;
}

// The key compared to the window's own: keys are unique to a name, and the
// drain asks this of every window every frame.
inline uint8_t* live_menu(Engine& e, int n, uint8_t** ctl_out) {
    const NameEntry& ne = e.inv.names[n];
    for (int i = 0; i < ne.row_count; ++i) {
        uint8_t** slot = e.game.slot[ne.rows[i]];
        uint8_t* ctl = slot ? *slot : NULL;
        if (!ctl) {
            continue;
        }
        uint8_t* menu = rdptr(ctl, kCtlMenu);
        const uint8_t* res = menu ? rdptr(menu, kMenuRes) : NULL;
        if (!res || memcmp(res + kResKey, ne.key, kKeyLen) != 0) {
            continue;
        }
        if (ctl_out) {
            *ctl_out = ctl;
        }
        return menu;
    }
    return NULL;
}

// Open, as every reader and verb reports it: a live instance close by
// instance has marked neither closing nor pending destroy. The instance
// outlives the marks until the update loop frees it; a covered window,
// dormant with the marks clear, is open.
inline bool menu_is_open(const uint8_t* menu) {
    return menu && menu[kMenuClosing] == 0 && menu[kMenuPendingDestroy] == 0;
}

inline uint8_t* open_menu(Engine& e, int n, uint8_t** ctl_out) {
    uint8_t* ctl = NULL;
    uint8_t* menu = live_menu(e, n, &ctl);
    if (!menu_is_open(menu)) {
        return NULL;
    }
    if (ctl_out) {
        *ctl_out = ctl;
    }
    return menu;
}

inline uint8_t* controller_of(Engine& e, int n, const uint8_t* menu) {
    const NameEntry& ne = e.inv.names[n];
    for (int i = 0; i < ne.row_count; ++i) {
        uint8_t** slot = e.game.slot[ne.rows[i]];
        uint8_t* ctl = slot ? *slot : NULL;
        if (ctl && rdptr(ctl, kCtlMenu) == menu) {
            return ctl;
        }
    }
    return NULL;
}

inline uint8_t* row_of(Engine& e, int row) {
    return e.game.registry + row * kRowStride;
}

// A controller through its registry row's slot, open or not: the reply
// routines work on the controller, and a blocked menu still has one.
inline uint8_t* controller_global(Engine& e, const char* nm) {
    const int n = e.inv.find_exact(nm);
    if (n < 0 || e.inv.names[n].row_count == 0) {
        return NULL;
    }
    uint8_t** slot = e.game.slot[e.inv.names[n].rows[0]];
    return slot ? *slot : NULL;
}

inline uint8_t* live_named(Engine& e, const char* nm, uint8_t** ctl_out) {
    const int n = e.inv.find_exact(nm);
    return n < 0 ? NULL : live_menu(e, n, ctl_out);
}

inline uint8_t* open_named(Engine& e, const char* nm, uint8_t** ctl_out) {
    const int n = e.inv.find_exact(nm);
    return n < 0 ? NULL : open_menu(e, n, ctl_out);
}

// ---------------------------------------------------------------------------
// Hiding: the draw layer, in the registry row for every future open and in
// the live menu for the instance on screen; and the live menu's mouse byte,
// which the open does not copy from the row, so the open's post puts it on
// a new instance. The unhide gives the byte back: the instance the hide
// found gets the value it had then, any other the constructor's 1.

inline void apply_layer(Engine& e, int n, bool hidden) {
    const NameEntry& ne = e.inv.names[n];
    Placement& p = e.place[n];
    for (int i = 0; i < ne.row_count; ++i) {
        row_of(e, ne.rows[i])[kRowLayer] = hidden ? kHiddenLayer : kRowSpecs[ne.rows[i]].layer;
    }
    uint8_t* menu = live_menu(e, n, NULL);
    if (menu) {
        menu[kMenuLayer] = hidden ? kHiddenLayer : kRowSpecs[ne.rows[0]].layer;
        if (hidden) {
            p.own_mouse = menu[kMenuMouse];
            p.mouse_menu = menu;
            menu[kMenuMouse] = 0;
        } else {
            menu[kMenuMouse] = menu == p.mouse_menu ? p.own_mouse : 1;
        }
    }
    if (!hidden) {
        p.mouse_menu = NULL;
    }
    p.applied_hidden = hidden ? 1 : 0;
}

inline void reconcile_layers(Engine& e) {
    for (int n = 0; n < e.inv.count; ++n) {
        const bool want = (e.holds.want[n] & kWantHidden) != 0;
        if (want != (e.place[n].applied_hidden != 0)) {
            apply_layer(e, n, want);
        }
    }
}

// ---------------------------------------------------------------------------
// Positioning.

// The window's origin is the game's own placement unless it still sits where
// this engine last put it.
inline void note_home(Engine& e, int n, const uint8_t* menu) {
    Placement& p = e.place[n];
    const int16_t x = rd16(menu, kMenuOrigin);
    const int16_t y = rd16(menu, kMenuOrigin + 2);
    if (p.touched && x == p.last_x && (p.last_keep_dock || y == p.last_y)) {
        return;
    }
    p.home_x = x;
    p.home_y = y;
    p.home_valid = 1;
}

inline int default_height(const uint8_t* menu) {
    return rd16(menu, kMenuDefault + 6) - rd16(menu, kMenuDefault + 2);
}

inline int frame_width(const uint8_t* menu) {
    return rd16(menu, kMenuRect + 4) - rd16(menu, kMenuRect);
}

inline int frame_height(const uint8_t* menu) {
    return rd16(menu, kMenuRect + 6) - rd16(menu, kMenuRect + 2);
}

// Before the engine first changes a window, so the frame read is the game's.
inline void classify(Engine& e, int n, const uint8_t* menu) {
    if (e.anchor[n] != kAnchorUnknown) {
        return;
    }
    const bool hung = reads_bottom_anchored(e.inv.names[n].name, rd16(menu, kMenuRect + 2),
        rd16(menu, kMenuRect + 6), rd16(menu, kMenuDefault + 2), rd16(menu, kMenuDefault + 6));
    InterlockedExchange(&e.anchor[n], hung ? kAnchorBottom : kAnchorTop);
}

inline bool bottom_anchored(const Engine& e, int n) {
    return e.anchor[n] == kAnchorBottom;
}

// The origin y that puts window n's frame top at y when `by_frame`, else y
// itself. A bottom-anchored window keeps its frame's bottom at its origin
// + its default height, so its top is that less its frame height; any
// other window's frame top is its origin. Its frame's left is its origin x
// either way.
inline int origin_y(Engine& e, int n, const uint8_t* menu, int y, bool by_frame) {
    classify(e, n, menu);
    return by_frame && bottom_anchored(e, n) ? y + frame_height(menu) - default_height(menu) : y;
}

// A bottom-anchored window by its bottom edge, at w x h. SetPosition puts
// the frame's top on the origin; SetFrameRect then sets the frame to end at
// y + the default height, the bottom the game keeps. No layout: the chat
// log's, driven, empties the log.
inline void place_by_bottom(Engine& e, int n, uint8_t* menu, int x, int y, int w, int h) {
    const int default_h = default_height(menu);
    if (w < 1 || h < 1 || default_h < 1) {
        drain_error(e, "%s: its frame or its default rect has no size; left where it is",
            e.inv.names[n].name);
        return;
    }
    e.game.set_position(menu, NULL, x, y);
    e.game.set_frame_rect(menu, NULL, x, y + default_h - h, w, h, 1, 0, menu[kMenuFrameByte]);
}

inline void place(Engine& e, int n, uint8_t* menu, uint8_t* ctl, int x, int y) {
    if (bottom_anchored(e, n)) {
        place_by_bottom(e, n, menu, x, y, frame_width(menu), frame_height(menu));
        return;
    }
    e.game.set_position(menu, NULL, x, y);
    const void* set_position;
    memcpy(&set_position, &e.game.set_position, sizeof(set_position));
    if (ctl && !refreshes_itself(ctl)) {
        run_layout(ctl, set_position);
    }
}

// keep_dock leaves the window's dock bits alone: a group move shifts docked
// windows and lets docking keep owning their y.
inline void position(Engine& e, int n, uint8_t* menu, uint8_t* ctl, int x, int y, bool keep_dock) {
    classify(e, n, menu);
    note_home(e, n, menu);
    if (!keep_dock) {
        wr32(menu, kMenuPolicy, rd32(menu, kMenuPolicy) & ~kDockMask);
    }
    place(e, n, menu, ctl, x, y);
    Placement& p = e.place[n];
    p.touched = 1;
    p.last_x = rd16(menu, kMenuOrigin);
    p.last_y = rd16(menu, kMenuOrigin + 2);
    p.last_keep_dock = keep_dock ? 1 : 0;
}

inline void undock_registry(Engine& e, int n) {
    const NameEntry& ne = e.inv.names[n];
    for (int i = 0; i < ne.row_count; ++i) {
        uint8_t* row = row_of(e, ne.rows[i]);
        wr32(row, kRowPolicy, rd32(row, kRowPolicy) & ~kDockMask);
    }
    e.place[n].registry_undocked = 1;
}

inline void redock_registry(Engine& e, int n) {
    const NameEntry& ne = e.inv.names[n];
    for (int i = 0; i < ne.row_count; ++i) {
        uint8_t* row = row_of(e, ne.rows[i]);
        const uint32_t original = kRowSpecs[ne.rows[i]].policy & kDockMask;
        wr32(row, kRowPolicy, (rd32(row, kRowPolicy) & ~kDockMask) | original);
    }
    e.place[n].registry_undocked = 0;
}

// The game's own re-dock: clears the group's stored edge and places the
// window above its anchor now. Nothing for a window that does not dock.
inline void redock(Engine& e, uint8_t* menu) {
    const bool target_open = live_named(e, "targetwi", NULL) != NULL;
    const int group = dock_reset_group(rd32(menu, kMenuPolicy), target_open);
    if (group >= 0) {
        e.game.dock_reset(e.game.mcb, NULL, group, menu);
    }
}

// The cursor where it is, clamped by SetCursor to 1..+0x58.
inline void put_cursor(Engine& e, uint8_t* menu) {
    e.game.set_cursor(menu, NULL, rd16(menu, kMenuCursor), 0);
}

// What playermo's and mp_pmode's owners do before their swap: the item
// count, then the cursor clamped into it. After the swap they put the cursor
// on again, then re-dock.
inline void set_items(Engine& e, uint8_t* menu, int items) {
    wr16(menu, kMenuItems, items);
    put_cursor(e, menu);
}

// A size on the open window at its origin, a bottom-anchored window's on its
// bottom, then the re-dock its new height needs. The first size the engine
// puts on a window records the frame and the item count as they were: the
// window's own size, which a reset puts back on a window that is not
// bottom-anchored, and the count its owner set.
inline void apply_size(Engine& e, int n, uint8_t* menu, uint8_t* ctl, uint8_t kind, int a, int b) {
    Placement& p = e.place[n];
    const char* nm = e.inv.names[n].name;
    const int x = rd16(menu, kMenuOrigin);
    const int y = rd16(menu, kMenuOrigin + 2);
    char key[32];
    const Family* f = family_of(nm);
    if (kind == kSizeRows && (!f || !family_key(*f, a, key))) {
        drain_error(e, "%s: no template for %d rows", nm, a);
        return;
    }
    classify(e, n, menu);
    if (!p.size_kind) {
        p.own_w = static_cast<int16_t>(frame_width(menu));
        p.own_h = static_cast<int16_t>(frame_height(menu));
        p.own_items = rd16(menu, kMenuItems);
    }
    if (kind == kSizeRows) {
        if (f->counts) {
            set_items(e, menu, a);
        }
        e.game.template_swap(menu, NULL, key);
        place(e, n, menu, ctl, x, y);
        if (f->counts) {
            put_cursor(e, menu);
        }
        p.swapped = 1;
    } else {
        const int top = bottom_anchored(e, n) ? y + default_height(menu) - b : y;
        e.game.set_frame_rect(menu, NULL, x, top, a, b, 1, 0, menu[kMenuFrameByte]);
    }
    p.size_kind = kind;
    redock(e, menu);
}

// The window's own size back at x,y, the placement the reset leaves: after
// a rows resize, first the swap to the template it was opened from (a swap
// never changes +0x04, whose key is at +0x46), around it the item count
// recorded before the engine first sized it for an owner that sets one. Then
// a bottom-anchored window goes on its bottom at the size it has: its height
// is its content's, so a recorded one is a moment's. Any other gets
// SetFrameRect to the size recorded before the engine first sized it; the
// default rect is not that size for a window the player configured
// (logwindo's says 366x134 where the player's log is 1774x166). `move` places
// the window at x,y first.
inline void restore_size(Engine& e, int n, uint8_t* menu, uint8_t* ctl, int x, int y, bool move) {
    const Placement& p = e.place[n];
    const char* nm = e.inv.names[n].name;
    const Family* f = family_of(nm);
    const bool counts = p.swapped && f && f->counts;
    if (p.swapped) {
        char key[32];
        memset(key, 0, sizeof(key));
        memcpy(key, rdptr(menu, kMenuRes) + kResKey, kKeyLen);
        if (counts) {
            set_items(e, menu, p.own_items);
        }
        e.game.template_swap(menu, NULL, key);
        move = true;
    }
    if (bottom_anchored(e, n)) {
        place(e, n, menu, ctl, x, y);
    } else if (p.own_w < 1 || p.own_h < 1) {
        drain_error(e, "%s: its frame had no size before the first resize; left at the size it has", nm);
        if (move) {
            place(e, n, menu, ctl, x, y);
        }
    } else {
        if (move) {
            place(e, n, menu, ctl, x, y);
        }
        e.game.set_frame_rect(menu, NULL, x, y, p.own_w, p.own_h, 1, 0, menu[kMenuFrameByte]);
    }
    if (counts) {
        put_cursor(e, menu);
    }
}

// A move of the compass: nothing to place now, the remembered position
// goes on at its next draw (hook_compass_pre). The anchor the game has on it
// is its home, taken at the first move alone: later ones find the anchor
// the draw wrote.
inline void move_compass(Engine& e, int n) {
    Placement& p = e.place[n];
    const uint8_t* c = compass_object(e);
    if (c && !e.compass_home_valid) {
        InterlockedExchange(&e.compass_home, pack_home(rd16(c, kCompassX), rd16(c, kCompassY)));
        InterlockedExchange(&e.compass_home_valid, 1);
    }
    p.touched = 1;
}

// The compass back on the game's anchor, forgetting what is remembered. The
// game rewrites the anchor itself at its next show and at the chat log's
// next edge change, so a home never taken costs nothing.
inline void reset_compass(Engine& e, int n) {
    Placement& p = e.place[n];
    e.memory.clear(n);
    uint8_t* c = compass_object(e);
    if (c && e.compass_home_valid) {
        wr16(c, kCompassX, home_x(e.compass_home));
        wr16(c, kCompassY, home_y(e.compass_home));
    }
    InterlockedExchange(&e.compass_home_valid, 0);
    p.touched = 0;
}

// Back to the game's placement and the window's own size, forgetting what is
// remembered, whoever wrote it. Placement: the default rect, or for a class
// whose open hook places it, where that hook put it; a bottom-anchored
// window's frame goes on its bottom; docking re-imposes y for a window that
// docks. A window the engine never placed or sized is left as it is. Once
// the engine holds neither its placement nor a size, its anchoring is
// forgotten too, and read again at the next touch. The compass has no size
// and no menu: its position alone, back on its anchor.
inline void reset_one(Engine& e, int n, bool position, bool size) {
    if (is_compass(e, n)) {
        if (position) {
            reset_compass(e, n);
        }
        return;
    }
    Placement& p = e.place[n];
    if (position) {
        e.memory.clear(n);
        if (p.registry_undocked) {
            redock_registry(e, n);
        }
    }
    if (size) {
        e.memory.clear_size(n);
    }
    uint8_t* ctl = NULL;
    uint8_t* menu = live_menu(e, n, &ctl);
    const bool placed = menu && position && p.touched;
    const bool sized = menu && size && p.size_kind;
    if (placed || sized) {
        const NameEntry& ne = e.inv.names[n];
        int x = rd16(menu, kMenuOrigin);
        int y = rd16(menu, kMenuOrigin + 2);
        if (placed) {
            const uint32_t policy = rd32(menu, kMenuPolicy) | (kRowSpecs[ne.rows[0]].policy & kDockMask);
            wr32(menu, kMenuPolicy, policy);
            x = rd16(menu, kMenuDefault);
            y = rd16(menu, kMenuDefault + 2);
            if (open_hook_places(ne.name) && p.home_valid) {
                x = p.home_x;
                y = p.home_y;
            }
        }
        if (sized) {
            restore_size(e, n, menu, ctl, x, y, placed);
        } else {
            place(e, n, menu, ctl, x, y);
        }
        redock(e, menu);
    }
    if (position) {
        p.touched = 0;
        p.home_valid = 0;
        p.last_keep_dock = 0;
    }
    if (size) {
        p.size_kind = 0;
        p.swapped = 0;
    }
    if (!p.touched && !p.size_kind) {
        InterlockedExchange(&e.anchor[n], kAnchorUnknown);
    }
}

// The group's anchor and every open window its move carries (group_closure:
// the chat log's takes the party list's along), each shifted by (dx, dy)
// without undocking any of them, and remembered as the group's move; the
// anchor as `frame`, the frame's top-left the caller gave, when there is
// one.
inline void shift_group(Engine& e, int g, int dx, int dy, int32_t slot, uint32_t gen, const int* frame = NULL) {
    uint32_t policy[kMaxNames] = {0};
    for (int n = 0; n < e.inv.count; ++n) {
        const uint8_t* menu = open_menu(e, n, NULL);
        policy[n] = menu ? rd32(menu, kMenuPolicy) : 0;
    }
    uint8_t in_set[kMaxNames];
    group_closure(e.inv, g, policy, in_set);
    const int anchor = e.inv.find_exact(kGroups[g].anchor);
    for (int n = 0; n < e.inv.count; ++n) {
        if (!in_set[n]) {
            continue;
        }
        uint8_t* ctl = NULL;
        uint8_t* menu = open_menu(e, n, &ctl);
        if (!menu) {
            continue;
        }
        const int x = rd16(menu, kMenuOrigin) + dx;
        const int y = rd16(menu, kMenuOrigin + 2) + dy;
        position(e, n, menu, ctl, x, y, true);
        if (frame && n == anchor) {
            e.memory.write(n, frame[0], frame[1], g, slot, gen, true);
        } else {
            e.memory.write(n, x, y, g, slot, gen);
        }
    }
}

// The group whose anchor is window n and whose move waits for it, or -1.
inline int waiting_on(const Engine& e, int n) {
    for (int g = 0; g < kGroupCount; ++g) {
        if (e.waiting.g[g].active && e.inv.find_exact(kGroups[g].anchor) == n) {
            return g;
        }
    }
    return -1;
}

// A later move or reset of the anchor itself replaces a group move waiting
// for it.
inline void forget_waiting_on(Engine& e, int n) {
    const int g = waiting_on(e, n);
    if (g >= 0) {
        e.waiting.clear(g);
    }
}

// A group move replacing another handle's move waiting for the same anchor:
// that handle gets an error event, the replacing handle named when it is
// read.
inline void note_replaced(Engine& e, const Command& c) {
    const GroupWait& w = e.waiting.g[c.target];
    if (!w.active || w.slot < 0 || (w.slot == c.slot && w.gen == c.gen)) {
        return;
    }
    const uint32_t pos = e.errors.add(w.slot, w.gen, verb_name(kVerbMoveGroup), kGroups[c.target].name,
        "replaced by another handle's move", c.slot, c.gen);
    e.events.post(pack_event(kEvError, static_cast<int>(pos & 0xFFFF)));
}

// kOpGroupTo's x, y are the anchor's new origin, or with kCmdFrame its
// frame's top-left: the shift is from where the anchor is now. With its
// anchor closed, a move that may wait is kept for the anchor's next open,
// replacing any that waited before it.
inline void move_group(Engine& e, const Command& c) {
    const GroupSpec& g = kGroups[c.target];
    const int anchor = e.inv.find_exact(g.anchor);
    const uint8_t* anchor_menu = anchor >= 0 ? open_menu(e, anchor, NULL) : NULL;
    const bool by_frame = (c.flags & kCmdFrame) != 0;
    if (!anchor_menu) {
        if (c.flags & kCmdWait) {
            note_replaced(e, c);
            e.waiting.write(c.target, c.x, c.y, c.slot, c.gen, by_frame);
        } else {
            drain_error(e, "its anchor %s is not open", g.anchor);
        }
        return;
    }
    int dx = c.x;
    int dy = c.y;
    if (c.op == kOpGroupTo) {
        dx = c.x - rd16(anchor_menu, kMenuOrigin);
        dy = origin_y(e, anchor, anchor_menu, c.y, by_frame) - rd16(anchor_menu, kMenuOrigin + 2);
    }
    note_replaced(e, c);
    e.waiting.clear(c.target);
    const int frame[2] = {c.x, c.y};
    shift_group(e, c.target, dx, dy, c.slot, c.gen, c.op == kOpGroupTo && by_frame ? frame : NULL);
}

// The waiting move of group g as the game opens its anchor n: the anchor
// from where the game placed it (game_x, game_y) to the move's x, y, and
// every window the group carries that is open now by as much. Those that
// open later the game docks to the anchor where it is.
inline void carry_waiting(Engine& e, int g, int n, const uint8_t* menu, int game_x, int game_y) {
    const GroupWait& w = e.waiting.g[g];
    const bool by_frame = w.by_frame != 0;
    const int frame[2] = {w.x, w.y};
    const int dx = w.x - game_x;
    const int dy = origin_y(e, n, menu, w.y, by_frame) - game_y;
    const int32_t slot = w.slot;
    const uint32_t gen = w.gen;
    e.waiting.clear(g);
    shift_group(e, g, dx, dy, slot, gen, by_frame ? frame : NULL);
}

// The open routine re-runs the class's open hook, which may place the window
// itself, and builds the window from its template; the remembered position
// goes back on top, then the remembered size and its re-dock. A
// bottom-anchored window remembered by its frame's top-left takes its size
// first: its frame top depends on its height. Without `place_it`, the size
// alone: a group move waiting for this anchor places it.
inline void reapply(Engine& e, int n, uint8_t* menu, bool place_it) {
    const MemEntry& m = e.memory.e[n];
    if (!m.active && !m.size_kind) {
        return;
    }
    uint8_t* ctl = controller_of(e, n, menu);
    classify(e, n, menu);
    const bool place = m.active && place_it;
    const bool size_first = place && m.by_frame && m.size_kind && bottom_anchored(e, n);
    if (size_first) {
        apply_size(e, n, menu, ctl, m.size_kind, m.size_a, m.size_b);
    }
    if (place) {
        const bool docked = m.keep_dock && (rd32(menu, kMenuPolicy) & kDockMask);
        const int y = docked ? rd16(menu, kMenuOrigin + 2) : origin_y(e, n, menu, m.y, m.by_frame != 0);
        position(e, n, menu, ctl, m.x, y, m.keep_dock != 0);
    }
    if (m.size_kind && !size_first) {
        apply_size(e, n, menu, ctl, m.size_kind, m.size_a, m.size_b);
    }
}

inline void make_key(const Engine& e, int n, char out[kKeyLen + 1]) {
    memcpy(out, e.inv.names[n].key, kKeyLen);
    out[kKeyLen] = '\0';
}

inline void close_named(Engine& e, const char* nm) {
    const int n = e.inv.find_exact(nm);
    if (n < 0) {
        return;
    }
    char key[kKeyLen + 1];
    make_key(e, n, key);
    e.game.close(e.game.mcb, NULL, key);
}

// The macro keys' first block as it lands: a bar the handler has up is
// closed as the handler's own close does, the bar by name, then the object's
// bar and bar-up bytes zeroed, the set byte left alone. From here on the
// gate answers no, so the handler opens and closes nothing itself.
inline void macros_blocked(Engine& e) {
    static const char* const bars[] = {"mcr1pall", "mcr2pall"};
    for (size_t i = 0; i < sizeof(bars) / sizeof(bars[0]); ++i) {
        if (open_named(e, bars[i], NULL)) {
            close_named(e, bars[i]);
        }
    }
    uint8_t* m = macro_object(e);
    if (m && readable(m, kMacroObjectBytes)) {
        m[kMacroBar] = 0;
        m[kMacroBarUp] = 0;
    }
}

// ---------------------------------------------------------------------------
// Reply calls. Each re-checks on this
// thread what the Lua thread checked, since the game may have moved on.

// The option index the game's confirm puts at +0x30 for a value: its
// 0-based position among the nodes not tombstoned, the options the draw
// shows; -1 when none holds it.
inline int query_index_of(const uint8_t* ctl, int value) {
    const uint8_t* node = rdptr(ctl, kQueryOptions);
    int index = 0;
    for (int steps = 0; node && steps < kQueryListMax; ++steps) {
        if (node[0x14] == 0) {
            const uint8_t* item = rdptr(node, 0x10);
            if (item && static_cast<uint16_t>(rd16(item, kOptionValue)) == value) {
                return index;
            }
            ++index;
        }
        node = rdptr(node, 0);
    }
    return -1;
}

// What the game's confirm writes: the option's index at +0x30, then its
// value in the result word. The first option shown moves as the cursor
// setter moves it, just enough to keep the option among the rows shown. A
// value no option holds (engine abi 1 and 2) goes into the result word
// alone.
inline void query_answer(Engine& e, const Command& c) {
    uint8_t* ctl = NULL;
    if (!open_menu(e, c.target, &ctl)) {
        drain_error(e, "query is not open");
        return;
    }
    const int value = static_cast<int>(c.value);
    const int index = query_index_of(ctl, value);
    if ((c.flags & kCmdListed) && index < 0) {
        drain_error(e, "no option in the list has the value %d", value);
        return;
    }
    if (index >= 0) {
        int top = rd16(ctl, kQueryTop);
        if (index < top) {
            top = index;
        } else if (index - top >= kQueryRowsShown) {
            top = index - (kQueryRowsShown - 1);
        }
        wr16(ctl, kQueryCursor, index);
        wr16(ctl, kQueryTop, top);
    }
    wr16(ctl, kQueryResult, value);
}

// The game's cancel: the cancel answer, written only while the
// cancel-allowed byte is 1, and nothing else. The event's wait closes query
// by name on its next tick.
inline void query_cancel(Engine& e, int n) {
    uint8_t* ctl = NULL;
    if (!open_menu(e, n, &ctl)) {
        drain_error(e, "query is not open");
        return;
    }
    if (*e.game.query_cancel_allowed != 1) {
        drain_error(e, "query cannot be cancelled now: the event allows no cancel");
        return;
    }
    wr16(ctl, kQueryResult, kQueryCancelled);
    if (static_cast<uint16_t>(rd16(ctl, kQueryResult)) != kQueryCancelled) {
        drain_error(e, "query's result word does not read the cancel");
    }
}

// The callback, then the reset, as the game's own confirm does after it
// reads the edit widget; and the close that follows it there.
inline void passinpu_reply(Engine& e, const Command& c) {
    const bool submit = c.op == kOpPassinpuSubmit;
    uint8_t* ctl = controller_global(e, "passinpu");
    void* cb = ctl ? rdptr(ctl, kPassCallback) : NULL;
    if (!cb) {
        drain_error(e, "no text entry is pending");
        return;
    }
    const int max = static_cast<int>(rd32(ctl, kPassMax));
    if (submit && static_cast<int>(c.value) > max) {
        drain_error(e, "%u bytes, the entry takes at most %d", static_cast<unsigned>(c.value), max);
        return;
    }
    TextCallbackFn callback;
    memcpy(&callback, &cb, sizeof(callback));
    void* context = rdptr(ctl, kPassContext);
    if (submit) {
        char text[2 * kPassinpuText];
        memset(text, 0, sizeof(text));
        memcpy(text, c.text, c.value < kPassinpuText ? c.value : kPassinpuText);
        callback(context, text);
    } else {
        callback(context, NULL);
    }
    e.game.passinpu_reset(ctl, NULL);
    if (rdptr(ctl, kCtlMenu)) {
        close_named(e, "passinpu");
    }
}

// The 0x074 reply; the invite stays pending when nothing could be queued.
inline void party_reply(Engine& e, const Command& c) {
    if (!*e.game.party_pending) {
        drain_error(e, "no party invite is pending");
        return;
    }
    if (!(e.game.party_send(c.value ? 1 : 0) & 0xFF)) {
        drain_error(e, "the game could not queue the reply; the invite stays pending");
        return;
    }
    e.game.party_clear();
}

// Why an incoming post-box session cannot end through its state while a box
// window is live, and why a box window takes no reply now.
const char kBoxOpen[] = "the incoming box is open; close it with cancel('post1')";
const char kBoxClosing[] = "the incoming box is closing; cancel('post1') once it has closed";
const char kBoxCovered[] = "the incoming box is under another window";
const char kBoxWaiting[] = "the incoming box takes no input yet";
const char kDeliveryAlready[] = "the box is already closing";
const char kDeliveryCovered[] = "the outgoing box is under another window";
const char kDeliveryBusy[] = "the outgoing box is busy";

// Why the incoming box's live window (post1's instance) passes no cancel to
// its controller now, or NULL. Its busy byte lets a cancel through.
inline const char* box_cancel_refusal(const uint8_t* menu) {
    if (menu[kMenuClosing]) {
        return kBoxClosing;
    }
    if (rd32(menu, kMenuState) == kStateDormant) {
        return kBoxCovered;
    }
    return rd16(menu, kMenuInputWait) > 0 ? kBoxWaiting : NULL;
}

// Why the outgoing box's live window takes no confirm on its closing row
// now, or NULL. Its request-close leaves states 0x15 to 0x17 alone and 0x19
// to the server's reply.
inline const char* delivery_row_refusal(const uint8_t* ctl, const uint8_t* menu) {
    const int state = static_cast<uint16_t>(rd16(ctl, kDeliveryState));
    if (state == 0 || state == kDeliveryUnstage || state == kDeliveryReturn || state == kDeliveryWait
        || state == kDeliveryClosing) {
        return kDeliveryAlready;
    }
    if (rd32(menu, kMenuState) == kStateDormant) {
        return kDeliveryCovered;
    }
    return menu[kMenuBusy] ? kDeliveryBusy : NULL;
}

// The outgoing box with its window: the two words the player's confirm on
// its closing row writes; its tick then takes the staged rows and the box-2
// slots back, sends request 15 and waits for the server's reply, which
// closes the family. Never its Escape: in idle states that returns to the
// recipient entry. With no window its tick never runs: the request goes out
// here and the state that waits for the reply is set once it is queued.
// The incoming box with no window: the request, then its request-close
// presets 0x12, so the server's reply tears the session down. With a box
// window live that preset would make post1's next tick close the windows
// before the reply comes.
inline void post_close(Engine& e, int n) {
    const char* nm = e.inv.names[n].name;
    if (strcmp(nm, "delivery") == 0) {
        uint8_t* ctl = controller_global(e, "delivery");
        const uint8_t* menu = ctl ? rdptr(ctl, kCtlMenu) : NULL;
        const char* why = menu ? delivery_row_refusal(ctl, menu) : NULL;
        const int state = ctl ? rd16(ctl, kDeliveryState) : 0;
        if (why) {
            drain_error(e, "%s", why);
        } else if (menu) {
            wr16(ctl, kDeliveryNext, kDeliveryRequest);
            wr16(ctl, kDeliveryState, kDeliveryUnstage);
        } else if (state == 0) {
            drain_error(e, "no outgoing post-box session is open");
        } else if (state == kDeliveryClosing) {
            drain_error(e, "its close is already waiting for the server");
        } else if (!(e.game.post_request(kPostCloseCommand) & 0xFF)) {
            drain_error(e, "the game could not queue the request");
        } else {
            wr16(ctl, kDeliveryState, kDeliveryClosing);
        }
        return;
    }
    const uint8_t* box1 = live_named(e, "post1", NULL);
    const uint8_t* box2 = live_named(e, "post2", NULL);
    if (box1 || box2) {
        drain_error(e, "%s", menu_is_open(box1) || menu_is_open(box2) ? kBoxOpen : kBoxClosing);
        return;
    }
    uint8_t* ctl = controller_global(e, "post1");
    const uint32_t state = ctl ? rd32(ctl, kPostState) : 0;
    if (state == 0) {
        drain_error(e, "no incoming post-box session is open");
    } else if (state == kPostClosing) {
        drain_error(e, "its close is already waiting for the server");
    } else if (!(e.game.post_request(kPostCloseCommand) & 0xFF)) {
        drain_error(e, "the game could not queue the request");
    } else {
        e.game.post_request_close(ctl, NULL);
    }
}

// The incoming box's own cancel with its window live: request 15 alone, no
// state written. The server's reply marks the session closing and post1's
// next tick closes the family. With no window, post_close.
inline void post_end(Engine& e, int n) {
    uint8_t* ctl = controller_global(e, "post1");
    const uint8_t* menu = ctl ? rdptr(ctl, kCtlMenu) : NULL;
    if (!menu) {
        post_close(e, n);
        return;
    }
    const char* why = box_cancel_refusal(menu);
    if (why) {
        drain_error(e, "%s", why);
    } else if (!(e.game.post_request(kPostCloseCommand) & 0xFF)) {
        drain_error(e, "the game could not queue the request");
    }
}

// link5's controller while the event waits on its choice: the latch is 1 and
// the event's handler opened link5, mode 1 with its callback. The three
// other list openers set mode and callback to 0 and never clear the latch.
inline uint8_t* link5_pending(Engine& e) {
    uint8_t* ctl = controller_global(e, "link5");
    if (!ctl || *e.game.link5_latch != 1 || rd32(ctl, kLink5Mode) != 1
        || rdptr(ctl, kLink5Callback) != e.game.link5_callback) {
        return NULL;
    }
    return ctl;
}

// The family close; it calls the callback with no choice, which clears the
// latch the script waits on.
inline void link5_cancel(Engine& e) {
    uint8_t* ctl = link5_pending(e);
    if (!ctl) {
        drain_error(e, "no linkshell choice is pending");
        return;
    }
    e.game.link5_clear(ctl, NULL);
}

// True when a row of the list link5 shows the player holds this slot's
// concierge cache entry; row 0 is the header.
inline bool link5_lists(const Engine& e, const uint8_t* ctl, int slot) {
    const uint8_t* rows = rdptr(ctl, kLink5Rows);
    const int count = static_cast<int>(rd32(ctl, kLink5Count));
    if (!rows || slot < 0 || slot >= kLink5Slots) {
        return false;
    }
    const uint8_t* entry = e.game.link5_cache + slot * kLink5Entry;
    for (int i = 1; i < count && i < 256; ++i) {
        if (rdptr(rows + i * kLink5RowStride, kLink5RowRecord) == entry) {
            return true;
        }
    }
    return false;
}

// The choice, as link5's own confirm makes it: the callback with the slot
// and its entry in the concierge cache, then the family close, which also
// calls the callback with no choice. The event's callback ignores that
// second call but for its latch.
inline void link5_confirm(Engine& e, const Command& c) {
    uint8_t* ctl = link5_pending(e);
    void* cb = ctl ? rdptr(ctl, kLink5Callback) : NULL;
    if (!cb) {
        drain_error(e, "no linkshell choice is pending");
        return;
    }
    const int slot = static_cast<int>(c.value);
    if (slot < 0 || slot >= kLink5Slots) {
        drain_error(e, "slot %d is not 0..%d", slot, kLink5Slots - 1);
        return;
    }
    if ((c.flags & kCmdListed) && !link5_lists(e, ctl, slot)) {
        drain_error(e, "slot %d is not in the list", slot);
        return;
    }
    Link5CallbackFn callback;
    memcpy(&callback, &cb, sizeof(callback));
    callback(slot, e.game.link5_cache + slot * kLink5Entry);
    e.game.link5_clear(ctl, NULL);
}

// arealist's rows as the game last built them: the filled count, within the
// array's capacity and what a rebuild allocates; 0 with no array, as after
// a close, which frees it and keeps the count.
inline int area_count(const uint8_t* ctl) {
    const int count = static_cast<int32_t>(rd32(ctl, kAreaCount));
    const int capacity = static_cast<int32_t>(rd32(ctl, kAreaCapacity));
    if (!rdptr(ctl, kAreaRows) || count <= 0 || count > capacity || count > kAreaRowMax) {
        return 0;
    }
    return count;
}

// The first row whose entry holds `id`, 0-based, or -1.
inline int area_row_of(const uint8_t* ctl, int id) {
    const uint8_t* rows = rdptr(ctl, kAreaRows);
    const int count = area_count(ctl);
    for (int i = 0; i < count; ++i) {
        const uint8_t* entry = rdptr(rows + i * kAreaRowStride, kAreaRowEntry);
        if (entry && static_cast<int32_t>(rd32(entry, 0)) == id) {
            return i;
        }
    }
    return -1;
}

// The row under arealist's cursor and the first row shown, each 1-based in
// its `count` rows; the menu's row (+0x4C) counts only the rows shown, from
// the first (+0x1E). False when they name no row.
inline bool area_position(const uint8_t* ctl, const uint8_t* menu, int count, int* row, int* top) {
    const int first = rd16(ctl, kListScroll);
    const int at = rd16(menu, kMenuCursor) - 1 + first;
    if (first < 0 || at < first || at >= count) {
        return false;
    }
    *row = at + 1;
    *top = first + 1;
    return true;
}

const char kAreaNotAsking[] = "the area list is not asking anything; the player's keys drive it";

// Modes 1 and 2: an NPC event's list, the only ones a reply ends.
inline bool area_event_mode(int mode) {
    return mode == 1 || mode == 2;
}

// arealist's controller while an NPC event waits on its choice: the latch
// the event's opener arms before the open, in mode 1 or 2, with the window
// open or its open refused by a block.
inline uint8_t* area_event_pending(Engine& e) {
    uint8_t* ctl = controller_global(e, "arealist");
    if (!ctl || !ctl[kAreaLatch]) {
        drain_error(e, "no area choice is pending");
        return NULL;
    }
    if (!area_event_mode(ctl[kAreaMode])) {
        drain_error(e, "%s", kAreaNotAsking);
        return NULL;
    }
    return ctl;
}

// In modes 1 and 2 every row with an id from 1 up is a zone.
inline bool area_lists_zone(const uint8_t* ctl, int zone) {
    return zone >= 1 && area_row_of(ctl, zone) >= 0;
}

// close+reset's close by name reaches the begin-close that clears the latch
// and the mode only through an open instance; with none, that begin-close is
// called alone.
inline void area_end(Engine& e, uint8_t* ctl, bool window) {
    e.game.arealist_close(ctl, NULL);
    if (!window) {
        e.game.arealist_latch(ctl, NULL);
    }
}

// What the game's own Enter on a zone row does in modes 1 and 2: the zone
// into the result the event reads once the latch is clear, close+reset, and
// the search window closed by name. With the window open the zone must be
// one of its zone rows; with none there are no rows, and any zone id goes.
inline void arealist_answer(Engine& e, const Command& c) {
    const int zone = static_cast<int32_t>(c.value);
    uint8_t* ctl = area_event_pending(e);
    if (!ctl) {
        return;
    }
    const bool window = open_named(e, "arealist", NULL) != NULL;
    if (window && !area_lists_zone(ctl, zone)) {
        drain_error(e, "no zone row of the list has the id %d", zone);
        return;
    }
    if (!window && (zone < 0 || zone > kZoneMax)) {
        drain_error(e, "%d is not a zone id, 0..%d", zone, kZoneMax);
        return;
    }
    wr16(ctl, kAreaZone, zone);
    area_end(e, ctl, window);
    close_named(e, "scsibori");
    if (ctl[kAreaLatch]) {
        drain_error(e, "the area list's latch is still set");
    }
}

// The game's cancel at the top: close+reset alone, the result left at the
// -1 the open hook wrote. A refused open never ran that hook, so the -1 is
// written here, or the event would read the last answer.
inline void arealist_cancel(Engine& e) {
    uint8_t* ctl = area_event_pending(e);
    if (!ctl) {
        return;
    }
    const bool window = open_named(e, "arealist", NULL) != NULL;
    wr16(ctl, kAreaZone, -1);
    area_end(e, ctl, window);
    if (ctl[kAreaLatch]) {
        drain_error(e, "the area list's latch is still set");
    }
}

// ---------------------------------------------------------------------------
// Resizing. The window keeps its
// origin; a docked window is re-docked under its new height. Remembered for
// the session like a position, and put on at every open.

inline void resize(Engine& e, const Command& c) {
    const int n = c.target;
    const bool rows = c.op == kOpResizeRows;
    const uint8_t kind = rows ? kSizeRows : kSizeRect;
    const int a = rows ? static_cast<int>(c.value) : c.x;
    const int b = rows ? 0 : c.y;
    char key[32];
    const Family* f = family_of(e.inv.names[n].name);
    if (rows && (!f || !family_key(*f, a, key))) {
        drain_error(e, "no template for %d rows", a);
        return;
    }
    e.memory.write_size(n, kind, a, b, c.slot, c.gen);
    uint8_t* ctl = NULL;
    uint8_t* menu = live_menu(e, n, &ctl);
    if (menu) {
        apply_size(e, n, menu, ctl, kind, a, b);
    }
}

// The option under query's cursor and the first option shown, each 1-based in
// its option list; the row at +0x4C counts only the rows shown. False when the
// words name no option of the list.
inline bool query_position(const uint8_t* ctl, int* option, int* top) {
    const int32_t count = static_cast<int32_t>(rd32(ctl, kQueryCount));
    const int at = rd16(ctl, kQueryCursor);
    const int first = rd16(ctl, kQueryTop);
    if (count < 1 || count > kQueryListMax || at < 0 || at >= count || first < 0 || first > at) {
        return false;
    }
    *option = at + 1;
    *top = first + 1;
    return true;
}

// A reset of window n by the command's handle: the aspects it placed. One
// another handle placed since the call was checked refuses it whole. A
// group move of this handle waiting for n goes with its position.
inline void reset_mine(Engine& e, const Command& c) {
    const int n = c.target;
    const MemEntry& m = e.memory.e[n];
    const bool want_position = (c.value & kAspectPosition) != 0;
    const bool want_size = (c.value & kAspectSize) != 0;
    if ((want_position && m.active && !e.memory.owned_by(n, c.slot, c.gen))
        || (want_size && m.size_kind && !e.memory.size_owned_by(n, c.slot, c.gen))) {
        drain_error(e, "%s was placed by another handle since", e.inv.names[n].name);
        return;
    }
    if (want_position) {
        const int g = waiting_on(e, n);
        if (g >= 0 && e.waiting.owned_by(g, c.slot, c.gen)) {
            e.waiting.clear(g);
        }
    }
    const bool position = want_position && m.active;
    const bool size = want_size && m.size_kind;
    if (position || size) {
        reset_one(e, n, position, size);
    }
}

// A reset of group g's moves by the command's handle: its move waiting for
// the anchor, and every position that handle's moves of g wrote. Another
// handle's move of g, waiting or placed on the anchor, refuses it whole.
// The anchors go first, in the order the groups carry each other, so each
// window re-docks to an anchor already home.
inline void reset_group_mine(Engine& e, const Command& c) {
    const int g = c.target;
    const int anchor = e.inv.find_exact(kGroups[g].anchor);
    const GroupWait& w = e.waiting.g[g];
    const bool anchor_other = anchor >= 0 && e.memory.e[anchor].active && e.memory.e[anchor].group == g + 1
        && !e.memory.owned_by(anchor, c.slot, c.gen);
    if ((w.active && !e.waiting.owned_by(g, c.slot, c.gen)) || anchor_other) {
        drain_error(e, "%s was placed by another handle since", kGroups[g].name);
        return;
    }
    e.waiting.clear(g);
    uint8_t listed[kMaxNames];
    memset(listed, 0, sizeof(listed));
    for (int k = -1; k < kGroupCount + e.inv.count; ++k) {
        const int n = k < 0 ? anchor : k < kGroupCount ? e.inv.find_exact(kGroups[k].anchor) : k - kGroupCount;
        if (n < 0 || listed[n]) {
            continue;
        }
        listed[n] = 1;
        const MemEntry& m = e.memory.e[n];
        if (m.active && m.group == g + 1 && e.memory.owned_by(n, c.slot, c.gen)) {
            reset_one(e, n, true, false);
        }
    }
}

inline void watch_init(Engine& e);
inline uint32_t current_session(Engine& e, int which);

inline void run_command(Engine& e, const Command& c) {
    if ((c.flags & kCmdInstance) && c.target >= 0 && c.target < e.inv.count) {
        const int session = (c.flags & kCmdSession) ? session_window(e.inv.names[c.target].name) : -1;
        const uint32_t current = session >= 0 ? current_session(e, session)
                                              : static_cast<uint32_t>(e.instance[c.target]);
        if (current != c.instance) {
            drain_error(e, "that prompt is gone");
            return;
        }
    }
    switch (c.op) {
    case kOpMove: {
        const int n = c.target;
        const bool by_frame = (c.flags & kCmdFrame) != 0;
        forget_waiting_on(e, n);
        e.memory.write(n, c.x, c.y, -1, c.slot, c.gen, by_frame);
        if (is_compass(e, n)) {
            move_compass(e, n);
            break;
        }
        undock_registry(e, n);
        uint8_t* ctl = NULL;
        uint8_t* menu = live_menu(e, n, &ctl);
        if (menu) {
            position(e, n, menu, ctl, c.x, origin_y(e, n, menu, c.y, by_frame), false);
        }
        break;
    }
    case kOpGroup:
    case kOpGroupTo:
        move_group(e, c);
        break;
    case kOpReset:
        forget_waiting_on(e, c.target);
        reset_one(e, c.target, true, true);
        break;
    case kOpResetMine:
        reset_mine(e, c);
        break;
    case kOpResetGroupMine:
        reset_group_mine(e, c);
        break;
    case kOpResetOwned:
        for (int g = 0; g < kGroupCount; ++g) {
            if (e.waiting.owned_by(g, c.slot, c.gen)) {
                e.waiting.clear(g);
            }
        }
        for (int n = 0; n < e.inv.count; ++n) {
            const bool position = e.memory.owned_by(n, c.slot, c.gen);
            const bool size = e.memory.size_owned_by(n, c.slot, c.gen);
            if (position || size) {
                reset_one(e, n, position, size);
            }
        }
        break;
    case kOpOpen: {
        char key[kKeyLen + 1];
        make_key(e, c.target, key);
        if (!e.game.open(e.game.mcb, NULL, key, 1, 1)) {
            drain_error(e, "the game refused it");
        }
        break;
    }
    case kOpClose: {
        if (hide_only(e.inv.names[c.target].name)) {
            break;
        }
        char key[kKeyLen + 1];
        make_key(e, c.target, key);
        e.game.close(e.game.mcb, NULL, key);
        break;
    }
    case kOpBlockClose: {
        // Closed since the block: nothing to close. Close by instance marks
        // the menu at once, so one still open after the call was refused.
        if (!open_menu(e, c.target, NULL)) {
            break;
        }
        char key[kKeyLen + 1];
        make_key(e, c.target, key);
        e.game.close(e.game.mcb, NULL, key);
        if (open_menu(e, c.target, NULL)) {
            drain_error(e, "the game did not close it; the block holds");
        }
        break;
    }
    case kOpQueryAnswer:
        query_answer(e, c);
        break;
    case kOpQueryCancel:
        query_cancel(e, c.target);
        break;
    case kOpPassinpuSubmit:
    case kOpPassinpuCancel:
        passinpu_reply(e, c);
        break;
    case kOpPartyReply:
        party_reply(e, c);
        break;
    case kOpPostClose:
        post_close(e, c.target);
        break;
    case kOpPostEnd:
        post_end(e, c.target);
        break;
    case kOpLink5Cancel:
        link5_cancel(e);
        break;
    case kOpLink5Confirm:
        link5_confirm(e, c);
        break;
    case kOpArealistAnswer:
        arealist_answer(e, c);
        break;
    case kOpArealistCancel:
        arealist_cancel(e);
        break;
    case kOpMacrosBlocked:
        macros_blocked(e);
        break;
    case kOpResizeRows:
    case kOpResizeRect:
        resize(e, c);
        break;
    case kOpRestoreAll:
        for (int g = 0; g < kGroupCount; ++g) {
            e.waiting.clear(g);
        }
        reconcile_layers(e);
        for (int n = 0; n < e.inv.count; ++n) {
            const Placement& p = e.place[n];
            const MemEntry& m = e.memory.e[n];
            if (p.touched || p.registry_undocked || p.size_kind || m.active || m.size_kind || e.anchor[n]) {
                reset_one(e, n, true, true);
            }
        }
        break;
    default:
        break;
    }
}

// Whether code page 932, the OS's own table, defines a two-byte pair.
inline bool cp932_defines(const char* pair) {
    WCHAR wide;
    return MultiByteToWideChar(932, MB_ERR_INVALID_CHARS, pair, 2, &wide, 1) == 1;
}

inline void set_glyph_bit(uint8_t* bits, int g, bool on) {
    const uint8_t mask = static_cast<uint8_t>(1 << (g & 7));
    bits[g >> 3] = static_cast<uint8_t>(on ? bits[g >> 3] | mask : bits[g >> 3] & ~mask);
}

// The inverse of the converter's two-byte characters, from the converter
// itself: each Shift-JIS lead and trail pair alone, room for one glyph, no raw
// output, and for lead 0xEF trail bytes from 0x1F, the gaiji. A pair that
// yields one code of 0x60 or more names it. A code any lead-0xEF pair yields
// keeps the first such pair, defined or not: FFXI's own text spells the gaiji
// EF 1F..EF FC, where code page 932 would name other rows' pairs for the same
// codes. Of the pairs that name any other code, the first that code page 932
// defines keeps it, else the first. Once per process, and only on the game
// thread.
inline void build_glyph_table(Engine& e) {
    GlyphTable& t = e.glyphs;
    if (t.ready || !e.game.glyph_convert) {
        return;
    }
    LARGE_INTEGER start;
    LARGE_INTEGER end;
    LARGE_INTEGER frequency;
    QueryPerformanceCounter(&start);
    int32_t size = 0;
    int32_t defined = 0;
    int32_t gaiji = 0;
    for (int lead = 0x81; lead <= 0xFC; ++lead) {
        if (lead > 0x9F && lead < 0xE0) {
            continue;
        }
        for (int trail = lead == 0xEF ? 0x1F : 0x40; trail <= 0xFC; ++trail) {
            if (trail == 0x7F) {
                continue;
            }
            const char text[3] = {static_cast<char>(lead), static_cast<char>(trail), '\0'};
            int16_t glyphs[2] = {0, 0};
            if (e.game.glyph_convert(text, glyphs, 2, NULL, 0) != 1 || glyphs[0] < kGlyphTwoByte) {
                continue;
            }
            const int g = glyphs[0];
            const bool ef = lead == 0xEF;
            uint16_t& slot = t.sjis[g];
            const bool named = slot != 0;
            if (named && (t.from_gaiji(g) || (!ef && t.defined(g)))) {
                continue;
            }
            const bool cp932 = cp932_defines(text);
            if (named && !ef && !cp932) {
                continue;
            }
            if (!named) {
                ++size;
            }
            if (t.defined(g)) {
                set_glyph_bit(t.cp932, g, false);
                --defined;
            }
            slot = static_cast<uint16_t>(lead << 8 | trail);
            if (cp932) {
                set_glyph_bit(t.cp932, g, true);
                ++defined;
            }
            if (ef) {
                set_glyph_bit(t.gaiji, g, true);
                ++gaiji;
            }
        }
    }
    QueryPerformanceCounter(&end);
    QueryPerformanceFrequency(&frequency);
    t.size = size;
    t.cp932_defined = defined;
    t.gaiji_named = gaiji;
    t.build_ms = frequency.QuadPart
        ? 1000.0 * static_cast<double>(end.QuadPart - start.QuadPart) / static_cast<double>(frequency.QuadPart)
        : 0.0;
    MemoryBarrier();
    InterlockedExchange(&t.ready, 1);
}

// A window's controller through its registry row's slot, open or not.
inline const uint8_t* controller_at(const Engine& e, int n) {
    if (n < 0 || e.inv.names[n].row_count == 0) {
        return NULL;
    }
    uint8_t** slot = e.game.slot[e.inv.names[n].rows[0]];
    return slot ? *slot : NULL;
}

inline void watch_init(Engine& e) {
    if (!e.watch_ready) {
        e.watch_delivery = static_cast<int16_t>(e.inv.find_exact("delivery"));
        e.watch_post1 = static_cast<int16_t>(e.inv.find_exact("post1"));
        e.watch_ready = 1;
    }
}

// What pending() reads: the invite flag, and a post-box session in either
// box (the outgoing state word, else post1's).
inline bool session_live(Engine& e, int which) {
    if (which == kSessionInvite) {
        return e.game.party_pending && e.game.party_pending[0] != 0;
    }
    watch_init(e);
    const uint8_t* delivery = controller_at(e, e.watch_delivery);
    const uint8_t* post1 = controller_at(e, e.watch_post1);
    return (delivery && rd16(delivery, kDeliveryState) != 0) || (post1 && rd32(post1, kPostState) != 0);
}

inline uint32_t current_session(Engine& e, int which) {
    return session_id(which == kSessionInvite ? e.invite_watch : e.post_watch, session_live(e, which));
}

// Each session that appears is counted, and each change posts a pending
// event.
inline void watch_session(Engine& e, int which, volatile LONG* watch) {
    const bool live = session_live(e, which);
    const LONG w = *watch;
    if (live == session_seen(w)) {
        return;
    }
    InterlockedExchange(watch, session_watch(session_count(w) + (live ? 1 : 0), live));
    e.events.post(pack_event(kEvPending, pending_payload(which == kSessionInvite ? kPendingInvite : kPendingPost,
        live)));
}

// Before the commands, so every session up at a frame's start has its id
// when a reply to it is checked; after them, so one a reply ended is seen
// gone the same frame.
inline void watch_pending(Engine& e) {
    watch_session(e, kSessionInvite, &e.invite_watch);
    watch_session(e, kSessionPost, &e.post_watch);
}

// A cursor event for each open window whose row (+0x4C) changed since the
// drain last read it; for query and arealist, the option or row under the
// cursor (query_position, area_position), which a scroll moves while the
// row stays. A window is first read, posting nothing, on the frame it is
// seen open or seen as another instance, and forgotten once it is not open.
// A query or arealist cursor on nothing in the list is not read.
inline void watch_cursors(Engine& e) {
    for (int n = 0; n < e.inv.count; ++n) {
        uint8_t* ctl = NULL;
        const uint8_t* menu = open_menu(e, n, &ctl);
        if (!menu) {
            e.row_menu[n] = NULL;
            continue;
        }
        int row = rd16(menu, kMenuCursor);
        int top = 0;
        const char* nm = e.inv.names[n].name;
        if (strcmp(nm, "query") == 0 && !query_position(ctl, &row, &top)) {
            continue;
        }
        if (strcmp(nm, "arealist") == 0 && !area_position(ctl, menu, area_count(ctl), &row, &top)) {
            continue;
        }
        if (e.row_menu[n] != menu) {
            e.row_menu[n] = menu;
            e.last_row[n] = static_cast<int16_t>(row);
            continue;
        }
        if (row != e.last_row[n]) {
            e.last_row[n] = static_cast<int16_t>(row);
            e.events.post(pack_cursor(n, row));
        }
    }
}

// The compass has no open or close of its own: opened and closed are its
// state byte leaving and reaching 0, as the drain sees it each frame. The
// manager shows it again on every frame it finds it at 0 in the world, and
// hides it for an event.
inline void watch_compass(Engine& e) {
    const int n = e.inv.compass;
    if (n < 0) {
        return;
    }
    const bool open = compass_open(compass_object(e));
    if (open == (e.compass_shown != 0)) {
        return;
    }
    e.compass_shown = open ? 1 : 0;
    e.events.post(pack_event(open ? kEvOpened : kEvClosed, n));
}

inline void drain(Engine& e) {
    build_glyph_table(e);
    reconcile_layers(e);
    watch_pending(e);
    Command c;
    while (e.commands.pop(&c)) {
        e.current_thread = GetCurrentThreadId();
        e.current = &c;
        run_command(e, c);
        e.current = NULL;
        if ((c.flags & kCmdReply) && c.target >= 0 && c.target < kMaxNames) {
            InterlockedDecrement(&e.replies[c.target]);
        }
        InterlockedIncrement(&e.completed);
    }
    watch_pending(e);
    watch_compass(e);
    // The game's close of a dormant menu marks it and defers its destruction
    // without a staged close, so a covered window that closes is noticed
    // here, at the marks.
    for (int n = 0; n < e.inv.count; ++n) {
        if (e.covered[n] && !open_menu(e, n, NULL)) {
            e.covered[n] = 0;
            e.events.post(pack_event(kEvClosed, n));
        }
    }
    watch_cursors(e);
}

// ---------------------------------------------------------------------------
// Hook handlers. `user` is the Engine.

// Open by name: args are (key, activate, overlap_aware), this = the manager.
// A blocked name returns 0, the game's own "not available". Every open of a
// window is counted, a refused one too: it is a new prompt all the same.
inline int __cdecl hook_open_pre(HuFrame* f) {
    Engine& e = *static_cast<Engine*>(f->user);
    const char* key = reinterpret_cast<const char*>(f->args[0]);
    const int n = key ? e.inv.find_key(key) : -1;
    if (n >= 0) {
        InterlockedIncrement(&e.instance[n]);
    }
    if (n >= 0 && (e.holds.want[n] & kWantBlocked)) {
        e.events.post(pack_event(kEvBlocked, n));
        f->result = 0;
        return 1;
    }
    if (n < 0) {
        InterlockedIncrement(&e.unmatched_keys);
    }
    e.opens.push(f->args, GetCurrentThreadId(), static_cast<int16_t>(n));
    return 0;
}

inline void __cdecl hook_open_post(HuFrame* f) {
    Engine& e = *static_cast<Engine*>(f->user);
    int16_t n = -1;
    if (!e.opens.pop(f->args, GetCurrentThreadId(), &n)) {
        return;
    }
    uint8_t* menu = reinterpret_cast<uint8_t*>(f->result);
    if (!menu) {
        return;
    }
    if (n < 0) {
        n = static_cast<int16_t>(name_of_menu(e, menu));
        if (n < 0) {
            return;
        }
    }
    if (e.place[n].applied_hidden) {
        menu[kMenuMouse] = 0;
    }
    const int waiting = waiting_on(e, n);
    const int game_x = rd16(menu, kMenuOrigin);
    const int game_y = rd16(menu, kMenuOrigin + 2);
    reapply(e, n, menu, waiting < 0);
    if (waiting >= 0) {
        carry_waiting(e, waiting, n, menu, game_x, game_y);
    }
    e.covered[n] = 0;
    e.events.post(pack_event(kEvOpened, n));
}

// The show path, arg (menu): a menu becoming visible. Inside an open of the
// same menu the open's post reports it; outside one it comes back from under
// another window.
inline int __cdecl hook_show_pre(HuFrame* f) {
    Engine& e = *static_cast<Engine*>(f->user);
    const uint8_t* menu = reinterpret_cast<const uint8_t*>(f->args[0]);
    if (!menu) {
        return 0;
    }
    const int n = name_of_menu(e, menu);
    if (n >= 0 && !e.opens.contains(GetCurrentThreadId(), static_cast<int16_t>(n))) {
        e.covered[n] = 0;
        e.events.post(pack_event(kEvUncovered, n));
    }
    return 0;
}

// The staged close, first arg (menu): a menu leaving the screen. Close by
// instance marks it closing first; the update loop, and controllers hiding
// their own lists, send a menu dormant through the same routine with the mark
// clear. On a menu already dormant the routine changes nothing.
inline int __cdecl hook_close_pre(HuFrame* f) {
    Engine& e = *static_cast<Engine*>(f->user);
    const uint8_t* menu = reinterpret_cast<const uint8_t*>(f->args[0]);
    if (!menu || rd32(menu, kMenuState) == kStateDormant) {
        return 0;
    }
    const int n = name_of_menu(e, menu);
    if (n >= 0) {
        const bool closing = menu[kMenuClosing] != 0;
        e.covered[n] = closing ? 0 : 1;
        e.events.post(pack_event(closing ? kEvClosed : kEvCovered, n));
    }
    return 0;
}

// The per-frame UI update: the one place queued work reaches the game.
inline int __cdecl hook_update_pre(HuFrame* f) {
    drain(*static_cast<Engine*>(f->user));
    return 0;
}

// The mouse mode picker, ECX = the mouse object, no arguments, nothing
// returned: mode 2 while the manager has a menu up, else 1. Every mouse path
// into a menu needs mode 2, and mouse input goes only to the active menu, so
// while that menu is one this engine hid the pointer stays in world mode and
// the original does not run.
inline int __cdecl hook_mouse_mode_pre(HuFrame* f) {
    const Engine& e = *static_cast<const Engine*>(f->user);
    const uint8_t* active = e.game.mcb ? rdptr(e.game.mcb, kMcbActive) : NULL;
    if (!active || active[kMenuLayer] != kHiddenLayer) {
        return 0;
    }
    reinterpret_cast<uint8_t*>(f->ecx)[kMouseMode] = kMouseWorld;
    return 1;
}

// The menu input sink, ECX = a menu, arg (code), returning a word its
// callers ignore. Every key and gamepad code reaches the active menu through
// the routing routine's one call to it; while that menu is one this engine
// hid, a code from that call never reaches it and the sink answers 0, as it
// does for a dormant menu. Its other callers -- SetCursor's code 9, the
// wheel, a controller passing a code on -- are not the player's keys.
inline int __cdecl hook_menu_input_pre(HuFrame* f) {
    const Engine& e = *static_cast<const Engine*>(f->user);
    if (f->return_address != e.game.routing_return || !e.game.mcb) {
        return 0;
    }
    const uint8_t* active = rdptr(e.game.mcb, kMcbActive);
    if (!active || reinterpret_cast<uintptr_t>(active) != f->ecx || active[kMenuLayer] != kHiddenLayer) {
        return 0;
    }
    f->result = 0;
    return 1;
}

// The compass draw entry, no arguments, nothing returned. A remembered
// position is written on every call, hidden or not, because the game
// rewrites the anchor on each show and on each change of the chat log's
// edge, and a reader expects the box where the move put it. The position is
// the box's top-left, or the anchor itself from a move by the origin. The
// drain runs earlier in the same frame, so the memory it wrote is read here
// directly. While an owner hides the compass the original does not run:
// nothing is drawn and its fade state stands still.
inline int __cdecl hook_compass_pre(HuFrame* f) {
    Engine& e = *static_cast<Engine*>(f->user);
    const int n = e.inv.compass;
    if (n < 0) {
        return 0;
    }
    const MemEntry& m = e.memory.e[n];
    uint8_t* c = compass_object(e);
    if (m.active && c) {
        wr16(c, kCompassX, m.by_frame ? m.x + kCompassWidth : m.x);
        wr16(c, kCompassY, m.by_frame ? m.y + rd16(c, kCompassHeight) : m.y);
    }
    return (e.holds.want[n] & kWantHidden) ? 1 : 0;
}

// The macro key gate, no arguments, al 1 while the macro keys may act. The
// handler consults it before opening a bar and before each number key, so
// with the gate at no nothing of the macro keys runs, as during a cutscene;
// while any handle blocks them the original does not run and the gate
// answers no.
inline int __cdecl hook_macro_gate_pre(HuFrame* f) {
    const Engine& e = *static_cast<const Engine*>(f->user);
    if (!e.holds.macros_want) {
        return 0;
    }
    f->result = 0;
    return 1;
}

}  // namespace hu

#endif  // HIDEUI_GAME_H_
