#ifndef _GLSTATEBUNDLE_H_
#define _GLSTATEBUNDLE_H_

#include "types.h"

struct glStateBundle
{
    /* 0x00 */ unsigned long long texturestate;
    /* 0x08 */ unsigned long raster;
    /* 0x0C */ unsigned long program;
    /* 0x10 */ unsigned long texture[6];
    /* 0x28 */ unsigned char texconfig;
    /* 0x2C */ unsigned long userStateKey;
}; // total size: 0x30

struct gl_StateBitfield
{
    /* 0x00 */ s32 startBit;
    /* 0x04 */ s32 numBits;
}; // total size: 0x8

#endif // _GLSTATEBUNDLE_H_
