#include "Game/TweakFileLoader.h"

#include "Game/TweakConfig.h"
#include "Game/TweakValueInt.h"
#include "NL/nlFile.h"
#include "NL/nlMemory.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"
#include "NL/nlTicker.h"

#include <stdlib.h>
#include "NL/nlstring_tmpl.h"

extern const float kGameTweakZero;

TweakValueInt::~TweakValueInt()
{
}

void TweakValueInt::UnidentifiedVirtual2C(
    TweakValueBase* other)
{
    switch (other->UnidentifiedVirtual10())
    {
    case 1:
        value = ((TweakValueInt*)other)->value;
        break;
    case 2:
        value = *((TweakIntBinding*)other)->m_pValue;
        break;
    }
}

int TweakValueInt::UnidentifiedVirtual10()
{
    return 1;
}

int TweakValueInt::UnidentifiedVirtual0C()
{
    return 3;
}

void* TweakValueInt::UnidentifiedVirtual20()
{
    return &value;
}

void TweakValueInt::UnidentifiedVirtual24(
    char* buffer, unsigned long size)
{
    nlSNPrintf(buffer, size, "%d", value);
}

void TweakValueInt::UnidentifiedVirtual28(
    const char* string)
{
    value = atoi(string);
}

void TweakValueInt::UnidentifiedVirtual14(
    float* minimum, float* maximum, float* increment)
{
    *minimum = kGameTweakZero;
    *maximum = kGameTweakZero;
    *increment = kGameTweakZero;
}

void TweakValueInt::UnidentifiedVirtual18()
{
}

void OnTweakFileLoaded(
    void* fileData, unsigned long fileSize, void* userData)
{
    TweakLoadEntry* entry =
        (TweakLoadEntry*)userData;

    entry->mFileData = fileData;
    entry->mFileSize = fileSize;
    ((char*)fileData)[fileSize] = '\0';
    entry->mLoadEnd = nlGetTicker();
    entry->mLoadTime =
        nlGetTickerDifference(entry->mLoadStart, entry->mLoadEnd);
    entry->mState = 1;
}

void TweakFileLoader::LoadFileAsync(const char* fileName, const char* category)
{
    TweakLoadEntry* entry =
        &mEntries[mCount];

    nlStrNCpy<char>(entry->mFileName, fileName, sizeof(entry->mFileName));
    nlStrNCpy<char>(entry->mCategory, category, sizeof(entry->mCategory));

    entry->mFileData = 0;
    entry->mFileSize = 0;
    entry->mLoadStart = nlGetTicker();
    entry->mLoadEnd = 0;
    entry->mParseStart = 0;
    entry->mParseEnd = 0;
    entry->mLoadTime = 0.0f;
    entry->mWaitTime = 0.0f;
    entry->mParseTime = 0.0f;
    entry->mState = 0;

    nlFile* file = nlOpen(fileName);
    unsigned int bufferSize = 0;
    unsigned long fileSize = nlFileSize(file, &bufferSize);
    nlClose(file);

    bufferSize += bufferSize == fileSize ? 0x20 : 0;

    void* buffer = nlMalloc(bufferSize, 0x20, true);
    nlLoadEntireFileAsync(fileName, OnTweakFileLoaded, entry, 0x20, AllocateEnd,
        buffer, bufferSize, 0);
    ++mCount;
}

bool TweakFileLoader::ProcessLoadedFiles()
{
    TweakLoadEntry* entry = mEntries;
    int completed = 0;

    for (int i = 0; i < mCount; ++i)
    {
        switch (entry->mState)
        {
        case 1:
            entry->mParseStart = nlGetTicker();
            entry->mWaitTime = nlGetTickerDifference(
                entry->mLoadEnd, entry->mParseStart);
            LoadTweakConfigBuffer(entry, (char*)entry->mFileData,
                entry->mFileSize, entry->mCategory);
            nlFree(entry->mFileData);
            entry->mFileData = 0;
            entry->mParseEnd = nlGetTicker();
            entry->mParseTime = nlGetTickerDifference(
                entry->mParseStart, entry->mParseEnd);
            entry->mState = 2;
        case 2:
            ++completed;
            break;
        case 0:
        default:
            break;
        }
        ++entry;
    }

    return completed == mCount;
}
