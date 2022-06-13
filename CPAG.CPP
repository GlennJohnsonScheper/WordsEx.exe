// This is file: CPag.cpp
// Copyright ( C ) 2006, Glenn Scheper

#include "stdafx.h"
#include "CAll.h" // Globals

// a CTxt is created to parse one web page, but CPag Pag is forever.

    // Notes:
    // I get re-confused about codepages.
    // But I already converted the standard 1250 series into
    // wchar_t of the UCS-16 encoding, called code page 1200.
    // Therefore, attempt to set that code page for whole program.
    //
    // wchar_t *_wsetlocale( int category, const wchar_t *locale );
    // Returns exact string if okay; zero if call failed.
    //
    // Setting codepage 1200 failed, try 1252.
    //
    // Yippie!
    // Doing _wsetlocale( LC_CTYPE, L".1252" ) fixed iswalpha,
    // which had been excluding chars in 0x80-0xff, like this
    // c-cedila, or the n-tilde, which iswalpha is now working!
    // "49999fran\347ais49999espa\361ol49999enciclopedia49999deutsch49999bahasa49999articles24999\355slenska24999zoeken24999ze\352uws24999zbek"
    //
    // Applications may also mislabel text in Windows-1252 as ISO-8859-1,
    // the default character set for HTML. Fortunately the only difference
    // between these code pages is that the range ISO-8859-1 reserves for
    // control characters, Windows-1252 uses for additional printable
    // characters. Since control characters have no function in html...

wchar_t CP1200TOP [128] = { // this one is an identity mapping for UTF-8
0x0080,0x0081,0x0082,0x0083,0x0084,0x0085,0x0086,0x0087,0x0088,0x0089,0x008a,0x008b,0x008c,0x008d,0x008e,0x008f, // 0x80 - 0x8f
0x0090,0x0091,0x0092,0x0093,0x0094,0x0095,0x0096,0x0097,0x0098,0x0099,0x009a,0x009b,0x009c,0x009d,0x009e,0x009f, // 0x90 - 0x9f
0x00a0,0x00a1,0x00a2,0x00a3,0x00a4,0x00a5,0x00a6,0x00a7,0x00a8,0x00a9,0x00aa,0x00ab,0x00ac,0x00ad,0x00ae,0x00af, // 0xa0 - 0xaf
0x00b0,0x00b1,0x00b2,0x00b3,0x00b4,0x00b5,0x00b6,0x00b7,0x00b8,0x00b9,0x00ba,0x00bb,0x00bc,0x00bd,0x00be,0x00bf, // 0xb0 - 0xbf
0x00c0,0x00c1,0x00c2,0x00c3,0x00c4,0x00c5,0x00c6,0x00c7,0x00c8,0x00c9,0x00ca,0x00cb,0x00cc,0x00cd,0x00ce,0x00cf, // 0xc0 - 0xcf
0x00d0,0x00d1,0x00d2,0x00d3,0x00d4,0x00d5,0x00d6,0x00d7,0x00d8,0x00d9,0x00da,0x00db,0x00dc,0x00dd,0x00de,0x00df, // 0xd0 - 0xdf
0x00e0,0x00e1,0x00e2,0x00e3,0x00e4,0x00e5,0x00e6,0x00e7,0x00e8,0x00e9,0x00ea,0x00eb,0x00ec,0x00ed,0x00ee,0x00ef, // 0xe0 - 0xef
0x00f0,0x00f1,0x00f2,0x00f3,0x00f4,0x00f5,0x00f6,0x00f7,0x00f8,0x00f9,0x00fa,0x00fb,0x00fc,0x00fd,0x00fe,0x00ff, // 0xf0 - 0xff
}; // end of CP1200TOP

wchar_t CP1250TOP [128] = {
0x20ac, 0x0000, U_201A, 0x0000, U_201E, 0x2026, 0x2020, 0x2021, // 0x80 - 0x87
0x0000, 0x2030, 0x0160, 0x2039, 0x015a, 0x0164, 0x017d, 0x0179, // 0x88 - 0x8f
0x0000, U_2018, U_2019, U_201C, U_201D, 0x2022, U_2013, U_2014, // 0x90 - 0x97
0x0000, 0x2122, 0x0161, 0x203a, 0x015b, 0x0165, 0x017e, 0x017a, // 0x98 - 0x9f
0x00a0, 0x02c7, 0x02d8, 0x0141, 0x00a4, 0x0104, 0x00a6, 0x00a7, // 0xa0 - 0xa7
0x00a8, 0x00a9, 0x015e, 0x00ab, 0x00ac, 0x00ad, 0x00ae, 0x017b, // 0xa8 - 0xaf
0x00b0, 0x00b1, 0x02db, 0x0142, 0x00b4, 0x00b5, 0x00b6, 0x00b7, // 0xb0 - 0xb7
0x00b8, 0x0105, 0x015f, 0x00bb, 0x013d, 0x02dd, 0x013e, 0x017c, // 0xb8 - 0xbf
0x0154, 0x00c1, 0x00c2, 0x0102, 0x00c4, 0x0139, 0x0106, 0x00c7, // 0xc0 - 0xc7
0x010c, 0x00c9, 0x0118, 0x00cb, 0x011a, 0x00cd, 0x00ce, 0x010e, // 0xc8 - 0xcf
0x0110, 0x0143, 0x0147, 0x00d3, 0x00d4, 0x0150, 0x00d6, 0x00d7, // 0xd0 - 0xd7
0x0158, 0x016e, 0x00da, 0x0170, 0x00dc, 0x00dd, 0x0162, 0x00df, // 0xd8 - 0xdf
0x0155, 0x00e1, 0x00e2, 0x0103, 0x00e4, 0x013a, 0x0107, 0x00e7, // 0xe0 - 0xe7
0x010d, 0x00e9, 0x0119, 0x00eb, 0x011b, 0x00ed, 0x00ee, 0x010f, // 0xe8 - 0xef
0x0111, 0x0144, 0x0148, 0x00f3, 0x00f4, 0x0151, 0x00f6, 0x00f7, // 0xf0 - 0xf7
0x0159, 0x016f, 0x00fa, 0x0171, 0x00fc, 0x00fd, 0x0163, 0x02d9, // 0xf8 - 0xff
}; // end of CP1250TOP

wchar_t CP1251TOP [128] = {
0x0402, 0x0403, U_201A, 0x0453, U_201E, 0x2026, 0x2020, 0x2021, // 0x80 - 0x87
0x20ac, 0x2030, 0x0409, 0x2039, 0x040a, 0x040c, 0x040b, 0x040f, // 0x88 - 0x8f
0x0452, U_2018, U_2019, U_201C, U_201D, 0x2022, U_2013, U_2014, // 0x90 - 0x97
0x0000, 0x2122, 0x0459, 0x203a, 0x045a, 0x045c, 0x045b, 0x045f, // 0x98 - 0x9f
0x00a0, 0x040e, 0x045e, 0x0408, 0x00a4, 0x0490, 0x00a6, 0x00a7, // 0xa0 - 0xa7
0x0401, 0x00a9, 0x0404, 0x00ab, 0x00ac, 0x00ad, 0x00ae, 0x0407, // 0xa8 - 0xaf
0x00b0, 0x00b1, 0x0406, 0x0456, 0x0491, 0x00b5, 0x00b6, 0x00b7, // 0xb0 - 0xb7
0x0451, 0x2116, 0x0454, 0x00bb, 0x0458, 0x0405, 0x0455, 0x0457, // 0xb8 - 0xbf
0x0410, 0x0411, 0x0412, 0x0413, 0x0414, 0x0415, 0x0416, 0x0417, // 0xc0 - 0xc7
0x0418, 0x0419, 0x041a, 0x041b, 0x041c, 0x041d, 0x041e, 0x041f, // 0xc8 - 0xcf
0x0420, 0x0421, 0x0422, 0x0423, 0x0424, 0x0425, 0x0426, 0x0427, // 0xd0 - 0xd7
0x0428, 0x0429, 0x042a, 0x042b, 0x042c, 0x042d, 0x042e, 0x042f, // 0xd8 - 0xdf
0x0430, 0x0431, 0x0432, 0x0433, 0x0434, 0x0435, 0x0436, 0x0437, // 0xe0 - 0xe7
0x0438, 0x0439, 0x043a, 0x043b, 0x043c, 0x043d, 0x043e, 0x043f, // 0xe8 - 0xef
0x0440, 0x0441, 0x0442, 0x0443, 0x0444, 0x0445, 0x0446, 0x0447, // 0xf0 - 0xf7
0x0448, 0x0449, 0x044a, 0x044b, 0x044c, 0x044d, 0x044e, 0x044f, // 0xf8 - 0xff
}; // end of CP1251TOP

wchar_t CP1252TOP [128] = {
0x20ac, 0x0000, U_201A, 0x0192, U_201E, 0x2026, 0x2020, 0x2021, // 0x80 - 0x87
0x02c6, 0x2030, 0x0160, 0x2039, 0x0152, 0x0000, 0x017d, 0x0000, // 0x88 - 0x8f
0x0000, U_2018, U_2019, U_201C, U_201D, 0x2022, U_2013, U_2014, // 0x90 - 0x97
0x02dc, 0x2122, 0x0161, 0x203a, 0x0153, 0x0000, 0x017e, 0x0178, // 0x98 - 0x9f
0x00a0, 0x00a1, 0x00a2, 0x00a3, 0x00a4, 0x00a5, 0x00a6, 0x00a7, // 0xa0 - 0xa7
0x00a8, 0x00a9, 0x00aa, 0x00ab, 0x00ac, 0x00ad, 0x00ae, 0x00af, // 0xa8 - 0xaf
0x00b0, 0x00b1, 0x00b2, 0x00b3, 0x00b4, 0x00b5, 0x00b6, 0x00b7, // 0xb0 - 0xb7
0x00b8, 0x00b9, 0x00ba, 0x00bb, 0x00bc, 0x00bd, 0x00be, 0x00bf, // 0xb8 - 0xbf
0x00c0, 0x00c1, 0x00c2, 0x00c3, 0x00c4, 0x00c5, 0x00c6, 0x00c7, // 0xc0 - 0xc7
0x00c8, 0x00c9, 0x00ca, 0x00cb, 0x00cc, 0x00cd, 0x00ce, 0x00cf, // 0xc8 - 0xcf
0x00d0, 0x00d1, 0x00d2, 0x00d3, 0x00d4, 0x00d5, 0x00d6, 0x00d7, // 0xd0 - 0xd7
0x00d8, 0x00d9, 0x00da, 0x00db, 0x00dc, 0x00dd, 0x00de, 0x00df, // 0xd8 - 0xdf
0x00e0, 0x00e1, 0x00e2, 0x00e3, 0x00e4, 0x00e5, 0x00e6, 0x00e7, // 0xe0 - 0xe7
0x00e8, 0x00e9, 0x00ea, 0x00eb, 0x00ec, 0x00ed, 0x00ee, 0x00ef, // 0xe8 - 0xef
0x00f0, 0x00f1, 0x00f2, 0x00f3, 0x00f4, 0x00f5, 0x00f6, 0x00f7, // 0xf0 - 0xf7
0x00f8, 0x00f9, 0x00fa, 0x00fb, 0x00fc, 0x00fd, 0x00fe, 0x00ff, // 0xf8 - 0xff
}; // end of CP1252TOP

wchar_t CP1253TOP [128] = {
0x20ac, 0x0000, U_201A, 0x0192, U_201E, 0x2026, 0x2020, 0x2021, // 0x80 - 0x87
0x0000, 0x2030, 0x0000, 0x2039, 0x0000, 0x0000, 0x0000, 0x0000, // 0x88 - 0x8f
0x0000, U_2018, U_2019, U_201C, U_201D, 0x2022, U_2013, U_2014, // 0x90 - 0x97
0x0000, 0x2122, 0x0000, 0x203a, 0x0000, 0x0000, 0x0000, 0x0000, // 0x98 - 0x9f
0x00a0, 0x0385, 0x0386, 0x00a3, 0x00a4, 0x00a5, 0x00a6, 0x00a7, // 0xa0 - 0xa7
0x00a8, 0x00a9, 0x0000, 0x00ab, 0x00ac, 0x00ad, 0x00ae, U_2015, // 0xa8 - 0xaf
0x00b0, 0x00b1, 0x00b2, 0x00b3, 0x0384, 0x00b5, 0x00b6, 0x00b7, // 0xb0 - 0xb7
0x0388, 0x0389, 0x038a, 0x00bb, 0x038c, 0x00bd, 0x038e, 0x038f, // 0xb8 - 0xbf
0x0390, 0x0391, 0x0392, 0x0393, 0x0394, 0x0395, 0x0396, 0x0397, // 0xc0 - 0xc7
0x0398, 0x0399, 0x039a, 0x039b, 0x039c, 0x039d, 0x039e, 0x039f, // 0xc8 - 0xcf
0x03a0, 0x03a1, 0x0000, 0x03a3, 0x03a4, 0x03a5, 0x03a6, 0x03a7, // 0xd0 - 0xd7
0x03a8, 0x03a9, 0x03aa, 0x03ab, 0x03ac, 0x03ad, 0x03ae, 0x03af, // 0xd8 - 0xdf
0x03b0, 0x03b1, 0x03b2, 0x03b3, 0x03b4, 0x03b5, 0x03b6, 0x03b7, // 0xe0 - 0xe7
0x03b8, 0x03b9, 0x03ba, 0x03bb, 0x03bc, 0x03bd, 0x03be, 0x03bf, // 0xe8 - 0xef
0x03c0, 0x03c1, 0x03c2, 0x03c3, 0x03c4, 0x03c5, 0x03c6, 0x03c7, // 0xf0 - 0xf7
0x03c8, 0x03c9, 0x03ca, 0x03cb, 0x03cc, 0x03cd, 0x03ce, 0x0000, // 0xf8 - 0xff
}; // end of CP1253TOP

