#include "NL/glx/glxFont.h"
#include "NL/glx/glxTexture.h"

#include <string.h>

extern "C"
{
    PlatTexture* fn_8036B730(bool permanent);
    void fn_8036B8B4(
        PlatTexture* texture, unsigned long width, unsigned long height,
        eGXTextureFormat format, bool permanent, int numLevels,
        bool bLinearData, bool bNewResourceMemory);
    void fn_8036B998(PlatTexture* texture, bool bDeleteLinear);
    void fn_8036BA00(PlatTexture* texture);
}

u32 glplatCreateFont(
    unsigned long width, unsigned long height, const unsigned short* data,
    unsigned long handle, bool permanent)
{
    PlatTexture* pTex = fn_8036B730(permanent);
    fn_8036B8B4(
        pTex, width, height, GXTex_RGB5A3, permanent, 1, true, false);
    memcpy(pTex->m_LinearData, data, height * (width * 2));
    fn_8036B998(pTex, true);
    fn_8036BA00(pTex);
    return (u32)pTex;
}
