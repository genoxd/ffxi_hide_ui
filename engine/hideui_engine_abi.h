// hideui_engine_abi.h - the contract between copies of _HideUI.dll in one
// client. Every addon ships its own copy; the first to install is the
// resident and holds the daemon's sites and every handle. Each later copy
// forwards its Lua calls to the resident through the table it publishes here.
//
// The rules of daemon/hideui_abi.h hold here too: plain C shapes, explicit
// calling conventions, 4-byte packing, HuEngineApi append-only with `size`
// checked before anything past it. A forwarder forwards to whatever resident
// is there, older or newer: a call whose slot lies past the resident's `size`
// answers that it needs a newer resident, and nothing else differs. A slot
// keeps its meaning for good; a call whose results change gets a new slot.

#ifndef HIDEUI_ENGINE_ABI_H_
#define HIDEUI_ENGINE_ABI_H_

#include "../daemon/hideui_abi.h"

// Bumped only when fields are appended to HuEngineApi: the table's version.
#define HU_ENGINE_ABI 7u

// Bumped whenever the engine binary changes, ABI or not: two addons can ship
// different builds at one ABI, and status names the one serving.
#define HU_ENGINE_BUILD "0.10.0"

// Written into the record last, behind a barrier. Any other value, zero
// included, means no resident: a copy that died mid-publish or a resident
// that shut down leaves the record free to publish again.
#define HU_ENGINE_MAGIC 0x48555231u

// Created and opened at this fixed size, never sizeof(HuEngineRecord), for
// the reason HU_MAPPING_BYTES gives. Local\ is session-scoped, so both names
// carry GetCurrentProcessId().
#define HU_ENGINE_MAPPING_BYTES 4096
#define HU_ENGINE_MAPPING_NAME_FORMAT "Local\\hideui_resident_v1_%08X"
#define HU_ENGINE_MUTEX_NAME_FORMAT "Local\\hideui_resident_lock_v1_%08X"

#pragma pack(push, 4)

// A handle as the resident's table knows it. slot -1 is no handle.
typedef struct HuEngineHandle {
    int32_t slot;
    uint32_t generation;
} HuEngineHandle;

// One polled event. `event` empty: a name the resident could not resolve,
// to be skipped. `fresh` is -1 for events that carry no `new`.
typedef struct HuEngineEvent {
    char event[12];
    char name[16];
    int32_t fresh;
} HuEngineEvent;

// One polled event, engine abi 3: opened, closed, covered, uncovered,
// blocked, or error with the refused call's `verb` and `reason`. `event`
// empty: nothing for this handle, to be skipped.
typedef struct HuEngineEvent3 {
    char event[12];
    char name[16];
    char verb[16];
    char reason[212];
} HuEngineEvent3;

// One polled event, engine abi 4: abi 3's, and `pending` with `what`
// (invite or post) and `pending` 1 when it appeared, 0 when it cleared.
typedef struct HuEngineEvent4 {
    char event[12];
    char name[16];
    char verb[16];
    char reason[212];
    char what[12];
    int32_t pending;
} HuEngineEvent4;

// The values a read returns to Lua, encoded as the Lua calls that push them
// (opcodes below), so the forwarder builds tables whose fields only the
// resident knows. The caller owns `data`. Past `capacity` the resident only
// counts: `used` > capacity means nothing in data may be read, and a retry
// with `used` bytes fits unless the state grew in between.
typedef struct HuEngineReply {
    uint32_t size;
    uint8_t* data;
    uint32_t capacity;
    uint32_t used;
} HuEngineReply;

// Little-endian, unaligned. FIELD and INDEX store the value on top into the
// table under it and pop the value. Closed: these carry every value a reply
// can hold, and an older forwarder must read any newer resident's replies,
// so none is ever added; new fields in a table need nothing new here.
enum {
    HU_REPLY_NIL = 1,
    HU_REPLY_FALSE = 2,
    HU_REPLY_TRUE = 3,
    HU_REPLY_NUMBER = 4,    /* 8 bytes: a double */
    HU_REPLY_STRING = 5,    /* 4-byte length, then the bytes */
    HU_REPLY_TABLE = 6,     /* a new, empty table */
    HU_REPLY_FIELD = 7,     /* 4-byte length, then the key */
    HU_REPLY_INDEX = 8      /* 4-byte signed integer key */
};

