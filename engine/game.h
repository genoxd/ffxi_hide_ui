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
typedef void (__cdecl* YesNoCallbackFn)(void* context, int answer);
// The cursor onto a row, clamped to 1..+0x58, with no input event.
typedef void (__fastcall* SelectRowFn)(void* menu, void* edx, int row);
// The yes/no class's ask, `this` its controller: al 0 at once while a
// question is pending, or when the open of its key is refused. Every
// argument is the dword its callers push.
typedef uint32_t (__fastcall* YesNoAskFn)(void* ctl, void* edx, const void* callback, int32_t context,
                                         const char* text, int32_t default_yes, int32_t help, int32_t cancellable,
                                         int32_t mode);
// What picks the KO window's question (al), and the game's message by
// category and id.
typedef uint32_t (__cdecl* DeadDeciderFn)(int32_t first, int32_t second);
typedef const char* (__cdecl* MessageFn)(int32_t category, int32_t id);
// A number box's confirm, `this` its controller: the callback with the
// number, the question cleared, the window closed when it has one.
typedef void (__fastcall* BoxConfirmFn)(void* ctl, void* edx, int32_t value);
// Close by instance, `this` the manager: nothing for a NULL menu.
typedef void (__fastcall* CloseInstanceFn)(void* mcb, void* edx, void* menu, int flag);
// The Asked windows' callbacks and the routines their replies call.
typedef void (__cdecl* SortynCallbackFn)(void* context, int answer);
typedef void (__cdecl* EvitemCallbackFn)(int answer);
typedef void (__cdecl* RestoreFn)(int which);
typedef void (__cdecl* BankCallbackFn)(int container, int canceled);
typedef void (__cdecl* MaplistCallbackFn)(const void* entry);
// maplist's list's cursor put on a row, `this` its controller; the second
// argument its key handler passes is always 0.
typedef void (__fastcall* ListSelectFn)(void* ctl, void* edx, int32_t row, int32_t flag);
// The map's close, `this` mapframe's controller; one dword, whose low byte
// map0's cancel passes 0.
typedef void (__fastcall* MapCloseFn)(void* frame, void* edx, int32_t flag);
typedef void (__cdecl* OkCallbackFn)(void* context);
// mogdoor's: its confirm's gate (al), the word its floor row reads and the
// condition its draw picks a text by (both with no arguments), its tick
// (`this` the controller; one dword it never reads), and the door actor's
// open (`this` the actor; one dword).
typedef uint32_t (__cdecl* DoorGateFn)(int32_t first, int32_t second);
typedef uint32_t (__cdecl* DoorReadFn)();
typedef void (__fastcall* DoorTickFn)(void* ctl, void* edx, int32_t unused);
typedef void (__fastcall* DoorActorFn)(void* actor, void* edx, int32_t arg);
// The NPC dialogue boxes' text dispatch, `this` a text object: the code to
// it, then to its twin; the chat log's release of a page's lines, `this` the
// log.
typedef void (__fastcall* TextDispatchFn)(void* text, void* edx, int32_t code);
typedef void (__fastcall* LogReleaseFn)(void* log, void* edx, uint32_t line);

// The text-entry windows' routines. A field's end, `this` the window's
// controller: stringdl's takes nothing, the others whether to keep what the
// field holds; a start opens a field: comment's on one of its lines,
// msgline's and passinpu's on their one. Focus, `this` the manager,
// raises a menu. The comment line's sanitize and trim, cdecl. msgline's: the
// POL interface's number for a word (`this` the interface), the clock's
// text, the senders (cdecl, 0 when queued), and the mark the sending
// message's indicator takes (`this` the indicator). scname's: the search
// criteria cleared and named (cdecl), and the search started (`this` the
// search). link11's (cdecl but the selection, `this` link5's controller):
// whether the second linkshell slot is taken, link5's selected row's
// record, the name's encoding, the request (al 1 when queued) and the item
// record a container and index name.
typedef void (__fastcall* FieldStopFn)(void* ctl, void* edx);
typedef void (__fastcall* FieldEndFn)(void* ctl, void* edx, int32_t keep);
typedef void (__fastcall* FieldLineFn)(void* ctl, void* edx, int32_t line);
typedef void (__fastcall* FocusFn)(void* mcb, void* edx, void* menu, int32_t flag);
typedef int32_t (__cdecl* SanitizeFn)(char* out, const char* line);
typedef int32_t (__cdecl* TrimFn)(char* line);
typedef uint32_t (__fastcall* PolNumberFn)(void* pol, void* edx, uint32_t word);
typedef void (__cdecl* ClockTextFn)(int32_t what, char* out, int32_t size);
typedef int32_t (__cdecl* Send7Fn)(uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t);
typedef int32_t (__cdecl* Send8Fn)(uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t,
                                   uintptr_t);
typedef int32_t (__cdecl* Send9Fn)(uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t, uintptr_t,
                                   uintptr_t, uintptr_t);
typedef void (__fastcall* MarkFn)(void* indicator, void* edx, uint32_t flags);
typedef void (__cdecl* CriteriaFn)(void* criteria);
typedef void (__cdecl* CriteriaNameFn)(void* criteria, const char* name);
typedef void (__fastcall* SearchFn)(void* search, void* edx, int32_t a, void* criteria, int32_t b, int32_t c,
                                    int32_t d, int32_t f);
typedef uint32_t (__cdecl* SlotTakenFn)(int32_t a, int32_t b, int32_t c, int32_t d);
typedef const uint8_t* (__fastcall* SelectedFn)(void* list, void* edx);
typedef const char* (__cdecl* EncodeFn)(const char* name);
typedef uint32_t (__cdecl* CreateFn)(uint32_t slot, uint32_t container, uint32_t index, uint32_t one,
                                     const char* encoded, uint32_t r, uint32_t g, uint32_t b);
typedef const uint32_t* (__cdecl* ItemFn)(uint32_t container, uint32_t index);

// The trade's and the NPC trade's routines. A stage, `this` the window's
// controller, puts an item, or the gil (index 0, id kGilId), into the
// window's offer, each argument the dword its callers push; the trade's
// cancel takes one dword it never reads and answers al, like the OKs. The
// player's data's routines: the game's clock in milliseconds, an item record
// by container and index, an item's data from the item cache (`this` the
// cache) and its description (cdecl); an inventor mode's gray test of a row,
// `this` the mode, the row's color in eax.
typedef void (__fastcall* StageFn)(void* ctl, void* edx, uint32_t index, uint32_t id, uint32_t count);
typedef uint32_t (__fastcall* TradeCancelFn)(void* ctl, void* edx, int32_t unused);
typedef uint32_t (__cdecl* ClockFn)();
typedef uint8_t* (__cdecl* ItemRecordFn)(uint32_t container, uint32_t index);
typedef uint8_t* (__fastcall* ItemDataFn)(void* cache, void* edx, uint32_t id);
typedef uint32_t (__cdecl* DescribeFn)(const uint8_t* data);
typedef uint32_t (__fastcall* GrayTestFn)(void* mode, void* edx, const uint8_t* record, uint32_t description);

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
const size_t kAbilityCategory = 0x64;   // ability's controller, int32: the list its window shows; 1 job abilities, 2 pet commands, 3 weapon skills (20 in Monstrosity), 4 job traits
// The yes/no class's controller, shared by the resyn family. Its ask keeps
// no default but the cursor row it opens the window on.
const size_t kYesNoCallback = 0x14;     // cb(context, answer), cdecl, 1 yes and 0 no or cancel; set while a question is pending
const size_t kYesNoContext = 0x18;
const size_t kYesNoText = 0x1C;         // the question, the game's own text, NUL-terminated
const size_t kYesNoKey = 0x20;          // the registry key it opens, 16 bytes, then a NUL
const size_t kYesNoCancellable = 0x39;
const size_t kYesNoChained = 0x3A;      // set by a question asked again from inside the callback
const size_t kYesNoChainedYes = 0x3B;   // that question's default is yes
const size_t kYesNoMode = 0x40;         // int32: 0, or 1..3 for a question confirmed by holding through holdtime
const size_t kYesNoBytes = 0x44;
const int kRowYes = 1;
const int kRowNo = 2;
// The KO window's controller (dead). Its tick keeps the countdown; the
// countdown run out, or a yes to its question, sets the byte the game's KO
// handler sends the player home on.
const size_t kDeadDeadline = 0x1C;      // the game clock's seconds when the countdown ends
const size_t kDeadRemaining = 0x20;     // int32: (deadline - now) * kDeadTicks, the countdown the window draws
const size_t kDeadChosen = 0x24;        // the home point chosen
const size_t kDeadBytes = 0x28;
const int32_t kDeadTicks = 60;          // kDeadRemaining's units in a second
// A number box's controller (core.h's kBoxNames). Its setup writes the
// question, then opens the window; its confirm hands the callback the
// number and clears it. Nothing else clears it: a close from outside leaves
// a callback set with no question behind it.
struct BoxLayout {
    size_t callback;        // cb(value, context), cdecl
    size_t context;
    size_t value;           // the number the window shows
    size_t max;
    uint8_t width;          // value's and max's bytes, signed
    int32_t min;
    size_t bytes;
};
const BoxLayout kBoxLayouts[kBoxCount] = {
    {0x2C, 0x30, 0x28, 0x24, 4, 0, 0x38},     // moneyctr: a max of 0 asked for is the gil the player has
    {0x24, 0x2C, 0x23, 0x22, 1, 1, 0x30},     // itemctrl: a max of 1 asked for answers itself, with no window
};
const int32_t kBoxCancel = -1;          // what the window's own cancel hands its confirm
// The fields of the windows that ask through a number box which the
// question waits on.
const size_t kTradeState = 0xC0;        // trade's controller, int32: kTradeOpen while its offer takes changes
const int32_t kTradeOpen = 3;
const size_t kHandoverRow = 0x40;       // handover's controller, u8: the row a picked item goes to
// The trade's controller. Its offer goes to the server one change at a
// time, and the game's own table of it in the player's data is written only
// by the server's acknowledgment; the partner's, only by the server.
const size_t kTradePartner = 0x6C;      // the partner's index in the actor table; 0 for none
const size_t kTradePartnerId = 0x70;    // and its id; 0 for none
const size_t kTradeTheirState = 0xC4;   // int32: kTradeOkSent once the partner's OK is in
const size_t kTradeBytes = 0xC8;
const int32_t kTradeOkSent = 4;
// The NPC trade's controller: its offer, local until its OK sends it whole.
const size_t kHandoverEntries = 0x14;   // kOfferTable x {u16 id, u8 index, u8 count}; 0 the gil's at the OK
const size_t kHandoverEntry = 4;
const size_t kHandoverGil = 0x3C;       // u32
const size_t kHandoverNpc = 0x44;       // the NPC's index in the actor table; 0 for none
const size_t kHandoverNpcId = 0x48;
const size_t kHandoverBytes = 0x4C;
// The offers in the player's data: the trade's own, kOfferTable x {u32
// count, u16 id at +4, u8 index at +6}, 0 the gil's; the partner's,
// kOfferTable x {u16 id, u8 slot at +2, u32 count at +4}, 0 the gil's; and the
// inventory's item records, 0 the gil's: {u16 id, u8 index at +2, u32 count
// at +4, u8 status at +8}.
const int kOfferTable = 10;
const int kOfferSlots = 8;              // the slots the windows' rows offer, 1..8
const size_t kOwnEntry = 8;
const size_t kOwnId = 4;
const size_t kOwnIndex = 6;
const size_t kTheirEntry = 0x2C;
const size_t kItemRecordBytes = 0x2C;
const size_t kItemCount = 4;
const size_t kItemStatus = 8;
const int kInventoryRecords = 81;
const uint32_t kGilId = 0xFFFF;
// The packets the game sends one of a second: a change to the trade's
// offer, and the NPC trade.
const uint32_t kSendTradeChange = 0x34;
const uint32_t kSendHandover = 0x36;
// The inventory list's row colors: listed, grayed out, offered already, and
// a row the pick refuses with a message of its own.
const uint32_t kRowListed = 0x80808080u;
const uint32_t kRowGray = 0x80404040u;
const uint32_t kRowOffered = 0x80804020u;
const uint32_t kRowRefused = 0x80602020u;
const int kStatusClasses = 0x1C;        // the statuses the list's rule names; any above them grays the row
const uint8_t kStatusListed = 0;
const uint8_t kStatusModal = 1;         // listed for one mode alone
const size_t kActorNameBytes = 32;
const size_t kModeGrayTest = 0x30;      // an inventor mode's vtable: its gray test of a row
const size_t kClockSeconds = 0x0C;
const size_t kClockMs = 0x10;
const int32_t kDeliveryIdle = 9;        // the outgoing box between steps
const int32_t kDeliveryGil = 0x0A;      // the outgoing box while its gil question waits
// The Asked windows' controllers (core.h's kAskedNames). sortyn's setup
// writes its question, then opens the window; its confirm and cancel call
// the callback and clear it, and nothing else clears it.
const size_t kSortynCallback = 0x14;    // cb(context, answer), cdecl, 1 yes and 0 no or cancel
const size_t kSortynContext = 0x18;
const size_t kSortynText = 0x1C;        // the question, the game's text, shown on the help line
const size_t kSortynTitle = 0x20;       // its title
const size_t kSortynBytes = 0x28;
// evitem01's ask opens the window, then writes the question and the
// callback, which nothing clears; the key item list it asks from is evitem.
const size_t kEvitemCallback = 0x64;    // cb(answer), cdecl, the answer a byte: 1 yes, 0 no or cancel
const size_t kEvitemCloseFlag = 0x70;   // what its confirm hands close by instance
const size_t kEvitemText = 0x74;        // the question, the game's text
const size_t kEvitemTextBytes = 0x100;
const size_t kEvitemBytes = 0x174;
const size_t kEvitemRow = 0x30;         // evitem's controller, int32: the key item under its cursor, an index
const size_t kEvitemListBytes = 0x94;
const size_t kConfynMode = 0x14;        // int32: 0 or 1, the defaults a yes restores less one
const size_t kConfynBytes = 0x18;
const size_t kBankCallback = 0x14;      // cb(container, canceled), cdecl; set from the ask to the window's close
const size_t kBankBytes = 0x18;
const int32_t kBankSafe = 1;            // the containers its rows offer, row 1 then row 2
const int32_t kBankSafe2 = 9;
// maplist's controller, the base list's layout (arealist's): rows of
// kAreaRowStride bytes, each with its entry at kAreaRowEntry, {int32 id;
// int32 map}: a zone's map when the id is 0 or more, a list of its own when
// below 0, the list's closing row at kMapCloseRow. A list row's step down
// writes -id to kMapRegion and the entry's map to kMapZone: the top's
// region rows hold -1 there, a region's rows for a zone of several maps 0.
const size_t kMapCapacity = 0x50;       // int32
const size_t kMapCount = 0x54;          // int32: the rows filled
const size_t kMapRegion = 0x58;         // int16: 0 the top, else the list shown, 2 + its region or zone
const size_t kMapSelection = 0x5A;      // int16: the row under the cursor at the last step down
const size_t kMapZone = 0x5C;           // int16: 0 or more while it lists one zone's maps, else -1
const size_t kMapBack = 0x5E;           // int16: the kMapRegion a zone's maps go back to
const size_t kMapRows = 0x60;           // freed and cleared by close+reset
const size_t kMapCallback = 0x6C;       // cb(entry), cdecl
const size_t kMapBytes = 0x70;
const int kMapRowMax = 0x202;
const int32_t kMapCloseRow = 0x7FFFFFFF;
// The GM tell queue: kGmTellSlots slots, each the GM's name, then the
// question's title as a pointer; gmtell's controller is the query class's.
const size_t kGmTellSlotBytes = 0x140;
const size_t kGmTellNameBytes = 0x18;
const size_t kGmTellTitle = 0x18;
const int kGmTellSlots = 4;
const size_t kMpDeadDeadline = 0x1C;    // the game clock's seconds when the countdown ends
const size_t kMpDeadRemaining = 0x20;   // int32: (deadline - now) * kDeadTicks
const size_t kMpDeadChoice = 0x24;      // int32: 0 until a row is confirmed, then 1 or 2
const size_t kMpDeadBytes = 0x28;
const size_t kItemRowId = 0x3A;         // a list item's (type 1) int16 row
const size_t kItemRowText = 0x4C;       // the text its window's open hook gives that row
const size_t kOkCallback = 0x14;        // cb(context), cdecl; NULL for the error screen's notice
const size_t kOkContext = 0x18;
const size_t kOkText = 0x1C;            // the notice, the game's text
const size_t kOkTextBytes = 0x230;
const size_t kOkCloses = 0x258;         // the confirm closes the window itself
const size_t kOkAnswered = 0x25C;       // int32: 1 once acknowledged, what the game waits on
const size_t kOkBytes = 0x260;
// mogdoor's controller. The door's handler writes the nation and +0x1B,
// then opens the window; its open hook puts the nation's list of areas at
// +0x1C, its count at +0x16 and the top level up; its tick enables the
// floor and Mog Garden rows each frame; its confirm writes the exit, which
// the door actor sends once it has opened.
const size_t kDoorNation = 0x14;        // u8: the nation whose areas the list holds, 1..the table's last
const size_t kDoorExit = 0x15;          // s8: the exit the confirm picked; below 0 none
const size_t kDoorCount = 0x16;         // s8: the list's rows
const size_t kDoorLevel = 0x18;         // u8: 0 the top, 1 the list of areas
const size_t kDoorGarden = 0x19;        // u8: the Mog Garden row of the list is enabled
const size_t kDoorFloor = 0x1A;         // u8: the floor row of the top is enabled
const size_t kDoorKept = 0x1B;          // u8: what the door's handler writes beside the nation
const size_t kDoorList = 0x1C;          // the list: u16 zone ids, a 0 after the last
const size_t kDoorBytes = 0x20;
const size_t kHouseDoor = 0x38;         // the Mog House object's door actor
const size_t kDoorOpened = 0x880;       // the door actor's routine run once the door has opened
const int kDoorTop = 0;
const int kDoorAreas = 1;
const int kDoorRows = 4;                // the top's: stay, exit, the other floor, the list of areas
const int kDoorListMax = 16;
const int32_t kDoorBack = 0;            // the exit to the area the player came from
const int32_t kDoorFloorUp = 0x7D;      // the floor row's from the first floor; 0x7E from the second
const int32_t kDoorGardenExit = 0x7F;   // the Mog Garden row's; any other row of the list writes its place
// The texts mogdoor's draw shows: the top's from the game's menu messages,
// the list's from its area names, by zone.
const int32_t kDoorTextCategory = 9;
const int32_t kDoorTitleText = 0x45;
const int32_t kDoorStayText = 0xB8;
const int32_t kDoorBackText = 0x46;     // while the draw's condition holds; kDoorBackTextOther else
const int32_t kDoorBackTextOther = 0xF1;
const int32_t kDoorFloorText = 0x125;
const int32_t kDoorListText = 0xB9;
const int32_t kAreaNameCategory = 0x14;
const size_t kDoorTextBytes = 64;
// An NPC dialogue box's controller (core.h's kDialogueNames) and its text
// object, which holds the messages the event sends, the one shown first,
// and that message's page flags and timer. Each page the text object shows
// adds its lines to the box's list; the page shown last is the last
// kBoxPageLines lines of it.
const size_t kBoxLines = 0x14;          // a list: +0 head, +4 tail, +0x10 count
const size_t kBoxModal = 0x48;          // u8, rem4line's: while set the walk draws no other menu but those exempt
const size_t kBoxLinesTail = 0x18;
const size_t kBoxLineCount = 0x24;      // int32
const size_t kBoxPageLines = 0x4E;      // int16: the lines of the page shown last
const size_t kBoxLogLine = 0x50;        // what the cancel hands the chat log's release
const size_t kBoxAuto = 0x60;           // int16: while set, the tick confirms each waiting page itself
const size_t kBoxLog = 0x64;            // the chat log's controller; its kLogObject the log
const size_t kBoxText = 0x68;           // the text object
const size_t kBoxBytes = 0x70;
const size_t kLogObject = 0x18;
const size_t kTextQueued = 0x10;        // int32: the messages held, the one shown among them
const size_t kTextCurrent = 0x1C;       // the message shown; NULL for none
const size_t kTextFlags = 0x20;         // the page's waits: kPageKey, kPageTimed
const size_t kTextTimer = 0x28;         // int64: the time a timed page has left, kDeadTicks a second
const size_t kTextBytes = 0x38;
const uint32_t kPageKey = 1;
const uint32_t kPageTimed = 2;
const size_t kMessageMode = 0x04;
const size_t kMessageText = 0x40;       // the text, its pages ended by 7F escapes
const size_t kMessageNext = 0x828;      // where the next page starts; NULL, or its end, at the last
const size_t kMessageBytes = 0x844;
const size_t kNodeNext = 0x00;          // a list node's links, its item and its removed mark
const size_t kNodePrev = 0x04;
const size_t kNodeItem = 0x10;
const size_t kNodeRemoved = 0x14;
const size_t kLineGlyphCodes = 0x04;    // a box line's int16 glyph codes
const size_t kLineCount = 0x208;        // u16: its code count
const size_t kLineBytes = 0x20A;
const int kDialogueAdvance = 5;         // the code the text dispatch advances a waiting page on
const uint8_t kWaitAborted = 2;         // what the cancel leaves the event's wait
// The text-entry windows' controllers. stringdl's: what the outgoing box
// reads, the name and the reply word, while its mode asks the name (the
// box's kDeliveryAsksName) or a yes or no on a slot (kDeliveryAsksSlot).
const size_t kStringdlMode = 0x14;      // s8
const size_t kStringdlName = 0x58;      // char[kStringdlNameBytes]
const size_t kStringdlNameBytes = 0x7F;
const size_t kStringdlPrompt = 0x118;   // the line shown over the field, char[kStringdlPromptBytes]
const size_t kStringdlPromptBytes = 0x40;
const size_t kStringdlReply = 0x15C;    // u16: 0 until a reply
const size_t kStringdlField = 0x160;    // its field, for good
const size_t kStringdlEditing = 0x164;  // u8
const size_t kStringdlBytes = 0x170;
const int kStringdlAsksName = 1;
const int kStringdlAsksSlot = 2;
const int kStringdlNamed = 1;
const int kStringdlCanceled = 2;
const int kStringdlYes = 3;
const int kStringdlNo = 4;
const int kDeliveryAsksName = 6;
const int kDeliveryAsksSlot = 0x10;
// comment's: the line its field edits, the message's kind (its slot of
// lines), the search's category, and the lines, slot s's line l at
// kCommentSlots + (s * 3 + l) * kCommentLineBytes.
const size_t kCommentEditing = 0x14;    // s8: 0..2, the line; else none
const size_t kCommentMode = 0x16;       // s8: 0 bazaar, 1 another's (read only), 2 search, 3 linkshell
const size_t kCommentCategory = 0x17;
const size_t kCommentSlots = 0x1A;
const size_t kCommentLineBytes = 0x29;
const size_t kCommentField = 0x378;
const size_t kCommentBytes = 0x37C;
const int kCommentView = 1;
// msgline's: its mode (which message, set by the opener before the open),
// its field, the text it sends, the recipient's name and the words the
// senders take.
const size_t kMsglineMode = 0x14;       // s16
const size_t kMsglineField = 0x1C;
const size_t kMsglineText = 0x53;       // char[kMsglineLong]
const size_t kMsglineTextEnd = 0x17E;   // the byte the commit zeroes
const size_t kMsglineName = 0x188;      // char[16]
const size_t kMsglineNameBytes = 16;
const size_t kMsglineWord1 = 0x198;
const size_t kMsglineWord2 = 0x19C;
const size_t kMsglineIds = 0x1D0;       // three words
const size_t kMsglineIds2 = 0x1E0;      // two words
const size_t kMsglineBytes = 0x210;
const size_t kPolWord = 0x80;           // the POL interface's word its number reads
const size_t kSendingMark = 0x20;       // the sending indicator, in its owner
// scname's: its field and the name the search takes; the search's criteria.
const size_t kScnameField = 0x14;
const size_t kScnameName = 0x18;        // char[16]
const size_t kScnameBytes = 0x28;
const size_t kSearchCriteria = 0x5C;
// link11's: the field being edited (0 none, 1 the name, 2..4 a color), the
// name's field, the colors and the name; link5's flag the request sets.
const size_t kLink11Editing = 0x14;
const size_t kLink11Field = 0x2C;
const size_t kLink11Colors = 0x30;
const size_t kLink11Name = 0x34;        // char[20]
const size_t kLink11Bytes = 0x48;
const size_t kLink5Made = 0x65;
// passinpu's field.
const size_t kPassField = 0x38;
const size_t kPassBytes = 0x3C;

const size_t kMcbUiW = 0x80;
const size_t kMcbUiH = 0x82;
const size_t kMcbActive = 0x54;
const size_t kMcbHideAll = 0xB4;        // u8: 1 keeps every menu off the screen but those kPolicyDrawnAlways exempts
const size_t kMcbBytes = 0xB8;
const uint32_t kPolicyDrawnAlways = 0x40;
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
// The target window's controller, as its draw reads it: past the window's
// content, an arrow at +0xBC/+0xBE unless that x is 0, and while the player
// picks a command's target (+0xB8) a second at +0xC0/+0xC2, both in screen
// pixels; nothing of them while +0x65 is set, which shows other content in
// their place. The per-menu draw runs a controller's draw only while the
// menu's +0x6A is set and it is not dormant.
const size_t kTargetOther = 0x65;       // u8
const size_t kTargetPicking = 0xB8;     // u8
const size_t kTargetArrowX = 0xBC;      // int16
const size_t kTargetArrowY = 0xBE;
const size_t kTargetPickX = 0xC0;       // int16
const size_t kTargetPickY = 0xC2;
const size_t kTargetBytes = 0xC4;
const size_t kMenuDraws = 0x6A;         // u8
// The macro key object, reached only through the game's global pointer. The
// handler records the bar it has up and the set its keys dispatch to; its
// own close of a bar is the bar closed by name, then +0x0C and +0x18 zeroed,
// +0x0D left alone.
const size_t kMacroBar = 0x0C;          // u8: 0 none, 1 the Ctrl bar, 2 the Alt bar up
const size_t kMacroSet = 0x0D;          // u8: the set the number keys dispatch to
const size_t kMacroBarUp = 0x18;        // u8: set while a bar is up
const size_t kMacroObjectBytes = 0x24;  // the constructor's allocation

const uint32_t kStateDormant = 0x0E;
const int kQueryCanceled = 0xFF;
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

