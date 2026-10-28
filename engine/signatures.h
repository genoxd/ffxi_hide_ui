// How the engine finds FFXiMain's routines and data.

#ifndef HIDEUI_SIGNATURES_H_
#define HIDEUI_SIGNATURES_H_

#include "game.h"

namespace hu {

// The 147 signatures the engine scans for; build.sh checks the count. Not one
// verbatim.
const char kSigRegistry[] =
    "a0 ?? ?? ?? ?? 53 56 57 33 ff 84 c0 74 ?? 8b 5c 24 ?? b8 ?? ?? ?? ?? 8b f0 6a 10 53 50 e8 ?? ?? ?? ?? 83 c4 0c 85 c0 74 ?? 8a 4e 2c 83 c6 2c";
const char kSigManager[] =
    "8d 44 24 0c b9 ?? ?? ?? ?? 50 e8 ?? ?? ?? ?? 8b f0 85 f6 74 ?? 38 5e 60 75 ?? 8b cf e8 ?? ?? ?? ?? 84 c0 74 ?? f6 46 34 40";
const char kSigSetPosition[] =
    "83 ec 0c 66 8b 44 24 14 53 55 8b 6c 24 18 56 8b f1 57 33 ff 8d 5e 14 66 89 6e 52 8b cb 66 89 46 54";
const char kSigCloseByName[] =
    "8b 44 24 04 83 ec 20 56 8b f1 8d 4c 24 04 50 51 8b ce e8 ?? ?? ?? ?? 8d 54 24 04 8b ce 52 e8 ?? ?? ?? ?? 85 c0 74 ?? 6a 01 50 8b ce e8";
// Close by name ends in its call of close by instance, which does nothing for
// a NULL menu.
const size_t kCloseByNameInstanceCall = 44;
const char kSigDockMasks[] =
    "8b 04 bd ?? ?? ?? ?? 8b 4e 34 85 c1 74 ?? 56 57 8b cd e8";
const char kSigOpen[] =
    "8b 44 24 04 83 ec 20 53 56 57 8b f9 8d 4c 24 0c 50 51 8b cf e8 ?? ?? ?? ?? 8d 54 24 30 8d 44 24 0c 52 50 8b cf e8";
const char kSigUpdate[] =
    "83 ec 14 53 55 56 57 6a ff 6a 04 68 8b 00 00 00 6a 3e 8b e9 e8 ?? ?? ?? ?? 83 c4 10 3c 01 75";
const char kSigStagedClose[] =
    "53 56 8b 74 24 0c 33 db 57 3b f3 8b f9 0f 84 ?? ?? ?? ?? 8b ce e8 ?? ?? ?? ?? 83 7e 10 0e 75";
// The show path's gate returns at once for a NULL menu, the closing byte
// +0x6E set, no registry entry at +0x04, or a state at +0x10 other than 0x0E;
// hook_show_pre tests the same.
const char kSigShow[] =
    "53 55 56 8b 74 24 10 32 db 57 85 f6 8b f9 0f 84 ?? ?? ?? ?? 8a 46 6e 84 c0 0f 85 ?? ?? ?? ?? 8b 46 04 85 c0 0f 84 ?? ?? ?? ?? 8b 46 10 bd 0e 00 00 00 3b c5 0f 85";
// The mouse mode picker, the whole routine.
const char kSigMouseMode[] =
    "56 8b f1 b9 ?? ?? ?? ?? e8 ?? ?? ?? ?? 84 c0 74 06 c6 46 4d 02 5e c3 c6 46 4d 01 5e c3";
const size_t kMouseModeManagerImm = 4;

// The menu input sink (this = the menu; the code), hooked. The routing
// routine hands every key and gamepad code to the active menu through its one
// call to the sink at kRoutingSinkCall, and is resolved only for that call's
// return address.
const char kSigMenuInput[] =
    "51 55 56 8b f1 57 83 7e 10 0e 75 09 5f 5e 33 c0 5d 59 c2 04 00 66 83 7e 56 00";
const char kSigMenuRouting[] =
    "56 8b f1 e8 ?? ?? ?? ?? 84 c0 75 3a 8b ce e8 ?? ?? ?? ?? 84 c0 74 20 8b ce e8 ?? ?? ?? ?? 84 c0 75 15 8b 44 24 08 8b 4e 54 50 e8";
const size_t kRoutingSinkCall = 0x2A;

// The compass draw entry (no arguments, plain ret), hooked. Its global is the
// imm32 at +2 and again at +31, which must agree; a hook's patch may cover
// +2, so the engine reads +31 and pins +2 before the scan.
const char kSigCompassDraw[] =
    "8b 0d ?? ?? ?? ?? 85 c9 74 ?? e8 ?? ?? ?? ?? b9 ?? ?? ?? ?? e8 ?? ?? ?? ?? 3c 01 74 ?? 8b 0d ?? ?? ?? ?? e9 ?? ?? ?? ?? c3";
const size_t kCompassDrawGlobal = 2;
const size_t kCompassDrawGlobal2 = 31;
const size_t kCompassDrawManagerImm = 16;

// The macro key gate (no arguments, al 1 while the macro keys may act),
// hooked: the macro key handler, its only caller, asks it before it opens a
// bar and before each number key. The prologue's imm32 (+2) is a word known
// from nowhere else, so it is read out of the hit (on a patched site, the
// daemon's saved original) and pinned before the scan that resolves the site.
const char kSigMacroGate[] =
    "66 a1 ?? ?? ?? ?? 56 33 f6 66 85 c0 74 ?? 0f bf c0 8b 34 85 ?? ?? ?? ?? 8b 0d ?? ?? ?? ?? 83 39 60 0f 85 ?? ?? ?? ?? b9 ?? ?? ?? ?? e8 ?? ?? ?? ?? 84 c0 0f 85";
const size_t kMacroGateGlobal = 2;
const size_t kMacroGateManagerImm = 40;

// The ability opener (kind, flag, extra; cdecl), hooked: every open of the
// ability lists goes through it. It opens abisortw when flag is 1, then
// ability, and hands kind to the controller (game.h's kAbilityCategory). Its
// prologue's global (+1) is read out of the hit and pinned as the macro
// gate's word is.
const char kSigAbilityOpen[] =
    "a1 ?? ?? ?? ?? 85 c0 74 ?? 53 8b 5c 24 0c 80 fb 01 75 ?? 6a 00 6a 01 68 ?? ?? ?? ?? b9 ?? ?? ?? ?? e8 ?? ?? ?? ?? 6a 00 6a 01 68 ?? ?? ?? ?? b9 ?? ?? ?? ?? e8";
const size_t kAbilityOpenGlobal = 1;
const size_t kAbilityOpenManagerImm = 29;
const size_t kAbilityOpenKeyImm = 43;

// The magic list's opener (category, sort, exclusion mask; cdecl), called,
// never hooked. The controller's rebuild opens subwindo, and with sort 1
// mgcsortw, or blusortw for blue magic.
const char kSigMagicOpen[] =
    "a1 ?? ?? ?? ?? 85 c0 74 2d 6a 00 6a 01 68 ?? ?? ?? ?? b9 ?? ?? ?? ?? e8 ?? ?? ?? ?? 8b 44 24 0c 8b 4c 24 08 8b 54 24 04 50 51 8b 0d ?? ?? ?? ?? 52 e8 ?? ?? ?? ?? c3";
const size_t kMagicOpenKeyImm = 0x0E;
const size_t kMagicOpenManagerImm = 0x13;
const size_t kMagicOpenCall = 0x17;
const size_t kMagicOpenSlot = 0x2C;

// The equipment set's opener (this = the controller in equip's registry
// slot), called, never hooked. It returns early after three tests that print
// the game's refusal, and at kEquipOpenGuard while that controller is NULL or
// its word +0x1A is not 0.
const char kSigEquipOpen[] =
    "e8 ?? ?? ?? ?? 84 c0 74 0e 8b 0d ?? ?? ?? ?? 6a 4e e8 ?? ?? ?? ?? c3 e8 ?? ?? ?? ?? 84 c0 74 0e 8b 0d ?? ?? ?? ?? 6a 4d e8 ?? ?? ?? ?? c3 6a 00 e8 ?? ?? ?? ?? 83 c4 04 84 c0 75 11";
const size_t kEquipOpenGuard = 0x4D;
const size_t kEquipOpenGuardWordAt = 0x5A;
const uint8_t kEquipOpenGuardWord[] = {0x66, 0x83, 0x78, 0x1a, 0x00};     // cmp word [eax+0x1A],0
const int kEquipOpenCount = 3;
const size_t kEquipOpenKeys[kEquipOpenCount] = {0x134, 0x14B, 0x17C};
const size_t kEquipOpenCalls[kEquipOpenCount] = {0x142, 0x155, 0x186};
const char* const kEquipOpenWindows[kEquipOpenCount] = {"persona", "money", "equip"};
const size_t kEquipOpenManager = 5;

// The equipment shortcut's handler, never run: it runs the set's opener only
// when the Monstrosity check (kShapeMonstrosity, al 1 in Monstrosity) answers
// no. open_set('equipment') asks the check first, as the shortcut does.
const char kSigEquipShortcut[] =
    "e8 ?? ?? ?? ?? 84 c0 75 0b 8b 0d ?? ?? ?? ?? e8 ?? ?? ?? ?? b8 01 00 00 00 c3";
const size_t kEquipShortcutCheck = 0;
const size_t kEquipShortcutSlot = 11;
const size_t kEquipShortcutOpen = 15;
const char kShapeMonstrosity[] =
    "a1 ?? ?? ?? ?? 85 c0 74 0d 66 83 b8 ?? ?? ?? ?? 00 74 03 b0 01 c3 80 3d ?? ?? ?? ?? 17 0f 94 c0 c3";

// The map's opener (mode; cdecl), called with mode 0 as the main menu's map
// row and the map command run it.
const char kSigMapOpen[] =
    "a1 ?? ?? ?? ?? 83 ec 10 85 c0 53 56 57 0f 84 ?? ?? ?? ?? 8b 5c 24 20 bf 02 00 00 00 83 fb 01 74 05 83 fb 04 75 05 bf 01 00 00 00 b9";
const size_t kMapOpenFrameAt = 0x77;
const char kShapeMapFrame[] =
    "a1 ?? ?? ?? ?? 85 c0 74 3a 8b 44 24 14 8b 4c 24 10 8b 54 24 0c 6a 01 6a 00 6a 00 57 53 50 51 8b 0d ?? ?? ?? ?? 52"
    " e8 ?? ?? ?? ?? 50 a1 ?? ?? ?? ?? 8b 88 ?? ?? ?? ?? 51 8b 0d ?? ?? ?? ?? e8 ?? ?? ?? ??";
const size_t kMapRoutineOpensAt = 0x791;
const char kShapeMapOpens[] =
    "53 6a 01 68 ?? ?? ?? ?? b9 ?? ?? ?? ?? e8 ?? ?? ?? ?? 53 6a 01 68 ?? ?? ?? ?? b9 ?? ?? ?? ?? e8 ?? ?? ?? ??";

// The blue magic set's and the automaton's openers (this = the controller in
// bluequip's or automato's registry slot), called as the magic menu's set row
// and the automaton menu's attachment row run them.
const char kSigSetSpellsOpen[] =
    "a0 ?? ?? ?? ?? 56 a8 01 8b f1 0f 84 ?? ?? ?? ?? a0 ?? ?? ?? ?? 8a 0d ?? ?? ?? ?? 3c 10 74 0d 80 f9 10 0f 85 ?? ?? ?? ?? 3a c1 75 1d c6 46 5c 00";
const size_t kSetSpellsOpensAt = 0x78;
const char kShapeSetSpellsOpens[] =
    "8b 0d ?? ?? ?? ?? 6a 00 6a 01 c6 41 1c 02 68 ?? ?? ?? ?? b9 ?? ?? ?? ?? e8 ?? ?? ?? ?? 8b 0d ?? ?? ?? ?? 6a 00 6a 00"
    " 68 19 fc ff ff 68 19 fc ff ff e8 ?? ?? ?? ?? 8b 15 ?? ?? ?? ?? 8b 0d ?? ?? ?? ?? 68 81 00 00 00 68 19 fc ff ff 68"
    " 19 fc ff ff 52 e8 ?? ?? ?? ?? 6a 00 6a 01 68 ?? ?? ?? ?? b9 ?? ?? ?? ?? e8 ?? ?? ?? ?? 6a 00 6a 01 68 ?? ?? ?? ??"
    " b9 ?? ?? ?? ?? e8 ?? ?? ?? ?? 5e c3";
const char kSigAttachmentsOpen[] =
    "f6 05 ?? ?? ?? ?? 01 74 7e 8a 15 ?? ?? ?? ?? b0 12 3a d0 74 6c 38 05 ?? ?? ?? ?? 75 6a c6 41 1c 01 8b 0d ?? ?? ?? ?? 6a 00 6a 00 68 19 fc ff ff 68 19 fc ff ff e8";
const size_t kAttachmentsOpensAt = 0x21;
const char kShapeAttachmentsOpens[] =
    "8b 0d ?? ?? ?? ?? 6a 00 6a 00 68 19 fc ff ff 68 19 fc ff ff e8 ?? ?? ?? ?? a1 ?? ?? ?? ?? 8b 0d ?? ?? ?? ?? 68 81"
    " 00 00 00 68 19 fc ff ff 68 19 fc ff ff 50 e8 ?? ?? ?? ?? 6a 00 6a 01 68 ?? ?? ?? ?? b9 ?? ?? ?? ?? e8 ?? ?? ?? ??"
    " 6a 00 6a 01 68 ?? ?? ?? ?? b9 ?? ?? ?? ?? e8 ?? ?? ?? ?? c3";

// The main menu's linkshell row, never run: its check (1, 1; al 1 refuses),
// then the linkshell list's opener (mode and two more, 0 each), which
// tail-jumps to the open routine with link5's key written over its arguments.
const char kSigLinkshellRow[] =
    "6a 01 6a 01 e8 ?? ?? ?? ?? 83 c4 08 84 c0 74 0e e8 ?? ?? ?? ?? e8 ?? ?? ?? ?? 5e c2 08 00 8b 0d ?? ?? ?? ?? 6a 00 6a 00 6a 00 e8";
const size_t kLinkshellCheckCall = 4;
const size_t kLinkshellSlot = 0x20;
const size_t kLinkshellOpenCall = 0x2A;
const size_t kLinkshellTailAt = 0xCF;
const char kShapeLinkshellTail[] =
    "5f 5e c7 44 24 0c 00 00 00 00 c7 44 24 08 01 00 00 00 c7 44 24 04 ?? ?? ?? ?? b9 ?? ?? ?? ?? e9 ?? ?? ?? ??";

// The Monstrosity instincts' opener (cdecl), called as the main menu's
// equipment row runs it in Monstrosity.
const char kSigInstinctsOpen[] =
    "56 be f8 ef 00 00 8b 0d ?? ?? ?? ?? 56 e8 ?? ?? ?? ?? 46 81 fe ff ef 00 00 72 eb e8";
const char kShapeInstinctsOpen[] =
    "56 be f8 ef 00 00 8b 0d ?? ?? ?? ?? 56 e8 ?? ?? ?? ?? 46 81 fe ff ef 00 00 72 eb e8 ?? ?? ?? ?? 8b 0d ?? ?? ?? ?? 6a"
    " 01 e8 ?? ?? ?? ?? 8b 0d ?? ?? ?? ?? e8 ?? ?? ?? ?? 6a 00 6a 01 68 ?? ?? ?? ?? b9 ?? ?? ?? ?? e8 ?? ?? ?? ?? 5e c3";
const char kShapeMpStatOpen[] =
    "8b 44 24 04 6a 00 89 41 14 6a 01 68 ?? ?? ?? ?? b9 ?? ?? ?? ?? e8 ?? ?? ?? ?? c2 04 00";
const char kShapeMpCstmOpen[] =
    "6a 00 6a 01 68 ?? ?? ?? ?? b9 ?? ?? ?? ?? e8 ?? ?? ?? ?? c3";

// The item menu's use of the item under the cursor, never run: the maze
// editor's opener for a tabula (mode 0 or 1), and for a crystal a check
// (0x77, 0; al 0 refuses), then the synthesis's opener (al 0 refuses). Hole
// 3: the item data's offset of its pointer to the type word.
const char kSigItemUse[] =
    "8b 0d ?? ?? ?? ?? 33 c0 66 8b 06 50 e8 ?? ?? ?? ?? 85 c0 0f 84 ?? ?? ?? ?? 8b 88 ?? ?? ?? ?? 66 8b 09 66 83 f9 16 75 18 8b 0d ?? ?? ?? ?? 33 d2 8a 56 02 52 6a 00 e8 ?? ?? ?? ??";
const char kShapeItemUse[] =
    "8b 0d ?? ?? ?? ?? 33 c0 66 8b 06 50 e8 ?? ?? ?? ?? 85 c0 0f 84 ?? ?? ?? ?? 8b 88 ?? ?? ?? ?? 66 8b 09 66 83 f9 16"
    " 75 18 8b 0d ?? ?? ?? ?? 33 d2 8a 56 02 52 6a 00 e8 ?? ?? ?? ?? e9 ?? ?? ?? ?? 66 83 f9 17 75 18 8b 0d ?? ?? ?? ??"
    " 33 c0 8a 46 02 50 6a 01 e8 ?? ?? ?? ?? e9 ?? ?? ?? ?? 66 83 f9 08 75 3d 6a 00 6a 77 e8 ?? ?? ?? ?? 83 c4 08 84 c0"
    " 75 0a e8 ?? ?? ?? ?? 5f 5e c2 08 00 66 8b 0e 8a 56 02 6a 00 51 8b 0d ?? ?? ?? ?? 52 e8 ?? ?? ?? ??";
const size_t kSynthesisOpensAt = 0xA9;
const char kShapeSynthesisOpens[] =
    "6a 00 6a 01 68 ?? ?? ?? ?? b9 ?? ?? ?? ?? e8 ?? ?? ?? ??";
// The synthesis's opener keeps inventor's list (hole 1: its offset in
// inventor's controller) for its cancel to reopen inventor with.
const size_t kSynthesisKeepsAt = 0x4F;
const char kShapeSynthesisKeeps[] =
    "8b 15 ?? ?? ?? ?? 8b 42 ?? 89 46 4c";
const size_t kTabulaSetupAt = 0x3A;
const char kShapeTabulaSetup[] =
    "50 8b 44 24 2c 50 e8 ?? ?? ?? ?? 5f 5e c2 08 00";
const size_t kTabulaOpensAt = 0x5F;
const char kShapeTabulaOpens[] =
    "6a 01 68 ?? ?? ?? ?? b9 ?? ?? ?? ?? e8 ?? ?? ?? ??";

// The macro set menu's input routine, never run: its edit row (the jump
// table at hole 4) selects the set (hole 5) and runs the editor's opener
// (hole 7, on the editor object in the global at hole 6), which opens
// mcrbedit and the two palettes, the one last used last.
const char kSigMacroEdit[] =
    "57 8b f9 8b 0d ?? ?? ?? ?? 85 c9 0f 84 ?? ?? ?? ?? 66 83 7c 24 08 05 0f 85 ?? ?? ?? ?? 8b 51 1c 33 c0 8a 41 6c 56 8d 04 80 8d 34 82 0f bf 44 24 10 48 83 f8 04 0f 87";
const char kShapeMacroEdit[] =
    "57 8b f9 8b 0d ?? ?? ?? ?? 85 c9 0f 84 ?? ?? ?? ?? 66 83 7c 24 08 05 0f 85 ?? ?? ?? ?? 8b 51 1c 33 c0 8a 41 6c 56"
    " 8d 04 80 8d 34 82 0f bf 44 24 10 48 83 f8 04 0f 87 ?? ?? ?? ?? ff 24 85 ?? ?? ?? ?? 8b 4f 14 56 e8 ?? ?? ?? ?? 8b"
    " 0d ?? ?? ?? ?? e8 ?? ?? ?? ??";
const size_t kMacroEditRow = 0x42;
const char kShapeMacroEditorOpens[] =
    "8a 41 0c 6a 00 6a 01 3c 01 b9 ?? ?? ?? ?? 68 ?? ?? ?? ?? 75 2c e8 ?? ?? ?? ?? 6a 00 6a 01 68 ?? ?? ?? ?? b9 ?? ?? ??"
    " ?? e8 ?? ?? ?? ?? 6a 00 6a 01 68 ?? ?? ?? ?? b9 ?? ?? ?? ?? e8 ?? ?? ?? ?? c3 e8 ?? ?? ?? ?? 6a 00 6a 01 68 ?? ??"
    " ?? ?? b9 ?? ?? ?? ?? e8 ?? ?? ?? ?? 6a 00 6a 01 68 ?? ?? ?? ?? b9 ?? ?? ?? ?? e8 ?? ?? ?? ?? c3";

// The template swap (this = the menu; the key), hooked and called by the
// resize: the resizable windows' owners run it as their rows change, and it
// puts the frame on the new template's default spot. Its prologue is 5 bytes.
const char kSigTemplateSwap[] =
    "83 ec 18 53 55 56 57 8b f9 89 7c 24";

// Hooked to keep a hidden window and the target arrows from painting: the
// per-menu draw (this = the menu), the sprite draw (this = the sprite, eight
// arguments, ret 0x20) and the draw primitive (this = the renderer;
// DrawPrimitiveUP's arguments, ret 0x10). Prologues of 5, 5 and 7 bytes. The
// quad routine is found only for its call of the draw primitive at
// kQuadPrimitiveCall; another hook may have patched its first five bytes.
const char kSigMenuDraw[] =
    "83 ec 24 53 55 56 57 8b e9 e8 ?? ?? ?? ?? 66 8b 45 56 33 ff 66 3b c7 7e 05";
const char kSigSpriteDraw[] =
    "83 ec 0c 53 55 56 8b f1 57 8b 6e 14 8d 4e 04 89 6c 24 18 89 4c 24 14";
const char kSigPrimitiveDraw[] =
    "83 ec 08 8d 44 24 00 56 8b f1 8d 4c 24 08 50 51 8b ce e8 ?? ?? ?? ?? 84 c0 0f 84 ?? ?? ?? ?? 53 55 57 8b be 10 08 00 00 8b ce c7 86 10 08 00 00 00 00 00 00 e8 ?? ?? ?? ?? 8b 5c 24 28";
const char kSigQuadDraw[] =
    "8b 44 24 08 83 ec 10 53 32 db 85 c0 a1 ?? ?? ?? ?? 56 57 8b f9 8b 40 0c 8b 08 0f 84 ?? ?? ?? ??";
const size_t kQuadPrimitiveCall = 0x231;

// The target window's sprite draw calls of its two arrows: the one it always
// draws, and the one while the player picks a command's target. Each call's
// return address tells its arrow from every other sprite.
const char kSigTargetArrow[] =
    "8b 4c 8e 78 68 00 00 80 3f 68 00 00 80 3f 52 50 e8 ?? ?? ?? ?? 8a 86 b8 00 00 00";
const char kSigPickArrow[] =
    "8b 4c 8e 78 52 50 e8 ?? ?? ?? ?? 5f 5e 83 c4 08 c3";
const size_t kTargetArrowCall = 0x10;
const size_t kPickArrowCall = 6;
const size_t kArrowSpan = 0x80;

// The macro key object's global, read out of the macro subsystem's
// constructor (the imm32s at +15 and +23 must agree). It is NULL before the
// subsystem exists.
const char kSigMacroObject[] =
    "8b 0d ?? ?? ?? ?? 51 8b c8 e8 ?? ?? ?? ?? a3 ?? ?? ?? ?? eb 06 89 1d ?? ?? ?? ?? 6a 50 e8";
const size_t kMacroObjectImm = 15;
const size_t kMacroObjectImm2 = 23;

// The row hit test, only checked: it must still treat a menu whose kMenuMouse
// byte (game.h) is 0 as a miss, by the read below once in its first
// kHitTestBytes.
const char kSigRowHitTest[] =
    "83 ec 08 53 55 56 8b f1 8b 0d ?? ?? ?? ?? 57 89 74 24 10 d9 41 48 d8 1d ?? ?? ?? ?? df e0 25 00 41 00 00 0f 84 ?? ?? ?? ?? 8a 41 4e 84 c0 0f 84 ?? ?? ?? ?? 8d 4e 14 e8";
// mov esi,[esp+0x10] (this); mov cl,[esi+0x77]; test cl,cl; je
const uint8_t kHitTestMouseRead[] = {0x8b, 0x74, 0x24, 0x10, 0x8a, 0x4e, 0x77, 0x84, 0xc9, 0x74};
const size_t kHitTestMouseByte = 6;     // where in the read the menu offset sits
const size_t kHitTestBytes = 0x100;

// Routines the replies, reset and resize call; none is hooked.
const char kSigPassinpuReset[] =
    "33 c0 89 41 1c 89 41 20 88 41 24 89";
const char kSigPartySend[] =
    "a1 ?? ?? ?? ?? 85 c0 74 12 6a 00 6a 00 6a 74";
const char kSigPartyClear[] =
    "c6 05 ?? ?? ?? ?? 00 c3 90 90 90 90 90 90 90 90 8a";
const char kSigLink5Clear[] =
    "53 56 8b f1 33 db 88 1d ?? ?? ?? ??";
// arealist's close and reset (this = the controller): an open instance's
// begin-close clears the latch and the mode, then the rows are freed. With no
// instance open, kSigArealistLatch, that begin-close, is called alone.
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
const char kSigSetFrameRect[] =
    "83 ec 08 56 8b f1 8d 4e 14 e8 ?? ?? ?? ?? 85 c0 89 44 24 08";
const char kSigSetCursor[] =
    "83 ec 08 56 57 8b 7c 24 14 8b f1 66";
// The cursor onto a row with no input event (this = a menu; the row): what
// the yes/no class's confirm runs on a question asked again whose default is
// no.
const char kSigSelectRow[] =
    "56 57 8b 7c 24 0c 8b f1 66 85 ff 7f 05 bf 01 00 00 00 66 8b 46 58 66 3b f8 7e 02 8b f8 66 8b 46 4c 66 3b c7 74 ??";

// The text-to-glyph converter: the engine calls it only on the game thread,
// once, to build the inverse of its two-byte characters (game.h).
const char kSigGlyphConvert[] =
    "55 8b 6c 24 0c 85 ed 75 04 33 c0 5d c3 8b 44 24 10 85 c0 75 02 5d c3 53 56 57 8d 48 ff 32 db 33 ff";

// Data read out of the code that uses it: link5's confirm adds the concierge
// cache to slot * 0x20; query's input handler compares the cancel-allowed
// byte with 1.
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

// The yes/no class's constructor, never run: its vtable is the imm32 at +31,
// and its 16-byte key lands at +0x20 (game.h's kYesNoKey).
const char kSigYesNoClass[] =
    "53 56 8b f1 8d 4e 04 e8 ?? ?? ?? ?? 33 db 8b ce 66 c7 46 0e ff ff 89 5e 08 c6 46 0c 01 c7 06 ?? ?? ?? ?? 89 5e 34 e8 ?? ?? ?? ?? 8b 4c 24 0c 8d 46 20";
const size_t kYesNoClassVtable = 31;

// The yes/no class's confirm, only checked: it must keep its question where
// game.h's replies read and write it (mode +0x40, callback +0x14, window
// +0x08, cursor row +0x4C, context +0x18, chained flag +0x3A, chained default
// +0x3B), with each read below once in its first kYesNoConfirmBytes.
const char kSigYesNoConfirm[] =
    "56 8b f1 8b 46 40 85 c0 74 05 e8 ?? ?? ?? ?? 8b 46 14 85 c0 75 12 8b 46 08 6a 01 50 b9 ?? ?? ?? ?? e8 ?? ?? ?? ?? 5e c3 8b 4e 08 66 83 79 4c 01 75 08 8b 56 18 6a 01 52 eb 06 8b 4e 18 6a 00";
// call eax; mov al,[esi+0x3A]; add esp,8; test al,al
const uint8_t kConfirmChainedRead[] = {0xff, 0xd0, 0x8a, 0x46, 0x3a, 0x83, 0xc4, 0x08, 0x84, 0xc0};
// mov dword [esi+0x14],0
const uint8_t kConfirmCallbackClear[] = {0xc7, 0x46, 0x14, 0x00, 0x00, 0x00, 0x00};
// mov al,[esi+0x3B]; mov byte [esi+0x3A],0; test al,al
const uint8_t kConfirmChainedDefault[] = {0x8a, 0x46, 0x3b, 0xc6, 0x46, 0x3a, 0x00, 0x84, 0xc0};
const size_t kYesNoConfirmBytes = 0xC0;

// The KO window's input handler from its confirm on, never run: the decider
// picks the question's message (kDeadTextIfSet when it answers 1, else
// kDeadTextOtherwise), and the yes/no class's ask takes the pushed values the
// offsets below name.
const char kSigDeadConfirm[] =
    "6a ?? 6a ?? e8 ?? ?? ?? ?? 83 c4 08 84 c0 74 07 68 ?? ?? ?? ?? eb 02 6a ?? 6a ?? e8 ?? ?? ?? ?? 83 c4 08 8b 0d ?? ?? ?? ?? 6a ?? 6a ?? 6a ?? 6a ?? 50 6a ?? 68 ?? ?? ?? ?? e8 ?? ?? ?? ??";
const size_t kDeadDeciderSecond = 1;
const size_t kDeadDeciderFirst = 3;
const size_t kDeadDeciderCall = 4;
const size_t kDeadTextIfSet = 17;
const size_t kDeadTextOtherwise = 24;
const size_t kDeadCategory = 26;
const size_t kDeadMessageCall = 27;
const size_t kDeadAskerGlobal = 37;
const size_t kDeadAskMode = 42;
const size_t kDeadAskCancellable = 44;
const size_t kDeadAskHelp = 46;
const size_t kDeadAskDefault = 48;
const size_t kDeadAskContext = 51;
const size_t kDeadAskCallback = 53;
const size_t kDeadAskCall = 57;

// The ask that confirm calls, checked at its address: the class's test of a
// pending question, which answers 0 early.
// push esi; mov esi,ecx; mov eax,[esi+0x14]; test eax,eax; je +6; xor al,al; pop esi; ret 0x1C
const uint8_t kYesNoAskHead[] = {0x56, 0x8b, 0xf1, 0x8b, 0x46, 0x14, 0x85, 0xc0, 0x74, 0x06, 0x32, 0xc0, 0x5e,
                                 0xc2, 0x1c, 0x00};

// That callback (cdecl; context, answer), checked at the address the confirm
// names: on a yes it sets byte kDeadChosen of the controller in the global
// between these two, which must be the registry's dead slot.
// mov al,[esp+8]; test al,al; je +9; mov eax,[imm32]
const uint8_t kDeadChosenHead[] = {0x8a, 0x44, 0x24, 0x08, 0x84, 0xc0, 0x74, 0x09, 0xa1};
// mov byte [eax+0x24],1; ret
const uint8_t kDeadChosenTail[] = {0xc6, 0x40, 0x24, 0x01, 0xc3};

// The KO window's tick, only checked: the countdown's end at +0x1C in game
// clock seconds, the countdown at +0x20 as (end - now) * 60, which
// options('dead') reads. Past the end, its decider (the confirm's own)
// answering 1 holds the countdown at 0x1E.
const char kSigDeadTick[] =
    "56 8b f1 e8 ?? ?? ?? ?? 8b 4e 1c 3b c8 76 12 2b c8 8d 04 49 8d 04 80 c1 e0 02 89 46 20 5e c2 04 00 6a 00 6a 01 e8 ?? ?? ?? ?? 83 c4 08 84 c0 74 0b c7 46 20 1e 00 00 00 5e c2 04 00";
const size_t kDeadTickDeciderCall = 37;

// The yes/no class's input routine (vtable slot kYesNoInputSlot): its cancel
// tests the cancellable byte (game.h's kYesNoCancellable) before calling the
// callback with no, kYesNoCancelCall bytes later, in its first
// kYesNoInputBytes.
// mov cl,[esi+0x39]; test cl,cl; je near
const uint8_t kYesNoCancelTest[] = {0x8a, 0x4e, 0x39, 0x84, 0xc9, 0x0f, 0x84};
// mov edx,[esi+0x18]; push 0; push edx; call eax
const uint8_t kYesNoCancelCallback[] = {0x8b, 0x56, 0x18, 0x6a, 0x00, 0x52, 0xff, 0xd0};
const size_t kYesNoCancelCall = 11;
const size_t kYesNoInputSlot = 6;
const size_t kYesNoInputBytes = 0x190;

// The number boxes' confirms (this = the controller; the number), called:
// they call and clear the callback (moneyctr also its extra one), then close
// the window.
const char kSigMoneyConfirm[] =
    "56 8b f1 8b 46 2c 85 c0 74 15 8b 4e 30 8b 54 24 08 51 52 ff d0 83 c4 08 c7 46 2c 00 00 00 00 8b 46 34 85 c0 74 09 ff d0 c7 46 34 00 00 00 00 8b 46 08 6a 01 50 b9 ?? ?? ?? ?? e8";
const char kSigItemConfirm[] =
    "56 8b f1 8b 46 24 85 c0 74 15 8b 4e 2c 8b 54 24 08 51 52 ff d0 83 c4 08 c7 46 24 00 00 00 00 8b 46 08 6a 01 50 b9 ?? ?? ?? ?? e8 ?? ?? ?? ?? 5e c2 04 00";
const size_t kBoxConfirmManager[kBoxCount] = {54, 38};

// The number boxes' setups, only checked: each must write its question where
// kBoxLayouts reads it before opening its own window. moneyctr's max 0 means
// the gil the player has; itemctrl's writes nothing for a max of 0, and for
// 1 calls the callback with no open.
const char kSigMoneySetup[] =
    "8b 44 24 04 56 8b f1 89 46 28 8b 44 24 0c 85 c0 75 15 50 50 e8 ?? ?? ?? ?? 83 c4 08 85 c0 74 05 8b 40 04 eb 02 33 c0 8b 4c 24 10 8b 54 24 14 6a 00 89 4e 2c 6a 01 68 ?? ?? ?? ?? b9";
const char kSigItemSetup[] =
    "8b 44 24 08 53 85 c0 56 74 4a 8b 74 24 1c 8a 5c 24 0c 8b 54 24 14 89 71 28 8b 74 24 18 83 f8 01 88 59 23 88 41 22 89 51 24 89 71 2c 74 18 6a 00 6a 01 68 ?? ?? ?? ?? b9 ?? ?? ?? ?? e8";
const size_t kBoxSetupKey[kBoxCount] = {55, 51};
const size_t kBoxSetupManager[kBoxCount] = {60, 56};
const size_t kBoxSetupOpen[kBoxCount] = {77, 60};
// mov [esi+0x24],eax; mov [esi+0x30],edx; mov dword [esi+0x34],0; call
const uint8_t kMoneySetupTail[] = {0x89, 0x46, 0x24, 0x89, 0x56, 0x30, 0xc7, 0x46, 0x34, 0x00, 0x00, 0x00, 0x00,
                                   0xe8};
const size_t kMoneySetupTailAt = 64;

// The routines that ask through a number box, in game.h's kAskers order,
// each known by the callback it asks with. `setup_call` reaches the box's
// setup; `box_global` and `owner_global` are the imm32s of the box's and the
// asker's controller globals; 0 for none.
const char kSigAskDeliveryGil[] =
    "8b 44 24 04 85 c0 7e 1d 50 6a 00 6a 00 e8 ?? ?? ?? ?? 8b 0d ?? ?? ?? ?? 83 c4 08 50 e8 ?? ?? ?? ?? 84 c0 75 0e a1 ?? ?? ?? ?? 66 c7 80 b4 01 00 00 09 00 8b 0d ?? ?? ?? ?? 6a 00 8b 51 08";
const char kSigAskHandoverGil[] =
    "8b 0d ?? ?? ?? ?? 68 ?? ?? ?? ?? 6a 00 6a 00 e8 ?? ?? ?? ?? eb 19 8b ce e8 ?? ?? ?? ?? 84 c0 74 09 8b ce e8 ?? ?? ?? ?? eb 05 e8 ?? ?? ?? ?? 8b 46 08 85 c0 74 2f 66 8b 78 4c 66 3b 7e 42";
const char kSigAskTradeGil[] =
    "a1 ?? ?? ?? ?? 8b 0d ?? ?? ?? ?? 6a 00 68 ?? ?? ?? ?? 85 c0 6a 00 74 11 8b 90 ?? ?? ?? ?? 52 e8 ?? ?? ?? ?? 5f 5e c2 08 00 6a 00 e8 ?? ?? ?? ?? 5f 5e c2 08 00 83 f8 04 0f 85 ?? ?? ?? ??";
const char kSigAskAuctionBid[] =
    "8b 4c 24 04 85 c9 0f 8e ?? ?? ?? ?? 56 8b 74 24 0c 89 0d ?? ?? ?? ?? 33 c0 8b 16 51 83 fa 04 0f 95 c0 a3 ?? ?? ?? ?? f7 d8 1b c0 83 c0 02 50 33 c0 66 8b 46 04 50 6a 01 6a 00 51";
const char kSigAskAuctionSell[] =
    "8b 44 24 04 85 c0 7e 57 8b 0d ?? ?? ?? ?? 6a 00 6a 01 a3 ?? ?? ?? ?? 8b 44 24 10 6a 00 6a 00 6a 00 50 68 ?? ?? ?? ?? e8 ?? ?? ?? ?? a1 ?? ?? ?? ?? 85 c0 74 51 6a 02 6a 38 6a 0e";
const char kSigAskBazaarPrice[] =
    "c6 47 10 01 8a 46 02 88 47 11 88 5f 12 8a 4e 08 33 c0 80 f9 19 75 03 8b 46 0c 8b 0d ?? ?? ?? ?? 56 68 ?? ?? ?? ?? 68 ff e0 f5 05 50 e8 ?? ?? ?? ?? 5f 5e b0 01 5b c2 08 00 5f 5e 32 c0 5b";
const char kSigAskDeliveryItem[] =
    "8b 44 24 04 85 c0 7e 14 8b 4c 24 08 0f be c0 50 51 8b 0d ?? ?? ?? ?? e8 ?? ?? ?? ?? 8b 15 ?? ?? ?? ?? 6a 00 b9 ?? ?? ?? ?? 8b 42 08 50 e8 ?? ?? ?? ?? c3";
const char kSigAskHandoverItem[] =
    "8b 44 24 04 85 c0 7e 18 50 8b 44 24 0c 66 8b 08 8a 50 02 51 8b 0d ?? ?? ?? ?? 52 e8 ?? ?? ?? ?? a1 ?? ?? ?? ?? 6a 00 8b 48 08 51 b9 ?? ?? ?? ?? e8 ?? ?? ?? ?? c3";
const char kSigAskTradeItem[] =
    "8b 44 24 04 85 c0 7e 1a 50 8b 44 24 0c 66 0f b6 50 02 66 8b 08 51 8b 0d ?? ?? ?? ?? 52 e8 ?? ?? ?? ?? a1 ?? ?? ?? ?? 6a 00 8b 48 08 51 b9 ?? ?? ?? ?? e8 ?? ?? ?? ?? c3";
const char kSigAskStorage[] =
    "8b 44 24 08 85 c0 74 36 8b 4c 24 04 85 c9 7e 1b 8b 10 51 8b 48 08 6a 52 51 33 c9 8a 4a 02 8b 50 04 51 52 e8 ?? ?? ?? ?? 83 c4 14 a1 ?? ?? ?? ?? c6 40 04 00 8b 0d ?? ?? ?? ?? c6 41 04 00";
const char kSigAskBazaarBuy[] =
    "83 ec 54 56 8b 74 24 5c 85 f6 7e 6d 57 b9 15 00 00 00 33 c0 8d 7c 24 08 f3 ab 8b 0d ?? ?? ?? ?? a1 ?? ?? ?? ?? 0f af ce 8d 54 24 08 25 ff ff 00 00 52 66 c7 44 24 14 5e 00 89 44 24 18";
const char kSigAskSynthesis[] =
    "8b 44 24 04 85 c0 7e 18 50 8b 44 24 0c 66 8b 08 8a 50 02 51 8b 0d ?? ?? ?? ?? 52 e8 ?? ?? ?? ?? 8b 0d ?? ?? ?? ?? 6a 00 6a 00 6a 00 6a 00 e8 ?? ?? ?? ?? a1 ?? ?? ?? ?? 6a 00 8b 48 08 51";
const char kSigAskGuildBuy[] =
    "8b c8 33 c0 8a 45 02 3b c8 7e 02 8b c8 85 c9 74 1f 6a 00 55 68 ?? ?? ?? ?? 51 8b 0d ?? ?? ?? ?? 6a 01 e8 ?? ?? ?? ?? 5f 5e 5d b0 01 5b c2 08 00 5f 5e 5d 32 c0 5b c2 08 00";
const char kSigAskGuildSell[] =
    "85 c0 74 31 3d 40 40 40 80 74 2a c6 47 10 01 8a 46 02 88 47 11 8b 4e 04 6a 00 56 68 ?? ?? ?? ?? 51 8b 0d ?? ?? ?? ?? 6a 01 e8 ?? ?? ?? ?? 5f b0 01 5e c2 08 00 5f 32 c0 5e c2 08 00";
const char kSigAskShopBuy[] =
    "8b 44 24 24 33 d2 f7 f6 3b c8 7e 02 8b c8 85 c9 74 c6 6a 00 6a 00 68 ?? ?? ?? ?? 51 8b 0d ?? ?? ?? ?? 6a 01 e8 ?? ?? ?? ?? 5f 5e 5d b0 01 5b 83 c4 0c c2 08 00";
const char kSigAskShopSell[] =
    "c6 47 10 01 8a 56 02 88 57 11 8b 46 04 8b 0d ?? ?? ?? ?? 6a 00 56 68 ?? ?? ?? ?? 50 6a 01 e8 ?? ?? ?? ?? 5f b0 01 5e c2 08 00 5f 32 c0 5e c2 08 00";

struct AskerSite {
    const char* name;
    const char* signature;
    uint8_t callback;           // the pushed callback's imm32; 0: the hit is the callback
    uint8_t setup_call;
    uint8_t box_global;
    uint8_t owner_global;
};

const AskerSite kAskerSites[kAskerCount] = {
    {"ask_delivery_gil", kSigAskDeliveryGil, 0, 0, 0, 20},
    {"ask_handover_gil", kSigAskHandoverGil, 7, 15, 2, 0},
    {"ask_trade_gil", kSigAskTradeGil, 14, 31, 7, 0},
    {"ask_auction_bid", kSigAskAuctionBid, 0, 0, 0, 0},
    {"ask_auction_sell", kSigAskAuctionSell, 0, 0, 0, 0},
    {"ask_bazaar_price", kSigAskBazaarPrice, 34, 44, 28, 0},
    {"ask_delivery_item", kSigAskDeliveryItem, 0, 0, 0, 19},
    {"ask_handover_item", kSigAskHandoverItem, 0, 0, 0, 22},
    {"ask_trade_item", kSigAskTradeItem, 0, 0, 0, 24},
    {"ask_storage", kSigAskStorage, 0, 0, 0, 0},
    {"ask_bazaar_buy", kSigAskBazaarBuy, 0, 0, 0, 0},
    {"ask_synthesis", kSigAskSynthesis, 0, 0, 0, 22},
    {"ask_guild_buy", kSigAskGuildBuy, 21, 34, 28, 0},
    {"ask_guild_sell", kSigAskGuildSell, 28, 41, 35, 0},
    {"ask_shop_buy", kSigAskShopBuy, 23, 36, 30, 0},
    {"ask_shop_sell", kSigAskShopSell, 23, 30, 15, 0},
};

// sortyn's setup, only checked: it must write the question where game.h's
// kSortyn* read it before opening its own key. It answers 1 whether the open
// was refused or not.
const char kSigSortynSetup[] =
    "8b 44 24 04 56 85 c0 8b f1 75 06 32 c0 5e c2 18 00 8b 4c 24 10 8b 54 24 14 89 46 14 8b 44 24 0c 89 46 18 8b 44 24 1c 6a 00 89 4e 1c 6a 01 68 ?? ?? ?? ?? b9 ?? ?? ?? ?? 89 56 20 89 46 24 e8";
const size_t kSortynSetupKey = 47;
const size_t kSortynSetupManager = 52;
const size_t kSortynSetupOpen = 62;

// sortyn's input routine from its confirm on, only checked: row 1 answers 1,
// row 2 and the cancel 0; it calls and clears the callback, then closes its
// own key.
const char kSigSortynConfirm[] =
    "0f bf 44 24 0c 48 74 0a 48 75 37 c6 44 24 08 00 eb 05 c6 44 24 08 01 8b 46 14 85 c0 74 15 8b 4c 24 08 8b 56 18 51 52 ff d0 83 c4 08 c7 46 14 00 00 00 00 68 ?? ?? ?? ?? b9 ?? ?? ?? ?? e8";
const size_t kSortynConfirmKey = 52;
const size_t kSortynConfirmManager = 57;
const size_t kSortynConfirmClose = 61;

// The key item list's ask, never run: it opens evitem01, then calls the
// setup, which writes the question at +0x74 and pushes the callback; the
// callback reads the key item list's global, its registry slot.
const char kSigEvitemAsk[] =
    "8b f8 85 ff 0f 8c ?? ?? ?? ?? 57 e8 ?? ?? ?? ?? 83 c4 04 8b ce 85 c0 75 30 e8 ?? ?? ?? ?? 8b ce e8 ?? ?? ?? ?? 6a 00 6a 01 68 ?? ?? ?? ?? b9 ?? ?? ?? ?? e8 ?? ?? ?? ?? 8b 0d ?? ?? ?? ?? 57";
const size_t kEvitemAskKey = 42;
const size_t kEvitemAskManager = 47;
const size_t kEvitemAskOpen = 51;
const size_t kEvitemAskGlobal = 58;
const size_t kEvitemAskSetup = 63;
// mov eax,[esp+4]; push esi; mov esi,ecx; push edi; push eax; lea edi,[esi+0x74]
const uint8_t kEvitemSetupHead[] = {0x8b, 0x44, 0x24, 0x04, 0x56, 0x8b, 0xf1, 0x57, 0x50, 0x8d, 0x7e, 0x74};
// push edi; push 0; push imm32
const uint8_t kEvitemSetupPush[] = {0x57, 0x6a, 0x00, 0x68};
const size_t kEvitemSetupPushAt = 51;
// push esi; mov esi,[imm32]
const uint8_t kEvitemCallbackHead[] = {0x56, 0x8b, 0x35};

// evitem01's input routine from its code test on, never run: the cancel
// answers 0, the confirm 1 on the list's first row (+0x30 0), passed as a
// byte to the callback at +0x64 (cdecl); then the window closes by instance.
const char kSigEvitemInput[] =
    "0f bf c1 83 e8 05 74 1b 48 74 11 8b 44 24 0c 50 51 8b ce e8 ?? ?? ?? ?? 5e c2 08 00 c6 44 24 08 00 eb 0c 8b 46 30 85 c0 0f 94 c1 88 4c 24 08 8b 54 24 08 81 e2 ff 00 00 00 52 ff 56 64";
const size_t kEvitemCloseAt = 61;
// mov al,[esi+0x70]; mov ecx,[esi+8]; add esp,4; push eax; push ecx; mov ecx,imm32
const uint8_t kEvitemCloseTail[] = {0x8a, 0x46, 0x70, 0x8b, 0x4e, 0x08, 0x83, 0xc4, 0x04, 0x50, 0x51, 0xb9};

// confyn's input routine, never run: the confirm on row 1 restores the
// defaults (cdecl; 1 in mode 0, 2 in mode 1, nothing in any other), and any
// confirm closes the window by instance.
const char kSigConfynInput[] =
    "66 83 7c 24 04 05 56 8b f1 75 31 66 83 7c 24 0c 01 75 19 8b 46 14 83 e8 00 74 07 48 75 0e 6a 02 eb 02 6a 01 e8 ?? ?? ?? ?? 83 c4 04 8b 46 08 6a 01 50 b9 ?? ?? ?? ?? e8";
const size_t kConfynRestore = 36;
const size_t kConfynManager = 51;
const size_t kConfynClose = 55;

// bankmenu's open hook, then its begin-close, never run: the hook turns row 2
// off unless the player has Mog Safe 2, a flag in the player data; the
// begin-close clears the pending callback.
const char kSigBankOpen[] =
    "a1 ?? ?? ?? ?? 56 50 8b f1 e8 ?? ?? ?? ?? 83 c4 04 85 c0 75 0c 8b 4e 08 6a 02 6a 05 e8 ?? ?? ?? ?? 5e c3 90 90 90 90 90 90 90 90 90 90 90 90 90 e9 ?? ?? ?? ??";
const size_t kBankData = 1;
const size_t kBankAsk = 9;
const size_t kBankBeginClose = 48;
// mov eax,[esp+4]; push eax; call
const uint8_t kBankAskHead[] = {0x8b, 0x44, 0x24, 0x04, 0x50, 0xe8};
// add esp,4; and eax,1; ret
const uint8_t kBankAskTail[] = {0x83, 0xc4, 0x04, 0x83, 0xe0, 0x01, 0xc3};
// mov eax,[esp+4]; test eax,eax; jne +9; mov eax,[imm32]
const uint8_t kBankFlagHead[] = {0x8b, 0x44, 0x24, 0x04, 0x85, 0xc0, 0x75, 0x09, 0xa1};
// test eax,eax; je +0xB; xor ecx,ecx; mov cl,[eax+imm32]
const uint8_t kBankFlagMid[] = {0x85, 0xc0, 0x74, 0x0b, 0x33, 0xc9, 0x8a, 0x88};
// mov eax,ecx; ret
const uint8_t kBankFlagTail[] = {0x8b, 0xc1, 0xc3};
// mov dword [ecx+0x14],0; ret
const uint8_t kBankClear[] = {0xc7, 0x41, 0x14, 0x00, 0x00, 0x00, 0x00, 0xc3};

// bankmenu's input routine, never run: row 1 is container 1, any other 9; the
// callback at +0x14 (cdecl) gets the container and 0 on a confirm, 1 on a
// cancel, then the window closes through its class's close.
const char kSigBankInput[] =
    "0f bf 44 24 04 56 8b f1 33 c9 66 83 7c 24 0c 01 0f 95 c1 83 e8 05 8d 0c cd 01 00 00 00 74 1d 48 75 30 8b 46 14 85 c0 74 22 6a 01 51 ff d0 83 c4 08 8b ce e8 ?? ?? ?? ?? 5e c2 08 00 8b 46 14";
const size_t kBankInputClose = 51;
// mov eax,[ecx]; jmp [eax+0x48]
const uint8_t kBankClose[] = {0x8b, 0x01, 0xff, 0x60, 0x48};

// maplist's close and reset (this = the controller), then its key handler,
// which first reads maplist's global, its registry slot. In the handler's
// first kMaplistHandlerBytes, once each, each ending in a call of the close
// and reset: a leaf row's confirm (the callback at +0x6C, cdecl, gets the row's entry),
// the confirm of a row with id 0x7FFFFFFF, and the cancel at the top level.
const char kSigMaplistClose[] =
    "56 8b f1 8b 06 ff 50 74 6a 01 6a 00 6a 00 8b ce c7 46 34 00 00 00 00 c6 46 49 01 e8 ?? ?? ?? ?? 8b ce 5e e9 ?? ?? ?? ?? 90 90 90 90 90 90 90 90 a1 ?? ?? ?? ?? 56 85 c0 8b f1 0f 84";
const size_t kMaplistHandler = 48;
const size_t kMaplistGlobal = 49;
const size_t kMaplistHandlerBytes = 0x130;
// test ecx,ecx; jl; mov ecx,[esi+0x6C]; test ecx,ecx; je +6; push eax; call ecx; add esp,4; mov ecx,esi; call
const uint8_t kMaplistLeaf[] = {0x85, 0xc9, 0x7c, 0x1f, 0x8b, 0x4e, 0x6c, 0x85, 0xc9, 0x74, 0x06, 0x50, 0xff, 0xd1,
                                0x83, 0xc4, 0x04, 0x8b, 0xce, 0xe8};
// mov ecx,[eax]; cmp ecx,0x7FFFFFFF; jne; mov ecx,esi; call
const uint8_t kMaplistCloseRow[] = {0x8b, 0x08, 0x81, 0xf9, 0xff, 0xff, 0xff, 0x7f, 0x75, 0x12, 0x8b, 0xce, 0xe8};
// cmp word [esi+0x58],0; jne +0xB; call
const uint8_t kMaplistTopCancel[] = {0x66, 0x83, 0x7e, 0x58, 0x00, 0x75, 0x0b, 0xe8};
// cmp word [esi+0x5C],0; mov ecx,esi; jl
const uint8_t kMaplistSubLevel[] = {0x66, 0x83, 0x7e, 0x5c, 0x00, 0x8b, 0xce, 0x7c};
// Every confirm and cancel first needs the player data's global (bankmenu's).
// Each step between levels rebuilds the list, then selects a row (this, row,
// 0): a descent (a row id below 0) row 0, a step back up the row the descent
// kept. The rebuild frees the rows through the calls close and reset makes.
const size_t kMaplistData = 65;
const size_t kMaplistSetRows = 27;
const size_t kMaplistFreeRows = 35;
// mov cx,[esi+0x58]; mov [esi+0x5E],cx; mov dx,[eax]; neg dx; mov [esi+0x58],dx; mov ax,[eax+4]; mov ecx,esi;
// mov [esi+0x5C],ax; call
const uint8_t kMaplistDescend[] = {0x66, 0x8b, 0x4e, 0x58, 0x66, 0x89, 0x4e, 0x5e, 0x66, 0x8b, 0x10, 0x66, 0xf7, 0xda,
                                   0x66, 0x89, 0x56, 0x58, 0x66, 0x8b, 0x40, 0x04, 0x8b, 0xce, 0x66, 0x89, 0x46, 0x5c,
                                   0xe8};
// mov ecx,esi; mov [esi+0x5A],ax; call
const uint8_t kMaplistDescendKeep[] = {0x8b, 0xce, 0x66, 0x89, 0x46, 0x5a, 0xe8};
// push 0; push 0; mov ecx,esi; call
const uint8_t kMaplistDescendSelect[] = {0x6a, 0x00, 0x6a, 0x00, 0x8b, 0xce, 0xe8};
// mov ax,[esi+0x5E]; mov word [esi+0x5C],-1; mov [esi+0x58],ax; call
const uint8_t kMaplistBackMaps[] = {0x66, 0x8b, 0x46, 0x5e, 0x66, 0xc7, 0x46, 0x5c, 0xff, 0xff, 0x66, 0x89, 0x46, 0x58,
                                    0xe8};
// mov word [esi+0x58],0; mov word [esi+0x5E],0; call
const uint8_t kMaplistBackRegion[] = {0x66, 0xc7, 0x46, 0x58, 0x00, 0x00, 0x66, 0xc7, 0x46, 0x5e, 0x00, 0x00, 0xe8};
// movsx ecx,word [esi+0x5A]; push 0; push ecx; mov ecx,esi; call -- and through edx
const uint8_t kMaplistBackSelect[2][10] = {{0x0f, 0xbf, 0x4e, 0x5a, 0x6a, 0x00, 0x51, 0x8b, 0xce, 0xe8},
                                           {0x0f, 0xbf, 0x56, 0x5a, 0x6a, 0x00, 0x52, 0x8b, 0xce, 0xe8}};
// mov eax,[ecx+8]; test eax,eax; je +0xD; movsx eax,word [eax+0x4C]; movsx ecx,word [ecx+0x1E];
// lea eax,[eax+ecx-1]; ret; or eax,-1; ret
const uint8_t kMaplistCursor[] = {0x8b, 0x41, 0x08, 0x85, 0xc0, 0x74, 0x0d, 0x0f, 0xbf, 0x40, 0x4c, 0x0f, 0xbf, 0x49,
                                  0x1e, 0x8d, 0x44, 0x08, 0xff, 0xc3, 0x83, 0xc8, 0xff, 0xc3};
// push ebx; push esi; push edi; push 1; push 0; push 0; mov esi,ecx; call
const uint8_t kMaplistRebuildHead[] = {0x53, 0x56, 0x57, 0x6a, 0x01, 0x6a, 0x00, 0x6a, 0x00, 0x8b, 0xf1, 0xe8};
// mov ecx,esi; call
const uint8_t kMaplistRebuildFree[] = {0x8b, 0xce, 0xe8};

// map0's input routine, never run: its cancel (code 6, Escape) closes the
// window by instance, then calls the map's close with 0 on mapframe's
// controller, which closes kMapWindows by name, kMapClose bytes each from
// kMapClosesAt, then fades out (kMapClosesTail).
const char kSigMap0Input[] =
    "0f bf 44 24 04 83 e8 05 74 27 48 0f 85 c3 00 00 00 8b 41 08 6a 01 50 b9 ?? ?? ?? ?? e8 ?? ?? ?? ?? 8b 0d ?? ?? ?? ?? 6a 00 e8 ?? ?? ?? ?? c2 08 00";
const size_t kMap0Manager = 24;
const size_t kMap0CloseInstance = 28;
const size_t kMap0Frame = 35;
const size_t kMap0MapClose = 41;
// mov al,[esp+4]; push esi; test al,al; mov esi,ecx; je +0x17
const uint8_t kMapCloseHead[] = {0x8a, 0x44, 0x24, 0x04, 0x56, 0x84, 0xc0, 0x8b, 0xf1, 0x74, 0x17};
const size_t kMapClosesAt = 0xAE;
const size_t kMapClose = 15;
const char* const kMapWindows[] = {"map0", "mapv3", "mapv2", "scanlist", "mapscan"};
const int kMapWindowCount = 5;
// mov edx,[esi]; mov ecx,esi; call [edx+0x50]; pop esi; ret 4
const uint8_t kMapClosesTail[] = {0x8b, 0x16, 0x8b, 0xce, 0xff, 0x52, 0x50, 0x5e, 0xc2, 0x04, 0x00};

// The game's GM tell poller, never run by the engine: the question's result
// word (+0x548 of the window's controller) is 0 while waiting, 0xFF or below
// 0 when declined, else the option chosen. kGmTellTitleRead's imm32 is the
// queue's start + kGmTellTitle (game.h).
const char kSigGmTellPoll[] =
    "8b 0d ?? ?? ?? ?? e8 ?? ?? ?? ?? 66 85 c0 0f 84 ?? ?? ?? ?? 66 3d ff 00 0f 84 ?? ?? ?? ?? 66 85 c0 0f 8c ?? ?? ?? ?? 8b 0d ?? ?? ?? ?? 0f bf c0 8d 14 89 c1 e2 04 03 d0 8d 04 89 c1 e0 06";
const size_t kGmTellRead = 6;
const size_t kGmTellSlot = 41;
const size_t kGmTellTitleAt = 62;
// mov ax,[ecx+0x548]; ret
const uint8_t kGmTellResultRead[] = {0x66, 0x8b, 0x81, 0x48, 0x05, 0x00, 0x00, 0xc3};
// mov edx,[edx*4+imm32]
const uint8_t kGmTellTitleRead[] = {0x8b, 0x14, 0x95};

// mp_dead's input routine from its code test on, never run: the confirm
// writes 1 at +0x24 on row 1, else 2; the cancel hands the focus to the chat
// logs. Its tick (kMpDeadTick) writes the countdown at +0x20 from the
// deadline at +0x1C; past it, 2 at +0x24, the countdown 0, and the window
// closed.
const char kSigMpDeadInput[] =
    "83 e8 05 74 1d 48 75 33 8b 0d ?? ?? ?? ?? b8 01 00 00 00 88 41 59 8b 15 ?? ?? ?? ?? 88 42 59 c2 08 00 b8 01 00 00 00 66 39 44 24 08 75 06 89 41 24 c2 08 00 c7 41 24 02 00 00 00";
const size_t kMpDeadTickBytes = 0x100;
const uint8_t kMpDeadTick[] = {0x8b, 0x4e, 0x1c, 0x3b, 0xc8, 0x76, 0x12, 0x2b, 0xc8, 0x8d, 0x04, 0x49, 0x8d, 0x04,
                               0x80, 0xc1, 0xe0, 0x02, 0x89, 0x46, 0x20, 0x5e, 0xc2, 0x04, 0x00, 0x8b, 0xce, 0xc7,
                               0x46, 0x24, 0x02, 0x00, 0x00, 0x00, 0xc7, 0x46, 0x20, 0x00, 0x00, 0x00, 0x00, 0xe8};

// ok's input routine, never run: once acknowledged (+0x25C 1) it takes
// nothing; its confirm marks the window busy, writes 1 at +0x25C, calls the
// callback at +0x14 (cdecl) with the context at +0x18 when one is set, and
// closes the window by instance, again when +0x258 is set.
const char kSigOkInput[] =
    "56 8b f1 83 be 5c 02 00 00 01 74 58 66 83 7c 24 08 05 75 50 8b 46 08 c6 40 5d 01 c7 86 5c 02 00 00 01 00 00 00 e8 ?? ?? ?? ?? 8b 46 14 85 c0 74 19 8b 4e 18 51 ff d0 8b 56 08 83 c4 04 b9";
const size_t kOkManager = 62;
const size_t kOkClose = 69;
const size_t kOkInputBytes = 0x68;
// mov al,[esi+0x258]; test al,al; je
const uint8_t kOkCloseAgain[] = {0x8a, 0x86, 0x58, 0x02, 0x00, 0x00, 0x84, 0xc0, 0x74};

// mogdoor's input routine, never run. Its confirm does nothing while the gate
// answers al 0. The exit byte +0x15: 0xFF stays (top row 1, which closes the
// window; row 4, which opens the list of areas, +0x18), 0 goes back (row 2),
// 0x7D plus the floor read's nonzero (row 3, with +0x1A set), 0x7F the Mog
// Garden (only with +0x19 set), else the area's place in the list. An exit of
// 0 or more sets the door actor's +0x880 routine, opens the door and writes
// bl, 1 at kDoorTailBytes.
const char kSigDoorInput[] =
    "53 56 8b f1 e8 ?? ?? ?? ?? 0f bf 44 24 0c 48 83 f8 08 0f 87 ?? ?? ?? ?? ff 24 85 ?? ?? ?? ?? 6a 00 6a 04 e8 ?? ?? ?? ?? 83 c4 08 84 c0 0f 84 ?? ?? ?? ?? 8a 46 18 bb 01 00 00 00 84 c0 74 3b";
const size_t kDoorFloorCall = 4;
const size_t kDoorCodeTable = 27;
const size_t kDoorConfirmCase = 31;
const size_t kDoorGateSecond = 32;
const size_t kDoorGateFirst = 34;
const size_t kDoorGateCall = 35;
const size_t kDoorAreaRowAt = 63;
const size_t kDoorInputBytes = 0x1C0;
// movsx: the row; mov edx,[esi+0x1C]; ...; cmp word [edx+ecx*2-2],imm16
const uint8_t kDoorAreaRow[] = {0x66, 0x8b, 0x44, 0x24, 0x10, 0x8b, 0x56, 0x1c, 0x0f, 0xbf, 0xc8, 0x66, 0x81, 0x7c,
                                0x4a, 0xfe};
// jne; mov al,[esi+0x19]; test al,al; jne
const uint8_t kDoorGardenRow[] = {0x75, 0x22, 0x8a, 0x46, 0x19, 0x84, 0xc0, 0x75, 0x15};
const size_t kDoorGardenRowAt = 81;
// mov byte [esi+0x15],0x7F; jmp; mov [esi+0x15],al; jmp
const uint8_t kDoorAreaExit[] = {0xc6, 0x46, 0x15, 0x7f, 0xeb, 0x71, 0x88, 0x46, 0x15, 0xeb, 0x6c};
const size_t kDoorAreaExitAt = 111;
// movsx eax,row; dec eax; cmp eax,3; ja; jmp [eax*4+imm32]
const uint8_t kDoorTopRow[] = {0x0f, 0xbf, 0x44, 0x24, 0x10, 0x48, 0x83, 0xf8, 0x03, 0x77, 0x61, 0xff, 0x24, 0x85};
const size_t kDoorTopRowAt = 122;
// Row 1: mov eax,[esi+8]; push ebx; push eax; mov ecx,imm32 (the manager);
// then mov byte [esi+0x15],0xFF; call (close by instance).
const uint8_t kDoorStayRow[] = {0x8b, 0x46, 0x08, 0x53, 0x50, 0xb9};
const uint8_t kDoorStayClose[] = {0xc6, 0x46, 0x15, 0xff, 0xe8};
// Row 2: mov byte [esi+0x15],0; jmp
const uint8_t kDoorBackRow[] = {0xc6, 0x46, 0x15, 0x00, 0xeb};
// Row 3: +0x1A tested, else 0xFF and the buzz; then the floor read called
// (its call at +18), and 0x7D plus its nonzero written at +0x15.
const uint8_t kDoorFloorRow[] = {0x8a, 0x46, 0x1a, 0x84, 0xc0, 0x75, 0x0b, 0xc6, 0x46, 0x15, 0xff, 0xe8};
const uint8_t kDoorFloorRowRead[] = {0xeb, 0x10, 0xe8};
const uint8_t kDoorFloorRowExit[] = {0x85, 0xc0, 0x0f, 0x95, 0xc2, 0x80, 0xc2, 0x7d, 0x88, 0x56, 0x15};
// Row 4: 0xFF at +0x15, +0x18 = 1, then the cursor on row 1.
const uint8_t kDoorListRow[] = {0x8b, 0x4e, 0x08, 0x6a, 0x00, 0x53, 0xc6, 0x46, 0x15, 0xff, 0x88, 0x5e, 0x18, 0xe8};
// mov al,[esi+0x15]; test al,al; jl
const uint8_t kDoorTail[] = {0x8a, 0x46, 0x15, 0x84, 0xc0, 0x0f, 0x8c};
// mov eax,[eax+0x38]; test eax,eax; je; mov dword [eax+0x880],imm32
const uint8_t kDoorTailActor[] = {0x8b, 0x40, 0x38, 0x85, 0xc0, 0x74, 0x1a, 0xc7, 0x80, 0x80, 0x08, 0x00, 0x00};
// mov ecx,[imm32]: the Mog House object's global again
const uint8_t kDoorTailHouseRead[] = {0x8b, 0x0d};
// push 0; mov ecx,[ecx+0x38]; call
const uint8_t kDoorTailOpenCall[] = {0x6a, 0x00, 0x8b, 0x49, 0x38, 0xe8};
// mov [imm32],bl
const uint8_t kDoorTailByte[] = {0x88, 0x1d};
// pop esi; pop ebx; ret 8
const uint8_t kDoorTailEnd[] = {0x5e, 0x5b, 0xc2, 0x08, 0x00};
const size_t kDoorTailHouse = 12;
const size_t kDoorTailActorAt = 16;
const size_t kDoorTailOpened = 29;
const size_t kDoorTailHouse2 = 35;
const size_t kDoorTailOpenAt = 39;
const size_t kDoorTailOpen = 44;
const size_t kDoorTailBytes[3] = {49, 55, 61};
const size_t kDoorTailEndAt = 67;
// The floor read (cdecl): the player data's global, then kDoorFloorMid, the
// word's offset, kDoorFloorTail.
const uint8_t kDoorFloorMid[] = {0x85, 0xc0, 0x75, 0x01, 0xc3, 0x33, 0xc9, 0x66, 0x8b, 0x88};
const uint8_t kDoorFloorTail[] = {0x8b, 0xc1, 0xc3};
// The routine the exit puts at the door actor's +0x880: it pushes mogdoor's
// +0x15 to the call that sends the exit.
const uint8_t kDoorOpenedHead[] = {0x0f, 0xbf, 0x05};
const uint8_t kDoorOpenedActor[] = {0x8b, 0x0c, 0x85};
const uint8_t kDoorOpenedTest[] = {0x85, 0xc9, 0x74, 0x0f, 0x8b, 0x15};
const uint8_t kDoorOpenedSend[] = {0x8a, 0x42, 0x15, 0x50, 0xe8};
// mogdoor's vtable is the one naming the input routine at kDoorInputSlot. In
// its open hook, kDoorOpenRange is followed by the last nation it takes (an
// imm8), kDoorOpenList by the table of lists (an imm32).
const size_t kDoorInputSlot = 6;
const size_t kDoorOpenSlot = 1;
const size_t kDoorDrawSlot = 4;
const size_t kDoorTickSlot = 10;
const uint8_t kDoorOpenRange[] = {0x33, 0xd2, 0x3b, 0xc2, 0x74, 0x4f, 0x8a, 0x41, 0x14, 0x3a, 0xc2, 0x7e, 0x38, 0x3c};
const uint8_t kDoorOpenList[] = {0x7f, 0x34, 0x0f, 0xbe, 0xc0, 0x8b, 0x04, 0x85};
const uint8_t kDoorOpenCount[] = {0x88, 0x51, 0x16, 0x89, 0x41, 0x1c};
// mov [ecx+0x18],dl; mov [ecx+0x19],dl; mov [ecx+0x1A],dl; ret
const uint8_t kDoorOpenTop[] = {0x88, 0x51, 0x18, 0x88, 0x51, 0x19, 0x88, 0x51, 0x1a, 0xc3};
const size_t kDoorOpenRangeAt = 5;
const size_t kDoorOpenNations = 19;
const size_t kDoorOpenListAt = 20;
const size_t kDoorOpenTable = 28;
const size_t kDoorOpenCountAt = 32;
const size_t kDoorOpenBytes = 0x60;
// Its tick writes +0x19 once and +0x1A twice within kDoorTickBytes.
const uint8_t kDoorTickHead[] = {0x51, 0x53, 0x56, 0x8b, 0xf1, 0xe8};
const uint8_t kDoorTickGarden[] = {0x88, 0x46, 0x19};
const uint8_t kDoorTickFloor[] = {0x88, 0x46, 0x1a};
const size_t kDoorTickBytes = 0x90;
// Its draw: the back row's text depends on the call before
// kDoorDrawCondition; the message getter is called with each text.
const uint8_t kDoorDrawCondition[] = {0x8b, 0x4e, 0x08, 0x8a, 0xd8};
const uint8_t kDoorDrawTitle[] = {0x55, 0x6a, 0x45, 0x6a, 0x09, 0xe8};
const uint8_t kDoorDrawStay[] = {0x55, 0x68, 0xb8, 0x00, 0x00, 0x00, 0x6a, 0x09, 0xe8};
const uint8_t kDoorDrawBack[] = {0x84, 0xdb, 0xb8, 0x46, 0x00, 0x00, 0x00, 0x75, 0x05, 0xb8, 0xf1, 0x00, 0x00, 0x00};
const uint8_t kDoorDrawFloor[] = {0x55, 0x68, 0x25, 0x01, 0x00, 0x00, 0x6a, 0x09, 0xe8};
const uint8_t kDoorDrawList[] = {0x55, 0x68, 0xb9, 0x00, 0x00, 0x00, 0x6a, 0x09, 0xe8};
const uint8_t kDoorDrawArea[] = {0x51, 0x6a, 0x14, 0xe8};
const size_t kDoorDrawBytes = 0x290;

// The door's handler from its open on, never run: it writes the nation at
// +0x14 and +0x1B of mogdoor's controller, then opens mogdoor.
const char kSigDoorHandler[] =
    "e8 ?? ?? ?? ?? 8b 0d ?? ?? ?? ?? 6a 00 6a 01 68 ?? ?? ?? ?? 88 41 14 88 59 1b b9 ?? ?? ?? ?? e8";
const size_t kDoorHandlerGlobal = 7;
const size_t kDoorHandlerKey = 16;
const size_t kDoorHandlerManager = 27;
const size_t kDoorHandlerOpen = 31;

// The NPC dialogue boxes' input routine (slot 6 of both vtables), never run:
// it calls the handler, again for a box in kDialogueGlobals. The handler
// hands the code to the box's text object's dispatch; its cancel (code 6,
// with the cancel-allowed byte 1) hands the box's lines back to the chat log,
// clears the text object, sets the event's wait flag to 2 and closes the box
// by instance.
const char kSigDialogueInput[] =
    "53 8b 5c 24 08 56 57 8b 7c 24 14 8b f1 57 53 e8 ?? ?? ?? ?? 8b 0d ?? ?? ?? ?? 3b f1 74 0a 8b 0d ?? ?? ?? ?? 3b f1 75 07";
const size_t kDialogueHandlerCall = 15;
const size_t kDialogueGlobals[2] = {22, 32};
const uint8_t kDialogueAgain[] = {0x57, 0x53, 0xe8};
const size_t kDialogueAgainAt = 40;
const size_t kDialogueHandlerBytes = 0x90;
// mov ecx,[esi+0x68]; push edi; call
const uint8_t kDialogueDispatchCall[] = {0x8b, 0x4e, 0x68, 0x57, 0xe8};
// cmp di,6; jne; call (the sound)
const uint8_t kDialogueCancelTest[] = {0x66, 0x83, 0xff, 0x06, 0x75, 0x33, 0xe8};
// cmp byte [imm32],1; jne
const uint8_t kDialogueGateHead[] = {0x80, 0x3d};
const uint8_t kDialogueGateTail[] = {0x01, 0x75, 0x39};
// mov eax,[esi+0x64]; mov edx,[esi+0x50]; push edx; mov ecx,[eax+0x18]; call
const uint8_t kDialogueRelease[] = {0x8b, 0x46, 0x64, 0x8b, 0x56, 0x50, 0x52, 0x8b, 0x48, 0x18, 0xe8};
// mov ecx,[esi+0x68]; call
const uint8_t kDialogueClear[] = {0x8b, 0x4e, 0x68, 0xe8};
// mov byte [imm32],2
const uint8_t kDialogueWait[] = {0xc6, 0x05};
// mov ecx,[esi+8]; push 1; push ecx; mov ecx,imm32
const uint8_t kDialogueClose[] = {0x8b, 0x4e, 0x08, 0x6a, 0x01, 0x51, 0xb9};
const size_t kDialogueGateAt = 9;               // before the test
const size_t kDialogueGateTailAt = 3;           // before the test
const size_t kDialogueReleaseAt = 11;
const size_t kDialogueClearAt = 26;
const size_t kDialogueWaitAt = 34;
const size_t kDialogueCloseAt = 41;
// The text dispatch (this = a text object; the code) calls its gate on it,
// then on the other box's text object (kShapeDialogueTwin holes: 0 one box's
// text object global, 1 and 3 the other's, 2 and 4 the gate). The gate, on
// a page waiting for a key (+0x20 bit 0) and code 5, calls the advance, then
// the page-turn sound. The advance's show counts the page's lines at +0x4E,
// each line's codes at +4 of a line object with their count at +0x208, and
// adds each line to the box's list at +0x14.
const uint8_t kDialogueDispatchHead[] = {0x56, 0x57, 0x8b, 0x7c, 0x24, 0x0c, 0x8b, 0xf1, 0x57, 0xe8};
const char kShapeDialogueTwin[] =
    "8b 0d ?? ?? ?? ?? 3b f1 75 11 8b 0d ?? ?? ?? ?? 57 e8 ?? ?? ?? ?? 5f 5e c2 04 00 3b 35 ?? ?? ?? ?? 75 06 57 e8 ?? ?? ?? ?? 5f 5e c2 04 00";
const uint8_t kDialogueGateHeadBytes[] = {0xf6, 0x41, 0x20, 0x01, 0x74, 0x12, 0x66, 0x83, 0x7c, 0x24, 0x04, 0x05,
                                          0x75, 0x0a, 0xe8};
// push '0015'
const uint8_t kDialogueSoundName[] = {0x68, 0x30, 0x30, 0x31, 0x35};
const size_t kDialogueSoundBytes = 0x20;
const uint8_t kDialogueAdvanceShow[] = {0x8b, 0x4e, 0x30, 0x50, 0xe8};
const size_t kDialogueAdvanceBytes = 0xC0;
const uint8_t kDialogueShowLines[] = {0x66, 0xc7, 0x43, 0x4e, 0x00, 0x00};
const uint8_t kDialogueShowLine[] = {0x66, 0xff, 0x43, 0x4e};
const uint8_t kDialogueShowCount[] = {0x66, 0x89, 0x85, 0x08, 0x02, 0x00, 0x00};
const uint8_t kDialogueShowCodes[] = {0x8d, 0x55, 0x04, 0x68, 0x01, 0x01, 0x00, 0x00};
const uint8_t kDialogueShowAdd[] = {0x8d, 0x4b, 0x14, 0xe8};
const size_t kDialogueShowBytes = 0x180;
const uint8_t kDialogueClearHead[] = {0x51, 0x53, 0x56, 0x8b, 0xf1, 0x33, 0xdb, 0x8b, 0x46, 0x1c, 0x3b, 0xc3};
// The boxes' tick, only checked, once each in its first kDialogueTickBytes:
// the key test, the read of +0x60, and the timer (an s64 count at +0x28 of
// the text object) before the last-page test.
const size_t kDialogueTickSlot = 10;
const size_t kDialogueTickBytes = 0x180;
const uint8_t kDialogueClick[] = {0x6a, 0xff, 0x6a, 0x05, 0x68, 0x8b, 0x00, 0x00, 0x00, 0x6a, 0x3e, 0xe8};
const uint8_t kDialogueClickAfter[] = {0x83, 0xc4, 0x10, 0x3c, 0x01};
const uint8_t kDialogueAuto[] = {0x66, 0x39, 0x6f, 0x60};
const uint8_t kDialogueTimed[] = {0x84, 0xc0, 0x74, 0x08, 0x8b, 0x4f, 0x68, 0xe8};
const uint8_t kDialogueTimerCall[] = {0x8b, 0x4f, 0x68, 0xe8};
const uint8_t kDialogueLastPage[] = {0x8b, 0x41, 0x1c, 0x85, 0xc0, 0x75, 0x03, 0xb0, 0x01, 0xc3, 0x8b, 0x80, 0x28, 0x08,
                                     0x00, 0x00, 0x85, 0xc0, 0x75, 0x03, 0xb0, 0x01, 0xc3, 0x80, 0x38, 0x00, 0x0f, 0x94,
                                     0xc0, 0xc3};
const uint8_t kDialogueTimerHead[] = {0x56, 0x8b, 0xf1, 0xf6, 0x46, 0x20, 0x02, 0x74, 0x35, 0xe8};
const uint8_t kDialogueTimerCount[] = {0x8b, 0x4e, 0x28, 0x99, 0x2b, 0xc8, 0x89, 0x4e, 0x28, 0x8b, 0x4e, 0x2c, 0x1b,
                                       0xca};
const size_t kDialogueTimerAt = 19;

// The text-entry windows' input routines, never run; what each reply does is
// read out of them. A shape is the game's bytes with ?? holes, compared where
// the code holds them (shape_is) or found once within a span (shape_once);
// its holes are numbered in order.
//
// stringdl's routine hands the code to the slot's or the name's handler; with
// a reply written, it raises the outgoing box through the manager's focus.
// The name's handler commits on row 2 (the text into +0x58, +0x15C = 1) and
// writes +0x15C = 2 on its cancel; the slot's writes 3 and 4. The field's end
// clears +0x164 and takes the line from the field holding it.
const char kSigStringdlInput[] =
    "56 8b f1 0f be 46 14 48 74 16 48 75 24 8b 44 24 0c 8b 4c 24 08 50 51 8b ce e8 ?? ?? ?? ?? eb 11 8b 54 24 0c 8b 44 24 08 52 50 8b ce e8 ?? ?? ?? ?? 66 83 be 5c 01 00 00 00 5e 74 23";
const size_t kStringdlSlotCall = 0x19;
const size_t kStringdlNameCall = 0x2C;
const size_t kStringdlTailAt = 0x3C;
const size_t kStringdlHandlerBytes = 0x1A0;
const char kShapeStringdlTail[] =
    "8b 0d ?? ?? ?? ?? 8b 41 08 85 c0 74 16 c7 44 24 08 00 00 00 00 89 44 24 04 b9 ?? ?? ?? ?? e9 ?? ?? ?? ??";
const char kShapeStringdlNamed[] =
    "8a 86 64 01 00 00 84 c0 74 07 8b ce e8 ?? ?? ?? ?? 8b 8e 60 01 00 00 6a 00 e8 ?? ?? ?? ?? 8d 56 58 8a 08 40 88 0a 42 84 c9 75 f6 66 c7 86 5c 01 00 00 01 00";
const char kShapeStringdlStop[] =
    "c6 81 64 01 00 00 00 8b 0d ?? ?? ?? ?? e8 ?? ?? ?? ?? 8b 0d ?? ?? ?? ?? 6a 00 e8 ?? ?? ?? ?? c3";
const uint8_t kStringdlCancelWrite[] = {0x66, 0xc7, 0x86, 0x5c, 0x01, 0x00, 0x00, 0x02, 0x00};
const uint8_t kStringdlSlotReplies[2][9] = {{0x66, 0xc7, 0x81, 0x5c, 0x01, 0x00, 0x00, 0x03, 0x00},
                                             {0x66, 0xc7, 0x81, 0x5c, 0x01, 0x00, 0x00, 0x04, 0x00}};
const size_t kStringdlSlotBytes = 0x60;

// comment's Escape ends the field keeping the line, then closes the window by
// instance; its begin-close ends a field still open, keeping its line. The
// field's end takes its 40 bytes into line +0x14 (0..2) of message slot
// +0x16, then sanitizes and trims it. The field's start (this = the
// controller; the line) trims the line and writes it to +0x14.
const char kSigCommentInput[] =
    "0f bf 44 24 04 48 56 83 f8 11 8b f1 0f 87 ?? ?? ?? ?? 33 c9 8a 88 ?? ?? ?? ?? ff 24 8d ?? ?? ?? ?? 6a 01 8b ce e8 ?? ?? ?? ?? 8a 46 15 fe c8 88 46 15";
const size_t kCommentStopCall = 0x25;
const size_t kCommentInputBytes = 0x100;
const size_t kCommentStopBytes = 0xE4;
const size_t kCommentCloseSlot = 2;
const size_t kCommentOpenSlot = 1;
const size_t kCommentHookBytes = 0x80;
const size_t kCommentStartBytes = 0x104;
const char kShapeCommentEscape[] =
    "6a 01 8b ce e8 ?? ?? ?? ?? 8b 4e 08 6a 01 51 b9 ?? ?? ?? ?? e8 ?? ?? ?? ??";
const char kShapeCommentStop[] =
    "8a 44 24 04 56 84 c0 8b f1 0f 84 ?? ?? ?? ?? 8a 46 14 84 c0 0f 8c ?? ?? ?? ?? 3c 03 0f 8d ?? ?? ?? ?? 8b 8e 78 03 00 00 6a 28 6a 00 e8";
const char kShapeCommentSanitize[] =
    "0f be 46 16 0f be 4e 14 8d 04 40 03 c1 8d 14 80 03 c6 8d 44 d0 1a 50 50 e8 ?? ?? ?? ??";
const char kShapeCommentTrim[] =
    "8d 4c d0 1a 51 e8 ?? ?? ?? ?? 83 c4 18";
const char kShapeCommentClose[] =
    "81 ec 88 00 00 00 53 56 8b f1 33 db 39 9e 78 03 00 00 74 07 6a 01 e8 ?? ?? ?? ??";
const char kShapeCommentOpen[] =
    "6a 00 8b ce 8b 50 34 80 ce 10 89 50 34 c6 46 14 ff c6 46 15 00 e8 ?? ?? ?? ??";
const char kShapeCommentStart[] =
    "83 ec 08 53 8b 5c 24 10 55 33 ed 56 3b dd 8b f1";
const char kShapeCommentStartTrim[] =
    "8d 4c d0 1a 51 e8 ?? ?? ?? ?? 8b 86 78 03 00 00";
// mov [esi+0x14],bl
const uint8_t kCommentStartLine[] = {0x88, 0x5e, 0x14};

// msgline's commit sends nothing for an empty text or a NULL player data
// global, else the mode's case sends it. A sender's 0 marks the sending
// indicator and sets the busy byte before msgline's close by name; its Escape
// drops the text and closes the same way. The field's end takes its shown
// text into +0x53 (300 bytes) and zeroes +0x17E; the field is short in modes
// 2 and 7.
const char kSigMsglineInput[] =
    "0f bf 44 24 04 81 ec 44 01 00 00 83 c0 fd 83 f8 0f 53 56 57 8b f1 0f 87 ?? ?? ?? ?? 33 c9 8a 88 ?? ?? ?? ?? ff 24 8d ?? ?? ?? ?? 6a 01 8b ce e8 ?? ?? ?? ?? 8d 5e 53";
const size_t kMsglineStopCall = 0x2F;
const size_t kMsglineCommitAt = 0x34;
const size_t kMsglineInputBytes = 0x470;
const size_t kMsglineOpenSlot = 1;
const size_t kMsglineHookBytes = 0x90;
const size_t kMsglineFieldBytes = 0xB8;
const char kShapeMsglineCommit[] =
    "8d 5e 53 83 c9 ff 8b fb 33 c0 f2 ae f7 d1 49 0f 84 ?? ?? ?? ?? a1 ?? ?? ?? ?? 85 c0 0f 84 ?? ?? ?? ?? 0f bf 46 14 83 f8 07 0f 87 ?? ?? ?? ?? ff 24 85 ?? ?? ?? ??";
const char kShapeMsglineTell[] =
    "8b 15 ?? ?? ?? ?? 8b be 9c 01 00 00 55 6a 11 8b aa 80 00 00 00 68 ?? ?? ?? ?? 53 6a 1c 6a 09 e8 ?? ?? ?? ?? 8b 0d ?? ?? ?? ?? 83 c4 08 50 8d 86 88 01 00 00 50 55 e8 ?? ?? ?? ?? 33 c9 8b d7 8a e8 8b 86 98 01 00 00 81 e2 00 00 0f 00 81 e7 ff ff 00 00 0b ca c1 e1 08 0b cf 51 50 e8 ?? ?? ?? ?? 83 c4 1c";
const char kShapeMsglineReply[] =
    "6a 11 68 ?? ?? ?? ?? 53 6a 1c 6a 09 e8 ?? ?? ?? ?? 8b 8e d8 01 00 00 8b 96 d4 01 00 00 83 c4 08 50 8b 86 d0 01 00 00 51 52 50 e8 ?? ?? ?? ?? 83 c4 1c";
const char kShapeMsglineShort[] =
    "8b cb 8a 01 84 c0 74 0f 3c 07 75 03 c6 01 00 8a 41 01 41 84 c0 75 f1 8d 4c 24 50 6a 7f 51 6a 01 e8 ?? ?? ?? ?? 8b 0d ?? ?? ?? ?? 83 c4 0c 8b be 9c 01 00 00 8d 96 88 01 00 00 8b 81 80 00 00 00 6a 14 68 ?? ?? ?? ?? 52 8d 54 24 5c 52 53 50 e8 ?? ?? ?? ?? 33 c9 8b d7 8a e8 8b 86 98 01 00 00 81 e2 00 00 0f 00 81 e7 ff ff 00 00 0b ca c1 e1 08 0b cf 51 50 e8 ?? ?? ?? ?? 83 c4 1c";
const char kShapeMsglineAnswer[] =
    "6a 12 68 ?? ?? ?? ?? 53 6a 1f 6a 09 e8 ?? ?? ?? ?? 8b 96 d4 01 00 00 8b 8e e4 01 00 00 83 c4 08 50 8b 86 d0 01 00 00 52 8b 96 e0 01 00 00 50 51 52 e8 ?? ?? ?? ?? 83 c4 20";
const char kShapeMsglineDated[] =
    "8d 44 24 50 6a 7f 50 6a 01 e8 ?? ?? ?? ?? 8d 8c 24 dc 00 00 00 6a 7f 51 6a 09 e8 ?? ?? ?? ?? 8b 8e d4 01 00 00 6a 15 8d 54 24 6c 68 ?? ?? ?? ?? 8d 84 24 f0 00 00 00 52 8b 96 d0 01 00 00 50 8b 86 e4 01 00 00 53 51 8b 8e e0 01 00 00 52 50 51 e8 ?? ?? ?? ?? 83 c4 3c";
const char kShapeMsglineSent[] =
    "85 c0 75 1a a1 ?? ?? ?? ?? 85 c0 74 0a 6a 10 8d 48 20 e8 ?? ?? ?? ?? c6 05 ?? ?? ?? ?? 01 68 ?? ?? ?? ?? b9 ?? ?? ?? ?? e8 ?? ?? ?? ??";
const char kShapeMsglineCancel[] =
    "6a 00 8b ce e8 ?? ?? ?? ?? 68 ?? ?? ?? ?? b9 ?? ?? ?? ?? e8 ?? ?? ?? ??";
const char kShapeMsglineStop[] =
    "8a 44 24 04 56 84 c0 8b f1 74 23 8b 4e 1c 68 2c 01 00 00 6a 01 e8 ?? ?? ?? ?? 50 8d 46 53 50 e8 ?? ?? ?? ?? 83 c4 0c c6 86 7e 01 00 00 00";
const char kShapeMsglineStart[] =
    "66 89 56 18 e8 ?? ?? ?? ??";
const char kShapeMsglineField[] =
    "56 8b f1 66 8b 46 14 66 3d 02 00 74 45 66 3d 07 00 74 3f";
// push 0x12c; push 8 (the long field's size and type); push 0xf; push 7
const uint8_t kMsglineFieldLong[] = {0x68, 0x2c, 0x01, 0x00, 0x00, 0x6a, 0x08};
const uint8_t kMsglineFieldShort[] = {0x6a, 0x0f, 0x6a, 0x07};

// scname's routine works only while the player search's global is set. Row 2
// clears the search's criteria at +0x5C, names them from +0x18, starts the
// search, then closes scname and scsibori. The field's end takes its text
// into +0x18 when told to keep it.
const char kSigScnameInput[] =
    "a1 ?? ?? ?? ?? 57 85 c0 8b f9 0f 84 ?? ?? ?? ?? 0f bf 4c 24 08 83 c1 fd 83 f9 0e 0f 87 ?? ?? ?? ?? 33 d2 8a 91 ?? ?? ?? ?? ff 24 95 ?? ?? ?? ?? 66 83 7c 24 0c 01 0f 85 ?? ?? ?? ?? 6a 01";
const size_t kScnameSearchGlobal = 1;
const size_t kScnameStopCall = 0x40;
const size_t kScnameInputBytes = 0x120;
const char kShapeScnameSearch[] =
    "56 8d 70 5c 56 e8 ?? ?? ?? ?? 83 c7 18 57 56 e8 ?? ?? ?? ?? 83 c4 0c 8b 0d ?? ?? ?? ?? 6a 00 6a 00 6a 00 6a 01 56 6a 01 e8 ?? ?? ?? ?? 68 ?? ?? ?? ?? b9 ?? ?? ?? ?? e8 ?? ?? ?? ?? 68 ?? ?? ?? ?? b9 ?? ?? ?? ?? e8 ?? ?? ?? ??";
const char kShapeScnameStop[] =
    "56 8b f1 8b 4e 14 85 c9 74 33 8a 44 24 08 84 c0 74 14 6a 00 e8 ?? ?? ?? ?? 8d 56 18";

// link11's cancel with no field closes link11, opens link6 and raises link5's
// window. Row 5 buzzes on an empty name (+0x34), asks whether the second
// linkshell slot is taken, takes link5's selected row, encodes the name,
// sends the request with the colors +0x30..+0x32 and, queued, writes the
// item's word and the slot into the request's bytes and sets link5's +0x65
// bit 0. The field's end takes the name into +0x34 whatever it is told.
const char kSigLink11Input[] =
    "55 56 8b f1 57 8b 46 14 85 c0 74 14 8b 7e 08 6a 00 50 8b cf e8 ?? ?? ?? ?? 8b cf e8 ?? ?? ?? ?? 8b 46 14 66 8b 4c 24 10 83 f8 01 75 47";
const size_t kLink11StopCall = 0x69;
const size_t kLink11InputBytes = 0x2A0;
const char kShapeLink11Cancel[] =
    "68 ?? ?? ?? ?? b9 ?? ?? ?? ?? e8 ?? ?? ?? ?? 6a 00 6a 01 68 ?? ?? ?? ?? b9 ?? ?? ?? ?? e8 ?? ?? ?? ?? 8b 0d ?? ?? ?? ?? 5f 5e 5d 8b 51 08 c7 44 24 08 00 00 00 00 89 54 24 04 b9 ?? ?? ?? ?? e9 ?? ?? ?? ??";
const char kShapeLink11Create[] =
    "8a 46 34 8d 6e 34 84 c0 75 0b e8 ?? ?? ?? ?? 5f 5e 5d c2 08 00 53 6a 01 6a 00 6a 00 6a 00 e8 ?? ?? ?? ?? 8b 0d ?? ?? ?? ?? 83 c4 10 84 c0 0f 95 c3 fe c3 88 5c 24 14 e8 ?? ?? ?? ?? 8b f8 85 ff 74 76 8a 46 32 8a 4e 31 8a 56 30 50 51 52 55 e8 ?? ?? ?? ?? 8a 4f 01 8b 54 24 24 83 c4 04 50 8a 07 6a 01 50 51 52 e8 ?? ?? ?? ?? 83 c4 20 84 c0 74 41 33 c0 33 c9 8a 07 8a 4f 01 50 51 e8 ?? ?? ?? ?? 83 c4 08 85 c0 74 1b 8b 00 88 1d ?? ?? ?? ?? a3 ?? ?? ?? ?? c6 05 ?? ?? ?? ?? 01 c6 05 ?? ?? ?? ?? 3c a1 ?? ?? ?? ?? 85 c0 74 0b 80 48 65 01";
const char kShapeLink11Stop[] =
    "56 8b f1 8b 46 14 83 f8 01 74 18 7e 4f 83 f8 04 7f 4a 8a 4c 24 08 84 c9 75 42 8a 4e 33 88 4c 30 2e eb 39 8b 4e 2c 6a 00 e8 ?? ?? ?? ?? 8d 56 34";

// passinpu's field's end takes its text into +0x24 when told to keep it. The
// field's start, from its open hook, ends in the text entry manager's unbind,
// then its bind of the field (holes as kShapeStringdlStop's).
const char kSigPassinpuInput[] =
    "0f bf 44 24 04 83 c0 fd 56 83 f8 0e 8b f1 77 76 33 c9 8a 88 ?? ?? ?? ?? ff 24 8d ?? ?? ?? ?? 6a 01 8b ce e8 ?? ?? ?? ?? 8b 46 1c 85 c0 74 0d";
const size_t kPassinpuStopCall = 0x23;
const size_t kPassinpuOpenSlot = 1;
const char kShapePassinpuOpen[] =
    "56 8b f1 6a 01 6a 10 8b 4e 08 e8 ?? ?? ?? ?? 8b 46 14 8b 4e 08 85 c0 75 19 6a 01 e8 ?? ?? ?? ?? 8b ce e8 ?? ?? ?? ??";
const char kShapePassinpuStop[] =
    "8a 44 24 04 56 84 c0 8b f1 74 17 8b 4e 38 6a 00 e8 ?? ?? ?? ?? 8d 56 24";
const size_t kPassinpuBindAt = 0x7D;
const char kShapePassinpuBind[] =
    "8b 0d ?? ?? ?? ?? e8 ?? ?? ?? ?? 8b 56 38 8b 0d ?? ?? ?? ?? 52 e8 ?? ?? ?? ?? 5f 5e c3";

// The key test (cdecl; 0x3E, action, query, slot; al), hooked for the
// dialogue boxes' left-button test. It has no signature: it is the routine
// the UI update calls at kUpdateKeyTestCall. Its prologue reads the input
// mute byte, whose imm32 is read out of the routine (on a patched site, the
// daemon's saved original); kKeyTestBody follows, nothing relative before
// its jne.
const size_t kUpdateKeyTestCall = 20;
const uint8_t kKeyTestHead[] = {0x51, 0xa0};
const uint8_t kKeyTestBody[] = {0x56, 0x8b, 0x74, 0x24, 0x14, 0x3c, 0x01, 0x57, 0x75, 0x09};
const size_t kKeyTestPrologue = 6;

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
    kCallSetFrameRect,
    kCallSelectRow,
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
    {"set_frame_rect", kSigSetFrameRect},
    {"select_row", kSigSelectRow},
};

