#include "Game/Render/StadiumLoading.h"

#include "Game/BasicStadium.h"
#include "Game/World.h"
#include "Game/Camera/CameraMan.h"
#include "Game/DB/CharacterInfo.h"
#include "Game/DB/StadiumInfo.h"
#include "Game/Drawable/DrawableObj.h"
#include "Game/Effects/EmissionManager.h"
#include "Game/GameInfo.h"
#include "Game/Game.h"
#include "Game/NetTournManager.h"
#include "Game/Render/AttackSideIndicators.h"
#include "Game/Render/CrowdImpostors.h"
#include "Game/Render/NPCManager.h"
#include "Game/Render/RLView.h"
#include "Game/Render/StadiumTweaks.h"
#include "Game/Render/WorldNPC.h"
#include "NL/gl/gl.h"
#include "NL/gl/glMemory.h"
#include "NL/gl/glModel.h"
#include "NL/gl/glTexture.h"
#include "NL/nlCompressedFile.h"
#include "NL/nlFile.h"
#include "NL/nlMemory.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"
#include "NL/nlTask.h"
#include "unclassified/tu_801A2004.h"

#include "Game/UnidentifiedStaticStorage.h"
extern "C"
{
    void fn_80182164();
    DrawableObject* fn_8027A4BC(void* storage,
        WorldObjectLoadContext* context, glModel* model, u32 hash);
    DrawableObject* fn_8027A7F0(void* storage,
        WorldObjectLoadContext* context, glModel* model, u32 hash);
    RLView* fn_8027261C();
    void fn_802785FC(BasicStadium* stadium, float fDeltaT);
    DrawableObject* fn_802787AC(BasicStadium* stadium, unsigned long uHashID);
    void fn_80278818(BasicStadium* stadium);
    float fn_802789A0(BasicStadium* stadium);
    void fn_8027313C();

    extern char StadiumExcludedMetalShellModel[];
    extern char StadiumModelCloneNameFormat[];
    extern char StadiumResourcePathFormat[];
    extern char StadiumTextureBundlePathFormat[];
    extern char StadiumModelBundlePathFormat[];
    extern char StadiumTextureResourceLabel[];
    extern char StadiumModelResourceLabel[];
    extern StadiumLoadResult gTournamentTrophyLoadResults[3];
    extern bool lbl_806DEE60;
    extern char lbl_806DEE64[6];
    extern char lbl_805222F0[0x11];
    extern char lbl_80522304[9];
    extern bool lbl_806E1960;
    extern bool lbl_806E1961;
    extern int lbl_806E1968;
    extern StadiumTweaks* lbl_806E196C;
    extern DrawableObject* lbl_8057AB20[12];
}
void fn_8027876C(BasicStadium* stadium, DrawableObject* object);

bool gSkipGameplayModels;
void* gStadiumResourceData;
unsigned long gStadiumResourceDataSize;
bool gStadiumResourceDataLoaded;
void* gStadiumTemporaryData;
unsigned long gStadiumTemporaryDataSize;
bool gStadiumWorldLoaded;
void* gStadiumEffectsData;
unsigned int gStadiumEffectsRequest;
void* gStadiumNonResidentEffectsData;
bool gStadiumNonResidentEffectsRequested;
void* gStadiumLoadBuffers[2];
StadiumLoadResult gStadiumModelLoadResults[2][22];

bool CreatePowerupDrawables(glModel* models, unsigned long numModels)
{
    glModel* end = models + numModels;
    WorldObjectLoadContext* context
        = (WorldObjectLoadContext*)nlMalloc(sizeof(WorldObjectLoadContext), 8, true);
    if (context != 0)
    {
        new (context) WorldObjectLoadContext(pBasicStadiumInstance);
    }

    unsigned long uExcluded = nlStringHash(StadiumExcludedMetalShellModel);
    for (; models < end; models++)
    {
        if (uExcluded == models->id)
        {
            continue;
        }

        DrawableObject* pObject = (DrawableObject*)nlMalloc(0x78, 8, false);
        if (pObject != 0)
        {
            pObject = fn_8027A4BC(pObject, context, models, models->id);
        }
        pObject->m_uHashID = models->id;
        fn_8027876C(pBasicStadiumInstance, pObject);
    }

    delete context;
    return true;
}