// A verb returns 1, or 0 with `why` filled. A number argument that was not a
// Lua number arrives as NaN; a string argument that was not a string arrives
// as NULL. The resident validates both, so its rules govern every copy.
typedef int32_t (__stdcall* HuEngineVerb)(const HuEngineHandle*, char* why, uint32_t why_size);
typedef int32_t (__stdcall* HuEngineNameVerb)(const HuEngineHandle*, const char* name,
                                              char* why, uint32_t why_size);
typedef int32_t (__stdcall* HuEngineNumberVerb)(const HuEngineHandle*, double value,
                                                char* why, uint32_t why_size);
typedef int32_t (__stdcall* HuEnginePlaceVerb)(const HuEngineHandle*, const char* name,
                                               double x, double y, char* why, uint32_t why_size);
typedef void (__stdcall* HuEngineRead)(const HuEngineHandle*, HuEngineReply*);
typedef void (__stdcall* HuEngineNamedRead)(const HuEngineHandle*, const char* name, HuEngineReply*);

// An engine abi 3 verb may also return -1 with `why` filled: the call was
// misused (a value of the wrong type), and the binding raises `why`.
typedef int32_t (__stdcall* HuEngineAnswer)(const HuEngineHandle*, const char* name, int32_t kind,
                                            double number, const char* text, char* why, uint32_t why_size);

// Engine abi 4's answer and cancel: `has_id` 1 when the caller passed the
// prompt's id from options(), in `id`; refused when another open of the
// window has come since.
typedef int32_t (__stdcall* HuEngineAnswer4)(const HuEngineHandle*, const char* name, int32_t kind,
                                             double number, const char* text, int32_t has_id, double id,
                                             char* why, uint32_t why_size);
typedef int32_t (__stdcall* HuEngineCancel4)(const HuEngineHandle*, const char* name, int32_t has_id, double id,
                                             char* why, uint32_t why_size);

// Engine abi 5's reset: `aspect` NULL or empty for the position and the
// size, else "position" or "size"; -1 for any other aspect.
typedef int32_t (__stdcall* HuEngineReset5)(const HuEngineHandle*, const char* name, const char* aspect,
                                            char* why, uint32_t why_size);

// Engine abi 7's block and unblock: `category` NULL or empty for the whole
// window, as block5 and unblock3 take it; else one list of the ability
// window, by name (job_abilities, pet_commands, weapon_skills, job_traits)
// or by number 1..31. A category on any other name is refused.
typedef int32_t (__stdcall* HuEngineCategoryVerb)(const HuEngineHandle*, const char* name, const char* category,
                                                  char* why, uint32_t why_size);

// The Lua type of answer()'s value: a number or boolean (1/0) in `number`,
// a string in `text`.
enum {
    HU_ANSWER_NONE = 0,
    HU_ANSWER_NUMBER = 1,
    HU_ANSWER_STRING = 2,
    HU_ANSWER_BOOLEAN = 3,
    HU_ANSWER_OTHER = 4
};