// The pending-invite flag is the byte prtyjoin_clear writes: its imm32.
const size_t kPartyClearFlag = 2;

const uint32_t kDockMaskTable[5] = {0x1000, 0x2000, 0x4000, 0x10000000, 0};

// The prologue is the run of whole, position-independent instructions the
// daemon relocates, inside the signature's exact bytes but for `manager_imm`,
// an imm32 the engine writes with the manager's address before it scans.
// `callee_pops`: 1 for thiscall and stdcall, 0 for cdecl. A site with no
// signature (the key test) is found from another, its prologue read out of
// the routine.
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
    kSiteMacroGate, kSiteAbilityOpen, kSiteKeyTest, kSiteTemplateSwap, kSiteMenuDraw, kSiteSpriteDraw,
    kSitePrimitiveDraw,
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
    {"key_test", NULL, kKeyTestPrologue, 16, 0, &hook_key_test_pre, NULL, 0},
    {"template_swap", kSigTemplateSwap, 5, 4, 1, &hook_swap_pre, &hook_swap_post, 0},
    {"menu_draw", kSigMenuDraw, 5, 0, 1, &hook_menu_draw_pre, &hook_menu_draw_post, 0},
    {"sprite_draw", kSigSpriteDraw, 5, 32, 1, &hook_sprite_pre, &hook_sprite_post, 0},
    {"primitive_draw", kSigPrimitiveDraw, 7, 16, 1, &hook_primitive_pre, NULL, 0},
};

