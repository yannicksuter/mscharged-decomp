#include "Game/MiiManager.h"

#include "NL/gl/glTexture.h"
#include "NL/MemAlloc.h"
#include "NL/gl/glMemory.h"
#include "NL/gl/glState.h"
#include "NL/glx/glxGX.h"
#include "NL/glx/glxTexture.h"
#include "NL/nlFile.h"
#include "NL/nlMemory.h"
#include "NL/nlPrint.h"

#include <RVLFaceLib/RFL_Database.h>
#include <RVLFaceLib/RFL_Icon.h>
#include <RVLFaceLib/RFL_System.h>
#include <revolution/arc.h>

template <>
MiiManager* nlSingleton<MiiManager>::s_pInstance = 0;

inline void MiiManager::LoadResources(void* buffer)
{
    unsigned long resourceSize;
    void* resource;
    bool found = false;
    ARCHandle handle;
    ARCFileInfo file;

    mResourceArchive = buffer;
    mWorkBuffer = VirtualAllocator.Allocate(RFLGetWorkSize(TRUE), 32, false);

    if (ARCInitHandle(buffer, &handle))
    {
        if (ARCOpen(&handle, RFLGetArcFilePath(), &file))
        {
            resource = ARCGetStartAddrInMem(&file);
            resourceSize = ARCGetLength(&file);
            found = true;
            ARCClose(&file);
        }
    }

    if (found)
    {
        mInitialized = RFLInitRes(
            mWorkBuffer, resource, resourceSize, TRUE) == RFLErrcode_Success;
    }
    mResourcesLoaded = true;
}

void MiiManager::ResourceLoaded(void* buffer, unsigned long size, void* userData)
{
    MiiManager::s_pInstance->LoadResources(buffer);
}

static inline void InitializeIconTextures(MiiManager& manager)
{
    for (int i = 0; i < 10; ++i)
    {
        char name[12];
        nlSNPrintf(name, sizeof(name), "mii icon %d", i);
        manager.mIconTextureIds[i] = glGetTexture(name);

        GLResourcePool* resourcePool = glGetCurrentResourcePool();
        manager.mIconTextures[i] = glx_CreatePlatTexture(resourcePool);
        glRegisterTexture(manager.mIconTextureIds[i], manager.mIconTextures[i], resourcePool);

        manager.mIconBuffers[i] = nlMalloc(0x8000, 32, false);
        manager.mIconTextures[i]->CreateWithMemory(
            128, 128, GXTex_RGB5A3, 1, manager.mIconBuffers[i]);
        manager.mIconTextures[i]->Prepare();
    }
}

MiiManager::MiiManager()
    : mResourcesLoaded(false)
    , mInitialized(false)
{
    InitializeIconTextures(*this);

    mMiddleDBBuffer = VirtualAllocator.Allocate(RFLGetMiddleDBBufferSize(1), 32, false);
    RFLInitMiddleDB(&mMiddleDB, RFLMiddleDBType_UserSet, mMiddleDBBuffer, 1);
}

MiiManager::~MiiManager()
{
    RFLExit();
    for (int i = 0; i < 10; ++i)
    {
        ::operator delete[](mIconBuffers[i]);
        mIconBuffers[i] = 0;
    }

    VirtualAllocator.Free(mWorkBuffer);
    VirtualAllocator.Free(mResourceArchive);
    VirtualAllocator.Free(mMiddleDBBuffer);
}

void MiiManager::LoadResources()
{
    char filename[32];
    nlSNPrintf(filename, sizeof(filename), "%s", "/RFLRes01.arc");
    nlLoadEntireFileAsync(filename, ResourceLoaded, 0, 32, AllocateStart, 0, 0, &VirtualAllocator);
}

bool MiiManager::CreateIcon(int officialIndex, int iconSlot, RFLExpression expression)
{
    if (officialIndex < 0)
    {
        return false;
    }

    gxInit();
    if (RFLIsAvailableOfficialData(officialIndex))
    {
        GXColor bgColor = { 0, 0, 0, 0 };
        RFLIconSetting setting;
        setting.bgType = RFLIconBG_Direct;
        setting.bgColor = bgColor;
        setting.drawXluOnly = FALSE;
        setting.width = 128;
        setting.height = 128;

        if (RFLMakeIcon(mIconBuffers[iconSlot], RFLDataSource_Official,
                0, officialIndex, expression, &setting) == RFLErrcode_Success)
        {
            gxInit();
            return true;
        }
    }
    gxInit();
    return false;
}

bool MiiManager::CreateIcon(const RFLStoreData* data, int iconSlot, RFLExpression expression)
{
    RFLUpdateMiddleDBAsync(&mMiddleDB);
    if (RFLAddMiddleDBStoreData(&mMiddleDB, data) == RFLErrcode_Success)
    {
        gxInit();
        GXColor bgColor = { 0, 0, 0, 0 };
        RFLIconSetting setting;
        setting.bgType = RFLIconBG_Direct;
        setting.bgColor = bgColor;
        setting.drawXluOnly = FALSE;
        setting.width = 128;
        setting.height = 128;

        if (RFLMakeIcon(mIconBuffers[iconSlot], RFLDataSource_Middle,
                &mMiddleDB, 0, expression, &setting) == RFLErrcode_Success)
        {
            gxInit();
            return true;
        }
        gxInit();
    }
    return false;
}
