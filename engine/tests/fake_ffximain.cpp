// fake_ffximain - a stand-in FFXiMain.dll for the end-to-end harness. Its
// .text carries the thirty-four signatures from signatures.h, each
// at a routine that behaves as far as the engine relies on it: the registry,
// manager, link5 cache, link5 latch, link5 open and query cancel sites hold
// the addresses of this image's own data and code, SetPosition writes the
// origin, the open routine opens fabricated menus, the UI update counts
// frames, and the reply, re-dock and resize routines, SetCursor among them,
// record what they were given in FakeLog. A menu's frame rect
// (+0x3A..+0x40) starts on its default rect, follows SetPosition at its size
// and takes the box SetFrameRect gives it. The query controller holds a
// parsed choice in glyph codes: a title and four options, the second
// tombstoned, its cursor on the first; query_down moves the cursor as the
// game's setter does, and query's confirm takes the value of the option under
// it; its input routine counts any call that reaches it. The event's
// link5 callback clears the latch, as the game's does. The text-to-glyph
// converter is fake_glyph_convert.h's and counts the calls made off the
// thread the harness names as the game's. post1's controller has its own
// input routine, which records the events the box gets. The mouse mode
// picker is the game's 29 bytes, the manager in its imm32, around a callee
// that answers as the game's does and counts its calls; a new menu gets
// +0x77 = 1, as the game's constructor gives it. Close by name frees the
// instance at once, or, while the harness sets close_frames, closes it as the
// game's does: the closing mark +0x6E, then the staged close, or on a dormant
// menu the pending-destroy mark +0x70 and no staged close, and the update
// frees it close_frames frames later; either way close by instance runs the
// controller's begin-close (vtable +0x08) on an instance not yet closing. The
// show path, the row hit test (carrying its +0x77 test) and the four data
// sites are never run, nor is the staged close but by that close.
// arealist's controller is fake_arealist.h's model; its close+reset is the
// game's bytes around two stand-in callees, its latch clear the game's
// routine byte for byte. While the harness sets box_ticks, each frame runs
// the post boxes' ticks on their bound instances as the game's do: post1's
// tears the family down (post2, then post1, by instance) once its state is
// 0x12; delivery's runs its closing states 0x15 (the staged rows back, 'u'),
// 0x16 (the box-2 slots back, 'v', then the state in +0x1B6) and 0x18
// (request 15, then 0x19). post_reply is the server's 0x04B reply to
// request 15 ('Z'), run at the next frame: post1's request-close, then
// delivery's (0x19: the family closed, state 0; 0x15 to 0x17 left alone;
// any other state the closing row's two words). While query_waits is set,
// the event waits on query: the frame after its result word turns non-zero
// closes query by name. The menu input sink is the game's first 26 bytes,
// its dormant test among them, then a body that records the menu and code
// it was given and answers 1. The routing routine (this = the manager; the
// code) is the game's, byte for byte, around three stand-in callees: input
// suspended while the manager's +0xB4 is 1, a menu up as the picker's callee
// reads it, and on code 7 with no menu up the main menu's open, counted; its
// call to the sink is the real call. The compass draw entry is the game's
// shape around three stand-in callees: the update counts its calls, the
// manager's gate answers al 0 (it draws), and the render, reached by the
// jump, counts its calls and records the anchor it saw; each frame runs it
// after the update, as the game's does, on the compass object the global
// names. The macro key gate is the game's shape around stand-in globals and
// a callee, its body answering al 1; each frame the harness holds a number
// key (macro_key), a macro fires when the gate, called through its site,
// says so. The macro object's constructor site, the two copies of its
// global, is never run. The ability opener is the game's shape around a
// stand-in player global, the two key pushes naming 16-byte keys in this
// image's data, the manager and two real calls to the open routine, then a
// body standing in for the controller method, which stores the kind at the
// ability controller's +0x64 and records it; the harness calls it as the
// game's callers do, cdecl (kind, flag, extra).

#define WIN32_LEAN_AND_MEAN
#include "../core.h"
#include "fake_glyph_convert.h"
#include "fake_arealist.h"

using namespace hu;

extern "C" {

uint8_t g_registry[(kRowCount + 1) * kRowStride];
uint8_t g_pristine[(kRowCount + 1) * kRowStride];
uint8_t g_mcb[0x100];
uint32_t g_dock_masks[5] = {0x1000, 0x2000, 0x4000, 0x10000000, 0};
uint8_t* g_ctl_global[kRowCount];
volatile LONG g_frames;
uint32_t g_net = 1;
uint8_t g_party_pending[2];         // the invite flag, then 1 party / 0 alliance
uint8_t g_link5_flag;
uint8_t g_link5_latch;
uint8_t g_link5_cache[16 * 0x20];
uint8_t g_query_cancel_allowed;
volatile LONG g_party_queues;
volatile LONG g_post_queues;
DWORD g_game_thread;
volatile LONG g_convert_calls;
volatile LONG g_convert_elsewhere;     // calls from any thread but g_game_thread
volatile LONG g_menu_up_calls;         // the mouse mode picker's callee, run when the original runs
uint32_t g_close_frames;               // frames a closed instance outlives its close; 0 frees it at once
volatile LONG g_box_ticks;
volatile LONG g_post_reply;
volatile LONG g_query_waits;
uint8_t g_compass[0x3C];               // the compass object: state 3, anchor 104,1058, height 42
uint8_t* g_compass_ptr;                // the game's global naming it
volatile LONG g_compass_draws;         // the render's calls
volatile LONG g_compass_updates;       // the update's calls
uint8_t g_macro_object[0x24];          // the macro key object: +0x0C the bar up, +0x0D the set, +0x18 bar-up
uint8_t* g_macro_object_ptr;           // the game's global naming it
volatile LONG g_macro_key;             // set by the harness: a number key is held this frame
volatile LONG g_macros_fired;          // macros run
int16_t g_macro_player = 1;            // the word the gate's prologue reads: the player's index
void* g_macro_table[2];                // its pointer table, indexed by that word
uint32_t g_macro_world = 0x60;         // the world state the gate compares with 0x60
uint32_t* g_macro_world_ptr = &g_macro_world;
uint32_t g_ability_player = 1;         // the global the ability opener tests first: non-zero, in the world
char g_key_abisortw[17] = "menu    abisortw";   // the opener's two key pushes, 16 bytes each
char g_key_ability[17] = "menu    ability ";
int32_t g_ability_last_kind = -1;      // the kind the opener's body stored last

// What the reply, re-dock and resize routines saw. Written on the game
// thread; the harness reads it after waiting for frames.
struct FakeLog {
    char order[96];             // one letter per call
    LONG count;
    LONG dock_resets;
    int dock_group;
    void* dock_menu;
    char swaps[64];             // each swap's eight-character name and '|'
    void* swap_menu;
    int frame[7];
    void* frame_menu;
    int query_event;
    void* text_context;
    char text[33];
    int text_null;
    int party_value;
    int post_command;
    char last_close[17];
    char closes[64];            // each close's eight-character name and '|'
    int link5_calls;
    int link5_slot;             // the last call that carried an entry
    void* link5_entry;
    int link5_last_null;
    int cursor_calls;
    void* cursor_menu;
    int cursor_row[4];          // the first four calls' arguments
    int cursor_warp[4];
    int cursor_items[4];        // +0x58 as each of them found it
    int box_calls;              // post1's input routine
    int box_event;
    int box_row;
    void* box_ctl;
    char last_open[17];         // the key the open routine looked up last
    int sink_calls;             // the menu input sink's body
    void* sink_menu;
    int sink_code;
    int main_menu_opens;        // the routing routine's code 7 with no menu up
    int compass_x;              // the anchor the compass render saw last
    int compass_y;
};
FakeLog g_log;

struct FakeState {
    uint8_t* registry;
    uint8_t* pristine;
    uint8_t* mcb;
    uint8_t** ctl_global;
    volatile LONG* frames;
    void* set_position;
    void* open;
    void* close;
    void* ui_update;
    void* staged_close;
    void* show_path;
    uint8_t* spare_set_position;
    void* calls[12];            // query's confirm, then signatures.h's kCalls in order
    uint8_t* party_pending;
    volatile LONG* party_queues;
    volatile LONG* post_queues;
    uint8_t* query_cancel_allowed;  // query's input handler cancels only while 1
    void* text_callback;        // a passinpu or link5 callback that logs
    FakeLog* log;
    const void** vtable;        // every fabricated controller's
    uint8_t* link5_cache;
    void* link5_callback;       // the event's link5 callback, recording each call
    uint8_t* link5_site;        // the signature sites of the two data
    uint8_t* query_cancel_site;
    void* set_cursor;
    uint8_t* link5_latch;       // 1 while the event waits on a choice
    uint8_t* link5_latch_site;
    uint8_t* link5_open_site;   // its imm32s: link5's controller global, the callback
    DWORD* game_thread;         // the harness's game thread, set before it runs
    volatile LONG* convert_calls;
    volatile LONG* convert_elsewhere;
    void* glyph_convert;
    void* mouse_mode;           // fastcall, ecx = a mouse object: +0x4D 2 or 1
    volatile LONG* menu_up_calls;
    void* row_hit_test;
    uint8_t* mouse_read;        // the hit test's +0x77 test inside it
    uint32_t* close_frames;     // 0: close by name frees the instance at once
    void* query_down;           // fastcall, ecx = query's controller: Down, as the game's setter moves it
    fake_area::Log* area_log;   // what arealist's model did
    void* area_prepare;         // cdecl (mode, grouped): arealist's controller ready for the open by name
    void* area_opened;          // cdecl (): once the open has bound its menu
    uint8_t* area_ctl;
    void* area_select;          // fastcall (ctl; index, row): the base list's select-index
    volatile LONG* box_ticks;   // non-zero: each frame runs the post boxes' ticks on their bound instances
    volatile LONG* post_reply;  // set: the next frame runs the server's reply to request 15, then clears it
    volatile LONG* query_waits; // set: the event waits on query's result word
    void* menu_input;           // thiscall (menu; code), ret 4: the sink
    void* menu_routing;         // thiscall (manager; code), ret 4: the routing routine
    uint8_t* compass;           // the compass object
    uint8_t** compass_ptr;      // the global naming it
    void* compass_draw;         // cdecl (), plain ret: the compass draw entry
    volatile LONG* compass_draws;
    volatile LONG* compass_updates;
    uint8_t* macro_object;      // the macro key object
    uint8_t** macro_object_ptr; // the global naming it
    void* macro_gate;           // cdecl (), al the answer: the macro key gate
    uint8_t* macro_ctor_site;   // the constructor site naming the global twice
    volatile LONG* macro_key;   // set: a number key is held each frame
    volatile LONG* macros_fired;
    void* ability_open;         // cdecl (kind, flag, extra), nothing returned: the ability opener
    int32_t* ability_last_kind;
};

void fake_query_confirm();
void fake_passinpu_reset();
void fake_party_send();
void fake_party_clear();
void fake_link5_clear();
void fake_arealist_close();
void fake_arealist_latch();
void fake_post_request();
void fake_post_request_close();
void fake_dock_reset();
void fake_template_swap();
void fake_set_frame_rect();
void fake_set_position();
void fake_open();
void fake_close();
void fake_ui_update();
void fake_staged_close();
void fake_show_path();
void fake_registry_lookup();
void fake_draw_walk();
void fake_dock_trigger();
void fake_spare_set_position();
void fake_link5_confirm();
void fake_query_cancel_site();
void fake_set_cursor();
void fake_link5_latch_site();
void fake_link5_open_site();
void fake_glyph_convert_site();
void fake_mouse_mode();
void fake_row_hit_test();
void fake_row_hit_read();
void fake_menu_sink();
void fake_menu_routing();
void fake_compass_draw();
void fake_macro_gate();
void fake_macro_ctor_site();
void fake_ability_open();

}  // extern "C"