// What resolve_asked() and resolve_dialogue() take from the resolution before
// them; `message` is the game's message getter.
const int kDataSections = 4;

struct AskedInputs {
    const uint8_t* text;
    size_t size;
    const uint8_t* data[kDataSections];
    size_t data_size[kDataSections];
    int data_count;
    const Inventory* inv;
    const uint8_t* registry;
    const uint8_t* mcb;
    const uint8_t* open;
    const uint8_t* close;
    const uint8_t* message;
    const uint8_t* key_test;
    const uint8_t* cancel_allowed;
};

// The one place a signature matches, or NULL with `why` filled.
inline const uint8_t* asked_find(const AskedInputs& in, const char* name, const char* signature, char* why,
                                 size_t len) {
    uint8_t bytes[64];
    char mask[65];
    if (!parse_signature(signature, bytes, mask, sizeof(bytes))) {
        snprintf(why, len, "signature %s: malformed", name);
        return NULL;
    }
    ScanResult r;
    scan(in.text, in.size, bytes, mask, false, &r);
    if (r.count != 1) {
        if (r.count == 0) {
            snprintf(why, len, "signature %s: not found (client patched?)", name);
        } else {
            snprintf(why, len, "signature %s: matched %d times", name, r.count);
        }
        return NULL;
    }
    return r.hits[0];
}

inline const uint8_t* asked_call(const uint8_t* at) {
    int32_t rel;
    memcpy(&rel, at + 1, 4);
    return at + 5 + rel;
}

// The routine a call or jmp at `at` reaches when it is in the image's code.
inline const uint8_t* asked_target(const AskedInputs& in, const uint8_t* at, uint8_t opcode) {
    if (at[0] != opcode) {
        return NULL;
    }
    const uint8_t* to = asked_call(at);
    return to >= in.text && to < in.text + in.size ? to : NULL;
}

inline bool asked_bytes(const AskedInputs& in, const uint8_t* at, const uint8_t* bytes, size_t length) {
    return at >= in.text && at + length <= in.text + in.size && memcmp(at, bytes, length) == 0;
}

// How often `bytes` occur in the first `span` bytes from `at`; the last one
// in *last.
inline int asked_count(const AskedInputs& in, const uint8_t* at, size_t span, const uint8_t* bytes, size_t length,
                       const uint8_t** last) {
    const size_t left = static_cast<size_t>(in.text + in.size - at);
    const size_t end = left < span ? left : span;
    int found = 0;
    for (size_t i = 0; i + length <= end; ++i) {
        if (memcmp(at + i, bytes, length) == 0) {
            *last = at + i;
            ++found;
        }
    }
    return found;
}

// Window nm's registry key, and the global holding its controller.
inline const char* asked_key(const AskedInputs& in, const char* nm) {
    const int n = in.inv->find_exact(nm);
    return n >= 0 ? in.inv->names[n].key : NULL;
}

inline const uint8_t* asked_slot(const AskedInputs& in, const char* nm) {
    const int n = in.inv->find_exact(nm);
    if (n < 0 || in.inv->names[n].row_count == 0) {
        return NULL;
    }
    return rdptr(in.registry + in.inv->names[n].rows[0] * kRowStride, kRowSlot);
}

// The imm32 at `at` names window nm's key.
inline bool asked_names_key(const AskedInputs& in, const uint8_t* at, const char* nm) {
    const uint8_t* key = rdptr(at, 0);
    const char* own = asked_key(in, nm);
    return own && readable(key, kKeyLen) && memcmp(key, own, kKeyLen) == 0;
}

// Bytes inside the image's code, as a jump table there is.
inline bool asked_code(const AskedInputs& in, const void* p, size_t bytes) {
    const uint8_t* at = static_cast<const uint8_t*>(p);
    return at >= in.text && at + bytes <= in.text + in.size;
}

// Data in the image outside its code.
inline bool asked_data(const AskedInputs& in, const void* p, size_t bytes) {
    const uint8_t* at = static_cast<const uint8_t*>(p);
    return readable(at, bytes) && (at + bytes <= in.text || at >= in.text + in.size);
}

// The vtables in the image's read-only data naming `routine` at `slot`,
// into `out`, at most `max`; the count found, past `max` too. A vtable is
// read as dwords on 4-byte bounds, `slot` dwords into its section at least.
inline int asked_vtables(const AskedInputs& in, const uint8_t* routine, size_t slot, const uint8_t** out, int max) {
    int found = 0;
    for (int k = 0; k < in.data_count; ++k) {
        const uint8_t* base = in.data[k];
        for (size_t at = 4 * slot; at + 4 <= in.data_size[k]; at += 4) {
            if (rdptr(base, at) == routine) {
                if (found < max) {
                    out[found] = base + at - 4 * slot;
                }
                ++found;
            }
        }
    }
    return found;
}

// A vtable's routine at `slot`, when the slot lies in the data section that
// holds the vtable and the routine in the image's code.
inline const uint8_t* asked_slot_routine(const AskedInputs& in, const uint8_t* vtable, size_t slot) {
    for (int k = 0; k < in.data_count; ++k) {
        const uint8_t* base = in.data[k];
        if (vtable >= base && vtable < base + in.data_size[k]) {
            if (static_cast<size_t>(vtable - base) + 4 * (slot + 1) > in.data_size[k]) {
                return NULL;
            }
            const uint8_t* fn = rdptr(vtable, 4 * slot);
            return fn >= in.text && fn < in.text + in.size ? fn : NULL;
        }
    }
    return NULL;
}

// `bytes` once within the first `span` bytes from `at`, into *found.
inline bool asked_once(const AskedInputs& in, const uint8_t* at, size_t span, const uint8_t* bytes, size_t length,
                       const uint8_t** found) {
    *found = NULL;
    return at && asked_count(in, at, span, bytes, length, found) == 1;
}