wchar_t CP1254TOP [128] = {
0x20ac, 0x0000, U_201A, 0x0192, U_201E, 0x2026, 0x2020, 0x2021, // 0x80 - 0x87
0x02c6, 0x2030, 0x0160, 0x2039, 0x0152, 0x0000, 0x0000, 0x0000, // 0x88 - 0x8f
0x0000, U_2018, U_2019, U_201C, U_201D, 0x2022, U_2013, U_2014, // 0x90 - 0x97
0x02dc, 0x2122, 0x0161, 0x203a, 0x0153, 0x0000, 0x0000, 0x0178, // 0x98 - 0x9f
0x00a0, 0x00a1, 0x00a2, 0x00a3, 0x00a4, 0x00a5, 0x00a6, 0x00a7, // 0xa0 - 0xa7
0x00a8, 0x00a9, 0x00aa, 0x00ab, 0x00ac, 0x00ad, 0x00ae, 0x00af, // 0xa8 - 0xaf
0x00b0, 0x00b1, 0x00b2, 0x00b3, 0x00b4, 0x00b5, 0x00b6, 0x00b7, // 0xb0 - 0xb7
0x00b8, 0x00b9, 0x00ba, 0x00bb, 0x00bc, 0x00bd, 0x00be, 0x00bf, // 0xb8 - 0xbf
0x00c0, 0x00c1, 0x00c2, 0x00c3, 0x00c4, 0x00c5, 0x00c6, 0x00c7, // 0xc0 - 0xc7
0x00c8, 0x00c9, 0x00ca, 0x00cb, 0x00cc, 0x00cd, 0x00ce, 0x00cf, // 0xc8 - 0xcf
0x011e, 0x00d1, 0x00d2, 0x00d3, 0x00d4, 0x00d5, 0x00d6, 0x00d7, // 0xd0 - 0xd7
0x00d8, 0x00d9, 0x00da, 0x00db, 0x00dc, 0x0130, 0x015e, 0x00df, // 0xd8 - 0xdf
0x00e0, 0x00e1, 0x00e2, 0x00e3, 0x00e4, 0x00e5, 0x00e6, 0x00e7, // 0xe0 - 0xe7
0x00e8, 0x00e9, 0x00ea, 0x00eb, 0x00ec, 0x00ed, 0x00ee, 0x00ef, // 0xe8 - 0xef
0x011f, 0x00f1, 0x00f2, 0x00f3, 0x00f4, 0x00f5, 0x00f6, 0x00f7, // 0xf0 - 0xf7
0x00f8, 0x00f9, 0x00fa, 0x00fb, 0x00fc, 0x0131, 0x015f, 0x00ff, // 0xf8 - 0xff
}; // end of CP1254TOP

wchar_t CP1255TOP [128] = {
0x20ac, 0x0000, U_201A, 0x0192, U_201E, 0x2026, 0x2020, 0x2021, // 0x80 - 0x87
0x02c6, 0x2030, 0x0000, 0x2039, 0x0000, 0x0000, 0x0000, 0x0000, // 0x88 - 0x8f
0x0000, U_2018, U_2019, U_201C, U_201D, 0x2022, U_2013, U_2014, // 0x90 - 0x97
0x02dc, 0x2122, 0x0000, 0x203a, 0x0000, 0x0000, 0x0000, 0x0000, // 0x98 - 0x9f
0x00a0, 0x00a1, 0x00a2, 0x00a3, 0x20aa, 0x00a5, 0x00a6, 0x00a7, // 0xa0 - 0xa7
0x00a8, 0x00a9, 0x00d7, 0x00ab, 0x00ac, 0x00ad, 0x00ae, 0x00af, // 0xa8 - 0xaf
0x00b0, 0x00b1, 0x00b2, 0x00b3, 0x00b4, 0x00b5, 0x00b6, 0x00b7, // 0xb0 - 0xb7
0x00b8, 0x00b9, 0x00f7, 0x00bb, 0x00bc, 0x00bd, 0x00be, 0x00bf, // 0xb8 - 0xbf
0x05b0, 0x05b1, 0x05b2, 0x05b3, 0x05b4, 0x05b5, 0x05b6, 0x05b7, // 0xc0 - 0xc7
0x05b8, 0x05b9, 0x0000, 0x05bb, 0x05bc, 0x05bd, 0x05be, 0x05bf, // 0xc8 - 0xcf
0x05c0, 0x05c1, 0x05c2, 0x05c3, 0x05f0, 0x05f1, 0x05f2, 0x05f3, // 0xd0 - 0xd7
0x05f4, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, // 0xd8 - 0xdf
0x05d0, 0x05d1, 0x05d2, 0x05d3, 0x05d4, 0x05d5, 0x05d6, 0x05d7, // 0xe0 - 0xe7
0x05d8, 0x05d9, 0x05da, 0x05db, 0x05dc, 0x05dd, 0x05de, 0x05df, // 0xe8 - 0xef
0x05e0, 0x05e1, 0x05e2, 0x05e3, 0x05e4, 0x05e5, 0x05e6, 0x05e7, // 0xf0 - 0xf7
0x05e8, 0x05e9, 0x05ea, 0x0000, 0x0000, 0x200e, 0x200f, 0x0000, // 0xf8 - 0xff
}; // end of CP1255TOP

wchar_t CP1256TOP [128] = {
0x20ac, 0x067e, U_201A, 0x0192, U_201E, 0x2026, 0x2020, 0x2021, // 0x80 - 0x87
0x02c6, 0x2030, 0x0679, 0x2039, 0x0152, 0x0686, 0x0698, 0x0688, // 0x88 - 0x8f
0x06af, U_2018, U_2019, U_201C, U_201D, 0x2022, U_2013, U_2014, // 0x90 - 0x97
0x06a9, 0x2122, 0x0691, 0x203a, 0x0153, 0x200c, 0x200d, 0x06ba, // 0x98 - 0x9f
0x00a0, 0x060c, 0x00a2, 0x00a3, 0x00a4, 0x00a5, 0x00a6, 0x00a7, // 0xa0 - 0xa7
0x00a8, 0x00a9, 0x06be, 0x00ab, 0x00ac, 0x00ad, 0x00ae, 0x00af, // 0xa8 - 0xaf
0x00b0, 0x00b1, 0x00b2, 0x00b3, 0x00b4, 0x00b5, 0x00b6, 0x00b7, // 0xb0 - 0xb7
0x00b8, 0x00b9, 0x061b, 0x00bb, 0x00bc, 0x00bd, 0x00be, 0x061f, // 0xb8 - 0xbf
0x06c1, 0x0621, 0x0622, 0x0623, 0x0624, 0x0625, 0x0626, 0x0627, // 0xc0 - 0xc7
0x0628, 0x0629, 0x062a, 0x062b, 0x062c, 0x062d, 0x062e, 0x062f, // 0xc8 - 0xcf
0x0630, 0x0631, 0x0632, 0x0633, 0x0634, 0x0635, 0x0636, 0x00d7, // 0xd0 - 0xd7
0x0637, 0x0638, 0x0639, 0x063a, 0x0640, 0x0641, 0x0642, 0x0643, // 0xd8 - 0xdf
0x00e0, 0x0644, 0x00e2, 0x0645, 0x0646, 0x0647, 0x0648, 0x00e7, // 0xe0 - 0xe7
0x00e8, 0x00e9, 0x00ea, 0x00eb, 0x0649, 0x064a, 0x00ee, 0x00ef, // 0xe8 - 0xef
0x064b, 0x064c, 0x064d, 0x064e, 0x00f4, 0x064f, 0x0650, 0x00f7, // 0xf0 - 0xf7
0x0651, 0x00f9, 0x0652, 0x00fb, 0x00fc, 0x200e, 0x200f, 0x06d2, // 0xf8 - 0xff
}; // end of CP1256TOP

wchar_t CP1257TOP [128] = {
0x20ac, 0x0000, U_201A, 0x0000, U_201E, 0x2026, 0x2020, 0x2021, // 0x80 - 0x87
0x0000, 0x2030, 0x0000, 0x2039, 0x0000, 0x00a8, 0x02c7, 0x00b8, // 0x88 - 0x8f
0x0000, U_2018, U_2019, U_201C, U_201D, 0x2022, U_2013, U_2014, // 0x90 - 0x97
0x0000, 0x2122, 0x0000, 0x203a, 0x0000, 0x00af, 0x02db, 0x0000, // 0x98 - 0x9f
0x00a0, 0x0000, 0x00a2, 0x00a3, 0x00a4, 0x0000, 0x00a6, 0x00a7, // 0xa0 - 0xa7
0x00d8, 0x00a9, 0x0156, 0x00ab, 0x00ac, 0x00ad, 0x00ae, 0x00c6, // 0xa8 - 0xaf
0x00b0, 0x00b1, 0x00b2, 0x00b3, 0x00b4, 0x00b5, 0x00b6, 0x00b7, // 0xb0 - 0xb7
0x00f8, 0x00b9, 0x0157, 0x00bb, 0x00bc, 0x00bd, 0x00be, 0x00e6, // 0xb8 - 0xbf
0x0104, 0x012e, 0x0100, 0x0106, 0x00c4, 0x00c5, 0x0118, 0x0112, // 0xc0 - 0xc7
0x010c, 0x00c9, 0x0179, 0x0116, 0x0122, 0x0136, 0x012a, 0x013b, // 0xc8 - 0xcf
0x0160, 0x0143, 0x0145, 0x00d3, 0x014c, 0x00d5, 0x00d6, 0x00d7, // 0xd0 - 0xd7
0x0172, 0x0141, 0x015a, 0x016a, 0x00dc, 0x017b, 0x017d, 0x00df, // 0xd8 - 0xdf
0x0105, 0x012f, 0x0101, 0x0107, 0x00e4, 0x00e5, 0x0119, 0x0113, // 0xe0 - 0xe7
0x010d, 0x00e9, 0x017a, 0x0117, 0x0123, 0x0137, 0x012b, 0x013c, // 0xe8 - 0xef
0x0161, 0x0144, 0x0146, 0x00f3, 0x014d, 0x00f5, 0x00f6, 0x00f7, // 0xf0 - 0xf7
0x0173, 0x0142, 0x015b, 0x016b, 0x00fc, 0x017c, 0x017e, 0x02d9, // 0xf8 - 0xff
}; // end of CP1257TOP

wchar_t CP1258TOP [128] = {
0x20ac, 0x0000, U_201A, 0x0192, U_201E, 0x2026, 0x2020, 0x2021, // 0x80 - 0x87
0x02c6, 0x2030, 0x0000, 0x2039, 0x0152, 0x0000, 0x0000, 0x0000, // 0x88 - 0x8f
0x0000, U_2018, U_2019, U_201C, U_201D, 0x2022, U_2013, U_2014, // 0x90 - 0x97
0x02dc, 0x2122, 0x0000, 0x203a, 0x0153, 0x0000, 0x0000, 0x0178, // 0x98 - 0x9f
0x00a0, 0x00a1, 0x00a2, 0x00a3, 0x00a4, 0x00a5, 0x00a6, 0x00a7, // 0xa0 - 0xa7
0x00a8, 0x00a9, 0x00aa, 0x00ab, 0x00ac, 0x00ad, 0x00ae, 0x00af, // 0xa8 - 0xaf
0x00b0, 0x00b1, 0x00b2, 0x00b3, 0x00b4, 0x00b5, 0x00b6, 0x00b7, // 0xb0 - 0xb7
0x00b8, 0x00b9, 0x00ba, 0x00bb, 0x00bc, 0x00bd, 0x00be, 0x00bf, // 0xb8 - 0xbf
0x00c0, 0x00c1, 0x00c2, 0x0102, 0x00c4, 0x00c5, 0x00c6, 0x00c7, // 0xc0 - 0xc7
0x00c8, 0x00c9, 0x00ca, 0x00cb, 0x0300, 0x00cd, 0x00ce, 0x00cf, // 0xc8 - 0xcf
0x0110, 0x00d1, 0x0309, 0x00d3, 0x00d4, 0x01a0, 0x00d6, 0x00d7, // 0xd0 - 0xd7
0x00d8, 0x00d9, 0x00da, 0x00db, 0x00dc, 0x01af, 0x0303, 0x00df, // 0xd8 - 0xdf
0x00e0, 0x00e1, 0x00e2, 0x0103, 0x00e4, 0x00e5, 0x00e6, 0x00e7, // 0xe0 - 0xe7
0x00e8, 0x00e9, 0x00ea, 0x00eb, 0x0301, 0x00ed, 0x00ee, 0x00ef, // 0xe8 - 0xef
0x0111, 0x00f1, 0x0323, 0x00f3, 0x00f4, 0x01a1, 0x00f6, 0x00f7, // 0xf0 - 0xf7
0x00f8, 0x00f9, 0x00fa, 0x00fb, 0x00fc, 0x01b0, 0x20ab, 0x00ff, // 0xf8 - 0xff
}; // end of CP1258TOP


CPag::CPag( )
{
    // I need a vector of CSols, let CPag own/create/destroy.
    // Init this array to all NULL ptrs, each is created jit:
    // CSol * CSolLGroupWords[ PAST_LANGUAGE_GROUP_IDS ];
    memset( CSolLGroupWords, 0, sizeof( CSolLGroupWords ) );

    #if DO_DEBUG_CALLS
        Routine( L"203" );
    #endif
}
CPag::~CPag( )
{
    // Delete any non-NULL items in vector CSolLGroupWords.
    int i = 0;
    for( ;; )
    {
        CSol * pSol = CSolLGroupWords[ i ];
        if( pSol != NULL )
        {
            // I like to nullify pointers to destroyed objects.
            // In fact, I like to nullify before the destroy!
            CSolLGroupWords[ i ] = NULL;
            delete pSol;
        }
        if( ++i == PAST_LANGUAGE_GROUP_IDS )
            break;
    }
    #if DO_DEBUG_CALLS
        Routine( L"204" );
    #endif
}

wchar_t * CPag::CategorizeAndConvertInputBytesToWideWithBinaryRejection( size_t * pnWBuf, BYTE * pBBuf, size_t nBBuf )
{
    #if DO_DEBUG_CALLS
        Routine( L"328" );
    #endif

    // This routine is a wrapper for original convert to Unicode routine.
    // So that all fetch/cache/file paths benefit from contents analysis.


    #if DO_DEBUG_REFUSAL
        Spew( L"Categoring, Outer routine..." );
    #endif

    // Start by calling original routine:
    size_t nWBuf = 0;
    wchar_t * pWBuf = CategorizeAndConvertInputBytesToWide( & nWBuf, pBBuf, nBBuf );

    #if DO_DEBUG_REFUSAL
        if( pWBuf == NULL )
            Spew( L"Categoring. Inner routine returned NULL" );
        else
            Spew( L"Categoring. Inner routine returned non-NULL" );
    #endif

    // Now call my little data recognition heuristic to discard some.
    // Take advantage of all my callers being ready for a NULL return.
    // But will a live fetch still save binary after dislike content?

    if( pWBuf != NULL
    && nWBuf != 0 )
    {
        if( DislikeDueToDataContent( pWBuf, nWBuf ) )
        {
            MyFree( 216, UNPREDICTABLE, pWBuf );
            pWBuf = NULL;
            nWBuf = 0;
            #if DO_DEBUG_REFUSAL
                Spew( L"Categoring. Dislike routine DISliked content." );
            #endif
        }
        else
        {
            #if DO_DEBUG_REFUSAL
                Spew( L"Categoring. Dislike routine LIKED content." );
            #endif
        }
    }

    *pnWBuf = nWBuf;
    return pWBuf;
}

