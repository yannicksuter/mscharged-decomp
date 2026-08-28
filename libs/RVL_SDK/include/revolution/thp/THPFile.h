#ifndef REVOLUTION_THP_THPFILE_H
#define REVOLUTION_THP_THPFILE_H

#include <revolution/types.h>

typedef struct THPHeader
{
    char magic[4];
    u32 version;
    u32 bufSize;
    u32 audioMaxSamples;
    f32 frameRate;
    u32 numFrames;
    u32 firstFrameSize;
    u32 movieDataSize;
    u32 compInfoDataOffsets;
    u32 offsetDataOffsets;
    u32 movieDataOffsets;
    u32 finalFrameDataOffsets;
} THPHeader;

#endif // REVOLUTION_THP_THPFILE_H
