#include "Game/NisPlayer.h"
#include "NL/nlBasicString.inl"
#include "NL/nlBasicString.h"
#include "Game/Render/StadiumLoading.h"
#include "Game/CharacterTemplate.h"
#include "Game/EventDataTypes.h"
#include "Game/EventRegistry.h"
#include "NL/nlFunction.inl"
#include "unclassified/tu_80284A58.h"
#include "Game/Sys/tweak.h"
#include "Game/Player.h"
#include "Game/ReplayManager.h"
#include "Game/Game.h"
#include "Game/Render/ShootToScoreArrow.h"
#include "Game/Weather.h"
#include "Game/GameInfo.h"
#include "Game/DB/StadiumInfo.h"
#include "Game/DB/CharacterInfo.h"
#include "Game/FE/feHelpFuncs_decl.h"
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
#include "NL/nlMath.h"
#include "NL/nlString.h"
#include "NL/nlTask.h"
#include "NL/nlstring_tmpl.h"

#include "Game/Camera/CameraMan.h"
#include "Game/Render/depthoffield.h"
#include "Game/Sys/audio.h"
#include "Game/Sys/simpleparser.h"
#include "NL/glx/glxSend.h"
#include "NL/gl/glState.h"
#include "NL/gl/glTextureManager.h"
#include "NL/gl/glMaterialParameters.h"
#include "NL/gl/glModel.h"
#include "NL/gl/glTexture.h"

extern "C" bool lbl_806DCD60;
extern "C" unsigned long OSGetConsoleType(void);
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

namespace
{
static unsigned char useAsyncLoading = true;
}

float lbl_806DEE9C = 0.65f;
float lbl_806DEEA0 = 0.65f;

void NisPlayer::fn_8027BD60()
{
}

void NisPlayer::fn_8027ED08()
{
    mUnidentified34358 = false;
}

void NisPlayer::fn_8027E054()
{
    fn_8027ED18();
}

void NisPlayer::fn_8027ED18()
{
    mUnidentified34358 = true;
    if (mUnidentified34350 != 0)
    {
        StopSound(mUnidentified34350, (void*)-1);
        mUnidentified34350 = 0;
    }
}

void NisPlayer::fn_8027DA28()
{
    UnidentifiedFindEvent<GoalScoredData>("GoalScored", -1)->Add(Function<GoalScoredData*>(Bind<void>(MemFun(&NisPlayer::fn_8027DF70), this, placeholder0)), 0, -1);
    UnidentifiedFindEvent<GoalieSaveData>("GoalieSave", -1)->Add(Function<GoalieSaveData*>(Bind<void>(MemFun(&NisPlayer::fn_8027DFE0), this, placeholder0)), 0, -1);
    UnidentifiedFindEvent<cPlayer>("MegaStrikeIntro", -1)->Add(Function<cPlayer*>(Bind<void>(MemFun(&NisPlayer::fn_8027DFE4), this, placeholder0)), 0, -1);
    UnidentifiedFindEvent<UnidentifiedEventNoData>("PauseGame", -1)->Add(Function<FnVoidVoid>(Bind<void>(MemFun(&NisPlayer::fn_8027E054), this)), 0, -1);
}

void NisPlayer::fn_8027BD64()
{
    for (int j = 0; j < 2; j++)
    {
        for (int i = 0; i < 8; i++)
        {
            if (mPlaying[i] != 0
                && (j == mPlaying[i]->unknown_0x034 || (j == 1 && mPlaying[i]->unknown_0x034 == 2))
                && mPlaying[i]->mNumCameras != 0)
            {
                mPlaying[i]->fn_80281C70(mCamera[j]);
                break;
            }
        }
    }
}

bool NisPlayer::fn_8027E64C() const
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

float NisPlayer::TimeLeft() const
{
    float timeLeft = 0.0f;
    for (int i = 0; i < 2; i++)
    {
        float remaining = 0.0f;
        if (mCamera[i].m_pActiveCameraData != 0)
        {
            remaining = mCamera[i].GetUnidentifiedTimeLeft();
        }
        timeLeft = timeLeft >= remaining ? timeLeft : remaining;
    }

    cCameraData* pCameraData = mCamera[0].m_pActiveCameraData;
    if (pCameraData != 0)
    {
        float remaining = mCamera[0].GetUnidentifiedTimeLeft();
        if (remaining < timeLeft)
        {
            timeLeft = remaining;
        }
    }
    return timeLeft;
}

