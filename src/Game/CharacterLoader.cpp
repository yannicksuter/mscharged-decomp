#include "Game/AI/Fielder.h"
#include "Game/Player.h"
#include "Game/FE/feHelpFuncs_decl.h"
#include "Game/AnimInventory.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/Audio/RegistryPools.h"
#include "Game/CharacterTemplate.h"
#include "Game/CharacterLoader.h"
#include "Game/CharacterTweaks.h"
#include "Game/DB/CharacterInfo.h"
#include "Game/DB/CharacterInfo.inl"
#include "Game/Effects/EmissionManager.h"
#include "Game/GameInfo.h"
#include "Game/Goalie.h"
#include "Game/Inventory.h"
#include "Game/Physics/CharacterPhysicsElement.h"
#include "Game/SAnim/AnimRetargeter.h"
#include "Game/SHierarchy.h"
#include "Game/Sys/audio.h"
#include "Game/Team.h"
#include "Game/Triggers/SebringAnimScript.h"
#include "Game/TweakQuery.h"
#include "Game/TweakValue.h"
#include "NL/MemAlloc.h"
#include "NL/gl/gl.h"
#include "NL/gl/glMemory.h"
#include "NL/gl/glModel.h"
#include "NL/gl/glState.h"
#include "NL/gl/glTexture.h"
#include "NL/glx/glxTexture.h"
#include "NL/nlFile.h"
#include "NL/nlCompressedFile.h"
#include "NL/plat/nlFileCache.h"
#include "NL/nlMemory.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"
#include "Game/TweakFileLoader.h"

#include <string.h>
#include "NL/nlstring_tmpl.h"
#include "NL/nlstring_impl.h"

static bool g_bLoadAnimsCached;

static int sPendingEffectsLoadCount;

static inline eCharacterClass GetAlternateCaptain(eCharacterClass captain0, eCharacterClass captain1)
{
    eCharacterClass altcaptain = captain1;
    const CharacterInfo& info0 = GetCharacterInfo(captain0);
    const CharacterInfo& info1 = GetCharacterInfo(captain1);
    if (info0.mColourMask & info1.mColourMask)
    {
        if (info0.mColourRank < info1.mColourRank)
        {
            altcaptain = captain1;
        }
        else
        {
            altcaptain = captain0;
        }
    }
    else
    {
        altcaptain = CHARACTER_CLASS_INVALID;
    }
    return altcaptain;
}

CharacterLoader::~CharacterLoader()
{
}

void CharacterLoader::BuildCharacterList()
{
    mCaptain[0] = (eCharacterClass)ConvertToCharacterClass((eTeamID)GameInfoManager::Instance()->GetTeam(0));
    mCaptain[1] = (eCharacterClass)ConvertToCharacterClass((eTeamID)GameInfoManager::Instance()->GetTeam(1));
    for (int i = 0; i < 3; i++)
    {
        mSidekick[0][i] = (eCharacterClass)ConvertToCharacterClass((eSidekickID)GameInfoManager::Instance()->GetSidekick(0, i));
        mSidekick[1][i] = (eCharacterClass)ConvertToCharacterClass((eSidekickID)GameInfoManager::Instance()->GetSidekick(1, i));
    }

    mGoalie[0] = (eCharacterClass)GetGoalieCharacterIndex(GetCharacterInfo(mCaptain[0]));
    mGoalie[1] = (eCharacterClass)GetGoalieCharacterIndex(GetCharacterInfo(mCaptain[1]));

    bool allcaptains = GetTweakBool("/user/allcaptains", false);
    if (allcaptains)
    {
        mSidekick[0][0] = mCaptain[0];
        mSidekick[1][0] = mCaptain[1];
        mSidekick[0][1] = mCaptain[0];
        mSidekick[1][1] = mCaptain[1];
        mSidekick[0][2] = mCaptain[0];
        mSidekick[1][2] = mCaptain[1];
    }

    GetAnimScriptInterpreter();

    int n = 0;
    int plrindex;
    int charIdx;

    for (int teami = 0; teami < 2; teami++)
    {
        plrindex = (mCaptain[0] > mCaptain[1]) ? !teami : teami;

        int idx = plrindex * 4;
        mEntries[n].nTeamID = plrindex;
        mEntries[n].nCharIdx = idx;
        mEntries[n].nPlayerID = 0;
        mEntries[n].cc = mCaptain[plrindex];
        mEntries[n].bCaptain = true;
        mEntries[n].bGoalie = false;
        mEntries[n].bSidekick = false;
        n++;

        mEntries[n].nTeamID = plrindex;
        mEntries[n].nCharIdx = plrindex + 8;
        mEntries[n].nPlayerID = 4;
        mEntries[n].cc = mGoalie[plrindex];
        mEntries[n].bCaptain = false;
        mEntries[n].bGoalie = true;
        mEntries[n].bSidekick = false;
        n++;
    }

    for (int teami = 0; teami < 2; teami++)
    {
        plrindex = (mSidekick[0][0] > mSidekick[1][0]) ? !teami : teami;

        charIdx = plrindex * 4 + 1;

        for (int index = 1; index < 4; index++)
        {
            mEntries[n].nTeamID = plrindex;
            mEntries[n].nCharIdx = charIdx;
            mEntries[n].nPlayerID = index;
            mEntries[n].cc = mSidekick[plrindex][index - 1];
            mEntries[n].bGoalie = false;
            if (mSidekick[plrindex][index - 1] == mCaptain[plrindex])
            {
                mEntries[n].bCaptain = true;
                mEntries[n].bSidekick = false;
            }
            else
            {
                mEntries[n].bCaptain = false;
                mEntries[n].bSidekick = true;
            }
            n++;
            charIdx++;
        }
    }

    mCurrentIndex = -1;
    mCurrent = 0;
    mTemplate = 0;
    mTemplateInfo = 0;
}

