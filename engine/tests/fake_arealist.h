// fake_arealist.h - the game's area list (arealist) for both stand-ins, as the
// 2026-05-10 image builds and drives it (its class at 0x1020E020..0x1020F020,
// the base list at 0x101F6B50..0x101F6C40): the controller's words; its rows,
// 0x54 bytes each, in a heap array every rebuild reallocates, each pointing
// at its {int32 id; u8 flag} entry; modes 0 (the search, whose top level
// opens on three rows of its own), 1 and 2 (an event's; mode 2 flat) and 4
// (another list, its Enter not modelled); the event's opener, which arms the
// latch and the mode before the open; close+reset (0x1020EA00), whose close
// by name (vtable +0x74) reaches the begin-close (vtable +0x08, 0x1020F020:
// latch and mode cleared) only through an open instance, then frees the
// rows; the base list's select-index; and the player's keys: the input
// routine (vtable +0x18), which hands Enter (5) and cancel (6) to the class's
// key handler (vtable +0x58), where a region row opens its zones and a
// cancel backs out of them. The engine calls close+reset and the latch clear
// alone; the input routine counts any call that reaches it.
//
// The world: 42 regions, region r holding r % 3 + 1 zones numbered from 1, the
// player in zone 4, the first of region 2's three. Mode 0's top level is then
// Current Area (4), Current Region (-1), All Areas (0) and the 42 regions
// (-2 .. -43): 45 rows.

#ifndef HIDEUI_FAKE_AREALIST_H_
#define HIDEUI_FAKE_AREALIST_H_

#include <windows.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

