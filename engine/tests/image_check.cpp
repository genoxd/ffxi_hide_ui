// image_check - the engine's resolution against an unpacked FFXiMain.dll on
// disk: every signature must match exactly once, land at the address given
// for it, when one is given, and the menu table must
// verify. The image is copied section by section to its preferred base and
// never run: no loader, no DllMain, no TLS callbacks.
//
// image_check.exe <unpacked FFXiMain.dll> [registry mcb masks set_position close open update staged show
//                  mouse_mode menu_input compass_draw macro_gate
//                  the eleven called routines in signatures.h's kCalls order
//                  link5_cache_site link5_cache query_cancel_site query_cancel_allowed
//                  set_cursor link5_latch_site link5_latch link5_open_site link5_global
//                  link5_callback glyph_convert row_hit_test mouse_read menu_routing
//                  compass_global macro_object_global]
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
        sites[i] = find(text, size, kSites[i].name, kSites[i].signature);
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
    for (int i = 0; i < kCallCount; ++i) {
        if (calls[i]) {
            expect(kCalls[i].name, reinterpret_cast<uintptr_t>(calls[i]), argc, argv, 7 + kSiteCount + i);
        }
    }
    if (calls[kCallPartyClear]) {
        const uint8_t* flag = rdptr(calls[kCallPartyClear], kPartyClearFlag);
        char label[128];
        snprintf(label, sizeof(label), "prtyjoin pending flag 0x%08X is data in the image, outside .text",
            static_cast<unsigned>(reinterpret_cast<uintptr_t>(flag)));
        check(flag >= image && flag < image + image_size && !(flag >= text && flag < text + size), label);
    }
    const int data_arg = 7 + kSiteCount + kCallCount;
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
    if (registry) {
        const uint8_t* table = rdptr(registry, 1);
        Inventory inv;
        inv.build();
        static const char* const replies[] = {
            "query", "passinpu", "prtyjoin", "link5", "arealist", "scsibori", "delivery", "post1", "post2",
            "partywin", "playermo", "mp_pmode", "iteminfo", "itemxinf",
        };
        bool all = true;
        for (size_t i = 0; i < sizeof(replies) / sizeof(replies[0]); ++i) {
            const int n = inv.find_exact(replies[i]);
            all = all && n >= 0 && rd32(table + inv.names[n].rows[0] * kRowStride, kRowSlot) != 0;
        }
        check(all, "every menu a reply call or resize reaches has a controller slot in its registry row");
    }

    std::printf("%d of %d checks failed\n", g_failures, g_checks);
    return g_failures;
}
