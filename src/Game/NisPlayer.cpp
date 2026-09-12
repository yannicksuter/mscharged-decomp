#include "Game/NisPlayer.h"
#include "unclassified/tu_80284A58.h"
#include "Game/CharacterTemplate.h"
#include "Game/Player.h"
#include "Game/Game.h"
#include "Game/Render/ShootToScoreArrow.h"
#include "unclassified/tu_800A9B78.h"
#include "Game/GameInfo.h"
#include "Game/DB/StadiumInfo.h"
#include "NL/nlConfig.h"

#include <string.h>
#include <stdio.h>

#include "Game/AI/Fielder.h"
#include "Game/Goalie.h"
#include "Game/Team.h"
#include "Game/TweakQuery.h"

#include "Game/Effects/EmissionManager.h"
#include "Game/Render/tu_80283D9C.h"
#include "NL/MemAlloc.h"
#include "NL/nlDebug.h"
#include "NL/nlFile.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"
#include "NL/nlstring_tmpl.h"

#include "Game/Camera/CameraMan.h"
#include "Game/Render/depthoffield.h"
#include "Game/Sys/audio.h"
#include "Game/Sys/simpleparser.h"
#include "NL/glx/glxSend.h"

extern "C" bool lbl_806DCD60;
void fn_8028346C();

extern "C" {
void fn_8027F018(void*, unsigned long, void*);
void fn_8027F064(void*, unsigned long, void*);
void fn_8027F084(void*, unsigned long, void*);
void fn_8027F0D8(void*, unsigned long, void*);
void fn_8027F12C(void*, unsigned long, void*);
void fn_8027F174(glModel*);
}
extern void (*lbl_806E217C)(glModel*);

NisPlayer* NisPlayer::sInstance;
bool g_ForceDoubleBallTransition;

bool NisPlayer::WorldIsFrozen() const
{
    for (int i = 0; i < 8; i++)
    {
        if (mPlaying[i] != 0 && mPlaying[i]->unknown_0x034 != 0)
        {
            return true;
        }
    }
    return false;
}

cAnimCamera* NisPlayer::fn_8027E708()
{
    return &mCamera[1];
}

void NisPlayer::SetExtraNameFilter(const char* filter)
{
    nlStrNCpy(mExtraNameFilter, filter, 128);
}

void NisPlayer::ResetEffects()
{
    EmissionManager::Instance()->Destroy((unsigned long)this, 0);
    EmissionManager::Instance()->DestroyAll(0, true);
    EmissionManager::Instance()->DestroyAll(3, true);
}

int NisPlayer::fn_8027E284(NisWinnerType winnerType) const
{
    if (winnerType < NIS_NUM_WINNER_TYPES)
    {
        return mWinnerSide[winnerType];
    }
    return 0;
}

void NisPlayer::fn_8027D994()
{
    for (int i = 0; i < 10; i++)
    {
        mBeginPositions[i].x = nlRandomf(-8.0f, 8.0f, fn_80287B2C(GetPresentation()));
        mBeginPositions[i].y = nlRandomf(-4.0f, 4.0f, fn_80287B2C(GetPresentation()));
        mBeginPositions[i].z = 0.0f;
    }
}

