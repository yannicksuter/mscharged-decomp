#include "Game/GameTweaks.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "NL/glx/GXMaterialShadowTweaks.h"
#include "NL/nlTicker.h"

#include <stdlib.h>

extern "C" bool fn_80073BC0(void*);

struct UnidentifiedTweakLoadEntry_8056BA04
{
    char mUnidentified000[0x80];
    void* mFileData;
    unsigned long mFileSize;
    u32 mLoadStart;
    u32 mLoadEnd;
    u32 mParseStart;
    u32 mParseEnd;
    float mLoadTime;
    float mUnidentified09C;
    float mUnidentified0A0;
    u32 mState;
};

struct UnidentifiedTweakLoadState
{
    UnidentifiedTweakLoadState()
        : mCount(0)
    {
    }

    u32 mCount;
    u8 mEntries[32][0xA8];
};

UnidentifiedTweakLoadState lbl_8056BA00;
unk_8056CF08 lbl_8056CF08;

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

extern "C" bool fn_80074198(void*)
{
    return fn_80073BC0(&lbl_8056BA00);
}

template struct UnidentifiedStaticStorage<UnidentifiedStaticTag>;
