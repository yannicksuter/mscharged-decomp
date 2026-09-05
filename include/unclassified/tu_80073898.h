#ifndef UNCLASSIFIED_TU_80073898_H
#define UNCLASSIFIED_TU_80073898_H

#include "types.h"

struct UnidentifiedTweakLoadEntry_8056BA04
{
    char mFileName[0x40];
    char mCategory[0x40];
    void* mFileData;
    unsigned long mFileSize;
    u32 mLoadStart;
    u32 mLoadEnd;
    u32 mParseStart;
    u32 mParseEnd;
    float mLoadTime;
    float mUnidentified09C;
    float mUnidentified0A0;
    int mState;
};

struct UnidentifiedTweakLoadState
{
    UnidentifiedTweakLoadState()
        : mCount(0)
    {
    }

    int mCount;
    UnidentifiedTweakLoadEntry_8056BA04 mEntries[32];
    u32 mUnidentified1504;
};

extern UnidentifiedTweakLoadState lbl_8056BA00;

#endif // UNCLASSIFIED_TU_80073898_H