void OnStadiumModelResourceLoaded(void* data, unsigned long size, void* userData)
{
    StadiumLoadResult* result = (StadiumLoadResult*)userData;
    result->mData = data;
    result->mSize = size;
}

DrawableObject* GetRenderObject(int entry, int instance)
{
    return gStadiumModelEntries[entry].mInstances[instance];
}

DrawableObject** fn_80276380()
{
    return lbl_8057AB20;
}

DrawableObject* GetBallRenderObject(unsigned int index)
{
    return gStadiumModelEntries[0].mInstances[index];
}

bool CreateStadiumModelInstances(int entry, glModel* models, unsigned long numModels)
{
    char name[128];
    glModel* end = models + numModels;
    WorldObjectLoadContext* context = new (8, true) WorldObjectLoadContext(pBasicStadiumInstance);

    int instance = 1;
    DrawableObject* pObject;
    if (entry == 0)
    {
        pObject = (DrawableObject*)nlMalloc(0xFC, 8, false);
        if (pObject != 0)
        {
            pObject = fn_8027A7F0(pObject, context, models, models->id);
        }
        fn_8027876C(pBasicStadiumInstance, pObject);
        instance = 0;
    }
    else
    {
        for (; models < end; models++)
        {
            pObject = (DrawableObject*)nlMalloc(0x78, 8, false);
            if (pObject != 0)
            {
                pObject = fn_8027A4BC(pObject, context, models, models->id);
            }
            fn_8027876C(pBasicStadiumInstance, pObject);
        }
    }

    gStadiumModelEntries[entry].mInstances[0] = pObject;
    for (; (unsigned long)instance < (unsigned long)gStadiumModelEntries[entry].mNumInstances;
         instance++)
    {
        nlSNPrintf(name, sizeof(name), StadiumModelCloneNameFormat, entry, instance);
        DrawableObject* pClone = pObject->Clone(nlStringLowerHash(name));
        pClone->m_uObjectFlags &= ~1;
        fn_8027876C(pBasicStadiumInstance, pClone);
        gStadiumModelEntries[entry].mInstances[instance] = pClone;
    }

    delete context;
    return true;
}

void OnStadiumResourceLoaded(void* data, unsigned long size, void* userData)
{
    gStadiumResourceData = data;
    gStadiumResourceDataSize = size;
}

void OnStadiumTemporaryResourceLoaded(void* data, unsigned long size, void* userData)
{
    gStadiumTemporaryData = data;
    gStadiumTemporaryDataSize = size;
}

void OnStadiumEffectsLoaded(void* data, unsigned long size, void* userData)
{
    *(void**)userData = data;
}

void BeginLoadStadium(const char* path, bool skipGameplayModels)
{
    char buffer[255];

    gSkipGameplayModels = skipGameplayModels;
    nlStrNCpy(gStadiumResourcePath, path, 255);

    gStadiumResourceData = 0;
    gStadiumResourceDataSize = 0;
    gStadiumResourceDataLoaded = false;
    gStadiumTemporaryData = 0;
    gStadiumTemporaryDataSize = 0;
    gStadiumWorldLoaded = false;
    gStadiumEffectsData = 0;
    gStadiumEffectsRequest = 0;
    gStadiumNonResidentEffectsData = 0;
    gStadiumNonResidentEffectsRequested = false;

    gStadiumLoadBuffers[0] = nlMalloc(0x80000, 32, true);
    gStadiumLoadBuffers[1] = (u8*)gStadiumLoadBuffers[0] + 0x40000;

    for (int i = 0; i < 22; ++i)
    {
        for (int j = 0; j < 2; ++j)
        {
            gStadiumModelLoadResults[j][i].mData = 0;
            gStadiumModelLoadResults[j][i].mSize = 0;
            gStadiumModelLoadResults[j][i].mProcessed = false;
        }
    }

    GLResourcePool* context = glGetCurrentResourcePool();
    pBasicStadiumInstance = new (8, false) BasicStadium(context);
    pBasicStadiumInstance->m_pOpaqueView = (GLView*)fn_8027261C();
    pBasicStadiumInstance->m_pAlphaView = (GLView*)GetLayerView(eCLV_WorldAlphaBlended);

    nlSNPrintf(buffer, sizeof(buffer), StadiumResourcePathFormat, gStadiumResourcePath);
    nlLoadCompressedFileAsync(buffer, OnStadiumResourceLoaded, 0, 32, AllocateStart,
        0x40000, gStadiumLoadBuffers[0], gStadiumLoadBuffers[1], 0, 0, 0);
}