// Append-only, never reordered. Every call takes the resident's own lock;
// none calls into Lua. handle_new and handle_open make no handle (slot -1)
// unless the resident is installed; handle_open says why. shutdown returns
// 1, or 0 with `why` and `kind` (handles, drain, busy, election).
typedef struct HuEngineApi {
    uint32_t abi_version;
    uint32_t size;
    void (__stdcall* handle_new)(const char* name, HuEngineHandle* out);
    void (__stdcall* handle_release)(const HuEngineHandle*);
    HuEngineNameVerb hide;
    HuEngineNameVerb unhide;
    HuEngineNameVerb block;
    HuEngineNameVerb unblock;
    HuEnginePlaceVerb move;
    HuEnginePlaceVerb move_group;
    HuEngineNameVerb reset;
    HuEngineVerb reset_all;
    HuEngineNameVerb open;
    HuEngineNameVerb close;
    int32_t (__stdcall* resize)(const HuEngineHandle*, const char* name, const char* size,
                                char* why, uint32_t why_size);
    HuEngineNumberVerb query_answer;
    HuEngineVerb query_cancel;
    HuEngineNameVerb passinpu_submit;   /* the text */
    HuEngineVerb passinpu_cancel;
    HuEngineVerb prtyjoin_accept;
    HuEngineVerb prtyjoin_decline;
    HuEngineNameVerb post_close;
    HuEngineNumberVerb link5_confirm;   /* the slot */
    HuEngineVerb link5_cancel;
    HuEngineNumberVerb arealist_confirm;    /* the zone id */
    HuEngineVerb arealist_cancel;
    int32_t (__stdcall* poll)(const HuEngineHandle*, HuEngineEvent* out, uint32_t capacity,
                              uint32_t* count, uint32_t* dropped, char* why, uint32_t why_size);
    void (__stdcall* info)(const HuEngineHandle*, const char* name, HuEngineReply*);
    HuEngineRead positions;
    HuEngineRead list;
    HuEngineRead groups;
    void (__stdcall* status)(HuEngineReply*);
    void (__stdcall* version)(char* out, uint32_t size);
    int32_t (__stdcall* shutdown)(char* why, uint32_t why_size, char* kind, uint32_t kind_size);
    HuEngineRead query_options;         /* engine abi 2 */

    /* engine abi 3, the 0.5.0 API: group names are refused except by
       move_group3 and groups3, move_group3 places the anchor at x,y, and the
       reads have 0.5.0's shapes. The earlier slots keep their meaning for
       the copies built against them. */
    void (__stdcall* handle_open)(const char* name, HuEngineHandle* out, char* why, uint32_t why_size);
    HuEngineNameVerb hide3;
    HuEngineNameVerb unhide3;
    HuEngineNameVerb block3;
    HuEngineNameVerb unblock3;
    HuEngineNameVerb open3;
    HuEngineNameVerb close3;
    HuEngineNameVerb reset3;
    HuEnginePlaceVerb move3;
    HuEnginePlaceVerb move_group3;
    int32_t (__stdcall* resize3)(const HuEngineHandle*, const char* name, const char* size,
                                 char* why, uint32_t why_size);
    HuEngineAnswer answer;
    HuEngineNameVerb cancel;
    int32_t (__stdcall* poll3)(const HuEngineHandle*, HuEngineEvent3* out, uint32_t capacity,
                               uint32_t* count, uint32_t* dropped, char* why, uint32_t why_size);
    HuEngineNamedRead info3;
    HuEngineNamedRead options;
    HuEngineRead list3;
    HuEngineRead opened;
    HuEngineRead focused;
    HuEngineRead positions3;
    HuEngineRead groups3;
    HuEngineRead layout;
    HuEngineRead pending;
    void (__stdcall* status3)(HuEngineReply*);

    /* engine abi 4, the 0.5.1 API: a group move whose anchor is closed waits
       for the game to open it; close refuses the prompt windows; answer and
       cancel take the prompt's id, and cancel ends the incoming box through
       the live box window; poll adds the pending event; the reads have
       0.5.1's shapes. */
    HuEnginePlaceVerb move_group4;
    HuEngineNameVerb close4;
    HuEngineAnswer4 answer4;
    HuEngineCancel4 cancel4;
    int32_t (__stdcall* poll4)(const HuEngineHandle*, HuEngineEvent4* out, uint32_t capacity,
                               uint32_t* count, uint32_t* dropped, char* why, uint32_t why_size);
    HuEngineNamedRead info4;
    HuEngineNamedRead options4;
    HuEngineRead list4;
    HuEngineRead focused4;
    HuEngineRead groups4;
    HuEngineRead pending4;
    void (__stdcall* status4)(HuEngineReply*);

    /* engine abi 5, the 0.7.0 API: move and move_group take the frame's
       top-left; reset and reset_group reset only what the handle placed;
       the replies to prtyjoin and the post boxes name the session they
       answer; poll is a reply, the events in a table and the count lost
       after it, and adds cursor, blocked's holders and the error a waiting
       group move replaced by another handle's gets; options and pending
       name their prompt; rects; block closes the window when it is open. */
    HuEnginePlaceVerb move5;
    HuEnginePlaceVerb move_group5;
    HuEngineReset5 reset5;
    HuEngineNameVerb reset_group5;
    HuEngineAnswer4 answer5;
    HuEngineCancel4 cancel5;
    HuEngineRead poll5;
    HuEngineNamedRead options5;
    HuEngineRead pending5;
    HuEngineRead rects5;
    HuEngineNameVerb block5;

    /* engine abi 6, the 0.9.0 API: the macro keys. block_macros holds the
       game's Ctrl+number and Alt+number macros off for the handle and
       closes a macro bar that is up, unblock_macros drops the hold, and
       macros reads whether they are blocked and by whom; status gains
       macros_blocked. */
    HuEngineVerb block_macros;
    HuEngineVerb unblock_macros;
    HuEngineRead macros;

    /* engine abi 7, the 0.10.0 API: one list of the ability window. block7
       with a category holds that list off for the handle and closes the
       window when it shows that list now; unblock7 with one drops that
       hold alone. Without a category they are block5 and unblock3. poll5's
       events on ability carry category and category_name, and info of
       ability lists blocked_categories. */
    HuEngineCategoryVerb block7;
    HuEngineCategoryVerb unblock7;
} HuEngineApi;