void NisPlayer::fn_802805B4(NisHeader& nisHeader, NisTarget target, NisUseStadiumOffset useStadiumOffset, NisWinnerType winnerType, int param5, bool param6)
{
    nisHeader.target = target;
    nisHeader.winnerType = winnerType;
    nisHeader.mTime = 0.0f;
    nisHeader.unknown_0x180 = param5;
    if (!param6)
    {
        nisHeader.mUnidentified195 = IsMirrored(target, nisHeader.name, winnerType);
    }
    if (useStadiumOffset == NIS_NO_STADIUM_OFFSET)
    {
        nisHeader.stadiumOffset.x = 0.0f;
        nisHeader.stadiumOffset.y = 0.0f;
        nisHeader.stadiumOffset.z = 0.0f;
    }
    else
    {
        float scale = (useStadiumOffset == NIS_AWAY_STADIUM_OFFSET) ? -1.0f : 1.0f;
        char offsetConfigName[64];
        nlSNPrintf(offsetConfigName, 64, "nisHeader/%s_offset", GetStadiumName(GameInfoManager::Instance()->GetStadium()));
        float offset = scale * GetConfigFloat(Config::Global(), offsetConfigName, 0.0f);
        nisHeader.stadiumOffset.x = 0.0f;
        nisHeader.stadiumOffset.y = offset;
        nisHeader.stadiumOffset.z = 0.0f;
    }
    for (int i = 0; i < nisHeader.numAnimations; i++)
    {
        mBeginPositions[i] = nisHeader.beginPositions[i];
        if (nisHeader.mUnidentified195)
        {
            mBeginPositions[i].x *= -1.0f;
        }
    }
    if (nisHeader.unknown_0x198 != NULL)
    {
        Load(nisHeader.unknown_0x198, nisHeader.unknown_0x19C, nisHeader);
    }
    else
    {
        for (int i = 0; i < 8; i++)
        {
            if (mLoadQueue[i] == NULL)
            {
                mLoadQueue[i] = &nisHeader;
                break;
            }
        }
    }
}

void NisPlayer::fn_8027DFE4(cPlayer* param1)
{
    if (g_pGame == NULL)
    {
        return;
    }
    if (g_pGame->m_eGameState == 3)
    {
        return;
    }
    g_ForceDoubleBallTransition = false;
    if (param1 != NULL)
    {
        mUnidentified34238 = param1->m_pTeam->m_nSide;
        mUnidentified340C0 = GetCharacterIndex(param1);
    }
}

void NisPlayer::Load(char* buffer, unsigned int size, NisHeader& nisHeader)
{
    if (!mActive)
        return;

    for (int i = 0; i < 8; i++)
    {
        if (mLoaded[i] != 0)
            continue;

        if (nisHeader.unknown_0x198 == 0)
        {
            for (int j = 0; j < 8; j++)
            {
                if (&nisHeader == mLoadQueue[j])
                {
                    mLoadQueue[j] = 0;
                    mAsyncStarted[j] = false;
                    break;
                }
            }
        }

        Nis* nis = new (nlMalloc(sizeof(Nis), 8, false))
            Nis(nisHeader, buffer, size);
        mLoaded[i] = nis;
        LoadTriggers(*mLoaded[i]);
        return;
    }
}

void NisPlayer::fn_8027CCEC()
{
    for (int i = 0; i < 8; i++)
    {
        mLoadQueue[i] = 0;
    }
}

void NisPlayer::Reset()
{
    if (!mActive)
    {
        return;
    }

    fn_8027D1EC();
    mUnidentified34338 = 0;
    if (mUnidentified34350 != 0 && mUnidentified34358)
    {
        StopSound(mUnidentified34350, (void*)-1);
        mUnidentified34350 = 0;
    }
    if (mUnidentified34354 != 0)
    {
        StopSound(mUnidentified34354, (void*)-1);
        mUnidentified34354 = 0;
    }

    for (int i = 0; i < 8; i++)
    {
        delete mPlaying[i];
        delete mLoaded[i];
        mPlaying[i] = 0;
        mLoaded[i] = 0;
        mLoadQueue[i] = 0;
        mAsyncStarted[i] = false;
    }

    mActive = false;
    mLoadingFromBack = false;
    mUsedFromFront = 0;
    mUsedFromBack = 0x70800;
    for (int i = 0; i < 2; i++)
    {
        mCamera[i].UnselectCameraAnimation();
    }
    cCameraManager::Remove(mCamera[0]);
    fn_8028346C();
    lbl_806DCD60 = true;
    if (mUnidentified343E8 != -1.0f)
    {
        glx_SetFogStart(mUnidentified343E8);
    }
    if (mUnidentified343F0 != -1.0f)
    {
        glx_SetFogEnd(mUnidentified343F0);
    }
    mUnidentified343E8 = -1.0f;
    mUnidentified343F0 = -1.0f;
    DepthOfFieldManager::instance.TurnOn();
    mUnidentified34468 = 0.0f;
}

NisPlayer* NisPlayer::Instance()
{
    if (sInstance == 0)
    {
        sInstance = new NisPlayer;
    }
    return sInstance;
}