// A string the game owns, copied up to `size` - 1 bytes; a page is checked
// before its first byte is read.
inline void copy_text(const char* p, char* out, size_t size) {
    size_t i = 0;
    for (; p && i + 1 < size; ++i) {
        const bool page_start = i == 0 || (reinterpret_cast<uintptr_t>(p + i) & 0xFFF) == 0;
        if ((page_start && !readable(p + i, 1)) || p[i] == '\0') {
            break;
        }
        out[i] = p[i];
    }
    out[i] = '\0';
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

// The KO window's confirm as its input handler makes it, every argument read
// out of the handler's own bytes: the decider picks one of two messages for
// the question, which the yes/no ask puts to the player through comyn, the
// controller in the handler's global.
struct DeadConfirm {
    DeadDeciderFn decider;
    int32_t decider_first;
    int32_t decider_second;
    MessageFn message;
    int32_t category;
    int32_t text_if_set;            // the message's id when the decider answers 1
    int32_t text_otherwise;
    const void* callback;
    int32_t context;
    int32_t default_yes;
    int32_t help;
    int32_t cancellable;
    int32_t mode;
};

// What mogdoor's reader and replies use, read out of the game's code by
// signatures.h's resolve_asked(); the routines never run or called are kept
// for status().
struct DoorSites {
    const uint8_t* input;           // its confirm's gate and rows, and the exit after them
    const void* vtable;
    const uint8_t* open_hook;
    const uint8_t* draw;
    const uint8_t* handler;         // the door's: the nation written, then the open
    DoorGateFn gate;
    int32_t gate_first;
    int32_t gate_second;
    DoorReadFn floor;               // nonzero on the Mog House's second floor
    uint32_t floor_word;            // the u16 it reads in the player's data
    DoorReadFn condition;           // the draw's: the back row's text
    DoorTickFn tick;
    uint8_t** house;                // the global holding the Mog House object
    const void* opened;             // what the exit puts at the door actor's kDoorOpened
    DoorActorFn open_door;
    uint8_t* exiting[3];            // the bytes the exit sets to 1, the first the door's own test
    const uint8_t* table;           // each nation's list of areas, a pointer each, by nation
    int32_t nations;                // the last nation the open hook takes
    int32_t garden;                 // the Mog Garden's zone in a list
};

// What the NPC dialogue boxes' replies, reader and left-click hook use,
// read out of the game's code by signatures.h's resolve_dialogue(); the
// routines never run or called, and the globals only the game reads, are
// kept for status().
struct DialogueSites {
    const uint8_t* input;           // the boxes' input routine
    const uint8_t* handler;         // what it hands each code: the dispatch, then the cancel
    TextDispatchFn dispatch;
    uint8_t** texts[2];             // the globals naming the boxes' text objects, which it hands each code between
    LogReleaseFn release;
    ControllerFn clear;             // a text object's messages dropped, its box closed
    uint8_t* wait;                  // the event's wait flag
    const uint8_t* tick;            // the boxes' tick
    uintptr_t click_return;         // where the tick's left-button test returns
};

// What the text-entry windows' replies, readers and field ends use, read
// out of the game's code by signatures.h's resolve_text(); the routines
// resolved only to check what the game does are kept for status().
struct TextSites {
    const uint8_t* stringdl_input;
    const uint8_t* stringdl_name;       // its handler while it asks the name
    const uint8_t* stringdl_slot;       // and while it asks about a slot
    FieldStopFn stringdl_stop;
    FocusFn focus;
    const uint8_t* comment_input;
    const void* comment_vtable;
    const uint8_t* comment_close;       // its begin-close
    FieldEndFn comment_stop;
    FieldLineFn comment_start;          // the one its open hook calls
    SanitizeFn sanitize;
    TrimFn trim;
    const uint8_t* msgline_input;
    FieldEndFn msgline_stop;
    FieldStopFn msgline_start;          // the one its open hook calls
    uint8_t** player;                   // the global the commit tests before it sends
    uint8_t** pol;                      // the global holding the POL interface
    PolNumberFn pol_number;
    ClockTextFn clock_text;
    Send7Fn send_tell;                  // mode 0
    Send7Fn send_reply;                 // mode 1
    Send7Fn send_short;                 // mode 2
    Send8Fn send_answer;                // mode 5
    Send9Fn send_dated;                 // mode 7
    const void* sent[2];                // the callbacks the senders take: modes 0 to 2, then 5 and 7
    uint8_t** sending;                  // the global holding the sending indicator's owner
    MarkFn mark;
    uint8_t* busy;                      // set while a message is on its way
    const uint8_t* scname_input;
    FieldEndFn scname_stop;
    uint8_t** search;                   // the global holding the player search
    CriteriaFn criteria_clear;
    CriteriaNameFn criteria_name;
    SearchFn search_start;
    const uint8_t* link11_input;
    FieldEndFn link11_stop;
    SlotTakenFn slot_taken;
    SelectedFn selected;
    EncodeFn encode;
    CreateFn create;
    ItemFn item;
    uint8_t* made[4];                   // what a queued request writes: its slot, the item's word, 1 and 0x3C
    const uint8_t* passinpu_input;
    FieldEndFn passinpu_stop;
    FieldStopFn passinpu_start;
};

// What the Asked windows' replies and readers use, each read out of the
// game's code by signatures.h's resolve_asked(); the routines resolved only
// to check what the game does with each field are kept for status().
struct AskedSites {
    const uint8_t* sortyn_setup;
    const uint8_t* sortyn_confirm;
    const uint8_t* evitem_ask;
    const uint8_t* evitem_setup;
    EvitemCallbackFn evitem_callback;   // what every ask of evitem01 sets as its callback
    const uint8_t* evitem_input;
    const uint8_t* confyn_input;
    RestoreFn confyn_restore;
    const uint8_t* bank_open;
    ControllerFn bank_clear;            // bankmenu's begin-close: its callback cleared
    uint8_t** bank_data;                // the global holding the player's data
    uint32_t bank_safe2;                // the Mog Safe 2 flag's offset in that data, bit 0
    const uint8_t* bank_input;
    ControllerFn maplist_close;         // close+reset
    ControllerFn maplist_rebuild;       // the rows built again for the level kMapRegion and kMapZone name
    ListSelectFn maplist_select;
    ControllerFn maplist_cursor;        // the row under the list's cursor, 0-based; -1 with no window
    const uint8_t* map0_input;
    MapCloseFn map_close;
    const uint8_t* gm_poll;
    const int32_t* gm_slot;             // the GM tell queue's current slot
    const uint8_t* gm_queue;            // its first slot
    const uint8_t* mp_dead_input;
    const uint8_t* ok_input;
    DoorSites door;
};

// What the trade's and the NPC trade's replies and readers use, read out of
// the game's code by signatures.h's resolve_offer(); the routines resolved
// only for what they tell of the game's fields are kept for status(). The
// windows by core.h's kOffer* index.
struct OfferSites {
    const uint8_t* trade_input;
    const uint8_t* trade_open;          // the trade's open, its three windows with it
    const uint8_t* handover_input;
    const uint8_t* rows;                // the inventory list's build of a row, around its mode's gray test
    const uint8_t* send;                // a change to the trade's offer sent
    const uint8_t* allocator;           // the packet allocator and its throttle
    StageFn stage[kOfferCount];
    ControllerFn ok[kOfferCount];       // al 1 once the OK is on its way
    TradeCancelFn trade_cancel;         // al 1 once the cancel is on its way
    ControllerFn trade_close;           // the trade's windows closed, `this` the trade's controller
    ControllerFn handover_close;
    const uint8_t* trade_rows;          // the trade's cursor row to its offer slot, 0 the gil's, 0xFF none
    uint8_t** mode[kOfferCount];        // the global holding each window's inventor mode
    uint8_t** player;                   // the global holding the player's data
    uint32_t own;                       // the trade's own offer in it, as acknowledged
    uint32_t theirs;                    // the partner's
    uint32_t inventory;                 // the inventory's item records
    uint32_t last_send;                 // the throttle's ms clock at each packet's last send, by id
    uint8_t** intervals;                // the global holding the throttle's intervals
    uint32_t interval;                  // their words there, by id
    uint32_t packets;                   // the ids the throttle takes, below this
    ClockFn clock;
    uint8_t** clock_object;             // its global: seconds at kClockSeconds, ms at kClockMs
    ItemRecordFn item;
    uint8_t** cache;                    // the global holding the item cache
    ItemDataFn item_data;
    DescribeFn describe;
    uint32_t lock;                      // the byte of an item's data locked around the test
    uint8_t status_class[kStatusClasses];
    uint8_t** status_mode;              // the global holding the mode that lists kStatusModal's
    const uint8_t* const* actors;       // the actor table
    uint32_t actor_titled;              // int16: a titled name at actor_title, else the name at actor_name
    uint32_t actor_title;
    uint32_t actor_name;
};

// The game's openers open_set and a list's open run, read out of its code by
// signatures.h's resolve_openers(): the equipment set's (`this` the
// controller in equip's registry slot, no arguments), the check the
// equipment shortcut makes before it (al 1 in Monstrosity), and the magic
// list's (category, sort, exclusion mask; cdecl). The ability list's is the
// hooked site's own, Game::ability_list. The others are each run as the
// player's own menu row, command or item use runs it: the map's (mode;
// cdecl), the blue magic set's and the automaton's (`this` the controller
// in bluequip's and automato's slot), the linkshell list's (`this` link5's
// controller; mode and two more, 0 from the main menu) behind the check
// its row asks first (1, 1; al 1 refuses), the instincts' (no arguments),
// the synthesis's (`this` tskill1's controller; the crystal's inventory
// index and id, and mode 0; al 0 refuses) behind the check the item use
// asks first (0x77, 0; al 0 refuses), the maze editor's (`this`
// prvdungp's controller; mode, the tabula's inventory index), and the
// macro editor's (`this` the editor object its global holds).
typedef void (__fastcall* SetOpenFn)(void* ctl, void* edx);
typedef uint32_t (__cdecl* CheckFn)();
typedef void (__cdecl* ListOpenFn)(int32_t category, int32_t sort, int32_t extra);
typedef void (__cdecl* MapOpenFn)(int32_t mode);
typedef uint32_t (__cdecl* RowCheckFn)(int32_t first, int32_t second);
typedef void (__fastcall* LinkshellOpenFn)(void* ctl, void* edx, int32_t mode, int32_t a, int32_t b);
typedef void (__cdecl* PlainFn)();
typedef uint32_t (__fastcall* SynthesisOpenFn)(void* ctl, void* edx, uint32_t index, uint32_t id, uint32_t mode);
typedef void (__fastcall* TabulaOpenFn)(void* ctl, void* edx, uint32_t mode, uint32_t index);

struct OpenerSites {
    SetOpenFn equipment;
    CheckFn monstrosity;
    ListOpenFn magic;
    const uint8_t* shortcut;            // never run: the equipment shortcut, resolved for its check
    MapOpenFn map;
    SetOpenFn set_spells;
    SetOpenFn attachments;
    RowCheckFn linkshell_check;
    LinkshellOpenFn linkshell;
    const uint8_t* linkshell_row;       // never run: the main menu's linkshell row, resolved for both
    PlainFn instincts;
    const uint8_t* item_use;            // never run: the item menu's use, resolved for what it runs
    uint8_t** item_cache;               // the global holding the item cache
    ItemDataFn item_data;               // (this = the cache; id): the item's data, NULL for none
    uint32_t item_type;                 // the item data's pointer to its type word
    RowCheckFn use_check;
    SynthesisOpenFn synthesis;
    uint32_t inventor_list;             // inventor's controller's list, which the synthesis's cancel reopens it with
    const uint8_t* code;                // the game's code, where a list of its own has its routines
    size_t code_size;
    TabulaOpenFn tabula;
    const uint8_t* macro_edit;          // never run: the macro set menu's edit row, resolved for what it runs
    uint8_t** macro_editor;             // the global holding the macro editor object
    SetOpenFn macros;
};

// What kRoutes' routes and the sets past macros run, read out of the
// game's code by signatures.h's resolve_routes(): each opener a menu row,
// command or list of the game's runs, `this` the controller in its window's
// registry slot unless noted, and each field the game's own route writes
// before an open, by its offset in that controller.
typedef void (__fastcall* ValueFn)(void* self, void* edx, uint32_t value);
typedef void (__fastcall* Value2Fn)(void* self, void* edx, int32_t a, int32_t b);
typedef uint32_t (__fastcall* Value3Fn)(void* self, void* edx, int32_t a, int32_t b, int32_t c);
typedef void (__cdecl* MountOpenFn)(int32_t list, int32_t sort);
typedef uint32_t (__stdcall* JobTestFn)(int32_t job);
typedef const char* (__cdecl* LanguageKeyFn)(int32_t which);
typedef uint32_t (__cdecl* ExpansionFn)(int32_t bit);
typedef const int32_t* (__cdecl* TrackingFn)();
typedef void (__fastcall* ItemPanelFn)(void* ctl, void* edx, int32_t mode, int32_t id, int32_t slot, int32_t bag,
                                       const void* extra, const void* beside, const void* under);
typedef uint32_t (__cdecl* BagSizeFn)(int32_t bag);
typedef void (__cdecl* VoidFn)();

// cnqframe's Escape (RouteSites::region_escape): with the closing bit of its
// controller's word kRegionClosingWord clear, the fade's start through
// vtable kRegionFadeSlot, then kRegionEscapeCloses closes by name, each
// kRegionEscapeStep bytes from kRegionEscapeAt; the fade's end closes the
// frame itself.
const size_t kRegionClosingWord = 0x14;
const uint16_t kRegionClosing = 0x40;
const size_t kRegionFadeSlot = 0x50;
const size_t kRegionEscapeAt = 0x1B;
const size_t kRegionEscapeStep = 15;
const int kRegionEscapeCloses = 13;

struct RouteSites {
    ValueFn storage;                // (mode): storage's 0, storage2's 1
    ValueFn mogpost;                // (mode)
    uint8_t** mog_house;            // the global holding the Mog House's manager, NULL outside it
    uint8_t** nomad;                // the global holding the Nomad Moogle service's, NULL where there is none
    uint32_t job_row;               // jobcselu's: the row the job change's confirm reads, 1 the main job
    LanguageKeyFn language_key;     // (6): the job list's key on this client
    ValueFn friend_list;            // (0)
    Value3Fn msglist;               // (0, 0, 0); al 0 refuses
    SetOpenFn blklist;
    MountOpenFn mount;              // (1, sort), cdecl
    JobTestFn job_points_test;      // (job): al 1 for a job the list offers
    ValueFn job_points;             // (job)
    ValueFn fp_cat2;                // (category)
    ValueFn search_reset;           // (0), `this` scresult's controller
    uint32_t search_mode;           // scresult's: 1 or 2 once a search has run
    ValueFn comgenre;               // (mode)
    Value2Fn comment_sub[3];        // sccompar's, sccomite's and sccomls's (mode, base)
    ItemPanelFn item_panel;         // (mode, id, slot, bag, extra, beside, under)
    uint32_t item_extra;            // an item record's extra data, which the panel is handed
    BagSizeFn bag_size;             // (bag): al its slots, slot 0 counted, as the game's lists go up to
    Value2Fn timer[2];              // casttime's and eventtim's (kind, ticks)
    ValueFn mp_stat;                // (mode)
    uint32_t persona_layout;        // persona's layout byte
    uint32_t conf11l_set;
    uint32_t quest00_mode;
    ControllerFn quest00_dim[2];    // quest00's confirm's calls on quest00's controller before quest01's open
    ValueFn quest01;                // (list), after quest01's open
    ValueFn miss00_menu;            // (row value): 0 the player's nation's log
    ExpansionFn expansion;          // (bit): al 1 when the expansion is on
    const uint8_t* mission_bits;    // each log's expansion bit
    uint32_t miss00_log;
    TrackingFn tracking;            // NULL, or the map's tracking, 1 while it tracks
    SetOpenFn scanlist;
    const void* scan_question;      // the callback the Wide Scan row asks comyn with while the map tracks
    uint8_t** player;               // the global holding the player's data
    uint32_t area;                  // the player's area in it
    Value2Fn region;                // (kind, area)
    uint32_t region_kind;           // cnqframe's
    uint32_t coloresu_region;
    uint32_t textcol[3];            // textcol1's, textcol2's and textcol3's category
    ValueFn macro_pick;             // (row), `this` the macro editor
    ValueFn macro_switch;           // (row), `this` the macro editor: the other palette, its window opened
    uint32_t macro_palette;         // the macro editor's: 1 Ctrl, 2 Alt
    ControllerFn macro_close;       // `this` the macro editor: mcrbedit, mcr1edit and mcr2edit closed by name
    Value2Fn title;                 // (0, 2), `this` titlewin's controller
    const uint8_t* status_close;    // never run: statcom2's begin-close, which closes persona and mp_stat
    const uint8_t* textcol_escape;  // never run: textcol1's Escape, textcol2 closed by name, then textcol1
    VoidFn instincts_escape;        // mp_mncst's Escape: mp_stat, mp_cstm and mp_mncst closed by name
    const uint8_t* region_escape;   // never run: cnqframe's input, its Escape kRegionEscapeCloses closes by
                                    // name after its fade's start
    Value2Fn items_view;            // (0, 1), `this` the object the global items_object holds
    uint8_t** items_object;
    uint32_t inventor_items;        // inventor's byte the main menu's Items row sets 1
    uint8_t** inventor_policy;      // the global holding the list it opens inventor with
    ValueFn inventor_open;          // (that list)
    uint32_t summary_type;          // the type word in the summary offer.item_data answers
};

// The game's config store and the routines the settings' pages set through,
// read out of the game's code by signatures.h's resolve_settings(), which
// checks every offset below against the store's own code. The store keeps
// one entry of kEntryBytes per id from kStoreEntries; its loaded byte is 1
// once the player's values are loaded, as the player enters the game.
const size_t kStoreEntries = 0x04;
const size_t kStoreLoaded = 0x09;
const size_t kEntryBytes = 0x2C;
const size_t kEntryId = 0x04;
const size_t kEntryValue = 0x08;
const size_t kEntryMin = 0x18;
const size_t kEntryMax = 0x1C;      // -1: the store takes any value
const size_t kEntryDefault = 0x20;
const int32_t kFlagAnonymous = 0;   // the player flag conf3win's own field sets
const int32_t kLockOff = 0;         // the equipment lock's request modes as conf12wi asks them
const int32_t kLockKept = 1;
const int32_t kLockOn = 4;
const int32_t kIdleSecondsPerUnit = 60;

typedef int32_t (__cdecl* StoreSetFn)(int32_t id, int32_t value);
typedef uint32_t (__cdecl* LockFn)(int32_t mode, int32_t a, int32_t b);
typedef void (__cdecl* IdleFn)(int32_t seconds);
typedef uint32_t (__cdecl* FlagSetFn)(int32_t flag, int32_t on);

// What each setting reads, from kSettings and, where a table of the game's
// gives it, from there: its store word, its bit and its chat filter row.
struct SettingSlot {
    uint8_t id;
    int8_t bit;
    FilterRow row;
};

struct SettingSites {
    uint8_t** store;                // the global holding the config store
    StoreSetFn set;                 // (id, value): the value stored and the id's subscribers run, -1 for no change
    const uint8_t* method;          // never run: the store's set, resolved for its entry lookup and range check
    const uint8_t* save;            // never run: the store's save, resolved for its test of the loaded byte
    LockFn lock;                    // (mode, 0, 0): the equipment lock's request; al it went, or 1 as its wait refuses
    ClockFn lock_clock;             // the game's clock in milliseconds, the request's wait's
    const uint8_t* lock_stamp;      // the global the request stamps with that clock as it goes on to send
    uint32_t lock_wait;             // the milliseconds the request waits from that stamp
    IdleFn idle;                    // (seconds): the idle timer, 0 off
    FlagSetFn flag;                 // (flag, on): the request that sets a player flag the server keeps
    uint8_t** player;               // the global holding the player's data, where the flags are
    uint32_t flags;                 // their byte's offset in it
    uint8_t anonymous;              // anonymous's bit in that byte
    const uint8_t* filter_rows;     // the chat filter page's row table
    const uint8_t* effect_rows;     // the battle effects filter page's
    const uint8_t* routing[4];      // the chat log routing page's four groups
    const uint8_t* colors[4];       // the text colors' four categories
    const uint8_t* keys[2];         // k1assign's and k2assign's rows
    SettingSlot slot[kSettingCount];
};

// The routines that ask through a number box, each known by the callback it
// asks with (signatures.h's kAskerSites, in this order).
enum {
    kAskDeliveryGil,
    kAskHandoverGil,
    kAskTradeGil,
    kAskAuctionBid,
    kAskAuctionSell,
    kAskBazaarPrice,
    kAskDeliveryItem,
    kAskHandoverItem,
    kAskTradeItem,
    kAskStorage,
    kAskBazaarBuy,
    kAskSynthesis,
    kAskGuildBuy,
    kAskGuildSell,
    kAskShopBuy,
    kAskShopSell,
    kAskerCount
};

const int32_t kFieldAtAsk = -1;

// What a question blocked from its window still depends on. `owner` is the
// window the asker works in: the question goes with its open instance (the
// asker's own close, or the game's, takes the question down, and the
// context points into what that window holds). An answer stages where the
// owner's cursor is (`row`) or reads the owner's live window unchecked
// (`window`); `field`, a word of the owner's controller, must hold
// `field_value` (kFieldAtAsk: what it held at the ask), its change ending
// the question (`field_ends`) or refusing answers until it is back.
// `blocked`, when set, refuses every answer while the question is kept from
// its window: what its context names can be replaced under it with nothing
// that shows it.
struct AskerSpec {
    const char* name;
    uint8_t box;
    const char* owner;
    uint8_t item;           // the context is an item record, its first u16 the item's id
    uint8_t row;
    uint8_t window;
    uint16_t field;
    uint8_t field_bytes;
    int32_t field_value;
    uint8_t field_ends;
    const char* moved;      // the refusal while the field is elsewhere
    const char* blocked;
};

const char kTradeMoved[] = "the trade's offer is confirmed; it takes no changes now";
const char kBidBlocked[] = "the auction's bid takes no number while its window is blocked: a new search can replace"
                           " the item it asks about";

const AskerSpec kAskers[kAskerCount] = {
    {"delivery", kBoxMoney, "delivery", 0, 1, 0, kDeliveryState, 2, kDeliveryGil, 1, NULL, NULL},
    {"handover", kBoxMoney, "handover", 0, 0, 0, 0, 0, 0, 0, NULL, NULL},
    {"trade", kBoxMoney, "trade", 0, 1, 1, kTradeState, 4, kTradeOpen, 0, kTradeMoved, NULL},
    {"auction_bid", kBoxMoney, "auc1", 0, 0, 0, 0, 0, 0, 0, NULL, kBidBlocked},
    {"auction_sell", kBoxMoney, "auc1", 0, 0, 0, 0, 0, 0, 0, NULL, NULL},
    {"bazaar_price", kBoxMoney, "shop", 0, 0, 0, 0, 0, 0, 0, NULL, NULL},
    {"delivery", kBoxItem, "delivery", 1, 1, 0, kDeliveryState, 2, kDeliveryIdle, 0,
     "the outgoing box is busy with another step", NULL},
    {"handover", kBoxItem, "handover", 1, 0, 0, kHandoverRow, 1, kFieldAtAsk, 0,
     "handover's cursor is not on the row the item was picked for", NULL},
    {"trade", kBoxItem, "trade", 1, 1, 1, kTradeState, 4, kTradeOpen, 0, kTradeMoved, NULL},
    {"mog_storage", kBoxItem, "bank", 0, 0, 0, 0, 0, 0, 0, NULL, NULL},
    {"bazaar_buy", kBoxItem, "shop", 0, 0, 0, 0, 0, 0, 0, NULL, NULL},
    {"synthesis", kBoxItem, "tskill1", 1, 1, 1, 0, 0, 0, 0, NULL, NULL},
    {"guild_buy", kBoxItem, "shop", 0, 0, 0, 0, 0, 0, 0, NULL, NULL},
    {"guild_sell", kBoxItem, "shop", 0, 0, 0, 0, 0, 0, 0, NULL, NULL},
    {"shop_buy", kBoxItem, "shop", 0, 0, 0, 0, 0, 0, 0, NULL, NULL},
    {"shop_sell", kBoxItem, "shop", 0, 0, 0, 0, 0, 0, 0, NULL, NULL},
};

// A number box's question whose open a block refused, taken from the
// controller at that open: every setup writes its fields before it opens.
// Written by the game thread alone; the Lua thread reads a copy through the
// sequence word.
struct BoxRecord {
    volatile LONG seq;          // odd while the game thread writes
    uint8_t active;
    int8_t asker;               // kAskers' index; -1 for a callback no known asker asks with
    int16_t owner_n;            // the asker's window, -1 for none
    uint32_t id;                // the box's open count at that open, the prompt's id
    const uint8_t* ctl;
    const void* callback;
    const void* context;
    int32_t max;                // an itemctrl ask that answers itself rewrites it, and nothing else does
    const uint8_t* owner;       // the asker's window open then; NULL when it was not
    int16_t row;                // its cursor row then
    int32_t field;              // what the asker's field must hold

    // A copy from another thread. False when the writer kept it busy.
    bool read(BoxRecord* out) const {
        for (int attempt = 0; attempt < 64; ++attempt) {
            const LONG before = seq;
            if (before & 1) {
                continue;
            }
            MemoryBarrier();
            out->active = active;
            out->asker = asker;
            out->owner_n = owner_n;
            out->id = id;
            out->ctl = ctl;
            out->callback = callback;
            out->context = context;
            out->max = max;
            out->owner = owner;
            out->row = row;
            out->field = field;
            MemoryBarrier();
            if (seq == before) {
                out->seq = before;
                return true;
            }
        }
        return false;
    }
};

// The question of an Asked window that keeps one when its open is refused
// (asked_records()), taken at that open: the callback and context its setup
// has written (sortyn, ok), or the rows its setup has built (maplist); the
// window that asked, the active menu then (sortyn, evitem01, maplist); and
// for evitem01, whose setup runs after its open, the row of the key item
// list it asks about. Written by the game thread alone; the Lua thread
// reads a copy through the sequence word.
struct AskedRecord {
    volatile LONG seq;          // odd while the game thread writes
    uint8_t active;
    int16_t owner_n;            // -1 for none
    uint32_t id;                // the window's open count at that open, the prompt's id
    const uint8_t* ctl;
    const void* callback;
    const void* context;
    const uint8_t* owner;
    int32_t row;

    // A copy from another thread. False when the writer kept it busy.
    bool read(AskedRecord* out) const {
        for (int attempt = 0; attempt < 64; ++attempt) {
            const LONG before = seq;
            if (before & 1) {
                continue;
            }
            MemoryBarrier();
            out->active = active;
            out->owner_n = owner_n;
            out->id = id;
            out->ctl = ctl;
            out->callback = callback;
            out->context = context;
            out->owner = owner;
            out->row = row;
            MemoryBarrier();
            if (seq == before) {
                out->seq = before;
                return true;
            }
        }
        return false;
    }
};

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
    uintptr_t arrow_return[2];      // where the target window's draw of each arrow returns to
    uint8_t** macro_object;         // the game's global: the macro key object, or NULL
    const void* yesno_vtable;       // the yes/no class's: a controller the replies may call into
    SelectRowFn select_row;
    const uint8_t* yesno_confirm;   // never called: resolved for the controller fields it uses
    const uint8_t* dead_tick;       // never called: resolved for the KO window's countdown
    YesNoAskFn yesno_ask;
    DeadConfirm dead;
    const uint8_t* yesno_cancel;    // never called: the class's cancel, resolved for its test of kYesNoCancellable
    BoxConfirmFn box_confirm[kBoxCount];
    const uint8_t* box_setup[kBoxCount];    // never called: resolved for the fields each writes before its open
    const void* askers[kAskerCount];        // each asker's callback
    CloseInstanceFn close_instance;
    AskedSites asked;
    DialogueSites dialogue;
    TextSites text;
    OfferSites offer;
    ListOpenFn ability_list;        // the ability opener, called through its hooked site
    OpenerSites openers;
    RouteSites routes;
    SettingSites settings;
};

// msgline's message whose open a block refused, taken at that open: the
// opener has set its mode and the words its sender takes, which stand until
// another message is set up. Written by the game thread alone; the Lua
// thread reads a copy through the sequence word.
struct TextRecord {
    volatile LONG seq;          // odd while the game thread writes
    uint8_t active;
    uint32_t id;                // the window's open count at that open, the prompt's id
    const uint8_t* ctl;
    int16_t mode;

    // A copy from another thread. False when the writer kept it busy.
    bool read(TextRecord* out) const {
        for (int attempt = 0; attempt < 64; ++attempt) {
            const LONG before = seq;
            if (before & 1) {
                continue;
            }
            MemoryBarrier();
            out->active = active;
            out->id = id;
            out->ctl = ctl;
            out->mode = mode;
            MemoryBarrier();
            if (seq == before) {
                out->seq = before;
                return true;
            }
        }
        return false;
    }
};

// The texts mogdoor's draw shows for one prompt, read with the game's own
// message getter on the game thread: the title, the top's rows in order and
// the list's areas. The Lua thread reads a copy through the sequence word.
struct DoorTexts {
    volatile LONG seq;          // odd while the game thread writes
    uint32_t id;                // the prompt they are for
    uint8_t ready;
    char title[kDoorTextBytes];
    char rows[kDoorRows][kDoorTextBytes];
    char areas[kDoorListMax][kDoorTextBytes];

    // A copy from another thread. False when the writer kept it busy.
    bool read(DoorTexts* out) const {
        for (int attempt = 0; attempt < 64; ++attempt) {
            const LONG before = seq;
            if (before & 1) {
                continue;
            }
            MemoryBarrier();
            out->id = id;
            out->ready = ready;
            memcpy(out->title, title, sizeof(title));
            memcpy(out->rows, rows, sizeof(rows));
            memcpy(out->areas, areas, sizeof(areas));
            MemoryBarrier();
            if (seq == before) {
                out->seq = before;
                return true;
            }
        }
        return false;
    }
};

// A dialogue box's page as the drain last saw it: the message shown, where
// its next page starts, the messages held, the box's lines and its opens.
struct PageSignal {
    const uint8_t* message;
    const uint8_t* next;
    int32_t queued;
    int32_t lines;
    LONG opens;
    uint8_t seen;
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

// One slot of an offer: the item's inventory index and id and the count, or
// the gil in slot 0 (id kGilId); id 0 for an empty slot. `pending`: sent by
// this engine and not acknowledged by the server yet.
struct OfferEntry {
    int16_t slot;
    uint8_t index;
    uint8_t pending;
    uint16_t id;
    uint32_t count;
};

// The items this engine offered in the trade that the server has not
// acknowledged yet: the game's own table of the offer shows one only once
// the acknowledgment writes it. Written by the game thread alone; the Lua
// thread reads a copy through the sequence word.
struct TradePending {
    volatile LONG seq;          // odd while the game thread writes
    uint8_t count;
    OfferEntry entry[kOfferSlots];
    uint32_t instance;          // the trade window's open they were sent in
    uint32_t partner;           // and the partner's id then

    // A copy from another thread. False when the writer kept it busy.
    bool read(TradePending* out) const {
        for (int attempt = 0; attempt < 64; ++attempt) {
            const LONG before = seq;
            if (before & 1) {
                continue;
            }
            MemoryBarrier();
            out->count = count;
            memcpy(out->entry, entry, sizeof(entry));
            out->instance = instance;
            out->partner = partner;
            MemoryBarrier();
            if (seq == before) {
                out->seq = before;
                return true;
            }
        }
        return false;
    }
};

// What the game's gray test (game.h's offer_gray()) says of an inventory
// item for an offer window.
const uint8_t kGrayNone = 0;            // no item at the index
const uint8_t kGrayTakes = 1;           // the list lets its row be picked
const uint8_t kGrayGrayed = 2;          // the list grays its row out
const uint8_t kGrayUnlisted = 3;        // the list has no row for it
const uint8_t kGrayNoData = 4;          // the cache has no data for it, so no row
const uint8_t kGrayNoList = 5;          // the window's list is not set up

// The gray test of every inventory item for the open offer window `which`,
// by index, with the item's id it ran on; `open` false with none open.
struct GrayRows {
    bool open;
    int32_t which;
    uint32_t instance;          // that window's open it ran in
    uint16_t id[kInventoryRecords];
    uint8_t code[kInventoryRecords];
};

// GrayRows as the drain last wrote it. Written by the game thread alone; the
// Lua thread reads a copy through the sequence word.
struct GrayTable {
    volatile LONG seq;          // odd while the game thread writes
    GrayRows rows;

    // A copy from another thread. False when the writer kept it busy.
    bool read(GrayRows* out) const {
        for (int attempt = 0; attempt < 64; ++attempt) {
            const LONG before = seq;
            if (before & 1) {
                continue;
            }
            MemoryBarrier();
            memcpy(out, &rows, sizeof(*out));
            MemoryBarrier();
            if (seq == before) {
                return true;
            }
        }
        return false;
    }
};

// The live menus whose draws paint nothing, each with the window it is an
// instance of: a slot for each hidden window that has an instance. The drain
// and the open's post write it on the game thread; the draw's pre reads it
// without a lock for every menu drawn, so a slot's name is written before
// its menu, and its menu cleared before the slot is given up.
struct HiddenDraws {
    const uint8_t* volatile menu[kMaxNames];
    volatile int16_t name[kMaxNames];
    volatile LONG used;                 // the slots a reader scans: [0, used)
    int16_t slot_plus_one[kMaxNames];   // each window's slot + 1, 0 for none: game thread only
};

// How a window is placed, read off its frame at the engine's first touch and
// kept until the engine forgets the window.
const LONG kAnchorUnknown = 0;
const LONG kAnchorTop = 1;
const LONG kAnchorBottom = 2;

// A set or a list the engine opened, which the game can close again in the
// frame after the open, its own per-frame undoing it: the window to see
// still open at each of the next `frames` drains, its open count after the
// opener ran, and the command that opened it, for the error.
struct OpenCheck {
    uint8_t active;
    uint8_t frames;
    uint8_t verb;
    uint8_t set;                // kSet* + 1 for open_set, 0 for a list's open
    int16_t n;
    int32_t slot;
    uint32_t gen;
    LONG instance;
};
const int kOpenChecks = 8;
const uint8_t kOpenCheckFrames = 2;

const size_t kLabelBytes = 64;

struct SettingLabels {
    volatile LONG ready;
    char text[kSettingCount][kLabelBytes];
};

struct Engine {
    Game game;
    Inventory inv;
    Holds holds;
    Memory memory;
    CommandRing commands;
    EventRing events;
    OpenStack opens;
    Placement place[kMaxNames];
    HiddenDraws hidden;
    volatile LONG anchor[kMaxNames];    // written by the game thread; info reads it
    GlyphTable glyphs;                  // built by the game thread; query_options reads it
    ErrorLog errors;
    volatile LONG replies[kMaxNames];   // replies queued per window and not yet carried out
    uint8_t covered[kMaxNames];         // game thread only: sent dormant under another window
    // What the events have said of each window: the instance an opened went
    // out for, NULL once a closed did, and the list ability's opened carried.
    // Seeded from the live menus at install; then game thread only.
    const uint8_t* reported[kMaxNames];
    int32_t reported_category;
    // The game's opens of each window, refused ones included, and maplist's
    // steps between levels: each a new prompt. The dialogue boxes' opens,
    // either box's, are also counted as the pair's: the one id both show.
    volatile LONG instance[kMaxNames];
    volatile LONG dialogue_opens;
    Waiting waiting;                   // group moves waiting for their anchor's open

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

    // The target arrows as the drain last saw them, drawn or not; and what
    // the draw hooks share while menus draw: the per-menu draws under way, a
    // bit for each of their levels that draws a hidden window, and one of the
    // target window's arrows drawn. Game thread only.
    uint8_t targetarrow_shown;
    uint8_t drawing_arrow;
    uint32_t draw_depth;
    uint32_t drawing_hidden;

    // The template swap: the menu and frame a swap of the game's found, kept
    // by its pre for its post, and the depth of the engine's own swaps and
    // re-applies, inside which a swap is the engine's. Game thread only.
    const uint8_t* swap_menu;
    int16_t swap_frame[4];
    int swap_guard;

    // The kind the ability opener was called with, while its open of
    // ability is in flight: the opened event's category. Game thread only.
    int32_t ability_pending_kind;
    uint8_t ability_pending;

    // The window whose opens the open hook refuses with no event while a
    // command runs a list's opener, its index + 1 (0 none), and the thread
    // running it; and the opens of sets and lists still to be seen open at
    // the next drains. Game thread only.
    int16_t suppressed;
    DWORD suppress_thread;
    OpenCheck open_checks[kOpenChecks];

    // The row each open of a boolean prompt had its cursor on when the drain
    // first saw it, the asker's default: packed by pack_first_row. Written
    // by the game thread alone.
    volatile LONG yesno_first[kMaxNames];

    // Each number box's question its block kept from opening.
    BoxRecord boxes[kBoxCount];

    // Each Asked window's question its block kept from opening, and
    // bankmenu's waiting with no window as the drain last saw it.
    AskedRecord asked[kAskedCount];
    volatile LONG bank_waiting;

    // maplist's instance, its count of prompts and the level it showed
    // (map_where()), as the drain last saw it open; game thread only.
    const uint8_t* map_menu;
    LONG map_prompts;
    int32_t map_level;

    // mogdoor's texts for the prompt up now; each dialogue box's page as the
    // drain last saw it, game thread only.
    DoorTexts door_texts;
    PageSignal pages[kDialogueCount];

    // Each text window's answer on its way to the drain (core.h's
    // TextAnswer), msgline's message its block kept from opening, and, game
    // thread only, the windows whose field a hide ended and the line
    // comment's field edited then.
    TextAnswer text_answers[kTextCount];
    TextRecord msgline;
    uint8_t field_ended[kTextCount + 1];
    int8_t comment_line;

    // The trade's offers on their way to the server, and the game's gray test
    // of the inventory for the offer window open.
    TradePending trade_pending;
    GrayTable gray_table;

    // Each setting's label, the game's text by its message getter, filled
    // once on the game thread; nothing writes a text after `ready`.
    SettingLabels setting_labels;

