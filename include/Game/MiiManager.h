#ifndef GAME_MII_MANAGER_H
#define GAME_MII_MANAGER_H

#include <RVLFaceLib/RFL_MiddleDatabase.h>
#include "NL/nlSingleton.h"

class PlatTexture;

struct MiiManager : nlSingleton<MiiManager>
{
    MiiManager();
    virtual ~MiiManager();

    static MiiManager* Instance();
    unsigned long GetIconTextureId(int iconSlot) { return mIconTextureIds[iconSlot]; }

    void LoadResources();
    void LoadResources(void* buffer);
    bool CreateIcon(int officialIndex, int iconSlot, RFLExpression expression);
    bool CreateIcon(const RFLStoreData* data, int iconSlot, RFLExpression expression);
    static void ResourceLoaded(void* buffer, unsigned long size, void* userData);

    /* 0x04 */ bool mResourcesLoaded;
    /* 0x05 */ bool mInitialized;
    /* 0x06 */ unsigned char mPadding06[2];
    /* 0x08 */ void* mWorkBuffer;
    /* 0x0C */ void* mResourceArchive;
    /* 0x10 */ void* mMiddleDBBuffer;
    /* 0x14 */ void* mIconBuffers[10];
    /* 0x3C */ unsigned long mIconTextureIds[10];
    /* 0x64 */ PlatTexture* mIconTextures[10];
    /* 0x8C */ RFLMiddleDB mMiddleDB;
};

#endif // GAME_MII_MANAGER_H
