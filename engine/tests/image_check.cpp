// image_check - the engine's resolution against an unpacked FFXiMain.dll on
// disk: every signature must match exactly once, land at the address given
// for it, when one is given, and the menu table must
// verify. The image is copied section by section to its preferred base and
// never run: no loader, no DllMain, no TLS callbacks.
//
// image_check.exe <unpacked FFXiMain.dll> [registry mcb masks set_position close open update staged show
//                  mouse_mode menu_input compass_draw macro_gate ability_open key_test
//                  template_swap menu_draw sprite_draw primitive_draw quad_draw
//                  the eleven called routines in signatures.h's kCalls order
//                  link5_cache_site link5_cache query_cancel_site query_cancel_allowed
//                  set_cursor link5_latch_site link5_latch link5_open_site link5_global
//                  link5_callback glyph_convert row_hit_test mouse_read menu_routing
//                  compass_global macro_object_global yesno_class yesno_vtable yesno_confirm
//                  dead_confirm dead_decider message yesno_ask dead_callback dead_tick
//                  yesno_cancel_test moneyctr_confirm itemctrl_confirm moneyctr_setup itemctrl_setup
//                  the sixteen askers' callbacks in signatures.h's kAskerSites order
//                  close_instance sortyn_setup sortyn_confirm evitem_ask evitem_setup evitem_callback
//                  evitem_input confyn_input confyn_restore bank_open bank_clear bank_data bank_safe2
//                  bank_input maplist_close maplist_rebuild maplist_select maplist_cursor map0_input
//                  map_close gmtell_poll
//                  gmtell_slot gmtell_queue mp_dead_input ok_input
//                  door_input door_vtable door_open_hook door_draw door_handler door_gate door_floor
//                  door_condition door_tick door_open door_opened door_house door_exiting(3) door_table
//                  dialogue_input dialogue_handler dialogue_dispatch dialogue_advance dialogue_page_sound
//                  dialogue_release dialogue_clear dialogue_wait dialogue_tick dialogue_click_return
//                  dialogue_texts(2)
//                  the text-entry windows' sites in this file's order of them
//                  target_arrow_return pick_arrow_return
//                  the trade windows' sites and offsets in this file's order of them
//                  equip_open monstrosity_check equip_shortcut magic_open map_open set_spells_open
//                  attachments_open linkshell_row linkshell_check linkshell_open instincts_open
//                  item_use item_cache item_data item_type use_check synthesis_open inventor_list
//                  tabula_open
//                  macro_edit macro_editor macro_editor_open]
// The optional hex addresses are the ones expected for that build.

#define WIN32_LEAN_AND_MEAN
#include "../signatures.h"

#include <cstdio>
#include <cstdlib>

using namespace hu;

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

const uint8_t* image;
size_t image_size;

uint8_t* map_image(const char* path, const uint8_t** text, size_t* text_size) {
    std::FILE* f = std::fopen(path, "rb");
    if (!f) {
        return NULL;
    }
    std::fseek(f, 0, SEEK_END);
    const long length = std::ftell(f);
    std::fseek(f, 0, SEEK_SET);
    uint8_t* file = static_cast<uint8_t*>(HeapAlloc(GetProcessHeap(), 0, length));
    const bool read = file && std::fread(file, 1, length, f) == static_cast<size_t>(length);
    std::fclose(f);
    if (!read) {
        return NULL;
    }
    const IMAGE_DOS_HEADER* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(file);
    const IMAGE_NT_HEADERS32* nt = reinterpret_cast<const IMAGE_NT_HEADERS32*>(file + dos->e_lfanew);
    uint8_t* base = static_cast<uint8_t*>(VirtualAlloc(reinterpret_cast<void*>(nt->OptionalHeader.ImageBase),
        nt->OptionalHeader.SizeOfImage, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE));
    if (base != reinterpret_cast<uint8_t*>(nt->OptionalHeader.ImageBase)) {
        std::printf("FAIL  the preferred base 0x%08X is not free\n",
            static_cast<unsigned>(nt->OptionalHeader.ImageBase));
        return NULL;
    }
    image = base;
    image_size = nt->OptionalHeader.SizeOfImage;
    memcpy(base, file, nt->OptionalHeader.SizeOfHeaders);
    const IMAGE_SECTION_HEADER* s = IMAGE_FIRST_SECTION(nt);
    for (unsigned i = 0; i < nt->FileHeader.NumberOfSections; ++i, ++s) {
        const DWORD bytes = s->SizeOfRawData < s->Misc.VirtualSize ? s->SizeOfRawData : s->Misc.VirtualSize;
        memcpy(base + s->VirtualAddress, file + s->PointerToRawData, bytes);
        if (memcmp(s->Name, ".text", 5) == 0) {
            *text = base + s->VirtualAddress;
            *text_size = s->Misc.VirtualSize;
        }
    }
    return base;
}

const uint8_t* find(const uint8_t* text, size_t size, const char* name, const char* signature) {
    uint8_t bytes[64];
    char mask[65];
    parse_signature(signature, bytes, mask, sizeof(bytes));
    ScanResult r;
    scan(text, size, bytes, mask, false, &r);
    char label[128];
    snprintf(label, sizeof(label), "%s matches exactly once (%d)", name, r.count);
    check(r.count == 1, label);
    return r.count == 1 ? r.hits[0] : NULL;
}

bool in_text(const uint8_t* p, const uint8_t* text, size_t size) {
    return p >= text && p < text + size;
}

void expect(const char* name, uintptr_t got, int argc, char** argv, int index) {
    if (index >= argc) {
        return;
    }
    const uintptr_t want = std::strtoul(argv[index], NULL, 16);
    char label[128];
    snprintf(label, sizeof(label), "%s at 0x%08X, expected 0x%08X", name,
        static_cast<unsigned>(got), static_cast<unsigned>(want));
    check(got == want, label);
}

}  // namespace

