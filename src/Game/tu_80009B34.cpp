#include "Game/AI/Fielder.h"
#include "Game/FE/feHelpFuncs_decl.h"
#include "Game/AnimInventory.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/Audio/UnidentifiedRegistryPools.h"
#include "Game/CharacterTemplate.h"
#include "Game/CharacterLoader.h"
#include "Game/CharacterTweaks.h"
#include "Game/DB/CharacterInfo.h"
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
#include "Game/TweakRegistry.h"
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
#include "NL/plat/tu_80372B4C.h"
#include "NL/nlMemory.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"
#include "Game/TweakFileLoader.h"

#include <string.h>
#include "NL/nlstring_tmpl.h"

extern "C" void fn_800957E4(cCharacter* pCharacter, cTeam* pTeam);
extern "C" bool nlLoadCompressedFileAsync(const char* path, LoadAsyncCallback callback,
    void* userData, unsigned int alignment, int allocType,
    unsigned int chunkSize, void* readBuffer0, void* readBuffer1, void*,
    unsigned long param, MemoryAllocator* allocator);
extern "C" void LoadBundle(void* data, void* nonResidentData,
    GLResourcePool* allocator, bool);

extern bool gAudioEnabled;

static bool g_bLoadAnimsCached;

static int sUnidentifiedEffectsLoadCount;

static inline bool SameCharacterClass(eCharacterClass first, eCharacterClass second)
{
    return first == second;
}

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

CharacterLoader_8056B290::~CharacterLoader_8056B290()
{
}