void NisPlayer::fn_8027E5D0()
{
}

void NisPlayer::fn_8027E5D4()
{
    for (int i = 0; i < mDictSize; i++)
    {
        delete mDict[i].unknown_0x198;
        mDict[i].mUnidentified194 = false;
        mDict[i].unknown_0x198 = 0;
        mDict[i].unknown_0x19C = 0;
    }
}

void NisPlayerOverlay::Reset()
{
}

extern "C" void fn_8002E3F8(cFielder*);

void NisPlayer::fn_8027D11C()
{
    mUnidentified34468 = 0.0f;
    ResetEffects();
    fn_8027D1EC();
    g_pGame->mUnidentified10DC->fn_800AA3E8(true);
    if (mUnidentified34338 != 4)
    {
        WorldDarkening::Instance().fn_801AF550();
    }
    if (cCameraManager::PeekCamera() != &mCamera[0])
    {
        cCameraManager::Remove(mCamera[0]);
        cCameraManager::PushCamera(&mCamera[0]);
    }
}

void NisPlayer::fn_8027D1EC()
{
    for (int side = 0; side < 2; side++)
    {
        cTeam* team = g_pTeams[side];
        team->GetGoalie()->ResetEffects();
        for (int i = 0; i < 4; i++)
        {
            cFielder* fielder = team->GetFielder(i);
            fielder->ResetEffects();
            fielder->fn_8001EE74(1.0f, 0.0f, 1.0f);
            fn_8002E3F8(fielder);
            fielder->fn_800974B0();
        }
    }
}

NisPlayer::NisPlayer()
    : InterpreterCore(100)
    , unknown_0x00028(0)
    , mActive(false)
    , mDictSize(0)
    , mMaxNumBallsVisible(1)
    , mLoadingFromBack(false)
    , mUsedFromFront(0)
    , mUsedFromBack(0x70800)
    , mUnidentified340C0(-1)
    , mUnidentified34334(0)
    , mUnidentified34338(1)
    , mUnidentified34350(0)
    , mUnidentified34354(0)
    , mUnidentified34358(true)
    , mUnidentified34359(false)
    , mUnidentified3435A(false)
    , mUnidentified3439C(false)
    , mUnidentified343E8(-1.0f)
    , mUnidentified343F0(-1.0f)
    , mUnidentified343F4(-1)
    , mUnidentified34468(0.0f)
{
    mMemory = (char*)nlMalloc(0x70800, 8, false);
    unknown_0x00028 = 0;
    mUnidentified34334 = NULL;
    nlLoadEntireFileAsync("art/Scripts/nis_triggers.byte_code", fn_8027F018, this, 0x20, AllocateEnd, NULL, 0, &VirtualAllocator);
    nlLoadEntireFileAsync("art/Scripts/nis_anim_proxy.byte_code", fn_8027F064, this, 0x20, AllocateEnd, NULL, 0, &VirtualAllocator);
    nlLoadEntireFileAsync("art/nis/do_not_mirror.txt", fn_8027F084, this, 0x20, AllocateEnd, NULL, 0, &VirtualAllocator);
    nlLoadEntireFileAsync("art/nis/do_not_showPiP.txt", fn_8027F0D8, this, 0x20, AllocateEnd, NULL, 0, &VirtualAllocator);
    nlLoadEntireFileAsync("art/nis/nis_dict.txt", fn_8027F12C, this, 0x20, AllocateEnd, NULL, 0, &VirtualAllocator);
    for (int i = 0; i < 8; i++)
    {
        mLoadQueue[i] = NULL;
        mPlaying[i] = NULL;
        mLoaded[i] = NULL;
        mAsyncStarted[i] = false;
    }
    Reset();
    for (int i = 0; i < 2; i++)
    {
        mCamera[i].m_LetManagerDoUpdate = false;
        mCamera[i].m_bCyclic = false;
    }
    mUnidentified3433C[0] = new (8, false) NisPlayerOverlay_80523878(this);
    mUnidentified3433C[1] = new (8, false) NisPlayerOverlay_8052385C(this);
    mUnidentified3433C[4] = new (8, false) NisPlayerOverlay_80523840(this);
    mUnidentified3433C[2] = new (8, false) NisPlayerOverlay_80523824(this);
    mUnidentified3433C[3] = new (8, false) NisPlayerOverlay_80523808(this, 1.0f);
    mUnidentified34359 = GetTweakBool("/user/DisplayNISInfo", false);
    mUnidentified343E0 = false;
    mUnidentified343E4 = -1.0f;
    mUnidentified343EC = -1.0f;
    for (int i = 0; i < 10; i++)
    {
        mUnidentified34440[i] = NULL;
    }
    lbl_806E217C = fn_8027F174;
    mUnidentified343F8 = false;
}