namespace {

uint8_t g_ctl[32][0x600];
int g_ctl_used;
uint8_t g_menus[64][0xD4];
uint8_t g_res[64][0x60];
int g_menus_used;
uint8_t g_self_refresh[16] = {0x90, 0xC3};
const void* g_vtable[0x20];
const void* g_box_vtable[0x20];
const void* g_area_vtable[0x20];
uint8_t* g_area_ctl;
const int kChoiceOptions = 4;
uint8_t g_option_nodes[kChoiceOptions][0x18];
uint8_t g_options[kChoiceOptions][0x108];
const int kQueryRowsShown = 3;

int row_of_key(const char* key16) {
    for (int r = 0; r < kRowCount; ++r) {
        if (memcmp(g_registry + r * kRowStride, key16, 16) == 0) {
            return r;
        }
    }
    return -1;
}

uint8_t* controller_for(int r) {
    if (r < 0) {
        return NULL;
    }
    const char* name = kRowSpecs[r].name;
    for (int i = 0; i < kRowCount; ++i) {
        if (g_ctl_global[i] && strcmp(kRowSpecs[i].name, name) == 0) {
            return g_ctl_global[i];
        }
    }
    return NULL;
}

void logged(char what) {
    if (g_log.count + 1 < static_cast<LONG>(sizeof(g_log.order))) {
        g_log.order[g_log.count] = what;
        g_log.order[g_log.count + 1] = '\0';
        InterlockedIncrement(&g_log.count);
    }
}

// The eight name characters of a 16-byte key, and a separator.
void append_name(char* list, size_t size, const char* key16) {
    const size_t used = strlen(list);
    if (used + 10 <= size) {
        memcpy(list + used, key16 + 8, 8);
        list[used + 8] = '|';
        list[used + 9] = '\0';
    }
}

// Freeing an instance: no controller holds it any more.
void unbind(const void* menu) {
    for (int r = 0; r < kRowCount; ++r) {
        uint8_t* ctl = g_ctl_global[r];
        if (!ctl) {
            continue;
        }
        void* bound;
        memcpy(&bound, ctl + 8, 4);
        if (bound == menu) {
            memset(ctl + 8, 0, 4);
        }
    }
}

// The game's setter of query's cursor: the option (+0x30) clamped into the
// list's count (+0x24), the first option shown (+0x32) moved just enough to
// keep it among the rows shown, the menu's row (+0x4C) cursor - top + 1.
void query_set_cursor(uint8_t* ctl, int option) {
    int32_t count;
    memcpy(&count, ctl + 0x24, 4);
    int at = option > count - 1 ? count - 1 : option;
    at = at < 0 ? 0 : at;
    int16_t top;
    memcpy(&top, ctl + 0x32, 2);
    if (at < top) {
        top = static_cast<int16_t>(at);
    } else if (at > top + kQueryRowsShown - 1) {
        top = static_cast<int16_t>(at - (kQueryRowsShown - 1));
    }
    const int16_t at16 = static_cast<int16_t>(at);
    const int16_t row = static_cast<int16_t>(at - top + 1);
    memcpy(ctl + 0x30, &at16, 2);
    memcpy(ctl + 0x32, &top, 2);
    uint8_t* menu;
    memcpy(&menu, ctl + 8, 4);
    if (menu) {
        memcpy(menu + 0x4C, &row, 2);
    }
}

// Closed instances the update frees once the frame count reaches `due`.
struct Dying {
    uint8_t* menu;
    LONG due;
};
Dying g_dying[16];
int g_dying_count;

typedef void (__fastcall* StagedCloseFn)(void* mcb, void* edx, void* menu, int, int, int, int);

}  // namespace

