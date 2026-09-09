#include "Game/AI/Fielder.h"
#include "Game/FE/feHelpFuncs.h"
#include "Game/AnimInventory.h"
#include "Game/Audio/UnidentifiedSoundPools.h"
#include "Game/CharacterTemplate.h"
#include "Game/CharacterTweaks.h"
#include "Game/DB/CharacterInfo.h"
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
#include "Game/UnidentifiedStaticStorage.h"
#include "NL/MemAlloc.h"
#include "NL/gl/gl.h"
#include "NL/gl/glMemory.h"
#include "NL/gl/glModel.h"
#include "NL/gl/glState.h"
#include "NL/gl/glTexture.h"
#include "NL/glx/glxTexture.h"
#include "NL/nlFile.h"
#include "NL/plat/tu_80372B4C.h"
#include "NL/nlMemory.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"
#include "unclassified/tu_80073898.h"

#include <string.h>
#include "NL/nlstring_tmpl.h"

extern "C" bool fn_80073BC0(void* pLoadState);
extern "C" void fn_800957E4(cCharacter* pCharacter, cTeam* pTeam);
extern "C" bool fn_802B3E94(const char* path, LoadAsyncCallback callback,
    void* userData, unsigned int alignment, int allocType,
    unsigned int chunkSize, void* readBuffer0, void* readBuffer1, void*,
    unsigned long param, MemoryAllocator* allocator);
extern "C" void fn_802E67E0(void* data, void* nonResidentData,
    ResourceInterface_802CC094* allocator, bool);

extern bool gAudioEnabled;

static bool g_bLoadAnimsCached;

static int sUnidentifiedEffectsLoadCount;

// Asynchronous per-character creation sequence. Its entries are the ten
// g_pCharacters slots (captain, goalie and three sidekicks per team); each
// step starts one file request whose callback stores the result here, and the
// matching finish step consumes it. No retained code drives the sequence.
class CharacterLoader_8056B290
{
public:
    struct Entry
    {
        /* 0x00 */ int nTeamID;
        /* 0x04 */ int nCharIdx;
        /* 0x08 */ int nPlayerID;
        /* 0x0C */ eCharacterClass cc;
        /* 0x10 */ bool bCaptain;
        /* 0x11 */ bool bGoalie;
        /* 0x12 */ bool bSidekick;
    }; // total size: 0x14

    CharacterLoader_8056B290()
    {
        for (int i = 0; i < 2; i++)
        {
            captain[i] = CHARACTER_CLASS_INVALID;
            for (int j = 0; j < 3; j++)
            {
                sidekick[i][j] = CHARACTER_CLASS_INVALID;
            }
            goalie[i] = CHARACTER_CLASS_INVALID;
        }
        mAudioRequestCount = -1;
        mAudioCompletedCount = -1;
    }
    ~CharacterLoader_8056B290();

    void fn_80009BC8();
    bool fn_80009EFC();
    bool fn_80009F48();
    void fn_80009FCC();
    void fn_8000A0A8();
    bool fn_8000A144();
    bool fn_8000A224();
    bool fn_8000A2FC();
    void fn_8000A324();
    bool fn_8000A378();
    void fn_8000A418();
    bool fn_8000A5D8();
    bool fn_8000A67C();
    bool fn_8000A790();
    bool fn_8000A870();
    void fn_8000A8E4(int nModel);
    bool fn_8000A9A4(int nModel);
    unsigned int fn_8000AAB8();
    bool fn_8000AB18();
    unsigned int fn_8000ACEC();
    bool fn_8000AD4C();
    bool fn_8000AE04();
    void fn_8000AE90();
    bool fn_8000B00C();
    void fn_8000B0E8();
    bool fn_8000B14C();
    bool fn_8000B1B8();
    unsigned int fn_8000B1F8();
    bool fn_8000B230();
    bool fn_8000B3C0();
    bool fn_8000B3E0();
    bool fn_8000B6C4();
    void fn_8000B8E8();
    bool fn_8000B9F4();
    void fn_8000BA00();
    void fn_8000BD70();
    bool fn_8000BD88();
    bool fn_8000BF04();
    bool fn_8000BFA0();
    void fn_8000C0FC();
    bool fn_8000C124();
    void fn_8000C130();
    bool fn_8000C1A4();
    void fn_8000C1B0();
    void fn_8000C22C();
    bool fn_8000C254();