inline bool NisPlayer::fn_8027E0AC(const char* name) const
{
    for (int i = 0; i < mUnidentified340B0; i++)
    {
        if (nlStrICmp(name, mUnidentified340AC[i]) == 0)
        {
            return false;
        }
    }
    return true;
}

bool NisPlayer::fn_8027E0AC()
{
    for (int i = 0; i < 8; i++)
    {
        if (mLoaded[i] != NULL && !fn_8027E0AC(mLoaded[i]->Name()))
        {
            return false;
        }
        if (mLoadQueue[i] != NULL && !fn_8027E0AC(mLoadQueue[i]->name))
        {
            return false;
        }
    }
    return true;
}

NisPlayer::~NisPlayer()
{
    delete[] mUnidentified340A4;
    delete[] mUnidentified340AC;
    for (int i = 0; i < 10; i++)
    {
        mUnidentified34440[i] = NULL;
    }
}

bool NisPlayer::IsMirrored(NisTarget target, const char* name, NisWinnerType winnerType) const
{
    for (int i = 0; i < mUnidentified340A8; i++)
    {
        if (nlStrICmp(name, mUnidentified340A4[i]) == 0)
        {
            return false;
        }
    }
    bool mirrored = true;
    if (strstr(name, "_goal_") == NULL && strstr(name, "goalie_") == NULL)
    {
        mirrored = false;
    }
    if (target == NIS_TARGET_UNIDENTIFIED_14 || target == NIS_TARGET_WINNER_CAPTAIN || target == NIS_TARGET_WINNER_SIDEKICK || target == NIS_TARGET_LOSER_GOALIE)
    {
        if (fn_8027E284(winnerType) == 0)
        {
            return mirrored;
        }
        return !mirrored;
    }
    if (target == NIS_TARGET_LOSER_CAPTAIN || target == NIS_TARGET_WINNER_GOALIE || target == NIS_TARGET_LOSER_SIDEKICK)
    {
        if (fn_8027E284(winnerType) == 0)
        {
            return !mirrored;
        }
        return mirrored;
    }
    if (target == NIS_TARGET_UNIDENTIFIED_21)
    {
        if (strstr(name, "_megastrike_") != NULL)
        {
            return false;
        }
        return mUnidentified34238 == 0;
    }
    if (target == NIS_TARGET_UNIDENTIFIED_22)
    {
        return mUnidentified34238 == 0;
    }
    if (strstr(name, "cup_win_home") != NULL)
    {
        return false;
    }
    if (strstr(name, "home") != NULL || strstr(name, "run_to_center") != NULL)
    {
        if (target == NIS_TARGET_AWAY_CAPTAIN)
        {
            return true;
        }
        if (target == NIS_TARGET_AWAY_SIDEKICK)
        {
            return true;
        }
        if (target == NIS_TARGET_NONE)
        {
            return true;
        }
    }
    return false;
}

namespace
{
static void* byteCode;
}

void NisPlayer::fn_8027B630(char* data, unsigned long size)
{
    SimpleParser parser;
    const char* separators = "\n\r";
    parser.StartParsing(data, size + 1, separators);
    int count = 0;
    bool more = parser.AdvanceLine();
    while (more)
    {
        count++;
        more = parser.AdvanceLine();
    }
    mUnidentified340A8 = count;
    parser.StartParsing(data, size + 1, separators);
    mUnidentified340A4 = new (8, false) char*[count];
    char* token = parser.NextTokenOnLine(false);
    int i = 0;
    while (token != NULL)
    {
        char* name = new (8, false) char[parser.GetTokenLength() + 1];
        memcpy(name, token, parser.GetTokenLength());
        name[parser.GetTokenLength()] = 0;
        mUnidentified340A4[i] = name;
        parser.AdvanceLine();
        token = parser.NextTokenOnLine(false);
        i++;
    }
    nlFree(data);
}