extern "C" {

// The query controller's input(event, row): its cancel writes 0xFF only while
// the cancel-allowed byte is 1, as the game's does.
uint32_t __fastcall fake_query_input(uint8_t* ctl, void*, int event, int row) {
    logged('Q');
    g_log.query_event = event;
    if (event == 6 && g_query_cancel_allowed == 1) {
        const int16_t cancelled = 0xFF;
        memcpy(ctl + 0x548, &cancelled, 2);
    }
    return 1;
}

// Down on query: the option after the one under the cursor.
void __fastcall fake_query_down(uint8_t* ctl, void*) {
    int16_t at;
    memcpy(&at, ctl + 0x30, 2);
    query_set_cursor(ctl, at + 1);
}

// post1's input(event, row): the box's own handling is the game's; this
// only records what it was given.
uint32_t __fastcall fake_box_input(uint8_t* ctl, void*, int event, int row) {
    logged('I');
    ++g_log.box_calls;
    g_log.box_event = event;
    g_log.box_row = row;
    g_log.box_ctl = ctl;
    return 1;
}

void __cdecl fake_text_callback(void* context, const char* text) {
    logged('C');
    g_log.text_context = context;
    g_log.text_null = text == NULL;
    if (text) {
        memcpy(g_log.text, text, 32);
    }
}

// The event's link5 callback: (slot, entry), and (0, NULL) from the clear.
// Either clears the latch the event waits on.
void __cdecl fake_link5_callback(int slot, void* entry) {
    logged('J');
    g_link5_latch = 0;
    ++g_log.link5_calls;
    g_log.link5_last_null = entry == NULL;
    if (entry) {
        g_log.link5_slot = slot;
        g_log.link5_entry = entry;
    }
}

void __stdcall h_log(int what) {
    logged(static_cast<char>(what));
}

int __stdcall h_query_row(int row) {
    return row;
}

// The confirm's lookup (this = the list at +0x14): the item of the index-th
// node linked.
void* __fastcall h_query_item(uint8_t* list, void*, int index) {
    uint8_t* node;
    memcpy(&node, list, 4);
    for (int i = 0; node && i < index; ++i) {
        memcpy(&node, node, 4);
    }
    void* item = NULL;
    if (node) {
        memcpy(&item, node + 0x10, 4);
    }
    return item;
}

uint32_t __cdecl h_party_alloc(int, int, int) {
    return g_party_queues ? 0x1000u : 0u;
}

void __cdecl h_party_sent(int accept) {
    logged('P');
    g_log.party_value = accept & 0xFF;
}

// close+reset's two callees: the base list's set-rows (this = ctrl; rows,
// count, flag), ret 0xC, here emptying the list; and the tail that frees the
// rows.
void __fastcall h_area_rows(uint8_t* ctl, void*, int, int, int) {
    ++fake_area::g_log.closes;
    fake_area::set_list(ctl, NULL, 0);
}

void __fastcall h_area_free(uint8_t* ctl, void*) {
    fake_area::free_rows(ctl);
}

// The base list's select-index, for the model's key handler and the harness.
void __fastcall h_area_select(uint8_t* ctl, void*, int index, int row) {
    fake_area::select_index(ctl, index, row);
}

void area_select(uint8_t* ctl, int index, int row) {
    h_area_select(ctl, NULL, index, row);
}

typedef int (__fastcall* CloseByNameFn)(void* mcb, void* edx, const char* key);

void area_close_by_name(const char* key16) {
    CloseByNameFn close = reinterpret_cast<CloseByNameFn>(&fake_close);
    close(g_mcb, NULL, key16);
}

void __cdecl fake_area_prepare(int mode, int grouped) {
    fake_area::prepare(g_area_ctl, mode, grouped != 0);
}

void __cdecl fake_area_opened() {
    fake_area::opened(g_area_ctl);
}

uint32_t __cdecl h_post_request(int command) {
    logged('B');
    g_log.post_command = command;
    return g_post_queues ? 1u : 0u;
}

uint32_t __fastcall h_post_active(uint8_t* ctl, void*) {
    uint32_t state;
    memcpy(&state, ctl + 0x14, 4);
    return state != 0;
}

void close_instance(const char* nm);

// The rest of post1's request-close: 0x12 the first time; the next, the
// family closed by instance and the state 0.
void __stdcall h_post_close_rest(uint8_t* ctl) {
    logged('Y');
    uint32_t state;
    memcpy(&state, ctl + 0x14, 4);
    if (state == 0x12) {
        close_instance("post2");
        close_instance("post1");
        state = 0;
    } else {
        state = 0x12;
    }
    memcpy(ctl + 0x14, &state, 4);
}

void __stdcall h_dock_reset(int group, void* menu) {
    logged('D');
    InterlockedIncrement(&g_log.dock_resets);
    g_log.dock_group = group;
    g_log.dock_menu = menu;
    const int16_t zero = 0;
    memcpy(g_mcb + 0xA0 + 2 * group, &zero, 2);
}

// Snaps the frame to a template position and leaves the origin, as the
// game's swap does.
void __stdcall h_template_swap(uint8_t* menu, const char* key) {
    logged('S');
    append_name(g_log.swaps, sizeof(g_log.swaps), key);
    g_log.swap_menu = menu;
    const int16_t snap = 7;
    memcpy(menu + 0x3A, &snap, 2);
    memcpy(menu + 0x3C, &snap, 2);
}

void* __fastcall h_list_head(void* list, void*) {
    return list;
}

// SetCursor's body: +0x4C clamped to 1..+0x58, as the game's writes it.
void __stdcall h_set_cursor(uint8_t* menu, int row, int warp) {
    logged('U');
    const int call = g_log.cursor_calls++;
    g_log.cursor_menu = menu;
    int16_t items;
    memcpy(&items, menu + 0x58, 2);
    const int16_t r = static_cast<int16_t>(row);
    if (call < 4) {
        g_log.cursor_row[call] = r;
        g_log.cursor_warp[call] = warp & 0xFF;
        g_log.cursor_items[call] = items;
    }
    int16_t at = r < 1 ? 1 : r;
    if (at > items) {
        at = items;
    }
    memcpy(menu + 0x4C, &at, 2);
}

void __stdcall h_frame_rect(uint8_t* menu, int x, int y, int w, int h, int one, int keep, int b9a) {
    logged('F');
    const int args[7] = {x, y, w, h, one, keep, b9a};
    memcpy(g_log.frame, args, sizeof(args));
    g_log.frame_menu = menu;
    const int16_t rect[4] = {static_cast<int16_t>(x), static_cast<int16_t>(y), static_cast<int16_t>(x + w),
                             static_cast<int16_t>(y + h)};
    memcpy(menu + 0x3A, rect, sizeof(rect));
}

// SetPosition's tail: the frame follows the origin at its size.
void __stdcall h_set_position(uint8_t* menu) {
    logged('M');
    int16_t rect[4];
    int16_t origin[2];
    memcpy(rect, menu + 0x3A, sizeof(rect));
    memcpy(origin, menu + 0x52, sizeof(origin));
    rect[2] = static_cast<int16_t>(origin[0] + rect[2] - rect[0]);
    rect[3] = static_cast<int16_t>(origin[1] + rect[3] - rect[1]);
    rect[0] = origin[0];
    rect[1] = origin[1];
    memcpy(menu + 0x3A, rect, sizeof(rect));
}

// The open routine's two inner calls and the work after them.
void __stdcall h_normalize(char* buf, const char* key) {
    size_t i = 0;
    for (; i < 16 && key[i]; ++i) {
        buf[i] = lower_ascii(key[i]);
    }
    for (; i < 16; ++i) {
        buf[i] = ' ';
    }
}

int __stdcall h_lookup(char* buf, void*) {
    return row_of_key(buf) >= 0;
}

void* __stdcall h_open(const char* buf) {
    memcpy(g_log.last_open, buf, 16);
    g_log.last_open[16] = '\0';
    const int r = row_of_key(buf);
    uint8_t* ctl = controller_for(r);
    if (!ctl) {
        return NULL;
    }
    const uint8_t* row = g_registry + r * kRowStride;
    uint8_t* menu;
    memcpy(&menu, ctl + 8, 4);
    if (!menu) {
        // Instances are never reused: past the pool, the open fails.
        if (g_menus_used >= static_cast<int>(sizeof(g_menus) / sizeof(g_menus[0]))) {
            return NULL;
        }
        menu = g_menus[g_menus_used];
        uint8_t* res = g_res[g_menus_used];
        ++g_menus_used;
        memset(menu, 0, 0xD4);
        memset(res, 0, 0x60);
        memcpy(res + 0x46, row, 16);
        memcpy(menu + 4, &res, 4);
        const int16_t x = static_cast<int16_t>(100 + r);
        const int16_t y = 50;
        const int16_t right = static_cast<int16_t>(x + 120);
        const int16_t bottom = static_cast<int16_t>(y + 40);
        memcpy(menu + 0x42, &x, 2);
        memcpy(menu + 0x44, &y, 2);
        memcpy(menu + 0x46, &right, 2);
        memcpy(menu + 0x48, &bottom, 2);
        memcpy(menu + 0x3A, menu + 0x42, 8);
        memcpy(menu + 0x52, &x, 2);
        memcpy(menu + 0x54, &y, 2);
        menu[0x77] = 1;
        memcpy(ctl + 8, &menu, 4);
    }
    // An instance the open finds is reused, a closing one too: both marks
    // cleared, its destruction called off.
    menu[0x6E] = 0;
    menu[0x70] = 0;
    for (int i = 0; i < g_dying_count; ++i) {
        if (g_dying[i].menu == menu) {
            g_dying[i] = g_dying[--g_dying_count];
            break;
        }
    }
    menu[0x60] = row[0x29];
    memcpy(menu + 0x34, row + 0x24, 4);
    return menu;
}

void __stdcall h_close_key(char* buf, const char* key) {
    h_normalize(buf, key);
    logged('K');
    memcpy(g_log.last_close, buf, 16);
    g_log.last_close[16] = '\0';
    append_name(g_log.closes, sizeof(g_log.closes), buf);
}

void* __stdcall h_close_find(char* buf) {
    uint8_t* ctl = controller_for(row_of_key(buf));
    uint8_t* menu = NULL;
    if (ctl) {
        memcpy(&menu, ctl + 8, 4);
    }
    return menu;
}

typedef uint32_t (__fastcall* BeginCloseFn)(uint8_t* ctl, void* edx);

// The begin-close of the controller bound to the menu, where its class has
// one (vtable +0x08).
void begin_close(const uint8_t* menu) {
    for (int r = 0; r < kRowCount; ++r) {
        uint8_t* ctl = g_ctl_global[r];
        void* bound = NULL;
        if (ctl) {
            memcpy(&bound, ctl + 8, 4);
        }
        if (ctl && bound == menu) {
            const void* const* vt;
            memcpy(&vt, ctl, 4);
            if (vt[2]) {
                reinterpret_cast<BeginCloseFn>(const_cast<void*>(vt[2]))(ctl, NULL);
            }
            return;
        }
    }
}

void __stdcall h_close_menu(void* menu, int) {
    uint8_t* m = static_cast<uint8_t*>(menu);
    if (!g_close_frames) {
        if (!m[0x6E]) {
            begin_close(m);
        }
        unbind(menu);
        return;
    }
    if (m[0x6E] || g_dying_count >= static_cast<int>(sizeof(g_dying) / sizeof(g_dying[0]))) {
        return;
    }
    m[0x6E] = 1;
    begin_close(m);
    uint32_t state;
    memcpy(&state, m + 0x10, 4);
    if (state == 0x0E) {
        m[0x70] = 1;
    } else {
        StagedCloseFn staged = reinterpret_cast<StagedCloseFn>(&fake_staged_close);
        staged(g_mcb, NULL, m, 1, 0, 0, 0);
    }
    g_dying[g_dying_count].menu = m;
    g_dying[g_dying_count].due = g_frames + static_cast<LONG>(g_close_frames);
    ++g_dying_count;
}

uint8_t* ctl_named(const char* nm) {
    for (int r = 0; r < kRowCount; ++r) {
        if (strcmp(kRowSpecs[r].name, nm) == 0) {
            return g_ctl_global[r];
        }
    }
    return NULL;
}

uint8_t* bound_menu(const uint8_t* ctl) {
    uint8_t* menu = NULL;
    if (ctl) {
        memcpy(&menu, ctl + 8, 4);
    }
    return menu;
}

// Close by instance, null-safe, as the boxes' teardowns call it.
void close_instance(const char* nm) {
    uint8_t* menu = bound_menu(ctl_named(nm));
    if (menu) {
        h_close_menu(menu, 1);
    }
}

int16_t word_at(const uint8_t* p, size_t off) {
    int16_t v;
    memcpy(&v, p + off, 2);
    return v;
}

void put_word(uint8_t* p, size_t off, int v) {
    const int16_t s = static_cast<int16_t>(v);
    memcpy(p + off, &s, 2);
}

typedef uint32_t (__fastcall* RequestCloseFn)(uint8_t* ctl, void* edx);

// delivery's request-close: 0x19 closes the family and ends the session;
// 0x15 to 0x17 are left alone; any other state takes the closing row's two
// words.
void delivery_request_close(uint8_t* ctl) {
    const int state = word_at(ctl, 0x1B4);
    if (state == 0 || state == 0x15 || state == 0x16 || state == 0x17) {
        return;
    }
    if (state == 0x19) {
        close_instance("delivery");
        put_word(ctl, 0x1B4, 0);
        put_word(ctl, 0x1B6, 0x18);
        return;
    }
    put_word(ctl, 0x1B6, 0x18);
    put_word(ctl, 0x1B4, 0x15);
}

// The server's 0x04B reply to request 15.
void post_reply() {
    logged('Z');
    uint8_t* post1 = ctl_named("post1");
    if (post1) {
        reinterpret_cast<RequestCloseFn>(&fake_post_request_close)(post1, NULL);
    }
    uint8_t* delivery = ctl_named("delivery");
    if (delivery) {
        delivery_request_close(delivery);
    }
}

void box_ticks() {
    uint8_t* post1 = ctl_named("post1");
    uint32_t state = 0;
    if (bound_menu(post1)) {
        memcpy(&state, post1 + 0x14, 4);
        if (state == 0x12) {
            reinterpret_cast<RequestCloseFn>(&fake_post_request_close)(post1, NULL);
        }
    }
    uint8_t* delivery = ctl_named("delivery");
    if (!bound_menu(delivery)) {
        return;
    }
    switch (word_at(delivery, 0x1B4)) {
    case 0x15:
        logged('u');
        put_word(delivery, 0x1B4, 0x16);
        break;
    case 0x16:
        logged('v');
        put_word(delivery, 0x1B4, word_at(delivery, 0x1B6));
        put_word(delivery, 0x1B6, 0x18);
        break;
    case 0x18:
        h_post_request(0x0F);
        put_word(delivery, 0x1B4, 0x19);
        break;
    default:
        break;
    }
}

// The event's wait on query: once a frame has seen the result word set, the
// next closes query by name.
LONG g_query_seen;

void query_wait() {
    uint8_t* query = ctl_named("query");
    if (!query || word_at(query, 0x548) == 0) {
        return;
    }
    if (!g_query_seen) {
        g_query_seen = 1;
        return;
    }
    g_query_seen = 0;
    InterlockedExchange(&g_query_waits, 0);
    char key[16];
    memcpy(key, "menu    query   ", 16);
    area_close_by_name(key);
}

// The compass draw entry, as the game's update runs it after its own work.
void draw_compass() {
    reinterpret_cast<void (*)()>(&fake_compass_draw)();
}

// The macro key handler's question before a held number key runs its macro:
// the gate through its site, al the answer.
void run_macro_keys() {
    if (g_macro_key && (reinterpret_cast<uint32_t (*)()>(&fake_macro_gate)() & 0xFF)) {
        InterlockedIncrement(&g_macros_fired);
    }
}

int h_frame(int, int, int, int) {
    const LONG now = InterlockedIncrement(&g_frames);
    for (int i = 0; i < g_dying_count;) {
        if (now - g_dying[i].due >= 0) {
            unbind(g_dying[i].menu);
            g_dying[i] = g_dying[--g_dying_count];
        } else {
            ++i;
        }
    }
    if (InterlockedExchange(&g_post_reply, 0)) {
        post_reply();
    }
    if (g_box_ticks) {
        box_ticks();
    }
    if (g_query_waits) {
        query_wait();
    } else {
        g_query_seen = 0;
    }
    draw_compass();
    run_macro_keys();
    return 1;
}

// The compass draw entry's callees: its update (this = the compass), the
// manager's gate (this = the manager; al 1 would skip the render), and its
// render (this = the compass), which records the anchor it drew at.
void __fastcall h_compass_update(uint8_t*, void*) {
    InterlockedIncrement(&g_compass_updates);
}

uint32_t __fastcall h_compass_gate(uint8_t*, void*) {
    return 0;
}

void __fastcall h_compass_render(uint8_t* c, void*) {
    g_log.compass_x = word_at(c, 0x28);
    g_log.compass_y = word_at(c, 0x2A);
    InterlockedIncrement(&g_compass_draws);
}

// The macro key gate's callee (this = the manager): al 1 would say a modal
// dim or the global hide is up, which here never is.
uint32_t __fastcall h_macro_manager(uint8_t*, void*) {
    return 0;
}

// The ability opener's body, standing in for the controller method it hands
// the kind to: stored at the ability controller's +0x64, and recorded.
void __stdcall h_ability_kind(int kind) {
    uint8_t* ctl = ctl_named("ability");
    if (ctl) {
        memcpy(ctl + 0x64, &kind, 4);
    }
    g_ability_last_kind = kind;
}

void h_never() {
}

// The mouse mode picker's callee (this = the manager): a menu is up while the
// active menu is neither dormant nor marked at +0x70, as the game's reads it.
uint32_t __fastcall h_menu_up(uint8_t* mcb, void*) {
    InterlockedIncrement(&g_menu_up_calls);
    uint8_t* active;
    memcpy(&active, mcb + 0x54, 4);
    if (!active) {
        return 0;
    }
    uint32_t state;
    memcpy(&state, active + 0x10, 4);
    return state != 0x0E && active[0x70] == 0 ? 1u : 0u;
}

// The routing routine's callees (this = the manager).
uint32_t __fastcall h_route_suspended(uint8_t* mcb, void*) {
    return mcb[0xB4] == 1 ? 1u : 0u;
}

uint32_t __fastcall h_route_up(uint8_t* mcb, void*) {
    uint8_t* active;
    memcpy(&active, mcb + 0x54, 4);
    if (!active) {
        return 0;
    }
    uint32_t state;
    memcpy(&state, active + 0x10, 4);
    return state != 0x0E && active[0x70] == 0 ? 1u : 0u;
}

void __fastcall h_route_main_menu(uint8_t*, void*) {
    ++g_log.main_menu_opens;
}

// The sink's body past its first 26 bytes.
uint32_t __stdcall h_menu_sink(uint8_t* menu, int code) {
    ++g_log.sink_calls;
    g_log.sink_menu = menu;
    g_log.sink_code = code;
    return 1;
}

uint32_t __cdecl h_glyph_convert(const char* text, int16_t* glyphs, uint32_t max_glyphs, char* raw, int raw_bytes) {
    InterlockedIncrement(&g_convert_calls);
    if (GetCurrentThreadId() != g_game_thread) {
        InterlockedIncrement(&g_convert_elsewhere);
    }
    return fake_glyph_convert(text, glyphs, max_glyphs, raw, raw_bytes);
}

__declspec(dllexport) FakeState* fake_state() {
    static FakeState s;
    s.registry = g_registry;
    s.pristine = g_pristine;
    s.mcb = g_mcb;
    s.ctl_global = g_ctl_global;
    s.frames = &g_frames;
    s.set_position = reinterpret_cast<void*>(&fake_set_position);
    s.open = reinterpret_cast<void*>(&fake_open);
    s.close = reinterpret_cast<void*>(&fake_close);
    s.ui_update = reinterpret_cast<void*>(&fake_ui_update);
    s.staged_close = reinterpret_cast<void*>(&fake_staged_close);
    s.show_path = reinterpret_cast<void*>(&fake_show_path);
    s.spare_set_position = reinterpret_cast<uint8_t*>(&fake_spare_set_position);
    void (*const calls[12])() = {
        &fake_query_confirm, &fake_passinpu_reset, &fake_party_send, &fake_party_clear,
        &fake_link5_clear, &fake_arealist_close, &fake_arealist_latch, &fake_post_request,
        &fake_post_request_close, &fake_dock_reset, &fake_template_swap, &fake_set_frame_rect,
    };
    for (int i = 0; i < 12; ++i) {
        s.calls[i] = reinterpret_cast<void*>(calls[i]);
    }
    s.party_pending = g_party_pending;
    s.party_queues = &g_party_queues;
    s.post_queues = &g_post_queues;
    s.query_cancel_allowed = &g_query_cancel_allowed;
    s.text_callback = reinterpret_cast<void*>(&fake_text_callback);
    s.log = &g_log;
    s.vtable = g_vtable;
    s.link5_cache = g_link5_cache;
    s.link5_callback = reinterpret_cast<void*>(&fake_link5_callback);
    s.link5_site = reinterpret_cast<uint8_t*>(&fake_link5_confirm);
    s.query_cancel_site = reinterpret_cast<uint8_t*>(&fake_query_cancel_site);
    s.set_cursor = reinterpret_cast<void*>(&fake_set_cursor);
    s.link5_latch = &g_link5_latch;
    s.link5_latch_site = reinterpret_cast<uint8_t*>(&fake_link5_latch_site);
    s.link5_open_site = reinterpret_cast<uint8_t*>(&fake_link5_open_site);
    s.game_thread = &g_game_thread;
    s.convert_calls = &g_convert_calls;
    s.convert_elsewhere = &g_convert_elsewhere;
    s.glyph_convert = reinterpret_cast<void*>(&fake_glyph_convert_site);
    s.mouse_mode = reinterpret_cast<void*>(&fake_mouse_mode);
    s.menu_up_calls = &g_menu_up_calls;
    s.row_hit_test = reinterpret_cast<void*>(&fake_row_hit_test);
    s.mouse_read = reinterpret_cast<uint8_t*>(&fake_row_hit_read);
    s.close_frames = &g_close_frames;
    s.query_down = reinterpret_cast<void*>(&fake_query_down);
    s.area_log = &fake_area::g_log;
    s.area_prepare = reinterpret_cast<void*>(&fake_area_prepare);
    s.area_opened = reinterpret_cast<void*>(&fake_area_opened);
    s.area_ctl = g_area_ctl;
    s.area_select = reinterpret_cast<void*>(&h_area_select);
    s.box_ticks = &g_box_ticks;
    s.post_reply = &g_post_reply;
    s.query_waits = &g_query_waits;
    s.menu_input = reinterpret_cast<void*>(&fake_menu_sink);
    s.menu_routing = reinterpret_cast<void*>(&fake_menu_routing);
    s.compass = g_compass;
    s.compass_ptr = &g_compass_ptr;
    s.compass_draw = reinterpret_cast<void*>(&fake_compass_draw);
    s.compass_draws = &g_compass_draws;
    s.compass_updates = &g_compass_updates;
    s.macro_object = g_macro_object;
    s.macro_object_ptr = &g_macro_object_ptr;
    s.macro_gate = reinterpret_cast<void*>(&fake_macro_gate);
    s.macro_ctor_site = reinterpret_cast<uint8_t*>(&fake_macro_ctor_site);
    s.macro_key = &g_macro_key;
    s.macros_fired = &g_macros_fired;
    s.ability_open = reinterpret_cast<void*>(&fake_ability_open);
    s.ability_last_kind = &g_ability_last_kind;
    return &s;
}

}  // extern "C"