// mogdoor's input routine, the door's handler and, through its vtable, its
// open hook, tick and draw; `data` the player data's global (bankmenu's).
inline bool resolve_door(const AskedInputs& in, const uint8_t* close_instance, uint8_t** data, DoorSites* out,
                         char* why, size_t len) {
    DoorSites s;
    memset(&s, 0, sizeof(s));
    s.input = asked_find(in, "door_input", kSigDoorInput, why, len);
    if (!s.input) {
        return false;
    }
    const uint8_t* m = s.input;
    const uint8_t* floor = asked_target(in, m + kDoorFloorCall, 0xE8);
    const uint8_t* gate = asked_target(in, m + kDoorGateCall, 0xE8);
    const uint8_t* codes = rdptr(m, kDoorCodeTable);
    if (!floor || !gate || !asked_code(in, codes, 9 * 4) || rdptr(codes, 4 * 4) != m + kDoorConfirmCase
        || floor[0] != 0xa1 || rdptr(floor, 1) != reinterpret_cast<const uint8_t*>(data)
        || !asked_bytes(in, floor + 5, kDoorFloorMid, sizeof(kDoorFloorMid))
        || !asked_bytes(in, floor + 5 + sizeof(kDoorFloorMid) + 4, kDoorFloorTail, sizeof(kDoorFloorTail))) {
        snprintf(why, len, "door_input: its confirm is not code 5's case, or the floor it reads is not a word of the"
            " player's data (client patched?)");
        return false;
    }
    s.floor_word = rd32(floor, 5 + sizeof(kDoorFloorMid));
    s.gate_first = static_cast<int8_t>(m[kDoorGateFirst]);
    s.gate_second = static_cast<int8_t>(m[kDoorGateSecond]);
    s.garden = static_cast<uint16_t>(rd16(m, kDoorAreaRowAt + sizeof(kDoorAreaRow)));
    const uint8_t* rows = rdptr(m, kDoorTopRowAt + sizeof(kDoorTopRow));
    if (!asked_bytes(in, m + kDoorAreaRowAt, kDoorAreaRow, sizeof(kDoorAreaRow))
        || !asked_bytes(in, m + kDoorGardenRowAt, kDoorGardenRow, sizeof(kDoorGardenRow))
        || !asked_bytes(in, m + kDoorAreaExitAt, kDoorAreaExit, sizeof(kDoorAreaExit))
        || m[kDoorAreaExitAt + 3] != kDoorGardenExit
        || !asked_bytes(in, m + kDoorTopRowAt, kDoorTopRow, sizeof(kDoorTopRow)) || !asked_code(in, rows, 4 * 4)) {
        snprintf(why, len, "door_input: its rows of the list of areas or of the top are not the ones the engine"
            " answers (client patched?)");
        return false;
    }
    const uint8_t* stay = rdptr(rows, 0);
    const uint8_t* back = rdptr(rows, 4);
    const uint8_t* floor_row = rdptr(rows, 8);
    const uint8_t* list_row = rdptr(rows, 12);
    const size_t stay_close = sizeof(kDoorStayRow) + 4;
    const size_t floor_read = sizeof(kDoorFloorRow) + 4;
    const size_t floor_exit = floor_read + sizeof(kDoorFloorRowRead) + 4;
    if (!asked_bytes(in, stay, kDoorStayRow, sizeof(kDoorStayRow)) || rdptr(stay, sizeof(kDoorStayRow)) != in.mcb
        || !asked_bytes(in, stay + stay_close, kDoorStayClose, sizeof(kDoorStayClose))
        || asked_target(in, stay + stay_close + sizeof(kDoorStayClose) - 1, 0xE8) != close_instance
        || !asked_bytes(in, back, kDoorBackRow, sizeof(kDoorBackRow))
        || !asked_bytes(in, floor_row, kDoorFloorRow, sizeof(kDoorFloorRow))
        || !asked_bytes(in, floor_row + floor_read, kDoorFloorRowRead, sizeof(kDoorFloorRowRead))
        || asked_target(in, floor_row + floor_read + sizeof(kDoorFloorRowRead) - 1, 0xE8) != floor
        || !asked_bytes(in, floor_row + floor_exit, kDoorFloorRowExit, sizeof(kDoorFloorRowExit))
        || !asked_bytes(in, list_row, kDoorListRow, sizeof(kDoorListRow))) {
        snprintf(why, len, "door_input: its top's rows do not stay, go back, change floors and open the list of areas"
            " as the engine answers them (client patched?)");
        return false;
    }
    const uint8_t* tail = NULL;
    if (!asked_once(in, m, kDoorInputBytes, kDoorTail, sizeof(kDoorTail), &tail) || tail[kDoorTailHouse - 1] != 0xa1
        || !asked_bytes(in, tail + kDoorTailActorAt, kDoorTailActor, sizeof(kDoorTailActor))
        || !asked_bytes(in, tail + kDoorTailHouse2 - sizeof(kDoorTailHouseRead), kDoorTailHouseRead,
            sizeof(kDoorTailHouseRead))
        || rd32(tail, kDoorTailHouse2) != rd32(tail, kDoorTailHouse)
        || !asked_bytes(in, tail + kDoorTailOpenAt, kDoorTailOpenCall, sizeof(kDoorTailOpenCall))
        || !asked_bytes(in, tail + kDoorTailEndAt, kDoorTailEnd, sizeof(kDoorTailEnd))) {
        snprintf(why, len, "door_input: the exit after its rows is not the one the engine makes (client patched?)");
        return false;
    }
    for (int i = 0; i < 3; ++i) {
        if (!asked_bytes(in, tail + kDoorTailBytes[i], kDoorTailByte, sizeof(kDoorTailByte))) {
            snprintf(why, len, "door_input: the exit does not set the three bytes the engine sets (client patched?)");
            return false;
        }
        s.exiting[i] = const_cast<uint8_t*>(rdptr(tail, kDoorTailBytes[i] + sizeof(kDoorTailByte)));
    }
    s.house = reinterpret_cast<uint8_t**>(const_cast<uint8_t*>(rdptr(tail, kDoorTailHouse)));
    s.opened = rdptr(tail, kDoorTailOpened);
    const uint8_t* open_door = asked_target(in, tail + kDoorTailOpen, 0xE8);
    const uint8_t* opened = static_cast<const uint8_t*>(s.opened);
    if (!open_door || !asked_data(in, s.house, 4) || !asked_data(in, s.exiting[0], 1)
        || !asked_data(in, s.exiting[1], 1) || !asked_data(in, s.exiting[2], 1)
        || !asked_bytes(in, opened, kDoorOpenedHead, sizeof(kDoorOpenedHead))
        || !asked_bytes(in, opened + 7, kDoorOpenedActor, sizeof(kDoorOpenedActor))
        || !asked_bytes(in, opened + 14, kDoorOpenedTest, sizeof(kDoorOpenedTest))
        || rdptr(opened, 14 + sizeof(kDoorOpenedTest)) != asked_slot(in, "mogdoor")
        || !asked_bytes(in, opened + 24, kDoorOpenedSend, sizeof(kDoorOpenedSend))) {
        snprintf(why, len, "door_input: the routine its exit gives the door does not send mogdoor's exit, or its"
            " globals are not data (client patched?)");
        return false;
    }

    const uint8_t* tables[2];
    if (asked_vtables(in, m, kDoorInputSlot, tables, 2) != 1) {
        snprintf(why, len, "door_input: not one vtable in the image names it");
        return false;
    }
    s.vtable = tables[0];
    s.open_hook = asked_slot_routine(in, tables[0], kDoorOpenSlot);
    s.draw = asked_slot_routine(in, tables[0], kDoorDrawSlot);
    const uint8_t* tick = asked_slot_routine(in, tables[0], kDoorTickSlot);
    const uint8_t* o = s.open_hook;
    const uint8_t* top = NULL;
    if (!o || o[0] != 0xa1 || rdptr(o, 1) != reinterpret_cast<const uint8_t*>(data)
        || !asked_bytes(in, o + kDoorOpenRangeAt, kDoorOpenRange, sizeof(kDoorOpenRange))
        || !asked_bytes(in, o + kDoorOpenListAt, kDoorOpenList, sizeof(kDoorOpenList))
        || !asked_bytes(in, o + kDoorOpenCountAt, kDoorOpenCount, sizeof(kDoorOpenCount))
        || !asked_once(in, o, kDoorOpenBytes, kDoorOpenTop, sizeof(kDoorOpenTop), &top)) {
        snprintf(why, len, "door_open: mogdoor's open hook does not take a nation's list of areas from a table and"
            " show the top (client patched?)");
        return false;
    }
    s.nations = o[kDoorOpenNations];
    s.table = rdptr(o, kDoorOpenTable);
    if (s.nations < 1 || !asked_data(in, s.table, 4 * (static_cast<size_t>(s.nations) + 1))) {
        snprintf(why, len, "door_open: its table of lists 0x%08X is not data",
            static_cast<unsigned>(reinterpret_cast<uintptr_t>(s.table)));
        return false;
    }
    const uint8_t* at = NULL;
    if (!tick || !asked_bytes(in, tick, kDoorTickHead, sizeof(kDoorTickHead))
        || asked_count(in, tick, kDoorTickBytes, kDoorTickGarden, sizeof(kDoorTickGarden), &at) != 1
        || asked_count(in, tick, kDoorTickBytes, kDoorTickFloor, sizeof(kDoorTickFloor), &at) != 2) {
        snprintf(why, len, "door_tick: mogdoor's tick does not enable its Mog Garden and floor rows (client"
            " patched?)");
        return false;
    }
    const uint8_t* condition_at = NULL;
    const uint8_t* condition = s.draw && asked_once(in, s.draw, kDoorDrawBytes, kDoorDrawCondition,
        sizeof(kDoorDrawCondition), &condition_at) ? asked_target(in, condition_at - 5, 0xE8) : NULL;
    const struct {
        const uint8_t* bytes;
        size_t length;
        bool gets;
    } texts[] = {
        {kDoorDrawTitle, sizeof(kDoorDrawTitle), true},
        {kDoorDrawStay, sizeof(kDoorDrawStay), true},
        {kDoorDrawBack, sizeof(kDoorDrawBack), false},
        {kDoorDrawFloor, sizeof(kDoorDrawFloor), true},
        {kDoorDrawList, sizeof(kDoorDrawList), true},
        {kDoorDrawArea, sizeof(kDoorDrawArea), true},
    };
    bool drawn = condition != NULL;
    for (size_t i = 0; drawn && i < sizeof(texts) / sizeof(texts[0]); ++i) {
        drawn = asked_once(in, s.draw, kDoorDrawBytes, texts[i].bytes, texts[i].length, &at)
            && (!texts[i].gets || asked_target(in, at + texts[i].length - 1, 0xE8) == in.message);
    }
    if (!drawn) {
        snprintf(why, len, "door_draw: mogdoor's draw does not show the texts the engine reads, through the game's"
            " message getter (client patched?)");
        return false;
    }

    const uint8_t* handler = asked_find(in, "door_handler", kSigDoorHandler, why, len);
    if (!handler) {
        return false;
    }
    if (rdptr(handler, kDoorHandlerGlobal) != asked_slot(in, "mogdoor")
        || !asked_names_key(in, handler + kDoorHandlerKey, "mogdoor") || rdptr(handler, kDoorHandlerManager) != in.mcb
        || asked_target(in, handler + kDoorHandlerOpen, 0xE8) != in.open) {
        snprintf(why, len, "door_handler: it does not write the nation on mogdoor's controller, then open mogdoor's"
            " key through the menu manager's open routine");
        return false;
    }
    s.handler = handler;
    memcpy(&s.gate, &gate, sizeof(s.gate));
    memcpy(&s.floor, &floor, sizeof(s.floor));
    memcpy(&s.condition, &condition, sizeof(s.condition));
    memcpy(&s.tick, &tick, sizeof(s.tick));
    memcpy(&s.open_door, &open_door, sizeof(s.open_door));
    *out = s;
    return true;
}

// The Asked windows' signatures and every check made on what they hold;
// `close_instance` the routine close by name ends in.
inline bool resolve_asked(const AskedInputs& in, const uint8_t* close_instance, AskedSites* out, char* why,
                          size_t len) {
    AskedSites s;
    memset(&s, 0, sizeof(s));

    s.sortyn_setup = asked_find(in, "sortyn_setup", kSigSortynSetup, why, len);
    if (!s.sortyn_setup) {
        return false;
    }
    if (!asked_names_key(in, s.sortyn_setup + kSortynSetupKey, "sortyn")
        || rdptr(s.sortyn_setup, kSortynSetupManager) != in.mcb
        || asked_target(in, s.sortyn_setup + kSortynSetupOpen, 0xE8) != in.open) {
        snprintf(why, len, "sortyn_setup: it does not open sortyn's key through the menu manager's open routine");
        return false;
    }
    s.sortyn_confirm = asked_find(in, "sortyn_confirm", kSigSortynConfirm, why, len);
    if (!s.sortyn_confirm) {
        return false;
    }
    if (!asked_names_key(in, s.sortyn_confirm + kSortynConfirmKey, "sortyn")
        || rdptr(s.sortyn_confirm, kSortynConfirmManager) != in.mcb
        || asked_target(in, s.sortyn_confirm + kSortynConfirmClose, 0xE8) != in.close) {
        snprintf(why, len, "sortyn_confirm: it does not close sortyn's key through close by name");
        return false;
    }

    s.evitem_ask = asked_find(in, "evitem_ask", kSigEvitemAsk, why, len);
    if (!s.evitem_ask) {
        return false;
    }
    if (!asked_names_key(in, s.evitem_ask + kEvitemAskKey, "evitem01")
        || rdptr(s.evitem_ask, kEvitemAskManager) != in.mcb
        || asked_target(in, s.evitem_ask + kEvitemAskOpen, 0xE8) != in.open
        || rdptr(s.evitem_ask, kEvitemAskGlobal) != asked_slot(in, "evitem01")) {
        snprintf(why, len, "evitem_ask: it does not open evitem01's key, then ask through evitem01's registry slot");
        return false;
    }
    s.evitem_setup = asked_target(in, s.evitem_ask + kEvitemAskSetup, 0xE8);
    const size_t pushed = kEvitemSetupPushAt + sizeof(kEvitemSetupPush);
    if (!s.evitem_setup || !asked_bytes(in, s.evitem_setup, kEvitemSetupHead, sizeof(kEvitemSetupHead))
        || !asked_bytes(in, s.evitem_setup + kEvitemSetupPushAt, kEvitemSetupPush, sizeof(kEvitemSetupPush))
        || s.evitem_setup[pushed + 4] != 0xE8) {
        snprintf(why, len, "evitem_ask: the setup it calls does not write the question and push its callback"
            " (client patched?)");
        return false;
    }
    const uint8_t* evitem_cb = rdptr(s.evitem_setup, pushed);
    if (!asked_bytes(in, evitem_cb, kEvitemCallbackHead, sizeof(kEvitemCallbackHead))
        || rdptr(evitem_cb, sizeof(kEvitemCallbackHead)) != asked_slot(in, "evitem")) {
        snprintf(why, len, "evitem_ask: the callback its setup sets does not read the registry's evitem slot");
        return false;
    }
    memcpy(&s.evitem_callback, &evitem_cb, sizeof(s.evitem_callback));
    s.evitem_input = asked_find(in, "evitem_input", kSigEvitemInput, why, len);
    if (!s.evitem_input) {
        return false;
    }
    const uint8_t* close_tail = s.evitem_input + kEvitemCloseAt;
    if (!asked_bytes(in, close_tail, kEvitemCloseTail, sizeof(kEvitemCloseTail))
        || rdptr(close_tail, sizeof(kEvitemCloseTail)) != in.mcb
        || asked_target(in, close_tail + sizeof(kEvitemCloseTail) + 4, 0xE8) != close_instance) {
        snprintf(why, len, "evitem_input: it does not close its window by instance after the callback");
        return false;
    }

    s.confyn_input = asked_find(in, "confyn_input", kSigConfynInput, why, len);
    if (!s.confyn_input) {
        return false;
    }
    const uint8_t* restore = asked_target(in, s.confyn_input + kConfynRestore, 0xE8);
    if (!restore || rdptr(s.confyn_input, kConfynManager) != in.mcb
        || asked_target(in, s.confyn_input + kConfynClose, 0xE8) != close_instance) {
        snprintf(why, len, "confyn_input: it does not restore through a routine of the game's and close by instance");
        return false;
    }
    memcpy(&s.confyn_restore, &restore, sizeof(s.confyn_restore));

    s.bank_open = asked_find(in, "bank_open", kSigBankOpen, why, len);
    if (!s.bank_open) {
        return false;
    }
    const uint8_t* ask = asked_target(in, s.bank_open + kBankAsk, 0xE8);
    const uint8_t* flag = ask ? asked_target(in, ask + sizeof(kBankAskHead) - 1, 0xE8) : NULL;
    const uint8_t* data = rdptr(s.bank_open, kBankData);
    const uint8_t* clear = asked_target(in, s.bank_open + kBankBeginClose, 0xE9);
    if (!ask || !asked_bytes(in, ask, kBankAskHead, sizeof(kBankAskHead))
        || !asked_bytes(in, ask + sizeof(kBankAskHead) + 4, kBankAskTail, sizeof(kBankAskTail)) || !flag
        || !asked_bytes(in, flag, kBankFlagHead, sizeof(kBankFlagHead))
        || rdptr(flag, sizeof(kBankFlagHead)) != data
        || !asked_bytes(in, flag + sizeof(kBankFlagHead) + 4, kBankFlagMid, sizeof(kBankFlagMid))
        || !asked_bytes(in, flag + sizeof(kBankFlagHead) + 8 + sizeof(kBankFlagMid), kBankFlagTail,
            sizeof(kBankFlagTail))) {
        snprintf(why, len, "bank_open: its test of Mog Safe 2 is not a read of a flag in the player's data"
            " (client patched?)");
        return false;
    }
    if (!asked_data(in, data, 4)) {
        snprintf(why, len, "bank_open: the player data's global 0x%08X is not readable data",
            static_cast<unsigned>(reinterpret_cast<uintptr_t>(data)));
        return false;
    }
    if (!clear || !asked_bytes(in, clear, kBankClear, sizeof(kBankClear))) {
        snprintf(why, len, "bank_open: bankmenu's begin-close does not clear its callback (client patched?)");
        return false;
    }
    s.bank_data = reinterpret_cast<uint8_t**>(const_cast<uint8_t*>(data));
    s.bank_safe2 = rd32(flag, sizeof(kBankFlagHead) + 4 + sizeof(kBankFlagMid));
    memcpy(&s.bank_clear, &clear, sizeof(s.bank_clear));
    s.bank_input = asked_find(in, "bank_input", kSigBankInput, why, len);
    if (!s.bank_input) {
        return false;
    }
    const uint8_t* bank_close = asked_target(in, s.bank_input + kBankInputClose, 0xE8);
    if (!bank_close || !asked_bytes(in, bank_close, kBankClose, sizeof(kBankClose))) {
        snprintf(why, len, "bank_input: it does not close the window through its class's close (client patched?)");
        return false;
    }

    const uint8_t* maplist = asked_find(in, "maplist_close", kSigMaplistClose, why, len);
    if (!maplist) {
        return false;
    }
    if (rdptr(maplist, kMaplistGlobal) != asked_slot(in, "maplist")) {
        snprintf(why, len, "maplist_close: the key handler after it does not read the registry's maplist slot");
        return false;
    }
    const uint8_t* handler = maplist + kMaplistHandler;
    const struct {
        const uint8_t* bytes;
        size_t length;
        bool closes;
        const char* what;
    } paths[] = {
        {kMaplistLeaf, sizeof(kMaplistLeaf), true, "confirm of a leaf row"},
        {kMaplistCloseRow, sizeof(kMaplistCloseRow), true, "confirm of its closing row"},
        {kMaplistTopCancel, sizeof(kMaplistTopCancel), true, "cancel at the top"},
        {kMaplistSubLevel, sizeof(kMaplistSubLevel), false, "cancel's test of the list it shows"},
        {kMaplistDescend, sizeof(kMaplistDescend), false, "confirm of a row with a list of its own"},
        {kMaplistBackMaps, sizeof(kMaplistBackMaps), false, "cancel inside a zone's maps"},
        {kMaplistBackRegion, sizeof(kMaplistBackRegion), false, "cancel inside a region"},
    };
    const size_t path_count = sizeof(paths) / sizeof(paths[0]);
    const uint8_t* path_at[path_count];
    for (size_t i = 0; i < path_count; ++i) {
        const uint8_t* at = NULL;
        const int found = asked_count(in, handler, kMaplistHandlerBytes, paths[i].bytes, paths[i].length, &at);
        if (found != 1 || (paths[i].closes && asked_target(in, at + paths[i].length - 1, 0xE8) != maplist)) {
            snprintf(why, len, "maplist_close: its key handler's %s is %s", paths[i].what,
                found > 1 ? "there more than once" : "not there (client patched?)");
            return false;
        }
        path_at[i] = at;
    }
    if (maplist[kMaplistData - 1] != 0xA1
        || rdptr(maplist, kMaplistData) != reinterpret_cast<const uint8_t*>(s.bank_data)) {
        snprintf(why, len, "maplist_close: its key handler does not need the player data's global first");
        return false;
    }
    // The last three paths: the step down, then the two steps back.
    const uint8_t* descend = path_at[path_count - 3];
    const uint8_t* keep = descend + sizeof(kMaplistDescend) + 4;
    const uint8_t* first = keep + sizeof(kMaplistDescendKeep) + 4;
    const uint8_t* cursor = asked_target(in, descend + sizeof(kMaplistDescend) - 1, 0xE8);
    const uint8_t* rebuild = asked_bytes(in, keep, kMaplistDescendKeep, sizeof(kMaplistDescendKeep))
        ? asked_target(in, keep + sizeof(kMaplistDescendKeep) - 1, 0xE8) : NULL;
    const uint8_t* select = asked_bytes(in, first, kMaplistDescendSelect, sizeof(kMaplistDescendSelect))
        ? asked_target(in, first + sizeof(kMaplistDescendSelect) - 1, 0xE8) : NULL;
    bool steps = cursor && rebuild && select;
    for (int i = 0; i < 2 && steps; ++i) {
        const uint8_t* back = path_at[path_count - 2 + i];
        const size_t length = paths[path_count - 2 + i].length;
        const uint8_t* kept = back + length + 4;
        steps = asked_target(in, back + length - 1, 0xE8) == rebuild
            && asked_bytes(in, kept, kMaplistBackSelect[i], sizeof(kMaplistBackSelect[i]))
            && asked_target(in, kept + sizeof(kMaplistBackSelect[i]) - 1, 0xE8) == select;
    }
    if (!steps) {
        snprintf(why, len, "maplist_close: its key handler's steps between levels do not rebuild the list through"
            " one routine and select a row through another (client patched?)");
        return false;
    }
    const uint8_t* set_rows = asked_target(in, maplist + kMaplistSetRows, 0xE8);
    const uint8_t* free_rows = asked_target(in, maplist + kMaplistFreeRows, 0xE9);
    const uint8_t* freed = rebuild + sizeof(kMaplistRebuildHead) + 4;
    if (!set_rows || !free_rows || !asked_bytes(in, rebuild, kMaplistRebuildHead, sizeof(kMaplistRebuildHead))
        || asked_target(in, rebuild + sizeof(kMaplistRebuildHead) - 1, 0xE8) != set_rows
        || !asked_bytes(in, freed, kMaplistRebuildFree, sizeof(kMaplistRebuildFree))
        || asked_target(in, freed + sizeof(kMaplistRebuildFree) - 1, 0xE8) != free_rows) {
        snprintf(why, len, "maplist_close: the rebuild its steps between levels call does not empty and free the rows"
            " as close+reset does (client patched?)");
        return false;
    }
    if (!asked_bytes(in, cursor, kMaplistCursor, sizeof(kMaplistCursor))) {
        snprintf(why, len, "maplist_close: the routine its step down keeps the row under the cursor with is not the"
            " list's cursor read (client patched?)");
        return false;
    }
    memcpy(&s.maplist_rebuild, &rebuild, sizeof(s.maplist_rebuild));
    memcpy(&s.maplist_select, &select, sizeof(s.maplist_select));
    memcpy(&s.maplist_cursor, &cursor, sizeof(s.maplist_cursor));
    memcpy(&s.maplist_close, &maplist, sizeof(s.maplist_close));

    s.map0_input = asked_find(in, "map0_input", kSigMap0Input, why, len);
    if (!s.map0_input) {
        return false;
    }
    const uint8_t* map_close = asked_target(in, s.map0_input + kMap0MapClose, 0xE8);
    if (!map_close || rdptr(s.map0_input, kMap0Manager) != in.mcb
        || asked_target(in, s.map0_input + kMap0CloseInstance, 0xE8) != close_instance
        || rdptr(s.map0_input, kMap0Frame) != asked_slot(in, "mapframe")) {
        snprintf(why, len, "map0_input: its cancel does not close map0 by instance, then call a routine on the"
            " controller in mapframe's registry slot");
        return false;
    }
    const uint8_t* closes = map_close + kMapClosesAt;
    bool shaped = asked_bytes(in, map_close, kMapCloseHead, sizeof(kMapCloseHead))
        && asked_bytes(in, closes + kMapWindowCount * kMapClose, kMapClosesTail, sizeof(kMapClosesTail));
    for (int i = 0; i < kMapWindowCount && shaped; ++i) {
        const uint8_t* at = closes + i * kMapClose;
        const size_t key = i ? 1 : 6;
        const size_t manager = i ? 6 : 1;
        shaped = at[key - 1] == 0x68 && at[manager - 1] == 0xB9 && asked_names_key(in, at + key, kMapWindows[i])
            && rdptr(at, manager) == in.mcb && asked_target(in, at + 10, 0xE8) == in.close;
    }
    if (!shaped) {
        snprintf(why, len, "map0_input: the routine its cancel calls is not the map's close: map0, mapv3, mapv2,"
            " scanlist and mapscan closed by name, then the frame's fade-out (client patched?)");
        return false;
    }
    memcpy(&s.map_close, &map_close, sizeof(s.map_close));

    s.gm_poll = asked_find(in, "gmtell_poll", kSigGmTellPoll, why, len);
    if (!s.gm_poll) {
        return false;
    }
    const uint8_t* read = asked_target(in, s.gm_poll + kGmTellRead, 0xE8);
    const uint8_t* title = s.gm_poll + kGmTellTitleAt;
    const uint8_t* slot = rdptr(s.gm_poll, kGmTellSlot);
    const uint8_t* queue = asked_bytes(in, title, kGmTellTitleRead, sizeof(kGmTellTitleRead))
        ? rdptr(title, sizeof(kGmTellTitleRead)) - kGmTellTitle : NULL;
    if (!read || !asked_bytes(in, read, kGmTellResultRead, sizeof(kGmTellResultRead)) || !queue) {
        snprintf(why, len, "gmtell_poll: it does not read the question's result word and the queue's title"
            " (client patched?)");
        return false;
    }
    if (!asked_data(in, slot, 4) || !asked_data(in, queue, kGmTellSlots * kGmTellSlotBytes)) {
        snprintf(why, len, "gmtell_poll: its queue 0x%08X or slot 0x%08X is not readable data",
            static_cast<unsigned>(reinterpret_cast<uintptr_t>(queue)),
            static_cast<unsigned>(reinterpret_cast<uintptr_t>(slot)));
        return false;
    }
    s.gm_slot = reinterpret_cast<const int32_t*>(slot);
    s.gm_queue = queue;

    s.mp_dead_input = asked_find(in, "mp_dead_input", kSigMpDeadInput, why, len);
    if (!s.mp_dead_input) {
        return false;
    }
    const uint8_t* tick = NULL;
    const int ticks = asked_count(in, s.mp_dead_input, kMpDeadTickBytes, kMpDeadTick, sizeof(kMpDeadTick), &tick);
    if (ticks != 1) {
        snprintf(why, len, "mp_dead_input: its tick's countdown and its choice at the deadline are %s",
            ticks ? "there more than once" : "not there (client patched?)");
        return false;
    }

    s.ok_input = asked_find(in, "ok_input", kSigOkInput, why, len);
    if (!s.ok_input) {
        return false;
    }
    const uint8_t* again = NULL;
    if (rdptr(s.ok_input, kOkManager) != in.mcb || asked_target(in, s.ok_input + kOkClose, 0xE8) != close_instance
        || asked_count(in, s.ok_input, kOkInputBytes, kOkCloseAgain, sizeof(kOkCloseAgain), &again) != 1) {
        snprintf(why, len, "ok_input: it does not close its window by instance after the callback, and again"
            " when +0x258 is set");
        return false;
    }

    if (!resolve_door(in, close_instance, s.bank_data, &s.door, why, len)) {
        return false;
    }
    *out = s;
    return true;
}

// A shape found: where, and each hole's offset in it, in order.
const int kShapeHoles = 16;

struct Shape {
    const uint8_t* at;
    size_t hole[kShapeHoles];
    int holes;
};

// Shape `text` at `at` in the image's code, its holes into *out.
inline bool shape_is(const AskedInputs& in, const uint8_t* at, const char* text, Shape* out) {
    uint8_t bytes[512];
    char mask[513];
    const size_t n = parse_signature(text, bytes, mask, sizeof(bytes));
    if (!n || !at || at < in.text || at + n > in.text + in.size || !match_from(at, bytes, mask, 0)) {
        return false;
    }
    out->at = at;
    out->holes = 0;
    for (size_t i = 0; i < n;) {
        if (mask[i] == 'x') {
            ++i;
            continue;
        }
        if (out->holes >= kShapeHoles) {
            return false;
        }
        out->hole[out->holes++] = i;
        while (i < n && mask[i] != 'x') {
            ++i;
        }
    }
    return true;
}

// Shape `text` once within the first `span` bytes from `from`.
inline bool shape_once(const AskedInputs& in, const uint8_t* from, size_t span, const char* text, Shape* out) {
    uint8_t bytes[512];
    char mask[513];
    const size_t n = parse_signature(text, bytes, mask, sizeof(bytes));
    if (!n || !from || from < in.text || from >= in.text + in.size) {
        return false;
    }
    const size_t left = static_cast<size_t>(in.text + in.size - from);
    const size_t end = left < span ? left : span;
    const uint8_t* found = NULL;
    int count = 0;
    for (size_t i = 0; i + n <= end; ++i) {
        if (match_from(from + i, bytes, mask, 0)) {
            found = from + i;
            ++count;
        }
    }
    return count == 1 && shape_is(in, found, text, out);
}

// Hole i's imm32, and the routine a call or jump ending in hole i reaches
// when it is in the image's code.
inline const uint8_t* shape_ptr(const Shape& s, int i) {
    return i < s.holes ? rdptr(s.at, s.hole[i]) : NULL;
}

inline const uint8_t* shape_target(const AskedInputs& in, const Shape& s, int i) {
    if (i >= s.holes) {
        return NULL;
    }
    const uint8_t* to = asked_call(s.at + s.hole[i] - 1);
    return to >= in.text && to < in.text + in.size ? to : NULL;
}

// The NPC dialogue boxes' routines and data, and the return address of
// their tick's left-button test, which `in.key_test` must be the target of;
// `close_instance` the routine close by name ends in.
inline bool resolve_dialogue(const AskedInputs& in, const uint8_t* close_instance, DialogueSites* out, char* why,
                             size_t len) {
    DialogueSites s;
    memset(&s, 0, sizeof(s));
    s.input = asked_find(in, "dialogue_input", kSigDialogueInput, why, len);
    if (!s.input) {
        return false;
    }
    s.handler = asked_target(in, s.input + kDialogueHandlerCall, 0xE8);
    if (!s.handler || rdptr(s.input, kDialogueGlobals[0]) != asked_slot(in, "rem4line")
        || rdptr(s.input, kDialogueGlobals[1]) != asked_slot(in, "rem4li2")
        || !asked_bytes(in, s.input + kDialogueAgainAt, kDialogueAgain, sizeof(kDialogueAgain))
        || asked_target(in, s.input + kDialogueAgainAt + sizeof(kDialogueAgain) - 1, 0xE8) != s.handler) {
        snprintf(why, len, "dialogue_input: it does not hand its codes to one handler, again for the controllers in"
            " the registry's rem4line and rem4li2 slots");
        return false;
    }
    const uint8_t* dispatch_call = NULL;
    const uint8_t* test = NULL;
    if (!asked_once(in, s.handler, kDialogueHandlerBytes, kDialogueDispatchCall, sizeof(kDialogueDispatchCall),
            &dispatch_call)
        || !asked_once(in, s.handler, kDialogueHandlerBytes, kDialogueCancelTest, sizeof(kDialogueCancelTest), &test)
        || !asked_bytes(in, test - kDialogueGateAt, kDialogueGateHead, sizeof(kDialogueGateHead))
        || rdptr(test - kDialogueGateAt, sizeof(kDialogueGateHead)) != in.cancel_allowed
        || !asked_bytes(in, test - kDialogueGateTailAt, kDialogueGateTail, sizeof(kDialogueGateTail))
        || !asked_bytes(in, test + kDialogueReleaseAt, kDialogueRelease, sizeof(kDialogueRelease))
        || !asked_bytes(in, test + kDialogueClearAt, kDialogueClear, sizeof(kDialogueClear))
        || !asked_bytes(in, test + kDialogueWaitAt, kDialogueWait, sizeof(kDialogueWait))
        || test[kDialogueWaitAt + sizeof(kDialogueWait) + 4] != kWaitAborted
        || !asked_bytes(in, test + kDialogueCloseAt, kDialogueClose, sizeof(kDialogueClose))
        || rdptr(test + kDialogueCloseAt, sizeof(kDialogueClose)) != in.mcb
        || asked_target(in, test + kDialogueCloseAt + sizeof(kDialogueClose) + 4, 0xE8) != close_instance) {
        snprintf(why, len, "dialogue_input: its handler does not hand the code to the text dispatch, then cancel"
            " as the box's own cancel does (client patched?)");
        return false;
    }
    const uint8_t* dispatch = asked_target(in, dispatch_call + sizeof(kDialogueDispatchCall) - 1, 0xE8);
    const uint8_t* release = asked_target(in, test + kDialogueReleaseAt + sizeof(kDialogueRelease) - 1, 0xE8);
    const uint8_t* clear = asked_target(in, test + kDialogueClearAt + sizeof(kDialogueClear) - 1, 0xE8);
    s.wait = const_cast<uint8_t*>(rdptr(test + kDialogueWaitAt, sizeof(kDialogueWait)));
    const uint8_t* gate = dispatch && asked_bytes(in, dispatch, kDialogueDispatchHead, sizeof(kDialogueDispatchHead))
        ? asked_target(in, dispatch + sizeof(kDialogueDispatchHead) - 1, 0xE8) : NULL;
    const uint8_t* advance = gate && asked_bytes(in, gate, kDialogueGateHeadBytes, sizeof(kDialogueGateHeadBytes))
        ? asked_target(in, gate + sizeof(kDialogueGateHeadBytes) - 1, 0xE8) : NULL;
    const uint8_t* sound = advance ? asked_target(in, gate + sizeof(kDialogueGateHeadBytes) + 4, 0xE8) : NULL;
    const uint8_t* sound_name = NULL;
    const uint8_t* show_call = NULL;
    const uint8_t* show = advance && asked_once(in, advance, kDialogueAdvanceBytes, kDialogueAdvanceShow,
        sizeof(kDialogueAdvanceShow), &show_call) ? asked_target(in, show_call + sizeof(kDialogueAdvanceShow) - 1, 0xE8)
        : NULL;
    if (!release || !clear || !asked_bytes(in, clear, kDialogueClearHead, sizeof(kDialogueClearHead)) || !show
        || !asked_once(in, sound, kDialogueSoundBytes, kDialogueSoundName, sizeof(kDialogueSoundName), &sound_name)
        || !asked_data(in, s.wait, 1)) {
        snprintf(why, len, "dialogue_input: the text dispatch, its gate on a page waiting for a key, its advance, the"
            " page-turn sound after it, the box's show or the text object's clear is not the routine the engine knows"
            " (client patched?)");
        return false;
    }
    Shape twin;
    if (!shape_is(in, dispatch + sizeof(kDialogueDispatchHead) + 4, kShapeDialogueTwin, &twin)
        || shape_target(in, twin, 2) != gate || shape_target(in, twin, 4) != gate
        || shape_ptr(twin, 3) != shape_ptr(twin, 1) || shape_ptr(twin, 0) == shape_ptr(twin, 1)
        || !asked_data(in, shape_ptr(twin, 0), 4) || !asked_data(in, shape_ptr(twin, 1), 4)) {
        snprintf(why, len, "dialogue_input: the text dispatch does not hand its code on to the other dialogue box's"
            " text object (client patched?)");
        return false;
    }
    for (int i = 0; i < 2; ++i) {
        s.texts[i] = reinterpret_cast<uint8_t**>(const_cast<uint8_t*>(shape_ptr(twin, i)));
    }
    const struct {
        const uint8_t* bytes;
        size_t length;
    } fields[] = {
        {kDialogueShowLines, sizeof(kDialogueShowLines)},
        {kDialogueShowLine, sizeof(kDialogueShowLine)},
        {kDialogueShowCount, sizeof(kDialogueShowCount)},
        {kDialogueShowCodes, sizeof(kDialogueShowCodes)},
        {kDialogueShowAdd, sizeof(kDialogueShowAdd)},
    };
    for (size_t i = 0; i < sizeof(fields) / sizeof(fields[0]); ++i) {
        const uint8_t* at = NULL;
        if (!asked_once(in, show, kDialogueShowBytes, fields[i].bytes, fields[i].length, &at)) {
            snprintf(why, len, "dialogue_input: the box's show does not keep its page's lines where the engine reads"
                " them (client patched?)");
            return false;
        }
    }
    const uint8_t* tables[8];
    const int vtables = asked_vtables(in, s.input, 6, tables, 8);
    for (int i = 0; i < vtables && i < 8; ++i) {
        const uint8_t* tick = asked_slot_routine(in, tables[i], kDialogueTickSlot);
        s.tick = i == 0 ? tick : s.tick == tick ? tick : NULL;
    }
    if (vtables < 1 || vtables > 8 || !s.tick) {
        snprintf(why, len, "dialogue_input: %d vtables name it, which do not agree on one tick", vtables);
        return false;
    }
    const uint8_t* click = NULL;
    const uint8_t* automatic = NULL;
    const uint8_t* timed = NULL;
    if (!asked_once(in, s.tick, kDialogueTickBytes, kDialogueClick, sizeof(kDialogueClick), &click)
        || asked_target(in, click + sizeof(kDialogueClick) - 1, 0xE8) != in.key_test
        || !asked_bytes(in, click + sizeof(kDialogueClick) + 4, kDialogueClickAfter, sizeof(kDialogueClickAfter))
        || !asked_once(in, s.tick, kDialogueTickBytes, kDialogueAuto, sizeof(kDialogueAuto), &automatic)
        || !asked_once(in, s.tick, kDialogueTickBytes, kDialogueTimed, sizeof(kDialogueTimed), &timed)) {
        snprintf(why, len, "dialogue_tick: its test of the left button through the key test, its read of the"
            " script's own confirm or its timer is not there (client patched?)");
        return false;
    }
    const uint8_t* last_page = asked_target(in, timed + sizeof(kDialogueTimed) - 1, 0xE8);
    const uint8_t* timer_call = timed - sizeof(kDialogueTimerCall) - 4;
    const uint8_t* timer = asked_bytes(in, timer_call, kDialogueTimerCall, sizeof(kDialogueTimerCall))
        ? asked_target(in, timer_call + sizeof(kDialogueTimerCall) - 1, 0xE8) : NULL;
    if (!last_page || !asked_bytes(in, last_page, kDialogueLastPage, sizeof(kDialogueLastPage)) || !timer
        || !asked_bytes(in, timer, kDialogueTimerHead, sizeof(kDialogueTimerHead))
        || !asked_bytes(in, timer + kDialogueTimerAt, kDialogueTimerCount, sizeof(kDialogueTimerCount))) {
        snprintf(why, len, "dialogue_tick: its timer or its test of a message's last page does not read the text"
            " object where the engine does (client patched?)");
        return false;
    }
    s.dispatch = dispatch;
    memcpy(&s.advance, &advance, sizeof(s.advance));
    memcpy(&s.page_sound, &sound, sizeof(s.page_sound));
    memcpy(&s.release, &release, sizeof(s.release));
    memcpy(&s.clear, &clear, sizeof(s.clear));
    s.click_return = reinterpret_cast<uintptr_t>(click + sizeof(kDialogueClick) + 4);
    *out = s;
    return true;
}

