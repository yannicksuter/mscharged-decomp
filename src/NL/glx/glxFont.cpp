#include "NL/glx/glxFont.h"
#include "NL/glx/glxTexture.h"

#include <string.h>

u32 glplatCreateFont(
    unsigned long width, unsigned long height, const unsigned short* data,
    unsigned long handle, bool permanent)
{
    PlatTexture* pTex = glx_CreatePlatTexture(permanent);
    pTex->Create(width, height, GXTex_RGB5A3, permanent, 1, true, false);
    memcpy(pTex->m_LinearData, data, height * (width * 2));
    pTex->Swizzle(true);
    pTex->Prepare();
    return (u32)pTex;
}