bool CharacterLoader::NextCharacter()
{
    mCurrentIndex++;
    if (mCurrentIndex < 10)
    {
        mCurrent = &mEntries[mCurrentIndex];
        mTemplate = 0;
        mTemplateInfo = 0;
        return true;
    }
    mCurrent = 0;
    mTemplate = 0;
    mTemplateInfo = 0;
    return false;
}

bool CharacterLoader::NeedsCharacterTextures()
{
    Entry* pEntry = mCurrent;
    if (pEntry->bGoalie)
    {
        return !GetGoalieTemplateInfo(pEntry->cc - 20)->bTexturesLoaded;
    }
    if (pEntry->bCaptain)
    {
        return true;
    }
    return !GetCharacterTemplateInfo(pEntry->cc)->bTexturesLoaded;
}

static void TextureBundleLoaded_cb(void* data, unsigned long size, void* param)
{
    CharacterLoader::sInstance.mTextureData = data;
    CharacterLoader::sInstance.mTextureSize = size;
}

void CharacterLoader::StartLoadingCharacterTextures()
{
    Entry* pEntry = mCurrent;
    mTextureData = 0;
    mTextureSize = 0;
    mAltTextureData = 0;
    mAltTextureSize = 0;
    if (pEntry->bGoalie)
    {
        s32 goalieIdx = pEntry->cc - 20;
        glBeginLoadTextureBundle(GetGoalieTemplateInfo(goalieIdx)->szTextureFilename, TextureBundleLoaded_cb,
            mCurrent, glGetCurrentResourcePool());
        GetGoalieTemplateInfo(goalieIdx)->bTexturesLoaded = 1;
    }
    else
    {
        glBeginLoadTextureBundle(GetCharacterTemplateInfo(pEntry->cc)->szTextureFilename, TextureBundleLoaded_cb,
            pEntry, glGetCurrentResourcePool());
        GetCharacterTemplateInfo(mCurrent->cc)->bTexturesLoaded = 1;
    }
}

static const char* sShockTextureName = "mario_shock/shock_tex";
static s32 skiptexture = 0xFFFFFFFF;

void CharacterLoader::StartLoadingShockTextures()
{
    mTextureData = 0;
    mTextureSize = 0;
    if (!glTextureLoad(glGetTexture(sShockTextureName)))
    {
        const char* szFilename = "art/characters/mario/mario_shock.rlt";
        glBeginLoadTextureBundle(szFilename, TextureBundleLoaded_cb, mCurrent, glGetCurrentResourcePool());
    }
    else
    {
        mTextureSize = 0xF0000000;
    }
}

static unsigned long SidekickTexture_cb(unsigned long textureId)
{
    unsigned long result = (unsigned long)-1;
    if (textureId != skiptexture)
    {
        result = textureId;
    }
    return result;
}

bool CharacterLoader::FinalizeLoadingCharacterTextures()
{
    char szTexPath[64];

    if (mTextureData == 0)
    {
        return false;
    }

    Entry* pEntry = mCurrent;
    glxTextureLoadCallback_t oldCallback = 0;
    bool bSidekick = pEntry->bSidekick;
    eCharacterClass cc = pEntry->cc;
    if (bSidekick)
    {
        const char* szName = GetCharacterInfo(cc).mName;
        nlSNPrintf(szTexPath, 64, "%s/%s_mario", szName, szName);
        skiptexture = glGetTexture(szTexPath);
        oldCallback = glx_SetLoadCallback(SidekickTexture_cb);
    }

    glEndLoadTextureBundle(mTextureData, mTextureSize, glGetCurrentResourcePool(), true);

    if (mCurrent->bSidekick)
    {
        glx_SetLoadCallback(oldCallback);
    }

    nlFree(mTextureData);
    mTextureData = 0;
    return true;
}

bool CharacterLoader::FinalizeLoadingShockTexture()
{
    if (mTextureSize == 0 && mTextureData == 0)
    {
        return false;
    }

    if (mTextureSize != 0xF0000000)
    {
        glEndLoadTextureBundle(mTextureData, mTextureSize, glGetCurrentResourcePool(), true);
        nlFree(mTextureData);
    }
    mTextureData = 0;
    mTextureSize = 0;

    cCharacter* pChar = g_pCharacters[mCurrent->nCharIdx];
    unsigned long texture = glGetTexture(sShockTextureName);
    if (glTextureLoad(texture))
    {
        pChar->fn_80022E24(texture);
    }
    else
    {
        pChar->fn_80022E24(0);
    }
    return true;
}

