#ifndef REVOLUTION_OS_FONT_H
#define REVOLUTION_OS_FONT_H

#include <revolution/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum OSFontEncode {
    OS_FONT_ENCODE_ANSI,
    OS_FONT_ENCODE_SJIS,
    OS_FONT_ENCODE_2,
    OS_FONT_ENCODE_UTF8,
    OS_FONT_ENCODE_UTF16,
    OS_FONT_ENCODE_UTF32,
    OS_FONT_ENCODE_MAX,
} OSFontEncode;

typedef struct OSFontHeader {
    u16 type;
    u16 firstChar;
    u16 lastChar;
    u16 invalidChar;
    u16 ascent;
    u16 descent;
    u16 width;
    u16 leading;
    u16 cellWidth;
    u16 cellHeight;
    u32 sheetSize;
    u16 sheetFormat;
    u16 sheetNumCol;
    u16 sheetNumRow;
    u16 sheetWidth;
    u16 sheetHeight;
    u16 widthTableOfs;
    u32 sheetImageOfs;
    u32 sheetFullSize;
    u8 c0;
    u8 c1;
    u8 c2;
    u8 c3;
} OSFontHeader;

u16 OSGetFontEncode(void);
u16 OSSetFontEncode(u16 encode);
u32 OSLoadFont(OSFontHeader* font, void* destination);
const char* OSGetFontTexel(const char* string, void* destination, s32 xOffset,
                           s32 stride, u32* width);
BOOL OSInitFont(OSFontHeader* font);
const char* OSGetFontTexture(const char* string, void** texture, u32* x,
                             u32* y, u32* width);
const char* OSGetFontWidth(const char* string, u32* width);

#ifdef __cplusplus
}
#endif

#endif  // REVOLUTION_OS_FONT_H