// The one vtable in the image's read-only data naming `input` at slot 6,
// and its routine at `slot`.
inline const uint8_t* text_vtable_routine(const AskedInputs& in, const uint8_t* input, size_t slot,
                                          const void** vtable) {
    const uint8_t* tables[2];
    if (asked_vtables(in, input, 6, tables, 2) != 1) {
        return NULL;
    }
    if (vtable) {
        *vtable = tables[0];
    }
    return asked_slot_routine(in, tables[0], slot);
}

// The text-entry windows' routines and data, every routine named by more than
// one site agreeing; `close_instance` is the routine close by name ends in,
// `focus` checked against link11's.
inline bool resolve_text(const AskedInputs& in, const uint8_t* close_instance, TextSites* out, char* why,
                         size_t len) {
    TextSites s;
    memset(&s, 0, sizeof(s));
    Shape a, b, c;

    // stringdl
    s.stringdl_input = asked_find(in, "stringdl_input", kSigStringdlInput, why, len);
    if (!s.stringdl_input) {
        return false;
    }
    s.stringdl_slot = asked_target(in, s.stringdl_input + kStringdlSlotCall, 0xE8);
    s.stringdl_name = asked_target(in, s.stringdl_input + kStringdlNameCall, 0xE8);
    const uint8_t* found = NULL;
    if (!shape_is(in, s.stringdl_input + kStringdlTailAt, kShapeStringdlTail, &a)
        || shape_ptr(a, 0) != asked_slot(in, "delivery") || shape_ptr(a, 1) != in.mcb || a.at[a.hole[2] - 1] != 0xE9
        || !shape_target(in, a, 2)) {
        snprintf(why, len, "stringdl_input: with a reply written, it does not raise the outgoing box through the"
            " manager");
        return false;
    }
    const uint8_t* focus = shape_target(in, a, 2);
    if (!s.stringdl_name || !s.stringdl_slot
        || !shape_once(in, s.stringdl_name, kStringdlHandlerBytes, kShapeStringdlNamed, &b)
        || !asked_once(in, s.stringdl_name, kStringdlHandlerBytes, kStringdlCancelWrite,
            sizeof(kStringdlCancelWrite), &found)
        || !asked_once(in, s.stringdl_slot, kStringdlSlotBytes, kStringdlSlotReplies[0],
            sizeof(kStringdlSlotReplies[0]), &found)
        || !asked_once(in, s.stringdl_slot, kStringdlSlotBytes, kStringdlSlotReplies[1],
            sizeof(kStringdlSlotReplies[1]), &found)) {
        snprintf(why, len, "stringdl_input: its handlers do not commit the name on row 2 and write the replies the"
            " engine writes (client patched?)");
        return false;
    }
    const uint8_t* stop = shape_target(in, b, 0);
    if (!shape_is(in, stop, kShapeStringdlStop, &c) || shape_ptr(c, 0) != shape_ptr(c, 2)
        || !asked_data(in, shape_ptr(c, 0), 4) || !shape_target(in, c, 1) || !shape_target(in, c, 3)) {
        snprintf(why, len, "stringdl_input: the field's end it calls does not clear +0x164 and take the line from"
            " the field holding it (client patched?)");
        return false;
    }
    const uint8_t* line = shape_ptr(c, 0);
    const uint8_t* unbind = shape_target(in, c, 1);
    const uint8_t* bind = shape_target(in, c, 3);
    s.line = reinterpret_cast<uint8_t**>(const_cast<uint8_t*>(line));
    memcpy(&s.unbind, &unbind, sizeof(s.unbind));
    memcpy(&s.bind, &bind, sizeof(s.bind));
    memcpy(&s.stringdl_stop, &stop, sizeof(s.stringdl_stop));
    memcpy(&s.focus, &focus, sizeof(s.focus));

    // comment
    s.comment_input = asked_find(in, "comment_input", kSigCommentInput, why, len);
    if (!s.comment_input) {
        return false;
    }
    stop = asked_target(in, s.comment_input + kCommentStopCall, 0xE8);
    if (!stop || !shape_once(in, s.comment_input, kCommentInputBytes, kShapeCommentEscape, &a)
        || shape_target(in, a, 0) != stop || shape_ptr(a, 1) != in.mcb || shape_target(in, a, 2) != close_instance) {
        snprintf(why, len, "comment_input: its Escape does not end the field, then close the window by instance"
            " (client patched?)");
        return false;
    }
    Shape sanitize, trim;
    if (!shape_is(in, stop, kShapeCommentStop, &b)
        || !shape_once(in, stop, kCommentStopBytes, kShapeCommentSanitize, &sanitize)
        || !shape_once(in, stop, kCommentStopBytes, kShapeCommentTrim, &trim)
        || !shape_target(in, sanitize, 0) || !shape_target(in, trim, 0)) {
        snprintf(why, len, "comment_input: the field's end does not commit 40 bytes into the message's line, then"
            " sanitize and trim it (client patched?)");
        return false;
    }
    s.comment_close = text_vtable_routine(in, s.comment_input, kCommentCloseSlot, &s.comment_vtable);
    if (!shape_is(in, s.comment_close, kShapeCommentClose, &c) || shape_target(in, c, 0) != stop) {
        snprintf(why, len, "comment_input: its window's begin-close does not end a field still open first"
            " (client patched?)");
        return false;
    }
    const uint8_t* open_hook = text_vtable_routine(in, s.comment_input, kCommentOpenSlot, NULL);
    const uint8_t* line_start = open_hook && shape_once(in, open_hook, kCommentHookBytes, kShapeCommentOpen, &c)
        ? shape_target(in, c, 0) : NULL;
    Shape start_trim;
    if (!line_start || !shape_is(in, line_start, kShapeCommentStart, &c)
        || !shape_once(in, line_start, kCommentStartBytes, kShapeCommentStartTrim, &start_trim)
        || shape_target(in, start_trim, 0) != shape_target(in, trim, 0)
        || !asked_once(in, line_start, kCommentStartBytes, kCommentStartLine, sizeof(kCommentStartLine), &found)) {
        snprintf(why, len, "comment_input: its open hook does not start the field on a line through the start"
            " that trims the line and records it (client patched?)");
        return false;
    }
    const uint8_t* fn = shape_target(in, sanitize, 0);
    memcpy(&s.sanitize, &fn, sizeof(s.sanitize));
    fn = shape_target(in, trim, 0);
    memcpy(&s.trim, &fn, sizeof(s.trim));
    memcpy(&s.comment_stop, &stop, sizeof(s.comment_stop));
    memcpy(&s.comment_start, &line_start, sizeof(s.comment_start));

    // msgline
    s.msgline_input = asked_find(in, "msgline_input", kSigMsglineInput, why, len);
    if (!s.msgline_input) {
        return false;
    }
    stop = asked_target(in, s.msgline_input + kMsglineStopCall, 0xE8);
    const uint8_t* table = NULL;
    if (!stop || !shape_is(in, s.msgline_input + kMsglineCommitAt, kShapeMsglineCommit, &a)
        || !asked_data(in, shape_ptr(a, 1), 4) || !asked_code(in, shape_ptr(a, 4), 8 * 4)) {
        snprintf(why, len, "msgline_input: its commit does not test the player's data, then take the mode's case"
            " (client patched?)");
        return false;
    }
    s.player = reinterpret_cast<uint8_t**>(const_cast<uint8_t*>(shape_ptr(a, 1)));
    table = shape_ptr(a, 4);
    const uint8_t* cases[8];
    for (int m = 0; m < 8; ++m) {
        cases[m] = rdptr(table, 4 * static_cast<size_t>(m));
    }
    Shape tell, reply, brief, answer, dated;
    const bool shaped = shape_is(in, cases[0], kShapeMsglineTell, &tell)
        && shape_is(in, cases[1], kShapeMsglineReply, &reply) && shape_is(in, cases[2], kShapeMsglineShort, &brief)
        && shape_is(in, cases[5], kShapeMsglineAnswer, &answer) && shape_is(in, cases[7], kShapeMsglineDated, &dated);
    const uint8_t* pol = shaped ? shape_ptr(tell, 0) : NULL;
    const uint8_t* number = shaped ? shape_target(in, tell, 4) : NULL;
    const uint8_t* clock = shaped ? shape_target(in, brief, 0) : NULL;
    s.sent[0] = shaped ? shape_ptr(tell, 1) : NULL;
    s.sent[1] = shaped ? shape_ptr(answer, 0) : NULL;
    if (!shaped || shape_ptr(tell, 3) != pol || shape_ptr(brief, 1) != pol || !asked_data(in, pol, 4)
        || shape_target(in, brief, 3) != number || !number || !clock || shape_target(in, dated, 0) != clock
        || shape_target(in, dated, 1) != clock || shape_ptr(reply, 0) != s.sent[0]
        || shape_ptr(brief, 2) != s.sent[0] || shape_ptr(dated, 2) != s.sent[1]
        || !asked_code(in, s.sent[0], 1) || !asked_code(in, s.sent[1], 1)
        || shape_target(in, tell, 2) != in.message || shape_target(in, reply, 1) != in.message
        || shape_target(in, answer, 1) != in.message || !shape_target(in, tell, 5) || !shape_target(in, reply, 2)
        || !shape_target(in, brief, 4) || !shape_target(in, answer, 2) || !shape_target(in, dated, 3)) {
        snprintf(why, len, "msgline_input: the cases of modes 0, 1, 2, 5 and 7 do not send the message as the engine"
            " sends it (client patched?)");
        return false;
    }
    s.pol = reinterpret_cast<uint8_t**>(const_cast<uint8_t*>(pol));
    memcpy(&s.pol_number, &number, sizeof(s.pol_number));
    memcpy(&s.clock_text, &clock, sizeof(s.clock_text));
    fn = shape_target(in, tell, 5);
    memcpy(&s.send_tell, &fn, sizeof(fn));
    fn = shape_target(in, reply, 2);
    memcpy(&s.send_reply, &fn, sizeof(fn));
    fn = shape_target(in, brief, 4);
    memcpy(&s.send_short, &fn, sizeof(fn));
    fn = shape_target(in, answer, 2);
    memcpy(&s.send_answer, &fn, sizeof(fn));
    fn = shape_target(in, dated, 3);
    memcpy(&s.send_dated, &fn, sizeof(fn));
    if (!shape_once(in, s.msgline_input, kMsglineInputBytes, kShapeMsglineSent, &a)
        || !asked_data(in, shape_ptr(a, 0), 4) || !shape_target(in, a, 1) || !asked_data(in, shape_ptr(a, 2), 1)
        || !asked_names_key(in, a.at + a.hole[3], "msgline") || shape_ptr(a, 4) != in.mcb
        || shape_target(in, a, 5) != in.close
        || !shape_once(in, s.msgline_input, kMsglineInputBytes, kShapeMsglineCancel, &b)
        || shape_target(in, b, 0) != stop || !asked_names_key(in, b.at + b.hole[1], "msgline")
        || shape_ptr(b, 2) != in.mcb || shape_target(in, b, 3) != in.close) {
        snprintf(why, len, "msgline_input: a sent message does not set the busy byte before msgline's close, or its"
            " Escape does not end the field and close it (client patched?)");
        return false;
    }
    s.sending = reinterpret_cast<uint8_t**>(const_cast<uint8_t*>(shape_ptr(a, 0)));
    fn = shape_target(in, a, 1);
    memcpy(&s.mark, &fn, sizeof(s.mark));
    s.busy = const_cast<uint8_t*>(shape_ptr(a, 2));
    const uint8_t* hook = text_vtable_routine(in, s.msgline_input, kMsglineOpenSlot, NULL);
    const uint8_t* start = hook && shape_once(in, hook, kMsglineHookBytes, kShapeMsglineStart, &c)
        ? shape_target(in, c, 0) : NULL;
    if (!shape_is(in, stop, kShapeMsglineStop, &b) || !start || !shape_is(in, start, kShapeMsglineField, &c)
        || !asked_once(in, start, kMsglineFieldBytes, kMsglineFieldLong, sizeof(kMsglineFieldLong), &found)
        || !asked_once(in, start, kMsglineFieldBytes, kMsglineFieldShort, sizeof(kMsglineFieldShort), &found)) {
        snprintf(why, len, "msgline_input: the field's end does not commit 300 bytes into +0x53, or the field its"
            " open starts is not 300 bytes long, 15 in modes 2 and 7 (client patched?)");
        return false;
    }
    memcpy(&s.msgline_stop, &stop, sizeof(s.msgline_stop));
    memcpy(&s.msgline_start, &start, sizeof(s.msgline_start));

    // scname
    s.scname_input = asked_find(in, "scname_input", kSigScnameInput, why, len);
    if (!s.scname_input) {
        return false;
    }
    const uint8_t* search = rdptr(s.scname_input, kScnameSearchGlobal);
    stop = asked_target(in, s.scname_input + kScnameStopCall, 0xE8);
    if (!stop || !asked_data(in, search, 4)
        || !shape_once(in, s.scname_input, kScnameInputBytes, kShapeScnameSearch, &a)
        || !shape_target(in, a, 0) || !shape_target(in, a, 1) || shape_ptr(a, 2) != search
        || !shape_target(in, a, 3) || !asked_names_key(in, a.at + a.hole[4], "scname") || shape_ptr(a, 5) != in.mcb
        || shape_target(in, a, 6) != in.close || !asked_names_key(in, a.at + a.hole[7], "scsibori")
        || shape_ptr(a, 8) != in.mcb || shape_target(in, a, 9) != in.close
        || !shape_is(in, stop, kShapeScnameStop, &b)) {
        snprintf(why, len, "scname_input: its row 2 does not name the player search and start it, then close scname"
            " and scsibori (client patched?)");
        return false;
    }
    s.search = reinterpret_cast<uint8_t**>(const_cast<uint8_t*>(search));
    fn = shape_target(in, a, 0);
    memcpy(&s.criteria_clear, &fn, sizeof(fn));
    fn = shape_target(in, a, 1);
    memcpy(&s.criteria_name, &fn, sizeof(fn));
    fn = shape_target(in, a, 3);
    memcpy(&s.search_start, &fn, sizeof(fn));
    memcpy(&s.scname_stop, &stop, sizeof(s.scname_stop));

    // link11
    s.link11_input = asked_find(in, "link11_input", kSigLink11Input, why, len);
    if (!s.link11_input) {
        return false;
    }
    stop = asked_target(in, s.link11_input + kLink11StopCall, 0xE8);
    const uint8_t* link5 = asked_slot(in, "link5");
    if (!stop || !shape_once(in, s.link11_input, kLink11InputBytes, kShapeLink11Cancel, &a)
        || !asked_names_key(in, a.at + a.hole[0], "link11") || shape_ptr(a, 1) != in.mcb
        || shape_target(in, a, 2) != in.close || !asked_names_key(in, a.at + a.hole[3], "link6")
        || shape_ptr(a, 4) != in.mcb || shape_target(in, a, 5) != in.open || shape_ptr(a, 6) != link5
        || shape_ptr(a, 7) != in.mcb || a.at[a.hole[8] - 1] != 0xE9 || shape_target(in, a, 8) != focus) {
        snprintf(why, len, "link11_input: its cancel does not close link11, open link6 and raise link5's window"
            " (client patched?)");
        return false;
    }
    if (!shape_once(in, s.link11_input, kLink11InputBytes, kShapeLink11Create, &b) || shape_ptr(b, 2) != link5
        || shape_ptr(b, 11) != link5 || !shape_target(in, b, 1) || !shape_target(in, b, 3)
        || !shape_target(in, b, 4) || !shape_target(in, b, 5) || !shape_target(in, b, 6)
        || !shape_is(in, stop, kShapeLink11Stop, &c)) {
        snprintf(why, len, "link11_input: its row 5 does not send the new linkshell's request as the engine sends it"
            " (client patched?)");
        return false;
    }
    for (int i = 0; i < 4; ++i) {
        s.made[i] = const_cast<uint8_t*>(shape_ptr(b, 7 + i));
        if (!asked_data(in, s.made[i], i == 1 ? 4 : 1)) {
            snprintf(why, len, "link11_input: the request's bytes its row 5 writes are not data");
            return false;
        }
    }
    fn = shape_target(in, b, 1);
    memcpy(&s.slot_taken, &fn, sizeof(fn));
    fn = shape_target(in, b, 3);
    memcpy(&s.selected, &fn, sizeof(fn));
    fn = shape_target(in, b, 4);
    memcpy(&s.encode, &fn, sizeof(fn));
    fn = shape_target(in, b, 5);
    memcpy(&s.create, &fn, sizeof(fn));
    fn = shape_target(in, b, 6);
    memcpy(&s.item, &fn, sizeof(fn));
    memcpy(&s.link11_stop, &stop, sizeof(s.link11_stop));

    // passinpu
    s.passinpu_input = asked_find(in, "passinpu_input", kSigPassinpuInput, why, len);
    if (!s.passinpu_input) {
        return false;
    }
    stop = asked_target(in, s.passinpu_input + kPassinpuStopCall, 0xE8);
    hook = text_vtable_routine(in, s.passinpu_input, kPassinpuOpenSlot, NULL);
    if (!stop || !shape_is(in, hook, kShapePassinpuOpen, &a) || !shape_target(in, a, 2)
        || !shape_is(in, stop, kShapePassinpuStop, &b)) {
        snprintf(why, len, "passinpu_input: its open hook does not start the field, or the field's end does not"
            " take its text into +0x24 (client patched?)");
        return false;
    }
    if (!shape_is(in, shape_target(in, a, 2) + kPassinpuBindAt, kShapePassinpuBind, &c) || shape_ptr(c, 0) != line
        || shape_ptr(c, 2) != line || shape_target(in, c, 1) != unbind || shape_target(in, c, 3) != bind) {
        snprintf(why, len, "passinpu_input: its field's start does not end in the unbind and bind of the text entry's"
            " manager stringdl's end calls (client patched?)");
        return false;
    }
    fn = shape_target(in, a, 2);
    memcpy(&s.passinpu_start, &fn, sizeof(fn));
    memcpy(&s.passinpu_stop, &stop, sizeof(s.passinpu_stop));
    *out = s;
    return true;
}

// The trade's input routine from its row dispatch on, never run. Rows 1..8
// hand an offered item's index and id to the stage with a count of 0, its
// removal; row 9 is the OK unless the player's is in (+0xC0 4); row 10 the
// cancel, then the trade's windows closed when it went. The stage un-OKs and
// sends the gil; the OK writes +0xC0 = 4 once sent; the cancel clears the
// partner's fields once sent; the close closes trade, then gift, by name.
// kShapeTradeTheirs' hole: the partner's offer's offset.
const char kSigTradeInput[] =
    "0f bf 44 24 04 48 56 83 f8 07 57 8b f1 0f 87 ?? ?? ?? ?? ff 24 85 ?? ?? ?? ?? 0f bf 44 24 10 8b c8 83 e9 09 0f 84 ?? ?? ?? ?? 49 0f 84 ?? ?? ?? ?? 49 74 72 8b 8e c0 00 00 00 83 f9 03 75 62";
const size_t kTradeOkJump = 38;
const size_t kTradeCancelJump = 45;
const size_t kTradeRowAt = 63;
const size_t kTradeInputBytes = 0x200;
const char kShapeTradeRow[] =
    "8a 80 ?? ?? ?? ?? 8b 15 ?? ?? ?? ?? 88 44 24 10 8b 4c 24 10 81 e1 ff 00 00 00 8d 04 ca 66 0f b6 8c ca ?? ?? ?? ?? 66 8b 80 ?? ?? ?? ?? 66 85 c9 75 20";
const char kShapeTradeRemove[] = "6a 00 50 51 8b ce e8 ?? ?? ?? ?? 5f 5e c2 08 00";
const char kShapeTradeOkRow[] = "83 be c0 00 00 00 04 0f 84 ?? ?? ?? ?? 8b ce e8 ?? ?? ?? ?? 5f 5e c2 08 00";
const char kShapeTradeCancelRow[] =
    "6a 00 8b ce e8 ?? ?? ?? ?? 84 c0 0f 84 ?? ?? ?? ?? 8b ce e8 ?? ?? ?? ?? 5f 5e c2 08 00";
const char kShapeTradeStage[] =
    "51 53 56 8b f1 57 8b 46 08 0f bf 48 4c 8a 81 ?? ?? ?? ?? 84 c0 88 44 24 0c 75 40 66 83 7c 24 14 00 0f 85 ?? ?? ?? ?? 66 81 7c 24 18 ff ff 0f 85 ?? ?? ?? ?? 8b 86 c0 00 00 00 bb 03 00 00 00 3b c3 74 05 e8 ?? ?? ?? ?? 8b 54 24 1c 6a 00 52 6a 00 e8 ?? ?? ?? ??";
const char kShapeTradeStageSlot[] = "66 0f b6 8c ca ?? ?? ?? ?? 66 3b f9";
const char kShapeTradeStageTail[] = "89 9e c4 00 00 00 89 9e c0 00 00 00 5f 5e b0 01 5b 59 c2 0c 00";
const size_t kTradeStageBytes = 0x160;
const char kShapeTradeOk[] = "56 8b f1 e8 ?? ?? ?? ?? 84 c0 74 0e c7 86 c0 00 00 00 04 00 00 00 b0 01 5e c3";
const char kShapeTradeCancel[] =
    "53 56 8b f1 33 db 39 5e 70 75 05 39 5e 78 74 25 e8 ?? ?? ?? ?? 84 c0 74 1c 89 5e 6c 89 5e 70 88 5e 7c 89 9e c0 00 00 00 89 5e 20 89 5e 24 5e b0 01 5b c2 04 00";
const char kShapeTradeClose[] =
    "68 ?? ?? ?? ?? b9 ?? ?? ?? ?? e8 ?? ?? ?? ?? 68 ?? ?? ?? ?? b9 ?? ?? ?? ?? e8 ?? ?? ?? ??";
const char kShapeTradeSend[] =
    "8b 0d ?? ?? ?? ?? 53 85 c9 74 41 8b 54 24 10 8b 5c 24 08 33 c0 81 e2 ff 00 00 00 81 c1 ?? ?? ?? ?? 3b c2 74 0c 84 db 74 08 85 c0 74 04 38 19 74 1f 40 83 c1 08 83 f8 0a 72 e7 6a 00 6a 00 6a 34 e8 ?? ?? ?? ??";
const char kShapeTradeTheirs[] =
    "3c 0a 72 03 32 c0 c3 25 ff 00 00 00 8d 14 80 8d 04 50 8d 84 81 ?? ?? ?? ?? 8b 48 04 85 c9 75 03 32 c0 c3 8b 4c 24 04 89 01 b0 01 c3";
const size_t kTradeSendBytes = 0x100;

// The trade's open of its three windows, never run: it cancels when the
// partner's name (index and id at +0x6C) cannot be read, hands the inventory
// list its mode and opens money; it cancels and closes the windows when its
// own did not open. kShapeActorName's holes: the actor table, then the titled
// name's length word, the titled name and the name.
const char kSigTradeOpen[] =
    "56 6a 08 8b f1 e8 ?? ?? ?? ?? 83 c4 04 84 c0 6a 00 75 0b 8b ce e8 ?? ?? ?? ?? 32 c0 5e c3 8d 46 6c 50 e8 ?? ?? ?? ?? 83 c4 08 84 c0 75 0d 6a 00 8b ce e8 ?? ?? ?? ?? 32 c0 5e c3";
const size_t kTradeOpenCancel[2] = {21, 50};
const size_t kTradeOpenName = 34;
const size_t kTradeOpenBytes = 0xE0;
const char kShapeOfferMode[] =
    "8b 0d ?? ?? ?? ?? 51 8b 0d ?? ?? ?? ?? e8 ?? ?? ?? ?? 6a 00 6a 01 68 ?? ?? ?? ?? b9 ?? ?? ?? ?? e8 ?? ?? ?? ??";
const char kShapeTradeOpenTail[] = "8b ce e8 ?? ?? ?? ?? 84 c0 74 07 8b ce e8 ?? ?? ?? ?? b0 01 5e c3";
const char kShapeActorName[] =
    "56 8b 74 24 08 57 8b 46 04 85 c0 76 24 8b 06 8b 04 85 ?? ?? ?? ?? 85 c0 74 17 66 83 b8 ?? ?? ?? ?? 00 76 08 8d b8 ?? ?? ?? ?? eb 07 8d 78 ?? eb 02 33 ff";

// The NPC trade's input routine, never run: rows 1..8 give the inventory
// list's window the focus and write the row to +0x40; row 9 is the OK, then
// the close when it went, else the buzz; row 10 the close alone. The OK sends
// the items, the gil and the NPC; the close clears the NPC and closes the
// window by instance. The stage, which the item count's asker calls, puts the
// gil at +0x3C for index 0, else clears every entry holding the item.
const char kSigHandoverInput[] =
    "0f bf 44 24 04 53 56 83 e8 05 57 8b f1 74 15 48 75 6e 8b 4e 08 85 c9 74 59 6a 01 6a 0a e8 ?? ?? ?? ?? eb 5c 66 8b 5c 24 14 0f bf c3 83 e8 09 74 36 48 74 3e 48 6a 00 74 18 a1 ?? ?? ?? ??";
const size_t kHandoverOkJump = 48;
const size_t kHandoverCancelJump = 51;
const size_t kHandoverList = 58;
const size_t kHandoverInputBytes = 0x100;
const size_t kHandoverOpenSlot = 1;
const size_t kHandoverOpenBytes = 0x80;
const size_t kHandoverOkBytes = 0xE0;
const char kShapeHandoverPick[] = "8b 48 08 51 b9 ?? ?? ?? ?? e8 ?? ?? ?? ?? 88 5e 40";
const char kShapeHandoverOkRow[] =
    "8b ce e8 ?? ?? ?? ?? 84 c0 74 09 8b ce e8 ?? ?? ?? ?? eb 05 e8 ?? ?? ?? ??";
const size_t kHandoverCloseRowAt = 11;
const char kShapeHandoverOk[] =
    "51 53 57 8b f9 b2 01 b9 08 00 00 00 8d 47 18 8a 58 03 84 db 74 08 66 83 38 00 74 02 32 d2 83 c0 04 49 75 eb 8b 47 3c 85 c0 74 17 66 c7 47 14 ff ff c6 47 17 01 8b 47 44 85 c0 75 10";
const char kShapeHandoverSend[] = "6a 00 6a 00 6a 36 e8 ?? ?? ?? ??";
const char kShapeHandoverClose[] = "8b 41 08 c7 41 44 00 00 00 00 6a 01 50 b9 ?? ?? ?? ?? e8 ?? ?? ?? ?? c3";
const char kShapeHandoverStage[] =
    "53 8a 5c 24 08 33 d2 56 3a da 57 75 0d 8b 44 24 18 5f 5e 89 41 3c 5b c2 0c 00 66 8b 7c 24 14 33 f6 8d 41 14 38 58 02 75 1b 66 39 38 75 16 33 db 66 89 10 88 50 03 88 50 02 8a 59 40";
const char kShapeHandoverMode[] = "8b 0d ?? ?? ?? ?? 85 c9 74 0b a1 ?? ?? ?? ?? 50 e8 ?? ?? ?? ??";

// Where each item count's asker (kAskerSites' trade and handover items)
// calls its window's stage, the e8 at these offsets of its callback.
const size_t kAskTradeItemStage = 29;
const size_t kAskHandoverItemStage = 27;

// The inventory list's build of a row, never run: the item's lock byte (a u16
// offset) is raised around its description, and the gil's row is gray. Then
// the mode's test of the row (vtable +0x30); unless it marks the row offered,
// a status at +8 above 0x1B grays the item, else its class picks a case: kept,
// kept only for the mode in the global of hole 4 (kRowsModalCase), or grayed
// (kRowsGrayCase). kShapeRowsTest holes 2 and 3: the class table and the
// cases' jump table.
const char kSigInventorRows[] =
    "8b 0d ?? ?? ?? ?? 33 c0 66 8b 07 50 e8 ?? ?? ?? ?? 85 c0 89 44 24 18 0f 84 ?? ?? ?? ?? 8a 88 ?? ?? 00 00 50 fe c1 88 88 ?? ?? 00 00 e8 ?? ?? ?? ?? 83 c4 04 66 81 3f ff ff 89 44 24 1c";
const size_t kRowsCache = 2;
const size_t kRowsItemData = 12;
const size_t kRowsLock = 31;
const size_t kRowsLockAgain = 40;
const size_t kRowsDescribe = 44;
const size_t kRowsBytes = 0x100;
const char kShapeRowsTest[] =
    "66 8b 4e 50 66 3b 4e 54 8b 4e 5c 8b 11 7d 05 ff 52 20 eb 05 50 57 ff 52 30 8b e8 85 ed 75 0f 8b 44 24 18 fe 88 ?? ?? 00 00 e9 ?? ?? ?? ?? 81 fd 20 40 80 80 74 2a 33 c0 8a 47 08 83 f8 1b 77 1b 33 c9 8a 88 ?? ?? ?? ?? ff 24 8d ?? ?? ?? ?? 8b 56 5c a1 ?? ?? ?? ?? 3b d0 74 05 bd 40 40 40 80";
const size_t kRowsModalCase = 79;
const size_t kRowsGrayCase = 91;
const size_t kRowsCases = 3;

// What those lead to: the allocator (cdecl; the packet id, a flag that skips
// the throttle, a size), the clock (seconds * 1000 + ms) and the item record
// (cdecl; container, index), which moneyctr's setup calls at
// kMoneySetupItemCall.
const char kShapeAllocator[] =
    "a1 ?? ?? ?? ?? 56 85 c0 57 0f 84 ?? ?? ?? ?? 8b 74 24 0c 81 fe ?? ?? ?? ?? 0f 8d ?? ?? ?? ?? 8b 7c 24 10 85 ff 0f 85 ?? ?? ?? ?? 8b 8c b0 ?? ?? ?? ?? 85 c9 0f 84 ?? ?? ?? ?? 53 e8 ?? ?? ?? ?? 8b 0d ?? ?? ?? ?? 8b 94 b1 ?? ?? ?? ??";
const char kShapeClock[] = "8b 0d ?? ?? ?? ?? 8b 41 0c 8b 49 10 8d 04 80 8d 04 80 8d 04 80 8d 04 c1 c3";
const char kShapeItemRecord[] =
    "8b 15 ?? ?? ?? ?? 85 d2 75 03 33 c0 c3 8b 44 24 04 8b 4c 24 08 56 8b f1 8d 04 c0 81 e6 ff 00 00 00 8d 04 c0 03 c6 84 c9 8d 34 80 8d 04 70 5e 8d 84 82 ?? ?? ?? ??";
const size_t kMoneySetupItemCall = 20;

// A routine's rel32 jump target: the rel32 at `at`.
inline const uint8_t* rel32_target(const uint8_t* at) {
    int32_t rel;
    memcpy(&rel, at, 4);
    return at + 4 + rel;
}