bool CharacterLoader::NeedsSharedTextures()
{
    if (mCaptain[0] == 0 || mCaptain[1] == 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}

void CharacterLoader::StartLoadingSharedTextures()
{
    mTextureData = 0;
    mTextureSize = 0;
    glBeginLoadTextureBundle("art/characters/mariogoalie/mariogoalie.rlt", TextureBundleLoaded_cb,
        mCurrent, glGetCurrentResourcePool());
}

bool CharacterLoader::FinalizeLoadingSharedTextures()
{
    if (mTextureData == 0)
    {
        return false;
    }

    skiptexture = glGetTexture("mariogoalie/mariogoalie");
    glxTextureLoadCallback_t oldCallback = glx_SetLoadCallback(SidekickTexture_cb);
    glEndLoadTextureBundle(mTextureData, mTextureSize, glGetCurrentResourcePool(), false);
    glx_SetLoadCallback(oldCallback);

    nlFree(mTextureData);
    mTextureData = 0;
    return true;
}

static void EffectsBundleLoaded_cb(void* data, unsigned long size, void* param)
{
    *(void**)param = data;
}

void CharacterLoader::StartLoadingCharacterEffects()
{
    char szPath[128];

    sPendingEffectsLoadCount++;
    mEffectsData = 0;
    mEffectsLoad = 0;
    mEffectsNonResData = 0;
    mEffectsNonResLoad = 0;

    nlStrNCpy(szPath, "art/effects/", sizeof(szPath));
    const char* szEffectsName = GetCharacterTemplateInfo(mCurrent->cc)->szEffectsName;
    nlStrNCat(szPath, szPath, szEffectsName, sizeof(szPath));
    nlStrNCat(szPath, szPath, "Effects.bun", sizeof(szPath));
    mEffectsLoad = nlLoadEntireFileAsync(szPath, EffectsBundleLoaded_cb, &mEffectsData,
        0x20, AllocateStart, 0, 0, 0);

    nlStrNCpy(szPath, "art/effects/", sizeof(szPath));
    szEffectsName = GetCharacterTemplateInfo(mCurrent->cc)->szEffectsName;
    nlStrNCat(szPath, szPath, szEffectsName, sizeof(szPath));
    nlStrNCat(szPath, szPath, "EffectsNonRes.bun.zlib", sizeof(szPath));
    mEffectsNonResLoad = nlLoadCompressedFileAsync(szPath, EffectsBundleLoaded_cb, &mEffectsNonResData,
        0x20, AllocateEnd, 0x20000, 0, 0, 0, 0, 0);
}

bool CharacterLoader::FinalizeLoadingCharacterEffects()
{
    if (mEffectsData == 0 && mEffectsLoad != 0)
    {
        return false;
    }
    if (mEffectsNonResData == 0 && mEffectsNonResLoad != 0)
    {
        return false;
    }

    EmissionManager::LoadBundle(mEffectsData, mEffectsNonResData, glGetCurrentResourcePool(), true);
    sPendingEffectsLoadCount--;
    return true;
}

static void ExtraTexturesLoaded_cb(void* data, unsigned long size, void* param)
{
    CharacterLoader::sInstance.mExtraTextureData = data;
    CharacterLoader::sInstance.mExtraTextureSize = size;
}

static inline void GetCharacterTexturePath(char* path, eCharacterClass cc, unsigned long capacity)
{
    nlStrNCpy(path, GetCharacterTemplateInfo(cc)->szTextureFilename, capacity);
}

bool CharacterLoader::StartLoadingExtraTextures()
{
    char szPath[128];

    Entry* pEntry = mCurrent;
    mExtraTextureData = 0;
    mExtraTextureSize = 0;
    if (pEntry->bGoalie)
    {
        return false;
    }

    GetCharacterTexturePath(szPath, pEntry->cc, sizeof(szPath));
    char* pEnd = &szPath[nlStrLen(szPath) - 1];
    while (*pEnd != '/')
    {
        pEnd--;
    }
    *pEnd = '\0';
    nlStrNCat(szPath, szPath, "/ExtraTextures.rlt", sizeof(szPath));
    return glBeginLoadTextureBundle(szPath, ExtraTexturesLoaded_cb, mCurrent, glGetCurrentResourcePool());
}

bool CharacterLoader::FinalizeLoadingExtraTextures()
{
    char szTexPath[64];

    if (mExtraTextureData == 0)
    {
        return false;
    }

    Entry* pEntry = mCurrent;
    glxTextureLoadCallback_t oldCallback = 0;
    bool bSidekick = pEntry->bSidekick;
    eCharacterClass cc = pEntry->cc;
    if (bSidekick)
    {
        const char* szName = GetCharacterInfo(cc).mName;
        nlSNPrintf(szTexPath, 64, "%s/%s_mario", szName, szName);
        skiptexture = glGetTexture(szTexPath);
        oldCallback = glx_SetLoadCallback(SidekickTexture_cb);
    }

    glEndLoadTextureBundle(mExtraTextureData, mExtraTextureSize, glGetCurrentResourcePool(), true);

    if (mCurrent->bSidekick)
    {
        glx_SetLoadCallback(oldCallback);
    }

    nlFree(mExtraTextureData);
    mExtraTextureData = 0;
    return true;
}

bool CharacterLoader::AcquireCurrentTemplate()
{
    bool bCreated = false;
    mTemplate = GetCharacterTemplate(mCurrent->cc, &bCreated);
    mTemplateInfo = GetCharacterTemplateInfo(mCurrent->cc);
    return bCreated;
}

static void ModelLoaded_cb(void* data, unsigned long size, void* param)
{
    CharacterLoader::sInstance.mModelData[(int)param] = data;
    CharacterLoader::sInstance.mModelSize[(int)param] = size;
}

void CharacterLoader::StartLoadingCharacterModel(int nModel)
{
    mModelData[nModel] = 0;
    mModelSize[nModel] = 0;
    const char* szFilename = 0;
    switch (nModel)
    {
    case 0:
        szFilename = mTemplateInfo->szModelFilename;
        break;
    case 1:
        szFilename = mTemplateInfo->szShockModelFilename;
        break;
    case 2:
        szFilename = mTemplateInfo->szLowPolyModelFilename;
        break;
    case 3:
        szFilename = mTemplateInfo->szShadowModelFilename;
        break;
    }
    if (szFilename != 0)
    {
        glBeginLoadModel(szFilename, ModelLoaded_cb, (void*)nModel, glGetCurrentResourcePool());
    }
}

bool CharacterLoader::FinalizeLoadingCharacterModel(int nModel)
{
    const char* szFilename = 0;
    switch (nModel)
    {
    case 0:
        szFilename = mTemplateInfo->szModelFilename;
        break;
    case 1:
        szFilename = mTemplateInfo->szShockModelFilename;
        break;
    case 2:
        szFilename = mTemplateInfo->szLowPolyModelFilename;
        break;
    case 3:
        szFilename = mTemplateInfo->szShadowModelFilename;
        break;
    }
    if (szFilename == 0)
    {
        mTemplate->nCharacterModelID[nModel] = 0;
        return true;
    }
    if (mModelData[nModel] == 0)
    {
        return false;
    }

    unsigned long numModels = 0;
    glModel* pModel = glEndLoadModel(mModelData[nModel],
        mModelSize[nModel], &numModels, glGetCurrentResourcePool());
    nlFree(mModelData[nModel]);
    mModelData[nModel] = 0;
    mTemplate->nCharacterModelID[nModel] = pModel->id;
    return true;
}

static void HierarchyLoaded_cb(void* data, unsigned long size, void* param)
{
    CharacterLoader::sInstance.mHierarchyData = data;
    CharacterLoader::sInstance.mHierarchySize = size;
}

unsigned int CharacterLoader::StartLoadingHierarchy()
{
    mHierarchyData = 0;
    mHierarchySize = 0;
    CurrentAllocator = &StandardAllocator;
    AllocatorStack[AllocatorStackDepth++] = &StandardAllocator;
    return nlLoadEntireFileAsync(mTemplateInfo->szHierarchyFilename, HierarchyLoaded_cb,
        mCurrent, 0x20, AllocateStart, 0, 0, 0);
}

bool CharacterLoader::FinalizeLoadingHierarchy()
{
    if (mHierarchyData == 0)
    {
        return false;
    }

    mTemplate->pHierarchyInventory = new (nlMalloc(sizeof(cInventory<cSHierarchy>), 8, false)) cInventory<cSHierarchy>();
    mTemplate->pHierarchyInventory->AddFile((char*)mHierarchyData, mHierarchySize);

    --AllocatorStackDepth;
    AllocatorStack[AllocatorStackDepth] = 0;
    CurrentAllocator = AllocatorStack[AllocatorStackDepth - 1];
    return true;
}

static void PhysicsElementsLoaded_cb(void* data, unsigned long size, void* param)
{
    CharacterLoader::sInstance.mPhysicsData = data;
    CharacterLoader::sInstance.mPhysicsSize = size;
}

unsigned int CharacterLoader::StartLoadingCharacterPhysicsElements()
{
    mPhysicsData = 0;
    mPhysicsSize = 0;
    CurrentAllocator = &StandardAllocator;
    AllocatorStack[AllocatorStackDepth++] = &StandardAllocator;
    return nlLoadEntireFileAsync(mTemplateInfo->szPhysicsFilename, PhysicsElementsLoaded_cb,
        mCurrent, 0x20, AllocateEnd, 0, 0, 0);
}

bool CharacterLoader::FinalizeLoadingCharacterPhysicsElements()
{
    if (mPhysicsData == 0)
    {
        return false;
    }

    CharacterPhysicsData* pPhys = new (nlMalloc(sizeof(CharacterPhysicsData), 8, false)) CharacterPhysicsData();
    mTemplate->pPhysicsData = pPhys;
    LoadCharacterPhysicsElements(mPhysicsData, mPhysicsSize, (CharacterPhysicsData*)mTemplate->pPhysicsData, true);
    mPhysicsData = 0;

    --AllocatorStackDepth;
    AllocatorStack[AllocatorStackDepth] = 0;
    CurrentAllocator = AllocatorStack[AllocatorStackDepth - 1];
    return true;
}

bool CharacterLoader::ShareDuplicateAnimInventory()
{
    mTemplate->uAnimInventoryHashID = nlStringLowerHash(mTemplateInfo->szAnimFilename);

    cAnimInventory* found = FindDuplicateAnimInventory(mCurrent->cc, mTemplate->uAnimInventoryHashID);
    if (found != 0)
    {
        mTemplate->pAnimInventory = found;
        mTemplate->bAnimInventoryCopy = true;
        return true;
    }
    return false;
}

static void AnimationsLoaded_cb(void* data, unsigned long size, void* param)
{
    CharacterLoader::sInstance.mAnimData = data;
    CharacterLoader::sInstance.mAnimSize = size;
}

static inline void GetUncompressedAnimationPath(char* path, const char* filename, unsigned long capacity)
{
    nlStrNCpy(path, filename, capacity);
    *strstr(path, ".zlib") = '\0';
}

void CharacterLoader::StartLoadingCharacterAnimations()
{
    char szPath[200];

    mAnimData = 0;
    mAnimSize = 0;
    CurrentAllocator = &VirtualAllocator;
    AllocatorStack[AllocatorStackDepth++] = &VirtualAllocator;

    const char* szAnimFilename = mTemplateInfo->szAnimFilename;
    bool bCompressed = strstr(szAnimFilename, ".zlib") != 0;
    if (bCompressed)
    {
        if (g_bLoadAnimsCached)
        {
            GetUncompressedAnimationPath(szPath, szAnimFilename, sizeof(szPath));
            nlLoadEntireCachedFileAsync(szPath, AnimationsLoaded_cb, mCurrent, 0x20, AllocateStart, 0, 0, 0);
        }
        else
        {
            nlLoadCompressedFileAsync(szAnimFilename, AnimationsLoaded_cb, mCurrent, 0x20,
                AllocateStart, 0x40000, 0, 0, 0, 0, 0);
        }
    }
    else
    {
        nlLoadEntireFileAsync(szAnimFilename, AnimationsLoaded_cb, mCurrent, 0x20,
            AllocateStart, 0, 0, 0);
    }
}

bool CharacterLoader::FinalizeLoadingCharacterAnimations()
{
    if (mAnimData == 0)
    {
        return false;
    }

    --AllocatorStackDepth;
    AllocatorStack[AllocatorStackDepth] = 0;
    CurrentAllocator = AllocatorStack[AllocatorStackDepth - 1];

    cAnimInventory* pAnim = new (nlMalloc(sizeof(cAnimInventory), 8, false))
        cAnimInventory(mTemplateInfo->pAnimProperties, mTemplateInfo->nNumAnimProperties);
    mTemplate->pAnimInventory = pAnim;
    mTemplate->pAnimInventory->AddAnimBundle((char*)mAnimData, mAnimSize, mTemplateInfo->szAnimFilename);
    mTemplate->bAnimInventoryCopy = false;
    return true;
}

static void TriggersLoaded_cb(void* data, unsigned long size, void* param)
{
    CharacterLoader::sInstance.mTriggerData = data;
    CharacterLoader::sInstance.mTriggerSize = size;
}

void CharacterLoader::StartLoadingCharacterTriggers()
{
    Entry* pEntry = mCurrent;
    mTriggerData = 0;
    mTriggerSize = 0;
    nlLoadEntireFileAsync(GetCharacterTemplateInfo(pEntry->cc)->szTriggerFilename, TriggersLoaded_cb,
        pEntry, 0x20, AllocateEnd, 0, 0, 0);
}

bool CharacterLoader::FinalizeLoadingCharacterTriggers()
{
    void* pData = mTriggerData;
    if (pData == 0)
    {
        return false;
    }

    GetAnimScriptInterpreter()->SetupAnimationTriggers(pData, mTriggerSize,
        mTemplate->pAnimInventory->m_pSAnimInventory);
    mTriggerData = 0;
    return true;
}

bool CharacterLoader::HasAnimRetarget()
{
    if (mTemplateInfo->szAnimRetargetFilename != 0)
    {
        return true;
    }
    mTemplate->pAnimRetargetListInventory = 0;
    return false;
}

static void AnimRetargetLoaded_cb(void* data, unsigned long size, void* param)
{
    CharacterLoader::sInstance.mAnimRetargetData = data;
    CharacterLoader::sInstance.mAnimRetargetSize = size;
}

unsigned int CharacterLoader::StartLoadingAnimRetarget()
{
    mAnimRetargetData = 0;
    mAnimRetargetSize = 0;
    return nlLoadEntireFileAsync(mTemplateInfo->szAnimRetargetFilename, AnimRetargetLoaded_cb,
        mCurrent, 0x20, AllocateStart, 0, 0, 0);
}

bool CharacterLoader::FinalizeLoadingAnimRetarget()
{
    if (mAnimRetargetData == 0)
    {
        return false;
    }

    mTemplate->pAnimRetargetListInventory = new (nlMalloc(sizeof(cInventory<AnimRetargetList>), 8, false)) cInventory<AnimRetargetList>();
    mTemplate->pAnimRetargetListInventory->AddFile((char*)mAnimRetargetData, mAnimRetargetSize);
    return true;
}

bool CharacterLoader::NeedsSidekickSwapTexture()
{
    return mCurrent->bSidekick;
}

static void SidekickSwapTextureLoaded_cb(void* data, unsigned long size, void* param)
{
    CharacterLoader::sInstance.mSidekickTextureData = data;
    CharacterLoader::sInstance.mSidekickTextureSize = size;
}

bool CharacterLoader::StartLoadingSidekickSwapTexture()
{
    char szArtPath[64];
    char szPlayerPath[64];

    eCharacterClass captain0;
    eCharacterClass captain1;
    const char* szCaptainName;

    Entry* pEntry = mCurrent;
    eCharacterClass captaincc = mCaptain[pEntry->nTeamID];
    eCharacterClass cc = pEntry->cc;
    const char* szName = GetCharacterInfo(cc).mName;
    szCaptainName = GetCharacterInfo(captaincc).mName;
    const char* szTexName;
    if (cc == 13)
    {
        szTexName = "hammer";
    }
    else
    {
        szTexName = szName;
    }

    captain0 = mCaptain[0];
    captain1 = mCaptain[1];
    if (captain0 == captain1)
    {
        if (mCurrent->nTeamID == 1)
        {
            nlSNPrintf(szArtPath, 64, "art/characters/%s/%s_%s_alt.rlt", szName, szTexName, szCaptainName);
            nlSNPrintf(szPlayerPath, 64, "%s_%s_alt/%s_%s_alt", szTexName, szCaptainName, szTexName, szCaptainName);
        }
        else
        {
            nlSNPrintf(szArtPath, 64, "art/characters/%s/%s_%s.rlt", szName, szTexName, szCaptainName);
            nlSNPrintf(szPlayerPath, 64, "%s_%s/%s_%s", szTexName, szCaptainName, szTexName, szCaptainName);
        }
    }
    else
    {
        eCharacterClass altcaptain = GetAlternateCaptain(captain0, captain1);
        if (altcaptain == captaincc)
        {
            nlSNPrintf(szArtPath, 64, "art/characters/%s/%s_%s_alt.rlt", szName, szTexName, szCaptainName);
            nlSNPrintf(szPlayerPath, 64, "%s_%s_alt/%s_%s_alt", szTexName, szCaptainName, szTexName, szCaptainName);
        }
        else
        {
            nlSNPrintf(szArtPath, 64, "art/characters/%s/%s_%s.rlt", szName, szTexName, szCaptainName);
            nlSNPrintf(szPlayerPath, 64, "%s_%s/%s_%s", szTexName, szCaptainName, szTexName, szCaptainName);
        }
    }

    mSidekickTextureData = 0;
    mSidekickTextureSize = 0;
    cCharacter* pChar = g_pCharacters[mCurrent->nCharIdx];
    if (glTextureLoad(glGetTexture(szPlayerPath)))
    {
        nlSNPrintf(szArtPath, 64, "%s/%s_mario", szName, szTexName);
        pChar->fn_80022DAC(glGetTexture(szArtPath));
        pChar->fn_80022DE8(glGetTexture(szPlayerPath));
        return false;
    }

    if (!glBeginLoadTextureBundle(szArtPath, SidekickSwapTextureLoaded_cb, mCurrent, glGetCurrentResourcePool()))
    {
        pChar->fn_80022DAC((unsigned long)-1);
        pChar->fn_80022DE8((unsigned long)-1);
        return false;
    }
    return true;
}

bool CharacterLoader::FinalizeLoadingSidekickSwapTexture()
{
    char szBundlePath[64];
    char szPlayerPath[64];
    eCharacterClass captain0;
    eCharacterClass altcaptain;
    const char* szName;
    const char* szCaptainName;
    const char* szTexName;
    cCharacter* pChar;
    eCharacterClass cc;
    eCharacterClass captaincc;

    if (mSidekickTextureData == 0)
    {
        return false;
    }

    Entry* pEntry = mCurrent;
    captaincc = mCaptain[pEntry->nTeamID];
    cc = pEntry->cc;

    glEndLoadTextureBundle(mSidekickTextureData, mSidekickTextureSize, glGetCurrentResourcePool(), false);
    nlFree(mSidekickTextureData);
    mSidekickTextureData = 0;

    pChar = g_pCharacters[mCurrent->nCharIdx];
    szName = GetCharacterInfo(cc).mName;
    szCaptainName = GetCharacterInfo(captaincc).mName;
    if (cc == 13)
    {
        szTexName = "hammer";
    }
    else
    {
        szTexName = szName;
    }

    captain0 = mCaptain[0];
    altcaptain = mCaptain[1];
    if (captain0 == altcaptain)
    {
        if (mCurrent->nTeamID == 1)
        {
            nlSNPrintf(szPlayerPath, 64, "%s_%s_alt/%s_%s_alt", szTexName, szCaptainName, szTexName, szCaptainName);
        }
        else
        {
            nlSNPrintf(szPlayerPath, 64, "%s_%s/%s_%s", szTexName, szCaptainName, szTexName, szCaptainName);
        }
    }
    else
    {
        altcaptain = GetAlternateCaptain(captain0, altcaptain);
        if (altcaptain == captaincc)
        {
            nlSNPrintf(szPlayerPath, 64, "%s_%s_alt/%s_%s_alt", szTexName, szCaptainName, szTexName, szCaptainName);
        }
        else
        {
            nlSNPrintf(szPlayerPath, 64, "%s_%s/%s_%s", szTexName, szCaptainName, szTexName, szCaptainName);
        }
    }

    nlSNPrintf(szBundlePath, 64, "%s/%s_mario", szName, szTexName);
    pChar->fn_80022DAC(glGetTexture(szBundlePath));
    pChar->fn_80022DE8(glGetTexture(szPlayerPath));
    return true;
}

void CharacterLoader::StartLoadingCharINIFiles()
{
    if (mCurrent->bGoalie)
    {
        mTemplate->pGoalieTweaks = new (nlMalloc(sizeof(GoalieTweaks), 8, false))
            GoalieTweaks(mTemplateInfo->szTweaksFilename, mTemplateInfo->szTweaksCategory);
        mTemplate->pPlayerTweaks = 0;
        mTemplate->pSuperPlayerTweaks = 0;
    }
    else
    {
        mTemplate->pPlayerTweaks = new (nlMalloc(sizeof(PlayerTweaks), 8, false))
            PlayerTweaks(mTemplateInfo->szTweaksFilename, mTemplateInfo->szTweaksCategory);
        if (mTemplateInfo->szSuperTweaksFilename != 0)
        {
            mTemplate->pSuperPlayerTweaks = new (nlMalloc(sizeof(PlayerTweaks), 8, false))
                PlayerTweaks(mTemplateInfo->szSuperTweaksFilename, mTemplateInfo->szSuperTweaksCategory);
        }
        else
        {
            mTemplate->pSuperPlayerTweaks = 0;
        }
        mTemplate->pGoalieTweaks = 0;
    }
}

bool CharacterLoader::FinalizeLoadingCharINIFiles()
{
    return gTweakFileLoader.ProcessLoadedFiles();
}

void CharacterLoader::CreateCharacterInstance()
{
    static nlVector3 pos[8] = {
        { 1.5f, 1.5f, 0.0f },
        { 1.5f, -1.5f, 0.0f },
        { 1.5f, 0.0f, 0.0f },
        { 1.5f, 2.5f, 0.0f },
        { -1.5f, 1.5f, 0.0f },
        { -1.5f, -1.5f, 0.0f },
        { -1.5f, 0.0f, 0.0f },
        { -1.5f, 2.5f, 0.0f },
    };

    static nlVector3 goaliepos[2] = {
        { 18.0f, 0.0f, 0.0f },
        { -18.0f, 0.0f, 0.0f },
    };

    bool bCreated;
    tCharacterTemplate* pTemplate = GetCharacterTemplate(mCurrent->cc, &bCreated);
    tCharacterTemplateInfo* pInfo = GetCharacterTemplateInfo(mCurrent->cc);

    cSHierarchy* pHierarchy = pTemplate->pHierarchyInventory->Find((char*)pInfo->szHierarchy);

    AnimRetargetList* pAnimRetargetList = 0;
    if (pTemplate->pAnimRetargetListInventory != 0)
    {
        pAnimRetargetList = pTemplate->pAnimRetargetListInventory->Find(0);
    }

    if (mCurrent->bGoalie)
    {
        s32 goalieIdx = mCurrent->cc - 20;
        Goalie* pGoalie = new (nlMalloc(sizeof(Goalie), 8, false)) Goalie(
            mCurrent->cc, (const int*)pTemplate, pHierarchy, pTemplate->pAnimInventory,
            pTemplate->pPhysicsData, pTemplate->pGoalieTweaks, pAnimRetargetList,
            mCurrent->nCharIdx);
        pGoalie->m_szEffectsName = pInfo->szEffectsName;
        pGoalie->fn_80022DAC(GetHashFromTextureFile(pInfo->szTextureFilename));
        pGoalie->fn_80022DE8(GetHashFromTextureFile(GetGoalieTemplateInfo(goalieIdx)->szTextureFilename));

        g_pCharacters[mCurrent->nCharIdx] = pGoalie;
        g_pCharacters[mCurrent->nCharIdx]->SetPosition(goaliepos[mCurrent->nTeamID]);
        g_pTeams[mCurrent->nTeamID]->SetGoalie(pGoalie);
        fn_800957E4(static_cast<cPlayer*>(g_pCharacters[mCurrent->nCharIdx]), g_pTeams[mCurrent->nTeamID]);

        if (mCurrent->bGoalie && mCurrent->cc != 20)
        {
            char szTexPath[64];
            char szSwapPath[64];
            cCharacter* pChar = g_pCharacters[mCurrent->nCharIdx];
            const char* szName = GetCharacterInfo(mCurrent->cc).GetName();
            nlSNPrintf(szTexPath, 64, "mariogoalie/mariogoalie");
            nlSNPrintf(szSwapPath, 64, "%s/%s", szName, szName);
            pChar->fn_80022DAC(glGetTexture(szTexPath));
            pChar->fn_80022DE8(glGetTexture(szSwapPath));
        }
    }
    else
    {
        cFielder* pFielder = new (nlMalloc(sizeof(cFielder), 8, false)) cFielder(
            mCurrent->nPlayerID, mCurrent->nTeamID, mCurrent->cc, (const int*)pTemplate,
            pHierarchy, pTemplate->pAnimInventory, pTemplate->pPhysicsData,
            pTemplate->pPlayerTweaks, pTemplate->pSuperPlayerTweaks, pAnimRetargetList,
            mCurrent->nCharIdx);
        pFielder->m_szEffectsName = pInfo->szEffectsName;

        g_pCharacters[mCurrent->nCharIdx] = pFielder;
        g_pCharacters[mCurrent->nCharIdx]->SetPosition(pos[mCurrent->nCharIdx]);
        g_pTeams[mCurrent->nTeamID]->SetPlayer((cPlayer*)g_pCharacters[mCurrent->nCharIdx], mCurrent->nPlayerID);
        fn_800957E4(static_cast<cPlayer*>(g_pCharacters[mCurrent->nCharIdx]), g_pTeams[mCurrent->nTeamID]);
    }
}

void CharacterLoader::fn_8000BD70()
{
}

static void AlternateSwapTextureLoaded_cb(void* data, unsigned long size, void* param)
{
    CharacterLoader::sInstance.mAltTextureData = data;
    CharacterLoader::sInstance.mAltTextureSize = size;
}

bool CharacterLoader::HasAlternateSwapTexture()
{
    Entry* pEntry = mCurrent;
    if (pEntry->bCaptain || pEntry->bGoalie)
    {
        eCharacterClass captain0 = mCaptain[0];
        eCharacterClass captain1 = mCaptain[1];
        eCharacterClass altcaptain = CHARACTER_CLASS_INVALID;
        if (captain0 == captain1)
        {
            if (pEntry->nTeamID == 1)
            {
                altcaptain = captain1;
            }
        }
        else
        {
            altcaptain = GetAlternateCaptain(captain0, captain1);
        }

        if (mCurrent->bCaptain)
        {
            if (altcaptain != CHARACTER_CLASS_INVALID && mCurrent->cc == altcaptain)
            {
                const char* szFilename = GetCharacterTemplateInfo(altcaptain)->szAlternateTextureFilename;
                if (szFilename != 0 && nlFileExists(szFilename))
                {
                    return true;
                }
            }
        }
        else if (mCurrent->bGoalie)
        {
            if (altcaptain != CHARACTER_CLASS_INVALID
                && mCurrent->nTeamID == (mCaptain[0] != altcaptain))
            {
                const char* szFilename = GetGoalieTemplateInfo(mCurrent->cc - 20)->szAlternateTextureFilename;
                if (szFilename != 0 && nlFileExists(szFilename))
                {
                    return true;
                }
            }
        }
    }
    return false;
}

bool CharacterLoader::StartLoadingCaptainOrGoalieAlternateSwapTexture()
{
    Entry* pEntry = mCurrent;
    mAltTextureData = 0;
    mAltTextureSize = 0;
    const char* szFilename = 0;
    if (pEntry->bCaptain)
    {
        szFilename = GetCharacterTemplateInfo(pEntry->cc)->szAlternateTextureFilename;
    }
    else if (pEntry->bGoalie)
    {
        szFilename = GetGoalieTemplateInfo(pEntry->cc - 20)->szAlternateTextureFilename;
    }
    glBeginLoadTextureBundle(szFilename, AlternateSwapTextureLoaded_cb, mCurrent, glGetCurrentResourcePool());
    return true;
}

bool CharacterLoader::FinalizeLoadingCaptainOrGoalieAlternateSwapTexture()
{
    char szTexPath[64];
    char szSwapPath[64];

    if (mAltTextureData == 0)
    {
        return false;
    }

    glEndLoadTextureBundle(mAltTextureData, mAltTextureSize, glGetCurrentResourcePool(), false);

    const char* szName = GetCharacterInfo(mCurrent->cc).mName;
    if (mCurrent->bGoalie)
    {
        nlSNPrintf(szTexPath, 64, "mariogoalie/mariogoalie");
    }
    else if (mCurrent->bCaptain)
    {
        nlSNPrintf(szTexPath, 64, "%s/%s", szName, szName);
    }
    nlSNPrintf(szSwapPath, 64, "%s_alt/%s_alt", szName, szName);

    cCharacter* pChar = g_pCharacters[mCurrent->nCharIdx];
    unsigned long texture = glGetTexture(szTexPath);
    unsigned long swapTexture = glGetTexture(szSwapPath);
    pChar->fn_80022DAC(texture);
    pChar->fn_80022DE8(swapTexture);

    nlFree(mAltTextureData);
    mAltTextureData = 0;
    return true;
}

static void AudioBankLoaded_cb(AudioResourceLoadOwner* handle, void* context)
{
    CharacterLoader::sInstance.mAudioCompletedCount = (int)context;
}

void CharacterLoader::StartLoadingAudioBank0()
{
    mAudioRequestCount += gAudioEnabled;
    LoadSoundBank((GameAudio*)g_pAudioSystem, 0, 0, AudioBankLoaded_cb,
        (void*)mAudioRequestCount);
}

bool CharacterLoader::NeedsCaptainAudio()
{
    return mCurrent->bCaptain;
}

void CharacterLoader::StartLoadingCaptainAudio()
{
    int nBank = GetCharacterInfo(mCurrent->cc).mSoundBankId;
    mAudioRequestCount += gAudioEnabled;
    LoadSoundBank((GameAudio*)g_pAudioSystem, nBank,
        (mCurrent->nTeamID == 0) ? 1 : 5, AudioBankLoaded_cb, (void*)mAudioRequestCount);
}

bool CharacterLoader::NeedsSidekickAudio()
{
    return mCurrent->bSidekick;
}

void CharacterLoader::StartLoadingSidekickAudio()
{
    int nBank = GetCharacterInfo(mCurrent->cc).mSoundBankId;
    mAudioRequestCount += gAudioEnabled;
    Entry* pEntry = mCurrent;
    int nSlot = (pEntry->nTeamID == 0) ? 1 : 5;
    nSlot += pEntry->nPlayerID;
    LoadSoundBank((GameAudio*)g_pAudioSystem, nBank, nSlot, AudioBankLoaded_cb,
        (void*)mAudioRequestCount);
}

void CharacterLoader::StartLoadingAudioBank13()
{
    mAudioRequestCount += gAudioEnabled;
    LoadSoundBank((GameAudio*)g_pAudioSystem, 13, 9, AudioBankLoaded_cb,
        (void*)mAudioRequestCount);
}

bool CharacterLoader::FinalizeAudio()
{
    if (mAudioCompletedCount >= mAudioRequestCount)
    {
        mAudioCompletedCount = -1;
        mAudioRequestCount = -1;
        return true;
    }
    return false;
}

static TweakBoolBinding sLoadAnimsCachedTweak(
    "g_bLoadAnimsCached", "FileCache", &g_bLoadAnimsCached, true);

CharacterLoader CharacterLoader::sInstance;
