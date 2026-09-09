#ifndef GAME_TWEAK_FILE_LOADER_H
#define GAME_TWEAK_FILE_LOADER_H

#include "types.h"

struct TweakLoadEntry
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
    float mWaitTime;
    float mParseTime;
    int mState;
};

struct TweakFileLoader
{
    TweakFileLoader()
        : mCount(0)
    {
    }

    void LoadFileAsync(const char* fileName, const char* category);
    bool ProcessLoadedFiles();

    int mCount;
    TweakLoadEntry mEntries[32];
    u32 mUnidentified1504;
};

extern TweakFileLoader gTweakFileLoader;

#endif // GAME_TWEAK_FILE_LOADER_H