// Byte for byte the signatures; wildcards filled with whatever
// the code needs there.
asm(
    ".text\n"
    ".p2align 4\n"
    ".globl _fake_registry_lookup\n"
    "_fake_registry_lookup:\n"
    "  .byte 0xa0\n"
    "  .long _g_registry\n"
    "  .byte 0x53,0x56,0x57,0x33,0xff,0x84,0xc0,0x74,0x00,0x8b,0x5c,0x24,0x10,0xb8\n"
    "  .long _g_registry\n"
    "  .byte 0x8b,0xf0,0x6a,0x10,0x53,0x50\n"
    "  call _h_never\n"
    "  .byte 0x83,0xc4,0x0c,0x85,0xc0,0x74,0x00,0x8a,0x4e,0x2c,0x83,0xc6,0x2c,0xc3\n"

    ".p2align 4\n"
    ".globl _fake_draw_walk\n"
    "_fake_draw_walk:\n"
    "  .byte 0x8d,0x44,0x24,0x0c,0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0x50\n"
    "  call _h_never\n"
    "  .byte 0x8b,0xf0,0x85,0xf6,0x74,0x00,0x38,0x5e,0x60,0x75,0x00,0x8b,0xcf\n"
    "  call _h_never\n"
    "  .byte 0x84,0xc0,0x74,0x00,0xf6,0x46,0x34,0x40,0xc3\n"

    ".p2align 4\n"
    ".globl _fake_dock_trigger\n"
    "_fake_dock_trigger:\n"
    "  .byte 0x8b,0x04,0xbd\n"
    "  .long _g_dock_masks\n"
    "  .byte 0x8b,0x4e,0x34,0x85,0xc1,0x74,0x00,0x56,0x57,0x8b,0xcd\n"
    "  call _h_never\n"
    "  .byte 0xc3\n"

    // SetPosition(this = menu, x, y), ret 8: writes the origin.
    ".p2align 4\n"
    ".globl _fake_set_position\n"
    "_fake_set_position:\n"
    "  .byte 0x83,0xec,0x0c,0x66,0x8b,0x44,0x24,0x14,0x53,0x55,0x8b,0x6c,0x24,0x18,0x56,0x8b,0xf1,0x57\n"
    "  .byte 0x33,0xff,0x8d,0x5e,0x14,0x66,0x89,0x6e,0x52,0x8b,0xcb,0x66,0x89,0x46,0x54\n"
    "  .byte 0x56\n"
    "  call _h_set_position@4\n"
    "  .byte 0x5f,0x5e,0x5d,0x5b,0x83,0xc4,0x0c,0xc2,0x08,0x00\n"

    // A second SetPosition signature, disarmed by its first byte; the harness
    // arms it to make the signature match twice.
    ".p2align 4\n"
    ".globl _fake_spare_set_position\n"
    "_fake_spare_set_position:\n"
    "  .byte 0xcc,0xec,0x0c,0x66,0x8b,0x44,0x24,0x14,0x53,0x55,0x8b,0x6c,0x24,0x18,0x56,0x8b,0xf1,0x57\n"
    "  .byte 0x33,0xff,0x8d,0x5e,0x14,0x66,0x89,0x6e,0x52,0x8b,0xcb,0x66,0x89,0x46,0x54\n"
    "  .byte 0x5f,0x5e,0x5d,0x5b,0x83,0xc4,0x0c,0xc2,0x08,0x00\n"

    // open(this = manager, key, activate, overlap), ret 0xC.
    ".p2align 4\n"
    ".globl _fake_open\n"
    "_fake_open:\n"
    "  .byte 0x8b,0x44,0x24,0x04,0x83,0xec,0x20,0x53,0x56,0x57,0x8b,0xf9,0x8d,0x4c,0x24,0x0c,0x50,0x51,0x8b,0xcf\n"
    "  call _h_normalize@8\n"
    "  .byte 0x8d,0x54,0x24,0x30,0x8d,0x44,0x24,0x0c,0x52,0x50,0x8b,0xcf\n"
    "  call _h_lookup@8\n"
    "  .byte 0x8d,0x44,0x24,0x0c,0x50\n"
    "  call _h_open@4\n"
    "  .byte 0x5f,0x5e,0x5b,0x83,0xc4,0x20,0xc2,0x0c,0x00\n"

    // close by name(this = manager, key), ret 4.
    ".p2align 4\n"
    ".globl _fake_close\n"
    "_fake_close:\n"
    "  .byte 0x8b,0x44,0x24,0x04,0x83,0xec,0x20,0x56,0x8b,0xf1,0x8d,0x4c,0x24,0x04,0x50,0x51,0x8b,0xce\n"
    "  call _h_close_key@8\n"
    "  .byte 0x8d,0x54,0x24,0x04,0x8b,0xce,0x52\n"
    "  call _h_close_find@4\n"
    "  .byte 0x85,0xc0,0x74,0x0a,0x6a,0x01,0x50,0x8b,0xce\n"
    "  call _h_close_menu@8\n"
    "  .byte 0x5e,0x83,0xc4,0x20,0xc2,0x04,0x00\n"

    // The per-frame UI update(this = manager), plain ret.
    ".p2align 4\n"
    ".globl _fake_ui_update\n"
    "_fake_ui_update:\n"
    "  .byte 0x83,0xec,0x14,0x53,0x55,0x56,0x57,0x6a,0xff,0x6a,0x04,0x68,0x8b,0x00,0x00,0x00,0x6a,0x3e,0x8b,0xe9\n"
    "  call _h_frame\n"
    "  .byte 0x83,0xc4,0x10,0x3c,0x01,0x75,0x00\n"
    "  .byte 0x5f,0x5e,0x5d,0x5b,0x83,0xc4,0x14,0xc3\n"

    ".p2align 4\n"
    ".globl _fake_staged_close\n"
    "_fake_staged_close:\n"
    "  .byte 0x53,0x56,0x8b,0x74,0x24,0x0c,0x33,0xdb,0x57,0x3b,0xf3,0x8b,0xf9,0x0f,0x84,0x00,0x00,0x00,0x00,0x8b,0xce\n"
    "  call _h_never\n"
    "  .byte 0x83,0x7e,0x10,0x0e,0x75,0x00,0x5f,0x5e,0x5b,0xc2,0x14,0x00\n"

    ".p2align 4\n"
    ".globl _fake_show_path\n"
    "_fake_show_path:\n"
    "  .byte 0x53,0x55,0x56,0x8b,0x74,0x24,0x10,0x32,0xdb,0x57,0x85,0xf6,0x8b,0xf9,0x0f,0x84,0x00,0x00,0x00,0x00\n"
    "  .byte 0x8a,0x46,0x6e,0x84,0xc0,0x0f,0x85,0x00,0x00,0x00,0x00,0x5f,0x5e,0x5d,0x5b,0xc2,0x04,0x00\n"

    // query confirm(this = ctrl): the game's layout, so its je lands on the
    // failure tail.
    ".p2align 4\n"
    ".globl _fake_query_confirm\n"
    "_fake_query_confirm:\n"
    "  .byte 0x56,0x8b,0xf1,0x57,0x0f,0xbf,0x46,0x30,0x8d,0x7e,0x14,0x50,0x8b,0xcf\n"
    "  call _h_query_row@4\n"
    "  .byte 0x50,0x8b,0xcf\n"
    "  call @h_query_item@12\n"
    "  .byte 0x85,0xc0,0x74,0x13\n"
    "  .byte 0x66,0x8b,0x88,0x04,0x01,0x00,0x00,0x5f,0x66,0x89,0x8e,0x48,0x05,0x00,0x00,0xb0,0x01,0x5e,0xc3\n"
    "  .byte 0x5f,0x32,0xc0,0x5e,0xc3\n"

    // passinpu reset(this = ctrl).
    ".p2align 4\n"
    ".globl _fake_passinpu_reset\n"
    "_fake_passinpu_reset:\n"
    "  .byte 0x33,0xc0,0x89,0x41,0x1c,0x89,0x41,0x20,0x88,0x41,0x24,0x89,0x41,0x14,0x89,0x41,0x18\n"
    "  .byte 0x6a,0x52\n"
    "  call _h_log@4\n"
    "  .byte 0xc3\n"

    // prtyjoin send(accept), cdecl: al 1 when queued. Its je lands on the
    // failure tail, as the game's does.
    ".p2align 4\n"
    ".globl _fake_party_send\n"
    "_fake_party_send:\n"
    "  .byte 0xa1\n"
    "  .long _g_net\n"
    "  .byte 0x85,0xc0,0x74,0x12,0x6a,0x00,0x6a,0x00,0x6a,0x74\n"
    "  call _h_party_alloc\n"
    "  .byte 0x83,0xc4,0x0c,0x85,0xc0,0x75,0x03,0x32,0xc0,0xc3\n"
    "  .byte 0x8b,0x44,0x24,0x04,0x50\n"
    "  call _h_party_sent\n"
    "  .byte 0x83,0xc4,0x04,0xb0,0x01,0xc3\n"

    // prtyjoin clear: zeroes the pending flag; the next routine begins 8a.
    ".p2align 4\n"
    ".globl _fake_party_clear\n"
    "_fake_party_clear:\n"
    "  .byte 0xc6,0x05\n"
    "  .long _g_party_pending\n"
    "  .byte 0x00,0xc3,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90\n"
    "  .byte 0x8a,0x05\n"
    "  .long _g_party_pending\n"
    "  .byte 0xc3\n"

    // link5 clear(this = ctrl): its flag, then the callback with no choice.
    ".p2align 4\n"
    ".globl _fake_link5_clear\n"
    "_fake_link5_clear:\n"
    "  .byte 0x53,0x56,0x8b,0xf1,0x33,0xdb,0x88,0x1d\n"
    "  .long _g_link5_flag\n"
    "  .byte 0x6a,0x4c\n"
    "  call _h_log@4\n"
    "  .byte 0x8b,0x46,0x6c,0x85,0xc0,0x74,0x07,0x53,0x53,0xff,0xd0,0x83,0xc4,0x08\n"
    "  .byte 0x5e,0x5b,0xc3\n"

    // arealist close+reset(this = ctrl): the game's bytes, its close by name
    // through vtable +0x74, then the stand-in's set-rows and the tail that
    // frees the rows.
    ".p2align 4\n"
    ".globl _fake_arealist_close\n"
    "_fake_arealist_close:\n"
    "  .byte 0x56,0x8b,0xf1,0x8b,0x06,0xff,0x50,0x74,0x6a,0x01,0x6a,0x00,0x6a,0x00,0x8b,0xce\n"
    "  .byte 0xc7,0x46,0x34,0x00,0x00,0x00,0x00,0xc6,0x46,0x49,0x01\n"
    "  call @h_area_rows@20\n"
    "  .byte 0x8b,0xce,0x5e,0xe9\n"
    "  .long @h_area_free@8 - (. + 4)\n"
    "  .byte 0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90\n"
    "  .byte 0xa1\n"
    "  .long _g_net\n"
    "  .byte 0x55,0x5d,0xc3\n"

    // arealist latch clear(this = ctrl), byte for byte.
    ".p2align 4\n"
    ".globl _fake_arealist_latch\n"
    "_fake_arealist_latch:\n"
    "  .byte 0x32,0xc0,0x88,0x41,0x6d,0x88,0x41,0x6c,0xc3,0x90,0x90,0x90\n"

    // post-box request(command), cdecl: al 1 when queued; its jl lands on the
    // tail that returns bl.
    ".p2align 4\n"
    ".globl _fake_post_request\n"
    "_fake_post_request:\n"
    "  .byte 0x8b,0x44,0x24,0x04,0x53,0x32,0xdb,0x83,0xf8,0x0d,0x7c,0x25\n"
    "  .byte 0x50\n"
    "  call _h_post_request\n"
    "  .byte 0x83,0xc4,0x04,0x88,0xc3\n"
    "  .fill 26, 1, 0x90\n"
    "  .byte 0x8a,0xc3,0x5b,0xc3\n"

    // post incoming request-close(this = post1's controller).
    ".p2align 4\n"
    ".globl _fake_post_request_close\n"
    "_fake_post_request_close:\n"
    "  .byte 0x56,0x8b,0xf1\n"
    "  call @h_post_active@8\n"
    "  .byte 0x84,0xc0,0x0f,0x84\n"
    "  .long 1f - (. + 4)\n"
    "  .byte 0x8b,0x4e,0x14,0x56\n"
    "  call _h_post_close_rest@4\n"
    "1:\n"
    "  .byte 0xb0,0x01,0x5e,0xc3\n"

    // DockReset(this = manager; group, menu), ret 8.
    ".p2align 4\n"
    ".globl _fake_dock_reset\n"
    "_fake_dock_reset:\n"
    "  .byte 0x8b,0x44,0x24,0x04,0x56,0x8b,0xf1,0x8b,0x4c,0x24,0x0c,0x33,0xd2,0x51,0x50\n"
    "  call _h_dock_reset@8\n"
    "  .byte 0x5e,0xc2,0x08,0x00\n"

    // template swap(this = menu; key), ret 4.
    ".p2align 4\n"
    ".globl _fake_template_swap\n"
    "_fake_template_swap:\n"
    "  .byte 0x83,0xec,0x18,0x53,0x55,0x56,0x57,0x8b,0xf9,0x89,0x7c,0x24,0x14\n"
    "  .byte 0xff,0x74,0x24,0x2c,0x57\n"
    "  call _h_template_swap@8\n"
    "  .byte 0x5f,0x5e,0x5d,0x5b,0x83,0xc4,0x18,0xc2,0x04,0x00\n"

    // SetFrameRect(this = menu; x, y, w, h, 1, keep, b9A), ret 0x1C: the seven
    // arguments pushed again, last first.
    ".p2align 4\n"
    ".globl _fake_set_frame_rect\n"
    "_fake_set_frame_rect:\n"
    "  .byte 0x83,0xec,0x08,0x56,0x8b,0xf1,0x8d,0x4e,0x14\n"
    "  call @h_list_head@8\n"
    "  .byte 0x85,0xc0,0x89,0x44,0x24,0x08\n"
    "  .byte 0xff,0x74,0x24,0x28,0xff,0x74,0x24,0x28,0xff,0x74,0x24,0x28,0xff,0x74,0x24,0x28\n"
    "  .byte 0xff,0x74,0x24,0x28,0xff,0x74,0x24,0x28,0xff,0x74,0x24,0x28,0x56\n"
    "  call _h_frame_rect@32\n"
    "  .byte 0x5e,0x83,0xc4,0x08,0xc2,0x1c,0x00\n"

    // link5's own confirm from the slot on: the callback with the slot's
    // entry in the concierge cache.
    ".p2align 4\n"
    ".globl _fake_link5_confirm\n"
    "_fake_link5_confirm:\n"
    "  .byte 0x8b,0x4e,0x6c,0x83,0xc4,0x04,0x85,0xc9,0x74,0x17,0x83,0xf8,0x10,0x73,0x12\n"
    "  .byte 0x8b,0xd0,0xc1,0xe2,0x05,0x81,0xc2\n"
    "  .long _g_link5_cache\n"
    "  .byte 0x52,0x50,0xff,0xd1,0x83,0xc4,0x08,0xc3\n"

    // query's input handler, its cancel: the answer only while the byte is 1.
    ".p2align 4\n"
    ".globl _fake_query_cancel_site\n"
    "_fake_query_cancel_site:\n"
    "  .byte 0x80,0x3d\n"
    "  .long _g_query_cancel_allowed\n"
    "  .byte 0x01,0x75,0x09,0x66,0xc7,0x86,0x48,0x05,0x00,0x00,0xff,0x00,0xc3\n"

    // SetCursor(this = menu; row, warp), ret 8.
    ".p2align 4\n"
    ".globl _fake_set_cursor\n"
    "_fake_set_cursor:\n"
    "  .byte 0x83,0xec,0x08,0x56,0x57,0x8b,0x7c,0x24,0x14,0x8b,0xf1,0x66,0x85,0xff\n"
    "  .byte 0xff,0x74,0x24,0x18,0x57,0x56\n"
    "  call _h_set_cursor@12\n"
    "  .byte 0x5f,0x5e,0x83,0xc4,0x08,0xc2,0x08,0x00\n"

    // The event's wait test: true once the callback has cleared the latch.
    ".p2align 4\n"
    ".globl _fake_link5_latch_site\n"
    "_fake_link5_latch_site:\n"
    "  .byte 0x8a,0x0d\n"
    "  .long _g_link5_latch\n"
    "  .byte 0x33,0xc0,0x84,0xc9,0x0f,0x94,0xc0,0xc3\n"

    // The 0x71 sub 0x40 handler's open of link5: link5's controller global
    // (its registry row, 121, in menu_table.inc) and the
    // event's callback.
    ".p2align 4\n"
    ".globl _fake_link5_open_site\n"
    "_fake_link5_open_site:\n"
    "  call _h_never\n"
    "  .byte 0x6a,0x02,0x8b,0xce\n"
    "  call _h_never\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_ctl_global + 4 * 121\n"
    "  .byte 0x50,0x68\n"
    "  .long _fake_link5_callback\n"
    "  .byte 0x6a,0x01\n"
    "  call _h_never\n"
    "  .byte 0xe9,0x00,0x00,0x00,0x00\n"
    "  call _h_never\n"
    "  .byte 0x84,0xc0,0x0f,0x84,0x00,0x00,0x00,0x00,0xc3\n"

    // The text-to-glyph converter(text, glyphs, max_glyphs, raw, raw_bytes),
    // cdecl: its two early returns, then its body.
    ".p2align 4\n"
    ".globl _fake_glyph_convert_site\n"
    "_fake_glyph_convert_site:\n"
    "  .byte 0x55,0x8b,0x6c,0x24,0x0c,0x85,0xed,0x75,0x04,0x33,0xc0,0x5d,0xc3\n"
    "  .byte 0x8b,0x44,0x24,0x10,0x85,0xc0,0x75,0x02,0x5d,0xc3\n"
    "  .byte 0x53,0x56,0x57,0x8d,0x48,0xff,0x32,0xdb,0x33,0xff\n"
    "  .byte 0x5f,0x5e,0x5b,0x5d\n"
    "  jmp _h_glyph_convert\n"

    // The mouse mode picker(ecx = the mouse object), plain ret, byte for byte
    // the game's: its callee on the manager, then +0x4D 2 or 1.
    ".p2align 4\n"
    ".globl _fake_mouse_mode\n"
    "_fake_mouse_mode:\n"
    "  .byte 0x56,0x8b,0xf1,0xb9\n"
    "  .long _g_mcb\n"
    "  call @h_menu_up@8\n"
    "  .byte 0x84,0xc0,0x74,0x06,0xc6,0x46,0x4d,0x02,0x5e,0xc3,0xc6,0x46,0x4d,0x01,0x5e,0xc3\n"

    // The row hit test(this = menu; point, selectable), ret 8, and in its
    // loop the test of the menu's +0x77.
    ".p2align 4\n"
    ".globl _fake_row_hit_test\n"
    "_fake_row_hit_test:\n"
    "  .byte 0x83,0xec,0x08,0x53,0x55,0x56,0x8b,0xf1,0x8b,0x0d\n"
    "  .long _g_net\n"
    "  .byte 0x57,0x89,0x74,0x24,0x10,0xd9,0x41,0x48,0xd8,0x1d\n"
    "  .long _g_net\n"
    "  .byte 0xdf,0xe0,0x25,0x00,0x41,0x00,0x00,0x0f,0x84,0x00,0x00,0x00,0x00\n"
    "  .byte 0x8a,0x41,0x4e,0x84,0xc0,0x0f,0x84,0x00,0x00,0x00,0x00,0x8d,0x4e,0x14\n"
    "  call _h_never\n"
    ".globl _fake_row_hit_read\n"
    "_fake_row_hit_read:\n"
    "  .byte 0x8b,0x74,0x24,0x10,0x8a,0x4e,0x77,0x84,0xc9,0x74,0x00\n"
    "  .byte 0x5f,0x5e,0x5d,0x5b,0x83,0xc4,0x08,0xc2,0x08,0x00\n"

    // The menu input sink(this = menu; code), ret 4: the game's first 26
    // bytes, a dormant menu answered 0 there, then the stand-in's body.
    ".p2align 4\n"
    ".globl _fake_menu_sink\n"
    "_fake_menu_sink:\n"
    "  .byte 0x51,0x55,0x56,0x8b,0xf1,0x57,0x83,0x7e,0x10,0x0e,0x75,0x09,0x5f,0x5e,0x33,0xc0,0x5d,0x59\n"
    "  .byte 0xc2,0x04,0x00,0x66,0x83,0x7e,0x56,0x00\n"
    "  .byte 0xff,0x74,0x24,0x14,0x56\n"
    "  call _h_menu_sink@8\n"
    "  .byte 0x5f,0x5e,0x5d,0x59,0xc2,0x04,0x00\n"

    // The routing routine(this = manager; code), ret 4, byte for byte the
    // game's: the code to the active menu's sink while a menu is up and input
    // is not suspended, else code 7 opens the main menu.
    ".p2align 4\n"
    ".globl _fake_menu_routing\n"
    "_fake_menu_routing:\n"
    "  .byte 0x56,0x8b,0xf1\n"
    "  call @h_route_suspended@8\n"
    "  .byte 0x84,0xc0,0x75,0x3a,0x8b,0xce\n"
    "  call @h_route_up@8\n"
    "  .byte 0x84,0xc0,0x74,0x20,0x8b,0xce\n"
    "  call @h_route_suspended@8\n"
    "  .byte 0x84,0xc0,0x75,0x15,0x8b,0x44,0x24,0x08,0x8b,0x4e,0x54,0x50\n"
    "  call _fake_menu_sink\n"
    "  .byte 0x85,0xc0,0x74,0x13,0x5e,0xc2,0x04,0x00\n"
    "  .byte 0x66,0x83,0x7c,0x24,0x08,0x07,0x75,0x07,0x8b,0xce\n"
    "  call @h_route_main_menu@8\n"
    "  .byte 0x33,0xc0,0x5e,0xc2,0x04,0x00\n"

    // The compass draw entry (no arguments), plain ret, the game's shape: the
    // compass through its global, nothing when it is NULL; its update; the
    // manager's gate, al 1 skipping the render; the render by a jump. Both
    // je land on the ret, as the game's do.
    ".p2align 4\n"
    ".globl _fake_compass_draw\n"
    "_fake_compass_draw:\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_compass_ptr\n"
    "  .byte 0x85,0xc9,0x74,0x1e\n"
    "  call @h_compass_update@8\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  call @h_compass_gate@8\n"
    "  .byte 0x3c,0x01,0x74,0x0b\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_compass_ptr\n"
    "  .byte 0xe9\n"
    "  .long @h_compass_render@8 - (. + 4)\n"
    "  .byte 0xc3\n"

    // The macro key gate (no arguments, al the answer), the game's shape: the
    // player index word, its entity by the pointer table, the world state
    // 0x60 through its pointer, the manager's callee, then a body that says
    // yes. The je at +12 and both jne land on the tail that says no, as the
    // game's do.
    ".p2align 4\n"
    ".globl _fake_macro_gate\n"
    "_fake_macro_gate:\n"
    "  .byte 0x66,0xa1\n"
    "  .long _g_macro_player\n"
    "  .byte 0x56,0x33,0xf6,0x66,0x85,0xc0,0x74\n"
    "  .byte 1f - (. + 1)\n"
    "  .byte 0x0f,0xbf,0xc0,0x8b,0x34,0x85\n"
    "  .long _g_macro_table\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_macro_world_ptr\n"
    "  .byte 0x83,0x39,0x60,0x0f,0x85\n"
    "  .long 1f - (. + 4)\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  call @h_macro_manager@8\n"
    "  .byte 0x84,0xc0,0x0f,0x85\n"
    "  .long 1f - (. + 4)\n"
    "  .byte 0xb0,0x01,0x5e,0xc3\n"
    "1:\n"
    "  .byte 0x32,0xc0,0x5e,0xc3\n"

    // The macro subsystem's constructor site, never run: the object's global
    // written at +15 and again at +23, the two imm32s the engine compares.
    ".p2align 4\n"
    ".globl _fake_macro_ctor_site\n"
    "_fake_macro_ctor_site:\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_net\n"
    "  .byte 0x51,0x8b,0xc8\n"
    "  call _h_never\n"
    "  .byte 0xa3\n"
    "  .long _g_macro_object_ptr\n"
    "  .byte 0xeb,0x06,0x89,0x1d\n"
    "  .long _g_macro_object_ptr\n"
    "  .byte 0x6a,0x50\n"
    "  call _h_never\n"
    "  .byte 0xc3\n"

    // The ability opener(kind, flag, extra), cdecl, nothing returned, the
    // game's shape: the player global, abisortw opened by name when flag is
    // 1 and ability by name, both through the open routine with the manager
    // in ecx, then the stand-in for the controller method storing the kind.
    // The je at +7 lands on the ret and the jne at +17 past the abisortw
    // open, as the game's do.
    ".p2align 4\n"
    ".globl _fake_ability_open\n"
    "_fake_ability_open:\n"
    "  .byte 0xa1\n"
    "  .long _g_ability_player\n"
    "  .byte 0x85,0xc0,0x74\n"
    "  .byte 1f - (. + 1)\n"
    "  .byte 0x53,0x8b,0x5c,0x24,0x0c,0x80,0xfb,0x01,0x75\n"
    "  .byte 2f - (. + 1)\n"
    "  .byte 0x6a,0x00,0x6a,0x01,0x68\n"
    "  .long _g_key_abisortw\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  call _fake_open\n"
    "2:\n"
    "  .byte 0x6a,0x00,0x6a,0x01,0x68\n"
    "  .long _g_key_ability\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  call _fake_open\n"
    "  .byte 0xff,0x74,0x24,0x08\n"
    "  call _h_ability_kind@4\n"
    "  .byte 0x5b\n"
    "1:\n"
    "  .byte 0xc3\n"
);