void NisPlayer::fn_8027B758(char* data, unsigned long size)
{
    SimpleParser parser;
    const char* separators = "\n\r";
    parser.StartParsing(data, size + 1, separators);
    int count = 0;
    bool more = parser.AdvanceLine();
    while (more)
    {
        count++;
        more = parser.AdvanceLine();
    }
    mUnidentified340B0 = count;
    parser.StartParsing(data, size + 1, separators);
    mUnidentified340AC = new (8, false) char*[count];
    char* token = parser.NextTokenOnLine(false);
    int i = 0;
    while (token != NULL)
    {
        char* name = new (8, false) char[parser.GetTokenLength() + 1];
        memcpy(name, token, parser.GetTokenLength());
        name[parser.GetTokenLength()] = 0;
        mUnidentified340AC[i] = name;
        parser.AdvanceLine();
        token = parser.NextTokenOnLine(false);
        i++;
    }
    nlFree(data);
}

static inline void SkipLine(const char*& data)
{
    while (*data != '\n')
        data++;
    while (*data == '\n')
        data++;
}

void NisPlayer::fn_8027B880(char* data)
{
    if (data != NULL)
    {
        mDictSize = 0;
        const char* dictionaryCursor = data;
        while (mDictSize < 512)
        {
            NisHeader& header = mDict[mDictSize];
            if (sscanf(dictionaryCursor, "name %s", header.name) != 1)
                break;
            SkipLine(dictionaryCursor);
            if (sscanf(dictionaryCursor, "\tsize %d", &header.size) != 1)
                break;
            SkipLine(dictionaryCursor);
            if (sscanf(dictionaryCursor, "\thas_ball %d", &header.numBalls) != 1)
                break;
            SkipLine(dictionaryCursor);
            if (sscanf(dictionaryCursor, "\tnum_animations %d", &header.numAnimations) != 1)
                break;
            SkipLine(dictionaryCursor);
            if (sscanf(dictionaryCursor, "\tnum_cameras %d", &header.numCameras) != 1)
                break;
            SkipLine(dictionaryCursor);
            if (sscanf(dictionaryCursor, "\tcenter %f, %f, %f", &header.center.x, &header.center.y, &header.center.z) != 3)
                break;
            SkipLine(dictionaryCursor);
            if (sscanf(dictionaryCursor, "\tmin_bounds %f, %f, %f", &header.minBounds.x, &header.minBounds.y, &header.minBounds.z) != 3)
                break;
            SkipLine(dictionaryCursor);
            if (sscanf(dictionaryCursor, "\tmax_bounds %f, %f, %f", &header.maxBounds.x, &header.maxBounds.y, &header.maxBounds.z) != 3)
                break;
            SkipLine(dictionaryCursor);

            nlToLower<char>(header.name);

            for (int i = 0; i < header.numAnimations; i++)
            {
                nlVector3 beginPos = { { 0, 0, 0 } };
                int parsedValueCount = sscanf(dictionaryCursor, "\tbegin_pos %f, %f, %f", &beginPos.x, &beginPos.y, &beginPos.z);
                if (parsedValueCount != 3)
                    break;
                SkipLine(dictionaryCursor);
                header.beginPositions[i] = beginPos;
            }
            sscanf(dictionaryCursor, "\tnum_anim_proxies %d", &header.mUnidentified0A4);
            SkipLine(dictionaryCursor);
            for (int i = 0; i < header.mUnidentified0A4; i++)
            {
                sscanf(dictionaryCursor, "\tanim_proxy_name %s", header.mUnidentified0A8[i]);
                SkipLine(dictionaryCursor);
                sscanf(dictionaryCursor, "\tanim_proxy_position %f %f", &header.mUnidentified128[i].x, &header.mUnidentified128[i].y);
                SkipLine(dictionaryCursor);
                int direction;
                sscanf(dictionaryCursor, "\tanim_proxy_direction %d", &direction);
                header.mUnidentified168[i] = direction;
                SkipLine(dictionaryCursor);
            }
            header.unknown_0x198 = 0;
            header.unknown_0x19C = 0;
            header.mUnidentified194 = false;
            mDictSize++;
        }
        nlFree(data);
    }
}

