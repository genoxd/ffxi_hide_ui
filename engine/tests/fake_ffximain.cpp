// fake_ffximain - a stand-in FFXiMain.dll for the end-to-end harness. Its
// .text carries the 147 signatures from signatures.h, each
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
// game's callers do, cdecl (kind, flag, extra). The yes/no class: its
// constructor site, never run, names this image's vtable for it; comyn,
// merityn, shopbuy and shopsell each have a controller of it keyed to its
// row, and the resyn family one, keyed to resynus, in all four rows. Its ask,
// run on the game thread, stores the question and opens the key through the
// open routine, clearing the callback when the open is refused, then puts
// the cursor on no through select-row (the game's bytes around a body that
// writes +0x4C) unless the default is yes; a chaining callback asks again
// from inside itself as the game's re-ask does. Its begin-close drops the
// question unless one asked again waits; its input routine counts any call.
// The class's confirm is the game's bytes, never run. The KO window has a
// controller of its own; its confirm site, never run, is the game's bytes
// around a stand-in decider (al g_dead_set), message getter and the class's
// ask as the engine calls it, the game's test of a pending question before a
// body that records its arguments and asks as the harness's askers do; the callback it names is the game's bytes, marking
// +0x24 of dead's controller on a yes; its tick, never run, the game's
// bytes around the decider. The class's input routine jumps to the body
// that counts calls, its cancel's test and call (the game's bytes) behind
// that jump. The number boxes: moneyctr and itemctrl have controllers of
// their own, and so do the windows that ask through them; each box's setup
// and confirm are the game's bytes, run by the harness's askers and the
// engine's replies, around stand-in callees: the inventory's gil record (its
// +4 the gil a max of 0 resolves to), the open routine, and close by
// instance. The sixteen askers are the game's bytes too: nine callbacks,
// every one never run but trade's item count, whose staging and refocus are
// stand-ins that record what they were given; and seven sites, never run,
// each pushing a stand-in callback of its own that records its number.
// The Asked windows (sortyn, evitem01, confyn, bankmenu, maplist, gmtell,
// mp_dead, ok) have controllers of their own, and so do the windows that ask
// them (iuse, evitem, map0); fake_asked_ask asks each as the game's askers
// do, on the game thread, opening the asking window first and making it the
// active menu. Run as the game's bytes: sortyn's setup (its open, then
// select-row), evitem01's setup and callback, bankmenu's begin-close and
// clear, and maplist's close+reset (its class's close by name and stand-in
// set-rows and free-rows); never run: sortyn's confirm, the key item list's
// ask, evitem01's, confyn's, bankmenu's, mp_dead's and ok's input routines,
// bankmenu's open hook and Mog Safe 2 test, maplist's key handler and the
// GM tell poller, each around this image's globals and close by instance.
// The callbacks and confyn's restore record what they were given. Mog Safe
// 2's flag is bit 0 of the player data at kFakeSafe2Flag; the GM tell queue
// and its slot are this image's data. While the harness sets gm_polls, each
// frame closes gmtell once its result word is set, recording the word; while
// it sets error_screen, each frame closes ok once +0x25C is 1, counting it.
// Close by instance (close by name's call at +44) takes a NULL menu.
// map0's input routine, never run, is the game's bytes around this image's
// manager, close by instance, mapframe's global and the map's close; the
// map's close, run, is the game's head and closes by name around a stand-in
// that records the frame, and its call of the frame's fade-out, a stand-in
// that closes mapframe by instance at once. mapframe and mapv2 have
// controllers of their own.
// The UI update asks the key test (0x3E, 0x8B, 4, -1), the game's prologue
// and mute test before a body that runs the frame, and after it runs the
// dialogue boxes' ticks while the harness sets dialogue_ticks. mogdoor's
// controller is on its class's vtable in read-only data: its input routine
// (the game's bytes around the stand-ins' calls), the door's handler and the
// routine an exit gives the door are never run; the open runs its open hook
// (the game's bytes), each frame it is up runs its tick, and the engine runs
// its floor read (the game's bytes), the confirm's gate and the door actor's
// open, stand-ins that record what they were given; the message getter
// answers its draw's texts. fake_asked_ask's 8 is the door's handler. The
// dialogue boxes, on two vtables in read-only data, each have a text object
// and a chat log; fake_asked_ask's 9 and 10 have rem4line or rem4li2 show a
// message's first page (opening the box) or end it (closing it). Their input
// routine, handler and text dispatch, which hands each code to the other
// box's text object too, are the game's bytes, never run, the dispatch's
// gate recording each code it is handed; the clear is run by the engine,
// and so are the gate's advance, a stand-in that shows the next page or ends
// the message, and its page-turn sound, a stand-in that counts its calls;
// the boxes' tick asks the key test for the left button as the game's does
// and moves the page on when it says so.
// The text-entry windows (stringdl, comment, msgline, scname, link11)
// and passinpu: their input routines, never run, and their fields' ends,
// run by the engine, are the game's bytes around stand-ins that record what
// they were given (the text entry, the fields, msgline's senders, the
// search, the linkshell request); passinpu's open hook, start and
// begin-close and scname's and link11's begin-closes are the game's bytes
// too, run by the open and the close; comment's open hook and start and
// msgline's open hook and start are the bytes the engine reads around
// stand-ins that start the fields; comment's and msgline's begin-closes are
// stand-ins behind the bytes the engine reads. The text entry's manager,
// its bind and unbind and the field objects behave as the game's, and
// passinpu's and msgline's draws read their fields as the game's do; a
// read, bind or delete of a NULL or deleted field is recorded where the
// game's would fault or free twice.
// Their vtables are in read-only data; stringdl and link6 have plain
// controllers. The per-menu draw, the sprite draw and the quad routine are
// the game's heads around stand-ins, and the draw primitive is the game's
// bytes: the per-menu draw's update ends a close (below), its head counts
// the input wait down, and its draw puts the cursor's row on +8 and draws
// the menu's frame through the quad routine; while it draws its
// controller, the target window's controller's draw has one quad of content
// and its two arrows, the game's bytes calling the sprite draw (one quad
// each) on the sprite at the controller's +0x78, and conq3pts' controller's
// draw closes it by name while conq_data is NULL, as the game's does, and
// has one quad of content otherwise. The quad routine draws through the draw
// primitive, called at +0x231 as the game's is, and the device's
// DrawPrimitiveUP behind it logs each sprite's tag in order and the menu
// drawing. While the harness sets draw_walk, each frame walks the live menus
// of layers 0..8 through the per-menu draw after the update, as the game's
// walk does; walk_now runs one such walk on the calling thread, and each
// walk logs the menus it called the per-menu draw for and those whose draw
// ran past its head. While the harness sets close_draws, a close ends as
// the game's does: the staged close leaves the menu dormant with its two
// countdowns, close_frames and 1, which the per-menu draw's update runs out
// one a frame before it marks +0x70; the update frees an instance once it is
// marked, fills it with 0xDD and logs it, and map0's callback opens map0
// again with its input wait open_wait. Each open focuses its menu, and each
// frame relinks the menus focused since by their draw layers as the game's
// update does; while the harness sets active_pick, the frame then makes the
// menu the game's pick finds the active one. party_swap and action_swap
// have the next frame's owners of partywin and playermo swap their
// templates through the swap's entry. The trade windows' routines are the
// game's bytes, generated from the 05-10 image, over this image's player
// data, packet queue, clock, actor table, item cache and inventor modes: the
// stages, OKs, cancel and closes, the send, the allocator, the clock and the
// item record run; the input routines, the trade's open, the actor name's
// reader and the inventory list's row build never do. The handover and
// trade gil askers are the sites inside those input routines, as the game's
// are. The ability opener's body takes its three arguments and opens
// subwindo for a list with a pane (every one but 4, as the game's table
// gives); while the harness sets no_pet, each frame closes ability showing
// a pet's list (2, 6, 10, 18) by name. The magic opener is the game's bytes
// around a stand-in for its controller's rebuild, which writes the sort
// window's controller at +0xC0 (NULL with no sort), opens mgcsortw
// (blusortw for 6) with a sort of 1 and subwindo every time; magic's class
// has a begin-close that counts a +0xC0 it did not write. The equipment
// set's opener is the game's bytes and layout around stand-in tests,
// refusal printer, list resets, list object and inventor hand-over (which
// opens inventor and iteminfo) and 0x061 request, its guard reading equip's
// controller's +0x1A; the equipment shortcut, never run, and the
// Monstrosity check, run, are the game's bytes, the check reading the
// stand-in player data's word +0x10 and mode byte. The routes' sites, the
// menu rows and commands the engine reads its routes from, are the game's
// bytes, never run; the routines it runs are the game's bytes where they
// are small (the storage list's, the post menu's, jbpcat's, fp_cat2's,
// comgenre's, the comment sub-lists' and the timers' openers, the mount
// list's opener and setup, the Job Points test, the missions menu's
// setter, scanlist's opener, the macro editor's pick and close), each open
// by name through the open routine, and otherwise stand-ins that record
// what they were given in FakeRoutes and open what the game's open, with
// the bytes the engine reads at the game's offsets after their entries.
// The tests the routes ask (the Mog House's and the Nomad Moogle service's
// managers, a job's level, the expansions, the map's tracking, the mail
// list's refusal) answer what the harness sets. The item data the
// inventory list's rows read answers a summary whose type word is at +4,
// the summary's builder never run but read. The config store is the game's
// object and entries, every id as the game defines it, each with one
// subscriber that records; its set, read, lookup, store and notify are the
// game's bytes, and so are the Config pages' rows the engine reads its
// routines from and the tables they name; the equipment lock's request, its
// wait and the idle timer are the game's bytes around stand-ins that record
// what they were given, the lock's gate answering lock_busy, its clock
// lock_clock and its send lock_ok; the player flags'
// read is the game's bytes on this image's player data, and the flag's
// request its head before a stand-in that records.

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
int32_t g_ability_last[3] = {-1, -1, -1};   // and all three of its arguments
char g_key_magic[17] = "menu    magic   ";  // the magic opener's key push
char g_key_persona[17] = "menu    persona ";  // the equipment set's opener's key pushes, g_key_money's too
char g_key_equip[17] = "menu    equip   ";
// The other sets' openers' stand-ins: each opener's calls by kSets*, the
// last one's `this` and arguments, its own test's refusal, and the checks
// asked before them.
enum { kSetsMap, kSetsSpells, kSetsAttachments, kSetsLinkshell, kSetsInstincts, kSetsSynthesis, kSetsTabula,
       kSetsCount };
struct FakeSets {
    volatile LONG calls[kSetsCount];
    const void* self;
    int32_t args[3];
    uint8_t refuse;             // the map's, the synthesis's opener refuses
    uint8_t linkshell_check;    // the linkshell row's check's al: 1 refuses
    uint8_t use_check;          // the item use's check's al: 0 refuses
    volatile LONG checks;
    int32_t check_args[2];
};
// The routes' stand-ins: the order they ran in, the last one's `this` and
// arguments, and what the tests they stand for answer.
struct FakeRoutes {
    volatile LONG calls;
    char order[256];            // the stand-ins run, names joined by ' '
    const void* self;
    int32_t args[7];
    uint32_t msglist;           // al of the mail list's opener
    uint8_t job_level[23];      // each job's level, which the Job Points test reads
    uint32_t expansion;         // the expansions on, a bit each
    int32_t tracking;           // the map's tracking word
    uint8_t untracked;          // set: the tracking routine answers NULL
    uint8_t bag_size[18];       // each bag's size, slot 0 counted
    volatile LONG switch_sounds;    // the palette switch's sound
    int32_t scan_answer;        // the Wide Scan question's callback's answer, -1 before one
};
// The settings' stand-ins: what the store's subscribers, the equipment
// lock's request, the idle timer and the player flag's request were given.
struct FakeSettings {
    uint8_t* store;             // the config store: +4 its entries, +9 its loaded byte
    uint8_t* entries;           // 0xD0 of 0x2C bytes, by id
    volatile LONG notifies;     // the subscribers' calls
    int32_t notified[64][2];    // the first 64 since the harness cleared notifies: the id and the value
    volatile LONG lock_requests;
    int32_t lock_args[2];       // the request's mode and its last argument
    uint8_t lock_ok;            // al of the request: it went
    uint8_t lock_busy;          // al of the gate: refused before the wait
    volatile LONG lock_messages;
    uint32_t lock_clock;        // the clock the wait and the stamp read, in ms
    uint32_t* lock_stamp;       // the stamp the request writes the clock in as it goes on to send
    volatile LONG idle_ms_calls;
    int32_t idle_ms;
    volatile LONG idle_on_calls;
    int32_t idle_on;
    volatile LONG flag_sets;
    int32_t flag_args[2];       // the flag and on
    uint8_t* player_flags;      // the byte of the player's data the flags' read takes bit 2 of
    uint8_t** store_ptr;        // the game's global naming the store
    const void* set;            // the store's set by id
    const void* lock;           // the equipment lock's request
    const void* idle;           // the idle timer
    const void* flag;           // the player flag's request
};

uint8_t g_job_flags = 1;               // bit 0: the player's jobs known; then the main and the sub job
uint8_t g_main_job = 0x10;
uint8_t g_sub_job = 0x12;
const size_t kFakeItemType = 0x1B4;    // the item data's pointer to its type word
uint16_t g_set_types[4] = {8, 0x16, 0x17, 1};
uint8_t g_set_items[4][0x200];
uint8_t g_macro_editor[0x10];          // the macro editor object: +0x0C the palette last used, 1 the first
uint8_t* g_macro_editor_ptr = g_macro_editor;
uint8_t g_map_object[0x10];            // the global the map's opener hands its position to
uint8_t* g_map_object_ptr = g_map_object;
const void* g_blu_list;                // the lists bluinven is handed
const void* g_pup_list;
char g_key_mapframe[17] = "menu    mapframe";  // the sets' openers' key pushes
char g_key_map0[17] = "menu    map0    ";
char g_key_bluepoin[17] = "menu    bluepoin";
char g_key_bluequip[17] = "menu    bluequip";
char g_key_statpup[17] = "menu    statpup ";
char g_key_automato[17] = "menu    automato";
char g_key_link5[17] = "menu    link5   ";
char g_key_mp_stat[17] = "menu    mp_stat ";
char g_key_mp_cstm[17] = "menu    mp_cstm ";
char g_key_mp_mncst[17] = "menu    mp_mncst";
char g_key_tskill1[17] = "menu    tskill1 ";
char g_key_prvdungp[17] = "menu    prvdungp";
char g_key_mcrbedit[17] = "menu    mcrbedit";
char g_key_mcr1edit[17] = "menu    mcr1edit";
char g_key_mcr2edit[17] = "menu    mcr2edit";
uint8_t g_mon_player[0x20];            // the player data the Monstrosity check reads, its word at +0x10
uint8_t* g_mon_player_ptr = g_mon_player;
uint8_t g_game_mode;                   // the game's mode byte the check compares with 0x17
uint8_t g_set_refusal;                 // the equipment set's opener's first test's answer
volatile LONG g_no_pet;                // set by the harness: no pet out
volatile LONG g_equipment_opens;       // the equipment set's opener got past its guard
volatile LONG g_stats_requests;        // its 0x061 request
volatile LONG g_magic_lists;           // the magic opener's controller step
int32_t g_magic_last[3] = {-1, -1, -1};
volatile LONG g_magic_closes;
volatile LONG g_magic_close_wild;
const void* g_magic_vtable[0x20];
void* g_list_resets[9];                // the nine lists the equipment set's opener resets, each a stand-in
uint8_t g_equipment_list[8];           // what it hands inventor: an object on g_equipment_vtable
uint8_t* g_equipment_list_ptr = g_equipment_list;
const void* g_equipment_vtable[0x20];
void* g_chat_owner;                    // the global its refusals' printer is called on
const void* g_yesno_vtable[0x20];      // the yes/no class's, which its constructor site names
uint8_t g_dead_set;                    // the KO window's decider answers it in al, the rest of eax dirty
char g_key_moneyctr[17] = "menu    moneyctr";   // the number boxes' setups' key pushes
char g_key_itemctrl[17] = "menu    itemctrl";
char g_key_sortyn[17] = "menu    sortyn  ";     // sortyn's setup's and confirm's key pushes
char g_key_evitem01[17] = "menu    evitem01";   // the key item list's ask's
uint8_t g_player_data[0x40];           // the player's data: the Mog Safe 2 flag at kFakeSafe2Flag, bit 0
uint8_t* g_player_data_ptr = g_player_data;     // the game's global naming it
uint8_t g_gm_queue[4 * 0x140];         // the GM tell queue: each slot the GM's name, then the title's pointer
int32_t g_gm_slot;                     // its current slot
char g_key_mogdoor[17] = "menu    mogdoor ";    // the door's handler's key push
uint8_t g_house[0x40];                 // the Mog House object: +0x38 its door's actor
uint8_t* g_house_ptr = g_house;        // the game's global naming it
uint8_t g_door_actor[0x900];           // the door's actor: +0x880 the routine an exit gives it
uint8_t g_door_exiting[3];             // the three bytes an exit sets
uint32_t g_door_gate_answer = 0xABCD0001u;     // the confirm's gate: al the answer, the rest of eax dirty
volatile LONG g_door_garden_on = 1;    // what the tick enables: the Mog Garden's row, the floor row
volatile LONG g_door_floor_on = 1;
uint32_t g_door_condition = 0xABCD0001u;       // the draw's condition, al
const uint16_t g_door_home[] = {230, 231, 232, 280, 0};    // a nation's list of areas, its Mog Garden last
const uint16_t g_door_whitegate[] = {48, 50, 280, 0};
const uint16_t* g_door_table[11] = {NULL, g_door_home, g_door_home, g_door_home, g_door_home, g_door_whitegate,
                                    g_door_home, g_door_home, g_door_home, g_door_home, g_door_home};
uint8_t g_never_data[0x100];           // the globals of code never run
uint8_t g_key_mute;                    // the key test's input mute byte, never 1 here
volatile LONG g_left_click;            // set by the harness: the left button is down
volatile LONG g_dialogue_ticks;        // set: each frame runs the dialogue boxes' ticks on their bound instances
uint8_t g_dialogue_wait;               // the event's wait flag the boxes' cancel sets to 2
uint8_t g_texts[2][0x38];              // rem4line's and rem4li2's text objects
uint8_t* g_text_objects[2] = {g_texts[0], g_texts[1]};  // the game's globals naming them
// The game's text entry: its manager and its global (+4 the field its last
// bind gave the line, +8 set by every bind, a bind of NULL too, until an
// unbind, +0xC the line, +0x10 the IME display, whose +0x2218 names the
// field bound); the line's mode, as the game's setmode writes it (2 cleared
// and closed, 3 closed, 4 the chat line, 5 a field's), and its text, what
// the player types. The field objects, 0x6B4 bytes each: +0 the vtable
// whose +0xC deletes, +0x12 the text kept, +0x694 set while bound, +0x6A0
// the manager then. A delete poisons all but the vtable.
uint8_t g_edit[0x14];
uint8_t* g_edit_ptr = g_edit;
uint8_t g_ime[0x2220];
int g_line_mode = 3;
char g_field_typed[64] = "Typed";
uint8_t g_fields[16][0x6B4];
uint8_t g_field_live[16];
int g_field_next;
uint8_t g_input_state[0x50];           // what the open hooks' global names: +0x4C set while a field is up
uint8_t* g_input_state_ptr = g_input_state;
char g_empty_text[4] = "";
uint8_t g_pol[0x100];                  // the POL interface: +0x80 the word its number reads
uint8_t* g_pol_ptr = g_pol;
uint8_t g_sending_owner[0x80];         // the sending indicator's owner, the indicator at +0x20
uint8_t* g_sending_ptr = g_sending_owner;
uint8_t g_msgline_busy;
char g_key_msgline[17] = "menu    msgline ";
uint8_t g_search[0x100];               // the player search: its criteria at +0x5C
uint8_t* g_search_ptr = g_search;
char g_key_scname[17] = "menu    scname  ";
char g_key_scsibori[17] = "menu    scsibori";
char g_key_link11[17] = "menu    link11  ";
char g_key_link6[17] = "menu    link6   ";
uint8_t g_made[12];                    // the new linkshell's request: the slot, the item's word, 1, 0x3C
uint32_t g_item_word = 0x00C0FFEEu;
int32_t g_send_result;                 // what msgline's senders answer: 0 queued
uint32_t g_slot_taken_answer = 0xABCD0000u;    // al: the second linkshell slot is taken
uint32_t g_create_answer = 0xABCD0001u;        // al: the request was queued
uint32_t g_comment_policy;             // where comment's open hook writes its policy bit here
// The renderer chain the quad routine's head and the draw primitive read
// (+0x0C the device, its first word the device's vtable, whose +0x120 is
// DrawPrimitiveUP); the sprites the draws pass, each its tag in its first
// byte: 'f' a window's frame, 't' the target window's, 'c' its content, 'a'
// an arrow, 'q' conq3pts' content. The menu whose per-menu draw is under
// way, NULL between them; the parts its draw puts the cursor's row on.
uint8_t g_device[0x10];
const void* g_device_vtable[0x50];
uint8_t g_render[0x20];
uint8_t* g_render_ptr = g_render;
uint8_t g_sprites[5][0x20];
uint8_t* g_painting;
uint8_t g_parts[16][0x40];
volatile LONG g_close_draws;           // set by the harness: a close ends in the per-menu draw, as the game's does
uint8_t g_conq_record[0x10];
uint8_t* g_conq_data = g_conq_record;  // conq3pts' data: its controller's draw closes it while this is NULL
uint32_t g_open_wait = 8;              // the input wait map0's callback opens it with
volatile LONG g_draw_walk;             // set by the harness: each frame walks the drawn layers
volatile LONG g_active_pick;           // set by the harness: each frame makes the menu the game would active
volatile LONG g_party_swap;            // set by the harness to n: the next frame swaps partywin to ptw<n>
volatile LONG g_action_swap;           // set by the harness to n: the next frame swaps playermo to actionm<n>

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
    int yesno_calls;            // the yes/no callbacks' calls
    int yesno_answer;           // the last one's answer
    void* yesno_context;
    int yesno_inputs;           // calls reaching a yes/no controller's input routine
    int select_calls;           // select-row's body
    void* select_menu;
    int select_row;
    int decider_calls;          // the KO window's decider
    int decider_args[2];
    int message_calls;          // the message getter
    int message_args[2];
    int ask_calls;              // the yes/no class's ask, as the KO window's confirm calls it
    void* ask_ctl;
    const void* ask_callback;
    const char* ask_text;
    int ask_args[5];            // context, default, help, cancellable, mode
    int box_answers;            // the number boxes' callbacks
    int box_value;              // the last one's number
    void* box_context;
    int box_who;                // which asker's: kAskerSites' index
    int focus_calls;            // a window given the focus back
    void* focus_menu;
    int asked_calls;            // the Asked windows' callbacks and routines: which, then what each was given
    int asked_who;              // core.h's kAsked* of the last
    int asked_value;            // its answer, container, choice or restore argument
    int asked_extra;            // bankmenu's canceled flag; maplist's map
    void* asked_context;        // sortyn's and ok's context; maplist's entry
    int gm_result;              // the GM tell poller's: the result word it read, the window then closed
    int ok_seen;                // the error screen's: the notice acknowledged, the window then closed
    int door_gates;             // mogdoor's confirm's gate: its calls, the last one's arguments
    int door_gate_args[2];
    int door_opens;             // the door actor's open: its calls, the last one's actor and argument
    void* door_open_actor;
    int door_open_arg;
    int door_exit_at_open;      // mogdoor's +0x15 as the open found it
    int advances;               // the text advance: its calls, the last one's text object
    void* advance_text;
    int dispatch_codes;         // the codes the text dispatch handed its gate: their count, the last one
    int dispatch_code;
    int page_sounds;            // the gate's page-turn sound: its calls
    int releases;               // the chat log's release: its calls, the last one's log and line
    void* release_log;
    int release_line;
    int clears;                 // the text object's clear: its calls, the last one's, the wait flag then
    void* clear_text;
    int wait_at_clear;
    int key_clicks;             // the key test's left-button queries that reached its original
    int dialogue_clicks;        // the boxes' ticks that saw the button down
    int attaches;               // the text entry's attach: its calls, the field the last one attached
    void* attached;
    int deactivates;
    int field_reads;
    int field_deletes;          // a field deleted: the last one
    void* field_deleted;
    int field_starts;           // a window's own start of its field
    int sanitizes;              // comment's line sanitize and trim
    int trims;
    int comment_sends;          // comment's begin-close: the kind of message and its lines as it found them
    int comment_mode;
    char comment_lines[3][41];
    int pol_numbers;            // the POL interface's number: its calls, the last one's word
    uint32_t pol_word;
    int clocks;
    int sends;                  // msgline's senders: their calls, the last one's mode, arguments and text
    int send_mode;
    uint32_t send_args[9];
    char send_text[301];
    int marks;                  // the sending indicator's mark: its calls, the last one's indicator and flags
    void* mark_obj;
    uint32_t mark_flags;
    int criteria_clears;        // the player search: criteria cleared and named, searches started
    void* criteria;
    int criteria_names;
    char criteria_name[17];
    int searches;
    int search_args[6];
    int encodes;                // link11's request: the name encoded, the request, the item looked up
    int creates;
    uint32_t create_args[8];
    char create_name[24];
    int items;
    char quads[128];            // the tag of each quad the last walked frame drew, in order
    int walks;                  // frames walked
    int queue_allocs;           // the packet queue's allocations: the last one's packet id
    int queue_id;
    int commits;                // packets committed: the last one's first 0x40 bytes
    uint8_t packet[0x40];
    int trade_sends[2];         // the trade's OK's and cancel's sends
    int trade_unoks;
    int grays;                  // the inventor modes' gray test: its calls, the lock byte then, the mode
    int gray_lock;
    void* gray_mode;
    void* walked[64];           // the menus the last walk called the per-menu draw for, in order
    int walked_count;
    void* drawn[64];            // those whose draw ran past the per-menu draw's head
    int drawn_count;
    int map_builds;             // maplist's rebuild: the rows it built
    int map_selects;            // maplist's select: its calls, the last one's row and flag
    int map_select_row;
    int map_select_flag;
    void* painted[64];          // the menu drawing at each call the device drew, the last walked frame's
    int painted_count;
    LONG pixels;                // the device's draws, every frame's
    void* freed[16];            // with close_draws, each instance the update freed and the frame it did
    LONG freed_frame[16];
    int freed_count;
    void* ready[16];            // each menu whose input wait ran out in its draw, and the frame it did
    LONG ready_frame[16];
    int ready_count;
    int map_closes;             // the map's close: its calls, the last one's frame and the frame's +0x104
    void* map_frame;
    int map_mode;
    int map0_open_at_close;     // map0 open and not closing as it ran
    int map_fades;              // the frame's fade-out
    char line_modes[64];        // each mode the line was set to, a digit, in order
    int bad_fields;             // a NULL or deleted field read, bound or deleted: the count, the first
    char bad_field[64];
    int field_news;             // field objects made
    int passinpu_draws;         // passinpu's and msgline's draws past their heads
    int msgline_draws;
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
    void* calls[13];            // query's confirm, then signatures.h's kCalls in order
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
    uint8_t* yesno_site;        // the yes/no class's constructor site, its vtable at +31
    const void** yesno_vtable;
    void* yesno_ask;            // cdecl (name, default_yes, cancellable, mode, chain): the class's ask
    uint8_t* yesno_confirm;     // the class's confirm, never run
    uint8_t* dead_site;         // the KO window's confirm, never run
    uint8_t* dead_tick;         // its tick, never run
    uint8_t* dead_callback;     // the callback that confirm names, the game's bytes
    void* dead_decider;
    void* message;
    void* yesno_ask_this;       // this = the controller, ret 0x1C: the class's ask as the engine calls it
    uint8_t* dead_set;          // what the decider answers
    uint8_t* yesno_input;       // the class's input routine, its cancel's bytes inside
    void* box_confirm[2];       // this = the controller (the number), ret 4: moneyctr's, itemctrl's
    void* box_setup[2];         // this = the controller: moneyctr's (initial, max, cb, ctx), itemctrl's (+ preview)
    uint8_t* askers[16];        // signatures.h's kAskerSites, in order: where each is found
    void* callbacks[16];        // the callback each asks with
    uint8_t* gil_record;
    void* asked_ask;            // cdecl (which, arg): an Asked window asked as the game's askers ask, on the game thread
    uint8_t* asked_sites[11];   // the Asked windows' signature sites, in signatures.h's order
    uint8_t* player_data;       // the Mog Safe 2 flag's data
    uint8_t* gm_queue;
    int32_t* gm_slot;
    volatile LONG* gm_polls;    // set: each frame runs the GM tell poller
    volatile LONG* error_screen; // set: each frame runs the error screen's wait on ok
    const void* const* door_vtable;     // mogdoor's, in read-only data
    uint8_t* door_input;        // its input routine, never run
    uint8_t* door_handler;      // the door's handler from its open on, never run
    uint8_t* door_opened;       // the routine an exit gives the door, never run
    uint8_t* door_open_hook;    // mogdoor's open hook, run by the open
    uint8_t* door_tick;         // its tick, run each frame it is up
    uint8_t** house_ptr;        // the Mog House object's global
    uint8_t* door_actor;
    uint8_t* door_exiting;      // the three bytes an exit sets
    uint32_t* door_gate_answer;
    volatile LONG* door_garden_on;
    volatile LONG* door_floor_on;
    uint32_t* door_condition;
    const void* const* dialogue_vtables;   // rem4line's, then rem4li2's, 0x20 slots each, in read-only data
    uint8_t* dialogue_input;    // their input routine, never run
    uint8_t* dialogue_tick;     // their tick
    uint8_t* key_test;          // cdecl (0x3E, action, query, slot), al the answer
    uint8_t* dialogue_wait;
    uint8_t* texts;             // the two text objects, 0x38 bytes each
    volatile LONG* left_click;
    volatile LONG* dialogue_ticks;
    uint8_t* edit;              // the game's text entry object
    char* field_typed;          // the line's text: what the player types into the field bound
    uint8_t* field_boxes;       // the field objects, 0x6B4 bytes each
    uint8_t* msgline_busy;
    int32_t* send_result;
    uint8_t* pol;
    uint8_t* sending_owner;
    uint8_t* search;
    uint8_t* made;
    uint32_t* item_word;
    uint32_t* slot_taken_answer;
    uint32_t* create_answer;
    uint8_t* input_state;
    uint8_t* text_inputs[6];    // the input routines, in the fields' order
    void* menu_draw;            // thiscall (menu), plain ret: the per-menu draw
    void* sprite_draw;          // thiscall (sprite; eight arguments), ret 0x20
    void* quad_draw;            // thiscall (sprite; five arguments), ret 0x14
    uint8_t* target_draw;       // the target window's controller draw, its two arrow calls inside
    uint8_t* comment_start;     // comment's start (this = the controller; the line), ret 4
    volatile LONG* draw_walk;   // set: each frame walks the drawn layers through the per-menu draw
    volatile LONG* party_swap;  // n: the next frame's partywin owner swaps it to ptw<n>, then 0
    volatile LONG* action_swap; // n: the next frame's playermo owner swaps it to actionm<n>, then 0
    uint8_t* trade_input;       // the trade's input routine, never run
    uint8_t* trade_open;        // its open of its windows, never run
    uint8_t* handover_input;    // the NPC trade's input routine, never run
    uint8_t* inventor_rows;     // the inventory list's build of a row, never run
    uint8_t* trade_stage;       // the trade's stage, the game's bytes
    uint8_t* player;            // the player's data
    uint8_t* intervals;         // the throttle's intervals: +0x43E2C + 4 * id
    uint8_t* clock;             // the game's clock: seconds at +0xC, ms at +0x10
    uint8_t** actors;           // the actor table, 0x10 slots
    uint8_t* item_data;         // the item cache's data for every item but 9999
    uint32_t* gray_answer;      // the inventor modes' gray test's color
    uint32_t* send_answer;      // the trade's OK's and cancel's sends' eax, al the answer
    uint8_t** status_mode;      // the mode the status rule's class 1 lists for
    int32_t* gray_index;        // 0, or the one inventory index gray_answer is for
    void* walk_now;             // cdecl (): one walk of the drawn layers on the calling thread
    volatile LONG* active_pick; // set: each frame makes the menu the game would active
    void* primitive_draw;       // thiscall (renderer; type, count, vertices, stride), ret 0x10
    volatile LONG* close_draws; // set: a close ends in the per-menu draw, and the update frees what it marks
    uint8_t** conq_data;        // conq3pts' data: NULL, its controller's draw closes it
    uint32_t* open_wait;        // the input wait map0's callback opens it with
    uint8_t* map0_input;        // map0's input routine, never run, and the map's close it calls
    uint8_t* map_close;
    uint8_t* equip_open;        // the equipment set's opener (this = equip's controller), plain ret
    uint8_t* equip_shortcut;    // the equipment shortcut, never run
    uint8_t* monstrosity;       // the Monstrosity check (cdecl, al)
    uint8_t* magic_open;        // the magic opener (cdecl; kind, sort, mask)
    uint8_t* mon_player;        // the player data the check reads: its word +0x10 non-zero in Monstrosity
    uint8_t* game_mode;         // the game's mode byte: 0x17 in Monstrosity too
    uint8_t* set_refusal;       // non-zero: the equipment set's opener's first test refuses
    volatile LONG* no_pet;      // set: each frame closes ability while it shows a pet's list
    volatile LONG* equipment_opens;
    volatile LONG* stats_requests;
    volatile LONG* magic_lists;
    int32_t* magic_last;        // the magic opener's last arguments, as its controller got them
    int32_t* ability_last;      // the ability opener's body's last arguments
    volatile LONG* magic_closes;        // magic's begin-close runs
    volatile LONG* magic_close_wild;    // of them, those that found +0xC0 neither NULL nor a sort window's
    FakeSets* sets;             // the other sets' openers
    uint8_t* job_flags;         // bit 0: the jobs known
    uint8_t* main_job;
    uint8_t* sub_job;
    uint8_t* macro_editor;      // the macro editor object, its global naming it
    uint8_t** macro_editor_ptr;
    int32_t* last_overlap;      // the open routine's last argument, as the last open got it
    FakeRoutes* routes;         // the routes' stand-ins
    uint8_t** mog_house_ptr;    // the Mog House's manager's global: mog_house stands in for one
    uint8_t** nomad_ptr;        // the Nomad Moogle service's
    uint8_t* mog_house;
    uint8_t* route_player;      // the player data the Region Info command reads, the area at +0x40
    uint8_t* items_object;      // the item view the Items row sets
    int8_t* nation;             // the player's nation, the missions menu's default log
    uint8_t** policy_ptr;       // the global holding the list the Items row hands inventor
    FakeSettings* settings;
    uint8_t* ime;               // the text entry's IME display: +0x2218 the field bound
    int* line_mode;             // the line's mode
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
void fake_menu_draw();
void fake_sprite_draw();
void fake_quad_draw();
void fake_primitive_draw();
void fake_two_targets();
void fake_projection();
void fake_target_draw();
void fake_comment_open();
void fake_comment_start();
void fake_macro_gate();
void fake_macro_ctor_site();
void fake_ability_open();
void fake_select_row();
void fake_yesno_ctor_site();
void fake_yesno_confirm();
void fake_dead_confirm();
void fake_dead_chosen();
void fake_dead_tick();
void fake_yesno_ask_site();
void fake_yesno_input_site();
void fake_money_confirm();
void fake_item_confirm();
void fake_money_setup();
void fake_item_setup();
void fake_ask_delivery_gil();
void fake_ask_handover_gil();
void fake_ask_trade_gil();
void fake_ask_auction_bid();
void fake_ask_auction_sell();
void fake_ask_bazaar_price();
void fake_ask_delivery_item();
void fake_ask_handover_item();
void fake_ask_trade_item();
void fake_ask_storage();
void fake_ask_bazaar_buy();
void fake_ask_synthesis();
void fake_ask_guild_buy();
void fake_ask_guild_sell();
void fake_ask_shop_buy();
void fake_ask_shop_sell();
void fake_sortyn_setup();
void fake_sortyn_confirm();
void fake_evitem_ask();
void fake_evitem_setup();
void fake_evitem_callback();
void fake_evitem_input();
void fake_confyn_input();
void fake_bank_open();
void fake_bank_begin_close();
void fake_bank_ask();
void fake_bank_flag();
void fake_bank_clear();
void fake_bank_input();
void fake_bank_close();
void fake_maplist_close();
void fake_map_rebuild();
void fake_map0_input();
void fake_map_close();
void fake_equip_open();
void fake_equip_shortcut();
void fake_monstrosity();
void fake_magic_open();
void fake_missions_setter();
void fake_item_summary();
void fake_gm_poll();
void fake_gm_read();
void fake_mp_dead_input();
void fake_ok_input();
void fake_key_test();
void fake_door_input();
void fake_door_handler();
void fake_door_opened();
void fake_door_open_hook();
void fake_door_tick();
void fake_dialogue_input();
void fake_dialogue_tick();
extern const void* const g_door_vtable[0x20];
extern const void* const g_dialogue_vtables[2 * 0x20];
void fake_stringdl_input();
void fake_comment_stop();
void fake_comment_input();
void fake_msgline_stop();
void fake_msgline_input();
void fake_scname_stop();
void fake_scname_input();
void fake_link11_stop();
void fake_link11_input();
void fake_passinpu_stop();
void fake_passinpu_input();
extern const void* const g_text_vtables[5 * 0x20];
extern const void* const g_field_vtable[4];
volatile LONG g_gm_polls;
volatile LONG g_error_screen;
void fake_trade_input();
void fake_trade_stage();
void fake_trade_open();
void fake_handover_input();
void fake_handover_stage();
void fake_inventor_rows();
void fake_item_record();
extern const void* const g_handover_vtable[0x20];
extern const void* const g_status_vtable[0x20];
extern const void* const g_region_vtable[0x20];

// The trade windows' world: the player's data (the offers, the inventory,
// the throttle's last sends at the game's offsets), the throttle's
// intervals, the game's clock, the trade's rows' slots (the game's 16
// bytes), the actor table, the item cache and its data, each window's
// inventor mode on a vtable whose gray test answers gray_answer (for the
// record of inventory index gray_index alone while that is set), and what
// the trade's OK and cancel sends answer (al).
uint8_t g_pd[0x19600];
uint8_t* g_pd_ptr = g_pd;
uint8_t g_intervals[0x44000];
uint8_t* g_intervals_ptr = g_intervals;
uint8_t g_clock_object[0x20];
uint8_t* g_clock_ptr = g_clock_object;
uint8_t g_trade_rows[16] = {0xFF, 1, 2, 3, 4, 5, 6, 7, 8, 0xFF, 0xFF, 0, 0xFF, 0xFF, 0xFF, 0};
uint8_t* g_actors[0x10];
uint8_t g_actor[2][0x240];
uint8_t g_cache_object[4];
uint8_t* g_cache_ptr = g_cache_object;
uint8_t g_item_data[0x200];
const void* g_mode_vtable[0x10];
uint8_t g_modes[2][4];
uint8_t* g_trade_mode_ptr = g_modes[0];
uint8_t* g_handover_mode_ptr = g_modes[1];
uint8_t* g_status_mode_ptr;
uint8_t* g_inventor_ptr;
uint32_t g_offer_zero;
uint32_t g_gray_answer = 0x80808080u;
int32_t g_gray_index;
uint32_t g_send_answer = 0xABCD0001u;
uint8_t g_packet[0x40];
char g_key_trade[17] = "menu    trade   ";
char g_key_gift[17] = "menu    gift    ";
char g_key_money[17] = "menu    money   ";

}  // extern "C"

namespace {

uint8_t g_ctl[256][0x600];
int g_ctl_used;
uint8_t g_menus[512][0xD4];
uint8_t g_res[512][0x60];
int g_menus_used;
uint8_t g_self_refresh[16] = {0x90, 0xC3};
const void* g_vtable[0x20];
const void* g_frame_vtable[0x20];
const void* g_box_vtable[0x20];
const void* g_area_vtable[0x20];
const void* g_bank_vtable[0x20];
const void* g_map_vtable[0x20];
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

int row_named(const char* nm) {
    for (int r = 0; r < kRowCount; ++r) {
        if (strcmp(kRowSpecs[r].name, nm) == 0) {
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

// The update freeing a closed instance: unbound, and one whose close the
// per-menu draw ended filled with 0xDD and logged, so a later touch of it
// shows.
const uint8_t kFreedByte = 0xDD;

void free_instance(uint8_t* menu, bool marked, LONG frame) {
    unbind(menu);
    if (!marked) {
        return;
    }
    memset(menu, kFreedByte, 0xD4);
    const int k = g_log.freed_count;
    if (k < 16) {
        g_log.freed[k] = menu;
        g_log.freed_frame[k] = frame;
        g_log.freed_count = k + 1;
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

// Closed instances the update frees once the frame count reaches `due`, or
// with `marked`, once the menu's +0x70 says its close ended.
struct Dying {
    uint8_t* menu;
    LONG due;
    bool marked;
};
Dying g_dying[16];
int g_dying_count;

typedef void (__fastcall* StagedCloseFn)(void* mcb, void* edx, void* menu, int, int, int, int);

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

}  // namespace

extern "C" {

// The query controller's input(event, row): its cancel writes 0xFF only while
// the cancel-allowed byte is 1, as the game's does.
uint32_t __fastcall fake_query_input(uint8_t* ctl, void*, int event, int row) {
    logged('Q');
    g_log.query_event = event;
    if (event == 6 && g_query_cancel_allowed == 1) {
        const int16_t canceled = 0xFF;
        memcpy(ctl + 0x548, &canceled, 2);
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

void put_word(uint8_t* p, size_t off, int v);

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

void focus(uint8_t* menu);

uint8_t* bound_menu(const uint8_t* ctl);
int32_t g_last_overlap = -1;

// With the open routine's last argument set, another menu of row r's
// overlap group up and not dormant, as the game's test of it finds one.
bool group_showing(int r, const uint8_t* self) {
    uint16_t group;
    memcpy(&group, g_registry + r * kRowStride + kRowOverlap, 2);
    for (int k = 0; group && k < kRowCount; ++k) {
        const uint8_t* m = g_ctl_global[k] ? bound_menu(g_ctl_global[k]) : NULL;
        uint16_t other;
        memcpy(&other, g_registry + k * kRowStride + kRowOverlap, 2);
        uint32_t state = 0;
        if (m) {
            memcpy(&state, m + 0x10, 4);
        }
        if (m && m != self && !m[0x6E] && !m[0x70] && state != 0x0E && (other & group)) {
            return true;
        }
    }
    return false;
}

void* __stdcall h_open(const char* buf, int overlap) {
    g_last_overlap = overlap;
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
    uint32_t was = 0;
    if (menu) {
        memcpy(&was, menu + 0x10, 4);
    }
    const bool revived = !menu || menu[0x6E] || menu[0x70] || was == 0x0E;
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
        menu[0x6A] = 1;
        memcpy(ctl + 8, &menu, 4);
    }
    // A yes/no window: its two rows, the cursor on the first, as its template
    // and its policy's start-at-1 leave it.
    const void* vt;
    memcpy(&vt, ctl, 4);
    if (vt == g_yesno_vtable || two_rows(kRowSpecs[r].name)) {
        put_word(menu, 0x58, 2);
        put_word(menu, 0x4C, 1);
    }
    // An instance the open finds is reused, a closing one too: both marks
    // cleared, its destruction called off, and with close_draws its staged
    // close too.
    if (g_close_draws && (menu[0x6E] || menu[0x70])) {
        memset(menu + 0x10, 0, 4);
        put_word(menu, 0x64, 0);
        put_word(menu, 0x66, 0);
    }
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
    // mogdoor's and the text windows' open hooks (vtable +0x04), as the
    // game's open runs them.
    const void* const* table = static_cast<const void* const*>(vt);
    const bool text = table >= g_text_vtables && table < g_text_vtables + 5 * 0x20;
    if ((vt == g_door_vtable || text) && table[1]) {
        reinterpret_cast<void (__fastcall*)(uint8_t*, void*)>(const_cast<void*>(table[1]))(ctl, NULL);
    }
    // Opened or brought back with the last argument set while another
    // window of its overlap group shows, it stays dormant and marked.
    if (revived && overlap && group_showing(r, menu)) {
        const uint32_t dormant = 0x0E;
        memcpy(menu + 0x10, &dormant, 4);
        menu[0x75] = 1;
        return menu;
    }
    focus(menu);
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
    if (!m) {
        return;
    }
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
        if (g_close_draws) {
            // The staged close as the game's runs it: dormant, its two
            // countdowns for the per-menu draw to run out.
            state = 0x0E;
            memcpy(m + 0x10, &state, 4);
            put_word(m, 0x66, static_cast<int>(g_close_frames));
            put_word(m, 0x64, 1);
        }
    }
    g_dying[g_dying_count].menu = m;
    g_dying[g_dying_count].due = g_frames + static_cast<LONG>(g_close_frames);
    g_dying[g_dying_count].marked = g_close_draws != 0;
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

// The manager's list as the game orders it for the active menu: each open
// focuses its menu, and each frame relinks every menu focused since, in
// focus order, by the draw layer it has then (0x1015D9A0: one of layer 5 or
// more to the tail, any other after the last menu below 5); the pick
// (0x1015E670) walks from the tail to the first menu still bound that is
// neither passive (policy bit 1, +0x98 clear), closing, marked +0x70 nor
// dormant.
struct Linked {
    uint8_t* menu;
    bool high;
    int focus;                  // its focus since the last relink, in order; 0 for none
};
Linked g_linked[512];
int g_linked_count;
int g_focuses;

void focus(uint8_t* menu) {
    int i = 0;
    while (i < g_linked_count && g_linked[i].menu != menu) {
        ++i;
    }
    if (i == g_linked_count) {
        if (g_linked_count == 512) {
            return;
        }
        g_linked[g_linked_count].menu = menu;
        g_linked[g_linked_count].high = false;
        ++g_linked_count;
    }
    g_linked[i].focus = ++g_focuses;
}

bool bound_anywhere(const uint8_t* menu) {
    for (int r = 0; r < kRowCount; ++r) {
        if (g_ctl_global[r] && bound_menu(g_ctl_global[r]) == menu) {
            return true;
        }
    }
    return false;
}

void relink() {
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
        l.high = l.menu[0x60] >= 5;
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
}

uint8_t* pick_active() {
    for (int i = g_linked_count - 1; i >= 0; --i) {
        uint8_t* m = g_linked[i].menu;
        uint32_t policy;
        uint32_t state;
        memcpy(&policy, m + 0x34, 4);
        memcpy(&state, m + 0x10, 4);
        const bool passive = (policy & 1) && m[0x98] == 0;
        if (bound_anywhere(m) && !passive && !m[0x6E] && !m[0x70] && state != 0x0E) {
            return m;
        }
    }
    return NULL;
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

void gm_poll();
void error_screen();

typedef void (__fastcall* TickFn)(uint8_t* ctl, void* edx, int32_t arg);

// mogdoor's tick (vtable +0x28), each frame its window is up.
void door_tick() {
    uint8_t* ctl = ctl_named("mogdoor");
    if (bound_menu(ctl)) {
        reinterpret_cast<TickFn>(const_cast<void*>(g_door_vtable[10]))(ctl, NULL, 0);
    }
}

// The per-menu draw's update, with close_draws as the game's pre-pass ends:
// on a dormant menu, its close's countdowns, +0x66 then +0x64, one a frame,
// and with both run out and the closing mark +0x6E set, +0x70, the close
// ended. A menu whose input wait the head runs out next is logged ready.
void __fastcall h_menu_update(uint8_t* menu, void*) {
    if (!g_close_draws) {
        return;
    }
    if (word_at(menu, 0x56) == 1 && g_log.ready_count < 16) {
        g_log.ready[g_log.ready_count] = menu;
        g_log.ready_frame[g_log.ready_count] = g_frames;
        ++g_log.ready_count;
    }
    uint32_t state;
    memcpy(&state, menu + 0x10, 4);
    if (state != 0x0E) {
        return;
    }
    if (word_at(menu, 0x66) > 0) {
        put_word(menu, 0x66, word_at(menu, 0x66) - 1);
    } else if (word_at(menu, 0x64) > 0) {
        put_word(menu, 0x64, word_at(menu, 0x64) - 1);
    } else if (menu[0x6E]) {
        menu[0x70] = 1;
    }
}

typedef uint32_t (__fastcall* QuadFn)(uint8_t* sprite, void* edx, int32_t a, int32_t blend, int32_t b, int32_t c,
                                      int32_t d);
typedef void (__fastcall* MenuDrawFn)(uint8_t* menu, void* edx);

// A menu's draw past the per-menu draw's head: its parts, the row the
// cursor is on (+0x4C) put on +8 as the game's parts loop does, its frame
// through the quad routine; then, while it draws its controller (+0x6A set,
// not dormant), the target window's controller's draw, conq3pts'
// controller's draw, which closes its window by name while its data is
// gone, as the game's does, and paints its content otherwise, and the
// text-entry windows' controllers' draws (vtable +0x10, where theirs has
// one).
void __stdcall h_menu_draw(uint8_t* menu) {
    if (g_log.drawn_count < 64) {
        g_log.drawn[g_log.drawn_count++] = menu;
    }
    uint8_t* const outer = g_painting;
    g_painting = menu;
    const uint8_t* part = g_parts[word_at(menu, 0x4C) & 15];
    memcpy(menu + 8, &part, 4);
    uint8_t* target = ctl_named("targetwi");
    const bool own = target && bound_menu(target) == menu;
    reinterpret_cast<QuadFn>(&fake_quad_draw)(g_sprites[own ? 1 : 0], NULL, 0, 1, 0, 0, 0);
    uint32_t state;
    memcpy(&state, menu + 0x10, 4);
    if (menu[0x6A] && state != 0x0E) {
        uint8_t* conq = ctl_named("conq3pts");
        if (own) {
            reinterpret_cast<MenuDrawFn>(&fake_target_draw)(target, NULL);
        } else if (conq && bound_menu(conq) == menu && !g_conq_data) {
            reinterpret_cast<CloseByNameFn>(&fake_close)(g_mcb, NULL, "menu    conq3pts");
        } else if (conq && bound_menu(conq) == menu) {
            reinterpret_cast<QuadFn>(&fake_quad_draw)(g_sprites[4], NULL, 0, 1, 0, 0, 0);
        }
        for (int r = 0; r < kRowCount; ++r) {
            uint8_t* ctl = g_ctl_global[r];
            const void* const* vt = NULL;
            if (ctl) {
                memcpy(&vt, ctl, 4);
            }
            if (ctl && bound_menu(ctl) == menu && vt >= g_text_vtables && vt < g_text_vtables + 5 * 0x20) {
                if (vt[4]) {
                    reinterpret_cast<MenuDrawFn>(const_cast<void*>(vt[4]))(ctl, NULL);
                }
                break;
            }
        }
    }
    g_painting = outer;
}

// The device's DrawPrimitiveUP (this = the device; type, count, vertices,
// stride), D3D_OK: the vertices are the sprite the quad routine was given,
// its tag in order, and the menu drawing, this walk's.
uint32_t __stdcall h_device_draw(void*, int, int, const uint8_t* vertices, int) {
    InterlockedIncrement(&g_log.pixels);
    const size_t n = strlen(g_log.quads);
    if (n + 1 < sizeof(g_log.quads)) {
        g_log.quads[n] = static_cast<char>(vertices[0]);
        g_log.quads[n + 1] = '\0';
    }
    if (g_log.painted_count < 64) {
        g_log.painted[g_log.painted_count++] = g_painting;
    }
    return 0;
}

// The draw walk, as the game's: every live menu whose layer is 0..8, in
// layer order, through the per-menu draw; the quads of this frame alone.
void walk() {
    g_log.quads[0] = '\0';
    g_log.painted_count = 0;
    g_log.walked_count = 0;
    g_log.drawn_count = 0;
    uint8_t* drawn[64];
    int count = 0;
    for (int layer = 0; layer < 9; ++layer) {
        for (int r = 0; r < kRowCount; ++r) {
            uint8_t* menu = bound_menu(g_ctl_global[r]);
            bool seen = !menu || menu[0x60] != layer || count >= 64;
            for (int i = 0; i < count && !seen; ++i) {
                seen = drawn[i] == menu;
            }
            if (!seen) {
                drawn[count++] = menu;
                g_log.walked[g_log.walked_count++] = menu;
                reinterpret_cast<MenuDrawFn>(&fake_menu_draw)(menu, NULL);
            }
        }
    }
    ++g_log.walks;
}

typedef void (__fastcall* TemplateSwapFn)(uint8_t* menu, void* edx, const char* key);

// An owner's swap of its window's template, through the swap's entry.
void owner_swap(const char* nm, const char* stem) {
    uint8_t* menu = bound_menu(ctl_named(nm));
    if (!menu) {
        return;
    }
    char key[32];
    memset(key, 0, sizeof(key));
    memcpy(key, "menu    ", 8);
    memset(key + 8, ' ', 8);
    memcpy(key + 8, stem, strlen(stem));
    reinterpret_cast<TemplateSwapFn>(&fake_template_swap)(menu, NULL, key);
}

// partywin's owner on a roster change, playermo's on a change of its rows.
void owner_swaps() {
    char stem[16];
    const LONG party = InterlockedExchange(&g_party_swap, 0);
    if (party > 0) {
        snprintf(stem, sizeof(stem), "ptw%ld", static_cast<long>(party));
        owner_swap("partywin", stem);
    }
    const LONG action = InterlockedExchange(&g_action_swap, 0);
    if (action > 0) {
        snprintf(stem, sizeof(stem), "actionm%ld", static_cast<long>(action));
        owner_swap("playermo", stem);
    }
}

// A pet's list with no pet out: the ability window's own per-frame closes
// it again, as the game's does.
void pet_check() {
    uint8_t* ctl = ctl_named("ability");
    uint8_t* menu = bound_menu(ctl);
    if (!g_no_pet || !menu || menu[0x6E]) {
        return;
    }
    int32_t kind;
    memcpy(&kind, ctl + 0x64, 4);
    if (kind == 2 || kind == 6 || kind == 10 || kind == 18) {
        reinterpret_cast<CloseByNameFn>(&fake_close)(g_mcb, NULL, "menu    ability ");
    }
}

void region_fade_tick();

int h_frame(int, int, int, int) {
    const LONG now = InterlockedIncrement(&g_frames);
    for (int i = 0; i < g_dying_count;) {
        const Dying d = g_dying[i];
        if (d.marked ? d.menu[0x70] != 0 : now - d.due >= 0) {
            g_dying[i] = g_dying[--g_dying_count];
            free_instance(d.menu, d.marked, now);
        } else {
            ++i;
        }
    }
    relink();
    if (g_active_pick) {
        uint8_t* active = pick_active();
        memcpy(g_mcb + 0x54, &active, 4);
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
    if (g_gm_polls) {
        gm_poll();
    }
    if (g_error_screen) {
        error_screen();
    }
    draw_compass();
    run_macro_keys();
    door_tick();
    owner_swaps();
    pet_check();
    region_fade_tick();
    if (g_draw_walk) {
        walk();
    }
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

typedef void* (__fastcall* OpenByNameFn)(void* mcb, void* edx, const char* key, int activate, int overlap);

// A list's detail pane: every list of ability has one but job traits (4)
// and the unused 5 and 7, as the game's table of them gives.
bool list_has_pane(int kind) {
    return kind >= 0 && kind <= 23 && kind != 4 && kind != 5 && kind != 7;
}

// The ability opener's body, standing in for the controller method it hands
// its three arguments to: the kind stored at the ability controller's
// +0x64, and with `pane` subwindo opened through the open routine for a
// list that has one, as the game's does; all recorded.
void __stdcall h_ability_kind(int kind, int flag, int pane) {
    uint8_t* ctl = ctl_named("ability");
    if (ctl) {
        memcpy(ctl + 0x64, &kind, 4);
    }
    g_ability_last_kind = kind;
    g_ability_last[0] = kind;
    g_ability_last[1] = flag;
    g_ability_last[2] = pane;
    if ((pane & 0xFF) && list_has_pane(kind)) {
        reinterpret_cast<OpenByNameFn>(&fake_open)(g_mcb, NULL, "menu    subwindo", 1, 0);
    }
}

// The magic list's controller step (this = magic's controller; kind, sort,
// mask), standing in for the game's rebuild: the kind stored at +0x64, then
// with a sort of 1 mgcsortw opened (blusortw for 6), then subwindo, every
// time, each through the open routine; recorded.
void __fastcall h_magic_setup(uint8_t* ctl, void*, int kind, int sort, int mask) {
    InterlockedIncrement(&g_magic_lists);
    if (ctl) {
        memcpy(ctl + 0x64, &kind, 4);
    }
    g_magic_last[0] = kind;
    g_magic_last[1] = sort;
    g_magic_last[2] = mask;
    const OpenByNameFn open = reinterpret_cast<OpenByNameFn>(&fake_open);
    uint8_t* sort_ctl = NULL;
    if ((sort & 0xFF) == 1) {
        open(g_mcb, NULL, kind == 6 ? "menu    blusortw" : "menu    mgcsortw", 1, 0);
        sort_ctl = ctl_named(kind == 6 ? "blusortw" : "mgcsortw");
    }
    if (ctl) {
        memcpy(ctl + 0xC0, &sort_ctl, 4);
    }
    open(g_mcb, NULL, "menu    subwindo", 1, 0);
}

// magic's begin-close (this = its controller): the game's reads the sort
// window's controller at +0xC0 and, when it is not NULL, that window's menu
// through it; a value the setup did not write would crash the client, and
// is counted here instead.
uint32_t __fastcall h_magic_begin_close(uint8_t* ctl, void*) {
    InterlockedIncrement(&g_magic_closes);
    const uint8_t* sort = NULL;
    memcpy(&sort, ctl + 0xC0, 4);
    if (sort && sort != ctl_named("mgcsortw") && sort != ctl_named("blusortw")) {
        InterlockedIncrement(&g_magic_close_wild);
    }
    return 0;
}

// The equipment set's opener's callees: its first two tests (al 1 refuses,
// the second never does), the game's refusal printed (this = the chat's
// owner; the message), the third test (al 1 lets it on), the nine lists'
// reset (this = a list; four dwords), the list object's step (this = the
// object; one dword), inventor handed the list (this = inventor's
// controller; the list), which opens inventor and iteminfo through the open
// routine, and the 0x061 request (cdecl; two dwords).
uint32_t __cdecl h_set_test1() {
    return g_set_refusal;
}

uint32_t __cdecl h_set_test2() {
    return 0;
}

void __fastcall h_set_message(void*, void*, int) {
}

uint32_t __cdecl h_set_test3(int) {
    return 1;
}

void __fastcall h_list_reset(void*, void*, int, int, int, int) {
}

void __fastcall h_list_step(void*, void*, int) {
    InterlockedIncrement(&g_equipment_opens);
}

void __fastcall h_inventor_list(void*, void*, void*) {
    const OpenByNameFn open = reinterpret_cast<OpenByNameFn>(&fake_open);
    open(g_mcb, NULL, "menu    inventor", 1, 0);
    open(g_mcb, NULL, "menu    iteminfo", 1, 0);
}

void __cdecl h_stats_request(int, int) {
    InterlockedIncrement(&g_stats_requests);
}

void h_never() {
}

// The other sets' openers' stand-in callees: what each opener was given,
// the windows they open by name, and the tests they answer.
FakeSets g_sets;

void set_opened(const char* key) {
    reinterpret_cast<OpenByNameFn>(&fake_open)(g_mcb, NULL, key, 1, 0);
}

void sets_called(int which, const void* self, int32_t a, int32_t b, int32_t c) {
    InterlockedIncrement(&g_sets.calls[which]);
    g_sets.self = self;
    g_sets.args[0] = a;
    g_sets.args[1] = b;
    g_sets.args[2] = c;
}

// The map's position read, then its routine's start (this = mapframe's
// controller; the seven arguments as the game's opener pushes them).
uint32_t __fastcall h_map_convert(void*, void*, int32_t, int32_t, int32_t) {
    return 0;
}

uint32_t __stdcall h_map_routine(const int32_t* args) {
    sets_called(kSetsMap, NULL, args[2], args[3], args[0]);
    return !g_sets.refuse;
}

// bluehelp's opener (this = its controller; x, y, group, flag) and
// bluinven's (this = its controller; list, x, y, group), each ret 0x10.
void __fastcall h_bluehelp_open(void*, void*, int32_t, int32_t, int32_t, int32_t) {
    set_opened("menu    bluehelp");
}

void __fastcall h_bluinven_open(void*, void*, const void*, int32_t, int32_t, int32_t) {
    set_opened("menu    bluinven");
}

uint32_t __cdecl h_linkshell_check(int32_t first, int32_t second) {
    InterlockedIncrement(&g_sets.checks);
    g_sets.check_args[0] = first;
    g_sets.check_args[1] = second;
    return 0x0BAD0000u | g_sets.linkshell_check;
}

void __stdcall h_linkshell_open(void* ctl, int32_t mode, int32_t a, int32_t b) {
    sets_called(kSetsLinkshell, ctl, mode, a, b);
    set_opened("menu    link6   ");
}

void __cdecl h_instincts_prep() {
    sets_called(kSetsInstincts, NULL, 0, 0, 0);
}

// The item data the item use reads its type from, kFakeItemType in: 4096 a
// crystal, 28672 an M Maze Tabula, 28704 an R one, 9999 none, any other
// item of type 1.
uint8_t* __fastcall h_set_item_data(void*, void*, uint32_t id) {
    if (id == 9999) {
        return NULL;
    }
    return g_set_items[id == 4096 ? 0 : id == 28672 ? 1 : id == 28704 ? 2 : 3];
}

uint32_t __cdecl h_use_check(int32_t first, int32_t second) {
    InterlockedIncrement(&g_sets.checks);
    g_sets.check_args[0] = first;
    g_sets.check_args[1] = second;
    return 0x0BAD0000u | g_sets.use_check;
}

uint32_t __stdcall h_synthesis(void* ctl, uint32_t index, uint32_t id, uint32_t mode) {
    sets_called(kSetsSynthesis, ctl, static_cast<int32_t>(index), static_cast<int32_t>(id),
        static_cast<int32_t>(mode));
    if (g_sets.refuse) {
        return 0;
    }
    set_opened("menu    inventor");
    set_opened("menu    iteminfo");
    return 1;
}

void __stdcall h_tabula(void* ctl, uint32_t mode, uint32_t index) {
    sets_called(kSetsTabula, ctl, static_cast<int32_t>(mode), static_cast<int32_t>(index), 0);
}

void __cdecl h_tabula_setup() {
    set_opened("menu    bluehelp");
    set_opened("menu    bluinven");
}

void __fastcall h_macro_select(void*, void*, int32_t) {
}

// The routes' stand-in callees: what each was given, in g_routes, the
// windows they open by name, and the tests they answer.
FakeRoutes g_routes;
uint8_t g_mog_house[4];                // what the managers' globals name while the harness sets them
uint8_t* g_mog_house_ptr;
uint8_t* g_nomad_ptr;
uint8_t g_items_object[0x20];          // the item view the Items row sets
uint8_t* g_items_object_ptr = g_items_object;
uint8_t* g_policy_ptr;                 // the list the Items row hands inventor, set at load
const void* g_policy_table[1];
const void* g_policy_list[1];
uint8_t g_route_player[0x80];
uint8_t* g_route_player_ptr = g_route_player;
uint8_t g_mission_bits[15] = {0, 0, 0, 1, 2, 3, 3, 4, 4, 0xb, 0, 0, 5, 6, 7};
const void* g_mission_cases[6];
int8_t g_nation = 1;
char g_key_jobcselu[17] = "menu    jobcselu";
char g_key_storage[17] = "menu    storage ";
char g_key_storage2[17] = "menu    storage2";
char g_key_mogpost[17] = "menu    mogpost ";
char g_key_friend[17] = "menu    friend  ";
char g_key_titlehan[17] = "menu    titlehan";
char g_key_msglist[17] = "menu    msglist ";
char g_key_blklist[17] = "menu    blklist ";
char g_key_mount[17] = "menu    mount   ";
char g_key_subwindo[17] = "menu    subwindo";
char g_key_jbpcat[17] = "menu    jbpcat  ";
char g_key_fp_cat2[17] = "menu    fp_cat2 ";
char g_key_searchma[17] = "menu    searchma";
char g_key_comgenre[17] = "menu    comgenre";
char g_key_sccompar[17] = "menu    sccompar";
char g_key_sccomite[17] = "menu    sccomite";
char g_key_sccomls[17] = "menu    sccomls ";
char g_key_casttime[17] = "menu    casttime";
char g_key_eventtim[17] = "menu    eventtim";
char g_key_conf11l[17] = "menu    conf11l ";
char g_key_quest00[17] = "menu    quest00 ";
char g_key_quest01[17] = "menu    quest01 ";
char g_key_miss00[17] = "menu    miss00  ";
char g_key_scanlist[17] = "menu    scanlist";
char g_key_coloresu[17] = "menu    coloresu";
char g_key_colorank[17] = "menu    colorank";
char g_key_colopoin[17] = "menu    colopoin";
char g_key_conquer1[17] = "menu    conquer1";
char g_key_conq3pts[17] = "menu    conq3pts";
char g_key_conq6sta[17] = "menu    conq6sta";
char g_key_beseige1[17] = "menu    beseige1";
char g_key_beseige2[17] = "menu    beseige2";
char g_key_imperial[17] = "menu    imperial";
char g_key_campresu[17] = "menu    campresu";
char g_key_campsan[17] = "menu    campsan ";
char g_key_camparea[17] = "menu    camparea";
char g_key_allied[17] = "menu    allied  ";
char g_key_textcol1[17] = "menu    textcol1";
char g_key_textcol2[17] = "menu    textcol2";
char g_key_mcrledit[17] = "menu    mcrledit";
char g_key_macro[17] = "menu    macro   ";

void routes_called(const char* what, const void* self, int count, const int32_t* args) {
    InterlockedIncrement(&g_routes.calls);
    const size_t used = strlen(g_routes.order);
    snprintf(g_routes.order + used, sizeof(g_routes.order) - used, "%s%s", used ? " " : "", what);
    g_routes.self = self;
    memset(g_routes.args, 0, sizeof(g_routes.args));
    for (int i = 0; i < count && i < 7; ++i) {
        g_routes.args[i] = args[i];
    }
}

const char* __cdecl h_language_key(int32_t which) {
    routes_called("language_key", NULL, 1, &which);
    return g_key_jobcselu;
}

void __fastcall h_friend_open(void* ctl, void*, uint32_t value) {
    const int32_t a = static_cast<int32_t>(value);
    routes_called("friend", ctl, 1, &a);
    set_opened(g_key_friend);
    set_opened(g_key_titlehan);
}

uint32_t __fastcall h_msglist_open(void* ctl, void*, int32_t a, int32_t b, int32_t c) {
    const int32_t args[3] = {a, b, c};
    routes_called("msglist", ctl, 3, args);
    if (!g_routes.msglist) {
        return 0x0BAD0000u;
    }
    set_opened(g_key_msglist);
    set_opened(g_key_titlehan);
    return 0x0BAD0001u;
}

void __fastcall h_blklist_open(void* ctl, void*) {
    routes_called("blklist", ctl, 0, NULL);
    set_opened(g_key_blklist);
}

void __fastcall h_mount_build(void* ctl, void*, uint32_t value) {
    const int32_t a = static_cast<int32_t>(value);
    routes_called("mount_build", ctl, 1, &a);
}

uint32_t __cdecl h_job_level(int32_t job) {
    return job >= 1 && job <= 22 ? g_routes.job_level[job] : 0;
}

void __fastcall h_search_reset(void* ctl, void*, uint32_t flag) {
    const int32_t a = static_cast<int32_t>(flag);
    routes_called("search_reset", ctl, 1, &a);
}

// The item panel's set-and-open: the panel its controller is, and for
// itemxinf its OnOpen's itemyinf.
void __fastcall h_item_panel(void* ctl, void*, int32_t mode, int32_t id, int32_t slot, int32_t bag,
                             const void* extra, const void* beside, const void* under) {
    const int32_t args[7] = {mode, id, slot, bag, static_cast<int32_t>(reinterpret_cast<uintptr_t>(extra)),
                             static_cast<int32_t>(reinterpret_cast<uintptr_t>(beside)),
                             static_cast<int32_t>(reinterpret_cast<uintptr_t>(under))};
    routes_called("item_panel", ctl, 7, args);
    const bool compare = ctl == ctl_named("itemxinf");
    set_opened(compare ? "menu    itemxinf" : "menu    iteminfo");
    if (compare) {
        set_opened("menu    itemyinf");
    }
}

uint32_t __fastcall h_quest00_dim(void* ctl, void*) {
    routes_called("quest00_dim", ctl, 0, NULL);
    return 0;
}

uint32_t __fastcall h_quest00_dim2(void* ctl, void*) {
    routes_called("quest00_dim2", ctl, 0, NULL);
    return 0;
}

void __fastcall h_quest01_setup(void* ctl, void*, uint32_t list) {
    const int32_t a = static_cast<int32_t>(list);
    routes_called("quest01", ctl, 1, &a);
}

uint32_t __cdecl h_expansion(int32_t bit) {
    routes_called("expansion", NULL, 1, &bit);
    return 0x0BAD0000u | ((g_routes.expansion >> bit) & 1);
}

void __cdecl h_switch_sound() {
    InterlockedIncrement(&g_routes.switch_sounds);
}

uint32_t __cdecl h_bag_size(int32_t bag) {
    return bag >= 0 && bag < 18 ? g_routes.bag_size[bag] : 0xFF;
}

// cnqframe's fade, which its Escape starts (vtable +0x50): its closing bit
// set, and eight frames on the frame closed by name, as the fade's end closes
// it.
LONG g_region_fade;
void __fastcall h_region_fade(uint8_t* ctl, void*) {
    ctl[0x14] |= 0x40;
    g_region_fade = 8;
}

void region_fade_tick() {
    if (g_region_fade && --g_region_fade == 0) {
        ctl_named("cnqframe")[0x14] &= static_cast<uint8_t>(~0x40);
        reinterpret_cast<CloseByNameFn>(&fake_close)(g_mcb, NULL, "menu    cnqframe");
    }
}

uint32_t __fastcall h_status_clear(void*, void*) {
    return 0;
}

// The Wide Scan question's callback (cdecl; context, answer): on a yes the
// tracking stopped and scanlist's opener run, as the game's does.
void fake_scanlist_open();
void __cdecl h_scan_question(int32_t, int32_t answer) {
    g_routes.scan_answer = answer & 0xFF;
    if (answer & 0xFF) {
        g_routes.tracking = 0;
        typedef void (__fastcall* Opener)(void* ctl, void* edx);
        reinterpret_cast<Opener>(&fake_scanlist_open)(ctl_named("scanlist"), NULL);
    }
}

const int32_t* __cdecl h_tracking() {
    routes_called("tracking", NULL, 0, NULL);
    return g_routes.untracked ? NULL : &g_routes.tracking;
}

void __fastcall h_list_format(void* ctl, void*, uint32_t format) {
    const int32_t a = static_cast<int32_t>(format);
    routes_called("list_format", ctl, 1, &a);
}

// The Region Info opener: the kind at +0x160, then cnqframe and the kind's
// points window.
void __fastcall h_region_open(void* ctl, void*, int32_t kind, int32_t area) {
    routes_called("region", ctl, 2, &kind);
    g_routes.args[1] = area;
    if (kind < 0 || kind > 3) {
        return;
    }
    memcpy(static_cast<uint8_t*>(ctl) + 0x160, &kind, 4);
    static const char* const points[4] = {"menu    conq3pts", "menu    imperial", "menu    allied  ",
                                          "menu    colopoin"};
    set_opened("menu    cnqframe");
    set_opened(points[kind]);
}

void __fastcall h_title(void* ctl, void*, int32_t a, int32_t b) {
    const int32_t args[2] = {a, b};
    routes_called("title", ctl, 2, args);
}

void __fastcall h_items_view(void* object, void*, int32_t a, int32_t b) {
    const int32_t args[2] = {a, b};
    routes_called("items_view", object, 2, args);
}

// inventor's open with a list: the list kept at +0x5C, then inventor with
// iteminfo.
void __fastcall h_inventor_open(void* ctl, void*, uint32_t list) {
    const int32_t a = static_cast<int32_t>(list);
    routes_called("inventor_open", ctl, 1, &a);
    memcpy(static_cast<uint8_t*>(ctl) + 0x5C, &list, 4);
    set_opened("menu    inventor");
    set_opened("menu    iteminfo");
}

uint8_t* rdptr_of(const uint8_t* p, size_t off) {
    uint8_t* v;
    memcpy(&v, p + off, 4);
    return v;
}

// A field the game reads, binds or deletes: false, the read recorded, for
// NULL or one already deleted, where the game's would fault or free twice.
bool field_live(const uint8_t* field, const char* what) {
    for (int k = 0; field && k < 16; ++k) {
        if (field == g_fields[k] && g_field_live[k]) {
            return true;
        }
    }
    if (g_log.bad_fields++ == 0) {
        snprintf(g_log.bad_field, sizeof(g_log.bad_field), "%s %s", what, field ? "a deleted field" : "NULL");
    }
    return false;
}

// The line's setmode: refused in mode 1; 2 clears the line.
void line_mode(int mode) {
    if (g_line_mode == 1) {
        return;
    }
    g_line_mode = mode;
    if (mode == 2) {
        g_field_typed[0] = '\0';
    }
    const size_t n = strlen(g_log.line_modes);
    if (n + 1 < sizeof(g_log.line_modes)) {
        g_log.line_modes[n] = static_cast<char>('0' + mode);
        g_log.line_modes[n + 1] = '\0';
    }
}

void put_ime(const void* field) {
    memcpy(g_ime + 0x2218, &field, 4);
}

// The manager's bind (this = the manager; the field or NULL; ret 4): +4 and
// +8 written first, then a field told it is bound and the line given its
// text, mode 5; for NULL mode 3.
void __fastcall h_edit_attach(uint8_t* edit, void*, uint8_t* field) {
    memcpy(edit + 4, &field, 4);
    edit[8] = 1;
    ++g_log.attaches;
    g_log.attached = field;
    if (field && field_live(field, "bind")) {
        field[0x694] = 1;
        memcpy(field + 0x6A0, &edit, 4);
        line_mode(5);
        snprintf(g_field_typed, sizeof(g_field_typed), "%s", reinterpret_cast<const char*>(field + 0x12));
        put_ime(field);
    } else if (!field) {
        line_mode(3);
        put_ime(NULL);
    }
}

// The manager's unbind (this = the manager): only while +8 is set; the
// field it bound, if any, keeps the line's text and is told it is unbound,
// +4 left naming it; then mode 2.
void __fastcall h_edit_deactivate(uint8_t* edit, void*) {
    ++g_log.deactivates;
    if (!edit[8]) {
        return;
    }
    edit[8] = 0;
    uint8_t* field = NULL;
    memcpy(&field, edit + 4, 4);
    if (field && field_live(field, "unbind")) {
        snprintf(reinterpret_cast<char*>(field + 0x12), 0x180, "%s", g_field_typed);
        field[0x694] = 0;
        memset(field + 0x6A0, 0, 4);
    }
    line_mode(2);
}

// A field's read of its text (this = the field; one dword): the line's text
// while it is bound, else what it kept.
const char* __fastcall h_field_read(uint8_t* field, void*, int) {
    ++g_log.field_reads;
    if (!field_live(field, "read")) {
        return g_empty_text;
    }
    if (field[0x694]) {
        snprintf(reinterpret_cast<char*>(field + 0x12), 0x180, "%s", g_field_typed);
    }
    return reinterpret_cast<const char*>(field + 0x12);
}

// A field's delete (vtable +0xC; one dword): its destructor's unbind while
// it is bound, then the object poisoned.
void __fastcall h_field_delete(uint8_t* field, void*, int) {
    ++g_log.field_deletes;
    g_log.field_deleted = field;
    if (!field_live(field, "delete")) {
        return;
    }
    if (field[0x694]) {
        snprintf(reinterpret_cast<char*>(field + 0x12), 0x180, "%s", g_field_typed);
        uint8_t* edit = NULL;
        memcpy(&edit, field + 0x6A0, 4);
        h_edit_deactivate(edit, NULL);
        memset(field + 0x6A0, 0, 4);
    }
    memset(field, 0xDD, 0x6B4);
    const void* vt = g_field_vtable;
    memcpy(field, &vt, 4);
    g_field_live[(field - g_fields[0]) / 0x6B4] = 0;
}

// The allocation a start makes for its field (cdecl; the size), a field
// object unused since the last cycled round, and the field's constructor
// (this = it; eight dwords, ret 0x20), its vtable alone set.
void* __cdecl h_field_new(uint32_t) {
    for (int i = 0; i < 16; ++i) {
        const int k = (g_field_next + i) % 16;
        if (!g_field_live[k]) {
            g_field_next = k + 1;
            g_field_live[k] = 1;
            memset(g_fields[k], 0, 0x6B4);
            ++g_log.field_news;
            return g_fields[k];
        }
    }
    return NULL;
}

uint8_t* __fastcall h_field_ctor(uint8_t* field, void*, int32_t, int32_t, void*, void*, const char*, int32_t, int32_t,
                                 int32_t) {
    const void* vt = g_field_vtable;
    memcpy(field, &vt, 4);
    return field;
}

uint8_t* new_field() {
    uint8_t* field = static_cast<uint8_t*>(h_field_new(0x6B4));
    return field ? h_field_ctor(field, NULL, 0, 0, NULL, NULL, NULL, 0, 0, 0) : NULL;
}

char* __cdecl h_strncpy(char* to, const char* from, size_t n) {
    return strncpy(to, from, n);
}

// A window's own start of its field, as the open hooks call it: the field
// it has, or a new one, given the line after the manager's unbind.
void start_field(uint8_t* ctl, size_t at) {
    uint8_t* field = NULL;
    memcpy(&field, ctl + at, 4);
    if (!field) {
        field = new_field();
        memcpy(ctl + at, &field, 4);
    }
    ++g_log.field_starts;
    h_edit_deactivate(g_edit, NULL);
    h_edit_attach(g_edit, NULL, field);
}

void __fastcall h_msgline_field(uint8_t* ctl, void*) {
    start_field(ctl, 0x1C);
}

// comment's start past the bytes the engine reads: the line's field.
void __stdcall h_comment_start(uint8_t* ctl, int32_t) {
    start_field(ctl, 0x378);
}

// What a draw reads of its field: the field's draw copy, through its
// manager while it is bound.
void field_drawn(const uint8_t* field, const char* what) {
    if (field_live(field, what) && field[0x694] && !rdptr_of(field, 0x6A0)) {
        field_live(NULL, what);
    }
}

// passinpu's draw past its head: its field read for the paint with no test
// of the field, as the game's does.
void __fastcall h_passinpu_draw(uint8_t* ctl, void*) {
    ++g_log.passinpu_draws;
    field_drawn(rdptr_of(ctl, 0x38), "passinpu's draw read");
}

// msgline's draw past its head: with a field, the field read for the
// paint, then the field's manager's +0xC with no test of the manager.
void __fastcall h_msgline_draw(uint8_t* ctl, void*) {
    ++g_log.msgline_draws;
    const uint8_t* field = rdptr_of(ctl, 0x1C);
    if (field && field_live(field, "msgline's draw read") && !rdptr_of(field, 0x6A0)) {
        field_live(NULL, "msgline's draw read the manager of");
    }
}

// comment's begin-close past its field's end: the kind of message and its
// slot's lines, as it sends them.
void __stdcall h_comment_sent(uint8_t* ctl) {
    ++g_log.comment_sends;
    const int mode = static_cast<int8_t>(ctl[0x16]);
    g_log.comment_mode = mode;
    for (int l = 0; l < 3 && mode >= 0 && mode <= 3; ++l) {
        memcpy(g_log.comment_lines[l], ctl + 0x1A + (mode * 3 + l) * 0x29, 0x29);
        g_log.comment_lines[l][40] = '\0';
    }
}

void __fastcall h_menu_two(void*, void*, int, int) {
}

void __fastcall h_menu_one(void*, void*, int) {
}

int __cdecl h_sanitize(char* out, const char* line) {
    ++g_log.sanitizes;
    const size_t n = strlen(line);
    memmove(out, line, n + 1);
    return static_cast<int>(n);
}

int __cdecl h_trim(char* line) {
    ++g_log.trims;
    return static_cast<int>(strlen(line));
}

// msgline's: the POL interface's number for a word, the clock's text, the
// senders (recording their mode, arguments and text), the callbacks they
// take (never run) and the sending indicator's mark.
uint32_t __fastcall h_pol_number(void*, void*, uint32_t word) {
    ++g_log.pol_numbers;
    g_log.pol_word = word;
    return 0xAB12CD34u;
}

void __cdecl h_clock_text(int what, char* out, int size) {
    ++g_log.clocks;
    snprintf(out, size, "clock %d", what);
}

int32_t sent(int mode, int count, const uintptr_t* args, int text) {
    ++g_log.sends;
    g_log.send_mode = mode;
    memset(g_log.send_args, 0, sizeof(g_log.send_args));
    for (int i = 0; i < count; ++i) {
        g_log.send_args[i] = static_cast<uint32_t>(args[i]);
    }
    snprintf(g_log.send_text, sizeof(g_log.send_text), "%s", reinterpret_cast<const char*>(args[text]));
    return g_send_result;
}

int32_t __cdecl h_send_tell(uintptr_t a, uintptr_t b, uintptr_t c, uintptr_t d, uintptr_t e, uintptr_t f,
                            uintptr_t g) {
    const uintptr_t v[] = {a, b, c, d, e, f, g};
    return sent(0, 7, v, 4);
}

int32_t __cdecl h_send_reply(uintptr_t a, uintptr_t b, uintptr_t c, uintptr_t d, uintptr_t e, uintptr_t f,
                             uintptr_t g) {
    const uintptr_t v[] = {a, b, c, d, e, f, g};
    return sent(1, 7, v, 4);
}

int32_t __cdecl h_send_short(uintptr_t a, uintptr_t b, uintptr_t c, uintptr_t d, uintptr_t e, uintptr_t f,
                             uintptr_t g) {
    const uintptr_t v[] = {a, b, c, d, e, f, g};
    return sent(2, 7, v, 2);
}

int32_t __cdecl h_send_answer(uintptr_t a, uintptr_t b, uintptr_t c, uintptr_t d, uintptr_t e, uintptr_t f,
                              uintptr_t g, uintptr_t h) {
    const uintptr_t v[] = {a, b, c, d, e, f, g, h};
    return sent(5, 8, v, 5);
}

int32_t __cdecl h_send_dated(uintptr_t a, uintptr_t b, uintptr_t c, uintptr_t d, uintptr_t e, uintptr_t f,
                             uintptr_t g, uintptr_t h, uintptr_t i) {
    const uintptr_t v[] = {a, b, c, d, e, f, g, h, i};
    return sent(7, 9, v, 4);
}

void __cdecl h_msgline_sent1(int, int, int) {
}

void __cdecl h_msgline_sent2(int, int, int) {
}

void __fastcall h_mark(void* obj, void*, uint32_t flags) {
    ++g_log.marks;
    g_log.mark_obj = obj;
    g_log.mark_flags = flags;
}

// The player search's criteria cleared and named, and its start.
void __cdecl h_criteria_clear(void* criteria) {
    ++g_log.criteria_clears;
    g_log.criteria = criteria;
}

void __cdecl h_criteria_name(void*, const char* name) {
    ++g_log.criteria_names;
    snprintf(g_log.criteria_name, sizeof(g_log.criteria_name), "%s", name);
}

void __fastcall h_search_start(void*, void*, int a, void* criteria, int b, int c, int d, int f) {
    ++g_log.searches;
    const int args[6] = {a, static_cast<int>(reinterpret_cast<uintptr_t>(criteria)), b, c, d, f};
    memcpy(g_log.search_args, args, sizeof(args));
}

// link11's: whether the second slot is taken (al), link5's selection as the
// game's reads it, the name's encoding, the request (al 1 queued) and the
// item record's word.
uint32_t __cdecl h_slot_taken(int, int, int, int) {
    return g_slot_taken_answer;
}

const uint8_t* __fastcall h_link5_selected(uint8_t* link5, void*) {
    uint8_t* menu;
    memcpy(&menu, link5 + 8, 4);
    if (!menu) {
        return NULL;
    }
    int16_t cursor;
    int16_t scroll;
    memcpy(&cursor, menu + 0x4C, 2);
    memcpy(&scroll, link5 + 0x1E, 2);
    const int i = cursor + scroll - 1;
    int32_t count;
    memcpy(&count, link5 + 0x50, 4);
    uint8_t* rows;
    memcpy(&rows, link5 + 0x54, 4);
    if (i < 0 || i >= count || !rows) {
        return NULL;
    }
    uint8_t* record;
    memcpy(&record, rows + i * 0x54 + 0x48, 4);
    return record;
}

const char* __cdecl h_encode(const char* name) {
    static char encoded[24];
    ++g_log.encodes;
    snprintf(encoded, sizeof(encoded), "<%s>", name);
    return encoded;
}

uint32_t __cdecl h_create(uint32_t slot, uint32_t container, uint32_t index, uint32_t one, const char* encoded,
                          uint32_t r, uint32_t g, uint32_t b) {
    ++g_log.creates;
    const uint32_t args[8] = {slot, container, index, one, static_cast<uint32_t>(reinterpret_cast<uintptr_t>(encoded)),
                              r, g, b};
    memcpy(g_log.create_args, args, sizeof(args));
    snprintf(g_log.create_name, sizeof(g_log.create_name), "%s", encoded);
    return g_create_answer;
}

const uint32_t* __cdecl h_item(uint32_t, uint32_t) {
    ++g_log.items;
    return &g_item_word;
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

// The yes/no class as the game's keeps it: the callback at +0x14, its
// context at +0x18, the question at +0x1C, the key at +0x20, the class's
// cancel at +0x38, the question's at +0x39, the chained flag and its
// default at +0x3A and +0x3B, the mode at +0x40.
typedef void (__cdecl* YesNoCallbackFn)(void* context, int answer);
typedef void (__fastcall* SelectRowFn)(void* menu, void* edx, int row);

int g_chain_default_yes;               // the default of the question the chaining callback asks

void __cdecl fake_yesno_answered(void* context, int answer) {
    logged('N');
    ++g_log.yesno_calls;
    g_log.yesno_answer = answer;
    g_log.yesno_context = context;
}

const char g_text_chained[] = "Really?";

// The class's re-ask from inside a callback: the question replaced, the
// chained flag and its default set; no open, the confirm opens it.
int fake_yesno_reask(uint8_t* ctl, YesNoCallbackFn cb, void* context, const char* text, int default_yes,
                     int cancellable) {
    YesNoCallbackFn pending;
    memcpy(&pending, ctl + 0x14, 4);
    if (!pending) {
        return 0;
    }
    memcpy(ctl + 0x14, &cb, 4);
    memcpy(ctl + 0x18, &context, 4);
    memcpy(ctl + 0x1C, &text, 4);
    ctl[0x39] = cancellable ? ctl[0x38] : 0;
    ctl[0x3A] = 1;
    ctl[0x3B] = default_yes ? 1 : 0;
    memset(ctl + 0x40, 0, 4);
    return 1;
}

// The answer, then the question asked again on the same controller (the
// context).
void __cdecl fake_yesno_chaining(void* context, int answer) {
    fake_yesno_answered(context, answer);
    fake_yesno_reask(static_cast<uint8_t*>(context), &fake_yesno_answered, context, g_text_chained,
        g_chain_default_yes, 1);
}

// The yes/no class's begin-close: the question dropped, its callback never
// called, unless a chained question waits for the confirm to open it again;
// holdtime closed by name while it is open.
uint32_t __fastcall fake_yesno_begin_close(uint8_t* ctl, void*) {
    ctl[0x3C] = 0;
    if (!ctl[0x3A]) {
        memset(ctl + 0x14, 0, 12);
        memset(ctl + 0x40, 0, 4);
    }
    if (bound_menu(ctl_named("holdtime"))) {
        char key[16];
        memcpy(key, "menu    holdtime", 16);
        area_close_by_name(key);
    }
    return 0;
}

// A yes/no controller's input(event, row): the player's keys. No reply
// reaches it.
uint32_t __fastcall fake_yesno_input(uint8_t*, void*, int, int) {
    logged('n');
    ++g_log.yesno_inputs;
    return 1;
}

// select-row's body: +0x4C, already clamped by the game's bytes before it.
void __stdcall h_select_row(uint8_t* menu, int row) {
    logged('W');
    ++g_log.select_calls;
    g_log.select_menu = menu;
    g_log.select_row = static_cast<int16_t>(row);
    put_word(menu, 0x4C, row);
}

// The question each window asks: comyn's in the game's own text, a two-byte
// character, a run in green and an escape the draw skips.
const char* yesno_text(const char* nm) {
    if (strcmp(nm, "comyn") == 0) {
        return "\x81\x99Log out \x1e\x02now\x1e\x01?\x7f\x31";
    }
    if (strcmp(nm, "merityn") == 0) {
        return "Spend the merit points?";
    }
    if (strncmp(nm, "resyn", 5) == 0) {
        return "Return to your home point?";
    }
    return strcmp(nm, "shopbuy") == 0 ? "Buy this?" : "Sell this?";
}

// The class's ask on its controller, as the game's: refused while a
// question is pending; else the question stored and its own key opened
// through the open routine (the hooked one), a refused open clearing the
// callback; then the cursor on no unless the default is yes, and the mode.
int yesno_ask_on(uint8_t* ctl, const void* cb, void* context, const char* text, int default_yes, int cancellable,
                 int mode) {
    YesNoCallbackFn pending = NULL;
    memcpy(&pending, ctl + 0x14, 4);
    if (pending) {
        return 0;
    }
    memcpy(ctl + 0x14, &cb, 4);
    memcpy(ctl + 0x18, &context, 4);
    memcpy(ctl + 0x1C, &text, 4);
    ctl[0x39] = cancellable ? ctl[0x38] : 0;
    ctl[0x3A] = 0;
    OpenByNameFn open = reinterpret_cast<OpenByNameFn>(&fake_open);
    void* menu = open(g_mcb, NULL, reinterpret_cast<const char*>(ctl + 0x20), 1, 0);
    if (!menu) {
        memset(ctl + 0x14, 0, 4);
        return 0;
    }
    if (!default_yes) {
        reinterpret_cast<SelectRowFn>(&fake_select_row)(menu, NULL, 2);
    }
    memcpy(ctl + 0x40, &mode, 4);
    return 1;
}

// The class's ask by window name, on the game thread, as the harness's
// askers ask. `chain` 1 or 2: the callback asks again, its question's
// default no or yes.
int __cdecl fake_yesno_ask(const char* nm, int default_yes, int cancellable, int mode, int chain) {
    uint8_t* ctl = ctl_named(nm);
    if (!ctl) {
        return 0;
    }
    YesNoCallbackFn pending = NULL;
    memcpy(&pending, ctl + 0x14, 4);
    if (!pending) {
        g_chain_default_yes = chain == 2 ? 1 : 0;
    }
    const YesNoCallbackFn cb = chain ? &fake_yesno_chaining : &fake_yesno_answered;
    return yesno_ask_on(ctl, reinterpret_cast<const void*>(cb), ctl, yesno_text(nm), default_yes, cancellable, mode);
}

// The class's ask as the KO window's confirm calls it (this = the
// controller), every argument the dword pushed, recorded; al the answer, the
// rest of eax dirty.
uint32_t __fastcall h_yesno_ask(uint8_t* ctl, void*, const void* callback, int context, const char* text,
                                int default_yes, int help, int cancellable, int mode) {
    logged('a');
    ++g_log.ask_calls;
    g_log.ask_ctl = ctl;
    g_log.ask_callback = callback;
    g_log.ask_text = text;
    const int args[5] = {context, default_yes, help, cancellable, mode};
    memcpy(g_log.ask_args, args, sizeof(args));
    return yesno_ask_on(ctl, callback, reinterpret_cast<void*>(context), text, default_yes, cancellable, mode)
        ? 0x5A5A5A01u : 0x5A5A5A00u;
}

// The KO window's decider (cdecl; first, second): al g_dead_set.
uint32_t __cdecl h_dead_decider(int first, int second) {
    logged('k');
    ++g_log.decider_calls;
    g_log.decider_args[0] = first;
    g_log.decider_args[1] = second;
    return 0x5A5A5A00u | g_dead_set;
}

const char g_text_home[] = "Return to home point?";
const char g_text_leave[] = "Leave this area?";

// The game's message by category and id (cdecl): the KO window's two.
// mogdoor's texts (category 9) and the areas' names (category 0x14), as
// its draw reads them, and the settings' labels (string table 0xB); any
// other message, recorded, as the KO window's confirm reads it.
const char* door_text(int category, int id) {
    struct Text {
        int category;
        int id;
        const char* text;
    };
    static const Text texts[] = {
        {9, 0x45, "Leave the Mog House?"}, {9, 0xB8, "Stay"}, {9, 0x46, "Go out"}, {9, 0xF1, "Go out to town"},
        {9, 0x125, "Second floor"}, {9, 0xB9, "Other areas"}, {0x14, 230, "Southern San d'Oria"},
        {0x14, 231, "Northern San d'Oria"}, {0x14, 232, "Port San d'Oria"}, {0x14, 280, "Mog Garden"},
        {0x14, 48, "Al Zahbi"}, {0x14, 50, "Aht Urhgan Whitegate"},
    };
    for (size_t i = 0; i < sizeof(texts) / sizeof(texts[0]); ++i) {
        if (texts[i].category == category && texts[i].id == id) {
            return texts[i].text;
        }
    }
    return "";
}

const char* settings_text(int id);

const char* __cdecl h_message(int category, int id) {
    if (category == 9 || category == 0x14) {
        return door_text(category, id);
    }
    if (category == kLabelTable) {
        return settings_text(id);
    }
    logged('m');
    ++g_log.message_calls;
    g_log.message_args[0] = category;
    g_log.message_args[1] = id;
    return id == 0x258 ? g_text_leave : g_text_home;
}

// The callbacks the seven asker sites push (cdecl; number, context), each
// recording which asked: kAskerSites' index.
void box_answered(int who, int value, void* context) {
    logged('#');
    ++g_log.box_answers;
    g_log.box_value = value;
    g_log.box_context = context;
    g_log.box_who = who;
}

void __cdecl fake_gil_handover(int value, void* context) { box_answered(1, value, context); }
void __cdecl fake_gil_trade(int value, void* context) { box_answered(2, value, context); }
void __cdecl fake_price_bazaar(int value, void* context) { box_answered(5, value, context); }
void __cdecl fake_buy_guild(int value, void* context) { box_answered(12, value, context); }
void __cdecl fake_sell_guild(int value, void* context) { box_answered(13, value, context); }
void __cdecl fake_buy_shop(int value, void* context) { box_answered(14, value, context); }
void __cdecl fake_sell_shop(int value, void* context) { box_answered(15, value, context); }

// Close by instance (this = the manager; menu, flag), ret 8, null-safe.
void __fastcall h_close_by_instance(uint8_t*, void*, void* menu, int flag) {
    logged('i');
    if (menu) {
        h_close_menu(menu, flag);
    }
}

// The stand-ins the trade windows' routines call: the trade's un-OK, OK
// and cancel sends (cdecl, al), the packet queue's allocation (cdecl; the
// queue, the id, a flag, the size: a zeroed packet) and commit (cdecl; the
// packet, its size, a flag), the item cache's data (this = the cache; the
// id) and an item's description (cdecl), and each inventor mode's gray test
// (this = the mode; the record, the description), ret 8.
uint32_t __cdecl h_trade_unok() {
    logged('-');
    ++g_log.trade_unoks;
    return 0xABCD0001u;
}

uint32_t __cdecl h_trade_ok_send() {
    logged('K');
    ++g_log.trade_sends[0];
    return g_send_answer;
}

uint32_t __cdecl h_trade_cancel_send() {
    logged('X');
    ++g_log.trade_sends[1];
    return g_send_answer;
}

uint8_t* __cdecl h_queue_alloc(void*, int id, int, int) {
    ++g_log.queue_allocs;
    g_log.queue_id = id;
    memset(g_packet, 0, sizeof(g_packet));
    return g_packet;
}

void __cdecl h_commit(const uint8_t* packet, int, int) {
    logged('P');
    ++g_log.commits;
    memcpy(g_log.packet, packet, sizeof(g_log.packet));
}

// The item data the inventory list's rows read, a summary whose type word
// is at +4: 4096 a crystal, 28672 an M Maze Tabula, 28704 an R one, 5020 to
// 5023 the types 20 to 23, 9999 none, any other item of type 1.
uint8_t* __fastcall h_item_data(void*, void*, uint32_t id) {
    if (id == 9999) {
        return NULL;
    }
    const uint16_t type = static_cast<uint16_t>(id == 4096 ? 8 : id == 28672 ? 0x16 : id == 28704 ? 0x17
        : id >= 5020 && id <= 5023 ? id - 5000 : 1);
    memcpy(g_item_data + 4, &type, 2);
    return g_item_data;
}

uint32_t __cdecl h_describe(const uint8_t* data) {
    return data == g_item_data ? 0x5EED0000u | g_item_data[0x1AF] : 0;
}

uint32_t __fastcall h_gray_test(void* mode, void*, const uint8_t* record, uint32_t) {
    ++g_log.grays;
    g_log.gray_lock = g_item_data[0x1AF];
    g_log.gray_mode = mode;
    return !g_gray_index || record[2] == g_gray_index ? g_gray_answer : 0x80808080u;
}

// The focus handed back to a window (this = the manager; menu, flag), ret 8.
void __fastcall h_focus(uint8_t*, void*, void* menu, int) {
    ++g_log.focus_calls;
    g_log.focus_menu = menu;
}

// The Asked windows (core.h's kAskedNames, by kAsked* index here): what
// their callbacks and the routines their replies call were given.
void asked_called(int who, int value, int extra, void* context) {
    logged('@');
    ++g_log.asked_calls;
    g_log.asked_who = who;
    g_log.asked_value = value;
    g_log.asked_extra = extra;
    g_log.asked_context = context;
}

// sortyn's askers' callback (cdecl; context, answer).
void __cdecl fake_sortyn_answered(void* context, int answer) { asked_called(0, answer, 0, context); }

// The key item list's callback's body (cdecl; answer), with the item under
// the list's cursor, as the game's reads it.
void __cdecl h_evitem_answer(int answer) {
    int32_t row = 0;
    memcpy(&row, ctl_named("evitem") + 0x30, 4);
    asked_called(1, answer & 0xFF, row, NULL);
}

// The routine confyn's yes restores the defaults through (cdecl; which).
void __cdecl h_confyn_restore(int which) { asked_called(2, which, 0, NULL); }

// bankmenu's askers' callback (cdecl; container, canceled).
void __cdecl fake_bank_answered(int container, int canceled) { asked_called(3, container, canceled, NULL); }

// The heap check's proof, for an engine built with -DHU_HEAPCHECK: a heap of
// the stand-in's own holding one block and the block after it, and a write
// that runs 32 bytes past the first block's 16, over the heap's own header of
// the next, as a routine overrunning its buffer does; the bytes it wrote
// over, to put back. Armed, the map window's callback makes the write, once.
// A heap of many blocks prices a check.
HANDLE g_trap_heap;
uint8_t* g_trap_block;
uint8_t g_trap_saved[32];
volatile LONG g_trap_armed;
LONG g_trap_written;
HANDLE g_load_heap;

__declspec(dllexport) void fake_heap_overrun() {
    if (!g_trap_heap) {
        g_trap_heap = HeapCreate(0, 0, 0);
        g_trap_block = static_cast<uint8_t*>(HeapAlloc(g_trap_heap, 0, 16));
        HeapAlloc(g_trap_heap, 0, 16);
    }
    if (g_trap_block && !g_trap_written) {
        memcpy(g_trap_saved, g_trap_block + 16, sizeof(g_trap_saved));
        memset(g_trap_block + 16, 0xA5, sizeof(g_trap_saved));
        g_trap_written = 1;
    }
}

__declspec(dllexport) void fake_heap_repair() {
    if (g_trap_written) {
        memcpy(g_trap_block + 16, g_trap_saved, sizeof(g_trap_saved));
        g_trap_written = 0;
    }
}

__declspec(dllexport) void fake_heap_arm() {
    InterlockedExchange(&g_trap_armed, 1);
}

// The heap of `blocks` 32-byte blocks in place of the last; 0 none.
__declspec(dllexport) int fake_heap_load(int blocks) {
    if (g_load_heap) {
        HeapDestroy(g_load_heap);
        g_load_heap = NULL;
    }
    int made = 0;
    if (blocks > 0 && (g_load_heap = HeapCreate(0, 0, 0)) != NULL) {
        while (made < blocks && HeapAlloc(g_load_heap, 0, 32)) {
            ++made;
        }
    }
    return made;
}

// The instances the update freed with close_draws whose bytes changed since.
__declspec(dllexport) int fake_freed_touched() {
    int touched = 0;
    for (int i = 0; i < g_log.freed_count; ++i) {
        const uint8_t* menu = static_cast<const uint8_t*>(g_log.freed[i]);
        bool same = true;
        for (int k = 0; k < 0xD4 && same; ++k) {
            same = menu[k] == kFreedByte;
        }
        touched += same ? 0 : 1;
    }
    return touched;
}

uint8_t* hooked_open_by_key(const char* key16);

// The map window's callback (cdecl; the row's entry {zone, map}). With
// close_draws, map0 opens again on the map, its input wait open_wait frames.
void __cdecl fake_map_answered(const void* entry) {
    int32_t zone = 0;
    int32_t map = 0;
    memcpy(&zone, entry, 4);
    memcpy(&map, static_cast<const uint8_t*>(entry) + 4, 4);
    asked_called(4, zone, map, const_cast<void*>(entry));
    if (InterlockedExchange(&g_trap_armed, 0)) {
        fake_heap_overrun();
    }
    if (g_close_draws) {
        uint8_t* map0 = hooked_open_by_key("menu    map0    ");
        if (map0) {
            put_word(map0, 0x56, static_cast<int>(g_open_wait));
        }
    }
}

// A callback a notice of ok's may carry (cdecl; context).
void __cdecl fake_ok_answered(void* context) { asked_called(7, 0, 0, context); }

// The key item list's setup's callees: the item's name, the question's
// format, the print into evitem01's +0x74, and the list's base setup
// (this = evitem01's controller; callback, context, text, x, y, extra; ret
// 0x18), which keeps the callback at +0x64 and puts the list's cursor on
// its first row.
const char g_key_item_name[] = "Rusty Key";
const char* __cdecl h_evitem_name(int) { return g_key_item_name; }
const char* __cdecl h_evitem_format(int, int) { return "Use the %s?"; }
int __cdecl h_evitem_print(char* out, const char* format, const char* name) { return sprintf(out, format, name); }

void __fastcall h_evitem_base(uint8_t* ctl, void*, const void* callback, int, const char*, int, int, int) {
    memcpy(ctl + 0x64, &callback, 4);
    memset(ctl + 0x30, 0, 4);
}

// maplist's close+reset's callees: the base list's set-rows (this = the
// controller; three arguments, ret 0xC), here emptying the list, and the
// tail that frees the rows, as the game's clears +0x60 and +0x64. Its
// class's close (vtable +0x74): maplist closed by name.
void __fastcall h_map_rows(uint8_t* ctl, void*, int, int, int) {
    memset(ctl + 0x54, 0, 4);
}

void __fastcall h_map_free(uint8_t* ctl, void*) {
    memset(ctl + 0x60, 0, 8);
}

void __fastcall h_map_close_name(uint8_t*, void*) {
    char key[16];
    memcpy(key, "menu    maplist ", 16);
    area_close_by_name(key);
}

// The map's close's call before its closes by name (this = the frame),
// recording the frame, its +0x104 (8 when the close was told to reset) and
// whether map0 was still open; and the frame's fade-out (its class's +0x50),
// which closes the frame by instance at once, for the fade and the frame's
// own close after it.
void __fastcall h_map_marks(uint8_t* frame, void*) {
    ++g_log.map_closes;
    g_log.map_frame = frame;
    int16_t mode;
    memcpy(&mode, frame + 0x104, 2);
    g_log.map_mode = mode;
    const uint8_t* map0 = bound_menu(ctl_named("map0"));
    g_log.map0_open_at_close = map0 && !map0[0x6E] ? 1 : 0;
}

void __fastcall h_map_fade(uint8_t* frame, void*) {
    ++g_log.map_fades;
    h_close_menu(bound_menu(frame), 1);
}

// maplist's rows, as the game's builders make them: at the top a zone's map
// labeled, a region's list of 6 (map -1), another map, the closing row; in
// that region (+0x58 2) a zone's one map and a zone of several maps (-2 -
// 190, map 0); in zone 190's maps (+0x58 192, +0x5C 0) its two maps.
uint8_t g_map_rows[4][0x54];
uint8_t g_map_entries[4][8];
const char* const g_map_texts[4] = {"Current Area", "Ronfaure", "Bastok Markets", "Close"};
const char* const g_map_labels[4] = {"East", NULL, NULL, NULL};
uint8_t g_map_region_rows[2][0x54];
uint8_t g_map_region_entries[2][8];
const char* const g_map_region_texts[2] = {"West Ronfaure", "King Ranperre's Tomb"};
uint8_t g_map_zone_rows[2][0x54];
uint8_t g_map_zone_entries[2][8];
const char* const g_map_zone_text = "King Ranperre's Tomb";
int32_t g_map_zone = 100;               // the zone the player is in: the top's first row

void map_row(uint8_t* row, const char* text, uint8_t* entry, int32_t id, int32_t map) {
    memset(row, 0, 0x54);
    row[0] = 1;
    memcpy(row + 0x28, &text, 4);
    memcpy(entry, &id, 4);
    memcpy(entry + 4, &map, 4);
    memcpy(row + 0x48, &entry, 4);
}

// maplist's rebuild's builders (this = the controller), after the game's own
// set-rows and free-rows: the rows of the level +0x58 and +0x5C name.
void __fastcall h_map_build(uint8_t* ctl, void*) {
    ++g_log.map_builds;
    int16_t region;
    int16_t zone;
    memcpy(&region, ctl + 0x58, 2);
    memcpy(&zone, ctl + 0x5C, 2);
    int32_t count = 2;
    const uint8_t* rows = g_map_zone_rows[0];
    const uint8_t* entries = g_map_zone_entries[0];
    if (zone != -1) {
        for (int i = 0; i < 2; ++i) {
            map_row(g_map_zone_rows[i], g_map_zone_text, g_map_zone_entries[i], region - 2, i + 1);
        }
    } else if (region > 0) {
        map_row(g_map_region_rows[0], g_map_region_texts[0], g_map_region_entries[0], 100, 1);
        map_row(g_map_region_rows[1], g_map_region_texts[1], g_map_region_entries[1], -2 - 190, 0);
        rows = g_map_region_rows[0];
        entries = g_map_region_entries[0];
    } else {
        const int32_t ids[4] = {g_map_zone, -2, g_map_zone + 1, 0x7FFFFFFF};
        const int32_t maps[4] = {0, -1, 2, 0};
        for (int i = 0; i < 4; ++i) {
            uint8_t* row = g_map_rows[i];
            map_row(row, g_map_texts[i], g_map_entries[i], ids[i], maps[i]);
            if (g_map_labels[i]) {
                row[1] = 0x21;
                memcpy(row + 0x2C, &g_map_labels[i], 4);
            } else if (ids[i] < 0) {
                row[1] = 9;
                const int32_t zones = 6;
                memcpy(row + 0x2C, &zones, 4);
            }
        }
        count = 4;
        rows = g_map_rows[0];
        entries = g_map_entries[0];
    }
    const int32_t capacity = 0x202;
    memcpy(ctl + 0x50, &capacity, 4);
    memcpy(ctl + 0x54, &count, 4);
    memcpy(ctl + 0x60, &rows, 4);
    memcpy(ctl + 0x64, &entries, 4);
}

// The base list's select (this = the controller; the row, a flag; ret 8):
// recorded, and the menu's row put on it, the list scrolled to its top.
void __fastcall h_map_select(uint8_t* ctl, void*, int row, int flag) {
    ++g_log.map_selects;
    g_log.map_select_row = row;
    g_log.map_select_flag = flag;
    uint8_t* menu;
    memcpy(&menu, ctl + 8, 4);
    if (menu && row >= 0) {
        put_word(menu, 0x4C, row + 1);
    }
}

typedef uint32_t (__fastcall* SortynSetupFn)(void* ctl, void* edx, const void* cb, void* ctx, const char* text,
                                            const char* title, int default_yes, int y);
typedef void (__fastcall* EvitemSetupFn)(void* ctl, void* edx, int id);
typedef uint32_t (__fastcall* FakeControllerFn)(void* ctl, void* edx);

const char g_sortyn_text[] = "Drop the \x1e\x02Rusty Key\x1e\x01?";
const char g_sortyn_title[] = "Drop";
uint8_t g_sortyn_item[8] = {0x34, 0x12, 5, 0, 1, 0, 0, 0};
const int kFakeSafe2Flag = 0x21;
uint8_t g_gm_nodes[3][0x18];
uint8_t g_gm_items[3][0x108];
uint8_t g_mp_nodes[2][0x18];
uint8_t g_mp_items[2][0x60];
const char g_mp_texts[2][16] = {"Retry", "Return"};

uint8_t* hooked_open_by_key(const char* key16) {
    OpenByNameFn open = reinterpret_cast<OpenByNameFn>(&fake_open);
    return static_cast<uint8_t*>(open(g_mcb, NULL, key16, 1, 0));
}

// The window that asks, opened and made the active menu, as the player's
// keys leave it before its question.
void open_asker(const char* key16) {
    uint8_t* menu = hooked_open_by_key(key16);
    memcpy(g_mcb + 0x54, &menu, 4);
}

// mogdoor's routines as its confirm, tick and draw call them: the gate
// (cdecl; al the answer), what the tick enables (cdecl; al), the draw's
// condition (cdecl; al) and the door actor's open (this = the actor; ret 4),
// each recording what it was given.
uint32_t __cdecl h_door_gate(int first, int second) {
    logged('G');
    ++g_log.door_gates;
    g_log.door_gate_args[0] = first;
    g_log.door_gate_args[1] = second;
    return g_door_gate_answer;
}

uint32_t __cdecl h_door_garden() {
    return g_door_garden_on ? 0xABCD0001u : 0xABCD0000u;
}

uint32_t __cdecl h_door_floor_on() {
    return g_door_floor_on ? 1u : 0u;
}

uint32_t __cdecl h_door_condition() {
    return g_door_condition;
}

void __fastcall h_door_open(uint8_t* actor, void*, int32_t arg) {
    logged('O');
    ++g_log.door_opens;
    g_log.door_open_actor = actor;
    g_log.door_open_arg = arg;
    const uint8_t* ctl = ctl_named("mogdoor");
    g_log.door_exit_at_open = ctl ? ctl[0x15] : -1;
}

// The NPC dialogue boxes as the game's keep them. A box's controller: its
// list of lines (+0x14 head, +0x18 tail, +0x24 count), the lines of the
// page shown last (+0x4E), the chat log's line (+0x50) and controller
// (+0x64, its log object at +0x18), its text object (+0x68). A text object:
// the messages held (+0x10), the message shown (+0x1C), the page's flags
// (+0x20: 1 waits for a key, 2 timed), the timer (+0x28, an s64 in 1/60 s),
// its box (+0x30). A message: its text at +0x40, a page break 7F 31 before
// each page but the first, the next page's start at +0x828. A line: its
// glyph codes at +4, their count at +0x208. Each node +0 next, +4 prev, +0x10
// the line.
struct TalkMessage {
    const char* raw;
    const char* pages[2][2];
    int lines[2];
};
const TalkMessage kTalk[2] = {
    {"Moogle : Welcome home, kupo!\x7F\x31Rest well.", {{"Moogle : Welcome home,", "kupo!"}, {"Rest well.", NULL}},
     {2, 1}},
    {"Moogle : One more thing.", {{"Moogle : One more thing.", NULL}, {NULL, NULL}}, {1, 0}},
};
const char* const kTalkKeys[2] = {"menu    rem4line", "menu    rem4li2 "};
const char* const kTalkNames[2] = {"rem4line", "rem4li2"};
uint8_t g_log_ctls[2][0x20];
uint8_t g_log_objects[2][0x20];
uint8_t g_talk_messages[2][0x844];
uint8_t g_talk_nodes[2][8][0x18];
uint8_t g_talk_lines[2][8][0x20A];
int g_talk_message[2];
int g_talk_page[2];

// Box d shows page `page` of message `message`: its lines added to the box's
// list after the pages shown before it, the cursor past the page.
void talk_show(int d, int message, int page, uint32_t flags, uint32_t held) {
    uint8_t* ctl = ctl_named(kTalkNames[d]);
    uint8_t* text = g_texts[d];
    uint8_t* m = g_talk_messages[d];
    const TalkMessage& t = kTalk[message];
    if (page == 0) {
        memset(m, 0, sizeof(g_talk_messages[d]));
        strcpy(reinterpret_cast<char*>(m + 0x40), t.raw);
    }
    const char* raw = reinterpret_cast<const char*>(m + 0x40);
    const char* next = raw;
    for (int k = 0; k <= page && next; ++k) {
        next = strstr(next + (k ? 1 : 0), "\x7F\x31");
    }
    const uint8_t* at = reinterpret_cast<const uint8_t*>(next ? next : raw + strlen(raw));
    memcpy(m + 0x828, &at, 4);
    int total = 0;
    for (int k = 0; k <= page; ++k) {
        for (int i = 0; i < t.lines[k]; ++i, ++total) {
            uint8_t* line = g_talk_lines[d][total];
            memset(line, 0, 0x20A);
            put_word(line, 0x208, put_glyphs(line + 4, t.pages[k][i]));
        }
    }
    for (int i = 0; i < total; ++i) {
        uint8_t* node = g_talk_nodes[d][i];
        memset(node, 0, 0x18);
        const uint8_t* after = i + 1 < total ? g_talk_nodes[d][i + 1] : NULL;
        const uint8_t* before = i > 0 ? g_talk_nodes[d][i - 1] : NULL;
        const uint8_t* line = g_talk_lines[d][i];
        memcpy(node, &after, 4);
        memcpy(node + 4, &before, 4);
        memcpy(node + 0x10, &line, 4);
    }
    const uint8_t* head = g_talk_nodes[d][0];
    const uint8_t* tail = g_talk_nodes[d][total - 1];
    memcpy(ctl + 0x14, &head, 4);
    memcpy(ctl + 0x18, &tail, 4);
    memcpy(ctl + 0x24, &total, 4);
    put_word(ctl, 0x4E, t.lines[page]);
    memcpy(text + 0x10, &held, 4);
    memcpy(text + 0x1C, &m, 4);
    memcpy(text + 0x20, &flags, 4);
    const int64_t timer = (flags & 2) ? 300 : 0;
    memcpy(text + 0x28, &timer, 8);
    g_talk_message[d] = message;
    g_talk_page[d] = page;
}

// The message ends: nothing shown, nothing held, the box closed by instance.
void talk_end(int d) {
    memset(g_texts[d] + 0x10, 0, 4);
    memset(g_texts[d] + 0x1C, 0, 4);
    memset(g_texts[d] + 0x20, 0, 4);
    close_instance(kTalkNames[d]);
}

int talk_box(const uint8_t* text) {
    return text == g_texts[0] ? 0 : text == g_texts[1] ? 1 : -1;
}

// The text advance (this = a text object), the gate's on code 5: the next
// page of the message, or its end.
void __fastcall h_text_advance(uint8_t* text, void*) {
    logged('A');
    ++g_log.advances;
    g_log.advance_text = text;
    const int d = talk_box(text);
    void* shown = NULL;
    memcpy(&shown, text + 0x1C, 4);
    if (d < 0 || !shown) {
        return;
    }
    const TalkMessage& t = kTalk[g_talk_message[d]];
    const int page = g_talk_page[d] + 1;
    uint32_t flags;
    uint32_t held;
    memcpy(&flags, text + 0x20, 4);
    memcpy(&held, text + 0x10, 4);
    if (page < 2 && t.lines[page]) {
        talk_show(d, g_talk_message[d], page, flags, held);
    } else {
        talk_end(d);
    }
}

// The code the text dispatch handed its gate, at the gate's end (ret 4).
void __stdcall h_dispatch_code(int32_t code) {
    ++g_log.dispatch_codes;
    g_log.dispatch_code = code;
}

// The page-turn sound, after its push of the sound's name (cdecl).
void __cdecl h_page_sound(uint32_t) {
    ++g_log.page_sounds;
}

// The text object's clear (this = it), after its head's own bytes: its
// messages, page flags and timer dropped and the event's wait zeroed, as
// the game's clear leaves them.
void __fastcall h_text_clear(uint8_t* text, void*) {
    logged('X');
    ++g_log.clears;
    g_log.clear_text = text;
    g_log.wait_at_clear = g_dialogue_wait;
    memset(text + 0x10, 0, 4);
    memset(text + 0x1C, 0, 8);
    memset(text + 0x28, 0, 8);
    g_dialogue_wait = 0;
}

// The chat log's release of a box's lines (this = the log object; line).
void __fastcall h_log_release(uint8_t* log, void*, int32_t line) {
    logged('R');
    ++g_log.releases;
    g_log.release_log = log;
    g_log.release_line = line;
}

// The boxes' tick after the key test answered the left button down (this =
// the box's controller): its page on, as the game's input routine moves it.
void __stdcall h_dialogue_click(uint8_t* ctl) {
    ++g_log.dialogue_clicks;
    uint8_t* text;
    memcpy(&text, ctl + 0x68, 4);
    h_text_advance(text, NULL);
}

// The key test's body past its prologue and its mute test (cdecl; 0x3E,
// action, query, slot): the UI update's query runs the frame, the left
// button's answers what the harness holds.
int __cdecl h_key_test(int a, int action, int query, int slot) {
    if (query == 5 && action == 0x8B) {
        ++g_log.key_clicks;
        return g_left_click ? 1 : 0;
    }
    return h_frame(a, action, query, slot);
}

// Each frame after the key test, while the harness sets dialogue_ticks: the
// boxes' ticks (vtable +0x28) on their bound instances.
void __cdecl h_dialogue_ticks() {
    if (!g_dialogue_ticks) {
        return;
    }
    for (int d = 0; d < 2; ++d) {
        uint8_t* ctl = ctl_named(kTalkNames[d]);
        if (bound_menu(ctl)) {
            reinterpret_cast<TickFn>(const_cast<void*>(g_dialogue_vtables[d * 0x20 + 10]))(ctl, NULL, 0);
        }
    }
}

// The Asked windows asked as the game's askers ask, on the game thread:
// `which` core.h's kAsked* index, `arg` what each takes; 1 when the window
// opened (sortyn: what its setup answers, 1 whether or not). 9 and 10 are
// rem4line and rem4li2 saying message `arg` & 0xFF (0xFF: the message's
// end), its page's flags `arg` >> 8 & 0xFF, the messages held `arg` >> 16.
int __cdecl fake_asked_ask(int which, int arg) {
    switch (which) {
    case 0: {
        // sortyn, asked from iuse: the setup's own bytes on its controller,
        // the item record as the context, `arg` the default.
        open_asker("menu    iuse    ");
        SortynSetupFn setup = reinterpret_cast<SortynSetupFn>(&fake_sortyn_setup);
        return static_cast<int>(setup(ctl_named("sortyn"), NULL, reinterpret_cast<const void*>(&fake_sortyn_answered),
            g_sortyn_item, g_sortyn_text, g_sortyn_title, arg, 0) & 0xFF);
    }
    case 1: {
        // The key item list's ask: evitem01 opened, then the setup on the
        // key item under its cursor, `arg`.
        open_asker("menu    evitem  ");
        uint8_t* list = ctl_named("evitem");
        memcpy(list + 0x30, &arg, 4);
        const uint8_t* menu = hooked_open_by_key(g_key_evitem01);
        reinterpret_cast<EvitemSetupFn>(&fake_evitem_setup)(ctl_named("evitem01"), NULL, 1000 + arg);
        return menu != NULL;
    }
    case 2: {
        // confyn: opened, then the defaults it restores, `arg`.
        const uint8_t* menu = hooked_open_by_key("menu    confyn  ");
        memcpy(ctl_named("confyn") + 0x14, &arg, 4);
        return menu != NULL;
    }
    case 3: {
        // bankmenu's setup: refused while a picker waits; its close, the
        // callback kept, then its open; 1 either way. `arg` the Mog Safe 2
        // flag in the player's data.
        g_player_data[kFakeSafe2Flag] = static_cast<uint8_t>(arg);
        uint8_t* ctl = ctl_named("bankmenu");
        void* pending;
        memcpy(&pending, ctl + 0x14, 4);
        if (pending) {
            return 0;
        }
        area_close_by_name("menu    bankmenu");
        const void* cb = reinterpret_cast<const void*>(&fake_bank_answered);
        memcpy(ctl + 0x14, &cb, 4);
        hooked_open_by_key("menu    bankmenu");
        return 1;
    }
    case 4: {
        // maplist, asked from map0: close+reset, the top shown (+0x58 0,
        // +0x5C -1) and its rows built through the rebuild (a zone's map, its
        // label; a region's list, its count of zones; another map; the
        // closing row), its open, then map0's callback. `arg` the first
        // map's zone.
        open_asker("menu    map0    ");
        uint8_t* ctl = ctl_named("maplist");
        reinterpret_cast<FakeControllerFn>(&fake_maplist_close)(ctl, NULL);
        g_map_zone = arg;
        put_word(ctl, 0x58, 0);
        put_word(ctl, 0x5C, -1);
        reinterpret_cast<FakeControllerFn>(&fake_map_rebuild)(ctl, NULL);
        const uint8_t* menu = hooked_open_by_key("menu    maplist ");
        const void* cb = reinterpret_cast<const void*>(&fake_map_answered);
        memcpy(ctl + 0x6C, &cb, 4);
        return menu != NULL;
    }
    case 5: {
        // A GM's question: the queue's slot `arg`, its name, then the window
        // opened and its title and three options set, valued 1 to 3.
        g_gm_slot = arg;
        memset(g_gm_queue + arg * 0x140, 0, 0x140);
        memcpy(g_gm_queue + arg * 0x140, "GM_Test", 7);
        uint8_t* menu = hooked_open_by_key("menu    gmtell  ");
        uint8_t* ctl = ctl_named("gmtell");
        static const char* const texts[3] = {"Yes", "No", "Not sure"};
        ctl[0x36] = static_cast<uint8_t>(put_glyphs(ctl + 0x38, "Are you there?"));
        for (int i = 0; i < 3; ++i) {
            uint8_t* node = g_gm_nodes[i];
            uint8_t* item = g_gm_items[i];
            const uint8_t* next = i + 1 < 3 ? g_gm_nodes[i + 1] : NULL;
            memcpy(node, &next, 4);
            memcpy(node + 0x10, &item, 4);
            node[0x14] = 0;
            item[0x106] = static_cast<uint8_t>(put_glyphs(item + 4, texts[i]));
            const uint16_t value = static_cast<uint16_t>(i + 1);
            memcpy(item + 0x104, &value, 2);
            item[0x107] = 1;
        }
        const uint8_t* head = g_gm_nodes[0];
        memcpy(ctl + 0x14, &head, 4);
        const int32_t three = 3;
        memcpy(ctl + 0x24, &three, 4);
        memset(ctl + 0x30, 0, 4);
        memset(ctl + 0x548, 0, 2);
        return menu != NULL;
    }
    case 6: {
        // Monstrosity's KO window: opened with `arg` seconds on its
        // countdown and its two rows' texts, no choice yet.
        uint8_t* menu = hooked_open_by_key("menu    mp_dead ");
        uint8_t* ctl = ctl_named("mp_dead");
        const int32_t remaining = arg * 60;
        memcpy(ctl + 0x20, &remaining, 4);
        const int32_t deadline = 5000 + arg;
        memcpy(ctl + 0x1C, &deadline, 4);
        memset(ctl + 0x24, 0, 4);
        if (menu) {
            for (int i = 0; i < 2; ++i) {
                uint8_t* node = g_mp_nodes[i];
                uint8_t* item = g_mp_items[i];
                const uint8_t* next = i == 0 ? g_mp_nodes[1] : NULL;
                memcpy(node, &next, 4);
                memcpy(node + 0x10, &item, 4);
                node[0x14] = 0;
                put_word(item, 0, 1);
                put_word(item, 0x3A, i + 1);
                const char* text = g_mp_texts[i];
                memcpy(item + 0x4C, &text, 4);
            }
            const uint8_t* head = g_mp_nodes[0];
            memcpy(menu + 0x14, &head, 4);
        }
        return menu != NULL;
    }
    case 8: {
        // The door's handler: the nation `arg` & 0xFF at +0x14, +0x1B the
        // byte above it, then mogdoor's open, its open hook run by the open.
        uint8_t* ctl = ctl_named("mogdoor");
        ctl[0x14] = static_cast<uint8_t>(arg & 0xFF);
        ctl[0x1B] = static_cast<uint8_t>((arg >> 8) & 0xFF);
        return hooked_open_by_key(g_key_mogdoor) != NULL;
    }
    case 9:
    case 10: {
        const int d = which - 9;
        if ((arg & 0xFF) == 0xFF) {
            talk_end(d);
            return 1;
        }
        const uint8_t* menu = bound_menu(ctl_named(kTalkNames[d]));
        if (!menu) {
            menu = hooked_open_by_key(kTalkKeys[d]);
        }
        if (menu) {
            talk_show(d, arg & 0xFF, 0, (arg >> 8) & 0xFF, (arg >> 16) & 0xFF);
        }
        return menu != NULL;
    }
    case 7: {
        // A notice of ok's: its text, `arg` 1 with the callback a notice
        // may carry, else the error screen's, whose wait then closes it.
        uint8_t* ctl = ctl_named("ok");
        ctl[0x258] = 0;
        memset(ctl + 0x25C, 0, 4);
        const void* cb = arg ? reinterpret_cast<const void*>(&fake_ok_answered) : NULL;
        memcpy(ctl + 0x14, &cb, 4);
        void* context = arg ? g_sortyn_item : NULL;
        memcpy(ctl + 0x18, &context, 4);
        strcpy(reinterpret_cast<char*>(ctl + 0x1C), "Lost connection to the server.");
        return hooked_open_by_key("menu    ok      ") != NULL;
    }
    default:
        return 0;
    }
}

// The GM tell poller, each frame while the harness sets gm_polls: a result
// word set on gmtell's controller is the reply sent, the window closed.
void gm_poll() {
    uint8_t* ctl = ctl_named("gmtell");
    if (!ctl || !bound_menu(ctl) || word_at(ctl, 0x548) == 0) {
        return;
    }
    g_log.gm_result = word_at(ctl, 0x548);
    close_instance("gmtell");
}

// The error screen's wait, each frame while the harness sets error_screen:
// ok acknowledged (+0x25C 1), ok closed by name.
void error_screen() {
    uint8_t* ctl = ctl_named("ok");
    int32_t answered = 0;
    if (ctl) {
        memcpy(&answered, ctl + 0x25C, 4);
    }
    if (answered != 1) {
        return;
    }
    ++g_log.ok_seen;
    memset(ctl + 0x25C, 0, 4);
    area_close_by_name("menu    ok      ");
}

// The config store as the game's keeps it: the object's +4 names its
// entries, +9 its loaded byte; each entry 0x2C bytes, by id: +4 the id, +8
// the value, +0xC the type, +0x18 the min, +0x1C the max (-1: none), +0x20
// the default, +0x24 its subscribers' chain of {vtable, next, routine,
// context}, +0x28 its flags. Every id has one subscriber, which records.
uint32_t g_filter_rows[56] = {
    0x00000000, 0x00020233, 0x00030333, 0x00040433, 0x00050833, 0x00060931, 0x00070A32, 0x00080733,
    0x00090503, 0x00010101, 0x00C30B2E, 0x000B0B04, 0x000C0B05, 0x000D0B06, 0x000E0B07, 0x000F0B08,
    0x00100B09, 0x001E0B17, 0x00140B0D, 0x00150B0E, 0x00160B0F, 0x00170B10, 0x00180B11, 0x001F0B18,
    0x00190B12, 0x001A0B13, 0x001B0B14, 0x001C0B15, 0x001D0B16, 0x00B10B27, 0x00B20B26, 0x00B30B22,
    0x00B40B23, 0x00B50B24, 0x00B60B25, 0x00AB0B21, 0x00AC0B20, 0x00AD0B1C, 0x00AE0B1D, 0x00AF0B1E,
    0x00B00B1F, 0x00200B19, 0x00110B0A, 0x00120B0B, 0x00130B0C, 0x00580B1A, 0x00900B1B, 0x00BF0B28,
    0x00C00B29, 0x00C10B2A, 0x00C20B2D, 0x000A062F, 0x0021FF33, 0x0022FE33, 0x0023FD33, 0x00000000,
};

uint32_t g_effect_rows[20] = {
    0x00990000, 0x009A0001, 0x009B0002, 0x009C0003, 0x009D0004, 0x009E0005, 0x009F0006, 0x00A00007,
    0x00A10008, 0x00A20009, 0x00A3000A, 0x00A4000B, 0x00A5000C, 0x00A6000D, 0x00A7000E, 0x00A8000F,
    0x00A90010, 0x00AA0011, 0x00000024, 0x0000003F,
};

uint32_t g_routing_1[28] = {
    0x00000024, 0x00000000, 0x00000026, 0x00000001, 0x00000027, 0x00000002, 0x00000028, 0x00000003,
    0x00000029, 0x00000004, 0x0000002A, 0x0000000C, 0x0000002B, 0x0000000D, 0x0000002C, 0x00000006,
    0x0000002D, 0x00000007, 0x0000002E, 0x00000008, 0x0000002F, 0x00000009, 0x00000025, 0x0000000A,
    0x000000C4, 0x0000000B, 0xFFFFFFFF, 0x00000000,
};

uint32_t g_routing_2[14] = {
    0x00000030, 0x00000021, 0x00000031, 0x00000022, 0x00000032, 0x00000023, 0x00000033, 0x00000024,
    0x00000034, 0x00000025, 0x00000035, 0x00000026, 0xFFFFFFFF, 0x00000000,
};

uint32_t g_routing_3[14] = {
    0x00000036, 0x00000027, 0x00000037, 0x00000028, 0x00000038, 0x00000029, 0x00000039, 0x0000002A,
    0x0000003A, 0x0000002B, 0x0000003B, 0x0000002C, 0xFFFFFFFF, 0x00000000,
};

uint32_t g_routing_4[8] = {
    0x000000CA, 0x0000002D, 0x000000CB, 0x0000002E, 0x0000003E, 0x0000002F, 0xFFFFFFFF, 0x00000000,
};

uint32_t g_colors_1[56] = {
    0x00000024, 0x0000003F, 0x80808080, 0x0000001B, 0x00000026, 0x00000041, 0x80A040A0, 0x0000001D,
    0x00000027, 0x00000042, 0x8020C0A0, 0x0000001E, 0x00000028, 0x00000043, 0x8050FF60, 0x0000001F,
    0x00000029, 0x00000044, 0x8000CC00, 0x000000B3, 0x0000002A, 0x000000CC, 0x800050FF, 0x000000CB,
    0x0000002B, 0x000000CD, 0x800070FF, 0x000000CC, 0x0000002C, 0x00000045, 0x80FFAF3F, 0x000000B0,
    0x0000002D, 0x00000046, 0x806050A0, 0x00000020, 0x0000002E, 0x00000047, 0x80A0D0D0, 0x00000021,
    0x0000002F, 0x00000048, 0x80808080, 0x00000022, 0x00000025, 0x00000040, 0x80A05040, 0x0000001C,
    0x000000C4, 0x000000C5, 0x80A04030, 0x0000009B, 0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000,
};

uint32_t g_colors_2[28] = {
    0x00000030, 0x00000049, 0x806090C0, 0x00000023, 0x00000031, 0x0000004A, 0x80A04040, 0x00000024,
    0x00000032, 0x0000004B, 0x80808080, 0x00000025, 0x00000033, 0x0000004C, 0x80808080, 0x00000026,
    0x00000034, 0x0000004D, 0x80808050, 0x00000027, 0x00000035, 0x0000004E, 0x80808080, 0x00000028,
    0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000,
};

uint32_t g_colors_3[28] = {
    0x00000036, 0x0000004F, 0x8090C0F0, 0x00000029, 0x00000037, 0x00000050, 0x80C08080, 0x0000002A,
    0x00000038, 0x00000051, 0x80808080, 0x0000002B, 0x00000039, 0x00000052, 0x80808080, 0x0000002C,
    0x0000003A, 0x00000053, 0x80A08040, 0x0000002D, 0x0000003B, 0x00000054, 0x80707070, 0x0000002E,
    0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000,
};

uint32_t g_colors_4[16] = {
    0x0000003C, 0x00000055, 0x80808010, 0x0000002F, 0x0000003D, 0x00000056, 0x80C060D0, 0x00000030,
    0x0000003E, 0x00000057, 0x80C0C050, 0x00000031, 0xFFFFFFFF, 0x00000000, 0x00000000, 0x00000000,
};

uint32_t g_key_rows1[30] = {
    0x00000059, 0x0000005A, 0x0000005C, 0x0000005B, 0x00000065, 0x00000066, 0x00000068, 0x00000067,
    0x0000005D, 0x0000005E, 0x00000060, 0x0000005F, 0x00000089, 0x00000088, 0x00000061, 0x00000062,
    0x00000064, 0x00000063, 0x00000069, 0x0000006A, 0x0000006B, 0x0000006C, 0x0000006D, 0x0000006E,
    0x0000006F, 0x0000008A, 0x0000008E, 0x0000008D, 0x000000C6, 0x000000C7,
};

uint32_t g_key_rows2[30] = {
    0x00000070, 0x00000071, 0x00000072, 0x00000073, 0x00000074, 0x00000085, 0x00000086, 0x00000087,
    0x0000008C, 0x0000008B, 0x0000008E, 0x0000008D, 0x0000008A, 0x00000089, 0x00000088, 0x00000075,
    0x00000076, 0x00000077, 0x00000078, 0x00000079, 0x0000007A, 0x0000007B, 0x0000007C, 0x0000007D,
    0x0000007E, 0x00000082, 0x00000081, 0x0000007F, 0x00000080, 0x00000000,
};

// Each id's type, min, max, default and flags, as the game's store defines them.
const int32_t kFakeStore[0xD0][5] = {
    {4, 0, 1, 0, 0}, {4, 0, 1, 0, 0}, {1, 0, 1, 0, 0}, {1, 0, 1, 0, 0},
    {4, 0, 0, 0, 0}, {1, 0, 1, 1, 1}, {1, 0, 1, 0, 0}, {4, 0, 1, 0, 1},
    {1, 0, 1, 0, 0}, {4, 0, 100, 100, 1}, {4, 0, 100, 100, 1}, {4, 0, 127, 60, 0},
    {4, 0, 512, 0, 0}, {4, 0, 7, 0, 0}, {4, 0, 1, 0, 0}, {4, 4, 20, 10, 0},
    {4, 0, 20, 0, 0}, {4, 3, 30, 5, 0}, {4, 0, 7, 0, 0}, {4, 0, 2, 1, 0},
    {5, 0, 1, 0, 0}, {4, 0, 1, 1, 0}, {4, 0, 100, 100, 0}, {4, 0, 1, 0, 1},
    {1, 0, 1, 0, 0}, {5, 0, 1, 0, 0}, {4, 0, 1, 0, 0}, {4, 0, -1, -2139062144, 0},
    {4, 0, -1, -2136977344, 0}, {4, 0, -1, -2136981344, 0}, {4, 0, -1, -2145337184, 0}, {4, 0, -1, -2142175392, 0},
    {4, 0, -1, -2141171552, 0}, {4, 0, -1, -2136944432, 0}, {4, 0, -1, -2139062144, 0}, {4, 0, -1, -2141155136, 0},
    {4, 0, -1, -2136981440, 0}, {4, 0, -1, -2139062144, 0}, {4, 0, -1, -2139062144, 0}, {4, 0, -1, -2139062192, 0},
    {4, 0, -1, -2139062144, 0}, {4, 0, -1, -2137997072, 0}, {4, 0, -1, -2134867840, 0}, {4, 0, -1, -2139062144, 0},
    {4, 0, -1, -2139062144, 0}, {4, 0, -1, -2136965056, 0}, {4, 0, -1, -2140114832, 0}, {4, 0, -1, -2139062256, 0},
    {4, 0, -1, -2134875952, 0}, {4, 0, -1, -2134851504, 0}, {5, 0, -1, 0, 1}, {5, 0, -1, 6144, 1},
    {4, 0, -1, 0, 0}, {5, 0, 3, 0, 1}, {1, 0, 1, 0, 1}, {4, 0, 100, 50, 0},
    {4, 0, 100, 50, 0}, {4, 0, 100, 50, 0}, {4, 0, 2, 0, 0}, {4, 0, 1, 1, 0},
    {4, 25, 51, 50, 0}, {4, 0, 255, 128, 0}, {4, 0, 25, 25, 0}, {4, 0, 1, 0, 0},
    {4, 0, 3, 3, 0}, {1, -1, 60, -1, 0}, {4, 0, 2, 2, 0}, {4, 0, 5, 0, 1},
    {4, 0, 1, 0, 0}, {4, 0, 1, 1, 0}, {4, 0, 1, 1, 0}, {4, 0, 1, 1, 0},
    {4, 0, 1, 1, 0}, {4, 242, 902, 350, 0}, {4, 0, 255, 44, 0}, {4, 0, 255, 31, 0},
    {4, 0, 255, 16, 0}, {4, 0, 255, 32, 0}, {4, 0, 255, 23, 0}, {4, 0, 255, 37, 0},
    {4, 0, 255, 36, 0}, {4, 0, 255, 38, 0}, {4, 0, 255, 0, 0}, {4, 0, 255, 0, 0},
    {4, 0, 255, 0, 0}, {4, 0, 255, 0, 0}, {4, 0, 255, 39, 0}, {4, 0, 255, 51, 0},
    {4, 0, 255, 0, 0}, {4, 0, 255, 0, 0}, {4, 0, 255, 0, 0}, {4, 0, 255, 0, 0},
    {4, 0, 255, 44, 0}, {4, 0, 255, 49, 0}, {4, 0, 255, 47, 0}, {4, 0, 255, 19, 0},
    {4, 0, 255, 35, 0}, {4, 0, 255, 33, 0}, {4, 0, 255, 243, 0}, {4, 0, 255, 21, 0},
    {4, 0, 255, 26, 0}, {4, 0, 255, 27, 0}, {4, 0, 255, 18, 0}, {4, 0, 255, 16, 0},
    {4, 0, 255, 0, 0}, {4, 0, 255, 0, 0}, {4, 0, 255, 30, 0}, {4, 0, 255, 23, 0},
    {4, 0, 255, 50, 0}, {4, 0, 255, 36, 0}, {4, 0, 255, 17, 0}, {4, 0, 255, 48, 0},
    {4, 0, 255, 44, 0}, {4, 0, 255, 49, 0}, {4, 0, 255, 243, 0}, {4, 0, 255, 33, 0},
    {4, 0, 255, 26, 0}, {4, 0, 255, 27, 0}, {4, 0, 255, 21, 0}, {4, 0, 255, 39, 0},
    {4, 0, 255, 51, 0}, {4, 0, 255, 0, 0}, {4, 0, 255, 0, 0}, {4, 0, 255, 18, 0},
    {4, 0, 255, 47, 0}, {4, 0, 255, 35, 0}, {4, 0, 255, 46, 0}, {4, 0, 255, 31, 0},
    {4, 0, 255, 25, 0}, {4, 0, 255, 20, 0}, {4, 0, 255, 38, 0}, {4, 0, 255, 19, 0},
    {4, 0, 255, 22, 0}, {4, 0, 255, 45, 0}, {4, 0, 255, 16, 0}, {4, 0, 255, 0, 0},
    {4, 0, 255, 0, 0}, {4, 0, 255, 0, 0}, {4, 1000, 4000, 2000, 0}, {4, 0, 1, 1, 0},
    {4, 0, 1, 1, 0}, {4, 0, 1, 0, 0}, {4, 0, 1, 0, 0}, {4, 0, 1, 1, 0},
    {4, 0, 1, 0, 0}, {4, 0, 1, 0, 0}, {4, 0, -1, 0, 0}, {1, -255, 255, 0, 0},
    {1, -255, 255, 0, 0}, {2, 0, 255, 0, 1}, {2, 0, 255, 0, 1}, {4, 0, 1, 0, 0},
    {4, 0, 65535, 0, 0}, {4, 0, 2147483647, 0, 0}, {4, 0, 1, 0, 1}, {4, 0, -1, -2136981456, 0},
    {4, 0, 100, 100, 0}, {4, 0, 1, 0, 0}, {4, 0, 1, 0, 0}, {4, 0, 1, 0, 0},
    {4, 0, 1, 0, 0}, {4, 1801548915, 1801548915, 1801548915, 0}, {4, 0, 3, 0, 0}, {4, 1936222574, 1936222574, 1936222574, 0},
    {4, 0, 1, 0, 0}, {4, 0, 1, 0, 0}, {4, 0, 1, 0, 0}, {4, 0, 1, 0, 0},
    {6, 0, 100, 100, 1}, {4, 0, 1, 0, 0}, {4, 0, 1, 0, 0}, {6, 0, 5, 0, 1},
    {6, 0, 1, 0, 1}, {6, 0, 1, 0, 1}, {4, 0, 1, 0, 0}, {6, 0, 8, 0, 1},
    {4, 0, -1, -2130727105, 0}, {4, 0, 1, 0, 0}, {4, 0, 1, 1, 0}, {4, 0, -1, -2147431424, 0},
    {4, 0, 2, 0, 0}, {4, 0, 2, 0, 0}, {4, 0, 1, 0, 0}, {4, 4, 20, 10, 0},
    {4, 0, 20, 0, 0}, {4, 0, -1, 0, 0}, {4, 0, -1, -1, 0}, {4, 0, -1, -1, 0},
    {4, 0, -1, 0, 0}, {4, 0, 1, 0, 0}, {4, 0, 1, 0, 0}, {4, 0, 1, 0, 0},
    {4, 0, 1, 1, 0}, {4, 0, 1, 0, 0}, {4, 0, 1, 1, 0}, {4, 0, 1, 1, 0},
    {4, 1, 100, 10, 0}, {4, -1, 100, -1, 0}, {4, -1, 30, -1, 0}, {4, 0, 65535, 0, 0},
    {4, 0, 1, 1, 0}, {4, 0, 2, 0, 0}, {4, 0, 1, 0, 0}, {4, 0, -1, -2147462913, 0},
    {4, 0, -1, -2147454721, 0}, {4, 0, 1, 0, 0}, {4, 0, 2, 0, 1}, {4, 0, 1, 0, 0},
};

void fake_cfg_set();
void fake_lock_route();
void fake_idle_route();
void fake_flag_set();

uint8_t g_store[0x10];
uint8_t* g_store_ptr = g_store;
uint8_t g_store_entries[0xD0 * 0x2C];
struct FakeNode {
    const void* vtable;
    FakeNode* next;
    const void* fn;
    void* context;
};
FakeNode g_store_nodes[0xD0];
FakeSettings g_settings;
uint32_t g_lock_time;                   // what the equipment lock's request keeps the clock in

int __cdecl h_store_notify(int id, int value, void*) {
    const LONG n = InterlockedIncrement(&g_settings.notifies) - 1;
    if (n < 64) {
        g_settings.notified[n][0] = id;
        g_settings.notified[n][1] = value;
    }
    return 1;
}

uint32_t __cdecl h_lock_busy() { return g_settings.lock_busy; }
uint32_t __cdecl h_lock_clock() { return g_settings.lock_clock; }
void __fastcall h_lock_message(void*, void*, int) { InterlockedIncrement(&g_settings.lock_messages); }
uint32_t __cdecl h_lock_request(int mode, int last) {
    g_settings.lock_args[0] = mode;
    g_settings.lock_args[1] = last;
    InterlockedIncrement(&g_settings.lock_requests);
    return g_settings.lock_ok;
}
void __cdecl h_idle_ms(int ms) {
    g_settings.idle_ms = ms;
    InterlockedIncrement(&g_settings.idle_ms_calls);
}
void __cdecl h_idle_on(int on) {
    g_settings.idle_on = on;
    InterlockedIncrement(&g_settings.idle_on_calls);
}
uint8_t g_flag_packet[0x40];
uint8_t* __cdecl h_flag_packet(int, int, int) { return g_flag_packet; }
uint32_t __cdecl h_flag_set(int flag, int on) {
    g_settings.flag_args[0] = flag;
    g_settings.flag_args[1] = on;
    InterlockedIncrement(&g_settings.flag_sets);
    return 1;
}

// String table 0xB's texts, as the message getter answers them: the
// game's for the first rows of each page's table, a made-up one for the
// rest.
char g_settings_text[0xD0][24];
const char* settings_text(int id) {
    static const struct {
        int id;
        const char* text;
    } texts[] = {
        {0x00, "Say"}, {0x02, "Tell"}, {0x24, "Immediate vicinity (\"Say\")"}, {0x59, "Move Up"},
        {0x70, "Attack"}, {0x99, "All effects during battle"}, {0x9A, "You \x82\xa0 you"},
        {0xCA, "Battle Messages"},
    };
    for (size_t i = 0; i < sizeof(texts) / sizeof(texts[0]); ++i) {
        if (texts[i].id == id) {
            return texts[i].text;
        }
    }
    return id >= 0 && id < 0xD0 ? g_settings_text[id] : "";
}

void build_store() {
    g_store_ptr = g_store;
    uint8_t* entries = g_store_entries;
    memcpy(g_store + 4, &entries, 4);
    g_store[9] = 1;
    for (int id = 0; id < 0xD0; ++id) {
        uint8_t* e = g_store_entries + id * 0x2C;
        memcpy(e + 0x04, &id, 4);
        memcpy(e + 0x08, &kFakeStore[id][3], 4);
        memcpy(e + 0x0C, &kFakeStore[id][0], 4);
        memcpy(e + 0x18, &kFakeStore[id][1], 4);
        memcpy(e + 0x1C, &kFakeStore[id][2], 4);
        memcpy(e + 0x20, &kFakeStore[id][3], 4);
        FakeNode* node = &g_store_nodes[id];
        node->fn = reinterpret_cast<const void*>(&h_store_notify);
        memcpy(e + 0x24, &node, 4);
        memcpy(e + 0x28, &kFakeStore[id][4], 4);
        snprintf(g_settings_text[id], sizeof(g_settings_text[id]), "text 0x%02X", id);
    }
    g_settings.store = g_store;
    g_settings.entries = g_store_entries;
    g_settings.lock_ok = 1;
    g_settings.lock_stamp = &g_lock_time;
    g_settings.player_flags = g_player_data + 0x30;
    g_settings.store_ptr = &g_store_ptr;
    g_settings.set = reinterpret_cast<const void*>(&fake_cfg_set);
    g_settings.lock = reinterpret_cast<const void*>(&fake_lock_route);
    g_settings.idle = reinterpret_cast<const void*>(&fake_idle_route);
    g_settings.flag = reinterpret_cast<const void*>(&fake_flag_set);
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
    void (*const calls[13])() = {
        &fake_query_confirm, &fake_passinpu_reset, &fake_party_send, &fake_party_clear,
        &fake_link5_clear, &fake_arealist_close, &fake_arealist_latch, &fake_post_request,
        &fake_post_request_close, &fake_dock_reset, &fake_template_swap, &fake_set_frame_rect,
        &fake_select_row,
    };
    for (int i = 0; i < 13; ++i) {
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
    s.yesno_site = reinterpret_cast<uint8_t*>(&fake_yesno_ctor_site);
    s.yesno_vtable = g_yesno_vtable;
    s.yesno_ask = reinterpret_cast<void*>(&fake_yesno_ask);
    s.yesno_confirm = reinterpret_cast<uint8_t*>(&fake_yesno_confirm);
    s.dead_site = reinterpret_cast<uint8_t*>(&fake_dead_confirm);
    s.dead_tick = reinterpret_cast<uint8_t*>(&fake_dead_tick);
    s.dead_callback = reinterpret_cast<uint8_t*>(&fake_dead_chosen);
    s.dead_decider = reinterpret_cast<void*>(&h_dead_decider);
    s.message = reinterpret_cast<void*>(&h_message);
    s.yesno_ask_this = reinterpret_cast<void*>(&fake_yesno_ask_site);
    s.dead_set = &g_dead_set;
    s.yesno_input = reinterpret_cast<uint8_t*>(&fake_yesno_input_site);
    s.box_confirm[0] = reinterpret_cast<void*>(&fake_money_confirm);
    s.box_confirm[1] = reinterpret_cast<void*>(&fake_item_confirm);
    s.box_setup[0] = reinterpret_cast<void*>(&fake_money_setup);
    s.box_setup[1] = reinterpret_cast<void*>(&fake_item_setup);
    void (*const askers[16])() = {
        &fake_ask_delivery_gil, &fake_ask_handover_gil, &fake_ask_trade_gil, &fake_ask_auction_bid,
        &fake_ask_auction_sell, &fake_ask_bazaar_price, &fake_ask_delivery_item, &fake_ask_handover_item,
        &fake_ask_trade_item, &fake_ask_storage, &fake_ask_bazaar_buy, &fake_ask_synthesis,
        &fake_ask_guild_buy, &fake_ask_guild_sell, &fake_ask_shop_buy, &fake_ask_shop_sell,
    };
    void (__cdecl* const pushed[16])(int, void*) = {
        NULL, &fake_gil_handover, &fake_gil_trade, NULL, NULL, &fake_price_bazaar, NULL, NULL, NULL, NULL, NULL,
        NULL, &fake_buy_guild, &fake_sell_guild, &fake_buy_shop, &fake_sell_shop,
    };
    for (int i = 0; i < 16; ++i) {
        s.askers[i] = reinterpret_cast<uint8_t*>(askers[i]);
        s.callbacks[i] = pushed[i] ? reinterpret_cast<void*>(pushed[i]) : reinterpret_cast<void*>(askers[i]);
    }
    s.gil_record = g_pd + 0x9860;
    s.asked_ask = reinterpret_cast<void*>(&fake_asked_ask);
    void (*const asked[11])() = {
        &fake_sortyn_setup, &fake_sortyn_confirm, &fake_evitem_ask, &fake_evitem_input, &fake_confyn_input,
        &fake_bank_open, &fake_bank_input, &fake_maplist_close, &fake_gm_poll, &fake_mp_dead_input, &fake_ok_input,
    };
    for (int i = 0; i < 11; ++i) {
        s.asked_sites[i] = reinterpret_cast<uint8_t*>(asked[i]);
    }
    s.player_data = g_player_data;
    s.gm_queue = g_gm_queue;
    s.gm_slot = &g_gm_slot;
    s.gm_polls = &g_gm_polls;
    s.error_screen = &g_error_screen;
    s.door_vtable = g_door_vtable;
    s.door_input = reinterpret_cast<uint8_t*>(&fake_door_input);
    s.door_handler = reinterpret_cast<uint8_t*>(&fake_door_handler);
    s.door_opened = reinterpret_cast<uint8_t*>(&fake_door_opened);
    s.door_open_hook = reinterpret_cast<uint8_t*>(&fake_door_open_hook);
    s.door_tick = reinterpret_cast<uint8_t*>(&fake_door_tick);
    s.house_ptr = &g_house_ptr;
    s.door_actor = g_door_actor;
    s.door_exiting = g_door_exiting;
    s.door_gate_answer = &g_door_gate_answer;
    s.door_garden_on = &g_door_garden_on;
    s.door_floor_on = &g_door_floor_on;
    s.door_condition = &g_door_condition;
    s.dialogue_vtables = g_dialogue_vtables;
    s.dialogue_input = reinterpret_cast<uint8_t*>(&fake_dialogue_input);
    s.dialogue_tick = reinterpret_cast<uint8_t*>(&fake_dialogue_tick);
    s.key_test = reinterpret_cast<uint8_t*>(&fake_key_test);
    s.dialogue_wait = &g_dialogue_wait;
    s.texts = g_texts[0];
    s.left_click = &g_left_click;
    s.dialogue_ticks = &g_dialogue_ticks;
    s.edit = g_edit;
    s.field_typed = g_field_typed;
    s.field_boxes = g_fields[0];
    s.msgline_busy = &g_msgline_busy;
    s.send_result = &g_send_result;
    s.pol = g_pol;
    s.sending_owner = g_sending_owner;
    s.search = g_search;
    s.made = g_made;
    s.item_word = &g_item_word;
    s.slot_taken_answer = &g_slot_taken_answer;
    s.create_answer = &g_create_answer;
    s.input_state = g_input_state;
    void (*const text_inputs[6])() = {
        &fake_stringdl_input, &fake_comment_input, &fake_msgline_input, &fake_scname_input, &fake_link11_input,
        &fake_passinpu_input,
    };
    for (int i = 0; i < 6; ++i) {
        s.text_inputs[i] = reinterpret_cast<uint8_t*>(text_inputs[i]);
    }
    s.menu_draw = reinterpret_cast<void*>(&fake_menu_draw);
    s.sprite_draw = reinterpret_cast<void*>(&fake_sprite_draw);
    s.quad_draw = reinterpret_cast<void*>(&fake_quad_draw);
    s.target_draw = reinterpret_cast<uint8_t*>(&fake_target_draw);
    s.comment_start = reinterpret_cast<uint8_t*>(&fake_comment_start);
    s.draw_walk = &g_draw_walk;
    s.party_swap = &g_party_swap;
    s.action_swap = &g_action_swap;
    s.trade_input = reinterpret_cast<uint8_t*>(&fake_trade_input);
    s.trade_open = reinterpret_cast<uint8_t*>(&fake_trade_open);
    s.handover_input = reinterpret_cast<uint8_t*>(&fake_handover_input);
    s.inventor_rows = reinterpret_cast<uint8_t*>(&fake_inventor_rows);
    s.trade_stage = reinterpret_cast<uint8_t*>(&fake_trade_stage);
    s.player = g_pd;
    s.intervals = g_intervals;
    s.clock = g_clock_object;
    s.actors = g_actors;
    s.item_data = g_item_data;
    s.gray_answer = &g_gray_answer;
    s.send_answer = &g_send_answer;
    s.status_mode = &g_status_mode_ptr;
    s.gray_index = &g_gray_index;
    s.walk_now = reinterpret_cast<void*>(&walk);
    s.active_pick = &g_active_pick;
    s.primitive_draw = reinterpret_cast<void*>(&fake_primitive_draw);
    s.close_draws = &g_close_draws;
    s.conq_data = &g_conq_data;
    s.open_wait = &g_open_wait;
    s.map0_input = reinterpret_cast<uint8_t*>(&fake_map0_input);
    s.map_close = reinterpret_cast<uint8_t*>(&fake_map_close);
    s.equip_open = reinterpret_cast<uint8_t*>(&fake_equip_open);
    s.equip_shortcut = reinterpret_cast<uint8_t*>(&fake_equip_shortcut);
    s.monstrosity = reinterpret_cast<uint8_t*>(&fake_monstrosity);
    s.magic_open = reinterpret_cast<uint8_t*>(&fake_magic_open);
    s.mon_player = g_mon_player;
    s.game_mode = &g_game_mode;
    s.set_refusal = &g_set_refusal;
    s.no_pet = &g_no_pet;
    s.equipment_opens = &g_equipment_opens;
    s.stats_requests = &g_stats_requests;
    s.magic_lists = &g_magic_lists;
    s.magic_last = g_magic_last;
    s.ability_last = g_ability_last;
    s.magic_closes = &g_magic_closes;
    s.magic_close_wild = &g_magic_close_wild;
    s.sets = &g_sets;
    s.job_flags = &g_job_flags;
    s.main_job = &g_main_job;
    s.sub_job = &g_sub_job;
    s.macro_editor = g_macro_editor;
    s.macro_editor_ptr = &g_macro_editor_ptr;
    s.last_overlap = &g_last_overlap;
    s.routes = &g_routes;
    s.mog_house_ptr = &g_mog_house_ptr;
    s.nomad_ptr = &g_nomad_ptr;
    s.mog_house = g_mog_house;
    s.route_player = g_route_player;
    s.items_object = g_items_object;
    s.nation = &g_nation;
    s.policy_ptr = &g_policy_ptr;
    s.settings = &g_settings;
    s.ime = g_ime;
    s.line_mode = &g_line_mode;
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
    "  .byte 0xff,0x74,0x24,0x38,0x8d,0x44,0x24,0x10,0x50\n"
    "  call _h_open@8\n"
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

    // The per-frame UI update(this = manager), plain ret: the key test asked
    // (0x3E, 0x8B, 4, -1) runs the frame; the dialogue boxes' ticks after it.
    ".p2align 4\n"
    ".globl _fake_ui_update\n"
    "_fake_ui_update:\n"
    "  .byte 0x83,0xec,0x14,0x53,0x55,0x56,0x57,0x6a,0xff,0x6a,0x04,0x68,0x8b,0x00,0x00,0x00,0x6a,0x3e,0x8b,0xe9\n"
    "  call _fake_key_test\n"
    "  .byte 0x83,0xc4,0x10,0x3c,0x01,0x75,0x00\n"
    "  call _h_dialogue_ticks\n"
    "  .byte 0x5f,0x5e,0x5d,0x5b,0x83,0xc4,0x14,0xc3\n"

    // The key test (cdecl; 0x3E, action, query, slot), al the answer: the
    // game's prologue and mute test, then its body.
    ".p2align 4\n"
    ".globl _fake_key_test\n"
    "_fake_key_test:\n"
    "  .byte 0x51,0xa0\n"
    "  .long _g_key_mute\n"
    "  .byte 0x56,0x8b,0x74,0x24,0x14,0x3c,0x01,0x57,0x75,0x09,0x83,0xfe,0x04,0x0f,0x85\n"
    "  .long 1f - . - 4\n"
    "  .byte 0x5f,0x5e,0x59\n"
    "  jmp _h_key_test\n"
    "1:\n"
    "  .byte 0x5f,0x5e,0x59,0x32,0xc0,0xc3\n"

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
    "  .byte 0x8a,0x46,0x6e,0x84,0xc0,0x0f,0x85,0x00,0x00,0x00,0x00,0x8b,0x46,0x04,0x85,0xc0,0x0f,0x84,0x00,0x00\n"
    "  .byte 0x00,0x00,0x8b,0x46,0x10,0xbd,0x0e,0x00,0x00,0x00,0x3b,0xc5,0x0f,0x85,0x00,0x00,0x00,0x00\n"
    "  .byte 0x5f,0x5e,0x5d,0x5b,0xc2,0x04,0x00\n"

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

    // select-row(this = menu; row), ret 4: the game's bytes clamp the row to
    // 1..+0x58 and skip a row the cursor is on already; the body writes it.
    ".p2align 4\n"
    ".globl _fake_select_row\n"
    "_fake_select_row:\n"
    "  .byte 0x56,0x57,0x8b,0x7c,0x24,0x0c,0x8b,0xf1,0x66,0x85,0xff,0x7f,0x05,0xbf,0x01,0x00,0x00,0x00\n"
    "  .byte 0x66,0x8b,0x46,0x58,0x66,0x3b,0xf8,0x7e,0x02,0x8b,0xf8,0x66,0x8b,0x46,0x4c,0x66,0x3b,0xc7,0x74,0x07\n"
    "  .byte 0x57,0x56\n"
    "  call _h_select_row@8\n"
    "  .byte 0x5f,0x5e,0xc2,0x04,0x00\n"

    // The yes/no class's constructor (this = the controller; key,
    // cancellable): never run, its vtable the stand-in's.
    ".p2align 4\n"
    ".globl _fake_yesno_ctor_site\n"
    "_fake_yesno_ctor_site:\n"
    "  .byte 0x53,0x56,0x8b,0xf1,0x8d,0x4e,0x04\n"
    "  call _h_never\n"
    "  .byte 0x33,0xdb,0x8b,0xce,0x66,0xc7,0x46,0x0e,0xff,0xff,0x89,0x5e,0x08,0xc6,0x46,0x0c,0x01,0xc7,0x06\n"
    "  .long _g_yesno_vtable\n"
    "  .byte 0x89,0x5e,0x34\n"
    "  call _h_never\n"
    "  .byte 0x8b,0x4c,0x24,0x0c,0x8d,0x46,0x20,0x5e,0x5b,0xc2,0x08,0x00\n"

    // The yes/no class's confirm (this = the controller), never run: the
    // game's bytes, its callees, the manager and holdtime's key the
    // stand-in's.
    ".p2align 4\n"
    ".globl _fake_yesno_confirm\n"
    "_fake_yesno_confirm:\n"
    "  .byte 0x56,0x8b,0xf1,0x8b,0x46,0x40,0x85,0xc0,0x74,0x05\n"
    "  call _h_never\n"
    "  .byte 0x8b,0x46,0x14,0x85,0xc0,0x75,0x12,0x8b,0x46,0x08,0x6a,0x01,0x50,0xb9\n"
    "  .long _g_mcb\n"
    "  call _h_never\n"
    "  .byte 0x5e,0xc3\n"
    "  .byte 0x8b,0x4e,0x08,0x66,0x83,0x79,0x4c,0x01,0x75,0x08,0x8b,0x56,0x18,0x6a,0x01,0x52,0xeb,0x06\n"
    "  .byte 0x8b,0x4e,0x18,0x6a,0x00,0x51,0xff,0xd0,0x8a,0x46,0x3a,0x83,0xc4,0x08,0x84,0xc0,0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0x75,0x23,0x68\n"
    "  .long _g_net\n"
    "  .byte 0xc7,0x46,0x14,0x00,0x00,0x00,0x00\n"
    "  call _h_never\n"
    "  .byte 0x8b,0x56,0x08,0x6a,0x01,0x52,0xb9\n"
    "  .long _g_mcb\n"
    "  call _h_never\n"
    "  .byte 0x5e,0xc3\n"
    "  .byte 0x8b,0x46,0x08,0x6a,0x01,0x50\n"
    "  call _h_never\n"
    "  .byte 0x6a,0x00,0x8d,0x4e,0x20,0x6a,0x01,0x51,0xb9\n"
    "  .long _g_mcb\n"
    "  call _h_never\n"
    "  .byte 0x8a,0x46,0x3b,0xc6,0x46,0x3a,0x00,0x84,0xc0,0x75,0x0a,0x8b,0x4e,0x08,0x6a,0x02\n"
    "  call _h_never\n"
    "  .byte 0x5e,0xc3\n"

    // The KO window's input handler from its confirm on, never run: the
    // game's bytes around the stand-in's decider, message getter and ask,
    // comyn's controller global (its registry row, 237, in menu_table.inc)
    // and the callback below.
    ".p2align 4\n"
    ".globl _fake_dead_confirm\n"
    "_fake_dead_confirm:\n"
    "  .byte 0x6a,0x00,0x6a,0x01\n"
    "  call _h_dead_decider\n"
    "  .byte 0x83,0xc4,0x08,0x84,0xc0,0x74,0x07,0x68,0x58,0x02,0x00,0x00,0xeb,0x02,0x6a,0x46,0x6a,0x0e\n"
    "  call _h_message\n"
    "  .byte 0x83,0xc4,0x08,0x8b,0x0d\n"
    "  .long _g_ctl_global + 4 * 237\n"
    "  .byte 0x6a,0x00,0x6a,0x01,0x6a,0x00,0x6a,0x00,0x50,0x6a,0x00,0x68\n"
    "  .long _fake_dead_chosen\n"
    "  call _fake_yesno_ask_site\n"
    "  .byte 0xc2,0x08,0x00\n"

    // The class's ask (this = the controller; seven arguments, ret 0x1C):
    // the game's test of a pending question, then the stand-in's body.
    ".p2align 4\n"
    ".globl _fake_yesno_ask_site\n"
    "_fake_yesno_ask_site:\n"
    "  .byte 0x56,0x8b,0xf1,0x8b,0x46,0x14,0x85,0xc0,0x74,0x06,0x32,0xc0,0x5e,0xc2,0x1c,0x00\n"
    "  .byte 0x5e\n"
    "  jmp @h_yesno_ask@36\n"

    // That callback (cdecl; context, answer), the game's bytes, run when
    // comyn's question is answered: on a yes, +0x24 of the controller in
    // dead's global (row 51) set.
    ".p2align 4\n"
    ".globl _fake_dead_chosen\n"
    "_fake_dead_chosen:\n"
    "  .byte 0x8a,0x44,0x24,0x08,0x84,0xc0,0x74,0x09,0xa1\n"
    "  .long _g_ctl_global + 4 * 51\n"
    "  .byte 0xc6,0x40,0x24,0x01,0xc3\n"

    // The KO window's tick (this = the controller), ret 4, never run: the
    // game's bytes around the clock and the decider.
    ".p2align 4\n"
    ".globl _fake_dead_tick\n"
    "_fake_dead_tick:\n"
    "  .byte 0x56,0x8b,0xf1\n"
    "  call _h_never\n"
    "  .byte 0x8b,0x4e,0x1c,0x3b,0xc8,0x76,0x12,0x2b,0xc8,0x8d,0x04,0x49,0x8d,0x04,0x80,0xc1,0xe0,0x02\n"
    "  .byte 0x89,0x46,0x20,0x5e,0xc2,0x04,0x00,0x6a,0x00,0x6a,0x01\n"
    "  call _h_dead_decider\n"
    "  .byte 0x83,0xc4,0x08,0x84,0xc0,0x74,0x0b,0xc7,0x46,0x20,0x1e,0x00,0x00,0x00,0x5e,0xc2,0x04,0x00\n"
    "  .byte 0x8b,0xce,0xc6,0x46,0x24,0x01,0xc7,0x46,0x20,0x00,0x00,0x00,0x00\n"
    "  call _h_never\n"
    "  .byte 0x5e,0xc2,0x04,0x00\n"

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

    // The per-menu draw (this = the menu, plain ret), the game's head: its
    // update's call a stand-in, the input wait counted down; then the
    // stand-in drawing the menu.
    ".p2align 4\n"
    ".globl _fake_menu_draw\n"
    "_fake_menu_draw:\n"
    "  .byte 0x83,0xec,0x24,0x53,0x55,0x56,0x57,0x8b,0xe9\n"
    "  call @h_menu_update@8\n"
    "  .byte 0x66,0x8b,0x45,0x56,0x33,0xff,0x66,0x3b,0xc7,0x7e,0x05,0x48,0x66,0x89,0x45,0x56,0x55\n"
    "  call _h_menu_draw@4\n"
    "  .byte 0x5f,0x5e,0x5d,0x5b,0x83,0xc4,0x24,0xc3\n"

    // The quad routine (this = a sprite; five arguments; ret 0x14), the
    // game's head reading the renderer's device, then its quad drawn with
    // the sprite for vertices through the draw primitive, called at +0x231
    // as the game's first call of it is.
    ".p2align 4\n"
    ".globl _fake_quad_draw\n"
    "_fake_quad_draw:\n"
    "  .byte 0x8b,0x44,0x24,0x08,0x83,0xec,0x10,0x53,0x32,0xdb,0x85,0xc0,0xa1\n"
    "  .long _g_render_ptr\n"
    "  .byte 0x56,0x57,0x8b,0xf9,0x8b,0x40,0x0c,0x8b,0x08,0x0f,0x84\n"
    "  .long 1f - (. + 4)\n"
    "1:\n"
    "  .byte 0x6a,0x18,0x57,0x6a,0x02,0x6a,0x05,0x8b,0x0d\n"
    "  .long _g_render_ptr\n"
    "  .byte 0xe9\n"
    "  .long 2f - (. + 4)\n"
    "  .org _fake_quad_draw + 0x231, 0xcc\n"
    "2:\n"
    "  call _fake_primitive_draw\n"
    "  .byte 0x5f,0x5e,0x5b,0x83,0xc4,0x10,0xc2,0x14,0x00\n"

    // The draw primitive (this = the renderer; type, count, vertices,
    // stride; ret 0x10), the game's bytes: the two-target test, which says
    // no, then the device's DrawPrimitiveUP. The two-target path past the
    // signature's bytes is never run.
    ".p2align 4\n"
    ".globl _fake_primitive_draw\n"
    "_fake_primitive_draw:\n"
    "  .byte 0x83,0xec,0x08,0x8d,0x44,0x24,0x00,0x56,0x8b,0xf1,0x8d,0x4c,0x24,0x08,0x50,0x51,0x8b,0xce\n"
    "  call _fake_two_targets\n"
    "  .byte 0x84,0xc0,0x0f,0x84\n"
    "  .long 1f - (. + 4)\n"
    "  .byte 0x53,0x55,0x57,0x8b,0xbe,0x10,0x08,0x00,0x00,0x8b,0xce,0xc7,0x86,0x10,0x08,0x00,0x00,0x00,0x00,0x00\n"
    "  .byte 0x00\n"
    "  call _fake_projection\n"
    "  .byte 0x8b,0x5c,0x24,0x28,0x5f,0x5d,0x5b\n"
    "1:\n"
    "  .byte 0x8b,0x4c,0x24,0x1c,0x8b,0x54,0x24,0x18,0x8b,0x76,0x0c,0x51,0x8b,0x4c,0x24,0x18,0x52,0x8b,0x54,0x24\n"
    "  .byte 0x18,0x8b,0x06,0x51,0x52,0x56,0xff,0x90,0x20,0x01,0x00,0x00,0x5e,0x83,0xc4,0x08,0xc2,0x10,0x00\n"

    // The two-target test (this = the renderer; two out pointers; ret 8):
    // the renderer's second target is off. The projection (this = the
    // renderer), never run.
    ".p2align 4\n"
    ".globl _fake_two_targets\n"
    "_fake_two_targets:\n"
    "  .byte 0x32,0xc0,0xc2,0x08,0x00\n"
    ".p2align 4\n"
    ".globl _fake_projection\n"
    "_fake_projection:\n"
    "  .byte 0xc3\n"

    // The sprite draw (this = the sprite; eight arguments; ret 0x20), the
    // game's head, then its one quad.
    ".p2align 4\n"
    ".globl _fake_sprite_draw\n"
    "_fake_sprite_draw:\n"
    "  .byte 0x83,0xec,0x0c,0x53,0x55,0x56,0x8b,0xf1,0x57,0x8b,0x6e,0x14,0x8d,0x4e,0x04,0x89,0x6c,0x24,0x18\n"
    "  .byte 0x89,0x4c,0x24,0x14,0x6a,0x00,0x6a,0x00,0x6a,0x00,0x6a,0x01,0x6a,0x00,0x8b,0xce\n"
    "  call _fake_quad_draw\n"
    "  .byte 0x5f,0x5e,0x5d,0x5b,0x83,0xc4,0x0c,0xc2,0x20,0x00\n"

    // The target window's controller draw (this = the controller, plain
    // ret): its content, one quad; then the game's bytes for its arrows,
    // the target arrow while +0xBC is set and the picking one while +0xB8
    // is, each through the sprite draw on the sprite at +0x78.
    ".p2align 4\n"
    ".globl _fake_target_draw\n"
    "_fake_target_draw:\n"
    "  .byte 0x83,0xec,0x08,0x56,0x8b,0xf1,0x57,0xb9\n"
    "  .long _g_sprites + 0x40\n"
    "  .byte 0x6a,0x00,0x6a,0x00,0x6a,0x00,0x6a,0x01,0x6a,0x00\n"
    "  call _fake_quad_draw\n"
    "  .byte 0xbf\n"
    "  .long 0xe0d0d040\n"
    "  .byte 0x66,0x8b,0x86,0xbc,0x00,0x00,0x00,0x66,0x85,0xc0\n"
    "  je 2f\n"
    "  .byte 0x0f,0xbf,0x96,0xbe,0x00,0x00,0x00,0x6a,0x00,0x6a,0x00,0x6a,0x00,0x33,0xc9,0x8a,0x8e,0xba,0x00,0x00\n"
    "  .byte 0x00,0x68,0x80,0x80,0x80,0xe0,0x0f,0xbf,0xc0\n"
    "  .byte 0x8b,0x4c,0x8e,0x78,0x68,0x00,0x00,0x80,0x3f,0x68,0x00,0x00,0x80,0x3f,0x52,0x50\n"
    "  call _fake_sprite_draw\n"
    "2:\n"
    "  .byte 0x8a,0x86,0xb8,0x00,0x00,0x00,0x84,0xc0\n"
    "  je 3f\n"
    "  .byte 0x0f,0xbf,0x96,0xc2,0x00,0x00,0x00,0x0f,0xbf,0x86,0xc0,0x00,0x00,0x00,0x6a,0x00,0x6a,0x00,0x6a,0x00\n"
    "  .byte 0x33,0xc9,0x8a,0x8e,0xba,0x00,0x00,0x00,0x57,0x68,0x00,0x00,0x80,0x3f,0x68,0x00,0x00,0x80,0x3f\n"
    "  .byte 0x8b,0x4c,0x8e,0x78,0x52,0x50\n"
    "  call _fake_sprite_draw\n"
    "3:\n"
    "  .byte 0x5f,0x5e,0x83,0xc4,0x08,0xc3\n"

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
    "  .byte 0xff,0x74,0x24,0x10,0x53,0xff,0x74,0x24,0x10\n"
    "  call _h_ability_kind@12\n"
    "  .byte 0x5b\n"
    "1:\n"
    "  .byte 0xc3\n"

    // The magic opener(kind, sort, mask), cdecl, nothing returned, the
    // game's bytes: the player global, magic opened by name through the open
    // routine with the manager in ecx, then the three arguments handed to
    // the controller in magic's registry slot (row 254), the stand-in for its
    // rebuild; the je at +7 lands on the ret, as the game's does.
    ".p2align 4\n"
    ".globl _fake_magic_open\n"
    "_fake_magic_open:\n"
    "  .byte 0xa1\n"
    "  .long _g_ability_player\n"
    "  .byte 0x85,0xc0,0x74\n"
    "  .byte 1f - (. + 1)\n"
    "  .byte 0x6a,0x00,0x6a,0x01,0x68\n"
    "  .long _g_key_magic\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  call _fake_open\n"
    "  .byte 0x8b,0x44,0x24,0x0c,0x8b,0x4c,0x24,0x08,0x8b,0x54,0x24,0x04,0x50,0x51,0x8b,0x0d\n"
    "  .long _g_ctl_global + 4 * 254\n"
    "  .byte 0x52\n"
    "  call @h_magic_setup@20\n"
    "1:\n"
    "  .byte 0xc3\n"

    // The equipment set's opener(this = equip's controller), plain ret, the
    // game's bytes and layout around stand-in callees: three tests, each
    // printing the game's refusal (0x4E, 0x4D, 0xA8) and returning; the
    // guard on the controller in equip's registry slot (row 41) and its
    // +0x1A word; the nine lists' resets; persona's (row 82) mode set to 2,
    // persona and money opened by name; the list object's step; inventor's
    // controller (row 72) handed the list, which opens inventor and
    // iteminfo; equip opened by name; the 0x061 request.
    ".p2align 4\n"
    ".globl _fake_equip_open\n"
    "_fake_equip_open:\n"
    "  call _h_set_test1\n"
    "  .byte 0x84,0xc0,0x74,0x0e,0x8b,0x0d\n"
    "  .long _g_chat_owner\n"
    "  .byte 0x6a,0x4e\n"
    "  call @h_set_message@12\n"
    "  .byte 0xc3\n"
    "  call _h_set_test2\n"
    "  .byte 0x84,0xc0,0x74,0x0e,0x8b,0x0d\n"
    "  .long _g_chat_owner\n"
    "  .byte 0x6a,0x4d\n"
    "  call @h_set_message@12\n"
    "  .byte 0xc3\n"
    "  .byte 0x6a,0x00\n"
    "  call _h_set_test3\n"
    "  .byte 0x83,0xc4,0x04,0x84,0xc0,0x75,0x11,0x8b,0x0d\n"
    "  .long _g_chat_owner\n"
    "  .byte 0x68,0xa8,0x00,0x00,0x00\n"
    "  call @h_set_message@12\n"
    "  .byte 0xc3\n"
    "  .byte 0xa1\n"
    "  .long _g_ctl_global + 4 * 41\n"
    "  .byte 0x85,0xc0,0x0f,0x84\n"
    "  .long 9f - . - 4\n"
    "  .byte 0x66,0x83,0x78,0x1a,0x00,0x0f,0x85\n"
    "  .long 9f - . - 4\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_list_resets + 0\n"
    "  .byte 0x6a,0x00,0x6a,0x12,0x6a,0x02,0x68,0xff,0xff,0x00,0x00\n"
    "  call @h_list_reset@24\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_list_resets + 4\n"
    "  .byte 0x6a,0x00,0x6a,0x12,0x6a,0x02,0x68,0xff,0xff,0x00,0x00\n"
    "  call @h_list_reset@24\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_list_resets + 8\n"
    "  .byte 0x6a,0x00,0x6a,0x12,0x6a,0x02,0x68,0xff,0xff,0x00,0x00\n"
    "  call @h_list_reset@24\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_list_resets + 12\n"
    "  .byte 0x6a,0x00,0x6a,0x12,0x6a,0x02,0x68,0xff,0xff,0x00,0x00\n"
    "  call @h_list_reset@24\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_list_resets + 16\n"
    "  .byte 0x6a,0x00,0x6a,0x12,0x6a,0x02,0x68,0xff,0xff,0x00,0x00\n"
    "  call @h_list_reset@24\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_list_resets + 20\n"
    "  .byte 0x6a,0x00,0x6a,0x12,0x6a,0x02,0x68,0xff,0xff,0x00,0x00\n"
    "  call @h_list_reset@24\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_list_resets + 24\n"
    "  .byte 0x6a,0x00,0x6a,0x12,0x6a,0x02,0x68,0xff,0xff,0x00,0x00\n"
    "  call @h_list_reset@24\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_list_resets + 28\n"
    "  .byte 0x6a,0x00,0x6a,0x12,0x6a,0x02,0x68,0xff,0xff,0x00,0x00\n"
    "  call @h_list_reset@24\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_list_resets + 32\n"
    "  .byte 0x6a,0x00,0x6a,0x12,0x6a,0x02,0x68,0xff,0xff,0x00,0x00\n"
    "  call @h_list_reset@24\n"
    "  .byte 0xa1\n"
    "  .long _g_ctl_global + 4 * 82\n"
    "  .byte 0x6a,0x00,0x6a,0x01,0x68\n"
    "  .long _g_key_persona\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xc6,0x40,0x1c,0x02\n"
    "  call _fake_open\n"
    "  .byte 0x6a,0x00,0x6a,0x01,0x68\n"
    "  .long _g_key_money\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  call _fake_open\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_equipment_list_ptr\n"
    "  .byte 0x6a,0x00,0x8b,0x11,0xff,0x52,0x58,0xa1\n"
    "  .long _g_equipment_list_ptr\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_ctl_global + 4 * 72\n"
    "  .byte 0x50\n"
    "  call @h_inventor_list@12\n"
    "  .byte 0x6a,0x00,0x6a,0x01,0x68\n"
    "  .long _g_key_equip\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  call _fake_open\n"
    "  .byte 0x6a,0x01,0x6a,0x00\n"
    "  call _h_stats_request\n"
    "  .byte 0x83,0xc4,0x08\n"
    "9:\n"
    "  .byte 0xc3\n"

    // The equipment shortcut, never run: the Monstrosity check, then the
    // set's opener on the controller in equip's registry slot.
    ".p2align 4\n"
    ".globl _fake_equip_shortcut\n"
    "_fake_equip_shortcut:\n"
    "  call _fake_monstrosity\n"
    "  .byte 0x84,0xc0,0x75,0x0b,0x8b,0x0d\n"
    "  .long _g_ctl_global + 4 * 41\n"
    "  call _fake_equip_open\n"
    "  .byte 0xb8,0x01,0x00,0x00,0x00,0xc3\n"

    // The Monstrosity check, cdecl, al the answer, the game's bytes: the
    // player data's word +0x10 non-zero, or the game's mode byte 0x17; the
    // rest of eax the data's pointer when it has one.
    ".p2align 4\n"
    ".globl _fake_monstrosity\n"
    "_fake_monstrosity:\n"
    "  .byte 0xa1\n"
    "  .long _g_mon_player_ptr\n"
    "  .byte 0x85,0xc0,0x74,0x0d,0x66,0x83,0xb8,0x10,0x00,0x00,0x00,0x00,0x74,0x03,0xb0,0x01,0xc3,0x80,0x3d\n"
    "  .long _g_game_mode\n"
    "  .byte 0x17,0x0f,0x94,0xc0,0xc3\n"

    // The yes/no class's input routine(this = the controller; event, row),
    // ret 8: a jump to the body that counts its calls, and behind it the
    // game's cancel, never run: the cancellable byte tested, then the
    // callback called with the context and no.
    ".p2align 4\n"
    ".globl _fake_yesno_input_site\n"
    "_fake_yesno_input_site:\n"
    "  jmp @fake_yesno_input@16\n"
    "  .byte 0x8a,0x4e,0x39,0x84,0xc9,0x0f,0x84,0x00,0x00,0x00,0x00,0x8b,0x56,0x18,0x6a,0x00,0x52,0xff,0xd0,0xc3\n"

    // moneyctr's confirm(this = the controller; number), ret 4, the game's
    // bytes: the callback with the number and the context, the callback
    // cleared, the extra callback, then close by instance through the
    // manager.
    ".p2align 4\n"
    ".globl _fake_money_confirm\n"
    "_fake_money_confirm:\n"
    "  .byte 0x56,0x8b,0xf1,0x8b,0x46,0x2c,0x85,0xc0,0x74,0x15,0x8b,0x4e,0x30,0x8b,0x54,0x24,0x08,0x51,0x52,0xff,0xd0\n"
    "  .byte 0x83,0xc4,0x08,0xc7,0x46,0x2c,0x00,0x00,0x00,0x00,0x8b,0x46,0x34,0x85,0xc0,0x74,0x09,0xff,0xd0\n"
    "  .byte 0xc7,0x46,0x34,0x00,0x00,0x00,0x00,0x8b,0x46,0x08,0x6a,0x01,0x50,0xb9\n"
    "  .long _g_mcb\n"
    "  call @h_close_by_instance@16\n"
    "  .byte 0x5e,0xc2,0x04,0x00\n"

    // itemctrl's confirm(this = the controller; number), ret 4, the game's
    // bytes.
    ".p2align 4\n"
    ".globl _fake_item_confirm\n"
    "_fake_item_confirm:\n"
    "  .byte 0x56,0x8b,0xf1,0x8b,0x46,0x24,0x85,0xc0,0x74,0x15,0x8b,0x4e,0x2c,0x8b,0x54,0x24,0x08,0x51,0x52,0xff,0xd0\n"
    "  .byte 0x83,0xc4,0x08,0xc7,0x46,0x24,0x00,0x00,0x00,0x00,0x8b,0x46,0x08,0x6a,0x01,0x50,0xb9\n"
    "  .long _g_mcb\n"
    "  call @h_close_by_instance@16\n"
    "  .byte 0x5e,0xc2,0x04,0x00\n"

    // moneyctr's setup(this = the controller; initial, max, callback,
    // context), ret 0x10, the game's bytes: a max of 0 is the gil record's,
    // then the question written and the window opened by its key through
    // the open routine.
    ".p2align 4\n"
    ".globl _fake_money_setup\n"
    "_fake_money_setup:\n"
    "  .byte 0x8b,0x44,0x24,0x04,0x56,0x8b,0xf1,0x89,0x46,0x28,0x8b,0x44,0x24,0x0c,0x85,0xc0,0x75,0x15,0x50,0x50\n"
    "  call _fake_item_record\n"
    "  .byte 0x83,0xc4,0x08,0x85,0xc0,0x74,0x05,0x8b,0x40,0x04,0xeb,0x02,0x33,0xc0\n"
    "  .byte 0x8b,0x4c,0x24,0x10,0x8b,0x54,0x24,0x14,0x6a,0x00,0x89,0x4e,0x2c,0x6a,0x01,0x68\n"
    "  .long _g_key_moneyctr\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0x89,0x46,0x24,0x89,0x56,0x30,0xc7,0x46,0x34,0x00,0x00,0x00,0x00\n"
    "  call _fake_open\n"
    "  .byte 0x5e,0xc2,0x10,0x00\n"

    // itemctrl's setup(this = the controller; initial, max, callback,
    // context, preview), ret 0x14, the game's bytes: a max of 0 writes
    // nothing, a max of 1 calls the callback with no open.
    ".p2align 4\n"
    ".globl _fake_item_setup\n"
    "_fake_item_setup:\n"
    "  .byte 0x8b,0x44,0x24,0x08,0x53,0x85,0xc0,0x56,0x74,0x4a,0x8b,0x74,0x24,0x1c,0x8a,0x5c,0x24,0x0c\n"
    "  .byte 0x8b,0x54,0x24,0x14,0x89,0x71,0x28,0x8b,0x74,0x24,0x18,0x83,0xf8,0x01,0x88,0x59,0x23,0x88,0x41,0x22\n"
    "  .byte 0x89,0x51,0x24,0x89,0x71,0x2c,0x74,0x18,0x6a,0x00,0x6a,0x01,0x68\n"
    "  .long _g_key_itemctrl\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  call _fake_open\n"
    "  .byte 0x5e,0x5b,0xc2,0x14,0x00\n"
    "  .byte 0x85,0xd2,0x74,0x0a,0x0f,0xbe,0xc3,0x56,0x50,0xff,0xd2,0x83,0xc4,0x08,0x5e,0x5b,0xc2,0x14,0x00\n"

    // The askers, in kAskerSites' order: the outgoing box's gil callback,
    // never run, the box's controller global (row 54) where the game's has
    // it.
    ".p2align 4\n"
    ".globl _fake_ask_delivery_gil\n"
    "_fake_ask_delivery_gil:\n"
    "  .byte 0x8b,0x44,0x24,0x04,0x85,0xc0,0x7e,0x1d,0x50,0x6a,0x00,0x6a,0x00\n"
    "  call _h_never\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_ctl_global + 4 * 54\n"
    "  .byte 0x83,0xc4,0x08,0x50\n"
    "  call _h_never\n"
    "  .byte 0x84,0xc0,0x75,0x0e,0xa1\n"
    "  .long _g_ctl_global + 4 * 54\n"
    "  .byte 0x66,0xc7,0x80,0xb4,0x01,0x00,0x00,0x09,0x00,0x8b,0x0d\n"
    "  .long _g_ctl_global + 4 * 54\n"
    "  .byte 0x6a,0x00,0x8b,0x51,0x08,0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0x52\n"
    "  call _h_never\n"
    "  .byte 0xc3\n"

    // The auction's bid callback, never run.
    ".p2align 4\n"
    ".globl _fake_ask_auction_bid\n"
    "_fake_ask_auction_bid:\n"
    "  .byte 0x8b,0x4c,0x24,0x04,0x85,0xc9,0x0f,0x8e,0x00,0x00,0x00,0x00,0x56,0x8b,0x74,0x24,0x0c,0x89,0x0d\n"
    "  .long _g_net\n"
    "  .byte 0x33,0xc0,0x8b,0x16,0x51,0x83,0xfa,0x04,0x0f,0x95,0xc0,0xa3\n"
    "  .long _g_net\n"
    "  .byte 0xf7,0xd8,0x1b,0xc0,0x83,0xc0,0x02,0x50,0x33,0xc0,0x66,0x8b,0x46,0x04,0x50,0x6a,0x01,0x6a,0x00,0x51,0xc3\n"

    // The auction's sale price callback, never run.
    ".p2align 4\n"
    ".globl _fake_ask_auction_sell\n"
    "_fake_ask_auction_sell:\n"
    "  .byte 0x8b,0x44,0x24,0x04,0x85,0xc0,0x7e,0x57,0x8b,0x0d\n"
    "  .long _g_net\n"
    "  .byte 0x6a,0x00,0x6a,0x01,0xa3\n"
    "  .long _g_net\n"
    "  .byte 0x8b,0x44,0x24,0x10,0x6a,0x00,0x6a,0x00,0x6a,0x00,0x50,0x68\n"
    "  .long _g_net\n"
    "  call _h_never\n"
    "  .byte 0xa1\n"
    "  .long _g_net\n"
    "  .byte 0x85,0xc0,0x74,0x51,0x6a,0x02,0x6a,0x38,0x6a,0x0e,0xc3\n"

    // The bazaar's price site, never run.
    ".p2align 4\n"
    ".globl _fake_ask_bazaar_price\n"
    "_fake_ask_bazaar_price:\n"
    "  .byte 0xc6,0x47,0x10,0x01,0x8a,0x46,0x02,0x88,0x47,0x11,0x88,0x5f,0x12,0x8a,0x4e,0x08,0x33,0xc0,0x80,0xf9,0x19\n"
    "  .byte 0x75,0x03,0x8b,0x46,0x0c,0x8b,0x0d\n"
    "  .long _g_ctl_global + 4 * 45\n"
    "  .byte 0x56,0x68\n"
    "  .long _fake_price_bazaar\n"
    "  .byte 0x68,0xff,0xe0,0xf5,0x05,0x50\n"
    "  call _fake_money_setup\n"
    "  .byte 0x5f,0x5e,0xb0,0x01,0x5b,0xc2,0x08,0x00,0x5f,0x5e,0x32,0xc0,0x5b,0xc3\n"

    // The outgoing box's item count callback, never run, its controller
    // global (row 54).
    ".p2align 4\n"
    ".globl _fake_ask_delivery_item\n"
    "_fake_ask_delivery_item:\n"
    "  .byte 0x8b,0x44,0x24,0x04,0x85,0xc0,0x7e,0x14,0x8b,0x4c,0x24,0x08,0x0f,0xbe,0xc0,0x50,0x51,0x8b,0x0d\n"
    "  .long _g_ctl_global + 4 * 54\n"
    "  call _h_never\n"
    "  .byte 0x8b,0x15\n"
    "  .long _g_ctl_global + 4 * 54\n"
    "  .byte 0x6a,0x00,0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0x8b,0x42,0x08,0x50\n"
    "  call _h_never\n"
    "  .byte 0xc3\n"

    // handover's item count callback, never run, its controller global
    // (row 55).
    ".p2align 4\n"
    ".globl _fake_ask_handover_item\n"
    "_fake_ask_handover_item:\n"
    "  .byte 0x8b,0x44,0x24,0x04,0x85,0xc0,0x7e,0x18,0x50,0x8b,0x44,0x24,0x0c,0x66,0x8b,0x08,0x8a,0x50,0x02,0x51,0x8b,0x0d\n"
    "  .long _g_ctl_global + 4 * 55\n"
    "  .byte 0x52\n"
    "  call _fake_handover_stage\n"
    "  .byte 0xa1\n"
    "  .long _g_ctl_global + 4 * 55\n"
    "  .byte 0x6a,0x00,0x8b,0x48,0x08,0x51,0xb9\n"
    "  .long _g_mcb\n"
    "  call _h_never\n"
    "  .byte 0xc3\n"

    // trade's item count callback(number, context), cdecl, the game's
    // bytes, run: a number above 0 staged through trade's controller (row
    // 43), then the focus back to trade's window.
    ".p2align 4\n"
    ".globl _fake_ask_trade_item\n"
    "_fake_ask_trade_item:\n"
    "  .byte 0x8b,0x44,0x24,0x04,0x85,0xc0,0x7e,0x1a,0x50,0x8b,0x44,0x24,0x0c,0x66,0x0f,0xb6,0x50,0x02,0x66,0x8b,0x08\n"
    "  .byte 0x51,0x8b,0x0d\n"
    "  .long _g_ctl_global + 4 * 43\n"
    "  .byte 0x52\n"
    "  call _fake_trade_stage\n"
    "  .byte 0xa1\n"
    "  .long _g_ctl_global + 4 * 43\n"
    "  .byte 0x6a,0x00,0x8b,0x48,0x08,0x51,0xb9\n"
    "  .long _g_mcb\n"
    "  call @h_focus@16\n"
    "  .byte 0xc3\n"

    // The Mog storage transfer callback, never run.
    ".p2align 4\n"
    ".globl _fake_ask_storage\n"
    "_fake_ask_storage:\n"
    "  .byte 0x8b,0x44,0x24,0x08,0x85,0xc0,0x74,0x36,0x8b,0x4c,0x24,0x04,0x85,0xc9,0x7e,0x1b,0x8b,0x10,0x51,0x8b\n"
    "  .byte 0x48,0x08,0x6a,0x52,0x51,0x33,0xc9,0x8a,0x4a,0x02,0x8b,0x50,0x04,0x51,0x52\n"
    "  call _h_never\n"
    "  .byte 0x83,0xc4,0x14,0xa1\n"
    "  .long _g_net\n"
    "  .byte 0xc6,0x40,0x04,0x00,0x8b,0x0d\n"
    "  .long _g_net\n"
    "  .byte 0xc6,0x41,0x04,0x00,0xc3\n"

    // The bazaar's buy callback, never run.
    ".p2align 4\n"
    ".globl _fake_ask_bazaar_buy\n"
    "_fake_ask_bazaar_buy:\n"
    "  .byte 0x83,0xec,0x54,0x56,0x8b,0x74,0x24,0x5c,0x85,0xf6,0x7e,0x6d,0x57,0xb9,0x15,0x00,0x00,0x00,0x33,0xc0\n"
    "  .byte 0x8d,0x7c,0x24,0x08,0xf3,0xab,0x8b,0x0d\n"
    "  .long _g_net\n"
    "  .byte 0xa1\n"
    "  .long _g_net\n"
    "  .byte 0x0f,0xaf,0xce,0x8d,0x54,0x24,0x08,0x25,0xff,0xff,0x00,0x00,0x52,0x66,0xc7,0x44,0x24,0x14,0x5e,0x00\n"
    "  .byte 0x89,0x44,0x24,0x18,0xc3\n"

    // Synthesis's ingredient count callback, never run, the synthesis
    // window's controller global (row 118).
    ".p2align 4\n"
    ".globl _fake_ask_synthesis\n"
    "_fake_ask_synthesis:\n"
    "  .byte 0x8b,0x44,0x24,0x04,0x85,0xc0,0x7e,0x18,0x50,0x8b,0x44,0x24,0x0c,0x66,0x8b,0x08,0x8a,0x50,0x02,0x51,0x8b,0x0d\n"
    "  .long _g_ctl_global + 4 * 118\n"
    "  .byte 0x52\n"
    "  call _h_never\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_net\n"
    "  .byte 0x6a,0x00,0x6a,0x00,0x6a,0x00,0x6a,0x00\n"
    "  call _h_never\n"
    "  .byte 0xa1\n"
    "  .long _g_ctl_global + 4 * 118\n"
    "  .byte 0x6a,0x00,0x8b,0x48,0x08,0x51,0xc3\n"

    // The guild shop's buy and sell sites and the NPC shop's, never run,
    // each pushing its stand-in callback and calling itemctrl's setup on
    // itemctrl's global (row 80).
    ".p2align 4\n"
    ".globl _fake_ask_guild_buy\n"
    "_fake_ask_guild_buy:\n"
    "  .byte 0x8b,0xc8,0x33,0xc0,0x8a,0x45,0x02,0x3b,0xc8,0x7e,0x02,0x8b,0xc8,0x85,0xc9,0x74,0x1f,0x6a,0x00,0x55,0x68\n"
    "  .long _fake_buy_guild\n"
    "  .byte 0x51,0x8b,0x0d\n"
    "  .long _g_ctl_global + 4 * 80\n"
    "  .byte 0x6a,0x01\n"
    "  call _fake_item_setup\n"
    "  .byte 0x5f,0x5e,0x5d,0xb0,0x01,0x5b,0xc2,0x08,0x00,0x5f,0x5e,0x5d,0x32,0xc0,0x5b,0xc2,0x08,0x00\n"
    ".p2align 4\n"
    ".globl _fake_ask_guild_sell\n"
    "_fake_ask_guild_sell:\n"
    "  .byte 0x85,0xc0,0x74,0x31,0x3d,0x40,0x40,0x40,0x80,0x74,0x2a,0xc6,0x47,0x10,0x01,0x8a,0x46,0x02,0x88,0x47,0x11\n"
    "  .byte 0x8b,0x4e,0x04,0x6a,0x00,0x56,0x68\n"
    "  .long _fake_sell_guild\n"
    "  .byte 0x51,0x8b,0x0d\n"
    "  .long _g_ctl_global + 4 * 80\n"
    "  .byte 0x6a,0x01\n"
    "  call _fake_item_setup\n"
    "  .byte 0x5f,0xb0,0x01,0x5e,0xc2,0x08,0x00,0x5f,0x32,0xc0,0x5e,0xc2,0x08,0x00\n"
    ".p2align 4\n"
    ".globl _fake_ask_shop_buy\n"
    "_fake_ask_shop_buy:\n"
    "  .byte 0x8b,0x44,0x24,0x24,0x33,0xd2,0xf7,0xf6,0x3b,0xc8,0x7e,0x02,0x8b,0xc8,0x85,0xc9,0x74,0xc6,0x6a,0x00\n"
    "  .byte 0x6a,0x00,0x68\n"
    "  .long _fake_buy_shop\n"
    "  .byte 0x51,0x8b,0x0d\n"
    "  .long _g_ctl_global + 4 * 80\n"
    "  .byte 0x6a,0x01\n"
    "  call _fake_item_setup\n"
    "  .byte 0x5f,0x5e,0x5d,0xb0,0x01,0x5b,0x83,0xc4,0x0c,0xc2,0x08,0x00\n"
    ".p2align 4\n"
    ".globl _fake_ask_shop_sell\n"
    "_fake_ask_shop_sell:\n"
    "  .byte 0xc6,0x47,0x10,0x01,0x8a,0x56,0x02,0x88,0x57,0x11,0x8b,0x46,0x04,0x8b,0x0d\n"
    "  .long _g_ctl_global + 4 * 80\n"
    "  .byte 0x6a,0x00,0x56,0x68\n"
    "  .long _fake_sell_shop\n"
    "  .byte 0x50,0x6a,0x01\n"
    "  call _fake_item_setup\n"
    "  .byte 0x5f,0xb0,0x01,0x5e,0xc2,0x08,0x00,0x5f,0x32,0xc0,0x5e,0xc2,0x08,0x00\n"
);

// The Asked windows' sites, byte for byte the signatures, in signatures.h's
// order; wildcards filled with whatever the code needs there.
asm(
    ".text\n"
    // sortyn's setup(this = the controller; callback, context, text, title,
    // default, y), ret 0x18, the game's bytes, run: refused with no
    // callback; the question written, its key opened through the open
    // routine, then the cursor put on no through select-row unless the
    // default is yes; 1 whether or not the window opened.
    ".p2align 4\n"
    ".globl _fake_sortyn_setup\n"
    "_fake_sortyn_setup:\n"
    "  .byte 0x8b,0x44,0x24,0x04,0x56,0x85,0xc0,0x8b,0xf1,0x75,0x06,0x32,0xc0,0x5e,0xc2,0x18,0x00\n"
    "  .byte 0x8b,0x4c,0x24,0x10,0x8b,0x54,0x24,0x14,0x89,0x46,0x14,0x8b,0x44,0x24,0x0c,0x89,0x46,0x18\n"
    "  .byte 0x8b,0x44,0x24,0x1c,0x6a,0x00,0x89,0x4e,0x1c,0x6a,0x01,0x68\n"
    "  .long _g_key_sortyn\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0x89,0x56,0x20,0x89,0x46,0x24\n"
    "  call _fake_open\n"
    "  .byte 0x8b,0x4e,0x08,0x85,0xc9,0x74,0x0f,0x8a,0x44,0x24,0x18,0x84,0xc0,0x75,0x07,0x6a,0x02\n"
    "  call _fake_select_row\n"
    "  .byte 0xb0,0x01,0x5e,0xc2,0x18,0x00\n"

    // sortyn's input routine from its confirm on, never run: the callback
    // with the context and the answer, cleared, its key closed by name.
    ".p2align 4\n"
    ".globl _fake_sortyn_confirm\n"
    "_fake_sortyn_confirm:\n"
    "  .byte 0x0f,0xbf,0x44,0x24,0x0c,0x48,0x74,0x0a,0x48,0x75,0x37,0xc6,0x44,0x24,0x08,0x00,0xeb,0x05\n"
    "  .byte 0xc6,0x44,0x24,0x08,0x01,0x8b,0x46,0x14,0x85,0xc0,0x74,0x15,0x8b,0x4c,0x24,0x08,0x8b,0x56,0x18\n"
    "  .byte 0x51,0x52,0xff,0xd0,0x83,0xc4,0x08,0xc7,0x46,0x14,0x00,0x00,0x00,0x00,0x68\n"
    "  .long _g_key_sortyn\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  call _fake_close\n"
    "  .byte 0x5e,0xc2,0x08,0x00\n"

    // The key item list's ask, never run: evitem01's key opened through the
    // open routine, then the setup on evitem01's controller (row 251).
    ".p2align 4\n"
    ".globl _fake_evitem_ask\n"
    "_fake_evitem_ask:\n"
    "  .byte 0x8b,0xf8,0x85,0xff,0x0f,0x8c,0x00,0x00,0x00,0x00,0x57\n"
    "  call _h_never\n"
    "  .byte 0x83,0xc4,0x04,0x8b,0xce,0x85,0xc0,0x75,0x30\n"
    "  call _h_never\n"
    "  .byte 0x8b,0xce\n"
    "  call _h_never\n"
    "  .byte 0x6a,0x00,0x6a,0x01,0x68\n"
    "  .long _g_key_evitem01\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  call _fake_open\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_ctl_global + 4 * 251\n"
    "  .byte 0x57\n"
    "  call _fake_evitem_setup\n"
    "  .byte 0x5f,0x5e,0xc2,0x08,0x00\n"

    // evitem01's setup(this = the controller; key item), ret 4, the game's
    // bytes, run: the question printed at +0x74 from the item's name and
    // its format, then the base setup given the callback below.
    ".p2align 4\n"
    ".globl _fake_evitem_setup\n"
    "_fake_evitem_setup:\n"
    "  .byte 0x8b,0x44,0x24,0x04,0x56,0x8b,0xf1,0x57,0x50,0x8d,0x7e,0x74\n"
    "  call _h_evitem_name\n"
    "  .byte 0x83,0xc4,0x04,0x50,0x6a,0x5f,0x6a,0x09\n"
    "  call _h_evitem_format\n"
    "  .byte 0x83,0xc4,0x08,0x50,0x57\n"
    "  call _h_evitem_print\n"
    "  .byte 0x83,0xc4,0x0c,0x8b,0xce,0x6a,0x00,0x6a,0x1e,0x6a,0x1c,0x57,0x6a,0x00,0x68\n"
    "  .long _fake_evitem_callback\n"
    "  call @h_evitem_base@32\n"
    "  .byte 0x5f,0x5e,0xc2,0x04,0x00\n"

    // Its callback(answer), cdecl, run: the key item list's controller
    // (row 250) read as the game's is, then the stand-in's body.
    ".p2align 4\n"
    ".globl _fake_evitem_callback\n"
    "_fake_evitem_callback:\n"
    "  .byte 0x56,0x8b,0x35\n"
    "  .long _g_ctl_global + 4 * 250\n"
    "  .byte 0xff,0x74,0x24,0x08\n"
    "  call _h_evitem_answer\n"
    "  .byte 0x83,0xc4,0x04,0x5e,0xc3\n"

    // evitem01's input routine from its code test on, never run: the
    // callback at +0x64, then close by instance with +0x70.
    ".p2align 4\n"
    ".globl _fake_evitem_input\n"
    "_fake_evitem_input:\n"
    "  .byte 0x0f,0xbf,0xc1,0x83,0xe8,0x05,0x74,0x1b,0x48,0x74,0x11,0x8b,0x44,0x24,0x0c,0x50,0x51,0x8b,0xce\n"
    "  call _h_never\n"
    "  .byte 0x5e,0xc2,0x08,0x00,0xc6,0x44,0x24,0x08,0x00,0xeb,0x0c,0x8b,0x46,0x30,0x85,0xc0,0x0f,0x94,0xc1\n"
    "  .byte 0x88,0x4c,0x24,0x08,0x8b,0x54,0x24,0x08,0x81,0xe2,0xff,0x00,0x00,0x00,0x52,0xff,0x56,0x64\n"
    "  .byte 0x8a,0x46,0x70,0x8b,0x4e,0x08,0x83,0xc4,0x04,0x50,0x51,0xb9\n"
    "  .long _g_mcb\n"
    "  call _h_close_menu@8\n"
    "  .byte 0x5e,0xc2,0x08,0x00\n"

    // confyn's input routine, never run: the restore stand-in, then close
    // by instance.
    ".p2align 4\n"
    ".globl _fake_confyn_input\n"
    "_fake_confyn_input:\n"
    "  .byte 0x66,0x83,0x7c,0x24,0x04,0x05,0x56,0x8b,0xf1,0x75,0x31,0x66,0x83,0x7c,0x24,0x0c,0x01,0x75,0x19\n"
    "  .byte 0x8b,0x46,0x14,0x83,0xe8,0x00,0x74,0x07,0x48,0x75,0x0e,0x6a,0x02,0xeb,0x02,0x6a,0x01\n"
    "  call _h_confyn_restore\n"
    "  .byte 0x83,0xc4,0x04,0x8b,0x46,0x08,0x6a,0x01,0x50,0xb9\n"
    "  .long _g_mcb\n"
    "  call _h_close_menu@8\n"
    "  .byte 0x5e,0xc2,0x08,0x00\n"

    // bankmenu's open hook, never run, then its begin-close, run by close by
    // instance: the jump to the clear of its callback.
    ".p2align 4\n"
    ".globl _fake_bank_open\n"
    "_fake_bank_open:\n"
    "  .byte 0xa1\n"
    "  .long _g_player_data_ptr\n"
    "  .byte 0x56,0x50,0x8b,0xf1\n"
    "  call _fake_bank_ask\n"
    "  .byte 0x83,0xc4,0x04,0x85,0xc0,0x75,0x0c,0x8b,0x4e,0x08,0x6a,0x02,0x6a,0x05\n"
    "  call _h_never\n"
    "  .byte 0x5e,0xc3\n"
    "  .fill 13, 1, 0x90\n"
    ".globl _fake_bank_begin_close\n"
    "_fake_bank_begin_close:\n"
    "  .byte 0xe9\n"
    "  .long _fake_bank_clear - (. + 4)\n"

    // The Mog Safe 2 test (cdecl; the player's data), and the read of its
    // flag (cdecl; data, else the global's), the game's bytes.
    ".p2align 4\n"
    ".globl _fake_bank_ask\n"
    "_fake_bank_ask:\n"
    "  .byte 0x8b,0x44,0x24,0x04,0x50\n"
    "  call _fake_bank_flag\n"
    "  .byte 0x83,0xc4,0x04,0x83,0xe0,0x01,0xc3\n"
    ".p2align 4\n"
    ".globl _fake_bank_flag\n"
    "_fake_bank_flag:\n"
    "  .byte 0x8b,0x44,0x24,0x04,0x85,0xc0,0x75,0x09,0xa1\n"
    "  .long _g_player_data_ptr\n"
    "  .byte 0x85,0xc0,0x74,0x0b,0x33,0xc9,0x8a,0x88\n"
    "  .long 0x21\n"
    "  .byte 0x8b,0xc1,0xc3,0x33,0xc0,0xc3\n"

    // The clear of bankmenu's callback (this = the controller), the game's.
    ".p2align 4\n"
    ".globl _fake_bank_clear\n"
    "_fake_bank_clear:\n"
    "  .byte 0xc7,0x41,0x14,0x00,0x00,0x00,0x00,0xc3\n"

    // bankmenu's input routine, never run, and its class's close.
    ".p2align 4\n"
    ".globl _fake_bank_input\n"
    "_fake_bank_input:\n"
    "  .byte 0x0f,0xbf,0x44,0x24,0x04,0x56,0x8b,0xf1,0x33,0xc9,0x66,0x83,0x7c,0x24,0x0c,0x01,0x0f,0x95,0xc1\n"
    "  .byte 0x83,0xe8,0x05,0x8d,0x0c,0xcd,0x01,0x00,0x00,0x00,0x74,0x1d,0x48,0x75,0x30,0x8b,0x46,0x14\n"
    "  .byte 0x85,0xc0,0x74,0x22,0x6a,0x01,0x51,0xff,0xd0,0x83,0xc4,0x08,0x8b,0xce\n"
    "  call _fake_bank_close\n"
    "  .byte 0x5e,0xc2,0x08,0x00,0x8b,0x46,0x14,0xc3\n"
    ".globl _fake_bank_close\n"
    "_fake_bank_close:\n"
    "  .byte 0x8b,0x01,0xff,0x60,0x48\n"

    // maplist's close+reset (this = the controller), the game's bytes, run:
    // its class's close, the set-rows and free-rows stand-ins. Then its key
    // handler, never run: maplist's global (row 206), the player data's
    // global, then its leaf confirm, its closing row's confirm, its cancel
    // at the top, each a call of close+reset, and the cancel's test of the
    // list it shows; its cancel inside a zone's maps and inside a region,
    // each a call of the rebuild and then of the select; its confirm of a
    // list row, a call of the cursor read, the rebuild and the select.
    ".p2align 4\n"
    ".globl _fake_maplist_close\n"
    "_fake_maplist_close:\n"
    "  .byte 0x56,0x8b,0xf1,0x8b,0x06,0xff,0x50,0x74,0x6a,0x01,0x6a,0x00,0x6a,0x00,0x8b,0xce\n"
    "  .byte 0xc7,0x46,0x34,0x00,0x00,0x00,0x00,0xc6,0x46,0x49,0x01\n"
    "  call @h_map_rows@20\n"
    "  .byte 0x8b,0xce,0x5e,0xe9\n"
    "  .long @h_map_free@8 - (. + 4)\n"
    "  .fill 8, 1, 0x90\n"
    "  .byte 0xa1\n"
    "  .long _g_ctl_global + 4 * 206\n"
    "  .byte 0x56,0x85,0xc0,0x8b,0xf1,0x0f,0x84,0x00,0x00,0x00,0x00\n"
    "  .byte 0xa1\n"
    "  .long _g_player_data_ptr\n"
    "  .byte 0x85,0xc9,0x7c,0x1f,0x8b,0x4e,0x6c,0x85,0xc9,0x74,0x06,0x50,0xff,0xd1,0x83,0xc4,0x04,0x8b,0xce\n"
    "  call _fake_maplist_close\n"
    "  .byte 0x8b,0x08,0x81,0xf9,0xff,0xff,0xff,0x7f,0x75,0x12,0x8b,0xce\n"
    "  call _fake_maplist_close\n"
    "  .byte 0x66,0x83,0x7e,0x58,0x00,0x75,0x0b\n"
    "  call _fake_maplist_close\n"
    "  .byte 0x66,0x83,0x7e,0x5c,0x00,0x8b,0xce,0x7c,0x00\n"
    "  .byte 0x66,0x8b,0x46,0x5e,0x66,0xc7,0x46,0x5c,0xff,0xff,0x66,0x89,0x46,0x58\n"
    "  call _fake_map_rebuild\n"
    "  .byte 0x0f,0xbf,0x4e,0x5a,0x6a,0x00,0x51,0x8b,0xce\n"
    "  call @h_map_select@16\n"
    "  .byte 0x66,0xc7,0x46,0x58,0x00,0x00,0x66,0xc7,0x46,0x5e,0x00,0x00\n"
    "  call _fake_map_rebuild\n"
    "  .byte 0x0f,0xbf,0x56,0x5a,0x6a,0x00,0x52,0x8b,0xce\n"
    "  call @h_map_select@16\n"
    "  .byte 0x66,0x8b,0x4e,0x58,0x66,0x89,0x4e,0x5e,0x66,0x8b,0x10,0x66,0xf7,0xda,0x66,0x89,0x56,0x58\n"
    "  .byte 0x66,0x8b,0x40,0x04,0x8b,0xce,0x66,0x89,0x46,0x5c\n"
    "  call _fake_map_cursor\n"
    "  .byte 0x8b,0xce,0x66,0x89,0x46,0x5a\n"
    "  call _fake_map_rebuild\n"
    "  .byte 0x6a,0x00,0x6a,0x00,0x8b,0xce\n"
    "  call @h_map_select@16\n"
    "  .byte 0x5e,0xc3\n"

    // maplist's rebuild (this = the controller), run: the game's head, its
    // set-rows and free-rows as close+reset's, then the builders' stand-in.
    // The list's cursor read, the game's bytes, run: the menu's row (+0x4C)
    // and the first row shown (+0x1E) less one, -1 with no window.
    ".p2align 4\n"
    ".globl _fake_map_rebuild\n"
    "_fake_map_rebuild:\n"
    "  .byte 0x53,0x56,0x57,0x6a,0x01,0x6a,0x00,0x6a,0x00,0x8b,0xf1\n"
    "  call @h_map_rows@20\n"
    "  .byte 0x8b,0xce\n"
    "  call @h_map_free@8\n"
    "  .byte 0x8b,0xce\n"
    "  call @h_map_build@8\n"
    "  .byte 0x5f,0x5e,0x5b,0xc3\n"
    ".p2align 4\n"
    "_fake_map_cursor:\n"
    "  .byte 0x8b,0x41,0x08,0x85,0xc0,0x74,0x0d,0x0f,0xbf,0x40,0x4c,0x0f,0xbf,0x49,0x1e,0x8d,0x44,0x08,0xff,0xc3\n"
    "  .byte 0x83,0xc8,0xff,0xc3\n"

    // map0's input routine, never run: its cancel closes the window by
    // instance, then calls the map's close with 0 on mapframe's global (row
    // 201). The map's close, run: the game's head, the stand-in that records
    // it, and from +0xAE the game's closes of map0, mapv3, mapv2, scanlist
    // and mapscan (rows 200, 202 to 205) by name and the frame's fade-out.
    ".p2align 4\n"
    ".globl _fake_map0_input\n"
    "_fake_map0_input:\n"
    "  .byte 0x0f,0xbf,0x44,0x24,0x04,0x83,0xe8,0x05,0x74,0x27,0x48,0x0f,0x85,0xc3,0x00,0x00,0x00\n"
    "  .byte 0x8b,0x41,0x08,0x6a,0x01,0x50,0xb9\n"
    "  .long _g_mcb\n"
    "  call _h_close_menu@8\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_ctl_global + 4 * 201\n"
    "  .byte 0x6a,0x00\n"
    "  call _fake_map_close\n"
    "  .byte 0xc2,0x08,0x00\n"
    ".p2align 4\n"
    ".globl _fake_map_close\n"
    "_fake_map_close:\n"
    "  .byte 0x8a,0x44,0x24,0x04,0x56,0x84,0xc0,0x8b,0xf1,0x74,0x17,0x8b,0x46,0x08\n"
    "  .byte 0x66,0xc7,0x86,0x04,0x01,0x00,0x00,0x08,0x00,0x85,0xc0,0x74,0x07,0xc7,0x40,0x7c,0x00,0x00,0x00,0x00\n"
    "  .byte 0x8b,0xce\n"
    "  call @h_map_marks@8\n"
    "  .byte 0xe9\n"
    "  .long 1f - . - 4\n"
    "  .fill 0x80, 1, 0xcc\n"
    "1:\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0x68\n"
    "  .long _g_registry + 0x2c * 200\n"
    "  call _fake_close\n"
    "  .byte 0x68\n"
    "  .long _g_registry + 0x2c * 202\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  call _fake_close\n"
    "  .byte 0x68\n"
    "  .long _g_registry + 0x2c * 203\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  call _fake_close\n"
    "  .byte 0x68\n"
    "  .long _g_registry + 0x2c * 204\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  call _fake_close\n"
    "  .byte 0x68\n"
    "  .long _g_registry + 0x2c * 205\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  call _fake_close\n"
    "  .byte 0x8b,0x16,0x8b,0xce,0xff,0x52,0x50,0x5e,0xc2,0x04,0x00\n"

    // The GM tell poller, never run: its result word read through the
    // getter below, the queue's slot global, then the slot's title pointer.
    ".p2align 4\n"
    ".globl _fake_gm_poll\n"
    "_fake_gm_poll:\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_ctl_global + 4 * 40\n"
    "  call _fake_gm_read\n"
    "  .byte 0x66,0x85,0xc0,0x0f,0x84,0x00,0x00,0x00,0x00,0x66,0x3d,0xff,0x00,0x0f,0x84,0x00,0x00,0x00,0x00\n"
    "  .byte 0x66,0x85,0xc0,0x0f,0x8c,0x00,0x00,0x00,0x00,0x8b,0x0d\n"
    "  .long _g_gm_slot\n"
    "  .byte 0x0f,0xbf,0xc0,0x8d,0x14,0x89,0xc1,0xe2,0x04,0x03,0xd0,0x8d,0x04,0x89,0xc1,0xe0,0x06\n"
    "  .byte 0x8b,0x14,0x95\n"
    "  .long _g_gm_queue + 0x18\n"
    "  .byte 0xc3\n"
    ".p2align 4\n"
    ".globl _fake_gm_read\n"
    "_fake_gm_read:\n"
    "  .byte 0x66,0x8b,0x81,0x48,0x05,0x00,0x00,0xc3\n"

    // mp_dead's input routine from its code test on, then its tick, never
    // run, the game's bytes around the chat logs' globals.
    ".p2align 4\n"
    ".globl _fake_mp_dead_input\n"
    "_fake_mp_dead_input:\n"
    "  .byte 0x83,0xe8,0x05,0x74,0x1d,0x48,0x75,0x33,0x8b,0x0d\n"
    "  .long _g_net\n"
    "  .byte 0xb8,0x01,0x00,0x00,0x00,0x88,0x41,0x59,0x8b,0x15\n"
    "  .long _g_net\n"
    "  .byte 0x88,0x42,0x59,0xc2,0x08,0x00,0xb8,0x01,0x00,0x00,0x00,0x66,0x39,0x44,0x24,0x08,0x75,0x06\n"
    "  .byte 0x89,0x41,0x24,0xc2,0x08,0x00,0xc7,0x41,0x24,0x02,0x00,0x00,0x00,0xc2,0x08,0x00\n"
    "  .fill 15, 1, 0x90\n"
    "  .byte 0x56,0x8b,0xf1\n"
    "  call _h_never\n"
    "  .byte 0x8b,0x4e,0x1c,0x3b,0xc8,0x76,0x12,0x2b,0xc8,0x8d,0x04,0x49,0x8d,0x04,0x80,0xc1,0xe0,0x02\n"
    "  .byte 0x89,0x46,0x20,0x5e,0xc2,0x04,0x00,0x8b,0xce,0xc7,0x46,0x24,0x02,0x00,0x00,0x00\n"
    "  .byte 0xc7,0x46,0x20,0x00,0x00,0x00,0x00\n"
    "  call _h_never\n"
    "  .byte 0x5e,0xc2,0x04,0x00\n"

    // ok's input routine, never run: the callback, close by instance, and
    // close again on +0x258.
    ".p2align 4\n"
    ".globl _fake_ok_input\n"
    "_fake_ok_input:\n"
    "  .byte 0x56,0x8b,0xf1,0x83,0xbe,0x5c,0x02,0x00,0x00,0x01,0x74,0x58,0x66,0x83,0x7c,0x24,0x08,0x05,0x75,0x50\n"
    "  .byte 0x8b,0x46,0x08,0xc6,0x40,0x5d,0x01,0xc7,0x86,0x5c,0x02,0x00,0x00,0x01,0x00,0x00,0x00\n"
    "  call _h_never\n"
    "  .byte 0x8b,0x46,0x14,0x85,0xc0,0x74,0x19,0x8b,0x4e,0x18,0x51,0xff,0xd0,0x8b,0x56,0x08,0x83,0xc4,0x04,0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0x6a,0x01,0x52\n"
    "  call _h_close_menu@8\n"
    "  .byte 0x8a,0x86,0x58,0x02,0x00,0x00,0x84,0xc0,0x74,0x10,0x8b,0x46,0x08,0x6a,0x01,0x50,0xb9\n"
    "  .long _g_mcb\n"
    "  call _h_close_menu@8\n"
    "  .byte 0x5e,0xc2,0x08,0x00\n"
);

// mogdoor and the NPC dialogue boxes, the game's bytes where the engine
// reads them: mogdoor's input routine (this = the controller; code, row;
// ret 8), never run, its floor read (cdecl), run by the engine; its open
// hook, run by the open; its tick (ret 4), run each frame it is up; its
// draw, the door's handler and the routine an exit gives the door, never
// run. The boxes' input routine, handler, text dispatch and its gate, never
// run, the gate recording each code at its end; the clear, and the gate's
// advance on code 5 and page-turn sound after it, stand-ins, run by the
// engine's replies; the box's show, the timer and the last-page test, never
// run; the boxes' tick (ret 4), the game's left-button test through the key
// test, then a stand-in that moves the page on. The vtables are in
// read-only data, as the game's are.
asm(
    ".text\n"
    ".p2align 4\n"
    ".globl _fake_door_input\n"
    "_fake_door_input:\n"
    "  .byte 0x53,0x56,0x8b,0xf1\n"
    "  call _fake_door_floor\n"
    "  .byte 0x0f,0xbf,0x44,0x24,0x0c,0x48,0x83,0xf8,0x08,0x0f,0x87\n"
    "  .long Ldoor_out - . - 4\n"
    "  .byte 0xff,0x24,0x85\n"
    "  .long Ldoor_codes\n"
    "Ldoor_confirm:\n"
    "  .byte 0x6a,0x00,0x6a,0x04\n"
    "  call _h_door_gate\n"
    "  .byte 0x83,0xc4,0x08,0x84,0xc0,0x0f,0x84\n"
    "  .long Ldoor_out - . - 4\n"
    "  .byte 0x8a,0x46,0x18,0xbb,0x01,0x00,0x00,0x00,0x84,0xc0,0x74,0x3b\n"
    "  .byte 0x66,0x8b,0x44,0x24,0x10,0x8b,0x56,0x1c,0x0f,0xbf,0xc8,0x66,0x81,0x7c,0x4a,0xfe,0x18,0x01\n"
    "  .byte 0x75,0x22,0x8a,0x46,0x19,0x84,0xc0,0x75,0x15,0x8b,0x0d\n"
    "  .long _g_never_data\n"
    "  .byte 0x68,0xbe,0x00,0x00,0x00\n"
    "  call _h_never\n"
    "  .byte 0x5e,0x5b,0xc2,0x08,0x00\n"
    "  .byte 0xc6,0x46,0x15,0x7f,0xeb,0x71,0x88,0x46,0x15,0xeb,0x6c\n"
    "  .byte 0x0f,0xbf,0x44,0x24,0x10,0x48,0x83,0xf8,0x03,0x77,0x61,0xff,0x24,0x85\n"
    "  .long Ldoor_rows\n"
    "Ldoor_stay:\n"
    "  .byte 0x8b,0x46,0x08,0x53,0x50,0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xc6,0x46,0x15,0xff\n"
    "  call _h_close_menu@8\n"
    "  .byte 0xeb,0x45\n"
    "Ldoor_back:\n"
    "  .byte 0xc6,0x46,0x15,0x00,0xeb,0x3f\n"
    "Ldoor_list:\n"
    "  .byte 0x8b,0x4e,0x08,0x6a,0x00,0x53,0xc6,0x46,0x15,0xff,0x88,0x5e,0x18\n"
    "  call _fake_set_cursor\n"
    "  .byte 0x8b,0x4e,0x08,0x66,0xc7,0x41,0x5e,0x00,0x00,0xeb,0x22\n"
    "Ldoor_floor:\n"
    "  .byte 0x8a,0x46,0x1a,0x84,0xc0,0x75,0x0b,0xc6,0x46,0x15,0xff\n"
    "  call _h_never\n"
    "  .byte 0xeb,0x10\n"
    "  call _fake_door_floor\n"
    "  .byte 0x85,0xc0,0x0f,0x95,0xc2,0x80,0xc2,0x7d,0x88,0x56,0x15\n"
    "  .byte 0x8a,0x46,0x15,0x84,0xc0,0x0f,0x8c\n"
    "  .long Ldoor_out - . - 4\n"
    "  .byte 0xa1\n"
    "  .long _g_house_ptr\n"
    "  .byte 0x8b,0x40,0x38,0x85,0xc0,0x74,0x1a,0xc7,0x80,0x80,0x08,0x00,0x00\n"
    "  .long _fake_door_opened\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_house_ptr\n"
    "  .byte 0x6a,0x00,0x8b,0x49,0x38\n"
    "  call @h_door_open@12\n"
    "  .byte 0x88,0x1d\n"
    "  .long _g_door_exiting\n"
    "  .byte 0x88,0x1d\n"
    "  .long _g_door_exiting + 1\n"
    "  .byte 0x88,0x1d\n"
    "  .long _g_door_exiting + 2\n"
    "  .byte 0x5e,0x5b,0xc2,0x08,0x00\n"
    "Ldoor_escape:\n"
    "  .byte 0x8a,0x46,0x18,0x84,0xc0,0x74,0x15,0x8b,0x4e,0x08,0x6a,0x00,0x6a,0x04,0xc6,0x46,0x18,0x00\n"
    "  call _fake_set_cursor\n"
    "  .byte 0x5e,0x5b,0xc2,0x08,0x00\n"
    "  .byte 0x8b,0x56,0x08,0x6a,0x01,0x52,0xb9\n"
    "  .long _g_mcb\n"
    "  call _h_close_menu@8\n"
    "  .byte 0x5e,0x5b,0xc2,0x08,0x00\n"
    "Ldoor_up:\n"
    "  .byte 0x8a,0x46,0x18,0x84,0xc0,0x74,0x06,0x0f,0xbe,0x46,0x16,0xeb,0x05,0xb8,0x04,0x00,0x00,0x00\n"
    "  .byte 0x0f,0xbf,0x4c,0x24,0x10,0x3b,0xc8,0x7e,0x41,0x8b,0x4e,0x08,0x6a,0x00,0x50\n"
    "  call _fake_set_cursor\n"
    "  .byte 0x8b,0x15\n"
    "  .long _g_never_data\n"
    "  .byte 0x5e,0x5b,0xc6,0x42,0x4c,0x00,0xc2,0x08,0x00\n"
    "Ldoor_down:\n"
    "  .byte 0x8a,0x46,0x18,0x84,0xc0,0x74,0x06,0x0f,0xbe,0x46,0x16,0xeb,0x05,0xb8,0x04,0x00,0x00,0x00\n"
    "  .byte 0x0f,0xbf,0x4c,0x24,0x10,0x3b,0xc8,0x7e,0x0c,0x8b,0x4e,0x08,0x6a,0x00,0x6a,0x01\n"
    "  call _fake_set_cursor\n"
    "Ldoor_out:\n"
    "  .byte 0x5e,0x5b,0xc2,0x08,0x00\n"
    "  .p2align 2\n"
    "Ldoor_codes:\n"
    "  .long Ldoor_down, Ldoor_up, Ldoor_out, Ldoor_out, Ldoor_confirm, Ldoor_escape, Ldoor_out, Ldoor_out\n"
    "  .long Ldoor_up\n"
    "Ldoor_rows:\n"
    "  .long Ldoor_stay, Ldoor_back, Ldoor_floor, Ldoor_list\n"
    "  .fill 0x40, 1, 0xcc\n"

    // The floor read: the word at +0x30 of the player's data.
    ".p2align 4\n"
    ".globl _fake_door_floor\n"
    "_fake_door_floor:\n"
    "  .byte 0xa1\n"
    "  .long _g_player_data_ptr\n"
    "  .byte 0x85,0xc0,0x75,0x01,0xc3,0x33,0xc9,0x66,0x8b,0x88,0x30,0x00,0x00,0x00,0x8b,0xc1,0xc3\n"

    // The routine an exit gives the door: the player's actor, then mogdoor's
    // +0x15 sent.
    ".p2align 4\n"
    ".globl _fake_door_opened\n"
    "_fake_door_opened:\n"
    "  .byte 0x0f,0xbf,0x05\n"
    "  .long _g_macro_player\n"
    "  .byte 0x8b,0x0c,0x85\n"
    "  .long _g_macro_table\n"
    "  .byte 0x85,0xc9,0x74,0x0f,0x8b,0x15\n"
    "  .long _g_ctl_global + 4 * 108\n"
    "  .byte 0x8a,0x42,0x15,0x50\n"
    "  call _h_never\n"
    "  .byte 0xc3\n"

    // The door's handler from its call before the open on.
    ".p2align 4\n"
    ".globl _fake_door_handler\n"
    "_fake_door_handler:\n"
    "  call _h_never\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_ctl_global + 4 * 108\n"
    "  .byte 0x6a,0x00,0x6a,0x01,0x68\n"
    "  .long _g_key_mogdoor\n"
    "  .byte 0x88,0x41,0x14,0x88,0x59,0x1b,0xb9\n"
    "  .long _g_mcb\n"
    "  call _fake_open\n"
    "  .byte 0x32,0xc0,0x5b,0xc3\n"

    // mogdoor's open hook (this = the controller): the nation's list from the
    // table, its count, the top shown; a nation out of range closes it.
    ".p2align 4\n"
    ".globl _fake_door_open_hook\n"
    "_fake_door_open_hook:\n"
    "  .byte 0xa1\n"
    "  .long _g_player_data_ptr\n"
    "  .byte 0x33,0xd2,0x3b,0xc2,0x74,0x4f,0x8a,0x41,0x14,0x3a,0xc2,0x7e,0x38,0x3c,0x0a,0x7f,0x34,0x0f,0xbe,0xc0\n"
    "  .byte 0x8b,0x04,0x85\n"
    "  .long _g_door_table\n"
    "  .byte 0x88,0x51,0x16,0x89,0x41,0x1c,0x66,0x39,0x10,0x74,0x15,0x53,0x56,0x8a,0x59,0x16,0xfe,0xc3,0x0f,0xbe\n"
    "  .byte 0xf3,0x88,0x59,0x16,0x66,0x39,0x14,0x70,0x75,0xef,0x5e,0x5b,0x88,0x51,0x18,0x88,0x51,0x19,0x88,0x51\n"
    "  .byte 0x1a,0xc3,0x8b,0x49,0x08,0x6a,0x01,0x51,0xb9\n"
    "  .long _g_mcb\n"
    "  call _h_close_menu@8\n"
    "  .byte 0xc3\n"
    "  .fill 0x60, 1, 0xcc\n"

    // Its tick (this = the controller; ret 4): the Mog Garden's row and the
    // floor row enabled as the stand-ins say.
    ".p2align 4\n"
    ".globl _fake_door_tick\n"
    "_fake_door_tick:\n"
    "  .byte 0x51,0x53,0x56,0x8b,0xf1\n"
    "  call _h_door_garden\n"
    "  .byte 0x88,0x46,0x19\n"
    "  call _h_door_floor_on\n"
    "  .byte 0x84,0xc0,0x74,0x05,0x88,0x46,0x1a,0xeb,0x05,0x33,0xc0,0x88,0x46,0x1a,0x5e,0x5b,0x59,0xc2,0x04,0x00\n"
    "  .fill 0x90, 1, 0xcc\n"

    // Its draw: the condition, then each text through the message getter.
    ".p2align 4\n"
    ".globl _fake_door_draw\n"
    "_fake_door_draw:\n"
    "  call _h_door_condition\n"
    "  .byte 0x8b,0x4e,0x08,0x8a,0xd8\n"
    "  .byte 0x55,0x6a,0x45,0x6a,0x09\n"
    "  call _h_message\n"
    "  .byte 0x55,0x68,0xb8,0x00,0x00,0x00,0x6a,0x09\n"
    "  call _h_message\n"
    "  .byte 0x84,0xdb,0xb8,0x46,0x00,0x00,0x00,0x75,0x05,0xb8,0xf1,0x00,0x00,0x00\n"
    "  .byte 0x55,0x68,0x25,0x01,0x00,0x00,0x6a,0x09\n"
    "  call _h_message\n"
    "  .byte 0x55,0x68,0xb9,0x00,0x00,0x00,0x6a,0x09\n"
    "  call _h_message\n"
    "  .byte 0x51,0x6a,0x14\n"
    "  call _h_message\n"
    "  .byte 0xc3\n"
    "  .fill 0x290, 1, 0xcc\n"

    // The dialogue boxes' input routine (this = the controller; code, row;
    // ret 8): the handler, again for the controllers in rem4line's and
    // rem4li2's registry slots.
    ".p2align 4\n"
    ".globl _fake_dialogue_input\n"
    "_fake_dialogue_input:\n"
    "  .byte 0x53,0x8b,0x5c,0x24,0x08,0x56,0x57,0x8b,0x7c,0x24,0x14,0x8b,0xf1,0x57,0x53\n"
    "  call _fake_dialogue_handler\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_ctl_global + 4 * 35\n"
    "  .byte 0x3b,0xf1,0x74,0x0a,0x8b,0x0d\n"
    "  .long _g_ctl_global + 4 * 36\n"
    "  .byte 0x3b,0xf1,0x75,0x07,0x57,0x53\n"
    "  call _fake_dialogue_handler\n"
    "  .byte 0x5f,0x5e,0x5b,0xc2,0x08,0x00\n"

    // The handler: the code to the text dispatch, then the box's cancel.
    ".p2align 4\n"
    "_fake_dialogue_handler:\n"
    "  .byte 0x51,0x56,0x8b,0xf1,0x57,0x8b,0x46,0x24,0x83,0xf8,0x04,0x7e,0x2f,0x8b,0x46,0x58,0x33,0xff,0x89,0x44\n"
    "  .byte 0x24,0x08,0x8b,0x44,0x24,0x08,0x85,0xc0,0x74,0x1e,0x8d,0x4c,0x24,0x08,0x51,0x8d,0x4e,0x14\n"
    "  call _h_never\n"
    "  .byte 0x47,0x83,0xff,0x04,0x7c,0xe5,0x8b,0x44,0x24,0x08,0x85,0xc0,0x74,0x03,0x89,0x46,0x58,0x8b,0x7c,0x24\n"
    "  .byte 0x10,0x8b,0x4e,0x68,0x57\n"
    "  call _fake_text_dispatch\n"
    "  .byte 0x80,0x3d\n"
    "  .long _g_query_cancel_allowed\n"
    "  .byte 0x01,0x75,0x39,0x66,0x83,0xff,0x06,0x75,0x33\n"
    "  call _h_never\n"
    "  .byte 0x8b,0x46,0x64,0x8b,0x56,0x50,0x52,0x8b,0x48,0x18\n"
    "  call @h_log_release@12\n"
    "  .byte 0x8b,0x4e,0x68\n"
    "  call _fake_text_clear\n"
    "  .byte 0xc6,0x05\n"
    "  .long _g_dialogue_wait\n"
    "  .byte 0x02,0x8b,0x4e,0x08,0x6a,0x01,0x51,0xb9\n"
    "  .long _g_mcb\n"
    "  call _h_close_menu@8\n"
    "  .byte 0x5f,0x5e,0x59,0xc2,0x08,0x00\n"
    "  .fill 0x90, 1, 0xcc\n"

    // The text dispatch (this = a text object; the code; ret 4), which hands
    // the code to its gate on the text object, then on the other box's, as
    // the game's does through the two globals naming them; and the gate (the
    // same), which on a page that waits for a key and code 5 calls the
    // advance, then the page-turn sound, and at its end records the code.
    ".p2align 4\n"
    "_fake_text_dispatch:\n"
    "  .byte 0x56,0x57,0x8b,0x7c,0x24,0x0c,0x8b,0xf1,0x57\n"
    "  call _fake_text_gate\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_text_objects\n"
    "  .byte 0x3b,0xf1,0x75,0x11,0x8b,0x0d\n"
    "  .long _g_text_objects + 4\n"
    "  .byte 0x57\n"
    "  call _fake_text_gate\n"
    "  .byte 0x5f,0x5e,0xc2,0x04,0x00,0x3b,0x35\n"
    "  .long _g_text_objects + 4\n"
    "  .byte 0x75,0x06,0x57\n"
    "  call _fake_text_gate\n"
    "  .byte 0x5f,0x5e,0xc2,0x04,0x00\n"
    ".p2align 4\n"
    "_fake_text_gate:\n"
    "  .byte 0xf6,0x41,0x20,0x01,0x74,0x12,0x66,0x83,0x7c,0x24,0x04,0x05,0x75,0x0a\n"
    "  call _fake_text_advance\n"
    "  call _fake_page_sound\n"
    "  jmp _h_dispatch_code@4\n"

    // The advance (this = the text object): the stand-in, then the box's
    // show as the game's calls it, never reached.
    ".p2align 4\n"
    "_fake_text_advance:\n"
    "  jmp @h_text_advance@8\n"
    "  .byte 0x8b,0x4e,0x30,0x50\n"
    "  call _fake_dialogue_show\n"
    "  .byte 0xc3\n"
    "  .fill 0xc0, 1, 0xcc\n"

    // The page-turn sound: its name pushed, then the stand-in.
    ".p2align 4\n"
    "_fake_page_sound:\n"
    "  .byte 0x68,0x30,0x30,0x31,0x35\n"
    "  call _h_page_sound\n"
    "  .byte 0x59,0xc3\n"
    ".p2align 4\n"
    "_fake_dialogue_show:\n"
    "  .byte 0x66,0xc7,0x43,0x4e,0x00,0x00,0x66,0xff,0x43,0x4e,0x66,0x89,0x85,0x08,0x02,0x00,0x00\n"
    "  .byte 0x8d,0x55,0x04,0x68,0x01,0x01,0x00,0x00,0x8d,0x4b,0x14\n"
    "  call _h_never\n"
    "  .byte 0xc2,0x04,0x00\n"
    "  .fill 0x180, 1, 0xcc\n"

    // The text object's clear (this = it): its head, then the stand-in.
    ".p2align 4\n"
    "_fake_text_clear:\n"
    "  .byte 0x51,0x53,0x56,0x8b,0xf1,0x33,0xdb,0x8b,0x46,0x1c,0x3b,0xc3,0x5e,0x5b,0x59\n"
    "  jmp @h_text_clear@8\n"

    // The boxes' tick (this = the controller; ret 4): the left button
    // through the key test, as the game's asks it; its read of the script's
    // own confirm and its timer, never run.
    ".p2align 4\n"
    ".globl _fake_dialogue_tick\n"
    "_fake_dialogue_tick:\n"
    "  .byte 0x53,0x56,0x57,0x8b,0xf9,0x6a,0xff,0x6a,0x05,0x68,0x8b,0x00,0x00,0x00,0x6a,0x3e\n"
    "  call _fake_key_test\n"
    "  .byte 0x83,0xc4,0x10,0x3c,0x01,0x75,0x06,0x57\n"
    "  call _h_dialogue_click@4\n"
    "  .byte 0x5f,0x5e,0x5b,0xc2,0x04,0x00\n"
    "  .byte 0x66,0x39,0x6f,0x60,0x8b,0x4f,0x68\n"
    "  call _fake_text_timer\n"
    "  .byte 0x84,0xc0,0x74,0x08,0x8b,0x4f,0x68\n"
    "  call _fake_text_last_page\n"
    "  .byte 0xc3\n"
    "  .fill 0x180, 1, 0xcc\n"
    ".p2align 4\n"
    "_fake_text_timer:\n"
    "  .byte 0x56,0x8b,0xf1,0xf6,0x46,0x20,0x02,0x74,0x35\n"
    "  call _h_never\n"
    "  .byte 0x90,0x90,0x90,0x90,0x90,0x8b,0x4e,0x28,0x99,0x2b,0xc8,0x89,0x4e,0x28,0x8b,0x4e,0x2c,0x1b,0xca\n"
    "  .byte 0x5e,0xc3\n"
    ".p2align 4\n"
    "_fake_text_last_page:\n"
    "  .byte 0x8b,0x41,0x1c,0x85,0xc0,0x75,0x03,0xb0,0x01,0xc3,0x8b,0x80,0x28,0x08,0x00,0x00,0x85,0xc0,0x75,0x03\n"
    "  .byte 0xb0,0x01,0xc3,0x80,0x38,0x00,0x0f,0x94,0xc0,0xc3\n"
    ".p2align 4\n"
    "_fake_bare_ret:\n"
    "  .byte 0xc3\n"

    ".section .rdata,\"dr\"\n"
    ".p2align 2\n"
    ".globl _g_door_vtable\n"
    "_g_door_vtable:\n"
    "  .long 0, _fake_door_open_hook, 0, 0, _fake_door_draw, _fake_bare_ret, _fake_door_input, 0, 0, 0\n"
    "  .long _fake_door_tick\n"
    "  .fill 21, 4, 0\n"
    ".globl _g_dialogue_vtables\n"
    "_g_dialogue_vtables:\n"
    "  .long 0, 0, 0, 0, 0, _fake_bare_ret, _fake_dialogue_input, 0, 0, 0, _fake_dialogue_tick\n"
    "  .fill 21, 4, 0\n"
    "  .long 0, 0, 0, 0, 0, _fake_bare_ret, _fake_dialogue_input, 0, 0, 0, _fake_dialogue_tick\n"
    "  .fill 21, 4, 0\n"
    ".text\n"
);

// The text-entry windows, the game's bytes where the engine reads or runs
// them: stringdl's input routine with its handlers and its field's end;
// comment's, msgline's, scname's, link11's and passinpu's input routines,
// never run, and their fields' ends, run by the engine; scname's, link11's
// and passinpu's begin-closes and passinpu's open hook, run by the open and
// the close. comment's begin-close is the game's head, its field ended,
// then a stand-in that records what it sends; msgline's open hook and the
// start of its field are the bytes the engine reads around a stand-in that
// starts the field, its begin-close a stand-in; comment's open hook and
// start the bytes the engine reads around a stand-in that starts the
// field. Every call outside these goes to the stand-ins above or to
// h_never.
asm(
    ".text\n"
    // stringdl's input routine, its start and end of the field, its handlers for
    // the name and for a slot: the game's bytes, the end run by the engine.
    ".p2align 4\n"
    ".globl _fake_stringdl_input\n"
    "_fake_stringdl_input:\n"
    "  .byte 0x56,0x8b,0xf1,0x0f,0xbe,0x46,0x14,0x48,0x74,0x16,0x48,0x75,0x24,0x8b,0x44,0x24,0x0c,0x8b,0x4c,0x24\n"
    "  .byte 0x08,0x50,0x51,0x8b,0xce,0xe8,0x82,0x02,0x00,0x00,0xeb,0x11,0x8b,0x54,0x24,0x0c,0x8b,0x44,0x24,0x08\n"
    "  .byte 0x52,0x50,0x8b,0xce,0xe8,0x8f,0x00,0x00,0x00,0x66,0x83,0xbe,0x5c,0x01,0x00,0x00,0x00,0x5e,0x74,0x23\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_ctl_global + 4 * 54\n"
    "  .byte 0x8b,0x41,0x08,0x85,0xc0,0x74,0x16,0xc7,0x44,0x24,0x08,0x00,0x00,0x00,0x00,0x89,0x44,0x24,0x04,0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe9\n"
    "  .long @h_focus@16 - . - 4\n"
    "  .byte 0xc2,0x08,0x00,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x56,0x8b,0xf1\n"
    "  .byte 0xc6,0x86,0x64,0x01,0x00,0x00,0x01,0x8b,0x0d\n"
    "  .long _g_edit_ptr\n"
    "  .byte 0xe8\n"
    "  .long @h_edit_deactivate@8 - . - 4\n"
    "  .byte 0x8b,0x86,0x60,0x01,0x00,0x00,0x8b,0x0d\n"
    "  .long _g_edit_ptr\n"
    "  .byte 0x50,0xe8\n"
    "  .long @h_edit_attach@12 - . - 4\n"
    "  .byte 0x5e,0xc3,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0xc6,0x81,0x64,0x01,0x00,0x00,0x00,0x8b,0x0d\n"
    "  .long _g_edit_ptr\n"
    "  .byte 0xe8\n"
    "  .long @h_edit_deactivate@8 - . - 4\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_edit_ptr\n"
    "  .byte 0x6a,0x00,0xe8\n"
    "  .long @h_edit_attach@12 - . - 4\n"
    "  .byte 0xc3,0x0f,0xbf,0x44,0x24,0x04,0x48,0x56,0x83,0xf8,0x10,0x8b,0xf1,0x0f,0x87,0x65,0x01,0x00,0x00,0x33\n"
    "  .byte 0xc9,0x8a,0x88\n"
    "  .long _fake_stringdl_input + 0x288\n"
    "  .byte 0xff,0x24,0x8d\n"
    "  .long _fake_stringdl_input + 0x268\n"
    "  .byte 0x8a,0x86,0x64,0x01,0x00,0x00,0x84,0xc0,0x75,0x1b,0x8b,0x56,0x08,0x66,0x83,0x7a,0x5a,0x02,0x75,0x09\n"
    "  .byte 0x66,0xc7,0x86,0x5c,0x01,0x00,0x00,0x05,0x00,0x84,0xc0,0x0f,0x84,0x31,0x01,0x00,0x00,0x8b,0x4e,0x08\n"
    "  .byte 0x6a,0x00,0x6a,0x01,0xe8\n"
    "  .long _fake_set_cursor - . - 4\n"
    "  .byte 0xe9,0x20,0x01,0x00,0x00,0x8a,0x86,0x64,0x01,0x00,0x00,0x84,0xc0,0x0f,0x84,0x12,0x01,0x00,0x00,0x8b\n"
    "  .byte 0x4e,0x08,0x6a,0x00,0x6a,0x01,0xe8\n"
    "  .long _fake_set_cursor - . - 4\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_edit_ptr\n"
    "  .byte 0x6a,0x04,0xe8\n"
    "  .long _h_never - . - 4\n"
    "  .byte 0xe9,0xf4,0x00,0x00,0x00,0x8a,0x86,0x64,0x01,0x00,0x00,0x84,0xc0,0x0f,0x84,0xe6,0x00,0x00,0x00,0x8b\n"
    "  .byte 0x4e,0x08,0x6a,0x00,0x6a,0x01,0xe8\n"
    "  .long _fake_set_cursor - . - 4\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_edit_ptr\n"
    "  .byte 0x6a,0x05,0xe8\n"
    "  .long _h_never - . - 4\n"
    "  .byte 0xe9,0xc8,0x00,0x00,0x00,0x8b,0x46,0x08,0x66,0x83,0x78,0x4c,0x01,0x0f,0x85,0xba,0x00,0x00,0x00,0x8a\n"
    "  .byte 0x86,0x64,0x01,0x00,0x00,0x84,0xc0,0x0f,0x84,0xac,0x00,0x00,0x00,0x8b,0xce,0xe8,0x0e,0xff,0xff,0xff\n"
    "  .byte 0xe9,0xa0,0x00,0x00,0x00,0x8b,0x4e,0x08,0x0f,0xbf,0x41,0x4c,0x48,0x74,0x53,0x48,0x74,0x1a,0x48,0x0f\n"
    "  .byte 0x85,0x8c,0x00,0x00,0x00,0x8a,0x86,0x64,0x01,0x00,0x00,0x84,0xc0,0x74,0x79,0x8b,0xce,0xe8,0xe4,0xfe\n"
    "  .byte 0xff,0xff,0xeb,0x70,0x8a,0x86,0x64,0x01,0x00,0x00,0x84,0xc0,0x74,0x07,0x8b,0xce,0xe8,0xd1,0xfe,0xff\n"
    "  .byte 0xff,0x8b,0x8e,0x60,0x01,0x00,0x00,0x6a,0x00,0xe8\n"
    "  .long @h_field_read@12 - . - 4\n"
    "  .byte 0x8d,0x56,0x58,0x8a,0x08,0x40,0x88,0x0a,0x42,0x84,0xc9,0x75,0xf6,0x66,0xc7,0x86,0x5c,0x01,0x00,0x00\n"
    "  .byte 0x01,0x00,0xeb,0x43,0x8a,0x86,0x64,0x01,0x00,0x00,0x84,0xc0,0x74,0x0f,0x8b,0x0d\n"
    "  .long _g_edit_ptr\n"
    "  .byte 0x6a,0x01,0xe8\n"
    "  .long _h_never - . - 4\n"
    "  .byte 0xeb,0x2a,0x66,0x83,0xbe,0x5c,0x01,0x00,0x00,0x00,0x75,0x28,0x8b,0xce,0xe8,0x52,0xfe,0xff,0xff,0xeb\n"
    "  .byte 0x17,0x8a,0x86,0x64,0x01,0x00,0x00,0x84,0xc0,0x0f,0x85,0x5d,0xff,0xff,0xff,0x66,0xc7,0x86,0x5c,0x01\n"
    "  .byte 0x00,0x00,0x02,0x00,0x66,0x83,0xbe,0x5c,0x01,0x00,0x00,0x00,0x5e,0x74,0x23,0x8b,0x15\n"
    "  .long _g_ctl_global + 4 * 54\n"
    "  .byte 0x8b,0x42,0x08,0x85,0xc0,0x74,0x16,0xc7,0x44,0x24,0x08,0x00,0x00,0x00,0x00,0x89,0x44,0x24,0x04,0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe9\n"
    "  .long @h_focus@16 - . - 4\n"
    "  .byte 0xc2,0x08,0x00\n"
    "  .long _fake_stringdl_input + 0x17d\n"
    "  .long _fake_stringdl_input + 0xe1\n"
    "  .long _fake_stringdl_input + 0x143\n"
    "  .long _fake_stringdl_input + 0x117\n"
    "  .long _fake_stringdl_input + 0x197\n"
    "  .long _fake_stringdl_input + 0x220\n"
    "  .long _fake_stringdl_input + 0x16f\n"
    "  .long _fake_stringdl_input + 0x237\n"
    "  .byte 0x00,0x01,0x02,0x03,0x04,0x05,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x07,0x06,0x90,0x90,0x90\n"
    "  .byte 0x90,0x90,0x90,0x90,0x0f,0xbf,0x44,0x24,0x04,0x48,0x83,0xf8,0x05,0x77,0x42,0xff,0x24,0x85\n"
    "  .long _fake_stringdl_input + 0x304\n"
    "  .byte 0x8a,0x81,0x64,0x01,0x00,0x00,0x84,0xc0,0x74,0x31,0x8b,0x49,0x08,0xc7,0x44,0x24,0x08,0x00,0x00,0x00\n"
    "  .byte 0x00,0xc7,0x44,0x24,0x04,0x01,0x00,0x00,0x00,0xe9\n"
    "  .long _fake_set_cursor - . - 4\n"
    "  .byte 0x8b,0x41,0x08,0x0f,0xbf,0x40,0x4c,0x48,0x74,0x1e,0x48,0x74,0x0f,0x48,0x75,0x09,0x66,0xc7,0x81,0x5c\n"
    "  .byte 0x01,0x00,0x00,0x04,0x00,0xc2,0x08,0x00,0x66,0xc7,0x81,0x5c,0x01,0x00,0x00,0x03,0x00,0xc2,0x08,0x00\n"
    "  .byte 0xe8\n"
    "  .long _h_never - . - 4\n"
    "  .byte 0xc2,0x08,0x00\n"
    "  .long _fake_stringdl_input + 0x2b2\n"
    "  .long _fake_stringdl_input + 0x2b2\n"
    "  .long _fake_stringdl_input + 0x2b2\n"
    "  .long _fake_stringdl_input + 0x2b2\n"
    "  .long _fake_stringdl_input + 0x2d4\n"
    "  .long _fake_stringdl_input + 0x2e4\n"
    // comment's field end (this = the controller; keep; ret 4), run by the engine,
    // and its input routine, never run: the game's bytes.
    ".p2align 4\n"
    ".globl _fake_comment_stop\n"
    "_fake_comment_stop:\n"
    "  .byte 0x8a,0x44,0x24,0x04,0x56,0x84,0xc0,0x8b,0xf1,0x0f,0x84,0x93,0x00,0x00,0x00,0x8a,0x46,0x14,0x84,0xc0\n"
    "  .byte 0x0f,0x8c,0x88,0x00,0x00,0x00,0x3c,0x03,0x0f,0x8d,0x80,0x00,0x00,0x00,0x8b,0x8e,0x78,0x03,0x00,0x00\n"
    "  .byte 0x6a,0x28,0x6a,0x00,0xe8\n"
    "  .long @h_field_read@12 - . - 4\n"
    "  .byte 0x0f,0xbe,0x4e,0x14,0x50,0x0f,0xbe,0x46,0x16,0x8d,0x04,0x40,0x03,0xc1,0x8d,0x14,0x80,0x03,0xc6,0x8d\n"
    "  .byte 0x4c,0xd0,0x1a,0x51,0xe8\n"
    "  .long _h_strncpy - . - 4\n"
    "  .byte 0x0f,0xbe,0x46,0x16,0x0f,0xbe,0x56,0x14,0x8d,0x04,0x40,0x03,0xc2,0x8d,0x0c,0x80,0x8d,0x14,0x06,0xc6\n"
    "  .byte 0x44,0xca,0x42,0x00,0x0f,0xbe,0x46,0x16,0x0f,0xbe,0x4e,0x14,0x8d,0x04,0x40,0x03,0xc1,0x8d,0x14,0x80\n"
    "  .byte 0x03,0xc6,0x8d,0x44,0xd0,0x1a,0x50,0x50,0xe8\n"
    "  .long _h_sanitize - . - 4\n"
    "  .byte 0x0f,0xbe,0x46,0x16,0x0f,0xbe,0x4e,0x14,0x8d,0x04,0x40,0x03,0xc1,0x8d,0x14,0x80,0x03,0xc6,0x8d,0x4c\n"
    "  .byte 0xd0,0x1a,0x51,0xe8\n"
    "  .long _h_trim - . - 4\n"
    "  .byte 0x83,0xc4,0x18,0x8b,0x8e,0x78,0x03,0x00,0x00,0x85,0xc9,0x74,0x11,0x8b,0x11,0x6a,0x01,0xff,0x52,0x0c\n"
    "  .byte 0xc7,0x86,0x78,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x8b,0x0d\n"
    "  .long _g_edit_ptr\n"
    "  .byte 0x6a,0x00,0xe8\n"
    "  .long @h_edit_attach@12 - . - 4\n"
    "  .byte 0x8b,0x46,0x08,0xc6,0x46,0x14,0xff,0x85,0xc0,0x74,0x0b,0xc6,0x40,0x61,0x01,0x8b,0x46,0x08,0xc6,0x40\n"
    "  .byte 0x62,0x01,0x5e,0xc2,0x04,0x00\n"
    ".p2align 4\n"
    ".globl _fake_comment_input\n"
    "_fake_comment_input:\n"
    "  .byte 0x0f,0xbf,0x44,0x24,0x04,0x48,0x56,0x83,0xf8,0x11,0x8b,0xf1,0x0f,0x87,0xd3,0x00,0x00,0x00,0x33,0xc9\n"
    "  .byte 0x8a,0x88\n"
    "  .long _fake_comment_input + 0x110\n"
    "  .byte 0xff,0x24,0x8d\n"
    "  .long _fake_comment_input + 0xec\n"
    "  .byte 0x6a,0x01,0x8b,0xce,0xe8\n"
    "  .long _fake_comment_stop - . - 4\n"
    "  .byte 0x8a,0x46,0x15,0xfe,0xc8,0x88,0x46,0x15,0x79,0x04,0xc6,0x46,0x15,0x00,0x0f,0xbe,0x56,0x15,0x52,0x8b\n"
    "  .byte 0xce,0xe8\n"
    "  .long _h_never - . - 4\n"
    "  .byte 0x5e,0xc2,0x08,0x00,0x6a,0x01,0x8b,0xce,0xe8\n"
    "  .long _fake_comment_stop - . - 4\n"
    "  .byte 0x8a,0x4e,0x15,0xfe,0xc1,0x8a,0xc1,0x88,0x4e,0x15,0x3c,0x03,0x7c,0x04,0xc6,0x46,0x15,0x02,0x0f,0xbe\n"
    "  .byte 0x46,0x15,0x50,0x8b,0xce,0xe8\n"
    "  .long _h_never - . - 4\n"
    "  .byte 0x5e,0xc2,0x08,0x00,0x6a,0x01,0x8b,0xce,0xe8\n"
    "  .long _fake_comment_stop - . - 4\n"
    "  .byte 0x8b,0x4e,0x08,0x6a,0x01,0x51,0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe8\n"
    "  .long _h_close_menu@8 - . - 4\n"
    "  .byte 0x5e,0xc2,0x08,0x00,0x8b,0x0d\n"
    "  .long _g_edit_ptr\n"
    "  .byte 0x6a,0x01,0xe8\n"
    "  .long _h_never - . - 4\n"
    "  .byte 0x5e,0xc2,0x08,0x00,0x8b,0x0d\n"
    "  .long _g_edit_ptr\n"
    "  .byte 0x6a,0x04,0xe8\n"
    "  .long _h_never - . - 4\n"
    "  .byte 0x5e,0xc2,0x08,0x00,0x8b,0x0d\n"
    "  .long _g_edit_ptr\n"
    "  .byte 0x6a,0x05,0xe8\n"
    "  .long _h_never - . - 4\n"
    "  .byte 0x5e,0xc2,0x08,0x00,0x8b,0x0d\n"
    "  .long _g_edit_ptr\n"
    "  .byte 0x6a,0x03,0xe8\n"
    "  .long _h_never - . - 4\n"
    "  .byte 0x5e,0xc2,0x08,0x00,0x8b,0x0d\n"
    "  .long _g_never_data\n"
    "  .byte 0x6a,0x00,0x6a,0x00,0x6a,0x02,0x8b,0x11,0xff,0x52,0x08,0x5e,0xc2,0x08,0x00,0x8d,0x49,0x00\n"
    "  .long _fake_comment_input + 0x48\n"
    "  .long _fake_comment_input + 0x21\n"
    "  .long _fake_comment_input + 0xb2\n"
    "  .long _fake_comment_input + 0xa1\n"
    "  .long _fake_comment_input + 0x90\n"
    "  .long _fake_comment_input + 0x73\n"
    "  .long _fake_comment_input + 0xc3\n"
    "  .long _fake_comment_input + 0xd4\n"
    "  .long _fake_comment_input + 0xe5\n"
    "  .byte 0x00,0x01,0x02,0x03,0x04,0x05,0x08,0x06,0x08,0x08,0x08,0x06,0x08,0x08,0x08,0x08,0x00,0x07\n"
    // msgline's field end, run by the engine, and its input routine, never run:
    // the game's bytes around the stand-ins' senders.
    ".p2align 4\n"
    ".globl _fake_msgline_stop\n"
    "_fake_msgline_stop:\n"
    "  .byte 0x8a,0x44,0x24,0x04,0x56,0x84,0xc0,0x8b,0xf1,0x74,0x23,0x8b,0x4e,0x1c,0x68,0x2c,0x01,0x00,0x00,0x6a\n"
    "  .byte 0x01,0xe8\n"
    "  .long @h_field_read@12 - . - 4\n"
    "  .byte 0x50,0x8d,0x46,0x53,0x50,0xe8\n"
    "  .long _h_strncpy - . - 4\n"
    "  .byte 0x83,0xc4,0x0c,0xc6,0x86,0x7e,0x01,0x00,0x00,0x00,0x8b,0x4e,0x1c,0x85,0xc9,0x74,0x0e,0x8b,0x11,0x6a\n"
    "  .byte 0x01,0xff,0x52,0x0c,0xc7,0x46,0x1c,0x00,0x00,0x00,0x00,0x8b,0x0d\n"
    "  .long _g_edit_ptr\n"
    "  .byte 0x6a,0x00,0xe8\n"
    "  .long @h_edit_attach@12 - . - 4\n"
    "  .byte 0x8b,0x76,0x08,0x85,0xf6,0x74,0x04,0xc6,0x46,0x62,0x01,0x5e,0xc2,0x04,0x00\n"
    ".p2align 4\n"
    ".globl _fake_msgline_input\n"
    "_fake_msgline_input:\n"
    "  .byte 0x0f,0xbf,0x44,0x24,0x04,0x81,0xec,0x44,0x01,0x00,0x00,0x83,0xc0,0xfd,0x83,0xf8,0x0f,0x53,0x56,0x57\n"
    "  .byte 0x8b,0xf1,0x0f,0x87,0x3e,0x04,0x00,0x00,0x33,0xc9,0x8a,0x88\n"
    "  .long _fake_msgline_input + 0x488\n"
    "  .byte 0xff,0x24,0x8d\n"
    "  .long _fake_msgline_input + 0x468\n"
    "  .byte 0x6a,0x01,0x8b,0xce,0xe8\n"
    "  .long _fake_msgline_stop - . - 4\n"
    "  .byte 0x8d,0x5e,0x53,0x83,0xc9,0xff,0x8b,0xfb,0x33,0xc0,0xf2,0xae,0xf7,0xd1,0x49,0x0f,0x84,0x9a,0x03,0x00\n"
    "  .byte 0x00,0xa1\n"
    "  .long _g_player_data_ptr\n"
    "  .byte 0x85,0xc0,0x0f,0x84,0x8d,0x03,0x00,0x00,0x0f,0xbf,0x46,0x14,0x83,0xf8,0x07,0x0f,0x87,0x21,0x03,0x00\n"
    "  .byte 0x00,0xff,0x24,0x85\n"
    "  .long _fake_msgline_input + 0x498\n"
    "  .byte 0x8b,0x15\n"
    "  .long _g_pol_ptr\n"
    "  .byte 0x8b,0xbe,0x9c,0x01,0x00,0x00,0x55,0x6a,0x11,0x8b,0xaa,0x80,0x00,0x00,0x00,0x68\n"
    "  .long _h_msgline_sent1\n"
    "  .byte 0x53,0x6a,0x1c,0x6a,0x09,0xe8\n"
    "  .long _h_message - . - 4\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_pol_ptr\n"
    "  .byte 0x83,0xc4,0x08,0x50,0x8d,0x86,0x88,0x01,0x00,0x00,0x50,0x55,0xe8\n"
    "  .long @h_pol_number@12 - . - 4\n"
    "  .byte 0x33,0xc9,0x8b,0xd7,0x8a,0xe8,0x8b,0x86,0x98,0x01,0x00,0x00,0x81,0xe2,0x00,0x00,0x0f,0x00,0x81,0xe7\n"
    "  .byte 0xff,0xff,0x00,0x00,0x0b,0xca,0xc1,0xe1,0x08,0x0b,0xcf,0x51,0x50,0xe8\n"
    "  .long _h_send_tell - . - 4\n"
    "  .byte 0x83,0xc4,0x1c,0x5d,0xe9,0xb4,0x02,0x00,0x00,0x6a,0x11,0x68\n"
    "  .long _h_msgline_sent1\n"
    "  .byte 0x53,0x6a,0x1c,0x6a,0x09,0xe8\n"
    "  .long _h_message - . - 4\n"
    "  .byte 0x8b,0x8e,0xd8,0x01,0x00,0x00,0x8b,0x96,0xd4,0x01,0x00,0x00,0x83,0xc4,0x08,0x50,0x8b,0x86,0xd0,0x01\n"
    "  .byte 0x00,0x00,0x51,0x52,0x50,0xe8\n"
    "  .long _h_send_reply - . - 4\n"
    "  .byte 0x83,0xc4,0x1c,0xe9,0x7d,0x02,0x00,0x00,0x8b,0xcb,0x8a,0x01,0x84,0xc0,0x74,0x0f,0x3c,0x07,0x75,0x03\n"
    "  .byte 0xc6,0x01,0x00,0x8a,0x41,0x01,0x41,0x84,0xc0,0x75,0xf1,0x8d,0x4c,0x24,0x50,0x6a,0x7f,0x51,0x6a,0x01\n"
    "  .byte 0xe8\n"
    "  .long _h_clock_text - . - 4\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_pol_ptr\n"
    "  .byte 0x83,0xc4,0x0c,0x8b,0xbe,0x9c,0x01,0x00,0x00,0x8d,0x96,0x88,0x01,0x00,0x00,0x8b,0x81,0x80,0x00,0x00\n"
    "  .byte 0x00,0x6a,0x14,0x68\n"
    "  .long _h_msgline_sent1\n"
    "  .byte 0x52,0x8d,0x54,0x24,0x5c,0x52,0x53,0x50,0xe8\n"
    "  .long @h_pol_number@12 - . - 4\n"
    "  .byte 0x33,0xc9,0x8b,0xd7,0x8a,0xe8,0x8b,0x86,0x98,0x01,0x00,0x00,0x81,0xe2,0x00,0x00,0x0f,0x00,0x81,0xe7\n"
    "  .byte 0xff,0xff,0x00,0x00,0x0b,0xca,0xc1,0xe1,0x08,0x0b,0xcf,0x51,0x50,0xe8\n"
    "  .long _h_send_short - . - 4\n"
    "  .byte 0x83,0xc4,0x1c,0xe9,0xfb,0x01,0x00,0x00,0x8b,0x0d\n"
    "  .long _g_pol_ptr\n"
    "  .byte 0x8b,0x81,0x80,0x00,0x00,0x00,0x50,0xe8\n"
    "  .long @h_pol_number@12 - . - 4\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_pol_ptr\n"
    "  .byte 0x50,0xe8\n"
    "  .long _h_never - . - 4\n"
    "  .byte 0x85,0xc0,0x74,0x22,0x8b,0x0d\n"
    "  .long _g_player_data_ptr\n"
    "  .byte 0x50,0x81,0xc1,0x9c,0xc3,0x03,0x00,0x8d,0x54,0x24,0x14,0x51,0x68\n"
    "  .long _g_never_data\n"
    "  .byte 0x52,0xe8\n"
    "  .long _h_never - . - 4\n"
    "  .byte 0x83,0xc4,0x10,0xeb,0x1d,0xa1\n"
    "  .long _g_player_data_ptr\n"
    "  .byte 0x8d,0x4c,0x24,0x10,0x05,0x9c,0xc3,0x03,0x00,0x50,0x68\n"
    "  .long _g_never_data\n"
    "  .byte 0x51,0xe8\n"
    "  .long _h_never - . - 4\n"
    "  .byte 0x83,0xc4,0x0c,0x8b,0x96,0x04,0x02,0x00,0x00,0x8b,0x0d\n"
    "  .long _g_pol_ptr\n"
    "  .byte 0x6a,0x13,0x68\n"
    "  .long _h_msgline_sent1\n"
    "  .byte 0x8b,0x81,0x80,0x00,0x00,0x00,0x52,0x8b,0x96,0x00,0x02,0x00,0x00,0x8b,0xbe,0x9c,0x01,0x00,0x00,0x52\n"
    "  .byte 0x8d,0x54,0x24,0x20,0x53,0x52,0x6a,0x00,0x50,0xe8\n"
    "  .long @h_pol_number@12 - . - 4\n"
    "  .byte 0x33,0xc9,0x8b,0xd7,0x8a,0xe8,0x8b,0x86,0x98,0x01,0x00,0x00,0x81,0xe2,0x00,0x00,0x0f,0x00,0x81,0xe7\n"
    "  .byte 0xff,0xff,0x00,0x00,0x0b,0xca,0xc1,0xe1,0x08,0x0b,0xcf,0x51,0x50,0xe8\n"
    "  .long _h_never - . - 4\n"
    "  .byte 0x83,0xc4,0x24,0xe9,0x37,0x01,0x00,0x00,0x8b,0x0d\n"
    "  .long _g_pol_ptr\n"
    "  .byte 0x8b,0x81,0x80,0x00,0x00,0x00,0x50,0xe8\n"
    "  .long @h_pol_number@12 - . - 4\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_pol_ptr\n"
    "  .byte 0x50,0xe8\n"
    "  .long _h_never - . - 4\n"
    "  .byte 0x85,0xc0,0x74,0x22,0x8b,0x0d\n"
    "  .long _g_player_data_ptr\n"
    "  .byte 0x50,0x81,0xc1,0x9c,0xc3,0x03,0x00,0x8d,0x54,0x24,0x14,0x51,0x68\n"
    "  .long _g_never_data\n"
    "  .byte 0x52,0xe8\n"
    "  .long _h_never - . - 4\n"
    "  .byte 0x83,0xc4,0x10,0xeb,0x1d,0xa1\n"
    "  .long _g_player_data_ptr\n"
    "  .byte 0x8d,0x4c,0x24,0x10,0x05,0x9c,0xc3,0x03,0x00,0x50,0x68\n"
    "  .long _g_never_data\n"
    "  .byte 0x51,0xe8\n"
    "  .long _h_never - . - 4\n"
    "  .byte 0x83,0xc4,0x0c,0x8b,0x96,0x04,0x02,0x00,0x00,0x8b,0x86,0x00,0x02,0x00,0x00,0x6a,0x13,0x68\n"
    "  .long _h_msgline_sent1\n"
    "  .byte 0x52,0x8b,0x96,0xd8,0x01,0x00,0x00,0x50,0x8b,0x86,0xd4,0x01,0x00,0x00,0x8d,0x4c,0x24,0x20,0x53,0x51\n"
    "  .byte 0x8b,0x8e,0xd0,0x01,0x00,0x00,0x52,0x50,0x51,0xe8\n"
    "  .long _h_never - . - 4\n"
    "  .byte 0x83,0xc4,0x24,0xe9,0x99,0x00,0x00,0x00,0x6a,0x12,0x68\n"
    "  .long _h_msgline_sent2\n"
    "  .byte 0x53,0x6a,0x1f,0x6a,0x09,0xe8\n"
    "  .long _h_message - . - 4\n"
    "  .byte 0x8b,0x96,0xd4,0x01,0x00,0x00,0x8b,0x8e,0xe4,0x01,0x00,0x00,0x83,0xc4,0x08,0x50,0x8b,0x86,0xd0,0x01\n"
    "  .byte 0x00,0x00,0x52,0x8b,0x96,0xe0,0x01,0x00,0x00,0x50,0x51,0x52,0xe8\n"
    "  .long _h_send_answer - . - 4\n"
    "  .byte 0x83,0xc4,0x20,0xeb,0x5e,0x8d,0x44,0x24,0x50,0x6a,0x7f,0x50,0x6a,0x01,0xe8\n"
    "  .long _h_clock_text - . - 4\n"
    "  .byte 0x8d,0x8c,0x24,0xdc,0x00,0x00,0x00,0x6a,0x7f,0x51,0x6a,0x09,0xe8\n"
    "  .long _h_clock_text - . - 4\n"
    "  .byte 0x8b,0x8e,0xd4,0x01,0x00,0x00,0x6a,0x15,0x8d,0x54,0x24,0x6c,0x68\n"
    "  .long _h_msgline_sent2\n"
    "  .byte 0x8d,0x84,0x24,0xf0,0x00,0x00,0x00,0x52,0x8b,0x96,0xd0,0x01,0x00,0x00,0x50,0x8b,0x86,0xe4,0x01,0x00\n"
    "  .byte 0x00,0x53,0x51,0x8b,0x8e,0xe0,0x01,0x00,0x00,0x52,0x50,0x51,0xe8\n"
    "  .long _h_send_dated - . - 4\n"
    "  .byte 0x83,0xc4,0x3c,0xeb,0x04,0x8b,0x44,0x24,0x0c,0x85,0xc0,0x75,0x1a,0xa1\n"
    "  .long _g_sending_ptr\n"
    "  .byte 0x85,0xc0,0x74,0x0a,0x6a,0x10,0x8d,0x48,0x20,0xe8\n"
    "  .long @h_mark@12 - . - 4\n"
    "  .byte 0xc6,0x05\n"
    "  .long _g_msgline_busy\n"
    "  .byte 0x01,0x68\n"
    "  .long _g_key_msgline\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe8\n"
    "  .long _fake_close - . - 4\n"
    "  .byte 0x5f,0x5e,0x5b,0x81,0xc4,0x44,0x01,0x00,0x00,0xc2,0x08,0x00,0x8b,0x0d\n"
    "  .long _g_edit_ptr\n"
    "  .byte 0x6a,0x01,0xe8\n"
    "  .long _h_never - . - 4\n"
    "  .byte 0x5f,0x5e,0x5b,0x81,0xc4,0x44,0x01,0x00,0x00,0xc2,0x08,0x00,0x6a,0x00,0x8b,0xce,0xe8\n"
    "  .long _fake_msgline_stop - . - 4\n"
    "  .byte 0x68\n"
    "  .long _g_key_msgline\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe8\n"
    "  .long _fake_close - . - 4\n"
    "  .byte 0x5f,0x5e,0x5b,0x81,0xc4,0x44,0x01,0x00,0x00,0xc2,0x08,0x00,0x8b,0x0d\n"
    "  .long _g_edit_ptr\n"
    "  .byte 0x6a,0x04,0xe8\n"
    "  .long _h_never - . - 4\n"
    "  .byte 0x5f,0x5e,0x5b,0x81,0xc4,0x44,0x01,0x00,0x00,0xc2,0x08,0x00,0x8b,0x0d\n"
    "  .long _g_edit_ptr\n"
    "  .byte 0x6a,0x05,0xe8\n"
    "  .long _h_never - . - 4\n"
    "  .byte 0x5f,0x5e,0x5b,0x81,0xc4,0x44,0x01,0x00,0x00,0xc2,0x08,0x00,0x8b,0x0d\n"
    "  .long _g_edit_ptr\n"
    "  .byte 0x6a,0x03,0xe8\n"
    "  .long _h_never - . - 4\n"
    "  .byte 0x5f,0x5e,0x5b,0x81,0xc4,0x44,0x01,0x00,0x00,0xc2,0x08,0x00,0x8b,0x0d\n"
    "  .long _g_never_data\n"
    "  .byte 0x6a,0x00,0x6a,0x00,0x6a,0x02,0x8b,0x11,0xff,0x52,0x08,0x5f,0x5e,0x5b,0x81,0xc4,0x44,0x01,0x00,0x00\n"
    "  .byte 0xc2,0x08,0x00,0x8b,0xff\n"
    "  .long _fake_msgline_input + 0x417\n"
    "  .long _fake_msgline_input + 0x3fe\n"
    "  .long _fake_msgline_input + 0x3c1\n"
    "  .long _fake_msgline_input + 0x3da\n"
    "  .long _fake_msgline_input + 0x430\n"
    "  .long _fake_msgline_input + 0x2b\n"
    "  .long _fake_msgline_input + 0x449\n"
    "  .long _fake_msgline_input + 0x45a\n"
    "  .byte 0x00,0x01,0x02,0x03,0x07,0x04,0x07,0x07,0x07,0x04,0x07,0x07,0x07,0x07,0x05,0x06\n"
    "  .long _fake_msgline_input + 0x6a\n"
    "  .long _fake_msgline_input + 0xd4\n"
    "  .long _fake_msgline_input + 0x10b\n"
    "  .long _fake_msgline_input + 0x18d\n"
    "  .long _fake_msgline_input + 0x251\n"
    "  .long _fake_msgline_input + 0x2ef\n"
    "  .long _fake_msgline_input + 0x384\n"
    "  .long _fake_msgline_input + 0x32a\n"
    // scname's begin-close and field end, run, and its input routine, never run:
    // the game's bytes.
    ".p2align 4\n"
    ".globl _fake_scname_close\n"
    "_fake_scname_close:\n"
    "  .byte 0x8b,0x41,0x14,0x85,0xc0,0x74,0x07,0x6a,0x00,0xe8\n"
    "  .long _fake_scname_stop - . - 4\n"
    "  .byte 0xc3\n"
    ".p2align 4\n"
    ".globl _fake_scname_stop\n"
    "_fake_scname_stop:\n"
    "  .byte 0x56,0x8b,0xf1,0x8b,0x4e,0x14,0x85,0xc9,0x74,0x33,0x8a,0x44,0x24,0x08,0x84,0xc0,0x74,0x14,0x6a,0x00\n"
    "  .byte 0xe8\n"
    "  .long @h_field_read@12 - . - 4\n"
    "  .byte 0x8d,0x56,0x18,0x8a,0x08,0x40,0x88,0x0a,0x42,0x84,0xc9,0x75,0xf6,0x8b,0x4e,0x14,0x85,0xc9,0x74,0x24\n"
    "  .byte 0x8b,0x01,0x6a,0x01,0xff,0x50,0x0c,0xc7,0x46,0x14,0x00,0x00,0x00,0x00,0xeb,0x14,0xb8\n"
    "  .long _g_empty_text\n"
    "  .byte 0x8d,0x56,0x18,0x2b,0xd0,0x8a,0x08,0x88,0x0c,0x02,0x40,0x84,0xc9,0x75,0xf6,0x8b,0x0d\n"
    "  .long _g_edit_ptr\n"
    "  .byte 0x6a,0x00,0xe8\n"
    "  .long @h_edit_attach@12 - . - 4\n"
    "  .byte 0x8b,0x46,0x08,0x85,0xc0,0x74,0x0b,0xc6,0x40,0x61,0x01,0x8b,0x4e,0x08,0xc6,0x41,0x62,0x01,0x5e,0xc2\n"
    "  .byte 0x04,0x00\n"
    ".p2align 4\n"
    ".globl _fake_scname_input\n"
    "_fake_scname_input:\n"
    "  .byte 0xa1\n"
    "  .long _g_search_ptr\n"
    "  .byte 0x57,0x85,0xc0,0x8b,0xf9,0x0f,0x84,0x07,0x01,0x00,0x00,0x0f,0xbf,0x4c,0x24,0x08,0x83,0xc1,0xfd,0x83\n"
    "  .byte 0xf9,0x0e,0x0f,0x87,0xf6,0x00,0x00,0x00,0x33,0xd2,0x8a,0x91\n"
    "  .long _fake_scname_input + 0x134\n"
    "  .byte 0xff,0x24,0x95\n"
    "  .long _fake_scname_input + 0x11c\n"
    "  .byte 0x66,0x83,0x7c,0x24,0x0c,0x01,0x0f,0x85,0xdb,0x00,0x00,0x00,0x6a,0x01,0x8b,0xcf,0xe8\n"
    "  .long _fake_scname_stop - . - 4\n"
    "  .byte 0x5f,0xc2,0x08,0x00,0x0f,0xbf,0x4c,0x24,0x0c,0x49,0x74,0x6d,0x49,0x74,0x1a,0x49,0x0f,0x85,0xbc,0x00\n"
    "  .byte 0x00,0x00,0x68\n"
    "  .long _g_key_scname\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe8\n"
    "  .long _fake_close - . - 4\n"
    "  .byte 0x5f,0xc2,0x08,0x00,0x56,0x8d,0x70,0x5c,0x56,0xe8\n"
    "  .long _h_criteria_clear - . - 4\n"
    "  .byte 0x83,0xc7,0x18,0x57,0x56,0xe8\n"
    "  .long _h_criteria_name - . - 4\n"
    "  .byte 0x83,0xc4,0x0c,0x8b,0x0d\n"
    "  .long _g_search_ptr\n"
    "  .byte 0x6a,0x00,0x6a,0x00,0x6a,0x00,0x6a,0x01,0x56,0x6a,0x01,0xe8\n"
    "  .long @h_search_start@32 - . - 4\n"
    "  .byte 0x68\n"
    "  .long _g_key_scname\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe8\n"
    "  .long _fake_close - . - 4\n"
    "  .byte 0x68\n"
    "  .long _g_key_scsibori\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe8\n"
    "  .long _fake_close - . - 4\n"
    "  .byte 0x5e,0x5f,0xc2,0x08,0x00,0x8b,0x47,0x14,0x85,0xc0,0x74,0x11,0x8b,0x0d\n"
    "  .long _g_edit_ptr\n"
    "  .byte 0x6a,0x01,0xe8\n"
    "  .long _h_never - . - 4\n"
    "  .byte 0x5f,0xc2,0x08,0x00,0x8b,0xcf,0xe8\n"
    "  .long _h_never - . - 4\n"
    "  .byte 0x5f,0xc2,0x08,0x00,0x8b,0x47,0x14,0x85,0xc0,0x0f,0x84,0x6f,0xff,0xff,0xff,0x6a,0x01,0x8b,0xcf,0xe8\n"
    "  .long _fake_scname_stop - . - 4\n"
    "  .byte 0x5f,0xc2,0x08,0x00,0x8b,0x0d\n"
    "  .long _g_edit_ptr\n"
    "  .byte 0x6a,0x04,0xe8\n"
    "  .long _h_never - . - 4\n"
    "  .byte 0x5f,0xc2,0x08,0x00,0x8b,0x0d\n"
    "  .long _g_edit_ptr\n"
    "  .byte 0x6a,0x05,0xe8\n"
    "  .long _h_never - . - 4\n"
    "  .byte 0x5f,0xc2,0x08,0x00,0x90\n"
    "  .long _fake_scname_input + 0x10a\n"
    "  .long _fake_scname_input + 0xf9\n"
    "  .long _fake_scname_input + 0x49\n"
    "  .long _fake_scname_input + 0xe1\n"
    "  .long _fake_scname_input + 0x30\n"
    "  .long _fake_scname_input + 0x117\n"
    "  .byte 0x00,0x01,0x02,0x03,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x04,0x90\n"
    // link11's field end and begin-close, run, and its input routine, never run:
    // the game's bytes around the stand-ins.
    ".p2align 4\n"
    ".globl _fake_link11_stop\n"
    "_fake_link11_stop:\n"
    "  .byte 0x56,0x8b,0xf1,0x8b,0x46,0x14,0x83,0xf8,0x01,0x74,0x18,0x7e,0x4f,0x83,0xf8,0x04,0x7f,0x4a,0x8a,0x4c\n"
    "  .byte 0x24,0x08,0x84,0xc9,0x75,0x42,0x8a,0x4e,0x33,0x88,0x4c,0x30,0x2e,0xeb,0x39,0x8b,0x4e,0x2c,0x6a,0x00\n"
    "  .byte 0xe8\n"
    "  .long @h_field_read@12 - . - 4\n"
    "  .byte 0x8d,0x56,0x34,0x8a,0x08,0x40,0x88,0x0a,0x42,0x84,0xc9,0x75,0xf6,0x8b,0x4e,0x2c,0x85,0xc9,0x74,0x0e\n"
    "  .byte 0x8b,0x11,0x6a,0x01,0xff,0x52,0x0c,0xc7,0x46,0x2c,0x00,0x00,0x00,0x00,0x8b,0x0d\n"
    "  .long _g_edit_ptr\n"
    "  .byte 0x6a,0x00,0xe8\n"
    "  .long @h_edit_attach@12 - . - 4\n"
    "  .byte 0x8b,0x46,0x08,0xc7,0x46,0x14,0x00,0x00,0x00,0x00,0x85,0xc0,0x74,0x0b,0xc6,0x40,0x61,0x01,0x8b,0x46\n"
    "  .byte 0x08,0xc6,0x40,0x62,0x01,0x5e,0xc2,0x04,0x00\n"
    ".p2align 4\n"
    ".globl _fake_link11_close\n"
    "_fake_link11_close:\n"
    "  .byte 0x8b,0x41,0x2c,0x85,0xc0,0x74,0x07,0x6a,0x00,0xe8\n"
    "  .long _fake_link11_stop - . - 4\n"
    "  .byte 0xc3\n"
    ".p2align 4\n"
    ".globl _fake_link11_input\n"
    "_fake_link11_input:\n"
    "  .byte 0x55,0x56,0x8b,0xf1,0x57,0x8b,0x46,0x14,0x85,0xc0,0x74,0x14,0x8b,0x7e,0x08,0x6a,0x00,0x50,0x8b,0xcf\n"
    "  .byte 0xe8\n"
    "  .long _fake_set_cursor - . - 4\n"
    "  .byte 0x8b,0xcf,0xe8\n"
    "  .long _h_never - . - 4\n"
    "  .byte 0x8b,0x46,0x14,0x66,0x8b,0x4c,0x24,0x10,0x83,0xf8,0x01,0x75,0x47,0x66,0x83,0xf9,0x04,0x75,0x13,0x8b\n"
    "  .byte 0x0d\n"
    "  .long _g_edit_ptr\n"
    "  .byte 0x6a,0x04,0xe8\n"
    "  .long _h_never - . - 4\n"
    "  .byte 0x5f,0x5e,0x5d,0xc2,0x08,0x00,0x66,0x83,0xf9,0x03,0x75,0x13,0x8b,0x0d\n"
    "  .long _g_edit_ptr\n"
    "  .byte 0x6a,0x05,0xe8\n"
    "  .long _h_never - . - 4\n"
    "  .byte 0x5f,0x5e,0x5d,0xc2,0x08,0x00,0x66,0x83,0xf9,0x11,0x75,0x0f,0x6a,0x01,0x8b,0xce,0xe8\n"
    "  .long _fake_link11_stop - . - 4\n"
    "  .byte 0x5f,0x5e,0x5d,0xc2,0x08,0x00,0x66,0x83,0xf9,0x05,0x74,0x61,0x66,0x83,0xf9,0x06,0x0f,0x85,0x19,0x02\n"
    "  .byte 0x00,0x00,0x85,0xc0,0x74,0x0f,0x6a,0x00,0x8b,0xce,0xe8\n"
    "  .long _fake_link11_stop - . - 4\n"
    "  .byte 0x5f,0x5e,0x5d,0xc2,0x08,0x00,0x68\n"
    "  .long _g_key_link11\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe8\n"
    "  .long _fake_close - . - 4\n"
    "  .byte 0x6a,0x00,0x6a,0x01,0x68\n"
    "  .long _g_key_link6\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe8\n"
    "  .long _fake_open - . - 4\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_ctl_global + 4 * 121\n"
    "  .byte 0x5f,0x5e,0x5d,0x8b,0x51,0x08,0xc7,0x44,0x24,0x08,0x00,0x00,0x00,0x00,0x89,0x54,0x24,0x04,0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe9\n"
    "  .long @h_focus@16 - . - 4\n"
    "  .byte 0x83,0xf8,0x01,0x75,0x12,0x8b,0x0d\n"
    "  .long _g_edit_ptr\n"
    "  .byte 0x50,0xe8\n"
    "  .long _h_never - . - 4\n"
    "  .byte 0x5f,0x5e,0x5d,0xc2,0x08,0x00,0x85,0xc0,0x74,0x0f,0x6a,0x01,0x8b,0xce,0xe8\n"
    "  .long _fake_link11_stop - . - 4\n"
    "  .byte 0x5f,0x5e,0x5d,0xc2,0x08,0x00,0x0f,0xbf,0x44,0x24,0x14,0x48,0x83,0xf8,0x05,0x0f,0x87,0x89,0x01,0x00\n"
    "  .byte 0x00,0xff,0x24,0x85\n"
    "  .long _fake_link11_input + 0x2a4\n"
    "  .byte 0x8a,0x46,0x34,0x8d,0x6e,0x34,0x84,0xc0,0x75,0x0b,0xe8\n"
    "  .long _h_never - . - 4\n"
    "  .byte 0x5f,0x5e,0x5d,0xc2,0x08,0x00,0x53,0x6a,0x01,0x6a,0x00,0x6a,0x00,0x6a,0x00,0xe8\n"
    "  .long _h_slot_taken - . - 4\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_ctl_global + 4 * 121\n"
    "  .byte 0x83,0xc4,0x10,0x84,0xc0,0x0f,0x95,0xc3,0xfe,0xc3,0x88,0x5c,0x24,0x14,0xe8\n"
    "  .long @h_link5_selected@8 - . - 4\n"
    "  .byte 0x8b,0xf8,0x85,0xff,0x74,0x76,0x8a,0x46,0x32,0x8a,0x4e,0x31,0x8a,0x56,0x30,0x50,0x51,0x52,0x55,0xe8\n"
    "  .long _h_encode - . - 4\n"
    "  .byte 0x8a,0x4f,0x01,0x8b,0x54,0x24,0x24,0x83,0xc4,0x04,0x50,0x8a,0x07,0x6a,0x01,0x50,0x51,0x52,0xe8\n"
    "  .long _h_create - . - 4\n"
    "  .byte 0x83,0xc4,0x20,0x84,0xc0,0x74,0x41,0x33,0xc0,0x33,0xc9,0x8a,0x07,0x8a,0x4f,0x01,0x50,0x51,0xe8\n"
    "  .long _h_item - . - 4\n"
    "  .byte 0x83,0xc4,0x08,0x85,0xc0,0x74,0x1b,0x8b,0x00,0x88,0x1d\n"
    "  .long _g_made\n"
    "  .byte 0xa3\n"
    "  .long _g_made + 4\n"
    "  .byte 0xc6,0x05\n"
    "  .long _g_made + 8\n"
    "  .byte 0x01,0xc6,0x05\n"
    "  .long _g_made + 9\n"
    "  .byte 0x3c,0xa1\n"
    "  .long _g_ctl_global + 4 * 121\n"
    "  .byte 0x85,0xc0,0x74,0x0b,0x80,0x48,0x65,0x01,0xeb,0x05,0xe8\n"
    "  .long _h_never - . - 4\n"
    "  .byte 0x68\n"
    "  .long _g_key_link11\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe8\n"
    "  .long _fake_close - . - 4\n"
    "  .byte 0x6a,0x00,0x6a,0x01,0x68\n"
    "  .long _g_key_link6\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe8\n"
    "  .long _fake_open - . - 4\n"
    "  .byte 0x8b,0x15\n"
    "  .long _g_ctl_global + 4 * 121\n"
    "  .byte 0x5b,0x5f,0x5e,0x8b,0x42,0x08,0x5d,0xc7,0x44,0x24,0x08,0x00,0x00,0x00,0x00,0x89,0x44,0x24,0x04,0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe9\n"
    "  .long @h_focus@16 - . - 4\n"
    "  .byte 0x68\n"
    "  .long _g_key_link11\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe8\n"
    "  .long _fake_close - . - 4\n"
    "  .byte 0x6a,0x00,0x6a,0x01,0x68\n"
    "  .long _g_key_link6\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe8\n"
    "  .long _fake_open - . - 4\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_ctl_global + 4 * 121\n"
    "  .byte 0x6a,0x00,0x8b,0x51,0x08,0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0x52,0xe8\n"
    "  .long @h_focus@16 - . - 4\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_ctl_global + 4 * 121\n"
    "  .byte 0x6a,0x00,0x6a,0x00,0x6a,0x00,0xe8\n"
    "  .long _h_never - . - 4\n"
    "  .byte 0x5f,0x5e,0x5d,0xc2,0x08,0x00,0x6a,0x01,0x8b,0xce,0xe8\n"
    "  .long _h_never - . - 4\n"
    "  .byte 0x5f,0x5e,0x5d,0xc2,0x08,0x00,0x6a,0x02,0x8b,0xce,0xe8\n"
    "  .long _h_never - . - 4\n"
    "  .byte 0x5f,0x5e,0x5d,0xc2,0x08,0x00,0x6a,0x03,0x8b,0xce,0xe8\n"
    "  .long _h_never - . - 4\n"
    "  .byte 0x5f,0x5e,0x5d,0xc2,0x08,0x00,0x6a,0x04,0x8b,0xce,0xe8\n"
    "  .long _h_never - . - 4\n"
    "  .byte 0x5f,0x5e,0x5d,0xc2,0x08,0x00,0x90\n"
    "  .long _fake_link11_input + 0x267\n"
    "  .long _fake_link11_input + 0x276\n"
    "  .long _fake_link11_input + 0x285\n"
    "  .long _fake_link11_input + 0x294\n"
    "  .long _fake_link11_input + 0x11b\n"
    "  .long _fake_link11_input + 0x218\n"
    // passinpu's field end, open hook and begin-close, run, and its input
    // routine, never run: the game's bytes around the stand-ins.
    ".p2align 4\n"
    ".globl _fake_passinpu_stop\n"
    "_fake_passinpu_stop:\n"
    "  .byte 0x8a,0x44,0x24,0x04,0x56,0x84,0xc0,0x8b,0xf1,0x74,0x17,0x8b,0x4e,0x38,0x6a,0x00,0xe8\n"
    "  .long @h_field_read@12 - . - 4\n"
    "  .byte 0x8d,0x56,0x24,0x8a,0x08,0x40,0x88,0x0a,0x42,0x84,0xc9,0x75,0xf6,0x8b,0x4e,0x38,0x85,0xc9,0x74,0x0e\n"
    "  .byte 0x8b,0x01,0x6a,0x01,0xff,0x50,0x0c,0xc7,0x46,0x38,0x00,0x00,0x00,0x00,0x8b,0x0d\n"
    "  .long _g_edit_ptr\n"
    "  .byte 0x5e,0xc7,0x44,0x24,0x04,0x00,0x00,0x00,0x00,0xe9\n"
    "  .long @h_edit_attach@12 - . - 4\n"
    ".p2align 4\n"
    ".globl _fake_passinpu_open\n"
    "_fake_passinpu_open:\n"
    "  .byte 0x56,0x8b,0xf1,0x6a,0x01,0x6a,0x10,0x8b,0x4e,0x08,0xe8\n"
    "  .long @h_menu_two@16 - . - 4\n"
    "  .byte 0x8b,0x46,0x14,0x8b,0x4e,0x08,0x85,0xc0,0x75,0x19,0x6a,0x01,0xe8\n"
    "  .long @h_menu_one@12 - . - 4\n"
    "  .byte 0x8b,0xce,0xe8\n"
    "  .long _fake_passinpu_start - . - 4\n"
    "  .byte 0xa1\n"
    "  .long _g_input_state_ptr\n"
    "  .byte 0x5e,0xc6,0x40,0x4c,0x01,0xc3,0x6a,0x00,0x6a,0x01,0xe8\n"
    "  .long @h_menu_two@16 - . - 4\n"
    "  .byte 0x8b,0xce,0xe8\n"
    "  .long _fake_passinpu_start - . - 4\n"
    "  .byte 0xa1\n"
    "  .long _g_input_state_ptr\n"
    "  .byte 0x5e,0xc6,0x40,0x4c,0x01,0xc3\n"
    // passinpu's start (this = the controller): the game's bytes, a field
    // made only when it has none, then the manager's unbind and its bind of
    // the field.
    ".p2align 4\n"
    ".globl _fake_passinpu_start\n"
    "_fake_passinpu_start:\n"
    "  .byte 0x56,0x8b,0xf1,0x57,0x8b,0x46,0x14,0x83,0xf8,0x01,0x7c,0x0c,0x83,0xf8,0x03,0x7d,0x0c,0xbf,0x0a,0x00\n"
    "  .byte 0x00,0x00,0xeb,0x14,0x83,0xf8,0x03,0x7c,0x0a,0x83,0xf8,0x06,0xbf,0x01,0x00,0x00,0x00,0x7c,0x05,0xbf\n"
    "  .byte 0x07,0x00,0x00,0x00,0x8b,0x4e,0x18,0xb8,0x10,0x00,0x00,0x00,0x3b,0xc8,0x7e,0x03,0x89,0x46,0x18,0x8b\n"
    "  .byte 0x46,0x38,0x85,0xc0,0x75,0x3b,0x68,0xb4,0x06,0x00,0x00,0xe8\n"
    "  .long _h_field_new - . - 4\n"
    "  .byte 0x83,0xc4,0x04,0x85,0xc0,0x74,0x25,0x8b,0x4e,0x18,0x68,0x14,0x01,0x00,0x00,0x6a,0x0f,0x68,0x4a,0x01\n"
    "  .byte 0x00,0x00,0x68\n"
    "  .long _g_empty_text\n"
    "  .byte 0x68\n"
    "  .long _h_never\n"
    "  .byte 0x56,0x51,0x57,0x8b,0xc8,0xe8\n"
    "  .long @h_field_ctor@40 - . - 4\n"
    "  .byte 0xeb,0x02,0x33,0xc0,0x89,0x46,0x38,0x8b,0x0d\n"
    "  .long _g_edit_ptr\n"
    "  .byte 0xe8\n"
    "  .long @h_edit_deactivate@8 - . - 4\n"
    "  .byte 0x8b,0x56,0x38,0x8b,0x0d\n"
    "  .long _g_edit_ptr\n"
    "  .byte 0x52,0xe8\n"
    "  .long @h_edit_attach@12 - . - 4\n"
    "  .byte 0x5f,0x5e,0xc3\n"
    ".p2align 4\n"
    ".globl _fake_passinpu_close\n"
    "_fake_passinpu_close:\n"
    "  .byte 0xe8\n"
    "  .long _fake_passinpu_reset - . - 4\n"
    "  .byte 0x8b,0x41,0x38,0x85,0xc0,0x74,0x11,0x6a,0x00,0xe8\n"
    "  .long _fake_passinpu_stop - . - 4\n"
    "  .byte 0xa1\n"
    "  .long _g_input_state_ptr\n"
    "  .byte 0xc6,0x40,0x4c,0x00,0xc3,0x8b,0x0d\n"
    "  .long _g_input_state_ptr\n"
    "  .byte 0xc6,0x41,0x4c,0x00,0xc3\n"
    ".p2align 4\n"
    ".globl _fake_passinpu_input\n"
    "_fake_passinpu_input:\n"
    "  .byte 0x0f,0xbf,0x44,0x24,0x04,0x83,0xc0,0xfd,0x56,0x83,0xf8,0x0e,0x8b,0xf1,0x77,0x76,0x33,0xc9,0x8a,0x88\n"
    "  .long _fake_passinpu_input + 0xa0\n"
    "  .byte 0xff,0x24,0x8d\n"
    "  .long _fake_passinpu_input + 0x8c\n"
    "  .byte 0x6a,0x01,0x8b,0xce,0xe8\n"
    "  .long _fake_passinpu_stop - . - 4\n"
    "  .byte 0x8b,0x46,0x1c,0x85,0xc0,0x74,0x0d,0x8b,0x4e,0x20,0x8d,0x56,0x24,0x52,0x51,0xff,0xd0,0x83,0xc4,0x08\n"
    "  .byte 0x8b,0xce,0xe8\n"
    "  .long _fake_passinpu_reset - . - 4\n"
    "  .byte 0x8b,0x56,0x08,0x6a,0x01,0x52,0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe8\n"
    "  .long _h_close_menu@8 - . - 4\n"
    "  .byte 0x5e,0xc2,0x08,0x00,0x8b,0x0d\n"
    "  .long _g_edit_ptr\n"
    "  .byte 0x6a,0x01,0xe8\n"
    "  .long _h_never - . - 4\n"
    "  .byte 0x5e,0xc2,0x08,0x00,0x8b,0x0d\n"
    "  .long _g_edit_ptr\n"
    "  .byte 0x6a,0x04,0xe8\n"
    "  .long _h_never - . - 4\n"
    "  .byte 0x5e,0xc2,0x08,0x00,0x8b,0x0d\n"
    "  .long _g_edit_ptr\n"
    "  .byte 0x6a,0x05,0xe8\n"
    "  .long _h_never - . - 4\n"
    "  .byte 0x5e,0xc2,0x08,0x00,0x8b,0xff\n"
    "  .long _fake_passinpu_input + 0x79\n"
    "  .long _fake_passinpu_input + 0x68\n"
    "  .long _fake_passinpu_input + 0x57\n"
    "  .long _fake_passinpu_input + 0x1f\n"
    "  .long _fake_passinpu_input + 0x86\n"
    "  .byte 0x00,0x01,0x02,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x04,0x03,0x90\n"

    // comment's begin-close (this = the controller): the game's first bytes,
    // a field still open ended keeping its line, then the stand-in.
    ".p2align 4\n"
    ".globl _fake_comment_close\n"
    "_fake_comment_close:\n"
    "  .byte 0x81,0xec,0x88,0x00,0x00,0x00,0x53,0x56,0x8b,0xf1,0x33,0xdb,0x39,0x9e,0x78,0x03,0x00,0x00,0x74,0x07\n"
    "  .byte 0x6a,0x01\n"
    "  call _fake_comment_stop\n"
    "  .byte 0x56\n"
    "  call _h_comment_sent@4\n"
    "  .byte 0x5e,0x5b,0x81,0xc4,0x88,0x00,0x00,0x00,0xc3\n"

    // msgline's open hook (this = the controller): the start of its field,
    // called as the game's hook calls it.
    ".p2align 4\n"
    ".globl _fake_msgline_open\n"
    "_fake_msgline_open:\n"
    "  .byte 0x56,0x8b,0xf1,0x33,0xd2,0x8b,0xce,0x66,0x89,0x56,0x18\n"
    "  call _fake_msgline_start\n"
    "  .byte 0x5e,0xc3\n"
    "  .fill 0x90, 1, 0xcc\n"

    // The start (this = the controller): the game's test of modes 2 and 7,
    // then on either path the stand-in; the two fields' pushes as the
    // game's carries them, never run.
    ".p2align 4\n"
    "_fake_msgline_start:\n"
    "  .byte 0x56,0x8b,0xf1,0x66,0x8b,0x46,0x14,0x66,0x3d,0x02,0x00,0x74,0x45,0x66,0x3d,0x07,0x00,0x74,0x3f\n"
    "  .byte 0x8b,0xce\n"
    "  call @h_msgline_field@8\n"
    "  .byte 0x5e,0xc3\n"
    "  .byte 0x68,0x2c,0x01,0x00,0x00,0x6a,0x08,0x6a,0x0f,0x6a,0x07\n"
    "  .fill 0x52 - 0x27, 1, 0xcc\n"
    "  .byte 0x8b,0xce\n"
    "  call @h_msgline_field@8\n"
    "  .byte 0x5e,0xc3\n"
    "  .fill 0xb8, 1, 0xcc\n"

    // comment's open hook (this = the controller): the game's bytes, its
    // policy bit written to a word of this image's, not the menu: another
    // player's message starts no field, any other line 0's through the
    // start.
    ".p2align 4\n"
    ".globl _fake_comment_open\n"
    "_fake_comment_open:\n"
    "  .byte 0x56,0x8b,0xf1,0x80,0x7e,0x16,0x01,0x75,0x02,0x5e,0xc3,0xb8\n"
    "  .long _g_comment_policy - 0x34\n"
    "  .byte 0x6a,0x00,0x8b,0xce,0x8b,0x50,0x34,0x80,0xce,0x10,0x89,0x50,0x34,0xc6,0x46,0x14,0xff,0xc6,0x46,0x15\n"
    "  .byte 0x00\n"
    "  call _fake_comment_start\n"
    "  .byte 0x5e,0xc3\n"
    "  .fill 0x80, 1, 0xcc\n"

    // comment's start (this = the controller; the line; ret 4): the game's
    // head, the line's trim, the stand-in starting the field, then the line
    // it edits written to +0x14.
    ".p2align 4\n"
    ".globl _fake_comment_start\n"
    "_fake_comment_start:\n"
    "  .byte 0x83,0xec,0x08,0x53,0x8b,0x5c,0x24,0x10,0x55,0x33,0xed,0x56,0x3b,0xdd,0x8b,0xf1,0x0f,0xbe,0x46,0x16\n"
    "  .byte 0x8d,0x0c,0x43,0x03,0xc1,0x8d,0x14,0x80,0x03,0xc6,0x8d,0x4c,0xd0,0x1a,0x51\n"
    "  call _h_trim\n"
    "  .byte 0x8b,0x86,0x78,0x03,0x00,0x00,0x83,0xc4,0x04,0x53,0x56\n"
    "  call _h_comment_start@8\n"
    "  .byte 0x88,0x5e,0x14,0x5e,0x5d,0x5b,0x83,0xc4,0x08,0xc2,0x04,0x00\n"
    "  .fill 0x104, 1, 0xcc\n"

    // msgline's begin-close (this = the controller): a field still open
    // ended, dropping its text.
    ".p2align 4\n"
    ".globl _fake_msgline_close\n"
    "_fake_msgline_close:\n"
    "  .byte 0x56,0x8b,0xf1,0x83,0x7e,0x1c,0x00,0x74,0x09,0x6a,0x00,0x8b,0xce\n"
    "  call _fake_msgline_stop\n"
    "  .byte 0x5e,0xc3\n"

    ".section .rdata,\"dr\"\n"
    ".p2align 2\n"
    ".globl _g_text_vtables\n"
    "_g_text_vtables:\n"
    "  .long 0, _fake_comment_open, _fake_comment_close, 0, 0, _fake_bare_ret, _fake_comment_input\n"
    "  .fill 25, 4, 0\n"
    "  .long 0, _fake_msgline_open, _fake_msgline_close, 0, @h_msgline_draw@8, _fake_bare_ret, _fake_msgline_input\n"
    "  .fill 25, 4, 0\n"
    "  .long 0, _fake_passinpu_open, _fake_passinpu_close, 0, @h_passinpu_draw@8, _fake_bare_ret\n"
    "  .long _fake_passinpu_input\n"
    "  .fill 25, 4, 0\n"
    "  .long 0, 0, _fake_scname_close, 0, 0, _fake_bare_ret, _fake_scname_input\n"
    "  .fill 25, 4, 0\n"
    "  .long 0, 0, _fake_link11_close, 0, 0, _fake_bare_ret, _fake_link11_input\n"
    "  .fill 25, 4, 0\n"
    ".data\n"
    ".p2align 2\n"
    ".globl _g_field_vtable\n"
    "_g_field_vtable:\n"
    "  .long 0, 0, 0, @h_field_delete@12\n"
    ".text\n"
);

namespace {

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
    build_store();
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
    for (int k = 0; k < 4; ++k) {
        const uint16_t* type = &g_set_types[k];
        memcpy(g_set_items[k] + kFakeItemType, &type, 4);
    }
    g_macro_editor[0x0C] = 1;
    g_policy_table[0] = reinterpret_cast<const void*>(&fake_open);
    g_policy_list[0] = g_policy_table;
    g_policy_ptr = reinterpret_cast<uint8_t*>(g_policy_list);
    for (int i = 0; i < 6; ++i) {
        g_mission_cases[i] = reinterpret_cast<const uint8_t*>(&fake_missions_setter) + 0x6d;
    }
    g_routes.msglist = 1;
    memset(g_routes.bag_size, 81, sizeof(g_routes.bag_size));
    g_routes.scan_answer = -1;

    g_vtable[5] = g_self_refresh;
    g_vtable[0x18 / 4] = reinterpret_cast<const void*>(&fake_query_input);
    memcpy(g_box_vtable, g_vtable, sizeof(g_vtable));
    g_box_vtable[0x18 / 4] = reinterpret_cast<const void*>(&fake_box_input);
    memcpy(g_area_vtable, g_vtable, sizeof(g_vtable));
    fake_area::fill_vtable(g_area_vtable);
    fake_area::install(&area_close_by_name, &area_select);
    memcpy(g_bank_vtable, g_vtable, sizeof(g_vtable));
    g_bank_vtable[2] = reinterpret_cast<const void*>(&fake_bank_begin_close);
    memcpy(g_map_vtable, g_vtable, sizeof(g_vtable));
    g_map_vtable[0x74 / 4] = reinterpret_cast<const void*>(&h_map_close_name);
    const char* pieces[] = {"logwindo", "ability", "equip", "menuwind", "query", "buff",
                            "passinpu", "prtyjoin", "link5", "arealist", "scsibori", "delivery",
                            "post1", "post2", "partywin", "playermo", "targetwi", "subwindo",
                            "dbdelsel", "mcr1pall", "mcr2pall", "abisortw", "holdtime", "dead",
                            "moneyctr", "itemctrl", "handover", "trade", "auc1", "shop", "bank",
                            "tskill1", "iuse", "evitem", "map0", "sortyn", "evitem01", "confyn",
                            "bankmenu", "maplist", "gmtell", "mp_dead", "ok", "conq3pts", "magic", "mgcsortw",
                            "blusortw", "persona", "money", "inventor", "iteminfo", "bluehelp",
                            "bluinven", "bluepoin", "bluequip", "statpup", "automato", "mp_stat", "level", "mp_cstm",
                            "mp_mncst", "prvdungp", "mcrbedit", "mcr1edit", "mcr2edit", "titlewin", "titlehan",
                            "ut_point", "storage", "storage2", "mogpost", "jobcselu", "friend", "msglist", "blklist",
                            "mount", "jbpcat", "fp_cat2", "scresult", "comgenre", "sccompar", "sccomite", "sccomls",
                            "itemxinf", "itemyinf", "casttime", "eventtim", "statcom2", "conf11l", "quest00",
                            "quest01", "miss00", "scanlist", "cnqframe", "coloresu", "colorank", "imperial", "allied",
                            "colopoin", "textcol1", "textcol2", "textcol3", "macro", "mcrledit", "searchjo",
                            "searchna", "searchra", "searchle", "scchar", "searchma", "cfilter", "conf3win",
                            "k1assign", "conf2win"};
    for (size_t i = 0; i < sizeof(pieces) / sizeof(pieces[0]); ++i) {
        for (int r = 0; r < kRowCount; ++r) {
            if (strcmp(kRowSpecs[r].name, pieces[i]) == 0) {
                uint8_t* ctl = g_ctl[g_ctl_used++];
                const bool area = strcmp(pieces[i], "arealist") == 0;
                const void* vt = strcmp(pieces[i], "post1") == 0 ? g_box_vtable : area ? g_area_vtable
                    : strcmp(pieces[i], "bankmenu") == 0 ? g_bank_vtable : strcmp(pieces[i], "maplist") == 0
                    ? g_map_vtable : g_vtable;
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
    // The yes/no windows, each its own controller keyed to its row, but the
    // resyn family's one, keyed to the English client's resynus; the class's
    // cancel as the game's constructors pass it.
    memcpy(g_yesno_vtable, g_vtable, sizeof(g_vtable));
    g_yesno_vtable[2] = reinterpret_cast<const void*>(&fake_yesno_begin_close);
    g_yesno_vtable[0x18 / 4] = reinterpret_cast<const void*>(&fake_yesno_input_site);
    struct YesNo {
        const char* name;
        const char* key;
        uint8_t cancellable;
    };
    const YesNo yesno[] = {
        {"comyn", "menu    comyn   ", 1}, {"merityn", "menu    merityn ", 0}, {"shopbuy", "menu    shopbuy ", 1},
        {"shopsell", "menu    shopsell", 1}, {"resyn", "menu    resynus ", 0},
    };
    uint8_t* resyn = NULL;
    for (size_t i = 0; i < sizeof(yesno) / sizeof(yesno[0]); ++i) {
        uint8_t* ctl = g_ctl[g_ctl_used++];
        const void* vt = g_yesno_vtable;
        memcpy(ctl, &vt, 4);
        memcpy(ctl + 0x20, yesno[i].key, 16);
        ctl[0x38] = yesno[i].cancellable;
        g_ctl_global[row_named(yesno[i].name)] = ctl;
        if (strcmp(yesno[i].name, "resyn") == 0) {
            resyn = ctl;
        }
    }
    const char* family[] = {"resynus", "resynde", "resynfr"};
    for (size_t i = 0; i < 3; ++i) {
        g_ctl_global[row_named(family[i])] = resyn;
    }
    // mogdoor and the dialogue boxes, on their classes' vtables in read-only
    // data; each box its text object and chat log.
    uint8_t* door = g_ctl[g_ctl_used++];
    const void* door_vt = g_door_vtable;
    memcpy(door, &door_vt, 4);
    g_ctl_global[row_named("mogdoor")] = door;
    uint8_t* actor = g_door_actor;
    memcpy(g_house + 0x38, &actor, 4);
    for (int d = 0; d < 2; ++d) {
        uint8_t* ctl = g_ctl[g_ctl_used++];
        const void* vt = g_dialogue_vtables + d * 0x20;
        memcpy(ctl, &vt, 4);
        uint8_t* text = g_texts[d];
        uint8_t* log_ctl = g_log_ctls[d];
        uint8_t* log = g_log_objects[d];
        memcpy(ctl + 0x68, &text, 4);
        memcpy(ctl + 0x64, &log_ctl, 4);
        memcpy(log_ctl + 0x18, &log, 4);
        const int32_t line = 0x60 + d;
        memcpy(ctl + 0x50, &line, 4);
        memcpy(text + 0x30, &ctl, 4);
        g_ctl_global[row_named(kTalkNames[d])] = ctl;
    }
    // The text-entry windows on their classes' vtables in read-only data
    // (passinpu's replacing the plain one above); stringdl's and link6's on
    // the plain one.
    static const char* const texts[] = {"comment", "msgline", "passinpu", "scname", "link11"};
    for (int t = 0; t < 5; ++t) {
        uint8_t* ctl = strcmp(texts[t], "passinpu") == 0 ? ctl_named("passinpu") : g_ctl[g_ctl_used++];
        const void* vt = g_text_vtables + t * 0x20;
        memcpy(ctl, &vt, 4);
        g_ctl_global[row_named(texts[t])] = ctl;
    }
    static const char* const plain[] = {"stringdl", "link6"};
    for (int i = 0; i < 2; ++i) {
        uint8_t* ctl = g_ctl[g_ctl_used++];
        const void* vt = g_vtable;
        memcpy(ctl, &vt, 4);
        g_ctl_global[row_named(plain[i])] = ctl;
    }
    // stringdl's field, its for good; the manager's line and IME display.
    {
        uint8_t* field = new_field();
        memcpy(ctl_named("stringdl") + 0x160, &field, 4);
        const void* line = &g_line_mode;
        const void* ime = g_ime;
        memcpy(g_edit + 0x0C, &line, 4);
        memcpy(g_edit + 0x10, &ime, 4);
    }
    // mapframe, on a class whose fade-out (+0x50) is the stand-in's, and
    // mapv2 on the plain one.
    memcpy(g_frame_vtable, g_vtable, sizeof(g_vtable));
    g_frame_vtable[0x50 / 4] = reinterpret_cast<const void*>(&h_map_fade);
    static const char* const map_windows[] = {"mapframe", "mapv2"};
    for (int i = 0; i < 2; ++i) {
        uint8_t* ctl = g_ctl[g_ctl_used++];
        const void* vt = i == 0 ? g_frame_vtable : g_vtable;
        memcpy(ctl, &vt, 4);
        g_ctl_global[row_named(map_windows[i])] = ctl;
    }
    // statcom2 on its class's vtable in read-only data.
    {
        uint8_t* status = ctl_named("statcom2");
        const void* vt = g_status_vtable;
        memcpy(status, &vt, 4);
    }
    // cnqframe on its class's, whose input routine its Escape is.
    {
        uint8_t* frame = ctl_named("cnqframe");
        const void* vt = g_region_vtable;
        memcpy(frame, &vt, 4);
    }
    // magic on a class whose begin-close (+0x08) is the stand-in's, its sort
    // window's controller at +0xC0 unset, as a fresh one's is.
    memcpy(g_magic_vtable, g_vtable, sizeof(g_vtable));
    g_magic_vtable[2] = reinterpret_cast<const void*>(&h_magic_begin_close);
    {
        uint8_t* magic = ctl_named("magic");
        const void* vt = g_magic_vtable;
        memcpy(magic, &vt, 4);
        const uint32_t unset = 0xBAADF00Du;
        memcpy(magic + 0xC0, &unset, 4);
    }
    // The list object the equipment set's opener steps, on a vtable whose
    // +0x58 is the stand-in's.
    g_equipment_vtable[0x58 / 4] = reinterpret_cast<const void*>(&h_list_step);
    const void* list_vt = g_equipment_vtable;
    memcpy(g_equipment_list, &list_vt, 4);
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

    static const char tags[] = "ftcaq";
    for (int i = 0; i < 5; ++i) {
        g_sprites[i][0] = static_cast<uint8_t>(tags[i]);
    }
    uint8_t* device = g_device;
    memcpy(g_render + 0x0C, &device, 4);
    const void* const* table = g_device_vtable;
    memcpy(g_device, &table, 4);
    g_device_vtable[0x120 / 4] = reinterpret_cast<const void*>(&h_device_draw);
    uint8_t* arrow = g_sprites[3];
    memcpy(ctl_named("targetwi") + 0x78, &arrow, 4);

    // The trade windows: the gil's record, every packet's interval 1000 ms,
    // the clock at 100 s, two actors (one with a titled name), and both
    // modes on the gray test's vtable.
    put_word(g_pd, 0x9860, 0xFFFF);
    for (int id = 0; id < 0x11E; ++id) {
        const uint32_t interval = 1000;
        memcpy(g_intervals + 0x43E2C + 4 * id, &interval, 4);
    }
    const uint32_t seconds = 100;
    memcpy(g_clock_object + 0x0C, &seconds, 4);
    memcpy(g_actor[0] + 0x7C, "Zaldon", 7);
    memcpy(g_actor[1] + 0x7C, "Plain", 6);
    memcpy(g_actor[1] + 0x214, "Moogle", 7);
    put_word(g_actor[1], 0x210, 1);
    g_actors[3] = g_actor[0];
    g_actors[5] = g_actor[1];
    g_mode_vtable[0x30 / 4] = reinterpret_cast<const void*>(&h_gray_test);
    for (int m = 0; m < 2; ++m) {
        const void* vt = g_mode_vtable;
        memcpy(g_modes[m], &vt, 4);
    }
}

}  // namespace

// The trade windows' routines, byte for byte the game's (05-10), their
// globals and the routines they call this image's.
asm(
    ".text\n"
    // The trade's input routine(this = the controller; code, row), ret 8, the
    // game's bytes and its two jump tables, never run.
    ".p2align 4\n"
    ".globl _fake_trade_input\n"
    "_fake_trade_input:\n"
    "  .byte 0x0f,0xbf,0x44,0x24,0x04,0x48,0x56,0x83,0xf8,0x07,0x57,0x8b,0xf1,0x0f,0x87,0x0f,0x02,0x00,0x00,0xff,0x24,0x85\n"
    "  .long _fake_trade_input + 0x264\n"
    "  .byte 0x0f,0xbf,0x44,0x24,0x10,0x8b,0xc8,0x83,0xe9,0x09,0x0f,0x84,0xf3,0x00,0x00,0x00,0x49,0x0f,0x84,0xcf,0x00,0x00,0x00,0x49\n"
    "  .byte 0x74,0x72,0x8b,0x8e,0xc0,0x00,0x00,0x00,0x83,0xf9,0x03,0x75,0x62,0x8a,0x80\n"
    "  .long _g_trade_rows\n"
    "  .byte 0x8b,0x15\n"
    "  .long _g_pd_ptr\n"
    "  .byte 0x88,0x44,0x24,0x10,0x8b,0x4c,0x24,0x10,0x81,0xe1,0xff,0x00,0x00,0x00,0x8d,0x04,0xca,0x66,0x0f,0xb6,0x8c,0xca,0xbe,0x94\n"
    "  .byte 0x01,0x00,0x66,0x8b,0x80,0xbc,0x94,0x01,0x00,0x66,0x85,0xc9,0x75,0x20,0xa1\n"
    "  .long _g_inventor_ptr\n"
    "  .byte 0x5f,0x5e,0xc7,0x44,0x24,0x08,0x00,0x00,0x00,0x00,0x8b,0x48,0x08,0x89,0x4c,0x24,0x04,0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe9\n"
    "  .long @h_focus@16 - . - 4\n"
    "  .byte 0x6a,0x00,0x50,0x51,0x8b,0xce,0xe8\n"
    "  .long _fake_trade_stage - . - 4\n"
    "  .byte 0x5f,0x5e,0xc2,0x08,0x00,0x83,0xf9,0x04,0xeb,0x43,0x8b,0x86,0xc0,0x00,0x00,0x00,0x83,0xf8,0x03,0x75,0x35,0xa1\n"
    "  .long _g_pd_ptr\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_ctl_global + 4 * 45\n"
    "  .byte 0x6a,0x00,0x68\n"
    "  .long _fake_gil_trade\n"
    "  .byte 0x85,0xc0,0x6a,0x00,0x74,0x11,0x8b,0x90,0xb8,0x94,0x01,0x00,0x52,0xe8\n"
    "  .long _fake_money_setup - . - 4\n"
    "  .byte 0x5f,0x5e,0xc2,0x08,0x00,0x6a,0x00,0xe8\n"
    "  .long _fake_money_setup - . - 4\n"
    "  .byte 0x5f,0x5e,0xc2,0x08,0x00,0x83,0xf8,0x04,0x0f,0x85,0x6f,0x01,0x00,0x00,0x8b,0x4e,0x08,0x6a,0x01,0x6a,0x09,0xe8\n"
    "  .long _fake_set_cursor - . - 4\n"
    "  .byte 0x5f,0x5e,0xc2,0x08,0x00,0x6a,0x00,0x8b,0xce,0xe8\n"
    "  .long _fake_trade_cancel - . - 4\n"
    "  .byte 0x84,0xc0,0x0f,0x84,0x4d,0x01,0x00,0x00,0x8b,0xce,0xe8\n"
    "  .long _fake_trade_close - . - 4\n"
    "  .byte 0x5f,0x5e,0xc2,0x08,0x00,0x83,0xbe,0xc0,0x00,0x00,0x00,0x04,0x0f,0x84,0x34,0x01,0x00,0x00,0x8b,0xce,0xe8\n"
    "  .long _fake_trade_ok - . - 4\n"
    "  .byte 0x5f,0x5e,0xc2,0x08,0x00,0x8b,0x4e,0x08,0x6a,0x01,0x6a,0x0a,0xe8\n"
    "  .long _fake_set_cursor - . - 4\n"
    "  .byte 0x5f,0x5e,0xc2,0x08,0x00,0x83,0xbe,0xc0,0x00,0x00,0x00,0x04,0x0f,0x85,0xce,0x00,0x00,0x00,0x66,0x83,0x7c,0x24,0x10,0x0a\n"
    "  .byte 0x0f,0x85,0xc2,0x00,0x00,0x00,0x8b,0x4e,0x08,0x6a,0x01,0x6a,0x09,0xe8\n"
    "  .long _fake_set_cursor - . - 4\n"
    "  .byte 0xe8\n"
    "  .long _h_never - . - 4\n"
    "  .byte 0xe9,0xac,0x00,0x00,0x00,0x83,0xbe,0xc0,0x00,0x00,0x00,0x04,0x75,0x22,0x66,0x83,0x7c,0x24,0x10,0x09,0x0f,0x85,0x97,0x00\n"
    "  .byte 0x00,0x00,0x8b,0x4e,0x08,0x6a,0x01,0x6a,0x0a,0xe8\n"
    "  .long _fake_set_cursor - . - 4\n"
    "  .byte 0xe8\n"
    "  .long _h_never - . - 4\n"
    "  .byte 0xe9,0x81,0x00,0x00,0x00,0x8b,0x46,0x08,0x66,0x83,0x78,0x5a,0x01,0x75,0x77,0x0f,0xbf,0x44,0x24,0x10,0x83,0xc0,0xfb,0x83\n"
    "  .byte 0xf8,0x03,0x77,0x3c,0xff,0x24,0x85\n"
    "  .long _fake_trade_input + 0x284\n"
    "  .byte 0x6a,0x00,0x6a,0x01,0xeb,0x21,0x8b,0x15\n"
    "  .long _g_offer_zero\n"
    "  .byte 0x6a,0x00,0x6a,0x02,0x8b,0x4a,0x08,0xeb,0x1b,0xa1\n"
    "  .long _g_offer_zero\n"
    "  .byte 0x6a,0x00,0x6a,0x03,0x8b,0x48,0x08,0xeb,0x0d,0x6a,0x00,0x6a,0x04,0x8b,0x0d\n"
    "  .long _g_offer_zero\n"
    "  .byte 0x8b,0x49,0x08,0xe8\n"
    "  .long _fake_set_cursor - . - 4\n"
    "  .byte 0xe8\n"
    "  .long _h_never - . - 4\n"
    "  .byte 0x8b,0x15\n"
    "  .long _g_offer_zero\n"
    "  .byte 0x6a,0x00,0x8b,0x42,0x08,0x50,0xeb,0x11,0xe8\n"
    "  .long _h_never - . - 4\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_offer_zero\n"
    "  .byte 0x6a,0x00,0x8b,0x51,0x08,0x52,0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe8\n"
    "  .long @h_focus@16 - . - 4\n"
    "  .byte 0x8b,0x46,0x08,0x85,0xc0,0x74,0x35,0x66,0x8b,0x78,0x4c,0x66,0x3b,0xbe,0xbc,0x00,0x00,0x00,0x74,0x28,0x6a,0x01,0x66,0x83\n"
    "  .byte 0xff,0x0b,0x6a,0x00,0x75,0x04,0x6a,0x07,0xeb,0x02,0x6a,0x0b,0xe8\n"
    "  .long _h_never - . - 4\n"
    "  .byte 0x8b,0x4e,0x08,0x83,0xc4,0x04,0x50,0xe8\n"
    "  .long _h_never - . - 4\n"
    "  .byte 0x66,0x89,0xbe,0xbc,0x00,0x00,0x00,0x5f,0x5e,0xc2,0x08,0x00,0x90\n"
    "  .long _fake_trade_input + 0x176\n"
    "  .long _fake_trade_input + 0x147\n"
    "  .long _fake_trade_input + 0x222\n"
    "  .long _fake_trade_input + 0x222\n"
    "  .long _fake_trade_input + 0x1a\n"
    "  .long _fake_trade_input + 0x136\n"
    "  .long _fake_trade_input + 0x222\n"
    "  .long _fake_trade_input + 0x207\n"
    "  .long _fake_trade_input + 0x1bf\n"
    "  .long _fake_trade_input + 0x1c5\n"
    "  .long _fake_trade_input + 0x1d4\n"
    "  .long _fake_trade_input + 0x1e2\n"

    // The trade's stage(this = the controller; index, id, count), ret 0xC, the
    // game's bytes, run: its un-OK and send are this image's.
    ".p2align 4\n"
    ".globl _fake_trade_stage\n"
    "_fake_trade_stage:\n"
    "  .byte 0x51,0x53,0x56,0x8b,0xf1,0x57,0x8b,0x46,0x08,0x0f,0xbf,0x48,0x4c,0x8a,0x81\n"
    "  .long _g_trade_rows\n"
    "  .byte 0x84,0xc0,0x88,0x44,0x24,0x0c,0x75,0x40,0x66,0x83,0x7c,0x24,0x14,0x00,0x0f,0x85,0x24,0x01,0x00,0x00,0x66,0x81,0x7c,0x24\n"
    "  .byte 0x18,0xff,0xff,0x0f,0x85,0x17,0x01,0x00,0x00,0x8b,0x86,0xc0,0x00,0x00,0x00,0xbb,0x03,0x00,0x00,0x00,0x3b,0xc3,0x74,0x05\n"
    "  .byte 0xe8\n"
    "  .long _h_trade_unok - . - 4\n"
    "  .byte 0x8b,0x54,0x24,0x1c,0x6a,0x00,0x52,0x6a,0x00,0xe8\n"
    "  .long _fake_trade_send - . - 4\n"
    "  .byte 0xe9,0xe1,0x00,0x00,0x00,0x3c,0xff,0x0f,0x84,0xe8,0x00,0x00,0x00,0x8b,0x15\n"
    "  .long _g_pd_ptr\n"
    "  .byte 0x85,0xd2,0x0f,0x84,0xda,0x00,0x00,0x00,0x8b,0x7c,0x24,0x14,0x66,0x85,0xff,0x0f,0x84,0xab,0x00,0x00,0x00,0x66,0x8b,0x44\n"
    "  .byte 0x24,0x18,0x66,0x3d,0xff,0xff,0x0f,0x84,0x9c,0x00,0x00,0x00,0x66,0x85,0xc0,0x0f,0x84,0x93,0x00,0x00,0x00,0xb0,0x01,0x88\n"
    "  .byte 0x44,0x24,0x18,0x8b,0x4c,0x24,0x18,0x81,0xe1,0xff,0x00,0x00,0x00,0x66,0x0f,0xb6,0x8c,0xca,0xbe,0x94,0x01,0x00,0x66,0x3b\n"
    "  .byte 0xf9,0x74,0x0c,0xfe,0xc0,0x3c,0x0a,0x88,0x44,0x24,0x18,0x72,0xde,0xeb,0x04,0x88,0x44,0x24,0x0c,0x8b,0x86,0xc0,0x00,0x00\n"
    "  .byte 0x00,0xbb,0x03,0x00,0x00,0x00,0x3b,0xc3,0x74,0x0b,0xe8\n"
    "  .long _h_trade_unok - . - 4\n"
    "  .byte 0x8b,0x15\n"
    "  .long _g_pd_ptr\n"
    "  .byte 0x8b,0x4c,0x24,0x0c,0x8b,0xc1,0x51,0x25,0xff,0x00,0x00,0x00,0x66,0x83,0xbc,0xc2,0xbc,0x94,0x01,0x00,0x00,0x8d,0x04,0xc2\n"
    "  .byte 0x75,0x0d,0x8b,0x4c,0x24,0x20,0x51,0x57,0xe8\n"
    "  .long _fake_trade_send - . - 4\n"
    "  .byte 0xeb,0x37,0x66,0x0f,0xb6,0x90,0xbe,0x94,0x01,0x00,0x66,0x3b,0xfa,0x74,0x0d,0x8b,0x44,0x24,0x20,0x50,0x57,0xe8\n"
    "  .long _fake_trade_send - . - 4\n"
    "  .byte 0xeb,0x1d,0x6a,0x00,0x57,0xe8\n"
    "  .long _fake_trade_send - . - 4\n"
    "  .byte 0xeb,0x13,0x8b,0x4c,0x24,0x0c,0x51,0x6a,0x00,0x6a,0x00,0xe8\n"
    "  .long _fake_trade_send - . - 4\n"
    "  .byte 0xbb,0x03,0x00,0x00,0x00,0x83,0xc4,0x0c,0x89,0x9e,0xc4,0x00,0x00,0x00,0x89,0x9e,0xc0,0x00,0x00,0x00,0x5f,0x5e,0xb0,0x01\n"
    "  .byte 0x5b,0x59,0xc2,0x0c,0x00\n"

    // The trade's OK and cancel, the game's bytes around stand-in sends.
    ".p2align 4\n"
    ".globl _fake_trade_ok\n"
    "_fake_trade_ok:\n"
    "  .byte 0x56,0x8b,0xf1,0xe8\n"
    "  .long _h_trade_ok_send - . - 4\n"
    "  .byte 0x84,0xc0,0x74,0x0e,0xc7,0x86,0xc0,0x00,0x00,0x00,0x04,0x00,0x00,0x00,0xb0,0x01,0x5e,0xc3,0x32,0xc0,0x5e,0xc3\n"

    ".p2align 4\n"
    ".globl _fake_trade_cancel\n"
    "_fake_trade_cancel:\n"
    "  .byte 0x53,0x56,0x8b,0xf1,0x33,0xdb,0x39,0x5e,0x70,0x75,0x05,0x39,0x5e,0x78,0x74,0x25,0xe8\n"
    "  .long _h_trade_cancel_send - . - 4\n"
    "  .byte 0x84,0xc0,0x74,0x1c,0x89,0x5e,0x6c,0x89,0x5e,0x70,0x88,0x5e,0x7c,0x89,0x9e,0xc0,0x00,0x00,0x00,0x89,0x5e,0x20,0x89,0x5e\n"
    "  .byte 0x24,0x5e,0xb0,0x01,0x5b,0xc2,0x04,0x00,0x5e,0x32,0xc0,0x5b,0xc2,0x04,0x00\n"

    // The trade's open of its windows, the game's bytes, never run.
    ".p2align 4\n"
    ".globl _fake_trade_open\n"
    "_fake_trade_open:\n"
    "  .byte 0x56,0x6a,0x08,0x8b,0xf1,0xe8\n"
    "  .long _h_never - . - 4\n"
    "  .byte 0x83,0xc4,0x04,0x84,0xc0,0x6a,0x00,0x75,0x0b,0x8b,0xce,0xe8\n"
    "  .long _fake_trade_cancel - . - 4\n"
    "  .byte 0x32,0xc0,0x5e,0xc3,0x8d,0x46,0x6c,0x50,0xe8\n"
    "  .long _fake_actor_name - . - 4\n"
    "  .byte 0x83,0xc4,0x08,0x84,0xc0,0x75,0x0d,0x6a,0x00,0x8b,0xce,0xe8\n"
    "  .long _fake_trade_cancel - . - 4\n"
    "  .byte 0x32,0xc0,0x5e,0xc3,0x68,0x00,0x00,0x02,0x00,0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe8\n"
    "  .long _h_never - . - 4\n"
    "  .byte 0x84,0xc0,0x74,0x0d,0x6a,0x00,0x8b,0xce,0xe8\n"
    "  .long _fake_trade_cancel - . - 4\n"
    "  .byte 0x32,0xc0,0x5e,0xc3,0x8b,0x0d\n"
    "  .long _g_trade_mode_ptr\n"
    "  .byte 0x51,0x8b,0x0d\n"
    "  .long _g_inventor_ptr\n"
    "  .byte 0xe8\n"
    "  .long _fake_offer_ret4 - . - 4\n"
    "  .byte 0x6a,0x00,0x6a,0x01,0x68\n"
    "  .long _g_key_money\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe8\n"
    "  .long _fake_open - . - 4\n"
    "  .byte 0x6a,0x00,0x6a,0x01,0x68\n"
    "  .long _g_key_gift\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe8\n"
    "  .long _fake_open - . - 4\n"
    "  .byte 0x6a,0x00,0x6a,0x01,0x68\n"
    "  .long _g_key_trade\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe8\n"
    "  .long _fake_open - . - 4\n"
    "  .byte 0xb8,0x03,0x00,0x00,0x00,0x89,0x86,0xc4,0x00,0x00,0x00,0x89,0x86,0xc0,0x00,0x00,0x00,0x8b,0x15\n"
    "  .long _g_ctl_global + 4 * 43\n"
    "  .byte 0x8b,0x42,0x08,0x85,0xc0,0x75,0x14,0x6a,0x00,0x8b,0xce,0xe8\n"
    "  .long _fake_trade_cancel - . - 4\n"
    "  .byte 0x84,0xc0,0x74,0x07,0x8b,0xce,0xe8\n"
    "  .long _fake_trade_close - . - 4\n"
    "  .byte 0xb0,0x01,0x5e,0xc3\n"

    // The trade's windows closed by name, the game's bytes, run.
    ".p2align 4\n"
    ".globl _fake_trade_close\n"
    "_fake_trade_close:\n"
    "  .byte 0x68\n"
    "  .long _g_key_trade\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe8\n"
    "  .long _fake_close - . - 4\n"
    "  .byte 0x68\n"
    "  .long _g_key_gift\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe8\n"
    "  .long _fake_close - . - 4\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_inventor_ptr\n"
    "  .byte 0xe8\n"
    "  .long _fake_offer_ret - . - 4\n"
    "  .byte 0x68\n"
    "  .long _g_key_money\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe8\n"
    "  .long _fake_close - . - 4\n"
    "  .byte 0x68\n"
    "  .long _g_key_itemctrl\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe8\n"
    "  .long _fake_close - . - 4\n"
    "  .byte 0x68\n"
    "  .long _g_key_moneyctr\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe8\n"
    "  .long _fake_close - . - 4\n"
    "  .byte 0xb0,0x01,0xc3\n"

    // The head of the reader of an actor's name, the game's bytes, never run.
    ".p2align 4\n"
    ".globl _fake_actor_name\n"
    "_fake_actor_name:\n"
    "  .byte 0x56,0x8b,0x74,0x24,0x08,0x57,0x8b,0x46,0x04,0x85,0xc0,0x76,0x24,0x8b,0x06,0x8b,0x04,0x85\n"
    "  .long _g_actors\n"
    "  .byte 0x85,0xc0,0x74,0x17,0x66,0x83,0xb8,0x10,0x02,0x00,0x00,0x00,0x76,0x08,0x8d,0xb8,0x14,0x02,0x00,0x00,0xeb,0x07,0x8d,0x78\n"
    "  .byte 0x7c,0xeb,0x02,0x33,0xff\n"
    "  .byte 0xc3\n"

    // The send of a change to the trade's offer, then the partner's offer's
    // reader, the game's bytes, run: its allocator, its commit and its
    // refusal's message are this image's.
    ".p2align 4\n"
    ".globl _fake_trade_send\n"
    "_fake_trade_send:\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_pd_ptr\n"
    "  .byte 0x53,0x85,0xc9,0x74,0x41,0x8b,0x54,0x24,0x10,0x8b,0x5c,0x24,0x08,0x33,0xc0,0x81,0xe2,0xff,0x00,0x00,0x00,0x81,0xc1,0xbe\n"
    "  .byte 0x94,0x01,0x00,0x3b,0xc2,0x74,0x0c,0x84,0xdb,0x74,0x08,0x85,0xc0,0x74,0x04,0x38,0x19,0x74,0x1f,0x40,0x83,0xc1,0x08,0x83\n"
    "  .byte 0xf8,0x0a,0x72,0xe7,0x6a,0x00,0x6a,0x00,0x6a,0x34,0xe8\n"
    "  .long _fake_alloc - . - 4\n"
    "  .byte 0x83,0xc4,0x0c,0x85,0xc0,0x75,0x27,0x32,0xc0,0x5b,0xc3,0x68,0x80,0x00,0x00,0x00,0x6a,0x09,0xe8\n"
    "  .long _h_message - . - 4\n"
    "  .byte 0x50,0x6a,0x00,0x6a,0x00,0x6a,0x00,0x6a,0x00,0x6a,0x09,0xe8\n"
    "  .long _h_never - . - 4\n"
    "  .byte 0x83,0xc4,0x20,0x32,0xc0,0x5b,0xc3,0x8b,0x4c,0x24,0x0c,0x8a,0x54,0x24,0x10,0x88,0x58,0x0a,0x81,0xe3,0xff,0x00,0x00,0x00\n"
    "  .byte 0x89,0x48,0x04,0x8b,0xcb,0x88,0x50,0x0b,0x6a,0x00,0x8d,0x14,0x89,0x6a,0x0c,0x50,0x8d,0x0c,0x51,0x8b,0x15\n"
    "  .long _g_pd_ptr\n"
    "  .byte 0x66,0x8b,0x8c,0x8a,0x60,0x98,0x00,0x00,0x66,0x89,0x48,0x08,0xe8\n"
    "  .long _h_commit - . - 4\n"
    "  .byte 0x83,0xc4,0x0c,0xb0,0x01,0x5b,0xc3,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x90,0x8b,0x0d\n"
    "  .long _g_pd_ptr\n"
    "  .byte 0x85,0xc9,0x75,0x03,0x32,0xc0,0xc3,0x8b,0x44,0x24,0x08,0x3c,0x0a,0x72,0x03,0x32,0xc0,0xc3,0x25,0xff,0x00,0x00,0x00,0x8d\n"
    "  .byte 0x14,0x80,0x8d,0x04,0x50,0x8d,0x84,0x81,0x00,0x93,0x01,0x00,0x8b,0x48,0x04,0x85,0xc9,0x75,0x03,0x32,0xc0,0xc3,0x8b,0x4c\n"
    "  .byte 0x24,0x04,0x89,0x01,0xb0,0x01,0xc3\n"

    // The packet allocator and its throttle, the game's bytes, run: the
    // queue's allocation is a stand-in.
    ".p2align 4\n"
    ".globl _fake_alloc\n"
    "_fake_alloc:\n"
    "  .byte 0xa1\n"
    "  .long _g_pd_ptr\n"
    "  .byte 0x56,0x85,0xc0,0x57,0x0f,0x84,0xdd,0x00,0x00,0x00,0x8b,0x74,0x24,0x0c,0x81,0xfe,0x1e,0x01,0x00,0x00,0x0f,0x8d,0xcd,0x00\n"
    "  .byte 0x00,0x00,0x8b,0x7c,0x24,0x10,0x85,0xff,0x0f,0x85,0xaa,0x00,0x00,0x00,0x8b,0x8c,0xb0,0xb4,0x41,0x00,0x00,0x85,0xc9,0x0f\n"
    "  .byte 0x84,0x84,0x00,0x00,0x00,0x53,0xe8\n"
    "  .long _fake_clock - . - 4\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_intervals_ptr\n"
    "  .byte 0x8b,0x94,0xb1,0x2c,0x3e,0x04,0x00,0x8b,0x0d\n"
    "  .long _g_pd_ptr\n"
    "  .byte 0x03,0x94,0xb1,0xb4,0x41,0x00,0x00,0x3b,0xc2,0x76,0x30,0xe8\n"
    "  .long _fake_clock - . - 4\n"
    "  .byte 0x8b,0x15\n"
    "  .long _g_intervals_ptr\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_pd_ptr\n"
    "  .byte 0x8b,0x9c,0xb2,0x2c,0x3e,0x04,0x00,0x8b,0x94,0xb1,0xb4,0x41,0x00,0x00,0x2b,0xc3,0x2b,0xc2,0xba,0xff,0xff,0xff,0x7f,0x3b\n"
    "  .byte 0xd0,0x1b,0xc0,0xf7,0xd8,0xeb,0x2b,0xe8\n"
    "  .long _fake_clock - . - 4\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_intervals_ptr\n"
    "  .byte 0x8b,0x94,0xb1,0x2c,0x3e,0x04,0x00,0x8b,0x0d\n"
    "  .long _g_pd_ptr\n"
    "  .byte 0x03,0x94,0xb1,0xb4,0x41,0x00,0x00,0x2b,0xd0,0xb8,0xff,0xff,0xff,0x7f,0x3b,0xc2,0x1b,0xc0,0x40,0x85,0xc0,0x5b,0x75,0x2e\n"
    "  .byte 0xe8\n"
    "  .long _fake_clock - . - 4\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_pd_ptr\n"
    "  .byte 0x89,0x84,0xb1,0xb4,0x41,0x00,0x00,0xa1\n"
    "  .long _g_pd_ptr\n"
    "  .byte 0x8b,0x54,0x24,0x14,0x83,0xc0,0x10,0x52,0x57,0x56,0x50,0xe8\n"
    "  .long _h_queue_alloc - . - 4\n"
    "  .byte 0x83,0xc4,0x10,0x85,0xc0,0x75,0x05,0x5f,0x33,0xc0,0x5e,0xc3,0x66,0x8b,0x08,0x81,0xe6,0xff,0x01,0x00,0x00,0x66,0x89,0x30\n"
    "  .byte 0x5f,0x89,0x4c,0x24,0x08,0x66,0xc7,0x40,0x02,0x00,0x00,0x5e,0xc3\n"

    // The game's clock, its bytes, run.
    ".p2align 4\n"
    ".globl _fake_clock\n"
    "_fake_clock:\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_clock_ptr\n"
    "  .byte 0x8b,0x41,0x0c,0x8b,0x49,0x10,0x8d,0x04,0x80,0x8d,0x04,0x80,0x8d,0x04,0x80,0x8d,0x04,0xc1,0xc3\n"

    // An item record by container and index, the game's bytes, run.
    ".p2align 4\n"
    ".globl _fake_item_record\n"
    "_fake_item_record:\n"
    "  .byte 0x8b,0x15\n"
    "  .long _g_pd_ptr\n"
    "  .byte 0x85,0xd2,0x75,0x03,0x33,0xc0,0xc3,0x8b,0x44,0x24,0x04,0x8b,0x4c,0x24,0x08,0x56,0x8b,0xf1,0x8d,0x04,0xc0,0x81,0xe6,0xff\n"
    "  .byte 0x00,0x00,0x00,0x8d,0x04,0xc0,0x03,0xc6,0x84,0xc9,0x8d,0x34,0x80,0x8d,0x04,0x70,0x5e,0x8d,0x84,0x82,0x60,0x98,0x00,0x00\n"
    "  .byte 0x75,0x13,0x66,0x83,0x38,0x00,0x75,0x17,0xc7,0x40,0x04,0x00,0x00,0x00,0x00,0x66,0xc7,0x00,0xff,0xff,0xc3,0x66,0x8b,0x08\n"
    "  .byte 0x66,0xf7,0xd9,0x1b,0xc9,0x23,0xc1,0xc3\n"

    // The NPC trade's input routine, its open hook, OK, close and stage, the
    // game's bytes: the input routine and the open hook never run.
    ".p2align 4\n"
    ".globl _fake_handover_input\n"
    "_fake_handover_input:\n"
    "  .byte 0x0f,0xbf,0x44,0x24,0x04,0x53,0x56,0x83,0xe8,0x05,0x57,0x8b,0xf1,0x74,0x15,0x48,0x75,0x6e,0x8b,0x4e,0x08,0x85,0xc9,0x74\n"
    "  .byte 0x59,0x6a,0x01,0x6a,0x0a,0xe8\n"
    "  .long _fake_set_cursor - . - 4\n"
    "  .byte 0xeb,0x5c,0x66,0x8b,0x5c,0x24,0x14,0x0f,0xbf,0xc3,0x83,0xe8,0x09,0x74,0x36,0x48,0x74,0x3e,0x48,0x6a,0x00,0x74,0x18,0xa1\n"
    "  .long _g_inventor_ptr\n"
    "  .byte 0x8b,0x48,0x08,0x51,0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe8\n"
    "  .long @h_focus@16 - . - 4\n"
    "  .byte 0x88,0x5e,0x40,0xeb,0x2f,0x8b,0x0d\n"
    "  .long _g_ctl_global + 4 * 45\n"
    "  .byte 0x68\n"
    "  .long _fake_gil_handover\n"
    "  .byte 0x6a,0x00,0x6a,0x00,0xe8\n"
    "  .long _fake_money_setup - . - 4\n"
    "  .byte 0xeb,0x19,0x8b,0xce,0xe8\n"
    "  .long _fake_handover_ok - . - 4\n"
    "  .byte 0x84,0xc0,0x74,0x09,0x8b,0xce,0xe8\n"
    "  .long _fake_handover_close - . - 4\n"
    "  .byte 0xeb,0x05,0xe8\n"
    "  .long _h_never - . - 4\n"
    "  .byte 0x8b,0x46,0x08,0x85,0xc0,0x74,0x2f,0x66,0x8b,0x78,0x4c,0x66,0x3b,0x7e,0x42,0x74,0x25,0x6a,0x01,0x66,0x83,0xff,0x0b,0x6a\n"
    "  .byte 0x00,0x75,0x04,0x6a,0x07,0xeb,0x02,0x6a,0x0b,0xe8\n"
    "  .long _h_never - . - 4\n"
    "  .byte 0x8b,0x4e,0x08,0x83,0xc4,0x04,0x50,0xe8\n"
    "  .long _h_never - . - 4\n"
    "  .byte 0x66,0x89,0x7e,0x42,0x5f,0x5e,0x5b,0xc2,0x08,0x00\n"

    ".p2align 4\n"
    ".globl _fake_handover_open_hook\n"
    "_fake_handover_open_hook:\n"
    "  .byte 0x8b,0xd1,0x57,0xb9,0x0a,0x00,0x00,0x00,0x33,0xc0,0x8d,0x7a,0x14,0x50,0xf3,0xab,0x6a,0x01,0x68\n"
    "  .long _g_key_money\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0x89,0x42,0x3c,0x66,0xc7,0x42,0x42,0x01,0x00,0xe8\n"
    "  .long _fake_open - . - 4\n"
    "  .byte 0xa1\n"
    "  .long _g_offer_zero\n"
    "  .byte 0x5f,0x85,0xc0,0x74,0x1a,0x6a,0x02,0x6a,0x0a,0x6a,0x0d,0xe8\n"
    "  .long _h_message - . - 4\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_offer_zero\n"
    "  .byte 0x83,0xc4,0x08,0x50,0xe8\n"
    "  .long _h_never - . - 4\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_inventor_ptr\n"
    "  .byte 0x85,0xc9,0x74,0x0b,0xa1\n"
    "  .long _g_handover_mode_ptr\n"
    "  .byte 0x50,0xe8\n"
    "  .long _fake_offer_ret4 - . - 4\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_ctl_global + 4 * 55\n"
    "  .byte 0x6a,0x00,0x8b,0x51,0x08,0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0x52,0xe8\n"
    "  .long @h_focus@16 - . - 4\n"
    "  .byte 0xc3\n"

    ".p2align 4\n"
    ".globl _fake_handover_ok\n"
    "_fake_handover_ok:\n"
    "  .byte 0x51,0x53,0x57,0x8b,0xf9,0xb2,0x01,0xb9,0x08,0x00,0x00,0x00,0x8d,0x47,0x18,0x8a,0x58,0x03,0x84,0xdb,0x74,0x08,0x66,0x83\n"
    "  .byte 0x38,0x00,0x74,0x02,0x32,0xd2,0x83,0xc0,0x04,0x49,0x75,0xeb,0x8b,0x47,0x3c,0x85,0xc0,0x74,0x17,0x66,0xc7,0x47,0x14,0xff\n"
    "  .byte 0xff,0xc6,0x47,0x17,0x01,0x8b,0x47,0x44,0x85,0xc0,0x75,0x10,0x5f,0x32,0xc0,0x5b,0x59,0xc3,0x84,0xd2,0x74,0xef,0x5f,0x32\n"
    "  .byte 0xc0,0x5b,0x59,0xc3,0x6a,0x00,0x6a,0x00,0x6a,0x36,0xe8\n"
    "  .long _fake_alloc - . - 4\n"
    "  .byte 0x83,0xc4,0x0c,0x85,0xc0,0x75,0x06,0x5f,0x32,0xc0,0x5b,0x59,0xc3,0x32,0xdb,0x55,0x56,0x88,0x5c,0x24,0x10,0x8d,0x4f,0x16\n"
    "  .byte 0xbd,0x09,0x00,0x00,0x00,0x8a,0x51,0x01,0x84,0xd2,0x74,0x34,0x66,0x8b,0x51,0xfe,0x66,0x85,0xd2,0x74,0x2b,0x8b,0x74,0x24\n"
    "  .byte 0x10,0x81,0xe6,0xff,0x00,0x00,0x00,0x66,0x81,0xfa,0xff,0xff,0x8a,0x11,0x88,0x54,0x06,0x30,0x75,0x05,0x8b,0x57,0x3c,0xeb\n"
    "  .byte 0x05,0x33,0xd2,0x8a,0x51,0x01,0xfe,0xc3,0x89,0x54,0xb0,0x08,0x88,0x5c,0x24,0x10,0x83,0xc1,0x04,0x4d,0x75,0xbf,0x66,0x8b\n"
    "  .byte 0x4f,0x44,0x6a,0x00,0x66,0x89,0x48,0x3a,0x8b,0x57,0x48,0x6a,0x40,0x50,0x89,0x50,0x04,0x88,0x58,0x3c,0xe8\n"
    "  .long _h_commit - . - 4\n"
    "  .byte 0x83,0xc4,0x0c,0xc7,0x47,0x44,0x00,0x00,0x00,0x00,0xb0,0x01,0x5e,0x5d,0x5f,0x5b,0x59,0xc3\n"

    ".p2align 4\n"
    ".globl _fake_handover_close\n"
    "_fake_handover_close:\n"
    "  .byte 0x8b,0x41,0x08,0xc7,0x41,0x44,0x00,0x00,0x00,0x00,0x6a,0x01,0x50,0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe8\n"
    "  .long _h_close_menu@8 - . - 4\n"
    "  .byte 0xc3\n"

    ".p2align 4\n"
    ".globl _fake_handover_stage\n"
    "_fake_handover_stage:\n"
    "  .byte 0x53,0x8a,0x5c,0x24,0x08,0x33,0xd2,0x56,0x3a,0xda,0x57,0x75,0x0d,0x8b,0x44,0x24,0x18,0x5f,0x5e,0x89,0x41,0x3c,0x5b,0xc2\n"
    "  .byte 0x0c,0x00,0x66,0x8b,0x7c,0x24,0x14,0x33,0xf6,0x8d,0x41,0x14,0x38,0x58,0x02,0x75,0x1b,0x66,0x39,0x38,0x75,0x16,0x33,0xdb\n"
    "  .byte 0x66,0x89,0x10,0x88,0x50,0x03,0x88,0x50,0x02,0x8a,0x59,0x40,0x3b,0xde,0x74,0x2d,0x8a,0x5c,0x24,0x10,0x46,0x83,0xc0,0x04\n"
    "  .byte 0x83,0xfe,0x0a,0x7c,0xd7,0x33,0xd2,0x33,0xc0,0x8a,0x51,0x40,0x66,0x89,0x7c,0x91,0x14,0x8a,0x41,0x40,0x8a,0x54,0x24,0x18\n"
    "  .byte 0x88,0x54,0x81,0x17,0x33,0xc0,0x8a,0x41,0x40,0x88,0x5c,0x81,0x16,0x5f,0x5e,0x5b,0xc2,0x0c,0x00\n"

    // The inventory list's build of a row from its cache read on, the game's
    // bytes, never run, then its status rule's class table and cases.
    ".p2align 4\n"
    ".globl _fake_inventor_rows\n"
    "_fake_inventor_rows:\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_cache_ptr\n"
    "  .byte 0x33,0xc0,0x66,0x8b,0x07,0x50,0xe8\n"
    "  .long _fake_item_summary - . - 4\n"
    "  .byte 0x85,0xc0,0x89,0x44,0x24,0x18,0x0f,0x84\n"
    "  .long _h_never - . - 4\n"
    "  .byte 0x8a,0x88,0xaf,0x01,0x00,0x00,0x50,0xfe,0xc1,0x88,0x88,0xaf,0x01,0x00,0x00,0xe8\n"
    "  .long _h_describe - . - 4\n"
    "  .byte 0x83,0xc4,0x04,0x66,0x81,0x3f,0xff,0xff,0x89,0x44,0x24,0x1c,0x75,0x07,0xbd,0x40,0x40,0x40,0x80,0xeb,0x36,0x66,0x8b,0x4e\n"
    "  .byte 0x50,0x66,0x3b,0x4e,0x54,0x8b,0x4e,0x5c,0x8b,0x11,0x7d,0x05,0xff,0x52,0x20,0xeb,0x05,0x50,0x57,0xff,0x52,0x30,0x8b,0xe8\n"
    "  .byte 0x85,0xed,0x75,0x0f,0x8b,0x44,0x24,0x18,0xfe,0x88,0xaf,0x01,0x00,0x00,0xe9\n"
    "  .long _h_never - . - 4\n"
    "  .byte 0x81,0xfd,0x20,0x40,0x80,0x80,0x74,0x2a,0x33,0xc0,0x8a,0x47,0x08,0x83,0xf8,0x1b,0x77,0x1b,0x33,0xc9,0x8a,0x88\n"
    "  .long _fake_rows_classes\n"
    "  .byte 0xff,0x24,0x8d\n"
    "  .long _fake_rows_cases\n"
    "  .byte 0x8b,0x56,0x5c,0xa1\n"
    "  .long _g_status_mode_ptr\n"
    "  .byte 0x3b,0xd0,0x74,0x05,0xbd,0x40,0x40,0x40,0x80\n"
    "_fake_rows_classes:\n"
    "  .byte 0x00,0x02,0x02,0x02,0x02,0x00,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x01,0x02,0x02,0x02,0x02,0x02,0x02,0x02,0x00\n"
    ".p2align 2\n"
    "_fake_rows_cases:\n"
    "  .long _fake_inventor_rows + 0xa6, _fake_inventor_rows + 0x95, _fake_inventor_rows + 0xa1\n"
    // handover's and trade's gil askers are the sites inside their input
    // routines, as the game's are.
    ".globl _fake_ask_handover_gil\n"
    ".set _fake_ask_handover_gil, _fake_handover_input + 0x51\n"
    ".globl _fake_ask_trade_gil\n"
    ".set _fake_ask_trade_gil, _fake_trade_input + 0xb1\n"
    ".p2align 4\n"
    ".globl _fake_offer_ret\n"
    "_fake_offer_ret:\n"
    "  .byte 0xc3\n"
    ".p2align 4\n"
    ".globl _fake_offer_ret4\n"
    "_fake_offer_ret4:\n"
    "  .byte 0xc2,0x04,0x00\n"

    ".section .rdata,\"dr\"\n"
    ".p2align 2\n"
    ".globl _g_handover_vtable\n"
    "_g_handover_vtable:\n"
    "  .long 0, _fake_handover_open_hook, 0, 0, 0, _fake_offer_ret, _fake_handover_input\n"
    "  .fill 25, 4, 0\n"
    ".text\n"
);

// The other sets' openers, the game's bytes and layout around stand-in
// callees: what the engine's signatures and shapes read at their offsets,
// and each open by name through the open routine; whatever lies between
// them is the stand-ins' own.
asm(
    ".text\n"

    // The map's opener(mode), cdecl: the player data's global tested, the
    // second argument picked by the mode, the position read (nops here: the
    // stand-in's convert takes what is on the stack), then the controller in
    // mapframe's registry slot (row 201) handed to the map's routine with
    // the game's seven arguments.
    ".p2align 4\n"
    ".globl _fake_map_open\n"
    "_fake_map_open:\n"
    "  .byte 0xa1\n"
    "  .long _g_pd_ptr\n"
    "  .byte 0x83,0xec,0x10,0x85,0xc0,0x53,0x56,0x57,0x0f,0x84\n"
    "  .long 9f - . - 4\n"
    "  .byte 0x8b,0x5c,0x24,0x20,0xbf,0x02,0x00,0x00,0x00,0x83,0xfb,0x01,0x74,0x05,0x83,0xfb,0x04,0x75,0x05\n"
    "  .byte 0xbf,0x01,0x00,0x00,0x00,0xb9\n"
    "  .long _g_map_object_ptr\n"
    "  .org _fake_map_open + 0x77, 0x90\n"
    "  .byte 0xa1\n"
    "  .long _g_ctl_global + 4 * 201\n"
    "  .byte 0x85,0xc0,0x74,0x3a,0x8b,0x44,0x24,0x14,0x8b,0x4c,0x24,0x10,0x8b,0x54,0x24,0x0c\n"
    "  .byte 0x6a,0x01,0x6a,0x00,0x6a,0x00,0x57,0x53,0x50,0x51,0x8b,0x0d\n"
    "  .long _g_map_object_ptr\n"
    "  .byte 0x52\n"
    "  call @h_map_convert@20\n"
    "  .byte 0x50,0xa1\n"
    "  .long _g_pd_ptr\n"
    "  .byte 0x8b,0x88,0x10,0x00,0x00,0x00,0x51,0x8b,0x0d\n"
    "  .long _g_ctl_global + 4 * 201\n"
    "  call _fake_map_routine\n"
    "9:\n"
    "  .byte 0x5f,0x5e,0x5b,0x83,0xc4,0x10,0xc3\n"

    // The map's routine(this = mapframe's controller; seven arguments), ret
    // 0x1C: the stand-in's test, then mapframe and map0 by name at the
    // game's offset, the overlap pushed from a cleared ebx.
    ".p2align 4\n"
    ".globl _fake_map_routine\n"
    "_fake_map_routine:\n"
    "  .byte 0x53,0x33,0xdb,0x8d,0x44,0x24,0x08,0x50\n"
    "  call _h_map_routine@4\n"
    "  .byte 0x84,0xc0,0x0f,0x84\n"
    "  .long 9f - . - 4\n"
    "  .byte 0xe9\n"
    "  .long 1f - . - 4\n"
    "  .org _fake_map_routine + 0x791, 0x90\n"
    "1:\n"
    "  .byte 0x53,0x6a,0x01,0x68\n"
    "  .long _g_key_mapframe\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  call _fake_open\n"
    "  .byte 0x53,0x6a,0x01,0x68\n"
    "  .long _g_key_map0\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  call _fake_open\n"
    "9:\n"
    "  .byte 0x5b,0xc2,0x1c,0x00\n"

    // The blue magic set's opener(this = bluequip's controller), plain ret:
    // the jobs known, a blue mage's main or sub job, else the game's
    // refusal 0x57; then persona's mode 2 and persona (row 82), bluehelp's
    // and bluinven's openers on their controllers (rows 153, 152),
    // bluepoin and bluequip.
    ".p2align 4\n"
    ".globl _fake_set_spells_open\n"
    "_fake_set_spells_open:\n"
    "  .byte 0xa0\n"
    "  .long _g_job_flags\n"
    "  .byte 0x56,0xa8,0x01,0x8b,0xf1,0x0f,0x84\n"
    "  .long 8f - . - 4\n"
    "  .byte 0xa0\n"
    "  .long _g_main_job\n"
    "  .byte 0x8a,0x0d\n"
    "  .long _g_sub_job\n"
    "  .byte 0x3c,0x10,0x74,0x0d,0x80,0xf9,0x10,0x0f,0x85\n"
    "  .long 8f - . - 4\n"
    "  .byte 0x3a,0xc1,0x75,0x1d,0xc6,0x46,0x5c,0x00\n"
    "  .org _fake_set_spells_open + 0x78, 0x90\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_ctl_global + 4 * 82\n"
    "  .byte 0x6a,0x00,0x6a,0x01,0xc6,0x41,0x1c,0x02,0x68\n"
    "  .long _g_key_persona\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  call _fake_open\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_ctl_global + 4 * 153\n"
    "  .byte 0x6a,0x00,0x6a,0x00,0x68,0x19,0xfc,0xff,0xff,0x68,0x19,0xfc,0xff,0xff\n"
    "  call @h_bluehelp_open@24\n"
    "  .byte 0x8b,0x15\n"
    "  .long _g_blu_list\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_ctl_global + 4 * 152\n"
    "  .byte 0x68,0x81,0x00,0x00,0x00,0x68,0x19,0xfc,0xff,0xff,0x68,0x19,0xfc,0xff,0xff,0x52\n"
    "  call @h_bluinven_open@24\n"
    "  .byte 0x6a,0x00,0x6a,0x01,0x68\n"
    "  .long _g_key_bluepoin\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  call _fake_open\n"
    "  .byte 0x6a,0x00,0x6a,0x01,0x68\n"
    "  .long _g_key_bluequip\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  call _fake_open\n"
    "  .byte 0x5e,0xc3\n"
    "8:\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_chat_owner\n"
    "  .byte 0x6a,0x57\n"
    "  call @h_set_message@12\n"
    "  .byte 0x5e,0xc3\n"

    // The automaton's opener(this = automato's controller), plain ret: the
    // jobs known, a puppetmaster's main (+0x1C 0) or sub job (1), else the
    // game's refusal 0x57; then bluehelp's and bluinven's openers, statpup
    // and automato.
    ".p2align 4\n"
    ".globl _fake_attachments_open\n"
    "_fake_attachments_open:\n"
    "  .byte 0xf6,0x05\n"
    "  .long _g_job_flags\n"
    "  .byte 0x01,0x74,0x7e,0x8a,0x15\n"
    "  .long _g_main_job\n"
    "  .byte 0xb0,0x12,0x3a,0xd0,0x74,0x6c,0x38,0x05\n"
    "  .long _g_sub_job\n"
    "  .byte 0x75,0x6a,0xc6,0x41,0x1c,0x01,0x8b,0x0d\n"
    "  .long _g_ctl_global + 4 * 153\n"
    "  .byte 0x6a,0x00,0x6a,0x00,0x68,0x19,0xfc,0xff,0xff,0x68,0x19,0xfc,0xff,0xff\n"
    "  call @h_bluehelp_open@24\n"
    "  .byte 0xa1\n"
    "  .long _g_pup_list\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_ctl_global + 4 * 152\n"
    "  .byte 0x68,0x81,0x00,0x00,0x00,0x68,0x19,0xfc,0xff,0xff,0x68,0x19,0xfc,0xff,0xff,0x50\n"
    "  call @h_bluinven_open@24\n"
    "  .byte 0x6a,0x00,0x6a,0x01,0x68\n"
    "  .long _g_key_statpup\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  call _fake_open\n"
    "  .byte 0x6a,0x00,0x6a,0x01,0x68\n"
    "  .long _g_key_automato\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  call _fake_open\n"
    "  .byte 0xc3,0xc6,0x41,0x1c,0x00,0xeb,0x9a,0x8b,0x0d\n"
    "  .long _g_chat_owner\n"
    "  .byte 0x6a,0x57\n"
    "  call @h_set_message@12\n"
    "  .byte 0xc3\n"

    // The main menu's linkshell row, never run: its check (1, 1), then the
    // linkshell list's opener on the controller in link5's registry slot
    // (row 121) with (0, 0, 0).
    ".p2align 4\n"
    ".globl _fake_linkshell_row\n"
    "_fake_linkshell_row:\n"
    "  .byte 0x6a,0x01,0x6a,0x01\n"
    "  call _h_linkshell_check\n"
    "  .byte 0x83,0xc4,0x08,0x84,0xc0,0x74,0x0e\n"
    "  call _h_never\n"
    "  call _h_never\n"
    "  .byte 0x5e,0xc2,0x08,0x00,0x8b,0x0d\n"
    "  .long _g_ctl_global + 4 * 121\n"
    "  .byte 0x6a,0x00,0x6a,0x00,0x6a,0x00\n"
    "  call _fake_linkshell_open\n"
    "  .byte 0x5e,0xc2,0x08,0x00\n"

    // The linkshell list's opener(this = link5's controller; mode, two
    // more), ret 0xC through the open routine: the stand-in's setup, which
    // opens link6, then link5's key, 1 and 0 over its own arguments and a
    // jump to the open routine.
    ".p2align 4\n"
    ".globl _fake_linkshell_open\n"
    "_fake_linkshell_open:\n"
    "  .byte 0x56,0x57,0x8b,0xf1,0xff,0x74,0x24,0x14,0xff,0x74,0x24,0x14,0xff,0x74,0x24,0x14,0x56\n"
    "  call _h_linkshell_open@16\n"
    "  .org _fake_linkshell_open + 0xcf, 0x90\n"
    "  .byte 0x5f,0x5e,0xc7,0x44,0x24,0x0c,0x00,0x00,0x00,0x00,0xc7,0x44,0x24,0x08,0x01,0x00,0x00,0x00\n"
    "  .byte 0xc7,0x44,0x24,0x04\n"
    "  .long _g_key_link5\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe9\n"
    "  .long _fake_open - . - 4\n"

    // The instincts' opener, cdecl, plain ret: the cache's lookups of
    // 0xEFF8..0xEFFE, the prep, mp_stat's opener on its controller (row
    // 355) with 1, mp_cstm's on its (row 362), then mp_mncst by name; the two
    // openers, each its window by name.
    ".p2align 4\n"
    ".globl _fake_instincts_open\n"
    "_fake_instincts_open:\n"
    "  .byte 0x56,0xbe,0xf8,0xef,0x00,0x00,0x8b,0x0d\n"
    "  .long _g_cache_ptr\n"
    "  .byte 0x56\n"
    "  call @h_set_item_data@12\n"
    "  .byte 0x46,0x81,0xfe,0xff,0xef,0x00,0x00,0x72,0xeb\n"
    "  call _h_instincts_prep\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_ctl_global + 4 * 355\n"
    "  .byte 0x6a,0x01\n"
    "  call _fake_mp_stat_open\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_ctl_global + 4 * 362\n"
    "  call _fake_mp_cstm_open\n"
    "  .byte 0x6a,0x00,0x6a,0x01,0x68\n"
    "  .long _g_key_mp_mncst\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  call _fake_open\n"
    "  .byte 0x5e,0xc3\n"
    ".p2align 4\n"
    "_fake_mp_stat_open:\n"
    "  .byte 0x8b,0x44,0x24,0x04,0x6a,0x00,0x89,0x41,0x14,0x6a,0x01,0x68\n"
    "  .long _g_key_mp_stat\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  call _fake_open\n"
    "  .byte 0xc2,0x04,0x00\n"
    ".p2align 4\n"
    "_fake_mp_cstm_open:\n"
    "  .byte 0x6a,0x00,0x6a,0x01,0x68\n"
    "  .long _g_key_mp_cstm\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  call _fake_open\n"
    "  .byte 0xc3\n"

    // The item menu's use, never run: the item's data (its type through the
    // pointer kFakeItemType in), an M or an R tabula the maze editor's
    // opener on prvdungp's controller (row 160), a crystal the check (0x77,
    // 0) and the synthesis's opener on tskill1's (row 118).
    ".p2align 4\n"
    ".globl _fake_item_use\n"
    "_fake_item_use:\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_cache_ptr\n"
    "  .byte 0x33,0xc0,0x66,0x8b,0x06,0x50\n"
    "  call @h_set_item_data@12\n"
    "  .byte 0x85,0xc0,0x0f,0x84\n"
    "  .long _h_never - . - 4\n"
    "  .byte 0x8b,0x88,0xb4,0x01,0x00,0x00,0x66,0x8b,0x09,0x66,0x83,0xf9,0x16,0x75,0x18,0x8b,0x0d\n"
    "  .long _g_ctl_global + 4 * 160\n"
    "  .byte 0x33,0xd2,0x8a,0x56,0x02,0x52,0x6a,0x00\n"
    "  call _fake_tabula_open\n"
    "  .byte 0xe9\n"
    "  .long _h_never - . - 4\n"
    "  .byte 0x66,0x83,0xf9,0x17,0x75,0x18,0x8b,0x0d\n"
    "  .long _g_ctl_global + 4 * 160\n"
    "  .byte 0x33,0xc0,0x8a,0x46,0x02,0x50,0x6a,0x01\n"
    "  call _fake_tabula_open\n"
    "  .byte 0xe9\n"
    "  .long _h_never - . - 4\n"
    "  .byte 0x66,0x83,0xf9,0x08,0x75,0x3d,0x6a,0x00,0x6a,0x77\n"
    "  call _h_use_check\n"
    "  .byte 0x83,0xc4,0x08,0x84,0xc0,0x75,0x0a\n"
    "  call _h_never\n"
    "  .byte 0x5f,0x5e,0xc2,0x08,0x00,0x66,0x8b,0x0e,0x8a,0x56,0x02,0x6a,0x00,0x51,0x8b,0x0d\n"
    "  .long _g_ctl_global + 4 * 118\n"
    "  .byte 0x52\n"
    "  call _fake_synthesis_open\n"
    "  .byte 0xc3\n"

    // The synthesis's opener(this = tskill1's controller; index, id, mode),
    // ret 0xC, al 0 for a refusal: the stand-in's test, which opens inventor
    // and iteminfo, the keep of the list inventor's controller (row 72)
    // holds at +0x5C, then tskill1 by name at the game's offset.
    ".p2align 4\n"
    ".globl _fake_synthesis_open\n"
    "_fake_synthesis_open:\n"
    "  .byte 0x56,0x8b,0xf1,0xff,0x74,0x24,0x10,0xff,0x74,0x24,0x10,0xff,0x74,0x24,0x10,0x56\n"
    "  call _h_synthesis@16\n"
    "  .byte 0x84,0xc0,0x0f,0x84\n"
    "  .long 9f - . - 4\n"
    "  .org _fake_synthesis_open + 0x4f, 0x90\n"
    "  .byte 0x8b,0x15\n"
    "  .long _g_ctl_global + 4 * 72\n"
    "  .byte 0x8b,0x42,0x5c,0x89,0x46,0x4c\n"
    "  .org _fake_synthesis_open + 0xa9, 0x90\n"
    "  .byte 0x6a,0x00,0x6a,0x01,0x68\n"
    "  .long _g_key_tskill1\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  call _fake_open\n"
    "  .byte 0xb0,0x01,0x5e,0xc2,0x0c,0x00\n"
    "9:\n"
    "  .byte 0x32,0xc0,0x5e,0xc2,0x0c,0x00\n"

    // The maze editor's opener(this = prvdungp's controller; mode, index),
    // ret 8: the stand-in's record, then its setup with the game's nine
    // arguments, the mode last read off the stack at the game's offset.
    ".p2align 4\n"
    ".globl _fake_tabula_open\n"
    "_fake_tabula_open:\n"
    "  .byte 0x56,0x57,0x8b,0xf9,0xff,0x74,0x24,0x10,0xff,0x74,0x24,0x10,0x57\n"
    "  call _h_tabula@12\n"
    "  .byte 0x56,0x56,0x56,0x56,0x56,0x56,0x56\n"
    "  .org _fake_tabula_open + 0x38, 0x90\n"
    "  .byte 0x8b,0xcf,0x50,0x8b,0x44,0x24,0x2c,0x50\n"
    "  call _fake_tabula_setup\n"
    "  .byte 0x5f,0x5e,0xc2,0x08,0x00\n"

    // Its setup(this = prvdungp's controller; nine arguments), ret 0x24:
    // prvdungp by name at the game's offset, the overlap pushed from a
    // cleared ebx; the stand-in opens bluehelp and bluinven after it.
    ".p2align 4\n"
    ".globl _fake_tabula_setup\n"
    "_fake_tabula_setup:\n"
    "  .byte 0x53,0x33,0xdb,0x53\n"
    "  .org _fake_tabula_setup + 0x5f, 0x90\n"
    "  .byte 0x6a,0x01,0x68\n"
    "  .long _g_key_prvdungp\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  call _fake_open\n"
    "  call _h_tabula_setup\n"
    "  .byte 0x5b,0xc2,0x24,0x00\n"

    // mcrselop's input routine(this = its controller; event, row), ret 8,
    // never run: mcrselec's controller (row 24) and its set, the row through
    // the jump table, the edit row the macro data's select and the editor's
    // opener on the object its global holds.
    ".p2align 4\n"
    ".globl _fake_mcrselop_input\n"
    "_fake_mcrselop_input:\n"
    "  .byte 0x57,0x8b,0xf9,0x8b,0x0d\n"
    "  .long _g_ctl_global + 4 * 24\n"
    "  .byte 0x85,0xc9,0x0f,0x84\n"
    "  .long 9f - . - 4\n"
    "  .byte 0x66,0x83,0x7c,0x24,0x08,0x05,0x0f,0x85\n"
    "  .long 9f - . - 4\n"
    "  .byte 0x8b,0x51,0x1c,0x33,0xc0,0x8a,0x41,0x6c,0x56,0x8d,0x04,0x80,0x8d,0x34,0x82,0x0f,0xbf,0x44,0x24,0x10\n"
    "  .byte 0x48,0x83,0xf8,0x04,0x0f,0x87\n"
    "  .long 8f - . - 4\n"
    "  .byte 0xff,0x24,0x85\n"
    "  .long _fake_mcrselop_rows\n"
    "  .byte 0x8b,0x4f,0x14,0x56\n"
    "  call @h_macro_select@12\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_macro_editor_ptr\n"
    "  call _fake_macro_editor_open\n"
    "8:\n"
    "  .byte 0x5e\n"
    "9:\n"
    "  .byte 0x5f,0xc2,0x08,0x00\n"
    ".p2align 2\n"
    "_fake_mcrselop_rows:\n"
    "  .long _fake_mcrselop_input + 0x42, 8b, 8b, 8b, 8b\n"

    // The macro editor's opener(this = the editor object), plain ret:
    // mcrbedit, then the two palettes, the one +0x0C names last.
    ".p2align 4\n"
    ".globl _fake_macro_editor_open\n"
    "_fake_macro_editor_open:\n"
    "  .byte 0x8a,0x41,0x0c,0x6a,0x00,0x6a,0x01,0x3c,0x01,0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0x68\n"
    "  .long _g_key_mcrbedit\n"
    "  .byte 0x75,0x2c\n"
    "  call _fake_open\n"
    "  .byte 0x6a,0x00,0x6a,0x01,0x68\n"
    "  .long _g_key_mcr2edit\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  call _fake_open\n"
    "  .byte 0x6a,0x00,0x6a,0x01,0x68\n"
    "  .long _g_key_mcr1edit\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  call _fake_open\n"
    "  .byte 0xc3\n"
    "  call _fake_open\n"
    "  .byte 0x6a,0x00,0x6a,0x01,0x68\n"
    "  .long _g_key_mcr1edit\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  call _fake_open\n"
    "  .byte 0x6a,0x00,0x6a,0x01,0x68\n"
    "  .long _g_key_mcr2edit\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  call _fake_open\n"
    "  .byte 0xc3\n"
);

extern "C" // The routes' sites and routines, the game's bytes and layout around
// stand-in callees: those the engine runs are the game's bytes where they
// are small, each open by name through the open routine and each close
// through close by name; the larger ones are stand-ins with the bytes the
// engine reads at the game's offsets after their entries.
asm(
    ".text\n"

    // The storage list's opener(this = storage's or storage2's controller;
    // mode), ret 4, the game's bytes: the mode at +0x14, then storage for 0
    // and storage2 for 1 by name.
    ".p2align 4\n"
    ".globl _fake_storage_open\n"
    "_fake_storage_open:\n"
    "  .byte 0x8b,0x44,0x24,0x04,0x6a,0x00,0x89,0x41,0x14,0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0x85,0xc0,0x6a,0x01,0x75,0x0d,0x68\n"
    "  .long _g_key_storage\n"
    "  .byte 0xe8\n"
    "  .long _fake_open - . - 4\n"
    "  .byte 0xc2,0x04,0x00,0x68\n"
    "  .long _g_key_storage2\n"
    "  .byte 0xe8\n"
    "  .long _fake_open - . - 4\n"
    "  .byte 0xc2,0x04,0x00\n"

    // The post menu's opener(this = mogpost's controller; mode), ret 4, the
    // game's bytes, as mp_stat's.
    ".p2align 4\n"
    ".globl _fake_mogpost_open\n"
    "_fake_mogpost_open:\n"
    "  .byte 0x8b,0x44,0x24,0x04,0x6a,0x00,0x89,0x41,0x14,0x6a,0x01,0x68\n"
    "  .long _g_key_mogpost\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe8\n"
    "  .long _fake_open - . - 4\n"
    "  .byte 0xc2,0x04,0x00\n"

    // The job change menu's main and support rows, never run: the row into
    // jobcselu's controller (row 117) at +0x14, then the open by the key the
    // language's lookup answers.
    ".p2align 4\n"
    ".globl _fake_job_change_row\n"
    "_fake_job_change_row:\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_ctl_global + 4 * 117\n"
    "  .byte 0x85,0xc9,0x74,0x03,0x89,0x41,0x14,0x6a,0x00,0x6a,0x01,0x6a,0x06,0xe8\n"
    "  .long _h_language_key - . - 4\n"
    "  .byte 0x83,0xc4,0x04,0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0x50,0xe8\n"
    "  .long _fake_open - . - 4\n"
    "  .byte 0xc2,0x08,0x00\n"

    // Its confirm, never run: the Mog House's and the Nomad Moogle service's
    // managers' globals, the row read at +0x14.
    ".p2align 4\n"
    ".globl _fake_job_change_confirm\n"
    "_fake_job_change_confirm:\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_mog_house_ptr\n"
    "  .byte 0x85,0xc9,0x74,0x1a,0x83,0x7e,0x14,0x01,0x75,0x0a,0x6a,0x00,0x50,0xe8\n"
    "  .long _h_never - . - 4\n"
    "  .byte 0xeb,0x27,0x50,0x6a,0x00,0xe8\n"
    "  .long _h_never - . - 4\n"
    "  .byte 0xeb,0x1d,0x8b,0x0d\n"
    "  .long _g_nomad_ptr\n"
    "  .byte 0x85,0xc9,0x74,0x13,0x83,0x7e,0x14,0x01,0xc3\n"

    // The friend list's text command, never run: the friend list's opener on
    // friend's controller (row 171) with 0.
    ".p2align 4\n"
    ".globl _fake_friend_command\n"
    "_fake_friend_command:\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_ctl_global + 4 * 171\n"
    "  .byte 0x85,0xc9,0x74,0x0e,0x8b,0x41,0x08,0x85,0xc0,0x75,0x07,0x6a,0x00,0xe8\n"
    "  .long _fake_friend_open - . - 4\n"
    "  .byte 0x8b,0x44,0x24,0x0c,0xc3\n"

    // The friend list's opener(this = friend's controller; 0), ret 4: the
    // stand-in, which opens friend and titlehan, then at the game's offset its
    // tail's bytes, never run.
    ".p2align 4\n"
    ".globl _fake_friend_open\n"
    "_fake_friend_open:\n"
    "  .byte 0xe9\n"
    "  .long @h_friend_open@12 - . - 4\n"
    "  .org _fake_friend_open + 0xbf, 0x90\n"
    "  .byte 0x6a,0x00,0x6a,0x01,0x68\n"
    "  .long _g_key_friend\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe8\n"
    "  .long _fake_open - . - 4\n"
    "  .byte 0x8b,0x46,0x08,0x5f,0x5e,0x5b,0x85,0xc0,0x74,0x1c,0xa1\n"
    "  .long _g_ctl_global + 4 * 60\n"
    "  .byte 0x85,0xc0,0x74,0x13,0x6a,0x00,0x6a,0x01,0x68\n"
    "  .long _g_key_titlehan\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe8\n"
    "  .long _fake_open - . - 4\n"
    "  .byte 0xc2,0x04,0x00\n"

    // The friend menu's Messages row, never run: the mail list's opener on
    // msglist's controller (row 178) with (0, 0, 0).
    ".p2align 4\n"
    ".globl _fake_messages_row\n"
    "_fake_messages_row:\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_ctl_global + 4 * 178\n"
    "  .byte 0x85,0xc9,0x74,0x2b,0x6a,0x00,0x6a,0x00,0x6a,0x00,0xe8\n"
    "  .long _fake_msglist_open - . - 4\n"
    "  .byte 0x84,0xc0,0x75,0x1c,0x8b,0x0d\n"
    "  .long _g_chat_owner\n"
    "  .byte 0x6a,0x7a,0xe8\n"
    "  .long _h_never - . - 4\n"
    "  .byte 0xc3\n"

    // The mail list's opener(this = msglist's controller; three ids), ret 0xC,
    // al: the stand-in, then its tail's bytes, never run.
    ".p2align 4\n"
    ".globl _fake_msglist_open\n"
    "_fake_msglist_open:\n"
    "  .byte 0xe9\n"
    "  .long @h_msglist_open@20 - . - 4\n"
    "  .org _fake_msglist_open + 0x76, 0x90\n"
    "  .byte 0x6a,0x00,0x6a,0x01,0x68\n"
    "  .long _g_key_msglist\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe8\n"
    "  .long _fake_open - . - 4\n"
    "  .byte 0xa1\n"
    "  .long _g_ctl_global + 4 * 60\n"
    "  .byte 0x85,0xc0,0x74,0x13,0x6a,0x00,0x6a,0x01,0x68\n"
    "  .long _g_key_titlehan\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe8\n"
    "  .long _fake_open - . - 4\n"
    "  .byte 0xb0,0x01,0xc2,0x0c,0x00,0x32,0xc0,0xc2,0x0c,0x00\n"

    // The blacklist command's call, never run: the blacklist's opener on
    // blklist's controller (row 182).
    ".p2align 4\n"
    ".globl _fake_blacklist_command\n"
    "_fake_blacklist_command:\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_ctl_global + 4 * 182\n"
    "  .byte 0xe8\n"
    "  .long _fake_blklist_open - . - 4\n"
    "  .byte 0x8b,0x44,0x24,0x14,0x5f,0x5e,0xc3\n"

    // The blacklist's opener(this = blklist's controller), plain ret: the
    // stand-in, then its tail's bytes, never run.
    ".p2align 4\n"
    ".globl _fake_blklist_open\n"
    "_fake_blklist_open:\n"
    "  .byte 0xe9\n"
    "  .long @h_blklist_open@8 - . - 4\n"
    "  .org _fake_blklist_open + 0x47, 0x90\n"
    "  .byte 0x6a,0x00,0x6a,0x01,0x68\n"
    "  .long _g_key_blklist\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe8\n"
    "  .long _fake_open - . - 4\n"
    "  .byte 0x5e,0xc3\n"

    // The mount list's opener(list, sort), cdecl, the game's bytes: abisortw
    // for a sort of 1, mount, its setup on mount's controller (row 259), then
    // abisortw's controller's (row 256) +0x14 set 2.
    ".p2align 4\n"
    ".globl _fake_mount_open\n"
    "_fake_mount_open:\n"
    "  .byte 0x80,0x7c,0x24,0x08,0x01,0x75,0x13,0x6a,0x00,0x6a,0x01,0x68\n"
    "  .long _g_key_abisortw\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe8\n"
    "  .long _fake_open - . - 4\n"
    "  .byte 0x6a,0x00,0x6a,0x01,0x68\n"
    "  .long _g_key_mount\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe8\n"
    "  .long _fake_open - . - 4\n"
    "  .byte 0x8b,0x44,0x24,0x04,0x8b,0x0d\n"
    "  .long _g_ctl_global + 4 * 259\n"
    "  .byte 0x50,0xe8\n"
    "  .long _fake_mount_setup - . - 4\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_ctl_global + 4 * 256\n"
    "  .byte 0xc7,0x41,0x14,0x02,0x00,0x00,0x00,0xc3\n"

    // Its setup(this = mount's controller; list), ret 4, the game's bytes
    // around a stand-in build: subwindo's controller (row 62) set up and
    // subwindo opened.
    ".p2align 4\n"
    ".globl _fake_mount_setup\n"
    "_fake_mount_setup:\n"
    "  .byte 0x8b,0x41,0x68,0x53,0x33,0xdb,0x50,0x89,0x59,0x64,0xe8\n"
    "  .long @h_mount_build@12 - . - 4\n"
    "  .byte 0xa1\n"
    "  .long _g_ctl_global + 4 * 62\n"
    "  .byte 0x53,0x6a,0x01,0x68\n"
    "  .long _g_key_subwindo\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xc7,0x40,0x28,0x0a,0x00,0x00,0x00,0x89,0x58,0x20,0x89,0x58,0x24,0x88,0x58,0x30,0x89,0x58,0x34,0xc7,0x40,0x2c,0xe8,0x03\n"
    "  .byte 0x00,0x00,0xe8\n"
    "  .long _fake_open - . - 4\n"
    "  .byte 0x5b,0xc2,0x04,0x00\n"

    // The Job Points list's test of a job, ret 4, the game's bytes around a
    // stand-in level read.
    ".p2align 4\n"
    ".globl _fake_job_points_test\n"
    "_fake_job_points_test:\n"
    "  .byte 0x8b,0x44,0x24,0x04,0x3c,0x01,0x72,0x15,0x3c,0x16,0x77,0x11,0x50,0xe8\n"
    "  .long _h_job_level - . - 4\n"
    "  .byte 0x83,0xc4,0x04,0x3c,0x63,0x0f,0x93,0xc0,0xc2,0x04,0x00,0x32,0xc0,0xc2,0x04,0x00\n"

    // jbpcat's opener(this = its controller; job), ret 4, the game's bytes.
    ".p2align 4\n"
    ".globl _fake_job_points_open\n"
    "_fake_job_points_open:\n"
    "  .byte 0x8a,0x44,0x24,0x04,0x6a,0x00,0x88,0x41,0x54,0xc7,0x41,0x58,0x00,0x00,0x00,0x00,0x6a,0x01,0x68\n"
    "  .long _g_key_jbpcat\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe8\n"
    "  .long _fake_open - . - 4\n"
    "  .byte 0xc2,0x04,0x00\n"

    // fp_cat2's opener(this = its controller; category), ret 4, the game's
    // bytes.
    ".p2align 4\n"
    ".globl _fake_fp_cat2_open\n"
    "_fake_fp_cat2_open:\n"
    "  .byte 0x8a,0x44,0x24,0x04,0x6a,0x00,0x88,0x41,0x50,0x6a,0x01,0x68\n"
    "  .long _g_key_fp_cat2\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe8\n"
    "  .long _fake_open - . - 4\n"
    "  .byte 0xc2,0x04,0x00\n"

    // The main menu's Search row, never run: the search reset on scresult's
    // controller (row 135) with 0, then searchma's open.
    ".p2align 4\n"
    ".globl _fake_search_row\n"
    "_fake_search_row:\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_ctl_global + 4 * 135\n"
    "  .byte 0x85,0xc9,0x74,0x07,0x6a,0x00,0xe8\n"
    "  .long @h_search_reset@12 - . - 4\n"
    "  .byte 0x6a,0x00,0x6a,0x01,0x68\n"
    "  .long _g_key_searchma\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe8\n"
    "  .long _fake_open - . - 4\n"
    "  .byte 0xc3\n"

    // The results list's narrow row, never run: scsibori only while the
    // results' mode (+0x58) is 1 or 2.
    ".p2align 4\n"
    ".globl _fake_results_narrow\n"
    "_fake_results_narrow:\n"
    "  .byte 0x8b,0x76,0x58,0x83,0xfe,0x01,0x74,0x0c,0x83,0xfe,0x02,0x74,0x07,0x5f,0x32,0xc0,0x5e,0xc2,0x04,0x00,0x6a,0x00,0x6a,0x01\n"
    "  .byte 0x68\n"
    "  .long _g_key_scsibori\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe8\n"
    "  .long _fake_open - . - 4\n"
    "  .byte 0x5f,0xb0,0x01,0x5e,0xc2,0x04,0x00\n"

    // comgenre's opener(this = its controller; mode), ret 4, the game's bytes.
    ".p2align 4\n"
    ".globl _fake_comgenre_open\n"
    "_fake_comgenre_open:\n"
    "  .byte 0x66,0x8b,0x44,0x24,0x04,0x66,0x85,0xc0,0x7c,0x1d,0x66,0x3d,0x03,0x00,0x7d,0x17,0x6a,0x00,0x66,0x89,0x41,0x14,0x6a,0x01\n"
    "  .byte 0x68\n"
    "  .long _g_key_comgenre\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe8\n"
    "  .long _fake_open - . - 4\n"
    "  .byte 0xc2,0x04,0x00\n"

    // The comment sub-lists' openers(this = the list's controller; mode,
    // base), ret 8, the game's bytes, alike but for the key.
    ".p2align 4\n"
    ".globl _fake_sccompar_open\n"
    "_fake_sccompar_open:\n"
    "  .byte 0x8b,0x44,0x24,0x04,0x83,0xf8,0x03,0x73,0x1d,0x89,0x41,0x14,0x8b,0x44,0x24,0x08,0x6a,0x00,0x89,0x41,0x18,0x6a,0x01,0x68\n"
    "  .long _g_key_sccompar\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe8\n"
    "  .long _fake_open - . - 4\n"
    "  .byte 0xc2,0x08,0x00\n"

    ".p2align 4\n"
    ".globl _fake_sccomite_open\n"
    "_fake_sccomite_open:\n"
    "  .byte 0x8b,0x44,0x24,0x04,0x83,0xf8,0x03,0x73,0x1d,0x89,0x41,0x14,0x8b,0x44,0x24,0x08,0x6a,0x00,0x89,0x41,0x18,0x6a,0x01,0x68\n"
    "  .long _g_key_sccomite\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe8\n"
    "  .long _fake_open - . - 4\n"
    "  .byte 0xc2,0x08,0x00\n"

    ".p2align 4\n"
    ".globl _fake_sccomls_open\n"
    "_fake_sccomls_open:\n"
    "  .byte 0x8b,0x44,0x24,0x04,0x83,0xf8,0x03,0x73,0x1d,0x89,0x41,0x14,0x8b,0x44,0x24,0x08,0x6a,0x00,0x89,0x41,0x18,0x6a,0x01,0x68\n"
    "  .long _g_key_sccomls\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe8\n"
    "  .long _fake_open - . - 4\n"
    "  .byte 0xc2,0x08,0x00\n"

    // The inventory's cursor, the compare option's panel, never run: the item
    // panel's set-and-open on itemxinf's controller (row 74) beside equip's
    // (41) and inventor's (72), the record's extra data at +0x10.
    ".p2align 4\n"
    ".globl _fake_compare_site\n"
    "_fake_compare_site:\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_ctl_global + 4 * 72\n"
    "  .byte 0x8b,0x15\n"
    "  .long _g_ctl_global + 4 * 41\n"
    "  .byte 0x51,0x8b,0x0d\n"
    "  .long _g_ctl_global + 4 * 74\n"
    "  .byte 0x83,0xc6,0x10,0x52,0x56,0x50,0x53,0x55,0x6a,0x05,0xe8\n"
    "  .long @h_item_panel@36 - . - 4\n"
    "  .byte 0xc3\n"

    // The timers' openers(this = the timer's controller; kind, ticks), ret 8,
    // the game's bytes, alike but for the key.
    ".p2align 4\n"
    ".globl _fake_casttime_open\n"
    "_fake_casttime_open:\n"
    "  .byte 0x8b,0x44,0x24,0x04,0x89,0x41,0x20,0x8b,0x44,0x24,0x08,0x85,0xc0,0x7d,0x08,0xc7,0x44,0x24,0x08,0x00,0x00,0x00,0x00,0xdb\n"
    "  .byte 0x44,0x24,0x08,0x6a,0x00,0x6a,0x01,0x68\n"
    "  .long _g_key_casttime\n"
    "  .byte 0xd9,0x51,0x18,0xd9,0x59,0x14,0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe8\n"
    "  .long _fake_open - . - 4\n"
    "  .byte 0xc2,0x08,0x00\n"

    ".p2align 4\n"
    ".globl _fake_eventtim_open\n"
    "_fake_eventtim_open:\n"
    "  .byte 0x8b,0x44,0x24,0x04,0x89,0x41,0x20,0x8b,0x44,0x24,0x08,0x85,0xc0,0x7d,0x08,0xc7,0x44,0x24,0x08,0x00,0x00,0x00,0x00,0xdb\n"
    "  .byte 0x44,0x24,0x08,0x6a,0x00,0x6a,0x01,0x68\n"
    "  .long _g_key_eventtim\n"
    "  .byte 0xd9,0x51,0x18,0xd9,0x59,0x14,0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe8\n"
    "  .long _fake_open - . - 4\n"
    "  .byte 0xc2,0x08,0x00\n"

    // The main menu's Status row, never run: in Monstrosity mp_stat's opener
    // on its controller (row 355) with 0, else persona's (row 82) layout +0x1C
    // set 1 and persona's open.
    ".p2align 4\n"
    ".globl _fake_status_row\n"
    "_fake_status_row:\n"
    "  .byte 0xe8\n"
    "  .long _fake_monstrosity - . - 4\n"
    "  .byte 0x84,0xc0,0x6a,0x00,0x74,0x14,0x8b,0x0d\n"
    "  .long _g_ctl_global + 4 * 355\n"
    "  .byte 0xe8\n"
    "  .long _fake_mp_stat_open - . - 4\n"
    "  .byte 0xe8\n"
    "  .long _h_never - . - 4\n"
    "  .byte 0x5e,0xc2,0x08,0x00,0xa1\n"
    "  .long _g_ctl_global + 4 * 82\n"
    "  .byte 0x6a,0x01,0x68\n"
    "  .long _g_key_persona\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xc6,0x40,0x1c,0x01,0xe8\n"
    "  .long _fake_open - . - 4\n"
    "  .byte 0xc3\n"

    // The configuration menu's message rows, never run: conf11l's (row 277)
    // set +0x14 written 1 or 0, then conf11l's open.
    ".p2align 4\n"
    ".globl _fake_conf11m_rows\n"
    "_fake_conf11m_rows:\n"
    "  .byte 0x8b,0x15\n"
    "  .long _g_ctl_global + 4 * 277\n"
    "  .byte 0xc7,0x42,0x14,0x01,0x00,0x00,0x00,0xeb,0x0c,0xa1\n"
    "  .long _g_ctl_global + 4 * 277\n"
    "  .byte 0xc7,0x40,0x14,0x00,0x00,0x00,0x00,0x6a,0x00,0x6a,0x01,0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0x68\n"
    "  .long _g_key_conf11l\n"
    "  .byte 0xe8\n"
    "  .long _fake_open - . - 4\n"
    "  .byte 0xc3\n"

    // The quest log command, never run: quest00's (row 246) mode +0x68
    // written 0, then quest00's open.
    ".p2align 4\n"
    ".globl _fake_quest_command\n"
    "_fake_quest_command:\n"
    "  .byte 0xa1\n"
    "  .long _g_ctl_global + 4 * 246\n"
    "  .byte 0x85,0xc0,0x74,0x07,0xc7,0x40,0x68,0x00,0x00,0x00,0x00,0x6a,0x00,0x6a,0x01,0x68\n"
    "  .long _g_key_quest00\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe8\n"
    "  .long _fake_open - . - 4\n"
    "  .byte 0x8b,0x44,0x24,0x0c,0xc3\n"

    // quest00's confirm, never run: its two stand-in calls on quest00's
    // controller, quest01's open, then quest01's setup on its controller (row
    // 247).
    ".p2align 4\n"
    ".globl _fake_quest00_confirm\n"
    "_fake_quest00_confirm:\n"
    "  .byte 0x8b,0xce,0xe8\n"
    "  .long @h_quest00_dim@8 - . - 4\n"
    "  .byte 0x8b,0xce,0xe8\n"
    "  .long @h_quest00_dim2@8 - . - 4\n"
    "  .byte 0x6a,0x00,0x6a,0x01,0x68\n"
    "  .long _g_key_quest01\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe8\n"
    "  .long _fake_open - . - 4\n"
    "  .byte 0x8b,0x56,0x30,0x8b,0x0d\n"
    "  .long _g_ctl_global + 4 * 247\n"
    "  .byte 0x52,0xe8\n"
    "  .long @h_quest01_setup@12 - . - 4\n"
    "  .byte 0x5e,0xc3\n"

    // The missions command, never run: on miss00's controller (row 248) the
    // setter with 0 or the log by name, then miss00's open.
    ".p2align 4\n"
    ".globl _fake_missions_command\n"
    "_fake_missions_command:\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_ctl_global + 4 * 248\n"
    "  .byte 0x85,0xc9,0x74,0x34,0xa1\n"
    "  .long _g_nation\n"
    "  .byte 0x83,0xe8,0x02,0x74,0x09,0x6a,0x00,0xe8\n"
    "  .long _fake_missions_setter - . - 4\n"
    "  .byte 0xeb,0x0e,0xa1\n"
    "  .long _g_nation\n"
    "  .byte 0x8b,0x50,0x04,0x52,0xe8\n"
    "  .long _fake_missions_by_name - . - 4\n"
    "  .byte 0x6a,0x00,0x6a,0x01,0x68\n"
    "  .long _g_key_miss00\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe8\n"
    "  .long _fake_open - . - 4\n"
    "  .byte 0x8b,0x44,0x24,0x0c,0xc3\n"

    // The missions menu's setter(this = miss00's controller; value), ret 4,
    // the game's bytes: past 6 (0 among them) the player's nation's log at
    // +0x70.
    ".p2align 4\n"
    ".globl _fake_missions_setter\n"
    "_fake_missions_setter:\n"
    "  .byte 0x8b,0x44,0x24,0x04,0x56,0x48,0x8b,0xf1,0x83,0xf8,0x05,0x77,0x60,0xff,0x24,0x85\n"
    "  .long _g_mission_cases\n"
    "  .byte 0xc7,0x46,0x70,0x03,0x00,0x00,0x00,0x5e,0xc2,0x04,0x00\n"
    "  .org _fake_missions_setter + 0x6d, 0x90\n"
    "  .byte 0x0f,0xbe,0x0d\n"
    "  .long _g_nation\n"
    "  .byte 0x89,0x4e,0x70,0x5e,0xc2,0x04,0x00\n"

    // The log by its name, never run: each log's expansion bit tested.
    ".p2align 4\n"
    ".globl _fake_missions_by_name\n"
    "_fake_missions_by_name:\n"
    "  .byte 0x53,0x55,0x8b,0x6c,0x24,0x0c,0x56,0x57,0x8b,0xd1,0x33,0xff,0x0f,0xbe,0x87\n"
    "  .long _g_mission_bits\n"
    "  .byte 0x50,0xe8\n"
    "  .long _h_expansion - . - 4\n"
    "  .byte 0x83,0xc4,0x04,0x84,0xc0,0x74,0x3a,0xc3\n"

    // map0's Wide Scan row, never run: while the map tracks a yes/no on
    // comyn's controller (row 237), the game's message asked through the
    // yes/no class's ask with a stand-in callback, else scanlist's opener on
    // its controller (row 204).
    ".p2align 4\n"
    ".globl _fake_wide_scan_row\n"
    "_fake_wide_scan_row:\n"
    "  .byte 0xe8\n"
    "  .long _h_tracking - . - 4\n"
    "  .byte 0x85,0xc0,0x74,0x2f,0x83,0x38,0x01,0x75,0x2a,0x6a,0x00,0x6a,0x01,0x6a,0x00,0x6a,0x00,0x6a,0x45,0x6a,0x0e,0xe8\n"
    "  .long _h_message - . - 4\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_ctl_global + 4 * 237\n"
    "  .byte 0x83,0xc4,0x08,0x50,0x6a,0x00,0x68\n"
    "  .long _h_scan_question\n"
    "  .byte 0xe8\n"
    "  .long _fake_yesno_ask_site - . - 4\n"
    "  .byte 0xc2,0x08,0x00,0x8b,0x0d\n"
    "  .long _g_ctl_global + 4 * 204\n"
    "  .byte 0xe8\n"
    "  .long _fake_scanlist_open - . - 4\n"
    "  .byte 0xc2,0x08,0x00\n"

    // scanlist's opener(this = its controller), plain ret, the game's bytes
    // around a stand-in list format.
    ".p2align 4\n"
    ".globl _fake_scanlist_open\n"
    "_fake_scanlist_open:\n"
    "  .byte 0x56,0x6a,0x0a,0x8b,0xf1,0xe8\n"
    "  .long @h_list_format@12 - . - 4\n"
    "  .byte 0x6a,0x00,0x6a,0x01,0x68\n"
    "  .long _g_key_scanlist\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0x66,0xc7,0x46,0x16,0x03,0x00,0xc7,0x46,0x30\n"
    "  .long _g_nation\n"
    "  .byte 0x66,0xc7,0x46,0x26,0x18,0x00,0xe8\n"
    "  .long _fake_open - . - 4\n"
    "  .byte 0x5e,0xc3\n"

    // The conquest map's text command, never run: the Region Info opener on
    // cnqframe's controller (row 207), closed, with the area at +0x40 of the
    // player's data.
    ".p2align 4\n"
    ".globl _fake_region_command\n"
    "_fake_region_command:\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_ctl_global + 4 * 207\n"
    "  .byte 0x85,0xc9,0x74,0x1e,0xa1\n"
    "  .long _g_route_player_ptr\n"
    "  .byte 0x85,0xc0,0x74,0x15,0x8b,0x51,0x08,0x85,0xd2,0x75,0x0e,0x8b,0x80,0x40,0x00,0x00,0x00,0x50,0x6a,0x00,0xe8\n"
    "  .long _fake_region_open - . - 4\n"
    "  .byte 0x8b,0x44,0x24,0x0c,0xc3\n"

    // The Region Info opener(this = cnqframe's controller; kind, area), ret 8:
    // the stand-in, then the kind's write at the game's offset, never run.
    ".p2align 4\n"
    ".globl _fake_region_open\n"
    "_fake_region_open:\n"
    "  .byte 0xe9\n"
    "  .long @h_region_open@16 - . - 4\n"
    "  .org _fake_region_open + 0x5e, 0x90\n"
    "  .byte 0x89,0x9e,0x60,0x01,0x00,0x00,0xc3\n"

    // cnqframe's Colonization cursor, never run: the region at coloresu's
    // (row 218) +0x18, coloresu's open, colorank's close.
    ".p2align 4\n"
    ".globl _fake_coloresu_row\n"
    "_fake_coloresu_row:\n"
    "  .byte 0xa1\n"
    "  .long _g_ctl_global + 4 * 218\n"
    "  .byte 0x85,0xc0,0x74,0x06,0x0f,0xbf,0xd7,0x89,0x50,0x18,0x6a,0x00,0x6a,0x01,0x68\n"
    "  .long _g_key_coloresu\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe8\n"
    "  .long _fake_open - . - 4\n"
    "  .byte 0x68\n"
    "  .long _g_key_colorank\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe8\n"
    "  .long _fake_close - . - 4\n"
    "  .byte 0xc3\n"

    // The text color menu's rows, never run: the category written into
    // textcol1's, textcol2's and textcol3's controllers (rows 272 to 274), then
    // at the game's offset textcol2's and textcol1's opens.
    ".p2align 4\n"
    ".globl _fake_text_color_rows\n"
    "_fake_text_color_rows:\n"
    "  .byte 0xa1\n"
    "  .long _g_ctl_global + 4 * 272\n"
    "  .byte 0xc7,0x40,0x14,0x00,0x00,0x00,0x00,0x8b,0x0d\n"
    "  .long _g_ctl_global + 4 * 273\n"
    "  .byte 0xc7,0x41,0x1c,0x00,0x00,0x00,0x00,0x8b,0x15\n"
    "  .long _g_ctl_global + 4 * 274\n"
    "  .byte 0xc7,0x42,0x18,0x00,0x00,0x00,0x00,0xeb,0x6a\n"
    "  .org _fake_text_color_rows + 0x92, 0x90\n"
    "  .byte 0x6a,0x00,0x6a,0x01,0x68\n"
    "  .long _g_key_textcol2\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe8\n"
    "  .long _fake_open - . - 4\n"
    "  .byte 0x6a,0x00,0x6a,0x01,0x68\n"
    "  .long _g_key_textcol1\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe8\n"
    "  .long _fake_open - . - 4\n"
    "  .byte 0xc2,0x08,0x00\n"

    // The macro editor's pick(this = the editor; row), ret 4, the game's
    // bytes: the row for the palette, the palettes closed, mcrledit and macro
    // opened.
    ".p2align 4\n"
    ".globl _fake_macro_pick\n"
    "_fake_macro_pick:\n"
    "  .byte 0x80,0x79,0x0c,0x01,0x75,0x09,0x8a,0x44,0x24,0x04,0x88,0x41,0x0d,0xeb,0x07,0x8a,0x54,0x24,0x04,0x88,0x51,0x0e,0x68\n"
    "  .long _g_key_mcrbedit\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe8\n"
    "  .long _fake_close - . - 4\n"
    "  .byte 0x68\n"
    "  .long _g_key_mcr1edit\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe8\n"
    "  .long _fake_close - . - 4\n"
    "  .byte 0x68\n"
    "  .long _g_key_mcr2edit\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe8\n"
    "  .long _fake_close - . - 4\n"
    "  .byte 0x6a,0x00,0x6a,0x01,0x68\n"
    "  .long _g_key_mcrledit\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe8\n"
    "  .long _fake_open - . - 4\n"
    "  .byte 0x6a,0x00,0x6a,0x01,0x68\n"
    "  .long _g_key_macro\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe8\n"
    "  .long _fake_open - . - 4\n"
    "  .byte 0xc2,0x04,0x00\n"

    // mcr1edit's and mcr2edit's Escape, never run: the editor's close, then
    // titlewin's (row 30) step back (0, 2).
    ".p2align 4\n"
    ".globl _fake_palette_escape_1\n"
    "_fake_palette_escape_1:\n"
    "  .byte 0x8b,0x4e,0x24,0xe8\n"
    "  .long _fake_macro_close - . - 4\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_ctl_global + 4 * 30\n"
    "  .byte 0x5e,0x5b,0xc7,0x44,0x24,0x0c,0x02,0x00,0x00,0x00,0x83,0xc4,0x04,0xc7,0x44,0x24,0x04,0x00,0x00,0x00,0x00,0xe9\n"
    "  .long @h_title@16 - . - 4\n"

    ".p2align 4\n"
    ".globl _fake_palette_escape_2\n"
    "_fake_palette_escape_2:\n"
    "  .byte 0x8b,0x4e,0x24,0xe8\n"
    "  .long _fake_macro_close - . - 4\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_ctl_global + 4 * 30\n"
    "  .byte 0x5e,0x5b,0xc7,0x44,0x24,0x0c,0x02,0x00,0x00,0x00,0x83,0xc4,0x04,0xc7,0x44,0x24,0x04,0x00,0x00,0x00,0x00,0xe9\n"
    "  .long @h_title@16 - . - 4\n"

    // The macro editor's close(this = the editor), plain ret, the game's bytes.
    ".p2align 4\n"
    ".globl _fake_macro_close\n"
    "_fake_macro_close:\n"
    "  .byte 0x68\n"
    "  .long _g_key_mcrbedit\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe8\n"
    "  .long _fake_close - . - 4\n"
    "  .byte 0x68\n"
    "  .long _g_key_mcr1edit\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe8\n"
    "  .long _fake_close - . - 4\n"
    "  .byte 0x68\n"
    "  .long _g_key_mcr2edit\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe8\n"
    "  .long _fake_close - . - 4\n"
    "  .byte 0xc3\n"

    // The main menu's Items row, never run: the item view (0, 1), inventor's
    // (row 72) +0x90 set 1, then inventor's open with the list its global
    // holds.
    ".p2align 4\n"
    ".globl _fake_items_row\n"
    "_fake_items_row:\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_items_object_ptr\n"
    "  .byte 0x6a,0x01,0x6a,0x00,0xe8\n"
    "  .long @h_items_view@16 - . - 4\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_ctl_global + 4 * 72\n"
    "  .byte 0xc6,0x81,0x90,0x00,0x00,0x00,0x01,0x8b,0x15\n"
    "  .long _g_policy_ptr\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_ctl_global + 4 * 72\n"
    "  .byte 0x52,0xe8\n"
    "  .long @h_inventor_open@12 - . - 4\n"
    "  .byte 0xc3\n"

    // The item data the inventory list's rows read(this = the cache; id), ret
    // 4: the stand-in, then the call of the summary's builder, never run,
    // whose copy of the type word (from the data's +0x1B4) the engine reads.
    ".p2align 4\n"
    ".globl _fake_item_summary\n"
    "_fake_item_summary:\n"
    "  .byte 0xe9\n"
    "  .long @h_item_data@12 - . - 4\n"
    "  .org _fake_item_summary + 0x10, 0x90\n"
    "  .byte 0x6a,0x00,0x51,0xe8\n"
    "  .long _fake_summary_builder - . - 4\n"
    "  .byte 0xc3\n"

    ".p2align 4\n"
    ".globl _fake_summary_builder\n"
    "_fake_summary_builder:\n"
    "  .byte 0xc3\n"
    "  .org _fake_summary_builder + 0x88, 0x90\n"
    "  .byte 0x8b,0x86,0xb4,0x01,0x00,0x00,0x85,0xc0,0x74,0x07,0x66,0x8b,0x00,0x66,0x89,0x43,0x04,0xc3\n"

);

// The Escape routes' and the palette switch's routines and sites, and
// statcom2's vtable, in read-only data as the game's: its begin-close and
// its input routine; and cnqframe's.
#define HU_FAKE_CLOSE(nm) "  .byte 0x68\n  .long _g_key_" nm "\n  .byte 0xb9\n  .long _g_mcb\n  .byte 0xe8\n" \
    "  .long _fake_close - . - 4\n"
asm(
    ".text\n"

    // The macro editor's palette switch(this = the editor; row), ret 4, the
    // game's bytes around a stand-in sound: the row for both palettes, the
    // palette toggled, the other palette's window opened by name.
    ".p2align 4\n"
    ".globl _fake_palette_switch\n"
    "_fake_palette_switch:\n"
    "  .byte 0x8a,0x41,0x0c,0xba,0x01,0x00,0x00,0x00,0x3a,0xc2,0x8a,0x44,0x24,0x04,0x6a,0x00,0x52,0x88,0x41,0x0e,0x88,0x41,0x0d,0x75\n"
    "  .byte 0x1b,0xc6,0x41,0x0c,0x02,0x68\n"
    "  .long _g_key_mcr2edit\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe8\n"
    "  .long _fake_open - . - 4\n"
    "  .byte 0xe8\n"
    "  .long _h_switch_sound - . - 4\n"
    "  .byte 0xc2,0x04,0x00,0x88,0x51,0x0c,0x68\n"
    "  .long _g_key_mcr1edit\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe8\n"
    "  .long _fake_open - . - 4\n"
    "  .byte 0xe8\n"
    "  .long _h_switch_sound - . - 4\n"
    "  .byte 0xc2,0x04,0x00\n"

    // statcom2's input routine, never run: at the game's offset its Escape
    // with no row picked, its menu closed by instance with 1.
    ".p2align 4\n"
    ".globl _fake_status_input\n"
    "_fake_status_input:\n"
    "  .byte 0xc2,0x08,0x00\n"
    "  .org _fake_status_input + 0x1b, 0x90\n"
    "  .byte 0x66,0x83,0x7f,0x18,0x00,0x7d,0x15,0x8b,0x47,0x08,0x6a,0x01,0x50,0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe8\n"
    "  .long _h_close_menu@8 - . - 4\n"
    "  .byte 0x5f,0x5e,0xc2,0x08,0x00,0x8b,0xcf,0xe8\n"
    "  .long @h_status_clear@8 - . - 4\n"
    "  .byte 0xc2,0x08,0x00\n"

    // statcom2's begin-close(this = its controller), the game's bytes around a
    // stand-in clear: persona and mp_stat closed by name.
    ".p2align 4\n"
    ".globl _fake_status_close\n"
    "_fake_status_close:\n"
    "  .byte 0x56,0x8b,0xf1,0x68\n"
    "  .long _g_key_persona\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe8\n"
    "  .long _fake_close - . - 4\n"
    "  .byte 0x68\n"
    "  .long _g_key_mp_stat\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe8\n"
    "  .long _fake_close - . - 4\n"
    "  .byte 0x8b,0xce,0x5e,0xe9\n"
    "  .long @h_status_clear@8 - . - 4\n"

    // textcol1's Escape, never run: textcol2, then textcol1, closed by name.
    ".p2align 4\n"
    ".globl _fake_text_color_escape\n"
    "_fake_text_color_escape:\n"
    "  .byte 0x68\n"
    "  .long _g_key_textcol2\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe8\n"
    "  .long _fake_close - . - 4\n"
    "  .byte 0x68\n"
    "  .long _g_key_textcol1\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe8\n"
    "  .long _fake_close - . - 4\n"
    "  .byte 0xe9\n"
    "  .long _h_never - . - 4\n"

    // The inventory's first free slot, never run: from slot 1 while below the
    // bag's size, the stand-in's.
    ".p2align 4\n"
    ".globl _fake_free_slot\n"
    "_fake_free_slot:\n"
    "  .byte 0xa1\n"
    "  .long _g_pd_ptr\n"
    "  .byte 0x53,0x85,0xc0,0x56,0x74,0x2f,0x6a,0x00,0xb3,0x01,0x8d,0xb0\n"
    "  .long 0x9894\n"
    "  .byte 0xe8\n"
    "  .long _h_bag_size - . - 4\n"
    "  .byte 0x83,0xc4,0x04,0x3a,0xc3,0x76,0x19,0x66,0x83,0x3e,0x00,0x74,0x18,0x6a,0x00,0xfe,0xc3,0x83,0xc6,0x2c,0xe8\n"
    "  .long _h_bag_size - . - 4\n"
    "  .byte 0xc3\n"

    // mp_mncst's Escape, run as the game's: mp_stat (row 355) and mp_cstm
    // (row 362) each closed by name through a routine of its own, then
    // mp_mncst.
    ".p2align 4\n"
    ".globl _fake_instincts_escape\n"
    "_fake_instincts_escape:\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_ctl_global + 4 * 355\n"
    "  .byte 0xe8\n"
    "  .long _fake_mp_stat_close - . - 4\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_ctl_global + 4 * 362\n"
    "  .byte 0xe8\n"
    "  .long _fake_mp_cstm_close - . - 4\n"
    "  .byte 0x68\n"
    "  .long _g_key_mp_mncst\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe8\n"
    "  .long _fake_close - . - 4\n"
    "  .byte 0xc3\n"
    ".p2align 4\n"
    "_fake_mp_stat_close:\n"
    "  .byte 0x68\n"
    "  .long _g_key_mp_stat\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe8\n"
    "  .long _fake_close - . - 4\n"
    "  .byte 0xc3\n"
    ".p2align 4\n"
    "_fake_mp_cstm_close:\n"
    "  .byte 0x68\n"
    "  .long _g_key_mp_cstm\n"
    "  .byte 0xb9\n"
    "  .long _g_mcb\n"
    "  .byte 0xe8\n"
    "  .long _fake_close - . - 4\n"
    "  .byte 0xc3\n"

    // cnqframe's input routine, never run: nothing while its closing bit is
    // set; on Escape the fade's start through vtable +0x50, then thirteen
    // windows closed by name, in the game's order.
    ".p2align 4\n"
    ".globl _fake_region_input\n"
    "_fake_region_input:\n"
    "  .byte 0xf6,0x41,0x14,0x40,0x0f,0x85\n"
    "  .long _fake_region_input_ret - . - 4\n"
    "  .byte 0x66,0x83,0x7c,0x24,0x04,0x06,0x0f,0x85\n"
    "  .long _fake_region_input_ret - . - 4\n"
    "  .byte 0x8b,0x01,0xff,0x50,0x50\n"
    HU_FAKE_CLOSE("conquer1") HU_FAKE_CLOSE("conq3pts") HU_FAKE_CLOSE("conq6sta") HU_FAKE_CLOSE("beseige1")
    HU_FAKE_CLOSE("beseige2") HU_FAKE_CLOSE("imperial") HU_FAKE_CLOSE("campresu") HU_FAKE_CLOSE("campsan")
    HU_FAKE_CLOSE("camparea") HU_FAKE_CLOSE("allied") HU_FAKE_CLOSE("coloresu") HU_FAKE_CLOSE("colorank")
    HU_FAKE_CLOSE("colopoin")
    "_fake_region_input_ret:\n"
    "  .byte 0xc2,0x08,0x00\n"


    ".section .rdata,\"dr\"\n"
    ".p2align 2\n"
    ".globl _g_status_vtable\n"
    "_g_status_vtable:\n"
    "  .long 0, 0, _fake_status_close, 0, 0, _fake_offer_ret, _fake_status_input\n"
    "  .fill 25, 4, 0\n"
    ".globl _g_region_vtable\n"
    "_g_region_vtable:\n"
    "  .long 0, 0, 0, 0, 0, _fake_offer_ret, _fake_region_input\n"
    "  .fill 13, 4, 0\n"
    "  .long @h_region_fade@8\n"
    "  .fill 11, 4, 0\n"
    ".text\n"
);

extern "C" // The settings' routines, the game's bytes: the config store's set, read,
// set method, lookup, store and notify, run; its save, never run; the chat
// filter page's row state with its two bit reads, the battle effects
// filter's toggle's head, conf11s's group load's head, textcol1's open's
// head and k1assign's and k2assign's opens' heads, never run, and the two
// table getters they call; conf12wi's equipment lock rows, conf4's idle
// timer rows and conf3win's flag rows, never run, around the routines the
// engine runs: the lock's request and the idle timer, the game's bytes
// around stand-ins, the player flags' read, the game's bytes on this
// image's player data (its flags at +0x30), and the flag's request, the
// game's head before a stand-in.
asm(
    ".text\n"
    // cfg_set
    ".p2align 4\n"
    ".globl _fake_cfg_set\n"
    "_fake_cfg_set:\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_store_ptr\n"
    "  .byte 0x85,0xc9,0x74,0x10,0x8b,0x44,0x24,0x08,0x8b,0x54,0x24,0x04,0x50,0x52,0xe8\n"
    "  .long _fake_cfg_method - . - 4\n"
    "  .byte 0xc3,0x83,0xc8,0xff,0xc3\n"
    // cfg_get
    ".p2align 4\n"
    ".globl _fake_cfg_get\n"
    "_fake_cfg_get:\n"
    "  .byte 0x8b,0x0d\n"
    "  .long _g_store_ptr\n"
    "  .byte 0x85,0xc9,0x74,0x0b,0x8b,0x44,0x24,0x04,0x50,0xe8\n"
    "  .long _fake_cfg_read - . - 4\n"
    "  .byte 0xc3,0x83,0xc8,0xff,0xc3\n"
    // cfg_method
    ".p2align 4\n"
    ".globl _fake_cfg_method\n"
    "_fake_cfg_method:\n"
    "  .byte 0x53,0x8b,0x5c,0x24,0x08,0x56,0x57,0x53,0x8b,0xf9,0xe8\n"
    "  .long _fake_cfg_lookup - . - 4\n"
    "  .byte 0x8b,0xf0,0x85,0xf6,0x75,0x09,0x5f,0x5e,0x83,0xc8,0xff,0x5b,0xc2,0x08,0x00,0x8b,0x44,0x24,0x14,0x81\n"
    "  .byte 0xfb,0xce,0x00,0x00,0x00,0x74,0x0e,0x39,0x46,0x08,0x75,0x09,0x5f,0x5e,0x83,0xc8,0xff,0x5b,0xc2,0x08\n"
    "  .byte 0x00,0x50,0x56,0x8b,0xcf,0xe8\n"
    "  .long _fake_cfg_store - . - 4\n"
    "  .byte 0x85,0xc0,0x7d,0x09,0x5f,0x5e,0x83,0xc8,0xff,0x5b,0xc2,0x08,0x00,0x8b,0x4e,0x0c,0xb8,0x01,0x00,0x00\n"
    "  .byte 0x00,0x3b,0xc8,0x75,0x03,0x88,0x47,0x08,0x83,0x7e,0x0c,0x02,0x75,0x03,0x88,0x47,0x08,0x5f,0x5e,0x5b\n"
    "  .byte 0xc2,0x08,0x00\n"
    // cfg_read
    ".p2align 4\n"
    ".globl _fake_cfg_read\n"
    "_fake_cfg_read:\n"
    "  .byte 0x8b,0x44,0x24,0x04,0x50,0xe8\n"
    "  .long _fake_cfg_lookup - . - 4\n"
    "  .byte 0x85,0xc0,0x75,0x06,0x83,0xc8,0xff,0xc2,0x04,0x00,0x8b,0x40,0x08,0xc2,0x04,0x00\n"
    // cfg_lookup
    ".p2align 4\n"
    ".globl _fake_cfg_lookup\n"
    "_fake_cfg_lookup:\n"
    "  .byte 0x8b,0x49,0x04,0x85,0xc9,0x74,0x10,0x8b,0x44,0x24,0x04,0x8d,0x14,0x80,0x8d,0x04,0x50,0x8d,0x04,0x81\n"
    "  .byte 0xc2,0x04,0x00,0x33,0xc0,0xc2,0x04,0x00\n"
    // cfg_store
    ".p2align 4\n"
    ".globl _fake_cfg_store\n"
    "_fake_cfg_store:\n"
    "  .byte 0x8b,0x44,0x24,0x04,0x8b,0x4c,0x24,0x08,0x39,0x48,0x08,0x75,0x0c,0xf6,0x40,0x28,0x01,0x75,0x06,0x83\n"
    "  .byte 0xc8,0xff,0xc2,0x08,0x00,0x8b,0x50,0x1c,0x83,0xfa,0xff,0x74,0x10,0x56,0x8b,0x70,0x18,0x3b,0xf1,0x5e\n"
    "  .byte 0x7f,0x04,0x3b,0xca,0x7e,0x03,0x8b,0x48,0x20,0x89,0x48,0x08,0x8b,0x48,0x24,0x85,0xc9,0x74,0x0d,0x8b\n"
    "  .byte 0x50,0x08,0x8b,0x40,0x04,0x52,0x50,0xe8\n"
    "  .long _fake_cfg_notify - . - 4\n"
    "  .byte 0xb8,0x01,0x00,0x00,0x00,0xc2,0x08,0x00\n"
    // cfg_notify
    ".p2align 4\n"
    ".globl _fake_cfg_notify\n"
    "_fake_cfg_notify:\n"
    "  .byte 0x53,0x8b,0x5c,0x24,0x08,0x56,0x57,0x8b,0x7c,0x24,0x14,0x8b,0xf1,0x8b,0x46,0x0c,0x50,0x57,0x53,0xff\n"
    "  .byte 0x56,0x08,0x8b,0x76,0x04,0x83,0xc4,0x0c,0x85,0xf6,0x75,0xed,0x5f,0x5e,0x5b,0xc2,0x08,0x00\n"
    // cfg_save
    ".p2align 4\n"
    ".globl _fake_cfg_save\n"
    "_fake_cfg_save:\n"
    "  .byte 0x56,0x8b,0xf1,0x80,0x7e,0x09,0x01,0x75,0x0c,0xe8\n"
    "  .long _h_never - . - 4\n"
    "  .byte 0x8b,0xce,0xe8\n"
    "  .long _h_never - . - 4\n"
    "  .byte 0xb8,0x01,0x00,0x00,0x00,0x5e,0xc3\n"
    // filter_state
    ".p2align 4\n"
    ".globl _fake_filter_state\n"
    "_fake_filter_state:\n"
    "  .byte 0x56,0x57,0x8b,0x7c,0x24,0x0c,0x8b,0xf1,0x8a,0x04,0xbd\n"
    "  .long _g_filter_rows\n"
    "  .byte 0x3c,0x33,0x75,0x55,0x8a,0x04,0xbd\n"
    "  .long _g_filter_rows + 1\n"
    "  .byte 0x84,0xc0,0x7d,0x1a,0x8b,0x4e,0x3c,0x0f,0xbe,0xc0,0xf7,0xd8,0x3b,0xc8,0x0f,0x8c,0xa4,0x00,0x00,0x00\n"
    "  .byte 0x5f,0xb8,0x01,0x00,0x00,0x00,0x5e,0xc2,0x04,0x00,0x3c,0x02,0x75,0x23,0x8b,0x4e,0x40,0x85,0xc9,0x74\n"
    "  .byte 0x1c,0x8b,0x4e,0x38,0x51,0x6a,0x02,0xe8\n"
    "  .long _fake_bit_test - . - 4\n"
    "  .byte 0x83,0xc4,0x08,0xf6,0xd8,0x1b,0xc0,0x5f,0xf7,0xd8,0x83,0xc0,0x03,0x5e,0xc2,0x04,0x00,0x3c,0x0b,0x74\n"
    "  .byte 0x6f,0x0f,0xbe,0xc0,0xeb,0x34,0x8a,0x0c,0xbd\n"
    "  .long _g_filter_rows + 1\n"
    "  .byte 0x25,0xff,0x00,0x00,0x00,0x80,0xf9,0x0b,0x74,0x3e,0x8d,0x4e,0x30,0x51,0x50,0xe8\n"
    "  .long _fake_filter_bit - . - 4\n"
    "  .byte 0x83,0xc4,0x08,0x84,0xc0,0x74,0x0a,0x5f,0xb8,0x01,0x00,0x00,0x00,0x5e,0xc2,0x04,0x00,0x0f,0xbe,0x04\n"
    "  .byte 0xbd\n"
    "  .long _g_filter_rows + 1\n"
    "  .byte 0x8b,0x56,0x38,0x52,0x50,0xe8\n"
    "  .long _fake_bit_test - . - 4\n"
    "  .byte 0x83,0xc4,0x08,0x84,0xc0,0x74,0x25,0x5f,0xb8,0x02,0x00,0x00,0x00,0x5e,0xc2,0x04,0x00,0x83,0xc6,0x30\n"
    "  .byte 0x56,0x50,0xe8\n"
    "  .long _fake_filter_bit - . - 4\n"
    "  .byte 0x83,0xc4,0x08,0x84,0xc0,0x74,0x0a,0x5f,0xb8,0x01,0x00,0x00,0x00,0x5e,0xc2,0x04,0x00,0x5f,0x33,0xc0\n"
    "  .byte 0x5e,0xc2,0x04,0x00\n"
    // bit_test
    ".p2align 4\n"
    ".globl _fake_bit_test\n"
    "_fake_bit_test:\n"
    "  .byte 0x8b,0x4c,0x24,0x04,0xb8,0x01,0x00,0x00,0x00,0xd3,0xe0,0x85,0x44,0x24,0x08,0x0f,0x95,0xc0,0xc3\n"
    // filter_bit
    ".p2align 4\n"
    ".globl _fake_filter_bit\n"
    "_fake_filter_bit:\n"
    "  .byte 0x8b,0x4c,0x24,0x04,0x83,0xf9,0x1e,0x7d,0x04,0x33,0xc0,0xeb,0x08,0xb8,0x01,0x00,0x00,0x00,0x83,0xc1\n"
    "  .byte 0xe2,0xba,0x01,0x00,0x00,0x00,0xd3,0xe2,0x8b,0x4c,0x24,0x08,0x85,0x14,0x81,0x0f,0x95,0xc0,0xc3\n"
    // effect_rows
    ".p2align 4\n"
    ".globl _fake_effect_toggle\n"
    "_fake_effect_toggle:\n"
    "  .byte 0x56,0x57,0x8b,0x7c,0x24,0x0c,0x8b,0xf1,0x33,0xc9,0x8b,0x46,0x30,0x8a,0x0c,0xbd\n"
    "  .long _g_effect_rows\n"
    "  .byte 0x50,0x51,0xe8\n"
    "  .long _fake_bit_test - . - 4\n"
    "  .byte 0x83,0xc4,0x08,0x84,0xc0,0x74,0x20\n"
    // routing_load
    ".p2align 4\n"
    ".globl _fake_routing_load\n"
    "_fake_routing_load:\n"
    "  .byte 0x8b,0x44,0x24,0x04,0x53,0x56,0x8b,0xf1,0x8b,0x4c,0x24,0x10,0x57,0x89,0x4e,0x38,0x8b,0xce,0x89,0x46\n"
    "  .byte 0x34,0xe8\n"
    "  .long _h_never - . - 4\n"
    "  .byte 0x8b,0xce,0xe8\n"
    "  .long _h_never - . - 4\n"
    "  .byte 0x8b,0x56,0x38,0x52,0xe8\n"
    "  .long _fake_routing_getter - . - 4\n"
    "  .byte 0x83,0xc4,0x04,0x89,0x46,0x3c\n"
    // routing_getter
    ".p2align 4\n"
    ".globl _fake_routing_getter\n"
    "_fake_routing_getter:\n"
    "  .byte 0x8b,0x44,0x24,0x04,0x83,0xf8,0x03,0x77,0x1f,0xff,0x24,0x85\n"
    "  .long _fake_routing_getter + 0x2c\n"
    "  .byte 0xb8\n"
    "  .long _g_routing_1\n"
    "  .byte 0xc3,0xb8\n"
    "  .long _g_routing_2\n"
    "  .byte 0xc3,0xb8\n"
    "  .long _g_routing_3\n"
    "  .byte 0xc3,0xb8\n"
    "  .long _g_routing_4\n"
    "  .byte 0xc3,0x33,0xc0,0xc3,0x90\n"
    "  .long _fake_routing_getter + 0x10\n"
    "  .long _fake_routing_getter + 0x16\n"
    "  .long _fake_routing_getter + 0x1c\n"
    "  .long _fake_routing_getter + 0x22\n"
    // color_open
    ".p2align 4\n"
    ".globl _fake_color_open\n"
    "_fake_color_open:\n"
    "  .byte 0x53,0x56,0x8b,0xf1,0x57,0x8b,0x46,0x14,0x50,0xe8\n"
    "  .long _fake_color_getter - . - 4\n"
    "  .byte 0x8b,0x7e,0x08,0x83,0xc4,0x04,0x33,0xdb,0x8b,0xcf,0x53,0x6a,0x01,0x89,0x46,0x18,0x89,0x5e,0x1c,0xe8\n"
    "  .long _h_never - . - 4\n"
    // color_getter
    ".p2align 4\n"
    ".globl _fake_color_getter\n"
    "_fake_color_getter:\n"
    "  .byte 0x8b,0x44,0x24,0x04,0x83,0xf8,0x03,0x77,0x1f,0xff,0x24,0x85\n"
    "  .long _fake_color_getter + 0x2c\n"
    "  .byte 0xb8\n"
    "  .long _g_colors_1\n"
    "  .byte 0xc3,0xb8\n"
    "  .long _g_colors_2\n"
    "  .byte 0xc3,0xb8\n"
    "  .long _g_colors_3\n"
    "  .byte 0xc3,0xb8\n"
    "  .long _g_colors_4\n"
    "  .byte 0xc3,0x33,0xc0,0xc3,0x90\n"
    "  .long _fake_color_getter + 0x10\n"
    "  .long _fake_color_getter + 0x16\n"
    "  .long _fake_color_getter + 0x1c\n"
    "  .long _fake_color_getter + 0x22\n"
    // key_rows1
    ".p2align 4\n"
    ".globl _fake_key_open1\n"
    "_fake_key_open1:\n"
    "  .byte 0x56,0x8b,0xf1,0x57,0xc7,0x46,0x24,0x1e,0x00,0x00,0x00,0xe8\n"
    "  .long _h_never - . - 4\n"
    "  .byte 0x84,0xc0,0x75,0x07,0xc7,0x46,0x24,0x1c,0x00,0x00,0x00,0x8b,0x7e,0x08,0x6a,0x00,0x6a,0x01,0x8b,0xcf\n"
    "  .byte 0xc7,0x46,0x6c\n"
    "  .long _g_key_rows1\n"
    // key_rows2
    ".p2align 4\n"
    ".globl _fake_key_open2\n"
    "_fake_key_open2:\n"
    "  .byte 0x56,0x8b,0xf1,0x57,0x6a,0x00,0x8b,0x7e,0x08,0x6a,0x01,0x8b,0xcf,0xc7,0x46,0x24,0x1d,0x00,0x00,0x00\n"
    "  .byte 0xc7,0x46,0x6c\n"
    "  .long _g_key_rows2\n"
    "  .byte 0xc7,0x46,0x14,0x00,0x00,0x00,0x00\n"
    // lock_rows
    ".p2align 4\n"
    ".globl _fake_lock_rows\n"
    "_fake_lock_rows:\n"
    "  .byte 0x33,0xd2,0x66,0x3d,0x04,0x00,0x56,0x68,0xbf,0x00,0x00,0x00,0x0f,0x94,0xc2,0x8b,0xf2,0xe8\n"
    "  .long _fake_cfg_get - . - 4\n"
    "  .byte 0x56,0x68,0xbf,0x00,0x00,0x00,0x8b,0xf8,0xe8\n"
    "  .long _fake_cfg_set - . - 4\n"
    "  .byte 0x83,0xc4,0x0c,0x83,0xfe,0x01,0x5e,0x6a,0x00,0x6a,0x00,0x75,0x21,0x83,0xff,0x01,0x74,0x0e,0x6a,0x04\n"
    "  .byte 0xe8\n"
    "  .long _fake_lock_route - . - 4\n"
    "  .byte 0x83,0xc4,0x0c,0x5f,0xc2,0x08,0x00,0x6a,0x01,0xe8\n"
    "  .long _fake_lock_route - . - 4\n"
    "  .byte 0x83,0xc4,0x0c,0x5f,0xc2,0x08,0x00,0x6a,0x00,0xe8\n"
    "  .long _fake_lock_route - . - 4\n"
    "  .byte 0x83,0xc4,0x0c,0x5f,0xc2,0x08,0x00\n"
    // lock_route
    ".p2align 4\n"
    ".globl _fake_lock_route\n"
    "_fake_lock_route:\n"
    "  .byte 0x53,0xe8\n"
    "  .long _h_lock_busy - . - 4\n"
    "  .byte 0x84,0xc0,0x74,0x04,0x32,0xc0,0x5b,0xc3,0x8b,0x5c,0x24,0x08,0x80,0xfb,0x02,0x74,0x2c,0x8a,0x44,0x24\n"
    "  .byte 0x0c,0x84,0xc0,0x75,0x24,0xe8\n"
    "  .long _fake_lock_wait - . - 4\n"
    "  .byte 0x84,0xc0,0x75,0x11,0x8b,0x0d\n"
    "  .long _g_chat_owner\n"
    "  .byte 0x6a,0x22,0xe8\n"
    "  .long @h_lock_message@12 - . - 4\n"
    "  .byte 0xb0,0x01,0x5b,0xc3,0xe8\n"
    "  .long _h_lock_clock - . - 4\n"
    "  .byte 0xa3\n"
    "  .long _g_lock_time\n"
    "  .byte 0x8b,0x44,0x24,0x10,0x50,0x53,0xe8\n"
    "  .long _h_lock_request - . - 4\n"
    "  .byte 0x83,0xc4,0x08,0x88,0x44,0x24,0x0c,0x84,0xc0,0x74,0x37,0x84,0xdb,0x75,0x15,0x6a,0x00,0x68,0xbf,0x00\n"
    "  .byte 0x00,0x00,0xe8\n"
    "  .long _fake_cfg_set - . - 4\n"
    "  .byte 0x8a,0x44,0x24,0x14,0x83,0xc4,0x08,0x5b,0xc3,0x80,0xfb,0x01,0x74,0x0a,0x80,0xfb,0x03,0x74,0x05,0x80\n"
    "  .byte 0xfb,0x04,0x75,0x0f,0x6a,0x01,0x68,0xbf,0x00,0x00,0x00,0xe8\n"
    "  .long _fake_cfg_set - . - 4\n"
    "  .byte 0x83,0xc4,0x08,0x8a,0x44,0x24,0x0c,0x5b,0xc3\n"
    // lock_wait
    ".p2align 4\n"
    ".globl _fake_lock_wait\n"
    "_fake_lock_wait:\n"
    "  .byte 0xe8\n"
    "  .long _h_lock_clock - . - 4\n"
    "  .byte 0x2b,0x05\n"
    "  .long _g_lock_time\n"
    "  .byte 0x3d,0x28,0x23,0x00,0x00,0x0f,0x97,0xc0,0xc3\n"
    // idle_rows
    ".p2align 4\n"
    ".globl _fake_idle_rows\n"
    "_fake_idle_rows:\n"
    "  .byte 0x8d,0x44,0x40,0xfa,0x8d,0x04,0x80,0x8d,0x04,0x80,0xc1,0xe0,0x03,0x50,0xe8\n"
    "  .long _fake_idle_route - . - 4\n"
    "  .byte 0x83,0xc4,0x04,0x66,0x89,0x77,0x22,0x5f,0x5e,0xc2,0x08,0x00,0x6a,0x00,0xe8\n"
    "  .long _fake_idle_route - . - 4\n"
    "  .byte 0x83,0xc4,0x04,0x66,0x89,0x77,0x22,0x5f,0x5e,0xc2,0x08,0x00\n"
    // idle_route
    ".p2align 4\n"
    ".globl _fake_idle_route\n"
    "_fake_idle_route:\n"
    "  .byte 0x56,0x8b,0x74,0x24,0x08,0x85,0xf6,0x74,0x3e,0xb8,0x89,0x88,0x88,0x88,0xf7,0xee,0x03,0xd6,0xc1,0xfa\n"
    "  .byte 0x05,0x8b,0xc2,0xc1,0xe8,0x1f,0x03,0xd0,0x52,0x6a,0x41,0xe8\n"
    "  .long _fake_cfg_set - . - 4\n"
    "  .byte 0x8d,0x04,0xb6,0x8d,0x04,0x80,0x8d,0x0c,0x80,0xc1,0xe1,0x03,0x51,0xe8\n"
    "  .long _h_idle_ms - . - 4\n"
    "  .byte 0x83,0xc4,0x0c,0xc7,0x44,0x24,0x08,0x01,0x00,0x00,0x00,0x5e,0xe9\n"
    "  .long _h_idle_on - . - 4\n"
    "  .byte 0x6a,0xff,0x6a,0x41,0xe8\n"
    "  .long _fake_cfg_set - . - 4\n"
    "  .byte 0x83,0xc4,0x08,0xc7,0x44,0x24,0x08,0x00,0x00,0x00,0x00,0x5e,0xe9\n"
    "  .long _h_idle_on - . - 4\n"
    // anonymous_rows
    ".p2align 4\n"
    ".globl _fake_anonymous_rows\n"
    "_fake_anonymous_rows:\n"
    "  .byte 0x6a,0x00,0x89,0x46,0x30,0xe8\n"
    "  .long _fake_flag_get - . - 4\n"
    "  .byte 0x83,0xc4,0x0c,0x25,0xff,0x00,0x00,0x00,0x89,0x46,0x38,0xc6,0x46,0x3c,0x00,0x5e,0xc3,0x90,0x90,0x90\n"
    "  .byte 0x80,0x79,0x3c,0x01,0x75,0x0e,0x8a,0x41,0x38,0x50,0x6a,0x00,0xe8\n"
    "  .long _fake_flag_set - . - 4\n"
    "  .byte 0x83,0xc4,0x08,0xc3\n"
    // flag_get
    ".p2align 4\n"
    ".globl _fake_flag_get\n"
    "_fake_flag_get:\n"
    "  .byte 0xa1\n"
    "  .long _g_player_data_ptr\n"
    "  .byte 0x85,0xc0,0x0f,0x84,0x8e,0x00,0x00,0x00,0x8b,0x4c,0x24,0x04,0x83,0xf9,0x0b,0x0f,0x87,0x81,0x00,0x00\n"
    "  .byte 0x00,0xff,0x24,0x8d\n"
    "  .long _fake_flag_get + 0xa0\n"
    "  .byte 0x8a,0x80\n"
    "  .long 0x30\n"
    "  .byte 0xc0,0xe8,0x02,0x24,0x01,0xc3,0x8a,0x80\n"
    "  .long 0x30\n"
    "  .byte 0xd0,0xe8,0x24,0x01,0xc3,0x8a,0x80\n"
    "  .long 0x30\n"
    "  .byte 0x24,0x01,0xc3,0x8a,0x80\n"
    "  .long 0x30\n"
    "  .byte 0xc0,0xe8,0x03,0x24,0x03,0xc3,0x8a,0x80\n"
    "  .long 0x31\n"
    "  .byte 0xc0,0xe8,0x06,0x24,0x01,0xc3,0x8a,0x80\n"
    "  .long 0x31\n"
    "  .byte 0xc0,0xe8,0x07,0xc3,0x8a,0x80\n"
    "  .long 0x33\n"
    "  .byte 0x24,0x01,0xc3,0x8a,0x80\n"
    "  .long 0x33\n"
    "  .byte 0xd0,0xe8,0x24,0x01,0xc3,0x8a,0x80\n"
    "  .long 0x33\n"
    "  .byte 0xc0,0xe8,0x02,0x24,0x01,0xc3,0x8a,0x80\n"
    "  .long 0x33\n"
    "  .byte 0xc0,0xe8,0x03,0x24,0x01,0xc3,0x8a,0x80\n"
    "  .long 0x33\n"
    "  .byte 0xc0,0xe8,0x05,0x24,0x01,0xc3,0x32,0xc0,0xc3,0x8b,0xff\n"
    "  .long _fake_flag_get + 0x21\n"
    "  .long _fake_flag_get + 0x2d\n"
    "  .long _fake_flag_get + 0x38\n"
    "  .long _fake_flag_get + 0x41\n"
    "  .long _fake_flag_get + 0x9b\n"
    "  .long _fake_flag_get + 0x4d\n"
    "  .long _fake_flag_get + 0x59\n"
    "  .long _fake_flag_get + 0x63\n"
    "  .long _fake_flag_get + 0x6c\n"
    "  .long _fake_flag_get + 0x77\n"
    "  .long _fake_flag_get + 0x83\n"
    "  .long _fake_flag_get + 0x8f\n"

    ".p2align 4\n"
    ".globl _fake_flag_set\n"
    "_fake_flag_set:\n"
    "  .byte 0xa1\n"
    "  .long _g_player_data_ptr\n"
    "  .byte 0x85,0xc0,0x0f,0x84\n"
    "  .long _fake_flag_set_none - . - 4\n"
    "  .byte 0x6a,0x01,0x6a,0x01,0x68,0xdc,0x00,0x00,0x00,0xe8\n"
    "  .long _h_flag_packet - . - 4\n"
    "  .byte 0x83,0xc4,0x0c,0xe9\n"
    "  .long _h_flag_set - . - 4\n"
    "_fake_flag_set_none:\n"
    "  .byte 0x32,0xc0,0xc3\n"
);

BOOL WINAPI DllMain(HINSTANCE, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH) {
        build_world();
    }
    return TRUE;
}