wchar_t * CPag::CategorizeAndConvertInputBytesToWide( size_t * pnWBuf, BYTE * pBBuf, size_t nBBuf )
{
    #if DO_DEBUG_CALLS
        Routine( L"205" );
    #endif
    // Up to now, this only served routine above, which converts html/text
    // for web page input: add cache, query/fetch, or add file/directory.
    // Now I will be using it when loading the search engine control file.

    // This will be the very first act upon a source text,
    // either from inputting a file, loading a cache item,
    // or fetching a web page.

    // To protect all later HTML and TEXT parsers and CSee,
    // change NULLS, and controls except CR and LF to space.

    // We already think input is text, if got any HTTP header.
    // But prove it to myself. Also notice Unicode vs. binary.
    // Even if it is Unicode, this counting technique is okay.
    // I will not yet check for string matches to <html>, etc.
    // Any WordsEx top header recognition can also come later.

    // Test leading bytes for MBS or Unicode identifiers.
    // Convert either SBCS bytes, UTF-8, or UCS, into UCS.
    // MBS ( UTF-8 ) needs no special flags, nor gating,
    // for it seems safe to always recognize UTF-8 codes.

    // Return a pointer to new wide buffer, and set count.
    // I may return a null pointer, when input is binary.
    // Do not allow a NULL in output before end of buffer.

    // I do not write into source byte buffer for any reason,
    // which in some cases is a read-only file mapping buffer.

    // The return malloc buffer may be fatter than data.
    // I strip out any UCS or UTF-8 introducer sequence.

    // Nah... All not here:
    // I might move the HTML entity work into this routine,
    // but then I need to map LAB and RAB to unused shorts,
    // and the HTML parser will test for those new values.
    // Or I might just leave entity work in the HTML parse.
    //
    // Charset revision will occur after HTML entity work,
    // for a user of entities has no foreknowledge of UCS,
    // but may be specifying a 8-bit value in a codepage.
    //
    // Having studyied codepages, I will not need to deal
    // with SBCS at this time, but can do a later revision
    // of the range 0x80-0xff single bytes after possibly
    // learning the correct charset parsing HTML META tag.
    // I may or may not try to a MBCS( CJK ) final revision.

    if( nBBuf == 0 )
    {
        * pnWBuf = 0;
        return NULL; // All callers are ready for a NULL return.
    }

    // 1. Pre-conversion analysis, limited to 4096 bytes.

    BYTE * tp = pBBuf;
    int n = nBBuf;

    if( n > 10000 )
        n = 10000;

    int ExtractFromUnicode = 0; // 1 for LSB-first, 2 for MSB-first
    int SkipIntroduction = 0;
    int ContentTypeIsBinary = 0;

    // I had to try it twice, thinking 0xfffe was the magic wchar_t value,
    // but it must be that 0xfeff is the wchar_t Unicode identifier value.

    if( n >= 2 && tp[0] == 0xff && tp[1] == 0xfe )
    {
        // Unicode saved by notepad: Unicode ( LSB-first ):
        // 000000:  FF FE 68 00 74 00 74 00 70 00 3A 00 2F 00 2F 00   ..h.t.t.p.:././.
        // Unicode saved by notepad:
        // FF FE 20 00 74 00 68 00 69 00 73 00 20 00 73 00   .. .t.h.i.s. .s.
        ExtractFromUnicode = 1; // 1 for LSB-first
        SkipIntroduction = 2;
    }
    else
    if( n >= 2 && tp[0] == 0xfe && tp[1] == 0xff )
    {
        // Unicode saved by notepad: Unicode, big-endian:
        // 000000:  FE FF 00 68 00 74 00 74 00 70 00 3A 00 2F 00 2F   ...h.t.t.p.:././
        // Unicode saved by notepad, big-endian:
        // FE FF 00 20 00 74 00 68 00 69 00 73 00 20 00 73   ... .t.h.i.s. .s
        ExtractFromUnicode = 2; // 2 for MSB-first
        SkipIntroduction = 2;
    }
    else
    if( n >= 3 && tp[0] == 0xef && tp[1] == 0xbb  && tp[2] == 0xbf )
    {
        // Unicode saved by notepad: Unicode, UTF-8:
        // 000000:  EF BB BF 68 74 74 70 3A 2F 2F 77 77 77 2E 6E 63   ...http://www.nc
        // UTF-8 saved by notepad:
        // EF BB BF 20 74 68 69 73 20 73 61 79 73 20 3C 68   ... this says <h
        // I do UTF-8 decoding all the time, even if no identifier appears.
        SkipIntroduction = 3;
    }

    // Later, I might add recognition for some common binary formats'
    // introductory byte sequences...

    else if( nBBuf > 1024 )
    {
        // Do not attempt this statistical analysis in too shallow of water.
        // Without one of those introducers, scan top of bytes to categorize.
        // You break me under every assumption: Text need not have a newline!
        // This first scan will not make html, nor wrapped text distinctions.

        int i = 0;

        int nNullBytes = 0; // total count of 0x00 bytes
        int nNullEven = 0; // count of 0x00 at even addresses
        int nNullOdd = 0; // count of 0x00 at odd addresses
        int nSpace = 0; // count of SPACE, TAB, FORMFEED or NEWLINE
        int nControlBytes = 0; // count of 27 other codes 0x01 to 0x1f
        int nControlEven = 0; // count of them at even address
        int nControlOdd = 0; // count of them at add address
        int nPrint = 0; // number above ' ' up to 255 ( incl UTF-8, MBCS )

        for( ;; )
        {
            BYTE c = tp[i];

            if( c > ' ' ) // consider 0x20 to 0xff as printable
            {
                nPrint ++;

                // One more thing I need to recognize: MBCS ( CJK ).
                // Recognize a CJK "introducer" byte followed by a
                // non-introducer byte range, and count that here.
                // No.
                // It cannot be done. 0x81-0xff = DBCS LEAD BYTES;
                // But roughly any byte value may be a completion.

                // If I can avoid mangling them until a later META
                // tag showing the DBCS charset, then I can elect
                // to convert them later, or exclude such pages.
                // I can avoid mangling them if I do not apply the
                // UTF-8 conversion; And I could gate that off, if
                // this pre-scan shows invalid UTF-8 combinations.
                // But I will not mess with CJK idea at this time.

            }
            else if( c == ' ' )
            {
                nSpace ++;
            }
            else if( c == '\r' || c == '\n' || c == '\t' || c == '\f' )
            {
                nSpace ++;
            }
            else if( c == 0 )
            {
                nNullBytes ++;

                if( ( i & 1 ) == 0 )
                    nNullEven ++;
                else
                    nNullOdd ++;
            }
            else
            {
                nControlBytes ++;

                if( ( i & 1 ) == 0 )
                    nControlEven ++;
                else
                    nControlOdd ++;
            }

            // Do you know how many hours OB1 ">" cost?
            // Byte[n] is past buffer. Do not access it!
            if( ++i == n )
                break;
        }

        // make an heuristic call from those byte statistics

        // Purely random binary data might have 1/256 nulls,
        // and likely structured binary data will have more.
        // But do not exclude page mentioned of 1/400 nulls.
        // Ascii or ISO Latin in Unicode would be 50% nulls.
        // Only apply binary / unicode heuristics above 2 %.

        if( nNullBytes * 50 > n )
        {
            // Recognize Unicode, otherwise file is likely binary data.
            // What heuristic? By preponderance on one side: 50% vs 5%.
            // No, that would break down in a purely foreign char text.
            // That is, a few Unicode codes may contain LSB value 0x00.
            // Furthermore, they could use non-ascii UCS with no nulls.
            // In fact, they may contain 0x00 to 0x1f 'control' values.
            // They should have used introducer. This will save others.
            // I shouldn't really try to make inferences about Unicode.
            // This math might also misrecognize too short UCS buffers.

            // Here is a new problem: A very important MS HTML web page
            // about internationalization contains sporadic NULL bytes!
            // http://www.microsoft.com/globaldev/nlsweb/default.mspx
            // WOW! this page must have every HTML deviation possible,
            // INCLUDING NULL bytes ending sub-strings all over it,
            // and many UTF-8 127-255 codes, and entity escapes, which
            // are native names in just about EVERY possible language!
            //
            //     416  15322  268139   in_nls.htm
            //
            //       546 Nasty byte 0x00 -- so that is about 0.2 % nulls
            //         8 Nasty byte 0xa0
            //         2 Nasty byte 0xad
            //         2 Nasty byte 0xb4
            //         1 Nasty byte 0xc9
            //         1 Nasty byte 0xcd
            //         1 Nasty byte 0xd6
            //         4 Nasty byte 0xe0
            //         9 Nasty byte 0xe1
            //         7 Nasty byte 0xe2
            //         6 Nasty byte 0xe4
            //         6 Nasty byte 0xe5
            //        11 Nasty byte 0xe7
            //         5 Nasty byte 0xe9
            //         8 Nasty byte 0xea
            //         3 Nasty byte 0xeb
            //         2 Nasty byte 0xed
            //         1 Nasty byte 0xef
            //        23 Nasty byte 0xf1
            //         1 Nasty byte 0xf5
            //         2 Nasty byte 0xf6
            //         3 Nasty byte 0xf8
            //         2 Nasty byte 0xfa
            //         5 Nasty byte 0xfc

            if( nNullOdd * 4 > n                // > 25 %
            && nNullEven * 50 < n )             // < 2 %
            {
                ExtractFromUnicode = 1; // 1 for LSB-first
            }
            else if( nNullEven * 4 > n         // > 25 %
            && nNullOdd * 50 < n )             // < 2 %
            {
                ExtractFromUnicode = 2; // 2 for MSB-first
            }
            else
            {
                ContentTypeIsBinary = 1; // over 2% nulls, but not UCS.
            }
        }

        // Next heuristic: I eliminated 4 values out of 1 to 31.
        // Random binary data might yield about 27/256 controls.
        // But also, Unicode can easily have all the even bytes,
        // or all the odd bytes, in the 0x01-0x1f control range,
        // when encoding UCS characters from 0x0020 to 0x1fff.
        // Only apply control-byte-range heuristics above 5 %.

        else if( nControlBytes * 20 > n )
        {
            if( nControlOdd * 4 > n                // > 25 %
            && nControlEven * 50 < n )             // < 2 %
            {
                ExtractFromUnicode = 1; // 1 for LSB-first
            }
            else if( nControlEven * 4 > n         // > 25 %
            && nControlOdd * 50 < n )             // < 2 %
            {
                ExtractFromUnicode = 2; // 2 for MSB-first
            }
            else
            {
                ContentTypeIsBinary = 1; // over 5% controls, but not UCS.
            }
        }

        // I cannot yet make any printable vs. space assumptions.

    }

    if( ContentTypeIsBinary )
    {
        * pnWBuf = 0;
        return NULL; // All callers are ready for a NULL return.
    }

    // Henceforth I will malloc, convert, and not return a null/zero.

    // Now convert to wide using corrected encoding method.
    // That means, I may use bytes in pairs if already UCS.
    // But in that case, I do not need the UTF8 conversion.

    if( ExtractFromUnicode )
    {
        // The input is already in UNICODE UCS.
        // It cannot grow ( except 2 bytes for null ).
        size_t MaxMalloc = nBBuf + 2;
        wchar_t * Converted = ( wchar_t * ) MyMalloc( 474, MaxMalloc );
        wchar_t * into = Converted;
        BYTE * from = pBBuf + SkipIntroduction;
        BYTE * stop = pBBuf + nBBuf;

        // I could think in BYTES, but want to eliminate NULL shorts.
        for( ;; )
        {
            unsigned int b1;
            unsigned int b2;
            unsigned int ucs;
            // test for limit n before access [n].
            if( from == stop )
                break;
            b1 = *from++;
            if( from == stop )
                break;
            b2 = *from++;
            if( ExtractFromUnicode == 2 ) // 2 for MSB-first
            {
                ucs = ( b1 << 8 ) | b2;
            }
            else
            {
                ucs = ( b2 << 8 ) | b1;
            }

            if( ucs >= 0x007f )
            {
                // Let's eliminate certain UNICODE ignorable characters:
                // "Default_Ignorable_Code_Point"

                if( ucs < 0x200B )
                {
                    // I'll take the low part,

                    // 0000..0008  <control-0000>..<control-0008>
                    // 000E..001F  <control-000E>..<control-001F>
                    // 007F..0084  <control-007F>..<control-0084>
                    // 0086..009F  <control-0086>..<control-009F>
                    // 00AD        SOFT HYPHEN
                    // 034F        COMBINING GRAPHEME JOINER

                    if( ucs < 0x00A0 )
                    {
                        // very low part.
                        if( ucs == 0x0085 ) // only character to do
                        {
                            // Convert this "character" to a new line:
                            // NEL next line 0085 85 15 25
                            *into++ = '\n'; // cannot afford to widen buffer.
                        }
                    }
                    else
                    {
                        // middling low part.
                        if( ucs != 0x00AD // That infamous SHY = soft hyphen!
                        && ucs != 0x034F ) // only characters NOT to do
                            *into++ = ucs;
                    }
                }
                else
                {
                    // And I'll take the high part.

                    // 200B..200F  ZERO WIDTH SPACE..RIGHT-TO-LEFT MARK
                    // 202A..202E  LEFT-TO-RIGHT EMBEDDING..RIGHT-TO-LEFT OVERRIDE
                    // 2060..2063  WORD JOINER..INVISIBLE SEPARATOR
                    // FEFF        ZERO WIDTH NO-BREAK SPACE

                    switch( ucs )
                    {
                        // Most chars use default case, except some to ignore.

                        // However convert these to a newline.
                        // LS line separator 2028 n/a n/a n/a
                        // PS paragraph separator 2029 n/a n/a n/a
                        // For PS, two newlines would be better.
                    case 0x2028:
                    case 0x2029:
                        *into++ = '\n'; // cannot afford to widen buffer.
                        break;

                    case 0x200B:
                    case 0x200C:
                    case 0x200D:
                    case 0x200E:
                    case 0x200F:
                    case 0x202A:
                    case 0x202B:
                    case 0x202D:
                    case 0x202E:
                    case 0x2060:
                    case 0x2061:
                    case 0x2062:
                    case 0x2063:
                    case 0xFEFF:
                        break;


                    // Now is my chance to fix some hyphens and quotes too:
                    //
                    // these as 0x002d - hyphen
                    //
                    // U+2010 HYPHEN,
                    // U+2011 NON-BREAKING HYPHEN,
                    // U+2012 FIGURE DASH,
                    // U+2013 EN DASH,
                    // U+2014 EM DASH,
                    // U+2015 HORIZONTAL BAR ( quotation dash ), and
                    // U+2212 MINUS SIGN
                    //
                    // these as 0x0027 // Single quote
                    //
                    // U+2018 LEFT SINGLE QUOTATION MARK
                    // U+2019 RIGHT SINGLE QUOTATION MARK
                    // U+201A SINGLE LOW-9 QUOTATION MARK
                    // U+201B SINGLE HIGH-REVERSED-9 QUOTATION MARK
                    //
                    // these as 0x0022 // Double quote
                    //
                    // U+201C LEFT DOUBLE QUOTATION MARK
                    // U+201D RIGHT DOUBLE QUOTATION MARK
                    // U+201E DOUBLE LOW-9 QUOTATION MARK
                    // U+201F DOUBLE HIGH-REVERSED-9 QUOTATION MARK

                    case 0x2010:
                    case 0x2011:
                    case 0x2012:
                    case 0x2013:
                    case 0x2014:
                    case 0x2015:
                    case 0x2212:
                        *into++ = 0x002d;
                        break;

                    case 0x2018:
                    case 0x2019:
                    case 0x201A:
                    case 0x201B:
                        *into++ = 0x0027;
                        break;

                    case 0x201C:
                    case 0x201D:
                    case 0x201E:
                    case 0x201F:
                        *into++ = 0x0022;
                        break;


                    default:
                        *into++ = ucs;
                        break;
                    }
                }

            }
            else if( ucs >= ' ' // UCS: output no NULLS or controls, except CR LF.
            || ucs == '\r'
            || ucs == '\n' )
            {
                *into++ = ucs;
            }
            else
            {
                // between 0x0000 and 0x001f - control codes.
                // Per rules above, I could ignore these. But...
                // Unicode suggests these all serve as newlines:
                // Not doing newline to CR LF rectification here.
                // Acronym Name Unicode ASCII EBCDIC
                // CR carriage return 000D 0D 0D 0D
                // LF line feed 000A 0A 25 15
                // CRLF carriage return and line feed 000D,000A 0D,0A 0D,25 0D,15
                // NEL next line 0085 85 15 25
                // VT vertical tab 000B 0B 0B 0B
                // FF form feed 000C 0C 0C 0C
                // LS line separator 2028 n/a n/a n/a
                // PS paragraph separator 2029 n/a n/a n/a

                if( ucs == '\t' )
                {
                    // Convert TAB to a space:
                    *into++ = ' ';
                }
                else
                if( ucs == 0x000B
                || ucs == 0x000C )
                {
                    // Convert VT or FF to a new line:
                    *into++ = '\n'; // cannot afford to widen buffer.
                }
                // Ignore all other controls.
            }

        }

        *into = NULL; // gratuitous?

        // I am ready to return buffer and count, i.e., wcslen.
        * pnWBuf = into - Converted;
        return Converted;
    }

    // Originally, I forced everything into US-ASCII.
    // Then I opened up to ISO-LATIN etc. Not any more.
    // I will return any codepoint in 0x0001 to 0xffff.

    // I have realized UTF-8 are sufficiently self-identifying
    // to always have active parsing and conversion in place.
    // They always start C,D,E,F hex, and continue 8,9,A,B hex.
    // The tersest UTF-8 idea comes from this Italian web page:
    // http://www.pluto.it/files/ildp/man/man7/utf-8.7.html
    //  0x00000000 - 0x0000007F: 0xxxxxxx
    //  0x00000080 - 0x000007FF: 110xxxxx 10xxxxxx
    //  0x00000800 - 0x0000FFFF: 1110xxxx 10xxxxxx 10xxxxxx
    //  0x00010000 - 0x001FFFFF: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
    //  0x00200000 - 0x03FFFFFF: 111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
    //  0x04000000 - 0x7FFFFFFF: 1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx

    // Here is a UTF8-intensive test page to parse:
    // http://zoonek.free.fr/LaTeX/Dictionnaire_japonais/documentation.html
    //  62 &Entity: "&amp"->'&'.
    // 740 &Entity: "&lt"->'<'.
    //  16 UTF-8 c2 ab->171->DQT.
    //  16 UTF-8 c2 bb->187->DQT.
    //   1 UTF-8 c3 80->192->'A'.
    //   1 UTF-8 c3 87->199->'C'.
    //   4 UTF-8 c3 89->201->'E'.
    //   2 UTF-8 c3 9f->223->'s'.
    //  86 UTF-8 c3 a0->224->'a'.
    //  35 UTF-8 c3 a7->231->'c'.
    // 155 UTF-8 c3 a8->232->'e'.
    // 394 UTF-8 c3 a9->233->'e'.
    //  36 UTF-8 c3 aa->234->'e'.
    //   2 UTF-8 c3 ae->238->'i'.
    //   3 UTF-8 c3 af->239->'i'.
    //   4 UTF-8 c3 b4->244->'o'.
    //   1 UTF-8 c3 b6->246->'o'.
    //   5 UTF-8 c3 b9->249->'u'.
    //   2 UTF-8 c3 bb->251->'u'.
    //   1 UTF-8 c5 93->339->'o'.
    //   1 UTF-8 e3 80 81->12289->' '.
    //   2 UTF-8 e3 80 82->12290->' '.
    //   1 UTF-8 e3 80 8c->12300->' '.
    //   1 UTF-8 e3 80 8d->12301->' '.
    //...etc...
    //   2 UTF-8 e9 a1 98->39000->' '.
    //   3 UTF-8 e9 a2 a8->39080->' '.
    //   1 UTF-8 e9 b3 a5->40165->' '.
    //   1 UTF-8 ef bc ae->65326->' '.

    // I will return this sloppy worst-case sized output buffer.
    wchar_t * Converted = ( wchar_t * ) MyMalloc( 1918, ( nBBuf + 1 ) * sizeof( wchar_t ) );
    wchar_t * into = Converted;
    BYTE * from = pBBuf + SkipIntroduction;
    BYTE * stop = pBBuf + nBBuf;

    int InputUtf8State = 0;
    wchar_t InputUtf8Byte1 = 0;
    wchar_t InputUtf8Byte2 = 0;
    wchar_t InputUtf8Byte3 = 0;

    for( ;; )
    {
        if( from == stop )
            break;

        wchar_t inputChar = *from ++;

        // If I were not doing the UTF-8 parse, I would just say now:
        // *into++ = inputChar;
        // However, those acts are scattered throughout switch below.

        // Possibly some valid UTF-8 might be mixed with some ISO LATIN,
        // although it would not be right; Restart after an invalid UTF-8
        // sequence by re-examining unacceptable input byte as a starter:

    RestartUsingInputChar: ;

        switch( InputUtf8State )
        {
        case 0:
            if( inputChar < 0xC0 )
            {
                // Most input characters will take this path:

                if( inputChar >= ' ' // post UTF-8 parse: output no NULLS or controls, except TAB CR LF.
                || inputChar == '\r'
                || inputChar == '\n' )
                    *into++ = inputChar;
                else
                    *into++ = ' '; // Convert TABs, NULLs, all controls except CR LF.

                continue;
            }

            // This group of if's saves UTF8 byte1:

            if( inputChar < 0xE0 )
            {
                InputUtf8State = 5; // I have 5 bits
                InputUtf8Byte1 = inputChar;
                continue;
            }
            if( inputChar < 0xF0 )
            {
                InputUtf8State = 4; // I have 4 bits
                InputUtf8Byte1 = inputChar;
                continue;
            }
            if( inputChar < 0xF8 )
            {
                InputUtf8State = 3; // I have 3 bits
                InputUtf8Byte1 = inputChar;
                continue;
            }

            // Ignore any longer UTF-8 encodings, useless to me.
            *into++ = inputChar; // Ignore UTF-8 for 0xf8 or above starters.
            continue;

            // This group of cases saves UTF8 byte2:

        case 3:
            // I have 3 bits
            if( ( inputChar & 0xC0 ) == 0x80 )
            {
                // This is a valid tail byte. Go 3 more states to confirm.
                InputUtf8State = 9; // I have 3 + 6 bits
                InputUtf8Byte2 = inputChar;
                continue;
            }
            // No UTF-8 sequence. Process 1 held and this byte.
            InputUtf8State = 0;
            if( InputUtf8Byte1 != NULL )
                *into++ = InputUtf8Byte1;
            goto RestartUsingInputChar;

        case 4:
            // I have 4 bits
            if( ( inputChar & 0xC0 ) == 0x80 )
            {
                // This is a valid tail byte. Go two more states to confirm.
                InputUtf8State = 10; // I have 4 + 6 bits
                InputUtf8Byte2 = inputChar;
                continue;
            }
            // No UTF-8 sequence. Process 1 held and this byte.
            InputUtf8State = 0;
            if( InputUtf8Byte1 != NULL )
                *into++ = InputUtf8Byte1;
            goto RestartUsingInputChar;

        case 5:
            // I have 5 bits
            // Next byte can be anything. Process this final byte now.
            if( ( inputChar & 0xC0 ) == 0x80 )
            {
                // This is a valid tail byte. Assemble bit representation.
                // Use 5 bits of byte1, 6 bits of byte2.
                wchar_t Assembled = ( InputUtf8Byte1 & 0x1F ) << 6;
                Assembled |= ( inputChar & 0x3F );
                if( Assembled != NULL )
                    *into++ = Assembled;
                InputUtf8State = 0;
                continue;
            }
            // No UTF-8 sequence. Process 1 held and this byte.
            InputUtf8State = 0;
            if( InputUtf8Byte1 != NULL )
                *into++ = InputUtf8Byte1;
            goto RestartUsingInputChar;

            // This group of cases saves UTF8 byte3:

        case 9:
            // I have 3 + 6 bits
            if( ( inputChar & 0xC0 ) == 0x80 )
            {
                // This is a valid tail byte. Go more states to confirm.
                InputUtf8State = 15; // I have 3 + 6 + 6 bits
                InputUtf8Byte2 = inputChar;
                continue;
            }
            // No UTF-8 sequence. Process 2 held and this byte.
            InputUtf8State = 0;
            if( InputUtf8Byte1 != NULL )
                *into++ = InputUtf8Byte1;
            if( InputUtf8Byte2 != NULL )
                *into++ = InputUtf8Byte2;
            goto RestartUsingInputChar;

        case 10:
            // I have 4 + 6 bits
            // Next byte can be anything. Process this final byte now.
            if( ( inputChar & 0xC0 ) == 0x80 )
            {
                // This is a valid tail byte. Assemble bit representation.
                // Use 4 bits of byte1, 6 bits of byte2, 6 bits of byte3.
                wchar_t Assembled = ( InputUtf8Byte1 & 0x0F ) << 12;
                Assembled |= ( InputUtf8Byte2 & 0x3F ) << 6;
                Assembled |= ( inputChar & 0x3F );

                // This clause processes these chars:
                // utf: e2 80 99 = apostraphe
                // 11100010 10000000 10011001
                // 1110xxxx 10xxxxxx 10xxxxxx
                // ....0010...000000...011001
                // 00100000 00011001 = 0x2019

                // Ugly American. Fix various hyphens and quotes now.
                switch( Assembled )
                {
                case 0x2010:
                case 0x2011:
                case 0x2012:
                case 0x2013:
                case 0x2014:
                case 0x2015:
                case 0x2212:
                    Assembled = 0x002d;
                    break;

                case 0x2018:
                case 0x2019:
                case 0x201A:
                case 0x201B:
                    Assembled = 0x0027;
                    break;

                case 0x201C:
                case 0x201D:
                case 0x201E:
                case 0x201F:
                    Assembled = 0x0022;
                    break;

                    // However convert these to a newline.
                    // LS line separator 2028 n/a n/a n/a
                    // PS paragraph separator 2029 n/a n/a n/a
                    // For PS, two newlines would be better.
                case 0x2028:
                case 0x2029:
                    Assembled = '\n'; // can't I widen buffer?
                    break;

                    // Rid these. See comments in unicode case.
                case 0x200B:
                case 0x200C:
                case 0x200D:
                case 0x200E:
                case 0x200F:
                case 0x202A:
                case 0x202B:
                case 0x202D:
                case 0x202E:
                case 0x2060:
                case 0x2061:
                case 0x2062:
                case 0x2063:
                    Assembled = NULL;
                    break;

                }

                if( Assembled != NULL )
                    *into++ = Assembled;
                InputUtf8State = 0;
                continue;
            }
            // No UTF-8 sequence. Process 2 held and this byte.
            InputUtf8State = 0;
            if( InputUtf8Byte1 != NULL )
                *into++ = InputUtf8Byte1;
            if( InputUtf8Byte2 != NULL )
                *into++ = InputUtf8Byte2;
            goto RestartUsingInputChar;

        case 15:
            // I have 3 + 6 + 6 bits
            // Next byte can be anything. Process this final byte now.
            if( ( inputChar & 0xC0 ) == 0x80 )
            {
                // This is a valid tail byte. Assemble bit representation.
                // Use 3 bits of byte1, 6 bits of byte2, byte3, byte4.
                wchar_t Assembled = ( InputUtf8Byte1 & 0x07 ) << 18;
                Assembled = ( InputUtf8Byte1 & 0x3F ) << 12;
                Assembled |= ( InputUtf8Byte2 & 0x3F ) << 6;
                Assembled |= ( inputChar & 0x3F );

                if( Assembled != NULL )
                    *into++ = Assembled;
                InputUtf8State = 0;
                continue;
            }
            // No UTF-8 sequence. Process 3 held and this byte.
            InputUtf8State = 0;
            if( InputUtf8Byte1 != NULL )
                *into++ = InputUtf8Byte1;
            if( InputUtf8Byte2 != NULL )
                *into++ = InputUtf8Byte2;
            if( InputUtf8Byte3 != NULL )
                *into++ = InputUtf8Byte3;
            goto RestartUsingInputChar;


        default:
            // Never expected, guide it out of programmer error state.
            InputUtf8State = 0;
            if( InputUtf8Byte1 != NULL )
                *into++ = InputUtf8Byte1;
            if( InputUtf8Byte2 != NULL )
                *into++ = InputUtf8Byte2;
            if( InputUtf8Byte3 != NULL )
                *into++ = InputUtf8Byte3;
            goto RestartUsingInputChar;
        }

    }
    *into = NULL;

    // Return WCSLEN, not buffer size:
    * pnWBuf = into - Converted;
    return Converted;
}