namespace fake_area {

const size_t kMenu = 0x08;
const size_t kVisible = 0x18;           // int16: the rows shown
const size_t kScroll = 0x1E;            // int16: the first row shown
const size_t kShown = 0x20;             // int16: the base list's row count
const size_t kMaxScroll = 0x22;
const size_t kCursorCopy = 0x24;
const size_t kViewRows = 0x38;
const size_t kCapacity = 0x50;
const size_t kFilled = 0x54;
const size_t kLevel = 0x58;
const size_t kSaved = 0x5A;             // int16: the top level's selection while inside a region
const size_t kGrouped = 0x5C;
const size_t kRows = 0x60;
const size_t kEntries = 0x64;
const size_t kMode = 0x6C;
const size_t kLatch = 0x6D;
const size_t kResult = 0x6E;
const size_t kMenuCursor = 0x4C;
const size_t kMenuItems = 0x58;
const int kRowBytes = 0x54;
const int kRowsAllocated = 0x102;
const int kRowsShown = 10;
const int kRegions = 42;
const int kZones = 84;
const int kCurrentZone = 4;
const int kModeOther = 4;
const int kOtherRows = 12;

struct Log {
    int inputs;             // the input routine's calls
    int last_input;         // its event
    int keys;               // the key handler's calls
    int searches;           // mode 0's player search
    int last_search;        // its zone, or -2 - region for a region's
    int closes;             // the close+reset
    int scsibori;           // the search window's close by name from the key handler
    int rebuilds;
    int selects;            // select-index calls the key handler made
    int latch_clears;       // the begin-close, through a close or called alone
};

typedef void (*CloseByNameFn)(const char* key16);
typedef void (*SelectFn)(uint8_t* ctl, int index, int row);
typedef uint32_t (__fastcall* KeyFn)(uint8_t* ctl, void* edx, int code, int param);
typedef uint32_t (__fastcall* CtlFn)(uint8_t* ctl, void* edx);

struct World {
    char regions[kRegions][16];
    char zones[kZones + 1][16];         // by zone id
    char tags[kZones + 1][8];           // mode 0's column 1, by zone id
    char others[kOtherRows][16];
    int first[kRegions];                // each region's first zone
    int count[kRegions];
};

static World g_world;
static Log g_log;
static CloseByNameFn g_close_by_name;
static SelectFn g_select;

inline int16_t r16(const uint8_t* p, size_t off) {
    int16_t v;
    memcpy(&v, p + off, 2);
    return v;
}
inline uint32_t r32(const uint8_t* p, size_t off) {
    uint32_t v;
    memcpy(&v, p + off, 4);
    return v;
}
inline uint8_t* rptr(const uint8_t* p, size_t off) {
    uint8_t* v;
    memcpy(&v, p + off, 4);
    return v;
}
inline void w16(uint8_t* p, size_t off, int v) {
    const int16_t s = static_cast<int16_t>(v);
    memcpy(p + off, &s, 2);
}
inline void w32(uint8_t* p, size_t off, uint32_t v) {
    memcpy(p + off, &v, 4);
}
inline void wptr(uint8_t* p, size_t off, const void* v) {
    memcpy(p + off, &v, 4);
}

inline void build_world() {
    int zone = 1;
    for (int r = 0; r < kRegions; ++r) {
        snprintf(g_world.regions[r], sizeof(g_world.regions[r]), "Region %02d", r + 1);
        g_world.first[r] = zone;
        g_world.count[r] = r % 3 + 1;
        zone += g_world.count[r];
    }
    for (int z = 1; z <= kZones; ++z) {
        snprintf(g_world.zones[z], sizeof(g_world.zones[z]), "Zone %03d", z);
        snprintf(g_world.tags[z], sizeof(g_world.tags[z]), "Z%d", z);
    }
    for (int i = 0; i < kOtherRows; ++i) {
        snprintf(g_world.others[i], sizeof(g_world.others[i]), "Choice %d", i + 1);
    }
}

// The model's game: its close by name, and the select-index its key handler
// calls (the stand-in's own routine, or select_index below).
inline void install(CloseByNameFn close_by_name, SelectFn select) {
    build_world();
    g_close_by_name = close_by_name;
    g_select = select;
}

inline int region_of(int zone) {
    for (int r = 0; r < kRegions; ++r) {
        if (zone >= g_world.first[r] && zone < g_world.first[r] + g_world.count[r]) {
            return r;
        }
    }
    return -1;
}

// One row: column 0 its text, column 1 `format` and `value`, and its entry.
inline void add_row(uint8_t* ctl, const char* text, uint8_t format, uint32_t value, int id, uint8_t flag) {
    uint8_t* rows = rptr(ctl, kRows);
    uint8_t* entries = rptr(ctl, kEntries);
    const int i = static_cast<int32_t>(r32(ctl, kFilled));
    uint8_t* row = rows + i * kRowBytes;
    uint8_t* entry = entries + i * 8;
    row[0] = 1;
    row[1] = format;
    for (int c = 0; c < 8; ++c) {
        w32(row, 8 + 4 * c, 0x80808080u);
    }
    wptr(row, 0x28, text);
    w32(row, 0x2C, value);
    w32(entry, 0, static_cast<uint32_t>(id));
    entry[4] = flag;
    wptr(row, 0x48, entry);
    w32(ctl, kFilled, static_cast<uint32_t>(i + 1));
}

// A zone: in modes 1 and 2 column 1 is drawn from the zone id (format 0x34),
// the entry's flag 0 in mode 1; otherwise it is text (format 0x21).
inline void add_zone(uint8_t* ctl, int zone) {
    const int mode = ctl[kMode];
    if (mode == 1 || mode == 2) {
        add_row(ctl, g_world.zones[zone], 0x34, static_cast<uint32_t>(zone), zone, mode == 1 ? 0 : 1);
    } else {
        add_row(ctl, g_world.zones[zone], 0x21, reinterpret_cast<uint32_t>(g_world.tags[zone]), zone, 0);
    }
}

// The three rows the search's top level opens on; Current Region only when
// the player's region holds more than one zone.
inline void add_specials(uint8_t* ctl) {
    static const char area[] = "Current Area";
    static const char region[] = "Current Region";
    static const char all[] = "All Areas";
    static const char beside_area[] = "here";
    static const char beside_region[] = "nearby";
    static const char beside_all[] = "everywhere";
    add_row(ctl, area, 0x21, reinterpret_cast<uint32_t>(beside_area), kCurrentZone, 0);
    if (g_world.count[region_of(kCurrentZone)] > 1) {
        add_row(ctl, region, 0x21, reinterpret_cast<uint32_t>(beside_region), -1, 0);
    }
    add_row(ctl, all, 0x21, reinterpret_cast<uint32_t>(beside_all), 0, 0);
}

// The base list's set-rows: the rows it shows and how far it scrolls.
inline void set_list(uint8_t* ctl, const uint8_t* rows, int count) {
    wptr(ctl, kViewRows, rows);
    w16(ctl, kShown, count);
    const int most = count - r16(ctl, kVisible);
    w16(ctl, kMaxScroll, most < 0 ? 0 : most);
    if (r16(ctl, kScroll) > r16(ctl, kMaxScroll)) {
        w16(ctl, kScroll, r16(ctl, kMaxScroll));
    }
}

// The array and the entries freed; the filled count stays, as the game's
// close leaves it.
inline void free_rows(uint8_t* ctl) {
    void* rows = rptr(ctl, kRows);
    void* entries = rptr(ctl, kEntries);
    if (rows) {
        HeapFree(GetProcessHeap(), 0, rows);
    }
    if (entries) {
        HeapFree(GetProcessHeap(), 0, entries);
    }
    w32(ctl, kRows, 0);
    w32(ctl, kEntries, 0);
}

// The rebuild: a new array before the old one goes, so every rebuild moves
// it, then the rows the mode, the grouping and the level make.
inline void rebuild(uint8_t* ctl) {
    ++g_log.rebuilds;
    set_list(ctl, NULL, 0);
    uint8_t* rows = static_cast<uint8_t*>(HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, kRowsAllocated * kRowBytes));
    uint8_t* entries = static_cast<uint8_t*>(HeapAlloc(GetProcessHeap(), HEAP_ZERO_MEMORY, kRowsAllocated * 8));
    free_rows(ctl);
    wptr(ctl, kRows, rows);
    wptr(ctl, kEntries, entries);
    w32(ctl, kCapacity, kRowsAllocated);
    w32(ctl, kFilled, 0);
    const int mode = ctl[kMode];
    const int level = r16(ctl, kLevel);
    const bool event = mode == 1 || mode == 2;
    if (mode == kModeOther) {
        for (int i = 0; i < kOtherRows; ++i) {
            add_row(ctl, g_world.others[i], 0x21, reinterpret_cast<uint32_t>(g_world.others[i]), i, 0);
        }
    } else if (ctl[kGrouped] && mode != 2 && mode != 3 && level >= 2 && level - 2 < kRegions) {
        const int r = level - 2;
        for (int z = g_world.first[r]; z < g_world.first[r] + g_world.count[r]; ++z) {
            add_zone(ctl, z);
        }
    } else if (ctl[kGrouped] && mode != 2 && mode != 3) {
        if (!event) {
            add_specials(ctl);
        }
        for (int r = 0; r < kRegions; ++r) {
            add_row(ctl, g_world.regions[r], 0x09, static_cast<uint32_t>(g_world.count[r]), -2 - r, 0);
        }
    } else {
        if (!event) {
            add_specials(ctl);
        }
        for (int z = 1; z <= kZones; ++z) {
            add_zone(ctl, z);
        }
    }
    set_list(ctl, rows, static_cast<int32_t>(r32(ctl, kFilled)));
}

