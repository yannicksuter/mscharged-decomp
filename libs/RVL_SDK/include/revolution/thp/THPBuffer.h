#ifndef REVOLUTION_THP_THPBUFFER_H
#define REVOLUTION_THP_THPBUFFER_H

#include <revolution/types.h>

typedef struct THPReadBuffer
{
    u8* ptr;
    s32 frameNumber;
    volatile BOOL isValid;
} THPReadBuffer;

typedef struct THPTextureSet
{
    u8* ytexture;
    u8* utexture;
    u8* vtexture;
    s32 frameNumber;
} THPTextureSet;

typedef struct THPAudioBuffer
{
    s16* buffer;
    s16* curPtr;
    u32 validSample;
} THPAudioBuffer;

#endif // REVOLUTION_THP_THPBUFFER_H
