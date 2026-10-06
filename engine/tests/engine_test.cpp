// engine_test - the engine's game-free parts under wine: the inventory and
// name lookup, the signature table and scanner, registry verification, hold
// and handle accounting, the queues under concurrent load, the error log, the
// reply writer, the glyph decoder and the table the drain builds from a
// stand-in converter, and game.h's drain and hook handlers driven against
// fabricated menus, including the events (opened, closed, covered,
// uncovered, blocked, and errors kept for the handle whose command failed),
// the reply calls, link5's pending test, the listed checks and reply counts,
// resize and the item count around an action menu's swap, remembered sizes,
// resets back to the size a window had before its first resize, the
// re-dock, bottom-anchored windows placed by their bottom edge (the chat logs
// and the party list by class, every other window read off its frame; the
// chat log and the party list at their measured numbers), the chat log
// carrying the party list, a group placed by its anchor and a group move
// waiting for its anchor's open, the count of each window's opens and a
// reply refused once its prompt is gone, the incoming box ended through its
// own cancel, the pending events the drain posts, and the mouse kept off a
// hidden window: its live menu's +0x77 at the hide, the unhide (the value
// the hide found, or a new instance's 1) and a new instance's open, and the
// mode picker's pre; close by instance with the instance freed frames after
// its marks, open-ness read off the marks; and the scans of a class's code
// reading past a C3 or an FF 60 that is part of an operand. 0.7.0's own: a
// move and a group move taking the frame's top-left (a bottom-anchored
// window converted, at once and at its open), the error a waiting group move
// replaced by another handle's posts, the resets of what one handle placed,
// the invite and post-box session ids, cursor events, and a block's close
// of an open window. 0.7.2's own: query's cursor events as the option under
// its cursor, through a list longer than the rows shown. 0.7.3's own:
// arealist on the game's layout (fake_arealist.h): its rows found by id and
// its cursor events as the row under the cursor. 0.7.4's own: query's
// answer writes the option's index (+0x30), the first option shown (+0x32)
// and the value. arealist's replies end an NPC event's choice (modes 1 and
// 2) with the window open or blocked: the zone written, close+reset,
// scsibori closed, and with no open instance the latch clear called alone; a
// cancel leaves -1; mode 0, 3 and 4, a region's id and an unlisted zone
// refused; the answer leaves the list as the player's own Enter on that zone
// does. 0.7.5's own: no reply goes through a menu's input routine. query's
// cancel writes 0xFF while the cancel-allowed byte is 1 and closes nothing.
// The incoming box's cancel with its window live is request 15 alone, no
// state written, refused while the window is closing, covered or takes no
// input yet, its busy byte no bar; the game's reply and tick then close it.
// The outgoing box's with its window live writes +0x1B6 = 0x18 then +0x1B4 =
// 0x15, refused while busy or covered and in states 0, 0x15 to 0x17 and
// 0x19. With no window, both as before. 0.7.6's own: keys and the gamepad
// kept off a hidden window: the sink's pre drops the routing routine's call
// to the active menu's sink while that menu is hidden, and passes the same
// call to a visible one, any other caller's (SetCursor's code 9, the wheel),
// and the call with no active menu, where the routing routine's own branch
// runs; the unhide gives the keys back.

#include "../signatures.h"
#include "fake_glyph_convert.h"
#include "fake_arealist.h"

#include <cstdio>

using namespace hu;

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

Engine g_e;

// ---------------------------------------------------------------------------
// A fabricated game: registry, controller globals, controllers, menus, the
// manager, and stand-ins for SetPosition, open and close.

uint8_t g_registry[(kRowCount + 1) * kRowStride];
uint8_t g_pristine[(kRowCount + 1) * kRowStride];
uint8_t* g_ctl_global[kRowCount];
uint8_t g_mcb[kMcbBytes];

struct FakeCtl {
    uint8_t bytes[0x600];
};
struct FakeMenu {
    uint8_t bytes[kMenuBytes];
};
struct FakeRes {
    uint8_t bytes[0x60];
};

FakeCtl g_ctl[32];
int g_ctl_used;
FakeMenu g_menus[64];
FakeRes g_res[64];
int g_menus_used;

uint8_t g_nop_code[0x200] = {0x90, 0xC3};
uint8_t g_ret_code[0x200] = {0xC3};
uint8_t g_open_tail[0x200] = {0x8B, 0x01, 0xFF, 0x60, 0x44, 0xC3};
uint8_t g_open_fixed[0x200];
// Open hooks with a C3 inside an imm32 (mov eax,0xC3) and an FF 60 inside a
// displacement (mov eax,[eax+0x1060FF]) ahead of their real end: a call to
// SetPosition then pop esi; ret, or the tail jump to vt+0x44. The second
// tail jump is the game's other form: mov ecx,edx; pop edi; jmp *0x44(%eax).
uint8_t g_open_operands[0x200] = {0xB8, 0xC3, 0x00, 0x00, 0x00, 0x8B, 0x80, 0xFF, 0x60, 0x10, 0x00};
uint8_t g_open_operand_tail[0x200] = {0xB8, 0xC3, 0x00, 0x00, 0x00, 0x8B, 0x80, 0xFF, 0x60, 0x10, 0x00,
                                      0x8B, 0x01, 0xFF, 0x60, 0x44};
uint8_t g_open_pop_tail[0x200] = {0xB8, 0xC3, 0x00, 0x00, 0x00, 0x8B, 0x02, 0x8B, 0xCA, 0x5F, 0xFF, 0x60, 0x44};
// A real end, pop esi; ret, and past it a call to SetPosition in whatever
// routine follows.
uint8_t g_open_ends[0x200] = {0x5E, 0xC3};
const void* vt_self[0x20];
const void* vt_layout[0x20];
const void* vt_fixed[0x20];
const void* vt_query[0x20];
const void* vt_box[0x20];
const void* vt_area[0x20];
const void* vt_operands[0x20];
const void* vt_operand_tail[0x20];
const void* vt_pop_tail[0x20];
const void* vt_ends[0x20];

int g_setpos_calls;
int g_open_calls;
int g_close_calls;
char g_open_key[kKeyLen + 1];   // the key the open routine was given last
// The close as the game's runs it: the instance stays bound after the marks
// until destroy() stands for the update loop freeing it. Clear, the close
// frees it at once.
bool g_close_keeps;
// Set, the close finds the window and refuses it, leaving it open.
bool g_close_refuses;

// What the reply, re-dock and resize stand-ins saw, in call order.
struct CallLog {
    char order[64];             // one letter per call
    int count;
    int dock_group;
    void* dock_menu;
    int dock_resets;
    char swap_key[33];
    void* swap_menu;
    int frame[7];
    void* frame_menu;
    int query_event;
    int query_inputs;
    void* text_context;
    char text[33];
    bool text_null;
    int text_calls;
    int party_value;
    bool party_queues;
    uint8_t party_pending;
    int post_command;
    bool post_queues;
    char last_close[17];
    int link5_slot;
    const void* link5_entry;
    int cursor_calls;
    void* cursor_menu;
    int cursor_row[4];          // the first four calls' arguments
    int cursor_warp[4];
    int cursor_items[4];        // +0x58 as each of them found it
    int box_event;              // post1's input routine
    int box_row;
    void* box_ctl;
};
CallLog g_log;
uint8_t g_link5_cache[kLink5Slots * kLink5Entry];
uint8_t g_link5_latch;
uint8_t g_query_cancel_allowed;

// A controller's input(event, row), its vtable slot, and its cancel event:
// the player's keys. No reply reaches it.
const size_t kCtlInput = 0x18;
const int kEventCancel = 6;

void logged(char what) {
    if (g_log.count + 1 < static_cast<int>(sizeof(g_log.order))) {
        g_log.order[g_log.count++] = what;
        g_log.order[g_log.count] = '\0';
    }
}

void clear_log() {
    const bool party = g_log.party_queues;
    const bool post = g_log.post_queues;
    const uint8_t pending = g_log.party_pending;
    memset(&g_log, 0, sizeof(g_log));
    g_log.party_queues = party;
    g_log.post_queues = post;
    g_log.party_pending = pending;
}

}  // namespace

extern "C" volatile LONG g_layout_calls;
extern "C" volatile int16_t g_guard_seen;
volatile LONG g_layout_calls;
volatile int16_t g_guard_seen;

// A layout whose one-shot guard is the word at +0x1A, the shape run_layout
// reads out of a class's code.
extern "C" __attribute__((naked)) void fake_layout() {
    asm volatile(
        "pushl %esi\n\t"
        "movl %ecx, %esi\n\t"
        "cmpw %bp, 0x1a(%esi)\n\t"
        "movw 0x1a(%esi), %ax\n\t"
        "movw %ax, _g_guard_seen\n\t"
        "movw $1, 0x1a(%esi)\n\t"
        "lock incl _g_layout_calls\n\t"
        "popl %esi\n\t"
        "ret\n\t");
}

namespace {

void __fastcall fake_set_position(void* menu, void*, int x, int y) {
    uint8_t* m = static_cast<uint8_t*>(menu);
    const int w = rd16(m, kMenuRect + 4) - rd16(m, kMenuRect);
    const int h = rd16(m, kMenuRect + 6) - rd16(m, kMenuRect + 2);
    wr16(m, kMenuOrigin, x);
    wr16(m, kMenuOrigin + 2, y);
    wr16(m, kMenuRect, x);
    wr16(m, kMenuRect + 2, y);
    wr16(m, kMenuRect + 4, x + w);
    wr16(m, kMenuRect + 6, y + h);
    ++g_setpos_calls;
}

int name(const char* nm) {
    return g_e.inv.find_exact(nm);
}

void __fastcall fake_dock_reset(void* mcb, void*, int group, void* menu) {
    logged('D');
    ++g_log.dock_resets;
    g_log.dock_group = group;
    g_log.dock_menu = menu;
    wr16(static_cast<uint8_t*>(mcb), 0xA0 + 2 * group, 0);
}

// Snaps the frame to a template position, as the game's swap does, and
// leaves the origin.
void __fastcall fake_template_swap(void* menu, void*, const char* key) {
    logged('S');
    memcpy(g_log.swap_key, key, 32);
    g_log.swap_key[32] = '\0';
    g_log.swap_menu = menu;
    uint8_t* m = static_cast<uint8_t*>(menu);
    wr16(m, kMenuRect, 7);
    wr16(m, kMenuRect + 2, 9);
}

// Rebuilds the frame's bounding rect as the game's does.
void __fastcall fake_set_frame_rect(void* menu, void*, int x, int y, int w, int h, int one, int keep, int b9a) {
    logged('F');
    const int args[7] = {x, y, w, h, one, keep, b9a};
    memcpy(g_log.frame, args, sizeof(args));
    g_log.frame_menu = menu;
    uint8_t* m = static_cast<uint8_t*>(menu);
    wr16(m, kMenuRect, x);
    wr16(m, kMenuRect + 2, y);
    wr16(m, kMenuRect + 4, x + w);
    wr16(m, kMenuRect + 6, y + h);
}

// Clamps the row to 1..+0x58 and writes +0x4C, as the game's does.
void __fastcall fake_set_cursor(void* menu, void*, int row, int warp) {
    logged('U');
    uint8_t* m = static_cast<uint8_t*>(menu);
    const int call = g_log.cursor_calls++;
    g_log.cursor_menu = menu;
    const int items = rd16(m, kMenuItems);
    const int r = static_cast<int16_t>(row);
    if (call < 4) {
        g_log.cursor_row[call] = r;
        g_log.cursor_warp[call] = warp & 0xFF;
        g_log.cursor_items[call] = items;
    }
    int at = r < 1 ? 1 : r;
    if (at > items) {
        at = items;
    }
    wr16(m, kMenuCursor, at);
}

bool frame_is(int x, int y, int w, int h) {
    return g_log.frame[0] == x && g_log.frame[1] == y && g_log.frame[2] == w && g_log.frame[3] == h
        && g_log.frame[4] == 1 && g_log.frame[5] == 0;
}

void set_rect(uint8_t* menu, int x, int y, int right, int bottom) {
    wr16(menu, kMenuRect, x);
    wr16(menu, kMenuRect + 2, y);
    wr16(menu, kMenuRect + 4, right);
    wr16(menu, kMenuRect + 6, bottom);
}

bool rect_is(const uint8_t* menu, int x, int y, int right, int bottom) {
    return rd16(menu, kMenuRect) == x && rd16(menu, kMenuRect + 2) == y && rd16(menu, kMenuRect + 4) == right
        && rd16(menu, kMenuRect + 6) == bottom;
}

// query's input routine, the player's keys: no reply reaches it.
uint32_t __fastcall fake_query_input(void* ctl, void*, int event, int row) {
    logged('Q');
    ++g_log.query_inputs;
    g_log.query_event = event;
    if (event == kEventCancel && g_query_cancel_allowed == 1) {
        wr16(static_cast<uint8_t*>(ctl), kQueryResult, kQueryCancelled);
    }
    return 1;
}

// The game's setter of query's cursor on Down: the option after the
// cursor's, clamped into the list, the first option shown moved just enough
// to keep it among the three rows shown, the row derived from both.
const int kQueryRowsShown = 3;

void query_down(uint8_t* ctl) {
    const int count = static_cast<int32_t>(rd32(ctl, kQueryCount));
    int at = rd16(ctl, kQueryCursor) + 1;
    at = at > count - 1 ? count - 1 : at;
    at = at < 0 ? 0 : at;
    int top = rd16(ctl, kQueryTop);
    if (at < top) {
        top = at;
    } else if (at > top + kQueryRowsShown - 1) {
        top = at - (kQueryRowsShown - 1);
    }
    wr16(ctl, kQueryCursor, at);
    wr16(ctl, kQueryTop, top);
    wr16(rdptr(ctl, kCtlMenu), kMenuCursor, at - top + 1);
}

// The game's confirm: the value of the option at +0x30, counting every node
// linked, into the result word.
bool query_confirm(uint8_t* ctl) {
    const uint8_t* node = rdptr(ctl, kQueryOptions);
    for (int i = rd16(ctl, kQueryCursor); node && i > 0; --i) {
        node = rdptr(node, 0);
    }
    const uint8_t* item = node ? rdptr(node, 0x10) : NULL;
    if (!item) {
        return false;
    }
    wr16(ctl, kQueryResult, rd16(item, kOptionValue));
    return true;
}

// post1's input(event, row): records what the box was given.
uint32_t __fastcall fake_box_input(void* ctl, void*, int event, int row) {
    logged('I');
    g_log.box_event = event;
    g_log.box_row = row;
    g_log.box_ctl = ctl;
    return 1;
}

void __cdecl fake_text_callback(void* context, const char* text) {
    logged('C');
    ++g_log.text_calls;
    g_log.text_context = context;
    g_log.text_null = text == NULL;
    if (text) {
        memcpy(g_log.text, text, 32);
    }
}

uint32_t __fastcall fake_passinpu_reset(void* ctl, void*) {
    logged('R');
    uint8_t* c = static_cast<uint8_t*>(ctl);
    wr32(c, kPassCallback, 0);
    wr32(c, kPassContext, 0);
    return 0;
}

// Only al is the answer: the rest of eax is left dirty on purpose.
uint32_t __cdecl fake_party_send(int accept) {
    logged('P');
    g_log.party_value = accept;
    return g_log.party_queues ? 0xABCD0001u : 0xABCD0000u;
}

void __cdecl fake_party_clear() {
    logged('X');
    g_log.party_pending = 0;
}

uint32_t __fastcall fake_link5_clear(void* ctl, void*) {
    logged('L');
    return 0;
}

void __cdecl fake_link5_callback(int slot, const void* entry) {
    logged('J');
    g_log.link5_slot = slot;
    g_log.link5_entry = entry;
}

// arealist's close+reset and latch clear, as the engine calls them.
uint32_t __fastcall fake_arealist_close(void* ctl, void*) {
    logged('A');
    return fake_area::close_reset(static_cast<uint8_t*>(ctl));
}

uint32_t __fastcall fake_arealist_latch(void* ctl, void*) {
    logged('H');
    return fake_area::latch_clear(static_cast<uint8_t*>(ctl), NULL);
}

// A begin-close that leaves the latch set.
uint32_t __fastcall keeping_begin_close(uint8_t*, void*) {
    return 0;
}

uint32_t __cdecl fake_post_request(int command) {
    logged('B');
    g_log.post_command = command;
    return g_log.post_queues ? 0x77777701u : 0x77777700u;
}

uint32_t __fastcall fake_post_request_close(void* ctl, void*) {
    logged('Y');
    uint8_t* c = static_cast<uint8_t*>(ctl);
    if (rd32(c, kPostState) != 0 && rd32(c, kPostState) != kPostClosing) {
        wr32(c, kPostState, kPostClosing);
    }
    return 1;
}

int first_row(const char* nm) {
    return g_e.inv.names[name(nm)].rows[0];
}

uint8_t* row(int r) {
    return g_registry + r * kRowStride;
}

// A controller for `nm`, published in the global of the given row (the
// name's first row by default).
uint8_t* give_controller(const char* nm, const void* const* vt, int which_row = 0) {
    uint8_t* ctl = g_ctl[g_ctl_used++].bytes;
    memset(ctl, 0, sizeof(FakeCtl));
    const void* vtp = vt;
    memcpy(ctl, &vtp, 4);
    g_ctl_global[g_e.inv.names[name(nm)].rows[which_row]] = ctl;
    return ctl;
}

uint8_t* controller(const char* nm) {
    const NameEntry& ne = g_e.inv.names[name(nm)];
    for (int i = 0; i < ne.row_count; ++i) {
        if (g_ctl_global[ne.rows[i]]) {
            return g_ctl_global[ne.rows[i]];
        }
    }
    return NULL;
}

struct Geometry {
    int x;
    int y;
    int w;
    int h;
    int default_x;
    int default_y;
};

Geometry geometry_of(const char* nm) {
    struct Entry {
        const char* name;
        Geometry g;
    };
    static const Entry table[] = {
        {"logwindo", {16, 930, 366, 134, 16, 930}},
        {"logwin2", {16, 760, 366, 134, 16, 760}},
        {"ability", {10, 340, 120, 58, 10, 200}},
        {"partywin", {500, 380, 120, 60, 500, 380}},
        {"targetwi", {500, 330, 112, 44, 500, 100}},
        {"subwindo", {500, 290, 100, 38, 500, 50}},
        {"equip", {130, 48, 200, 160, 130, 48}},
        {"persona", {16, 48, 180, 200, 100, 100}},
        {"query", {20, 200, 200, 80, 20, 200}},
        {"conf1win", {40, 40, 100, 100, 40, 40}},
        {"buff", {0, 0, 200, 20, 0, 0}},
    };
    for (size_t i = 0; i < sizeof(table) / sizeof(table[0]); ++i) {
        if (strcmp(table[i].name, nm) == 0) {
            return table[i].g;
        }
    }
    Geometry none = {0, 0, 10, 10, 0, 0};
    return none;
}

// What the open routine does to a new instance, as far as the engine sees it:
// the registry row copied in, the template position written to the origin
// and the default rect, then a class's open hook placing it.
uint8_t* instantiate(int n, uint8_t* ctl) {
    uint8_t* menu = g_menus[g_menus_used].bytes;
    uint8_t* res = g_res[g_menus_used].bytes;
    ++g_menus_used;
    memset(menu, 0, sizeof(FakeMenu));
    memset(res, 0, sizeof(FakeRes));
    const NameEntry& ne = g_e.inv.names[n];
    memcpy(res + kResKey, ne.key, kKeyLen);
    memcpy(menu + kMenuRes, &res, 4);
    const uint8_t* r = row(ne.rows[0]);
    menu[kMenuLayer] = r[kRowLayer];
    menu[kMenuMouse] = 1;           // the constructor's
    wr32(menu, kMenuPolicy, rd32(r, kRowPolicy));
    const Geometry g = geometry_of(ne.name);
    wr16(menu, kMenuDefault, g.default_x);
    wr16(menu, kMenuDefault + 2, g.default_y);
    wr16(menu, kMenuDefault + 4, g.default_x + g.w);
    wr16(menu, kMenuDefault + 6, g.default_y + g.h);
    wr16(menu, kMenuRect, g.default_x);
    wr16(menu, kMenuRect + 2, g.default_y);
    wr16(menu, kMenuRect + 4, g.default_x + g.w);
    wr16(menu, kMenuRect + 6, g.default_y + g.h);
    wr16(menu, kMenuOrigin, g.default_x);
    wr16(menu, kMenuOrigin + 2, g.default_y);
    if (g.x != g.default_x || g.y != g.default_y) {
        fake_set_position(menu, NULL, g.x, g.y);
    }
    memcpy(ctl + kCtlMenu, &menu, 4);
    return menu;
}

uint8_t* live(const char* nm) {
    uint8_t* ctl = controller(nm);
    return ctl ? rdptr(ctl, kCtlMenu) : NULL;
}

void* __fastcall fake_open(void*, void*, const char* key, int, int) {
    ++g_open_calls;
    size_t k = 0;
    for (; k < kKeyLen && key[k]; ++k) {
        g_open_key[k] = key[k];
    }
    g_open_key[k] = '\0';
    const int n = g_e.inv.find_key(key);
    if (n < 0) {
        return NULL;
    }
    uint8_t* ctl = controller(g_e.inv.names[n].name);
    if (!ctl) {
        return NULL;
    }
    uint8_t* menu = rdptr(ctl, kCtlMenu);
    if (menu) {
        // The open reuses an instance it finds, a closing one too, and clears
        // both marks on it.
        menu[kMenuClosing] = 0;
        menu[kMenuPendingDestroy] = 0;
        const uint8_t* r = row(g_e.inv.names[n].rows[0]);
        menu[kMenuLayer] = r[kRowLayer];
        wr32(menu, kMenuPolicy, rd32(r, kRowPolicy));
        return menu;
    }
    menu = instantiate(n, ctl);
    uint32_t args[1] = {reinterpret_cast<uint32_t>(menu)};
    HuFrame f;
    memset(&f, 0, sizeof(f));
    f.args = args;
    f.user = &g_e;
    hook_show_pre(&f);
    return menu;
}

// The open routine as the daemon presents it: pre, the original unless pre
// refused, post.
void* __fastcall hooked_open(void* mcb, void* edx, const char* key, int activate, int overlap) {
    uint32_t args[3] = {reinterpret_cast<uint32_t>(key), static_cast<uint32_t>(activate),
                        static_cast<uint32_t>(overlap)};
    HuFrame f;
    memset(&f, 0, sizeof(f));
    f.args = args;
    f.user = &g_e;
    if (hook_open_pre(&f)) {
        return reinterpret_cast<void*>(f.result);
    }
    f.result = reinterpret_cast<uint32_t>(fake_open(mcb, edx, key, activate, overlap));
    hook_open_post(&f);
    return reinterpret_cast<void*>(f.result);
}

int __fastcall fake_close(void*, void*, const char* key) {
    ++g_close_calls;
    if (g_close_refuses) {
        return 0;
    }
    logged('K');
    memcpy(g_log.last_close, key, 16);
    g_log.last_close[16] = '\0';
    const int n = g_e.inv.find_key(key);
    uint8_t* ctl = n >= 0 ? controller(g_e.inv.names[n].name) : NULL;
    if (!ctl || !rdptr(ctl, kCtlMenu)) {
        return 0;
    }
    // Close by instance marks the menu closing and runs the controller's
    // begin-close (vtable +0x08, where its class has one) unless it was
    // closing already, then its staged close; a dormant menu it marks
    // pending destroy instead, with no staged close.
    uint8_t* menu = rdptr(ctl, kCtlMenu);
    const bool closing = menu[kMenuClosing] != 0;
    menu[kMenuClosing] = 1;
    const ControllerFn begin_close = reinterpret_cast<ControllerFn>(rdptr(rdptr(ctl, 0), 0x08));
    if (!closing && begin_close) {
        begin_close(ctl, NULL);
    }
    if (g_close_keeps && rd32(menu, kMenuState) == kStateDormant) {
        menu[kMenuPendingDestroy] = 1;
        return 1;
    }
    uint32_t args[5] = {rd32(ctl, kCtlMenu), 1, 0, 0, 0};
    HuFrame f;
    memset(&f, 0, sizeof(f));
    f.args = args;
    f.user = &g_e;
    hook_close_pre(&f);
    if (!g_close_keeps) {
        wr32(ctl, kCtlMenu, 0);
    }
    return 1;
}

// The update loop freeing a closed instance, later than the close.
void destroy(const char* nm) {
    wr32(controller(nm), kCtlMenu, 0);
}

// The game's own close by name, as walking away from an NPC runs it.
void game_closes(const char* nm) {
    char key[kKeyLen + 1];
    make_key(g_e, name(nm), key);
    fake_close(g_mcb, NULL, key);
}

// The close by name arealist's model calls.
void area_close_by_name(const char* key16) {
    fake_close(g_mcb, NULL, key16);
}

// The update loop sending a menu dormant under another: the staged close
// with the closing mark clear, which leaves it dormant.
void cover(uint8_t* menu) {
    menu[kMenuClosing] = 0;
    uint32_t args[5] = {reinterpret_cast<uint32_t>(menu), 0, 0, 0, 0};
    HuFrame f;
    memset(&f, 0, sizeof(f));
    f.args = args;
    f.user = &g_e;
    hook_close_pre(&f);
    wr32(menu, kMenuState, kStateDormant);
}

// The show path revealing it again.
void reveal(uint8_t* menu) {
    wr32(menu, kMenuState, 0);
    uint32_t args[1] = {reinterpret_cast<uint32_t>(menu)};
    HuFrame f;
    memset(&f, 0, sizeof(f));
    f.args = args;
    f.user = &g_e;
    hook_show_pre(&f);
}

// The mouse mode picker as the daemon presents it: the pre, then the
// original unless the pre ran instead: mode 2 while the manager has a menu
// up, else 1, as the game's reads it.
int g_picker_originals;

void hooked_mouse_mode(uint8_t* mouse) {
    HuFrame f;
    memset(&f, 0, sizeof(f));
    f.ecx = reinterpret_cast<uint32_t>(mouse);
    f.user = &g_e;
    if (hook_mouse_mode_pre(&f)) {
        return;
    }
    ++g_picker_originals;
    const uint8_t* active = rdptr(g_mcb, kMcbActive);
    const bool up = active && rd32(active, kMenuState) != kStateDormant && active[0x70] == 0;
    mouse[kMouseMode] = up ? 2 : 1;
}

// The menu input sink as the daemon presents it, called from `from`: the
// pre, then the original unless the pre ran instead, recording the menu and
// the code.
int g_sink_originals;
const uint8_t* g_sink_menu;
int g_sink_code;

uint32_t hooked_menu_input(uint8_t* menu, int code, uintptr_t from, HuFrame* seen = NULL) {
    uint32_t args[1] = {static_cast<uint32_t>(code)};
    HuFrame f;
    memset(&f, 0, sizeof(f));
    f.ecx = reinterpret_cast<uint32_t>(menu);
    f.args = args;
    f.return_address = from;
    f.result = 0xDEAD;
    f.user = &g_e;
    const int skipped = hook_menu_input_pre(&f);
    if (seen) {
        *seen = f;
    }
    if (skipped) {
        return f.result;
    }
    ++g_sink_originals;
    g_sink_menu = menu;
    g_sink_code = code;
    return 1;
}

// The routing routine: its bytes stand for its address alone. As the game's
// runs it, the code goes to the active menu's sink while a menu is up, and
// code 7 opens the main menu while none is.
uint8_t g_routing_code[0x4C];
int g_main_menu_opens;

void route(int code) {
    uint8_t* active = rdptr(g_mcb, kMcbActive);
    if (active && rd32(active, kMenuState) != kStateDormant && active[0x70] == 0) {
        hooked_menu_input(active, code, g_e.game.routing_return);
    } else if (code == 7) {
        ++g_main_menu_opens;
    }
}

// A call rel32 at `at` to `target`.
void put_call(uint8_t* at, const void* target) {
    at[0] = 0xE8;
    const int32_t rel = static_cast<int32_t>(reinterpret_cast<uintptr_t>(target)
        - (reinterpret_cast<uintptr_t>(at) + 5));
    memcpy(at + 1, &rel, 4);
}

uint8_t* open_now(const char* nm) {
    char key[kKeyLen + 1];
    make_key(g_e, name(nm), key);
    return static_cast<uint8_t*>(hooked_open(g_mcb, NULL, key, 1, 1));
}

Command cmd(uint8_t op, const char* target, int x, int y, int slot, uint32_t gen, uint32_t value = 0) {
    Command c;
    memset(&c, 0, sizeof(c));
    c.op = op;
    c.target = static_cast<int16_t>(target ? name(target) : 0);
    c.x = static_cast<int16_t>(x);
    c.y = static_cast<int16_t>(y);
    c.slot = slot;
    c.gen = gen;
    c.value = value;
    return c;
}

Command group_cmd(int group, int dx, int dy, int slot, uint32_t gen) {
    Command c = cmd(kOpGroup, NULL, dx, dy, slot, gen);
    c.target = static_cast<int16_t>(group);
    return c;
}

Command group_to_cmd(int group, int x, int y, int slot, uint32_t gen) {
    Command c = cmd(kOpGroupTo, NULL, x, y, slot, gen);
    c.target = static_cast<int16_t>(group);
    c.verb = kVerbMoveGroup;
    return c;
}

void run(const Command& c) {
    g_e.commands.push(c);
    drain(g_e);
}

int16_t ox(const char* nm) { return rd16(live(nm), kMenuOrigin); }
int16_t oy(const char* nm) { return rd16(live(nm), kMenuOrigin + 2); }

// Events since *cursor.
int events(uint32_t* cursor, uint32_t* out, int max) {
    uint32_t dropped = 0;
    return g_e.events.read(cursor, out, max, &dropped);
}

bool is_event(uint32_t e, int type, const char* nm) {
    return event_type(e) == type && event_name(e) == name(nm);
}

// A pending event: what changed, and whether it is pending now.
bool is_pending(uint32_t e, int what, bool on) {
    return event_type(e) == kEvPending && pending_what(event_name(e)) == what && pending_on(event_name(e)) == on;
}

// An error event whose record names this handle, verb, target and reason.
bool is_error(uint32_t e, int32_t slot, uint32_t gen, const char* verb, const char* target, const char* reason) {
    ErrorRecord r;
    return event_type(e) == kEvError && g_e.errors.read(static_cast<uint32_t>(event_name(e)), &r) && r.slot == slot
        && r.gen == gen && strcmp(r.verb, verb) == 0 && strcmp(r.name, target) == 0 && strstr(r.reason, reason) != NULL;
}

void build_registry() {
    memset(g_registry, 0, sizeof(g_registry));
    for (int r = 0; r < kRowCount; ++r) {
        uint8_t* p = row(r);
        const RowSpec& s = kRowSpecs[r];
        memcpy(p, s.type, 8);
        memset(p + 8, ' ', 8);
        memcpy(p + 8, s.name, strlen(s.name));
        uint8_t** slot = &g_ctl_global[r];
        memcpy(p + kRowSlot, &slot, 4);
        wr32(p, kRowPolicy, s.policy);
        p[kRowAvail] = static_cast<uint8_t>(s.avail < 0 ? 0 : s.avail);
        p[kRowLayer] = s.layer;
        wr16(p, kRowOverlap, s.overlap < 0 ? 0 : s.overlap);
    }
    memcpy(g_pristine, g_registry, sizeof(g_registry));
}

// ---------------------------------------------------------------------------
// Concurrency.

struct EventLoad {
    int producer;
    int count;
};

DWORD WINAPI event_producer(LPVOID arg) {
    const EventLoad* load = static_cast<const EventLoad*>(arg);
    for (int i = 1; i <= load->count; ++i) {
        g_e.events.post((static_cast<uint32_t>(load->producer) << 28) | static_cast<uint32_t>(i));
    }
    return 0;
}

volatile LONG g_stop;

struct CommandLoad {
    uint32_t count;
};

DWORD WINAPI command_producer(LPVOID arg) {
    const CommandLoad* load = static_cast<const CommandLoad*>(arg);
    for (uint32_t i = 0; i < load->count;) {
        Command c;
        memset(&c, 0, sizeof(c));
        c.value = i;
        c.x = static_cast<int16_t>(i);
        if (g_e.commands.push(c)) {
            ++i;
        } else {
            SwitchToThread();
        }
    }
    return 0;
}

DWORD WINAPI memory_writer(LPVOID) {
    for (int i = 0; !g_stop; ++i) {
        g_e.memory.write(5, i & 0x7FFF, i & 0x7FFF, (i & 1) ? 0 : -1, i, static_cast<uint32_t>(i));
    }
    return 0;
}

// Appends `text` as glyph codes, each character less 0x20; returns the count.
int put_text(int16_t* glyphs, int at, const char* text) {
    for (; *text; ++text) {
        glyphs[at++] = static_cast<int16_t>(*text - 0x20);
    }
    return at;
}

// `count` codes decoded, its runs as "text/escape:color|...".
const char* decoded_runs(const int16_t* glyphs, int count, GlyphText* t, const GlyphTable* table = NULL) {
    static char out[1024];
    decode_glyphs(reinterpret_cast<const uint8_t*>(glyphs), count, table, t);
    size_t used = 0;
    out[0] = '\0';
    for (int i = 0; i < t->run_count && used < sizeof(out); ++i) {
        const GlyphRun& r = t->runs[i];
        used += snprintf(out + used, sizeof(out) - used, "%s%.*s/%02X:%X", i ? "|" : "", r.length,
            t->text + r.start, r.escape, r.color);
    }
    return out;
}

int g_convert_calls;

uint32_t __cdecl counted_convert(const char* text, int16_t* glyphs, uint32_t max_glyphs, char* raw, int raw_bytes) {
    ++g_convert_calls;
    return fake_glyph_convert(text, glyphs, max_glyphs, raw, raw_bytes);
}

}  // namespace

