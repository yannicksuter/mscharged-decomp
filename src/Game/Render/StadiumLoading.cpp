#include "NL/nlDLListContainer.inl"
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
#include "Game/Render/HighRange.h"
#include "Game/Render/PlanarShadowDrawable.h"
#include "Game/GameObjectLighting.h"
#include "Game/AI/Powerups.h"
#include "Game/Field.h"
#include "Game/Physics/PhysicsNet.h"
#include "Game/Render/ImpostorLighting.h"
#include "Game/TweakFileLoader.h"
#include "Game/Render/RenderShadow.h"
#include "NL/gl/glState.h"

#include "Game/UnidentifiedStaticStorage.h"
extern "C"
{
    extern bool lbl_806DEE60;
    bool lbl_806E1960;
    bool gDisableHighRange;
    BasicStadium* pBasicStadiumInstance;
    int lbl_806E1968;
    StadiumTweaks* lbl_806E196C;
    unsigned long lbl_806DEE30 = -1;
    unsigned long lbl_806DEE34 = -1;
}
DrawableObject* fn_802787AC(BasicStadium* stadium, unsigned long uHashID);
void fn_80278818(BasicStadium* stadium, nlVector4* corners);
float fn_802789A0(BasicStadium* stadium);
float fn_8027313C();
void fn_802785FC(BasicStadium* stadium, float fDeltaT);
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
char gStadiumName[32];
char gStadiumResourcePath[256];
StadiumLoadResult gStadiumModelLoadResults[2][22];
StadiumLoadResult gTournamentTrophyLoadResults[2];
DrawableObject* gStadiumSingleModelInstances[22];
DrawableObject* gStadiumCameraInstances[10];
DrawableObject* gStadiumBallInstances[10];
DrawableObject* gStadiumBulletBillInstances[6];
DrawableObject* gStadiumHammerInstances[15];
DrawableObject* gStadiumDaisyFistInstances[8];
DrawableObject* gStadiumThwompInstances[8];
DrawableObject* gStadiumNumberInstances[12];

StadiumModelEntry gStadiumModelEntries[22] =
{
    { 0, "art/objects/gameplay/ball", 10, gStadiumBallInstances, -1, -1, -1 },
    { 1, "art/objects/gameplay/bulletbill", 6, gStadiumBulletBillInstances, -1, 7, -1 },
    { 2, "art/objects/gameplay/hammer", 15, gStadiumHammerInstances, -1, 2, -1 },
    { 3, "art/objects/gameplay/yoshi_egg", 1, &gStadiumSingleModelInstances[3], 8, -1, -1 },
    { 4, "art/objects/gameplay/birdo_egg", 1, &gStadiumSingleModelInstances[4], -1, 3, -1 },
    { 5, "art/objects/gameplay/koopa_shell", 1, &gStadiumSingleModelInstances[5], -1, 1, -1 },
    { 6, "art/objects/gameplay/daisy_fist", 8, gStadiumDaisyFistInstances, 2, -1, -1 },
    { 7, "art/objects/cameras/flyingcamera3", 10, gStadiumCameraInstances, 5, -1, -1 },
    { 8, "art/objects/gameplay/thwomp", 8, gStadiumThwompInstances, -1, -1, 15 },
    { 9, "art/objects/gameplay/Number0", 1, &gStadiumNumberInstances[0], -1, -1, -1 },
    { 10, "art/objects/gameplay/Number1", 1, &gStadiumNumberInstances[1], -1, -1, -1 },
    { 11, "art/objects/gameplay/Number2", 1, &gStadiumNumberInstances[2], -1, -1, -1 },
    { 12, "art/objects/gameplay/Number3", 1, &gStadiumNumberInstances[3], -1, -1, -1 },
    { 13, "art/objects/gameplay/Number4", 1, &gStadiumNumberInstances[4], -1, -1, -1 },
    { 14, "art/objects/gameplay/Number5", 1, &gStadiumNumberInstances[5], -1, -1, -1 },
    { 15, "art/objects/gameplay/Number6", 1, &gStadiumNumberInstances[6], -1, -1, -1 },
    { 16, "art/objects/gameplay/Number7", 1, &gStadiumNumberInstances[7], -1, -1, -1 },
    { 17, "art/objects/gameplay/Number8", 1, &gStadiumNumberInstances[8], -1, -1, -1 },
    { 18, "art/objects/gameplay/Number9", 1, &gStadiumNumberInstances[9], -1, -1, -1 },
    { 19, "art/objects/gameplay/NumberDash", 1, &gStadiumNumberInstances[10], -1, -1, -1 },
    { 20, "art/objects/gameplay/NumberColon", 1, &gStadiumNumberInstances[11], -1, -1, -1 },
    { 21, "art/objects/gameplay/powerups", 1, 0, -1, -1, -1 },
};