bool IsStadiumResourceDataLoaded()
{
    if (!gStadiumResourceDataLoaded)
    {
        if (gStadiumResourceData != 0)
        {
            gStadiumResourceDataLoaded = true;
        }
        else
        {
            return false;
        }
    }
    return true;
}

void BeginLoadStadiumTemporaryResources()
{
    char buffer[255];
    nlSNPrintf(buffer, sizeof(buffer), "%s/gameworld.tmp.zlib", gStadiumResourcePath);
    GLResourcePool* context = glGetCurrentResourcePool();
    nlLoadCompressedFileAsync(buffer, OnStadiumTemporaryResourceLoaded, 0, 32, AllocateEnd, 0x40000,
        gStadiumLoadBuffers[0], gStadiumLoadBuffers[1], 0, 0, &VirtualAllocator);

    if (!gSkipGameplayModels)
    {
        char path[128];
        for (int i = 0; i < 22; ++i)
        {
            StadiumModelEntry& entry = gStadiumModelEntries[i];
            if (ShouldLoadStadiumModel(&entry))
            {
                nlSNPrintf(path, sizeof(path), "%s.rlt", entry.mResourceName);
                glBeginLoadTextureBundle(path, OnStadiumModelResourceLoaded, &gStadiumModelLoadResults[0][i], context);
                nlSNPrintf(path, sizeof(path), "%s.rlg", entry.mResourceName);
                glBeginLoadModel(path, OnStadiumModelResourceLoaded, &gStadiumModelLoadResults[1][i], context);
            }
        }
    }
}

void SetStadiumBannerTextures()
{
    const char* szOriginalTexture = "flag/mario_banners";
    const CharacterInfo& team = GetCharacterInfo(GetCharacterIndexFromCaptain(
        GameInfoManager::Instance()->GetTeam(0)));
    const CharacterInfo& opponent = GetCharacterInfo(GetCharacterIndexFromCaptain(
        GameInfoManager::Instance()->GetTeam(1)));
    const char* szName = team.mName;
    char buffer[64];

    if (NeedsAlternateColour(team, opponent))
    {
        nlSNPrintf(buffer, sizeof(buffer), "%s/%s_banners_alt", szName, szName);
    }
    else
    {
        nlSNPrintf(buffer, sizeof(buffer), "%s/%s_banners", szName, szName);
    }

    for (nlListIterator<ImpostorModel*> iterator
             = gpWorldNPCManager->mWorldNPCs.Begin();
         iterator.IsValid(); iterator.Next())
    {
        ImpostorModel* model = iterator.Current();
        model->mOriginalTexture = nlStringHash(szOriginalTexture);
        model->SetReplacementTexture(nlStringHash(buffer));
    }
}

void SetWorldNPCsVisible(bool visible)
{
    for (nlListIterator<ImpostorModel*> iterator
             = gpWorldNPCManager->mWorldNPCs.Begin();
         iterator.IsValid(); iterator.Next())
    {
        iterator.Current()->mVisible = visible;
    }
}

