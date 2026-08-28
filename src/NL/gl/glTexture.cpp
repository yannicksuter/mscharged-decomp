#include "NL/gl/glTexture.h"
#include "NL/glx/glxTexture.h"

extern "C"
{
    extern void* lbl_806E1F08;

    unsigned long fn_802CE1B8(void* manager, unsigned long texture);
    void* fn_802CE294(void* manager, const unsigned long* texture);
}

bool glTextureLoad(unsigned long texture)
{
    unsigned long result = fn_802CE1B8(lbl_806E1F08, texture);
    if (result == 0xFFFF)
        return false;

    unsigned long key = result;
    return glplatTextureLoad((PlatTexture*)fn_802CE294(lbl_806E1F08, &key));
}

u32 glTextureGetWidth()
{
    return glplatTextureGetWidth();
}

u32 glTextureGetHeight()
{
    return glplatTextureGetHeight();
}

int glTextureGetNumBits(int component)
{
    return glplatTextureGetNumBits(component);
}