// SetCursor: the row clamped to 1..the template's rows.
inline void set_cursor(uint8_t* menu, int row) {
    const int items = r16(menu, kMenuItems);
    int at = row < 1 ? 1 : row;
    at = at > items ? items : at;
    w16(menu, kMenuCursor, at);
}

// The base list's select-index (0x101F6BB0), as C: the first row shown
// clamped to put `index` in view, then the cursor, worked out from the two
// when `row` is not a row on screen.
inline void select_index(uint8_t* ctl, int index, int row) {
    const int most = r16(ctl, kMaxScroll);
    if (index >= 0) {
        w16(ctl, kScroll, index > most ? most : index);
    }
    uint8_t* menu = rptr(ctl, kMenu);
    if (!menu) {
        return;
    }
    if (row >= 1) {
        if (row <= r16(ctl, kVisible)) {
            set_cursor(menu, row);
        }
    } else {
        set_cursor(menu, index > most ? index - most + 1 : 1);
    }
    w16(ctl, kCursorCopy, r16(menu, kMenuCursor));
}

// The row under the cursor, 0-based: the menu's row less one, plus the
// first row shown.
inline int selected(const uint8_t* ctl) {
    const uint8_t* menu = rptr(ctl, kMenu);
    return menu ? r16(menu, kMenuCursor) - 1 + r16(ctl, kScroll) : 0;
}

inline const uint8_t* selected_entry(const uint8_t* ctl) {
    const int i = selected(ctl);
    const uint8_t* rows = rptr(ctl, kRows);
    if (!rows || i < 0 || i >= static_cast<int32_t>(r32(ctl, kCapacity))) {
        return NULL;
    }
    return rptr(rows + i * kRowBytes, 0x48);
}

// The begin-close (vtable +0x08), which is also the latch clear: the latch
// and the mode cleared, nothing else.
inline uint32_t __fastcall latch_clear(uint8_t* ctl, void*) {
    ++g_log.latch_clears;
    ctl[kLatch] = 0;
    ctl[kMode] = 0;
    return 0;
}

// vtable +0x74: the list's close by name.
inline uint32_t __fastcall close_by_name(uint8_t*, void*) {
    g_close_by_name("menu    arealist");
    return 0;
}