bool NisPlayer::WorldIsFrozen() const
{
    bool stateOK = (nlTaskManager::m_pInstance->mCurrentState == 0x10);
    if (stateOK)
    {
        stateOK = TimeLeft() == 0.0f;
    }
    return stateOK;
}

void NisPlayer::fn_8027EE38()
{
    if (mUnidentified34354 != 0)
    {
        mUnidentified34350 = mUnidentified34354;
        mUnidentified34354 = 0;
        StartTrackedSound(mUnidentified34350, (void*)-1);
    }
}

cAnimCamera* NisPlayer::fn_8027E708()
{
    return &mCamera[1];
}

void NisPlayer::fn_8027DFE0(GoalieSaveData*)
{
}

void NisPlayer::SetExtraNameFilter(const char* filter)
{
    nlStrNCpy(mExtraNameFilter, filter, 128);
}

void NisPlayer::fn_8027EEA0(float param1)
{
    if (mUnidentified34338 == 4)
    {
        WorldDarkening::Instance().Fade(lbl_806DEE9C, param1);
    }
}

void NisPlayer::fn_8027EE60(bool param1)
{
    mUnidentified34338 = 4;
    if (param1)
    {
        WorldDarkening::Instance().Fade(lbl_806DEE9C, lbl_806DEEA0);
    }
}

void NisPlayer::fn_8027EF8C()
{
    for (int i = 0; i < 8; i++)
    {
        if (mPlaying[i] != 0 && mPlaying[i]->unknown_0x034 != 0)
        {
            delete mPlaying[i];
            mPlaying[i] = 0;
        }
    }
}

void NisPlayer::fn_8027EEF0()
{
    WorldDarkening::Instance().fn_801AF550();
    mUnidentified34338 = 0;
    fn_8027EF8C();
}

void NisPlayer::ResetEffects()
{
    EmissionManager::Instance()->Destroy((unsigned long)this, 0);
    EmissionManager::Instance()->DestroyAll(0, true);
    EmissionManager::Instance()->DestroyAll(3, true);
}

void NisPlayer::fn_8027ED70()
{
    mUnidentified34358 = true;
    if (mUnidentified34350 != 0)
    {
        SetSoundCallbackEnabled(mUnidentified34350, (void*)-1, 1);
        mUnidentified34350 = 0;
    }
}

void NisPlayer::fn_8027EDCC(unsigned long param1)
{
    StopSound(param1, (void*)-1);
    if (PrepareTrackedSound(19, param1, 0, "Nis Cue", (void*)-1, true))
    {
        mUnidentified34354 = param1;
    }
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
        mGoalScorerCharIndex = GetCharacterIndex(param1);
    }
}