void CPag::AdjustTextBufferPerCharset( COnePaper * pOnePaper )
{
    if( pOnePaper->HttpHeaderCharset == 0 )
        return;

    // Don't let a 0 for UNKNOWN override something Known!
    // We know that either 0 returns codepage 1252.
    //
    // ATTR_HTTP_EQUIV -- matched Content-Type.
    // AdjustPer...Charset: : 26
    // AdjustPer...Language: : 0
    // AdjustPer...Conflict: Charset Determined: : 1251
    // AdjustPer...Conflict: Language Determined: : 1252

    size_t nCSet = pOnePaper->HttpHeaderCharset;
    size_t nLang = pOnePaper->HttpHeaderContentLanguage;

    #if DO_DEBUG_LANGUAGE
        ; SpewValue( L"AdjustPer...Charset: ", nCSet );
    #endif

    #if DO_DEBUG_LANGUAGE
        ; SpewValue( L"AdjustPer...Language: ", nLang );
    #endif

    size_t nCodePage1 = 0;
    size_t nCodePage2 = 0;
    wchar_t * TopTable1 = NULL;
    wchar_t * TopTable2 = NULL;
    if( nCSet != 0 )
    {
        nCodePage1 = FamilyCodePageforCharsetIndex( nCSet );
        #if DO_DEBUG_LANGUAGE
            ; SpewValue( L"CodePagePerNonZero...Charset: ", nCodePage1 );
        #endif
        switch( nCodePage1 )
        {
        case 1200: TopTable1 = NULL; break; // for UTF-8
        case 1250: TopTable1 = CP1250TOP; break;
        case 1251: TopTable1 = CP1251TOP; break;
        case 1252: TopTable1 = CP1252TOP; break;
        case 1253: TopTable1 = CP1253TOP; break;
        case 1254: TopTable1 = CP1254TOP; break;
        case 1255: TopTable1 = CP1255TOP; break;
        case 1256: TopTable1 = CP1256TOP; break;
        case 1257: TopTable1 = CP1257TOP; break;
        case 1258: TopTable1 = CP1258TOP; break;
        }
    }
    if( nLang != 0 )
    {
        nCodePage2 = FamilyCodePageforLanguageIndex( nLang );
        #if DO_DEBUG_LANGUAGE
            ; SpewValue( L"CodePagePerNonZero...Language: ", nCodePage2 );
        #endif
        switch( nCodePage2 )
        {
        case 1200: TopTable2 = NULL; break; // for UTF-8
        case 1250: TopTable2 = CP1250TOP; break;
        case 1251: TopTable2 = CP1251TOP; break;
        case 1252: TopTable2 = CP1252TOP; break;
        case 1253: TopTable2 = CP1253TOP; break;
        case 1254: TopTable2 = CP1254TOP; break;
        case 1255: TopTable2 = CP1255TOP; break;
        case 1256: TopTable2 = CP1256TOP; break;
        case 1257: TopTable2 = CP1257TOP; break;
        case 1258: TopTable2 = CP1258TOP; break;
        }
    }

    if( TopTable1 == NULL
    && TopTable2 == NULL )
    {
        if( nCodePage1 == 1200
        || nCodePage2 == 1200 )
        {
            // Do not apply 1252 default when UTF-8 was used.
            #if DO_DEBUG_LANGUAGE
                ; Spew( L"AdjustPer...UTF=8 1200 specified. Apply no charset." );
            #endif

            // Do not just return now because already in UCS!
            // I want to loop over all the UCS characters to
            // eliminate UNICODE "Default_Ignorable_Code_Point"
            // return;

            // The easiest way is to make a do-nothing array:
            TopTable1 = TopTable2 = CP1200TOP;
        }
        else
        {
            TopTable1 = TopTable2 = CP1252TOP;

            #if DO_DEBUG_LANGUAGE
                ; Spew( L"AdjustPer...Nothing specified. Using 1252 default." );
            #endif
        }
    }
    else
    if( TopTable1 != NULL
    && TopTable2 != NULL )
    {
        if( TopTable1 == TopTable2 )
        {
            #if DO_DEBUG_LANGUAGE
                ; SpewValue( L"AdjustPer...Charset+Language Agreed: ", nCodePage1 );
            #endif
        }
        else
        {
            #if DO_DEBUG_LANGUAGE
                ; SpewValue( L"AdjustPer...Conflict: Charset Determined: ", nCodePage1 );
                ; SpewValue( L"AdjustPer...Conflict: Language Determined: ", nCodePage2 );
            #endif
        }
    }
    else
    {
        // Not neither; But not both. So just one.
        if( TopTable1 != NULL )
        {
            #if DO_DEBUG_LANGUAGE
                ; SpewValue( L"AdjustPer...Charset Only: ", nCodePage1 );
            #endif
        }
        if( TopTable2 != NULL )
        {
            TopTable1 = TopTable2; // Make it so, below!
            #if DO_DEBUG_LANGUAGE
                ; SpewValue( L"AdjustPer...Language Only: ", nCodePage2 );
            #endif
        }
    }

    // Get out a copy of the result buffer, convert it, and put it back.

    size_t nMallocBuf = 0;
    wchar_t * pMallocBuf = pOnePaper->pWsbResultText->GetBuffer( & nMallocBuf );

    size_t i = 0;
    size_t j = 0;
    for( ;; )
    {
        wchar_t wc = pMallocBuf[ i ];
        // The SBCS codepages ONLY convert certain bytes in 0x80-0xff range:
        wchar_t ucs = wc;
        if( wc >= 0x80
        && wc <= 0xff )
        {
            ucs = TopTable1[ wc - 0x80 ]; // index as 0x00 - 0x7f
        }
        // Now I have an UCS character, but should I ignore it?

        if( ucs >= 0x007f )
        {
            // Let's eliminate certain UNICODE ignorable characters:
            // "Default_Ignorable_Code_Point"

            if( ucs < 0x200B )
            {
                // I'll take the low part,

                // 0000..0008  <control-0000>..<control-0008>
                // 000E..001F  <control-000E>..<control-001F>
                // 007F..0084  <control-007F>..<control-0084>
                // 0086..009F  <control-0086>..<control-009F>
                // 00AD        SOFT HYPHEN
                // 034F        COMBINING GRAPHEME JOINER

                if( ucs < 0x00A0 )
                {
                    // very low part.
                    if( ucs == 0x0085 ) // only character to do
                    {
                        // Convert this "character" to a new line:
                        // NEL next line 0085 85 15 25
                        pMallocBuf[ j++ ] = '\n'; // cannot afford to widen buffer.
                    }
                }
                else
                {
                    // middling low part.
                    if( ucs != 0x00AD // That infamous SHY = soft hyphen!
                    && ucs != 0x034F ) // only characters NOT to do
                        pMallocBuf[ j++ ] = ucs;
                }
            }
            else
            {
                // And I'll take the high part.

                // 200B..200F  ZERO WIDTH SPACE..RIGHT-TO-LEFT MARK
                // 202A..202E  LEFT-TO-RIGHT EMBEDDING..RIGHT-TO-LEFT OVERRIDE
                // 2060..2063  WORD JOINER..INVISIBLE SEPARATOR
                // FEFF        ZERO WIDTH NO-BREAK SPACE

                switch( ucs )
                {
                    // Most chars use default case, except some to ignore.

                    // However convert these to a newline.
                    // LS line separator 2028 n/a n/a n/a
                    // PS paragraph separator 2029 n/a n/a n/a
                    // For PS, two newlines would be better.
                case 0x2028:
                case 0x2029:
                    pMallocBuf[ j++ ] = '\n'; // cannot afford to widen buffer.
                    break;

                case 0x200B:
                case 0x200C:
                case 0x200D:
                case 0x200E:
                case 0x200F:
                case 0x202A:
                case 0x202B:
                case 0x202D:
                case 0x202E:
                case 0x2060:
                case 0x2061:
                case 0x2062:
                case 0x2063:
                case 0xFEFF:
                    break;

                // Now is my chance to fix some hyphens and quotes too:
                //
                // these as 0x002d - hyphen
                //
                // U+2010 HYPHEN,
                // U+2011 NON-BREAKING HYPHEN,
                // U+2012 FIGURE DASH,
                // U+2013 EN DASH,
                // U+2014 EM DASH,
                // U+2015 HORIZONTAL BAR ( quotation dash ), and
                // U+2212 MINUS SIGN
                //
                // these as 0x0027 // Single quote
                //
                // U+2018 LEFT SINGLE QUOTATION MARK
                // U+2019 RIGHT SINGLE QUOTATION MARK
                // U+201A SINGLE LOW-9 QUOTATION MARK
                // U+201B SINGLE HIGH-REVERSED-9 QUOTATION MARK
                //
                // these as 0x0022 // Double quote
                //
                // U+201C LEFT DOUBLE QUOTATION MARK
                // U+201D RIGHT DOUBLE QUOTATION MARK
                // U+201E DOUBLE LOW-9 QUOTATION MARK
                // U+201F DOUBLE HIGH-REVERSED-9 QUOTATION MARK

                case 0x2010:
                case 0x2011:
                case 0x2012:
                case 0x2013:
                case 0x2014:
                case 0x2015:
                case 0x2212:
                    pMallocBuf[ j++ ] = 0x002d;
                    break;

                case 0x2018:
                case 0x2019:
                case 0x201A:
                case 0x201B:
                    pMallocBuf[ j++ ] = 0x0027;
                    break;

                case 0x201C:
                case 0x201D:
                case 0x201E:
                case 0x201F:
                    pMallocBuf[ j++ ] = 0x0022;
                    break;

                default:
                    pMallocBuf[ j++ ] = ucs;
                    break;
                }
            }

        }
        else if( ucs >= ' ' // UCS: output no NULLS or controls, except CR LF.
        || ucs == '\r'
        || ucs == '\n' )
        {
            pMallocBuf[ j++ ] = ucs;
        }
        else
        {
            // between 0x0000 and 0x001f - control codes.
            // Per rules above, I could ignore these. But...
            // Unicode suggests these all serve as newlines:
            // Not doing newline to CR LF rectification here.
            // Acronym Name Unicode ASCII EBCDIC
            // CR carriage return 000D 0D 0D 0D
            // LF line feed 000A 0A 25 15
            // CRLF carriage return and line feed 000D,000A 0D,0A 0D,25 0D,15
            // NEL next line 0085 85 15 25
            // VT vertical tab 000B 0B 0B 0B
            // FF form feed 000C 0C 0C 0C
            // LS line separator 2028 n/a n/a n/a
            // PS paragraph separator 2029 n/a n/a n/a

            if( ucs == '\t' )
            {
                // Convert TAB to a space:
                pMallocBuf[ j++ ] = ' ';
            }
            else
            if( ucs == 0x000B
            || ucs == 0x000C )
            {
                // Convert VT or FF to a new line:
                pMallocBuf[ j++ ] = '\n'; // cannot afford to widen buffer.
            }
            // Ignore all other controls.
        }

        // output index j might not have been incremented on every loop!
        if( ++i == nMallocBuf ) // input incremented on every loop, and tested.
            break;
    }

    pOnePaper->pWsbResultText->Reset( );
    pOnePaper->pWsbResultText->Addn( pMallocBuf, j );
    MyFree( 822, ( nMallocBuf + 1 ) * sizeof( wchar_t ), pMallocBuf );
}