void CharacterLoader_8056B290::fn_80009BC8()
{
    captain[0] = (eCharacterClass)ConvertToCharacterClass((eTeamID)GameInfoManager::Instance()->GetTeam(0));
    captain[1] = (eCharacterClass)ConvertToCharacterClass((eTeamID)GameInfoManager::Instance()->GetTeam(1));
    for (short i = 0; i < 3; i++)
    {
        sidekick[0][i] = (eCharacterClass)ConvertToCharacterClass((eSidekickID)GameInfoManager::Instance()->GetSidekick(0, i));
        sidekick[1][i] = (eCharacterClass)ConvertToCharacterClass((eSidekickID)GameInfoManager::Instance()->GetSidekick(1, i));
    }

    goalie[0] = (eCharacterClass)GetGoalieCharacterIndex(GetCharacterInfo(captain[0]));
    goalie[1] = (eCharacterClass)GetGoalieCharacterIndex(GetCharacterInfo(captain[1]));

    bool allcaptains = GetTweakBool("/user/allcaptains", false);
    if (allcaptains)
    {
        sidekick[0][0] = captain[0];
        sidekick[1][0] = captain[1];
        sidekick[0][1] = captain[0];
        sidekick[1][1] = captain[1];
        sidekick[0][2] = captain[0];
        sidekick[1][2] = captain[1];
    }

    GetAnimScriptInterpreter();

    int n = 0;
    int plrindex;
    int charIdx;

    for (int teami = 0; teami < 2; teami++)
    {
        plrindex = (captain[0] > captain[1]) ? !teami : teami;

        int idx = plrindex * 4;
        mEntries[n].nTeamID = plrindex;
        mEntries[n].nCharIdx = idx;
        mEntries[n].nPlayerID = 0;
        mEntries[n].cc = captain[plrindex];
        mEntries[n].bCaptain = true;
        mEntries[n].bGoalie = false;
        mEntries[n].bSidekick = false;
        n++;

        mEntries[n].nTeamID = plrindex;
        mEntries[n].nCharIdx = plrindex + 8;
        mEntries[n].nPlayerID = 4;
        mEntries[n].cc = goalie[plrindex];
        mEntries[n].bCaptain = false;
        mEntries[n].bGoalie = true;
        mEntries[n].bSidekick = false;
        n++;
    }

    for (int teami = 0; teami < 2; teami++)
    {
        plrindex = (sidekick[0][0] > sidekick[1][0]) ? !teami : teami;

        charIdx = plrindex * 4 + 1;

        for (int index = 1; index < 4; index++)
        {
            mEntries[n].nTeamID = plrindex;
            mEntries[n].nCharIdx = charIdx;
            mEntries[n].nPlayerID = index;
            mEntries[n].cc = sidekick[plrindex][index - 1];
            mEntries[n].bGoalie = false;
            if (SameCharacterClass(sidekick[plrindex][index - 1], captain[plrindex]))
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

bool CharacterLoader_8056B290::fn_80009EFC()
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

bool CharacterLoader_8056B290::fn_80009F48()
{
    Entry* pEntry = mCurrent;
    if (pEntry->bGoalie)
    {
        return !GetGoalieTemplateInfo(pEntry->cc - 20)->bLoaded;
    }
    if (pEntry->bCaptain)
    {
        return true;
    }
    return !GetCharacterTemplateInfo(pEntry->cc)->bUnidentified58;
}

static void fn_80009FB8(void* data, unsigned long size, void* param)
{
    CharacterLoader_8056B290::sUnidentifiedInstance.mTextureData = data;
    CharacterLoader_8056B290::sUnidentifiedInstance.mTextureSize = size;
}

void CharacterLoader_8056B290::fn_80009FCC()
{
    Entry* pEntry = mCurrent;
    mTextureData = 0;
    mTextureSize = 0;
    mAltTextureData = 0;
    mAltTextureSize = 0;
    if (pEntry->bGoalie)
    {
        s32 goalieIdx = pEntry->cc - 20;
        glBeginLoadTextureBundle(GetGoalieTemplateInfo(goalieIdx)->szTextureFilename, fn_80009FB8,
            mCurrent, glGetCurrentResourcePool());
        GetGoalieTemplateInfo(goalieIdx)->bLoaded = 1;
    }
    else
    {
        glBeginLoadTextureBundle(GetCharacterTemplateInfo(pEntry->cc)->szTextureFilename, fn_80009FB8,
            pEntry, glGetCurrentResourcePool());
        GetCharacterTemplateInfo(mCurrent->cc)->bUnidentified58 = 1;
    }
}

static const char* sUnidentifiedShockTextureName = "mario_shock/shock_tex";
static s32 skiptexture = 0xFFFFFFFF;

void CharacterLoader_8056B290::fn_8000A0A8()
{
    mTextureData = 0;
    mTextureSize = 0;
    if (!glTextureLoad(glGetTexture(sUnidentifiedShockTextureName)))
    {
        const char* szFilename = "art/characters/mario/mario_shock.rlt";
        glBeginLoadTextureBundle(szFilename, fn_80009FB8, mCurrent, glGetCurrentResourcePool());
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

bool CharacterLoader_8056B290::fn_8000A144()
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

bool CharacterLoader_8056B290::fn_8000A224()
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
    unsigned long texture = glGetTexture(sUnidentifiedShockTextureName);
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

bool CharacterLoader_8056B290::fn_8000A2FC()
{
    if (captain[0] == 0 || captain[1] == 0)
    {
        return false;
    }
    else
    {
        return true;
    }
}

void CharacterLoader_8056B290::fn_8000A324()
{
    mTextureData = 0;
    mTextureSize = 0;
    glBeginLoadTextureBundle("art/characters/mariogoalie/mariogoalie.rlt", fn_80009FB8,
        mCurrent, glGetCurrentResourcePool());
}

bool CharacterLoader_8056B290::fn_8000A378()
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

static void fn_8000A410(void* data, unsigned long size, void* param)
{
    *(void**)param = data;
}

void CharacterLoader_8056B290::fn_8000A418()
{
    char szPath[128];

    sUnidentifiedEffectsLoadCount++;
    mEffectsData = 0;
    mEffectsLoad = 0;
    mEffectsNonResData = 0;
    mEffectsNonResLoad = 0;

    nlStrNCpy(szPath, "art/effects/", sizeof(szPath));
    const char* szEffectsName = GetCharacterTemplateInfo(mCurrent->cc)->szEffectsName;
    nlStrNCat(szPath, szPath, szEffectsName, sizeof(szPath));
    nlStrNCat(szPath, szPath, "Effects.bun", sizeof(szPath));
    mEffectsLoad = nlLoadEntireFileAsync(szPath, fn_8000A410, &mEffectsData,
        0x20, AllocateStart, 0, 0, 0);

    nlStrNCpy(szPath, "art/effects/", sizeof(szPath));
    szEffectsName = GetCharacterTemplateInfo(mCurrent->cc)->szEffectsName;
    nlStrNCat(szPath, szPath, szEffectsName, sizeof(szPath));
    nlStrNCat(szPath, szPath, "EffectsNonRes.bun.zlib", sizeof(szPath));
    mEffectsNonResLoad = nlLoadCompressedFileAsync(szPath, fn_8000A410, &mEffectsNonResData,
        0x20, AllocateEnd, 0x20000, 0, 0, 0, 0, 0);
}

bool CharacterLoader_8056B290::fn_8000A5D8()
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
    sUnidentifiedEffectsLoadCount--;
    return true;
}

static void fn_8000A668(void* data, unsigned long size, void* param)
{
    CharacterLoader_8056B290::sUnidentifiedInstance.mExtraTextureData = data;
    CharacterLoader_8056B290::sUnidentifiedInstance.mExtraTextureSize = size;
}

bool CharacterLoader_8056B290::fn_8000A67C()
{
    char szPath[128];

    Entry* pEntry = mCurrent;
    mExtraTextureData = 0;
    mExtraTextureSize = 0;
    if (pEntry->bGoalie)
    {
        return false;
    }

    nlStrNCpy(szPath, GetCharacterTemplateInfo(pEntry->cc)->szTextureFilename, sizeof(szPath));
    char* pEnd = &szPath[nlStrLen(szPath) - 1];
    while (*pEnd != '/')
    {
        pEnd--;
    }
    *pEnd = '\0';
    nlStrNCat(szPath, szPath, "/ExtraTextures.rlt", sizeof(szPath));
    return glBeginLoadTextureBundle(szPath, fn_8000A668, mCurrent, glGetCurrentResourcePool());
}

bool CharacterLoader_8056B290::fn_8000A790()
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

bool CharacterLoader_8056B290::fn_8000A870()
{
    bool bCreated = false;
    mTemplate = GetCharacterTemplate(mCurrent->cc, &bCreated);
    mTemplateInfo = GetCharacterTemplateInfo(mCurrent->cc);
    return bCreated;
}

static void fn_8000A8C8(void* data, unsigned long size, void* param)
{
    CharacterLoader_8056B290::sUnidentifiedInstance.mModelData[(int)param] = data;
    CharacterLoader_8056B290::sUnidentifiedInstance.mModelSize[(int)param] = size;
}

void CharacterLoader_8056B290::fn_8000A8E4(int nModel)
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
        szFilename = mTemplateInfo->pUnidentified08;
        break;
    case 2:
        szFilename = mTemplateInfo->pUnidentified0C;
        break;
    case 3:
        szFilename = mTemplateInfo->pUnidentified10;
        break;
    }
    if (szFilename != 0)
    {
        glBeginLoadModel(szFilename, fn_8000A8C8, (void*)nModel, glGetCurrentResourcePool());
    }
}

bool CharacterLoader_8056B290::fn_8000A9A4(int nModel)
{
    const char* szFilename = 0;
    switch (nModel)
    {
    case 0:
        szFilename = mTemplateInfo->szModelFilename;
        break;
    case 1:
        szFilename = mTemplateInfo->pUnidentified08;
        break;
    case 2:
        szFilename = mTemplateInfo->pUnidentified0C;
        break;
    case 3:
        szFilename = mTemplateInfo->pUnidentified10;
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

static void fn_8000AAA4(void* data, unsigned long size, void* param)
{
    CharacterLoader_8056B290::sUnidentifiedInstance.mHierarchyData = data;
    CharacterLoader_8056B290::sUnidentifiedInstance.mHierarchySize = size;
}

unsigned int CharacterLoader_8056B290::fn_8000AAB8()
{
    mHierarchyData = 0;
    mHierarchySize = 0;
    CurrentAllocator = &StandardAllocator;
    AllocatorStack[AllocatorStackDepth++] = &StandardAllocator;
    return nlLoadEntireFileAsync(mTemplateInfo->szHierarchyFilename, fn_8000AAA4,
        mCurrent, 0x20, AllocateStart, 0, 0, 0);
}

bool CharacterLoader_8056B290::fn_8000AB18()
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

static void fn_8000ACD8(void* data, unsigned long size, void* param)
{
    CharacterLoader_8056B290::sUnidentifiedInstance.mPhysicsData = data;
    CharacterLoader_8056B290::sUnidentifiedInstance.mPhysicsSize = size;
}

unsigned int CharacterLoader_8056B290::fn_8000ACEC()
{
    mPhysicsData = 0;
    mPhysicsSize = 0;
    CurrentAllocator = &StandardAllocator;
    AllocatorStack[AllocatorStackDepth++] = &StandardAllocator;
    return nlLoadEntireFileAsync(mTemplateInfo->szPhysicsFilename, fn_8000ACD8,
        mCurrent, 0x20, AllocateEnd, 0, 0, 0);
}

bool CharacterLoader_8056B290::fn_8000AD4C()
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

bool CharacterLoader_8056B290::fn_8000AE04()
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

static void fn_8000AE7C(void* data, unsigned long size, void* param)
{
    CharacterLoader_8056B290::sUnidentifiedInstance.mAnimData = data;
    CharacterLoader_8056B290::sUnidentifiedInstance.mAnimSize = size;
}

void CharacterLoader_8056B290::fn_8000AE90()
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
            nlStrNCpy(szPath, szAnimFilename, sizeof(szPath));
            *strstr(szPath, ".zlib") = '\0';
            fn_80372B4C(szPath, fn_8000AE7C, mCurrent, 0x20, AllocateStart, 0, 0, 0);
        }
        else
        {
            nlLoadCompressedFileAsync(szAnimFilename, fn_8000AE7C, mCurrent, 0x20,
                AllocateStart, 0x40000, 0, 0, 0, 0, 0);
        }
    }
    else
    {
        nlLoadEntireFileAsync(szAnimFilename, fn_8000AE7C, mCurrent, 0x20,
            AllocateStart, 0, 0, 0);
    }
}

bool CharacterLoader_8056B290::fn_8000B00C()
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

static void fn_8000B0D4(void* data, unsigned long size, void* param)
{
    CharacterLoader_8056B290::sUnidentifiedInstance.mTriggerData = data;
    CharacterLoader_8056B290::sUnidentifiedInstance.mTriggerSize = size;
}

void CharacterLoader_8056B290::fn_8000B0E8()
{
    Entry* pEntry = mCurrent;
    mTriggerData = 0;
    mTriggerSize = 0;
    nlLoadEntireFileAsync(GetCharacterTemplateInfo(pEntry->cc)->szTriggerFilename, fn_8000B0D4,
        pEntry, 0x20, AllocateEnd, 0, 0, 0);
}

bool CharacterLoader_8056B290::fn_8000B14C()
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

bool CharacterLoader_8056B290::fn_8000B1B8()
{
    if (mTemplateInfo->szAnimRetargetFilename != 0)
    {
        return true;
    }
    mTemplate->pAnimRetargetListInventory = 0;
    return false;
}

static void fn_8000B1E4(void* data, unsigned long size, void* param)
{
    CharacterLoader_8056B290::sUnidentifiedInstance.mAnimRetargetData = data;
    CharacterLoader_8056B290::sUnidentifiedInstance.mAnimRetargetSize = size;
}

unsigned int CharacterLoader_8056B290::fn_8000B1F8()
{
    mAnimRetargetData = 0;
    mAnimRetargetSize = 0;
    return nlLoadEntireFileAsync(mTemplateInfo->szAnimRetargetFilename, fn_8000B1E4,
        mCurrent, 0x20, AllocateStart, 0, 0, 0);
}

bool CharacterLoader_8056B290::fn_8000B230()
{
    if (mAnimRetargetData == 0)
    {
        return false;
    }

    mTemplate->pAnimRetargetListInventory = new (nlMalloc(sizeof(cInventory<AnimRetargetList>), 8, false)) cInventory<AnimRetargetList>();
    mTemplate->pAnimRetargetListInventory->AddFile((char*)mAnimRetargetData, mAnimRetargetSize);
    return true;
}

bool CharacterLoader_8056B290::fn_8000B3C0()
{
    return mCurrent->bSidekick;
}

static void fn_8000B3CC(void* data, unsigned long size, void* param)
{
    CharacterLoader_8056B290::sUnidentifiedInstance.mSidekickTextureData = data;
    CharacterLoader_8056B290::sUnidentifiedInstance.mSidekickTextureSize = size;
}

bool CharacterLoader_8056B290::fn_8000B3E0()
{
    char szArtPath[64];
    char szPlayerPath[64];

    Entry* pEntry = mCurrent;
    eCharacterClass captaincc = captain[pEntry->nTeamID];
    eCharacterClass cc = pEntry->cc;
    const char* szName = GetCharacterInfo(cc).mName;
    const char* szCaptainName = GetCharacterInfo(captaincc).mName;
    const char* szTexName = (cc == 13) ? "hammer" : szName;

    eCharacterClass captain0 = captain[0];
    eCharacterClass captain1 = captain[1];
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
        if (GetAlternateCaptain(captain0, captain1) == captaincc)
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

    if (!glBeginLoadTextureBundle(szArtPath, fn_8000B3CC, mCurrent, glGetCurrentResourcePool()))
    {
        pChar->fn_80022DAC((unsigned long)-1);
        pChar->fn_80022DE8((unsigned long)-1);
        return false;
    }
    return true;
}

bool CharacterLoader_8056B290::fn_8000B6C4()
{
    char szBundlePath[64];
    char szPlayerPath[64];

    if (mSidekickTextureData == 0)
    {
        return false;
    }

    Entry* pEntry = mCurrent;
    eCharacterClass captaincc = captain[pEntry->nTeamID];
    eCharacterClass cc = pEntry->cc;

    glEndLoadTextureBundle(mSidekickTextureData, mSidekickTextureSize, glGetCurrentResourcePool(), false);
    nlFree(mSidekickTextureData);
    mSidekickTextureData = 0;

    cCharacter* pChar = g_pCharacters[mCurrent->nCharIdx];
    const char* szName = GetCharacterInfo(cc).mName;
    const char* szCaptainName = GetCharacterInfo(captaincc).mName;
    const char* szTexName = (cc == 13) ? "hammer" : szName;

    eCharacterClass captain0 = captain[0];
    eCharacterClass captain1 = captain[1];
    if (captain0 == captain1)
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
        if (GetAlternateCaptain(captain0, captain1) == captaincc)
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

void CharacterLoader_8056B290::fn_8000B8E8()
{
    if (mCurrent->bGoalie)
    {
        mTemplate->pUnidentified30 = new (nlMalloc(sizeof(GoalieTweaks), 8, false))
            GoalieTweaks(mTemplateInfo->szTweaksFilename, mTemplateInfo->pUnidentified48);
        mTemplate->pUnidentified28 = 0;
        mTemplate->pUnidentified2C = 0;
    }
    else
    {
        mTemplate->pUnidentified28 = new (nlMalloc(sizeof(PlayerTweaks), 8, false))
            PlayerTweaks(mTemplateInfo->szTweaksFilename, mTemplateInfo->pUnidentified48);
        if (mTemplateInfo->pUnidentified4C != 0)
        {
            mTemplate->pUnidentified2C = new (nlMalloc(sizeof(PlayerTweaks), 8, false))
                PlayerTweaks(mTemplateInfo->pUnidentified4C, mTemplateInfo->pUnidentified50);
        }
        else
        {
            mTemplate->pUnidentified2C = 0;
        }
        mTemplate->pUnidentified30 = 0;
    }
}

bool CharacterLoader_8056B290::fn_8000B9F4()
{
    return gTweakFileLoader.ProcessLoadedFiles();
}

void CharacterLoader_8056B290::fn_8000BA00()
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

    cInventory<cSHierarchy>* pHierInv = pTemplate->pHierarchyInventory;
    u32 hash = nlStringHash(pInfo->szHierarchy);
    cSHierarchy* pHierarchy = pHierInv->Find((unsigned int)hash);

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
            pTemplate->pPhysicsData, pTemplate->pUnidentified30, pAnimRetargetList,
            mCurrent->nCharIdx);
        pGoalie->m_szEffectsName = pInfo->szEffectsName;
        pGoalie->fn_80022DAC(GetHashFromTextureFile(pInfo->szTextureFilename));
        pGoalie->fn_80022DE8(GetHashFromTextureFile(GetGoalieTemplateInfo(goalieIdx)->szTextureFilename));

        g_pCharacters[mCurrent->nCharIdx] = pGoalie;
        g_pCharacters[mCurrent->nCharIdx]->SetPosition(goaliepos[mCurrent->nTeamID]);
        g_pTeams[mCurrent->nTeamID]->SetGoalie(pGoalie);
        fn_800957E4(g_pCharacters[mCurrent->nCharIdx], g_pTeams[mCurrent->nTeamID]);

        if (mCurrent->bGoalie && mCurrent->cc != 20)
        {
            char szTexPath[64];
            char szSwapPath[64];
            cCharacter* pChar = g_pCharacters[mCurrent->nCharIdx];
            const char* szName = GetCharacterInfo(mCurrent->cc).mName;
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
            pTemplate->pUnidentified28, pTemplate->pUnidentified2C, pAnimRetargetList,
            mCurrent->nCharIdx);
        pFielder->m_szEffectsName = pInfo->szEffectsName;

        g_pCharacters[mCurrent->nCharIdx] = pFielder;
        g_pCharacters[mCurrent->nCharIdx]->SetPosition(pos[mCurrent->nCharIdx]);
        g_pTeams[mCurrent->nTeamID]->SetPlayer((cPlayer*)g_pCharacters[mCurrent->nCharIdx], mCurrent->nPlayerID);
        fn_800957E4(g_pCharacters[mCurrent->nCharIdx], g_pTeams[mCurrent->nTeamID]);
    }
}