    /* 0x000 */ Entry mEntries[10];
    /* 0x0C8 */ int mCurrentIndex;
    /* 0x0CC */ Entry* mCurrent;
    /* 0x0D0 */ tCharacterTemplate* mTemplate;
    /* 0x0D4 */ tCharacterTemplateInfo* mTemplateInfo;
    /* 0x0D8 */ eCharacterClass captain[2];
    /* 0x0E0 */ eCharacterClass sidekick[2][3];
    /* 0x0F8 */ eCharacterClass goalie[2];
    /* 0x100 */ void* mTextureData;
    /* 0x104 */ unsigned long mTextureSize;
    /* 0x108 */ void* mAltTextureData;
    /* 0x10C */ unsigned long mAltTextureSize;
    /* 0x110 */ void* mExtraTextureData;
    /* 0x114 */ unsigned long mExtraTextureSize;
    /* 0x118 */ void* mEffectsData;
    /* 0x11C */ unsigned int mEffectsLoad;
    /* 0x120 */ void* mEffectsNonResData;
    /* 0x124 */ unsigned int mEffectsNonResLoad;
    /* 0x128 */ void* mModelData[4];
    /* 0x138 */ unsigned long mModelSize[4];
    /* 0x148 */ void* mUnidentified148;
    /* 0x14C */ unsigned long mUnidentified14C;
    /* 0x150 */ void* mHierarchyData;
    /* 0x154 */ unsigned long mHierarchySize;
    /* 0x158 */ void* mPhysicsData;
    /* 0x15C */ unsigned long mPhysicsSize;
    /* 0x160 */ void* mAnimData;
    /* 0x164 */ unsigned long mAnimSize;
    /* 0x168 */ void* mTriggerData;
    /* 0x16C */ unsigned long mTriggerSize;
    /* 0x170 */ void* mAnimRetargetData;
    /* 0x174 */ unsigned long mAnimRetargetSize;
    /* 0x178 */ void* mSidekickTextureData;
    /* 0x17C */ unsigned long mSidekickTextureSize;
    /* 0x180 */ int mAudioRequestCount;
    /* 0x184 */ int mAudioCompletedCount;

    static CharacterLoader_8056B290 sUnidentifiedInstance;
}; // total size: 0x188

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

    fn_80025E9C();

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
        return !fn_80025F48(pEntry->cc - 20)->bLoaded;
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
        fn_802C8204(fn_80025F48(goalieIdx)->szTextureFilename, fn_80009FB8,
            mCurrent, fn_802CC094());
        fn_80025F48(goalieIdx)->bLoaded = 1;
    }
    else
    {
        fn_802C8204(GetCharacterTemplateInfo(pEntry->cc)->szTextureFilename, fn_80009FB8,
            pEntry, fn_802CC094());
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
        fn_802C8204(szFilename, fn_80009FB8, mCurrent, fn_802CC094());
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

    fn_802CDD78(mTextureData, mTextureSize, fn_802CC094(), true);

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
        fn_802CDD78(mTextureData, mTextureSize, fn_802CC094(), true);
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
    fn_802C8204("art/characters/mariogoalie/mariogoalie.rlt", fn_80009FB8,
        mCurrent, fn_802CC094());
}

bool CharacterLoader_8056B290::fn_8000A378()
{
    if (mTextureData == 0)
    {
        return false;
    }

    skiptexture = glGetTexture("mariogoalie/mariogoalie");
    glxTextureLoadCallback_t oldCallback = glx_SetLoadCallback(SidekickTexture_cb);
    fn_802CDD78(mTextureData, mTextureSize, fn_802CC094(), false);
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
    mEffectsNonResLoad = fn_802B3E94(szPath, fn_8000A410, &mEffectsNonResData,
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

    fn_802E67E0(mEffectsData, mEffectsNonResData, fn_802CC094(), true);
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
    return fn_802C8204(szPath, fn_8000A668, mCurrent, fn_802CC094());
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

    fn_802CDD78(mExtraTextureData, mExtraTextureSize, fn_802CC094(), true);

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
    mTemplate = fn_80025F5C(mCurrent->cc, &bCreated);
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
        fn_802C8200(szFilename, fn_8000A8C8, (void*)nModel, fn_802CC094());
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
    glModel* pModel = (glModel*)fn_802C81FC(mModelData[nModel],
        mModelSize[nModel], &numModels, fn_802CC094());
    nlFree(mModelData[nModel]);
    mModelData[nModel] = 0;
    mTemplate->nCharacterModelID[nModel] = pModel->unknown00;
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
            fn_802B3E94(szAnimFilename, fn_8000AE7C, mCurrent, 0x20,
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

    fn_80025E9C()->SetupAnimationTriggers(pData, mTriggerSize,
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

    if (!fn_802C8204(szArtPath, fn_8000B3CC, mCurrent, fn_802CC094()))
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

    fn_802CDD78(mSidekickTextureData, mSidekickTextureSize, fn_802CC094(), false);
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
    return fn_80073BC0(&lbl_8056BA00);
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
    tCharacterTemplate* pTemplate = fn_80025F5C(mCurrent->cc, &bCreated);
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
        pGoalie->fn_80022DE8(GetHashFromTextureFile(fn_80025F48(goalieIdx)->szTextureFilename));

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
                const char* szFilename = fn_80025F48(mCurrent->cc - 20)->pUnidentified08;
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
        szFilename = fn_80025F48(pEntry->cc - 20)->pUnidentified08;
    }
    fn_802C8204(szFilename, fn_8000BD74, mCurrent, fn_802CC094());
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

    fn_802CDD78(mAltTextureData, mAltTextureSize, fn_802CC094(), false);

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

static TweakValueBoolImpl_804F4538 sUnidentifiedLoadAnimsCachedTweak(
    "g_bLoadAnimsCached", "FileCache", &g_bLoadAnimsCached, true);

CharacterLoader_8056B290 CharacterLoader_8056B290::sUnidentifiedInstance;

template struct UnidentifiedSoundPools<UnidentifiedSoundPoolTag>;
template struct UnidentifiedStaticStorage<UnidentifiedStaticTag>;

#include "NL/nlstring_impl.h"