void CPag::ScanForClearTextUrls( COnePaper * pOnePaper )
{
    // as needed, create and fill member in COnePaper:
    // pIdxClearTextUrls = NULL; // do JIT: new CIdx( );

    // I scan both HTML and TEXT inputs, and will recognize
    // all absolute URLS: those starting with method prefix,
    // and some idiomatic forms, .com, even without methods.
    // However, omit any URL containing an ellipsis.

    // Start the scan with a bi-phase loop to find tokens.
    // Notice if token contains a dot between alnums too.

    #if DO_DEBUG_CLEARURLS
        ; Spew( L"Entering ScanForClearTextUrls" );
    #endif

    size_t nMallocBuf = 0;
    wchar_t * pMallocBuf = pOnePaper->pWsbResultText->GetBuffer( & nMallocBuf ); // a malloc, user frees
    if( pMallocBuf == NULL
    || nMallocBuf == 0 )
        return;

    wchar_t * scan = pMallocBuf;

    for( ;; )
    {
        wchar_t wc; // Phase 1: traverse any spaces.
        for( ;; )
        {
            wc = *scan;
            if( wc == NULL )
                break;
            if( wc > ' ' ) // spaces being CR, LF, SPACE.
                break;
            scan++;
        }
        if( wc == NULL )
            break;

        wchar_t * atop = scan;

        // Phase 1: traverse non-spaces.
        for( ;; )
        {
            wc = *scan;
            if( wc == NULL )
                break;
            if( wc <= ' ' ) // spaces being CR, LF, SPACE.
                break;
            scan++;
        }
        if( wc == NULL )
            break;

        wchar_t * past = scan;

        int len = past - atop;
        if( len >= 5 ) // minimum URL has 5 chars: x.com
        {
            wchar_t save = *past;
            *past = NULL;

            #if DO_DEBUG_CLEARURLS
                ; SpewTwo( L"token", atop );
            #endif

            // This URL test can be further developed:
            if( ClearTextTokenResemblesUrl( atop, len ) )
            {
                int StartOffsetInText = atop - pMallocBuf;
                int EndingOffsetInText = past - pMallocBuf;

                // I need to canonicalize it, hang it, etc...
                int UrlIndex = Www.CombineAndHangUrl( NULL, atop, 0 ); // NULL=no BASE URL. 0=not for form.

                #if DO_DEBUG_CLEARURLS
                    ; SpewValue( L"Resembles; Hung UrlIndex", UrlIndex );
                #endif

                if( UrlIndex > 1 )
                {
                    if( pOnePaper->pIdxClearTextUrls == NULL )
                    {
                        pOnePaper->pIdxClearTextUrls = new CIdx( ); // create JIT
                    }

                    pOnePaper->pIdxClearTextUrls->AddIdx( StartOffsetInText, EndingOffsetInText, UrlIndex, 0 );

                    // New work to cross-reference links:
                    size_t iAnchorText = CSolAnchorText.AddKey( L"Clear text URL" );
                    #if DO_DEBUG_ADDFIND
                    if( iAnchorText <= 1 )
                        { Spew( L"AddFind <= 1 at cpag 1354" ); }
                    #endif
                    AddAnchorHeadTailText( pOnePaper->m_CSolIndex, UrlIndex, iAnchorText );
                }
            }
            *past = save;
        }
    }
    MyFree( 1261, ( nMallocBuf + 1 ) * sizeof( wchar_t ), pMallocBuf );
}