int main(int argc, char** argv) {
    if (argc < 2) {
        std::printf("usage: image_check.exe <unpacked FFXiMain.dll> [expected addresses]\n");
        return 2;
    }
    const uint8_t* text = NULL;
    size_t size = 0;
    if (!map_image(argv[1], &text, &size) || !text) {
        std::printf("FAIL  could not map %s\n", argv[1]);
        return 1;
    }

    const uint8_t* registry = find(text, size, "menu_registry", kSigRegistry);
    const uint8_t* manager = find(text, size, "menu_manager", kSigManager);
    const uint8_t* masks = find(text, size, "dock_masks", kSigDockMasks);
    const uint8_t* set_position = find(text, size, "set_position", kSigSetPosition);
    const uint8_t* close = find(text, size, "close_by_name", kSigCloseByName);
    const uint8_t* sites[kSiteCount];
    for (int i = 0; i < kSiteCount; ++i) {
        sites[i] = kSites[i].signature ? find(text, size, kSites[i].name, kSites[i].signature) : NULL;
    }
    // The key test: the UI update's call at kUpdateKeyTestCall, its
    // prologue the push and the read of the input mute byte, its body next.
    if (sites[kSiteUpdate] && sites[kSiteUpdate][kUpdateKeyTestCall] == 0xE8) {
        const uint8_t* at = asked_call(sites[kSiteUpdate] + kUpdateKeyTestCall);
        char label[192];
        snprintf(label, sizeof(label), "key_test: the UI update's call at +%u reaches 0x%08X, push ecx, mov al,[imm32],"
            " then its body", static_cast<unsigned>(kUpdateKeyTestCall),
            static_cast<unsigned>(reinterpret_cast<uintptr_t>(at)));
        const bool ok = in_text(at, text, size) && memcmp(at, kKeyTestHead, sizeof(kKeyTestHead)) == 0
            && memcmp(at + kKeyTestPrologue, kKeyTestBody, sizeof(kKeyTestBody)) == 0;
        check(ok, label);
        sites[kSiteKeyTest] = ok ? at : NULL;
        if (ok) {
            const uint8_t* mute = rdptr(at, sizeof(kKeyTestHead));
            snprintf(label, sizeof(label), "key_test: the byte its prologue reads, 0x%08X, is data in the image, outside"
                " .text", static_cast<unsigned>(reinterpret_cast<uintptr_t>(mute)));
            check(mute >= image && mute < image + image_size && !(mute >= text && mute < text + size), label);
        }
    } else {
        check(false, "key_test: the UI update has no call at its +20");
    }
    const uint8_t* calls[kCallCount];
    for (int i = 0; i < kCallCount; ++i) {
        calls[i] = find(text, size, kCalls[i].name, kCalls[i].signature);
    }
    const uint8_t* link5_site = find(text, size, "link5_cache", kSigLink5Cache);
    const uint8_t* cancel_site = find(text, size, "query_cancel_allowed", kSigQueryCancelAllowed);
    const uint8_t* set_cursor = find(text, size, "set_cursor", kSigSetCursor);
    const uint8_t* latch_site = find(text, size, "link5_latch", kSigLink5Latch);
    const uint8_t* open_site = find(text, size, "link5_open", kSigLink5Open);
    const uint8_t* glyph_convert = find(text, size, "glyph_convert", kSigGlyphConvert);
    const uint8_t* hit_test = find(text, size, "row_hit_test", kSigRowHitTest);
    const uint8_t* routing = find(text, size, "menu_routing", kSigMenuRouting);
    const uint8_t* macro_site = find(text, size, "macro_object", kSigMacroObject);
    const uint8_t* yesno_site = find(text, size, "yesno_class", kSigYesNoClass);
    const uint8_t* yesno_confirm = find(text, size, "yesno_confirm", kSigYesNoConfirm);
    const uint8_t* dead_site = find(text, size, "dead_confirm", kSigDeadConfirm);
    const uint8_t* dead_tick = find(text, size, "dead_tick", kSigDeadTick);

    if (registry) {
        check(rd32(registry, 1) == rd32(registry, 19), "menu_registry: its two table addresses agree");
        const uint8_t* table = rdptr(registry, 1);
        char why[256] = "";
        const bool ok = verify_registry(table, kRowCount + 1, why, sizeof(why));
        char label[320];
        snprintf(label, sizeof(label), "the menu table verifies against the inventory%s%s", ok ? "" : ": ", why);
        check(ok, label);
        expect("menu table", reinterpret_cast<uintptr_t>(table), argc, argv, 2);
    }
    if (manager) {
        expect("menu manager", rd32(manager, 5), argc, argv, 3);
    }
    if (masks) {
        const uint8_t* table = rdptr(masks, 3);
        check(memcmp(table, kDockMaskTable, sizeof(kDockMaskTable)) == 0,
            "the dock mask table reads 1000 2000 4000 10000000 0");
        expect("dock mask table", reinterpret_cast<uintptr_t>(table), argc, argv, 4);
    }
    if (set_position) {
        expect("set_position", reinterpret_cast<uintptr_t>(set_position), argc, argv, 5);
    }
    if (close) {
        expect("close_by_name", reinterpret_cast<uintptr_t>(close), argc, argv, 6);
    }
    for (int i = 0; i < kSiteCount; ++i) {
        if (sites[i]) {
            expect(kSites[i].name, reinterpret_cast<uintptr_t>(sites[i]), argc, argv, 7 + i);
        }
        if (sites[i] && kSites[i].manager_imm && manager) {
            char label[128];
            snprintf(label, sizeof(label), "%s: its imm32 at +%u is the menu manager 0x%08X", kSites[i].name,
                static_cast<unsigned>(kSites[i].manager_imm), static_cast<unsigned>(rd32(manager, 5)));
            check(rd32(sites[i], kSites[i].manager_imm) == rd32(manager, 5), label);
        }
    }
    // The quad routine, never hooked: its call at kQuadPrimitiveCall must
    // reach the draw primitive.
    const uint8_t* quad = find(text, size, "quad_draw", kSigQuadDraw);
    if (quad) {
        expect("quad_draw", reinterpret_cast<uintptr_t>(quad), argc, argv, 7 + kSiteCount);
        char label[128];
        snprintf(label, sizeof(label), "quad_draw: its call at +0x%03X reaches primitive_draw",
            static_cast<unsigned>(kQuadPrimitiveCall));
        check(quad[kQuadPrimitiveCall] == 0xE8 && sites[kSitePrimitiveDraw]
                && asked_call(quad + kQuadPrimitiveCall) == sites[kSitePrimitiveDraw], label);
    }
    for (int i = 0; i < kCallCount; ++i) {
        if (calls[i]) {
            expect(kCalls[i].name, reinterpret_cast<uintptr_t>(calls[i]), argc, argv, 8 + kSiteCount + i);
        }
    }
    if (calls[kCallPartyClear]) {
        const uint8_t* flag = rdptr(calls[kCallPartyClear], kPartyClearFlag);
        char label[128];
        snprintf(label, sizeof(label), "prtyjoin pending flag 0x%08X is data in the image, outside .text",
            static_cast<unsigned>(reinterpret_cast<uintptr_t>(flag)));
        check(flag >= image && flag < image + image_size && !(flag >= text && flag < text + size), label);
    }
    const int data_arg = 8 + kSiteCount + kCallCount;
    if (link5_site) {
        expect("link5 cache site", reinterpret_cast<uintptr_t>(link5_site), argc, argv, data_arg);
        const uint8_t* cache = rdptr(link5_site, kLink5CacheImm);
        expect("link5 concierge cache", reinterpret_cast<uintptr_t>(cache), argc, argv, data_arg + 1);
        const uint8_t* end = cache + kLink5Slots * kLink5Entry;
        char label[128];
        snprintf(label, sizeof(label), "link5 concierge cache 0x%08X, all %d entries, is data in the image, outside .text",
            static_cast<unsigned>(reinterpret_cast<uintptr_t>(cache)), kLink5Slots);
        check(cache >= image && end <= image + image_size && (end <= text || cache >= text + size), label);
    }
    if (cancel_site) {
        expect("query cancel site", reinterpret_cast<uintptr_t>(cancel_site), argc, argv, data_arg + 2);
        const uint8_t* allowed = rdptr(cancel_site, kQueryCancelImm);
        expect("query cancel-allowed byte", reinterpret_cast<uintptr_t>(allowed), argc, argv, data_arg + 3);
        char label[128];
        snprintf(label, sizeof(label), "query cancel-allowed byte 0x%08X is data in the image, outside .text",
            static_cast<unsigned>(reinterpret_cast<uintptr_t>(allowed)));
        check(allowed >= image && allowed < image + image_size && !(allowed >= text && allowed < text + size), label);
    }
    if (set_cursor) {
        expect("set_cursor", reinterpret_cast<uintptr_t>(set_cursor), argc, argv, data_arg + 4);
    }
    if (latch_site) {
        expect("link5 latch site", reinterpret_cast<uintptr_t>(latch_site), argc, argv, data_arg + 5);
        const uint8_t* latch = rdptr(latch_site, kLink5LatchImm);
        expect("link5 pending latch", reinterpret_cast<uintptr_t>(latch), argc, argv, data_arg + 6);
        char label[128];
        snprintf(label, sizeof(label), "link5 pending latch 0x%08X is data in the image, outside .text",
            static_cast<unsigned>(reinterpret_cast<uintptr_t>(latch)));
        check(latch >= image && latch < image + image_size && !(latch >= text && latch < text + size), label);
    }
    if (open_site) {
        expect("link5 open site", reinterpret_cast<uintptr_t>(open_site), argc, argv, data_arg + 7);
        const uint8_t* global = rdptr(open_site, kLink5OpenGlobal);
        const uint8_t* callback = rdptr(open_site, kLink5OpenCallback);
        expect("link5 controller global", reinterpret_cast<uintptr_t>(global), argc, argv, data_arg + 8);
        expect("link5 event callback", reinterpret_cast<uintptr_t>(callback), argc, argv, data_arg + 9);
        char label[160];
        snprintf(label, sizeof(label), "link5 event callback 0x%08X is in .text; controller global 0x%08X is data outside it",
            static_cast<unsigned>(reinterpret_cast<uintptr_t>(callback)),
            static_cast<unsigned>(reinterpret_cast<uintptr_t>(global)));
        check(callback >= text && callback < text + size && global >= image && global < image + image_size
                && !(global >= text && global < text + size), label);
        if (registry) {
            Inventory inv;
            inv.build();
            const int n = inv.find_exact("link5");
            const uint8_t* table = rdptr(registry, 1);
            const uint32_t slot = n >= 0 ? rd32(table + inv.names[n].rows[0] * kRowStride, kRowSlot) : 0;
            snprintf(label, sizeof(label), "the link5 open site's controller global is the registry's link5 slot (0x%08X)",
                static_cast<unsigned>(slot));
            check(slot != 0 && slot == reinterpret_cast<uintptr_t>(global), label);
        }
    }
    if (glyph_convert) {
        expect("glyph_convert", reinterpret_cast<uintptr_t>(glyph_convert), argc, argv, data_arg + 10);
    }
    if (hit_test) {
        expect("row_hit_test", reinterpret_cast<uintptr_t>(hit_test), argc, argv, data_arg + 11);
        const uint8_t* read = NULL;
        int reads = 0;
        for (size_t i = 0; i + sizeof(kHitTestMouseRead) <= kHitTestBytes; ++i) {
            if (memcmp(hit_test + i, kHitTestMouseRead, sizeof(kHitTestMouseRead)) == 0) {
                read = hit_test + i;
                ++reads;
            }
        }
        char label[128];
        snprintf(label, sizeof(label), "row_hit_test: its test of menu+0x%02X is there once in its first 0x%X bytes (%d)",
            static_cast<unsigned>(kMenuMouse), static_cast<unsigned>(kHitTestBytes), reads);
        check(reads == 1 && kHitTestMouseRead[kHitTestMouseByte] == kMenuMouse, label);
        if (read) {
            expect("row_hit_test's menu+0x77 test", reinterpret_cast<uintptr_t>(read), argc, argv, data_arg + 12);
        }
    }
    if (routing) {
        expect("menu_routing", reinterpret_cast<uintptr_t>(routing), argc, argv, data_arg + 13);
        const uint8_t* call = routing + kRoutingSinkCall;
        int32_t rel;
        memcpy(&rel, call + 1, 4);
        const uint8_t* target = call + 5 + rel;
        char label[160];
        snprintf(label, sizeof(label), "menu_routing: its call at +0x%02X reaches menu_input (0x%08X), returning to 0x%08X",
            static_cast<unsigned>(kRoutingSinkCall), static_cast<unsigned>(reinterpret_cast<uintptr_t>(target)),
            static_cast<unsigned>(reinterpret_cast<uintptr_t>(call + 5)));
        check(call[0] == 0xE8 && sites[kSiteMenuInput] && target == sites[kSiteMenuInput], label);
    }
    if (sites[kSiteCompassDraw]) {
        const uint8_t* site = sites[kSiteCompassDraw];
        const uint8_t* global = rdptr(site, kCompassDrawGlobal);
        check(rd32(site, kCompassDrawGlobal) == rd32(site, kCompassDrawGlobal2),
            "compass_draw: its two compass globals, at +2 and +31, agree");
        expect("compass global", reinterpret_cast<uintptr_t>(global), argc, argv, data_arg + 14);
        char label[128];
        snprintf(label, sizeof(label), "compass global 0x%08X is data in the image, outside .text",
            static_cast<unsigned>(reinterpret_cast<uintptr_t>(global)));
        check(global >= image && global + 4 <= image + image_size && !(global >= text && global < text + size), label);
    }
    if (sites[kSiteMacroGate]) {
        const uint8_t* word = rdptr(sites[kSiteMacroGate], kMacroGateGlobal);
        char label[128];
        snprintf(label, sizeof(label), "macro_gate: the word its prologue reads, 0x%08X, is data in the image, outside .text",
            static_cast<unsigned>(reinterpret_cast<uintptr_t>(word)));
        check(word >= image && word + 2 <= image + image_size && !(word >= text && word < text + size), label);
    }
    if (sites[kSiteAbilityOpen]) {
        const uint8_t* site = sites[kSiteAbilityOpen];
        const uint8_t* global = rdptr(site, kAbilityOpenGlobal);
        char label[192];
        snprintf(label, sizeof(label), "ability_open: the global its prologue reads, 0x%08X, is data in the image, outside .text",
            static_cast<unsigned>(reinterpret_cast<uintptr_t>(global)));
        check(global >= image && global + 4 <= image + image_size && !(global >= text && global < text + size), label);
        check(rd32(site, kAbilityOpenManagerImm) == rd32(site, kAbilityOpenManagerImm + 19),
            "ability_open: its two manager imm32s, at +29 and +48, agree");
        Inventory inv;
        inv.build();
        const uint8_t* first = rdptr(site, kAbilityOpenKeyImm - 19);
        const uint8_t* key = rdptr(site, kAbilityOpenKeyImm);
        const bool first_data = first >= image && first + kKeyLen <= image + image_size
            && !(first >= text && first < text + size);
        const bool key_data = key >= image && key + kKeyLen <= image + image_size && !(key >= text && key < text + size);
        snprintf(label, sizeof(label), "ability_open: its first key push, at +%u, names the registry's abisortw key (0x%08X)",
            static_cast<unsigned>(kAbilityOpenKeyImm - 19), static_cast<unsigned>(reinterpret_cast<uintptr_t>(first)));
        check(first_data && inv.find_key16(first) >= 0 && inv.find_key16(first) == inv.find_exact("abisortw"), label);
        snprintf(label, sizeof(label), "ability_open: its second key push, at +%u, names the registry's ability key (0x%08X)",
            static_cast<unsigned>(kAbilityOpenKeyImm), static_cast<unsigned>(reinterpret_cast<uintptr_t>(key)));
        check(key_data && inv.ability >= 0 && memcmp(key, inv.names[inv.ability].key, kKeyLen) == 0, label);
    }
    if (macro_site) {
        check(rd32(macro_site, kMacroObjectImm) == rd32(macro_site, kMacroObjectImm2),
            "macro_object: its two globals, at +15 and +23, agree");
        const uint8_t* global = rdptr(macro_site, kMacroObjectImm);
        expect("macro object global", reinterpret_cast<uintptr_t>(global), argc, argv, data_arg + 15);
        char label[128];
        snprintf(label, sizeof(label), "macro object global 0x%08X is data in the image, outside .text",
            static_cast<unsigned>(reinterpret_cast<uintptr_t>(global)));
        check(global >= image && global + 4 <= image + image_size && !(global >= text && global < text + size), label);
    }
    if (yesno_site) {
        expect("yes/no class constructor", reinterpret_cast<uintptr_t>(yesno_site), argc, argv, data_arg + 16);
        const uint8_t* vtable = rdptr(yesno_site, kYesNoClassVtable);
        expect("yes/no class vtable", reinterpret_cast<uintptr_t>(vtable), argc, argv, data_arg + 17);
        char label[128];
        snprintf(label, sizeof(label), "yes/no class vtable 0x%08X is data in the image, outside .text",
            static_cast<unsigned>(reinterpret_cast<uintptr_t>(vtable)));
        check(vtable >= image && vtable + 4 <= image + image_size && !(vtable >= text && vtable < text + size), label);
    }
    if (yesno_confirm) {
        expect("yes/no class confirm", reinterpret_cast<uintptr_t>(yesno_confirm), argc, argv, data_arg + 18);
        const struct {
            const uint8_t* bytes;
            size_t length;
            const char* what;
        } fields[] = {
            {kConfirmChainedRead, sizeof(kConfirmChainedRead), "the callback called, then the chained flag +0x3A read"},
            {kConfirmCallbackClear, sizeof(kConfirmCallbackClear), "the callback +0x14 cleared"},
            {kConfirmChainedDefault, sizeof(kConfirmChainedDefault),
             "the chained default +0x3B read as the chained flag is cleared"},
        };
        for (size_t f = 0; f < sizeof(fields) / sizeof(fields[0]); ++f) {
            int found = 0;
            for (size_t i = 0; i + fields[f].length <= kYesNoConfirmBytes; ++i) {
                found += memcmp(yesno_confirm + i, fields[f].bytes, fields[f].length) == 0;
            }
            char label[160];
            snprintf(label, sizeof(label), "yesno_confirm: %s, once in its first 0x%X bytes (%d)", fields[f].what,
                static_cast<unsigned>(kYesNoConfirmBytes), found);
            check(found == 1, label);
        }
    }
    const uint8_t* dead_decider = NULL;
    if (dead_site) {
        expect("KO window confirm site", reinterpret_cast<uintptr_t>(dead_site), argc, argv, data_arg + 19);
        int32_t rel;
        memcpy(&rel, dead_site + kDeadDeciderCall + 1, 4);
        dead_decider = dead_site + kDeadDeciderCall + 5 + rel;
        memcpy(&rel, dead_site + kDeadMessageCall + 1, 4);
        const uint8_t* message = dead_site + kDeadMessageCall + 5 + rel;
        memcpy(&rel, dead_site + kDeadAskCall + 1, 4);
        const uint8_t* ask = dead_site + kDeadAskCall + 5 + rel;
        const uint8_t* callback = rdptr(dead_site, kDeadAskCallback);
        expect("KO window decider", reinterpret_cast<uintptr_t>(dead_decider), argc, argv, data_arg + 20);
        expect("message getter", reinterpret_cast<uintptr_t>(message), argc, argv, data_arg + 21);
        expect("yes/no class ask", reinterpret_cast<uintptr_t>(ask), argc, argv, data_arg + 22);
        expect("KO window question callback", reinterpret_cast<uintptr_t>(callback), argc, argv, data_arg + 23);
        char label[256];
        snprintf(label, sizeof(label), "dead_confirm: the decider's arguments (%d, %d); message 0x%X of category"
            " 0x%X when it answers 0, 0x%X when 1; the ask's context %d, default %d, help %d, cancellable %d, mode %d",
            static_cast<int8_t>(dead_site[kDeadDeciderFirst]), static_cast<int8_t>(dead_site[kDeadDeciderSecond]),
            static_cast<int8_t>(dead_site[kDeadTextOtherwise]), static_cast<int8_t>(dead_site[kDeadCategory]),
            static_cast<int>(rd32(dead_site, kDeadTextIfSet)), static_cast<int8_t>(dead_site[kDeadAskContext]),
            static_cast<int8_t>(dead_site[kDeadAskDefault]), static_cast<int8_t>(dead_site[kDeadAskHelp]),
            static_cast<int8_t>(dead_site[kDeadAskCancellable]), static_cast<int8_t>(dead_site[kDeadAskMode]));
        check(dead_site[kDeadDeciderFirst] == 1 && dead_site[kDeadDeciderSecond] == 0
                && dead_site[kDeadTextOtherwise] == 0x46 && dead_site[kDeadCategory] == 0x0E
                && rd32(dead_site, kDeadTextIfSet) == 0x258 && dead_site[kDeadAskContext] == 0
                && dead_site[kDeadAskDefault] == 0 && dead_site[kDeadAskHelp] == 0
                && dead_site[kDeadAskCancellable] == 1 && dead_site[kDeadAskMode] == 0, label);
        snprintf(label, sizeof(label), "dead_confirm: the decider, the getter, the ask and the callback are in .text");
        check(in_text(dead_decider, text, size) && in_text(message, text, size) && in_text(ask, text, size)
                && in_text(callback, text, size), label);
        check(memcmp(ask, kYesNoAskHead, sizeof(kYesNoAskHead)) == 0,
            "dead_confirm: its ask tests the yes/no class's pending callback first and pops seven arguments");
        if (registry) {
            Inventory inv;
            inv.build();
            const uint8_t* table = rdptr(registry, 1);
            const uint32_t comyn = rd32(table + inv.names[inv.find_exact("comyn")].rows[0] * kRowStride, kRowSlot);
            const uint32_t dead = rd32(table + inv.names[inv.find_exact("dead")].rows[0] * kRowStride, kRowSlot);
            snprintf(label, sizeof(label), "dead_confirm: it asks through the registry's comyn slot (0x%08X)",
                static_cast<unsigned>(comyn));
            check(comyn != 0 && rd32(dead_site, kDeadAskerGlobal) == comyn, label);
            const size_t head = sizeof(kDeadChosenHead);
            snprintf(label, sizeof(label), "dead_confirm: its callback marks +0x%02X of the controller in the registry's"
                " dead slot (0x%08X) on a yes", static_cast<unsigned>(kDeadChosen), static_cast<unsigned>(dead));
            check(dead != 0 && memcmp(callback, kDeadChosenHead, head) == 0 && rd32(callback, head) == dead
                    && memcmp(callback + head + 4, kDeadChosenTail, sizeof(kDeadChosenTail)) == 0, label);
        }
    }
    if (dead_tick) {
        expect("KO window tick", reinterpret_cast<uintptr_t>(dead_tick), argc, argv, data_arg + 24);
        int32_t rel;
        memcpy(&rel, dead_tick + kDeadTickDeciderCall + 1, 4);
        check(dead_decider && dead_tick + kDeadTickDeciderCall + 5 + rel == dead_decider,
            "dead_tick: its decider is the one the KO window's confirm calls");
    }
    if (yesno_site) {
        const uint8_t* vtable = rdptr(yesno_site, kYesNoClassVtable);
        const uint8_t* input = rdptr(vtable, kYesNoInputSlot * 4);
        const uint8_t* test = NULL;
        int tests = 0;
        for (size_t i = 0; in_text(input, text, size) && i + sizeof(kYesNoCancelTest) <= kYesNoInputBytes; ++i) {
            if (memcmp(input + i, kYesNoCancelTest, sizeof(kYesNoCancelTest)) == 0) {
                test = input + i;
                ++tests;
            }
        }
        char label[192];
        snprintf(label, sizeof(label), "yes/no class: its input routine 0x%08X tests the cancellable byte +0x%02X once in"
            " its first 0x%X bytes (%d), then calls back with the context and no",
            static_cast<unsigned>(reinterpret_cast<uintptr_t>(input)), static_cast<unsigned>(kYesNoCancellable),
            static_cast<unsigned>(kYesNoInputBytes), tests);
        check(tests == 1 && kYesNoCancelTest[2] == kYesNoCancellable && kYesNoCancelCallback[2] == kYesNoContext
                && memcmp(test + kYesNoCancelCall, kYesNoCancelCallback, sizeof(kYesNoCancelCallback)) == 0, label);
        if (test) {
            expect("yes/no class cancel test", reinterpret_cast<uintptr_t>(test), argc, argv, data_arg + 25);
        }
    }
    const char* const box_confirms[kBoxCount] = {kSigMoneyConfirm, kSigItemConfirm};
    const char* const box_setups[kBoxCount] = {kSigMoneySetup, kSigItemSetup};
    const uint8_t* setups[kBoxCount] = {NULL, NULL};
    Inventory boxes;
    boxes.build();
    for (int b = 0; b < kBoxCount; ++b) {
        char name[64];
        snprintf(name, sizeof(name), "%s_confirm", kBoxNames[b]);
        const uint8_t* confirm = find(text, size, name, box_confirms[b]);
        if (confirm) {
            expect(name, reinterpret_cast<uintptr_t>(confirm), argc, argv, data_arg + 26 + b);
            char label[128];
            snprintf(label, sizeof(label), "%s: its close goes through the menu manager", name);
            check(manager && rd32(confirm, kBoxConfirmManager[b]) == rd32(manager, 5), label);
        }
        snprintf(name, sizeof(name), "%s_setup", kBoxNames[b]);
        setups[b] = find(text, size, name, box_setups[b]);
        if (setups[b]) {
            const uint8_t* setup = setups[b];
            expect(name, reinterpret_cast<uintptr_t>(setup), argc, argv, data_arg + 28 + b);
            const uint8_t* key = rdptr(setup, kBoxSetupKey[b]);
            int32_t rel;
            memcpy(&rel, setup + kBoxSetupOpen[b] + 1, 4);
            const uint8_t* open = setup + kBoxSetupOpen[b] + 5 + rel;
            char label[192];
            snprintf(label, sizeof(label), "%s: it opens the registry's %s key through the manager's open routine",
                name, kBoxNames[b]);
            check(key >= image && key + kKeyLen <= image + image_size
                    && memcmp(key, boxes.names[boxes.box_window[b]].key, kKeyLen) == 0 && manager
                    && rd32(setup, kBoxSetupManager[b]) == rd32(manager, 5) && setup[kBoxSetupOpen[b]] == 0xE8
                    && sites[kSiteOpen] && open == sites[kSiteOpen], label);
        }
    }
    if (setups[kBoxMoney]) {
        check(memcmp(setups[kBoxMoney] + kMoneySetupTailAt, kMoneySetupTail, sizeof(kMoneySetupTail)) == 0,
            "moneyctr_setup: it writes the max, the context and the extra callback, then calls the open");
    }
    const uint8_t* callbacks[kAskerCount];
    for (int i = 0; i < kAskerCount; ++i) {
        const AskerSite& site = kAskerSites[i];
        const AskerSpec& a = kAskers[i];
        const uint8_t* hit = find(text, size, site.name, site.signature);
        callbacks[i] = NULL;
        if (!hit) {
            continue;
        }
        const uint8_t* callback = site.callback ? rdptr(hit, site.callback) : hit;
        callbacks[i] = callback;
        expect(site.name, reinterpret_cast<uintptr_t>(callback), argc, argv, data_arg + 30 + i);
        bool ok = in_text(callback, text, size);
        if (site.setup_call) {
            int32_t rel;
            memcpy(&rel, hit + site.setup_call + 1, 4);
            ok = ok && hit[site.setup_call] == 0xE8 && hit + site.setup_call + 5 + rel == setups[a.box];
        }
        const uint8_t* table = registry ? rdptr(registry, 1) : NULL;
        if (site.box_global) {
            const int n = boxes.box_window[a.box];
            ok = ok && table && rd32(hit, site.box_global) == rd32(table + boxes.names[n].rows[0] * kRowStride, kRowSlot);
        }
        if (site.owner_global) {
            const int n = boxes.find_exact(a.owner);
            ok = ok && table && n >= 0
                && rd32(hit, site.owner_global) == rd32(table + boxes.names[n].rows[0] * kRowStride, kRowSlot);
        }
        char label[256];
        snprintf(label, sizeof(label), "%s at 0x%08X: its callback 0x%08X is in .text%s%s%s", site.name,
            static_cast<unsigned>(reinterpret_cast<uintptr_t>(hit)),
            static_cast<unsigned>(reinterpret_cast<uintptr_t>(callback)),
            site.setup_call ? "; its call reaches the box's setup" : "",
            site.box_global ? "; its box global is the box's registry slot" : "",
            site.owner_global ? "; its global is its window's registry slot" : "");
        check(ok, label);
    }
    {
        bool distinct = true;
        for (int i = 0; i < kAskerCount; ++i) {
            for (int k = 0; k < i; ++k) {
                distinct = distinct && (!callbacks[i] || callbacks[i] != callbacks[k]);
            }
        }
        check(distinct, "every asker asks with a callback of its own");
    }
    // The Asked windows: resolve_asked() as the engine runs it, then where
    // each site and what it leads to lies.
    if (registry && manager && close && sites[kSiteOpen] && dead_site && cancel_site) {
        Inventory inv;
        inv.build();
        AskedInputs in;
        memset(&in, 0, sizeof(in));
        in.text = text;
        in.size = size;
        const IMAGE_NT_HEADERS32* nt = reinterpret_cast<const IMAGE_NT_HEADERS32*>(
            image + reinterpret_cast<const IMAGE_DOS_HEADER*>(image)->e_lfanew);
        const IMAGE_SECTION_HEADER* section = IMAGE_FIRST_SECTION(nt);
        for (unsigned i = 0; i < nt->FileHeader.NumberOfSections; ++i, ++section) {
            const DWORD flags = section->Characteristics;
            if ((flags & IMAGE_SCN_MEM_READ) && !(flags & (IMAGE_SCN_MEM_WRITE | IMAGE_SCN_MEM_EXECUTE))
                && in.data_count < kDataSections) {
                in.data[in.data_count] = image + section->VirtualAddress;
                in.data_size[in.data_count] = section->Misc.VirtualSize;
                ++in.data_count;
            }
        }
        in.inv = &inv;
        in.registry = rdptr(registry, 1);
        in.mcb = rdptr(manager, 5);
        in.open = sites[kSiteOpen];
        in.close = close;
        in.message = asked_call(dead_site + kDeadMessageCall);
        in.key_test = sites[kSiteKeyTest];
        in.cancel_allowed = rdptr(cancel_site, kQueryCancelImm);
        const uint8_t* close_instance = close[kCloseByNameInstanceCall] == 0xE8
            ? asked_call(close + kCloseByNameInstanceCall) : NULL;
        AskedSites a;
        char why[256] = "";
        const bool ok = close_instance && in_text(close_instance, text, size)
            && resolve_asked(in, close_instance, &a, why, sizeof(why));
        char label[320];
        snprintf(label, sizeof(label), "the Asked windows resolve: each signature once, and every key, global,"
            " routine and byte the engine checks in them%s%s", ok ? "" : ": ", why);
        check(ok, label);
        if (ok) {
            const int at = data_arg + 46;
            const void* fns[8];
            memcpy(&fns[0], &a.evitem_callback, 4);
            memcpy(&fns[1], &a.confyn_restore, 4);
            memcpy(&fns[2], &a.bank_clear, 4);
            memcpy(&fns[3], &a.maplist_close, 4);
            memcpy(&fns[4], &a.maplist_rebuild, 4);
            memcpy(&fns[5], &a.maplist_select, 4);
            memcpy(&fns[6], &a.maplist_cursor, 4);
            memcpy(&fns[7], &a.map_close, 4);
            const struct {
                const char* name;
                uintptr_t got;
            } found[] = {
                {"close by instance", reinterpret_cast<uintptr_t>(close_instance)},
                {"sortyn_setup", reinterpret_cast<uintptr_t>(a.sortyn_setup)},
                {"sortyn_confirm", reinterpret_cast<uintptr_t>(a.sortyn_confirm)},
                {"evitem_ask", reinterpret_cast<uintptr_t>(a.evitem_ask)},
                {"evitem01's setup", reinterpret_cast<uintptr_t>(a.evitem_setup)},
                {"evitem01's callback", reinterpret_cast<uintptr_t>(fns[0])},
                {"evitem_input", reinterpret_cast<uintptr_t>(a.evitem_input)},
                {"confyn_input", reinterpret_cast<uintptr_t>(a.confyn_input)},
                {"confyn's restore", reinterpret_cast<uintptr_t>(fns[1])},
                {"bank_open", reinterpret_cast<uintptr_t>(a.bank_open)},
                {"bankmenu's clear", reinterpret_cast<uintptr_t>(fns[2])},
                {"the player data's global", reinterpret_cast<uintptr_t>(a.bank_data)},
                {"the Mog Safe 2 flag's offset", a.bank_safe2},
                {"bank_input", reinterpret_cast<uintptr_t>(a.bank_input)},
                {"maplist_close", reinterpret_cast<uintptr_t>(fns[3])},
                {"maplist's rebuild", reinterpret_cast<uintptr_t>(fns[4])},
                {"maplist's select", reinterpret_cast<uintptr_t>(fns[5])},
                {"maplist's cursor read", reinterpret_cast<uintptr_t>(fns[6])},
                {"map0_input", reinterpret_cast<uintptr_t>(a.map0_input)},
                {"the map's close", reinterpret_cast<uintptr_t>(fns[7])},
                {"gmtell_poll", reinterpret_cast<uintptr_t>(a.gm_poll)},
                {"the GM tell queue's slot", reinterpret_cast<uintptr_t>(a.gm_slot)},
                {"the GM tell queue", reinterpret_cast<uintptr_t>(a.gm_queue)},
                {"mp_dead_input", reinterpret_cast<uintptr_t>(a.mp_dead_input)},
                {"ok_input", reinterpret_cast<uintptr_t>(a.ok_input)},
            };
            const int count = static_cast<int>(sizeof(found) / sizeof(found[0]));
            for (int i = 0; i < count; ++i) {
                expect(found[i].name, found[i].got, argc, argv, at + i);
            }
            const DoorSites& d = a.door;
            const void* door_fns[6];
            memcpy(&door_fns[0], &d.gate, 4);
            memcpy(&door_fns[1], &d.floor, 4);
            memcpy(&door_fns[2], &d.condition, 4);
            memcpy(&door_fns[3], &d.tick, 4);
            memcpy(&door_fns[4], &d.open_door, 4);
            door_fns[5] = d.opened;
            const struct {
                const char* name;
                uintptr_t got;
            } door[] = {
                {"door_input", reinterpret_cast<uintptr_t>(d.input)},
                {"mogdoor's vtable", reinterpret_cast<uintptr_t>(d.vtable)},
                {"mogdoor's open hook", reinterpret_cast<uintptr_t>(d.open_hook)},
                {"mogdoor's draw", reinterpret_cast<uintptr_t>(d.draw)},
                {"door_handler", reinterpret_cast<uintptr_t>(d.handler)},
                {"mogdoor's confirm gate", reinterpret_cast<uintptr_t>(door_fns[0])},
                {"mogdoor's floor read", reinterpret_cast<uintptr_t>(door_fns[1])},
                {"mogdoor's draw condition", reinterpret_cast<uintptr_t>(door_fns[2])},
                {"mogdoor's tick", reinterpret_cast<uintptr_t>(door_fns[3])},
                {"the door actor's open", reinterpret_cast<uintptr_t>(door_fns[4])},
                {"the routine the exit gives the door", reinterpret_cast<uintptr_t>(door_fns[5])},
                {"the Mog House object's global", reinterpret_cast<uintptr_t>(d.house)},
                {"the exit's first byte", reinterpret_cast<uintptr_t>(d.exiting[0])},
                {"the exit's second byte", reinterpret_cast<uintptr_t>(d.exiting[1])},
                {"the exit's third byte", reinterpret_cast<uintptr_t>(d.exiting[2])},
                {"the nations' lists of areas", reinterpret_cast<uintptr_t>(d.table)},
            };
            const int doors = static_cast<int>(sizeof(door) / sizeof(door[0]));
            for (int i = 0; i < doors; ++i) {
                expect(door[i].name, door[i].got, argc, argv, at + count + i);
            }
            char label[256];
            snprintf(label, sizeof(label), "mogdoor: its gate called with (%d, %d), the floor word +0x%X of the player"
                " data, nations 1..%d, the Mog Garden zone %d", static_cast<int>(d.gate_first),
                static_cast<int>(d.gate_second), static_cast<unsigned>(d.floor_word), static_cast<int>(d.nations),
                static_cast<int>(d.garden));
            check(d.gate_first == 4 && d.gate_second == 0 && d.nations == 10 && d.garden == 280, label);
            const uint8_t* exit_manager = d.exiting[1];
            snprintf(label, sizeof(label), "mogdoor: the exit's second byte is the menu manager's +0xAE (0x%08X)",
                static_cast<unsigned>(reinterpret_cast<uintptr_t>(exit_manager)));
            check(exit_manager == rdptr(manager, 5) + 0xAE, label);
            DialogueSites t;
            const bool talks = resolve_dialogue(in, close_instance, &t, why, sizeof(why));
            snprintf(label, sizeof(label), "the dialogue boxes resolve: their input routine once, its handler, the"
                " text dispatch handing its code on to the other box's text object, its gate's advance and page-turn"
                " sound, the cancel, the tick through their vtables and its left-button test%s%s",
                talks ? "" : ": ", why);
            check(talks, label);
            if (talks) {
                const void* fns[4];
                memcpy(&fns[0], &t.advance, 4);
                memcpy(&fns[1], &t.page_sound, 4);
                memcpy(&fns[2], &t.release, 4);
                memcpy(&fns[3], &t.clear, 4);
                const struct {
                    const char* name;
                    uintptr_t got;
                } dialogue[] = {
                    {"dialogue_input", reinterpret_cast<uintptr_t>(t.input)},
                    {"the dialogue boxes' handler", reinterpret_cast<uintptr_t>(t.handler)},
                    {"the text dispatch", reinterpret_cast<uintptr_t>(t.dispatch)},
                    {"the text advance", reinterpret_cast<uintptr_t>(fns[0])},
                    {"the gate's page-turn sound", reinterpret_cast<uintptr_t>(fns[1])},
                    {"the chat log's release", reinterpret_cast<uintptr_t>(fns[2])},
                    {"the text object's clear", reinterpret_cast<uintptr_t>(fns[3])},
                    {"the event's wait flag", reinterpret_cast<uintptr_t>(t.wait)},
                    {"the dialogue boxes' tick", reinterpret_cast<uintptr_t>(t.tick)},
                    {"the tick's left-button test's return", t.click_return},
                    {"the global naming one box's text object, which the dispatch tests first",
                     reinterpret_cast<uintptr_t>(t.texts[0])},
                    {"the global naming the other box's text object", reinterpret_cast<uintptr_t>(t.texts[1])},
                };
                for (int i = 0; i < static_cast<int>(sizeof(dialogue) / sizeof(dialogue[0])); ++i) {
                    expect(dialogue[i].name, dialogue[i].got, argc, argv, at + count + doors + i);
                }
            }
            const int dialogues = 12;
            TextSites x;
            const bool typed = resolve_text(in, close_instance, &x, why, sizeof(why));
            snprintf(label, sizeof(label), "the text-entry windows resolve: their input routines once each, every"
                " shape in them, their fields' ends, starts, vtables, globals and the routines their commits call%s%s",
                typed ? "" : ": ", why);
            check(typed, label);
            if (typed) {
                const struct {
                    const char* name;
                    const void* got;
                } text[] = {
                    {"stringdl_input", x.stringdl_input},
                    {"stringdl's name handler", x.stringdl_name},
                    {"stringdl's slot handler", x.stringdl_slot},
                    {"stringdl's field end", reinterpret_cast<const void*>(x.stringdl_stop)},
                    {"a field's text set", reinterpret_cast<const void*>(x.field_text)},
                    {"the manager's focus", reinterpret_cast<const void*>(x.focus)},
                    {"comment_input", x.comment_input},
                    {"comment's vtable", x.comment_vtable},
                    {"comment's begin-close", x.comment_close},
                    {"comment's field end", reinterpret_cast<const void*>(x.comment_stop)},
                    {"the line's sanitize", reinterpret_cast<const void*>(x.sanitize)},
                    {"the line's trim", reinterpret_cast<const void*>(x.trim)},
                    {"msgline_input", x.msgline_input},
                    {"msgline's field end", reinterpret_cast<const void*>(x.msgline_stop)},
                    {"the player data global msgline tests", x.player},
                    {"the POL interface's global", x.pol},
                    {"the POL interface's number", reinterpret_cast<const void*>(x.pol_number)},
                    {"the clock's text", reinterpret_cast<const void*>(x.clock_text)},
                    {"msgline's mode 0 sender", reinterpret_cast<const void*>(x.send_tell)},
                    {"msgline's mode 1 sender", reinterpret_cast<const void*>(x.send_reply)},
                    {"msgline's mode 2 sender", reinterpret_cast<const void*>(x.send_short)},
                    {"msgline's mode 5 sender", reinterpret_cast<const void*>(x.send_answer)},
                    {"msgline's mode 7 sender", reinterpret_cast<const void*>(x.send_dated)},
                    {"the senders' first callback", x.sent[0]},
                    {"the senders' second callback", x.sent[1]},
                    {"the sending indicator's owner's global", x.sending},
                    {"the sending indicator's mark", reinterpret_cast<const void*>(x.mark)},
                    {"msgline's busy byte", x.busy},
                    {"scname_input", x.scname_input},
                    {"scname's field end", reinterpret_cast<const void*>(x.scname_stop)},
                    {"the player search's global", x.search},
                    {"the search criteria's clear", reinterpret_cast<const void*>(x.criteria_clear)},
                    {"the search criteria's name", reinterpret_cast<const void*>(x.criteria_name)},
                    {"the search's start", reinterpret_cast<const void*>(x.search_start)},
                    {"link11_input", x.link11_input},
                    {"link11's field end", reinterpret_cast<const void*>(x.link11_stop)},
                    {"the second linkshell slot's test", reinterpret_cast<const void*>(x.slot_taken)},
                    {"link5's selection", reinterpret_cast<const void*>(x.selected)},
                    {"the linkshell name's encoder", reinterpret_cast<const void*>(x.encode)},
                    {"the linkshell request's sender", reinterpret_cast<const void*>(x.create)},
                    {"the item record lookup", reinterpret_cast<const void*>(x.item)},
                    {"the request's slot byte", x.made[0]},
                    {"the request's item word", x.made[1]},
                    {"the request's flag byte", x.made[2]},
                    {"the request's timer byte", x.made[3]},
                    {"passinpu_input", x.passinpu_input},
                    {"passinpu's field end", reinterpret_cast<const void*>(x.passinpu_stop)},
                    {"passinpu's field start", reinterpret_cast<const void*>(x.passinpu_start)},
                    {"comment's field start", reinterpret_cast<const void*>(x.comment_start)},
                    {"msgline's field start", reinterpret_cast<const void*>(x.msgline_start)},
                    {"the text entry's manager's global", x.line},
                    {"the manager's unbind", reinterpret_cast<const void*>(x.unbind)},
                    {"the manager's bind", reinterpret_cast<const void*>(x.bind)},
                };
                const int texts = static_cast<int>(sizeof(text) / sizeof(text[0]));
                for (int i = 0; i < texts; ++i) {
                    expect(text[i].name, reinterpret_cast<uintptr_t>(text[i].got), argc, argv,
                        at + count + doors + dialogues + i);
                }
                // The target window's arrow draws: each a call of the sprite
                // draw, the second just after the first; the sprite draw's pre
                // tells them by where each returns.
                const uint8_t* target = find(in.text, in.size, "target_arrow", kSigTargetArrow);
                const uint8_t* pick = find(in.text, in.size, "pick_arrow", kSigPickArrow);
                if (target && pick && sites[kSiteSpriteDraw]) {
                    snprintf(label, sizeof(label), "the two arrow draws call sprite_draw, the second within 0x%X"
                        " bytes after the first", static_cast<unsigned>(kArrowSpan));
                    check(asked_call(target + kTargetArrowCall) == sites[kSiteSpriteDraw]
                            && asked_call(pick + kPickArrowCall) == sites[kSiteSpriteDraw] && pick > target
                            && pick <= target + kArrowSpan, label);
                    expect("the target arrow's draw's return", reinterpret_cast<uintptr_t>(target + kTargetArrowCall + 5),
                        argc, argv, at + count + doors + dialogues + texts);
                    expect("the picking arrow's draw's return", reinterpret_cast<uintptr_t>(pick + kPickArrowCall + 5),
                        argc, argv, at + count + doors + dialogues + texts + 1);
                }
                // The trade windows: resolve_offer() as the engine runs it,
                // after the number boxes' setups and askers.
                const void* askers[kAskerCount];
                for (int i = 0; i < kAskerCount; ++i) {
                    askers[i] = callbacks[i];
                }
                OfferSites o;
                const bool offers = setups[kBoxMoney]
                    && resolve_offer(in, close_instance, setups[kBoxMoney], askers, &o, why, sizeof(why));
                snprintf(label, sizeof(label), "the trade windows resolve: their input routines, the trade's open"
                    " and the inventory list's rows once each, every shape in them and in what they reach%s%s",
                    offers ? "" : ": ", why);
                check(offers, label);
                if (offers) {
                    // The engine never calls the clock; the allocator's call
                    // to it is what resolve_offer() matched kShapeClock at.
                    Shape alloc;
                    const uint8_t* throttle_clock = shape_is(in, o.allocator, kShapeAllocator, &alloc)
                        ? shape_target(in, alloc, 7) : NULL;
                    const struct {
                        const char* name;
                        uintptr_t got;
                    } offer[] = {
                        {"trade_input", reinterpret_cast<uintptr_t>(o.trade_input)},
                        {"the trade's stage", reinterpret_cast<uintptr_t>(o.stage[kOfferTrade])},
                        {"the trade's OK", reinterpret_cast<uintptr_t>(o.ok[kOfferTrade])},
                        {"the trade's cancel", reinterpret_cast<uintptr_t>(o.trade_cancel)},
                        {"the trade's close", reinterpret_cast<uintptr_t>(o.trade_close)},
                        {"the trade's rows' slots", reinterpret_cast<uintptr_t>(o.trade_rows)},
                        {"the trade's send", reinterpret_cast<uintptr_t>(o.send)},
                        {"the allocator", reinterpret_cast<uintptr_t>(o.allocator)},
                        {"trade_open", reinterpret_cast<uintptr_t>(o.trade_open)},
                        {"the trade's mode's global", reinterpret_cast<uintptr_t>(o.mode[kOfferTrade])},
                        {"the actor table", reinterpret_cast<uintptr_t>(o.actors)},
                        {"handover_input", reinterpret_cast<uintptr_t>(o.handover_input)},
                        {"the NPC trade's stage", reinterpret_cast<uintptr_t>(o.stage[kOfferHandover])},
                        {"the NPC trade's OK", reinterpret_cast<uintptr_t>(o.ok[kOfferHandover])},
                        {"the NPC trade's close", reinterpret_cast<uintptr_t>(o.handover_close)},
                        {"the NPC trade's mode's global", reinterpret_cast<uintptr_t>(o.mode[kOfferHandover])},
                        {"the player's data's global", reinterpret_cast<uintptr_t>(o.player)},
                        {"the own offer's offset", o.own},
                        {"the partner's offer's offset", o.theirs},
                        {"the inventory's records' offset", o.inventory},
                        {"the last sends' offset", o.last_send},
                        {"the intervals' global", reinterpret_cast<uintptr_t>(o.intervals)},
                        {"the intervals' offset", o.interval},
                        {"the packet ids the throttle takes", o.packets},
                        {"the clock", reinterpret_cast<uintptr_t>(throttle_clock)},
                        {"the clock's object's global", reinterpret_cast<uintptr_t>(o.clock_object)},
                        {"the item record", reinterpret_cast<uintptr_t>(o.item)},
                        {"inventor_rows", reinterpret_cast<uintptr_t>(o.rows)},
                        {"the item cache's global", reinterpret_cast<uintptr_t>(o.cache)},
                        {"the item's data", reinterpret_cast<uintptr_t>(o.item_data)},
                        {"the item's description", reinterpret_cast<uintptr_t>(o.describe)},
                        {"the item data's lock byte", o.lock},
                        {"the mode listing the modal statuses", reinterpret_cast<uintptr_t>(o.status_mode)},
                        {"the titled name's length", o.actor_titled},
                        {"the titled name", o.actor_title},
                        {"the name", o.actor_name},
                    };
                    const int first = at + count + doors + dialogues + texts + 2;
                    for (int i = 0; i < static_cast<int>(sizeof(offer) / sizeof(offer[0])); ++i) {
                        expect(offer[i].name, offer[i].got, argc, argv, first + i);
                    }
                    bool rule = true;
                    for (int k = 0; k < kStatusClasses; ++k) {
                        const uint8_t want = k == 0 || k == 5 || k == 0x1B ? kStatusListed
                            : k == 0x13 ? kStatusModal : 2;
                        rule = rule && o.status_class[k] == want;
                    }
                    check(rule, "the inventory list's status rule: 0, 5 and 0x1B listed, 0x13 for one mode alone,"
                        " every other status gray");
                    // The openers open_set and a list's open run:
                    // resolve_openers() as the engine runs it, the magic
                    // opener's manager pinned into its signature.
                    OpenerSites op;
                    const bool openers = resolve_openers(in, &op, why, sizeof(why));
                    snprintf(label, sizeof(label), "the openers resolve: equip_open, magic_open, map_open,"
                        " set_spells_open, attachments_open, linkshell_row, instincts_open, item_use and"
                        " macro_edit%s%s", openers ? "" : ": ", why);
                    check(openers, label);
                    if (openers) {
                        const void* fns[14];
                        memcpy(&fns[0], &op.equipment, 4);
                        memcpy(&fns[1], &op.monstrosity, 4);
                        memcpy(&fns[2], &op.magic, 4);
                        memcpy(&fns[3], &op.map, 4);
                        memcpy(&fns[4], &op.set_spells, 4);
                        memcpy(&fns[5], &op.attachments, 4);
                        memcpy(&fns[6], &op.linkshell_check, 4);
                        memcpy(&fns[7], &op.linkshell, 4);
                        memcpy(&fns[8], &op.instincts, 4);
                        memcpy(&fns[9], &op.item_data, 4);
                        memcpy(&fns[10], &op.use_check, 4);
                        memcpy(&fns[11], &op.synthesis, 4);
                        memcpy(&fns[12], &op.tabula, 4);
                        memcpy(&fns[13], &op.macros, 4);
                        const struct {
                            const char* name;
                            uintptr_t got;
                        } opener[] = {
                            {"equip_open", reinterpret_cast<uintptr_t>(fns[0])},
                            {"the Monstrosity check", reinterpret_cast<uintptr_t>(fns[1])},
                            {"equip_shortcut", reinterpret_cast<uintptr_t>(op.shortcut)},
                            {"magic_open", reinterpret_cast<uintptr_t>(fns[2])},
                            {"map_open", reinterpret_cast<uintptr_t>(fns[3])},
                            {"set_spells_open", reinterpret_cast<uintptr_t>(fns[4])},
                            {"attachments_open", reinterpret_cast<uintptr_t>(fns[5])},
                            {"linkshell_row", reinterpret_cast<uintptr_t>(op.linkshell_row)},
                            {"the linkshell row's check", reinterpret_cast<uintptr_t>(fns[6])},
                            {"the linkshell list's opener", reinterpret_cast<uintptr_t>(fns[7])},
                            {"instincts_open", reinterpret_cast<uintptr_t>(fns[8])},
                            {"item_use", reinterpret_cast<uintptr_t>(op.item_use)},
                            {"the item cache's global, as item_use reads it", reinterpret_cast<uintptr_t>(op.item_cache)},
                            {"the item's data, as item_use reads it", reinterpret_cast<uintptr_t>(fns[9])},
                            {"the item data's pointer to its type", op.item_type},
                            {"the item use's check", reinterpret_cast<uintptr_t>(fns[10])},
                            {"the synthesis's opener", reinterpret_cast<uintptr_t>(fns[11])},
                            {"inventor's list, as the synthesis's opener keeps it", op.inventor_list},
                            {"the maze editor's opener", reinterpret_cast<uintptr_t>(fns[12])},
                            {"macro_edit", reinterpret_cast<uintptr_t>(op.macro_edit)},
                            {"the macro editor's global", reinterpret_cast<uintptr_t>(op.macro_editor)},
                            {"the macro editor's opener", reinterpret_cast<uintptr_t>(fns[13])},
                        };
                        const int next = first + static_cast<int>(sizeof(offer) / sizeof(offer[0]));
                        for (int i = 0; i < static_cast<int>(sizeof(opener) / sizeof(opener[0])); ++i) {
                            expect(opener[i].name, opener[i].got, argc, argv, next + i);
                        }
                        // The routes open(name, arg) and the later sets
                        // run: resolve_routes() as the engine runs it.
                        RouteSites rt;
                        const uint8_t* yesno_ask = NULL;
                        if (dead_site) {
                            int32_t ask_rel;
                            memcpy(&ask_rel, dead_site + kDeadAskCall + 1, 4);
                            yesno_ask = dead_site + kDeadAskCall + 5 + ask_rel;
                        }
                        const bool routes = openers && resolve_routes(in, o, op.item_type, close_instance, yesno_ask,
                            &rt, why, sizeof(why));
                        snprintf(label, sizeof(label), "the routes resolve%s%s", routes ? "" : ": ", why);
                        check(routes, label);
                        if (routes) {
                            const void* f[32];
                            memcpy(&f[0], &rt.storage, 4);
                            memcpy(&f[1], &rt.mogpost, 4);
                            memcpy(&f[2], &rt.language_key, 4);
                            memcpy(&f[3], &rt.friend_list, 4);
                            memcpy(&f[4], &rt.msglist, 4);
                            memcpy(&f[5], &rt.blklist, 4);
                            memcpy(&f[6], &rt.mount, 4);
                            memcpy(&f[7], &rt.job_points_test, 4);
                            memcpy(&f[8], &rt.job_points, 4);
                            memcpy(&f[9], &rt.fp_cat2, 4);
                            memcpy(&f[10], &rt.search_reset, 4);
                            memcpy(&f[11], &rt.comgenre, 4);
                            memcpy(&f[12], &rt.comment_sub[0], 4);
                            memcpy(&f[13], &rt.comment_sub[1], 4);
                            memcpy(&f[14], &rt.comment_sub[2], 4);
                            memcpy(&f[15], &rt.item_panel, 4);
                            memcpy(&f[16], &rt.timer[0], 4);
                            memcpy(&f[17], &rt.timer[1], 4);
                            memcpy(&f[18], &rt.mp_stat, 4);
                            memcpy(&f[19], &rt.quest00_dim[0], 4);
                            memcpy(&f[20], &rt.quest00_dim[1], 4);
                            memcpy(&f[21], &rt.quest01, 4);
                            memcpy(&f[22], &rt.miss00_menu, 4);
                            memcpy(&f[23], &rt.expansion, 4);
                            memcpy(&f[24], &rt.tracking, 4);
                            memcpy(&f[25], &rt.scanlist, 4);
                            memcpy(&f[26], &rt.region, 4);
                            memcpy(&f[27], &rt.macro_pick, 4);
                            memcpy(&f[28], &rt.macro_close, 4);
                            memcpy(&f[29], &rt.title, 4);
                            memcpy(&f[30], &rt.items_view, 4);
                            memcpy(&f[31], &rt.inventor_open, 4);
                            const void* sw;
                            const void* bag;
                            memcpy(&sw, &rt.macro_switch, 4);
                            memcpy(&bag, &rt.bag_size, 4);
                            const struct {
                                const char* name;
                                uintptr_t got;
                            } route[] = {
                                {"storage_open", reinterpret_cast<uintptr_t>(f[0])},
                                {"mogpost_open", reinterpret_cast<uintptr_t>(f[1])},
                                {"the job list's key lookup", reinterpret_cast<uintptr_t>(f[2])},
                                {"jobcselu's row", rt.job_row},
                                {"the Mog House's manager's global", reinterpret_cast<uintptr_t>(rt.mog_house)},
                                {"the Nomad Moogle service's manager's global", reinterpret_cast<uintptr_t>(rt.nomad)},
                                {"the friend list's opener", reinterpret_cast<uintptr_t>(f[3])},
                                {"the mail list's opener", reinterpret_cast<uintptr_t>(f[4])},
                                {"the blacklist's opener", reinterpret_cast<uintptr_t>(f[5])},
                                {"mount_open", reinterpret_cast<uintptr_t>(f[6])},
                                {"job_points_test", reinterpret_cast<uintptr_t>(f[7])},
                                {"job_points_open", reinterpret_cast<uintptr_t>(f[8])},
                                {"fp_cat2_open", reinterpret_cast<uintptr_t>(f[9])},
                                {"the search reset", reinterpret_cast<uintptr_t>(f[10])},
                                {"the results' mode", rt.search_mode},
                                {"comgenre_open", reinterpret_cast<uintptr_t>(f[11])},
                                {"sccompar's opener", reinterpret_cast<uintptr_t>(f[12])},
                                {"sccomite's opener", reinterpret_cast<uintptr_t>(f[13])},
                                {"sccomls's opener", reinterpret_cast<uintptr_t>(f[14])},
                                {"the item panel's set-and-open", reinterpret_cast<uintptr_t>(f[15])},
                                {"an item record's extra data", rt.item_extra},
                                {"casttime's opener", reinterpret_cast<uintptr_t>(f[16])},
                                {"eventtim's opener", reinterpret_cast<uintptr_t>(f[17])},
                                {"mp_stat's opener", reinterpret_cast<uintptr_t>(f[18])},
                                {"persona's layout", rt.persona_layout},
                                {"conf11l's set", rt.conf11l_set},
                                {"quest00's mode", rt.quest00_mode},
                                {"quest00's confirm's first call", reinterpret_cast<uintptr_t>(f[19])},
                                {"quest00's confirm's second call", reinterpret_cast<uintptr_t>(f[20])},
                                {"quest01's setup", reinterpret_cast<uintptr_t>(f[21])},
                                {"the missions menu's setter", reinterpret_cast<uintptr_t>(f[22])},
                                {"the expansion test", reinterpret_cast<uintptr_t>(f[23])},
                                {"the logs' expansion bits", reinterpret_cast<uintptr_t>(rt.mission_bits)},
                                {"miss00's log", rt.miss00_log},
                                {"the map's tracking", reinterpret_cast<uintptr_t>(f[24])},
                                {"scanlist's opener", reinterpret_cast<uintptr_t>(f[25])},
                                {"the player data's global", reinterpret_cast<uintptr_t>(rt.player)},
                                {"the player's area", rt.area},
                                {"the Region Info opener", reinterpret_cast<uintptr_t>(f[26])},
                                {"cnqframe's kind", rt.region_kind},
                                {"coloresu's region", rt.coloresu_region},
                                {"textcol1's category", rt.textcol[0]},
                                {"textcol2's category", rt.textcol[1]},
                                {"textcol3's category", rt.textcol[2]},
                                {"macro_pick", reinterpret_cast<uintptr_t>(f[27])},
                                {"the macro editor's palette", rt.macro_palette},
                                {"the macro editor's close", reinterpret_cast<uintptr_t>(f[28])},
                                {"titlewin's step back", reinterpret_cast<uintptr_t>(f[29])},
                                {"the item view", reinterpret_cast<uintptr_t>(f[30])},
                                {"the item view's global", reinterpret_cast<uintptr_t>(rt.items_object)},
                                {"inventor's Items byte", rt.inventor_items},
                                {"the Items row's list's global", reinterpret_cast<uintptr_t>(rt.inventor_policy)},
                                {"inventor's open with a list", reinterpret_cast<uintptr_t>(f[31])},
                                {"the item summary's type", rt.summary_type},
                                {"the palette switch", reinterpret_cast<uintptr_t>(sw)},
                                {"the Wide Scan question's callback", reinterpret_cast<uintptr_t>(rt.scan_question)},
                                {"statcom2's begin-close", reinterpret_cast<uintptr_t>(rt.status_close)},
                                {"textcol1's Escape", reinterpret_cast<uintptr_t>(rt.textcol_escape)},
                                {"the bag's size", reinterpret_cast<uintptr_t>(bag)},
                                {"mp_mncst's Escape", reinterpret_cast<uintptr_t>(rt.instincts_escape)},
                                {"cnqframe's input", reinterpret_cast<uintptr_t>(rt.region_escape)},
                            };
                            const int after = next + static_cast<int>(sizeof(opener) / sizeof(opener[0]));
                            for (int i = 0; i < static_cast<int>(sizeof(route) / sizeof(route[0])); ++i) {
                                if (after + i >= argc) {
                                    std::printf("        %s %08X\n", route[i].name,
                                        static_cast<unsigned>(route[i].got));
                                }
                                expect(route[i].name, route[i].got, argc, argv, after + i);
                            }
                            // The settings: resolve_settings() as the engine
                            // runs it, every row of kSettings checked
                            // against the game's tables.
                            SettingSites st;
                            const bool settings = resolve_settings(in, a.bank_data, &st, why, sizeof(why));
                            snprintf(label, sizeof(label), "the settings resolve: each signature once, the store's"
                                " layout, the pages' routines and every row of the game's tables%s%s",
                                settings ? "" : ": ", why);
                            check(settings, label);
                            if (settings) {
                                const void* s[5];
                                memcpy(&s[0], &st.set, 4);
                                memcpy(&s[1], &st.lock, 4);
                                memcpy(&s[2], &st.idle, 4);
                                memcpy(&s[3], &st.flag, 4);
                                memcpy(&s[4], &st.lock_clock, 4);
                                const struct {
                                    const char* name;
                                    uintptr_t got;
                                } setting[] = {
                                    {"cfg_set", reinterpret_cast<uintptr_t>(s[0])},
                                    {"the config store's global", reinterpret_cast<uintptr_t>(st.store)},
                                    {"the store's set method", reinterpret_cast<uintptr_t>(st.method)},
                                    {"cfg_save", reinterpret_cast<uintptr_t>(st.save)},
                                    {"the chat filter's rows", reinterpret_cast<uintptr_t>(st.filter_rows)},
                                    {"the battle effects filter's rows", reinterpret_cast<uintptr_t>(st.effect_rows)},
                                    {"the chat log routing's group 1", reinterpret_cast<uintptr_t>(st.routing[0])},
                                    {"the chat log routing's group 2", reinterpret_cast<uintptr_t>(st.routing[1])},
                                    {"the chat log routing's group 3", reinterpret_cast<uintptr_t>(st.routing[2])},
                                    {"the chat log routing's group 4", reinterpret_cast<uintptr_t>(st.routing[3])},
                                    {"the text colors' category 1", reinterpret_cast<uintptr_t>(st.colors[0])},
                                    {"the text colors' category 2", reinterpret_cast<uintptr_t>(st.colors[1])},
                                    {"the text colors' category 3", reinterpret_cast<uintptr_t>(st.colors[2])},
                                    {"the text colors' category 4", reinterpret_cast<uintptr_t>(st.colors[3])},
                                    {"k1assign's rows", reinterpret_cast<uintptr_t>(st.keys[0])},
                                    {"k2assign's rows", reinterpret_cast<uintptr_t>(st.keys[1])},
                                    {"the equipment lock's request", reinterpret_cast<uintptr_t>(s[1])},
                                    {"the lock's wait's clock", reinterpret_cast<uintptr_t>(s[4])},
                                    {"the lock's stamp", reinterpret_cast<uintptr_t>(st.lock_stamp)},
                                    {"the lock's wait in milliseconds", st.lock_wait},
                                    {"the idle timer", reinterpret_cast<uintptr_t>(s[2])},
                                    {"the player flag's request", reinterpret_cast<uintptr_t>(s[3])},
                                    {"the player flags' data's global", reinterpret_cast<uintptr_t>(st.player)},
                                    {"the player flags' offset", st.flags},
                                };
                                const int later = after + static_cast<int>(sizeof(route) / sizeof(route[0]));
                                for (int i = 0; i < static_cast<int>(sizeof(setting) / sizeof(setting[0])); ++i) {
                                    if (later + i >= argc) {
                                        std::printf("        %s %08X\n", setting[i].name,
                                            static_cast<unsigned>(setting[i].got));
                                    }
                                    expect(setting[i].name, setting[i].got, argc, argv, later + i);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    if (registry) {
        const uint8_t* table = rdptr(registry, 1);
        Inventory inv;
        inv.build();
        static const char* const replies[] = {
            "query", "passinpu", "prtyjoin", "link5", "arealist", "scsibori", "delivery", "post1", "post2",
            "partywin", "playermo", "mp_pmode", "iteminfo", "itemxinf", "comyn", "merityn", "resyn", "resynus",
            "resynde", "resynfr", "shopbuy", "shopsell", "holdtime", "dead", "moneyctr", "itemctrl", "trade",
            "handover", "auc1", "shop", "bank", "tskill1", "sortyn", "evitem01", "evitem", "confyn", "bankmenu",
            "maplist", "gmtell", "mp_dead", "ok", "mogdoor", "rem4line", "rem4li2", "stringdl", "comment",
            "msgline", "scname", "link11", "link6",
        };
        bool all = true;
        for (size_t i = 0; i < sizeof(replies) / sizeof(replies[0]); ++i) {
            const int n = inv.find_exact(replies[i]);
            all = all && n >= 0 && rd32(table + inv.names[n].rows[0] * kRowStride, kRowSlot) != 0;
        }
        check(all, "every menu a reply call or resize reaches has a controller slot in its registry row");
        static const char* const resyn[] = {"resyn", "resynus", "resynde", "resynfr"};
        uint32_t slots[4];
        for (int i = 0; i < 4; ++i) {
            const int n = inv.find_exact(resyn[i]);
            slots[i] = n >= 0 ? rd32(table + inv.names[n].rows[0] * kRowStride, kRowSlot) : 0;
        }
        check(slots[0] != 0 && slots[0] == slots[1] && slots[0] == slots[2] && slots[0] == slots[3],
            "resyn, resynus, resynde and resynfr share one controller slot");
    }

    std::printf("%d of %d checks failed\n", g_failures, g_checks);
    return g_failures;
}