void CharacterLoader_8056B290::fn_8000BD70()
{
}

static void fn_8000BD74(void* data, unsigned long size, void* param)
{
    CharacterLoader_8056B290::sUnidentifiedInstance.mAltTextureData = data;
    CharacterLoader_8056B290::sUnidentifiedInstance.mAltTextureSize = size;
}

bool CharacterLoader_8056B290::fn_8000BD88()
{
    Entry* pEntry = mCurrent;
    if (pEntry->bCaptain || pEntry->bGoalie)
    {
        eCharacterClass captain0 = captain[0];
        eCharacterClass captain1 = captain[1];
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
                const char* szFilename = GetCharacterTemplateInfo(altcaptain)->pUnidentified18;
                if (szFilename != 0 && nlFileExists(szFilename))
                {
                    return true;
                }
            }
        }
        else if (mCurrent->bGoalie)
        {
            if (altcaptain != CHARACTER_CLASS_INVALID
                && mCurrent->nTeamID == (captain[0] != altcaptain))
            {
                const char* szFilename = GetGoalieTemplateInfo(mCurrent->cc - 20)->pUnidentified08;
                if (szFilename != 0 && nlFileExists(szFilename))
                {
                    return true;
                }
            }
        }
    }
    return false;
}

bool CharacterLoader_8056B290::fn_8000BF04()
{
    Entry* pEntry = mCurrent;
    mAltTextureData = 0;
    mAltTextureSize = 0;
    const char* szFilename = 0;
    if (pEntry->bCaptain)
    {
        szFilename = GetCharacterTemplateInfo(pEntry->cc)->pUnidentified18;
    }
    else if (pEntry->bGoalie)
    {
        szFilename = GetGoalieTemplateInfo(pEntry->cc - 20)->pUnidentified08;
    }
    glBeginLoadTextureBundle(szFilename, fn_8000BD74, mCurrent, glGetCurrentResourcePool());
    return true;
}

