#include "NL/gl/glTexture.h"
#include "NL/glx/glxTexture.h"

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
