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
//
// e2e_test.exe <LuaCore.dll> <a/_HideUI.dll> <FFXiMain.dll> <b/_HideUI.dll> <c/_HideUI.dll> <hideui.lua>
//              <d/_HideUI.dll> <e/_HideUI.dll> <f/_HideUI.dll> <g/_HideUI.dll>
//
// Copy e publishes engine abi 4's table, standing for a 0.5.1 to 0.6.3
// resident; copy f engine abi 5's, standing for a 0.7.0 to 0.8.0 one; copy
// g engine abi 6's, standing for a 0.9.0 one.

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
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
    "  return path and path:match('[/\\\\]([abcdefg])[/\\\\]_[Hh]ide[Uu][Ii]%.dll$')\n"
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
    void* calls[12];
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
const char* const kCallNames[12] = {
    "query_confirm", "passinpu_reset", "prtyjoin_send", "prtyjoin_clear", "link5_clear",
    "arealist_close", "arealist_latch", "post_request", "post_request_close", "dock_reset", "template_swap",
    "set_frame_rect",
};

FakeState* g_fake;

typedef void (__fastcall* UpdateFn)(void* mcb, void* edx);
typedef void* (__fastcall* OpenFn)(void* mcb, void* edx, const char* key, int activate, int overlap);
typedef void (__cdecl* AbilityOpenFn)(int kind, int flag, int extra);

volatile LONG g_stop;
volatile LONG g_pending;
volatile LONG g_pause;
volatile LONG g_paused;
const char* g_open_key;
void* g_open_result;
HANDLE g_open_done;
volatile LONG g_ability_pending;
int g_ability_args[3];

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
            g_open_result = open(g_fake->mcb, NULL, g_open_key, 1, 1);
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
        update(g_fake->mcb, NULL);
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

// The hooked open from this thread, for an open that must come while no
// frame runs: only between pause_game() and resume_game().
void* open_here(const char* key) {
    OpenFn open;
    memcpy(&open, &g_fake->open, sizeof(open));
    return open(g_fake->mcb, NULL, key, 1, 1);
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

}  // namespace

// A thread made suspended and terminated before it runs: its context is set
// to stand inside a hooked routine's prologue.
DWORD WINAPI never_runs(LPVOID) {
    return 0;
}