bool CharacterLoader_8056B290::fn_8000BFA0()
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

static void fn_8000C0EC(AudioResourceLoadOwner* handle, void* context)
{
    CharacterLoader_8056B290::sUnidentifiedInstance.mAudioCompletedCount = (int)context;
}

void CharacterLoader_8056B290::fn_8000C0FC()
{
    mAudioRequestCount += gAudioEnabled;
    LoadSoundBank((GameAudio*)g_pAudioSystem, 0, 0, fn_8000C0EC,
        (void*)mAudioRequestCount);
}

bool CharacterLoader_8056B290::fn_8000C124()
{
    return mCurrent->bCaptain;
}

void CharacterLoader_8056B290::fn_8000C130()
{
    int nBank = GetCharacterInfo(mCurrent->cc).unknown_0x1C;
    mAudioRequestCount += gAudioEnabled;
    LoadSoundBank((GameAudio*)g_pAudioSystem, nBank,
        (mCurrent->nTeamID == 0) ? 1 : 5, fn_8000C0EC, (void*)mAudioRequestCount);
}

bool CharacterLoader_8056B290::fn_8000C1A4()
{
    return mCurrent->bSidekick;
}

void CharacterLoader_8056B290::fn_8000C1B0()
{
    int nBank = GetCharacterInfo(mCurrent->cc).unknown_0x1C;
    mAudioRequestCount += gAudioEnabled;
    Entry* pEntry = mCurrent;
    int nSlot = (pEntry->nTeamID == 0) ? 1 : 5;
    nSlot += pEntry->nPlayerID;
    LoadSoundBank((GameAudio*)g_pAudioSystem, nBank, nSlot, fn_8000C0EC,
        (void*)mAudioRequestCount);
}

void CharacterLoader_8056B290::fn_8000C22C()
{
    mAudioRequestCount += gAudioEnabled;
    LoadSoundBank((GameAudio*)g_pAudioSystem, 13, 9, fn_8000C0EC,
        (void*)mAudioRequestCount);
}

bool CharacterLoader_8056B290::fn_8000C254()
{
    if (mAudioCompletedCount >= mAudioRequestCount)
    {
        mAudioCompletedCount = -1;
        mAudioRequestCount = -1;
        return true;
    }
    return false;
}

static TweakBoolBinding sUnidentifiedLoadAnimsCachedTweak(
    "g_bLoadAnimsCached", "FileCache", &g_bLoadAnimsCached, true);

CharacterLoader_8056B290 CharacterLoader_8056B290::sUnidentifiedInstance;

#include "NL/nlstring_impl.h"

int nlPrintf(const char* format, ...)
{
    return 0;
}