int main() {
    // -- inventory and lookup
    check(g_e.inv.build(), "inventory indexes menu_table.inc");
    check(kRowCount == 370 && g_e.inv.count == 369,
        "370 registry rows, 369 distinct names: conf1win on two rows, the tkdebug record one of them");
    check(g_e.inv.names[name("conf1win")].row_count == 2
            && g_e.inv.names[name("conf1win")].rows[0] == 264
            && g_e.inv.names[name("conf1win")].rows[1] == 265,
        "conf1win is the one name on two rows (264, 265)");
    {
        const int d = name("dbdelsel");
        char key[kKeyLen + 1] = "";
        if (d >= 0) {
            make_key(g_e, d, key);
        }
        check(d >= 0 && strcmp(kRowSpecs[327].type, "tkdebug ") == 0 && g_e.inv.name_of_row[327] == d
                && g_e.inv.names[d].row_count == 1 && g_e.inv.names[d].rows[0] == 327
                && memcmp(g_e.inv.names[d].key, "tkdebug dbdelsel", kKeyLen) == 0
                && strcmp(key, "tkdebug dbdelsel") == 0,
            "row 327, the tkdebug record, is dbdelsel, keyed and made into an open key with its own type");
        bool every_row = true;
        for (int r = 0; r < kRowCount; ++r) {
            every_row = every_row && g_e.inv.name_of_row[r] >= 0;
        }
        check(every_row, "every registry row names an entry");
    }
    check(first_row("logwindo") == 31 && first_row("query") == 39 && first_row("targetwi") == 61,
        "names map to their expected rows");
    {
        bool round_trip = true;
        for (int n = 0; n < g_e.inv.count; ++n) {
            round_trip = round_trip && g_e.inv.find_key16(g_e.inv.names[n].key) == n
                && g_e.inv.find(g_e.inv.names[n].name) == n;
        }
        check(round_trip, "every name and every registry key finds its own entry");
    }
    check(g_e.inv.find("LOGWINDO") == name("logwindo"), "names are case-insensitive");
    check(g_e.inv.find("chat_log") == name("logwindo") && g_e.inv.find("party_list") == name("partywin")
            && g_e.inv.find("target_window") == name("targetwi"),
        "the dock-group names stand for their anchors (engine abi 1 and 2)");
    check(g_e.inv.find_window("chat_log") < 0 && g_e.inv.find_window("Party_List") < 0
            && g_e.inv.find_window("LogWindo") == name("logwindo") && g_e.inv.find_window(NULL) < 0
            && find_group_any("Chat_Log") == 0 && find_group_any("TARGET_WINDOW") == 2 && find_group_any("logwindo") < 0,
        "engine abi 3: a window by its name in any case, never by a group's; a group by its name in any case");
    check(g_e.inv.find("nosuch") < 0 && g_e.inv.find("") < 0 && g_e.inv.find("logwindoo") < 0
            && g_e.inv.find(NULL) < 0,
        "unknown, empty, overlong and null names find nothing");
    check(g_e.inv.find_key("menu    buff") == name("buff"), "a NUL-terminated short key matches");
    check(g_e.inv.find_key("menu    buff    ") == name("buff"), "a space-padded key matches");
    check(g_e.inv.find_key("MENU    Logwindo") == name("logwindo"), "keys match case-insensitively");
    check(g_e.inv.find_key("tkdebug dbdelsel") == name("dbdelsel")
            && g_e.inv.find_key("TKDEBUG DBDELSEL") == name("dbdelsel"),
        "the raw key \"tkdebug dbdelsel\" finds dbdelsel, in any case");
    check(g_e.inv.find_key("menu    dbdelsel") < 0 && g_e.inv.find_key("tkdebug buff") < 0
            && g_e.inv.find_key("buff") < 0,
        "a key matches only with its row's own type; a bare name is no key");
    check(find_group("chat_log") == 0 && find_group("party_list") == 1
            && find_group("target_window") == 2 && find_group("logwindo") < 0 && find_group(NULL) < 0,
        "group lookup");
    check(hide_only("query") && !hide_only("passinpu") && !hide_only("logwindo"),
        "query alone is hide-only");
    check(dock_reset_group(0x1000, true) == 0 && dock_reset_group(0x1000 | 0x2000, true) == 0
            && dock_reset_group(0x4000, true) == 2 && dock_reset_group(0x4000, false) == 1
            && dock_reset_group(0x2000, false) == 1 && dock_reset_group(0x10000000, true) == 3
            && dock_reset_group(0x00020002, true) == -1,
        "DockReset group per dock bit, in the show path's order; subwindo falls back to the party list");
    {
        uint32_t policy[kMaxNames];
        for (int n = 0; n < g_e.inv.count; ++n) {
            policy[n] = kRowSpecs[g_e.inv.names[n].rows[0]].policy;
        }
        uint8_t chat[kMaxNames];
        uint8_t party[kMaxNames];
        uint8_t target[kMaxNames];
        group_closure(g_e.inv, 0, policy, chat);
        group_closure(g_e.inv, 1, policy, party);
        group_closure(g_e.inv, 2, policy, target);
        int nc = 0;
        int np = 0;
        int nt = 0;
        for (int n = 0; n < g_e.inv.count; ++n) {
            nc += chat[n];
            np += party[n];
            nt += target[n];
        }
        check(nc == 51 && chat[name("logwindo")] && chat[name("ability")] && chat[name("partywin")]
                && chat[name("targetwi")] && chat[name("subwindo")]
                && np == 8 && party[name("partywin")] && party[name("targetwi")] && party[name("subwindo")]
                && !party[name("logwindo")] && !party[name("ability")]
                && nt == 2 && target[name("targetwi")] && target[name("subwindo")] && !target[name("partywin")],
            "group closure by the registry: the chat log carries its 42, partywin, its 6 and subwindo under"
            " targetwi (51 with the anchor); the party list 8 and not the log; the target window 2");
    }
    {
        char key[32];
        const Family* party = family_of("partywin");
        const bool ptw = party && family_key(*party, 3, key) && memcmp(key, "menu    ptw3    ", 16) == 0
            && key[16] == 0 && key[31] == 0 && !family_key(*party, 0, key) && !family_key(*party, 7, key);
        const Family* action = family_of("playermo");
        const bool actions = action && family_key(*action, 10, key) && memcmp(key, "menu    actiom10", 16) == 0
            && family_key(*action, 1, key) && memcmp(key, "menu    actionm1", 16) == 0;
        const Family* pmode = family_of("mp_pmode");
        const bool pmode_eight = pmode && pmode->max == 8 && !family_key(*pmode, 9, key);
        const Family* item = family_of("iteminfo");
        const bool items = item && family_key(*item, 3, key) && memcmp(key, "menu    iteminfo", 16) == 0
            && family_key(*item, 12, key) && memcmp(key, "menu    item12in", 16) == 0 && !family_key(*item, 2, key);
        const Family* itemx = family_of("itemxinf");
        const bool itemxs = itemx && family_key(*itemx, 10, key) && memcmp(key, "menu    itemx10i", 16) == 0;
        check(ptw && actions && pmode_eight && items && itemxs && !family_of("equip"),
            "resize families: template keys per row count, ranges, and none for equip");
        check(strcmp(size_holds("partywin"), "trigger") == 0 && strcmp(size_holds("playermo"), "trigger") == 0
                && strcmp(size_holds("mp_pmode"), "trigger") == 0 && strcmp(size_holds("iteminfo"), "frame") == 0
                && strcmp(size_holds("itemxinf"), "frame") == 0 && strcmp(size_holds("equip"), "reopen") == 0
                && strcmp(size_holds("logwindo"), "reopen") == 0,
            "how long a size holds, per family: partywin, playermo and mp_pmode until their owners' trigger,"
            " iteminfo and itemxinf a frame, any other window until it closes");
    }

    // -- signatures
    {
        bool parsed = true;
        const char* all[] = {kSigRegistry, kSigManager, kSigSetPosition, kSigCloseByName,
                             kSigDockMasks, kSigOpen, kSigUpdate, kSigStagedClose, kSigShow,
                             kSigPassinpuReset, kSigPartySend, kSigPartyClear,
                             kSigLink5Clear, kSigArealistClose, kSigArealistLatch, kSigPostRequest,
                             kSigPostRequestClose, kSigDockReset, kSigTemplateSwap, kSigSetFrameRect,
                             kSigLink5Cache, kSigQueryCancelAllowed};
        for (size_t i = 0; i < sizeof(all) / sizeof(all[0]); ++i) {
            uint8_t b[64];
            char m[65];
            size_t tokens = 0;
            for (const char* p = all[i]; *p; ++p) {
                if (*p != ' ' && (p == all[i] || p[-1] == ' ')) {
                    ++tokens;
                }
            }
            parsed = parsed && parse_signature(all[i], b, m, sizeof(b)) == tokens && tokens >= 5;
        }
        check(parsed && sizeof(all) / sizeof(all[0]) == 22, "all twenty-two signatures parse to one byte per token");
        bool listed = kCallCount == 11;
        for (int i = 0; listed && i < kCallCount; ++i) {
            listed = kCalls[i].signature == all[9 + i] && kCalls[i].name && kCalls[i].name[0];
        }
        uint8_t cb[64];
        char cm[65];
        const size_t clear = parse_signature(kSigPartyClear, cb, cm, sizeof(cb));
        check(listed && clear > kPartyClearFlag + 4 && cm[kPartyClearFlag] == '?' && cm[kPartyClearFlag + 3] == '?'
                && cb[0] == 0xc6 && cb[1] == 0x05,
            "the eleven called routines, in order; prtyjoin_clear's flag is its masked imm32 at +2");
        uint8_t b[8];
        char m[9];
        check(parse_signature("8b 4", b, m, sizeof(b)) == 0 && parse_signature("zz", b, m, sizeof(b)) == 0,
            "malformed signature text is refused");
        uint8_t lb[64];
        char lm[65];
        const size_t link5 = parse_signature(kSigLink5Cache, lb, lm, sizeof(lb));
        uint8_t qb[64];
        char qm[65];
        const size_t query = parse_signature(kSigQueryCancelAllowed, qb, qm, sizeof(qb));
        check(link5 >= kLink5CacheImm + 4 && lb[kLink5CacheImm - 2] == 0x81 && lb[kLink5CacheImm - 1] == 0xc2
                && strncmp(lm + kLink5CacheImm, "????", 4) == 0 && strspn(lm, "x") == kLink5CacheImm
                && query >= kQueryCancelImm + 4 && qb[0] == 0x80 && qb[1] == 0x3d
                && strncmp(qm + kQueryCancelImm, "????", 4) == 0,
            "link5's cache is the masked imm32 of add edx at +22, query's cancel-allowed byte the masked imm32 of cmp at +2");
        const char* added[] = {kSigSetCursor, kSigLink5Latch, kSigLink5Open};
        bool added_parse = true;
        for (size_t i = 0; i < sizeof(added) / sizeof(added[0]); ++i) {
            size_t tokens = 0;
            for (const char* p = added[i]; *p; ++p) {
                if (*p != ' ' && (p == added[i] || p[-1] == ' ')) {
                    ++tokens;
                }
            }
            added_parse = added_parse && parse_signature(added[i], lb, lm, sizeof(lb)) == tokens && tokens >= 5;
        }
        const size_t cursor = parse_signature(kSigSetCursor, lb, lm, sizeof(lb));
        const bool exact = cursor == 12 && strspn(lm, "x") == 12;
        const size_t latch = parse_signature(kSigLink5Latch, lb, lm, sizeof(lb));
        const bool latch_imm = latch >= kLink5LatchImm + 4 && lb[0] == 0x8a && lb[1] == 0x0d
            && strncmp(lm + kLink5LatchImm, "????", 4) == 0 && lm[kLink5LatchImm + 4] == 'x';
        const size_t open = parse_signature(kSigLink5Open, lb, lm, sizeof(lb));
        const bool open_imms = open >= kLink5OpenCallback + 4 && lb[kLink5OpenGlobal - 2] == 0x8b
            && lb[kLink5OpenGlobal - 1] == 0x0d && strncmp(lm + kLink5OpenGlobal, "????", 4) == 0
            && lb[kLink5OpenCallback - 2] == 0x50 && lb[kLink5OpenCallback - 1] == 0x68
            && strncmp(lm + kLink5OpenCallback, "????", 4) == 0;
        check(added_parse && exact && latch_imm && open_imms,
            "the three 0.4.4 signatures parse: SetCursor's 12 exact bytes; the link5 latch the masked imm32 of mov cl"
            " at +2; the open site's controller global the masked imm32 of mov ecx at +16 and its callback that of"
            " push at +22");
        const size_t convert = parse_signature(kSigGlyphConvert, lb, lm, sizeof(lb));
        check(convert == 33 && strspn(lm, "x") == 33 && lb[0] == 0x55 && lb[32] == 0xff,
            "the 0.4.6 text-to-glyph converter signature parses: 33 exact bytes");
        const size_t picker = parse_signature(kSigMouseMode, lb, lm, sizeof(lb));
        const bool picker_ok = picker == 29 && kMouseModeManagerImm == 4 && lb[3] == 0xb9
            && strncmp(lm, "xxxx????x????xxxxxxxxxxxxxxxx", 29) == 0 && lb[19] == 0x4d && lb[20] == 0x02
            && lb[26] == 0x01 && lb[28] == 0xc3;
        const size_t hit = parse_signature(kSigRowHitTest, lb, lm, sizeof(lb));
        const bool hit_ok = hit == 56 && lb[0] == 0x83 && lb[55] == 0xe8 && lm[55] == 'x'
            && sizeof(kHitTestMouseRead) == 10 && kHitTestMouseRead[kHitTestMouseByte] == kMenuMouse
            && kHitTestMouseRead[kHitTestMouseByte - 1] == 0x4e && kHitTestBytes == 0x100;
        check(picker_ok && hit_ok,
            "the 0.6.0 signatures parse: the mouse mode picker's 29 bytes, the manager the masked imm32 of mov ecx"
            " at +4, mode 2 then 1; the row hit test's 56, and its test reads menu+0x77 through esi");
        const size_t sink = parse_signature(kSigMenuInput, lb, lm, sizeof(lb));
        const bool sink_ok = sink == 26 && strspn(lm, "x") == 26 && lb[0] == 0x51 && lb[8] == 0x10
            && lb[9] == 0x0e && lb[18] == 0xc2 && lb[19] == 0x04 && lb[24] == 0x56;
        const size_t routing = parse_signature(kSigMenuRouting, lb, lm, sizeof(lb));
        const bool routing_ok = routing == kRoutingSinkCall + 1 && lb[kRoutingSinkCall] == 0xe8
            && strncmp(lm, "xxxx????xxxxxxx????xxxxxxx????xxxxxxxxxxxxx", 43) == 0
            && lb[38] == 0x8b && lb[39] == 0x4e && lb[40] == 0x54 && lb[41] == 0x50;
        check(sink_ok && routing_ok,
            "the 0.7.6 signatures parse: the sink's 26 exact bytes, its dormant test and ret 4; the routing"
            " routine's 43, its three callees masked, ending on the call to the sink at +0x2A after it loads"
            " the active menu (+0x54) and pushes the code");
    }
    {
        static const char* const prologues[kSiteCount] = {
            "8b 44 24 04 83 ec 20", "83 ec 14 53 55 56 57",
            "53 56 8b 74 24 0c 33 db 57", "53 55 56 8b 74 24 10 32 db 57",
            "56 8b f1 b9 ?? ?? ?? ??", "51 55 56 8b f1 57"};
        static const uint32_t args[kSiteCount] = {12, 0, 20, 4, 0, 4};
        bool ok = true;
        for (int i = 0; i < kSiteCount; ++i) {
            uint8_t sb[64];
            char sm[65];
            uint8_t pb[16];
            char pm[17];
            parse_signature(kSites[i].signature, sb, sm, sizeof(sb));
            const size_t pl = parse_signature(prologues[i], pb, pm, sizeof(pb));
            ok = ok && pl == kSites[i].prologue && memcmp(sb, pb, pl) == 0
                && kSites[i].arg_bytes == args[i] && kSites[i].pre != NULL;
            const uint32_t imm = kSites[i].manager_imm;
            for (size_t k = 0; k < pl; ++k) {
                const bool in_imm = imm && k >= imm && k < imm + 4;
                ok = ok && sm[k] == (in_imm ? '?' : 'x');
            }
            ok = ok && (imm == 0 || imm + 4 <= pl);
        }
        check(ok && kSites[kSiteMouseMode].manager_imm == 4 && kSites[kSiteMouseMode].post == NULL
                && kSites[kSiteMenuInput].manager_imm == 0 && kSites[kSiteMenuInput].post == NULL,
            "hooked prologues: open 7/12, ui_update 7/0, staged_close 9/20, show_path 10/4, menu_input 6/4, exact"
            " bytes; mouse_mode 8/0, exact but for the manager's imm32 at +4, which the engine fills in");
    }
    {
        static uint8_t text[8192];
        uint8_t sig[64];
        char mask[65];
        const size_t len = parse_signature(kSigOpen, sig, mask, sizeof(sig));
        memcpy(text + 100, sig, len);
        text[100 + 21] = 0x12;      // a wildcard byte differs
        ScanResult r;
        scan(text, sizeof(text), sig, mask, false, &r);
        check(r.count == 1 && r.hits[0] == text + 100 && !r.jumped[0], "scan: one match, wildcards ignored");
        memcpy(text + 3000, sig, len);
        scan(text, sizeof(text), sig, mask, false, &r);
        check(r.count == 2, "scan: a second copy is counted, not ignored");
        text[3000] = 0xE9;
        text[3001] = 0x11;
        text[3002] = 0x22;
        text[3003] = 0x33;
        text[3004] = 0x44;
        scan(text, sizeof(text), sig, mask, false, &r);
        const bool plain = r.count == 1;
        scan(text, sizeof(text), sig, mask, true, &r);
        check(plain && r.count == 2 && !r.jumped[0] && r.jumped[1] && r.hits[1] == text + 3000,
            "scan: a jump-patched copy is a candidate only when asked for");
        text[3010] ^= 0xFF;
        scan(text, sizeof(text), sig, mask, true, &r);
        check(r.count == 1, "scan: a jump with the wrong body is not a candidate");
    }

    // -- registry verification
    build_registry();
    {
        char why[256] = "";
        check(verify_registry(g_registry, kRowCount + 1, why, sizeof(why)), "the expected table verifies");
        row(31)[kRowLayer] = kHiddenLayer;
        const bool layer = !verify_registry(g_registry, kRowCount + 1, why, sizeof(why))
            && strstr(why, "row 31 (logwindo): draw layer") != NULL;
        row(31)[kRowLayer] = 1;
        check(layer, "a hidden row fails verification and names itself");
        wr32(row(61), kRowPolicy, 0x02000401);
        const bool policy = !verify_registry(g_registry, kRowCount + 1, why, sizeof(why))
            && strstr(why, "targetwi") && strstr(why, "policy");
        wr32(row(61), kRowPolicy, 0x02002401);
        check(policy, "a cleared dock bit fails verification");
        row(kRowCount)[0] = 'm';
        const bool terminator = !verify_registry(g_registry, kRowCount + 1, why, sizeof(why));
        row(kRowCount)[0] = 0;
        check(terminator, "a 371st record fails verification");
        check(!verify_registry(g_registry, kRowCount, why, sizeof(why)), "a short table fails verification");
        check(verify_registry(g_registry, kRowCount + 1, why, sizeof(why)), "restored table verifies again");
    }

    // -- handles and holds
    HandleTable handles;
    memset(&handles, 0, sizeof(handles));
    {
        int slots[10];
        for (int i = 0; i < 10; ++i) {
            slots[i] = handles.claim("h", 0);
        }
        check(handles.open == 10 && handles.capacity >= 10 && slots[9] == 9, "the handle table grows");
        const uint32_t old = handles.slots[3].generation;
        handles.free_slot(3);
        check(handles.get(3, old) == NULL && handles.open == 9, "a released handle is gone");
        const int again = handles.claim("h2", 0);
        check(again == 3 && handles.slots[3].generation == old + 1 && handles.get(3, old) == NULL
                && handles.get(3, old + 1) != NULL,
            "a reused slot gets a new generation; the old reference stays dead");
        for (int i = 0; i < 10; ++i) {
            handles.free_slot(i);
        }
        check(handles.open == 0, "every handle released");
    }
    const int s1 = handles.claim("addon_one", 0);
    const int s2 = handles.claim("addon_two", 0);
    Handle& h1 = handles.slots[s1];
    Handle& h2 = handles.slots[s2];
    {
        const int n = name("logwindo");
        g_e.holds.set(h1, n, kHoldHide, true);
        g_e.holds.set(h1, n, kHoldHide, true);
        g_e.holds.set(h2, n, kHoldHide, true);
        const bool both = g_e.holds.want[n] == kWantHidden && g_e.holds.hide_count[n] == 2;
        g_e.holds.set(h1, n, kHoldHide, false);
        const bool one = g_e.holds.want[n] == kWantHidden;
        g_e.holds.set(h2, n, kHoldHide, false);
        check(both && one && g_e.holds.want[n] == 0 && g_e.holds.hide_count[n] == 0,
            "hides are a union: hidden while anyone holds it, a repeat counts once");
        const int e = name("equip");
        g_e.holds.set(h1, e, kHoldBlock, true);
        const bool blocked = g_e.holds.want[e] == (kWantHidden | kWantBlocked);
        g_e.holds.set(h2, e, kHoldHide, true);
        g_e.holds.set(h1, e, kHoldBlock, false);
        const bool still_hidden = g_e.holds.want[e] == kWantHidden;
        g_e.holds.set(h2, e, kHoldHide, false);
        check(blocked && still_hidden && g_e.holds.want[e] == 0,
            "block implies hide; unblocking leaves another handle's hide");
        g_e.holds.set(h1, name("buff"), kHoldHide, true);
        g_e.holds.set(h1, name("menuwind"), kHoldBlock, true);
        g_e.holds.set(h2, name("buff"), kHoldHide, true);
        g_e.holds.release(h1, g_e.inv.count);
        check(g_e.holds.want[name("buff")] == kWantHidden && g_e.holds.want[name("menuwind")] == 0,
            "releasing a handle drops only its holds");
        g_e.holds.release(h2, g_e.inv.count);
        check(g_e.holds.want[name("buff")] == 0, "the last holder's release clears the union");
    }

    // -- the open stack
    {
        OpenStack& s = g_e.opens;
        const void* outer = reinterpret_cast<const void*>(0x2000);
        const void* inner = reinterpret_cast<const void*>(0x1F00);
        s.push(outer, 7, 10);
        s.push(inner, 7, 11);
        const bool nested = s.depth == 2 && s.contains(7, 10) && s.contains(7, 11) && !s.contains(8, 10);
        int16_t got = -1;
        const bool inner_pop = s.pop(inner, 7, &got) && got == 11;
        const bool outer_pop = s.pop(outer, 7, &got) && got == 10 && s.depth == 0;
        check(nested && inner_pop && outer_pop, "open stack: nested pre/post pairs");
        s.push(outer, 7, 10);
        s.push(outer, 7, 12);
        const bool replaced = s.depth == 1 && s.contains(7, 12) && !s.contains(7, 10);
        s.push(inner, 7, 13);
        s.pop(outer, 7, &got);
        check(replaced && s.depth == 0 && got == 12,
            "open stack: a frame whose post never ran is dropped, deeper ones with it");
    }

    // -- the fabricated game
    g_e.game.registry = g_registry;
    for (int r = 0; r < kRowCount; ++r) {
        g_e.game.slot[r] = &g_ctl_global[r];
    }
    g_e.game.mcb = g_mcb;
    g_e.game.menu_routing = g_routing_code;
    g_e.game.routing_return = reinterpret_cast<uintptr_t>(g_routing_code + kRoutingSinkCall + 5);
    g_e.game.set_position = &fake_set_position;
    g_e.game.open = &hooked_open;
    g_e.game.close = &fake_close;
    g_e.game.dock_reset = &fake_dock_reset;
    g_e.game.template_swap = &fake_template_swap;
    g_e.game.set_frame_rect = &fake_set_frame_rect;
    g_e.game.set_cursor = &fake_set_cursor;
    g_e.game.passinpu_reset = &fake_passinpu_reset;
    g_e.game.party_send = &fake_party_send;
    g_e.game.party_clear = &fake_party_clear;
    g_e.game.party_pending = &g_log.party_pending;
    g_e.game.link5_clear = &fake_link5_clear;
    g_e.game.link5_cache = g_link5_cache;
    g_e.game.link5_latch = &g_link5_latch;
    g_e.game.link5_callback = reinterpret_cast<const void*>(&fake_link5_callback);
    g_e.game.query_cancel_allowed = &g_query_cancel_allowed;
    g_e.game.arealist_close = &fake_arealist_close;
    g_e.game.arealist_latch = &fake_arealist_latch;
    g_e.game.post_request = &fake_post_request;
    g_e.game.post_request_close = &fake_post_request_close;
    vt_query[5] = g_nop_code;
    vt_query[kCtlInput / 4] = reinterpret_cast<const void*>(&fake_query_input);
    vt_box[5] = g_nop_code;
    vt_box[kCtlInput / 4] = reinterpret_cast<const void*>(&fake_box_input);
    vt_area[5] = g_nop_code;
    fake_area::fill_vtable(vt_area);
    fake_area::install(&area_close_by_name, &fake_area::select_index);
    vt_self[5] = g_nop_code;
    vt_layout[5] = g_ret_code;
    vt_layout[1] = g_open_tail;
    vt_layout[0x44 / 4] = reinterpret_cast<const void*>(&fake_layout);
    vt_fixed[5] = g_ret_code;
    vt_fixed[1] = g_open_fixed;
    vt_operands[5] = g_ret_code;
    vt_operands[1] = g_open_operands;
    vt_ends[5] = g_ret_code;
    vt_ends[1] = g_open_ends;
    const void** const tails[2] = {vt_operand_tail, vt_pop_tail};
    for (int i = 0; i < 2; ++i) {
        tails[i][5] = g_ret_code;
        tails[i][0x10 / 4] = g_ret_code;
        tails[i][0x44 / 4] = reinterpret_cast<const void*>(&fake_layout);
    }
    vt_operand_tail[1] = g_open_operand_tail;
    vt_pop_tail[1] = g_open_pop_tail;
    {
        g_open_fixed[0] = 0xE8;
        const int32_t rel = static_cast<int32_t>(reinterpret_cast<uintptr_t>(&fake_set_position)
            - (reinterpret_cast<uintptr_t>(g_open_fixed) + 5));
        memcpy(g_open_fixed + 1, &rel, 4);
        g_open_fixed[5] = 0xC3;
    }
    const char* self_classes[] = {"logwindo", "ability", "partywin", "targetwi", "subwindo",
                                  "persona", "buff", "passinpu", "prtyjoin", "link5",
                                  "scsibori", "delivery", "post1", "post2", "playermo"};
    for (size_t i = 0; i < sizeof(self_classes) / sizeof(self_classes[0]); ++i) {
        give_controller(self_classes[i], vt_self);
    }
    give_controller("query", vt_query);
    give_controller("arealist", vt_area);
    give_controller("equip", vt_layout);
    give_controller("logwin2", vt_layout);
    give_controller("conf1win", vt_self, 1);
    give_controller("dbdelsel", vt_self);
    uint8_t* fixed_ctl = g_ctl[g_ctl_used++].bytes;
    {
        const void* vtp = vt_fixed;
        memcpy(fixed_ctl, &vtp, 4);
    }

    const void* set_position = reinterpret_cast<const void*>(&fake_set_position);
    check(refreshes_itself(controller("logwindo")) && !refreshes_itself(controller("equip")),
        "a class refreshes itself unless vtable+0x14 is a bare ret");
    check(relayout_is_safe(controller("equip"), set_position) && !relayout_is_safe(fixed_ctl, set_position),
        "an open hook that calls SetPosition makes the layout unsafe to drive");
    {
        const LONG before = g_layout_calls;
        const bool refused = !run_layout(fixed_ctl, set_position);
        wr16(controller("equip"), 0x1A, 1);
        const bool ran = run_layout(controller("equip"), set_position);
        check(refused && ran && g_layout_calls == before + 1 && g_guard_seen == 0
                && rd16(controller("equip"), 0x1A) == 1,
            "run_layout clears the class's guard and runs its layout, once");
    }
    // the scans read the forms the compiler writes, never an operand byte
    {
        const void* const tables[4] = {vt_operands, vt_operand_tail, vt_pop_tail, vt_ends};
        uint8_t* ctl[4];
        for (int i = 0; i < 4; ++i) {
            ctl[i] = g_ctl[g_ctl_used++].bytes;
            memcpy(ctl[i], &tables[i], 4);
        }
        put_call(g_open_operands + 11, reinterpret_cast<const void*>(&fake_set_position));
        g_open_operands[16] = 0x5E;
        g_open_operands[17] = 0xC3;
        put_call(g_open_ends + 2, reinterpret_cast<const void*>(&fake_set_position));
        check(!relayout_is_safe(ctl[0], set_position) && relayout_is_safe(ctl[3], set_position),
            "a C3 inside an imm32 and an FF 60 inside a displacement are read past: the SetPosition call after them"
            " makes the layout unsafe; a ret after pop esi ends the routine, so a call past it is another's");
        const LONG before = g_layout_calls;
        wr16(ctl[1], 0x1A, 1);
        wr16(ctl[2], 0x1A, 1);
        const bool ran = run_layout(ctl[1], set_position) && run_layout(ctl[2], set_position);
        check(ran && g_layout_calls == before + 2 && rd16(ctl[1], 0x1A) == 1 && rd16(ctl[2], 0x1A) == 1,
            "past the same operands, the tail jump names the layout at vt+0x44, never the displacement's +0x10,"
            " after mov eax,[ecx] and after mov ecx,edx; pop edi");
    }

    uint32_t cursor = g_e.events.position();
    uint32_t ev[64];
    const uint32_t gen1 = h1.generation;
    const uint32_t gen2 = h2.generation;

    // hides reach the registry and the live menu in the drain
    {
        uint8_t* log = open_now("logwindo");
        const int n = name("logwindo");
        g_e.holds.set(h1, n, kHoldHide, true);
        g_e.holds.set(h2, n, kHoldHide, true);
        const bool untouched = row(31)[kRowLayer] == 1;
        drain(g_e);
        const bool hidden = row(31)[kRowLayer] == kHiddenLayer && log[kMenuLayer] == kHiddenLayer;
        g_e.holds.set(h1, n, kHoldHide, false);
        drain(g_e);
        const bool held = row(31)[kRowLayer] == kHiddenLayer;
        g_e.holds.set(h2, n, kHoldHide, false);
        drain(g_e);
        check(untouched && hidden && held && row(31)[kRowLayer] == 1 && log[kMenuLayer] == 1,
            "hide: nothing changes until the drain; registry and live layer follow the union");
        uint8_t* conf = open_now("conf1win");
        g_e.holds.set(h1, name("conf1win"), kHoldHide, true);
        drain(g_e);
        const bool both_rows = row(264)[kRowLayer] == kHiddenLayer && row(265)[kRowLayer] == kHiddenLayer
            && conf[kMenuLayer] == kHiddenLayer;
        g_e.holds.set(h1, name("conf1win"), kHoldHide, false);
        drain(g_e);
        check(both_rows && row(264)[kRowLayer] == 2 && conf[kMenuLayer] == 2,
            "a name on two rows is hidden on both, and its live menu found through either");
    }

    // the mouse kept off a hidden window: the hit test's own test of +0x77,
    // and the mode picker held in world mode while a hidden window is active
    {
        const int b = name("buff");
        uint8_t* buff_row = row(g_e.inv.names[b].rows[0]);
        const uint8_t layer = kRowSpecs[g_e.inv.names[b].rows[0]].layer;
        uint8_t* first = open_now("buff");
        const bool fresh = first && first[kMenuMouse] == 1;
        g_e.holds.set(h1, b, kHoldHide, true);
        drain(g_e);
        const bool hidden = first && buff_row[kRowLayer] == kHiddenLayer && first[kMenuLayer] == kHiddenLayer
            && first[kMenuMouse] == 0;
        check(fresh && hidden, "hide: the live menu's +0x77 goes from the constructor's 1 to 0, its draw layer and the"
            " registry row's to 0x7F");
        run(cmd(kOpClose, "buff", 0, 0, s1, gen1));
        const bool closed = !live("buff");
        uint8_t* second = open_now("buff");
        check(closed && second && second != first && second[kMenuLayer] == kHiddenLayer && second[kMenuMouse] == 0,
            "a hidden window opened again as a new instance: the open's post writes 0 over the constructor's 1");
        uint8_t mouse[0x60];
        memset(mouse, 0, sizeof(mouse));
        wr32(g_mcb, kMcbActive, reinterpret_cast<uint32_t>(second));
        mouse[kMouseMode] = 2;
        const int originals = g_picker_originals;
        hooked_mouse_mode(mouse);
        check(mouse[kMouseMode] == kMouseWorld && g_picker_originals == originals,
            "mouse mode picker with the hidden window active: the pre writes mode 1 and the original does not run");
        g_e.holds.set(h1, b, kHoldHide, false);
        drain(g_e);
        check(buff_row[kRowLayer] == layer && second[kMenuLayer] == layer && second[kMenuMouse] == 1,
            "unhide: +0x77 back to 1, the draw layer and the row back to the expected layer");
        hooked_mouse_mode(mouse);
        const bool menu_mode = mouse[kMouseMode] == 2 && g_picker_originals == originals + 1;
        wr32(g_mcb, kMcbActive, 0);
        hooked_mouse_mode(mouse);
        check(menu_mode && mouse[kMouseMode] == 1 && g_picker_originals == originals + 2,
            "the picker's original runs with the window visible (mode 2) and with no active menu (mode 1)");
        second[kMenuLayer] = kHiddenLayer;
        wr32(g_mcb, kMcbActive, reinterpret_cast<uint32_t>(second));
        mouse[kMouseMode] = 2;
        HuFrame f;
        memset(&f, 0, sizeof(f));
        f.ecx = reinterpret_cast<uint32_t>(mouse);
        f.user = &g_e;
        const int skipped = hook_mouse_mode_pre(&f);
        check(skipped == 1 && mouse[kMouseMode] == kMouseWorld && f.result == 0,
            "the pre reads the active menu's live layer alone, and leaves the result word alone");
        second[kMenuLayer] = layer;
        wr32(g_mcb, kMcbActive, 0);
        run(cmd(kOpClose, "buff", 0, 0, s1, gen1));
        events(&cursor, ev, 64);
    }

    // keys and the gamepad kept off a hidden window: the routing routine's
    // call to the active menu's sink, and only that call, dropped
    {
        const int b = name("buff");
        const uint8_t layer = kRowSpecs[g_e.inv.names[b].rows[0]].layer;
        uint8_t* menu = open_now("buff");
        wr32(g_mcb, kMcbActive, reinterpret_cast<uint32_t>(menu));
        const int originals = g_sink_originals;
        const int opens = g_main_menu_opens;
        route(5);
        check(menu && menu[kMenuLayer] == layer && g_sink_originals == originals + 1 && g_sink_menu == menu
                && g_sink_code == 5,
            "a key on the visible active window: the routing routine's code reaches its sink");
        g_e.holds.set(h1, b, kHoldHide, true);
        drain(g_e);
        const bool hidden = menu[kMenuLayer] == kHiddenLayer;
        for (int code = 1; code <= 0x20; ++code) {
            route(code);
        }
        check(hidden && g_sink_originals == originals + 1 && g_main_menu_opens == opens,
            "hidden and active: no code from the routing routine, 1 to 0x20, reaches its sink, and code 7 opens no"
            " main menu");
        HuFrame f;
        const uint32_t answer = hooked_menu_input(menu, 6, g_e.game.routing_return, &f);
        check(answer == 0 && f.result == 0 && g_sink_originals == originals + 1,
            "the pre answers 0 for the sink over whatever the result word held, so the original does not run");
        hooked_menu_input(menu, 9, reinterpret_cast<uintptr_t>(&fake_set_cursor) + 0x20);
        const bool from_cursor = g_sink_originals == originals + 2 && g_sink_code == 9 && g_sink_menu == menu;
        hooked_menu_input(menu, 0x16, g_e.game.routing_return + 0x100);
        check(from_cursor && g_sink_originals == originals + 3 && g_sink_code == 0x16,
            "SetCursor's code 9 and the wheel's 0x16, from other callers, still reach the hidden window's sink");
        uint8_t* other = open_now("equip");
        wr32(g_mcb, kMcbActive, reinterpret_cast<uint32_t>(other));
        route(5);
        const bool visible_other = g_sink_originals == originals + 4 && g_sink_menu == other;
        hooked_menu_input(menu, 5, g_e.game.routing_return);
        check(visible_other && g_sink_originals == originals + 5 && g_sink_menu == menu,
            "with another window active, its keys reach it; the pre drops only a call to the active menu");
        wr32(g_mcb, kMcbActive, 0);
        route(5);
        route(7);
        const bool none = g_sink_originals == originals + 5 && g_main_menu_opens == opens + 1;
        hooked_menu_input(menu, 5, g_e.game.routing_return);
        check(none && g_sink_originals == originals + 6,
            "with no active menu the routing routine's own branch runs (code 7 opens the main menu), and the pre"
            " passes a call to the hidden window's sink");
        g_e.holds.set(h1, b, kHoldHide, false);
        drain(g_e);
        wr32(g_mcb, kMcbActive, reinterpret_cast<uint32_t>(menu));
        route(6);
        check(menu[kMenuLayer] == layer && g_sink_originals == originals + 7 && g_sink_menu == menu
                && g_sink_code == 6,
            "unhide: the routing routine's codes reach the window's sink again");
        wr32(g_mcb, kMcbActive, 0);
        run(cmd(kOpClose, "buff", 0, 0, s1, gen1));
        run(cmd(kOpClose, "equip", 0, 0, s1, gen1));
        events(&cursor, ev, 64);
    }

    // the unhide gives +0x77 back as the hide found it on the live instance;
    // an instance the game made while the window was hidden gets the
    // constructor's 1
    {
        const int b = name("buff");
        uint8_t* menu = open_now("buff");
        bool restored = true;
        const uint8_t owns[2] = {2, 0};
        for (int i = 0; i < 2; ++i) {
            const uint8_t own = owns[i];
            menu[kMenuMouse] = own;
            g_e.holds.set(h1, b, kHoldHide, true);
            drain(g_e);
            const bool hidden = menu[kMenuMouse] == 0;
            g_e.holds.set(h1, b, kHoldHide, false);
            drain(g_e);
            restored = restored && hidden && menu[kMenuMouse] == own;
        }
        check(restored, "unhide: the live instance's +0x77 back to what the hide found, 2 and 0 alike, not 1");
        g_e.holds.set(h1, b, kHoldHide, true);
        drain(g_e);
        run(cmd(kOpClose, "buff", 0, 0, s1, gen1));
        uint8_t* again = open_now("buff");
        const bool zeroed = again && again != menu && again[kMenuMouse] == 0;
        g_e.holds.set(h1, b, kHoldHide, false);
        drain(g_e);
        check(zeroed && again[kMenuMouse] == 1 && menu[kMenuMouse] == 0,
            "a hidden window opened again as a new instance: its +0x77 back to the constructor's 1 at the unhide,"
            " whatever the first instance had");
        run(cmd(kOpClose, "buff", 0, 0, s1, gen1));
        events(&cursor, ev, 64);
    }

    // the tkdebug record opens by its own key and hides like any window
    {
        const int d = name("dbdelsel");
        const LONG unmatched = g_e.unmatched_keys;
        const LONG opens = g_e.instance[d];
        events(&cursor, ev, 64);
        run(cmd(kOpOpen, "dbdelsel", 0, 0, s1, gen1));
        const int n = events(&cursor, ev, 64);
        uint8_t* menu = live("dbdelsel");
        check(menu && strcmp(g_open_key, "tkdebug dbdelsel") == 0 && g_e.unmatched_keys == unmatched
                && g_e.instance[d] == opens + 1 && n == 1 && is_event(ev[0], kEvOpened, "dbdelsel"),
            "open dbdelsel: the open routine gets \"tkdebug dbdelsel\", the hook maps it back; opened{dbdelsel}");
        g_e.holds.set(h1, d, kHoldHide, true);
        drain(g_e);
        const bool hidden = menu && row(327)[kRowLayer] == kHiddenLayer && menu[kMenuLayer] == kHiddenLayer;
        g_e.holds.set(h1, d, kHoldHide, false);
        drain(g_e);
        check(hidden && row(327)[kRowLayer] == 2 && menu[kMenuLayer] == 2
                && memcmp(row(327), g_pristine + 327 * kRowStride, kRowStride) == 0,
            "hide dbdelsel: 0x7F on row 327's draw layer and the live menu's; unhide puts back 2, the row as expected");
        run(cmd(kOpClose, "dbdelsel", 0, 0, s1, gen1));
        check(!live("dbdelsel") && strcmp(g_log.last_close, "tkdebug dbdelsel") == 0,
            "close dbdelsel: the close routine gets \"tkdebug dbdelsel\"");
        events(&cursor, ev, 64);
    }

    // block refuses the open with the game's own 0 and says so
    {
        events(&cursor, ev, 64);
        g_e.holds.set(h1, name("equip"), kHoldBlock, true);
        drain(g_e);
        const int opens = g_open_calls;
        void* refused = open_now("equip");
        const int n = events(&cursor, ev, 64);
        check(refused == NULL && g_open_calls == opens && n == 1 && is_event(ev[0], kEvBlocked, "equip"),
            "block: the open returns 0 without running the original, blocked{equip} posted");
        g_e.holds.set(h1, name("equip"), kHoldBlock, false);
        drain(g_e);
        uint8_t* menu = open_now("equip");
        const int m = events(&cursor, ev, 64);
        check(menu != NULL && m == 1 && is_event(ev[0], kEvOpened, "equip")
                && row(41)[kRowLayer] == 2 && menu[kMenuLayer] == 2,
            "unblock: opens visible; one opened{equip}, the show nested in the open not reported as uncovered");
    }

    // move: SetPosition, the class's layout, memory, home
    {
        const LONG layouts = g_layout_calls;
        run(cmd(kOpMove, "equip", 300, 200, s1, gen1));
        check(ox("equip") == 300 && oy("equip") == 200 && g_layout_calls == layouts + 1,
            "move equip: SetPosition then its layout, which it does not refresh itself");
        const MemEntry& m = g_e.memory.e[name("equip")];
        check(m.active && m.x == 300 && m.y == 200 && !m.keep_dock && m.group == 0
                && g_e.memory.owned_by(name("equip"), s1, gen1),
            "move: remembered for the session, owned by the mover, by no group");
        run(cmd(kOpReset, "equip", 0, 0, s2, gen2));
        check(ox("equip") == 130 && oy("equip") == 48 && !g_e.memory.e[name("equip")].active,
            "reset equip: back to its default rect, forgotten");
    }

    // a class whose open hook places it resets to that placement
    {
        open_now("persona");
        const bool placed = ox("persona") == 16 && oy("persona") == 48;
        run(cmd(kOpMove, "persona", 200, 150, s1, gen1));
        run(cmd(kOpMove, "persona", 220, 170, s1, gen1));
        const bool moved = ox("persona") == 220 && oy("persona") == 170;
        run(cmd(kOpReset, "persona", 0, 0, s1, gen1));
        check(placed && moved && ox("persona") == 16 && oy("persona") == 48,
            "reset persona: back where its open hook put it (16,48), not the default rect");
        const int calls = g_setpos_calls;
        run(cmd(kOpReset, "persona", 0, 0, s1, gen1));
        check(g_setpos_calls == calls, "reset of a window the engine did not move leaves it alone");
    }

    // move undocks, reset re-docks through DockReset
    {
        open_now("ability");
        clear_log();
        run(cmd(kOpMove, "ability", 400, 100, s1, gen1));
        const bool undocked = !(rd32(live("ability"), kMenuPolicy) & 0x1000)
            && !(rd32(row(257), kRowPolicy) & 0x1000) && ox("ability") == 400 && oy("ability") == 100;
        const bool no_redock = g_log.dock_resets == 0;
        run(cmd(kOpReset, "ability", 0, 0, s1, gen1));
        const bool redocked = (rd32(live("ability"), kMenuPolicy) & 0x1000)
            && rd32(row(257), kRowPolicy) == kRowSpecs[257].policy;
        check(undocked && no_redock, "move ability: dock bit cleared in the live menu and the registry, no re-dock");
        check(redocked && ox("ability") == 10 && oy("ability") == 200 && g_log.dock_resets == 1
                && g_log.dock_group == 0 && g_log.dock_menu == live("ability"),
            "reset ability: dock bit restored, default rect, then DockReset(0, ability)");
        run(cmd(kOpMove, "equip", 300, 200, s1, gen1));
        clear_log();
        run(cmd(kOpReset, "equip", 0, 0, s1, gen1));
        check(ox("equip") == 130 && g_log.dock_resets == 0, "reset of a window that does not dock calls no DockReset");
    }

    // a remembered position goes back on at every open, not at a reveal
    {
        run(cmd(kOpClose, "equip", 0, 0, s1, gen1));
        events(&cursor, ev, 64);
        const int calls = g_setpos_calls;
        run(cmd(kOpMove, "equip", 250, 60, s1, gen1));
        const bool remembered = g_setpos_calls == calls && g_e.memory.e[name("equip")].active
            && !(rd32(row(41), kRowPolicy) & kDockMask);
        run(cmd(kOpOpen, "equip", 0, 0, s1, gen1));
        const int n = events(&cursor, ev, 64);
        check(remembered && live("equip") && ox("equip") == 250 && oy("equip") == 60
                && n == 1 && is_event(ev[0], kEvOpened, "equip"),
            "move while closed is remembered and applied when it opens; opened{equip}");
        uint32_t args[1] = {reinterpret_cast<uint32_t>(live("equip"))};
        HuFrame f;
        memset(&f, 0, sizeof(f));
        f.args = args;
        f.user = &g_e;
        wr16(live("equip"), kMenuOrigin, 1);
        hook_show_pre(&f);
        const int r = events(&cursor, ev, 64);
        check(r == 1 && is_event(ev[0], kEvUncovered, "equip") && ox("equip") == 1,
            "a reveal posts uncovered{equip} and does not re-apply the position");
        wr16(live("equip"), kMenuOrigin, 250);
        run(cmd(kOpClose, "equip", 0, 0, s1, gen1));
        const int c = events(&cursor, ev, 64);
        check(c == 1 && is_event(ev[0], kEvClosed, "equip") && !live("equip"),
            "close: close by instance marks the menu closing, and its staged close posts closed{equip}");
    }

    // covered: the staged close with the closing mark clear; the game's close
    // of a dormant menu runs no staged close, and the drain notices it
    {
        uint8_t* eq = open_now("equip");
        events(&cursor, ev, 64);
        cover(eq);
        int n = events(&cursor, ev, 64);
        const bool covered = n == 1 && is_event(ev[0], kEvCovered, "equip") && g_e.covered[name("equip")];
        reveal(eq);
        n = events(&cursor, ev, 64);
        const bool uncovered = n == 1 && is_event(ev[0], kEvUncovered, "equip") && !g_e.covered[name("equip")];
        check(covered && uncovered,
            "a menu sent dormant under another posts covered{equip}; the show path bringing it back, uncovered{equip}");
        cover(eq);
        events(&cursor, ev, 64);
        cover(eq);
        check(events(&cursor, ev, 64) == 0 && g_e.covered[name("equip")],
            "the staged close of a menu already dormant changes nothing and posts nothing");
        drain(g_e);
        const bool quiet = events(&cursor, ev, 64) == 0;
        wr32(controller("equip"), kCtlMenu, 0);
        drain(g_e);
        n = events(&cursor, ev, 64);
        check(quiet && n == 1 && is_event(ev[0], kEvClosed, "equip") && !g_e.covered[name("equip")],
            "a covered window the game destroys without a staged close: the next drain posts closed{equip}, once");
        drain(g_e);
        check(events(&cursor, ev, 64) == 0, "and nothing more after it");
    }

    // group moves keep docking and carry everything docked to the group
    {
        run(cmd(kOpReset, "equip", 0, 0, s1, gen1));
        const int lx = ox("logwindo");
        const int ly = oy("logwindo");
        const int ax = ox("ability");
        const int ay = oy("ability");
        run(group_cmd(0, 5, -10, s2, gen2));
        check(ox("logwindo") == lx + 5 && oy("logwindo") == ly - 10 && ox("ability") == ax + 5
                && oy("ability") == ay - 10 && (rd32(live("ability"), kMenuPolicy) & 0x1000)
                && g_e.memory.e[name("ability")].keep_dock && g_e.memory.owned_by(name("logwindo"), s2, gen2),
            "move_group chat_log: anchor and docked window shifted, still docked, owned by the mover");
        open_now("partywin");
        open_now("targetwi");
        open_now("subwindo");
        const int px = ox("partywin");
        const int tx = ox("targetwi");
        const int sx = ox("subwindo");
        run(group_cmd(1, 20, 0, s2, gen2));
        check(ox("partywin") == px + 20 && ox("targetwi") == tx + 20 && ox("subwindo") == sx + 20
                && ox("logwindo") == lx + 5,
            "move_group party_list: partywin, targetwi and subwindo (docked to targetwi) move; the chat log does not");
        run(cmd(kOpClose, "partywin", 0, 0, s2, gen2));
        const LONG errors = g_e.drain_errors;
        run(group_cmd(1, 1, 1, s2, gen2));
        check(g_e.drain_errors == errors + 1 && strstr(g_e.error, "partywin is not open") != NULL,
            "move_group with its anchor closed records a drain error");

        Command failing = group_cmd(1, 1, 1, s2, gen2);
        failing.verb = kVerbMoveGroup;
        events(&cursor, ev, 64);
        run(failing);
        const int n = events(&cursor, ev, 64);
        check(n == 1 && is_error(ev[0], s2, gen2, "move_group", "party_list", "its anchor partywin is not open")
                && strcmp(g_e.error, "move_group party_list: its anchor partywin is not open") == 0,
            "a command the game thread refuses posts an error event: its record names the handle that queued it, the"
            " verb, the group and the reason; status keeps it as one line");
        drain_error(g_e, "outside any command");
        check(events(&cursor, ev, 64) == 0 && strcmp(g_e.error, "outside any command") == 0,
            "an error outside a queued command posts no event: no handle asked for it");
    }

    // move_group to x, y: the anchor's origin goes there, and every open
    // window it carries by as much, still docked
    {
        const int lx = ox("logwindo");
        const int ly = oy("logwindo");
        const int ax = ox("ability");
        const int ay = oy("ability");
        run(group_to_cmd(0, lx + 40, ly - 30, s2, gen2));
        const MemEntry& lm = g_e.memory.e[name("logwindo")];
        const MemEntry& am = g_e.memory.e[name("ability")];
        check(ox("logwindo") == lx + 40 && oy("logwindo") == ly - 30 && ox("ability") == ax + 40
                && oy("ability") == ay - 30 && (rd32(live("ability"), kMenuPolicy) & 0x1000)
                && lm.group == 1 && am.group == 1 && lm.x == lx + 40 && lm.y == ly - 30
                && g_e.memory.owned_by(name("logwindo"), s2, gen2) && g_e.memory.owned_by(name("ability"), s2, gen2),
            "move_group chat_log to x,y: logwindo's origin to x,y, ability carried by the same 40,-30, docked; both"
            " remembered as the chat_log group's move by the mover");
        run(group_to_cmd(0, lx + 40, ly - 30, s2, gen2));
        const bool again = ox("logwindo") == lx + 40 && ox("ability") == ax + 40 && oy("ability") == ay - 30;
        run(group_to_cmd(0, lx, ly, s2, gen2));
        check(again && ox("logwindo") == lx && oy("logwindo") == ly && ox("ability") == ax && oy("ability") == ay,
            "the same move_group to x,y again moves nothing; to where the anchor was puts both back");
    }

    // ownership: last writer wins; a handle's reset touches only what it last wrote
    {
        run(cmd(kOpMove, "persona", 100, 100, s1, gen1));
        run(cmd(kOpMove, "equip", 0, 0, s1, gen1));
        open_now("equip");
        run(cmd(kOpMove, "equip", 111, 222, s2, gen2));
        run(cmd(kOpResetOwned, NULL, 0, 0, s1, gen1));
        check(!g_e.memory.e[name("persona")].active && ox("persona") == 16
                && g_e.memory.owned_by(name("equip"), s2, gen2) && ox("equip") == 111,
            "reset_all for one handle resets its windows, not the one another handle wrote last");
    }

    // query: answered by its result word, never blocked or closed
    {
        open_now("query");
        run(cmd(kOpQueryAnswer, "query", 0, 0, s1, gen1, 3));
        check(rd16(controller("query"), kQueryResult) == 3, "query_answer writes the result word the script waits on");
        const int closes = g_close_calls;
        run(cmd(kOpClose, "query", 0, 0, s1, gen1));
        check(g_close_calls == closes && live("query"), "the drain never closes query");
        wr32(controller("query"), kCtlMenu, 0);
        const LONG errors = g_e.drain_errors;
        run(cmd(kOpQueryAnswer, "query", 0, 0, s1, gen1, 3));
        check(g_e.drain_errors == errors + 1, "query_answer with query closed is a drain error");
    }

    // keys the inventory does not know
    {
        const LONG before = g_e.unmatched_keys;
        void* unknown = hooked_open(g_mcb, NULL, "tkdebug buff", 1, 1);
        check(unknown == NULL && g_e.unmatched_keys == before + 1,
            "an open of a key no row has (a menu's name under the debug type) is counted, not reported");
        events(&cursor, ev, 64);
        hooked_open(g_mcb, NULL, "MENU    BUFF", 1, 1);
        const int n = events(&cursor, ev, 64);
        check(n == 1 && is_event(ev[0], kEvOpened, "buff"), "an upper-case key still identifies its menu");
    }

    // query_cancel: the cancel answer the game's cancel writes, only while
    // the cancel-allowed byte is 1, and nothing else; never its input. The
    // event's wait closes query, a tick later.
    {
        uint8_t* q = open_now("query");
        uint8_t* qc = controller("query");
        wr16(q, kMenuCursor, 2);
        wr16(qc, kQueryResult, 0);
        clear_log();
        g_query_cancel_allowed = 0;
        LONG errors = g_e.drain_errors;
        const int closes = g_close_calls;
        run(cmd(kOpQueryCancel, "query", 0, 0, s1, gen1));
        check(g_log.count == 0 && rd16(qc, kQueryResult) == 0 && live("query") == q && g_e.drain_errors == errors + 1
                && strstr(g_e.error, "query cannot be cancelled now"),
            "query_cancel while the cancel-allowed byte is 0: nothing written, nothing closed, a drain error says so");
        g_query_cancel_allowed = 1;
        errors = g_e.drain_errors;
        run(cmd(kOpQueryCancel, "query", 0, 0, s1, gen1));
        check(rd16(qc, kQueryResult) == 0xFF && g_log.count == 0 && g_close_calls == closes && live("query") == q
                && open_menu(g_e, name("query"), NULL) && g_e.drain_errors == errors,
            "query_cancel while it is 1: the word 0xFF and nothing else; no close, query still open");
        events(&cursor, ev, 64);
        game_closes("query");
        const int n = events(&cursor, ev, 64);
        check(n == 1 && is_event(ev[0], kEvClosed, "query") && !live("query"),
            "the event's wait closing query by name posts closed{query}");
        clear_log();
        errors = g_e.drain_errors;
        run(cmd(kOpQueryCancel, "query", 0, 0, s1, gen1));
        check(g_log.count == 0 && g_e.drain_errors == errors + 1 && strstr(g_e.error, "query is not open"),
            "query_cancel with query closed calls nothing");
        check(g_log.query_inputs == 0, "no query reply went through query's input routine");
        g_query_cancel_allowed = 0;
    }

    // an answer checked against the list on the game thread: a value the list
    // does not hold, or holds tombstoned, writes nothing; a reply's count
    // drops once the drain has carried it out
    {
        open_now("query");
        uint8_t* qc = controller("query");
        static uint8_t nodes[2][0x18];
        static uint8_t items[2][kOptionBytes];
        memset(nodes, 0, sizeof(nodes));
        memset(items, 0, sizeof(items));
        wr32(nodes[0], 0, reinterpret_cast<uint32_t>(nodes[1]));
        wr32(nodes[0], 0x10, reinterpret_cast<uint32_t>(items[0]));
        wr32(nodes[1], 0x10, reinterpret_cast<uint32_t>(items[1]));
        nodes[1][0x14] = 1;
        wr16(items[0], kOptionValue, 1);
        wr16(items[1], kOptionValue, 3);
        wr32(qc, kQueryOptions, reinterpret_cast<uint32_t>(nodes[0]));
        wr16(qc, kQueryResult, 0);
        const int q = name("query");
        Command a = cmd(kOpQueryAnswer, "query", 0, 0, s1, gen1, 2);
        a.verb = kVerbAnswer;
        a.flags = kCmdListed | kCmdReply;
        events(&cursor, ev, 64);
        InterlockedIncrement(&g_e.replies[q]);
        run(a);
        int n = events(&cursor, ev, 64);
        const bool hidden = rd16(qc, kQueryResult) == 0 && n == 1
            && is_error(ev[0], s1, gen1, "answer", "query", "no option in the list has the value 2")
            && g_e.replies[q] == 0;
        a.value = 3;
        InterlockedIncrement(&g_e.replies[q]);
        run(a);
        n = events(&cursor, ev, 64);
        const bool tombstoned = rd16(qc, kQueryResult) == 0 && n == 1
            && is_error(ev[0], s1, gen1, "answer", "query", "value 3") && g_e.replies[q] == 0;
        a.value = 1;
        InterlockedIncrement(&g_e.replies[q]);
        run(a);
        check(hidden && tombstoned && rd16(qc, kQueryResult) == 1 && events(&cursor, ev, 64) == 0 && g_e.replies[q] == 0,
            "a listed answer: value 2 (hidden: no node) and value 3 (tombstoned) refused on the game thread with an"
            " error event for the handle, value 1 written; each reply's count back to 0 after the drain");
        Command old = cmd(kOpQueryAnswer, "query", 0, 0, s1, gen1, 2);
        run(old);
        check(rd16(qc, kQueryResult) == 2, "engine abi 1 and 2's query_answer writes any value, as it always did");
        wr32(qc, kQueryOptions, 0);
    }

    // passinpu: the callback, the reset, and the close the game's confirm does
    {
        uint8_t* pc = controller("passinpu");
        int context = 0;
        const void* cb = reinterpret_cast<const void*>(&fake_text_callback);
        wr32(pc, kPassCallback, reinterpret_cast<uint32_t>(cb));
        wr32(pc, kPassContext, reinterpret_cast<uint32_t>(&context));
        wr32(pc, kPassMax, 16);
        open_now("passinpu");
        clear_log();
        Command c = cmd(kOpPassinpuSubmit, NULL, 0, 0, s1, gen1, 3);
        memcpy(c.text, "abcdefghijklmnop", 16);
        run(c);
        static const char expect[32] = {'a', 'b', 'c'};
        check(g_log.text_calls == 1 && g_log.text_context == &context && !g_log.text_null
                && memcmp(g_log.text, expect, 32) == 0,
            "passinpu_submit: the callback gets its context and the text, zero-padded past 16 bytes");
        check(strcmp(g_log.order, "CRK") == 0 && !live("passinpu") && !rdptr(pc, kPassCallback)
                && strncmp(g_log.last_close, "menu    passinpu", 16) == 0,
            "passinpu_submit: callback, reset, then the live entry is closed by name");
        clear_log();
        LONG errors = g_e.drain_errors;
        run(cmd(kOpPassinpuCancel, NULL, 0, 0, s1, gen1));
        check(g_log.count == 0 && g_e.drain_errors == errors + 1 && strstr(g_e.error, "no text entry is pending"),
            "passinpu_cancel with no entry pending calls nothing");
        wr32(pc, kPassCallback, reinterpret_cast<uint32_t>(cb));
        wr32(pc, kPassContext, reinterpret_cast<uint32_t>(&context));
        wr32(pc, kPassMax, 4);
        errors = g_e.drain_errors;
        c = cmd(kOpPassinpuSubmit, NULL, 0, 0, s1, gen1, 5);
        memcpy(c.text, "abcde", 5);
        run(c);
        check(g_log.count == 0 && g_e.drain_errors == errors + 1 && strstr(g_e.error, "at most 4"),
            "passinpu_submit longer than the entry's limit calls nothing");
        run(cmd(kOpPassinpuCancel, NULL, 0, 0, s1, gen1));
        check(strcmp(g_log.order, "CR") == 0 && g_log.text_null && g_log.text_context == &context,
            "passinpu_cancel (blocked, no instance): callback with no text, reset, no close");
    }

    // prtyjoin: the 0x074 reply, then the pending flag, only once queued
    {
        clear_log();
        g_log.party_pending = 0;
        LONG errors = g_e.drain_errors;
        run(cmd(kOpPartyReply, NULL, 0, 0, s1, gen1, 1));
        check(g_log.count == 0 && g_e.drain_errors == errors + 1 && strstr(g_e.error, "no party invite"),
            "prtyjoin_accept with no invite pending sends nothing");
        g_log.party_pending = 1;
        g_log.party_queues = false;
        errors = g_e.drain_errors;
        run(cmd(kOpPartyReply, NULL, 0, 0, s1, gen1, 1));
        check(strcmp(g_log.order, "P") == 0 && g_log.party_pending == 1 && g_e.drain_errors == errors + 1
                && strstr(g_e.error, "stays pending"),
            "prtyjoin_accept the game cannot queue: the invite stays pending (only al of the result is read)");
        clear_log();
        g_log.party_queues = true;
        run(cmd(kOpPartyReply, NULL, 0, 0, s1, gen1, 1));
        check(strcmp(g_log.order, "PX") == 0 && g_log.party_value == 1 && g_log.party_pending == 0,
            "prtyjoin_accept: send(1), then the pending flag is cleared");
        clear_log();
        g_log.party_pending = 1;
        run(cmd(kOpPartyReply, NULL, 0, 0, s1, gen1, 0));
        check(strcmp(g_log.order, "PX") == 0 && g_log.party_value == 0 && g_log.party_pending == 0,
            "prtyjoin_decline: send(0), then the pending flag is cleared");
    }

    // post_close delivery with the outgoing box's window live: the closing
    // row's two words, +0x1B6 then +0x1B4, and no request; refused while the
    // session is closing or gone, the window covered or busy
    {
        uint8_t* dc = controller("delivery");
        uint8_t* dm = open_now("delivery");
        g_log.post_queues = true;
        struct Gate {
            int state;
            uint32_t menu_state;
            uint8_t busy;
            const char* why;
        };
        const Gate gates[] = {
            {0, 0, 0, "the box is already closing"},
            {kDeliveryUnstage, 0, 0, "the box is already closing"},
            {kDeliveryReturn, 0, 0, "the box is already closing"},
            {kDeliveryWait, 0, 0, "the box is already closing"},
            {kDeliveryClosing, 0, 0, "the box is already closing"},
            {5, kStateDormant, 0, "the outgoing box is under another window"},
            {5, 0, 1, "the outgoing box is busy"},
        };
        bool refused = dm != NULL;
        for (size_t i = 0; i < sizeof(gates) / sizeof(gates[0]); ++i) {
            wr16(dc, kDeliveryState, gates[i].state);
            wr16(dc, kDeliveryNext, 0x77);
            wr32(dm, kMenuState, gates[i].menu_state);
            dm[kMenuBusy] = gates[i].busy;
            clear_log();
            const LONG errors = g_e.drain_errors;
            run(cmd(kOpPostClose, "delivery", 0, 0, s1, gen1));
            refused = refused && g_log.count == 0 && rd16(dc, kDeliveryState) == gates[i].state
                && rd16(dc, kDeliveryNext) == 0x77 && g_e.drain_errors == errors + 1
                && strstr(g_e.error, gates[i].why);
        }
        check(refused,
            "post_close delivery with its window live: refused in states 0, 0x15, 0x16, 0x17 and 0x19 (the box is"
            " already closing), covered and busy; nothing sent, nothing written");
        wr32(dm, kMenuState, 0);
        dm[kMenuBusy] = 0;
        wr16(dc, kDeliveryState, 5);
        wr16(dc, kDeliveryNext, 0x77);
        clear_log();
        LONG errors = g_e.drain_errors;
        run(cmd(kOpPostClose, "delivery", 0, 0, s1, gen1));
        check(g_log.count == 0 && rd16(dc, kDeliveryNext) == kDeliveryRequest
                && rd16(dc, kDeliveryState) == kDeliveryUnstage && g_e.drain_errors == errors,
            "post_close delivery with its window live in state 5: +0x1B6 = 0x18, +0x1B4 = 0x15, as its closing row"
            " writes; no request, no other call");
        wr16(dc, kDeliveryState, kDeliveryRequest);
        clear_log();
        run(cmd(kOpPostClose, "delivery", 0, 0, s1, gen1));
        check(g_log.count == 0 && rd16(dc, kDeliveryState) == kDeliveryUnstage && g_e.drain_errors == errors,
            "and in state 0x18, which its request-close also takes back to 0x15");
        run(cmd(kOpClose, "delivery", 0, 0, s1, gen1));
    }

    // post_close: the request first, the waiting state only once it is queued
    {
        uint8_t* dc = controller("delivery");
        wr16(dc, kDeliveryState, 0);
        clear_log();
        g_log.post_queues = true;
        LONG errors = g_e.drain_errors;
        run(cmd(kOpPostClose, "delivery", 0, 0, s1, gen1));
        check(g_log.count == 0 && g_e.drain_errors == errors + 1 && strstr(g_e.error, "no outgoing"),
            "post_close delivery with no session sends nothing");
        wr16(dc, kDeliveryState, 5);
        g_log.post_queues = false;
        run(cmd(kOpPostClose, "delivery", 0, 0, s1, gen1));
        check(strcmp(g_log.order, "B") == 0 && rd16(dc, kDeliveryState) == 5 && strstr(g_e.error, "could not queue"),
            "post_close delivery the game cannot queue: the state is left as it was");
        clear_log();
        g_log.post_queues = true;
        run(cmd(kOpPostClose, "delivery", 0, 0, s1, gen1));
        check(strcmp(g_log.order, "B") == 0 && g_log.post_command == 0x0F && rd16(dc, kDeliveryState) == 0x19,
            "post_close delivery: request 15, then state 0x19 for the server's reply");
        clear_log();
        errors = g_e.drain_errors;
        run(cmd(kOpPostClose, "delivery", 0, 0, s1, gen1));
        check(g_log.count == 0 && g_e.drain_errors == errors + 1 && strstr(g_e.error, "already waiting"),
            "post_close delivery twice sends nothing the second time");

        uint8_t* pc = controller("post1");
        wr32(pc, kPostState, 3);
        open_now("post1");
        clear_log();
        errors = g_e.drain_errors;
        run(cmd(kOpPostClose, "post2", 0, 0, s1, gen1));
        check(g_log.count == 0 && rd32(pc, kPostState) == 3 && g_e.drain_errors == errors + 1
                && strstr(g_e.error, "incoming box is open"),
            "post_close post2 while post1 is live sets nothing");
        run(cmd(kOpClose, "post1", 0, 0, s1, gen1));
        clear_log();
        run(cmd(kOpPostClose, "post2", 0, 0, s1, gen1));
        check(strcmp(g_log.order, "BY") == 0 && g_log.post_command == 0x0F && rd32(pc, kPostState) == kPostClosing,
            "post_close post2 (blocked): request 15, then post1's request-close sets 0x12");
        clear_log();
        errors = g_e.drain_errors;
        run(cmd(kOpPostClose, "post1", 0, 0, s1, gen1));
        check(g_log.count == 0 && g_e.drain_errors == errors + 1 && strstr(g_e.error, "already waiting"),
            "post_close post1 with 0x12 set never calls request-close again (that would tear it down)");
        wr32(pc, kPostState, 0);
        errors = g_e.drain_errors;
        run(cmd(kOpPostClose, "post1", 0, 0, s1, gen1));
        check(g_log.count == 0 && g_e.drain_errors == errors + 1 && strstr(g_e.error, "no incoming"),
            "post_close post1 with no session sends nothing");
    }

    // link5_cancel: the family close, only with a choice pending: the event's
    // latch 1, and link5 opened by the event's handler (mode 1, its callback)
    {
        uint8_t* lc = controller("link5");
        const uint32_t event_cb = reinterpret_cast<uint32_t>(&fake_link5_callback);
        clear_log();
        LONG errors = g_e.drain_errors;
        run(cmd(kOpLink5Cancel, NULL, 0, 0, s1, gen1));
        check(g_log.count == 0 && g_e.drain_errors == errors + 1, "link5_cancel with no callback set calls nothing");
        g_link5_latch = 1;
        wr32(lc, kLink5Mode, 0);
        wr32(lc, kLink5Callback, 0);
        errors = g_e.drain_errors;
        run(cmd(kOpLink5Cancel, NULL, 0, 0, s1, gen1));
        check(g_log.count == 0 && g_e.drain_errors == errors + 1 && strstr(g_e.error, "no linkshell choice is pending"),
            "link5_cancel with the latch at 1 but link5 opened by another list opener (mode 0, no callback) calls"
            " nothing: no linkshell choice is pending");
        g_link5_latch = 0;
        wr32(lc, kLink5Mode, 1);
        wr32(lc, kLink5Callback, event_cb);
        errors = g_e.drain_errors;
        run(cmd(kOpLink5Cancel, NULL, 0, 0, s1, gen1));
        check(g_log.count == 0 && g_e.drain_errors == errors + 1 && strstr(g_e.error, "no linkshell choice is pending"),
            "link5_cancel with mode 1 and the event's callback but the latch already cleared calls nothing");
        g_link5_latch = 1;
        wr32(lc, kLink5Callback, reinterpret_cast<uint32_t>(&fake_text_callback));
        errors = g_e.drain_errors;
        run(cmd(kOpLink5Cancel, NULL, 0, 0, s1, gen1));
        check(g_log.count == 0 && g_e.drain_errors == errors + 1 && strstr(g_e.error, "no linkshell choice is pending"),
            "link5_cancel with the latch 1 and mode 1 but a callback other than the event's calls nothing");
        wr32(lc, kLink5Callback, event_cb);
        run(cmd(kOpLink5Cancel, NULL, 0, 0, s1, gen1));
        check(strcmp(g_log.order, "L") == 0, "link5_cancel: the clear routine, this = link5's controller");
    }

    // link5_confirm: the callback with the slot's cache entry, then the clear
    {
        uint8_t* lc = controller("link5");
        wr32(lc, kLink5Callback, 0);
        clear_log();
        LONG errors = g_e.drain_errors;
        run(cmd(kOpLink5Confirm, NULL, 0, 0, s1, gen1, 3));
        check(g_log.count == 0 && g_e.drain_errors == errors + 1 && strstr(g_e.error, "no linkshell choice is pending"),
            "link5_confirm with no callback set calls nothing");
        wr32(lc, kLink5Callback, reinterpret_cast<uint32_t>(&fake_link5_callback));
        g_link5_latch = 0;
        errors = g_e.drain_errors;
        run(cmd(kOpLink5Confirm, NULL, 0, 0, s1, gen1, 3));
        check(g_log.count == 0 && g_e.drain_errors == errors + 1 && strstr(g_e.error, "no linkshell choice is pending"),
            "link5_confirm with mode 1 and the event's callback but the latch 0 calls nothing");
        g_link5_latch = 1;
        errors = g_e.drain_errors;
        run(cmd(kOpLink5Confirm, NULL, 0, 0, s1, gen1, 16));
        check(g_log.count == 0 && g_e.drain_errors == errors + 1 && strstr(g_e.error, "slot 16"),
            "link5_confirm of slot 16, past the cache, calls nothing");
        run(cmd(kOpLink5Confirm, NULL, 0, 0, s1, gen1, 3));
        check(strcmp(g_log.order, "JL") == 0 && g_log.link5_slot == 3
                && g_log.link5_entry == g_link5_cache + 3 * kLink5Entry,
            "link5_confirm 3: the callback gets slot 3 and its cache entry, then the clear routine");
        clear_log();
        run(cmd(kOpLink5Confirm, NULL, 0, 0, s1, gen1, 15));
        check(strcmp(g_log.order, "JL") == 0 && g_log.link5_slot == 15
                && g_log.link5_entry == g_link5_cache + 15 * kLink5Entry,
            "link5_confirm 15: the last entry of the cache");

        // the list link5 shows: a header row, then rows holding their cache
        // entries (slots 3 and 15)
        static uint8_t rows[3 * kLink5RowStride];
        memset(rows, 0, sizeof(rows));
        wr32(rows + kLink5RowStride, kLink5RowRecord, reinterpret_cast<uint32_t>(g_link5_cache + 3 * kLink5Entry));
        wr32(rows + 2 * kLink5RowStride, kLink5RowRecord, reinterpret_cast<uint32_t>(g_link5_cache + 15 * kLink5Entry));
        wr32(lc, kLink5Rows, reinterpret_cast<uint32_t>(rows));
        wr32(lc, kLink5Count, 3);
        g_link5_latch = 1;
        clear_log();
        Command listed = cmd(kOpLink5Confirm, "link5", 0, 0, s1, gen1, 4);
        listed.verb = kVerbAnswer;
        listed.flags = kCmdListed;
        events(&cursor, ev, 64);
        run(listed);
        const int n = events(&cursor, ev, 64);
        const bool refused = g_log.count == 0 && n == 1
            && is_error(ev[0], s1, gen1, "answer", "link5", "slot 4 is not in the list");
        listed.value = 15;
        run(listed);
        check(refused && strcmp(g_log.order, "JL") == 0 && g_log.link5_slot == 15 && link5_lists(g_e, lc, 3)
                && !link5_lists(g_e, lc, 0),
            "a listed link5 answer: slot 4, which no row holds, refused with an error event; slot 15 answered;"
            " the header row's empty record lists no slot 0");
    }

    // arealist on the game's layout (fake_arealist.h): the reader, then the
    // replies, which end an NPC event's choice (modes 1 and 2) the way the
    // game's own Enter on a zone row and its cancel at the top do, with the
    // window open or blocked, never through the input routine
    {
        uint8_t* ac = controller("arealist");
        fake_area::prepare(ac, 0, true);
        memset(&fake_area::g_log, 0, sizeof(fake_area::g_log));
        clear_log();
        LONG errors = g_e.drain_errors;
        run(cmd(kOpArealistAnswer, "arealist", 0, 0, s1, gen1, 4));
        run(cmd(kOpArealistCancel, "arealist", 0, 0, s1, gen1));
        check(g_log.count == 0 && fake_area::g_log.closes == 0 && g_e.drain_errors == errors + 2
                && strstr(g_e.error, kAreaNotAsking) && ac[kAreaLatch] == 1,
            "the search (mode 0) with its latch set and no window: answer and cancel refused on the game thread,"
            " nothing called");

        uint8_t* am = open_now("arealist");
        fake_area::opened(ac);
        drain(g_e);
        events(&cursor, ev, 64);
        int at = 0;
        int top = 0;
        const bool position = area_position(ac, am, area_count(ac), &at, &top) && at == 1 && top == 1;
        check(area_count(ac) == 45 && position && area_row_of(ac, 4) == 0 && area_row_of(ac, -1) == 1
                && area_row_of(ac, 0) == 2 && area_row_of(ac, -2) == 3 && area_row_of(ac, -4) == 5
                && area_row_of(ac, -43) == 44 && area_row_of(ac, 5) == -1,
            "arealist, the search's top level: 45 rows, Current Area (4), Current Region (-1) and All Areas (0)"
            " first, then regions -2 .. -43; the cursor on row 1, row 1 the first shown");

        // the cursor events follow the row under the cursor through a scroll
        char seen[64] = "";
        const int picks[2] = {23, 44};
        for (int k = 0; k < 2; ++k) {
            fake_area::select_index(ac, picks[k], 0);
            drain(g_e);
            const int n = events(&cursor, ev, 64);
            at = top = 0;
            area_position(ac, am, area_count(ac), &at, &top);
            const size_t used = strlen(seen);
            snprintf(seen + used, sizeof(seen) - used, "%s%d/%d/%d:%d", k ? " " : "", at, top, n,
                n == 1 && event_type(ev[0]) == kEvCursor && cursor_name(ev[0]) == name("arealist") ? cursor_row(ev[0]) : 0);
        }
        check(strcmp(seen, "24/24/1:24 45/36/1:45") == 0,
            "the selection on row 24, then on the last: cursor 24 and 45, top 24 and 36 with ten rows shown, one"
            " cursor{arealist} each with that row");

        // the search (mode 0), open: no reply; the player's own close ends it
        clear_log();
        events(&cursor, ev, 64);
        Command answer = cmd(kOpArealistAnswer, "arealist", 0, 0, s1, gen1, 4);
        answer.verb = kVerbAnswer;
        run(answer);
        int n = events(&cursor, ev, 64);
        errors = g_e.drain_errors;
        run(cmd(kOpArealistCancel, "arealist", 0, 0, s1, gen1));
        check(g_log.count == 0 && fake_area::g_log.closes == 0 && n == 1
                && is_error(ev[0], s1, gen1, "answer", "arealist", kAreaNotAsking) && g_e.drain_errors == errors + 1
                && ac[kAreaLatch] == 1 && live("arealist") == am,
            "the search open (mode 0): answer refused with an error event to the handle, cancel refused, nothing"
            " called, the list open");
        game_closes("arealist");
        check(ac[kAreaLatch] == 0 && ac[kAreaMode] == 0 && fake_area::g_log.latch_clears == 1,
            "the game's close of the open list runs its begin-close: latch and mode cleared");
        drain(g_e);
        events(&cursor, ev, 64);

        // mode 1, grouped: the regions alone; a region's id and a zone not
        // listed refused; once the player opens region 03, zone 6 answered
        fake_area::prepare(ac, 1, true);
        am = open_now("arealist");
        fake_area::opened(ac);
        drain(g_e);
        events(&cursor, ev, 64);
        const bool regions = area_count(ac) == 42 && area_row_of(ac, -4) == 2;
        memset(&fake_area::g_log, 0, sizeof(fake_area::g_log));
        clear_log();
        answer.value = static_cast<uint32_t>(-4);
        run(answer);
        n = events(&cursor, ev, 64);
        const bool region_refused = n == 1 && is_error(ev[0], s1, gen1, "answer", "arealist",
            "no zone row of the list has the id -4");
        answer.value = 6;
        run(answer);
        n = events(&cursor, ev, 64);
        check(regions && region_refused && n == 1
                && is_error(ev[0], s1, gen1, "answer", "arealist", "no zone row of the list has the id 6")
                && g_log.count == 0 && fake_area::g_log.closes == 0 && ac[kAreaLatch] == 1 && live("arealist") == am,
            "mode 1, grouped: 42 region rows; answer -4 (a region) and 6 (no row at this level) refused with error"
            " events, nothing called, the list open");
        fake_area::select_index(ac, 2, 0);
        fake_area::input(ac, NULL, 5, 0);
        const bool drilled = rd16(ac, kAreaLevel) == 4 && area_row_of(ac, 6) == 2 && ac[kAreaLatch] == 1;
        const int inputs = fake_area::g_log.inputs;
        drain(g_e);
        events(&cursor, ev, 64);
        clear_log();
        run(answer);
        n = events(&cursor, ev, 64);
        check(drilled && strcmp(g_log.order, "AKK") == 0 && strncmp(g_log.last_close, "menu    scsibori", 16) == 0
                && rd16(ac, kAreaZone) == 6 && ac[kAreaLatch] == 0 && ac[kAreaMode] == 0
                && fake_area::g_log.closes == 1 && fake_area::g_log.latch_clears == 1 && !rdptr(ac, kAreaRows)
                && !live("arealist") && fake_area::g_log.inputs == inputs && fake_area::g_log.searches == 0,
            "mode 1, the player inside region 03, answer 6: 6 written, close+reset, its close by name reaching the"
            " begin-close (latch and mode cleared, once), the rows freed, scsibori closed by name; the input"
            " routine never called by the engine");

        // the same choice made by the player's own Enter leaves the list the same
        fake_area::prepare(ac, 1, true);
        am = open_now("arealist");
        fake_area::opened(ac);
        fake_area::select_index(ac, 2, 0);
        fake_area::input(ac, NULL, 5, 0);
        fake_area::select_index(ac, 2, 0);
        memset(&fake_area::g_log, 0, sizeof(fake_area::g_log));
        clear_log();
        fake_area::input(ac, NULL, 5, 0);
        check(strcmp(g_log.order, "KK") == 0 && rd16(ac, kAreaZone) == 6 && ac[kAreaLatch] == 0 && ac[kAreaMode] == 0
                && fake_area::g_log.closes == 1 && fake_area::g_log.latch_clears == 1 && !rdptr(ac, kAreaRows)
                && !live("arealist") && strncmp(g_log.last_close, "menu    scsibori", 16) == 0,
            "the player's own Enter on zone 6 there: the same result, closes and cleared latch as the answer");
        drain(g_e);
        events(&cursor, ev, 64);

        // blocked: the event's opener armed the latch and the mode, the open
        // was refused, the result still holds an earlier answer
        fake_area::arm(ac, 1);
        wr16(ac, kAreaZone, 231);
        memset(&fake_area::g_log, 0, sizeof(fake_area::g_log));
        clear_log();
        answer.value = 600;
        run(answer);
        n = events(&cursor, ev, 64);
        const bool out_of_range = n == 1 && is_error(ev[0], s1, gen1, "answer", "arealist", "600 is not a zone id")
            && g_log.count == 0 && ac[kAreaLatch] == 1 && rd16(ac, kAreaZone) == 231;
        answer.value = 12;
        run(answer);
        n = events(&cursor, ev, 64);
        check(!live("arealist") && out_of_range && n == 0 && strcmp(g_log.order, "AKHK") == 0
                && rd16(ac, kAreaZone) == 12 && ac[kAreaLatch] == 0 && ac[kAreaMode] == 0
                && fake_area::g_log.closes == 1 && fake_area::g_log.latch_clears == 1
                && strncmp(g_log.last_close, "menu    scsibori", 16) == 0,
            "blocked, mode 1: 600 refused (no zone id); 12 written, close+reset (its close by name finds no"
            " instance), the latch clear called alone (latch and mode cleared), scsibori closed by name");
        fake_area::arm(ac, 2);
        wr16(ac, kAreaZone, 231);
        memset(&fake_area::g_log, 0, sizeof(fake_area::g_log));
        clear_log();
        run(cmd(kOpArealistCancel, "arealist", 0, 0, s1, gen1));
        check(strcmp(g_log.order, "AKH") == 0 && rd16(ac, kAreaZone) == -1 && ac[kAreaLatch] == 0
                && ac[kAreaMode] == 0 && fake_area::g_log.latch_clears == 1,
            "blocked, mode 2, cancel: -1 written over the earlier answer, close+reset, the latch clear alone, no"
            " scsibori");

        // mode 2, open: flat; cancel at the top leaves the open hook's -1
        fake_area::prepare(ac, 2, true);
        am = open_now("arealist");
        fake_area::opened(ac);
        const bool flat = area_count(ac) == 84 && area_row_of(ac, 1) == 0;
        memset(&fake_area::g_log, 0, sizeof(fake_area::g_log));
        clear_log();
        run(cmd(kOpArealistCancel, "arealist", 0, 0, s1, gen1));
        check(flat && strcmp(g_log.order, "AK") == 0 && strncmp(g_log.last_close, "menu    arealist", 16) == 0
                && rd16(ac, kAreaZone) == -1 && ac[kAreaLatch] == 0 && fake_area::g_log.latch_clears == 1
                && !live("arealist") && fake_area::g_log.inputs == 0,
            "mode 2, open: every zone, flat; cancel: close+reset through the open instance's begin-close, the"
            " result -1, no scsibori, no input");
        drain(g_e);
        events(&cursor, ev, 64);

        // a latch nothing clears is an error
        fake_area::prepare(ac, 2, true);
        am = open_now("arealist");
        fake_area::opened(ac);
        vt_area[0x08 / 4] = reinterpret_cast<const void*>(&keeping_begin_close);
        errors = g_e.drain_errors;
        answer.value = 5;
        run(answer);
        events(&cursor, ev, 64);
        fake_area::fill_vtable(vt_area);
        check(g_e.drain_errors == errors + 1 && strstr(g_e.error, "latch is still set") && rd16(ac, kAreaZone) == 5,
            "a close that leaves the latch set: an error after the answer");
        ac[kAreaLatch] = 0;
        ac[kAreaMode] = 0;

        // modes 3 and 4: no reply reaches them
        for (int mode = 3; mode <= 4; ++mode) {
            fake_area::prepare(ac, mode, false);
            if (!live("arealist")) {
                am = open_now("arealist");
            }
            fake_area::opened(ac);
            errors = g_e.drain_errors;
            clear_log();
            memset(&fake_area::g_log, 0, sizeof(fake_area::g_log));
            run(cmd(kOpArealistAnswer, "arealist", 0, 0, s1, gen1, 3));
            run(cmd(kOpArealistCancel, "arealist", 0, 0, s1, gen1));
            char label[96];
            snprintf(label, sizeof(label), "mode %d: answer and cancel refused on the game thread, nothing called", mode);
            check(g_log.count == 0 && fake_area::g_log.closes == 0 && g_e.drain_errors == errors + 2
                    && strstr(g_e.error, kAreaNotAsking) && ac[kAreaLatch] == 1 && live("arealist") == am, label);
        }
        game_closes("arealist");
        ac[kAreaLatch] = 0;
        ac[kAreaMode] = 0;
        drain(g_e);
        events(&cursor, ev, 64);
    }

    // resize: template swap, back to the origin, re-dock; or SetFrameRect
    {
        uint8_t* party = open_now("partywin");
        const int px = ox("partywin");
        const int py = oy("partywin");
        wr16(party, kMenuItems, 2);
        clear_log();
        const int calls = g_setpos_calls;
        run(cmd(kOpResizeRows, "partywin", 0, 0, s1, gen1, 3));
        check(strcmp(g_log.order, "SF") == 0 && memcmp(g_log.swap_key, "menu    ptw3    ", 16) == 0
                && g_log.swap_key[16] == 0 && g_log.swap_menu == party && g_setpos_calls == calls + 1
                && ox("partywin") == px && oy("partywin") == py && rd16(party, kMenuRect) == px
                && rd16(party, kMenuRect + 6) == py + 60,
            "resize partywin 3: swap to ptw3, then SetPosition back to its origin and SetFrameRect hanging the frame"
            " the swap left on its bottom (py + its default height 60), no re-dock (it does not dock)");
        check(rd16(party, kMenuItems) == 2 && g_log.cursor_calls == 0,
            "resize partywin 3 leaves its item count (+0x58) alone and calls no SetCursor, as its owner does");
        uint8_t* pmo = open_now("playermo");
        wr16(pmo, kMenuOrigin, 40);
        wr16(pmo, kMenuOrigin + 2, 60);
        wr16(pmo, kMenuItems, 3);
        wr16(pmo, kMenuCursor, 2);
        clear_log();
        run(cmd(kOpResizeRows, "playermo", 0, 0, s1, gen1, 10));
        check(strcmp(g_log.order, "USUD") == 0 && memcmp(g_log.swap_key, "menu    actiom10", 16) == 0
                && ox("playermo") == 40 && oy("playermo") == 60 && g_log.dock_group == 0 && g_log.dock_menu == pmo,
            "resize playermo 10: SetCursor, swap to actiom10, back to 40,60, SetCursor, DockReset(0) for its"
            " chat-log dock bit");
        check(rd16(pmo, kMenuItems) == 10 && g_log.cursor_calls == 2 && g_log.cursor_menu == pmo
                && g_log.cursor_items[0] == 10 && g_log.cursor_row[0] == 2 && g_log.cursor_warp[0] == 0
                && g_log.cursor_items[1] == 10 && g_log.cursor_row[1] == 2 && g_log.cursor_warp[1] == 0,
            "resize playermo 10: +0x58 = 10 before the first SetCursor(+0x4C, 0), and SetCursor(+0x4C, 0) again"
            " after the swap, as its owner does");
        wr16(pmo, kMenuCursor, 6);
        clear_log();
        run(cmd(kOpResizeRows, "playermo", 0, 0, s1, gen1, 4));
        check(strcmp(g_log.order, "USUD") == 0 && rd16(pmo, kMenuItems) == 4 && rd16(pmo, kMenuCursor) == 4
                && g_log.cursor_row[0] == 6 && g_log.cursor_items[0] == 4 && g_log.cursor_row[1] == 4
                && memcmp(g_log.swap_key, "menu    actionm4", 16) == 0,
            "resize playermo 4 with the cursor on row 6: the count goes to 4 first, so the first SetCursor clamps"
            " the cursor to 4 before the swap");
        run(cmd(kOpResizeRows, "playermo", 0, 0, s1, gen1, 10));
        uint8_t* eq = live("equip");
        eq[kMenuFrameByte] = 1;
        clear_log();
        run(cmd(kOpResizeRect, "equip", 300, 200, s1, gen1));
        check(strcmp(g_log.order, "F") == 0 && g_log.frame_menu == eq && g_log.frame[0] == ox("equip")
                && g_log.frame[1] == oy("equip") && g_log.frame[2] == 300 && g_log.frame[3] == 200
                && g_log.frame[4] == 1 && g_log.frame[5] == 0 && g_log.frame[6] == 1,
            "resize equip 300x200: SetFrameRect at its origin, the frame byte kept, no re-dock");
        clear_log();
        run(cmd(kOpResizeRect, "ability", 90, 30, s1, gen1));
        check(strcmp(g_log.order, "FD") == 0 && g_log.dock_group == 0, "resize of a docked window re-docks it");
        run(cmd(kOpClose, "partywin", 0, 0, s1, gen1));
        clear_log();
        LONG errors = g_e.drain_errors;
        run(cmd(kOpResizeRows, "partywin", 0, 0, s1, gen1, 4));
        const MemEntry& pm = g_e.memory.e[name("partywin")];
        check(g_log.count == 0 && g_e.drain_errors == errors && pm.size_kind == kSizeRows && pm.size_a == 4
                && g_e.memory.size_owned_by(name("partywin"), s1, gen1),
            "resize of a closed window calls nothing now and is remembered, owned by the caller");
        errors = g_e.drain_errors;
        run(cmd(kOpResizeRows, "equip", 0, 0, s1, gen1, 3));
        check(g_log.count == 0 && g_e.drain_errors == errors + 1, "resize by rows of a window with no family calls nothing");
    }

    // remembered sizes: put back at every open after the position and before
    // the re-dock, restored by reset and by the last writer's release, owned
    // apart from the position
    {
        clear_log();
        uint8_t* party = open_now("partywin");
        check(strcmp(g_log.order, "FSF") == 0 && memcmp(g_log.swap_key, "menu    ptw4    ", 16) == 0
                && g_log.swap_menu == party && ox("partywin") == g_e.memory.e[name("partywin")].x
                && rd16(party, kMenuRect + 6) == oy("partywin") + 60,
            "partywin, resized to 4 rows while closed, is put at its remembered position by its bottom edge, then"
            " swapped to ptw4 and hung on that bottom, in the open's post");
        run(cmd(kOpClose, "equip", 0, 0, s1, gen1));
        clear_log();
        uint8_t* eq = open_now("equip");
        check(strcmp(g_log.order, "F") == 0 && g_log.frame_menu == eq && g_log.frame[2] == 300
                && g_log.frame[3] == 200 && g_log.frame[0] == ox("equip") && g_log.frame[1] == oy("equip"),
            "equip reopened: its remembered 300x200 put back on at its origin");
        run(cmd(kOpClose, "ability", 0, 0, s1, gen1));
        clear_log();
        open_now("ability");
        check(strcmp(g_log.order, "FD") == 0 && g_log.frame[0] == g_e.memory.e[name("ability")].x
                && g_log.frame[2] == 90 && g_log.frame[3] == 30 && g_log.dock_group == 0,
            "ability reopened: its remembered position, then 90x30 at that origin, then DockReset(0)");

        clear_log();
        run(cmd(kOpReset, "partywin", 0, 0, s2, gen2));
        check(strcmp(g_log.order, "SF") == 0 && memcmp(g_log.swap_key, "menu    partywin", 16) == 0
                && g_log.swap_key[16] == 0 && g_log.frame[0] == 500 && g_log.frame[1] + g_log.frame[3] == 440
                && rd16(party, kMenuRect) == 500 && rd16(party, kMenuRect + 6) == 440
                && !g_e.memory.e[name("partywin")].size_kind && !g_e.memory.e[name("partywin")].active
                && ox("partywin") == 500 && oy("partywin") == 380,
            "reset partywin: swapped back to the template it was opened from, then home at 500,380 on its default"
            " bottom 440 at the size the swap left, never the 120x60 recorded before its first resize (a"
            " bottom-anchored window's height is its content's); both forgotten");
        clear_log();
        run(cmd(kOpReset, "equip", 0, 0, s1, gen1));
        check(strcmp(g_log.order, "F") == 0 && frame_is(130, 48, 200, 160)
                && ox("equip") == 130 && oy("equip") == 48 && !g_e.memory.e[name("equip")].size_kind,
            "reset equip: its default, then SetFrameRect there to the 200x160 it had before its first resize");
        check(rect_is(live("equip"), 130, 48, 330, 208),
            "reset of the moved and resized equip ends at the game's placement: its frame on the default rect");
        clear_log();
        const int calls = g_setpos_calls;
        run(cmd(kOpReset, "equip", 0, 0, s1, gen1));
        check(g_log.count == 0 && g_setpos_calls == calls, "reset of a window neither moved nor resized leaves it alone");

        // two more writers, owning nothing else
        const int sa = 30;
        const int sb = 31;
        run(cmd(kOpMove, "equip", 250, 90, sa, 1));
        run(cmd(kOpResizeRect, "equip", 180, 120, sb, 1));
        clear_log();
        run(cmd(kOpResetOwned, NULL, 0, 0, sb, 1));
        check(strcmp(g_log.order, "F") == 0 && g_log.frame[0] == 250 && g_log.frame[1] == 90
                && g_log.frame[2] == 200 && g_log.frame[3] == 160 && ox("equip") == 250 && oy("equip") == 90
                && g_e.memory.owned_by(name("equip"), sa, 1) && !g_e.memory.e[name("equip")].size_kind,
            "the release of the size's last writer restores equip's own size at the position another writer set, which stays");
        run(cmd(kOpResizeRect, "equip", 180, 120, sa, 1));
        run(cmd(kOpMove, "equip", 260, 100, sb, 1));
        clear_log();
        run(cmd(kOpResetOwned, NULL, 0, 0, sb, 1));
        const MemEntry& em = g_e.memory.e[name("equip")];
        check(g_log.count == 0 && ox("equip") == 130 && oy("equip") == 48 && !em.active
                && em.size_kind == kSizeRect && em.size_a == 180 && em.size_b == 120
                && g_e.memory.size_owned_by(name("equip"), sa, 1),
            "the release of the position's last writer puts equip home and leaves the size another writer set");
        clear_log();
        run(cmd(kOpResetOwned, NULL, 0, 0, sa, 1));
        check(strcmp(g_log.order, "F") == 0 && g_log.frame[2] == 200 && g_log.frame[3] == 160
                && !g_e.memory.e[name("equip")].size_kind,
            "then that writer's release restores the size");
    }

    // the size a reset restores is the frame's before the engine first sized
    // the window, kept across writers until the size is reset: a window the
    // player configured is not its default rect's size
    {
        uint8_t* eq = live("equip");
        const Placement& ep = g_e.place[name("equip")];
        set_rect(eq, 130, 48, 380, 238);
        run(cmd(kOpResizeRect, "equip", 300, 200, s1, gen1));
        const bool recorded = ep.own_w == 250 && ep.own_h == 190 && rect_is(eq, 130, 48, 430, 248);
        clear_log();
        run(cmd(kOpReset, "equip", 0, 0, s1, gen1));
        check(recorded && strcmp(g_log.order, "F") == 0 && frame_is(130, 48, 250, 190) && rect_is(eq, 130, 48, 380, 238)
                && !ep.size_kind && !g_e.memory.e[name("equip")].size_kind,
            "a w,h resize then reset: SetFrameRect back to the 250x190 the frame had before the resize, not the"
            " default rect's 200x160");

        set_rect(eq, 130, 48, 360, 218);
        run(cmd(kOpResizeRect, "equip", 300, 200, 32, 1));
        run(cmd(kOpResizeRect, "equip", 150, 100, 33, 1));
        const bool second = rect_is(eq, 130, 48, 280, 148) && ep.own_w == 230 && ep.own_h == 170;
        clear_log();
        run(cmd(kOpResetOwned, NULL, 0, 0, 32, 1));
        const bool first_owns_nothing = g_log.count == 0;
        run(cmd(kOpResetOwned, NULL, 0, 0, 33, 1));
        check(second && first_owns_nothing && strcmp(g_log.order, "F") == 0 && frame_is(130, 48, 230, 170)
                && rect_is(eq, 130, 48, 360, 218),
            "two resizes by two handles: the release of the second, the size's last writer, restores the 230x170"
            " recorded before the first, not the 300x200 between them");

        uint8_t* party = live("partywin");
        run(cmd(kOpResizeRows, "partywin", 0, 0, s1, gen1, 3));
        run(cmd(kOpResizeRect, "partywin", 200, 50, s1, gen1));
        const bool hung = rect_is(party, 500, 390, 700, 440);
        clear_log();
        run(cmd(kOpReset, "partywin", 0, 0, s1, gen1));
        check(hung && strcmp(g_log.order, "SF") == 0 && memcmp(g_log.swap_key, "menu    partywin", 16) == 0
                && g_log.frame[0] == 500 && g_log.frame[1] + g_log.frame[3] == 440 && rd16(party, kMenuRect + 6) == 440,
            "partywin rows, then w,h (200x50 on its bottom, 390..440), then reset: swapped back to its own template"
            " all the same, then hung on its default bottom 440 at the size the swap left");

        // the same on a top-anchored window whose owner sets the item count:
        // the count recorded before the first resize goes back on around the
        // swap back, then the size recorded then
        uint8_t* pmo = live("playermo");
        const Placement& pp = g_e.place[name("playermo")];
        const bool counted = pp.own_items == 3 && pp.own_w == 10 && pp.own_h == 10
            && g_e.anchor[name("playermo")] == kAnchorTop;
        clear_log();
        run(cmd(kOpReset, "playermo", 0, 0, s1, gen1));
        check(counted && strcmp(g_log.order, "USFUD") == 0 && memcmp(g_log.swap_key, "menu    playermo", 16) == 0
                && g_log.cursor_items[0] == 3 && g_log.cursor_items[1] == 3 && rd16(pmo, kMenuItems) == 3
                && frame_is(40, 60, 10, 10) && rect_is(pmo, 40, 60, 50, 70) && g_log.dock_menu == pmo
                && !g_e.memory.e[name("playermo")].size_kind,
            "reset playermo after rows 10, 4, 10: +0x58 back to the 3 recorded before the first resize and"
            " SetCursor, the swap back to its own template, SetFrameRect to the 10x10 recorded then, SetCursor,"
            " DockReset(0)");
    }

    // the chat-log windows: read off their frames as bottom-anchored, placed
    // by their bottom edge, never laid out. logwindo as measured on the
    // 2026-09-10 client: origin 16,930, default rect 16,930..382,1064, the
    // player's log 1774 wide and 166 tall at rest
    {
        uint8_t* log_ctl = controller("logwindo");
        const void* driven = vt_layout;
        memcpy(log_ctl, &driven, 4);
        run(cmd(kOpReset, "logwindo", 0, 0, s1, gen1));
        const bool forgotten = g_e.anchor[name("logwindo")] == kAnchorUnknown;
        uint8_t* log = live("logwindo");
        set_rect(log, 16, 898, 1790, 1064);
        log[kMenuFrameByte] = 1;
        LONG layouts = g_layout_calls;
        clear_log();
        run(cmd(kOpMove, "logwindo", 16, 898, s1, gen1));
        check(forgotten && g_e.anchor[name("logwindo")] == kAnchorBottom,
            "logwindo at rest, frame 898..1064 on its default rect 930..1064, is read as bottom-anchored at the"
            " engine's first touch (the reset before it forgot the earlier reading)");
        check(strcmp(g_log.order, "F") == 0 && frame_is(16, 866, 1774, 166) && g_log.frame[6] == 1
                && ox("logwindo") == 16 && oy("logwindo") == 898 && rect_is(log, 16, 866, 1790, 1032)
                && g_layout_calls == layouts,
            "move logwindo 16,898: SetPosition, then SetFrameRect(16, 866, 1774, 166): the bottom stays"
            " at y + the default height (1032); its layout, drivable, is never run");
        clear_log();
        run(cmd(kOpReset, "logwindo", 0, 0, s1, gen1));
        check(strcmp(g_log.order, "F") == 0 && frame_is(16, 898, 1774, 166) && ox("logwindo") == 16
                && oy("logwindo") == 930 && rect_is(log, 16, 898, 1790, 1064) && !g_e.memory.e[name("logwindo")].active,
            "reset logwindo: SetPosition(16,930), then SetFrameRect(16, 898, 1774, 166): 898..1064 as the game"
            " keeps it, not the 930..1096 of SetPosition alone");

        const int py = oy("partywin");
        const int ty = oy("targetwi");
        const int sy = oy("subwindo");
        const int ay = oy("ability");
        const int px = ox("partywin");
        clear_log();
        run(group_cmd(0, 0, -100, s2, gen2));
        check(ox("logwindo") == 16 && oy("logwindo") == 830 && rect_is(log, 16, 798, 1790, 964),
            "move_group chat_log 0,-100: logwindo by its bottom edge, frame 798..964");
        check(strcmp(g_log.order, "FF") == 0 && g_log.frame_menu == live("partywin")
                && g_log.frame[1] + g_log.frame[3] == py - 40 && rd16(live("partywin"), kMenuRect + 6) == py - 40,
            "and partywin, carried, by its bottom edge too: SetFrameRect after logwindo's, its bottom at its new"
            " origin + its default height 60");
        check(oy("partywin") == py - 100 && ox("partywin") == px && oy("targetwi") == ty - 100
                && oy("subwindo") == sy - 100 && oy("ability") == ay - 100
                && (rd32(live("targetwi"), kMenuPolicy) & 0x2000) && (rd32(live("subwindo"), kMenuPolicy) & 0x4000)
                && (rd32(live("ability"), kMenuPolicy) & 0x1000)
                && g_e.memory.e[name("partywin")].active && g_e.memory.owned_by(name("partywin"), s2, gen2),
            "move_group chat_log carries the party list: partywin, targetwi docked to it and subwindo docked to"
            " that, with ability docked to the log, all 100 up and still docked; partywin's move is the mover's");
        const int lx = ox("logwindo");
        run(group_cmd(1, 0, 7, s2, gen2));
        check(oy("partywin") == py - 93 && ox("logwindo") == lx && oy("logwindo") == 830,
            "move_group party_list alone still leaves the chat log where it is");

        clear_log();
        run(cmd(kOpResizeRect, "logwindo", 1000, 120, s1, gen1));
        check(strcmp(g_log.order, "F") == 0 && frame_is(16, 844, 1000, 120) && rect_is(log, 16, 844, 1016, 964),
            "resize logwindo 1000x120: SetFrameRect(16, 844, 1000, 120), the bottom kept at 964");
        run(cmd(kOpClose, "logwindo", 0, 0, s1, gen1));
        clear_log();
        log = open_now("logwindo");
        check(strcmp(g_log.order, "FF") == 0 && frame_is(16, 844, 1000, 120) && ox("logwindo") == 16
                && oy("logwindo") == 830 && rect_is(log, 16, 844, 1016, 964),
            "logwindo reopened at its template size: its remembered 16,830 by the bottom edge, then its"
            " remembered 1000x120 on that bottom, frame 844..964");
        clear_log();
        layouts = g_layout_calls;
        run(cmd(kOpReset, "logwindo", 0, 0, s1, gen1));
        check(strcmp(g_log.order, "F") == 0 && frame_is(16, 944, 1000, 120) && ox("logwindo") == 16
                && oy("logwindo") == 930 && rect_is(log, 16, 944, 1016, 1064) && g_layout_calls == layouts
                && !g_e.memory.e[name("logwindo")].active && !g_e.memory.e[name("logwindo")].size_kind
                && g_e.anchor[name("logwindo")] == kAnchorUnknown,
            "reset of the moved and resized logwindo: home at 16,930 on its bottom 1064 at the 1000x120 it has"
            " now (a bottom-anchored window's height is its content's: no recorded size, never the default"
            " rect's 366x134); position, size and anchoring forgotten");

        uint8_t* log2 = open_now("logwin2");
        set_rect(log2, 16, 728, 1790, 894);
        clear_log();
        layouts = g_layout_calls;
        run(cmd(kOpMove, "logwin2", 30, 500, s1, gen1));
        check(strcmp(g_log.order, "F") == 0 && frame_is(30, 468, 1774, 166) && rect_is(log2, 30, 468, 1804, 634)
                && g_layout_calls == layouts,
            "move logwin2 30,500: by its bottom edge too (634 = 500 + 134), its layout never run");
        clear_log();
        run(cmd(kOpReset, "logwin2", 0, 0, s1, gen1));
        check(frame_is(16, 728, 1774, 166) && ox("logwin2") == 16 && oy("logwin2") == 760
                && rect_is(log2, 16, 728, 1790, 894),
            "reset logwin2: back on its default bottom, 894");
        set_rect(log2, 16, 894, 16, 894);
        clear_log();
        const LONG errors = g_e.drain_errors;
        run(cmd(kOpMove, "logwin2", 40, 40, s1, gen1));
        check(g_log.count == 0 && ox("logwin2") == 16 && g_e.drain_errors == errors + 1
                && strstr(g_e.error, "logwin2: its frame or its default rect has no size"),
            "a chat-log window whose frame has no size is left where it is, and a drain error says so");
        const void* self = vt_self;
        memcpy(log_ctl, &self, 4);
    }

    // the party window hangs from its default rect's bottom like the chat
    // log, and is read off its frame the same way. partywin as measured on
    // the 2026-09-10 client, 2026-10-03 12:54, with one member: origin
    // 1792,930, default rect 1792,930..1904,1064, frame 1792,1030..1904,1064
    {
        run(cmd(kOpReset, "logwindo", 0, 0, s1, gen1));
        run(cmd(kOpReset, "partywin", 0, 0, s1, gen1));
        uint8_t* log = live("logwindo");
        uint8_t* party = live("partywin");
        uint8_t* eq = live("equip");
        set_rect(log, 16, 898, 1790, 1064);
        wr16(party, kMenuDefault, 1792);
        wr16(party, kMenuDefault + 2, 930);
        wr16(party, kMenuDefault + 4, 1904);
        wr16(party, kMenuDefault + 6, 1064);
        wr16(party, kMenuOrigin, 1792);
        wr16(party, kMenuOrigin + 2, 930);
        set_rect(party, 1792, 1030, 1904, 1064);
        set_rect(eq, 130, 48, 330, 208);
        const bool unread = g_e.anchor[name("logwindo")] == kAnchorUnknown
            && g_e.anchor[name("partywin")] == kAnchorUnknown && g_e.anchor[name("equip")] == kAnchorUnknown;
        const void* driven = vt_layout;
        memcpy(controller("logwindo"), &driven, 4);
        memcpy(controller("partywin"), &driven, 4);
        LONG layouts = g_layout_calls;
        run(cmd(kOpMove, "equip", 140, 60, s1, gen1));
        check(unread && g_e.anchor[name("equip")] == kAnchorTop && ox("equip") == 140 && oy("equip") == 60
                && rect_is(eq, 140, 60, 340, 220) && g_layout_calls == layouts + 1,
            "equip, its frame on its default rect, is read as top-anchored: SetPosition and its layout");
        run(cmd(kOpReset, "equip", 0, 0, s1, gen1));

        layouts = g_layout_calls;
        run(group_cmd(0, 0, -100, s2, gen2));
        check(g_e.anchor[name("logwindo")] == kAnchorBottom && g_e.anchor[name("partywin")] == kAnchorBottom,
            "the group move's first touch reads logwindo (898..1064 on 930..1064) and partywin (1030..1064 on"
            " 930..1064) as bottom-anchored");
        check(ox("partywin") == 1792 && oy("partywin") == 830 && rect_is(party, 1792, 930, 1904, 964)
                && oy("logwindo") == 830 && rect_is(log, 16, 798, 1790, 964) && g_layout_calls == layouts,
            "move_group chat_log 0,-100: partywin to 830, frame 930..964, 100 up (SetPosition alone put it at"
            " 830..864, 200 up); logwindo 798..964; neither layout run");
        run(group_cmd(0, 0, 100, s2, gen2));
        check(ox("partywin") == 1792 && oy("partywin") == 930 && rect_is(party, 1792, 1030, 1904, 1064)
                && oy("logwindo") == 930 && rect_is(log, 16, 898, 1790, 1064) && g_layout_calls == layouts,
            "move_group chat_log 0,100: back where the game had them, partywin 1030..1064 and logwindo 898..1064");

        set_rect(party, 1792, 1010, 1904, 1064);
        run(cmd(kOpMove, "partywin", 1500, 500, s1, gen1));
        const bool moved = ox("partywin") == 1500 && oy("partywin") == 500 && rect_is(party, 1500, 580, 1612, 634);
        set_rect(party, 1500, 600, 1612, 634);
        clear_log();
        run(cmd(kOpReset, "partywin", 0, 0, s1, gen1));
        check(moved && strcmp(g_log.order, "F") == 0 && frame_is(1792, 1030, 112, 34) && ox("partywin") == 1792
                && oy("partywin") == 930 && rect_is(party, 1792, 1030, 1904, 1064) && g_layout_calls == layouts
                && g_e.anchor[name("partywin")] == kAnchorUnknown,
            "partywin moved to 1500,500 with two members (frame 580..634), its roster then back to one: reset puts"
            " it home on its bottom 1064 at the 112x34 it has now; its anchoring forgotten");

        run(cmd(kOpResizeRect, "partywin", 112, 74, s1, gen1));
        const bool sized = rect_is(party, 1792, 990, 1904, 1064) && g_e.anchor[name("partywin")] == kAnchorBottom;
        set_rect(party, 1792, 1010, 1904, 1064);
        clear_log();
        run(cmd(kOpReset, "partywin", 0, 0, s1, gen1));
        check(sized && strcmp(g_log.order, "F") == 0 && frame_is(1792, 1010, 112, 54)
                && rect_is(party, 1792, 1010, 1904, 1064) && !g_e.memory.e[name("partywin")].size_kind,
            "partywin resized to 112x74 on its bottom (990..1064), its roster then at two: reset keeps the 112x54"
            " it has, not the 112x34 recorded before the resize nor the 112x74");

        clear_log();
        run(cmd(kOpResizeRows, "partywin", 0, 0, s1, gen1, 3));
        const bool rows = strcmp(g_log.order, "SF") == 0 && rd16(party, kMenuRect + 6) == 1064
            && g_log.frame[1] + g_log.frame[3] == 1064 && g_layout_calls == layouts;
        clear_log();
        run(cmd(kOpReset, "partywin", 0, 0, s1, gen1));
        check(rows && strcmp(g_log.order, "SF") == 0 && memcmp(g_log.swap_key, "menu    partywin", 16) == 0
                && rd16(party, kMenuRect + 6) == 1064 && g_log.frame[1] + g_log.frame[3] == 1064
                && ox("partywin") == 1792 && oy("partywin") == 930 && g_layout_calls == layouts,
            "resize partywin 3 and its reset: each swap, then the frame the swap left hung on the bottom 1064;"
            " no layout");

        const void* self = vt_self;
        memcpy(controller("logwindo"), &self, 4);
        memcpy(controller("partywin"), &self, 4);
    }

    // logwindo, logwin2 and partywin hang from their bottom as a class: at
    // exactly their default height their frame is their default rect, which
    // the shape alone reads as a template window's. partywin with six members
    // (34 + 5 x 20 = 134) and the log at 134, each on its default rect
    // 930..1064 at the engine's first touch; equip on its default rect still
    // reads top-anchored
    {
        run(cmd(kOpReset, "logwindo", 0, 0, s1, gen1));
        run(cmd(kOpReset, "partywin", 0, 0, s1, gen1));
        run(cmd(kOpReset, "equip", 0, 0, s1, gen1));
        uint8_t* log = live("logwindo");
        uint8_t* party = live("partywin");
        uint8_t* eq = live("equip");
        wr16(party, kMenuDefault, 1792);
        wr16(party, kMenuDefault + 2, 930);
        wr16(party, kMenuDefault + 4, 1904);
        wr16(party, kMenuDefault + 6, 1064);
        wr16(party, kMenuOrigin, 1792);
        wr16(party, kMenuOrigin + 2, 930);
        set_rect(party, 1792, 930, 1904, 1064);
        set_rect(log, 16, 930, 1790, 1064);
        set_rect(eq, 130, 48, 330, 208);
        const bool unread = g_e.anchor[name("logwindo")] == kAnchorUnknown
            && g_e.anchor[name("partywin")] == kAnchorUnknown && g_e.anchor[name("equip")] == kAnchorUnknown;
        const bool shapes = !hangs_from_bottom(930, 1064, 930, 1064)
            && reads_bottom_anchored("partywin", 930, 1064, 930, 1064)
            && reads_bottom_anchored("logwindo", 930, 1064, 930, 1064)
            && reads_bottom_anchored("logwin2", 728, 894, 728, 894)
            && !reads_bottom_anchored("equip", 48, 208, 48, 208)
            && reads_bottom_anchored("targetwi", 900, 1064, 930, 1064);
        check(unread && shapes,
            "logwindo, logwin2 and partywin read as bottom-anchored on their default rect by class; equip on its"
            " default rect does not; any other window still by its frame's shape");
        const void* driven = vt_layout;
        memcpy(controller("logwindo"), &driven, 4);
        memcpy(controller("partywin"), &driven, 4);
        const LONG layouts = g_layout_calls;
        clear_log();
        run(cmd(kOpMove, "partywin", 1500, 500, s1, gen1));
        check(g_e.anchor[name("partywin")] == kAnchorBottom && strcmp(g_log.order, "F") == 0
                && frame_is(1500, 500, 112, 134) && rect_is(party, 1500, 500, 1612, 634) && ox("partywin") == 1500
                && oy("partywin") == 500 && g_layout_calls == layouts,
            "move partywin 1500,500, its frame exactly its default rect at the first touch (six members): read as"
            " bottom-anchored, SetPosition then SetFrameRect(1500, 500, 112, 134) on the bottom 634; its layout,"
            " drivable, never run");
        clear_log();
        run(cmd(kOpMove, "logwindo", 16, 898, s1, gen1));
        check(g_e.anchor[name("logwindo")] == kAnchorBottom && strcmp(g_log.order, "F") == 0
                && frame_is(16, 898, 1774, 134) && rect_is(log, 16, 898, 1790, 1032) && g_layout_calls == layouts,
            "move logwindo 16,898, a 134-px log on its default rect at the first touch: read as bottom-anchored,"
            " SetFrameRect on the bottom 1032; its layout never run");
        run(cmd(kOpMove, "equip", 140, 60, s1, gen1));
        check(g_e.anchor[name("equip")] == kAnchorTop && rect_is(eq, 140, 60, 340, 220) && g_layout_calls == layouts + 1,
            "move equip, its frame on its default rect: still read as top-anchored, SetPosition and its layout");
        set_rect(party, 1500, 600, 1612, 634);
        clear_log();
        run(cmd(kOpReset, "partywin", 0, 0, s1, gen1));
        check(strcmp(g_log.order, "F") == 0 && frame_is(1792, 1030, 112, 34) && rect_is(party, 1792, 1030, 1904, 1064)
                && ox("partywin") == 1792 && oy("partywin") == 930 && g_layout_calls == layouts + 1,
            "its roster then down to one (frame 600..634): reset puts partywin home on its bottom 1064 at the"
            " 112x34 it has now");
        run(cmd(kOpReset, "logwindo", 0, 0, s1, gen1));
        run(cmd(kOpReset, "equip", 0, 0, s1, gen1));
        const void* self = vt_self;
        memcpy(controller("logwindo"), &self, 4);
        memcpy(controller("partywin"), &self, 4);
    }

    // every open of a window is counted, one a block refuses too; a reply
    // naming its prompt is refused on the game thread once another open has
    // come since
    {
        const int q = name("query");
        uint8_t* qc = controller("query");
        open_now("query");
        const LONG first = g_e.instance[q];
        open_now("query");
        const bool again = g_e.instance[q] == first + 1;
        const int eq = name("equip");
        const LONG equip_before = g_e.instance[eq];
        g_e.holds.set(h1, eq, kHoldBlock, true);
        const bool refused = open_now("equip") == NULL;
        g_e.holds.set(h1, eq, kHoldBlock, false);
        drain(g_e);
        check(first > 0 && again && refused && g_e.instance[eq] == equip_before + 1,
            "every open of a window is counted, an open of one already open and one a block refuses included");
        wr16(qc, kQueryResult, 0);
        Command a = cmd(kOpQueryAnswer, "query", 0, 0, s1, gen1, 7);
        a.verb = kVerbAnswer;
        a.flags = kCmdInstance;
        a.instance = static_cast<uint32_t>(g_e.instance[q]) - 1;
        events(&cursor, ev, 64);
        run(a);
        const int n = events(&cursor, ev, 64);
        const bool gone = rd16(qc, kQueryResult) == 0 && n == 1
            && is_error(ev[0], s1, gen1, "answer", "query", "that prompt is gone");
        a.instance = static_cast<uint32_t>(g_e.instance[q]);
        run(a);
        check(gone && rd16(qc, kQueryResult) == 7 && events(&cursor, ev, 64) == 0,
            "a reply naming an earlier open of query writes nothing and posts 'that prompt is gone' to its handle;"
            " one naming the open that is up now is carried out");
    }

    // ending the incoming post-box session with a box window live: the box's
    // own cancel, request 15 alone, for post1 and post2 alike, never its
    // input; refused while the window is covered or takes no input yet, its
    // busy byte no bar; the game's reply and tick then close it. With none
    // live, the window-less close
    {
        uint8_t* pc = controller("post1");
        const void* box = vt_box;
        const void* self = vt_self;
        memcpy(pc, &box, 4);
        wr32(pc, kPostState, 3);
        uint8_t* p1 = open_now("post1");
        wr16(p1, kMenuCursor, 2);
        clear_log();
        g_log.post_queues = true;
        LONG errors = g_e.drain_errors;
        run(cmd(kOpPostEnd, "post1", 0, 0, s1, gen1));
        const bool own = strcmp(g_log.order, "B") == 0 && g_log.post_command == 0x0F && rd32(pc, kPostState) == 3
            && g_e.drain_errors == errors;
        clear_log();
        run(cmd(kOpPostEnd, "post2", 0, 0, s1, gen1));
        const bool through_post1 = strcmp(g_log.order, "B") == 0 && g_log.post_command == 0x0F
            && rd32(pc, kPostState) == 3;
        check(own && through_post1 && g_log.box_event == 0,
            "end the incoming session with post1 live: request 15 alone, the state left at 3, post1's input never"
            " called; the same for post2");
        p1[kMenuBusy] = 1;
        clear_log();
        run(cmd(kOpPostEnd, "post1", 0, 0, s1, gen1));
        const bool busy_passes = strcmp(g_log.order, "B") == 0 && g_e.drain_errors == errors;
        p1[kMenuBusy] = 0;
        g_log.post_queues = false;
        clear_log();
        run(cmd(kOpPostEnd, "post1", 0, 0, s1, gen1));
        g_log.post_queues = true;
        check(busy_passes && strcmp(g_log.order, "B") == 0 && rd32(pc, kPostState) == 3
                && g_e.drain_errors == errors + 1 && strstr(g_e.error, "could not queue"),
            "the busy byte does not hold a cancel back; a request the game cannot queue: an error, nothing written");
        wr16(p1, kMenuInputWait, 2);
        clear_log();
        errors = g_e.drain_errors;
        run(cmd(kOpPostEnd, "post1", 0, 0, s1, gen1));
        const bool waiting = g_log.count == 0 && g_e.drain_errors == errors + 1
            && strstr(g_e.error, "the incoming box takes no input yet");
        wr16(p1, kMenuInputWait, 0);
        wr32(p1, kMenuState, kStateDormant);
        run(cmd(kOpPostEnd, "post1", 0, 0, s1, gen1));
        const bool covered = g_log.count == 0 && g_e.drain_errors == errors + 2
            && strstr(g_e.error, "the incoming box is under another window");
        wr32(p1, kMenuState, 0);
        check(waiting && covered && rd32(pc, kPostState) == 3,
            "end the incoming session while post1 takes no input yet (+0x56 above 0) or lies covered (+0x10 0xE):"
            " refused, nothing sent");
        // the server's reply: the 0x04B handler's request-close marks the
        // session 0x12; post1's next tick closes the family by instance
        events(&cursor, ev, 64);
        fake_post_request_close(pc, NULL);
        game_closes("post1");
        wr32(pc, kPostState, 0);
        drain(g_e);
        int n = events(&cursor, ev, 64);
        bool closed = false;
        for (int i = 0; i < n; ++i) {
            closed = closed || is_event(ev[i], kEvClosed, "post1");
        }
        check(closed && !live("post1"), "the game's teardown after the reply posts closed{post1}");
        wr32(pc, kPostState, 3);
        open_now("post1");
        run(cmd(kOpClose, "post1", 0, 0, s1, gen1));
        clear_log();
        run(cmd(kOpPostEnd, "post1", 0, 0, s1, gen1));
        check(strcmp(g_log.order, "BY") == 0 && g_log.post_command == 0x0F && rd32(pc, kPostState) == kPostClosing,
            "with no box window live: request 15, then post1's request-close sets 0x12, as before");
        wr32(pc, kPostState, 3);
        open_now("post2");
        clear_log();
        errors = g_e.drain_errors;
        run(cmd(kOpPostEnd, "post1", 0, 0, s1, gen1));
        check(g_log.count == 0 && rd32(pc, kPostState) == 3 && g_e.drain_errors == errors + 1
                && strstr(g_e.error, "incoming box is open"),
            "with post2 live and post1 not: nothing is sent and nothing set, and a drain error says why");
        run(cmd(kOpClose, "post2", 0, 0, s1, gen1));
        wr32(pc, kPostState, 0);
        memcpy(pc, &self, 4);
    }

    // a group move whose anchor is closed waits for the game to open it: the
    // anchor from where the game puts it to x,y, and every open window it
    // carries by as much; a later move or reset of the anchor, the reset_all
    // of the handle that made it and another move of the group replace it
    {
        run(cmd(kOpReset, "partywin", 0, 0, s1, gen1));
        run(cmd(kOpReset, "targetwi", 0, 0, s1, gen1));
        run(cmd(kOpReset, "subwindo", 0, 0, s1, gen1));
        open_now("targetwi");
        open_now("subwindo");
        run(cmd(kOpClose, "partywin", 0, 0, s1, gen1));
        const int tx = ox("targetwi");
        const int ty = oy("targetwi");
        const int sx = ox("subwindo");
        const int sy = oy("subwindo");
        Command wait = group_to_cmd(1, 1700, 800, s2, gen2);
        wait.flags = kCmdWait;
        const LONG errors = g_e.drain_errors;
        events(&cursor, ev, 64);
        run(wait);
        const GroupWait& w = g_e.waiting.g[1];
        check(!live("partywin") && g_e.waiting.owned_by(1, s2, gen2) && w.x == 1700 && w.y == 800
                && g_e.drain_errors == errors && events(&cursor, ev, 64) == 0 && ox("targetwi") == tx
                && !g_e.memory.e[name("partywin")].active && !g_e.memory.e[name("targetwi")].active,
            "move_group party_list to 1700,800 with partywin closed, as one that may wait: kept for partywin's open,"
            " owned by its mover; nothing moved, nothing remembered, no error");
        open_now("partywin");
        const int dx = 1700 - 500;
        const int dy = 800 - 380;
        const MemEntry& pm = g_e.memory.e[name("partywin")];
        const MemEntry& tm = g_e.memory.e[name("targetwi")];
        check(ox("partywin") == 1700 && oy("partywin") == 800 && ox("targetwi") == tx + dx && oy("targetwi") == ty + dy
                && ox("subwindo") == sx + dx && oy("subwindo") == sy + dy && !w.active
                && (rd32(live("targetwi"), kMenuPolicy) & 0x2000) && pm.active && pm.x == 1700 && pm.y == 800
                && pm.group == 2 && tm.group == 2 && g_e.memory.owned_by(name("targetwi"), s2, gen2)
                && g_e.memory.owned_by(name("subwindo"), s2, gen2),
            "the game opens partywin: from where the game put it, 500,380, to 1700,800; targetwi and subwindo, open"
            " and docked, carried by the same 1200,420; all three remembered as the party_list group's move, by its"
            " mover; the waiting move gone");

        run(cmd(kOpReset, "partywin", 0, 0, s1, gen1));
        run(cmd(kOpReset, "targetwi", 0, 0, s1, gen1));
        run(cmd(kOpReset, "subwindo", 0, 0, s1, gen1));
        run(cmd(kOpClose, "partywin", 0, 0, s1, gen1));
        run(cmd(kOpClose, "subwindo", 0, 0, s1, gen1));
        run(wait);
        open_now("partywin");
        check(ox("partywin") == 1700 && oy("partywin") == 800 && !live("subwindo")
                && !g_e.memory.e[name("subwindo")].active,
            "a window the group carries that is closed at the anchor's open is not moved or remembered: the game"
            " docks it when it opens");

        run(cmd(kOpReset, "partywin", 0, 0, s1, gen1));
        run(cmd(kOpReset, "targetwi", 0, 0, s1, gen1));
        run(cmd(kOpClose, "partywin", 0, 0, s1, gen1));
        run(wait);
        run(cmd(kOpMove, "partywin", 600, 300, s1, gen1));
        const bool moved_over = !w.active;
        const int ty_now = oy("targetwi");
        open_now("partywin");
        const bool moved = ox("partywin") == 600 && oy("partywin") == 300 && oy("targetwi") == ty_now
            && g_e.memory.e[name("partywin")].group == 0;
        run(cmd(kOpClose, "partywin", 0, 0, s1, gen1));
        run(wait);
        run(cmd(kOpReset, "partywin", 0, 0, s1, gen1));
        const bool reset_over = !w.active;
        run(wait);
        run(cmd(kOpResetOwned, NULL, 0, 0, s1, gen1));
        const bool others_kept = g_e.waiting.owned_by(1, s2, gen2);
        run(cmd(kOpResetOwned, NULL, 0, 0, s2, gen2));
        const bool owner_reset = !w.active;
        run(wait);
        Command other = group_to_cmd(1, 100, 120, s1, gen1);
        other.flags = kCmdWait;
        run(other);
        check(moved_over && moved && reset_over && others_kept && owner_reset && g_e.waiting.owned_by(1, s1, gen1)
                && w.x == 100 && w.y == 120,
            "a move of partywin replaces the group move waiting for it (partywin opens at the move's 600,300, nothing"
            " carried), and so does its reset; reset_all drops it for the handle that made it and no other; a later"
            " move of the group replaces it");
        Command plain = group_to_cmd(1, 50, 60, s1, gen1);
        const LONG before_plain = g_e.drain_errors;
        run(plain);
        check(g_e.waiting.owned_by(1, s1, gen1) && w.x == 100 && g_e.drain_errors == before_plain + 1
                && strstr(g_e.error, "its anchor partywin is not open"),
            "a group move that may not wait (engine abi 3's) is refused with the anchor closed, as before, and"
            " leaves the waiting one alone");
        open_now("partywin");
        check(ox("partywin") == 100 && oy("partywin") == 120 && !w.active, "and partywin opens at the waiting 100,120");
        open_now("subwindo");
    }

    // the drain watches what pending() reads, the invite flag and each box's
    // session, and posts a pending event at each change
    {
        uint8_t* dc = controller("delivery");
        uint8_t* pc = controller("post1");
        g_log.party_pending = 0;
        wr16(dc, kDeliveryState, 0);
        wr32(pc, kPostState, 0);
        drain(g_e);
        events(&cursor, ev, 64);
        g_log.party_pending = 1;
        drain(g_e);
        int n = events(&cursor, ev, 64);
        const bool invite = n == 1 && is_pending(ev[0], kPendingInvite, true);
        drain(g_e);
        const bool once = events(&cursor, ev, 64) == 0;
        wr16(dc, kDeliveryState, 5);
        drain(g_e);
        n = events(&cursor, ev, 64);
        const bool post = n == 1 && is_pending(ev[0], kPendingPost, true);
        wr32(pc, kPostState, 3);
        drain(g_e);
        const bool both = events(&cursor, ev, 64) == 0;
        wr16(dc, kDeliveryState, 0);
        drain(g_e);
        const bool incoming = events(&cursor, ev, 64) == 0;
        wr32(pc, kPostState, 0);
        g_log.party_pending = 0;
        drain(g_e);
        n = events(&cursor, ev, 64);
        check(invite && once && post && both && incoming && n == 2 && is_pending(ev[0], kPendingInvite, false)
                && is_pending(ev[1], kPendingPost, false),
            "pending events: the invite flag set, posted once; a session in the outgoing box; none while the"
            " incoming one opens and the outgoing one ends, a session being up throughout; both clearing, one each");
    }

    // close by instance as the game runs it: the marks at once, the instance
    // freed by the update loop frames later. Open is a live instance with
    // neither mark, so from the closed event on the verbs on the game thread
    // treat the window as closed, and a covered window closed by instance
    // (marked pending destroy, no staged close) is noticed at its marks
    {
        static uint8_t bare[kMenuBytes];
        memset(bare, 0, sizeof(bare));
        const bool shown = menu_is_open(bare);
        wr32(bare, kMenuState, kStateDormant);
        const bool dormant = menu_is_open(bare);
        bare[kMenuClosing] = 1;
        const bool closing = !menu_is_open(bare);
        bare[kMenuClosing] = 0;
        bare[kMenuPendingDestroy] = 1;
        const bool pending = !menu_is_open(bare);
        check(!menu_is_open(NULL) && shown && dormant && closing && pending,
            "menu_is_open: a live instance, dormant or not, while +0x6E (closing) and +0x70 (pending destroy) are"
            " both 0");

        g_close_keeps = true;
        const uint8_t allows = g_query_cancel_allowed;
        const int q = name("query");
        uint8_t* qc = controller("query");
        uint8_t* qm = live("query") ? live("query") : open_now("query");
        wr16(qc, kQueryResult, 0);
        events(&cursor, ev, 64);
        game_closes("query");
        int n = events(&cursor, ev, 64);
        check(n == 1 && is_event(ev[0], kEvClosed, "query") && live("query") == qm && qm[kMenuClosing] == 1
                && qm[kMenuPendingDestroy] == 0 && !open_menu(g_e, q, NULL),
            "the game closes query by instance: closed{query} from its staged close; the instance stays bound,"
            " marked closing, and is not open");
        clear_log();
        g_query_cancel_allowed = 1;
        LONG errors = g_e.drain_errors;
        run(cmd(kOpQueryAnswer, "query", 0, 0, s1, gen1, 1));
        const bool answer_refused = g_e.drain_errors == errors + 1 && strstr(g_e.error, "query is not open")
            && rd16(qc, kQueryResult) == 0;
        errors = g_e.drain_errors;
        run(cmd(kOpQueryCancel, "query", 0, 0, s1, gen1));
        check(answer_refused && g_e.drain_errors == errors + 1 && strstr(g_e.error, "query is not open")
                && g_log.query_inputs == 0 && rd16(qc, kQueryResult) == 0,
            "query_answer and query_cancel on the closing query: drain errors, query is not open; no word written,"
            " no input");
        events(&cursor, ev, 64);
        for (int i = 0; i < 4; ++i) {
            drain(g_e);
        }
        const bool quiet = events(&cursor, ev, 64) == 0 && live("query") == qm;
        destroy("query");
        drain(g_e);
        const bool freed_quiet = events(&cursor, ev, 64) == 0 && !live("query");
        errors = g_e.drain_errors;
        run(cmd(kOpQueryAnswer, "query", 0, 0, s1, gen1, 1));
        check(quiet && freed_quiet && g_e.drain_errors == errors + 1 && rd16(qc, kQueryResult) == 0,
            "frames pass with nothing posted while the instance lives; once it is freed, nothing more is posted and"
            " query is still not open");
        g_query_cancel_allowed = allows;
        qm = open_now("query");
        game_closes("query");
        events(&cursor, ev, 64);
        uint8_t* again = open_now("query");
        n = events(&cursor, ev, 64);
        errors = g_e.drain_errors;
        run(cmd(kOpQueryAnswer, "query", 0, 0, s1, gen1, 4));
        check(again == qm && n == 1 && is_event(ev[0], kEvOpened, "query") && open_menu(g_e, q, NULL) == qm
                && g_e.drain_errors == errors && rd16(qc, kQueryResult) == 4,
            "the game opens query again before the closed instance is freed: the open reuses it with both marks"
            " cleared, opened{query}, and it is open and answered");

        const int eqn = name("equip");
        uint8_t* eq = live("equip") ? live("equip") : open_now("equip");
        events(&cursor, ev, 64);
        cover(eq);
        n = events(&cursor, ev, 64);
        drain(g_e);
        const bool covered_open = n == 1 && is_event(ev[0], kEvCovered, "equip") && open_menu(g_e, eqn, NULL) == eq
            && events(&cursor, ev, 64) == 0;
        game_closes("equip");
        const bool marked = events(&cursor, ev, 64) == 0 && eq[kMenuClosing] == 1 && eq[kMenuPendingDestroy] == 1
            && live("equip") == eq;
        drain(g_e);
        n = events(&cursor, ev, 64);
        const bool noticed = n == 1 && is_event(ev[0], kEvClosed, "equip") && !g_e.covered[eqn] && live("equip") == eq
            && !open_menu(g_e, eqn, NULL);
        drain(g_e);
        drain(g_e);
        const bool once = events(&cursor, ev, 64) == 0;
        destroy("equip");
        drain(g_e);
        check(covered_open && marked && noticed && once && events(&cursor, ev, 64) == 0,
            "a covered window, dormant with the marks clear, is open; the game's close of it marks it closing and"
            " pending destroy with no staged close, and the next drain posts closed{equip} while the instance lives,"
            " once; nothing when it is freed");

        uint8_t* pw = live("partywin") ? live("partywin") : open_now("partywin");
        if (!live("targetwi")) {
            open_now("targetwi");
        }
        if (!live("subwindo")) {
            open_now("subwindo");
        }
        run(cmd(kOpReset, "subwindo", 0, 0, s1, gen1));
        game_closes("subwindo");
        const int px = ox("partywin");
        const int tx = ox("targetwi");
        const int sx = ox("subwindo");
        run(group_cmd(1, 20, 0, s2, gen2));
        check(ox("partywin") == px + 20 && ox("targetwi") == tx + 20 && ox("subwindo") == sx
                && !g_e.memory.e[name("subwindo")].active,
            "move_group party_list with subwindo closing: partywin and targetwi move, subwindo is neither moved nor"
            " remembered");
        destroy("subwindo");
        game_closes("partywin");
        errors = g_e.drain_errors;
        run(group_to_cmd(1, 700, 400, s2, gen2));
        const bool refused = g_e.drain_errors == errors + 1 && strstr(g_e.error, "its anchor partywin is not open")
            && ox("partywin") == px + 20;
        Command wait = group_to_cmd(1, 700, 400, s2, gen2);
        wait.flags = kCmdWait;
        run(wait);
        const bool waits = g_e.waiting.owned_by(1, s2, gen2) && g_e.waiting.g[1].x == 700 && live("partywin") == pw;
        destroy("partywin");
        open_now("partywin");
        check(refused && waits && ox("partywin") == 700 && oy("partywin") == 400 && !g_e.waiting.g[1].active,
            "with partywin closing, a group move that may not wait is refused (its anchor is not open) and one that"
            " may waits; partywin's next open takes it");

        uint8_t* pc = controller("post1");
        const void* box = vt_box;
        const void* self = vt_self;
        memcpy(pc, &box, 4);
        wr32(pc, kPostState, 3);
        if (!live("post1")) {
            open_now("post1");
        }
        game_closes("post1");
        clear_log();
        g_log.post_queues = true;
        errors = g_e.drain_errors;
        run(cmd(kOpPostEnd, "post1", 0, 0, s1, gen1));
        const bool kept_off = g_log.count == 0 && rd32(pc, kPostState) == 3 && g_e.drain_errors == errors + 1
            && strstr(g_e.error, "incoming box is closing");
        destroy("post1");
        clear_log();
        run(cmd(kOpPostEnd, "post1", 0, 0, s1, gen1));
        check(kept_off && strcmp(g_log.order, "BY") == 0 && rd32(pc, kPostState) == kPostClosing,
            "with post1 closing: no request, and its close state is not set while the instance lives (a drain"
            " error says the box is closing); once freed, request 15 then post1's request-close");
        wr32(pc, kPostState, 0);
        memcpy(pc, &self, 4);
        drain(g_e);
        events(&cursor, ev, 64);
        g_close_keeps = false;
    }

    // 0.7.0: a move by the frame's top-left. logwindo at rest as measured
    // (origin 16,930, default rect 16,930..382,1064, frame 898..1064, 166
    // tall): its own frame top is a move that changes nothing, and a top
    // 100 up puts its origin 100 up; at its next open, built at its template
    // size, the frame top goes back where it was asked to be, and with a
    // remembered size the size goes on first
    {
        run(cmd(kOpReset, "logwindo", 0, 0, s1, gen1));
        run(cmd(kOpReset, "equip", 0, 0, s1, gen1));
        uint8_t* log = live("logwindo") ? live("logwindo") : open_now("logwindo");
        wr16(log, kMenuOrigin, 16);
        wr16(log, kMenuOrigin + 2, 930);
        set_rect(log, 16, 898, 1790, 1064);
        Command m = cmd(kOpMove, "logwindo", 16, 898, s1, gen1);
        m.flags = kCmdFrame;
        run(m);
        const bool still = ox("logwindo") == 16 && oy("logwindo") == 930 && rect_is(log, 16, 898, 1790, 1064);
        m.y = 798;
        run(m);
        const MemEntry& lm = g_e.memory.e[name("logwindo")];
        check(still && ox("logwindo") == 16 && oy("logwindo") == 830 && rect_is(log, 16, 798, 1790, 964) && lm.active
                && lm.x == 16 && lm.y == 798 && lm.by_frame,
            "move logwindo by its frame's top-left: to its own 16,898 nothing changes; to 16,798 its origin goes to"
            " 830, frame 798..964; remembered as given, 16,798, by the frame");
        run(cmd(kOpClose, "logwindo", 0, 0, s1, gen1));
        log = open_now("logwindo");
        check(ox("logwindo") == 16 && oy("logwindo") == 798 && rect_is(log, 16, 798, 382, 932),
            "logwindo reopened at its template's 134 rows of height: origin 798, so its frame top is 798 again");
        run(cmd(kOpResizeRect, "logwindo", 1000, 120, s1, gen1));
        run(cmd(kOpClose, "logwindo", 0, 0, s1, gen1));
        clear_log();
        log = open_now("logwindo");
        check(strcmp(g_log.order, "FF") == 0 && oy("logwindo") == 784 && rect_is(log, 16, 798, 1016, 918),
            "reopened with a remembered 1000x120: the size first, then the origin that puts that frame's top at 798"
            " (784 = 798 + 120 - 134)");
        run(cmd(kOpReset, "logwindo", 0, 0, s1, gen1));
        uint8_t* eq = live("equip") ? live("equip") : open_now("equip");
        Command e = cmd(kOpMove, "equip", 140, 60, s1, gen1);
        e.flags = kCmdFrame;
        run(e);
        check(ox("equip") == 140 && oy("equip") == 60 && rect_is(eq, 140, 60, 340, 220),
            "move equip by its frame's top-left: a top-anchored window's frame top-left is its origin");
        run(cmd(kOpReset, "equip", 0, 0, s1, gen1));
    }

    // 0.7.0: move_group by the anchor frame's top-left, and the resets of what
    // one handle placed: another's reset is refused on the game thread whole,
    // each aspect resets alone, a group's reset goes to its anchor and what
    // its move carried, and a waiting move goes with its owner's reset alone
    {
        run(cmd(kOpReset, "partywin", 0, 0, s1, gen1));
        uint8_t* log = live("logwindo");
        if (!live("partywin")) {
            open_now("partywin");
        }
        set_rect(log, 16, 898, 1790, 1064);
        const int py = oy("partywin");
        Command g = group_to_cmd(0, 16, 798, s2, gen2);
        g.flags = kCmdFrame;
        run(g);
        const MemEntry& lm = g_e.memory.e[name("logwindo")];
        const MemEntry& pm = g_e.memory.e[name("partywin")];
        check(oy("logwindo") == 830 && rect_is(log, 16, 798, 1790, 964) && oy("partywin") == py - 100 && lm.by_frame
                && lm.y == 798 && lm.group == 1 && !pm.by_frame && pm.y == py - 100 && pm.group == 1,
            "move_group chat_log by its anchor frame's top-left 16,798: logwindo's origin to 830, partywin carried by"
            " the same -100; logwindo remembered by its frame, partywin by its origin");
        Command theirs = cmd(kOpResetGroupMine, NULL, 0, 0, s1, gen1);
        theirs.target = 0;
        theirs.verb = kVerbResetGroup;
        events(&cursor, ev, 64);
        run(theirs);
        int n = events(&cursor, ev, 64);
        check(oy("logwindo") == 830 && oy("partywin") == py - 100 && n == 1
                && is_error(ev[0], s1, gen1, "reset_group", "chat_log", "chat_log was placed by another handle since"),
            "reset_group chat_log by a handle that did not move it: nothing reset, an error to it");
        Command mine = theirs;
        mine.slot = s2;
        mine.gen = gen2;
        run(mine);
        check(oy("logwindo") == 930 && rect_is(log, 16, 898, 1790, 1064) && oy("partywin") == py && !lm.active
                && !pm.active && events(&cursor, ev, 64) == 0,
            "reset_group chat_log by its mover: logwindo home on its bottom, partywin back, both forgotten");

        run(cmd(kOpReset, "equip", 0, 0, s1, gen1));
        uint8_t* eq = live("equip") ? live("equip") : open_now("equip");
        run(cmd(kOpMove, "equip", 300, 200, s1, gen1));
        Command r = cmd(kOpResetMine, "equip", 0, 0, s2, gen2, kAspectPosition | kAspectSize);
        r.verb = kVerbReset;
        events(&cursor, ev, 64);
        run(r);
        n = events(&cursor, ev, 64);
        check(ox("equip") == 300 && g_e.memory.owned_by(name("equip"), s1, gen1) && n == 1
                && is_error(ev[0], s2, gen2, "reset", "equip", "equip was placed by another handle since"),
            "reset of equip by a handle that did not place it: refused whole on the game thread, an error to it");
        run(cmd(kOpResizeRect, "equip", 250, 150, s1, gen1));
        run(cmd(kOpMove, "equip", 111, 222, s2, gen2));
        Command size = cmd(kOpResetMine, "equip", 0, 0, s1, gen1, kAspectSize);
        size.verb = kVerbReset;
        run(size);
        const bool sized_back = !g_e.memory.e[name("equip")].size_kind && rect_is(eq, 111, 222, 311, 382)
            && g_e.memory.owned_by(name("equip"), s2, gen2);
        Command position = cmd(kOpResetMine, "equip", 0, 0, s1, gen1, kAspectPosition);
        position.verb = kVerbReset;
        events(&cursor, ev, 64);
        run(position);
        n = events(&cursor, ev, 64);
        check(sized_back && ox("equip") == 111 && n == 1 && is_error(ev[0], s1, gen1, "reset", "equip", "placed by"),
            "s1 resized equip, s2 moved it: s1's reset of the size puts its own 200x160 back where s2 put it; s1's"
            " reset of the position is refused");
        position.slot = s2;
        position.gen = gen2;
        run(position);
        const int calls = g_setpos_calls;
        run(position);
        check(ox("equip") == 130 && oy("equip") == 48 && !g_e.memory.e[name("equip")].active
                && g_setpos_calls == calls && events(&cursor, ev, 64) == 0,
            "s2's reset of the position puts equip home; another reset with nothing placed is no change and no error");

        run(cmd(kOpClose, "partywin", 0, 0, s1, gen1));
        Command wait = group_to_cmd(1, 1700, 800, s2, gen2);
        wait.flags = kCmdWait;
        run(wait);
        Command other = cmd(kOpResetMine, "partywin", 0, 0, s1, gen1, kAspectPosition);
        other.verb = kVerbReset;
        run(other);
        const bool kept = g_e.waiting.owned_by(1, s2, gen2);
        other.slot = s2;
        other.gen = gen2;
        run(other);
        check(kept && !g_e.waiting.g[1].active,
            "a reset of partywin keeps another handle's group move waiting for it; its owner's reset drops it");
    }

    // 0.7.0: a group move waiting for its closed anchor, replaced by another
    // handle's, posts that handle an error naming the one that replaced it;
    // one by the frame's top-left converts at the anchor's open, after a
    // remembered size
    {
        events(&cursor, ev, 64);
        Command w2 = group_to_cmd(1, 1700, 800, s2, gen2);
        w2.flags = kCmdWait | kCmdFrame;
        run(w2);
        run(w2);
        const bool quiet = events(&cursor, ev, 64) == 0;
        Command w1 = group_to_cmd(1, 600, 300, s1, gen1);
        w1.flags = kCmdWait;
        run(w1);
        int n = events(&cursor, ev, 64);
        ErrorRecord r;
        const bool named = n == 1 && g_e.errors.read(static_cast<uint32_t>(event_name(ev[0])), &r) && r.by_slot == s1
            && r.by_gen == gen1;
        check(quiet && named && is_error(ev[0], s2, gen2, "move_group", "party_list", "replaced by another handle's move")
                && g_e.waiting.owned_by(1, s1, gen1),
            "s2's group move waiting for partywin, again by s2 (no error), then replaced by s1's: one error event to s2,"
            " its record naming s1 as the handle that replaced it");
        run(w2);
        n = events(&cursor, ev, 64);
        check(n == 1 && is_error(ev[0], s1, gen1, "move_group", "party_list", "replaced by"),
            "and s2's move replacing s1's tells s1");
        run(cmd(kOpResizeRect, "partywin", 120, 100, s2, gen2));
        uint8_t* party = open_now("partywin");
        const MemEntry& pm = g_e.memory.e[name("partywin")];
        check(ox("partywin") == 1700 && oy("partywin") == 840 && rect_is(party, 1700, 800, 1820, 900) && pm.by_frame
                && pm.x == 1700 && pm.y == 800 && !g_e.waiting.g[1].active,
            "partywin opens with a remembered 120x100: the size first, then the waiting move puts its frame top at 800"
            " (origin 840 = 800 + 100 - 60); remembered as given, by the frame");
        run(cmd(kOpReset, "partywin", 0, 0, s1, gen1));
    }

    // 0.7.0: the invite and post-box sessions, counted as the drain sees each
    // appear; the id of one up that the drain has not seen yet is the next,
    // the drain watches before its commands, and a reply naming another
    // session is refused
    {
        g_log.party_pending = 0;
        drain(g_e);
        events(&cursor, ev, 64);
        const uint32_t before = current_session(g_e, kSessionInvite);
        g_log.party_pending = 1;
        const uint32_t unseen = current_session(g_e, kSessionInvite);
        drain(g_e);
        const uint32_t seen = current_session(g_e, kSessionInvite);
        events(&cursor, ev, 64);
        check(unseen == before + 1 && seen == unseen && session_seen(g_e.invite_watch)
                && session_count(g_e.invite_watch) == seen,
            "an invite's id: the next count while the drain has not seen it, the same once it has");
        Command a = cmd(kOpPartyReply, "prtyjoin", 0, 0, s1, gen1, 1);
        a.verb = kVerbAnswer;
        a.flags = kCmdInstance | kCmdSession;
        a.instance = before;
        clear_log();
        g_log.party_queues = true;
        run(a);
        int n = events(&cursor, ev, 64);
        const bool gone = g_log.count == 0 && g_log.party_pending == 1 && n == 1
            && is_error(ev[0], s1, gen1, "answer", "prtyjoin", "that prompt is gone");
        a.instance = seen;
        run(a);
        check(gone && strcmp(g_log.order, "PX") == 0 && g_log.party_pending == 0,
            "a reply naming the invite before is refused, that prompt is gone; one naming this one is sent");
        drain(g_e);
        g_log.party_pending = 1;
        const uint32_t next = current_session(g_e, kSessionInvite);
        a.instance = next;
        clear_log();
        run(a);
        check(next == seen + 1 && strcmp(g_log.order, "PX") == 0,
            "a reply naming an invite the drain had not seen yet: the drain numbers it before its commands, and sends"
            " it");
        drain(g_e);
        uint8_t* dc = controller("delivery");
        const uint32_t post = current_session(g_e, kSessionPost);
        wr16(dc, kDeliveryState, 5);
        const uint32_t up = current_session(g_e, kSessionPost);
        drain(g_e);
        Command c = cmd(kOpPostClose, "delivery", 0, 0, s1, gen1);
        c.verb = kVerbCancel;
        c.flags = kCmdInstance | kCmdSession;
        c.instance = post;
        clear_log();
        events(&cursor, ev, 64);
        run(c);
        n = events(&cursor, ev, 64);
        check(up == post + 1 && g_log.count == 0 && n == 1
                && is_error(ev[0], s1, gen1, "cancel", "delivery", "that prompt is gone") && rd16(dc, kDeliveryState) == 5,
            "a post-box session is counted the same way, and a cancel naming the one before is refused");
        wr16(dc, kDeliveryState, 0);
        drain(g_e);
        events(&cursor, ev, 64);
        check(session_window("prtyjoin") == kSessionInvite && session_window("post2") == kSessionPost
                && session_window("delivery") == kSessionPost && session_window("query") == -1,
            "prtyjoin's replies name the invite, the three boxes' the post-box session, any other window none");
    }

    // 0.7.0: a cursor event when an open window's row moves; a window is read
    // first, posting nothing, when it is seen open, and forgotten closed
    {
        const uint32_t clamped = pack_cursor(383, -3);
        const uint32_t high = pack_cursor(5, 70000);
        check(event_type(clamped) == kEvCursor && cursor_name(clamped) == 383 && cursor_row(clamped) == 0
                && cursor_name(high) == 5 && cursor_row(high) == 0x7FFF && cursor_row(pack_cursor(7, 12)) == 12,
            "a cursor event packs the window and the row, the row clamped to 0..0x7FFF");
        uint8_t* eq = live("equip") ? live("equip") : open_now("equip");
        drain(g_e);
        events(&cursor, ev, 64);
        wr16(eq, kMenuCursor, 3);
        drain(g_e);
        int n = events(&cursor, ev, 64);
        const bool moved = n == 1 && event_type(ev[0]) == kEvCursor && cursor_name(ev[0]) == name("equip")
            && cursor_row(ev[0]) == 3;
        drain(g_e);
        const bool once = events(&cursor, ev, 64) == 0;
        run(cmd(kOpClose, "equip", 0, 0, s1, gen1));
        uint8_t* again = open_now("equip");
        wr16(again, kMenuCursor, 5);
        drain(g_e);
        n = events(&cursor, ev, 64);
        bool cursor_seen = false;
        for (int i = 0; i < n; ++i) {
            cursor_seen = cursor_seen || event_type(ev[i]) == kEvCursor;
        }
        wr16(again, kMenuCursor, 2);
        drain(g_e);
        const int m = events(&cursor, ev, 64);
        check(moved && once && !cursor_seen && m == 1 && cursor_row(ev[0]) == 2,
            "equip's row 0 to 3: one cursor{equip, 3}, once; reopened at row 5 nothing (read first), then 5 to 2:"
            " cursor{equip, 2}");
    }

    // 0.7.2: query's cursor event carries the option under its cursor (+0x30),
    // 1-based in its list, which Down moves past the three rows shown while
    // the row (+0x4C) stays; the row alone moving posts nothing for query, a
    // cursor on no option is not read, and any other window still posts its row
    {
        uint8_t* q = open_now("query");
        uint8_t* qc = controller("query");
        static uint8_t nodes[4][0x18];
        static uint8_t items[4][kOptionBytes];
        memset(nodes, 0, sizeof(nodes));
        memset(items, 0, sizeof(items));
        for (int i = 0; i < 4; ++i) {
            wr32(nodes[i], 0, i + 1 < 4 ? reinterpret_cast<uint32_t>(nodes[i + 1]) : 0);
            wr32(nodes[i], 0x10, reinterpret_cast<uint32_t>(items[i]));
            wr16(items[i], kOptionValue, 21 + i);
        }
        wr32(qc, kQueryOptions, reinterpret_cast<uint32_t>(nodes[0]));
        wr32(qc, kQueryCount, 4);
        wr16(qc, kQueryCursor, 0);
        wr16(qc, kQueryTop, 0);
        wr16(qc, kQueryResult, 0);
        wr16(q, kMenuCursor, 1);
        drain(g_e);
        events(&cursor, ev, 64);
        int option = 0;
        int top = 0;
        const bool first = query_position(qc, &option, &top) && option == 1 && top == 1;
        char seen[96] = "";
        char rows[32] = "";
        for (int i = 0; i < 3; ++i) {
            query_down(qc);
            drain(g_e);
            const int n = events(&cursor, ev, 64);
            option = top = 0;
            query_position(qc, &option, &top);
            const size_t used = strlen(seen);
            snprintf(seen + used, sizeof(seen) - used, "%s%d/%d/", i ? " " : "", option, top);
            for (int k = 0; k < n; ++k) {
                const size_t at = strlen(seen);
                snprintf(seen + at, sizeof(seen) - at, "%s%d:%d", k ? "," : "",
                    event_type(ev[k]) == kEvCursor && cursor_name(ev[k]) == name("query") ? 1 : 0, cursor_row(ev[k]));
            }
            const size_t r = strlen(rows);
            snprintf(rows + r, sizeof(rows) - r, "%s%d", i ? " " : "", rd16(q, kMenuCursor));
        }
        check(first && strcmp(seen, "2/1/1:2 3/1/1:3 4/2/1:4") == 0 && strcmp(rows, "2 3 3") == 0,
            "a four-option query, Down three times: the option under the cursor 2, 3, 4 and the first shown 1, 1, 2"
            " while the row goes 2, 3, 3; one cursor{query} each, with the option");
        check(query_confirm(qc) && rd16(qc, kQueryResult) == 24,
            "the game's confirm, reading +0x30, takes the fourth option's value: the one the events named");
        wr16(q, kMenuCursor, 1);
        drain(g_e);
        const bool row_alone = events(&cursor, ev, 64) == 0;
        wr16(qc, kQueryCursor, 4);
        drain(g_e);
        const bool past = events(&cursor, ev, 64) == 0 && !query_position(qc, &option, &top);
        wr32(qc, kQueryCount, 0);
        wr16(qc, kQueryCursor, 0);
        wr16(qc, kQueryTop, 0);
        drain(g_e);
        const bool empty = events(&cursor, ev, 64) == 0 && !query_position(qc, &option, &top);
        wr32(qc, kQueryCount, 4);
        drain(g_e);
        int n = events(&cursor, ev, 64);
        const bool back = n == 1 && event_type(ev[0]) == kEvCursor && cursor_name(ev[0]) == name("query")
            && cursor_row(ev[0]) == 1;
        check(row_alone && past && empty && back,
            "query's row alone moving posts nothing; a cursor past the list or a list of none is not read; the"
            " cursor back on the first option posts cursor{query, 1}");

        // 0.7.4: an answer writes what the game's confirm writes: the
        // option's index at +0x30 and its value; the first option shown
        // moves as the cursor setter moves it, the row (+0x4C) is left alone
        clear_log();
        const int row_before = rd16(q, kMenuCursor);
        Command pick = cmd(kOpQueryAnswer, "query", 0, 0, s1, gen1, 24);
        pick.flags = kCmdListed;
        run(pick);
        n = events(&cursor, ev, 64);
        const bool fourth = rd16(qc, kQueryCursor) == 3 && rd16(qc, kQueryTop) == 1 && rd16(qc, kQueryResult) == 24
            && n == 1 && event_type(ev[0]) == kEvCursor && cursor_name(ev[0]) == name("query") && cursor_row(ev[0]) == 4;
        wr16(qc, kQueryResult, 0);
        const bool confirmed = query_confirm(qc) && rd16(qc, kQueryResult) == 24;
        pick.value = 21;
        run(pick);
        const bool first_back = rd16(qc, kQueryCursor) == 0 && rd16(qc, kQueryTop) == 0
            && rd16(qc, kQueryResult) == 21;
        pick.value = 23;
        run(pick);
        const bool third = rd16(qc, kQueryCursor) == 2 && rd16(qc, kQueryTop) == 0 && rd16(qc, kQueryResult) == 23;
        events(&cursor, ev, 64);
        check(fourth && confirmed && first_back && third && rd16(q, kMenuCursor) == row_before && g_log.count == 0,
            "answer 24 from the first option: +0x30 = 3, +0x32 = 1 (the fourth among three rows), 24 written,"
            " cursor{query, 4}; the game's confirm reading that +0x30 takes 24 too; then 21: +0x30 = 0, +0x32 = 0;"
            " then 23: +0x30 = 2, +0x32 stays 0; the row and every game routine untouched");
        uint8_t* eq = live("equip") ? live("equip") : open_now("equip");
        drain(g_e);
        events(&cursor, ev, 64);
        const int was = rd16(eq, kMenuCursor);
        wr16(eq, kMenuCursor, was == 4 ? 3 : 4);
        drain(g_e);
        n = events(&cursor, ev, 64);
        check(n == 1 && event_type(ev[0]) == kEvCursor && cursor_name(ev[0]) == name("equip")
                && cursor_row(ev[0]) == (was == 4 ? 3 : 4),
            "any other window's cursor event still carries its row");
        wr32(qc, kQueryOptions, 0);
        wr32(qc, kQueryCount, 0);
    }

    // 0.7.0: a block closes the window when it is open, through the game's own
    // close; a closed one has nothing to close; a close the game refuses
    // leaves the block holding and posts an error to the blocking handle
    {
        if (!live("equip")) {
            open_now("equip");
        }
        events(&cursor, ev, 64);
        const int closes = g_close_calls;
        Command b = cmd(kOpBlockClose, "equip", 0, 0, s1, gen1);
        b.verb = kVerbBlock;
        run(b);
        int n = events(&cursor, ev, 64);
        check(g_close_calls == closes + 1 && !live("equip") && n == 1 && is_event(ev[0], kEvClosed, "equip")
                && strcmp(g_log.last_close, "menu    equip   ") == 0,
            "a block's close of the open equip: the game's close by name, then closed{equip}");
        run(b);
        check(g_close_calls == closes + 1 && events(&cursor, ev, 64) == 0,
            "a block's close of a window closed since: nothing to close, nothing posted");
        open_now("equip");
        events(&cursor, ev, 64);
        g_close_refuses = true;
        run(b);
        g_close_refuses = false;
        n = events(&cursor, ev, 64);
        check(live("equip") && open_menu(g_e, name("equip"), NULL) && n == 1
                && is_error(ev[0], s1, gen1, "block", "equip", "the game did not close it; the block holds"),
            "a close the game refuses: equip stays open, and an error with verb block reaches the blocking handle");
    }

    // 0.7.0: a handle's name stays readable by its slot and generation until
    // the slot is claimed again, for the error that names a handle
    {
        HandleTable t;
        memset(&t, 0, sizeof(t));
        const int a = t.claim("alpha", 0);
        const uint32_t g = t.slots[a].generation;
        t.free_slot(a);
        const bool kept = t.name_of(a, g) && strcmp(t.name_of(a, g), "alpha") == 0 && t.name_of(a, g + 1) == NULL;
        const int b = t.claim("beta", 0);
        check(kept && b == a && t.name_of(a, g) == NULL && strcmp(t.name_of(a, g + 1), "beta") == 0,
            "a released handle's name until its slot is claimed again; the new holder's after");
        HeapFree(GetProcessHeap(), 0, t.slots);
    }

    // restore everything
    {
        g_e.holds.set(h1, name("targetwi"), kHoldHide, true);
        g_e.holds.set(h2, name("menuwind"), kHoldBlock, true);
        run(cmd(kOpMove, "subwindo", 3, 4, s1, gen1));
        g_e.waiting.write(2, 10, 20, s1, gen1);
        g_e.holds.release(h1, g_e.inv.count);
        g_e.holds.release(h2, g_e.inv.count);
        run(cmd(kOpRestoreAll, NULL, 0, 0, -1, 0));
        bool forgotten = true;
        bool untouched = true;
        for (int n = 0; n < g_e.inv.count; ++n) {
            forgotten = forgotten && !g_e.memory.e[n].active;
            untouched = untouched && !g_e.place[n].touched && !g_e.place[n].registry_undocked
                && !g_e.place[n].applied_hidden;
        }
        check(memcmp(g_registry, g_pristine, sizeof(g_registry)) == 0,
            "restore_all: the registry is byte-identical to the expected table");
        check(forgotten && untouched, "restore_all: no memory, no placement, no hide left");
        check(ox("persona") == 16 && ox("ability") == 10, "restore_all: moved windows are home");
        bool unsized = true;
        for (int n = 0; n < g_e.inv.count; ++n) {
            unsized = unsized && !g_e.memory.e[n].size_kind && !g_e.place[n].size_kind && !g_e.place[n].swapped;
        }
        check(unsized, "restore_all: no size remembered or applied");
        bool unwaited = true;
        for (int g = 0; g < kGroupCount; ++g) {
            unwaited = unwaited && !g_e.waiting.g[g].active;
        }
        check(unwaited, "restore_all: no group move left waiting");
        bool unread = true;
        for (int n = 0; n < g_e.inv.count; ++n) {
            unread = unread && g_e.anchor[n] == kAnchorUnknown;
        }
        check(unread, "restore_all: no window's anchoring kept");
    }

    // -- queues under load
    {
        g_e.events.claimed = 0;
        memset(g_e.events.slot, 0, sizeof(g_e.events.slot));
        uint32_t c = 0;
        uint32_t dropped = 0;
        for (uint32_t i = 0; i < kEventCapacity + 10; ++i) {
            g_e.events.post(i);
        }
        uint32_t out[16];
        const int n = g_e.events.read(&c, out, 16, &dropped);
        check(dropped == 10 && n == 16 && out[0] == 10 && out[15] == 25,
            "event ring: a reader a lap behind loses exactly the overwritten events, oldest first");
    }
    {
        g_e.events.claimed = 0;
        memset(g_e.events.slot, 0, sizeof(g_e.events.slot));
        const int producers = 4;
        const int each = 200000;
        EventLoad load[producers];
        HANDLE threads[producers];
        for (int p = 0; p < producers; ++p) {
            load[p].producer = p + 1;
            load[p].count = each;
            threads[p] = CreateThread(NULL, 0, &event_producer, &load[p], 0, NULL);
        }
        uint32_t c = 0;
        uint32_t dropped = 0;
        uint32_t last[producers + 1] = {0, 0, 0, 0, 0};
        uint32_t received = 0;
        bool ordered = true;
        static uint32_t out[4096];
        bool done = false;
        while (!done) {
            done = WaitForMultipleObjects(producers, threads, TRUE, 0) == WAIT_OBJECT_0;
            for (;;) {
                const int n = g_e.events.read(&c, out, 4096, &dropped);
                for (int i = 0; i < n; ++i) {
                    const uint32_t p = out[i] >> 28;
                    const uint32_t seq = out[i] & 0x0FFFFFFF;
                    if (p < 1 || p > static_cast<uint32_t>(producers) || seq <= last[p]) {
                        ordered = false;
                    } else {
                        last[p] = seq;
                    }
                }
                received += n;
                if (n == 0) {
                    break;
                }
            }
        }
        for (int p = 0; p < producers; ++p) {
            CloseHandle(threads[p]);
        }
        char label[160];
        snprintf(label, sizeof(label),
            "event ring: 4 producers x %d against a live reader, %u received + %u dropped = all, each in order",
            each, received, dropped);
        check(ordered && received + dropped == static_cast<uint32_t>(producers * each) && c == g_e.events.position(),
            label);
    }
    {
        g_e.commands.head = 0;
        g_e.commands.tail = 0;
        Command c;
        memset(&c, 0, sizeof(c));
        uint32_t pushed = 0;
        while (g_e.commands.push(c)) {
            ++pushed;
        }
        check(pushed == kCommandCapacity, "command ring: full at capacity, and refuses the next push");
        while (g_e.commands.pop(&c)) {
        }
        CommandLoad load;
        load.count = 1000000;
        HANDLE t = CreateThread(NULL, 0, &command_producer, &load, 0, NULL);
        uint32_t expect = 0;
        bool ordered = true;
        while (expect < load.count) {
            if (g_e.commands.pop(&c)) {
                ordered = ordered && c.value == expect && c.x == static_cast<int16_t>(expect);
                ++expect;
            } else {
                SwitchToThread();
            }
        }
        WaitForSingleObject(t, INFINITE);
        CloseHandle(t);
        check(ordered && !g_e.commands.pop(&c), "command ring: 1000000 commands across threads, in order, none lost");
    }
    {
        HANDLE t = CreateThread(NULL, 0, &memory_writer, NULL, 0, NULL);
        uint32_t reads = 0;
        bool consistent = true;
        for (int i = 0; i < 200000; ++i) {
            MemEntry m;
            if (g_e.memory.read(5, &m) && m.active) {
                ++reads;
                consistent = consistent && m.x == m.y && m.owner_slot == static_cast<int32_t>(m.owner_gen)
                    && (m.owner_slot & 0x7FFF) == m.x && m.keep_dock == (m.owner_slot & 1)
                    && m.group == (m.owner_slot & 1);
            }
        }
        InterlockedExchange(&g_stop, 1);
        WaitForSingleObject(t, INFINITE);
        CloseHandle(t);
        char label[128];
        snprintf(label, sizeof(label), "memory: %u reads against a live writer, every copy consistent", reads);
        check(consistent && reads > 0, label);
    }

    // -- the reply writer: {x = 1.5, [1] = 'ab'}, true
    {
        static const uint8_t expect[] = {
            HU_REPLY_TABLE,
            HU_REPLY_NUMBER, 0, 0, 0, 0, 0, 0, 0xF8, 0x3F,
            HU_REPLY_FIELD, 1, 0, 0, 0, 'x',
            HU_REPLY_STRING, 2, 0, 0, 0, 'a', 'b',
            HU_REPLY_INDEX, 1, 0, 0, 0,
            HU_REPLY_TRUE,
        };
        uint8_t data[64];
        memset(data, 0xEE, sizeof(data));
        HuEngineReply out = {sizeof(HuEngineReply), data, sizeof(data), 0};
        ReplyWriter w = {&out};
        w.table();
        w.set_number("x", 1.5);
        w.string("ab");
        w.index(1);
        w.boolean(true);
        check(out.used == sizeof(expect) && memcmp(data, expect, sizeof(expect)) == 0,
            "reply writer: opcodes, little-endian lengths and doubles as hideui_engine_abi.h defines them");

        uint8_t small[8];
        memset(small, 0xEE, sizeof(small));
        HuEngineReply cut = {sizeof(HuEngineReply), small, 5, 0};
        ReplyWriter c = {&cut};
        c.table();
        c.set_number("x", 1.5);
        c.string("ab");
        c.index(1);
        c.boolean(true);
        check(cut.used == sizeof(expect) && small[5] == 0xEE && small[6] == 0xEE && small[7] == 0xEE,
            "reply writer: past the capacity it writes nothing and counts what it needs");
    }

    // -- the glyph decoder: query's parsed title and options
    {
        GlyphText t;
        int16_t g[200];
        int n = put_text(g, 0, "A ");
        g[n++] = -0x102;
        n = put_text(g, n, "pair of pugilists");
        g[n++] = -0x101;
        n = put_text(g, n, ".");
        const char* runs = decoded_runs(g, n, &t);
        check(n == 22 && strcmp(t.text, "A pair of pugilists.") == 0 && t.undecoded == 0
                && strcmp(runs, "A /1E:1|pair of pugilists/1E:2|./1E:1") == 0,
            "glyphs: the measured 22-glyph \"A pair of pugilists.\" -- glyph 0 a space, the green run its own"
            " segment, the colour codes out of the text");

        static const int16_t codes[] = {0x00, 0x5F, 0x60, 0x7FFF, -1, -0xFF, -0x100, 0x22, -0x1FF, 0x23,
                                        -0x200, 0x24, -0x279, 0x25, -0x8000, 0x26};
        memcpy(g, codes, sizeof(codes));
        runs = decoded_runs(g, sizeof(codes) / 2, &t);
        check(strcmp(t.text, " \x7F??BCDEF") == 0 && t.undecoded == 2 && t.run_count == 6
                && strcmp(runs, " \x7F??" "/1E:1|B/1E:0|C/1E:FF|D/1F:0|E/1F:79|F/1F:7E00") == 0,
            "glyphs at every edge: 0 and 0x5F are 0x20 and 0x7F, 0x60 and up a counted ?, -1 and -0xFF not"
            " drawn, -0x100..-0x1FF colour 1E 00..FF, -0x200 and below 1F nn");

        static const int16_t merges[] = {-0x102, -0x105, -0x101, 0x41, -0x101, 0x42, -0x102};
        memcpy(g, merges, sizeof(merges));
        runs = decoded_runs(g, sizeof(merges) / 2, &t);
        check(strcmp(t.text, "ab") == 0 && strcmp(runs, "ab/1E:1") == 0,
            "glyphs: colours with no text under them make no segment; a colour that changes nothing splits none");

        n = put_text(g, 0, "Hi");
        g[n] = g[n + 1] = g[n + 2] = 0;
        decode_glyphs(reinterpret_cast<const uint8_t*>(g), 2, NULL, &t);
        const bool two = strcmp(t.text, "Hi") == 0;
        decode_glyphs(reinterpret_cast<const uint8_t*>(g), 5, NULL, &t);
        check(two && strcmp(t.text, "Hi   ") == 0, "glyphs: read by their count, a 0 a space and never the end");

        for (int i = 0; i < 200; ++i) {
            g[i] = 0x58;
        }
        decode_glyphs(reinterpret_cast<const uint8_t*>(g), 200, NULL, &t);
        const bool capped = strlen(t.text) == static_cast<size_t>(kGlyphMax) && t.run_count == 1
            && t.runs[0].length == kGlyphMax;
        decode_glyphs(reinterpret_cast<const uint8_t*>(g), 0, NULL, &t);
        const bool empty = t.text[0] == '\0' && t.run_count == 0 && t.undecoded == 0;
        decode_glyphs(reinterpret_cast<const uint8_t*>(g), -3, NULL, &t);
        check(capped && empty && t.text[0] == '\0' && t.run_count == 0,
            "glyphs: a count past 127 reads 127; 0 and a negative count read nothing");

        // The table: built by the drain, on its first run, from the stand-in
        // converter (fake_glyph_convert.h).
        const GlyphTable& table = g_e.glyphs;
        drain(g_e);
        check(!table.ready && table.lookup(0x60) == 0 && g_convert_calls == 0,
            "glyph table: no converter, no table; the drain goes on without one");
        g_e.game.glyph_convert = &counted_convert;
        drain(g_e);
        const int built = g_convert_calls;
        drain(g_e);
        check(built == 11313 && g_convert_calls == built && table.ready && table.size == 715
                && table.cp932_defined == 490 && table.gaiji_named == 221 && table.build_ms >= 0,
            "glyph table: the first drain calls the converter once per Shift-JIS lead and trail pair, EF 1F..EF 3F"
            " included (11313), and keeps 715 glyphs, 490 of them named by a pair code page 932 defines and 221 by"
            " a lead-0xEF pair; the next drain does not call it");
        check(table.lookup(0x60) == 0x8140 && table.lookup(0x89) == 0x8169 && table.lookup(0x8A) == 0x816A,
            "glyph table: 81 40 is 0x60; the full-width parentheses 81 69 and 81 6A are 0x89 and 0x8A");
        check(table.lookup(0xCB) == 0x81AC && table.lookup(0x2136) == 0xEF40 && table.lookup(0x21F1) == 0xEFFC
                && table.lookup(0x5F) == 0 && table.lookup(-0x7F) == 0,
            "glyph table: 0xCB, which 81 AC and every FA..FC pair yield, keeps 81 AC, the first defined pair;"
            " EF 40..EF FC are 0x2136..0x21F1; no code under 0x60");
        check(table.lookup(0xCC) == 0x81B8 && table.defined(0xCC) && table.lookup(0xD6) == 0x81CA
                && table.lookup(0xF2) == 0x81F8 && !table.defined(0xF2),
            "glyph table: the unassigned 81 AD yields 0xCC before the assigned 81 B8 and the defined pair wins,"
            " 0xD6 goes to 81 CA the same way; 0xF2, which only unassigned pairs yield, keeps the first, 81 F8");
        check(table.lookup(0x2115) == 0xEF1F && table.lookup(0x211D) == 0xEF27 && table.lookup(0x211E) == 0xEF28
                && table.lookup(0x2135) == 0xEF3F && !table.defined(0x211D) && table.from_gaiji(0x211D),
            "glyph table: EF 1F..EF 3F are tried, and a glyph a lead-0xEF pair yields keeps that pair over the"
            " earlier, defined EE DC..EE FC: the auto-translate brackets are EF 27 and EF 28, not EE E4 and EE E5");
        check(table.lookup(0x1FBE) == 0xED40 && table.lookup(0x2114) == 0xEEDB && table.defined(0x2114)
                && !table.from_gaiji(0x2114),
            "glyph table: the ED and EE glyphs no EF pair yields keep their defined pairs (ED 40, EE DB)");
        check(cp932_defines("\x81\x40") && cp932_defines("\x81\xB8") && !cp932_defines("\x81\xAD")
                && !cp932_defines("\xEF\x27") && cp932_defines("\xEE\xE4"),
            "code page 932 as the build reads it: 81 40 and 81 B8 defined, 81 AD and EF 27 not, EE E4 defined (it"
            " yields 0x211D, as EF 27 does)");

        static const int16_t mixed[] = {0x60, 0x21, -0x102, 0x89, 0x22, 0x8A, -0x101, 0x3000, 0x23};
        memcpy(g, mixed, sizeof(mixed));
        runs = decoded_runs(g, sizeof(mixed) / 2, &t, &table);
        check(strcmp(t.text, "\x81\x40" "A\x81\x69" "B\x81\x6A?C") == 0 && t.undecoded == 1
                && strcmp(runs, "\x81\x40" "A/1E:1|\x81\x69" "B\x81\x6A/1E:2|?C/1E:1") == 0,
            "glyphs through the table: a two-byte character is its two Shift-JIS bytes, whole inside its run; one"
            " the table lacks is a counted ?");

        static GlyphTable unbuilt;
        memcpy(&unbuilt, &table, sizeof(unbuilt));
        unbuilt.ready = 0;
        runs = decoded_runs(g, 4, &t, &unbuilt);
        check(strcmp(t.text, "?A?") == 0 && t.undecoded == 2,
            "glyphs: a table not yet ready reads every two-byte character as a counted ?");

        for (int i = 0; i < 200; ++i) {
            g[i] = 0x89;
        }
        decode_glyphs(reinterpret_cast<const uint8_t*>(g), 200, &table, &t);
        check(strlen(t.text) == 2 * static_cast<size_t>(kGlyphMax) && t.run_count == 1
                && t.runs[0].length == 2 * kGlyphMax && t.undecoded == 0 && memcmp(t.text, "\x81\x69\x81\x69", 4) == 0,
            "glyphs: 127 two-byte characters are 254 bytes in one run");
    }

    check(g_menus_used <= static_cast<int>(sizeof(g_menus) / sizeof(g_menus[0])),
        "the fabricated menus fit their pool");

    std::printf("%d of %d checks failed\n", g_failures, g_checks);
    return g_failures;
}