BYTE * CPag::Interleave( CBud * pFruit,  size_t * nMallocBuf, int AsText )
{
    // Every Fruit, being derived from CBud,
    // has a non-NULL member pointer to both:
    // CWsb * pWsbResultText;
    // CIdx * pIdxResultIndex;
    // One or both of them may have no mallocs.

    // During File output, it will be necessary
    // to convert any IDX ranges into <a> </a>
    // annotations interspered in the HTML text.
    // Do not insert links if AsText is true.

    // Optionally convert Unicode to MSB or USASCII.

    // I will build that byte buffer in one CAsb,
    // but get the buffer and delete CAsb myself.

    CAsb * pAsb = new CAsb( );

    // Get the first of IDX items that we must paw through.
    // CIdx * pIdxResultIndex;

    size_t Atop = 0;
    size_t Past = 0;
    size_t Index = 0;
    size_t Offset = 0;

    // I need not test return success of Get( ), as
    // failure will set Atop and Past to max values.
    size_t nLookup = 0; // I'll pre-increment this before later calls
    pFruit->pIdxResultIndex->GetIdx( nLookup, & Atop, & Past, & Index, & Offset );

    if( AsText )
    {
        Atop = 0x7fffffff; // way past text size
        Past = 0x7fffffff;
    }

    // Get the wide text buffer that we must paw through.
    size_t nMallocW = 0;
    wchar_t * pMallocW = pFruit->pWsbResultText->GetBuffer( & nMallocW );


    if( pMallocW != NULL )
    {
        int NeedMBS = 0;
        // Prescan the buffer to decide if MBS is needed.
        {
            size_t i = 0;
            for( ;; )
            {
                if( pMallocW[ i ] > 0x007f ) // Other than PURE US-ASCII?
                {
                    NeedMBS = 1;
                    break;
                }
                if( ++ i == nMallocW )
                    break;
            }
        }

        if( NeedMBS )
        {
            // Output an MBS ( UTF-8 ) file marker sequence:
            static BYTE MBSHeader[3] = { 0xef, 0xbb, 0xbf };
            pAsb->Addn( MBSHeader, 3 );
        }

        //  I will only handle UCS up to 16 bits, UTF-8 thus:
        //  0x00000000 - 0x0000007F: 0xxxxxxx
        //  0x00000080 - 0x000007FF: 110xxxxx 10xxxxxx
        //  0x00000800 - 0x0000FFFF: 1110xxxx 10xxxxxx 10xxxxxx

        size_t nScan = 0;
        for( ;; )
        {
            // We can process as many wides as exist,
            // just up to, but not including, [Atop].

            #if DO_DEBUG_INTERLEAVE
                ; SpewValue( L"nLookup", nLookup );
                ; SpewValue( L"Atop", Atop );
                ; SpewValue( L"Past", Past );
                ; SpewValue( L"Index", Index );
                ; SpewValue( L"Offset", Offset );
            #endif

            size_t nLimit = Atop;
            if( nLimit > nMallocW )
                nLimit = nMallocW;

            int nChars = nLimit - nScan;
            if( nChars < 0 )
            {
                ProgramError( L"Interleave: nLimit < nScan 1" );
                break;
            }
            if( nChars > 0 )
            {
                // This is one of two places to test/convert chars.
                // Before conversion, it was just this easy:
                // pAsb->AddnWide( pMallocW + nScan, nChars );
                // nScan += nChars;

                size_t nStop = nScan + nChars;
                for( ;; )
                {
                    BYTE convert[4];
                    wchar_t wc = pMallocW [ nScan ];
                    if( wc <= 0x007f )
                    {
                        //  0x00000000 - 0x0000007F: 0xxxxxxx
                        convert[0] = ( BYTE ) wc;
                        pAsb->Addn( convert, 1 );
                    }
                    else if( wc <= 0x07ff )
                    {
                        //  0x00000080 - 0x000007FF: 110xxxxx 10xxxxxx
                        convert[0] = ( BYTE ) ( ( ( wc >> 6 ) & 0x1f ) | 0xc0 );
                        convert[1] = ( BYTE ) ( ( wc & 0x3f ) | 0x80 );
                        pAsb->Addn( convert, 2 );
                    }
                    else
                    {
                        //  0x00000800 - 0x0000FFFF: 1110xxxx 10xxxxxx 10xxxxxx
                        convert[0] = ( BYTE ) ( ( ( wc >> 12 ) & 0x0f ) | 0xe0 );
                        convert[1] = ( BYTE ) ( ( ( wc >> 6 ) & 0x3f ) | 0x80 );
                        convert[2] = ( BYTE ) ( ( wc & 0x3f ) | 0x80 );
                        pAsb->Addn( convert, 3 );
                    }

                    if( ++ nScan == nStop )
                        break;
                }
            }
            if( nScan == nMallocW )
                break;

            // If that did not reach nMallocW due to
            // no more IDXs and excessive Atop, then
            // we will insert an <A> style of anchor
            // surrounding source from Atop to Past;

            // Whatever Add ... progress text, or any HTML web page,
            // will have links to fruit that are web pages, or maybe
            // an Add-File page using FILE: URL, but ALL have an URL.

            pAsb->Add( ( unsigned char * ) "<a href=\"" );

            // Hmmm. Importing an HTML file with relative anchors,
            // and not yet having the BASE work done, produced an
            // invalid URL, leaving a zero here, but now is fixed.
            if( Index == 0 )
            {
                pAsb->Add( ( BYTE * ) "-x-" );
                ProgramError( L"Interleave: Index == 0" );
            }
            else
            {
                wchar_t * pMalUrl = CSolAllUrls.GetFullKey( Index );
                if( pMalUrl != NULL )
                {
                    // Can my URLs contain Unicode characters?
                    // Just in case, convert them to MBS too:
                    // Before conversion, it was just this easy:
                    // pAsb->AddnWide( pMalUrl, wcslen( pMalUrl ) );

                    // This is THREE of two places to test/convert chars.
                    size_t nTake = 0;
                    size_t nStop = wcslen( pMalUrl );
                    for( ;; )
                    {
                        BYTE convert[4];
                        wchar_t wc = pMalUrl [ nTake ];
                        if( wc <= 0x007f )
                        {
                            //  0x00000000 - 0x0000007F: 0xxxxxxx
                            convert[0] = ( BYTE ) wc;
                            pAsb->Addn( convert, 1 );
                        }
                        else if( wc <= 0x07ff )
                        {
                            //  0x00000080 - 0x000007FF: 110xxxxx 10xxxxxx
                            convert[0] = ( BYTE ) ( ( ( wc >> 6 ) & 0x1f ) | 0xc0 );
                            convert[1] = ( BYTE ) ( ( wc & 0x3f ) | 0x80 );
                            pAsb->Addn( convert, 2 );
                        }
                        else
                        {
                            //  0x00000800 - 0x0000FFFF: 1110xxxx 10xxxxxx 10xxxxxx
                            convert[0] = ( BYTE ) ( ( ( wc >> 12 ) & 0x0f ) | 0xe0 );
                            convert[1] = ( BYTE ) ( ( ( wc >> 6 ) & 0x3f ) | 0x80 );
                            convert[2] = ( BYTE ) ( ( wc & 0x3f ) | 0x80 );
                            pAsb->Addn( convert, 3 );
                        }

                        if( ++ nTake == nStop )
                            break;
                    }

                    MyFree( 943, zx, pMalUrl );
                    pMalUrl = NULL;
                }
            }

            pAsb->Add( ( unsigned char * ) "\">" );

            nLimit = Past;
            if( nLimit > nMallocW )
                nLimit = nMallocW;

            nChars = nLimit - nScan;
            if( nChars < 0 )
            {
                ProgramError( L"Interleave: nLimit < nScan 2" );
                break;
            }
            if( nChars > 0 )
            {
                // This is two of two places to test/convert chars.
                // Before conversion, it was just this easy:
                // pAsb->AddnWide( pMallocW + nScan, nChars );
                // nScan += nChars;

                size_t nStop = nScan + nChars;
                for( ;; )
                {
                    BYTE convert[4];
                    wchar_t wc = pMallocW [ nScan ];
                    if( wc <= 0x007f )
                    {
                        //  0x00000000 - 0x0000007F: 0xxxxxxx
                        convert[0] = ( BYTE ) wc;
                        pAsb->Addn( convert, 1 );
                    }
                    else if( wc <= 0x07ff )
                    {
                        //  0x00000080 - 0x000007FF: 110xxxxx 10xxxxxx
                        convert[0] = ( BYTE ) ( ( ( wc >> 6 ) & 0x1f ) | 0xc0 );
                        convert[1] = ( BYTE ) ( ( wc & 0x3f ) | 0x80 );
                        pAsb->Addn( convert, 2 );
                    }
                    else
                    {
                        //  0x00000800 - 0x0000FFFF: 1110xxxx 10xxxxxx 10xxxxxx
                        convert[0] = ( BYTE ) ( ( ( wc >> 12 ) & 0x0f ) | 0xe0 );
                        convert[1] = ( BYTE ) ( ( ( wc >> 6 ) & 0x3f ) | 0x80 );
                        convert[2] = ( BYTE ) ( ( wc & 0x3f ) | 0x80 );
                        pAsb->Addn( convert, 3 );
                    }

                    if( ++ nScan == nStop )
                        break;
                }
            }

            pAsb->Add( ( unsigned char * ) "</a>" );

            if( nScan == nMallocW )
                break;

            // If we got to this point in outer for loop,
            // we used up one IDX slot, and need another:

            nLookup ++;
            pFruit->pIdxResultIndex->GetIdx( nLookup, & Atop, & Past, & Index, & Offset );
        }

        // having scanned and copied it all,
        // we are done with the source text:

        MyFree( 981, zx, pMallocW );
        pMallocW = NULL;
    }

    // Now get the buffer and delete CAsb myself.
    BYTE * pMallocA = pAsb->GetBuffer( nMallocBuf );
    delete pAsb;
    pAsb = NULL;
    return pMallocA; // Can be NULL, my caller test for NULL.
}


size_t CPag::ProcessPaper( size_t UrlIndex, COnePaper * pOnePaper, wchar_t * pInputBuffer, size_t nInputSize, int AsText, CSol * pSolFrames, CSol * pSolMores, CSol * pSolHits, COneSurl * pOneSurl )
{
    // CDOC shows "recursion" of the classes CHtm & CTxt with CBud.
    // To break that, I will pass elements of pOnePaper into parse.
    // CHtm and CTxt will not even get at CBud's pOnePaper pointer.

    // CFio ( Add File, Add Folder ) will pass UrlIndex == NULL.
    // No, old news. CFio hung/passed either File: or Http: URL.

    // CWww will pass non-NULL UrlIndex of HTML/TEXT Cache or Fetch.

    // CFio Add File will pass a NULL for pBudLog. Don't freak out.
    // No, ProcessPaper will receive no pBudLog, rather, pWsbNotes.
    // Add File will simply delete a pWsbNotes that he will ignore.
    // I have since made that a member pWsbAnnotation of COnePaper.

    // I will parse the whole input text at once, receive back any
    // new URL found, and the annotations, assemble them atop the
    // output text, and hang the finished pOnePaper into CSolAllUrls.

    // May return a newbaseindex if found, else the one passed in.
    size_t NewBaseIndex = UrlIndex; // affects CFio Add File caller.

    #if DO_DEBUG_BASE
        ; SpewValue( L"CPag: ProcessPaper: Passed in UrlIndex", NewBaseIndex );
        {
            wchar_t * p = ( wchar_t * ) CSolAllUrls.GetFullKey( NewBaseIndex );
            ; Spew( p );
            MyFree( 952, UNPREDICTABLE, p );
        }
    #endif

    #if DO_DEBUG_PAPER
    {
        wchar_t wk[200];
        wsprintf( wk, L"ProcessPaper: URL %8d, pp %08x, pI %08x, nI %8d, Txt %d",
            UrlIndex,
            pOnePaper,
            pInputBuffer,
            nInputSize,
            AsText );
        ; Spew( wk );
    }
    #endif


    // AsText is really two bools that I should split apart:
    // Bit 1 means: DO NO HTML PARSE, input text is exact.
    // That is if file extension or http header indicated text.
    //
    // Otherwise, bit 2 means during HTML parse, honor both
    // newlines ( 1 or 2 ) and indentation ( .txt / PRE ) input.
    // That is if my idiomatic header is recognized, and is
    // only for file input, because I have links to extract
    // from an otherwise well-formatted text ( as .htm file ).



    // bit 1 is set when inputting a .txt filename ( like a WordsEx file ).
    // bit 1 is set when some web resource HTTP header says text/plain.
    if( ( AsText & 1 ) != 0 ) // New strategy: skip CHtm parse totally.
    {
        // Do no HTML parse at all. Just copy input to pOnePaper->pWsbResultText
        pOnePaper->pWsbResultText->Addn( pInputBuffer, nInputSize );
        #if DO_DEBUG_BASE
            ; Spew( L"CPag: ProcessPaper: Did no HTML parse. Treated as Text." );
        #endif
    }
    else
    {
        // First, parse the HTM, as it may yield a new URL from BASE tag.
        // Note that these two passed parameters RECEIVE the html parse,
        // being the final formatted text and index to any URLS therein:

        // It's always a confusing time when imperfectly correlated
        // concepts get conflated together into a single variable.
        // Like terrible: pOnePaper->PageIsAQrpThisIsItsOrdinal !
        // The case of zero is a 'ringer' in an otherwise ordinal.
        // Further, I hang decisions how to treat tags, and whether
        // to annotate with facts, and how to format filename on it!
        // So I added another bit into the ordinal.

        int PageIsAQrp = ZERO_ORDINAL_FOR_NON_QRP;
        int FromEngineNo = ZERO_ORDINAL_FOR_NON_QRP;
        int BoolsEngineNo = pOnePaper->PageIsAQrpThisIsItsOrdinal; // two bools and ordinal

        if( BoolsEngineNo != ZERO_ORDINAL_FOR_NON_QRP )
        {
            PageIsAQrp = 1;
            FromEngineNo = BoolsEngineNo & ~ BIT_IN_ORDINAL_TO_KEEP;
        }

        #if DO_DEBUG_FACTTEST
            Spew( L"CPag::ProcessPaper - Setting up parameters..." );
            SpewValue( L"BoolsEngineNo", BoolsEngineNo );
            SpewValue( L"PageIsAQrp", PageIsAQrp );
            SpewValue( L"FromEngineNo", FromEngineNo );
            if( pOneSurl == NULL )
                Spew( L"pOneSurl == NULL" );
            else
                Spew( L"pOneSurl != NULL" );
        #endif

        // Irrelevant to me... if( BoolsEngineNo & BIT_IN_ORDINAL_TO_KEEP )...

        CHtm * pHtmParser = new CHtm( UrlIndex, pOnePaper->pWsbResultText, pOnePaper->pIdxResultIndex, pSolFrames, pSolMores, pSolHits, PageIsAQrp, pOneSurl );
        if( UrlIndex != NULL )
        {
            // For CWww.cpp, who already knew the true URL it fetched.
            // Use the actual HTTP fetch URL to resolve relative URLs.
            // I need a fresh malloc, as CHTM destructor will free it.
            pHtmParser->m_BaseUrlIndex = UrlIndex;
            pHtmParser->m_pMalBase = ( wchar_t * ) CSolAllUrls.GetFullKey( UrlIndex );

            #if DO_DEBUG_BASE
                ; Spew( L"CPag: ProcessPaper: Setting pHtmParser->m_pMalBase, and pHtmParser->m_BaseUrlIndex:" );
                ; SpewValue( ( ( pHtmParser->m_pMalBase==NULL )?pHtmParser->m_pMalBase:L"-null-" ), pHtmParser->m_BaseUrlIndex );
            #endif
        }

        if( ( AsText & 2 ) != 0 ) // When add file recognized WordsEx format
        {
            // For CFio.cpp Add File, Folder, when recognized one of my own texts.
            pHtmParser->BitMaskReasonsToHonorNewlines |= ( 1 << 1 );
            pHtmParser->BitMaskReasonsToHonorIndent |= ( 1 << 1 );
        }

        pHtmParser->ParseWideUCSInputBuffer( pInputBuffer, nInputSize ); // whole input at once
        pHtmParser->FlushParse( );

        #if DO_DEBUG_BASE
            ; Spew( L"CPag: ProcessPaper: Post-HTML-Parser pHtmParser->m_pMalBase, and pHtmParser->m_BaseUrlIndex:" );
            ; SpewValue( ( ( pHtmParser->m_pMalBase==NULL )?pHtmParser->m_pMalBase:L"-null-" ), pHtmParser->m_BaseUrlIndex );
        #endif

        // Notice that ProcessPaper will only return a new base index
        // if it will be non-zero, so callers need not test for zero.
        if( pHtmParser->m_BaseUrlIndex != 0 )
        {
            // Use the definitive URL, from a BASE_TAG.
            NewBaseIndex = pHtmParser->m_BaseUrlIndex;
            #if DO_DEBUG_BASE
                ; SpewValue( L"HTML parser returned new Base Index", NewBaseIndex );
                {
                    wchar_t * p = ( wchar_t * ) CSolAllUrls.GetFullKey( NewBaseIndex );
                    ; Spew( p );
                    MyFree( 952, UNPREDICTABLE, p );
                }
            #endif
        }
        else
        {
            #if DO_DEBUG_BASE
                ; Spew( L"HTML parser returned NO new Base Index" );
            #endif
        }

        if( pHtmParser->m_nCSetFound != 0 )
        {
            // Use charset from META_TAG / http-equiv Content-Type / etc.
            pOnePaper->HttpHeaderCharset = pHtmParser->m_nCSetFound;
        }

        if( pHtmParser->m_nLangFound != 0 )
        {
            // MAY use charset from META_TAG / http-equiv Content-Language.
            pOnePaper->HttpHeaderContentLanguage = pHtmParser->m_nLangFound;
            pOnePaper->LanguageGroup = GroupIndexForLanguageIndex( pOnePaper->HttpHeaderContentLanguage );
        }

        // Because I did not hand over the whole COnePaper object to CHtm,
        // I'll have to copy any title string from a CHtm member to paper.

        // Rather than hold an empty pWsbTitleText on each URL,
        // constructor left pWsbTitleText = NULL; I create JIT.

        // Just a minute. In the case of inputting .HTM files,
        // and CPag::Skip* recognized my header, it has a title.
        // So only add a title from the HTML parse if none now.
        if( pOnePaper->pWsbTitleText == NULL )
        {
            // old way, for reference...
            // if( pOnePaper->pWsbTitleText != NULL )
            // {
            //     CWsb * Temp = pOnePaper->pWsbTitleText; // copy before delete
            //     pOnePaper->pWsbTitleText = NULL;        // NULL before delete
            //     delete Temp;
            //     Temp = NULL; // obligatory rigor after delete
            // }
            pOnePaper->pWsbTitleText = new CWsb( );
            pOnePaper->pWsbTitleText->AddWsb( pHtmParser->m_pWsbTitleText );
        }

        delete pHtmParser;
        pHtmParser = NULL;
    }

    // Since second half of process must also be called
    // for internal texts, split off from this routine:
    // I need to set the new base now, so annotations use it.
    // Or better, leave my callers to decide, and call part 2:
    // SecondPartOfProcessForText( UrlIndex, pOnePaper );

    #if DO_DEBUG_BASE
        ; SpewValue( L"ProcessPaper returning new Base Index", NewBaseIndex );
    #endif
    return NewBaseIndex;
}