void NisPlayer::AsyncLoad(nlFile* file, void* buffer, unsigned int size, unsigned long param)
{
    if (file != NULL)
    {
        nlClose(file);
    }
    Instance()->Load((char*)buffer, size, *(NisHeader*)param);
}

extern "C" void fn_8027F018(void* data, unsigned long size, void* userData)
{
    NisPlayer* player = (NisPlayer*)userData;
    if (size != 0)
    {
        byteCode = data;
        player->LoadByteCode(data);
        player->unknown_0x00028 |= 1;
    }
}

extern "C" void fn_8027F064(void* data, unsigned long size, void* userData)
{
    NisPlayer* player = (NisPlayer*)userData;
    if (size != 0)
    {
        player->mUnidentified34334 = data;
        player->unknown_0x00028 |= 2;
    }
}

extern "C" void fn_8027F084(void* data, unsigned long size, void* userData)
{
    NisPlayer* player = (NisPlayer*)userData;
    if (size != 0)
    {
        player->fn_8027B630((char*)data, size);
        player->unknown_0x00028 |= 4;
    }
}

namespace
{
static char kNisEmpty[] = "";
}

void NisPlayer::DoFunctionCall(unsigned int functionId)
{
    switch (functionId)
    {
    case 0:
    {
        bool triggerParam1 = m_SP[-1] != 0;
        float frame = ((float*)m_SP)[-2];
        m_SP -= 2;

        Nis::TriggerParams params;
        params.float1 = -1.0f;
        params.param1 = -1;
        params.param2 = -1;
        params.param3 = -1;
        params.param4 = -1;
        params.param1 = triggerParam1;
        mNisForTriggerLoading->AddTrigger(NIS_TRIGGER_TYPE_UNIDENTIFIED_8, frame, kNisEmpty, kNisEmpty, &params);
        break;
    }
    case 1:
    {
        float frame = ((float*)m_SP)[-1];
        m_SP -= 1;

        Nis::TriggerParams params;
        params.float1 = -1.0f;
        params.param1 = -1;
        params.param2 = -1;
        params.param3 = -1;
        params.param4 = -1;
        mNisForTriggerLoading->AddTrigger(NIS_TRIGGER_TYPE_UNIDENTIFIED_9, frame, kNisEmpty, kNisEmpty, &params);
        break;
    }
    case 2:
    {
        unsigned long triggerParam1 = m_SP[-1];
        const char* target = (const char*)m_SP[-2];
        const char* name = (const char*)m_SP[-3];
        float frame = ((float*)m_SP)[-4];
        m_SP -= 4;

        Nis::TriggerParams params;
        params.float1 = -1.0f;
        params.param1 = -1;
        params.param2 = -1;
        params.param3 = -1;
        params.param4 = -1;
        params.param1 = triggerParam1;
        mNisForTriggerLoading->AddTrigger(NIS_TRIGGER_TYPE_EFFECT, frame, name, target, &params);
        break;
    }
    case 3:
    {
        unsigned long triggerParam2 = m_SP[-1];
        unsigned long triggerParam1 = m_SP[-2];
        float frame = ((float*)m_SP)[-3];
        m_SP -= 3;

        Nis::TriggerParams params;
        params.float1 = -1.0f;
        params.param1 = -1;
        params.param2 = -1;
        params.param3 = -1;
        params.param4 = -1;
        params.param1 = triggerParam1;
        params.param2 = triggerParam2;
        mNisForTriggerLoading->AddTrigger(NIS_TRIGGER_TYPE_PLAY_SOUND, frame, kNisEmpty, kNisEmpty, &params);
        break;
    }
    case 4:
    {
        const char* target = (const char*)m_SP[-1];
        const char* name = (const char*)m_SP[-2];
        float frame = ((float*)m_SP)[-3];
        m_SP -= 3;

        mNisForTriggerLoading->AddTrigger(NIS_TRIGGER_TYPE_RAISE_EVENT, frame, name, target, NULL);
        break;
    }
    case 5:
    {
        float value = ((float*)m_SP)[-1];
        float frame = ((float*)m_SP)[-2];
        m_SP -= 2;

        Nis::TriggerParams params;
        params.float1 = -1.0f;
        params.param1 = -1;
        params.param2 = -1;
        params.param3 = -1;
        params.param4 = -1;
        params.float1 = value;
        if (params.float1 > 1.0f)
        {
            params.float1 = 1.0f;
        }
        if (params.float1 < 0.0f)
        {
            params.float1 = 0.0f;
        }
        mNisForTriggerLoading->AddTrigger(NIS_TRIGGER_TYPE_UNIDENTIFIED_7, frame, kNisEmpty, kNisEmpty, &params);
        break;
    }
    case 6:
    {
        bool triggerParam1 = m_SP[-1] != 0;
        float frame = ((float*)m_SP)[-2];
        m_SP -= 2;

        Nis::TriggerParams params;
        params.float1 = -1.0f;
        params.param1 = -1;
        params.param2 = -1;
        params.param3 = -1;
        params.param4 = -1;
        params.param1 = triggerParam1;
        mNisForTriggerLoading->AddTrigger(NIS_TRIGGER_TYPE_UNIDENTIFIED_5, frame, kNisEmpty, kNisEmpty, &params);
        break;
    }
    case 7:
    {
        unsigned long triggerParam1 = m_SP[-1];
        float frame = ((float*)m_SP)[-2];
        m_SP -= 2;

        Nis::TriggerParams params;
        params.float1 = -1.0f;
        params.param1 = -1;
        params.param2 = -1;
        params.param3 = -1;
        params.param4 = -1;
        params.param1 = triggerParam1;
        mNisForTriggerLoading->AddTrigger(NIS_TRIGGER_TYPE_UNIDENTIFIED_4, frame, kNisEmpty, kNisEmpty, &params);
        break;
    }
    case 8:
    {
        unsigned long triggerParam1 = m_SP[-1];
        float frame = ((float*)m_SP)[-2];
        m_SP -= 2;

        Nis::TriggerParams params;
        params.float1 = -1.0f;
        params.param1 = -1;
        params.param2 = -1;
        params.param3 = -1;
        params.param4 = -1;
        params.param1 = triggerParam1;
        mNisForTriggerLoading->AddTrigger(NIS_TRIGGER_TYPE_UNIDENTIFIED_6, frame, kNisEmpty, kNisEmpty, &params);
        break;
    }
    case 9:
    {
        float frame = ((float*)m_SP)[-1];
        m_SP -= 1;

        Nis::TriggerParams params;
        params.float1 = -1.0f;
        params.param1 = -1;
        params.param2 = -1;
        params.param3 = -1;
        params.param4 = -1;
        mNisForTriggerLoading->AddTrigger(NIS_TRIGGER_TYPE_UNIDENTIFIED_10, frame, kNisEmpty, kNisEmpty, &params);
        break;
    }
    case 10:
    {
        float delta = ((float*)m_SP)[-1];
        float frame = ((float*)m_SP)[-2];
        m_SP -= 2;

        Nis::TriggerParams params;
        params.float1 = -1.0f;
        params.param1 = -1;
        params.param2 = -1;
        params.param3 = -1;
        params.param4 = -1;
        params.float1 = delta;
        mNisForTriggerLoading->AddTrigger(NIS_TRIGGER_TYPE_TIME_DILATION, frame, kNisEmpty, kNisEmpty, &params);
        break;
    }
    default:
        nlBreak();
        break;
    }
}

extern "C" void fn_8027F0D8(void* data, unsigned long size, void* userData)
{
    NisPlayer* player = (NisPlayer*)userData;
    if (size != 0)
    {
        player->fn_8027B758((char*)data, size);
        player->unknown_0x00028 |= 8;
    }
}

extern "C" void fn_8027F12C(void* data, unsigned long size, void* userData)
{
    NisPlayer* player = (NisPlayer*)userData;
    if (size != 0)
    {
        player->fn_8027B880((char*)data);
        player->unknown_0x00028 |= 16;
    }
}
