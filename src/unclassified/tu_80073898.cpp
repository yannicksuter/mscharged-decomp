#include "Game/GameTweaks.h"
#include "NL/glx/GXMaterialShadowTweaks.h"
#include "NL/nlFile.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"
#include "NL/nlTicker.h"
#include "unclassified/tu_80073898.h"

#include <stdlib.h>

extern "C" void fn_802C7018(
    void*, char*, unsigned long, const char*);

GXMaterialColourTweak_804FC520::~GXMaterialColourTweak_804FC520()
{
}

void GXMaterialColourTweak_804FC520::UnidentifiedVirtual2C(
    TweakValueBase_8052BF70* other)
{
    switch (other->UnidentifiedVirtual10())
    {
    case 1:
        value = ((GXMaterialColourTweak_804FC520*)other)->value;
        break;
    case 2:
        value = *((TweakValueIntImpl_804FD898*)other)->m_pValue;
        break;
    }
}

int GXMaterialColourTweak_804FC520::UnidentifiedVirtual10()
{
    return 1;
}

int GXMaterialColourTweak_804FC520::UnidentifiedVirtual0C()
{
    return 3;
}

void* GXMaterialColourTweak_804FC520::UnidentifiedVirtual20()
{
    return &value;
}

void GXMaterialColourTweak_804FC520::UnidentifiedVirtual18()
{
}

void GXMaterialColourTweak_804FC520::UnidentifiedVirtual28(
    const char* string)
{
    value = atoi(string);
}

extern "C" void fn_800739F0(
    void* fileData, unsigned long fileSize, void* userData)
{
    UnidentifiedTweakLoadEntry_8056BA04* entry =
        (UnidentifiedTweakLoadEntry_8056BA04*)userData;

    entry->mFileData = fileData;
    entry->mFileSize = fileSize;
    ((char*)fileData)[fileSize] = '\0';
    entry->mLoadEnd = nlGetTicker();
    entry->mLoadTime =
        nlGetTickerDifference(entry->mLoadStart, entry->mLoadEnd);
    entry->mState = 1;
}

extern "C" void fn_80073A48(UnidentifiedTweakLoadState* state,
    const char* fileName, const char* category)
{
    UnidentifiedTweakLoadEntry_8056BA04* entry =
        &state->mEntries[state->mCount];

    nlStrNCpy<char>(entry->mFileName, fileName, sizeof(entry->mFileName));
    nlStrNCpy<char>(entry->mCategory, category, sizeof(entry->mCategory));

    entry->mFileData = 0;
    entry->mFileSize = 0;
    entry->mLoadStart = nlGetTicker();
    entry->mLoadEnd = 0;
    entry->mParseStart = 0;
    entry->mParseEnd = 0;
    entry->mLoadTime = 0.0f;
    entry->mUnidentified09C = 0.0f;
    entry->mUnidentified0A0 = 0.0f;
    entry->mState = 0;

    nlFile* file = nlOpen(fileName);
    unsigned int bufferSize = 0;
    unsigned long fileSize = nlFileSize(file, &bufferSize);
    nlClose(file);

    bufferSize += bufferSize == fileSize ? 0x20 : 0;

    void* buffer = nlMalloc(bufferSize, 0x20, true);
    nlLoadEntireFileAsync(fileName, fn_800739F0, entry, 0x20, AllocateEnd,
        buffer, bufferSize, 0);
    ++state->mCount;
}

extern "C" bool fn_80073BC0(void* loadState)
{
    UnidentifiedTweakLoadState* state =
        (UnidentifiedTweakLoadState*)loadState;
    UnidentifiedTweakLoadEntry_8056BA04* entry = state->mEntries;
    int completed = 0;

    for (int i = 0; i < state->mCount; ++i)
    {
        switch (entry->mState)
        {
        case 1:
            entry->mParseStart = nlGetTicker();
            entry->mUnidentified09C = nlGetTickerDifference(
                entry->mLoadEnd, entry->mParseStart);
            fn_802C7018(entry, (char*)entry->mFileData,
                entry->mFileSize, entry->mCategory);
            nlFree(entry->mFileData);
            entry->mFileData = 0;
            entry->mParseEnd = nlGetTicker();
            entry->mUnidentified0A0 = nlGetTickerDifference(
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

    return completed == state->mCount;
}
