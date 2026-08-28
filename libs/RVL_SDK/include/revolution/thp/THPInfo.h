#ifndef REVOLUTION_THP_THPINFO_H
#define REVOLUTION_THP_THPINFO_H

#include <revolution/types.h>

typedef struct THPVideoInfo
{
    u32 xSize;
    u32 ySize;
    u32 videoType;
} THPVideoInfo;

typedef struct THPFrameCompInfo
{
    u32 numComponents;
    u8 frameComp[16];
} THPFrameCompInfo;

typedef struct THPAudioInfo
{
    u32 sndChannels;
    u32 sndFrequency;
    u32 sndNumSamples;
    u32 sndNumTracks;
} THPAudioInfo;

#endif // REVOLUTION_THP_THPINFO_H