// The trade's and the NPC trade's routines and data, every routine named by
// more than one site agreeing; `money_setup` is moneyctr's setup and
// `askers` the number boxes' askers' callbacks.
inline bool resolve_offer(const AskedInputs& in, const uint8_t* close_instance, const uint8_t* money_setup,
                          const void* const* askers, OfferSites* out, char* why, size_t len) {
    OfferSites s;
    memset(&s, 0, sizeof(s));
    Shape a, b, c, d;

    // the trade's rows, and the routines they reach
    s.trade_input = asked_find(in, "trade_input", kSigTradeInput, why, len);
    if (!s.trade_input) {
        return false;
    }
    const uint8_t* input = s.trade_input;
    const uint8_t* stage = askers ? asked_target(in, static_cast<const uint8_t*>(askers[kAskTradeItem])
        + kAskTradeItemStage, 0xE8) : NULL;
    if (!shape_is(in, input + kTradeRowAt, kShapeTradeRow, &a)
        || !shape_once(in, input, kTradeInputBytes, kShapeTradeRemove, &b) || shape_target(in, b, 0) != stage
        || !stage) {
        snprintf(why, len, "trade_input: its rows of the offer do not take an item back out through the stage its"
            " item count's asker calls (client patched?)");
        return false;
    }
    s.trade_rows = shape_ptr(a, 0);
    s.player = const_cast<uint8_t**>(reinterpret_cast<uint8_t* const*>(shape_ptr(a, 1)));
    const uint32_t own_index = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(shape_ptr(a, 2)));
    s.own = own_index - kOwnIndex;
    const uint8_t* ok_row = rel32_target(input + kTradeOkJump);
    const uint8_t* cancel_row = rel32_target(input + kTradeCancelJump);
    if (!shape_is(in, ok_row, kShapeTradeOkRow, &b) || !shape_is(in, cancel_row, kShapeTradeCancelRow, &c)) {
        snprintf(why, len, "trade_input: its rows 9 and 10 are not the OK and the cancel (client patched?)");
        return false;
    }
    const uint8_t* ok = shape_target(in, b, 1);
    const uint8_t* cancel = shape_target(in, c, 0);
    const uint8_t* close = shape_target(in, c, 2);
    if (reinterpret_cast<uintptr_t>(shape_ptr(a, 3)) != s.own + kOwnId
        || !asked_data(in, s.trade_rows, kTradeGilRow + 1) || !asked_data(in, s.player, 4)
        || s.trade_rows[kTradeOkRow] != 0xFF || s.trade_rows[kTradeCancelRow] != 0xFF
        || s.trade_rows[kTradeGilRow] != 0) {
        snprintf(why, len, "trade_input: its rows' slots are not the offer's 1 to 8 and the gil's on row 11 (client"
            " patched?)");
        return false;
    }
    for (int r = 1; r <= kOfferSlots; ++r) {
        if (s.trade_rows[r] != r) {
            snprintf(why, len, "trade_input: its row %d is not the offer's slot %d (client patched?)", r, r);
            return false;
        }
    }
    if (!shape_is(in, stage, kShapeTradeStage, &a) || shape_ptr(a, 0) != s.trade_rows
        || !shape_once(in, stage, kTradeStageBytes, kShapeTradeStageSlot, &b)
        || reinterpret_cast<uintptr_t>(shape_ptr(b, 0)) != own_index
        || !shape_once(in, stage, kTradeStageBytes, kShapeTradeStageTail, &c) || !shape_target(in, a, 3)
        || !shape_target(in, a, 4)) {
        snprintf(why, len, "trade_stage: it does not stage on its cursor's row into the offer's slots, then write"
            " both states (client patched?)");
        return false;
    }
    s.send = shape_target(in, a, 4);
    if (!shape_is(in, ok, kShapeTradeOk, &a) || !shape_is(in, cancel, kShapeTradeCancel, &b)
        || !shape_is(in, close, kShapeTradeClose, &c) || !asked_names_key(in, c.at + c.hole[0], "trade")
        || !asked_names_key(in, c.at + c.hole[3], "gift") || shape_ptr(c, 1) != in.mcb || shape_ptr(c, 4) != in.mcb
        || shape_target(in, c, 2) != in.close || shape_target(in, c, 5) != in.close) {
        snprintf(why, len, "trade_input: its OK, its cancel or its close of the trade's windows is not the one the"
            " engine calls (client patched?)");
        return false;
    }
    if (!shape_is(in, s.send, kShapeTradeSend, &a) || shape_ptr(a, 0) != reinterpret_cast<const uint8_t*>(s.player)
        || reinterpret_cast<uintptr_t>(shape_ptr(a, 1)) != own_index || !shape_target(in, a, 2)
        || !shape_once(in, s.send, kTradeSendBytes, kShapeTradeTheirs, &b)) {
        snprintf(why, len, "trade_send: it does not send a change of the offer through the allocator, the partner's"
            " offer's reader after it (client patched?)");
        return false;
    }
    s.allocator = shape_target(in, a, 2);
    s.theirs = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(shape_ptr(b, 0)));
    memcpy(&s.stage[kOfferTrade], &stage, sizeof(stage));
    memcpy(&s.ok[kOfferTrade], &ok, sizeof(ok));
    memcpy(&s.trade_cancel, &cancel, sizeof(cancel));
    memcpy(&s.trade_close, &close, sizeof(close));

    // the trade's open: its mode, the partner's name
    s.trade_open = asked_find(in, "trade_open", kSigTradeOpen, why, len);
    if (!s.trade_open) {
        return false;
    }
    const uint8_t* name = asked_target(in, s.trade_open + kTradeOpenName, 0xE8);
    if (asked_target(in, s.trade_open + kTradeOpenCancel[0], 0xE8) != cancel
        || asked_target(in, s.trade_open + kTradeOpenCancel[1], 0xE8) != cancel
        || !shape_once(in, s.trade_open, kTradeOpenBytes, kShapeOfferMode, &a)
        || !asked_names_key(in, a.at + a.hole[3], "money") || shape_ptr(a, 4) != in.mcb
        || shape_target(in, a, 5) != in.open || !shape_target(in, a, 2) || !asked_data(in, shape_ptr(a, 0), 4)
        || !shape_once(in, s.trade_open, kTradeOpenBytes, kShapeTradeOpenTail, &b)
        || shape_target(in, b, 0) != cancel || shape_target(in, b, 1) != close) {
        snprintf(why, len, "trade_open: it does not hand the inventory list the trade's mode, canceling and closing"
            " as the engine's cancel does (client patched?)");
        return false;
    }
    s.mode[kOfferTrade] = const_cast<uint8_t**>(reinterpret_cast<uint8_t* const*>(shape_ptr(a, 0)));
    const uint8_t* list = shape_ptr(a, 1);
    const uint8_t* set_mode = shape_target(in, a, 2);
    if (!shape_is(in, name, kShapeActorName, &b) || !asked_data(in, shape_ptr(b, 0), 4)) {
        snprintf(why, len, "trade_open: the reader of the partner's name is not the actor table's (client"
            " patched?)");
        return false;
    }
    s.actors = reinterpret_cast<const uint8_t* const*>(shape_ptr(b, 0));
    s.actor_titled = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(shape_ptr(b, 1)));
    s.actor_title = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(shape_ptr(b, 2)));
    s.actor_name = b.at[b.hole[3]];

    // the NPC trade
    s.handover_input = asked_find(in, "handover_input", kSigHandoverInput, why, len);
    if (!s.handover_input) {
        return false;
    }
    input = s.handover_input;
    stage = askers ? asked_target(in, static_cast<const uint8_t*>(askers[kAskHandoverItem]) + kAskHandoverItemStage,
        0xE8) : NULL;
    ok_row = input + kHandoverOkJump + 1 + static_cast<int8_t>(input[kHandoverOkJump]);
    cancel_row = input + kHandoverCancelJump + 1 + static_cast<int8_t>(input[kHandoverCancelJump]);
    if (rdptr(input, kHandoverList) != list || !shape_once(in, input, kHandoverInputBytes, kShapeHandoverPick, &a)
        || shape_ptr(a, 0) != in.mcb || !shape_is(in, ok_row, kShapeHandoverOkRow, &b)
        || cancel_row != ok_row + kHandoverCloseRowAt) {
        snprintf(why, len, "handover_input: its rows do not pick into +0x40, send on row 9 and close on row 10"
            " (client patched?)");
        return false;
    }
    ok = shape_target(in, b, 0);
    close = shape_target(in, b, 1);
    const uint8_t* open_hook = text_vtable_routine(in, input, kHandoverOpenSlot, NULL);
    if (!shape_is(in, ok, kShapeHandoverOk, &a) || !shape_once(in, ok, kHandoverOkBytes, kShapeHandoverSend, &c)
        || shape_target(in, c, 0) != s.allocator || !shape_is(in, close, kShapeHandoverClose, &d)
        || shape_ptr(d, 0) != in.mcb || shape_target(in, d, 1) != close_instance || !stage
        || !shape_is(in, stage, kShapeHandoverStage, &a)) {
        snprintf(why, len, "handover_input: its OK, its close or its stage is not the one the engine calls (client"
            " patched?)");
        return false;
    }
    if (!open_hook || !shape_once(in, open_hook, kHandoverOpenBytes, kShapeHandoverMode, &a)
        || shape_ptr(a, 0) != list || shape_target(in, a, 2) != set_mode || !asked_data(in, shape_ptr(a, 1), 4)) {
        snprintf(why, len, "handover_input: its open hook does not hand the inventory list its mode (client"
            " patched?)");
        return false;
    }
    s.mode[kOfferHandover] = const_cast<uint8_t**>(reinterpret_cast<uint8_t* const*>(shape_ptr(a, 1)));
    memcpy(&s.stage[kOfferHandover], &stage, sizeof(stage));
    memcpy(&s.ok[kOfferHandover], &ok, sizeof(ok));
    memcpy(&s.handover_close, &close, sizeof(close));

    // the allocator, its throttle and the clock; the item record. The clock
    // is never called: kShapeClock pins it to the words clock_read() reads.
    if (!shape_is(in, s.allocator, kShapeAllocator, &a) || shape_ptr(a, 0) != reinterpret_cast<const uint8_t*>(s.player)
        || reinterpret_cast<uintptr_t>(shape_ptr(a, 2)) <= kSendHandover || !shape_target(in, a, 7)
        || !asked_data(in, shape_ptr(a, 8), 4) || !shape_is(in, shape_target(in, a, 7), kShapeClock, &b)
        || !asked_data(in, shape_ptr(b, 0), 4)) {
        snprintf(why, len, "trade_send: its allocator does not throttle by the player's data's last sends and the"
            " game's clock (client patched?)");
        return false;
    }
    s.packets = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(shape_ptr(a, 2)));
    s.last_send = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(shape_ptr(a, 5)));
    s.intervals = const_cast<uint8_t**>(reinterpret_cast<uint8_t* const*>(shape_ptr(a, 8)));
    s.interval = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(shape_ptr(a, 9)));
    s.clock_object = const_cast<uint8_t**>(reinterpret_cast<uint8_t* const*>(shape_ptr(b, 0)));
    const uint8_t* item = money_setup ? asked_target(in, money_setup + kMoneySetupItemCall, 0xE8) : NULL;
    if (!item || !shape_is(in, item, kShapeItemRecord, &a)
        || shape_ptr(a, 0) != reinterpret_cast<const uint8_t*>(s.player)) {
        snprintf(why, len, "moneyctr_setup: the item record it reads the gil from is not the player's data's (client"
            " patched?)");
        return false;
    }
    s.inventory = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(shape_ptr(a, 1)));
    memcpy(&s.item, &item, sizeof(item));

    // the inventory list's gray test
    s.rows = asked_find(in, "inventor_rows", kSigInventorRows, why, len);
    if (!s.rows) {
        return false;
    }
    const uint16_t lock = static_cast<uint16_t>(rd16(s.rows, kRowsLock));
    const uint8_t* data_fn = asked_target(in, s.rows + kRowsItemData, 0xE8);
    const uint8_t* describe = asked_target(in, s.rows + kRowsDescribe, 0xE8);
    if (static_cast<uint16_t>(rd16(s.rows, kRowsLockAgain)) != lock || !data_fn || !describe
        || !asked_data(in, rdptr(s.rows, kRowsCache), 4)
        || !shape_once(in, s.rows, kRowsBytes, kShapeRowsTest, &a)
        || static_cast<uint16_t>(rd16(a.at, a.hole[0])) != lock) {
        snprintf(why, len, "inventor_rows: it does not lock an item's data around its mode's test of the row"
            " (client patched?)");
        return false;
    }
    const uint8_t* classes = shape_ptr(a, 2);
    const uint8_t* cases = shape_ptr(a, 3);
    const uint8_t* end = a.at + 96;
    if (!asked_code(in, classes, kStatusClasses) || !asked_code(in, cases, 4 * kRowsCases)
        || rdptr(cases, 0) != end || rdptr(cases, 4) != a.at + kRowsModalCase
        || rdptr(cases, 8) != a.at + kRowsGrayCase || !asked_data(in, shape_ptr(a, 4), 4)) {
        snprintf(why, len, "inventor_rows: its status rule is not the one the engine runs (client patched?)");
        return false;
    }
    for (int k = 0; k < kStatusClasses; ++k) {
        if (classes[k] >= kRowsCases) {
            snprintf(why, len, "inventor_rows: its status rule names a case it does not have (client patched?)");
            return false;
        }
        s.status_class[k] = classes[k];
    }
    s.cache = const_cast<uint8_t**>(reinterpret_cast<uint8_t* const*>(rdptr(s.rows, kRowsCache)));
    s.lock = lock;
    s.status_mode = const_cast<uint8_t**>(reinterpret_cast<uint8_t* const*>(shape_ptr(a, 4)));
    memcpy(&s.item_data, &data_fn, sizeof(data_fn));
    memcpy(&s.describe, &describe, sizeof(describe));
    *out = s;
    return true;
}

// Shape hole `i` pushes window nm's key, `i` + 1 is the manager and `i` + 2
// a call of the open routine: an open by name.
inline bool shape_opens(const AskedInputs& in, const Shape& s, int i, const char* nm) {
    return i + 2 < s.holes && asked_names_key(in, s.at + s.hole[i], nm) && shape_ptr(s, i + 1) == in.mcb
        && shape_target(in, s, i + 2) == in.open;
}

// The openers of the sets past equipment, ability and magic, each where the
// player's own menu row, command or item use runs it. Every open must name its
// window's key through the manager's open routine, on the controllers in the
// windows' registry slots.
inline bool resolve_set_openers(const AskedInputs& in, OpenerSites* s, char* why, size_t len) {
    Shape a;
    Shape b;
    const uint8_t* fn;

    // the map
    const uint8_t* map = asked_find(in, "map_open", kSigMapOpen, why, len);
    if (!map) {
        return false;
    }
    const uint8_t* frame = asked_slot(in, "mapframe");
    const uint8_t* routine = NULL;
    if (shape_is(in, map + kMapOpenFrameAt, kShapeMapFrame, &a) && frame && shape_ptr(a, 0) == frame
        && shape_ptr(a, 5) == frame) {
        routine = shape_target(in, a, 6);
    }
    if (!routine || !shape_is(in, routine + kMapRoutineOpensAt, kShapeMapOpens, &b)
        || !shape_opens(in, b, 0, "mapframe") || !shape_opens(in, b, 3, "map0")) {
        snprintf(why, len, "map_open: it does not hand the controller in mapframe's registry slot to a routine that"
            " opens mapframe and map0 by name through the menu manager's open routine (client patched?)");
        return false;
    }
    memcpy(&s->map, &map, sizeof(map));

    // the blue magic set and the automaton's
    const uint8_t* spells = asked_find(in, "set_spells_open", kSigSetSpellsOpen, why, len);
    if (!spells) {
        return false;
    }
    if (!shape_is(in, spells + kSetSpellsOpensAt, kShapeSetSpellsOpens, &a)
        || shape_ptr(a, 0) != asked_slot(in, "persona") || !shape_opens(in, a, 1, "persona")
        || shape_ptr(a, 4) != asked_slot(in, "bluehelp") || !shape_target(in, a, 5)
        || shape_ptr(a, 7) != asked_slot(in, "bluinven") || !shape_target(in, a, 8)
        || !asked_data(in, shape_ptr(a, 6), 4) || !shape_opens(in, a, 9, "bluepoin")
        || !shape_opens(in, a, 12, "bluequip")) {
        snprintf(why, len, "set_spells_open: it does not open persona, bluehelp, bluinven, bluepoin and bluequip as"
            " the engine knows them (client patched?)");
        return false;
    }
    const uint8_t* attachments = asked_find(in, "attachments_open", kSigAttachmentsOpen, why, len);
    if (!attachments) {
        return false;
    }
    if (!shape_is(in, attachments + kAttachmentsOpensAt, kShapeAttachmentsOpens, &b)
        || shape_ptr(b, 0) != asked_slot(in, "bluehelp") || shape_target(in, b, 1) != shape_target(in, a, 5)
        || shape_ptr(b, 3) != asked_slot(in, "bluinven") || shape_target(in, b, 4) != shape_target(in, a, 8)
        || !asked_data(in, shape_ptr(b, 2), 4) || !shape_opens(in, b, 5, "statpup")
        || !shape_opens(in, b, 8, "automato")) {
        snprintf(why, len, "attachments_open: it does not open bluehelp and bluinven as set_spells_open does, then"
            " statpup and automato (client patched?)");
        return false;
    }
    memcpy(&s->set_spells, &spells, sizeof(spells));
    memcpy(&s->attachments, &attachments, sizeof(attachments));

    // the linkshell list, behind its row's check
    s->linkshell_row = asked_find(in, "linkshell_row", kSigLinkshellRow, why, len);
    if (!s->linkshell_row) {
        return false;
    }
    const uint8_t* check = asked_target(in, s->linkshell_row + kLinkshellCheckCall, 0xE8);
    fn = asked_target(in, s->linkshell_row + kLinkshellOpenCall, 0xE8);
    if (!check || !fn || rdptr(s->linkshell_row, kLinkshellSlot) != asked_slot(in, "link5")
        || !shape_is(in, fn + kLinkshellTailAt, kShapeLinkshellTail, &a)
        || !asked_names_key(in, a.at + a.hole[0], "link5") || shape_ptr(a, 1) != in.mcb
        || shape_target(in, a, 2) != in.open) {
        snprintf(why, len, "linkshell_row: it does not run the linkshell list's opener on the controller in link5's"
            " registry slot after its check, the opener ending in the open of link5 by name (client patched?)");
        return false;
    }
    memcpy(&s->linkshell_check, &check, sizeof(check));
    memcpy(&s->linkshell, &fn, sizeof(fn));

    // the Monstrosity instincts
    const uint8_t* instincts = asked_find(in, "instincts_open", kSigInstinctsOpen, why, len);
    if (!instincts) {
        return false;
    }
    Shape c;
    if (!shape_is(in, instincts, kShapeInstinctsOpen, &a) || shape_ptr(a, 3) != asked_slot(in, "mp_stat")
        || shape_ptr(a, 5) != asked_slot(in, "mp_cstm") || !shape_opens(in, a, 7, "mp_mncst")
        || !shape_is(in, shape_target(in, a, 4), kShapeMpStatOpen, &b) || !shape_opens(in, b, 0, "mp_stat")
        || !shape_is(in, shape_target(in, a, 6), kShapeMpCstmOpen, &c) || !shape_opens(in, c, 0, "mp_cstm")) {
        snprintf(why, len, "instincts_open: it does not open mp_stat, mp_cstm and mp_mncst by name through the menu"
            " manager's open routine, the first two on the controllers in their registry slots (client patched?)");
        return false;
    }
    memcpy(&s->instincts, &instincts, sizeof(instincts));

    // the item use: synthesis for a crystal, the maze editor for a tabula
    s->item_use = asked_find(in, "item_use", kSigItemUse, why, len);
    if (!s->item_use) {
        return false;
    }
    const uint8_t* data = NULL;
    const uint8_t* tabula = NULL;
    const uint8_t* use_check = NULL;
    const uint8_t* synthesis = NULL;
    if (shape_is(in, s->item_use, kShapeItemUse, &a) && asked_data(in, shape_ptr(a, 0), 4)
        && shape_ptr(a, 4) == asked_slot(in, "prvdungp") && shape_ptr(a, 7) == asked_slot(in, "prvdungp")
        && shape_ptr(a, 12) == asked_slot(in, "tskill1") && shape_target(in, a, 5) == shape_target(in, a, 8)
        && reinterpret_cast<uintptr_t>(shape_ptr(a, 3)) < 0x10000) {
        data = shape_target(in, a, 1);
        tabula = shape_target(in, a, 5);
        use_check = shape_target(in, a, 10);
        synthesis = shape_target(in, a, 13);
    }
    const uint8_t* setup = tabula && shape_is(in, tabula + kTabulaSetupAt, kShapeTabulaSetup, &b)
        ? shape_target(in, b, 0) : NULL;
    Shape keeps;
    if (!data || !use_check || !synthesis || !setup
        || !shape_is(in, synthesis + kSynthesisOpensAt, kShapeSynthesisOpens, &b) || !shape_opens(in, b, 0, "tskill1")
        || !shape_is(in, synthesis + kSynthesisKeepsAt, kShapeSynthesisKeeps, &keeps)
        || shape_ptr(keeps, 0) != asked_slot(in, "inventor")
        || !shape_is(in, setup + kTabulaOpensAt, kShapeTabulaOpens, &c) || !shape_opens(in, c, 0, "prvdungp")) {
        snprintf(why, len, "item_use: it does not run the maze editor's opener on the controller in prvdungp's"
            " registry slot for a tabula, and the synthesis's on tskill1's for a crystal after its check, each opening"
            " its window by name, the synthesis's keeping inventor's list (client patched?)");
        return false;
    }
    s->inventor_list = keeps.at[keeps.hole[1]];
    s->code = in.text;
    s->code_size = in.size;
    s->item_cache = const_cast<uint8_t**>(reinterpret_cast<uint8_t* const*>(shape_ptr(a, 0)));
    s->item_type = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(shape_ptr(a, 3)));
    memcpy(&s->item_data, &data, sizeof(data));
    memcpy(&s->use_check, &use_check, sizeof(use_check));
    memcpy(&s->synthesis, &synthesis, sizeof(synthesis));
    memcpy(&s->tabula, &tabula, sizeof(tabula));

    // the macro editor, as the macro set menu's edit row runs it
    s->macro_edit = asked_find(in, "macro_edit", kSigMacroEdit, why, len);
    if (!s->macro_edit) {
        return false;
    }
    fn = NULL;
    if (shape_is(in, s->macro_edit, kShapeMacroEdit, &a) && shape_ptr(a, 0) == asked_slot(in, "mcrselec")
        && asked_code(in, shape_ptr(a, 4), 4) && rdptr(shape_ptr(a, 4), 0) == s->macro_edit + kMacroEditRow
        && shape_target(in, a, 5) && asked_data(in, shape_ptr(a, 6), 4)) {
        fn = shape_target(in, a, 7);
    }
    if (!fn || !shape_is(in, fn, kShapeMacroEditorOpens, &b) || shape_ptr(b, 0) != in.mcb
        || !asked_names_key(in, b.at + b.hole[1], "mcrbedit") || shape_target(in, b, 2) != in.open
        || !shape_opens(in, b, 3, "mcr2edit") || !shape_opens(in, b, 6, "mcr1edit") || shape_target(in, b, 9) != in.open
        || !shape_opens(in, b, 10, "mcr1edit") || !shape_opens(in, b, 13, "mcr2edit")) {
        snprintf(why, len, "macro_edit: its edit row does not run the macro editor's opener, which opens mcrbedit,"
            " mcr1edit and mcr2edit by name through the menu manager's open routine (client patched?)");
        return false;
    }
    s->macro_editor = const_cast<uint8_t**>(reinterpret_cast<uint8_t* const*>(shape_ptr(a, 6)));
    memcpy(&s->macros, &fn, sizeof(fn));
    return true;
}

// The equipment set's opener, the Monstrosity check the equipment shortcut
// asks before it, and the magic list's opener, whose signature has the
// manager pinned before the scan. Opens and controllers are checked as
// resolve_set_openers() checks them.
inline bool resolve_openers(const AskedInputs& in, OpenerSites* out, char* why, size_t len) {
    OpenerSites s;
    memset(&s, 0, sizeof(s));

    const uint8_t* equip = asked_find(in, "equip_open", kSigEquipOpen, why, len);
    if (!equip) {
        return false;
    }
    const uint8_t* slot = asked_slot(in, "equip");
    bool shaped = slot && equip[kEquipOpenGuard] == 0xa1 && rdptr(equip, kEquipOpenGuard + 1) == slot
        && asked_bytes(in, equip + kEquipOpenGuardWordAt, kEquipOpenGuardWord, sizeof(kEquipOpenGuardWord));
    if (!shaped) {
        snprintf(why, len, "equip_open: its guard does not read the controller in equip's registry slot and test its"
            " word +0x1A (client patched?)");
        return false;
    }
    for (int i = 0; i < kEquipOpenCount && shaped; ++i) {
        const uint8_t* at = equip + kEquipOpenKeys[i];
        shaped = asked_code(in, equip, kEquipOpenCalls[i] + 5) && at[0] == 0x68
            && asked_names_key(in, at + 1, kEquipOpenWindows[i]) && at[kEquipOpenManager] == 0xb9
            && rdptr(at, kEquipOpenManager + 1) == in.mcb
            && asked_target(in, equip + kEquipOpenCalls[i], 0xe8) == in.open;
    }
    if (!shaped) {
        snprintf(why, len, "equip_open: it does not open persona, money and equip by name through the menu manager's"
            " open routine (client patched?)");
        return false;
    }

    s.shortcut = asked_find(in, "equip_shortcut", kSigEquipShortcut, why, len);
    if (!s.shortcut) {
        return false;
    }
    const uint8_t* check = asked_target(in, s.shortcut + kEquipShortcutCheck, 0xe8);
    Shape m;
    if (rdptr(s.shortcut, kEquipShortcutSlot) != slot
        || asked_target(in, s.shortcut + kEquipShortcutOpen, 0xe8) != equip) {
        snprintf(why, len, "equip_shortcut: it does not run equip_open on the controller in equip's registry slot");
        return false;
    }
    if (!check || !shape_is(in, check, kShapeMonstrosity, &m) || !asked_data(in, shape_ptr(m, 0), 4)
        || !asked_data(in, shape_ptr(m, 2), 1)) {
        snprintf(why, len, "equip_shortcut: the check it asks first is not the Monstrosity check (client patched?)");
        return false;
    }

    uint8_t bytes[64];
    char mask[65];
    if (!parse_signature(kSigMagicOpen, bytes, mask, sizeof(bytes))) {
        snprintf(why, len, "signature magic_open: malformed");
        return false;
    }
    memcpy(bytes + kMagicOpenManagerImm, &in.mcb, 4);
    memset(mask + kMagicOpenManagerImm, 'x', 4);
    ScanResult r;
    scan(in.text, in.size, bytes, mask, false, &r);
    if (r.count != 1) {
        if (r.count == 0) {
            snprintf(why, len, "signature magic_open: not found (client patched?)");
        } else {
            snprintf(why, len, "signature magic_open: matched %d times", r.count);
        }
        return false;
    }
    const uint8_t* magic = r.hits[0];
    if (!asked_names_key(in, magic + kMagicOpenKeyImm, "magic")
        || asked_target(in, magic + kMagicOpenCall, 0xe8) != in.open
        || rdptr(magic, kMagicOpenSlot) != asked_slot(in, "magic")) {
        snprintf(why, len, "magic_open: it does not open magic by name through the menu manager's open routine and"
            " hand on to the controller in magic's registry slot (client patched?)");
        return false;
    }

    memcpy(&s.equipment, &equip, sizeof(s.equipment));
    memcpy(&s.monstrosity, &check, sizeof(s.monstrosity));
    memcpy(&s.magic, &magic, sizeof(s.magic));
    if (!resolve_set_openers(in, &s, why, len)) {
        return false;
    }
    *out = s;
    return true;
}

// The routes open(name, arg) runs and the openers of the sets past macros,
// each read where the game's own menu row, command or list runs it. A
// signature at a call site is never run; one at a routine's entry is that
// routine. Routines built alike for several windows are told apart by the key
// they open.

// The storage list's opener (this = storage's or storage2's controller;
// mode): the mode at +0x14, then storage for 0, storage2 for 1.
const char kSigStorageOpen[] =
    "8b 44 24 04 6a 00 89 41 14 b9 ?? ?? ?? ?? 85 c0 6a 01 75 0d 68 ?? ?? ?? ?? e8 ?? ?? ?? ?? c2 04 00 68 ?? ?? ?? ?? e8";

// The post menu's opener (this = mogpost's controller; mode): the mode at
// +0x14, then the open, keyed at kFieldOpenKey.
const char kSigMogpostOpen[] =
    "8b 44 24 04 6a 00 89 41 14 6a 01 68 ?? ?? ?? ?? b9 ?? ?? ?? ?? e8 ?? ?? ?? ?? c2 04 00";
const size_t kFieldOpenKey = 12;

// The job change menu's main and support rows write the row into a word (hole
// 1) of jobcselu's controller, then open by the key the language's lookup (6)
// answers. Its confirm, never run, tests the Mog House manager's global (hole
// 0) or the Nomad Moogle service's (hole 4), reading the row from that word
// (holes 1 and 5).
const char kSigJobChangeRow[] =
    "8b 0d ?? ?? ?? ?? 85 c9 74 03 89 41 ?? 6a 00 6a 01 6a 06 e8 ?? ?? ?? ?? 83 c4 04 b9 ?? ?? ?? ?? 50 e8 ?? ?? ?? ?? c2 08 00";
const char kSigJobChangeConfirm[] =
    "8b 0d ?? ?? ?? ?? 85 c9 74 1a 83 7e ?? 01 75 0a 6a 00 50 e8 ?? ?? ?? ?? eb 27 50 6a 00 e8 ?? ?? ?? ?? eb 1d 8b 0d ?? ?? ?? ?? 85 c9 74 13 83 7e ?? 01";

// The friend list's text command closes friend's controller and hands it to
// the friend list's opener with 0, which ends at kFriendOpensAt in friend's
// open, then titlehan's when its controller exists.
const char kSigFriendCommand[] =
    "8b 0d ?? ?? ?? ?? 85 c9 74 0e 8b 41 08 85 c0 75 07 6a 00 e8 ?? ?? ?? ?? 8b 44 24 0c c3";
const size_t kFriendOpensAt = 0xbf;
const char kShapeFriendOpens[] =
    "6a 00 6a 01 68 ?? ?? ?? ?? b9 ?? ?? ?? ?? e8 ?? ?? ?? ?? 8b 46 08 5f 5e 5b 85 c0 74 1c a1 ?? ?? ?? ?? 85 c0 74 13 6a"
    " 00 6a 01 68 ?? ?? ?? ?? b9 ?? ?? ?? ?? e8 ?? ?? ?? ?? c2 04 00";

// The friend menu's Messages row runs the mail list's opener on msglist's
// controller with (0, 0, 0), printing its refusal (al 0). The opener ends at
// kMsglistOpensAt in msglist's open, then titlehan's when its controller
// exists.
const char kSigMessagesRow[] =
    "8b 0d ?? ?? ?? ?? 85 c9 74 2b 6a 00 6a 00 6a 00 e8 ?? ?? ?? ?? 84 c0 75 1c 8b 0d ?? ?? ?? ?? 6a 7a e8";
const size_t kMsglistOpensAt = 0x76;
const char kShapeMsglistOpens[] =
    "6a 00 6a 01 68 ?? ?? ?? ?? b9 ?? ?? ?? ?? e8 ?? ?? ?? ?? a1 ?? ?? ?? ?? 85 c0 74 13 6a 00 6a 01 68 ?? ?? ?? ?? b9 ??"
    " ?? ?? ?? e8 ?? ?? ?? ?? b0 01 c2 0c 00 32 c0 c2 0c 00";

// The blacklist command's call of the blacklist's opener on blklist's
// controller; the opener ends at kBlklistOpensAt in blklist's open.
const char kSigBlacklistCommand[] =
    "8b 0d ?? ?? ?? ?? e8 ?? ?? ?? ?? 8b 44 24 14 5f 5e c3";
const size_t kBlklistOpensAt = 0x47;
const char kShapeBlklistOpens[] = "6a 00 6a 01 68 ?? ?? ?? ?? b9 ?? ?? ?? ?? e8 ?? ?? ?? ?? 5e c3";

// The mount list's opener (list, sort; cdecl): abisortw for sort 1, mount,
// then its setup on mount's controller, which opens subwindo.
const char kSigMountOpen[] =
    "80 7c 24 08 01 75 13 6a 00 6a 01 68 ?? ?? ?? ?? b9 ?? ?? ?? ?? e8 ?? ?? ?? ?? 6a 00 6a 01 68 ?? ?? ?? ?? b9 ?? ?? ?? ?? e8 ?? ?? ?? ?? 8b 44 24 04 8b 0d ?? ?? ?? ?? 50 e8";
const char kShapeMountSetup[] =
    "8b 41 68 53 33 db 50 89 59 64 e8 ?? ?? ?? ?? a1 ?? ?? ?? ?? 53 6a 01 68 ?? ?? ?? ?? b9 ?? ?? ?? ?? c7 40 28 0a 00 00"
    " 00 89 58 20 89 58 24 88 58 30 89 58 34 c7 40 2c e8 03 00 00 e8 ?? ?? ?? ??";

// The Job Points job list's test of a job (1..22 at level 99; `this`
// unread), and the job's category list's opener (this = jbpcat's controller;
// job), which writes the job at +0x54 and the page at +0x58.
const char kSigJobPointsTest[] =
    "8b 44 24 04 3c 01 72 15 3c 16 77 11 50 e8 ?? ?? ?? ?? 83 c4 04 3c 63 0f 93 c0 c2 04 00 32 c0 c2 04 00";
const char kSigJobPointsOpen[] =
    "8a 44 24 04 6a 00 88 41 54 c7 41 58 00 00 00 00 6a 01 68 ?? ?? ?? ?? b9 ?? ?? ?? ?? e8 ?? ?? ?? ?? c2 04 00";

// fp_cat2's opener (this = its controller; category): the category at +0x50,
// then the open, keyed at kFieldOpenKey.
const char kSigFpCat2Open[] =
    "8a 44 24 04 6a 00 88 41 50 6a 01 68 ?? ?? ?? ?? b9 ?? ?? ?? ?? e8 ?? ?? ?? ?? c2 04 00";

// The main menu's Search row resets the search on scresult's controller with
// 0, then opens searchma. The results list's narrow row opens scsibori only
// while the results' mode (the word of hole 0) is 1 or 2, a search run.
const char kSigSearchRow[] =
    "8b 0d ?? ?? ?? ?? 85 c9 74 07 6a 00 e8 ?? ?? ?? ?? 6a 00 6a 01 68 ?? ?? ?? ?? b9 ?? ?? ?? ?? e8";