// What the resident publishes into the pid-scoped mapping. `module` is its
// image base, so a copy knows itself and a forwarder can check the resident
// is still the image that published. The api travels by value and comes
// last, so appending to it moves nothing an older reader depends on.
typedef struct HuEngineRecord {
    uint32_t magic;
    uint32_t record_size;
    uint32_t abi_version;
    const void* module;
    char resident_path[MAX_PATH];       /* UTF-8 */
    char resident_build[HU_BUILD_MAX];
    HuEngineApi api;
} HuEngineRecord;

#pragma pack(pop)

HU_STATIC_ASSERT(sizeof(HuEngineHandle) == 8, "HuEngineHandle size is ABI");
HU_STATIC_ASSERT(sizeof(HuEngineEvent) == 32, "HuEngineEvent size is ABI");
HU_STATIC_ASSERT(offsetof(HuEngineEvent, name) == 12, "HuEngineEvent layout is ABI");
HU_STATIC_ASSERT(offsetof(HuEngineEvent, fresh) == 28, "HuEngineEvent layout is ABI");
HU_STATIC_ASSERT(sizeof(HuEngineEvent3) == 256, "HuEngineEvent3 size is ABI");
HU_STATIC_ASSERT(offsetof(HuEngineEvent3, verb) == 28, "HuEngineEvent3 layout is ABI");
HU_STATIC_ASSERT(offsetof(HuEngineEvent3, reason) == 44, "HuEngineEvent3 layout is ABI");
HU_STATIC_ASSERT(sizeof(HuEngineEvent4) == 272, "HuEngineEvent4 size is ABI");
HU_STATIC_ASSERT(offsetof(HuEngineEvent4, reason) == 44, "HuEngineEvent4 layout is ABI");
HU_STATIC_ASSERT(offsetof(HuEngineEvent4, what) == 256, "HuEngineEvent4 layout is ABI");
HU_STATIC_ASSERT(offsetof(HuEngineEvent4, pending) == 268, "HuEngineEvent4 layout is ABI");
HU_STATIC_ASSERT(sizeof(HuEngineReply) == 16, "HuEngineReply size is ABI");
HU_STATIC_ASSERT(offsetof(HuEngineReply, used) == 12, "HuEngineReply layout is ABI");