void CPag::SecondPartOfProcessForText( size_t UrlIndex, int ShowAsAQrp, COnePaper * pOnePaper )
{
    // Second, parse the pretty text for observations.

    // Since the CHtm step is already done, and I have a final
    // charset indication, make that adjustment to buffer now.
    // Many file input and HTTP resources will make no change.

    Pag.AdjustTextBufferPerCharset( pOnePaper );
    Pag.ScanForClearTextUrls( pOnePaper );

    size_t WordCount = 0;
    size_t Score = 0;

    // Outside the IF block to make a final call to get most-used words:
    CTxt * pTxtParser = NULL;

    if( ShowAsAQrp )
    {
        // Do a different header annotation for QRP's...
    }
    else
    {
        pTxtParser = new CTxt( pOnePaper );
        WordCount = pTxtParser->SimpleWordScan( ); // Tally words
        pTxtParser->LanguageGuessingWork( );
        pTxtParser->AddWordsToOtherLists( );
        Score = pTxtParser->SentenceScan( ); // Tally phrases, blocks
        // Revised methods are producing six digits! Reduce somewhat:
        Score /= 100;
    }

    // Also record this for later Find results ordering:
    pOnePaper->PageRankingScore = Score;

    // Format first line of text with URL and 1 newline:
    {
        wchar_t * pMalUrl = CSolAllUrls.GetFullKey( UrlIndex ); // a malloc, user frees
        #if DO_DEBUG_PAPER
            ; Spew( pMalUrl );
            ; Spew( L"\r\n" );
        #endif
        pOnePaper->pWsbAnnotation->Add( pMalUrl );
        pOnePaper->pWsbAnnotation->Add( L"\r\n" );
        MyFree( 952, UNPREDICTABLE, pMalUrl );
        pMalUrl = NULL;
    }

    // Format another line of text, 1 newline, like this...
    // Untitled. ( later, I will extract HTML TITLE field )
    // I want the TITLE, maybe it had always belonged here:
    if( pOnePaper->pWsbTitleText != NULL
    && pOnePaper->pWsbTitleText->StrLen > 0 )
    {
        pOnePaper->pWsbAnnotation->AddWsb( pOnePaper->pWsbTitleText );
        pOnePaper->pWsbAnnotation->Add( L"\r\n" );
    }
    else
    {
        pOnePaper->pWsbAnnotation->Add( L"Untitled\r\n" );
    }

    // Format another line of text, 1 newline, like this...
    // 2001-09-11 / English ( United States ) / us-ascii

    // Format another line of text, 1 newline, like this...
    // 77 score, 92 phrases, 1834 words, 10 kb, 569 terms, 0 links.

    wchar_t * pStaticLang = BestLanguageNameforIndex( pOnePaper->HttpHeaderContentLanguage );
    wchar_t * pStaticCSet = BestCharsetNameforIndex( pOnePaper->HttpHeaderCharset );

    wchar_t wk[200];
    wchar_t * into = wk;

    wsprintf( into, L"%04d-%02d-%02d / %s / %s\r\n%d score, %d phrases, %d words, %d kb, %d terms, %d links.\r\n",
        pOnePaper->HttpHeaderDateyyyymmdd / 10000, // yyyy
        pOnePaper->HttpHeaderDateyyyymmdd / 100 % 100, // mm
        pOnePaper->HttpHeaderDateyyyymmdd % 100, // dd
        pStaticLang,                            // ContentLanguage
        pStaticCSet,                            // charsetName
        Score,                                  // score
        pOnePaper->pIdxSentences->nSlots,       // phrases
        WordCount,                              // words
        ( pOnePaper->pWsbResultText->StrLen + 512 ) / 1024, // kb
        pOnePaper->pSolWordList->nList - 2,     // terms
        pOnePaper->pIdxResultIndex->nSlots );   // links
    pOnePaper->pWsbAnnotation->Add( wk );

    // What a convenient time to update some status globals:

    TotalPageCount   ++ ;
    TotalPhraseCount += pOnePaper->pIdxSentences->nSlots;
    TotalWordCount   += WordCount;

    // Format another line of text, with uncommon words, 1 newline:
    if( pTxtParser != NULL )
    {
        pTxtParser->MostUncommonWords( pOnePaper->pWsbAnnotation );
        delete pTxtParser;
        pTxtParser = NULL;
    }

    #if DO_DEBUG_PAPER
        ; Spew( L"CTxt ended" );
    #endif
}

int CPag::ClearTextTokenResemblesUrl( wchar_t * pToken, size_t nChars )
{
    // Be very speedy to eliminate non-urls, because I will be calling
    // this for EVERY token being added to the text during HTML parse!
    // I am only expecting to recognize absolute URLs, not relative.

    // suppose a minmum URL is "a.com". Then minimum nChars is 5.

    if( nChars < 5 )
        return 0; // 0 = cleartext does not resemble a URL.

    // So far, I have verified at least 5 characters in string.


    // Every url must have a dot. Optimize first scan to rid others.
    // Probably the system's strstr, N variant, wcs variant is best.
    // If they had one. DIY.

    int i = nChars - 1; // Do not test the final character here
    for( ;; )
    {
        if( --i == 0 ) // Do not test the first character here
            return 0; // No dot, no url

        if( pToken[i] == '.' )
        {
            // We can spend a moment more to qualify this dot.
            // Offhand, I'd say it should be between two alnums.
            // Specifially, USASCII. Narrow loop limits do CYA.
            wchar_t wc1 = pToken[i-1];
            wchar_t wc2 = pToken[i+1];
            if( wc1 <= 'z' && isalnum( wc1 )
            &&  wc2 <= 'z' && isalnum( wc2 ) )
                break;
        }
    }

    // So far, I have verified a dot between alnums somewhere in string.


    i = nChars - 2; // CYA after -- for +0,1,2
    for( ;; )
    {
        if( --i < 0 )
            break;

        if( pToken[i] == '.'
        && pToken[i+1] == '.'
        && pToken[i+2] == '.' )
        {
            // Ellipsis often occurs in URLs on query result pages.
            // Eliminate it first.
            return 0; // Ellipsis, no url
        }
    }

    // So far, I have verified no ellipsis.

    if( ( pToken[0] | ' ' ) == 'h'
    &&  ( pToken[1] | ' ' ) == 't'
    &&  ( pToken[2] | ' ' ) == 't'
    &&  ( pToken[3] | ' ' ) == 'p' )
    {
        if( pToken[4] == ':'
        &&  pToken[5] == '/'
        &&  pToken[6] == '/' )
            return 1; // accept any http://... without hesitation

        if( ( pToken[4] | ' ' ) == 's'
        &&  pToken[5] == ':'
        &&  pToken[6] == '/'
        &&  pToken[7] == '/' )
            return 1; // accept any https://... without hesitation
    }


    // Because some FTP fetches hang up WordsEx, exclude FTP here:
    // No, let it be included in the lists of URLs. Allow FTP here:

    if( ( pToken[0] | ' ' ) == 'f'
    &&  ( pToken[1] | ' ' ) == 't'
    &&  ( pToken[2] | ' ' ) == 'p'
    &&  pToken[3] == ':'
    &&  pToken[4] == '/'
    &&  pToken[5] == '/' )
        return 1; // accept any ftp://... without hesitation


    if( ( pToken[0] | ' ' ) == 'w'
    &&  ( pToken[1] | ' ' ) == 'w'
    &&  ( pToken[2] | ' ' ) == 'w'
    &&  pToken[3] == '.'
    &&  iswalnum ( pToken[4] ) )
        return 1; // accept any "www." without hesitation.

    // I rid the .com, etc rule here. Need to re-add detailed rules.
    // this will be enuf to get CHTM to list absolute exported URLs.

    return 0;
}