bool CreatePowerupDrawables(glModel* models, unsigned long numModels)
{
    glModel* model = models;
    glModel* end = models + numModels;
    WorldObjectLoadContext* context
        = (WorldObjectLoadContext*)nlMalloc(sizeof(WorldObjectLoadContext), 8, true);
    new (context) WorldObjectLoadContext(pBasicStadiumInstance);

    unsigned long uExcluded = nlStringHash("gameplay/metalshell");
    for (; model < end; model++)
    {
        if (uExcluded == model->id)
        {
            continue;
        }

        DrawableObject* pObject = (DrawableObject*)nlMalloc(sizeof(PlanarShadowDrawable), 8, false);
        pObject = new (pObject) PlanarShadowDrawable(
            context, model, model->id);
        pObject->m_uHashID = model->id;
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

DrawableObject** GetNumberRenderObjects()
{
    return gStadiumNumberInstances;
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
        pObject = (DrawableObject*)nlMalloc(sizeof(ChargeShadowDrawable), 8, false);
        pObject = new (pObject) ChargeShadowDrawable(
            context, models, models->id);
        fn_8027876C(pBasicStadiumInstance, pObject);
        instance = 0;
    }
    else
    {
        for (; models < end; models++)
        {
            pObject = (DrawableObject*)nlMalloc(sizeof(PlanarShadowDrawable), 8, false);
            pObject = new (pObject) PlanarShadowDrawable(
                context, models, models->id);
            fn_8027876C(pBasicStadiumInstance, pObject);
        }
    }

    gStadiumModelEntries[entry].mInstances[0] = pObject;
    for (; (unsigned long)instance < (unsigned long)gStadiumModelEntries[entry].mNumInstances;
         instance++)
    {
        nlSNPrintf(name, sizeof(name), "npc%dclone%d", entry, instance);
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
    pBasicStadiumInstance->m_pOpaqueView = (GLView*)GetShadowedView();
    pBasicStadiumInstance->m_pAlphaView = (GLView*)GetLayerView(eCLV_WorldAlphaBlended);

    nlSNPrintf(buffer, sizeof(buffer), "%s/gameworld.res.zlib", gStadiumResourcePath);
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
    const char* szName;
    const CharacterInfo& team = GetCharacterInfo(GetCharacterIndexFromCaptain(
        GameInfoManager::Instance()->GetTeam(0)));
    const CharacterInfo& opponent = GetCharacterInfo(GetCharacterIndexFromCaptain(
        GameInfoManager::Instance()->GetTeam(1)));
    szName = team.mName;
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
            if (ShouldLoadStadiumModel(&gStadiumModelEntries[i]))
            {
                if (gStadiumModelLoadResults[0][i].mProcessed)
                {
                    continue;
                }
                else if (!gStadiumModelLoadResults[0][i].mProcessed)
                {
                    if (gStadiumModelLoadResults[0][i].mData != 0
                        && gStadiumModelLoadResults[1][i].mData != 0)
                    {
                        glBeginResource("Tex");
                        glEndLoadTextureBundle(gStadiumModelLoadResults[0][i].mData,
                            gStadiumModelLoadResults[0][i].mSize,
                            glGetCurrentResourcePool(), 1);
                        glEndResource();
                        nlFree(gStadiumModelLoadResults[0][i].mData);
                        gStadiumModelLoadResults[0][i].mData = 0;
                        gStadiumModelLoadResults[0][i].mProcessed = true;

                        glBeginResource("Model");
                        unsigned long numModels = 0;
                        glModel* models = glEndLoadModel(
                            gStadiumModelLoadResults[1][i].mData,
                            gStadiumModelLoadResults[1][i].mSize,
                            &numModels, glGetCurrentResourcePool());
                        nlFree(gStadiumModelLoadResults[1][i].mData);
                        gStadiumModelLoadResults[1][i].mData = 0;
                        gStadiumModelLoadResults[1][i].mProcessed = true;
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

    nlSNPrintf(path, sizeof(path), "%s.rlt", resource);
    glBeginLoadTextureBundle(path, OnStadiumModelResourceLoaded,
        &gTournamentTrophyLoadResults[0], context);
    nlSNPrintf(path, sizeof(path), "%s.rlg", resource);
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

    glBeginResource("Tex");
    glEndLoadTextureBundle(gTournamentTrophyLoadResults[0].mData,
        gTournamentTrophyLoadResults[0].mSize, glGetCurrentResourcePool(), 0);
    glEndResource();
    nlFree(gTournamentTrophyLoadResults[0].mData);
    gTournamentTrophyLoadResults[0].mData = 0;
    gTournamentTrophyLoadResults[0].mProcessed = true;

    glBeginResource("Model");
    unsigned long numModels = 0;
    glModel* models = glEndLoadModel(gTournamentTrophyLoadResults[1].mData,
        gTournamentTrophyLoadResults[1].mSize, &numModels, glGetCurrentResourcePool());
    nlFree(gTournamentTrophyLoadResults[1].mData);
    gTournamentTrophyLoadResults[1].mData = 0;
    gTournamentTrophyLoadResults[1].mProcessed = true;

    WorldObjectLoadContext* context = new (8, true) WorldObjectLoadContext(pBasicStadiumInstance);

    DrawableObject* pObject = (DrawableObject*)nlMalloc(sizeof(PlanarShadowDrawable), 8, false);
    pObject = new (pObject) PlanarShadowDrawable(
        context, models, models->id);
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

extern "C" bool lbl_806DEE60 = true;

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
    if (state != 8 && state != 0x20000 && state != 0x10)
    {
        pBasicStadiumInstance->Update(fDeltaT, bUpdateNPCs, true);
    }
    else
    {
        fn_802785FC(pBasicStadiumInstance, fDeltaT);
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
    else if (gDisableHighRange)
    {
        bDisable = true;
    }
    else if (IsHighRangeEnabled(&gHighRange))
    {
        cBaseCamera* pCamera = cCameraManager::PeekCamera();
        bool bBlocked;
        if (pCamera != 0 && pCamera->GetType() == 0)
        {
            bBlocked = true;
        }
        else
        {
            bBlocked = false;
        }
        if (bBlocked || (nlTaskManager::m_pInstance->mCurrentState & 4) != 0)
        {
            bDisable = true;
        }
        else if (StadiumHasHighRangeDrawables(GameInfoManager::Instance()->GetStadium())
            && (nlTaskManager::m_pInstance->mCurrentState & 0x20018) != 0)
        {
            bDisable = true;
        }
    }

    if (bDisable)
    {
        SetHighRangeTargetsEnabled(&gHighRange, 1);
        RenderHighRangeChain(&gHighRange);
        CompositeHighRange(&gHighRange);
    }
    else
    {
        SetHighRangeTargetsEnabled(&gHighRange, 0);
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

static inline void SetStadiumName(const char* name)
{
    nlStrNCpy(gStadiumName, name, sizeof(gStadiumName));
}

void InitializeStadiumLighting()
{
    char name[64];
    nlSNPrintf(name, sizeof(name), "_%s/lightramp", gStadiumName);
    unsigned long lightRamp = glGetTexture(name);
    nlSNPrintf(name, sizeof(name), "_%s/playerlightramp", gStadiumName);
    glGetTexture(name);
    if (glTextureLoad(lightRamp))
    {
        lbl_806DEE30 = lightRamp;
    }
    lbl_806DEE34 = glGetTexture("global/lightramp_sts");
    nlSNPrintf(name, sizeof(name), "_%s/shadowlookup", gStadiumName);
    SetImpostorLightingTexture(glGetTexture(name));
    nlSNPrintf(name, sizeof(name), "_%s/shadowlookup", gStadiumName);
    LoadShadowLightingLookup(glGetTexture(name));
}

void fn_802772D0(const char* name, bool)
{
    char path[255];

    gSkipGameplayModels = false;
    fn_80182164();
    CreateAttackSideIndicators();
    lbl_806E1968 = 1;

    nlSNPrintf(path, sizeof(path), "%s/%s", "art/environments", name);
    SetStadiumName(name);
    BeginLoadStadium(path, false);

    lbl_806E196C = new (nlMalloc(sizeof(StadiumTweaks), 8, true))
        StadiumTweaks("/Stadium", name);
}

static inline void SetPhysicsNetDimensions(float width, float height, float depth)
{
    PhysicsNet::sfPhysicsNetWidth = width;
    PhysicsNet::sfPhysicsNetHeight = height;
    PhysicsNet::sfPhysicsNetDepth = depth;
}

static inline DrawableObject* FindStadiumDrawableObject(const char* name)
{
    return FindStadiumDrawableObject(nlStringLowerHash(name));
}

static inline bool HasSoftnessOverride(const StadiumTweaks& tweaks)
{
    return tweaks.fSoftness >= 0.0f;
}

static inline void SetNetSoftness(float softness)
{
    // Stadium softness overrides are disabled.
}

bool FinishLoadStadium(bool stadiumViewer)
{
    if (lbl_806E1968 == 1)
    {
        if (IsStadiumResourceDataLoaded())
        {
            lbl_806E1968 = 2;
            BeginLoadStadiumTemporaryResources();
        }
        return false;
    }
    if (lbl_806E1968 == 2)
    {
        if (FinishLoadStadiumResources())
        {
            lbl_806E1968 = 3;
            BeginLoadStadiumEffects();
        }
        return false;
    }
    if (!FinishLoadStadiumEffects())
    {
        return false;
    }
    if (!gTweakFileLoader.ProcessLoadedFiles())
    {
        return false;
    }
    if (!stadiumViewer)
    {
        InitializePowerups();
    }

    InitializeStadiumLighting();

    DrawableObject* fieldCorner = FindStadiumDrawableObject("FieldCorner");
    if (fieldCorner != 0)
    {
        float y = fieldCorner->GetWorldMatrix()->m42;
        cField::SetFieldDimensions(fieldCorner->GetWorldMatrix()->m41, y, 0.0f);
    }
    DrawableObject* penaltyCorner = FindStadiumDrawableObject("PenaltyCorner");
    if (penaltyCorner != 0)
    {
        float y = penaltyCorner->GetWorldMatrix()->m42;
        cField::mfPenaltyBoxX = penaltyCorner->GetWorldMatrix()->m41;
        cField::mfPenaltyBoxY = y;
    }

    cNet::SetNetDimensions(lbl_806E196C->fNetWidth, lbl_806E196C->fNetHeight,
        lbl_806E196C->fGoalpostRadius, lbl_806E196C->fGoalpostOffset);
    SetPhysicsNetDimensions(lbl_806E196C->fPhysNetWidth,
        lbl_806E196C->fPhysNetHeight, lbl_806E196C->fPhysNetDepth);
    if (HasSoftnessOverride(*lbl_806E196C))
    {
        SetNetSoftness(lbl_806E196C->fSoftness);
    }
    NetMesh::SetDontUseLowestNetTextureLOD(lbl_806E196C->bDontUseLowest);
    NetMesh::s_bAnimatedNetMeshEnabled = true;
    SetCoPlanarZ(lbl_806E196C->fShadowHeight);
    SetPlanarShadowOpacity(lbl_806E196C->fShadowOpacity);
    delete lbl_806E196C;
    lbl_806E196C = 0;
    return true;
}

void StadiumScreenToWorldPosition(nlVector3& result, float screenX, float screenY, float distance)
{
    fn_802785FC(pBasicStadiumInstance, 0.0f);
    nlVector4 corners[8];
    fn_80278818(pBasicStadiumInstance, corners);
    nlVector3 point = *(nlVector3*)&corners[0];
    nlVector3 horizontal;
    nlVector3 vertical;
    nlVec3Sub(horizontal, *(nlVector3*)&corners[1], point);
    nlVec3Sub(vertical, *(nlVector3*)&corners[3], point);
    nlVec3ScaleAdd(point, (-screenX + 1.0f) / 2.0f, horizontal, point);
    nlVec3ScaleAdd(point, (screenY + 1.0f) / 2.0f, vertical, point);
    nlVector3 cameraPosition = cCameraManager::PeekCamera()->GetCameraPosition();
    float scale = distance / fn_8027313C();
    nlVector3 offset;
    nlVec3Sub(offset, point, cameraPosition);
    nlVec3Scale(offset, offset, scale);
    nlVec3Add(result, offset, cameraPosition);
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

    for (int side = 0; side < 2; side++)
    {
        if (entry->mCaptain != -1
            && entry->mCaptain == pInfo->GetCurrentGameInfo()->GetTeam((short)side))
        {
            return true;
        }
        for (int slot = 0; slot < 3; ++slot)
        {
            if (entry->mSidekick != -1
                && entry->mSidekick
                    == pInfo->GetCurrentGameInfo()->GetSidekick((short)side, slot))
            {
                return true;
            }
        }
    }

    if (entry->mStadium != -1 && entry->mStadium == pInfo->GetStadium())
    {
        return true;
    }

    return entry->mCaptain == -1 && entry->mSidekick == -1
        && entry->mStadium == -1;
}

float GetStadiumTime()
{
    if (pBasicStadiumInstance != 0)
    {
        return fn_802789A0(pBasicStadiumInstance);
    }
    return 0.0f;
}

HighRangeTweaks* GetHighRangeTweaks()
{
    return pBasicStadiumInstance->m_pHighRangeTweaks;
}

bool ShouldRenderStadiumNPC(ImpostorModel* model)
{
    if (cCameraManager::m_BeginFrameCameraType == 1
        || cCameraManager::m_BeginFrameCameraType == 6)
    {
        return model->mWorldMatrix.m42 > 0.0f;
    }
    return true;
}