const char kSigResultsNarrow[] =
    "8b 76 ?? 83 fe 01 74 0c 83 fe 02 74 07 5f 32 c0 5e c2 04 00 6a 00 6a 01 68 ?? ?? ?? ?? b9 ?? ?? ?? ?? e8 ?? ?? ?? ?? 5f b0 01 5e";

// comgenre's opener (this = its controller; mode) refuses a mode of 3 or more
// and writes it at +0x14 before the open. The comment sub-lists' opener
// (this = the list's controller; mode, base) is alike for sccompar, sccomite
// and sccomls.
const char kSigComgenreOpen[] =
    "66 8b 44 24 04 66 85 c0 7c 1d 66 3d 03 00 7d 17 6a 00 66 89 41 14 6a 01 68 ?? ?? ?? ?? b9 ?? ?? ?? ?? e8 ?? ?? ?? ?? c2 04 00";
const char kSigCommentSubOpen[] =
    "8b 44 24 04 83 f8 03 73 1d 89 41 14 8b 44 24 08 6a 00 89 41 18 6a 01 68 ?? ?? ?? ?? b9 ?? ?? ?? ?? e8 ?? ?? ?? ?? c2 08 00";
const size_t kCommentSubKey = 24;

// The inventory's compare option: the item panel's set-and-open on
// itemxinf's controller with mode 5, the item's id, slot and bag, its
// record's extra data (the record + the byte of hole 3), and equip's and
// inventor's controllers.
const char kSigCompareSite[] =
    "8b 0d ?? ?? ?? ?? 8b 15 ?? ?? ?? ?? 51 8b 0d ?? ?? ?? ?? 83 c6 ?? 52 56 50 53 55 6a 05 e8";

// The timers' opener (this = the timer's controller; kind, ticks), alike for
// casttime and eventtim.
const char kSigTimerOpen[] =
    "8b 44 24 04 89 41 20 8b 44 24 08 85 c0 7d 08 c7 44 24 08 00 00 00 00 db 44 24 08 6a 00 6a 01 68 ?? ?? ?? ?? d9 51 18 d9 59 14 b9 ?? ?? ?? ?? e8 ?? ?? ?? ?? c2 08 00";
const size_t kTimerKey = 32;

// The main menu's Status row: in Monstrosity (the check of hole 0)
// mp_stat's opener on its controller with 0, else persona's layout byte
// (hole 7) set 1 and persona's open.
const char kSigStatusRow[] =
    "e8 ?? ?? ?? ?? 84 c0 6a 00 74 14 8b 0d ?? ?? ?? ?? e8 ?? ?? ?? ?? e8 ?? ?? ?? ?? 5e c2 08 00 a1 ?? ?? ?? ?? 6a 01 68 ?? ?? ?? ?? b9 ?? ?? ?? ?? c6 40 ?? 01 e8";

// The configuration menu's message rows: conf11l's set word (holes 1 and
// 3) written 1 or 0, then conf11l's open.
const char kSigConf11mRows[] =
    "8b 15 ?? ?? ?? ?? c7 42 ?? 01 00 00 00 eb 0c a1 ?? ?? ?? ?? c7 40 ?? 00 00 00 00 6a 00 6a 01 b9 ?? ?? ?? ?? 68 ?? ?? ?? ?? e8";

// The quest log command: quest00's mode word (hole 1) written 0, then
// quest00's open. quest00's confirm: two calls on its own controller,
// quest01's open, then quest01's setup on its controller with the row.
const char kSigQuestCommand[] =
    "a1 ?? ?? ?? ?? 85 c0 74 07 c7 40 ?? 00 00 00 00 6a 00 6a 01 68 ?? ?? ?? ?? b9 ?? ?? ?? ?? e8 ?? ?? ?? ?? 8b 44 24 0c c3";
const char kSigQuest00Confirm[] =
    "8b ce e8 ?? ?? ?? ?? 8b ce e8 ?? ?? ?? ?? 6a 00 6a 01 68 ?? ?? ?? ?? b9 ?? ?? ?? ?? e8 ?? ?? ?? ?? 8b 56 30 8b 0d ?? ?? ?? ?? 52 e8 ?? ?? ?? ?? 5e";

// The missions command runs, on miss00's controller, the missions menu's
// setter with 0 (the player's nation's log) or the by-name routine, then
// opens miss00. The setter writes the log word (kShapeMissionsSetter hole 1);
// the by-name routine tests each log's expansion bit (kShapeMissionsByName
// holes: 0 the table, 1 the test).
const char kSigMissionsCommand[] =
    "8b 0d ?? ?? ?? ?? 85 c9 74 34 a1 ?? ?? ?? ?? 83 e8 02 74 09 6a 00 e8 ?? ?? ?? ?? eb 0e a1 ?? ?? ?? ?? 8b 50 04 52 e8 ?? ?? ?? ?? 6a 00 6a 01 68";
const size_t kMissionsOpenAt = 47;
const char kShapeMissionsOpen[] = "68 ?? ?? ?? ?? b9 ?? ?? ?? ?? e8 ?? ?? ?? ?? 8b 44 24 0c c3";
const char kShapeMissionsSetter[] =
    "8b 44 24 04 56 48 8b f1 83 f8 05 77 60 ff 24 85 ?? ?? ?? ?? c7 46 ?? 03 00 00 00 5e c2 04 00";
const char kShapeMissionsByName[] =
    "53 55 8b 6c 24 0c 56 57 8b d1 33 ff 0f be 87 ?? ?? ?? ?? 50 e8 ?? ?? ?? ?? 83 c4 04 84 c0 74 3a";
const int kMissionLogCount = 15;

// map0's Wide Scan row: while the map tracks (the routine of hole 0 answers a
// word of 1), a yes/no on comyn first; else scanlist's opener on its
// controller, which opens scanlist by name.
const char kSigWideScanRow[] =
    "e8 ?? ?? ?? ?? 85 c0 74 2f 83 38 01 75 2a 6a 00 6a 01 6a 00 6a 00 6a 45 6a 0e e8 ?? ?? ?? ?? 8b 0d ?? ?? ?? ?? 83 c4 08 50 6a 00 68 ?? ?? ?? ?? e8 ?? ?? ?? ?? c2 08 00";
const size_t kWideScanOpenAt = 56;
const char kShapeWideScanOpen[] = "8b 0d ?? ?? ?? ?? e8 ?? ?? ?? ?? c2 08 00";
const char kShapeScanlistOpen[] = "56 6a ?? 8b f1 e8 ?? ?? ?? ?? 6a 00 6a 01 68 ?? ?? ?? ?? b9 ?? ?? ?? ??";

// The conquest map's text command runs, on cnqframe's closed controller, the
// Region Info opener (kind, the player's area: the word of hole 2 in the
// player's data), which writes the kind at kShapeRegionKind's hole,
// kRegionKindAt into it.
const char kSigRegionCommand[] =
    "8b 0d ?? ?? ?? ?? 85 c9 74 1e a1 ?? ?? ?? ?? 85 c0 74 15 8b 51 08 85 d2 75 0e 8b 80 ?? ?? ?? ?? 50 6a 00 e8 ?? ?? ?? ?? 8b 44 24 0c c3";
const size_t kRegionKindAt = 0x5e;
const char kShapeRegionKind[] = "89 9e ?? ?? ?? ??";

// cnqframe's Colonization cursor on a region past the first: the region
// written at coloresu's word (hole 1), coloresu's open, colorank's close
// by name.
const char kSigColoresuRow[] =
    "a1 ?? ?? ?? ?? 85 c0 74 06 0f bf d7 89 50 ?? 6a 00 6a 01 68 ?? ?? ?? ?? b9 ?? ?? ?? ?? e8 ?? ?? ?? ?? 68 ?? ?? ?? ?? b9 ?? ?? ?? ?? e8";

// The text color menu's first row writes 0 at textcol1's, textcol2's and
// textcol3's category words (holes 1, 3 and 5); its tail opens textcol2,
// then textcol1.
const char kSigTextColorRows[] =
    "a1 ?? ?? ?? ?? c7 40 ?? 00 00 00 00 8b 0d ?? ?? ?? ?? c7 41 ?? 00 00 00 00 8b 15 ?? ?? ?? ?? c7 42 ?? 00 00 00 00 eb";
const size_t kTextColorOpensAt = 0x92;
const char kShapeTextColorOpens[] =
    "6a 00 6a 01 68 ?? ?? ?? ?? b9 ?? ?? ?? ?? e8 ?? ?? ?? ?? 6a 00 6a 01 68 ?? ?? ?? ?? b9 ?? ?? ?? ?? e8 ?? ?? ?? ??";

// The macro editor's pick (this = the editor; row) stores the row for the
// palette in its byte (hole 0; 1 Ctrl), closes mcrbedit, mcr1edit and
// mcr2edit by name, then opens mcrledit and macro.
const char kSigMacroPick[] =
    "80 79 ?? 01 75 09 8a 44 24 04 88 41 ?? eb 07 8a 54 24 04 88 51 ?? 68 ?? ?? ?? ?? b9 ?? ?? ?? ?? e8 ?? ?? ?? ?? 68 ?? ?? ?? ?? b9 ?? ?? ?? ?? e8";
const size_t kMacroPickOpensAt = 47;
const char kShapeMacroPickOpens[] =
    "e8 ?? ?? ?? ?? 68 ?? ?? ?? ?? b9 ?? ?? ?? ?? e8 ?? ?? ?? ?? 6a 00 6a 01 68 ?? ?? ?? ?? b9 ?? ?? ?? ?? e8 ?? ?? ?? ?? 6a"
    " 00 6a 01 68 ?? ?? ?? ?? b9 ?? ?? ?? ?? e8 ?? ?? ?? ?? c2 04 00";

// The macro palettes' Escape, the same in mcr1edit's and mcr2edit's input
// routines: the editor's close (mcrbedit, mcr1edit and mcr2edit by name),
// then titlewin's step back (0, 2).
const char kSigPaletteEscape[] =
    "8b 4e 24 e8 ?? ?? ?? ?? 8b 0d ?? ?? ?? ?? 5e 5b c7 44 24 0c 02 00 00 00 83 c4 04 c7 44 24 04 00 00 00 00 e9";
const char kShapeMacroClose[] =
    "68 ?? ?? ?? ?? b9 ?? ?? ?? ?? e8 ?? ?? ?? ?? 68 ?? ?? ?? ?? b9 ?? ?? ?? ?? e8 ?? ?? ?? ?? 68 ?? ?? ?? ?? b9 ?? ?? ??"
    " ?? e8 ?? ?? ?? ?? c3";

// The main menu's Items row: the item view (0, 1) on the object its global
// holds, inventor's byte (hole 3) set 1, then inventor's open with the list
// the global of hole 4 holds.
const char kSigItemsRow[] =
    "8b 0d ?? ?? ?? ?? 6a 01 6a 00 e8 ?? ?? ?? ?? 8b 0d ?? ?? ?? ?? c6 81 ?? ?? ?? ?? 01 8b 15 ?? ?? ?? ?? 8b 0d ?? ?? ?? ?? 52 e8";

// The item data's summary builder (the first call within kSummaryBuildSpan
// pushing 0 and the data) copies each word from its pointer at hole 0's
// offset into the summary at the displacement ending hole 2; the type word is
// the one whose pointer the item use reads.
const size_t kSummaryBuildSpan = 0x100;
const size_t kSummaryTypeSpan = 0x200;
const char kShapeSummaryType[] = "8b 86 ?? ?? ?? ?? 85 c0 74 07 66 8b ?? 66 89 ?? ??";

// The macro editor's palette switch (this = the editor; row), Left or Right
// on a palette: the row written for both palettes (holes 1 and 2), the
// palette byte (hole 0) toggled, the other palette's window opened by name,
// then the switch's sound.
const char kSigPaletteSwitch[] =
    "8a 41 ?? ba 01 00 00 00 3a c2 8a 44 24 04 6a 00 52 88 41 ?? 88 41 ?? 75 1b c6 41 ?? 02 68 ?? ?? ?? ?? b9 ?? ?? ?? ?? e8 ?? ?? ?? ?? e8 ?? ?? ?? ?? c2 04 00 88 51 ?? 68";
const size_t kPaletteSwitchBackAt = 55;
const char kShapePaletteSwitchBack[] = "68 ?? ?? ?? ?? b9 ?? ?? ?? ?? e8 ?? ?? ?? ?? e8 ?? ?? ?? ?? c2 04 00";

// The Status screen's close: statcom2's Escape with no row picked closes its
// menu by instance with 1, and its begin-close closes persona and mp_stat by
// name; one vtable names both.
const char kSigStatusEscape[] =
    "66 83 7f 18 00 7d 15 8b 47 08 6a 01 50 b9 ?? ?? ?? ?? e8 ?? ?? ?? ?? 5f 5e c2 08 00 8b cf e8";
const size_t kStatusEscapeAt = 0x1b;
const char kSigStatusClose[] =
    "56 8b f1 68 ?? ?? ?? ?? b9 ?? ?? ?? ?? e8 ?? ?? ?? ?? 68 ?? ?? ?? ?? b9 ?? ?? ?? ?? e8 ?? ?? ?? ?? 8b ce 5e e9";
const size_t kStatusCloseKey = 4;

// textcol1's Escape closes textcol2, then textcol1, by name.
const char kSigTextColorEscape[] =
    "68 ?? ?? ?? ?? b9 ?? ?? ?? ?? e8 ?? ?? ?? ?? 68 ?? ?? ?? ?? b9 ?? ?? ?? ?? e8 ?? ?? ?? ?? e9";
const size_t kTextColorEscapeKey = 1;

// mp_mncst's Escape closes mp_stat and mp_cstm through routines of theirs on
// their controllers (holes 0 and 2), then mp_mncst by name.
const char kSigInstinctsEscape[] =
    "8b 0d ?? ?? ?? ?? e8 ?? ?? ?? ?? 8b 0d ?? ?? ?? ?? e8 ?? ?? ?? ?? 68 ?? ?? ?? ?? b9 ?? ?? ?? ?? e8 ?? ?? ?? ?? c3";
const size_t kInstinctsEscapeKey = 23;
const char kShapeCloseByNameRet[] = "68 ?? ?? ?? ?? b9 ?? ?? ?? ?? e8 ?? ?? ?? ?? c3";

// cnqframe's input routine does nothing while its closing bit is set; on
// Escape (event 6) it starts the fade (vtable +0x50), then closes
// kRegionEscapeNames by name, each a kShapeCloseByName at game.h's
// kRegionEscapeAt + kRegionEscapeStep * i.
const char kSigRegionEscape[] =
    "f6 41 14 40 0f 85 ?? ?? ?? ?? 66 83 7c 24 04 06 0f 85 ?? ?? ?? ?? 8b 01 ff 50 50 68";
const char kShapeCloseByName[] = "68 ?? ?? ?? ?? b9 ?? ?? ?? ?? e8 ?? ?? ?? ??";
const char* const kRegionEscapeNames[] = {"conquer1", "conq3pts", "conq6sta", "beseige1", "beseige2", "imperial",
                                          "campresu", "campsan", "camparea", "allied", "coloresu", "colorank",
                                          "colopoin"};

// The inventory's first free slot, never run: record by record from slot 1
// while below the bag's size (cdecl (bag); al the size, slot 0 counted).
const char kSigFreeSlot[] =
    "a1 ?? ?? ?? ?? 53 85 c0 56 74 2f 6a 00 b3 01 8d b0 ?? ?? ?? ?? e8 ?? ?? ?? ?? 83 c4 04 3a c3 76 19 66 83 3e 00 74 18 6a 00 fe c3 83 c6 2c e8";

// The one place a signature matches with the imm32 at `key_at` naming
// window nm's key, or NULL with `why` filled.
inline const uint8_t* asked_find_keyed(const AskedInputs& in, const char* name, const char* signature, size_t key_at,
                                       const char* nm, char* why, size_t len) {
    uint8_t bytes[64];
    char mask[65];
    const size_t n = parse_signature(signature, bytes, mask, sizeof(bytes));
    if (!n) {
        snprintf(why, len, "signature %s: malformed", name);
        return NULL;
    }
    const uint8_t* found = NULL;
    int count = 0;
    for (size_t i = 0; i + n <= in.size; ++i) {
        if (match_from(in.text + i, bytes, mask, 0) && asked_names_key(in, in.text + i + key_at, nm)) {
            found = in.text + i;
            ++count;
        }
    }
    if (count == 0) {
        snprintf(why, len, "signature %s: not found for %s (client patched?)", name, nm);
        return NULL;
    }
    if (count > 1) {
        snprintf(why, len, "signature %s: matched %d times for %s", name, count, nm);
        return NULL;
    }
    return found;
}

// A shape's hole as the byte it is.
inline uint8_t shape_byte(const Shape& s, int i) {
    return i < s.holes ? s.at[s.hole[i]] : 0;
}

// Hole i pushes window nm's key, then the manager and a call of `routine`.
inline bool shape_calls_named(const AskedInputs& in, const Shape& s, int i, const char* nm, const uint8_t* routine) {
    return i + 2 < s.holes && asked_names_key(in, s.at + s.hole[i], nm) && shape_ptr(s, i + 1) == in.mcb
        && shape_target(in, s, i + 2) == routine;
}

inline bool route_fail(char* why, size_t len, const char* name, const char* says) {
    snprintf(why, len, "%s: %s (client patched?)", name, says);
    return false;
}

// game.h's RouteSites, each opener checked to do what its route needs.
// `item_type` is the item use's offset of the type word's pointer, which the
// summary's builder must read too; the Status screen's Escape and the Wide
// Scan row call `close_instance` and `yesno_ask`.
inline bool resolve_routes(const AskedInputs& in, const OfferSites& offer, uint32_t item_type,
                           const uint8_t* close_instance, const uint8_t* yesno_ask, RouteSites* out, char* why,
                           size_t len) {
    RouteSites r;
    memset(&r, 0, sizeof(r));
    Shape a;
    Shape b;
    const uint8_t* at;
    const uint8_t* fn;

    at = asked_find(in, "storage_open", kSigStorageOpen, why, len);
    if (!at) {
        return false;
    }
    if (!shape_is(in, at, kSigStorageOpen, &a) || shape_ptr(a, 0) != in.mcb || !asked_names_key(in, at + 21, "storage")
        || shape_target(in, a, 2) != in.open || !asked_names_key(in, at + 34, "storage2")
        || asked_target(in, at + 38, 0xE8) != in.open) {
        return route_fail(why, len, "storage_open", "it does not open storage for 0 and storage2 for 1 by name");
    }
    memcpy(&r.storage, &at, sizeof(at));

    at = asked_find_keyed(in, "mogpost_open", kSigMogpostOpen, kFieldOpenKey, "mogpost", why, len);
    if (!at) {
        return false;
    }
    if (!shape_is(in, at, kSigMogpostOpen, &a) || !shape_opens(in, a, 0, "mogpost")) {
        return route_fail(why, len, "mogpost_open", "it does not open mogpost by name");
    }
    memcpy(&r.mogpost, &at, sizeof(at));

    at = asked_find(in, "job_change_row", kSigJobChangeRow, why, len);
    if (!at) {
        return false;
    }
    if (!shape_is(in, at, kSigJobChangeRow, &a) || shape_ptr(a, 0) != asked_slot(in, "jobcselu")
        || !shape_target(in, a, 2) || shape_ptr(a, 3) != in.mcb || shape_target(in, a, 4) != in.open) {
        return route_fail(why, len, "job_change_row", "it does not write the row into the controller in jobcselu's"
            " slot and open the key the language's lookup answers");
    }
    r.job_row = shape_byte(a, 1);
    fn = shape_target(in, a, 2);
    memcpy(&r.language_key, &fn, sizeof(fn));
    at = asked_find(in, "job_change_confirm", kSigJobChangeConfirm, why, len);
    if (!at) {
        return false;
    }
    if (!shape_is(in, at, kSigJobChangeConfirm, &a) || !asked_data(in, shape_ptr(a, 0), 4)
        || !asked_data(in, shape_ptr(a, 4), 4) || shape_byte(a, 1) != r.job_row || shape_byte(a, 5) != r.job_row) {
        return route_fail(why, len, "job_change_confirm", "it does not act through the Mog House's or the Nomad"
            " Moogle service's manager on the row the job change menu writes");
    }
    r.mog_house = const_cast<uint8_t**>(reinterpret_cast<uint8_t* const*>(shape_ptr(a, 0)));
    r.nomad = const_cast<uint8_t**>(reinterpret_cast<uint8_t* const*>(shape_ptr(a, 4)));

    at = asked_find(in, "friend_command", kSigFriendCommand, why, len);
    if (!at) {
        return false;
    }
    fn = shape_is(in, at, kSigFriendCommand, &a) && shape_ptr(a, 0) == asked_slot(in, "friend")
        ? shape_target(in, a, 1) : NULL;
    if (!fn || !shape_is(in, fn + kFriendOpensAt, kShapeFriendOpens, &b) || !shape_opens(in, b, 0, "friend")
        || shape_ptr(b, 3) != asked_slot(in, "titlehan") || !shape_opens(in, b, 4, "titlehan")) {
        return route_fail(why, len, "friend_command", "it does not run, on the controller in friend's slot, an opener"
            " of friend and titlehan by name");
    }
    memcpy(&r.friend_list, &fn, sizeof(fn));

    at = asked_find(in, "messages_row", kSigMessagesRow, why, len);
    if (!at) {
        return false;
    }
    fn = shape_is(in, at, kSigMessagesRow, &a) && shape_ptr(a, 0) == asked_slot(in, "msglist")
        ? shape_target(in, a, 1) : NULL;
    if (!fn || !shape_is(in, fn + kMsglistOpensAt, kShapeMsglistOpens, &b) || !shape_opens(in, b, 0, "msglist")
        || shape_ptr(b, 3) != asked_slot(in, "titlehan") || !shape_opens(in, b, 4, "titlehan")) {
        return route_fail(why, len, "messages_row", "it does not run, on the controller in msglist's slot, an opener"
            " of msglist and titlehan by name");
    }
    memcpy(&r.msglist, &fn, sizeof(fn));

    at = asked_find(in, "blacklist_command", kSigBlacklistCommand, why, len);
    if (!at) {
        return false;
    }
    fn = shape_is(in, at, kSigBlacklistCommand, &a) && shape_ptr(a, 0) == asked_slot(in, "blklist")
        ? shape_target(in, a, 1) : NULL;
    if (!fn || !shape_is(in, fn + kBlklistOpensAt, kShapeBlklistOpens, &b) || !shape_opens(in, b, 0, "blklist")) {
        return route_fail(why, len, "blacklist_command", "it does not run, on the controller in blklist's slot, an"
            " opener of blklist by name");
    }
    memcpy(&r.blklist, &fn, sizeof(fn));

    at = asked_find(in, "mount_open", kSigMountOpen, why, len);
    if (!at) {
        return false;
    }
    fn = shape_is(in, at, kSigMountOpen, &a) && shape_opens(in, a, 0, "abisortw") && shape_opens(in, a, 3, "mount")
        && shape_ptr(a, 6) == asked_slot(in, "mount") ? asked_target(in, at + 56, 0xE8) : NULL;
    if (!fn || !shape_is(in, fn, kShapeMountSetup, &b) || shape_ptr(b, 1) != asked_slot(in, "subwindo")
        || !shape_opens(in, b, 2, "subwindo")) {
        return route_fail(why, len, "mount_open", "it does not open abisortw and mount by name, then run a setup that"
            " opens subwindo on the controller in mount's slot");
    }
    memcpy(&r.mount, &at, sizeof(at));

    at = asked_find(in, "job_points_test", kSigJobPointsTest, why, len);
    if (!at) {
        return false;
    }
    memcpy(&r.job_points_test, &at, sizeof(at));
    at = asked_find(in, "job_points_open", kSigJobPointsOpen, why, len);
    if (!at) {
        return false;
    }
    if (!shape_is(in, at, kSigJobPointsOpen, &a) || !shape_opens(in, a, 0, "jbpcat")) {
        return route_fail(why, len, "job_points_open", "it does not open jbpcat by name");
    }
    memcpy(&r.job_points, &at, sizeof(at));
    at = asked_find_keyed(in, "fp_cat2_open", kSigFpCat2Open, kFieldOpenKey, "fp_cat2", why, len);
    if (!at) {
        return false;
    }
    if (!shape_is(in, at, kSigFpCat2Open, &a) || !shape_opens(in, a, 0, "fp_cat2")) {
        return route_fail(why, len, "fp_cat2_open", "it does not open fp_cat2 by name");
    }
    memcpy(&r.fp_cat2, &at, sizeof(at));

    at = asked_find(in, "search_row", kSigSearchRow, why, len);
    if (!at) {
        return false;
    }
    fn = shape_is(in, at, kSigSearchRow, &a) && shape_ptr(a, 0) == asked_slot(in, "scresult")
        && asked_names_key(in, at + 22, "searchma") && shape_ptr(a, 3) == in.mcb
        && asked_target(in, at + 31, 0xE8) == in.open ? shape_target(in, a, 1) : NULL;
    if (!fn) {
        return route_fail(why, len, "search_row", "it does not reset the search on the controller in scresult's slot"
            " before searchma's open");
    }
    memcpy(&r.search_reset, &fn, sizeof(fn));
    at = asked_find(in, "results_narrow", kSigResultsNarrow, why, len);
    if (!at) {
        return false;
    }
    if (!shape_is(in, at, kSigResultsNarrow, &a) || !shape_opens(in, a, 1, "scsibori")) {
        return route_fail(why, len, "results_narrow", "it does not open scsibori by name");
    }
    r.search_mode = shape_byte(a, 0);
    at = asked_find(in, "comgenre_open", kSigComgenreOpen, why, len);
    if (!at) {
        return false;
    }
    if (!shape_is(in, at, kSigComgenreOpen, &a) || !shape_opens(in, a, 0, "comgenre")) {
        return route_fail(why, len, "comgenre_open", "it does not open comgenre by name");
    }
    memcpy(&r.comgenre, &at, sizeof(at));
    static const char* const subs[3] = {"sccompar", "sccomite", "sccomls"};
    for (int i = 0; i < 3; ++i) {
        at = asked_find_keyed(in, "comment_sub_open", kSigCommentSubOpen, kCommentSubKey, subs[i], why, len);
        if (!at) {
            return false;
        }
        if (!shape_is(in, at, kSigCommentSubOpen, &a) || !shape_opens(in, a, 0, subs[i])) {
            return route_fail(why, len, "comment_sub_open", "it does not open its list by name");
        }
        memcpy(&r.comment_sub[i], &at, sizeof(at));
    }

    at = asked_find(in, "compare_site", kSigCompareSite, why, len);
    if (!at) {
        return false;
    }
    fn = shape_is(in, at, kSigCompareSite, &a) && shape_ptr(a, 0) == asked_slot(in, "inventor")
        && shape_ptr(a, 1) == asked_slot(in, "equip") && shape_ptr(a, 2) == asked_slot(in, "itemxinf")
        ? asked_target(in, at + 29, 0xE8) : NULL;
    if (!fn) {
        return route_fail(why, len, "compare_site", "it does not run the item panel's set-and-open on the controller"
            " in itemxinf's slot beside equip's and inventor's");
    }
    r.item_extra = shape_byte(a, 3);
    memcpy(&r.item_panel, &fn, sizeof(fn));

    static const char* const timers[2] = {"casttime", "eventtim"};
    for (int i = 0; i < 2; ++i) {
        at = asked_find_keyed(in, "timer_open", kSigTimerOpen, kTimerKey, timers[i], why, len);
        if (!at) {
            return false;
        }
        if (!shape_is(in, at, kSigTimerOpen, &a) || shape_ptr(a, 1) != in.mcb || shape_target(in, a, 2) != in.open) {
            return route_fail(why, len, "timer_open", "it does not open its timer by name");
        }
        memcpy(&r.timer[i], &at, sizeof(at));
    }

    at = asked_find(in, "status_row", kSigStatusRow, why, len);
    if (!at) {
        return false;
    }
    fn = shape_is(in, at, kSigStatusRow, &a) && shape_ptr(a, 1) == asked_slot(in, "mp_stat")
        && shape_ptr(a, 4) == asked_slot(in, "persona") && asked_names_key(in, at + 39, "persona")
        && shape_ptr(a, 6) == in.mcb && asked_target(in, at + 52, 0xE8) == in.open ? shape_target(in, a, 2) : NULL;
    if (!fn || !shape_target(in, a, 0) || !shape_is(in, fn, kSigMogpostOpen, &b) || !shape_opens(in, b, 0, "mp_stat")) {
        return route_fail(why, len, "status_row", "it does not run mp_stat's opener in Monstrosity, else set persona's"
            " layout and open persona by name");
    }
    r.persona_layout = shape_byte(a, 7);
    memcpy(&r.mp_stat, &fn, sizeof(fn));

    at = asked_find(in, "conf11m_rows", kSigConf11mRows, why, len);
    if (!at) {
        return false;
    }
    if (!shape_is(in, at, kSigConf11mRows, &a) || shape_ptr(a, 0) != asked_slot(in, "conf11l")
        || shape_ptr(a, 2) != asked_slot(in, "conf11l") || shape_byte(a, 1) != shape_byte(a, 3)
        || shape_ptr(a, 4) != in.mcb || !asked_names_key(in, at + 37, "conf11l")
        || asked_target(in, at + 41, 0xE8) != in.open) {
        return route_fail(why, len, "conf11m_rows", "it does not write conf11l's set into the controller in its slot"
            " before its open by name");
    }
    r.conf11l_set = shape_byte(a, 1);

    at = asked_find(in, "quest_command", kSigQuestCommand, why, len);
    if (!at) {
        return false;
    }
    if (!shape_is(in, at, kSigQuestCommand, &a) || shape_ptr(a, 0) != asked_slot(in, "quest00")
        || !shape_opens(in, a, 2, "quest00")) {
        return route_fail(why, len, "quest_command", "it does not write quest00's mode into the controller in its slot"
            " before its open by name");
    }
    r.quest00_mode = shape_byte(a, 1);
    at = asked_find(in, "quest00_confirm", kSigQuest00Confirm, why, len);
    if (!at) {
        return false;
    }
    if (!shape_is(in, at, kSigQuest00Confirm, &a) || !shape_target(in, a, 0) || !shape_target(in, a, 1)
        || !shape_opens(in, a, 2, "quest01") || shape_ptr(a, 5) != asked_slot(in, "quest01")
        || !shape_target(in, a, 6)) {
        return route_fail(why, len, "quest00_confirm", "it does not open quest01 by name and set it up on the"
            " controller in its slot");
    }
    for (int i = 0; i < 2; ++i) {
        fn = shape_target(in, a, i);
        memcpy(&r.quest00_dim[i], &fn, sizeof(fn));
    }
    fn = shape_target(in, a, 6);
    memcpy(&r.quest01, &fn, sizeof(fn));

    at = asked_find(in, "missions_command", kSigMissionsCommand, why, len);
    if (!at) {
        return false;
    }
    const uint8_t* setter = NULL;
    const uint8_t* by_name = NULL;
    Shape c;
    if (shape_is(in, at, kSigMissionsCommand, &a) && shape_ptr(a, 0) == asked_slot(in, "miss00")
        && shape_is(in, at + kMissionsOpenAt, kShapeMissionsOpen, &b) && asked_names_key(in, b.at + b.hole[0], "miss00")
        && shape_ptr(b, 1) == in.mcb && shape_target(in, b, 2) == in.open) {
        setter = shape_target(in, a, 2);
        by_name = shape_target(in, a, 4);
    }
    if (!setter || !by_name || !shape_is(in, setter, kShapeMissionsSetter, &b)
        || !shape_is(in, by_name, kShapeMissionsByName, &c) || !asked_data(in, shape_ptr(c, 0), kMissionLogCount)
        || !shape_target(in, c, 1)) {
        return route_fail(why, len, "missions_command", "it does not set miss00's log on the controller in its slot,"
            " by the missions menu's setter or by name behind the expansion test, before its open by name");
    }
    r.miss00_log = shape_byte(b, 1);
    r.mission_bits = shape_ptr(c, 0);
    memcpy(&r.miss00_menu, &setter, sizeof(setter));
    fn = shape_target(in, c, 1);
    memcpy(&r.expansion, &fn, sizeof(fn));

    at = asked_find(in, "wide_scan_row", kSigWideScanRow, why, len);
    if (!at) {
        return false;
    }
    fn = shape_is(in, at, kSigWideScanRow, &a) && shape_ptr(a, 2) == asked_slot(in, "comyn")
        && shape_is(in, at + kWideScanOpenAt, kShapeWideScanOpen, &b) && shape_ptr(b, 0) == asked_slot(in, "scanlist")
        ? shape_target(in, b, 1) : NULL;
    if (!fn || !shape_target(in, a, 0) || !shape_is(in, fn, kShapeScanlistOpen, &c)
        || !asked_names_key(in, c.at + c.hole[2], "scanlist") || shape_ptr(c, 3) != in.mcb) {
        return route_fail(why, len, "wide_scan_row", "it does not ask on comyn while the map tracks, else run"
            " scanlist's opener on the controller in its slot");
    }
    if (shape_target(in, a, 1) != in.message || shape_target(in, a, 4) != yesno_ask
        || !asked_code(in, shape_ptr(a, 3), 1)) {
        return route_fail(why, len, "wide_scan_row", "its question is not the game's message asked on comyn through the"
            " yes/no class's ask");
    }
    memcpy(&r.scanlist, &fn, sizeof(fn));
    fn = shape_target(in, a, 0);
    memcpy(&r.tracking, &fn, sizeof(fn));
    r.scan_question = shape_ptr(a, 3);

    at = asked_find(in, "region_command", kSigRegionCommand, why, len);
    if (!at) {
        return false;
    }
    fn = shape_is(in, at, kSigRegionCommand, &a) && shape_ptr(a, 0) == asked_slot(in, "cnqframe")
        && asked_data(in, shape_ptr(a, 1), 4) ? shape_target(in, a, 3) : NULL;
    if (!fn || !shape_is(in, fn + kRegionKindAt, kShapeRegionKind, &b)) {
        return route_fail(why, len, "region_command", "it does not run the Region Info opener on the controller in"
            " cnqframe's slot with the player's area");
    }
    r.player = const_cast<uint8_t**>(reinterpret_cast<uint8_t* const*>(shape_ptr(a, 1)));
    r.area = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(shape_ptr(a, 2)));
    r.region_kind = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(shape_ptr(b, 0)));
    memcpy(&r.region, &fn, sizeof(fn));

    at = asked_find(in, "coloresu_row", kSigColoresuRow, why, len);
    if (!at) {
        return false;
    }
    if (!shape_is(in, at, kSigColoresuRow, &a) || shape_ptr(a, 0) != asked_slot(in, "coloresu")
        || !shape_opens(in, a, 2, "coloresu") || !asked_names_key(in, at + 35, "colorank") || shape_ptr(a, 6) != in.mcb
        || asked_target(in, at + 44, 0xE8) != in.close) {
        return route_fail(why, len, "coloresu_row", "it does not write the region into the controller in coloresu's"
            " slot, open coloresu and close colorank by name");
    }
    r.coloresu_region = shape_byte(a, 1);

    at = asked_find(in, "text_color_rows", kSigTextColorRows, why, len);
    if (!at) {
        return false;
    }
    if (!shape_is(in, at, kSigTextColorRows, &a) || shape_ptr(a, 0) != asked_slot(in, "textcol1")
        || shape_ptr(a, 2) != asked_slot(in, "textcol2") || shape_ptr(a, 4) != asked_slot(in, "textcol3")
        || !shape_is(in, at + kTextColorOpensAt, kShapeTextColorOpens, &b) || !shape_opens(in, b, 0, "textcol2")
        || !shape_opens(in, b, 3, "textcol1")) {
        return route_fail(why, len, "text_color_rows", "it does not write the category into textcol1's, textcol2's"
            " and textcol3's controllers before textcol2's and textcol1's opens by name");
    }
    for (int i = 0; i < 3; ++i) {
        r.textcol[i] = shape_byte(a, 2 * i + 1);
    }

    at = asked_find(in, "macro_pick", kSigMacroPick, why, len);
    if (!at) {
        return false;
    }
    if (!shape_is(in, at, kSigMacroPick, &a) || !shape_calls_named(in, a, 3, "mcrbedit", in.close)
        || !asked_names_key(in, at + 38, "mcr1edit") || rdptr(at, 43) != in.mcb
        || !shape_is(in, at + kMacroPickOpensAt, kShapeMacroPickOpens, &b) || shape_target(in, b, 0) != in.close
        || !shape_calls_named(in, b, 1, "mcr2edit", in.close) || !shape_opens(in, b, 4, "mcrledit")
        || !shape_opens(in, b, 7, "macro")) {
        return route_fail(why, len, "macro_pick", "it does not close the three palette windows and open mcrledit and"
            " macro by name");
    }
    r.macro_palette = shape_byte(a, 0);
    memcpy(&r.macro_pick, &at, sizeof(at));
    const uint8_t ctrl_row = shape_byte(a, 1);
    const uint8_t alt_row = shape_byte(a, 2);
    at = asked_find(in, "palette_switch", kSigPaletteSwitch, why, len);
    if (!at) {
        return false;
    }
    if (!shape_is(in, at, kSigPaletteSwitch, &a) || shape_byte(a, 0) != r.macro_palette
        || shape_byte(a, 3) != r.macro_palette || shape_byte(a, 8) != r.macro_palette
        || shape_byte(a, 1) != alt_row || shape_byte(a, 2) != ctrl_row || !shape_opens(in, a, 4, "mcr2edit")
        || !shape_target(in, a, 7) || !shape_is(in, at + kPaletteSwitchBackAt, kShapePaletteSwitchBack, &b)
        || !shape_opens(in, b, 0, "mcr1edit") || shape_target(in, b, 3) != shape_target(in, a, 7)) {
        return route_fail(why, len, "palette_switch", "it does not toggle the macro editor's palette and open the"
            " other palette by name");
    }
    memcpy(&r.macro_switch, &at, sizeof(at));

    uint8_t bytes[64];
    char mask[65];
    ScanResult hits;
    hits.count = 0;
    if (parse_signature(kSigPaletteEscape, bytes, mask, sizeof(bytes))) {
        scan(in.text, in.size, bytes, mask, false, &hits);
    }
    fn = NULL;
    const uint8_t* title = NULL;
    for (int i = 0; i < hits.count && i < 2 && hits.count == 2; ++i) {
        const uint8_t* close = asked_target(in, hits.hits[i] + 3, 0xE8);
        const uint8_t* step = asked_target(in, hits.hits[i] + 35, 0xE9);
        const bool same = i == 0 || (close == fn && step == title);
        fn = same && rdptr(hits.hits[i], 10) == asked_slot(in, "titlewin") ? close : NULL;
        title = fn ? step : NULL;
    }
    if (!fn || !title || !shape_is(in, fn, kShapeMacroClose, &a) || !shape_calls_named(in, a, 0, "mcrbedit", in.close)
        || !shape_calls_named(in, a, 3, "mcr1edit", in.close) || !shape_calls_named(in, a, 6, "mcr2edit", in.close)) {
        snprintf(why, len, "palette_escape: mcr1edit's and mcr2edit's Escape is not the macro editor's close of"
            " mcrbedit, mcr1edit and mcr2edit by name, then titlewin's step back (client patched?)");
        return false;
    }
    memcpy(&r.macro_close, &fn, sizeof(fn));
    memcpy(&r.title, &title, sizeof(title));

    at = asked_find(in, "status_escape", kSigStatusEscape, why, len);
    if (!at) {
        return false;
    }
    const uint8_t* status_input = at - kStatusEscapeAt;
    const uint8_t* status_close = asked_find_keyed(in, "status_close", kSigStatusClose, kStatusCloseKey, "persona",
        why, len);
    if (!status_close) {
        return false;
    }
    const uint8_t* vtables[4];
    const int named = asked_vtables(in, status_input, 6, vtables, 4);
    bool paired = false;
    for (int i = 0; i < named && i < 4; ++i) {
        paired = paired || asked_slot_routine(in, vtables[i], 2) == status_close;
    }
    if (!shape_is(in, at, kSigStatusEscape, &a) || shape_ptr(a, 0) != in.mcb || shape_target(in, a, 1) != close_instance
        || !paired || !shape_is(in, status_close, kSigStatusClose, &b)
        || !shape_calls_named(in, b, 0, "persona", in.close) || !shape_calls_named(in, b, 3, "mp_stat", in.close)) {
        return route_fail(why, len, "status_escape", "the Status screen's Escape is not statcom2's close by instance,"
            " its begin-close closing persona and mp_stat by name");
    }
    r.status_close = status_close;
    at = asked_find_keyed(in, "text_color_escape", kSigTextColorEscape, kTextColorEscapeKey, "textcol2", why, len);
    if (!at) {
        return false;
    }
    if (!shape_is(in, at, kSigTextColorEscape, &a) || !shape_calls_named(in, a, 0, "textcol2", in.close)
        || !shape_calls_named(in, a, 3, "textcol1", in.close)) {
        return route_fail(why, len, "text_color_escape", "textcol1's Escape does not close textcol2, then textcol1,"
            " by name");
    }
    r.textcol_escape = at;
    at = asked_find_keyed(in, "instincts_escape", kSigInstinctsEscape, kInstinctsEscapeKey, "mp_mncst", why, len);
    if (!at) {
        return false;
    }
    const bool stat_close = shape_is(in, at, kSigInstinctsEscape, &a)
        && shape_is(in, shape_target(in, a, 1), kShapeCloseByNameRet, &b) && shape_calls_named(in, b, 0, "mp_stat", in.close);
    if (!stat_close || shape_ptr(a, 0) != asked_slot(in, "mp_stat") || shape_ptr(a, 2) != asked_slot(in, "mp_cstm")
        || !shape_is(in, shape_target(in, a, 3), kShapeCloseByNameRet, &c)
        || !shape_calls_named(in, c, 0, "mp_cstm", in.close) || !shape_calls_named(in, a, 4, "mp_mncst", in.close)) {
        return route_fail(why, len, "instincts_escape", "mp_mncst's Escape does not close mp_stat, mp_cstm and mp_mncst"
            " by name");
    }
    memcpy(&r.instincts_escape, &at, sizeof(at));
    at = asked_find(in, "region_escape", kSigRegionEscape, why, len);
    if (!at) {
        return false;
    }
    const uint8_t* region_vtables[2];
    bool region_ok = asked_vtables(in, at, 6, region_vtables, 2) > 0
        && memcmp(at + kRegionEscapeAt + kRegionEscapeStep * kRegionEscapeCloses, "\xc2\x08\x00", 3) == 0;
    for (int i = 0; region_ok && i < kRegionEscapeCloses; ++i) {
        region_ok = shape_is(in, at + kRegionEscapeAt + kRegionEscapeStep * static_cast<size_t>(i), kShapeCloseByName, &c)
            && shape_calls_named(in, c, 0, kRegionEscapeNames[i], in.close);
    }
    if (!region_ok) {
        return route_fail(why, len, "region_escape", "cnqframe's Escape does not start its fade, then close its points"
            " windows and the Colonization panes by name");
    }
    r.region_escape = at;
    at = asked_find(in, "free_slot", kSigFreeSlot, why, len);
    if (!at) {
        return false;
    }
    fn = asked_target(in, at + 21, 0xE8);
    if (!fn || asked_target(in, at + 46, 0xE8) != fn) {
        return route_fail(why, len, "free_slot", "it does not read the bag's size before each slot");
    }
    memcpy(&r.bag_size, &fn, sizeof(fn));

    at = asked_find(in, "items_row", kSigItemsRow, why, len);
    if (!at) {
        return false;
    }
    if (!shape_is(in, at, kSigItemsRow, &a) || !asked_data(in, shape_ptr(a, 0), 4) || !shape_target(in, a, 1)
        || shape_ptr(a, 2) != asked_slot(in, "inventor") || shape_ptr(a, 5) != asked_slot(in, "inventor")
        || !asked_data(in, shape_ptr(a, 4), 4) || !asked_target(in, at + 41, 0xE8)) {
        return route_fail(why, len, "items_row", "it does not open inventor with the list of the game's the Items row"
            " hands it, on the controller in inventor's slot");
    }
    r.items_object = const_cast<uint8_t**>(reinterpret_cast<uint8_t* const*>(shape_ptr(a, 0)));
    r.inventor_items = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(shape_ptr(a, 3)));
    r.inventor_policy = const_cast<uint8_t**>(reinterpret_cast<uint8_t* const*>(shape_ptr(a, 4)));
    fn = shape_target(in, a, 1);
    memcpy(&r.items_view, &fn, sizeof(fn));
    fn = asked_target(in, at + 41, 0xE8);
    memcpy(&r.inventor_open, &fn, sizeof(fn));

    const uint8_t* data = NULL;
    memcpy(&data, &offer.item_data, sizeof(data));
    static const uint8_t kSummaryCall[] = {0x6a, 0x00, 0x51, 0xe8};
    const uint8_t* call = NULL;
    for (size_t i = 0; data && !call && i + sizeof(kSummaryCall) + 4 <= kSummaryBuildSpan; ++i) {
        if (asked_bytes(in, data + i, kSummaryCall, sizeof(kSummaryCall))) {
            call = data + i + 3;
        }
    }
    const uint8_t* builder = call ? asked_target(in, call, 0xE8) : NULL;
    int copies = 0;
    for (size_t i = 0; builder && i < kSummaryTypeSpan; ++i) {
        if (shape_is(in, builder + i, kShapeSummaryType, &b)
            && static_cast<uint32_t>(reinterpret_cast<uintptr_t>(shape_ptr(b, 0))) == item_type) {
            a = b;
            ++copies;
        }
    }
    if (copies != 1) {
        snprintf(why, len, "item_data: its summary's builder does not copy the item's type word from the pointer the"
            " item use reads (client patched?)");
        return false;
    }
    r.summary_type = a.at[a.hole[2] + 1];
    *out = r;
    return true;
}

