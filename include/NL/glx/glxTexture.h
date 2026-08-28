#ifndef _GLXTEXTURE_H_
#define _GLXTEXTURE_H_

#include "types.h"

enum eGXTextureFormat
{
    GXTex_RGB565 = 0,
    GXTex_RGB5A3 = 1,
    GXTex_CMPR = 2,
    GXTex_RGBA8 = 3,
    GXTex_I8 = 4,
    GXTex_I4 = 5,
    GXTex_A8 = 6,
    GXTex_IA8 = 7,
    GXTex_CI8 = 8,
    GXTex_Num = 9,
};

class PlatTexture
{
public:
    /* 0x00 */ u16 m_Width;
    /* 0x02 */ u16 m_Height;
    /* 0x04 */ u8 m_Levels;
    /* 0x05 */ u8 m_MaxLevel;
    /* 0x06 */ u8 _pad06[2];
    /* 0x08 */ eGXTextureFormat m_Format;
    /* 0x0C */ s16 m_nPaletteEntries;
    /* 0x0E */ u16 unknown0E;
    /* 0x10 */ bool m_bMissingTexture;
    /* 0x11 */ u8 _pad11[3];
    /* 0x14 */ void* m_SwizzledData;
    /* 0x18 */ void* m_LinearData;
    /* 0x1C */ u16* m_PaletteData;
    /* 0x20 */ u8 m_Bits[4];
    /* 0x24 */ u32 m_TexObj[8];
    /* 0x44 */ u32 m_TlutObj[3];
};

PlatTexture* glx_GetTex(unsigned long handle);

#endif // _GLXTEXTURE_H_