namespace {

// A string as the game's text converter writes it: a character is itself
// less 0x20, so a space is 0; \1 switches to green (-0x102) and \2 back to
// the default color (-0x101), \3 is a code the game does not draw (-0x7F),
// and \4 and \5 are the two-byte auto-translate brackets EF 27 and EF 28
// (0x211D, 0x211E). Writes the codes and the 0 after them; returns the count.
int put_glyphs(uint8_t* at, const char* text) {
    static const int16_t marks[] = {0, -0x102, -0x101, -0x7F, 0x211D, 0x211E};
    int n = 0;
    for (const char* c = text; *c; ++c, ++n) {
        const int16_t g = *c < 6 ? marks[static_cast<int>(*c)] : static_cast<int16_t>(*c - 0x20);
        memcpy(at + 2 * n, &g, 2);
    }
    const int16_t zero = 0;
    memcpy(at + 2 * n, &zero, 2);
    return n;
}

// What the event's text leaves in query's controller: the title's glyphs at
// +0x38 (their count at +0x36) and the options in the list at +0x14 (count
// at +0x24), each node +0 next, +0x10 the item, +0x14 set when tombstoned,
// each item 0x108 bytes with its glyphs at +4, its value at +0x104, its
// glyph count at +0x106 and its line count at +0x107. Values 1, 3, 4 and 5;
// the option valued 3 is tombstoned. The cursor (+0x30) and the first option
// shown (+0x32) on the first.
void give_query_choice(uint8_t* ctl) {
    static const char* const texts[kChoiceOptions] = {
        "Yes, \1gladly\2.", "(tombstoned)", "Not\3 today.", "\4Cancel\5"};
    static const uint16_t values[kChoiceOptions] = {1, 3, 4, 5};
    ctl[0x36] = static_cast<uint8_t>(put_glyphs(ctl + 0x38, "Will you lend a hand?"));
    for (int i = 0; i < kChoiceOptions; ++i) {
        uint8_t* node = g_option_nodes[i];
        uint8_t* item = g_options[i];
        const uint8_t* next = i + 1 < kChoiceOptions ? g_option_nodes[i + 1] : NULL;
        memcpy(node, &next, 4);
        memcpy(node + 0x10, &item, 4);
        node[0x14] = i == 1 ? 1 : 0;
        item[0x106] = static_cast<uint8_t>(put_glyphs(item + 4, texts[i]));
        memcpy(item + 0x104, &values[i], 2);
        item[0x107] = 1;
    }
    const uint8_t* head = g_option_nodes[0];
    memcpy(ctl + 0x14, &head, 4);
    const int32_t count = kChoiceOptions;
    memcpy(ctl + 0x24, &count, 4);
    memset(ctl + 0x30, 0, 4);
}

// The expected registry, each row's controller slot pointing into this
// image, and controllers for the pieces the harness uses.
void build_world() {
    for (int r = 0; r < kRowCount; ++r) {
        uint8_t* p = g_registry + r * kRowStride;
        const RowSpec& s = kRowSpecs[r];
        memcpy(p, s.type, 8);
        memset(p + 8, ' ', 8);
        memcpy(p + 8, s.name, strlen(s.name));
        uint8_t** slot = &g_ctl_global[r];
        memcpy(p + kRowSlot, &slot, 4);
        memcpy(p + kRowPolicy, &s.policy, 4);
        p[kRowAvail] = static_cast<uint8_t>(s.avail < 0 ? 0 : s.avail);
        p[kRowLayer] = s.layer;
        const uint16_t overlap = static_cast<uint16_t>(s.overlap < 0 ? 0 : s.overlap);
        memcpy(p + kRowOverlap, &overlap, 2);
    }
    memcpy(g_pristine, g_registry, sizeof(g_registry));

    g_vtable[5] = g_self_refresh;
    g_vtable[0x18 / 4] = reinterpret_cast<const void*>(&fake_query_input);
    memcpy(g_box_vtable, g_vtable, sizeof(g_vtable));
    g_box_vtable[0x18 / 4] = reinterpret_cast<const void*>(&fake_box_input);
    memcpy(g_area_vtable, g_vtable, sizeof(g_vtable));
    fake_area::fill_vtable(g_area_vtable);
    fake_area::install(&area_close_by_name, &area_select);
    const char* pieces[] = {"logwindo", "ability", "equip", "menuwind", "query", "buff",
                            "passinpu", "prtyjoin", "link5", "arealist", "scsibori", "delivery",
                            "post1", "post2", "partywin", "playermo", "targetwi", "subwindo",
                            "dbdelsel", "mcr1pall", "mcr2pall", "abisortw"};
    for (size_t i = 0; i < sizeof(pieces) / sizeof(pieces[0]); ++i) {
        for (int r = 0; r < kRowCount; ++r) {
            if (strcmp(kRowSpecs[r].name, pieces[i]) == 0) {
                uint8_t* ctl = g_ctl[g_ctl_used++];
                const bool area = strcmp(pieces[i], "arealist") == 0;
                const void* vt = strcmp(pieces[i], "post1") == 0 ? g_box_vtable : area ? g_area_vtable : g_vtable;
                memcpy(ctl, &vt, 4);
                g_ctl_global[r] = ctl;
                if (area) {
                    g_area_ctl = ctl;
                }
                if (strcmp(pieces[i], "query") == 0) {
                    give_query_choice(ctl);
                }
                break;
            }
        }
    }
    const uint16_t w = 1920;
    const uint16_t h = 1080;
    memcpy(g_mcb + 0x80, &w, 2);
    memcpy(g_mcb + 0x82, &h, 2);

    g_compass[0x0D] = 3;
    put_word(g_compass, 0x28, 104);
    put_word(g_compass, 0x2A, 1058);
    put_word(g_compass, 0x2C, 42);
    g_compass[0x2E] = 1;
    g_compass_ptr = g_compass;

    g_macro_object_ptr = g_macro_object;
    g_macro_table[1] = g_mcb;
}

}  // namespace

extern "C" BOOL WINAPI DllMain(HINSTANCE, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH) {
        build_world();
    }
    return TRUE;
}
