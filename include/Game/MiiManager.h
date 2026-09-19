#ifndef GAME_MII_MANAGER_H
#define GAME_MII_MANAGER_H

#include <RVLFaceLib/RFL_MiddleDatabase.h>

class PlatTexture;

struct MiiManager
{
    MiiManager();
    virtual ~MiiManager();

    static MiiManager* Instance();
    unsigned long GetIconTextureId(int index);

    void LoadResources();
    void LoadResources(void* buffer);
    bool CreateIcon(int index, int slot, RFLExpression value);
    bool CreateIcon(const RFLStoreData* data, int slot, RFLExpression value);
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

extern MiiManager* g_pMiiManager;

#endif // GAME_MII_MANAGER_H