int main(int argc, char** argv) {
    if (argc < 11) {
        std::printf("usage: e2e_test.exe <LuaCore.dll> <a/_HideUI.dll> <FFXiMain.dll> <b/_HideUI.dll>"
            " <c/_HideUI.dll> <hideui.lua> <d/_HideUI.dll> <e/_HideUI.dll> <f/_HideUI.dll> <g/_HideUI.dll>\n");
        return 2;
    }
    for (int i = 2; i < 11; ++i) {
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
            "check(s.engine == '0.10.0' and s.handles == 0 and s.dropped == 0 and type(s.hidden) == 'table'"
            "  and type(s.resized) == 'table' and d.image and d.image.abi == 7 and s.ui == nil,"
            "  'status: the public fields at the top, the image under detail')\n"
            "local ok, err = pcall(hu.new, 42)\n"
            "check(not ok and tostring(err):find('string expected', 1, true), 'new(42) raises: ' .. tostring(err))\n"
            "check(hu.shutdown() == true, 'shutdown of an engine that never installed is a no-op')\n"
            "check(hu.version() == 'hideui 0.10.0', 'version: ' .. hu.version())\n"
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

    char addresses[26][16];
    char call_globals[12][24];
    void* const fns[] = {g_fake->open, g_fake->ui_update, g_fake->staged_close, g_fake->show_path,
                         g_fake->set_position, g_fake->close, g_fake->registry, g_fake->mcb,
                         g_fake->party_pending};
    for (int i = 0; i < 9; ++i) {
        snprintf(addresses[i], sizeof(addresses[i]), "0x%08X",
            static_cast<unsigned>(reinterpret_cast<uintptr_t>(fns[i])));
    }
    for (int i = 0; i < 12; ++i) {
        snprintf(addresses[9 + i], sizeof(addresses[9 + i]), "0x%08X",
            static_cast<unsigned>(reinterpret_cast<uintptr_t>(g_fake->calls[i])));
        // The stand-in still has query's confirm; the engine resolves it no
        // longer, so it is no F_ global.
        snprintf(call_globals[i], sizeof(call_globals[i]), "%s_%s", i == 0 ? "UNRESOLVED" : "F", kCallNames[i]);
    }
    snprintf(addresses[21], sizeof(addresses[21]), "0x%08X",
        static_cast<unsigned>(reinterpret_cast<uintptr_t>(g_fake->link5_cache)));
    snprintf(addresses[22], sizeof(addresses[22]), "0x%08X",
        static_cast<unsigned>(reinterpret_cast<uintptr_t>(g_fake->query_cancel_allowed)));
    snprintf(addresses[23], sizeof(addresses[23]), "0x%08X",
        static_cast<unsigned>(reinterpret_cast<uintptr_t>(g_fake->set_cursor)));
    snprintf(addresses[24], sizeof(addresses[24]), "0x%08X",
        static_cast<unsigned>(reinterpret_cast<uintptr_t>(g_fake->link5_latch)));
    snprintf(addresses[25], sizeof(addresses[25]), "0x%08X",
        static_cast<unsigned>(reinterpret_cast<uintptr_t>(g_fake->link5_callback)));
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
    const char* globals[] = {"OPEN", addresses[0], "UPDATE", addresses[1], "STAGED", addresses[2],
                             "SHOW", addresses[3], "SETPOS", addresses[4], "CLOSE", addresses[5],
                             "REGISTRY", addresses[6], "MCB", addresses[7], "PENDING", addresses[8],
                             call_globals[0], addresses[9], call_globals[1], addresses[10],
                             call_globals[2], addresses[11], call_globals[3], addresses[12],
                             call_globals[4], addresses[13], call_globals[5], addresses[14],
                             call_globals[6], addresses[15], call_globals[7], addresses[16],
                             call_globals[8], addresses[17], call_globals[9], addresses[18],
                             call_globals[10], addresses[19], call_globals[11], addresses[20],
                             "LINK5CACHE", addresses[21], "CANCELBYTE", addresses[22],
                             "SETCURSOR", addresses[23], "LINK5LATCH", addresses[24], "LINK5CB", addresses[25],
                             "CONVERT", convert_address, "MOUSE", mouse_addresses[0], "HITTEST", mouse_addresses[1],
                             "MOUSEREAD", mouse_addresses[2], "SINK", input_addresses[0],
                             "ROUTING", input_addresses[1], "COMPASS", compass_addresses[0],
                             "COMPASSGLOBAL", compass_addresses[1], "MACROGATE", macro_addresses[0],
                             "MACROOBJECT", macro_addresses[1], "ABILITYOPEN", ability_address, NULL};

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
            const uint8_t saved = *pokes[i].at;
            poke(pokes[i].at, pokes[i].value);
            const char* g[] = {"EXPECT", pokes[i].expect, NULL};
            lua_State* L = fresh(g);
            phase(L, kExpectFailure);
            lua.close(L);
            poke(pokes[i].at, saved);
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
        check(!engine_mapped() && static_cast<uint8_t*>(g_fake->open)[0] == 0xE9,
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
        "check(s.engine == '0.10.0' and s.handles == 1 and s.dropped == 0 and hu.version() == 'hideui 0.10.0'"
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
        "check(calls == 11 and all and f.prtyjoin_pending == PENDING and f.query_confirm == nil"
        "  and f.list_select == nil,"
        "  'the eleven reply, re-dock and resize routines, arealist\\'s close+reset and latch clear among them,'"
        "  .. ' and the invite flag resolved to the stand-in; query\\'s confirm and the list select-index are'"
        "  .. ' not resolved')\n"
        "check(f.link5_cache == LINK5CACHE and f.query_cancel_allowed == CANCELBYTE,"
        "  'the link5 concierge cache and the query cancel-allowed byte resolved to the stand-in: '"
        "  .. tostring(f.link5_cache) .. ' ' .. tostring(f.query_cancel_allowed))\n"
        "check(f.set_cursor == SETCURSOR and f.link5_latch == LINK5LATCH and f.link5_callback == LINK5CB,"
        "  'SetCursor, the link5 pending latch and the event callback the link5 open site names resolved to the'"
        "  .. ' stand-in: ' .. tostring(f.set_cursor) .. ' ' .. tostring(f.link5_latch) .. ' '"
        "  .. tostring(f.link5_callback))\n"
        "check(d.registry == REGISTRY and d.manager == MCB and d.rows == 370 and d.names == 370"
        "  and d.ui and d.ui.w == 1920 and d.ui.h == 1080, 'registry, manager and UI size resolved')\n"
        "check(d.pinned and d.daemon and d.daemon.abi == 1, 'pinned, daemon abi ' .. tostring(d.daemon and d.daemon.abi))\n"
        "check(h:hide('logwindo') == true and h:block('menuwind') == true and h:move('equip', 300, 200) == true,"
        "  'hide, block and move queue')\n"
        "local r, why = h:block('query')\n"
        "check(r == nil and why == \"query cannot be blocked or closed; answer it with answer('query', value) or"
        " cancel('query')\", 'block(query) is refused: ' .. tostring(why))\n"
        "local prompts = {}\n"
        "for _, w in ipairs({'query', 'passinpu', 'prtyjoin', 'delivery', 'post1', 'post2', 'link5', 'arealist'}) do\n"
        "  local ok2, why2 = h:close(w)\n"
        "  if ok2 == nil and why2 == w .. \" is a prompt the game waits on; use cancel('\" .. w .. \"')\" then"
        "    prompts[#prompts + 1] = w end\n"
        "end\n"
        "check(#prompts == 8, 'close() is refused for the eight prompt windows, with \"use cancel(name)\": '"
        "  .. table.concat(prompts, ' '))\n"
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
    uint8_t* log = live_menu(kRowLog);
    check(registry_row(kRowLog)[0x29] == 0x7F && log && log[0x60] == 0x7F,
        "the drain, on the game thread, hid logwindo in the registry and on screen");
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
    check(registry_row(kRowMenuwind)[0x29] == 0x7F, "block hides menuwind's registry row");

    // -- the mouse kept off a hidden window (0.6.0): +0x77 on a new instance's
    //    open, the unhide and the hide, and the stand-in's mouse mode picker
    //    through the hook, called here with no frame depending on it
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
        check(ability && registry_row(kRowAbility)[0x29] == 0x7F && ability[0x60] == 0x7F && ability[0x77] == 0,
            "ability opened while hidden: registry and live layer 0x7F, and +0x77 0, the open's post over the"
            " stand-in constructor's 1");
        const LONG calls = *g_fake->menu_up_calls;
        put32(g_fake->mcb, 0x54, reinterpret_cast<uint32_t>(ability));
        mouse[0x4D] = 2;
        mouse_mode(mouse, NULL);
        check(mouse[0x4D] == 1 && *g_fake->menu_up_calls == calls,
            "the mouse mode picker with the hidden window active: mode 1, its callee never ran (the pre did instead)");
        phase(L,
            "check(h:unhide('ability') == true, 'unhide(ability) queues')\n"
            "return finish()\n");
        wait_frames(3);
        const bool shown = ability[0x60] == layer && ability[0x77] == 1 && registry_row(kRowAbility)[0x29] == layer;
        mouse_mode(mouse, NULL);
        check(shown && mouse[0x4D] == 2 && *g_fake->menu_up_calls == calls + 1,
            "unhide: +0x77 back to 1 with the layer; the picker's original runs and sets mode 2");
        phase(L,
            "check(h:hide('ability') == true, 'hide(ability) while it is open queues')\n"
            "return finish()\n");
        wait_frames(3);
        const bool hidden = ability[0x60] == 0x7F && ability[0x77] == 0;
        mouse_mode(mouse, NULL);
        const bool held = mouse[0x4D] == 1 && *g_fake->menu_up_calls == calls + 1;
        put32(g_fake->mcb, 0x54, 0);
        mouse[0x4D] = 2;
        mouse_mode(mouse, NULL);
        check(hidden && held && mouse[0x4D] == 1 && *g_fake->menu_up_calls == calls + 2,
            "hide of the open window: +0x77 0 and the picker held in mode 1; with no active menu the original"
            " runs (mode 1, its callee called)");
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
    //    here with no frame depending on it
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
        bool dropped = ability[0x60] == 0x7F;
        for (int code = 1; code <= 0x20; ++code) {
            dropped = route(g_fake->mcb, NULL, code) == 0 && dropped;
        }
        check(dropped && fl->sink_calls == calls + 1 && fl->main_menu_opens == opens,
            "hidden and active: every code from the routing routine, 1 to 0x20, answers 0 without the sink's body"
            " running, and code 7 opens no main menu");
        const uint32_t cursor = sink(ability, NULL, 9);
        const bool from_cursor = cursor == 1 && fl->sink_calls == calls + 2 && fl->sink_code == 9;
        const uint32_t wheel = sink(ability, NULL, 0x16);
        check(from_cursor && wheel == 1 && fl->sink_calls == calls + 3 && fl->sink_code == 0x16
                && fl->sink_menu == ability,
            "SetCursor's code 9 and the wheel's 0x16, from another caller, still reach the hidden window's sink");
        put32(g_fake->mcb, 0x54, 0);
        const uint32_t none = route(g_fake->mcb, NULL, 5);
        const uint32_t menu_key = route(g_fake->mcb, NULL, 7);
        check(none == 0 && menu_key == 0 && fl->sink_calls == calls + 3 && fl->main_menu_opens == opens + 1,
            "with no active menu the routing routine runs as the game's: no sink, and code 7 opens the main menu");
        phase(L,
            "check(h:unhide('ability') == true, 'unhide(ability) queues')\n"
            "return finish()\n");
        wait_frames(3);
        put32(g_fake->mcb, 0x54, reinterpret_cast<uint32_t>(ability));
        const uint32_t again = route(g_fake->mcb, NULL, 6);
        check(ability[0x60] == layer && again == 1 && fl->sink_calls == calls + 4 && fl->sink_code == 6,
            "unhide: the routing routine's codes reach the window's sink again");
        put32(g_fake->mcb, 0x54, 0);
        phase(L,
            "local seen = {}\n"
            "for _, e in ipairs((h:poll())) do seen[#seen + 1] = e.event .. ':' .. e.name end\n"
            "check(table.concat(seen, ' ') == 'opened:ability', 'events: ' .. table.concat(seen, ' '))\n"
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
        ability_open_on_game_thread(2, 0, 1);
        wait_frames(3);
        check(live_menu(kRowAbility) && !live_menu(kRowSort) && *g_fake->ability_last_kind == 2 && get32(actl, 0x64) == 2,
            "ability_open(2, 0, 1) through the site opens ability alone, its body storing 2 at the controller's +0x64");
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
        ability_open_on_game_thread(2, 0, 1);
        wait_frames(3);
        check(!live_menu(kRowAbility) && *g_fake->ability_last_kind == -1,
            "ability_open(2, 0, 1) with pet_commands blocked: the hooked opener runs nothing; nothing opens, no kind"
            " stored");
        ability_open_on_game_thread(2, 1, 1);
        wait_frames(3);
        check(!live_menu(kRowAbility) && !live_menu(kRowSort) && *g_fake->ability_last_kind == -1,
            "with flag 1 neither abisortw nor ability opens");
        ability_open_on_game_thread(1, 0, 1);
        wait_frames(3);
        check(live_menu(kRowAbility) && *g_fake->ability_last_kind == 1 && get32(actl, 0x64) == 1,
            "ability_open(1, 0, 1) still opens ability on job_abilities");
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
        ability_open_on_game_thread(2, 0, 1);
        wait_frames(3);
        check(live_menu(kRowAbility) && get32(actl, 0x64) == 2,
            "unblocked: ability_open(2, 0, 1) opens again on pet_commands");
        phase(L,
            "h:poll()\n"
            "check(h:block('ability', '2') == true, 'block(ability, 2) by number with the window on list 2 queues its close')\n"
            "return finish()\n");
        wait_frames(3);
        check(!live_menu(kRowAbility), "the category block closed the window showing that list");
        ability_open_on_game_thread(3, 0, 1);
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
        ability_open_on_game_thread(1, 0, 1);
        ability_open_on_game_thread(4, 0, 1);
        ability_open_on_game_thread(20, 0, 1);
        wait_frames(3);
        check(!live_menu(kRowAbility) && *g_fake->ability_last_kind == -1,
            "blocked whole: kinds 1, 4 and 20 through the opener open nothing");
        phase(L,
            "local seen = {}\n"
            "for _, e in ipairs((h:poll())) do if e.event == 'blocked' then seen[#seen + 1] = tostring(e.category) .. ':'"
            "  .. tostring(e.category_name) .. ':' .. table.concat(e.by, '+') .. ':' .. tostring(e.mine) end end\n"
            "check(table.concat(seen, ' ') == '1:job_abilities:alpha:true 4:job_traits:alpha:true 20:weapon_skills:alpha:true',"
            "  'blocked events carry each kind, 4 named job_traits and 20 weapon_skills, by alpha: ' .. table.concat(seen, ' '))\n"
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
        ability_open_on_game_thread(4, 0, 1);
        wait_frames(3);
        check(live_menu(kRowAbility) && get32(actl, 0x64) == 4, "nothing held: ability_open(4, 0, 1) opens job traits");
        phase(L,
            "local seen = {}\n"
            "for _, e in ipairs((h:poll())) do seen[#seen + 1] = e.event .. ':' .. tostring(e.category_name) end\n"
            "check(table.concat(seen, ' ') == 'opened:job_traits', 'opened carrying 4, job_traits: ' .. table.concat(seen, ' '))\n"
            "check(h:close('ability') == true, 'close ability')\n"
            "return finish()\n");
        wait_frames(3);
        put32(actl, 0x64, 0);
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
        "check(table.concat(seen, ' ') == 'blocked:menuwind opened:equip' and dropped == 0,"
        "  'events: ' .. table.concat(seen, ' '))\n"
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
        "check(g.chat_log.anchor == 'logwindo' and g.chat_log.anchor_open and #g.chat_log.members == 50"
        "  and carried.ability and carried.partywin and carried.targetwi and carried.subwindo and not carried.logwindo"
        "  and g.chat_log.index == nil and g.chat_log.mask == nil and g.chat_log.name == nil,"
        "  'groups(): chat_log anchored on logwindo, carrying ' .. #g.chat_log.members .. ' pieces: its 42,"
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
    check(registry_row(kRowLog)[0x29] == 0x7F, "logwindo stays hidden while beta holds it");
    check(registry_row(kRowMenuwind)[0x29] == 2, "alpha's block is gone with alpha");
    check(origin(log, 0) == 100 + kRowLog && origin(log, 1) == 50, "alpha's group move is reset with alpha");
    check(origin(equip, 0) == 10 && origin(equip, 1) == 20, "beta's move of equip survives alpha's release");

    phase(L,
        "local ok, why, kind = hu.shutdown()\n"
        "check(ok == false and kind == 'handles', 'shutdown refuses while beta is open')\n"
        "h2:release()\n"
        "ok, why, kind = hu.shutdown()\n"
        "check(ok == true, 'shutdown after the last release: ' .. tostring(why))\n"
        "local s = hu.status()\n"
        "check(not s.ok and s.state == 'idle' and not s.detail.pinned, 'idle and unpinned after shutdown')\n"
        "local r, why2 = h2:hide('buff')\n"
        "check(r == nil and why2:find('shut down', 1, true), 'verbs after shutdown say so')\n"
        "return finish()\n");
    check(memcmp(g_fake->registry, g_fake->pristine, 371 * 0x2C) == 0,
        "after shutdown the registry is byte-identical to the expected table");
    check(log[0x60] == 1 && origin(equip, 0) == 100 + kRowEquip && origin(equip, 1) == 50,
        "logwindo is visible again and equip is back at its default rect");
    lua.close(L);
    check(!engine_mapped(), "after shutdown, closing Lua unmaps the engine");
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
        "  and f.macro_gate == MACROGATE and f.ability_open == ABILITYOPEN, 'at the same addresses')\n"
        "check(h:hide('buff') == true, 'and hides through them')\n"
        "return finish()\n");
    wait_frames(3);
    check(registry_row(kRowBuff)[0x29] == 0x7F, "the re-registered drain hid buff");

    // -- Lua closed with no release and no shutdown
    lua.close(L);
    check(engine_mapped(), "an engine collected without shutdown stays pinned");
    wait_frames(3);
    check(registry_row(kRowBuff)[0x29] == 0, "and its drain still restores what the collected handle held");

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
    check(!engine_mapped(), "then the engine unmaps");
    check(memcmp(g_fake->registry, g_fake->pristine, 371 * 0x2C) == 0, "and the registry is pristine");

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
    check(fl->frame_menu == menuwind && fl->frame[0] == 100 + kRowMenuwind && fl->frame[1] == 50
            && fl->frame[2] == 120 && fl->frame[3] == 40,
        "A's release, as the last writer of the size of menuwind, restores its own: the 120x40 its frame had at"
        " the open that first put the size on");
    check(origin(pmode, 0) == 100 + row_pmode && origin(pmode, 1) == 50 && registry_pristine(),
        "and puts playermo back at its default; the registry is pristine");
    lua.close(L);
    check(!engine_mapped(), "the engine of the sizes unmaps");

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
        check(origin(party2, 1) == party_y && get16(party2, 0x3C) == 1030 && get16(party2, 0x40) == 1064 && target
                && origin(target, 1) == target_y && registry_pristine(),
            "the release puts partywin (frame 1030..1064) and targetwi back; the registry is pristine");
        lua.close(L);
        check(!engine_mapped(), "the engine of the chat log unmaps");
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
            "check(hu.status().ok and hu.status().engine == '0.10.0', 'a fresh engine for the six-member party list: '"
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
        check(origin(equip, 0) == 100 + kRowEquip && origin(equip, 1) == 50 && registry_pristine(),
            "the release puts equip back at its default rect; the registry is pristine");
        lua.close(L);
        check(!engine_mapped(), "the engine of the six-member party list unmaps");
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
        check(origin(target, 0) == 100 + row_target && origin(target, 1) == 50 && origin(sub, 0) == sx
                && origin(sub, 1) == sy && registry_pristine(),
            "the release puts targetwi and subwindo back where the game had them; the registry is pristine");
        lua.close(L);
        check(!engine_mapped(), "the engine of the waiting group move unmaps");
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
            "check(ui ~= nil and ui.name == 'layout' and hideui.version() == 'hideui 0.10.0' and hideui.status().handles == 1,"
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
            "return finish()\n");
        ability_open_on_game_thread(2, 0, 1);
        ability_open_on_game_thread(1, 0, 1);
        ability_open_on_game_thread(4, 0, 1);
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
                && registry_row(kRowDbdelsel)[0x29] == 0x7F && dbdel[0x60] == 0x7F,
            "open('dbdelsel') reached the open routine as \"tkdebug dbdelsel\"; hide put 0x7F on row 327's draw"
            " layer and the live menu's");
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
        check(record_magic() == 0 && registry_pristine() && origin(log, 1) == 930 && origin(equip, 0) == 100 + kRowEquip,
            "unload through hideui.lua released the handle and shut the engine down: everything back, no resident");
        lua.close(L);
        check(!engine_mapped(), "the engine hideui.lua loaded unmaps");
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
    char daemon_b[MAX_PATH];
    char daemon_c[MAX_PATH];
    beside(dll_b, "hideui_daemon.dll", daemon_b, sizeof(daemon_b));
    beside(dll_c, "hideui_daemon.dll", daemon_c, sizeof(daemon_c));
    check(record_magic() == 0, "no resident is published before the several-addons phase");

    lua_State* LA = fresh_from(dll_a, globals);
    phase(LA,
        "hu = load_engine()\n"
        "ha = hu.new('addon_a')\n"
        "local s = hu.status()\n"
        "local d = s.detail\n"
        "check(s.ok and s.role == 'resident' and copy_of(d.image.path) == 'a' and d.resident"
        "  and copy_of(d.resident.path) == 'a' and d.resident.abi == 7 and d.resident.build == s.engine,"
        "  'copy A installs and is resident: ' .. tostring(d.error))\n"
        "check(ha:hide('buff') == true and ha:block('menuwind') == true and ha:move('equip', 300, 200) == true,"
        "  'A hides buff, blocks menuwind and moves equip')\n"
        "return finish()\n");
    wait_frames(3);
    check(registry_row(kRowBuff)[0x29] == 0x7F && registry_row(kRowMenuwind)[0x29] == 0x7F
            && origin(equip, 0) == 300 && origin(equip, 1) == 200,
        "A's hide, block and move reach the game");
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
        "check(hu.version() == 'hideui 0.10.0', 'the version of B is its own: ' .. hu.version())\n"
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
        "check(n == 370, 'list() through B: all ' .. n .. ' names, the compass among them')\n"
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
    check(registry_row(kRowBuff)[0x29] == 0x7F, "a hide held by B survives the release of A's handle");
    check(registry_row(kRowMenuwind)[0x29] == 2, "the block held by A alone went with it");
    check(origin(equip, 0) == 10 && origin(equip, 1) == 20, "B's move survives A's release: B wrote it last");
    lua.close(LA);
    check(mapped(dll_a), "A's Lua state is closed; A stays mapped and pinned while B holds a handle");
    phase(LB,
        "check(hb:hide('logwindo') == true, 'B keeps working through A after A unloaded')\n"
        "local s = hu.status()\n"
        "check(s.ok and s.handles == 1 and copy_of(s.detail.resident.path) == 'a', 'A still serves, with one handle open')\n"
        "return finish()\n");
    wait_frames(3);
    check(registry_row(kRowLog)[0x29] == 0x7F, "B's hide of logwindo reached the game through A");

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
        "local s = hu.status()\n"
        "check(s.role == 'none' and s.state == 'idle' and not s.detail.resident, 'afterwards B finds no resident')\n"
        "return finish()\n");
    check(record_magic() == 0, "the resident record is cleared");
    check(registry_pristine(), "and the registry is byte-identical to the expected table");
    check(mapped(dll_a), "A stays mapped while B, which forwarded to it, is loaded");
    lua.close(LB);
    check(!mapped(dll_a) && !mapped(dll_b), "with both Lua states closed, neither copy stays mapped");

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
    check(registry_row(kRowBuff)[0x29] == 0x7F && registry_row(kRowLog)[0x29] == 0x7F,
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
        check(read && api.size == sizeof(HuEngineApi) && api.abi_version == HU_ENGINE_ABI && group_name
                && registry_row(row_named("targetwi"))[0x29] == 0x7F && delta && origin(party2, 1) == party_y - 5,
            "B's table, read out of the record: hide('target_window') through abi 2's slot hides its anchor"
            " targetwi, and move_group through it still shifts by dx, dy");
        HuEngineEvent events[16];
        uint32_t count = 0;
        uint32_t lost = 0;
        if (read) {
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
        check(origin(party2, 1) == party_y && registry_row(row_named("targetwi"))[0x29] != 0x7F,
            "the older handle's release takes its hide and its group move back");
    }
    lua.close(LA);
    wait_frames(3);
    phase(LB,
        "local s = hu.status()\n"
        "check(s.ok and s.handles == 1, 'A unloaded without a release: its handle went with its Lua state')\n"
        "local p = hb:info('ability')\n"
        "check(p.open and p.origin.x == p.default.x and p.origin.y == p.default.y and not p.memory,"
        "  'and the window A moved is back at its default')\n"
        "check(not hb:info('logwindo').hidden and hb:info('buff').hidden, 'the hide of A is gone, the one of B stays')\n"
        "check(hb:release() == true and hu.shutdown() == true, 'B releases and shuts down as the resident')\n"
        "return finish()\n");
    check(record_magic() == 0 && registry_pristine(), "the record is cleared and the registry pristine again");
    lua.close(LB);
    check(!mapped(dll_a) && !mapped(dll_b), "and neither copy stays mapped");

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
        "check(h == nil and tostring(why):find('new needs hideui 0.10.0 or newer; the resident copy is ', 1, true) == 1"
        "  and copy_of(path) == 'c', 'new() through A to C: ' .. tostring(why))\n"
        "local s, why2 = hu.status()\n"
        "check(s == nil and tostring(why2):find('status needs hideui 0.5.1 or newer; the resident copy is ', 1, true) == 1,"
        "  'status() through A to C: ' .. tostring(why2))\n"
        "check(hu.version() == 'hideui 0.10.0', 'A\\'s version is its own')\n"
        "local ok, why3, kind = hu.shutdown()\n"
        "check(ok == false and kind == 'handles', 'A\\'s shutdown, an engine abi 1 slot, reaches C: ' .. tostring(why3))\n"
        "return finish()\n");
    lua.close(LA);
    phase(LC,
        "check(hc:hide('buff') == true and hc:release() == true and hu.shutdown() == true,"
        "  'C serves itself, releases and shuts down')\n"
        "return finish()\n");
    check(record_magic() == 0 && registry_pristine(), "the record is cleared and the registry pristine");
    lua.close(LC);
    check(!mapped(dll_a) && !mapped(dll_c), "and neither copy stays mapped");

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
        "check(h == nil and tostring(why):find('new needs hideui 0.10.0 or newer; the resident copy is ', 1, true) == 1"
        "  and copy_of(path) == 'd', 'new() through A to D, a 0.5.0 resident: no handle at all: ' .. tostring(why))\n"
        "local ok, why2, kind = hu.shutdown()\n"
        "check(ok == false and kind == 'handles', 'A\\'s shutdown reaches D, which refuses while D holds a handle')\n"
        "return finish()\n");
    lua.close(LA);
    phase(LD,
        "check(hd:hide('buff') == true and hd:release() == true and hu.shutdown() == true,"
        "  'D serves itself, releases and shuts down')\n"
        "return finish()\n");
    check(record_magic() == 0 && registry_pristine(), "the record is cleared and the registry pristine");
    lua.close(LD);
    check(!mapped(dll_a) && !mapped(dll_d), "and neither copy stays mapped");

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
        "check(h == nil and tostring(why):find('new needs hideui 0.10.0 or newer; the resident copy is ', 1, true) == 1"
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
    check(record_magic() == 0 && registry_pristine(), "the record is cleared and the registry pristine");
    lua.close(LE);
    check(!mapped(dll_a) && !mapped(dll_e), "and neither copy stays mapped");

    // -- F resident: its table is engine abi 5's, as a 0.7.0 to 0.8.0
    //    resident's is; a 0.10.0 copy makes no handle through it, the macro
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
        "check(h == nil and tostring(why):find('new needs hideui 0.10.0 or newer; the resident copy is ', 1, true) == 1"
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
    check(record_magic() == 0 && registry_pristine(), "the record is cleared and the registry pristine");
    lua.close(LF);
    check(!mapped(dll_a) && !mapped(dll_f), "and neither copy stays mapped");

    // -- G resident: its table is engine abi 6's, as a 0.9.0 resident's is;
    //    a 0.10.0 copy makes no handle through it, the category slots past
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
        "check(h == nil and tostring(why):find('new needs hideui 0.10.0 or newer; the resident copy is ', 1, true) == 1"
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
    check(record_magic() == 0 && registry_pristine(), "the record is cleared and the registry pristine");
    lua.close(LG);
    check(!mapped(dll_a) && !mapped(dll_g), "and neither copy stays mapped");

    InterlockedExchange(&g_stop, 1);
    WaitForSingleObject(game, 5000);
    CloseHandle(game);

    std::printf("%d of %d checks failed\n", g_failures, g_checks);
    return g_failures;
}