    const Command* current;         // the command the drain is carrying out, on `current_thread`
    DWORD current_thread;
    volatile LONG completed;        // commands carried out, not merely taken
    volatile LONG unmatched_keys;
    volatile LONG drain_errors;
    volatile LONG error_seq;        // odd while the game thread writes `error`
    char error[sizeof(ErrorRecord::verb) + kErrorName + kErrorReason + 2];   // "verb name: reason", whole
};

inline bool is_compass(const Engine& e, int n) {
    return n >= 0 && n == e.inv.compass;
}

inline bool is_targetarrow(const Engine& e, int n) {
    return n >= 0 && n == e.inv.targetarrow;
}

// A name on no registry row: no window opens, closes, docks or moves under
// it, and its hold is the engine's own doing.
inline bool synthetic(const Engine& e, int n) {
    return is_compass(e, n) || is_targetarrow(e, n);
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
    if (c.op == kOpOpenSet) {
        return c.x >= 0 && c.x < kSetCount ? kSets[c.x].name : "";
    }
    if (c.op == kOpSetSetting) {
        return c.target >= 0 && c.target < kSettingCount ? kSettings[c.target].name : "";
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

#ifdef HU_HEAPCHECK
// ---------------------------------------------------------------------------
// Heap checks, the drain's side (core.h's heap_run() does the checking). The
// checks wait on heap locks and on each other, which nothing else here does.

// The failed check's report, once: status()'s drain error, and an error
// event every handle reads, verb "debug", name "heap".
inline void heap_report(Engine& e, const char* reason) {
    InterlockedIncrement(&e.error_seq);
    snprintf(e.error, sizeof(e.error), "%s", reason);
    InterlockedIncrement(&e.error_seq);
    InterlockedIncrement(&e.drain_errors);
    const uint32_t pos = e.errors.add(kEveryHandle, 0, "debug", "heap", reason);
    e.events.post(pack_event(kEvError, static_cast<int>(pos & 0xFFFF)));
}

// The latest step this thread's trail has from outside the drain since the
// drain last ended: a hook's, as the game called it.
inline void heap_note_step(HeapCheck& h, DWORD now) {
    const CrumbThread* t = crumb_thread();
    if (!t) {
        return;
    }
    LONG at = t->trail_at;
    for (int k = 0; k < kCrumbTrail && at > 0; ++k) {
        --at;
        const CrumbMark& m = t->trail[static_cast<uint32_t>(at) % kCrumbTrail];
        if (static_cast<LONG>(static_cast<uint32_t>(m.count) - static_cast<uint32_t>(h.frame_end)) <= 0) {
            return;
        }
        const char* step = m.step;
        if (step && strcmp(step, "drain") != 0 && strcmp(step, "hook ui update pre") != 0) {
            h.outside_step = step;
            h.outside_left = m.left;
            h.outside_tick = static_cast<LONG>(now);
            return;
        }
    }
}

// Each drain's start: the step outside the drain noted, a report that is
// due posted, and once a second the background check, which catches what
// the game does outside every command and hook within that second.
inline void heap_frame(Engine& e) {
    HeapCheck& h = hideui_heapcheck;
    const DWORD now = GetTickCount();
    h.drain_thread = static_cast<LONG>(GetCurrentThreadId());
    heap_note_step(h, now);
    if (h.report_due && static_cast<LONG>(now - static_cast<DWORD>(h.report_at)) >= 0
        && InterlockedExchange(&h.report_due, 0)) {
        heap_report(e, h.report);
    }
    if (h.failed || now - static_cast<DWORD>(h.background_tick) < kHeapBackgroundMs) {
        return;
    }
    h.background_tick = static_cast<LONG>(now);
    char command[80];
    if (h.command[0]) {
        snprintf(command, sizeof(command), "%s, %lu ms before", h.command,
            static_cast<unsigned long>(now - static_cast<DWORD>(h.command_tick)));
    } else {
        snprintf(command, sizeof(command), "none");
    }
    char step[64];
    if (h.outside_step) {
        snprintf(step, sizeof(step), "%s %s, %lu ms before", h.outside_step, h.outside_left ? "left" : "entered",
            static_cast<unsigned long>(now - static_cast<DWORD>(h.outside_tick)));
    } else {
        snprintf(step, sizeof(step), "none");
    }
    char context[kHeapLabel];
    snprintf(context, sizeof(context), "last command %s; last step outside the drain %s", command, step);
    heap_run("background", context);
}

// A check before or after the drain runs a command, named by its op and
// target; the one before keeps it as the last command.
inline void heap_command(const Engine& e, const Command& c, bool before) {
    HeapCheck& h = hideui_heapcheck;
    if (before) {
        const char* target = command_target(e, c);
        snprintf(h.command, sizeof(h.command), "%s%s%s", op_name(c.op), target[0] ? " " : "", target);
        h.command_tick = static_cast<LONG>(GetTickCount());
    }
    heap_check("%s %s", before ? "before" : "after", h.command);
}

inline bool heap_held_hidden(const Engine& e, int n) {
    return n >= 0 && n < e.inv.count && ((e.holds.want[n] & kWantHidden) || e.place[n].applied_hidden);
}

// The windows a reconcile hides or unhides: a check before the first, and
// one after them all naming each.
struct HeapChanges {
    char text[96];
    size_t used;
};

inline void heap_change(HeapChanges& c, const Engine& e, int n, bool hide) {
    const char* verb = hide ? "hide" : "unhide";
    if (!c.used) {
        heap_check("before reconcile_hidden: %s %s", verb, e.inv.names[n].name);
    }
    if (c.used < sizeof(c.text)) {
        const int w = snprintf(c.text + c.used, sizeof(c.text) - c.used, "%s%s %s", c.used ? ", " : "", verb,
            e.inv.names[n].name);
        c.used += w > 0 ? static_cast<size_t>(w) : 0;
    }
}

inline void heap_changes_done(const HeapChanges& c) {
    if (c.used) {
        heap_check("after reconcile_hidden: %s", c.text);
    }
}

#define HU_HEAP_FRAME(e) ::hu::heap_frame(e)
#define HU_HEAP_FRAME_END() (::hideui_heapcheck.frame_end = ::hideui_crumbs.counter)
#define HU_HEAP_COMMAND(e, c, before) ::hu::heap_command(e, c, before)
#define HU_HEAP_CHANGES(var) ::hu::HeapChanges var = {}
#define HU_HEAP_CHANGE(var, e, n, hide) ::hu::heap_change(var, e, n, hide)
#define HU_HEAP_CHANGES_DONE(var) ::hu::heap_changes_done(var)
#define HU_HEAP_HIDDEN_CHECK(e, n, ...) \
    do { \
        if (::hu::heap_held_hidden(e, n)) { \
            ::hu::heap_check(__VA_ARGS__); \
        } \
    } while (0)
#else
#define HU_HEAP_FRAME(e) ((void)0)
#define HU_HEAP_FRAME_END() ((void)0)
#define HU_HEAP_COMMAND(e, c, before) ((void)0)
#define HU_HEAP_CHANGES(var) ((void)0)
#define HU_HEAP_CHANGE(var, e, n, hide) ((void)0)
#define HU_HEAP_CHANGES_DONE(var) ((void)0)
#define HU_HEAP_HIDDEN_CHECK(e, n, ...) ((void)0)
#endif

// ---------------------------------------------------------------------------
// Finding menus. On this thread the game's pointers are consistent: a
// non-null controller or menu is live.

inline int name_of_menu(const Engine& e, const uint8_t* menu) {
    const uint8_t* res = rdptr(menu, kMenuRes);
    return res ? e.inv.find_key16(res + kResKey) : -1;
}

// The window the engine hides whose live instance `menu` is, or -1. The
// per-menu draw asks this of every menu it draws each frame, so it scans the
// hidden instances alone. A slot can hold an instance the game freed after
// the drain filled it, and a menu opened since can sit at that address: the
// key tells them apart.
inline int hidden_window(const Engine& e, const uint8_t* menu) {
    if (!menu) {
        return -1;
    }
    const HiddenDraws& h = e.hidden;
    const LONG used = h.used;
    for (LONG i = 0; i < used; ++i) {
        if (h.menu[i] != menu) {
            continue;
        }
        const int n = h.name[i];
        const uint8_t* res = rdptr(menu, kMenuRes);
        if (res && e.inv.names[n].keyed(res + kResKey)) {
            return n;
        }
    }
    return -1;
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
        if (!res || !ne.keyed(res + kResKey)) {
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

// Whether the draw walk draws `menu` at all: not while the manager's
// global hide is on, nor while rem4line's modal dim (`modal`, its
// controller, NULL for none) keeps every window but its own back, unless
// the menu's policy exempts it from both.
inline bool walk_draws(const uint8_t* menu, const uint8_t* mcb, const uint8_t* modal) {
    if (rd32(menu, kMenuPolicy) & kPolicyDrawnAlways) {
        return true;
    }
    if (mcb && mcb[kMcbHideAll] == 1) {
        return false;
    }
    return !(modal && modal[kBoxModal] != 0 && rdptr(modal, kCtlMenu) != menu);
}

// Whether the target window's draw paints an arrow: its instance open, not
// dormant, drawn by the walk and drawing its controller, which shows the
// target with an arrow to paint.
inline bool arrows_drawn(const uint8_t* menu, const uint8_t* ctl, const uint8_t* mcb, const uint8_t* modal) {
    return menu_is_open(menu) && ctl && rd32(menu, kMenuState) != kStateDormant && walk_draws(menu, mcb, modal)
        && menu[kMenuDraws] != 0 && ctl[kTargetOther] == 0
        && (rd16(ctl, kTargetArrowX) != 0 || ctl[kTargetPicking] != 0);
}

inline bool targetarrow_drawn(Engine& e) {
    uint8_t* ctl = NULL;
    const uint8_t* menu = e.inv.targetwi >= 0 ? live_menu(e, e.inv.targetwi, &ctl) : NULL;
    const int box = e.inv.dialogue_window[0];
    uint8_t** slot = box >= 0 && e.inv.names[box].row_count ? e.game.slot[e.inv.names[box].rows[0]] : NULL;
    return arrows_drawn(menu, ctl, e.game.mcb, slot ? *slot : NULL);
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
// The fields of the text-entry windows and of passinpu. While one edits, the
// game's text entry gives it every character typed, hidden or not; a hide,
// an open of a hidden window that starts its field at once, and the drain
// each frame a hidden window's field is found started again (the player's
// keys reach a hidden window that asks nothing) end the field with the
// window's own end, told to drop what the field holds. A show gives
// passinpu, comment and msgline the field the engine ended back with the
// window's own start, the one its open hook calls, before the window is on
// screen: passinpu draws its field unchecked, and the other two start
// theirs only at the open.

const int kFieldPassinpu = kTextCount;
const int kFieldCount = kTextCount + 1;

inline int field_of(const Engine& e, int n) {
    const int t = e.inv.text_of(n);
    if (t >= 0) {
        return t;
    }
    return n >= 0 && strcmp(e.inv.names[n].name, "passinpu") == 0 ? kFieldPassinpu : -1;
}

// Whether field f's window edits now, as its own code tests it.
inline bool field_editing(int f, const uint8_t* ctl) {
    switch (f) {
    case kTextStringdl:
        return ctl[kStringdlEditing] != 0;
    case kTextComment:
        return rdptr(ctl, kCommentField) != NULL;
    case kTextMsgline:
        return rdptr(ctl, kMsglineField) != NULL;
    case kTextScname:
        return rdptr(ctl, kScnameField) != NULL;
    case kTextLink11:
        return rdptr(ctl, kLink11Field) != NULL;
    case kFieldPassinpu:
        return rdptr(ctl, kPassField) != NULL;
    default:
        return false;
    }
}

// Field f's own end: `keep` its text for the window, else the window keeps
// what it held before (link11 takes its name either way).
inline void end_field(Engine& e, int f, uint8_t* ctl, bool keep) {
    const TextSites& s = e.game.text;
    switch (f) {
    case kTextStringdl:
        s.stringdl_stop(ctl, NULL);
        break;
    case kTextComment:
        s.comment_stop(ctl, NULL, keep ? 1 : 0);
        break;
    case kTextMsgline:
        s.msgline_stop(ctl, NULL, keep ? 1 : 0);
        break;
    case kTextScname:
        s.scname_stop(ctl, NULL, keep ? 1 : 0);
        break;
    case kTextLink11:
        s.link11_stop(ctl, NULL, keep ? 1 : 0);
        break;
    case kFieldPassinpu:
        s.passinpu_stop(ctl, NULL, keep ? 1 : 0);
        break;
    default:
        break;
    }
}

// Window n, hidden now, open on `menu`: its field ended if it edits.
inline void end_hidden_field(Engine& e, int n, uint8_t* ctl, const uint8_t* menu) {
    const int f = field_of(e, n);
    if (f < 0 || !ctl || !menu_is_open(menu) || !field_editing(f, ctl)) {
        return;
    }
    if (f == kTextComment) {
        e.comment_line = static_cast<int8_t>(ctl[kCommentEditing]);
    }
    end_field(e, f, ctl, false);
    e.field_ended[f] = 1;
}

// Window n shown again: the field a hide ended started again, comment's on
// the line it edited then.
inline void reopen_field(Engine& e, int n, uint8_t* ctl, const uint8_t* menu) {
    const int f = field_of(e, n);
    if (f < 0 || !e.field_ended[f]) {
        return;
    }
    e.field_ended[f] = 0;
    if (!ctl || !menu_is_open(menu) || field_editing(f, ctl)) {
        return;
    }
    const TextSites& s = e.game.text;
    switch (f) {
    case kTextComment:
        s.comment_start(ctl, NULL, e.comment_line);
        break;
    case kTextMsgline:
        s.msgline_start(ctl, NULL);
        break;
    case kFieldPassinpu:
        s.passinpu_start(ctl, NULL);
        break;
    default:
        break;
    }
}

// ---------------------------------------------------------------------------
// Hiding: the per-menu draw of every live instance of a hidden window
// (HiddenDraws) runs whole, as shown, and the draw primitive's pre drops
// what it would put on the screen; the live menu's mouse byte keeps the row
// hit test off it. The window keeps the draw layer its row gives it: the
// manager's list puts every menu of layer 5 and up after the others, and
// each update makes the last eligible menu in it the active one, so a hidden
// window takes the focus exactly when it would shown. The open does not copy
// the mouse byte from the row, so the open's post puts it on a new instance.
// The unhide gives the byte back: the instance the hide found gets the value
// it had then, any other the constructor's 1.

// Slot i of the hidden draws given up: its menu cleared first, as a reader
// may be scanning it, then the slots no reader needs dropped off the end.
inline void free_hidden_slot(HiddenDraws& h, LONG i) {
    h.menu[i] = NULL;
    h.slot_plus_one[h.name[i]] = 0;
    LONG used = h.used;
    while (used > 0 && !h.menu[used - 1]) {
        --used;
    }
    h.used = used;
}

// Window n's live instance in the hidden draws, or with `menu` NULL, out of
// them. One slot a window: the open reuses a name's live instance, so a name
// never has two. An address another window's slot still holds is a freed
// instance's, and that slot goes.
inline void set_hidden_draw(Engine& e, int n, const uint8_t* menu) {
    HiddenDraws& h = e.hidden;
    const LONG slot = h.slot_plus_one[n] - 1;
    if (!menu) {
        if (slot >= 0) {
            free_hidden_slot(h, slot);
        }
        return;
    }
    if (slot >= 0 && h.menu[slot] == menu) {
        return;
    }
    for (LONG i = 0; i < h.used; ++i) {
        if (i != slot && h.menu[i] == menu) {
            free_hidden_slot(h, i);
        }
    }
    if (slot >= 0) {
        h.menu[slot] = menu;
        return;
    }
    const LONG used = h.used;
    LONG empty = 0;
    while (empty < used && h.menu[empty]) {
        ++empty;
    }
    h.name[empty] = static_cast<int16_t>(n);
    h.menu[empty] = menu;
    if (empty == used) {
        h.used = used + 1;
    }
    h.slot_plus_one[n] = static_cast<int16_t>(empty + 1);
}

inline void apply_hidden(Engine& e, int n, bool hidden) {
    Placement& p = e.place[n];
    uint8_t* ctl = NULL;
    uint8_t* menu = live_menu(e, n, &ctl);
    if (menu) {
        if (hidden) {
            end_hidden_field(e, n, ctl, menu);
            p.own_mouse = menu[kMenuMouse];
            p.mouse_menu = menu;
            menu[kMenuMouse] = 0;
        } else {
            reopen_field(e, n, ctl, menu);
            menu[kMenuMouse] = menu == p.mouse_menu ? p.own_mouse : 1;
        }
    }
    set_hidden_draw(e, n, hidden ? menu : NULL);
    if (!hidden) {
        p.mouse_menu = NULL;
        const int f = field_of(e, n);
        if (f >= 0) {
            e.field_ended[f] = 0;
        }
    }
    p.applied_hidden = hidden ? 1 : 0;
}

// Each frame: every hide and unhide asked for since the last, and each
// hidden window's field ended and its slot on the instance its controller
// holds now. The game clears a controller's menu only as it frees the menu,
// so a window the game closes keeps its slot through its staged close, whose
// draws paint too, until the game frees its instance.
inline void reconcile_hidden(Engine& e) {
    HU_STEP("reconcile_hidden");
    HU_HEAP_CHANGES(changes);
    for (int n = 0; n < e.inv.count; ++n) {
        const bool want = (e.holds.want[n] & kWantHidden) != 0;
        if (want != (e.place[n].applied_hidden != 0)) {
            HU_HEAP_CHANGE(changes, e, n, want);
            apply_hidden(e, n, want);
        } else if (want) {
            uint8_t* ctl = NULL;
            uint8_t* menu = live_menu(e, n, &ctl);
            end_hidden_field(e, n, ctl, menu);
            set_hidden_draw(e, n, menu);
        }
    }
    HU_HEAP_CHANGES_DONE(changes);
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

// The origin y window n's remembered position puts it at now. A
// bottom-anchored window's move by its frame keeps the origin pinned at its
// first placement, the move's own while the window is open: its bottom
// stays at the move's y plus the frame's height then, however the height
// changes after (a zone-in opens the party list at its template's full
// height, then its roster's swap shrinks it).
inline int remembered_y(Engine& e, int n, const uint8_t* menu, const MemEntry& m) {
    classify(e, n, menu);
    if (m.pinned && bottom_anchored(e, n)) {
        return m.pinned_y;
    }
    return origin_y(e, n, menu, m.y, m.by_frame != 0);
}

// Pins a bottom-anchored window's move by its frame at the origin its first
// placement just gave it. A frame or default rect with no size placed
// nothing, and pins nothing.
inline void pin_frame_move(Engine& e, int n, const uint8_t* menu) {
    const MemEntry& m = e.memory.e[n];
    if (m.active && m.by_frame && !m.pinned && bottom_anchored(e, n) && frame_width(menu) > 0
        && frame_height(menu) > 0 && default_height(menu) > 0) {
        e.memory.pin(n, rd16(menu, kMenuOrigin + 2));
    }
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

// The engine's own template swap: inside it the swap's hooks re-apply
// nothing, the engine placing the window itself after it.
inline void swap_template(Engine& e, uint8_t* menu, const char* key) {
    ++e.swap_guard;
    e.game.template_swap(menu, NULL, key);
    --e.swap_guard;
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
        swap_template(e, menu, key);
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
        swap_template(e, menu, key);
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
// and no menu: its position alone, back on its anchor. The target arrows
// have neither to reset.
inline void reset_one(Engine& e, int n, bool position, bool size) {
    if (is_compass(e, n)) {
        if (position) {
            reset_compass(e, n);
        }
        return;
    }
    if (is_targetarrow(e, n)) {
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
            pin_frame_move(e, n, menu);
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
// bottom-anchored window remembered by its frame's top-left and never placed
// since (moved while closed) takes its size first: its first placement
// converts the frame top at the height it has then, and pins that origin.
// Without `place_it`, the size alone: a group move waiting for this anchor
// places it.
inline void reapply(Engine& e, int n, uint8_t* menu, bool place_it) {
    const MemEntry& m = e.memory.e[n];
    if (!m.active && !m.size_kind) {
        return;
    }
    uint8_t* ctl = controller_of(e, n, menu);
    classify(e, n, menu);
    const bool place = m.active && place_it;
    const bool size_first = place && m.by_frame && !m.pinned && m.size_kind && bottom_anchored(e, n);
    if (size_first) {
        apply_size(e, n, menu, ctl, m.size_kind, m.size_a, m.size_b);
    }
    if (place) {
        const bool docked = m.keep_dock && (rd32(menu, kMenuPolicy) & kDockMask);
        const int y = docked ? rd16(menu, kMenuOrigin + 2) : remembered_y(e, n, menu, m);
        position(e, n, menu, ctl, m.x, y, m.keep_dock != 0);
        pin_frame_move(e, n, menu);
    }
    if (m.size_kind && !size_first) {
        apply_size(e, n, menu, ctl, m.size_kind, m.size_a, m.size_b);
    }
}

// After the game's own swap of a window's template: the remembered
// position alone, the window's size being the game's count of its rows. A
// bottom-anchored window goes on its pinned origin, else on the one it has,
// which the swap leaves alone, and the swap's height hangs from that bottom
// edge: the window grows and shrinks upward, as the game's does.
inline void reapply_position(Engine& e, int n, uint8_t* menu) {
    const MemEntry& m = e.memory.e[n];
    if (!m.active) {
        return;
    }
    uint8_t* ctl = controller_of(e, n, menu);
    classify(e, n, menu);
    const bool docked = m.keep_dock && (rd32(menu, kMenuPolicy) & kDockMask);
    const bool unpinned_bottom = bottom_anchored(e, n) && !m.pinned;
    const int y = docked || unpinned_bottom ? rd16(menu, kMenuOrigin + 2) : remembered_y(e, n, menu, m);
    position(e, n, menu, ctl, m.x, y, m.keep_dock != 0);
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

// map0's own cancel, the one its input routine runs on Escape: the window
// closed by instance, then the map's close on mapframe's controller, which
// closes the map's other windows and fades the frame out; the frame closes
// itself once faded. Close by name alone leaves the frame up, holding the
// focus. False, with nothing done, unless n is map0 and open.
inline bool map0_cancel(Engine& e, int n) {
    uint8_t* menu = strcmp(e.inv.names[n].name, "map0") == 0 ? open_menu(e, n, NULL) : NULL;
    if (!menu) {
        return false;
    }
    HU_HEAP_CHECK("before map0 cancel");
    e.game.close_instance(e.game.mcb, NULL, menu, 1);
    if (uint8_t* frame = controller_global(e, "mapframe")) {
        e.game.asked.map_close(frame, NULL, 0);
    }
    HU_HEAP_CHECK("after map0 cancel");
    return true;
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
        drain_error(e, "query cannot be canceled now: the event allows no cancel");
        return;
    }
    wr16(ctl, kQueryResult, kQueryCanceled);
    if (static_cast<uint16_t>(rd16(ctl, kQueryResult)) != kQueryCanceled) {
        drain_error(e, "query's result word does not read the cancel");
    }
}

// passinpu asks while its callback is set; its confirm's reset clears it.
inline bool passinpu_asks(const uint8_t* ctl) {
    return ctl && rdptr(ctl, kPassCallback);
}

// The callback, then the reset, as the game's own confirm does after it
// reads the edit widget; and the close that follows it there.
inline void passinpu_reply(Engine& e, const Command& c) {
    const bool submit = c.op == kOpPassinpuSubmit;
    uint8_t* ctl = controller_global(e, "passinpu");
    if (!passinpu_asks(ctl)) {
        drain_error(e, "no text entry is pending");
        return;
    }
    void* cb = rdptr(ctl, kPassCallback);
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

// Why arealist's controller `ctl`, NULL for none, has no NPC event waiting
// on its choice, or NULL while one does: the latch the event's opener arms
// before the open, in mode 1 or 2, with the window open or its open refused
// by a block.
inline const char* area_refusal(const uint8_t* ctl) {
    if (!ctl || !ctl[kAreaLatch]) {
        return "no area choice is pending";
    }
    return area_event_mode(ctl[kAreaMode]) ? NULL : kAreaNotAsking;
}

inline uint8_t* area_event_pending(Engine& e) {
    uint8_t* ctl = controller_global(e, "arealist");
    const char* why = area_refusal(ctl);
    if (why) {
        drain_error(e, "%s", why);
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

// The yes/no default as the drain recorded it: the window's open count in
// the top 30 bits, 2 once recorded, 1 when its cursor was on yes.
inline LONG pack_first_row(LONG instance, bool yes) {
    return static_cast<LONG>(static_cast<uint32_t>(instance) << 2 | 2u | (yes ? 1u : 0u));
}
inline bool first_row_is(LONG packed, LONG instance) {
    return (packed & 2) != 0 && static_cast<uint32_t>(packed) >> 2 == (static_cast<uint32_t>(instance) & 0x3FFFFFFFu);
}
inline bool first_row_yes(LONG packed) { return (packed & 1) != 0; }

// Whether window n of the yes/no class asks a question now; `why` says why
// not, and `none`, when given, whether that is because no question is
// pending in it rather than a controller this engine cannot read as n's.
// `ctl` is its controller through its registry row, NULL for none, with
// kYesNoBytes of it readable: the class's own (its vtable), keyed to n (the
// resyn family shares one, keyed by the client's language), its callback
// set.
inline bool yesno_asks(const Engine& e, int n, const uint8_t* ctl, char* why, size_t size, bool* none = NULL) {
    const char* nm = e.inv.names[n].name;
    bool unused;
    none = none ? none : &unused;
    *none = false;
    if (ctl && rdptr(ctl, 0) != e.game.yesno_vtable) {
        snprintf(why, size, "%s's controller is not the yes/no prompt this engine knows", nm);
        return false;
    }
    if (ctl && memcmp(ctl + kYesNoKey, e.inv.names[n].key, kKeyLen) != 0) {
        const int asks = e.inv.find_key16(ctl + kYesNoKey);
        snprintf(why, size, "the game asks through %s, not %s", asks >= 0 ? e.inv.names[asks].name : "another window",
            nm);
        return false;
    }
    if (!ctl || !rdptr(ctl, kYesNoCallback)) {
        snprintf(why, size, "no question is pending in %s", nm);
        *none = true;
        return false;
    }
    return true;
}

inline uint8_t* yesno_pending(Engine& e, int n) {
    uint8_t* ctl = controller_global(e, e.inv.names[n].name);
    char why[kErrorReason];
    if (!yesno_asks(e, n, ctl, why, sizeof(why))) {
        drain_error(e, "%s", why);
        return NULL;
    }
    return ctl;
}

// What the class's own confirm and cancel do with the window's event in
// hand: the callback with the answer; then, unless the callback asked again,
// the question cleared and the window and holdtime closed, in the order
// each runs; a question asked again closes the window and opens its key anew
// on that question, the cursor put on no unless its default is yes, the
// cancel closing holdtime after. A question confirmed by holding ends in the
// same confirm when holdtime runs out. The window is closed by its key,
// whose close by name closes the instance the controller holds.
inline void yesno_reply(Engine& e, const Command& c) {
    const bool cancel = c.op == kOpYesNoCancel;
    uint8_t* ctl = yesno_pending(e, c.target);
    if (!ctl) {
        return;
    }
    if (cancel && !ctl[kYesNoCancellable]) {
        drain_error(e, "this question cannot be canceled; answer it");
        return;
    }
    const void* cb = rdptr(ctl, kYesNoCallback);
    YesNoCallbackFn callback;
    memcpy(&callback, &cb, sizeof(callback));
    callback(rdptr(ctl, kYesNoContext), cancel || !c.value ? 0 : 1);
    const char* key = reinterpret_cast<const char*>(ctl + kYesNoKey);
    if (!ctl[kYesNoChained]) {
        wr32(ctl, kYesNoCallback, 0);
        if (!cancel) {
            close_named(e, "holdtime");
        }
        e.game.close(e.game.mcb, NULL, key);
        if (cancel) {
            close_named(e, "holdtime");
        }
        return;
    }
    e.game.close(e.game.mcb, NULL, key);
    e.game.open(e.game.mcb, NULL, key, 1, 0);
    ctl[kYesNoChained] = 0;
    uint8_t* menu = rdptr(ctl, kCtlMenu);
    if (!ctl[kYesNoChainedYes] && menu) {
        e.game.select_row(menu, NULL, kRowNo);
    }
    if (cancel) {
        close_named(e, "holdtime");
    }
}

// Why the KO window's own confirm would not ask now, or NULL. `menu` is
// dead's open instance, NULL for none; `asker` the controller in the global
// the confirm asks through (comyn's), with kYesNoBytes of it readable, NULL
// for none. The input sink hands the window its confirm only while it is
// not dormant, its input wait is over and it is not busy; the window's input
// handler then asks with no check of its own, and the ask answers 0 at once
// while the asker has a question pending.
inline const char* dead_refusal(const Engine& e, const uint8_t* menu, const uint8_t* asker) {
    if (!menu) {
        return "dead is not open";
    }
    if (rd32(menu, kMenuState) == kStateDormant) {
        return "dead is under another window";
    }
    if (rd16(menu, kMenuInputWait) > 0) {
        return "dead takes no input yet";
    }
    if (menu[kMenuBusy]) {
        return "dead is busy";
    }
    if (!asker || rdptr(asker, 0) != e.game.yesno_vtable) {
        return "comyn's controller is not the yes/no prompt this engine knows";
    }
    if (rdptr(asker, kYesNoCallback)) {
        return "comyn already has a question pending";
    }
    return NULL;
}

// The KO window's own confirm: the home-point question put through comyn
// with the arguments the window's input handler passes, its message picked
// as the handler picks it, and nothing more. The question is then comyn's
// to answer; a yes marks the home point chosen, as the player's would.
inline void dead_confirm(Engine& e) {
    const uint8_t* menu = open_named(e, "dead", NULL);
    uint8_t* asker = controller_global(e, "comyn");
    const char* why = dead_refusal(e, menu, asker);
    if (why) {
        drain_error(e, "%s", why);
        return;
    }
    const DeadConfirm& d = e.game.dead;
    const bool set = (d.decider(d.decider_first, d.decider_second) & 0xFF) != 0;
    const char* text = d.message(d.category, set ? d.text_if_set : d.text_otherwise);
    if (!(e.game.yesno_ask(asker, NULL, d.callback, d.context, text, d.default_yes, d.help, d.cancellable, d.mode)
          & 0xFF)) {
        drain_error(e, "the game refused comyn's open; nothing was asked");
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
inline void box_reply(Engine& e, const Command& c);
inline void asked_reply(Engine& e, const Command& c);
inline void dialogue_reply(Engine& e, const Command& c);
inline void text_reply(Engine& e, const Command& c);
inline void offer_reply(Engine& e, const Command& c);

inline const uint8_t* controller_at(const Engine& e, int n);
inline bool prompt_asks(Engine& e, int n);

// The game's open binds a new instance to the controller in the slot of
// its name's first registry row, writing into it with no test for NULL: a
// slot that holds none crashes the client. A row with no slot is not bound.
inline bool controller_missing(const Engine& e, int n) {
    if (n < 0 || n >= e.inv.count || e.inv.names[n].row_count == 0) {
        return false;
    }
    uint8_t** slot = e.game.slot[e.inv.names[n].rows[0]];
    return slot && !*slot;
}

// A child window has one of its parents open, as the game thread finds it;
// any other window has none to need.
inline bool parent_open(Engine& e, int n) {
    const ChildSpec* c = child_spec(e.inv.names[n].name);
    for (int i = 0; c && c->parents[i]; ++i) {
        if (open_menu(e, e.inv.find_exact(c->parents[i]), NULL)) {
            return true;
        }
    }
    return c == NULL;
}

// A set's or a list's opener has run: the window it opens must be open now,
// and is looked at again at the next drains, where the game's own
// per-frame may have closed it again (a pet's list with no pet out).
inline void expect_open(Engine& e, const Command& c, int n, int set) {
    if (n < 0 || !open_menu(e, n, NULL)) {
        drain_error(e, "the game refused it");
        return;
    }
    int at = -1;
    for (int i = 0; i < kOpenChecks && at < 0; ++i) {
        if (e.open_checks[i].active && e.open_checks[i].n == n) {
            at = i;
        }
    }
    for (int i = 0; i < kOpenChecks && at < 0; ++i) {
        if (!e.open_checks[i].active) {
            at = i;
        }
    }
    if (at < 0) {
        return;
    }
    OpenCheck& k = e.open_checks[at];
    k.active = 1;
    k.frames = kOpenCheckFrames;
    k.verb = c.verb;
    k.set = static_cast<uint8_t>(set + 1);
    k.n = static_cast<int16_t>(n);
    k.slot = c.slot;
    k.gen = c.gen;
    k.instance = e.instance[n];
}

// At a drain's start: an open expect_open() keeps that the game has closed
// since, with no other open of the window in between, is the game's
// refusal, reported to the handle that asked; one still open once its
// frames are up is the game's to keep.
inline void watch_open_checks(Engine& e) {
    for (int i = 0; i < kOpenChecks; ++i) {
        OpenCheck& k = e.open_checks[i];
        if (!k.active) {
            continue;
        }
        if (e.instance[k.n] != k.instance) {
            k.active = 0;
            continue;
        }
        if (!open_menu(e, k.n, NULL)) {
            Command c;
            memset(&c, 0, sizeof(c));
            c.op = k.set ? kOpOpenSet : kOpOpenList;
            c.verb = k.verb;
            c.x = static_cast<int16_t>(k.set ? k.set - 1 : 0);
            c.target = k.n;
            c.slot = k.slot;
            c.gen = k.gen;
            const Command* outer = e.current;
            const DWORD outer_thread = e.current_thread;
            e.current = &c;
            e.current_thread = GetCurrentThreadId();
            drain_error(e, "the game refused it: it closed again at once");
            e.current = outer;
            e.current_thread = outer_thread;
            k.active = 0;
            continue;
        }
        if (--k.frames == 0) {
            k.active = 0;
        }
    }
}

// The engine's own close of window n: the close is no refusal of an open.
inline void forget_open_checks(Engine& e, int n) {
    for (int i = 0; i < kOpenChecks; ++i) {
        if (e.open_checks[i].n == n) {
            e.open_checks[i].active = 0;
        }
    }
}

// open(name, list): the list's opener with no sort window and no detail
// pane, so the list alone. The magic list's rebuild opens subwindo every
// time; the open hook refuses that open while the opener runs, with no
// event, as if it had never been asked. The list does not need the pane's
// window: what it writes for the pane each frame goes to subwindo's
// controller, which always exists.
inline void open_list(Engine& e, const Command& c) {
    const int32_t category = static_cast<int32_t>(c.value);
    const bool ability = c.target == e.inv.ability;
    if (ability ? !e.game.ability_list : !e.game.openers.magic) {
        drain_error(e, "the game's opener of the list is not resolved");
        return;
    }
    if (ability) {
        e.game.ability_list(category, 0, 0);
    } else {
        e.suppressed = static_cast<int16_t>(e.inv.find_exact("subwindo") + 1);
        e.suppress_thread = GetCurrentThreadId();
        e.game.openers.magic(category, 0, 0);
        e.suppressed = 0;
    }
    expect_open(e, c, c.target, -1);
}

// The open the game's own routes make of a window by its name: activate 1,
// and what the game passes as the overlap.
inline bool open_by_name(Engine& e, const char* nm) {
    const int n = e.inv.find_exact(nm);
    if (n < 0 || controller_missing(e, n)) {
        return false;
    }
    char key[kKeyLen + 1];
    make_key(e, n, key);
    return e.game.open(e.game.mcb, NULL, key, 1, open_overlap(nm)) != NULL;
}

// Whether the route of `route` resolved.
inline bool route_resolved(const Engine& e, int route) {
    const RouteSites& r = e.game.routes;
    switch (route) {
    case kRouteStorage: return r.storage != NULL;
    case kRouteMogpost: return r.mogpost && r.mog_house && r.nomad;
    case kRouteFriend: return r.friend_list != NULL;
    case kRouteMsglist: return r.msglist != NULL;
    case kRouteBlklist: return r.blklist != NULL;
    case kRouteMount: return r.mount != NULL;
    case kRouteJobChange: return r.language_key && r.mog_house && r.nomad && r.job_row;
    case kRouteConf11l: return r.conf11l_set != 0;
    case kRouteJobPoints: return r.job_points_test && r.job_points;
    case kRouteFpCat2: return r.fp_cat2 != NULL;
    case kRouteQuest00: return r.quest00_mode != 0;
    case kRouteQuest01: return r.quest00_dim[0] && r.quest00_dim[1] && r.quest01;
    case kRouteMiss00: return r.miss00_menu && r.expansion && r.mission_bits && r.miss00_log;
    case kRouteSearch:
    case kRouteComgenre:
    case kRouteCommentSub:
    case kRouteScsibori: return r.search_reset && r.search_mode && r.comgenre && r.comment_sub[0]
                             && r.comment_sub[1] && r.comment_sub[2];
    case kRouteItemPanel: return r.item_panel && r.bag_size && r.summary_type && e.game.offer.item
                              && e.game.offer.cache && e.game.offer.item_data;
    case kRouteTimer: return r.timer[0] && r.timer[1];
    case kRoutePersona: return r.persona_layout != 0;
    case kRouteMacro: return r.macro_pick && r.macro_switch && r.macro_palette && e.game.openers.macro_editor;
    case kRouteScanlist: return r.tracking && r.scanlist && r.scan_question && e.game.yesno_ask
                             && e.game.dead.message;
    case kRouteColoresu: return r.coloresu_region && r.region_kind;
    }
    return false;
}

// open_route() and the sets' opens refused with an error: the controller a
// route writes or runs on is not there, or a test the game makes first
// fails.
inline uint8_t* route_controller(Engine& e, const char* nm) {
    uint8_t* ctl = controller_global(e, nm);
    if (!ctl) {
        drain_error(e, "%s has no controller now, and the game's opener would crash the client", nm);
    }
    return ctl;
}

// The Mog House, and an area a Nomad Moogle serves, each where its manager
// exists: the job change's confirm acts through the manager of either, and
// nothing else. The Mog House's menu runs the storage list and the post for
// it, the Nomad Moogle's the storage list for it.
inline bool in_mog_house(const Engine& e) {
    return e.game.routes.mog_house && *e.game.routes.mog_house;
}

inline bool nomad_area(const Engine& e) {
    return e.game.routes.nomad && *e.game.routes.nomad;
}

inline bool job_change_area(const Engine& e) {
    return in_mog_house(e) || nomad_area(e);
}

// A search the results list holds, as its own narrow row asks.
inline bool search_ran(Engine& e) {
    const uint8_t* results = controller_global(e, "scresult");
    const uint32_t mode = results ? rd32(results, e.game.routes.search_mode) : 0;
    if (mode != 1 && mode != 2) {
        drain_error(e, "the game refused it: no search has run to refine");
        return false;
    }
    return true;
}

// The search reset the search menu's own Search row runs: the criteria
// cleared, the area set to the player's, nothing sent.
inline bool search_reset(Engine& e) {
    uint8_t* results = route_controller(e, "scresult");
    if (results) {
        e.game.routes.search_reset(results, NULL, 0);
    }
    return results != NULL;
}

// The summary the inventory list's rows read an item's data from: its type
// word, or -1 with the error reported for an item the game has no data for.
inline int32_t item_type_of(Engine& e, uint32_t id, const char* what) {
    uint8_t* cache = e.game.offer.cache ? *e.game.offer.cache : NULL;
    const uint8_t* summary = cache ? e.game.offer.item_data(cache, NULL, id) : NULL;
    if (!summary) {
        drain_error(e, "the game has no data for %s", what);
        return -1;
    }
    return static_cast<uint16_t>(rd16(summary, e.game.routes.summary_type));
}

// The item panel for an item: iteminfo beside a list (mode 0), or itemxinf
// as the inventory's cursor opens it for the compare option (mode 5, beside
// equip's controller and over inventor's). An item in a bag is shown with
// its record's extra data; one by its id alone has none, and the panel
// reads the extra data of kItemPanelExtraFirst..Last's items with no test.
inline bool item_panel(Engine& e, int n, int16_t bag, int16_t slot, uint32_t id) {
    const RouteSites& r = e.game.routes;
    uint8_t* ctl = route_controller(e, e.inv.names[n].name);
    if (!ctl) {
        return false;
    }
    const bool compare = strcmp(e.inv.names[n].name, "itemxinf") == 0;
    const void* beside = compare ? controller_global(e, "equip") : NULL;
    const void* under = compare ? controller_global(e, "inventor") : NULL;
    const uint8_t* extra = NULL;
    int32_t index = 0;
    if (bag >= 0) {
        // The game's lists go from slot 1 to below the bag's size, which
        // counts slot 0.
        const int32_t size = static_cast<int32_t>(r.bag_size(bag) & 0xFF);
        if (size == 0xFF || size <= 1) {
            drain_error(e, "%s has no slots now", list_name(HU_CHOICES(kBags), bag));
            return false;
        }
        if (slot >= size) {
            drain_error(e, "%s has no slot %d: it has slots 1 to %d now", list_name(HU_CHOICES(kBags), bag), slot,
                size - 1);
            return false;
        }
        const uint8_t* record = e.game.offer.item(static_cast<uint32_t>(bag), static_cast<uint32_t>(slot));
        id = record ? static_cast<uint16_t>(rd16(record, 0)) : 0;
        if (!id || id == kGilId) {
            drain_error(e, "%s has no item in slot %d", list_name(HU_CHOICES(kBags), bag), slot);
            return false;
        }
        index = record[2];
        extra = record + e.game.routes.item_extra;
    } else {
        char what[24];
        snprintf(what, sizeof(what), "item %u", id);
        const int32_t type = item_type_of(e, id, what);
        if (type < 0) {
            return false;
        }
        if (type >= kItemPanelExtraFirst && type <= kItemPanelExtraLast) {
            drain_error(e, "item %u is of a kind the panel shows with its bag's copy of it, and by its id alone"
                " it would crash the client: give its bag and slot", id);
            return false;
        }
    }
    r.item_panel(ctl, NULL, compare ? 5 : 0, static_cast<int32_t>(id), index, bag >= 0 ? bag : 0, extra, beside,
        under);
    return true;
}

// open(name, arg) of a window kRoutes names: the game's own route to it,
// as its menu row, command or list runs it, with the argument in what that
// route takes. A window the route's opener opens along with the named one
// (`held`, "" for none) is held off, as open_list() holds subwindo. Each
// refusal a test the game makes first gives is an error; one its opener
// makes leaves the window closed, which expect_open() reports.
inline void open_route(Engine& e, const Command& c) {
    const char* nm = e.inv.names[c.target].name;
    const RouteSpec* spec = route_spec(nm);
    const RouteSites& r = e.game.routes;
    if (!spec || !route_resolved(e, spec->route)) {
        drain_error(e, "the game's route to %s is not resolved", nm);
        return;
    }
    if (!parent_open(e, c.target)) {
        char why[kErrorReason];
        child_reason(*child_spec(nm), why, sizeof(why));
        drain_error(e, "%s", why);
        return;
    }
    const int32_t value = static_cast<int32_t>(c.value);
    const char* held = NULL;
    uint8_t* ctl = NULL;
    const bool needs_ctl = spec->route != kRouteMacro && spec->route != kRouteSearch
        && spec->route != kRouteScsibori;
    if (needs_ctl && !(ctl = route_controller(e, nm))) {
        return;
    }
    switch (spec->route) {
    case kRouteStorage:
        if (c.x == 0 ? !in_mog_house(e) : !nomad_area(e)) {
            drain_error(e, c.x == 0 ? "the game refused it: the Mog House's storage opens only in the Mog House"
                                    : "the game refused it: the Nomad Moogle's storage opens only where a Nomad"
                                      " Moogle serves");
            return;
        }
        r.storage(ctl, NULL, static_cast<uint32_t>(c.x));
        break;
    case kRouteMogpost:
        if (value == 1 && !in_mog_house(e)) {
            drain_error(e, "the game refused it: the Mog House's post opens only in the Mog House");
            return;
        }
        if (value == 2 && !job_change_area(e)) {
            drain_error(e, "the game refused it: the Nomad Moogle's post asks the server for the delivery box only"
                " in the Mog House or where a Nomad Moogle serves");
            return;
        }
        r.mogpost(ctl, NULL, static_cast<uint32_t>(value - 1));
        break;
    case kRouteFriend:
        held = "titlehan";
        break;
    case kRouteMsglist:
        held = "titlehan";
        break;
    case kRouteBlklist:
        r.blklist(ctl, NULL);
        break;
    case kRouteMount:
        held = "subwindo";
        break;
    case kRouteJobChange: {
        if (!job_change_area(e)) {
            drain_error(e, "the game refused it: the job changes only in the Mog House or where a Nomad Moogle"
                " serves");
            return;
        }
        const char* key = r.language_key(6);
        if (!key || !readable(key, kKeyLen) || e.inv.find_key(key) != c.target) {
            drain_error(e, "the game's job list on this client is not jobcselu");
            return;
        }
        wr32(ctl, r.job_row, static_cast<uint32_t>(value));
        e.game.open(e.game.mcb, NULL, key, 1, 0);
        break;
    }
    case kRouteConf11l:
        wr32(ctl, r.conf11l_set, static_cast<uint32_t>(value - 1));
        open_by_name(e, nm);
        break;
    case kRouteJobPoints:
        if (!(r.job_points_test(value) & 0xFF)) {
            drain_error(e, "the game refused it: its job list offers %s only at level 99",
                list_name(HU_CHOICES(kJobs), value));
            return;
        }
        r.job_points(ctl, NULL, static_cast<uint32_t>(value));
        break;
    case kRouteFpCat2:
        r.fp_cat2(ctl, NULL, static_cast<uint32_t>(value));
        break;
    case kRouteQuest00:
        wr32(ctl, r.quest00_mode, static_cast<uint32_t>(value - 1));
        open_by_name(e, nm);
        break;
    case kRouteQuest01: {
        uint8_t* quests = route_controller(e, "quest00");
        if (!quests) {
            return;
        }
        // quest00's confirm opens lists 1 to 5 from its rows in mode 1, and
        // lists 6 and 7 from its rows in mode 3.
        const uint32_t mode = rd32(quests, r.quest00_mode);
        const uint32_t needs = value <= 5 ? 0 : 2;
        if (mode != needs) {
            drain_error(e, "the game refused it: quest01's list %d opens only from quest00's mode %s", value,
                needs == 0 ? "1" : "3, 'Ranking Information'");
            return;
        }
        r.quest00_dim[0](quests, NULL);
        r.quest00_dim[1](quests, NULL);
        if (open_by_name(e, nm)) {
            r.quest01(ctl, NULL, static_cast<uint32_t>(value - 1));
        }
        break;
    }
    case kRouteMiss00:
        if (value == kMissionNation) {
            r.miss00_menu(ctl, NULL, 0);
        } else if (!(r.expansion(r.mission_bits[value - 1]) & 0xFF)) {
            drain_error(e, "the game refused it: the %s log's expansion is not on",
                list_name(HU_CHOICES(kMissionLogs), value));
            return;
        } else {
            wr32(ctl, r.miss00_log, static_cast<uint32_t>(value - 1));
        }
        open_by_name(e, nm);
        break;
    case kRouteSearch:
        if (value == 1 ? !search_reset(e) : !search_ran(e)) {
            return;
        }
        open_by_name(e, nm);
        break;
    case kRouteComgenre:
        if ((value == 2 && !search_reset(e)) || (value == 3 && !search_ran(e))) {
            return;
        }
        r.comgenre(ctl, NULL, static_cast<uint32_t>(value - 1));
        break;
    case kRouteCommentSub: {
        if (value == kCommentModeRefine && !search_ran(e)) {
            return;
        }
        const int which = c.y == 4 ? 1 : c.y == 5 ? 2 : 0;
        r.comment_sub[which](ctl, NULL, value, c.y * kCommentBaseStep);
        break;
    }
    case kRouteScsibori:
        if (!search_ran(e)) {
            return;
        }
        open_by_name(e, nm);
        break;
    case kRouteItemPanel:
        held = strcmp(nm, "itemxinf") == 0 ? "itemyinf" : "";
        break;
    case kRouteTimer:
        r.timer[strcmp(nm, "eventtim") == 0 ? 1 : 0](ctl, NULL, c.x, value);
        break;
    case kRoutePersona:
        ctl[r.persona_layout] = 2;
        open_by_name(e, nm);
        break;
    case kRouteMacro: {
        uint8_t* editor = *e.game.openers.macro_editor;
        if (!editor) {
            drain_error(e, "the game's macro editor does not exist yet");
            return;
        }
        // The palette switch (Left or Right on a palette) is the game's one
        // writer of the palette: it toggles it and opens the other palette's
        // window, held off here, which the pick closes again.
        if (editor[r.macro_palette] != c.x) {
            e.suppressed = static_cast<int16_t>(e.inv.find_exact(c.x == 2 ? "mcr2edit" : "mcr1edit") + 1);
            e.suppress_thread = GetCurrentThreadId();
            r.macro_switch(editor, NULL, static_cast<uint32_t>(value));
            e.suppressed = 0;
        }
        if (editor[r.macro_palette] != c.x) {
            drain_error(e, "the game's palette switch left the other palette in place");
            return;
        }
        held = "mcrledit";
        break;
    }
    case kRouteScanlist: {
        // While the map tracks, the Wide Scan row asks on comyn whether to
        // stop first, its callback opening the list on a yes: the question
        // is the open's outcome, answered as any other.
        const int32_t* tracking = r.tracking();
        if (tracking && *tracking == 1) {
            uint8_t* yesno = route_controller(e, "comyn");
            if (!yesno) {
                return;
            }
            e.game.yesno_ask(yesno, NULL, r.scan_question, 0, e.game.dead.message(0xE, 0x45), 0, 0, 1, 0);
            return;
        }
        r.scanlist(ctl, NULL);
        break;
    }
    case kRouteColoresu: {
        const uint8_t* frame = controller_global(e, "cnqframe");
        if (!frame || rd32(frame, r.region_kind) != 3) {
            drain_error(e, "coloresu needs cnqframe open on its Colonization map");
            return;
        }
        wr32(ctl, r.coloresu_region, static_cast<uint32_t>(value));
        if (open_by_name(e, nm)) {
            close_named(e, "colorank");
        }
        break;
    }
    }
    if (held) {
        e.suppressed = static_cast<int16_t>(held[0] ? e.inv.find_exact(held) + 1 : 0);
        e.suppress_thread = GetCurrentThreadId();
        bool ran = true;
        switch (spec->route) {
        case kRouteFriend:
            r.friend_list(ctl, NULL, 0);
            break;
        case kRouteMsglist:
            ran = (r.msglist(ctl, NULL, 0, 0, 0) & 0xFF) != 0;
            break;
        case kRouteMount:
            r.mount(1, 0);
            break;
        case kRouteItemPanel:
            ran = item_panel(e, c.target, c.x, c.y, c.value);
            break;
        case kRouteMacro:
            r.macro_pick(*e.game.openers.macro_editor, NULL, static_cast<uint32_t>(value));
            break;
        }
        e.suppressed = 0;
        if (!ran) {
            if (spec->route == kRouteMsglist) {
                drain_error(e, "the game refused it: PlayOnline's mail is busy or not there");
            }
            return;
        }
    }
    expect_open(e, c, c.target, -1);
}

// What the game's Escape closes where a set's main window has one of its
// own, run for a close of that window: a macro palette's (the macro
// editor's close of mcrbedit, mcr1edit and mcr2edit, then titlewin's step
// back), the Status screen's (statcom2 closed by instance, which closes
// persona and mp_stat, and persona's close level), and textcol1's (textcol2
// closed by name, then textcol1), mp_mncst's (the game's routine: mp_stat,
// mp_cstm and mp_mncst closed by name) and cnqframe's (the fade's start,
// then its points windows and Colonization panes closed by name; `now`,
// for a block, closes the frame at once instead of fading it). False, with
// nothing done, for any other window, one whose route did not resolve, or a
// cnqframe already fading out.
inline bool escape_close(Engine& e, int n, bool now) {
    const RouteSites& r = e.game.routes;
    const char* nm = e.inv.names[n].name;
    if (strcmp(nm, "mcr1edit") == 0 || strcmp(nm, "mcr2edit") == 0) {
        if (!r.macro_close || !r.title || !e.game.openers.macro_editor || !*e.game.openers.macro_editor) {
            return false;
        }
        forget_open_checks(e, e.inv.find_exact("mcrbedit"));
        forget_open_checks(e, e.inv.find_exact("mcr1edit"));
        forget_open_checks(e, e.inv.find_exact("mcr2edit"));
        r.macro_close(*e.game.openers.macro_editor, NULL);
        if (uint8_t* title = controller_global(e, "titlewin")) {
            r.title(title, NULL, 0, 2);
        }
        return true;
    }
    if (strcmp(nm, "persona") == 0 || strcmp(nm, "mp_stat") == 0) {
        uint8_t* status = r.status_close ? open_named(e, "statcom2", NULL) : NULL;
        if (!status) {
            return false;
        }
        forget_open_checks(e, e.inv.find_exact("statcom2"));
        forget_open_checks(e, n);
        e.game.close_instance(e.game.mcb, NULL, status, 1);
        return true;
    }
    if (strcmp(nm, "textcol1") == 0 && r.textcol_escape) {
        forget_open_checks(e, e.inv.find_exact("textcol2"));
        forget_open_checks(e, n);
        close_named(e, "textcol2");
        close_named(e, "textcol1");
        return true;
    }
    if (strcmp(nm, "mp_mncst") == 0 && r.instincts_escape) {
        forget_open_checks(e, e.inv.find_exact("mp_stat"));
        forget_open_checks(e, e.inv.find_exact("mp_cstm"));
        forget_open_checks(e, n);
        r.instincts_escape();
        return true;
    }
    if (strcmp(nm, "cnqframe") == 0 && r.region_escape) {
        uint8_t* frame = controller_global(e, "cnqframe");
        if (!frame || (rd16(frame, kRegionClosingWord) & kRegionClosing)) {
            return false;
        }
        forget_open_checks(e, n);
        if (!now) {
            typedef void (__fastcall* FadeFn)(void* ctl, void* edx);
            FadeFn fade;
            const uint8_t* vt = rdptr(frame, 0);
            memcpy(&fade, vt + kRegionFadeSlot, sizeof(fade));
            fade(frame, NULL);
        }
        for (int i = 0; i < kRegionEscapeCloses; ++i) {
            const char* key = reinterpret_cast<const char*>(
                rdptr(r.region_escape, kRegionEscapeAt + static_cast<size_t>(i) * kRegionEscapeStep + 1));
            const int k = e.inv.find_key(key);
            if (k >= 0) {
                forget_open_checks(e, k);
            }
            e.game.close(e.game.mcb, NULL, key);
        }
        if (now) {
            close_named(e, "cnqframe");
        }
        return true;
    }
    return false;
}

// A window's own close, as close() runs it.
inline void own_close(Engine& e, int n, bool now) {
    forget_open_checks(e, n);
    if (map0_cancel(e, n) || escape_close(e, n, now)) {
        return;
    }
    char key[kKeyLen + 1];
    make_key(e, n, key);
    e.game.close(e.game.mcb, NULL, key);
}

// Whether a parent of c but window n is open.
inline bool other_parent_open(Engine& e, const ChildSpec& c, int n) {
    for (int i = 0; c.parents[i]; ++i) {
        const int p = e.inv.find_exact(c.parents[i]);
        if (p >= 0 && p != n && open_menu(e, p, NULL)) {
            return true;
        }
    }
    return false;
}

// The open children of window n with no other parent of theirs open, each
// closed through its own close, its own such children first: before n's
// close those its close leaves (not kParentCloses', nor one that may not
// close while n is open), as the player's Escape leaves a child before the
// window under it; `after` it, any still open.
inline void close_children(Engine& e, int n, bool after, bool now, int depth) {
    const char* nm = e.inv.names[n].name;
    for (int i = 0; i < kChildCount && depth < kChildDepth; ++i) {
        const ChildSpec& c = kChildren[i];
        const int k = child_of(c, nm) ? e.inv.find_exact(c.name) : -1;
        if (k < 0 || !open_menu(e, k, NULL) || other_parent_open(e, c, n)) {
            continue;
        }
        const PairSpec* kept = pair_spec(kCloseWhileOpen, kCloseWhileOpenCount, c.name);
        if (!after && (parent_closes(nm, c.name) || (kept && strcmp(kept->other, nm) == 0))) {
            continue;
        }
        close_children(e, k, false, now, depth + 1);
        own_close(e, k, now);
        if (!open_menu(e, k, NULL)) {
            close_children(e, k, true, now, depth + 1);
        }
    }
}

// close() and a block's close of window n: its orphans first, then its own
// close, then any child of it that close left open.
inline void close_with_children(Engine& e, int n, bool now) {
    close_children(e, n, false, now, 0);
    own_close(e, n, now);
    if (!open_menu(e, n, NULL)) {
        close_children(e, n, true, now, 0);
    }
}

// Whether open_set's opener of `set` and what it asks first are resolved.
inline bool set_resolved(const Engine& e, int set) {
    const OpenerSites& o = e.game.openers;
    const RouteSites& r = e.game.routes;
    const bool items = e.game.offer.item && e.game.offer.cache && e.game.offer.item_data && r.summary_type;
    switch (set) {
    case kSetEquipment: return o.equipment && o.monstrosity;
    case kSetAbility: return e.game.ability_list != NULL;
    case kSetMagic: return o.magic != NULL;
    case kSetMap: return o.map != NULL;
    case kSetSpells: return o.set_spells != NULL;
    case kSetAttachments: return o.attachments != NULL;
    case kSetLinkshell: return o.linkshell_check && o.linkshell;
    case kSetInstincts: return o.monstrosity && o.instincts;
    case kSetSynthesis: return items && o.use_check && o.synthesis && r.items_view && r.items_object
                            && r.inventor_items && r.inventor_policy && r.inventor_open;
    case kSetTabula: return items && o.tabula;
    case kSetMacros: return o.macro_editor && o.macros;
    case kSetFriend: return route_resolved(e, kRouteFriend);
    case kSetMail: return route_resolved(e, kRouteMsglist);
    case kSetMount: return route_resolved(e, kRouteMount);
    case kSetCompare: return route_resolved(e, kRouteItemPanel);
    case kSetStatus: return o.monstrosity && r.mp_stat && r.persona_layout;
    case kSetRegion: return r.region && r.player && r.area && r.region_kind;
    case kSetTextColors: return r.textcol[0] && r.textcol[1] && r.textcol[2];
    }
    return false;
}

// The synthesis's opener keeps the list inventor's controller holds, and its
// cancel reopens inventor with it. The game runs it from inventor's own
// list; before inventor has had one the controller holds whatever its
// allocation left there. A list of the game's has its routines in its code.
inline bool game_list(const OpenerSites& o, const uint8_t* inventor) {
    const uint8_t* list = inventor && readable(inventor + o.inventor_list, 4) ? rdptr(inventor, o.inventor_list) : NULL;
    const uint8_t* table = list && readable(list, 4) ? rdptr(list, 0) : NULL;
    const uint8_t* first = table && readable(table, 4) ? rdptr(table, 0) : NULL;
    return first >= o.code && first < o.code + o.code_size;
}

// The type word of the inventory item at `index` as the inventory list's
// rows read it, its record's id and index, the two the item use hands the
// openers, in *id and *at; -1 with the error reported when there is none.
inline int32_t set_item_type(Engine& e, int index, uint16_t* id, uint8_t* at) {
    const uint8_t* record = e.game.offer.item(0, static_cast<uint32_t>(index));
    *id = record ? static_cast<uint16_t>(rd16(record, 0)) : 0;
    *at = record ? record[2] : 0;
    if (!*id || *id == kGilId) {
        drain_error(e, "the inventory has no item at index %d", index);
        return -1;
    }
    char what[40];
    snprintf(what, sizeof(what), "the item at index %d", index);
    return item_type_of(e, *id, what);
}

// The main menu's Items row: the item view set to the inventory, then
// inventor opened with the list of the game's that row hands it.
inline bool items_row(Engine& e) {
    const RouteSites& r = e.game.routes;
    uint8_t* inventor = route_controller(e, "inventor");
    void* view = *r.items_object;
    if (!inventor) {
        return false;
    }
    if (!view) {
        drain_error(e, "the game's item view does not exist yet");
        return false;
    }
    r.items_view(view, NULL, 0, 1);
    inventor[r.inventor_items] = 1;
    r.inventor_open(inventor, NULL, static_cast<uint32_t>(reinterpret_cast<uintptr_t>(*r.inventor_policy)));
    return true;
}

// open_set(set[, argument]): the game's opener of the set, as its own menu
// row, shortcut, command or item use runs it, with what that runs first.
// The equipment set's is refused in Monstrosity, where the game's shortcut
// does not run it, and the instincts' outside it, where the main menu's row
// runs the equipment's instead; the linkshell list's when the row's check
// refuses; a list's set opens the list with its sort window and its detail
// pane; synthesis runs the main menu's Items row, then the item use's check
// and opener for a crystal at an inventory index, and the maze editor runs
// for a tabula, as the game's item use opens each; friend, mail and mount
// open with the windows their openers bring; compare is the item panel's
// second half as the inventory's cursor opens it; status is the main
// menu's Status row (mp_stat's opener in Monstrosity); region is the
// Region Info text command's opener for the player's area; text_colors is
// the text color menu's row. An opener's own refusal leaves its window
// closed: the game refused it.
inline void open_set(Engine& e, const Command& c) {
    const int set = c.x;
    const int32_t value = static_cast<int32_t>(c.value);
    const OpenerSites& o = e.game.openers;
    const RouteSites& r = e.game.routes;
    if (!set_resolved(e, set)) {
        drain_error(e, "the game's opener of the set is not resolved");
        return;
    }
    int opened = c.target;
    void* ctl = const_cast<uint8_t*>(controller_at(e, c.target));
    const bool needs_ctl = set == kSetSpells || set == kSetAttachments || set == kSetLinkshell
        || set == kSetSynthesis || set == kSetTabula || set == kSetFriend || set == kSetMail || set == kSetMount
        || set == kSetCompare || set == kSetRegion;
    if (needs_ctl && !ctl) {
        drain_error(e, "%s has no controller now, and the game's opener would crash the client",
            e.inv.names[c.target].name);
        return;
    }
    switch (set) {
    case kSetEquipment:
        if (o.monstrosity() & 0xFF) {
            drain_error(e, "the game refused it: it opens no equipment set in Monstrosity");
            return;
        }
        o.equipment(ctl, NULL);
        break;
    case kSetAbility:
        e.game.ability_list(value, 1, 1);
        break;
    case kSetMagic:
        o.magic(value, 1, 0);
        break;
    case kSetMap:
        o.map(0);
        break;
    case kSetSpells:
        o.set_spells(ctl, NULL);
        break;
    case kSetAttachments:
        o.attachments(ctl, NULL);
        break;
    case kSetLinkshell:
        if (o.linkshell_check(1, 1) & 0xFF) {
            drain_error(e, "the game refused it");
            return;
        }
        o.linkshell(ctl, NULL, 0, 0, 0);
        break;
    case kSetInstincts:
        if (!(o.monstrosity() & 0xFF)) {
            drain_error(e, "the game refused it: it opens instincts only in Monstrosity");
            return;
        }
        o.instincts();
        break;
    case kSetSynthesis: {
        uint16_t id = 0;
        uint8_t at = 0;
        const int32_t type = set_item_type(e, value, &id, &at);
        if (type < 0) {
            return;
        }
        if (type != kItemCrystal) {
            drain_error(e, "the item at index %d is not a crystal", value);
            return;
        }
        if (!items_row(e)) {
            return;
        }
        if (!game_list(o, controller_at(e, e.inv.find_exact("inventor")))) {
            drain_error(e, "the game's Items row left inventor with no list of the game's, which the synthesis's"
                " cancel would reopen it with");
            return;
        }
        if (!(o.use_check(0x77, 0) & 0xFF) || !(o.synthesis(ctl, NULL, at, id, 0) & 0xFF)) {
            drain_error(e, "the game refused it");
            return;
        }
        break;
    }
    case kSetTabula: {
        uint16_t id = 0;
        uint8_t at = 0;
        const int32_t type = set_item_type(e, value, &id, &at);
        if (type < 0) {
            return;
        }
        if (type != kItemTabulaM && type != kItemTabulaR) {
            drain_error(e, "the item at index %d is not a Maze Tabula", value);
            return;
        }
        o.tabula(ctl, NULL, type == kItemTabulaM ? 0 : 1, at);
        break;
    }
    case kSetMacros: {
        uint8_t* editor = *o.macro_editor;
        if (!editor) {
            drain_error(e, "the game's macro editor does not exist yet");
            return;
        }
        o.macros(editor, NULL);
        break;
    }
    case kSetFriend:
        r.friend_list(ctl, NULL, 0);
        break;
    case kSetMail:
        if (!(r.msglist(ctl, NULL, 0, 0, 0) & 0xFF)) {
            drain_error(e, "the game refused it: PlayOnline's mail is busy or not there");
            return;
        }
        break;
    case kSetMount:
        r.mount(1, 1);
        break;
    case kSetCompare:
        if (!parent_open(e, c.target)) {
            char why[kErrorReason];
            child_reason(*child_spec(e.inv.names[c.target].name), why, sizeof(why));
            drain_error(e, "%s", why);
            return;
        }
        if (!item_panel(e, c.target, c.z, c.y, c.value)) {
            return;
        }
        break;
    case kSetStatus:
        if (o.monstrosity() & 0xFF) {
            uint8_t* stat = route_controller(e, "mp_stat");
            if (!stat) {
                return;
            }
            r.mp_stat(stat, NULL, 0);
            opened = e.inv.find_exact("mp_stat");
        } else {
            uint8_t* persona = route_controller(e, "persona");
            if (!persona) {
                return;
            }
            persona[r.persona_layout] = 1;
            open_by_name(e, "persona");
        }
        break;
    case kSetRegion: {
        if (open_menu(e, c.target, NULL)) {
            drain_error(e, "the game refused it: cnqframe is open already");
            return;
        }
        const uint8_t* player = *r.player;
        if (!player) {
            drain_error(e, "the game refused it: the player is not in an area");
            return;
        }
        r.region(ctl, NULL, value - 1, static_cast<int32_t>(rd32(player, r.area)));
        break;
    }
    case kSetTextColors: {
        static const char* const windows[] = {"textcol1", "textcol2", "textcol3"};
        uint8_t* ctls[3];
        for (int i = 0; i < 3; ++i) {
            if (!(ctls[i] = route_controller(e, windows[i]))) {
                return;
            }
        }
        for (int i = 0; i < 3; ++i) {
            wr32(ctls[i], r.textcol[i], static_cast<uint32_t>(value - 1));
        }
        open_by_name(e, "textcol2");
        open_by_name(e, "textcol1");
        break;
    }
    }
    expect_open(e, c, opened, set);
}

// ---------------------------------------------------------------------------
// Settings (core.h's kSettings). The reads come from the store's entries and
// the player's flags, guarded, on any thread; the sets run on the game
// thread, as each page writes.

// The store's entry for id: NULL while the store or its entry is not there.
inline const uint8_t* store_entry(const SettingSites& s, uint8_t id) {
    const uint8_t* store = s.store ? *s.store : NULL;
    const uint8_t* entries = readable(store, kStoreLoaded + 1) ? rdptr(store, kStoreEntries) : NULL;
    const uint8_t* e = entries ? entries + id * kEntryBytes : NULL;
    return readable(e, kEntryBytes) && rd32(e, kEntryId) == id ? e : NULL;
}

// The player's values are in the store: the player is in the game.
inline bool store_loaded(const SettingSites& s) {
    const uint8_t* store = s.store ? *s.store : NULL;
    return readable(store, kStoreLoaded + 1) && store[kStoreLoaded] == 1;
}

inline bool store_word(const SettingSites& s, uint8_t id, size_t field, int32_t* out) {
    const uint8_t* e = store_entry(s, id);
    if (!e) {
        return false;
    }
    *out = static_cast<int32_t>(rd32(e, field));
    return true;
}

inline bool filter_words(const SettingSites& s, size_t field, FilterWords* w) {
    int32_t v[5];
    for (int k = 0; k < 5; ++k) {
        if (k != 3 && !store_word(s, kFilterWords[k], field, &v[k])) {
            return false;
        }
    }
    w->masks[0] = static_cast<uint32_t>(v[0]);
    w->masks[1] = static_cast<uint32_t>(v[1]);
    w->second = static_cast<uint32_t>(v[2]);
    w->flag = static_cast<uint32_t>(v[4]);
    return true;
}

// Setting i as its store keeps it: the word, the bit (0 or 1), the chat
// filter row's state, the player's flag (0 or 1); with `defaults`, the
// default. False while what it reads is not there, and for the flag's
// default, which the game keeps none of.
inline bool setting_raw(const SettingSites& s, int i, bool defaults, int32_t* out) {
    const SettingSpec& spec = kSettings[i];
    const SettingSlot& slot = s.slot[i];
    const size_t field = defaults ? kEntryDefault : kEntryValue;
    switch (spec.store) {
    case kStoreBit: {
        int32_t word = 0;
        if (!store_word(s, bit_word(slot.id, slot.bit), field, &word)) {
            return false;
        }
        *out = (static_cast<uint32_t>(word) & bit_mask(slot.bit)) ? 1 : 0;
        return true;
    }
    case kStoreFilter: {
        FilterWords w;
        if (!filter_words(s, field, &w)) {
            return false;
        }
        *out = filter_state(slot.row, w);
        return true;
    }
    case kStoreFlag: {
        const uint8_t* player = s.player ? *s.player : NULL;
        if (defaults || !readable(player ? player + s.flags : NULL, 1)) {
            return false;
        }
        *out = (player[s.flags] & s.anonymous) ? 1 : 0;
        return true;
    }
    default:
        return store_word(s, slot.id, field, out);
    }
}

// The values setting i takes as setting_raw() reads it, lo..hi: a word's
// store range (a color's 0..255 for each channel), 0..1 for a bit and the
// player's flag, a filter row's states. False while what it reads is not
// there.
inline bool setting_range(const SettingSites& s, int i, int32_t* lo, int32_t* hi) {
    const SettingSpec& spec = kSettings[i];
    if (spec.value == kValueColor) {
        *lo = 0;
        *hi = 255;
        return true;
    }
    if (spec.store == kStoreBit || spec.store == kStoreFlag) {
        *lo = 0;
        *hi = 1;
        return true;
    }
    if (spec.store == kStoreFilter) {
        *lo = 0;
        *hi = filter_states(s.slot[i].row) - 1;
        return true;
    }
    return store_word(s, s.slot[i].id, kEntryMin, lo) && store_word(s, s.slot[i].id, kEntryMax, hi);
}

// The open page that would undo a set of `spec` or apply it mid-edit: its
// own page, if that edits a copy and writes it back as it closes; for a key,
// either key page, since both edit the copy in k1assign's controller, which
// every key's subscriber applies from. NULL for none.
inline const char* committing_page(Engine& e, const SettingSpec& spec) {
    if (!(spec.flags & kCommits)) {
        return NULL;
    }
    if (spec.from == kFromKeys) {
        for (size_t k = 0; k < sizeof(kKeyPages) / sizeof(kKeyPages[0]); ++k) {
            if (open_named(e, kKeyPages[k], NULL)) {
                return kKeyPages[k];
            }
        }
        return NULL;
    }
    return open_named(e, spec.page, NULL) ? spec.page : NULL;
}

// set_setting, as the setting's page writes it: a word through the store's
// set, which runs the id's subscribers; a bit or a filter row composed into
// the words as they are now, each word that changes set in the order the
// page writes them; the equipment lock through the game's request alone,
// which writes 0xBF only once its request goes out, so that a refused one
// leaves the value as it was; the idle timer and the player flag through
// their routines. Refused while the player's values are not loaded, and
// while a page that writes its own copy back as it closes is open, which
// would put its copy over this.
inline void setting_command(Engine& e, const Command& c) {
    if (c.target < 0 || c.target >= kSettingCount) {
        return;
    }
    const SettingSpec& spec = kSettings[c.target];
    const SettingSites& s = e.game.settings;
    const SettingSlot& slot = s.slot[c.target];
    const int32_t v = static_cast<int32_t>(c.value);
    if (!store_loaded(s)) {
        drain_error(e, "the game's settings are not loaded: the player is not in the game");
        return;
    }
    if (const char* page = committing_page(e, spec)) {
        drain_error(e, spec.from == kFromKeys
                ? "%s is open: the two key pages share one copy of the keys, and a set now would apply it in the"
                  " middle of the page's edit; set it once %s has closed"
                : "%s is open: it writes its own copy back as it closes, over this; set it once %s has closed",
            page, page);
        return;
    }
    HU_HEAP_CHECK("before set_setting %s", spec.name);
    switch (spec.store) {
    case kStoreWord:
        s.set(slot.id, v);
        break;
    case kStoreBit: {
        const uint8_t id = bit_word(slot.id, slot.bit);
        int32_t word = 0;
        if (!store_word(s, id, kEntryValue, &word)) {
            drain_error(e, "the game's setting is not in its store");
            return;
        }
        const uint32_t m = bit_mask(slot.bit);
        s.set(id, static_cast<int32_t>(v ? static_cast<uint32_t>(word) | m : static_cast<uint32_t>(word) & ~m));
        break;
    }
    case kStoreFilter: {
        FilterWords before;
        if (!filter_words(s, kEntryValue, &before)) {
            drain_error(e, "the game's setting is not in its store");
            return;
        }
        FilterWords after = before;
        filter_compose(slot.row, v, &after);
        const uint32_t was[5] = {before.masks[0], before.masks[1], before.second, 0, before.flag};
        const uint32_t now[5] = {after.masks[0], after.masks[1], after.second, 0, after.flag};
        for (int k = 0; k < 5; ++k) {
            if (now[k] != was[k]) {
                s.set(kFilterWords[k], static_cast<int32_t>(now[k]));
            }
        }
        break;
    }
    case kStoreLock: {
        // The request's al does not tell its wait's refusal from a send;
        // its stamp, which only a request past the gate and the wait moves,
        // does.
        int32_t was = 0;
        store_word(s, slot.id, kEntryValue, &was);
        const uint32_t stamp = rd32(s.lock_stamp, 0);
        const uint32_t now = s.lock_clock();
        const bool went = (s.lock(v ? (was == 1 ? kLockKept : kLockOn) : kLockOff, 0, 0) & 0xFF) != 0;
        if (rd32(s.lock_stamp, 0) == stamp && !went) {
            drain_error(e, "the game is not accepting style lock changes right now");
        } else if (rd32(s.lock_stamp, 0) == stamp) {
            const uint32_t waited = now - stamp;
            const uint32_t left = waited < s.lock_wait ? s.lock_wait - waited : 0;
            const uint32_t seconds = left ? (left + 999) / 1000 : 1;
            drain_error(e, "the game allows one style lock change every %u seconds; %u s left",
                static_cast<unsigned>((s.lock_wait + 999) / 1000), static_cast<unsigned>(seconds));
        } else if (!went) {
            drain_error(e, "the game could not send the request");
        }
        break;
    }
    case kStoreIdle:
        s.idle(v > 0 ? v * kIdleSecondsPerUnit : 0);
        break;
    case kStoreFlag:
        s.flag(kFlagAnonymous, v);
        break;
    default:
        break;
    }
    HU_HEAP_CHECK("after set_setting %s", spec.name);
}

// The settings' labels, read once the game's message getter answers: the
// game's text, cut short of kLabelBytes at a whole character.
inline void fill_setting_labels(Engine& e) {
    SettingLabels& l = e.setting_labels;
    const MessageFn get = e.game.dead.message;
    if (l.ready || !get || !e.game.settings.store) {
        return;
    }
    int first = 0;
    while (first < kSettingCount && kSettings[first].label < 0) {
        ++first;
    }
    const char* probe = first < kSettingCount ? get(kLabelTable, kSettings[first].label) : NULL;
    if (!probe || !readable(probe, 1) || !probe[0]) {
        return;
    }
    for (int i = 0; i < kSettingCount; ++i) {
        char* out = l.text[i];
        out[0] = '\0';
        if (kSettings[i].label < 0) {
            continue;
        }
        copy_text(get(kLabelTable, kSettings[i].label), out, kLabelBytes);
        const size_t n = strlen(out);
        size_t k = 0;
        while (k < n) {
            k += text_lead_byte(static_cast<uint8_t>(out[k])) ? 2 : 1;
        }
        if (k > n) {
            out[n - 1] = '\0';
        }
    }
    MemoryBarrier();
    InterlockedExchange(&l.ready, 1);
}

// The count of opens that names window n's prompt now: its own, or a
// dialogue box's the pair's, the two boxes being one dialogue.
inline uint32_t open_count(const Engine& e, int n) {
    return static_cast<uint32_t>(e.inv.dialogue_of(n) >= 0 ? e.dialogue_opens : e.instance[n]);
}

inline void run_command(Engine& e, const Command& c) {
    HU_STEP_AT("run_command", c.op);
    if ((c.flags & kCmdInstance) && c.target >= 0 && c.target < e.inv.count) {
        const int session = (c.flags & kCmdSession) ? session_window(e.inv.names[c.target].name) : -1;
        const uint32_t current = session >= 0 ? current_session(e, session) : open_count(e, c.target);
        if (current != c.instance) {
            drain_error(e, "that prompt is gone");
            return;
        }
    }
    switch (c.op) {
    case kOpMove: {
        const int n = c.target;
        const bool by_frame = (c.flags & kCmdFrame) != 0;
        if (is_targetarrow(e, n)) {
            break;
        }
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
            pin_frame_move(e, n, menu);
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
        if (open_refusal(e.inv.names[c.target].name, 1)) {
            drain_error(e, "%s", open_refusal(e.inv.names[c.target].name, 1));
            break;
        }
        if (!parent_open(e, c.target)) {
            char why[kErrorReason];
            child_reason(*child_spec(e.inv.names[c.target].name), why, sizeof(why));
            drain_error(e, "%s", why);
            break;
        }
        if (controller_missing(e, c.target)) {
            drain_error(e, "%s has no controller now, and the game's open would crash the client",
                e.inv.names[c.target].name);
            break;
        }
        char key[kKeyLen + 1];
        make_key(e, c.target, key);
        if (!e.game.open(e.game.mcb, NULL, key, 1, open_overlap(e.inv.names[c.target].name))) {
            drain_error(e, "the game refused it");
        }
        break;
    }
    case kOpOpenList:
        open_list(e, c);
        break;
    case kOpOpenSet:
        open_set(e, c);
        break;
    case kOpOpenRoute:
        open_route(e, c);
        break;
    case kOpSetSetting:
        setting_command(e, c);
        break;
    case kOpClose: {
        if ((c.flags & kCmdPrompt) && prompt_asks(e, c.target)) {
            drain_error(e, "%s is a prompt the game waits on; use cancel('%s')", e.inv.names[c.target].name,
                e.inv.names[c.target].name);
            break;
        }
        forget_open_checks(e, c.target);
        const PairSpec* other = pair_spec(kCloseWhileOpen, kCloseWhileOpenCount, e.inv.names[c.target].name);
        if (other && open_menu(e, e.inv.find_exact(other->other), NULL)) {
            drain_error(e, "%s", other->reason);
            break;
        }
        if (!close_refusal(e.inv.names[c.target].name)) {
            close_with_children(e, c.target, false);
        }
        break;
    }
    case kOpBlockClose: {
        // Closed since the block: nothing to close. Close by instance marks
        // the menu at once, so one still open after the call was refused.
        if (block_refusal(e.inv.names[c.target].name) || !open_menu(e, c.target, NULL)) {
            break;
        }
        forget_open_checks(e, c.target);
        close_with_children(e, c.target, true);
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
    case kOpYesNoAnswer:
    case kOpYesNoCancel:
        yesno_reply(e, c);
        break;
    case kOpDeadConfirm:
        dead_confirm(e);
        break;
    case kOpBoxAnswer:
    case kOpBoxCancel:
        box_reply(e, c);
        break;
    case kOpAskedAnswer:
    case kOpAskedCancel:
        asked_reply(e, c);
        break;
    case kOpDialogueAnswer:
    case kOpDialogueCancel:
        dialogue_reply(e, c);
        break;
    case kOpTextAnswer:
    case kOpTextCancel:
        text_reply(e, c);
        break;
    case kOpOffer:
    case kOpWithdraw:
    case kOpOfferAnswer:
    case kOpOfferCancel:
        offer_reply(e, c);
        break;
    case kOpResizeRows:
    case kOpResizeRect:
        resize(e, c);
        break;
    case kOpRestoreAll:
        for (int g = 0; g < kGroupCount; ++g) {
            e.waiting.clear(g);
        }
        reconcile_hidden(e);
        for (int n = 0; n < e.inv.count; ++n) {
            const Placement& p = e.place[n];
            const MemEntry& m = e.memory.e[n];
            if (p.touched || p.registry_undocked || p.size_kind || m.active || m.size_kind || e.anchor[n]) {
                reset_one(e, n, true, true);
                HU_HEAP_CHECK("restore_all: after reset %s", e.inv.names[n].name);
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
    HU_STEP("build_glyph_table");
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

// ---------------------------------------------------------------------------
// The number boxes. A box asks while its window is up and its callback set;
// with its open refused, while the question its record took stands.

inline int32_t box_word(const uint8_t* ctl, size_t off, uint8_t width) {
    return width == 1 ? static_cast<int8_t>(ctl[off]) : static_cast<int32_t>(rd32(ctl, off));
}

inline int32_t box_max(const uint8_t* ctl, int b) {
    return box_word(ctl, kBoxLayouts[b].max, kBoxLayouts[b].width);
}

inline int32_t box_value(const uint8_t* ctl, int b) {
    return box_word(ctl, kBoxLayouts[b].value, kBoxLayouts[b].width);
}

inline const void* box_callback(const uint8_t* ctl, int b) {
    return rdptr(ctl, kBoxLayouts[b].callback);
}

// The known asker that asks with `callback`, or -1.
inline int asker_of(const Engine& e, const void* callback) {
    for (int i = 0; callback && i < kAskerCount; ++i) {
        if (e.game.askers[i] == callback) {
            return i;
        }
    }
    return -1;
}

inline int32_t asker_field(const uint8_t* ctl, const AskerSpec& a) {
    if (a.field_bytes == 1) {
        return ctl[a.field];
    }
    if (a.field_bytes == 2) {
        return static_cast<uint16_t>(rd16(ctl, a.field));
    }
    return static_cast<int32_t>(rd32(ctl, a.field));
}

// Why box b's blocked question in `r` no longer stands, or NULL while it
// does. `ctl` is the controller in the box's registry slot now, its
// layout's bytes readable, NULL for none; `owner` the asker's window open
// now and `owner_ctl` its controller through its registry slot, the field
// readable, each NULL for none.
inline const char* box_record_gone(const BoxRecord& r, int b, const uint8_t* ctl, const uint8_t* owner,
                                   const uint8_t* owner_ctl) {
    const BoxLayout& l = kBoxLayouts[b];
    if (!r.active) {
        return "no question was kept from its window";
    }
    if (!ctl || ctl != r.ctl) {
        return "the box's controller is another";
    }
    if (rdptr(ctl, l.callback) != r.callback || rdptr(ctl, l.context) != r.context) {
        return "the box holds another question, or none";
    }
    if (box_max(ctl, b) != r.max) {
        return "the box was asked again";
    }
    if (r.asker < 0) {
        return NULL;
    }
    const AskerSpec& a = kAskers[r.asker];
    if (owner != r.owner) {
        return "the window that asked has closed";
    }
    if (a.field && a.field_ends && (!owner_ctl || asker_field(owner_ctl, a) != r.field)) {
        return "the window that asked has moved on";
    }
    return NULL;
}

// Whether box b asks now: kept from its window while the question its
// record `r` took stands (*blocked), else with its window open (`open`) and
// its callback set. `ctl`, `owner` and `owner_ctl` as box_record_gone()
// takes them.
inline bool box_asks(const BoxRecord& r, int b, const uint8_t* ctl, const uint8_t* owner, const uint8_t* owner_ctl,
                     bool open, bool* blocked) {
    *blocked = r.active && !box_record_gone(r, b, ctl, owner, owner_ctl);
    return *blocked || (ctl && open && box_callback(ctl, b));
}

// Why a reply to box b's blocked question in `r` is refused now, or NULL:
// `answer` for a number, else the cancel, which no known asker's callback
// needs anything more for. The arguments are box_record_gone's for a record
// that stands.
inline const char* box_blocked_refusal(const BoxRecord& r, int b, const uint8_t* owner, const uint8_t* owner_ctl,
                                       bool answer, char* buf, size_t size) {
    if (r.asker < 0) {
        snprintf(buf, size, "%s's question came from a window hideui does not know, so it takes no reply while %s"
            " is blocked", kBoxNames[b], kBoxNames[b]);
        return buf;
    }
    if (!answer) {
        return NULL;
    }
    const AskerSpec& a = kAskers[r.asker];
    if (a.blocked) {
        snprintf(buf, size, "%s", a.blocked);
        return buf;
    }
    if (!r.owner || !owner) {
        snprintf(buf, size, "%s, which asked, is not open", a.owner);
        return buf;
    }
    if (a.row && rd16(owner, kMenuCursor) != r.row) {
        snprintf(buf, size, "%s's cursor is not on the row the question was asked from", a.owner);
        return buf;
    }
    if (a.field && !a.field_ends && (!owner_ctl || asker_field(owner_ctl, a) != r.field)) {
        snprintf(buf, size, "%s", a.moved);
        return buf;
    }
    return NULL;
}

// Why an answer to a box's question with its window up is refused, or
// NULL: a callback that reads its asker's window unchecked needs that window
// open. `asker` is asker_of() the callback.
inline const char* box_window_refusal(int asker, bool owner_open, bool answer, char* buf, size_t size) {
    if (!answer || asker < 0 || !kAskers[asker].window || owner_open) {
        return NULL;
    }
    snprintf(buf, size, "%s, which asked, is not open", kAskers[asker].owner);
    return buf;
}

// The asker's window open now, and its controller.
inline const uint8_t* box_owner(Engine& e, const BoxRecord& r, const uint8_t** owner_ctl) {
    *owner_ctl = r.owner_n >= 0 ? controller_at(e, r.owner_n) : NULL;
    return r.owner_n >= 0 ? open_menu(e, r.owner_n, NULL) : NULL;
}

// Box b's record ended, and a pending event saying so for every handle but
// the one whose reply ended it (slot -1 for none).
inline void box_end(Engine& e, int b, int32_t slot, uint32_t gen) {
    BoxRecord& r = e.boxes[b];
    if (!r.active) {
        return;
    }
    InterlockedIncrement(&r.seq);
    r.active = 0;
    InterlockedIncrement(&r.seq);
    e.events.post(pack_pending(kPendingBox + b, false, slot, gen));
}

// A refused open of box b, inside its setup: the question that setup has
// just written, taken with its asker's window, cursor row and field as they
// stand, and announced. A controller with no callback set asks nothing.
inline void box_refused(Engine& e, int b) {
    const int n = e.inv.box_window[b];
    const BoxLayout& l = kBoxLayouts[b];
    const uint8_t* ctl = controller_at(e, n);
    const void* cb = ctl ? box_callback(ctl, b) : NULL;
    if (!cb) {
        return;
    }
    const int asker = asker_of(e, cb);
    const AskerSpec* a = asker >= 0 ? &kAskers[asker] : NULL;
    const int owner_n = a ? e.inv.find_exact(a->owner) : -1;
    const uint8_t* owner = owner_n >= 0 ? open_menu(e, owner_n, NULL) : NULL;
    const uint8_t* owner_ctl = owner_n >= 0 ? controller_at(e, owner_n) : NULL;
    int32_t field = 0;
    if (a && a->field) {
        field = a->field_value != kFieldAtAsk ? a->field_value : owner_ctl ? asker_field(owner_ctl, *a) : 0;
    }
    BoxRecord& r = e.boxes[b];
    InterlockedIncrement(&r.seq);
    r.asker = static_cast<int8_t>(asker);
    r.owner_n = static_cast<int16_t>(owner_n);
    r.id = static_cast<uint32_t>(e.instance[n]);
    r.ctl = ctl;
    r.callback = cb;
    r.context = rdptr(ctl, l.context);
    r.max = box_max(ctl, b);
    r.owner = owner;
    r.row = owner ? rd16(owner, kMenuCursor) : 0;
    r.field = field;
    r.active = 1;
    InterlockedIncrement(&r.seq);
    e.events.post(pack_pending(kPendingBox + b, true));
}

// Each blocked question that no longer stands ended: the drain sees the
// game take it down when the window that asked closes or moves on, the box
// is asked again, or its confirm runs from elsewhere.
inline void watch_boxes(Engine& e) {
    HU_STEP("watch_boxes");
    for (int b = 0; b < kBoxCount; ++b) {
        const BoxRecord& r = e.boxes[b];
        if (!r.active) {
            continue;
        }
        const uint8_t* owner_ctl = NULL;
        const uint8_t* owner = box_owner(e, r, &owner_ctl);
        if (box_record_gone(r, b, controller_at(e, e.inv.box_window[b]), owner, owner_ctl)) {
            box_end(e, b, -1, 0);
        }
    }
}

// The window's own confirm with the number, or its cancel with
// kBoxCancel: the box's confirm routine, which calls the callback, clears
// it and closes the window when there is one. A blocked question's record
// ends first, its pending event skipped by the handle that replied.
inline void box_reply(Engine& e, const Command& c) {
    const int b = e.inv.box_of(c.target);
    if (b < 0) {
        return;
    }
    const char* nm = kBoxNames[b];
    const bool answer = c.op == kOpBoxAnswer;
    uint8_t* ctl = controller_global(e, nm);
    const BoxRecord& r = e.boxes[b];
    const uint8_t* owner_ctl = NULL;
    const uint8_t* owner = r.active ? box_owner(e, r, &owner_ctl) : NULL;
    bool blocked = false;
    if (!box_asks(r, b, ctl, owner, owner_ctl, open_menu(e, c.target, NULL) != NULL, &blocked)) {
        drain_error(e, "no number is pending in %s", nm);
        return;
    }
    char buf[kErrorReason];
    const char* why = NULL;
    if (blocked) {
        why = box_blocked_refusal(r, b, owner, owner_ctl, answer, buf, sizeof(buf));
    } else {
        const int asker = asker_of(e, box_callback(ctl, b));
        const bool owner_open = asker >= 0 && open_named(e, kAskers[asker].owner, NULL);
        why = box_window_refusal(asker, owner_open, answer, buf, sizeof(buf));
    }
    if (why) {
        drain_error(e, "%s", why);
        return;
    }
    const int32_t value = static_cast<int32_t>(c.value);
    const int32_t max = box_max(ctl, b);
    if (answer && (value < kBoxLayouts[b].min || value > max)) {
        drain_error(e, "%d is outside %d..%d, what %s takes now", static_cast<int>(value),
            static_cast<int>(kBoxLayouts[b].min), static_cast<int>(max), nm);
        return;
    }
    if (blocked) {
        box_end(e, b, c.slot, c.gen);
    }
    e.game.box_confirm[b](ctl, NULL, answer ? value : kBoxCancel);
}

// ---------------------------------------------------------------------------
// The Asked windows (core.h's kAskedNames). Each asks while its window is
// up with its question set; sortyn, evitem01, maplist, ok and mogdoor, with
// their open refused, while the question their record took stands; bankmenu
// while its callback is set, which its window's close clears and a refused
// open leaves.

inline bool asked_records(int a) {
    return a == kAskedSortyn || a == kAskedEvitem || a == kAskedMaplist || a == kAskedOk || a == kAskedDoor;
}

// The bytes of Asked window a's controller its replies and readers use.
inline size_t asked_size(int a) {
    static const size_t sizes[kAskedCount] = {kSortynBytes, kEvitemBytes, kConfynBytes, kBankBytes, kMapBytes,
                                              kQueryResult + 2, kMpDeadBytes, kOkBytes, kDoorBytes};
    return a >= 0 && a < kAskedCount ? sizes[a] : 0;
}

// An Asked window as a reply or a read finds it, every pointer checked by
// whoever filled it in: its controller through its registry slot,
// asked_size() bytes of it readable; its window open now; for a record, the
// window that asked open now, and evitem01's key item list's controller
// with kEvitemListBytes readable; bankmenu's Mog Safe 2 flag, 1 or 0, -1
// unknown; mogdoor's Mog House object, the door actor with kDoorOpened + 4
// bytes readable, the exit's first byte (-1 unknown) and the floor word
// (-1 unknown). NULL for each that is not there.
struct AskedNow {
    const uint8_t* ctl;
    const uint8_t* menu;
    const uint8_t* owner;
    const uint8_t* list;
    int32_t safe2;
    const uint8_t* house;
    const uint8_t* door;
    int32_t exiting;
    int32_t floor;
};

// The rows maplist's controller holds, each with its entry readable; 0 with
// no array, as after close+reset.
inline int map_count(const uint8_t* ctl) {
    const int count = static_cast<int32_t>(rd32(ctl, kMapCount));
    const int capacity = static_cast<int32_t>(rd32(ctl, kMapCapacity));
    const uint8_t* rows = rdptr(ctl, kMapRows);
    if (!rows || count <= 0 || count > capacity || count > kMapRowMax || !readable(rows, count * kAreaRowStride)) {
        return 0;
    }
    return count;
}

// Row i's entry, 0-based, readable; NULL when it is not.
inline const uint8_t* map_entry(const uint8_t* ctl, int i) {
    const uint8_t* rows = rdptr(ctl, kMapRows);
    const uint8_t* entry = rdptr(rows + i * kAreaRowStride, kAreaRowEntry);
    return readable(entry, 8) ? entry : NULL;
}

// The list shows the regions, where its cancel closes it.
inline bool map_top(const uint8_t* ctl) {
    return rd16(ctl, kMapZone) < 0 && rd16(ctl, kMapRegion) == 0;
}

// The level the list shows, as its key handler's cancel tells them apart:
// the regions at its top, a region's list, or one zone's maps.
enum { kMapLevelTop, kMapLevelRegion, kMapLevelZone };
inline int map_level(const uint8_t* ctl) {
    return rd16(ctl, kMapZone) >= 0 ? kMapLevelZone : rd16(ctl, kMapRegion) != 0 ? kMapLevelRegion : kMapLevelTop;
}

// The list shown, kMapRegion's word and kMapZone's in one: either changes
// at every step between levels.
inline int32_t map_where(const uint8_t* ctl) {
    return static_cast<int32_t>(static_cast<uint16_t>(rd16(ctl, kMapRegion))
        | static_cast<uint32_t>(static_cast<uint16_t>(rd16(ctl, kMapZone))) << 16);
}

// The player's data, which maplist's key handler needs before any confirm or
// cancel, and its rebuild of the top reads unchecked. Either thread.
inline bool map_data_loaded(const Engine& e) {
    uint8_t** data = e.game.asked.bank_data;
    return data && readable(data, 4) && *data;
}

// mogdoor's world as either thread finds it, every pointer guarded: the
// Mog House object, its door actor, the exit's first byte and the floor
// word.
inline void door_now(const Engine& e, AskedNow* now) {
    const DoorSites& s = e.game.asked.door;
    now->house = s.house && readable(s.house, 4) ? *s.house : NULL;
    const uint8_t* door = now->house && readable(now->house + kHouseDoor, 4) ? rdptr(now->house, kHouseDoor) : NULL;
    now->door = door && readable(door, kDoorOpened + 4) ? door : NULL;
    now->exiting = s.exiting[0] && readable(s.exiting[0], 1) ? s.exiting[0][0] : -1;
    uint8_t** data_global = e.game.asked.bank_data;
    const uint8_t* data = data_global && readable(data_global, 4) ? *data_global : NULL;
    now->floor = data && readable(data + s.floor_word, 2) ? static_cast<uint16_t>(rd16(data, s.floor_word)) : -1;
}

// mogdoor's list of areas, at most kDoorListMax zones into `zones`: the
// window's own while it is up, no more than its count, else the one its
// open hook would take for `nation`. 0 when none can be read.
inline int door_areas(const Engine& e, const uint8_t* ctl, bool window, int32_t nation, uint16_t* zones) {
    const DoorSites& s = e.game.asked.door;
    const uint8_t* list = NULL;
    int most = kDoorListMax;
    if (window) {
        list = rdptr(ctl, kDoorList);
        const int count = static_cast<int8_t>(ctl[kDoorCount]);
        most = count < most ? count : most;
    } else if (nation >= 1 && nation <= s.nations && readable(s.table + 4 * nation, 4)) {
        list = rdptr(s.table, 4 * static_cast<size_t>(nation));
    }
    int n = 0;
    while (n < most && list && readable(list + 2 * n, 2) && rd16(list, 2 * n) != 0) {
        zones[n] = static_cast<uint16_t>(rd16(list, 2 * n));
        ++n;
    }
    return n;
}

// The exit area i of the list sends: its place, but the Mog Garden's own.
inline int32_t door_area_exit(const Engine& e, uint16_t zone, int i) {
    return zone == e.game.asked.door.garden ? kDoorGardenExit : i + 1;
}

// The exit the floor row sends, or -1 while the floor is not known.
inline int32_t door_floor_exit(const AskedNow& now) {
    return now.floor < 0 ? -1 : kDoorFloorUp + (now.floor ? 1 : 0);
}

// Why mogdoor offers no exit `value` now, or NULL: the back row, the floor
// row while enabled, or an area of the list, the Mog Garden's while
// enabled. `nation` is the record's, for a question its block kept from its
// window.
inline const char* door_offer_refusal(const Engine& e, const AskedNow& now, bool window, int32_t nation,
                                      int32_t value, char* buf, size_t size) {
    if (value == kDoorBack) {
        return NULL;
    }
    if (value == kDoorFloorUp || value == kDoorFloorUp + 1) {
        const int32_t floor = door_floor_exit(now);
        if (floor < 0) {
            snprintf(buf, size, "the player's data is not loaded, so the floor the player is on is not known");
        } else if (value != floor) {
            snprintf(buf, size, "the player is on the other floor: mogdoor's floor row is %d", static_cast<int>(floor));
        } else if (!now.ctl[kDoorFloor]) {
            snprintf(buf, size, "mogdoor's floor row is turned off now");
        } else {
            return NULL;
        }
        return buf;
    }
    uint16_t zones[kDoorListMax];
    const int count = door_areas(e, now.ctl, window, nation, zones);
    for (int i = 0; i < count; ++i) {
        if (door_area_exit(e, zones[i], i) != value) {
            continue;
        }
        if (zones[i] == e.game.asked.door.garden && !now.ctl[kDoorGarden]) {
            snprintf(buf, size, "the Mog Garden is not open to the player now");
            return buf;
        }
        return NULL;
    }
    snprintf(buf, size, "mogdoor offers no exit %d (options('mogdoor') lists them)", static_cast<int>(value));
    return buf;
}

// The texts mogdoor's draw shows, read with the game's message getter on
// the game thread for prompt `id`: the title, the top's rows, the back
// row's by the draw's own condition, and each area's name.
inline void door_texts(Engine& e, uint32_t id, const uint16_t* zones, int count) {
    const DoorSites& s = e.game.asked.door;
    const MessageFn get = e.game.dead.message;
    DoorTexts& t = e.door_texts;
    const int32_t back = (s.condition() & 0xFF) ? kDoorBackText : kDoorBackTextOther;
    const int32_t rows[kDoorRows] = {kDoorStayText, back, kDoorFloorText, kDoorListText};
    InterlockedIncrement(&t.seq);
    t.id = id;
    copy_text(get(kDoorTextCategory, kDoorTitleText), t.title, kDoorTextBytes);
    for (int i = 0; i < kDoorRows; ++i) {
        copy_text(get(kDoorTextCategory, rows[i]), t.rows[i], kDoorTextBytes);
    }
    for (int i = 0; i < kDoorListMax; ++i) {
        t.areas[i][0] = '\0';
        if (i < count) {
            copy_text(get(kAreaNameCategory, zones[i]), t.areas[i], kDoorTextBytes);
        }
    }
    t.ready = 1;
    InterlockedIncrement(&t.seq);
}

// Why the question in Asked window a's record no longer stands, or NULL
// while it does.
inline const char* asked_record_gone(const Engine& e, int a, const AskedRecord& r, const AskedNow& now) {
    if (!r.active) {
        return "no question was kept from its window";
    }
    if (!now.ctl || now.ctl != r.ctl) {
        return "the window's controller is another";
    }
    switch (a) {
    case kAskedSortyn:
        if (rdptr(now.ctl, kSortynCallback) != r.callback || rdptr(now.ctl, kSortynContext) != r.context) {
            return "the window holds another question, or none";
        }
        break;
    case kAskedOk:
        if (rdptr(now.ctl, kOkCallback) != r.callback || rdptr(now.ctl, kOkContext) != r.context) {
            return "the window holds another question, or none";
        }
        if (rd32(now.ctl, kOkAnswered) == 1) {
            return "the notice was acknowledged";
        }
        break;
    case kAskedMaplist:
        if (rdptr(now.ctl, kMapRows) != r.context || !rdptr(now.ctl, kMapCallback)) {
            return "the list was built again, or closed";
        }
        break;
    case kAskedEvitem: {
        const void* cb;
        memcpy(&cb, &e.game.asked.evitem_callback, sizeof(cb));
        if (rdptr(now.ctl, kEvitemCallback) != cb) {
            return "the window holds another question, or none";
        }
        if (!now.list || static_cast<int32_t>(rd32(now.list, kEvitemRow)) != r.row) {
            return "the key item list moved on";
        }
        break;
    }
    case kAskedDoor:
        if (rdptr(now.ctl, 0) != e.game.asked.door.vtable) {
            return "the window's controller is another";
        }
        if (now.ctl[kDoorNation] != (r.row & 0xFF) || now.ctl[kDoorKept] != ((r.row >> 8) & 0xFF)) {
            return "the door was used again";
        }
        if (now.house != r.callback || now.door != r.context) {
            return "the player has left the Mog House";
        }
        if (now.exiting != 0) {
            return "the player is on the way out";
        }
        break;
    default:
        return "no question was kept from its window";
    }
    if (r.owner_n >= 0 && now.owner != r.owner) {
        return "the window that asked has closed";
    }
    return NULL;
}

// Whether Asked window a asks now, its window up or (*blocked) kept from
// opening; else why not. `r` is a copy of its record.
inline const char* asked_pending(const Engine& e, int a, const AskedRecord& r, const AskedNow& now, bool* blocked,
                                 char* buf, size_t size) {
    *blocked = false;
    if (asked_records(a) && !asked_record_gone(e, a, r, now)) {
        *blocked = true;
        return NULL;
    }
    const uint8_t* ctl = now.ctl;
    bool asks = false;
    switch (a) {
    case kAskedSortyn:
        asks = now.menu && rdptr(ctl, kSortynCallback);
        break;
    case kAskedEvitem: {
        const void* cb;
        memcpy(&cb, &e.game.asked.evitem_callback, sizeof(cb));
        asks = now.menu && rdptr(ctl, kEvitemCallback) == cb;
        break;
    }
    case kAskedConfyn:
        asks = now.menu != NULL;
        break;
    case kAskedBank:
        asks = ctl && rdptr(ctl, kBankCallback);
        *blocked = asks && !now.menu;
        break;
    case kAskedMaplist:
        asks = now.menu && rdptr(ctl, kMapCallback) && map_count(ctl) > 0;
        break;
    case kAskedGmtell:
        asks = now.menu && rd16(ctl, kQueryResult) == 0;
        break;
    case kAskedMpDead:
        asks = now.menu && rd32(ctl, kMpDeadChoice) == 0;
        break;
    case kAskedOk:
        asks = now.menu && rd32(ctl, kOkAnswered) != 1;
        break;
    case kAskedDoor:
        asks = now.menu && rdptr(ctl, 0) == e.game.asked.door.vtable && now.door && now.exiting == 0;
        break;
    default:
        break;
    }
    if (!asks || !ctl) {
        *blocked = false;
        snprintf(buf, size, "no question is pending in %s", kAskedNames[a]);
        return buf;
    }
    return NULL;
}

// Why Asked window a takes no cancel, or NULL for one that does.
inline const char* asked_cancel_refusal(int a) {
    switch (a) {
    case kAskedConfyn:
        return "confyn has no cancel; answer('confyn', false) says no";
    case kAskedMpDead:
        return "mp_dead has no cancel: the game's own Escape there only hands focus to the chat log";
    case kAskedOk:
        return "ok has no cancel; answer('ok') acknowledges it";
    default:
        return NULL;
    }
}

// Why a menu's input sink would hand its window no confirm now, or NULL.
inline const char* sink_refusal(const char* nm, const uint8_t* menu, char* buf, size_t size) {
    if (rd32(menu, kMenuState) == kStateDormant) {
        snprintf(buf, size, "%s is under another window", nm);
    } else if (rd16(menu, kMenuInputWait) > 0) {
        snprintf(buf, size, "%s takes no input yet", nm);
    } else if (menu[kMenuBusy]) {
        snprintf(buf, size, "%s is busy", nm);
    } else {
        return NULL;
    }
    return buf;
}

// Why a reply to Asked window a's pending question is refused, or NULL:
// `answer` with `value` (already of its window's kind), else its cancel;
// `r` and `blocked` as asked_pending() found them. The game's own confirm
// on each row, and its cancel, run with the checks the game's input routine
// makes on the way.
inline const char* asked_refusal(const Engine& e, int a, const AskedNow& now, const AskedRecord& r, bool blocked,
                                 bool answer, int32_t value, char* buf, size_t size) {
    if (!answer && asked_cancel_refusal(a)) {
        snprintf(buf, size, "%s", asked_cancel_refusal(a));
        return buf;
    }
    switch (a) {
    case kAskedDoor:
        if (now.menu && sink_refusal("mogdoor", now.menu, buf, size)) {
            return buf;
        }
        return answer ? door_offer_refusal(e, now, !blocked, r.row & 0xFF, value, buf, size) : NULL;
    case kAskedBank:
        if (answer && value != kBankSafe && value != kBankSafe2) {
            snprintf(buf, size, "bankmenu offers the containers %d and %d, not %d", static_cast<int>(kBankSafe),
                static_cast<int>(kBankSafe2), static_cast<int>(value));
            return buf;
        }
        if (answer && value == kBankSafe2 && now.safe2 != 1) {
            snprintf(buf, size, now.safe2 < 0 ? "the player's data is not loaded, so whether Mog Safe 2 is open"
                " is not known" : "Mog Safe 2 is not open to the player");
            return buf;
        }
        break;
    case kAskedMaplist: {
        if (answer) {
            const int count = map_count(now.ctl);
            const uint8_t* entry = value >= 1 && value <= count ? map_entry(now.ctl, value - 1) : NULL;
            if (!entry) {
                snprintf(buf, size, "maplist has no row %d (options('maplist') lists them)", static_cast<int>(value));
                return buf;
            }
            if (static_cast<int32_t>(rd32(entry, 0)) >= 0) {
                break;
            }
        } else if (map_top(now.ctl)) {
            break;
        }
        if (!map_data_loaded(e)) {
            snprintf(buf, size, "the player's data is not loaded, and maplist builds no other level's list without"
                " it");
            return buf;
        }
        break;
    }
    case kAskedMpDead:
        if (value != 1 && value != 2) {
            snprintf(buf, size, "mp_dead takes 1 or 2, not %d", static_cast<int>(value));
            return buf;
        }
        return sink_refusal("mp_dead", now.menu, buf, size);
    default:
        break;
    }
    return NULL;
}

// Bankmenu's Mog Safe 2 flag in the player's data: 1 or 0, -1 while the
// data is not there. Either thread.
inline int32_t bank_safe2(const Engine& e) {
    const AskedSites& s = e.game.asked;
    const uint8_t* data = s.bank_data && readable(s.bank_data, 4) ? *s.bank_data : NULL;
    if (!data || !readable(data + s.bank_safe2, 1)) {
        return -1;
    }
    return data[s.bank_safe2] & 1;
}

// The asking window the record takes: the active menu, open.
inline const uint8_t* asked_owner(Engine& e, int16_t* owner_n) {
    const uint8_t* active = e.game.mcb ? rdptr(e.game.mcb, kMcbActive) : NULL;
    const int n = active ? name_of_menu(e, active) : -1;
    *owner_n = static_cast<int16_t>(n);
    return n >= 0 && open_menu(e, n, NULL) == active ? active : NULL;
}

// Asked window a as the game thread finds it.
inline AskedNow asked_now(Engine& e, int a) {
    AskedNow now;
    memset(&now, 0, sizeof(now));
    const int n = e.inv.asked_window[a];
    now.ctl = controller_at(e, n);
    now.menu = now.ctl ? open_menu(e, n, NULL) : NULL;
    const AskedRecord& r = e.asked[a];
    if (r.active && r.owner_n >= 0) {
        now.owner = open_menu(e, r.owner_n, NULL);
    }
    if (a == kAskedEvitem) {
        now.list = controller_at(e, e.inv.find_exact("evitem"));
    }
    if (a == kAskedBank) {
        now.safe2 = bank_safe2(e);
    }
    if (a == kAskedDoor) {
        door_now(e, &now);
    }
    return now;
}

// Asked window a's record ended, and a pending event saying so for every
// handle but the one whose reply ended it (slot -1 for none).
inline void asked_end(Engine& e, int a, int32_t slot, uint32_t gen) {
    AskedRecord& r = e.asked[a];
    if (!r.active) {
        return;
    }
    InterlockedIncrement(&r.seq);
    r.active = 0;
    InterlockedIncrement(&r.seq);
    e.events.post(pack_pending(kPendingAsked + a, false, slot, gen));
}

// The texts of the question mogdoor asks now, its window up or (`blocked`)
// kept from opening; and while a block keeps it, the window's own tick run
// on its controller, which enables the floor and Mog Garden rows from the
// player's state each frame the window is up.
inline void watch_door(Engine& e) {
    HU_STEP("watch_door");
    const int n = e.inv.asked_window[kAskedDoor];
    const AskedNow now = asked_now(e, kAskedDoor);
    const AskedRecord& r = e.asked[kAskedDoor];
    bool blocked = false;
    char buf[kErrorReason];
    if (asked_pending(e, kAskedDoor, r, now, &blocked, buf, sizeof(buf))) {
        return;
    }
    if (blocked) {
        e.game.asked.door.tick(const_cast<uint8_t*>(now.ctl), NULL, 0);
    }
    uint16_t zones[kDoorListMax];
    const int count = door_areas(e, now.ctl, !blocked, r.row & 0xFF, zones);
    door_texts(e, blocked ? r.id : static_cast<uint32_t>(e.instance[n]), zones, count);
}

// A refused open of Asked window a: the question its setup has written, or
// for evitem01 the one it is about to, taken with the window that asked as
// it stands, and announced. sortyn's and maplist's need that window,
// evitem01's the key item list; ok's a notice not yet acknowledged;
// mogdoor's the nation its door's handler wrote and the Mog House's door.
inline void asked_refused(Engine& e, int a) {
    if (!asked_records(a)) {
        return;
    }
    const int n = e.inv.asked_window[a];
    const uint8_t* ctl = controller_at(e, n);
    if (!ctl) {
        return;
    }
    int16_t owner_n = -1;
    const uint8_t* owner = a == kAskedOk || a == kAskedDoor ? NULL : asked_owner(e, &owner_n);
    const void* callback = NULL;
    const void* context = NULL;
    int32_t row = 0;
    switch (a) {
    case kAskedSortyn:
        callback = rdptr(ctl, kSortynCallback);
        context = rdptr(ctl, kSortynContext);
        if (!callback || !owner) {
            return;
        }
        break;
    case kAskedEvitem: {
        const uint8_t* list = controller_at(e, e.inv.find_exact("evitem"));
        if (!owner || !list || owner_n != e.inv.find_exact("evitem")) {
            return;
        }
        row = static_cast<int32_t>(rd32(list, kEvitemRow));
        break;
    }
    case kAskedMaplist:
        context = rdptr(ctl, kMapRows);
        if (!context || !owner) {
            return;
        }
        break;
    case kAskedOk:
        callback = rdptr(ctl, kOkCallback);
        context = rdptr(ctl, kOkContext);
        if (rd32(ctl, kOkAnswered) == 1) {
            return;
        }
        break;
    case kAskedDoor: {
        AskedNow now;
        memset(&now, 0, sizeof(now));
        door_now(e, &now);
        const int32_t nation = ctl[kDoorNation];
        if (rdptr(ctl, 0) != e.game.asked.door.vtable || !now.door || now.exiting != 0 || nation < 1
            || nation > e.game.asked.door.nations) {
            return;
        }
        callback = now.house;
        context = now.door;
        row = nation | ctl[kDoorKept] << 8;
        break;
    }
    default:
        return;
    }
    AskedRecord& r = e.asked[a];
    InterlockedIncrement(&r.seq);
    r.owner_n = owner_n;
    r.id = static_cast<uint32_t>(e.instance[n]);
    r.ctl = ctl;
    r.callback = callback;
    r.context = context;
    r.owner = owner;
    r.row = row;
    r.active = 1;
    InterlockedIncrement(&r.seq);
    if (a == kAskedDoor) {
        watch_door(e);
    }
    e.events.post(pack_pending(kPendingAsked + a, true));
}

// bankmenu's question waiting with no window, as the drain sees it each
// frame: a pending event as it starts and as it ends; one a reply ended
// carries the replying handle (slot -1 for none).
inline void watch_bank(Engine& e, int32_t slot = -1, uint32_t gen = 0) {
    HU_STEP("watch_bank");
    const int n = e.inv.asked_window[kAskedBank];
    const uint8_t* ctl = controller_at(e, n);
    const bool waiting = ctl && rdptr(ctl, kBankCallback) && !open_menu(e, n, NULL);
    if (waiting == (e.bank_waiting != 0)) {
        return;
    }
    InterlockedExchange(&e.bank_waiting, waiting ? 1 : 0);
    e.events.post(pack_pending(kPendingAsked + kAskedBank, waiting, waiting ? -1 : slot, gen));
}

// maplist's level each frame its window is up: a step the player's keys
// take between levels is a new prompt, counted as a reply's step is
// (map_step()). Another open is one already.
inline void watch_map_level(Engine& e) {
    HU_STEP("watch_map_level");
    const int n = e.inv.asked_window[kAskedMaplist];
    uint8_t* ctl = NULL;
    const uint8_t* menu = open_menu(e, n, &ctl);
    const int32_t level = menu && ctl ? map_where(ctl) : 0;
    if (menu && menu == e.map_menu && e.instance[n] == e.map_prompts && level != e.map_level) {
        InterlockedIncrement(&e.instance[n]);
    }
    e.map_menu = menu;
    e.map_prompts = e.instance[n];
    e.map_level = level;
}

// Each Asked window's record that no longer stands ended, bankmenu's
// waiting and maplist's level watched.
inline void watch_asked(Engine& e) {
    HU_STEP("watch_asked");
    for (int a = 0; a < kAskedCount; ++a) {
        if (asked_records(a) && e.asked[a].active
            && asked_record_gone(e, a, e.asked[a], asked_now(e, a))) {
            asked_end(e, a, -1, 0);
        }
    }
    watch_bank(e);
    watch_map_level(e);
}

// mogdoor's confirm as far as its row: the gate it runs on every row, which
// shows the game's own message when it refuses; for the floor row, the exit
// the game's own read of the floor picks, which must be the one answered.
// False, with the error posted, when either refuses.
inline bool door_gate(Engine& e, int32_t value) {
    const DoorSites& s = e.game.asked.door;
    HU_HEAP_CHECK("before mogdoor gate");
    if (!(s.gate(s.gate_first, s.gate_second) & 0xFF)) {
        HU_HEAP_CHECK("after mogdoor gate");
        drain_error(e, "the game refuses an exit now; its own message says why");
        return false;
    }
    HU_HEAP_CHECK("after mogdoor gate");
    if (value == kDoorFloorUp || value == kDoorFloorUp + 1) {
        const int32_t floor = kDoorFloorUp + (s.floor() ? 1 : 0);
        HU_HEAP_CHECK("after mogdoor floor read");
        if (floor != value) {
            drain_error(e, "the player is on the other floor: mogdoor's floor row is %d", static_cast<int>(floor));
            return false;
        }
    }
    return true;
}

// The rest of mogdoor's confirm, the exit's row written, then the exit: the
// door actor given the routine that sends that exit once the door has
// opened, the door opened, and the three bytes the game's exit sets. The
// window stays up, as the game's confirm leaves it. Its cancel is the
// Escape of its top level: the window closed by instance, no gate and no
// exit written. The levels are the addon's own navigation, so the cancel
// closes the window whichever level it shows.
inline void door_exit(Engine& e, uint8_t* ctl, const uint8_t* menu, bool answer, int32_t value) {
    const DoorSites& s = e.game.asked.door;
    if (!answer) {
        if (menu) {
            e.game.close_instance(e.game.mcb, NULL, const_cast<uint8_t*>(menu), 1);
            HU_HEAP_CHECK("after mogdoor close");
        }
        return;
    }
    ctl[kDoorExit] = static_cast<uint8_t>(value);
    uint8_t* house = *s.house;
    uint8_t* door = house ? rdptr(house, kHouseDoor) : NULL;
    if (door) {
        memcpy(door + kDoorOpened, &s.opened, 4);
        HU_HEAP_CHECK("before mogdoor door open");
        s.open_door(door, NULL, 0);
        HU_HEAP_CHECK("after mogdoor door open");
    }
    for (int i = 0; i < 3; ++i) {
        *s.exiting[i] = 1;
    }
}

// maplist's own step between levels, as its key handler takes it, without
// its sound: with `entry`, the row whose list it opens, the list shown kept
// to go back to, -id and the entry's map written and the row under the
// cursor kept; without, back up a level, from a zone's maps to the list
// kept, from a region to the top. Then the rows built again and the cursor
// put on the first row going down, on the kept one going up. Each level is a
// prompt of its own: the window's count moves on, and a question its block
// keeps from the window, whose record the reply has ended, is recorded again
// with the new rows and announced.
inline void map_step(Engine& e, uint8_t* ctl, const uint8_t* entry, bool blocked) {
    const AskedSites& s = e.game.asked;
    if (entry) {
        wr16(ctl, kMapBack, rd16(ctl, kMapRegion));
        wr16(ctl, kMapRegion, -rd16(entry, 0));
        wr16(ctl, kMapZone, rd16(entry, 4));
        HU_HEAP_CHECK("before maplist cursor read");
        wr16(ctl, kMapSelection, static_cast<int32_t>(s.maplist_cursor(ctl, NULL)));
        HU_HEAP_CHECK("after maplist cursor read");
    } else if (rd16(ctl, kMapZone) >= 0) {
        const int16_t back = rd16(ctl, kMapBack);
        wr16(ctl, kMapZone, -1);
        wr16(ctl, kMapRegion, back);
    } else {
        wr16(ctl, kMapRegion, 0);
        wr16(ctl, kMapBack, 0);
    }
    HU_HEAP_CHECK("before maplist rebuild");
    s.maplist_rebuild(ctl, NULL);
    HU_HEAP_CHECK("after maplist rebuild");
    HU_HEAP_CHECK("before maplist select");
    s.maplist_select(ctl, NULL, entry ? 0 : rd16(ctl, kMapSelection), 0);
    HU_HEAP_CHECK("after maplist select");
    const int n = e.inv.asked_window[kAskedMaplist];
    InterlockedIncrement(&e.instance[n]);
    e.map_prompts = e.instance[n];
    e.map_level = map_where(ctl);
    if (!blocked || !rdptr(ctl, kMapRows)) {
        return;
    }
    AskedRecord& r = e.asked[kAskedMaplist];
    InterlockedIncrement(&r.seq);
    r.id = static_cast<uint32_t>(e.instance[n]);
    r.context = rdptr(ctl, kMapRows);
    r.active = 1;
    InterlockedIncrement(&r.seq);
    e.events.post(pack_pending(kPendingAsked + kAskedMaplist, true));
}

// What each window's own confirm on the row the answer names does, or its
// own cancel, with no input event: the callbacks the game's input routine
// calls, the fields it writes and the closes it makes, in its order; for a
// question kept from its window, the same with nothing to close but what
// the game itself would have closed had the window come up. A record ends
// first, its pending event skipped by the handle that replied.
inline void asked_reply(Engine& e, const Command& c) {
    const int a = e.inv.asked_of(c.target);
    if (a < 0) {
        return;
    }
    const bool answer = c.op == kOpAskedAnswer;
    const int32_t value = static_cast<int32_t>(c.value);
    const AskedNow now = asked_now(e, a);
    AskedRecord r = e.asked[a];
    bool blocked = false;
    char buf[kErrorReason];
    const char* why = asked_pending(e, a, r, now, &blocked, buf, sizeof(buf));
    if (!why) {
        why = asked_refusal(e, a, now, r, blocked, answer, value, buf, sizeof(buf));
    }
    if (why) {
        drain_error(e, "%s", why);
        return;
    }
    if (a == kAskedDoor && answer && !door_gate(e, value)) {
        return;
    }
    if (blocked && asked_records(a)) {
        asked_end(e, a, c.slot, c.gen);
    }
    uint8_t* ctl = const_cast<uint8_t*>(now.ctl);
    const int yes = answer && value ? 1 : 0;
    switch (a) {
    case kAskedSortyn: {
        SortynCallbackFn cb;
        const void* p = rdptr(ctl, kSortynCallback);
        memcpy(&cb, &p, sizeof(cb));
        HU_HEAP_CHECK("before sortyn callback");
        cb(rdptr(ctl, kSortynContext), yes);
        HU_HEAP_CHECK("after sortyn callback");
        wr32(ctl, kSortynCallback, 0);
        close_named(e, "sortyn");
        HU_HEAP_CHECK("after sortyn close");
        break;
    }
    case kAskedEvitem:
        HU_HEAP_CHECK("before evitem01 callback");
        e.game.asked.evitem_callback(yes);
        HU_HEAP_CHECK("after evitem01 callback");
        if (uint8_t* menu = rdptr(ctl, kCtlMenu)) {
            e.game.close_instance(e.game.mcb, NULL, menu, ctl[kEvitemCloseFlag]);
            HU_HEAP_CHECK("after evitem01 close");
        }
        break;
    case kAskedConfyn: {
        const int32_t mode = static_cast<int32_t>(rd32(ctl, kConfynMode));
        if (yes && (mode == 0 || mode == 1)) {
            HU_HEAP_CHECK("before confyn restore");
            e.game.asked.confyn_restore(mode + 1);
            HU_HEAP_CHECK("after confyn restore");
        }
        HU_HEAP_CHECK("before confyn close");
        e.game.close_instance(e.game.mcb, NULL, rdptr(ctl, kCtlMenu), 1);
        HU_HEAP_CHECK("after confyn close");
        break;
    }
    case kAskedBank: {
        const uint8_t* menu = live_named(e, "bankmenu", NULL);
        const int32_t container = answer ? value : menu && rd16(menu, kMenuCursor) != 1 ? kBankSafe2 : kBankSafe;
        BankCallbackFn cb;
        const void* p = rdptr(ctl, kBankCallback);
        memcpy(&cb, &p, sizeof(cb));
        HU_HEAP_CHECK("before bankmenu callback");
        cb(container, answer ? 0 : 1);
        HU_HEAP_CHECK("after bankmenu callback");
        if (menu) {
            close_named(e, "bankmenu");
            HU_HEAP_CHECK("after bankmenu close");
        } else {
            e.game.asked.bank_clear(ctl, NULL);
            HU_HEAP_CHECK("after bankmenu clear");
        }
        if (blocked) {
            watch_bank(e, c.slot, c.gen);
        }
        break;
    }
    case kAskedMaplist: {
        const uint8_t* entry = answer ? map_entry(ctl, value - 1) : NULL;
        const int32_t id = entry ? static_cast<int32_t>(rd32(entry, 0)) : kMapCloseRow;
        if (answer ? id < 0 : !map_top(ctl)) {
            map_step(e, ctl, entry, blocked);
            break;
        }
        if (id != kMapCloseRow) {
            MaplistCallbackFn cb;
            const void* p = rdptr(ctl, kMapCallback);
            memcpy(&cb, &p, sizeof(cb));
            HU_HEAP_CHECK("before map0 callback");
            cb(entry);
            HU_HEAP_CHECK("after map0 callback");
        }
        HU_HEAP_CHECK("before maplist_close");
        e.game.asked.maplist_close(ctl, NULL);
        HU_HEAP_CHECK("after maplist_close");
        break;
    }
    case kAskedGmtell:
        if (answer) {
            query_answer(e, c);
        } else {
            wr16(ctl, kQueryResult, kQueryCanceled);
        }
        break;
    case kAskedMpDead:
        wr32(ctl, kMpDeadChoice, static_cast<uint32_t>(value));
        break;
    case kAskedOk: {
        if (uint8_t* menu = rdptr(ctl, kCtlMenu)) {
            menu[kMenuBusy] = 1;
        }
        wr32(ctl, kOkAnswered, 1);
        if (const void* p = rdptr(ctl, kOkCallback)) {
            OkCallbackFn cb;
            memcpy(&cb, &p, sizeof(cb));
            HU_HEAP_CHECK("before ok callback");
            cb(rdptr(ctl, kOkContext));
            HU_HEAP_CHECK("after ok callback");
            e.game.close_instance(e.game.mcb, NULL, rdptr(ctl, kCtlMenu), 1);
            HU_HEAP_CHECK("after ok close");
        }
        if (ctl[kOkCloses]) {
            e.game.close_instance(e.game.mcb, NULL, rdptr(ctl, kCtlMenu), 1);
            HU_HEAP_CHECK("after ok close (closes)");
        }
        break;
    }
    case kAskedDoor:
        door_exit(e, ctl, now.menu, answer, value);
        break;
    default:
        break;
    }
}

// ---------------------------------------------------------------------------
// The NPC dialogue boxes (core.h's kDialogueNames). Each shows the pages of
// the messages its own text object holds, the same in both: a reply to
// either reaches both, an answer advancing a page as the player's key does,
// a cancel each box's own cancel.

// The message a dialogue box shows, NULL for none: `text` its text object
// with kTextBytes readable, NULL for none. A box asks while it shows one.
inline const uint8_t* dialogue_message(const uint8_t* text) {
    return text ? rdptr(text, kTextCurrent) : NULL;
}

// Why dialogue box d takes no reply now, or NULL: `text` its text object
// with kTextBytes readable, NULL for none; `log` the chat log its cancel
// hands the box's lines back to, NULL for none; `cancel_allowed` the event's
// byte that lets the player cancel at 1.
inline const char* dialogue_refusal(int d, const uint8_t* text, const uint8_t* log, uint8_t cancel_allowed,
                                    bool answer, char* buf, size_t size) {
    const char* nm = kDialogueNames[d];
    if (!dialogue_message(text)) {
        snprintf(buf, size, "no dialogue is shown in %s", nm);
    } else if (answer && !(rd32(text, kTextFlags) & kPageKey)) {
        snprintf(buf, size, "the page %s shows does not wait for a key: it goes on by itself", nm);
    } else if (!answer && cancel_allowed != 1) {
        snprintf(buf, size, "%s cannot be canceled now: the event allows no cancel", nm);
    } else if (!answer && !log) {
        snprintf(buf, size, "%s has no chat log to hand its lines back to", nm);
    } else {
        return NULL;
    }
    return buf;
}

// dialogue_refusal() for a reply to box d that reaches its twin too, whose
// `twin_text` and `twin_log` are as `text` and `log`: a cancel makes the
// twin's own while the twin shows a message, and is refused whole when the
// twin could not take it.
inline const char* dialogue_pair_refusal(int d, const uint8_t* text, const uint8_t* log, const uint8_t* twin_text,
                                         const uint8_t* twin_log, uint8_t cancel_allowed, bool answer, char* buf,
                                         size_t size) {
    const char* why = dialogue_refusal(d, text, log, cancel_allowed, answer, buf, size);
    if (!why && !answer && dialogue_message(twin_text)) {
        why = dialogue_refusal(kDialogueCount - 1 - d, twin_text, twin_log, cancel_allowed, false, buf, size);
    }
    return why;
}

// The chat log dialogue box `ctl` hands its lines back to, NULL for none.
inline uint8_t* dialogue_log(const uint8_t* ctl) {
    const uint8_t* log_ctl = ctl ? rdptr(ctl, kBoxLog) : NULL;
    return log_ctl ? rdptr(log_ctl, kLogObject) : NULL;
}

// Whether a page follows the one `message` shows, kMessageBytes of it
// readable: as the game's own test of the last page, which finds none
// where the next page would start at nothing; and none where all that
// would start there is the escapes that end a page, which show no page of
// their own.
inline bool more_pages(const uint8_t* message) {
    const uint8_t* at = rdptr(message, kMessageNext);
    const uint8_t* end = message + kMessageNext;
    if (!at || at < message + kMessageText || at >= end) {
        return false;
    }
    while (at + 1 < end && at[0] == 0x7F && at[1] >= 0x31 && at[1] <= 0x38) {
        at += text_escape_bytes(at[1]);
    }
    return at < end && at[0] != 0;
}

// A dialogue box's own cancel, as the game's handler makes it on the box
// the player's key reaches: its lines handed back to its chat log, its text
// object cleared, which closes it and zeroes the event's wait, the wait
// then told it was canceled, and the box closed.
inline void dialogue_cancel(Engine& e, uint8_t* ctl) {
    const DialogueSites& s = e.game.dialogue;
    s.release(dialogue_log(ctl), NULL, rd32(ctl, kBoxLogLine));
    s.clear(rdptr(ctl, kBoxText), NULL);
    *s.wait = kWaitAborted;
    e.game.close_instance(e.game.mcb, NULL, rdptr(ctl, kCtlMenu), 1);
}

// One page on, as the player sees it: the text dispatch once with the
// advance code, which moves the box's text object on a page that waits for
// a key, then its twin's, whichever box the reply names. The player's Enter
// runs it twice, which shows a second page whenever the first it shows
// waits for a key too. The game's cancel is the box's own and leaves the
// twin its message, so a cancel makes the named box's, then the twin's
// while that shows a message.
inline void dialogue_reply(Engine& e, const Command& c) {
    const int d = e.inv.dialogue_of(c.target);
    if (d < 0) {
        return;
    }
    const bool answer = c.op == kOpDialogueAnswer;
    uint8_t* ctl = const_cast<uint8_t*>(controller_at(e, c.target));
    uint8_t* text = ctl ? rdptr(ctl, kBoxText) : NULL;
    uint8_t* twin = const_cast<uint8_t*>(controller_at(e, e.inv.dialogue_twin(c.target)));
    uint8_t* twin_text = twin ? rdptr(twin, kBoxText) : NULL;
    char buf[kErrorReason];
    const char* why = dialogue_pair_refusal(d, text, dialogue_log(ctl), twin_text, dialogue_log(twin),
        *e.game.query_cancel_allowed, answer, buf, sizeof(buf));
    if (why) {
        drain_error(e, "%s", why);
        return;
    }
    if (answer) {
        e.game.dialogue.dispatch(text, NULL, kDialogueAdvance);
        return;
    }
    const bool both = dialogue_message(twin_text) != NULL;
    dialogue_cancel(e, ctl);
    if (both) {
        dialogue_cancel(e, twin);
    }
}

// A page event for each dialogue box that shows a new page: while its text
// object holds a message, the message shown, where its next page starts,
// the messages held, the box's lines and the box's opens as the drain last
// saw them, any of them changed. A message alike to the one before can
// take its place, address and all, but only through a close and an open of
// the box, or with the count of messages held changed.
inline void watch_pages(Engine& e) {
    HU_STEP("watch_pages");
    for (int d = 0; d < kDialogueCount; ++d) {
        const int n = e.inv.dialogue_window[d];
        const uint8_t* ctl = controller_at(e, n);
        const uint8_t* text = ctl ? rdptr(ctl, kBoxText) : NULL;
        const uint8_t* message = text ? rdptr(text, kTextCurrent) : NULL;
        PageSignal& last = e.pages[d];
        if (!message) {
            last.seen = 0;
            continue;
        }
        PageSignal now;
        now.message = message;
        now.next = rdptr(message, kMessageNext);
        now.queued = static_cast<int32_t>(rd32(text, kTextQueued));
        now.lines = static_cast<int32_t>(rd32(ctl, kBoxLineCount));
        now.opens = e.instance[n];
        now.seen = 1;
        if (last.seen && last.message == now.message && last.next == now.next && last.queued == now.queued
            && last.lines == now.lines && last.opens == now.opens) {
            continue;
        }
        last = now;
        e.events.post(pack_event(kEvPage, n));
    }
}

// ---------------------------------------------------------------------------
// The text-entry windows (core.h's kTextNames). Each asks while its window
// is up and its owner asks through it: stringdl while the outgoing box waits
// on the name or on a yes or no about a slot, comment while it is up (on
// another player's message it takes no answer), msgline while it writes a
// message, or with its open refused while the message its opener set up
// stands, scname while the player search is set up and link11 while link5's
// controller is there. A reply does what the window's own submit or cancel
// does, with no input event: its field ended first, then the fields the
// commit writes, the routines it calls and the closes and opens it makes,
// in its order.

// A text window as a reply or a read finds it, every pointer checked by
// whoever filled it in: its controller, its window open, its owner's
// controller and window (stringdl: delivery's, link11: link5's; scname: the
// player search), link5's selected row's record, msgline's busy byte,
// whether the player's data and the POL interface are there (1, 0, or -1
// unknown), and whether msgline's message is one its block kept from
// opening.
struct TextNow {
    const uint8_t* ctl;
    const uint8_t* menu;
    const uint8_t* owner;
    const uint8_t* owner_menu;
    const uint8_t* selected;
    int32_t busy;
    int32_t player;
    int32_t pol;
    bool blocked;
};

inline size_t text_size(int t) {
    static const size_t sizes[kTextCount] = {kStringdlBytes, kCommentBytes, kMsglineBytes, kScnameBytes,
                                             kLink11Bytes};
    return t >= 0 && t < kTextCount ? sizes[t] : 0;
}

// link5's selected row's record as its own selection routine finds it:
// the row under its window's cursor, scrolled, among the rows it holds;
// NULL for none, or one either thread cannot read.
inline const uint8_t* link5_selected(const uint8_t* link5) {
    const uint8_t* menu = link5 && readable(link5, kLink5Rows + 4) ? rdptr(link5, kCtlMenu) : NULL;
    if (!menu || !readable(menu, kMenuBytes)) {
        return NULL;
    }
    const int i = rd16(menu, kMenuCursor) + rd16(link5, kListScroll) - 1;
    const int count = static_cast<int32_t>(rd32(link5, kLink5Count));
    const uint8_t* rows = rdptr(link5, kLink5Rows);
    if (i < 0 || i >= count || !rows || !readable(rows + i * kLink5RowStride, kLink5RowStride)) {
        return NULL;
    }
    const uint8_t* record = rdptr(rows + i * kLink5RowStride, kLink5RowRecord);
    return record && readable(record, 2) ? record : NULL;
}

// The longest text msgline's mode takes: its field's size, as the field's
// start gives it; the long field's commit zeroes its last byte.
inline int msgline_max(int mode) {
    return mode == 2 || mode == 7 ? kMsglineShort : kMsglineKept;
}

// The modes whose message msgline's window writes and sends; the others
// send a text of the game's own with no window.
inline bool msgline_writes(int mode) {
    return mode == 0 || mode == 1 || mode == 2 || mode == 5 || mode == 7;
}

// Why msgline's record no longer stands, or NULL while it does.
inline const char* msgline_record_gone(const TextRecord& r, const uint8_t* ctl) {
    if (!r.active) {
        return "no message was kept from its window";
    }
    if (!ctl || ctl != r.ctl) {
        return "the window's controller is another";
    }
    if (rd16(ctl, kMsglineMode) != r.mode) {
        return "another message was set up since";
    }
    return NULL;
}

// Whether text window t asks now, its window up or (now.blocked) kept from
// opening; else why not, and `none`, when given, whether that is because
// nothing asks through it now rather than a state of the window a reply
// waits out or that this engine cannot read. `r` is a copy of msgline's
// record.
inline const char* text_asks(int t, const TextNow& now, const TextRecord& r, bool answer, char* buf, size_t size,
                             bool* none = NULL) {
    const char* nm = kTextNames[t];
    bool unused;
    none = none ? none : &unused;
    *none = false;
    if (t == kTextMsgline && !msgline_record_gone(r, now.ctl)) {
        return NULL;
    }
    if (!now.ctl || !now.menu) {
        if (t == kTextMsgline) {
            snprintf(buf, size, "no message is being written in msgline");
        } else {
            snprintf(buf, size, "%s is not open, and nothing asks through it until the game opens it", nm);
        }
        *none = true;
        return buf;
    }
    switch (t) {
    case kTextStringdl: {
        const int state = now.owner ? static_cast<uint16_t>(rd16(now.owner, kDeliveryState)) : 0;
        const int mode = static_cast<int8_t>(now.ctl[kStringdlMode]);
        const bool asks = (mode == kStringdlAsksName && state == kDeliveryAsksName)
            || (mode == kStringdlAsksSlot && state == kDeliveryAsksSlot);
        if (!asks) {
            snprintf(buf, size, "stringdl is not asking anything now");
            *none = true;
            return buf;
        }
        if (rd16(now.ctl, kStringdlReply) != 0) {
            snprintf(buf, size, "the outgoing box has not read stringdl's last reply yet");
            return buf;
        }
        break;
    }
    case kTextComment: {
        const int mode = static_cast<int8_t>(now.ctl[kCommentMode]);
        if (mode < 0 || mode > 3) {
            snprintf(buf, size, "comment holds no message of a kind it knows");
            return buf;
        }
        if (answer && mode == kCommentView) {
            snprintf(buf, size, "comment shows another player's message, which takes no answer");
            return buf;
        }
        break;
    }
    case kTextScname:
        if (!now.owner) {
            snprintf(buf, size, "the player search is not set up, so scname takes no input");
            return buf;
        }
        break;
    case kTextLink11:
        if (!now.owner) {
            snprintf(buf, size, "link5's controller is not there");
            return buf;
        }
        break;
    default:
        break;
    }
    return NULL;
}

// Why a reply to text window t is refused now, or NULL: `answer` with `a`,
// else its cancel; the gates the player's keys pass on the way, then what
// the game's commit would refuse or could not do.
inline const char* text_refusal(int t, const TextNow& now, bool answer, const TextAnswer& a, char* buf,
                                size_t size) {
    if (now.menu && sink_refusal(kTextNames[t], now.menu, buf, size)) {
        return buf;
    }
    switch (t) {
    case kTextStringdl: {
        const bool name = now.ctl[kStringdlMode] == kStringdlAsksName;
        if (answer && name && a.yes >= 0) {
            snprintf(buf, size, "stringdl asks for the recipient's name now: answer('stringdl', name)");
            return buf;
        }
        if (answer && !name && a.yes < 0) {
            snprintf(buf, size, "stringdl asks yes or no about a delivery slot now: answer('stringdl', true or"
                " false)");
            return buf;
        }
        break;
    }
    case kTextMsgline: {
        if (!answer) {
            break;
        }
        const int mode = rd16(now.ctl, kMsglineMode);
        if (!msgline_writes(mode)) {
            snprintf(buf, size, "msgline's mode %d sends a text of the game's own; it takes no answer", mode);
            return buf;
        }
        if (now.busy) {
            snprintf(buf, size, "a message is still on its way; msgline sends the next once it has gone");
            return buf;
        }
        if (now.player != 1) {
            snprintf(buf, size, "the player's data is not loaded, so msgline sends nothing");
            return buf;
        }
        if (now.pol != 1) {
            snprintf(buf, size, "the POL interface is not there, so msgline sends nothing");
            return buf;
        }
        const int length = static_cast<int>(strlen(a.text));
        if (length > msgline_max(mode)) {
            snprintf(buf, size, "%d bytes; msgline's mode %d takes at most %d", length, mode, msgline_max(mode));
            return buf;
        }
        break;
    }
    case kTextLink11:
        if (answer && !now.selected) {
            snprintf(buf, size, "link5 has no linkshell selected for the new one");
            return buf;
        }
        break;
    default:
        break;
    }
    return NULL;
}

// Text window t as the game thread finds it.
inline TextNow text_now(Engine& e, int t) {
    TextNow now;
    memset(&now, 0, sizeof(now));
    const int n = e.inv.text_window[t];
    now.ctl = controller_at(e, n);
    now.menu = now.ctl ? open_menu(e, n, NULL) : NULL;
    const TextSites& s = e.game.text;
    switch (t) {
    case kTextStringdl:
        now.owner = controller_global(e, "delivery");
        break;
    case kTextScname:
        now.owner = s.search ? *s.search : NULL;
        break;
    case kTextLink11:
        now.owner = controller_global(e, "link5");
        now.selected = link5_selected(now.owner);
        break;
    case kTextMsgline:
        now.busy = s.busy ? *s.busy : 0;
        now.player = s.player && *s.player ? 1 : 0;
        now.pol = s.pol && *s.pol ? 1 : 0;
        now.blocked = !msgline_record_gone(e.msgline, now.ctl);
        break;
    default:
        break;
    }
    now.owner_menu = now.owner && t != kTextScname ? rdptr(now.owner, kCtlMenu) : NULL;
    return now;
}

// msgline's record ended, and a pending event saying so for every handle
// but the one whose reply ended it (slot -1 for none).
inline void msgline_end(Engine& e, int32_t slot, uint32_t gen) {
    TextRecord& r = e.msgline;
    if (!r.active) {
        return;
    }
    InterlockedIncrement(&r.seq);
    r.active = 0;
    InterlockedIncrement(&r.seq);
    e.events.post(pack_pending(kPendingMsgline, false, slot, gen));
}

// A refused open of msgline: the message its opener set up, taken with the
// mode it set, and announced; none for a mode that writes no message.
inline void msgline_refused(Engine& e) {
    const int n = e.inv.text_window[kTextMsgline];
    const uint8_t* ctl = controller_at(e, n);
    if (!ctl || !msgline_writes(rd16(ctl, kMsglineMode))) {
        return;
    }
    TextRecord& r = e.msgline;
    InterlockedIncrement(&r.seq);
    r.id = static_cast<uint32_t>(e.instance[n]);
    r.ctl = ctl;
    r.mode = rd16(ctl, kMsglineMode);
    r.active = 1;
    InterlockedIncrement(&r.seq);
    e.events.post(pack_pending(kPendingMsgline, true));
}

inline void watch_msgline(Engine& e) {
    HU_STEP("watch_msgline");
    if (e.msgline.active && msgline_record_gone(e.msgline, controller_at(e, e.inv.text_window[kTextMsgline]))) {
        msgline_end(e, -1, 0);
    }
}

// msgline's send, as its commit makes it in each mode that writes one,
// with the text in place: the result the sender returns, 0 when queued.
inline int32_t msgline_send(Engine& e, uint8_t* ctl, int mode) {
    const TextSites& s = e.game.text;
    uint8_t* pol = *s.pol;
    const MessageFn message = e.game.dead.message;
    const uintptr_t text = reinterpret_cast<uintptr_t>(ctl + kMsglineText);
    const uintptr_t name = reinterpret_cast<uintptr_t>(ctl + kMsglineName);
    const uintptr_t sent0 = reinterpret_cast<uintptr_t>(s.sent[0]);
    const uintptr_t sent1 = reinterpret_cast<uintptr_t>(s.sent[1]);
    const uint32_t word2 = rd32(ctl, kMsglineWord2);
    const uintptr_t word1 = rd32(ctl, kMsglineWord1);
    auto packed = [&]() -> uintptr_t {
        const uint32_t number = s.pol_number(pol, NULL, rd32(pol, kPolWord)) & 0xFF;
        return (((number << 8) | (word2 & 0xF0000)) << 8) | (word2 & 0xFFFF);
    };
    switch (mode) {
    case 0: {
        const uintptr_t said = reinterpret_cast<uintptr_t>(message(9, 0x1C));
        return s.send_tell(word1, packed(), name, said, text, sent0, 0x11);
    }
    case 1: {
        const uintptr_t said = reinterpret_cast<uintptr_t>(message(9, 0x1C));
        return s.send_reply(rd32(ctl, kMsglineIds), rd32(ctl, kMsglineIds + 4), rd32(ctl, kMsglineIds + 8), said,
            text, sent0, 0x11);
    }
    case 2: {
        for (uint8_t* c = ctl + kMsglineText; *c; ++c) {
            if (*c == 7) {
                *c = 0;
            }
        }
        char clock[0x80];
        memset(clock, 0, sizeof(clock));
        s.clock_text(1, clock, 0x7F);
        return s.send_short(word1, packed(), text, reinterpret_cast<uintptr_t>(clock), name, sent0, 0x14);
    }
    case 5: {
        const uintptr_t said = reinterpret_cast<uintptr_t>(message(9, 0x1F));
        return s.send_answer(rd32(ctl, kMsglineIds2), rd32(ctl, kMsglineIds2 + 4), rd32(ctl, kMsglineIds),
            rd32(ctl, kMsglineIds + 4), said, text, sent1, 0x12);
    }
    case 7: {
        char date[0x80];
        char clock[0x80];
        memset(date, 0, sizeof(date));
        memset(clock, 0, sizeof(clock));
        s.clock_text(1, clock, 0x7F);
        s.clock_text(9, date, 0x7F);
        return s.send_dated(rd32(ctl, kMsglineIds2), rd32(ctl, kMsglineIds2 + 4), rd32(ctl, kMsglineIds),
            rd32(ctl, kMsglineIds + 4), text, reinterpret_cast<uintptr_t>(date), reinterpret_cast<uintptr_t>(clock),
            sent1, 0x15);
    }
    default:
        return -1;
    }
}

// strncpy's copy: at most `size` bytes of `from`, the rest of `size`
// zeroed.
inline void copy_padded(uint8_t* to, const char* from, size_t size) {
    size_t n = 0;
    while (n < size && from[n]) {
        ++n;
    }
    memcpy(to, from, n);
    memset(to + n, 0, size - n);
}

// What each window's own submit, on the row that commits, or its own
// cancel does, as text_reply() runs it.
inline void stringdl_reply(Engine& e, uint8_t* ctl, const TextNow& now, bool answer, const TextAnswer& a) {
    const bool name = ctl[kStringdlMode] == kStringdlAsksName;
    int reply = kStringdlNo;
    if (name) {
        if (ctl[kStringdlEditing]) {
            e.game.text.stringdl_stop(ctl, NULL);
        }
        if (answer) {
            copy_padded(ctl + kStringdlName, a.text, kStringdlNameBytes);
        }
        reply = answer ? kStringdlNamed : kStringdlCanceled;
    } else if (answer && a.yes) {
        reply = kStringdlYes;
    }
    wr16(ctl, kStringdlReply, reply);
    // The name's handler raises the outgoing box, and the input routine
    // around it does so again.
    for (int i = 0; i < (name ? 2 : 1) && now.owner_menu; ++i) {
        e.game.text.focus(e.game.mcb, NULL, const_cast<uint8_t*>(now.owner_menu), 0);
    }
}

inline void comment_reply(Engine& e, uint8_t* ctl, const TextNow& now, const TextAnswer& a) {
    const TextSites& s = e.game.text;
    const int mode = static_cast<int8_t>(ctl[kCommentMode]);
    if (rdptr(ctl, kCommentField)) {
        s.comment_stop(ctl, NULL, 0);
    }
    for (int l = 0; l < kCommentLines; ++l) {
        uint8_t* bytes = ctl + kCommentSlots + (mode * 3 + l) * kCommentLineBytes;
        char* line = reinterpret_cast<char*>(bytes);
        copy_padded(bytes, a.lines[l], kCommentLine);
        line[kCommentLine] = '\0';
        s.sanitize(line, line);
        s.trim(line);
    }
    e.game.close_instance(e.game.mcb, NULL, const_cast<uint8_t*>(now.menu), 1);
}

inline void msgline_reply(Engine& e, const Command& c, uint8_t* ctl, const TextNow& now, bool answer,
                          const TextAnswer& a) {
    const TextSites& s = e.game.text;
    if (now.blocked) {
        msgline_end(e, c.slot, c.gen);
    }
    if (now.menu && rdptr(ctl, kMsglineField)) {
        s.msgline_stop(ctl, NULL, 0);
    }
    if (answer) {
        copy_padded(ctl + kMsglineText, a.text, kMsglineLong);
        ctl[kMsglineTextEnd] = 0;
        if (msgline_send(e, ctl, rd16(ctl, kMsglineMode)) == 0) {
            if (uint8_t* owner = *s.sending) {
                s.mark(owner + kSendingMark, NULL, 0x10);
            }
            *s.busy = 1;
        }
    }
    if (now.menu) {
        close_named(e, "msgline");
    }
}

inline void scname_reply(Engine& e, uint8_t* ctl, const TextNow& now, bool answer, const TextAnswer& a) {
    if (answer) {
        const TextSites& s = e.game.text;
        if (rdptr(ctl, kScnameField)) {
            s.scname_stop(ctl, NULL, 0);
        }
        memset(ctl + kScnameName, 0, kScnameBytes - kScnameName);
        memcpy(ctl + kScnameName, a.text, strlen(a.text));
        uint8_t* search = const_cast<uint8_t*>(now.owner);
        uint8_t* criteria = search + kSearchCriteria;
        s.criteria_clear(criteria);
        s.criteria_name(criteria, reinterpret_cast<const char*>(ctl + kScnameName));
        s.search_start(search, NULL, 1, criteria, 1, 0, 0, 0);
    }
    close_named(e, "scname");
    if (answer) {
        close_named(e, "scsibori");
    }
}

inline void link11_reply(Engine& e, uint8_t* ctl, const TextNow& now, bool answer, const TextAnswer& a) {
    const TextSites& s = e.game.text;
    uint8_t* link5 = const_cast<uint8_t*>(now.owner);
    const uint8_t* item = answer ? s.selected(link5, NULL) : NULL;
    if (answer && !item) {
        drain_error(e, "link5 has no linkshell selected for the new one");
        return;
    }
    if (rd32(ctl, kLink11Editing)) {
        s.link11_stop(ctl, NULL, answer ? 1 : 0);
    }
    if (answer) {
        memset(ctl + kLink11Name, 0, kLink11Bytes - kLink11Name);
        memcpy(ctl + kLink11Name, a.text, strlen(a.text));
        if (a.colored) {
            memcpy(ctl + kLink11Colors, a.color, 3);
        }
        const uint32_t slot = (s.slot_taken(0, 0, 0, 1) & 0xFF) ? 2 : 1;
        const char* encoded = s.encode(reinterpret_cast<const char*>(ctl + kLink11Name));
        const uint8_t* c = ctl + kLink11Colors;
        if (s.create(slot, item[1], item[0], 1, encoded, c[0], c[1], c[2]) & 0xFF) {
            if (const uint32_t* made = s.item(item[1], item[0])) {
                *s.made[0] = static_cast<uint8_t>(slot);
                memcpy(s.made[1], made, 4);
                *s.made[2] = 1;
                *s.made[3] = 0x3C;
            }
            link5[kLink5Made] |= 1;
        } else {
            drain_error(e, "the game could not queue the new linkshell's request");
        }
    }
    close_named(e, "link11");
    char key[kKeyLen + 1];
    make_key(e, e.inv.find_exact("link6"), key);
    e.game.open(e.game.mcb, NULL, key, 1, 0);
    s.focus(e.game.mcb, NULL, rdptr(link5, kCtlMenu), 0);
}

// A reply to a text window: checked again against the window as it stands,
// then the window's own commit or cancel; msgline's record, when it was the
// question, ends first, its pending event skipped by the handle that
// replied.
inline void text_reply(Engine& e, const Command& c) {
    const int t = e.inv.text_of(c.target);
    if (t < 0) {
        return;
    }
    const bool answer = c.op == kOpTextAnswer;
    const TextAnswer& a = e.text_answers[t];
    const TextNow now = text_now(e, t);
    char buf[kErrorReason];
    const char* why = text_asks(t, now, e.msgline, answer, buf, sizeof(buf));
    if (!why) {
        why = text_refusal(t, now, answer, a, buf, sizeof(buf));
    }
    if (why) {
        drain_error(e, "%s", why);
        return;
    }
    uint8_t* ctl = const_cast<uint8_t*>(now.ctl);
    switch (t) {
    case kTextStringdl:
        stringdl_reply(e, ctl, now, answer, a);
        break;
    case kTextComment:
        comment_reply(e, ctl, now, a);
        break;
    case kTextMsgline:
        msgline_reply(e, c, ctl, now, answer, a);
        break;
    case kTextScname:
        scname_reply(e, ctl, now, answer, a);
        break;
    case kTextLink11:
        link11_reply(e, ctl, now, answer, a);
        break;
    default:
        break;
    }
}

// An event on window n; on ability, with the list its controller holds at
// kAbilityCategory: the one the window shows, or showed last.
inline uint32_t window_event(const Engine& e, int type, int n) {
    const uint8_t* ctl = n >= 0 && n == e.inv.ability ? controller_at(e, n) : NULL;
    return ctl ? pack_event_category(type, n, static_cast<int32_t>(rd32(ctl, kAbilityCategory)))
               : pack_event(type, n);
}

// The opened and closed events, each posted only where it changes what the
// events said last: the game re-opens some windows every frame (cnqframe's
// points windows, for one), and an open of a window already open is no
// news, unless it is another instance, or ability showing another list.
inline void post_opened(Engine& e, int n, const uint8_t* menu, uint32_t event, int32_t category) {
    const bool ability = n == e.inv.ability;
    if (e.reported[n] == menu && (!ability || e.reported_category == category)) {
        return;
    }
    e.reported[n] = menu;
    if (ability) {
        e.reported_category = category;
    }
    e.events.post(event);
}

inline void post_closed(Engine& e, int n) {
    if (!e.reported[n]) {
        return;
    }
    e.reported[n] = NULL;
    e.events.post(window_event(e, kEvClosed, n));
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
    HU_STEP("watch_pending");
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
    HU_STEP("watch_cursors");
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

// The cursor row of each open of a boolean prompt, recorded once, the first
// frame the drain sees that open: the ask puts the cursor on its default
// right after the open and keeps it nowhere else.
inline void watch_yesno(Engine& e) {
    HU_STEP("watch_yesno");
    for (int n = 0; n < e.inv.count; ++n) {
        if (!e.inv.first_row[n]) {
            continue;
        }
        const uint8_t* menu = open_menu(e, n, NULL);
        const LONG instance = e.instance[n];
        if (menu && !first_row_is(e.yesno_first[n], instance)) {
            InterlockedExchange(&e.yesno_first[n], pack_first_row(instance, rd16(menu, kMenuCursor) == kRowYes));
        }
    }
}

// The compass has no open or close of its own: opened and closed are its
// state byte leaving and reaching 0, as the drain sees it each frame. The
// manager shows it again on every frame it finds it at 0 in the world, and
// hides it for an event.
inline void watch_compass(Engine& e) {
    HU_STEP("watch_compass");
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

// The target arrows have no open or close of their own either: opened and
// closed are the target window's draw starting and ending to paint one, as
// the drain reads it each frame.
inline void watch_targetarrow(Engine& e) {
    HU_STEP("watch_targetarrow");
    const int n = e.inv.targetarrow;
    if (n < 0) {
        return;
    }
    const bool drawn = targetarrow_drawn(e);
    if (drawn == (e.targetarrow_shown != 0)) {
        return;
    }
    e.targetarrow_shown = drawn ? 1 : 0;
    e.events.post(pack_event(drawn ? kEvOpened : kEvClosed, n));
}

// ---------------------------------------------------------------------------
// The offer windows (core.h's kOfferNames): the trade with another player
// and the trade with an NPC. Each takes items and gil into its offer through
// the game's own stage, as the inventory list's pick and the gil box hand
// them over; the trade's stage sends each change to the server at once and
// shows it once the server acknowledges it, the NPC trade's keeps the offer
// until its OK sends it whole.

const int kTradeOkRow = 9;              // the trade's rows: the OK, the cancel, the gil
const int kTradeCancelRow = 10;
const int kTradeGilRow = 11;

// The trade's and the NPC trade's state as either thread finds it, every
// pointer checked by whoever filled it in: the window's controller with
// kTradeBytes or kHandoverBytes readable, its window open now, the player's
// data with every table offer_data_ok() names readable, the throttle's
// intervals readable, and the game's clock (`clock` false when not read).
struct OfferNow {
    const uint8_t* ctl;
    const uint8_t* menu;
    const uint8_t* data;
    const uint8_t* intervals;
    uint32_t now;
    bool clock;
};

// What the replies and options() read of one window: whether it is open or
// blocked, its offer, slots 0..9
// by slot (the trade's as the server acknowledged it, then the offers this
// engine has on their way, in `pending`), the partner's offer, the target's
// actor index and id, the trade's states, the gil the player holds, and the
// milliseconds until the window's packet may go again (-1 unknown).
struct OfferView {
    int which;
    bool open;
    bool blocked;           // a handle blocks the window: it never opens
    bool data;
    int32_t state;
    int32_t their_state;
    uint32_t target;
    uint32_t target_id;
    uint32_t gil;
    OfferEntry mine[kOfferTable];
    OfferEntry pending[kOfferSlots];
    int pending_count;
    uint32_t their_gil;
    OfferEntry theirs[kOfferTable];
    uint32_t held_gil;
    int32_t wait;
};

// An inventory item as its record holds it: `present` for an item, the gil's
// record and an empty one aside.
struct HeldItem {
    bool present;
    uint16_t id;
    uint32_t count;
};

inline size_t offer_bytes(int w) {
    return w == kOfferTrade ? kTradeBytes : kHandoverBytes;
}

// The player's data's tables the readers use, readable from this thread.
inline bool offer_data_ok(const OfferSites& s, const uint8_t* data) {
    return data && readable(data + s.inventory, kInventoryRecords * kItemRecordBytes)
        && readable(data + s.own, kOfferTable * kOwnEntry) && readable(data + s.theirs, kOfferTable * kTheirEntry)
        && readable(data + s.last_send + 4 * kSendTradeChange, 4 * (kSendHandover - kSendTradeChange + 1));
}

inline bool offer_intervals_ok(const OfferSites& s, const uint8_t* intervals) {
    return intervals
        && readable(intervals + s.interval + 4 * kSendTradeChange, 4 * (kSendHandover - kSendTradeChange + 1));
}

// The game's clock read as its routine reads it, from this thread.
inline bool clock_read(const OfferSites& s, uint32_t* now) {
    const uint8_t* clock = s.clock_object && readable(s.clock_object, 4) ? *s.clock_object : NULL;
    if (!clock || !readable(clock + kClockSeconds, 8)) {
        return false;
    }
    *now = rd32(clock, kClockSeconds) * 1000u + rd32(clock, kClockMs);
    return true;
}

// The milliseconds until the throttle lets packet `id` go again, as the
// allocator tests it: refused while the clock is not past the last send by
// the interval, 0 once it is or before any send.
inline int32_t throttle_wait(const OfferSites& s, const uint8_t* data, const uint8_t* intervals, uint32_t id,
                             uint32_t now) {
    const uint32_t last = rd32(data, s.last_send + 4 * id);
    if (!last) {
        return 0;
    }
    const uint32_t interval = rd32(intervals, s.interval + 4 * id);
    const uint32_t limit = last + interval;
    if (now > limit) {
        const int32_t whole = static_cast<int32_t>(interval & 0x7FFFFFFF);
        return now - interval - last > 0x7FFFFFFFu ? (whole ? whole : 1) : 0;
    }
    const uint32_t left = limit - now;
    return left <= 0x7FFFFFFFu ? static_cast<int32_t>(left) + 1 : 0;
}

// Inventory item `index`, 1..kInventoryRecords - 1, in `data` read as
// offer_data_ok() checked it.
inline HeldItem held_item(const OfferSites& s, const uint8_t* data, int index) {
    HeldItem h;
    memset(&h, 0, sizeof(h));
    if (!data || index < 1 || index >= kInventoryRecords) {
        return h;
    }
    const size_t at = s.inventory + static_cast<size_t>(index) * kItemRecordBytes;
    h.id = static_cast<uint16_t>(rd16(data, at));
    h.count = rd32(data, at + kItemCount);
    h.present = h.id != 0 && h.id != kGilId && h.count != 0;
    return h;
}

// The trade's acknowledged slot holding inventory index `index` (the
// game's own test of an item offered), else -1.
inline int offered_slot(const OfferView& v, int index) {
    for (int k = 1; k < kOfferTable; ++k) {
        if (v.mine[k].id != 0 && v.mine[k].index == index) {
            return k;
        }
    }
    return -1;
}

inline bool pending_index(const OfferView& v, int index) {
    for (int i = 0; i < v.pending_count; ++i) {
        if (v.pending[i].index == index) {
            return true;
        }
    }
    return false;
}

// Window w as `now` finds it; `pending`, the trade's offers on their way,
// a copy read by the caller, counted while they are this trade's.
inline void offer_view(const Engine& e, int w, const OfferNow& now, const TradePending* pending, OfferView* v) {
    const OfferSites& s = e.game.offer;
    memset(v, 0, sizeof(*v));
    v->which = w;
    v->open = now.ctl && now.menu;
    v->blocked = (e.holds.want[e.inv.offer_window[w]] & kWantBlocked) != 0;
    v->data = now.data != NULL;
    v->wait = -1;
    for (int k = 0; k < kOfferTable; ++k) {
        v->mine[k].slot = static_cast<int16_t>(k);
        v->theirs[k].slot = static_cast<int16_t>(k);
    }
    const uint8_t* ctl = now.ctl;
    if (ctl && w == kOfferTrade) {
        v->state = static_cast<int32_t>(rd32(ctl, kTradeState));
        v->their_state = static_cast<int32_t>(rd32(ctl, kTradeTheirState));
        v->target = rd32(ctl, kTradePartner);
        v->target_id = rd32(ctl, kTradePartnerId);
    } else if (ctl) {
        v->target = rd32(ctl, kHandoverNpc);
        v->target_id = rd32(ctl, kHandoverNpcId);
        v->gil = rd32(ctl, kHandoverGil);
        for (int k = 1; k < kOfferTable; ++k) {
            const size_t at = kHandoverEntries + k * kHandoverEntry;
            OfferEntry& m = v->mine[k];
            m.id = static_cast<uint16_t>(rd16(ctl, at));
            m.index = ctl[at + 2];
            m.count = ctl[at + 3];
            if (m.count == 0) {
                m.id = 0;
            }
        }
    }
    const uint8_t* data = now.data;
    if (!data) {
        return;
    }
    v->held_gil = rd32(data, s.inventory + kItemCount);
    if (now.clock && now.intervals) {
        v->wait = throttle_wait(s, data, now.intervals, w == kOfferTrade ? kSendTradeChange : kSendHandover, now.now);
    }
    if (w != kOfferTrade) {
        return;
    }
    for (int k = 0; k < kOfferTable; ++k) {
        OfferEntry& m = v->mine[k];
        m.count = rd32(data, s.own + k * kOwnEntry);
        m.id = static_cast<uint16_t>(rd16(data, s.own + k * kOwnEntry + kOwnId));
        m.index = data[s.own + k * kOwnEntry + kOwnIndex];
        OfferEntry& t = v->theirs[k];
        t.count = rd32(data, s.theirs + k * kTheirEntry + 4);
        t.id = t.count ? static_cast<uint16_t>(rd16(data, s.theirs + k * kTheirEntry)) : 0;
    }
    v->gil = v->mine[0].count;
    v->their_gil = v->theirs[0].count;
    const bool same = pending && pending->instance == static_cast<uint32_t>(e.instance[e.inv.offer_window[w]])
        && pending->partner == v->target_id;
    for (int i = 0; same && i < pending->count && i < kOfferSlots; ++i) {
        if (offered_slot(*v, pending->entry[i].index) < 0) {
            v->pending[v->pending_count] = pending->entry[i];
            v->pending[v->pending_count++].pending = 1;
        }
    }
}

// Why a window that is not open takes no reply: blocked, it never opens,
// and with it the trade is canceled, the NPC trade never starts.
inline const char* offer_closed(const OfferView& v, char* buf, size_t size) {
    const char* nm = kOfferNames[v.which];
    if (v.blocked) {
        snprintf(buf, size, "%s is blocked and never opens: %s, and nothing waits on a reply", nm,
            v.which == kOfferTrade ? "the game cancels the trade instead" : "the NPC trade does not start");
    } else {
        snprintf(buf, size, "%s is not open", nm);
    }
    return buf;
}

// Why window w takes no reply now, or NULL. `change`: an offer or a
// withdrawal, which the trade takes only until the player's own OK.
inline const char* offer_window_refusal(const OfferView& v, bool change, char* buf, size_t size) {
    if (!v.open) {
        return offer_closed(v, buf, size);
    }
    if (change && !v.data) {
        snprintf(buf, size, "the player's data is not loaded");
    } else if (v.which == kOfferTrade && v.target_id == 0) {
        snprintf(buf, size, "the trade has no partner");
    } else if (v.which == kOfferTrade && change && v.state == kTradeOkSent) {
        snprintf(buf, size, "your OK is in: the trade takes no more changes");
    } else if (v.which == kOfferTrade && v.state != kTradeOpen && v.state != kTradeOkSent) {
        snprintf(buf, size, "no trade is under way");
    } else if (v.which == kOfferHandover && v.target == 0) {
        snprintf(buf, size, "handover has no NPC to hand anything to");
    } else {
        return NULL;
    }
    return buf;
}

// Why the window's packet may not go now: the game sends one a second.
inline const char* throttle_refusal(const OfferView& v, char* buf, size_t size) {
    if (v.wait < 0) {
        snprintf(buf, size, "the game's clock cannot be read, so whether the game would send it now is not known");
    } else if (v.wait > 0) {
        const int tenths = v.wait < 150 ? 1 : (v.wait + 50) / 100;
        snprintf(buf, size, "the game sends one %s per second; try again in %d.%d s",
            v.which == kOfferTrade ? "trade change" : "NPC trade", tenths / 10, tenths % 10);
    } else {
        return NULL;
    }
    return buf;
}

// Why an offer in v's window of `count` of inventory item `index` (`held`
// as its record holds it), or with index 0 of `count` gil, is refused now,
// or NULL with the slot an item takes in *slot: the first free one of the
// window's eight, the trade's offers on their way counted as taken.
inline const char* offer_refusal(const OfferView& v, int index, const HeldItem& held, int64_t count, int* slot,
                                 char* buf, size_t size) {
    *slot = 0;
    const char* why = offer_window_refusal(v, true, buf, size);
    if (!why && v.which == kOfferTrade) {
        why = throttle_refusal(v, buf, size);
    }
    if (why) {
        return why;
    }
    if (index == 0) {
        if (count < 0) {
            snprintf(buf, size, "the gil offered is 0 or more");
        } else if (count > static_cast<int64_t>(v.held_gil)) {
            snprintf(buf, size, "the player holds %u gil", static_cast<unsigned>(v.held_gil));
        } else {
            return NULL;
        }
        return buf;
    }
    if (count <= 0) {
        snprintf(buf, size, "count is 1 or more; use withdraw to take it out");
        return buf;
    }
    if (!held.present) {
        snprintf(buf, size, "inventory slot %d holds no item", index);
        return buf;
    }
    if (count > static_cast<int64_t>(held.count) || (v.which == kOfferHandover && count > 0xFF)) {
        snprintf(buf, size, "inventory slot %d holds %u of that item, fewer than %lld", index,
            static_cast<unsigned>(held.count), static_cast<long long>(count));
        return buf;
    }
    if (offered_slot(v, index) >= 0 || pending_index(v, index)) {
        snprintf(buf, size, "that item is already offered; withdraw it first");
        return buf;
    }
    for (int k = 1; k <= kOfferSlots; ++k) {
        bool taken = v.mine[k].id != 0;
        for (int i = 0; i < v.pending_count; ++i) {
            taken = taken || v.pending[i].slot == k;
        }
        if (!taken) {
            *slot = k;
            return NULL;
        }
    }
    snprintf(buf, size, "all %d item slots are full; withdraw one first", kOfferSlots);
    return buf;
}

// Why taking inventory item `index` back out of v's window is refused now,
// or NULL with the slot holding it in *slot.
inline const char* withdraw_refusal(const OfferView& v, int index, int* slot, char* buf, size_t size) {
    *slot = 0;
    const char* why = offer_window_refusal(v, true, buf, size);
    if (!why && v.which == kOfferTrade) {
        why = throttle_refusal(v, buf, size);
    }
    if (why) {
        return why;
    }
    const int k = index >= 1 && index < kInventoryRecords ? offered_slot(v, index) : -1;
    if (k >= 1) {
        *slot = k;
        return NULL;
    }
    if (pending_index(v, index)) {
        snprintf(buf, size, "the server has not acknowledged that item's offer yet; withdraw it once options() shows"
            " it without pending");
    } else {
        snprintf(buf, size, "that item is not offered");
    }
    return buf;
}

// Why the window's OK (`answer`) or its cancel is refused now, or NULL.
inline const char* offer_reply_refusal(const OfferView& v, bool answer, char* buf, size_t size) {
    if (!answer) {
        return v.open ? NULL : offer_closed(v, buf, size);
    }
    if (v.which == kOfferTrade) {
        if (v.open && v.target_id != 0 && v.state == kTradeOkSent) {
            snprintf(buf, size, "your OK is already in");
            return buf;
        }
        return offer_window_refusal(v, false, buf, size);
    }
    const char* why = offer_window_refusal(v, false, buf, size);
    if (why) {
        return why;
    }
    bool offered = v.gil != 0;
    for (int k = 1; k <= kOfferSlots; ++k) {
        offered = offered || v.mine[k].id != 0;
    }
    if (!offered) {
        snprintf(buf, size, "nothing is offered; offer an item or gil first");
        return buf;
    }
    if (!v.data) {
        snprintf(buf, size, "the player's data is not loaded");
        return buf;
    }
    return throttle_refusal(v, buf, size);
}

// An actor's name by its index in the actor table, as the game's own reader
// takes it: the titled name while its length word is set, else the name;
// false when no actor holds the index. Every hop guarded.
inline bool actor_name(const OfferSites& s, uint32_t index, char* out, size_t size) {
    out[0] = '\0';
    if (!s.actors || index == 0 || !readable(s.actors + index, 4)) {
        return false;
    }
    const uint8_t* actor = s.actors[index];
    if (!actor || !readable(actor + s.actor_titled, 2)) {
        return false;
    }
    const size_t at = rd16(actor, s.actor_titled) != 0 ? s.actor_title : s.actor_name;
    if (!readable(actor + at, kActorNameBytes)) {
        return false;
    }
    size_t n = 0;
    while (n + 1 < size && n < kActorNameBytes && actor[at + n]) {
        out[n] = static_cast<char>(actor[at + n]);
        ++n;
    }
    out[n] = '\0';
    return true;
}

// Window w as the game thread finds it.
inline OfferNow offer_now(Engine& e, int w) {
    const OfferSites& s = e.game.offer;
    OfferNow now;
    memset(&now, 0, sizeof(now));
    const int n = e.inv.offer_window[w];
    now.ctl = controller_at(e, n);
    now.menu = now.ctl ? open_menu(e, n, NULL) : NULL;
    const uint8_t* data = s.player ? *s.player : NULL;
    now.data = offer_data_ok(s, data) ? data : NULL;
    const uint8_t* intervals = s.intervals ? *s.intervals : NULL;
    now.intervals = offer_intervals_ok(s, intervals) ? intervals : NULL;
    if (s.clock) {
        now.now = s.clock();
        now.clock = true;
    }
    return now;
}

inline void pending_write(Engine& e, const TradePending& p) {
    TradePending& t = e.trade_pending;
    InterlockedIncrement(&t.seq);
    t.count = p.count;
    memcpy(t.entry, p.entry, sizeof(t.entry));
    t.instance = p.instance;
    t.partner = p.partner;
    InterlockedIncrement(&t.seq);
}

// The trade's offers on their way, each dropped once the server's
// acknowledgment has written its item into the game's own table of the
// offer, and every one once the trade ends or another starts: the window
// closed or opened again, another partner, the trade's state neither 3
// (open to changes) nor 4 (the player's OK in).
inline void watch_trade(Engine& e) {
    HU_STEP("watch_trade");
    const TradePending& p = e.trade_pending;
    if (p.count == 0) {
        return;
    }
    const int n = e.inv.offer_window[kOfferTrade];
    const OfferNow now = offer_now(e, kOfferTrade);
    OfferView v;
    offer_view(e, kOfferTrade, now, NULL, &v);
    TradePending kept;
    memset(&kept, 0, sizeof(kept));
    kept.instance = p.instance;
    kept.partner = p.partner;
    const bool same = v.open && now.data && p.instance == static_cast<uint32_t>(e.instance[n])
        && p.partner == v.target_id && (v.state == kTradeOpen || v.state == kTradeOkSent);
    for (int i = 0; same && i < p.count; ++i) {
        if (offered_slot(v, p.entry[i].index) < 0) {
            kept.entry[kept.count++] = p.entry[i];
        }
    }
    if (kept.count != p.count) {
        pending_write(e, kept);
    }
}

// The game's own gray test of inventory item `index`'s row in the list the
// window's items are picked from, as the list builds it: the item's data
// from the cache, its lock byte raised around the window's mode's test of
// the row and the item's description; a row the test does not mark offered
// grayed by its status, unless the list's class of that status lists it (or
// lists it for this mode alone). kGrayTakes while the game would let the
// row be picked. Game thread.
inline uint8_t gray_code(Engine& e, int w, int index) {
    const OfferSites& s = e.game.offer;
    uint8_t* mode = s.mode[w] ? *s.mode[w] : NULL;
    uint8_t* cache = s.cache ? *s.cache : NULL;
    const uint8_t* record = s.item(0, static_cast<uint32_t>(index));
    const uint8_t* vtable = mode ? rdptr(mode, 0) : NULL;
    if (!mode || !cache || !record || !vtable) {
        return kGrayNoList;
    }
    uint8_t* data = s.item_data(cache, NULL, rd16(record, 0));
    if (!data) {
        return kGrayNoData;
    }
    ++data[s.lock];
    const uint32_t description = s.describe(data);
    GrayTestFn test;
    const void* fn = rdptr(vtable, kModeGrayTest);
    memcpy(&test, &fn, sizeof(test));
    uint32_t color = test(mode, NULL, record, description);
    --data[s.lock];
    if (color == 0) {
        return kGrayUnlisted;
    }
    if (color != kRowOffered) {
        const uint8_t status = record[kItemStatus];
        const uint8_t cls = status < kStatusClasses ? s.status_class[status] : 0xFF;
        const bool listed = cls == kStatusListed
            || (cls == kStatusModal && s.status_mode && *s.status_mode == mode);
        if (!listed) {
            color = kRowGray;
        }
    }
    return color == kRowGray || color == kRowRefused ? kGrayGrayed : kGrayTakes;
}

// Why an offer of an item gray_code() answered `code` for in window w is
// refused, or NULL.
inline const char* gray_refusal(uint8_t code, int w, char* buf, size_t size) {
    switch (code) {
    case kGrayNoList:
        snprintf(buf, size, "the game's inventory list for %s is not set up", kOfferNames[w]);
        return buf;
    case kGrayNoData:
        snprintf(buf, size, "the game has no data for that item, so its list does not show it");
        return buf;
    case kGrayUnlisted:
        snprintf(buf, size, "the game's list for %s does not show that item", kOfferNames[w]);
        return buf;
    case kGrayGrayed:
        snprintf(buf, size, "%s",
            w == kOfferTrade ? "that item cannot be traded" : "that item cannot be handed to an NPC");
        return buf;
    default:
        return NULL;
    }
}

inline const char* offer_gray(Engine& e, int w, int index, char* buf, size_t size) {
    return gray_refusal(gray_code(e, w, index), w, buf, size);
}

// The gray test of every inventory item for the offer window open, run each
// frame: its inputs include the equipment and the offer, which no word this
// engine reads signals a change of. Written only when it changed.
inline void watch_gray(Engine& e) {
    HU_STEP("watch_gray");
    const OfferSites& s = e.game.offer;
    const uint8_t* data = s.player ? *s.player : NULL;
    GrayRows r;
    memset(&r, 0, sizeof(r));
    for (int w = 0; w < kOfferCount && !r.open; ++w) {
        const int n = e.inv.offer_window[w];
        if (!controller_at(e, n) || !open_menu(e, n, NULL) || !offer_data_ok(s, data)) {
            continue;
        }
        r.open = true;
        r.which = w;
        r.instance = static_cast<uint32_t>(e.instance[n]);
        for (int i = 1; i < kInventoryRecords; ++i) {
            const HeldItem held = held_item(s, data, i);
            if (held.present) {
                r.id[i] = held.id;
                r.code[i] = gray_code(e, w, i);
            }
        }
    }
    GrayTable& t = e.gray_table;
    if (memcmp(&r, &t.rows, sizeof(r)) == 0) {
        return;
    }
    InterlockedIncrement(&t.seq);
    memcpy(&t.rows, &r, sizeof(r));
    InterlockedIncrement(&t.seq);
}

// Why `rows` refuses an offer of inventory item `index` (`held` as its record
// holds it now) in window w's open `instance`, or NULL: only while the test
// ran on that open and on that item.
inline const char* gray_rows_refusal(const GrayRows& rows, int w, uint32_t instance, int index,
                                     const HeldItem& held, char* buf, size_t size) {
    if (!rows.open || rows.which != w || rows.instance != instance || index < 1 || index >= kInventoryRecords
        || !held.present || rows.id[index] != held.id) {
        return NULL;
    }
    return gray_refusal(rows.code[index], w, buf, size);
}

// What each window's own rows do with an item, the gil, the OK and the
// cancel, with no input event: an offer or a withdrawal through the window's
// stage, the trade's on the row of the slot (its stage reads the cursor's
// row) and the NPC trade's on the slot its stage reads, the trade's offer
// then on its way until the server acknowledges it; the OK, the NPC trade's
// window closed once it went; the cancel, the trade's windows closed once it
// went. Refused, each posts the error.
inline void offer_reply(Engine& e, const Command& c) {
    const int w = e.inv.offer_of(c.target);
    if (w < 0) {
        return;
    }
    const OfferSites& s = e.game.offer;
    const OfferNow now = offer_now(e, w);
    OfferView v;
    offer_view(e, w, now, &e.trade_pending, &v);
    uint8_t* ctl = const_cast<uint8_t*>(now.ctl);
    uint8_t* menu = const_cast<uint8_t*>(now.menu);
    char buf[kErrorReason];
    const char* why = NULL;
    const int index = c.x;
    int slot = 0;
    switch (c.op) {
    case kOpOffer: {
        const HeldItem held = held_item(s, now.data, index);
        why = offer_refusal(v, index, held, static_cast<int64_t>(c.value), &slot, buf, sizeof(buf));
        if (!why && index) {
            why = offer_gray(e, w, index, buf, sizeof(buf));
        }
        if (why) {
            break;
        }
        const uint32_t id = index ? held.id : kGilId;
        if (w == kOfferHandover) {
            if (index) {
                ctl[kHandoverRow] = static_cast<uint8_t>(slot);
            }
            s.stage[w](ctl, NULL, static_cast<uint32_t>(index), id, c.value);
            break;
        }
        const uint32_t before = rd32(now.data, s.last_send + 4 * kSendTradeChange);
        e.game.select_row(menu, NULL, index ? slot : kTradeGilRow);
        s.stage[w](ctl, NULL, static_cast<uint32_t>(index), id, c.value);
        if (rd32(now.data, s.last_send + 4 * kSendTradeChange) == before) {
            why = "the game did not send the change";
            break;
        }
        if (index) {
            TradePending p = e.trade_pending;
            const int n = e.inv.offer_window[w];
            if (p.count == 0) {
                p.instance = static_cast<uint32_t>(e.instance[n]);
                p.partner = v.target_id;
            }
            if (p.count < kOfferSlots) {
                OfferEntry& entry = p.entry[p.count++];
                entry.slot = static_cast<int16_t>(slot);
                entry.index = static_cast<uint8_t>(index);
                entry.pending = 1;
                entry.id = static_cast<uint16_t>(id);
                entry.count = c.value;
            }
            pending_write(e, p);
        }
        break;
    }
    case kOpWithdraw: {
        why = withdraw_refusal(v, index, &slot, buf, sizeof(buf));
        if (why) {
            break;
        }
        const OfferEntry& m = v.mine[slot];
        if (w == kOfferHandover) {
            ctl[kHandoverRow] = static_cast<uint8_t>(slot);
            s.stage[w](ctl, NULL, m.index, m.id, m.count);
            break;
        }
        const uint32_t before = rd32(now.data, s.last_send + 4 * kSendTradeChange);
        e.game.select_row(menu, NULL, slot);
        s.stage[w](ctl, NULL, m.index, m.id, 0);
        if (rd32(now.data, s.last_send + 4 * kSendTradeChange) == before) {
            why = "the game did not send the change";
        }
        break;
    }
    case kOpOfferAnswer:
        why = offer_reply_refusal(v, true, buf, sizeof(buf));
        if (why) {
            break;
        }
        if (!(s.ok[w](ctl, NULL) & 0xFF)) {
            why = w == kOfferTrade ? "the game did not send the OK" : "the game did not send the NPC trade";
        } else if (w == kOfferHandover) {
            s.handover_close(ctl, NULL);
        }
        break;
    case kOpOfferCancel:
        why = offer_reply_refusal(v, false, buf, sizeof(buf));
        if (why) {
            break;
        }
        if (w == kOfferHandover) {
            s.handover_close(ctl, NULL);
        } else if (s.trade_cancel(ctl, NULL, 0) & 0xFF) {
            s.trade_close(ctl, NULL);
        } else {
            why = "the game did not send the cancel";
        }
        break;
    default:
        break;
    }
    if (why) {
        drain_error(e, "%s", why);
    }
    watch_trade(e);
}

inline void drain(Engine& e) {
    HU_STEP("drain");
    HU_HEAP_FRAME(e);
    build_glyph_table(e);
    fill_setting_labels(e);
    reconcile_hidden(e);
    watch_pending(e);
    watch_boxes(e);
    watch_asked(e);
    watch_msgline(e);
    watch_trade(e);
    watch_open_checks(e);
    Command c;
    while (e.commands.pop(&c)) {
        e.current_thread = GetCurrentThreadId();
        e.current = &c;
        HU_HEAP_COMMAND(e, c, true);
        run_command(e, c);
        HU_HEAP_COMMAND(e, c, false);
        e.current = NULL;
        if ((c.flags & kCmdReply) && c.target >= 0 && c.target < kMaxNames) {
            InterlockedDecrement(&e.replies[c.target]);
        }
        InterlockedIncrement(&e.completed);
    }
    watch_pending(e);
    watch_boxes(e);
    watch_asked(e);
    watch_msgline(e);
    watch_trade(e);
    watch_gray(e);
    watch_door(e);
    watch_pages(e);
    watch_compass(e);
    watch_targetarrow(e);
    watch_yesno(e);
    // The game's close of a dormant menu marks it and defers its destruction
    // without a staged close, so a covered window that closes is noticed
    // here, at the marks; so is any other the hook did not see leave, so
    // that the events never contradict open_menu().
    for (int n = 0; n < e.inv.count; ++n) {
        if ((e.covered[n] || e.reported[n]) && !open_menu(e, n, NULL)) {
            e.covered[n] = 0;
            post_closed(e, n);
        }
    }
    watch_cursors(e);
    HU_HEAP_FRAME_END();
}

// ---------------------------------------------------------------------------
// The player's input on a hidden prompt.

// Whether prompt n asks the player something now, as the game thread finds
// it by the rule its replies and options() go by: not while they find
// nothing pending in it, its window not open or its question answered or
// not yet asked. A state a reply waits out, or one this engine cannot read,
// asks, and so do the post boxes, whose questions options() does not read.
// The area list asks only for an NPC event's choice, the one its replies
// end.
inline bool prompt_asks(Engine& e, int n) {
    HU_STEP_AT("prompt_asks", n);
    const char* nm = e.inv.names[n].name;
    char why[kErrorReason];
    bool none = false;
    if (yesno_window(nm)) {
        yesno_asks(e, n, controller_at(e, n), why, sizeof(why), &none);
        return !none;
    }
    const int b = e.inv.box_of(n);
    if (b >= 0) {
        const BoxRecord& r = e.boxes[b];
        const uint8_t* owner_ctl = NULL;
        const uint8_t* owner = r.active ? box_owner(e, r, &owner_ctl) : NULL;
        bool blocked = false;
        return box_asks(r, b, controller_at(e, n), owner, owner_ctl, open_menu(e, n, NULL) != NULL, &blocked);
    }
    const int a = e.inv.asked_of(n);
    if (a >= 0) {
        bool blocked = false;
        return !asked_pending(e, a, e.asked[a], asked_now(e, a), &blocked, why, sizeof(why));
    }
    if (e.inv.dialogue_of(n) >= 0) {
        const uint8_t* ctl = controller_at(e, n);
        return dialogue_message(ctl ? rdptr(ctl, kBoxText) : NULL) != NULL;
    }
    const int t = e.inv.text_of(n);
    if (t >= 0) {
        text_asks(t, text_now(e, t), e.msgline, false, why, sizeof(why), &none);
        return !none;
    }
    if (strcmp(nm, "passinpu") == 0) {
        return passinpu_asks(controller_at(e, n));
    }
    if (strcmp(nm, "prtyjoin") == 0) {
        return session_live(e, kSessionInvite);
    }
    if (strcmp(nm, "link5") == 0) {
        return link5_pending(e) != NULL;
    }
    if (strcmp(nm, "arealist") == 0) {
        return area_refusal(controller_at(e, n)) == NULL;
    }
    if (strcmp(nm, "query") == 0 || strcmp(nm, "dead") == 0 || e.inv.offer_of(n) >= 0) {
        return open_menu(e, n, NULL) != NULL;
    }
    return true;
}

// A window the engine hid that asks the player something now: the one the
// input hooks hold the player's keys, the menu mouse mode and the dialogue
// boxes' left click off. Read at the input, never from what a drain saw: a
// prompt that opens hidden, or starts asking within a frame, is held from
// its first input. Any other hidden window takes them as it would shown, a
// prompt that asks nothing now among them; the party list, driven by
// <stpt> and <stal> through its keys, must. Only a window found among the
// hidden draws reads its prompt's state.
inline bool hidden_prompt(Engine& e, const uint8_t* menu) {
    const int n = hidden_window(e, menu);
    return n >= 0 && e.inv.prompt[n] && prompt_asks(e, n);
}

// ---------------------------------------------------------------------------
// Hook handlers. `user` is the Engine.

// Open by name: args are (key, activate, overlap_aware), this = the manager.
// A blocked name returns 0, the game's own "not available", and so does the
// window open_list() keeps from opening, with no event and not counted. Every open of a
// window is counted, a refused one too: it is a new prompt all the same. A
// number box's setup opens it once its question is written, so a refused
// open of one keeps that question, replacing any kept before; so does an
// Asked window's that keeps one (asked_records()), and msgline's, whose
// opener sets its message up before the open.
inline int __cdecl hook_open_pre(HuFrame* f) {
    HU_STEP("hook open pre");
    Engine& e = *static_cast<Engine*>(f->user);
    const char* key = reinterpret_cast<const char*>(f->args[0]);
    const int n = key ? e.inv.find_key(key) : -1;
    HU_HEAP_HIDDEN_CHECK(e, n, "hook_open_pre %s", e.inv.names[n].name);
    if (n >= 0 && n + 1 == e.suppressed && e.suppress_thread == GetCurrentThreadId()) {
        f->result = 0;
        return 1;
    }
    if (n >= 0) {
        InterlockedIncrement(&e.instance[n]);
        if (e.inv.dialogue_of(n) >= 0) {
            InterlockedIncrement(&e.dialogue_opens);
        }
    }
    if (n >= 0 && (e.holds.want[n] & kWantBlocked)) {
        const int b = e.inv.box_of(n);
        const int a = e.inv.asked_of(n);
        const bool message = e.inv.text_of(n) == kTextMsgline;
        if (b >= 0) {
            box_end(e, b, -1, 0);
        }
        if (a >= 0) {
            asked_end(e, a, -1, 0);
        }
        if (message) {
            msgline_end(e, -1, 0);
        }
        e.events.post(pack_event(kEvBlocked, n));
        if (b >= 0) {
            box_refused(e, b);
        }
        if (a >= 0) {
            asked_refused(e, a);
        }
        if (message) {
            msgline_refused(e);
        }
        f->result = 0;
        return 1;
    }
    if (n < 0) {
        InterlockedIncrement(&e.unmatched_keys);
    }
    // Reported open but gone, freed without the staged close the hooks see:
    // closed now, so that a new instance at the same address is an open.
    if (n >= 0 && e.reported[n] && !open_menu(e, n, NULL)) {
        e.covered[n] = 0;
        post_closed(e, n);
    }
    e.opens.push(f->args, GetCurrentThreadId(), static_cast<int16_t>(n));
    return 0;
}

inline void __cdecl hook_open_post(HuFrame* f) {
    HU_STEP("hook open post");
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
    HU_HEAP_HIDDEN_CHECK(e, n, "hook_open_post %s entry", e.inv.names[n].name);
    if (e.place[n].applied_hidden) {
        menu[kMenuMouse] = 0;
        end_hidden_field(e, n, controller_of(e, n, menu), menu);
        set_hidden_draw(e, n, menu);
    }
    if (e.inv.text_of(n) == kTextMsgline) {
        msgline_end(e, -1, 0);
    }
    // A box or an Asked window that opens asks with its window.
    const int box = e.inv.box_of(n);
    if (box >= 0) {
        box_end(e, box, -1, 0);
    }
    const int asked = e.inv.asked_of(n);
    if (asked >= 0) {
        asked_end(e, asked, -1, 0);
    }
    if (asked == kAskedDoor) {
        watch_door(e);
    }
    // The opener stores its kind in the controller only after this open
    // returns, so the opened event takes the kind the opener's pre kept.
    uint32_t opened = pack_event(kEvOpened, n);
    int32_t category = -1;
    if (n == e.inv.ability && e.ability_pending) {
        opened = pack_event_category(kEvOpened, n, e.ability_pending_kind);
        category = e.ability_pending_kind;
        e.ability_pending = 0;
    } else if (n == e.inv.ability) {
        const uint8_t* ctl = controller_at(e, n);
        category = ctl ? static_cast<int32_t>(rd32(ctl, kAbilityCategory)) : -1;
    }
    const int waiting = waiting_on(e, n);
    const int game_x = rd16(menu, kMenuOrigin);
    const int game_y = rd16(menu, kMenuOrigin + 2);
    reapply(e, n, menu, waiting < 0);
    if (waiting >= 0) {
        carry_waiting(e, waiting, n, menu, game_x, game_y);
    }
    // An open that brings a window already reported open out from under
    // another is an uncover; the show path inside the open leaves it to here.
    if (e.covered[n] && e.reported[n] == menu && rd32(menu, kMenuState) != kStateDormant) {
        e.events.post(window_event(e, kEvUncovered, n));
    }
    e.covered[n] = 0;
    post_opened(e, n, menu, opened, category);
    HU_HEAP_HIDDEN_CHECK(e, n, "hook_open_post %s exit", e.inv.names[n].name);
}

// The show path, arg (menu): a menu becoming visible. The routine shows
// only a dormant menu (kStateDormant) that is not closing and still has its
// registry entry, and returns at once on any other, as it is called for
// menus already up; the pre goes by the same test. Inside an open of the
// same menu the open's post reports it; outside one it comes back from
// under another window.
inline int __cdecl hook_show_pre(HuFrame* f) {
    HU_STEP("hook show pre");
    Engine& e = *static_cast<Engine*>(f->user);
    const uint8_t* menu = reinterpret_cast<const uint8_t*>(f->args[0]);
    if (!menu || menu[kMenuClosing] != 0 || !rdptr(menu, kMenuRes) || rd32(menu, kMenuState) != kStateDormant) {
        return 0;
    }
    const int n = name_of_menu(e, menu);
    if (n >= 0 && !e.opens.contains(GetCurrentThreadId(), static_cast<int16_t>(n))) {
        e.covered[n] = 0;
        e.events.post(window_event(e, kEvUncovered, n));
    }
    return 0;
}

// The staged close, first arg (menu): a menu leaving the screen. Close by
// instance marks it closing first; the update loop, and controllers hiding
// their own lists, send a menu dormant through the same routine with the mark
// clear. On a menu already dormant the routine changes nothing.
inline int __cdecl hook_close_pre(HuFrame* f) {
    HU_STEP("hook staged close pre");
    Engine& e = *static_cast<Engine*>(f->user);
    const uint8_t* menu = reinterpret_cast<const uint8_t*>(f->args[0]);
    if (!menu || rd32(menu, kMenuState) == kStateDormant) {
        return 0;
    }
    const int n = name_of_menu(e, menu);
    HU_HEAP_HIDDEN_CHECK(e, n, "hook_close_pre %s entry", e.inv.names[n].name);
    if (n >= 0) {
        const bool closing = menu[kMenuClosing] != 0;
        e.covered[n] = closing ? 0 : 1;
        if (closing) {
            post_closed(e, n);
        } else {
            e.events.post(window_event(e, kEvCovered, n));
        }
    }
    HU_HEAP_HIDDEN_CHECK(e, n, "hook_close_pre %s exit", e.inv.names[n].name);
    return 0;
}

// The per-frame UI update: the one place queued work reaches the game.
inline int __cdecl hook_update_pre(HuFrame* f) {
    HU_STEP("hook ui update pre");
    drain(*static_cast<Engine*>(f->user));
    return 0;
}

// The mouse mode picker, ECX = the mouse object, no arguments, nothing
// returned: mode 2 while the manager has a menu up, else 1. Every mouse path
// into a menu needs mode 2, and mouse input goes only to the active menu, so
// while that menu is a prompt this engine hid that asks now the pointer
// stays in world mode and the original does not run. A hidden window's +0x77
// keeps clicks off its rows either way.
inline int __cdecl hook_mouse_mode_pre(HuFrame* f) {
    HU_HOT_STEP("hook mouse mode pre");
    Engine& e = *static_cast<Engine*>(f->user);
    const uint8_t* active = e.game.mcb ? rdptr(e.game.mcb, kMcbActive) : NULL;
    if (!hidden_prompt(e, active)) {
        return 0;
    }
    reinterpret_cast<uint8_t*>(f->ecx)[kMouseMode] = kMouseWorld;
    return 1;
}

// The menu input sink, ECX = a menu, arg (code), returning a word its
// callers ignore. Every key and gamepad code reaches the active menu through
// the routing routine's one call to it; while that menu is a prompt this
// engine hid that asks now, a code from that call never reaches it and the
// sink answers 0, as it does for a dormant menu. Its other callers --
// SetCursor's code 9, the wheel, a controller passing a code on -- are not
// the player's keys.
inline int __cdecl hook_menu_input_pre(HuFrame* f) {
    HU_HOT_STEP("hook menu input pre");
    Engine& e = *static_cast<Engine*>(f->user);
    if (f->return_address != e.game.routing_return || !e.game.mcb) {
        return 0;
    }
    const uint8_t* active = rdptr(e.game.mcb, kMcbActive);
    if (!active || reinterpret_cast<uintptr_t>(active) != f->ecx || !hidden_prompt(e, active)) {
        return 0;
    }
    f->result = 0;
    return 1;
}

// The key test, cdecl (0x3E, action, query, slot), al 1 while the action
// fires; some two hundred sites call it, many every frame, so a call from
// anywhere but the dialogue boxes' tick's test of the left button passes on
// one compare. That test confirms the box whose tick runs, the active menu,
// on a click anywhere; while that box is a dialogue box this engine hid
// that shows a message, the test answers no and the original does not run.
inline int __cdecl hook_key_test_pre(HuFrame* f) {
    HU_HOT_STEP("hook key test pre");
    Engine& e = *static_cast<Engine*>(f->user);
    if (f->return_address != e.game.dialogue.click_return || !e.game.mcb) {
        return 0;
    }
    const uint8_t* active = rdptr(e.game.mcb, kMcbActive);
    if (!hidden_prompt(e, active) || e.inv.dialogue_of(name_of_menu(e, active)) < 0) {
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
    HU_HOT_STEP("hook compass pre");
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

// The per-menu draw, ECX = the menu, no arguments, nothing returned: the
// walk calls it once for each menu of a drawn layer, and all the menu does
// each frame is inside it: the update that ends its opening and its staged
// close, its input-wait count, and its controller's draw, which in many
// classes opens, closes and keeps time besides painting. A hidden window's
// draw runs whole, as shown; the pre marks its level for the draw
// primitive's pre, and the post clears the level it leaves. Nested draws
// unwind in order; past 32 levels none is marked. Once for every menu drawn
// each frame: a scan of the hidden instances alone.
inline int __cdecl hook_menu_draw_pre(HuFrame* f) {
    HU_HOT_STEP("hook menu draw pre");
    Engine& e = *static_cast<Engine*>(f->user);
    const uint32_t level = e.draw_depth < 32 ? 1u << e.draw_depth : 0;
    ++e.draw_depth;
    if (hidden_window(e, reinterpret_cast<const uint8_t*>(f->ecx)) >= 0) {
        e.drawing_hidden |= level;
    }
    return 0;
}

inline void __cdecl hook_menu_draw_post(HuFrame* f) {
    HU_HOT_STEP("hook menu draw post");
    Engine& e = *static_cast<Engine*>(f->user);
    if (e.draw_depth > 0) {
        --e.draw_depth;
    }
    if (e.draw_depth < 32) {
        e.drawing_hidden &= ~(1u << e.draw_depth);
    }
}

// The sprite draw, ECX = the sprite, eight arguments, nothing returned. The
// target window's draw paints each of its arrows with one call, told apart
// by its return address; the arrow's draw is marked for the draw primitive's
// pre. Some hundreds of calls a frame: a compare each.
inline bool arrow_call(const Engine& e, const HuFrame* f) {
    return f->return_address == e.game.arrow_return[0] || f->return_address == e.game.arrow_return[1];
}

inline int __cdecl hook_sprite_pre(HuFrame* f) {
    HU_HOT_STEP("hook sprite pre");
    Engine& e = *static_cast<Engine*>(f->user);
    if (arrow_call(e, f)) {
        e.drawing_arrow = 1;
    }
    return 0;
}

inline void __cdecl hook_sprite_post(HuFrame* f) {
    HU_HOT_STEP("hook sprite post");
    Engine& e = *static_cast<Engine*>(f->user);
    if (arrow_call(e, f)) {
        e.drawing_arrow = 0;
    }
}

// The draw primitive, ECX = the renderer, four arguments, ret 0x10, the
// device's result in eax: every quad, glyph and model a menu's draw puts on
// the screen goes through it. The walk runs with the renderer's second
// target off, so the routine is the device's DrawPrimitiveUP and nothing
// else, and a call dropped leaves no renderer or device state a later draw
// relies on; it answers D3D_OK, as the device does. Inside a hidden window's
// draw every call is dropped but an arrow's, and an arrow's, whichever way
// the target window stands, while a handle hides targetarrow. Every draw of
// the game's, thousands a frame: flag tests alone.
inline int __cdecl hook_primitive_pre(HuFrame* f) {
    HU_HOT_STEP("hook draw primitive pre");
    const Engine& e = *static_cast<const Engine*>(f->user);
    const bool drop = e.drawing_arrow
        ? e.inv.targetarrow >= 0 && (e.holds.want[e.inv.targetarrow] & kWantHidden) != 0
        : e.drawing_hidden != 0;
    if (!drop) {
        return 0;
    }
    f->result = 0;
    return 1;
}

// The template swap, ECX = the menu, arg (key), nothing returned: a swap
// that binds another template rewrites the frame at the new template's
// default spot, and leaves it alone otherwise. Its pre keeps the menu and
// its frame for the post, whose registers are no longer the call's. A
// window a handle moved gets its remembered position back at once; its size
// stays the swap's. Inside the engine's own swaps and re-applies a swap is
// left to the engine, and inside an open of the window to the open's post.
inline int __cdecl hook_swap_pre(HuFrame* f) {
    HU_STEP("hook template swap pre");
    Engine& e = *static_cast<Engine*>(f->user);
    if (e.swap_guard) {
        return 0;
    }
    const uint8_t* menu = reinterpret_cast<const uint8_t*>(f->ecx);
    e.swap_menu = menu;
    for (int i = 0; menu && i < 4; ++i) {
        e.swap_frame[i] = rd16(menu, kMenuRect + 2 * i);
    }
    return 0;
}

inline void __cdecl hook_swap_post(HuFrame* f) {
    HU_STEP("hook template swap post");
    Engine& e = *static_cast<Engine*>(f->user);
    uint8_t* menu = const_cast<uint8_t*>(e.swap_menu);
    if (e.swap_guard || !menu) {
        return;
    }
    e.swap_menu = NULL;
    bool rewritten = false;
    for (int i = 0; i < 4; ++i) {
        rewritten = rewritten || rd16(menu, kMenuRect + 2 * i) != e.swap_frame[i];
    }
    const int n = rewritten ? name_of_menu(e, menu) : -1;
    if (n < 0 || e.opens.contains(GetCurrentThreadId(), static_cast<int16_t>(n))) {
        return;
    }
    ++e.swap_guard;
    reapply_position(e, n, menu);
    --e.swap_guard;
}

// The macro key gate, no arguments, al 1 while the macro keys may act. The
// handler consults it before opening a bar and before each number key, so
// with the gate at no nothing of the macro keys runs, as during a cutscene;
// while any handle blocks them the original does not run and the gate
// answers no.
inline int __cdecl hook_macro_gate_pre(HuFrame* f) {
    HU_HOT_STEP("hook macro gate pre");
    const Engine& e = *static_cast<const Engine*>(f->user);
    if (!e.holds.macros_want) {
        return 0;
    }
    f->result = 0;
    return 1;
}

// The ability opener, args (kind, flag, extra), nothing returned. With the
// ability window blocked whole, or its list `kind` blocked, the original
// does not run: nothing opens, abisortw included, and blocked{ability}
// carries the kind. Else the kind is kept for the opened event the open of
// ability inside the original posts, and the post forgets it once the
// original has returned, opened or not.
inline int __cdecl hook_ability_open_pre(HuFrame* f) {
    HU_STEP("hook ability open pre");
    Engine& e = *static_cast<Engine*>(f->user);
    const int n = e.inv.ability;
    if (n < 0) {
        return 0;
    }
    const int32_t kind = static_cast<int32_t>(f->args[0]);
    const bool listed = kind >= 0 && kind < kCategoryBits && (e.holds.ability_want & (1L << kind)) != 0;
    if ((e.holds.want[n] & kWantBlocked) || listed) {
        e.events.post(pack_event_category(kEvBlocked, n, kind));
        return 1;
    }
    e.ability_pending_kind = kind;
    e.ability_pending = 1;
    return 0;
}

inline void __cdecl hook_ability_open_post(HuFrame* f) {
    HU_STEP("hook ability open post");
    Engine& e = *static_cast<Engine*>(f->user);
    e.ability_pending = 0;
}

}  // namespace hu

#endif  // HIDEUI_GAME_H_
