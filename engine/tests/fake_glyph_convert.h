// fake_glyph_convert.h - a stand-in for FFXiMain's text-to-glyph converter
// (0x10128500 in the 2026-09-10 build, 0x10128390 in 2026-05-10), for
// engine_test and the stand-in FFXiMain.dll.
//
// The converter's shape: at most max_glyphs - 1 codes, then a 0, the count
// returned; a byte the game's lead test takes (0x81..0x9F, 0xE0..0xEF,
// 0xFA..0xFC) and the byte after it are one two-byte character; 1E nn and
// 1F nn are colors; any other byte is itself less 0x20, signed. No 7F
// escapes and no raw output.
//
// Two-byte characters as FUN_1011C950 maps them, for three groups only: the
// 0x81 row (JIS rows 0x21 and 0x22, every branch; 81 40 is 0x60, and the
// unassigned 81 AD..81 B7 yield the glyphs of the assigned 81 B8.., as the
// game's do), the ED, EE and EF rows (EF 1F..EF 4D are the gaiji
// 0x2115..0x2143, and EE DC..EE FC yield 0x2115..0x2135 too, as the game's
// do), and 0xCB for every code past JIS 0x931F (FA..FC). Any other two-byte
// character comes back as -0x7F, a code the game does not draw.

#ifndef HIDEUI_FAKE_GLYPH_CONVERT_H_
#define HIDEUI_FAKE_GLYPH_CONVERT_H_

#include <stdint.h>

inline bool fake_two_byte_lead(uint8_t c) {
    return (c > 0x80 && c < 0xA0) || (c > 0xDF && c < 0xF0) || (c > 0xF9 && c < 0xFD);
}

inline int16_t fake_two_byte_glyph(uint8_t lead, uint8_t trail) {
    int row = ((lead < 0xA0 ? lead - 0x71 : lead - 0xB1) * 2) | 1;
    int col = trail > 0x7F ? trail - 1 : trail;
    if (col >= 0x9E) {
        ++row;
        col -= 0x7D;
    } else {
        col -= 0x1F;
    }
    const int jis = row << 8 | col;
    if (jis >= 0x9320) {
        return 0xCB;
    }
    if (jis >= 0x7921) {
        return static_cast<int16_t>(col + 94 * row - 0xCD1);
    }
    if (jis >= 0x2330) {
        return -0x7F;
    }
    if (jis == 0x227F) {
        return 0xF2;
    }
    if (jis >= 0x2272) {
        return static_cast<int16_t>(col + row + 0x56);
    }
    if (jis >= 0x225C) {
        return static_cast<int16_t>(col + row + 0x5D);
    }
    if (jis >= 0x224A) {
        return static_cast<int16_t>(col + row + 0x68);
    }
    if (jis >= 0x223A) {
        return static_cast<int16_t>(col + row + 0x70);
    }
    return static_cast<int16_t>(col + 94 * row - 0xBDF);
}

inline uint32_t fake_glyph_convert(const char* text, int16_t* glyphs, uint32_t max_glyphs, char* raw, int) {
    if (!glyphs || !max_glyphs) {
        return 0;
    }
    uint32_t n = 0;
    while (n + 1 < max_glyphs && *text) {
        const uint8_t c = static_cast<uint8_t>(*text);
        if (c == 0x1E || c == 0x1F) {
            glyphs[n] = static_cast<int16_t>((c == 0x1E ? -0x100 : -0x200) - static_cast<uint8_t>(text[1]));
            text += 2;
        } else if (fake_two_byte_lead(c)) {
            glyphs[n] = fake_two_byte_glyph(c, static_cast<uint8_t>(text[1]));
            text += 2;
        } else {
            glyphs[n] = static_cast<int16_t>(static_cast<int8_t>(c) - 0x20);
            ++text;
        }
        ++n;
    }
    if (raw) {
        *raw = '\0';
    }
    glyphs[n] = 0;
    return n;
}

#endif  // HIDEUI_FAKE_GLYPH_CONVERT_H_
