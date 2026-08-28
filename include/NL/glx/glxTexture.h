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
    /* 0x00 */ u8 _unk0[0x14];
    /* 0x14 */ void* m_SwizzledData;
};

PlatTexture* glx_GetTex(unsigned long handle);

#endif // _GLXTEXTURE_H_