bool FinishLoadStadiumResources()
{
    if (!gStadiumWorldLoaded)
    {
        if (gStadiumTemporaryData != 0)
        {
            glDiscardFrame(1);
            pBasicStadiumInstance->LoadData(gStadiumTemporaryData, gStadiumTemporaryDataSize,
                gStadiumResourceData, gStadiumResourceDataSize, true);
            nlFree(gStadiumTemporaryData);
            gStadiumWorldLoaded = true;
        }
        else
        {
            return false;
        }
    }

    if (!gSkipGameplayModels)
    {
        for (int i = 0; i < 22; ++i)
        {
            StadiumLoadResult& textures = gStadiumModelLoadResults[0][i];
            StadiumLoadResult& geometry = gStadiumModelLoadResults[1][i];
            if (ShouldLoadStadiumModel(&gStadiumModelEntries[i]) && !textures.mProcessed)
            {
                if (textures.mData != 0 && geometry.mData != 0)
                {
                    glBeginResource("Tex");
                    glEndLoadTextureBundle(textures.mData, textures.mSize, glGetCurrentResourcePool(), 1);
                    glEndResource();
                    nlFree(textures.mData);
                    textures.mData = 0;
                    textures.mProcessed = true;

                    glBeginResource("Model");
                    unsigned long numModels = 0;
                    glModel* models = glEndLoadModel(
                        geometry.mData, geometry.mSize, &numModels, glGetCurrentResourcePool());
                    nlFree(geometry.mData);
                    geometry.mData = 0;
                    geometry.mProcessed = true;
                    if (i < 21)
                    {
                        CreateStadiumModelInstances(i, models, numModels);
                    }
                    else
                    {
                        CreatePowerupDrawables(models, numModels);
                    }
                    glEndResource();
                }
                else
                {
                    return false;
                }
            }
        }
    }
    return true;
}

void BeginLoadStadiumEffects()
{
    if (nlStrLen(gStadiumName) != 0)
    {
        char buffer[128];
        nlSNPrintf(buffer, sizeof(buffer),
            "art/effects/%sEffects.bun", gStadiumName);
        gStadiumEffectsRequest = nlLoadEntireFileAsync(buffer, OnStadiumEffectsLoaded,
            &gStadiumEffectsData, 32, AllocateStart, 0, 0, 0);
        nlSNPrintf(buffer, sizeof(buffer),
            "art/effects/%sEffectsNonRes.bun.zlib", gStadiumName);
        gStadiumNonResidentEffectsRequested = nlLoadCompressedFileAsync(buffer, OnStadiumEffectsLoaded,
            &gStadiumNonResidentEffectsData, 32, AllocateEnd, 0x40000,
            gStadiumLoadBuffers[0], gStadiumLoadBuffers[1], 0, 0, 0);
    }
}

bool FinishLoadStadiumEffects()
{
    if (gStadiumEffectsRequest != 0 || gStadiumNonResidentEffectsRequested)
    {
        if (gStadiumEffectsData == 0 && gStadiumEffectsRequest != 0)
        {
            return false;
        }
        if (gStadiumNonResidentEffectsData == 0 && gStadiumNonResidentEffectsRequested)
        {
            return false;
        }
        glBeginResource("Effects");
        EmissionManager::LoadBundle(gStadiumEffectsData, gStadiumNonResidentEffectsData, glGetCurrentResourcePool(), 1);
        glEndResource();
        gStadiumEffectsData = 0;
        gStadiumEffectsRequest = 0;
        gStadiumNonResidentEffectsData = 0;
        gStadiumNonResidentEffectsRequested = false;
    }
    nlFree(gStadiumLoadBuffers[0]);
    gStadiumLoadBuffers[0] = 0;
    gStadiumLoadBuffers[1] = 0;
    return true;
}

void BeginLoadTournamentTrophy()
{
    char path[128];

    gTournamentTrophyLoadResults[0].mData = 0;
    gTournamentTrophyLoadResults[0].mSize = 0;
    gTournamentTrophyLoadResults[0].mProcessed = false;
    gTournamentTrophyLoadResults[1].mData = 0;
    gTournamentTrophyLoadResults[1].mSize = 0;
    gTournamentTrophyLoadResults[1].mProcessed = false;

    GLResourcePool* context = glGetCurrentResourcePool();
    const char* resource = NetTournManager::Instance()->GetTournamentTrophyResource();

    nlSNPrintf(path, sizeof(path), StadiumTextureBundlePathFormat, resource);
    glBeginLoadTextureBundle(path, OnStadiumModelResourceLoaded,
        &gTournamentTrophyLoadResults[0], context);
    nlSNPrintf(path, sizeof(path), StadiumModelBundlePathFormat, resource);
    glBeginLoadModel(path, OnStadiumModelResourceLoaded,
        &gTournamentTrophyLoadResults[1], context);
}

