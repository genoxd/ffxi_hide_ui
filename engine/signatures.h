// signatures.h - how the engine finds FFXiMain's routines and data, the
// routines it calls, and the nine routines it hooks.

#ifndef HIDEUI_SIGNATURES_H_
#define HIDEUI_SIGNATURES_H_

#include "game.h"

namespace hu {

// The 34 signatures the engine scans for; build.sh checks the count. Not one
// verbatim.
const char kSigRegistry[] =
    "a0 ?? ?? ?? ?? 53 56 57 33 ff 84 c0 74 ?? 8b 5c 24 ?? b8 ?? ?? ?? ?? 8b f0 6a 10 53 50 e8 ?? ?? ?? ?? 83 c4 0c 85 c0 74 ?? 8a 4e 2c 83 c6 2c";
const char kSigManager[] =
    "8d 44 24 0c b9 ?? ?? ?? ?? 50 e8 ?? ?? ?? ?? 8b f0 85 f6 74 ?? 38 5e 60 75 ?? 8b cf e8 ?? ?? ?? ?? 84 c0 74 ?? f6 46 34 40";
const char kSigSetPosition[] =
    "83 ec 0c 66 8b 44 24 14 53 55 8b 6c 24 18 56 8b f1 57 33 ff 8d 5e 14 66 89 6e 52 8b cb 66 89 46 54";
const char kSigCloseByName[] =
    "8b 44 24 04 83 ec 20 56 8b f1 8d 4c 24 04 50 51 8b ce e8 ?? ?? ?? ?? 8d 54 24 04 8b ce 52 e8 ?? ?? ?? ?? 85 c0 74 ?? 6a 01 50 8b ce e8";
const char kSigDockMasks[] =
    "8b 04 bd ?? ?? ?? ?? 8b 4e 34 85 c1 74 ?? 56 57 8b cd e8";
const char kSigOpen[] =
    "8b 44 24 04 83 ec 20 53 56 57 8b f9 8d 4c 24 0c 50 51 8b cf e8 ?? ?? ?? ?? 8d 54 24 30 8d 44 24 0c 52 50 8b cf e8";
const char kSigUpdate[] =
    "83 ec 14 53 55 56 57 6a ff 6a 04 68 8b 00 00 00 6a 3e 8b e9 e8 ?? ?? ?? ?? 83 c4 10 3c 01 75";
const char kSigStagedClose[] =
    "53 56 8b 74 24 0c 33 db 57 3b f3 8b f9 0f 84 ?? ?? ?? ?? 8b ce e8 ?? ?? ?? ?? 83 7e 10 0e 75";
const char kSigShow[] =
    "53 55 56 8b 74 24 10 32 db 57 85 f6 8b f9 0f 84 ?? ?? ?? ?? 8a 46 6e 84 c0 0f 85";
// The mouse mode picker, the whole routine; its mov ecx imm32 at +4 is the
// manager.
const char kSigMouseMode[] =
    "56 8b f1 b9 ?? ?? ?? ?? e8 ?? ?? ?? ?? 84 c0 74 06 c6 46 4d 02 5e c3 c6 46 4d 01 5e c3";
const size_t kMouseModeManagerImm = 4;

// The menu input sink (this = the menu; the code), hooked. The routing
// routine (this = the manager; the code) hands every key and gamepad code to
// the active menu through its one call to the sink at kRoutingSinkCall; it is
// never hooked or called, only resolved for that call's return address.
const char kSigMenuInput[] =
    "51 55 56 8b f1 57 83 7e 10 0e 75 09 5f 5e 33 c0 5d 59 c2 04 00 66 83 7e 56 00";
const char kSigMenuRouting[] =
    "56 8b f1 e8 ?? ?? ?? ?? 84 c0 75 3a 8b ce e8 ?? ?? ?? ?? 84 c0 74 20 8b ce e8 ?? ?? ?? ?? 84 c0 75 15 8b 44 24 08 8b 4e 54 50 e8";
const size_t kRoutingSinkCall = 0x2A;

// The compass draw entry (no arguments, plain ret), hooked: the compass
// through its global, its update, the manager's gate, then its render by a
// jump. The global is the imm32 at +2 and again at +31, which must agree;
// the manager's at +16. The first instruction is the prologue, so the engine
// reads the global out of the intact copy at +31 and pins it at +2 before
// the scan, as it pins the manager.
const char kSigCompassDraw[] =
    "8b 0d ?? ?? ?? ?? 85 c9 74 ?? e8 ?? ?? ?? ?? b9 ?? ?? ?? ?? e8 ?? ?? ?? ?? 3c 01 74 ?? 8b 0d ?? ?? ?? ?? e9 ?? ?? ?? ?? c3";
const size_t kCompassDrawGlobal = 2;
const size_t kCompassDrawGlobal2 = 31;
const size_t kCompassDrawManagerImm = 16;

// The macro key gate (no arguments, `this` unused, al 1 while the macro keys
// may act; plain ret), hooked: the macro key handler's one question before
// it opens a bar and before each number key, and the gate's only caller.
// The prologue, mov ax,[imm32], reads a word the engine knows from nowhere
// else, so the engine reads the imm32 out of the hit (on a patched site,
// out of the daemon's saved original) and pins it at +2 before the scan
// that resolves the site; the manager's imm32 at +40 is pinned as the
// mouse mode picker's.
const char kSigMacroGate[] =
    "66 a1 ?? ?? ?? ?? 56 33 f6 66 85 c0 74 ?? 0f bf c0 8b 34 85 ?? ?? ?? ?? 8b 0d ?? ?? ?? ?? 83 39 60 0f 85 ?? ?? ?? ?? b9 ?? ?? ?? ?? e8 ?? ?? ?? ?? 84 c0 0f 85";
const size_t kMacroGateGlobal = 2;
const size_t kMacroGateManagerImm = 40;

// The ability opener (kind, flag, extra; cdecl, the caller popping the 12
// bytes, nothing returned), hooked: the one routine every open of the job
// ability, pet command, weapon skill and job trait lists goes through, from
// the abilities menu rows and the /ja, /ws and /pet commands. It tests a
// player global, opens abisortw by name when flag is 1, opens ability by
// name, then hands kind to the controller, which stores it at
// kAbilityCategory (game.h). The prologue, mov eax,[imm32], reads a global
// the engine knows from nowhere else, so it is read out of the hit and
// pinned at +1 before the scan that resolves the site, as the macro gate's
// word is; the manager's imm32 at +29 is pinned as the mouse mode picker's.
// The second push's imm32, at +43, must point at the registry's ability
// key, checked once the site is found.
const char kSigAbilityOpen[] =
    "a1 ?? ?? ?? ?? 85 c0 74 ?? 53 8b 5c 24 0c 80 fb 01 75 ?? 6a 00 6a 01 68 ?? ?? ?? ?? b9 ?? ?? ?? ?? e8 ?? ?? ?? ?? 6a 00 6a 01 68 ?? ?? ?? ?? b9 ?? ?? ?? ?? e8";
const size_t kAbilityOpenGlobal = 1;
const size_t kAbilityOpenManagerImm = 29;
const size_t kAbilityOpenKeyImm = 43;

// The macro key object's global, read out of the macro subsystem's
// constructor, a data site never run or hooked: the global is the imm32 at
// +15 and again at +23, which must agree. The object is kMacroObjectBytes
// (game.h), the constructor's allocation; the global is NULL before the
// subsystem exists.
const char kSigMacroObject[] =
    "8b 0d ?? ?? ?? ?? 51 8b c8 e8 ?? ?? ?? ?? a3 ?? ?? ?? ?? eb 06 89 1d ?? ?? ?? ?? 6a 50 e8";
const size_t kMacroObjectImm = 15;
const size_t kMacroObjectImm2 = 23;

// The row hit test: never hooked or called. Resolved to check that it still
// takes a hit on a menu whose kMenuMouse byte is 0 for a miss (game.h), the
// read below, once, inside its first kHitTestBytes.
const char kSigRowHitTest[] =
    "83 ec 08 53 55 56 8b f1 8b 0d ?? ?? ?? ?? 57 89 74 24 10 d9 41 48 d8 1d ?? ?? ?? ?? df e0 25 00 41 00 00 0f 84 ?? ?? ?? ?? 8a 41 4e 84 c0 0f 84 ?? ?? ?? ?? 8d 4e 14 e8";
// mov esi,[esp+0x10] (this); mov cl,[esi+0x77]; test cl,cl; je
const uint8_t kHitTestMouseRead[] = {0x8b, 0x74, 0x24, 0x10, 0x8a, 0x4e, 0x77, 0x84, 0xc9, 0x74};
const size_t kHitTestMouseByte = 6;     // where in the read the menu offset sits
const size_t kHitTestBytes = 0x100;

// The routines the reply calls, reset and resize call; none is hooked.
// SetCursor and the glyph converter are resolved on their own, beside
// SetPosition.
const char kSigPassinpuReset[] =
    "33 c0 89 41 1c 89 41 20 88 41 24 89";
const char kSigPartySend[] =
    "a1 ?? ?? ?? ?? 85 c0 74 12 6a 00 6a 00 6a 74";
const char kSigPartyClear[] =
    "c6 05 ?? ?? ?? ?? 00 c3 90 90 90 90 90 90 90 90 8a";
const char kSigLink5Clear[] =
    "53 56 8b f1 33 db 88 1d ?? ?? ?? ??";
// arealist's close+reset (this = the controller): its close by name, whose
// begin-close clears the latch and the mode on an open instance, then the
// rows freed. The latch clear is that begin-close, called alone when no
// instance is open.
const char kSigArealistClose[] =
    "56 8b f1 8b 06 ff 50 74 6a 01 6a 00 6a 00 8b ce c7 46 34 00 00 00 00 c6 46 49 01 e8 ?? ?? ?? ?? 8b ce 5e e9 ?? ?? ?? ?? 90 90 90 90 90 90 90 90 a1 ?? ?? ?? ?? 55";
const char kSigArealistLatch[] =
    "32 c0 88 41 6d 88 41 6c c3 90 90 90";
const char kSigPostRequest[] =
    "8b 44 24 04 53 32 db 83 f8 0d 7c 25";
const char kSigPostRequestClose[] =
    "56 8b f1 e8 ?? ?? ?? ?? 84 c0 0f 84 ?? ?? ?? ?? 8b";
const char kSigDockReset[] =
    "8b 44 24 04 56 8b f1 8b 4c 24 0c 33";
const char kSigTemplateSwap[] =
    "83 ec 18 53 55 56 57 8b f9 89 7c 24";
const char kSigSetFrameRect[] =
    "83 ec 08 56 8b f1 8d 4e 14 e8 ?? ?? ?? ?? 85 c0 89 44 24 08";
const char kSigSetCursor[] =
    "83 ec 08 56 57 8b 7c 24 14 8b f1 66";

// The text-to-glyph converter: the engine calls it only on the game thread,
// once, to build the inverse of its two-byte characters (game.h).
const char kSigGlyphConvert[] =
    "55 8b 6c 24 0c 85 ed 75 04 33 c0 5d c3 8b 44 24 10 85 c0 75 02 5d c3 53 56 57 8d 48 ff 32 db 33 ff";

// Data read out of the code that uses it: link5's own confirm adds the
// concierge cache to slot * 0x20, query's input handler compares the
// cancel-allowed byte with 1.
const char kSigLink5Cache[] =
    "8b 4e 6c 83 c4 04 85 c9 74 17 83 f8 10 73 12 8b d0 c1 e2 05 81 c2 ?? ?? ?? ?? 52 50 ff d1";
const char kSigQueryCancelAllowed[] =
    "80 3d ?? ?? ?? ?? 01 75 ?? 66 c7 86 48 05 00 00 ff 00";
const size_t kLink5CacheImm = 22;
const size_t kQueryCancelImm = 2;

// The event's wait test reads the latch its callback clears; the 0x71 sub
// 0x40 handler opens link5 with the controller's global and that callback.
const char kSigLink5Latch[] =
    "8a 0d ?? ?? ?? ?? 33 c0 84 c9 0f 94 c0 c3";
const char kSigLink5Open[] =
    "e8 ?? ?? ?? ?? 6a 02 8b ce e8 ?? ?? ?? ?? 8b 0d ?? ?? ?? ?? 50 68 ?? ?? ?? ?? 6a 01 e8 ?? ?? ?? ?? e9 ?? ?? ?? ?? e8 ?? ?? ?? ?? 84 c0 0f 84";
const size_t kLink5LatchImm = 2;
const size_t kLink5OpenGlobal = 16;
const size_t kLink5OpenCallback = 22;

struct CallSpec {
    const char* name;
    const char* signature;
};

enum {
    kCallPassinpuReset,
    kCallPartySend,
    kCallPartyClear,
    kCallLink5Clear,
    kCallArealistClose,
    kCallArealistLatch,
    kCallPostRequest,
    kCallPostRequestClose,
    kCallDockReset,
    kCallTemplateSwap,
    kCallSetFrameRect,
    kCallCount
};

const CallSpec kCalls[kCallCount] = {
    {"passinpu_reset", kSigPassinpuReset},
    {"prtyjoin_send", kSigPartySend},
    {"prtyjoin_clear", kSigPartyClear},
    {"link5_clear", kSigLink5Clear},
    {"arealist_close", kSigArealistClose},
    {"arealist_latch", kSigArealistLatch},
    {"post_request", kSigPostRequest},
    {"post_request_close", kSigPostRequestClose},
    {"dock_reset", kSigDockReset},
    {"template_swap", kSigTemplateSwap},
    {"set_frame_rect", kSigSetFrameRect},
};

// The pending-invite flag is the byte prtyjoin_clear writes: its imm32.
const size_t kPartyClearFlag = 2;

const uint32_t kDockMaskTable[5] = {0x1000, 0x2000, 0x4000, 0x10000000, 0};

// The prologue is the run of whole, position-independent instructions the
// daemon relocates; each lies inside the exact bytes of its signature, but
// for `manager_imm`: an imm32 in it that must be the manager's address, which
// the engine writes into the signature before it scans. `callee_pops` is 1
// for a routine that pops its own arguments (thiscall, stdcall), 0 for one
// whose caller does (cdecl).
struct SiteSpec {
    const char* name;
    const char* signature;
    uint32_t prologue;
    uint32_t arg_bytes;
    uint32_t callee_pops;
    HuPreFn pre;
    HuPostFn post;
    uint32_t manager_imm;           // 0: none
};

enum {
    kSiteOpen, kSiteUpdate, kSiteStagedClose, kSiteShow, kSiteMouseMode, kSiteMenuInput, kSiteCompassDraw,
    kSiteMacroGate, kSiteAbilityOpen,
    kSiteCount
};

const SiteSpec kSites[kSiteCount] = {
    {"open_by_name", kSigOpen, 7, 12, 1, &hook_open_pre, &hook_open_post, 0},
    {"ui_update", kSigUpdate, 7, 0, 1, &hook_update_pre, NULL, 0},
    {"staged_close", kSigStagedClose, 9, 20, 1, &hook_close_pre, NULL, 0},
    {"show_path", kSigShow, 10, 4, 1, &hook_show_pre, NULL, 0},
    {"mouse_mode", kSigMouseMode, 8, 0, 1, &hook_mouse_mode_pre, NULL, kMouseModeManagerImm},
    {"menu_input", kSigMenuInput, 6, 4, 1, &hook_menu_input_pre, NULL, 0},
    {"compass_draw", kSigCompassDraw, 6, 0, 1, &hook_compass_pre, NULL, kCompassDrawManagerImm},
    {"macro_gate", kSigMacroGate, 6, 0, 1, &hook_macro_gate_pre, NULL, kMacroGateManagerImm},
    {"ability_open", kSigAbilityOpen, 5, 12, 0, &hook_ability_open_pre, &hook_ability_open_post,
     kAbilityOpenManagerImm},
};

}  // namespace hu

#endif  // HIDEUI_SIGNATURES_H_