size_t CPag::SkipWordsExHeader( size_t * pnUrl, size_t * pnDate, size_t * pnLang, size_t * pnCSet, wchar_t * pWBuf, size_t nWBuf, COnePaper * pOnePaper )
{
    // This routine will write temporary nulls into caller's data.
    // Caller should have applied any prior conversion to Unicode.
    // My files starts something like this before body of content:

    // http://waste.org/four.html
    // Untitled
    // 1997-11-23 / Unknown / Windows-1252
    // 21 score, 34 phrases, 845 words, 5 kb, 272 terms, 2 links.
    //  number numbers complex rational natural positive decimal real called ...

    // I seem to be losing the titles on import.
    // Here is more recent ( jul 31 2007 ) example:

    // skipping the unicode introducer bytes...
    // http://en.wikipedia.org/wiki/Pearson_product-moment_correlation_coefficient
    // Pearson product-moment correlation coefficient - Wikipedia, the free encyclopedia
    // 2007-07-25 / English / utf-8
    // 36 score, 32 phrases, 714 words, 5 kb, 296 terms, 83 links.
    //  correlation coefficient pearson value calculates variables linear between relationship equation prime sum calculated measure variance moment changes standard cleanup function shows registered defined covariance

    // Whether saved as .htm or as .txt, top is identical.
    // I will seek the "score," phrase as part of my idiom.

    // If like that, canonicalize and hang the URL.
    // If like that and canonical, pass back UrlIndex.
    // If like that, extract the TITLE into OnePaper.
    // If like that, extract the language and charset.
    // If like that, return count to skip before parse.
    // I will not bother to save the old valid header,
    // but reannotate with best url and new word list.
    // If not like that, then do nothing, return zero.

    *pnUrl = 0; // in case
    *pnDate = 0;
    *pnLang = 0;
    *pnCSet = 0;

    // Scan the buffer for my header annotations.
    // If found, return char count to skip over.
    // If found, add the top line URL to AllUrls.
    // If found, set user's * pnUrl to UrlIndex.

    wchar_t * scan = pWBuf;
    wchar_t * stop = scan + nWBuf;

    if( stop > scan + MAX_MARKUP_TEXT + 1000 )
        stop = scan + MAX_MARKUP_TEXT + 1000;

    #if DO_DEBUG_INPUT
        ; Spew( L"SkipWordsEx called" );
    #endif

    wchar_t * newline[6];

    // Before Jun 20 2007:
    // newline[0] is after URL;
    // newline[1] is after date / language / charset;
    // newline[2] is after score;
    // newline[3] is after words;
    // newline[4] is after blank line;

    // After Jun 20 2007:
    // newline[0] is after URL;
    // newline[1] is after title;
    // newline[2] is after date / language / charset;
    // newline[3] is after score;
    // newline[4] is after words;
    // newline[5] is after blank line;

    int FillNL = 0;
    for( ;; )
    {
        if( scan == stop )
            break;
        if( scan[0] == '\r'
        &&  scan[1] == '\n' )
        {
            newline[FillNL] = scan;
            if( ++FillNL == 6 )
                break;
        }
        scan ++;
    }

    if( FillNL != 6 )
    {
        #if DO_DEBUG_INPUT
            ; Spew( L"SkipWordsEx FillNL != 6" );
        #endif
        return 0;
    }
    // This tests for the blank separator line after header.
    if( newline[5] != newline[4] + 2 )
    {
        #if DO_DEBUG_INPUT
            ; Spew( L"SkipWordsEx newline[5] != newline[4] + 2" );
        #endif
        return 0;
    }

    // Null terminate the first four, now five, lines while working
    newline[0][0] = NULL;
    newline[1][0] = NULL;
    newline[2][0] = NULL;
    newline[3][0] = NULL;
    newline[4][0] = NULL;

    // Here, recover the date.
    int Dated = 0;
    wchar_t * pDate = newline[1] + 2; // Past newline, at: 2001-09-11
    if( pDate[4] == '-'
    && pDate[7] == '-' )
    {
        int yr = 0;
        yr += ( pDate[0] - '0' ) * 1000;
        yr += ( pDate[1] - '0' ) * 100;
        yr += ( pDate[2] - '0' ) * 10;
        yr += ( pDate[3] - '0' ) * 1;

        int mo = 0;
        mo += ( pDate[5] - '0' ) * 10;
        mo += ( pDate[6] - '0' ) * 1;

        int dy = 0;
        dy += ( pDate[8] - '0' ) * 10;
        dy += ( pDate[9] - '0' ) * 1;

        *pnDate = ( yr * 10000 ) + ( mo * 100 ) + dy;
    }

    // I will seek the SLASH character as part of my idiom. Twice.
    // May as well parse the ( en/utf8 ) feature if present:
    // Here is a test URL, my homepage includes these meta tags:
    // http://home.earthlink.net/~glenn_scheper/
    // 0000-00-00 / English ( United States ) / us-ascii
    wchar_t * pSlash1 = NULL;
    wchar_t * pSlash2 = NULL;

    // This numbering seemed to have confused me.
    // newline[0] is AFTER the first line with URL.

    // Get title line at end of routine...

    // Now, newline[1] is AFTER the second line with TITLE.
    scan = newline[1] + 2;

    // Here is a test, my homepage includes these meta tags:
    // http://home.earthlink.net/~glenn_scheper/
    // 0000-00-00 / English ( United States ) / us-ascii
    for( ;; )
    {
        wchar_t c = *scan;
        if( c == NULL )
            break;
        if( c == '/'
        && scan[1] == ' '   // verify format: space slash space
        && scan[-1] == ' ' )
        {
            pSlash1 = scan;
            scan++;         // move past for second loop
            break;
        }
        scan++;
    }
    for( ;; )
    {
        wchar_t c = *scan;
        if( c == NULL )
            break;
        if( c == '/'
        && scan[1] == ' '   // verify format: space slash space
        && scan[-1] == ' ' )
        {
            pSlash2 = scan;
            break;
        }
        scan++;
    }

    // newline[2] is AFTER the third line with Language / charset.
    scan = newline[2] + 2;

    // I will seek the "score," phrase as part of my idiom.
    int Scored = 0;

    for( ;; )
    {
        wchar_t c = *scan;
        if( c == NULL )
            break;
        if( c == ',' )
        {
            // test for the just prior word, "score"
            if( scan > pWBuf + 5 // protective test
            && scan[ -5 ] == 's'
            && scan[ -4 ] == 'c'
            && scan[ -3 ] == 'o'
            && scan[ -2 ] == 'r'
            && scan[ -1 ] == 'e' )
            {
                Scored = 1;
            }
            break;
        }
        scan++;
    }

    if( pSlash1 == NULL
    || pSlash2 == NULL
    || Scored == 0 )
    {
        // repair the first four, now five, lines' CR characters
        newline[0][0] = '\r';
        newline[1][0] = '\r';
        newline[2][0] = '\r';
        newline[3][0] = '\r';
        newline[4][0] = '\r';
        #if DO_DEBUG_INPUT
            ; Spew( L"SkipWordsEx No Slash, Score" );
        #endif
        return 0;
    }

    // Extract the Contentlanguage and charset for Add File caller.

    pSlash2[-1] = NULL;
    *pnLang = BestIndexforLanguageString( pSlash1 + 2, NULL );

    #if DO_DEBUG_LANGUAGE
        ; Spew( L"Extracted language from atop WordsEx idiomatic file:" );
        ; SpewValue( pSlash1 + 2, *pnLang );
    #endif

    *pnCSet = BestIndexforCharsetString( pSlash2 + 2, NULL );
    // restore
    pSlash2[-1] = ' ';

    // Extract the top line URL for Add File caller.
    int FoundUrl = 0;
    int TopUrlTypeIsFile = 0;
    int TopUrlTypeIsCanonical = 0;

    // I do not actually re-use the old word list line,
    // rather I will re-parse the rest of body of text.

    // Canonicalized URLs have lowercase methods
    // test for: http:/, https:/, ftp:/, file:/
    scan = pWBuf;

    if( ( scan[0] == 'h'
        && scan[1] == 't'
        && scan[2] == 't'
        && scan[3] == 'p'
        && ( scan[4] == ':' && scan[5] == '/'
            || scan[4] == 's' && scan[5] == ':' && scan[6] == '/' ) )
    || (
        scan[0] == 'f'
        && scan[1] == 't'
        && scan[2] == 'p'
        && scan[3] == ':'
        && scan[4] == '/' )
    || (
        ( TopUrlTypeIsFile = 1 ),
        (
        scan[0] == 'f'
        && scan[1] == 'i'
        && scan[2] == 'l'
        && scan[3] == 'e'
        && scan[4] == ':'
        && scan[5] == '/' ) )
    ) // close enough for me...
    {
        if( ! TopUrlTypeIsFile )
        {
            // re-canonicalize.
            wchar_t * pMalCanonized = NULL;
            // returns 1=success, ( 0=failure, -1=fail/stay in dialog )
            if( Www.FixUpUserUrl( & pMalCanonized, scan ) == 1 )
            {
                // hang this url, and report back new UrlIndex.

                int UrlIndex = CSolAllUrls.AddKey( pMalCanonized );
                #if DO_DEBUG_ADDFIND
                    if( UrlIndex == 1 )
                        { Spew( L"AddFind 1 at call 1011" ); }
                #endif
                #if DO_DEBUG_INPUT
                    ; SpewValue( L"Cfio Hung URL, #", UrlIndex );
                    ; Spew( pMalCanonized );
                #endif
                *pnUrl = UrlIndex;
            }
            if( pMalCanonized != NULL )
            {
                MyFree( 1013, UNPREDICTABLE, pMalCanonized );
                pMalCanonized = NULL;
            }
        }
        // Having the typical case of a web page, hung,
        // all hunky dorey, now extract the title line.

        // newline[0] is AFTER the first line with URL.
        wchar_t * pTitle = newline[0] + 2; // Past newline, at: Untitled, or...
        // Oh, wait, do I have a pOnePaper yet? Yes. No. Passed it. Now I do.

        #if DO_DEBUG_INPUT
            ; SpewTwo( L"Ready to do title", pTitle );
        #endif

        // Rather than hold an empty pWsbTitleText on each URL,
        // constructor left pWsbTitleText = NULL; I create JIT.
        if( pOnePaper->pWsbTitleText != NULL )
        {
            #if DO_DEBUG_INPUT
                ; Spew( L"Removing an existing pOnePaper->pWsbTitleText" );
            #endif
            CWsb * Temp = pOnePaper->pWsbTitleText; // copy before delete
            pOnePaper->pWsbTitleText = NULL;        // NULL before delete
            delete Temp;
            Temp = NULL; // obligatory rigor after delete
        }

        #if DO_DEBUG_INPUT
            ; Spew( L"Making a new pOnePaper->pWsbTitleText" );
        #endif

        pOnePaper->pWsbTitleText = new CWsb( );
        pOnePaper->pWsbTitleText->Add( pTitle );

        // Hmmm, somebody after me must have thrown it away!
    }
    else
    {
        // repair the first four, now five, lines' CR characters
        newline[0][0] = '\r';
        newline[1][0] = '\r';
        newline[2][0] = '\r';
        newline[3][0] = '\r';
        newline[4][0] = '\r';
        #if DO_DEBUG_INPUT
            ; Spew( L"SkipWordsEx ! No URL" );
        #endif
        return 0;
    }

    // repair the first four, now five, lines' CR characters
    newline[0][0] = '\r';
    newline[1][0] = '\r';
    newline[2][0] = '\r';
    newline[3][0] = '\r';
    newline[4][0] = '\r';

    #if DO_DEBUG_INPUT
        ; Spew( L"SkipWordsEx success" );
    #endif

    // This is the count of the whole header, including
    // the final two newlines, to ignore to reach body:
    return ( newline[5] + 2 ) - pWBuf;
}

wchar_t * CPag::GetWordsExHeader( size_t UrlIndex )
{
    // This is for when fetch re-encounters an old URL.
    // Skim off just the top lines and 1 final newline.
    // Call can handle a NULL return due to any errors.

    if( UrlIndex < 2 )
    {
        #if DO_DEBUG_FETCH
            ; Spew( L"GetWordsExHeader: UrlIndex < 2" );
        #endif
        return NULL;
    }
    COnePaper * pOnePaper = ( COnePaper * ) CSolAllUrls.GetUserpVoid( UrlIndex );
    if( pOnePaper == NULL )
    {
        #if DO_DEBUG_FETCH
            ; Spew( L"GetWordsExHeader: pOnePaper == NULL" );
        #endif
        return NULL;
    }

    CWsb * pWsbRes = pOnePaper->pWsbResultText;
    if( pWsbRes == NULL )
    {
        #if DO_DEBUG_FETCH
            ; Spew( L"GetWordsExHeader: pWsbRes == NULL" );
        #endif
        return NULL;
    }

    size_t nMalAll = 0;
    wchar_t * pMalAll = pWsbRes->GetBuffer( & nMalAll );

    if( pMalAll == NULL )
    {
        #if DO_DEBUG_FETCH
            ; Spew( L"GetWordsExHeader: pMalAll == NULL" );
        #endif
        return NULL;
    }

    // My caller is about to free this after using it,
    // so just set NULL on second of a double newline.

    wchar_t * scan = pMalAll;
    wchar_t * stop = scan + nMalAll;
    for( ;; )
    {
        if( scan == stop )
            break;
        if( scan[0] == '\r'
        &&  scan[1] == '\n'
        &&  scan[2] == '\r'
        &&  scan[3] == '\n' )
        {
            scan[2] = NULL;
            break;
        }
        scan ++;
    }

    return pMalAll;
}

int CPag::DislikeDueToDataContent( wchar_t * pWBuf, size_t nWBuf )
{
    #if DO_DEBUG_CALLS
        Routine( L"327" );
    #endif

    wchar_t * scan = pWBuf;
    wchar_t * stop = pWBuf + nWBuf;
    if( nWBuf > 10000 )
        stop = pWBuf + 10000;

    #if DO_DEBUG_REFUSAL
        Spew( L"Categoring. Entered Dislike routine..." );
    #endif

    // prejudice against: Javascript
    // recognize it by some count of lower case "var" keywords.
    // Qualify every file containing an "<HTML..." tag as okay.
    // Qualify on prior <script language="JavaScript"> as okay.

    int nVar = 0;

    for( ;; )
    {
        if( scan == stop )
            break;

        // NULL in pWBuf protects these accesses[1,2,...]:

        if( scan[0] == '<'
        &&  ( scan[1] | ' ' ) == 'h'
        &&  ( scan[2] | ' ' ) == 't'
        &&  ( scan[3] | ' ' ) == 'm'
        &&  ( scan[4] | ' ' ) == 'l' )
        {
            #if DO_DEBUG_BINARIES
                ; Spew( L"Stopping scan on <HTML... tag." );
            #endif
            return 0; // Any file containing "<HTML..." is okay.
        }

        // this put back into the cache too many undesirable files:
        // if( scan[0] == '<'
        // &&  ( scan[1] | ' ' ) == 's'
        // &&  ( scan[2] | ' ' ) == 'c'
        // &&  ( scan[3] | ' ' ) == 'r'
        // &&  ( scan[4] | ' ' ) == 'i'
        // &&  ( scan[5] | ' ' ) == 'p'
        // &&  ( scan[6] | ' ' ) == 't' )
        // {
        //     #if DO_DEBUG_BINARIES
        //         ; Spew( L"Stopping scan on <SCRIPT... tag." );
        //     #endif
        //     return 0; // Any file containing "<SCRIPT..." is okay.
        // }

        if( scan[0] == 'v'
        &&  scan[1] == 'a'
        &&  scan[2] == 'r'
        && ! iswalnum( scan[3] )
        && ( scan == pWBuf  // prevents -1 access:
           || ! iswalnum( scan[-1] ) )
        )
        {
            nVar ++;
        }

        scan ++;
    }

    // I find that some of my own saved Query Result Pages are refused.
    // The reason was that "var" appeared in the clear text I created.
    // They were indeed some javascript that slipped by my CHTM parser.
    // But let's restore them back in for now, and see what happens...
    // if( nVar > 1 )
    // {
    //     #if DO_DEBUG_REFUSAL
    //         ; SpewValue( L"Categoring. Dislike file due to JavaScript var keywords", nVar );
    //     #endif
    //     return 1; // Dislike JavaScript
    // }


    // Go back to re-examine the top few bytes:

    scan = pWBuf;


    // prejudice against: XML ( only after not finding <HTML tag )
    // recognize it by this exact starting sequence:
    // <?xml ...

    if( scan[0] == '<'
    &&  scan[1] == '?'
    &&  scan[2] == 'x'
    &&  scan[3] == 'm'
    &&  scan[4] == 'l' )
    {
        #if DO_DEBUG_REFUSAL
            Spew( L"Categoring. Dislike file due to <?xml marker atop file." );
        #endif
        return 1; // Dislike XML
    }


    // prejudice against: PDF
    // recognize it by this exact starting sequence:
    // %PDF...

    scan = pWBuf;
    if( scan[0] == '%'
    &&  scan[1] == 'P'
    &&  scan[2] == 'D'
    &&  scan[3] == 'F' )
    {
        #if DO_DEBUG_REFUSAL
            Spew( L"Categoring. Dislike file due to %PDF marker atop file." );
        #endif
        return 1; // Dislike PDF
    }


    // prejudice against: GIF
    // recognize it by this exact starting sequence:
    // GIF8...

    scan = pWBuf;
    if( scan[0] == 'G'
    &&  scan[1] == 'I'
    &&  scan[2] == 'F'
    &&  scan[3] == '8' )
    {
        #if DO_DEBUG_REFUSAL
            Spew( L"Categoring. Dislike file due to GIF8 marker atop file." );
        #endif
        return 1; // Dislike GIF
    }

    #if DO_DEBUG_REFUSAL
        Spew( L"Categoring. Dislike routine found no prejudice against file." );
    #endif

    return 0; // no prejudice
}

