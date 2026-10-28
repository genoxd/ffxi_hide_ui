// e2e_test - the shipped _HideUI.dll and hideui_daemon.dll, loaded through
// Windower's own LuaCore.dll, against the stand-in FFXiMain.dll from
// fake_ffximain.cpp, with a separate thread playing the game's main thread.
// Covers resolution, the four hooks, the cross-thread drain, events and the
// error event reaching the handle that queued the refused call, answers and
// cancels (a second reply while one is queued, a query value the list does
// not hold), options for query, link5, arealist and passinpu, pending(),
// link5's pending test, resize and the item count around an action menu's
// swap, info (and how long each family's size holds) and the query's options
// (the glyph-coded title and options decoded, two-byte characters through the
// table the engine builds on the game thread from the stand-in's converter)
// against the stand-in's routines and data, remembered sizes and their
// owners, resets back to the size a window had before its first resize, the
// chat log and the party list placed by their bottom edge (by class, the
// party list on its default rect included), the chat log carrying the party
// list under a move_group to x,y, layout() replayed by hideui.lua's apply(),
// shutdown and the pin, the rescan of routines the daemon already patched,
// the fail-closed paths, and several addons: the same engine from two
// folders under one basename (a resident and a forwarder), the slots of
// engine abi 1, 2 and 3 keeping their meaning for older copies, and copies
// that publish engine abi 2's and abi 3's tables, standing for a 0.4.x and a
// 0.5.0 resident. 0.5.1's own: close refused for the prompt windows, the
// prompt's id refused once the game opens the window again (up front and on
// the game thread), the incoming box ended through its own cancel, a group
// move waiting for its closed anchor, the pending and resync events, the
// holders of each hide and block, and the layout a released handle keeps.
// 0.5.2's own: the registry's tkdebug record, dbdelsel, opened, hidden and
// closed through hideui.lua by its own key. 0.6.0's own: the mouse kept off a
// hidden window -- the live menu's +0x77 at the hide, the unhide and a new
// instance's open, and the stand-in's own mouse mode picker held in world
// mode while a hidden window is active -- and the two new sites failing
// closed. 0.6.1's own: close by instance as the game runs it, the instance
// freed frames after its marks, every reader and the prompt verbs saying
// closed from the closed event on. 0.6.3's own: an install the daemon could
// not patch just now (a thread inside a prologue) left idle and retried by
// the next new(), query's own refusal, and hideui.lua keeping `detail`
// behind hideui.debug(true). 0.7.0's own: options() and pending() naming
// their prompt, prtyjoin's inviter and the session ids its replies and the
// post boxes' take, move and move_group by the frame's top-left (the old
// slot by the origin still), the resets of what one handle placed, rects(),
// cursor events, blocked's holders, the error a replaced waiting group move
// posts, a block closing an open window (the old slot's leaving it), and
// hideui.lua's own layout, its UI size and apply()'s refusal of another,
// replies taking the prompt table, new() named after _addon.name, and the
// game's text as UTF-8. 0.7.2's own: query's cursor in info() and its cursor
// event as the option under it, and info's top, through a list longer than
// the rows shown, the stand-in's confirm taking the option under it. 0.7.3's
// own: arealist on the game's own layout (fake_arealist.h) -- every row with
// its kind and its label or count, and the cursor as the row under it.
// 0.7.4's own: answer(query) writes the option's index, the first option
// shown and the value; arealist's answer and cancel end an NPC event's
// choice (modes 1 and 2) through close+reset, open or blocked, the game's
// latch clear called alone when blocked, and refuse the search, mode 4, a
// region and an unlisted zone. 0.7.5's own: no reply reaches a menu's input
// routine. cancel(query) writes 0xFF alone and the stand-in's event wait
// closes query a tick later; cancel(post1) with the incoming box up sends
// request 15 alone, refused while the box takes no input yet, and the
// stand-in's reply and tick close the boxes (closed events); cancel(delivery)
// with the outgoing box up writes its closing row's two words, refused while
// busy or closing, and the stand-in's tick sends request 15, its reply
// closing the box; options('arealist') with the list blocked in an NPC's
// prompt is {name, id, mode, level, pending, rows = {}}. 0.7.6's own: keys
// and the gamepad kept off a hidden window -- the stand-in's routing routine,
// the game's bytes, calling the hooked sink: its codes reach a visible active
// window's sink and never a hidden one's, the sink's other callers still
// reach it, with no active menu the routing routine's own branch runs, the
// unhide gives the keys back -- and the two new signatures and the routing
// routine's call to the sink failing closed. 0.8.0's own: the compass, on
// the stand-in's object and draw entry (the game's bytes around three
// stand-in callees, run each frame after the update): listed open and not
// blockable, its box and anchor in info() and rects(), a hide stopping its
// draws and updates, a move seen by its render at the anchor and put back by
// a reset, remembered() and the layout, open, close, block and resize
// refused, opened and closed as its state byte is poked, the site resolved
// again once the daemon's jump is on it, a release unhiding and resetting
// it, and the site and its two copies of the global failing closed. 0.9.0's
// own: the macro keys, on the stand-in's gate (the game's shape around
// stand-in globals and a callee) consulted each frame a number key is held:
// a block has the hooked gate say no and nothing fires, closes a bar the
// handler has up with the handler's own sequence, is read back by macros()
// and status().macros_blocked with its holders, holds while any handle
// holds it and goes with a release; the gate resolved again over the
// daemon's jump; the gate, its manager imm32 and the macro object's
// constructor site failing closed; and a 0.8.0 resident (copy f) making no
// handle for this copy. 0.10.0's own: the ability opener, on the stand-in's
// site (the game's shape around a stand-in global, the two keys in the
// stand-in's data, the manager and two real calls to the open routine, and
// a body storing the kind at the controller's +0x64), called cdecl on the
// game thread as the game's callers do: an open through it stamps the
// opened event with its kind, closed carries the controller's list, a
// category block refuses that kind alone with blocked carrying it while
// the others open, the whole-window block refuses every kind, info lists
// the categories blocked and each handle's own, a release drops them,
// hideui.lua's on() with a category filter, the site resolved again over
// the daemon's jump, the site, its manager imm32 and its key push failing
// closed, and a 0.9.0 resident (copy g) making no handle for this copy.
// 0.11.0's own: a hidden window that asks nothing keeps the routing
// routine's codes and the menu mouse mode, and only a hidden prompt loses
// them; the yes/no windows on the stand-in's class, asked on the game thread
// as the game's askers ask: options() decoding the question from the game's
// text, its default from the cursor the ask left, cancellable and hold;
// answer and cancel ending it as the class's confirm and cancel do (the
// callback, the question cleared, holdtime and the window closed in each
// one's order), refused for a class built not cancellable; the resyn family
// answered under the one key its controller holds; a question asked again
// from inside the callback opened anew as a new prompt with its own id; a
// held confirm; open and close refused, a block leaving an open question to
// its reply and refusing the next; the earlier slots treating them as any
// window; select-row and the class's constructor failing closed; and a
// 0.10.0 resident (copy h) making no handle for this copy. gmtell, comyn,
// dead and mp_dead refused by every block and close slot of every engine
// abi, each with its reason. The KO window on a stand-in controller:
// options() reading its countdown and whether its question can be asked;
// answer() its own confirm, asking through comyn with the arguments its
// input handler's bytes hold (the decider, the message getter and the
// class's ask the stand-in's, the callback the game's own bytes, run by
// comyn's yes), refused while comyn asks; cancel() refused; hidden, no key
// reaching it; and the class's confirm, the KO window's confirm site, its
// callback and its tick failing closed. The resyn family refused by every
// block and close slot too. The number boxes: their setups and confirms the
// game's bytes, asked on the game thread as the game's askers ask:
// options() reading the number, its range and whether a block kept the
// window from opening, the picked item's id; answer and cancel through the
// confirm, a number that is no whole number or out of range refused; a
// hidden box taking no routed key; a callback a close from outside leaves
// set asking nothing; the earlier slots treating the boxes as any window
// and block8 leaving an open one to its reply; a blocked count announced by
// a blocked and a pending event, listed by pending(), refused while
// trade's cursor is off its row, answered through trade's own callback (the
// game's bytes) with no window, its end a pending event every handle but
// the replying one reads; trade's window closing ending it; an unknown
// asker's question refused both replies; a later open ending a blocked
// question; pending5 and poll5 leaving the boxes out; and the boxes'
// confirms, setups, three askers and the yes/no class's cancel test failing
// closed. The Asked windows (sortyn, evitem01, confyn, bankmenu, maplist,
// gmtell, mp_dead, ok) asked through the stand-in's askers on the game
// thread: options() in plain fields with nothing asked, with the window up
// and blocked; open, close and the wrong kind of value refused, and cancel
// where the window has none; answer and cancel doing what each window's own
// confirm and cancel do, through the game's bytes where the stand-in has
// them (sortyn's setup and select-row, evitem01's setup and callback,
// bankmenu's begin-close and clear, maplist's close+reset), the GM tell
// poller and the error screen acting on what the replies wrote; a hidden
// sortyn taking no routed key; sortyn, evitem01, bankmenu, maplist and ok
// blocked: announced, listed by pending() and pending8 (not pending5 or
// poll5), answered with no window by the prompt's id, the end read by
// every other handle; resolution to the stand-in's sites; and each Asked
// signature, and every key, global, routine and byte the engine checks in
// them, failing closed. mogdoor, opened by the stand-in's door handler:
// options() with its rows, areas and texts, answer() through its confirm's
// gate and exit, the floor row and the Mog Garden refused while off or for
// the other floor, the gate's refusal as an error event, cancel() its top
// level's Escape at either level, no gate run; blocked, recorded, listed by
// pending8 and pending(), answered with no window. The dialogue boxes: options() reading the page from the box's
// lines, answer() one page through the text dispatch, the message's end
// closing the box, a timed page and a cancel the event allows not refused,
// the cancel releasing the box's lines and clearing its own text object;
// page events through poll8 and not poll5; the key test hooked, the boxes'
// tick told no about the left button while a hidden box is active, its other
// callers untouched. The two boxes one dialogue: a hide or unhide through
// either name holding both, two handles' holds on the pair as on one window,
// a block refused on both alike, a move or a resize each box's own, an
// answer through either moving both on and a cancel through either ending
// both, under the one id both report, a stale one refused, and a reply
// through one refused while one through the other is queued. rem4line and
// rem4li2 refused by every block and close slot, ok by every close slot;
// and mogdoor's, the boxes' (the text dispatch's hand-off to the other box
// among them) and the key test's signatures and checks failing closed. The
// text-entry windows (stringdl,
// comment, msgline, scname, link11), their routines the game's bytes where
// the engine reads and runs them: open, close and block refused as their
// rules give, stringdl by every block and close slot; options() in plain
// fields; answer and cancel doing what each window's own commit and cancel
// do, comment's lines and link11's name and colors as tables through the
// binding; the refusals of the game's limits, msgline's 300 bytes among
// them; a hide taking the line from the window's own field alone, ending it
// or suspending passinpu's under its draw, a hidden open doing so for the one
// its hook starts, passinpu's, comment's (on the line it edited) and
// msgline's started again at the show through each one's own start, the
// game's active menu's last, msgline's field ended first, keeping its text,
// while it holds the line; msgline blocked, announced, listed by pending(), answered
// with no window, its end read by the other handle; each input routine's
// signature and a shape in each failing closed, comment's start and
// passinpu's among them. The target arrows on the stand-in's per-menu
// draw, sprite draw and quad routine, hooked under the stand-in's draw walk
// each frame: listed and read as a name on no row, opened and closed as the
// target window's draw starts and stops painting an arrow, its arrows'
// points under detail; hidden targetwi keeping its layer with only its
// arrows drawn, targetarrow's hide, close and block taking only the arrows,
// no other window's frame dropped; move, resize and reset accepted; the
// three draws, the template swap and the two arrow draws resolved, again
// over the daemon's jumps, and failing closed. The template swap: the
// owners' swaps through the stand-in's hooked entry putting partywin back
// on the bottom edge its move gave it, the swap's size standing, and
// leaving playermo's rows to its owner. A zone change: partywin moved by its
// frame while one member tall, closed, opened again at its template's
// height and swapped to its roster, on the bottom edge the move gave it at
// each step. The trade windows, their routines
// the game's bytes over the stand-in's player data, packet queue and clock:
// options() reading both offers and the names through the actor table;
// offer, withdraw, the gil, the OK and the cancel through the game's own
// stage, OK, cancel and close; the trade's offer on its way until the
// acknowledgment writes it; one change a second; the gray test refusing on
// the game thread; the NPC trade's offer sent at its OK, then closed; a
// blocked NPC trade refusing its replies; each signature and a shape in
// what it leads to failing closed. A shutdown while no frame runs: accepted
// at once and closing, a second one starting no second thread, the restore
// run by the next frame and the hooks taken out by the engine's own thread,
// which gives the image back; a new() before that frame, as a reload makes
// it, taking the closing engine back; a hooked call still running as the
// handlers come out leaving the engine pinned until the next shutdown. The
// path of the 2026-10-21 freeze: the demo itself through hideui.lua with
// debug and events on, the dialogue pair hidden by one name before either
// box exists, map0 open and closed, the target window drawing its arrow
// closing as both boxes open hidden on one message under a held left
// button, prerender between frames and inside the game thread's frame, and
// a watchdog failing any step that does not come back. With an engine built
// with -DHU_HEAPCHECK (ENGINE_DLL), the heap check against the stand-in's
// overrun of a heap block of its own: the check after map0's callback on a
// hidden maplist's leaf failing, the one before it the last good, reported
// once to the handle; the background check catching an overrun outside
// every command within its second; the shutdown checking to its unhook, and
// the next two images naming an overrun between images; and a check's cost.
// 0.11.0's own: open of exactly the window named, through hideui.lua's
// binding: a window that cannot work alone refused with what to use
// instead, ability and magic without a list, a list past either table and a
// window with no controller in its slot refused, every window whose open
// alone crashes or strands the client refused by the open slots of engine
// abi 1, 3, 8 and 9; a list of ability alone through the stand-in's opener
// with no sort and no pane, and of magic alone through the magic opener's
// game bytes with its rebuild's open of subwindo kept from opening, no event
// about it, and magic's begin-close finding the sort window's controller the
// setup wrote; the ability and magic sets with their sort windows (blusortw
// for blue magic) and pane; the equipment set through the opener's game
// bytes and layout, each window its own opened event, the 0x061 request;
// the game's refusals as error events (Monstrosity by the check's game
// bytes, by its word and by the mode byte, al alone read; the opener's
// first test; its guard), the blocked members and lists refused up front;
// a pet's list with no pet out closed by the game's per-frame and reported;
// the map, blue magic, automaton, linkshell, instincts, synthesis, tabula
// and macro sets through the stand-in's openers of the game's bytes, which
// the engine's signatures and shapes resolve, the job tests, the linkshell
// row's and the item use's checks, Monstrosity and the item at an index
// each refusing as an error event; the open routine's last argument as the
// game's openers pass it, titlehan over titlewin and targetwi parked under
// ut_point; and an earlier 0.11.0 build of engine abi 8 (copy i) making no
// handle for this copy. The settings: settings() in the Config menu's order,
// each kind's fields read from the stand-in's store, the game's object and
// entries, with the game's labels, through hideui.lua as UTF-8; setting();
// set_setting() of each kind through the store's set, the game's bytes,
// each id's subscriber run, a set to the stored value running none, bits and
// chat filter rows composed with every other bit kept, the inverted bools
// and a choice on a bit, the equipment lock, auto_disconnect and
// character_information through their routines; misuse raising, the
// refusals up front, a set while cfilter is open refused by an error event,
// and every call refused while the store is not loaded.
//
// e2e_test.exe <LuaCore.dll> <a/_HideUI.dll> <FFXiMain.dll> <b/_HideUI.dll> <c/_HideUI.dll> <hideui.lua>
//              <d/_HideUI.dll> <e/_HideUI.dll> <f/_HideUI.dll> <g/_HideUI.dll> <h/_HideUI.dll>
//              <i/_HideUI.dll> <j/_HideUI.dll>
//
// Copy e publishes engine abi 4's table, standing for a 0.5.1 to 0.6.3
// resident; copy f engine abi 5's, standing for a 0.7.0 to 0.8.0 one; copy
// g engine abi 6's, standing for a 0.9.0 one; copy h engine abi 7's,
// standing for a 0.10.0 one; copy i engine abi 8's, and copy j engine abi
// 9's up to set_setting9, standing for earlier 0.11.0 builds.

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <tlhelp32.h>
#include <stdint.h>
#include <cstdio>
#include <cstring>

#include "../hideui_engine_abi.h"

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

// ---------------------------------------------------------------------------
// LuaCore, by name, so this exe needs no import library.

struct lua_State;
const int kGlobals = -10002;

struct LuaApi {
    lua_State* (*newstate)();
    void (*openlibs)(lua_State*);
    int (*loadstring)(lua_State*, const char*);
    int (*pcall)(lua_State*, int, int, int);
    const char* (*tolstring)(lua_State*, int, size_t*);
    double (*tonumber)(lua_State*, int);
    void (*settop)(lua_State*, int);
    void (*close)(lua_State*);
    void (*pushstring)(lua_State*, const char*);
    void (*setfield)(lua_State*, int, const char*);
} lua;

template <typename T>
bool bind(HMODULE module, const char* name, T* out) {
    FARPROC p = GetProcAddress(module, name);
    memcpy(out, &p, sizeof(p));
    return p != NULL;
}

bool load_lua(const char* path) {
    HMODULE m = LoadLibraryA(path);
    return m && bind(m, "luaL_newstate", &lua.newstate) && bind(m, "luaL_openlibs", &lua.openlibs)
        && bind(m, "luaL_loadstring", &lua.loadstring) && bind(m, "lua_pcall", &lua.pcall)
        && bind(m, "lua_tolstring", &lua.tolstring) && bind(m, "lua_tonumber", &lua.tonumber)
        && bind(m, "lua_settop", &lua.settop) && bind(m, "lua_close", &lua.close)
        && bind(m, "lua_pushstring", &lua.pushstring) && bind(m, "lua_setfield", &lua.setfield);
}

void set_global(lua_State* L, const char* name, const char* value) {
    lua.pushstring(L, value);
    lua.setfield(L, kGlobals, name);
}

const char kPrelude[] =
    "failures = 0\n"
    "out = {}\n"
    "function check(ok, what)\n"
    "  if not ok then failures = failures + 1 end\n"
    "  out[#out + 1] = (ok and 'PASS' or 'FAIL') .. '  ' .. what\n"
    "end\n"
    "function load_engine()\n"
    "  local open, message = package.loadlib(DLL, 'luaopen__HideUI')\n"
    "  assert(open, message)\n"
    "  return open()\n"
    "end\n"
    "function copy_of(path)\n"
    "  return path and path:match('[/\\\\]([abcdefghij])[/\\\\]_[Hh]ide[Uu][Ii]%.dll$')\n"
    "end\n"
    "function finish()\n"
    "  local text = table.concat(out, '\\n')\n"
    "  out = {}\n"
    "  local f = failures\n"
    "  failures = 0\n"
    "  return f, text\n"
    "end\n";

// Runs a chunk that ends in `return finish()`, prints what it checked, and
// folds its failures into this harness's count.
void run(lua_State* L, const char* chunk) {
    if (lua.loadstring(L, chunk) != 0 || lua.pcall(L, 0, 2, 0) != 0) {
        const char* message = lua.tolstring(L, -1, NULL);
        check(false, message ? message : "(a Lua error with no message)");
        lua.settop(L, 0);
        return;
    }
    const int failed = static_cast<int>(lua.tonumber(L, -2));
    const char* text = lua.tolstring(L, -1, NULL);
    if (text && *text) {
        std::printf("%s\n", text);
    }
    g_failures += failed;
    lua.settop(L, 0);
}

int count_checks(const char* chunk) {
    int n = 0;
    for (const char* p = chunk; (p = strstr(p, "check(")) != NULL; ++p) {
        ++n;
    }
    return n;
}

const char* g_dll;

lua_State* fresh(const char* const* globals) {
    lua_State* L = lua.newstate();
    lua.openlibs(L);
    set_global(L, "DLL", g_dll);
    for (int i = 0; globals && globals[i]; i += 2) {
        set_global(L, globals[i], globals[i + 1]);
    }
    if (lua.loadstring(L, kPrelude) != 0 || lua.pcall(L, 0, 0, 0) != 0) {
        check(false, "prelude");
    }
    return L;
}

void phase(lua_State* L, const char* chunk) {
    g_checks += count_checks(chunk);
    run(L, chunk);
}

bool engine_mapped() {
    return GetModuleHandleA("_HideUI.dll") != NULL;
}

lua_State* fresh_from(const char* dll, const char* const* globals) {
    const char* const saved = g_dll;
    g_dll = dll;
    lua_State* L = fresh(globals);
    g_dll = saved;
    return L;
}

bool mapped(const char* path) {
    return GetModuleHandleA(path) != NULL;
}

// A shutdown returns once the restore is queued; the engine's own thread
// finishes it after the next frame. Waits, up to five seconds, for the engine
// the global `module` names in L to leave "closing".
void settle(lua_State* L, const char* module = "hu") {
    char chunk[64];
    snprintf(chunk, sizeof(chunk), "return %s.status().state", module);
    const DWORD start = GetTickCount();
    for (;;) {
        bool closing = true;
        if (lua.loadstring(L, chunk) == 0 && lua.pcall(L, 0, 1, 0) == 0) {
            const char* state = lua.tolstring(L, -1, NULL);
            closing = state && strcmp(state, "closing") == 0;
        }
        lua.settop(L, 0);
        if (!closing || GetTickCount() - start > 5000) {
            return;
        }
        Sleep(1);
    }
}

// That thread gives back the image's last reference as it ends: whether
// `module` unmaps within five seconds.
bool unmaps(const char* module) {
    const DWORD start = GetTickCount();
    while (mapped(module) && GetTickCount() - start < 5000) {
        Sleep(1);
    }
    return !mapped(module);
}

bool engine_unmaps() {
    return unmaps("_HideUI.dll");
}

typedef LONG (WINAPI* QueryThreadFn)(HANDLE, int, void*, ULONG, ULONG*);
const int kThreadStartAddress = 9;      // ThreadQuerySetWin32StartAddress

// The threads of this process that started in the engine's image: the one a
// shutdown starts to finish it.
int engine_threads() {
    const uint8_t* base = reinterpret_cast<const uint8_t*>(GetModuleHandleA("_HideUI.dll"));
    QueryThreadFn query;
    FARPROC p = GetProcAddress(GetModuleHandleA("ntdll.dll"), "NtQueryInformationThread");
    memcpy(&query, &p, sizeof(p));
    if (!base || !query) {
        return 0;
    }
    const IMAGE_DOS_HEADER* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(base);
    const IMAGE_NT_HEADERS32* nt = reinterpret_cast<const IMAGE_NT_HEADERS32*>(base + dos->e_lfanew);
    const uint8_t* end = base + nt->OptionalHeader.SizeOfImage;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }
    int n = 0;
    THREADENTRY32 t;
    t.dwSize = sizeof(t);
    for (BOOL more = Thread32First(snapshot, &t); more; more = Thread32Next(snapshot, &t)) {
        if (t.th32OwnerProcessID != GetCurrentProcessId()) {
            continue;
        }
        HANDLE thread = OpenThread(THREAD_QUERY_INFORMATION, FALSE, t.th32ThreadID);
        if (!thread) {
            continue;
        }
        const uint8_t* entry = NULL;
        if (query(thread, kThreadStartAddress, &entry, sizeof(entry), NULL) == 0 && entry >= base && entry < end) {
            ++n;
        }
        CloseHandle(thread);
    }
    CloseHandle(snapshot);
    return n;
}

// The engines name their own paths as the loader reports them.
void to_backslashes(char* path) {
    for (; *path; ++path) {
        if (*path == '/') {
            *path = '\\';
        }
    }
}

void beside(const char* dll, const char* name, char* out, size_t size) {
    snprintf(out, size, "%s", dll);
    char* cut = strrchr(out, '\\');
    if (cut) {
        snprintf(cut + 1, size - static_cast<size_t>(cut + 1 - out), "%s", name);
    }
}

// The resident's table as an older copy reads it out of the record.
bool record_api(HuEngineApi* out) {
    char name[HU_NAME_MAX];
    snprintf(name, sizeof(name), HU_ENGINE_MAPPING_NAME_FORMAT, static_cast<unsigned>(GetCurrentProcessId()));
    HANDLE mapping = OpenFileMappingA(FILE_MAP_READ, FALSE, name);
    if (!mapping) {
        return false;
    }
    bool ok = false;
    const void* view = MapViewOfFile(mapping, FILE_MAP_READ, 0, 0, sizeof(HuEngineRecord));
    if (view) {
        const HuEngineRecord* rec = static_cast<const HuEngineRecord*>(view);
        ok = rec->magic == HU_ENGINE_MAGIC;
        memcpy(out, &rec->api, sizeof(*out));
        UnmapViewOfFile(view);
    }
    CloseHandle(mapping);
    return ok;
}

// A read through a slot of the resident's table, as an older copy makes it.
uint8_t g_raw[1 << 16];
HuEngineReply g_reply;

const HuEngineReply& raw_read(HuEngineRead read, const HuEngineHandle* h) {
    g_reply.size = sizeof(g_reply);
    g_reply.data = g_raw;
    g_reply.capacity = sizeof(g_raw);
    g_reply.used = 0;
    read(h, &g_reply);
    return g_reply;
}

const HuEngineReply& raw_read(HuEngineNamedRead read, const HuEngineHandle* h, const char* name) {
    g_reply.size = sizeof(g_reply);
    g_reply.data = g_raw;
    g_reply.capacity = sizeof(g_raw);
    g_reply.used = 0;
    read(h, name, &g_reply);
    return g_reply;
}

// Whether a reply sets a field named `key` anywhere in it.
bool sets_field(const HuEngineReply& r, const char* key) {
    const uint32_t n = static_cast<uint32_t>(strlen(key));
    for (uint32_t i = 0; r.used <= r.capacity && i + 5 + n <= r.used; ++i) {
        uint32_t length;
        memcpy(&length, r.data + i + 1, 4);
        if (r.data[i] == HU_REPLY_FIELD && length == n && memcmp(r.data + i + 5, key, n) == 0) {
            return true;
        }
    }
    return false;
}

// Whether a reply holds the string `text` as a value.
bool has_string(const HuEngineReply& r, const char* text) {
    const uint32_t n = static_cast<uint32_t>(strlen(text));
    for (uint32_t i = 0; r.used <= r.capacity && i + 5 + n <= r.used; ++i) {
        uint32_t length;
        memcpy(&length, r.data + i + 1, 4);
        if (r.data[i] == HU_REPLY_STRING && length == n && memcmp(r.data + i + 5, text, n) == 0) {
            return true;
        }
    }
    return false;
}

// How often a reply holds the string `text` as a value.
int strings_named(const HuEngineReply& r, const char* text) {
    const uint32_t n = static_cast<uint32_t>(strlen(text));
    int found = 0;
    for (uint32_t i = 0; r.used <= r.capacity && i + 5 + n <= r.used; ++i) {
        uint32_t length;
        memcpy(&length, r.data + i + 1, 4);
        found += r.data[i] == HU_REPLY_STRING && length == n && memcmp(r.data + i + 5, text, n) == 0;
    }
    return found;
}

// Whether the first field named `key` a reply sets is true.
bool field_true(const HuEngineReply& r, const char* key) {
    const uint32_t n = static_cast<uint32_t>(strlen(key));
    for (uint32_t i = 1; r.used <= r.capacity && i + 5 + n <= r.used; ++i) {
        uint32_t length;
        memcpy(&length, r.data + i + 1, 4);
        if (r.data[i] == HU_REPLY_FIELD && length == n && memcmp(r.data + i + 5, key, n) == 0) {
            return r.data[i - 1] == HU_REPLY_TRUE;
        }
    }
    return false;
}

// How many fields named `key` a reply sets.
int fields_named(const HuEngineReply& r, const char* key) {
    const uint32_t n = static_cast<uint32_t>(strlen(key));
    int found = 0;
    for (uint32_t i = 0; r.used <= r.capacity && i + 5 + n <= r.used; ++i) {
        uint32_t length;
        memcpy(&length, r.data + i + 1, 4);
        found += r.data[i] == HU_REPLY_FIELD && length == n && memcmp(r.data + i + 5, key, n) == 0;
    }
    return found;
}

// The resident record's magic, read the way another copy would.
uint32_t record_magic() {
    char name[HU_NAME_MAX];
    snprintf(name, sizeof(name), HU_ENGINE_MAPPING_NAME_FORMAT, static_cast<unsigned>(GetCurrentProcessId()));
    HANDLE mapping = OpenFileMappingA(FILE_MAP_READ, FALSE, name);
    if (!mapping) {
        return 0;
    }
    uint32_t magic = 0;
    const void* view = MapViewOfFile(mapping, FILE_MAP_READ, 0, 0, sizeof(magic));
    if (view) {
        memcpy(&magic, view, sizeof(magic));
        UnmapViewOfFile(view);
    }
    CloseHandle(mapping);
    return magic;
}

// Whether an engine holds the claim on the hooks, which it gives up only once
// its handlers are out.
bool claim_held() {
    char name[64];
    snprintf(name, sizeof(name), "Local\\hideui_engine_v1_%08X", static_cast<unsigned>(GetCurrentProcessId()));
    HANDLE mapping = OpenFileMappingA(FILE_MAP_READ, FALSE, name);
    if (mapping) {
        CloseHandle(mapping);
    }
    return mapping != NULL;
}

// ---------------------------------------------------------------------------
// The stand-in game.

// fake_ffximain.cpp's, field for field.
struct FakeLog {
    char order[96];
    LONG count;
    LONG dock_resets;
    int dock_group;
    void* dock_menu;
    char swaps[64];
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
    char closes[64];
    int link5_calls;
    int link5_slot;
    void* link5_entry;
    int link5_last_null;
    int cursor_calls;
    void* cursor_menu;
    int cursor_row[4];
    int cursor_warp[4];
    int cursor_items[4];
    int box_calls;
    int box_event;
    int box_row;
    void* box_ctl;
    char last_open[17];
    int sink_calls;
    void* sink_menu;
    int sink_code;
    int main_menu_opens;
    int compass_x;
    int compass_y;
    int yesno_calls;
    int yesno_answer;
    void* yesno_context;
    int yesno_inputs;
    int select_calls;
    void* select_menu;
    int select_row;
    int decider_calls;
    int decider_args[2];
    int message_calls;
    int message_args[2];
    int ask_calls;
    void* ask_ctl;
    const void* ask_callback;
    const char* ask_text;
    int ask_args[5];
    int box_answers;
    int box_value;
    void* box_context;
    int box_who;
    int focus_calls;
    void* focus_menu;
    int asked_calls;
    int asked_who;
    int asked_value;
    int asked_extra;
    void* asked_context;
    int gm_result;
    int ok_seen;
    int door_gates;
    int door_gate_args[2];
    int door_opens;
    void* door_open_actor;
    int door_open_arg;
    int door_exit_at_open;
    int advances;
    void* advance_text;
    int dispatch_codes;
    int dispatch_code;
    int page_sounds;
    int releases;
    void* release_log;
    int release_line;
    int clears;
    void* clear_text;
    int wait_at_clear;
    int key_clicks;
    int dialogue_clicks;
    int attaches;
    void* attached;
    int deactivates;
    int field_reads;
    int field_deletes;
    void* field_deleted;
    int field_starts;
    int sanitizes;
    int trims;
    int comment_sends;
    int comment_mode;
    char comment_lines[3][41];
    int pol_numbers;
    uint32_t pol_word;
    int clocks;
    int sends;
    int send_mode;
    uint32_t send_args[9];
    char send_text[301];
    int marks;
    void* mark_obj;
    uint32_t mark_flags;
    int criteria_clears;
    void* criteria;
    int criteria_names;
    char criteria_name[17];
    int searches;
    int search_args[6];
    int encodes;
    int creates;
    uint32_t create_args[8];
    char create_name[24];
    int items;
    char quads[128];
    int walks;
    int queue_allocs;
    int queue_id;
    int commits;
    uint8_t packet[0x40];
    int trade_sends[2];
    int trade_unoks;
    int grays;
    int gray_lock;
    void* gray_mode;
    void* walked[64];
    int walked_count;
    void* drawn[64];
    int drawn_count;
    int map_builds;
    int map_selects;
    int map_select_row;
    int map_select_flag;
    void* painted[64];
    int painted_count;
    LONG pixels;
    void* freed[16];
    LONG freed_frame[16];
    int freed_count;
    void* ready[16];
    LONG ready_frame[16];
    int ready_count;
    int map_closes;
    void* map_frame;
    int map_mode;
    int map0_open_at_close;
    int map_fades;
    char line_modes[64];
    int bad_fields;
    char bad_field[64];
    int field_news;
    int passinpu_draws;
    int msgline_draws;
};

// fake_ffximain.cpp's FakeSets, field for field.
enum { kSetsMap, kSetsSpells, kSetsAttachments, kSetsLinkshell, kSetsInstincts, kSetsSynthesis, kSetsTabula,
       kSetsCount };
struct FakeSets {
    volatile LONG calls[kSetsCount];
    const void* self;
    int32_t args[3];
    uint8_t refuse;
    uint8_t linkshell_check;
    uint8_t use_check;
    volatile LONG checks;
    int32_t check_args[2];
};

// fake_ffximain.cpp's FakeRoutes, field for field.
struct FakeRoutes {
    volatile LONG calls;
    char order[256];
    const void* self;
    int32_t args[7];
    uint32_t msglist;
    uint8_t job_level[23];
    uint32_t expansion;
    int32_t tracking;
    uint8_t untracked;
    uint8_t bag_size[18];
    volatile LONG switch_sounds;
    int32_t scan_answer;
};

// fake_ffximain.cpp's FakeSettings, field for field.
struct FakeSettings {
    uint8_t* store;
    uint8_t* entries;
    volatile LONG notifies;
    int32_t notified[64][2];
    volatile LONG lock_requests;
    int32_t lock_args[2];
    uint8_t lock_ok;
    uint8_t lock_busy;
    volatile LONG lock_messages;
    uint32_t lock_clock;
    uint32_t* lock_stamp;
    volatile LONG idle_ms_calls;
    int32_t idle_ms;
    volatile LONG idle_on_calls;
    int32_t idle_on;
    volatile LONG flag_sets;
    int32_t flag_args[2];
    uint8_t* player_flags;
    uint8_t** store_ptr;
    const void* set;
    const void* lock;
    const void* idle;
    const void* flag;
};

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
    void* calls[13];
    uint8_t* party_pending;
    volatile LONG* party_queues;
    volatile LONG* post_queues;
    uint8_t* query_cancel_allowed;
    void* text_callback;
    FakeLog* log;
    const void** vtable;
    uint8_t* link5_cache;
    void* link5_callback;
    uint8_t* link5_site;
    uint8_t* query_cancel_site;
    void* set_cursor;
    uint8_t* link5_latch;
    uint8_t* link5_latch_site;
    uint8_t* link5_open_site;
    DWORD* game_thread;
    volatile LONG* convert_calls;
    volatile LONG* convert_elsewhere;
    void* glyph_convert;
    void* mouse_mode;
    volatile LONG* menu_up_calls;
    void* row_hit_test;
    uint8_t* mouse_read;
    uint32_t* close_frames;
    void* query_down;
    struct AreaLog* area_log;
    void* area_prepare;
    void* area_opened;
    uint8_t* area_ctl;
    void* area_select;
    volatile LONG* box_ticks;
    volatile LONG* post_reply;
    volatile LONG* query_waits;
    void* menu_input;
    void* menu_routing;
    uint8_t* compass;
    uint8_t** compass_ptr;
    void* compass_draw;
    volatile LONG* compass_draws;
    volatile LONG* compass_updates;
    uint8_t* macro_object;
    uint8_t** macro_object_ptr;
    void* macro_gate;
    uint8_t* macro_ctor_site;
    volatile LONG* macro_key;
    volatile LONG* macros_fired;
    void* ability_open;
    int32_t* ability_last_kind;
    uint8_t* yesno_site;
    const void** yesno_vtable;
    void* yesno_ask;
    uint8_t* yesno_confirm;
    uint8_t* dead_site;
    uint8_t* dead_tick;
    uint8_t* dead_callback;
    void* dead_decider;
    void* message;
    void* yesno_ask_this;
    uint8_t* dead_set;
    uint8_t* yesno_input;
    void* box_confirm[2];
    void* box_setup[2];
    uint8_t* askers[16];
    void* callbacks[16];
    uint8_t* gil_record;
    void* asked_ask;
    uint8_t* asked_sites[11];
    uint8_t* player_data;
    uint8_t* gm_queue;
    int32_t* gm_slot;
    volatile LONG* gm_polls;
    volatile LONG* error_screen;
    const void* const* door_vtable;
    uint8_t* door_input;
    uint8_t* door_handler;
    uint8_t* door_opened;
    uint8_t* door_open_hook;
    uint8_t* door_tick;
    uint8_t** house_ptr;
    uint8_t* door_actor;
    uint8_t* door_exiting;
    uint32_t* door_gate_answer;
    volatile LONG* door_garden_on;
    volatile LONG* door_floor_on;
    uint32_t* door_condition;
    const void* const* dialogue_vtables;
    uint8_t* dialogue_input;
    uint8_t* dialogue_tick;
    uint8_t* key_test;
    uint8_t* dialogue_wait;
    uint8_t* texts;
    volatile LONG* left_click;
    volatile LONG* dialogue_ticks;
    uint8_t* edit;
    char* field_typed;
    uint8_t* field_boxes;
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
    uint8_t* text_inputs[6];
    void* menu_draw;
    void* sprite_draw;
    void* quad_draw;
    uint8_t* target_draw;
    uint8_t* comment_start;
    volatile LONG* draw_walk;
    volatile LONG* party_swap;
    volatile LONG* action_swap;
    uint8_t* trade_input;
    uint8_t* trade_open;
    uint8_t* handover_input;
    uint8_t* inventor_rows;
    uint8_t* trade_stage;
    uint8_t* player;
    uint8_t* intervals;
    uint8_t* clock;
    uint8_t** actors;
    uint8_t* item_data;
    uint32_t* gray_answer;
    uint32_t* send_answer;
    uint8_t** status_mode;
    int32_t* gray_index;
    void* walk_now;
    volatile LONG* active_pick;
    void* primitive_draw;
    volatile LONG* close_draws;
    uint8_t** conq_data;
    uint32_t* open_wait;
    uint8_t* map0_input;
    uint8_t* map_close;
    uint8_t* equip_open;
    uint8_t* equip_shortcut;
    uint8_t* monstrosity;
    uint8_t* magic_open;
    uint8_t* mon_player;
    uint8_t* game_mode;
    uint8_t* set_refusal;
    volatile LONG* no_pet;
    volatile LONG* equipment_opens;
    volatile LONG* stats_requests;
    volatile LONG* magic_lists;
    int32_t* magic_last;
    int32_t* ability_last;
    volatile LONG* magic_closes;
    volatile LONG* magic_close_wild;
    FakeSets* sets;
    uint8_t* job_flags;
    uint8_t* main_job;
    uint8_t* sub_job;
    uint8_t* macro_editor;
    uint8_t** macro_editor_ptr;
    int32_t* last_overlap;
    FakeRoutes* routes;
    uint8_t** mog_house_ptr;
    uint8_t** nomad_ptr;
    uint8_t* mog_house;
    uint8_t* route_player;
    uint8_t* items_object;
    int8_t* nation;
    uint8_t** policy_ptr;
    FakeSettings* settings;
    uint8_t* ime;
    int* line_mode;
};

// fake_arealist.h's Log, field for field.
struct AreaLog {
    int inputs;
    int last_input;
    int keys;
    int searches;
    int last_search;
    int closes;
    int scsibori;
    int rebuilds;
    int selects;
    int latch_clears;
};

// query's confirm, then signatures.h's kCalls, in order.
const int kFakeCalls = 13;
// Where the stand-in's dialogue tick resumes after its call of the key test.
const int kFakeClickReturn = 21;
const char* const kCallNames[kFakeCalls] = {
    "query_confirm", "passinpu_reset", "prtyjoin_send", "prtyjoin_clear", "link5_clear",
    "arealist_close", "arealist_latch", "post_request", "post_request_close", "dock_reset", "template_swap",
    "set_frame_rect", "select_row",
};

FakeState* g_fake;

typedef void (__fastcall* UpdateFn)(void* mcb, void* edx);
typedef void* (__fastcall* OpenFn)(void* mcb, void* edx, const char* key, int activate, int overlap);
typedef void (__cdecl* AbilityOpenFn)(int kind, int flag, int extra);
typedef int (__cdecl* YesNoAskFn)(const char* name, int default_yes, int cancellable, int mode, int chain);

volatile LONG g_stop;
volatile LONG g_pending;
volatile LONG g_pause;
volatile LONG g_paused;
const char* g_open_key;
void* g_open_result;
HANDLE g_open_done;
volatile LONG g_ability_pending;
int g_ability_args[3];
volatile LONG g_ask_pending;
const char* g_ask_name;
int g_ask_args[4];
int g_ask_result;
volatile LONG g_box_pending;
int g_box_args[6];                      // box, initial, max, callback, context, preview
void* g_box_result;
volatile LONG g_asked_pending;
int g_asked_args[2];                    // core.h's kAsked*, then what the stand-in's asker takes
int g_asked_result;
typedef int (__cdecl* AskedAskFn)(int which, int arg);
// A Lua chunk the game thread runs after a frame's update, as Windower's Lua
// would run if it shared the game's thread.
volatile LONG g_lua_pending;
lua_State* g_lua_state;
const char* g_lua_chunk;

typedef void* (__fastcall* MoneySetupFn)(void* ctl, void* edx, int initial, int max, const void* cb, void* ctx);
typedef void* (__fastcall* ItemSetupFn)(void* ctl, void* edx, int initial, int max, const void* cb, void* ctx,
                                        int preview);

// The open routine's last argument the game's own openers pass for the
// window `key` names: 1 for the four they open parked, else 0.
int game_overlap(const char* key) {
    static const char* const parked[] = {"menu    gaugewin", "menu    targetwi", "menu    playermo",
                                         "menu    mp_pmode"};
    for (size_t i = 0; i < sizeof(parked) / sizeof(parked[0]); ++i) {
        if (strncmp(key, parked[i], 16) == 0) {
            return 1;
        }
    }
    return 0;
}

// Plays the game's main thread: runs a frame every couple of milliseconds
// and carries out the opens the harness asks for, on this thread.
DWORD WINAPI game_thread(LPVOID) {
    UpdateFn update;
    OpenFn open;
    memcpy(&update, &g_fake->ui_update, sizeof(update));
    memcpy(&open, &g_fake->open, sizeof(open));
    *g_fake->game_thread = GetCurrentThreadId();
    while (!g_stop) {
        if (g_pause) {
            InterlockedExchange(&g_paused, 1);
            Sleep(1);
            continue;
        }
        InterlockedExchange(&g_paused, 0);
        if (g_pending) {
            g_open_result = open(g_fake->mcb, NULL, g_open_key, 1, game_overlap(g_open_key));
            g_pending = 0;
            SetEvent(g_open_done);
        }
        if (g_ability_pending) {
            AbilityOpenFn ability_open;
            memcpy(&ability_open, &g_fake->ability_open, sizeof(ability_open));
            ability_open(g_ability_args[0], g_ability_args[1], g_ability_args[2]);
            g_ability_pending = 0;
            SetEvent(g_open_done);
        }
        if (g_ask_pending) {
            YesNoAskFn ask;
            memcpy(&ask, &g_fake->yesno_ask, sizeof(ask));
            g_ask_result = ask(g_ask_name, g_ask_args[0], g_ask_args[1], g_ask_args[2], g_ask_args[3]);
            g_ask_pending = 0;
            SetEvent(g_open_done);
        }
        if (g_box_pending) {
            // The box's setup on its controller (rows 45 and 80), as an
            // asker calls it.
            const int b = g_box_args[0];
            void* ctl = g_fake->ctl_global[b ? 80 : 45];
            const void* cb = reinterpret_cast<const void*>(g_box_args[3]);
            void* ctx = reinterpret_cast<void*>(g_box_args[4]);
            if (b == 0) {
                MoneySetupFn setup;
                memcpy(&setup, &g_fake->box_setup[0], sizeof(setup));
                g_box_result = setup(ctl, NULL, g_box_args[1], g_box_args[2], cb, ctx);
            } else {
                ItemSetupFn setup;
                memcpy(&setup, &g_fake->box_setup[1], sizeof(setup));
                g_box_result = setup(ctl, NULL, g_box_args[1], g_box_args[2], cb, ctx, g_box_args[5]);
            }
            g_box_pending = 0;
            SetEvent(g_open_done);
        }
        if (g_asked_pending) {
            AskedAskFn ask;
            memcpy(&ask, &g_fake->asked_ask, sizeof(ask));
            g_asked_result = ask(g_asked_args[0], g_asked_args[1]);
            g_asked_pending = 0;
            SetEvent(g_open_done);
        }
        update(g_fake->mcb, NULL);
        if (g_lua_pending) {
            run(g_lua_state, g_lua_chunk);
            g_lua_pending = 0;
            SetEvent(g_open_done);
        }
        Sleep(2);
    }
    return 0;
}

// No frame runs between these two: what Lua queues meanwhile waits.
void pause_game() {
    InterlockedExchange(&g_pause, 1);
    while (!g_paused) {
        Sleep(1);
    }
}

void resume_game() {
    InterlockedExchange(&g_pause, 0);
    while (g_paused) {
        Sleep(1);
    }
}

void* open_on_game_thread(const char* key) {
    g_open_key = key;
    g_pending = 1;
    WaitForSingleObject(g_open_done, 5000);
    return g_open_result;
}

// The ability opener as the game's callers run it, on the game thread.
void ability_open_on_game_thread(int kind, int flag, int extra) {
    g_ability_args[0] = kind;
    g_ability_args[1] = flag;
    g_ability_args[2] = extra;
    g_ability_pending = 1;
    WaitForSingleObject(g_open_done, 5000);
}

// The yes/no class's ask as the game's askers run it, on the game thread:
// 1 when the question went up. `chain` 1 or 2: its callback asks again, the
// new question's default no or yes.
int ask_on_game_thread(const char* name, int default_yes, int cancellable, int mode, int chain) {
    g_ask_name = name;
    g_ask_args[0] = default_yes;
    g_ask_args[1] = cancellable;
    g_ask_args[2] = mode;
    g_ask_args[3] = chain;
    g_ask_pending = 1;
    WaitForSingleObject(g_open_done, 5000);
    return g_ask_result;
}

// A number box's setup as an asker runs it, on the game thread: box 0
// moneyctr, 1 itemctrl; the window when it opened.
void* box_ask_on_game_thread(int box, int initial, int max, const void* cb, const void* ctx, int preview = 0) {
    g_box_args[0] = box;
    g_box_args[1] = initial;
    g_box_args[2] = max;
    g_box_args[3] = static_cast<int>(reinterpret_cast<uintptr_t>(cb));
    g_box_args[4] = static_cast<int>(reinterpret_cast<uintptr_t>(ctx));
    g_box_args[5] = preview;
    g_box_pending = 1;
    WaitForSingleObject(g_open_done, 5000);
    return g_box_result;
}

// An Asked window asked as the game's askers ask it, on the game thread:
// `which` core.h's kAsked*, `arg` what fake_ffximain.cpp's fake_asked_ask
// takes for it; what that answers.
int asked_on_game_thread(int which, int arg) {
    g_asked_args[0] = which;
    g_asked_args[1] = arg;
    g_asked_pending = 1;
    WaitForSingleObject(g_open_done, 5000);
    return g_asked_result;
}

// A phase run by the game thread after its next frame's update; false when
// it has not come back within five seconds.
bool phase_on_game_thread(lua_State* L, const char* chunk) {
    g_checks += count_checks(chunk);
    g_lua_state = L;
    g_lua_chunk = chunk;
    g_lua_pending = 1;
    return WaitForSingleObject(g_open_done, 5000) == WAIT_OBJECT_0;
}

// The step the watchdog names: a run that has not reached the watchdog's end
// within its time fails there, hung, and stops.
const char* volatile g_step = "";
HANDLE g_watch_done;

DWORD WINAPI watchdog(LPVOID ms) {
    if (WaitForSingleObject(g_watch_done, static_cast<DWORD>(reinterpret_cast<uintptr_t>(ms))) != WAIT_OBJECT_0) {
        std::printf("FAIL  hung at: %s\n", g_step);
        std::fflush(stdout);
        TerminateProcess(GetCurrentProcess(), 3);
    }
    return 0;
}

// Where the call or jmp at `at` goes.
uint8_t* call_target(uint8_t* at) {
    int32_t rel;
    memcpy(&rel, at + 1, 4);
    return at + 5 + rel;
}

// The hooked open from this thread, for an open that must come while no
// frame runs: only between pause_game() and resume_game().
void* open_here(const char* key) {
    OpenFn open;
    memcpy(&open, &g_fake->open, sizeof(open));
    return open(g_fake->mcb, NULL, key, 1, game_overlap(key));
}

void wait_frames(int n) {
    const LONG target = *g_fake->frames + n;
    const DWORD start = GetTickCount();
    while (*g_fake->frames < target && GetTickCount() - start < 5000) {
        Sleep(1);
    }
}

uint8_t* registry_row(int r) {
    return g_fake->registry + r * 0x2C;
}

// One walk of the drawn layers, made here while no frame runs: 'd' when it
// calls the per-menu draw for `menu` and the device draws what it paints,
// 'h' when the draw runs and the device draws nothing of it, 's' when the
// call's pre skips the draw, '-' when the walk never calls it. Never between
// pause_game() and resume_game().
char walk_of(const uint8_t* menu) {
    typedef void (__cdecl* WalkFn)();
    WalkFn walk;
    memcpy(&walk, &g_fake->walk_now, sizeof(walk));
    pause_game();
    walk();
    const FakeLog* fl = g_fake->log;
    bool walked = false;
    bool drawn = false;
    bool painted = false;
    for (int i = 0; i < fl->walked_count; ++i) {
        walked = walked || fl->walked[i] == menu;
    }
    for (int i = 0; i < fl->drawn_count; ++i) {
        drawn = drawn || fl->drawn[i] == menu;
    }
    for (int i = 0; i < fl->painted_count; ++i) {
        painted = painted || fl->painted[i] == menu;
    }
    resume_game();
    return painted ? 'd' : drawn ? 'h' : walked ? 's' : '-';
}

// A call parked inside a hooked routine: the template swap called with its
// menu on a page nothing may read, so the engine's pre faults there and the
// handler holds the thread until the harness lets it go.
uint8_t* g_trap;
HANDLE g_trap_entered;
HANDLE g_trap_release;

LONG CALLBACK trap_handler(EXCEPTION_POINTERS* x) {
    const EXCEPTION_RECORD* r = x->ExceptionRecord;
    if (r->ExceptionCode != EXCEPTION_ACCESS_VIOLATION || r->NumberParameters < 2 || !g_trap) {
        return EXCEPTION_CONTINUE_SEARCH;
    }
    const uint8_t* at = reinterpret_cast<const uint8_t*>(r->ExceptionInformation[1]);
    if (at < g_trap || at >= g_trap + 4096) {
        return EXCEPTION_CONTINUE_SEARCH;
    }
    SetEvent(g_trap_entered);
    WaitForSingleObject(g_trap_release, INFINITE);
    DWORD old;
    VirtualProtect(g_trap, 4096, PAGE_READWRITE, &old);
    return EXCEPTION_CONTINUE_EXECUTION;
}

DWORD WINAPI swap_into_trap(LPVOID) {
    typedef void (__fastcall* SwapFn)(void* menu, void* edx, const char* key);
    SwapFn swap;
    memcpy(&swap, &g_fake->calls[10], sizeof(swap));
    swap(g_trap, NULL, "menu    trapped ");
    return 0;
}

// A window the engine hides, as the game holds it: the walk calls its draw,
// which runs and paints nothing, its live menu and its registry row at the
// row's own draw layer.
bool hidden_in_place(int r, const uint8_t* menu) {
    const uint8_t layer = g_fake->pristine[r * 0x2C + 0x29];
    return menu && menu[0x60] == layer && registry_row(r)[0x29] == layer && walk_of(menu) == 'h';
}

uint8_t* live_menu(int r) {
    uint8_t* ctl = g_fake->ctl_global[r];
    uint8_t* menu = NULL;
    if (ctl) {
        memcpy(&menu, ctl + 8, 4);
    }
    return menu;
}

int16_t origin(uint8_t* menu, int axis) {
    int16_t v;
    memcpy(&v, menu + 0x52 + 2 * axis, 2);
    return v;
}

int count_of(const char* text, char c) {
    int n = 0;
    for (; *text; ++text) {
        n += *text == c;
    }
    return n;
}

void poke(uint8_t* at, uint8_t value) {
    DWORD old;
    VirtualProtect(at, 1, PAGE_EXECUTE_READWRITE, &old);
    *at = value;
    VirtualProtect(at, 1, old, &old);
    FlushInstructionCache(GetCurrentProcess(), at, 1);
}

const int kRowLog = 31;
const int kRowEquip = 41;
const int kRowMenuwind = 0;
const int kRowBuff = 260;
const int kRowDbdelsel = 327;

bool registry_pristine() {
    return memcmp(g_fake->registry, g_fake->pristine, 371 * 0x2C) == 0;
}

int row_named(const char* name) {
    char key[16];
    memcpy(key, "menu    ", 8);
    memset(key + 8, ' ', 8);
    memcpy(key + 8, name, strlen(name));
    for (int r = 0; r < 370; ++r) {
        if (memcmp(registry_row(r), key, 16) == 0) {
            return r;
        }
    }
    return -1;
}

uint8_t* controller_named(const char* name) {
    return g_fake->ctl_global[row_named(name)];
}

void put16(uint8_t* p, size_t off, int v) {
    const int16_t s = static_cast<int16_t>(v);
    memcpy(p + off, &s, 2);
}

void put32(uint8_t* p, size_t off, uint32_t v) {
    memcpy(p + off, &v, 4);
}

int16_t get16(const uint8_t* p, size_t off) {
    int16_t v;
    memcpy(&v, p + off, 2);
    return v;
}

uint32_t get32(const uint8_t* p, size_t off) {
    uint32_t v;
    memcpy(&v, p + off, 4);
    return v;
}

// A fabricated element list on a menu: a frame, a list item whose text is at
// +0x40, one whose text is only at +0x44, and a tombstoned item.
struct Elements {
    uint8_t nodes[4][0x18];
    uint8_t items[4][0x60];
};
Elements g_elements;
const char g_text_main[] = "Party";
const char g_text_alt[] = "alt";

void give_elements(uint8_t* menu) {
    memset(&g_elements, 0, sizeof(g_elements));
    const int16_t types[4] = {0, 1, 1, 1};
    for (int i = 0; i < 4; ++i) {
        uint8_t* node = g_elements.nodes[i];
        uint8_t* item = g_elements.items[i];
        put32(node, 0, i < 3 ? reinterpret_cast<uint32_t>(g_elements.nodes[i + 1]) : 0);
        put32(node, 0x10, reinterpret_cast<uint32_t>(item));
        node[0x14] = i == 3 ? 1 : 0;
        put16(item, 0, types[i]);
        put16(item, 0x2A, 10 * i);
        put16(item, 0x2C, 20 * i);
        put16(item, 0x22, 30);
        put16(item, 0x24, 8);
    }
    put32(g_elements.items[0], 0x40, reinterpret_cast<uint32_t>(g_text_main));
    put32(g_elements.items[1], 0x40, reinterpret_cast<uint32_t>(g_text_main));
    put32(g_elements.items[2], 0x44, reinterpret_cast<uint32_t>(g_text_alt));
    put32(menu, 0x14, reinterpret_cast<uint32_t>(g_elements.nodes[0]));
}

// One element of each type, each box where its type keeps it and junk where
// another type keeps one: a frame, a list item whose +0x1C and +0x20 hold
// its sprite's float scales, the cursor, and a type the draw does not know.
Elements g_typed;

void give_typed_elements(uint8_t* menu) {
    memset(&g_typed, 0, sizeof(g_typed));
    const int16_t types[4] = {0, 1, 2, 7};
    for (int i = 0; i < 4; ++i) {
        uint8_t* node = g_typed.nodes[i];
        uint8_t* item = g_typed.items[i];
        put32(node, 0, i < 3 ? reinterpret_cast<uint32_t>(g_typed.nodes[i + 1]) : 0);
        put32(node, 0x10, reinterpret_cast<uint32_t>(item));
        for (int k = 2; k < 0x40; k += 2) {
            put16(item, k, -1);
        }
        put16(item, 0, types[i]);
    }
    uint8_t* frame = g_typed.items[0];
    put16(frame, 0x1A, 5);
    put16(frame, 0x1C, 6);
    put16(frame, 0x22, 300);
    put16(frame, 0x24, 80);
    uint8_t* row = g_typed.items[1];
    put32(row, 0x1C, 0x3F800000);
    put32(row, 0x20, 0x3F800000);
    put16(row, 0x2A, 11);
    put16(row, 0x2C, 12);
    put16(row, 0x2E, 90);
    put16(row, 0x30, 16);
    uint8_t* cursor = g_typed.items[2];
    put16(cursor, 0x20, 33);
    put16(cursor, 0x22, 44);
    put32(menu, 0x14, reinterpret_cast<uint32_t>(g_typed.nodes[0]));
}

// An engine built with -DHU_HEAPCHECK exports core.h's HeapCheck as
// hideui_heapcheck, read here as far as its labels; core.h's HeapRecord is
// the process's section, which outlives each image.
struct HeapView {
    char magic[16];
    volatile LONG failed;
    volatile LONG checks;
    volatile LONG busy;
    LONG heaps;
    LONG last_us;
    LONG max_us;
    LONG total_us;
    LONG image;
    LONG good_image;
    LONG good_tick;
    LONG good_counter;
    LONG fail_image;
    LONG fail_tick;
    LONG fail_counter;
    LONG fail_thread;
    LONG fail_heap;
    void* fail_handle;
    LONG command_tick;
    const char* outside_step;
    LONG outside_left;
    LONG outside_tick;
    LONG background_tick;
    char good[128];
    char fail[128];
    char command[48];
};

struct HeapRecordView {
    uint32_t magic;
    volatile LONG images;
    volatile LONG failed;
    LONG fail_image;
    LONG fail_tick;
    LONG good_image;
    LONG good_tick;
    char fail[128];
    char good[128];
};

HeapView* heap_view() {
    HMODULE engine = GetModuleHandleA("_HideUI.dll");
    return engine ? reinterpret_cast<HeapView*>(GetProcAddress(engine, "hideui_heapcheck")) : NULL;
}

template <typename T>
T fake_export(const char* name) {
    T fn = NULL;
    FARPROC p = GetProcAddress(GetModuleHandleA("FFXiMain.dll"), name);
    memcpy(&fn, &p, sizeof(fn));
    return fn;
}

template <typename F>
bool wait_for(F done, DWORD ms) {
    const DWORD start = GetTickCount();
    while (!done()) {
        if (GetTickCount() - start > ms) {
            return false;
        }
        Sleep(1);
    }
    return true;
}

// A new engine and handle, and heap_errors(): the reasons of the error
// events the handle reads, as "verb name: reason", joined by " | ".
const char kHeapStart[] =
    "hu = load_engine()\n"
    "h = hu.new('heapcheck')\n"
    "check(h ~= nil and hu.status().ok, 'an engine built with -DHU_HEAPCHECK installs and makes a handle')\n"
    "function heap_errors()\n"
    "  local out = {}\n"
    "  for _, e in ipairs((h:poll())) do\n"
    "    if e.event == 'error' then out[#out + 1] = e.verb .. ' ' .. e.name .. ': ' .. e.reason end\n"
    "  end\n"
    "  return table.concat(out, ' | ')\n"
    "end\n"
    "return finish()\n";

const char kHeapStop[] =
    "check(h:release() == true and hu.shutdown() == true, 'the handle released and the engine shut down')\n"
    "return finish()\n";

// The heap check of an engine built with -DHU_HEAPCHECK, against the
// stand-in's overrun of a block in a heap of its own (fake_ffximain.cpp's
// fake_heap_overrun): no check failed in the run before it; a check's cost;
// the overrun inside map0's callback, on the leaf of maplist's steps
// answered hidden, failing the check after the callback with the one before
// it the last good, reported once to the handle; the overrun outside every
// command and hook caught by the background check within its second, named
// by the last command; a shutdown checking to its unhook, and an overrun
// between two images failing the next image's first check, the earlier
// image's last check named; a third image reporting the second's failure.
// Between the cases the overrun is put back and the latch and the section
// cleared, as in a fresh process.
void heap_checks(const char* const* globals) {
    typedef void (*VoidFn)();
    typedef int (*LoadFn)(int);
    const VoidFn arm = fake_export<VoidFn>("fake_heap_arm");
    const VoidFn overrun = fake_export<VoidFn>("fake_heap_overrun");
    const VoidFn repair = fake_export<VoidFn>("fake_heap_repair");
    const LoadFn load = fake_export<LoadFn>("fake_heap_load");
    lua_State* L = fresh(globals);
    phase(L, kHeapStart);
    HeapView* hv = heap_view();
    char name[64];
    snprintf(name, sizeof(name), "Local\\hideui_heapcheck_v1_%08X", static_cast<unsigned>(GetCurrentProcessId()));
    HANDLE mapping = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, name);
    HeapRecordView* rec = mapping
        ? static_cast<HeapRecordView*>(MapViewOfFile(mapping, FILE_MAP_ALL_ACCESS, 0, 0, sizeof(HeapRecordView)))
        : NULL;
    if (!arm || !overrun || !repair || !load || !hv || !rec) {
        check(false, "the stand-in's overrun, the engine's hideui_heapcheck and the process's section are there");
        lua.close(L);
        return;
    }
    char text[640];
    snprintf(text, sizeof(text),
        "no check failed in this run before this section: engine image %ld of the process, %ld checks so far,"
        " the last good: %s", static_cast<long>(hv->image), static_cast<long>(hv->checks), hv->good);
    check(strcmp(hv->magic, "hideui heap 1") == 0 && hv->failed == 0 && rec->failed == 0 && hv->checks >= 2
            && rec->images == hv->image, text);

    // a check's cost: the background check's, the process as it is, then
    // with a heap of 100000 and of 1000000 blocks more
    {
        const int blocks[3] = {0, 100000, 1000000};
        LONG cost[3];
        LONG heaps = 0;
        bool measured = true;
        for (int i = 0; i < 3; ++i) {
            const int made = load(blocks[i]);
            const LONG from = hv->checks;
            measured = wait_for([&] { return hv->checks >= from + 2; }, 5000) && made == blocks[i] && measured;
            cost[i] = hv->last_us;
            heaps = i == 0 ? hv->heaps : heaps;
        }
        load(0);
        snprintf(text, sizeof(text),
            "a check's cost: %ld heaps walked in %ld us; with a heap of 100000 blocks more %ld us, of 1000000"
            " %ld us", static_cast<long>(heaps), static_cast<long>(cost[0]), static_cast<long>(cost[1]),
            static_cast<long>(cost[2]));
        check(measured && hv->failed == 0, text);
    }

    // the overrun inside map0's callback: maplist hidden, asked from map0,
    // a region answered, canceled back, answered again, then a map, the
    // leaf: the callback, then close+reset
    FakeLog* al = g_fake->log;
    phase(L,
        "check(h:hide('maplist') == true, 'hide(maplist)')\n"
        "return finish()\n");
    wait_frames(3);
    asked_on_game_thread(4, 100);
    wait_frames(2);
    phase(L,
        "local o = h:options('maplist')\n"
        "check(o and o.level == 'top' and #o.rows == 4 and h:info('maplist').hidden == true,"
        "  'maplist up, hidden, at its top')\n"
        "check(h:answer('maplist', 2) == true, 'answer(maplist, 2): a region')\n"
        "return finish()\n");
    wait_frames(3);
    phase(L,
        "check(h:options('maplist').level == 'region' and h:cancel('maplist') == true, 'cancel(maplist): back up')\n"
        "return finish()\n");
    wait_frames(3);
    phase(L,
        "check(h:options('maplist').level == 'top' and h:answer('maplist', 2) == true, 'the region again')\n"
        "return finish()\n");
    wait_frames(3);
    snprintf(text, sizeof(text), "no check failed through the steps between levels; the last good: %s", hv->good);
    check(hv->failed == 0 && hv->good[0], text);
    const int calls = al->asked_calls;
    arm();
    phase(L,
        "local o = h:options('maplist')\n"
        "check(o.level == 'region' and o.rows[1].kind == 'map' and h:answer('maplist', 1) == true,"
        "  'answer(maplist, 1): a map, the leaf, the overrun armed in map0\\'s callback')\n"
        "return finish()\n");
    wait_frames(3);
    snprintf(text, sizeof(text),
        "map0's callback ran once with the entry {100, 1} and overran: the check after it failed, \"%s\", the one"
        " before it the last good, \"%s\"; heap %ld of the process's, crumb counter %ld", hv->fail, hv->good,
        static_cast<long>(hv->fail_heap), static_cast<long>(hv->fail_counter));
    check(al->asked_calls == calls + 1 && al->asked_who == 4 && al->asked_value == 100 && al->asked_extra == 1
            && hv->failed == 1 && strcmp(hv->fail, "after map0 callback") == 0
            && strcmp(hv->good, "before map0 callback") == 0 && hv->fail_heap > 0 && hv->fail_counter > 0
            && rec->failed == 1 && strcmp(rec->fail, "after map0 callback") == 0, text);
    wait_frames(2);
    phase(L,
        "local errors = heap_errors()\n"
        "check(errors:find('debug heap: heap check failed: after map0 callback (last good: before map0 callback, ',"
        "  1, true) == 1 and not errors:find(' | ', 1, true), 'reported once, to the handle: ' .. errors)\n"
        "local d = hu.status().detail\n"
        "check(tostring(d.drain_error):find('heap check failed: after map0 callback', 1, true) == 1,"
        "  'and as status()\\'s drain error: ' .. tostring(d.drain_error))\n"
        "check(h:unhide('maplist') == true, 'unhide(maplist)')\n"
        "return finish()\n");
    const LONG checks = hv->checks;
    Sleep(1200);
    wait_frames(2);
    check(hv->checks == checks && hv->failed == 1 && !live_menu(row_named("maplist")),
        "past the failure no check runs, the background's included; maplist closed by close+reset");
    phase(L,
        "local errors = heap_errors()\n"
        "check(errors == '', 'and nothing more is reported: ' .. errors)\n"
        "return finish()\n");
    {
        typedef int (__fastcall* CloseFn)(void* mcb, void* edx, const char* key);
        CloseFn close_by_name;
        memcpy(&close_by_name, &g_fake->close, sizeof(close_by_name));
        pause_game();
        close_by_name(g_fake->mcb, NULL, "menu    map0    ");
        resume_game();
    }

    // the overrun outside every command and hook, the game paused, caught by
    // the background check within its second
    repair();
    rec->failed = 0;
    hv->failed = 0;
    LONG from = hv->checks;
    const bool passed = wait_for([&] { return hv->checks > from && strcmp(hv->good, "background") == 0; }, 3000);
    pause_game();
    overrun();
    const DWORD overran = GetTickCount();
    resume_game();
    const bool caught = wait_for([&] { return hv->failed != 0; }, 3000);
    snprintf(text, sizeof(text), "the overrun outside every command and hook caught %lu ms later: \"%s\"",
        static_cast<unsigned long>(static_cast<DWORD>(hv->fail_tick) - overran), hv->fail);
    check(passed && caught && hv->failed == 1 && static_cast<DWORD>(hv->fail_tick) - overran <= 1500
            && strstr(hv->fail, "background (last command asked_answer maplist, ") == hv->fail
            && strstr(hv->fail, "; last step outside the drain ") && strcmp(hv->good, "background") == 0, text);
    wait_frames(2);
    phase(L,
        "local errors = heap_errors()\n"
        "check(errors:find('debug heap: heap check failed: background (last command asked_answer maplist, ', 1, true)"
        "  == 1 and errors:find('(last good: background, ', 1, true), 'reported: ' .. errors)\n"
        "return finish()\n");

    // a shutdown checking to its unhook; the overrun between two images
    repair();
    rec->failed = 0;
    hv->failed = 0;
    const LONG image = hv->image;
    from = hv->checks;
    wait_for([&] { return hv->checks > from; }, 3000);
    phase(L, kHeapStop);
    settle(L);
    lua.close(L);
    check(engine_unmaps(), "the engine unmaps");
    snprintf(text, sizeof(text), "the shutdown's checks ran to the end: the section's last good, image %ld's, \"%s\"",
        static_cast<long>(rec->good_image), rec->good);
    check(rec->failed == 0 && rec->good_image == image && strcmp(rec->good, "shutdown thread: after unhook") == 0,
        text);
    overrun();
    L = fresh(globals);
    phase(L, kHeapStart);
    hv = heap_view();
    snprintf(text, sizeof(text), "the next image's first check failed: \"%s\" (image %ld), the last good image %ld's"
        " \"%s\"", hv ? hv->fail : "", hv ? static_cast<long>(hv->image) : 0L,
        hv ? static_cast<long>(hv->good_image) : 0L, hv ? hv->good : "");
    check(hv && hv->image == image + 1 && hv->failed == 1 && strcmp(hv->fail, "install: before the hooks") == 0
            && hv->good_image == image && strcmp(hv->good, "shutdown thread: after unhook") == 0, text);
    Sleep(1200);
    wait_frames(2);
    char chunk[768];
    snprintf(chunk, sizeof(chunk),
        "local errors = heap_errors()\n"
        "check(errors:find('debug heap: heap check failed: install: before the hooks (last good: shutdown thread:"
        " after unhook, engine image %ld, ', 1, true) == 1, 'reported to the new handle a second on: ' .. errors)\n"
        "return finish()\n", static_cast<long>(image));
    phase(L, chunk);

    // a third image: the second's failure reported, no check run
    phase(L, kHeapStop);
    settle(L);
    lua.close(L);
    check(engine_unmaps(), "the engine unmaps");
    L = fresh(globals);
    phase(L, kHeapStart);
    hv = heap_view();
    check(hv && hv->image == image + 2 && hv->failed == 2 && hv->fail_image == image + 1 && hv->checks == 0
            && strcmp(hv->fail, "install: before the hooks") == 0,
        "the third image takes the second's failure as its own and runs no check");
    Sleep(1200);
    wait_frames(2);
    snprintf(chunk, sizeof(chunk),
        "local errors = heap_errors()\n"
        "check(errors:find('debug heap: heap check failed in engine image %ld: install: before the hooks (last good:"
        " shutdown thread: after unhook, engine image %ld, ', 1, true) == 1, 'and reports it: ' .. errors)\n"
        "return finish()\n", static_cast<long>(image + 1), static_cast<long>(image));
    phase(L, chunk);
    phase(L, kHeapStop);
    settle(L);
    lua.close(L);
    check(engine_unmaps(), "the engine unmaps");
    repair();
    rec->failed = 0;
    UnmapViewOfFile(rec);
    CloseHandle(mapping);
}

}  // namespace

// A thread made suspended and terminated before it runs: its context is set
// to stand inside a hooked routine's prologue.
DWORD WINAPI never_runs(LPVOID) {
    return 0;
}

int main(int argc, char** argv) {
    if (argc < 14) {
        std::printf("usage: e2e_test.exe <LuaCore.dll> <a/_HideUI.dll> <FFXiMain.dll> <b/_HideUI.dll>"
            " <c/_HideUI.dll> <hideui.lua> <d/_HideUI.dll> <e/_HideUI.dll> <f/_HideUI.dll> <g/_HideUI.dll>"
            " <h/_HideUI.dll> <i/_HideUI.dll> <j/_HideUI.dll>\n");
        return 2;
    }
    for (int i = 2; i < 14; ++i) {
        to_backslashes(argv[i]);
    }
    g_dll = argv[2];
    if (!load_lua(argv[1])) {
        std::printf("FAIL  could not load %s\n", argv[1]);
        return 1;
    }

    // -- no FFXiMain.dll in the process
    {
        lua_State* L = fresh(NULL);
        phase(L,
            "local hu = load_engine()\n"
            "local h, why = hu.new('a')\n"
            "local s = hu.status()\n"
            "local d = s.detail or {}\n"
            "check(h == nil and why == \"hideui can't work with this version of FFXI; update the addon\","
            "  'new with the engine down: nil and a reason a player can act on: ' .. tostring(why))\n"
            "check(not s.ok and s.state == 'failed' and s.role == 'none' and s.error == nil"
            "  and tostring(d.error):find('FFXiMain', 1, true),"
            "  'no FFXiMain.dll: status().detail.error names the failure: ' .. tostring(d.error))\n"
            "check(s.engine == '0.11.0' and s.handles == 0 and s.dropped == 0 and type(s.hidden) == 'table'"
            "  and type(s.resized) == 'table' and d.image and d.image.abi == 9 and s.ui == nil,"
            "  'status: the public fields at the top, the image under detail')\n"
            "local ok, err = pcall(hu.new, 42)\n"
            "check(not ok and tostring(err):find('string expected', 1, true), 'new(42) raises: ' .. tostring(err))\n"
            "check(hu.shutdown() == true, 'shutdown of an engine that never installed is a no-op')\n"
            "check(hu.version() == 'hideui 0.11.0', 'version: ' .. hu.version())\n"
            "return finish()\n");
        lua.close(L);
        check(!engine_mapped(), "a failed engine holds no pin: closing Lua unmaps it");
    }

    HMODULE fake = LoadLibraryA(argv[3]);
    typedef FakeState* (*StateFn)();
    StateFn state_fn = NULL;
    if (fake) {
        FARPROC p = GetProcAddress(fake, "fake_state");
        memcpy(&state_fn, &p, sizeof(p));
    }
    if (!state_fn) {
        std::printf("FAIL  could not load the stand-in FFXiMain.dll from %s\n", argv[3]);
        return 1;
    }
    g_fake = state_fn();
    g_open_done = CreateEventA(NULL, FALSE, FALSE, NULL);
    HANDLE game = CreateThread(NULL, 0, &game_thread, NULL, 0, NULL);
    wait_frames(2);
    const uint8_t open_first = static_cast<uint8_t*>(g_fake->open)[0];
    const uint8_t update_first = static_cast<uint8_t*>(g_fake->ui_update)[0];

    const int kData = 9 + kFakeCalls;   // the first of the five data addresses
    char addresses[kData + 5][16];
    char call_globals[kFakeCalls][24];
    void* const fns[] = {g_fake->open, g_fake->ui_update, g_fake->staged_close, g_fake->show_path,
                         g_fake->set_position, g_fake->close, g_fake->registry, g_fake->mcb,
                         g_fake->party_pending};
    for (int i = 0; i < 9; ++i) {
        snprintf(addresses[i], sizeof(addresses[i]), "0x%08X",
            static_cast<unsigned>(reinterpret_cast<uintptr_t>(fns[i])));
    }
    for (int i = 0; i < kFakeCalls; ++i) {
        snprintf(addresses[9 + i], sizeof(addresses[9 + i]), "0x%08X",
            static_cast<unsigned>(reinterpret_cast<uintptr_t>(g_fake->calls[i])));
        // The stand-in still has query's confirm; the engine resolves it no
        // longer, so it is no F_ global.
        snprintf(call_globals[i], sizeof(call_globals[i]), "%s_%s", i == 0 ? "UNRESOLVED" : "F", kCallNames[i]);
    }
    const void* const data[5] = {g_fake->link5_cache, g_fake->query_cancel_allowed, g_fake->set_cursor,
                                 g_fake->link5_latch, g_fake->link5_callback};
    for (int i = 0; i < 5; ++i) {
        snprintf(addresses[kData + i], sizeof(addresses[kData + i]), "0x%08X",
            static_cast<unsigned>(reinterpret_cast<uintptr_t>(data[i])));
    }
    char convert_address[16];
    snprintf(convert_address, sizeof(convert_address), "0x%08X",
        static_cast<unsigned>(reinterpret_cast<uintptr_t>(g_fake->glyph_convert)));
    char mouse_addresses[3][16];
    const void* const mouse_fns[3] = {g_fake->mouse_mode, g_fake->row_hit_test, g_fake->mouse_read};
    for (int i = 0; i < 3; ++i) {
        snprintf(mouse_addresses[i], sizeof(mouse_addresses[i]), "0x%08X",
            static_cast<unsigned>(reinterpret_cast<uintptr_t>(mouse_fns[i])));
    }
    char input_addresses[2][16];
    const void* const input_fns[2] = {g_fake->menu_input, g_fake->menu_routing};
    for (int i = 0; i < 2; ++i) {
        snprintf(input_addresses[i], sizeof(input_addresses[i]), "0x%08X",
            static_cast<unsigned>(reinterpret_cast<uintptr_t>(input_fns[i])));
    }
    char compass_addresses[2][16];
    const void* const compass_fns[2] = {g_fake->compass_draw, g_fake->compass_ptr};
    for (int i = 0; i < 2; ++i) {
        snprintf(compass_addresses[i], sizeof(compass_addresses[i]), "0x%08X",
            static_cast<unsigned>(reinterpret_cast<uintptr_t>(compass_fns[i])));
    }
    char macro_addresses[2][16];
    const void* const macro_fns[2] = {g_fake->macro_gate, g_fake->macro_object_ptr};
    for (int i = 0; i < 2; ++i) {
        snprintf(macro_addresses[i], sizeof(macro_addresses[i]), "0x%08X",
            static_cast<unsigned>(reinterpret_cast<uintptr_t>(macro_fns[i])));
    }
    char ability_address[16];
    snprintf(ability_address, sizeof(ability_address), "0x%08X",
        static_cast<unsigned>(reinterpret_cast<uintptr_t>(g_fake->ability_open)));
    // The per-menu draw, the sprite draw, the quad routine, where the target
    // window's draw of each arrow returns, past the call in each of its two
    // arrow sequences, and the draw primitive.
    char draw_addresses[6][16];
    {
        static const uint8_t arrow[] = {0x8b, 0x4c, 0x8e, 0x78, 0x68, 0x00, 0x00, 0x80, 0x3f};
        static const uint8_t pick[] = {0x8b, 0x4c, 0x8e, 0x78, 0x52, 0x50, 0xe8};
        const uint8_t* returns[2] = {NULL, NULL};
        for (int i = 0; i < 0x100 && !returns[1]; ++i) {
            const uint8_t* at = g_fake->target_draw + i;
            if (!returns[0] && memcmp(at, arrow, sizeof(arrow)) == 0) {
                returns[0] = at + 0x15;
            } else if (memcmp(at, pick, sizeof(pick)) == 0) {
                returns[1] = at + 0x0B;
            }
        }
        const void* const draw_fns[6] = {g_fake->menu_draw, g_fake->sprite_draw, g_fake->quad_draw, returns[0],
                                         returns[1], g_fake->primitive_draw};
        for (int i = 0; i < 6; ++i) {
            snprintf(draw_addresses[i], sizeof(draw_addresses[i]), "0x%08X",
                static_cast<unsigned>(reinterpret_cast<uintptr_t>(draw_fns[i])));
        }
    }
    // The number boxes' confirms and setups, the yes/no class's cancel test
    // inside its input routine, and two askers' callbacks: trade's item count
    // (its entry the signature) and handover's gil (a site's push).
    char box_addresses[7][16];
    {
        const uint8_t* input = g_fake->yesno_input;
        const uint8_t* test = NULL;
        for (int i = 0; i < 16 && !test; ++i) {
            if (input[i] == 0x8a && input[i + 1] == 0x4e && input[i + 2] == 0x39) {
                test = input + i;
            }
        }
        const void* const box_fns[7] = {g_fake->box_confirm[0], g_fake->box_confirm[1], g_fake->box_setup[0],
                                        g_fake->box_setup[1], test, g_fake->callbacks[8], g_fake->callbacks[1]};
        for (int i = 0; i < 7; ++i) {
            snprintf(box_addresses[i], sizeof(box_addresses[i]), "0x%08X",
                static_cast<unsigned>(reinterpret_cast<uintptr_t>(box_fns[i])));
        }
    }
    char yesno_address[16];
    snprintf(yesno_address, sizeof(yesno_address), "0x%08X",
        static_cast<unsigned>(reinterpret_cast<uintptr_t>(g_fake->yesno_vtable)));
    char dead_addresses[6][16];
    const void* const dead_fns[6] = {g_fake->yesno_confirm, g_fake->yesno_ask_this, g_fake->dead_tick,
                                     g_fake->dead_decider, g_fake->message, g_fake->dead_callback};
    for (int i = 0; i < 6; ++i) {
        snprintf(dead_addresses[i], sizeof(dead_addresses[i]), "0x%08X",
            static_cast<unsigned>(reinterpret_cast<uintptr_t>(dead_fns[i])));
    }
    // The Asked windows' sites, and close by instance, where the stand-in's
    // close by name calls it.
    char asked_addresses[7][16];
    {
        const void* const asked_fns[7] = {g_fake->asked_sites[0], g_fake->asked_sites[5], g_fake->asked_sites[7],
                                          g_fake->asked_sites[10],
                                          call_target(static_cast<uint8_t*>(g_fake->close) + 44),
                                          g_fake->map0_input, g_fake->map_close};
        for (int i = 0; i < 7; ++i) {
            snprintf(asked_addresses[i], sizeof(asked_addresses[i]), "0x%08X",
                static_cast<unsigned>(reinterpret_cast<uintptr_t>(asked_fns[i])));
        }
    }
    // mogdoor's routines and vtable, the dialogue boxes' input routine and
    // tick, the key test, and the return address of the tick's call of it.
    char talk_addresses[10][16];
    {
        const void* const talk_fns[10] = {g_fake->door_input, g_fake->door_handler, g_fake->door_vtable,
                                          g_fake->door_opened, g_fake->door_open_hook, g_fake->door_tick,
                                          g_fake->dialogue_input, g_fake->dialogue_tick, g_fake->key_test,
                                          g_fake->dialogue_tick + kFakeClickReturn};
        for (int i = 0; i < 10; ++i) {
            snprintf(talk_addresses[i], sizeof(talk_addresses[i]), "0x%08X",
                static_cast<unsigned>(reinterpret_cast<uintptr_t>(talk_fns[i])));
        }
    }
    // The trade windows' input routines, the trade's open and stage, and the
    // inventory list's rows.
    char offer_addresses[5][16];
    {
        const void* const offer_fns[5] = {g_fake->trade_input, g_fake->trade_open, g_fake->handover_input,
                                          g_fake->inventor_rows, g_fake->trade_stage};
        for (int i = 0; i < 5; ++i) {
            snprintf(offer_addresses[i], sizeof(offer_addresses[i]), "0x%08X",
                static_cast<unsigned>(reinterpret_cast<uintptr_t>(offer_fns[i])));
        }
    }
    const char* globals[] = {"OPEN", addresses[0], "UPDATE", addresses[1], "STAGED", addresses[2],
                             "SHOW", addresses[3], "SETPOS", addresses[4], "CLOSE", addresses[5],
                             "REGISTRY", addresses[6], "MCB", addresses[7], "PENDING", addresses[8],
                             call_globals[0], addresses[9], call_globals[1], addresses[10],
                             call_globals[2], addresses[11], call_globals[3], addresses[12],
                             call_globals[4], addresses[13], call_globals[5], addresses[14],
                             call_globals[6], addresses[15], call_globals[7], addresses[16],
                             call_globals[8], addresses[17], call_globals[9], addresses[18],
                             call_globals[10], addresses[19], call_globals[11], addresses[20],
                             call_globals[12], addresses[21],
                             "LINK5CACHE", addresses[kData], "CANCELBYTE", addresses[kData + 1],
                             "SETCURSOR", addresses[kData + 2], "LINK5LATCH", addresses[kData + 3],
                             "LINK5CB", addresses[kData + 4],
                             "CONVERT", convert_address, "MOUSE", mouse_addresses[0], "HITTEST", mouse_addresses[1],
                             "MOUSEREAD", mouse_addresses[2], "SINK", input_addresses[0],
                             "ROUTING", input_addresses[1], "COMPASS", compass_addresses[0],
                             "COMPASSGLOBAL", compass_addresses[1], "MACROGATE", macro_addresses[0],
                             "MACROOBJECT", macro_addresses[1], "ABILITYOPEN", ability_address,
                             "YESNOVTABLE", yesno_address, "YESNOCONFIRM", dead_addresses[0],
                             "YESNOASK", dead_addresses[1], "DEADTICK", dead_addresses[2],
                             "DEADDECIDER", dead_addresses[3], "MESSAGE", dead_addresses[4],
                             "DEADCALLBACK", dead_addresses[5], "MONEYCONFIRM", box_addresses[0],
                             "ITEMCONFIRM", box_addresses[1], "MONEYSETUP", box_addresses[2],
                             "ITEMSETUP", box_addresses[3], "YESNOCANCEL", box_addresses[4],
                             "ASKTRADEITEM", box_addresses[5], "ASKHANDOVERGIL", box_addresses[6],
                             "SORTYNSETUP", asked_addresses[0], "BANKOPEN", asked_addresses[1],
                             "MAPLISTCLOSE", asked_addresses[2], "OKINPUT", asked_addresses[3],
                             "CLOSEINSTANCE", asked_addresses[4], "MAP0INPUT", asked_addresses[5],
                             "MAPCLOSE", asked_addresses[6], "DOORINPUT", talk_addresses[0],
                             "DOORHANDLER", talk_addresses[1], "DOORVTABLE", talk_addresses[2],
                             "DOOROPENED", talk_addresses[3], "DOOROPENHOOK", talk_addresses[4],
                             "DOORTICK", talk_addresses[5], "DIALOGUEINPUT", talk_addresses[6],
                             "DIALOGUETICK", talk_addresses[7], "KEYTEST", talk_addresses[8],
                             "CLICKRETURN", talk_addresses[9], "MENUDRAW", draw_addresses[0],
                             "SPRITEDRAW", draw_addresses[1], "QUADDRAW", draw_addresses[2],
                             "TARGETRETURN", draw_addresses[3], "PICKRETURN", draw_addresses[4],
                             "PRIMITIVEDRAW", draw_addresses[5], "TRADEINPUT", offer_addresses[0],
                             "TRADEOPEN", offer_addresses[1], "HANDOVERINPUT", offer_addresses[2],
                             "INVENTORROWS", offer_addresses[3], "TRADESTAGE", offer_addresses[4], NULL};

    const char kExpectFailure[] =
        "local hu = load_engine()\n"
        "local h, why = hu.new('x')\n"
        "local s = hu.status()\n"
        "local e = s.detail and s.detail.error\n"
        "check(not s.ok and tostring(e):find(EXPECT, 1, true), 'fails closed: ' .. tostring(e))\n"
        "check(h == nil and why == (PLAYER or \"hideui can't work with this version of FFXI; update the addon\"),"
        "  'and new answers nil and the reason as a player can act on it: ' .. tostring(why))\n"
        "return finish()\n";

    // -- a signature that no longer matches
    {
        uint8_t* at = static_cast<uint8_t*>(g_fake->set_position) + 26;
        const uint8_t saved = *at;
        poke(at, 0x53);
        const char* g[] = {"EXPECT", "signature set_position: not found", NULL};
        lua_State* L = fresh(g);
        phase(L, kExpectFailure);
        lua.close(L);
        poke(at, saved);
        check(static_cast<uint8_t*>(g_fake->open)[0] == open_first
                && static_cast<uint8_t*>(g_fake->ui_update)[0] == update_first,
            "a missing signature installs nothing: the routines' first bytes are untouched");
        check(!engine_mapped(), "and the failed engine unmaps");
    }

    // -- a called routine whose signature no longer matches
    {
        uint8_t* at = static_cast<uint8_t*>(g_fake->calls[1]);
        const uint8_t saved = *at;
        poke(at, 0x31);
        const char* g[] = {"EXPECT", "signature passinpu_reset: not found", NULL};
        lua_State* L = fresh(g);
        phase(L, kExpectFailure);
        lua.close(L);
        poke(at, saved);
        check(static_cast<uint8_t*>(g_fake->open)[0] == open_first && !engine_mapped(),
            "a missing reply routine installs nothing either");
    }

    // -- a data site whose signature no longer matches
    {
        uint8_t* at = g_fake->link5_site + 3;
        const uint8_t saved = *at;
        poke(at, 0x90);
        const char* g[] = {"EXPECT", "signature link5_cache: not found", NULL};
        lua_State* L = fresh(g);
        phase(L, kExpectFailure);
        lua.close(L);
        poke(at, saved);
        check(static_cast<uint8_t*>(g_fake->open)[0] == open_first && !engine_mapped(),
            "a missing link5 cache site installs nothing either");
    }

    // -- the 0.4.4 sites: SetCursor, the link5 latch, and the link5 open site,
    //    whose controller global must be the registry's link5 slot
    {
        uint8_t* at = static_cast<uint8_t*>(g_fake->set_cursor) + 11;
        uint8_t saved = *at;
        poke(at, 0x90);
        const char* g[] = {"EXPECT", "signature set_cursor: not found", NULL};
        lua_State* L = fresh(g);
        phase(L, kExpectFailure);
        lua.close(L);
        poke(at, saved);
        check(static_cast<uint8_t*>(g_fake->open)[0] == open_first && !engine_mapped(),
            "a missing SetCursor installs nothing");
        at = g_fake->link5_latch_site + 6;
        saved = *at;
        poke(at, 0x90);
        const char* g2[] = {"EXPECT", "signature link5_latch: not found", NULL};
        L = fresh(g2);
        phase(L, kExpectFailure);
        lua.close(L);
        poke(at, saved);
        check(static_cast<uint8_t*>(g_fake->open)[0] == open_first && !engine_mapped(),
            "a missing link5 latch site installs nothing");
        at = g_fake->link5_open_site + 16;
        saved = *at;
        poke(at, static_cast<uint8_t>(saved + 4));
        const char* g3[] = {"EXPECT", "link5 open site: its controller global", NULL};
        L = fresh(g3);
        phase(L, kExpectFailure);
        lua.close(L);
        poke(at, saved);
        check(static_cast<uint8_t*>(g_fake->open)[0] == open_first && !engine_mapped(),
            "a link5 open site whose controller global is not the registry's link5 slot installs nothing");
    }

    // -- the text-to-glyph converter (0.4.6)
    {
        uint8_t* at = static_cast<uint8_t*>(g_fake->glyph_convert) + 5;
        const uint8_t saved = *at;
        poke(at, 0x90);
        const char* g[] = {"EXPECT", "signature glyph_convert: not found", NULL};
        lua_State* L = fresh(g);
        phase(L, kExpectFailure);
        lua.close(L);
        poke(at, saved);
        check(static_cast<uint8_t*>(g_fake->open)[0] == open_first && !engine_mapped() && *g_fake->convert_calls == 0,
            "a missing text-to-glyph converter installs nothing, and nothing calls the converter");
    }

    // -- the 0.6.0 sites: the mouse mode picker, whose imm32 must be the
    //    manager, and the row hit test, which must still test menu+0x77
    {
        uint8_t* picker = static_cast<uint8_t*>(g_fake->mouse_mode);
        const uint8_t picker_first = picker[0];
        const size_t at_bytes[2] = {20, 4};     // mode 2's immediate; the manager's low byte
        for (int i = 0; i < 2; ++i) {
            uint8_t* at = picker + at_bytes[i];
            const uint8_t saved = *at;
            poke(at, static_cast<uint8_t>(saved + 1));
            const char* g[] = {"EXPECT", "signature mouse_mode: not found", NULL};
            lua_State* L = fresh(g);
            phase(L, kExpectFailure);
            lua.close(L);
            poke(at, saved);
        }
        check(static_cast<uint8_t*>(g_fake->open)[0] == open_first && picker[0] == picker_first && !engine_mapped(),
            "a mouse mode picker that differs, or whose imm32 is not the manager, installs nothing");
        uint8_t* at = g_fake->mouse_read + 6;
        const uint8_t saved = *at;
        poke(at, 0x78);
        const char* g[] = {"EXPECT", "row_hit_test: its test of the menu's mouse byte +0x77 is not there", NULL};
        lua_State* L = fresh(g);
        phase(L, kExpectFailure);
        lua.close(L);
        poke(at, saved);
        at = static_cast<uint8_t*>(g_fake->row_hit_test) + 1;
        const uint8_t saved_hit = *at;
        poke(at, 0x90);
        const char* g2[] = {"EXPECT", "signature row_hit_test: not found", NULL};
        L = fresh(g2);
        phase(L, kExpectFailure);
        lua.close(L);
        poke(at, saved_hit);
        check(static_cast<uint8_t*>(g_fake->open)[0] == open_first && picker[0] == picker_first && !engine_mapped(),
            "a row hit test that is missing, or no longer tests menu+0x77, installs nothing");
    }

    // -- the 0.7.6 sites: the menu input sink, the routing routine, and the
    //    routing routine's call, which must reach the sink
    {
        uint8_t* sink = static_cast<uint8_t*>(g_fake->menu_input);
        uint8_t* routing = static_cast<uint8_t*>(g_fake->menu_routing);
        const uint8_t sink_first = sink[0];
        struct Poke {
            uint8_t* at;
            uint8_t value;
            const char* expect;
        };
        const Poke pokes[3] = {
            {sink + 9, 0x0f, "signature menu_input: not found"},
            {routing + 40, 0x55, "signature menu_routing: not found"},
            {routing + 0x2B, static_cast<uint8_t>(routing[0x2B] + 1),
             "menu_routing: its call at +0x2A does not reach menu_input"},
        };
        for (int i = 0; i < 3; ++i) {
            const uint8_t saved = *pokes[i].at;
            poke(pokes[i].at, pokes[i].value);
            const char* g[] = {"EXPECT", pokes[i].expect, NULL};
            lua_State* L = fresh(g);
            phase(L, kExpectFailure);
            lua.close(L);
            poke(pokes[i].at, saved);
        }
        check(static_cast<uint8_t*>(g_fake->open)[0] == open_first && sink[0] == sink_first && !engine_mapped(),
            "a sink or a routing routine that differs, or a routing routine whose call misses the sink, installs"
            " nothing");
    }

    // -- the 0.8.0 site: the compass draw entry, whose two copies of the
    //    compass global must agree
    {
        uint8_t* draw = static_cast<uint8_t*>(g_fake->compass_draw);
        const uint8_t draw_first = draw[0];
        struct Poke {
            uint8_t* at;
            uint8_t value;
            const char* expect;
        };
        const Poke pokes[3] = {
            {draw + 7, 0xc8, "signature compass_draw: not found"},
            {draw + 31, static_cast<uint8_t>(draw[31] + 1), "compass_draw: its two compass globals disagree"},
            {draw + 2, static_cast<uint8_t>(draw[2] + 1), "compass_draw: its two compass globals disagree"},
        };
        for (int i = 0; i < 3; ++i) {
            // No frame while a global's imm32 is poked: the stand-in's draw
            // would render through a pointer read from the global's next byte.
            pause_game();
            const uint8_t saved = *pokes[i].at;
            poke(pokes[i].at, pokes[i].value);
            const char* g[] = {"EXPECT", pokes[i].expect, NULL};
            lua_State* L = fresh(g);
            phase(L, kExpectFailure);
            lua.close(L);
            poke(pokes[i].at, saved);
            resume_game();
        }
        check(static_cast<uint8_t*>(g_fake->open)[0] == open_first && draw[0] == draw_first && !engine_mapped()
                && *g_fake->compass_draws > 0,
            "a compass draw entry that differs, or whose two copies of the global disagree, installs nothing; the"
            " stand-in draws it every frame all the same");
    }

    // -- the 0.9.0 sites: the macro key gate, whose imm32 at +40 must be the
    //    manager, and the macro object's constructor site, whose two copies
    //    of the global must agree
    {
        uint8_t* gate = static_cast<uint8_t*>(g_fake->macro_gate);
        uint8_t* site = g_fake->macro_ctor_site;
        const uint8_t gate_first = gate[0];
        struct Poke {
            uint8_t* at;
            uint8_t value;
            const char* expect;
        };
        const Poke pokes[4] = {
            {gate + 7, 0x90, "signature macro_gate: not found"},
            {gate + 40, static_cast<uint8_t>(gate[40] + 1), "signature macro_gate: not found"},
            {site + 6, 0x90, "signature macro_object: not found"},
            {site + 15, static_cast<uint8_t>(site[15] + 1), "macro_object: its two globals disagree"},
        };
        for (int i = 0; i < 4; ++i) {
            const uint8_t saved = *pokes[i].at;
            poke(pokes[i].at, pokes[i].value);
            const char* g[] = {"EXPECT", pokes[i].expect, NULL};
            lua_State* L = fresh(g);
            phase(L, kExpectFailure);
            lua.close(L);
            poke(pokes[i].at, saved);
        }
        check(static_cast<uint8_t*>(g_fake->open)[0] == open_first && gate[0] == gate_first && !engine_mapped()
                && *g_fake->macros_fired == 0,
            "a macro key gate that differs or whose imm32 is not the manager, and a constructor site that differs"
            " or whose two globals disagree, install nothing; no key held, no macro fired");
    }

    // -- the 0.10.0 site: the ability opener, whose imm32 at +29 must be the
    //    manager and whose key push at +43 must name the registry's ability key
    {
        uint8_t* opener = static_cast<uint8_t*>(g_fake->ability_open);
        const uint8_t opener_first = opener[0];
        struct Poke {
            uint8_t* at;
            uint8_t value;
            const char* expect;
        };
        const Poke pokes[3] = {
            {opener + 9, 0x90, "signature ability_open: not found"},
            {opener + 29, static_cast<uint8_t>(opener[29] + 1), "signature ability_open: not found"},
            {opener + 43, static_cast<uint8_t>(opener[43] + 1),
             "ability_open: its key push at +43 does not name the registry's ability key"},
        };
        for (int i = 0; i < 3; ++i) {
            const uint8_t saved = *pokes[i].at;
            poke(pokes[i].at, pokes[i].value);
            const char* g[] = {"EXPECT", pokes[i].expect, NULL};
            lua_State* L = fresh(g);
            phase(L, kExpectFailure);
            lua.close(L);
            poke(pokes[i].at, saved);
        }
        check(static_cast<uint8_t*>(g_fake->open)[0] == open_first && opener[0] == opener_first && !engine_mapped(),
            "an ability opener that differs, whose imm32 is not the manager, or whose second key push names another"
            " key, installs nothing");
    }

    // -- the 0.11.0 sites of the target arrows and the template swap: the
    //    per-menu draw, the sprite draw, the draw primitive, the quad routine
    //    and the swap, each a signature, the quad routine's call reaching the
    //    draw primitive; the target window's two arrow draws, each a
    //    signature and a call of the sprite draw; and comment's start,
    //    reached through its open hook
    {
        uint8_t* menu_draw = static_cast<uint8_t*>(g_fake->menu_draw);
        uint8_t* sprite = static_cast<uint8_t*>(g_fake->sprite_draw);
        uint8_t* primitive = static_cast<uint8_t*>(g_fake->primitive_draw);
        uint8_t* quad = static_cast<uint8_t*>(g_fake->quad_draw);
        uint8_t* swap = static_cast<uint8_t*>(g_fake->calls[10]);
        uint8_t* arrow = NULL;
        uint8_t* pick = NULL;
        for (int i = 0; i < 0x100; ++i) {
            uint8_t* at = g_fake->target_draw + i;
            if (!arrow && at[0] == 0x8b && at[1] == 0x4c && at[4] == 0x68) {
                arrow = at;
            } else if (!pick && at[0] == 0x8b && at[1] == 0x4c && at[4] == 0x52) {
                pick = at;
            }
        }
        uint8_t* line = NULL;
        for (int i = 0; i < 0x40 && !line; ++i) {
            uint8_t* at = g_fake->comment_start + i;
            line = at[0] == 0x88 && at[1] == 0x5e && at[2] == 0x14 ? at : NULL;
        }
        struct Poke {
            uint8_t* at;
            uint8_t value;
            const char* expect;
        };
        const Poke pokes[10] = {
            {menu_draw + 7, 0x90, "signature menu_draw: not found"},
            {sprite + 9, 0x90, "signature sprite_draw: not found"},
            {primitive + 8, 0x90, "signature primitive_draw: not found"},
            {quad + 8, 0x90, "signature quad_draw: not found"},
            {quad + 0x232, static_cast<uint8_t>(quad[0x232] + 1),
             "quad_draw: its call at +0x231 does not reach primitive_draw"},
            {swap + 7, 0x90, "signature template_swap: not found"},
            {arrow + 4, 0x6a, "signature target_arrow: not found"},
            {pick + 4, 0x51, "signature pick_arrow: not found"},
            {arrow + 0x11, static_cast<uint8_t>(arrow[0x11] + 1),
             "target_arrow: the target window's two arrow draws are not its calls of sprite_draw"},
            {line + 2, 0x15, "comment_input: its open hook does not start the field on a line"},
        };
        for (int i = 0; i < 10; ++i) {
            const uint8_t saved = *pokes[i].at;
            poke(pokes[i].at, pokes[i].value);
            const char* g[] = {"EXPECT", pokes[i].expect, NULL};
            lua_State* L = fresh(g);
            phase(L, kExpectFailure);
            lua.close(L);
            poke(pokes[i].at, saved);
        }
        check(arrow && pick && line && static_cast<uint8_t*>(g_fake->open)[0] == open_first && menu_draw[0] == 0x83
                && primitive[0] == 0x83 && quad[0] == 0x8b && swap[0] == 0x83 && !engine_mapped(),
            "a per-menu draw, sprite draw, draw primitive, quad routine or template swap that differs, a quad routine"
            " calling another draw, an arrow draw that differs or calls another routine, and a comment start that"
            " records no line, each install nothing");
    }

    // -- the 0.11.0 sites: select-row, and the yes/no class's constructor
    //    the class's vtable is read out of
    {
        uint8_t* select = static_cast<uint8_t*>(g_fake->calls[kFakeCalls - 1]);
        uint8_t* site = g_fake->yesno_site;
        struct Poke {
            uint8_t* at;
            uint8_t value;
            const char* expect;
        };
        const Poke pokes[2] = {
            {select + 9, 0x90, "signature select_row: not found"},
            {site + 30, 0x07, "signature yesno_class: not found"},
        };
        for (int i = 0; i < 2; ++i) {
            const uint8_t saved = *pokes[i].at;
            poke(pokes[i].at, pokes[i].value);
            const char* g[] = {"EXPECT", pokes[i].expect, NULL};
            lua_State* L = fresh(g);
            phase(L, kExpectFailure);
            lua.close(L);
            poke(pokes[i].at, saved);
        }
        check(static_cast<uint8_t*>(g_fake->open)[0] == open_first && !engine_mapped(),
            "a select-row or a yes/no class constructor that differs installs nothing");
    }

    // -- the 0.11.0 checks on the yes/no class's confirm, which must still
    //    keep its question where the replies read and write it, and on the
    //    KO window's confirm, which must ask through the registry's comyn
    //    slot with a callback marking dead's controller, and its tick, on the
    //    confirm's decider
    {
        uint8_t* confirm = g_fake->yesno_confirm;
        uint8_t* site = g_fake->dead_site;
        uint8_t* callback = g_fake->dead_callback;
        uint8_t* tick = g_fake->dead_tick;
        uint8_t* ask = static_cast<uint8_t*>(g_fake->yesno_ask_this);
        struct Poke {
            uint8_t* at;
            uint8_t value;
            const char* expect;
        };
        const Poke pokes[11] = {
            {confirm + 5, 0x44, "signature yesno_confirm: not found"},
            {confirm + 0x44, 0x3b, "yesno_confirm: its read of the chained flag +0x3A is not there"},
            {confirm + 0x58, 0x18, "yesno_confirm: its clear of the callback +0x14 is not there"},
            {confirm + 0x93, 0x3c, "yesno_confirm: its read of the chained default +0x3B is not there"},
            {site + 14, 0x75, "signature dead_confirm: not found"},
            {site + 37, static_cast<uint8_t>(site[37] + 4), "dead_confirm: its asker's global"},
            {callback + 15, 0x25, "dead_confirm: its callback"},
            {callback + 9, static_cast<uint8_t>(callback[9] + 4), "dead_confirm: its callback"},
            {tick + 10, 0x18, "signature dead_tick: not found"},
            {tick + 38, static_cast<uint8_t>(tick[38] + 1),
             "dead_tick: its decider is not the one the KO window's confirm calls"},
            {ask + 14, 0x18, "dead_confirm: the ask it calls"},
        };
        for (int i = 0; i < 11; ++i) {
            const uint8_t saved = *pokes[i].at;
            poke(pokes[i].at, pokes[i].value);
            const char* g[] = {"EXPECT", pokes[i].expect, NULL};
            lua_State* L = fresh(g);
            phase(L, kExpectFailure);
            lua.close(L);
            poke(pokes[i].at, saved);
        }
        check(static_cast<uint8_t*>(g_fake->open)[0] == open_first && !engine_mapped() && g_fake->log->ask_calls == 0,
            "a yes/no confirm that differs or keeps its question elsewhere, a KO window confirm that differs, asks"
            " through another global or another ask or names a callback that marks nothing in dead's controller, and"
            " a KO window tick that differs or calls another decider, install nothing");
    }

    // -- the 0.11.0 number boxes: the confirms, which must close through the
    //    manager; the setups, which must open their own key through the open
    //    routine after writing the question; the askers, whose globals must be
    //    the registry's and whose calls must reach the box's setup; and the
    //    yes/no class's cancel, which must test the cancellable byte
    {
        uint8_t* money_confirm = static_cast<uint8_t*>(g_fake->box_confirm[0]);
        uint8_t* money_setup = static_cast<uint8_t*>(g_fake->box_setup[0]);
        uint8_t* item_setup = static_cast<uint8_t*>(g_fake->box_setup[1]);
        uint8_t* input = g_fake->yesno_input;
        uint8_t* cancellable = NULL;
        for (int i = 0; i < 16 && !cancellable; ++i) {
            if (input[i] == 0x8a && input[i + 1] == 0x4e && input[i + 2] == 0x39) {
                cancellable = input + i + 2;
            }
        }
        struct Poke {
            uint8_t* at;
            uint8_t value;
            const char* expect;
        };
        const Poke pokes[10] = {
            {money_confirm + 5, 0x2d, "signature moneyctr_confirm: not found"},
            {money_confirm + 54, static_cast<uint8_t>(money_confirm[54] + 4),
             "moneyctr_confirm: its close does not go through the menu manager"},
            {item_setup + 51, static_cast<uint8_t>(item_setup[51] + 1),
             "itemctrl_setup: its open does not name the registry's itemctrl key"},
            {money_setup + 64, 0x88, "moneyctr_setup: its writes of the max, the context and the extra callback"},
            {money_setup + 78, static_cast<uint8_t>(money_setup[78] + 1),
             "moneyctr_setup: it does not open through the menu manager's open routine"},
            {g_fake->askers[8] + 24, static_cast<uint8_t>(g_fake->askers[8][24] + 4),
             "ask_trade_item: its global is not the registry's trade slot"},
            {g_fake->askers[1] + 2, static_cast<uint8_t>(g_fake->askers[1][2] + 4),
             "ask_handover_gil: its box global is not the registry's moneyctr slot"},
            {g_fake->askers[15] + 31, static_cast<uint8_t>(g_fake->askers[15][31] + 1),
             "ask_shop_sell: its call does not reach itemctrl's setup"},
            {g_fake->askers[0] + 6, 0x7f, "signature ask_delivery_gil: not found"},
            {cancellable, 0x38, "yes/no class: its cancel's test of the cancellable byte +0x39 is not there"},
        };
        for (int i = 0; i < 10; ++i) {
            if (!pokes[i].at) {
                check(false, "the yes/no class's input routine carries its cancel's test");
                continue;
            }
            const uint8_t saved = *pokes[i].at;
            poke(pokes[i].at, pokes[i].value);
            const char* g[] = {"EXPECT", pokes[i].expect, NULL};
            lua_State* L = fresh(g);
            phase(L, kExpectFailure);
            lua.close(L);
            poke(pokes[i].at, saved);
        }
        check(static_cast<uint8_t*>(g_fake->open)[0] == open_first && !engine_mapped(),
            "a box confirm or setup that differs or names another manager, key or open routine, an asker whose global"
            " is not the registry's or whose call reaches no setup, and a yes/no class whose cancel does not test its"
            " cancellable byte, install nothing");
    }

    // -- the 0.11.0 Asked windows: each signature, and what the engine
    //    checks it holds: the keys, globals and routines it names, and the
    //    routines it leads to
    {
        uint8_t* const* a = g_fake->asked_sites;
        uint8_t* evitem_setup = call_target(a[2] + 63);
        uint8_t* evitem_cb = NULL;
        memcpy(&evitem_cb, evitem_setup + 55, 4);
        uint8_t* bank_clear = call_target(a[5] + 48);
        uint8_t* map_rebuild = call_target(a[7] + 48 + 83 + 14);
        uint8_t* map_cursor = call_target(a[7] + 48 + 147 + 28);
        uint8_t* map0 = g_fake->map0_input;
        uint8_t* map_close = g_fake->map_close;
        const char kMap0Cancel[] = "map0_input: its cancel does not close map0 by instance, then call a routine on the"
            " controller in mapframe's registry slot";
        const char kMapClose[] = "map0_input: the routine its cancel calls is not the map's close";
        struct Poke {
            uint8_t* at;
            uint8_t value;
            const char* expect;
        };
        const Poke pokes[33] = {
            {a[0] + 5, 0x86, "signature sortyn_setup: not found"},
            {a[0] + 47, static_cast<uint8_t>(a[0][47] + 1), "sortyn_setup: it does not open sortyn's key"},
            {a[1] + 62, static_cast<uint8_t>(a[1][62] + 1), "sortyn_confirm: it does not close sortyn's key"},
            {a[2] + 58, static_cast<uint8_t>(a[2][58] + 4), "evitem_ask: it does not open evitem01's key, then ask"
             " through evitem01's registry slot"},
            {evitem_setup + 11, 0x75, "evitem_ask: the setup it calls does not write the question"},
            {evitem_cb + 3, static_cast<uint8_t>(evitem_cb[3] + 4), "evitem_ask: the callback its setup sets does not"
             " read the registry's evitem slot"},
            {a[3] + 61, 0x8b, "evitem_input: it does not close its window by instance after the callback"},
            {a[4] + 56, static_cast<uint8_t>(a[4][56] + 1), "confyn_input: it does not restore through a routine of"
             " the game's and close by instance"},
            {a[5] + 1, static_cast<uint8_t>(a[5][1] + 4), "bank_open: its test of Mog Safe 2 is not a read of a flag"},
            {bank_clear + 3, 0x01, "bank_open: bankmenu's begin-close does not clear its callback"},
            {a[6] + 52, static_cast<uint8_t>(a[6][52] + 1), "bank_input: it does not close the window through its"
             " class's close"},
            {a[6] + 5, 0x57, "signature bank_input: not found"},
            {a[7] + 49, static_cast<uint8_t>(a[7][49] + 4), "maplist_close: the key handler after it does not read"
             " the registry's maplist slot"},
            {a[7] + 48 + 27, 0x68, "maplist_close: its key handler's confirm of a leaf row is not there"},
            {a[7] + 2, 0xf0, "signature maplist_close: not found"},
            {a[7] + 65, static_cast<uint8_t>(a[7][65] + 4), "maplist_close: its key handler does not need the player"
             " data's global first"},
            {a[7] + 48 + 147 + 12, 0xd8, "maplist_close: its key handler's confirm of a row with a list of its own is"
             " not there"},
            {a[7] + 48 + 102 + 2, 0x46, "maplist_close: its key handler's steps between levels do not rebuild the list"
             " through one routine and select a row through another"},
            {map_rebuild + 4, 0x02, "maplist_close: the rebuild its steps between levels call does not empty and free"
             " the rows as close+reset does"},
            {map_cursor + 10, 0x4e, "maplist_close: the routine its step down keeps the row under the cursor with is"
             " not the list's cursor read"},
            {a[8] + 7, static_cast<uint8_t>(a[8][7] + 1), "gmtell_poll: it does not read the question's result word"},
            {a[8] + 13, 0x85, "signature gmtell_poll: not found"},
            {a[9] + 85 + 2, 0x18, "mp_dead_input: its tick's countdown and its choice at the deadline are not there"},
            {a[9] + 3, 0x75, "signature mp_dead_input: not found"},
            {a[10] + 62, static_cast<uint8_t>(a[10][62] + 4), "ok_input: it does not close its window by instance"},
            {a[10] + 76, 0x59, "ok_input: it does not close its window by instance after the callback, and again"},
            {map0 + 9, 0x26, "signature map0_input: not found"},
            {map0 + 24, static_cast<uint8_t>(map0[24] + 4), kMap0Cancel},
            {map0 + 29, static_cast<uint8_t>(map0[29] + 1), kMap0Cancel},
            {map0 + 35, static_cast<uint8_t>(map0[35] + 4), kMap0Cancel},
            {map_close + 3, 0x08, kMapClose},
            {map_close + 0xAE + 15 + 1, static_cast<uint8_t>(map_close[0xAE + 15 + 1] + 0x2c), kMapClose},
            {map_close + 0xAE + 75 + 6, 0x54, kMapClose},
        };
        for (int i = 0; i < 33; ++i) {
            const uint8_t saved = *pokes[i].at;
            poke(pokes[i].at, pokes[i].value);
            const char* g[] = {"EXPECT", pokes[i].expect, NULL};
            lua_State* L = fresh(g);
            phase(L, kExpectFailure);
            lua.close(L);
            poke(pokes[i].at, saved);
        }
        check(static_cast<uint8_t*>(g_fake->open)[0] == open_first && !engine_mapped(),
            "an Asked window's signature that differs, or holds another key, global, manager or routine than the"
            " registry's and the game's, or leads to a setup, callback, clear, close or tick that differs, installs"
            " nothing; so does a map0 cancel that does not close map0 by instance on the manager and call a routine"
            " on mapframe's global, or calls one that is not the map's close of its windows by name and fade-out");
    }

    // -- mogdoor, the dialogue boxes and the key test: each signature, and
    //    what the engine checks in it and in what it leads to. The game
    //    thread is paused: the UI update calls the key test every frame.
    {
        uint8_t* door = g_fake->door_input;
        uint8_t* handler = g_fake->door_handler;
        uint8_t* input = g_fake->dialogue_input;
        uint8_t* talk = call_target(input + 15);
        uint8_t* dispatch = call_target(talk + 0x44);
        uint8_t* gate = call_target(dispatch + 9);
        uint8_t* sound = call_target(gate + 19);
        uint8_t* update = static_cast<uint8_t*>(g_fake->ui_update);
        struct Poke {
            uint8_t* at;
            uint8_t value;
            const char* expect;
        };
        // The dispatch past its gate's call: the global its first compare
        // names (+2), the other box's (+12, again at +29), the gate's call
        // (+17, its displacement at +18).
        uint8_t* twin = dispatch + 14;
        const char kTwin[] = "dialogue_input: the text dispatch does not hand its code on to the other dialogue box's"
            " text object";
        const char kGate[] = "dialogue_input: the text dispatch, its gate on a page waiting for a key";
        const Poke pokes[22] = {
            {door + 9, 0x90, "signature door_input: not found"},
            {door + 63, 0x90, "door_input: its rows of the list of areas or of the top are not the ones the engine"
             " answers"},
            {door + 0x8c + 1, 0x4e, "door_input: its top's rows do not stay, go back, change floors and open the list"
             " of areas"},
            {door + 0xe6 + 16, 0x90, "door_input: the exit after its rows is not the one the engine makes"},
            {door + 0xe6 + 50, 0x05, "door_input: the exit does not set the three bytes the engine sets"},
            {g_fake->door_opened + 24, 0x90, "door_input: the routine its exit gives the door does not send"
             " mogdoor's exit"},
            {g_fake->door_open_hook + 5, 0x90, "door_open: mogdoor's open hook does not take a nation's list of"
             " areas from a table and show the top"},
            {g_fake->door_tick + 12, 0x18, "door_tick: mogdoor's tick does not enable its Mog Garden and floor rows"},
            {static_cast<uint8_t*>(const_cast<void*>(g_fake->door_vtable[4])) + 5, 0x90,
             "door_draw: mogdoor's draw does not show the texts the engine reads"},
            {handler + 20, 0x89, "signature door_handler: not found"},
            {handler + 7, static_cast<uint8_t>(handler[7] + 4), "door_handler: it does not write the nation on"
             " mogdoor's controller"},
            {input + 1, 0x89, "signature dialogue_input: not found"},
            {input + 22, static_cast<uint8_t>(input[22] + 4), "dialogue_input: it does not hand its codes to one"
             " handler, again for the controllers in the registry's rem4line and rem4li2 slots"},
            {talk + 0x5d + 2, 0x60, "dialogue_input: its handler does not hand the code to the text dispatch, then"
             " cancel as the box's own cancel does"},
            {gate + 3, 0x02, kGate},
            {gate + 19, 0x90, kGate},
            {sound + 1, 0x31, kGate},
            {twin + 2, static_cast<uint8_t>(twin[2] + 4), kTwin},
            {twin + 12, static_cast<uint8_t>(twin[12] + 4), kTwin},
            {twin + 18, static_cast<uint8_t>(twin[18] + 1), kTwin},
            {g_fake->dialogue_tick + 8, 0x04, "dialogue_tick: its test of the left button through the key test"},
            {g_fake->key_test + 6, 0x90, "key_test: the routine the UI update calls is not the key test"},
        };
        for (int i = 0; i < 22; ++i) {
            pause_game();
            const uint8_t saved = *pokes[i].at;
            poke(pokes[i].at, pokes[i].value);
            const char* g[] = {"EXPECT", pokes[i].expect, NULL};
            lua_State* L = fresh(g);
            phase(L, kExpectFailure);
            lua.close(L);
            poke(pokes[i].at, saved);
            resume_game();
        }
        // The UI update's call reaching past the key test's first byte.
        pause_game();
        int32_t rel;
        memcpy(&rel, update + 21, 4);
        const int32_t moved = rel + 1;
        for (int k = 0; k < 4; ++k) {
            poke(update + 21 + k, static_cast<uint8_t>(moved >> (8 * k)));
        }
        const char* g[] = {"EXPECT", "key_test: the routine the UI update calls is not the key test", NULL};
        lua_State* L = fresh(g);
        phase(L, kExpectFailure);
        lua.close(L);
        for (int k = 0; k < 4; ++k) {
            poke(update + 21 + k, static_cast<uint8_t>(rel >> (8 * k)));
        }
        resume_game();
        check(static_cast<uint8_t*>(g_fake->open)[0] == open_first && !engine_mapped(),
            "mogdoor's input routine, rows, exit, the routine it gives the door, open hook, tick and draw, the door's"
            " handler, the dialogue boxes' input routine, handler, dispatch gate, its call of the page-turn sound,"
            " the sound's name and tick, a dispatch whose two globals are one, that loads one global and compares"
            " with another, or that calls another gate for the other box, the key test, and a UI update whose call"
            " reaches another routine, each differing, install nothing");
    }

    // -- the text-entry windows: each input routine's signature, and a
    //    shape in what the engine reads out of each; passinpu's start, its
    //    open hook's call at +0x22, without the manager's unbind
    {
        uint8_t* const* in = g_fake->text_inputs;
        const void* const* pass_vtable;
        memcpy(&pass_vtable, controller_named("passinpu"), 4);
        uint8_t* pass_start = call_target(static_cast<uint8_t*>(const_cast<void*>(pass_vtable[1])) + 0x22);
        struct Poke {
            uint8_t* at;
            uint8_t value;
            const char* expect;
        };
        const Poke pokes[13] = {
            {in[0] + 3, 0x90, "signature stringdl_input: not found"},
            {in[0] + 0x3C + 7, 0x90, "stringdl_input: with a reply written, it does not raise the outgoing box"
             " through the manager"},
            {in[1] + 1, 0x90, "signature comment_input: not found"},
            {in[1] + 0x73 + 10, 0x90, "comment_input: its Escape does not end the field, then close the window by"
             " instance"},
            {in[2] + 1, 0x90, "signature msgline_input: not found"},
            {in[2] + 0x34 + 3, 0x90, "msgline_input: its commit does not test the player's data, then take the"
             " mode's case"},
            {in[3] + 5, 0x90, "signature scname_input: not found"},
            {in[3] + 0x6E + 11, 0x90, "scname_input: its row 2 does not name the player search and start it"},
            {in[4] + 1, 0x90, "signature link11_input: not found"},
            {in[4] + 0x11B + 50, 0x90, "link11_input: its row 5 does not send the new linkshell's request"},
            {in[5] + 1, 0x90, "signature passinpu_input: not found"},
            {in[5] + 0x23, 0x90, "signature passinpu_input: not found"},
            {pass_start + 0x83, 0x90, "passinpu_input: its field's start does not end in the unbind and bind of the"
             " text entry's manager stringdl's end calls"},
        };
        for (int i = 0; i < 13; ++i) {
            pause_game();
            const uint8_t saved = *pokes[i].at;
            poke(pokes[i].at, pokes[i].value);
            const char* g[] = {"EXPECT", pokes[i].expect, NULL};
            lua_State* L = fresh(g);
            phase(L, kExpectFailure);
            lua.close(L);
            poke(pokes[i].at, saved);
            resume_game();
        }
        check(!engine_mapped(), "each text window's input routine differing, or a shape in it, installs nothing");
    }

    // -- the trade windows: each signature, and a shape in what the engine
    //    reads out of each
    {
        uint8_t* ti = g_fake->trade_input;
        uint8_t* to = g_fake->trade_open;
        uint8_t* hi = g_fake->handover_input;
        uint8_t* ir = g_fake->inventor_rows;
        struct Poke {
            uint8_t* at;
            uint8_t value;
            const char* expect;
        };
        const Poke pokes[10] = {
            {ti + 1, 0x90, "signature trade_input: not found"},
            {ti + 0x3F + 12, 0x90, "trade_input: its rows of the offer do not take an item back out through the stage"
             " its item count's asker calls"},
            {g_fake->trade_stage + 9, 0x90, "trade_stage: it does not stage on its cursor's row into the offer's slots,"
             " then write both states"},
            {to + 1, 0x90, "signature trade_open: not found"},
            {to + 0x5B + 6, 0x90, "trade_open: it does not hand the inventory list the trade's mode"},
            {hi + 1, 0x90, "signature handover_input: not found"},
            {call_target(hi + 0x67 + 2) + 1, 0x90, "handover_input: its OK, its close or its stage is not the one the"
             " engine calls"},
            {ir + 6, 0x90, "signature inventor_rows: not found"},
            {ir + 0x46 + 52, 0x90, "inventor_rows: it does not lock an item's data around its mode's test of the row"},
            {ir + 0xA6 + 1, 0x03, "inventor_rows: its status rule names a case it does not have"},
        };
        for (int i = 0; i < 10; ++i) {
            pause_game();
            const uint8_t saved = *pokes[i].at;
            poke(pokes[i].at, pokes[i].value);
            const char* g[] = {"EXPECT", pokes[i].expect, NULL};
            lua_State* L = fresh(g);
            phase(L, kExpectFailure);
            lua.close(L);
            poke(pokes[i].at, saved);
            resume_game();
        }
        check(!engine_mapped(), "each trade window's signature differing, or a shape in what it leads to, installs"
            " nothing");
    }

    // -- a signature that matches twice
    {
        poke(g_fake->spare_set_position, 0x83);
        const char* g[] = {"EXPECT", "signature set_position: matched 2 times", NULL};
        lua_State* L = fresh(g);
        phase(L, kExpectFailure);
        lua.close(L);
        poke(g_fake->spare_set_position, 0xCC);
        check(static_cast<uint8_t*>(g_fake->open)[0] == open_first, "a duplicated signature installs nothing");
    }

    // -- another engine image holds the hooks
    {
        char name[64];
        snprintf(name, sizeof(name), "Local\\hideui_engine_v1_%08X", static_cast<unsigned>(GetCurrentProcessId()));
        HANDLE claim = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, 4096, name);
        uint8_t* view = static_cast<uint8_t*>(MapViewOfFile(claim, FILE_MAP_ALL_ACCESS, 0, 0, 4096));
        const uint32_t magic = 0x48554531u;
        strcpy(reinterpret_cast<char*>(view + 4), "C:\\elsewhere\\libs\\_HideUI.dll");
        memcpy(view, &magic, 4);
        const char* g[] = {"EXPECT", "another hideui engine holds the hooks in this client: C:\\elsewhere", "PLAYER",
                           "hideui can't work while an addon with hideui 0.1.0 is loaded; unload it", NULL};
        lua_State* L = fresh(g);
        phase(L, kExpectFailure);
        lua.close(L);
        UnmapViewOfFile(view);
        CloseHandle(claim);
        check(static_cast<uint8_t*>(g_fake->open)[0] == open_first && !engine_mapped(),
            "a refused claim installs nothing and leaves no pin");
    }

    // -- the daemon cannot patch just now: a thread sits inside the open
    //    routine's prologue, so the jump is never written (HU_E_THREADS).
    //    Nothing latches: the engine stays idle and the next new() installs.
    {
        HANDLE parked = CreateThread(NULL, 0, &never_runs, NULL, CREATE_SUSPENDED, NULL);
        CONTEXT context;
        memset(&context, 0, sizeof(context));
        context.ContextFlags = CONTEXT_CONTROL;
        bool inside = parked && GetThreadContext(parked, &context);
        context.Eip = static_cast<DWORD>(reinterpret_cast<uintptr_t>(g_fake->open)) + 2;
        inside = inside && SetThreadContext(parked, &context);
        check(inside, "a suspended thread parked two bytes into the open routine's prologue");
        lua_State* T = fresh(NULL);
        phase(T,
            "hu = load_engine()\n"
            "local h, why = hu.new('x')\n"
            "local s = hu.status()\n"
            "local e = s.detail and s.detail.error\n"
            "check(h == nil and why == \"hideui could not hook the game just now; try again\","
            "  'a jump the daemon could not write just now: new answers nil and to try again: ' .. tostring(why))\n"
            "check(not s.ok and s.state == 'idle'"
            "  and tostring(e):find('hook open_by_name: the daemon could not patch it just now (error -7)', 1, true),"
            "  'and the engine stays idle, not failed; status().detail.error says why: ' .. tostring(e))\n"
            "return finish()\n");
        check(static_cast<uint8_t*>(g_fake->open)[0] == open_first, "the open routine's first byte is untouched");
        if (parked) {
            TerminateThread(parked, 0);
            WaitForSingleObject(parked, INFINITE);
            CloseHandle(parked);
        }
        phase(T,
            "local h, why = hu.new('x')\n"
            "local s = hu.status()\n"
            "check(h ~= nil and s.ok and s.state == 'installed' and s.detail.error == nil,"
            "  'with the thread gone, the next new() installs: ' .. tostring(why))\n"
            "h:release()\n"
            "local ok, err = hu.shutdown()\n"
            "check(ok == true, 'and shuts down: ' .. tostring(err))\n"
            "return finish()\n");
        lua.close(T);
        check(engine_unmaps() && static_cast<uint8_t*>(g_fake->open)[0] == 0xE9,
            "the retried engine unmaps; the daemon's jump stays");
        // Its drain built a glyph table; the next image counts its own.
        InterlockedExchange(g_fake->convert_calls, 0);
        InterlockedExchange(g_fake->convert_elsewhere, 0);
    }

    // -- installed: hides, blocks, moves, events, two handles, shutdown
    open_on_game_thread("menu    logwindo");
    lua_State* L = fresh(globals);
    phase(L,
        "hu = load_engine()\n"
        "h = hu.new('alpha')\n"
        "local s = hu.status()\n"
        "local d = s.detail or {}\n"
        "check(s.ok and s.state == 'installed' and s.role == 'resident' and h ~= nil,"
        "  'installs against the stand-in FFXiMain: ' .. tostring(d.error))\n"
        "check(s.engine == '0.11.0' and s.handles == 1 and s.dropped == 0 and hu.version() == 'hideui 0.11.0'"
        "  and s.registry == nil and s.functions == nil,"
        "  'status: engine, handles and dropped at the top, the internals under detail; version is hideui 0.7.0')\n"
        "check(s.ui and s.ui.w == 1920 and s.ui.h == 1080 and d.ui and d.ui.w == 1920,"
        "  'status().ui: the game\\'s UI size at the top, and under detail as before')\n"
        "local f = d.functions or {}\n"
        "check(f.open_by_name == OPEN and f.ui_update == UPDATE and f.staged_close == STAGED"
        "  and f.show_path == SHOW and f.set_position == SETPOS and f.close_by_name == CLOSE,"
        "  'every hooked and placing routine resolved to its stand-in')\n"
        "local calls, all = 0, true\n"
        "for k, v in pairs(_G) do\n"
        "  if type(k) == 'string' and k:sub(1, 2) == 'F_' then\n"
        "    calls = calls + 1\n"
        "    all = all and f[k:sub(3)] == v\n"
        "  end\n"
        "end\n"
        "check(f.mouse_mode == MOUSE and f.row_hit_test == HITTEST and f.mouse_read == MOUSEREAD,"
        "  'the mouse mode picker, the row hit test and its menu+0x77 test resolved to the stand-in: '"
        "  .. tostring(f.mouse_mode) .. ' ' .. tostring(f.row_hit_test) .. ' ' .. tostring(f.mouse_read))\n"
        "check(f.menu_input == SINK and f.menu_routing == ROUTING,"
        "  'the menu input sink and the routing routine resolved to the stand-in: '"
        "  .. tostring(f.menu_input) .. ' ' .. tostring(f.menu_routing))\n"
        "check(f.compass_draw == COMPASS and f.compass_global == COMPASSGLOBAL,"
        "  'the compass draw entry and the compass global resolved to the stand-in: '"
        "  .. tostring(f.compass_draw) .. ' ' .. tostring(f.compass_global))\n"
        "check(f.macro_gate == MACROGATE and f.macro_object == MACROOBJECT,"
        "  'the macro key gate and the macro object global resolved to the stand-in: '"
        "  .. tostring(f.macro_gate) .. ' ' .. tostring(f.macro_object))\n"
        "check(f.ability_open == ABILITYOPEN, 'the ability opener resolved to the stand-in: '"
        "  .. tostring(f.ability_open))\n"
        "check(f.template_swap == F_template_swap and f.menu_draw == MENUDRAW and f.sprite_draw == SPRITEDRAW"
        "  and f.primitive_draw == PRIMITIVEDRAW and f.quad_draw == QUADDRAW"
        "  and f.target_arrow_return == TARGETRETURN and f.pick_arrow_return == PICKRETURN,"
        "  'the template swap and the three draws hooked at the stand-in\\'s, the quad routine whose call names the'"
        "  .. ' draw primitive found, and the two arrow draws\\' returns: '"
        "  .. tostring(f.target_arrow_return) .. ' ' .. tostring(f.pick_arrow_return))\n"
        "check(calls == 12 and all and f.prtyjoin_pending == PENDING and f.query_confirm == nil"
        "  and f.list_select == nil,"
        "  'the twelve reply, re-dock and resize routines, arealist\\'s close+reset and latch clear and select-row'"
        "  .. ' among them, and the invite flag resolved to the stand-in; query\\'s confirm and the list'"
        "  .. ' select-index are not resolved')\n"
        "check(f.yesno_confirm == YESNOCONFIRM and f.yesno_ask == YESNOASK and f.dead_tick == DEADTICK"
        "  and f.dead_decider == DEADDECIDER and f.message == MESSAGE and f.dead_callback == DEADCALLBACK,"
        "  'the yes/no class\\'s confirm and ask, and the KO window\\'s tick, decider, message getter and callback'"
        "  .. ' resolved to the stand-in\\'s')\n"
        "check(f.moneyctr_confirm == MONEYCONFIRM and f.itemctrl_confirm == ITEMCONFIRM"
        "  and f.moneyctr_setup == MONEYSETUP and f.itemctrl_setup == ITEMSETUP and f.yesno_cancel == YESNOCANCEL"
        "  and type(f.askers) == 'table' and f.askers.ask_trade_item == ASKTRADEITEM"
        "  and f.askers.ask_handover_gil == ASKHANDOVERGIL,"
        "  'the number boxes\\' confirms and setups, the yes/no class\\'s cancel test and the askers\\' callbacks,"
        " a site\\'s pushed one among them, resolved to the stand-in\\'s')\n"
        "local asked = 0\n"
        "for _ in pairs(f.askers or {}) do asked = asked + 1 end\n"
        "check(asked == 16, 'sixteen askers under functions.askers: ' .. asked)\n"
        "local a = f.asked or {}\n"
        "check(a.sortyn_setup == SORTYNSETUP and a.bank_open == BANKOPEN and a.maplist_close == MAPLISTCLOSE"
        "  and a.ok_input == OKINPUT and a.bank_safe2 == 0x21 and f.close_instance == CLOSEINSTANCE"
        "  and type(a.evitem_callback) == 'string' and type(a.confyn_restore) == 'string'"
        "  and type(a.gmtell_queue) == 'string' and type(a.maplist_rebuild) == 'string'"
        "  and type(a.maplist_select) == 'string' and type(a.maplist_cursor) == 'string'"
        "  and a.map0_input == MAP0INPUT and a.map_close == MAPCLOSE,"
        "  'the Asked windows\\' sites, the routines they lead to, map0\\'s input routine and the map\\'s close and"
        " close by instance resolved to the stand-in\\'s')\n"
        "local dl = f.dialogue or {}\n"
        "check(a.door_input == DOORINPUT and a.door_handler == DOORHANDLER and a.door_vtable == DOORVTABLE"
        "  and a.door_opened == DOOROPENED and a.door_open_hook == DOOROPENHOOK and a.door_tick == DOORTICK"
        "  and a.door_nations == 10 and a.door_garden == 280 and a.door_floor_word == 0x30"
        "  and dl.input == DIALOGUEINPUT and dl.tick == DIALOGUETICK and dl.click_return == CLICKRETURN"
        "  and f.key_test == KEYTEST,"
        "  'mogdoor\\'s routines, vtable and data, the dialogue boxes\\' input routine and tick, the key test and the"
        " tick\\'s call of it resolved to the stand-in\\'s')\n"
        "check(f.yesno_vtable == YESNOVTABLE, 'the yes/no class\\'s vtable resolved to the stand-in\\'s: '"
        "  .. tostring(f.yesno_vtable))\n"
        "check(f.link5_cache == LINK5CACHE and f.query_cancel_allowed == CANCELBYTE,"
        "  'the link5 concierge cache and the query cancel-allowed byte resolved to the stand-in: '"
        "  .. tostring(f.link5_cache) .. ' ' .. tostring(f.query_cancel_allowed))\n"
        "check(f.set_cursor == SETCURSOR and f.link5_latch == LINK5LATCH and f.link5_callback == LINK5CB,"
        "  'SetCursor, the link5 pending latch and the event callback the link5 open site names resolved to the'"
        "  .. ' stand-in: ' .. tostring(f.set_cursor) .. ' ' .. tostring(f.link5_latch) .. ' '"
        "  .. tostring(f.link5_callback))\n"
        "check(d.registry == REGISTRY and d.manager == MCB and d.rows == 370 and d.names == 368"
        "  and d.ui and d.ui.w == 1920 and d.ui.h == 1080, 'registry, manager and UI size resolved')\n"
        "check(d.pinned and d.daemon and d.daemon.abi == 1, 'pinned, daemon abi ' .. tostring(d.daemon and d.daemon.abi))\n"
        "check(h:hide('logwindo') == true and h:block('menuwind') == true and h:move('equip', 300, 200) == true,"
        "  'hide, block and move queue')\n"
        "local r, why = h:block('query')\n"
        "check(r == nil and why == \"query cannot be blocked or closed; answer it with answer('query', value) or"
        " cancel('query')\", 'block(query) is refused: ' .. tostring(why))\n"
        "local prompts, queued = {}, {}\n"
        "for _, w in ipairs({'query', 'passinpu', 'prtyjoin', 'delivery', 'post1', 'post2', 'link5', 'arealist'}) do\n"
        "  local ok2, why2 = h:close(w)\n"
        "  if ok2 == nil and why2 == w .. \" is a prompt the game waits on; use cancel('\" .. w .. \"')\" then"
        "    prompts[#prompts + 1] = w elseif ok2 == true then queued[#queued + 1] = w end\n"
        "end\n"
        "check(table.concat(prompts, ' ') == 'query' and #queued == 7, 'close() of query, which is never closed, is'"
        "  .. ' refused at once with \"use cancel(name)\"; of the other seven prompt windows it queues, for the game'"
        "  .. ' thread to refuse while they ask: ' .. table.concat(prompts, ' ') .. ' / ' .. table.concat(queued, ' '))\n"
        "check(h:hide('query') == true and h:unhide('query') == true, 'hide(query) is allowed')\n"
        "r, why = h:open('menuwind')\n"
        "check(r == nil and why:find('blocked', 1, true), 'open of a blocked name is refused')\n"
        "local refused = {}\n"
        "for _, w in ipairs({'query', 'passinpu', 'prtyjoin', 'delivery', 'post1', 'post2', 'link5', 'arealist'}) do\n"
        "  local ok2, why2 = h:open(w)\n"
        "  if ok2 == nil and why2:find('opens only for the game', 1, true) then refused[#refused + 1] = w end\n"
        "end\n"
        "check(#refused == 8, 'open() is refused for the eight windows the game opens for an event: '"
        "  .. table.concat(refused, ' '))\n"
        "r, why = h:hide('nosuch')\n"
        "check(r == nil and why:find('no such window', 1, true), 'an unknown name is refused')\n"
        "local groups = {}\n"
        "for _, call in ipairs({{'hide', 'chat_log'}, {'block', 'Party_List'}, {'reset', 'target_window'},"
        "    {'open', 'chat_log'}, {'info', 'party_list'}}) do\n"
        "  local ok2, why2 = h[call[1]](h, call[2])\n"
        "  if ok2 == nil and why2:find('is a group name', 1, true) then groups[#groups + 1] = call[1] end\n"
        "end\n"
        "local ok3, why3 = h:move('chat_log', 1, 2)\n"
        "check(#groups == 5 and ok3 == nil and why3:find('group name', 1, true),"
        "  'group names are refused by hide, block, reset, open, info and move: ' .. table.concat(groups, ' '))\n"
        "r, why = h:move('equip', 1.5, 2)\n"
        "check(r == nil and why:find('usage', 1, true), 'a fractional coordinate is refused')\n"
        "local misuse = {}\n"
        "for _, call in ipairs({{h.hide, 42}, {h.move, 'equip', 'x', 2}, {h.move, 'equip', 1},"
        "    {h.resize, 'partywin', 'ptw3'}, {h.resize, 'equip', 3, 'x'}, {h.info}, {h.options, false},"
        "    {h.answer, 'query', 'yes'}, {h.answer, 'link5'}, {h.answer, 'passinpu', 3},"
        "    {h.answer, 'prtyjoin', 1}, {h.move_group, 'chat_log', nil, 2}, {h.answer, 'query', 1, 'x'},"
        "    {h.answer, 'query', 1, 1.5}, {h.cancel, 'query', {}}, {h.cancel, 'query', -1}}) do\n"
        "  local ok2, err = pcall(call[1], h, unpack(call, 2, 4))\n"
        "  if not ok2 then misuse[#misuse + 1] = tostring(err) end\n"
        "end\n"
        "check(#misuse == 16 and misuse[1]:find('string expected', 1, true)"
        "  and misuse[14]:find('id is the id options(name) gave', 1, true)"
        "  and misuse[16]:find('id is the id options(name) gave', 1, true)"
        "  and misuse[8]:find(\"answer('query', value) takes\", 1, true)"
        "  and misuse[11]:find('true to accept', 1, true),"
        "  'misuse raises: a wrong or missing argument, and an answer of the wrong type for its window: '"
        "  .. tostring(misuse[8]))\n"
        "r, why = h:answer('passinpu', 'abc')\n"
        "check(r == nil and why == 'no text entry is pending', 'an answer with nothing to answer refuses: ' .. tostring(why))\n"
        "r, why = h:answer('query', 2)\n"
        "check(r == nil and why == 'query is not open', 'answer(query) while query is closed is refused')\n"
        "r, why = h:options('query')\n"
        "check(r == nil and why == 'query is not open', 'options(query) while query is closed is refused: ' .. tostring(why))\n"
        "r, why = h:options('equip')\n"
        "check(r == nil and why:find('has no options', 1, true), 'options of a window that is no prompt is refused')\n"
        "r, why = h:answer('equip', 1)\n"
        "check(r == nil and why:find('takes no answer', 1, true), 'answer to a window that is no prompt is refused')\n"
        "r, why = h:cancel('equip')\n"
        "check(r == nil and why:find('nothing to cancel', 1, true), 'cancel of a window that is no prompt is refused')\n"
        "check(next(h:pending()) == nil, 'pending(): empty with no invite and no post-box session')\n"
        "check(h:move_group('party_list', 1, 1) == true, 'move_group with its anchor closed is accepted: it waits for the'"
        "  .. ' anchor to open')\n"
        "local rects, li = h:rects(), h:info('logwindo')\n"
        "local n_rects = 0\n"
        "for _ in pairs(rects) do n_rects = n_rects + 1 end\n"
        "check(n_rects == #h:opened() and rects.logwindo and rects.logwindo.x == li.rect.x and rects.logwindo.y == li.rect.y"
        "  and rects.logwindo.w == li.rect.w and rects.logwindo.h == li.rect.h and rects.logwindo.right == nil"
        "  and rects.equip == nil, 'rects(): {x, y, w, h} of every open window, keyed by name')\n"
        "local r2, why2 = h:reset_group('logwindo')\n"
        "check(r2 == nil and why2:find('no such group', 1, true), 'reset_group takes group names only')\n"
        "local ok4, err4 = pcall(h.reset, h, 'equip', 'everything')\n"
        "check(not ok4 and tostring(err4):find(\"aspect is 'position' or 'size'\", 1, true),"
        "  'reset(name, aspect) with an aspect neither position nor size raises: ' .. tostring(err4))\n"
        "r, why = h:move_group('logwindo', 1, 1)\n"
        "check(r == nil and why:find('no such group', 1, true), 'move_group takes group names only')\n"
        "return finish()\n");
    wait_frames(3);
    phase(L,
        "local refused = {}\n"
        "for _, e in ipairs((h:poll())) do\n"
        "  if e.event == 'error' and e.verb == 'close'\n"
        "      and e.reason == e.name .. \" is a prompt the game waits on; use cancel('\" .. e.name .. \"')\" then\n"
        "    refused[#refused + 1] = e.name\n"
        "  end\n"
        "end\n"
        "check(table.concat(refused, ' ') == 'delivery post1 post2', 'the game thread refused the closes of the'"
        "  .. ' prompts asking now, the delivery and post boxes, which always do; passinpu, prtyjoin, link5 and'"
        "  .. ' arealist asked nothing: ' .. table.concat(refused, ' '))\n"
        "return finish()\n");

    wait_frames(3);
    uint8_t* log = live_menu(kRowLog);
    check(hidden_in_place(kRowLog, log),
        "the drain, on the game thread, hid logwindo: the walk's call of its draw runs and paints nothing, its"
        " registry row and live menu at its own layer");
    check(*g_fake->convert_calls == 11313 && *g_fake->convert_elsewhere == 0,
        "the glyph table: the converter called once for each of the 11313 Shift-JIS lead and trail pairs (EF 1F..EF 3F"
        " included), every call on the game thread");
    phase(L,
        "local s = hu.status().detail\n"
        "local t = s.glyph_table or {}\n"
        "check(s.functions.glyph_convert == CONVERT and t.size == 715 and t.cp932_defined == 490"
        "  and t.cp932_undefined == 225 and t.gaiji == 221 and type(t.build_ms) == 'number' and t.build_ms >= 0,"
        "  'status: the converter resolved to the stand-in; glyph_table: ' .. tostring(t.size) .. ' glyphs, '"
        "  .. tostring(t.cp932_defined) .. ' by a pair code page 932 defines, ' .. tostring(t.cp932_undefined)"
        "  .. ' not, ' .. tostring(t.gaiji) .. ' by a lead-0xEF pair, built in ' .. tostring(t.build_ms) .. ' ms')\n"
        "return finish()\n");
    check(memcmp(registry_row(kRowMenuwind), g_fake->pristine + kRowMenuwind * 0x2C, 0x2C) == 0,
        "the block of menuwind, closed, leaves its registry row as the table has it");

    // -- the mouse kept off a hidden window (0.6.0): +0x77 on a new instance's
    //    open, the unhide and the hide, and the stand-in's mouse mode picker
    //    through the hook, called here with no frame depending on it, which
    //    from 0.11.0 holds world mode over a hidden prompt alone (the yes/no
    //    windows below), never over a hidden window that asks nothing
    {
        typedef void (__fastcall* MouseFn)(void* mouse, void* edx);
        MouseFn mouse_mode;
        memcpy(&mouse_mode, &g_fake->mouse_mode, sizeof(mouse_mode));
        static uint8_t mouse[0x60];
        const int kRowAbility = row_named("ability");
        const uint8_t layer = g_fake->pristine[kRowAbility * 0x2C + 0x29];
        check(log && log[0x77] == 0, "the hide of the open chat log wrote 0 to its live menu's +0x77");
        phase(L,
            "check(h:hide('ability') == true, 'hide(ability) queues while ability is closed')\n"
            "return finish()\n");
        wait_frames(3);
        uint8_t* ability = static_cast<uint8_t*>(open_on_game_thread("menu    ability"));
        check(ability && ability[0x77] == 0 && hidden_in_place(kRowAbility, ability),
            "ability opened while hidden: +0x77 0, the open's post over the stand-in constructor's 1; its draw"
            " paints nothing, its registry row and live menu at its own layer");
        const LONG calls = *g_fake->menu_up_calls;
        put32(g_fake->mcb, 0x54, reinterpret_cast<uint32_t>(ability));
        mouse[0x4D] = 2;
        mouse_mode(mouse, NULL);
        check(mouse[0x4D] == 2 && *g_fake->menu_up_calls == calls + 1,
            "the mouse mode picker with the hidden window active, one that asks nothing: the original runs (mode 2);"
            " its +0x77 keeps clicks off its rows");
        phase(L,
            "check(h:unhide('ability') == true, 'unhide(ability) queues')\n"
            "return finish()\n");
        wait_frames(3);
        const bool shown = ability[0x60] == layer && ability[0x77] == 1 && registry_row(kRowAbility)[0x29] == layer
            && walk_of(ability) == 'd';
        mouse_mode(mouse, NULL);
        check(shown && mouse[0x4D] == 2 && *g_fake->menu_up_calls == calls + 2,
            "unhide: +0x77 back to 1 and the walk draws it; the picker's original runs and sets mode 2");
        phase(L,
            "check(h:hide('ability') == true, 'hide(ability) while it is open queues')\n"
            "return finish()\n");
        wait_frames(3);
        const bool hidden = ability[0x77] == 0 && hidden_in_place(kRowAbility, ability);
        mouse_mode(mouse, NULL);
        const bool menu_mode = mouse[0x4D] == 2 && *g_fake->menu_up_calls == calls + 3;
        put32(g_fake->mcb, 0x54, 0);
        mouse[0x4D] = 2;
        mouse_mode(mouse, NULL);
        check(hidden && menu_mode && mouse[0x4D] == 1 && *g_fake->menu_up_calls == calls + 4,
            "hide of the open window: +0x77 0 and its draw painting nothing at its own layer, the picker's original"
            " still runs over it (mode 2); with no active menu the original runs (mode 1, its callee called)");
        phase(L,
            "local seen = {}\n"
            "for _, e in ipairs((h:poll())) do seen[#seen + 1] = e.event .. ':' .. e.name end\n"
            "check(table.concat(seen, ' ') == 'opened:ability', 'events: ' .. table.concat(seen, ' '))\n"
            "check(h:unhide('ability') == true and h:close('ability') == true, 'unhide and close of ability queue')\n"
            "return finish()\n");
        wait_frames(3);
        check(!live_menu(kRowAbility)
                && memcmp(registry_row(kRowAbility), g_fake->pristine + kRowAbility * 0x2C, 0x2C) == 0,
            "ability closed, its registry row as the table has it");
    }

    // -- keys and the gamepad kept off a hidden window (0.7.6): the stand-in's
    //    routing routine, the game's bytes, calling the hooked sink, called
    //    here with no frame depending on it. From 0.11.0 only a hidden prompt
    //    loses them (the yes/no windows below); ability asks nothing, so its
    //    codes reach it hidden too, as the party list's must for <stpt>
    {
        typedef uint32_t (__fastcall* InputFn)(void* self, void* edx, int code);
        InputFn route;
        InputFn sink;
        memcpy(&route, &g_fake->menu_routing, sizeof(route));
        memcpy(&sink, &g_fake->menu_input, sizeof(sink));
        const FakeLog* fl = g_fake->log;
        const int kRowAbility = row_named("ability");
        const uint8_t layer = g_fake->pristine[kRowAbility * 0x2C + 0x29];
        uint8_t* ability = static_cast<uint8_t*>(open_on_game_thread("menu    ability"));
        check(ability && static_cast<uint8_t*>(g_fake->menu_input)[0] == 0xE9,
            "ability open; the sink's first bytes are the daemon's jump");
        put32(g_fake->mcb, 0x54, reinterpret_cast<uint32_t>(ability));
        const int calls = fl->sink_calls;
        const int opens = fl->main_menu_opens;
        const uint32_t visible = route(g_fake->mcb, NULL, 5);
        check(ability[0x60] == layer && visible == 1 && fl->sink_calls == calls + 1 && fl->sink_menu == ability
                && fl->sink_code == 5,
            "a key on the visible active window: the routing routine's code reaches its sink");
        phase(L,
            "check(h:hide('ability') == true, 'hide(ability) while it is open queues')\n"
            "return finish()\n");
        wait_frames(3);
        bool reached = hidden_in_place(kRowAbility, ability);
        for (int code = 1; code <= 0x20; ++code) {
            reached = route(g_fake->mcb, NULL, code) == 1 && fl->sink_code == code && reached;
        }
        check(reached && fl->sink_calls == calls + 1 + 0x20 && fl->sink_menu == ability
                && fl->main_menu_opens == opens,
            "hidden and active, a window that asks nothing: every code from the routing routine, 1 to 0x20, reaches"
            " its sink, and code 7 opens no main menu");
        const int routed = calls + 1 + 0x20;
        const uint32_t cursor = sink(ability, NULL, 9);
        const bool from_cursor = cursor == 1 && fl->sink_calls == routed + 1 && fl->sink_code == 9;
        const uint32_t wheel = sink(ability, NULL, 0x16);
        check(from_cursor && wheel == 1 && fl->sink_calls == routed + 2 && fl->sink_code == 0x16
                && fl->sink_menu == ability,
            "SetCursor's code 9 and the wheel's 0x16, from another caller, reach the hidden window's sink");
        put32(g_fake->mcb, 0x54, 0);
        const uint32_t none = route(g_fake->mcb, NULL, 5);
        const uint32_t menu_key = route(g_fake->mcb, NULL, 7);
        check(none == 0 && menu_key == 0 && fl->sink_calls == routed + 2 && fl->main_menu_opens == opens + 1,
            "with no active menu the routing routine runs as the game's: no sink, and code 7 opens the main menu");
        phase(L,
            "check(h:unhide('ability') == true, 'unhide(ability) queues')\n"
            "return finish()\n");
        wait_frames(3);
        put32(g_fake->mcb, 0x54, reinterpret_cast<uint32_t>(ability));
        const uint32_t again = route(g_fake->mcb, NULL, 6);
        check(ability[0x60] == layer && again == 1 && fl->sink_calls == routed + 3 && fl->sink_code == 6,
            "unhide: the routing routine's codes reach the window's sink as before");
        put32(g_fake->mcb, 0x54, 0);
        phase(L,
            "local seen = {}\n"
            "for _, e in ipairs((h:poll())) do seen[#seen + 1] = e.event .. ':' .. e.name end\n"
            "check(table.concat(seen, ' ') == 'closed:ability opened:ability', 'events, the last block\\'s close"
            " first: ' .. table.concat(seen, ' '))\n"
            "check(h:close('ability') == true, 'close of ability queues')\n"
            "return finish()\n");
        wait_frames(3);
        check(!live_menu(kRowAbility)
                && memcmp(registry_row(kRowAbility), g_fake->pristine + kRowAbility * 0x2C, 0x2C) == 0,
            "ability closed, its registry row as the table has it");
    }

    // -- info('ability').category: the controller's +0x64, the list the window
    //    shows, named for the known values, reported only while it is open
    {
        const int kRowAbility = row_named("ability");
        uint8_t* actl = controller_named("ability");
        put32(actl, 0x64, 2);
        check(open_on_game_thread("menu    ability") != NULL, "ability opens with 2 in its controller's +0x64");
        wait_frames(3);
        phase(L,
            "local p = h:info('ability')\n"
            "check(p.open and p.category == 2 and p.category_name == 'pet_commands',"
            "  'info(ability): category 2, category_name pet_commands: ' .. tostring(p.category) .. ' '"
            "  .. tostring(p.category_name))\n"
            "return finish()\n");
        put32(actl, 0x64, 7);
        phase(L,
            "local p = h:info('ability')\n"
            "check(p.category == 7 and p.category_name == nil, 'category 7: the number and no name: '"
            "  .. tostring(p.category) .. ' ' .. tostring(p.category_name))\n"
            "check(h:close('ability') == true, 'close of ability queues')\n"
            "return finish()\n");
        wait_frames(3);
        check(!live_menu(kRowAbility), "ability closed, 7 still in its controller");
        phase(L,
            "local p = h:info('ability')\n"
            "check(not p.open and p.category == nil and p.category_name == nil,"
            "  'info(ability) closed: neither field: ' .. tostring(p.category) .. ' ' .. tostring(p.category_name))\n"
            "h:poll()\n"
            "return finish()\n");
        put32(actl, 0x64, 0);
    }

    // -- the ability opener (0.10.0): the stand-in's site, hooked. An open
    //    through it opens ability with the kind stored in the controller and
    //    an opened event carrying it; closed carries the controller's list;
    //    a category block refuses that kind alone, blocked carrying it and
    //    naming the holder, and the others open; the whole-window block
    //    refuses every kind through it; info lists the categories blocked,
    //    each handle its own; a release drops them
    {
        const int kRowAbility = row_named("ability");
        const int kRowSort = row_named("abisortw");
        uint8_t* actl = controller_named("ability");
        check(static_cast<uint8_t*>(g_fake->ability_open)[0] == 0xE9, "the daemon's jump is on the stand-in's ability opener");
        phase(L, "h:poll()\nreturn finish()\n");
        ability_open_on_game_thread(2, 0, 0);
        wait_frames(3);
        check(live_menu(kRowAbility) && !live_menu(kRowSort) && *g_fake->ability_last_kind == 2 && get32(actl, 0x64) == 2,
            "ability_open(2, 0, 0) through the site opens ability alone, its body storing 2 at the controller's +0x64");
        *g_fake->close_frames = 1;
        phase(L,
            "local p = h:info('ability')\n"
            "check(p.open and p.category == 2 and p.category_name == 'pet_commands' and #p.blocked_categories == 0"
            "  and #p.mine.blocked_categories == 0, 'info(ability): open on pet_commands, no category blocked: '"
            "  .. tostring(p.category) .. ' ' .. tostring(p.category_name))\n"
            "local seen = {}\n"
            "for _, e in ipairs((h:poll())) do seen[#seen + 1] = e.event .. ':' .. e.name .. ':' .. tostring(e.category)"
            "  .. ':' .. tostring(e.category_name) end\n"
            "check(table.concat(seen, ' ') == 'opened:ability:2:pet_commands',"
            "  'the opened event carries the opener\\'s kind, 2, named pet_commands: ' .. table.concat(seen, ' '))\n"
            "check(h:close('ability') == true, 'close of ability queues')\n"
            "return finish()\n");
        wait_frames(4);
        *g_fake->close_frames = 0;
        phase(L,
            "local seen = {}\n"
            "for _, e in ipairs((h:poll())) do seen[#seen + 1] = e.event .. ':' .. e.name .. ':' .. tostring(e.category)"
            "  .. ':' .. tostring(e.category_name) end\n"
            "check(table.concat(seen, ' ') == 'closed:ability:2:pet_commands',"
            "  'the closed event carries the list the controller holds: ' .. table.concat(seen, ' '))\n"
            "local r, why = h:block('equip', 'pet_commands')\n"
            "check(r == nil and why == 'only ability has categories', 'block(equip, pet_commands): ' .. tostring(why))\n"
            "r, why = h:unblock('compass', '2')\n"
            "check(r == nil and why == 'only ability has categories', 'unblock(compass, 2): ' .. tostring(why))\n"
            "r, why = h:block('ability', 'spells')\n"
            "check(r == nil and tostring(why):find('no such category: spells', 1, true) == 1,"
            "  'block(ability, spells): ' .. tostring(why))\n"
            "r, why = h:block('ability', '32')\n"
            "check(r == nil and tostring(why):find('no such category: 32', 1, true) == 1, 'block(ability, 32): ' .. tostring(why))\n"
            "check(h:block('ability', 'pet_commands') == true, 'block(ability, pet_commands) holds')\n"
            "local p = h:info('ability')\n"
            "check(not p.blocked and not p.hidden and #p.blocked_categories == 1 and p.blocked_categories[1] == 'pet_commands'"
            "  and p.mine.blocked_categories[1] == 'pet_commands' and p.mine.blocked == false,"
            "  'info(ability): not blocked or hidden whole, pet_commands blocked, mine')\n"
            "local all = h:list()\n"
            "local listed = false\n"
            "for _, nm in ipairs(hu.status().blocked) do listed = listed or nm == 'ability' end\n"
            "check(all.ability.blocked == false and not listed,"
            "  'list() and status(): ability\\'s blocked stays the whole-window flag')\n"
            "return finish()\n");
        *g_fake->ability_last_kind = -1;
        ability_open_on_game_thread(2, 0, 0);
        wait_frames(3);
        check(!live_menu(kRowAbility) && *g_fake->ability_last_kind == -1,
            "ability_open(2, 0, 0) with pet_commands blocked: the hooked opener runs nothing; nothing opens, no kind"
            " stored");
        ability_open_on_game_thread(2, 1, 0);
        wait_frames(3);
        check(!live_menu(kRowAbility) && !live_menu(kRowSort) && *g_fake->ability_last_kind == -1,
            "with flag 1 neither abisortw nor ability opens");
        ability_open_on_game_thread(1, 0, 0);
        wait_frames(3);
        check(live_menu(kRowAbility) && *g_fake->ability_last_kind == 1 && get32(actl, 0x64) == 1,
            "ability_open(1, 0, 0) still opens ability on job_abilities");
        phase(L,
            "local seen = {}\n"
            "for _, e in ipairs((h:poll())) do seen[#seen + 1] = e.event .. ':' .. e.name .. ':' .. tostring(e.category)"
            "  .. ':' .. tostring(e.category_name)"
            "  .. (e.event == 'blocked' and (':' .. table.concat(e.by, '+') .. ':' .. tostring(e.mine)) or '') end\n"
            "check(table.concat(seen, ' ') == 'blocked:ability:2:pet_commands:alpha:true blocked:ability:2:pet_commands:alpha:true"
            " opened:ability:1:job_abilities',"
            "  'two blocked events carrying 2, by alpha and mine, then opened carrying 1: ' .. table.concat(seen, ' '))\n"
            "check(h:unblock('ability', 'pet_commands') == true and #h:info('ability').blocked_categories == 0,"
            "  'unblock(ability, pet_commands) drops it')\n"
            "check(h:close('ability') == true, 'close ability')\n"
            "return finish()\n");
        wait_frames(3);
        ability_open_on_game_thread(2, 0, 0);
        wait_frames(3);
        check(live_menu(kRowAbility) && get32(actl, 0x64) == 2,
            "unblocked: ability_open(2, 0, 0) opens again on pet_commands");
        phase(L,
            "h:poll()\n"
            "check(h:block('ability', '2') == true, 'block(ability, 2) by number with the window on list 2 queues its close')\n"
            "return finish()\n");
        wait_frames(3);
        check(!live_menu(kRowAbility), "the category block closed the window showing that list");
        ability_open_on_game_thread(3, 0, 0);
        wait_frames(3);
        phase(L,
            "h:poll()\n"
            "check(h:block('ability', 'job_abilities') == true, 'block(ability, job_abilities) with the window on list 3')\n"
            "return finish()\n");
        wait_frames(3);
        check(live_menu(kRowAbility) && get32(actl, 0x64) == 3,
            "a category block leaves the window open on another list");
        phase(L,
            "check(h:unblock('ability', '2') == true and h:unblock('ability', 'job_abilities') == true, 'both dropped')\n"
            "check(h:block('ability') == true, 'block(ability) whole, the window open: queues its close')\n"
            "return finish()\n");
        wait_frames(3);
        check(!live_menu(kRowAbility), "the whole block closed it");
        *g_fake->ability_last_kind = -1;
        ability_open_on_game_thread(1, 0, 0);
        ability_open_on_game_thread(4, 0, 0);
        ability_open_on_game_thread(20, 0, 0);
        wait_frames(3);
        check(!live_menu(kRowAbility) && *g_fake->ability_last_kind == -1,
            "blocked whole: kinds 1, 4 and 20 through the opener open nothing");
        phase(L,
            "local seen = {}\n"
            "for _, e in ipairs((h:poll())) do if e.event == 'blocked' then seen[#seen + 1] = tostring(e.category) .. ':'"
            "  .. tostring(e.category_name) .. ':' .. table.concat(e.by, '+') .. ':' .. tostring(e.mine) end end\n"
            "check(table.concat(seen, ' ') == '1:job_abilities:alpha:true 4:job_traits:alpha:true 20:monstrosity:alpha:true',"
            "  'blocked events carry each kind, 4 named job_traits and 20 monstrosity, by alpha: ' .. table.concat(seen, ' '))\n"
            "local p = h:info('ability')\n"
            "check(p.blocked and #p.blocked_categories == 0, 'info(ability): blocked whole, no category')\n"
            "check(h:unblock('ability') == true, 'unblock(ability)')\n"
            "hl = hu.new('lists')\n"
            "check(hl:block('ability', '4') == true and hl:block('ability', 'weapon_skills') == true"
            "  and h:block('ability', '7') == true, 'lists blocks 4 by number and weapon_skills by name, alpha 7')\n"
            "p = h:info('ability')\n"
            "local mine = hl:info('ability').mine\n"
            "check(table.concat(p.blocked_categories, ' ') == 'weapon_skills job_traits 7'"
            "  and table.concat(p.mine.blocked_categories, ' ') == '7'"
            "  and table.concat(mine.blocked_categories, ' ') == 'weapon_skills job_traits',"
            "  'info(ability).blocked_categories lists every handle\\'s, named where the engine has a name, in category'"
            "  .. ' order; mine each handle\\'s own: ' .. table.concat(p.blocked_categories, ' ') .. ' / '"
            "  .. table.concat(mine.blocked_categories, ' '))\n"
            "check(hl:release() == true, 'lists releases')\n"
            "p = h:info('ability')\n"
            "check(table.concat(p.blocked_categories, ' ') == '7', 'the release dropped its two: '"
            "  .. table.concat(p.blocked_categories, ' '))\n"
            "check(h:unblock('ability', '7') == true and #h:info('ability').blocked_categories == 0, 'alpha unblocks 7')\n"
            "return finish()\n");
        ability_open_on_game_thread(4, 0, 0);
        wait_frames(3);
        check(live_menu(kRowAbility) && get32(actl, 0x64) == 4, "nothing held: ability_open(4, 0, 0) opens job traits");
        phase(L,
            "local seen = {}\n"
            "for _, e in ipairs((h:poll())) do seen[#seen + 1] = e.event .. ':' .. tostring(e.category_name) end\n"
            "check(table.concat(seen, ' ') == 'opened:job_traits', 'opened carrying 4, job_traits: ' .. table.concat(seen, ' '))\n"
            "check(h:close('ability') == true, 'close ability')\n"
            "return finish()\n");
        wait_frames(3);
        put32(actl, 0x64, 0);
    }

    // -- the lists as the slots of 0.10.0 name them, exactly as 0.10.0 did:
    //    through block7, unblock7, poll5 and info4, job_abilities,
    //    pet_commands, weapon_skills (3 and its Monstrosity form 20, listed
    //    once) and job_traits, every other list by its number alone; the
    //    library's block8, poll8, info9 and unblock9 name every list, 20
    //    monstrosity, its unblock by name dropping what its block holds by
    //    that name
    {
        const int kRowAbility = row_named("ability");
        HuEngineApi api;
        HuEngineHandle raw = {-1, 0};
        char why[512] = "";
        if (record_api(&api)) {
            api.handle_open("old", &raw, why, sizeof(why));
        }
        phase(L, "h:poll()\nreturn finish()\n");
        const bool held = raw.slot >= 0 && api.block7(&raw, "ability", "Weapon_Skills", why, sizeof(why)) == 1
            && api.block7(&raw, "ability", "6", why, sizeof(why)) == 1;
        const HuEngineReply& info = raw_read(api.info4, &raw, "ability");
        const bool listed_once = strings_named(info, "weapon_skills") == 2 && !has_string(info, "monstrosity")
            && !has_string(info, "blood_pact_rage");
        const bool monstrosity = raw.slot >= 0 && api.block7(&raw, "ability", "monstrosity", why, sizeof(why)) == 0
            && strcmp(why, "no such category: monstrosity (job_abilities, pet_commands, weapon_skills, job_traits, or"
                " a number 1..31)") == 0;
        const bool rage = raw.slot >= 0 && api.block7(&raw, "ability", "blood_pact_rage", why, sizeof(why)) == 0
            && strstr(why, "no such category: blood_pact_rage (") == why;
        raw_read(api.poll5, &raw);
        *g_fake->ability_last_kind = -1;
        ability_open_on_game_thread(20, 0, 0);
        ability_open_on_game_thread(3, 0, 0);
        ability_open_on_game_thread(6, 0, 0);
        wait_frames(3);
        const HuEngineReply& polled = raw_read(api.poll5, &raw);
        const bool poll5 = strings_named(polled, "weapon_skills") == 2 && !has_string(polled, "monstrosity")
            && !has_string(polled, "blood_pact_rage");
        check(held && listed_once && monstrosity && rage && poll5 && !live_menu(kRowAbility)
                && *g_fake->ability_last_kind == -1,
            "block7('ability', 'weapon_skills') holds 3 and 20 and block7 of 6 holds 6; info4 lists 3 and 20 once as"
            " weapon_skills and 6 by its number; block7 refuses monstrosity and blood_pact_rage with 0.10.0's"
            " reason; poll5's blocked events name 20 and 3 weapon_skills and 6 nothing: 0.10.0's results");
        phase(L,
            "local seen = {}\n"
            "for _, e in ipairs((h:poll())) do if e.event == 'blocked' then"
            "  seen[#seen + 1] = tostring(e.category) .. ':' .. tostring(e.category_name) end end\n"
            "check(table.concat(seen, ' ') == '20:monstrosity 3:weapon_skills 6:blood_pact_rage',"
            "  'poll8, the library\\'s, names every list, 20 monstrosity: ' .. table.concat(seen, ' '))\n"
            "local listed = table.concat(h:info('ability').blocked_categories, ' ')\n"
            "check(listed == 'weapon_skills blood_pact_rage monstrosity',"
            "  'info9, the library\\'s, names them as poll8 does: ' .. listed)\n"
            "return finish()\n");
        const bool dropped = raw.slot >= 0 && api.unblock7(&raw, "ability", "weapon_skills", why, sizeof(why)) == 1
            && api.unblock7(&raw, "ability", "6", why, sizeof(why)) == 1
            && !has_string(raw_read(api.info4, &raw, "ability"), "weapon_skills");
        check(dropped, "unblock7('ability', 'weapon_skills') drops 3 and 20, unblock7 of 6 drops 6");
        phase(L,
            "check(h:block('ability', 'weapon_skills') == true and h:block('ability', 'monstrosity') == true,"
            "  'block(ability, weapon_skills) and block(ability, monstrosity) through block8')\n"
            "local mine = table.concat(h:info('ability').mine.blocked_categories, ' ')\n"
            "check(mine == 'weapon_skills monstrosity', 'info9: 3 weapon_skills, 20 monstrosity: ' .. mine)\n"
            "check(h:unblock('ability', 'Weapon_Skills') == true, 'unblock(ability, Weapon_Skills) through unblock9')\n"
            "h:poll()\n"
            "return finish()\n");
        ability_open_on_game_thread(20, 0, 0);
        ability_open_on_game_thread(3, 0, 0);
        wait_frames(3);
        check(live_menu(kRowAbility) && *g_fake->ability_last_kind == 3,
            "the library's unblock of weapon_skills dropped 3 alone: ability opens on 3");
        phase(L,
            "local seen = {}\n"
            "for _, e in ipairs((h:poll())) do seen[#seen + 1] = e.event .. ':' .. tostring(e.category) end\n"
            "check(table.concat(seen, ' ') == 'blocked:20 opened:3', '20 still held by block8\\'s monstrosity: '"
            "  .. table.concat(seen, ' '))\n"
            "check(h:unblock('ability', 'monstrosity') == true and h:close('ability') == true,"
            "  'unblock(ability, monstrosity), and close')\n"
            "return finish()\n");
        wait_frames(3);
        ability_open_on_game_thread(20, 0, 0);
        wait_frames(3);
        check(live_menu(kRowAbility) && *g_fake->ability_last_kind == 20, "unblocked: ability opens on 20");
        phase(L,
            "h:poll()\n"
            "check(h:close('ability') == true, 'close ability')\n"
            "return finish()\n");
        wait_frames(3);
        put32(controller_named("ability"), 0x64, 0);
        if (raw.slot >= 0) {
            api.handle_release(&raw);
        }
    }

    // -- open and open_set (0.11.0): open of exactly the window named, the
    //    windows it refuses on every open slot and from abi 9, a list of
    //    ability or magic alone through the game's opener of it, and the sets
    //    through theirs: the stand-in's ability opener with its pane, the
    //    magic opener (the game's bytes around a rebuild opening subwindo
    //    every time), the equipment set's opener (the game's bytes and
    //    layout, its guard, its opens and the 0x061 request) and the
    //    Monstrosity check the equipment shortcut asks (the game's bytes)
    {
        static const char* const family[] = {"ability", "abisortw", "subwindo", "magic", "mgcsortw", "blusortw",
                                             "equip", "persona", "money", "inventor", "iteminfo"};
        const int count = static_cast<int>(sizeof(family) / sizeof(family[0]));
        // The family's open windows, joined by ' ', in this order.
        auto opened = [&](char* out, size_t size) -> const char* {
            out[0] = '\0';
            for (int i = 0; i < count; ++i) {
                const uint8_t* menu = live_menu(row_named(family[i]));
                if (menu && !menu[0x6E] && !menu[0x70]) {
                    const size_t used = strlen(out);
                    snprintf(out + used, size - used, "%s%s", used ? " " : "", family[i]);
                }
            }
            return out;
        };
#define HU_CLOSE_FAMILY \
            "for _, w in ipairs({'ability', 'abisortw', 'subwindo', 'magic', 'mgcsortw', 'blusortw', 'equip'," \
            "    'persona', 'money', 'inventor', 'iteminfo'}) do\n" \
            "  if h:info(w).open then check(h:close(w) == true, 'close ' .. w) end\n" \
            "end\n"
        static const char kCloseFamily[] = "h:poll()\n" HU_CLOSE_FAMILY "return finish()\n";
        char now[192];
        char label[512];
        phase(L, kCloseFamily);
        wait_frames(3);
        snprintf(label, sizeof(label), "the family closed before the opens: [%s]", opened(now, sizeof(now)));
        check(now[0] == '\0', label);
        phase(L,
            "h:poll()\n"
            "local ok, why = h:open('equip')\n"
            "check(ok == nil and why == \"equip opens only with its set: use open_set('equipment')\","
            "  'open(equip) refused, pointing at its set: ' .. tostring(why))\n"
            "ok, why = h:open('inventor')\n"
            "check(ok == nil and tostring(why):find('inventor cannot be opened alone', 1, true) == 1"
            "  and why:find(\"open_set('equipment')\", 1, true), 'open(inventor) refused: ' .. tostring(why))\n"
            "ok, why = h:open('evitem')\n"
            "check(ok == nil and tostring(why):find('evitem cannot be opened alone: it opens with what the game left', 1,"
            "  true) == 1, 'open(evitem) refused: ' .. tostring(why))\n"
            "ok, why = h:open('ability')\n"
            "check(ok == nil and why == \"ability opens one list at a time: name the list, as in open('ability',"
            " 'job_abilities')\", 'open(ability) with no list refused: ' .. tostring(why))\n"
            "ok, why = h:open('magic')\n"
            "check(ok == nil and why == \"magic opens one list at a time: name the list, as in open('magic',"
            " 'white_magic')\", 'open(magic) with no list refused: ' .. tostring(why))\n"
            "local lists = {}\n"
            "for _, c in ipairs({{'ability', 'spells'}, {'ability', '5'}, {'ability', '7'}, {'ability', '0'},"
            "    {'ability', '24'}, {'ability', '31'}, {'magic', '9'}, {'magic', '0'}, {'magic', 'pet_commands'}}) do\n"
            "  local r, w = h:open(c[1], c[2])\n"
            "  if r == nil and tostring(w):find('no such list of ' .. c[1] .. ': ' .. c[2], 1, true) == 1 then"
            "    lists[#lists + 1] = c[1] .. ':' .. c[2] end\n"
            "end\n"
            "check(#lists == 9, 'a list past either table, 0, 5, 7 or another window\\'s list refused: '"
            "  .. table.concat(lists, ' '))\n"
            "ok, why = h:open('equip', 'job_abilities')\n"
            "check(ok == nil and why == 'equip takes no argument: only ability, magic and the windows the README lists as"
            " taking one do', 'open(equip, list): '"
            "  .. tostring(why))\n"
            "ok, why = h:open('fulllog')\n"
            "check(ok == nil and why == \"fulllog has no controller now, and the game's open would crash the client\","
            "  'open(fulllog), no controller in its slot: ' .. tostring(why))\n"
            "ok, why = h:open('gift')\n"
            "check(ok == nil and tostring(why):find('gift needs trade open', 1, true) == 1,"
            "  'open(gift) with trade closed: ' .. tostring(why))\n"
            "ok, why = h:open('auc2')\n"
            "check(ok == nil and tostring(why):find('auc2 needs auc1 open', 1, true) == 1,"
            "  'open(auc2) with auc1 closed: ' .. tostring(why))\n"
            "ok, why = h:block('gift')\n"
            "check(ok == nil and tostring(why):find('gift cannot be blocked', 1, true) == 1 and not h:info('gift').blockable,"
            "  'block(gift) refused, and info says it cannot be blocked: ' .. tostring(why))\n"
            "check(h:close('gift') == true, 'close(gift) with trade closed queues')\n"
            "ok, why = h:open_set('nosuch')\n"
            "check(ok == nil and why == 'no such set: nosuch (equipment, ability, magic, map, set_spells, attachments,'"
            "  .. ' linkshell, instincts, synthesis, tabula, macros, friend, mail, mount, compare, status, region or'"
            "  .. ' text_colors)', 'open_set(nosuch): ' .. tostring(why))\n"
            "ok, why = h:open_set('equipment', 'job_abilities')\n"
            "check(ok == nil and why == 'the equipment set takes no argument', 'open_set(equipment, list): '"
            "  .. tostring(why))\n"
            "ok, why = h:open_set('ability')\n"
            "check(ok == nil and why == \"the ability set opens one list: name it, as in open_set('ability',"
            " 'job_abilities')\", 'open_set(ability) with no list: ' .. tostring(why))\n"
            "ok, why = h:open_set('magic', 'ready')\n"
            "check(ok == nil and tostring(why):find('no such list of magic: ready', 1, true) == 1,"
            "  'open_set(magic, ready): ' .. tostring(why))\n"
            "local raised, err = pcall(h.open, h, 'ability', {})\n"
            "check(not raised and tostring(err):find('string expected', 1, true),"
            "  'open with a table for the list raises: ' .. tostring(err))\n"
            "raised, err = pcall(h.open_set, h, {})\n"
            "check(not raised and tostring(err):find('string expected', 1, true),"
            "  'open_set with a table for the set raises: ' .. tostring(err))\n"
            "check(#h:poll() == 0, 'no refusal reached the game thread')\n"
            "return finish()\n");
        // every open slot of every engine abi refuses the windows whose open
        // alone crashes or strands the client
        {
            HuEngineApi api;
            HuEngineHandle raw = {-1, 0};
            char why[512] = "";
            if (record_api(&api)) {
                api.handle_open("raw_open", &raw, why, sizeof(why));
            }
            static const char* const kOpenRefusedAll[] = {"inventor", "bank", "itmstora", "shop", "bluinven", "bluequip", "cmbhlst", "mp_invnt", "holdtime", "mcrledit", "mcresed", "loot",
                "lootope", "prvdungp", "conquer1", "beseige2", "campsan", "camparea", "auc4", "faqmain", "textcol1",
                "textcol3", "conf11s", "rmlo1", "rmlo2", "rmlo4", "plnt1", "plnt2", "plnt3", "plnt4", "plnt5",
                "plnt6", "netwait", "netbar", "keypad2", "keyselec", "keypad3", "roomlist", "mogdoor", "bankmenu",
                "chocobor", "racecard", "mes1rcv", "mes2frnd", "auc1", "auclist", "auchisto", "ranking", "shopmain",
                "guildsho", "ok", "mp_dead"};
            const int kOpenRefusedCountAll = static_cast<int>(sizeof(kOpenRefusedAll) / sizeof(kOpenRefusedAll[0]));
            int refused = 0;
            int named = 0;
            int every = 0;
            for (int i = 0; raw.slot >= 0 && i < kOpenRefusedCountAll; ++i) {
                const char* nm = kOpenRefusedAll[i];
                char want[48];
                snprintf(want, sizeof(want), "%s cannot be opened alone", nm);
                const bool prompt = strcmp(nm, "ok") == 0 || strcmp(nm, "mp_dead") == 0 || strcmp(nm, "mogdoor") == 0
                    || strcmp(nm, "bankmenu") == 0;
                const int32_t r[4] = {
                    api.open(&raw, nm, why, sizeof(why)) == 0 && strstr(why, want) == why,
                    api.open3(&raw, nm, why, sizeof(why)) == 0 && strstr(why, want) == why,
                    api.open8(&raw, nm, why, sizeof(why)) == 0 && (prompt ? strstr(why, "opens only for the game's own"
                        " event") != NULL : strstr(why, want) == why),
                    api.open9(&raw, nm, NULL, why, sizeof(why)) == 0 && (prompt ? strstr(why, "opens only for the"
                        " game's own event") != NULL : strstr(why, want) == why),
                };
                for (int k = 0; k < 4; ++k) {
                    refused += r[k];
                }
                named += r[0] && r[1];
                ++every;
            }
            int gift = 0;
            if (raw.slot >= 0) {
                gift += api.block(&raw, "gift", why, sizeof(why)) == 0 && strstr(why, "gift cannot be blocked") == why;
                gift += api.block3(&raw, "gift", why, sizeof(why)) == 0 && strstr(why, "gift cannot be blocked") == why;
                gift += api.block5(&raw, "gift", why, sizeof(why)) == 0 && strstr(why, "gift cannot be blocked") == why;
                gift += api.block7(&raw, "gift", NULL, why, sizeof(why)) == 0
                    && strstr(why, "gift cannot be blocked") == why;
                gift += api.block8(&raw, "gift", NULL, why, sizeof(why)) == 0
                    && strstr(why, "gift cannot be blocked") == why;
                gift += api.open(&raw, "gift", why, sizeof(why)) == 0 && strstr(why, "gift needs trade open") == why;
                gift += api.open3(&raw, "mcrselop", why, sizeof(why)) == 0
                    && strstr(why, "mcrselop needs mcrselec open") == why;
            }
            check(gift == 7, "gift refused by the block slots of engine abi 1, 3, 5, 7 and 8, and opened by no old slot"
                " while trade is closed; mcrselop by none while mcrselec is closed");
            const bool old_equip = raw.slot >= 0 && api.open8(&raw, "equip", why, sizeof(why)) == 1
                && api.open9(&raw, "equip", NULL, why, sizeof(why)) == 0;
            const bool missing = raw.slot >= 0 && api.open3(&raw, "fulllog", why, sizeof(why)) == 0
                && strcmp(why, "fulllog has no controller now, and the game's open would crash the client") == 0;
            if (raw.slot >= 0) {
                api.handle_release(&raw);
            }
            check(every == kOpenRefusedCountAll && refused == 4 * every && named == every && old_equip && missing,
                "every window whose open alone crashes or strands the client refused by the open slots of engine abi"
                " 1, 3, 8 and 9, the earlier two with its own reason, the prompts among them by the later two as"
                " prompts; a window with no controller in its slot refused by an old slot too; open8 keeps"
                " opening equip, open9 refuses it");
            wait_frames(3);
            phase(L, kCloseFamily);
            wait_frames(3);
        }

        // a list of ability alone, then its set
        phase(L,
            "h:poll()\n"
            "check(h:open('ability', 'blood_pact_rage') == true, 'open(ability, blood_pact_rage) queues')\n"
            "return finish()\n");
        wait_frames(3);
        const int32_t* last = g_fake->ability_last;
        snprintf(label, sizeof(label), "open(ability, blood_pact_rage): the ability opener with (6, 0, 0): ability"
            " alone, no abisortw, and no subwindo though the list has a pane: [%s]", opened(now, sizeof(now)));
        check(strcmp(now, "ability") == 0 && last[0] == 6 && last[1] == 0 && last[2] == 0, label);
        phase(L,
            "local seen = {}\n"
            "for _, e in ipairs((h:poll())) do seen[#seen + 1] = e.event .. ':' .. e.name .. ':' .. tostring(e.category)"
            "  .. ':' .. tostring(e.category_name) end\n"
            "check(table.concat(seen, ' ') == 'opened:ability:6:blood_pact_rage',"
            "  'one opened event, carrying 6, blood_pact_rage: ' .. table.concat(seen, ' '))\n"
            "check(h:open_set('ability', '6') == true, 'open_set(ability, 6) queues')\n"
            "return finish()\n");
        wait_frames(3);
        snprintf(label, sizeof(label), "open_set(ability, 6): the opener with (6, 1, 1): ability, abisortw and"
            " subwindo: [%s]", opened(now, sizeof(now)));
        check(strcmp(now, "ability abisortw subwindo") == 0 && last[0] == 6 && last[1] == 1 && last[2] == 1, label);
        phase(L,
            "local seen = {}\n"
            "for _, e in ipairs((h:poll())) do seen[#seen + 1] = e.event .. ':' .. e.name end\n"
            "check(table.concat(seen, ' ') == 'opened:abisortw opened:subwindo',"
            "  'each window its own opened event, in the game\\'s order, none for ability, open on the same list: '"
            "  .. table.concat(seen, ' '))\n"
            HU_CLOSE_FAMILY
            "return finish()\n");
        wait_frames(3);

        // a list of magic alone, its rebuild's subwindo kept from opening,
        // then its sets
        const LONG lists = *g_fake->magic_lists;
        const LONG closes = *g_fake->magic_closes;
        put32(controller_named("magic"), 0xC0, 0xBAADF00Du);
        phase(L,
            "h:poll()\n"
            "check(h:open('magic', 'songs') == true, 'open(magic, songs) queues')\n"
            "return finish()\n");
        wait_frames(3);
        const int32_t* mlast = g_fake->magic_last;
        snprintf(label, sizeof(label), "open(magic, songs): the magic opener with (5, 0, 0), its rebuild run: magic"
            " alone, the rebuild's open of subwindo refused: [%s]", opened(now, sizeof(now)));
        check(strcmp(now, "magic") == 0 && *g_fake->magic_lists == lists + 1 && mlast[0] == 5 && mlast[1] == 0
                && mlast[2] == 0, label);
        phase(L,
            "local seen = {}\n"
            "for _, e in ipairs((h:poll())) do seen[#seen + 1] = e.event .. ':' .. e.name end\n"
            "check(table.concat(seen, ' ') == 'opened:magic', 'opened{magic} alone, nothing about subwindo: '"
            "  .. table.concat(seen, ' '))\n"
            "check(h:info('magic').open and not h:info('subwindo').open, 'info: magic open, subwindo not')\n"
            "check(h:close('magic') == true, 'close(magic) queues')\n"
            "return finish()\n");
        wait_frames(3);
        snprintf(label, sizeof(label), "magic's begin-close, run by the close of a list opened alone on a fresh"
            " controller, finds the sort window's controller the setup wrote with no sort, NULL (%ld runs, %ld"
            " wild)", static_cast<long>(*g_fake->magic_closes - closes), static_cast<long>(*g_fake->magic_close_wild));
        check(*g_fake->magic_closes == closes + 1 && *g_fake->magic_close_wild == 0 && !live_menu(row_named("magic")),
            label);
        phase(L,
            "h:poll()\n"
            "check(h:open_set('magic', 'blue_magic') == true, 'open_set(magic, blue_magic) queues')\n"
            "return finish()\n");
        wait_frames(3);
        const bool blue = strcmp(opened(now, sizeof(now)), "subwindo magic blusortw") == 0 && mlast[0] == 6
            && mlast[1] == 1 && mlast[2] == 0;
        phase(L, kCloseFamily);
        wait_frames(3);
        phase(L,
            "h:poll()\n"
            "check(h:open_set('magic', 'white_magic') == true, 'open_set(magic, white_magic) queues')\n"
            "return finish()\n");
        wait_frames(3);
        snprintf(label, sizeof(label), "open_set(magic, blue_magic): magic, blusortw and subwindo; open_set(magic,"
            " white_magic): mgcsortw: [%s]", opened(now, sizeof(now)));
        check(blue && strcmp(now, "subwindo magic mgcsortw") == 0 && mlast[0] == 1, label);
        phase(L, kCloseFamily);
        wait_frames(3);

        // the equipment set
        const LONG sets = *g_fake->equipment_opens;
        const LONG stats = *g_fake->stats_requests;
        uint8_t* pctl = controller_named("persona");
        pctl[0x1C] = 0;
        phase(L,
            "h:poll()\n"
            "check(h:open_set('equipment') == true, 'open_set(equipment) queues')\n"
            "return finish()\n");
        wait_frames(3);
        snprintf(label, sizeof(label), "open_set(equipment): the set's opener past its tests and guard: persona in"
            " mode 2, money, inventor, iteminfo and equip, then the 0x061 request: [%s]", opened(now, sizeof(now)));
        check(strcmp(now, "equip persona money inventor iteminfo") == 0 && *g_fake->equipment_opens == sets + 1
                && *g_fake->stats_requests == stats + 1 && pctl[0x1C] == 2, label);
        phase(L,
            "local seen = {}\n"
            "for _, e in ipairs((h:poll())) do seen[#seen + 1] = e.event .. ':' .. e.name end\n"
            "check(table.concat(seen, ' ') == 'opened:persona opened:money opened:inventor opened:iteminfo opened:equip',"
            "  'each window its own opened event, in the game\\'s order, and no error: ' .. table.concat(seen, ' '))\n"
            HU_CLOSE_FAMILY
            "return finish()\n");
        wait_frames(3);
        // refused by the game: in Monstrosity (the player data's word, then
        // the mode byte with that word clear and the data's pointer in eax),
        // by the opener's first test, and by its guard
        uint8_t* ectl = controller_named("equip");
        const int16_t guard = get16(ectl, 0x1A);
        struct Refusal {
            const char* why;
            const char* reason;
        };
        static const Refusal refusals[4] = {
            {"the Monstrosity word of the player data", "the game refused it: it opens no equipment set in Monstrosity"},
            {"the mode byte 0x17", "the game refused it: it opens no equipment set in Monstrosity"},
            {"the first test of the opener", "the game refused it"},
            {"the guard of the opener on the controller of equip", "the game refused it"},
        };
        int refusals_seen = 0;
        for (int k = 0; k < 4; ++k) {
            put16(g_fake->mon_player, 0x10, k == 0 ? 1 : 0);
            *g_fake->game_mode = k == 1 ? 0x17 : 0;
            *g_fake->set_refusal = k == 2 ? 1 : 0;
            put16(ectl, 0x1A, k == 3 ? 1 : 0);
            const LONG before = *g_fake->equipment_opens;
            phase(L,
                "h:poll()\n"
                "check(h:open_set('equipment') == true, 'open_set(equipment) queues')\n"
                "return finish()\n");
            wait_frames(3);
            char script[1024];
            snprintf(script, sizeof(script),
                "local seen = {}\n"
                "for _, e in ipairs((h:poll())) do seen[#seen + 1] = e.event .. ':' .. tostring(e.verb) .. ':'"
                "  .. tostring(e.name) .. ':' .. tostring(e.reason) end\n"
                "check(table.concat(seen, ' ') == 'error:open_set:equipment:%s', 'refused by %s: '"
                "  .. table.concat(seen, ' '))\n"
                "return finish()\n", refusals[k].reason, refusals[k].why);
            phase(L, script);
            refusals_seen += strcmp(opened(now, sizeof(now)), "") == 0 && *g_fake->equipment_opens == before;
        }
        put16(g_fake->mon_player, 0x10, 0);
        *g_fake->game_mode = 0;
        *g_fake->set_refusal = 0;
        put16(ectl, 0x1A, guard);
        check(refusals_seen == 4, "open_set(equipment) refused by the game, each an error event to the handle,"
            " nothing opened: in Monstrosity by the player data's word and by the mode byte (the opener not run,"
            " al alone read), by the opener's first test and by its guard");

        // refused up front: a window of the set blocked, a list blocked
        phase(L,
            "local other = hu.new('blocker')\n"
            "check(other:block('persona') == true, 'another handle blocks persona')\n"
            "local ok, why = h:open_set('equipment')\n"
            "check(ok == nil and why == 'persona is blocked; the equipment set opens it', 'open_set(equipment) with'"
            "  .. ' persona blocked: ' .. tostring(why))\n"
            "check(other:block('blusortw') == true, 'and blusortw')\n"
            "ok, why = h:open_set('magic', 'blue_magic')\n"
            "check(ok == nil and why == 'blusortw is blocked; the magic set opens it', 'open_set(magic, blue_magic)'"
            "  .. ' with blusortw blocked: ' .. tostring(why))\n"
            "check(h:open_set('magic', 'white_magic') == true, 'open_set(magic, white_magic) does not open blusortw:'"
            "  .. ' it queues')\n"
            "check(other:block('ability', 'pet_commands') == true, 'and the pet commands list')\n"
            "ok, why = h:open('ability', 'pet_commands')\n"
            "local list_ok = ok == nil and why == \"ability's list pet_commands is blocked\"\n"
            "ok, why = h:open_set('ability', '2')\n"
            "check(list_ok and ok == nil and why == \"ability's list pet_commands is blocked\","
            "  'open and open_set of a blocked list refused: ' .. tostring(why))\n"
            "check(other:release() == true, 'the blocker releases')\n"
            "return finish()\n");
        wait_frames(3);
        phase(L, kCloseFamily);
        wait_frames(3);

        // a pet's list with no pet out: the game closes it again at once,
        // and the handle is told
        *g_fake->no_pet = 1;
        *g_fake->close_frames = 1;
        phase(L,
            "h:poll()\n"
            "check(h:open('ability', 'pet_commands') == true, 'open(ability, pet_commands) queues')\n"
            "return finish()\n");
        wait_frames(4);
        *g_fake->no_pet = 0;
        *g_fake->close_frames = 0;
        phase(L,
            "local seen = {}\n"
            "for _, e in ipairs((h:poll())) do seen[#seen + 1] = e.event .. ':' .. tostring(e.name)"
            "  .. (e.event == 'error' and (':' .. e.verb .. ':' .. e.reason) or '') end\n"
            "check(table.concat(seen, ' ') == 'opened:ability closed:ability error:ability:open:the game refused it:'"
            "  .. ' it closed again at once', 'a pet\\'s list with no pet out: opened, closed by the game\\'s'"
            "  .. ' per-frame, then the error: ' .. table.concat(seen, ' '))\n"
            "check(not h:info('ability').open, 'ability is closed')\n"
            "return finish()\n");
#undef HU_CLOSE_FAMILY
    }

    // -- the other sets (0.11.0), each through the stand-in's opener of the
    //    game's bytes, which the engine's signatures and shapes resolve, run
    //    as the player's route runs it: map; set_spells and attachments on
    //    their controllers, refused but for the job; linkshell behind its
    //    row's check; instincts in Monstrosity alone; synthesis and tabula
    //    for an inventory index through the item use's type, and the item
    //    use's check; macros on the editor's object. Then the open
    //    routine's last argument as the game's openers pass it: titlehan
    //    shows over titlewin, targetwi parks under ut_point.
    {
        static const char* const members[] = {"mapframe", "map0", "persona", "bluehelp", "bluinven", "bluepoin",
                                              "bluequip", "statpup", "automato", "link5", "link6", "mp_stat",
                                              "mp_cstm", "mp_mncst", "inventor", "iteminfo", "tskill1", "prvdungp",
                                              "mcrbedit", "mcr1edit", "mcr2edit", "titlewin", "titlehan", "targetwi",
                                              "ut_point"};
        const int count = static_cast<int>(sizeof(members) / sizeof(members[0]));
        typedef int (__fastcall* CloseFn)(void* mcb, void* edx, const char* key);
        CloseFn close_by_name;
        memcpy(&close_by_name, &g_fake->close, sizeof(close_by_name));
        // The members open now, joined by ' ', in this order.
        auto opened = [&](char* out, size_t size) -> const char* {
            out[0] = '\0';
            for (int i = 0; i < count; ++i) {
                const uint8_t* menu = live_menu(row_named(members[i]));
                if (menu && !menu[0x6E] && !menu[0x70]) {
                    const size_t used = strlen(out);
                    snprintf(out + used, size - used, "%s%s", used ? " " : "", members[i]);
                }
            }
            return out;
        };
        // Every member closed by the game's own close by name.
        auto close_all = [&]() {
            pause_game();
            for (int i = 0; i < count; ++i) {
                if (live_menu(row_named(members[i]))) {
                    char key[17];
                    snprintf(key, sizeof(key), "menu    %-8s", members[i]);
                    close_by_name(g_fake->mcb, NULL, key);
                }
            }
            resume_game();
            wait_frames(3);
            phase(L, "h:poll()\nreturn finish()\n");
        };
        // The script that checks the handle's events, joined as
        // event:name[:verb:reason], are `want`.
        auto events_are = [&](const char* want, const char* what) {
            char script[1400];
            snprintf(script, sizeof(script),
                "local seen = {}\n"
                "for _, e in ipairs((h:poll())) do seen[#seen + 1] = e.event .. ':' .. tostring(e.name)"
                "  .. (e.event == 'error' and (':' .. e.verb .. ':' .. e.reason) or '') end\n"
                "check(table.concat(seen, ' ') == '%s', '%s: ' .. table.concat(seen, ' '))\n"
                "return finish()\n", want, what);
            phase(L, script);
        };
        auto run_lua = [&](const char* line) {
            char script[512];
            snprintf(script, sizeof(script), "h:poll()\n%s\nreturn finish()\n", line);
            phase(L, script);
            wait_frames(3);
        };
        FakeSets* fs = g_fake->sets;
        char now[256];
        char label[512];
        close_all();
        memset(const_cast<LONG*>(fs->calls), 0, sizeof(fs->calls));

        // the map
        run_lua("check(h:open_set('map') == true, 'open_set(map) queues')");
        snprintf(label, sizeof(label), "open_set(map): the map's opener, mode 0, its routine on mapframe's controller"
            " with the mode as its third argument: [%s]", opened(now, sizeof(now)));
        check(strcmp(now, "mapframe map0") == 0 && fs->calls[kSetsMap] == 1 && fs->args[0] == 0 && fs->args[1] == 2,
            label);
        events_are("opened:mapframe opened:map0", "each its own opened event, in the game\\'s order");
        close_all();
        fs->refuse = 1;
        run_lua("check(h:open_set('map') == true, 'open_set(map) queues')");
        fs->refuse = 0;
        events_are("error:map:open_set:the game refused it", "the map\\'s routine refusing");
        check(opened(now, sizeof(now))[0] == '\0', "the map refused: nothing open");

        // the blue magic set and the automaton's
        uint8_t* pctl = controller_named("persona");
        pctl[0x1C] = 0;
        run_lua("check(h:open_set('set_spells') == true, 'open_set(set_spells) queues')");
        snprintf(label, sizeof(label), "open_set(set_spells): the opener on bluequip's controller past its job test:"
            " persona in mode 2, bluehelp and bluinven by their openers, bluepoin, bluequip: [%s]",
            opened(now, sizeof(now)));
        check(strcmp(now, "persona bluehelp bluinven bluepoin bluequip") == 0 && pctl[0x1C] == 2, label);
        events_are("opened:persona opened:bluehelp opened:bluinven opened:bluepoin opened:bluequip",
            "the blue magic set, each its own opened event");
        close_all();
        uint8_t* actl = controller_named("automato");
        actl[0x1C] = 0xFF;
        run_lua("check(h:open_set('attachments') == true, 'open_set(attachments) queues')");
        snprintf(label, sizeof(label), "open_set(attachments): the opener on automato's controller, a puppetmaster by"
            " the sub job (+0x1C 1): bluehelp, bluinven, statpup, automato: [%s]", opened(now, sizeof(now)));
        check(strcmp(now, "bluehelp bluinven statpup automato") == 0 && actl[0x1C] == 1, label);
        events_are("opened:bluehelp opened:bluinven opened:statpup opened:automato",
            "the automaton\\'s set, each its own opened event");
        close_all();
        *g_fake->main_job = 1;
        *g_fake->sub_job = 2;
        run_lua("check(h:open_set('set_spells') == true and h:open_set('attachments') == true, 'both queue')");
        *g_fake->main_job = 0x10;
        *g_fake->sub_job = 0x12;
        events_are("error:set_spells:open_set:the game refused it error:attachments:open_set:the game refused it",
            "neither a blue mage nor a puppetmaster: each opener\\'s refusal");
        check(opened(now, sizeof(now))[0] == '\0', "both refused for the job: nothing open");

        // the linkshell list behind its row's check
        fs->linkshell_check = 1;
        run_lua("check(h:open_set('linkshell') == true, 'open_set(linkshell) queues')");
        const bool held = fs->calls[kSetsLinkshell] == 0 && fs->check_args[0] == 1 && fs->check_args[1] == 1;
        events_are("error:linkshell:open_set:the game refused it", "the linkshell row\\'s check refusing");
        fs->linkshell_check = 0;
        run_lua("check(h:open_set('linkshell') == true, 'open_set(linkshell) queues')");
        snprintf(label, sizeof(label), "open_set(linkshell): the row's check (1, 1) refusing, the opener unrun;"
            " passing, the opener on link5's controller with (0, 0, 0): link6, then link5 through its jump to the open"
            " routine: [%s]", opened(now, sizeof(now)));
        check(held && strcmp(now, "link5 link6") == 0 && fs->calls[kSetsLinkshell] == 1
                && fs->self == controller_named("link5") && fs->args[0] == 0 && fs->args[1] == 0 && fs->args[2] == 0,
            label);
        events_are("opened:link6 opened:link5", "link6, then link5");
        // link5 as the plain list asks nothing, and close() closes it as its
        // Escape does; while the event's choice is pending, the game thread
        // refuses the close and points at cancel()
        uint8_t* lctl = controller_named("link5");
        put32(lctl, 0x68, 0);
        put32(lctl, 0x6C, 0);
        const uint32_t close_frames = *g_fake->close_frames;
        *g_fake->close_frames = 1;
        run_lua("check(h:close('link5') == true, 'close(link5) queues')");
        wait_frames(2);
        *g_fake->close_frames = close_frames;
        snprintf(label, sizeof(label), "close(link5) showing the plain list: closed, no refusal: [%s]",
            opened(now, sizeof(now)));
        check(!live_menu(row_named("link5")), label);
        events_are("closed:link5", "link5 closed through its close, no error");
        open_on_game_thread("menu    link5   ");
        wait_frames(2);
        phase(L, "h:poll()\nreturn finish()\n");
        *g_fake->link5_latch = 1;
        put32(lctl, 0x68, 1);
        put32(lctl, 0x6C, reinterpret_cast<uint32_t>(g_fake->link5_callback));
        run_lua("check(h:close('link5') == true, 'close(link5) queues')");
        const bool kept = live_menu(row_named("link5")) != NULL;
        *g_fake->link5_latch = 0;
        put32(lctl, 0x68, 0);
        put32(lctl, 0x6C, 0);
        check(kept, "close(link5) with the event's choice pending: link5 left open");
        events_are("error:link5:close:link5 is a prompt the game waits on; use cancel(\\'link5\\')",
            "the game thread refusing the close of a pending link5");
        close_all();

        // the instincts, in Monstrosity alone
        put16(g_fake->mon_player, 0x10, 0);
        *g_fake->game_mode = 0;
        run_lua("check(h:open_set('instincts') == true, 'open_set(instincts) queues')");
        events_are("error:instincts:open_set:the game refused it: it opens instincts only in Monstrosity",
            "outside Monstrosity");
        put16(g_fake->mon_player, 0x10, 1);
        run_lua("check(h:open_set('instincts') == true, 'open_set(instincts) queues')");
        put16(g_fake->mon_player, 0x10, 0);
        snprintf(label, sizeof(label), "open_set(instincts): outside Monstrosity the opener unrun; in it the opener:"
            " mp_stat's and mp_cstm's openers, mp_mncst: [%s]", opened(now, sizeof(now)));
        check(strcmp(now, "mp_stat mp_cstm mp_mncst") == 0 && fs->calls[kSetsInstincts] == 1, label);
        events_are("opened:mp_stat opened:mp_cstm opened:mp_mncst", "each its own opened event");
        close_all();

        // synthesis and the maze editor, by the item at an inventory index:
        // 5 a crystal, 6 an M tabula, 7 an R tabula, 8 a plain item, 9 an
        // item with no data, 10 empty
        uint8_t* inventory = g_fake->player + 0x9860;
        uint8_t saved[6 * 0x2C];
        memcpy(saved, inventory + 5 * 0x2C, sizeof(saved));
        memset(inventory + 5 * 0x2C, 0, sizeof(saved));
        const uint16_t ids[5] = {4096, 28672, 28704, 4509, 9999};
        for (int i = 0; i < 5; ++i) {
            put16(inventory + (5 + i) * 0x2C, 0, ids[i]);
            inventory[(5 + i) * 0x2C + 2] = static_cast<uint8_t>(5 + i);
            put32(inventory + (5 + i) * 0x2C, 4, 1);
        }
        fs->use_check = 1;
        // the main menu's Items row first, which opens inventor with the
        // row's list; a list that is none of the game's (its table's first
        // routine outside the code) refuses the synthesis, whose cancel would
        // reopen inventor with it
        uint8_t* ictl = controller_named("inventor");
        uint32_t list_was;
        memcpy(&list_was, ictl + 0x5C, 4);
        put32(ictl, 0x5C, 0xBAADF00Du);
        static const void* bad_table[1];
        static const void* bad_list[1];
        bad_list[0] = bad_table;
        uint8_t* policy_was = *g_fake->policy_ptr;
        *g_fake->policy_ptr = reinterpret_cast<uint8_t*>(bad_list);
        g_fake->routes->order[0] = '\0';
        run_lua("check(h:open_set('synthesis', '5') == true, 'open_set(synthesis, 5) queues')");
        events_are("opened:inventor opened:iteminfo error:synthesis:open_set:the game\\'s Items row left inventor with"
            " no list of the game\\'s, which the synthesis\\'s cancel would reopen it with closed:inventor"
            " closed:iteminfo",
            "the Items row\\'s list none of the game\\'s: what the row opened closed again by the game\\'s close");
        check(fs->calls[kSetsSynthesis] == 0 && strcmp(g_fake->routes->order, "items_view inventor_open") == 0
                && !live_menu(row_named("inventor")) && !live_menu(row_named("iteminfo")),
            "synthesis refused when the Items row leaves inventor no list of the game's: the opener unrun, inventor"
            " and iteminfo closed again");
        close_all();
        *g_fake->policy_ptr = policy_was;
        put32(ictl, 0x5C, 0xBAADF00Du);
        ictl[0x90] = 0;
        g_fake->routes->order[0] = '\0';
        run_lua("check(h:open_set('synthesis', '5') == true, 'open_set(synthesis, 5) queues')");
        snprintf(label, sizeof(label), "open_set(synthesis, 5), a crystal: the Items row (the item view (0, 1),"
            " inventor's +0x90 set 1, inventor opened with the row's list), the item use's check (0x77, 0), then the"
            " synthesis's opener on tskill1's controller with (5, 4096, 0): inventor, iteminfo, tskill1: [%s]",
            opened(now, sizeof(now)));
        uint32_t list_now;
        memcpy(&list_now, ictl + 0x5C, 4);
        check(strcmp(now, "inventor iteminfo tskill1") == 0 && fs->calls[kSetsSynthesis] == 1
                && fs->self == controller_named("tskill1") && fs->args[0] == 5 && fs->args[1] == 4096
                && fs->args[2] == 0 && fs->check_args[0] == 0x77 && fs->check_args[1] == 0
                && strcmp(g_fake->routes->order, "items_view inventor_open") == 0 && ictl[0x90] == 1
                && list_now == static_cast<uint32_t>(reinterpret_cast<uintptr_t>(policy_was)), label);
        events_are("opened:inventor opened:iteminfo opened:tskill1",
            "the Items row\\'s inventor and iteminfo, none for the opener\\'s open of them again, then tskill1");
        close_all();
        run_lua("for _, i in ipairs({'6', '8', '9', '10'}) do\n"
            "  check(h:open_set('synthesis', i) == true, 'open_set(synthesis, ' .. i .. ') queues')\n"
            "end");
        events_are("error:synthesis:open_set:the item at index 6 is not a crystal"
            " error:synthesis:open_set:the item at index 8 is not a crystal"
            " error:synthesis:open_set:the game has no data for the item at index 9"
            " error:synthesis:open_set:the inventory has no item at index 10",
            "a tabula, a plain item, an item with no data and an empty index refused");
        fs->use_check = 0;
        run_lua("check(h:open_set('synthesis', '5') == true, 'open_set(synthesis, \\'5\\') queues')");
        fs->use_check = 1;
        events_are("opened:inventor opened:iteminfo error:synthesis:open_set:the game refused it",
            "the item use\\'s check refusing after the Items row");
        check(fs->calls[kSetsSynthesis] == 1 && strcmp(opened(now, sizeof(now)), "inventor iteminfo") == 0,
            "synthesis refused by the item, and by the item use's check: the opener unrun, the Items row's inventor"
            " and iteminfo open");
        close_all();
        phase(L,
            "h:poll()\n"
            "local ok, why = h:open_set('synthesis')\n"
            "check(ok == nil and why == \"the synthesis set opens for one crystal: give its index in the inventory, as"
            " in open_set('synthesis', 5)\", 'open_set(synthesis) with no index: ' .. tostring(why))\n"
            "ok, why = h:open_set('synthesis', '81')\n"
            "check(ok == nil and why == \"no such inventory index: 81 (the crystal's index in the inventory, 1 to"
            " 80)\", 'open_set(synthesis, 81): ' .. tostring(why))\n"
            "ok, why = h:open_set('tabula', 'x')\n"
            "check(ok == nil and why == \"no such inventory index: x (the Maze Tabula's index in the inventory, 1 to"
            " 80)\", 'open_set(tabula, x): ' .. tostring(why))\n"
            "ok, why = h:open_set('map', '5')\n"
            "check(ok == nil and why == 'the map set takes no argument', 'open_set(map, 5): ' .. tostring(why))\n"
            "local raised, err = pcall(h.open_set, h, 'synthesis', 5)\n"
            "check(not raised and tostring(err):find('string expected', 1, true),"
            "  'the binding takes the index as its digits, as hideui.lua hands it on: ' .. tostring(err))\n"
            "check(#h:poll() == 0, 'no refusal reached the game thread')\n"
            "return finish()\n");
        run_lua("check(h:open_set('tabula', '6') == true, 'open_set(tabula, 6) queues')");
        snprintf(label, sizeof(label), "open_set(tabula, 6), an M tabula: the maze editor's opener on prvdungp's"
            " controller with (0, 6), its setup: prvdungp, bluehelp, bluinven: [%s]", opened(now, sizeof(now)));
        check(strcmp(now, "bluehelp bluinven prvdungp") == 0 && fs->calls[kSetsTabula] == 1
                && fs->self == controller_named("prvdungp") && fs->args[0] == 0 && fs->args[1] == 6, label);
        events_are("opened:prvdungp opened:bluehelp opened:bluinven", "each its own opened event");
        close_all();
        run_lua("check(h:open_set('tabula', '7') == true and h:open_set('tabula', '5') == true, 'both queue')");
        const bool r_mode = fs->calls[kSetsTabula] == 2 && fs->args[0] == 1 && fs->args[1] == 7;
        events_are("opened:prvdungp opened:bluehelp opened:bluinven"
            " error:tabula:open_set:the item at index 5 is not a Maze Tabula", "an R tabula, then a crystal");
        check(r_mode && fs->calls[kSetsTabula] == 2, "an R tabula opens the editor in mode 1; a crystal is refused,"
            " the opener unrun");
        close_all();
        memcpy(inventory + 5 * 0x2C, saved, sizeof(saved));
        memcpy(ictl + 0x5C, &list_was, 4);

        // the macro editor on its object, the palette last used last
        g_fake->macro_editor[0x0C] = 1;
        run_lua("check(h:open_set('macros') == true, 'open_set(macros) queues')");
        snprintf(label, sizeof(label), "open_set(macros): the macro editor's opener on its object: mcrbedit, mcr2edit"
            " and mcr1edit: [%s]", opened(now, sizeof(now)));
        check(strcmp(now, "mcrbedit mcr1edit mcr2edit") == 0, label);
        events_are("opened:mcrbedit opened:mcr2edit opened:mcr1edit", "palette 1 last used: mcr1edit last");
        close_all();
        *g_fake->macro_editor_ptr = NULL;
        run_lua("check(h:open_set('macros') == true, 'open_set(macros) queues')");
        *g_fake->macro_editor_ptr = g_fake->macro_editor;
        events_are("error:macros:open_set:the game\\'s macro editor does not exist yet", "no editor object yet");

        // a block on a member refuses up front
        phase(L,
            "h:poll()\n"
            "local other = hu.new('blocker')\n"
            "check(other:block('statpup') == true and other:block('mapframe') == true, 'another handle blocks')\n"
            "local ok, why = h:open_set('attachments')\n"
            "local ok2, why2 = h:open_set('map')\n"
            "check(ok == nil and why == 'statpup is blocked; the attachments set opens it' and ok2 == nil"
            "  and why2 == 'mapframe is blocked; the map set opens it', 'blocked members: ' .. tostring(why) .. ', '"
            "  .. tostring(why2))\n"
            "check(other:release() == true, 'the blocker releases')\n"
            "return finish()\n");

        // the open routine's last argument: 0 for titlehan, which shows at
        // once over titlewin; 1 for targetwi, which parks under ut_point
        close_all();
        run_lua("check(h:open('titlewin') == true and h:open('titlehan') == true, 'titlewin and titlehan open')");
        const uint8_t* han = live_menu(row_named("titlehan"));
        uint32_t han_state = 0xFF;
        if (han) {
            memcpy(&han_state, han + 0x10, 4);
        }
        snprintf(label, sizeof(label), "open(titlehan) over titlewin, its overlap group's: the open routine's last"
            " argument 0 (%d), and titlehan shows at once (state 0x%X)", *g_fake->last_overlap,
            static_cast<unsigned>(han_state));
        check(*g_fake->last_overlap == 0 && han && han_state != 0x0E && han[0x75] == 0, label);
        events_are("opened:titlewin opened:titlehan", "both opened, nothing covered");
        run_lua("check(h:open('ut_point') == true and h:open('targetwi') == true, 'ut_point and targetwi open')");
        const uint8_t* target = live_menu(row_named("targetwi"));
        uint32_t target_state = 0;
        if (target) {
            memcpy(&target_state, target + 0x10, 4);
        }
        snprintf(label, sizeof(label), "open(targetwi) under ut_point: the open routine's last argument 1 (%d), as"
            " the game's own openers pass it, and targetwi opens dormant under ut_point (state 0x%X)",
            *g_fake->last_overlap, static_cast<unsigned>(target_state));
        check(*g_fake->last_overlap == 1 && target && target_state == 0x0E && target[0x75] == 1, label);
        phase(L,
            "local seen = {}\n"
            "for _, e in ipairs((h:poll())) do seen[#seen + 1] = e.event .. ':' .. tostring(e.name) end\n"
            "check(table.concat(seen, ' ') == 'opened:ut_point opened:targetwi' and h:info('targetwi').covered,"
            "  'targetwi opened, and covered as info reads it: ' .. table.concat(seen, ' '))\n"
            "return finish()\n");
        close_all();
    }

    // -- the compass (0.8.0): the stand-in's object and draw entry, hooked;
    //    read, hidden, moved and reset through Lua, its events as the state
    //    byte is poked, and a release putting it back
    {
        uint8_t* compass = g_fake->compass;
        const FakeLog* fl = g_fake->log;
        phase(L,
            "local all = h:list()\n"
            "local c = all.compass or {}\n"
            "check(c.name == 'compass' and c.open == true and c.blockable == true and c.hidden == false"
            "  and c.blocked == false and c.moved == false and c.resized == false and c.layer == nil and c.detail"
            "  and c.detail.policy == 0 and c.detail.dock == nil,"
            "  'list(): compass open, blockable, no hold, no layer or dock group')\n"
            "local i = h:info('compass')\n"
            "check(i.name == 'compass' and i.open and i.blockable == true and i.hidden == false and i.blocked == false"
            "  and #i.hidden_by == 0 and #i.blocked_by == 0 and i.mine.hidden == false and i.mine.blocked == false"
            "  and i.docked == false and i.covered == false and i.focused == false and i.layer == nil"
            "  and i.resize.holds == 'none' and i.resize.min_rows == nil and i.memory == nil,"
            "  'info(compass): open, no hold, no layer, nothing to resize')\n"
            "check(i.rect.x == 16 and i.rect.y == 1016 and i.rect.w == 88 and i.rect.h == 42 and i.rect.right == 104"
            "  and i.rect.bottom == 1058 and i.origin.x == 104 and i.origin.y == 1058 and i.default.x == 16"
            "  and i.default.y == 1016 and i.default.w == 88 and i.default.h == 42 and i.cursor == nil"
            "  and i.elements == nil and i.items == nil,"
            "  'info(compass): its box {16, 1016, 88, 42} from its anchor 104,1058, the default the same box, no'"
            "  .. ' cursor, items or elements')\n"
            "check(i.detail.state == 3 and i.detail.x == 104 and i.detail.y == 1058 and i.detail.height == 42"
            "  and i.detail.policy == 0 and #i.detail.rows == 0 and i.detail.address and i.detail.layer == nil"
            "  and i.detail.controller == nil,"
            "  'info(compass).detail: the object: state 3, anchor, height; policy 0, no rows')\n"
            "local r = h:rects().compass\n"
            "local set = {}\n"
            "for _, n in ipairs(h:opened()) do set[n] = true end\n"
            "check(r and r.x == 16 and r.y == 1016 and r.w == 88 and r.h == 42 and set.compass,"
            "  'rects() has the compass box; opened() names it')\n"
            "check(h:focused() == false, 'focused() never names the compass')\n"
            "local a, awhy = h:answer('compass', 1)\n"
            "local cn, cwhy = h:cancel('compass')\n"
            "local o, owhy = h:options('compass')\n"
            "check(a == nil and awhy:find('takes no answer', 1, true) and cn == nil and cwhy:find('nothing to cancel', 1, true)"
            "  and o == nil and owhy:find('has no options', 1, true),"
            "  'answer, cancel and options of the compass refuse as for any window that is no prompt')\n"
            "check(h:hide('compass') == true, 'hide(compass) queues')\n"
            "return finish()\n");
        wait_frames(3);
        const LONG draws = *g_fake->compass_draws;
        const LONG updates = *g_fake->compass_updates;
        wait_frames(3);
        check(*g_fake->compass_draws == draws && *g_fake->compass_updates == updates && compass[0x0D] == 3
                && static_cast<uint8_t*>(g_fake->compass_draw)[0] == 0xE9,
            "hidden: over three frames the compass draw entry's pre runs instead of it, no render and no update; the"
            " state byte stands");
        phase(L,
            "local i = h:info('compass')\n"
            "check(i.hidden and #i.hidden_by == 1 and i.hidden_by[1] == 'alpha' and i.mine.hidden and i.open"
            "  and h:list().compass.hidden, 'info(compass) while hidden: hidden by alpha, still open')\n"
            "check(h:unhide('compass') == true, 'unhide(compass) queues')\n"
            "return finish()\n");
        wait_frames(3);
        const LONG resumed = *g_fake->compass_draws;
        wait_frames(3);
        check(resumed > draws && *g_fake->compass_draws > resumed && *g_fake->compass_updates > updates,
            "unhidden: the draws and updates resume");
        // A block holds the compass as a hide does: no window closes, no
        // event posts; close and open are the hide hold set and dropped;
        // resize is accepted and changes nothing.
        phase(L,
            "h:poll()\n"
            "check(h:block('compass') == true, 'block(compass) holds')\n"
            "return finish()\n");
        wait_frames(3);
        const LONG blocked = *g_fake->compass_draws;
        const LONG blocked_updates = *g_fake->compass_updates;
        wait_frames(3);
        check(*g_fake->compass_draws == blocked && *g_fake->compass_updates == blocked_updates && compass[0x0D] == 3,
            "blocked: over three frames no render and no update, as under a hide; the state byte stands");
        phase(L,
            "local i = h:info('compass')\n"
            "local c = h:list().compass\n"
            "check(i.blocked and i.blockable == true and i.hidden and i.open and #i.blocked_by == 1"
            "  and i.blocked_by[1] == 'alpha' and #i.hidden_by == 0 and i.mine.blocked and not i.mine.hidden"
            "  and c.blocked and c.blockable and c.hidden,"
            "  'info(compass) while blocked: blocked by alpha, hidden by the block, blockable, still open')\n"
            "check(#(h:poll()) == 0, 'no event for the block: the game never attempts an open of the compass')\n"
            "check(h:unblock('compass') == true, 'unblock(compass) drops the hold')\n"
            "return finish()\n");
        wait_frames(3);
        const LONG unblocked = *g_fake->compass_draws;
        wait_frames(3);
        check(unblocked > blocked && *g_fake->compass_draws > unblocked && *g_fake->compass_updates > blocked_updates,
            "unblocked: the draws and updates resume");
        phase(L,
            "local i = h:info('compass')\n"
            "check(not i.blocked and not i.hidden and #i.blocked_by == 0 and not i.mine.blocked"
            "  and not h:list().compass.blocked, 'info(compass) after the unblock: no hold')\n"
            "check(h:close('compass') == true, 'close(compass) is a hide')\n"
            "return finish()\n");
        wait_frames(3);
        const LONG closed = *g_fake->compass_draws;
        wait_frames(3);
        check(*g_fake->compass_draws == closed && compass[0x0D] == 3,
            "closed: no render over three frames, the state byte stands: nothing the game closed");
        phase(L,
            "local i = h:info('compass')\n"
            "check(i.hidden and i.open and #i.hidden_by == 1 and i.hidden_by[1] == 'alpha' and i.mine.hidden"
            "  and not i.blocked, 'info(compass) after close: hidden by alpha, still open, not blocked')\n"
            "check(#(h:poll()) == 0, 'no event for the close')\n"
            "check(h:open('compass') == true, 'open(compass) is an unhide')\n"
            "return finish()\n");
        wait_frames(3);
        const LONG opened = *g_fake->compass_draws;
        wait_frames(3);
        check(opened > closed && *g_fake->compass_draws > opened, "opened: the draws resume");
        phase(L,
            "check(not h:info('compass').hidden and #(h:poll()) == 0,"
            "  'info(compass) after open: no hold, no event')\n"
            "check(h:resize('compass', 3) == true and h:resize('compass', 10, 20) == true,"
            "  'resize(compass, 3) and resize(compass, 10, 20) are accepted')\n"
            "local raised = not pcall(h.resize, h, 'compass', 'three')\n"
            "local z, zwhy = h:resize('compass', 0)\n"
            "local f, fwhy = h:resize('compass', 1.5)\n"
            "local r, rwhy = h:resize('compass', 0, 20)\n"
            "check(raised and z == nil and zwhy == 'resize compass: it has no template family; resize(name, w, h) sets'"
            "  .. ' any size' and f == nil and fwhy == 'usage: resize(name, rows) or resize(name, w, h) with whole numbers'"
            "  and r == nil and rwhy == 'resize compass: width and height are 1..32767',"
            "  'the usage errors every name gets stay: a string raises, zero rows, a fraction and a zero width refuse: '"
            "  .. tostring(zwhy) .. ' | ' .. tostring(fwhy) .. ' | ' .. tostring(rwhy))\n"
            "return finish()\n");
        wait_frames(3);
        check(fl->compass_x == 104 && fl->compass_y == 1058 && get16(compass, 0x28) == 104
                && get16(compass, 0x2A) == 1058 && get16(compass, 0x2C) == 42,
            "resized: nothing changed, the anchor and the height as the game had them");
        phase(L,
            "local i = h:info('compass')\n"
            "check(i.resize.holds == 'none' and i.resize.min_rows == nil and i.memory == nil"
            "  and h:list().compass.resized == false and h:remembered().compass == nil and i.rect.w == 88"
            "  and i.rect.h == 42, 'info(compass) after resize: nothing held, nothing remembered, the box as it was')\n"
            "return finish()\n");
        phase(L,
            "check(h:move('compass', 10, 20) == true, 'move(compass, 10, 20) queues')\n"
            "return finish()\n");
        wait_frames(3);
        check(fl->compass_x == 98 && fl->compass_y == 62 && get16(compass, 0x28) == 98 && get16(compass, 0x2A) == 62,
            "moved: the render sees the anchor 98,62, the box's top-left at 10,20");
        put16(compass, 0x28, 104);
        put16(compass, 0x2A, 1058);
        wait_frames(3);
        check(fl->compass_x == 98 && fl->compass_y == 62 && get16(compass, 0x28) == 98,
            "the game writes its own anchor back (a chat log edge change): the next draw writes the move's again");
        phase(L,
            "local i = h:info('compass')\n"
            "local m = i.memory and i.memory.position or {}\n"
            "check(i.rect.x == 10 and i.rect.y == 20 and i.rect.w == 88 and i.rect.h == 42 and i.origin.x == 98"
            "  and i.origin.y == 62 and i.default.x == 16 and i.default.y == 1016 and m.x == 10 and m.y == 20"
            "  and m.mine and m.owner == 'alpha' and m.group == nil,"
            "  'info(compass) moved: the box at 10,20, the anchor 98,62, the default the box the game had, the'"
            "  .. ' position remembered by alpha')\n"
            "local rem = h:remembered().compass\n"
            "check(rem and rem.position and rem.position.x == 10 and rem.position.y == 20 and rem.size == nil"
            "  and h:list().compass.moved == true and h:layout().positions.compass.x == 10,"
            "  'remembered() lists the compass while it is moved, and layout() carries it')\n"
            "check(h:reset('compass') == true, 'reset(compass) queues')\n"
            "return finish()\n");
        wait_frames(3);
        check(fl->compass_x == 104 && fl->compass_y == 1058 && get16(compass, 0x28) == 104 && get16(compass, 0x2A) == 1058,
            "reset: the anchor back at 104,1058 and the render draws there");
        phase(L,
            "check(h:remembered().compass == nil and h:list().compass.moved == false and h:info('compass').memory == nil,"
            "  'reset: nothing remembered for the compass')\n"
            "h:poll()\n"
            "return finish()\n");
        poke(compass + 0x0D, 0);
        wait_frames(2);
        phase(L,
            "local seen = {}\n"
            "for _, e in ipairs((h:poll())) do seen[#seen + 1] = e.event .. ':' .. e.name end\n"
            "check(table.concat(seen, ' ') == 'closed:compass' and h:info('compass').open == false"
            "  and h:list().compass.open == false and h:rects().compass == nil and h:info('compass').rect == nil"
            "  and h:info('compass').detail.state == 0,"
            "  'the state byte poked to 0: closed{compass}; info, list and rects say closed: ' .. table.concat(seen, ' '))\n"
            "return finish()\n");
        poke(compass + 0x0D, 3);
        wait_frames(2);
        phase(L,
            "local seen = {}\n"
            "for _, e in ipairs((h:poll())) do seen[#seen + 1] = e.event .. ':' .. e.name end\n"
            "check(table.concat(seen, ' ') == 'opened:compass' and h:info('compass').open == true,"
            "  'the state byte back to 3: opened{compass}: ' .. table.concat(seen, ' '))\n"
            "hc = hu.new('compass_owner')\n"
            "check(hc:hide('compass') == true and hc:move('compass', 30, 40) == true, 'another handle hides and moves it')\n"
            "return finish()\n");
        wait_frames(3);
        const LONG held = *g_fake->compass_draws;
        wait_frames(3);
        const bool held_still = *g_fake->compass_draws == held;
        phase(L,
            "check(h:info('compass').hidden and h:info('compass').hidden_by[1] == 'compass_owner'"
            "  and h:remembered().compass.position.owner == 'compass_owner' and hc:release() == true,"
            "  'held by compass_owner, which releases')\n"
            "return finish()\n");
        wait_frames(3);
        const LONG released = *g_fake->compass_draws;
        wait_frames(3);
        check(held_still && *g_fake->compass_draws > released && fl->compass_x == 104 && fl->compass_y == 1058
                && get16(compass, 0x28) == 104 && get16(compass, 0x2A) == 1058,
            "the release unhides the compass and resets it: drawn again at the game's anchor");
        phase(L,
            "check(not h:info('compass').hidden and h:remembered().compass == nil, 'nothing of compass_owner remains')\n"
            "h:poll()\n"
            "return finish()\n");
    }

    // -- open(name, argument) and the sets past macros (0.11.0): the argument
    //    as hideui.lua hands it on (a number as its digits, a table as
    //    key=value pairs), refused up front for a value outside the window's
    //    range; each route as the game's own row, command or list runs it,
    //    through the stand-in's routines and sites (the game's bytes where
    //    the engine runs them), the window its opener brings along held off
    {
        static const char* const members[] = {
            "storage", "storage2", "mogpost", "jobcselu", "friend", "titlehan", "msglist", "blklist", "mount",
            "abisortw", "subwindo", "jbpcat", "fp_cat2", "conf11l", "quest00", "quest01", "miss00", "scresult",
            "searchjo", "comgenre", "sccompar", "scsibori", "inventor", "iteminfo", "itemxinf", "itemyinf",
            "casttime", "eventtim", "persona", "mp_stat", "macro", "mcrledit", "map0", "scanlist", "cnqframe",
            "colopoin", "coloresu", "colorank", "textcol1", "textcol2", "mcrbedit", "mcr1edit", "mcr2edit", "level",
            "statcom2", "mp_stat", "mp_cstm", "mp_mncst", "comgenre", "sccompar"};
        const int count = static_cast<int>(sizeof(members) / sizeof(members[0]));
        typedef int (__fastcall* CloseFn)(void* mcb, void* edx, const char* key);
        CloseFn close_by_name;
        memcpy(&close_by_name, &g_fake->close, sizeof(close_by_name));
        auto opened = [&](char* out, size_t size) -> const char* {
            out[0] = '\0';
            for (int i = 0; i < count; ++i) {
                const uint8_t* menu = live_menu(row_named(members[i]));
                if (menu && !menu[0x6E] && !menu[0x70]) {
                    const size_t used = strlen(out);
                    snprintf(out + used, size - used, "%s%s", used ? " " : "", members[i]);
                }
            }
            return out;
        };
        FakeRoutes* fr = g_fake->routes;
        auto close_all = [&]() {
            pause_game();
            for (int i = 0; i < count; ++i) {
                if (live_menu(row_named(members[i]))) {
                    char key[17];
                    snprintf(key, sizeof(key), "menu    %-8s", members[i]);
                    close_by_name(g_fake->mcb, NULL, key);
                }
            }
            resume_game();
            wait_frames(3);
            phase(L, "h:poll()\nreturn finish()\n");
            fr->order[0] = '\0';
        };
        auto events_are = [&](const char* want, const char* what) {
            char script[1400];
            snprintf(script, sizeof(script),
                "local seen = {}\n"
                "for _, e in ipairs((h:poll())) do seen[#seen + 1] = e.event .. ':' .. tostring(e.name)"
                "  .. (e.event == 'error' and (':' .. e.verb .. ':' .. e.reason) or '') end\n"
                "check(table.concat(seen, ' ') == '%s', '%s: ' .. table.concat(seen, ' '))\n"
                "return finish()\n", want, what);
            phase(L, script);
        };
        auto run_lua = [&](const char* line) {
            char script[600];
            snprintf(script, sizeof(script), "h:poll()\n%s\nreturn finish()\n", line);
            phase(L, script);
            wait_frames(3);
        };
        auto open_now = [&](const char* nm) {
            char key[17];
            snprintf(key, sizeof(key), "menu    %-8s", nm);
            open_on_game_thread(key);
            wait_frames(2);
            phase(L, "h:poll()\nreturn finish()\n");
        };
        char now[512];
        char label[640];
        close_all();

        // refused on the Lua thread, nothing queued
        phase(L,
            "h:poll()\n"
            "local cases = {\n"
            "  {'open', 'jobcselu', nil, \"jobcselu needs a job to change: open('jobcselu', 'main')\"},\n"
            "  {'open', 'mogpost', 'post', \"no such mode of mogpost: post ('mog_house' or 'nomad')\"},\n"
            "  {'open', 'jbpcat', '23', 'no such job of jbpcat: 23'},\n"
            "  {'open', 'iteminfo', 'bag=wallet;slot=1', 'no such bag for iteminfo: wallet'},\n"
            "  {'open', 'iteminfo', 'item=65535', 'no such item id for iteminfo: 65535'},\n"
            "  {'open', 'macro', 'palette=alt;row=11', 'no such row of macro: 11 (1 to 10)'},\n"
            "  {'open', 'casttime', '0', 'no such length of casttime: 0'},\n"
            "  {'open', 'persona', 'status', 'persona takes no argument'},\n"
            "  {'open', 'mp_stat', nil, \"mp_stat opens only with its set: use open_set('status')\"},\n"
            "  {'open', 'miss00', 'zilart', \"no such log of miss00: zilart ('San d'Oria', 'Bastok'\"},\n"
            "  {'open', 'quest01', nil, 'quest01 needs quest00 open'},\n"
            "  {'open', 'sccompar', 'purpose=comment;category=2', 'sccompar needs comgenre or scresult open'},\n"
            "  {'open', 'storage', 'bag=inventory;slot=1;item', 'storage: the argument is not one hideui reads'},\n"
            "  {'open_set', 'region', nil, \"the region set opens for one kind: 'Conquest', 'Besieged', 'Campaign' or"
            " 'Colonization'\"},\n"
            "  {'open_set', 'region', 'moon', 'no such kind of the region set: moon'},\n"
            "  {'open_set', 'text_colors', '5', 'no such category of the text_colors set: 5'},\n"
            "  {'open_set', 'compare', 'item=16555', 'itemxinf needs iteminfo open'},\n"
            "  {'open_set', 'friend', '1', 'the friend set takes no argument'},\n"
            "}\n"
            "for _, c in ipairs(cases) do\n"
            "  local ok, why = h[c[1]](h, c[2], c[3])\n"
            "  check(ok == nil and tostring(why):find(c[4], 1, true) == 1, c[1] .. '(' .. c[2] .. ', '"
            "    .. tostring(c[3]) .. ') refused: ' .. tostring(why))\n"
            "end\n"
            "check(#h:poll() == 0, 'no refusal reached the game thread')\n"
            "return finish()\n");
        {
            HuEngineApi api;
            HuEngineHandle raw = {-1, 0};
            char why[512] = "";
            if (record_api(&api)) {
                api.handle_open("raw_route", &raw, why, sizeof(why));
            }
            const bool old_route = raw.slot >= 0 && api.open(&raw, "storage", why, sizeof(why)) == 1;
            const bool old_bare = raw.slot >= 0 && api.open8(&raw, "casttime", why, sizeof(why)) == 0
                && strstr(why, "casttime needs a length") == why;
            const bool new_needs = raw.slot >= 0 && api.open9(&raw, "casttime", NULL, why, sizeof(why)) == 0
                && strstr(why, "casttime needs a length") == why;
            if (raw.slot >= 0) {
                api.handle_release(&raw);
            }
            wait_frames(3);
            check(old_route && old_bare && new_needs, "an old slot's plain open of a window with a default runs its"
                " route; one of a window that needs an argument is refused in every slot");
            close_all();
        }

        run_lua("check(h:open('storage') == true and h:open('storage2') == true and h:open('mogpost') == true,"
            " 'storage, storage2 and mogpost queue')");
        events_are("error:storage:open:the game refused it: the Mog House\\'s storage opens only in the Mog House"
            " error:storage2:open:the game refused it: the Nomad Moogle\\'s storage opens only where a Nomad Moogle"
            " serves error:mogpost:open:the game refused it: the Mog House\\'s post opens only in the Mog House",
            "the Mog House\\'s storage and post and the Nomad Moogle\\'s storage refused outside their areas");
        *g_fake->mog_house_ptr = g_fake->mog_house;
        run_lua("check(h:open('storage') == true and h:open('storage2') == true, 'storage and storage2 queue')");
        *g_fake->mog_house_ptr = NULL;
        events_are("opened:storage error:storage2:open:the game refused it: the Nomad Moogle\\'s storage opens only"
            " where a Nomad Moogle serves", "in the Mog House its storage, and the Nomad Moogle\\'s refused");
        *g_fake->nomad_ptr = g_fake->mog_house;
        run_lua("check(h:open('storage2') == true, 'storage2 queues')");
        *g_fake->nomad_ptr = NULL;
        snprintf(label, sizeof(label), "open(storage) in the Mog House and open(storage2) where a Nomad Moogle"
            " serves: the storage list's opener, the game's bytes, on each one's controller with 0 and 1: [%s]",
            opened(now, sizeof(now)));
        check(strcmp(now, "storage storage2") == 0 && get32(controller_named("storage"), 0x14) == 0
                && get32(controller_named("storage2"), 0x14) == 1, label);
        events_are("opened:storage2", "storage2 its own opened event");
        close_all();

        run_lua("check(h:open('mogpost', 'nomad') == true, 'mogpost, nomad queues')");
        events_are("error:mogpost:open:the game refused it: the Nomad Moogle\\'s post asks the server for the delivery"
            " box only in the Mog House or where a Nomad Moogle serves", "nomad refused with neither manager");
        *g_fake->nomad_ptr = g_fake->mog_house;
        run_lua("check(h:open('mogpost', 'nomad') == true, 'mogpost, nomad queues')");
        *g_fake->nomad_ptr = NULL;
        check(live_menu(row_named("mogpost")) && get32(controller_named("mogpost"), 0x14) == 1,
            "open(mogpost, nomad) where a Nomad Moogle serves: the post menu's opener, the game's bytes, with 1");
        events_are("opened:mogpost", "mogpost opened");
        close_all();

        *g_fake->mog_house_ptr = g_fake->mog_house;
        run_lua("check(h:open('jobcselu', 'support') == true, 'jobcselu, support queues')");
        *g_fake->mog_house_ptr = NULL;
        check(live_menu(row_named("jobcselu")) && get32(controller_named("jobcselu"), 0x14) == 2
                && strcmp(fr->order, "language_key") == 0 && fr->args[0] == 6,
            "open(jobcselu, support) in the Mog House: the row 2 at +0x14, then the open by the key the language's"
            " lookup (6) answers");
        events_are("opened:jobcselu", "jobcselu opened");
        close_all();

        run_lua("check(h:open('friend') == true, 'friend queues')");
        snprintf(label, sizeof(label), "open(friend): the friend list's opener with 0, titlehan it opens held off:"
            " [%s]", opened(now, sizeof(now)));
        check(strcmp(now, "friend") == 0 && strcmp(fr->order, "friend") == 0 && fr->args[0] == 0
                && fr->self == controller_named("friend"), label);
        events_are("opened:friend", "friend alone");
        close_all();
        run_lua("check(h:open_set('friend') == true, 'open_set(friend) queues')");
        events_are("opened:friend opened:titlehan", "open_set(friend): friend and titlehan");
        close_all();
        fr->msglist = 0;
        run_lua("check(h:open('msglist') == true, 'msglist queues')");
        fr->msglist = 1;
        events_are("error:msglist:open:the game refused it: PlayOnline\\'s mail is busy or not there",
            "the mail list\\'s opener refusing");
        run_lua("check(h:open('msglist') == true and h:open('blklist') == true, 'msglist and blklist queue')");
        snprintf(label, sizeof(label), "open(msglist): the mail list's opener with (0, 0, 0), titlehan held off;"
            " open(blklist): the blacklist's opener: [%s]", opened(now, sizeof(now)));
        check(strcmp(now, "msglist blklist") == 0, label);
        events_are("opened:msglist opened:blklist", "msglist alone, then blklist");
        close_all();
        run_lua("check(h:open_set('mail') == true, 'open_set(mail) queues')");
        events_are("opened:msglist opened:titlehan", "open_set(mail): msglist and titlehan");
        close_all();

        run_lua("check(h:open('mount') == true, 'mount queues')");
        snprintf(label, sizeof(label), "open(mount): the mount list's opener (the game's bytes) with (1, 0), its"
            " setup on mount's controller, subwindo held off: [%s]", opened(now, sizeof(now)));
        check(strcmp(now, "mount") == 0 && strcmp(fr->order, "mount_build") == 0
                && fr->self == controller_named("mount"), label);
        events_are("opened:mount", "mount alone");
        close_all();
        run_lua("check(h:open_set('mount') == true, 'open_set(mount) queues')");
        events_are("opened:abisortw opened:mount opened:subwindo", "open_set(mount) with a sort: abisortw, mount,"
            " subwindo");
        close_all();

        fr->job_level[6] = 90;
        run_lua("check(h:open('jbpcat', 'thf') == true, 'jbpcat, thf queues')");
        events_are("error:jbpcat:open:the game refused it: its job list offers thf only at level 99",
            "the Job Points test, the game\\'s bytes, refusing a job under 99");
        fr->job_level[6] = 99;
        run_lua("check(h:open('jbpcat', 'thf') == true and h:open('fp_cat2', '3') == true, 'jbpcat and fp_cat2"
            " queue')");
        check(controller_named("jbpcat")[0x54] == 6 && controller_named("fp_cat2")[0x50] == 3,
            "open(jbpcat, thf) at 99: its opener with 6; open(fp_cat2, 3) with 3, each the game's bytes");
        events_are("opened:jbpcat opened:fp_cat2", "jbpcat and fp_cat2 opened");
        close_all();

        run_lua("check(h:open('conf11l', '2') == true and h:open('quest00', 'Ranking Information') == true,"
            " 'conf11l and quest00 queue')");
        check(get32(controller_named("conf11l"), 0x14) == 1 && get32(controller_named("quest00"), 0x68) == 2,
            "open(conf11l, 2) writes 1 and open(quest00, 'Ranking Information') 2 where the game's rows write them");
        events_are("opened:conf11l opened:quest00", "conf11l and quest00 opened");
        fr->order[0] = '\0';
        run_lua("check(h:open('quest01', '2') == true, 'quest01 queues')");
        events_are("error:quest01:open:the game refused it: quest01\\'s list 2 opens only from quest00\\'s mode 1",
            "quest01\\'s list 2 refused from quest00\\'s mode 3");
        close_all();
        run_lua("check(h:open('quest00') == true, 'quest00 queues')");
        events_are("opened:quest00", "quest00 in its mode 1");
        fr->order[0] = '\0';
        run_lua("check(h:open('quest01', '2') == true, 'quest01 queues')");
        check(get32(controller_named("quest00"), 0x68) == 0 && strcmp(fr->order, "quest00_dim quest00_dim2 quest01") == 0
                && fr->args[0] == 1 && fr->self == controller_named("quest01") && live_menu(row_named("quest01")),
            "open(quest01, 2) from quest00's mode 1: quest00's confirm's two calls, quest01's open, its setup with 1");
        events_are("opened:quest01", "quest01 opened");
        close_all();

        *g_fake->nation = 1;
        run_lua("check(h:open('miss00') == true, 'miss00 queues')");
        const bool nation = get32(controller_named("miss00"), 0x70) == 1;
        events_are("opened:miss00", "miss00 opened on the nation\\'s log");
        fr->expansion = 0;
        run_lua("check(h:open('miss00', 'Chains of Promathia') == true, 'miss00, Chains of Promathia queues')");
        events_are("error:miss00:open:the game refused it: the Chains of Promathia log\\'s expansion is not on",
            "a log whose expansion is off");
        fr->expansion = 1u << 2;
        close_all();
        run_lua("check(h:open('miss00', 'chains of promathia') == true, 'miss00, chains of promathia queues')");
        check(nation && get32(controller_named("miss00"), 0x70) == 4, "open(miss00): the missions menu's setter, the"
            " game's bytes, with 0 for the nation's log; a named log behind its expansion bit, written");
        events_are("opened:miss00", "miss00 opened again on its log");
        close_all();

        uint8_t* results = controller_named("scresult");
        put32(results, 0x58, 0);
        run_lua("check(h:open('searchjo') == true, 'searchjo queues')");
        check(strcmp(fr->order, "search_reset") == 0 && fr->self == results && fr->args[0] == 0
                && live_menu(row_named("searchjo")),
            "open(searchjo): the search reset on scresult's controller with 0, then the picker");
        events_are("opened:searchjo", "searchjo opened");
        close_all();
        run_lua("check(h:open('searchjo', 'refine') == true, 'searchjo, refine queues')");
        events_are("error:searchjo:open:the game refused it: no search has run to refine", "refine with no search");
        put32(results, 0x58, 1);
        run_lua("check(h:open('comgenre', 'refine') == true, 'comgenre, refine queues')");
        check(get16(controller_named("comgenre"), 0x14) == 2 && fr->order[0] == '\0',
            "open(comgenre, refine) after a search: its opener, the game's bytes, with 2, no reset");
        events_are("opened:comgenre", "comgenre opened");
        run_lua("check(h:open('sccompar', 'purpose=comment;category=2') == true, 'sccompar queues')");
        check(get32(controller_named("sccompar"), 0x14) == 0 && get32(controller_named("sccompar"), 0x18) == 0x20,
            "open(sccompar, {purpose = comment, category = 2}) with comgenre open: its opener, the game's bytes, with"
            " 0 and the base 0x20");
        events_are("opened:sccompar", "sccompar opened");
        open_now("scresult");
        run_lua("check(h:open('scsibori') == true, 'scsibori queues')");
        events_are("opened:scsibori", "scsibori with scresult open and a search run");
        put32(results, 0x58, 0);
        close_all();

        uint8_t* inventory = g_fake->player + 0x9860;
        uint8_t kept[0x2C];
        memcpy(kept, inventory + 5 * 0x2C, sizeof(kept));
        memset(inventory + 5 * 0x2C, 0, sizeof(kept));
        put16(inventory + 5 * 0x2C, 0, 16555);
        inventory[5 * 0x2C + 2] = 5;
        put32(inventory + 5 * 0x2C, 4, 1);
        run_lua("check(h:open('iteminfo', 'bag=inventory;slot=5') == true, 'iteminfo queues')");
        check(strcmp(fr->order, "item_panel") == 0 && fr->self == controller_named("iteminfo") && fr->args[0] == 0
                && fr->args[1] == 16555 && fr->args[2] == 5 && fr->args[3] == 0
                && fr->args[4] == static_cast<int32_t>(reinterpret_cast<uintptr_t>(inventory + 5 * 0x2C + 0x10)),
            "open(iteminfo, {bag, slot}): the item panel's set-and-open with mode 0, the record's id, index, bag and"
            " extra data");
        events_are("opened:iteminfo", "iteminfo opened");
        fr->bag_size[0] = 5;
        run_lua("check(h:open('iteminfo', 'bag=inventory;slot=5') == true, 'iteminfo queues')");
        events_are("error:iteminfo:open:inventory has no slot 5: it has slots 1 to 4 now",
            "a slot past the bag\\'s size");
        fr->bag_size[0] = 0xFF;
        run_lua("check(h:open('iteminfo', 'bag=0;slot=1') == true, 'iteminfo queues')");
        fr->bag_size[0] = 81;
        events_are("error:iteminfo:open:inventory has no slots now", "a bag with none");
        run_lua("check(h:open('iteminfo', 'item=5021') == true and h:open('itemxinf', 'item=16555') == true,"
            " 'both queue')");
        snprintf(label, sizeof(label), "open(iteminfo, {item = 5021}) refused for an item of type 21; open(itemxinf,"
            " {item}) with mode 5 beside equip and inventor, itemyinf held off: [%s]", opened(now, sizeof(now)));
        check(strcmp(now, "iteminfo itemxinf") == 0 && fr->args[0] == 5 && fr->args[4] == 0
                && fr->args[5] == static_cast<int32_t>(reinterpret_cast<uintptr_t>(controller_named("equip")))
                && fr->args[6] == static_cast<int32_t>(reinterpret_cast<uintptr_t>(controller_named("inventor"))),
            label);
        events_are("error:iteminfo:open:item 5021 is of a kind the panel shows with its bag\\'s copy of it, and by its"
            " id alone it would crash the client: give its bag and slot opened:itemxinf", "the type refused, then"
            " itemxinf");
        run_lua("check(h:open_set('compare', 'bag=inventory;slot=5') == true, 'open_set(compare) queues')");
        events_are("opened:itemyinf", "open_set(compare): itemyinf, itemxinf already open");
        memcpy(inventory + 5 * 0x2C, kept, sizeof(kept));
        close_all();

        run_lua("check(h:open('casttime', 'seconds=2;style=countdown') == true and h:open('eventtim', '30') == true,"
            " 'the timers queue')");
        float cast_ticks = 0;
        float event_ticks = 0;
        memcpy(&cast_ticks, controller_named("casttime") + 0x14, 4);
        memcpy(&event_ticks, controller_named("eventtim") + 0x14, 4);
        check(get32(controller_named("casttime"), 0x20) == 4 && cast_ticks == 120.0f
                && get32(controller_named("eventtim"), 0x20) == 0 && event_ticks == 1800.0f,
            "the timers' openers, the game's bytes, with the kind and the length in 1/60 seconds");
        events_are("opened:casttime opened:eventtim", "both timers opened");
        close_all();
        run_lua("check(h:open('eventtim', '2.5') == true and h:open('casttime', 'seconds=0.016666666666666666')"
            " == true,"
            " 'fractions queue')");
        memcpy(&cast_ticks, controller_named("casttime") + 0x14, 4);
        memcpy(&event_ticks, controller_named("eventtim") + 0x14, 4);
        check(cast_ticks == 1.0f && event_ticks == 150.0f, "a fraction of a second in the game's unit: 2.5 seconds"
            " 150 counts, 1/60 one");
        events_are("opened:eventtim opened:casttime", "both timers opened");
        close_all();

        run_lua("check(h:open('persona') == true, 'persona queues')");
        const bool layout = controller_named("persona")[0x1C] == 2 && live_menu(row_named("persona"));
        events_are("opened:persona", "persona in the equipment layout");
        close_all();
        run_lua("check(h:open_set('status') == true, 'open_set(status) queues')");
        const bool status = controller_named("persona")[0x1C] == 1;
        events_are("opened:persona", "open_set(status): persona in the Status row\\'s layout");
        close_all();
        put16(g_fake->mon_player, 0x10, 1);
        run_lua("check(h:open_set('status') == true, 'open_set(status) queues')");
        put16(g_fake->mon_player, 0x10, 0);
        check(layout && status && live_menu(row_named("mp_stat")) && get32(controller_named("mp_stat"), 0x14) == 0,
            "open(persona) writes the equipment layout 2; open_set(status) the Status row's 1, or in Monstrosity runs"
            " mp_stat's opener, the game's bytes, with 0");
        events_are("opened:mp_stat", "mp_stat in Monstrosity");
        close_all();
        run_lua("check(h:open_set('status') == true, 'open_set(status) queues')");
        open_now("level");
        open_now("statcom2");
        run_lua("check(h:close('persona') == true, 'close(persona) queues')");
        snprintf(label, sizeof(label), "close(persona) with statcom2 open: the Status screen's Escape, statcom2 closed"
            " by instance, its begin-close (the game's bytes) closing persona: [%s]", opened(now, sizeof(now)));
        check(strcmp(now, "level") == 0, label);
        phase(L,
            "local seen = {}\n"
            "for _, e in ipairs((h:poll())) do seen[e.event .. ':' .. e.name] = true end\n"
            "check(seen['closed:statcom2'] and seen['closed:persona'] and not seen['closed:level'],"
            "  'closed statcom2 and persona')\n"
            "return finish()\n");
        close_all();

        g_fake->macro_editor[0x0C] = 1;
        const LONG sounds = fr->switch_sounds;
        run_lua("check(h:open('macro', 'palette=alt;row=4') == true, 'macro queues')");
        snprintf(label, sizeof(label), "open(macro, {palette = alt, row = 4}) on the Ctrl palette: the game's palette"
            " switch (its bytes, its sound) to 2, mcr2edit it opens held off, then the editor's pick with 3, mcrledit"
            " held off: [%s]", opened(now, sizeof(now)));
        check(strcmp(now, "macro") == 0 && g_fake->macro_editor[0x0C] == 2 && g_fake->macro_editor[0x0E] == 3
                && fr->switch_sounds == sounds + 1, label);
        events_are("opened:macro", "macro alone");
        close_all();
        run_lua("check(h:open('macro', 'palette=alt;row=2') == true, 'macro queues')");
        check(g_fake->macro_editor[0x0C] == 2 && g_fake->macro_editor[0x0E] == 1 && fr->switch_sounds == sounds + 1,
            "open(macro) on the palette already up: no switch, the pick");
        events_are("opened:macro", "macro again");
        g_fake->macro_editor[0x0C] = 1;
        close_all();

        open_now("map0");
        fr->untracked = 0;
        fr->tracking = 1;
        fr->scan_answer = -1;
        run_lua("check(h:open('scanlist') == true, 'scanlist queues')");
        phase(L,
            "local seen = {}\n"
            "for _, e in ipairs((h:poll())) do seen[#seen + 1] = e.event .. ':' .. e.name end\n"
            "check(table.concat(seen, ' ') == 'opened:comyn', 'the Wide Scan row\\'s question: '"
            "  .. table.concat(seen, ' '))\n"
            "local q = h:options('comyn')\n"
            "check(q and q.name == 'comyn' and q.cancellable ~= nil, 'its question on comyn')\n"
            "check(h:answer('comyn', false) == true, 'no')\n"
            "return finish()\n");
        wait_frames(3);
        check(fr->scan_answer == 0 && fr->tracking == 1 && !live_menu(row_named("scanlist")),
            "open(scanlist) while the map tracks: the Wide Scan row's question on comyn, its callback (the game's"
            " own) answered no: still tracking, no list");
        phase(L, "h:poll()\nreturn finish()\n");
        run_lua("check(h:open('scanlist') == true, 'scanlist queues')");
        phase(L,
            "h:poll()\n"
            "check(h:answer('comyn', true) == true, 'yes')\n"
            "return finish()\n");
        wait_frames(3);
        check(fr->scan_answer == 1 && fr->tracking == 0 && live_menu(row_named("scanlist")),
            "answered yes: the callback stops the tracking and opens the list");
        events_are("opened:scanlist closed:comyn", "scanlist opened, and comyn closed, the stand-in freeing it at"
            " once and its closed event coming at the frame\\'s end");
        close_all();
        open_now("map0");
        fr->tracking = 0;
        run_lua("check(h:open('scanlist') == true, 'scanlist queues')");
        check(live_menu(row_named("scanlist")) && strstr(fr->order, "list_format") != NULL,
            "open(scanlist) with map0 open, not tracking: scanlist's opener, the game's bytes");
        events_are("opened:scanlist", "scanlist opened");
        fr->untracked = 1;
        close_all();

        put32(g_fake->route_player, 0x40, 230);
        run_lua("check(h:open_set('region', 'colonization') == true, 'open_set(region) queues')");
        snprintf(label, sizeof(label), "open_set(region, colonization): the Region Info opener on cnqframe's"
            " controller with 3 and the player's area: cnqframe, colopoin: [%s]", opened(now, sizeof(now)));
        check(strcmp(now, "cnqframe colopoin") == 0 && fr->args[0] == 3 && fr->args[1] == 230, label);
        events_are("opened:cnqframe opened:colopoin", "cnqframe and colopoin");
        open_now("colorank");
        run_lua("check(h:open('coloresu', '5') == true, 'coloresu queues')");
        check(get32(controller_named("coloresu"), 0x18) == 5 && live_menu(row_named("coloresu"))
                && !live_menu(row_named("colorank")),
            "open(coloresu, 5) on the Colonization map: the region written, coloresu opened, colorank closed");
        events_are("opened:coloresu closed:colorank", "coloresu, and colorank closed");
        run_lua("check(h:close('cnqframe') == true, 'close(cnqframe) queues')");
        const bool fading = live_menu(row_named("cnqframe")) && (controller_named("cnqframe")[0x14] & 0x40)
            && !live_menu(row_named("coloresu")) && !live_menu(row_named("colopoin"));
        wait_frames(10);
        check(fading && !live_menu(row_named("cnqframe")),
            "close(cnqframe): its Escape, the fade's start (the stand-in's) and its windows closed by name, coloresu"
            " and colopoin among them; the frame closes itself once faded");
        phase(L,
            "local seen = {}\n"
            "for _, e in ipairs((h:poll())) do seen[e.event .. ':' .. e.name] = true end\n"
            "check(seen['closed:coloresu'] and seen['closed:colopoin'] and seen['closed:cnqframe'],"
            "  'closed coloresu, colopoin and cnqframe')\n"
            "return finish()\n");
        close_all();

        run_lua("check(h:open_set('text_colors', '2') == true, 'open_set(text_colors) queues')");
        check(get32(controller_named("textcol1"), 0x14) == 1 && get32(controller_named("textcol2"), 0x1C) == 1
                && get32(controller_named("textcol3"), 0x18) == 1,
            "open_set(text_colors, 2): the category 1 into textcol1, textcol2 and textcol3, as the menu's row writes");
        events_are("opened:textcol2 opened:textcol1", "textcol2, then textcol1");
        run_lua("check(h:close('textcol1') == true, 'close(textcol1) queues')");
        snprintf(label, sizeof(label), "close(textcol1): its Escape, textcol2 then textcol1 closed by name: [%s]",
            opened(now, sizeof(now)));
        check(!live_menu(row_named("textcol1")) && !live_menu(row_named("textcol2")), label);
        events_are("closed:textcol1 closed:textcol2", "textcol1 and textcol2 closed, at the frame\\'s end as the"
            " stand-in frees them at once");
        close_all();

        // a parent's close leaves no child behind: quest01 closed through its
        // own close before quest00's, as the player's Escape leaves it; and
        // the instincts' Escape, the game's own routine (the stand-in's bytes)
        run_lua("check(h:open('quest00', '3') == true, 'quest00 queues')");
        run_lua("check(h:open('quest01', '6') == true, 'quest01 queues')");
        run_lua("check(h:close('quest00') == true, 'close(quest00) queues')");
        check(!live_menu(row_named("quest00")) && !live_menu(row_named("quest01")),
            "close(quest00) with quest01 open: quest01 closed, then quest00");
        phase(L,
            "local seen = {}\n"
            "for _, e in ipairs((h:poll())) do seen[e.event .. ':' .. e.name] = true end\n"
            "check(seen['closed:quest01'] and seen['closed:quest00'], 'closed quest01 and quest00')\n"
            "return finish()\n");
        open_now("comgenre");
        open_now("sccompar");
        run_lua("check(h:close('comgenre') == true, 'close(comgenre) queues')");
        check(!live_menu(row_named("comgenre")) && !live_menu(row_named("sccompar")),
            "close(comgenre) with sccompar open and scresult closed: sccompar closed, then comgenre");
        open_now("mp_stat");
        open_now("mp_cstm");
        open_now("mp_mncst");
        run_lua("check(h:close('mp_mncst') == true, 'close(mp_mncst) queues')");
        check(!live_menu(row_named("mp_stat")) && !live_menu(row_named("mp_cstm")) && !live_menu(row_named("mp_mncst")),
            "close(mp_mncst): its Escape, the game's routine: mp_stat, mp_cstm and mp_mncst closed by name");
        close_all();

        run_lua("check(h:open_set('macros') == true, 'open_set(macros) queues')");
        phase(L, "h:poll()\nreturn finish()\n");
        fr->order[0] = '\0';
        run_lua("check(h:close('mcr1edit') == true, 'close(mcr1edit) queues')");
        snprintf(label, sizeof(label), "close(mcr1edit): a palette's Escape: the macro editor's close (the game's"
            " bytes) of mcrbedit, mcr1edit and mcr2edit, then titlewin's step back (0, 2): [%s]",
            opened(now, sizeof(now)));
        check(now[0] == '\0' && strcmp(fr->order, "title") == 0 && fr->args[0] == 0 && fr->args[1] == 2
                && fr->self == controller_named("titlewin"), label);
        close_all();
    }

    // -- the target arrows (0.11.0): the stand-in's per-menu draw, sprite
    //    draw and quad routine, hooked, under the stand-in's draw walk each
    //    frame; the target window's draw paints its frame, its content and
    //    its arrows, each quad tagged in the walk's log
    {
        FakeLog* fl = g_fake->log;
        uint8_t* tctl = controller_named("targetwi");
        const int trow = row_named("targetwi");
        // The quads of the last walked frame, read between frames.
        char drawn[128];
        auto walked = [&]() {
            wait_frames(3);
            pause_game();
            memcpy(drawn, fl->quads, sizeof(drawn));
            drawn[sizeof(drawn) - 1] = '\0';
            resume_game();
            return static_cast<const char*>(drawn);
        };
        const bool target_was_open = live_menu(trow) != NULL;
        if (target_was_open) {
            phase(L,
                "check(h:close('targetwi') == true, 'close(targetwi)')\n"
                "return finish()\n");
            wait_frames(3);
        }
        tctl[0x65] = 0;
        tctl[0xB8] = 0;
        tctl[0xBA] = 0;
        put16(tctl, 0xBC, 0);
        phase(L,
            "h:poll()\n"
            "return finish()\n");
        open_on_game_thread("menu    targetwi");
        wait_frames(2);
        phase(L,
            "local seen = {}\n"
            "for _, e in ipairs((h:poll())) do seen[#seen + 1] = e.event .. ':' .. e.name end\n"
            "check(table.concat(seen, ' ') == 'opened:targetwi', 'targetwi opens with no arrow: '"
            "  .. table.concat(seen, ' '))\n"
            "local t = h:list().targetarrow or {}\n"
            "check(t.name == 'targetarrow' and t.open == false and t.blockable == true and t.hidden == false"
            "  and t.layer == nil and t.moved == false and t.detail and t.detail.policy == 0,"
            "  'list(): targetarrow, not drawn, blockable, no hold, no layer')\n"
            "local i = h:info('TargetArrow')\n"
            "check(i and i.name == 'targetarrow' and i.open == false and i.resize.holds == 'none' and i.rect == nil"
            "  and i.layer == nil and i.focused == false and i.docked == false and i.detail.controller"
            "  and #i.detail.arrows == 0, 'info(targetarrow) in any case: not drawn, nothing to resize, no rect')\n"
            "return finish()\n");
        put16(tctl, 0xBC, 640);
        put16(tctl, 0xBE, 300);
        wait_frames(2);
        phase(L,
            "local seen = {}\n"
            "for _, e in ipairs((h:poll())) do seen[#seen + 1] = e.event .. ':' .. e.name end\n"
            "check(table.concat(seen, ' ') == 'opened:targetarrow', 'an arrow to draw: opened{targetarrow}: '"
            "  .. table.concat(seen, ' '))\n"
            "local i = h:info('targetarrow')\n"
            "local a = i.detail.arrows\n"
            "check(i.open == true and i.rect == nil and i.origin == nil and #a == 1 and a[1].x == 640 and a[1].y == 300,"
            "  'info(targetarrow): drawn, no box, the arrow at 640,300 under detail')\n"
            "local set = {}\n"
            "for _, n in ipairs(h:opened()) do set[n] = true end\n"
            "check(set.targetarrow and set.targetwi and h:rects().targetarrow == nil and h:rects().targetwi,"
            "  'opened() names targetarrow; rects() has targetwi\\'s frame and nothing for the arrows')\n"
            "check(h:move('targetarrow', 10, 20) == true and h:resize('targetarrow', 3) == true"
            "  and h:resize('targetarrow', 10, 20) == true and h:reset('targetarrow') == true,"
            "  'move, resize and reset of targetarrow are accepted')\n"
            "local a1, w1 = h:answer('targetarrow', 1)\n"
            "local c1, w2 = h:cancel('targetarrow')\n"
            "local o1, w3 = h:options('targetarrow')\n"
            "check(a1 == nil and w1:find('takes no answer', 1, true) and c1 == nil and o1 == nil,"
            "  'answer, cancel and options of targetarrow refuse as for any window that is no prompt')\n"
            "return finish()\n");
        tctl[0xB8] = 1;
        put16(tctl, 0xC0, 700);
        put16(tctl, 0xC2, 310);
        InterlockedExchange(g_fake->draw_walk, 1);
        const char* q = walked();
        const int frames = count_of(q, 'f');
        const bool plain = strstr(q, "tcaa") != NULL && count_of(q, 't') == 1 && count_of(q, 'a') == 2;
        phase(L,
            "check(h:remembered().targetarrow == nil and h:list().targetarrow.moved == false"
            "  and h:info('targetarrow').detail.arrows[2].x == 700, 'nothing remembered for the arrows; the picking'"
            "  .. ' arrow at 700 under detail')\n"
            "check(h:hide('targetwi') == true, 'hide(targetwi)')\n"
            "return finish()\n");
        q = walked();
        uint8_t* tmenu = live_menu(trow);
        const bool kept = strstr(q, "aa") != NULL && count_of(q, 't') == 0 && count_of(q, 'c') == 0
            && count_of(q, 'a') == 2 && count_of(q, 'f') == frames && registry_row(trow)[0x29] == 2
            && tmenu && tmenu[0x60] == 2 && tmenu[0x77] == 0;
        phase(L,
            "check(h:hide('targetarrow') == true, 'hide(targetarrow)')\n"
            "return finish()\n");
        q = walked();
        const bool both = count_of(q, 't') == 0 && count_of(q, 'c') == 0 && count_of(q, 'a') == 0
            && count_of(q, 'f') == frames;
        phase(L,
            "check(h:unhide('targetwi') == true, 'unhide(targetwi)')\n"
            "return finish()\n");
        q = walked();
        const bool arrowless = strstr(q, "tc") != NULL && count_of(q, 'a') == 0 && count_of(q, 'f') == frames
            && tmenu[0x77] == 1;
        phase(L,
            "check(h:unhide('targetarrow') == true, 'unhide(targetarrow)')\n"
            "return finish()\n");
        q = walked();
        const bool back = strstr(q, "tcaa") != NULL && count_of(q, 'f') == frames;
        check(plain && kept && both && arrowless && back,
            "the walk over the target window: frame, content and both arrows drawn; hidden, it keeps its layer 2 in"
            " its row and live menu and +0x77 0, and only its arrows are drawn; with targetarrow hidden too,"
            " nothing of it; targetarrow alone hidden, all but the arrows; both shown, all again; no other"
            " window's frame dropped at any step");
        phase(L,
            "check(h:close('targetarrow') == true, 'close(targetarrow) is a hide')\n"
            "return finish()\n");
        q = walked();
        const bool closed = count_of(q, 'a') == 0 && strstr(q, "tc") != NULL;
        phase(L,
            "check(h:info('targetarrow').hidden and h:info('targetarrow').open and #(h:poll()) == 0,"
            "  'info(targetarrow) after close: hidden, still drawn by the game, no event')\n"
            "check(h:open('targetarrow') == true and h:block('targetarrow') == true, 'open(targetarrow) is an unhide;'"
            "  .. ' block(targetarrow) holds')\n"
            "return finish()\n");
        q = walked();
        const bool blocked = count_of(q, 'a') == 0 && strstr(q, "tc") != NULL;
        phase(L,
            "local i = h:info('targetarrow')\n"
            "check(i.blocked and i.hidden and #i.blocked_by == 1 and #(h:poll()) == 0, 'blocked: hidden by the block,'"
            "  .. ' no event')\n"
            "check(h:unblock('targetarrow') == true, 'unblock(targetarrow)')\n"
            "return finish()\n");
        q = walked();
        check(closed && blocked && strstr(q, "tcaa") != NULL,
            "close and block of targetarrow hold its arrows off the screen as a hide does; open and unblock drop"
            " the hold");
        tctl[0xB8] = 0;
        put16(tctl, 0xBC, 0);
        wait_frames(2);
        phase(L,
            "local seen = {}\n"
            "for _, e in ipairs((h:poll())) do seen[#seen + 1] = e.event .. ':' .. e.name end\n"
            "check(table.concat(seen, ' ') == 'closed:targetarrow' and h:info('targetarrow').open == false,"
            "  'no arrow left to draw: closed{targetarrow}: ' .. table.concat(seen, ' '))\n"
            "check(h:close('targetwi') == true, 'close(targetwi)')\n"
            "return finish()\n");
        InterlockedExchange(g_fake->draw_walk, 0);
        wait_frames(3);
        if (target_was_open) {
            open_on_game_thread("menu    targetwi");
            wait_frames(2);
        }
        phase(L,
            "h:poll()\n"
            "return finish()\n");
    }

    // -- a hidden window runs as the game runs it shown (0.11.0): the
    //    stand-in's walk each frame, a close ended by the per-menu draw and
    //    an instance freed by the update once it is marked. Nothing of a
    //    hidden window's draw reaches the device and every other window's
    //    does as before; conq3pts, closed by its own controller's draw once
    //    its data is gone, runs out its input wait, closes and is freed on
    //    the same frames hidden as shown; hidden maplist, answered, runs
    //    map0's callback, which opens map0 again, then closes, and is freed
    //    before map0's input wait runs out; nothing touches a freed instance
    {
        FakeLog* fl = g_fake->log;
        typedef int (*CountFn)();
        typedef int (__fastcall* CloseFn)(void* mcb, void* edx, const char* key);
        const CountFn touched = fake_export<CountFn>("fake_freed_touched");
        CloseFn close_by_name;
        memcpy(&close_by_name, &g_fake->close, sizeof(close_by_name));
        const uint32_t close_frames = *g_fake->close_frames;
        uint8_t* const conq_data = *g_fake->conq_data;
        *g_fake->close_frames = 2;
        InterlockedExchange(g_fake->close_draws, 1);
        phase(L,
            "h:poll()\n"
            "return finish()\n");

        // The device's draws of one walk, each menu's, made here while no
        // frame runs.
        auto painted_by = [&](const uint8_t* menu) {
            int n = 0;
            for (int i = 0; i < fl->painted_count; ++i) {
                n += fl->painted[i] == menu ? 1 : 0;
            }
            return n;
        };
        auto walk_here = [&]() {
            typedef void (__cdecl* WalkFn)();
            WalkFn walk;
            memcpy(&walk, &g_fake->walk_now, sizeof(walk));
            pause_game();
            const LONG before = fl->pixels;
            walk();
            const LONG pixels = fl->pixels - before;
            resume_game();
            return pixels;
        };
        uint8_t* equip = live_menu(kRowEquip) ? live_menu(kRowEquip)
            : static_cast<uint8_t*>(open_on_game_thread("menu    equip"));
        uint8_t* buff = live_menu(kRowBuff) ? live_menu(kRowBuff)
            : static_cast<uint8_t*>(open_on_game_thread("menu    buff"));
        wait_frames(2);
        const LONG all = walk_here();
        int before[64];
        for (int i = 0; i < fl->walked_count; ++i) {
            before[i] = painted_by(static_cast<const uint8_t*>(fl->walked[i]));
        }
        const int walked = fl->walked_count;
        const int buff_before = painted_by(buff);
        phase(L,
            "check(h:hide('buff') == true, 'hide(buff)')\n"
            "return finish()\n");
        wait_frames(2);
        const LONG hidden_all = walk_here();
        bool others = equip && buff && fl->walked_count == walked;
        int buff_after = -1;
        for (int i = 0; others && i < walked; ++i) {
            const uint8_t* m = static_cast<const uint8_t*>(fl->walked[i]);
            const int now = painted_by(m);
            if (m == buff) {
                buff_after = now;
            } else {
                others = now == before[i];
            }
        }
        char text[512];
        snprintf(text, sizeof(text),
            "one walk over %d menus: %ld draws at the device with nothing hidden, buff's %d; with buff hidden, %ld,"
            " none of buff's (%d) and every other menu's as before", walked, static_cast<long>(all), buff_before,
            static_cast<long>(hidden_all), buff_after);
        check(others && buff_before > 0 && buff_after == 0 && hidden_all == all - buff_before
                && painted_by(equip) > 0, text);
        phase(L,
            "check(h:unhide('buff') == true, 'unhide(buff)')\n"
            "return finish()\n");

        // conq3pts shown, then hidden: opened, its input wait 3 put on it,
        // then its data gone, each between frames; the frames each of the
        // two runs out after, the frame of its closed event, and its freed
        // instance.
        InterlockedExchange(g_fake->draw_walk, 1);
        const int crow = row_named("conq3pts");
        struct Run {
            LONG ready;
            LONG freed;
            bool gone;
            int pixels;
        };
        auto conq_run = [&](bool hidden) {
            Run r;
            memset(&r, 0, sizeof(r));
            r.ready = -1;
            r.freed = -1;
            if (hidden) {
                phase(L,
                    "check(h:hide('conq3pts') == true, 'hide(conq3pts)')\n"
                    "return finish()\n");
                wait_frames(2);
            }
            uint8_t* menu = static_cast<uint8_t*>(open_on_game_thread("menu    conq3pts"));
            pause_game();
            const int readies = fl->ready_count;
            if (menu) {
                put16(menu, 0x56, 3);
            }
            const LONG opened = *g_fake->frames;
            resume_game();
            wait_frames(5);
            phase(L,
                "h:poll()\n"
                "return finish()\n");
            pause_game();
            for (int i = readies; i < fl->ready_count; ++i) {
                r.ready = fl->ready[i] == menu ? fl->ready_frame[i] - opened : r.ready;
            }
            const int frees = fl->freed_count;
            for (int i = 0; i < fl->painted_count; ++i) {
                r.pixels += fl->painted[i] == menu ? 1 : 0;
            }
            *g_fake->conq_data = NULL;
            const LONG gone = *g_fake->frames;
            resume_game();
            wait_frames(8);
            pause_game();
            for (int i = frees; i < fl->freed_count; ++i) {
                r.freed = fl->freed[i] == menu ? fl->freed_frame[i] - gone : r.freed;
            }
            r.gone = live_menu(crow) == NULL;
            *g_fake->conq_data = conq_data;
            resume_game();
            phase(L,
                "local closed, set = 0, {}\n"
                "for _, e in ipairs((h:poll())) do\n"
                "  if e.event == 'closed' and e.name == 'conq3pts' then closed = closed + 1 end\n"
                "end\n"
                "for _, n in ipairs(h:opened()) do set[n] = true end\n"
                "check(closed == 1 and not set.conq3pts and h:unhide('conq3pts') == true, 'conq3pts closed by its own'"
                "  .. ' draw: closed{conq3pts} once, out of opened(); unhide(conq3pts)')\n"
                "return finish()\n");
            return r;
        };
        const Run shown = conq_run(false);
        const Run hidden = conq_run(true);
        snprintf(text, sizeof(text),
            "conq3pts shown: its input wait out %ld frames after it was put on, freed %ld frames after its data is"
            " gone, its own controller's draw closing it, %d draws of it at the device in the last walked frame"
            " before; hidden: out %ld frames after, freed %ld frames after, %d draws of it",
            static_cast<long>(shown.ready), static_cast<long>(shown.freed), shown.pixels,
            static_cast<long>(hidden.ready), static_cast<long>(hidden.freed), hidden.pixels);
        check(shown.ready == 3 && shown.freed == 6 && shown.gone && shown.pixels == 2 && hidden.ready == shown.ready
                && hidden.freed == shown.freed && hidden.gone && hidden.pixels == 0, text);

        // hidden maplist answered at its top, a map: map0's callback opens
        // map0 again with its input wait of 8, then maplist's close+reset
        // closes it; its staged close runs out in its draws and the update
        // frees it, before map0's wait is out; then map0 closed by the game
        const int mrow = row_named("maplist");
        const int zrow = row_named("map0");
        phase(L,
            "check(h:hide('maplist') == true, 'hide(maplist)')\n"
            "return finish()\n");
        wait_frames(3);
        asked_on_game_thread(4, 100);
        wait_frames(2);
        uint8_t* list = live_menu(mrow);
        const int readies = fl->ready_count;
        const int frees = fl->freed_count;
        phase(L,
            "local o = h:options('maplist')\n"
            "check(o and o.level == 'top' and o.rows[1].kind == 'map' and h:info('maplist').hidden == true,"
            "  'maplist up, hidden, at its top, its first row a map')\n"
            "check(h:answer('maplist', 1) == true, 'answer(maplist, 1): the leaf')\n"
            "return finish()\n");
        wait_frames(12);
        uint8_t* map0 = live_menu(zrow);
        LONG list_freed = -1;
        LONG map0_ready = -1;
        pause_game();
        for (int i = frees; i < fl->freed_count; ++i) {
            list_freed = fl->freed[i] == list ? fl->freed_frame[i] : list_freed;
        }
        for (int i = readies; i < fl->ready_count; ++i) {
            map0_ready = fl->ready[i] == map0 ? fl->ready_frame[i] : map0_ready;
        }
        const bool list_gone = live_menu(mrow) == NULL;
        resume_game();
        phase(L,
            "local closed, set = 0, {}\n"
            "for _, e in ipairs((h:poll())) do\n"
            "  if e.event == 'closed' and e.name == 'maplist' then closed = closed + 1 end\n"
            "end\n"
            "for _, n in ipairs(h:opened()) do set[n] = true end\n"
            "check(closed == 1 and not set.maplist and set.map0, 'closed{maplist} once; opened() names map0 and not'"
            "  .. ' maplist')\n"
            "return finish()\n");
        pause_game();
        const int map0_frees = fl->freed_count;
        close_by_name(g_fake->mcb, NULL, "menu    map0    ");
        resume_game();
        wait_frames(8);
        pause_game();
        bool map0_freed = false;
        for (int i = map0_frees; i < fl->freed_count; ++i) {
            map0_freed = map0_freed || fl->freed[i] == map0;
        }
        const int touches = touched ? touched() : -1;
        resume_game();
        snprintf(text, sizeof(text),
            "hidden maplist answered with map0 opening again: its instance freed at frame %ld, map0's input wait out"
            " at frame %ld; map0 then closed by the game and freed (%d); %d freed instances touched since",
            static_cast<long>(list_freed), static_cast<long>(map0_ready), map0_freed ? 1 : 0, touches);
        check(list && map0 && list_gone && list_freed > 0 && map0_ready > 0 && list_freed < map0_ready && map0_freed
                && touches == 0, text);
        phase(L,
            "check(h:unhide('maplist') == true, 'unhide(maplist)')\n"
            "h:poll()\n"
            "return finish()\n");
        InterlockedExchange(g_fake->draw_walk, 0);
        InterlockedExchange(g_fake->close_draws, 0);
        *g_fake->close_frames = close_frames;
        wait_frames(3);
    }

    // -- map0's close (0.11.0): its own cancel, as its input routine runs it
    //    on Escape: closed by instance, then the map's close on mapframe's
    //    global with 0, which closes the map's windows by name and starts the
    //    frame's fade-out (the stand-in's closes the frame at once). Shown,
    //    hidden, and as a block's close of it alike; mapv2 and mapframe close
    //    by name alone
    {
        FakeLog* fl = g_fake->log;
        typedef int (__fastcall* CloseFn)(void* mcb, void* edx, const char* key);
        CloseFn close_by_name;
        memcpy(&close_by_name, &g_fake->close, sizeof(close_by_name));
        uint8_t* frame = controller_named("mapframe");
        const uint32_t close_frames = *g_fake->close_frames;
        *g_fake->close_frames = 1;
        const int map_row = row_named("map0");
        const int frame_row = row_named("mapframe");
        const int v2_row = row_named("mapv2");
        auto map_up = [&]() {
            open_on_game_thread("menu    mapframe");
            open_on_game_thread("menu    map0    ");
            open_on_game_thread("menu    mapv2   ");
            wait_frames(2);
            phase(L,
                "h:poll()\n"
                "return finish()\n");
            return live_menu(map_row) && live_menu(frame_row) && live_menu(v2_row);
        };
        // Each of the three map windows closed once since the last poll, and
        // none open.
        auto all_closed = [&](const char* what) {
            char script[768];
            snprintf(script, sizeof(script),
                "local n, seen = {}, {}\n"
                "for _, e in ipairs((h:poll())) do\n"
                "  seen[#seen + 1] = e.event .. ':' .. tostring(e.name)\n"
                "  if e.event == 'closed' then n[e.name] = (n[e.name] or 0) + 1 end\n"
                "end\n"
                "local o = {}\n"
                "for _, w in ipairs(h:opened()) do o[w] = true end\n"
                "check(n.map0 == 1 and n.mapv2 == 1 and n.mapframe == 1 and not o.map0 and not o.mapv2"
                "  and not o.mapframe, '%s: closed{map0}, closed{mapv2} and closed{mapframe} once each, none open: '"
                "  .. table.concat(seen, ' '))\n"
                "return finish()\n", what);
            phase(L, script);
        };
        const bool up = map_up();
        int closes = fl->map_closes;
        int fades = fl->map_fades;
        phase(L,
            "check(h:close('map0') == true, 'close(map0) queues')\n"
            "return finish()\n");
        wait_frames(3);
        pause_game();
        const bool shown = fl->map_closes == closes + 1 && fl->map_fades == fades + 1 && fl->map_frame == frame
            && fl->map_mode == 0 && fl->map0_open_at_close == 0 && !live_menu(map_row) && !live_menu(frame_row)
            && !live_menu(v2_row);
        resume_game();
        all_closed("close(map0)");
        check(up && shown,
            "close(map0): closed by instance first, then the map's close once, on mapframe's controller with 0, its"
            " closes by name taking mapv2 and its fade-out the frame");

        const bool up_hidden = map_up();
        closes = fl->map_closes;
        fades = fl->map_fades;
        phase(L,
            "check(h:hide('map0') == true and h:info('map0').hidden == true, 'hide(map0)')\n"
            "return finish()\n");
        wait_frames(2);
        phase(L,
            "check(h:close('map0') == true, 'close(map0), hidden')\n"
            "return finish()\n");
        wait_frames(3);
        pause_game();
        const bool hidden = fl->map_closes == closes + 1 && fl->map_fades == fades + 1 && fl->map_frame == frame
            && fl->map0_open_at_close == 0 && !live_menu(map_row) && !live_menu(frame_row) && !live_menu(v2_row);
        resume_game();
        all_closed("close(map0), hidden");
        phase(L,
            "check(h:unhide('map0') == true, 'unhide(map0)')\n"
            "return finish()\n");

        const bool up_blocked = map_up();
        closes = fl->map_closes;
        fades = fl->map_fades;
        phase(L,
            "check(h:block('map0') == true, 'block(map0) with map0 open')\n"
            "return finish()\n");
        wait_frames(3);
        pause_game();
        const bool blocked = fl->map_closes == closes + 1 && fl->map_fades == fades + 1 && fl->map_frame == frame
            && fl->map0_open_at_close == 0 && !live_menu(map_row) && !live_menu(frame_row) && !live_menu(v2_row);
        resume_game();
        all_closed("block(map0)");
        phase(L,
            "check(h:unblock('map0') == true, 'unblock(map0)')\n"
            "h:poll()\n"
            "return finish()\n");
        check(up_hidden && hidden && up_blocked && blocked,
            "hidden, and as the close a block of it queues, map0's close is the same cancel");

        const bool up_others = map_up();
        closes = fl->map_closes;
        phase(L,
            "check(h:close('mapv2') == true and h:close('mapframe') == true, 'close(mapv2) and close(mapframe)')\n"
            "return finish()\n");
        wait_frames(3);
        pause_game();
        const bool others = fl->map_closes == closes && live_menu(map_row) && !live_menu(frame_row)
            && !live_menu(v2_row);
        close_by_name(g_fake->mcb, NULL, "menu    map0    ");
        resume_game();
        wait_frames(2);
        phase(L,
            "local n = {}\n"
            "for _, e in ipairs((h:poll())) do\n"
            "  if e.event == 'closed' then n[e.name] = (n[e.name] or 0) + 1 end\n"
            "end\n"
            "check(n.map0 == 1 and n.mapv2 == 1 and n.mapframe == 1, 'mapv2 and mapframe closed, then map0 by the"
            " game')\n"
            "check(h:close('map0') == true, 'close(map0) with map0 not open queues')\n"
            "return finish()\n");
        wait_frames(3);
        check(up_others && others && fl->map_closes == closes,
            "mapv2 and mapframe close by name alone, map0 left open; close(map0) with no map0 open runs no map's"
            " close");
        *g_fake->close_frames = close_frames;
    }

    // -- a hidden window at its own draw layer (0.11.0): the stand-in's
    //    update picks the active menu as the game's does, by the order the
    //    draw layers give the manager's list. Hidden stringdl, asking the
    //    name, is the active menu while it is up alone and takes no routed
    //    key; delivery, opened after it as the outgoing box's commit of a
    //    name focuses it, takes the focus and the keys. stringdl's draw
    //    runs and paints nothing throughout.
    {
        typedef uint32_t (__fastcall* InputFn)(void* self, void* edx, int code);
        InputFn route;
        memcpy(&route, &g_fake->menu_routing, sizeof(route));
        const FakeLog* fl = g_fake->log;
        const int row_entry = row_named("stringdl");
        const int row_box = row_named("delivery");
        const bool closed_before = !live_menu(row_entry) && !live_menu(row_box);
        phase(L,
            "h:poll()\n"
            "check(h:hide('stringdl') == true, 'hide(stringdl)')\n"
            "return finish()\n");
        wait_frames(3);
        uint8_t* outgoing = controller_named("delivery");
        uint8_t* asking = controller_named("stringdl");
        put16(outgoing, 0x1B4, 6);
        asking[0x14] = 1;
        InterlockedExchange(g_fake->active_pick, 1);
        uint8_t* entry = static_cast<uint8_t*>(open_on_game_thread("menu    stringdl"));
        wait_frames(3);
        const bool entry_active = entry && get32(g_fake->mcb, 0x54) == reinterpret_cast<uint32_t>(entry);
        const int sinks = fl->sink_calls;
        const bool dropped = route(g_fake->mcb, NULL, 5) == 0 && fl->sink_calls == sinks;
        const bool entry_hidden = hidden_in_place(row_entry, entry);
        uint8_t* box = static_cast<uint8_t*>(open_on_game_thread("menu    delivery"));
        wait_frames(3);
        const bool box_active = box && get32(g_fake->mcb, 0x54) == reinterpret_cast<uint32_t>(box);
        const bool reached = route(g_fake->mcb, NULL, 5) == 1 && fl->sink_calls == sinks + 1 && fl->sink_menu == box;
        const bool still_hidden = hidden_in_place(row_entry, entry) && walk_of(box) == 'd';
        InterlockedExchange(g_fake->active_pick, 0);
        put32(g_fake->mcb, 0x54, 0);
        check(closed_before && entry_active && dropped && entry_hidden,
            "hidden stringdl asking the name, opened with nothing above it, is the active menu at its own layer 2:"
            " its draw paints nothing and no routed key reaches it");
        check(box_active && reached && still_hidden,
            "delivery, opened after it, is the active menu the stand-in's pick makes and takes the routed keys;"
            " stringdl stays hidden at its own layer under it");
        phase(L,
            "check(h:unhide('stringdl') == true, 'unhide(stringdl)')\n"
            "return finish()\n");
        wait_frames(3);
        {
            typedef int (__fastcall* CloseFn)(void* mcb, void* edx, const char* key);
            CloseFn close_by_name;
            memcpy(&close_by_name, &g_fake->close, sizeof(close_by_name));
            pause_game();
            close_by_name(g_fake->mcb, NULL, "menu    delivery");
            close_by_name(g_fake->mcb, NULL, "menu    stringdl");
            put16(outgoing, 0x1B4, 0);
            asking[0x14] = 0;
            resume_game();
        }
        wait_frames(3);
        phase(L,
            "h:poll()\n"
            "return finish()\n");
    }

    // -- the macro keys (0.9.0): the stand-in's gate, hooked, consulted each
    //    frame a number key is held; a block has it say no and closes a bar
    //    the handler has up; macros() and status() read who holds it
    {
        uint8_t* macro = g_fake->macro_object;
        const FakeLog* fl = g_fake->log;
        const int row_bar = row_named("mcr1pall");
        InterlockedExchange(g_fake->macro_key, 1);
        wait_frames(3);
        const LONG fired = *g_fake->macros_fired;
        wait_frames(3);
        check(*g_fake->macros_fired > fired && static_cast<uint8_t*>(g_fake->macro_gate)[0] == 0xE9,
            "a number key held: a macro fires each frame through the hooked gate, which says yes");
        phase(L,
            "local m = h:macros()\n"
            "check(m.blocked == false and #m.blocked_by == 0 and m.mine == false"
            "  and hu.status().macros_blocked == false,"
            "  'macros(): not blocked, nobody holds it; status().macros_blocked false')\n"
            "check(h:block_macros() == true, 'block_macros holds')\n"
            "return finish()\n");
        wait_frames(3);
        const LONG blocked = *g_fake->macros_fired;
        wait_frames(3);
        check(*g_fake->macros_fired == blocked,
            "blocked: over three frames the gate's pre answers no and no macro fires");
        phase(L,
            "local m = h:macros()\n"
            "check(m.blocked == true and #m.blocked_by == 1 and m.blocked_by[1] == 'alpha' and m.mine == true"
            "  and hu.status().macros_blocked == true, 'macros(): blocked by alpha, mine; status().macros_blocked')\n"
            "hm = hu.new('macro_owner')\n"
            "local o = hm:macros()\n"
            "check(o.blocked == true and #o.blocked_by == 1 and o.blocked_by[1] == 'alpha' and o.mine == false,"
            "  'macros() through another handle: blocked by alpha, not mine')\n"
            "check(h:unblock_macros() == true, 'unblock_macros drops the hold')\n"
            "return finish()\n");
        wait_frames(3);
        const LONG resumed = *g_fake->macros_fired;
        wait_frames(3);
        check(*g_fake->macros_fired > resumed, "unblocked: the macros fire again");
        phase(L,
            "check(h:block_macros() == true and hm:block_macros() == true and h:block_macros() == true,"
            "  'alpha and macro_owner both block the macro keys, alpha twice')\n"
            "check(h:unblock_macros() == true, 'alpha unblocks')\n"
            "local m = h:macros()\n"
            "check(m.blocked == true and #m.blocked_by == 1 and m.blocked_by[1] == 'macro_owner' and m.mine == false"
            "  and hu.status().macros_blocked == true, 'still blocked by macro_owner: ' .. table.concat(m.blocked_by, ' '))\n"
            "return finish()\n");
        wait_frames(3);
        const LONG held = *g_fake->macros_fired;
        wait_frames(3);
        check(*g_fake->macros_fired == held, "and no macro fires while macro_owner holds the block");
        phase(L,
            "check(hm:release() == true, 'macro_owner releases')\n"
            "local m = h:macros()\n"
            "check(m.blocked == false and #m.blocked_by == 0 and hu.status().macros_blocked == false,"
            "  'the release drops its block: not blocked')\n"
            "return finish()\n");
        wait_frames(3);
        const LONG released = *g_fake->macros_fired;
        wait_frames(3);
        check(*g_fake->macros_fired > released, "released: the macros fire again");
        // the Ctrl bar up at the block: opened by name as the handler opens
        // it, the object's bytes as the handler writes them; the close as the
        // game runs it, the instance freed a frame after its marks
        uint8_t* bar = static_cast<uint8_t*>(open_on_game_thread("menu    mcr1pall"));
        macro[0x0C] = 1;
        macro[0x0D] = 2;
        macro[0x18] = 1;
        wait_frames(2);
        memset(g_fake->log, 0, sizeof(*g_fake->log));
        *g_fake->close_frames = 1;
        phase(L,
            "h:poll()\n"
            "check(h:block_macros() == true, 'block_macros with the Ctrl bar up queues its close')\n"
            "return finish()\n");
        wait_frames(3);
        *g_fake->close_frames = 0;
        check(bar && !live_menu(row_bar) && strcmp(fl->last_close, "menu    mcr1pall") == 0 && macro[0x0C] == 0
                && macro[0x18] == 0 && macro[0x0D] == 2,
            "the bar is closed through the game's close by name, +0x0C and +0x18 read 0, +0x0D untouched");
        phase(L,
            "local out = {}\n"
            "for _, e in ipairs((h:poll())) do out[#out + 1] = e.event .. ':' .. e.name end\n"
            "check(table.concat(out, ' ') == 'closed:mcr1pall', 'closed{mcr1pall} follows: ' .. table.concat(out, ' '))\n"
            "check(h:unblock_macros() == true, 'unblock_macros')\n"
            "return finish()\n");
        wait_frames(3);
        InterlockedExchange(g_fake->macro_key, 0);
        const LONG idle = *g_fake->macros_fired;
        wait_frames(3);
        check(*g_fake->macros_fired == idle, "with no number key held nothing fires");
    }

    check(open_on_game_thread("menu    menuwind") == NULL, "the hooked open refuses a blocked name");
    uint8_t* equip = static_cast<uint8_t*>(open_on_game_thread("menu    equip"));
    check(equip && origin(equip, 0) == 300 && origin(equip, 1) == 200,
        "equip opens at its remembered position, re-applied by the open hook's post");

    phase(L,
        "local list, dropped = h:poll()\n"
        "local seen = {}\n"
        "for _, e in ipairs(list) do\n"
        "  seen[#seen + 1] = e.event .. ':' .. e.name\n"
        "end\n"
        "check(table.concat(seen, ' ') == 'blocked:menuwind' and dropped == 0,"
        "  'events, none for equip, open already: ' .. table.concat(seen, ' '))\n"
        "local p = h:info('equip')\n"
        "local m = p and p.memory and p.memory.position or {}\n"
        "check(p and p.open and p.origin.x == 300 and p.origin.y == 200 and m.mine and m.owner == 'alpha'"
        "  and m.group == nil and p.detail.layout == 'self',"
        "  'info(equip): open at 300,200, its position remembered, owned by alpha')\n"
        "check(p.rect.w == p.rect.right - p.rect.x and p.rect.h == p.rect.bottom - p.rect.y and p.default.w == 120"
        "  and p.default.h == 40 and p.default.right == p.default.x + 120,"
        "  'info: rectangles as {x, y, w, h}, with right and bottom')\n"
        "check(p.detail.address and p.detail.controller and p.detail.policy and p.detail.rows and p.detail.live_layer"
        "  and p.address == nil and p.policy == nil and p.layout == nil and p.rows == nil and p.covered == false"
        "  and p.docked == false and p.layer == 2,"
        "  'info: the engine and the game words under detail; covered, docked and layer at the top')\n"
        "local pos = h:remembered()\n"
        "local e = pos.equip and pos.equip.position or {}\n"
        "check(e.x == 300 and e.y == 200 and e.mine and pos.equip.size == nil and h.positions == nil,"
        "  'remembered() has equip; positions() is no more')\n"
        "local all = h:list()\n"
        "check(all.logwindo and all.logwindo.open and all.logwindo.hidden and all.logwindo.detail.policy"
        "  and all.logwindo.policy == nil and #all == 0, 'list(): keyed by name, logwindo open and hidden')\n"
        "check(all.logwindo.blockable == true and all.query.blockable == false and all.logwindo.hide_only == nil,"
        "  'list(): blockable, true for every window but query')\n"
        "local opened, open_set = h:opened(), {}\n"
        "for _, name in ipairs(opened) do open_set[name] = true end\n"
        "check(open_set.logwindo and open_set.equip and not open_set.menuwind, 'opened(): ' .. table.concat(opened, ' '))\n"
        "local focus, focus_why = h:focused()\n"
        "check(focus == false and focus_why == nil, 'focused(): false while the manager has no active menu')\n"
        "local g = h:groups()\n"
        "local carried = {}\n"
        "for _, n in ipairs(g.chat_log.members) do carried[n] = true end\n"
        "check(g.chat_log.anchor == 'logwindo' and g.chat_log.anchor_open and #g.chat_log.members == 49"
        "  and carried.ability and carried.partywin and carried.targetwi and carried.subwindo and not carried.logwindo"
        "  and g.chat_log.index == nil and g.chat_log.mask == nil and g.chat_log.name == nil,"
        "  'groups(): chat_log anchored on logwindo, carrying ' .. #g.chat_log.members .. ' pieces: its 41,"
        " partywin, its 6 and subwindo; no index or mask')\n"
        "check(#g.party_list.members == 7 and #g.target_window.members == 1 and g.target_window.members[1] == 'subwindo',"
        "  'groups(): party_list carries ' .. #g.party_list.members .. ', target_window ' .. #g.target_window.members)\n"
        "local log = h:info('logwindo')\n"
        "check(g.chat_log.origin and g.chat_log.origin.x == log.origin.x and g.chat_log.origin.y == log.origin.y"
        "  and g.party_list.origin == nil, 'groups(): the anchor\\'s origin while it is open')\n"
        "check(log.blockable == true and log.hide_only == nil and h:info('query').blockable == false"
        "  and #log.hidden_by == 1 and log.hidden_by[1] == 'alpha' and log.mine.hidden == true"
        "  and log.mine.blocked == false and #log.blocked_by == 0, 'info(): blockable, and who hides it: alpha')\n"
        "local mw = h:info('menuwind')\n"
        "check(mw.hidden and mw.blocked and #mw.hidden_by == 0 and #mw.blocked_by == 1 and mw.blocked_by[1] == 'alpha'"
        "  and mw.mine.blocked and not mw.mine.hidden, 'info(): a block hides, and is listed as a block alone')\n"
        "check(h:move_group('chat_log', log.origin.x + 5, log.origin.y + 7) == true, 'move_group chat_log to x,y queues')\n"
        "h2 = hu.new('beta')\n"
        "check(h2:hide('logwindo') == true and h2:move('equip', 10, 20) == true, 'a second handle hides logwindo and moves equip')\n"
        "local by = h2:info('logwindo')\n"
        "check(#by.hidden_by == 2 and by.hidden_by[1] == 'alpha' and by.hidden_by[2] == 'beta' and by.mine.hidden,"
        "  'info(logwindo) through beta: hidden by alpha and beta, and beta is one')\n"
        "local w = h:groups().party_list\n"
        "check(w.anchor_open == false and w.origin == nil and w.waiting and w.waiting.x == 1 and w.waiting.y == 1"
        "  and w.waiting.owner == 'alpha' and w.waiting.mine == true and h2:groups().party_list.waiting.mine == false,"
        "  'groups(): the move waiting for partywin once the game thread has taken it, its owner, and mine for alpha'"
        "  .. ' alone')\n"
        "check(h:layout().groups.party_list.x == 1 and h2:layout().groups == nil and h:reset('partywin') == true,"
        "  'layout(): the waiting move of party_list, for alpha alone; reset of partywin queues')\n"
        "return finish()\n");

    wait_frames(3);
    check(origin(log, 0) == 100 + kRowLog + 5 && origin(log, 1) == 50 + 7,
        "move_group chat_log to its origin + 5,7: logwindo's origin there");
    check(origin(equip, 0) == 10 && origin(equip, 1) == 20, "beta's later move of equip wins");

    // -- the yes/no windows (0.11.0): the stand-in's class, its ask run on the
    //    game thread as the game's askers run it, each question answered or
    //    canceled as the class's own confirm and cancel end it
    {
        FakeLog* yl = g_fake->log;
        uint8_t* comyn = controller_named("comyn");
        uint8_t* resyn = controller_named("resyn");
        const int kRowComyn = row_named("comyn");
        const int kRowResynus = row_named("resynus");
        phase(L,
            "for _, x in ipairs({h, h2}) do x:poll() end\n"
            "function yesno_events(handle)\n"
            "  local out = {}\n"
            "  for _, e in ipairs((handle:poll())) do out[#out + 1] = e.event .. ':' .. tostring(e.name) end\n"
            "  return table.concat(out, ' ')\n"
            "end\n"
            "local o, why = h:options('comyn')\n"
            "check(o == nil and why == 'no question is pending in comyn', 'options(comyn) with nothing asked: '"
            "  .. tostring(why))\n"
            "local ok, w = h:open('comyn')\n"
            "check(ok == nil and w == \"comyn opens only for the game's own event; answer or cancel it instead\","
            "  'open(comyn) refused: ' .. tostring(w))\n"
            "ok, w = h:close('comyn')\n"
            "check(ok == nil and w == \"comyn is a prompt the game waits on; use cancel('comyn')\","
            "  'close(comyn) refused: ' .. tostring(w))\n"
            "ok, w = h:answer('comyn', true)\n"
            "check(ok == nil and w == 'no question is pending in comyn', 'answer(comyn) with nothing pending: '"
            "  .. tostring(w))\n"
            "return finish()\n");
        const int selects = yl->select_calls;
        const int asked = ask_on_game_thread("comyn", 0, 1, 0, 0);
        uint8_t* first = live_menu(kRowComyn);
        check(asked == 1 && first && get16(first, 0x4C) == 2 && yl->select_calls == selects + 1
                && yl->select_menu == first && yl->select_row == 2,
            "comyn asks, its default no: its window opens and the class's select-row puts the cursor on no");
        wait_frames(2);
        phase(L,
            "local o = h:options('comyn')\n"
            "check(o and o.name == 'comyn' and o.text == '\\129\\153Log out now?' and o.undecoded == 0"
            "  and o.default == false and o.cancellable == true and o.hold == false and o.detail"
            "  and o.detail.mode == 0 and type(o.id) == 'number',"
            "  'options(comyn): the question from the game\\'s text, no first, cancellable, no hold: '"
            "  .. tostring(o and o.text))\n"
            "local seg = o and o.segments or {}\n"
            "check(#seg == 3 and seg[1].text == '\\129\\153Log out ' and seg[1].color == 1 and seg[1].escape == 30"
            "  and seg[2].text == 'now' and seg[2].color == 2 and seg[3].text == '?' and seg[3].color == 1,"
            "  'its runs: the green one between, the 7F escape at its end drawn as nothing')\n"
            "local ok, err = pcall(h.answer, h, 'comyn', 1)\n"
            "check(not ok and tostring(err):find(\"answer('comyn', yes) takes true for yes or false for no\", 1, true),"
            "  'answer(comyn, 1) raises: ' .. tostring(err))\n"
            "comyn_prompt = o\n"
            "return finish()\n");
        {
            HuEngineApi api;
            HuEngineHandle raw = {-1, 0};
            char why[512] = "";
            const bool have = record_api(&api);
            if (have) {
                api.handle_open("raw", &raw, why, sizeof(why));
            }
            const int32_t old = raw.slot >= 0
                ? api.answer5(&raw, "comyn", HU_ANSWER_BOOLEAN, 1, NULL, 0, 0, why, sizeof(why)) : -2;
            check(old == 0 && strstr(why, "comyn takes no answer; answer() replies to query, link5, arealist,"
                    " passinpu and prtyjoin") == why,
                "an engine abi 5 slot treats comyn as any window: answer5 refuses it, naming abi 5's prompts");
            check(raw.slot >= 0 && !sets_field(raw_read(api.options5, &raw, "comyn"), "hold")
                    && sets_field(raw_read(api.options8, &raw, "comyn"), "hold"),
                "options5 reads no question in comyn; options8 does");
            if (raw.slot >= 0) {
                api.handle_release(&raw);
            }
        }
        yl->closes[0] = '\0';
        const int calls = yl->yesno_calls;
        phase(L,
            "check(h:answer('comyn', true, comyn_prompt.id) == true, 'answer(comyn, true) with its prompt queues')\n"
            "return finish()\n");
        wait_frames(3);
        check(yl->yesno_calls == calls + 1 && yl->yesno_answer == 1 && yl->yesno_context == comyn
                && get32(comyn, 0x14) == 0 && !live_menu(kRowComyn) && strcmp(yl->closes, "holdtime|comyn   |") == 0
                && yl->yesno_inputs == 0,
            "answer(comyn, true): the callback once, with yes and the question's context; the question cleared,"
            " holdtime then comyn closed by name, as the class's confirm closes them; nothing reached the class's"
            " input routine");
        phase(L,
            "local o, why = h:options('comyn')\n"
            "check(o == nil and why == 'no question is pending in comyn', 'answered, comyn asks nothing')\n"
            "check(yesno_events(h) == 'opened:comyn closed:comyn', 'one opened event for the question, and its'"
            "  .. ' closed once answered')\n"
            "return finish()\n");

        // a default of yes, and the cancel
        check(ask_on_game_thread("comyn", 1, 1, 0, 0) == 1 && get16(live_menu(kRowComyn), 0x4C) == 1,
            "comyn asks again, its default yes: the cursor stays on yes");
        wait_frames(2);
        yl->closes[0] = '\0';
        phase(L,
            "local o = h:options('comyn')\n"
            "check(o and o.default == true and o.id == comyn_prompt.id + 1, 'options(comyn): yes first, a new id')\n"
            "local ok, w = h:answer('comyn', false, comyn_prompt.id)\n"
            "check(ok == nil and w == 'that prompt is gone', 'answer with the last question\\'s id: ' .. tostring(w))\n"
            "check(h:cancel(o.name, o.id) == true, 'cancel(comyn) queues: the question is cancellable')\n"
            "return finish()\n");
        wait_frames(3);
        check(yl->yesno_calls == calls + 2 && yl->yesno_answer == 0 && get32(comyn, 0x14) == 0
                && !live_menu(kRowComyn) && strcmp(yl->closes, "comyn   |holdtime|") == 0,
            "cancel(comyn): the callback with no, then comyn and holdtime closed, the cancel's order");

        // merityn: built not cancellable
        check(ask_on_game_thread("merityn", 0, 1, 0, 0) == 1, "merityn asks");
        wait_frames(2);
        phase(L,
            "local o = h:options('merityn')\n"
            "check(o and o.text == 'Spend the merit points?' and o.cancellable == false and #o.segments == 1,"
            "  'options(merityn): its class is built not cancellable')\n"
            "local ok, w = h:cancel(o.name, o.id)\n"
            "check(ok == nil and w == 'this question cannot be canceled; answer it', 'cancel(merityn): ' .. tostring(w))\n"
            "check(h:answer(o.name, false, o.id) == true, 'answer(merityn, false) queues')\n"
            "return finish()\n");
        wait_frames(3);
        check(yl->yesno_calls == calls + 3 && yl->yesno_answer == 0 && !live_menu(row_named("merityn")),
            "answer(merityn, false): the callback with no, the window closed");

        // the resyn family: one controller, keyed to the client's language
        uint8_t* family_menu = ask_on_game_thread("resyn", 1, 1, 0, 0) == 1 ? live_menu(kRowResynus) : NULL;
        const uint8_t* family_res = family_menu ? reinterpret_cast<const uint8_t*>(get32(family_menu, 4)) : NULL;
        check(family_res && memcmp(family_res + 0x46, "menu    resynus ", 16) == 0,
            "the resyn family's one controller asks through the key it holds: the window that opens is resynus");
        wait_frames(2);
        phase(L,
            "local o, why = h:options('resyn')\n"
            "check(o == nil and why == 'the game asks through resynus, not resyn', 'options(resyn): ' .. tostring(why))\n"
            "local ok, w = h:answer('resynde', true)\n"
            "check(ok == nil and w == 'the game asks through resynus, not resynde', 'answer(resynde): ' .. tostring(w))\n"
            "o = h:options('resynus')\n"
            "check(o and o.name == 'resynus' and o.text == 'Return to your home point?' and o.default == true"
            "  and o.cancellable == false, 'options(resynus): the question, yes first, not cancellable')\n"
            "check(h:answer(o.name, true, o.id) == true, 'answer(resynus, true) queues')\n"
            "return finish()\n");
        wait_frames(3);
        check(yl->yesno_calls == calls + 4 && yl->yesno_answer == 1 && yl->yesno_context == resyn
                && get32(resyn, 0x14) == 0 && !live_menu(kRowResynus),
            "answer(resynus, true): the shared controller's callback with yes, the window closed");

        // a question asked again from inside the callback, its default no
        check(ask_on_game_thread("comyn", 1, 1, 0, 1) == 1, "comyn asks, its callback asking again");
        wait_frames(2);
        uint8_t* asked_first = live_menu(kRowComyn);
        const int selects2 = yl->select_calls;
        phase(L,
            "chained = h:options('comyn')\n"
            "check(chained and chained.default == true and h:answer('comyn', true, chained.id) == true, 'answer(comyn, true) queues')\n"
            "return finish()\n");
        wait_frames(3);
        uint8_t* again = live_menu(kRowComyn);
        check(yl->yesno_calls == calls + 5 && yl->yesno_answer == 1 && get32(comyn, 0x14) != 0 && comyn[0x3A] == 0
                && again && again != asked_first && get16(again, 0x4C) == 2 && yl->select_calls == selects2 + 1
                && yl->select_menu == again,
            "the callback asked again: comyn closed and opened anew on that question, the chained flag cleared,"
            " the cursor on no, its default");
        phase(L,
            "local o = h:options('comyn')\n"
            "check(o and o.text == 'Really?' and o.default == false and o.id == chained.id + 1,"
            "  'options(comyn): the question asked again, a new prompt with a new id')\n"
            "local ok, w = h:answer('comyn', true, chained.id)\n"
            "check(ok == nil and w == 'that prompt is gone', 'the first question\\'s prompt is gone: ' .. tostring(w))\n"
            "check(h:answer(o.name, false, o.id) == true, 'answer to the question asked again queues')\n"
            "return finish()\n");
        wait_frames(3);
        check(yl->yesno_calls == calls + 6 && yl->yesno_answer == 0 && get32(comyn, 0x14) == 0 && !live_menu(kRowComyn),
            "it ends as any question does");

        // asked again with its default yes: no select-row
        check(ask_on_game_thread("comyn", 1, 1, 0, 2) == 1, "comyn asks, its callback asking again, yes first");
        wait_frames(2);
        const int selects3 = yl->select_calls;
        phase(L,
            "check(h:answer('comyn', false) == true, 'answer(comyn, false) queues')\n"
            "return finish()\n");
        wait_frames(3);
        check(yl->yesno_answer == 0 && get32(comyn, 0x14) != 0 && live_menu(kRowComyn)
                && get16(live_menu(kRowComyn), 0x4C) == 1 && yl->select_calls == selects3,
            "a question asked again with its default yes opens with the cursor on yes, no select-row");
        phase(L,
            "local o = h:options('comyn')\n"
            "check(o and o.default == true and h:cancel(o.name, o.id) == true, 'it reads yes first, and cancel queues')\n"
            "return finish()\n");
        wait_frames(3);
        check(get32(comyn, 0x14) == 0 && !live_menu(kRowComyn), "and its cancel ends it");

        // confirmed by holding: the reply is the confirm holdtime runs into
        check(ask_on_game_thread("comyn", 0, 1, 1, 0) == 1, "comyn asks, its yes held to confirm (mode 1)");
        wait_frames(2);
        yl->closes[0] = '\0';
        phase(L,
            "local o = h:options('comyn')\n"
            "check(o and o.hold == true and o.detail.mode == 1, 'options(comyn): hold, mode 1 under detail')\n"
            "check(h:answer(o.name, true, o.id) == true, 'answer(comyn, true) queues')\n"
            "return finish()\n");
        wait_frames(3);
        check(yl->yesno_answer == 1 && get32(comyn, 0x14) == 0 && !live_menu(kRowComyn)
                && strcmp(yl->closes, "holdtime|comyn   |") == 0,
            "answer(comyn, true) held to confirm: the callback with yes, holdtime and comyn closed");

        // hidden, it asks: no click, no key, no menu mouse mode
        {
            typedef uint32_t (__fastcall* InputFn)(void* self, void* edx, int code);
            typedef void (__fastcall* MouseFn)(void* mouse, void* edx);
            InputFn route;
            MouseFn mouse_mode;
            memcpy(&route, &g_fake->menu_routing, sizeof(route));
            memcpy(&mouse_mode, &g_fake->mouse_mode, sizeof(mouse_mode));
            static uint8_t mouse[0x60];
            phase(L,
                "check(h:hide('comyn') == true, 'hide(comyn) queues')\n"
                "return finish()\n");
            wait_frames(3);
            check(ask_on_game_thread("comyn", 0, 1, 0, 0) == 1, "comyn asks while hidden");
            uint8_t* menu = live_menu(kRowComyn);
            const int sinks = yl->sink_calls;
            const LONG ups = *g_fake->menu_up_calls;
            put32(g_fake->mcb, 0x54, reinterpret_cast<uint32_t>(menu));
            bool dropped = true;
            for (int code = 1; code <= 0x20; ++code) {
                dropped = route(g_fake->mcb, NULL, code) == 0 && dropped;
            }
            mouse[0x4D] = 2;
            mouse_mode(mouse, NULL);
            check(menu && menu[0x77] == 0 && dropped && yl->sink_calls == sinks && mouse[0x4D] == 1
                    && *g_fake->menu_up_calls == ups && hidden_in_place(kRowComyn, menu),
                "a hidden prompt active at its own layer: +0x77 0, no routed code reaches its sink, the picker holds"
                " world mode, and its draw paints nothing");
            put32(g_fake->mcb, 0x54, 0);
            phase(L,
                "check(h:answer('comyn', false) == true and h:unhide('comyn') == true, 'answer and unhide queue')\n"
                "return finish()\n");
            wait_frames(3);
            check(!live_menu(kRowComyn) && registry_row(kRowComyn)[0x29] == g_fake->pristine[kRowComyn * 0x2C + 0x29],
                "hidden, it is answered as any question; its row's layer the table's throughout");
        }

        // blocked (shopbuy; comyn is hide-only): an open question is left to
        // its reply; a new one is refused
        uint8_t* shop = controller_named("shopbuy");
        const int kRowShopbuy = row_named("shopbuy");
        check(ask_on_game_thread("shopbuy", 0, 1, 0, 0) == 1, "shopbuy asks");
        phase(L,
            "local ok, w = h:block('comyn')\n"
            "check(ok == nil and w:find('comyn cannot be blocked or closed: ', 1, true) == 1, 'block(comyn) refused: '"
            "  .. tostring(w))\n"
            "check(h:block('shopbuy') == true, 'block(shopbuy) with a question up queues nothing to close')\n"
            "return finish()\n");
        wait_frames(3);
        check(hidden_in_place(kRowShopbuy, live_menu(kRowShopbuy)) && get32(shop, 0x14) != 0,
            "block(shopbuy): the open question stays, hidden at its own layer, for its reply");
        phase(L,
            "check(h:answer('shopbuy', true) == true, 'the blocked window\\'s question is answered')\n"
            "return finish()\n");
        wait_frames(3);
        const int blocked_calls = yl->yesno_calls;
        check(!live_menu(kRowShopbuy) && ask_on_game_thread("shopbuy", 0, 1, 0, 0) == 0 && get32(shop, 0x14) == 0
                && !live_menu(kRowShopbuy) && yl->yesno_calls == blocked_calls,
            "blocked, a new question's open is refused: the ask answers 0 and clears its callback, unanswered");
        phase(L,
            "local o, why = h:options('shopbuy')\n"
            "check(o == nil and why == 'no question is pending in shopbuy', 'nothing is pending after the refusal')\n"
            "local seen = yesno_events(h)\n"
            "check(seen:find('blocked:shopbuy', 1, true) ~= nil, 'the refused open is a blocked event: ' .. seen)\n"
            "check(h:unblock('shopbuy') == true, 'unblock(shopbuy)')\n"
            "for _, x in ipairs({h, h2}) do x:poll() end\n"
            "return finish()\n");

        // the hide-only windows: refused by every block slot and every close
        // slot, whatever its engine abi, each with its own reason
        phase(L,
            "local blocks = {}\n"
            "for _, w in ipairs({'query', 'gmtell', 'comyn', 'dead', 'mp_dead', 'resyn', 'resynus', 'resynde', 'resynfr'})"
            " do\n"
            "  local ok, why = h:block(w)\n"
            "  if ok == nil and why:find(w .. ' cannot be blocked or closed', 1, true) == 1 then blocks[#blocks + 1] = w end\n"
            "end\n"
            "check(#blocks == 9, 'block() refuses the nine hide-only windows, naming each: ' .. table.concat(blocks, ' '))\n"
            "local ok, why = h:block('resynde')\n"
            "check(ok == nil and why == 'resynde cannot be blocked or closed: blocked, the raise question is never shown,"
            " so the player cannot accept a raise, and the game asks again every frame; hide it instead',"
            "  'block(resynde): the raise question\\'s reason: ' .. tostring(why))\n"
            "ok, why = h:close('dead')\n"
            "check(ok == nil and why:find('dead cannot be blocked or closed: closing the KO window during a raise offer"
            " declines the raise', 1, true) == 1 and why:find('; hide it instead', 1, true),"
            "  'close(dead): its own reason, not cancel(): ' .. tostring(why))\n"
            "ok, why = h:close('gmtell')\n"
            "local gm = ok == nil and why == \"gmtell is a prompt the game waits on; use cancel('gmtell')\"\n"
            "ok, why = h:close('mp_dead')\n"
            "check(gm and ok == nil and why:find('mp_dead cannot be blocked or closed', 1, true) == 1,"
            "  'close(gmtell) points at cancel(), a prompt cancel() ends; close(mp_dead) refused with its own"
            " reason')\n"
            "ok, why = h:open('dead')\n"
            "check(ok == nil and why == \"dead opens only for the game's own event; answer it instead\","
            "  'open(dead) refused: ' .. tostring(why))\n"
            "return finish()\n");
        {
            HuEngineApi api;
            HuEngineHandle raw = {-1, 0};
            char why[512] = "";
            if (record_api(&api)) {
                api.handle_open("raw", &raw, why, sizeof(why));
            }
            static const char* const only[] = {"gmtell", "comyn", "dead", "mp_dead", "resyn", "resynus", "resynde",
                                               "resynfr", "rem4line", "rem4li2", "stringdl"};
            int blocks = 0;
            int closes = 0;
            for (int i = 0; raw.slot >= 0 && i < 11; ++i) {
                char want[32];
                snprintf(want, sizeof(want), "%s cannot be blocked or closed", only[i]);
                const int32_t refused[5] = {
                    api.block(&raw, only[i], why, sizeof(why)) == 0 && strstr(why, want) == why,
                    api.block3(&raw, only[i], why, sizeof(why)) == 0 && strstr(why, want) == why,
                    api.block5(&raw, only[i], why, sizeof(why)) == 0 && strstr(why, want) == why,
                    api.block7(&raw, only[i], NULL, why, sizeof(why)) == 0 && strstr(why, want) == why,
                    api.block8(&raw, only[i], NULL, why, sizeof(why)) == 0 && strstr(why, want) == why,
                };
                for (int k = 0; k < 5; ++k) {
                    blocks += refused[k];
                }
                closes += api.close(&raw, only[i], why, sizeof(why)) == 0 && strstr(why, want) == why;
                closes += api.close3(&raw, only[i], why, sizeof(why)) == 0 && strstr(why, want) == why;
                closes += api.close4(&raw, only[i], why, sizeof(why)) == 0 && strstr(why, want) == why;
                const bool prompt = strcmp(only[i], "comyn") == 0 || strcmp(only[i], "gmtell") == 0
                    || strncmp(only[i], "resyn", 5) == 0 || strncmp(only[i], "rem4", 4) == 0
                    || strcmp(only[i], "stringdl") == 0;
                char use[64];
                snprintf(use, sizeof(use), "%s is a prompt the game waits on; use cancel('%s')", only[i], only[i]);
                closes += api.close8(&raw, only[i], why, sizeof(why)) == 0 && strstr(why, prompt ? use : want) == why;
            }
            check(blocks == 55 && closes == 44,
                "gmtell, comyn, dead, mp_dead, the four resyn windows, the two dialogue boxes and stringdl refused by"
                " the block slots of engine abi 1, 3, 5, 7 and 8 and the close slots of abi 1, 3, 4 and 8, each with"
                " its reason (close8 sends gmtell, comyn, the resyn windows, the dialogue boxes and stringdl, prompts"
                " cancel() ends, to cancel())");
            static const char kOk[] = "ok cannot be closed: the game waits until it is acknowledged, and closed, it"
                " waits forever; answer('ok') acknowledges it";
            int ok_closes = 0;
            if (raw.slot >= 0) {
                ok_closes += api.close(&raw, "ok", why, sizeof(why)) == 0 && strcmp(why, kOk) == 0;
                ok_closes += api.close3(&raw, "ok", why, sizeof(why)) == 0 && strcmp(why, kOk) == 0;
                ok_closes += api.close4(&raw, "ok", why, sizeof(why)) == 0 && strcmp(why, kOk) == 0;
                ok_closes += api.close8(&raw, "ok", why, sizeof(why)) == 0 && strcmp(why, kOk) == 0;
            }
            const bool ok_blocks = raw.slot >= 0 && api.block8(&raw, "ok", NULL, why, sizeof(why)) == 1
                && api.unblock3(&raw, "ok", why, sizeof(why)) == 1;
            check(ok_closes == 4 && ok_blocks,
                "close('ok') refused by the close slots of engine abi 1, 3, 4 and 8 with its own reason, pointing at"
                " answer('ok'); a block of it still taken");
            const int32_t answered = raw.slot >= 0
                ? api.answer5(&raw, "dead", HU_ANSWER_NONE, 0, NULL, 0, 0, why, sizeof(why)) : -2;
            check(answered == 0 && strstr(why, "dead takes no answer; answer() replies to query, link5, arealist,"
                    " passinpu and prtyjoin") == why
                    && !sets_field(raw_read(api.options5, &raw, "dead"), "seconds"),
                "an engine abi 5 slot treats dead as any window: answer5 and options5 refuse it");
            if (raw.slot >= 0) {
                api.handle_release(&raw);
            }
        }

        // the KO window: options() reads its countdown; answer() is its own
        // confirm, asking its question through comyn with the arguments its
        // input handler passes; cancel() and close() refused
        {
            uint8_t* ko = controller_named("dead");
            const int kRowDead = row_named("dead");
            phase(L,
                "local o, why = h:options('dead')\n"
                "check(o == nil and why == 'dead is not open', 'options(dead) with the KO window down: ' .. tostring(why))\n"
                "local ok, w = h:answer('dead')\n"
                "check(ok == nil and w == 'dead is not open', 'answer(dead) with the KO window down: ' .. tostring(w))\n"
                "return finish()\n");
            put32(ko, 0x1C, 54321);
            put32(ko, 0x20, 125 * 60);
            ko[0x24] = 0;
            uint8_t* ko_menu = static_cast<uint8_t*>(open_on_game_thread("menu    dead"));
            wait_frames(2);
            phase(L,
                "dead_prompt = h:options('dead')\n"
                "local o = dead_prompt\n"
                "check(o and o.name == 'dead' and type(o.id) == 'number' and o.seconds == 125 and o.askable == true"
                "  and o.detail and o.detail.remaining == 7500 and o.detail.deadline == 54321"
                "  and o.detail.chosen == false and o.text == nil,"
                "  'options(dead): the countdown in seconds, askable, the raw fields under detail')\n"
                "local misuse = {}\n"
                "for _, v in ipairs({false, 1, 'yes', {}}) do\n"
                "  local ok, err = pcall(h.answer, h, 'dead', v)\n"
                "  if not ok and tostring(err):find(\"answer('dead') takes no value, or true\", 1, true) then"
                "    misuse[#misuse + 1] = type(v) end\n"
                "end\n"
                "check(#misuse == 4, 'answer(dead) with false, a number, a string or a table raises: '"
                "  .. table.concat(misuse, ' '))\n"
                "local ok, w = h:cancel('dead')\n"
                "check(ok == nil and w == \"dead has no cancel: the game's own Escape there only hands focus to the chat"
                " log\", 'cancel(dead) refused: ' .. tostring(w))\n"
                "return finish()\n");
            // a question pending in comyn: the ask would answer 0
            check(ask_on_game_thread("comyn", 0, 1, 0, 0) == 1, "comyn asks while the KO window is up");
            wait_frames(2);
            phase(L,
                "local o = h:options('dead')\n"
                "local ok, w = h:answer('dead')\n"
                "check(o and o.askable == false and ok == nil and w == 'comyn already has a question pending',"
                "  'with comyn asking, options(dead).askable is false and answer(dead) refused: ' .. tostring(w))\n"
                "check(h:answer('comyn', false) == true, 'comyn answered')\n"
                "return finish()\n");
            wait_frames(3);
            const int deciders = yl->decider_calls;
            const int messages = yl->message_calls;
            const int asks = yl->ask_calls;
            const int selects4 = yl->select_calls;
            *g_fake->dead_set = 0;
            phase(L,
                "check(h:answer('dead', nil, dead_prompt.id) == true, 'answer(dead) with its prompt\\'s id and no value"
                " queues')\n"
                "return finish()\n");
            wait_frames(3);
            uint8_t* question = live_menu(kRowComyn);
            const int pushed[5] = {0, 0, 0, 1, 0};
            check(yl->decider_calls == deciders + 1 && yl->decider_args[0] == 1 && yl->decider_args[1] == 0
                    && yl->message_calls == messages + 1 && yl->message_args[0] == 0x0E && yl->message_args[1] == 0x46
                    && yl->ask_calls == asks + 1 && yl->ask_ctl == comyn && yl->ask_callback == g_fake->dead_callback
                    && yl->ask_text && strcmp(yl->ask_text, "Return to home point?") == 0
                    && memcmp(yl->ask_args, pushed, sizeof(pushed)) == 0 && question && get16(question, 0x4C) == 2
                    && yl->select_calls == selects4 + 1 && get32(comyn, 0x14) == get32(g_fake->dead_site, 53)
                    && ko[0x24] == 0 && live_menu(kRowDead) == ko_menu,
                "answer(dead): the site's decider with (1, 0), its 0 picking message 0x46 of category 0x0E, then"
                " comyn's ask with the site's callback, context 0, no first, no help, cancellable, mode 0: comyn asks,"
                " its cursor on no; the KO window left as it was");
            phase(L,
                "local o = h:options('comyn')\n"
                "check(o and o.text == 'Return to home point?' and o.default == false and o.cancellable == true,"
                "  'options(comyn): the KO window\\'s question')\n"
                "local d = h:options('dead')\n"
                "check(d and d.askable == false, 'options(dead).askable is false while its question waits')\n"
                "check(h:answer(o.name, true, o.id) == true, 'answer(comyn, true) queues')\n"
                "return finish()\n");
            wait_frames(3);
            check(ko[0x24] == 1 && !live_menu(kRowComyn) && get32(comyn, 0x14) == 0,
                "comyn's yes runs the KO window's callback, the game's own bytes: the home point chosen");
            phase(L,
                "local d = h:options('dead')\n"
                "check(d and d.detail.chosen == true and d.askable == true, 'options(dead): chosen under detail')\n"
                "return finish()\n");
            ko[0x24] = 0;
            *g_fake->dead_set = 1;
            phase(L,
                "local d = h:options('dead')\n"
                "check(h:answer('dead', true, d.id) == true, 'answer(dead, true) with its id queues')\n"
                "return finish()\n");
            wait_frames(3);
            check(yl->message_args[0] == 0x0E && yl->message_args[1] == 0x258 && yl->ask_text
                    && strcmp(yl->ask_text, "Leave this area?") == 0 && get32(comyn, 0x14) != 0,
                "the decider's 1, al alone, picks message 0x258");
            phase(L,
                "check(h:cancel('comyn') == true, 'cancel(comyn) queues')\n"
                "return finish()\n");
            wait_frames(3);
            check(ko[0x24] == 0 && !live_menu(kRowComyn) && get32(comyn, 0x14) == 0,
                "the question canceled through comyn: the home point not chosen");
            *g_fake->dead_set = 0;

            // hidden, the KO window takes no routed key: Enter on it asks nothing
            {
                typedef uint32_t (__fastcall* InputFn)(void* self, void* edx, int code);
                InputFn route;
                memcpy(&route, &g_fake->menu_routing, sizeof(route));
                phase(L,
                    "check(h:hide('dead') == true, 'hide(dead) queues')\n"
                    "return finish()\n");
                wait_frames(3);
                const int sinks = yl->sink_calls;
                put32(g_fake->mcb, 0x54, reinterpret_cast<uint32_t>(ko_menu));
                bool dropped = true;
                for (int code = 1; code <= 0x20; ++code) {
                    dropped = route(g_fake->mcb, NULL, code) == 0 && dropped;
                }
                put32(g_fake->mcb, 0x54, 0);
                check(hidden_in_place(kRowDead, ko_menu) && dropped && yl->sink_calls == sinks,
                    "a hidden KO window active at its own layer: no routed code reaches its sink");
                phase(L,
                    "check(h:unhide('dead') == true, 'unhide(dead)')\n"
                    "return finish()\n");
                wait_frames(3);
            }

            typedef int (__fastcall* CloseFn)(void* mcb, void* edx, const char* key);
            CloseFn close_by_name;
            memcpy(&close_by_name, &g_fake->close, sizeof(close_by_name));
            pause_game();
            close_by_name(g_fake->mcb, NULL, "menu    dead    ");
            resume_game();
            wait_frames(2);
            phase(L,
                "local o, why = h:options('dead')\n"
                "check(o == nil and why == 'dead is not open', 'the KO window closed by the game: nothing to read')\n"
                "for _, x in ipairs({h, h2}) do x:poll() end\n"
                "return finish()\n");
        }
    }

    // -- the number boxes (0.11.0): moneyctr and itemctrl asked through their
    //    setups, the game's bytes, on the game thread; answered and canceled
    //    through their confirms, the game's bytes, with the window up, hidden
    //    and blocked; a blocked question announced, listed by pending() and
    //    ended with a pending event every other handle reads; the asker's row
    //    checked; the earlier slots treating the boxes as any window
    {
        FakeLog* bl = g_fake->log;
        uint8_t* money = controller_named("moneyctr");
        uint8_t* item = controller_named("itemctrl");
        const int kRowMoney = row_named("moneyctr");
        const int kRowItem = row_named("itemctrl");
        put32(g_fake->gil_record, 4, 7000);
        phase(L,
            "for _, x in ipairs({h, h2}) do x:poll() end\n"
            "function box_events(handle)\n"
            "  local out = {}\n"
            "  for _, e in ipairs((handle:poll())) do\n"
            "    if e.event == 'pending' then out[#out + 1] = 'pending:' .. e.what .. ':' .. tostring(e.pending)\n"
            "    elseif e.name == 'moneyctr' or e.name == 'itemctrl' then out[#out + 1] = e.event .. ':' .. e.name end\n"
            "  end\n"
            "  return table.concat(out, ' ')\n"
            "end\n"
            "local o, why = h:options('moneyctr')\n"
            "check(o == nil and why == 'no number is pending in moneyctr', 'options(moneyctr) with nothing asked: '"
            "  .. tostring(why))\n"
            "local ok, w = h:open('itemctrl')\n"
            "check(ok == nil and w == \"itemctrl opens only for the game's own event; answer or cancel it instead\","
            "  'open(itemctrl) refused: ' .. tostring(w))\n"
            "ok, w = h:close('moneyctr')\n"
            "check(ok == true, 'close(moneyctr) with nothing asked queues: the game thread refuses it only while it'"
            "  .. ' asks: ' .. tostring(w))\n"
            "local raised, err = pcall(h.answer, h, 'moneyctr', true)\n"
            "check(not raised and tostring(err):find(\"answer('moneyctr', number) takes a number\", 1, true),"
            "  'answer(moneyctr, true) raises: ' .. tostring(err))\n"
            "ok, w = h:answer('itemctrl', 3)\n"
            "check(ok == nil and w == 'no number is pending in itemctrl', 'answer(itemctrl) with nothing asked: '"
            "  .. tostring(w))\n"
            "ok, w = h:cancel('moneyctr')\n"
            "check(ok == nil and w == 'no number is pending in moneyctr', 'cancel(moneyctr) with nothing asked')\n"
            "return finish()\n");
        wait_frames(3);
        phase(L,
            "local seen = {}\n"
            "for _, e in ipairs((h:poll())) do seen[#seen + 1] = e.event .. ':' .. tostring(e.name) end\n"
            "check(#seen == 0, 'the close of moneyctr asking nothing: no refusal, nothing to close: '"
            "  .. table.concat(seen, ' '))\n"
            "return finish()\n");

        // the window up: handover's gil
        open_on_game_thread("menu    handover");
        uint8_t* box = static_cast<uint8_t*>(box_ask_on_game_thread(0, 100, 0, g_fake->callbacks[1], NULL));
        wait_frames(2);
        check(box && live_menu(kRowMoney) == box && get32(money, 0x24) == 7000 && get32(money, 0x28) == 100
                && get32(money, 0x2C) == reinterpret_cast<uint32_t>(g_fake->callbacks[1]),
            "moneyctr's setup, the game's bytes: a max of 0 is the gil record's, the question written, the window"
            " opened through the hooked open");
        phase(L,
            "local o = h:options('moneyctr')\n"
            "check(o and o.name == 'moneyctr' and type(o.id) == 'number' and o.value == 100 and o.min == 0"
            "  and o.max == 7000 and o.blocked == false and o.item == nil and o.detail"
            "  and o.detail.asker == 'handover' and type(o.detail.callback) == 'string',"
            "  'options(moneyctr): the number shown, 0..the gil, not blocked, handover asking under detail')\n"
            "local ok, w = h:answer('moneyctr', 1.5)\n"
            "check(ok == nil and w == \"answer('moneyctr', number): the number is a whole number\", 'a fraction: '"
            "  .. tostring(w))\n"
            "ok, w = h:answer('moneyctr', 7001)\n"
            "check(ok == nil and w == '7001 is outside 0..7000, what moneyctr takes now', 'past the max: ' .. tostring(w))\n"
            "ok, w = h:answer(o.name, 2500, o.id - 1)\n"
            "check(ok == nil and w == 'that prompt is gone', 'an earlier open\\'s id: ' .. tostring(w))\n"
            "box_prompt = o\n"
            "check(h:close('moneyctr') == true, 'close(moneyctr) while it asks queues')\n"
            "check(h:answer(o.name, 2500, o.id) == true, 'answer(moneyctr, 2500, its id) queues')\n"
            "return finish()\n");
        const int answers = bl->box_answers;
        wait_frames(3);
        check(bl->box_answers == answers + 1 && bl->box_value == 2500 && bl->box_who == 1 && bl->box_context == NULL
                && get32(money, 0x2C) == 0 && !live_menu(kRowMoney),
            "answer: moneyctr's confirm, the game's bytes, called the asker's callback with 2500, cleared it and"
            " closed the window by instance");
        phase(L,
            "local seen, refused = {}, {}\n"
            "for _, e in ipairs((h:poll())) do\n"
            "  if e.event == 'pending' then seen[#seen + 1] = 'pending:' .. e.what .. ':' .. tostring(e.pending)\n"
            "  elseif e.name == 'moneyctr' then seen[#seen + 1] = e.event .. ':' .. e.name end\n"
            "  if e.event == 'error' then refused[#refused + 1] = e.verb .. ':' .. e.reason end\n"
            "end\n"
            "check(table.concat(seen, ' ') == 'opened:moneyctr error:moneyctr closed:moneyctr'"
            "  and table.concat(refused, ' ') == \"close:moneyctr is a prompt the game waits on; use cancel('moneyctr')\","
            "  'a box with its window posts no pending event, its opened event describing it, then its closed (the'"
            "  .. ' stand-in frees a closed instance at once, and the drain sees it gone); the close while it asked'"
            "  .. ' refused by the game thread, before the answer: ' .. table.concat(seen, ' ') .. ' / '"
            "  .. table.concat(refused, ' '))\n"
            "return finish()\n");

        // hidden: no key reaches it; the reply ends it all the same
        box = static_cast<uint8_t*>(box_ask_on_game_thread(0, 0, 50, g_fake->callbacks[1], NULL));
        phase(L,
            "check(h:hide('moneyctr') == true, 'hide(moneyctr) queues')\n"
            "return finish()\n");
        wait_frames(3);
        {
            typedef uint32_t (__fastcall* InputFn)(void* self, void* edx, int code);
            InputFn route;
            memcpy(&route, &g_fake->menu_routing, sizeof(route));
            const int sinks = bl->sink_calls;
            put32(g_fake->mcb, 0x54, reinterpret_cast<uint32_t>(box));
            bool dropped = true;
            for (int code = 1; code <= 0x20; ++code) {
                dropped = route(g_fake->mcb, NULL, code) == 0 && dropped;
            }
            put32(g_fake->mcb, 0x54, 0);
            check(hidden_in_place(kRowMoney, box) && dropped && bl->sink_calls == sinks,
                "a hidden moneyctr active at its own layer: no routed code reaches it");
        }
        phase(L,
            "check(h:cancel('moneyctr') == true, 'cancel(moneyctr) queues')\n"
            "check(h:unhide('moneyctr') == true, 'unhide(moneyctr)')\n"
            "return finish()\n");
        wait_frames(3);
        check(bl->box_value == -1 && !live_menu(kRowMoney) && get32(money, 0x2C) == 0,
            "cancel: the confirm with -1, the window's own cancel");

        // a stale callback: the window closed from outside asks nothing
        box_ask_on_game_thread(0, 0, 50, g_fake->callbacks[1], NULL);
        {
            typedef int (__fastcall* CloseFn)(void* mcb, void* edx, const char* key);
            CloseFn close_by_name;
            memcpy(&close_by_name, &g_fake->close, sizeof(close_by_name));
            pause_game();
            close_by_name(g_fake->mcb, NULL, "menu    moneyctr");
            resume_game();
        }
        wait_frames(2);
        phase(L,
            "local o, why = h:options('moneyctr')\n"
            "check(o == nil and why == 'no number is pending in moneyctr', 'a callback a close from outside left set"
            " asks nothing: ' .. tostring(why))\n"
            "return finish()\n");
        check(get32(money, 0x2C) != 0, "the stand-in's callback is still set, as the game leaves it");
        put32(money, 0x2C, 0);

        // the earlier slots: any window; block8 leaves an open box to its
        // reply, block5 closes it
        box = static_cast<uint8_t*>(box_ask_on_game_thread(0, 0, 50, g_fake->callbacks[1], NULL));
        wait_frames(2);
        {
            HuEngineApi api;
            HuEngineHandle raw = {-1, 0};
            char why[512] = "";
            if (record_api(&api)) {
                api.handle_open("raw", &raw, why, sizeof(why));
            }
            const int32_t old = raw.slot >= 0
                ? api.answer5(&raw, "moneyctr", HU_ANSWER_NUMBER, 5, NULL, 0, 0, why, sizeof(why)) : -2;
            check(old == 0 && strstr(why, "moneyctr takes no answer; answer() replies to query, link5, arealist,"
                    " passinpu and prtyjoin") == why
                    && !sets_field(raw_read(api.options5, &raw, "moneyctr"), "max")
                    && sets_field(raw_read(api.options8, &raw, "moneyctr"), "max"),
                "an engine abi 5 slot treats moneyctr as any window; options8 reads its question");
            const int32_t blocked = raw.slot >= 0 ? api.block8(&raw, "moneyctr", NULL, why, sizeof(why)) : 0;
            wait_frames(3);
            const bool kept = blocked == 1 && live_menu(kRowMoney) == box && hidden_in_place(kRowMoney, box);
            api.unblock3(&raw, "moneyctr", why, sizeof(why));
            const int32_t old_block = raw.slot >= 0 ? api.block5(&raw, "moneyctr", why, sizeof(why)) : 0;
            wait_frames(3);
            check(kept && old_block == 1 && !live_menu(kRowMoney) && get32(money, 0x2C) != 0,
                "block8 leaves an open moneyctr to its reply; block5 closes it as any window, its callback left set");
            api.unblock3(&raw, "moneyctr", why, sizeof(why));
            if (raw.slot >= 0) {
                api.handle_release(&raw);
            }
        }
        put32(money, 0x2C, 0);
        wait_frames(2);

        // blocked: trade's item count, its callback the game's bytes
        uint8_t* trade = static_cast<uint8_t*>(open_on_game_thread("menu    trade"));
        uint8_t* trade_ctl = controller_named("trade");
        put32(trade_ctl, 0xC0, 3);
        put16(trade, 0x4C, 3);
        static uint8_t picked[8] = {0x34, 0x12, 7, 0, 12, 0, 0, 0};
        phase(L,
            "box_events(h) box_events(h2)\n"
            "check(h:block('itemctrl') == true, 'block(itemctrl) queues')\n"
            "return finish()\n");
        wait_frames(3);
        void* refused = box_ask_on_game_thread(1, 1, 12, g_fake->callbacks[8], picked);
        wait_frames(2);
        check(!refused && !live_menu(kRowItem) && item[0x22] == 12 && item[0x23] == 1
                && get32(item, 0x24) == reinterpret_cast<uint32_t>(g_fake->callbacks[8])
                && get32(item, 0x2C) == reinterpret_cast<uint32_t>(picked),
            "itemctrl blocked: its setup wrote the question and its open was refused");
        phase(L,
            "local seen = box_events(h)\n"
            "check(seen == 'blocked:itemctrl pending:itemctrl:true', 'the refused open: a blocked event, then a pending"
            " one, what itemctrl: ' .. seen)\n"
            "local o = h:options('itemctrl')\n"
            "check(o and o.name == 'itemctrl' and o.value == 1 and o.min == 1 and o.max == 12 and o.blocked == true"
            "  and o.item == 0x1234 and o.detail and o.detail.asker == 'trade',"
            "  'options(itemctrl) blocked: the number, 1..12, blocked, the picked item\\'s id')\n"
            "local p = h:pending()\n"
            "check(p and p.itemctrl and p.itemctrl.name == 'itemctrl' and p.itemctrl.id == o.id and p.itemctrl.max == 12"
            "  and p.itemctrl.blocked == true and p.moneyctr == nil, 'pending() lists the blocked count under itemctrl')\n"
            "item_prompt = p.itemctrl\n"
            "return finish()\n");
        put16(trade, 0x4C, 4);
        phase(L,
            "local ok, w = h:answer(item_prompt.name, 5, item_prompt.id)\n"
            "check(ok == nil and w == \"trade's cursor is not on the row the question was asked from\", 'trade\\'s"
            " cursor moved: ' .. tostring(w))\n"
            "return finish()\n");
        put16(trade, 0x4C, 3);
        pause_game();
        put16(g_fake->player, 0x9860 + 7 * 0x2C, 0x1234);
        const uint32_t later = get32(g_fake->clock, 0x0C) + 2;
        put32(g_fake->clock, 0x0C, later);
        resume_game();
        const int commits = bl->commits;
        phase(L,
            "check(h:answer(item_prompt.name, 5, item_prompt.id) == true, 'back on its row: answer(itemctrl, 5, the id"
            " pending() gave) queues')\n"
            "return finish()\n");
        wait_frames(3);
        check(bl->commits == commits + 1 && get16(bl->packet, 0) == 0x34 && get32(bl->packet, 4) == 5
                && get16(bl->packet, 8) == 0x1234 && bl->packet[0x0A] == 7 && bl->packet[0x0B] == 3
                && bl->focus_menu == trade && get32(item, 0x24) == 0,
            "the answer: itemctrl's confirm with no window, trade's own callback staging 5 of item 0x1234 from index 7"
            " on its cursor row's slot 3, the change sent, the focus back to trade");
        phase(L,
            "local mine, other = box_events(h), box_events(h2)\n"
            "check(mine == '', 'the replying handle gets no pending event for its own reply: ' .. mine)\n"
            "check(other == 'blocked:itemctrl pending:itemctrl:true pending:itemctrl:false', 'another handle reads the"
            " question start and end: ' .. other)\n"
            "local p = h:pending()\n"
            "check(p and p.itemctrl == nil, 'pending() lists it no more')\n"
            "return finish()\n");

        // the game drops it: trade's window closes
        box_ask_on_game_thread(1, 1, 12, g_fake->callbacks[8], picked);
        wait_frames(2);
        {
            typedef int (__fastcall* CloseFn)(void* mcb, void* edx, const char* key);
            CloseFn close_by_name;
            memcpy(&close_by_name, &g_fake->close, sizeof(close_by_name));
            pause_game();
            close_by_name(g_fake->mcb, NULL, "menu    trade");
            resume_game();
        }
        wait_frames(3);
        phase(L,
            "local seen = box_events(h)\n"
            "check(seen == 'blocked:itemctrl pending:itemctrl:true pending:itemctrl:false', 'trade\\'s window closing"
            " ends the question: ' .. seen)\n"
            "local o, why = h:options('itemctrl')\n"
            "check(o == nil and why == 'no number is pending in itemctrl', 'nothing pending once trade closed')\n"
            "box_events(h2)\n"
            "return finish()\n");

        // an asker the engine does not know: pending, but no reply
        box_ask_on_game_thread(1, 1, 9, g_fake->text_callback, picked);
        wait_frames(2);
        phase(L,
            "local o = h:options('itemctrl')\n"
            "check(o and o.blocked == true and o.detail.asker == 'unknown' and o.item == nil, 'options(itemctrl) of an"
            " unknown asker: blocked, asker unknown')\n"
            "local ok, w = h:answer(o.name, 2, o.id)\n"
            "check(ok == nil and w:find(\"itemctrl's question came from a window hideui does not know\", 1, true) == 1,"
            "  'answer refused: ' .. tostring(w))\n"
            "ok, w = h:cancel(o.name, o.id)\n"
            "check(ok == nil and w:find('does not know', 1, true), 'cancel refused: ' .. tostring(w))\n"
            "check(h:unblock('itemctrl') == true, 'unblock(itemctrl)')\n"
            "return finish()\n");
        wait_frames(3);
        // unblocked, the next ask opens the window, which ends the record
        box = static_cast<uint8_t*>(box_ask_on_game_thread(1, 1, 9, g_fake->callbacks[8], picked));
        wait_frames(3);
        phase(L,
            "local seen = box_events(h)\n"
            "check(seen == 'blocked:itemctrl pending:itemctrl:true pending:itemctrl:false opened:itemctrl',"
            "  'an open of the window ends the blocked question: ' .. seen)\n"
            "local o = h:options('itemctrl')\n"
            "check(o and o.blocked == false and o.item == 0x1234, 'the window up: not blocked')\n"
            "local ok, w = h:answer(o.name, 99, o.id)\n"
            "check(ok == nil and w == 'trade, which asked, is not open', 'an answer past the max to a question whose'"
            "  .. ' asker\\'s window is closed: the asker\\'s refusal, checked first as the drain checks it: '"
            "  .. tostring(w))\n"
            "check(h:cancel(o.name, o.id) == true, 'cancel(itemctrl) queues')\n"
            "return finish()\n");
        wait_frames(3);
        check(box && !live_menu(kRowItem) && get32(item, 0x24) == 0 && bl->focus_calls > 0,
            "the cancel: the confirm with -1, trade's callback refocusing, the window closed");
        {
            HuEngineApi api;
            HuEngineHandle raw = {-1, 0};
            char why[512] = "";
            if (record_api(&api)) {
                api.handle_open("raw", &raw, why, sizeof(why));
            }
            phase(L,
                "check(h:block('moneyctr') == true, 'block(moneyctr)')\n"
                "return finish()\n");
            wait_frames(3);
            raw_read(api.poll5, &raw);
            box_ask_on_game_thread(0, 0, 50, g_fake->callbacks[1], NULL);
            wait_frames(2);
            const HuEngineReply& five = raw_read(api.pending5, &raw);
            const bool old_pending = !sets_field(five, "moneyctr");
            const bool new_pending = sets_field(raw_read(api.pending8, &raw), "moneyctr");
            const HuEngineReply& polled = raw_read(api.poll5, &raw);
            const bool old_poll = has_string(polled, "blocked") && !has_string(polled, "pending");
            check(raw.slot >= 0 && old_pending && new_pending && old_poll,
                "pending5 lists no box and poll5 reports no box's pending event; pending8 lists it");
            if (raw.slot >= 0) {
                api.handle_release(&raw);
            }
            phase(L,
                "local p = h:pending()\n"
                "check(p.moneyctr and p.moneyctr.blocked and h:cancel(p.moneyctr.name, p.moneyctr.id) == true,"
                "  'the blocked gil question pending() lists, canceled with its id')\n"
                "return finish()\n");
            wait_frames(3);
            check(bl->box_value == -1 && get32(money, 0x2C) == 0, "the blocked gil question canceled through its record");
            phase(L,
                "check(h:unblock('moneyctr') == true, 'unblock(moneyctr)')\n"
                "for _, x in ipairs({h, h2}) do x:poll() end\n"
                "return finish()\n");
            wait_frames(2);
        }
        {
            typedef int (__fastcall* CloseFn)(void* mcb, void* edx, const char* key);
            CloseFn close_by_name;
            memcpy(&close_by_name, &g_fake->close, sizeof(close_by_name));
            pause_game();
            close_by_name(g_fake->mcb, NULL, "menu    handover");
            resume_game();
            wait_frames(2);
        }
        phase(L,
            "for _, x in ipairs({h, h2}) do x:poll() end\n"
            "return finish()\n");
    }

    // -- the Asked windows (0.11.0): sortyn, evitem01, confyn, bankmenu,
    //    maplist, gmtell, mp_dead and ok asked on the game thread as the
    //    game's askers ask, their setups the game's bytes where the engine
    //    relies on them: options() in plain fields, answer and cancel doing
    //    what each window's own confirm and cancel do, the refusals, hidden
    //    prompts taking no routed key, a blocked question announced, listed by
    //    pending() and pending8, answered with no window, its end a pending
    //    event every other handle reads; pending5 and poll5 leaving them out
    {
        FakeLog* al = g_fake->log;
        typedef int (__fastcall* CloseFn)(void* mcb, void* edx, const char* key);
        CloseFn close_by_name;
        memcpy(&close_by_name, &g_fake->close, sizeof(close_by_name));
        auto game_close = [&](const char* key) {
            pause_game();
            close_by_name(g_fake->mcb, NULL, key);
            resume_game();
        };
        phase(L,
            "for _, x in ipairs({h, h2}) do x:poll() end\n"
            "asked_names = {sortyn = true, evitem01 = true, confyn = true, bankmenu = true, maplist = true,"
            "  gmtell = true, mp_dead = true, ok = true}\n"
            "function asked_events(handle)\n"
            "  local out = {}\n"
            "  for _, e in ipairs((handle:poll())) do\n"
            "    if e.event == 'pending' and asked_names[e.what] then\n"
            "      out[#out + 1] = 'pending:' .. e.what .. ':' .. tostring(e.pending)\n"
            "    elseif asked_names[e.name] and e.event ~= 'cursor' then out[#out + 1] = e.event .. ':' .. e.name end\n"
            "  end\n"
            "  return table.concat(out, ' ')\n"
            "end\n"
            "local quiet = {}\n"
            "for w in pairs(asked_names) do\n"
            "  local o, why = h:options(w)\n"
            "  if o == nil and why == 'no question is pending in ' .. w then quiet[#quiet + 1] = w end\n"
            "end\n"
            "check(#quiet == 8, 'options() of each Asked window with nothing asked: ' .. table.concat(quiet, ' '))\n"
            "local ok, w = h:open('sortyn')\n"
            "check(ok == nil and w == \"sortyn opens only for the game's own event; answer or cancel it instead\","
            "  'open(sortyn) refused: ' .. tostring(w))\n"
            "ok, w = h:open('ok')\n"
            "check(ok == nil and w == \"ok opens only for the game's own event; answer it instead\","
            "  'open(ok) refused, pointing at answer alone: ' .. tostring(w))\n"
            "ok, w = h:close('maplist')\n"
            "check(ok == true, 'close(maplist) with nothing asked queues: ' .. tostring(w))\n"
            "local raised, err = pcall(h.answer, h, 'sortyn', 1)\n"
            "check(not raised and tostring(err):find(\"answer('sortyn', yes) takes true for yes or false for no\", 1,"
            "  true), 'answer(sortyn, 1) raises: ' .. tostring(err))\n"
            "raised, err = pcall(h.answer, h, 'bankmenu', true)\n"
            "check(not raised and tostring(err):find(\"answer('bankmenu', container) takes the container\", 1, true),"
            "  'answer(bankmenu, true) raises: ' .. tostring(err))\n"
            "raised, err = pcall(h.answer, h, 'ok', 5)\n"
            "check(not raised and tostring(err):find(\"answer('ok') takes no value, or true\", 1, true),"
            "  'answer(ok, 5) raises: ' .. tostring(err))\n"
            "ok, w = h:cancel('confyn')\n"
            "check(ok == nil and w == \"confyn has no cancel; answer('confyn', false) says no\", 'cancel(confyn): '"
            "  .. tostring(w))\n"
            "ok, w = h:cancel('mp_dead')\n"
            "check(ok == nil and w:find('mp_dead has no cancel', 1, true) == 1, 'cancel(mp_dead): ' .. tostring(w))\n"
            "ok, w = h:cancel('ok')\n"
            "check(ok == nil and w == \"ok has no cancel; answer('ok') acknowledges it\", 'cancel(ok): ' .. tostring(w))\n"
            "ok, w = h:answer('maplist', 1)\n"
            "check(ok == nil and w == 'no question is pending in maplist', 'answer(maplist) with nothing asked')\n"
            "return finish()\n");

        // sortyn, asked from iuse: its setup the game's bytes
        uint8_t* sortyn = controller_named("sortyn");
        const int kRowSortyn = row_named("sortyn");
        check(asked_on_game_thread(0, 0) == 1 && live_menu(kRowSortyn) && get16(live_menu(kRowSortyn), 0x4C) == 2,
            "sortyn's setup, the game's bytes: the question written, the window opened through the hooked open, the"
            " cursor put on no through select-row");
        wait_frames(2);
        phase(L,
            "local o = h:options('sortyn')\n"
            "local seg = o and o.segments or {}\n"
            "check(o and o.name == 'sortyn' and type(o.id) == 'number' and o.text == 'Drop the Rusty Key?'"
            "  and #seg == 3 and seg[2].text == 'Rusty Key' and o.title and o.title.text == 'Drop'"
            "  and o.default == false and o.cancellable == true and o.blocked == false and o.detail"
            "  and type(o.detail.callback) == 'string' and o.detail.asker == nil,"
            "  'options(sortyn): the question and its title from the game text, no first, cancellable, not blocked')\n"
            "check(h:answer(o.name, true, o.id) == true, 'answer(sortyn, true, its id) queues')\n"
            "return finish()\n");
        const int sortyn_calls = al->asked_calls;
        const uint32_t sortyn_context = get32(sortyn, 0x18);
        wait_frames(3);
        check(al->asked_calls == sortyn_calls + 1 && al->asked_who == 0 && al->asked_value == 1
                && reinterpret_cast<uint32_t>(al->asked_context) == sortyn_context && get32(sortyn, 0x14) == 0
                && !live_menu(kRowSortyn) && strcmp(al->last_close, "menu    sortyn  ") == 0,
            "answer(sortyn, true): the callback with the item record and 1, the callback cleared, sortyn closed by"
            " name, as its confirm does");

        // hidden: no routed key reaches it
        phase(L,
            "asked_events(h) asked_events(h2)\n"
            "check(h:hide('sortyn') == true, 'hide(sortyn) queues')\n"
            "return finish()\n");
        wait_frames(3);
        asked_on_game_thread(0, 1);
        wait_frames(2);
        {
            uint8_t* menu = live_menu(kRowSortyn);
            typedef uint32_t (__fastcall* InputFn)(void* self, void* edx, int code);
            InputFn route;
            memcpy(&route, &g_fake->menu_routing, sizeof(route));
            const int sinks = al->sink_calls;
            put32(g_fake->mcb, 0x54, reinterpret_cast<uint32_t>(menu));
            bool dropped = true;
            for (int code = 1; code <= 0x20; ++code) {
                dropped = route(g_fake->mcb, NULL, code) == 0 && dropped;
            }
            put32(g_fake->mcb, 0x54, 0);
            check(hidden_in_place(kRowSortyn, menu) && dropped && al->sink_calls == sinks,
                "a hidden sortyn active at its own layer: no routed code reaches it");
        }
        phase(L,
            "local o = h:options('sortyn')\n"
            "check(o and o.default == true and o.blocked == false, 'hidden, its question still read: yes first')\n"
            "check(h:cancel('sortyn') == true and h:unhide('sortyn') == true, 'cancel(sortyn) and unhide queue')\n"
            "return finish()\n");
        wait_frames(3);
        check(al->asked_value == 0 && !live_menu(kRowSortyn) && get32(sortyn, 0x14) == 0,
            "cancel(sortyn): the callback with 0, the window closed");

        // sortyn blocked: kept with iuse, the window that asked
        {
            HuEngineApi api;
            HuEngineHandle raw = {-1, 0};
            char why[512] = "";
            if (record_api(&api)) {
                api.handle_open("raw", &raw, why, sizeof(why));
            }
            phase(L,
                "asked_events(h) asked_events(h2)\n"
                "check(h:block('sortyn') == true, 'block(sortyn) queues')\n"
                "return finish()\n");
            wait_frames(3);
            raw_read(api.poll5, &raw);
            asked_on_game_thread(0, 0);
            wait_frames(2);
            const bool old_pending = !sets_field(raw_read(api.pending5, &raw), "sortyn");
            const bool new_pending = sets_field(raw_read(api.pending8, &raw), "sortyn");
            const HuEngineReply& polled = raw_read(api.poll5, &raw);
            check(raw.slot >= 0 && !live_menu(kRowSortyn) && get32(sortyn, 0x14) != 0 && old_pending && new_pending
                    && has_string(polled, "blocked") && !has_string(polled, "pending"),
                "sortyn blocked: its open refused with the question written; pending8 lists it, pending5 and poll5"
                " leave it out");
            if (raw.slot >= 0) {
                api.handle_release(&raw);
            }
        }
        phase(L,
            "local seen = asked_events(h)\n"
            "check(seen == 'blocked:sortyn pending:sortyn:true', 'the refused open: blocked, then pending: ' .. seen)\n"
            "local o = h:options('sortyn')\n"
            "check(o and o.blocked == true and o.default == nil and o.text == 'Drop the Rusty Key?'"
            "  and o.detail.asker == 'iuse', 'options(sortyn) blocked: the question, blocked, iuse asking')\n"
            "local p = h:pending()\n"
            "check(p and p.sortyn and p.sortyn.id == o.id and p.sortyn.blocked == true, 'pending() lists sortyn')\n"
            "check(h:answer(p.sortyn.name, false, p.sortyn.id) == true, 'answer(sortyn, false, its id) queues')\n"
            "return finish()\n");
        wait_frames(3);
        check(al->asked_value == 0 && get32(sortyn, 0x14) == 0 && !live_menu(kRowSortyn),
            "answered with no window: the callback with 0, the callback cleared");
        phase(L,
            "local mine, other = asked_events(h), asked_events(h2)\n"
            "check(mine == '', 'the replying handle reads no event for its own reply: ' .. mine)\n"
            "check(other == 'blocked:sortyn pending:sortyn:true pending:sortyn:false', 'another handle reads the start"
            " and the end: ' .. other)\n"
            "check(h:pending().sortyn == nil, 'pending() lists it no more')\n"
            "check(h:unblock('sortyn') == true, 'unblock(sortyn)')\n"
            "return finish()\n");
        wait_frames(2);
        game_close("menu    iuse    ");
        put32(g_fake->mcb, 0x54, 0);

        // evitem01, asked from the key item list
        uint8_t* evitem01 = controller_named("evitem01");
        const int kRowEvitem = row_named("evitem01");
        const uint32_t evitem_callback = get32(call_target(g_fake->asked_sites[2] + 63), 55);
        check(asked_on_game_thread(1, 0) == 1 && live_menu(kRowEvitem) && get32(evitem01, 0x64) == evitem_callback,
            "evitem01 opened, then its setup, the game's bytes, writing the question and the callback it pushes");
        wait_frames(2);
        phase(L,
            "local o = h:options('evitem01')\n"
            "check(o and o.text == 'Use the Rusty Key?' and o.default == true and o.cancellable == true"
            "  and o.blocked == false and o.title == nil, 'options(evitem01): the question its setup printed, yes first')\n"
            "check(h:answer('evitem01', true) == true, 'answer(evitem01, true) queues')\n"
            "return finish()\n");
        wait_frames(3);
        check(al->asked_who == 1 && al->asked_value == 1 && al->asked_extra == 0 && !live_menu(kRowEvitem),
            "answer(evitem01, true): the callback the setup set, with 1, the key item list under its cursor, then"
            " the window closed by instance");
        phase(L,
            "asked_events(h) asked_events(h2)\n"
            "check(h:block('evitem01') == true, 'block(evitem01)')\n"
            "return finish()\n");
        wait_frames(3);
        check(asked_on_game_thread(1, 2) == 0 && !live_menu(kRowEvitem), "evitem01 blocked: its open refused");
        wait_frames(2);
        phase(L,
            "local seen = asked_events(h)\n"
            "check(seen == 'blocked:evitem01 pending:evitem01:true', 'blocked, then pending: ' .. seen)\n"
            "local p = h:pending()\n"
            "check(p.evitem01 and p.evitem01.blocked == true and p.evitem01.text == 'Use the Rusty Key?',"
            "  'pending() lists evitem01 with its question')\n"
            "check(h:cancel(p.evitem01.name, p.evitem01.id) == true, 'cancel(evitem01, its id) queues')\n"
            "return finish()\n");
        wait_frames(3);
        check(al->asked_who == 1 && al->asked_value == 0 && al->asked_extra == 2,
            "canceled with no window: the callback with 0, the key item list still on the item asked about");
        phase(L,
            "check(asked_events(h2) == 'blocked:evitem01 pending:evitem01:true pending:evitem01:false',"
            "  'another handle reads its end')\n"
            "asked_events(h)\n"
            "check(h:unblock('evitem01') == true, 'unblock(evitem01)')\n"
            "return finish()\n");
        wait_frames(2);
        game_close("menu    evitem  ");
        put32(g_fake->mcb, 0x54, 0);

        // confyn: the defaults restored on yes
        const int kRowConfyn = row_named("confyn");
        asked_on_game_thread(2, 1);
        wait_frames(2);
        phase(L,
            "local o = h:options('confyn')\n"
            "check(o and o.default == true and o.blocked == nil and o.cancellable == nil and o.detail.mode == 1,"
            "  'options(confyn): yes first, mode 1, nothing that blocks or cancels')\n"
            "check(h:answer('confyn', true) == true, 'answer(confyn, true) queues')\n"
            "return finish()\n");
        wait_frames(3);
        check(al->asked_who == 2 && al->asked_value == 2 && !live_menu(kRowConfyn),
            "answer(confyn, true) in mode 1: the restore with 2, then the window closed by instance");

        // bankmenu: Mog Safe 2's availability from the player's data
        uint8_t* bankmenu = controller_named("bankmenu");
        const int kRowBank = row_named("bankmenu");
        asked_on_game_thread(3, 0);
        wait_frames(2);
        phase(L,
            "local o = h:options('bankmenu')\n"
            "local c = o and o.containers or {}\n"
            "check(o and #c == 2 and c[1].id == 1 and c[1].available == true and c[2].id == 9"
            "  and c[2].available == false and o.blocked == false, 'options(bankmenu): the Mog Safe, Mog Safe 2'"
            "  .. ' not open to the player')\n"
            "local ok, w = h:answer('bankmenu', 9)\n"
            "check(ok == nil and w == 'Mog Safe 2 is not open to the player', 'answer(bankmenu, 9) refused: '"
            "  .. tostring(w))\n"
            "ok, w = h:answer('bankmenu', 2)\n"
            "check(ok == nil and w == 'bankmenu offers the containers 1 and 9, not 2', 'answer(bankmenu, 2) refused')\n"
            "return finish()\n");
        g_fake->player_data[0x21] = 1;
        phase(L,
            "check(h:options('bankmenu').containers[2].available == true, 'Mog Safe 2 open')\n"
            "check(h:answer('bankmenu', 9) == true, 'answer(bankmenu, 9) queues')\n"
            "return finish()\n");
        wait_frames(3);
        check(al->asked_who == 3 && al->asked_value == 9 && al->asked_extra == 0 && !live_menu(kRowBank)
                && get32(bankmenu, 0x14) == 0,
            "answer(bankmenu, 9): the callback with 9 and 0, then the window closed, its begin-close clearing the"
            " callback");
        phase(L,
            "asked_events(h) asked_events(h2)\n"
            "check(h:block('bankmenu') == true, 'block(bankmenu)')\n"
            "return finish()\n");
        wait_frames(3);
        check(asked_on_game_thread(3, 1) == 1 && !live_menu(kRowBank) && get32(bankmenu, 0x14) != 0,
            "bankmenu blocked: the callback stays set with no window");
        wait_frames(2);
        check(asked_on_game_thread(3, 1) == 0, "while it waits, the next picker's setup refuses, as the game's does");
        phase(L,
            "local seen = asked_events(h)\n"
            "check(seen == 'blocked:bankmenu pending:bankmenu:true', 'blocked, then pending: ' .. seen)\n"
            "local p = h:pending()\n"
            "check(p.bankmenu and p.bankmenu.blocked == true and #p.bankmenu.containers == 2,"
            "  'pending() lists bankmenu')\n"
            "check(h:answer(p.bankmenu.name, 1, p.bankmenu.id) == true, 'answer(bankmenu, 1, its id) queues')\n"
            "return finish()\n");
        wait_frames(3);
        check(al->asked_value == 1 && al->asked_extra == 0 && get32(bankmenu, 0x14) == 0,
            "answered with no window: the callback with 1 and 0, the callback cleared as its begin-close does");
        phase(L,
            "check(asked_events(h) == '', 'the replying handle reads no event for its reply')\n"
            "check(asked_events(h2) == 'blocked:bankmenu pending:bankmenu:true pending:bankmenu:false',"
            "  'another handle reads the refused open, the start and the end')\n"
            "check(h:unblock('bankmenu') == true, 'unblock(bankmenu)')\n"
            "return finish()\n");
        wait_frames(2);

        // maplist, asked from map0
        uint8_t* maplist = controller_named("maplist");
        const int kRowMaplist = row_named("maplist");
        asked_on_game_thread(4, 100);
        wait_frames(2);
        const int map_builds = al->map_builds;
        const int map_answers = al->asked_calls;
        phase(L,
            "local o = h:options('maplist')\n"
            "local r = o and o.rows or {}\n"
            "check(o and #r == 4 and r[1].text == 'Current Area' and r[1].value == 1 and r[1].kind == 'map'"
            "  and r[1].zone == 100 and r[1].map == 0 and r[1].label == 'East' and r[2].kind == 'list'"
            "  and r[2].count == 6 and r[2].zone == nil and r[3].zone == 101 and r[3].map == 2"
            "  and r[4].kind == 'close' and o.top == true and o.level == 'top' and o.blocked == false,"
            "  'options(maplist): its rows with their kind, zone and map, label or count; the regions shown, the top"
            " level')\n"
            "map_first = o and o.id\n"
            "check(h:answer('maplist', 2) == true, 'answer(maplist, 2), a region\\'s row, queues')\n"
            "return finish()\n");
        wait_frames(3);
        check(live_menu(kRowMaplist) && al->map_builds == map_builds + 1 && al->map_select_row == 0
                && al->map_select_flag == 0 && get16(maplist, 0x58) == 2 && get16(maplist, 0x5C) == -1
                && get16(maplist, 0x5E) == 0 && get32(maplist, 0x54) == 2 && al->asked_calls == map_answers,
            "answer(maplist, 2): the key handler's step down -- +0x5E 0, +0x58 2, +0x5C -1 -- the rebuild, the game's"
            " head around the builders' stand-in, and the select of row 0; the window stays, no callback");
        phase(L,
            "local o = h:options('maplist')\n"
            "local r = o and o.rows or {}\n"
            "check(o and #r == 2 and r[1].text == 'West Ronfaure' and r[1].kind == 'map' and r[1].zone == 100"
            "  and r[1].map == 1 and r[2].kind == 'list' and r[2].zone == nil and o.top == false"
            "  and o.level == 'region' and o.id ~= map_first,"
            "  'options(maplist) after the step: the region\\'s rows, level region, a new id')\n"
            "local ok, w = h:answer('maplist', 1, map_first)\n"
            "check(ok == nil and w == 'that prompt is gone', 'an answer naming the first level\\'s id refused: '"
            "  .. tostring(w))\n"
            "check(h:answer('maplist', 2, o.id) == true, 'answer(maplist, 2, its id) queues')\n"
            "return finish()\n");
        wait_frames(3);
        check(live_menu(kRowMaplist) && al->map_builds == map_builds + 2 && get16(maplist, 0x58) == 192
                && get16(maplist, 0x5C) == 0 && get16(maplist, 0x5E) == 2 && get16(maplist, 0x5A) == 0
                && al->asked_calls == map_answers,
            "answer(maplist, 2) in the region: a zone of several maps, stepped down into: +0x5E 2, +0x58 192, +0x5C 0,"
            " the row under the cursor (row 0, the select's) kept at +0x5A");
        phase(L,
            "local o = h:options('maplist')\n"
            "local r = o and o.rows or {}\n"
            "check(o and #r == 2 and r[1].zone == 190 and r[1].map == 1 and r[2].zone == 190 and r[2].map == 2"
            "  and o.level == 'zone', 'options(maplist): the zone\\'s maps, level zone')\n"
            "check(h:cancel('maplist', o.id) == true, 'cancel(maplist) inside a zone\\'s maps queues')\n"
            "return finish()\n");
        wait_frames(3);
        check(live_menu(kRowMaplist) && al->map_builds == map_builds + 3 && get16(maplist, 0x58) == 2
                && get16(maplist, 0x5C) == -1 && al->map_select_row == 0 && al->asked_calls == map_answers,
            "cancel(maplist) inside a zone's maps: back to the region, +0x58 from +0x5E, +0x5C -1, the select of the"
            " row kept");
        phase(L,
            "local o = h:options('maplist')\n"
            "check(o and o.level == 'region' and #o.rows == 2, 'back in the region')\n"
            "check(h:cancel('maplist') == true, 'cancel(maplist) inside a region queues')\n"
            "return finish()\n");
        wait_frames(3);
        check(live_menu(kRowMaplist) && al->map_builds == map_builds + 4 && get16(maplist, 0x58) == 0
                && get16(maplist, 0x5E) == 0 && get32(maplist, 0x54) == 4 && al->asked_calls == map_answers,
            "cancel(maplist) inside a region: back to the top, its rows built again");
        // a row with no entry: options() passes over it and lists the rows
        // after it, which an answer takes by their place
        {
            pause_game();
            uint8_t* top_rows = reinterpret_cast<uint8_t*>(get32(maplist, 0x60));
            const uint32_t first_entry = get32(top_rows, 0x48);
            put32(top_rows, 0x48, 0);
            phase(L,
                "local o = h:options('maplist')\n"
                "local values = {}\n"
                "for _, r in ipairs(o and o.rows or {}) do values[#values + 1] = tostring(r.value) end\n"
                "check(table.concat(values, ' ') == '2 3 4', 'the top with its first row\\'s entry gone: options()'"
                "  .. ' lists the rows after it: ' .. table.concat(values, ' '))\n"
                "return finish()\n");
            put32(top_rows, 0x48, first_entry);
            resume_game();
        }
        phase(L,
            "local o = h:options('maplist')\n"
            "check(o and o.level == 'top' and o.top == true and #o.rows == 4, 'back at the top')\n"
            "check(h:answer('maplist', 3) == true, 'answer(maplist, 3) queues')\n"
            "return finish()\n");
        wait_frames(3);
        check(al->asked_who == 4 && al->asked_value == 101 && al->asked_extra == 2 && !live_menu(kRowMaplist)
                && get32(maplist, 0x60) == 0 && get32(maplist, 0x6C) != 0,
            "answer(maplist, 3): the callback with the row's entry {101, 2}, then close+reset, the game's bytes:"
            " the window closed, its rows freed, the callback left set");
        phase(L,
            "local o, why = h:options('maplist')\n"
            "check(o == nil and why == 'no question is pending in maplist', 'its callback left set asks nothing')\n"
            "asked_events(h) asked_events(h2)\n"
            "check(h:block('maplist') == true, 'block(maplist)')\n"
            "return finish()\n");
        wait_frames(3);
        const int map_calls = al->asked_calls;
        check(asked_on_game_thread(4, 100) == 0 && !live_menu(kRowMaplist), "maplist blocked: its open refused");
        wait_frames(2);
        phase(L,
            "check(asked_events(h) == 'blocked:maplist pending:maplist:true', 'blocked, then pending')\n"
            "local p = h:pending()\n"
            "check(p.maplist and p.maplist.blocked == true and #p.maplist.rows == 4, 'pending() lists maplist')\n"
            "map_first = p.maplist.id\n"
            "check(h:answer(p.maplist.name, 2, p.maplist.id) == true, 'answer(maplist, 2) with no window queues')\n"
            "return finish()\n");
        wait_frames(3);
        check(al->asked_calls == map_calls && !live_menu(kRowMaplist) && get16(maplist, 0x58) == 2
                && get16(maplist, 0x5A) == -1 && get32(maplist, 0x54) == 2,
            "answered with no window: the step down all the same, the cursor read -1 with no window");
        phase(L,
            "check(asked_events(h) == 'pending:maplist:true', 'the replying handle reads the new level\\'s start"
            " alone')\n"
            "local p = h:pending()\n"
            "check(p.maplist and p.maplist.blocked == true and #p.maplist.rows == 2 and p.maplist.level == 'region'"
            "  and p.maplist.id ~= map_first, 'pending() lists the region as a new prompt')\n"
            "check(h:cancel(p.maplist.name, p.maplist.id) == true, 'cancel(maplist) inside the region queues')\n"
            "return finish()\n");
        wait_frames(3);
        check(al->asked_calls == map_calls && get16(maplist, 0x58) == 0 && get32(maplist, 0x54) == 4,
            "canceled inside the region with no window: back to the top");
        phase(L,
            "check(asked_events(h) == 'pending:maplist:true', 'the top again, a new prompt')\n"
            "local p = h:pending()\n"
            "check(p.maplist and p.maplist.level == 'top' and #p.maplist.rows == 4, 'pending() lists the top')\n"
            "check(h:cancel(p.maplist.name, p.maplist.id) == true, 'cancel(maplist, its id) queues')\n"
            "return finish()\n");
        wait_frames(3);
        check(al->asked_calls == map_calls && get32(maplist, 0x60) == 0,
            "canceled with no window: close+reset alone, no callback");
        phase(L,
            "check(asked_events(h2) == 'blocked:maplist pending:maplist:true pending:maplist:false"
            " pending:maplist:true pending:maplist:false pending:maplist:true pending:maplist:false',"
            "  'another handle reads each level\\'s start and end')\n"
            "asked_events(h)\n"
            "check(h:unblock('maplist') == true, 'unblock(maplist)')\n"
            "return finish()\n");
        wait_frames(2);
        game_close("menu    map0    ");
        put32(g_fake->mcb, 0x54, 0);

        // gmtell, hide-only: the GM tell poller sends what the result word says
        const int kRowGm = row_named("gmtell");
        *g_fake->gm_polls = 1;
        asked_on_game_thread(5, 2);
        wait_frames(2);
        phase(L,
            "local o = h:options('gmtell')\n"
            "local opts = o and o.options or {}\n"
            "check(o and o.gm == 'GM_Test' and o.title.text == 'Are you there?' and #opts == 3"
            "  and opts[2].text == 'No' and opts[2].value == 2 and o.cancellable == true and o.detail.slot == 2"
            "  and o.blocked == nil, 'options(gmtell): the GM, the title and the options in plain fields')\n"
            "local ok, w = h:answer('gmtell', 4)\n"
            "check(ok == nil and w == \"no option in the list has the value 4 (options('gmtell') lists them)\","
            "  'an unlisted value refused: ' .. tostring(w))\n"
            "check(h:answer('gmtell', 2) == true, 'answer(gmtell, 2) queues')\n"
            "return finish()\n");
        wait_frames(3);
        check(al->gm_result == 2 && !live_menu(kRowGm), "answer(gmtell, 2): the poller read 2 and closed the window");
        asked_on_game_thread(5, 0);
        wait_frames(2);
        phase(L,
            "check(h:cancel('gmtell') == true, 'cancel(gmtell) queues')\n"
            "return finish()\n");
        wait_frames(3);
        check(al->gm_result == 0xFF && !live_menu(kRowGm), "cancel(gmtell): the poller read the decline, 0xFF");
        *g_fake->gm_polls = 0;

        // mp_dead, hide-only: the choice the KO handler polls
        uint8_t* mp = controller_named("mp_dead");
        const int kRowMp = row_named("mp_dead");
        asked_on_game_thread(6, 30);
        wait_frames(2);
        phase(L,
            "local o = h:options('mp_dead')\n"
            "local c = o and o.options or {}\n"
            "check(o and #c == 2 and c[1].text == 'Retry' and c[1].value == 1 and c[2].text == 'Return'"
            "  and c[2].value == 2 and o.seconds == 30 and o.detail.remaining == 1800,"
            "  'options(mp_dead): its two rows as the game wrote them, their values, the seconds left')\n"
            "local ok, w = h:answer('mp_dead', 3)\n"
            "check(ok == nil and w == 'mp_dead takes 1 or 2, not 3', 'answer(mp_dead, 3) refused')\n"
            "check(h:answer('mp_dead', 1) == true, 'answer(mp_dead, 1) queues')\n"
            "return finish()\n");
        wait_frames(3);
        check(get32(mp, 0x24) == 1 && live_menu(kRowMp), "answer(mp_dead, 1): 1 at +0x24, the window left to the KO"
            " handler");
        phase(L,
            "local o, why = h:options('mp_dead')\n"
            "check(o == nil and why == 'no question is pending in mp_dead', 'a choice made: nothing pending')\n"
            "return finish()\n");
        game_close("menu    mp_dead ");

        // ok: the error screen waits on its acknowledgment
        const int kRowOk = row_named("ok");
        uint8_t* okc = controller_named("ok");
        *g_fake->error_screen = 1;
        asked_on_game_thread(7, 0);
        wait_frames(2);
        phase(L,
            "local o = h:options('ok')\n"
            "check(o and o.text == 'Lost connection to the server.' and o.blocked == false and o.default == nil,"
            "  'options(ok): the notice')\n"
            "check(h:answer('ok') == true, 'answer(ok) queues')\n"
            "return finish()\n");
        wait_frames(3);
        check(al->ok_seen == 1 && !live_menu(kRowOk), "answer(ok): acknowledged, the error screen closed it");
        asked_on_game_thread(7, 1);
        wait_frames(2);
        phase(L,
            "check(h:answer('ok', true) == true, 'answer(ok, true) queues')\n"
            "return finish()\n");
        wait_frames(3);
        check(al->asked_who == 7 && al->asked_context != NULL && !live_menu(kRowOk),
            "a notice with a callback: the callback with its context, the window closed by instance");
        // the blocks of engine abi 5 and 7, to which ok is no prompt, leave an
        // open ok to its acknowledgment as block8 does: held, hidden, open
        {
            HuEngineApi api;
            HuEngineHandle raw = {-1, 0};
            char why[512] = "";
            if (record_api(&api)) {
                api.handle_open("old_block", &raw, why, sizeof(why));
            }
            phase(L,
                "asked_events(h) asked_events(h2)\n"
                "return finish()\n");
            asked_on_game_thread(7, 0);
            wait_frames(2);
            uint8_t* up = live_menu(kRowOk);
            const bool held = raw.slot >= 0 && api.block5(&raw, "ok", why, sizeof(why)) == 1
                && api.block7(&raw, "ok", NULL, why, sizeof(why)) == 1;
            wait_frames(3);
            check(held && up && live_menu(kRowOk) == up && hidden_in_place(kRowOk, up),
                "block5 and block7 of an open ok hold it and leave it open, hidden, unacknowledged");
            phase(L,
                "local seen = asked_events(h)\n"
                "check(seen == 'opened:ok', 'no closed event while the old blocks hold it: ' .. seen)\n"
                "return finish()\n");
            const bool unheld = raw.slot >= 0 && api.unblock3(&raw, "ok", why, sizeof(why)) == 1;
            if (raw.slot >= 0) {
                api.handle_release(&raw);
            }
            phase(L,
                "check(h:answer('ok') == true, 'answer(ok) queues')\n"
                "return finish()\n");
            wait_frames(3);
            check(unheld && !live_menu(kRowOk), "unblocked and answered, ok closes");
        }
        phase(L,
            "asked_events(h) asked_events(h2)\n"
            "check(h:block('ok') == true, 'block(ok)')\n"
            "return finish()\n");
        wait_frames(3);
        const int seen_before = al->ok_seen;
        check(asked_on_game_thread(7, 0) == 0 && !live_menu(kRowOk), "ok blocked: its open refused");
        wait_frames(2);
        phase(L,
            "check(asked_events(h) == 'blocked:ok pending:ok:true', 'blocked, then pending')\n"
            "local p = h:pending()\n"
            "check(p.ok and p.ok.blocked == true and p.ok.text == 'Lost connection to the server.',"
            "  'pending() lists the notice')\n"
            "check(h:answer(p.ok.name, true, p.ok.id) == true, 'answer(ok, true, its id) queues')\n"
            "return finish()\n");
        wait_frames(3);
        check(al->ok_seen == seen_before + 1, "answered with no window: acknowledged, the error screen moved on");
        phase(L,
            "check(asked_events(h2) == 'blocked:ok pending:ok:true pending:ok:false', 'another handle reads its end')\n"
            "asked_events(h)\n"
            "check(h:unblock('ok') == true, 'unblock(ok)')\n"
            "for _, x in ipairs({h, h2}) do x:poll() end\n"
            "return finish()\n");
        wait_frames(2);
        *g_fake->error_screen = 0;
        put32(okc, 0x25C, 0);

        // mogdoor: the door's handler writes the nation, then opens the
        // window, whose open hook takes the nation's list; options() reads
        // its rows, answer and cancel run its confirm's gate and its exit or
        // stay; blocked, the question kept and answered with no window
        uint8_t* door = controller_named("mogdoor");
        const int kRowDoor = row_named("mogdoor");
        auto door_reset = [&]() {
            memset(g_fake->door_exiting, 0, 3);
            put32(g_fake->door_actor, 0x880, 0);
            door[0x15] = 0xEE;
        };
        door_reset();
        phase(L,
            "function door_errors(handle)\n"
            "  local out = {}\n"
            "  for _, e in ipairs((handle:poll())) do\n"
            "    if e.event == 'error' then out[#out + 1] = e.verb .. ' ' .. e.name .. ': ' .. e.reason end\n"
            "  end\n"
            "  return table.concat(out, ' | ')\n"
            "end\n"
            "asked_names.mogdoor = true\n"
            "for _, x in ipairs({h, h2}) do x:poll() end\n"
            "return finish()\n");
        check(asked_on_game_thread(8, 1) == 1 && live_menu(kRowDoor) && door[0x16] == 4 && door[0x18] == 0,
            "the door's handler: the nation written, mogdoor opened, its open hook taking the nation's four areas");
        wait_frames(2);
        const int gates = al->door_gates;
        const int opens = al->door_opens;
        phase(L,
            "local o = h:options('mogdoor')\n"
            "local r, a = o and o.options or {}, o and o.areas or {}\n"
            "check(o and o.title and o.title.text == 'Leave the Mog House?' and o.level == 'top' and #r == 4"
            "  and r[1].text == 'Stay' and r[1].kind == 'stay' and r[1].value == nil and r[2].text == 'Go out'"
            "  and r[2].kind == 'back' and r[2].value == 0 and r[3].text == 'Second floor' and r[3].kind == 'floor'"
            "  and r[3].value == 125 and r[3].enabled == true and r[4].text == 'Other areas' and r[4].kind == 'areas'"
            "  and #a == 4 and a[1].text == \"Southern San d'Oria\" and a[1].kind == 'area' and a[1].value == 1"
            "  and a[1].zone == 230 and a[3].value == 3 and a[4].text == 'Mog Garden' and a[4].kind == 'garden'"
            "  and a[4].value == 127 and a[4].enabled == true and o.blocked == false,"
            "  'options(mogdoor): the title, the top\\'s rows with the values that answer them, the list of areas, in the"
            " texts the draw shows')\n"
            "local ok, w = h:answer('mogdoor', 4)\n"
            "check(ok == nil and w == \"mogdoor offers no exit 4 (options('mogdoor') lists them)\","
            "  'answer(mogdoor, 4) refused: ' .. tostring(w))\n"
            "ok, w = h:answer('mogdoor', 126)\n"
            "check(ok == nil and w == \"the player is on the other floor: mogdoor's floor row is 125\","
            "  'the other floor\\'s value refused: ' .. tostring(w))\n"
            "local raised, err = pcall(h.answer, h, 'mogdoor', true)\n"
            "check(not raised and tostring(err):find(\"answer('mogdoor', exit) takes the value of an exit\", 1, true),"
            "  'answer(mogdoor, true) raises: ' .. tostring(err))\n"
            "check(h:answer('mogdoor', 2) == true, 'answer(mogdoor, 2) queues')\n"
            "return finish()\n");
        wait_frames(3);
        check(al->door_gates == gates + 1 && al->door_gate_args[0] == 4 && al->door_gate_args[1] == 0
                && door[0x15] == 2 && get32(g_fake->door_actor, 0x880)
                    == static_cast<uint32_t>(reinterpret_cast<uintptr_t>(g_fake->door_opened))
                && al->door_opens == opens + 1 && al->door_open_actor == g_fake->door_actor && al->door_open_arg == 0
                && al->door_exit_at_open == 2 && g_fake->door_exiting[0] == 1 && g_fake->door_exiting[1] == 1
                && g_fake->door_exiting[2] == 1 && live_menu(kRowDoor),
            "answer(mogdoor, 2): the confirm's gate (4, 0), 2 at +0x15, the door actor given the routine that sends"
            " it, its door opened, the three bytes set; the window left up");
        phase(L,
            "local o, why = h:options('mogdoor')\n"
            "check(o == nil and why == 'no question is pending in mogdoor', 'an exit under way asks nothing')\n"
            "return finish()\n");
        game_close("menu    mogdoor ");
        door_reset();
        wait_frames(2);

        // the gate saying no; the Mog Garden and the floor row turned off;
        // the stay row
        asked_on_game_thread(8, 1);
        *g_fake->door_gate_answer = 0xABCD0000u;
        *g_fake->door_garden_on = 0;
        *g_fake->door_floor_on = 0;
        wait_frames(2);
        phase(L,
            "door_errors(h)\n"
            "local o = h:options('mogdoor')\n"
            "check(o and o.options[3].enabled == false and o.areas[4].enabled == false, 'the floor row and the Mog"
            " Garden turned off by the tick: enabled false')\n"
            "local ok, w = h:answer('mogdoor', 127)\n"
            "check(ok == nil and w == 'the Mog Garden is not open to the player now', 'the Mog Garden refused: '"
            "  .. tostring(w))\n"
            "ok, w = h:answer('mogdoor', 125)\n"
            "check(ok == nil and w == \"mogdoor's floor row is turned off now\", 'the floor row refused: ' .. tostring(w))\n"
            "check(h:answer('mogdoor', 0) == true, 'answer(mogdoor, 0) queues')\n"
            "return finish()\n");
        wait_frames(3);
        phase(L,
            "local seen = door_errors(h)\n"
            "check(seen == 'answer mogdoor: the game refuses an exit now; its own message says why', 'the gate"
            " saying no: an error event, nothing written: ' .. seen)\n"
            "return finish()\n");
        check(door[0x15] == 0xEE && al->door_opens == opens + 1 && !g_fake->door_exiting[0] && live_menu(kRowDoor),
            "a gate that says no: nothing written, no exit");
        *g_fake->door_garden_on = 1;
        *g_fake->door_floor_on = 1;
        const int closes_before = al->door_gates;
        pause_game();
        door[0x18] = 1;
        resume_game();
        phase(L,
            "check(h:cancel('mogdoor') == true, 'cancel(mogdoor) queues')\n"
            "return finish()\n");
        wait_frames(3);
        *g_fake->door_gate_answer = 0xABCD0001u;
        check(al->door_gates == closes_before && door[0x15] == 0xEE && !live_menu(kRowDoor)
                && al->door_opens == opens + 1 && !g_fake->door_exiting[0],
            "cancel(mogdoor) at the list of areas, the gate saying no: the top level's Escape, the window closed by"
            " instance, no gate run, nothing written at +0x15, no exit");
        door_reset();

        // blocked: the door's handler wrote the nation, the open was refused
        {
            HuEngineApi api;
            HuEngineHandle raw = {-1, 0};
            char why[512] = "";
            if (record_api(&api)) {
                api.handle_open("raw", &raw, why, sizeof(why));
            }
            phase(L,
                "asked_events(h) asked_events(h2)\n"
                "check(h:block('mogdoor') == true, 'block(mogdoor)')\n"
                "return finish()\n");
            wait_frames(3);
            raw_read(api.poll5, &raw);
            check(asked_on_game_thread(8, 5) == 0 && !live_menu(kRowDoor), "mogdoor blocked: its open refused");
            wait_frames(2);
            const bool old_pending = !sets_field(raw_read(api.pending5, &raw), "mogdoor");
            const bool new_pending = sets_field(raw_read(api.pending8, &raw), "mogdoor");
            check(raw.slot >= 0 && old_pending && new_pending, "pending8 lists the blocked mogdoor, pending5 not");
            if (raw.slot >= 0) {
                api.handle_release(&raw);
            }
        }
        phase(L,
            "local seen = asked_events(h)\n"
            "check(seen == 'blocked:mogdoor pending:mogdoor:true', 'blocked, then pending: ' .. seen)\n"
            "local p = h:pending()\n"
            "local q = p.mogdoor or {}\n"
            "local a = q.areas or {}\n"
            "check(q.blocked == true and q.level == 'top' and #a == 3 and a[1].text == 'Al Zahbi' and a[1].value == 1"
            "  and a[3].kind == 'garden' and a[3].value == 127 and q.options[2].value == 0,"
            "  'pending() lists mogdoor: the top, the nation\\'s list from the table')\n"
            "check(h:answer(q.name, 2, q.id) == true, 'answer(mogdoor, 2, its id) queues')\n"
            "return finish()\n");
        wait_frames(3);
        check(door[0x15] == 2 && al->door_opens == opens + 2 && g_fake->door_exiting[0] == 1 && !live_menu(kRowDoor),
            "answered with no window: the gate, 2 at +0x15, the exit");
        phase(L,
            "check(asked_events(h) == '', 'the replying handle reads no event for its reply')\n"
            "check(asked_events(h2) == 'blocked:mogdoor pending:mogdoor:true pending:mogdoor:false',"
            "  'another handle reads the refused open, the start and the end')\n"
            "check(h:unblock('mogdoor') == true, 'unblock(mogdoor)')\n"
            "for _, x in ipairs({h, h2}) do x:poll() end\n"
            "return finish()\n");
        wait_frames(2);
        door_reset();

        // the NPC dialogue boxes: options() reading the page from the box's
        // own lines, answer() one page on through the gate's advance and
        // sound with no code handed to the text dispatch, cancel() the box's
        // own cancel, a page event through poll8 alone, and the left button
        // kept off a hidden box
        const int kRowTalk1 = row_named("rem4line");
        const int kRowTalk2 = row_named("rem4li2");
        *g_fake->query_cancel_allowed = 1;
        *g_fake->close_frames = 1;
        {
            HuEngineApi api;
            HuEngineHandle raw = {-1, 0};
            char why[512] = "";
            if (record_api(&api)) {
                api.handle_open("raw", &raw, why, sizeof(why));
            }
            phase(L,
                "function talk_events(handle)\n"
                "  local out = {}\n"
                "  for _, e in ipairs((handle:poll())) do\n"
                "    if (e.name == 'rem4line' or e.name == 'rem4li2') and e.event ~= 'cursor' then\n"
                "      out[#out + 1] = e.event .. ':' .. e.name\n"
                "    elseif e.event == 'error' then out[#out + 1] = 'error:' .. e.reason end\n"
                "  end\n"
                "  return table.concat(out, ' ')\n"
                "end\n"
                "talk_events(h) talk_events(h2)\n"
                "local o, why = h:options('rem4line')\n"
                "check(o == nil and why == 'no dialogue is shown in rem4line', 'options(rem4line) with no box: '"
                "  .. tostring(why))\n"
                "return finish()\n");
            raw_read(api.poll8, &raw);
            const int advances = al->advances;
            const int sounds = al->page_sounds;
            const int codes = al->dispatch_codes;
            check(asked_on_game_thread(9, (1 << 8) | (1 << 16)) == 1 && live_menu(kRowTalk1),
                "rem4line opened, a message's first page shown, waiting for a key");
            wait_frames(2);
            const HuEngineReply& polled8 = raw_read(api.poll8, &raw);
            const bool page8 = has_string(polled8, "page");
            check(raw.slot >= 0 && page8, "poll8 carries the page event");
            phase(L,
                "local seen = talk_events(h)\n"
                "check(seen == 'opened:rem4line page:rem4line', 'opened, then the first page: ' .. seen)\n"
                "local o = h:options('rem4line')\n"
                "check(o and o.name == 'rem4line' and type(o.id) == 'number' and o.text == 'Moogle : Welcome home,\\nkupo!'"
                "  and o.waits == 'key' and o.seconds == nil and o.auto == false and o.more == true and o.queued == 0"
                "  and o.cancellable == true, 'options(rem4line): the page from its lines, joined by a newline, waiting"
                " for a key, a page after it')\n"
                "local raised, err = pcall(h.answer, h, 'rem4line', 2)\n"
                "check(not raised and tostring(err):find(\"answer('rem4line') takes no value, or true\", 1, true),"
                "  'answer(rem4line, 2) raises: ' .. tostring(err))\n"
                "check(h:answer('rem4line') == true, 'answer(rem4line) queues')\n"
                "return finish()\n");
            wait_frames(3);
            check(al->advances == advances + 1 && al->advance_text == g_fake->texts && al->page_sounds == sounds + 1
                    && al->dispatch_codes == codes && live_menu(kRowTalk1),
                "answer(rem4line): rem4line's own text object advanced once, then the page-turn sound once; no code"
                " handed to the text dispatch");
            raw_read(api.poll5, &raw);
            phase(L,
                "local seen = talk_events(h)\n"
                "check(seen == 'page:rem4line', 'the next page: ' .. seen)\n"
                "local o = h:options('rem4line')\n"
                "check(o and o.text == 'Rest well.' and o.more == false, 'the second page, the last')\n"
                "check(h:answer('rem4line', true) == true, 'answer(rem4line, true) queues')\n"
                "return finish()\n");
            wait_frames(3);
            const bool page5 = has_string(raw_read(api.poll5, &raw), "page");
            check(al->advances == advances + 2 && al->page_sounds == sounds + 2 && al->dispatch_codes == codes
                    && !live_menu(kRowTalk1) && !page5,
                "the last page answered: the message ends, the box closes; poll5 carries no page event");
            phase(L,
                "local seen = talk_events(h)\n"
                "check(seen == 'closed:rem4line', 'closed at the message\\'s end: ' .. seen)\n"
                "local ok, w = h:answer('rem4line')\n"
                "check(ok == nil and w == 'no dialogue is shown in rem4line', 'answer with no box: ' .. tostring(w))\n"
                "return finish()\n");
            if (raw.slot >= 0) {
                api.handle_release(&raw);
            }
        }

        // a timed page; a new message replacing the one shown; the cancel
        asked_on_game_thread(10, (2 << 8) | (1 << 16));
        wait_frames(2);
        phase(L,
            "local seen = talk_events(h)\n"
            "check(seen == 'opened:rem4li2 page:rem4li2', 'rem4li2 opened, its first page: ' .. seen)\n"
            "local o = h:options('rem4li2')\n"
            "check(o and o.waits == 'timer' and o.seconds == 5, 'a timed page: its seconds left')\n"
            "local ok, w = h:answer('rem4li2')\n"
            "check(ok == nil and w == 'the page rem4li2 shows does not wait for a key: it goes on by itself',"
            "  'answer on a timed page refused: ' .. tostring(w))\n"
            "return finish()\n");
        // an answer through a box showing a page that waits for a key, its
        // twin's on its timer
        asked_on_game_thread(9, (1 << 8) | (1 << 16));
        wait_frames(2);
        const int timed_advances = al->advances;
        const int timed_sounds = al->page_sounds;
        const int timed_codes = al->dispatch_codes;
        phase(L,
            "local seen = talk_events(h)\n"
            "check(seen == 'opened:rem4line page:rem4line',"
            "  'rem4line opened beside rem4li2\\'s timed page: ' .. seen)\n"
            "check(h:answer('rem4line') == true, 'answer(rem4line) queues')\n"
            "return finish()\n");
        wait_frames(3);
        check(al->advances == timed_advances + 1 && al->advance_text == g_fake->texts
                && al->page_sounds == timed_sounds + 1 && al->dispatch_codes == timed_codes && live_menu(kRowTalk1)
                && live_menu(kRowTalk2),
            "answer(rem4line) beside rem4li2's timed page: rem4line's text object advanced once, the page-turn sound"
            " once, rem4li2's left to its timer; no code handed to the text dispatch");
        phase(L,
            "local seen = talk_events(h)\n"
            "local o = h:options('rem4li2')\n"
            "check(seen == 'page:rem4line' and o and o.waits == 'timer' and o.seconds == 5,"
            "  'rem4line\\'s next page alone, rem4li2 still on its timer: ' .. seen)\n"
            "check(h:answer('rem4line') == true, 'answer(rem4line) on its last page queues')\n"
            "return finish()\n");
        wait_frames(3);
        phase(L,
            "local seen = talk_events(h)\n"
            "check(seen == 'closed:rem4line' and h:options('rem4li2').waits == 'timer',"
            "  'rem4line closed at its message\\'s end, rem4li2 still on its timer: ' .. seen)\n"
            "return finish()\n");
        asked_on_game_thread(10, 1 | (1 << 8) | (2 << 16));
        wait_frames(2);
        *g_fake->query_cancel_allowed = 0;
        phase(L,
            "local seen = talk_events(h)\n"
            "check(seen == 'page:rem4li2', 'a new message with no close: a page event: ' .. seen)\n"
            "local o = h:options('rem4li2')\n"
            "check(o and o.text == 'Moogle : One more thing.' and o.queued == 1 and o.cancellable == false,"
            "  'the new message, one more held behind it, no cancel allowed')\n"
            "local ok, w = h:cancel('rem4li2')\n"
            "check(ok == nil and w == 'rem4li2 cannot be canceled now: the event allows no cancel',"
            "  'cancel refused while the event allows none: ' .. tostring(w))\n"
            "return finish()\n");
        *g_fake->query_cancel_allowed = 1;
        *g_fake->dialogue_wait = 1;
        const int releases = al->releases;
        const int clears = al->clears;
        phase(L,
            "check(h:cancel('rem4li2') == true, 'cancel(rem4li2) queues')\n"
            "return finish()\n");
        wait_frames(3);
        const uint8_t* log_ctl = reinterpret_cast<const uint8_t*>(get32(controller_named("rem4li2"), 0x64));
        check(al->releases == releases + 1 && reinterpret_cast<uintptr_t>(al->release_log) == get32(log_ctl, 0x18)
                && al->release_line == 0x61 && al->clears == clears + 1 && al->clear_text == g_fake->texts + 0x38
                && al->wait_at_clear == 1 && *g_fake->dialogue_wait == 2 && !live_menu(kRowTalk2),
            "cancel(rem4li2): its lines handed back to its chat log with +0x50, its own text object cleared, the"
            " event's wait told 2, the box closed by instance");
        phase(L,
            "local seen = talk_events(h)\n"
            "check(seen == 'closed:rem4li2', 'closed by the cancel: ' .. seen)\n"
            "return finish()\n");

        // the left button: the tick's test answered no while a hidden box is
        // the active menu, the key test's other callers untouched
        *g_fake->dialogue_ticks = 1;
        asked_on_game_thread(9, (1 << 8) | (1 << 16));
        wait_frames(2);
        uint8_t* box = live_menu(kRowTalk1);
        phase(L,
            "talk_events(h)\n"
            "check(h:hide('rem4line') == true, 'hide(rem4line) queues')\n"
            "return finish()\n");
        wait_frames(3);
        put32(g_fake->mcb, 0x54, static_cast<uint32_t>(reinterpret_cast<uintptr_t>(box)));
        const int clicks = al->key_clicks;
        const int seen_clicks = al->dialogue_clicks;
        const LONG frames = *g_fake->frames;
        *g_fake->left_click = 1;
        wait_frames(4);
        *g_fake->left_click = 0;
        const bool held_off = al->key_clicks == clicks && al->dialogue_clicks == seen_clicks
            && *g_fake->frames - frames >= 4 && live_menu(kRowTalk1) == box;
        put32(g_fake->mcb, 0x54, 0);
        *g_fake->left_click = 1;
        wait_frames(2);
        *g_fake->left_click = 0;
        const bool inactive = al->key_clicks > clicks && al->dialogue_clicks > seen_clicks;
        check(box && held_off && inactive,
            "hidden and active, rem4line's tick asks the key test for the left button and is answered no, the"
            " original never run, while the UI update's own call still runs the frame; not the active menu, the"
            " original answers and the click moves the page on");
        wait_frames(2);
        phase(L,
            "talk_events(h)\n"
            "check(h:unhide('rem4line') == true, 'unhide(rem4line)')\n"
            "return finish()\n");
        wait_frames(2);
        const int shown_clicks = al->dialogue_clicks;
        asked_on_game_thread(9, (1 << 8) | (1 << 16));
        wait_frames(2);
        box = live_menu(kRowTalk1);
        put32(g_fake->mcb, 0x54, static_cast<uint32_t>(reinterpret_cast<uintptr_t>(box)));
        *g_fake->left_click = 1;
        wait_frames(2);
        *g_fake->left_click = 0;
        put32(g_fake->mcb, 0x54, 0);
        check(box && al->dialogue_clicks > shown_clicks, "shown and active, the click reaches it");
        *g_fake->dialogue_ticks = 0;
        asked_on_game_thread(9, 0xFF);
        wait_frames(3);

        // the two boxes one dialogue: a hold on either is one on both,
        // refused on both alike, each name reporting it; a move or a resize
        // each box's own; an answer or a cancel through either name reaching
        // both, under either box's id
        phase(L,
            "talk_events(h) talk_events(h2)\n"
            "local function holds(n)\n"
            "  local i = h:info(n)\n"
            "  return (i.hidden and 'H' or '-') .. (i.blocked and 'B' or '-') .. ':' .. table.concat(i.hidden_by, ',')\n"
            "end\n"
            "local function pair() return holds('rem4line') .. ' ' .. holds('rem4li2') end\n"
            "check(pair() == '--: --:', 'neither box held: ' .. pair())\n"
            "check(h:hide('rem4line') == true and pair() == 'H-:alpha H-:alpha',"
            "  'hide(rem4line) hides both, each name reporting it: ' .. pair())\n"
            "local l = h:list()\n"
            "local listed = {}\n"
            "for _, nm in ipairs(hu.status().hidden) do listed[nm] = true end\n"
            "check(l.rem4line.hidden and l.rem4li2.hidden and listed.rem4line and listed.rem4li2"
            "  and h:info('rem4li2').mine.hidden and h2:info('rem4li2').mine.hidden == false,"
            "  'list(), status() and mine report both hidden, by alpha alone')\n"
            "check(h:unhide('rem4li2') == true and pair() == '--: --:', 'unhide(rem4li2) unhides both: ' .. pair())\n"
            "check(h:hide('rem4li2') == true and h2:hide('rem4line') == true and pair() == 'H-:alpha,beta H-:alpha,beta',"
            "  'two handles, each hiding one name, each hold both: ' .. pair())\n"
            "check(h:unhide('rem4line') == true and pair() == 'H-:beta H-:beta',"
            "  'one handle\\'s unhide leaves the other\\'s hold on both: ' .. pair())\n"
            "local ok, why = h:block('rem4li2')\n"
            "local ok2, why2 = h2:block('rem4line')\n"
            "check(ok == nil and why:find('rem4li2 cannot be blocked or closed', 1, true) == 1 and ok2 == nil"
            "  and why2:find('rem4line cannot be blocked or closed', 1, true) == 1 and pair() == 'H-:beta H-:beta',"
            "  'a block refused through either name, neither box changed: ' .. pair())\n"
            "check(h:unblock('rem4line') == true and h2:unblock('rem4li2') == true and pair() == 'H-:beta H-:beta',"
            "  'an unblock through either name changes neither: ' .. pair())\n"
            "check(h2:unhide('rem4li2') == true and pair() == '--: --:',"
            "  'the last hold dropped through the other name: neither hidden: ' .. pair())\n"
            "return finish()\n");
        asked_on_game_thread(9, (1 << 8) | (1 << 16));
        asked_on_game_thread(10, (1 << 8) | (1 << 16));
        wait_frames(2);
        phase(L,
            "local seen = talk_events(h)\n"
            "check(seen:find('opened:rem4line', 1, true) and seen:find('opened:rem4li2', 1, true)"
            "  and seen:find('page:rem4line', 1, true) and seen:find('page:rem4li2', 1, true),"
            "  'both boxes open on one message: ' .. seen)\n"
            "talk_placed = {h:info('rem4line'), h:info('rem4li2')}\n"
            "local a = talk_placed[1]\n"
            "local up = {}\n"
            "for _, nm in ipairs(h:opened()) do up[nm] = true end\n"
            "check(a.open and talk_placed[2].open and up.rem4line and up.rem4li2, 'info() and opened() report each box')\n"
            "check(h:move('rem4line', a.rect.x + 10, a.rect.y - 20) == true and h:resize('rem4li2', 300, 40) == true,"
            "  'move(rem4line) and resize(rem4li2) queue')\n"
            "return finish()\n");
        wait_frames(3);
        // with the game paused, a reply through the other name meets the
        // queued one
        pause_game();
        phase(L,
            "local a, b = talk_placed[1], talk_placed[2]\n"
            "local a2, b2 = h:info('rem4line'), h:info('rem4li2')\n"
            "local l = h:list()\n"
            "check(a2.rect.x == a.rect.x + 10 and a2.rect.y == a.rect.y - 20 and b2.rect.x == b.rect.x"
            "  and b2.rect.y == b.rect.y and l.rem4line.moved and not l.rem4li2.moved,"
            "  'move(rem4line) places rem4line alone')\n"
            "check(l.rem4li2.resized and not l.rem4line.resized and a2.rect.w == a.rect.w and a2.rect.h == a.rect.h,"
            "  'resize(rem4li2) sizes rem4li2 alone')\n"
            "check(h:reset('rem4line') == true and h:reset('rem4li2') == true, 'both reset')\n"
            "local o1, o2 = h:options('rem4line'), h:options('rem4li2')\n"
            "talk_id = o1.id\n"
            "check(o1.text == 'Moogle : Welcome home,\\nkupo!' and o2.text == o1.text and o1.id == o2.id,"
            "  'both show the first page under one id')\n"
            "check(h:answer('rem4line', true, o2.id) == true, 'answer(rem4line) under the id options(rem4li2) gives"
            " queues')\n"
            "local r, w = h2:answer('rem4li2')\n"
            "local r2, w2 = h:cancel('rem4li2', o2.id)\n"
            "check(r == nil and w == 'a reply to rem4line is already queued' and r2 == nil and w2 == w,"
            "  'a reply through rem4li2, from either handle, refused while one through rem4line is queued: '"
            "  .. tostring(w) .. ' / ' .. tostring(w2))\n"
            "return finish()\n");
        const int pair_advances = al->advances;
        const int pair_sounds = al->page_sounds;
        const int pair_codes = al->dispatch_codes;
        resume_game();
        wait_frames(3);
        check(al->advances == pair_advances + 2 && al->advance_text == g_fake->texts + 0x38
                && al->page_sounds == pair_sounds + 2 && al->dispatch_codes == pair_codes && live_menu(kRowTalk1)
                && live_menu(kRowTalk2),
            "answer(rem4line): rem4line's text object advanced, then rem4li2's, each page waiting for a key, each"
            " advance with the page-turn sound once; no code handed to the text dispatch");
        pause_game();
        phase(L,
            "local seen = talk_events(h)\n"
            "local a, b = h:options('rem4line'), h:options('rem4li2')\n"
            "check(a and b and a.text == 'Rest well.' and b.text == 'Rest well.' and a.more == false and b.more == false"
            "  and seen:find('page:rem4line', 1, true) and seen:find('page:rem4li2', 1, true),"
            "  'neither box waits on the page answered: both show the next, each with its page event: ' .. seen)\n"
            "check(a.id == talk_id and b.id == talk_id, 'the next page under the same id, no box having opened')\n"
            "check(h:answer('rem4li2', true, a.id) == true, 'answer(rem4li2) under the id options(rem4line) gives"
            " queues')\n"
            "local r, w = h:answer('rem4line')\n"
            "check(r == nil and w == 'a reply to rem4li2 is already queued',"
            "  'a reply through rem4line refused while one through rem4li2 is queued: ' .. tostring(w))\n"
            "return finish()\n");
        resume_game();
        wait_frames(3);
        check(al->advances == pair_advances + 4 && al->page_sounds == pair_sounds + 4
                && al->dispatch_codes == pair_codes && !live_menu(kRowTalk1) && !live_menu(kRowTalk2),
            "answer(rem4li2) on the last page: the message ends in both boxes, and both close");
        phase(L,
            "local seen = talk_events(h)\n"
            "check(seen:find('closed:rem4line', 1, true) and seen:find('closed:rem4li2', 1, true)"
            "  and h:options('rem4line') == nil and h:options('rem4li2') == nil, 'both closed: ' .. seen)\n"
            "return finish()\n");
        asked_on_game_thread(9, 1 | (1 << 8) | (1 << 16));
        asked_on_game_thread(10, 1 | (1 << 8) | (1 << 16));
        wait_frames(2);
        *g_fake->query_cancel_allowed = 0;
        phase(L,
            "talk_events(h)\n"
            "local n1, n2 = h:options('rem4line'), h:options('rem4li2')\n"
            "check(n1.text == 'Moogle : One more thing.' and n2.text == n1.text and n1.id == n2.id"
            "  and n1.id == talk_id + 2, 'both open again on a new message under one new id, moved by each box\\'s'"
            "  .. ' open: ' .. tostring(n1.id) .. ' after ' .. tostring(talk_id))\n"
            "local gone = {}\n"
            "for _, id in ipairs({talk_id, talk_id + 1}) do\n"
            "  for _, nm in ipairs({'rem4line', 'rem4li2'}) do\n"
            "    local r, w = h:answer(nm, true, id)\n"
            "    local rc, wc = h:cancel(nm, id)\n"
            "    gone[#gone + 1] = tostring(r == nil and w) .. ',' .. tostring(rc == nil and wc)\n"
            "  end\n"
            "end\n"
            "check(table.concat(gone, ' / ') == ('that prompt is gone,that prompt is gone / '):rep(3)"
            "  .. 'that prompt is gone,that prompt is gone', 'the last message\\'s id, and the one current between'"
            "  .. ' the two boxes\\' opens, refused through either name: ' .. table.concat(gone, ' / '))\n"
            "local r, w = h:cancel('rem4line')\n"
            "check(r == nil and w == 'rem4line cannot be canceled now: the event allows no cancel'"
            "  and h:options('rem4line') and h:options('rem4li2'),"
            "  'a cancel the event allows not refused, both boxes as they were: ' .. tostring(w))\n"
            "return finish()\n");
        *g_fake->query_cancel_allowed = 1;
        *g_fake->dialogue_wait = 1;
        const int pair_releases = al->releases;
        const int pair_clears = al->clears;
        phase(L,
            "check(h:cancel('rem4li2') == true, 'cancel(rem4li2) queues')\n"
            "return finish()\n");
        wait_frames(3);
        const uint8_t* log1 = reinterpret_cast<const uint8_t*>(get32(controller_named("rem4line"), 0x64));
        check(al->releases == pair_releases + 2 && reinterpret_cast<uintptr_t>(al->release_log) == get32(log1, 0x18)
                && al->clears == pair_clears + 2 && al->clear_text == g_fake->texts && al->wait_at_clear == 2
                && *g_fake->dialogue_wait == 2 && !live_menu(kRowTalk1) && !live_menu(kRowTalk2),
            "cancel(rem4li2): rem4li2's own cancel, then rem4line's: its lines handed back to its own chat log and"
            " its text object cleared after rem4li2's told the wait 2, which it tells again; both boxes closed");
        phase(L,
            "local seen = talk_events(h)\n"
            "check(seen:find('closed:rem4line', 1, true) and seen:find('closed:rem4li2', 1, true)"
            "  and h:options('rem4line') == nil and h:options('rem4li2') == nil, 'the event ended in both: ' .. seen)\n"
            "return finish()\n");
        asked_on_game_thread(9, 1 | (1 << 8) | (1 << 16));
        asked_on_game_thread(10, 1 | (1 << 8) | (1 << 16));
        wait_frames(2);
        *g_fake->dialogue_wait = 1;
        const int line_releases = al->releases;
        phase(L,
            "talk_events(h)\n"
            "check(h:cancel('rem4line', h:options('rem4li2').id) == true,"
            "  'cancel(rem4line) under the id options(rem4li2) gives queues')\n"
            "return finish()\n");
        wait_frames(3);
        const uint8_t* log2 = reinterpret_cast<const uint8_t*>(get32(controller_named("rem4li2"), 0x64));
        check(al->releases == line_releases + 2 && reinterpret_cast<uintptr_t>(al->release_log) == get32(log2, 0x18)
                && al->clear_text == g_fake->texts + 0x38 && al->wait_at_clear == 2 && *g_fake->dialogue_wait == 2
                && !live_menu(kRowTalk1) && !live_menu(kRowTalk2),
            "cancel(rem4line): rem4line's own cancel, then rem4li2's; both boxes closed");
        phase(L,
            "local seen = talk_events(h)\n"
            "check(seen:find('closed:rem4line', 1, true) and seen:find('closed:rem4li2', 1, true),"
            "  'the event ended in both: ' .. seen)\n"
            "return finish()\n");
        *g_fake->close_frames = 0;
        phase(L,
            "for _, x in ipairs({h, h2}) do x:poll() end\n"
            "return finish()\n");
    }

    // -- the text-entry windows (0.11.0): stringdl, comment, msgline, scname
    //    and link11, their routines the game's bytes where the engine reads
    //    and runs them: open, close and block refused as the rules give,
    //    options() in plain fields, answer and cancel doing what each window's
    //    own commit and cancel do, a table answer as the binding encodes it;
    //    the line taken from the window's own field alone at a hide and at
    //    a hidden open, passinpu's field suspended under a draw that reads
    //    it, the fields started again at the show, the active menu's last;
    //    msgline's message blocked, listed by pending(),
    //    answered with no window, its end a pending event the other handle
    //    reads
    {
        FakeLog* tl = g_fake->log;
        typedef int (__fastcall* CloseFn)(void* mcb, void* edx, const char* key);
        CloseFn close_by_name;
        memcpy(&close_by_name, &g_fake->close, sizeof(close_by_name));
        auto game_close = [&](const char* key) {
            pause_game();
            close_by_name(g_fake->mcb, NULL, key);
            resume_game();
            wait_frames(2);
        };
        uint8_t* delivery = controller_named("delivery");
        uint8_t* sd = controller_named("stringdl");
        uint8_t* cm = controller_named("comment");
        uint8_t* ml = controller_named("msgline");
        uint8_t* sc = controller_named("scname");
        uint8_t* lk = controller_named("link11");
        uint8_t* pi = controller_named("passinpu");
        uint8_t* link5 = controller_named("link5");
        phase(L,
            "for _, x in ipairs({h, h2}) do x:poll() end\n"
            "function text_errors(handle)\n"
            "  local out = {}\n"
            "  for _, e in ipairs((handle:poll())) do\n"
            "    if e.event == 'error' then out[#out + 1] = e.verb .. ' ' .. e.name .. ': ' .. e.reason end\n"
            "  end\n"
            "  return table.concat(out, ' | ')\n"
            "end\n"
            "local entry = \"stringdl cannot be blocked or closed: the outgoing box gives up when its name entry\"\n"
            "  .. \" cannot open, which ends the delivery session; hide it instead, and answer('stringdl') or\"\n"
            "  .. \" cancel('stringdl')\"\n"
            "local ok, w = h:block('stringdl')\n"
            "check(ok == nil and w == entry, 'block(stringdl) refused: ' .. tostring(w))\n"
            "ok, w = h:close('stringdl')\n"
            "check(ok == nil and w == \"stringdl is a prompt the game waits on; use cancel('stringdl')\","
            "  'close(stringdl) refused: ' .. tostring(w))\n"
            "ok, w = h:close('msgline')\n"
            "check(ok == true, 'close(msgline) with nothing asked queues: ' .. tostring(w))\n"
            "ok, w = h:open('comment')\n"
            "check(ok == nil and w == \"comment opens only for the game's own event; answer it instead\","
            "  'open(comment) refused: ' .. tostring(w))\n"
            "ok, w = h:open('scname')\n"
            "check(ok == nil and w == \"scname opens only for the game's own event; answer or cancel it instead\","
            "  'open(scname) refused: ' .. tostring(w))\n"
            "ok, w = h:cancel('comment')\n"
            "check(ok == nil and w == \"comment has no cancel: closing it sends the text, as the game's Escape does;\""
            "  .. ' answer it', 'cancel(comment) refused: ' .. tostring(w))\n"
            "local o, why = h:options('scname')\n"
            "check(o == nil and why == 'scname is not open, and nothing asks through it until the game opens it',"
            "  'options(scname) with no window: ' .. tostring(why))\n"
            "o, why = h:options('msgline')\n"
            "check(o == nil and why == 'no message is being written in msgline', 'options(msgline) with none: '"
            "  .. tostring(why))\n"
            "return finish()\n");

        // stringdl: the name the outgoing box asks for, then a yes or no on
        // a slot
        put16(delivery, 0x1B4, 6);
        open_on_game_thread("menu    delivery");
        open_on_game_thread("menu    stringdl");
        sd[0x14] = 1;
        sd[0x164] = 0;
        put16(sd, 0x15C, 0);
        strcpy(reinterpret_cast<char*>(sd + 0x118), "Send to whom?");
        wait_frames(2);
        const int focus_before = tl->focus_calls;
        phase(L,
            "local o = h:options('stringdl')\n"
            "check(o and o.mode == 'name' and o.min_length == 3 and o.max_length == 15 and o.prompt == 'Send to"
            " whom?' and o.name == 'stringdl' and type(o.id) == 'number', 'options(stringdl): the name, its lengths"
            " and the line over the field')\n"
            "local ok, w = h:answer('stringdl', 'Ro')\n"
            "local letters = \"the recipient's name is 3 to 15 letters, A to Z, and nothing else; the game sets their\""
            "  .. ' case itself'\n"
            "check(ok == nil and w == letters, 'two letters refused: ' .. tostring(w))\n"
            "ok, w = h:answer('stringdl', 'Rolan1')\n"
            "check(ok == nil and w == letters, 'a digit refused: ' .. tostring(w))\n"
            "local raised, err = pcall(h.answer, h, 'stringdl', 12)\n"
            "check(not raised and tostring(err):find(\"answer('stringdl', name) takes the recipient's name\", 1, true),"
            "  'answer(stringdl, 12) raises: ' .. tostring(err))\n"
            "ok, w = h:answer('stringdl', true)\n"
            "check(ok == nil and w == \"stringdl asks for the recipient's name now: answer('stringdl', name)\","
            "  'a yes while it asks the name refused: ' .. tostring(w))\n"
            "check(h:answer('stringdl', 'rolanberry', o.id) == true, 'answer(stringdl, rolanberry, id) queues')\n"
            "return finish()\n");
        wait_frames(3);
        check(memcmp(sd + 0x58, "rolanberry", 11) == 0 && get16(sd, 0x15C) == 1 && tl->focus_calls == focus_before + 2
                && tl->focus_menu == live_menu(row_named("delivery")),
            "answer(stringdl, name): the name into +0x58, +0x15C 1, the outgoing box raised twice");
        put16(sd, 0x15C, 0);
        sd[0x14] = 2;
        put16(delivery, 0x1B4, 0x10);
        phase(L,
            "local o = h:options('stringdl')\n"
            "check(o and o.mode == 'confirm' and o.max_length == nil, 'options(stringdl) about a slot: confirm')\n"
            "local ok, w = h:answer('stringdl', 'Rolanberry')\n"
            "check(ok == nil and w == \"stringdl asks yes or no about a delivery slot now: answer('stringdl', true or\""
            "  .. ' false)', 'a name while it asks about a slot refused: ' .. tostring(w))\n"
            "check(h:answer('stringdl', true) == true, 'answer(stringdl, true) queues')\n"
            "return finish()\n");
        wait_frames(3);
        const bool yes = get16(sd, 0x15C) == 3;
        put16(sd, 0x15C, 0);
        phase(L,
            "check(h:cancel('stringdl') == true, 'cancel(stringdl) queues')\n"
            "return finish()\n");
        wait_frames(3);
        check(yes && get16(sd, 0x15C) == 4, "on a slot, yes writes 3 and the cancel 4");
        put16(sd, 0x15C, 0);
        put16(delivery, 0x1B4, 0);
        phase(L,
            "local o, why = h:options('stringdl')\n"
            "check(o == nil and why == 'stringdl is not asking anything now', 'between steps it asks nothing: '"
            "  .. tostring(why))\n"
            "return finish()\n");
        game_close("menu    stringdl");
        game_close("menu    delivery");

        // hidden stringdl, as found live: held while the outgoing box asks
        // the name through it and until the box has read the name answered;
        // with the box moved on, stringdl focused again asks nothing, and
        // the player's Escape and the menu mouse mode reach it as shown
        {
            typedef uint32_t (__fastcall* InputFn)(void* self, void* edx, int code);
            typedef void (__fastcall* MouseFn)(void* mouse, void* edx);
            InputFn route;
            MouseFn mouse_mode;
            memcpy(&route, &g_fake->menu_routing, sizeof(route));
            memcpy(&mouse_mode, &g_fake->mouse_mode, sizeof(mouse_mode));
            static uint8_t mouse[0x60];
            phase(L,
                "check(h:hide('stringdl') == true, 'hide(stringdl)')\n"
                "return finish()\n");
            wait_frames(3);
            put16(delivery, 0x1B4, 6);
            sd[0x14] = 1;
            sd[0x164] = 0;
            put16(sd, 0x15C, 0);
            uint8_t* entry = static_cast<uint8_t*>(open_on_game_thread("menu    stringdl"));
            wait_frames(2);
            // 1: the routed code reached its sink and the picker's original
            // ran; 0: the code was dropped and the pre held world mode
            auto input = [&](int code) {
                const int sinks = tl->sink_calls;
                put32(g_fake->mcb, 0x54, reinterpret_cast<uint32_t>(entry));
                const uint32_t routed = route(g_fake->mcb, NULL, code);
                mouse[0x4D] = 0;
                mouse_mode(mouse, NULL);
                put32(g_fake->mcb, 0x54, 0);
                if (routed == 1 && tl->sink_calls == sinks + 1 && tl->sink_menu == entry && tl->sink_code == code
                    && mouse[0x4D] == 2) {
                    return 1;
                }
                return routed == 0 && tl->sink_calls == sinks && mouse[0x4D] == 1 ? 0 : -1;
            };
            const bool asks = entry && hidden_in_place(row_named("stringdl"), entry) && input(5) == 0;
            phase(L,
                "check(h:answer('stringdl', 'rolanberry') == true, 'answer(stringdl, rolanberry) queues')\n"
                "return finish()\n");
            wait_frames(3);
            const bool unread = get16(sd, 0x15C) == 1 && input(5) == 0;
            put16(sd, 0x15C, 0);
            put16(delivery, 0x1B4, 9);
            wait_frames(2);
            const bool escape = input(6) == 1;
            phase(L,
                "local o, why = h:options('stringdl')\n"
                "check(o == nil and why == 'stringdl is not asking anything now', 'hidden, the box moved on: '"
                "  .. tostring(why))\n"
                "check(h:unhide('stringdl') == true, 'unhide(stringdl)')\n"
                "return finish()\n");
            wait_frames(3);
            check(asks && unread && escape,
                "hidden stringdl: no routed code while the outgoing box asks the name through it, nor once it is"
                " answered until the box has read it; with the box moved on, options() finds it asking nothing and"
                " the player's Escape reaches its sink, the picker's original giving the menu mouse mode");
            put16(delivery, 0x1B4, 0);
            sd[0x14] = 0;
            game_close("menu    stringdl");
        }

        // comment: opened, its open hook starts line 0's field; a hide ends
        // it; the lines answered as a table
        cm[0x16] = 0;
        memset(cm + 0x1A, 0, 0x29 * 3);
        strcpy(reinterpret_cast<char*>(cm + 0x1A), "Old line");
        int attaches = tl->attaches;
        int deletes = tl->field_deletes;
        open_on_game_thread("menu    comment ");
        uint8_t* field = NULL;
        memcpy(&field, cm + 0x378, 4);
        const bool started = field != NULL && tl->attaches == attaches + 1 && tl->attached == field;
        phase(L,
            "check(h:hide('comment') == true, 'hide(comment)')\n"
            "return finish()\n");
        wait_frames(3);
        memcpy(&field, cm + 0x378, 4);
        check(started && field == NULL && tl->field_deletes == deletes + 1 && tl->attached == NULL
                && static_cast<int8_t>(cm[0x14]) == -1,
            "comment's open hook starts line 0's field; hide(comment) ends it with the window's own end, the field"
            " deleted and the text entry turned off");
        const int sends = tl->comment_sends;
        const int sanitizes = tl->sanitizes;
        phase(L,
            "local o = h:options('comment')\n"
            "check(o and o.mode == 'bazaar' and #o.lines == 3 and o.lines[1].text == 'Old line' and o.lines[2].text"
            "  == '' and o.category == nil, 'options(comment): the bazaar message and its lines')\n"
            "local ok, w = h:answer('comment', {'Hello', string.rep('x', 41)})\n"
            "check(ok == nil and w == 'line 2 is 41 bytes; comment takes at most 40 a line', 'a long line refused: '"
            "  .. tostring(w))\n"
            "local raised, err = pcall(h.answer, h, 'comment', 'Hello')\n"
            "check(not raised and tostring(err):find(\"answer('comment', lines) takes a table of up to 3 strings\", 1,"
            "  true), 'a string raises: ' .. tostring(err))\n"
            "raised, err = pcall(h.answer, h, 'comment', {'a', 'b', 'c', 'd'})\n"
            "check(not raised, 'four lines raise')\n"
            "raised, err = pcall(h.answer, h, 'comment', {x = 'a'})\n"
            "check(not raised, 'a named key raises')\n"
            "check(h:answer('comment', {'Hello', [3] = 'World'}) == true, 'answer(comment, lines) queues')\n"
            "return finish()\n");
        wait_frames(3);
        check(tl->comment_sends == sends + 1 && tl->comment_mode == 0 && strcmp(tl->comment_lines[0], "Hello") == 0
                && tl->comment_lines[1][0] == '\0' && strcmp(tl->comment_lines[2], "World") == 0
                && tl->sanitizes == sanitizes + 3 && !live_menu(row_named("comment")),
            "answer(comment, lines): each line of the slot written, sanitized and trimmed, then the window closed"
            " by instance, its begin-close sending them");
        phase(L,
            "check(h:unhide('comment') == true, 'unhide(comment)')\n"
            "return finish()\n");
        wait_frames(2);

        // msgline: a message written and sent; blocked, answered with no
        // window
        put16(ml, 0x14, 0);
        put32(ml, 0x198, 0x1111);
        put32(ml, 0x19C, 0x00051234);
        strcpy(reinterpret_cast<char*>(ml + 0x188), "Friend");
        put32(g_fake->pol, 0x80, 0x55);
        *g_fake->msgline_busy = 0;
        *g_fake->send_result = 0;
        open_on_game_thread("menu    msgline ");
        const int marks = tl->marks;
        phase(L,
            "local o = h:options('msgline')\n"
            "check(o and o.mode == 0 and o.max_length == 299 and o.recipient == 'Friend' and o.blocked == false,"
            "  'options(msgline): the mode, its length and the recipient')\n"
            "local ok, w = h:answer('msgline', '')\n"
            "check(ok == nil and w == \"empty text sends nothing; cancel('msgline') instead\", 'empty refused: '"
            "  .. tostring(w))\n"
            "ok, w = h:answer('msgline', string.rep('x', 300))\n"
            "check(ok == nil and w == '300 bytes; msgline takes at most 299 (15 in its modes 2 and 7)',"
            "  '300 bytes refused: the game keeps 299: ' .. tostring(w))\n"
            "check(h:answer('msgline', 'Hello there') == true, 'answer(msgline, text) queues')\n"
            "return finish()\n");
        wait_frames(3);
        check(tl->send_mode == 0 && strcmp(tl->send_text, "Hello there") == 0 && tl->send_args[0] == 0x1111
                && tl->send_args[1] == 0x05341234 && tl->send_args[6] == 0x11 && tl->marks == marks + 1
                && tl->mark_obj == g_fake->sending_owner + 0x20 && *g_fake->msgline_busy == 1
                && !live_menu(row_named("msgline")),
            "answer(msgline): sent through mode 0's sender with +0x198 and the number packed with +0x19C, the"
            " indicator marked, the busy byte set, msgline closed");
        phase(L,
            "check(h:block('msgline') == true, 'block(msgline)')\n"
            "for _, x in ipairs({h, h2}) do x:poll() end\n"
            "return finish()\n");
        wait_frames(2);
        put16(ml, 0x14, 7);
        const bool refused_open = open_on_game_thread("menu    msgline ") == NULL;
        wait_frames(2);
        phase(L,
            "local seen = {}\n"
            "for _, e in ipairs((h2:poll())) do\n"
            "  if e.event == 'pending' or e.name == 'msgline' then\n"
            "    seen[#seen + 1] = e.event .. ':' .. tostring(e.what or e.name) .. ':' .. tostring(e.pending)\n"
            "  end\n"
            "end\n"
            "check(table.concat(seen, ' ') == 'blocked:msgline:nil pending:msgline:true', 'the refused open:'"
            "  .. ' blocked, then pending: ' .. table.concat(seen, ' '))\n"
            "h:poll()\n"
            "local p = h:pending()\n"
            "check(p.msgline and p.msgline.mode == 7 and p.msgline.blocked == true and p.msgline.max_length == 15,"
            "  'pending() lists the blocked message under msgline')\n"
            "local ok, w = h:answer('msgline', 'Hi')\n"
            "check(ok == nil and w == 'a message is still on its way; msgline sends the next once it has gone',"
            "  'refused while the busy byte is set: ' .. tostring(w))\n"
            "return finish()\n");
        *g_fake->msgline_busy = 0;
        phase(L,
            "local p = h:pending()\n"
            "local ok, w = h:answer('msgline', 'sixteen bytes!!!')\n"
            "check(ok == nil and w == \"16 bytes; msgline's mode 7 takes at most 15\", 'past mode 7\\'s field'"
            "  .. ' refused: ' .. tostring(w))\n"
            "check(h:answer('msgline', 'Hi', p.msgline.id) == true, 'answer(msgline, text, id) queues with no window')\n"
            "return finish()\n");
        wait_frames(3);
        phase(L,
            "local own = {}\n"
            "for _, e in ipairs((h:poll())) do\n"
            "  if e.event == 'pending' then own[#own + 1] = e.what end\n"
            "end\n"
            "local other = {}\n"
            "for _, e in ipairs((h2:poll())) do\n"
            "  if e.event == 'pending' then other[#other + 1] = e.what .. ':' .. tostring(e.pending) end\n"
            "end\n"
            "check(#own == 0 and table.concat(other, ' ') == 'msgline:false', 'the reply\\'s handle skips the end;'"
            "  .. ' the other reads it: ' .. table.concat(other, ' '))\n"
            "check(h:pending().msgline == nil, 'pending() lists it no more')\n"
            "check(h:unblock('msgline') == true, 'unblock(msgline)')\n"
            "return finish()\n");
        check(refused_open && tl->send_mode == 7 && strcmp(tl->send_text, "Hi") == 0 && tl->send_args[8] == 0x15
                && *g_fake->msgline_busy == 1,
            "blocked msgline in mode 7: the message sent with no window, through mode 7's sender");
        *g_fake->msgline_busy = 0;
        wait_frames(2);

        // scname: the searched name
        open_on_game_thread("menu    scname  ");
        const int searches = tl->searches;
        phase(L,
            "local o = h:options('scname')\n"
            "check(o and o.max_length == 15 and type(o.text) == 'string' and o.name == 'scname', 'options(scname)')\n"
            "local ok, w = h:answer('scname', 'Rolan Berry')\n"
            "check(ok == nil and w == 'the name to search for is at most 15 letters, A to Z, and nothing else',"
            "  'a space refused: ' .. tostring(w))\n"
            "check(h:answer('scname', 'Rolan') == true, 'answer(scname, name) queues')\n"
            "return finish()\n");
        wait_frames(3);
        check(tl->searches == searches + 1 && strcmp(tl->criteria_name, "Rolan") == 0
                && strcmp(reinterpret_cast<const char*>(sc + 0x18), "Rolan") == 0 && tl->search_args[0] == 1
                && !live_menu(row_named("scname")),
            "answer(scname, name): the name into +0x18, the criteria named and the search started, scname closed");

        // link11: link5's selected row's new linkshell
        open_on_game_thread("menu    link5   ");
        uint8_t* list = live_menu(row_named("link5"));
        uint8_t saved5[0x60];
        memcpy(saved5, link5 + 0x10, sizeof(saved5));
        static uint8_t rows[3][0x54];
        static uint8_t record[4] = {5, 0, 0, 0};
        memset(rows, 0, sizeof(rows));
        uint8_t* rec = record;
        memcpy(rows[1] + 0x48, &rec, 4);
        uint8_t* first = rows[0];
        memcpy(link5 + 0x54, &first, 4);
        put32(link5, 0x50, 3);
        put16(link5, 0x1E, 0);
        put16(list, 0x4C, 2);
        open_on_game_thread("menu    link11  ");
        lk[0x30] = 7;
        lk[0x31] = 7;
        lk[0x32] = 7;
        const int creates = tl->creates;
        phase(L,
            "local o = h:options('link11')\n"
            "check(o and o.max_length == 19 and o.color and o.color[1] == 7 and o.color[3] == 7,"
            "  'options(link11): the name\\'s length and the colors')\n"
            "local ok, w = h:answer('link11', {name = 'Kupo1'})\n"
            "check(ok == nil and w == 'the linkshell\\'s name is 1 to 19 letters, A to Z, and nothing else',"
            "  'a digit refused: ' .. tostring(w))\n"
            "ok, w = h:answer('link11', {name = 'Kupo', color = {1, 2, 16}})\n"
            "check(ok == nil and w == 'each color is a whole number from 0 to 15', 'a color past 15 refused: '"
            "  .. tostring(w))\n"
            "local raised = pcall(h.answer, h, 'link11', {name = 'Kupo', color = {1, 2}})\n"
            "check(not raised, 'two colors raise')\n"
            "raised = pcall(h.answer, h, 'link11', 'Kupo')\n"
            "check(not raised, 'a string raises')\n"
            "check(h:answer('link11', {name = 'Kupo', color = {1, 2, 3}}) == true, 'answer(link11, {name, color})"
            " queues')\n"
            "return finish()\n");
        wait_frames(3);
        check(tl->creates == creates + 1 && tl->create_args[0] == 1 && tl->create_args[2] == 5
                && strcmp(tl->create_name, "<Kupo>") == 0 && tl->create_args[5] == 1 && tl->create_args[7] == 3
                && g_fake->made[0] == 1 && g_fake->made[9] == 0x3C && (link5[0x65] & 1)
                && !live_menu(row_named("link11")) && live_menu(row_named("link6")),
            "answer(link11, {name, color}): the request for link5's selected row sent with the encoded name and"
            " the colors, its bytes written, link11 closed and link6 opened");
        memcpy(link5 + 0x10, saved5, sizeof(saved5));
        game_close("menu    link6   ");
        game_close("menu    link5   ");

        // passinpu: a hide while its field holds the line suspends the
        // field, the manager's unbind then its bind of NULL, and its draw,
        // run each frame while hidden, reads the field kept; an open while
        // hidden suspends the field its hook binds again; the show drops
        // the field, the typed text with it, and starts a new one; a cancel
        // while hidden drops the field once, through the begin-close, and no
        // draw runs after it
        auto at = [](const uint8_t* p, size_t off) {
            uint8_t* v;
            memcpy(&v, p + off, 4);
            return v;
        };
        auto holder = [&]() {
            return g_fake->edit[8] ? at(g_fake->edit, 4) : NULL;
        };
        auto pass_hidden = [&](bool on) {
            phase(L, on ? "check(h:hide('passinpu') == true, 'hide(passinpu)')\nreturn finish()\n"
                : "check(h:unhide('passinpu') == true, 'unhide(passinpu)')\nreturn finish()\n");
            wait_frames(3);
        };
        game_close("menu    passinpu");
        InterlockedExchange(g_fake->draw_walk, 1);
        const int bad = tl->bad_fields;
        open_on_game_thread("menu    passinpu");
        wait_frames(2);
        uint8_t* pass1 = at(pi, 0x38);
        const bool bound = pass1 && holder() == pass1 && *g_fake->line_mode == 5 && at(g_fake->ime, 0x2218) == pass1;
        strcpy(g_fake->field_typed, "secret");
        deletes = tl->field_deletes;
        int news = tl->field_news;
        tl->line_modes[0] = '\0';
        pass_hidden(true);
        int draws = tl->passinpu_draws;
        wait_frames(2);
        const bool suspended = at(pi, 0x38) == pass1 && pass1[0x694] == 0 && tl->field_deletes == deletes
            && g_fake->edit[8] == 1 && !at(g_fake->edit, 4) && *g_fake->line_mode == 3 && !at(g_fake->ime, 0x2218)
            && strcmp(tl->line_modes, "23") == 0 && strcmp(reinterpret_cast<char*>(pass1 + 0x12), "secret") == 0
            && tl->passinpu_draws > draws && tl->bad_fields == bad
            && hidden_in_place(row_named("passinpu"), live_menu(row_named("passinpu")));
        check(bound && suspended, "hide(passinpu) while its field holds the line: the field kept, unbound, its text"
            " in it; the manager's unbind (mode 2) then its bind of NULL (mode 3, no field, the IME display's"
            " cleared); its draw runs each frame hidden and reads only a live field");
        open_on_game_thread("menu    passinpu");
        wait_frames(2);
        const bool hidden_open = at(pi, 0x38) == pass1 && tl->field_news == news && tl->field_deletes == deletes
            && pass1[0x694] == 0 && g_fake->edit[8] == 1 && !at(g_fake->edit, 4) && *g_fake->line_mode == 3
            && strcmp(reinterpret_cast<char*>(pass1 + 0x12), "secret") == 0 && tl->bad_fields == bad;
        tl->line_modes[0] = '\0';
        pass_hidden(false);
        uint8_t* pass2 = at(pi, 0x38);
        const bool shown = tl->field_deletes == deletes + 1 && tl->field_deleted == pass1
            && tl->field_news == news + 1 && pass2 && pass2 != pass1 && holder() == pass2 && pass2[0x694] == 1
            && *g_fake->line_mode == 5 && at(g_fake->ime, 0x2218) == pass2 && g_fake->field_typed[0] == '\0'
            && strcmp(tl->line_modes, "2325") == 0 && tl->bad_fields == bad;
        check(hidden_open && shown, "passinpu opened again while hidden: its start binds the field it has, then the"
            " field is suspended at once; unhide(passinpu): the field dropped (one delete), a new one started"
            " and bound (mode 5), the typed text gone");
        static int pass_context;
        pass_hidden(true);
        put32(pi, 0x1C, reinterpret_cast<uint32_t>(g_fake->text_callback));
        put32(pi, 0x20, reinterpret_cast<uint32_t>(&pass_context));
        deletes = tl->field_deletes;
        news = tl->field_news;
        phase(L,
            "check(h:cancel('passinpu') == true, 'cancel(passinpu) while hidden')\n"
            "return finish()\n");
        wait_frames(3);
        draws = tl->passinpu_draws;
        wait_frames(2);
        const bool canceled = tl->field_deletes == deletes + 1 && tl->field_deleted == pass2 && !at(pi, 0x38)
            && !live_menu(row_named("passinpu")) && tl->passinpu_draws == draws && tl->bad_fields == bad;
        pass_hidden(false);
        check(canceled && tl->field_news == news && !at(pi, 0x38),
            "cancel(passinpu) while hidden: its begin-close drops the field once, and no draw runs after it; the"
            " show starts none");

        // shown while msgline's field holds the line: msgline's field ended
        // keeping its text first, msgline's draw then finding none
        pass_hidden(true);
        open_on_game_thread("menu    passinpu");
        wait_frames(2);
        uint8_t* pass3 = at(pi, 0x38);
        put16(ml, 0x14, 0);
        memset(ml + 0x53, 0, 0x12C);
        open_on_game_thread("menu    msgline ");
        wait_frames(2);
        const bool message_bound = at(ml, 0x1C) && holder() == at(ml, 0x1C);
        strcpy(g_fake->field_typed, "draft");
        deletes = tl->field_deletes;
        pass_hidden(false);
        uint8_t* pass4 = at(pi, 0x38);
        int message_draws = tl->msgline_draws;
        wait_frames(2);
        const bool taken = message_bound && !at(ml, 0x1C) && strcmp(reinterpret_cast<char*>(ml + 0x53), "draft") == 0
            && tl->field_deletes == deletes + 2 && pass4 && pass4 != pass3 && holder() == pass4
            && *g_fake->line_mode == 5 && live_menu(row_named("msgline")) && tl->msgline_draws > message_draws
            && tl->bad_fields == bad;
        check(taken, "unhide(passinpu) while msgline's field holds the line: msgline's field ended keeping its text"
            " (+0x53), msgline left open with no field, its draw reading nothing; passinpu's new field holds the"
            " line");

        // the manager bound to no field: hidden passinpu with no field (no
        // draw walking it, which would read it) gets no unbind and no bind;
        // shown with msgline open but its field gone, nothing of msgline's
        // is ended
        pass_hidden(true);
        InterlockedExchange(g_fake->draw_walk, 0);
        pause_game();
        put32(pi, 0x38, 0);
        tl->line_modes[0] = '\0';
        resume_game();
        wait_frames(3);
        const bool quiet = tl->line_modes[0] == '\0' && g_fake->edit[8] == 1 && !at(g_fake->edit, 4);
        pause_game();
        put32(pi, 0x38, reinterpret_cast<uint32_t>(pass4));
        resume_game();
        InterlockedExchange(g_fake->draw_walk, 1);
        strcpy(reinterpret_cast<char*>(ml + 0x53), "kept");
        pass_hidden(false);
        const bool spared = strcmp(reinterpret_cast<char*>(ml + 0x53), "kept") == 0 && at(pi, 0x38)
            && holder() == at(pi, 0x38) && strcmp(tl->line_modes, "2325") == 0 && tl->bad_fields == bad;
        check(quiet && spared, "the manager bound to no field: hidden passinpu with no field, no setmode at all;"
            " unhide(passinpu) with msgline's field gone: msgline's end not called, passinpu's field started");
        game_close("menu    passinpu");
        game_close("menu    msgline ");

        // stringdl editing, hidden while msgline's field holds the line: its
        // end, whose unbind would take the line from msgline's field, is not
        // called, and msgline's draw finds its manager
        open_on_game_thread("menu    msgline ");
        open_on_game_thread("menu    stringdl");
        wait_frames(2);
        uint8_t* message = at(ml, 0x1C);
        sd[0x164] = 1;
        phase(L,
            "check(h:hide('stringdl') == true, 'hide(stringdl)')\n"
            "return finish()\n");
        wait_frames(3);
        message_draws = tl->msgline_draws;
        wait_frames(2);
        const bool spared_line = sd[0x164] == 1 && message && holder() == message && message[0x694] == 1
            && at(message, 0x6A0) == g_fake->edit && *g_fake->line_mode == 5 && tl->msgline_draws > message_draws
            && tl->bad_fields == bad;
        phase(L,
            "check(h:unhide('stringdl') == true, 'unhide(stringdl)')\n"
            "return finish()\n");
        wait_frames(3);
        check(spared_line, "hide(stringdl) editing while msgline's field holds the line: stringdl's end not called,"
            " msgline's field still bound and its draw reading its manager");
        sd[0x164] = 0;
        game_close("menu    stringdl");
        game_close("menu    msgline ");

        // comment and msgline: a hide ends the field holding the line; both
        // shown in one pass start again in inventory order, the game's active
        // menu's last: with neither active, comment's on line 2, the one it
        // edited, then msgline's, which keeps the line; with comment active,
        // msgline's first, then comment's, msgline's ended keeping its text
        auto hide_both = [&](int line) {
            cm[0x16] = 0;
            put16(ml, 0x14, 0);
            open_on_game_thread("menu    comment ");
            wait_frames(2);
            cm[0x14] = static_cast<uint8_t>(line);
            phase(L,
                "check(h:hide('comment') == true, 'hide(comment)')\n"
                "return finish()\n");
            wait_frames(3);
            open_on_game_thread("menu    msgline ");
            wait_frames(2);
            phase(L,
                "check(h:hide('msgline') == true, 'hide(msgline)')\n"
                "return finish()\n");
            wait_frames(3);
        };
        auto show_both = [&](const uint8_t* active) {
            pause_game();
            put32(g_fake->mcb, 0x54, reinterpret_cast<uint32_t>(active));
            resume_game();
            phase(L,
                "check(h:unhide('comment') == true and h:unhide('msgline') == true, 'unhide(comment), unhide(msgline)')\n"
                "return finish()\n");
            wait_frames(3);
            put32(g_fake->mcb, 0x54, 0);
        };
        deletes = tl->field_deletes;
        hide_both(2);
        uint8_t* comment_field = at(cm, 0x378);
        uint8_t* message_field = at(ml, 0x1C);
        const bool ended = !comment_field && !message_field && tl->field_deletes == deletes + 2;
        int restarts = tl->field_starts;
        show_both(NULL);
        comment_field = at(cm, 0x378);
        message_field = at(ml, 0x1C);
        check(ended && comment_field && comment_field[0x694] == 0 && message_field && holder() == message_field
                && cm[0x14] == 2 && tl->field_starts == restarts + 2 && tl->bad_fields == bad,
            "comment and msgline: each hide ends the field holding the line; shown in one pass with neither the"
            " game's active menu, comment's field started on line 2, the one it edited, then msgline's, last in"
            " inventory order, which holds the line, comment's let go");
        game_close("menu    comment ");
        game_close("menu    msgline ");
        hide_both(1);
        restarts = tl->field_starts;
        show_both(live_menu(row_named("comment")));
        comment_field = at(cm, 0x378);
        const int focused_draws = tl->msgline_draws;
        wait_frames(2);
        check(comment_field && holder() == comment_field && !at(ml, 0x1C) && cm[0x14] == 1
                && tl->field_starts == restarts + 2 && tl->msgline_draws > focused_draws && tl->bad_fields == bad,
            "comment, the game's active menu, shown with msgline in one pass: msgline's field started first, then"
            " ended keeping its text, then comment's, on line 1, which holds the line; msgline's draw finds no"
            " field");
        game_close("menu    comment ");
        game_close("menu    msgline ");
        InterlockedExchange(g_fake->draw_walk, 0);
        check(tl->bad_fields == bad, tl->bad_fields == bad ? "no NULL or deleted field read, bound or deleted"
            : tl->bad_field);
        phase(L,
            "for _, x in ipairs({h, h2}) do x:poll() end\n"
            "return finish()\n");
    }

    // -- reply calls, resize and info, queued from Lua and carried out by the
    //    stand-in's own routines on the game thread
    FakeLog* fl = g_fake->log;
    uint8_t* query_menu = static_cast<uint8_t*>(open_on_game_thread("menu    query"));
    uint8_t* party = static_cast<uint8_t*>(open_on_game_thread("menu    partywin"));
    uint8_t* pmode = static_cast<uint8_t*>(open_on_game_thread("menu    playermo"));
    static int text_context;
    static int32_t foreign_record[2] = {230, 0};
    static uint8_t link5_rows[4 * 0x54];
    static char link5_names[4 * 0x17 + 0x20];
    uint8_t* qctl = controller_named("query");
    uint8_t* pctl = controller_named("passinpu");
    uint8_t* dctl = controller_named("delivery");
    uint8_t* p1ctl = controller_named("post1");
    uint8_t* lctl = controller_named("link5");
    uint8_t* actl = controller_named("arealist");
    {
        *g_fake->query_cancel_allowed = 1;
        put16(qctl, 0x548, 0);
        *g_fake->query_waits = 1;
        put32(pctl, 0x1C, reinterpret_cast<uint32_t>(g_fake->text_callback));
        put32(pctl, 0x20, reinterpret_cast<uint32_t>(&text_context));
        put32(pctl, 0x18, 16);
        g_fake->party_pending[0] = 1;
        g_fake->party_pending[1] = 1;
        memcpy(controller_named("prtyjoin") + 0x14, "Zaldon\0\0\0\0\0\0\0\0\0\0", 16);
        *g_fake->party_queues = 1;
        put16(dctl, 0x1B4, 5);
        *g_fake->post_queues = 1;
        put32(p1ctl, 0x14, 3);
        *g_fake->link5_latch = 1;
        put32(lctl, 0x68, 1);
        put32(lctl, 0x6C, reinterpret_cast<uint32_t>(g_fake->link5_callback));
        // link5's list: the header, slots 3 and 15, and a row whose record is
        // not in the concierge cache
        put32(link5_rows + 1 * 0x54, 0x48, reinterpret_cast<uint32_t>(g_fake->link5_cache + 3 * 0x20));
        put32(link5_rows + 2 * 0x54, 0x48, reinterpret_cast<uint32_t>(g_fake->link5_cache + 15 * 0x20));
        put32(link5_rows + 3 * 0x54, 0x48, reinterpret_cast<uint32_t>(foreign_record));
        strcpy(link5_names + 1 * 0x17 + 3, "Shell One");
        strcpy(link5_names + 2 * 0x17 + 3, "Twentycharactername!");
        put32(lctl, 0x50, 4);
        put32(lctl, 0x54, reinterpret_cast<uint32_t>(link5_rows));
        put32(lctl, 0x58, reinterpret_cast<uint32_t>(link5_names));
        put16(pmode, 0x58, 2);
        put16(pmode, 0x4C, 2);
        put16(party, 0x58, 3);
        // arealist's latch set with no window open in mode 0 (the search):
        // no NPC event is waiting on it
        actl[0x6D] = 1;
        give_elements(party);
        give_typed_elements(pmode);
        char party_row[8];
        snprintf(party_row, sizeof(party_row), "%d", row_named("partywin"));
        set_global(L, "PARTYROW", party_row);
        put32(g_fake->mcb, 0x54, reinterpret_cast<uint32_t>(party));
        wait_frames(2);
        memset(fl, 0, sizeof(*fl));
    }
    phase(L,
        "local function seen(handle)\n"
        "  local out = {}\n"
        "  for _, e in ipairs((handle:poll())) do\n"
        "    if e.event == 'pending' then out[#out + 1] = e.what .. '=' .. tostring(e.pending)"
        "      .. (e.name == nil and '' or ' named') end\n"
        "  end\n"
        "  return table.concat(out, ' ')\n"
        "end\n"
        "local pl = h:groups().party_list\n"
        "check(pl.waiting == nil and pl.origin and pl.origin.x == 100 + PARTYROW and pl.origin.y == 50,"
        "  'reset(partywin) dropped the move waiting for it: partywin opened where the game put it')\n"
        "local a, b = seen(h), seen(h2)\n"
        "check(a == 'invite=true post=true' and b == a, 'pending events: the invite and the post-box session appearing,'"
        "  .. ' to every handle, each {event, what, pending} with no name: ' .. a)\n"
        "local c = h:options('query')\n"
        "local o = c and c.options or {}\n"
        "check(c and c.title.text == 'Will you lend a hand?' and #o == 3 and o[1].text == 'Yes, gladly.' and o[1].value == 1"
        "  and o[2].text == 'Not today.' and o[2].value == 4 and o[3].value == 5 and c.cancellable == true"
        "  and c.title_segments == nil and c.title_undecoded == nil,"
        "  'options(query): the parsed title, as {text, segments, undecoded}, and the options in list order with their'"
        "  .. ' values 1, 4 and 5; the tombstoned one (value 3) left out; cancellable')\n"
        "check(type(c.id) == 'number' and c.id >= 1 and h:options('query').id == c.id,"
        "  'options(query): the id of the open it read, the same until the game opens query again')\n"
        "pj = h:options('prtyjoin')\n"
        "check(pj and pj.name == 'prtyjoin' and pj.inviter == 'Zaldon' and pj.alliance == false and type(pj.id) == 'number'"
        "  and pj.id >= 1, 'options(prtyjoin): the pending invite, {name = prtyjoin, inviter, alliance, id}')\n"
        "local named = {}\n"
        "for _, w in ipairs({'query', 'link5', 'passinpu', 'prtyjoin'}) do\n"
        "  local o = h:options(w)\n"
        "  if o and o.name == w and type(o.id) == 'number' then named[#named + 1] = w end\n"
        "end\n"
        "check(#named == 4, 'options(): every prompt up names itself, with its id: ' .. table.concat(named, ' '))\n"
        "local r, why = h:answer('query', 3)\n"
        "check(r == nil and why:find('no option in the list has the value 3', 1, true),"
        "  'answer(query, 3), a value the game has tombstoned, is refused: ' .. tostring(why))\n"
        "r, why = h:answer('query', 2)\n"
        "check(r == nil and why:find('value 2', 1, true), 'answer(query, 2), a hidden value no option has, is refused')\n"
        "check(h:cancel('query') == true, 'cancel(query) queues while query is open and the event allows it')\n"
        "r, why = h:answer('query', 1)\n"
        "check(r == nil and why == 'a reply to query is already queued',"
        "  'a second reply to query while the first is queued is refused: ' .. tostring(why))\n"
        "check(h:answer('passinpu', 'hello') == true, 'answer(passinpu, text) queues with an entry pending')\n"
        "r, why = h:answer('passinpu', 'seventeen bytes!!')\n"
        "check(r == nil and why:find('at most 16 bytes', 1, true), 'a text of more than 16 bytes is refused')\n"
        "r, why = h2:answer('passinpu', 'again')\n"
        "check(r == nil and why == 'a reply to passinpu is already queued',"
        "  'a second reply to passinpu, from another handle, is refused while the first is queued')\n"
        "local pi = h:options('passinpu')\n"
        "check(pi and pi.max_length == 16, 'options(passinpu): max_length ' .. tostring(pi and pi.max_length))\n"
        "local pend = h:pending()\n"
        "check(pend.invite and pend.invite.name == 'prtyjoin' and pend.invite.inviter == 'Zaldon'"
        "  and pend.invite.alliance == false and pend.invite.id == pj.id and pend.post and pend.post.name == 'delivery'"
        "  and pend.post.box == 'delivery' and type(pend.post.id) == 'number' and pend.post.id >= 1"
        "  and pend.post.state == nil and pend.post.detail.state == 5,"
        "  'pending(): the party invite from Zaldon and the outgoing post-box session, each naming the prompt a reply'"
        "  .. ' takes and its session id, the state 5 under detail')\n"
        "local stale, stale_why = h:answer('prtyjoin', true, pj.id + 1)\n"
        "local stale_post, stale_post_why = h:cancel('delivery', pend.post.id - 1)\n"
        "check(stale == nil and stale_why == 'that prompt is gone' and stale_post == nil"
        "  and stale_post_why == 'that prompt is gone',"
        "  'a reply naming another invite or post-box session than the one up is refused: that prompt is gone')\n"
        "check(h:answer('prtyjoin', true, pj.id) == true, 'answer(prtyjoin, true, id) queues with that invite pending')\n"
        "check(h:cancel('delivery', pend.post.id) == true and h:cancel('post1', pend.post.id) == true,"
        "  'cancel ends both post-box sessions, naming the session')\n"
        "local l = h:options('link5')\n"
        "local slots = l and l.slots or {}\n"
        "check(#slots == 2 and slots[1].slot == 3 and slots[1].name == 'Shell One' and slots[2].slot == 15"
        "  and slots[2].name == 'Twentycharactername!',"
        "  'options(link5): slots 3 and 15 by their concierge records, with their names; the header and a row whose'"
        "  .. ' record is no cache entry left out')\n"
        "r, why = h:answer('link5', 16)\n"
        "check(r == nil and why:find('0..15', 1, true), 'answer(link5, 16), past the concierge cache, is refused: '"
        "  .. tostring(why))\n"
        "r, why = h:answer('link5', 4)\n"
        "check(r == nil and why:find('slot 4 is not in the list', 1, true), 'answer(link5, 4), a slot no row holds, is refused')\n"
        "check(h:cancel('link5') == true,"
        "  'cancel(link5) queues with a linkshell choice pending (latch 1, mode 1, the event callback)')\n"
        "local z, zwhy = h:options('arealist')\n"
        "local ra, rwhy = h:answer('arealist', 231)\n"
        "local ca, cwhy = h:cancel('arealist')\n"
        "check(z == nil and zwhy == 'no area choice is pending' and ra == nil"
        "  and rwhy == \"the area list is not asking anything; the player's keys drive it\" and ca == nil"
        "  and cwhy == rwhy, 'arealist with its latch set in mode 0 and no window open: options refused, and answer'"
        "  .. ' and cancel, no NPC event asking: ' .. tostring(zwhy) .. ' / ' .. tostring(rwhy) .. ' / '"
        "  .. tostring(cwhy))\n"
        "r, why = h:answer('arealist', 1.5)\n"
        "check(r == nil and why:find('whole number', 1, true), 'answer(arealist) of a fractional id is refused: '"
        "  .. tostring(why))\n"
        "check(h:resize('partywin', 3) == true and h:resize('playermo', 4) == true"
        "  and h:resize('equip', 300, 200) == true, 'resize by rows and by width and height queue')\n"
        "r, why = h:resize('partywin', 7)\n"
        "check(r == nil and why:find('1..6', 1, true), 'resize outside the family is refused: ' .. tostring(why))\n"
        "r, why = h:resize('equip', 3)\n"
        "check(r == nil and why:find('no template family', 1, true), 'resize by rows of a window with no family is refused')\n"
        "check(h:resize('itemxinf', 3) == true, 'resize of a closed window is accepted, for its next open')\n"
        "r, why = h:resize('partywin', 1.5)\n"
        "check(r == nil and why:find('usage', 1, true), 'a fractional row count is refused')\n"
        "r, why = h:resize('equip', 0, 10)\n"
        "check(r == nil and why:find('1..32767', 1, true), 'a zero width is refused')\n"
        "local p = h:info('partywin')\n"
        "check(p.focused == true and p.resize and p.resize.min_rows == 1 and p.resize.max_rows == 6"
        "  and p.resize.holds == 'trigger' and p.sizes == nil and h:focused() == 'partywin',"
        "  'info(partywin): focused, resize by rows 1..6 held until its trigger; focused() names it')\n"
        "check(p.elements_truncated == false and p.detail.elements_truncated == nil,"
        "  'info(): elements_truncated beside elements')\n"
        "local holds = {}\n"
        "for _, n in ipairs({'playermo', 'mp_pmode', 'partywin', 'iteminfo', 'itemxinf', 'equip', 'logwindo'}) do\n"
        "  local i = h:info(n)\n"
        "  holds[#holds + 1] = n .. '=' .. i.resize.holds"
        "    .. (i.resize.min_rows and (':' .. i.resize.min_rows .. '..' .. i.resize.max_rows) or '')\n"
        "end\n"
        "check(table.concat(holds, ' ') == 'playermo=trigger:1..10 mp_pmode=trigger:1..8 partywin=trigger:1..6'"
        "  .. ' iteminfo=frame:3..12 itemxinf=frame:3..12 equip=reopen logwindo=reopen',"
        "  'sizes.holds per family, open or closed: ' .. table.concat(holds, ' '))\n"
        "local e = p.elements\n"
        "check(#e == 3 and e[1].type == 'frame' and e[1].text == '' and e[2].type == 'item' and e[2].text == 'Party'"
        "  and e[3].text == 'alt' and e[3].x == 20 and e[3].y == 40,"
        "  'info elements: a frame has no text, an item its +0x40 text, else its +0x44 text')\n"
        "local t = h:info('playermo').elements\n"
        "check(#t == 4 and t[1].type == 'frame' and t[1].x == 5 and t[1].y == 6 and t[1].w == 300 and t[1].h == 80"
        "  and t[1].right == 305 and t[1].bottom == 86"
        "  and t[2].type == 'item' and t[2].x == 11 and t[2].y == 12 and t[2].w == 90 and t[2].h == 16"
        "  and t[3].type == 'cursor' and t[3].x == 33 and t[3].y == 44 and t[3].w == nil and t[3].right == nil"
        "  and t[4].type == 'other' and t[4].x == nil and t[4].y == nil and t[4].w == nil,"
        "  'info elements, each type named and its own box: a frame at +0x1A sized +0x22, an item at +0x2A sized'"
        "  .. ' +0x2E (not its float scales), the cursor at +0x20 with no size, an unknown type other with none')\n"
        "local q = h:info('equip')\n"
        "check(q.resize.min_rows == nil and q.resize.holds == 'reopen' and q.focused == false,"
        "  'info(equip): resize {holds} alone, no rows; not focused')\n"
        "local function runs(segments)\n"
        "  local out = {}\n"
        "  for i, r in ipairs(segments or {}) do out[i] = r.text .. '/' .. r.escape .. ':' .. r.color end\n"
        "  return table.concat(out, '|')\n"
        "end\n"
        "c = c or {}\n"
        "check(runs(c.title.segments) == 'Will you lend a hand?/30:1' and c.title.undecoded == 0,"
        "  'options(query): the title decoded by its glyph count, its spaces (glyph 0) kept: ' .. runs(c.title.segments))\n"
        "check(o[1] and runs(o[1].segments) == 'Yes, /30:1|gladly/30:2|./30:1' and o[1].undecoded == 0,"
        "  'options(query): a green run (1E 02) between default ones (1E 01), the color codes out of the text: '"
        "  .. runs(o[1] and o[1].segments))\n"
        "check(o[2] and runs(o[2].segments) == 'Not today./30:1' and o[2].undecoded == 0,"
        "  'options(query): a code the game does not draw is dropped: ' .. runs(o[2] and o[2].segments))\n"
        "local brackets = string.char(0xEF, 0x27) .. 'Cancel' .. string.char(0xEF, 0x28)\n"
        "check(o[3] and o[3].text == brackets and o[3].undecoded == 0 and runs(o[3].segments) == brackets .. '/30:1',"
        "  'options(query): the auto-translate brackets come back as EF 27 and EF 28, their trail bytes below 0x40'"
        "  .. ' tried for lead EF, over the defined EE E4 and EE E5 that yield the same glyphs')\n"
        "return finish()\n");
    wait_frames(3);
    {
        // query's close is the stand-in's event wait, a frame after the word
        // turned: where it lands among the replies depends on the frames
        char replies[sizeof(fl->order)];
        size_t k = 0;
        for (const char* c = fl->order; *c; ++c) {
            if (*c != 'K') {
                replies[k++] = *c;
            }
        }
        replies[k] = '\0';
        check(strcmp(replies, "CRPBBYLJSMFUSMUDF") == 0 && count_of(fl->order, 'K') == 1,
            "the drain ran the replies and resizes in queue order through the stand-in's routines; one close by"
            " name, query's, by the stand-in's event wait");
    }
    check(fl->query_event == 0 && static_cast<uint16_t>(get16(qctl, 0x548)) == 0xFF && query_menu
            && strcmp(fl->closes, "query   |") == 0 && live_menu(row_named("query")) == NULL
            && *g_fake->query_waits == 0,
        "cancel query: 0xFF written, the game allowing a cancel, and nothing closed by the engine; the event's wait"
        " closed query by name a tick later; its input never called");
    query_menu = static_cast<uint8_t*>(open_on_game_thread("menu    query"));
    static const char hello[32] = {'h', 'e', 'l', 'l', 'o'};
    check(fl->text_context == &text_context && !fl->text_null && get32(pctl, 0x1C) == 0,
        "answer passinpu: the callback got its context and a text, and the reset cleared the entry");
    check(fl->link5_calls == 1 && fl->link5_last_null && *g_fake->link5_latch == 0,
        "cancel link5: the clear routine called the event's callback with no choice, which cleared the latch");
    check(*g_fake->party_pending == 0 && fl->party_value == 1, "answer prtyjoin true: 0x074 accept sent, the invite cleared");
    check(fl->post_command == 0x0F && get16(dctl, 0x1B4) == 0x19 && get32(p1ctl, 0x14) == 0x12,
        "cancel delivery and post1: request 15 for both boxes; delivery waits in 0x19, post1 in 0x12");
    check(strcmp(fl->swaps, "ptw3    |actionm4|") == 0 && get16(party, 0x52) == 100 + row_named("partywin")
            && get16(party, 0x3A) == get16(party, 0x52),
        "resize partywin 3 swapped to ptw3 and SetPosition put the frame back on its origin");
    check(get16(party, 0x40) == get16(party, 0x48) && get16(party, 0x58) == 3,
        "and SetFrameRect hung the frame the swap left on its default bottom (a bottom-anchored class), its item"
        " count left alone");
    check(fl->dock_resets == 1 && fl->dock_group == 0 && fl->dock_menu == pmode,
        "resize playermo 4: DockReset(0, playermo), the only re-dock");
    check(get16(pmode, 0x58) == 4 && fl->cursor_calls == 2 && fl->cursor_menu == pmode && fl->cursor_items[0] == 4
            && fl->cursor_row[0] == 2 && fl->cursor_warp[0] == 0 && fl->cursor_items[1] == 4 && fl->cursor_row[1] == 2
            && fl->cursor_warp[1] == 0,
        "resize playermo 4: +0x58 = 4 before the first SetCursor(+0x4C, 0), then the swap, then SetCursor(+0x4C, 0)"
        " again before the re-dock, as its owner does");
    check(fl->frame_menu == equip && fl->frame[0] == 10 && fl->frame[1] == 20 && fl->frame[2] == 300
            && fl->frame[3] == 200 && fl->frame[4] == 1 && fl->frame[5] == 0 && fl->frame[6] == 0,
        "resize equip 300x200: SetFrameRect(10, 20, 300, 200, 1, 0, 0) at its origin");
    // -- two-byte characters through the table: the home point list's title
    //    as measured live, a full-width space (81 40) after the question
    //    mark, the full-width parentheses (81 69, 81 6A) in a green run, an
    //    auto-translate bracket (EF 27), and a code the table lacks
    {
        uint8_t saved[2 + 2 * 128];
        memcpy(saved, qctl + 0x36, sizeof(saved));
        int16_t title[64];
        int n = 0;
        for (const char* c = "Teleport where?"; *c; ++c) {
            title[n++] = static_cast<int16_t>(*c - 0x20);
        }
        title[n++] = 0x60;
        title[n++] = -0x102;
        title[n++] = 0x89;
        for (const char* c = "Windurst Waters"; *c; ++c) {
            title[n++] = static_cast<int16_t>(*c - 0x20);
        }
        title[n++] = 0x8A;
        title[n++] = -0x101;
        title[n++] = 0x211D;
        title[n++] = 0x3000;
        qctl[0x36] = static_cast<uint8_t>(n);
        memcpy(qctl + 0x38, title, 2 * n);
        phase(L,
            "local function hex(text) return (tostring(text):gsub('[\\128-\\255]', function(b)"
            " return ('<%02X>'):format(b:byte()) end)) end\n"
            "local function runs(segments)\n"
            "  local out = {}\n"
            "  for i, r in ipairs(segments or {}) do out[i] = r.text .. '/' .. r.escape .. ':' .. r.color end\n"
            "  return table.concat(out, '|')\n"
            "end\n"
            "local space, open, close = string.char(0x81, 0x40), string.char(0x81, 0x69), string.char(0x81, 0x6A)\n"
            "local bracket = string.char(0xEF, 0x27)\n"
            "local c = (h:options('query') or {}).title or {}\n"
            "check(c.text == 'Teleport where?' .. space .. open .. 'Windurst Waters' .. close .. bracket .. '?'"
            "  and c.undecoded == 1,"
            "  'options(query): 81 40, the full-width parentheses 81 69 and 81 6A and the bracket EF 27 come back as'"
            "  .. ' their Shift-JIS bytes, a code the table lacks as a counted ?: ' .. hex(c.text) .. ', undecoded '"
            "  .. tostring(c.undecoded))\n"
            "check(runs(c.segments) == 'Teleport where?' .. space .. '/30:1|' .. open .. 'Windurst Waters' .. close"
            "  .. '/30:2|' .. bracket .. '?/30:1', 'options(query): a run holds its two-byte characters whole: '"
            "  .. hex(runs(c.segments)))\n"
            "return finish()\n");
        memcpy(qctl + 0x36, saved, sizeof(saved));
    }
    // -- the prompt's id: options() names the open it read, and an answer or
    //    cancel naming another is refused, up front and on the game thread
    {
        phase(L,
            "id1 = h:options('query').id\n"
            "local r, why = h:answer('query', 1, id1 + 1)\n"
            "check(r == nil and why == 'that prompt is gone',"
            "  'answer(query, 1, id) naming an open the window has not had is refused: ' .. tostring(why))\n"
            "return finish()\n");
        open_on_game_thread("menu    query");
        phase(L,
            "local c = h:options('query')\n"
            "local r, why = h:cancel('query', id1)\n"
            "local r2, why2 = h:answer('query', 1, id1)\n"
            "check(c.id == id1 + 1 and r == nil and why == 'that prompt is gone' and r2 == nil"
            "  and why2 == 'that prompt is gone',"
            "  'the game opens query again: options() gives the next id, and cancel and answer naming the one before'"
            "  .. ' are refused: that prompt is gone')\n"
            "id2 = c.id\n"
            "h2:poll()\n"
            "return finish()\n");
        const int16_t word = get16(qctl, 0x548);
        pause_game();
        phase(L,
            "check(h2:answer('query', 1, id2) == true, 'beta answers query naming the open that is up')\n"
            "return finish()\n");
        open_here("menu    query");
        resume_game();
        wait_frames(3);
        phase(L,
            "local out = {}\n"
            "for _, e in ipairs((h2:poll())) do\n"
            "  if e.event == 'error' then out[#out + 1] = e.verb .. ' ' .. e.name .. ': ' .. e.reason end\n"
            "end\n"
            "check(#out == 1 and out[1] == 'answer query: that prompt is gone' and h:options('query').id == id2 + 1,"
            "  'the game opens query again before the game thread carries the answer out: refused there, an error'"
            "  .. ' event to beta: ' .. table.concat(out, ' | '))\n"
            "return finish()\n");
        check(get16(qctl, 0x548) == word, "and query's result word is left as it was");
    }
    {
        memset(fl, 0, sizeof(*fl));
        put32(pctl, 0x1C, reinterpret_cast<uint32_t>(g_fake->text_callback));
        put32(pctl, 0x20, reinterpret_cast<uint32_t>(&text_context));
        put32(pctl, 0x18, 16);
        *g_fake->query_cancel_allowed = 0;
    }
    phase(L,
        "local rq, whyq = h:cancel('query')\n"
        "check(rq == nil and whyq:find('allows no cancel', 1, true) and h:options('query').cancellable == false,"
        "  'cancel(query) while the cancel-allowed byte is not 1 is refused up front: ' .. tostring(whyq))\n"
        "local ok1, why1 = h:answer('passinpu', 'hello')\n"
        "check(ok1 == true, 'answer(passinpu) queues: ' .. tostring(why1))\n"
        "local r, why = h:cancel('prtyjoin')\n"
        "check(r == nil and why:find('no party invite', 1, true), 'a reply already given is not given twice')\n"
        "check(h:pending().invite == nil, 'and pending() has no invite')\n"
        "r, why = h:cancel('post2')\n"
        "check(r == nil and why:find('already waiting', 1, true), 'cancel(post2) while the close waits on the server is refused')\n"
        "return finish()\n");
    wait_frames(3);
    check(strcmp(fl->order, "CR") == 0 && fl->text_context == &text_context && memcmp(fl->text, hello, 32) == 0,
        "answer passinpu: the callback got its context and 'hello' zero-padded, then the reset");
    check(strchr(fl->order, 'Q') == NULL && fl->query_event == 0, "the refused cancel(query) never reached query's input");

    // -- answer link5 through the event's callback
    memset(fl, 0, sizeof(*fl));
    *g_fake->link5_latch = 1;
    put32(lctl, 0x6C, reinterpret_cast<uint32_t>(g_fake->link5_callback));
    phase(L,
        "check(h:answer('link5', 3) == true, 'answer(link5, 3) queues with a linkshell choice pending and slot 3 listed')\n"
        "local r, why = h:answer('link5', 1.5)\n"
        "check(r == nil and why:find('whole number', 1, true), 'a fractional slot is refused')\n"
        "return finish()\n");
    wait_frames(3);
    check(strcmp(fl->order, "JLJ") == 0 && fl->link5_calls == 2 && fl->link5_slot == 3
            && fl->link5_entry == g_fake->link5_cache + 3 * 0x20 && fl->link5_last_null,
        "answer link5 3: the callback got slot 3 and its concierge entry, then the clear routine called it with no choice");
    check(*g_fake->link5_latch == 0, "and the event's callback cleared the latch");
    phase(L,
        "local r, why = h:answer('link5', 15)\n"
        "check(r == nil and why:find('no linkshell choice is pending', 1, true),"
        "  'answer(link5) once the callback cleared the latch is refused: ' .. tostring(why))\n"
        "return finish()\n");
    *g_fake->link5_latch = 1;
    put32(lctl, 0x68, 0);
    put32(lctl, 0x6C, 0);
    phase(L,
        "local r, why = h:answer('link5', 15)\n"
        "check(r == nil and why:find('no linkshell choice is pending', 1, true),"
        "  'answer(link5) with the latch at 1 but link5 opened by another list opener (mode 0, no callback) is refused')\n"
        "r, why = h:cancel('link5')\n"
        "check(r == nil and why:find('no linkshell choice is pending', 1, true), 'and so is cancel(link5)')\n"
        "r, why = h:options('link5')\n"
        "check(r == nil and why:find('no linkshell choice is pending', 1, true), 'and options(link5)')\n"
        "return finish()\n");
    put32(lctl, 0x68, 1);
    put32(lctl, 0x6C, reinterpret_cast<uint32_t>(g_fake->text_callback));
    phase(L,
        "local r, why = h:answer('link5', 15)\n"
        "check(r == nil and why:find('no linkshell choice is pending', 1, true),"
        "  'answer(link5) with the latch 1 and mode 1 but a callback other than the event one is refused')\n"
        "return finish()\n");
    put32(lctl, 0x6C, 0);
    phase(L,
        "local r, why = h:answer('link5', 15)\n"
        "check(r == nil and why:find('no linkshell choice is pending', 1, true), 'answer(link5) with no callback set is refused')\n"
        "return finish()\n");
    *g_fake->link5_latch = 0;
    put32(lctl, 0x68, 0);

    // -- the error event: beta answers the text entry, which goes away before
    //    the game thread carries the answer out; the refusal reaches beta and
    //    not alpha
    put32(pctl, 0x1C, reinterpret_cast<uint32_t>(g_fake->text_callback));
    put32(pctl, 0x20, reinterpret_cast<uint32_t>(&text_context));
    put32(pctl, 0x18, 16);
    phase(L,
        "h:poll()\n"
        "h2:poll()\n"
        "return finish()\n");
    pause_game();
    phase(L,
        "check(h2:answer('passinpu', 'late') == true, 'beta answers the text entry while it is pending')\n"
        "return finish()\n");
    put32(pctl, 0x1C, 0);
    resume_game();
    wait_frames(3);
    phase(L,
        "local function errors(handle)\n"
        "  local out = {}\n"
        "  for _, e in ipairs((handle:poll())) do\n"
        "    if e.event == 'error' then out[#out + 1] = e.verb .. ' ' .. e.name .. ': ' .. e.reason end\n"
        "  end\n"
        "  return out\n"
        "end\n"
        "local b, a = errors(h2), errors(h)\n"
        "check(#b == 1 and b[1] == 'answer passinpu: no text entry is pending' and #a == 0,"
        "  'the game thread refused beta\\'s answer: one error event, {verb, name, reason}, reaches beta and none'"
        "  .. ' alpha: ' .. table.concat(b, ' | '))\n"
        "local d = hu.status().detail\n"
        "check(d.drain_error == 'answer passinpu: no text entry is pending', 'status().detail.drain_error keeps it: '"
        "  .. tostring(d.drain_error))\n"
        "check(h2:answer('passinpu', 'late') == nil, 'and with the entry gone a new answer is refused up front')\n"
        "return finish()\n");

    // -- the incoming box with its window up: cancel('post1') and
    //    cancel('post2') send request 15 alone, as the box's own cancel does,
    //    never its input, and write nothing; refused while the box takes no
    //    input yet. The stand-in's reply marks the session closing and
    //    post1's next tick closes post2 and post1. Engine abi 3's cancel, as a
    //    0.5.0 copy calls it, still refuses
    {
        const uint32_t closing = get32(p1ctl, 0x14);
        const int16_t out_state = get16(dctl, 0x1B4);
        put16(dctl, 0x1B4, 0);
        put32(p1ctl, 0x14, 3);
        uint8_t* box = static_cast<uint8_t*>(open_on_game_thread("menu    post1"));
        uint8_t* box2 = static_cast<uint8_t*>(open_on_game_thread("menu    post2"));
        put16(box, 0x4C, 4);
        put16(box, 0x56, 2);
        memset(fl, 0, sizeof(*fl));
        phase(L,
            "h:poll()\n"
            "local r, why = h:cancel('post1')\n"
            "check(r == nil and why == 'the incoming box takes no input yet',"
            "  'cancel(post1) while the box takes no input yet (+0x56 above 0) is refused: ' .. tostring(why))\n"
            "return finish()\n");
        put16(box, 0x56, 0);
        box[0x5D] = 1;
        phase(L,
            "check(h:cancel('post1') == true, 'cancel(post1) with the incoming box up queues, its busy byte no bar')\n"
            "return finish()\n");
        wait_frames(3);
        check(box && box2 && count_of(fl->order, 'B') == 1 && fl->post_command == 0x0F && fl->box_calls == 0
                && strchr(fl->order, 'Y') == NULL && get32(p1ctl, 0x14) == 3,
            "cancel post1 with the box up: one request 15, the box's own cancel; post1's input never called, no"
            " request-close, the session state left at 3");
        box[0x5D] = 0;
        phase(L,
            "check(h:cancel('post2') == true, 'cancel(post2) with the box up queues')\n"
            "return finish()\n");
        wait_frames(3);
        check(count_of(fl->order, 'B') == 2 && fl->box_calls == 0 && get32(p1ctl, 0x14) == 3,
            "cancel post2 with the box up: the same request 15 alone");
        HuEngineApi api;
        memset(&api, 0, sizeof(api));
        HuEngineHandle old = {-1, 0};
        char why[512] = "";
        const bool read = record_api(&api);
        if (read) {
            api.handle_new("abi3", &old);
        }
        const int32_t refused = read && old.slot >= 0 ? api.cancel(&old, "post1", why, sizeof(why)) : -2;
        check(refused == 0 && strstr(why, "the incoming box is open") != NULL,
            "engine abi 3's cancel(post1) still refuses while the box is up, as 0.5.0 did");
        if (read && old.slot >= 0) {
            api.handle_release(&old);
        }
        // the game's close by instance: the marks and the staged close, the
        // instance freed frames later
        memset(fl, 0, sizeof(*fl));
        *g_fake->close_frames = 2;
        *g_fake->box_ticks = 1;
        *g_fake->post_reply = 1;
        wait_frames(6);
        *g_fake->box_ticks = 0;
        check(strcmp(fl->order, "ZYY") == 0 && get32(p1ctl, 0x14) == 0 && live_menu(row_named("post1")) == NULL
                && live_menu(row_named("post2")) == NULL && fl->box_calls == 0,
            "the stand-in's reply to request 15: post1's request-close marks 0x12, and post1's next tick closes post2"
            " and post1 and ends the session");
        phase(L,
            "local out = {}\n"
            "for _, e in ipairs((h:poll())) do\n"
            "  if e.event == 'closed' then out[#out + 1] = e.name end\n"
            "end\n"
            "table.sort(out)\n"
            "check(table.concat(out, ' ') == 'post1 post2' and h:pending().post == nil,"
            "  'closed{post1} and closed{post2} follow the game\\'s teardown, and no post-box session is pending: '"
            "  .. table.concat(out, ' '))\n"
            "return finish()\n");
        *g_fake->close_frames = 0;
        put32(p1ctl, 0x14, closing);
        put16(dctl, 0x1B4, out_state);
    }

    // -- the incoming box with post1 blocked: the game's open of post1 is
    //    refused and post2 opens alone, so no tick reads the session.
    //    cancel('post1') sends request 15 and post1's request-close marks the
    //    session closing; the stand-in's reply closes post2 and ends it
    {
        const uint32_t in_state = get32(p1ctl, 0x14);
        const int16_t out_state = get16(dctl, 0x1B4);
        put16(dctl, 0x1B4, 0);
        phase(L,
            "check(h:block('post1') == true, 'block(post1) holds')\n"
            "return finish()\n");
        put32(p1ctl, 0x14, 3);
        const bool refused = open_on_game_thread("menu    post1") == NULL;
        uint8_t* box2 = static_cast<uint8_t*>(open_on_game_thread("menu    post2"));
        memset(fl, 0, sizeof(*fl));
        phase(L,
            "h:poll()\n"
            "local p = h:pending()\n"
            "check(p.post and p.post.box == 'post1', 'pending() lists the session with post1 blocked and post2 up')\n"
            "check(h:cancel('post1') == true, 'cancel(post1) with post1 blocked and post2 up queues')\n"
            "return finish()\n");
        wait_frames(3);
        check(refused && box2 && strcmp(fl->order, "BY") == 0 && fl->post_command == 0x0F
                && get32(p1ctl, 0x14) == 0x12 && live_menu(row_named("post2")) == box2,
            "cancel post1 with post1 blocked and post2 up: request 15, then post1's request-close marks 0x12; post2"
            " stays up for the server's reply");
        phase(L,
            "local r, why = h:cancel('post2')\n"
            "check(r == nil and why == 'its close is already waiting for the server',"
            "  'cancel(post2) once the close waits for the server is refused: ' .. tostring(why))\n"
            "return finish()\n");
        memset(fl, 0, sizeof(*fl));
        *g_fake->box_ticks = 1;
        *g_fake->post_reply = 1;
        wait_frames(3);
        *g_fake->box_ticks = 0;
        check(strcmp(fl->order, "ZY") == 0 && get32(p1ctl, 0x14) == 0 && live_menu(row_named("post2")) == NULL,
            "the stand-in's reply to request 15: post1's request-close closes post2 and ends the session");
        phase(L,
            "local out = {}\n"
            "for _, e in ipairs((h:poll())) do\n"
            "  if e.event == 'closed' then out[#out + 1] = e.name end\n"
            "end\n"
            "check(table.concat(out, ' ') == 'post2' and h:pending().post == nil,"
            "  'closed{post2} follows the game\\'s teardown, and no post-box session is pending: '"
            "  .. table.concat(out, ' '))\n"
            "check(h:unblock('post1') == true, 'unblock(post1)')\n"
            "return finish()\n");
        put32(p1ctl, 0x14, in_state);
        put16(dctl, 0x1B4, out_state);
    }

    // -- the outgoing box with its window up: cancel('delivery') writes its
    //    closing row's two words, +0x1B6 = 0x18 then +0x1B4 = 0x15, and sends
    //    nothing; refused while the box is busy or already closing. The
    //    stand-in's tick then takes the staged rows and the box-2 slots back
    //    and sends request 15, and its reply closes the box
    {
        const int16_t out_state = get16(dctl, 0x1B4);
        const uint32_t in_state = get32(p1ctl, 0x14);
        put32(p1ctl, 0x14, 0);
        put16(dctl, 0x1B4, 5);
        uint8_t* dbox = static_cast<uint8_t*>(open_on_game_thread("menu    delivery"));
        dbox[0x5D] = 1;
        memset(fl, 0, sizeof(*fl));
        phase(L,
            "h:poll()\n"
            "local r, why = h:cancel('delivery')\n"
            "check(r == nil and why == 'the outgoing box is busy', 'cancel(delivery) while the box is busy is refused: '"
            "  .. tostring(why))\n"
            "return finish()\n");
        dbox[0x5D] = 0;
        static const int closing_states[] = {0x15, 0x16, 0x17, 0x19};
        for (size_t i = 0; i < sizeof(closing_states) / sizeof(closing_states[0]); ++i) {
            put16(dctl, 0x1B4, closing_states[i]);
            char state[8];
            snprintf(state, sizeof(state), "%d", closing_states[i]);
            set_global(L, "STATE", state);
            phase(L,
                "local r, why = h:cancel('delivery')\n"
                "check(r == nil and why == 'the box is already closing', 'cancel(delivery) in state ' .. STATE"
                "  .. ' is refused: ' .. tostring(why))\n"
                "return finish()\n");
        }
        put16(dctl, 0x1B4, 5);
        put16(dctl, 0x1B6, 0x77);
        phase(L,
            "check(h:cancel('delivery') == true, 'cancel(delivery) with the outgoing box up queues')\n"
            "return finish()\n");
        wait_frames(3);
        check(dbox && fl->count == 0 && get16(dctl, 0x1B6) == 0x18 && get16(dctl, 0x1B4) == 0x15
                && live_menu(row_named("delivery")) == dbox,
            "cancel delivery with the box up: +0x1B6 = 0x18, +0x1B4 = 0x15, the closing row's two words; nothing"
            " called, nothing sent, the box still up");
        *g_fake->box_ticks = 1;
        wait_frames(5);
        check(strcmp(fl->order, "uvB") == 0 && fl->post_command == 0x0F && get16(dctl, 0x1B4) == 0x19
                && live_menu(row_named("delivery")) == dbox,
            "the stand-in's tick: the staged rows back (0x15), the box-2 slots back (0x16), then request 15 from"
            " 0x18 and the wait for the reply in 0x19");
        *g_fake->close_frames = 2;
        *g_fake->post_reply = 1;
        wait_frames(6);
        *g_fake->box_ticks = 0;
        *g_fake->close_frames = 0;
        check(strcmp(fl->order, "uvBZ") == 0 && get16(dctl, 0x1B4) == 0 && live_menu(row_named("delivery")) == NULL,
            "the stand-in's reply: delivery's request-close in 0x19 closes the box and ends the session");
        phase(L,
            "local out = {}\n"
            "for _, e in ipairs((h:poll())) do\n"
            "  if e.event == 'closed' then out[#out + 1] = e.name end\n"
            "end\n"
            "check(table.concat(out, ' ') == 'delivery', 'closed{delivery} follows: ' .. table.concat(out, ' '))\n"
            "return finish()\n");
        put16(dctl, 0x1B4, out_state);
        put32(p1ctl, 0x14, in_state);
    }

    // -- close by instance as the game runs it (0.6.1): the marks at once, the
    //    instance freed frames later. From the closed event on, every reader
    //    and the prompt verbs say closed; a covered window, dormant with the
    //    marks clear, reads open, and once the game closes it by instance the
    //    next frame posts closed for it. Measured live 2026-10-04: opened()
    //    still listed query 18 s after closed{query}.
    {
        typedef int (__fastcall* CloseFn)(void* mcb, void* edx, const char* key);
        typedef void (__fastcall* StagedFn)(void* mcb, void* edx, void* menu, int, int, int, int);
        CloseFn close_by_name;
        StagedFn staged_close;
        memcpy(&close_by_name, &g_fake->close, sizeof(close_by_name));
        memcpy(&staged_close, &g_fake->staged_close, sizeof(staged_close));
        const int row_query = row_named("query");
        const int row_scsi = row_named("scsibori");
        const uint32_t active = get32(g_fake->mcb, 0x54);
        *g_fake->close_frames = 50;
        uint8_t* scsi = static_cast<uint8_t*>(open_on_game_thread("menu    scsibori"));
        wait_frames(2);
        pause_game();
        phase(L,
            "h:poll()\n"
            "function seen_events()\n"
            "  local out = {}\n"
            "  for _, e in ipairs((h:poll())) do out[#out + 1] = e.event .. ':' .. tostring(e.name) end\n"
            "  return table.concat(out, ' ')\n"
            "end\n"
            "function opened_set()\n"
            "  local set = {}\n"
            "  for _, n in ipairs(h:opened()) do set[n] = true end\n"
            "  return set\n"
            "end\n"
            "return finish()\n");

        // the update loop sending scsibori dormant under another window: the
        // staged close with the closing mark clear
        staged_close(g_fake->mcb, NULL, scsi, 0, 0, 0, 0);
        put32(scsi, 0x10, 0x0E);
        phase(L,
            "local ev = seen_events()\n"
            "local i = h:info('scsibori')\n"
            "check(ev == 'covered:scsibori' and i.open and i.covered and i.rect ~= nil and h:list().scsibori.open"
            "  and opened_set().scsibori,"
            "  'a covered window, dormant with the marks clear, reads open: info open and covered, with its rect;'"
            "  .. ' list() and opened() have it: ' .. ev)\n"
            "return finish()\n");

        // the game closes it by instance while it is dormant: the closing and
        // pending-destroy marks, no staged close; the next frame's drain
        // notices it
        close_by_name(g_fake->mcb, NULL, "menu    scsibori");
        const bool scsi_marked = scsi[0x6E] == 1 && scsi[0x70] == 1 && live_menu(row_scsi) == scsi;
        resume_game();
        wait_frames(2);
        pause_game();
        phase(L,
            "local ev = seen_events()\n"
            "local i = h:info('scsibori')\n"
            "check(ev == 'closed:scsibori' and not i.open and not i.covered and i.rect == nil"
            "  and not h:list().scsibori.open and not opened_set().scsibori,"
            "  'the game closes the covered scsibori by instance (no staged close): the next frame posts'"
            "  .. ' closed{scsibori}, and info, list() and opened() say closed: ' .. ev)\n"
            "return finish()\n");
        check(scsi_marked && live_menu(row_scsi) == scsi,
            "scsibori, closed while dormant, carries +0x6E and +0x70, and its instance is still bound after the event");

        // query, open and focused, closed by instance as walking away from
        // the NPC closes it: its staged close posts closed{query}
        put32(g_fake->mcb, 0x54, reinterpret_cast<uint32_t>(query_menu));
        phase(L,
            "local i = h:info('query')\n"
            "check(i.open and i.focused and h:focused() == 'query' and opened_set().query and h:options('query') ~= nil,"
            "  'query is open and focused before the game closes it')\n"
            "return finish()\n");
        close_by_name(g_fake->mcb, NULL, "menu    query");
        const bool query_marked = query_menu[0x6E] == 1 && query_menu[0x70] == 0;
        phase(L,
            "local ev = seen_events()\n"
            "local i = h:info('query')\n"
            "local o, owhy = h:options('query')\n"
            "local a, awhy = h:answer('query', 1)\n"
            "local c, cwhy = h:cancel('query')\n"
            "check(ev == 'closed:query', 'the game closes query by instance: its staged close posts closed{query}: ' .. ev)\n"
            "check(not opened_set().query and not h:list().query.open and not i.open and i.rect == nil"
            "  and i.cursor == nil and i.elements == nil and i.focused == false and i.covered == false,"
            "  'from that event, with the instance alive: opened() leaves query out, list() and info() say open ='"
            "  .. ' false, and info reports no rect, cursor or elements')\n"
            "check(o == nil and owhy == 'query is not open' and a == nil and awhy == 'query is not open' and c == nil"
            "  and cwhy == 'query is not open', 'options, answer and cancel of query refuse: query is not open')\n"
            "check(h:focused() == false, 'focused(): false while the active menu is the closing query')\n"
            "return finish()\n");
        check(query_marked && live_menu(row_query) == query_menu,
            "query carries +0x6E, not +0x70, and its instance is still bound through those reads");

        resume_game();
        wait_frames(60);
        check(!live_menu(row_query) && !live_menu(row_scsi), "the update frees both instances, 50 frames after their close");
        phase(L,
            "local ev = seen_events()\n"
            "local set = opened_set()\n"
            "check(ev == '' and not set.query and not set.scsibori and not h:info('query').open"
            "  and not h:list().scsibori.open and h:options('query') == nil and h:focused() == false,"
            "  'once the instances are freed nothing changes: no event, both still closed: ' .. ev)\n"
            "return finish()\n");

        *g_fake->close_frames = 0;
        put32(g_fake->mcb, 0x54, active);
        query_menu = static_cast<uint8_t*>(open_on_game_thread("menu    query"));
        wait_frames(2);
        phase(L,
            "local ev = seen_events()\n"
            "check(ev == 'opened:query' and h:info('query').open and opened_set().query,"
            "  'the game opens query again: a new instance, open: ' .. ev)\n"
            "return finish()\n");
    }

    phase(L,
        "check(h:release() == true, 'alpha releases')\n"
        "return finish()\n");
    wait_frames(3);
    check(hidden_in_place(kRowLog, log), "logwindo stays hidden while beta holds it");
    check(memcmp(registry_row(kRowMenuwind), g_fake->pristine + kRowMenuwind * 0x2C, 0x2C) == 0,
        "menuwind's registry row as the table has it, its block held and dropped");
    check(origin(log, 0) == 100 + kRowLog && origin(log, 1) == 50, "alpha's group move is reset with alpha");
    check(origin(equip, 0) == 10 && origin(equip, 1) == 20, "beta's move of equip survives alpha's release");

    phase(L,
        "local ok, why, kind = hu.shutdown()\n"
        "check(ok == false and kind == 'handles', 'shutdown refuses while beta is open')\n"
        "check(not h2:info('menuwind').blocked and not h2:info('menuwind').hidden,"
        "  'alpha\\'s block is gone with alpha')\n"
        "h2:release()\n"
        "ok, why, kind = hu.shutdown()\n"
        "check(ok == true, 'shutdown after the last release: ' .. tostring(why))\n"
        "return finish()\n");
    settle(L);
    phase(L,
        "local s = hu.status()\n"
        "check(not s.ok and s.state == 'idle' and not s.detail.pinned, 'idle and unpinned after shutdown')\n"
        "local r, why2 = h2:hide('buff')\n"
        "check(r == nil and why2:find('shut down', 1, true), 'verbs after shutdown say so')\n"
        "return finish()\n");
    check(memcmp(g_fake->registry, g_fake->pristine, 371 * 0x2C) == 0,
        "after shutdown the registry is byte-identical to the expected table");
    check(log[0x60] == 1 && walk_of(log) == 'd' && origin(equip, 0) == 100 + kRowEquip && origin(equip, 1) == 50,
        "logwindo is drawn again and equip is back at its default rect");
    lua.close(L);
    check(engine_unmaps(), "after shutdown, closing Lua unmaps the engine");
    {
        const LONG before = *g_fake->frames;
        wait_frames(3);
        check(*g_fake->frames >= before + 3, "the patched routines keep running as passthroughs");
    }
    check(static_cast<uint8_t*>(g_fake->open)[0] == 0xE9 && static_cast<uint8_t*>(g_fake->ui_update)[0] == 0xE9,
        "the daemon's jumps stay in place");

    // -- a fresh engine image over routines the daemon already patched
    L = fresh(globals);
    phase(L,
        "hu = load_engine()\n"
        "h = hu.new('gamma')\n"
        "local s = hu.status()\n"
        "check(s.ok, 'a fresh engine resolves the already-patched routines: ' .. tostring(s.detail.error))\n"
        "local f = s.detail.functions\n"
        "check(f.open_by_name == OPEN and f.ui_update == UPDATE"
        "  and f.staged_close == STAGED and f.show_path == SHOW and f.compass_draw == COMPASS"
        "  and f.macro_gate == MACROGATE and f.ability_open == ABILITYOPEN and f.template_swap == F_template_swap"
        "  and f.menu_draw == MENUDRAW and f.sprite_draw == SPRITEDRAW and f.primitive_draw == PRIMITIVEDRAW"
        "  and f.quad_draw == QUADDRAW,"
        "  'at the same addresses, the template swap and the three draws among them')\n"
        "check(h:hide('buff') == true, 'and hides through them')\n"
        "return finish()\n");
    wait_frames(3);
    uint8_t* buff_menu = static_cast<uint8_t*>(open_on_game_thread("menu    buff"));
    check(hidden_in_place(kRowBuff, buff_menu), "the re-registered drain hid buff");

    // -- Lua closed with no release and no shutdown
    lua.close(L);
    check(engine_mapped(), "an engine collected without shutdown stays pinned");
    wait_frames(3);
    check(walk_of(buff_menu) == 'd' && buff_menu[0x77] == 1,
        "and its drain still restores what the collected handle held: buff drawn, +0x77 1");
    {
        typedef int (__fastcall* CloseFn)(void* mcb, void* edx, const char* key);
        CloseFn close_by_name;
        memcpy(&close_by_name, &g_fake->close, sizeof(close_by_name));
        pause_game();
        close_by_name(g_fake->mcb, NULL, "menu    buff    ");
        resume_game();
    }

    L = fresh(globals);
    phase(L,
        "hu = load_engine()\n"
        "h = hu.new('delta')\n"
        "check(hu.status().ok, 'a new Lua state reuses the pinned engine')\n"
        "h:release()\n"
        "local ok, why = hu.shutdown()\n"
        "check(ok == true, 'and shuts it down: ' .. tostring(why))\n"
        "return finish()\n");
    lua.close(L);
    check(engine_unmaps(), "then the engine unmaps");
    check(memcmp(g_fake->registry, g_fake->pristine, 371 * 0x2C) == 0, "and the registry is pristine");

    // -- a shutdown while no frame runs, as two live unloads of six met it on
    //    2026-10-21 (each waited 1.5 s for the frame, refused with "drain" and
    //    left the engine installed with no handle): accepted at once and
    //    closing, a second one starting nothing more; the next frame runs the
    //    restore, the engine's own thread then takes the hooks out, and the
    //    image unmaps once Lua lets it go
    L = fresh(globals);
    phase(L,
        "hu = load_engine()\n"
        "h = hu.new('paused')\n"
        "check(hu.status().ok and h:move('equip', 300, 200) == true, 'an engine for the paused shutdown moves equip')\n"
        "return finish()\n");
    wait_frames(3);
    check(origin(equip, 0) == 300 && origin(equip, 1) == 200, "equip moved to 300,200");
    pause_game();
    phase(L,
        "check(h:release() == true, 'the handle releases while no frame runs')\n"
        "return finish()\n");
    {
        const DWORD start = GetTickCount();
        phase(L,
            "ok, why, kind = hu.shutdown()\n"
            "return finish()\n");
        const DWORD took = GetTickCount() - start;
        char what[128];
        snprintf(what, sizeof(what), "the shutdown returns at once, not after the 1.5 s the engine waited for a"
            " frame: %lu ms", static_cast<unsigned long>(took));
        check(took < 250, what);
    }
    phase(L,
        "check(ok == true, 'and is accepted: ' .. tostring(why) .. ' ' .. tostring(kind))\n"
        "local s = hu.status()\n"
        "check(not s.ok and s.state == 'closing' and s.handles == 0 and s.detail.pinned,"
        "  'status: closing, not ok, still pinned: ' .. tostring(s.state))\n"
        "return finish()\n");
    check(engine_threads() == 1, "the engine runs one thread of its own, waiting for the frame");
    phase(L,
        "local ok2, why2 = hu.shutdown()\n"
        "check(ok2 == true and hu.status().state == 'closing',"
        "  'a second shutdown while closing is accepted too: ' .. tostring(why2))\n"
        "return finish()\n");
    check(engine_threads() == 1, "and starts no second thread");
    check(origin(equip, 0) == 300 && origin(equip, 1) == 200 && record_magic() == HU_ENGINE_MAGIC && claim_held(),
        "with no frame run, equip stays where the handle moved it, the record published and the claim held");
    resume_game();
    settle(L);
    phase(L,
        "local s = hu.status()\n"
        "check(not s.ok and s.state == 'idle' and not s.detail.pinned and not s.detail.busy,"
        "  'once a frame has run: idle and unpinned: ' .. tostring(s.state))\n"
        "return finish()\n");
    check(origin(equip, 0) == 100 + kRowEquip && origin(equip, 1) == 50 && registry_pristine(),
        "the frame ran the release and the restore: equip at its default rect, the registry pristine");
    check(record_magic() == 0 && !claim_held(), "and the hooks came out: the record retired, the claim given up");
    lua.close(L);
    check(engine_unmaps(), "closing Lua unmaps the engine");

    // -- lua reload: the unload's shutdown, then the load's new() before any
    //    frame. The handle takes the closing engine back: installed again,
    //    the queued restore running first and leaving the new handle's work
    //    alone, the engine's thread leaving without unhooking
    L = fresh(globals);
    phase(L,
        "hu = load_engine()\n"
        "h = hu.new('unloading')\n"
        "check(hu.status().ok and h:release() == true, 'an engine for the reload, its handle released')\n"
        "return finish()\n");
    pause_game();
    phase(L,
        "local ok, why = hu.shutdown()\n"
        "check(ok == true and hu.status().state == 'closing',"
        "  'the unload\\'s shutdown with no frame running: closing: ' .. tostring(why))\n"
        "return finish()\n");
    lua.close(L);
    check(engine_mapped() && engine_threads() == 1,
        "the unloaded addon's Lua state is closed; the engine stays mapped, its thread waiting");
    L = fresh(globals);
    phase(L,
        "hu = load_engine()\n"
        "h = hu.new('reloaded')\n"
        "local s = hu.status()\n"
        "check(h ~= nil and s.ok and s.state == 'installed' and s.role == 'resident' and s.handles == 1,"
        "  'the load\\'s new(), before any frame, takes the closing engine back: ' .. tostring(s.state))\n"
        "check(h:hide('logwindo') == true, 'and its handle hides logwindo')\n"
        "return finish()\n");
    {
        const DWORD start = GetTickCount();
        while (engine_threads() != 0 && GetTickCount() - start < 5000) {
            Sleep(1);
        }
    }
    check(engine_threads() == 0, "the engine's thread leaves with no frame run, its shutdown canceled");
    resume_game();
    wait_frames(3);
    check(hidden_in_place(kRowLog, log) && engine_mapped() && record_magic() == HU_ENGINE_MAGIC && claim_held(),
        "after the frame that ran the queued restore, logwindo is hidden all the same; the image, the record and the"
        " claim are still the engine's");
    phase(L,
        "local s = hu.status()\n"
        "check(s.ok and s.state == 'installed' and s.handles == 1 and s.detail.pinned,"
        "  'the engine is still installed and pinned: ' .. tostring(s.state))\n"
        "check(h:release() == true and hu.shutdown() == true, 'then the handle releases and the engine shuts down')\n"
        "return finish()\n");
    settle(L);
    check(record_magic() == 0 && !claim_held() && registry_pristine() && walk_of(log) == 'd',
        "which unhooks: the record retired, the claim given up, the registry pristine, logwindo drawn");
    lua.close(L);
    check(engine_unmaps(), "and the engine unmaps");

    // -- a hooked call still running when the engine's thread clears the
    //    handlers: the template swap parked in its pre on a page it cannot
    //    read. The engine stays idle and pinned for the session; once the
    //    call has returned, the next shutdown clears what is left
    L = fresh(globals);
    phase(L,
        "hu = load_engine()\n"
        "h = hu.new('busy')\n"
        "check(hu.status().ok, 'an engine for the busy shutdown: ' .. tostring(hu.status().detail.error))\n"
        "return finish()\n");
    {
        g_trap = static_cast<uint8_t*>(VirtualAlloc(NULL, 4096, MEM_RESERVE | MEM_COMMIT, PAGE_NOACCESS));
        g_trap_entered = CreateEventA(NULL, TRUE, FALSE, NULL);
        g_trap_release = CreateEventA(NULL, TRUE, FALSE, NULL);
        PVOID handler = AddVectoredExceptionHandler(1, &trap_handler);
        HANDLE swapper = g_trap && handler ? CreateThread(NULL, 0, &swap_into_trap, NULL, 0, NULL) : NULL;
        check(swapper && WaitForSingleObject(g_trap_entered, 5000) == WAIT_OBJECT_0,
            "a thread parked inside the hooked template swap's pre");
        phase(L,
            "check(h:release() == true and hu.shutdown() == true, 'the handle releases and the shutdown is accepted')\n"
            "return finish()\n");
        settle(L);
        phase(L,
            "local s = hu.status()\n"
            "check(not s.ok and s.state == 'idle' and s.detail.busy and s.detail.pinned,"
            "  'the swap\\'s handlers could not be cleared: idle, busy and pinned: ' .. tostring(s.state))\n"
            "return finish()\n");
        check(record_magic() == HU_ENGINE_MAGIC && claim_held(),
            "the record and the claim stay with the pinned engine");
        lua.close(L);
        check(engine_mapped(), "closing Lua leaves the engine mapped: the parked call is still inside it");
        SetEvent(g_trap_release);
        check(swapper && WaitForSingleObject(swapper, 5000) == WAIT_OBJECT_0, "let go, the parked call returns");
        if (swapper) {
            CloseHandle(swapper);
        }
        if (handler) {
            RemoveVectoredExceptionHandler(handler);
        }
        CloseHandle(g_trap_entered);
        CloseHandle(g_trap_release);
        if (g_trap) {
            VirtualFree(g_trap, 0, MEM_RELEASE);
        }
        g_trap = NULL;
    }
    L = fresh(globals);
    phase(L,
        "hu = load_engine()\n"
        "local ok, why = hu.shutdown()\n"
        "local s = hu.status()\n"
        "check(ok == true and s.state == 'idle' and not s.detail.busy and not s.detail.pinned,"
        "  'the next shutdown clears the handlers: idle, no longer busy or pinned: ' .. tostring(why))\n"
        "return finish()\n");
    check(record_magic() == 0 && !claim_held() && registry_pristine(),
        "the record retired, the claim given up, the registry pristine");
    lua.close(L);
    check(engine_unmaps(), "and the engine unmaps");

    // -- remembered sizes: both forms put back at every open, restored by a
    //    reset and by the last writer's release, owned apart from positions
    const int row_party = row_named("partywin");
    const int row_pmode = row_named("playermo");
    L = fresh(globals);
    memset(fl, 0, sizeof(*fl));
    phase(L,
        "hu = load_engine()\n"
        "ha = hu.new('size_a')\n"
        "hb = hu.new('size_b')\n"
        "check(hu.status().ok, 'a fresh engine for the sizes: ' .. tostring(hu.status().detail.error))\n"
        "check(ha:resize('partywin', 2) == true and ha:resize('menuwind', 50, 40) == true,"
        "  'A resizes the open partywin to 2 rows and the closed menuwind to 50x40')\n"
        "return finish()\n");
    wait_frames(3);
    check(strcmp(fl->swaps, "ptw2    |") == 0 && fl->swap_menu == live_menu(row_party)
            && fl->frame_menu == live_menu(row_party) && count_of(fl->order, 'F') == 1
            && get16(live_menu(row_party), 0x40) == get16(live_menu(row_party), 0x48),
        "partywin, open, is swapped to ptw2 at once and hung on its default bottom (the one SetFrameRect);"
        " menuwind, closed, is not touched");
    memset(fl, 0, sizeof(*fl));
    uint8_t* menuwind = static_cast<uint8_t*>(open_on_game_thread("menu    menuwind"));
    check(menuwind && fl->frame_menu == menuwind && fl->frame[0] == origin(menuwind, 0)
            && fl->frame[1] == origin(menuwind, 1) && fl->frame[2] == 50 && fl->frame[3] == 40,
        "menuwind, resized while closed, gets 50x40 at its origin in the post of its first open");
    phase(L,
        "local s = hu.status()\n"
        "check(#s.resized == 2 and #s.moved == 0, 'status: two windows resized, none moved')\n"
        "check(ha:close('partywin') == true and ha:close('menuwind') == true, 'partywin and menuwind close')\n"
        "return finish()\n");
    wait_frames(3);
    check(!live_menu(row_party) && !live_menu(kRowMenuwind), "both are closed");
    memset(fl, 0, sizeof(*fl));
    uint8_t* party2 = static_cast<uint8_t*>(open_on_game_thread("menu    partywin"));
    check(party2 && strcmp(fl->swaps, "ptw2    |") == 0 && fl->swap_menu == party2
            && get16(party2, 0x3A) == get16(party2, 0x52),
        "partywin reopened: its 2 rows put back on, the frame back on its origin");
    memset(fl, 0, sizeof(*fl));
    menuwind = static_cast<uint8_t*>(open_on_game_thread("menu    menuwind"));
    check(menuwind && fl->frame_menu == menuwind && fl->frame[2] == 50 && fl->frame[3] == 40,
        "menuwind reopened: 50x40 put back on");
    put16(pmode, 0x58, 5);
    phase(L,
        "check(hb:move('menuwind', 400, 300) == true, 'B moves menuwind, whose size A wrote')\n"
        "check(ha:move('playermo', 30, 60) == true and hb:resize('playermo', 3) == true,"
        "  'A moves playermo; B resizes it to 3 rows')\n"
        "return finish()\n");
    wait_frames(3);
    phase(L,
        "local m = ha:info('menuwind').memory\n"
        "local mp = m and m.position or {}\n"
        "check(mp.x == 400 and mp.y == 300 and mp.owner == 'size_b' and not mp.mine and m.size and m.size.w == 50"
        "  and m.size.h == 40 and m.size.rows == nil and m.size.owner == 'size_a' and m.size.mine,"
        "  'info(menuwind) through A: B owns the position, A the size')\n"
        "local p = hb:remembered()\n"
        "local pp = p.playermo and p.playermo.position or {}\n"
        "check(pp.x == 30 and pp.owner == 'size_a' and not pp.mine"
        "  and p.playermo.size and p.playermo.size.rows == 3 and p.playermo.size.mine,"
        "  'remembered() through B: A owns the position of playermo, B its size')\n"
        "check(p.partywin and p.partywin.position == nil and p.partywin.size.rows == 2 and p.partywin.size.owner == 'size_a',"
        "  'remembered(): a window only resized has its size and no position')\n"
        "return finish()\n");
    memset(fl, 0, sizeof(*fl));
    phase(L,
        "check(hb:release() == true, 'B releases')\n"
        "return finish()\n");
    wait_frames(3);
    check(origin(menuwind, 0) == 100 + kRowMenuwind && origin(menuwind, 1) == 50 && count_of(fl->order, 'F') == 1
            && get16(menuwind, 0x3E) - get16(menuwind, 0x3A) == 50 && get16(menuwind, 0x40) - get16(menuwind, 0x3C) == 40,
        "B's release puts menuwind back at its default and leaves the size A wrote, 50x40");
    check(strcmp(fl->swaps, "playermo|") == 0 && fl->swap_menu == pmode && origin(pmode, 0) == 30 && origin(pmode, 1) == 60
            && fl->frame_menu == pmode && fl->frame[0] == 30 && fl->frame[1] == 60 && fl->frame[2] == 120
            && fl->frame[3] == 40,
        "and swaps playermo back to its own template, then SetFrameRect(30, 60, 120, 40): the size it had before"
        " B's resize, at the position A wrote, which stays");
    check(get16(pmode, 0x58) == 5 && fl->cursor_calls == 2 && fl->cursor_menu == pmode && fl->cursor_items[0] == 5
            && fl->cursor_items[1] == 5,
        "and puts back playermo's item count, the 5 it had before B's resize, with SetCursor before the swap back"
        " and again after");
    memset(fl, 0, sizeof(*fl));
    phase(L,
        "check(ha:reset('partywin') == true, 'A resets partywin')\n"
        "return finish()\n");
    wait_frames(3);
    check(strcmp(fl->swaps, "partywin|") == 0 && fl->swap_menu == party2 && origin(party2, 0) == 100 + row_party
            && origin(party2, 1) == 50 && get16(party2, 0x3A) == get16(party2, 0x52) && fl->frame_menu == party2
            && fl->frame[1] + fl->frame[3] == 90 && get16(party2, 0x40) == 90,
        "reset partywin: swapped back to the template it was opened from, then home on its default bottom 90 at"
        " the size the swap left, never the 120x40 it had before its first resize (a bottom-anchored class)");
    memset(fl, 0, sizeof(*fl));
    phase(L,
        "check(ha:info('partywin').memory == nil and ha:remembered().partywin == nil, 'and its size is forgotten')\n"
        "check(ha:release() == true, 'A releases')\n"
        "local ok, why = hu.shutdown()\n"
        "check(ok == true, 'and the engine shuts down: ' .. tostring(why))\n"
        "return finish()\n");
    settle(L);
    check(fl->frame_menu == menuwind && fl->frame[0] == 100 + kRowMenuwind && fl->frame[1] == 50
            && fl->frame[2] == 120 && fl->frame[3] == 40,
        "A's release, as the last writer of the size of menuwind, restores its own: the 120x40 its frame had at"
        " the open that first put the size on");
    check(origin(pmode, 0) == 100 + row_pmode && origin(pmode, 1) == 50 && registry_pristine(),
        "and puts playermo back at its default; the registry is pristine");
    lua.close(L);
    check(engine_unmaps(), "the engine of the sizes unmaps");

    // -- the template swap (0.11.0): an owner's swap through the stand-in's
    //    hooked entry, with the frame snapped to the template's spot, puts
    //    back the position a handle gave the window and nothing else: the
    //    party list by its bottom edge, the swap's size standing; playermo's
    //    rows the owner's
    L = fresh(globals);
    phase(L,
        "hu = load_engine()\n"
        "h = hu.new('swapper')\n"
        "check(hu.status().ok, 'an engine for the template swap: ' .. tostring(hu.status().detail.error))\n"
        "return finish()\n");
    {
        const int prow = row_named("partywin");
        const int arow = row_named("playermo");
        const bool party_was_open = live_menu(prow) != NULL;
        const bool actions_were_open = live_menu(arow) != NULL;
        if (!party_was_open) {
            open_on_game_thread("menu    partywin");
        }
        if (!actions_were_open) {
            open_on_game_thread("menu    playermo");
        }
        phase(L,
            "check(h:move('partywin', 600, 300) == true and h:resize('playermo', 4) == true,"
            "  'move partywin by its frame; playermo four rows')\n"
            "return finish()\n");
        wait_frames(3);
        uint8_t* party = live_menu(prow);
        pause_game();
        const int bottom = get16(party, 0x40);
        fl->swaps[0] = '\0';
        resume_game();
        InterlockedExchange(g_fake->party_swap, 5);
        wait_frames(3);
        pause_game();
        const bool roster = strcmp(fl->swaps, "ptw5    |") == 0 && get16(party, 0x3A) == 600
            && get16(party, 0x40) == bottom && get16(party, 0x3C) == 7;
        fl->swaps[0] = '\0';
        resume_game();
        InterlockedExchange(g_fake->action_swap, 6);
        wait_frames(3);
        pause_game();
        const bool rows = strcmp(fl->swaps, "actionm6|") == 0;
        resume_game();
        check(roster && rows,
            "partywin's owner swaps to ptw5: its frame goes back to the move's left, 600, on the bottom edge the"
            " move gave it, the swap's taller frame standing; playermo's owner swaps to actionm6 and the engine"
            " swaps nothing back");
        char reads[512];
        snprintf(reads, sizeof(reads),
            "local p = h:info('partywin')\n"
            "check(p.rect.x == 600 and p.rect.bottom == %d and h:remembered().playermo.size.rows == 4,"
            "  'info(partywin): the frame at x 600 on its bottom edge; playermo four rows still remembered for its"
            " next open')\n"
            "check(h:reset('partywin') == true and h:reset('playermo') == true, 'reset both')\n"
            "return finish()\n", bottom);
        phase(L, reads);
        wait_frames(3);
        char closes[256];
        snprintf(closes, sizeof(closes), "%s%sreturn finish()\n",
            party_was_open ? "" : "check(h:close('partywin') == true, 'close(partywin)')\n",
            actions_were_open ? "" : "check(h:close('playermo') == true, 'close(playermo)')\n");
        phase(L, closes);
        wait_frames(3);
        phase(L,
            "check(h:release() == true and hu.shutdown() == true, 'swapper releases, and the engine shuts down')\n"
            "return finish()\n");
        settle(L);
        check(registry_pristine(), "after the template swap's engine, the registry is pristine");
    }
    lua.close(L);
    check(engine_unmaps(), "the engine of the template swap unmaps");

    // -- the trade windows (0.11.0): offer, withdraw, the OK and the cancel
    //    through the game's own stage, OK, cancel and close, the game's bytes
    //    over the stand-in's player data, packet queue and clock; the
    //    trade's offers on their way until the acknowledgment writes them;
    //    one change a second; the gray test run as the change is made; the
    //    NPC trade's offer sent whole at its OK, then closed; a blocked NPC
    //    trade refusing every reply
    L = fresh(globals);
    phase(L,
        "hu = load_engine()\n"
        "h = hu.new('trader')\n"
        "check(hu.status().ok, 'an engine for the trade windows: ' .. tostring(hu.status().detail.error))\n"
        "function trade_errors(handle)\n"
        "  local out = {}\n"
        "  for _, e in ipairs((handle:poll())) do\n"
        "    if e.event == 'error' then out[#out + 1] = e.verb .. ' ' .. e.name .. ': ' .. e.reason end\n"
        "  end\n"
        "  return table.concat(out, ' | ')\n"
        "end\n"
        "local f = hu.status().detail.functions.offer or {}\n"
        "check(f.trade_input == TRADEINPUT and f.trade_open == TRADEOPEN and f.handover_input == HANDOVERINPUT"
        "  and f.rows == INVENTORROWS and f.trade_stage == TRADESTAGE and f.own == 0x194B8 and f.theirs == 0x19300"
        "  and f.inventory == 0x9860 and f.last_send == 0x41B4 and f.interval == 0x43E2C and f.lock == 0x1AF"
        "  and f.actor_titled == 0x210 and f.actor_title == 0x214 and f.actor_name == 0x7C,"
        "  'the trade windows\\' sites, routines and offsets resolved to the stand-in\\'s')\n"
        "return finish()\n");
    {
        FakeLog* tl = g_fake->log;
        uint8_t* pd = g_fake->player;
        uint8_t* ctl = controller_named("trade");
        const int kRowTrade = row_named("trade");
        const int kRowHandover = row_named("handover");
        auto item = [&](int index, int id, uint32_t count) {
            uint8_t* r = pd + 0x9860 + index * 0x2C;
            memset(r, 0, 0x2C);
            put16(r, 0, id);
            r[2] = static_cast<uint8_t>(index);
            put32(r, 4, count);
        };
        auto tick = [&]() {
            pause_game();
            put32(g_fake->clock, 0x0C, get32(g_fake->clock, 0x0C) + 2);
            resume_game();
        };
        pause_game();
        item(1, 4096, 12);
        item(2, 4097, 3);
        item(3, 4096, 5);
        put32(g_fake->gil_record, 4, 1000);
        memset(pd + 0x194B8, 0, 10 * 8);
        memset(pd + 0x19300, 0, 10 * 0x2C);
        resume_game();
        uint8_t* menu = static_cast<uint8_t*>(open_on_game_thread("menu    trade"));
        pause_game();
        put16(menu, 0x58, 11);
        put32(ctl, 0x6C, 3);
        put32(ctl, 0x70, 0x01000003);
        put32(ctl, 0xC0, 3);
        put32(ctl, 0xC4, 3);
        put16(pd + 0x19300 + 2 * 0x2C, 0, 640);
        put32(pd + 0x19300 + 2 * 0x2C, 4, 12);
        resume_game();
        tick();
        const int grays = tl->grays;
        wait_frames(2);
        const bool tested = tl->grays > grays && tl->gray_lock == 1 && tl->gray_mode;
        int commits = tl->commits;
        phase(L,
            "trade_errors(h)\n"
            "local o = h:options('trade')\n"
            "check(o and o.name == 'trade' and o.partner == 'Zaldon' and o.mine.gil == 0 and #o.mine.items == 0"
            "  and o.theirs.items[1].slot == 2 and o.theirs.items[1].id == 640 and o.theirs.items[1].count == 12"
            "  and o.theirs.items[1].item == nil and o.ok_mine == false and o.ok_theirs == false and o.next_change == 0"
            "  and type(o.gray) == 'table' and #o.gray == 0,"
            "  'options(trade): the partner\\'s name through the actor table, the partner\\'s offer, nothing of mine,'"
            "  .. ' a change allowed now, nothing grayed')\n"
            "check(h:offer('trade', 1, 4) == true, 'offer(trade, 1, 4) queues')\n"
            "return finish()\n");
        wait_frames(3);
        check(tested && tl->commits == commits + 1 && get16(tl->packet, 0) == 0x34 && get32(tl->packet, 4) == 4
                && get16(tl->packet, 8) == 4096 && tl->packet[0x0A] == 1 && tl->packet[0x0B] == 1
                && get16(menu, 0x4C) == 1 && tl->gray_lock == 1 && get32(ctl, 0xC0) == 3,
            "the gray test run each frame with the trade open, the item's data locked; the offer: row 1 selected, the"
            " game's stage sending 4 of item 4096 from index 1 into slot 1");
        phase(L,
            "local ok, why = h:offer('trade', 2, 1)\n"
            "check(ok == nil and tostring(why):find('the game sends one trade change per second; try again in ', 1, true)"
            "  == 1, 'a second change within the second refused with the wait: ' .. tostring(why))\n"
            "local o = h:options('trade')\n"
            "local it = o.mine.items[1]\n"
            "check(#o.mine.items == 1 and it.pending == true and it.item == 1 and it.slot == 1 and it.id == 4096"
            "  and it.count == 4 and o.next_change > 0 and o.next_change < 1.1,"
            "  'options(trade): the item on its way, pending, the next change within the second')\n"
            "return finish()\n");
        tick();
        phase(L,
            "local ok, why = h:offer('trade', 1, 2)\n"
            "check(ok == nil and why == 'that item is already offered; withdraw it first', 'the item on its way offered"
            " again: ' .. tostring(why))\n"
            "ok, why = h:withdraw('trade', 1)\n"
            "check(ok == nil and tostring(why):find('the server has not acknowledged that item', 1, true) == 1,"
            "  'withdraw of the item on its way: ' .. tostring(why))\n"
            "ok, why = h:offer('trade', 2, 4)\n"
            "check(ok == nil and why == 'inventory slot 2 holds 3 of that item, fewer than 4', 'more than the slot"
            " holds: ' .. tostring(why))\n"
            "ok, why = h:offer('trade', 'gil', 1001)\n"
            "check(ok == nil and why == 'the player holds 1000 gil', 'more gil than held: ' .. tostring(why))\n"
            "ok, why = h:offer('trade', 9, 1)\n"
            "check(ok == nil and why == 'inventory slot 9 holds no item', 'an empty inventory slot: ' .. tostring(why))\n"
            "ok, why = h:offer('equip', 1, 1)\n"
            "check(ok == nil and why == 'equip takes no offer; offer() takes trade and handover', 'a window that takes"
            " no offer: ' .. tostring(why))\n"
            "local raised, err = pcall(h.offer, h, 'trade', 1.5, 1)\n"
            "check(not raised and tostring(err):find('whole number', 1, true), 'a fractional index raises: '"
            "  .. tostring(err))\n"
            "raised, err = pcall(h.offer, h, 'trade', 2, 0.5)\n"
            "check(not raised and tostring(err):find('the count is a whole number', 1, true), 'a fractional count"
            " raises: ' .. tostring(err))\n"
            "return finish()\n");
        // the acknowledgment writes the item into the acknowledged offer
        pause_game();
        put32(pd + 0x194B8 + 8, 0, 4);
        put16(pd + 0x194B8 + 8, 4, 4096);
        pd[0x194B8 + 8 + 6] = 1;
        resume_game();
        wait_frames(2);
        commits = tl->commits;
        phase(L,
            "local o = h:options('trade')\n"
            "local it = o.mine.items[1]\n"
            "check(#o.mine.items == 1 and it.pending == nil and it.item == 1 and it.slot == 1,"
            "  'acknowledged: the item in the offer, no longer pending')\n"
            "check(h:withdraw('trade', 1) == true, 'withdraw(trade, 1) queues')\n"
            "return finish()\n");
        wait_frames(3);
        check(tl->commits == commits + 1 && get32(tl->packet, 4) == 0 && tl->packet[0x0A] == 1
                && tl->packet[0x0B] == 1 && get16(menu, 0x4C) == 1,
            "the withdrawal: row 1 selected, the game's stage sending a count of 0 for index 1 in slot 1, its removal");
        tick();
        commits = tl->commits;
        phase(L,
            "check(h:offer('trade', 'gil', 250) == true, 'offer(trade, gil, 250) queues')\n"
            "return finish()\n");
        wait_frames(3);
        check(tl->commits == commits + 1 && get32(tl->packet, 4) == 250 && tl->packet[0x0A] == 0
                && tl->packet[0x0B] == 0 && get16(menu, 0x4C) == 11,
            "the gil: row 11 selected, the game's stage sending 250 gil into slot 0");
        tick();
        commits = tl->commits;
        // the game grays index 2 out: refused up front by the drain's rows
        pause_game();
        *g_fake->gray_answer = 0x80404040u;
        *g_fake->gray_index = 2;
        resume_game();
        wait_frames(2);
        phase(L,
            "trade_errors(h)\n"
            "local o = h:options('trade')\n"
            "check(#o.gray == 1 and o.gray[1] == 2, 'options(trade).gray: the index the game grays out')\n"
            "local ok, why = h:offer('trade', 2, 1)\n"
            "check(ok == nil and why == 'that item cannot be traded', 'an item the game grays out refused up front: '"
            "  .. tostring(why))\n"
            "return finish()\n");
        wait_frames(3);
        const bool up_front = tl->commits == commits;
        // grayed between the Lua thread's check and the drain: the game
        // thread's test refuses it by an error event
        pause_game();
        *g_fake->gray_answer = 0x80808080u;
        *g_fake->gray_index = 0;
        resume_game();
        wait_frames(2);
        pause_game();
        *g_fake->gray_answer = 0x80404040u;
        *g_fake->gray_index = 2;
        phase(L,
            "trade_errors(h)\n"
            "check(#h:options('trade').gray == 0, 'the drain\\'s rows from before the change: nothing grayed')\n"
            "check(h:offer('trade', 2, 1) == true, 'so an offer of the item queues')\n"
            "return finish()\n");
        resume_game();
        wait_frames(3);
        pause_game();
        *g_fake->gray_answer = 0x80808080u;
        *g_fake->gray_index = 0;
        resume_game();
        wait_frames(2);
        check(up_front && tl->commits == commits, "an item the game grays out: no change sent, refused up front or by"
            " the game thread");
        phase(L,
            "local seen = trade_errors(h)\n"
            "check(seen == 'offer trade: that item cannot be traded',"
            "  'the game\\'s gray test refuses it on the game thread: ' .. seen)\n"
            "check(#h:options('trade').gray == 0, 'listed again: nothing grayed')\n"
            "check(h:answer('trade') == true, 'answer(trade), the OK, queues')\n"
            "return finish()\n");
        wait_frames(3);
        const bool okd = tl->commits == commits && tl->trade_sends[0] >= 1 && get32(ctl, 0xC0) == 4;
        tick();
        phase(L,
            "local ok, why = h:offer('trade', 2, 1)\n"
            "check(ok == nil and why == 'your OK is in: the trade takes no more changes', 'a change after the OK: '"
            "  .. tostring(why))\n"
            "ok, why = h:answer('trade', true)\n"
            "check(ok == nil and why == 'your OK is already in', 'the OK again: ' .. tostring(why))\n"
            "local o = h:options('trade')\n"
            "check(o.ok_mine == true, 'options(trade): ok_mine')\n"
            "check(h:cancel('trade') == true, 'cancel(trade) queues')\n"
            "return finish()\n");
        wait_frames(3);
        check(okd && tl->trade_sends[1] >= 1 && !live_menu(kRowTrade),
            "the OK through the game's OK, its state then 4; the cancel through the game's cancel, the trade's windows"
            " closed by its close");
        phase(L,
            "local ok, why = h:offer('trade', 2, 1)\n"
            "check(ok == nil and why == 'trade is not open', 'with the trade closed: ' .. tostring(why))\n"
            "return finish()\n");

        // the NPC trade
        uint8_t* hctl = controller_named("handover");
        uint8_t* hmenu = static_cast<uint8_t*>(open_on_game_thread("menu    handover"));
        pause_game();
        memset(hctl + 0x14, 0, 0x28);
        put32(hctl, 0x3C, 0);
        put32(hctl, 0x44, 5);
        put32(hctl, 0x48, 0x01000005);
        resume_game();
        tick();
        phase(L,
            "local o = h:options('handover')\n"
            "check(o and o.name == 'handover' and o.npc == 'Moogle' and #o.items == 0 and o.gil == 0 and o.next_ok == 0"
            "  and #o.gray == 0 and o.next_change == nil,"
            "  'options(handover): the NPC\\'s titled name through the actor table, nothing offered, the NPC trade'"
            "  .. ' allowed now, nothing grayed')\n"
            "local ok, why = h:answer('handover')\n"
            "check(ok == nil and why == 'nothing is offered; offer an item or gil first', 'the OK with nothing offered: '"
            "  .. tostring(why))\n"
            "check(h:offer('handover', 1, 12) == true, 'offer(handover, 1, 12) queues')\n"
            "return finish()\n");
        wait_frames(3);
        phase(L,
            "check(h:offer('handover', 2, 3) == true, 'offer(handover, 2, 3) queues')\n"
            "return finish()\n");
        wait_frames(3);
        const bool two = hmenu && get16(hctl, 0x18) == 4096 && hctl[0x1A] == 1 && hctl[0x1B] == 12
            && get16(hctl, 0x1C) == 4097 && hctl[0x1E] == 2 && hctl[0x1F] == 3 && hctl[0x40] == 2;
        phase(L,
            "check(h:withdraw('handover', 1) == true, 'withdraw(handover, 1) queues')\n"
            "return finish()\n");
        wait_frames(3);
        phase(L,
            "check(h:offer('handover', 'gil', 300) == true, 'offer(handover, gil, 300) queues')\n"
            "return finish()\n");
        wait_frames(3);
        const bool staged = get16(hctl, 0x18) == 0 && hctl[0x1B] == 0 && get32(hctl, 0x3C) == 300
            && hctl[0x40] == 1;
        commits = tl->commits;
        phase(L,
            "local o = h:options('handover')\n"
            "check(#o.items == 1 and o.items[1].slot == 2 and o.items[1].item == 2 and o.items[1].count == 3"
            "  and o.gil == 300, 'options(handover): the item left in slot 2 and the gil')\n"
            "check(h:answer('handover') == true, 'answer(handover), its OK, queues')\n"
            "return finish()\n");
        wait_frames(3);
        check(two && staged && tl->commits == commits + 1 && get16(tl->packet, 0) == 0x36 && !live_menu(kRowHandover)
                && get32(hctl, 0x44) == 0,
            "the NPC trade: items into slots 1 and 2 through +0x40 and the game's stage, the first taken back out by"
            " staging it again, the gil through the stage's index 0; the OK sending its packet, then the window"
            " closed");
        // the next NPC trade within the second: its OK refused up front with
        // the wait; an item the game grays out for it refused up front
        hmenu = static_cast<uint8_t*>(open_on_game_thread("menu    handover"));
        pause_game();
        memset(hctl + 0x14, 0, 0x28);
        put32(hctl, 0x3C, 0);
        put32(hctl, 0x44, 5);
        put32(hctl, 0x48, 0x01000005);
        *g_fake->gray_answer = 0x80404040u;
        *g_fake->gray_index = 3;
        resume_game();
        wait_frames(2);
        commits = tl->commits;
        phase(L,
            "local o = h:options('handover')\n"
            "check(o.next_ok > 0 and o.next_ok < 1.1 and #o.gray == 1 and o.gray[1] == 3, 'options(handover): the NPC"
            " trade within the second, next_ok ' .. tostring(o.next_ok) .. '; the index the game grays out')\n"
            "local ok, why = h:offer('handover', 3, 1)\n"
            "check(ok == nil and why == 'that item cannot be handed to an NPC', 'an item the game grays out for the"
            " NPC refused up front: ' .. tostring(why))\n"
            "check(h:offer('handover', 2, 1) == true, 'offer(handover, 2, 1) queues')\n"
            "return finish()\n");
        wait_frames(3);
        phase(L,
            "local ok, why = h:answer('handover', true)\n"
            "check(ok == nil and tostring(why):find('the game sends one NPC trade per second; try again in ', 1, true)"
            "  == 1, 'the OK within the second refused up front with the wait: ' .. tostring(why))\n"
            "return finish()\n");
        const bool held_back = tl->commits == commits && live_menu(kRowHandover) && hctl[0x1A] == 2;
        tick();
        pause_game();
        *g_fake->gray_answer = 0x80808080u;
        *g_fake->gray_index = 0;
        resume_game();
        wait_frames(2);
        phase(L,
            "local o = h:options('handover')\n"
            "check(o.next_ok == 0 and #o.gray == 0, 'options(handover) a second on: next_ok 0, nothing grayed')\n"
            "check(h:cancel('handover') == true, 'cancel(handover) queues')\n"
            "return finish()\n");
        wait_frames(3);
        check(hmenu && held_back && tl->commits == commits && !live_menu(kRowHandover),
            "the next NPC trade: nothing sent within the second, the window up with the item in slot 1; canceled,"
            " closed");
        phase(L,
            "check(h:block('handover') == true, 'block(handover)')\n"
            "return finish()\n");
        wait_frames(2);
        const bool refused = open_on_game_thread("menu    handover") == NULL;
        wait_frames(2);
        phase(L,
            "local ok, why = h:offer('handover', 2, 1)\n"
            "check(ok == nil and why == 'handover is blocked and never opens: the NPC trade does not start, and"
            " nothing waits on a reply', 'a blocked handover takes no offer: ' .. tostring(why))\n"
            "ok, why = h:answer('handover')\n"
            "check(ok == nil and tostring(why):find('handover is blocked and never opens', 1, true) == 1, 'nor its OK: '"
            "  .. tostring(why))\n"
            "ok, why = h:cancel('handover')\n"
            "check(ok == nil and tostring(why):find('handover is blocked and never opens', 1, true) == 1, 'nor its"
            " cancel: ' .. tostring(why))\n"
            "check(h:unblock('handover') == true, 'unblock(handover)')\n"
            "return finish()\n");
        wait_frames(2);
        check(refused, "a blocked handover never opens");
        phase(L,
            "check(h:release() == true and hu.shutdown() == true, 'trader releases, and the engine shuts down')\n"
            "return finish()\n");
        settle(L);
        check(registry_pristine(), "after the trade windows' engine, the registry is pristine");
    }
    lua.close(L);
    check(engine_unmaps(), "the engine of the trade windows unmaps");

    // -- the settings (0.11.0): settings() in the Config menu's order with
    //    each kind's fields, read from the stand-in's store, the game's
    //    object and entries; setting(); set_setting() of each kind through
    //    the game's own set and its subscribers, a bit or a chat filter row
    //    composed with every other bit kept, the inverted bools and a choice
    //    on a bit, the equipment lock, auto_disconnect and
    //    character_information through their routines; misuse raising, every
    //    refusal, and a page that writes its copy back as it closes refusing
    //    by an error event
    {
        FakeSettings* fs = g_fake->settings;
        auto entry = [&](int id) { return fs->entries + id * 0x2C; };
        auto word = [&](int id) { return static_cast<int32_t>(get32(entry(id), 8)); };
        typedef int (__fastcall* CloseFn)(void* mcb, void* edx, const char* key);
        CloseFn close_by_name;
        memcpy(&close_by_name, &g_fake->close, sizeof(close_by_name));
        auto game_close = [&](const char* key) {
            pause_game();
            close_by_name(g_fake->mcb, NULL, key);
            resume_game();
        };
        char addresses[5][16];
        const void* const fns[5] = {fs->set, fs->store_ptr, fs->lock, fs->idle, fs->flag};
        for (int i = 0; i < 5; ++i) {
            snprintf(addresses[i], sizeof(addresses[i]), "0x%08X",
                static_cast<unsigned>(reinterpret_cast<uintptr_t>(fns[i])));
        }
        const char* sg[] = {"S_SET", addresses[0], "S_STORE", addresses[1], "S_LOCK", addresses[2], "S_IDLE",
                            addresses[3], "S_FLAG", addresses[4], NULL};
        L = fresh(sg);
        phase(L,
            "hu = load_engine()\n"
            "h = hu.new('settings')\n"
            "local s = hu.status()\n"
            "local d = s.detail.functions.settings\n"
            "check(s.ok and d.set == S_SET and d.store == S_STORE and d.lock == S_LOCK and d.idle == S_IDLE"
            "  and d.flag == S_FLAG and d.loaded == true and d.flags == 0x30 and d.anonymous == 4,"
            "  'the settings resolve on the stand-in: the store\\'s set and global, the lock\\'s request, the idle'"
            "  .. ' timer, the flag\\'s request, the player flags at +0x30 bit 2; the store loaded')\n"
            "return finish()\n");
        wait_frames(2);
        put32(entry(0x92), 8, 0x20006);
        phase(L,
            "local list = h:settings()\n"
            "local by = {}\n"
            "for i, e in ipairs(list) do by[e.name] = e; e.index = i end\n"
            "check(#list == 268 and list[1].name == 'auto_target' and list[2].name == 'sound_effects_volume'"
            "  and list[9].name == 'filter_say' and list[268].name == 'term_filter',"
            "  'settings(): all 268, in the Config menu\\'s order')\n"
            "local m = by.music_volume\n"
            "check(m.page == 'conf2win' and m.kind == 'number' and m.value == 100 and m.default == 100 and m.min == 0"
            "  and m.max == 100 and m.label == nil and m.choices == nil,"
            "  'a number: its value, default, min and max from its store entry; no label where the game has none')\n"
            "local sh = by.shadows\n"
            "check(sh.kind == 'choice' and sh.value == 'normal' and sh.default == 'normal' and sh.min == nil"
            "  and table.concat(sh.choices, ' ') == 'high normal off', 'a choice: its value by name, its choices in'"
            "  .. ' its page\\'s row order')\n"
            "local c = by.color_say\n"
            "check(c.kind == 'color' and c.value.r == 128 and c.value.g == 128 and c.value.b == 128 and c.default.r"
            "  == 128 and c.min == 0 and c.max == 255 and c.page == 'conftxtc' and c.label == 'Immediate vicinity'"
            "  .. ' (\"Say\")', 'a color: {r, g, b}, each channel 0..255, and the game\\'s label')\n"
            "local f = by.footstep_effects\n"
            "check(f.kind == 'bool' and f.value == true and f.default == true and f.min == nil,"
            "  'a bool: true while its word is 0, for a setting whose 0 is on')\n"
            "local k = by.key1_move_up\n"
            "check(k.kind == 'key' and k.value == 44 and k.min == 0 and k.max == 255 and k.label == 'Move Up'"
            "  and k.page == 'k1assign', 'a key: its DirectInput code, 0..255, and its action\\'s label')\n"
            "local t = by.filter_tell\n"
            "check(t.kind == 'number' and t.value == 2 and t.default == 2 and t.min == 0 and t.max == 3"
            "  and t.label == 'Tell' and by.filter_say.max == 2 and by.filter_say.label == 'Say'"
            "  and by.filter_tell_messages_deemed_spam.value == true and by.filter_yell.value == false,"
            "  'the chat filter rows: Tell\\'s four states, its flag on by default; Say\\'s three; the spam filters'"
            "  .. ' on by default')\n"
            "check(by.log1_say.value == false and by.log2_say.value == true and by.log1_battle_messages.value == true"
            "  and by.log1_battle_messages.label == 'Battle Messages' and by.effect_filter_you_to_you.value == true"
            "  and by.effect_filter_all_effects_during_battle.label == 'All effects during battle',"
            "  'the routing\\'s and the battle effects filter\\'s bits')\n"
            "local a = by.character_information\n"
            "check(a.kind == 'choice' and a.value == 'show' and a.default == nil"
            "  and table.concat(a.choices, ' ') == 'show hide' and by.auto_disconnect.value == -1"
            "  and by.auto_disconnect.min == -1 and by.auto_disconnect.max == 60,"
            "  'character_information from the player\\'s flags, which the game keeps no default of; auto_disconnect\\'s'"
            "  .. ' -1..60')\n"
            "local i = by.icon_type\n"
            "check(i.kind == 'choice' and i.value == 'type2' and i.default == 'type2' and i.min == nil"
            "  and table.concat(i.choices, ' ') == 'type1 type2' and by.auto_target.kind == 'bool'"
            "  and by.auto_target.value == true and by.style_lock.value == 'normal'"
            "  and table.concat(by.style_lock.choices, ' ') == 'normal static',"
            "  'a choice on a bit, its bit clear type2; auto_target on while its word is 0; style_lock a choice')\n"
            "check(h:setting('MUSIC_Volume') == 100 and h:setting('shadows') == 'normal'"
            "  and h:setting('color_say').b == 128, 'setting(name): the value, by a name in any case')\n"
            "local v, why = h:setting('nosuch')\n"
            "check(v == nil and why == 'no such setting: nosuch (settings() lists them)', 'setting(nosuch)')\n"
            "local raised, err = pcall(h.setting, h, 42)\n"
            "check(not raised and tostring(err):find('string expected', 1, true), 'setting(42) raises')\n"
            "local function misuse(name, value, text)\n"
            "  local ok, e = pcall(h.set_setting, h, name, value)\n"
            "  check(not ok and tostring(e):find(text, 1, true), 'set_setting(' .. name .. ', ' .. tostring(value)"
            "    .. ') raises: ' .. tostring(e))\n"
            "end\n"
            "misuse('music_volume', 'loud', \"set_setting('music_volume', value): music_volume takes a whole number,\")\n"
            "misuse('footstep_effects', 1, 'footstep_effects takes true or false')\n"
            "misuse('shadows', 2, \"shadows takes one of its choices' names, a string\")\n"
            "misuse('color_say', 5, 'color_say takes a color, {r = red, g = green, b = blue}')\n"
            "misuse('color_say', {r = 1, g = 2}, 'color_say takes a color')\n"
            "misuse('color_say', {1, 2, 3}, 'color_say takes a color')\n"
            "misuse('color_say', {r = 1, g = 2, b = 3, a = 4}, 'color_say takes a color')\n"
            "misuse('key1_move_up', true, 'key1_move_up takes a DirectInput key code, a number')\n"
            "local function refused(name, value, text)\n"
            "  local ok, e = h:set_setting(name, value)\n"
            "  check(ok == nil and e == text, 'set_setting(' .. name .. ', ' .. tostring(value) .. ') refused: '"
            "    .. tostring(e))\n"
            "end\n"
            "refused('nosuch', 1, 'no such setting: nosuch (settings() lists them)')\n"
            "refused('music_volume', 101, 'music_volume takes a whole number from 0 to 100')\n"
            "refused('music_volume', 50.5, 'music_volume takes a whole number from 0 to 100')\n"
            "refused('shadows', 'ultra', 'no such choice of shadows: ultra (high, normal or off)')\n"
            "refused('color_say', {r = 256, g = 0, b = 0}, 'each of r, g and b is a whole number from 0 to 255')\n"
            "refused('color_say', {r = 1.5, g = 0, b = 0}, 'each of r, g and b is a whole number from 0 to 255')\n"
            "refused('key1_move_up', 256, 'key1_move_up takes a whole number from 0 to 255')\n"
            "refused('auto_disconnect', 61, 'auto_disconnect takes a whole number from -1 to 60')\n"
            "refused('icon_type', 'type3', 'no such choice of icon_type: type3 (type1 or type2)')\n"
            "refused('filter_say', 3, 'filter_say takes a whole number from 0 to 2')\n"
            "return finish()\n");

        // A number whose store entry has no range (its max -1), which the
        // store's set takes any value for: no min or max, and refused.
        put32(entry(0x0A), 0x1C, 0xFFFFFFFFu);
        phase(L,
            "local m\n"
            "for _, e in ipairs(h:settings()) do if e.name == 'music_volume' then m = e end end\n"
            "check(m and m.value == 100 and m.min == nil and m.max == nil,"
            "  'settings(): a number whose store entry has no range has no min or max')\n"
            "local ok, why = h:set_setting('music_volume', 40)\n"
            "check(ok == nil and why == \"the game's store gives this setting no range, so the values it takes are\""
            "  .. \" unknown\", 'set_setting of it refused: ' .. tostring(why))\n"
            "return finish()\n");
        put32(entry(0x0A), 0x1C, 100);

        // Each kind through the game's set, its subscriber called with what it
        // stores.
        InterlockedExchange(&fs->notifies, 0);
        phase(L,
            "check(h:set_setting('music_volume', 40) == true and h:set_setting('shadows', 'high') == true"
            "  and h:set_setting('footstep_effects', false) == true"
            "  and h:set_setting('color_say', {r = 160, g = 64, b = 160}) == true"
            "  and h:set_setting('key1_move_up', 0x11) == true, 'a number, a choice, a bool, a color and a key queue')\n"
            "return finish()\n");
        wait_frames(3);
        check(word(0x0A) == 40 && word(0x3A) == 2 && word(0x3F) == 1 && static_cast<uint32_t>(word(0x1B)) == 0x80A040A0u
                && word(0x4A) == 0x11 && fs->notifies == 5 && fs->notified[0][0] == 0x0A && fs->notified[0][1] == 40
                && fs->notified[3][0] == 0x1B && static_cast<uint32_t>(fs->notified[3][1]) == 0x80A040A0u,
            "set_setting through the game's set: music_volume 40, shadows high (2), footstep_effects off (1),"
            " color_say 0x80A040A0, key1_move_up 0x11 (W), each id's subscriber run with the value stored");
        InterlockedExchange(&fs->notifies, 0);
        phase(L,
            "check(h:setting('music_volume') == 40 and h:setting('color_say').r == 160 and h:setting('shadows') =="
            "  'high' and h:setting('footstep_effects') == false, 'setting() reads what the sets stored')\n"
            "check(h:set_setting('music_volume', 40) == true, 'a set to the value already stored queues')\n"
            "check(h:set_setting('effect_filter_you_to_you', false) == true"
            "  and h:set_setting('log2_hp_mp_you_recover', true) == true"
            "  and h:set_setting('target_expressions', false) == true, 'three bits queue')\n"
            "return finish()\n");
        wait_frames(3);
        check(word(0x0A) == 40 && fs->notifies == 3 && word(0x92) == 0x20004 && word(0xBC) == 2 && word(0xBB) == -1
                && word(0x99) == 0x80,
            "a set to the stored value changes nothing and runs no subscriber; bits composed into their words as they"
            " are, every other bit kept: 0x92's bit 1 cleared beside bits 2 and 17, log window 2's bit 33 0xBC's bit"
            " 1 (0xBB untouched), 0x99's bit 7 set for target_expressions off");
        // The bools whose page's ON is a stored 0, and two whose ON is a 1.
        InterlockedExchange(&fs->notifies, 0);
        static const char kInvertedNames[] =
            "{'auto_target', 'window_effect', 'log_window1_reactive_sizing', 'log_window2_reactive_sizing',"
            " 'screen_edge_panning', 'chat_language_filter', 'weapon_effect'}";
        static const uint8_t kInvertedIds[] = {0x07, 0xA4, 0x0E, 0xB6, 0x8F, 0x18, 0xBE};
        char inverted_chunk[1024];
        snprintf(inverted_chunk, sizeof(inverted_chunk),
            "for _, n in ipairs(%s) do check(h:set_setting(n, false) == true, n .. ' off queues') end\n"
            "check(h:set_setting('inventory_sort', true) == true and h:set_setting('weather_effects', false) == true,"
            "  'inventory_sort on and weather_effects off queue')\n"
            "return finish()\n", kInvertedNames);
        phase(L, inverted_chunk);
        wait_frames(3);
        bool off_one = word(0x9A) == 1 && word(0x3B) == 0;
        for (size_t k = 0; k < sizeof(kInvertedIds); ++k) {
            off_one = off_one && word(kInvertedIds[k]) == 1;
        }
        snprintf(inverted_chunk, sizeof(inverted_chunk),
            "for _, n in ipairs(%s) do\n"
            "  check(h:setting(n) == false and h:set_setting(n, true) == true, n .. ' reads off, and on queues')\n"
            "end\n"
            "return finish()\n", kInvertedNames);
        phase(L, inverted_chunk);
        wait_frames(3);
        bool on_zero = true;
        for (size_t k = 0; k < sizeof(kInvertedIds); ++k) {
            on_zero = on_zero && word(kInvertedIds[k]) == 0;
        }
        check(off_one && on_zero,
            "each bool whose page's ON is a stored 0 (auto_target, window_effect, both log windows' reactive sizing,"
            " screen_edge_panning, chat_language_filter, weapon_effect) stores 1 for false and 0 for true;"
            " inventory_sort's true a 1 and weather_effects' false a 0");
        // A choice on a bit: icon_type's type1 sets 0x99's bit 2.
        phase(L,
            "check(h:set_setting('icon_type', 'TYPE1') == true, 'icon_type type1 queues')\n"
            "return finish()\n");
        wait_frames(3);
        const bool icon_one = word(0x99) == 0x84;
        phase(L,
            "check(h:setting('icon_type') == 'type1' and h:set_setting('icon_type', 'type2') == true,"
            "  'icon_type reads type1, and type2 queues')\n"
            "return finish()\n");
        wait_frames(3);
        check(icon_one && word(0x99) == 0x80,
            "a choice on a bit: icon_type's type1 (in any case) sets 0x99's bit 2, type2 clears it, bit 7 kept");
        InterlockedExchange(&fs->notifies, 0);
        phase(L,
            "check(h:set_setting('filter_say', 2) == true and h:set_setting('filter_say', 1) == true"
            "  and h:set_setting('filter_tell', 1) == true, 'chat filter rows queue')\n"
            "return finish()\n");
        wait_frames(3);
        check(word(0x32) == 1 && word(0x34) == 4 && word(0x47) == 0 && word(0x33) == 0x1800 && fs->notifies == 5
                && fs->notified[0][0] == 0x34 && fs->notified[0][1] == 1 && fs->notified[1][0] == 0x32
                && fs->notified[2][0] == 0x34 && fs->notified[2][1] == 0 && fs->notified[3][0] == 0x34
                && fs->notified[3][1] == 4 && fs->notified[4][0] == 0x47 && fs->notified[4][1] == 0,
            "a chat filter row: Say's second state, then its mask bit (0x32, then 0x34 cleared, as the page writes"
            " them), Tell's second state with its flag cleared; the spam filters' bits kept");
        // The equipment lock: the game's request through its own bytes, its
        // wait 9000 ms on the clock from its stamp, which starts at 0.
        auto lock_set = [&](const char* value, const char* reason) {
            char chunk[768];
            snprintf(chunk, sizeof(chunk),
                "h:poll()\n"
                "check(h:set_setting('style_lock', '%s') == true, 'style_lock %s queues')\n"
                "return finish()\n", value, value);
            phase(L, chunk);
            wait_frames(3);
            snprintf(chunk, sizeof(chunk),
                "local errors = {}\n"
                "for _, e in ipairs((h:poll())) do\n"
                "  if e.event == 'error' then errors[#errors + 1] = e end\n"
                "end\n"
                "local want = %s%s%s\n"
                "local e = errors[1] or {}\n"
                "check(want == nil and #errors == 0 or #errors == 1 and e.verb == 'set_setting' and e.name == 'style_lock'"
                "  and e.reason == want, 'style_lock %s: ' .. (want and 'the error event ' .. want or 'no error event')"
                "  .. ', got ' .. tostring(e.reason))\n"
                "return finish()\n", reason ? "'" : "", reason ? reason : "nil", reason ? "'" : "", value);
            phase(L, chunk);
        };
        InterlockedExchange(&fs->notifies, 0);
        fs->lock_clock = 100000;
        lock_set("static", NULL);
        const bool locked = word(0xBF) == 1 && fs->lock_requests == 1 && fs->lock_args[0] == 4 && fs->lock_args[1] == 0
            && *fs->lock_stamp == 100000 && fs->notifies == 1 && fs->notified[0][0] == 0xBF && fs->notified[0][1] == 1;
        InterlockedExchange(&fs->notifies, 0);
        fs->lock_clock = 104500;
        lock_set("normal", "the game allows one style lock change every 9 seconds; 5 s left");
        const bool waited = word(0xBF) == 1 && fs->lock_requests == 1 && fs->lock_messages == 1
            && *fs->lock_stamp == 100000 && fs->notifies == 0;
        fs->lock_busy = 1;
        lock_set("normal", "the game is not accepting style lock changes right now");
        const bool gated = word(0xBF) == 1 && fs->lock_requests == 1 && fs->lock_messages == 1
            && *fs->lock_stamp == 100000 && fs->notifies == 0;
        fs->lock_busy = 0;
        fs->lock_ok = 0;
        fs->lock_clock = 120000;
        lock_set("normal", "the game could not send the request");
        const bool unsent = word(0xBF) == 1 && fs->lock_requests == 2 && fs->lock_args[0] == 0
            && *fs->lock_stamp == 120000 && fs->notifies == 0;
        fs->lock_ok = 1;
        fs->lock_clock = 140000;
        lock_set("normal", NULL);
        check(locked && waited && gated && unsent && word(0xBF) == 0 && fs->lock_requests == 3 && fs->lock_args[0] == 0
                && *fs->lock_stamp == 140000 && fs->notifies == 1 && fs->notified[0][1] == 0,
            "style_lock, a choice, through the game's request alone, its own bytes: static (mode 4) and normal (mode"
            " 0) written by the"
            " game as its request goes; its wait's refusal (its message, 5 of its 9 seconds left on its clock and"
            " stamp), its gate's and a failed send each an error event, 0xBF untouched");
        phase(L,
            "check(h:set_setting('auto_disconnect', 30) == true, 'auto_disconnect 30 queues')\n"
            "return finish()\n");
        wait_frames(3);
        const bool idle_on = word(0x41) == 30 && fs->idle_ms == 1800000 && fs->idle_on == 1;
        phase(L,
            "check(h:set_setting('auto_disconnect', -1) == true and h:set_setting('character_information', 'hide')"
            "  == true, 'auto_disconnect off and character_information hide queue')\n"
            "return finish()\n");
        wait_frames(3);
        check(idle_on && word(0x41) == -1 && fs->idle_on == 0 && fs->idle_ms_calls == 1 && fs->flag_sets == 1
                && fs->flag_args[0] == 0 && fs->flag_args[1] == 1,
            "auto_disconnect through the game's timer: 30 minutes stored and 1800000 ms handed on, -1 off;"
            " character_information's hide through the player flag's request, flag 0 on");
        fs->player_flags[0] = 4;
        phase(L,
            "check(h:setting('character_information') == 'hide', 'character_information read from the player\\'s"
            " flags')\n"
            "return finish()\n");
        fs->player_flags[0] = 0;

        // A page that writes its own copy back as it closes, open.
        open_on_game_thread("menu    cfilter ");
        open_on_game_thread("menu    conf2win");
        wait_frames(2);
        InterlockedExchange(&fs->notifies, 0);
        phase(L,
            "h:poll()\n"
            "check(h:set_setting('filter_say', 0) == true and h:set_setting('music_volume', 60) == true"
            "  and h:set_setting('filter_others_synthesis_and_fishing_results', true) == true,"
            "  'sets with cfilter and conf2win open queue')\n"
            "return finish()\n");
        wait_frames(3);
        phase(L,
            "local errors = {}\n"
            "for _, e in ipairs((h:poll())) do\n"
            "  if e.event == 'error' then errors[#errors + 1] = e end\n"
            "end\n"
            "local e = errors[1] or {}\n"
            "local long = errors[2] or {}\n"
            "check(#errors == 2 and e.verb == 'set_setting' and e.name == 'filter_say' and e.reason == 'cfilter is open:'"
            "  .. ' it writes its own copy back as it closes, over this; set it once cfilter has closed',"
            "  'a set while cfilter is open: an error event: ' .. tostring(e.reason))\n"
            "check(long.verb == 'set_setting' and long.name == 'filter_others_synthesis_and_fishing_results',"
            "  'the error event for a 43-character setting name carries the whole name: ' .. tostring(long.name))\n"
            "check(h:setting('music_volume') == 60, 'a page that writes at once takes a set while it is open')\n"
            "return finish()\n");
        check(word(0x32) == 1 && fs->notifies == 1, "the refused set stores nothing");
        game_close("menu    cfilter ");
        game_close("menu    conf2win");

        // The key pages as one: a key2_ set while k1assign is open.
        open_on_game_thread("menu    k1assign");
        wait_frames(2);
        InterlockedExchange(&fs->notifies, 0);
        phase(L,
            "h:poll()\n"
            "check(h:set_setting('key2_attack', 0x20) == true, 'key2_attack with k1assign open queues')\n"
            "return finish()\n");
        wait_frames(3);
        phase(L,
            "local errors = {}\n"
            "for _, e in ipairs((h:poll())) do\n"
            "  if e.event == 'error' then errors[#errors + 1] = e end\n"
            "end\n"
            "local e = errors[1] or {}\n"
            "check(#errors == 1 and e.name == 'key2_attack' and e.reason == 'k1assign is open: the two key pages share'"
            "  .. ' one copy of the keys, and a set now would apply it in the middle of the page\\'s edit; set it once'"
            "  .. ' k1assign has closed', 'a key2_ set while k1assign is open: an error event: ' .. tostring(e.reason))\n"
            "return finish()\n");
        check(fs->notifies == 0 && word(0x6A) != 0x20, "the refused key set stores nothing");
        game_close("menu    k1assign");

        // The player not in the game.
        pause_game();
        fs->store[9] = 0;
        resume_game();
        phase(L,
            "local ok, why = h:set_setting('music_volume', 10)\n"
            "local list, why2 = h:settings()\n"
            "local v, why3 = h:setting('music_volume')\n"
            "local text = \"the game's settings are not loaded: the player is not in the game\"\n"
            "check(ok == nil and why == text and list == nil and why2 == text and v == nil and why3 == text,"
            "  'with the store not loaded: set_setting, settings() and setting() refused: ' .. tostring(why))\n"
            "check(h:release() == true and hu.shutdown() == true, 'the settings\\' handle releases, the engine shuts down')\n"
            "return finish()\n");
        pause_game();
        fs->store[9] = 1;
        resume_game();
        settle(L);
        check(registry_pristine(), "after the settings' engine, the registry is pristine");
    }
    lua.close(L);
    check(engine_unmaps(), "the engine of the settings unmaps");

    // -- the chat log by its bottom edge, as measured on the 2026-09-10 client
    //    (origin 16,930, default rect 16,930..382,1064, the player's log
    //    1774x166 at rest, 16,898..1790,1064); the party list carried with
    //    it, partywin as measured with one member (origin 1792,930, default
    //    rect 1792,930..1904,1064, frame 1792,1030..1904,1064); resets of
    //    moved and resized windows back on the game's placement
    {
        uint8_t* target = static_cast<uint8_t*>(open_on_game_thread("menu    targetwi"));
        put16(log, 0x42, 16);
        put16(log, 0x44, 930);
        put16(log, 0x46, 382);
        put16(log, 0x48, 1064);
        put16(log, 0x52, 16);
        put16(log, 0x54, 930);
        put16(log, 0x3A, 16);
        put16(log, 0x3C, 898);
        put16(log, 0x3E, 1790);
        put16(log, 0x40, 1064);
        put16(equip, 0x3E, get16(equip, 0x3A) + 200);
        put16(equip, 0x40, get16(equip, 0x3C) + 100);
        put16(party2, 0x42, 1792);
        put16(party2, 0x44, 930);
        put16(party2, 0x46, 1904);
        put16(party2, 0x48, 1064);
        put16(party2, 0x52, 1792);
        put16(party2, 0x54, 930);
        put16(party2, 0x3A, 1792);
        put16(party2, 0x3C, 1030);
        put16(party2, 0x3E, 1904);
        put16(party2, 0x40, 1064);
        const int party_y = origin(party2, 1);
        const int target_y = target ? origin(target, 1) : 0;
        L = fresh(globals);
        memset(fl, 0, sizeof(*fl));
        phase(L,
            "hu = load_engine()\n"
            "h = hu.new('logs')\n"
            "check(hu.status().ok, 'a fresh engine for the chat log: ' .. tostring(hu.status().detail.error))\n"
            "check(h:info('logwindo').detail.layout == 'bottom' and h:info('equip').detail.layout == 'self',"
            "  'info: logwindo is placed by its bottom edge, equip refreshes itself')\n"
            "check(h:info('partywin').detail.layout == 'bottom',"
            "  'info: partywin, its frame 1030..1064 on its default rect 930..1064, reads as placed by its bottom edge')\n"
            "local li, pi = h:info('logwindo'), h:info('partywin')\n"
            "check(h:move('logwindo', li.rect.x, li.rect.y) == true and h:move('partywin', pi.rect.x, pi.rect.y) == true,"
            "  'logwindo and partywin moved to their own frame top-left, ' .. li.rect.x .. ',' .. li.rect.y .. ' and '"
            "  .. pi.rect.x .. ',' .. pi.rect.y)\n"
            "return finish()\n");
        wait_frames(3);
        check(origin(log, 0) == 16 && origin(log, 1) == 930 && get16(log, 0x3C) == 898 && get16(log, 0x40) == 1064
                && origin(party2, 0) == 1792 && origin(party2, 1) == 930 && get16(party2, 0x3C) == 1030
                && get16(party2, 0x40) == 1064,
            "move(name, info(name).rect.x, info(name).rect.y) changes nothing for the bottom-anchored logwindo"
            " (frame 898..1064 on origin 930) and partywin (1030..1064 on 930)");
        memset(fl, 0, sizeof(*fl));
        phase(L,
            "check(h:reset('logwindo') == true and h:reset('partywin') == true, 'both reset')\n"
            "check(h:move_group('chat_log', 16, 798) == true, 'move_group chat_log to its frame top 798 (100 up) queues')\n"
            "return finish()\n");
        wait_frames(3);
        check(origin(log, 0) == 16 && origin(log, 1) == 830 && get16(log, 0x3A) == 16 && get16(log, 0x3C) == 798
                && get16(log, 0x3E) == 1790 && get16(log, 0x40) == 964 && count_of(fl->order, 'F') == 4,
            "move_group chat_log to the frame top-left 16,798: logwindo's origin to 830, then SetFrameRect(16, 798,"
            " 1774, 166): its bottom 964");
        check(target && origin(party2, 1) == party_y - 100 && origin(target, 1) == target_y - 100
                && (get32(target, 0x34) & 0x2000),
            "and the party list with it: partywin and targetwi, still docked to it, 100 up");
        check(origin(party2, 0) == 1792 && origin(party2, 1) == 830 && get16(party2, 0x3A) == 1792
                && get16(party2, 0x3C) == 930 && get16(party2, 0x3E) == 1904 && get16(party2, 0x40) == 964
                && fl->frame_menu == party2 && fl->frame[0] == 1792 && fl->frame[1] == 930 && fl->frame[2] == 112
                && fl->frame[3] == 34,
            "partywin by its bottom edge: origin 830, then SetFrameRect(1792, 930, 112, 34): frame 930..964, 100 up"
            " (SetPosition alone put it at 830..864)");
        memset(fl, 0, sizeof(*fl));
        phase(L,
            "check(h:info('partywin').detail.layout == 'bottom' and h:info('logwindo').detail.layout == 'bottom',"
            "  'info: partywin and logwindo, moved off their default bottom, still read as placed by it')\n"
            "local pw = h:info('partywin').memory.position\n"
            "local lw = h:remembered().logwindo.position\n"
            "check(pw.group == 'chat_log' and pw.mine and pw.y == 830 and lw.group == 'chat_log' and lw.y == 798,"
            "  'remembered(): partywin and logwindo remembered as the chat_log group\\'s move, logwindo as given, by its'"
            "  .. ' frame top 798, partywin by its origin')\n"
            "local x = hu.new('intruder')\n"
            "local r1, w1 = x:reset_group('chat_log')\n"
            "local r2, w2 = x:reset('logwindo')\n"
            "local r3, w3 = x:reset('partywin', 'position')\n"
            "check(r1 == nil and w1 == 'chat_log was placed by logs' and r2 == nil and w2 == 'logwindo was placed by logs'"
            "  and r3 == nil and w3 == 'partywin was placed by logs' and x:reset('logwindo', 'size') == true,"
            "  'another handle\\'s reset of what logs placed is refused, naming logs; of an aspect nobody placed it is'"
            "  .. ' no change: ' .. tostring(w1) .. ' / ' .. tostring(w2))\n"
            "x:release()\n"
            "check(h:move_group('chat_log', 16, 898) == true, 'move_group chat_log to the frame top 898 queues')\n"
            "return finish()\n");
        wait_frames(3);
        check(origin(log, 1) == 930 && get16(log, 0x3C) == 898 && get16(log, 0x40) == 1064 && origin(party2, 1) == 930
                && get16(party2, 0x3C) == 1030 && get16(party2, 0x40) == 1064 && origin(target, 1) == target_y
                && count_of(fl->order, 'F') == 2,
            "move_group chat_log to the frame top 898: back where the game had them, logwindo 898..1064 and partywin"
            " 1030..1064");
        {
            HuEngineApi api;
            memset(&api, 0, sizeof(api));
            HuEngineHandle old = {-1, 0};
            char why[512] = "";
            const bool read = record_api(&api);
            if (read) {
                api.handle_new("abi4", &old);
            }
            const bool moved = read && old.slot >= 0 && api.move3(&old, "logwindo", 16, 830, why, sizeof(why)) == 1;
            wait_frames(3);
            const bool by_origin = origin(log, 1) == 830 && get16(log, 0x3C) == 798;
            const bool reset = read && old.slot >= 0 && api.reset3(&old, "logwindo", why, sizeof(why)) == 1;
            wait_frames(3);
            check(moved && by_origin && reset && origin(log, 1) == 930 && get16(log, 0x3C) == 898,
                "engine abi 3's move, as a 0.5.0 to 0.6.3 copy calls it, still takes logwindo's origin: 830, frame"
                " 798..964; and its reset still resets whoever placed it");
            if (read && old.slot >= 0) {
                api.handle_release(&old);
            }
            phase(L,
                "check(h:move_group('chat_log', 16, 898) == true, 'logs places the chat log group again')\n"
                "return finish()\n");
            wait_frames(3);
        }
        phase(L,
            "check(h:reset('logwindo') == true, 'reset logwindo queues')\n"
            "return finish()\n");
        wait_frames(3);
        check(origin(log, 0) == 16 && origin(log, 1) == 930 && get16(log, 0x3C) == 898 && get16(log, 0x40) == 1064
                && fl->frame[0] == 16 && fl->frame[1] == 898 && fl->frame[2] == 1774 && fl->frame[3] == 166,
            "reset logwindo: SetPosition(16,930), then SetFrameRect(16, 898, 1774, 166): 898..1064 as the game keeps it");
        phase(L,
            "check(h:resize('logwindo', 1000, 120) == true and h:move('logwindo', 40, 714) == true,"
            "  'logwindo resized to 1000x120 and moved by its frame top-left to 40,714')\n"
            "check(h:move('equip', 333, 222) == true and h:resize('equip', 50, 60) == true, 'equip moved and resized')\n"
            "return finish()\n");
        wait_frames(3);
        check(origin(log, 0) == 40 && origin(log, 1) == 700 && get16(log, 0x3A) == 40 && get16(log, 0x3C) == 714
                && get16(log, 0x3E) == 1040 && get16(log, 0x40) == 834,
            "logwindo resized, then moved: frame 40,714..1040,834, origin 700, its bottom on 700 + its default height");
        check(origin(equip, 0) == 333 && origin(equip, 1) == 222 && get16(equip, 0x3E) == 383 && get16(equip, 0x40) == 282,
            "equip moved to 333,222 and resized to 50x60");
        phase(L,
            "check(h:reset('logwindo') == true and h:reset('equip') == true, 'both reset')\n"
            "return finish()\n");
        wait_frames(3);
        check(origin(log, 0) == 16 && origin(log, 1) == 930 && get16(log, 0x3A) == 16 && get16(log, 0x3C) == 944
                && get16(log, 0x3E) == 1016 && get16(log, 0x40) == 1064,
            "reset of the moved and resized logwindo: home at 16,930 on its bottom 1064 at the 1000x120 it has now"
            " (a bottom-anchored window's height is its content's: no recorded size, never the default rect's"
            " 366x134)");
        check(origin(equip, 0) == 100 + kRowEquip && origin(equip, 1) == 50 && get16(equip, 0x3A) == 100 + kRowEquip
                && get16(equip, 0x3C) == 50 && get16(equip, 0x3E) == 300 + kRowEquip && get16(equip, 0x40) == 150
                && fl->frame_menu == equip && fl->frame[2] == 200 && fl->frame[3] == 100,
            "reset of the moved and resized equip: the game's placement at the 200x100 its frame had before the"
            " resize, not the default rect's 120x40");
        phase(L,
            "check(h:resize('equip', 300, 200) == true, 'equip resized to 300x200')\n"
            "h2 = hu.new('logs_b')\n"
            "check(h2:resize('equip', 80, 90) == true, 'then by a second handle to 80x90')\n"
            "return finish()\n");
        wait_frames(3);
        check(get16(equip, 0x3E) - get16(equip, 0x3A) == 80 && get16(equip, 0x40) - get16(equip, 0x3C) == 90,
            "equip is 80x90, the second handle's size");
        memset(fl, 0, sizeof(*fl));
        phase(L,
            "check(h2:release() == true, 'the second handle, last writer of the size, releases')\n"
            "return finish()\n");
        wait_frames(3);
        check(fl->frame_menu == equip && fl->frame[2] == 200 && fl->frame[3] == 100 && get16(equip, 0x3E) == 300 + kRowEquip
                && get16(equip, 0x40) == 150 && count_of(fl->order, 'F') == 1,
            "two resizes by two handles: the release restores the 200x100 recorded before the first, not the"
            " first handle's 300x200");
        phase(L,
            "check(h:info('equip').memory == nil, 'and the size is forgotten')\n"
            "check(h:release() == true, 'the chat log handle releases')\n"
            "local ok, why = hu.shutdown()\n"
            "check(ok == true, 'and the engine shuts down: ' .. tostring(why))\n"
            "return finish()\n");
        settle(L);
        check(origin(party2, 1) == party_y && get16(party2, 0x3C) == 1030 && get16(party2, 0x40) == 1064 && target
                && origin(target, 1) == target_y && registry_pristine(),
            "the release puts partywin (frame 1030..1064) and targetwi back; the registry is pristine");
        lua.close(L);
        check(engine_unmaps(), "the engine of the chat log unmaps");
    }

    // -- partywin with six members: its frame exactly its default rect
    //    (1792,930..1904,1064, 34 + 5 x 20 tall) at the engine's first touch,
    //    which only its class says hangs from the bottom; equip on its default
    //    rect still reads top-anchored
    {
        put16(party2, 0x3A, 1792);
        put16(party2, 0x3C, 930);
        put16(party2, 0x3E, 1904);
        put16(party2, 0x40, 1064);
        for (int i = 0; i < 4; ++i) {
            put16(equip, 0x3A + 2 * i, get16(equip, 0x42 + 2 * i));
        }
        put16(equip, 0x52, get16(equip, 0x42));
        put16(equip, 0x54, get16(equip, 0x44));
        L = fresh(globals);
        memset(fl, 0, sizeof(*fl));
        phase(L,
            "hu = load_engine()\n"
            "h = hu.new('six')\n"
            "check(hu.status().ok and hu.status().engine == '0.11.0', 'a fresh engine for the six-member party list: '"
            "  .. tostring(hu.status().detail.error))\n"
            "local p, e = h:info('partywin'), h:info('equip')\n"
            "check(p.detail.layout == 'bottom' and p.rect.y == p.default.y and p.rect.bottom == p.default.bottom"
            "  and p.rect.h == 134,"
            "  'info: partywin, its frame exactly its default rect 930..1064, reads as placed by its bottom edge')\n"
            "check(e.detail.layout == 'self' and e.rect.y == e.default.y and e.rect.bottom == e.default.bottom,"
            "  'info: equip, its frame exactly its default rect, does not')\n"
            "check(h:move('partywin', 1500, 500) == true and h:move('equip', 333, 222) == true,"
            "  'partywin and equip moved')\n"
            "return finish()\n");
        wait_frames(3);
        check(strcmp(fl->order, "MFM") == 0 && origin(party2, 0) == 1500 && origin(party2, 1) == 500
                && fl->frame_menu == party2 && fl->frame[0] == 1500 && fl->frame[1] == 500 && fl->frame[2] == 112
                && fl->frame[3] == 134 && get16(party2, 0x3C) == 500 && get16(party2, 0x40) == 634,
            "move partywin 1500,500: SetPosition, then SetFrameRect(1500, 500, 112, 134), its bottom on 500 + its"
            " default height (634); move equip: SetPosition alone");
        check(origin(equip, 0) == 333 && origin(equip, 1) == 222 && get16(equip, 0x3A) == 333
                && get16(equip, 0x3C) == 222 && get16(equip, 0x3E) == 453 && get16(equip, 0x40) == 262,
            "equip's frame follows SetPosition at its size: read as top-anchored");
        put16(party2, 0x3C, 600);
        memset(fl, 0, sizeof(*fl));
        phase(L,
            "check(h:reset('partywin') == true, 'partywin, its roster down to one (frame 600..634), reset')\n"
            "return finish()\n");
        wait_frames(3);
        check(origin(party2, 0) == 1792 && origin(party2, 1) == 930 && get16(party2, 0x3C) == 1030
                && get16(party2, 0x40) == 1064 && fl->frame[0] == 1792 && fl->frame[1] == 1030 && fl->frame[2] == 112
                && fl->frame[3] == 34,
            "reset partywin: home at 1792,930, SetFrameRect(1792, 1030, 112, 34) on its bottom 1064 at the size it"
            " has now");
        phase(L,
            "check(h:release() == true, 'the six-member handle releases')\n"
            "local ok, why = hu.shutdown()\n"
            "check(ok == true, 'and the engine shuts down: ' .. tostring(why))\n"
            "return finish()\n");
        settle(L);
        check(origin(equip, 0) == 100 + kRowEquip && origin(equip, 1) == 50 && registry_pristine(),
            "the release puts equip back at its default rect; the registry is pristine");
        lua.close(L);
        check(engine_unmaps(), "the engine of the six-member party list unmaps");
    }

    // -- a group move whose anchor is closed waits for the game to open it:
    //    the target window closed and the sub-target window, which its group
    //    carries, open; then events lost, and the resync that says so
    {
        const int row_target = row_named("targetwi");
        put32(controller_named("targetwi"), 8, 0);
        uint8_t* sub = static_cast<uint8_t*>(open_on_game_thread("menu    subwindo"));
        const int sx = sub ? origin(sub, 0) : 0;
        const int sy = sub ? origin(sub, 1) : 0;
        L = fresh(globals);
        phase(L,
            "hu = load_engine()\n"
            "h = hu.new('waits')\n"
            "check(hu.status().ok, 'a fresh engine for the waiting group move: ' .. tostring(hu.status().detail.error))\n"
            "check(h:move_group('target_window', 1500, 600) == true,"
            "  'move_group target_window to 1500,600 with targetwi closed queues')\n"
            "h:poll()\n"
            "other = hu.new('other')\n"
            "check(other:move_group('target_window', 10, 20) == true, 'another handle\\'s move of target_window queues')\n"
            "return finish()\n");
        wait_frames(3);
        phase(L,
            "local function errors(handle)\n"
            "  local out = {}\n"
            "  for _, e in ipairs((handle:poll())) do\n"
            "    if e.event == 'error' then out[#out + 1] = e.verb .. ' ' .. e.name .. ': ' .. e.reason end\n"
            "  end\n"
            "  return table.concat(out, ' | ')\n"
            "end\n"
            "local mine = errors(h)\n"
            "check(mine == \"move_group target_window: replaced by other's move\" and h:groups().target_window.waiting.owner"
            "  == 'other', 'another handle\\'s move_group replacing the waiting one: an error event to its maker, naming'"
            "  .. ' the other handle: ' .. mine)\n"
            "other:poll()\n"
            "check(h:move_group('target_window', 1500, 600) == true, 'waits moves the group again')\n"
            "return finish()\n");
        wait_frames(3);
        phase(L,
            "local out = {}\n"
            "for _, e in ipairs((other:poll())) do out[#out + 1] = e.event .. ' ' .. tostring(e.reason) end\n"
            "check(table.concat(out, ' | ') == \"error replaced by waits's move\", 'and other is told in turn: '"
            "  .. table.concat(out, ' | '))\n"
            "other:release()\n"
            "return finish()\n");
        HuEngineApi api;
        memset(&api, 0, sizeof(api));
        HuEngineHandle old = {-1, 0};
        char why[512] = "";
        const bool read = record_api(&api);
        if (read) {
            api.handle_new("abi3", &old);
        }
        const int32_t refused = read && old.slot >= 0
            ? api.move_group3(&old, "target_window", 1, 2, why, sizeof(why)) : -2;
        check(refused == 0 && strstr(why, "its anchor targetwi is not open") != NULL,
            "engine abi 3's move_group, as a 0.5.0 copy calls it, still refuses with the anchor closed");
        if (read && old.slot >= 0) {
            api.handle_release(&old);
        }
        phase(L,
            "local g = h:groups().target_window\n"
            "check(not g.anchor_open and g.origin == nil and g.waiting and g.waiting.x == 1500 and g.waiting.y == 600"
            "  and g.waiting.owner == 'waits' and g.waiting.mine, 'groups(): the move waits for targetwi, owned by waits')\n"
            "local lay = h:layout()\n"
            "check(lay.groups and lay.groups.target_window and lay.groups.target_window.x == 1500"
            "  and lay.groups.target_window.y == 600 and lay.positions == nil,"
            "  'layout(): the waiting move as the group\\'s entry')\n"
            "check(h:remembered().targetwi == nil and h:remembered().subwindo == nil,"
            "  'remembered(): nothing yet for either window')\n"
            "return finish()\n");
        check(sub && origin(sub, 0) == sx && origin(sub, 1) == sy, "subwindo has not moved while targetwi is closed");
        uint8_t* target = static_cast<uint8_t*>(open_on_game_thread("menu    targetwi"));
        const int dx = 1500 - (100 + row_target);
        const int dy = 600 - 50;
        check(target && origin(target, 0) == 1500 && origin(target, 1) == 600 && origin(sub, 0) == sx + dx
                && origin(sub, 1) == sy + dy && (get32(target, 0x34) & 0x2000),
            "the game opens targetwi: from where the game put it to 1500,600, still docked, and the open subwindo"
            " carried by as much");
        phase(L,
            "local g = h:groups().target_window\n"
            "check(g.anchor_open and g.waiting == nil and g.origin.x == 1500 and g.origin.y == 600,"
            "  'groups(): targetwi open at 1500,600, nothing waiting')\n"
            "local r = h:remembered()\n"
            "check(r.targetwi and r.targetwi.position.group == 'target_window' and r.targetwi.position.x == 1500"
            "  and r.subwindo and r.subwindo.position.group == 'target_window' and r.subwindo.position.mine,"
            "  'remembered(): targetwi and subwindo, as the target_window group\\'s move')\n"
            "local lay = h:layout()\n"
            "check(lay.groups.target_window.x == 1500 and lay.groups.target_window.y == 600 and lay.positions == nil,"
            "  'layout(): the same entry, now from the anchor')\n"
            "h:poll()\n"
            "return finish()\n");
        put16(sub, 0x4C, 4);
        wait_frames(3);
        phase(L,
            "local out = {}\n"
            "for _, e in ipairs((h:poll())) do out[#out + 1] = e.event .. ':' .. e.name .. ':' .. tostring(e.row) end\n"
            "check(table.concat(out, ' ') == 'cursor:subwindo:4', 'subwindo\\'s cursor row moved: cursor{subwindo, 4}: '"
            "  .. table.concat(out, ' '))\n"
            "check(h:reset_group('target_window') == true, 'reset_group target_window queues')\n"
            "return finish()\n");
        wait_frames(3);
        check(origin(target, 0) == 100 + row_target && origin(target, 1) == 50 && origin(sub, 0) == sx
                && origin(sub, 1) == sy,
            "reset_group target_window: targetwi and subwindo, which its move carried, back where the game had them");

        // 0.7.2: query's cursor is the option under it, an index into
        // options('query'), and the row only counts the three rows shown. Four
        // options, Down three times: the option 2, 3, 4 and the first shown
        // 1, 1, 2 while the row goes 2, 3, 3; then the game's confirm.
        {
            static uint8_t nodes[4][0x18];
            static uint8_t items[4][0x108];
            uint8_t* qmenu = static_cast<uint8_t*>(open_on_game_thread("menu    query"));
            const uint32_t head = get32(qctl, 0x14);
            const uint32_t count = get32(qctl, 0x24);
            const int16_t word = get16(qctl, 0x548);
            typedef void (__fastcall* DownFn)(void* ctl, void* edx);
            typedef uint32_t (__fastcall* ConfirmFn)(void* ctl, void* edx);
            DownFn down;
            ConfirmFn confirm;
            memcpy(&down, &g_fake->query_down, sizeof(down));
            memcpy(&confirm, &g_fake->calls[0], sizeof(confirm));
            pause_game();
            memset(nodes, 0, sizeof(nodes));
            memset(items, 0, sizeof(items));
            for (int i = 0; i < 4; ++i) {
                put32(nodes[i], 0, i + 1 < 4 ? reinterpret_cast<uint32_t>(nodes[i + 1]) : 0);
                put32(nodes[i], 0x10, reinterpret_cast<uint32_t>(items[i]));
                put16(items[i], 0x104, 21 + i);
            }
            put32(qctl, 0x14, reinterpret_cast<uint32_t>(nodes[0]));
            put32(qctl, 0x24, 4);
            put32(qctl, 0x30, 0);
            put16(qctl, 0x548, 0);
            put16(qmenu, 0x4C, 1);
            resume_game();
            wait_frames(3);
            phase(L,
                "h:poll()\n"
                "local i, s = h:info('query'), h:info('subwindo')\n"
                "check(i.cursor == 1 and i.top == 1 and #h:options('query').options == 4,"
                "  'info(query): four options, the cursor on the first, the first shown: cursor 1, top 1: '"
                "  .. tostring(i.cursor) .. ' ' .. tostring(i.top))\n"
                "check(s.cursor == 4 and s.top == nil, 'info(subwindo): any other window\\'s cursor is its row, 4, and'"
                "  .. ' it has no top: ' .. tostring(s.cursor) .. ' ' .. tostring(s.top))\n"
                "qsteps = {}\n"
                "return finish()\n");
            const char step[] =
                "local i = h:info('query')\n"
                "local ev = {}\n"
                "for _, e in ipairs((h:poll())) do ev[#ev + 1] = e.event .. ':' .. e.name .. ':' .. tostring(e.row) end\n"
                "qsteps[#qsteps + 1] = tostring(i.cursor) .. '/' .. tostring(i.top) .. '/' .. table.concat(ev, ',')\n"
                "return finish()\n";
            int rows[4];
            for (int i = 0; i < 4; ++i) {
                pause_game();
                down(qctl, NULL);
                resume_game();
                wait_frames(3);
                rows[i] = get16(qmenu, 0x4C);
                phase(L, step);
            }
            check(rows[0] == 2 && rows[1] == 3 && rows[2] == 3 && rows[3] == 3 && get16(qctl, 0x30) == 3
                    && get16(qctl, 0x32) == 1,
                "the stand-in's Down moves query's row 2, 3, 3, as the game's setter does, and a fourth stops on the"
                " last option");
            phase(L,
                "check(table.concat(qsteps, ' ', 1, 3) == '2/1/cursor:query:2 3/1/cursor:query:3 4/2/cursor:query:4',"
                "  'Down three times: info(query) cursor 2, 3, 4 and top 1, 1, 2, and cursor{query} events with rows 2, 3,'"
                "  .. ' 4, the option under the cursor though the row stays 3: ' .. table.concat(qsteps, ' ', 1, 3))\n"
                "check(qsteps[4] == '4/2/', 'a fourth Down on the last option moves nothing and posts nothing: '"
                "  .. tostring(qsteps[4]))\n"
                "local o = h:options('query').options\n"
                "check(o[h:info('query').cursor].value == 24, 'options(query).options[info(query).cursor] is the fourth,'"
                "  .. ' value 24')\n"
                "return finish()\n");
            pause_game();
            const uint32_t took = confirm(qctl, NULL) & 0xFF;
            resume_game();
            check(took == 1 && get16(qctl, 0x548) == 24,
                "the stand-in's confirm, as the game's reads +0x30, answers with the fourth option's value, 24");
            // 0.7.4: answer writes what that confirm writes, the option's index
            // at +0x30 and its value, the first option shown moved as the
            // cursor setter moves it; the row and query's input untouched
            pause_game();
            put16(qctl, 0x548, 0);
            memset(fl, 0, sizeof(*fl));
            resume_game();
            phase(L,
                "h:poll()\n"
                "check(h:answer('query', 21) == true, 'answer(query, 21) from the fourth option queues')\n"
                "return finish()\n");
            wait_frames(3);
            const bool written = get16(qctl, 0x30) == 0 && get16(qctl, 0x32) == 0 && get16(qctl, 0x548) == 21
                && get16(qmenu, 0x4C) == 3 && fl->query_event == 0 && fl->count == 0;
            pause_game();
            put16(qctl, 0x548, 0);
            const uint32_t again = confirm(qctl, NULL) & 0xFF;
            resume_game();
            check(written && again == 1 && get16(qctl, 0x548) == 21,
                "answer query 21: +0x30 = 0 and +0x32 = 0 (the first option now shown), 21 written, the row (+0x4C)"
                " left at 3, no stand-in routine called; the confirm reading that +0x30 takes 21 too");
            phase(L,
                "local i = h:info('query')\n"
                "local ev = {}\n"
                "for _, e in ipairs((h:poll())) do ev[#ev + 1] = e.event .. ':' .. e.name .. ':' .. tostring(e.row) end\n"
                "check(i.cursor == 1 and i.top == 1 and table.concat(ev, ',') == 'cursor:query:1',"
                "  'after answer(query, 21): info(query) cursor 1, top 1, and cursor{query, 1}: ' .. table.concat(ev, ','))\n"
                "return finish()\n");
            pause_game();
            put32(qctl, 0x14, head);
            put32(qctl, 0x24, count);
            put32(qctl, 0x30, 0);
            put16(qctl, 0x548, word);
            put16(qmenu, 0x4C, 1);
            resume_game();
            wait_frames(3);
            phase(L,
                "h:poll()\n"
                "return finish()\n");
        }
        // 0.7.3: arealist on the game's own layout (fake_arealist.h): every
        // row read with its kind, and the cursor as the row under it. 0.7.4:
        // the replies end an NPC event's choice (modes 1 and 2) as the game's
        // own Enter on a zone row and its cancel at the top do, open or
        // blocked, and never reach the list's input routine; the player's
        // keys, the input routine called here as the game would, drill in
        // and out.
        {
            typedef void (__cdecl* PrepareFn)(int mode, int grouped);
            typedef void (__cdecl* OpenedFn)();
            typedef void (__fastcall* SelectFn)(void* ctl, void* edx, int index, int row);
            typedef int (__fastcall* CloseFn)(void* mcb, void* edx, const char* key);
            typedef uint32_t (__fastcall* InputFn)(void* ctl, void* edx, int event, int row);
            PrepareFn prepare;
            OpenedFn opened;
            SelectFn select;
            CloseFn close_by_name;
            memcpy(&prepare, &g_fake->area_prepare, sizeof(prepare));
            memcpy(&opened, &g_fake->area_opened, sizeof(opened));
            memcpy(&select, &g_fake->area_select, sizeof(select));
            memcpy(&close_by_name, &g_fake->close, sizeof(close_by_name));
            AreaLog* al = g_fake->area_log;
            const int row_area = row_named("arealist");
            // An open of the list as an event or the search makes it: the
            // controller ready, the open by name, the cursor on the first row.
            auto open_list = [&](int mode, int grouped) {
                pause_game();
                prepare(mode, grouped);
                open_here("menu    arealist");
                opened();
                resume_game();
                wait_frames(3);
            };
            auto select_row = [&](int index) {
                pause_game();
                select(actl, NULL, index, 0);
                resume_game();
                wait_frames(3);
            };
            // The player's Enter (5) or cancel (6) on the row under the cursor,
            // after moving the selection to row `index` when it is not -1: one
            // step of the game thread.
            auto key = [&](int event, int index) {
                pause_game();
                if (index >= 0) {
                    select(actl, NULL, index, 0);
                }
                const void* const* vt;
                memcpy(&vt, actl, 4);
                InputFn input;
                memcpy(&input, &vt[0x18 / 4], sizeof(input));
                input(actl, NULL, event, 0);
                resume_game();
                wait_frames(3);
            };
            // closes as the game's run: the staged close, the instance freed a
            // frame later
            *g_fake->close_frames = 1;
            memset(al, 0, sizeof(*al));
            memset(fl, 0, sizeof(*fl));
            open_list(0, 1);
            const uint32_t top_rows = get32(actl, 0x60);
            phase(L,
                "h:poll()\n"
                "local a = h:options('arealist')\n"
                "local rows = a and a.rows or {}\n"
                "aid = a and a.id\n"
                "check(a and a.name == 'arealist' and type(a.id) == 'number' and a.mode == 0 and a.level == 0"
                "  and #rows == 45 and a.zones == nil and a.pending == true,"
                "  'options(arealist): the search\\'s top level, mode 0 level 0, all 45 rows, pending, named with its'"
                "  .. ' id: ' .. tostring(a and #rows))\n"
                "local r1, r2, r3 = rows[1] or {}, rows[2] or {}, rows[3] or {}\n"
                "check(r1.text == 'Current Area' and r1.id == 4 and r1.kind == 'current_area' and r1.label == 'here'"
                "  and r2.text == 'Current Region' and r2.id == -1 and r2.kind == 'current_region' and r2.label == 'nearby'"
                "  and r3.text == 'All Areas' and r3.id == 0 and r3.kind == 'all' and r3.label == 'everywhere'"
                "  and r1.count == nil and r3.count == nil,"
                "  'the three rows it opens on: Current Area (the zone, 4), Current Region (-1) and All Areas (0), each'"
                "  .. ' with the text beside it as label')\n"
                "local regions, zones, ids = 0, 0, true\n"
                "for i = 4, #rows do\n"
                "  local r = rows[i]\n"
                "  if r.kind == 'region' and r.label == nil then regions = regions + 1; zones = zones + (r.count or 0) end\n"
                "  ids = ids and r.id == 2 - i\n"
                "end\n"
                "check(regions == 42 and zones == 84 and ids and rows[4].text == 'Region 01' and rows[4].count == 1"
                "  and rows[6].id == -4 and rows[6].count == 3 and rows[45].text == 'Region 42' and rows[45].id == -43,"
                "  'the 42 region rows, ids -2 .. -43, each with the count of its zones (84 in all): ' .. regions .. ' '"
                "  .. zones)\n"
                "local i = h:info('arealist')\n"
                "check(i.cursor == 1 and i.top == 1, 'info(arealist): the cursor on row 1, row 1 the first shown: '"
                "  .. tostring(i.cursor) .. ' ' .. tostring(i.top))\n"
                "return finish()\n");
            const char area_step[] =
                "local i = h:info('arealist')\n"
                "local ev = {}\n"
                "for _, e in ipairs((h:poll())) do\n"
                "  if e.event == 'cursor' then ev[#ev + 1] = e.name .. ':' .. tostring(e.row) end\n"
                "end\n"
                "asteps = (asteps or '') .. tostring(i.cursor) .. '/' .. tostring(i.top) .. '/' .. table.concat(ev, ',') .. ' '\n"
                "return finish()\n";
            select_row(23);
            phase(L, area_step);
            select_row(44);
            phase(L, area_step);
            phase(L,
                "check(asteps == '24/24/arealist:24 45/36/arealist:45 ',"
                "  'the selection on row 24, then on the last: info(arealist) cursor 24 and 45, top 24 and 36 (ten rows'"
                "  .. ' shown), and cursor{arealist} with those rows: ' .. tostring(asteps))\n"
                "check(h:options('arealist').rows[h:info('arealist').cursor].text == 'Region 42',"
                "  'options(arealist).rows[info(arealist).cursor] is the row under the cursor')\n"
                "local r, why = h:answer('arealist', 9999)\n"
                "local r2, why2 = h:answer('arealist', 4)\n"
                "local c, cwhy = h:cancel('arealist')\n"
                "check(r == nil and why == \"the area list is not asking anything; the player's keys drive it\""
                "  and r2 == nil and why2 == why and c == nil and cwhy == why,"
                "  'the search (mode 0): answer and cancel refused, whatever the id: ' .. tostring(why))\n"
                "return finish()\n");
            key(5, 5);
            check(get16(actl, 0x58) == 4 && get32(actl, 0x60) != top_rows && actl[0x6D] == 1 && al->inputs == 1
                    && al->searches == 0 && al->closes == 0 && live_menu(row_area) != NULL,
                "the player's Enter on region 03: level 4 on a new row array, the list still open and waiting");
            {
                HuEngineApi api;
                memset(&api, 0, sizeof(api));
                HuEngineHandle old = {-1, 0};
                const bool read = record_api(&api);
                if (read) {
                    api.handle_new("older", &old);
                }
                int zones3 = -1;
                int zones4 = -1;
                bool rows5 = false;
                if (read && old.slot >= 0) {
                    const HuEngineReply& o3 = raw_read(api.options, &old, "arealist");
                    zones3 = sets_field(o3, "zones") && !sets_field(o3, "rows") && !sets_field(o3, "id")
                        ? fields_named(o3, "zone") : -1;
                    const HuEngineReply& o4 = raw_read(api.options4, &old, "arealist");
                    zones4 = sets_field(o4, "zones") && sets_field(o4, "id") && !sets_field(o4, "kind")
                        ? fields_named(o4, "zone") : -1;
                    const HuEngineReply& o5 = raw_read(api.options5, &old, "arealist");
                    rows5 = sets_field(o5, "rows") && sets_field(o5, "level") && !sets_field(o5, "zones")
                        && fields_named(o5, "kind") == 3;
                    api.handle_release(&old);
                }
                check(zones3 == 3 && zones4 == 3 && rows5,
                    "engine abi 3's and 4's options('arealist') keep their shape, the zone rows alone as {zone} under"
                    " zones (abi 4's with the prompt's id); abi 5's has the rows");
            }
            phase(L,
                "local a = h:options('arealist')\n"
                "local rows = a and a.rows or {}\n"
                "local out = {}\n"
                "for _, r in ipairs(rows) do out[#out + 1] = r.text .. '=' .. r.id .. ':' .. r.kind .. ':' .. tostring(r.label) end\n"
                "check(a and a.level == 4 and a.mode == 0 and a.id == aid"
                "  and table.concat(out, ' ') == 'Zone 004=4:zone:Z4 Zone 005=5:zone:Z5 Zone 006=6:zone:Z6',"
                "  'the next options(arealist): level 4, region 03\\'s three zones, the same prompt: '"
                "  .. table.concat(out, ' '))\n"
                "local i = h:info('arealist')\n"
                "local ev = {}\n"
                "for _, e in ipairs((h:poll())) do if e.event == 'cursor' then ev[#ev + 1] = e.row end end\n"
                "check(i.cursor == 1 and i.top == 1 and table.concat(ev, ',') == '1',"
                "  'the cursor on the first zone: info cursor 1, top 1, cursor{arealist, 1}')\n"
                "return finish()\n");
            key(6, -1);
            check(get16(actl, 0x58) == 0 && actl[0x6D] == 1 && al->closes == 0 && live_menu(row_area) != NULL,
                "the player's cancel inside region 03 backs out to the top, the list open");
            phase(L,
                "local a = h:options('arealist')\n"
                "local i = h:info('arealist')\n"
                "check(a.level == 0 and #a.rows == 45 and i.cursor == 6 and i.top == 6 and a.rows[i.cursor].id == -4,"
                "  'backed out: the top level\\'s 45 rows, the cursor on region 03 where it was: '"
                "  .. tostring(i.cursor) .. ' ' .. tostring(i.top))\n"
                "h:poll()\n"
                "return finish()\n");
            key(5, 5);
            phase(L,
                "h:poll()\n"
                "return finish()\n");
            key(5, 1);
            check(get16(actl, 0x6E) == 5 && actl[0x6D] == 0 && actl[0x6C] == 0 && al->searches == 1
                    && al->last_search == 5 && al->closes == 1 && al->latch_clears == 1
                    && strcmp(fl->closes, "arealist|scsibori|") == 0 && live_menu(row_area) == NULL,
                "the player's Enter on zone 5 in the search: 5 written, the search run, the list closed (its"
                " begin-close clearing the latch) and the search window after it");
            phase(L,
                "local out = {}\n"
                "for _, e in ipairs((h:poll())) do out[#out + 1] = e.event .. ':' .. e.name end\n"
                "local a, why = h:options('arealist')\n"
                "check(table.concat(out, ' ') == 'closed:arealist' and a == nil and why == 'no area choice is pending',"
                "  'closed{arealist}, and options(arealist) has nothing pending: ' .. table.concat(out, ' '))\n"
                "return finish()\n");

            // mode 1, an event's: the regions alone; a region's id and a zone
            // the level does not list refused; inside region 03, zone 5
            // answered: the result, close+reset, scsibori
            memset(al, 0, sizeof(*al));
            memset(fl, 0, sizeof(*fl));
            open_list(1, 1);
            phase(L,
                "h:poll()\n"
                "local a = h:options('arealist')\n"
                "check(a.mode == 1 and a.level == 0 and #a.rows == 42 and a.rows[1].kind == 'region'"
                "  and a.rows[1].id == -2 and a.id == aid + 1,"
                "  'mode 1: the 42 regions alone, a new prompt: ' .. #a.rows)\n"
                "local r, why = h:answer('arealist', -4)\n"
                "check(r == nil and why == \"the row with the id -4 is not a zone; the answer takes a zone row's id\","
                "  'answer(arealist, -4), a region, is refused: ' .. tostring(why))\n"
                "r, why = h:answer('arealist', 5)\n"
                "check(r == nil and why == \"no row of the list has the id 5 (options('arealist') lists them)\","
                "  'answer(arealist, 5), a zone the top level does not list, is refused: ' .. tostring(why))\n"
                "return finish()\n");
            key(5, 2);
            const int inputs = al->inputs;
            phase(L,
                "local a = h:options('arealist')\n"
                "local z = a.rows[2] or {}\n"
                "check(a.level == 4 and #a.rows == 3 and z.id == 5 and z.kind == 'zone' and z.label == nil and z.count == nil,"
                "  'mode 1 inside region 03: its zones, column 1 drawn from the zone id, so no label')\n"
                "check(h:answer('arealist', 5, a.id) == true, 'answer(arealist, 5) in mode 1 queues')\n"
                "return finish()\n");
            wait_frames(3);
            check(get16(actl, 0x6E) == 5 && actl[0x6D] == 0 && actl[0x6C] == 0 && al->searches == 0
                    && al->closes == 1 && al->latch_clears == 1 && al->inputs == inputs
                    && strcmp(fl->closes, "arealist|scsibori|") == 0 && live_menu(row_area) == NULL,
                "answer arealist 5 in mode 1: +0x6E = 5 for the event, close+reset, whose close reached the open"
                " instance's begin-close (latch and mode cleared), then scsibori; no search, no input");

            // mode 2: flat; cancel at the top leaves the open hook's -1
            memset(al, 0, sizeof(*al));
            memset(fl, 0, sizeof(*fl));
            open_list(2, 1);
            phase(L,
                "h:poll()\n"
                "local a = h:options('arealist')\n"
                "local zones = 0\n"
                "for _, r in ipairs(a.rows) do if r.kind == 'zone' then zones = zones + 1 end end\n"
                "check(a.mode == 2 and a.level == 0 and #a.rows == 84 and zones == 84, 'mode 2: every zone, flat')\n"
                "check(h:cancel('arealist') == true, 'cancel(arealist) at the top queues')\n"
                "return finish()\n");
            wait_frames(3);
            check(get16(actl, 0x6E) == -1 && actl[0x6D] == 0 && al->inputs == 0 && al->closes == 1
                    && al->latch_clears == 1 && al->scsibori == 0 && strcmp(fl->closes, "arealist|") == 0
                    && live_menu(row_area) == NULL,
                "cancel arealist at the top: close+reset, the latch cleared through the instance, the result left at"
                " -1, no search window closed, no input");
            phase(L,
                "h:poll()\n"
                "return finish()\n");

            // blocked: the event's opener arms the latch and the mode, the
            // open is refused, the result still holds an earlier answer; the
            // answer and the cancel end it with no window
            phase(L,
                "check(h:block('arealist') == true, 'block(arealist) is allowed')\n"
                "return finish()\n");
            wait_frames(3);
            auto blocked_event = [&](int mode) -> bool {
                pause_game();
                memset(al, 0, sizeof(*al));
                memset(fl, 0, sizeof(*fl));
                actl[0x6C] = static_cast<uint8_t>(mode);
                actl[0x6D] = 1;
                put16(actl, 0x6E, 231);
                const bool refused = open_here("menu    arealist") == NULL;
                resume_game();
                wait_frames(3);
                return refused;
            };
            const bool refused1 = blocked_event(1);
            phase(L,
                "local out = {}\n"
                "for _, e in ipairs((h:poll())) do out[#out + 1] = e.event .. ':' .. e.name end\n"
                "local a, awhy = h:options('arealist')\n"
                "local r, why = h:answer('arealist', 600)\n"
                "local keys = {}\n"
                "for k in pairs(a or {}) do keys[#keys + 1] = k end\n"
                "table.sort(keys)\n"
                "check(table.concat(out, ' ') == 'blocked:arealist' and a and a.name == 'arealist'"
                "  and type(a.id) == 'number' and a.mode == 1 and type(a.level) == 'number' and a.pending == true"
                "  and type(a.rows) == 'table' and #a.rows == 0 and table.concat(keys, ' ') == 'id level mode name pending rows'"
                "  and r == nil and why == 'the area list is blocked, so there are no rows to check 600 against; the'"
                "  .. ' answer takes a zone id, 0..511',"
                "  'blocked in mode 1: blocked{arealist}; options() is {name, id, mode, level, pending = true, rows ='"
                "  .. ' {}}, an answer awaited with no rows to read (' .. table.concat(keys, ' ') .. ', ' .. tostring(awhy)"
                "  .. '); answer 600 is no zone id: ' .. tostring(why))\n"
                "check(h:answer('arealist', 12, a.id) == true, 'answer(arealist, 12) with the list blocked queues, naming'"
                "  .. ' the prompt options() gave')\n"
                "return finish()\n");
            wait_frames(3);
            check(refused1 && get16(actl, 0x6E) == 12 && actl[0x6D] == 0 && actl[0x6C] == 0 && al->closes == 1
                    && al->latch_clears == 0 && strcmp(fl->closes, "arealist|scsibori|") == 0
                    && live_menu(row_area) == NULL && al->inputs == 0,
                "answer arealist 12, blocked in mode 1: 12 written, close+reset (its close by name finding no"
                " instance, so no begin-close), the game's latch clear called alone (latch and mode cleared), then"
                " scsibori");
            const bool refused2 = blocked_event(2);
            phase(L,
                "h:poll()\n"
                "local a = h:options('arealist')\n"
                "check(a and a.mode == 2 and a.pending == true and #a.rows == 0, 'blocked in mode 2: options() the same'"
                "  .. ' shape, mode 2')\n"
                "check(h:cancel('arealist', a.id) == true, 'cancel(arealist) with the list blocked queues, naming the'"
                "  .. ' prompt options() gave')\n"
                "return finish()\n");
            wait_frames(3);
            check(refused2 && get16(actl, 0x6E) == -1 && actl[0x6D] == 0 && actl[0x6C] == 0 && al->closes == 1
                    && al->latch_clears == 0 && strcmp(fl->closes, "arealist|") == 0,
                "cancel arealist, blocked in mode 2: -1 written over the earlier answer, close+reset, the latch clear"
                " alone, no scsibori");
            phase(L,
                "h:poll()\n"
                "check(h:unblock('arealist') == true, 'unblock(arealist)')\n"
                "return finish()\n");
            wait_frames(3);

            // a row with no entry: options() and an answer pass over it, as
            // the drain's check does, and take the rows after it
            memset(al, 0, sizeof(*al));
            memset(fl, 0, sizeof(*fl));
            open_list(1, 1);
            key(5, 2);
            pause_game();
            put32(reinterpret_cast<uint8_t*>(get32(actl, 0x60)), 0x48, 0);
            resume_game();
            phase(L,
                "h:poll()\n"
                "local a = h:options('arealist')\n"
                "local ids = {}\n"
                "for _, r in ipairs(a.rows) do ids[#ids + 1] = tostring(r.id) end\n"
                "check(a.level == 4 and table.concat(ids, ' ') == '5 6', 'region 03 with its first row\\'s entry gone:'"
                "  .. ' options() lists the two rows after it: ' .. table.concat(ids, ' '))\n"
                "check(h:answer('arealist', 6, a.id) == true, 'answer(arealist, 6), a zone past the row with no entry,'"
                "  .. ' queues')\n"
                "return finish()\n");
            wait_frames(3);
            check(get16(actl, 0x6E) == 6 && actl[0x6D] == 0 && al->closes == 1
                    && strcmp(fl->closes, "arealist|scsibori|") == 0 && live_menu(row_area) == NULL,
                "answer arealist 6 past a row with no entry: 6 written for the event, close+reset, then scsibori");

            // mode 4: another list; no reply
            open_list(4, 0);
            memset(al, 0, sizeof(*al));
            phase(L,
                "local out = {}\n"
                "for _, e in ipairs((h:poll())) do out[#out + 1] = e.event .. ':' .. e.name end\n"
                "local a = h:options('arealist')\n"
                "check(a.mode == 4 and #a.rows == 12 and a.rows[1].kind == 'other' and a.rows[12].id == 11,"
                "  'mode 4: its twelve rows, kind other')\n"
                "local r, why = h:answer('arealist', 3)\n"
                "local c, cwhy = h:cancel('arealist')\n"
                "check(r == nil and why == \"the area list is not asking anything; the player's keys drive it\""
                "  and c == nil and cwhy == why,"
                "  'mode 4: answer and cancel refused: ' .. tostring(why) .. ' / ' .. tostring(cwhy))\n"
                "return finish()\n");
            wait_frames(3);
            check(al->inputs == 0 && al->closes == 0 && actl[0x6D] == 1, "nothing reached mode 4's list");
            pause_game();
            close_by_name(g_fake->mcb, NULL, "menu    arealist");
            actl[0x6D] = 0;
            actl[0x6C] = 0;
            resume_game();
            wait_frames(3);
            *g_fake->close_frames = 0;
            phase(L,
                "h:poll()\n"
                "return finish()\n");
        }
        uint8_t* buff = static_cast<uint8_t*>(open_on_game_thread("menu    buff"));
        *g_fake->close_frames = 1;
        phase(L,
            "h:poll()\n"
            "check(h:block('buff') == true, 'block of the open buff queues')\n"
            "return finish()\n");
        wait_frames(3);
        *g_fake->close_frames = 0;
        const bool closed_by_block = buff && !live_menu(kRowBuff)
            && strcmp(g_fake->log->last_close, "menu    buff    ") == 0;
        phase(L,
            "local out = {}\n"
            "for _, e in ipairs((h:poll())) do out[#out + 1] = e.event .. ':' .. e.name end\n"
            "check(table.concat(out, ' ') == 'closed:buff' and h:info('buff').blocked and not h:info('buff').open,"
            "  'block of an open window closes it through the game\\'s close: closed{buff}, and it stays blocked: '"
            "  .. table.concat(out, ' '))\n"
            "check(h:unblock('buff') == true and h:block('buff') == true, 'buff unblocked and blocked again while closed')\n"
            "return finish()\n");
        check(closed_by_block, "the game's close by name closed buff");
        wait_frames(3);
        phase(L,
            "check(#(h:poll()) == 0 and h:unblock('buff') == true, 'a block of the closed buff has nothing to close')\n"
            "return finish()\n");
        wait_frames(3);
        {
            uint8_t* again = static_cast<uint8_t*>(open_on_game_thread("menu    buff"));
            HuEngineApi api;
            memset(&api, 0, sizeof(api));
            HuEngineHandle old = {-1, 0};
            char why[512] = "";
            const bool read = record_api(&api);
            if (read) {
                api.handle_new("abi4", &old);
            }
            const bool blocked = read && old.slot >= 0 && api.block3(&old, "buff", why, sizeof(why)) == 1;
            wait_frames(3);
            check(again && blocked && live_menu(kRowBuff) == again && again[0x6E] == 0,
                "engine abi 3's block, as a copy before 0.7.0 calls it, leaves the open buff open");
            if (read && old.slot >= 0) {
                api.unblock3(&old, "buff", why, sizeof(why));
                api.handle_release(&old);
            }
            put32(controller_named("buff"), 8, 0);
        }
        phase(L,
            "h:poll()\n"
            "check(h:block('menuwind') == true, 'menuwind blocked')\n"
            "return finish()\n");
        wait_frames(3);
        check(!live_menu(kRowMenuwind), "the block closed menuwind, open since the sizes");
        phase(L,
            "h:poll()\n"
            "return finish()\n");
        pause_game();
        for (int i = 0; i < 4200; ++i) {
            open_here("menu    menuwind");
        }
        resume_game();
        phase(L,
            "local list, dropped = h:poll()\n"
            "local last = list[#list]\n"
            "check(#list == 4097 and dropped >= 104 and last.event == 'resync' and last.dropped == dropped"
            "  and last.name == nil and list[1].event == 'blocked' and list[4096].event == 'blocked'"
            "  and #list[1].by == 1 and list[1].by[1] == 'waits' and list[1].mine == true,"
            "  'events lost: 4200 refused opens with 4096 kept, each blocked by waits (mine), then {event = resync,'"
            "  .. ' dropped} closing the list: '"
            "  .. tostring(dropped) .. ' dropped')\n"
            "list, dropped = h:poll()\n"
            "check(#list == 0 and dropped == 0, 'and no resync once nothing more is lost')\n"
            "check(h:release() == true, 'the waiting handle releases')\n"
            "local ok, why = hu.shutdown()\n"
            "check(ok == true, 'and the engine shuts down: ' .. tostring(why))\n"
            "return finish()\n");
        settle(L);
        check(origin(target, 0) == 100 + row_target && origin(target, 1) == 50 && origin(sub, 0) == sx
                && origin(sub, 1) == sy && registry_pristine(),
            "the release puts targetwi and subwindo back where the game had them; the registry is pristine");
        lua.close(L);
        check(engine_unmaps(), "the engine of the waiting group move unmaps");
    }

    // -- hideui.lua itself over copy A, Windower stubbed: per-window
    //    callbacks, layout() of a group move, a move and two sizes, reset_all,
    //    and apply() putting all of it back
    {
        const char* g[] = {"HIDEUI", argv[6], NULL};
        L = fresh(g);
        phase(L,
            "chat, handlers = {}, {}\n"
            "windower = {add_to_chat = function(color, text) chat[#chat + 1] = text end,"
            "  register_event = function(name, fn) handlers[name] = fn end,"
            "  from_shift_jis = function(text) return (text:gsub('\\130\\160', '\\227\\129\\130')) end}\n"
            "_addon = {name = 'layout'}\n"
            "hideui = dofile(HIDEUI)\n"
            "ui = hideui.new()\n"
            "check(ui ~= nil and ui.name == 'layout' and hideui.version() == 'hideui 0.11.0' and hideui.status().handles == 1,"
            "  'hideui.lua loads copy A beside it and makes a handle, named after _addon.name')\n"
            "seen = {}\n"
            "ui:on('opened', 'ability', function(e) seen[#seen + 1] = e.event .. ':' .. e.name end)\n"
            "check(ui:move_group('chat_log', 16, 844) == true and ui:move('equip', 333, 222) == true"
            "  and ui:resize('menuwind', 50, 40) == true and ui:resize('partywin', 2) == true,"
            "  'a group move to the frame top 16,844 (the log 120 tall on its default 134), a move and two sizes queue')\n"
            "local now = ui:layout()\n"
            "check(now.groups.chat_log.y == 844 and now.positions.equip.x == 333 and now.sizes.menuwind.w == 50"
            "  and now.ui.w == 1920 and now.ui.h == 1080,"
            "  'layout() has each of them at once, before any frame, with the UI size from status()')\n"
            "local q = ui:options('query') or {title = {}, options = {{segments = {}}}}\n"
            "local s1 = q.options[1].segments\n"
            "check(q.name == 'query' and q.title.text == 'Will you lend a hand?' and q.title.raw == q.title.text"
            "  and s1[1] and s1[1].color == 'default' and s1[2].color == 'green' and s1[2].text == 'gladly'"
            "  and s1[2].raw == 'gladly' and s1[2].escape == nil,"
            "  'options(query) through hideui.lua: the prompt\\'s name, the text with its bytes as raw, segments\\''"
            "  .. ' colors named')\n"
            "local r1, w1 = ui:answer(q, 2)\n"
            "local r2, w2 = ui:answer({name = 'query', id = q.id + 1}, 1)\n"
            "check(r1 == nil and tostring(w1):find('value 2', 1, true) and r2 == nil and w2 == 'that prompt is gone',"
            "  'answer(prompt, value) reaches the engine with the prompt\\'s name and id: ' .. tostring(w1) .. ' / '"
            "  .. tostring(w2))\n"
            "return finish()\n");
        open_on_game_thread("menu    ability");
        open_on_game_thread("menu    equip");
        open_on_game_thread("menu    menuwind");
        wait_frames(3);
        uint8_t* menuwind_now = live_menu(kRowMenuwind);
        check(origin(log, 1) == 830 && origin(party2, 1) == 830 && origin(equip, 0) == 333 && origin(equip, 1) == 222
                && menuwind_now && get16(menuwind_now, 0x3E) - get16(menuwind_now, 0x3A) == 50,
            "the group move carried partywin to 830 with the log; equip at 333,222; menuwind 50 wide");
        phase(L,
            "handlers.prerender()\n"
            "check(table.concat(seen, ' ') == 'opened:ability',"
            "  'on(opened, ability, fn) fires for ability and not for equip: ' .. table.concat(seen, ' '))\n"
            "layout = ui:layout()\n"
            "local g, p, z = layout.groups or {}, layout.positions or {}, layout.sizes or {}\n"
            "check(g.chat_log and g.chat_log.x == 16 and g.chat_log.y == 844 and g.party_list == nil"
            "  and p.equip and p.equip.x == 333 and p.equip.y == 222 and p.logwindo == nil and p.partywin == nil"
            "  and z.menuwind and z.menuwind.w == 50 and z.menuwind.h == 40 and z.partywin and z.partywin.rows == 2"
            "  and layout.ui.w == 1920,"
            "  'layout(): the group move as asked, 16,844, the windows it carried left out; the move; both sizes; the UI'"
            "  .. ' size')\n"
            "local plain = true\n"
            "local function walk(t)\n"
            "  for k, v in pairs(t) do\n"
            "    plain = plain and type(k) == 'string' and (type(v) == 'number' or type(v) == 'table')\n"
            "    if type(v) == 'table' then plain = plain and next(v) ~= nil; walk(v) end\n"
            "  end\n"
            "end\n"
            "walk(layout)\n"
            "check(plain, 'layout(): string keys, numbers and no empty table, as the config library saves them')\n"
            "check(ui:reset_all() == true, 'reset_all queues')\n"
            "return finish()\n");
        wait_frames(3);
        check(origin(log, 1) == 930 && origin(party2, 1) == 930 && origin(equip, 0) == 100 + kRowEquip
                && get16(menuwind_now, 0x3E) - get16(menuwind_now, 0x3A) == 120,
            "reset_all put the log, partywin, equip and menuwind's size back");
        phase(L,
            "check(ui:move_group('target_window', 1500, 600) == true and ui:move_group('party_list', 1700, 900) == true,"
            "  'move_group target_window, then party_list')\n"
            "local g = ui:layout().groups or {}\n"
            "check(g.party_list and g.party_list.x == 1700 and g.target_window == nil,"
            "  'layout(): the party list\\'s move carries the target_window group, as groups() lists its members, so'"
            "  .. ' that entry is the party list\\'s now')\n"
            "check(ui:move_group('target_window', 1500, 600) == true and ui:move_group('chat_log', 16, 844) == true,"
            "  'move_group target_window, then chat_log')\n"
            "g = ui:layout().groups or {}\n"
            "check(g.chat_log and g.party_list == nil and g.target_window == nil,"
            "  'layout(): the chat log\\'s move carries both other groups')\n"
            "local before = #ui.callbacks.opened\n"
            "local ok, err = pcall(ui.on, ui, 'opened', 'JobCSel', function() end)\n"
            "local added = ui.callbacks.opened[#ui.callbacks.opened]\n"
            "check(ok and #ui.callbacks.opened == before + 1 and added.name == 'jobcselu',"
            "  'on(opened, JobCSel): the second name taken, under the first name the engine\\'s info gives: '"
            "  .. tostring(err))\n"
            "ui:off('opened', 'jobcselu')\n"
            "check(#ui.callbacks.opened == before, 'off by the first name removes it')\n"
            "check(ui:move('mcr1edlo', 10, 20) == true and ui:move('mcr1edit', 30, 40) == true"
            "  and ui:resize('jobcsel', 300, 200) == true, 'a move by each of mcr1edit\\'s names, a size by jobcsel')\n"
            "local lay = ui:layout()\n"
            "check(lay.positions.mcr1edit.x == 30 and lay.positions.mcr1edlo == nil and lay.sizes.jobcselu.w == 300"
            "  and lay.sizes.jobcsel == nil, 'layout(): each window\\'s entries under its first name, the last move'"
            "  .. ' the one kept')\n"
            "check(ui:reset('mcr1edlo') == true and ui:layout().positions == nil,"
            "  'a reset by the second name forgets the entry under the first')\n"
            "check(ui:reset_all() == true, 'reset_all queues')\n"
            "return finish()\n");
        wait_frames(3);
        check(origin(log, 1) == 930 && origin(party2, 1) == 930,
            "reset_all put the log and partywin back after the group moves");
        phase(L,
            "ui:off('opened', 'ability')\n"
            "lists = {}\n"
            "ui:on('opened', 'ability', 'pet_commands', function(e) lists[#lists + 1] = 'pets:' .. tostring(e.category) end)\n"
            "ui:on('opened', 'ability', '1', function(e) lists[#lists + 1] = 'one:' .. tostring(e.category_name) end)\n"
            "ui:on('blocked', 'ability', 'job_traits', function(e) lists[#lists + 1] = 'traits:' .. tostring(e.category)"
            "  .. ':' .. tostring(e.mine) end)\n"
            "local ok, err = pcall(ui.on, ui, 'opened', 'equip', 'pet_commands', function() end)\n"
            "check(not ok and tostring(err):find('category filters apply to ability only', 1, true) ~= nil,"
            "  'on(opened, equip, pet_commands, fn) raises: ' .. tostring(err))\n"
            "check(ui:block('ability', 'job_traits') == true, 'block(ability, job_traits) through hideui.lua')\n"
            "check(ui:close('ability') == true, 'close(ability), so that each list below is an open')\n"
            "return finish()\n");
        wait_frames(3);
        ability_open_on_game_thread(2, 0, 0);
        ability_open_on_game_thread(1, 0, 0);
        ability_open_on_game_thread(4, 0, 0);
        wait_frames(3);
        phase(L,
            "handlers.prerender()\n"
            "check(table.concat(lists, ' ') == 'pets:2 one:job_abilities traits:4:true',"
            "  'on(event, ability, category, fn) fires for its list alone, by name or number, blocked too: '"
            "  .. table.concat(lists, ' '))\n"
            "check(ui:unblock('ability', 'job_traits') == true and ui:close('ability') == true,"
            "  'unblock(ability, job_traits) and close through hideui.lua')\n"
            "return finish()\n");
        wait_frames(3);
        check(!live_menu(row_named("ability")), "ability closed again");
        phase(L,
            "check(ui:apply(layout) == true, 'apply(layout) queues every entry')\n"
            "return finish()\n");
        wait_frames(3);
        check(origin(log, 1) == 830 && origin(party2, 1) == 830 && origin(equip, 0) == 333 && origin(equip, 1) == 222
                && get16(menuwind_now, 0x3E) - get16(menuwind_now, 0x3A) == 50,
            "apply(layout): the group move to the frame top 844 carried partywin to 830 again; equip at 333,222;"
            " menuwind 50 wide");
        phase(L,
            "dbdel = {}\n"
            "ui:on('opened', 'dbdelsel', function(e) dbdel[#dbdel + 1] = e.event .. ':' .. e.name end)\n"
            "hideui.debug(true)\n"
            "unmatched = hideui.status().detail.unmatched_open_keys\n"
            "hideui.debug(false)\n"
            "check(ui:open('dbdelsel') == true and ui:hide('dbdelsel') == true,"
            "  'open and hide of dbdelsel, the registry\\'s tkdebug record, queue through hideui.lua')\n"
            "return finish()\n");
        wait_frames(3);
        uint8_t* dbdel = live_menu(kRowDbdelsel);
        check(dbdel && strcmp(g_fake->log->last_open, "tkdebug dbdelsel") == 0
                && hidden_in_place(kRowDbdelsel, dbdel),
            "open('dbdelsel') reached the open routine as \"tkdebug dbdelsel\"; its draw paints nothing, row 327"
            " and the live menu at their layer");
        phase(L,
            "handlers.prerender()\n"
            "local plain_status, plain_info, plain_list = hideui.status(), ui:info('dbdelsel'), ui:list().dbdelsel\n"
            "check(plain_status.detail == nil and plain_info.detail == nil and plain_list.detail == nil,"
            "  'with debug off, status, info and list carry no detail')\n"
            "hideui.debug(true)\n"
            "local i = ui:info('dbdelsel')\n"
            "local l = ui:list().dbdelsel\n"
            "check(table.concat(dbdel, ' ') == 'opened:dbdelsel'"
            "  and hideui.status().detail.unmatched_open_keys == unmatched,"
            "  'the open hook mapped \"tkdebug dbdelsel\" back to its name: opened{dbdelsel}, no unmatched key: '"
            "  .. table.concat(dbdel, ' '))\n"
            "check(i and i.open and i.hidden and i.layer == 2 and i.detail.rows[1] == 327 and l and l.layer == 2"
            "  and l.detail.avail == -1 and l.detail.overlap == -1,"
            "  'info and list report dbdelsel: open, hidden, layer 2, row 327, avail and overlap -1 as the table has them')\n"
            "hideui.debug(false)\n"
            "check(ui:unhide('dbdelsel') == true and ui:close('dbdelsel') == true, 'unhide and close of dbdelsel queue')\n"
            "return finish()\n");
        wait_frames(3);
        check(!live_menu(kRowDbdelsel) && strcmp(g_fake->log->last_close, "tkdebug dbdelsel") == 0
                && memcmp(registry_row(kRowDbdelsel), g_fake->pristine + kRowDbdelsel * 0x2C, 0x2C) == 0,
            "unhide put row 327's draw layer back, the row byte-identical to the table; close('dbdelsel') reached"
            " the game's close as \"tkdebug dbdelsel\"");
        phase(L,
            "local list = ui:settings()\n"
            "local by = {}\n"
            "for _, e in ipairs(list) do by[e.name] = e end\n"
            "local fx = by.effect_filter_you_to_you\n"
            "check(#list == 268 and fx.label == 'You \\227\\129\\130 you' and fx.label_raw == 'You \\130\\160 you'"
            "  and by.music_volume.label == nil and by.music_volume.label_raw == nil,"
            "  'settings() through hideui.lua: each label as UTF-8, its bytes in label_raw')\n"
            "check(ui:setting('shadows') == by.shadows.value, 'setting() through hideui.lua')\n"
            "local ok, e = pcall(ui.set_setting, ui, 'music_volume', 'loud')\n"
            "check(not ok and tostring(e):find('music_volume takes a whole number', 1, true),"
            "  'set_setting of a value of the wrong type raises through hideui.lua: ' .. tostring(e))\n"
            "ok, e = pcall(ui.setting, ui, 42)\n"
            "check(not ok and tostring(e):find('setting(name): name must be a string', 1, true),"
            "  'setting(42) raises: ' .. tostring(e))\n"
            "ok, e = pcall(ui.set_setting, 'music_volume', 1)\n"
            "check(not ok and tostring(e):find('use ui:set_setting(...)', 1, true), 'ui.set_setting(...) raises: '"
            "  .. tostring(e))\n"
            "local done, why = ui:set_setting('music_volume', 101)\n"
            "check(done == nil and why == 'music_volume takes a whole number from 0 to 100',"
            "  'a value out of range comes back as nil and the reason: ' .. tostring(why))\n"
            "return finish()\n");
        phase(L,
            "local again = ui:layout()\n"
            "local function same(a, b)\n"
            "  for k, v in pairs(a) do\n"
            "    if type(v) == 'table' then if type(b[k]) ~= 'table' or not same(v, b[k]) then return false end\n"
            "    elseif b[k] ~= v then return false end\n"
            "  end\n"
            "  for k in pairs(b) do if a[k] == nil then return false end end\n"
            "  return true\n"
            "end\n"
            "check(same(layout, again), 'layout() after apply() is the layout applied')\n"
            "local r, why, list = ui:apply({positions = {nosuch = {x = 1, y = 2}}, groups = {chat_log = {x = 16, y = 844}}})\n"
            "check(r == nil and why == 'move nosuch: no such window: nosuch' and #list == 1 and list[1].verb == 'move'"
            "  and list[1].name == 'nosuch' and list[1].reason == 'no such window: nosuch',"
            "  'apply(): a refused entry is reported with what it was, as a line and as {verb, name, reason}, the others'"
            "  .. ' applied: ' .. tostring(why))\n"
            "check(ui:layout().positions.nosuch.x == 1, 'layout(): the entry apply() could not apply stays in it')\n"
            "local wide = {ui = {w = 2560, h = 1440}, positions = {equip = {x = 1, y = 1}}}\n"
            "local rw, why_wide = ui:apply(wide)\n"
            "check(rw == nil and why_wide == 'this layout was saved at 2560x1440; the game is at 1920x1080'"
            "  and ui:layout().positions.equip.x == 333, 'apply() of a layout saved at another UI size applies nothing: '"
            "  .. tostring(why_wide))\n"
            "local ok, err = pcall(ui.apply, ui, {groups = {chat_log = {x = 'a'}}})\n"
            "check(not ok and tostring(err):find('groups.chat_log needs numbers x and y', 1, true),"
            "  'apply(): a malformed entry raises: ' .. tostring(err))\n"
            "local line = debug.getinfo(1, 'l').currentline; ok, err = pcall(function() ui:move('equip', 'x', 1) end)\n"
            "check(not ok and tostring(err):find(':' .. line .. ': move(name, x, y): x and y must be numbers', 1, true),"
            "  'hideui.lua raises misuse at the addon\\'s line: ' .. tostring(err))\n"
            "handlers.unload()\n"
            "check(#chat == 0, 'hideui.lua printed nothing')\n"
            "local snap = ui:layout()\n"
            "local kept = snap and snap.positions and snap.positions.nosuch\n"
            "if kept then snap.positions.nosuch = nil end\n"
            "check(kept and kept.x == 1 and same(again, snap) and ui:info('equip') == nil,"
            "  'after unload, the released handle\\'s layout() is the one it held at its release, for the addon\\'s own'"
            "  .. ' unload handler to save')\n"
            "return finish()\n");
        settle(L, "hideui");
        check(record_magic() == 0 && registry_pristine() && origin(log, 1) == 930 && origin(equip, 0) == 100 + kRowEquip,
            "unload through hideui.lua released the handle and shut the engine down: everything back, no resident");
        lua.close(L);
        check(engine_unmaps(), "the engine hideui.lua loaded unmaps");
    }

    // -- several addons: copy A from build/a and copy B from build/b, one
    //    basename in two folders; copy C from build/c, which publishes engine
    //    abi 2's table and stands for an older resident
    const char* const dll_a = argv[2];
    const char* const dll_b = argv[4];
    const char* const dll_c = argv[5];
    const char* const dll_d = argv[7];
    const char* const dll_e = argv[8];
    const char* const dll_f = argv[9];
    const char* const dll_g = argv[10];
    const char* const dll_h = argv[11];
    const char* const dll_i = argv[12];
    const char* const dll_j = argv[13];
    char daemon_b[MAX_PATH];
    char daemon_c[MAX_PATH];
    beside(dll_b, "hideui_daemon.dll", daemon_b, sizeof(daemon_b));
    beside(dll_c, "hideui_daemon.dll", daemon_c, sizeof(daemon_c));
    check(record_magic() == 0, "no resident is published before the several-addons phase");
    // buff, open before any handle of this phase: a hide of it shows in the
    // walk, and no handle sees its open
    uint8_t* buff_open = static_cast<uint8_t*>(open_on_game_thread("menu    buff"));

    lua_State* LA = fresh_from(dll_a, globals);
    phase(LA,
        "hu = load_engine()\n"
        "ha = hu.new('addon_a')\n"
        "local s = hu.status()\n"
        "local d = s.detail\n"
        "check(s.ok and s.role == 'resident' and copy_of(d.image.path) == 'a' and d.resident"
        "  and copy_of(d.resident.path) == 'a' and d.resident.abi == 9 and d.resident.build == s.engine,"
        "  'copy A installs and is resident: ' .. tostring(d.error))\n"
        "check(ha:hide('buff') == true and ha:block('menuwind') == true and ha:move('equip', 300, 200) == true,"
        "  'A hides buff, blocks menuwind and moves equip')\n"
        "return finish()\n");
    wait_frames(3);
    check(hidden_in_place(kRowBuff, buff_open)
            && memcmp(registry_row(kRowMenuwind), g_fake->pristine + kRowMenuwind * 0x2C, 0x2C) == 0
            && origin(equip, 0) == 300 && origin(equip, 1) == 200,
        "A's hide, block and move reach the game; the block of the closed menuwind leaves its row as it is");
    check(record_magic() == HU_ENGINE_MAGIC, "A published the resident record");

    lua_State* LB = fresh_from(dll_b, globals);
    phase(LB,
        "hu = load_engine()\n"
        "hb = hu.new('addon_b')\n"
        "local s = hu.status()\n"
        "local d = s.detail\n"
        "check(s.ok and s.role == 'forwarder' and copy_of(d.image.path) == 'b' and d.resident"
        "  and copy_of(d.resident.path) == 'a' and d.resident.build == s.engine,"
        "  'copy B forwards; its status names A as resident: ' .. tostring(d.resident and d.resident.path))\n"
        "check(s.handles == 2, 'one handle table in the client: B counts the handle of A and its own')\n"
        "check(hu.version() == 'hideui 0.11.0', 'the version of B is its own: ' .. hu.version())\n"
        "check(hb:hide('buff') == true and hb:move('equip', 10, 20) == true, 'B hides buff and moves equip after A')\n"
        "return finish()\n");
    check(!mapped(daemon_b), "B loaded no daemon: it installs nothing of its own");
    wait_frames(3);
    check(origin(equip, 0) == 10 && origin(equip, 1) == 20, "a move by B wins over the one by A");
    open_on_game_thread("menu    ability");
    phase(LB,
        "local list, dropped = hb:poll()\n"
        "local seen = {}\n"
        "for _, e in ipairs(list) do\n"
        "  seen[#seen + 1] = e.event .. ':' .. e.name\n"
        "end\n"
        "check(table.concat(seen, ' ') == 'opened:ability' and dropped == 0,"
        "  'B polls the events the resident posts: ' .. table.concat(seen, ' '))\n"
        "local pos = hb:remembered()\n"
        "local e = pos.equip and pos.equip.position or {}\n"
        "check(e.x == 10 and e.y == 20 and e.owner == 'addon_b' and e.mine, 'remembered() through B: equip is remembered for B')\n"
        "local p = hb:info('equip')\n"
        "check(p and p.open and p.origin.x == 10 and p.origin.y == 20 and p.memory and p.memory.position.mine,"
        "  'info() through B reads the live menu')\n"
        "local n = 0\n"
        "for _ in pairs(hb:list()) do n = n + 1 end\n"
        "check(n == 368, 'list() through B: all ' .. n .. ' names, the compass and the target arrows among them')\n"
        "local q = hb:options('query')\n"
        "local s = q and q.options[1] and q.options[1].segments or {}\n"
        "check(q and q.title.text == 'Will you lend a hand?' and #q.options == 3 and q.options[2].value == 4"
        "  and s[2] and s[2].text == 'gladly' and s[2].color == 2 and type(q.id) == 'number',"
        "  'options(query) through B, an engine abi 5 slot, segments and id included')\n"
        "return finish()\n");

    phase(LA,
        "check(ha:release() == true, 'A releases its handle')\n"
        "local ok, why, kind = hu.shutdown()\n"
        "check(ok == false and kind == 'handles', 'A refuses to shut down while B holds a handle: ' .. tostring(why))\n"
        "return finish()\n");
    wait_frames(3);
    check(hidden_in_place(kRowBuff, buff_open), "a hide held by B survives the release of A's handle");
    check(memcmp(registry_row(kRowMenuwind), g_fake->pristine + kRowMenuwind * 0x2C, 0x2C) == 0,
        "menuwind's registry row as the table has it");
    check(origin(equip, 0) == 10 && origin(equip, 1) == 20, "B's move survives A's release: B wrote it last");
    lua.close(LA);
    check(mapped(dll_a), "A's Lua state is closed; A stays mapped and pinned while B holds a handle");
    phase(LB,
        "check(not hb:info('menuwind').blocked and not hb:info('menuwind').hidden,"
        "  'the block held by A alone went with it')\n"
        "check(hb:hide('logwindo') == true, 'B keeps working through A after A unloaded')\n"
        "local s = hu.status()\n"
        "check(s.ok and s.handles == 1 and copy_of(s.detail.resident.path) == 'a', 'A still serves, with one handle open')\n"
        "return finish()\n");
    wait_frames(3);
    check(hidden_in_place(kRowLog, live_menu(kRowLog)), "B's hide of logwindo reached the game through A");

    lua_State* LC = fresh_from(dll_c, globals);
    phase(LC,
        "hu = load_engine()\n"
        "hc = hu.new('addon_c')\n"
        "local s = hu.status()\n"
        "check(hc ~= nil and s.ok and s.role == 'forwarder' and s.detail.image.abi == 2"
        "  and copy_of(s.detail.resident.path) == 'a' and s.handles == 2,"
        "  'copy C, built with engine abi 2\\'s table, forwards to A all the same: no copy refuses another')\n"
        "check(hc:hide('ability') == true and hc:release() == true, 'C hides ability through A and releases')\n"
        "local ok, why, kind = hu.shutdown()\n"
        "check(ok == false and kind == 'handles', 'the shutdown of C reaches A, which refuses while B holds a handle')\n"
        "return finish()\n");
    lua.close(LC);
    check(!mapped(dll_c) && !mapped(daemon_c), "C installed nothing, loaded no daemon and holds no pin");
    phase(LB,
        "local s = hu.status()\n"
        "check(s.ok and s.handles == 1 and s.role == 'forwarder', 'A still serves B after C left')\n"
        "return finish()\n");

    phase(LB,
        "check(hb:release() == true, 'B releases its handle')\n"
        "local ok, why = hu.shutdown()\n"
        "check(ok == true, 'with every handle released, the shutdown of B reaches A and succeeds: ' .. tostring(why))\n"
        "return finish()\n");
    settle(LB);
    phase(LB,
        "local s = hu.status()\n"
        "check(s.role == 'none' and s.state == 'idle' and not s.detail.resident, 'afterwards B finds no resident')\n"
        "return finish()\n");
    check(record_magic() == 0, "the resident record is cleared");
    check(registry_pristine(), "and the registry is byte-identical to the expected table");
    check(mapped(dll_a), "A stays mapped while B, which forwarded to it, is loaded");
    lua.close(LB);
    check(unmaps(dll_a) && unmaps(dll_b), "with both Lua states closed, neither copy stays mapped");

    LB = fresh_from(dll_b, globals);
    phase(LB,
        "hu = load_engine()\n"
        "hb = hu.new('addon_b')\n"
        "local s = hu.status()\n"
        "check(s.ok and s.role == 'resident' and copy_of(s.detail.resident.path) == 'b',"
        "  'a fresh load with B first makes B resident: ' .. tostring(s.detail.error))\n"
        "check(hb:hide('buff') == true, 'B hides buff')\n"
        "return finish()\n");
    check(record_magic() == HU_ENGINE_MAGIC && mapped(daemon_b), "B published the record and acquired the daemon");
    LA = fresh_from(dll_a, globals);
    phase(LA,
        "hu = load_engine()\n"
        "ha = hu.new('addon_a')\n"
        "local s = hu.status()\n"
        "check(s.ok and s.role == 'forwarder' and copy_of(s.detail.resident.path) == 'b' and s.handles == 2,"
        "  'A, loaded second, forwards to B')\n"
        "check(ha:hide('logwindo') == true and ha:move('ability', 400, 300) == true,"
        "  'A hides logwindo and moves ability through B')\n"
        "return finish()\n");
    wait_frames(3);
    check(hidden_in_place(kRowBuff, buff_open) && hidden_in_place(kRowLog, live_menu(kRowLog)),
        "both hides reach the game through B");

    // -- the slots of engine abi 1 and 2, called as a copy built against them
    //    calls them: their meaning is the one that copy was built for
    {
        HuEngineApi api;
        memset(&api, 0, sizeof(api));
        const bool read = record_api(&api);
        HuEngineHandle old = {-1, 0};
        char why[512] = "";
        if (read) {
            api.handle_new("older", &old);
        }
        const int party_y = origin(party2, 1);
        const bool group_name = read && old.slot >= 0 && api.hide(&old, "target_window", why, sizeof(why)) == 1;
        const bool delta = read && api.move_group(&old, "party_list", 0, -5, why, sizeof(why)) == 1;
        wait_frames(3);
        const bool hidden = read && field_true(raw_read(api.info3, &old, "targetwi"), "hidden");
        check(read && api.size == sizeof(HuEngineApi) && api.abi_version == HU_ENGINE_ABI && group_name && hidden
                && registry_row(row_named("targetwi"))[0x29] == 2 && delta && origin(party2, 1) == party_y - 5,
            "B's table, read out of the record: hide('target_window') through abi 2's slot hides its anchor"
            " targetwi, which keeps its layer, and move_group through it still shifts by dx, dy");
        HuEngineEvent events[16];
        uint32_t count = 0;
        uint32_t lost = 0;
        if (read) {
            // equip closed first, since an open of an open window posts nothing
            api.close(&old, "equip", why, sizeof(why));
            wait_frames(3);
            api.poll(&old, events, 16, &count, &lost, why, sizeof(why));
            open_on_game_thread("menu    equip");
            api.poll(&old, events, 16, &count, &lost, why, sizeof(why));
        }
        check(read && count == 1 && strcmp(events[0].event, "show") == 0 && strcmp(events[0].name, "equip") == 0
                && events[0].fresh == 1,
            "and abi 1's poll reports an open as show{new=true}, as it always did");
        // engine abi 3's slots, as a 0.5.0 copy calls them: 0.5.0's shapes
        // and rules
        put32(g_fake->mcb, 0x54, 0);
        bool focused_nil = false;
        bool info_shape = false;
        bool options_shape = false;
        bool pending_shape = false;
        bool close_prompt = false;
        if (read) {
            const HuEngineReply& f = raw_read(api.focused, &old);
            focused_nil = f.used == 1 && f.data[0] == HU_REPLY_NIL;
            const HuEngineReply& i = raw_read(api.info3, &old, "logwindo");
            info_shape = sets_field(i, "hide_only") && sets_field(i, "sizes") && !sets_field(i, "blockable")
                && !sets_field(i, "resize") && !sets_field(i, "hidden_by");
            const HuEngineReply& o = raw_read(api.options, &old, "query");
            options_shape = sets_field(o, "title_segments") && !sets_field(o, "id");
            const HuEngineReply& q = raw_read(api.pending, &old);
            pending_shape = sets_field(q, "state") && !sets_field(q, "detail");
            close_prompt = api.close3(&old, "passinpu", why, sizeof(why)) == 1;
        }
        check(focused_nil && info_shape && options_shape && pending_shape && close_prompt,
            "and engine abi 3's slots keep 0.5.0's meaning: focused() nil with nothing focused, info() with hide_only"
            " and sizes, options('query') with title_segments and no id, pending() with the state beside the box,"
            " close('passinpu') queued");
        // engine abi 4's, as a 0.5.1 to 0.6.3 copy calls them: 0.6.3's shapes
        bool options4_shape = false;
        bool pending4_shape = false;
        bool options5_shape = false;
        bool pending5_shape = false;
        if (read) {
            g_fake->party_pending[0] = 1;
            wait_frames(2);
            const HuEngineReply& o4 = raw_read(api.options4, &old, "prtyjoin");
            options4_shape = sets_field(o4, "name") && !sets_field(o4, "inviter") && sets_field(o4, "id");
            const HuEngineReply& q4 = raw_read(api.pending4, &old);
            pending4_shape = sets_field(q4, "name") && !sets_field(q4, "inviter") && !sets_field(q4, "id");
            const HuEngineReply& o5 = raw_read(api.options5, &old, "prtyjoin");
            options5_shape = sets_field(o5, "inviter") && sets_field(o5, "name") && sets_field(o5, "id");
            const HuEngineReply& q5 = raw_read(api.pending5, &old);
            pending5_shape = sets_field(q5, "inviter") && sets_field(q5, "id");
            g_fake->party_pending[0] = 0;
            wait_frames(2);
        }
        check(options4_shape && pending4_shape && options5_shape && pending5_shape,
            "engine abi 4's options('prtyjoin') and pending() keep the inviter as name and pending() no id; abi 5's"
            " have inviter and id");
        uint32_t pending_seen = 0;
        uint32_t polled = 0;
        if (read) {
            HuEngineEvent3 three[16];
            api.poll3(&old, three, 16, &count, &lost, why, sizeof(why));
            g_fake->party_pending[0] = 1;
            wait_frames(3);
            api.poll3(&old, three, 16, &count, &lost, why, sizeof(why));
            polled = count;
            for (uint32_t k = 0; k < count && k < 16; ++k) {
                pending_seen += three[k].event[0] != '\0';
            }
            g_fake->party_pending[0] = 0;
            wait_frames(3);
        }
        check(read && polled == 1 && pending_seen == 0,
            "and engine abi 3's poll skips the pending event 0.5.1 posts when an invite arrives");
        if (read) {
            api.move_group(&old, "party_list", 0, 5, why, sizeof(why));
            api.handle_release(&old);
        }
        wait_frames(3);
        check(origin(party2, 1) == party_y && registry_row(row_named("targetwi"))[0x29] == 2,
            "the older handle's release takes its group move back");
    }
    lua.close(LA);
    wait_frames(3);
    phase(LB,
        "local s = hu.status()\n"
        "check(s.ok and s.handles == 1, 'A unloaded without a release: its handle went with its Lua state')\n"
        "local p = hb:info('ability')\n"
        "check(p.open and p.origin.x == p.default.x and p.origin.y == p.default.y and not p.memory,"
        "  'and the window A moved is back at its default')\n"
        "check(not hb:info('logwindo').hidden and hb:info('buff').hidden and not hb:info('targetwi').hidden,"
        "  'the hide of A is gone, the one of B stays, the older handle\\'s of targetwi gone with it')\n"
        "check(hb:release() == true and hu.shutdown() == true, 'B releases and shuts down as the resident')\n"
        "return finish()\n");
    settle(LB);
    check(record_magic() == 0 && registry_pristine(), "the record is cleared and the registry pristine again");
    check(walk_of(buff_open) == 'd' && buff_open[0x77] == 1 && walk_of(live_menu(kRowLog)) == 'd',
        "the shutdown gives every hidden window back: buff and logwindo drawn, buff's +0x77 1");
    {
        typedef int (__fastcall* CloseFn)(void* mcb, void* edx, const char* key);
        CloseFn close_by_name;
        memcpy(&close_by_name, &g_fake->close, sizeof(close_by_name));
        pause_game();
        close_by_name(g_fake->mcb, NULL, "menu    buff    ");
        resume_game();
    }
    lua.close(LB);
    check(unmaps(dll_a) && unmaps(dll_b), "and neither copy stays mapped");

    // -- C resident: its table is engine abi 2's, as an older resident's is;
    //    A forwards to it, and a call whose slot is past C's table says so
    LC = fresh_from(dll_c, globals);
    phase(LC,
        "hu = load_engine()\n"
        "hc = hu.new('addon_c')\n"
        "local s = hu.status()\n"
        "check(hc ~= nil and s.ok and s.role == 'resident' and copy_of(s.detail.resident.path) == 'c'"
        "  and s.detail.resident.abi == 2, 'C installs and is resident, publishing engine abi 2\\'s table')\n"
        "return finish()\n");
    LA = fresh_from(dll_a, globals);
    phase(LA,
        "hu = load_engine()\n"
        "local h, why = hu.new('addon_a')\n"
        "local path = tostring(why):match('; the resident copy is (.+)$')\n"
        "check(h == nil and tostring(why):find('new needs hideui 0.11.0 or newer; the resident copy is ', 1, true) == 1"
        "  and copy_of(path) == 'c', 'new() through A to C: ' .. tostring(why))\n"
        "local s, why2 = hu.status()\n"
        "check(s == nil and tostring(why2):find('status needs hideui 0.5.1 or newer; the resident copy is ', 1, true) == 1,"
        "  'status() through A to C: ' .. tostring(why2))\n"
        "check(hu.version() == 'hideui 0.11.0', 'A\\'s version is its own')\n"
        "local ok, why3, kind = hu.shutdown()\n"
        "check(ok == false and kind == 'handles', 'A\\'s shutdown, an engine abi 1 slot, reaches C: ' .. tostring(why3))\n"
        "return finish()\n");
    lua.close(LA);
    phase(LC,
        "check(hc:hide('buff') == true and hc:release() == true and hu.shutdown() == true,"
        "  'C serves itself, releases and shuts down')\n"
        "return finish()\n");
    settle(LC);
    check(record_magic() == 0 && registry_pristine(), "the record is cleared and the registry pristine");
    lua.close(LC);
    check(unmaps(dll_a) && unmaps(dll_c), "and neither copy stays mapped");

    // -- D resident: its table is engine abi 3's, as a 0.5.0 resident's is;
    //    a 0.5.1 copy makes no handle through it, and names the copy to update
    lua_State* LD = fresh_from(dll_d, globals);
    phase(LD,
        "hu = load_engine()\n"
        "hd = hu.new('addon_d')\n"
        "local s = hu.status()\n"
        "check(hd ~= nil and s.ok and s.role == 'resident' and copy_of(s.detail.resident.path) == 'd'"
        "  and s.detail.resident.abi == 3, 'D installs and is resident, publishing engine abi 3\\'s table')\n"
        "return finish()\n");
    LA = fresh_from(dll_a, globals);
    phase(LA,
        "hu = load_engine()\n"
        "local h, why = hu.new('addon_a')\n"
        "local path = tostring(why):match('; the resident copy is (.+)$')\n"
        "check(h == nil and tostring(why):find('new needs hideui 0.11.0 or newer; the resident copy is ', 1, true) == 1"
        "  and copy_of(path) == 'd', 'new() through A to D, a 0.5.0 resident: no handle at all: ' .. tostring(why))\n"
        "local ok, why2, kind = hu.shutdown()\n"
        "check(ok == false and kind == 'handles', 'A\\'s shutdown reaches D, which refuses while D holds a handle')\n"
        "return finish()\n");
    lua.close(LA);
    phase(LD,
        "check(hd:hide('buff') == true and hd:release() == true and hu.shutdown() == true,"
        "  'D serves itself, releases and shuts down')\n"
        "return finish()\n");
    settle(LD);
    check(record_magic() == 0 && registry_pristine(), "the record is cleared and the registry pristine");
    lua.close(LD);
    check(unmaps(dll_a) && unmaps(dll_d), "and neither copy stays mapped");

    // -- E resident: its table is engine abi 4's, as a 0.5.1 to 0.6.3
    //    resident's is; a 0.7.0 copy makes no handle through it, and names the
    //    copy to update, while the calls whose slots E has still reach it
    lua_State* LE = fresh_from(dll_e, globals);
    phase(LE,
        "hu = load_engine()\n"
        "he = hu.new('addon_e')\n"
        "local s = hu.status()\n"
        "check(he ~= nil and s.ok and s.role == 'resident' and copy_of(s.detail.resident.path) == 'e'"
        "  and s.detail.resident.abi == 4, 'E installs and is resident, publishing engine abi 4\\'s table')\n"
        "return finish()\n");
    LA = fresh_from(dll_a, globals);
    phase(LA,
        "hu = load_engine()\n"
        "local h, why = hu.new('addon_a')\n"
        "local path = tostring(why):match('; the resident copy is (.+)$')\n"
        "check(h == nil and tostring(why):find('new needs hideui 0.11.0 or newer; the resident copy is ', 1, true) == 1"
        "  and copy_of(path) == 'e', 'new() through A to E, a 0.6.3 resident: no handle, and the copy to update: '"
        "  .. tostring(why))\n"
        "local s = hu.status()\n"
        "check(s and s.ok and s.role == 'forwarder' and copy_of(s.detail.resident.path) == 'e' and s.handles == 1,"
        "  'status() through A to E, a slot E has, still answers')\n"
        "local ok, why2, kind = hu.shutdown()\n"
        "check(ok == false and kind == 'handles', 'A\\'s shutdown reaches E, which refuses while E holds a handle')\n"
        "return finish()\n");
    lua.close(LA);
    phase(LE,
        "check(he:hide('buff') == true and he:release() == true and hu.shutdown() == true,"
        "  'E serves itself, releases and shuts down')\n"
        "return finish()\n");
    settle(LE);
    check(record_magic() == 0 && registry_pristine(), "the record is cleared and the registry pristine");
    lua.close(LE);
    check(unmaps(dll_a) && unmaps(dll_e), "and neither copy stays mapped");

    // -- F resident: its table is engine abi 5's, as a 0.7.0 to 0.8.0
    //    resident's is; this copy makes no handle through it, the macro
    //    slots past its table, and names the copy to update, while the calls
    //    whose slots F has still reach it; F's own handle blocks the keys
    lua_State* LF = fresh_from(dll_f, globals);
    phase(LF,
        "hu = load_engine()\n"
        "hf = hu.new('addon_f')\n"
        "local s = hu.status()\n"
        "check(hf ~= nil and s.ok and s.role == 'resident' and copy_of(s.detail.resident.path) == 'f'"
        "  and s.detail.resident.abi == 5, 'F installs and is resident, publishing engine abi 5\\'s table')\n"
        "check(hf:block_macros() == true and hf:macros().mine == true and hu.status().macros_blocked == true,"
        "  'F\\'s own handle, on its own full table, blocks the macro keys')\n"
        "return finish()\n");
    LA = fresh_from(dll_a, globals);
    phase(LA,
        "hu = load_engine()\n"
        "local h, why = hu.new('addon_a')\n"
        "local path = tostring(why):match('; the resident copy is (.+)$')\n"
        "check(h == nil and tostring(why):find('new needs hideui 0.11.0 or newer; the resident copy is ', 1, true) == 1"
        "  and copy_of(path) == 'f', 'new() through A to F, a 0.8.0 resident: no handle, and the copy to update: '"
        "  .. tostring(why))\n"
        "local s = hu.status()\n"
        "check(s and s.ok and s.role == 'forwarder' and copy_of(s.detail.resident.path) == 'f' and s.handles == 1"
        "  and s.macros_blocked == true, 'status() through A to F, a slot F has, still answers, macros_blocked with it')\n"
        "local ok, why2, kind = hu.shutdown()\n"
        "check(ok == false and kind == 'handles', 'A\\'s shutdown reaches F, which refuses while F holds a handle')\n"
        "return finish()\n");
    lua.close(LA);
    phase(LF,
        "check(hf:unblock_macros() == true and hu.status().macros_blocked == false and hf:release() == true"
        "  and hu.shutdown() == true, 'F unblocks, releases and shuts down')\n"
        "return finish()\n");
    settle(LF);
    check(record_magic() == 0 && registry_pristine(), "the record is cleared and the registry pristine");
    lua.close(LF);
    check(unmaps(dll_a) && unmaps(dll_f), "and neither copy stays mapped");

    // -- G resident: its table is engine abi 6's, as a 0.9.0 resident's is;
    //    this copy makes no handle through it, the category slots past
    //    its table, and names the copy to update, while the calls whose
    //    slots G has still reach it; G's own handle blocks a list of ability
    lua_State* LG = fresh_from(dll_g, globals);
    phase(LG,
        "hu = load_engine()\n"
        "hg = hu.new('addon_g')\n"
        "local s = hu.status()\n"
        "check(hg ~= nil and s.ok and s.role == 'resident' and copy_of(s.detail.resident.path) == 'g'"
        "  and s.detail.resident.abi == 6, 'G installs and is resident, publishing engine abi 6\\'s table')\n"
        "check(hg:block('ability', 'pet_commands') == true and hg:info('ability').mine.blocked_categories[1] == 'pet_commands'"
        "  and hg:block_macros() == true, 'G\\'s own handle, on its own full table, blocks a list of ability and the keys')\n"
        "return finish()\n");
    LA = fresh_from(dll_a, globals);
    phase(LA,
        "hu = load_engine()\n"
        "local h, why = hu.new('addon_a')\n"
        "local path = tostring(why):match('; the resident copy is (.+)$')\n"
        "check(h == nil and tostring(why):find('new needs hideui 0.11.0 or newer; the resident copy is ', 1, true) == 1"
        "  and copy_of(path) == 'g', 'new() through A to G, a 0.9.0 resident: no handle, and the copy to update: '"
        "  .. tostring(why))\n"
        "local s = hu.status()\n"
        "check(s and s.ok and s.role == 'forwarder' and copy_of(s.detail.resident.path) == 'g' and s.handles == 1"
        "  and s.macros_blocked == true, 'status() through A to G, a slot G has, still answers, macros_blocked with it')\n"
        "local ok, why2, kind = hu.shutdown()\n"
        "check(ok == false and kind == 'handles', 'A\\'s shutdown reaches G, which refuses while G holds a handle')\n"
        "return finish()\n");
    lua.close(LA);
    phase(LG,
        "check(hg:unblock('ability', 'pet_commands') == true and hg:unblock_macros() == true and hg:release() == true"
        "  and hu.shutdown() == true, 'G unblocks, releases and shuts down')\n"
        "return finish()\n");
    settle(LG);
    check(record_magic() == 0 && registry_pristine(), "the record is cleared and the registry pristine");
    lua.close(LG);
    check(unmaps(dll_a) && unmaps(dll_g), "and neither copy stays mapped");

    // -- H resident: its table is engine abi 7's, as a 0.10.0 resident's is;
    //    this copy makes no handle through it, the yes/no slots past its
    //    table, and names the copy to update, while the calls whose slots H
    //    has still reach it
    lua_State* LH = fresh_from(dll_h, globals);
    phase(LH,
        "hu = load_engine()\n"
        "hh = hu.new('addon_h')\n"
        "local s = hu.status()\n"
        "check(hh ~= nil and s.ok and s.role == 'resident' and copy_of(s.detail.resident.path) == 'h'"
        "  and s.detail.resident.abi == 7, 'H installs and is resident, publishing engine abi 7\\'s table')\n"
        "check(hh:block_macros() == true, 'H\\'s own handle, on its own full table, blocks the macro keys')\n"
        "return finish()\n");
    {
        HuEngineApi api;
        check(record_api(&api) && api.abi_version == 7 && api.size == offsetof(HuEngineApi, open8),
            "H's record holds engine abi 7's table, ending at unblock7");
    }
    LA = fresh_from(dll_a, globals);
    phase(LA,
        "hu = load_engine()\n"
        "local h, why = hu.new('addon_a')\n"
        "local path = tostring(why):match('; the resident copy is (.+)$')\n"
        "check(h == nil and tostring(why):find('new needs hideui 0.11.0 or newer; the resident copy is ', 1, true) == 1"
        "  and copy_of(path) == 'h', 'new() through A to H, a 0.10.0 resident: no handle, and the copy to update: '"
        "  .. tostring(why))\n"
        "local s = hu.status()\n"
        "check(s and s.ok and s.role == 'forwarder' and copy_of(s.detail.resident.path) == 'h' and s.handles == 1"
        "  and s.macros_blocked == true, 'status() through A to H, a slot H has, still answers')\n"
        "local ok, why2, kind = hu.shutdown()\n"
        "check(ok == false and kind == 'handles', 'A\\'s shutdown reaches H, which refuses while H holds a handle')\n"
        "return finish()\n");
    lua.close(LA);
    phase(LH,
        "check(hh:unblock_macros() == true and hh:release() == true and hu.shutdown() == true,"
        "  'H unblocks, releases and shuts down')\n"
        "return finish()\n");
    settle(LH);
    check(record_magic() == 0 && registry_pristine(), "the record is cleared and the registry pristine");
    lua.close(LH);
    check(unmaps(dll_a) && unmaps(dll_h), "and neither copy stays mapped");

    // -- I resident: its table is engine abi 8's, as an earlier 0.11.0
    //    build's is; this copy makes no handle through it, open9 and
    //    open_set9 past its table, and names the copy to update, while the
    //    calls whose slots I has still reach it; I's own open8 refuses
    //    inventor, whose open alone can crash the client, as every open slot
    //    does
    lua_State* LI = fresh_from(dll_i, globals);
    phase(LI,
        "hu = load_engine()\n"
        "hi = hu.new('addon_i')\n"
        "local s = hu.status()\n"
        "check(hi ~= nil and s.ok and s.role == 'resident' and copy_of(s.detail.resident.path) == 'i'"
        "  and s.detail.resident.abi == 8, 'I installs and is resident, publishing engine abi 8\\'s table')\n"
        "check(hi:block_macros() == true, 'I\\'s own handle, on its own full table, blocks the macro keys')\n"
        "return finish()\n");
    {
        HuEngineApi api;
        check(record_api(&api) && api.abi_version == 8 && api.size == offsetof(HuEngineApi, open9),
            "I's record holds engine abi 8's table, ending at withdraw8");
        HuEngineHandle raw = {-1, 0};
        char why[512] = "";
        if (record_api(&api)) {
            api.handle_open("raw_i", &raw, why, sizeof(why));
        }
        const bool refused = raw.slot >= 0 && api.open8(&raw, "inventor", why, sizeof(why)) == 0
            && strstr(why, "inventor cannot be opened alone") == why;
        if (raw.slot >= 0) {
            api.handle_release(&raw);
        }
        check(refused, "I's own open8 refuses inventor with its reason, as every open slot does");
    }
    LA = fresh_from(dll_a, globals);
    phase(LA,
        "hu = load_engine()\n"
        "local h, why = hu.new('addon_a')\n"
        "local path = tostring(why):match('; the resident copy is (.+)$')\n"
        "check(h == nil and tostring(why):find('new needs hideui 0.11.0 or newer; the resident copy is ', 1, true) == 1"
        "  and copy_of(path) == 'i', 'new() through A to I, an earlier 0.11.0 build: no handle, and the copy to update: '"
        "  .. tostring(why))\n"
        "local s = hu.status()\n"
        "check(s and s.ok and s.role == 'forwarder' and copy_of(s.detail.resident.path) == 'i' and s.handles == 1"
        "  and s.macros_blocked == true, 'status() through A to I, a slot I has, still answers')\n"
        "local ok, why2, kind = hu.shutdown()\n"
        "check(ok == false and kind == 'handles', 'A\\'s shutdown reaches I, which refuses while I holds a handle')\n"
        "return finish()\n");
    lua.close(LA);
    phase(LI,
        "check(hi:unblock_macros() == true and hi:release() == true and hu.shutdown() == true,"
        "  'I unblocks, releases and shuts down')\n"
        "return finish()\n");
    settle(LI);
    check(record_magic() == 0 && registry_pristine(), "the record is cleared and the registry pristine");
    lua.close(LI);
    check(unmaps(dll_a) && unmaps(dll_i), "and neither copy stays mapped");

    // -- J resident: its table is engine abi 9's up to set_setting9, without
    //    info9 and unblock9; this copy makes a handle through it, and its
    //    info and unblock fall back to info4 and unblock7, which name the
    //    lists as 0.10.0 did
    lua_State* LJ = fresh_from(dll_j, globals);
    phase(LJ,
        "hu = load_engine()\n"
        "hj = hu.new('addon_j')\n"
        "local s = hu.status()\n"
        "check(hj ~= nil and s.ok and s.role == 'resident' and copy_of(s.detail.resident.path) == 'j'"
        "  and s.detail.resident.abi == 9, 'J installs and is resident, publishing engine abi 9\\'s table')\n"
        "return finish()\n");
    {
        HuEngineApi api;
        check(record_api(&api) && api.abi_version == 9 && api.size == offsetof(HuEngineApi, info9),
            "J's record holds engine abi 9's table, ending at set_setting9");
    }
    LA = fresh_from(dll_a, globals);
    phase(LA,
        "hu = load_engine()\n"
        "local h, why = hu.new('addon_a')\n"
        "local s = hu.status()\n"
        "check(h ~= nil and s.role == 'forwarder' and copy_of(s.detail.resident.path) == 'j',"
        "  'new() through A to J makes a handle: ' .. tostring(why))\n"
        "check(h:block('ability', 'monstrosity') == true and h:block('ability', '6') == true,"
        "  'block(ability, monstrosity) and block(ability, 6) through J\\'s block8')\n"
        "local p = h:info('ability')\n"
        "local mine = table.concat(p.mine.blocked_categories, ' ')\n"
        "check(mine == '6 weapon_skills', 'info falls back to info4, which names 20 weapon_skills and 6 by its number: '"
        "  .. mine)\n"
        "check(h:unblock('ability', 'weapon_skills') == true and h:unblock('ability', '6') == true"
        "  and #h:info('ability').mine.blocked_categories == 0,"
        "  'unblock falls back to unblock7, whose weapon_skills drops 20 too')\n"
        "check(h:release() == true, 'A releases its handle')\n"
        "return finish()\n");
    lua.close(LA);
    phase(LJ,
        "check(hj:release() == true and hu.shutdown() == true, 'J releases and shuts down')\n"
        "return finish()\n");
    settle(LJ);
    check(record_magic() == 0 && registry_pristine(), "the record is cleared and the registry pristine");
    lua.close(LJ);
    check(unmaps(dll_a) && unmaps(dll_j), "and neither copy stays mapped");

    // -- a zone change (0.11.0): partywin moved by its frame while one member
    //    tall, then closed, opened again at its template's height (40 on the
    //    stand-in) and swapped to its roster through the stand-in's hooked
    //    entry, keeps the bottom edge the move gave it at each step
    L = fresh(globals);
    phase(L,
        "hu = load_engine()\n"
        "h = hu.new('zoner')\n"
        "check(hu.status().ok, 'an engine for the zone change: ' .. tostring(hu.status().detail.error))\n"
        "return finish()\n");
    {
        const int prow = row_named("partywin");
        if (live_menu(prow)) {
            phase(L,
                "check(h:close('partywin') == true, 'close(partywin), for an instance at its template')\n"
                "return finish()\n");
            wait_frames(3);
        }
        uint8_t* party = static_cast<uint8_t*>(open_on_game_thread("menu    partywin"));
        pause_game();
        const bool fresh_party = party && get16(party, 0x3C) == 50 && get16(party, 0x40) == 90;
        if (party) {
            put16(party, 0x3C, 70);
        }
        resume_game();
        phase(L,
            "check(h:move('partywin', 600, 300) == true, 'move partywin by its frame, one member tall (20 of 40)')\n"
            "return finish()\n");
        wait_frames(3);
        pause_game();
        const bool moved = party && origin(party, 0) == 600 && origin(party, 1) == 280 && get16(party, 0x3A) == 600
            && get16(party, 0x3C) == 300 && get16(party, 0x40) == 320;
        resume_game();
        phase(L,
            "check(h:close('partywin') == true, 'the zone change closes partywin')\n"
            "return finish()\n");
        wait_frames(3);
        const bool closed = live_menu(prow) == NULL;
        uint8_t* reopened = static_cast<uint8_t*>(open_on_game_thread("menu    partywin"));
        pause_game();
        const bool on_bottom = reopened && origin(reopened, 0) == 600 && origin(reopened, 1) == 280
            && get16(reopened, 0x3C) == 280 && get16(reopened, 0x40) == 320;
        fl->swaps[0] = '\0';
        resume_game();
        check(fresh_party && moved && closed && on_bottom,
            "partywin one member tall (70..90 on its template's 50..90) moved by its frame to 600,300: origin 280,"
            " frame 300..320; closed and opened again at its template's 40: on the origin 280 the move pinned, its"
            " bottom at 320 where the move put it, not converted again to 300..340");
        InterlockedExchange(g_fake->party_swap, 1);
        wait_frames(3);
        pause_game();
        const bool roster = reopened && strcmp(fl->swaps, "ptw1    |") == 0 && get16(reopened, 0x3A) == 600
            && get16(reopened, 0x3C) == 7 && get16(reopened, 0x40) == 320 && origin(reopened, 1) == 280;
        resume_game();
        check(roster,
            "its roster's swap to ptw1 (the stand-in's frame snapped up to 7): hung from the same bottom 320, the top"
            " moving up, its left back at 600");
        phase(L,
            "local p = h:info('partywin')\n"
            "local r = h:remembered().partywin.position\n"
            "check(p.rect.x == 600 and p.rect.y == 7 and p.rect.bottom == 320 and r.x == 600 and r.y == 300,"
            "  'info(partywin): the frame 7..320 at x 600; remembered(): the frame top-left as given, 600,300')\n"
            "check(h:reset('partywin') == true, 'reset partywin')\n"
            "return finish()\n");
        wait_frames(3);
        phase(L,
            "check(h:release() == true and hu.shutdown() == true, 'zoner releases, and the engine shuts down')\n"
            "return finish()\n");
        settle(L);
        check(registry_pristine(), "after the zone change's engine, the registry is pristine");
    }
    lua.close(L);
    check(engine_unmaps(), "the engine of the zone change unmaps");

    // -- the 2026-10-21 20:09 freeze (7ae7e9da) as near as the stand-in plays
    //    it: the demo itself through hideui.lua, Windower stubbed, debug and
    //    events on (each page event reads options()), the dialogue pair
    //    hidden by one name before either box exists, map0 open and then
    //    closed, the target window drawing its arrow closing as both boxes
    //    open hidden on one message, the boxes' ticks asking the key test
    //    for a held left button; prerender run between frames on this thread
    //    and inside the game thread's frame. A step that does not come back
    //    fails as hung.
    {
        char demo[MAX_PATH];
        snprintf(demo, sizeof(demo), "%s", argv[6]);
        char* tests = strstr(demo, "\\engine\\tests\\");
        if (tests) {
            snprintf(tests, sizeof(demo) - static_cast<size_t>(tests - demo), "\\examples\\hideuidemo\\hideuidemo.lua");
        }
        g_watch_done = CreateEventA(NULL, TRUE, FALSE, NULL);
        HANDLE dog = CreateThread(NULL, 0, &watchdog, reinterpret_cast<LPVOID>(static_cast<uintptr_t>(60000)), 0, NULL);
        FakeLog* fl = g_fake->log;
        const int talk1 = row_named("rem4line");
        const int talk2 = row_named("rem4li2");
        uint8_t* tctl = controller_named("targetwi");
        const uint32_t close_frames = *g_fake->close_frames;
        *g_fake->close_frames = 1;
        *g_fake->query_cancel_allowed = 1;
        InterlockedExchange(g_fake->draw_walk, 1);
        InterlockedExchange(g_fake->dialogue_ticks, 1);
        g_step = "map0's open";
        open_on_game_thread("menu    map0    ");
        wait_frames(2);
        const char* g[] = {"HIDEUI", argv[6], "DEMO", demo, NULL};
        lua_State* LT = fresh(g);
        g_step = "the demo's load and its commands";
        phase(LT,
            "chat, handlers = {}, {}\n"
            "windower = {add_to_chat = function(color, text) chat[#chat + 1] = text end,"
            "  register_event = function(name, fn) handlers[name] = handlers[name] or {} table.insert(handlers[name], fn) end,"
            "  from_shift_jis = function(text) return text end, to_shift_jis = function(text) return text end,"
            "  ffxi = {get_items = function() return nil end}}\n"
            "function fire(name, ...) for _, fn in ipairs(handlers[name] or {}) do fn(...) end end\n"
            "function command(line)\n"
            "  local args = {}\n"
            "  for word in line:gmatch('%S+') do args[#args + 1] = word end\n"
            "  fire('addon command', unpack(args))\n"
            "end\n"
            "said_up_to = 0\n"
            "function said()\n"
            "  local out = {}\n"
            "  for i = said_up_to + 1, #chat do out[#out + 1] = chat[i] end\n"
            "  said_up_to = #chat\n"
            "  return table.concat(out, ' | ')\n"
            "end\n"
            "heard = ''\n"
            "_addon = {}\n"
            "package.preload['libs.hideui'] = function() return dofile(HIDEUI) end\n"
            "package.preload['config'] = function() return {load = function(t) return t end,"
            "  save = function() return true end} end\n"
            "dofile(DEMO)\n"
            "library = require('libs.hideui')\n"
            "check(_addon.name == 'hideuidemo' and handlers['addon command'] and handlers.prerender and handlers.unload"
            "  and library.status().handles == 1, 'the demo loads through hideui.lua with its handle, Windower stubbed')\n"
            "said()\n"
            "command('debug on') command('events on') command('hide rem4line') command('info rem4li2')\n"
            "local text = said()\n"
            "check(text:find('events on: every window, and pending', 1, true) and text:find('hide rem4line', 1, true)"
            "  and text:find('rem4li2: hidden hide-only', 1, true) and text:find('hidden by hideuidemo', 1, true)"
            "  and text:find('not open', 1, true), 'debug on, events on, hide rem4line, info rem4li2: ' .. text)\n"
            "return finish()\n");
        for (int i = 0; i < 4; ++i) {
            wait_frames(1);
            phase(LT, "fire('prerender') heard = heard .. ' | ' .. said() return finish()\n");
        }
        g_step = "targetwi's open, its arrow drawn";
        if (live_menu(row_named("targetwi"))) {
            // closed first: an open of a window already open posts nothing
            typedef int (__fastcall* CloseFn)(void* mcb, void* edx, const char* key);
            CloseFn close_first;
            memcpy(&close_first, &g_fake->close, sizeof(close_first));
            pause_game();
            close_first(g_fake->mcb, NULL, "menu    targetwi");
            resume_game();
            wait_frames(2);
        }
        tctl[0x65] = 0;
        tctl[0xB8] = 0;
        put16(tctl, 0xBC, 640);
        put16(tctl, 0xBE, 300);
        open_on_game_thread("menu    targetwi");
        wait_frames(2);
        {
            typedef int (__fastcall* CloseFn)(void* mcb, void* edx, const char* key);
            CloseFn close_by_name;
            memcpy(&close_by_name, &g_fake->close, sizeof(close_by_name));
            g_step = "map0's close";
            pause_game();
            close_by_name(g_fake->mcb, NULL, "menu    map0    ");
            resume_game();
            for (int i = 0; i < 4; ++i) {
                wait_frames(1);
                phase(LT, "fire('prerender') heard = heard .. ' | ' .. said() return finish()\n");
            }
            phase(LT,
                "check(heard:find('closed map0', 1, true) and heard:find('opened targetwi', 1, true)"
                "  and heard:find('opened targetarrow', 1, true), 'map0 closed, targetwi and its arrow opened: ' .. heard)\n"
                "heard = ''\n"
                "return finish()\n");
            // the conversation: the target window closes and both boxes open
            // on one message, waiting for a key, as the player clicks
            g_step = "the conversation's start";
            const int advances = fl->advances;
            const int clicks = fl->dialogue_clicks;
            pause_game();
            close_by_name(g_fake->mcb, NULL, "menu    targetwi");
            resume_game();
            asked_on_game_thread(9, (1 << 8) | (1 << 16));
            asked_on_game_thread(10, (1 << 8) | (1 << 16));
            uint8_t* box1 = live_menu(talk1);
            uint8_t* box2 = live_menu(talk2);
            put32(g_fake->mcb, 0x54, static_cast<uint32_t>(reinterpret_cast<uintptr_t>(box1)));
            *g_fake->left_click = 1;
            bool back = true;
            for (int i = 0; i < 20; ++i) {
                g_step = "prerender between frames, the boxes up";
                wait_frames(1);
                phase(LT, "fire('prerender') heard = heard .. ' | ' .. said() return finish()\n");
                g_step = "prerender inside the game thread's frame, the boxes up";
                back = phase_on_game_thread(LT, "fire('prerender') heard = heard .. ' | ' .. said() return finish()\n")
                    && back;
            }
            *g_fake->left_click = 0;
            const LONG frames = *g_fake->frames;
            wait_frames(3);
            check(back && *g_fake->frames - frames >= 3,
                "prerender came back every time, on either thread, and the game kept running frames");
            g_step = "the boxes' walk";
            check(box1 && box2 && walk_of(box1) == 'h' && walk_of(box2) == 'h' && fl->advances == advances
                    && fl->dialogue_clicks == clicks,
                "both boxes open hidden: their draws run and paint nothing; the held left button moves no page on");
            g_step = "the events' check";
            phase(LT,
                "check(heard:find('closed targetwi', 1, true) and heard:find('opened rem4line', 1, true)"
                "  and heard:find('opened rem4li2', 1, true)"
                "  and heard:find('page rem4line: \"Moogle : Welcome home, / kupo!\"', 1, true)"
                "  and heard:find('page rem4li2: \"Moogle : Welcome home, / kupo!\"', 1, true)"
                "  and not heard:find('failed', 1, true) and not heard:find('error', 1, true),"
                "  'the demo printed the close, both opens and each box\\'s page, read through options(): ' .. heard)\n"
                "heard = ''\n"
                "command('answer rem4line')\n"
                "return finish()\n");
            for (int i = 0; i < 4; ++i) {
                g_step = "prerender inside the game thread's frame, the page answered";
                back = phase_on_game_thread(LT, "fire('prerender') heard = heard .. ' | ' .. said() return finish()\n")
                    && back;
            }
            phase(LT,
                "check(heard:find('page rem4line: \"Rest well.\"', 1, true)"
                "  and heard:find('page rem4li2: \"Rest well.\"', 1, true),"
                "  'answer rem4line moved both boxes on to the next page: ' .. heard)\n"
                "heard = ''\n"
                "command('answer rem4li2')\n"
                "return finish()\n");
            for (int i = 0; i < 4; ++i) {
                g_step = "prerender inside the game thread's frame, the last page answered";
                back = phase_on_game_thread(LT, "fire('prerender') heard = heard .. ' | ' .. said() return finish()\n")
                    && back;
            }
            check(back && !live_menu(talk1) && !live_menu(talk2) && fl->advances == advances + 4,
                "answer rem4li2 on the last page ended the message in both boxes, and both closed");
            phase(LT,
                "check(heard:find('closed rem4line', 1, true) and heard:find('closed rem4li2', 1, true),"
                "  'the demo printed both closes: ' .. heard)\n"
                "return finish()\n");
        }
        put32(g_fake->mcb, 0x54, 0);
        put16(tctl, 0xBC, 0);
        put16(tctl, 0xBE, 0);
        InterlockedExchange(g_fake->dialogue_ticks, 0);
        InterlockedExchange(g_fake->draw_walk, 0);
        *g_fake->close_frames = close_frames;
        g_step = "the demo's unload";
        phase(LT,
            "fire('unload')\n"
            "return finish()\n");
        settle(LT, "library");
        check(record_magic() == 0 && registry_pristine(),
            "the demo's unload released its handle and shut the engine down: no resident, the registry pristine");
        lua.close(LT);
        check(engine_unmaps(), "the engine the demo loaded unmaps");
        SetEvent(g_watch_done);
        WaitForSingleObject(dog, 5000);
        CloseHandle(dog);
        CloseHandle(g_watch_done);
    }

    // -- the heap check, in an engine built with -DHU_HEAPCHECK alone
    {
        HMODULE probe = LoadLibraryA(g_dll);
        const bool built = probe && GetProcAddress(probe, "hideui_heapcheck");
        if (probe) {
            FreeLibrary(probe);
        }
        if (built) {
            heap_checks(globals);
        } else {
            std::printf("SKIP  the heap check: this engine is not built with -DHU_HEAPCHECK\n");
        }
    }

    InterlockedExchange(&g_stop, 1);
    WaitForSingleObject(game, 5000);
    CloseHandle(game);

    std::printf("%d of %d checks failed\n", g_failures, g_checks);
    return g_failures;
}