HU_STATIC_ASSERT(sizeof(HuEngineApi) == 348, "HuEngineApi size is ABI");
HU_STATIC_ASSERT(offsetof(HuEngineApi, handle_new) == 8, "HuEngineApi layout is ABI");
HU_STATIC_ASSERT(offsetof(HuEngineApi, hide) == 16, "HuEngineApi layout is ABI");
HU_STATIC_ASSERT(offsetof(HuEngineApi, move) == 32, "HuEngineApi layout is ABI");
HU_STATIC_ASSERT(offsetof(HuEngineApi, resize) == 56, "HuEngineApi layout is ABI");
HU_STATIC_ASSERT(offsetof(HuEngineApi, arealist_cancel) == 100, "HuEngineApi layout is ABI");
HU_STATIC_ASSERT(offsetof(HuEngineApi, poll) == 104, "HuEngineApi layout is ABI");
HU_STATIC_ASSERT(offsetof(HuEngineApi, status) == 124, "HuEngineApi layout is ABI");
HU_STATIC_ASSERT(offsetof(HuEngineApi, shutdown) == 132, "HuEngineApi layout is ABI");
HU_STATIC_ASSERT(offsetof(HuEngineApi, query_options) == 136, "HuEngineApi layout is ABI");
HU_STATIC_ASSERT(offsetof(HuEngineApi, handle_open) == 140, "HuEngineApi layout is ABI");
HU_STATIC_ASSERT(offsetof(HuEngineApi, answer) == 184, "HuEngineApi layout is ABI");
HU_STATIC_ASSERT(offsetof(HuEngineApi, poll3) == 192, "HuEngineApi layout is ABI");
HU_STATIC_ASSERT(offsetof(HuEngineApi, status3) == 232, "HuEngineApi layout is ABI");
HU_STATIC_ASSERT(offsetof(HuEngineApi, move_group4) == 236, "HuEngineApi layout is ABI");
HU_STATIC_ASSERT(offsetof(HuEngineApi, poll4) == 252, "HuEngineApi layout is ABI");
HU_STATIC_ASSERT(offsetof(HuEngineApi, status4) == 280, "HuEngineApi layout is ABI");
HU_STATIC_ASSERT(offsetof(HuEngineApi, move5) == 284, "HuEngineApi layout is ABI");
HU_STATIC_ASSERT(offsetof(HuEngineApi, poll5) == 308, "HuEngineApi layout is ABI");
HU_STATIC_ASSERT(offsetof(HuEngineApi, rects5) == 320, "HuEngineApi layout is ABI");
HU_STATIC_ASSERT(offsetof(HuEngineApi, block5) == 324, "HuEngineApi layout is ABI");
HU_STATIC_ASSERT(offsetof(HuEngineApi, block_macros) == 328, "HuEngineApi layout is ABI");
HU_STATIC_ASSERT(offsetof(HuEngineApi, unblock_macros) == 332, "HuEngineApi layout is ABI");
HU_STATIC_ASSERT(offsetof(HuEngineApi, macros) == 336, "HuEngineApi layout is ABI");
HU_STATIC_ASSERT(offsetof(HuEngineApi, block7) == 340, "HuEngineApi layout is ABI");
HU_STATIC_ASSERT(offsetof(HuEngineApi, unblock7) == 344, "HuEngineApi layout is ABI");

HU_STATIC_ASSERT(offsetof(HuEngineRecord, module) == 12, "HuEngineRecord layout is ABI");
HU_STATIC_ASSERT(offsetof(HuEngineRecord, resident_path) == 16, "HuEngineRecord layout is ABI");
HU_STATIC_ASSERT(offsetof(HuEngineRecord, resident_build) == 276, "HuEngineRecord layout is ABI");
HU_STATIC_ASSERT(offsetof(HuEngineRecord, api) == 340, "the api comes last so appending to it moves nothing");
HU_STATIC_ASSERT(sizeof(HuEngineRecord) == 688, "HuEngineRecord size is ABI");
HU_STATIC_ASSERT(sizeof(HU_ENGINE_BUILD) <= HU_BUILD_MAX, "the build string must fit the record");
HU_STATIC_ASSERT(sizeof(HuEngineRecord) <= HU_ENGINE_MAPPING_BYTES, "the record must fit the fixed section");
HU_STATIC_ASSERT(sizeof(HU_ENGINE_MAPPING_NAME_FORMAT) + 8 <= HU_NAME_MAX, "the mapping name must fit for every pid");
HU_STATIC_ASSERT(sizeof(HU_ENGINE_MUTEX_NAME_FORMAT) + 8 <= HU_NAME_MAX, "the mutex name must fit for every pid");

#endif  // HIDEUI_ENGINE_ABI_H_