void NisPlayer::fn_8027DF70(GoalScoredData* goalScoredData)
{
    if (g_pGame == NULL)
    {
        return;
    }
    if (goalScoredData != NULL)
    {
        mGoalScorerCharIndex = GetCharacterIndex(goalScoredData->pLastTouch[goalScoredData->uTeamIndex]);
        mWinnerSide[NIS_GOAL_WINNER] = goalScoredData->uTeamIndex;
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
    g_pGame->mpWeatherManager->Stop(true);
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
    , mGoalScorerCharIndex(-1)
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
    mUnidentified343F8[0] = '\0';
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

float NisPlayer::fn_8027C064(int param1) const
{
    cCameraData* pCameraData = mCamera[param1].m_pActiveCameraData;
    if (pCameraData != 0)
    {
        float timeLeft = mCamera[param1].GetUnidentifiedTimeLeft();

        cCameraData* pCameraData = mCamera[0].m_pActiveCameraData;
        if (pCameraData != 0)
        {
            float remaining = mCamera[0].GetUnidentifiedTimeLeft();
            if (remaining < timeLeft)
            {
                timeLeft = remaining;
            }
        }
        return timeLeft;
    }
    return -1.0f;
}

void NisPlayer::HandleAsyncs()
{
    for (int i = 0; i < 8; i++)
    {
        if (mLoadQueue[i] != 0)
        {
            if (!mAsyncStarted[i])
            {
                mAsyncStarted[i] = 1;
                if (mLoadingFromBack)
                {
                    mUsedFromBack -= mLoadQueue[i]->size;
                    mUsedFromBack -= 0x20;
                }

                int memoryOffset = mLoadingFromBack ? mUsedFromBack : mUsedFromFront;

                char* loadAt = mMemory + memoryOffset;
                loadAt = loadAt + (0x20 - ((unsigned int)loadAt & 0x1F));
                if (!mLoadingFromBack)
                {
                    mUsedFromFront += mLoadQueue[i]->size;
                    mUsedFromFront += 0x20;
                }
                if (mUsedFromFront >= mUsedFromBack)
                {
                    nlBreak();
                }

                char fileName[64];
                nlSNPrintf(fileName, sizeof(fileName), "art/nis/%s", mLoadQueue[i]->name);
                if (useAsyncLoading)
                {
                    nlFile* file = nlOpen(fileName);
                    OSGetConsoleType();
                    nlReadAsync(file, loadAt, mLoadQueue[i]->size, AsyncLoad, (unsigned long)mLoadQueue[i], 0);
                }
                else
                {
                    unsigned long size = 0;
                    nlLoadEntireFile(fileName, &size, 0x20, AllocateEnd, loadAt, AlignUp32(mLoadQueue[i]->size), 0);
                    AsyncLoad(0, loadAt, mLoadQueue[i]->size, (unsigned long)mLoadQueue[i]);
                }
            }
        }
    }
}

void NisPlayer::fn_8027CA44()
{
    for (int i = 0; i < 8; i++)
    {
        if (mLoadQueue[i] != 0)
        {
            return;
        }
    }
    for (int i = 0; i < 8; i++)
    {
        if (mLoaded[i] != 0 && !mLoaded[i]->mUnidentifiedBAC)
        {
            mLoaded[i]->fn_802815E0();
        }
    }
}

void NisPlayer::Update(float deltaT)
{
    float realTimeDelta = nlTaskManager::m_pInstance->mRealTimeDelta;
    float timeDilation = nlTaskManager::m_pInstance->mTimeDilation;
    deltaT = realTimeDelta * timeDilation;
    if (deltaT > 0.5f)
    {
        deltaT = 0.5f;
    }
    deltaT += mUnidentified34468;
    mUnidentified34468 = 0.0f;
    HandleAsyncs();

    for (int i = 0; i < 8; i++)
    {
        if (mLoadQueue[i] != 0)
        {
            mLoadQueue[i]->mTime += deltaT;
        }
    }

    if (nlTaskManager::m_pInstance->mCurrentState == 0x10)
    {
        UpdateStadium(deltaT);
        fn_8027CA44();

        float animTime[2];
        for (int i = 0; i < 2; i++)
        {
            animTime[i] = mCamera[i].GetUnidentifiedAnimationTime();
            if (mCamera[i].m_pActiveCameraData != 0)
            {
                float overrun = mCamera[i].ManualUpdate(deltaT);
                if (i == 0 && overrun > 0.0f)
                {
                    mUnidentified34468 = overrun;
                }
            }
        }
        DepthOfFieldManager::instance.m_fDistanceFromCamera = mCamera[0].GetFocalLength();

        for (int i = 0; i < 8; i++)
        {
            if (mPlaying[i] == 0)
            {
                continue;
            }
            int cameraIndex = mPlaying[i]->unknown_0x034;
            if (cameraIndex == 2)
            {
                cameraIndex = 1;
            }
            mPlaying[i]->Update(deltaT);
            float duration = mCamera[cameraIndex].GetUnidentifiedDuration();
            mPlaying[i]->UpdateTriggers(animTime[cameraIndex], mCamera[cameraIndex].GetUnidentifiedAnimationTime(), duration);
            mCamera[cameraIndex].m_OffsetPos = mPlaying[i]->Offset();
        }

        int overlay = mUnidentified3433C[mUnidentified34338]->Update(deltaT);
        if (mUnidentified34338 != overlay)
        {
            mUnidentified34338 = overlay;
            mUnidentified3433C[mUnidentified34338]->Reset();
            mUnidentified3433C[mUnidentified34338]->GetOverlayType();
        }
    }
}

bool NisPlayer::fn_8027CB44()
{
    if (unknown_0x00028 != 0x1F)
    {
        return false;
    }
    for (int i = 0; i < 8; i++)
    {
        if (mLoadQueue[i] != 0)
        {
            return false;
        }
    }
    for (int i = 0; i < 8; i++)
    {
        if (mLoaded[i] != 0)
        {
            if (!mLoaded[i]->mUnidentifiedBAC)
            {
                mLoaded[i]->fn_802815E0();
            }
            if (mLoaded[i]->fn_80283930() == true)
            {
                return false;
            }
        }
    }
    if (mUnidentified34354 != 0 && GetSoundState(mUnidentified34354, (void*)-1) == 2)
    {
        return false;
    }
    return true;
}

void NisPlayer::Play()
{
    mUnidentified3435A = false;
    mUnidentified3439C = false;
    mActive = true;
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

    for (int j = 0; j < 8; j++)
    {
        delete mPlaying[j];
        mPlaying[j] = mLoaded[j];
        mLoaded[j] = 0;
        if (mPlaying[j] != 0)
        {
            mPlaying[j]->fn_802816CC();
            mPlaying[j]->fn_802834A0();
        }
    }

    if (mUnidentified343E4 != -1.0f)
    {
        mUnidentified343E8 = glx_GetFogStart();
        glx_SetFogStart(mUnidentified343E4);
    }
    if (mUnidentified343EC != -1.0f)
    {
        mUnidentified343F0 = glx_GetFogEnd();
        glx_SetFogEnd(mUnidentified343EC);
    }
    if (mUnidentified343E0 == true)
    {
        lbl_806DCD60 = false;
    }
    mUnidentified343E0 = false;
    mUnidentified343E4 = -1.0f;
    mUnidentified343EC = -1.0f;
    if (mUnidentified34338 != 4)
    {
        mUnidentified34338 = 0;
    }

    if (mLoadingFromBack)
    {
        mLoadingFromBack = false;
        mUsedFromFront = 0;
    }
    else
    {
        mLoadingFromBack = true;
        mUsedFromBack = 0x70800;
    }
    fn_8027BD64();
    fn_8027EE38();
    fn_8028346C();
}

const char* NisPlayer::GetTargetFilter(NisTarget target, NisWinnerType winnerType) const
{
    if (target == NIS_TARGET_STADIUM)
    {
        int stadium = GameInfoManager::Instance()->GetStadium();
        const char* stadiumName = GetStadiumName(stadium);
        return stadiumName;
    }

    if (target == NIS_TARGET_HOME_CAPTAIN)
    {
        return GetTeamName((eTeamID)GameInfoManager::Instance()->GetTeam(0));
    }

    if (target == NIS_TARGET_AWAY_CAPTAIN)
    {
        return GetTeamName((eTeamID)GameInfoManager::Instance()->GetTeam(1));
    }

    if (target == NIS_TARGET_HOME_SIDEKICK)
    {
        return GetSidekickName((eSidekickID)GameInfoManager::Instance()->GetSidekick(0, 0));
    }

    if (target == NIS_TARGET_UNIDENTIFIED_5)
    {
        return GetSidekickName((eSidekickID)GameInfoManager::Instance()->GetSidekick(0, 0));
    }

    if (target == NIS_TARGET_UNIDENTIFIED_6)
    {
        return GetSidekickName((eSidekickID)GameInfoManager::Instance()->GetSidekick(0, 1));
    }

    if (target == NIS_TARGET_UNIDENTIFIED_7)
    {
        return GetSidekickName((eSidekickID)GameInfoManager::Instance()->GetSidekick(0, 2));
    }

    if (target == NIS_TARGET_AWAY_SIDEKICK)
    {
        return GetSidekickName((eSidekickID)GameInfoManager::Instance()->GetSidekick(1, 0));
    }

    if (target == NIS_TARGET_UNIDENTIFIED_9)
    {
        return GetSidekickName((eSidekickID)GameInfoManager::Instance()->GetSidekick(1, 0));
    }

    if (target == NIS_TARGET_UNIDENTIFIED_10)
    {
        return GetSidekickName((eSidekickID)GameInfoManager::Instance()->GetSidekick(1, 1));
    }

    if (target == NIS_TARGET_UNIDENTIFIED_11)
    {
        return GetSidekickName((eSidekickID)GameInfoManager::Instance()->GetSidekick(1, 2));
    }

    if (target == NIS_TARGET_UNIDENTIFIED_14)
    {
        return g_pCharacters[mGoalScorerCharIndex]->mUnidentified11C->mName;
    }

    if (target == NIS_TARGET_WINNER_SIDEKICK)
    {
        return GetSidekickName((eSidekickID)GameInfoManager::Instance()->GetSidekick((short)fn_8027E284(winnerType), 0));
    }

    if (target == NIS_TARGET_LOSER_SIDEKICK)
    {
        int side = (fn_8027E284(winnerType) + 1) % 2;
        return GetSidekickName((eSidekickID)GameInfoManager::Instance()->GetSidekick((short)side, 0));
    }

    if (target == NIS_TARGET_WINNER_CAPTAIN)
    {
        return GetTeamName((eTeamID)GameInfoManager::Instance()->GetTeam((short)fn_8027E284(winnerType)));
    }

    if (target == NIS_TARGET_LOSER_CAPTAIN)
    {
        int side = (fn_8027E284(winnerType) + 1) % 2;
        return GetTeamName((eTeamID)GameInfoManager::Instance()->GetTeam((short)side));
    }

    if (target == NIS_TARGET_UNIDENTIFIED_21)
    {
        return GetTeamName((eTeamID)GameInfoManager::Instance()->GetTeam((short)mUnidentified34238));
    }

    if (target == NIS_TARGET_HOME_GOALIE || target == NIS_TARGET_AWAY_GOALIE || target == NIS_TARGET_WINNER_GOALIE || target == NIS_TARGET_LOSER_GOALIE || target == NIS_TARGET_UNIDENTIFIED_22)
    {
        return "goalie";
    }

    if (target == NIS_TARGET_AWAY_SIDEKICK)
    {
        return GetSidekickName((eSidekickID)GameInfoManager::Instance()->GetSidekick(1, 0));
    }

    return kNisEmpty;
}

void NisPlayer::fn_8027F4B0(NisTarget target, NisWinnerType winnerType)
{
    const char* filter = NisPlayer::Instance()->GetTargetFilter(target, winnerType);
    char textureName[64];
    nlSNPrintf(textureName, sizeof(textureName), "%s/%s_logo", filter, filter);
    mUnidentified34438 = glGetTexture(textureName);
    mUnidentified3443C = glGetTextureManager()->GetTextureIndex(mUnidentified34438);
}

void NisPlayer::Load(const char* nisType, NisTarget target, NisUseStadiumOffset useStadiumOffset, NisUseFilter useFilter, NisWinnerType winnerType, int param5, int param6)
{
    mActive = true;

    const char* filter = GetTargetFilter(target, winnerType);
    char prefix[64];
    if (nlStrCmp(filter, kNisEmpty) != 0)
    {
        nlSNPrintf(prefix, sizeof(prefix), "%s_", filter);
    }
    else
    {
        prefix[0] = '\0';
    }

    char extra[64];
    if (useFilter != NIS_NO_FILTER)
    {
        nlSNPrintf(extra, sizeof(extra), "_%s", mExtraNameFilter);
    }
    else
    {
        extra[0] = '\0';
    }

    char fullName[64];
    nlSNPrintf(fullName, sizeof(fullName), "%s%s%s", prefix, nisType, extra);

    int numAvailableNis = 0;
    NisHeader* availableNis[10] = { 0 };
    int dictionaryIndex;
    for (dictionaryIndex = 0; dictionaryIndex < mDictSize && numAvailableNis < 10; dictionaryIndex++)
    {
        if (nlStrNICmp(mDict[dictionaryIndex].name, fullName, nlStrLen(fullName)) != 0)
        {
            continue;
        }
        if (strstr(mDict[dictionaryIndex].name, "_same") != NULL)
        {
            continue;
        }
        if (strstr(mDict[dictionaryIndex].name, "_other") != NULL)
        {
            continue;
        }
        availableNis[numAvailableNis++] = &mDict[dictionaryIndex];
    }

    if (numAvailableNis == 0)
    {
        return;
    }

    int index;
    if (param6 >= 0)
    {
        index = param6 % numAvailableNis;
    }
    else
    {
        float randomValue = (numAvailableNis - 1) * nlRandomf(1.0f, &GetPresentation()->mRandomSeed);
        index = (int)(randomValue + (randomValue < 0.0f ? -0.5f : 0.5f));
        if (fn_80287B34(GetPresentation()) && param5 == 0)
        {
            if (numAvailableNis > 1 && mUnidentified343F4 == index && nlStrCmp(mUnidentified343F8, mExtraNameFilter) == 0)
            {
                while (index == mUnidentified343F4)
                {
                    randomValue = (numAvailableNis - 1) * nlRandomf(1.0f, &GetPresentation()->mRandomSeed);
                    index = (int)(randomValue + (randomValue < 0.0f ? -0.5f : 0.5f));
                }
            }
            mUnidentified343F4 = index;
            nlStrNCpy(mUnidentified343F8, mExtraNameFilter, sizeof(mUnidentified343F8));
        }
    }

    NisHeader& nisHeader = *availableNis[index];
    fn_802805B4(nisHeader, target, useStadiumOffset, winnerType, param5, false);

    NisTarget sameTarget = NIS_TARGET_NONE;
    NisTarget otherTarget = NIS_TARGET_NONE;
    switch (target)
    {
    case NIS_TARGET_HOME_CAPTAIN:
    case NIS_TARGET_HOME_GOALIE:
        sameTarget = NIS_TARGET_HOME_SIDEKICK;
        otherTarget = NIS_TARGET_AWAY_SIDEKICK;
        break;
    case NIS_TARGET_AWAY_CAPTAIN:
    case NIS_TARGET_AWAY_GOALIE:
        sameTarget = NIS_TARGET_AWAY_SIDEKICK;
        otherTarget = NIS_TARGET_HOME_SIDEKICK;
        break;
    case NIS_TARGET_LOSER_CAPTAIN:
    case NIS_TARGET_LOSER_GOALIE:
        sameTarget = NIS_TARGET_LOSER_SIDEKICK;
        otherTarget = NIS_TARGET_WINNER_SIDEKICK;
        break;
    case NIS_TARGET_WINNER_CAPTAIN:
    case NIS_TARGET_WINNER_GOALIE:
        sameTarget = NIS_TARGET_WINNER_SIDEKICK;
        otherTarget = NIS_TARGET_LOSER_SIDEKICK;
        break;
    case NIS_TARGET_UNIDENTIFIED_14:
    {
        cCharacter* character = g_pCharacters[mGoalScorerCharIndex];
        if (character != NULL && character->fn_8001E168())
        {
            if (((cPlayer*)character)->m_pTeam->m_nSide == 0)
            {
                sameTarget = NIS_TARGET_HOME_SIDEKICK;
                otherTarget = NIS_TARGET_AWAY_SIDEKICK;
            }
            else
            {
                sameTarget = NIS_TARGET_AWAY_SIDEKICK;
                otherTarget = NIS_TARGET_HOME_SIDEKICK;
            }
        }
        break;
    }
    case NIS_TARGET_UNIDENTIFIED_22:
    {
        cCharacter* character = g_pCharacters[mGoalScorerCharIndex];
        if (character != NULL && character->fn_8001E168())
        {
            if (((cPlayer*)character)->m_pTeam->m_nSide == 0)
            {
                sameTarget = NIS_TARGET_AWAY_SIDEKICK;
                otherTarget = NIS_TARGET_HOME_SIDEKICK;
            }
            else
            {
                sameTarget = NIS_TARGET_HOME_SIDEKICK;
                otherTarget = NIS_TARGET_AWAY_SIDEKICK;
            }
        }
        break;
    }
    case NIS_TARGET_UNIDENTIFIED_21:
    {
        cCharacter* character = g_pCharacters[mGoalScorerCharIndex];
        if (character != NULL && character->fn_8001E168())
        {
            if (((cPlayer*)character)->m_pTeam->m_nSide == 0)
            {
                sameTarget = NIS_TARGET_HOME_SIDEKICK;
                otherTarget = NIS_TARGET_AWAY_SIDEKICK;
            }
            else
            {
                sameTarget = NIS_TARGET_AWAY_SIDEKICK;
                otherTarget = NIS_TARGET_HOME_SIDEKICK;
            }
        }
        break;
    }
    }

    if (sameTarget != NIS_TARGET_NONE)
    {
        fn_8028041C(nisHeader.name, "same", sameTarget, useStadiumOffset, winnerType, nisHeader.mUnidentified195, param5);
    }
    if (otherTarget != NIS_TARGET_NONE)
    {
        fn_8028041C(nisHeader.name, "other", otherTarget, useStadiumOffset, winnerType, nisHeader.mUnidentified195, param5);
    }

    if (param5 != 1 && mUnidentified34354 == 0)
    {
        char cueName[128];
        nlStrNCpy(cueName, nisHeader.name, sizeof(cueName));
        unsigned long length = nlStrLen(cueName);
        if (GetStadiumUnknown0x10(GameInfoManager::Instance()->GetStadium()))
        {
            cueName[length - 4] = '\0';
        }
        else
        {
            nlStrNCpy(cueName + length - 4, "_nocrowd", sizeof(cueName) - length - 4);
        }
        fn_8027EDCC(nlStringLowerHash(cueName));
    }
}

void NisPlayer::fn_8028041C(const char* param1, const char* param2, NisTarget target, NisUseStadiumOffset useStadiumOffset, NisWinnerType winnerType, bool param5, int param6)
{
    char baseName[64];
    int length = nlStrChr(param1, '.') - param1 + 1;
    nlStrNCpy(baseName, param1, nlMin((int)sizeof(baseName), length));

    const char* filter = GetTargetFilter(target, winnerType);
    char fullName[64];
    nlSNPrintf(fullName, sizeof(fullName), "%s_%s_%s.nis", baseName, filter, param2);

    NisHeader* nisHeader = NULL;
    for (int dictionaryIndex = 0; dictionaryIndex < mDictSize; dictionaryIndex++)
    {
        if (nlStrCmp(mDict[dictionaryIndex].name, fullName) == 0)
        {
            mDict[dictionaryIndex].mUnidentified195 = param5;
            nisHeader = &mDict[dictionaryIndex];
            break;
        }
    }

    if (nisHeader != NULL)
    {
        fn_802805B4(*nisHeader, target, useStadiumOffset, winnerType, param6, true);
    }
}

void NisPlayer::fn_8027E714()
{
    for (int i = 0; i < 8; i++)
    {
        if (mPlaying[i] != NULL)
        {
            if (mPlaying[i]->unknown_0x034 == 0)
            {
                mPlaying[i]->unknown_0x034 = 1;
            }
            else if (mPlaying[i]->unknown_0x034 == 1)
            {
                mPlaying[i]->unknown_0x034 = 0;
            }
        }
    }

    cCameraManager::Remove(mCamera[0]);
    cAnimCamera camera = mCamera[0];
    mCamera[0] = mCamera[1];
    mCamera[1] = camera;
    cCameraManager::PushCamera(&mCamera[0]);
}

void fn_8027F174(glModel* model)
{
    static const unsigned long texture_806E19CC = glGetTexture("transitions/waluigi_logo");
    for (glModelPacket* packet = model->packets; packet < model->packets + model->numPackets; packet++)
    {
        unsigned long diffuseTexture = glGetMaterialUnsignedParameter(packet, gDiffuseTextureSemantic);
        if (NisPlayer::Instance()->HasUnidentifiedPacket(packet))
        {
            glSetMaterialTextureParameter(packet, gDiffuseTextureSemantic, NisPlayer::Instance()->mUnidentified34438);
            unsigned long textureIndex = NisPlayer::Instance()->mUnidentified3443C;
            glSetMaterialTextureIndexParameter(packet, gDiffuseTextureSemantic, &textureIndex);
        }
        if (diffuseTexture == texture_806E19CC)
        {
            glSetMaterialTextureParameter(packet, gDiffuseTextureSemantic, NisPlayer::Instance()->mUnidentified34438);
            unsigned long textureIndex = NisPlayer::Instance()->mUnidentified3443C;
            glSetMaterialTextureIndexParameter(packet, gDiffuseTextureSemantic, &textureIndex);
            NisPlayer::Instance()->AddUnidentifiedPacket(packet);
        }
    }
}

void NisPlayer::HideAllActors() const
{
    RenderSnapshot& snapshot = ReplayManager::Instance()->GetMutableRenderSnapshot();
    for (int i = 0; i < 150; i++)
    {
        snapshot.GetUnidentifiedPowerup(i).SetUnidentifiedVisible(false);
    }
    for (int i = 0; i < 10; i++)
    {
        snapshot.mCharacters[i].visible = false;
    }
    for (int i = 0; i < 15; i++)
    {
        snapshot._1DA4[i].mVisible = false;
    }
    for (int i = 0; i < 3; i++)
    {
        snapshot._1FC0[i].visible = false;
    }
    for (int i = 0; i < 8; i++)
    {
        snapshot.mDaisyFists[i].mVisible = false;
    }
    for (int i = 0; i < 6; i++)
    {
        snapshot._1CC8[i].mVisible = false;
    }
    for (int i = 0; i < 8; i++)
    {
        snapshot._2194[i].mVisible = false;
    }
    snapshot._1BA0.mVisible = false;
    snapshot.mBall.mFlags.bits.visible = false;
    snapshot.mChainChomp.visible = false;
}

void NisPlayer::Render(int pass) const
{
    nlTaskManager* taskManager = nlTaskManager::m_pInstance;
    unsigned long currentState = taskManager->mCurrentState;

    if (currentState != 0x10 || ((taskManager->mPreviousState == 0x10) && (currentState != 1)))
    {
        return;
    }

    HideAllActors();

    int line;
    bool renderPass = false;
    if (fn_8027E64C() && pass == 0)
    {
        renderPass = true;
    }
    for (int i = 0; i < 8; i++)
    {
        if (mPlaying[i] != NULL)
        {
            if (mPlaying[i]->unknown_0x034 == renderPass || mPlaying[i]->unknown_0x034 == 2)
            {
                mPlaying[i]->Render(renderPass);
            }
        }
    }
    if (mUnidentified34359 && renderPass == 0)
    {
        line = 0;
        for (int i = 0; i < 8; i++)
        {
            if (mPlaying[i] != NULL)
            {
                nlScreenPrintf(0, line++, false, 4, "Mirrored: %s", mPlaying[i]->mMirrored ? "True" : "False");
                if (mPlaying[i]->mCamera != NULL && mPlaying[i]->mCamera->m_pActiveCameraData != NULL)
                {
                    nlScreenPrintf(0, line++, false, 4, "Camera: %s", mPlaying[i]->mCamera->m_pActiveCameraData->field_0x0C);
                }
                nlScreenPrintf(0, line++, false, 4, "Name: %s", mPlaying[i]->Name());
            }
        }
    }
    if (pass == 0 && fn_8027E64C())
    {
        mUnidentified3433C[mUnidentified34338]->Render();
    }
}

void NisPlayer::LoadTriggers(Nis& nis)
{
    BasicString<char, Detail::TempStringAllocator> name(nis.Name());
    for (int i = name.size() - 1; i >= 0; --i)
    {
        if (name[i] == '.')
        {
            name[i] = '\0';
            break;
        }
    }
    unsigned long nisHash = nlStringHash(name.c_str());
    if (!FunctionExists(nisHash))
    {
        for (int i = 0; i < name.size(); ++i)
        {
            if (name[i] == '_')
            {
                name.erase(name.begin(), name.begin() + i);
                char unknown_18[] = "all";
                BasicString<char, Detail::TempStringAllocator> all("all");
                name.insert(name.begin(), unknown_18, unknown_18 + sizeof(unknown_18) - 1);
                break;
            }
        }
        nisHash = nlStringHash(name.c_str());
        if (!FunctionExists(nisHash))
        {
            return;
        }
    }
    mNisForTriggerLoading = &nis;
    CallFunction(nisHash);
    mNisForTriggerLoading = NULL;
}
