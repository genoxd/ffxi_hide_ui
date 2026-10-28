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
// window converted at once, or at its open while closed, and kept on that
// origin at every later open), the error a waiting group move
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
// runs; the unhide gives the keys back. 0.8.0's own: the compass, a name on
// no registry row: its draw hook's pre runs nothing while an owner hides it
// and writes a remembered position's anchor on every draw, a reset puts the
// game's anchor back, the drain posts opened and closed as its state byte
// leaves and reaches 0, and a null object pointer is safe throughout.
// 0.9.0's own: the macro keys, a hold under no name counted once per
// handle: the gate hook's pre answers no while any handle holds it, the
// block's command closes a bar the handler has up and zeroes the object's
// bar bytes as the handler's own close does, and a null object is safe.
// 0.10.0's own: the ability opener's hooks: the pre refuses a kind a handle
// blocks as a category, or every kind while the window is blocked whole,
// posting blocked with the kind, else keeps the kind for the opened event
// the open inside the routine posts, which the post forgets; closed,
// covered and uncovered on ability carry the controller's list; the
// category holds counted once per handle and dropped by a release.
// 0.11.0's own: the prompt list and its engine abi; hiding through the
// per-menu draw's pre, which skips exactly the hidden windows' live
// instances (a closing one's while it lives, never another window's menu at
// a freed instance's address) while every window keeps its row's draw
// layer, live and in the registry; the active menu as the game's update
// picks it by the order that layer gives its list, a hidden window handing
// the focus to a shown one focused after it; the input hooks holding keys
// and the menu mouse mode off a hidden prompt alone, by what the engine
// hides and never by a layer, a hidden window that asks nothing keeping
// them; decode_text on the game's own text; and
// the yes/no class's replies: the callback with the answer, the question
// cleared and holdtime and the window closed in the confirm's and the
// cancel's own orders, a class built not cancellable refusing its cancel,
// the resyn family answered under its controller's key alone, a question
// asked again from inside the callback opened anew with the cursor on its
// default, that open refused by a block leaving the question with no window
// for a reply to end, a held confirm, a stale prompt id refused, and the
// default the drain records from the open's first frame. The hide-only
// windows and their reasons; the KO window's confirm: the decider, the
// message getter and the class's ask called with its input handler's
// arguments, refused while the window is down, dormant, waiting or busy,
// while comyn's controller is foreign or asks, and when comyn's open is
// refused; a hidden KO window holding keys off; and the layouts of the
// yes/no confirm's, the KO window's confirm's and its tick's signatures.
// The resyn family hide-only, the drain closing none of it. The number
// boxes: a question with its window up answered and canceled through the
// box's confirm, a number outside its range refused, a callback a close
// from outside leaves set asking nothing, itemctrl's ask of 1 answering
// itself, a hidden box holding keys off; one a block kept from its window
// recorded at the refused open with its asker's window, row and field and
// announced by a blocked and a pending event; refused while its asker's
// row or field is elsewhere, its cancel needing neither; answered, its
// pending event marked as the replying handle's; ended, with a pending
// event, by its asker's window closing (also in the frame a reply is
// queued), by the confirm run from elsewhere, by an ask that answers
// itself, by another refused open, by a later open, by its asker's state
// moving on, and by another controller in the box's slot; moneyctr's extra
// callback; an unknown asker's question refused both replies; a callback
// that reads its asker's window refused with that window down; and the
// layouts of the boxes' confirms', setups' and the askers' signatures. The
// Asked windows (sortyn, evitem01, confyn, bankmenu, maplist, gmtell,
// mp_dead, ok): each a hidden prompt holding keys and the menu mouse mode
// off; with its window up answered and canceled as its own confirm and
// cancel do, in their order (sortyn's callback, clear and close by name;
// evitem01's callback and close by instance with +0x70; confyn's restore by
// mode; bankmenu's callback with the container and its begin-close's clear;
// maplist's callback with the row's entry and close+reset; gmtell's result
// word; mp_dead's choice; ok's busy byte, acknowledgment, callback and
// closes), refused where its input routine would take nothing; the stale
// fields a close from outside leaves asking nothing; a question a block
// kept from its window recorded at the refused open with the window that
// asked (none for ok, bankmenu's from its own field), announced, answered
// with no window, and ended by that window closing, by another question, by
// a later open and by the game's own end of it, each end a pending event the
// replying handle skips; and the pending payload's room for every one.
// mogdoor: its texts read with the game's getter for each open, its offers,
// answered through its confirm's gate and exit, canceled as its top level's
// Escape closes it, with no gate, whichever level it shows, the floor row
// and the Mog Garden refused while off or for the other floor, the gate's
// refusal writing nothing; blocked, recorded with the nation, the Mog House
// and its door, its tick run each frame, answered with no window, and ended
// as the player leaves, the nation changes, an exit begins or the window
// opens. The dialogue boxes: page events (a page, a new message, one
// replacing another with no close, one alike through a close and an open),
// each other's twin, answer through the text dispatch once under the pair's
// count of opens, which either box's open moves, the cancel's release,
// clear, wait flag and close on the named box and then its twin, the
// refusals, the drain closing neither box, the key
// test's pre by return address and hidden active box, the last-page test
// and the page decoder; close('ok') refused. The text-entry windows: the
// table answer's parse; a hide and a hidden open ending each field with the
// window's own end, passinpu's, msgline's and comment's (on the line it
// edited) started again at the show with the start each open hook calls;
// each window's commit and cancel as its own code makes them, every refusal
// of the game's limits, msgline's long field keeping 299 bytes, msgline's
// every mode's send, and its message blocked, answered with no window and
// ended as the game drops it. The target arrows, a name on no row: opened
// and closed as the target window's draw starts and stops painting one, the
// walk's global hide and rem4line's modal dim among what stops it; the
// target window hidden keeping its layer, and the per-menu draw's, the
// sprite draw's and the quad routine's hooks dropping everything it paints
// but the arrows, and the arrows while targetarrow is hidden or blocked,
// never another menu's quads. The template swap: a swap of the game's that
// rewrites a moved window's frame puts its position back at once and
// nothing else, the party list on the bottom edge the move gave it; one
// that rewrites nothing, one inside the engine's own resize and one inside
// an open re-apply nothing. A zone change: the party list moved by its frame
// while one member tall, closed, opened again at its template's full height
// and swapped to its roster, shrunk and grown, on the bottom edge the move
// gave it at each step. The trade windows: offers, withdrawals, the OK
// and the cancel through each window's own routines, every refusal, the
// gray test with the item's data locked, the trade's offers on their way
// until acknowledged and dropped as the trade ends, the one change a second
// the game sends. 0.11.0's own: the windows open refuses and the engine abi
// each is refused from, the lists of ability and magic by name and number
// and their ranges, the sets; the drain's open refusing the crash windows,
// a slot holding no controller, and a window whose other must be open, its
// close of gift left while trade is open; a list alone through the hooked
// ability opener and the magic opener with the subwindo its rebuild opens
// refused by the open hook with no event, not counted, and already open
// left alone; the sets through their openers, the equipment set's guard
// and the Monstrosity check's al alone read; the error for a list the game
// closes before the next frame, and none for the engine's own close, a
// close after the check's frames or one after another open; the open
// routine's last argument as the game's openers pass it, titlehan showing
// at once over titlewin and targetwi parking under ut_point until the show
// path brings it out; the map, blue magic, automaton, linkshell, instincts,
// synthesis, tabula and macro sets through their openers, with the check
// each player's route asks first and the item use's type of the item at an
// index, every refusal an error and nothing opened; and the show path's
// uncovered posted only for the menu its routine shows. The settings:
// kSettings in the Config menu's order, its names and kinds; the colors'
// packing, the chat filter rows' states and the bits across words; the
// drain's sets against a store laid out as the game's, a word through the
// store's set, a bit and a filter row composed with every other bit kept,
// the equipment lock through the game's request alone, each of its gate's,
// its wait's and its send's refusals an error event, auto_disconnect and
// character_information through their routines, a choice on a bit
// (icon_type) composed as a bit, refused while the player is not in the
// game and while a page that writes its copy back as it closes is open; the
// reads and ranges; and the labels read once through the message getter,
// cut at a whole character.

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

FakeCtl g_ctl[224];
int g_ctl_used;
FakeMenu g_menus[512];
FakeRes g_res[512];
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
const void* vt_yesno[0x20];
const void* vt_bank[0x20];
const void* vt_status[0x20];
const void* vt_door[0x20];
const void* vt_rem4[0x20];
const void* vt_text[0x20];
const void* vt_pass[0x20];

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
    char closes[96];            // each close by name's eight-character name and '|'
    int select_calls;           // select-row's
    int select_row;
    int yesno_calls;            // the yes/no callbacks'
    int yesno_answer;
    void* yesno_context;
    void* equipment_this;       // the equipment set's opener's `this`
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

// The template swap as the daemon presents it: the pre, the original, then
// the post, whose registers are no longer the call's.
int g_swap_hooks;

void __fastcall hooked_template_swap(void* menu, void* edx, const char* key) {
    uint32_t args[1] = {reinterpret_cast<uint32_t>(key)};
    HuFrame f;
    memset(&f, 0, sizeof(f));
    f.ecx = reinterpret_cast<uint32_t>(menu);
    f.args = args;
    f.user = &g_e;
    ++g_swap_hooks;
    hook_swap_pre(&f);
    fake_template_swap(menu, edx, key);
    f.ecx = 0;
    hook_swap_post(&f);
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
        wr16(static_cast<uint8_t*>(ctl), kQueryResult, kQueryCanceled);
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

// The yes/no class's begin-close: the question dropped, its callback never
// called, unless a question asked again waits for the confirm's open.
uint32_t __fastcall fake_yesno_begin_close(void* ctl, void*) {
    uint8_t* c = static_cast<uint8_t*>(ctl);
    if (!c[kYesNoChained]) {
        memset(c + kYesNoCallback, 0, 12);
        wr32(c, kYesNoMode, 0);
    }
    return 0;
}

void __fastcall fake_select_row(void* menu, void*, int row) {
    logged('W');
    ++g_log.select_calls;
    g_log.select_row = row;
    wr16(static_cast<uint8_t*>(menu), kMenuCursor, row);
}

void __cdecl fake_yesno_answered(void* context, int answer) {
    logged('N');
    ++g_log.yesno_calls;
    g_log.yesno_answer = answer;
    g_log.yesno_context = context;
}

const char g_text_chained[] = "Really?";
int g_chain_yes;

// The class's re-ask from inside a callback.
bool yesno_reask(uint8_t* ctl, const char* text, bool default_yes) {
    if (!rdptr(ctl, kYesNoCallback)) {
        return false;
    }
    const YesNoCallbackFn cb = &fake_yesno_answered;
    memcpy(ctl + kYesNoCallback, &cb, 4);
    memcpy(ctl + kYesNoContext, &ctl, 4);
    memcpy(ctl + kYesNoText, &text, 4);
    ctl[kYesNoCancellable] = ctl[0x38];
    ctl[kYesNoChained] = 1;
    ctl[kYesNoChainedYes] = default_yes ? 1 : 0;
    wr32(ctl, kYesNoMode, 0);
    return true;
}

void __cdecl fake_yesno_chaining(void* context, int answer) {
    fake_yesno_answered(context, answer);
    yesno_reask(static_cast<uint8_t*>(context), g_text_chained, g_chain_yes != 0);
}

uint8_t* controller(const char* nm);
void* __fastcall hooked_open(void* mcb, void* edx, const char* key, int activate, int overlap);

// The class's ask, as the game's: refused while a question is pending;
// else stored, its key opened through the hooked open, a refused open
// clearing the callback, then the cursor on no unless the default is yes,
// then the mode. `chain` 1 or 2: the callback asks again, no or yes first.
bool yesno_ask(const char* nm, const char* text, bool default_yes, bool cancellable, int mode, int chain = 0) {
    uint8_t* ctl = controller(nm);
    if (!ctl || rdptr(ctl, kYesNoCallback)) {
        return false;
    }
    g_chain_yes = chain == 2;
    const YesNoCallbackFn cb = chain ? &fake_yesno_chaining : &fake_yesno_answered;
    memcpy(ctl + kYesNoCallback, &cb, 4);
    memcpy(ctl + kYesNoContext, &ctl, 4);
    memcpy(ctl + kYesNoText, &text, 4);
    ctl[kYesNoCancellable] = cancellable ? ctl[0x38] : 0;
    ctl[kYesNoChained] = 0;
    void* menu = hooked_open(g_mcb, NULL, reinterpret_cast<const char*>(ctl + kYesNoKey), 1, 0);
    if (!menu) {
        wr32(ctl, kYesNoCallback, 0);
        return false;
    }
    if (!default_yes) {
        fake_select_row(menu, NULL, kRowNo);
    }
    wr32(ctl, kYesNoMode, static_cast<uint32_t>(mode));
    return true;
}

// What the KO window's confirm handed the routines it calls.
struct DeadLog {
    int decider_calls;
    int32_t decider_args[2];
    uint32_t decider_answer;    // only al is the answer: the rest of eax dirty on purpose
    int message_calls;
    int32_t message_args[2];
    int ask_calls;
    void* ask_ctl;
    const void* ask_callback;
    const char* ask_text;
    int32_t ask_args[5];        // context, default, help, cancellable, mode
};
DeadLog g_dead;

const char g_text_home[] = "Return to home point?";
const char g_text_other[] = "Leave this area?";

uint32_t __cdecl fake_dead_decider(int32_t first, int32_t second) {
    logged('d');
    ++g_dead.decider_calls;
    g_dead.decider_args[0] = first;
    g_dead.decider_args[1] = second;
    return g_dead.decider_answer;
}

// The settings' labels the message getter answers, by string table 0xB's
// id; NULL answers NULL, as the getter does before its tables load.
const char* g_label_texts[0x100];
int g_label_calls;

const char* __cdecl fake_message(int32_t category, int32_t id) {
    if (category == kLabelTable) {
        ++g_label_calls;
        return id >= 0 && id < 0x100 ? g_label_texts[id] : NULL;
    }
    if (category == kDoorTextCategory || category == kAreaNameCategory) {
        static char texts[32][24];
        static int next;
        char* text = texts[next++ % 32];
        snprintf(text, sizeof(texts[0]), "%s %d", category == kAreaNameCategory ? "Area" : "Text", id);
        return text;
    }
    logged('m');
    ++g_dead.message_calls;
    g_dead.message_args[0] = category;
    g_dead.message_args[1] = id;
    return id == 0x258 ? g_text_other : g_text_home;
}

// The settings' stand-ins: a config store laid out as the game's, its set
// writing the entry's value and recording each call, the equipment lock's
// request as the game's (its gate, its wait on the clock from its stamp,
// its send, 0xBF written as the send goes), the idle timer and the player
// flag's request recording theirs, and the player's data, its flags at
// kSettingFlags.
uint8_t g_cfg_store[0x10];
uint8_t* g_cfg_store_ptr = g_cfg_store;
uint8_t g_cfg_entries[0xD0 * kEntryBytes];
uint8_t g_setting_player[0x40];
uint8_t* g_setting_player_ptr = g_setting_player;
const uint32_t kSettingFlags = 0x30;

struct SettingLog {
    int sets;
    int32_t set[16][2];         // each set's id and value, the first 16
    int locks;
    int32_t lock[3];
    int32_t lock_bf;            // 0xBF's value when the request ran
    int lock_sets;              // the store's sets before the request ran
    int idles;
    int32_t idle;
    int flags;
    int32_t flag[2];
};
SettingLog g_setting_log;

uint8_t* cfg_entry(int id) {
    return g_cfg_entries + id * kEntryBytes;
}

int32_t cfg_value(int id) {
    return static_cast<int32_t>(rd32(cfg_entry(id), kEntryValue));
}

void put_cfg(int id, int32_t value, int32_t min, int32_t max, int32_t def) {
    uint8_t* e = cfg_entry(id);
    wr32(e, kEntryId, static_cast<uint32_t>(id));
    wr32(e, kEntryValue, static_cast<uint32_t>(value));
    wr32(e, kEntryMin, static_cast<uint32_t>(min));
    wr32(e, kEntryMax, static_cast<uint32_t>(max));
    wr32(e, kEntryDefault, static_cast<uint32_t>(def));
}

int32_t __cdecl fake_store_set(int32_t id, int32_t value) {
    SettingLog& l = g_setting_log;
    if (l.sets < 16) {
        l.set[l.sets][0] = id;
        l.set[l.sets][1] = value;
    }
    ++l.sets;
    if (cfg_value(id) == value) {
        return -1;
    }
    wr32(cfg_entry(id), kEntryValue, static_cast<uint32_t>(value));
    return 1;
}

struct LockGame {
    uint8_t busy;               // the gate refuses
    uint8_t unsent;             // the send fails
    uint32_t clock;
    uint32_t stamp;
    uint32_t wait;
    int messages;               // the wait's refusals' message
};
LockGame g_lock_game;

uint32_t __cdecl fake_lock_clock() {
    return g_lock_game.clock;
}

uint32_t __cdecl fake_lock_request(int32_t mode, int32_t a, int32_t b) {
    SettingLog& l = g_setting_log;
    LockGame& g = g_lock_game;
    ++l.locks;
    l.lock[0] = mode;
    l.lock[1] = a;
    l.lock[2] = b;
    l.lock_bf = cfg_value(0xBF);
    l.lock_sets = l.sets;
    if (g.busy) {
        return 0;
    }
    if (mode != 2 && !(a & 0xFF)) {
        if (!(fake_lock_clock() - g.stamp > g.wait)) {
            ++g.messages;
            return 1;
        }
        g.stamp = fake_lock_clock();
    }
    if (g.unsent) {
        return 0;
    }
    if (mode == 0) {
        fake_store_set(0xBF, 0);
    } else if (mode == 1 || mode == 3 || mode == 4) {
        fake_store_set(0xBF, 1);
    }
    return 1;
}

void __cdecl fake_idle_timer(int32_t seconds) {
    ++g_setting_log.idles;
    g_setting_log.idle = seconds;
}

uint32_t __cdecl fake_flag_request(int32_t flag, int32_t on) {
    ++g_setting_log.flags;
    g_setting_log.flag[0] = flag;
    g_setting_log.flag[1] = on;
    return 1;
}

// The KO window's question's callback: a yes marks the home point chosen.
void __cdecl fake_dead_chosen(void*, int answer) {
    logged('k');
    if (answer) {
        controller("dead")[kDeadChosen] = 1;
    }
}

// The class's ask as the engine calls it, `this` the controller, every
// argument the dword the game's callers push; as the game's: refused while a
// question is pending, a refused open clearing the callback. al the answer.
uint32_t __fastcall fake_yesno_ask_this(void* ctl, void*, const void* callback, int32_t context, const char* text,
                                       int32_t default_yes, int32_t help, int32_t cancellable, int32_t mode) {
    logged('a');
    ++g_dead.ask_calls;
    g_dead.ask_ctl = ctl;
    g_dead.ask_callback = callback;
    g_dead.ask_text = text;
    const int32_t args[5] = {context, default_yes, help, cancellable, mode};
    memcpy(g_dead.ask_args, args, sizeof(args));
    uint8_t* c = static_cast<uint8_t*>(ctl);
    if (rdptr(c, kYesNoCallback)) {
        return 0xABCD0000u;
    }
    memcpy(c + kYesNoCallback, &callback, 4);
    wr32(c, kYesNoContext, static_cast<uint32_t>(context));
    memcpy(c + kYesNoText, &text, 4);
    c[kYesNoCancellable] = cancellable ? c[0x38] : 0;
    c[kYesNoChained] = 0;
    void* menu = hooked_open(g_mcb, NULL, reinterpret_cast<const char*>(c + kYesNoKey), 1, 0);
    if (!menu) {
        wr32(c, kYesNoCallback, 0);
        return 0xABCD0000u;
    }
    if (!default_yes) {
        fake_select_row(menu, NULL, kRowNo);
    }
    wr32(c, kYesNoMode, static_cast<uint32_t>(mode));
    return 0xABCD0001u;
}

// The number boxes as the game's: each setup writes its question into the
// controller and only then opens its key through the hooked open; each
// confirm calls the callback with the number and clears it, moneyctr's runs
// its extra callback, and both close their window by instance.
int __fastcall fake_close(void*, void*, const char* key);
typedef void (__cdecl* BoxCallbackFn)(int32_t value, void* context);

const int32_t kGil = 5000;              // what a moneyctr max of 0 resolves to

struct BoxLog {
    int calls;                          // the askers' callbacks
    int value;
    void* context;
    int who;                            // kAskers' index, -1 for the unknown asker's
    int extras;                         // moneyctr's extra callback
    int confirms[kBoxCount];
    int confirm_value;
};
BoxLog g_box;

void box_called(int who, int32_t value, void* context) {
    logged('#');
    ++g_box.calls;
    g_box.value = value;
    g_box.context = context;
    g_box.who = who;
}

#define ASKER_CALLBACK(i) \
    void __cdecl asker_callback_##i(int32_t value, void* context) { box_called(i, value, context); }
ASKER_CALLBACK(0)
ASKER_CALLBACK(1)
ASKER_CALLBACK(2)
ASKER_CALLBACK(3)
ASKER_CALLBACK(4)
ASKER_CALLBACK(5)
ASKER_CALLBACK(6)
ASKER_CALLBACK(7)
ASKER_CALLBACK(8)
ASKER_CALLBACK(9)
ASKER_CALLBACK(10)
ASKER_CALLBACK(11)
ASKER_CALLBACK(12)
ASKER_CALLBACK(13)
ASKER_CALLBACK(14)
ASKER_CALLBACK(15)
#undef ASKER_CALLBACK

const BoxCallbackFn kAskerCallbacks[kAskerCount] = {
    &asker_callback_0, &asker_callback_1, &asker_callback_2, &asker_callback_3, &asker_callback_4,
    &asker_callback_5, &asker_callback_6, &asker_callback_7, &asker_callback_8, &asker_callback_9,
    &asker_callback_10, &asker_callback_11, &asker_callback_12, &asker_callback_13, &asker_callback_14,
    &asker_callback_15,
};

void __cdecl unknown_asker_callback(int32_t value, void* context) {
    box_called(-1, value, context);
}

void extra_callback() {
    logged('x');
    ++g_box.extras;
}

// Close by instance, null-safe: the instance's own key closed by name.
void close_menu(const uint8_t* menu) {
    if (!menu) {
        return;
    }
    char key[kKeyLen + 1];
    memcpy(key, rdptr(menu, kMenuRes) + kResKey, kKeyLen);
    key[kKeyLen] = '\0';
    fake_close(g_mcb, NULL, key);
}

void box_confirm(int b, uint8_t* ctl, int32_t value) {
    const BoxLayout& l = kBoxLayouts[b];
    logged(b == kBoxMoney ? 'g' : 'c');
    ++g_box.confirms[b];
    g_box.confirm_value = value;
    const void* cb = rdptr(ctl, l.callback);
    if (cb) {
        BoxCallbackFn fn;
        memcpy(&fn, &cb, sizeof(fn));
        fn(value, rdptr(ctl, l.context));
        wr32(ctl, l.callback, 0);
    }
    if (b == kBoxMoney && rdptr(ctl, 0x34)) {
        void (*extra)();
        const void* p = rdptr(ctl, 0x34);
        memcpy(&extra, &p, sizeof(extra));
        extra();
        wr32(ctl, 0x34, 0);
    }
    close_menu(rdptr(ctl, kCtlMenu));
}

void __fastcall fake_money_confirm(void* ctl, void*, int32_t value) {
    box_confirm(kBoxMoney, static_cast<uint8_t*>(ctl), value);
}

void __fastcall fake_item_confirm(void* ctl, void*, int32_t value) {
    box_confirm(kBoxItem, static_cast<uint8_t*>(ctl), value);
}

// The setups, as the askers call them on the game thread: the window, or
// NULL when its open was refused or never made.
uint8_t* money_setup(int32_t initial, int32_t max, BoxCallbackFn cb, const void* context) {
    uint8_t* ctl = controller("moneyctr");
    wr32(ctl, 0x28, static_cast<uint32_t>(initial));
    memcpy(ctl + 0x2C, &cb, 4);
    wr32(ctl, 0x24, static_cast<uint32_t>(max ? max : kGil));
    memcpy(ctl + 0x30, &context, 4);
    wr32(ctl, 0x34, 0);
    return static_cast<uint8_t*>(hooked_open(g_mcb, NULL, "menu    moneyctr", 1, 0));
}

uint8_t* item_setup(int initial, int max, BoxCallbackFn cb, const void* context) {
    uint8_t* ctl = controller("itemctrl");
    if (!max) {
        return NULL;
    }
    wr32(ctl, 0x28, 0);
    ctl[0x23] = static_cast<uint8_t>(initial);
    ctl[0x22] = static_cast<uint8_t>(max);
    memcpy(ctl + 0x24, &cb, 4);
    memcpy(ctl + 0x2C, &context, 4);
    if (max == 1) {
        cb(initial, const_cast<void*>(context));
        return NULL;
    }
    return static_cast<uint8_t*>(hooked_open(g_mcb, NULL, "menu    itemctrl", 1, 0));
}

int put_text(int16_t* glyphs, int at, const char* text);

// The Asked windows as the game's: each setup writes its question and
// opens its key through the hooked open (evitem01's opens first, then
// writes); the callbacks and the routines the replies call record what they
// were given.
struct AskedLog {
    int calls;
    int who;                    // core.h's kAsked*
    int value;                  // the answer, container, zone or restore argument
    int extra;                  // bankmenu's canceled flag; maplist's map
    const void* context;
    int instance_closes;        // close by instance, the flag it was given
    int instance_flag;
};
AskedLog g_asked;

void asked_called(int who, int value, int extra, const void* context) {
    logged('@');
    ++g_asked.calls;
    g_asked.who = who;
    g_asked.value = value;
    g_asked.extra = extra;
    g_asked.context = context;
}

void __cdecl sortyn_answered(void* context, int answer) { asked_called(kAskedSortyn, answer, 0, context); }
void __cdecl evitem_answered(int answer) { asked_called(kAskedEvitem, answer & 0xFF, 0, NULL); }
void __cdecl confyn_restored(int which) { asked_called(kAskedConfyn, which, 0, NULL); }
void __cdecl bank_answered(int container, int canceled) { asked_called(kAskedBank, container, canceled, NULL); }
void __cdecl ok_answered(void* context) { asked_called(kAskedOk, 0, 0, context); }

void __cdecl map_answered(const void* entry) {
    const uint8_t* p = static_cast<const uint8_t*>(entry);
    asked_called(kAskedMaplist, static_cast<int32_t>(rd32(p, 0)), static_cast<int32_t>(rd32(p, 4)), entry);
}

void __fastcall close_by_instance(void*, void*, void* menu, int flag) {
    ++g_asked.instance_closes;
    g_asked.instance_flag = flag;
    close_menu(static_cast<uint8_t*>(menu));
}

// bankmenu's begin-close, and the clear the reply calls with no window.
uint32_t __fastcall bank_clear(void* ctl, void*) {
    wr32(static_cast<uint8_t*>(ctl), kBankCallback, 0);
    return 0;
}

// maplist's close+reset: its key closed by name, the list emptied and its
// rows freed; the callback stays.
uint32_t __fastcall maplist_close_reset(void* ctl, void*) {
    logged('R');
    uint8_t* c = static_cast<uint8_t*>(ctl);
    fake_close(g_mcb, NULL, "menu    maplist ");
    wr32(c, kMapCount, 0);
    wr32(c, kMapRows, 0);
    wr32(c, kMapRows + 4, 0);
    return 0;
}

uint8_t g_player[0x40];
uint8_t* g_player_ptr = g_player;
const uint32_t kSafe2Flag = 0x21;
uint8_t g_gm_queue[kGmTellSlots * kGmTellSlotBytes];
int32_t g_gm_slot;

const char g_sortyn_text[] = "Drop the Rusty Key?";
const char g_sortyn_title[] = "Drop";

uint8_t* sortyn_setup(const void* context, bool default_yes) {
    uint8_t* ctl = controller("sortyn");
    const SortynCallbackFn cb = &sortyn_answered;
    memcpy(ctl + kSortynCallback, &cb, 4);
    memcpy(ctl + kSortynContext, &context, 4);
    const char* text = g_sortyn_text;
    const char* title = g_sortyn_title;
    memcpy(ctl + kSortynText, &text, 4);
    memcpy(ctl + kSortynTitle, &title, 4);
    uint8_t* menu = static_cast<uint8_t*>(hooked_open(g_mcb, NULL, "menu    sortyn  ", 1, 0));
    if (menu && !default_yes) {
        fake_select_row(menu, NULL, kRowNo);
    }
    return menu;
}

// evitem's ask: evitem01 opened, then its setup: the question, the
// callback.
uint8_t* evitem_ask(int32_t row) {
    wr32(controller("evitem"), kEvitemRow, static_cast<uint32_t>(row));
    uint8_t* menu = static_cast<uint8_t*>(hooked_open(g_mcb, NULL, "menu    evitem01", 1, 0));
    uint8_t* ctl = controller("evitem01");
    strcpy(reinterpret_cast<char*>(ctl + kEvitemText), "Use the Rusty Key?");
    const EvitemCallbackFn cb = &evitem_answered;
    memcpy(ctl + kEvitemCallback, &cb, 4);
    return menu;
}

uint8_t* confyn_ask(int32_t mode) {
    uint8_t* menu = static_cast<uint8_t*>(hooked_open(g_mcb, NULL, "menu    confyn  ", 1, 0));
    wr32(controller("confyn"), kConfynMode, static_cast<uint32_t>(mode));
    return menu;
}

// bankmenu's setup: refused while a question waits; else its close, the
// callback, its open. False when refused.
bool bank_setup() {
    uint8_t* ctl = controller("bankmenu");
    if (rdptr(ctl, kBankCallback)) {
        return false;
    }
    fake_close(g_mcb, NULL, "menu    bankmenu");
    const BankCallbackFn cb = &bank_answered;
    memcpy(ctl + kBankCallback, &cb, 4);
    hooked_open(g_mcb, NULL, "menu    bankmenu", 1, 0);
    return true;
}

// maplist's rows: at the top a zone's map labeled, a region's list of 6 (map
// -1, as the game's region rows), another map, the closing row; in that
// region (kMapRegion 2) a zone's one map and a zone of several maps (-2 - 190,
// map 0); in zone 190's maps (kMapRegion 192, kMapZone 0) its two maps.
uint8_t g_map_rows[4 * kAreaRowStride];
uint8_t g_map_entries[4][8];
uint8_t g_map_rows_again[4 * kAreaRowStride];
uint8_t g_map_region_rows[2 * kAreaRowStride];
uint8_t g_map_zone_rows[2 * kAreaRowStride];
uint8_t g_map_level_entries[2][8];
const char* const g_map_texts[4] = {"Current Area", "Ronfaure", "Bastok Markets", "Close"};
const char* const g_map_region_texts[2] = {"West Ronfaure", "King Ranperre's Tomb"};
const char* const g_map_zone_texts[2] = {"King Ranperre's Tomb", "King Ranperre's Tomb"};
int32_t g_map_zone_now = 100;

// maplist's rebuild, its select and its cursor read, as the game's: the
// rows of the level kMapRegion and kMapZone name, rebuilt into the arrays
// above; the cursor put on a row, recorded; the row under the cursor,
// 0-based, -1 with no window.
struct MapLog {
    int rebuilds;
    int selects;
    int row;
    int flag;
};
MapLog g_map;

void map_row(uint8_t* r, const char* text, uint8_t* entry, int32_t id, int32_t map) {
    memset(r, 0, kAreaRowStride);
    r[0] = kFormatText;
    memcpy(r + kAreaRowValue, &text, 4);
    wr32(entry, 0, static_cast<uint32_t>(id));
    wr32(entry, 4, static_cast<uint32_t>(map));
    memcpy(r + kAreaRowEntry, &entry, 4);
}

void map_top_rows(uint8_t* ctl, int32_t zone, uint8_t* rows) {
    const int32_t ids[4] = {zone, -2, zone + 1, kMapCloseRow};
    const int32_t maps[4] = {0, -1, 2, 0};
    static const char label[] = "East";
    for (int i = 0; i < 4; ++i) {
        uint8_t* r = rows + i * kAreaRowStride;
        map_row(r, g_map_texts[i], g_map_entries[i], ids[i], maps[i]);
        if (i == 0) {
            r[1] = kFormatTextParens;
            const char* l = label;
            memcpy(r + kAreaRowValue + 4, &l, 4);
        } else if (i == 1) {
            r[1] = kFormatCount;
            wr32(r, kAreaRowValue + 4, 6);
        }
    }
    wr32(ctl, kMapCapacity, kMapRowMax);
    wr32(ctl, kMapCount, 4);
    memcpy(ctl + kMapRows, &rows, 4);
}

uint32_t __fastcall maplist_rebuild(void* ctl, void*) {
    logged('u');
    ++g_map.rebuilds;
    uint8_t* c = static_cast<uint8_t*>(ctl);
    if (rd16(c, kMapZone) != -1) {
        for (int i = 0; i < 2; ++i) {
            map_row(g_map_zone_rows + i * kAreaRowStride, g_map_zone_texts[i], g_map_level_entries[i],
                rd16(c, kMapRegion) - 2, i + 1);
        }
        uint8_t* rows = g_map_zone_rows;
        memcpy(c + kMapRows, &rows, 4);
    } else if (rd16(c, kMapRegion) > 0) {
        map_row(g_map_region_rows, g_map_region_texts[0], g_map_level_entries[0], 100, 1);
        map_row(g_map_region_rows + kAreaRowStride, g_map_region_texts[1], g_map_level_entries[1], -2 - 190, 0);
        uint8_t* rows = g_map_region_rows;
        memcpy(c + kMapRows, &rows, 4);
    } else {
        map_top_rows(c, g_map_zone_now, g_map_rows);
        return 0;
    }
    wr32(c, kMapCapacity, kMapRowMax);
    wr32(c, kMapCount, 2);
    return 0;
}

void __fastcall maplist_select(void* ctl, void*, int32_t row, int32_t flag) {
    logged('v');
    ++g_map.selects;
    g_map.row = row;
    g_map.flag = flag;
    uint8_t* menu = rdptr(static_cast<uint8_t*>(ctl), kCtlMenu);
    if (menu && row >= 0) {
        wr16(menu, kMenuCursor, row + 1);
    }
}

uint32_t __fastcall maplist_cursor(void* ctl, void*) {
    const uint8_t* menu = rdptr(static_cast<uint8_t*>(ctl), kCtlMenu);
    return menu ? static_cast<uint32_t>(rd16(menu, kMenuCursor) + rd16(static_cast<uint8_t*>(ctl), kListScroll) - 1)
                : 0xFFFFFFFFu;
}

uint8_t* maplist_setup(int32_t zone, uint8_t* rows = g_map_rows) {
    uint8_t* ctl = controller("maplist");
    maplist_close_reset(ctl, NULL);
    g_map_zone_now = zone;
    map_top_rows(ctl, zone, rows);
    wr16(ctl, kMapRegion, 0);
    wr16(ctl, kMapZone, -1);
    uint8_t* menu = static_cast<uint8_t*>(hooked_open(g_mcb, NULL, "menu    maplist ", 1, 0));
    const MaplistCallbackFn cb = &map_answered;
    memcpy(ctl + kMapCallback, &cb, 4);
    return menu;
}

// A GM's question: the queue's slot, the window, its title and options
// valued 1 to 3.
uint8_t g_gm_nodes[3][0x18];
uint8_t g_gm_items[3][0x108];

uint8_t* gmtell_ask(int32_t slot) {
    g_gm_slot = slot;
    memset(g_gm_queue + slot * kGmTellSlotBytes, 0, kGmTellSlotBytes);
    memcpy(g_gm_queue + slot * kGmTellSlotBytes, "GM_Test", 7);
    uint8_t* menu = static_cast<uint8_t*>(hooked_open(g_mcb, NULL, "menu    gmtell  ", 1, 0));
    uint8_t* ctl = controller("gmtell");
    int16_t* title = reinterpret_cast<int16_t*>(ctl + 0x38);
    ctl[0x36] = static_cast<uint8_t>(put_text(title, 0, "Are you there?"));
    static const char* const texts[3] = {"Yes", "No", "Not sure"};
    for (int i = 0; i < 3; ++i) {
        uint8_t* node = g_gm_nodes[i];
        uint8_t* item = g_gm_items[i];
        const uint8_t* next = i + 1 < 3 ? g_gm_nodes[i + 1] : NULL;
        memcpy(node, &next, 4);
        memcpy(node + 0x10, &item, 4);
        node[0x14] = 0;
        item[0x106] = static_cast<uint8_t>(put_text(reinterpret_cast<int16_t*>(item + 4), 0, texts[i]));
        wr16(item, 0x104, i + 1);
        item[0x107] = 1;
    }
    const uint8_t* head = g_gm_nodes[0];
    memcpy(ctl + 0x14, &head, 4);
    wr32(ctl, 0x24, 3);
    wr32(ctl, 0x30, 0);
    wr16(ctl, kQueryResult, 0);
    return menu;
}

// Monstrosity's KO window: open with its countdown and its two rows' texts.
uint8_t g_mp_nodes[2][0x18];
uint8_t g_mp_items[2][0x60];
const char* const g_mp_texts[2] = {"Retry", "Return"};

uint8_t* mp_dead_open(int32_t seconds) {
    uint8_t* menu = static_cast<uint8_t*>(hooked_open(g_mcb, NULL, "menu    mp_dead ", 1, 0));
    uint8_t* ctl = controller("mp_dead");
    wr32(ctl, kMpDeadRemaining, static_cast<uint32_t>(seconds * kDeadTicks));
    wr32(ctl, kMpDeadDeadline, 5000);
    wr32(ctl, kMpDeadChoice, 0);
    if (menu) {
        for (int i = 0; i < 2; ++i) {
            uint8_t* node = g_mp_nodes[i];
            uint8_t* item = g_mp_items[i];
            const uint8_t* next = i == 0 ? g_mp_nodes[1] : NULL;
            memcpy(node, &next, 4);
            memcpy(node + 0x10, &item, 4);
            node[0x14] = 0;
            wr16(item, 0, 1);
            wr16(item, kItemRowId, i + 1);
            memcpy(item + kItemRowText, &g_mp_texts[i], 4);
        }
        const uint8_t* head = g_mp_nodes[0];
        memcpy(menu + kMenuList, &head, 4);
    }
    return menu;
}

// A notice of ok's: its text, the callback a notice may carry, +0x258.
uint8_t* ok_notice(bool callback, const void* context, bool closes) {
    uint8_t* ctl = controller("ok");
    wr32(ctl, kOkAnswered, 0);
    const OkCallbackFn cb = callback ? &ok_answered : NULL;
    memcpy(ctl + kOkCallback, &cb, 4);
    memcpy(ctl + kOkContext, &context, 4);
    strcpy(reinterpret_cast<char*>(ctl + kOkText), "Lost connection to the server.");
    ctl[kOkCloses] = closes ? 1 : 0;
    return static_cast<uint8_t*>(hooked_open(g_mcb, NULL, "menu    ok      ", 1, 0));
}

// mogdoor's world as the game keeps it: the Mog House object and its door
// actor, the three bytes an exit sets, the nations' lists of areas, and the
// routines its confirm, tick and draw call, each recording what it was
// given. The door's handler writes the nation, then opens the window.
struct DoorLog {
    int gates;
    int32_t gate_args[2];
    uint32_t gate_answer;           // only al is the answer: the rest of eax dirty on purpose
    int floors;
    uint32_t floor;                 // the floor read's answer, nonzero on the second floor
    uint32_t condition;             // the draw's condition, al
    int ticks;
    uint8_t garden_on;              // what the tick writes at +0x19 and +0x1A
    uint8_t floor_on;
    int opens;
    void* open_actor;
    int32_t open_arg;
    uint8_t exit_at_open;           // +0x15 as the door actor's open found it
};
DoorLog g_door;
uint8_t g_house[0x40];
uint8_t* g_house_ptr = g_house;
uint8_t g_door_actor[0x900];
uint8_t g_door_exiting[3];
const uint16_t g_sandoria[] = {230, 231, 232, 280, 0};
const uint16_t g_whitegate[] = {48, 50, 280, 0};
const uint16_t* g_door_lists[11] = {NULL, g_sandoria, g_sandoria, g_sandoria, g_sandoria, g_whitegate,
                                    g_sandoria, g_sandoria, g_sandoria, g_sandoria, NULL};
uint8_t g_door_opened_code[8] = {0xC3};
const uint32_t kFloorWord = 0x30;

uint32_t __cdecl fake_door_gate(int32_t first, int32_t second) {
    logged('G');
    ++g_door.gates;
    g_door.gate_args[0] = first;
    g_door.gate_args[1] = second;
    return g_door.gate_answer;
}

uint32_t __cdecl fake_door_floor() {
    ++g_door.floors;
    return g_door.floor;
}

uint32_t __cdecl fake_door_condition() {
    return g_door.condition;
}

void __fastcall fake_door_tick(void* ctl, void*, int32_t) {
    ++g_door.ticks;
    static_cast<uint8_t*>(ctl)[kDoorGarden] = g_door.garden_on;
    static_cast<uint8_t*>(ctl)[kDoorFloor] = g_door.floor_on;
}

void __fastcall fake_door_open(void* actor, void*, int32_t arg) {
    logged('O');
    ++g_door.opens;
    g_door.open_actor = actor;
    g_door.open_arg = arg;
    g_door.exit_at_open = controller("mogdoor")[kDoorExit];
}

// The NPC dialogue boxes: two controllers, each with its text object, named
// by the game's two globals, its chat log and its list of lines; the text
// dispatch records what it was given last, the text advance, the chat log's
// release and the text object's clear what each of their first two calls
// was given, the page-turn sound its calls.
struct DialogueLog {
    int dispatches;
    void* dispatch_text;
    int32_t dispatch_code;
    int advances;
    void* advance_text[2];
    int sounds;
    int releases;
    void* release_log[2];
    uint32_t release_line[2];
    int clears;
    void* clear_text[2];
    uint8_t wait_at_clear[2];
};
DialogueLog g_talk;
uint8_t g_wait;
uint8_t g_texts[2][kTextBytes];
uint8_t* g_text_objects[2] = {g_texts[0], g_texts[1]};
uint8_t g_log_ctls[2][0x20];
uint8_t g_log_objects[2][0x20];
uint8_t g_messages[4][kMessageBytes];
uint8_t g_nodes[2][8][0x18];
uint8_t g_lines[2][8][kLineBytes];

void __fastcall fake_text_dispatch(void* text, void*, int32_t code) {
    logged('T');
    ++g_talk.dispatches;
    g_talk.dispatch_text = text;
    g_talk.dispatch_code = code;
}

void __fastcall fake_text_advance(void* text, void*) {
    logged('V');
    if (g_talk.advances < 2) {
        g_talk.advance_text[g_talk.advances] = text;
    }
    ++g_talk.advances;
}

void __cdecl fake_page_sound() {
    logged('p');
    ++g_talk.sounds;
}

void __fastcall fake_log_release(void* log, void*, uint32_t line) {
    logged('R');
    if (g_talk.releases < 2) {
        g_talk.release_log[g_talk.releases] = log;
        g_talk.release_line[g_talk.releases] = line;
    }
    ++g_talk.releases;
}

uint32_t __fastcall fake_text_clear(void* text, void*) {
    logged('X');
    if (g_talk.clears < 2) {
        g_talk.clear_text[g_talk.clears] = text;
        g_talk.wait_at_clear[g_talk.clears] = g_wait;
    }
    ++g_talk.clears;
    wr32(static_cast<uint8_t*>(text), kTextCurrent, 0);
    g_wait = 0;
    return 0;
}

// The text-entry windows' routines as the game's behave, as far as the
// engine relies on them: each field's end clears what the window's own
// clears, the starts open fields (comment's on the line given), the open
// hooks of comment, msgline and passinpu (their vtables' +4) start theirs; each
// call a letter in `order` (s a field's end, P passinpu's start, o comment's
// start, M msgline's start, u and b the manager's unbind and bind the engine
// calls itself, f focus, z sanitize, t trim, n the POL number, c the clock,
// S a send, m the mark, C and N the criteria cleared and named, Q the
// search, k the slot test, L link5's selection, e the encoding, R the
// request, I the item) and what it was given.
struct TextLog {
    char order[64];
    int ends[6];
    int keep;
    int starts;
    int start_line;             // comment's start's line, the last
    int deletes;                // fields deleted, and those deleted while bound
    int bound_deletes;
    int kept[6];                // what each field's last end was told
    void* focus_menu;
    uint32_t pol_word;
    int send_mode;
    uint32_t send_args[9];
    char send_text[301];
    char clocks[2][16];         // the clock texts a send was given, as they were then
    int32_t send_result;
    void* mark_obj;
    uint32_t mark_flags;
    void* criteria;
    char criteria_name[17];
    int search_args[6];
    uint32_t create_args[8];
    char create_name[24];
    uint32_t slot_taken;
    uint32_t create_answer;
};
TextLog g_text;
// The text entry's manager as the game's keeps it: +4 the field its last
// bind gave the line, +8 set by every bind, a bind of NULL too, until an
// unbind. Each window's field, in the fields' order, its first byte its own
// bound flag, which the unbind clears and a delete tests before it unbinds.
uint8_t g_line[0x14];
uint8_t* g_line_ptr = g_line;
uint8_t g_text_boxes[6][16];
uint8_t g_pol[0x100];
uint8_t* g_pol_ptr = g_pol;
uint8_t g_msgline_player[8];
uint8_t* g_msgline_player_ptr = g_msgline_player;
uint8_t g_sending[0x40];
uint8_t* g_sending_ptr = g_sending;
uint8_t g_busy;
uint8_t g_search[0x80];
uint8_t* g_search_ptr = g_search;
uint8_t g_made[12];
uint32_t g_item_word = 0x00C0FFEEu;
uint8_t g_link5_rows[4][kLink5RowStride];
uint8_t g_link5_records[4][4];

void text_logged(char c) {
    const size_t n = strlen(g_text.order);
    if (n + 1 < sizeof(g_text.order)) {
        g_text.order[n] = c;
        g_text.order[n + 1] = '\0';
    }
}

void line_unbind() {
    if (!g_line[kLineBound]) {
        return;
    }
    g_line[kLineBound] = 0;
    if (uint8_t* field = rdptr(g_line, kLineField)) {
        field[0] = 0;
    }
}

void line_bind(uint8_t* field) {
    memcpy(g_line + kLineField, &field, 4);
    g_line[kLineBound] = 1;
    if (field) {
        field[0] = 1;
    }
}

void __fastcall fake_unbind(void*, void*) {
    text_logged('u');
    line_unbind();
}

void __fastcall fake_bind(void*, void*, void* field) {
    text_logged('b');
    line_bind(static_cast<uint8_t*>(field));
}

// A field's delete: its destructor unbinds while the field is bound.
void field_deleted(uint8_t* ctl, size_t at) {
    uint8_t* field = rdptr(ctl, at);
    if (!field) {
        return;
    }
    ++g_text.deletes;
    if (field[0]) {
        ++g_text.bound_deletes;
        line_unbind();
    }
    wr32(ctl, at, 0);
}

void text_ended(int f, int keep) {
    text_logged('s');
    ++g_text.ends[f];
    g_text.keep = keep;
    g_text.kept[f] = keep;
}

void __fastcall fake_stringdl_stop(void* ctl, void*) {
    text_ended(kTextStringdl, -1);
    static_cast<uint8_t*>(ctl)[kStringdlEditing] = 0;
    line_unbind();
    line_bind(NULL);
}

void __fastcall fake_comment_stop(void* ctl, void*, int32_t keep) {
    text_ended(kTextComment, keep);
    static_cast<uint8_t*>(ctl)[kCommentEditing] = 0xFF;
    field_deleted(static_cast<uint8_t*>(ctl), kCommentField);
    line_bind(NULL);
}

void __fastcall fake_msgline_stop(void* ctl, void*, int32_t keep) {
    text_ended(kTextMsgline, keep);
    field_deleted(static_cast<uint8_t*>(ctl), kMsglineField);
    line_bind(NULL);
}

void __fastcall fake_scname_stop(void* ctl, void*, int32_t keep) {
    text_ended(kTextScname, keep);
    field_deleted(static_cast<uint8_t*>(ctl), kScnameField);
    line_bind(NULL);
}

void __fastcall fake_link11_stop(void* ctl, void*, int32_t keep) {
    text_ended(kTextLink11, keep);
    uint8_t* c = static_cast<uint8_t*>(ctl);
    if (rd32(c, kLink11Editing) == 1) {
        field_deleted(c, kLink11Field);
        line_bind(NULL);
    }
    wr32(c, kLink11Editing, 0);
}

void __fastcall fake_passinpu_stop(void* ctl, void*, int32_t keep) {
    text_ended(kTextCount, keep);
    field_deleted(static_cast<uint8_t*>(ctl), kPassField);
    line_bind(NULL);
}

// Field f's start in its window's controller: the field there kept, or
// field f's box, then the line taken from the field holding it and given to
// it.
void open_field(uint8_t* ctl, size_t at, int f) {
    ++g_text.starts;
    uint8_t* field = rdptr(ctl, at);
    if (!field) {
        field = g_text_boxes[f];
        field[0] = 0;
        memcpy(ctl + at, &field, 4);
    }
    line_unbind();
    line_bind(field);
}

void __fastcall fake_passinpu_start(void* ctl, void*) {
    text_logged('P');
    open_field(static_cast<uint8_t*>(ctl), kPassField, kTextCount);
}

void __fastcall fake_comment_start(void* ctl, void*, int32_t line) {
    text_logged('o');
    g_text.start_line = line;
    uint8_t* c = static_cast<uint8_t*>(ctl);
    c[kCommentEditing] = static_cast<uint8_t>(line);
    open_field(c, kCommentField, kTextComment);
}

void __fastcall fake_msgline_start(void* ctl, void*) {
    text_logged('M');
    open_field(static_cast<uint8_t*>(ctl), kMsglineField, kTextMsgline);
}

// The open hooks: comment's, msgline's and passinpu's fields started at
// once, comment's on its line 0 but on another player's message.
void __fastcall fake_text_open(void* p, void*) {
    uint8_t* ctl = static_cast<uint8_t*>(p);
    if (ctl == controller("comment") && ctl[kCommentMode] != kCommentView) {
        ctl[kCommentEditing] = 0;
        open_field(ctl, kCommentField, kTextComment);
    } else if (ctl == controller("msgline")) {
        open_field(ctl, kMsglineField, kTextMsgline);
    } else if (ctl == controller("passinpu")) {
        open_field(ctl, kPassField, kTextCount);
    }
}

// The begin-closes: a field still open ended, comment's keeping its line,
// the others dropping their text; stringdl's field is its for good.
void __fastcall fake_text_close(void* p, void*) {
    uint8_t* ctl = static_cast<uint8_t*>(p);
    if (ctl == controller("comment") && rdptr(ctl, kCommentField)) {
        fake_comment_stop(ctl, NULL, 1);
    } else if (ctl == controller("msgline") && rdptr(ctl, kMsglineField)) {
        fake_msgline_stop(ctl, NULL, 0);
    } else if (ctl == controller("scname") && rdptr(ctl, kScnameField)) {
        fake_scname_stop(ctl, NULL, 0);
    } else if (ctl == controller("link11") && rdptr(ctl, kLink11Field)) {
        fake_link11_stop(ctl, NULL, 0);
    } else if (ctl == controller("passinpu") && rdptr(ctl, kPassField)) {
        fake_passinpu_stop(ctl, NULL, 0);
    }
}

// stringdl's start, as a key on its row runs it: +0x164 set, then its field
// given the line.
void stringdl_edits(uint8_t* sd) {
    sd[kStringdlEditing] = 1;
    line_unbind();
    line_bind(rdptr(sd, kStringdlField));
}

void __fastcall fake_focus(void*, void*, void* menu, int32_t) {
    text_logged('f');
    g_text.focus_menu = menu;
}

int32_t __cdecl fake_sanitize(char* out, const char* line) {
    text_logged('z');
    const size_t n = strlen(line);
    memmove(out, line, n + 1);
    return static_cast<int32_t>(n);
}

int32_t __cdecl fake_trim(char* line) {
    text_logged('t');
    return static_cast<int32_t>(strlen(line));
}

uint32_t __fastcall fake_pol_number(void*, void*, uint32_t word) {
    text_logged('n');
    g_text.pol_word = word;
    return 0xAB12CD34u;
}

void __cdecl fake_clock_text(int32_t what, char* out, int32_t size) {
    text_logged('c');
    snprintf(out, size, "clock %d", static_cast<int>(what));
}

int32_t text_sent(int mode, int count, const uintptr_t* args, int text) {
    text_logged('S');
    g_text.send_mode = mode;
    memset(g_text.send_args, 0, sizeof(g_text.send_args));
    for (int i = 0; i < count; ++i) {
        g_text.send_args[i] = static_cast<uint32_t>(args[i]);
    }
    snprintf(g_text.send_text, sizeof(g_text.send_text), "%s", reinterpret_cast<const char*>(args[text]));
    return g_text.send_result;
}

int32_t __cdecl fake_send_tell(uintptr_t a, uintptr_t b, uintptr_t c, uintptr_t d, uintptr_t e, uintptr_t f,
                               uintptr_t g) {
    const uintptr_t v[] = {a, b, c, d, e, f, g};
    return text_sent(0, 7, v, 4);
}

int32_t __cdecl fake_send_reply(uintptr_t a, uintptr_t b, uintptr_t c, uintptr_t d, uintptr_t e, uintptr_t f,
                                uintptr_t g) {
    const uintptr_t v[] = {a, b, c, d, e, f, g};
    return text_sent(1, 7, v, 4);
}

int32_t __cdecl fake_send_short(uintptr_t a, uintptr_t b, uintptr_t c, uintptr_t d, uintptr_t e, uintptr_t f,
                                uintptr_t g) {
    const uintptr_t v[] = {a, b, c, d, e, f, g};
    snprintf(g_text.clocks[0], sizeof(g_text.clocks[0]), "%s", reinterpret_cast<const char*>(d));
    return text_sent(2, 7, v, 2);
}

int32_t __cdecl fake_send_answer(uintptr_t a, uintptr_t b, uintptr_t c, uintptr_t d, uintptr_t e, uintptr_t f,
                                 uintptr_t g, uintptr_t h) {
    const uintptr_t v[] = {a, b, c, d, e, f, g, h};
    return text_sent(5, 8, v, 5);
}

int32_t __cdecl fake_send_dated(uintptr_t a, uintptr_t b, uintptr_t c, uintptr_t d, uintptr_t e, uintptr_t f,
                                uintptr_t g, uintptr_t h, uintptr_t i) {
    const uintptr_t v[] = {a, b, c, d, e, f, g, h, i};
    snprintf(g_text.clocks[0], sizeof(g_text.clocks[0]), "%s", reinterpret_cast<const char*>(f));
    snprintf(g_text.clocks[1], sizeof(g_text.clocks[1]), "%s", reinterpret_cast<const char*>(g));
    return text_sent(7, 9, v, 4);
}

void __fastcall fake_mark(void* obj, void*, uint32_t flags) {
    text_logged('m');
    g_text.mark_obj = obj;
    g_text.mark_flags = flags;
}

void __cdecl fake_criteria_clear(void* criteria) {
    text_logged('C');
    g_text.criteria = criteria;
}

void __cdecl fake_criteria_name(void*, const char* name) {
    text_logged('N');
    snprintf(g_text.criteria_name, sizeof(g_text.criteria_name), "%s", name);
}

void __fastcall fake_search_start(void*, void*, int32_t a, void* criteria, int32_t b, int32_t c, int32_t d,
                                  int32_t f) {
    text_logged('Q');
    const int args[6] = {a, static_cast<int>(reinterpret_cast<uintptr_t>(criteria)), b, c, d, f};
    memcpy(g_text.search_args, args, sizeof(args));
}

uint32_t __cdecl fake_slot_taken(int32_t, int32_t, int32_t, int32_t) {
    text_logged('k');
    return g_text.slot_taken;
}

const uint8_t* __fastcall fake_selected(void* link5, void*) {
    text_logged('L');
    return link5_selected(kPlain, static_cast<uint8_t*>(link5));
}

const char* __cdecl fake_encode(const char* name) {
    static char encoded[24];
    text_logged('e');
    snprintf(encoded, sizeof(encoded), "<%s>", name);
    return encoded;
}

uint32_t __cdecl fake_create(uint32_t slot, uint32_t container, uint32_t index, uint32_t one, const char* encoded,
                             uint32_t r, uint32_t g, uint32_t b) {
    text_logged('R');
    const uint32_t args[8] = {slot, container, index, one,
                              static_cast<uint32_t>(reinterpret_cast<uintptr_t>(encoded)), r, g, b};
    memcpy(g_text.create_args, args, sizeof(args));
    snprintf(g_text.create_name, sizeof(g_text.create_name), "%s", encoded);
    return g_text.create_answer;
}

const uint32_t* __cdecl fake_item(uint32_t, uint32_t) {
    text_logged('I');
    return &g_item_word;
}

void __cdecl fake_sent(int32_t, int32_t, int32_t) {
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
    menu[kMenuDraws] = 1;           // the constructor's
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

// The Asked windows whose templates hold two rows.
bool two_rows(const char* nm) {
    static const char* const names[] = {"sortyn", "evitem01", "confyn", "bankmenu", "mp_dead"};
    for (size_t i = 0; i < sizeof(names) / sizeof(names[0]); ++i) {
        if (strcmp(names[i], nm) == 0) {
            return true;
        }
    }
    return false;
}

uint8_t* live(const char* nm) {
    uint8_t* ctl = controller(nm);
    return ctl ? rdptr(ctl, kCtlMenu) : NULL;
}

void focus(const uint8_t* menu);

int32_t g_open_overlap = -1;    // the last argument the open routine was given last
const size_t kMenuParked = 0x75;

// With the open routine's last argument set, another menu of window n's
// overlap group showing, as the game's test of it finds one: the menu
// opens dormant and the open marks it.
bool group_showing(int n, const uint8_t* self) {
    const uint16_t group = static_cast<uint16_t>(rd16(row(g_e.inv.names[n].rows[0]), kRowOverlap));
    for (int r = 0; group && r < kRowCount; ++r) {
        const uint8_t* m = g_ctl_global[r] ? rdptr(g_ctl_global[r], kCtlMenu) : NULL;
        if (m && m != self && menu_is_open(m) && rd32(m, kMenuState) != kStateDormant
            && (static_cast<uint16_t>(rd16(row(r), kRowOverlap)) & group)) {
            return true;
        }
    }
    return false;
}

void* __fastcall fake_open(void*, void*, const char* key, int, int overlap) {
    ++g_open_calls;
    g_open_overlap = overlap;
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
        // both marks on it; a dormant or closing one stays dormant when it
        // parks.
        const bool revived = menu[kMenuClosing] || rd32(menu, kMenuState) == kStateDormant;
        menu[kMenuClosing] = 0;
        menu[kMenuPendingDestroy] = 0;
        const uint8_t* r = row(g_e.inv.names[n].rows[0]);
        menu[kMenuLayer] = r[kRowLayer];
        wr32(menu, kMenuPolicy, rd32(r, kRowPolicy));
        if (revived && overlap && group_showing(n, menu)) {
            wr32(menu, kMenuState, kStateDormant);
            menu[kMenuParked] = 1;
            return menu;
        }
        focus(menu);
        return menu;
    }
    menu = instantiate(n, ctl);
    if (overlap && group_showing(n, menu)) {
        wr32(menu, kMenuState, kStateDormant);
        menu[kMenuParked] = 1;
        return menu;
    }
    // comment's, msgline's and passinpu's open hooks start their fields.
    if (rdptr(ctl, 0) == reinterpret_cast<const uint8_t*>(vt_text)
        || rdptr(ctl, 0) == reinterpret_cast<const uint8_t*>(vt_pass)) {
        fake_text_open(ctl, NULL);
    }
    // A yes/no window or an Asked window of two rows: the cursor on the
    // first.
    if (rdptr(ctl, 0) == reinterpret_cast<const uint8_t*>(vt_yesno) || two_rows(g_e.inv.names[n].name)) {
        wr16(menu, kMenuItems, 2);
        wr16(menu, kMenuCursor, 1);
    }
    uint32_t args[1] = {reinterpret_cast<uint32_t>(menu)};
    HuFrame f;
    memset(&f, 0, sizeof(f));
    f.args = args;
    f.user = &g_e;
    hook_show_pre(&f);
    focus(menu);
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

// The openers open_set and a list's open run, as the game's do: every open
// through the hooked open routine, in the game's order.
void* open_key(const char* nm) {
    char key[kKeyLen + 1];
    make_key(g_e, name(nm), key);
    return hooked_open(g_mcb, NULL, key, 1, 0);
}

// The ability opener as the daemon presents it: the pre, then unless it
// refused, abisortw with a sort of 1, ability, the kind stored, and with
// `pane` subwindo for a list the stand-in gives a pane (every one but job
// traits, 4), then the post.
int g_ability_lists;
void __cdecl hooked_ability_list(int32_t kind, int32_t sort, int32_t pane) {
    ++g_ability_lists;
    uint32_t args[3] = {static_cast<uint32_t>(kind), static_cast<uint32_t>(sort), static_cast<uint32_t>(pane)};
    HuFrame f;
    memset(&f, 0, sizeof(f));
    f.args = args;
    f.user = &g_e;
    if (!hook_ability_open_pre(&f)) {
        if ((sort & 0xFF) == 1) {
            open_key("abisortw");
        }
        open_key("ability");
        wr32(controller("ability"), kAbilityCategory, static_cast<uint32_t>(kind));
        if ((pane & 0xFF) && kind != 4) {
            open_key("subwindo");
        }
    }
    hook_ability_open_post(&f);
}

// The magic list's opener: magic, the kind stored, then its rebuild's
// opens: with a sort of 1, mgcsortw (blusortw for 6), then subwindo every
// time.
int g_magic_lists;
void __cdecl fake_magic_list(int32_t kind, int32_t sort, int32_t) {
    ++g_magic_lists;
    open_key("magic");
    wr32(controller("magic"), kAbilityCategory, static_cast<uint32_t>(kind));
    if ((sort & 0xFF) == 1) {
        open_key(kind == 6 ? "blusortw" : "mgcsortw");
    }
    open_key("subwindo");
}

// The equipment set's opener, `this` equip's controller, which it never
// reads: its guard (equip's controller from the slot, NULL or its +0x1A
// word set, and nothing opens), persona's mode 2, persona, money, inventor
// with iteminfo, then equip.
int g_equipment_opens;
void __fastcall fake_equipment(void* ctl, void*) {
    ++g_equipment_opens;
    g_log.equipment_this = ctl;
    const uint8_t* equip = controller("equip");
    if (!equip || rd16(equip, 0x1A) != 0) {
        return;
    }
    controller("persona")[0x1C] = 2;
    open_key("persona");
    open_key("money");
    open_key("inventor");
    open_key("iteminfo");
    open_key("equip");
}

// The Monstrosity check: al alone is the answer, the rest of eax the
// player data pointer's bytes.
uint32_t g_monstrosity;
uint32_t __cdecl fake_monstrosity() {
    return 0x0BAD0000u | g_monstrosity;
}

// The other sets' openers, each opening its windows in the game's order
// unless its own test refuses, and what they were given; the checks asked
// first answer in al alone.
struct SetLog {
    int calls;
    int32_t args[3];
    const void* self;
    bool refuse;            // the opener's own test refuses: nothing opens
    uint32_t check;         // the linkshell row's and the item use's checks' al
    int checks;
    int32_t check_args[2];
};
SetLog g_set;

void set_called(const void* self, int32_t a, int32_t b, int32_t c) {
    ++g_set.calls;
    g_set.self = self;
    g_set.args[0] = a;
    g_set.args[1] = b;
    g_set.args[2] = c;
}

void __cdecl fake_map_open(int32_t mode) {
    set_called(NULL, mode, 0, 0);
    if (!g_set.refuse) {
        open_key("mapframe");
        open_key("map0");
    }
}

void __fastcall fake_set_spells(void* ctl, void*) {
    set_called(ctl, 0, 0, 0);
    if (!g_set.refuse) {
        controller("persona")[0x1C] = 2;
        open_key("persona");
        open_key("bluehelp");
        open_key("bluinven");
        open_key("bluepoin");
        open_key("bluequip");
    }
}

void __fastcall fake_attachments(void* ctl, void*) {
    set_called(ctl, 0, 0, 0);
    if (!g_set.refuse) {
        open_key("bluehelp");
        open_key("bluinven");
        open_key("statpup");
        open_key("automato");
    }
}

uint32_t __cdecl fake_set_check(int32_t first, int32_t second) {
    ++g_set.checks;
    g_set.check_args[0] = first;
    g_set.check_args[1] = second;
    return 0x0BAD0000u | g_set.check;
}

void __fastcall fake_linkshell(void* ctl, void*, int32_t mode, int32_t a, int32_t b) {
    set_called(ctl, mode, a, b);
    open_key("link6");
    open_key("link5");
}

void __cdecl fake_instincts() {
    set_called(NULL, 0, 0, 0);
    open_key("mp_stat");
    open_key("level");
    open_key("mp_cstm");
    open_key("mp_mncst");
}

uint32_t __fastcall fake_synthesis(void* ctl, void*, uint32_t index, uint32_t id, uint32_t mode) {
    set_called(ctl, static_cast<int32_t>(index), static_cast<int32_t>(id), static_cast<int32_t>(mode));
    if (g_set.refuse) {
        return 0x0BAD0000u;
    }
    open_key("inventor");
    open_key("iteminfo");
    open_key("tskill1");
    return 0x0BAD0001u;
}

void __fastcall fake_tabula(void* ctl, void*, uint32_t mode, uint32_t index) {
    set_called(ctl, static_cast<int32_t>(mode), static_cast<int32_t>(index), 0);
    open_key("prvdungp");
    open_key("bluehelp");
    open_key("bluinven");
}

void __fastcall fake_macros(void* editor, void*) {
    set_called(editor, 0, 0, 0);
    open_key("mcrbedit");
    open_key("mcr2edit");
    open_key("mcr1edit");
}

// The item data the item use reads a type from: its pointer to the type
// word kSetItemType in; 4096 a crystal, 28672 an M Maze Tabula, 28704 an R
// one, 9999 no data, any other item of type 1.
const uint32_t kSetItemType = 0x40;
const uint16_t kSetTypes[4] = {kItemCrystal, kItemTabulaM, kItemTabulaR, 1};
uint8_t g_set_data[4][0x80];
uint8_t g_macro_editor[0x10];
uint8_t* g_macro_editor_ptr = g_macro_editor;
// A list of the game's, as inventor's controller holds it at kInventorList:
// an object whose table's first routine is code.
const size_t kInventorList = 0x5C;
const void* const kListTable[1] = {reinterpret_cast<const void*>(&fake_macros)};
const void* const kList[1] = {kListTable};

// The routes' stand-ins: each opens what the game's does through the hooked
// open, writes what the game's writes, and records its `this` and the
// arguments it was handed; the tests' answers stand for the game's tests.
struct RouteLog {
    int calls;
    char last[24];          // the stand-in last run
    const void* self;
    int32_t args[7];
    uint32_t msglist;       // al of the mail list's opener
    uint32_t job_test;      // al of the Job Points test
    uint32_t expansion;     // bit mask of the expansions on
    int32_t tracking;       // the map's tracking word
    bool no_tracking;       // the tracking routine answers NULL
    char order[160];        // the stand-ins run, names joined by ' '
    uint8_t bag_size[18];   // each bag's size, slot 0 counted
};
RouteLog g_route;
char g_route_key[kKeyLen + 1] = "menu    jobcselu";
const size_t kStorageMode = 0x14;
const size_t kCnqKindAt = 0x160;
const int32_t kNation = 2;

void route_called(const char* what, const void* self, int count, const int32_t* args) {
    ++g_route.calls;
    snprintf(g_route.last, sizeof(g_route.last), "%s", what);
    const size_t used = strlen(g_route.order);
    snprintf(g_route.order + used, sizeof(g_route.order) - used, "%s%s", used ? " " : "", what);
    g_route.self = self;
    memset(g_route.args, 0, sizeof(g_route.args));
    for (int i = 0; i < count && i < 7; ++i) {
        g_route.args[i] = args[i];
    }
}

void route_one(const char* what, const void* self, int32_t a) {
    route_called(what, self, 1, &a);
}

void route_two(const char* what, const void* self, int32_t a, int32_t b) {
    const int32_t args[2] = {a, b};
    route_called(what, self, 2, args);
}

void close_key(const char* nm) {
    char key[kKeyLen + 1];
    make_key(g_e, name(nm), key);
    g_e.game.close(g_mcb, NULL, key);
}

void __fastcall fake_storage(void* ctl, void*, uint32_t mode) {
    route_one("storage", ctl, static_cast<int32_t>(mode));
    wr32(static_cast<uint8_t*>(ctl), kStorageMode, mode);
    open_key(mode ? "storage2" : "storage");
}

void __fastcall fake_mogpost(void* ctl, void*, uint32_t mode) {
    route_one("mogpost", ctl, static_cast<int32_t>(mode));
    wr32(static_cast<uint8_t*>(ctl), kStorageMode, mode);
    open_key("mogpost");
}

const char* __cdecl fake_language_key(int32_t which) {
    route_one("language_key", NULL, which);
    return g_route_key;
}

void __fastcall fake_friend(void* ctl, void*, uint32_t value) {
    route_one("friend", ctl, static_cast<int32_t>(value));
    open_key("friend");
    open_key("titlehan");
}

uint32_t __fastcall fake_msglist(void* ctl, void*, int32_t a, int32_t b, int32_t c) {
    const int32_t args[3] = {a, b, c};
    route_called("msglist", ctl, 3, args);
    if (!g_route.msglist) {
        return 0x0BAD0000u;
    }
    open_key("msglist");
    open_key("titlehan");
    return 0x0BAD0001u;
}

void __fastcall fake_blklist(void* ctl, void*) {
    route_called("blklist", ctl, 0, NULL);
    open_key("blklist");
}

void __cdecl fake_mount(int32_t list, int32_t sort) {
    route_two("mount", NULL, list, sort);
    if ((sort & 0xFF) == 1) {
        open_key("abisortw");
    }
    open_key("mount");
    open_key("subwindo");
}

uint32_t __stdcall fake_job_test(int32_t job) {
    route_one("job_test", NULL, job);
    return 0x0BAD0000u | g_route.job_test;
}

void __fastcall fake_job_points(void* ctl, void*, uint32_t job) {
    route_one("job_points", ctl, static_cast<int32_t>(job));
    open_key("jbpcat");
}

void __fastcall fake_fp_cat2(void* ctl, void*, uint32_t category) {
    route_one("fp_cat2", ctl, static_cast<int32_t>(category));
    open_key("fp_cat2");
}

void __fastcall fake_search_reset(void* ctl, void*, uint32_t flag) {
    route_one("search_reset", ctl, static_cast<int32_t>(flag));
}

void __fastcall fake_comgenre(void* ctl, void*, uint32_t mode) {
    route_one("comgenre", ctl, static_cast<int32_t>(mode));
    if (mode < 3) {
        open_key("comgenre");
    }
}

void __fastcall fake_sccompar(void* ctl, void*, int32_t mode, int32_t base) {
    route_two("sccompar", ctl, mode, base);
    open_key("sccompar");
}

void __fastcall fake_sccomite(void* ctl, void*, int32_t mode, int32_t base) {
    route_two("sccomite", ctl, mode, base);
    open_key("sccomite");
}

void __fastcall fake_sccomls(void* ctl, void*, int32_t mode, int32_t base) {
    route_two("sccomls", ctl, mode, base);
    open_key("sccomls");
}

// The item panel's set-and-open: the panel its controller is, and for
// itemxinf its OnOpen's itemyinf.
void __fastcall fake_item_panel(void* ctl, void*, int32_t mode, int32_t id, int32_t slot, int32_t bag,
                                const void* extra, const void* beside, const void* under) {
    const int32_t args[7] = {mode, id, slot, bag, static_cast<int32_t>(reinterpret_cast<uintptr_t>(extra)),
                             static_cast<int32_t>(reinterpret_cast<uintptr_t>(beside)),
                             static_cast<int32_t>(reinterpret_cast<uintptr_t>(under))};
    route_called("item_panel", ctl, 7, args);
    const bool x = ctl == controller("itemxinf");
    open_key(x ? "itemxinf" : "iteminfo");
    if (x) {
        open_key("itemyinf");
    }
}

void __fastcall fake_casttime(void* ctl, void*, int32_t kind, int32_t ticks) {
    route_two("casttime", ctl, kind, ticks);
    open_key("casttime");
}

void __fastcall fake_eventtim(void* ctl, void*, int32_t kind, int32_t ticks) {
    route_two("eventtim", ctl, kind, ticks);
    open_key("eventtim");
}

// mp_stat's opener: the mode at +0x14, then mp_stat, whose open opens level
// and statcom2 for 0.
void __fastcall fake_mp_stat(void* ctl, void*, uint32_t mode) {
    route_one("mp_stat", ctl, static_cast<int32_t>(mode));
    wr32(static_cast<uint8_t*>(ctl), kStorageMode, mode);
    open_key("mp_stat");
    if (mode == 0) {
        open_key("level");
        open_key("statcom2");
    }
}

uint32_t __fastcall fake_quest00_dim(void* ctl, void*) {
    route_called("quest00_dim", ctl, 0, NULL);
    return 0;
}

uint32_t __fastcall fake_quest00_dim2(void* ctl, void*) {
    route_called("quest00_dim2", ctl, 0, NULL);
    return 0;
}

void __fastcall fake_quest01(void* ctl, void*, uint32_t list) {
    route_one("quest01", ctl, static_cast<int32_t>(list));
}

// The missions menu's setter: 0 the player's nation's log, as the game's
// default case writes it.
const size_t kMissionLog = 0x70;
void __fastcall fake_miss00_menu(void* ctl, void*, uint32_t value) {
    route_one("miss00_menu", ctl, static_cast<int32_t>(value));
    if (value == 0) {
        wr32(static_cast<uint8_t*>(ctl), kMissionLog, kNation);
    }
}

uint32_t __cdecl fake_expansion(int32_t bit) {
    route_one("expansion", NULL, bit);
    return 0x0BAD0000u | ((g_route.expansion >> bit) & 1);
}

const uint8_t kMissionBits[15] = {0, 0, 0, 1, 2, 3, 3, 4, 4, 0xb, 0, 0, 5, 6, 7};

const int32_t* __cdecl fake_tracking() {
    route_called("tracking", NULL, 0, NULL);
    return g_route.no_tracking ? NULL : &g_route.tracking;
}

void __fastcall fake_scanlist(void* ctl, void*) {
    route_called("scanlist", ctl, 0, NULL);
    open_key("scanlist");
}

// The Region Info opener: the kind at +0x160, then cnqframe and the kind's
// points window.
void __fastcall fake_region(void* ctl, void*, int32_t kind, int32_t area) {
    route_two("region", ctl, kind, area);
    wr32(static_cast<uint8_t*>(ctl), kCnqKindAt, static_cast<uint32_t>(kind));
    open_key("cnqframe");
    open_key(kRegionPoints[kind]);
}

// The macro editor's pick: the row stored for the palette in use, the
// palettes closed, then mcrledit and macro.
void __fastcall fake_macro_pick(void* editor, void*, uint32_t row) {
    route_one("macro_pick", editor, static_cast<int32_t>(row));
    uint8_t* e = static_cast<uint8_t*>(editor);
    e[e[0x0C] == 1 ? 0x0D : 0x0E] = static_cast<uint8_t>(row);
    close_key("mcrbedit");
    close_key("mcr1edit");
    close_key("mcr2edit");
    open_key("mcrledit");
    open_key("macro");
}

uint32_t __fastcall fake_macro_close(void* editor, void*) {
    route_called("macro_close", editor, 0, NULL);
    close_key("mcrbedit");
    close_key("mcr1edit");
    close_key("mcr2edit");
    return 0;
}

// The palette switch: the row for both palettes, the palette toggled, the
// other palette's window opened.
void __fastcall fake_macro_switch(void* editor, void*, uint32_t row) {
    route_one("macro_switch", editor, static_cast<int32_t>(row));
    uint8_t* e = static_cast<uint8_t*>(editor);
    e[0x0D] = static_cast<uint8_t>(row);
    e[0x0E] = static_cast<uint8_t>(row);
    const bool ctrl = e[0x0C] == 1;
    e[0x0C] = ctrl ? 2 : 1;
    open_key(ctrl ? "mcr2edit" : "mcr1edit");
}

uint32_t __cdecl fake_bag_size(int32_t bag) {
    return bag >= 0 && bag < 18 ? g_route.bag_size[bag] : 0xFF;
}

// statcom2's begin-close: persona and mp_stat closed by name.
uint32_t __fastcall fake_status_begin_close(void* ctl, void*) {
    route_called("status_close", ctl, 0, NULL);
    close_key("persona");
    close_key("mp_stat");
    return 0;
}

// mp_mncst's Escape: mp_stat, mp_cstm and mp_mncst closed by name.
void __cdecl fake_instincts_escape() {
    route_called("instincts_escape", NULL, 0, NULL);
    close_key("mp_stat");
    close_key("mp_cstm");
    close_key("mp_mncst");
}

// link5's begin-close: link1 closed by name, as the game's closes its
// submenus.
const void* vt_link5[0x20];
uint32_t __fastcall fake_link5_begin_close(void* ctl, void*) {
    route_called("link5_close", ctl, 0, NULL);
    close_key("link1");
    return 0;
}

// cnqframe's input as the engine reads it: its Escape's keys, each at its
// close's push; and the fade's start, its closing bit set.
uint8_t g_region_input[kRegionEscapeAt + kRegionEscapeStep * kRegionEscapeCloses + 3];
char g_region_keys[kRegionEscapeCloses][kKeyLen + 1];
const void* vt_region[0x20];

void __fastcall fake_region_fade(void* ctl, void*) {
    route_called("region_fade", ctl, 0, NULL);
    static_cast<uint8_t*>(ctl)[kRegionClosingWord] |= kRegionClosing;
}

// The Wide Scan question's callback (cdecl; context, answer).
int32_t g_scan_answer = -1;
void __cdecl fake_scan_question(int32_t, int32_t answer) {
    g_scan_answer = answer;
}

void __fastcall fake_title(void* ctl, void*, int32_t a, int32_t b) {
    route_two("title", ctl, a, b);
}

int32_t g_view_args[2];
const void* g_view_self;
void __fastcall fake_items_view(void* object, void*, int32_t a, int32_t b) {
    route_two("items_view", object, a, b);
    g_view_args[0] = a;
    g_view_args[1] = b;
    g_view_self = object;
}

// inventor's open with a list: the list kept, then inventor with iteminfo.
const size_t kItemsByte = 0x90;
void __fastcall fake_inventor_open(void* ctl, void*, uint32_t list) {
    route_one("inventor_open", ctl, static_cast<int32_t>(list));
    wr32(static_cast<uint8_t*>(ctl), kInventorList, list);
    open_key("inventor");
    open_key("iteminfo");
}

uint8_t g_items_object[0x20];
uint8_t* g_items_object_ptr = g_items_object;
uint8_t g_manager[4];                  // stands for the Mog House's and the Nomad Moogle service's managers
uint8_t* g_mog_house_ptr;
uint8_t* g_nomad_ptr;
uint8_t g_route_player[0x80];
uint8_t* g_route_player_ptr = g_route_player;
const uint32_t kRouteArea = 0x40;
// The list the Items row hands inventor: the game's, or one whose table's
// first routine is outside the code.
const void* const kBadTable[1] = {NULL};
const void* const kBadList[1] = {kBadTable};
uint8_t* g_policy = reinterpret_cast<uint8_t*>(const_cast<void**>(kList));

uint8_t* __fastcall fake_set_item_data(void*, void*, uint32_t id) {
    if (id == 9999) {
        return NULL;
    }
    uint8_t* data = g_set_data[id == 4096 ? 0 : id == 28672 ? 1 : id == 28704 ? 2 : 3];
    const uint16_t* type = &kSetTypes[data == g_set_data[0] ? 0 : data == g_set_data[1] ? 1 : data == g_set_data[2] ? 2
        : 3];
    memcpy(data + kSetItemType, &type, 4);
    return data;
}

int __fastcall fake_close(void*, void*, const char* key) {
    ++g_close_calls;
    if (g_close_refuses) {
        return 0;
    }
    logged('K');
    memcpy(g_log.last_close, key, 16);
    g_log.last_close[16] = '\0';
    const size_t used = strlen(g_log.closes);
    if (used + 10 <= sizeof(g_log.closes)) {
        memcpy(g_log.closes + used, key + 8, 8);
        g_log.closes[used + 8] = '|';
        g_log.closes[used + 9] = '\0';
    }
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

// The map's close (this = mapframe's controller): map0, mapv3, mapv2,
// scanlist and mapscan closed by name, as the game's; the frame's fade-out,
// and its close after it, are the game's own and not modeled.
struct MapCloseLog {
    int calls;
    const void* frame;
    int32_t flag;
    bool map0_open;             // map0 still open when it ran
};
MapCloseLog g_map_close;

void __fastcall fake_map_close(void* frame, void*, int32_t flag) {
    logged('M');
    ++g_map_close.calls;
    g_map_close.frame = frame;
    g_map_close.flag = flag;
    g_map_close.map0_open = menu_is_open(live("map0"));
    const char* keys[] = {"menu    map0    ", "menu    mapv3   ", "menu    mapv2   ", "menu    scanlist",
                          "menu    mapscan "};
    for (size_t i = 0; i < sizeof(keys) / sizeof(keys[0]); ++i) {
        fake_close(g_mcb, NULL, keys[i]);
    }
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

// The show path revealing it again: the pre on the dormant menu, then the
// routine's own state change.
void reveal(uint8_t* menu) {
    uint32_t args[1] = {reinterpret_cast<uint32_t>(menu)};
    HuFrame f;
    memset(&f, 0, sizeof(f));
    f.args = args;
    f.user = &g_e;
    hook_show_pre(&f);
    wr32(menu, kMenuState, 0);
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

// The player's input on `menu` as the active menu, put back to none after: 1
// when a routed key reached its sink and the mouse mode picker's original
// ran (mode 2), as on a window shown; 0 when the key was dropped and the pre
// held world mode; -1 for anything else.
int input_on(const uint8_t* menu) {
    wr32(g_mcb, kMcbActive, reinterpret_cast<uint32_t>(menu));
    const int sinks = g_sink_originals;
    const int pickers = g_picker_originals;
    route(5);
    uint8_t mouse[0x60];
    memset(mouse, 0, sizeof(mouse));
    mouse[kMouseMode] = 2;
    hooked_mouse_mode(mouse);
    wr32(g_mcb, kMcbActive, 0);
    if (g_sink_originals == sinks + 1 && g_sink_menu == menu && g_picker_originals == pickers + 1
        && mouse[kMouseMode] == 2) {
        return 1;
    }
    if (g_sink_originals == sinks && g_picker_originals == pickers && mouse[kMouseMode] == kMouseWorld) {
        return 0;
    }
    return -1;
}

// The device's draws through the draw primitive, every one counted.
int g_pixels;

// One call of the draw primitive as the daemon presents it: 1 when it
// reached the device, 0 when the pre answered D3D_OK in its place, -1 for
// any other answer.
int primitive() {
    HuFrame f;
    memset(&f, 0, sizeof(f));
    f.result = 0xDEAD;
    f.user = &g_e;
    if (hook_primitive_pre(&f)) {
        return f.result == 0 ? 0 : -1;
    }
    ++g_pixels;
    return 1;
}

// The per-menu draw as the daemon presents it: the pre, then the original,
// one quad through the draw primitive, and the post unless the pre ran
// instead. True when its quad reached the device.
bool draw_paints(const uint8_t* menu) {
    HuFrame f;
    memset(&f, 0, sizeof(f));
    f.ecx = reinterpret_cast<uint32_t>(menu);
    f.user = &g_e;
    if (hook_menu_draw_pre(&f)) {
        return false;
    }
    const int drawn = primitive();
    hook_menu_draw_post(&f);
    return drawn == 1;
}

// The game's close's two countdowns on a dormant menu, which the per-menu
// draw's pre-pass runs out, +0x66 first.
const size_t kCloseFirst = 0x66;
const size_t kCloseSecond = 0x64;
// The parts the per-menu draw puts the cursor's row on, at +8; conq3pts'
// data, gone while set: its controller's draw then closes it.
uint8_t g_parts[16][0x40];
bool g_conq_gone;

// The game's close by name and the staged close it ends in, as the game's
// leaves a menu: dormant, its two countdowns at 2 and 1.
void game_closes_drawn(const char* nm) {
    uint8_t* menu = live(nm);
    const bool dormant = menu && rd32(menu, kMenuState) == kStateDormant;
    game_closes(nm);
    if (menu && !dormant && menu[kMenuClosing]) {
        wr32(menu, kMenuState, kStateDormant);
        wr16(menu, kCloseFirst, 2);
        wr16(menu, kCloseSecond, 1);
    }
}

// The per-menu draw as the game's runs it, through the hooks as the daemon
// presents them: the pre-pass's end, on a dormant menu its close's
// countdowns, +0x66 then +0x64, one a frame, then +0x70 once both ran out
// with +0x6E set; the input wait +0x56 counted down; the parts loop's +8,
// the part of the cursor's row; the frame's quad; then, while it draws its
// controller (+0x6A set, not dormant), conq3pts' controller's draw, which
// closes its window by name while its data is gone, as the game's does, and
// draws its content's quad otherwise. Every quad goes through the draw
// primitive.
void game_draw(uint8_t* menu) {
    HuFrame f;
    memset(&f, 0, sizeof(f));
    f.ecx = reinterpret_cast<uint32_t>(menu);
    f.user = &g_e;
    if (hook_menu_draw_pre(&f)) {
        return;
    }
    if (rd32(menu, kMenuState) == kStateDormant) {
        if (rd16(menu, kCloseFirst) > 0) {
            wr16(menu, kCloseFirst, rd16(menu, kCloseFirst) - 1);
        } else if (rd16(menu, kCloseSecond) > 0) {
            wr16(menu, kCloseSecond, rd16(menu, kCloseSecond) - 1);
        } else if (menu[kMenuClosing]) {
            menu[kMenuPendingDestroy] = 1;
        }
    }
    if (rd16(menu, kMenuInputWait) > 0) {
        wr16(menu, kMenuInputWait, rd16(menu, kMenuInputWait) - 1);
    }
    const uint8_t* part = g_parts[rd16(menu, kMenuCursor) & 15];
    memcpy(menu + 8, &part, 4);
    primitive();
    if (menu[kMenuDraws] && rd32(menu, kMenuState) != kStateDormant && menu == live("conq3pts")) {
        if (g_conq_gone) {
            game_closes_drawn("conq3pts");
        } else {
            primitive();
        }
    }
    hook_menu_draw_post(&f);
}

// One of the game's frames for window nm: the update, the drain first, then
// the manager freeing its instance once its close ended (+0x70), then the
// walk's draw of it.
void game_frame(const char* nm) {
    drain(g_e);
    uint8_t* menu = live(nm);
    if (menu && menu[kMenuPendingDestroy]) {
        destroy(nm);
        menu = NULL;
    }
    if (menu) {
        game_draw(menu);
    }
}

// The manager's list as the game orders it for the active menu. Each open
// focuses its menu; the next update, after the drain, relinks every focused
// menu in focus order by the draw layer it has then (0x1015D9A0): one of
// layer 5 or more to the tail, any other after the last menu below 5. The
// pick (0x1015E670) walks from the tail to the first menu that is neither
// passive (policy bit 1, +0x98 clear), closing, marked +0x70 nor dormant.
struct Linked {
    const uint8_t* menu;
    bool high;              // layer 5 or more at its relink
    int focus;              // its focus since the last relink, in order; 0 for none
};
Linked g_linked[512];
int g_linked_count;
int g_focuses;

void focus(const uint8_t* menu) {
    int i = 0;
    while (i < g_linked_count && g_linked[i].menu != menu) {
        ++i;
    }
    if (i == g_linked_count) {
        g_linked[g_linked_count].menu = menu;
        g_linked[g_linked_count].high = false;
        ++g_linked_count;
    }
    g_linked[i].focus = ++g_focuses;
}

bool bound(const uint8_t* menu) {
    for (int r = 0; r < kRowCount; ++r) {
        if (g_ctl_global[r] && rdptr(g_ctl_global[r], kCtlMenu) == menu) {
            return true;
        }
    }
    return false;
}

// The update's relink, then its pick, written to the manager's +0x54 as the
// game's is.
const uint8_t* game_active() {
    for (;;) {
        int next = -1;
        for (int i = 0; i < g_linked_count; ++i) {
            if (g_linked[i].focus && (next < 0 || g_linked[i].focus < g_linked[next].focus)) {
                next = i;
            }
        }
        if (next < 0) {
            break;
        }
        Linked l = g_linked[next];
        l.focus = 0;
        l.high = l.menu[kMenuLayer] >= 5;
        for (int k = next; k + 1 < g_linked_count; ++k) {
            g_linked[k] = g_linked[k + 1];
        }
        int at = g_linked_count - 1;
        while (!l.high && at > 0 && g_linked[at - 1].high) {
            --at;
        }
        for (int k = g_linked_count - 1; k > at; --k) {
            g_linked[k] = g_linked[k - 1];
        }
        g_linked[at] = l;
    }
    g_focuses = 0;
    const uint8_t* active = NULL;
    for (int i = g_linked_count - 1; i >= 0 && !active; --i) {
        const uint8_t* m = g_linked[i].menu;
        const bool passive = (rd32(m, kMenuPolicy) & 1) && m[0x98] == 0;
        if (bound(m) && !passive && !m[kMenuClosing] && !m[kMenuPendingDestroy]
            && rd32(m, kMenuState) != kStateDormant) {
            active = m;
        }
    }
    wr32(g_mcb, kMcbActive, reinterpret_cast<uint32_t>(active));
    return active;
}

// A call rel32 at `at` to `target`.
void put_call(uint8_t* at, const void* target) {
    at[0] = 0xE8;
    const int32_t rel = static_cast<int32_t>(reinterpret_cast<uintptr_t>(target)
        - (reinterpret_cast<uintptr_t>(at) + 5));
    memcpy(at + 1, &rel, 4);
}

// The game's own open of window nm, with the last argument its openers pass.
uint8_t* open_now(const char* nm) {
    char key[kKeyLen + 1];
    make_key(g_e, name(nm), key);
    return static_cast<uint8_t*>(hooked_open(g_mcb, NULL, key, 1, open_overlap(nm)));
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

// A decoded text's runs as "text/escape:color|...".
const char* runs_of(const GlyphText& t) {
    static char out[1024];
    size_t used = 0;
    out[0] = '\0';
    for (int i = 0; i < t.run_count && used < sizeof(out); ++i) {
        const GlyphRun& r = t.runs[i];
        used += snprintf(out + used, sizeof(out) - used, "%s%.*s/%02X:%X", i ? "|" : "", r.length,
            t.text + r.start, r.escape, r.color);
    }
    return out;
}

// The trade windows' world: the player's data at offsets of the test's own
// (the offer tables, the inventory, the throttle's last sends), the
// throttle's intervals, the game's clock as its object holds it, the item
// cache's data, each window's inventor mode and its gray test, the actor
// table, and what each stand-in routine was handed.
const uint32_t kPdOwn = 0x100;
const uint32_t kPdTheirs = 0x200;
const uint32_t kPdInventory = 0x400;
const uint32_t kPdLastSend = 0x40;
const uint32_t kIntervalAt = 0x10;
const uint32_t kItemLock = 0x1AF;
uint8_t g_pd[0x1400];
uint8_t* g_pd_ptr = g_pd;
uint8_t g_intervals[0x200];
uint8_t* g_intervals_ptr = g_intervals;
uint8_t g_clock_object[0x20];
uint8_t* g_clock_ptr = g_clock_object;
uint8_t g_item_data[0x200];
uint8_t g_cache_object[4];
uint8_t* g_cache_ptr = g_cache_object;
const void* vt_mode[2][16];
uint8_t g_mode[2][4];
uint8_t* g_mode_ptr[2] = {g_mode[0], g_mode[1]};
uint8_t* g_status_mode_ptr;
uint8_t g_actor[2][0x240];
const uint8_t* g_actors[0x10];

struct OfferLog {
    int stages[2];
    uint32_t stage_args[3];
    int16_t stage_row;              // the trade menu's cursor row at the stage
    uint8_t stage_slot;             // handover's +0x40 at the stage
    int sends;                      // trade changes the stand-in send let through
    uint32_t sent[3];               // index, count, slot
    int oks[2];
    uint32_t ok_answer[2];          // 1: sent; else nothing
    int cancels;
    uint32_t cancel_answer;
    int closes[2];
    int tests;
    uint8_t lock_at_test;
    uint32_t test_answer;           // the mode's test's color
    uint32_t describe_seen;
};
OfferLog g_offer;

void set_clock(uint32_t ms) {
    wr32(g_clock_object, kClockSeconds, ms / 1000);
    wr32(g_clock_object, kClockMs, ms % 1000);
}

uint32_t clock_now() {
    return rd32(g_clock_object, kClockSeconds) * 1000 + rd32(g_clock_object, kClockMs);
}

// The allocator's throttle for packet `id`, as the game's runs it.
bool fake_throttle(uint32_t id) {
    const uint32_t last = rd32(g_pd, kPdLastSend + 4 * id);
    const uint32_t interval = rd32(g_intervals, kIntervalAt + 4 * id);
    if (last && clock_now() <= last + interval) {
        return false;
    }
    wr32(g_pd, kPdLastSend + 4 * id, clock_now());
    return true;
}

void put_item(int index, uint16_t id, uint32_t count, uint8_t status) {
    uint8_t* r = g_pd + kPdInventory + index * kItemRecordBytes;
    memset(r, 0, kItemRecordBytes);
    wr16(r, 0, id);
    r[2] = static_cast<uint8_t>(index);
    wr32(r, kItemCount, count);
    r[kItemStatus] = status;
}

void put_own(int slot, uint16_t id, uint8_t index, uint32_t count) {
    uint8_t* e = g_pd + kPdOwn + slot * kOwnEntry;
    wr32(e, 0, count);
    wr16(e, kOwnId, id);
    e[kOwnIndex] = index;
}

void put_theirs(int slot, uint16_t id, uint32_t count) {
    uint8_t* e = g_pd + kPdTheirs + slot * kTheirEntry;
    wr16(e, 0, id);
    e[2] = static_cast<uint8_t>(slot);
    wr32(e, 4, count);
}

void fake_send(uint32_t index, uint32_t count, uint32_t slot) {
    if (!fake_throttle(kSendTradeChange)) {
        return;
    }
    ++g_offer.sends;
    g_offer.sent[0] = index;
    g_offer.sent[1] = count;
    g_offer.sent[2] = slot;
}

// The trade's stage as the game's runs it: the slot by the cursor's row,
// the gil on row 11; an item already in a slot of the acknowledged offer
// goes to that slot, where the same index takes it out; then both states 3.
void __fastcall fake_trade_stage(void* ctl, void*, uint32_t index, uint32_t id, uint32_t count) {
    uint8_t* c = static_cast<uint8_t*>(ctl);
    ++g_offer.stages[kOfferTrade];
    g_offer.stage_args[0] = index;
    g_offer.stage_args[1] = id;
    g_offer.stage_args[2] = count;
    const int16_t row = rd16(rdptr(c, kCtlMenu), kMenuCursor);
    g_offer.stage_row = row;
    int slot = row >= 1 && row <= kOfferSlots ? row : row == kTradeGilRow ? 0 : -1;
    if (slot < 0) {
        return;
    }
    if (slot == 0) {
        if (index == 0 && id == kGilId) {
            fake_send(0, count, 0);
            wr32(c, kTradeState, 3);
            wr32(c, kTradeTheirState, 3);
        }
        return;
    }
    for (int k = 1; k < kOfferTable; ++k) {
        if (g_pd[kPdOwn + k * kOwnEntry + kOwnIndex] == index) {
            slot = k;
        }
    }
    const uint8_t* e = g_pd + kPdOwn + slot * kOwnEntry;
    const bool removal = rd16(e, kOwnId) != 0 && e[kOwnIndex] == index;
    fake_send(index, removal ? 0 : count, slot);
    wr32(c, kTradeState, 3);
    wr32(c, kTradeTheirState, 3);
}

// The NPC trade's stage as the game's: index 0 the gil; else every entry
// with the item cleared, returning when that was +0x40's, then +0x40's
// entry the item.
void __fastcall fake_handover_stage(void* ctl, void*, uint32_t index, uint32_t id, uint32_t count) {
    uint8_t* c = static_cast<uint8_t*>(ctl);
    ++g_offer.stages[kOfferHandover];
    g_offer.stage_args[0] = index;
    g_offer.stage_args[1] = id;
    g_offer.stage_args[2] = count;
    g_offer.stage_slot = c[kHandoverRow];
    if ((index & 0xFF) == 0) {
        wr32(c, kHandoverGil, count);
        return;
    }
    for (int k = 0; k < kOfferTable; ++k) {
        uint8_t* e = c + kHandoverEntries + k * kHandoverEntry;
        if (e[2] == (index & 0xFF) && static_cast<uint16_t>(rd16(e, 0)) == (id & 0xFFFF)) {
            memset(e, 0, kHandoverEntry);
            if (c[kHandoverRow] == k) {
                return;
            }
        }
    }
    uint8_t* e = c + kHandoverEntries + c[kHandoverRow] * kHandoverEntry;
    wr16(e, 0, static_cast<uint16_t>(id));
    e[2] = static_cast<uint8_t>(index);
    e[3] = static_cast<uint8_t>(count);
}

uint32_t __fastcall fake_trade_ok(void* ctl, void*) {
    ++g_offer.oks[kOfferTrade];
    if (g_offer.ok_answer[kOfferTrade] != 1) {
        return 0xABCD0000u;
    }
    wr32(static_cast<uint8_t*>(ctl), kTradeState, kTradeOkSent);
    return 0xABCD0001u;
}

// The NPC trade's OK as the game's: nothing offered or no NPC, nothing; then
// the throttle, then sent, the NPC cleared.
uint32_t __fastcall fake_handover_ok(void* ctl, void*) {
    uint8_t* c = static_cast<uint8_t*>(ctl);
    ++g_offer.oks[kOfferHandover];
    bool items = false;
    for (int k = 1; k <= kOfferSlots; ++k) {
        items = items || (c[kHandoverEntries + k * kHandoverEntry + 3] && rd16(c, kHandoverEntries + k * 4));
    }
    if ((!items && !rd32(c, kHandoverGil)) || !rd32(c, kHandoverNpc) || g_offer.ok_answer[kOfferHandover] != 1
        || !fake_throttle(kSendHandover)) {
        return 0xABCD0000u;
    }
    wr32(c, kHandoverNpc, 0);
    return 0xABCD0001u;
}

uint32_t __fastcall fake_trade_cancel(void* ctl, void*, int32_t) {
    ++g_offer.cancels;
    if (g_offer.cancel_answer != 1) {
        return 0xABCD0000u;
    }
    uint8_t* c = static_cast<uint8_t*>(ctl);
    wr32(c, kTradePartner, 0);
    wr32(c, kTradePartnerId, 0);
    wr32(c, kTradeState, 0);
    return 0xABCD0001u;
}

uint32_t __fastcall fake_trade_close(void*, void*) {
    ++g_offer.closes[kOfferTrade];
    game_closes("trade");
    return 1;
}

uint32_t __fastcall fake_handover_close(void* ctl, void*) {
    ++g_offer.closes[kOfferHandover];
    uint8_t* c = static_cast<uint8_t*>(ctl);
    wr32(c, kHandoverNpc, 0);
    close_by_instance(g_mcb, NULL, rdptr(c, kCtlMenu), 1);
    return 0;
}

uint8_t* __cdecl fake_item_record(uint32_t container, uint32_t index) {
    uint8_t* r = g_pd + kPdInventory + index * kItemRecordBytes;
    return container == 0 && index > 0 && index < 81 && rd16(r, 0) ? r : NULL;
}

// The item data the inventory list's rows read, a summary whose type word is
// at +4: 4096 a crystal, 28672 an M Maze Tabula, 28704 an R one, 5020 to
// 5023 the types 20 to 23, 9999 none, any other item of type 1.
uint8_t* __fastcall fake_item_data(void*, void*, uint32_t id) {
    if (id == 9999) {
        return NULL;
    }
    const int type = id == 4096 ? kItemCrystal : id == 28672 ? kItemTabulaM : id == 28704 ? kItemTabulaR
        : id >= 5020 && id <= 5023 ? static_cast<int>(id) - 5000 : 1;
    wr16(g_item_data, 4, type);
    return g_item_data;
}

uint32_t __cdecl fake_describe(const uint8_t* data) {
    return data == g_item_data ? 0x5EED0000u | g_item_data[kItemLock] : 0;
}

uint32_t __fastcall fake_gray_test(void* mode, void*, const uint8_t* record, uint32_t description) {
    ++g_offer.tests;
    g_offer.lock_at_test = g_item_data[kItemLock];
    g_offer.describe_seen = description;
    return g_offer.test_answer;
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
    check(kRowCount == 370 && g_e.inv.count == 368 && g_e.inv.compass == 366 && g_e.inv.targetarrow == 367
            && g_e.inv.targetwi == name("targetwi") && g_e.inv.targetwi >= 0,
        "370 registry rows, 366 windows, conf1win on two rows, the tkdebug record one of them, and mcr1edlo,"
        " mcr2edlo and jobcsel folded into mcr1edit, mcr2edit and jobcselu, then the compass as the 367th name and"
        " the target arrows as the 368th; targetwi's index kept");
    {
        static const char* const pairs[3][2] = {{"mcr1edit", "mcr1edlo"}, {"mcr2edit", "mcr2edlo"},
                                                 {"jobcselu", "jobcsel"}};
        bool folded = true;
        for (int i = 0; i < 3; ++i) {
            const int n = name(pairs[i][0]);
            char alias[kKeyLen + 1];
            snprintf(alias, sizeof(alias), "menu    %-8s", pairs[i][1]);
            folded = folded && n >= 0 && g_e.inv.find_exact(pairs[i][1]) == n && g_e.inv.find_key(alias) == n
                && g_e.inv.names[n].row_count == 2 && strcmp(g_e.inv.names[n].name, pairs[i][0]) == 0
                && g_e.inv.names[g_e.inv.name_of_row[g_e.inv.names[n].rows[1]]].row_count == 2
                && strcmp(kRowSpecs[g_e.inv.names[n].rows[0]].name, pairs[i][0]) == 0
                && strcmp(kRowSpecs[g_e.inv.names[n].rows[1]].name, pairs[i][1]) == 0;
        }
        check(folded, "mcr1edlo, mcr2edlo and jobcsel are their windows under a second name: each name and its key"
            " find the window, which carries its own name and row first, the alias's row second");
    }
    {
        const int c = g_e.inv.compass;
        static const uint8_t nuls[kKeyLen] = {0};
        check(c >= 0 && name("compass") == c && g_e.inv.names[c].row_count == 0
                && memcmp(g_e.inv.names[c].key, nuls, kKeyLen) == 0 && g_e.inv.find_key16(nuls) == c
                && g_e.inv.find_key("") < 0 && g_e.inv.find_key("compass") < 0 && g_e.inv.find("COMPASS") == c
                && g_e.inv.find_window("compass") == c,
            "the compass: no row, a key of 16 NULs that no open key reaches (an empty key pads to spaces), found by"
            " name in any case");
    }
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
            round_trip = round_trip && g_e.inv.find(g_e.inv.names[n].name) == n
                && (g_e.inv.names[n].row_count == 0 || g_e.inv.find_key16(g_e.inv.names[n].key) == n);
        }
        check(round_trip, "every name and every registry key finds its own entry");
        const int t = g_e.inv.targetarrow;
        static const uint8_t nuls[kKeyLen] = {0};
        check(t >= 0 && name("targetarrow") == t && g_e.inv.names[t].row_count == 0
                && memcmp(g_e.inv.names[t].key, nuls, kKeyLen) == 0 && g_e.inv.find_key("targetarrow") < 0
                && g_e.inv.find("TargetArrow") == t && g_e.inv.find_window("TARGETARROW") == t
                && g_e.inv.find_window("targetarrows") < 0 && g_e.inv.find("targetarro") < 0
                && strlen(kTargetArrowName) == kNameMax,
            "the target arrows: an eleven-character name on no row, the 16 NULs no open key reaches as its key,"
            " found by name in any case by both lookups, nothing longer or shorter");
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
    {
        static const char* const only[] = {"query", "gmtell", "comyn", "dead", "mp_dead", "resyn", "resynus",
                                           "resynde", "resynfr", "rem4line", "rem4li2", "stringdl"};
        static const char kInstead[] = "; hide it instead";
        const size_t instead = sizeof(kInstead) - 1;
        bool listed = kHideOnlyCount == 12;
        for (int i = 0; listed && i < 12; ++i) {
            const char* reason = hide_only_reason(only[i]);
            const size_t length = reason ? strlen(reason) : 0;
            listed = hide_only(only[i]) && g_e.inv.find_exact(only[i]) >= 0 && reason
                && strncmp(reason, only[i], strlen(only[i])) == 0 && strstr(reason, " cannot be blocked or closed")
                && (i == 0 || i >= 9 || (length > instead && strcmp(reason + length - instead, kInstead) == 0))
                && close_refusal(only[i]) == reason;
        }
        bool covered = true;
        for (int i = 0; i < kPromptCount; ++i) {
            const char* nm = kPrompts[i].name;
            const bool closes = strcmp(nm, "confyn") == 0 || strcmp(nm, "ok") == 0 || strcmp(nm, "comment") == 0;
            covered = covered && (kPrompts[i].cancels || hide_only(nm) || closes) && !(closes && hide_only(nm));
        }
        const char* raise = hide_only_reason("resynde");
        const char* talk = hide_only_reason("rem4li2");
        const char* ok = close_refusal("ok");
        const char* name_entry = hide_only_reason("stringdl");
        check(listed && covered && !hide_only("passinpu") && !hide_only("logwindo") && !hide_only("merityn")
                && !hide_only("moneyctr") && !hide_only("itemctrl") && hide_only_reason("buff") == NULL && raise
                && strcmp(raise, "resynde cannot be blocked or closed: blocked, the raise question is never shown, so"
                    " the player cannot accept a raise, and the game asks again every frame; hide it instead") == 0
                && talk && strcmp(talk, "rem4li2 cannot be blocked or closed: without its window nothing advances the"
                    " dialogue, not even its timed pages or the script's own advance, and the NPC's script waits"
                    " forever; hide it instead, and answer('rem4li2') or cancel('rem4li2')") == 0
                && !hide_only("ok") && ok && strcmp(ok, "ok cannot be closed: the game waits until it is"
                    " acknowledged, and closed, it waits forever; answer('ok') acknowledges it") == 0
                && close_refusal("confyn") == NULL && close_refusal("mogdoor") == NULL && close_refusal("buff") == NULL
                && name_entry && strcmp(name_entry, "stringdl cannot be blocked or closed: the outgoing box gives up"
                    " when its name entry cannot open, which ends the delivery session; hide it instead, and"
                    " answer('stringdl') or cancel('stringdl')") == 0
                && close_refusal("comment") == NULL && !hide_only("msgline") && !hide_only("scname")
                && !hide_only("link11"),
            "query, gmtell, comyn, dead, mp_dead, the four resyn windows, the two dialogue boxes and stringdl are"
            " hide-only, each refusal its own reason that names it, the new ones saying to hide it instead, and each"
            " the reason a close is refused with; a prompt cancel() cannot end is hide-only but confyn, ok and"
            " comment; ok, not hide-only, is refused a close with its own reason");
    }
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
        check(nc == 50 && chat[name("logwindo")] && chat[name("ability")] && chat[name("partywin")]
                && chat[name("targetwi")] && chat[name("subwindo")]
                && np == 8 && party[name("partywin")] && party[name("targetwi")] && party[name("subwindo")]
                && !party[name("logwindo")] && !party[name("ability")]
                && nt == 2 && target[name("targetwi")] && target[name("subwindo")] && !target[name("partywin")],
            "group closure by the registry: the chat log carries its 41 (jobcsel folded into jobcselu),"
            " partywin, its 6 and subwindo under targetwi (50 with the anchor); the party list 8 and not the log;"
            " the target window 2");
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
                             kSigPostRequestClose, kSigDockReset, kSigSetFrameRect,
                             kSigSelectRow, kSigLink5Cache, kSigQueryCancelAllowed, kSigYesNoClass,
                             kSigYesNoConfirm, kSigDeadConfirm, kSigDeadTick, kSigTemplateSwap, kSigMenuDraw,
                             kSigSpriteDraw, kSigQuadDraw, kSigTargetArrow, kSigPickArrow};
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
        check(parsed && sizeof(all) / sizeof(all[0]) == 32, "all thirty-two signatures parse to one byte per token");
        bool listed = kCallCount == 11;
        for (int i = 0; listed && i < kCallCount; ++i) {
            listed = kCalls[i].signature == all[9 + i] && kCalls[i].name && kCalls[i].name[0];
        }
        uint8_t cb[64];
        char cm[65];
        const size_t clear = parse_signature(kSigPartyClear, cb, cm, sizeof(cb));
        check(listed && clear > kPartyClearFlag + 4 && cm[kPartyClearFlag] == '?' && cm[kPartyClearFlag + 3] == '?'
                && cb[0] == 0xc6 && cb[1] == 0x05,
            "the eleven called routines, in order, the template swap no longer one of them; prtyjoin_clear's flag is"
            " its masked imm32 at +2");
        // The number boxes' and the askers' signatures: each parses, and each
        // offset the engine reads names the bytes it says it does.
        {
            const char* const boxes[] = {kSigMoneyConfirm, kSigItemConfirm, kSigMoneySetup, kSigItemSetup};
            bool ok = true;
            uint8_t b[4][64];
            char m[4][65];
            size_t length[4];
            for (int i = 0; i < 4; ++i) {
                length[i] = parse_signature(boxes[i], b[i], m[i], sizeof(b[i]));
                ok = ok && length[i] >= 40;
            }
            for (int k = 0; k < kBoxCount; ++k) {
                const size_t mcb = kBoxConfirmManager[k];
                ok = ok && b[k][mcb - 1] == 0xb9 && strncmp(m[k] + mcb, "????", 4) == 0 && b[k][mcb + 4] == 0xe8;
                ok = ok && b[k][5] == kBoxLayouts[k].callback && b[k][12] == kBoxLayouts[k].context
                    && b[k][26] == kBoxLayouts[k].callback;
                const size_t key = kBoxSetupKey[k];
                ok = ok && b[2 + k][key - 1] == 0x68 && strncmp(m[2 + k] + key, "????", 4) == 0
                    && b[2 + k][kBoxSetupManager[k] - 1] == 0xb9;
            }
            // moneyctr's setup: the value from the first argument, the callback
            // and the context before the open, the max and the extra callback
            // in the tail.
            ok = ok && b[2][9] == kBoxLayouts[kBoxMoney].value && b[2][51] == kBoxLayouts[kBoxMoney].callback
                && kMoneySetupTail[2] == kBoxLayouts[kBoxMoney].max && kMoneySetupTail[5] == kBoxLayouts[kBoxMoney].context
                && kMoneySetupTail[8] == 0x34 && kMoneySetupTailAt + sizeof(kMoneySetupTail) - 1 == kBoxSetupOpen[kBoxMoney]
                && length[2] == kBoxSetupManager[kBoxMoney];
            // itemctrl's: the preview, the value, the max, the callback and the
            // context, its open at the signature's last byte.
            ok = ok && b[3][24] == 0x28 && b[3][34] == kBoxLayouts[kBoxItem].value
                && b[3][37] == kBoxLayouts[kBoxItem].max && b[3][40] == kBoxLayouts[kBoxItem].callback
                && b[3][43] == kBoxLayouts[kBoxItem].context && b[3][kBoxSetupOpen[kBoxItem]] == 0xe8
                && length[3] == kBoxSetupOpen[kBoxItem] + 1;
            check(ok && kBoxLayouts[kBoxMoney].width == 4 && kBoxLayouts[kBoxItem].width == 1
                    && kBoxLayouts[kBoxMoney].min == 0 && kBoxLayouts[kBoxItem].min == 1,
                "the number boxes' confirms and setups parse: each confirm calls the callback at the layout's offset"
                " with its context and clears it, then closes through the manager's imm32; each setup writes the"
                " layout's fields, its key push and the manager's imm32 where the engine reads them, and calls the"
                " open last");
            bool sites = sizeof(kAskerSites) / sizeof(kAskerSites[0]) == kAskerCount;
            for (int i = 0; sites && i < kAskerCount; ++i) {
                const AskerSite& a = kAskerSites[i];
                uint8_t sb[64];
                char sm[65];
                const size_t n = parse_signature(a.signature, sb, sm, sizeof(sb));
                sites = n >= 40 && a.name && strncmp(a.name, "ask_", 4) == 0;
                if (a.callback) {
                    sites = sites && sb[a.callback - 1] == 0x68 && strncmp(sm + a.callback, "????", 4) == 0;
                }
                if (a.setup_call) {
                    sites = sites && sb[a.setup_call] == 0xe8 && strncmp(sm + a.setup_call + 1, "????", 4) == 0;
                }
                if (a.box_global) {
                    sites = sites && sb[a.box_global - 2] == 0x8b && sb[a.box_global - 1] == 0x0d
                        && strncmp(sm + a.box_global, "????", 4) == 0;
                }
                if (a.owner_global) {
                    sites = sites && sb[a.owner_global - 2] == 0x8b
                        && (sb[a.owner_global - 1] == 0x0d || sb[a.owner_global - 1] == 0x15)
                        && strncmp(sm + a.owner_global, "????", 4) == 0;
                }
                sites = sites && (a.callback != 0) == (a.setup_call != 0) && (a.callback != 0) == (a.box_global != 0);
            }
            check(sites, "the sixteen askers parse: a site's callback is a push's masked imm32, its call reaches a"
                " masked rel32 and its box global and a callback's window global are each a mov ecx's masked imm32");
            bool spec = true;
            for (int i = 0; i < kAskerCount; ++i) {
                const AskerSpec& a = kAskers[i];
                spec = spec && g_e.inv.find_exact(a.owner) >= 0 && a.box < kBoxCount
                    && (!a.field || (a.field_bytes && (a.field_ends || a.moved)));
            }
            check(spec && kAskers[kAskTradeGil].window && kAskers[kAskSynthesis].window
                    && !kAskers[kAskDeliveryGil].window && kAskers[kAskDeliveryGil].field_ends
                    && kAskers[kAskHandoverItem].field_value == kFieldAtAsk && kAskers[kAskTradeItem].item
                    && kAskers[kAskSynthesis].item && !kAskers[kAskStorage].item,
                "the askers: each window a registry name, each field named with its width and either ending the"
                " question or a reason to refuse");
        }
        uint8_t yb[64];
        char ym[65];
        const size_t yesno = parse_signature(kSigYesNoClass, yb, ym, sizeof(yb));
        check(yesno == 50 && ym[kYesNoClassVtable] == '?' && ym[kYesNoClassVtable + 3] == '?'
                && yb[kYesNoClassVtable - 2] == 0xc7 && yb[kYesNoClassVtable - 1] == 0x06
                && yb[yesno - 3] == 0x8d && yb[yesno - 2] == 0x46 && yb[yesno - 1] == kYesNoKey,
            "the yes/no class's constructor: its vtable the masked imm32 of mov [esi] at +31, its key landing at"
            " +0x20, game.h's kYesNoKey");
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
        const size_t compass = parse_signature(kSigCompassDraw, lb, lm, sizeof(lb));
        const bool compass_ok = compass == 41 && kCompassDrawGlobal == 2 && kCompassDrawGlobal2 == 31
            && kCompassDrawManagerImm == 16 && lb[0] == 0x8b && lb[1] == 0x0d && lb[29] == 0x8b && lb[30] == 0x0d
            && lb[15] == 0xb9 && lb[8] == 0x74 && lb[27] == 0x74 && lb[35] == 0xe9 && lb[40] == 0xc3
            && strcmp(lm, "xx????xxx?x????x????x????xxx?xx????x????x") == 0;
        check(compass_ok,
            "the 0.8.0 signature parses: the compass draw entry's 41 bytes, its global the masked imm32 of mov ecx"
            " at +2 and again at +31, the manager's that of mov ecx at +16, two je, the jump to its render, ret");
        const size_t gate = parse_signature(kSigMacroGate, lb, lm, sizeof(lb));
        const bool gate_ok = gate == 53 && kMacroGateGlobal == 2 && kMacroGateManagerImm == 40 && lb[0] == 0x66
            && lb[1] == 0xa1 && lb[30] == 0x83 && lb[31] == 0x39 && lb[32] == 0x60 && lb[39] == 0xb9 && lb[44] == 0xe8
            && strcmp(lm, "xx????xxxxxxx?xxxxxx????xx????xxxxx????x????x????xxxx") == 0;
        const size_t object = parse_signature(kSigMacroObject, lb, lm, sizeof(lb));
        const bool object_ok = object == 30 && kMacroObjectImm == 15 && kMacroObjectImm2 == 23 && lb[14] == 0xa3
            && lb[21] == 0x89 && lb[22] == 0x1d && lb[27] == 0x6a && lb[28] == 0x50
            && strcmp(lm, "xx????xxxx????x????xxxx????xxx") == 0;
        check(gate_ok && object_ok,
            "the 0.9.0 signatures parse: the macro key gate's 53 bytes, the word of mov ax at +2 and the manager of"
            " mov ecx at +40 masked, its cmp with 0x60 and its call; the constructor site's 30, the object's global"
            " the masked imm32 of mov at +15 and again at +23, before the push 0x50");
        const size_t opener = parse_signature(kSigAbilityOpen, lb, lm, sizeof(lb));
        const bool opener_ok = opener == 53 && kAbilityOpenGlobal == 1 && kAbilityOpenManagerImm == 29
            && kAbilityOpenKeyImm == 43 && lb[0] == 0xa1 && lb[5] == 0x85 && lb[6] == 0xc0 && lb[7] == 0x74
            && lb[9] == 0x53 && lb[14] == 0x80 && lb[15] == 0xfb && lb[16] == 0x01 && lb[17] == 0x75
            && lb[23] == 0x68 && lb[28] == 0xb9 && lb[33] == 0xe8 && lb[42] == 0x68 && lb[47] == 0xb9
            && lb[52] == 0xe8 && strcmp(lm, "x????xxx?xxxxxxxxx?xxxxx????x????x????xxxxx????x????x") == 0;
        check(opener_ok,
            "the 0.10.0 signature parses: the ability opener's 53 bytes, its global the masked imm32 of mov eax at"
            " +1, the two key pushes' imm32s at +24 and +43 and the manager's of mov ecx at +29 and +48 masked, the"
            " je and the jne displacements masked, cmp bl,1 and the two calls exact");
        const size_t confirm = parse_signature(kSigYesNoConfirm, lb, lm, sizeof(lb));
        const bool confirm_ok = confirm == 63 && strspn(lm, "x") == 11 && strncmp(lm + 11, "????", 4) == 0
            && strncmp(lm + 29, "????", 4) == 0 && strncmp(lm + 34, "????", 4) == 0 && strspn(lm + 38, "x") == 25
            && lb[5] == kYesNoMode && lb[17] == kYesNoCallback && lb[24] == kCtlMenu && lb[42] == kCtlMenu
            && lb[46] == kMenuCursor && lb[47] == kRowYes && lb[52] == kYesNoContext && lb[60] == kYesNoContext
            && sizeof(kConfirmChainedRead) == 10 && kConfirmChainedRead[4] == kYesNoChained
            && sizeof(kConfirmCallbackClear) == 7 && kConfirmCallbackClear[2] == kYesNoCallback
            && sizeof(kConfirmChainedDefault) == 9 && kConfirmChainedDefault[2] == kYesNoChainedYes
            && kConfirmChainedDefault[5] == kYesNoChained && kYesNoConfirmBytes == 0xC0;
        check(confirm_ok,
            "the yes/no class's confirm parses: 63 bytes, its callees and the manager masked, reading the mode, the"
            " callback, the window, its cursor row against yes and the context where game.h keeps them; the three"
            " reads and writes checked inside it name the chained flag, the callback and the chained default");
        const size_t dead = parse_signature(kSigDeadConfirm, lb, lm, sizeof(lb));
        const size_t imm8s[] = {kDeadDeciderSecond, kDeadDeciderFirst, kDeadTextOtherwise, kDeadCategory, kDeadAskMode,
                                kDeadAskCancellable, kDeadAskHelp, kDeadAskDefault, kDeadAskContext};
        bool pushes = true;
        for (size_t i = 0; i < sizeof(imm8s) / sizeof(imm8s[0]); ++i) {
            pushes = pushes && lb[imm8s[i] - 1] == 0x6a && lm[imm8s[i]] == '?';
        }
        const size_t calls[] = {kDeadDeciderCall, kDeadMessageCall, kDeadAskCall};
        bool rel32s = true;
        for (size_t i = 0; i < 3; ++i) {
            rel32s = rel32s && lb[calls[i]] == 0xe8 && lm[calls[i]] == 'x' && strncmp(lm + calls[i] + 1, "????", 4) == 0;
        }
        const bool dead_ok = dead == 62 && pushes && rel32s && lb[kDeadTextIfSet - 1] == 0x68
            && strncmp(lm + kDeadTextIfSet, "????", 4) == 0 && lb[kDeadAskerGlobal - 2] == 0x8b
            && lb[kDeadAskerGlobal - 1] == 0x0d && strncmp(lm + kDeadAskerGlobal, "????", 4) == 0
            && lb[kDeadAskCallback - 1] == 0x68 && strncmp(lm + kDeadAskCallback, "????", 4) == 0
            && lb[kDeadAskContext - 2] == 0x50 && lb[14] == 0x74 && lb[15] == 0x07 && lb[21] == 0xeb
            && lb[22] == 0x02 && kDeadAskCall + 5 == dead
            && sizeof(kDeadChosenHead) == 9 && kDeadChosenHead[8] == 0xa1 && sizeof(kDeadChosenTail) == 5
            && kDeadChosenTail[2] == kDeadChosen && kDeadChosenTail[4] == 0xc3 && sizeof(kYesNoAskHead) == 16
            && kYesNoAskHead[5] == kYesNoCallback && kYesNoAskHead[13] == 0xc2 && kYesNoAskHead[14] == 0x1c;
        check(dead_ok,
            "the KO window's confirm parses: 62 bytes, every argument a masked push imm8 (the two ids an imm32 and an"
            " imm8 either side of the je on the decider's al), the asker's global the masked imm32 of mov ecx, the"
            " callback that of push, three masked calls; its callback's check sets kDeadChosen, its ask's tests the"
            " callback and pops the seven arguments");
        const size_t tick = parse_signature(kSigDeadTick, lb, lm, sizeof(lb));
        check(tick == 60 && lb[10] == kDeadDeadline && lb[28] == kDeadRemaining && lb[17] == 0x8d && lb[18] == 0x04
                && lb[19] == 0x49 && lb[20] == 0x8d && lb[21] == 0x04 && lb[22] == 0x80 && lb[23] == 0xc1
                && lb[24] == 0xe0 && lb[25] == 0x02 && kDeadTicks == 60 && lb[kDeadTickDeciderCall] == 0xe8
                && strncmp(lm + kDeadTickDeciderCall + 1, "????", 4) == 0 && lb[51] == 0x20 && lb[52] == 0x1e,
            "the KO window's tick parses: 60 bytes, the countdown's end at +0x1C and the countdown at +0x20 written as"
            " (end - now) * 3 * 5 * 4, kDeadTicks; its decider's call masked, past the end 0x1E");
    }
    {
        static const char* const prologues[kSiteCount] = {
            "8b 44 24 04 83 ec 20", "83 ec 14 53 55 56 57",
            "53 56 8b 74 24 0c 33 db 57", "53 55 56 8b 74 24 10 32 db 57",
            "56 8b f1 b9 ?? ?? ?? ??", "51 55 56 8b f1 57", "8b 0d ?? ?? ?? ??", "66 a1 ?? ?? ?? ??",
            "a1 ?? ?? ?? ??", "", "83 ec 18 53 55", "83 ec 24 53 55", "83 ec 0c 53 55", "83 ec 08 8d 44 24 00"};
        static const uint32_t args[kSiteCount] = {12, 0, 20, 4, 0, 4, 0, 0, 12, 16, 4, 0, 32, 16};
        static const uint32_t pops[kSiteCount] = {1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 1, 1, 1, 1};
        bool ok = true;
        for (int i = 0; i < kSiteCount; ++i) {
            if (!kSites[i].signature) {
                ok = ok && i == kSiteKeyTest && kSites[i].prologue == kKeyTestPrologue && kSites[i].arg_bytes == args[i]
                    && kSites[i].callee_pops == pops[i] && kSites[i].pre != NULL && kSites[i].post == NULL
                    && kSites[i].manager_imm == 0 && sizeof(kKeyTestHead) + 4 == kKeyTestPrologue;
                continue;
            }
            uint8_t sb[64];
            char sm[65];
            uint8_t pb[16];
            char pm[17];
            parse_signature(kSites[i].signature, sb, sm, sizeof(sb));
            const size_t pl = parse_signature(prologues[i], pb, pm, sizeof(pb));
            ok = ok && pl == kSites[i].prologue && memcmp(sb, pb, pl) == 0
                && kSites[i].arg_bytes == args[i] && kSites[i].callee_pops == pops[i] && kSites[i].pre != NULL;
            const uint32_t imm = kSites[i].manager_imm;
            for (size_t k = 0; k < pl; ++k) {
                const bool in_imm = (imm && k >= imm && k < imm + 4)
                    || (i == kSiteCompassDraw && k >= kCompassDrawGlobal && k < kCompassDrawGlobal + 4)
                    || (i == kSiteMacroGate && k >= kMacroGateGlobal && k < kMacroGateGlobal + 4)
                    || (i == kSiteAbilityOpen && k >= kAbilityOpenGlobal && k < kAbilityOpenGlobal + 4);
                ok = ok && sm[k] == (in_imm ? '?' : 'x');
            }
            ok = ok && (imm == 0 || imm + 4 <= pl || i == kSiteCompassDraw || i == kSiteMacroGate
                        || i == kSiteAbilityOpen);
        }
        check(ok && kSites[kSiteMouseMode].manager_imm == 4 && kSites[kSiteMouseMode].post == NULL
                && kSites[kSiteMenuInput].manager_imm == 0 && kSites[kSiteMenuInput].post == NULL
                && kSites[kSiteCompassDraw].manager_imm == 16 && kSites[kSiteCompassDraw].post == NULL
                && kSites[kSiteMacroGate].manager_imm == 40 && kSites[kSiteMacroGate].post == NULL
                && kSites[kSiteAbilityOpen].manager_imm == 29 && kSites[kSiteAbilityOpen].post != NULL
                && kSites[kSiteTemplateSwap].post != NULL && kSites[kSiteMenuDraw].post != NULL
                && kSites[kSiteSpriteDraw].post != NULL && kSites[kSitePrimitiveDraw].post == NULL
                && kSitePrimitiveDraw == kSiteCount - 1,
            "hooked prologues: open 7/12, ui_update 7/0, staged_close 9/20, show_path 10/4, menu_input 6/4, exact"
            " bytes; mouse_mode 8/0, exact but for the manager's imm32 at +4, which the engine fills in;"
            " compass_draw 6/0, exact but for the compass global's imm32 at +2, which the engine reads out of the"
            " entry's +31 and fills in, the manager's at +16 past the prologue; macro_gate 6/0, exact but for the"
            " word its mov ax reads at +2, which the engine reads out of the hit and fills in, the manager's at +40"
            " past the prologue; ability_open 5/12, the caller popping, exact but for the global its mov eax reads"
            " at +1, read out of the hit the same way, the manager's at +29 past the prologue, and a post; key_test"
            " 6/16, the caller popping, with no signature: push ecx and the mov al of the byte it reads, read out of"
            " the routine the UI update calls; 0.11.0's template_swap 5/4 and a post, menu_draw 5/0 and"
            " sprite_draw 5/32 each with a post, primitive_draw 7/16 with none, exact bytes");
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
        row(31)[kRowLayer] = 0x7F;
        const bool layer = !verify_registry(g_registry, kRowCount + 1, why, sizeof(why))
            && strstr(why, "row 31 (logwindo): draw layer") != NULL;
        row(31)[kRowLayer] = 1;
        check(layer, "a row at a draw layer of its own (0x7F) fails verification and names itself, so no engine"
            " installs over it");
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
    g_e.game.yesno_vtable = vt_yesno;
    g_e.game.select_row = &fake_select_row;
    g_e.game.yesno_ask = &fake_yesno_ask_this;
    g_e.game.box_confirm[kBoxMoney] = &fake_money_confirm;
    g_e.game.box_confirm[kBoxItem] = &fake_item_confirm;
    for (int i = 0; i < kAskerCount; ++i) {
        g_e.game.askers[i] = reinterpret_cast<const void*>(kAskerCallbacks[i]);
    }
    g_e.game.close_instance = &close_by_instance;
    g_e.game.ability_list = &hooked_ability_list;
    g_e.game.openers.equipment = &fake_equipment;
    g_e.game.openers.monstrosity = &fake_monstrosity;
    g_e.game.openers.magic = &fake_magic_list;
    {
        OpenerSites& o = g_e.game.openers;
        o.map = &fake_map_open;
        o.set_spells = &fake_set_spells;
        o.attachments = &fake_attachments;
        o.linkshell_check = &fake_set_check;
        o.linkshell = &fake_linkshell;
        o.instincts = &fake_instincts;
        o.item_cache = &g_cache_ptr;
        o.item_data = &fake_set_item_data;
        o.item_type = kSetItemType;
        o.use_check = &fake_set_check;
        o.synthesis = &fake_synthesis;
        o.inventor_list = kInventorList;
        o.code = reinterpret_cast<const uint8_t*>(0x1000);
        o.code_size = 0x7FFFF000;
        o.tabula = &fake_tabula;
        o.macro_editor = &g_macro_editor_ptr;
        o.macros = &fake_macros;
    }
    {
        RouteSites& r = g_e.game.routes;
        r.storage = &fake_storage;
        r.mogpost = &fake_mogpost;
        r.mog_house = &g_mog_house_ptr;
        r.nomad = &g_nomad_ptr;
        r.job_row = 0x14;
        r.language_key = &fake_language_key;
        r.friend_list = &fake_friend;
        r.msglist = &fake_msglist;
        r.blklist = &fake_blklist;
        r.mount = &fake_mount;
        r.job_points_test = &fake_job_test;
        r.job_points = &fake_job_points;
        r.fp_cat2 = &fake_fp_cat2;
        r.search_reset = &fake_search_reset;
        r.search_mode = 0x58;
        r.comgenre = &fake_comgenre;
        r.comment_sub[0] = &fake_sccompar;
        r.comment_sub[1] = &fake_sccomite;
        r.comment_sub[2] = &fake_sccomls;
        r.item_panel = &fake_item_panel;
        r.item_extra = 0x10;
        r.timer[0] = &fake_casttime;
        r.timer[1] = &fake_eventtim;
        r.mp_stat = &fake_mp_stat;
        r.persona_layout = 0x1C;
        r.conf11l_set = 0x14;
        r.quest00_mode = 0x68;
        r.quest00_dim[0] = &fake_quest00_dim;
        r.quest00_dim[1] = &fake_quest00_dim2;
        r.quest01 = &fake_quest01;
        r.miss00_menu = &fake_miss00_menu;
        r.expansion = &fake_expansion;
        r.mission_bits = kMissionBits;
        r.miss00_log = kMissionLog;
        r.tracking = &fake_tracking;
        r.scanlist = &fake_scanlist;
        r.player = &g_route_player_ptr;
        r.area = kRouteArea;
        r.region = &fake_region;
        r.region_kind = kCnqKindAt;
        r.coloresu_region = 0x18;
        r.textcol[0] = 0x14;
        r.textcol[1] = 0x1C;
        r.textcol[2] = 0x18;
        r.macro_pick = &fake_macro_pick;
        r.macro_palette = 0x0C;
        r.macro_close = &fake_macro_close;
        r.title = &fake_title;
        r.items_view = &fake_items_view;
        r.items_object = &g_items_object_ptr;
        r.inventor_items = kItemsByte;
        r.inventor_policy = &g_policy;
        r.inventor_open = &fake_inventor_open;
        r.summary_type = 4;
        r.macro_switch = &fake_macro_switch;
        r.bag_size = &fake_bag_size;
        r.scan_question = reinterpret_cast<const void*>(&fake_scan_question);
        r.status_close = reinterpret_cast<const uint8_t*>(&fake_status_begin_close);
        r.textcol_escape = reinterpret_cast<const uint8_t*>(&fake_title);
        r.instincts_escape = &fake_instincts_escape;
        for (int i = 0; i < kRegionEscapeCloses; ++i) {
            make_key(g_e, name(kRegionEscapeNames[i]), g_region_keys[i]);
            const char* key = g_region_keys[i];
            memcpy(g_region_input + kRegionEscapeAt + kRegionEscapeStep * static_cast<size_t>(i) + 1, &key, 4);
        }
        r.region_escape = g_region_input;
    }
    {
        AskedSites& a = g_e.game.asked;
        a.evitem_callback = &evitem_answered;
        a.confyn_restore = &confyn_restored;
        a.bank_clear = &bank_clear;
        a.bank_data = &g_player_ptr;
        a.bank_safe2 = kSafe2Flag;
        a.maplist_close = &maplist_close_reset;
        a.maplist_rebuild = &maplist_rebuild;
        a.maplist_select = &maplist_select;
        a.maplist_cursor = &maplist_cursor;
        a.map_close = &fake_map_close;
        a.gm_slot = &g_gm_slot;
        a.gm_queue = g_gm_queue;
        DoorSites& d = a.door;
        d.vtable = vt_door;
        d.gate = &fake_door_gate;
        d.gate_first = 4;
        d.gate_second = 0;
        d.floor = &fake_door_floor;
        d.floor_word = kFloorWord;
        d.condition = &fake_door_condition;
        d.tick = &fake_door_tick;
        d.house = &g_house_ptr;
        d.opened = g_door_opened_code;
        d.open_door = &fake_door_open;
        for (int i = 0; i < 3; ++i) {
            d.exiting[i] = &g_door_exiting[i];
        }
        d.table = reinterpret_cast<const uint8_t*>(g_door_lists);
        d.nations = 10;
        d.garden = 280;
        uint8_t* actor = g_door_actor;
        memcpy(g_house + kHouseDoor, &actor, 4);
    }
    {
        TextSites& t = g_e.game.text;
        t.stringdl_stop = &fake_stringdl_stop;
        t.line = &g_line_ptr;
        t.unbind = &fake_unbind;
        t.bind = &fake_bind;
        t.focus = &fake_focus;
        t.comment_stop = &fake_comment_stop;
        t.comment_start = &fake_comment_start;
        t.sanitize = &fake_sanitize;
        t.trim = &fake_trim;
        t.msgline_stop = &fake_msgline_stop;
        t.msgline_start = &fake_msgline_start;
        t.player = &g_msgline_player_ptr;
        t.pol = &g_pol_ptr;
        t.pol_number = &fake_pol_number;
        t.clock_text = &fake_clock_text;
        t.send_tell = &fake_send_tell;
        t.send_reply = &fake_send_reply;
        t.send_short = &fake_send_short;
        t.send_answer = &fake_send_answer;
        t.send_dated = &fake_send_dated;
        t.sent[0] = reinterpret_cast<const void*>(&fake_sent);
        t.sent[1] = reinterpret_cast<const void*>(&fake_text_open);
        t.sending = &g_sending_ptr;
        t.mark = &fake_mark;
        t.busy = &g_busy;
        t.scname_stop = &fake_scname_stop;
        t.search = &g_search_ptr;
        t.criteria_clear = &fake_criteria_clear;
        t.criteria_name = &fake_criteria_name;
        t.search_start = &fake_search_start;
        t.link11_stop = &fake_link11_stop;
        t.slot_taken = &fake_slot_taken;
        t.selected = &fake_selected;
        t.encode = &fake_encode;
        t.create = &fake_create;
        t.item = &fake_item;
        static const size_t made[4] = {0, 4, 8, 9};
        for (int i = 0; i < 4; ++i) {
            t.made[i] = g_made + made[i];
        }
        t.passinpu_stop = &fake_passinpu_stop;
        t.passinpu_start = &fake_passinpu_start;
    }
    {
        static const uint8_t rows[16] = {0xFF, 1, 2, 3, 4, 5, 6, 7, 8, 0xFF, 0xFF, 0, 0xFF, 0xFF, 0xFF, 0xFF};
        OfferSites& o = g_e.game.offer;
        o.stage[kOfferTrade] = &fake_trade_stage;
        o.stage[kOfferHandover] = &fake_handover_stage;
        o.ok[kOfferTrade] = &fake_trade_ok;
        o.ok[kOfferHandover] = &fake_handover_ok;
        o.trade_cancel = &fake_trade_cancel;
        o.trade_close = &fake_trade_close;
        o.handover_close = &fake_handover_close;
        o.trade_rows = rows;
        o.player = &g_pd_ptr;
        o.own = kPdOwn;
        o.theirs = kPdTheirs;
        o.inventory = kPdInventory;
        o.last_send = kPdLastSend;
        o.intervals = &g_intervals_ptr;
        o.interval = kIntervalAt;
        o.packets = 0x11E;
        o.clock_object = &g_clock_ptr;
        o.item = &fake_item_record;
        o.cache = &g_cache_ptr;
        o.item_data = &fake_item_data;
        o.describe = &fake_describe;
        o.lock = kItemLock;
        for (int k = 0; k < kStatusClasses; ++k) {
            o.status_class[k] = k == 0 || k == 5 || k == 0x1B ? kStatusListed : k == 0x13 ? kStatusModal : 2;
        }
        o.status_mode = &g_status_mode_ptr;
        for (int w = 0; w < kOfferCount; ++w) {
            vt_mode[w][kModeGrayTest / 4] = reinterpret_cast<const void*>(&fake_gray_test);
            const void* vtp = vt_mode[w];
            memcpy(g_mode[w], &vtp, 4);
            o.mode[w] = &g_mode_ptr[w];
        }
        o.actors = g_actors;
        o.actor_titled = 0x210;
        o.actor_title = 0x214;
        o.actor_name = 0x7C;
        g_actors[3] = g_actor[0];
        g_actors[5] = g_actor[1];
        memcpy(g_actor[0] + 0x7C, "Partner", 8);
        memcpy(g_actor[1] + 0x7C, "Plain", 6);
        memcpy(g_actor[1] + 0x214, "Titled NPC", 11);
        wr16(g_actor[1], 0x210, 1);
        for (uint32_t id = kSendTradeChange; id <= kSendHandover; ++id) {
            wr32(g_intervals, kIntervalAt + 4 * id, 1000);
        }
        set_clock(500000);
    }
    {
        DialogueSites& t = g_e.game.dialogue;
        t.dispatch = reinterpret_cast<const uint8_t*>(&fake_text_dispatch);
        t.advance = &fake_text_advance;
        t.page_sound = &fake_page_sound;
        for (int d = 0; d < kDialogueCount; ++d) {
            t.texts[d] = &g_text_objects[d];
        }
        t.release = &fake_log_release;
        t.clear = &fake_text_clear;
        t.wait = &g_wait;
        t.click_return = 0x10136C79;
    }
    {
        // The KO window's confirm as the 05-10 image's input handler makes it.
        DeadConfirm& d = g_e.game.dead;
        d.decider = &fake_dead_decider;
        d.decider_first = 1;
        d.decider_second = 0;
        d.message = &fake_message;
        d.category = 0x0E;
        d.text_if_set = 0x258;
        d.text_otherwise = 0x46;
        d.callback = reinterpret_cast<const void*>(&fake_dead_chosen);
        d.context = 0;
        d.default_yes = 0;
        d.help = 0;
        d.cancellable = 1;
        d.mode = 0;
    }
    vt_yesno[5] = g_nop_code;
    vt_yesno[2] = reinterpret_cast<const void*>(&fake_yesno_begin_close);
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
                                  "persona", "buff", "prtyjoin", "link5",
                                  "scsibori", "delivery", "post1", "post2", "playermo",
                                  "mcr1pall", "mcr2pall"};
    for (size_t i = 0; i < sizeof(self_classes) / sizeof(self_classes[0]); ++i) {
        give_controller(self_classes[i], vt_self);
    }
    give_controller("query", vt_query);
    give_controller("arealist", vt_area);
    give_controller("equip", vt_layout);
    give_controller("logwin2", vt_layout);
    give_controller("conf1win", vt_self, 1);
    give_controller("dbdelsel", vt_self);
    give_controller("dead", vt_self);
    // The number boxes and the windows that ask through them.
    {
        const char* boxes[] = {"moneyctr", "itemctrl", "handover", "trade", "auc1", "shop", "bank",
                               "tskill1"};
        for (size_t i = 0; i < sizeof(boxes) / sizeof(boxes[0]); ++i) {
            give_controller(boxes[i], vt_self);
        }
    }
    // The yes/no windows: each its own controller keyed to its row, but the
    // resyn family's one, keyed to the English client's resynus, in all four
    // rows; the class's cancel as the game's constructors pass it.
    {
        struct YesNo {
            const char* name;
            const char* key;
            uint8_t cancellable;
        };
        const YesNo yesno[] = {
            {"comyn", "menu    comyn   ", 1}, {"merityn", "menu    merityn ", 0}, {"shopbuy", "menu    shopbuy ", 1},
            {"shopsell", "menu    shopsell", 1}, {"resyn", "menu    resynus ", 0},
        };
        for (size_t i = 0; i < sizeof(yesno) / sizeof(yesno[0]); ++i) {
            uint8_t* ctl = give_controller(yesno[i].name, vt_yesno);
            memcpy(ctl + kYesNoKey, yesno[i].key, kKeyLen);
            ctl[0x38] = yesno[i].cancellable;
        }
        const char* family[] = {"resynus", "resynde", "resynfr"};
        for (int i = 0; i < 3; ++i) {
            g_ctl_global[first_row(family[i])] = controller("resyn");
        }
    }
    // The Asked windows and the windows that ask through them; bankmenu's
    // begin-close clears its callback, gmtell is of query's class.
    {
        vt_bank[5] = g_nop_code;
        vt_bank[2] = reinterpret_cast<const void*>(&bank_clear);
        const char* asked[] = {"sortyn", "evitem01", "confyn", "maplist", "mp_dead", "ok", "iuse", "evitem", "map0"};
        for (size_t i = 0; i < sizeof(asked) / sizeof(asked[0]); ++i) {
            give_controller(asked[i], vt_self);
        }
        give_controller("bankmenu", vt_bank);
        give_controller("gmtell", vt_query);
        vt_door[5] = g_nop_code;
        give_controller("mogdoor", vt_door);
    }
    // The text-entry windows and passinpu; comment's, msgline's and
    // passinpu's open hooks start their fields, the begin-closes end them;
    // stringdl's field is its for good.
    vt_text[5] = g_nop_code;
    vt_text[1] = reinterpret_cast<const void*>(&fake_text_open);
    vt_text[2] = reinterpret_cast<const void*>(&fake_text_close);
    for (int t = 0; t < kTextCount; ++t) {
        give_controller(kTextNames[t], vt_text);
    }
    memcpy(vt_pass, vt_self, sizeof(vt_pass));
    vt_pass[1] = reinterpret_cast<const void*>(&fake_text_open);
    vt_pass[2] = reinterpret_cast<const void*>(&fake_text_close);
    give_controller("passinpu", vt_pass);
    {
        uint8_t* field = g_text_boxes[kTextStringdl];
        memcpy(controller("stringdl") + kStringdlField, &field, 4);
    }
    give_controller("link6", vt_self);
    // The dialogue boxes, each its text object and its chat log's.
    vt_rem4[5] = g_nop_code;
    for (int d = 0; d < kDialogueCount; ++d) {
        uint8_t* ctl = give_controller(kDialogueNames[d], vt_rem4);
        uint8_t* text = g_texts[d];
        uint8_t* log_ctl = g_log_ctls[d];
        uint8_t* log = g_log_objects[d];
        memcpy(ctl + kBoxText, &text, 4);
        memcpy(ctl + kBoxLog, &log_ctl, 4);
        memcpy(log_ctl + kLogObject, &log, 4);
        memcpy(text + 0x30, &ctl, 4);
    }
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

    // hides reach the per-menu draw in the drain; the registry row and the
    // live menu keep their draw layer
    {
        uint8_t* log = open_now("logwindo");
        const int n = name("logwindo");
        g_e.holds.set(h1, n, kHoldHide, true);
        g_e.holds.set(h2, n, kHoldHide, true);
        const bool untouched = draw_paints(log);
        drain(g_e);
        const bool hidden = !draw_paints(log) && row(31)[kRowLayer] == 1 && log[kMenuLayer] == 1;
        g_e.holds.set(h1, n, kHoldHide, false);
        drain(g_e);
        const bool held = !draw_paints(log) && g_e.hidden.used == 1;
        g_e.holds.set(h2, n, kHoldHide, false);
        drain(g_e);
        check(untouched && hidden && held && draw_paints(log) && g_e.hidden.used == 0 && row(31)[kRowLayer] == 1
                && log[kMenuLayer] == 1,
            "hide: nothing changes until the drain; the per-menu draw paints nothing while the union holds, and paints"
            " again once it drops; the registry row and the live menu keep draw layer 1 throughout");
        uint8_t* conf = open_now("conf1win");
        g_e.holds.set(h1, name("conf1win"), kHoldHide, true);
        drain(g_e);
        const bool both_rows = !draw_paints(conf) && row(264)[kRowLayer] == 2 && row(265)[kRowLayer] == 2
            && conf[kMenuLayer] == 2;
        g_e.holds.set(h1, name("conf1win"), kHoldHide, false);
        drain(g_e);
        check(both_rows && draw_paints(conf),
            "a name on two rows: its live menu, found through its second row, painting nothing while hidden;"
            " both rows and the menu keep layer 2");
    }

    // the mouse kept off a hidden window: the hit test's own test of +0x77
    // on every hidden window, and the mode picker held in world mode while a
    // hidden prompt that asks is active, never over a hidden window that asks
    // nothing
    {
        const int b = name("buff");
        uint8_t* buff_row = row(g_e.inv.names[b].rows[0]);
        const uint8_t layer = kRowSpecs[g_e.inv.names[b].rows[0]].layer;
        uint8_t* first = open_now("buff");
        const bool fresh = first && first[kMenuMouse] == 1;
        g_e.holds.set(h1, b, kHoldHide, true);
        drain(g_e);
        const bool hidden = first && buff_row[kRowLayer] == layer && first[kMenuLayer] == layer
            && first[kMenuMouse] == 0 && !draw_paints(first);
        check(fresh && hidden, "hide: the live menu's +0x77 goes from the constructor's 1 to 0 and its draw"
            " paints nothing; its draw layer and the registry row's stay the row's");
        run(cmd(kOpClose, "buff", 0, 0, s1, gen1));
        const bool closed = !live("buff");
        uint8_t* second = open_now("buff");
        check(closed && second && second != first && second[kMenuLayer] == layer && second[kMenuMouse] == 0
                && !draw_paints(second) && draw_paints(first),
            "a hidden window opened again as a new instance: the open's post writes 0 over the constructor's 1 and"
            " paints nothing before any drain, the freed instance's address painting again");
        uint8_t mouse[0x60];
        memset(mouse, 0, sizeof(mouse));
        wr32(g_mcb, kMcbActive, reinterpret_cast<uint32_t>(second));
        mouse[kMouseMode] = 2;
        const int originals = g_picker_originals;
        hooked_mouse_mode(mouse);
        check(mouse[kMouseMode] == 2 && g_picker_originals == originals + 1,
            "mouse mode picker with a hidden window that asks nothing active: the original runs (mode 2)");
        const int y = name("comyn");
        g_e.holds.set(h1, y, kHoldHide, true);
        drain(g_e);
        uint8_t* prompt = yesno_ask("comyn", "Log out?", false, true, 0) ? live("comyn") : NULL;
        wr32(g_mcb, kMcbActive, reinterpret_cast<uint32_t>(prompt));
        hooked_mouse_mode(mouse);
        check(prompt && prompt[kMenuLayer] == kRowSpecs[g_e.inv.names[y].rows[0]].layer && prompt[kMenuMouse] == 0
                && mouse[kMouseMode] == kMouseWorld && g_picker_originals == originals + 1,
            "with a hidden prompt (comyn) asking active at its own layer: +0x77 0 on it too, and the pre writes mode"
            " 1, the original not run");
        HuFrame f;
        memset(&f, 0, sizeof(f));
        f.ecx = reinterpret_cast<uint32_t>(mouse);
        f.user = &g_e;
        mouse[kMouseMode] = 2;
        const int skipped = hook_mouse_mode_pre(&f);
        check(skipped == 1 && mouse[kMouseMode] == kMouseWorld && f.result == 0,
            "the pre leaves the result word alone");
        uint8_t* shown_prompt = open_now("merityn");
        wr32(g_mcb, kMcbActive, reinterpret_cast<uint32_t>(shown_prompt));
        shown_prompt[kMenuLayer] = 0x7F;
        mouse[kMouseMode] = 2;
        const int shown = hook_mouse_mode_pre(&f);
        shown_prompt[kMenuLayer] = kRowSpecs[g_e.inv.names[name("merityn")].rows[0]].layer;
        game_closes("merityn");
        wr32(g_mcb, kMcbActive, reinterpret_cast<uint32_t>(prompt));
        check(shown == 0 && mouse[kMouseMode] == 2,
            "the pre goes by what the engine hides, never by a layer: a prompt it does not hide, at layer 0x7F, no pre");
        g_e.holds.set(h1, b, kHoldHide, false);
        g_e.holds.set(h1, y, kHoldHide, false);
        drain(g_e);
        wr32(g_mcb, kMcbActive, reinterpret_cast<uint32_t>(second));
        check(buff_row[kRowLayer] == layer && second[kMenuLayer] == layer && second[kMenuMouse] == 1
                && prompt[kMenuMouse] == 1 && draw_paints(second) && draw_paints(prompt),
            "unhide: +0x77 back to 1 and the draws paint, the draw layer and the row at the expected layer");
        hooked_mouse_mode(mouse);
        const bool menu_mode = mouse[kMouseMode] == 2 && g_picker_originals == originals + 2;
        wr32(g_mcb, kMcbActive, 0);
        hooked_mouse_mode(mouse);
        check(menu_mode && mouse[kMouseMode] == 1 && g_picker_originals == originals + 3,
            "the picker's original runs with the window visible (mode 2) and with no active menu (mode 1)");
        run(cmd(kOpClose, "buff", 0, 0, s1, gen1));
        game_closes("comyn");
        events(&cursor, ev, 64);
    }

    // keys and the gamepad kept off a hidden prompt that asks: the routing
    // routine's call to the active menu's sink, and only that call, dropped;
    // a hidden window that asks nothing takes its keys
    {
        const int b = name("buff");
        const int y = name("comyn");
        uint8_t* menu = yesno_ask("comyn", "Log out?", false, true, 0) ? live("comyn") : NULL;
        wr32(g_mcb, kMcbActive, reinterpret_cast<uint32_t>(menu));
        const int originals = g_sink_originals;
        const int opens = g_main_menu_opens;
        route(5);
        check(menu && g_sink_originals == originals + 1 && g_sink_menu == menu && g_sink_code == 5,
            "a key on the visible active prompt: the routing routine's code reaches its sink");
        g_e.holds.set(h1, y, kHoldHide, true);
        drain(g_e);
        const bool hidden = !draw_paints(menu) && menu[kMenuLayer] == kRowSpecs[g_e.inv.names[y].rows[0]].layer;
        for (int code = 1; code <= 0x20; ++code) {
            route(code);
        }
        check(hidden && g_sink_originals == originals + 1 && g_main_menu_opens == opens,
            "a hidden prompt that asks active: no code from the routing routine, 1 to 0x20, reaches its sink, and code"
            " 7 opens no main menu");
        HuFrame f;
        const uint32_t answer = hooked_menu_input(menu, 6, g_e.game.routing_return, &f);
        check(answer == 0 && f.result == 0 && g_sink_originals == originals + 1,
            "the pre answers 0 for the sink over whatever the result word held, so the original does not run");
        hooked_menu_input(menu, 9, reinterpret_cast<uintptr_t>(&fake_set_cursor) + 0x20);
        const bool from_cursor = g_sink_originals == originals + 2 && g_sink_code == 9 && g_sink_menu == menu;
        hooked_menu_input(menu, 0x16, g_e.game.routing_return + 0x100);
        check(from_cursor && g_sink_originals == originals + 3 && g_sink_code == 0x16,
            "SetCursor's code 9 and the wheel's 0x16, from other callers, still reach the hidden prompt's sink");
        uint8_t* other = open_now("equip");
        wr32(g_mcb, kMcbActive, reinterpret_cast<uint32_t>(other));
        route(5);
        const bool visible_other = g_sink_originals == originals + 4 && g_sink_menu == other;
        hooked_menu_input(menu, 5, g_e.game.routing_return);
        check(visible_other && g_sink_originals == originals + 5 && g_sink_menu == menu,
            "with another window active, its keys reach it; the pre drops only a call to the active menu");
        uint8_t* buff = open_now("buff");
        g_e.holds.set(h1, b, kHoldHide, true);
        drain(g_e);
        wr32(g_mcb, kMcbActive, reinterpret_cast<uint32_t>(buff));
        bool reached = buff && !draw_paints(buff) && buff[kMenuMouse] == 0;
        for (int code = 1; code <= 0x20; ++code) {
            route(code);
            reached = reached && g_sink_menu == buff && g_sink_code == code;
        }
        check(reached && g_sink_originals == originals + 5 + 0x20 && g_main_menu_opens == opens,
            "a hidden window that asks nothing active (buff): +0x77 0, and every routed code, 1 to 0x20, reaches its"
            " sink, as the party list's must for <stpt>");
        wr32(g_mcb, kMcbActive, 0);
        route(5);
        route(7);
        const bool none = g_sink_originals == originals + 5 + 0x20 && g_main_menu_opens == opens + 1;
        hooked_menu_input(menu, 5, g_e.game.routing_return);
        check(none && g_sink_originals == originals + 6 + 0x20,
            "with no active menu the routing routine's own branch runs (code 7 opens the main menu), and the pre"
            " passes a call to the hidden prompt's sink");
        g_e.holds.set(h1, y, kHoldHide, false);
        g_e.holds.set(h1, b, kHoldHide, false);
        drain(g_e);
        wr32(g_mcb, kMcbActive, reinterpret_cast<uint32_t>(menu));
        route(6);
        check(draw_paints(menu) && g_sink_originals == originals + 7 + 0x20 && g_sink_menu == menu
                && g_sink_code == 6,
            "unhide: the routing routine's codes reach the prompt's sink again");
        wr32(g_mcb, kMcbActive, 0);
        game_closes("comyn");
        run(cmd(kOpClose, "buff", 0, 0, s1, gen1));
        run(cmd(kOpClose, "equip", 0, 0, s1, gen1));
        events(&cursor, ev, 64);
    }

    // the per-menu draw over every open window: every original runs, a
    // hidden window's, targetwi's among them, marked so its quads stay off
    // the device; a hidden window the game closes keeps its mark while its
    // instance lives, and an address its freed instance leaves is the next
    // window's to paint
    {
        const char* names[] = {"equip", "buff", "logwin2", "targetwi"};
        uint8_t* menus[4];
        bool was[4];
        for (int i = 0; i < 4; ++i) {
            was[i] = live(names[i]) != NULL;
            menus[i] = open_now(names[i]);
        }
        const int b = name("buff");
        const int tw = name("targetwi");
        g_e.holds.set(h1, b, kHoldHide, true);
        g_e.holds.set(h1, tw, kHoldHide, true);
        drain(g_e);
        // Each menu's draw: 'r' its original run painting, 'h' run with its
        // quad kept off the device, 's' skipped; the marks clear after.
        char walked[5];
        bool clear = true;
        for (int i = 0; i < 4; ++i) {
            HuFrame f;
            memset(&f, 0, sizeof(f));
            f.ecx = reinterpret_cast<uint32_t>(menus[i]);
            f.user = &g_e;
            const int skipped = hook_menu_draw_pre(&f);
            walked[i] = skipped ? 's' : (primitive() == 0 ? 'h' : 'r');
            if (!skipped) {
                hook_menu_draw_post(&f);
            }
            clear = clear && g_e.drawing_hidden == 0 && g_e.draw_depth == 0;
        }
        walked[4] = '\0';
        bool layers = true;
        for (int i = 0; i < 4; ++i) {
            const int r = g_e.inv.names[name(names[i])].rows[0];
            layers = layers && menus[i] && menus[i][kMenuLayer] == kRowSpecs[r].layer
                && row(r)[kRowLayer] == kRowSpecs[r].layer;
        }
        check(strcmp(walked, "rhrh") == 0 && clear && layers && g_e.hidden.used == 2,
            "a walk over equip, buff, logwin2 and targetwi with buff and targetwi hidden: every original runs, buff's"
            " and targetwi's quads kept off the device, the others' drawn; the marks clear after each post; every"
            " window at its row's layer, live and in the registry");
        uint8_t* kept = menus[1];
        g_close_keeps = true;
        game_closes("buff");
        drain(g_e);
        const bool closing = kept[kMenuClosing] && live("buff") == kept && !draw_paints(kept);
        destroy("buff");
        uint8_t* res = rdptr(kept, kMenuRes);
        memcpy(res + kResKey, g_e.inv.names[name("logwin2")].key, kKeyLen);
        const bool reused = draw_paints(kept) && g_e.hidden.slot_plus_one[b] != 0;
        memcpy(res + kResKey, g_e.inv.names[b].key, kKeyLen);
        drain(g_e);
        const bool freed = g_e.hidden.slot_plus_one[b] == 0 && draw_paints(kept);
        g_close_keeps = false;
        check(closing && reused && freed,
            "buff closed by the game paints nothing through its staged close while its controller holds it; freed,"
            " a menu of another window at its address paints before the drain, and the drain drops its slot");
        g_e.holds.set(h1, b, kHoldHide, false);
        g_e.holds.set(h1, tw, kHoldHide, false);
        drain(g_e);
        check(g_e.hidden.used == 0 && draw_paints(menus[3]) && g_e.drawing_hidden == 0 && g_e.draw_depth == 0,
            "unhidden: no slot left, targetwi's draw paints");
        for (int i = 0; i < 4; ++i) {
            if (!was[i] && live(names[i])) {
                game_closes(names[i]);
            }
        }
        events(&cursor, ev, 64);
    }

    // a hidden window runs as shown: conq3pts opened with an input wait of
    // 3, its cursor put on row 2, its data gone so its own controller's
    // draw closes it, its close's countdowns run out and the manager frees
    // it, over the same frames shown and hidden; hidden, every field and
    // event the same, frame by frame, and none of its quads on the device
    {
        if (!controller("conq3pts")) {
            give_controller("conq3pts", vt_self);
        }
        const int q = name("conq3pts");
        struct Seen {
            int16_t wait;
            int16_t first;
            int16_t second;
            uint8_t closing;
            uint8_t ended;
            uint32_t state;
            int part;
            bool bound;
            bool open;
            int closed;
            int pixels;
        };
        const int kFrames = 12;
        auto through = [&](bool hidden, Seen* seen) {
            g_e.holds.set(h1, q, kHoldHide, hidden);
            drain(g_e);
            events(&cursor, ev, 64);
            g_conq_gone = false;
            g_close_keeps = true;
            uint8_t* menu = open_now("conq3pts");
            if (menu) {
                wr16(menu, kMenuInputWait, 3);
            }
            for (int k = 0; k < kFrames; ++k) {
                if (k == 2 && live("conq3pts")) {
                    wr16(live("conq3pts"), kMenuCursor, 2);
                }
                g_conq_gone = k >= 4;
                const int before = g_pixels;
                game_frame("conq3pts");
                const uint8_t* m = live("conq3pts");
                Seen& s = seen[k];
                memset(&s, 0, sizeof(s));
                s.part = -1;
                if (m) {
                    s.wait = rd16(m, kMenuInputWait);
                    s.first = rd16(m, kCloseFirst);
                    s.second = rd16(m, kCloseSecond);
                    s.closing = m[kMenuClosing];
                    s.ended = m[kMenuPendingDestroy];
                    s.state = rd32(m, kMenuState);
                    s.part = static_cast<int>((rdptr(m, 8) - g_parts[0]) / 0x40);
                }
                s.bound = m != NULL;
                s.open = open_menu(g_e, kPlain, q, NULL) != NULL;
                s.pixels = g_pixels - before;
                const int got = events(&cursor, ev, 64);
                for (int i = 0; i < got; ++i) {
                    s.closed += is_event(ev[i], kEvClosed, "conq3pts") ? 1 : 0;
                }
            }
            g_close_keeps = false;
            g_conq_gone = false;
            g_e.holds.set(h1, q, kHoldHide, false);
            drain(g_e);
            return menu != NULL && !live("conq3pts");
        };
        Seen shown[kFrames];
        Seen hidden[kFrames];
        const bool ran = through(false, shown) && through(true, hidden);
        // Shown: the wait out at frame 2, the cursor's part from frame 2, its
        // own draw's close at frame 4 (one closed event, out of opened()),
        // the countdowns run out at frames 5 to 7, +0x70 at 8, freed at 9.
        bool schedule = ran && shown[2].wait == 0 && shown[1].wait == 1 && shown[1].part == 0 && shown[2].part == 2
            && shown[3].open && shown[3].closed == 0 && shown[4].closed == 1 && !shown[4].open
            && shown[4].state == kStateDormant && shown[4].first == 2 && shown[5].first == 1 && shown[6].first == 0
            && shown[6].second == 1 && shown[7].second == 0 && !shown[7].ended && shown[8].ended && shown[8].bound
            && !shown[9].bound && shown[3].pixels == 2 && shown[5].pixels == 1 && shown[9].pixels == 0;
        bool same = ran;
        int pixels = 0;
        int first_diff = -1;
        for (int k = 0; k < kFrames; ++k) {
            const Seen& a = shown[k];
            const Seen& b = hidden[k];
            const bool equal = a.wait == b.wait && a.first == b.first && a.second == b.second && a.closing == b.closing
                && a.ended == b.ended && a.state == b.state && a.part == b.part && a.bound == b.bound
                && a.open == b.open && a.closed == b.closed;
            if (!equal && first_diff < 0) {
                first_diff = k;
            }
            same = same && equal;
            pixels += b.pixels;
        }
        char label[320];
        snprintf(label, sizeof(label),
            "conq3pts over 12 frames shown: its input wait out at frame 2, +8 on the cursor's row's part, its own"
            " controller's draw closing it at frame 4 (closed once, out of opened()), +0x66 then +0x64 run out,"
            " +0x70 at frame 8, the manager freeing it at frame 9; two quads a frame drawn, then one");
        check(schedule, label);
        snprintf(label, sizeof(label),
            "hidden over the same frames, every field and event as shown (input wait, +0x66, +0x64, +0x6E, +0x70,"
            " the state, +8, live, open, closed events): first frame that differs %d; %d of its quads on the device",
            first_diff, pixels);
        check(same && pixels == 0, label);
    }

    // nested per-menu draws unwind level by level: inside a hidden window's
    // draw a shown window's quads stay off the device too, and inside a
    // shown window's draw a hidden one's own alone do
    {
        uint8_t* outer = live("equip") ? live("equip") : open_now("equip");
        uint8_t* inner = live("buff") ? live("buff") : open_now("buff");
        const int b = name("buff");
        g_e.holds.set(h1, b, kHoldHide, true);
        drain(g_e);
        auto pre = [&](const uint8_t* m) {
            HuFrame f;
            memset(&f, 0, sizeof(f));
            f.ecx = reinterpret_cast<uint32_t>(m);
            f.user = &g_e;
            return hook_menu_draw_pre(&f);
        };
        auto post = [&]() {
            HuFrame f;
            memset(&f, 0, sizeof(f));
            f.user = &g_e;
            hook_menu_draw_post(&f);
        };
        auto quad = [&]() {
            const int drawn = primitive();
            return drawn == 1 ? 'd' : drawn == 0 ? '-' : '?';
        };
        char seen[16];
        int k = 0;
        int pres = pre(outer);
        seen[k++] = quad();
        pres += pre(inner);
        seen[k++] = quad();
        post();
        seen[k++] = quad();
        post();
        seen[k++] = '|';
        pres += pre(inner);
        seen[k++] = quad();
        pres += pre(outer);
        seen[k++] = quad();
        post();
        seen[k++] = quad();
        post();
        seen[k++] = quad();
        seen[k] = '\0';
        g_e.holds.set(h1, b, kHoldHide, false);
        drain(g_e);
        check(pres == 0 && strcmp(seen, "d-d|---d") == 0 && g_e.draw_depth == 0 && g_e.drawing_hidden == 0,
            "equip's draw around hidden buff's: buff's quad alone dropped; buff's around equip's: every quad"
            " dropped until buff's draw returns; the depth and the marks clear after");
    }

    // the active menu as the game picks it, by the order its draw layer
    // gives the manager's list: hidden stringdl at its own layer 2, asking
    // the name, is the active menu until delivery, focused after it, takes
    // the focus, as the outgoing box's commit of a name focuses it; a menu
    // at layer 0x7F sorts above every menu below layer 5 and would keep it
    {
        const int s = name("stringdl");
        g_e.holds.set(h1, s, kHoldHide, true);
        drain(g_e);
        wr16(controller("delivery"), kDeliveryState, kDeliveryAsksName);
        controller("stringdl")[kStringdlMode] = kStringdlAsksName;
        uint8_t* entry = open_now("stringdl");
        drain(g_e);
        const uint8_t* first = game_active();
        const int originals = g_sink_originals;
        route(5);
        const bool dropped = g_sink_originals == originals;
        uint8_t* box = open_now("delivery");
        drain(g_e);
        const bool box_active = game_active() == box;
        route(5);
        const bool reached = g_sink_originals == originals + 1 && g_sink_menu == box;
        focus(box);
        const bool kept_box = game_active() == box;
        entry[kMenuLayer] = 0x7F;
        focus(entry);
        game_active();
        focus(box);
        const bool sorted_above = game_active() == entry;
        entry[kMenuLayer] = 2;
        focus(entry);
        focus(box);
        const bool back = game_active() == box;
        check(entry && box && first == entry && dropped && box_active && reached && kept_box && entry[kMenuLayer] == 2
                && !draw_paints(entry) && sorted_above && back,
            "hidden stringdl asking the name at layer 2: active and its keys dropped until delivery opens after it;"
            " then delivery is the active menu and takes the routed keys, and stays it when focused again; with"
            " stringdl at layer 0x7F the same pick keeps stringdl over delivery");
        g_e.holds.set(h1, s, kHoldHide, false);
        wr16(controller("delivery"), kDeliveryState, 0);
        controller("stringdl")[kStringdlMode] = 0;
        drain(g_e);
        game_closes("delivery");
        game_closes("stringdl");
        wr32(g_mcb, kMcbActive, 0);
        events(&cursor, ev, 64);
    }

    // the compass: no row, no menu; hidden by its draw hook's pre, moved by
    // the anchor that pre writes at every draw, its home put back by a
    // reset, opened and closed as its state byte flips
    {
        static uint8_t compass[kCompassBytes];
        static uint8_t* compass_ptr;
        memset(compass, 0, sizeof(compass));
        compass[kCompassState] = 3;
        wr16(compass, kCompassX, 104);
        wr16(compass, kCompassY, 1058);
        wr16(compass, kCompassHeight, 42);
        compass_ptr = compass;
        g_e.game.compass = &compass_ptr;
        g_e.compass_shown = 1;
        const int n = g_e.inv.compass;
        int16_t box[4];
        compass_frame(compass, box);
        check(is_compass(g_e, n) && !is_compass(g_e, name("buff")) && !is_compass(g_e, -1)
                && live_menu(g_e, kPlain, n, NULL) == NULL && open_menu(g_e, kPlain, n, NULL) == NULL
                && controller_at(g_e, g_e.inv.find_exact("compass")) == NULL && controller_at(g_e, n) == NULL
                && compass_object(g_e, kPlain) == compass && compass_open(compass) && !compass_open(NULL)
                && box[0] == 16 && box[1] == 1016 && box[2] == 104 && box[3] == 1058,
            "the compass: no live menu, no controller through any slot; its object through the global, open while"
            " its state byte is not 0, its box 88 x 42 ending on its anchor");
        HuFrame f;
        memset(&f, 0, sizeof(f));
        f.user = &g_e;
        const bool plain = hook_compass_pre(&f) == 0 && rd16(compass, kCompassX) == 104 && rd16(compass, kCompassY) == 1058;
        g_e.holds.set(h1, n, kHoldHide, true);
        drain(g_e);
        const bool hidden = hook_compass_pre(&f) == 1 && rd16(compass, kCompassX) == 104
            && rd16(compass, kCompassY) == 1058 && g_e.place[n].applied_hidden;
        g_e.holds.set(h1, n, kHoldHide, false);
        drain(g_e);
        check(plain && hidden && hook_compass_pre(&f) == 0 && !g_e.place[n].applied_hidden
                && events(&cursor, ev, 64) == 0,
            "the draw hook's pre: the original runs and nothing is written with no hold and no move; it does not"
            " run while a hold hides the compass, which the drain's layer walk survives; no event for a hide");
        Command move = cmd(kOpMove, "compass", 10, 20, s1, gen1);
        move.flags = kCmdFrame;
        move.verb = kVerbMove;
        run(move);
        const MemEntry& m = g_e.memory.e[n];
        const bool queued = m.active && m.x == 10 && m.y == 20 && m.by_frame && m.owner_slot == s1
            && g_e.place[n].touched && !g_e.place[n].registry_undocked && g_e.compass_home_valid
            && home_x(g_e.compass_home) == 104 && home_y(g_e.compass_home) == 1058
            && rd16(compass, kCompassX) == 104 && rd16(compass, kCompassY) == 1058;
        const int ran = hook_compass_pre(&f);
        const bool drawn = ran == 0 && rd16(compass, kCompassX) == 98 && rd16(compass, kCompassY) == 62;
        wr16(compass, kCompassX, 104);
        wr16(compass, kCompassY, 1058);
        hook_compass_pre(&f);
        check(queued && drawn && rd16(compass, kCompassX) == 98 && rd16(compass, kCompassY) == 62
                && g_e.drain_errors == 0,
            "move(compass, 10, 20) by the frame: remembered, the game's anchor 104,1058 taken as home, no registry"
            " row undocked, nothing written until the next draw, whose pre writes the anchor 98,62 and writes it"
            " again after the game put its own back");
        Command second = cmd(kOpMove, "compass", 30, 40, s1, gen1);
        second.flags = kCmdFrame;
        second.verb = kVerbMove;
        run(second);
        hook_compass_pre(&f);
        check(home_x(g_e.compass_home) == 104 && home_y(g_e.compass_home) == 1058 && rd16(compass, kCompassX) == 118
                && rd16(compass, kCompassY) == 82,
            "a second move keeps the home the first found, and the next draw writes its anchor");
        g_e.holds.set(h1, n, kHoldHide, true);
        drain(g_e);
        wr16(compass, kCompassX, 104);
        wr16(compass, kCompassY, 1058);
        const int skipped = hook_compass_pre(&f);
        g_e.holds.set(h1, n, kHoldHide, false);
        drain(g_e);
        check(skipped == 1 && rd16(compass, kCompassX) == 118 && rd16(compass, kCompassY) == 82,
            "hidden and moved: the pre still writes the anchor the move asks for, then keeps the original from running");
        Command origin_move = cmd(kOpMove, "compass", 200, 900, s1, gen1);
        origin_move.verb = kVerbMove;
        run(origin_move);
        hook_compass_pre(&f);
        check(!m.by_frame && rd16(compass, kCompassX) == 200 && rd16(compass, kCompassY) == 900,
            "a move by the origin (the older slots) is the anchor itself");
        run(cmd(kOpReset, "compass", 0, 0, s1, gen1));
        const bool reset = !m.active && rd16(compass, kCompassX) == 104 && rd16(compass, kCompassY) == 1058
            && !g_e.compass_home_valid && !g_e.place[n].touched;
        wr16(compass, kCompassX, 105);
        hook_compass_pre(&f);
        check(reset && rd16(compass, kCompassX) == 105 && g_e.drain_errors == 0,
            "reset(compass): the home anchor written back, the memory and the home forgotten; the next draw writes"
            " nothing");
        const uint8_t ops[3] = {kOpResetOwned, kOpRestoreAll, kOpResetMine};
        bool each = true;
        for (int i = 0; i < 3; ++i) {
            wr16(compass, kCompassX, 104);
            wr16(compass, kCompassY, 1058);
            run(move);
            hook_compass_pre(&f);
            const bool moved = m.active && rd16(compass, kCompassX) == 98;
            Command r = cmd(ops[i], ops[i] == kOpResetMine ? "compass" : NULL, 0, 0, s1, gen1,
                ops[i] == kOpResetMine ? kAspectPosition : 0);
            run(r);
            each = each && moved && !m.active && rd16(compass, kCompassX) == 104 && rd16(compass, kCompassY) == 1058
                && !g_e.compass_home_valid;
        }
        check(each && g_e.drain_errors == 0,
            "reset_all (the handle's own), restore_all and reset(compass, 'position') each put the anchor back through"
            " the same path");
        events(&cursor, ev, 64);
        compass[kCompassState] = 0;
        drain(g_e);
        const int closed = events(&cursor, ev, 64);
        const bool closed_once = closed == 1 && is_event(ev[0], kEvClosed, "compass");
        drain(g_e);
        const bool quiet = events(&cursor, ev, 64) == 0;
        compass[kCompassState] = 1;
        drain(g_e);
        const int opened = events(&cursor, ev, 64);
        const bool opened_once = opened == 1 && is_event(ev[0], kEvOpened, "compass");
        compass[kCompassState] = 3;
        drain(g_e);
        check(closed_once && quiet && opened_once && events(&cursor, ev, 64) == 0,
            "the drain posts closed{compass} as the state byte reaches 0, once, and opened{compass} as it leaves 0;"
            " 1 to 3, still shown, posts nothing");
        compass_ptr = NULL;
        const bool no_object = hook_compass_pre(&f) == 0 && compass_object(g_e, kPlain) == NULL;
        run(move);
        hook_compass_pre(&f);
        drain(g_e);
        const bool gone = events(&cursor, ev, 64) == 1 && is_event(ev[0], kEvClosed, "compass") && m.active
            && !g_e.compass_home_valid;
        run(cmd(kOpReset, "compass", 0, 0, s1, gen1));
        g_e.holds.set(h1, n, kHoldHide, true);
        const bool hidden_none = hook_compass_pre(&f) == 1;
        g_e.holds.set(h1, n, kHoldHide, false);
        compass_ptr = compass;
        drain(g_e);
        check(no_object && gone && !m.active && hidden_none && events(&cursor, ev, 64) == 1
                && is_event(ev[0], kEvOpened, "compass") && g_e.drain_errors == 0,
            "a null object pointer: the pre runs the original, a move takes no home and writes nothing, the drain"
            " reads it closed, a reset and a hide are safe; the object back, opened again");
        g_e.game.compass = NULL;
        g_e.compass_shown = 0;
        drain(g_e);
        check(hook_compass_pre(&f) == 0 && events(&cursor, ev, 64) == 0,
            "with no global at all the pre runs the original and the drain posts nothing");
    }

    // the target arrows: no row, no menu; opened and closed as the target
    // window's draw starts and stops painting one; the target window hidden
    // keeps its layer, and the draw hooks drop everything it paints but the
    // arrows; the arrows, while a handle hides targetarrow, whichever way
    // the target window stands
    {
        const int t = g_e.inv.targetarrow;
        const int tw = name("targetwi");
        uint8_t* tctl = controller("targetwi");
        const uintptr_t kArrow = 0x1015040A;
        const uintptr_t kPick = 0x10150446;
        const uintptr_t kOther = 0x10117053;
        g_e.game.arrow_return[0] = kArrow;
        g_e.game.arrow_return[1] = kPick;
        check(is_targetarrow(g_e, t) && synthetic(g_e, t) && synthetic(g_e, g_e.inv.compass) && !synthetic(g_e, tw)
                && !is_targetarrow(g_e, tw) && !is_targetarrow(g_e, -1) && g_e.inv.targetwi == tw
                && controller_at(g_e, g_e.inv.find_exact("targetarrow")) == NULL && controller_at(g_e, t) == NULL
                && live_menu(g_e, kPlain, t, NULL) == NULL,
            "targetarrow: a name on no row, with no controller and no live menu");

        // opened and closed: what the target window's draw paints, read by
        // the drain each frame
        if (live("targetwi")) {
            game_closes("targetwi");
        }
        tctl[kTargetOther] = 0;
        tctl[kTargetPicking] = 0;
        wr16(tctl, kTargetArrowX, 0);
        drain(g_e);
        events(&cursor, ev, 64);
        auto arrows = [&](int type) {
            drain(g_e);
            const int k = events(&cursor, ev, 64);
            int seen = 0;
            for (int i = 0; i < k; ++i) {
                if (event_name(ev[i]) == t) {
                    seen = is_event(ev[i], type, "targetarrow") ? seen + 1 : seen + 100;
                }
            }
            return seen;
        };
        uint8_t* menu = open_now("targetwi");
        const bool none = arrows(kEvOpened) == 0;
        wr16(tctl, kTargetArrowX, 640);
        wr16(tctl, kTargetArrowY, 300);
        const bool target = arrows(kEvOpened) == 1 && arrows(kEvOpened) == 0;
        tctl[kTargetOther] = 1;
        const bool other = arrows(kEvClosed) == 1;
        tctl[kTargetOther] = 0;
        const bool back = arrows(kEvOpened) == 1;
        wr16(tctl, kTargetArrowX, 0);
        const bool gone = arrows(kEvClosed) == 1;
        tctl[kTargetPicking] = 1;
        const bool picking = arrows(kEvOpened) == 1;
        menu[kMenuDraws] = 0;
        const bool undrawn = arrows(kEvClosed) == 1;
        menu[kMenuDraws] = 1;
        const bool redrawn = arrows(kEvOpened) == 1;
        cover(menu);
        const bool covered = arrows(kEvClosed) == 1;
        reveal(menu);
        const bool revealed = arrows(kEvOpened) == 1;
        // the walk's own gates: the manager's global hide, and rem4line's
        // modal dim of every window but its own; a policy 0x40 window is
        // drawn through both
        g_mcb[kMcbHideAll] = 1;
        const bool hidden_all = arrows(kEvClosed) == 1;
        const uint32_t policy = rd32(menu, kMenuPolicy);
        wr32(menu, kMenuPolicy, policy | kPolicyDrawnAlways);
        const bool exempt = arrows(kEvOpened) == 1;
        wr32(menu, kMenuPolicy, policy);
        const bool hidden_again = arrows(kEvClosed) == 1;
        g_mcb[kMcbHideAll] = 0;
        const bool shown_all = arrows(kEvOpened) == 1;
        uint8_t* modal = controller("rem4line");
        uint8_t* modal_menu = rdptr(modal, kCtlMenu);
        modal[kBoxModal] = 1;
        const bool dimmed = arrows(kEvClosed) == 1;
        wr32(modal, kCtlMenu, reinterpret_cast<uint32_t>(menu));
        const bool own_dim = arrows(kEvOpened) == 1;
        wr32(modal, kCtlMenu, reinterpret_cast<uint32_t>(modal_menu));
        const bool dimmed_again = arrows(kEvClosed) == 1;
        modal[kBoxModal] = 0;
        const bool undimmed = arrows(kEvOpened) == 1;
        game_closes("targetwi");
        const bool closed = arrows(kEvClosed) == 1 && arrows(kEvClosed) == 0;
        check(none && target && other && back && gone && picking && undrawn && redrawn && covered && revealed && closed,
            "opened{targetarrow} once as the target window's draw starts to paint an arrow (+0xBC's x set, or +0xB8"
            " while picking), closed{targetarrow} once as it stops: +0x65 set, no arrow, the menu's +0x6A clear,"
            " dormant, closed");
        check(hidden_all && exempt && hidden_again && shown_all && dimmed && own_dim && dimmed_again && undimmed,
            "the walk's gates: closed{targetarrow} while the manager's +0xB4 is 1 and while rem4line's +0x48 dims"
            " every window but the one its controller's +8 names, opened again as each lifts; a policy 0x40"
            " target window is drawn through the global hide");

        // a move, a resize and a reset of targetarrow carry nothing to the game
        LONG errors = g_e.drain_errors;
        run(cmd(kOpMove, "targetarrow", 10, 20, s1, gen1));
        run(cmd(kOpReset, "targetarrow", 0, 0, s1, gen1));
        run(cmd(kOpResetMine, "targetarrow", 0, 0, s1, gen1, kAspectPosition | kAspectSize));
        MemEntry tm;
        check(g_e.memory.read(t, &tm) && !tm.active && !tm.size_kind && !g_e.place[t].touched
                && g_e.drain_errors == errors,
            "move, reset and reset_mine of targetarrow on the game thread: nothing remembered, nothing placed, no"
            " error");

        // the hooks
        auto draw_pre = [&](const uint8_t* m) {
            HuFrame f;
            memset(&f, 0, sizeof(f));
            f.ecx = reinterpret_cast<uint32_t>(m);
            f.user = &g_e;
            return hook_menu_draw_pre(&f);
        };
        auto draw_post = [&]() {
            HuFrame f;
            memset(&f, 0, sizeof(f));
            f.ecx = 0x12345678;
            f.user = &g_e;
            hook_menu_draw_post(&f);
        };
        auto sprite = [&](uintptr_t from, bool pre) {
            HuFrame f;
            memset(&f, 0, sizeof(f));
            f.return_address = from;
            f.user = &g_e;
            if (pre) {
                return hook_sprite_pre(&f);
            }
            hook_sprite_post(&f);
            return 0;
        };
        // One quad through the draw primitive: 'd' drawn, '-' dropped with
        // D3D_OK.
        auto quad = [&]() {
            const int drawn = primitive();
            return drawn == 1 ? 'd' : drawn == 0 ? '-' : '?';
        };
        // The target window's draw as the game's makes it: its frame and
        // content, the arrow over the target, more content, the arrow over
        // the selection, each sprite's quads inside its call; then another
        // menu's draw and a sprite from elsewhere.
        auto frame = [&](const uint8_t* m, char out[16]) {
            int k = 0;
            const int pre = draw_pre(m);
            out[k++] = quad();
            out[k++] = quad();
            sprite(kArrow, true);
            out[k++] = quad();
            sprite(kArrow, false);
            out[k++] = quad();
            sprite(kPick, true);
            out[k++] = quad();
            sprite(kPick, false);
            out[k++] = quad();
            draw_post();
            out[k++] = '|';
            draw_pre(live("equip"));
            out[k++] = quad();
            sprite(kOther, true);
            out[k++] = quad();
            sprite(kOther, false);
            draw_post();
            out[k++] = quad();
            out[k] = '\0';
            return pre == 0 && g_e.drawing_hidden == 0 && g_e.draw_depth == 0 && !g_e.drawing_arrow;
        };
        if (!live("equip")) {
            open_now("equip");
        }
        menu = open_now("targetwi");
        wr16(tctl, kTargetArrowX, 640);
        tctl[kTargetPicking] = 1;
        char seen[4][16];
        const bool clear0 = frame(menu, seen[0]);
        g_e.holds.set(h1, tw, kHoldHide, true);
        drain(g_e);
        const uint8_t* trow = row(g_e.inv.names[tw].rows[0]);
        const bool kept = menu[kMenuLayer] == kRowSpecs[g_e.inv.names[tw].rows[0]].layer
            && trow[kRowLayer] == kRowSpecs[g_e.inv.names[tw].rows[0]].layer && menu[kMenuMouse] == 0
            && g_e.place[tw].applied_hidden && !hidden_prompt(g_e, menu);
        const bool clear1 = frame(menu, seen[1]);
        g_e.holds.set(h1, t, kHoldHide, true);
        drain(g_e);
        const bool clear2 = frame(menu, seen[2]);
        g_e.holds.set(h1, tw, kHoldHide, false);
        drain(g_e);
        const bool unhidden = menu[kMenuMouse] == 1 && !g_e.place[tw].applied_hidden;
        const bool clear3 = frame(menu, seen[3]);
        g_e.holds.set(h1, t, kHoldHide, false);
        drain(g_e);
        check(clear0 && clear1 && clear2 && clear3 && strcmp(seen[0], "dddddd|ddd") == 0
                && strcmp(seen[1], "--d-d-|ddd") == 0 && strcmp(seen[2], "------|ddd") == 0
                && strcmp(seen[3], "dd-d-d|ddd") == 0,
            "the draw primitive's pre over the target window's draw (frame, content, the target arrow, content,"
            " the picking arrow, content) then another menu's and a sprite from elsewhere: all drawn with nothing"
            " hidden; targetwi hidden, all of its own dropped with D3D_OK but the arrows; both hidden, all of its"
            " own dropped; targetarrow alone hidden, the arrows alone dropped; never another menu's or another"
            " sprite's, and every flag clear after each post");
        check(kept && unhidden,
            "hidden targetwi keeps its layer in its row and live menu, so the walk still draws it; its +0x77 is 0"
            " and it is no prompt; the unhide gives +0x77 back");
        // targetarrow takes a block as the compass does: the hide want bit.
        g_e.holds.set(h1, t, kHoldBlock, true);
        drain(g_e);
        char blocked[16];
        frame(menu, blocked);
        g_e.holds.set(h1, t, kHoldBlock, false);
        drain(g_e);
        // A blocked targetwi never opens, so its draw paints no arrow.
        game_closes("targetwi");
        g_e.holds.set(h1, tw, kHoldBlock, true);
        drain(g_e);
        events(&cursor, ev, 64);
        const bool refused = open_now("targetwi") == NULL && !targetarrow_drawn(g_e, kPlain, NULL)
            && arrows(kEvOpened) == 0;
        g_e.holds.set(h1, tw, kHoldBlock, false);
        drain(g_e);
        check(strcmp(blocked, "dd-d-d|ddd") == 0 && refused,
            "block(targetarrow) drops the arrows as its hide does; a blocked targetwi is refused its open and no"
            " arrow is drawn");
        events(&cursor, ev, 64);
    }

    // the macro keys: a hold under no name, counted once per handle; the
    // gate's pre answers no while any handle holds it; the block's command
    // closes a bar the handler has up as the handler's own close does
    {
        static uint8_t macro[kMacroObjectBytes];
        static uint8_t* macro_ptr;
        memset(macro, 0, sizeof(macro));
        macro_ptr = macro;
        g_e.game.macro_object = &macro_ptr;
        HuFrame f;
        memset(&f, 0, sizeof(f));
        f.user = &g_e;
        f.result = 0xDEAD;
        const bool yes = hook_macro_gate_pre(&f) == 0 && f.result == 0xDEAD && g_e.holds.macros_want == 0
            && strcmp(verb_name(kVerbBlockMacros), "block_macros") == 0;
        g_e.holds.set_macros(h1, true);
        g_e.holds.set_macros(h1, true);
        const bool once = g_e.holds.macros_count == 1 && g_e.holds.macros_want == 1 && h1.macros_hold == 1;
        const bool no = hook_macro_gate_pre(&f) == 1 && f.result == 0;
        g_e.holds.set_macros(h2, true);
        g_e.holds.set_macros(h1, false);
        f.result = 0xDEAD;
        const bool held = g_e.holds.macros_count == 1 && g_e.holds.macros_want == 1 && !h1.macros_hold
            && h2.macros_hold && hook_macro_gate_pre(&f) == 1 && f.result == 0;
        g_e.holds.release(h2, g_e.inv.count);
        f.result = 0xDEAD;
        check(yes && once && no && held && g_e.holds.macros_count == 0 && g_e.holds.macros_want == 0
                && !h2.macros_hold && hook_macro_gate_pre(&f) == 0 && f.result == 0xDEAD,
            "the macro key gate's pre: with no hold the original runs and the result word is left alone; a hold,"
            " counted once however often one handle sets it, has it answer 0 and run nothing; the hold stays while"
            " another handle holds it, and that handle's release drops it");

        uint8_t* bar = open_now("mcr1pall");
        macro[kMacroBar] = 1;
        macro[kMacroSet] = 2;
        macro[kMacroBarUp] = 1;
        events(&cursor, ev, 64);
        clear_log();
        const LONG errors = g_e.drain_errors;
        Command b = cmd(kOpMacrosBlocked, NULL, 0, 0, s1, gen1);
        b.target = -1;
        b.verb = kVerbBlockMacros;
        run(b);
        int n = events(&cursor, ev, 64);
        check(bar && !live("mcr1pall") && strcmp(g_log.order, "K") == 0
                && strncmp(g_log.last_close, "menu    mcr1pall", 16) == 0 && macro[kMacroBar] == 0
                && macro[kMacroBarUp] == 0 && macro[kMacroSet] == 2 && n == 1 && is_event(ev[0], kEvClosed, "mcr1pall")
                && g_e.drain_errors == errors,
            "the block's command with the Ctrl bar up: the bar closed by name (closed{mcr1pall}), then +0x0C and"
            " +0x18 zeroed, +0x0D left alone");
        open_now("mcr1pall");
        open_now("mcr2pall");
        macro[kMacroBar] = 2;
        macro[kMacroBarUp] = 1;
        events(&cursor, ev, 64);
        clear_log();
        run(b);
        n = events(&cursor, ev, 64);
        check(!live("mcr1pall") && !live("mcr2pall") && strcmp(g_log.order, "KK") == 0 && macro[kMacroBar] == 0
                && macro[kMacroBarUp] == 0 && n == 2,
            "both bars up: both closed");
        clear_log();
        run(b);
        const bool nothing = g_log.count == 0 && events(&cursor, ev, 64) == 0;
        macro_ptr = NULL;
        macro[kMacroBar] = 1;
        macro[kMacroBarUp] = 1;
        run(b);
        const bool no_object = macro[kMacroBar] == 1 && macro[kMacroBarUp] == 1 && g_log.count == 0;
        g_e.game.macro_object = NULL;
        run(b);
        check(nothing && no_object && g_log.count == 0 && g_e.drain_errors == errors,
            "with no bar up nothing is closed; with the object's global NULL, or no global at all, nothing is"
            " written and nothing fails");
        macro[kMacroBar] = 0;
        macro[kMacroBarUp] = 0;
    }

    // the ability opener: its pre refuses a kind a handle blocks as a
    // category, or every kind while the window is blocked whole, posting
    // blocked{ability} with the kind; else it keeps the kind for the opened
    // event the open of ability inside the routine posts, and the post
    // forgets it. closed, covered and uncovered carry the controller's list.
    {
        const int a = name("ability");
        uint8_t* actl = controller("ability");
        if (!actl) {
            actl = give_controller("ability", vt_self);
        }
        wr32(actl, kAbilityCategory, 0);
        events(&cursor, ev, 64);
        uint32_t args[3] = {4, 0, 1};
        HuFrame f;
        memset(&f, 0, sizeof(f));
        f.args = args;
        f.user = &g_e;
        g_e.ability_pending = 0;
        const bool kept = a >= 0 && g_e.inv.ability == a && hook_ability_open_pre(&f) == 0 && g_e.ability_pending == 1
            && g_e.ability_pending_kind == 4 && events(&cursor, ev, 64) == 0;
        uint8_t* menu = open_now("ability");
        int n = events(&cursor, ev, 64);
        const bool stamped = menu != NULL && n == 1 && is_event(ev[0], kEvOpened, "ability")
            && event_has_category(ev[0]) && event_category(ev[0]) == 4 && g_e.ability_pending == 0;
        wr32(actl, kAbilityCategory, 4);
        uint8_t* again = open_now("ability");
        n = events(&cursor, ev, 64);
        const bool plain = again == menu && n == 0;
        hook_ability_open_pre(&f);
        hook_ability_open_post(&f);
        wr32(actl, kAbilityCategory, 2);
        open_now("ability");
        n = events(&cursor, ev, 64);
        const bool forgotten = g_e.ability_pending == 0 && n == 1 && is_event(ev[0], kEvOpened, "ability")
            && !event_has_category(ev[0]);
        check(kept && stamped && plain && forgotten,
            "the opener's pre with nothing blocked runs the original and keeps kind 4; the open of ability inside"
            " it posts opened{ability} carrying 4 and forgets the kind; an open of ability already open on the"
            " list it shows posts nothing; on another list, from anywhere but the opener, opened carries none, and"
            " so does one after the opener's post ran with no open inside");

        wr32(actl, kAbilityCategory, 2);
        uint32_t margs[1] = {reinterpret_cast<uint32_t>(menu)};
        HuFrame m;
        memset(&m, 0, sizeof(m));
        m.args = margs;
        m.user = &g_e;
        menu[kMenuClosing] = 0;
        hook_close_pre(&m);
        wr32(menu, kMenuState, kStateDormant);
        n = events(&cursor, ev, 64);
        const bool covered = n == 1 && is_event(ev[0], kEvCovered, "ability") && event_category(ev[0]) == 2
            && g_e.covered[a];
        hook_show_pre(&m);
        wr32(menu, kMenuState, 0);
        n = events(&cursor, ev, 64);
        const bool uncovered = n == 1 && is_event(ev[0], kEvUncovered, "ability") && event_category(ev[0]) == 2;
        menu[kMenuClosing] = 1;
        hook_close_pre(&m);
        n = events(&cursor, ev, 64);
        const bool closed = n == 1 && is_event(ev[0], kEvClosed, "ability") && event_category(ev[0]) == 2;
        menu[kMenuClosing] = 0;
        check(covered && uncovered && closed,
            "covered, uncovered and closed on ability carry the list its controller holds, 2");
        run(cmd(kOpClose, "ability", 0, 0, s1, gen1));
        events(&cursor, ev, 64);

        g_e.holds.set_ability(h1, 2, true);
        g_e.holds.set_ability(h1, 2, true);
        const bool once = g_e.holds.ability_block_count[2] == 1 && g_e.holds.ability_want == (1L << 2)
            && h1.ability_block == (1u << 2) && g_e.holds.want[a] == 0;
        args[0] = 2;
        g_e.ability_pending = 0;
        const bool refused = hook_ability_open_pre(&f) == 1 && g_e.ability_pending == 0
            && events(&cursor, ev, 64) == 1 && is_event(ev[0], kEvBlocked, "ability") && event_category(ev[0]) == 2;
        args[0] = 1;
        const bool other = hook_ability_open_pre(&f) == 0 && g_e.ability_pending == 1 && g_e.ability_pending_kind == 1
            && events(&cursor, ev, 64) == 0;
        hook_ability_open_post(&f);
        g_e.holds.set_ability(h2, 20, true);
        args[0] = 20;
        const bool twenty = hook_ability_open_pre(&f) == 1 && events(&cursor, ev, 64) == 1
            && is_event(ev[0], kEvBlocked, "ability") && event_category(ev[0]) == 20;
        g_e.holds.set(h1, a, kHoldBlock, true);
        args[0] = 1;
        const bool whole = hook_ability_open_pre(&f) == 1 && events(&cursor, ev, 64) == 1
            && is_event(ev[0], kEvBlocked, "ability") && event_category(ev[0]) == 1;
        g_e.holds.set(h1, a, kHoldBlock, false);
        g_e.holds.release(h1, g_e.inv.count);
        const bool released = h1.ability_block == 0 && g_e.holds.ability_block_count[2] == 0
            && g_e.holds.ability_want == (1L << 20) && h2.ability_block == (1u << 20);
        g_e.holds.release(h2, g_e.inv.count);
        args[0] = 2;
        const bool free_again = g_e.holds.ability_want == 0 && g_e.holds.ability_block_count[20] == 0
            && hook_ability_open_pre(&f) == 0 && events(&cursor, ev, 64) == 0;
        hook_ability_open_post(&f);
        drain(g_e);
        check(once && refused && other && twenty && whole && released && free_again,
            "a category hold, counted once however often one handle sets it, has the pre refuse that kind alone,"
            " blocked{ability} carrying it, while another kind runs; a hold on 20 refuses 20; the whole-window"
            " block refuses every kind through the opener, blocked carrying the kind; a release drops a handle's"
            " category holds and leaves the other handle's");
    }

    // 0.11.0: the windows open refuses, the lists of ability and magic, and
    // the sets
    {
        bool known = true;
        for (int i = 0; i < kOpenRefusedCount; ++i) {
            known = known && name(kOpenRefused[i].name) >= 0 && kOpenRefused[i].reason[0];
        }
        check(known && open_refusal("inventor", 1) && open_refusal("mp_invnt", 8) && open_refusal("bank", 3)
                && !open_refusal("equip", 8) && open_refusal("equip", 9) && !open_refusal("persona", 9)
                && open_refusal("guildsho", 9) && !open_refusal("mount", 9) && !open_refusal("money", 9)
                && !open_refusal("ability", 9) && strstr(open_refusal("equip", 9), "open_set('equipment')")
                && strstr(open_refusal("cnqframe", 9), "open_set('region', kind)") && !open_refusal("cnqframe", 8)
                && !open_refusal("searchjo", 1) && !open_refusal("msglist", 1) && !open_refusal("comgenre", 1)
                && strstr(open_refusal("textcol1", 1), "open_set('text_colors', category)"),
            "every window open refuses is in the inventory with a reason; inventor, bank, shop, bluinven, cmbhlst and"
            " mp_invnt refused by every open slot, the rest from engine abi 9, equip and cnqframe pointing at their"
            " sets, textcol1 at text_colors; persona, mount, the search pickers, comgenre and msglist refused by none,"
            " their routes opening them");
        const struct {
            bool ability;
            const char* text;
            int32_t want;
        } lists[] = {
            {true, "job_abilities", 1}, {true, "2", 2}, {true, "pet_commands", 2}, {true, "monstrosity", 20},
            {true, "effusion", 23}, {true, "23", 23}, {true, "5", -1}, {true, "7", -1}, {true, "0", -1},
            {true, "24", -1}, {true, "31", -1}, {true, "", -1}, {true, "white_magic", -1}, {true, "02", 2},
            {false, "white_magic", 1}, {false, "trust", 8}, {false, "8", 8}, {false, "9", -1}, {false, "0", -1},
            {false, "blue_magic", 6}, {false, "job_abilities", -1}, {false, "99999999999", -1},
        };
        bool parsed = true;
        for (size_t i = 0; i < sizeof(lists) / sizeof(lists[0]); ++i) {
            const int32_t got = lists[i].ability ? list_category(kAbilityLists, kAbilityListCount, lists[i].text)
                                                 : list_category(kMagicLists, kMagicListCount, lists[i].text);
            parsed = parsed && got == lists[i].want;
        }
        bool ranged = true;
        for (int i = 0; i < kAbilityListCount; ++i) {
            ranged = ranged && kAbilityLists[i].category >= 1 && kAbilityLists[i].category <= kAbilityListMax
                && kAbilityLists[i].category != 5 && kAbilityLists[i].category != 7;
        }
        for (int i = 0; i < kMagicListCount; ++i) {
            ranged = ranged && kMagicLists[i].category >= 1 && kMagicLists[i].category <= kMagicListMax;
        }
        check(parsed && ranged && strcmp(ability_list_name(20), "monstrosity") == 0
                && strcmp(ability_list_name(3), "weapon_skills") == 0 && ability_list_name(5) == NULL
                && ability_list_name(24) == NULL && kAbilityListCount == 21 && kMagicListCount == 8,
            "the lists: ability's 21 by name or number, 1 to 23 but 5 and 7, 20 monstrosity; magic's 8; a number"
            " past either table, 0, 5, 7 or another window's name none");
        bool sets = find_set("equipment") == kSetEquipment && find_set("ability") == kSetAbility
            && find_set("magic") == kSetMagic && find_set("nosuch") < 0 && find_set(NULL) < 0;
        for (int k = 0; k < kSetCount; ++k) {
            sets = sets && name(kSets[k].opens) >= 0;
            for (int i = 0; kSets[k].windows[i]; ++i) {
                sets = sets && name(kSets[k].windows[i]) >= 0;
            }
        }
        check(sets && kSets[kSetEquipment].takes == kTakesNothing && kSets[kSetAbility].takes == kTakesList
                && kSets[kSetMagic].takes == kTakesList,
            "the sets, each of windows in the inventory; equipment takes nothing, the list sets a list");
    }

    // 0.11.0: every open path refuses what crashes the client: the drain's
    // open checks the table's every-slot names, a slot holding no
    // controller, and a window whose other must be open; its close and the
    // block's leave gift open while trade is open, and no block holds gift
    {
        bool every = true;
        int crash = 0;
        for (int i = 0; i < kOpenRefusedCount; ++i) {
            crash += kOpenRefused[i].since == 1;
            every = every && (kOpenRefused[i].since == 1 || kOpenRefused[i].since == 9);
        }
        const char* const all_paths[] = {"netwait", "keypad2", "roomlist", "mogdoor", "bankmenu", "ok", "mp_dead",
                                         "holdtime", "mcrledit", "mcresed", "loot", "lootope", "bluequip", "prvdungp",
                                         "conquer1", "beseige2", "campsan", "camparea", "chocobor", "racecard",
                                         "mes1rcv", "mes2frnd", "auc1", "shopmain", "guildsho", "itmstora", "plnt4"};
        for (size_t i = 0; i < sizeof(all_paths) / sizeof(all_paths[0]); ++i) {
            every = every && open_refusal(all_paths[i], 1) != NULL;
        }
        check(every && crash >= 40 && !open_refusal("persona", 8) && block_refusal("gift") && !block_refusal("trade")
                && block_refusal("query") && child_spec("mcrselop") && child_spec("itmsortw")
                && !open_refusal("itmsortw", 1) && !open_refusal("iuse", 9) && open_refusal("map0", 9)
                && !open_refusal("map0", 8) && pair_spec(kCloseWhileOpen, kCloseWhileOpenCount, "gift"),
            "the crash and server windows refused from engine abi 1, the rest from 9, the sets' main windows among"
            " them; gift never blocked; mcrselop, itmsortw and iuse children of their parents, gift closed only with"
            " trade closed");
        bool pairs = true;
        char reason[kErrorReason];
        for (int i = 0; i < kChildCount; ++i) {
            pairs = pairs && name(kChildren[i].name) >= 0 && kChildren[i].parents[0]
                && !open_refusal(kChildren[i].name, 9);
            for (int k = 0; kChildren[i].parents[k]; ++k) {
                pairs = pairs && name(kChildren[i].parents[k]) >= 0;
            }
        }
        child_reason(*child_spec("subwindo"), reason, sizeof(reason));
        const bool many = strcmp(reason, "subwindo needs chatctrl, ability, mount or magic open") == 0;
        child_reason(*child_spec("gift"), reason, sizeof(reason));
        const bool why = strstr(reason, "gift needs trade open: its input moves trade's cursor") == reason;
        check(many && why, "a child's refusal names its parents, any one of them enough, and why where alone it would"
            " crash the client");
        for (int i = 0; i < kCloseWhileOpenCount; ++i) {
            pairs = pairs && name(kCloseWhileOpen[i].name) >= 0 && name(kCloseWhileOpen[i].other) >= 0;
        }
        check(pairs, "every window the pairs name is in the inventory");

        if (!controller("loot")) {
            give_controller("loot", vt_self);
        }
        const int calls = g_open_calls;
        run(cmd(kOpOpen, "loot", 0, 0, s1, gen1));
        int n = events(&cursor, ev, 64);
        const bool table = n == 1 && is_error(ev[0], s1, gen1, "", "loot", "loot cannot be opened alone")
            && g_open_calls == calls && !live("loot");
        const bool none = controller("fulllog") == NULL;
        run(cmd(kOpOpen, "fulllog", 0, 0, s1, gen1));
        n = events(&cursor, ev, 64);
        const bool missing = none && n == 1
            && is_error(ev[0], s1, gen1, "", "fulllog", "fulllog has no controller now")
            && g_open_calls == calls && controller_missing(g_e, kPlain, name("fulllog"))
            && !controller_missing(g_e, kPlain, name("equip"));
        check(table && missing, "the drain's open: a window the table refuses from abi 1, and one whose slot holds"
            " no controller, refused with an error and never handed to the game's open");

        static const char* const pair[] = {"gift", "trade"};
        for (int i = 0; i < 2; ++i) {
            if (!controller(pair[i])) {
                give_controller(pair[i], vt_self);
            }
        }
        run(cmd(kOpOpen, "gift", 0, 0, s1, gen1));
        n = events(&cursor, ev, 64);
        const bool alone = n == 1 && is_error(ev[0], s1, gen1, "", "gift", "gift needs trade open") && !live("gift");
        open_now("trade");
        events(&cursor, ev, 64);
        run(cmd(kOpOpen, "gift", 0, 0, s1, gen1));
        n = events(&cursor, ev, 64);
        const bool with = n == 1 && is_event(ev[0], kEvOpened, "gift") && live("gift");
        run(cmd(kOpClose, "gift", 0, 0, s1, gen1));
        n = events(&cursor, ev, 64);
        const bool kept = n == 1 && is_error(ev[0], s1, gen1, "", "gift", "gift cannot be closed while trade is open")
            && live("gift");
        run(cmd(kOpClose, "trade", 0, 0, s1, gen1));
        n = events(&cursor, ev, 64);
        check(alone && with && kept && n == 2 && is_event(ev[0], kEvClosed, "trade") && is_event(ev[1], kEvClosed, "gift")
                && !live("gift"),
            "gift opened only while trade is open, and closed only while it is not; each refusal an error, the"
            " window left as it was; trade's close closes it after, once trade is gone");
    }

    // 0.11.0: open passes the open routine's last argument as the game's
    // openers pass it, 0 but for the four windows they open parked: a
    // window shows at once over another of its overlap group, where with 1
    // it would open dormant under it; targetwi parks under ut_point as the
    // game's own opens park it, and comes out through the show path
    {
        static const char* const four[] = {"titlewin", "titlehan", "targetwi", "ut_point"};
        for (int i = 0; i < 4; ++i) {
            if (!controller(four[i])) {
                give_controller(four[i], vt_self);
            }
            if (live(four[i])) {
                run(cmd(kOpClose, four[i], 0, 0, s1, gen1));
                destroy(four[i]);
            }
        }
        events(&cursor, ev, 64);
        const bool overlaps = open_overlap("targetwi") == 1 && open_overlap("gaugewin") == 1
            && open_overlap("playermo") == 1 && open_overlap("mp_pmode") == 1 && open_overlap("titlehan") == 0
            && open_overlap("logwindo") == 0;
        open_now("titlewin");
        events(&cursor, ev, 64);
        run(cmd(kOpOpen, "titlehan", 0, 0, s1, gen1));
        int n = events(&cursor, ev, 64);
        uint8_t* han = live("titlehan");
        const bool shown = g_open_overlap == 0 && han && rd32(han, kMenuState) != kStateDormant && !han[kMenuParked]
            && n == 1 && is_event(ev[0], kEvOpened, "titlehan");
        uint32_t args[1] = {reinterpret_cast<uint32_t>(han)};
        HuFrame f;
        memset(&f, 0, sizeof(f));
        f.args = args;
        f.user = &g_e;
        hook_show_pre(&f);
        const bool quiet = events(&cursor, ev, 64) == 0;
        check(overlaps && shown && quiet, "open(titlehan) over titlewin, its overlap group's: the open routine's last"
            " argument 0, as the game's openers pass it, and titlehan shows at once with its opened event; the show"
            " path finds nothing to bring out, no uncovered");

        open_now("ut_point");
        events(&cursor, ev, 64);
        run(cmd(kOpOpen, "targetwi", 0, 0, s1, gen1));
        n = events(&cursor, ev, 64);
        uint8_t* target = live("targetwi");
        const bool parked = g_open_overlap == 1 && target && rd32(target, kMenuState) == kStateDormant
            && target[kMenuParked] == 1 && n == 1 && is_event(ev[0], kEvOpened, "targetwi");
        run(cmd(kOpClose, "ut_point", 0, 0, s1, gen1));
        destroy("ut_point");
        events(&cursor, ev, 64);
        args[0] = reinterpret_cast<uint32_t>(target);
        hook_show_pre(&f);
        wr32(target, kMenuState, 0);
        n = events(&cursor, ev, 64);
        check(parked && n == 1 && is_event(ev[0], kEvUncovered, "targetwi"),
            "open(targetwi) under ut_point: 1, as the game's own openers pass it, and targetwi opens dormant under"
            " it; once ut_point goes, the show path brings it out: uncovered{targetwi}");
        for (int i = 0; i < 4; ++i) {
            if (live(four[i])) {
                run(cmd(kOpClose, four[i], 0, 0, s1, gen1));
                destroy(four[i]);
            }
        }
        events(&cursor, ev, 64);
    }

    // 0.11.0: a list alone, a set, the subwindo the magic list's rebuild
    // opens kept from opening, the game's refusals, and a list the game
    // closes again in the next frame
    {
        static const char* const needed[] = {"magic", "mgcsortw", "blusortw", "abisortw", "subwindo", "money",
                                             "inventor", "iteminfo", "persona", "equip", "ability"};
        for (size_t i = 0; i < sizeof(needed) / sizeof(needed[0]); ++i) {
            if (!controller(needed[i])) {
                give_controller(needed[i], vt_self);
            }
        }
        static const char* const family[] = {"magic", "mgcsortw", "blusortw", "abisortw", "subwindo", "money",
                                             "inventor", "iteminfo", "persona", "equip", "ability"};
        const int count = static_cast<int>(sizeof(family) / sizeof(family[0]));
        // The family closed through the engine's own close.
        auto close_all = [&]() {
            for (int i = 0; i < count; ++i) {
                if (live(family[i])) {
                    g_e.commands.push(cmd(kOpClose, family[i], 0, 0, s1, gen1));
                }
            }
            drain(g_e);
            events(&cursor, ev, 64);
        };
        // Which of the family are open, as names joined by ' '.
        auto opened_now = [&](char* out, size_t size) {
            out[0] = '\0';
            for (int i = 0; i < count; ++i) {
                if (open_menu(g_e, kPlain, name(family[i]), NULL)) {
                    const size_t used = strlen(out);
                    snprintf(out + used, size - used, "%s%s", used ? " " : "", family[i]);
                }
            }
            return out;
        };
        auto opened_events = [&](char* out, size_t size, int n) {
            out[0] = '\0';
            for (int i = 0; i < n; ++i) {
                if (event_type(ev[i]) == kEvOpened) {
                    const size_t used = strlen(out);
                    snprintf(out + used, size - used, "%s%s", used ? " " : "", g_e.inv.names[event_name(ev[i])].name);
                }
            }
            return out;
        };
        char now[160];
        char seen[160];
        close_all();

        Command list = cmd(kOpOpenList, "ability", 0, 0, s1, gen1, 2);
        list.verb = kVerbOpen;
        int calls = g_ability_lists;
        run(list);
        int n = events(&cursor, ev, 64);
        const bool alone = g_ability_lists == calls + 1 && strcmp(opened_now(now, sizeof(now)), "ability") == 0
            && n == 1 && is_event(ev[0], kEvOpened, "ability") && event_category(ev[0]) == 2
            && rd32(controller("ability"), kAbilityCategory) == 2;
        check(alone, "open('ability', 2): the opener with no sort and no pane, through its hook: ability alone, opened"
            " carrying the list");
        close_all();

        Command set = cmd(kOpOpenSet, "ability", kSetAbility, 0, s1, gen1, 1);
        set.verb = kVerbOpenSet;
        run(set);
        n = events(&cursor, ev, 64);
        check(strcmp(opened_now(now, sizeof(now)), "abisortw subwindo ability") == 0
                && strcmp(opened_events(seen, sizeof(seen), n), "abisortw ability subwindo") == 0,
            "open_set('ability', 1): the opener with the sort and the pane: abisortw, ability and subwindo, each its"
            " own opened event in the game's order");
        close_all();

        const int sub = name("subwindo");
        const LONG sub_opens = g_e.instance[sub];
        Command magic = cmd(kOpOpenList, "magic", 0, 0, s1, gen1, 1);
        magic.verb = kVerbOpen;
        calls = g_magic_lists;
        run(magic);
        n = events(&cursor, ev, 64);
        const bool kept = g_magic_lists == calls + 1 && strcmp(opened_now(now, sizeof(now)), "magic") == 0
            && n == 1 && is_event(ev[0], kEvOpened, "magic") && g_e.instance[sub] == sub_opens
            && g_e.suppressed == 0 && !live("subwindo");
        const bool after = open_key("subwindo") != NULL && events(&cursor, ev, 64) == 1
            && is_event(ev[0], kEvOpened, "subwindo");
        check(kept && after, "open('magic', 1): magic alone; the rebuild's open of subwindo refused with no event and"
            " not counted, no instance made; subwindo opens again once the opener has returned");
        uint8_t* pane = live("subwindo");
        run(magic);
        n = events(&cursor, ev, 64);
        check(live("subwindo") == pane && n == 0,
            "open('magic', 1) with magic and subwindo already up leaves both as they were: no open of subwindo, and"
            " no opened for magic, which the events report open already");
        close_all();
        g_e.holds.set(h2, sub, kHoldBlock, true);
        run(magic);
        n = events(&cursor, ev, 64);
        g_e.holds.set(h2, sub, kHoldBlock, false);
        check(n == 1 && is_event(ev[0], kEvOpened, "magic") && open_menu(g_e, kPlain, name("magic"), NULL),
            "with subwindo blocked, open('magic', 1) opens the list and posts no blocked{subwindo}");
        close_all();

        Command blue = cmd(kOpOpenSet, "magic", kSetMagic, 0, s1, gen1, 6);
        blue.verb = kVerbOpenSet;
        run(blue);
        n = events(&cursor, ev, 64);
        const bool blue_set = strcmp(opened_now(now, sizeof(now)), "magic blusortw subwindo") == 0
            && strcmp(opened_events(seen, sizeof(seen), n), "magic blusortw subwindo") == 0;
        close_all();
        Command white = cmd(kOpOpenSet, "magic", kSetMagic, 0, s1, gen1, 1);
        white.verb = kVerbOpenSet;
        run(white);
        n = events(&cursor, ev, 64);
        check(blue_set && strcmp(opened_now(now, sizeof(now)), "magic mgcsortw subwindo") == 0,
            "open_set('magic', 6): magic, blusortw and subwindo; open_set('magic', 1): mgcsortw for the sort");
        close_all();

        Command equipment = cmd(kOpOpenSet, "equip", kSetEquipment, 0, s1, gen1, 0);
        equipment.verb = kVerbOpenSet;
        uint8_t* ectl = controller("equip");
        const uint16_t layout_word = rd16(ectl, 0x1A);
        wr16(ectl, 0x1A, 0);
        controller("persona")[0x1C] = 0;
        g_monstrosity = 0;
        calls = g_equipment_opens;
        run(equipment);
        n = events(&cursor, ev, 64);
        const bool whole = g_equipment_opens == calls + 1 && g_log.equipment_this == ectl
            && controller("persona")[0x1C] == 2
            && strcmp(opened_now(now, sizeof(now)), "money inventor iteminfo persona equip") == 0
            && strcmp(opened_events(seen, sizeof(seen), n), "persona money inventor iteminfo equip") == 0;
        bool no_error = true;
        for (int i = 0; i < n; ++i) {
            no_error = no_error && event_type(ev[i]) != kEvError;
        }
        check(whole && no_error, "open_set('equipment'): the set's opener on equip's controller, the Monstrosity"
            " check's upper bytes ignored: persona in mode 2, money, inventor, iteminfo and equip, each its opened"
            " event; no error");
        close_all();

        wr16(ectl, 0x1A, 1);
        run(equipment);
        n = events(&cursor, ev, 64);
        wr16(ectl, 0x1A, layout_word);
        check(n == 1 && is_error(ev[0], s1, gen1, "open_set", "equipment", "the game refused it") && !live("equip")
                && !live("persona"),
            "the opener's guard returning with nothing open: error{open_set equipment, the game refused it}");
        g_monstrosity = 1;
        calls = g_equipment_opens;
        run(equipment);
        n = events(&cursor, ev, 64);
        g_monstrosity = 0;
        check(g_equipment_opens == calls && n == 1 && is_error(ev[0], s1, gen1, "open_set", "equipment", "Monstrosity")
                && !live("equip"),
            "in Monstrosity the opener is not run, as the game's shortcut does not run it: an error naming Monstrosity");

        // the game closing a list again at once: reported at the next drain
        run(list);
        events(&cursor, ev, 64);
        char akey[kKeyLen + 1];
        make_key(g_e, name("ability"), akey);
        fake_close(g_mcb, NULL, akey);
        drain(g_e);
        n = events(&cursor, ev, 64);
        bool refused = false;
        for (int i = 0; i < n; ++i) {
            refused = refused || is_error(ev[i], s1, gen1, "open", "ability", "closed again at once");
        }
        drain(g_e);
        const bool once = events(&cursor, ev, 64) == 0;
        check(refused && once, "a list the game closes again before the next frame: error{open ability, the game"
            " refused it: it closed again at once}, once");
        Command set_again = set;
        run(set_again);
        events(&cursor, ev, 64);
        fake_close(g_mcb, NULL, akey);
        drain(g_e);
        n = events(&cursor, ev, 64);
        refused = false;
        for (int i = 0; i < n; ++i) {
            refused = refused || is_error(ev[i], s1, gen1, "open_set", "ability", "closed again at once");
        }
        check(refused, "the same for a set, named by the set: error{open_set ability, ...}");
        close_all();
        run(list);
        events(&cursor, ev, 64);
        run(cmd(kOpClose, "ability", 0, 0, s1, gen1));
        drain(g_e);
        n = events(&cursor, ev, 64);
        bool quiet = true;
        for (int i = 0; i < n; ++i) {
            quiet = quiet && event_type(ev[i]) != kEvError;
        }
        run(list);
        drain(g_e);
        drain(g_e);
        events(&cursor, ev, 64);
        fake_close(g_mcb, NULL, akey);
        drain(g_e);
        n = events(&cursor, ev, 64);
        for (int i = 0; i < n; ++i) {
            quiet = quiet && event_type(ev[i]) != kEvError;
        }
        run(list);
        open_key("ability");
        events(&cursor, ev, 64);
        fake_close(g_mcb, NULL, akey);
        drain(g_e);
        n = events(&cursor, ev, 64);
        for (int i = 0; i < n; ++i) {
            quiet = quiet && event_type(ev[i]) != kEvError;
        }
        check(quiet, "no error for a close the engine made itself, for a close after the two frames the check"
            " looks, or for one after the game opened the list again");
        close_all();
    }

    // 0.11.0: the sets past equipment, ability and magic, each through its
    // opener as the player's route runs it and with what that runs first:
    // the map, the blue magic set and the automaton's on their controllers,
    // the linkshell list behind its row's check, the instincts in
    // Monstrosity alone, synthesis for a crystal and the maze editor for a
    // tabula at an inventory index as the item use reads its type, the macro
    // editor on its object; each refusal an error, nothing opened
    {
        static const char* const family[] = {"mapframe", "map0", "persona", "bluehelp", "bluinven", "bluepoin",
                                             "bluequip", "statpup", "automato", "link5", "link6", "mp_stat", "level",
                                             "mp_cstm", "mp_mncst", "inventor", "iteminfo", "tskill1", "prvdungp",
                                             "mcrbedit", "mcr1edit", "mcr2edit"};
        const int count = static_cast<int>(sizeof(family) / sizeof(family[0]));
        for (int i = 0; i < count; ++i) {
            if (!controller(family[i])) {
                give_controller(family[i], vt_self);
            }
        }
        auto close_all = [&]() {
            for (int i = 0; i < count; ++i) {
                if (live(family[i])) {
                    game_closes(family[i]);
                    destroy(family[i]);
                }
            }
            drain(g_e);
            drain(g_e);
            events(&cursor, ev, 64);
        };
        // The opened events among the first n, names joined by ' '; any
        // other event a '!'.
        auto opened_events = [&](char* out, size_t size, int n) {
            out[0] = '\0';
            for (int i = 0; i < n; ++i) {
                const size_t used = strlen(out);
                snprintf(out + used, size - used, "%s%s", used ? " " : "",
                    event_type(ev[i]) == kEvOpened ? g_e.inv.names[event_name(ev[i])].name : "!");
            }
            return out;
        };
        auto set_cmd = [&](const char* opens, int set, uint32_t value) {
            Command c = cmd(kOpOpenSet, opens, set, 0, s1, gen1, value);
            c.verb = kVerbOpenSet;
            return c;
        };
        char seen[256];
        close_all();
        memset(&g_set, 0, sizeof(g_set));

        run(set_cmd("map0", kSetMap, 0));
        int n = events(&cursor, ev, 64);
        check(g_set.calls == 1 && g_set.args[0] == 0 && strcmp(opened_events(seen, sizeof(seen), n), "mapframe map0") == 0,
            "open_set('map'): the map's opener with mode 0, as the main menu's row runs it: mapframe and map0, each its"
            " opened event");
        close_all();

        run(set_cmd("bluequip", kSetSpells, 0));
        n = events(&cursor, ev, 64);
        const bool spells = g_set.calls == 2 && g_set.self == controller("bluequip") && controller("persona")[0x1C] == 2
            && strcmp(opened_events(seen, sizeof(seen), n), "persona bluehelp bluinven bluepoin bluequip") == 0;
        close_all();
        run(set_cmd("automato", kSetAttachments, 0));
        n = events(&cursor, ev, 64);
        check(spells && g_set.calls == 3 && g_set.self == controller("automato")
                && strcmp(opened_events(seen, sizeof(seen), n), "bluehelp bluinven statpup automato") == 0,
            "open_set('set_spells') and open_set('attachments'): each opener on the controller in bluequip's and"
            " automato's slot, opening its windows in the game's order");
        close_all();
        g_set.refuse = true;
        run(set_cmd("bluequip", kSetSpells, 0));
        n = events(&cursor, ev, 64);
        g_set.refuse = false;
        check(g_set.calls == 4 && n == 1 && is_error(ev[0], s1, gen1, "open_set", "set_spells", "the game refused it")
                && !live("bluequip"),
            "the blue magic set's opener refusing, as it does but for a blue mage: error{open_set set_spells, the game"
            " refused it}, nothing open");

        g_set.check = 1;
        run(set_cmd("link5", kSetLinkshell, 0));
        n = events(&cursor, ev, 64);
        const bool held = g_set.calls == 4 && g_set.checks == 1 && g_set.check_args[0] == 1 && g_set.check_args[1] == 1
            && n == 1 && is_error(ev[0], s1, gen1, "open_set", "linkshell", "the game refused it") && !live("link5");
        g_set.check = 0;
        run(set_cmd("link5", kSetLinkshell, 0));
        n = events(&cursor, ev, 64);
        check(held && g_set.calls == 5 && g_set.self == controller("link5") && g_set.args[0] == 0 && g_set.args[1] == 0
                && g_set.args[2] == 0 && strcmp(opened_events(seen, sizeof(seen), n), "link6 link5") == 0,
            "open_set('linkshell'): the row's check (1, 1) refusing leaves the opener unrun, an error; passing, the"
            " opener on link5's controller with (0, 0, 0): link6 and link5");
        // link5 as the plain list asks nothing: a close from engine abi 4 on
        // closes it; while the event's choice is pending, the drain refuses it
        uint8_t* lctl = controller("link5");
        wr32(lctl, kLink5Mode, 0);
        wr32(lctl, kLink5Callback, 0);
        Command close = cmd(kOpClose, "link5", 0, 0, s1, gen1);
        close.verb = kVerbClose;
        close.flags = kCmdPrompt;
        run(close);
        n = events(&cursor, ev, 64);
        const bool plain = n == 1 && is_event(ev[0], kEvClosed, "link5") && !live("link5");
        open_key("link5");
        events(&cursor, ev, 64);
        g_link5_latch = 1;
        wr32(lctl, kLink5Mode, 1);
        wr32(lctl, kLink5Callback, reinterpret_cast<uint32_t>(&fake_link5_callback));
        run(close);
        n = events(&cursor, ev, 64);
        g_link5_latch = 0;
        wr32(lctl, kLink5Mode, 0);
        wr32(lctl, kLink5Callback, 0);
        check(plain && n == 1
                && is_error(ev[0], s1, gen1, "close", "link5", "link5 is a prompt the game waits on; use cancel('link5')")
                && live("link5"),
            "close(link5) showing the plain list asks nothing and closes as Escape does; with the event's choice"
            " pending the game thread refuses it, pointing at cancel, the window left open");
        close_all();

        g_monstrosity = 0;
        run(set_cmd("mp_mncst", kSetInstincts, 0));
        n = events(&cursor, ev, 64);
        const bool outside = g_set.calls == 5 && n == 1
            && is_error(ev[0], s1, gen1, "open_set", "instincts", "only in Monstrosity");
        g_monstrosity = 1;
        run(set_cmd("mp_mncst", kSetInstincts, 0));
        n = events(&cursor, ev, 64);
        g_monstrosity = 0;
        check(outside && g_set.calls == 6
                && strcmp(opened_events(seen, sizeof(seen), n), "mp_stat level mp_cstm mp_mncst") == 0,
            "open_set('instincts'): outside Monstrosity an error and the opener unrun, as the main menu's row runs the"
            " equipment's there; in it the opener: mp_stat, level, mp_cstm and mp_mncst");
        close_all();

        // the inventory: 5 a crystal, 6 an M tabula, 7 an R tabula, 8 a plain
        // item, 9 an item with no data, 10 empty
        uint8_t saved[6 * kItemRecordBytes];
        uint8_t* records = g_pd + kPdInventory + 5 * kItemRecordBytes;
        memcpy(saved, records, sizeof(saved));
        memset(records, 0, sizeof(saved));
        const uint16_t ids[5] = {4096, 28672, 28704, 4509, 9999};
        for (int i = 0; i < 5; ++i) {
            wr16(records + i * kItemRecordBytes, 0, ids[i]);
            records[i * kItemRecordBytes + 2] = static_cast<uint8_t>(5 + i);
            wr32(records + i * kItemRecordBytes, kItemCount, 1);
        }
        uint8_t* inventor = controller("inventor");
        const uint32_t list = rd32(inventor, kInventorList);
        wr32(inventor, kInventorList, 0xBAADF00Du);
        g_set.check = 1;
        g_policy = reinterpret_cast<uint8_t*>(const_cast<void**>(kBadList));
        memset(&g_route, 0, sizeof(g_route));
        const int checks = g_set.checks;
        run(set_cmd("tskill1", kSetSynthesis, 5));
        n = events(&cursor, ev, 64);
        check(g_set.calls == 6 && g_set.checks == checks && strcmp(g_route.order, "items_view inventor_open") == 0
                && n >= 1 && is_error(ev[n - 1], s1, gen1, "open_set", "synthesis",
                "the game's Items row left inventor with no list of the game's")
                && !live("tskill1") && !live("inventor") && !live("iteminfo"),
            "open_set('synthesis', 5) runs the main menu's Items row first; a row's list that is no list of the"
            " game's, which the synthesis's cancel would reopen inventor with: what the row opened closed again by"
            " the game's close, then an error, the check and the opener unrun");
        open_now("inventor");
        events(&cursor, ev, 64);
        memset(&g_route, 0, sizeof(g_route));
        run(set_cmd("tskill1", kSetSynthesis, 5));
        n = events(&cursor, ev, 64);
        check(n >= 1 && is_error(ev[n - 1], s1, gen1, "open_set", "synthesis",
                "the game's Items row left inventor with no list of the game's")
                && live("inventor") && !live("iteminfo"),
            "the same refusal with inventor open before the Items row: inventor left open, iteminfo, which the row"
            " opened, closed again");
        close_all();
        g_policy = reinterpret_cast<uint8_t*>(const_cast<void**>(kList));
        wr32(inventor, kInventorList, 0xBAADF00Du);
        inventor[kItemsByte] = 0;
        memset(&g_route, 0, sizeof(g_route));
        run(set_cmd("tskill1", kSetSynthesis, 5));
        n = events(&cursor, ev, 64);
        check(g_set.calls == 7 && g_set.self == controller("tskill1") && g_set.args[0] == 5 && g_set.args[1] == 4096
                && g_set.args[2] == 0 && g_set.check_args[0] == 0x77 && g_set.check_args[1] == 0
                && strcmp(g_route.order, "items_view inventor_open") == 0 && g_view_self == g_items_object
                && g_view_args[0] == 0 && g_view_args[1] == 1 && inventor[kItemsByte] == 1
                && rd32(inventor, kInventorList) == reinterpret_cast<uint32_t>(kList)
                && strcmp(opened_events(seen, sizeof(seen), n), "inventor iteminfo tskill1") == 0,
            "open_set('synthesis', 5) with a crystal at 5: the Items row (the item view (0, 1) on its object,"
            " inventor's byte set 1, inventor opened with the row's list: inventor, iteminfo), the item use's check"
            " (0x77, 0), then the synthesis's opener on tskill1's controller with the index, the crystal's id and"
            " mode 0: inventor and iteminfo again, no news, then tskill1");
        close_all();
        static const struct {
            uint32_t index;
            const char* reason;
        } wrong[] = {
            {6, "the item at index 6 is not a crystal"},
            {8, "the item at index 8 is not a crystal"},
            {9, "the game has no data for the item at index 9"},
            {10, "the inventory has no item at index 10"},
        };
        int refused = 0;
        for (size_t k = 0; k < sizeof(wrong) / sizeof(wrong[0]); ++k) {
            run(set_cmd("tskill1", kSetSynthesis, wrong[k].index));
            n = events(&cursor, ev, 64);
            refused += n == 1 && is_error(ev[0], s1, gen1, "open_set", "synthesis", wrong[k].reason);
        }
        g_set.check = 0;
        run(set_cmd("tskill1", kSetSynthesis, 5));
        n = events(&cursor, ev, 64);
        const bool checked = n == 3 && strcmp(opened_events(seen, sizeof(seen), 2), "inventor iteminfo") == 0
            && is_error(ev[2], s1, gen1, "open_set", "synthesis", "the game refused it");
        g_set.check = 1;
        g_set.refuse = true;
        run(set_cmd("tskill1", kSetSynthesis, 5));
        n = events(&cursor, ev, 64);
        g_set.refuse = false;
        check(refused == 4 && checked && g_set.calls == 8 && n == 1
                && is_error(ev[0], s1, gen1, "open_set", "synthesis", "the game refused it") && !live("tskill1"),
            "synthesis refused for a tabula, a plain item, an item with no data and an empty index, before the Items"
            " row and the opener; by the item use's check after the Items row, the opener unrun; and by the opener's"
            " own al 0");

        run(set_cmd("prvdungp", kSetTabula, 6));
        n = events(&cursor, ev, 64);
        const bool m = g_set.calls == 9 && g_set.self == controller("prvdungp") && g_set.args[0] == 0
            && g_set.args[1] == 6 && strcmp(opened_events(seen, sizeof(seen), n), "prvdungp bluehelp bluinven") == 0;
        close_all();
        run(set_cmd("prvdungp", kSetTabula, 7));
        n = events(&cursor, ev, 64);
        const bool r = g_set.calls == 10 && g_set.args[0] == 1 && g_set.args[1] == 7 && n == 3;
        close_all();
        run(set_cmd("prvdungp", kSetTabula, 5));
        n = events(&cursor, ev, 64);
        check(m && r && g_set.calls == 10 && n == 1
                && is_error(ev[0], s1, gen1, "open_set", "tabula", "the item at index 5 is not a Maze Tabula"),
            "open_set('tabula', 6) with an M tabula: the maze editor's opener on prvdungp's controller in mode 0 with"
            " the index: prvdungp, bluehelp, bluinven; an R tabula in mode 1; a crystal refused, the opener unrun");
        memcpy(records, saved, sizeof(saved));
        wr32(inventor, kInventorList, list);

        run(set_cmd("mcrbedit", kSetMacros, 0));
        n = events(&cursor, ev, 64);
        const bool editor = g_set.calls == 11 && g_set.self == g_macro_editor
            && strcmp(opened_events(seen, sizeof(seen), n), "mcrbedit mcr2edit mcr1edit") == 0;
        close_all();
        g_macro_editor_ptr = NULL;
        run(set_cmd("mcrbedit", kSetMacros, 0));
        n = events(&cursor, ev, 64);
        g_macro_editor_ptr = g_macro_editor;
        check(editor && g_set.calls == 11 && n == 1
                && is_error(ev[0], s1, gen1, "open_set", "macros", "the game's macro editor does not exist yet"),
            "open_set('macros'): the macro editor's opener on the object its global holds: mcrbedit and the two"
            " palettes; with no object yet an error, the opener unrun");

        bool sets = find_set("map") == kSetMap && find_set("set_spells") == kSetSpells
            && find_set("attachments") == kSetAttachments && find_set("linkshell") == kSetLinkshell
            && find_set("instincts") == kSetInstincts && find_set("synthesis") == kSetSynthesis
            && find_set("tabula") == kSetTabula && find_set("macros") == kSetMacros && find_set("friend") == kSetFriend
            && find_set("mail") == kSetMail && find_set("mount") == kSetMount && find_set("compare") == kSetCompare
            && find_set("status") == kSetStatus && find_set("region") == kSetRegion
            && find_set("text_colors") == kSetTextColors && kSetCount == 18
            && kSets[kSetSynthesis].takes == kTakesItem && kSets[kSetTabula].takes == kTakesItem
            && kSets[kSetMap].takes == kTakesNothing && kSets[kSetCompare].takes == kTakesItemPanel
            && kSets[kSetRegion].takes == kTakesChoice && kSets[kSetTextColors].takes == kTakesChoice;
        for (int k = 0; k < kSetCount; ++k) {
            sets = sets && set_resolved(g_e, k);
        }
        check(sets, "the eighteen sets, synthesis and tabula taking an item, compare an item panel's, region and"
            " text_colors a choice, every one's opener resolved here");
        close_all();
    }

    // 0.11.0: open(name, argument): the argument as the engine reads it, each
    // route as the game's own menu row, command or list runs it, the window
    // its opener brings along held off, and the sets past macros
    {
        OpenArg a;
        RouteArgs r;
        char why[kErrorReason];
        const bool parsed = parse_open_arg(NULL, &a) && a.count == 0 && parse_open_arg("Main", &a) && a.count == 1
            && !a.table && strcmp(a.value[0], "main") == 0 && parse_open_arg("bag=Inventory;slot=5", &a)
            && a.table && a.count == 2 && strcmp(arg_field(a, "bag"), "inventory") == 0
            && strcmp(arg_field(a, "slot"), "5") == 0 && !arg_field(a, "item") && !parse_open_arg("a=b;c", &a)
            && !parse_open_arg("a;b", &a) && !parse_open_arg("=x", &a) && !parse_open_arg("a=", &a)
            && !parse_open_arg("a=b;", &a) && !parse_open_arg("a=1;b=2;c=3;d=4;e=5", &a);
        check(parsed, "an argument: a word, lowercased, or key=value fields joined by ';'; a field with no key or"
            " value, a word beside fields, a trailing ';' and more than four fields are not arguments");
        int32_t number = 0;
        check(arg_number("-12", &number) && number == -12 && arg_number("2147483647", &number) && !arg_number("", &number)
                && !arg_number("2147483648", &number) && !arg_number("1x", &number) && !arg_number("-", &number),
            "a whole number in digits, a leading '-' allowed, within 32 bits");
        struct Case {
            const char* window;
            const char* text;
            bool ok;
            int16_t x;
            int16_t y;
            uint32_t value;
            const char* why;
        };
        static const Case cases[] = {
            {"mogpost", NULL, true, 0, 0, 1, NULL},
            {"mogpost", "nomad", true, 0, 0, 2, NULL},
            {"mogpost", "2", true, 0, 0, 2, NULL},
            {"mogpost", "post", false, 0, 0, 0, "no such mode of mogpost: post ('mog_house' or 'nomad')"},
            {"jobcselu", NULL, false, 0, 0, 0, "jobcselu needs a job to change: open('jobcselu', 'main')"},
            {"jobcselu", "support", true, 0, 0, 2, NULL},
            {"jbpcat", "thf", true, 0, 0, 6, NULL},
            {"jbpcat", "22", true, 0, 0, 22, NULL},
            {"jbpcat", "23", false, 0, 0, 0, "no such job of jbpcat: 23"},
            {"fp_cat2", "7", true, 0, 0, 7, NULL},
            {"fp_cat2", "8", false, 0, 0, 0, "no such category of fp_cat2: 8 (1 to 7)"},
            {"conf11l", NULL, true, 0, 0, 1, NULL},
            {"conf11l", "3", false, 0, 0, 0, "(1 to 2)"},
            {"quest00", "Ranking Information", true, 0, 0, 3, NULL},
            {"quest00", "unity information", true, 0, 0, 2, NULL},
            {"quest00", "quests", false, 0, 0, 0,
             "no such mode of quest00: quests (1, 'Unity Information' or 'Ranking Information')"},
            {"quest01", "2", true, 0, 0, 2, NULL},
            {"quest01", "completed", false, 0, 0, 0, "no such list of quest01: completed (1, 2, 3, 4, 5, 6 or 7)"},
            {"quest01", "8", false, 0, 0, 0, "no such list of quest01"},
            {"miss00", NULL, true, 0, 0, kMissionNation, NULL},
            {"miss00", "RISE OF THE ZILART", true, 0, 0, 4, NULL},
            {"miss00", "The Voracious Resurgence", true, 0, 0, 12, NULL},
            {"miss00", "san d'oria", true, 0, 0, 1, NULL},
            {"miss00", "15", true, 0, 0, 15, NULL},
            {"miss00", "16", false, 0, 0, 0, "no such log of miss00: 16 ('San d'Oria', 'Bastok', 'Windurst'"},
            {"miss00", "zilart", false, 0, 0, 0, "no such log of miss00: zilart ('San d'Oria', 'Bastok'"},
            {"miss00", "nation", false, 0, 0, 0, "no such log of miss00: nation"},
            {"searchjo", NULL, true, 0, 0, 1, NULL},
            {"scchar", "refine", true, 0, 0, 2, NULL},
            {"comgenre", NULL, false, 0, 0, 0, "comgenre needs a mode"},
            {"comgenre", "search", true, 0, 0, 2, NULL},
            {"sccompar", "purpose=refine;category=3", true, 0, 3, kCommentModeRefine, NULL},
            {"sccompar", "comment", false, 0, 0, 0, "sccompar takes {purpose = 'comment' or 'refine', category = 1"},
            {"sccompar", "category=4;purpose=comment", false, 0, 0, 0, "no such category of sccompar: 4 (1 to 3)"},
            {"sccomite", "comment", true, 0, 4, kCommentModeComment, NULL},
            {"sccomls", "refine", true, 0, 5, kCommentModeRefine, NULL},
            {"iteminfo", "bag=inventory;slot=5", true, 0, 5, 0, NULL},
            {"iteminfo", "bag=17;slot=80", true, 17, 80, 0, NULL},
            {"iteminfo", "bag=safe;slot=0", false, 0, 0, 0, "no such slot for iteminfo: 0 (1 to 80"},
            {"iteminfo", "bag=wallet;slot=1", false, 0, 0, 0, "no such bag for iteminfo: wallet"},
            {"itemxinf", "item=16555", true, -1, 0, 16555, NULL},
            {"iteminfo", "item=65535", false, 0, 0, 0, "no such item id for iteminfo: 65535 (1 to 65534)"},
            {"iteminfo", "slot=5", false, 0, 0, 0, "iteminfo takes an item as {bag = 'inventory', slot = 5}"},
            {"casttime", "3", true, kCastKindBar, 0, 180, NULL},
            {"casttime", "seconds=3;style=countdown", true, kCastKindCountdown, 0, 180, NULL},
            {"casttime", "seconds=4", true, kCastKindBar, 0, 240, NULL},
            {"casttime", "0.5", true, kCastKindBar, 0, 30, NULL},
            {"casttime", "0.016666666666666666", true, kCastKindBar, 0, 1, NULL},
            {"casttime", "1.5e1", true, kCastKindBar, 0, 900, NULL},
            {"casttime", "0.001", false, 0, 0, 0, "no such length of casttime: 0.001"},
            {"casttime", "1,5", false, 0, 0, 0, "no such length of casttime: 1,5"},
            {"casttime", "seconds=4;style=slow", false, 0, 0, 0, "casttime takes seconds, or {seconds = 3, style"},
            {"casttime", "style=bar", false, 0, 0, 0, "casttime takes seconds, or {seconds = 3, style"},
            {"casttime", "0", false, 0, 0, 0, "no such length of casttime: 0"},
            {"eventtim", "seconds=3;style=bar", false, 0, 0, 0, "eventtim takes seconds"},
            {"eventtim", "35791394", true, 0, 0, 2147483640, NULL},
            {"eventtim", "35791394.11", true, 0, 0, 2147483647, NULL},
            {"eventtim", "35791395", false, 0, 0, 0, "no such length of eventtim"},
            {"macro", "palette=alt;row=10", true, 2, 0, 9, NULL},
            {"macro", "palette=alt;row=11", false, 0, 0, 0, "no such row of macro: 11 (1 to 10)"},
            {"macro", "palette=shift;row=1", false, 0, 0, 0, "macro takes {palette = 'ctrl' or 'alt'"},
            {"coloresu", "9", true, 0, 0, 9, NULL},
            {"coloresu", "1", false, 0, 0, 0, "no such region of coloresu: 1 (2 to 9)"},
            {"storage2", NULL, true, 1, 0, 0, NULL},
            {"persona", "status", false, 0, 0, 0, "persona takes no argument"},
        };
        int good = 0;
        const int count = static_cast<int>(sizeof(cases) / sizeof(cases[0]));
        for (int i = 0; i < count; ++i) {
            const Case& k = cases[i];
            why[0] = '\0';
            const bool ok = parse_open_arg(k.text, &a) && route_args(*route_spec(k.window), a, &r, why, sizeof(why));
            const bool right = ok == k.ok && (!ok || (r.x == k.x && r.y == k.y && r.value == k.value))
                && (ok || strstr(why, k.why) != NULL);
            good += right;
            if (!right) {
                printf("  route_args %s %s: %s\n", k.window, k.text ? k.text : "(none)", why);
            }
        }
        check(good == count, "each window's argument by name or number, its default, and the refusal of a value"
            " outside the window's range, as kRoutes gives them");
        bool named = true;
        for (int i = 0; i < kRouteCount; ++i) {
            named = named && name(kRoutes[i].name) >= 0 && !open_refusal(kRoutes[i].name, 9);
        }
        check(named && route_spec("jobcsel") == NULL && route_spec("ability") == NULL,
            "every window of kRoutes is in the inventory, and no open slot refuses it outright");

        static const char* const family[] = {
            "storage", "storage2", "mogpost", "jobcselu", "friend", "titlehan", "msglist", "blklist", "mount",
            "abisortw", "subwindo", "jbpcat", "fp_cat2", "scresult", "comgenre", "sccompar", "sccomite", "sccomls",
            "scsibori", "searchjo", "iteminfo", "itemxinf", "itemyinf", "casttime", "eventtim", "persona", "level",
            "statcom2", "mp_stat", "conf11l", "quest00", "quest01", "miss00", "map0", "scanlist", "cnqframe",
            "coloresu", "colorank", "conq3pts", "imperial", "allied", "colopoin", "textcol1", "textcol2", "textcol3",
            "macro", "mcrledit", "mcrbedit", "mcr1edit", "mcr2edit", "titlewin", "equip", "inventor"};
        const int windows = static_cast<int>(sizeof(family) / sizeof(family[0]));
        for (int i = 0; i < windows; ++i) {
            if (!controller(family[i])) {
                give_controller(family[i], vt_self);
            }
        }
        auto close_all = [&]() {
            for (int i = 0; i < windows; ++i) {
                if (live(family[i])) {
                    game_closes(family[i]);
                    destroy(family[i]);
                }
            }
            drain(g_e);
            drain(g_e);
            events(&cursor, ev, 64);
            memset(&g_route, 0, sizeof(g_route));
            g_route.msglist = 1;
            g_route.job_test = 1;
            g_route.no_tracking = true;
        };
        auto opened_events = [&](char* out, size_t size, int n) {
            out[0] = '\0';
            for (int i = 0; i < n; ++i) {
                const size_t used = strlen(out);
                snprintf(out + used, size - used, "%s%s", used ? " " : "",
                    event_type(ev[i]) == kEvOpened ? g_e.inv.names[event_name(ev[i])].name : "!");
            }
            return out;
        };
        // open(window, argument) as the engine's Lua side queues it.
        auto route = [&](const char* window, const char* text) {
            OpenArg arg;
            RouteArgs args;
            char reason[kErrorReason];
            parse_open_arg(text, &arg);
            route_args(*route_spec(window), arg, &args, reason, sizeof(reason));
            Command c = cmd(kOpOpenRoute, window, args.x, args.y, s1, gen1, args.value);
            c.verb = kVerbOpen;
            run(c);
            return events(&cursor, ev, 64);
        };
        auto set_cmd = [&](const char* opens, int set, uint32_t value, int16_t y = 0, int16_t z = 0) {
            Command c = cmd(kOpOpenSet, opens, set, y, s1, gen1, value);
            c.verb = kVerbOpenSet;
            c.z = z;
            run(c);
            return events(&cursor, ev, 64);
        };
        char seen[256];
        close_all();

        int n = route("storage", NULL);
        const bool storage_away = g_route.calls == 0 && n == 1 && is_error(ev[0], s1, gen1, "open", "storage",
            "the Mog House's storage opens only in the Mog House");
        n = route("storage2", NULL);
        const bool storage2_away = g_route.calls == 0 && n == 1 && is_error(ev[0], s1, gen1, "open", "storage2",
            "the Nomad Moogle's storage opens only where a Nomad Moogle serves");
        g_mog_house_ptr = g_manager;
        n = route("storage", NULL);
        const bool storage = g_route.self == controller("storage") && g_route.args[0] == 0
            && strcmp(opened_events(seen, sizeof(seen), n), "storage") == 0;
        n = route("storage2", NULL);
        const bool house_only = n == 1 && is_error(ev[0], s1, gen1, "open", "storage2", "where a Nomad Moogle serves");
        g_mog_house_ptr = NULL;
        close_all();
        g_nomad_ptr = g_manager;
        n = route("storage2", NULL);
        g_nomad_ptr = NULL;
        check(storage_away && storage2_away && storage && house_only && g_route.self == controller("storage2")
                && g_route.args[0] == 1 && strcmp(opened_events(seen, sizeof(seen), n), "storage2") == 0,
            "open('storage') and open('storage2'): refused but in the Mog House and where a Nomad Moogle serves, the"
            " areas whose menus run them; there the storage list's opener on each one's controller with 0 and 1");
        close_all();

        n = route("mogpost", "nomad");
        const bool away = g_route.calls == 0 && n == 1 && is_error(ev[0], s1, gen1, "open", "mogpost",
            "only in the Mog House or where a Nomad Moogle serves");
        n = route("mogpost", NULL);
        const bool house_away = g_route.calls == 0 && n == 1 && is_error(ev[0], s1, gen1, "open", "mogpost",
            "the Mog House's post opens only in the Mog House");
        g_mog_house_ptr = g_manager;
        n = route("mogpost", NULL);
        g_mog_house_ptr = NULL;
        const bool house = g_route.args[0] == 0 && strcmp(opened_events(seen, sizeof(seen), n), "mogpost") == 0;
        close_all();
        g_nomad_ptr = g_manager;
        n = route("mogpost", "nomad");
        g_nomad_ptr = NULL;
        check(away && house_away && house && g_route.self == controller("mogpost") && g_route.args[0] == 1
                && strcmp(opened_events(seen, sizeof(seen), n), "mogpost") == 0,
            "open('mogpost'): 'mog_house' (0) only in the Mog House; 'nomad' (1) only in the Mog House or where a"
            " Nomad Moogle serves, as the job change's confirm asks; each the post menu's opener with its mode");
        close_all();

        n = route("jobcselu", "support");
        const bool outside = g_route.calls == 0 && n == 1 && is_error(ev[0], s1, gen1, "open", "jobcselu",
            "the job changes only in the Mog House");
        g_mog_house_ptr = g_manager;
        n = route("jobcselu", "main");
        g_mog_house_ptr = NULL;
        check(outside && strcmp(g_route.order, "language_key") == 0 && g_route.args[0] == 6
                && rd32(controller("jobcselu"), 0x14) == 1
                && strcmp(opened_events(seen, sizeof(seen), n), "jobcselu") == 0,
            "open('jobcselu', 'main'): refused outside the Mog House and a Nomad Moogle's area; inside, the row (1"
            " main) written where the confirm reads it, then the open by the key the language's lookup (6) answers");
        close_all();

        n = route("friend", NULL);
        const bool friend_alone = g_route.args[0] == 0 && strcmp(opened_events(seen, sizeof(seen), n), "friend") == 0
            && !live("titlehan");
        close_all();
        n = set_cmd("friend", kSetFriend, 0);
        check(friend_alone && strcmp(opened_events(seen, sizeof(seen), n), "friend titlehan") == 0,
            "open('friend'): the friend list's opener with 0, titlehan it opens held off; open_set('friend'): friend"
            " and titlehan");
        close_all();

        g_route.msglist = 0;
        n = route("msglist", NULL);
        const bool busy = n == 1 && is_error(ev[0], s1, gen1, "open", "msglist", "PlayOnline's mail is busy");
        g_route.msglist = 1;
        n = route("msglist", NULL);
        const bool mail = g_route.args[0] == 0 && g_route.args[1] == 0 && g_route.args[2] == 0
            && strcmp(opened_events(seen, sizeof(seen), n), "msglist") == 0 && !live("titlehan");
        close_all();
        n = set_cmd("msglist", kSetMail, 0);
        check(busy && mail && strcmp(opened_events(seen, sizeof(seen), n), "msglist titlehan") == 0,
            "open('msglist'): the mail list's opener with (0, 0, 0), its refusal an error, titlehan held off;"
            " open_set('mail'): msglist and titlehan");
        close_all();

        n = route("blklist", NULL);
        check(g_route.self == controller("blklist") && strcmp(opened_events(seen, sizeof(seen), n), "blklist") == 0,
            "open('blklist'): the blacklist's opener on its controller");
        close_all();

        n = route("mount", NULL);
        const bool mount_alone = g_route.args[0] == 1 && g_route.args[1] == 0
            && strcmp(opened_events(seen, sizeof(seen), n), "mount") == 0 && !live("subwindo");
        close_all();
        n = set_cmd("mount", kSetMount, 0);
        check(mount_alone && g_route.args[0] == 1 && g_route.args[1] == 1
                && strcmp(opened_events(seen, sizeof(seen), n), "abisortw mount subwindo") == 0,
            "open('mount'): the mount list's opener as the ability list runs it (1, 0), subwindo held off;"
            " open_set('mount') as the Abilities menu runs it (1, 1): abisortw, mount, subwindo");
        close_all();

        g_route.job_test = 0;
        n = route("jbpcat", "thf");
        const bool low = strcmp(g_route.order, "job_test") == 0 && g_route.args[0] == 6 && n == 1
            && is_error(ev[0], s1, gen1, "open", "jbpcat", "offers thf only at level 99");
        g_route.job_test = 1;
        g_route.order[0] = '\0';
        n = route("jbpcat", "run");
        check(low && strcmp(g_route.order, "job_test job_points") == 0 && g_route.args[0] == 22
                && g_route.self == controller("jbpcat") && strcmp(opened_events(seen, sizeof(seen), n), "jbpcat") == 0,
            "open('jbpcat', job): the Job Points list's test of the job first, its refusal an error; then the"
            " opener with the job");
        close_all();

        n = route("fp_cat2", "3");
        check(g_route.args[0] == 3 && strcmp(opened_events(seen, sizeof(seen), n), "fp_cat2") == 0,
            "open('fp_cat2', 3): the opener with the category");
        close_all();

        n = route("conf11l", "2");
        const bool second = rd32(controller("conf11l"), 0x14) == 1 && strcmp(opened_events(seen, sizeof(seen), n),
            "conf11l") == 0;
        close_all();
        n = route("quest00", "Unity Information");
        check(second && rd32(controller("quest00"), 0x68) == 1
                && strcmp(opened_events(seen, sizeof(seen), n), "quest00") == 0,
            "open('conf11l', 2) writes the set 1 and open('quest00', 'Unity Information') the mode 1 where the"
            " game's rows write them, then open the window");
        n = route("quest01", "2");
        const bool wrong_mode = g_route.calls == 0 && n == 1 && is_error(ev[0], s1, gen1, "open", "quest01",
            "the game refused it: quest01's list 2 opens only from quest00's mode 1");
        route("quest00", "1");
        g_route.order[0] = '\0';
        n = route("quest01", "2");
        check(wrong_mode && strcmp(g_route.order, "quest00_dim quest00_dim2 quest01") == 0
                && g_route.self == controller("quest01") && g_route.args[0] == 1
                && strcmp(opened_events(seen, sizeof(seen), n), "quest01") == 0,
            "open('quest01', 2): refused from quest00's mode 2; from its mode 1, quest00's confirm: its two calls on"
            " quest00's controller, quest01's open, then its setup with the list from 0");
        n = route("quest01", "7");
        const bool unity_wrong = n == 1 && is_error(ev[0], s1, gen1, "open", "quest01",
            "quest01's list 7 opens only from quest00's mode 3, 'Ranking Information'");
        route("quest00", "ranking information");
        n = route("quest01", "7");
        check(unity_wrong && g_route.args[0] == 6 && n == 0,
            "quest01's lists 6 and 7 open only from quest00's mode 3, its Ranking Information");
        close_all();
        n = route("quest01", NULL);
        check(g_route.calls == 0 && n == 1 && is_error(ev[0], s1, gen1, "open", "quest01", "quest01 needs quest00"
                " open"),
            "quest01 with quest00 closed: refused on the game thread too");
        close_all();

        n = route("miss00", NULL);
        const bool nation = strcmp(g_route.order, "miss00_menu") == 0 && g_route.args[0] == 0
            && rd32(controller("miss00"), kMissionLog) == kNation
            && strcmp(opened_events(seen, sizeof(seen), n), "miss00") == 0;
        close_all();
        g_route.expansion = 0;
        n = route("miss00", "Rise of the Zilart");
        const bool off = strcmp(g_route.order, "expansion") == 0 && g_route.args[0] == 1 && n == 1
            && is_error(ev[0], s1, gen1, "open", "miss00",
                "the game refused it: the Rise of the Zilart log's expansion is not on");
        g_route.expansion = 2;
        n = route("miss00", "4");
        check(nation && off && rd32(controller("miss00"), kMissionLog) == 3
                && strcmp(opened_events(seen, sizeof(seen), n), "miss00") == 0,
            "open('miss00'): the missions menu's setter with 0, the player's nation's log; a named log behind the"
            " expansion test of its bit, refused when off, else written");
        close_all();

        n = route("searchjo", NULL);
        const bool fresh = strcmp(g_route.order, "search_reset") == 0 && g_route.self == controller("scresult")
            && g_route.args[0] == 0 && strcmp(opened_events(seen, sizeof(seen), n), "searchjo") == 0;
        close_all();
        wr32(controller("scresult"), 0x58, 0);
        n = route("searchjo", "refine");
        const bool none = g_route.calls == 0 && n == 1 && is_error(ev[0], s1, gen1, "open", "searchjo",
            "no search has run to refine");
        wr32(controller("scresult"), 0x58, 2);
        n = route("searchjo", "refine");
        check(fresh && none && g_route.calls == 0 && strcmp(opened_events(seen, sizeof(seen), n), "searchjo") == 0,
            "open('searchjo'): the search reset on scresult's controller with 0, as the Search row runs it, then the"
            " picker; 'refine' refused unless the results' mode is 1 or 2, a search run");
        close_all();
        n = route("comgenre", "search");
        const bool search = strcmp(g_route.order, "search_reset comgenre") == 0 && g_route.args[0] == 1
            && strcmp(opened_events(seen, sizeof(seen), n), "comgenre") == 0;
        close_all();
        wr32(controller("scresult"), 0x58, 0);
        n = route("comgenre", "refine");
        const bool refine_none = g_route.calls == 0 && n == 1 && is_error(ev[0], s1, gen1, "open", "comgenre",
            "no search has run");
        n = route("comgenre", "comment");
        check(search && refine_none && strcmp(g_route.order, "comgenre") == 0 && g_route.args[0] == 0
                && strcmp(opened_events(seen, sizeof(seen), n), "comgenre") == 0,
            "open('comgenre', mode): search after the search reset, refine behind the results' test, comment"
            " alone, each the opener with its mode");
        n = route("sccompar", "purpose=comment;category=3");
        const bool sub = g_route.self == controller("sccompar") && g_route.args[0] == 0 && g_route.args[1] == 0x30
            && strcmp(opened_events(seen, sizeof(seen), n), "sccompar") == 0;
        n = route("sccomls", "comment");
        check(sub && g_route.self == controller("sccomls") && g_route.args[0] == 0 && g_route.args[1] == 0x50
                && strcmp(opened_events(seen, sizeof(seen), n), "sccomls") == 0,
            "open('sccompar', {purpose = 'comment', category = 3}): its opener with mode 0 and the category's base"
            " 0x30; sccomls's with its fixed category 5's base 0x50");
        close_all();
        n = route("scsibori", NULL);
        const bool no_parent = n == 1 && is_error(ev[0], s1, gen1, "open", "scsibori", "scsibori needs scresult open");
        open_key("scresult");
        events(&cursor, ev, 64);
        wr32(controller("scresult"), 0x58, 0);
        n = route("scsibori", NULL);
        const bool unrun = n == 1 && is_error(ev[0], s1, gen1, "open", "scsibori", "no search has run");
        wr32(controller("scresult"), 0x58, 1);
        n = route("scsibori", NULL);
        check(no_parent && unrun && strcmp(opened_events(seen, sizeof(seen), n), "scsibori") == 0,
            "open('scsibori'): with scresult open and a search run, as the results' narrow row asks; else refused");
        close_all();

        // the item panel: 5 a crystal in the inventory, 6 empty
        uint8_t* record = g_pd + kPdInventory + 5 * kItemRecordBytes;
        uint8_t kept[2 * kItemRecordBytes];
        memcpy(kept, record, sizeof(kept));
        memset(record, 0, sizeof(kept));
        wr16(record, 0, 4096);
        record[2] = 5;
        memset(g_route.bag_size, 81, sizeof(g_route.bag_size));
        g_route.bag_size[0] = 5;
        n = route("iteminfo", "bag=inventory;slot=5");
        const bool small = g_route.calls == 0 && n == 1 && is_error(ev[0], s1, gen1, "open", "iteminfo",
            "inventory has no slot 5: it has slots 1 to 4 now");
        g_route.bag_size[0] = 0xFF;
        n = route("iteminfo", "bag=inventory;slot=5");
        const bool no_player = n == 1 && is_error(ev[0], s1, gen1, "open", "iteminfo", "inventory has no slots now");
        g_route.bag_size[0] = 81;
        n = route("iteminfo", "bag=inventory;slot=5");
        const bool slot = small && no_player && g_route.self == controller("iteminfo") && g_route.args[0] == 0
            && g_route.args[1] == 4096
            && g_route.args[2] == 5 && g_route.args[3] == 0
            && g_route.args[4] == static_cast<int32_t>(reinterpret_cast<uintptr_t>(record + 0x10))
            && g_route.args[5] == 0 && g_route.args[6] == 0
            && strcmp(opened_events(seen, sizeof(seen), n), "iteminfo") == 0;
        n = route("iteminfo", "bag=inventory;slot=6");
        const bool empty = n == 1 && is_error(ev[0], s1, gen1, "open", "iteminfo", "inventory has no item in slot 6");
        n = route("iteminfo", "item=5021");
        const bool typed = n == 1 && is_error(ev[0], s1, gen1, "open", "iteminfo", "give its bag and slot");
        n = route("iteminfo", "item=9999");
        const bool unknown = n == 1 && is_error(ev[0], s1, gen1, "open", "iteminfo", "the game has no data for item"
            " 9999");
        n = route("iteminfo", "item=16555");
        check(slot && empty && typed && unknown && g_route.args[1] == 16555 && g_route.args[2] == 0
                && g_route.args[3] == 0 && g_route.args[4] == 0 && n == 0,
            "open('iteminfo', {bag, slot}): a slot at or past the bag's size, which counts slot 0, refused as the"
            " game's lists stop below it; the item panel's set-and-open with mode 0, the record's id, index, bag"
            " and extra data; an empty slot refused; by id alone the panel with no extra data, refused for an item"
            " of types 20 to 23, whose extra data the panel reads, and for one the game has no data for");
        n = route("itemxinf", "bag=inventory;slot=5");
        const bool x = g_route.self == controller("itemxinf") && g_route.args[0] == 5
            && g_route.args[5] == static_cast<int32_t>(reinterpret_cast<uintptr_t>(controller("equip")))
            && g_route.args[6] == static_cast<int32_t>(reinterpret_cast<uintptr_t>(controller("inventor")))
            && strcmp(opened_events(seen, sizeof(seen), n), "itemxinf") == 0 && !live("itemyinf");
        game_closes("itemxinf");
        destroy("itemxinf");
        drain(g_e);
        events(&cursor, ev, 64);
        n = set_cmd("itemxinf", kSetCompare, 0, 5, 0);
        check(x && g_route.args[0] == 5 && g_route.args[1] == 4096
                && strcmp(opened_events(seen, sizeof(seen), n), "itemxinf itemyinf") == 0,
            "open('itemxinf', item): the set-and-open with mode 5 beside equip's and inventor's controllers, as the"
            " inventory's cursor runs it, itemyinf held off; open_set('compare', item): itemxinf and itemyinf");
        memcpy(record, kept, sizeof(kept));
        close_all();

        n = route("casttime", "seconds=3;style=countdown");
        const bool cast = g_route.args[0] == kCastKindCountdown && g_route.args[1] == 180
            && strcmp(opened_events(seen, sizeof(seen), n), "casttime") == 0;
        n = route("eventtim", "30");
        check(cast && g_route.args[0] == 0 && g_route.args[1] == 1800
                && strcmp(opened_events(seen, sizeof(seen), n), "eventtim") == 0,
            "the timers' openers with the kind and the length in 1/60 seconds");
        close_all();

        n = route("persona", NULL);
        const bool layout = controller("persona")[0x1C] == 2 && strcmp(opened_events(seen, sizeof(seen), n),
            "persona") == 0;
        close_all();
        n = set_cmd("persona", kSetStatus, 0);
        const bool status = controller("persona")[0x1C] == 1 && strcmp(opened_events(seen, sizeof(seen), n),
            "persona") == 0;
        close_all();
        g_monstrosity = 1;
        n = set_cmd("persona", kSetStatus, 0);
        g_monstrosity = 0;
        check(layout && status && g_route.self == controller("mp_stat") && g_route.args[0] == 0
                && strcmp(opened_events(seen, sizeof(seen), n), "mp_stat level statcom2") == 0,
            "open('persona'): the equipment layout (2) written, then persona; open_set('status'): the Status row,"
            " the layout 1 and persona, or in Monstrosity mp_stat's opener with 0: mp_stat, level, statcom2");
        close_all();

        g_macro_editor[0x0C] = 1;
        g_route.order[0] = '\0';
        n = route("macro", "palette=alt;row=4");
        const bool switched = strcmp(g_route.order, "macro_switch macro_pick") == 0 && g_route.self == g_macro_editor
            && g_route.args[0] == 3 && g_macro_editor[0x0C] == 2 && g_macro_editor[0x0E] == 3
            && strcmp(opened_events(seen, sizeof(seen), n), "macro") == 0 && !live("mcrledit") && !live("mcr2edit");
        close_all();
        n = route("macro", "palette=alt;row=2");
        check(switched && strcmp(g_route.order, "macro_pick") == 0 && g_macro_editor[0x0C] == 2
                && g_macro_editor[0x0E] == 1 && strcmp(opened_events(seen, sizeof(seen), n), "macro") == 0,
            "open('macro', {palette = 'alt', row = 4}) with Ctrl in use: the palette switch, the game's one writer"
            " of the palette, its mcr2edit held off, then the editor's pick with the row from 0, mcrledit held off;"
            " with Alt in use already, the pick alone");
        g_macro_editor[0x0C] = 1;
        close_all();
        uint8_t*& macro_slot = g_ctl_global[g_e.inv.names[name("macro")].rows[0]];
        uint8_t* const macro_ctl = macro_slot;
        macro_slot = NULL;
        n = route("macro", "palette=alt;row=2");
        macro_slot = macro_ctl;
        check(n == 1 && is_error(ev[0], s1, gen1, "open", "macro",
                  "macro has no controller now, and the game's open would crash the client")
                && g_route.order[0] == '\0' && !live("macro"),
            "open('macro', ...) with macro's slot holding no controller: refused on the game thread as on the Lua"
            " thread, before the editor's pick, whose open of macro would bind to that slot");
        close_all();

        n = route("scanlist", NULL);
        const bool unmapped = n == 1 && is_error(ev[0], s1, gen1, "open", "scanlist", "scanlist needs map0 open");
        open_key("map0");
        events(&cursor, ev, 64);
        g_route.no_tracking = false;
        g_route.tracking = 1;
        const int asks = g_dead.ask_calls;
        n = route("scanlist", NULL);
        uint8_t* yes_no = controller("comyn");
        const bool asked = strcmp(g_route.order, "tracking") == 0 && g_dead.ask_calls == asks + 1
            && g_dead.ask_ctl == yes_no && g_dead.ask_callback == reinterpret_cast<const void*>(&fake_scan_question)
            && g_dead.message_args[0] == 0x0E && g_dead.message_args[1] == 0x45 && g_dead.ask_args[0] == 0
            && g_dead.ask_args[1] == 0 && g_dead.ask_args[2] == 0 && g_dead.ask_args[3] == 1 && g_dead.ask_args[4] == 0
            && strcmp(opened_events(seen, sizeof(seen), n), "comyn") == 0 && !live("scanlist");
        n = route("scanlist", NULL);
        const bool refused_ask = g_dead.ask_calls == asks + 2 && n == 1
            && is_error(ev[0], s1, gen1, "open", "scanlist", "the game refused comyn's open; nothing was asked")
            && !live("scanlist");
        game_closes("comyn");
        destroy("comyn");
        wr32(yes_no, kYesNoCallback, 0);
        drain(g_e);
        events(&cursor, ev, 64);
        g_route.tracking = 0;
        g_route.order[0] = '\0';
        n = route("scanlist", NULL);
        check(unmapped && asked && refused_ask && strcmp(g_route.order, "tracking scanlist") == 0
                && strcmp(opened_events(seen, sizeof(seen), n), "scanlist") == 0,
            "open('scanlist'): with map0 open and the map not tracking, scanlist's opener, as map0's Wide Scan row"
            " runs it; while it tracks, the row's own question on comyn whether to stop (message 0x0E/0x45, its"
            " callback, no first, cancellable), and no scanlist; an ask the game refuses, comyn's question still"
            " pending, an error event");
        close_all();

        wr32(g_route_player, kRouteArea, 230);
        n = set_cmd("cnqframe", kSetRegion, 4);
        const bool region = g_route.self == controller("cnqframe") && g_route.args[0] == 3 && g_route.args[1] == 230
            && strcmp(opened_events(seen, sizeof(seen), n), "cnqframe colopoin") == 0;
        n = set_cmd("cnqframe", kSetRegion, 1);
        const bool open_already = n == 1 && is_error(ev[0], s1, gen1, "open_set", "region", "cnqframe is open");
        open_key("colorank");
        events(&cursor, ev, 64);
        n = route("coloresu", "5");
        check(region && open_already && rd32(controller("coloresu"), 0x18) == 5 && !live("colorank")
                && strcmp(opened_events(seen, sizeof(seen), n), "coloresu !") == 0,
            "open_set('region', 'colonization'): the Region Info opener with the kind 3 and the player's area:"
            " cnqframe and colopoin; refused while cnqframe is open; open('coloresu', 5) on the Colonization map:"
            " the region written, coloresu, colorank closed");
        close_all();
        open_key("cnqframe");
        events(&cursor, ev, 64);
        wr32(controller("cnqframe"), kCnqKindAt, 0);
        n = route("coloresu", "5");
        check(n == 1 && is_error(ev[0], s1, gen1, "open", "coloresu", "on its Colonization map"),
            "coloresu with cnqframe on another map: refused");
        close_all();

        n = set_cmd("textcol1", kSetTextColors, 3);
        check(rd32(controller("textcol1"), 0x14) == 2 && rd32(controller("textcol2"), 0x1C) == 2
                && rd32(controller("textcol3"), 0x18) == 2
                && strcmp(opened_events(seen, sizeof(seen), n), "textcol2 textcol1") == 0,
            "open_set('text_colors', 3): the category 2 written into textcol1, textcol2 and textcol3 as the text"
            " color menu's row does, then textcol2 and textcol1");
        close_all();

        hooked_open(g_mcb, NULL, "menu    mcr1edlo", 1, 0);
        drain(g_e);
        n = events(&cursor, ev, 64);
        const bool alias_opened = strcmp(opened_events(seen, sizeof(seen), n), "mcr1edit") == 0;
        fake_close(g_mcb, NULL, "menu    mcr1edlo");
        destroy("mcr1edit");
        drain(g_e);
        drain(g_e);
        n = events(&cursor, ev, 64);
        check(alias_opened && n == 1 && is_event(ev[0], kEvClosed, "mcr1edit"),
            "the game's open and close by mcr1edit's other layout's key, mcr1edlo, post opened{mcr1edit} and"
            " closed{mcr1edit}: one window under one name");
        open_key("mcrbedit");
        open_key("mcr2edit");
        open_key("mcr1edit");
        events(&cursor, ev, 64);
        run(cmd(kOpClose, "mcr1edit", 0, 0, s1, gen1));
        n = events(&cursor, ev, 64);
        check(strcmp(g_route.order, "macro_close title") == 0 && g_route.self == controller("titlewin")
                && g_route.args[0] == 0 && g_route.args[1] == 2 && !live("mcrbedit") && !live("mcr1edit")
                && !live("mcr2edit"),
            "close('mcr1edit'): what Escape on a palette does: the macro editor's close of the three palette"
            " windows, then titlewin's step back (0, 2)");
        close_all();

        // the Status screen: statcom2's close by instance, its begin-close
        // closing persona and mp_stat
        memcpy(vt_status, vt_self, sizeof(vt_status));
        vt_status[2] = reinterpret_cast<const void*>(&fake_status_begin_close);
        const void* status_vt = vt_status;
        memcpy(controller("statcom2"), &status_vt, 4);
        open_key("persona");
        open_key("level");
        open_key("statcom2");
        events(&cursor, ev, 64);
        g_route.order[0] = '\0';
        run(cmd(kOpClose, "persona", 0, 0, s1, gen1));
        n = events(&cursor, ev, 64);
        const bool status_closed = strcmp(g_route.order, "status_close") == 0 && !live("statcom2") && !live("persona");
        close_all();
        open_key("persona");
        events(&cursor, ev, 64);
        g_route.order[0] = '\0';
        run(cmd(kOpClose, "persona", 0, 0, s1, gen1));
        n = events(&cursor, ev, 64);
        check(status_closed && g_route.order[0] == '\0' && !live("persona") && n == 1
                && is_event(ev[0], kEvClosed, "persona"),
            "close('persona') with statcom2 up: what the Status screen's Escape does, statcom2 closed by instance,"
            " whose begin-close closes persona and mp_stat; with no Status screen, persona alone");
        close_all();
        open_key("textcol2");
        open_key("textcol1");
        events(&cursor, ev, 64);
        run(cmd(kOpClose, "textcol1", 0, 0, s1, gen1));
        n = events(&cursor, ev, 64);
        check(!live("textcol1") && !live("textcol2") && n == 2 && is_event(ev[0], kEvClosed, "textcol2")
                && is_event(ev[1], kEvClosed, "textcol1"),
            "close('textcol1'): what its Escape does, textcol2 closed, then textcol1");
        close_all();

        // a parent's close leaves no child open with no parent of it open
        static const char* const extra[] = {"mp_mncst", "mp_cstm", "mapv3", "mapv2", "link5", "link1"};
        for (size_t i = 0; i < sizeof(extra) / sizeof(extra[0]); ++i) {
            if (!controller(extra[i])) {
                give_controller(extra[i], vt_self);
            }
        }
        auto closed_order = [&](char* out, size_t size, int count) {
            out[0] = '\0';
            for (int i = 0; i < count; ++i) {
                const size_t used = strlen(out);
                snprintf(out + used, size - used, "%s%s", used ? " " : "",
                    event_type(ev[i]) == kEvClosed ? g_e.inv.names[event_name(ev[i])].name : "!");
            }
            return out;
        };
        open_key("quest00");
        open_key("quest01");
        events(&cursor, ev, 64);
        run(cmd(kOpClose, "quest00", 0, 0, s1, gen1));
        n = events(&cursor, ev, 64);
        check(!live("quest00") && !live("quest01") && strcmp(closed_order(seen, sizeof(seen), n), "quest01 quest00") == 0,
            "close('quest00') with quest01 up: quest01 closed first through its own close, with its event, then"
            " quest00, whose close leaves it");
        open_key("comgenre");
        open_key("sccompar");
        open_key("iteminfo");
        open_key("itemxinf");
        events(&cursor, ev, 64);
        run(cmd(kOpClose, "comgenre", 0, 0, s1, gen1));
        run(cmd(kOpClose, "iteminfo", 0, 0, s1, gen1));
        n = events(&cursor, ev, 64);
        const bool orphans = strcmp(closed_order(seen, sizeof(seen), n), "sccompar comgenre itemxinf iteminfo") == 0;
        open_key("scresult");
        open_key("comgenre");
        open_key("sccompar");
        events(&cursor, ev, 64);
        run(cmd(kOpClose, "comgenre", 0, 0, s1, gen1));
        n = events(&cursor, ev, 64);
        check(orphans && live("sccompar") && n == 1 && is_event(ev[0], kEvClosed, "comgenre"),
            "comgenre's and iteminfo's children closed before them; sccompar left open while scresult, its other"
            " parent, is");
        close_all();
        // left to the game where its close closes the child, and after it
        // when it does not
        memcpy(vt_link5, vt_self, sizeof(vt_link5));
        vt_link5[2] = reinterpret_cast<const void*>(&fake_link5_begin_close);
        const void* link5_vt = vt_link5;
        memcpy(controller("link5"), &link5_vt, 4);
        g_route.order[0] = '\0';
        open_key("link5");
        open_key("link1");
        events(&cursor, ev, 64);
        run(cmd(kOpClose, "link5", 0, 0, s1, gen1));
        n = events(&cursor, ev, 64);
        const bool own_close = strcmp(g_route.order, "link5_close") == 0 && !live("link5") && !live("link1")
            && strcmp(closed_order(seen, sizeof(seen), n), "link1 link5") == 0;
        const void* plain_vt = vt_self;
        memcpy(controller("link5"), &plain_vt, 4);
        open_key("link5");
        open_key("link1");
        events(&cursor, ev, 64);
        run(cmd(kOpClose, "link5", 0, 0, s1, gen1));
        n = events(&cursor, ev, 64);
        check(own_close && !live("link1") && strcmp(closed_order(seen, sizeof(seen), n), "link5 link1") == 0,
            "link5, whose begin-close closes link1, is closed with link1 left to its close; when its close leaves"
            " link1, link1 is closed after it");
        open_key("map0");
        open_key("mapv3");
        open_key("mapv2");
        open_key("scanlist");
        events(&cursor, ev, 64);
        uint8_t* const map_frame = g_ctl_global[first_row("mapframe")];
        g_ctl_global[first_row("mapframe")] = NULL;
        run(cmd(kOpBlockClose, "map0", 0, 0, s1, gen1));
        g_ctl_global[first_row("mapframe")] = map_frame;
        n = events(&cursor, ev, 64);
        check(!live("map0") && !live("mapv3") && !live("mapv2") && !live("scanlist")
                && strcmp(closed_order(seen, sizeof(seen), n), "map0 mapv2 mapv3 scanlist") == 0,
            "a block's close of map0 with no frame to run the map's close: map0, then each child it left, mapv3's"
            " own child mapv2 before mapv3");
        close_all();
        // mp_mncst's Escape, and cnqframe's
        open_key("mp_stat");
        open_key("mp_cstm");
        open_key("mp_mncst");
        events(&cursor, ev, 64);
        g_route.order[0] = '\0';
        run(cmd(kOpClose, "mp_mncst", 0, 0, s1, gen1));
        n = events(&cursor, ev, 64);
        const bool instincts = strcmp(g_route.order, "instincts_escape") == 0 && !live("mp_stat") && !live("mp_cstm")
            && !live("mp_mncst") && strcmp(closed_order(seen, sizeof(seen), n), "mp_stat mp_cstm mp_mncst") == 0;
        memcpy(vt_region, vt_self, sizeof(vt_region));
        vt_region[kRegionFadeSlot / 4] = reinterpret_cast<const void*>(&fake_region_fade);
        const void* region_vt = vt_region;
        uint8_t* frame = controller("cnqframe");
        memcpy(frame, &region_vt, 4);
        wr16(frame, kRegionClosingWord, 0);
        open_key("cnqframe");
        open_key("colopoin");
        open_key("coloresu");
        events(&cursor, ev, 64);
        g_route.order[0] = '\0';
        run(cmd(kOpClose, "cnqframe", 0, 0, s1, gen1));
        n = events(&cursor, ev, 64);
        const bool fading = strcmp(g_route.order, "region_fade") == 0 && g_route.self == frame && live("cnqframe")
            && !live("coloresu") && !live("colopoin")
            && strcmp(closed_order(seen, sizeof(seen), n), "coloresu colopoin") == 0;
        run(cmd(kOpClose, "cnqframe", 0, 0, s1, gen1));
        n = events(&cursor, ev, 64);
        const bool faded_close = !live("cnqframe") && n == 1 && is_event(ev[0], kEvClosed, "cnqframe");
        wr16(frame, kRegionClosingWord, 0);
        open_key("cnqframe");
        open_key("coloresu");
        events(&cursor, ev, 64);
        g_route.order[0] = '\0';
        run(cmd(kOpBlockClose, "cnqframe", 0, 0, s1, gen1));
        n = events(&cursor, ev, 64);
        check(instincts && fading && faded_close && g_route.order[0] == '\0' && !live("cnqframe")
                && strcmp(closed_order(seen, sizeof(seen), n), "coloresu cnqframe") == 0,
            "close('mp_mncst'): its Escape, the game's routine; close('cnqframe'): its Escape, the fade's start and"
            " its windows closed by name in its order, the frame left to fade; a close while it fades, and a"
            " block's close, close the frame at once");
        memcpy(frame, &plain_vt, 4);
        close_all();
        bool pairs = true;
        for (int i = 0; i < kParentClosesCount; ++i) {
            for (int k = 0; kParentCloses[i].children[k]; ++k) {
                const ChildSpec* c = child_spec(kParentCloses[i].children[k]);
                pairs = pairs && c && child_of(*c, kParentCloses[i].parent);
            }
        }
        check(pairs, "every pair kParentCloses gives is a child of kChildren with that parent");

        // the events: an open of a window reported open, the same instance,
        // posts nothing, as cnqframe's points windows re-open every frame;
        // a close of one reported closed posts nothing
        open_key("conq3pts");
        n = events(&cursor, ev, 64);
        const bool first = n == 1 && is_event(ev[0], kEvOpened, "conq3pts");
        for (int i = 0; i < 5; ++i) {
            open_key("conq3pts");
        }
        const bool quiet = events(&cursor, ev, 64) == 0;
        game_closes("conq3pts");
        drain(g_e);
        n = events(&cursor, ev, 64);
        const bool once = n == 1 && is_event(ev[0], kEvClosed, "conq3pts");
        game_closes("conq3pts");
        destroy("conq3pts");
        drain(g_e);
        const bool no_second = events(&cursor, ev, 64) == 0;
        open_key("conq3pts");
        n = events(&cursor, ev, 64);
        check(first && quiet && once && no_second && n == 1 && is_event(ev[0], kEvOpened, "conq3pts"),
            "opened once for a window opened every frame, closed once for a window closed twice, and opened again"
            " once it has been reported closed");
        // freed with no staged close for the hooks to see: the drain posts
        // its closed; freed and opened again before a drain, the open posts
        // the closed first, then its opened
        destroy("conq3pts");
        drain(g_e);
        n = events(&cursor, ev, 64);
        const bool unseen = n == 1 && is_event(ev[0], kEvClosed, "conq3pts");
        open_key("conq3pts");
        const bool back = events(&cursor, ev, 64) == 1 && is_event(ev[0], kEvOpened, "conq3pts");
        destroy("conq3pts");
        open_key("conq3pts");
        n = events(&cursor, ev, 64);
        check(unseen && back && n == 2 && is_event(ev[0], kEvClosed, "conq3pts")
                && is_event(ev[1], kEvOpened, "conq3pts"),
            "a window gone with no staged close: closed at the next drain, or at its next open before one, an open"
            " then posted as one");
        close_all();
    }

    // the show path's gate: uncovered only for a dormant menu, not closing,
    // with its registry entry
    {
        uint8_t* eq = open_now("equip");
        events(&cursor, ev, 64);
        uint32_t args[1] = {reinterpret_cast<uint32_t>(eq)};
        HuFrame f;
        memset(&f, 0, sizeof(f));
        f.args = args;
        f.user = &g_e;
        wr32(eq, kMenuState, kStateDormant);
        eq[kMenuClosing] = 1;
        hook_show_pre(&f);
        const bool closing = events(&cursor, ev, 64) == 0;
        eq[kMenuClosing] = 0;
        uint8_t* res = rdptr(eq, kMenuRes);
        memset(eq + kMenuRes, 0, 4);
        hook_show_pre(&f);
        const bool unbound = events(&cursor, ev, 64) == 0;
        memcpy(eq + kMenuRes, &res, 4);
        wr32(eq, kMenuState, 0);
        hook_show_pre(&f);
        const bool up = events(&cursor, ev, 64) == 0;
        wr32(eq, kMenuState, kStateDormant);
        hook_show_pre(&f);
        wr32(eq, kMenuState, 0);
        const int n = events(&cursor, ev, 64);
        check(closing && unbound && up && n == 1 && is_event(ev[0], kEvUncovered, "equip"),
            "the show path posts uncovered only where the routine shows: a dormant menu, not closing, with its"
            " registry entry; nothing for a closing one, one with no entry or one already up");
        run(cmd(kOpClose, "equip", 0, 0, s1, gen1));
        events(&cursor, ev, 64);
    }

    // the prompts: one list, the yes/no windows counted from engine abi 8
    {
        check(prompt_window("query", 3) && prompt_window("arealist", 4) && !prompt_window("query", 2)
                && !prompt_window("comyn", 7) && prompt_window("comyn", 8) && prompt_window("resynfr", 8)
                && !prompt_window("buff", 8) && !prompt_window("partywin", 8) && yesno_window("shopsell")
                && !yesno_window("query") && prompt_window("dead", 8) && !prompt_window("dead", 7)
                && !yesno_window("dead") && !prompt_cancels("dead") && prompt_cancels("comyn")
                && prompt_cancels("query") && prompt_window("mp_dead", 8) && !prompt_window("gmtell", 7)
                && prompt_window("gmtell", 8) && prompt_cancels("gmtell") && !prompt_cancels("mp_dead")
                && !prompt_cancels("confyn") && !prompt_cancels("ok") && prompt_cancels("sortyn")
                && prompt_cancels("bankmenu") && prompt_cancels("maplist") && prompt_cancels("evitem01")
                && boolean_window("comyn") && boolean_window("sortyn") && boolean_window("evitem01")
                && boolean_window("confyn") && !boolean_window("bankmenu") && !boolean_window("ok")
                && !yesno_window("sortyn"),
            "prompts: the first eight from engine abi 3's verbs, the yes/no windows, dead, the number boxes and the"
            " Asked windows from abi 8's; dead, confyn, mp_dead and ok the ones cancel() does not end; the yes/no"
            " class, sortyn, evitem01 and confyn answered true or false; partywin is none");
        int prompts = 0;
        int first_row = 0;
        for (int n = 0; n < g_e.inv.count; ++n) {
            prompts += g_e.inv.prompt[n];
            first_row += g_e.inv.first_row[n];
        }
        bool asked = true;
        for (int a = 0; a < kAskedCount; ++a) {
            const int n = g_e.inv.asked_window[a];
            asked = asked && n == name(kAskedNames[a]) && g_e.inv.prompt[n] && g_e.inv.asked_of(n) == a
                && asked_named(kAskedNames[a]) == a;
        }
        bool talks = true;
        for (int d = 0; d < kDialogueCount; ++d) {
            const int n = g_e.inv.dialogue_window[d];
            talks = talks && n == name(kDialogueNames[d]) && g_e.inv.prompt[n] && !g_e.inv.first_row[n]
                && g_e.inv.dialogue_of(n) == d && dialogue_named(kDialogueNames[d]) == d && g_e.inv.asked_of(n) < 0;
        }
        check(talks && prompt_window("rem4line", 8) && !prompt_window("rem4li2", 7) && prompt_cancels("rem4li2")
                && !boolean_window("rem4line") && !yesno_window("rem4line") && prompt_window("mogdoor", 8)
                && prompt_cancels("mogdoor") && !boolean_window("mogdoor") && dialogue_named("mogdoor") < 0
                && g_e.inv.dialogue_of(name("query")) < 0,
            "the dialogue boxes are prompts from engine abi 8, cancel() ends them, by index both ways; mogdoor a"
            " prompt cancel() ends");
        check(g_e.inv.dialogue_twin(name("rem4line")) == name("rem4li2")
                && g_e.inv.dialogue_twin(name("rem4li2")) == name("rem4line")
                && g_e.inv.dialogue_twin(name("query")) < 0 && g_e.inv.dialogue_twin(-1) < 0 && hide_only("rem4line")
                && hide_only("rem4li2"),
            "the dialogue boxes are each other's twin, and no other name has one; both hide-only, so a block of"
            " the pair is refused alike through either name");
        bool typed = true;
        for (int t = 0; t < kTextCount; ++t) {
            const int n = g_e.inv.text_window[t];
            typed = typed && n == name(kTextNames[t]) && g_e.inv.prompt[n] && !g_e.inv.first_row[n]
                && g_e.inv.text_of(n) == t && text_named(kTextNames[t]) == t && g_e.inv.asked_of(n) < 0
                && prompt_window(kTextNames[t], 8) && !prompt_window(kTextNames[t], 7)
                && prompt_cancels(kTextNames[t]) == (t != kTextComment) && !boolean_window(kTextNames[t]);
        }
        check(typed && text_named("passinpu") < 0 && g_e.inv.text_of(name("passinpu")) < 0,
            "the text-entry windows are prompts from engine abi 8, cancel() ending all but comment, by index both"
            " ways; passinpu is none of them");
        check(prompts == 37 && first_row == 11 && g_e.inv.prompt[name("comyn")] && g_e.inv.first_row[name("resynde")]
                && !g_e.inv.first_row[name("query")] && g_e.inv.prompt[name("dead")]
                && !g_e.inv.first_row[name("dead")] && g_e.inv.prompt[name("moneyctr")]
                && g_e.inv.prompt[name("itemctrl")] && !g_e.inv.first_row[name("itemctrl")]
                && g_e.inv.first_row[name("sortyn")] && g_e.inv.first_row[name("confyn")] && asked
                && g_e.inv.asked_of(name("comyn")) < 0 && asked_named("dead") < 0,
            "the inventory flags the thirty-seven prompts by index, eleven of them answered true or false with their"
            " default the row they open on, and the nine Asked windows by index both ways, for the hooks");
    }

    // the yes/no class's question, the game's own text decoded as its
    // converter reads it
    {
        GlyphText t;
        const uint8_t logout[] = {0x81, 0x99, 'L', 'o', 'g', ' ', 'o', 'u', 't', ' ', 0x1E, 0x02, 'n', 'o', 'w', 0x1E,
                                  0x01, '?', 0x7F, 0x31, 0};
        decode_text(logout, sizeof(logout), &t);
        check(strcmp(t.text, "\x81\x99Log out now?") == 0 && t.undecoded == 0 && strcmp(runs_of(t),
                "\x81\x99Log out /1E:1|now/1E:2|?/1E:1") == 0,
            "decode_text: a two-byte character kept as its bytes, a green run, the 7F 31 escape drawn as nothing");
        const uint8_t escapes[] = {'A', 0x7F, 0x34, 0x01, 'B', 0x7F, 0x38, 0x01, 0x02, 'C', 0x7F, 0x05, 'D', 0x7F,
                                   0xFB, 'E', 0x7F, 0x33, 'F', 0};
        decode_text(escapes, sizeof(escapes), &t);
        check(strcmp(t.text, "ABCDEF") == 0,
            "decode_text: 7F 34 takes three bytes, 7F 38 four, 7F FB and 7F 33 two, an unknown 7F only itself");
        const uint8_t others[] = {0x1F, 0x05, 'r', 0x1E, 0x01, 'a', 0x07, 'b', 0xA1, 'c', 0x80, 'd', 0xFA, 0x40, 0};
        decode_text(others, sizeof(others), &t);
        check(strcmp(t.text, "rabcd\xFA\x40") == 0 && strcmp(runs_of(t), "r/1F:5|abcd\xFA\x40/1E:1") == 0,
            "decode_text: 1F nn the second color table; a control byte and a byte that leads nothing drawn as nothing;"
            " FA leads a two-byte character");
        const uint8_t cut[] = {'a', 'b', 0x81, 0};
        decode_text(cut, sizeof(cut), &t);
        const bool lead_cut = strcmp(t.text, "ab") == 0;
        const uint8_t color_cut[] = {'x', 0x1E, 0};
        decode_text(color_cut, sizeof(color_cut), &t);
        const bool escape_cut = strcmp(t.text, "x") == 0;
        const uint8_t space[] = {'z', 0x7F, 0};
        decode_text(space, sizeof(space), &t);
        check(lead_cut && escape_cut && strcmp(t.text, "z ") == 0,
            "decode_text: a lead or a color the terminator cuts short ends the text; 7F 00 is the space the converter"
            " makes of it");
        uint8_t many[300];
        memset(many, 'a', sizeof(many) - 1);
        many[sizeof(many) - 1] = 0;
        decode_text(many, sizeof(many), &t);
        const bool capped = strlen(t.text) == static_cast<size_t>(kGlyphMax);
        decode_text(many, 3, &t);
        check(capped && strcmp(t.text, "aaa") == 0, "decode_text: at most kGlyphMax codes, and at most `bytes` bytes");
    }

    // the yes/no class's replies: what its own confirm and cancel do once
    // the window has the event, held confirms and a question asked again
    // from inside the callback among them
    {
        const int y = name("comyn");
        uint8_t* ctl = controller("comyn");
        char why[kErrorReason];
        check(!yesno_asks(g_e, y, NULL, why, sizeof(why)) && strcmp(why, "no question is pending in comyn") == 0
                && !yesno_asks(g_e, y, ctl, why, sizeof(why)) && strcmp(why, "no question is pending in comyn") == 0,
            "yesno_asks: no controller, or no callback set, asks nothing");
        events(&cursor, ev, 64);
        clear_log();
        check(yesno_ask("comyn", "Log out?", false, true, 0), "comyn asks, its default no");
        uint8_t* menu = live("comyn");
        const LONG asked = g_e.instance[y];
        check(menu && rd16(menu, kMenuCursor) == kRowNo && g_log.select_calls == 1 && yesno_asks(g_e, y, ctl, why, 64),
            "the ask puts the cursor on no; yesno_asks: comyn asks");
        drain(g_e);
        const bool recorded = first_row_is(g_e.yesno_first[y], asked) && !first_row_yes(g_e.yesno_first[y]);
        wr16(menu, kMenuCursor, kRowYes);
        drain(g_e);
        check(recorded && first_row_is(g_e.yesno_first[y], asked) && !first_row_yes(g_e.yesno_first[y]),
            "the drain records the row of the open's first frame, no; a cursor moved later leaves the record");
        wr16(menu, kMenuCursor, kRowNo);
        const void* other = vt_self;
        memcpy(ctl, &other, 4);
        const bool foreign = !yesno_asks(g_e, y, ctl, why, sizeof(why))
            && strcmp(why, "comyn's controller is not the yes/no prompt this engine knows") == 0;
        const void* own = vt_yesno;
        memcpy(ctl, &own, 4);
        check(foreign, "yesno_asks: a controller of another class asks nothing the engine may answer");
        Command stale = cmd(kOpYesNoAnswer, "comyn", 0, 0, s1, gen1, 1);
        stale.verb = kVerbAnswer;
        stale.flags = kCmdInstance;
        stale.instance = static_cast<uint32_t>(asked - 1);
        run(stale);
        int n = events(&cursor, ev, 64);
        bool gone = false;
        for (int i = 0; i < n; ++i) {
            gone = gone || is_error(ev[i], s1, gen1, "answer", "comyn", "that prompt is gone");
        }
        check(gone && g_log.yesno_calls == 0,
            "an answer naming an earlier open of comyn is refused on the game thread, the callback not called");
        Command c = stale;
        c.instance = static_cast<uint32_t>(asked);
        run(c);
        check(g_log.yesno_calls == 1 && g_log.yesno_answer == 1 && g_log.yesno_context == ctl
                && !rdptr(ctl, kYesNoCallback) && !live("comyn") && strcmp(g_log.closes, "holdtime|comyn   |") == 0
                && g_log.select_calls == 1,
            "answer yes: the callback once, with 1 and the question's context, the question cleared, holdtime then"
            " comyn closed by name as the confirm closes them");
        clear_log();
        check(yesno_ask("comyn", "Log out?", true, true, 0) && rd16(live("comyn"), kMenuCursor) == kRowYes,
            "comyn asks, its default yes");
        drain(g_e);
        const bool yes_first = first_row_yes(g_e.yesno_first[y]) && first_row_is(g_e.yesno_first[y], g_e.instance[y]);
        run(cmd(kOpYesNoAnswer, "comyn", 0, 0, s1, gen1, 0));
        check(yes_first && g_log.yesno_calls == 1 && g_log.yesno_answer == 0 && !live("comyn"),
            "recorded yes first; answer no: the callback with 0");
        clear_log();
        yesno_ask("comyn", "Log out?", false, true, 0);
        run(cmd(kOpYesNoCancel, "comyn", 0, 0, s1, gen1));
        check(g_log.yesno_calls == 1 && g_log.yesno_answer == 0 && !rdptr(ctl, kYesNoCallback) && !live("comyn")
                && strcmp(g_log.closes, "comyn   |holdtime|") == 0,
            "cancel: the callback with 0, then comyn and holdtime closed, the cancel's order");
        events(&cursor, ev, 64);

        // merityn: built not cancellable
        clear_log();
        yesno_ask("merityn", "Spend?", false, true, 0);
        Command refused = cmd(kOpYesNoCancel, "merityn", 0, 0, s1, gen1);
        refused.verb = kVerbCancel;
        run(refused);
        n = events(&cursor, ev, 64);
        const bool kept = live("merityn") && rdptr(controller("merityn"), kYesNoCallback) && g_log.yesno_calls == 0;
        run(cmd(kOpYesNoAnswer, "merityn", 0, 0, s1, gen1, 0));
        check(n >= 1 && is_error(ev[n - 1], s1, gen1, "cancel", "merityn", "this question cannot be canceled; answer it")
                && kept && g_log.yesno_calls == 1 && !live("merityn"),
            "merityn's cancel is refused, its question kept; its answer ends it");

        // the resyn family: one controller, keyed to resynus
        clear_log();
        const int rn = name("resyn");
        const int us = name("resynus");
        uint8_t* shared = controller("resyn");
        check(yesno_ask("resyn", "Return?", true, false, 0) && live_menu(g_e, kPlain, us, NULL)
                && !live_menu(g_e, kPlain, rn, NULL)
                && !yesno_asks(g_e, rn, shared, why, sizeof(why)) && strcmp(why, "the game asks through resynus, not resyn") == 0
                && yesno_asks(g_e, us, shared, why, sizeof(why)),
            "the resyn family asks through resynus: resyn is refused naming it, resynus asks");
        Command wrong = cmd(kOpYesNoAnswer, "resynde", 0, 0, s1, gen1, 1);
        wrong.verb = kVerbAnswer;
        run(wrong);
        n = events(&cursor, ev, 64);
        const bool named = n >= 1 && is_error(ev[n - 1], s1, gen1, "answer", "resynde",
            "the game asks through resynus, not resynde") && g_log.yesno_calls == 0;
        run(cmd(kOpYesNoAnswer, "resynus", 0, 0, s1, gen1, 1));
        check(named && g_log.yesno_calls == 1 && g_log.yesno_answer == 1 && g_log.yesno_context == shared
                && !live("resynus"),
            "an answer to resynde is refused on the game thread; resynus's is the shared controller's");
        events(&cursor, ev, 64);

        // a question asked again from inside the callback, its default no
        clear_log();
        yesno_ask("comyn", "Log out?", true, true, 0, 1);
        uint8_t* before = live("comyn");
        const LONG opened = g_e.instance[y];
        run(cmd(kOpYesNoAnswer, "comyn", 0, 0, s1, gen1, 1));
        uint8_t* after = live("comyn");
        n = events(&cursor, ev, 64);
        bool reopened = false;
        for (int i = 0; i < n; ++i) {
            reopened = reopened || is_event(ev[i], kEvOpened, "comyn");
        }
        check(g_log.yesno_calls == 1 && g_log.yesno_answer == 1 && rdptr(ctl, kYesNoCallback) && ctl[kYesNoChained] == 0
                && after && after != before && g_e.instance[y] == opened + 1 && reopened
                && rd16(after, kMenuCursor) == kRowNo && g_log.select_calls == 1 && g_log.select_row == kRowNo
                && strcmp(g_log.closes, "comyn   |") == 0,
            "the callback asked again: comyn closed by its key and opened anew (a new open, an opened event), the"
            " chained flag cleared, select-row on no, its default; holdtime left to the new question");
        clear_log();
        run(cmd(kOpYesNoAnswer, "comyn", 0, 0, s1, gen1, 0));
        check(g_log.yesno_calls == 1 && g_log.yesno_answer == 0 && !rdptr(ctl, kYesNoCallback) && !live("comyn"),
            "the question asked again is answered as any other");
        clear_log();
        yesno_ask("comyn", "Log out?", true, true, 0, 2);
        run(cmd(kOpYesNoCancel, "comyn", 0, 0, s1, gen1));
        check(g_log.yesno_answer == 0 && rdptr(ctl, kYesNoCallback) && live("comyn")
                && rd16(live("comyn"), kMenuCursor) == kRowYes && g_log.select_calls == 0
                && strcmp(g_log.closes, "comyn   |holdtime|") == 0,
            "cancel, the callback asking again yes first: comyn opened anew with no select-row, then holdtime closed,"
            " the cancel's order");
        run(cmd(kOpYesNoCancel, "comyn", 0, 0, s1, gen1));
        events(&cursor, ev, 64);

        // asked again while shopbuy is blocked (comyn is hide-only): the
        // game's own open refused, the question left with no window; a reply
        // still ends it
        clear_log();
        const int sb = name("shopbuy");
        uint8_t* buy = controller("shopbuy");
        yesno_ask("shopbuy", "Buy this?", true, true, 0, 1);
        g_e.holds.set(h1, sb, kHoldBlock, true);
        run(cmd(kOpYesNoAnswer, "shopbuy", 0, 0, s1, gen1, 1));
        n = events(&cursor, ev, 64);
        bool refused_open = false;
        for (int i = 0; i < n; ++i) {
            refused_open = refused_open || is_event(ev[i], kEvBlocked, "shopbuy");
        }
        const bool stuck = refused_open && rdptr(buy, kYesNoCallback) && !live("shopbuy")
            && yesno_asks(g_e, sb, buy, why, sizeof(why)) && !yesno_ask("shopbuy", "Again?", true, true, 0);
        run(cmd(kOpYesNoAnswer, "shopbuy", 0, 0, s1, gen1, 0));
        check(stuck && g_log.yesno_calls == 2 && g_log.yesno_answer == 0 && !rdptr(buy, kYesNoCallback),
            "blocked, the open of the question asked again is refused (blocked): the question waits with no window"
            " and every new ask answers 0; a reply with no window still ends it");
        clear_log();
        check(!yesno_ask("shopbuy", "Buy this?", true, true, 0) && !rdptr(buy, kYesNoCallback) && !live("shopbuy")
                && g_log.yesno_calls == 0,
            "blocked, an ask's own open is refused: it answers 0 and clears its callback, nothing pending");
        g_e.holds.set(h1, sb, kHoldBlock, false);
        events(&cursor, ev, 64);

        // confirmed by holding: the reply is the confirm holdtime runs into
        clear_log();
        yesno_ask("comyn", "Log out?", false, true, 1);
        run(cmd(kOpYesNoAnswer, "comyn", 0, 0, s1, gen1, 1));
        check(g_log.yesno_calls == 1 && g_log.yesno_answer == 1 && !live("comyn")
                && strcmp(g_log.closes, "holdtime|comyn   |") == 0,
            "a question held to confirm (mode 1): answer yes is the confirm's, holdtime and comyn closed");
        events(&cursor, ev, 64);
    }

    // the KO window: its confirm asks its question through comyn with the
    // arguments its input handler passes, and nothing more; the confirm's
    // refusals, a hidden KO window holding keys off it, and no close of it
    {
        events(&cursor, ev, 64);
        clear_log();
        memset(&g_dead, 0, sizeof(g_dead));
        uint8_t* ko = controller("dead");
        uint8_t* asker = controller("comyn");
        const int cy = name("comyn");
        Command confirm = cmd(kOpDeadConfirm, "dead", 0, 0, s1, gen1);
        confirm.verb = kVerbAnswer;
        run(confirm);
        int n = events(&cursor, ev, 64);
        const char* down = dead_refusal(g_e, NULL, asker);
        check(n == 1 && is_error(ev[0], s1, gen1, "answer", "dead", "dead is not open") && g_dead.decider_calls == 0
                && g_dead.ask_calls == 0 && down && strcmp(down, "dead is not open") == 0,
            "dead's confirm with the KO window down: refused on the game thread, nothing called");
        uint8_t* menu = open_now("dead");
        events(&cursor, ev, 64);
        const char* none = dead_refusal(g_e, menu, asker);
        wr32(menu, kMenuState, kStateDormant);
        const char* covered = dead_refusal(g_e, menu, asker);
        wr32(menu, kMenuState, 0);
        wr16(menu, kMenuInputWait, 2);
        const char* waits = dead_refusal(g_e, menu, asker);
        wr16(menu, kMenuInputWait, 0);
        menu[kMenuBusy] = 1;
        const char* busy = dead_refusal(g_e, menu, asker);
        menu[kMenuBusy] = 0;
        const void* other = vt_self;
        memcpy(asker, &other, 4);
        const char* foreign = dead_refusal(g_e, menu, asker);
        const void* own = vt_yesno;
        memcpy(asker, &own, 4);
        const char* missing = dead_refusal(g_e, menu, NULL);
        check(menu && none == NULL && covered && strcmp(covered, "dead is under another window") == 0 && waits
                && strcmp(waits, "dead takes no input yet") == 0 && busy && strcmp(busy, "dead is busy") == 0
                && foreign && strcmp(foreign, "comyn's controller is not the yes/no prompt this engine knows") == 0
                && missing == foreign,
            "dead_refusal: open and taking input it asks; dormant, its input wait running, busy, or no yes/no"
            " controller in comyn's global, the input sink or the ask would not");
        yesno_ask("comyn", "Log out?", false, true, 0);
        const char* pending = dead_refusal(g_e, menu, asker);
        run(confirm);
        n = events(&cursor, ev, 64);
        bool refused = false;
        for (int i = 0; i < n; ++i) {
            refused = refused || is_error(ev[i], s1, gen1, "answer", "dead", "comyn already has a question pending");
        }
        check(pending && strcmp(pending, "comyn already has a question pending") == 0 && refused
                && g_dead.ask_calls == 0 && g_dead.decider_calls == 0,
            "with a question pending in comyn, the ask would answer 0: refused, nothing called");
        run(cmd(kOpYesNoAnswer, "comyn", 0, 0, s1, gen1, 0));
        events(&cursor, ev, 64);

        clear_log();
        memset(&g_dead, 0, sizeof(g_dead));
        g_dead.decider_answer = 0xABCD0000u;
        const LONG opens = g_e.instance[cy];
        run(confirm);
        n = events(&cursor, ev, 64);
        bool opened = false;
        for (int i = 0; i < n; ++i) {
            opened = opened || is_event(ev[i], kEvOpened, "comyn");
        }
        uint8_t* question = live("comyn");
        const int32_t pushed[5] = {0, 0, 0, 1, 0};
        check(g_dead.decider_calls == 1 && g_dead.decider_args[0] == 1 && g_dead.decider_args[1] == 0
                && g_dead.message_calls == 1 && g_dead.message_args[0] == 0x0E && g_dead.message_args[1] == 0x46
                && g_dead.ask_calls == 1 && g_dead.ask_ctl == asker
                && g_dead.ask_callback == reinterpret_cast<const void*>(&fake_dead_chosen)
                && g_dead.ask_text == g_text_home && memcmp(g_dead.ask_args, pushed, sizeof(pushed)) == 0
                && strcmp(g_log.order, "dmaW") == 0 && question && rd16(question, kMenuCursor) == kRowNo && opened
                && g_e.instance[cy] == opens + 1 && rdptr(asker, kYesNoCallback) && !ko[kDeadChosen]
                && live("dead") == menu,
            "dead's confirm: the decider with (1, 0), its 0 picking message 0x46 of category 0x0E, then comyn's ask"
            " with the handler's callback, context 0, no first, no help, cancellable, mode 0: comyn opens, its"
            " cursor on no; nothing else, the KO window left as it was");
        run(cmd(kOpYesNoAnswer, "comyn", 0, 0, s1, gen1, 1));
        check(ko[kDeadChosen] == 1 && !live("comyn") && !rdptr(asker, kYesNoCallback),
            "comyn's yes runs the question's callback, which marks the home point chosen");
        ko[kDeadChosen] = 0;
        memset(&g_dead, 0, sizeof(g_dead));
        g_dead.decider_answer = 0x12345601u;
        run(confirm);
        check(g_dead.message_args[0] == 0x0E && g_dead.message_args[1] == 0x258 && g_dead.ask_text == g_text_other
                && rdptr(asker, kYesNoCallback),
            "the decider's 1, al alone, picks message 0x258");
        run(cmd(kOpYesNoCancel, "comyn", 0, 0, s1, gen1));
        check(!ko[kDeadChosen] && !live("comyn") && !rdptr(asker, kYesNoCallback),
            "the question canceled through comyn: the callback with no, the home point not chosen");
        events(&cursor, ev, 64);

        // comyn's open refused (a hold set here stands for the game's refusal)
        g_e.holds.set(h1, cy, kHoldBlock, true);
        run(confirm);
        n = events(&cursor, ev, 64);
        refused = false;
        for (int i = 0; i < n; ++i) {
            refused = refused
                || is_error(ev[i], s1, gen1, "answer", "dead", "the game refused comyn's open; nothing was asked");
        }
        check(refused && !rdptr(asker, kYesNoCallback) && !live("comyn"),
            "comyn's open refused: the ask answers 0 and clears its callback; the confirm says nothing was asked");
        g_e.holds.set(h1, cy, kHoldBlock, false);
        events(&cursor, ev, 64);

        // hidden, the KO window is a prompt: no routed key reaches it
        g_e.holds.set(h1, name("dead"), kHoldHide, true);
        drain(g_e);
        wr32(g_mcb, kMcbActive, reinterpret_cast<uint32_t>(menu));
        const int sinks = g_sink_originals;
        for (int code = 1; code <= 0x20; ++code) {
            route(code);
        }
        const bool held = hidden_prompt(g_e, menu) && g_sink_originals == sinks;
        wr32(g_mcb, kMcbActive, 0);
        g_e.holds.set(h1, name("dead"), kHoldHide, false);
        drain(g_e);
        check(held && !hidden_prompt(g_e, menu),
            "hidden, the KO window is a hidden prompt: no routed code reaches its sink, so Enter asks nothing");

        // the drain closes no hide-only window
        run(cmd(kOpClose, "dead", 0, 0, s1, gen1));
        run(cmd(kOpBlockClose, "dead", 0, 0, s1, gen1));
        check(live("dead") == menu && menu_is_open(menu), "the drain closes no hide-only window, by close or by block");
        game_closes("dead");
        destroy("dead");
        events(&cursor, ev, 64);

        // nor the raise question's, by close or by block
        uint8_t* raise = (yesno_ask("resyn", "Return?", true, false, 0), live("resynus"));
        run(cmd(kOpClose, "resynus", 0, 0, s1, gen1));
        run(cmd(kOpBlockClose, "resynus", 0, 0, s1, gen1));
        check(raise && live("resynus") == raise && menu_is_open(raise) && rdptr(controller("resyn"), kYesNoCallback),
            "the drain closes no resyn window either: the raise question stays up");
        run(cmd(kOpYesNoAnswer, "resynus", 0, 0, s1, gen1, 0));
        events(&cursor, ev, 64);
    }

    // the number boxes: a question with its window up answered and canceled
    // through the box's own confirm, the number checked against its range; a
    // stale callback asking nothing; one a block kept from its window recorded
    // at the refused open, announced, answered the same way, and ended at
    // every way the game drops it, each end a pending event that the handle
    // whose reply ended it skips; the asker's preconditions refusing an answer
    // while the record stands
    {
        events(&cursor, ev, 64);
        memset(&g_box, 0, sizeof(g_box));
        clear_log();
        const int mb = name("moneyctr");
        const int ib = name("itemctrl");
        uint8_t* money = controller("moneyctr");
        uint8_t* item = controller("itemctrl");
        const BoxRecord& mr = g_e.boxes[kBoxMoney];
        const BoxRecord& ir = g_e.boxes[kBoxItem];
        auto reply = [&](uint8_t op, const char* nm, int32_t value) {
            Command c = cmd(op, nm, 0, 0, s1, gen1, static_cast<uint32_t>(value));
            c.verb = op == kOpBoxCancel ? kVerbCancel : kVerbAnswer;
            run(c);
        };
        // The events since the last call as one string: b(locked) n, p+ n or
        // p- n for a box's pending event (r when this handle's reply ended
        // it), o(pened) n and c(losed) n.
        auto seen = [&]() {
            static char out[256];
            out[0] = '\0';
            const int k = events(&cursor, ev, 64);
            for (int i = 0; i < k; ++i) {
                const int type = event_type(ev[i]);
                const int payload = event_name(ev[i]);
                char one[32] = "";
                if (type == kEvPending && pending_what(payload) >= kPendingBox) {
                    snprintf(one, sizeof(one), "p%c%s%s ", pending_on(payload) ? '+' : '-',
                        kBoxNames[pending_what(payload) - kPendingBox], pending_from(ev[i], s1, gen1) ? "r" : "");
                } else if (type == kEvBlocked || type == kEvOpened || type == kEvClosed) {
                    snprintf(one, sizeof(one), "%c%s ", type == kEvBlocked ? 'b' : type == kEvOpened ? 'o' : 'c',
                        g_e.inv.names[payload].name);
                } else if (type == kEvError) {
                    snprintf(one, sizeof(one), "error ");
                }
                strncat(out, one, sizeof(out) - strlen(out) - 1);
            }
            return static_cast<const char*>(out);
        };
        auto refused = [&](const char* verb, const char* nm, const char* reason) {
            const int k = events(&cursor, ev, 64);
            for (int i = 0; i < k; ++i) {
                if (is_error(ev[i], s1, gen1, verb, nm, reason)) {
                    return true;
                }
            }
            return false;
        };

        check(g_e.inv.box_window[kBoxMoney] == mb && g_e.inv.box_window[kBoxItem] == ib
                && g_e.inv.box_of(mb) == kBoxMoney && g_e.inv.box_of(ib) == kBoxItem && g_e.inv.box_of(name("comyn")) < 0
                && box_named("itemctrl") == kBoxItem && box_named("query") < 0 && prompt_window("moneyctr", 8)
                && !prompt_window("itemctrl", 7) && prompt_cancels("moneyctr") && !yesno_window("moneyctr"),
            "the number boxes: prompts from engine abi 8 that cancel() ends, by index both ways");
        {
            const uint32_t plain = pack_pending(kPendingBox + kBoxItem, true);
            const uint32_t replied = pack_pending(kPendingBox + kBoxMoney, false, 5, 0x1234);
            const uint32_t distant = pack_pending(kPendingPost, false, kPendingSlots, 7);
            check(event_type(plain) == kEvPending && pending_what(event_name(plain)) == kPendingBox + kBoxItem
                    && pending_on(event_name(plain)) && !pending_from(plain, 0, 0)
                    && pending_what(event_name(replied)) == kPendingBox + kBoxMoney && !pending_on(event_name(replied))
                    && pending_from(replied, 5, 0x1234) && pending_from(replied, 5, 0x34) && !pending_from(replied, 5, 0x35)
                    && !pending_from(replied, 4, 0x1234) && !pending_from(replied, -1, 0)
                    && pack_pending(kPendingInvite, true) == pack_event(kEvPending, pending_payload(kPendingInvite, true))
                    && pending_what(event_name(distant)) == kPendingPost && !pending_from(distant, kPendingSlots, 7),
                "pack_pending: what and whether pending as before, and the replying handle's slot + 1 and its"
                " generation's low byte, which pending_from matches alone; a slot past the bits goes unnamed");
        }

        // asked with its window up: answered through the confirm, the number
        // in range
        uint8_t* handover = open_now("handover");
        seen();
        uint8_t* up = money_setup(100, 0, kAskerCallbacks[kAskHandoverGil], NULL);
        check(up && live("moneyctr") == up && rd32(money, 0x24) == static_cast<uint32_t>(kGil)
                && rd32(money, 0x28) == 100 && !mr.active && strcmp(seen(), "omoneyctr ") == 0,
            "moneyctr asked with its window up: the max of 0 asked for is the gil the player has; no record");
        reply(kOpBoxAnswer, "moneyctr", kGil + 1);
        const bool high = refused("answer", "moneyctr", "5001 is outside 0..5000, what moneyctr takes now");
        reply(kOpBoxAnswer, "moneyctr", -1);
        check(high && refused("answer", "moneyctr", "-1 is outside 0..5000") && g_box.calls == 0
                && g_box.confirms[kBoxMoney] == 0 && live("moneyctr") == up,
            "a number past the max or under 0 is refused on the game thread, nothing called");
        reply(kOpBoxAnswer, "moneyctr", 2500);
        check(g_box.confirms[kBoxMoney] == 1 && g_box.confirm_value == 2500 && g_box.calls == 1 && g_box.value == 2500
                && g_box.who == kAskHandoverGil && g_box.context == NULL && !rdptr(money, 0x2C) && !live("moneyctr")
                && strcmp(seen(), "cmoneyctr ") == 0,
            "answer: moneyctr's confirm with the number, the callback with it and the context, the callback cleared,"
            " the window closed by instance");
        money_setup(0, 300, kAskerCallbacks[kAskHandoverGil], NULL);
        reply(kOpBoxCancel, "moneyctr", 0);
        check(g_box.confirms[kBoxMoney] == 2 && g_box.confirm_value == kBoxCancel && g_box.value == -1
                && !live("moneyctr") && rd32(money, 0x24) == 300,
            "cancel: the confirm with -1, the window's own cancel");
        seen();

        // a callback a close from outside leaves set asks nothing
        money_setup(0, 0, kAskerCallbacks[kAskHandoverGil], NULL);
        game_closes("moneyctr");
        reply(kOpBoxAnswer, "moneyctr", 1);
        check(rdptr(money, 0x2C) && !live("moneyctr") && refused("answer", "moneyctr", "no number is pending in moneyctr")
                && g_box.calls == 2,
            "a close from outside leaves the callback set with no window: no number is pending, nothing called");
        wr32(money, 0x2C, 0);
        int sinks = 0;
        {
            uint8_t* box = item_setup(1, 1, kAskerCallbacks[kAskShopBuy], NULL);
            reply(kOpBoxAnswer, "itemctrl", 1);
            check(!box && g_box.calls == 3 && g_box.value == 1 && g_box.who == kAskShopBuy && rdptr(item, 0x24)
                    && !live("itemctrl") && !ir.active && refused("answer", "itemctrl", "no number is pending in itemctrl"),
                "itemctrl asked for at most 1 answers itself with no window and leaves its callback set: nothing is"
                " pending");
            // hidden, a box takes none of the player's keys
            uint8_t* shown = item_setup(1, 9, kAskerCallbacks[kAskShopBuy], NULL);
            g_e.holds.set(h1, ib, kHoldHide, true);
            drain(g_e);
            wr32(g_mcb, kMcbActive, reinterpret_cast<uint32_t>(shown));
            sinks = g_sink_originals;
            for (int code = 1; code <= 0x20; ++code) {
                route(code);
            }
            const bool held = shown && hidden_prompt(g_e, shown) && g_sink_originals == sinks;
            wr32(g_mcb, kMcbActive, 0);
            g_e.holds.set(h1, ib, kHoldHide, false);
            drain(g_e);
            reply(kOpBoxAnswer, "itemctrl", 9);
            check(held && g_box.value == 9 && !live("itemctrl"),
                "hidden, itemctrl is a hidden prompt: no routed code reaches it; its answer still ends it");
            seen();
        }

        // blocked: the question recorded at the refused open and announced
        uint8_t* trade = open_now("trade");
        uint8_t* trade_ctl = controller("trade");
        wr32(trade_ctl, kTradeState, kTradeOpen);
        wr16(trade, kMenuCursor, 3);
        uint8_t picked[8] = {0x34, 0x12, 7, 0, 12, 0, 0, 0};
        g_e.holds.set(h1, ib, kHoldBlock, true);
        drain(g_e);
        seen();
        const LONG opens = g_e.instance[ib];
        uint8_t* none = item_setup(1, 12, kAskerCallbacks[kAskTradeItem], picked);
        check(!none && !live("itemctrl") && ir.active && ir.asker == kAskTradeItem && ir.owner == trade && ir.row == 3
                && ir.field == kTradeOpen && ir.id == static_cast<uint32_t>(opens + 1) && ir.max == 12
                && ir.context == picked && ir.callback == reinterpret_cast<const void*>(kAskerCallbacks[kAskTradeItem])
                && ir.ctl == item && g_e.instance[ib] == opens + 1 && strcmp(seen(), "bitemctrl p+itemctrl ") == 0,
            "itemctrl blocked: its refused open records the question its setup wrote, with trade's window, cursor row"
            " and state, and the box's open count as its id; one blocked event, then a pending one");
        drain(g_e);
        drain(g_e);
        check(ir.active && strcmp(seen(), "") == 0, "the record stands across frames while nothing moves");
        wr16(trade, kMenuCursor, 4);
        reply(kOpBoxAnswer, "itemctrl", 5);
        const bool moved = refused("answer", "itemctrl", "trade's cursor is not on the row the question was asked from");
        wr16(trade, kMenuCursor, 3);
        wr32(trade_ctl, kTradeState, 4);
        reply(kOpBoxAnswer, "itemctrl", 5);
        const bool confirmed = refused("answer", "itemctrl", kTradeMoved);
        wr32(trade_ctl, kTradeState, kTradeOpen);
        reply(kOpBoxAnswer, "itemctrl", 13);
        check(moved && confirmed && refused("answer", "itemctrl", "13 is outside 1..12, what itemctrl takes now")
                && ir.active && g_box.confirms[kBoxItem] == 1,
            "while it stands, an answer is refused with trade's cursor off the row it asked from, with its offer"
            " confirmed, or with a number outside 1..12; the record stays");
        Command named = cmd(kOpBoxAnswer, "itemctrl", 0, 0, s1, gen1, 5);
        named.verb = kVerbAnswer;
        named.flags = kCmdInstance;
        named.instance = ir.id - 1;
        run(named);
        const bool stale = refused("answer", "itemctrl", "that prompt is gone") && ir.active;
        named.instance = ir.id;
        run(named);
        check(stale && !ir.active && g_box.confirms[kBoxItem] == 2 && g_box.confirm_value == 5 && g_box.value == 5
                && g_box.context == picked && g_box.who == kAskTradeItem && !rdptr(item, 0x24)
                && strcmp(seen(), "p-itemctrlr ") == 0,
            "answer naming the prompt: the record ended first, its pending event marked as this handle's reply, then"
            " itemctrl's confirm with no window, the callback with the number and the item record");

        // a cancel needs no row
        item_setup(1, 12, kAskerCallbacks[kAskTradeItem], picked);
        wr16(trade, kMenuCursor, 4);
        reply(kOpBoxCancel, "itemctrl", 0);
        check(!ir.active && g_box.value == -1 && strcmp(seen(), "bitemctrl p+itemctrl p-itemctrlr ") == 0,
            "cancel with trade's cursor moved: the callback's -1 needs no row; the record ended");
        wr16(trade, kMenuCursor, 3);

        // ended by the game: trade's window closing
        item_setup(1, 12, kAskerCallbacks[kAskTradeItem], picked);
        seen();
        game_closes("trade");
        reply(kOpBoxAnswer, "itemctrl", 2);
        check(!ir.active && refused("answer", "itemctrl", "no number is pending in itemctrl") && g_box.value == -1,
            "trade's window closes in the frame an answer is queued: the drain ends the record before its commands,"
            " the answer finds nothing, nothing called");
        trade = open_now("trade");
        item_setup(1, 12, kAskerCallbacks[kAskTradeItem], picked);
        seen();
        game_closes("trade");
        drain(g_e);
        check(!ir.active && strcmp(seen(), "ctrade p-itemctrl ") == 0,
            "trade's window closing ends the record: a pending event no handle's reply ended");

        // ended by the game: the box's confirm run from elsewhere, as the
        // shop's close cancels its count
        uint8_t* shop = open_now("shop");
        item_setup(1, 20, kAskerCallbacks[kAskShopBuy], NULL);
        seen();
        fake_item_confirm(item, NULL, -1);
        drain(g_e);
        check(shop && !ir.active && g_box.who == kAskShopBuy && g_box.value == -1 && strcmp(seen(), "p-itemctrl ") == 0,
            "the shop's close canceling the count through the confirm: the callback cleared, the record ended");

        // ended by the game: an ask that answers itself rewrites the box's max
        uint8_t* bank = open_now("bank");
        uint8_t* entry = picked;
        item_setup(1, 5, kAskerCallbacks[kAskStorage], &entry);
        seen();
        item_setup(1, 1, kAskerCallbacks[kAskStorage], &entry);
        drain(g_e);
        check(bank && !ir.active && g_box.who == kAskStorage && g_box.value == 1 && strcmp(seen(), "p-itemctrl ") == 0,
            "an ask of 1 with the same callback and context answers itself, and its max of 1 ends the record");
        wr32(item, 0x24, 0);

        // replaced: another ask's refused open records afresh
        item_setup(1, 9, kAskerCallbacks[kAskShopBuy], NULL);
        seen();
        item_setup(1, 7, kAskerCallbacks[kAskShopSell], picked);
        check(ir.active && ir.asker == kAskShopSell && ir.max == 7 && ir.owner == shop
                && strcmp(seen(), "p-itemctrl bitemctrl p+itemctrl ") == 0,
            "another refused open: the old question's end, its blocked event, the new one's pending event");

        // ended by a later open of the window
        g_e.holds.set(h1, ib, kHoldBlock, false);
        drain(g_e);
        uint8_t* opened = item_setup(1, 7, kAskerCallbacks[kAskShopSell], picked);
        check(opened && !ir.active && strcmp(seen(), "p-itemctrl oitemctrl ") == 0,
            "the window opening on a later ask ends the record: its question has its window");
        reply(kOpBoxCancel, "itemctrl", 0);
        seen();

        // the outgoing box's gil: its state the question waits in
        uint8_t* delivery = open_now("delivery");
        uint8_t* delivery_ctl = controller("delivery");
        wr16(delivery_ctl, kDeliveryState, kDeliveryIdle);
        wr16(delivery, kMenuCursor, 2);
        g_e.holds.set(h1, mb, kHoldBlock, true);
        drain(g_e);
        money_setup(0, 0, kAskerCallbacks[kAskDeliveryGil], NULL);
        wr16(delivery_ctl, kDeliveryState, kDeliveryGil);
        drain(g_e);
        const bool waits = mr.active && mr.asker == kAskDeliveryGil && mr.row == 2 && mr.field == kDeliveryGil
            && mr.owner == delivery && mr.max == kGil;
        wr16(delivery_ctl, kDeliveryState, 0x15);
        drain(g_e);
        check(waits && !mr.active && strcmp(seen(), "odelivery bmoneyctr p+moneyctr p-moneyctr ") == 0,
            "the outgoing box's gil question stands while the box waits in its state, and ends once the box moves on"
            " (its closing states)");
        wr16(delivery_ctl, kDeliveryState, kDeliveryIdle);

        // moneyctr's extra callback, set after the setup returns
        uint8_t* auc1 = open_now("auc1");
        money_setup(0, 999999999, kAskerCallbacks[kAskAuctionSell], picked);
        {
            void (*extra)() = &extra_callback;
            memcpy(money + 0x34, &extra, 4);
        }
        clear_log();
        reply(kOpBoxAnswer, "moneyctr", 1200);
        check(auc1 && g_box.who == kAskAuctionSell && g_box.value == 1200 && g_box.extras == 1 && !rdptr(money, 0x34)
                && !mr.active && strstr(g_log.order, "g#x") != NULL,
            "moneyctr's confirm runs the extra callback the auction set after its setup, after the callback");
        seen();

        // the bid names a row of a result list a new search frees
        money_setup(0, 0, kAskerCallbacks[kAskAuctionBid], picked);
        drain(g_e);
        const bool bid = mr.active && mr.asker == kAskAuctionBid && mr.owner == auc1;
        const int bids = g_box.calls;
        reply(kOpBoxAnswer, "moneyctr", 500);
        const bool no_bid = refused("answer", "moneyctr", kBidBlocked) && mr.active && g_box.calls == bids;
        reply(kOpBoxCancel, "moneyctr", 0);
        check(bid && no_bid && g_box.calls == bids + 1 && g_box.who == kAskAuctionBid && g_box.value == -1
                && !mr.active,
            "a blocked auction bid takes no number, its cancel still reaching the bid's callback");
        seen();

        // a callback no known asker has: pending, but no reply
        money_setup(0, 50, &unknown_asker_callback, NULL);
        drain(g_e);
        const bool kept = mr.active && mr.asker == -1 && mr.owner_n == -1;
        const int calls = g_box.calls;
        reply(kOpBoxAnswer, "moneyctr", 10);
        const bool no_answer = refused("answer", "moneyctr", "moneyctr's question came from a window hideui does not"
            " know, so it takes no reply while moneyctr is blocked");
        reply(kOpBoxCancel, "moneyctr", 0);
        check(kept && no_answer && refused("cancel", "moneyctr", "does not know") && mr.active && g_box.calls == calls,
            "an unknown asker's question is recorded and stands, but answer and cancel are refused");

        // the controller swapped out of the box's registry slot ends it
        uint8_t* slot = g_ctl_global[first_row("moneyctr")];
        static uint8_t other[0x100];
        g_ctl_global[first_row("moneyctr")] = other;
        drain(g_e);
        g_ctl_global[first_row("moneyctr")] = slot;
        check(!mr.active && box_record_gone(mr, kBoxMoney, money, NULL, NULL) != NULL,
            "another controller in moneyctr's registry slot ends the record");
        wr32(money, 0x2C, 0);
        seen();

        // handover's picked item goes to the row picked for, kept at the ask
        uint8_t* handover_ctl = controller("handover");
        handover_ctl[kHandoverRow] = 2;
        g_e.holds.set(h1, ib, kHoldBlock, true);
        drain(g_e);
        item_setup(1, 4, kAskerCallbacks[kAskHandoverItem], picked);
        handover_ctl[kHandoverRow] = 5;
        reply(kOpBoxAnswer, "itemctrl", 3);
        const bool other_row = refused("answer", "itemctrl", "handover's cursor is not on the row the item was picked"
            " for");
        handover_ctl[kHandoverRow] = 2;
        reply(kOpBoxAnswer, "itemctrl", 3);
        check(other_row && g_box.who == kAskHandoverItem && g_box.value == 3 && !ir.active,
            "handover's item count is refused once its row has moved from the one picked for, and taken back on it");

        // the outgoing box's item count waits in its idle state
        wr16(delivery, kMenuCursor, 4);
        item_setup(1, 6, kAskerCallbacks[kAskDeliveryItem], picked);
        wr16(delivery_ctl, kDeliveryState, kDeliveryGil);
        reply(kOpBoxAnswer, "itemctrl", 2);
        const bool busy = refused("answer", "itemctrl", "the outgoing box is busy with another step") && ir.active;
        wr16(delivery_ctl, kDeliveryState, kDeliveryIdle);
        reply(kOpBoxAnswer, "itemctrl", 2);
        check(busy && g_box.who == kAskDeliveryItem && g_box.value == 2 && !ir.active,
            "the outgoing box's item count is refused while the box is on another step, and taken in its idle state");
        g_e.holds.set(h1, ib, kHoldBlock, false);
        g_e.holds.set(h1, mb, kHoldBlock, false);
        drain(g_e);
        seen();

        // with its window up, an answer whose callback reads its asker's
        // window unchecked needs that window
        item_setup(1, 3, kAskerCallbacks[kAskSynthesis], picked);
        reply(kOpBoxAnswer, "itemctrl", 2);
        const bool gone = refused("answer", "itemctrl", "tskill1, which asked, is not open") && live("itemctrl");
        reply(kOpBoxCancel, "itemctrl", 0);
        check(gone && g_box.who == kAskSynthesis && g_box.value == -1 && !live("itemctrl"),
            "synthesis's count with tskill1 down: the answer refused, the cancel taken");
        open_now("tskill1");
        item_setup(1, 3, kAskerCallbacks[kAskSynthesis], picked);
        reply(kOpBoxAnswer, "itemctrl", 2);
        check(g_box.value == 2 && !live("itemctrl"), "with tskill1 open, the answer goes through");

        char buf[128];
        const char* refusal = box_window_refusal(kAskTradeGil, false, true, buf, sizeof(buf));
        check(handover && refusal && strcmp(refusal, "trade, which asked, is not open") == 0
                && !box_window_refusal(kAskTradeGil, true, true, buf, sizeof(buf))
                && !box_window_refusal(kAskTradeGil, false, false, buf, sizeof(buf))
                && !box_window_refusal(kAskHandoverGil, false, true, buf, sizeof(buf))
                && !box_window_refusal(-1, false, true, buf, sizeof(buf)),
            "box_window_refusal: only an answer to an asker whose callback reads its window, that window closed");
        const char* const askers[] = {"handover", "trade", "shop", "bank", "delivery", "auc1", "tskill1"};
        for (size_t i = 0; i < sizeof(askers) / sizeof(askers[0]); ++i) {
            if (live(askers[i])) {
                game_closes(askers[i]);
            }
        }
        seen();
        clear_log();
    }

    // the Asked windows: each with its window up answered and canceled as its
    // own confirm and cancel do, and refused where its input routine would
    // take nothing; the stale fields a close from outside leaves asking
    // nothing; a question a block kept from its window recorded at the
    // refused open (bankmenu's read from its own field), answered with no
    // window as its confirm would have, and ended at each way the game drops
    // it, every end a pending event the replying handle skips; hidden, none
    // takes a routed key or the menu mouse mode
    {
        events(&cursor, ev, 64);
        memset(&g_asked, 0, sizeof(g_asked));
        clear_log();
        auto reply = [&](uint8_t op, const char* nm, int32_t value, uint8_t flags) {
            Command c = cmd(op, nm, 0, 0, s1, gen1, static_cast<uint32_t>(value));
            c.verb = op == kOpAskedCancel ? kVerbCancel : kVerbAnswer;
            c.flags = flags;
            run(c);
        };
        auto answer = [&](const char* nm, int32_t value) { reply(kOpAskedAnswer, nm, value, 0); };
        auto cancel = [&](const char* nm) { reply(kOpAskedCancel, nm, 0, 0); };
        // The events since the last call as one string: b(locked) n, p+ n or
        // p- n for a pending event (r when this handle's reply ended it),
        // o(pened) n and c(losed) n.
        auto seen = [&]() {
            static char out[256];
            out[0] = '\0';
            const int k = events(&cursor, ev, 64);
            for (int i = 0; i < k; ++i) {
                const int type = event_type(ev[i]);
                const int payload = event_name(ev[i]);
                char one[40] = "";
                if (type == kEvPending) {
                    snprintf(one, sizeof(one), "p%c%s%s ", pending_on(payload) ? '+' : '-',
                        pending_name(pending_what(payload)), pending_from(ev[i], s1, gen1) ? "r" : "");
                } else if (type == kEvBlocked || type == kEvOpened || type == kEvClosed) {
                    snprintf(one, sizeof(one), "%c%s ", type == kEvBlocked ? 'b' : type == kEvOpened ? 'o' : 'c',
                        g_e.inv.names[payload].name);
                } else if (type == kEvPage) {
                    snprintf(one, sizeof(one), "g%s ", g_e.inv.names[payload].name);
                } else if (type == kEvError) {
                    snprintf(one, sizeof(one), "error ");
                }
                strncat(out, one, sizeof(out) - strlen(out) - 1);
            }
            return static_cast<const char*>(out);
        };
        auto refused = [&](const char* verb, const char* nm, const char* reason) {
            const int k = events(&cursor, ev, 64);
            for (int i = 0; i < k; ++i) {
                if (is_error(ev[i], s1, gen1, verb, nm, reason)) {
                    return true;
                }
            }
            return false;
        };
        auto block = [&](const char* nm, bool on) {
            g_e.holds.set(h1, name(nm), kHoldBlock, on);
            drain(g_e);
        };
        auto active = [&](const uint8_t* menu) { wr32(g_mcb, kMcbActive, reinterpret_cast<uint32_t>(menu)); };
        bool blocked = false;
        auto pending_now = [&](int a) {
            char buf[kErrorReason];
            return !asked_pending(g_e, a, g_e.asked[a], asked_now(g_e, kPlain, a, g_e.asked[a]), &blocked, buf,
                sizeof(buf));
        };

        check(g_e.inv.asked_window[kAskedSortyn] == name("sortyn") && g_e.inv.asked_of(name("ok")) == kAskedOk
                && g_e.inv.asked_of(name("comyn")) < 0 && asked_named("maplist") == kAskedMaplist
                && asked_named("query") < 0 && asked_records(kAskedSortyn) && asked_records(kAskedEvitem)
                && asked_records(kAskedMaplist) && asked_records(kAskedOk) && asked_records(kAskedDoor)
                && !asked_records(kAskedBank) && asked_named("mogdoor") == kAskedDoor
                && !asked_records(kAskedConfyn) && !asked_records(kAskedGmtell) && !asked_records(kAskedMpDead)
                && prompt_window("bankmenu", 8) && !prompt_window("bankmenu", 7) && prompt_cancels("maplist")
                && !prompt_cancels("confyn") && !prompt_cancels("ok") && boolean_window("sortyn")
                && boolean_window("confyn") && !boolean_window("ok") && !boolean_window("bankmenu"),
            "the Asked windows: prompts from engine abi 8, by index both ways; sortyn, evitem01, maplist, ok and"
            " mogdoor keep a blocked question in a record, bankmenu's waits in its own field, confyn, gmtell and"
            " mp_dead in neither;"
            " sortyn, evitem01 and confyn answered true or false");
        {
            const uint32_t on = pack_pending(kPendingAsked + kAskedOk, true);
            const uint32_t off = pack_pending(kPendingAsked + kAskedBank, false, 9, 0x77);
            check(pending_what(event_name(on)) == kPendingAsked + kAskedOk && pending_on(event_name(on))
                    && strcmp(pending_name(kPendingAsked + kAskedOk), "ok") == 0
                    && pending_what(event_name(off)) == kPendingAsked + kAskedBank && !pending_on(event_name(off))
                    && pending_from(off, 9, 0x77) && !pending_from(off, 8, 0x77)
                    && strcmp(pending_name(kPendingAsked + kAskedBank), "bankmenu") == 0
                    && kPendingCount <= (1 << kPendingWhatBits) && strcmp(pending_name(kPendingCount), "") == 0,
                "pack_pending: every Asked window's what fits its bits and is named as its window; the replying"
                " handle kept");
        }

        // hidden, a prompt with its question pending: no routed key, the menu
        // mouse mode held off it; with none pending (confyn's is pending
        // while it is up) it takes them as shown
        {
            auto question = [&](int a, bool on) {
                uint8_t* ctl = controller(kAskedNames[a]);
                const void* cb = on ? reinterpret_cast<const void*>(&extra_callback) : NULL;
                switch (a) {
                case kAskedSortyn:
                    memcpy(ctl + kSortynCallback, &cb, 4);
                    break;
                case kAskedEvitem: {
                    const void* asks = NULL;
                    if (on) {
                        memcpy(&asks, &g_e.game.asked.evitem_callback, 4);
                    }
                    memcpy(ctl + kEvitemCallback, &asks, 4);
                    break;
                }
                case kAskedBank:
                    memcpy(ctl + kBankCallback, &cb, 4);
                    break;
                case kAskedMaplist: {
                    const uint8_t* rows = on ? g_map_rows : NULL;
                    memcpy(ctl + kMapRows, &rows, 4);
                    wr32(ctl, kMapCount, on ? 1 : 0);
                    wr32(ctl, kMapCapacity, on ? kMapRowMax : 0);
                    memcpy(ctl + kMapCallback, &cb, 4);
                    break;
                }
                case kAskedGmtell:
                    wr16(ctl, kQueryResult, on ? 0 : 1);
                    break;
                case kAskedMpDead:
                    wr32(ctl, kMpDeadChoice, on ? 0 : 1);
                    break;
                case kAskedOk:
                    wr32(ctl, kOkAnswered, on ? 0 : 1);
                    break;
                case kAskedDoor:
                    g_door_exiting[0] = on ? 0 : 1;
                    break;
                default:
                    break;
                }
            };
            bool held = true;
            bool taken = true;
            uint8_t mouse[0x60];
            for (int a = 0; a < kAskedCount; ++a) {
                const int n = g_e.inv.asked_window[a];
                g_e.holds.set(h1, n, kHoldHide, true);
                drain(g_e);
                uint8_t* menu = open_now(kAskedNames[a]);
                question(a, false);
                taken = taken && menu && input_on(menu) == (a == kAskedConfyn ? 0 : 1);
                question(a, true);
                active(menu);
                const int sinks = g_sink_originals;
                for (int code = 1; code <= 0x20; ++code) {
                    route(code);
                }
                memset(mouse, 0, sizeof(mouse));
                mouse[kMouseMode] = 2;
                hooked_mouse_mode(mouse);
                held = held && menu && hidden_prompt(g_e, menu) && g_sink_originals == sinks
                    && mouse[kMouseMode] == kMouseWorld && menu[kMenuMouse] == 0;
                active(NULL);
                question(a, false);
                g_e.holds.set(h1, n, kHoldHide, false);
                drain(g_e);
                game_closes(kAskedNames[a]);
            }
            g_door_exiting[0] = 0;
            wr16(controller("gmtell"), kQueryResult, 0);
            wr32(controller("mp_dead"), kMpDeadChoice, 0);
            wr32(controller("ok"), kOkAnswered, 0);
            check(held, "hidden, every Asked window with its question pending is a hidden prompt: no routed code, 1 to"
                " 0x20, reaches it, its +0x77 is 0 and the mouse mode picker writes world mode");
            check(taken, "hidden with no question pending (its callback clear, maplist's rows gone, gmtell's, mp_dead's"
                " and ok's answer in, mogdoor's exit under way), every Asked window but confyn, which asks while it is"
                " up, takes the routed key and the menu mouse mode as shown");
            seen();
        }

        // sortyn with its window up
        uint8_t* sortyn = controller("sortyn");
        const AskedRecord& sr = g_e.asked[kAskedSortyn];
        uint8_t item_record[8] = {0x34, 0x12, 5, 0, 1, 0, 0, 0};
        uint8_t* up = sortyn_setup(item_record, false);
        check(up && live("sortyn") == up && rd16(up, kMenuCursor) == kRowNo && pending_now(kAskedSortyn) && !blocked
                && !sr.active && strcmp(seen(), "osortyn ") == 0,
            "sortyn asked with its window up: pending, not blocked, the cursor on no as its default; no record");
        clear_log();
        answer("sortyn", 1);
        check(g_asked.calls == 1 && g_asked.who == kAskedSortyn && g_asked.value == 1 && g_asked.context == item_record
                && !rdptr(sortyn, kSortynCallback) && !live("sortyn") && strstr(g_log.order, "@K") != NULL
                && strcmp(g_log.last_close, "menu    sortyn  ") == 0,
            "answer(true): the callback with the context and 1, the callback cleared, then sortyn closed by name, as"
            " its confirm does");
        up = sortyn_setup(item_record, true);
        const bool yes_first = up && rd16(up, kMenuCursor) == kRowYes;
        cancel("sortyn");
        check(yes_first && g_asked.calls == 2 && g_asked.value == 0 && !rdptr(sortyn, kSortynCallback)
                && !live("sortyn"),
            "cancel: the callback with 0, cleared, the window closed");
        sortyn_setup(item_record, false);
        game_closes("sortyn");
        answer("sortyn", 1);
        check(rdptr(sortyn, kSortynCallback) && refused("answer", "sortyn", "no question is pending in sortyn")
                && g_asked.calls == 2,
            "a close from outside leaves sortyn's callback set with no window: nothing pending, nothing called");
        wr32(sortyn, kSortynCallback, 0);
        seen();

        // sortyn blocked: kept with the active menu as the window that asked
        uint8_t* iuse = open_now("iuse");
        active(iuse);
        block("sortyn", true);
        seen();
        const LONG opens = g_e.instance[name("sortyn")];
        uint8_t* none = sortyn_setup(item_record, false);
        check(!none && sr.active && sr.owner == iuse && sr.owner_n == name("iuse") && sr.ctl == sortyn
                && sr.context == item_record && sr.callback == reinterpret_cast<const void*>(&sortyn_answered)
                && sr.id == static_cast<uint32_t>(opens + 1) && strcmp(seen(), "bsortyn p+sortyn ") == 0,
            "sortyn blocked: its refused open keeps the question its setup wrote, the active menu (iuse) as the"
            " window that asked and the open count as its id; a blocked event, then a pending one");
        drain(g_e);
        check(sr.active && pending_now(kAskedSortyn) && blocked && strcmp(seen(), "") == 0,
            "the record stands across frames: pending, blocked");
        Command named = cmd(kOpAskedAnswer, "sortyn", 0, 0, s1, gen1, 1);
        named.verb = kVerbAnswer;
        named.flags = kCmdInstance;
        named.instance = sr.id - 1;
        run(named);
        const bool stale = refused("answer", "sortyn", "that prompt is gone") && sr.active;
        named.instance = sr.id;
        run(named);
        check(stale && !sr.active && g_asked.calls == 3 && g_asked.value == 1 && g_asked.context == item_record
                && !rdptr(sortyn, kSortynCallback) && strcmp(seen(), "p-sortynr ") == 0,
            "answer naming the prompt: an older id refused; the record ended first, its pending event marked as this"
            " handle's reply, then the callback with the context and 1 and the callback cleared, as the confirm does");
        sortyn_setup(item_record, false);
        cancel("sortyn");
        check(!sr.active && g_asked.calls == 4 && g_asked.value == 0 && strcmp(seen(), "bsortyn p+sortyn p-sortynr ") == 0,
            "cancel with no window: the callback with 0");
        sortyn_setup(item_record, false);
        seen();
        game_closes("iuse");
        drain(g_e);
        const bool asker_gone = !sr.active && strcmp(seen(), "ciuse p-sortyn ") == 0;
        answer("sortyn", 1);
        check(asker_gone && refused("answer", "sortyn", "no question is pending in sortyn") && g_asked.calls == 4,
            "iuse closing ends the record, a pending event no handle's reply ended; the question is gone");
        iuse = open_now("iuse");
        active(iuse);
        sortyn_setup(item_record, false);
        seen();
        uint8_t* other_record = item_record + 4;
        memcpy(sortyn + kSortynContext, &other_record, 4);
        drain(g_e);
        check(!sr.active && strcmp(seen(), "p-sortyn ") == 0, "another question written into sortyn ends the record");
        active(NULL);
        sortyn_setup(item_record, false);
        answer("sortyn", 1);
        check(!sr.active && strcmp(seen(), "bsortyn error ") == 0 && g_asked.calls == 4,
            "with no active menu nothing asked: no record, nothing pending, the answer refused");
        active(iuse);
        sortyn_setup(item_record, false);
        seen();
        block("sortyn", false);
        up = sortyn_setup(item_record, false);
        check(up && !sr.active && strcmp(seen(), "p-sortyn osortyn ") == 0,
            "a later open with the block gone ends the record: its question has its window");
        cancel("sortyn");
        game_closes("iuse");
        active(NULL);
        seen();

        // evitem01: the callback every ask sets, closed by instance with the
        // byte at +0x70
        uint8_t* evitem01 = controller("evitem01");
        uint8_t* evitem = open_now("evitem");
        const AskedRecord& er = g_e.asked[kAskedEvitem];
        active(evitem);
        seen();
        up = evitem_ask(0);
        check(up && pending_now(kAskedEvitem) && !blocked && rd16(up, kMenuCursor) == kRowYes && !er.active,
            "evitem01 asked with its window up: pending, the cursor on yes, its default; no record");
        evitem01[kEvitemCloseFlag] = 0x5A;
        g_asked.instance_closes = 0;
        answer("evitem01", 1);
        check(g_asked.who == kAskedEvitem && g_asked.value == 1 && g_asked.instance_closes == 1
                && g_asked.instance_flag == 0x5A && !live("evitem01") && rdptr(evitem01, kEvitemCallback),
            "answer(true): the callback with 1, then close by instance with the byte at +0x70, as its input routine"
            " does; the callback stays set, as the game leaves it");
        evitem_ask(0);
        cancel("evitem01");
        answer("evitem01", 1);
        check(g_asked.value == 0 && g_asked.instance_closes == 2 && !live("evitem01")
                && refused("answer", "evitem01", "no question is pending in evitem01"),
            "cancel: the callback with 0, closed by instance; the callback left set asks nothing once the window is"
            " gone");
        block("evitem01", true);
        seen();
        none = evitem_ask(2);
        check(!none && er.active && er.owner == evitem && er.owner_n == name("evitem") && er.row == 2
                && strcmp(seen(), "bevitem01 p+evitem01 ") == 0 && pending_now(kAskedEvitem) && blocked,
            "evitem01 blocked: the record keeps the key item list as the window that asked and the key item under its"
            " cursor (+0x30, 2)");
        wr32(controller("evitem"), kEvitemRow, 0);
        drain(g_e);
        check(!er.active && strcmp(seen(), "p-evitem01 ") == 0, "the list's cursor moving to another key item ends it");
        evitem_ask(0);
        seen();
        answer("evitem01", 1);
        check(!er.active && g_asked.value == 1 && g_asked.instance_closes == 2 && strcmp(seen(), "p-evitem01r ") == 0,
            "answered with no window: the record ended first, the callback with 1, no window to close");
        uint8_t* elsewhere = open_now("iuse");
        active(elsewhere);
        evitem_ask(0);
        check(!er.active && strcmp(seen(), "oiuse bevitem01 ") == 0,
            "asked with another window active than the key item list: nothing kept");
        block("evitem01", false);
        game_closes("iuse");
        game_closes("evitem");
        active(NULL);
        seen();

        // confyn: nothing waits on it
        uint8_t* confyn = controller("confyn");
        up = confyn_ask(0);
        check(up && pending_now(kAskedConfyn) && !blocked && rd16(up, kMenuCursor) == kRowYes,
            "confyn asked: pending while its window is up, the cursor on yes");
        memset(&g_asked, 0, sizeof(g_asked));
        answer("confyn", 1);
        const bool mode0 = g_asked.calls == 1 && g_asked.who == kAskedConfyn && g_asked.value == 1
            && g_asked.instance_closes == 1 && g_asked.instance_flag == 1 && !live("confyn");
        confyn_ask(1);
        answer("confyn", 1);
        const bool mode1 = g_asked.calls == 2 && g_asked.value == 2 && !live("confyn");
        confyn_ask(2);
        answer("confyn", 1);
        const bool other_mode = g_asked.calls == 2 && g_asked.instance_closes == 3 && !live("confyn");
        confyn_ask(0);
        answer("confyn", 0);
        check(mode0 && mode1 && other_mode && g_asked.calls == 2 && g_asked.instance_closes == 4 && !live("confyn"),
            "answer(true): the restore with mode + 1 in modes 0 and 1, none in another, then close by instance;"
            " answer(false) the close alone");
        confyn_ask(0);
        cancel("confyn");
        const bool no_cancel = refused("cancel", "confyn", "confyn has no cancel; answer('confyn', false) says no")
            && live("confyn");
        answer("confyn", 0);
        block("confyn", true);
        seen();
        confyn_ask(0);
        answer("confyn", 1);
        check(no_cancel && !live("confyn") && refused("answer", "confyn", "no question is pending in confyn")
                && g_asked.calls == 2 && rd32(confyn, kConfynMode) == 0,
            "cancel refused, its window left; blocked, nothing waits: no record, nothing pending, the answer refused");
        block("confyn", false);
        seen();

        // bankmenu: its own field says a question waits
        uint8_t* bankmenu = controller("bankmenu");
        g_player[kSafe2Flag] = 0;
        bank_setup();
        up = live("bankmenu");
        check(up && pending_now(kAskedBank) && !blocked && !g_e.bank_waiting && strcmp(seen(), "obankmenu ") == 0,
            "bankmenu asked: pending while its callback is set and its window up");
        answer("bankmenu", 9);
        const bool closed9 = refused("answer", "bankmenu", "Mog Safe 2 is not open to the player");
        answer("bankmenu", 3);
        const bool three = refused("answer", "bankmenu", "bankmenu offers the containers 1 and 9, not 3");
        g_player_ptr = NULL;
        answer("bankmenu", 9);
        const bool unknown = refused("answer", "bankmenu", "the player's data is not loaded");
        g_player_ptr = g_player;
        g_player[kSafe2Flag] = 1;
        answer("bankmenu", 9);
        check(closed9 && three && unknown && g_asked.who == kAskedBank && g_asked.value == 9 && g_asked.extra == 0
                && !live("bankmenu") && !rdptr(bankmenu, kBankCallback),
            "answer: 9 refused while Mog Safe 2 is closed or the player's data is not there, 3 refused; 9 once open"
            " the callback with 9 and 0, then the window closed, its begin-close clearing the callback");
        bank_setup();
        wr16(live("bankmenu"), kMenuCursor, 2);
        cancel("bankmenu");
        const bool second = g_asked.value == 9 && g_asked.extra == 1 && !live("bankmenu");
        bank_setup();
        cancel("bankmenu");
        check(second && g_asked.value == 1 && g_asked.extra == 1 && !live("bankmenu") && !rdptr(bankmenu, kBankCallback),
            "cancel: the callback with the container under the cursor and 1, as its cancel passes them");
        seen();
        block("bankmenu", true);
        const bool asked = bank_setup();
        drain(g_e);
        check(asked && !live("bankmenu") && rdptr(bankmenu, kBankCallback) && g_e.bank_waiting
                && pending_now(kAskedBank) && blocked && strcmp(seen(), "bbankmenu p+bankmenu ") == 0 && !bank_setup(),
            "bankmenu blocked: the callback its setup wrote stays with no window, the game's own sign the question"
            " waits, a pending event once the drain sees it; every later picker refuses while it waits");
        answer("bankmenu", 1);
        check(!rdptr(bankmenu, kBankCallback) && !g_e.bank_waiting && g_asked.value == 1 && g_asked.extra == 0
                && strcmp(seen(), "p-bankmenur ") == 0,
            "answered with no window: the callback with 1 and 0, then the clear its begin-close makes; its end the"
            " replying handle's");
        bank_setup();
        drain(g_e);
        seen();
        cancel("bankmenu");
        const bool canceled = g_asked.value == 1 && g_asked.extra == 1 && strcmp(seen(), "p-bankmenur ") == 0;
        bank_setup();
        drain(g_e);
        seen();
        block("bankmenu", false);
        const bool stays = g_e.bank_waiting && pending_now(kAskedBank) && blocked;
        answer("bankmenu", 1);
        seen();
        block("bankmenu", true);
        bank_setup();
        drain(g_e);
        seen();
        bank_clear(bankmenu, NULL);
        drain(g_e);
        check(canceled && stays && !g_e.bank_waiting && strcmp(seen(), "p-bankmenu ") == 0,
            "cancel with no window: 1 and 1; unblocked, the question still waits with no window; the game clearing"
            " its callback ends it");
        block("bankmenu", false);
        seen();

        // maplist: the rows the map window built
        uint8_t* maplist = controller("maplist");
        uint8_t* map0 = open_now("map0");
        const AskedRecord& mr = g_e.asked[kAskedMaplist];
        active(map0);
        seen();
        up = maplist_setup(100);
        check(up && pending_now(kAskedMaplist) && !blocked && map_count(kPlain, maplist) == 4 && map_top(maplist)
                && !mr.active,
            "maplist asked: pending with its callback, rows and window; showing the regions");
        answer("maplist", 5);
        const bool past = refused("answer", "maplist", "maplist has no row 5");
        clear_log();
        const int calls = g_asked.calls;
        answer("maplist", 1);
        check(past && g_asked.calls == calls + 1 && g_asked.who == kAskedMaplist && g_asked.value == 100
                && g_asked.extra == 0 && g_asked.context == g_map_entries[0] && !live("maplist")
                && !rdptr(maplist, kMapRows) && rdptr(maplist, kMapCallback) && strstr(g_log.order, "@R") != NULL,
            "answer: a row past the list refused; a map's row the callback with its entry {zone, map}, then"
            " close+reset, the callback left set");
        answer("maplist", 1);
        const bool freed = refused("answer", "maplist", "no question is pending in maplist");
        maplist_setup(100);
        answer("maplist", 4);
        const bool closing_row = g_asked.calls == calls + 1 && !live("maplist");
        maplist_setup(100);
        cancel("maplist");
        const bool top = g_asked.calls == calls + 1 && !live("maplist");
        check(freed && closing_row && top && !live("maplist") && g_asked.calls == calls + 1,
            "the rows freed ask nothing; the closing row's answer and the cancel at the top close it with no"
            " callback");

        // maplist's levels: an answer on a list row and a cancel below the
        // top step as its key handler does, each level a new prompt
        const int nmap = name("maplist");
        auto answer_id = [&](int32_t value, LONG id) {
            Command c = cmd(kOpAskedAnswer, "maplist", 0, 0, s1, gen1, static_cast<uint32_t>(value));
            c.verb = kVerbAnswer;
            c.flags = kCmdInstance;
            c.instance = static_cast<uint32_t>(id);
            run(c);
        };
        auto cancel_id = [&](LONG id) {
            Command c = cmd(kOpAskedCancel, "maplist", 0, 0, s1, gen1, 0);
            c.verb = kVerbCancel;
            c.flags = kCmdInstance;
            c.instance = static_cast<uint32_t>(id);
            run(c);
        };
        up = maplist_setup(100);
        wr16(up, kMenuCursor, 2);
        const LONG first_id = g_e.instance[nmap];
        memset(&g_map, 0, sizeof(g_map));
        clear_log();
        answer_id(2, first_id);
        const LONG region_id = g_e.instance[nmap];
        check(live("maplist") == up && rd16(maplist, kMapBack) == 0 && rd16(maplist, kMapRegion) == 2
                && rd16(maplist, kMapZone) == -1 && rd16(maplist, kMapSelection) == 1 && g_map.rebuilds == 1
                && g_map.selects == 1 && g_map.row == 0 && g_map.flag == 0 && strcmp(g_log.order, "uv") == 0
                && rdptr(maplist, kMapRows) == g_map_region_rows && map_count(kPlain, maplist) == 2
                && map_level(maplist) == kMapLevelRegion && !map_top(maplist) && region_id == first_id + 1
                && pending_now(kAskedMaplist) && !blocked && g_asked.calls == calls + 1,
            "answer on a region's row: the key handler's step down, no sound -- the list shown kept to go back to"
            " (+0x5E 0), -id and the entry's map written (+0x58 2, +0x5C -1), the row under the cursor kept (+0x5A"
            " 1), the rows built again, the cursor put on row 0; no callback, no close; a new prompt id");
        answer_id(1, first_id);
        const bool map_stale = refused("answer", "maplist", "that prompt is gone") && g_asked.calls == calls + 1
            && g_map.rebuilds == 1 && rdptr(maplist, kMapRows) == g_map_region_rows && live("maplist") == up;
        wr16(up, kMenuCursor, 2);
        answer_id(2, region_id);
        const LONG zone_id = g_e.instance[nmap];
        check(map_stale && rd16(maplist, kMapBack) == 2 && rd16(maplist, kMapRegion) == 192 && rd16(maplist, kMapZone) == 0
                && rd16(maplist, kMapSelection) == 1 && g_map.rebuilds == 2 && g_map.row == 0
                && rdptr(maplist, kMapRows) == g_map_zone_rows && map_level(maplist) == kMapLevelZone
                && zone_id == region_id + 1 && g_asked.calls == calls + 1 && pending_now(kAskedMaplist),
            "an answer naming the first level's prompt refused as gone, nothing applied to the region's rows; the"
            " region's row of a zone of several maps steps down again: the region kept (+0x5E 2), +0x58 192,"
            " +0x5C 0, its maps built; another new id");
        cancel_id(region_id);
        const bool stale_cancel = refused("cancel", "maplist", "that prompt is gone") && g_map.rebuilds == 2;
        clear_log();
        cancel_id(zone_id);
        const LONG back_id = g_e.instance[nmap];
        check(stale_cancel && rd16(maplist, kMapRegion) == 2 && rd16(maplist, kMapZone) == -1
                && rd16(maplist, kMapBack) == 2 && g_map.rebuilds == 3 && g_map.selects == 3 && g_map.row == 1
                && g_map.flag == 0 && strcmp(g_log.order, "uv") == 0 && rdptr(maplist, kMapRows) == g_map_region_rows
                && map_level(maplist) == kMapLevelRegion && back_id == zone_id + 1 && live("maplist") == up
                && g_asked.calls == calls + 1,
            "cancel inside a zone's maps: the key handler's step back -- +0x58 from +0x5E, +0x5C -1, the region's"
            " rows built again, the cursor put on the row kept (+0x5A 1); a new id; a cancel with an older id"
            " refused");
        clear_log();
        cancel_id(back_id);
        const LONG top_id = g_e.instance[nmap];
        check(rd16(maplist, kMapRegion) == 0 && rd16(maplist, kMapBack) == 0 && rd16(maplist, kMapZone) == -1
                && g_map.rebuilds == 4 && g_map.row == 1 && strcmp(g_log.order, "uv") == 0
                && rdptr(maplist, kMapRows) == g_map_rows && map_count(kPlain, maplist) == 4 && map_top(maplist)
                && top_id == back_id + 1 && live("maplist") == up && g_asked.calls == calls + 1,
            "cancel inside a region: back to the top -- +0x58 and +0x5E 0, the regions built again, the cursor"
            " put on the row kept; a new id");
        cancel_id(top_id);
        const bool closed = !live("maplist") && g_asked.calls == calls + 1 && g_map.rebuilds == 4;
        maplist_setup(100);
        answer("maplist", 2);
        clear_log();
        answer("maplist", 1);
        check(closed && g_asked.calls == calls + 2 && g_asked.value == 100 && g_asked.extra == 1
                && g_asked.context == g_map_level_entries[0] && !live("maplist") && strstr(g_log.order, "@R") != NULL,
            "cancel at the top closes it, no callback; a map's row inside a region: the callback with its entry"
            " {100, 1}, then close+reset");
        maplist_setup(100);
        g_player_ptr = NULL;
        answer("maplist", 2);
        const bool no_data = refused("answer", "maplist", "the player's data is not loaded") && map_top(maplist);
        g_player_ptr = g_player;
        answer("maplist", 2);
        g_player_ptr = NULL;
        cancel("maplist");
        const bool no_data_back = refused("cancel", "maplist", "the player's data is not loaded")
            && map_level(maplist) == kMapLevelRegion;
        g_player_ptr = g_player;
        const LONG before_keys = g_e.instance[nmap];
        wr16(maplist, kMapRegion, 0);
        maplist_rebuild(maplist, NULL);
        drain(g_e);
        const bool keys_step = g_e.instance[nmap] == before_keys + 1;
        drain(g_e);
        const bool once = g_e.instance[nmap] == before_keys + 1;
        wr16(maplist, kMapRegion, 2);
        maplist_rebuild(maplist, NULL);
        const LONG before_open = g_e.instance[nmap];
        maplist_setup(100);
        drain(g_e);
        check(no_data && no_data_back && keys_step && once && g_e.instance[nmap] == before_open + 1
                && g_asked.calls == calls + 2,
            "with the player's data not loaded a step is refused, as the key handler does nothing then; the"
            " player's own step between levels, window up, is a new prompt at the next drain, once; a new open"
            " that also changes the level counts once");
        g_e.holds.set(h1, nmap, kHoldHide, true);
        drain(g_e);
        answer("maplist", 2);
        const uint8_t* hidden_up = live("maplist");
        check(hidden_up && map_level(maplist) == kMapLevelRegion && hidden_prompt(g_e, hidden_up),
            "hidden, a region opened by an answer still asks: its rows hold the player's keys off it");
        cancel("maplist");
        cancel("maplist");
        g_e.holds.set(h1, nmap, kHoldHide, false);
        drain(g_e);
        seen();
        block("maplist", true);
        seen();
        none = maplist_setup(100);
        check(!none && mr.active && mr.owner == map0 && mr.context == g_map_rows && pending_now(kAskedMaplist) && blocked
                && strcmp(seen(), "bmaplist p+maplist ") == 0,
            "maplist blocked: the record keeps map0, the window that asked, and the rows the list was built with");
        const uint32_t blocked_id = mr.id;
        memset(&g_map, 0, sizeof(g_map));
        answer_id(2, static_cast<LONG>(blocked_id));
        const uint32_t blocked_region = mr.id;
        check(mr.active && mr.context == g_map_region_rows && mr.owner == map0 && blocked_region == blocked_id + 1
                && blocked_region == static_cast<uint32_t>(g_e.instance[nmap]) && rd16(maplist, kMapRegion) == 2
                && rd16(maplist, kMapSelection) == -1 && g_map.row == 0 && pending_now(kAskedMaplist) && blocked
                && strcmp(seen(), "p-maplistr p+maplist ") == 0,
            "blocked, a region's row steps down with no window (the cursor read -1); the record ended by this"
            " reply, then taken again for the region's rows under a new id and announced");
        answer_id(1, static_cast<LONG>(blocked_id));
        const bool blocked_stale = refused("answer", "maplist", "that prompt is gone") && mr.active;
        cancel_id(static_cast<LONG>(blocked_region));
        check(blocked_stale && mr.active && mr.context == g_map_rows && map_top(maplist)
                && mr.id == blocked_region + 1 && g_map.row == -1 && strcmp(seen(), "p-maplistr p+maplist ") == 0,
            "blocked: the first level's id refused after the step down; the cancel goes back to the top, a new"
            " prompt again");
        clear_log();
        answer("maplist", 3);
        check(!mr.active && g_asked.value == 101 && g_asked.extra == 2 && g_asked.context == g_map_entries[2]
                && !rdptr(maplist, kMapRows) && strstr(g_log.order, "@R") != NULL && strcmp(seen(), "p-maplistr ") == 0,
            "answered with no window: the record ended first, the callback with row 3's entry, then close+reset");
        maplist_setup(100);
        seen();
        uint8_t* again = g_map_rows_again;
        memcpy(maplist + kMapRows, &again, 4);
        drain(g_e);
        const bool rebuilt = !mr.active && strcmp(seen(), "p-maplist ") == 0;
        maplist_setup(100);
        seen();
        game_closes("map0");
        drain(g_e);
        check(rebuilt && !mr.active && strcmp(seen(), "cmap0 p-maplist ") == 0,
            "the list built again, or map0 closing, ends the record");
        block("maplist", false);
        active(NULL);
        maplist_close_reset(maplist, NULL);
        seen();

        // map0's close is its own cancel, as Escape runs it: closed by
        // instance, then the map's close on mapframe's controller with 0,
        // shown, hidden, or under the list; a block's close of it the same.
        // map0 not open, and every other map window, close by name alone
        {
            uint8_t* frame = give_controller("mapframe", vt_self);
            give_controller("mapv2", vt_self);
            uint8_t* frame_menu = open_now("mapframe");
            auto map_up = [&]() {
                open_now("map0");
                open_now("mapv2");
                seen();
                clear_log();
                memset(&g_map_close, 0, sizeof(g_map_close));
                memset(&g_asked, 0, sizeof(g_asked));
            };
            map_up();
            run(cmd(kOpClose, "map0", 0, 0, s1, gen1));
            check(g_asked.instance_closes == 1 && g_asked.instance_flag == 1 && g_map_close.calls == 1
                    && g_map_close.frame == frame && g_map_close.flag == 0 && !g_map_close.map0_open
                    && strcmp(g_log.order, "KMKKKKK") == 0
                    && strcmp(g_log.closes, "map0    |map0    |mapv3   |mapv2   |scanlist|mapscan |") == 0
                    && !live("map0") && !live("mapv2") && live("mapframe") == frame_menu
                    && strcmp(seen(), "cmap0 cmapv2 ") == 0,
                "close(map0): closed by instance with 1, then the map's close with 0 on mapframe's controller,"
                " which closes the map's windows by name; the frame left to its own fade-out");
            run(cmd(kOpClose, "map0", 0, 0, s1, gen1));
            const bool shut = g_map_close.calls == 1 && g_asked.instance_closes == 1
                && strcmp(g_log.last_close, "menu    map0    ") == 0 && strcmp(seen(), "") == 0;
            map_up();
            run(cmd(kOpClose, "mapv2", 0, 0, s1, gen1));
            run(cmd(kOpClose, "mapframe", 0, 0, s1, gen1));
            check(shut && g_map_close.calls == 0 && g_asked.instance_closes == 0
                    && strcmp(g_log.closes, "mapv2   |mapframe|") == 0 && live("map0") && !live("mapv2")
                    && !live("mapframe") && strcmp(seen(), "cmapv2 cmapframe ") == 0,
                "map0 not open: close by name, nothing else; mapv2 and mapframe close by name alone, the map's"
                " close not run");
            frame_menu = open_now("mapframe");
            run(cmd(kOpClose, "map0", 0, 0, s1, gen1));
            seen();
            map_up();
            g_e.holds.set(h1, name("map0"), kHoldHide, true);
            drain(g_e);
            run(cmd(kOpClose, "map0", 0, 0, s1, gen1));
            const bool hidden = g_map_close.calls == 1 && g_map_close.frame == frame && g_asked.instance_closes == 1
                && !live("map0") && strcmp(seen(), "cmap0 cmapv2 ") == 0;
            g_e.holds.set(h1, name("map0"), kHoldHide, false);
            drain(g_e);
            map_up();
            maplist_setup(100);
            seen();
            memset(&g_map_close, 0, sizeof(g_map_close));
            run(cmd(kOpClose, "map0", 0, 0, s1, gen1));
            const bool listed = g_map_close.calls == 1 && !live("map0") && live("maplist") && pending_now(kAskedMaplist)
                && strcmp(seen(), "cmap0 cmapv2 ") == 0;
            maplist_close_reset(maplist, NULL);
            seen();
            map_up();
            run(cmd(kOpBlockClose, "map0", 0, 0, s1, gen1));
            check(hidden && listed && g_map_close.calls == 1 && g_map_close.frame == frame && g_map_close.flag == 0
                    && g_asked.instance_closes == 1 && !live("map0") && strcmp(seen(), "cmap0 cmapv2 ") == 0,
                "hidden, under maplist (left up and asking, for the frame's own close to take down as the game"
                " leaves it), and as a block's close, map0's close is the same cancel; nothing refused");
            g_ctl_global[first_row("mapframe")] = NULL;
            map_up();
            run(cmd(kOpClose, "map0", 0, 0, s1, gen1));
            check(g_map_close.calls == 0 && g_asked.instance_closes == 1 && !live("map0") && live("mapv2")
                    && strcmp(seen(), "cmap0 cmapframe ") == 0,
                "no controller in mapframe's slot: map0 closed by instance, the map's close not called; mapframe,"
                " which nothing finds open with no controller, closed as the drain reads it");
            g_ctl_global[first_row("mapframe")] = frame;
            game_closes("mapv2");
            close_menu(frame_menu);
            check(!live("mapframe") && !live("mapv2") && strcmp(seen(), "cmapv2 ") == 0,
                "the map's windows closed by the game, mapframe's closed already posted");
            g_ctl_global[first_row("mapframe")] = NULL;
            g_ctl_global[first_row("mapv2")] = NULL;
        }

        // gmtell, hide-only: the result word its poller reads
        uint8_t* gm = controller("gmtell");
        up = gmtell_ask(1);
        check(up && pending_now(kAskedGmtell) && !blocked, "gmtell asked: pending while its result word is 0");
        reply(kOpAskedAnswer, "gmtell", 5, kCmdListed);
        const bool unlisted = refused("answer", "gmtell",
            "no option in the list has the value 5 (options('gmtell') lists them)");
        reply(kOpAskedAnswer, "gmtell", 2, kCmdListed);
        check(unlisted && rd16(gm, kQueryResult) == 2 && rd16(gm, kQueryCursor) == 1 && live("gmtell") == up
                && !pending_now(kAskedGmtell),
            "answer: a value no option has refused; an option's, its cursor and the result word written, the poller's"
            " to send and close; nothing pending once written");
        game_closes("gmtell");
        gmtell_ask(1);
        cancel("gmtell");
        check(static_cast<uint16_t>(rd16(gm, kQueryResult)) == kQueryCanceled && !pending_now(kAskedGmtell),
            "cancel: 0xFF in the result word, the decline its own cancel writes");
        game_closes("gmtell");
        seen();

        // mp_dead, hide-only: its choice at +0x24
        uint8_t* mp = controller("mp_dead");
        up = mp_dead_open(30);
        check(up && pending_now(kAskedMpDead) && !blocked, "mp_dead open: pending while its choice is 0");
        answer("mp_dead", 3);
        const bool range = refused("answer", "mp_dead", "mp_dead takes 1 or 2, not 3");
        const uint32_t state = rd32(up, kMenuState);
        wr32(up, kMenuState, kStateDormant);
        answer("mp_dead", 1);
        const bool under = refused("answer", "mp_dead", "mp_dead is under another window");
        wr32(up, kMenuState, state);
        wr16(up, kMenuInputWait, 5);
        answer("mp_dead", 1);
        const bool waits = refused("answer", "mp_dead", "mp_dead takes no input yet");
        wr16(up, kMenuInputWait, 0);
        up[kMenuBusy] = 1;
        answer("mp_dead", 1);
        const bool busy = refused("answer", "mp_dead", "mp_dead is busy");
        up[kMenuBusy] = 0;
        cancel("mp_dead");
        const bool no_escape = refused("cancel", "mp_dead", "mp_dead has no cancel");
        answer("mp_dead", 2);
        check(range && under && waits && busy && no_escape && rd32(mp, kMpDeadChoice) == 2 && live("mp_dead") == up
                && !pending_now(kAskedMpDead),
            "answer: 3 refused, and while dormant, waiting for input or busy, as its input routine would take"
            " nothing; cancel refused; 2 written at +0x24, the window left to the KO handler");
        answer("mp_dead", 1);
        check(refused("answer", "mp_dead", "no question is pending in mp_dead") && rd32(mp, kMpDeadChoice) == 2,
            "a choice made: nothing pending, a second answer refused");
        game_closes("mp_dead");
        seen();

        // ok: the error screen's notice
        uint8_t* okc = controller("ok");
        const AskedRecord& orec = g_e.asked[kAskedOk];
        int context = 0;
        up = ok_notice(true, &context, false);
        check(up && pending_now(kAskedOk) && !blocked && !orec.active, "ok up: pending until acknowledged");
        run(cmd(kOpClose, "ok", 0, 0, s1, gen1));
        run(cmd(kOpBlockClose, "ok", 0, 0, s1, gen1));
        check(live("ok") == up && menu_is_open(up) && pending_now(kAskedOk) && rd32(okc, kOkAnswered) == 0,
            "the drain closes no ok, by close or by a block's close: it stays up, unacknowledged");
        memset(&g_asked, 0, sizeof(g_asked));
        cancel("ok");
        const bool ok_cancel = refused("cancel", "ok", "ok has no cancel; answer('ok') acknowledges it");
        answer("ok", 0);
        check(ok_cancel && up[kMenuBusy] == 1 && rd32(okc, kOkAnswered) == 1 && g_asked.calls == 1
                && g_asked.who == kAskedOk && g_asked.context == &context && g_asked.instance_closes == 1 && !live("ok"),
            "answer(): busy, +0x25C 1, the callback with its context, then close by instance, as its confirm does;"
            " cancel refused");
        up = ok_notice(false, NULL, true);
        answer("ok", 0);
        const bool closes = g_asked.calls == 1 && g_asked.instance_closes == 2 && !live("ok");
        up = ok_notice(false, NULL, false);
        answer("ok", 0);
        const bool waits_close = rd32(okc, kOkAnswered) == 1 && live("ok") == up && g_asked.instance_closes == 2
            && !pending_now(kAskedOk);
        answer("ok", 0);
        check(closes && waits_close && refused("answer", "ok", "no question is pending in ok"),
            "no callback: closed when +0x258 says so, else acknowledged alone, the error screen's to close; then"
            " nothing pending");
        game_closes("ok");
        seen();
        block("ok", true);
        seen();
        ok_notice(true, &context, false);
        check(orec.active && orec.owner_n == -1 && orec.callback == reinterpret_cast<const void*>(&ok_answered)
                && orec.context == &context && pending_now(kAskedOk) && blocked && strcmp(seen(), "bok p+ok ") == 0,
            "ok blocked: the notice kept with no window that asked");
        answer("ok", 0);
        check(!orec.active && rd32(okc, kOkAnswered) == 1 && g_asked.calls == 2 && g_asked.instance_closes == 3
                && strcmp(seen(), "p-okr ") == 0,
            "answered with no window: the record ended, +0x25C 1, the callback, and close by instance given no"
            " window, which the game's takes");
        ok_notice(false, NULL, false);
        seen();
        wr32(okc, kOkAnswered, 1);
        drain(g_e);
        check(!orec.active && strcmp(seen(), "p-ok ") == 0, "the notice acknowledged by the game ends the record");
        block("ok", false);
        seen();
        clear_log();

        // mogdoor, the Mog House's door: the door's handler writes the nation
        // and +0x1B, then opens the window, whose open hook puts the nation's
        // list at +0x1C, its count at +0x16 and the top up
        uint8_t* door = controller("mogdoor");
        const int door_n = name("mogdoor");
        memset(&g_door, 0, sizeof(g_door));
        g_door.gate_answer = 0xABCD0001u;
        g_door.garden_on = 1;
        g_door.floor_on = 1;
        wr16(g_player, kFloorWord, 0);
        auto door_list = [&](int nation) {
            const uint16_t* list = g_door_lists[nation];
            int count = 0;
            while (list && list[count]) {
                ++count;
            }
            memcpy(door + kDoorList, &list, 4);
            door[kDoorCount] = static_cast<uint8_t>(count);
            door[kDoorLevel] = kDoorTop;
            door[kDoorGarden] = g_door.garden_on;
            door[kDoorFloor] = g_door.floor_on;
        };
        auto door_use = [&](int nation, int kept, bool hook) {
            door[kDoorNation] = static_cast<uint8_t>(nation);
            door[kDoorKept] = static_cast<uint8_t>(kept);
            if (hook) {
                door_list(nation);
            }
            return static_cast<uint8_t*>(hooked_open(g_mcb, NULL, "menu    mogdoor ", 1, 0));
        };
        auto door_reset = [&]() {
            memset(g_door_exiting, 0, sizeof(g_door_exiting));
            memset(g_door_actor, 0, sizeof(g_door_actor));
            door[kDoorExit] = 0xEE;
            g_door.gates = 0;
            g_door.opens = 0;
            g_asked.instance_closes = 0;
            clear_log();
        };
        auto exited = [&](int exit) {
            const void* opened;
            memcpy(&opened, g_door_actor + kDoorOpened, 4);
            return g_door.gates == 1 && g_door.gate_args[0] == 4 && g_door.gate_args[1] == 0 && g_door.opens == 1
                && g_door.open_actor == g_door_actor && g_door.open_arg == 0 && g_door.exit_at_open == exit
                && door[kDoorExit] == exit && opened == g_door_opened_code && g_door_exiting[0] == 1
                && g_door_exiting[1] == 1 && g_door_exiting[2] == 1 && strcmp(g_log.order, "GO") == 0;
        };
        door_reset();
        uint8_t* door_menu = door_use(1, 0, true);
        drain(g_e);
        DoorTexts texts;
        g_e.door_texts.read(&texts);
        check(door_menu && pending_now(kAskedDoor) && !blocked && texts.ready
                && texts.id == static_cast<uint32_t>(g_e.instance[door_n]) && strcmp(texts.title, "Text 69") == 0
                && strcmp(texts.rows[0], "Text 184") == 0 && strcmp(texts.rows[1], "Text 241") == 0
                && strcmp(texts.rows[2], "Text 293") == 0 && strcmp(texts.rows[3], "Text 185") == 0
                && strcmp(texts.areas[0], "Area 230") == 0 && strcmp(texts.areas[3], "Area 280") == 0
                && texts.areas[4][0] == '\0' && strcmp(seen(), "omogdoor ") == 0,
            "mogdoor open: pending; the draw's texts read for this open with the game's getter: the title, the"
            " stay, back (the draw's condition 0: 0xF1), floor and list rows, each area's name");
        g_door.condition = 0xABCD0001u;
        drain(g_e);
        g_e.door_texts.read(&texts);
        check(strcmp(texts.rows[1], "Text 70") == 0, "the back row's text follows the draw's condition, al alone");
        {
            uint16_t zones[kDoorListMax];
            const AskedNow now = asked_now(g_e, kPlain, kAskedDoor, g_e.asked[kAskedDoor]);
            const int count = door_areas(g_e, kPlain, door, true, 0, zones);
            char buf[kErrorReason];
            check(count == 4 && zones[0] == 230 && zones[3] == 280 && door_area_exit(g_e, zones[2], 2) == 3
                    && door_area_exit(g_e, zones[3], 3) == kDoorGardenExit && door_floor_exit(now) == kDoorFloorUp
                    && !door_offer_refusal(g_e, now, true, 0, 0, buf, sizeof(buf))
                    && !door_offer_refusal(g_e, now, true, 0, 2, buf, sizeof(buf))
                    && !door_offer_refusal(g_e, now, true, 0, 127, buf, sizeof(buf))
                    && door_offer_refusal(g_e, now, true, 0, 4, buf, sizeof(buf))
                    && strcmp(buf, "mogdoor offers no exit 4 (options('mogdoor') lists them)") == 0,
                "mogdoor's offers: the list's places, the Mog Garden's 127 in place of its own, back 0, the floor"
                " 125 on the first floor");
        }
        answer("mogdoor", kDoorBack);
        check(exited(kDoorBack) && open_now("mogdoor") == door_menu && !pending_now(kAskedDoor),
            "answer('mogdoor', 0): the confirm's gate (4, 0), 0 at +0x15, the door actor given the routine that"
            " sends it, its door opened, the three bytes set; the window stays up, asking nothing more");
        door_reset();
        answer("mogdoor", 3);
        check(exited(3), "an area of the list: its place at +0x15, then the exit");
        door_reset();
        answer("mogdoor", kDoorGardenExit);
        check(exited(kDoorGardenExit), "the Mog Garden's row, enabled: 0x7F at +0x15");
        door_reset();
        door[kDoorGarden] = 0;
        answer("mogdoor", kDoorGardenExit);
        const bool garden_off = refused("answer", "mogdoor", "the Mog Garden is not open to the player now");
        door[kDoorGarden] = 1;
        answer("mogdoor", 4);
        const bool no_place = refused("answer", "mogdoor", "mogdoor offers no exit 4");
        check(garden_off && no_place && g_door.gates == 0 && door[kDoorExit] == 0xEE,
            "the Mog Garden's row turned off, and its own place, refused before the gate");
        answer("mogdoor", kDoorFloorUp);
        check(exited(kDoorFloorUp) && g_door.floors == 1, "the floor row: 0x7D on the first floor, as the floor"
            " read the confirm calls picks it");
        door_reset();
        answer("mogdoor", kDoorFloorUp + 1);
        const bool other_floor = refused("answer", "mogdoor", "the player is on the other floor: mogdoor's floor row"
            " is 125");
        door[kDoorFloor] = 0;
        answer("mogdoor", kDoorFloorUp);
        const bool floor_off = refused("answer", "mogdoor", "mogdoor's floor row is turned off now");
        door[kDoorFloor] = 1;
        g_door.floor = 1;
        answer("mogdoor", kDoorFloorUp);
        const bool moved = refused("answer", "mogdoor", "the player is on the other floor: mogdoor's floor row is"
            " 126");
        g_door.floor = 0;
        check(other_floor && floor_off && moved && g_door.gates == 1 && g_door.opens == 0 && door[kDoorExit] == 0xEE,
            "the floor row refused for the other floor's value and while turned off; the game's own floor read"
            " picking the other floor, past the gate, refuses it too");
        door_reset();
        g_door.gate_answer = 0xABCD0000u;
        answer("mogdoor", kDoorBack);
        const bool gated = refused("answer", "mogdoor", "the game refuses an exit now; its own message says why");
        g_door.gate_answer = 0xABCD0001u;
        check(gated && g_door.gates == 1 && g_door.opens == 0 && door[kDoorExit] == 0xEE && !g_door_exiting[0],
            "the gate answering al 0: nothing written, nothing opened");
        wr32(door_menu, kMenuState, kStateDormant);
        answer("mogdoor", kDoorBack);
        const bool door_under = refused("answer", "mogdoor", "mogdoor is under another window");
        cancel("mogdoor");
        const bool under_cancel = refused("cancel", "mogdoor", "mogdoor is under another window");
        wr32(door_menu, kMenuState, 0);
        door_menu[kMenuBusy] = 1;
        answer("mogdoor", kDoorBack);
        const bool door_busy = refused("answer", "mogdoor", "mogdoor is busy");
        door_menu[kMenuBusy] = 0;
        check(door_under && under_cancel && door_busy && g_door.gates == 1, "refused while its window is under another or"
            " busy, as its input sink would");
        door_reset();
        door[kDoorLevel] = kDoorAreas;
        g_door.gate_answer = 0xABCD0000u;
        cancel("mogdoor");
        g_door.gate_answer = 0xABCD0001u;
        check(g_door.gates == 0 && door[kDoorExit] == 0xEE && g_door.opens == 0 && g_asked.instance_closes == 1
                && !live("mogdoor") && !g_door_exiting[0] && strcmp(seen(), "cmogdoor ") == 0,
            "cancel('mogdoor'), the window at its list of areas and the gate refusing: the top level's Escape, the"
            " window closed by instance with no gate run and nothing written at +0x15, no exit");
        door_reset();
        door_menu = door_use(1, 0, true);
        g_door_exiting[0] = 1;
        drain(g_e);
        check(door_menu && !pending_now(kAskedDoor), "an exit under way: mogdoor asks nothing");
        game_closes("mogdoor");
        seen();

        // blocked: the door's handler wrote the nation; the open hook never ran
        door_reset();
        block("mogdoor", true);
        seen();
        door_list(5);
        const int ticks = g_door.ticks;
        door_use(1, 2, false);
        const AskedRecord& drec = g_e.asked[kAskedDoor];
        g_e.door_texts.read(&texts);
        check(drec.active && drec.row == (1 | 2 << 8) && drec.callback == g_house && drec.context == g_door_actor
                && drec.id == static_cast<uint32_t>(g_e.instance[door_n]) && g_door.ticks == ticks + 1
                && texts.id == drec.id && strcmp(texts.areas[0], "Area 230") == 0 && strcmp(texts.areas[3],
                    "Area 280") == 0 && pending_now(kAskedDoor) && blocked
                && strcmp(seen(), "bmogdoor p+mogdoor ") == 0,
            "blocked: the refused open records the nation, +0x1B, the Mog House and its door, runs the window's tick"
            " for its rows' enables, reads the texts of the nation's list from the table, not the stale one, and"
            " announces it");
        drain(g_e);
        check(g_door.ticks == ticks + 2 && drec.active, "while it stands the drain runs the tick each frame");
        {
            const AskedNow now = asked_now(g_e, kPlain, kAskedDoor, g_e.asked[kAskedDoor]);
            char buf[kErrorReason];
            check(!door_offer_refusal(g_e, now, false, 1, 2, buf, sizeof(buf))
                    && door_offer_refusal(g_e, now, false, 1, 4, buf, sizeof(buf)),
                "blocked, the offers are the nation's list's from the table");
        }
        answer("mogdoor", 2);
        check(!drec.active && exited(2) && g_asked.instance_closes == 0 && strcmp(seen(), "p-mogdoorr ") == 0,
            "answered with no window: the record ended, the replying handle named, then the exit; nothing to"
            " close");
        door_reset();
        door_use(1, 0, false);
        seen();
        g_door.gate_answer = 0xABCD0000u;
        cancel("mogdoor");
        g_door.gate_answer = 0xABCD0001u;
        check(!drec.active && door[kDoorExit] == 0xEE && g_door.gates == 0 && g_door.opens == 0
                && g_asked.instance_closes == 0 && strcmp(seen(), "p-mogdoorr ") == 0,
            "blocked, a cancel ends the record with no gate run, the gate refusing or not, and nothing written at"
            " +0x15; nothing to close");
        door_use(1, 0, false);
        seen();
        uint8_t other_house[0x40];
        memcpy(other_house, g_house, sizeof(other_house));
        g_house_ptr = other_house;
        drain(g_e);
        const bool left = !drec.active && strcmp(seen(), "p-mogdoor ") == 0;
        g_house_ptr = g_house;
        door_use(1, 0, false);
        seen();
        door[kDoorNation] = 3;
        drain(g_e);
        const bool used_again = !drec.active && strcmp(seen(), "p-mogdoor ") == 0;
        door_use(1, 0, false);
        seen();
        g_door_exiting[0] = 1;
        drain(g_e);
        const bool leaving = !drec.active && strcmp(seen(), "p-mogdoor ") == 0;
        g_door_exiting[0] = 0;
        door_use(1, 0, false);
        seen();
        door_use(9, 0, false);
        const bool afresh = drec.active && drec.row == 9 && strcmp(seen(), "p-mogdoor bmogdoor p+mogdoor ") == 0;
        block("mogdoor", false);
        door_use(1, 0, true);
        const bool window = !drec.active && strcmp(seen(), "p-mogdoor omogdoor ") == 0;
        check(left && used_again && leaving && afresh && window, "the record ends as the player leaves the Mog"
            " House, as the door's handler writes another nation, as an exit gets under way, and at the window's"
            " open; a refused open records afresh");
        game_closes("mogdoor");
        seen();
        door_reset();

        // the NPC dialogue boxes: each its own text object's message, its page
        // the last lines of its list
        {
            auto put_line = [&](int d, int i, const char* text) {
                uint8_t* l = g_lines[d][i];
                memset(l, 0, kLineBytes);
                wr16(l, kLineCount, put_text(reinterpret_cast<int16_t*>(l + kLineGlyphCodes), 0, text));
            };
            auto lines = [&](int d, const char* const* texts, int total, int page) {
                uint8_t* ctl = controller(kDialogueNames[d]);
                for (int i = 0; i < total; ++i) {
                    put_line(d, i, texts[i]);
                    uint8_t* node = g_nodes[d][i];
                    memset(node, 0, 0x18);
                    const uint8_t* next = i + 1 < total ? g_nodes[d][i + 1] : NULL;
                    const uint8_t* prev = i > 0 ? g_nodes[d][i - 1] : NULL;
                    const uint8_t* item = g_lines[d][i];
                    memcpy(node + kNodeNext, &next, 4);
                    memcpy(node + kNodePrev, &prev, 4);
                    memcpy(node + kNodeItem, &item, 4);
                }
                const uint8_t* head = total ? g_nodes[d][0] : NULL;
                const uint8_t* tail = total ? g_nodes[d][total - 1] : NULL;
                memcpy(ctl + kBoxLines, &head, 4);
                memcpy(ctl + kBoxLinesTail, &tail, 4);
                wr32(ctl, kBoxLineCount, static_cast<uint32_t>(total));
                wr16(ctl, kBoxPageLines, page);
            };
            auto show = [&](int d, uint8_t* message, int next, uint32_t flags, int held) {
                uint8_t* text = g_texts[d];
                memcpy(text + kTextCurrent, &message, 4);
                if (message) {
                    const uint8_t* at = next < 0 ? NULL : message + kMessageText + next;
                    memcpy(message + kMessageNext, &at, 4);
                }
                wr32(text, kTextFlags, flags);
                wr32(text, kTextQueued, static_cast<uint32_t>(held));
            };
            uint8_t* talk1 = controller("rem4line");
            uint8_t* talk2 = controller("rem4li2");
            uint8_t* box1 = open_now("rem4line");
            uint8_t* box2 = open_now("rem4li2");
            seen();
            static const char* const said[] = {"Old line", "Rolanberry : Hello", "there."};
            memset(g_messages, 0, sizeof(g_messages));
            strcpy(reinterpret_cast<char*>(g_messages[0] + kMessageText), "Rolanberry : Hello there.\x7F\x31" "Bye.");
            memcpy(g_messages[3], g_messages[0], kMessageBytes);
            for (int d = 0; d < kDialogueCount; ++d) {
                lines(d, said, 3, 2);
                show(d, d == 0 ? g_messages[3] : g_messages[0], 27, kPageKey, 1);
            }
            drain(g_e);
            const bool first = strcmp(seen(), "grem4line grem4li2 ") == 0;
            drain(g_e);
            const bool still = strcmp(seen(), "") == 0;
            show(1, g_messages[0], -1, kPageKey, 1);
            drain(g_e);
            const bool next_page = strcmp(seen(), "grem4li2 ") == 0;
            show(1, g_messages[1], -1, kPageKey, 2);
            drain(g_e);
            show(1, g_messages[2], -1, kPageKey, 1);
            drain(g_e);
            const bool queued = strcmp(seen(), "grem4li2 grem4li2 ") == 0;
            show(1, NULL, -1, 0, 0);
            drain(g_e);
            const bool ended = strcmp(seen(), "") == 0;
            show(1, g_messages[2], -1, kPageKey, 1);
            drain(g_e);
            show(1, NULL, -1, 0, 0);
            game_closes("rem4li2");
            box2 = open_now("rem4li2");
            show(1, g_messages[2], -1, kPageKey, 1);
            drain(g_e);
            const bool alike = strcmp(seen(), "grem4li2 crem4li2 orem4li2 grem4li2 ") == 0;
            check(first && still && next_page && queued && ended && alike,
                "page events: each box's first page, none while nothing changes, the next page of a message, a"
                " message replacing another with no close (the count held changed), none as the message ends,"
                " and a message alike to the one before at the same address through a close and an open");

            run(cmd(kOpClose, "rem4line", 0, 0, s1, gen1));
            run(cmd(kOpBlockClose, "rem4line", 0, 0, s1, gen1));
            check(live("rem4line") == box1 && menu_is_open(box1), "the drain closes no dialogue box, by close or by"
                " block");

            // one page on through the gate's advance and sound, no code handed
            // to the text dispatch; the box's own cancel
            clear_log();
            memset(&g_talk, 0, sizeof(g_talk));
            auto talk = [&](uint8_t op, const char* nm) {
                Command c = cmd(op, nm, 0, 0, s1, gen1);
                c.verb = op == kOpDialogueCancel ? kVerbCancel : kVerbAnswer;
                run(c);
            };
            talk(kOpDialogueAnswer, "rem4li2");
            check(g_talk.dispatches == 0 && g_talk.advances == 2 && g_talk.advance_text[0] == g_texts[1]
                    && g_talk.advance_text[1] == g_texts[0] && g_talk.sounds == 2
                    && strcmp(g_log.order, "VpVp") == 0,
                "answer('rem4li2'): no code handed to the text dispatch; rem4li2's own text object advanced, then the"
                " page-turn sound, then rem4line's, whose page waits for a key too, and the sound again; nothing"
                " else");
            show(0, g_messages[3], 27, kPageTimed, 1);
            clear_log();
            memset(&g_talk, 0, sizeof(g_talk));
            talk(kOpDialogueAnswer, "rem4li2");
            check(g_talk.dispatches == 0 && g_talk.advances == 1 && g_talk.advance_text[0] == g_texts[1]
                    && g_talk.sounds == 1 && strcmp(g_log.order, "Vp") == 0,
                "answer('rem4li2') while rem4line's page goes on by its timer: rem4li2's own text object advanced"
                " and the sound once; rem4line's left to its timer");
            show(0, g_messages[3], 27, kPageKey, 1);
            memset(&g_talk, 0, sizeof(g_talk));
            show(1, g_messages[2], -1, kPageTimed, 1);
            talk(kOpDialogueAnswer, "rem4li2");
            const bool timed = refused("answer", "rem4li2", "the page rem4li2 shows does not wait for a key: it goes"
                " on by itself");
            show(1, NULL, -1, 0, 0);
            talk(kOpDialogueAnswer, "rem4li2");
            const bool none = refused("answer", "rem4li2", "no dialogue is shown in rem4li2");
            show(1, g_messages[2], -1, kPageKey | kPageTimed, 1);
            g_query_cancel_allowed = 0;
            talk(kOpDialogueCancel, "rem4li2");
            const bool no_cancel = refused("cancel", "rem4li2", "rem4li2 cannot be canceled now: the event allows no"
                " cancel");
            check(timed && none && no_cancel && g_talk.advances == 0 && g_talk.sounds == 0 && g_talk.dispatches == 0
                    && g_talk.releases == 0,
                "refused: a page that goes on by itself, no message shown, a cancel the event allows not");
            g_query_cancel_allowed = 1;

            // either name's answer is the one dialogue's, the gate run on the
            // twin's text object too; both names' prompt is the pair's count
            // of opens, and an id either box's own count gives, or the pair's
            // before, is gone
            const uint32_t pair = static_cast<uint32_t>(g_e.dialogue_opens);
            const uint32_t own1 = static_cast<uint32_t>(g_e.instance[name("rem4line")]);
            const uint32_t own2 = static_cast<uint32_t>(g_e.instance[name("rem4li2")]);
            memset(&g_talk, 0, sizeof(g_talk));
            Command named = cmd(kOpDialogueAnswer, "rem4line", 0, 0, s1, gen1);
            named.verb = kVerbAnswer;
            named.flags = kCmdInstance;
            named.instance = pair;
            run(named);
            const bool line_pair = g_talk.advances == 2 && g_talk.advance_text[0] == g_texts[0]
                && g_talk.advance_text[1] == g_texts[1] && g_talk.sounds == 2;
            memset(&g_talk, 0, sizeof(g_talk));
            named.target = static_cast<int16_t>(name("rem4li2"));
            run(named);
            const bool li2_pair = g_talk.advances == 2 && g_talk.advance_text[0] == g_texts[1]
                && g_talk.advance_text[1] == g_texts[0] && g_talk.sounds == 2;
            bool gone = true;
            const uint32_t stale[3] = {own1, own2, pair - 1};
            for (int i = 0; i < 3; ++i) {
                for (int d = 0; d < kDialogueCount; ++d) {
                    named.target = static_cast<int16_t>(name(kDialogueNames[d]));
                    named.instance = stale[i];
                    run(named);
                    gone = gone && refused("answer", kDialogueNames[d], "that prompt is gone");
                }
            }
            check(own1 != pair && own2 != pair && open_count(g_e, name("rem4line")) == pair
                    && open_count(g_e, name("rem4li2")) == pair && line_pair && li2_pair && gone
                    && g_talk.advances == 2 && g_talk.dispatches == 0 && open_count(g_e, name("query"))
                        == static_cast<uint32_t>(g_e.instance[name("query")]),
                "answer('rem4line') and answer('rem4li2') under the pair's id: the named box's own text object"
                " advanced, then its twin's, each with the sound, no code handed to the text dispatch; under either"
                " box's own count of opens or the pair's before: that prompt is gone; any other window's prompt its"
                " own count");

            // a cancel each box's own: refused whole while the twin shows a
            // message it could not cancel; else the named box's, then the
            // twin's while that shows a message
            wr32(talk1, kBoxLogLine, 0x44);
            wr32(talk2, kBoxLogLine, 0x55);
            wr32(g_log_ctls[0], kLogObject, 0);
            memset(&g_talk, 0, sizeof(g_talk));
            clear_log();
            talk(kOpDialogueCancel, "rem4li2");
            const bool twin_refused = refused("cancel", "rem4li2", "rem4line has no chat log to hand its lines back to")
                && strcmp(g_log.order, "") == 0 && live("rem4line") == box1 && live("rem4li2")
                && rdptr(g_texts[0], kTextCurrent) == g_messages[3] && rdptr(g_texts[1], kTextCurrent) == g_messages[2];
            uint8_t* log0 = g_log_objects[0];
            memcpy(g_log_ctls[0] + kLogObject, &log0, 4);
            check(twin_refused, "cancel('rem4li2') while rem4line shows a message with no chat log to hand its lines"
                " back to: refused whole, neither box touched");
            g_wait = 1;
            g_asked.instance_closes = 0;
            memset(&g_talk, 0, sizeof(g_talk));
            clear_log();
            talk(kOpDialogueCancel, "rem4li2");
            check(strcmp(g_log.order, "RXKRXK") == 0 && g_talk.release_log[0] == g_log_objects[1]
                    && g_talk.release_line[0] == 0x55 && g_talk.clear_text[0] == g_texts[1]
                    && g_talk.wait_at_clear[0] == 1 && g_talk.release_log[1] == g_log_objects[0]
                    && g_talk.release_line[1] == 0x44 && g_talk.clear_text[1] == g_texts[0]
                    && g_talk.wait_at_clear[1] == kWaitAborted && g_wait == kWaitAborted
                    && g_asked.instance_closes == 2 && g_asked.instance_flag == 1 && !live("rem4li2")
                    && !live("rem4line") && !rdptr(g_texts[0], kTextCurrent),
                "cancel('rem4li2'): its lines handed back to its chat log with +0x50, its own text object cleared,"
                " then the event's wait told 2, then the box closed by instance; then rem4line's own cancel the same"
                " way, its clear after the wait was told 2, which it tells again: neither box keeps the message");
            seen();
            const LONG pair_before = g_e.dialogue_opens;
            const LONG li2_before = g_e.instance[name("rem4li2")];
            box1 = open_now("rem4line");
            const bool line_moves = g_e.dialogue_opens == pair_before + 1
                && g_e.instance[name("rem4li2")] == li2_before
                && open_count(g_e, name("rem4li2")) == static_cast<uint32_t>(pair_before + 1);
            box2 = open_now("rem4li2");
            const bool li2_moves = g_e.dialogue_opens == pair_before + 2
                && open_count(g_e, name("rem4line")) == static_cast<uint32_t>(pair_before + 2);
            check(line_moves && li2_moves, "the pair's count moves on either box's open, each box's own on its own"
                " opens alone; both names' prompt is the pair's count");
            show(0, g_messages[3], -1, kPageKey, 1);
            show(1, g_messages[2], -1, kPageKey, 1);
            g_wait = 1;
            g_asked.instance_closes = 0;
            memset(&g_talk, 0, sizeof(g_talk));
            clear_log();
            talk(kOpDialogueCancel, "rem4line");
            const bool through_line = strcmp(g_log.order, "RXKRXK") == 0 && g_talk.release_log[0] == g_log_objects[0]
                && g_talk.release_line[0] == 0x44 && g_talk.clear_text[0] == g_texts[0]
                && g_talk.release_log[1] == g_log_objects[1] && g_talk.release_line[1] == 0x55
                && g_talk.clear_text[1] == g_texts[1] && g_wait == kWaitAborted && g_asked.instance_closes == 2
                && !live("rem4line") && !live("rem4li2");
            box1 = open_now("rem4line");
            box2 = open_now("rem4li2");
            show(0, NULL, -1, 0, 0);
            show(1, g_messages[2], -1, kPageKey, 1);
            g_asked.instance_closes = 0;
            memset(&g_talk, 0, sizeof(g_talk));
            clear_log();
            talk(kOpDialogueCancel, "rem4li2");
            const bool alone = strcmp(g_log.order, "RXK") == 0 && g_talk.clear_text[0] == g_texts[1]
                && g_asked.instance_closes == 1 && !live("rem4li2") && live("rem4line") == box1;
            check(through_line && alone, "cancel('rem4line'): rem4line's own cancel, then rem4li2's; with rem4line"
                " showing no message, cancel('rem4li2') makes rem4li2's alone and leaves rem4line open");
            show(0, g_messages[3], -1, kPageKey, 1);
            seen();

            // the left button's test: answered no only from the tick's call
            // with a hidden dialogue box active that shows a message; with
            // none shown the box asks nothing and the click and keys reach it
            box2 = open_now("rem4li2");
            show(1, g_messages[2], -1, kPageKey, 1);
            auto clicked = [&](uintptr_t from, const uint8_t* menu) {
                active(menu);
                HuFrame f;
                memset(&f, 0, sizeof(f));
                f.return_address = from;
                f.result = 0xDEAD;
                f.user = &g_e;
                const int skipped = hook_key_test_pre(&f);
                active(NULL);
                return skipped ? static_cast<int>(f.result) : -1;
            };
            const uintptr_t tick = g_e.game.dialogue.click_return;
            const bool shown = clicked(tick, box2) == -1;
            g_e.holds.set(h1, name("rem4li2"), kHoldHide, true);
            drain(g_e);
            const bool hidden = clicked(tick, box2) == 0 && clicked(tick + 1, box2) == -1 && clicked(tick, NULL) == -1
                && input_on(box2) == 0;
            show(1, NULL, -1, 0, 0);
            const bool no_message = clicked(tick, box2) == -1 && input_on(box2) == 1;
            const bool other_box = clicked(tick, box1) == -1;
            g_e.holds.set(h1, name("comyn"), kHoldHide, true);
            drain(g_e);
            uint8_t* comyn = yesno_ask("comyn", "Log out?", false, true, 0) ? live("comyn") : NULL;
            const bool other_prompt = comyn && clicked(tick, comyn) == -1;
            game_closes("comyn");
            g_e.holds.set(h1, name("comyn"), kHoldHide, false);
            g_e.holds.set(h1, name("rem4li2"), kHoldHide, false);
            drain(g_e);
            check(shown && hidden && other_box && other_prompt && clicked(tick, box2) == -1,
                "the key test's pre: from the tick's left-button test with hidden rem4li2 active showing a page, al 0"
                " and no original, and no routed key or menu mouse mode; from any other call site, with rem4li2"
                " shown, with visible rem4line active, with another hidden prompt asking active or none, the original"
                " runs");
            check(no_message, "hidden rem4li2 active with no message shown asks nothing: the tick's left-button test"
                " runs its original, and the routed key and the menu mouse mode reach it as shown");

            // more pages: the game's own test of the last page, a bare escape no page
            uint8_t* m = g_messages[2];
            auto next_at = [&](const char* rest) {
                memset(m + kMessageText, 0, kMessageNext - kMessageText);
                memcpy(m + kMessageText + 10, rest, strlen(rest) + 1);
                const uint8_t* at = m + kMessageText + 10;
                memcpy(m + kMessageNext, &at, 4);
                return more_pages(m);
            };
            const bool more = next_at("Next page.");
            const bool bare = !next_at("\x7F\x31");
            const bool timed_bare = !next_at("\x7F\x34\x05" "\x7F\x31");
            const bool escaped = next_at("\x7F\x31More.");
            const bool end = !next_at("");
            const uint8_t* outside = m + kMessageNext;
            memcpy(m + kMessageNext, &outside, 4);
            const bool beyond = !more_pages(m);
            memset(m + kMessageNext, 0, 4);
            check(more && bare && timed_bare && escaped && end && beyond && !more_pages(m),
                "more_pages: text past the cursor; none at the end, at a cursor on bare escapes, out of the text, or"
                " with none");
            game_closes("rem4line");
            game_closes("rem4li2");
            for (int d = 0; d < kDialogueCount; ++d) {
                show(d, NULL, -1, 0, 0);
            }
            drain(g_e);
            seen();
            (void)talk1;
        }
        clear_log();
    }

    // a table answer as the binding encodes it: each entry's key parts,
    // type and value; too large; text no binding of this engine makes
    {
        AnswerTable t;
        const bool lines = parse_answer_table("#1=s4869;#3=s;", &t) && t.count == 2 && !t.too_large
            && strcmp(t.entry[0].key[0], "#1") == 0 && t.entry[0].parts == 1 && t.entry[0].type == 's'
            && t.entry[0].length == 2 && strcmp(t.entry[0].bytes, "Hi") == 0 && t.entry[1].length == 0;
        const bool nested = parse_answer_table("name=s4b75706f;color/#2=n15;flag=b1;more=t;f=o;", &t)
            && t.count == 5 && t.entry[1].parts == 2 && strcmp(t.entry[1].key[0], "color") == 0
            && strcmp(t.entry[1].key[1], "#2") == 0 && t.entry[1].type == 'n' && t.entry[1].number == 15
            && t.entry[2].type == 'b' && t.entry[2].number == 1 && t.entry[3].type == 't' && t.entry[4].type == 'o';
        const bool large = parse_answer_table("!", &t) && t.too_large;
        char many[512] = "";
        for (int i = 0; i < kTableEntries + 1; ++i) {
            char one[16];
            snprintf(one, sizeof(one), "#%d=n%d;", i + 1, i);
            strcat(many, one);
        }
        const bool count = parse_answer_table(many, &t) && t.too_large;
        const bool bad = !parse_answer_table("#1=x;", &t) && !parse_answer_table("#1=s4;", &t)
            && !parse_answer_table("=s;", &t) && !parse_answer_table("#1=n;", &t) && !parse_answer_table("#1=b2;", &t)
            && !parse_answer_table("#1=s41", &t) && !parse_answer_table(NULL, &t);
        check(lines && nested && large && count && bad,
            "parse_answer_table: each entry's key, its parts, type and value, a string by its hex; '!' and more than"
            " kTableEntries entries too large; any other text refused");
    }

    // the text-entry windows' fields: a hide takes the line from the
    // window's own field alone, passinpu's suspended, any other's ended
    // with the window's own end, told to drop it; a show starts passinpu's,
    // comment's (on the line it edited) and msgline's again, in the order
    // the next update leaves their menus, msgline's field ended first,
    // keeping its text, when it holds the line
    {
        memset(&g_text, 0, sizeof(g_text));
        memset(g_line, 0, sizeof(g_line));
        auto hide = [&](const char* nm, bool on) {
            g_e.holds.set(h1, name(nm), kHoldHide, on);
            drain(g_e);
        };
        auto holder = [&]() {
            return g_line[kLineBound] ? rdptr(g_line, kLineField) : NULL;
        };
        uint8_t* sd = controller("stringdl");
        uint8_t* cm = controller("comment");
        uint8_t* ml = controller("msgline");
        uint8_t* sc = controller("scname");
        uint8_t* lk = controller("link11");
        uint8_t* pi = controller("passinpu");
        uint8_t* pass_box = g_text_boxes[kTextCount];
        wr32(cm, kCommentField, 0);
        wr32(ml, kMsglineField, 0);
        wr32(sc, kScnameField, 0);
        wr32(lk, kLink11Field, 0);
        wr32(pi, kPassField, 0);
        open_now("stringdl");
        stringdl_edits(sd);
        hide("stringdl", true);
        cm[kCommentMode] = 0;
        open_now("comment");
        const bool started = rdptr(cm, kCommentField) == g_text_boxes[kTextComment] && cm[kCommentEditing] == 0
            && holder() == g_text_boxes[kTextComment];
        cm[kCommentEditing] = 2;
        hide("comment", true);
        open_now("msgline");
        hide("msgline", true);
        open_now("scname");
        open_field(sc, kScnameField, kTextScname);
        hide("scname", true);
        open_now("link11");
        wr32(lk, kLink11Editing, 1);
        open_field(lk, kLink11Field, kTextLink11);
        hide("link11", true);
        open_now("passinpu");
        const int deletes = g_text.deletes;
        hide("passinpu", true);
        bool ended = strcmp(g_text.order, "sssssub") == 0 && !sd[kStringdlEditing] && !rdptr(cm, kCommentField)
            && !rdptr(ml, kMsglineField) && !rdptr(sc, kScnameField) && !rdptr(lk, kLink11Field);
        for (int f = 0; f < 5; ++f) {
            ended = ended && g_text.ends[f] == 1 && g_text.kept[f] == (f == kTextStringdl ? -1 : 0);
        }
        for (int f = 0; f < 6; ++f) {
            ended = ended && g_e.field_ended[f] == 1;
        }
        const bool suspended = rdptr(pi, kPassField) == pass_box && pass_box[0] == 0 && g_text.ends[kTextCount] == 0
            && g_text.deletes == deletes && g_line[kLineBound] == 1 && !rdptr(g_line, kLineField);
        check(started && ended && suspended,
            "a hide of each text window while its field holds the line ends it with the window's own end, told to"
            " drop it; passinpu's is suspended instead, the manager's unbind then its bind of NULL, the field kept");
        // Shown in one pass, passinpu the game's active menu: started in
        // inventory order, passinpu's last, so that it keeps the line;
        // msgline's field, holding it, ended keeping its text before
        // passinpu's start, which drops the field its hide suspended.
        const uint8_t* active = game_active();
        g_text.order[0] = '\0';
        const int pass_deletes = g_text.deletes;
        static const char* const fields[] = {"stringdl", "comment", "msgline", "scname", "link11", "passinpu"};
        for (int f = 0; f < 6; ++f) {
            g_e.holds.set(h1, name(fields[f]), kHoldHide, false);
        }
        drain(g_e);
        bool shown = active == live("passinpu") && strcmp(g_text.order, "oMsusP") == 0
            && rdptr(cm, kCommentField) == g_text_boxes[kTextComment] && g_text_boxes[kTextComment][0] == 0
            && cm[kCommentEditing] == 2 && g_text.start_line == 2 && !rdptr(ml, kMsglineField)
            && g_text.kept[kTextMsgline] == 1 && g_text.kept[kTextCount] == 0 && g_text.deletes == pass_deletes + 2
            && rdptr(pi, kPassField) == pass_box && holder() == pass_box && !sd[kStringdlEditing]
            && !rdptr(sc, kScnameField) && !rdptr(lk, kLink11Field);
        for (int f = 0; f < 6; ++f) {
            shown = shown && g_e.field_ended[f] == 0;
        }
        check(shown,
            "the show of all six in one pass, passinpu the game's active menu, starts comment's field on line 2,"
            " msgline's, then passinpu's: msgline's, holding the line, ended keeping its text first; passinpu's"
            " suspended field dropped, a new one started holding the line");
        game_closes("comment");
        game_closes("msgline");
        game_closes("stringdl");
        game_closes("scname");
        game_closes("link11");
        game_closes("passinpu");

        // The active menu's field started last whatever its place in the
        // inventory: comment's, the first, after msgline's and passinpu's.
        cm[kCommentMode] = 0;
        open_now("comment");
        hide("comment", true);
        open_now("passinpu");
        hide("passinpu", true);
        open_now("msgline");
        hide("msgline", true);
        focus(live("comment"));
        active = game_active();
        g_text.order[0] = '\0';
        g_e.holds.set(h1, name("comment"), kHoldHide, false);
        g_e.holds.set(h1, name("passinpu"), kHoldHide, false);
        g_e.holds.set(h1, name("msgline"), kHoldHide, false);
        drain(g_e);
        const bool focused = active == live("comment") && strcmp(g_text.order, "MsusPo") == 0
            && holder() == g_text_boxes[kTextComment] && !rdptr(ml, kMsglineField) && g_text.kept[kTextMsgline] == 1
            && pass_box[0] == 0;
        check(focused, "comment, the game's active menu, shown with msgline and passinpu: msgline's field started,"
            " then passinpu's after msgline's is ended keeping its text, then comment's, which holds the line");
        game_closes("comment");
        game_closes("passinpu");
        game_closes("msgline");

        // None of them the active menu: started in inventory order, the last
        // keeping the line.
        cm[kCommentMode] = 0;
        open_now("comment");
        hide("comment", true);
        open_now("msgline");
        hide("msgline", true);
        wr32(g_mcb, kMcbActive, 0);
        g_text.order[0] = '\0';
        g_e.holds.set(h1, name("comment"), kHoldHide, false);
        g_e.holds.set(h1, name("msgline"), kHoldHide, false);
        drain(g_e);
        const bool unfocused = strcmp(g_text.order, "oM") == 0 && holder() == g_text_boxes[kTextMsgline]
            && rdptr(cm, kCommentField) == g_text_boxes[kTextComment] && g_text_boxes[kTextComment][0] == 0;
        check(unfocused, "comment and msgline shown with neither the game's active menu: comment's field started,"
            " then msgline's, last in inventory order, which holds the line");
        game_closes("comment");
        game_closes("msgline");

        // One shown while msgline holds the line: msgline's field ended
        // keeping its text, then passinpu's started.
        open_now("passinpu");
        hide("passinpu", true);
        open_now("msgline");
        g_text.order[0] = '\0';
        hide("passinpu", false);
        const bool taken = strcmp(g_text.order, "susP") == 0 && !rdptr(ml, kMsglineField)
            && g_text.kept[kTextMsgline] == 1 && holder() == pass_box && live("msgline");
        check(taken, "passinpu shown while msgline's field holds the line: msgline's ended keeping its text,"
            " then passinpu's dropped and started, holding the line");
        game_closes("msgline");
        game_closes("passinpu");

        // The line held by another window's field: a hide, and the drain
        // after it, leave the hidden window's alone.
        open_now("msgline");
        open_now("stringdl");
        sd[kStringdlEditing] = 1;
        g_text.order[0] = '\0';
        hide("stringdl", true);
        drain(g_e);
        const bool other = g_text.order[0] == '\0' && sd[kStringdlEditing] == 1
            && holder() == g_text_boxes[kTextMsgline] && g_e.field_ended[kTextStringdl] == 0;
        hide("stringdl", false);
        const bool other_shown = g_text.order[0] == '\0';
        sd[kStringdlEditing] = 0;
        game_closes("msgline");
        game_closes("stringdl");
        cm[kCommentMode] = 0;
        open_now("comment");
        open_now("passinpu");
        g_text.order[0] = '\0';
        hide("comment", true);
        const bool unbound = g_text.order[0] == '\0' && rdptr(cm, kCommentField) == g_text_boxes[kTextComment]
            && g_e.field_ended[kTextComment] == 0;
        hide("comment", false);
        check(other && other_shown && unbound && g_text.order[0] == '\0',
            "hidden stringdl editing while msgline's field holds the line: its end is never called, msgline keeps"
            " the line; hidden comment whose field passinpu's start let go: left alone, and nothing at the show");
        game_closes("comment");
        game_closes("passinpu");

        // A bind of NULL leaves the manager bound with no field: no window
        // with none is taken for the one holding the line, neither hidden
        // passinpu at the drain nor msgline at passinpu's show.
        hide("passinpu", true);
        open_now("passinpu");
        open_now("msgline");
        fake_msgline_stop(ml, NULL, 0);
        uint8_t* suspended_box = rdptr(pi, kPassField);
        wr32(pi, kPassField, 0);
        g_text.order[0] = '\0';
        const int message_ends = g_text.ends[kTextMsgline];
        drain(g_e);
        const bool no_field = g_text.order[0] == '\0' && g_line[kLineBound] == 1 && !rdptr(g_line, kLineField);
        memcpy(pi + kPassField, &suspended_box, 4);
        hide("passinpu", false);
        const bool no_message = strcmp(g_text.order, "usP") == 0 && g_text.ends[kTextMsgline] == message_ends
            && holder() == pass_box;
        check(no_field && no_message, "the manager bound to no field: hidden passinpu with no field gets no unbind"
            " and no bind; passinpu shown with msgline open but its field gone ends nothing of msgline's");
        game_closes("msgline");
        game_closes("passinpu");
        hide("passinpu", true);

        // Opened hidden: its open hook's field suspended at once, and again
        // each drain that finds it holding the line; the answer's close
        // drops it once, and the show starts none.
        g_text.order[0] = '\0';
        g_text.deletes = 0;
        g_text.bound_deletes = 0;
        open_now("passinpu");
        const bool hidden_open = strcmp(g_text.order, "ub") == 0 && rdptr(pi, kPassField) == pass_box
            && pass_box[0] == 0 && g_e.field_ended[kFieldPassinpu] == 1;
        open_field(pi, kPassField, kTextCount);
        drain(g_e);
        drain(g_e);
        const bool again = strcmp(g_text.order, "ubub") == 0 && pass_box[0] == 0;
        int context = 0;
        wr32(pi, kPassCallback, reinterpret_cast<uint32_t>(&fake_text_callback));
        wr32(pi, kPassContext, reinterpret_cast<uint32_t>(&context));
        run(cmd(kOpPassinpuCancel, NULL, 0, 0, s1, gen1));
        const bool canceled = strcmp(g_text.order, "ububs") == 0 && g_text.deletes == 1 && g_text.bound_deletes == 0
            && !rdptr(pi, kPassField) && !live("passinpu");
        hide("passinpu", false);
        check(hidden_open && again && canceled && strcmp(g_text.order, "ububs") == 0
                && g_e.field_ended[kFieldPassinpu] == 0,
            "passinpu opened hidden: its field suspended at once and at each drain that finds it bound again;"
            " cancel('passinpu') while hidden: its begin-close drops the field once; the show starts none");

        // A window that edits nothing is left alone; msgline opened hidden
        // has its hook's field ended at once; comment's another player's
        // message has none, its search comment's is ended.
        g_text.order[0] = '\0';
        open_now("scname");
        hide("scname", true);
        hide("scname", false);
        const bool idle = g_text.order[0] == '\0';
        game_closes("scname");
        hide("msgline", true);
        open_now("msgline");
        const bool message = strcmp(g_text.order, "s") == 0 && !rdptr(ml, kMsglineField)
            && g_e.field_ended[kTextMsgline] == 1;
        hide("comment", true);
        cm[kCommentMode] = kCommentView;
        open_now("comment");
        const bool view = strcmp(g_text.order, "s") == 0 && !rdptr(cm, kCommentField);
        game_closes("comment");
        cm[kCommentMode] = 2;
        open_now("comment");
        const bool search = strcmp(g_text.order, "ss") == 0 && !rdptr(cm, kCommentField)
            && g_text.ends[kTextComment] >= 1;
        check(idle && message && view && search,
            "a window that edits nothing is left alone; an open of hidden msgline, whose hook starts its field,"
            " ends it at once; comment's another player's message has none to end; its search comment's is ended");
        game_closes("msgline");
        game_closes("comment");
        hide("msgline", false);
        hide("comment", false);

        // A field the player's key starts again in a hidden window that asks
        // nothing (Enter on the empty stringdl after a delivery is sent): the
        // next drain ends it as the hide does, and never a shown window's;
        // comment's, msgline's and passinpu's start again at the show
        memset(&g_text, 0, sizeof(g_text));
        open_now("stringdl");
        hide("stringdl", true);
        const bool idle_hidden = g_text.order[0] == '\0';
        stringdl_edits(sd);
        drain(g_e);
        const bool key_started = strcmp(g_text.order, "s") == 0 && !sd[kStringdlEditing]
            && g_text.ends[kTextStringdl] == 1 && g_e.field_ended[kTextStringdl] == 1;
        drain(g_e);
        const bool once = strcmp(g_text.order, "s") == 0;
        hide("stringdl", false);
        stringdl_edits(sd);
        drain(g_e);
        const bool shown_kept = strcmp(g_text.order, "s") == 0 && sd[kStringdlEditing] == 1;
        check(idle_hidden && key_started && once && shown_kept,
            "a hidden stringdl whose field a passed key starts: the next drain ends it with the window's own end,"
            " once; shown, the field it edits is left alone");
        sd[kStringdlEditing] = 0;
        game_closes("stringdl");
        cm[kCommentMode] = 0;
        open_now("comment");
        open_now("msgline");
        open_now("passinpu");
        hide("comment", true);
        hide("msgline", true);
        hide("passinpu", true);
        memset(&g_text, 0, sizeof(g_text));
        open_field(cm, kCommentField, kTextComment);
        cm[kCommentEditing] = 1;
        drain(g_e);
        open_field(ml, kMsglineField, kTextMsgline);
        drain(g_e);
        open_field(pi, kPassField, kTextCount);
        drain(g_e);
        const bool restarted_ended = strcmp(g_text.order, "ssub") == 0 && !rdptr(cm, kCommentField)
            && !rdptr(ml, kMsglineField) && rdptr(pi, kPassField) == pass_box && g_text.keep == 0
            && g_e.field_ended[kTextComment] == 1 && g_e.field_ended[kTextMsgline] == 1
            && g_e.field_ended[kFieldPassinpu] == 1;
        g_text.order[0] = '\0';
        game_active();
        g_e.holds.set(h1, name("comment"), kHoldHide, false);
        g_e.holds.set(h1, name("msgline"), kHoldHide, false);
        g_e.holds.set(h1, name("passinpu"), kHoldHide, false);
        drain(g_e);
        const bool restarted_shown = strcmp(g_text.order, "oMsusP") == 0
            && rdptr(cm, kCommentField) == g_text_boxes[kTextComment] && g_text.start_line == 1
            && rdptr(pi, kPassField) == pass_box && holder() == pass_box;
        check(restarted_ended && restarted_shown,
            "hidden comment, msgline and passinpu whose fields keys start again: each drain ends comment's and"
            " msgline's, told to drop them, and suspends passinpu's; the show starts each again with its own"
            " start, comment's on the line it edited (1)");
        game_closes("comment");
        game_closes("msgline");
        game_closes("passinpu");
        events(&cursor, ev, 64);
    }

    // the text windows' replies, each the window's own commit or cancel
    {
        events(&cursor, ev, 64);
        memset(&g_text, 0, sizeof(g_text));
        clear_log();
        auto reply = [&](uint8_t op, const char* nm) {
            Command c = cmd(op, nm, 0, 0, s1, gen1);
            c.verb = op == kOpTextCancel ? kVerbCancel : kVerbAnswer;
            run(c);
        };
        auto refused = [&](const char* verb, const char* nm, const char* reason) {
            const int k = events(&cursor, ev, 64);
            for (int i = 0; i < k; ++i) {
                if (is_error(ev[i], s1, gen1, verb, nm, reason)) {
                    return true;
                }
            }
            return false;
        };
        auto answer = [&](int t, const char* text, int yes = -1) {
            TextAnswer& a = g_e.text_answers[t];
            memset(&a, 0, sizeof(a));
            a.yes = yes;
            snprintf(a.text, sizeof(a.text), "%s", text);
        };
        auto fresh = [&]() {
            memset(&g_text, 0, sizeof(g_text));
            clear_log();
        };

        // stringdl: the name, as row 2 commits it; a yes or no on a slot;
        // their cancels
        uint8_t* delivery = controller("delivery");
        uint8_t* outgoing = open_now("delivery");
        uint8_t* sd = controller("stringdl");
        open_now("stringdl");
        wr16(delivery, kDeliveryState, kDeliveryAsksName);
        sd[kStringdlMode] = kStringdlAsksName;
        stringdl_edits(sd);
        memset(sd + kStringdlName, 'x', kStringdlNameBytes);
        wr16(sd, kStringdlReply, 0);
        answer(kTextStringdl, "Rolanberry");
        reply(kOpTextAnswer, "stringdl");
        bool padded = true;
        for (size_t i = 10; i < kStringdlNameBytes; ++i) {
            padded = padded && sd[kStringdlName + i] == 0;
        }
        check(strcmp(g_text.order, "sff") == 0 && memcmp(sd + kStringdlName, "Rolanberry", 10) == 0 && padded
                && rd16(sd, kStringdlReply) == kStringdlNamed && g_text.focus_menu == outgoing,
            "answer('stringdl', name): the field ended, the name into +0x58 as its strncpy pads it, +0x15C 1, the"
            " outgoing box raised by the handler and again by the routine around it");
        reply(kOpTextAnswer, "stringdl");
        const bool unread = refused("answer", "stringdl", "the outgoing box has not read stringdl's last reply yet");
        wr16(sd, kStringdlReply, 0);
        sd[kStringdlMode] = kStringdlAsksSlot;
        wr16(delivery, kDeliveryState, kDeliveryAsksSlot);
        reply(kOpTextAnswer, "stringdl");
        const bool slot_mode = refused("answer", "stringdl", "stringdl asks yes or no about a delivery slot now:"
            " answer('stringdl', true or false)");
        fresh();
        answer(kTextStringdl, "", 1);
        reply(kOpTextAnswer, "stringdl");
        const bool yes = rd16(sd, kStringdlReply) == kStringdlYes && strcmp(g_text.order, "f") == 0;
        wr16(sd, kStringdlReply, 0);
        reply(kOpTextCancel, "stringdl");
        const bool no = rd16(sd, kStringdlReply) == kStringdlNo;
        wr16(sd, kStringdlReply, 0);
        sd[kStringdlMode] = kStringdlAsksName;
        reply(kOpTextAnswer, "stringdl");
        const bool name_mode = refused("answer", "stringdl", "stringdl is not asking anything now");
        wr16(delivery, kDeliveryState, kDeliveryAsksName);
        answer(kTextStringdl, "", 0);
        reply(kOpTextAnswer, "stringdl");
        const bool asks_name = refused("answer", "stringdl", "stringdl asks for the recipient's name now:"
            " answer('stringdl', name)");
        fresh();
        stringdl_edits(sd);
        reply(kOpTextCancel, "stringdl");
        const bool named_cancel = rd16(sd, kStringdlReply) == kStringdlCanceled && strcmp(g_text.order, "sff") == 0;
        check(unread && slot_mode && yes && no && name_mode && asks_name && named_cancel,
            "stringdl: refused while the box has not read the last reply, a name while it asks about a slot and a"
            " yes or no while it asks the name, anything in another state; on a slot +0x15C 3 for yes, 4 for its"
            " cancel, the box raised once; the name's cancel ends the field and writes 2");
        wr16(sd, kStringdlReply, 0);
        open_now("msgline");
        sd[kStringdlEditing] = 1;
        fresh();
        answer(kTextStringdl, "Rolanberry");
        reply(kOpTextAnswer, "stringdl");
        check(strcmp(g_text.order, "ff") == 0 && rd16(sd, kStringdlReply) == kStringdlNamed && sd[kStringdlEditing] == 1
                && g_line[kLineBound] && rdptr(g_line, kLineField) == g_text_boxes[kTextMsgline],
            "answer('stringdl', name) while msgline's field holds the line: stringdl's end, which would take it,"
            " is not called; the name written and the box raised");
        sd[kStringdlEditing] = 0;
        game_closes("msgline");
        wr16(delivery, kDeliveryState, 0);
        game_closes("stringdl");
        game_closes("delivery");

        // comment: the lines of the message's slot as its line commit
        // writes them, then its close by instance, whose begin-close sends
        fresh();
        g_asked.instance_closes = 0;
        uint8_t* cm = controller("comment");
        cm[kCommentMode] = 0;
        open_now("comment");
        TextAnswer& lines = g_e.text_answers[kTextComment];
        memset(&lines, 0, sizeof(lines));
        snprintf(lines.lines[0], sizeof(lines.lines[0]), "Hello");
        snprintf(lines.lines[2], sizeof(lines.lines[2]), "%s", "0123456789012345678901234567890123456789");
        memset(cm + kCommentSlots, 'x', 3 * kCommentLineBytes);
        reply(kOpTextAnswer, "comment");
        const char* line0 = reinterpret_cast<const char*>(cm + kCommentSlots);
        const char* line1 = line0 + kCommentLineBytes;
        const char* line2 = line1 + kCommentLineBytes;
        check(strcmp(g_text.order, "sztztzt") == 0 && g_text.keep == 0 && strcmp(line0, "Hello") == 0
                && line1[0] == '\0' && strlen(line2) == 40 && g_asked.instance_closes == 1
                && g_asked.instance_flag == 1 && !live("comment"),
            "answer('comment', lines): the field ended dropping its line, each line of the bazaar message's slot"
            " copied 40 bytes at most, sanitized and trimmed, then the window closed by instance");
        fresh();
        cm[kCommentMode] = 2;
        open_now("comment");
        reply(kOpTextAnswer, "comment");
        const char* search = reinterpret_cast<const char*>(cm + kCommentSlots + 6 * kCommentLineBytes);
        const bool searched = strcmp(search, "Hello") == 0 && strcmp(g_text.order, "sztztzt") == 0;
        cm[kCommentMode] = kCommentView;
        open_now("comment");
        reply(kOpTextAnswer, "comment");
        const bool viewing = refused("answer", "comment", "comment shows another player's message, which takes no"
            " answer");
        game_closes("comment");
        reply(kOpTextAnswer, "comment");
        const bool shut = refused("answer", "comment", "comment is not open, and nothing asks through it until the game"
            " opens it");
        check(searched && viewing && shut, "comment: a search comment's lines into its own slot; refused on another"
            " player's message and with no window");

        // msgline: the commit of each mode that writes a message, as the
        // game's sends it; the busy byte and the sending indicator after a
        // queued one; its refusals; its cancel
        fresh();
        uint8_t* ml = controller("msgline");
        wr16(ml, kMsglineMode, 0);
        wr32(ml, kMsglineWord1, 0x1111);
        wr32(ml, kMsglineWord2, 0x00051234);
        wr32(g_pol, kPolWord, 0x55);
        snprintf(reinterpret_cast<char*>(ml + kMsglineName), kMsglineNameBytes, "Friend");
        memset(ml + kMsglineText, 'x', kMsglineLong + 1);
        open_now("msgline");
        answer(kTextMsgline, "Hi there");
        g_busy = 0;
        reply(kOpTextAnswer, "msgline");
        const uint32_t* a = g_text.send_args;
        check(strcmp(g_text.order, "snSm") == 0 && g_text.keep == 0 && strcmp(g_log.order, "K") == 0
                && g_text.pol_word == 0x55 && g_text.send_mode == 0 && a[0] == 0x1111 && a[1] == 0x05341234
                && a[2] == reinterpret_cast<uintptr_t>(ml + kMsglineName)
                && strcmp(reinterpret_cast<const char*>(a[3]), "Text 28") == 0
                && a[4] == reinterpret_cast<uintptr_t>(ml + kMsglineText)
                && a[5] == reinterpret_cast<uintptr_t>(&fake_sent) && a[6] == 0x11
                && strcmp(g_text.send_text, "Hi there") == 0 && ml[kMsglineTextEnd] == 0
                && ml[kMsglineText + 8] == 0 && g_text.mark_obj == g_sending + kSendingMark && g_text.mark_flags == 0x10
                && g_busy == 1 && !live("msgline"),
            "answer('msgline', text) in mode 0: the field ended dropping its text, the text into +0x53 (+0x17E"
            " zeroed), the game's message 9/0x1C, the POL interface's number for its +0x80 word packed with +0x19C, the sender"
            " given +0x198, the name, the message, the text, the first callback and 0x11; queued, the indicator"
            " marked 0x10 and the busy byte set; then msgline closed by name");
        open_now("msgline");
        reply(kOpTextAnswer, "msgline");
        const bool busy = refused("answer", "msgline", "a message is still on its way; msgline sends the next once it"
            " has gone");
        g_busy = 0;
        g_msgline_player_ptr = NULL;
        reply(kOpTextAnswer, "msgline");
        const bool player = refused("answer", "msgline", "the player's data is not loaded, so msgline sends nothing");
        g_msgline_player_ptr = g_msgline_player;
        g_pol_ptr = NULL;
        reply(kOpTextAnswer, "msgline");
        const bool pol = refused("answer", "msgline", "the POL interface is not there, so msgline sends nothing");
        g_pol_ptr = g_pol;
        wr16(ml, kMsglineMode, 3);
        reply(kOpTextAnswer, "msgline");
        const bool fixed = refused("answer", "msgline", "msgline's mode 3 sends a text of the game's own; it takes no"
            " answer");
        wr16(ml, kMsglineMode, 2);
        answer(kTextMsgline, "sixteen bytes!!!");
        reply(kOpTextAnswer, "msgline");
        const bool short_field = refused("answer", "msgline", "16 bytes; msgline's mode 2 takes at most 15");
        check(busy && player && pol && fixed && short_field, "msgline: refused while the busy byte is set, with the"
            " player's data global or the POL interface's NULL, in a mode that writes no message, and past its mode's"
            " field");
        // The long field is 300 bytes, and the commit zeroes its last: the
        // game keeps 299.
        char long_text[kMsglineLong + 1];
        memset(long_text, 'y', kMsglineLong);
        long_text[kMsglineLong] = '\0';
        wr16(ml, kMsglineMode, 0);
        answer(kTextMsgline, long_text);
        fresh();
        reply(kOpTextAnswer, "msgline");
        const bool three_hundred = refused("answer", "msgline", "300 bytes; msgline's mode 0 takes at most 299")
            && g_text.order[0] == '\0';
        long_text[kMsglineKept] = '\0';
        answer(kTextMsgline, long_text);
        reply(kOpTextAnswer, "msgline");
        const bool kept = strcmp(g_text.order, "snSm") == 0 && strlen(g_text.send_text) == 299
            && ml[kMsglineText + 298] == 'y' && ml[kMsglineText + 299] == 0 && ml[kMsglineTextEnd] == 0;
        check(kMsglineKept == 299 && msgline_max(0) == 299 && msgline_max(1) == 299 && msgline_max(5) == 299
                && msgline_max(2) == 15 && msgline_max(7) == 15 && three_hundred && kept,
            "msgline's long field keeps 299 bytes, the 300th the commit zeroes: 300 refused, 299 sent whole");
        g_busy = 0;
        open_now("msgline");
        wr16(ml, kMsglineMode, 2);
        fresh();
        answer(kTextMsgline, "a\x07" "b");
        g_text.send_result = 2;
        events(&cursor, ev, 64);
        reply(kOpTextAnswer, "msgline");
        a = g_text.send_args;
        check(strcmp(g_text.order, "scnS") == 0 && g_text.send_mode == 2 && strcmp(g_text.send_text, "a") == 0
                && a[2] == reinterpret_cast<uintptr_t>(ml + kMsglineText)
                && strcmp(g_text.clocks[0], "clock 1") == 0
                && a[4] == reinterpret_cast<uintptr_t>(ml + kMsglineName) && a[6] == 0x14 && g_busy == 0
                && !live("msgline") && refused("answer", "msgline", "the game could not queue the message"),
            "mode 2: each 7 in the text zeroed, the clock's text 1, the sender given +0x198, the number, the text, the"
            " clock, the name, the first callback and 0x14; a sender's nonzero answer marks nothing and leaves the"
            " busy byte, msgline still closes, and an error event says the message was not queued");
        g_text.send_result = 0;
        g_busy = 0;
        for (int i = 0; i < 5; ++i) {
            wr32(ml, kMsglineIds + 4 * (i < 3 ? i : 0), 0);
        }
        wr32(ml, kMsglineIds, 0xA1);
        wr32(ml, kMsglineIds + 4, 0xA2);
        wr32(ml, kMsglineIds + 8, 0xA3);
        wr32(ml, kMsglineIds2, 0xB1);
        wr32(ml, kMsglineIds2 + 4, 0xB2);
        bool modes = true;
        const int each[3] = {1, 5, 7};
        for (int i = 0; i < 3; ++i) {
            fresh();
            g_busy = 0;
            wr16(ml, kMsglineMode, each[i]);
            open_now("msgline");
            answer(kTextMsgline, "Ok");
            reply(kOpTextAnswer, "msgline");
            a = g_text.send_args;
            if (each[i] == 1) {
                modes = modes && strcmp(g_text.order, "sSm") == 0 && a[0] == 0xA1 && a[1] == 0xA2 && a[2] == 0xA3
                    && strcmp(reinterpret_cast<const char*>(a[3]), "Text 28") == 0 && a[6] == 0x11;
            } else if (each[i] == 5) {
                modes = modes && strcmp(g_text.order, "sSm") == 0 && a[0] == 0xB1 && a[1] == 0xB2 && a[2] == 0xA1
                    && a[3] == 0xA2 && strcmp(reinterpret_cast<const char*>(a[4]), "Text 31") == 0
                    && a[5] == reinterpret_cast<uintptr_t>(ml + kMsglineText)
                    && a[6] == reinterpret_cast<uintptr_t>(&fake_text_open) && a[7] == 0x12;
            } else {
                modes = modes && strcmp(g_text.order, "sccSm") == 0 && a[0] == 0xB1 && a[1] == 0xB2 && a[2] == 0xA1
                    && a[3] == 0xA2 && a[4] == reinterpret_cast<uintptr_t>(ml + kMsglineText)
                    && strcmp(g_text.clocks[0], "clock 9") == 0 && strcmp(g_text.clocks[1], "clock 1") == 0
                    && a[7] == reinterpret_cast<uintptr_t>(&fake_text_open) && a[8] == 0x15;
            }
            modes = modes && g_text.send_mode == each[i] && strcmp(g_text.send_text, "Ok") == 0 && g_busy == 1;
        }
        check(modes, "modes 1, 5 and 7 send through their own senders with +0x1D0.., +0x1E0.., the messages 9/0x1C"
            " and 9/0x1F, the clock's texts 9 and 1 and the second callback, as the game's cases give them");
        fresh();
        g_busy = 0;
        open_now("msgline");
        reply(kOpTextCancel, "msgline");
        check(strcmp(g_text.order, "s") == 0 && g_text.keep == 0 && strcmp(g_log.order, "K") == 0
                && g_text.send_mode == 0 && !live("msgline"),
            "cancel('msgline'): the field ended dropping its text, then msgline closed by name; nothing sent");

        // a message a block kept from opening: recorded at the refused open,
        // announced, answered with no window, ended as another message is
        // set up, by its cancel and by an open
        {
            auto pending = [&](bool on, bool mine) {
                const int k = events(&cursor, ev, 64);
                for (int i = 0; i < k; ++i) {
                    if (is_pending(ev[i], kPendingMsgline, on) && pending_from(ev[i], s1, gen1) == mine) {
                        return true;
                    }
                }
                return false;
            };
            fresh();
            wr16(ml, kMsglineMode, 0);
            g_e.holds.set(h1, name("msgline"), kHoldBlock, true);
            drain(g_e);
            events(&cursor, ev, 64);
            const bool refused_open = open_now("msgline") == NULL;
            const bool announced = refused_open && g_e.msgline.active && g_e.msgline.mode == 0
                && g_e.msgline.id == static_cast<uint32_t>(g_e.instance[name("msgline")]) && pending(true, false)
                && strcmp(pending_name(kPendingMsgline), "msgline") == 0;
            answer(kTextMsgline, "Blocked");
            g_busy = 0;
            reply(kOpTextAnswer, "msgline");
            const bool sent = strcmp(g_text.order, "nSm") == 0 && strcmp(g_text.send_text, "Blocked") == 0
                && g_busy == 1 && !g_e.msgline.active && g_log.order[0] == '\0' && pending(false, true);
            g_busy = 0;
            open_now("msgline");
            wr16(ml, kMsglineMode, 3);
            drain(g_e);
            const bool replaced = !g_e.msgline.active && pending(false, false);
            wr16(ml, kMsglineMode, 3);
            open_now("msgline");
            const bool none = !g_e.msgline.active;
            wr16(ml, kMsglineMode, 7);
            open_now("msgline");
            fresh();
            reply(kOpTextCancel, "msgline");
            const bool canceled = !g_e.msgline.active && g_text.order[0] == '\0' && strcmp(g_log.order, "") == 0;
            const bool ended = pending(false, true);
            open_now("msgline");
            g_e.holds.set(h1, name("msgline"), kHoldBlock, false);
            drain(g_e);
            events(&cursor, ev, 64);
            open_now("msgline");
            const bool opened = !g_e.msgline.active && pending(false, false);
            game_closes("msgline");
            check(announced && sent && replaced && none && canceled && ended && opened,
                "msgline blocked: the refused open records its mode and open count and posts pending; an answer"
                " ends it (pending false, its handle's) and sends as the window would, closing nothing; another"
                " message set up ends it, a mode that writes none records nothing, a cancel ends it sending"
                " nothing, and an open of the window ends it");
        }

        // scname: row 2's search
        fresh();
        uint8_t* sc = controller("scname");
        open_now("scname");
        open_field(sc, kScnameField, kTextScname);
        memset(sc + kScnameName, 'x', kScnameBytes - kScnameName);
        answer(kTextScname, "Rolan");
        reply(kOpTextAnswer, "scname");
        check(strcmp(g_text.order, "sCNQ") == 0 && g_text.keep == 0
                && strcmp(reinterpret_cast<const char*>(sc + kScnameName), "Rolan") == 0
                && sc[kScnameBytes - 1] == 0 && g_text.criteria == g_search + kSearchCriteria
                && strcmp(g_text.criteria_name, "Rolan") == 0 && g_text.search_args[0] == 1
                && g_text.search_args[1] == static_cast<int>(reinterpret_cast<uintptr_t>(g_search + kSearchCriteria))
                && g_text.search_args[2] == 1 && g_text.search_args[3] == 0 && g_text.search_args[4] == 0
                && g_text.search_args[5] == 0 && strcmp(g_log.closes, "scname  |scsibori|") == 0,
            "answer('scname', name): the field ended dropping its text, the name into +0x18, the search's criteria"
            " cleared and named +0x18, the search started (1, criteria, 1, 0, 0, 0), then scname and scsibori"
            " closed by name");
        fresh();
        open_now("scname");
        reply(kOpTextCancel, "scname");
        const bool closed = strcmp(g_log.closes, "scname  |") == 0 && g_text.order[0] == '\0';
        open_now("scname");
        g_search_ptr = NULL;
        reply(kOpTextAnswer, "scname");
        const bool unset = refused("answer", "scname", "the player search is not set up, so scname takes no input");
        g_search_ptr = g_search;
        game_closes("scname");
        check(closed && unset, "cancel('scname') closes scname alone; with no player search set up scname takes no"
            " answer");

        // link11: row 5's request for link5's selected row
        fresh();
        uint8_t* link5 = controller("link5");
        uint8_t* list = open_now("link5");
        uint8_t* rows = g_link5_rows[0];
        memset(g_link5_rows, 0, sizeof(g_link5_rows));
        g_link5_records[1][0] = 5;
        g_link5_records[1][1] = 0;
        uint8_t* record = g_link5_records[1];
        memcpy(g_link5_rows[1] + kLink5RowRecord, &record, 4);
        memcpy(link5 + kLink5Rows, &rows, 4);
        wr32(link5, kLink5Count, 3);
        wr16(link5, kListScroll, 0);
        wr16(list, kMenuCursor, 2);
        link5[kLink5Made] = 0;
        uint8_t* lk = controller("link11");
        open_now("link11");
        wr32(lk, kLink11Editing, 1);
        open_field(lk, kLink11Field, kTextLink11);
        lk[kLink11Colors] = 7;
        lk[kLink11Colors + 1] = 7;
        lk[kLink11Colors + 2] = 7;
        memset(g_made, 0, sizeof(g_made));
        g_text.create_answer = 0xABCD0001u;
        TextAnswer& made = g_e.text_answers[kTextLink11];
        answer(kTextLink11, "Kupo");
        made.color[0] = 1;
        made.color[1] = 2;
        made.color[2] = 3;
        made.colored = 1;
        reply(kOpTextAnswer, "link11");
        const uint32_t* c = g_text.create_args;
        uint32_t word = 0;
        memcpy(&word, g_made + 4, 4);
        check(strcmp(g_text.order, "LskeRIf") == 0 && g_text.keep == 1
                && strcmp(reinterpret_cast<const char*>(lk + kLink11Name), "Kupo") == 0 && lk[kLink11Colors] == 1
                && lk[kLink11Colors + 2] == 3 && c[0] == 1 && c[1] == 0 && c[2] == 5 && c[3] == 1
                && strcmp(g_text.create_name, "<Kupo>") == 0 && c[5] == 1 && c[6] == 2 && c[7] == 3
                && g_made[0] == 1 && word == 0x00C0FFEEu && g_made[8] == 1 && g_made[9] == 0x3C
                && (link5[kLink5Made] & 1) && strcmp(g_log.closes, "link11  |") == 0
                && strcmp(g_open_key, "menu    link6   ") == 0 && g_text.focus_menu == list && !live("link11"),
            "answer('link11', {name, color}): link5's selection, the field ended keeping it, the name into +0x34 and"
            " the colors into +0x30.., slot 1 while the second is free, the encoded name and the colors requested"
            " for the selected row's container and index; queued, the request's bytes (slot, the item's word, 1,"
            " 0x3C) and link5's +0x65 bit 0; then link11 closed, link6 opened and link5 raised");
        fresh();
        open_now("link11");
        g_text.slot_taken = 0xABCD0001u;
        g_text.create_answer = 0xABCD0000u;
        memset(g_made, 0, sizeof(g_made));
        link5[kLink5Made] = 0;
        made.colored = 0;
        lk[kLink11Colors] = 9;
        reply(kOpTextAnswer, "link11");
        const bool failed = strcmp(g_text.order, "LkeRf") == 0 && g_text.create_args[0] == 2
            && lk[kLink11Colors] == 9 && g_made[0] == 0 && link5[kLink5Made] == 0
            && strcmp(g_log.closes, "link11  |") == 0 && refused("answer", "link11", "the game could not queue the new"
                " linkshell's request");
        fresh();
        open_now("link11");
        wr16(list, kMenuCursor, 4);
        reply(kOpTextAnswer, "link11");
        const bool none = refused("answer", "link11", "link5 has no linkshell selected for the new one")
            && g_text.order[0] == '\0' && live("link11");
        wr32(lk, kLink11Editing, 2);
        reply(kOpTextCancel, "link11");
        const bool backed = strcmp(g_text.order, "sf") == 0 && g_text.keep == 0
            && strcmp(g_log.closes, "link11  |") == 0 && strcmp(g_open_key, "menu    link6   ") == 0
            && !live("link11") && g_text.create_args[0] == 0;
        check(failed && none && backed,
            "link11: slot 2 while the second is taken, the colors kept with none given; a request not queued writes"
            " nothing, posts why and still closes link11, opens link6 and raises link5; refused with no row"
            " selected; its cancel ends the field, then closes, opens and raises the same");
        game_closes("link5");
        events(&cursor, ev, 64);
        clear_log();
    }

    // the trade windows: prompts from engine abi 8; an offer, a withdrawal,
    // the OK and the cancel each through the window's own routines, refused
    // up front for all the game refuses, the game's own gray test run on the
    // game thread; the trade's offers on their way until the server's
    // acknowledgment shows them, and dropped as the trade ends; the one change
    // a second the game sends
    {
        events(&cursor, ev, 64);
        memset(&g_offer, 0, sizeof(g_offer));
        g_offer.ok_answer[0] = g_offer.ok_answer[1] = 1;
        g_offer.cancel_answer = 1;
        g_offer.test_answer = kRowListed;
        clear_log();
        auto refused = [&](const char* verb, const char* nm, const char* reason) {
            const int k = events(&cursor, ev, 64);
            for (int i = 0; i < k; ++i) {
                if (is_error(ev[i], s1, gen1, verb, nm, reason)) {
                    return true;
                }
            }
            return false;
        };
        auto quiet = [&]() {
            const int k = events(&cursor, ev, 64);
            for (int i = 0; i < k; ++i) {
                if (event_type(ev[i]) == kEvError) {
                    return false;
                }
            }
            return true;
        };
        auto offer = [&](const char* nm, int index, uint32_t count) {
            Command c = cmd(kOpOffer, nm, index, 0, s1, gen1, count);
            c.verb = kVerbOffer;
            run(c);
        };
        auto withdraw = [&](const char* nm, int index) {
            Command c = cmd(kOpWithdraw, nm, index, 0, s1, gen1);
            c.verb = kVerbWithdraw;
            run(c);
        };
        auto ok = [&](const char* nm) {
            Command c = cmd(kOpOfferAnswer, nm, 0, 0, s1, gen1, 1);
            c.verb = kVerbAnswer;
            run(c);
        };
        auto cancel = [&](const char* nm) {
            Command c = cmd(kOpOfferCancel, nm, 0, 0, s1, gen1);
            c.verb = kVerbCancel;
            run(c);
        };
        auto view = [&](int w) {
            static OfferView v;
            offer_view(g_e, w, offer_now(g_e, kPlain, w), &g_e.trade_pending, &v);
            return &v;
        };
        // Time on past the last send, so the next change goes at once.
        auto later = [&]() { set_clock(clock_now() + 1001); };

        check(prompt_window("trade", 8) && !prompt_window("trade", 7) && prompt_cancels("handover")
                && !boolean_window("trade") && offer_named("handover") == kOfferHandover && offer_named("comyn") < 0
                && g_e.inv.offer_of(name("trade")) == kOfferTrade && g_e.inv.offer_window[kOfferHandover]
                    == name("handover") && g_e.inv.offer_of(name("gift")) < 0,
            "trade and handover: prompts from engine abi 8 that cancel() ends, by index both ways");

        // the world: 1000 gil, a stack of 12 (id 4096) in 1, 3 of id 4097 in 2,
        // id 4096 again in 3, one of status 0x19 in 4, one in a bazaar
        // (status 0x13) in 6, the cache knowing nothing of 9999 in 7
        memset(g_pd, 0, sizeof(g_pd));
        put_item(0, kGilId, 1000, 0);
        put_item(1, 4096, 12, 0);
        put_item(2, 4097, 3, 0);
        put_item(3, 4096, 5, 0);
        put_item(4, 16384, 1, 0x19);
        put_item(6, 4098, 1, 0x13);
        put_item(7, 9999, 1, 0);
        put_item(8, 4099, 300, 0);
        for (int i = 9; i <= 16; ++i) {
            put_item(i, static_cast<uint16_t>(5000 + i), 1, 0);
        }
        uint8_t* trade = controller("trade");
        uint8_t* menu = open_now("trade");
        wr16(menu, kMenuItems, 11);
        wr32(trade, kTradePartner, 3);
        wr32(trade, kTradePartnerId, 0x01000003);
        wr32(trade, kTradeState, kTradeOpen);
        wr32(trade, kTradeTheirState, kTradeOpen);
        clear_log();

        // the gray test of every item each frame while the window is open,
        // for the Lua thread through the sequence word: a status outside 0, 5
        // and 0x1B grays 4, 0x13 grays 6 for any mode but the one the rule
        // names, the cache knows nothing of 7, 5 holds nothing
        {
            const int tests = g_offer.tests;
            drain(g_e);
            GrayRows rows;
            const bool read = g_e.gray_table.read(&rows);
            const uint32_t open_now = static_cast<uint32_t>(g_e.instance[name("trade")]);
            const LONG seq = g_e.gray_table.seq;
            drain(g_e);
            check(read && rows.open && rows.which == kOfferTrade && rows.instance == open_now && open_now != 0
                    && rows.id[1] == 4096 && rows.code[1] == kGrayTakes && rows.code[2] == kGrayTakes
                    && rows.code[4] == kGrayGrayed && rows.code[6] == kGrayGrayed && rows.code[7] == kGrayNoData
                    && rows.id[7] == 9999 && rows.code[5] == kGrayNone && rows.id[5] == 0
                    && rows.code[16] == kGrayTakes && rows.code[17] == kGrayNone && rows.code[0] == kGrayNone
                    && g_offer.tests == tests + 2 * 14 && g_item_data[kItemLock] == 0 && (seq & 1) == 0
                    && g_e.gray_table.seq == seq,
                "the drain runs the game's gray test of every inventory item each frame while trade is open (14"
                " items with data, twice), the lock byte back each time; the rows by index with the id each ran on,"
                " written through the sequence word only when they changed");
            char buf[kErrorReason];
            HeldItem held;
            memset(&held, 0, sizeof(held));
            held.present = true;
            held.id = 16384;
            const char* gray = gray_rows_refusal(rows, kOfferTrade, open_now, 4, held, buf, sizeof(buf));
            const bool trade_text = gray && strcmp(gray, "that item cannot be traded") == 0;
            const bool other_open = gray_rows_refusal(rows, kOfferTrade, open_now + 1, 4, held, buf, sizeof(buf))
                == NULL;
            const bool other_window = gray_rows_refusal(rows, kOfferHandover, open_now, 4, held, buf, sizeof(buf))
                == NULL;
            held.id = 16385;
            const bool other_item = gray_rows_refusal(rows, kOfferTrade, open_now, 4, held, buf, sizeof(buf)) == NULL;
            held.id = 4096;
            const bool takes = gray_rows_refusal(rows, kOfferTrade, open_now, 1, held, buf, sizeof(buf)) == NULL;
            held.id = 9999;
            const char* no_data = gray_rows_refusal(rows, kOfferTrade, open_now, 7, held, buf, sizeof(buf));
            const bool no_data_text = no_data && strstr(no_data, "the game has no data for that item") == no_data;
            GrayRows handover = rows;
            handover.which = kOfferHandover;
            held.id = 16384;
            const char* npc = gray_rows_refusal(handover, kOfferHandover, open_now, 4, held, buf, sizeof(buf));
            check(trade_text && other_open && other_window && other_item && takes && no_data_text && npc
                    && strcmp(npc, "that item cannot be handed to an NPC") == 0,
                "the rows refuse an item up front only for the window and the open they ran on and the item they ran"
                " on: grayed 'that item cannot be traded', for handover 'that item cannot be handed to an NPC'; an"
                " item with no data as the game thread's test says it");
        }

        // options' reads: the tables, the names, the wait
        put_theirs(0, kGilId, 50);
        put_theirs(2, 4321, 7);
        {
            const OfferView& v = *view(kOfferTrade);
            char partner[kActorNameBytes + 1];
            char npc[kActorNameBytes + 1];
            check(v.open && v.data && v.target == 3 && v.target_id == 0x01000003 && v.gil == 0 && v.their_gil == 50
                    && v.theirs[2].id == 4321 && v.theirs[2].count == 7 && v.theirs[1].id == 0 && v.held_gil == 1000
                    && v.wait == 0 && v.pending_count == 0
                    && actor_name(kGuarded, g_e.game.offer, 3, partner, sizeof(partner))
                    && strcmp(partner, "Partner") == 0 && actor_name(kGuarded, g_e.game.offer, 5, npc, sizeof(npc))
                    && strcmp(npc, "Titled NPC") == 0 && !actor_name(kGuarded, g_e.game.offer, 4, npc, sizeof(npc))
                    && !actor_name(kGuarded, g_e.game.offer, 0, npc, sizeof(npc)),
                "the trade read: the partner's offer from the player's data, the gil held, the wait 0 before any send;"
                " names through the actor table, a titled one while its length word is set, none for an empty slot");
        }

        // an item: the first free slot's row, then the stage; on its way
        // until acknowledged
        const int tests = g_offer.tests;
        offer("trade", 1, 4);
        const TradePending& tp = g_e.trade_pending;
        const bool staged = quiet() && g_offer.stages[kOfferTrade] == 1 && g_offer.stage_row == 1
            && g_offer.stage_args[0] == 1 && g_offer.stage_args[1] == 4096 && g_offer.stage_args[2] == 4
            && g_offer.sends == 1 && g_offer.sent[2] == 1 && g_log.select_row == 1 && tp.count == 1
            && tp.entry[0].slot == 1 && tp.entry[0].index == 1 && tp.entry[0].count == 4
            && tp.partner == 0x01000003 && g_offer.tests == tests + 1 + 14 && g_offer.lock_at_test == 1
            && g_item_data[kItemLock] == 0 && g_offer.describe_seen == (0x5EED0000u | 1);
        check(staged, "offer('trade', 1, 4): the gray test of the item once more, with its data locked around the"
            " mode's test and its description, then row 1 selected and the stage with the index, the id and the count;"
            " the change sent, the offer on its way; then the frame's test of every item");
        offer("trade", 2, 1);
        const bool throttled = refused("offer", "trade", "the game sends one trade change per second; try again in 1.0 s")
            && g_offer.stages[kOfferTrade] == 1;
        set_clock(clock_now() + 600);
        offer("trade", 2, 1);
        const bool sooner = refused("offer", "trade", "try again in 0.4 s");
        later();
        {
            const OfferView& v = *view(kOfferTrade);
            check(throttled && sooner && v.wait == 0 && v.pending_count == 1 && v.pending[0].pending,
                "a change within the second refused with the wait (1.0 s, then 0.4 s), nothing staged; options'"
                " offer counts the one on its way, pending");
        }
        offer("trade", 1, 2);
        const bool again = refused("offer", "trade", "that item is already offered; withdraw it first");
        withdraw("trade", 1);
        const bool unacked = refused("withdraw", "trade", "the server has not acknowledged that item's offer yet");
        offer("trade", 3, 2);
        const bool other_stack = quiet() && g_offer.stage_row == 2 && tp.count == 2;
        check(again && unacked && other_stack, "the item on its way: offered again or withdrawn, refused; another stack"
            " of the same id at another index offered into slot 2");
        // the acknowledgment
        put_own(1, 4096, 1, 4);
        drain(g_e);
        const bool acked = tp.count == 1 && tp.entry[0].index == 3;
        put_own(2, 4096, 3, 2);
        drain(g_e);
        check(acked && tp.count == 0, "each offer on its way dropped once the server's acknowledgment writes its"
            " index into the acknowledged offer");

        // the refusals
        later();
        offer("trade", 5, 1);
        const bool empty = refused("offer", "trade", "inventory slot 5 holds no item");
        offer("trade", 2, 0);
        const bool zero = refused("offer", "trade", "count is 1 or more; use withdraw to take it out");
        offer("trade", 2, 4);
        const bool more = refused("offer", "trade", "inventory slot 2 holds 3 of that item, fewer than 4");
        offer("trade", 0, 1001);
        const bool gil_more = refused("offer", "trade", "the player holds 1000 gil");
        check(empty && zero && more && gil_more && g_offer.stages[kOfferTrade] == 2,
            "refused: an empty inventory slot, a count of 0, more than the slot holds, more gil than held");
        // the gray test
        g_offer.test_answer = kRowGray;
        offer("trade", 2, 1);
        const bool gray = refused("offer", "trade", "that item cannot be traded");
        g_offer.test_answer = kRowListed;
        offer("trade", 4, 1);
        const bool status = refused("offer", "trade", "that item cannot be traded");
        g_offer.test_answer = kRowOffered;
        offer("trade", 4, 1);
        const bool highlighted = quiet() && g_offer.stage_args[0] == 4;
        later();
        g_offer.test_answer = kRowListed;
        offer("trade", 6, 1);
        const bool bazaar = refused("offer", "trade", "that item cannot be traded");
        g_status_mode_ptr = g_mode[kOfferTrade];
        offer("trade", 6, 1);
        const bool modal = quiet() && g_offer.stage_args[0] == 6;
        g_status_mode_ptr = NULL;
        later();
        offer("trade", 7, 1);
        const bool unknown = refused("offer", "trade", "the game has no data for that item");
        g_offer.test_answer = 0;
        offer("trade", 2, 1);
        const bool unlisted = refused("offer", "trade", "does not show that item");
        g_offer.test_answer = kRowListed;
        check(gray && status && highlighted && bazaar && modal && unknown && unlisted && g_item_data[kItemLock] == 0,
            "the game's gray test: a row its mode grays refused; a status outside 0, 5 and 0x1B grays one;"
            " the offered color passes the status by; 0x13 listed for the one mode the rule names; an item the cache"
            " has no data for, and a row the test does not list, refused; the lock byte back each time");
        // the slots fill: 4 and 6 went to slots 3 and 4, 9..12 to 5..8
        put_own(3, 16384, 4, 1);
        put_own(4, 4098, 6, 1);
        for (int i = 9; i <= 12; ++i) {
            later();
            offer("trade", i, 1);
            put_own(i - 4, static_cast<uint16_t>(5000 + i), static_cast<uint8_t>(i), 1);
            drain(g_e);
        }
        later();
        offer("trade", 13, 1);
        const bool full = refused("offer", "trade", "all 8 item slots are full; withdraw one first");
        // a withdrawal: the slot's row, the stage with the slot's item and 0
        withdraw("trade", 3);
        const bool taken = quiet() && g_offer.stage_row == 2 && g_offer.stage_args[0] == 3
            && g_offer.stage_args[1] == 4096 && g_offer.stage_args[2] == 0 && g_offer.sent[1] == 0
            && g_offer.sent[2] == 2;
        later();
        withdraw("trade", 2);
        const bool not_offered = refused("withdraw", "trade", "that item is not offered");
        check(full && taken && not_offered, "a 9th item refused; withdraw('trade', 3): row 2 selected, the stage with"
            " index 3, its id and 0, the game's removal; an item not offered refused");
        // the gil: row 11, index 0, the gil's id
        put_own(2, 0, 0, 0);
        offer("trade", 0, 250);
        const bool gil = quiet() && g_offer.stage_row == kTradeGilRow && g_offer.stage_args[0] == 0
            && g_offer.stage_args[1] == kGilId && g_offer.stage_args[2] == 250 && g_offer.sent[1] == 250
            && tp.count == 0;
        check(gil, "offer('trade', 'gil', 250): row 11 selected, the stage with 0, the gil's id and 250, nothing on its"
            " way");

        // the trade ends: the offers on their way dropped
        later();
        offer("trade", 14, 1);
        const bool sent = tp.count == 1;
        wr32(trade, kTradeState, 5);
        drain(g_e);
        const bool ended = tp.count == 0;
        wr32(trade, kTradeState, kTradeOpen);
        later();
        offer("trade", 14, 1);
        wr32(trade, kTradePartnerId, 0x01000004);
        drain(g_e);
        const bool other_partner = tp.count == 0;
        wr32(trade, kTradePartnerId, 0x01000003);
        check(sent && ended && other_partner, "the offers on their way dropped as the trade leaves its states 3 and 4,"
            " and for another partner");

        // the OK and the cancel
        wr32(trade, kTradeState, 1);
        ok("trade");
        const bool not_yet = refused("answer", "trade", "no trade is under way");
        wr32(trade, kTradeState, kTradeOpen);
        g_offer.ok_answer[kOfferTrade] = 0;
        ok("trade");
        const bool unsent = refused("answer", "trade", "the game did not send the OK");
        g_offer.ok_answer[kOfferTrade] = 1;
        ok("trade");
        const bool okd = quiet() && g_offer.oks[kOfferTrade] == 2 && rd32(trade, kTradeState) == kTradeOkSent;
        ok("trade");
        const bool twice = refused("answer", "trade", "your OK is already in");
        later();
        offer("trade", 15, 1);
        const bool locked = refused("offer", "trade", "your OK is in: the trade takes no more changes");
        check(not_yet && unsent && okd && twice && locked, "answer('trade'): the OK, refused outside state 3 and"
            " once in; the game not sending it an error; no change after it");
        g_offer.cancel_answer = 0;
        cancel("trade");
        const bool kept = refused("cancel", "trade", "the game did not send the cancel")
            && g_offer.closes[kOfferTrade] == 0 && live("trade");
        g_offer.cancel_answer = 1;
        cancel("trade");
        const bool canceled = quiet() && g_offer.cancels == 2 && g_offer.closes[kOfferTrade] == 1 && !live("trade");
        cancel("trade");
        const bool closed = refused("cancel", "trade", "trade is not open");
        offer("trade", 15, 1);
        const bool no_window = refused("offer", "trade", "trade is not open");
        GrayRows gone;
        const bool no_rows = g_e.gray_table.read(&gone) && !gone.open;
        check(kept && canceled && closed && no_window && no_rows, "cancel('trade'): the cancel, then the trade's"
            " windows closed once it went; the game not sending it an error, nothing closed; with no window every"
            " reply refused, and the gray test's rows for no window");

        // the NPC trade: local until its OK
        uint8_t* hand = controller("handover");
        open_now("handover");
        memset(hand + kHandoverEntries, 0, kOfferTable * kHandoverEntry);
        wr32(hand, kHandoverGil, 0);
        wr32(hand, kHandoverNpc, 5);
        wr32(hand, kHandoverNpcId, 0x01000005);
        clear_log();
        ok("handover");
        const bool nothing = refused("answer", "handover", "nothing is offered; offer an item or gil first");
        offer("handover", 1, 12);
        offer("handover", 2, 3);
        const bool two = quiet() && g_offer.stages[kOfferHandover] == 2 && g_offer.stage_slot == 2
            && hand[kHandoverEntries + 4 + 2] == 1 && hand[kHandoverEntries + 4 + 3] == 12
            && rd16(hand, kHandoverEntries + 8) == 4097 && hand[kHandoverEntries + 8 + 3] == 3;
        offer("handover", 1, 1);
        const bool hand_again = refused("offer", "handover", "that item is already offered");
        offer("handover", 8, 300);
        const bool too_many = refused("offer", "handover", "inventory slot 8 holds 300 of that item, fewer than 300");
        withdraw("handover", 1);
        const bool toggled = quiet() && g_offer.stage_slot == 1 && g_offer.stage_args[0] == 1
            && g_offer.stage_args[1] == 4096 && g_offer.stage_args[2] == 12 && rd16(hand, kHandoverEntries + 4) == 0;
        offer("handover", 0, 300);
        const bool hand_gil = quiet() && rd32(hand, kHandoverGil) == 300 && g_offer.stage_args[1] == kGilId;
        GrayRows hand_rows;
        const bool hand_read = g_e.gray_table.read(&hand_rows) && hand_rows.open
            && hand_rows.which == kOfferHandover && hand_rows.code[3] == kGrayTakes && hand_rows.code[4] == kGrayGrayed;
        const int hand_stages = g_offer.stages[kOfferHandover];
        g_offer.test_answer = kRowGray;
        offer("handover", 3, 1);
        const bool hand_gray = refused("offer", "handover", "that item cannot be handed to an NPC")
            && g_offer.stages[kOfferHandover] == hand_stages;
        g_offer.test_answer = kRowListed;
        check(nothing && two && hand_again && too_many && toggled && hand_gil && hand_read && hand_gray,
            "handover: nothing offered refuses the OK; offers into slots 1 and 2 by +0x40 and the stage; the same"
            " item again refused, and past 255 (its count a byte); withdraw writes +0x40 and stages the item again,"
            " its toggle-off; the gil through the stage's index 0; the gray test's rows for handover, and an item"
            " its mode grays refused on the game thread");
        {
            const OfferView& v = *view(kOfferHandover);
            check(v.open && v.target == 5 && v.gil == 300 && v.mine[2].id == 4097 && v.mine[2].index == 2
                    && v.mine[2].count == 3 && v.mine[1].id == 0 && v.wait == 0,
                "handover's read: its entries, the gil, the NPC; the wait for its own packet");
        }
        ok("handover");
        const bool handed = quiet() && g_offer.oks[kOfferHandover] == 1 && g_offer.closes[kOfferHandover] == 1
            && !live("handover");
        check(handed, "answer('handover'): the OK, then the window closed once it went");
        open_now("handover");
        wr32(hand, kHandoverNpc, 5);
        ok("handover");
        const bool hand_throttled = refused("answer", "handover", "the game sends one NPC trade per second")
            && g_offer.oks[kOfferHandover] == 1;
        later();
        wr32(hand, kHandoverNpc, 0);
        ok("handover");
        const bool no_npc = refused("answer", "handover", "handover has no NPC");
        wr32(hand, kHandoverNpc, 5);
        g_offer.ok_answer[kOfferHandover] = 0;
        ok("handover");
        const bool buzz = refused("answer", "handover", "the game did not send the NPC trade") && live("handover")
            && g_offer.closes[kOfferHandover] == 1;
        g_offer.ok_answer[kOfferHandover] = 1;
        cancel("handover");
        const bool hand_closed = quiet() && g_offer.closes[kOfferHandover] == 2 && !live("handover");
        check(hand_throttled && no_npc && buzz && hand_closed, "handover: an OK within the second refused with the"
            " wait, and with no NPC; the game not sending it an error, the window up; cancel('handover') its close");

        // blocked: handover's open refused, no window, every reply refused
        g_e.holds.set(h1, name("handover"), kHoldBlock, true);
        drain(g_e);
        hooked_open(g_mcb, NULL, "menu    handover", 1, 0);
        offer("handover", 2, 1);
        const bool blocked_offer = refused("offer", "handover", "handover is blocked and never opens: the NPC trade"
            " does not start, and nothing waits on a reply");
        ok("handover");
        const bool blocked_ok = refused("answer", "handover", "handover is blocked and never opens");
        g_e.holds.set(h1, name("handover"), kHoldBlock, false);
        drain(g_e);
        check(blocked_offer && blocked_ok && !live("handover"), "a blocked handover never opens: every reply refused");
        events(&cursor, ev, 64);
        clear_log();
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
        const bool hidden = menu && !draw_paints(menu) && row(327)[kRowLayer] == 2 && menu[kMenuLayer] == 2;
        g_e.holds.set(h1, d, kHoldHide, false);
        drain(g_e);
        check(hidden && draw_paints(menu) && row(327)[kRowLayer] == 2 && menu[kMenuLayer] == 2
                && memcmp(row(327), g_pristine + 327 * kRowStride, kRowStride) == 0,
            "hide dbdelsel: its draw paints nothing, row 327 and the live menu at layer 2; unhide paints it, the row as"
            " expected");
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
        const bool up = events(&cursor, ev, 64) == 0;
        wr32(live("equip"), kMenuState, kStateDormant);
        hook_show_pre(&f);
        wr32(live("equip"), kMenuState, 0);
        const int r = events(&cursor, ev, 64);
        check(up && r == 1 && is_event(ev[0], kEvUncovered, "equip") && ox("equip") == 1,
            "the show path on a menu already up posts nothing; on the dormant one, a reveal posts uncovered{equip}"
            " and does not re-apply the position");
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
                && strstr(g_e.error, "query cannot be canceled now"),
            "query_cancel while the cancel-allowed byte is 0: nothing written, nothing closed, a drain error says so");
        g_query_cancel_allowed = 1;
        errors = g_e.drain_errors;
        run(cmd(kOpQueryCancel, "query", 0, 0, s1, gen1));
        check(rd16(qc, kQueryResult) == 0xFF && g_log.count == 0 && g_close_calls == closes && live("query") == q
                && open_menu(g_e, kPlain, name("query"), NULL) && g_e.drain_errors == errors,
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
            && is_error(ev[0], s1, gen1, "answer", "query",
                "no option in the list has the value 2 (options('query') lists them)")
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
        open_now("post2");
        static const char* const closers[] = {"post1", "post2"};
        bool ended = true;
        for (size_t i = 0; i < sizeof(closers) / sizeof(closers[0]); ++i) {
            wr32(pc, kPostState, 3);
            clear_log();
            errors = g_e.drain_errors;
            run(cmd(kOpPostClose, closers[i], 0, 0, s1, gen1));
            ended = ended && strcmp(g_log.order, "BY") == 0 && g_log.post_command == 0x0F
                && rd32(pc, kPostState) == kPostClosing && g_e.drain_errors == errors;
        }
        check(ended && live("post2"),
            "post_close post1 and post2 with post2 live and post1 not: request 15, then post1's request-close sets"
            " 0x12, no error; post2's window, which has no tick, is left to the server's reply");
        run(cmd(kOpClose, "post2", 0, 0, s1, gen1));
        wr32(pc, kPostState, 0);
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
            && is_error(ev[0], s1, gen1, "answer", "link5", "slot 4 is not in the list (options('link5') lists them)");
        listed.value = 15;
        run(listed);
        check(refused && strcmp(g_log.order, "JL") == 0 && g_log.link5_slot == 15 && link5_lists(g_e, kPlain, lc, 3)
                && !link5_lists(g_e, kPlain, lc, 0),
            "a listed link5 answer: slot 4, which no row holds, refused with an error event; slot 15 answered;"
            " the header row's empty record lists no slot 0");
        wr32(lc, kLink5Count, kLink5RowMax + 1);
        const bool past = !link5_lists(g_e, kPlain, lc, 3) && !link5_lists(g_e, kGuarded, lc, 3);
        wr32(lc, kLink5Count, 3);
        check(past && link5_lists(g_e, kGuarded, lc, 3),
            "a link5 count past the 81 rows its array holds lists no slot, in either read mode");
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
        check(area_count(ac) == 45 && position && area_row_of(kPlain, ac, 4) == 0 && area_row_of(kPlain, ac, -1) == 1
                && area_row_of(kPlain, ac, 0) == 2 && area_row_of(kPlain, ac, -2) == 3
                && area_row_of(kPlain, ac, -4) == 5
                && area_row_of(kPlain, ac, -43) == 44 && area_row_of(kPlain, ac, 5) == -1,
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
        const bool regions = area_count(ac) == 42 && area_row_of(kPlain, ac, -4) == 2;
        memset(&fake_area::g_log, 0, sizeof(fake_area::g_log));
        clear_log();
        answer.value = static_cast<uint32_t>(-4);
        run(answer);
        n = events(&cursor, ev, 64);
        const bool region_refused = n == 1 && is_error(ev[0], s1, gen1, "answer", "arealist",
            "the row with the id -4 is not a zone; the answer takes a zone row's id");
        answer.value = 6;
        run(answer);
        n = events(&cursor, ev, 64);
        check(regions && region_refused && n == 1
                && is_error(ev[0], s1, gen1, "answer", "arealist",
                    "no row of the list has the id 6 (options('arealist') lists them)")
                && g_log.count == 0 && fake_area::g_log.closes == 0 && ac[kAreaLatch] == 1 && live("arealist") == am,
            "mode 1, grouped: 42 region rows; answer -4 (a region) and 6 (no row at this level) refused with error"
            " events, nothing called, the list open");
        fake_area::select_index(ac, 2, 0);
        fake_area::input(ac, NULL, 5, 0);
        const bool drilled = rd16(ac, kAreaLevel) == 4 && area_row_of(kPlain, ac, 6) == 2 && ac[kAreaLatch] == 1;
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
        const bool out_of_range = n == 1
            && is_error(ev[0], s1, gen1, "answer", "arealist", "the area list is blocked, so there are no rows to"
                " check 600 against; the answer takes a zone id, 0..511")
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
        const bool flat = area_count(ac) == 84 && area_row_of(kPlain, ac, 1) == 0;
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

    // the player's input on a hidden prompt, read at the input: held while
    // the prompt asks, by the rule its replies and options() go by, and
    // taken as the shown window takes it any other time
    {
        auto hide = [&](const char* nm, bool on) {
            g_e.holds.set(h1, name(nm), kHoldHide, on);
            drain(g_e);
        };

        // stringdl, as found live: held from the first key after a hidden
        // open while the outgoing box asks the name through it, and while
        // the box has not read the name answered; once the box has moved
        // on, stringdl, focused again and asking nothing, takes the player's
        // keys, the Escape that leaves it among them, until the box asks
        // through it again
        uint8_t* dc = controller("delivery");
        uint8_t* sd = controller("stringdl");
        wr16(dc, kDeliveryState, kDeliveryAsksName);
        sd[kStringdlMode] = kStringdlAsksName;
        wr16(sd, kStringdlReply, 0);
        hide("stringdl", true);
        uint8_t* entry = open_now("stringdl");
        const bool first = entry && input_on(entry) == 0;
        drain(g_e);
        TextAnswer& given = g_e.text_answers[kTextStringdl];
        memset(&given, 0, sizeof(given));
        given.yes = -1;
        snprintf(given.text, sizeof(given.text), "Rolanberry");
        Command named = cmd(kOpTextAnswer, "stringdl", 0, 0, s1, gen1);
        named.verb = kVerbAnswer;
        run(named);
        const bool unread = rd16(sd, kStringdlReply) == kStringdlNamed && input_on(entry) == 0;
        wr16(sd, kStringdlReply, 0);
        wr16(dc, kDeliveryState, kDeliveryIdle);
        const bool moved_on = input_on(entry) == 1;
        wr16(dc, kDeliveryState, kDeliveryAsksName);
        const bool asks_again = input_on(entry) == 0;
        check(first && unread && moved_on && asks_again,
            "hidden stringdl: held while the outgoing box asks the name through it, from the first key after its"
            " open with no drain between, and while the box has not read the name answered; with the box moved on"
            " it asks nothing and the routed key and the menu mouse mode reach it as shown; held again once the box"
            " asks through it, with no drain between");
        game_closes("stringdl");
        wr16(dc, kDeliveryState, 0);
        sd[kStringdlMode] = 0;
        hide("stringdl", false);

        // the yes/no class: comyn opened hidden by its ask, held from its
        // first key; with its question cleared and the window still up it
        // asks nothing
        hide("comyn", true);
        uint8_t* yn = yesno_ask("comyn", "Log out?", false, true, 0) ? live("comyn") : NULL;
        const bool yn_first = yn && input_on(yn) == 0;
        uint8_t* ync = controller("comyn");
        const uint32_t asked_cb = rd32(ync, kYesNoCallback);
        wr32(ync, kYesNoCallback, 0);
        const bool yn_none = input_on(yn) == 1;
        wr32(ync, kYesNoCallback, asked_cb);
        const bool yn_again = input_on(yn) == 0;
        game_closes("comyn");
        hide("comyn", false);
        check(yn_first && yn_none && yn_again,
            "hidden comyn opened by its ask: held from the first key, no drain between; with no question pending in"
            " it, the key and the menu mouse mode reach it as shown; held again once a question is");

        // passinpu while its callback is set, prtyjoin while an invite is
        // pending, link5 while an event waits on its linkshell choice (the
        // lists the other openers put in it are the player's), a number box
        // while its callback is set
        uint8_t* pc = controller("passinpu");
        uint8_t* lc = controller("link5");
        uint8_t* mc = controller("moneyctr");
        const char* const kinds[] = {"passinpu", "prtyjoin", "link5", "moneyctr"};
        uint8_t* up[4];
        for (int i = 0; i < 4; ++i) {
            hide(kinds[i], true);
        }
        wr32(pc, kPassCallback, 0);
        g_log.party_pending = 0;
        g_link5_latch = 1;
        wr32(lc, kLink5Mode, 0);
        wr32(lc, kLink5Callback, 0);
        wr32(mc, kBoxLayouts[kBoxMoney].callback, 0);
        bool idle = true;
        for (int i = 0; i < 4; ++i) {
            up[i] = open_now(kinds[i]);
            idle = idle && up[i] && input_on(up[i]) == 1;
        }
        wr32(pc, kPassCallback, reinterpret_cast<uint32_t>(&fake_text_callback));
        g_log.party_pending = 1;
        wr32(lc, kLink5Mode, 1);
        wr32(lc, kLink5Callback, reinterpret_cast<uint32_t>(&fake_link5_callback));
        wr32(mc, kBoxLayouts[kBoxMoney].callback, reinterpret_cast<uint32_t>(kAskerCallbacks[kAskShopBuy]));
        bool asking = true;
        for (int i = 0; i < 4; ++i) {
            asking = asking && input_on(up[i]) == 0;
        }
        wr32(pc, kPassCallback, 0);
        g_log.party_pending = 0;
        g_link5_latch = 0;
        wr32(lc, kLink5Mode, 0);
        wr32(lc, kLink5Callback, 0);
        wr32(mc, kBoxLayouts[kBoxMoney].callback, 0);
        for (int i = 0; i < 4; ++i) {
            game_closes(kinds[i]);
            hide(kinds[i], false);
        }
        check(idle && asking,
            "hidden passinpu with no callback, prtyjoin with no invite, link5 holding another opener's list (latch"
            " 1, mode 0) and moneyctr with no callback ask nothing and take the key and the menu mouse mode as"
            " shown; with the entry's callback, the invite, the event's choice and the box's callback, each is"
            " held");

        // arealist only while an NPC event waits on its choice (modes 1
        // and 2): the search's list and the others are the player's keys'
        uint8_t* ac = controller("arealist");
        hide("arealist", true);
        fake_area::prepare(ac, 0, false);
        uint8_t* am = open_now("arealist");
        bool by_mode = am != NULL;
        for (int mode = 0; mode <= 4; ++mode) {
            fake_area::prepare(ac, mode, false);
            fake_area::opened(ac);
            by_mode = by_mode && input_on(am) == (area_event_mode(mode) ? 0 : 1);
        }
        fake_area::prepare(ac, 1, false);
        ac[kAreaLatch] = 0;
        const bool unlatched = input_on(am) == 1;
        game_closes("arealist");
        ac[kAreaLatch] = 0;
        ac[kAreaMode] = 0;
        hide("arealist", false);
        check(by_mode && unlatched,
            "hidden arealist: held in an NPC event's modes 1 and 2; in modes 0, 3 and 4, and in mode 1 with its"
            " latch clear, the key and the menu mouse mode reach it as shown");

        // the prompts that ask whenever they are up: options() reads query,
        // trade and handover only while open, and nothing of delivery and
        // the post boxes
        const char* const while_up[] = {"query", "trade", "handover", "delivery", "post1", "post2"};
        bool held_up = true;
        for (size_t i = 0; i < sizeof(while_up) / sizeof(while_up[0]); ++i) {
            hide(while_up[i], true);
            uint8_t* m = open_now(while_up[i]);
            held_up = held_up && m && hidden_prompt(g_e, m) && input_on(m) == 0;
            game_closes(while_up[i]);
            hide(while_up[i], false);
        }
        check(held_up, "hidden query, trade, handover, delivery, post1 and post2, each up and active: held, as before");
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
        // ability's close closed subwindo too, its pane with no other parent open
        if (!live("subwindo")) {
            open_now("subwindo");
        }

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
    // busy byte no bar; the game's reply and tick then close it. With post1's
    // window not live, post2's or not, the window-less close
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
        open_now("post2");
        static const char* const enders[] = {"post1", "post2"};
        bool ended = true;
        for (size_t i = 0; i < sizeof(enders) / sizeof(enders[0]); ++i) {
            wr32(pc, kPostState, 3);
            clear_log();
            errors = g_e.drain_errors;
            run(cmd(kOpPostEnd, enders[i], 0, 0, s1, gen1));
            ended = ended && strcmp(g_log.order, "BY") == 0 && g_log.post_command == 0x0F
                && rd32(pc, kPostState) == kPostClosing && g_e.drain_errors == errors;
        }
        check(ended && live("post2"),
            "with post2 live and post1 not, for post1 and post2: request 15, then post1's request-close sets 0x12, no"
            " error; post2's window, which has no tick, is left to the server's reply");
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
                && qm[kMenuPendingDestroy] == 0 && !open_menu(g_e, kPlain, q, NULL),
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
        check(again == qm && n == 1 && is_event(ev[0], kEvOpened, "query") && open_menu(g_e, kPlain, q, NULL) == qm
                && g_e.drain_errors == errors && rd16(qc, kQueryResult) == 4,
            "the game opens query again before the closed instance is freed: the open reuses it with both marks"
            " cleared, opened{query}, and it is open and answered");

        const int eqn = name("equip");
        uint8_t* eq = live("equip") ? live("equip") : open_now("equip");
        events(&cursor, ev, 64);
        cover(eq);
        n = events(&cursor, ev, 64);
        drain(g_e);
        const bool covered_open = n == 1 && is_event(ev[0], kEvCovered, "equip")
            && open_menu(g_e, kPlain, eqn, NULL) == eq
            && events(&cursor, ev, 64) == 0;
        game_closes("equip");
        const bool marked = events(&cursor, ev, 64) == 0 && eq[kMenuClosing] == 1 && eq[kMenuPendingDestroy] == 1
            && live("equip") == eq;
        drain(g_e);
        n = events(&cursor, ev, 64);
        const bool noticed = n == 1 && is_event(ev[0], kEvClosed, "equip") && !g_e.covered[eqn] && live("equip") == eq
            && !open_menu(g_e, kPlain, eqn, NULL);
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
    // 100 up puts its origin 100 up and pins it there; at its next open,
    // built at its template size, it goes back on that origin, its bottom
    // where the move put it, and a remembered size hangs from that bottom
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
                && lm.x == 16 && lm.y == 798 && lm.by_frame && lm.pinned && lm.pinned_y == 830,
            "move logwindo by its frame's top-left: to its own 16,898 nothing changes; to 16,798 its origin goes to"
            " 830, frame 798..964; remembered as given, 16,798, by the frame, the origin 830 pinned");
        run(cmd(kOpClose, "logwindo", 0, 0, s1, gen1));
        log = open_now("logwindo");
        check(ox("logwindo") == 16 && oy("logwindo") == 830 && rect_is(log, 16, 830, 382, 964) && lm.y == 798
                && lm.pinned_y == 830,
            "logwindo reopened at its template's 134 of height: on the pinned origin 830, its bottom 964 where the"
            " move put it (830..964), not converted again to 798..932; still remembered as given, 798");
        run(cmd(kOpResizeRect, "logwindo", 1000, 120, s1, gen1));
        run(cmd(kOpClose, "logwindo", 0, 0, s1, gen1));
        clear_log();
        log = open_now("logwindo");
        check(strcmp(g_log.order, "FF") == 0 && oy("logwindo") == 830 && rect_is(log, 16, 844, 1016, 964),
            "reopened with a remembered 1000x120: the pinned origin 830, then the size hung from its bottom 964"
            " (844..964)");
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
                && pm.x == 1700 && pm.y == 800 && pm.pinned && pm.pinned_y == 840 && !g_e.waiting.g[1].active,
            "partywin opens with a remembered 120x100: the size first, then the waiting move puts its frame top at 800"
            " (origin 840 = 800 + 100 - 60); remembered as given, by the frame, the origin 840 pinned");
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
        const uint32_t before = current_session(g_e, kPlain, kSessionInvite);
        g_log.party_pending = 1;
        const uint32_t unseen = current_session(g_e, kPlain, kSessionInvite);
        drain(g_e);
        const uint32_t seen = current_session(g_e, kPlain, kSessionInvite);
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
        const uint32_t next = current_session(g_e, kPlain, kSessionInvite);
        a.instance = next;
        clear_log();
        run(a);
        check(next == seen + 1 && strcmp(g_log.order, "PX") == 0,
            "a reply naming an invite the drain had not seen yet: the drain numbers it before its commands, and sends"
            " it");
        drain(g_e);
        uint8_t* dc = controller("delivery");
        const uint32_t post = current_session(g_e, kPlain, kSessionPost);
        wr16(dc, kDeliveryState, 5);
        const uint32_t up = current_session(g_e, kPlain, kSessionPost);
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
        check(live("equip") && open_menu(g_e, kPlain, name("equip"), NULL) && n == 1
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

    // the template swap: a swap of the game's that rewrites the frame puts a
    // window a handle moved back at once, its position alone, the size the
    // swap's; the party list by its bottom edge; a swap that rewrites
    // nothing, one inside the engine's own resize, and one inside an open of
    // the window re-apply nothing
    {
        static const char* const placed[] = {"equip", "playermo", "partywin", "ability", "buff"};
        for (size_t i = 0; i < sizeof(placed) / sizeof(placed[0]); ++i) {
            run(cmd(kOpReset, placed[i], 0, 0, s1, gen1));
            if (!live(placed[i])) {
                open_now(placed[i]);
            }
        }
        g_e.game.template_swap = &hooked_template_swap;
        events(&cursor, ev, 64);
        // The game's swap: the daemon's pre, the swap, its post.
        auto game_swap = [&](const char* nm, const char* stem) {
            char key[32];
            memset(key, 0, sizeof(key));
            memcpy(key, "menu    ", 8);
            memset(key + 8, ' ', 8);
            memcpy(key + 8, stem, strlen(stem));
            hooked_template_swap(live(nm), NULL, key);
        };
        const LONG errors = g_e.drain_errors;

        // moved, no size: the position back, nothing else
        run(cmd(kOpMove, "equip", 300, 200, s1, gen1));
        clear_log();
        int calls = g_setpos_calls;
        game_swap("equip", "equip2");
        const bool moved = strcmp(g_log.order, "S") == 0 && g_setpos_calls == calls + 1 && ox("equip") == 300
            && oy("equip") == 200 && rd16(live("equip"), kMenuRect) == 300 && rd16(live("equip"), kMenuRect + 2) == 200;
        // a swap that rewrites no frame, as the game's of the template a
        // window has
        clear_log();
        calls = g_setpos_calls;
        {
            uint32_t args[1] = {0};
            HuFrame f;
            memset(&f, 0, sizeof(f));
            f.ecx = reinterpret_cast<uint32_t>(live("equip"));
            f.args = args;
            f.user = &g_e;
            hook_swap_pre(&f);
            f.ecx = 0;
            hook_swap_post(&f);
        }
        const bool same = g_log.count == 0 && g_setpos_calls == calls;
        // a window no handle placed
        clear_log();
        calls = g_setpos_calls;
        game_swap("buff", "buff2");
        const bool unplaced = strcmp(g_log.order, "S") == 0 && g_setpos_calls == calls
            && rd16(live("buff"), kMenuRect) == 7 && rd16(live("buff"), kMenuRect + 2) == 9;
        check(moved && same && unplaced && g_e.drain_errors == errors && g_e.swap_guard == 0 && !g_e.swap_menu,
            "a swap of the game's that rewrites a moved window's frame: SetPosition back to the remembered 300,200"
            " at once, nothing else; a swap that rewrites no frame and one of a window no handle placed re-apply"
            " nothing");

        // the party list: back to the bottom edge the move gave it, grown
        // upward to the swap's height, as the game grows it
        Command pmove = cmd(kOpMove, "partywin", 600, 300, s1, gen1);
        pmove.flags |= kCmdFrame;
        run(pmove);
        uint8_t* party = live("partywin");
        const int party_bottom = rd16(party, kMenuRect + 6);
        const int party_origin = oy("partywin");
        clear_log();
        calls = g_setpos_calls;
        const int swaps = g_swap_hooks;
        game_swap("partywin", "ptw5");
        const int grown = rd16(party, kMenuRect + 6) - rd16(party, kMenuRect + 2);
        const bool bottom = strcmp(g_log.order, "SF") == 0 && memcmp(g_log.swap_key, "menu    ptw5    ", 16) == 0
            && g_swap_hooks == swaps + 1 && g_setpos_calls == calls + 1 && rd16(party, kMenuRect) == 600
            && rd16(party, kMenuRect + 6) == party_bottom && oy("partywin") == party_origin
            && g_log.frame[0] == 600 && g_log.frame[1] == party_bottom - grown && grown == party_bottom - 9;
        check(bottom && g_e.drain_errors == errors,
            "partywin moved by its frame to 600,300: the roster's swap to ptw5 is followed by SetPosition on the"
            " origin the move gave it and SetFrameRect hanging the swap's taller frame from the same bottom edge,"
            " its left at 600; no swap of the engine's");

        // a window sized by a handle: the swap's size stands; moved too,
        // its position alone goes back
        run(cmd(kOpResizeRows, "playermo", 0, 0, s1, gen1, 4));
        uint8_t* pmo = live("playermo");
        clear_log();
        calls = g_setpos_calls;
        const int before = g_swap_hooks;
        game_swap("playermo", "actionm6");
        const bool sized = strcmp(g_log.order, "S") == 0 && memcmp(g_log.swap_key, "menu    actionm6", 16) == 0
            && g_swap_hooks == before + 1 && g_setpos_calls == calls && g_e.swap_guard == 0;
        run(cmd(kOpMove, "playermo", 40, 60, s1, gen1));
        const int items = rd16(pmo, kMenuItems);
        clear_log();
        calls = g_setpos_calls;
        game_swap("playermo", "actionm5");
        const bool placed_back = strcmp(g_log.order, "S") == 0 && g_setpos_calls == calls + 1 && ox("playermo") == 40
            && oy("playermo") == 60 && rd16(pmo, kMenuItems) == items && g_e.swap_guard == 0;
        clear_log();
        calls = g_setpos_calls;
        run(cmd(kOpResizeRows, "playermo", 0, 0, s1, gen1, 6));
        const bool own = strcmp(g_log.order, "USU") == 0 && memcmp(g_log.swap_key, "menu    actionm6", 16) == 0
            && g_setpos_calls == calls + 1 && g_e.swap_guard == 0;
        check(sized && placed_back && own && g_e.drain_errors == errors,
            "playermo sized 4 rows: its owner's swap to actionm6 stands, nothing re-applied; moved to 40,60, the"
            " owner's next swap is followed by SetPosition back there alone, the item count the owner's; the"
            " engine's own resize through the hooked swap places it once, no re-dock (the move undocked it), its"
            " swap's hooks re-applying nothing");

        // inside an open of the window, the open's post re-applies it
        {
            char key[kKeyLen + 1];
            make_key(g_e, name("equip"), key);
            uint32_t args[3] = {reinterpret_cast<uint32_t>(key), 1, 1};
            int16_t popped = -1;
            g_e.opens.push(args, GetCurrentThreadId(), static_cast<int16_t>(name("equip")));
            clear_log();
            calls = g_setpos_calls;
            game_swap("equip", "equip2");
            const bool deferred = strcmp(g_log.order, "S") == 0 && g_setpos_calls == calls;
            check(g_e.opens.pop(args, GetCurrentThreadId(), &popped) && popped == name("equip") && deferred,
                "a swap inside an open of the window re-applies nothing: the open's post does it");
        }
        g_e.game.template_swap = &fake_template_swap;
        for (size_t i = 0; i < sizeof(placed) / sizeof(placed[0]); ++i) {
            run(cmd(kOpReset, placed[i], 0, 0, s1, gen1));
        }
        events(&cursor, ev, 64);
    }

    // a zone change (measured 2026-10-18: partywin 112x34 moved by its frame
    // to 900,500 came back at 900,600), on the stand-in's template (500,380,
    // 120x60, rows 20 tall): the party list moved by its frame while one
    // member tall keeps the bottom edge the move gave it when the game
    // closes it, opens it again at its template's full height and swaps it
    // to its roster; the roster's frame hangs from that bottom shrunk to one
    // member and grown to two, and the memory keeps the frame's top-left as
    // given
    {
        run(cmd(kOpReset, "partywin", 0, 0, s1, gen1));
        uint8_t* party = live("partywin") ? live("partywin") : open_now("partywin");
        wr16(party, kMenuOrigin, 500);
        wr16(party, kMenuOrigin + 2, 380);
        set_rect(party, 500, 420, 620, 440);
        // The roster's swap as the game's: the frame rewritten at the
        // template's spot, `members` rows tall on its bottom 440.
        auto roster = [&](int members) {
            uint8_t* menu = live("partywin");
            uint32_t args[1] = {0};
            HuFrame f;
            memset(&f, 0, sizeof(f));
            f.ecx = reinterpret_cast<uint32_t>(menu);
            f.args = args;
            f.user = &g_e;
            hook_swap_pre(&f);
            set_rect(menu, 500, 440 - 20 * members, 620, 440);
            f.ecx = 0;
            hook_swap_post(&f);
        };
        const LONG errors = g_e.drain_errors;
        Command m = cmd(kOpMove, "partywin", 900, 500, s1, gen1);
        m.flags = kCmdFrame;
        run(m);
        const MemEntry& pm = g_e.memory.e[name("partywin")];
        const bool moved = ox("partywin") == 900 && oy("partywin") == 460 && rect_is(party, 900, 500, 1020, 520)
            && pm.pinned && pm.pinned_y == 460;
        run(cmd(kOpClose, "partywin", 0, 0, s1, gen1));
        const bool closed = !live("partywin");
        party = open_now("partywin");
        const bool reopened = ox("partywin") == 900 && oy("partywin") == 460 && rect_is(party, 900, 460, 1020, 520);
        check(moved && closed && reopened && g_e.drain_errors == errors,
            "partywin one member tall (420..440) moved by its frame to 900,500: origin 460 (500 + 20 - 60) pinned,"
            " frame 500..520; closed and opened again at its template's full 60: on the pinned origin, its bottom at"
            " 520 where the move put it (460..520), not converted again to 500..560");
        roster(1);
        const bool shrunk = ox("partywin") == 900 && oy("partywin") == 460 && rect_is(party, 900, 500, 1020, 520);
        roster(2);
        const bool grown = ox("partywin") == 900 && oy("partywin") == 460 && rect_is(party, 900, 480, 1020, 520);
        check(shrunk && grown && pm.active && pm.by_frame && pm.x == 900 && pm.y == 500 && g_e.drain_errors == errors,
            "its roster's swap down to one member: the frame back at 500..520, its top 500 where the move asked; up to"
            " two: 480..520, the bottom fixed, the top moving up; remembered as given, 900,500, by the frame");
        run(cmd(kOpReset, "partywin", 0, 0, s1, gen1));
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
            " segment, the color codes out of the text");

        static const int16_t codes[] = {0x00, 0x5F, 0x60, 0x7FFF, -1, -0xFF, -0x100, 0x22, -0x1FF, 0x23,
                                        -0x200, 0x24, -0x279, 0x25, -0x8000, 0x26};
        memcpy(g, codes, sizeof(codes));
        runs = decoded_runs(g, sizeof(codes) / 2, &t);
        check(strcmp(t.text, " \x7F??BCDEF") == 0 && t.undecoded == 2 && t.run_count == 6
                && strcmp(runs, " \x7F??" "/1E:1|B/1E:0|C/1E:FF|D/1F:0|E/1F:79|F/1F:7E00") == 0,
            "glyphs at every edge: 0 and 0x5F are 0x20 and 0x7F, 0x60 and up a counted ?, -1 and -0xFF not"
            " drawn, -0x100..-0x1FF color 1E 00..FF, -0x200 and below 1F nn");

        static const int16_t merges[] = {-0x102, -0x105, -0x101, 0x41, -0x101, 0x42, -0x102};
        memcpy(g, merges, sizeof(merges));
        runs = decoded_runs(g, sizeof(merges) / 2, &t);
        check(strcmp(t.text, "ab") == 0 && strcmp(runs, "ab/1E:1") == 0,
            "glyphs: colors with no text under them make no segment; a color that changes nothing splits none");

        {
            // A dialogue page: each line from the default color, a '\n'
            // between two lines, a line past kLineGlyphs read as kLineGlyphs.
            static int16_t first[8];
            static int16_t second[8];
            static int16_t long_line[kLineGlyphs + 8];
            int a = put_text(first, 0, "Ko : ");
            first[a++] = -0x102;
            a = put_text(first, a, "Hi");
            int b = put_text(second, 0, "you");
            for (int i = 0; i < kLineGlyphs + 8; ++i) {
                long_line[i] = 0x21;
            }
            const uint8_t* lines[3] = {reinterpret_cast<const uint8_t*>(first),
                                       reinterpret_cast<const uint8_t*>(second),
                                       reinterpret_cast<const uint8_t*>(long_line)};
            const int counts[3] = {a, b, kLineGlyphs + 8};
            static PageText page;
            decode_page(lines, counts, 2, NULL, &page);
            char out[128] = "";
            for (int i = 0; i < page.run_count; ++i) {
                char one[40];
                snprintf(one, sizeof(one), "%s%.*s/%X", i ? "|" : "", page.runs[i].length,
                    page.text + page.runs[i].start, page.runs[i].color);
                strncat(out, one, sizeof(out) - strlen(out) - 1);
            }
            const bool joined = strcmp(page.text, "Ko : Hi\nyou") == 0 && strcmp(out, "Ko : /1|Hi/2|\nyou/1") == 0
                && !page.truncated && page.undecoded == 0;
            decode_page(lines, counts, 3, NULL, &page);
            check(joined && strlen(page.text) == 12 + kLineGlyphs && page.text[11] == '\n' && !page.truncated,
                "decode_page: a page's lines joined by '\\n', each from the default color, the newline in the color"
                " the next line starts with; a line read up to kLineGlyphs codes");
        }

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

    // -- settings: kSettings, the store's reads and the drain's sets
    {
        static const char* const pages[] = {"conf2win", "cfilter", "conftxtc", "conf5win", "conf5w1", "conf5w2",
                                            "conf3win", "conf6win", "k1assign", "k2assign", "fxfilter", "conf7",
                                            "conf4", "conf11s", "conf12wi", "conf13wi"};
        static const int per_page[16] = {8, 53, 28, 4, 5, 5, 7, 7, 30, 29, 18, 7, 2, 56, 7, 2};
        int counted[16] = {0};
        int kinds[5] = {0};
        int page = 0;
        bool ordered = true;
        bool names = true;
        bool unique = true;
        bool shaped = true;
        for (int i = 0; i < kSettingCount; ++i) {
            const SettingSpec& s = kSettings[i];
            while (page < 16 && strcmp(s.page, pages[page]) != 0) {
                ++page;
            }
            ordered = ordered && page < 16 && name(s.page) >= 0;
            if (page < 16) {
                ++counted[page];
            }
            ++kinds[s.value];
            for (const char* c = s.name; *c; ++c) {
                names = names && ((*c >= 'a' && *c <= 'z') || (*c >= '0' && *c <= '9') || *c == '_');
            }
            names = names && strstr(s.name, "_row") == NULL;
            for (int k = 0; k < i; ++k) {
                unique = unique && strcmp(kSettings[k].name, s.name) != 0;
            }
            shaped = shaped && (s.value == kValueChoice) == (s.choices != NULL)
                && (s.label >= 0) == (s.from != kFromNone && s.store != kStoreWord ? true : s.from == kFromColors
                    || s.from == kFromKeys)
                && ((s.flags & kCommits) != 0) == (s.from == kFromFilter || s.from == kFromEffects
                    || s.from == kFromRouting || s.from == kFromKeys || s.store == kStoreFlag);
        }
        bool counts = true;
        for (int p = 0; p < 16; ++p) {
            counts = counts && counted[p] == per_page[p];
        }
        check(kSettingCount == 268 && ordered && counts && names && unique && shaped,
            "settings: 268, each page's together in the Config menu's order (conf2win, cfilter, the text colors,"
            " conf5win, conf5w1, conf5w2, conf3win, conf6win, k1assign, k2assign, fxfilter, conf7, conf4, conf11s,"
            " conf12wi, conf13wi), each page a registry window; names unique, lowercase snake_case ASCII, none a"
            " page's row; choices exactly for a choice; a label for each row of a game's table; the pages that"
            " write a copy back as they close marked so");
        check(kinds[kValueNumber] == 31 && kinds[kValueBool] == 130 && kinds[kValueChoice] == 20
                && kinds[kValueColor] == 28 && kinds[kValueKey] == 59,
            "settings: 31 numbers, 130 bools, 20 choices, 28 colors and 59 keys");
        static const char* const inverted[] = {"auto_target", "window_effect", "log_window1_reactive_sizing",
                                               "log_window2_reactive_sizing", "screen_edge_panning",
                                               "chat_language_filter", "weapon_effect", "footstep_effects",
                                               "target_expressions", "furniture_camera_collision"};
        int inverted_found = 0;
        for (int i = 0; i < kSettingCount; ++i) {
            bool listed = false;
            for (size_t k = 0; k < sizeof(inverted) / sizeof(inverted[0]); ++k) {
                listed = listed || strcmp(kSettings[i].name, inverted[k]) == 0;
            }
            inverted_found += listed && kSettings[i].value == kValueBool && (kSettings[i].flags & kInverted) ? 1 : 0;
            inverted_found -= !listed && (kSettings[i].flags & kInverted) ? 100 : 0;
        }
        const SettingSpec& icon = kSettings[find_setting("icon_type")];
        const SettingSpec& lock_spec = kSettings[find_setting("style_lock")];
        const SettingSpec& info = kSettings[find_setting("character_information")];
        check(inverted_found == 10 && icon.value == kValueChoice && icon.store == kStoreBit && icon.id == 0x99
                && icon.bit == 2 && strcmp(icon.choices[0].name, "type1") == 0 && icon.choices[0].value == 1
                && icon.choices[1].value == 0 && lock_spec.value == kValueChoice && lock_spec.store == kStoreLock
                && strcmp(lock_spec.choices[1].name, "static") == 0 && lock_spec.choices[1].value == 1
                && info.value == kValueChoice && info.store == kStoreFlag && strcmp(info.choices[1].name, "hide") == 0
                && info.choices[1].value == 1,
            "the ten bools whose page's ON is a stored 0 inverted, and no other; icon_type a choice on 0x99's bit 2"
            " (type1 the bit set), style_lock one through the equipment lock's request (static 1),"
            " character_information one on the player's flag (hide 1)");
        size_t longest = 0;
        for (int i = 0; i < kSettingCount; ++i) {
            longest = strlen(kSettings[i].name) > longest ? strlen(kSettings[i].name) : longest;
        }
        for (int g = 0; g < kGroupCount; ++g) {
            longest = strlen(kGroups[g].name) > longest ? strlen(kGroups[g].name) : longest;
        }
        for (int k = 0; k < kSetCount; ++k) {
            longest = strlen(kSets[k].name) > longest ? strlen(kSets[k].name) : longest;
        }
        check(longest == 43 && longest < sizeof(ErrorRecord().name) && kNameMax < sizeof(ErrorRecord().name),
            "the longest setting, group or set name, 43 characters, and every window name fit an error record whole");
        const int music = find_setting("music_volume");
        check(music >= 0 && find_setting("MUSIC_Volume") == music && find_setting("music_volume_") < 0
                && find_setting("") < 0 && find_setting(NULL) < 0,
            "find_setting: a name in any case, nothing else");
        check(pack_color(160, 64, 160) == 0x80A040A0u && pack_color(0, 0, 0) == 0x80000000u
                && pack_color(255, 255, 255) == 0x80FFFFFFu && color_red(0x80A040A0u) == 160
                && color_green(0x80A040A0u) == 64 && color_blue(0x80A040A0u) == 160,
            "colors: 0x80, then red, green and blue, as the game's editor packs them, and back");
        check(bit_word(0xB9, 33) == 0xBA && bit_mask(33) == 2u && bit_word(0x92, 17) == 0x92 && bit_mask(17) == 1u << 17,
            "a bit from a word on: bit 33 is the next word's bit 1");

        // The chat filter's rows, by their state: Say (a mask bit and a
        // second state), Tell (a second state and the flag), Party (a
        // second state alone) and Yell (a mask bit alone, in the second
        // mask word).
        const FilterRow say = filter_row(0x00, 0x00);
        const FilterRow tell = filter_row(kFilterNoBit, 2);
        const FilterRow party = filter_row(kFilterNoBit, 3);
        const FilterRow yell = filter_row(0x2E, kFilterOneState);
        FilterWords w = {{0x3FFFFFFEu, 0x3FFFFFFFu}, 0xFFFFFFFEu, 0};
        const FilterWords start = w;
        filter_compose(say, 1, &w);
        const bool say1 = filter_state(say, w) == 1 && w.masks[0] == 0x3FFFFFFFu && w.second == 0xFFFFFFFEu;
        filter_compose(say, 2, &w);
        const bool say2 = filter_state(say, w) == 2 && w.masks[0] == 0x3FFFFFFEu && w.second == 0xFFFFFFFFu;
        filter_compose(say, 0, &w);
        check(filter_states(say) == 3 && say1 && say2 && filter_state(say, w) == 0 && w.masks[0] == start.masks[0]
                && w.masks[1] == start.masks[1] && w.second == start.second && w.flag == 0,
            "a chat filter row of three states: 1 its mask bit, 2 its second state's, 0 neither, every other bit"
            " kept");
        FilterWords t = {{0, 0}, 0, 0};
        bool cycle = filter_states(tell) == 4;
        for (int s = 0; s < 4; ++s) {
            filter_compose(tell, s, &t);
            cycle = cycle && filter_state(tell, t) == s && (t.second == ((s & 1) ? 4u : 0u)) && t.flag == (s >> 1 ? 1u : 0u);
        }
        check(cycle && tell.flagged && tell.mask < 0 && tell.second == 2,
            "Tell's row: four states, its second state's bit (bit 2 of 0x34) and the flag word 0x47, in the order its"
            " confirm cycles");
        FilterWords p = {{0, 0}, 0x10, 0};
        filter_compose(party, 1, &p);
        const bool party1 = filter_state(party, p) == 1 && p.second == 0x18;
        FilterWords y = {{0, 0}, 0, 0};
        filter_compose(yell, 1, &y);
        check(filter_states(party) == 2 && party1 && filter_states(yell) == 2 && y.masks[0] == 0
                && y.masks[1] == 1u << 16 && filter_state(yell, y) == 1,
            "a row of a second state alone takes 0 and 1; one of a mask bit alone too, bit 46 the second mask word's"
            " bit 16 (30 bits to a word)");

        // The chat filter's row table as the resolution reads it: a mask bit
        // past the two mask words, or a second state's bit past its word,
        // refuses the settings with the row named.
        {
            AskedInputs none;
            memset(&none, 0, sizeof(none));
            SettingSites rows_only;
            memset(&rows_only, 0, sizeof(rows_only));
            uint8_t table[4 * kFilterRows + 12];
            memset(table, 0, sizeof(table));
            rows_only.filter_rows = table;
            const int say_i = find_setting("filter_say");
            SettingSlot slot;
            char buf[192];
            auto row = [&](uint8_t bit, uint8_t sub) {
                table[4 * kSettings[say_i].row] = bit;
                table[4 * kSettings[say_i].row + 1] = sub;
                return setting_row(none, rows_only, say_i, &slot, buf, sizeof(buf));
            };
            const bool edges = row(59, 31) == NULL && slot.row.mask == 59 && slot.row.second == 31;
            const char* mask_past = row(60, 0);
            const bool mask_refused = mask_past && strstr(mask_past, "the chat filter's row 1 is not a bit of its words")
                == mask_past;
            const char* second_past = row(0, 32);
            const bool second_refused = second_past
                && strstr(second_past, "the chat filter's row 1 is not a bit of its words") == second_past;
            const char* high = row(0x90, 0);
            check(kSettings[say_i].from == kFromFilter && edges && mask_refused && second_refused && high,
                "a chat filter row: mask bit 59 and second state 31 taken; mask bit 60 (past two words of 30), 0x90"
                " and second state 32 (past its 32-bit word) refused at resolution, the row named");
        }

        // A vtable's slot read only where the data section holding the
        // vtable holds it: a slot past the section's end gives no routine,
        // whatever lies there.
        {
            static uint8_t code[16];
            static uint32_t data[16];
            AskedInputs in;
            memset(&in, 0, sizeof(in));
            in.text = code;
            in.size = sizeof(code);
            in.data[0] = reinterpret_cast<const uint8_t*>(data);
            in.data_size[0] = 12 * 4;
            in.data_count = 1;
            for (int k = 0; k < 16; ++k) {
                data[k] = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(code + k % 8));
            }
            const uint8_t* vtable = reinterpret_cast<const uint8_t*>(data + 4);
            const bool inside = asked_slot_routine(in, vtable, 7) == code + 3;
            const bool past = asked_slot_routine(in, vtable, 8) == NULL;
            const bool elsewhere = asked_slot_routine(in, reinterpret_cast<const uint8_t*>(data + 13), 0) == NULL;
            data[10] = 0x10;
            const bool not_code = asked_slot_routine(in, vtable, 6) == NULL;
            check(inside && past && elsewhere && not_code,
                "asked_slot_routine: a slot inside the vtable's data section is read, one past its end is not, nor a"
                " vtable outside every section; a routine outside the code is none");
        }

        // The drain's sets against a store laid out as the game's.
        SettingSites& st = g_e.game.settings;
        st.store = &g_cfg_store_ptr;
        st.set = &fake_store_set;
        st.lock = &fake_lock_request;
        st.lock_clock = &fake_lock_clock;
        st.lock_stamp = reinterpret_cast<const uint8_t*>(&g_lock_game.stamp);
        st.idle = &fake_idle_timer;
        st.flag = &fake_flag_request;
        st.player = &g_setting_player_ptr;
        st.flags = kSettingFlags;
        st.anonymous = kAnonymousBit;
        uint8_t* entries = g_cfg_entries;
        memcpy(g_cfg_store + kStoreEntries, &entries, 4);
        g_cfg_store[kStoreLoaded] = 1;
        for (int id = 0; id < 0xD0; ++id) {
            put_cfg(id, 0, 0, 1, 0);
        }
        put_cfg(0x0A, 100, 0, 100, 100);
        put_cfg(0x1B, static_cast<int32_t>(0x80808080u), 0, -1, static_cast<int32_t>(0x80808080u));
        put_cfg(0x33, 0x1800, 0, -1, 0x1800);
        put_cfg(0x41, -1, -1, 60, -1);
        put_cfg(0x47, 1, 0, 1, 1);
        put_cfg(0x4A, 0x11, 0, 255, 0x11);
        put_cfg(0x92, 0x20004, 0, -1, 0);
        put_cfg(0x99, 0x105, 0, 0x7FFFFFFF, 0);
        put_cfg(0xBB, -1, 0, -1, -1);
        for (int i = 0; i < kSettingCount; ++i) {
            st.slot[i].id = kSettings[i].id;
            st.slot[i].bit = kSettings[i].bit;
            st.slot[i].row = filter_row(kFilterNoBit, kFilterOneState);
        }
        st.slot[find_setting("filter_say")].row = say;
        st.slot[find_setting("filter_tell")].row = tell;
        st.slot[find_setting("filter_yell")].row = yell;
        st.slot[find_setting("effect_filter_you_to_you")].bit = 1;
        st.slot[find_setting("log2_hp_mp_you_recover")].bit = 33;
        auto set = [&](const char* nm, int32_t value) {
            Command c = cmd(kOpSetSetting, NULL, 0, 0, s1, gen1, static_cast<uint32_t>(value));
            c.target = static_cast<int16_t>(find_setting(nm));
            c.verb = kVerbSetSetting;
            memset(&g_setting_log, 0, sizeof(g_setting_log));
            run(c);
        };
        const SettingLog& sl = g_setting_log;

        set("music_volume", 40);
        check(sl.sets == 1 && sl.set[0][0] == 0x0A && sl.set[0][1] == 40 && cfg_value(0x0A) == 40,
            "set_setting of a word: the store's set with the id and the value, on the game thread");
        set("effect_filter_you_to_you", 1);
        const bool bit_on = sl.sets == 1 && sl.set[0][0] == 0x92 && cfg_value(0x92) == 0x20006;
        set("effect_filter_you_to_you", 0);
        check(bit_on && sl.sets == 1 && cfg_value(0x92) == 0x20004,
            "set_setting of a bit: composed into the word as it is now, every other bit kept, and set");
        set("target_expressions", 1);
        const bool bit7 = cfg_value(0x99) == 0x185;
        set("log2_hp_mp_you_recover", 1);
        check(bit7 && sl.sets == 1 && sl.set[0][0] == 0xBC && cfg_value(0xBC) == 2 && cfg_value(0xBB) == -1,
            "a bit past 31 is the next word's: log window 2's routing bit 33 is 0xBC's bit 1, 0xBB untouched");
        set("filter_say", 1);
        const bool say_on = sl.sets == 1 && sl.set[0][0] == 0x32 && cfg_value(0x32) == 1;
        set("filter_say", 2);
        const bool say_two = sl.sets == 2 && sl.set[0][0] == 0x32 && sl.set[0][1] == 0 && sl.set[1][0] == 0x34
            && sl.set[1][1] == 1;
        set("filter_tell", 1);
        check(say_on && say_two && sl.sets == 2 && sl.set[0][0] == 0x34 && sl.set[0][1] == 5 && sl.set[1][0] == 0x47
                && sl.set[1][1] == 0 && cfg_value(0x33) == 0x1800,
            "set_setting of a chat filter row: the words it changes set in the order its page writes them (0x32,"
            " 0x33, 0x34, 0x35, 0x47), every other bit kept, an unchanged word not set");
        set("filter_yell", 1);
        check(sl.sets == 1 && sl.set[0][0] == 0x33 && cfg_value(0x33) == (0x1800 | (1 << 16)),
            "a chat filter row's mask bit past 29 is the second mask word's");
        set("color_say", static_cast<int32_t>(0x80A040A0u));
        check(sl.sets == 1 && sl.set[0][0] == 0x1B && static_cast<uint32_t>(cfg_value(0x1B)) == 0x80A040A0u,
            "set_setting of a color: the packed word through the store's set");
        // The equipment lock: a wait of 12 s, so that nothing reads the game's 9.
        memset(&g_lock_game, 0, sizeof(g_lock_game));
        g_lock_game.wait = 12000;
        g_lock_game.stamp = 100000;
        g_lock_game.clock = 200000;
        st.lock_wait = g_lock_game.wait;
        uint32_t lock_at = g_e.events.position();
        uint32_t lock_got[4];
        set("style_lock", 1);
        const bool locked = sl.locks == 1 && sl.lock[0] == kLockOn && sl.lock[1] == 0 && sl.lock[2] == 0
            && sl.lock_sets == 0 && sl.lock_bf == 0 && sl.sets == 1 && sl.set[0][0] == 0xBF && sl.set[0][1] == 1
            && cfg_value(0xBF) == 1 && g_lock_game.stamp == 200000 && events(&lock_at, lock_got, 4) == 0;
        g_lock_game.clock = 213000;
        set("style_lock", 1);
        const bool kept = sl.locks == 1 && sl.lock[0] == kLockKept && sl.lock_sets == 0 && sl.sets == 1
            && g_lock_game.stamp == 213000 && events(&lock_at, lock_got, 4) == 0;
        check(locked && kept,
            "set_setting of the equipment lock: the game's request alone, mode 4 to lock and 1 when it was locked"
            " already; 0xBF the request's own write as its send goes, none before it");
        g_lock_game.clock = 217500;
        set("style_lock", 0);
        int n_lock = events(&lock_at, lock_got, 4);
        check(n_lock == 1 && is_error(lock_got[0], s1, gen1, "set_setting", "style_lock",
                  "the game allows one style lock change every 12 seconds; 8 s left")
                && sl.locks == 1 && sl.lock[0] == kLockOff && sl.sets == 0 && g_lock_game.messages == 1
                && g_lock_game.stamp == 213000 && cfg_value(0xBF) == 1,
            "the equipment lock refused by the request's wait (its message shown, al 1, its stamp kept): an error"
            " event with the wait and the seconds left from the stamp, the clock and the wait, rounded up; 0xBF as"
            " it was");
        g_lock_game.clock = 225000;
        g_lock_game.stamp = 213000;
        set("style_lock", 0);
        n_lock = events(&lock_at, lock_got, 4);
        check(n_lock == 1 && is_error(lock_got[0], s1, gen1, "set_setting", "style_lock",
                  "the game allows one style lock change every 12 seconds; 1 s left")
                && sl.sets == 0 && cfg_value(0xBF) == 1,
            "the wait refusing at its last millisecond leaves 1 s, not 0");
        g_lock_game.busy = 1;
        g_lock_game.clock = 300000;
        set("style_lock", 0);
        n_lock = events(&lock_at, lock_got, 4);
        check(n_lock == 1 && is_error(lock_got[0], s1, gen1, "set_setting", "style_lock",
                  "the game is not accepting style lock changes right now")
                && sl.locks == 1 && sl.sets == 0 && g_lock_game.messages == 2 && g_lock_game.stamp == 213000
                && cfg_value(0xBF) == 1,
            "the equipment lock refused by the request's gate (al 0, its stamp kept): an error event; 0xBF as it was");
        g_lock_game.busy = 0;
        g_lock_game.unsent = 1;
        set("style_lock", 0);
        n_lock = events(&lock_at, lock_got, 4);
        check(n_lock == 1 && is_error(lock_got[0], s1, gen1, "set_setting", "style_lock",
                  "the game could not send the request")
                && sl.locks == 1 && sl.sets == 0 && g_lock_game.stamp == 300000 && cfg_value(0xBF) == 1,
            "the equipment lock's send failing (its stamp moved, al 0): an error event; 0xBF as it was");
        g_lock_game.unsent = 0;
        g_lock_game.clock = 400000;
        set("style_lock", 0);
        check(events(&lock_at, lock_got, 4) == 0 && sl.locks == 1 && sl.lock[0] == kLockOff && sl.lock_sets == 0
                && sl.sets == 1 && sl.set[0][0] == 0xBF && sl.set[0][1] == 0 && cfg_value(0xBF) == 0,
            "the equipment lock off: mode 0, 0xBF the request's own write as its send goes");
        set("auto_disconnect", 30);
        const bool idle30 = sl.idles == 1 && sl.idle == 1800 && sl.sets == 0;
        set("auto_disconnect", -1);
        const bool idle_off = sl.idles == 1 && sl.idle == 0;
        set("auto_disconnect", 0);
        check(idle30 && idle_off && sl.idles == 1 && sl.idle == 0,
            "set_setting of auto_disconnect: through the game's timer, the minutes in seconds; -1 and 0 turn it off");
        set("character_information", 1);
        const bool anon_on = sl.flags == 1 && sl.flag[0] == kFlagAnonymous && sl.flag[1] == 1 && sl.sets == 0;
        set("character_information", 0);
        check(anon_on && sl.flags == 1 && sl.flag[1] == 0,
            "set_setting of character_information: the player flag's request, flag 0 (1 hides), as conf3win's close"
            " sends it");
        set("icon_type", 0);
        const bool icon_clear = sl.sets == 1 && sl.set[0][0] == 0x99 && cfg_value(0x99) == 0x181;
        set("icon_type", 1);
        check(icon_clear && sl.sets == 1 && sl.set[0][0] == 0x99 && cfg_value(0x99) == 0x185,
            "set_setting of a choice on a bit, icon_type: type1 sets 0x99's bit 2 and type2 clears it, every other bit"
            " kept");

        uint32_t at = g_e.events.position();
        uint32_t got[8];
        g_cfg_store[kStoreLoaded] = 0;
        set("music_volume", 50);
        int n = events(&at, got, 8);
        check(n == 1 && is_error(got[0], s1, gen1, "set_setting", "music_volume", "not loaded") && sl.sets == 0
                && cfg_value(0x0A) == 40,
            "set_setting while the player's values are not loaded: an error event, nothing set");
        g_cfg_store[kStoreLoaded] = 1;
        give_controller("cfilter", vt_fixed);
        give_controller("k1assign", vt_fixed);
        give_controller("k2assign", vt_fixed);
        give_controller("conf3win", vt_fixed);
        open_now("cfilter");
        events(&at, got, 8);
        set("filter_say", 0);
        n = events(&at, got, 8);
        const bool filter_open = n == 1 && is_error(got[0], s1, gen1, "set_setting", "filter_say", "cfilter is open")
            && sl.sets == 0;
        set("filter_special_actions_started_on_by_allies", 1);
        n = events(&at, got, 8);
        check(n == 1 && is_error(got[0], s1, gen1, "set_setting", "filter_special_actions_started_on_by_allies",
                  "cfilter is open")
                && strstr(g_e.error, "set_setting filter_special_actions_started_on_by_allies: cfilter is open")
                    == g_e.error
                && sl.sets == 0,
            "an error event for a 43-character setting name carries the whole name, as status()'s drain error"
            " does");
        run(cmd(kOpClose, "cfilter", 0, 0, s1, gen1));
        open_now("k1assign");
        events(&at, got, 8);
        set("key1_move_up", 0x2C);
        n = events(&at, got, 8);
        const bool keys_open = n == 1 && is_error(got[0], s1, gen1, "set_setting", "key1_move_up", "k1assign is open")
            && cfg_value(0x4A) == 0x11;
        set("key2_attack", 0x20);
        n = events(&at, got, 8);
        const bool k2_in_k1 = n == 1 && is_error(got[0], s1, gen1, "set_setting", "key2_attack",
            "k1assign is open: the two key pages share one copy of the keys") && sl.sets == 0;
        run(cmd(kOpClose, "k1assign", 0, 0, s1, gen1));
        open_now("k2assign");
        events(&at, got, 8);
        set("key1_move_up", 0x2C);
        n = events(&at, got, 8);
        const bool k1_in_k2 = n == 1 && is_error(got[0], s1, gen1, "set_setting", "key1_move_up", "k2assign is open")
            && sl.sets == 0;
        set("key2_attack", 0x20);
        n = events(&at, got, 8);
        const bool k2_in_k2 = n == 1 && is_error(got[0], s1, gen1, "set_setting", "key2_attack", "k2assign is open")
            && sl.sets == 0;
        run(cmd(kOpClose, "k2assign", 0, 0, s1, gen1));
        events(&at, got, 8);
        set("key1_move_up", 0x2C);
        const bool k1_closed = events(&at, got, 8) == 0 && sl.sets == 1 && cfg_value(0x4A) == 0x2C;
        set("key2_attack", 0x20);
        const bool k2_closed = events(&at, got, 8) == 0 && sl.sets == 1 && cfg_value(0x6A) == 0x20;
        check(keys_open && k2_in_k1 && k1_in_k2 && k2_in_k2 && k1_closed && k2_closed,
            "the key pages as one: a key1_ or a key2_ set refused while either k1assign or k2assign is open, the"
            " error naming the open page; both set once neither is");
        set("key1_move_up", 0x11);
        bool key_pages = true;
        for (int i = 0; i < kSettingCount; ++i) {
            if (kSettings[i].from == kFromKeys) {
                key_pages = key_pages && (strcmp(kSettings[i].page, kKeyPages[0]) == 0
                    || strcmp(kSettings[i].page, kKeyPages[1]) == 0);
            }
        }
        check(key_pages && strcmp(kSettings[find_setting("key1_move_up")].page, kKeyPages[0]) == 0
                && strcmp(kSettings[find_setting("key2_attack")].page, kKeyPages[1]) == 0,
            "every key's page one of the two key pages the refusal checks");
        open_now("conf3win");
        events(&at, got, 8);
        set("character_information", 1);
        n = events(&at, got, 8);
        const bool flag_open = n == 1 && is_error(got[0], s1, gen1, "set_setting", "character_information",
            "conf3win is open") && sl.flags == 0;
        set("pc_armor_display", 1);
        n = events(&at, got, 8);
        check(filter_open && flag_open && n == 0 && sl.sets == 1 && cfg_value(0xA5) == 1,
            "set_setting while its page is open and writes its own copy back as it closes (cfilter, conf3win for"
            " character_information): an error event, nothing set; conf3win's other rows write at once, set");
        run(cmd(kOpClose, "conf3win", 0, 0, s1, gen1));

        int32_t raw = 0;
        int32_t lo = 0;
        int32_t hi = 0;
        const int tell_i = find_setting("filter_tell");
        const bool reads = setting_raw(kPlain, st, music, false, &raw) && raw == 40
            && setting_raw(kPlain, st, music, true, &raw) && raw == 100 && setting_raw(kPlain, st, tell_i, false, &raw)
            && raw == 1 && setting_raw(kPlain, st, tell_i, true, &raw) && raw == 2
            && setting_raw(kPlain, st, find_setting("target_expressions"), false, &raw) && raw == 1;
        g_setting_player[kSettingFlags] = kAnonymousBit;
        const int anon = find_setting("character_information");
        const bool flag = setting_raw(kPlain, st, anon, false, &raw) && raw == 1
            && !setting_raw(kPlain, st, anon, true, &raw);
        g_setting_player[kSettingFlags] = 0;
        const bool ranges = setting_range(kPlain, st, music, &lo, &hi) && lo == 0 && hi == 100
            && setting_range(kPlain, st, tell_i, &lo, &hi) && lo == 0 && hi == 3
            && setting_range(kPlain, st, find_setting("filter_say"), &lo, &hi) && hi == 2
            && setting_range(kPlain, st, find_setting("color_say"), &lo, &hi) && lo == 0 && hi == 255
            && setting_range(kPlain, st, find_setting("auto_disconnect"), &lo, &hi) && lo == -1 && hi == 60
            && setting_range(kPlain, st, find_setting("effect_filter_you_to_you"), &lo, &hi) && lo == 0 && hi == 1;
        bool unbounded = true;
        wr32(cfg_entry(0x0A), kEntryId, 0x0B);
        const bool foreign = !setting_raw(kPlain, st, music, false, &raw)
            && !setting_range(kPlain, st, music, &lo, &hi, &unbounded)
            && !unbounded;
        wr32(cfg_entry(0x0A), kEntryId, 0x0A);
        check(reads && flag && ranges && foreign && store_loaded(kPlain, st),
            "the reads: a word, its default from the entry, a filter row's state (Tell's 1, its default 2: the flag),"
            " a bit, character_information from the player's flags with no default; the ranges: the store's, a"
            " row's states, a color's channel, a bit's 0..1; an entry that is not the id's read as none");
        const int ranged[3] = {music, find_setting("key1_move_up"), find_setting("auto_target")};
        uint32_t maxes[3];
        for (int k = 0; k < 3; ++k) {
            maxes[k] = rd32(cfg_entry(kSettings[ranged[k]].id), kEntryMax);
            wr32(cfg_entry(kSettings[ranged[k]].id), kEntryMax, 0xFFFFFFFFu);
        }
        unbounded = false;
        const bool number_none = !setting_range(kPlain, st, ranged[0], &lo, &hi, &unbounded) && unbounded;
        unbounded = false;
        const bool key_none = !setting_range(kPlain, st, ranged[1], &lo, &hi, &unbounded) && unbounded;
        unbounded = true;
        const bool bool_kept = setting_range(kPlain, st, ranged[2], &lo, &hi, &unbounded) && !unbounded;
        for (int k = 0; k < 3; ++k) {
            wr32(cfg_entry(kSettings[ranged[k]].id), kEntryMax, maxes[k]);
        }
        check(kSettings[ranged[1]].value == kValueKey && kSettings[ranged[1]].store == kStoreWord
                && kSettings[ranged[2]].value == kValueBool && kSettings[ranged[2]].store == kStoreWord && number_none
                && key_none && bool_kept,
            "a number or a key whose store entry has no range (max -1, which the store's set takes any value for):"
            " no range, said as such, so that set_setting refuses it; a bool in such a word is not refused, its"
            " value taking no range");

        // The labels: read through the message getter once it answers.
        g_label_calls = 0;
        drain(g_e);
        const bool waits = !g_e.setting_labels.ready && g_label_calls == 1;
        static char long_label[80];
        memset(long_label, 'a', sizeof(long_label) - 1);
        long_label[62] = '\x81';
        long_label[63] = '\xA8';
        long_label[79] = '\0';
        for (int i = 0; i < kSettingCount; ++i) {
            if (kSettings[i].label >= 0) {
                g_label_texts[kSettings[i].label] = "label";
            }
        }
        g_label_texts[0x24] = "Immediate vicinity (\"Say\")";
        g_label_texts[0x99] = long_label;
        g_label_calls = 0;
        drain(g_e);
        const int calls = g_label_calls;
        drain(g_e);
        const SettingLabels& labels = g_e.setting_labels;
        check(waits && labels.ready && g_label_calls == calls && calls > 200
                && strcmp(labels.text[find_setting("color_say")], "Immediate vicinity (\"Say\")") == 0
                && strcmp(labels.text[find_setting("filter_say")], "label") == 0
                && labels.text[music][0] == '\0' && strlen(labels.text[find_setting("effect_filter_all_effects_during_"
                    "battle")]) == 62,
            "labels: the game's text by its message getter, string table 0xB, once it answers; nothing for a"
            " setting it has none for; cut short of 64 bytes at a whole character");
        memset(&st, 0, sizeof(st));
    }

    check(g_menus_used <= static_cast<int>(sizeof(g_menus) / sizeof(g_menus[0])),
        "the fabricated menus fit their pool");

    std::printf("%d of %d checks failed\n", g_failures, g_checks);
    return g_failures;
}