// The close+reset: the close by name through the vtable, whose begin-close
// runs only on an open instance, then the rows gone.
inline uint32_t __fastcall close_reset(uint8_t* ctl, void* = NULL) {
    ++g_log.closes;
    const uint8_t* vt = rptr(ctl, 0);
    CtlFn close;
    memcpy(&close, vt + 0x74, sizeof(close));
    close(ctl, NULL);
    set_list(ctl, NULL, 0);
    free_rows(ctl);
    return 0;
}

// The key handler (vtable +0x58). Enter on a region opens its zones; on any
// other row it writes the result (zones and All Areas), searches in mode 0,
// then closes the list and the search window. Cancel at the top closes the
// list; inside a region it backs out to the top, the selection where it was.
inline uint32_t __fastcall key(uint8_t* ctl, void*, int code, int) {
    ++g_log.keys;
    const uint8_t* entry = selected_entry(ctl);
    if (!entry) {
        return 0;
    }
    if (code == 6) {
        if (r16(ctl, kLevel) == 0) {
            close_reset(ctl);
            return 1;
        }
        w16(ctl, kLevel, 0);
        rebuild(ctl);
        ++g_log.selects;
        g_select(ctl, r16(ctl, kSaved), 0);
        return 1;
    }
    const int mode = ctl[kMode];
    if (code != 5 || mode == kModeOther) {
        return 1;
    }
    const int id = static_cast<int32_t>(r32(entry, 0));
    if (id <= -2) {
        w16(ctl, kLevel, -id);
        w16(ctl, kSaved, selected(ctl));
        rebuild(ctl);
        ++g_log.selects;
        g_select(ctl, 0, 0);
        return 1;
    }
    if (id >= 0) {
        w16(ctl, kResult, id);
    }
    if (mode == 0 || mode == 3) {
        ++g_log.searches;
        g_log.last_search = id >= 0 ? id : -2 - region_of(kCurrentZone);
    }
    if (mode == 3) {
        return 1;
    }
    close_reset(ctl);
    ++g_log.scsibori;
    g_close_by_name("menu    scsibori");
    return 1;
}

// The base list's input routine (vtable +0x18) for Enter and cancel: the
// cursor pulled back onto the last row when it is past it, then the key
// handler through the vtable.
inline uint32_t __fastcall input(uint8_t* ctl, void*, int event, int row) {
    ++g_log.inputs;
    g_log.last_input = event;
    if (event != 5 && event != 6) {
        return 0;
    }
    uint8_t* menu = rptr(ctl, kMenu);
    const int shown = r16(ctl, kShown);
    if (menu && selected(ctl) >= shown) {
        set_cursor(menu, shown - r16(ctl, kScroll));
        w16(ctl, kCursorCopy, r16(menu, kMenuCursor));
    }
    const uint8_t* vt = rptr(ctl, 0);
    KeyFn handler;
    memcpy(&handler, vt + 0x58, sizeof(handler));
    return handler(ctl, NULL, event, row) & 0xFF;
}

// Puts the class's begin-close, input routine, key handler and close by
// name into a vtable.
inline void fill_vtable(const void** vt) {
    vt[0x08 / 4] = reinterpret_cast<const void*>(&latch_clear);
    vt[0x18 / 4] = reinterpret_cast<const void*>(&input);
    vt[0x58 / 4] = reinterpret_cast<const void*>(&key);
    vt[0x74 / 4] = reinterpret_cast<const void*>(&close_by_name);
}

// The event's opener before its open by name: the mode and the latch armed.
// The result keeps whatever it held: only the open hook, which a refused
// open never runs, writes -1.
inline void arm(uint8_t* ctl, int mode) {
    ctl[kMode] = static_cast<uint8_t>(mode);
    ctl[kLatch] = 1;
}

// An open of the list up to the open by name, which the stand-in makes: the
// mode and the grouping, the latch armed and the result -1 as the open hook
// leaves them, the top level built.
inline void prepare(uint8_t* ctl, int mode, bool grouped) {
    ctl[kMode] = static_cast<uint8_t>(mode);
    ctl[kGrouped] = grouped ? 1 : 0;
    w16(ctl, kLevel, 0);
    w16(ctl, kSaved, 0);
    w16(ctl, kScroll, 0);
    w16(ctl, kVisible, kRowsShown);
    ctl[kLatch] = 1;
    w16(ctl, kResult, -1);
    rebuild(ctl);
}

// Once the open has bound the menu: the template's rows, the cursor on the
// first row.
inline void opened(uint8_t* ctl) {
    uint8_t* menu = rptr(ctl, kMenu);
    if (menu) {
        w16(menu, kMenuItems, kRowsShown);
        g_select(ctl, 0, 0);
    }
}

}  // namespace fake_area

#endif  // HIDEUI_FAKE_AREALIST_H_