// The settings (core.h's kSettings): the game's config store, the routines
// the Config pages set through and the tables naming their rows.

// The store's set by id (cdecl; id, value) calls the store's set method
// (this = the store; id, value), which changes nothing for the value already
// stored, but for id 0xCE. A value outside kEntryMin..kEntryMax (-1: no
// range) takes kEntryDefault instead; it goes to kEntryValue, then the id's
// subscribers run.
const char kSigCfgSet[] =
    "8b 0d ?? ?? ?? ?? 85 c9 74 10 8b 44 24 08 8b 54 24 04 50 52 e8 ?? ?? ?? ?? c3 83 c8 ff c3";
const size_t kCfgSetStore = 2;
const size_t kCfgSetMethod = 20;
const char kShapeCfgMethod[] =
    "53 8b 5c 24 08 56 57 53 8b f9 e8 ?? ?? ?? ?? 8b f0 85 f6 75 09 5f 5e 83 c8 ff 5b c2 08 00 8b 44 24 14 81 fb ce"
    " 00 00 00 74 0e 39 46 08 75 09 5f 5e 83 c8 ff 5b c2 08 00 50 56 8b cf e8 ?? ?? ?? ??";
const char kShapeCfgLookup[] = "8b 49 04 85 c9 74 10 8b 44 24 04 8d 14 80 8d 04 50 8d 04 81 c2 04 00";
const char kShapeCfgStore[] =
    "8b 44 24 04 8b 4c 24 08 39 48 08 75 0c f6 40 28 01 75 06 83 c8 ff c2 08 00 8b 50 1c 83 fa ff 74 10 56 8b 70 18"
    " 3b f1 5e 7f 04 3b ca 7e 03 8b 48 20 89 48 08 8b 48 24 85 c9 74 0d";
// The store's read by id (cdecl; id), as conf12wi's equipment lock rows call
// it.
const char kShapeCfgGet[] = "8b 0d ?? ?? ?? ?? 85 c9 74 0b 8b 44 24 04 50 e8 ?? ?? ?? ?? c3";

// The store's save, never run: nothing unless its loaded byte is 1.
const char kSigCfgSave[] =
    "56 8b f1 80 7e 09 01 75 0c e8 ?? ?? ?? ?? 8b ce e8 ?? ?? ?? ?? b8 01 00 00 00 5e c3";

// The chat filter page's row state, never run: the row table, 4 bytes a row
// (the mask bit, the second state's bit, the text's id). A negative second
// state is a system level, read from the page's copy of 0x35 at +0x3C; second
// state 2 with no mask bit is Tell's, with the page's copy of 0x47 at +0x40.
const char kSigFilterState[] =
    "56 57 8b 7c 24 0c 8b f1 8a 04 bd ?? ?? ?? ?? 3c 33 75 55 8a 04 bd ?? ?? ?? ?? 84 c0 7d 1a 8b 4e 3c 0f be c0 f7 d8 3b c8 0f 8c ?? ?? ?? ?? 5f b8 01 00 00 00 5e c2 04 00 3c 02 75 23 8b 4e 40";
const size_t kFilterTable = 11;
const size_t kFilterTableAgain = 22;
const size_t kFilterBitCall = 0x7E;
const size_t kFilterStateBytes = 0xD6;
const char kShapeFilterBit[] =
    "8b 4c 24 04 83 f9 1e 7d 04 33 c0 eb 08 b8 01 00 00 00 83 c1 e2 ba 01 00 00 00 d3 e2 8b 4c 24 08 85 14 81 0f 95 c0 c3";
const uint8_t kFilterOneStateTest[] = {0x80, 0xf9, 0x0b};

// The battle effects filter page's toggle, never run: its copy of 0x92 at
// +0x30, the row table 4 bytes a row (the bit, 0, the text's id).
const char kSigEffectRows[] =
    "56 57 8b 7c 24 0c 8b f1 33 c9 8b 46 30 8a 0c bd ?? ?? ?? ?? 50 51 e8 ?? ?? ?? ?? 83 c4 08 84 c0 74 20";
const size_t kEffectTable = 16;
const size_t kEffectBitCall = 22;
const char kShapeBitTest[] = "8b 4c 24 04 b8 01 00 00 00 d3 e0 85 44 24 08 0f 95 c0 c3";

// conf11s's load of a group, never run: its copy of 0xB9..0xBC written back
// and read again, then the group's table, 8 bytes a row (the text's id, the
// bit), ended by an id of -1. textcol1's open, never run: the category's
// table, 16 bytes a row (the label's id, the sample's, the color, the store
// word), ended by a label of -1. Both tables come from a kShapeFourTables
// getter: a jump table (hole 0) to a table for each of 0 to 3 (holes 1 to 4).
const char kSigRoutingGroups[] =
    "8b 44 24 04 53 56 8b f1 8b 4c 24 10 57 89 4e 38 8b ce 89 46 34 e8 ?? ?? ?? ?? 8b ce e8 ?? ?? ?? ?? 8b 56 38 52 e8 ?? ?? ?? ?? 83 c4 04 89 46 3c";
const size_t kRoutingGetter = 37;
const char kSigTextColorTables[] =
    "53 56 8b f1 57 8b 46 14 50 e8 ?? ?? ?? ?? 8b 7e 08 83 c4 04 33 db 8b cf 53 6a 01 89 46 18 89 5e 1c e8";
const size_t kColorGetter = 9;
const char kShapeFourTables[] =
    "8b 44 24 04 83 f8 03 77 1f ff 24 85 ?? ?? ?? ?? b8 ?? ?? ?? ?? c3 b8 ?? ?? ?? ?? c3 b8 ?? ?? ?? ?? c3 b8 ?? ?? ?? ??"
    " c3 33 c0 c3";
const size_t kFourTablesCase = 16;
const size_t kFourTablesStep = 6;

// k1assign's and k2assign's opens, never run: their rows (30 and 29) and
// the table of each row's text id at kKeyRows1 and kKeyRows2, a dword a row.
const char kSigKeyRows1[] =
    "56 8b f1 57 c7 46 24 1e 00 00 00 e8 ?? ?? ?? ?? 84 c0 75 07 c7 46 24 1c 00 00 00 8b 7e 08 6a 00 6a 01 8b cf c7 46 6c ?? ?? ?? ??";
const size_t kKeyRows1 = 39;
const char kSigKeyRows2[] =
    "56 8b f1 57 6a 00 8b 7e 08 6a 01 8b cf c7 46 24 1d 00 00 00 c7 46 6c ?? ?? ?? ?? c7 46 14 00 00 00 00";
const size_t kKeyRows2 = 23;

// conf12wi's equipment lock rows, never run: 0xBF read through the store's
// read and set through its set, then the request routine called with 4, 1 or
// 0, which sets 0xBF itself as it succeeds.
const char kSigLockRows[] =
    "33 d2 66 3d 04 00 56 68 bf 00 00 00 0f 94 c2 8b f2 e8 ?? ?? ?? ?? 56 68 bf 00 00 00 8b f8 e8 ?? ?? ?? ?? 83 c4 0c 83 fe 01 5e 6a 00 6a 00 75 21 83 ff 01 74 0e 6a 04 e8";
const size_t kLockGet = 17;
const size_t kLockSet = 30;
const size_t kLockCalls[3] = {55, 69, 83};
const size_t kLockRouteBytes = 0x96;
const uint8_t kLockSetsBytes[] = {0x68, 0xbf, 0x00, 0x00, 0x00, 0xe8};
// kShapeLockHead holes: 0 the gate (al 1: nothing sent), 3 the wait (its
// refusal message 0x22, nothing sent), 7 the clock, 8 the stamp.
// kShapeLockWait holes: 0 the clock, 1 the stamp, 2 the interval in ms.
const char kShapeLockHead[] =
    "53 e8 ?? ?? ?? ?? 84 c0 74 04 32 c0 5b c3 8b 5c 24 08 80 fb 02 74 ?? 8a 44 24 0c 84 c0 75 ?? e8 ?? ?? ?? ?? 84 c0"
    " 75 ?? 8b 0d ?? ?? ?? ?? 6a 22 e8 ?? ?? ?? ?? b0 01 5b c3 e8 ?? ?? ?? ?? a3 ?? ?? ?? ??";
const char kShapeLockWait[] = "e8 ?? ?? ?? ?? 2b 05 ?? ?? ?? ?? 3d ?? ?? ?? ?? 0f 97 c0 c3";

// conf4's idle timer rows, never run: each row's minutes in seconds go to the
// timer's routine (cdecl; seconds), which stores seconds / 60 in 0x41
// through the store's set, or -1 for 0.
const char kSigIdleRows[] =
    "8d 44 40 fa 8d 04 80 8d 04 80 c1 e0 03 50 e8 ?? ?? ?? ?? 83 c4 04 66 89 77 22 5f 5e c2 08 00 6a 00 e8 ?? ?? ?? ?? 83 c4 04 66 89 77 22";
const size_t kIdleCalls[2] = {14, 33};
const char kShapeIdle[] =
    "56 8b 74 24 08 85 f6 74 3e b8 89 88 88 88 f7 ee 03 d6 c1 fa 05 8b c2 c1 e8 1f 03 d0 52 6a 41 e8 ?? ?? ?? ??";
const size_t kIdleOffAt = 0x47;
const char kShapeIdleOff[] = "6a ff 6a 41 e8 ?? ?? ?? ??";

// conf3win's open reads player flag 0 into its field +0x38, and its
// begin-close hands that field to the flag's request once a row has changed
// it. kShapeFlagGet holes: 0 the player's data's global, 3 the flags' jump
// table; flag 0 is bit 2 of the byte at kShapeFlagBit's hole.
const char kSigAnonymousRows[] =
    "6a 00 89 46 30 e8 ?? ?? ?? ?? 83 c4 0c 25 ff 00 00 00 89 46 38 c6 46 3c 00 5e c3 90 90 90 80 79 3c 01 75 0e 8a 41 38 50 6a 00 e8 ?? ?? ?? ?? 83 c4 08 c3";
const size_t kAnonymousGet = 5;
const size_t kAnonymousSet = 42;
const char kShapeFlagGet[] =
    "a1 ?? ?? ?? ?? 85 c0 0f 84 ?? ?? ?? ?? 8b 4c 24 04 83 f9 0b 0f 87 ?? ?? ?? ?? ff 24 8d ?? ?? ?? ??";
const char kShapeFlagBit[] = "8a 80 ?? ?? ?? ?? c0 e8 02 24 01 c3";
const uint8_t kAnonymousBit = 1 << 2;
const char kShapeFlagSet[] = "a1 ?? ?? ?? ?? 85 c0 0f 84 ?? ?? ?? ?? 6a 01 6a 01 68 dc 00 00 00 e8";

// The four tables kShapeFourTables's getter at `fn` answers, in order.
inline bool four_tables(const AskedInputs& in, const uint8_t* fn, const uint8_t** out) {
    Shape a;
    if (!fn || !shape_is(in, fn, kShapeFourTables, &a) || !asked_code(in, shape_ptr(a, 0), 16)) {
        return false;
    }
    for (int k = 0; k < 4; ++k) {
        out[k] = shape_ptr(a, k + 1);
        if (rdptr(shape_ptr(a, 0), 4 * k) != fn + kFourTablesCase + kFourTablesStep * k || !asked_data(in, out[k], 4)) {
            return false;
        }
    }
    return true;
}

// The rows a table holds before its end, at most `max`: each `stride`
// bytes, ended by a first dword of `end`.
inline int table_rows(const AskedInputs& in, const uint8_t* table, size_t stride, uint32_t end, int max) {
    int n = 0;
    while (n < max && asked_data(in, table + n * stride, stride) && rd32(table + n * stride, 0) != end) {
        ++n;
    }
    return n;
}

// Why setting i's row in the game's tables is not the one kSettings names
// for it, or NULL with its slot filled from that row.
inline const char* setting_row(const AskedInputs& in, const SettingSites& s, int i, SettingSlot* slot, char* buf,
                               size_t size) {
    const SettingSpec& spec = kSettings[i];
    slot->id = spec.id;
    slot->bit = spec.bit;
    slot->row = filter_row(kFilterNoBit, kFilterOneState);
    const int r = spec.row;
    uint32_t text = 0xFFFFFFFFu;
    switch (spec.from) {
    case kFromColors: {
        const uint8_t* t = s.colors[spec.list];
        if (table_rows(in, t, 16, 0xFFFFFFFFu, r + 1) <= r) {
            snprintf(buf, size, "the text colors' category %d has no row %d", spec.list + 1, r + 1);
            return buf;
        }
        text = rd32(t + 16 * r, 0);
        slot->id = static_cast<uint8_t>(rd32(t + 16 * r, 12));
        if (rd32(t + 16 * r, 12) != spec.id) {
            snprintf(buf, size, "the text colors' category %d row %d is store word 0x%X, not 0x%02X", spec.list + 1,
                r + 1, static_cast<unsigned>(rd32(t + 16 * r, 12)), spec.id);
            return buf;
        }
        break;
    }
    case kFromFilter: {
        const uint8_t* t = s.filter_rows;
        const uint8_t bit = t[4 * r];
        const int8_t sub = static_cast<int8_t>(t[4 * r + 1]);
        text = static_cast<uint32_t>(rd16(t + 4 * r, 2)) & 0xFFFF;
        if (spec.store == kStoreWord) {
            const bool levels = bit == kFilterNoBit && sub == -1 && t[4 * r + 4] == kFilterNoBit
                && static_cast<int8_t>(t[4 * r + 5]) == -2 && t[4 * r + 8] == kFilterNoBit
                && static_cast<int8_t>(t[4 * r + 9]) == -3;
            if (!levels) {
                snprintf(buf, size, "the chat filter's rows %d to %d are not its three system levels", r + 1, r + 3);
                return buf;
            }
            return NULL;
        }
        if ((bit != kFilterNoBit && bit >= 2 * kFilterBitsPerWord) || (sub != kFilterOneState && sub >= 32)) {
            snprintf(buf, size, "the chat filter's row %d is not a bit of its words (mask bit %d, second state %d)",
                r + 1, bit, sub);
            return buf;
        }
        slot->row = filter_row(bit, sub);
        const bool one = slot->row.mask >= 0 && slot->row.second < 0 && !slot->row.flagged;
        if (sub < 0 || one != (spec.value == kValueBool)) {
            snprintf(buf, size, "the chat filter's row %d is no longer %s", r + 1,
                spec.value == kValueBool ? "an on/off row" : "a row of more states");
            return buf;
        }
        break;
    }
    case kFromEffects: {
        const uint8_t* t = s.effect_rows;
        if (!asked_data(in, t + 4 * r, 4) || t[4 * r] >= 32) {
            snprintf(buf, size, "the battle effects filter's row %d is not a bit of 0x92", r + 1);
            return buf;
        }
        slot->bit = static_cast<int8_t>(t[4 * r]);
        text = static_cast<uint32_t>(rd16(t + 4 * r, 2)) & 0xFFFF;
        break;
    }
    case kFromRouting: {
        const uint8_t* t = s.routing[spec.list];
        if (table_rows(in, t, 8, 0xFFFFFFFFu, r + 1) <= r || rd32(t + 8 * r, 4) >= 64) {
            snprintf(buf, size, "the chat log routing's group %d has no row %d of a bit", spec.list + 1, r + 1);
            return buf;
        }
        text = rd32(t + 8 * r, 0);
        slot->bit = static_cast<int8_t>(rd32(t + 8 * r, 4));
        break;
    }
    case kFromKeys:
        text = rd32(s.keys[spec.list] + 4 * r, 0);
        break;
    default:
        return NULL;
    }
    if (text != static_cast<uint32_t>(spec.label)) {
        snprintf(buf, size, "%s's row %d is not %s's (its text 0x%X, not 0x%02X)", spec.page, r + 1, spec.name,
            static_cast<unsigned>(text), static_cast<unsigned>(spec.label));
        return buf;
    }
    return NULL;
}

// game.h's SettingSites; `player` the player data's global the engine
// resolved from bankmenu's open, which conf3win's flag reads through too.
inline bool resolve_settings(const AskedInputs& in, uint8_t** player, SettingSites* out, char* why, size_t len) {
    SettingSites s;
    memset(&s, 0, sizeof(s));
    Shape a;
    Shape b;

    const uint8_t* at = asked_find(in, "cfg_set", kSigCfgSet, why, len);
    if (!at) {
        return false;
    }
    const uint8_t* method = asked_target(in, at + kCfgSetMethod, 0xE8);
    if (!asked_data(in, rdptr(at, kCfgSetStore), 4) || !method || !shape_is(in, method, kShapeCfgMethod, &a)
        || !shape_is(in, shape_target(in, a, 0), kShapeCfgLookup, &b)
        || !shape_is(in, shape_target(in, a, 1), kShapeCfgStore, &b)) {
        return route_fail(why, len, "cfg_set", "it does not set the store's entry by id, its range checked, as the"
            " engine reads the entries");
    }
    s.store = const_cast<uint8_t**>(reinterpret_cast<uint8_t* const*>(rdptr(at, kCfgSetStore)));
    memcpy(&s.set, &at, sizeof(at));
    s.method = method;
    s.save = asked_find(in, "cfg_save", kSigCfgSave, why, len);
    if (!s.save) {
        return false;
    }

    at = asked_find(in, "filter_state", kSigFilterState, why, len);
    if (!at) {
        return false;
    }
    const uint8_t* found = NULL;
    s.filter_rows = rdptr(at, kFilterTable);
    if (rdptr(at, kFilterTableAgain) != s.filter_rows + 1 || !asked_data(in, s.filter_rows, 4 * (kFilterRows + 1))
        || rd32(s.filter_rows + 4 * kFilterRows, 0) != 0
        || !shape_is(in, asked_target(in, at + kFilterBitCall, 0xE8), kShapeFilterBit, &a)
        || !asked_once(in, at, kFilterStateBytes, kFilterOneStateTest, sizeof(kFilterOneStateTest), &found)) {
        return route_fail(why, len, "filter_state", "the chat filter page's rows are not 55 of a mask bit, a second"
            " state and a text");
    }
    at = asked_find(in, "effect_rows", kSigEffectRows, why, len);
    if (!at) {
        return false;
    }
    s.effect_rows = rdptr(at, kEffectTable);
    if (!asked_data(in, s.effect_rows, 4) || !shape_is(in, asked_target(in, at + kEffectBitCall, 0xE8), kShapeBitTest, &a)) {
        return route_fail(why, len, "effect_rows", "the battle effects filter page's rows are not a bit and a text");
    }
    at = asked_find(in, "routing_groups", kSigRoutingGroups, why, len);
    if (!at) {
        return false;
    }
    if (!four_tables(in, asked_target(in, at + kRoutingGetter, 0xE8), s.routing)) {
        return route_fail(why, len, "routing_groups", "conf11s does not take its four groups' rows from one getter");
    }
    at = asked_find(in, "text_color_tables", kSigTextColorTables, why, len);
    if (!at) {
        return false;
    }
    if (!four_tables(in, asked_target(in, at + kColorGetter, 0xE8), s.colors)) {
        return route_fail(why, len, "text_color_tables", "textcol1 does not take its four categories' rows from one"
            " getter");
    }
    at = asked_find(in, "key_rows1", kSigKeyRows1, why, len);
    if (!at) {
        return false;
    }
    s.keys[0] = rdptr(at, kKeyRows1);
    at = asked_find(in, "key_rows2", kSigKeyRows2, why, len);
    if (!at) {
        return false;
    }
    s.keys[1] = rdptr(at, kKeyRows2);
    if (!asked_data(in, s.keys[0], 4 * 30) || !asked_data(in, s.keys[1], 4 * 29)) {
        return route_fail(why, len, "key_rows", "k1assign's and k2assign's rows are not readable data");
    }

    at = asked_find(in, "lock_rows", kSigLockRows, why, len);
    if (!at) {
        return false;
    }
    const uint8_t* lock = asked_target(in, at + kLockCalls[0], 0xE8);
    const uint8_t* set;
    memcpy(&set, &s.set, sizeof(set));
    int sets = 0;
    for (size_t k = 0; lock && k + sizeof(kLockSetsBytes) + 4 <= kLockRouteBytes; ++k) {
        if (asked_bytes(in, lock + k, kLockSetsBytes, sizeof(kLockSetsBytes))
            && asked_target(in, lock + k + sizeof(kLockSetsBytes) - 1, 0xE8) == set) {
            ++sets;
        }
    }
    if (!lock || asked_target(in, at + kLockCalls[1], 0xE8) != lock || asked_target(in, at + kLockCalls[2], 0xE8) != lock
        || asked_target(in, at + kLockSet, 0xE8) != set || !shape_is(in, asked_target(in, at + kLockGet, 0xE8), kShapeCfgGet, &a)
        || shape_ptr(a, 0) != reinterpret_cast<const uint8_t*>(s.store) || sets != 2) {
        return route_fail(why, len, "lock_rows", "conf12wi's equipment lock rows do not set 0xBF, then run a request"
            " that sets it through the store's set");
    }
    memcpy(&s.lock, &lock, sizeof(lock));
    const uint8_t* clock = shape_is(in, lock, kShapeLockHead, &a)
        && shape_is(in, shape_target(in, a, 3), kShapeLockWait, &b) ? shape_target(in, b, 0) : NULL;
    if (!clock || shape_target(in, a, 7) != clock || shape_ptr(a, 8) != shape_ptr(b, 1)
        || !asked_data(in, shape_ptr(b, 1), 4) || !shape_ptr(b, 2)) {
        return route_fail(why, len, "lock_wait", "the equipment lock's request does not wait an interval on the game's"
            " clock from the stamp it writes as it goes on to send");
    }
    memcpy(&s.lock_clock, &clock, sizeof(clock));
    s.lock_stamp = shape_ptr(b, 1);
    s.lock_wait = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(shape_ptr(b, 2)));

    at = asked_find(in, "idle_rows", kSigIdleRows, why, len);
    if (!at) {
        return false;
    }
    const uint8_t* idle = asked_target(in, at + kIdleCalls[0], 0xE8);
    if (!idle || asked_target(in, at + kIdleCalls[1], 0xE8) != idle || !shape_is(in, idle, kShapeIdle, &a)
        || shape_target(in, a, 0) != set || !shape_is(in, idle + kIdleOffAt, kShapeIdleOff, &b)
        || shape_target(in, b, 0) != set) {
        return route_fail(why, len, "idle_rows", "conf4's rows do not run the idle timer, which stores its minutes in"
            " 0x41 through the store's set");
    }
    memcpy(&s.idle, &idle, sizeof(idle));

    at = asked_find(in, "anonymous_rows", kSigAnonymousRows, why, len);
    if (!at) {
        return false;
    }
    const uint8_t* get = asked_target(in, at + kAnonymousGet, 0xE8);
    const uint8_t* flag = asked_target(in, at + kAnonymousSet, 0xE8);
    const uint8_t* bit = get && shape_is(in, get, kShapeFlagGet, &a) && asked_code(in, shape_ptr(a, 3), 4)
        ? rdptr(shape_ptr(a, 3), 0) : NULL;
    if (!bit || shape_ptr(a, 0) != reinterpret_cast<const uint8_t*>(player) || !shape_is(in, bit, kShapeFlagBit, &b)
        || !flag || !shape_is(in, flag, kShapeFlagSet, &a) || shape_ptr(a, 0) != reinterpret_cast<const uint8_t*>(player)) {
        return route_fail(why, len, "anonymous_rows", "conf3win does not read the player's flag 0 from the player's data"
            " and hand it to the flag's request");
    }
    s.player = player;
    s.flags = static_cast<uint32_t>(reinterpret_cast<uintptr_t>(shape_ptr(b, 0)));
    s.anonymous = kAnonymousBit;
    memcpy(&s.flag, &flag, sizeof(flag));

    for (int i = 0; i < kSettingCount; ++i) {
        if (in.inv->find_exact(kSettings[i].page) < 0) {
            snprintf(why, len, "settings: %s's page %s is no window of the registry's", kSettings[i].name,
                kSettings[i].page);
            return false;
        }
        char buf[192];
        const char* refusal = setting_row(in, s, i, &s.slot[i], buf, sizeof(buf));
        if (refusal) {
            return route_fail(why, len, "settings", refusal);
        }
    }
    *out = s;
    return true;
}

}  // namespace hu

#endif  // HIDEUI_SIGNATURES_H_
