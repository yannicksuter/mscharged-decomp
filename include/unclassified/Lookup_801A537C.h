#ifndef _LOOKUP_801A537C_H_
#define _LOOKUP_801A537C_H_

#include "NL/nlColour.h"

class PlatTexture;

class Lookup_801A537C
{
public:
    Lookup_801A537C();
    ~Lookup_801A537C();
    void fn_801A53F0(unsigned int textureHandle);
    nlColour fn_801A54D0(int x, int y, bool tint) const;
    nlColour fn_801A5760(float x, float y, bool tint) const;
    u8 fn_801A59EC(const PlatTexture* texture, int x, int y) const;

    /* 0x00 */ u8* mValues;
    /* 0x04 */ int mWidth;
    /* 0x08 */ int mHeight;
}; // size: 0x0C

#endif // _LOOKUP_801A537C_H_
