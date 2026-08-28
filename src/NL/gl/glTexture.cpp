#include "NL/gl/glTexture.h"

extern "C"
{
    extern void* lbl_806E1F08;

    unsigned long fn_802CE1B8(void* manager, unsigned long texture);
    void* fn_802CE294(void* manager, const unsigned long* texture);
    bool fn_8036B5C8(void* texture);
    u32 fn_8036B620();
    u32 fn_8036B62C();
    int fn_8036B63C(int component);
}

bool glTextureLoad(unsigned long texture)
{
    unsigned long result = fn_802CE1B8(lbl_806E1F08, texture);
    if (result == 0xFFFF)
        return false;

    unsigned long key = result;
    return fn_8036B5C8(fn_802CE294(lbl_806E1F08, &key));
}

u32 glTextureGetWidth()
{
    return fn_8036B620();
}

u32 glTextureGetHeight()
{
    return fn_8036B62C();
}

int glTextureGetNumBits(int component)
{
    return fn_8036B63C(component);
}