bool IsTournamentTrophyLoaded()
{
    return gTournamentTrophyLoadResults[0].mData != 0
        && gTournamentTrophyLoadResults[1].mData != 0;
}

void FinishLoadTournamentTrophy()
{
    NetTournManager::Instance()->mTrophyResource = (void*)glGetCurrentResourcePool()->MarkResource();

    glBeginResource(StadiumTextureResourceLabel);
    glEndLoadTextureBundle(gTournamentTrophyLoadResults[0].mData,
        gTournamentTrophyLoadResults[0].mSize, glGetCurrentResourcePool(), 0);
    glEndResource();
    nlFree(gTournamentTrophyLoadResults[0].mData);
    gTournamentTrophyLoadResults[0].mData = 0;
    gTournamentTrophyLoadResults[0].mProcessed = true;

    glBeginResource(StadiumModelResourceLabel);
    unsigned long numModels = 0;
    glModel* models = glEndLoadModel(gTournamentTrophyLoadResults[1].mData,
        gTournamentTrophyLoadResults[1].mSize, &numModels, glGetCurrentResourcePool());
    nlFree(gTournamentTrophyLoadResults[1].mData);
    gTournamentTrophyLoadResults[1].mData = 0;
    gTournamentTrophyLoadResults[1].mProcessed = true;

    WorldObjectLoadContext* context
        = (WorldObjectLoadContext*)nlMalloc(sizeof(WorldObjectLoadContext), 8, true);
    if (context != 0)
    {
        new (context) WorldObjectLoadContext(pBasicStadiumInstance);
    }

    DrawableObject* pObject = (DrawableObject*)nlMalloc(0x78, 8, false);
    if (pObject != 0)
    {
        pObject = fn_8027A4BC(pObject, context, models, models->id);
    }
    pObject->m_uObjectFlags |= 1;
    pBasicStadiumInstance->AddDrawableObject(pObject);
    NetTournManager::Instance()->AttachTournamentTrophy(pObject);

    delete context;
    glEndResource();
}

void DestroyStadium()
{
    if (pBasicStadiumInstance != 0)
    {
        delete pBasicStadiumInstance;
        pBasicStadiumInstance = 0;
    }

    if (!gSkipGameplayModels)
    {
        DestroyAttackSideIndicators();
        UninitializeCrowdImpostors();
    }
}

void UpdateStadium(float fDeltaT)
{
    bool bUpdateNPCs = true;
    if (GameInfoManager::Instance() != 0
        && nlTaskManager::m_pInstance->mCurrentState == 2 && lbl_806DEE60
        && GameInfoManager::Instance()->GetStadium() == 13)
    {
        bUpdateNPCs = false;
    }

    if (!IsStadiumWorldLoaded())
    {
        return;
    }
    if (fDeltaT == 0.0f)
    {
        return;
    }

    unsigned int state = nlTaskManager::m_pInstance->mCurrentState;
    if (state == 8 || state == 0x20000 || state == 0x10)
    {
        fn_802785FC(pBasicStadiumInstance, fDeltaT);
    }
    else
    {
        pBasicStadiumInstance->Update(fDeltaT, bUpdateNPCs, true);
    }

    if (gNPCManager != 0)
    {
        gNPCManager->UpdateNPCs(fDeltaT);
    }
}

