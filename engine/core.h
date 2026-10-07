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
#include <string.h>

#include "hideui_engine_abi.h"

namespace hu {

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

const size_t kRowStride = 0x2C;
const size_t kRowSlot = 0x20;
const size_t kRowPolicy = 0x24;
const size_t kRowAvail = 0x28;
const size_t kRowLayer = 0x29;
const size_t kRowOverlap = 0x2A;

const uint8_t kHiddenLayer = 0x7F;

// The registry's dock bits. 0x10000000 is set at runtime by iteminfo alone
// and is never cleared or restored by the engine.
const uint32_t kDockMask = 0x1000 | 0x2000 | 0x4000;

inline char lower_ascii(char c) {
    return (c >= 'A' && c <= 'Z') ? static_cast<char>(c - 'A' + 'a') : c;
}

struct NameEntry {
    char name[9];
    char key[kKeyLen];      // the row's type + name, space-padded: the registry's own form
    int16_t rows[2];
    uint8_t row_count;
};

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

// Names that may be hidden but never blocked, or closed by the engine before
// the event has its answer: the event parser dereferences query's live menu
// with no null check. cancel('query') closes it once the answer is written,
// as the event's wait does on its next tick.
inline bool hide_only(const char* name) {
    return strcmp(name, "query") == 0;
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
// NUL) and no live resource key can equal.
const char kCompassName[] = "compass";

struct Inventory {
    NameEntry names[kMaxNames];
    int16_t name_of_row[kRowCount];
    int count;
    int16_t compass;        // the compass entry's index; -1 until built

    // Every record, whatever its type, then the compass; rows share an entry
    // only when type and name both match. Fails on an inventory the engine
    // cannot index, which only a broken menu_table.inc can produce: one name
    // under two types could not be told apart by name.
    bool build() {
        count = 0;
        compass = -1;
        for (int r = 0; r < kRowCount; ++r) {
            name_of_row[r] = -1;
            const char* tp = kRowSpecs[r].type;
            const char* nm = kRowSpecs[r].name;
            const size_t len = strlen(nm);
            if (strlen(tp) != 8 || len == 0 || len > 8) {
                return false;
            }
            char key[kKeyLen];
            memcpy(key, tp, 8);
            memset(key + 8, ' ', 8);
            memcpy(key + 8, nm, len);
            int n = find_exact(nm);
            if (n >= 0 && memcmp(names[n].key, key, kKeyLen) != 0) {
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
        if (count == 0 || count >= kMaxNames || find_exact(kCompassName) >= 0) {
            return false;
        }
        NameEntry& c = names[count];
        memset(&c, 0, sizeof(c));
        memcpy(c.name, kCompassName, strlen(kCompassName));
        compass = static_cast<int16_t>(count++);
        return true;
    }

    int find_exact(const char* nm) const {
        for (int n = 0; n < count; ++n) {
            if (strcmp(names[n].name, nm) == 0) {
                return n;
            }
        }
        return -1;
    }

    // A window by its registry name in any case; never a group name.
    int find_window(const char* text) const {
        char low[16];
        return lower_name(text, low) && strlen(low) <= 8 ? find_exact(low) : -1;
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
        if (len > 8) {
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
            if (memcmp(names[n].key, key, kKeyLen) == 0) {
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
// Holds: which handles hide or block which names. Lua-thread state; the game
// thread reads only the published want words.

const uint8_t kHoldHide = 1;
const uint8_t kHoldBlock = 2;
const LONG kWantHidden = 1;
const LONG kWantBlocked = 2;

struct Handle {
    uint32_t generation;
    uint8_t active;
    char name[48];
    uint8_t holds[kMaxNames];
    uint8_t macros_hold;    // this handle blocks the macro keys
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
        m.owner_slot = slot;
        m.owner_gen = gen;
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
};

inline const char* verb_name(int verb) {
    static const char* const names[] = {"", "move", "move_group", "reset", "reset_all", "open", "close",
                                        "resize", "answer", "cancel", "reset_group", "block", "block_macros"};
    return verb >= 0 && verb < static_cast<int>(sizeof(names) / sizeof(names[0])) ? names[verb] : "";
}

// Command flags. A reply is counted against its window until the drain has
// carried it out; a listed reply is checked again on the game thread against
// the list the player would pick from; a group move that may wait is kept
// for the anchor's next open when the anchor is closed; a reply naming its
// prompt is refused once another open of the window has come, or with
// kCmdSession once another invite or post-box session has; a move's x, y
// are the frame's top-left with kCmdFrame, else the origin.
const uint8_t kCmdReply = 1;
const uint8_t kCmdListed = 2;
const uint8_t kCmdWait = 4;
const uint8_t kCmdInstance = 8;
const uint8_t kCmdSession = 16;
const uint8_t kCmdFrame = 32;

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
// window and its row packed by pack_cursor.
enum EventType {
    kEvOpened = 1,
    kEvClosed = 2,
    kEvBlocked = 3,
    kEvCovered = 4,
    kEvUncovered = 5,
    kEvError = 6,
    kEvPending = 7,
    kEvCursor = 8,
};

const int kPendingInvite = 0;
const int kPendingPost = 1;

inline int pending_payload(int what, bool on) { return what << 1 | (on ? 1 : 0); }
inline int pending_what(int payload) { return payload >> 1; }
inline bool pending_on(int payload) { return (payload & 1) != 0; }

inline uint32_t pack_event(int type, int name) {
    return static_cast<uint32_t>(type & 0xFF) | (static_cast<uint32_t>(name & 0xFFFF) << 16);
}
inline int event_type(uint32_t e) { return static_cast<int>(e & 0xFF); }
inline int event_name(uint32_t e) { return static_cast<int>(e >> 16); }

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
const size_t kErrorReason = 200;

struct ErrorRecord {
    volatile LONG seq;      // position + 1 once written; 0 while being written
    int32_t slot;
    uint32_t gen;
    int32_t by_slot;        // the handle whose move replaced this one's, named when read; else -1
    uint32_t by_gen;
    char verb[16];
    char name[16];
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

// `glyphs`: `count` little-endian int16 codes, unaligned; a count past
// kGlyphMax reads kGlyphMax. `table` may be NULL or not yet built: every
// two-byte character is then a '?'.
inline void decode_glyphs(const uint8_t* glyphs, int count, const GlyphTable* table, GlyphText* out) {
    if (count > kGlyphMax) {
        count = kGlyphMax;
    }
    uint8_t escape = kColorTable;
    uint16_t color = kColorDefault;
    int length = 0;
    out->run_count = 0;
    out->undecoded = 0;
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
        if (g >= kGlyphTwoByte) {
            const uint16_t sjis = table ? table->lookup(g) : 0;
            if (sjis) {
                c[0] = static_cast<char>(sjis >> 8);
                c[1] = static_cast<char>(sjis & 0xFF);
                width = 2;
            } else {
                c[0] = '?';
                ++out->undecoded;
            }
        }
        GlyphRun* run = out->run_count > 0 ? &out->runs[out->run_count - 1] : NULL;
        if (!run || run->escape != escape || run->color != color) {
            run = &out->runs[out->run_count++];
            run->start = static_cast<uint8_t>(length);
            run->length = 0;
            run->escape = escape;
            run->color = color;
        }
        memcpy(out->text + length, c, width);
        length += width;
        run->length = static_cast<uint8_t>(run->length + width);
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