void UpdateHighRange()
{
    bool bDisable = false;

    if (g_pGame == 0)
    {
        bDisable = true;
    }
    else if (lbl_806E1961)
    {
        bDisable = true;
    }
    else if (fn_801A238C(&lbl_80572020))
    {
        cBaseCamera* pCamera = cCameraManager::PeekCamera();
        bool bBlocked = pCamera == 0 || pCamera->GetType() == 0;
        if (bBlocked || (nlTaskManager::m_pInstance->mCurrentState & 4) != 0)
        {
            bDisable = true;
        }
        else if (GetStadiumUnknown0x2C(GameInfoManager::Instance()->GetStadium())
            && (nlTaskManager::m_pInstance->mCurrentState & 0x2000C) != 0)
        {
            bDisable = true;
        }
    }

    if (bDisable)
    {
        fn_801A2860(&lbl_80572020, 1);
        fn_801A2A78(&lbl_80572020);
        fn_801A28F0(&lbl_80572020);
    }
    else
    {
        fn_801A2860(&lbl_80572020, 0);
    }
}

void RenderWorldNPCs()
{
    pBasicStadiumInstance->Render();
    if (gNPCManager != 0)
    {
        gNPCManager->RenderNPCs();
    }
}

bool IsStadiumWorldLoaded()
{
    return gStadiumResourceDataLoaded && gStadiumWorldLoaded;
}

DrawableObject* FindStadiumDrawableObject(unsigned long uHashID)
{
    DrawableObject* pObject = fn_802787AC(pBasicStadiumInstance, uHashID);
    if (pObject == 0)
    {
        pObject = pBasicStadiumInstance->FindDrawableObject(uHashID);
    }
    return pObject;
}

void fn_802772A4(DrawableObject* pObject)
{
    if (pObject == 0)
    {
        return;
    }
    fn_8027876C(pBasicStadiumInstance, pObject);
}

BasicStadium* BasicStadium::GetCurrentStadium()
{
    return pBasicStadiumInstance;
}

char* fn_802772C4()
{
    return gStadiumName;
}

void fn_802772D0(const char* name, bool)
{
    char path[255];

    gSkipGameplayModels = false;
    fn_80182164();
    CreateAttackSideIndicators();
    lbl_806E1968 = 1;

    nlSNPrintf(path, sizeof(path), lbl_806DEE64, lbl_805222F0, name);
    nlStrNCpy(gStadiumName, name, sizeof(gStadiumName));
    BeginLoadStadium(path, false);

    lbl_806E196C = new (nlMalloc(sizeof(StadiumTweaks), 8, true))
        StadiumTweaks(lbl_80522304, name);
}

void fn_80277BB0()
{
}

bool SetWorldAnimation(const char* objectName, const char* animationName,
    ePlayMode playMode)
{
    WorldAnimManager* pManager = &pBasicStadiumInstance->mWorldAnimManager;
    WorldAnimController* pController
        = pManager->FindController(nlStringLowerHash(objectName));
    if (pController != 0)
    {
        pController->SetAnimation(nlStringLowerHash(animationName), playMode);
        return true;
    }
    return false;
}

bool ShouldLoadStadiumModel(const StadiumModelEntry* entry)
{
    GameInfoManager* pInfo = GameInfoManager::Instance();

    for (short side = 0; side < 2; side++)
    {
        if (entry->mCaptain != -1
            && entry->mCaptain == pInfo->GetCurrentGameInfo()->mTeamIndex[side])
        {
            return true;
        }
        if (entry->mSidekick != -1
            && entry->mSidekick
                == pInfo->GetCurrentGameInfo()->mSidekickIndex[side][0])
        {
            return true;
        }
        if (entry->mSidekick != -1
            && entry->mSidekick
                == pInfo->GetCurrentGameInfo()->mSidekickIndex[side][1])
        {
            return true;
        }
        if (entry->mSidekick != -1
            && entry->mSidekick
                == pInfo->GetCurrentGameInfo()->mSidekickIndex[side][2])
        {
            return true;
        }
    }

    if (entry->mStadium != -1 && entry->mStadium == pInfo->GetStadium())
    {
        return true;
    }

    return entry->mCaptain == -1 && entry->mSidekick == -1
        && entry->mStadium == -1;
}

float fn_80277DB0()
{
    if (pBasicStadiumInstance != 0)
    {
        return fn_802789A0(pBasicStadiumInstance);
    }
    return 0.0f;
}

HighRangeTweakValues_801A2004* fn_80277DC8()
{
    return pBasicStadiumInstance->m_pHighRangeTweaks;
}
