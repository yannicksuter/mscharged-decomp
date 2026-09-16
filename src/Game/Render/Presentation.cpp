#include "Game/Render/Presentation.h"

#include "Game/Ball.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/Audio/RegistryPools.h"
#include "Game/BaseGameSceneManager.h"
#include "Game/OverlayManager.h"
#include "Game/OverlayHandlerGoal.h"
#include "Game/BasicStadium.h"
#include "Game/Character.h"
#include "Game/CharacterTemplate.h"
#include "Game/Debug/ShapeRender.h"
#include "Game/DB/GameProgress.h"
#include "Game/Effects/EmissionController.h"
#include "Game/Effects/EmissionManager.h"
#include "Game/EventDataTypes.h"
#include "Game/EventRegistry.h"
#include "Game/FE/feManager.h"
#include "Game/Game.h"
#include "Game/GameInfo.h"
#include "Game/InputManager.h"
#include "Game/InputRouter.h"
#include "Game/NetworkMessageRegistry.h"
#include "Game/NetworkMessages.h"
#include "Game/NetworkInputRecording.h"
#include "Game/NetworkStatsManager.h"
#include "Game/NetTournManager.h"
#include "Game/NetworkSession.h"
#include "Game/NisPlayer.h"
#include "Game/ReplayChoreo.h"
#include "Game/ReplayManager.h"
#include "Game/Render/RLView.h"
#include "Game/Render/RLViewLayers.h"
#include "Game/Render/CrowdImpostors.h"
#include "Game/Render/Jumbotron.h"
#include "Game/Render/StadiumLoading.h"
#include "Game/Render/tu_80283D9C.h"
#include "Game/Render/Wiper.h"
#include "Game/Sys/audio.h"
#include "Game/Sys/debug.h"
#include "Game/Task/BeginFrameTask.h"
#include "Game/Task/FixedUpdateTask.h"
#include "Game/Task/GameRenderTask.h"
#include "Game/Transitions/ScreenTransitionManager.h"
#include "Game/AI/Fielder.h"
#include "Game/Camera/tu_800F9460.h"
#include "Game/Team.h"
#include "Game/Render/NumberDisplay.h"
#include "Game/TweakQuery.h"
#include "Game/main.h"
#include "NL/gl/gl.h"
#include "NL/nlBindMember.h"
#include "NL/globalpad.h"
#include "NL/nlConfig.h"
#include "NL/nlFile.h"
#include "NL/nlFunction.inl"
#include "NL/nlMath.h"
#include "NL/nlString.h"
#include "NL/nlTask.h"
#include "Game/Render/MegastrikeBackgroundOverlay.h"

extern "C"
{
    void fn_801B9DAC(const char* name);
    void fn_801BA358();
    void fn_801E2564(void* manager);
    void fn_80195868(ReplayChoreo* choreo, float deltaTime);
    void fn_801955E8(ReplayChoreo* choreo, bool alternate);
    void fn_8019571C(ReplayChoreo* choreo);
    void fn_801959F0(ReplayChoreo* choreo, int quality);
    void fn_801E2230(void* manager, int value);
    float fn_800155A0(void* ball, int index);
    extern bool lbl_806DCD60;
    extern bool lbl_806E1961;
    extern int lbl_806E2130;
}

static inline bool IsNumberDisplayCounting()
{
    return gpNumberDisplay->mShowAccumulatedScore
        && gpNumberDisplay->mScoreUpdateTimer > 0.0f;
}

static const char* idleFun = "Idle";
static bool sLoopPresentation;
static bool sUseCupTrophy;
static int sCupTrophy;
static bool sUseCupPersonaTrophy;
static int sCupPersonaTrophy;
static nlColour sLetterBoxColour = { { 0x00, 0x00, 0x00, 0xFF } };

static inline bool IsDuringGamePauseState()
{
    bool bGameFrameUnlocked;
    bool bDuringGamePauseState;
    bDuringGamePauseState = false;
    bGameFrameUnlocked = false;
    if (!FrontEnd::m_bGameOver)
    {
        bool bFrameLocked
            = GetFixedUpdateTask()->mfFrameLockTime > 0.0f;
        if (!bFrameLocked)
        {
            bGameFrameUnlocked = true;
        }
    }
    if (bGameFrameUnlocked
        && nlTaskManager::m_pInstance->mCurrentState == 1)
    {
        bDuringGamePauseState = true;
    }
    return bDuringGamePauseState;
}

static inline bool IsCupWinner()
{
    if (sUseCupTrophy)
    {
        return true;
    }

    short winnerSide
        = (short)NisPlayer::Instance()->mWinnerSide[NIS_GAME_WINNER];
    GameInfoManager* gameInfo = GameInfoManager::Instance();
    int winnerTeam
        = gameInfo->GetCurrentGameInfo()->GetTeam(winnerSide);
    return gameInfo->IsInMode3()
        && g_pCupManager->IsCupWinningGame(winnerTeam);
}

static inline bool IsCupPersonaWinner()
{
    if (sUseCupPersonaTrophy)
    {
        return true;
    }

    short winnerSide
        = (short)NisPlayer::Instance()->mWinnerSide[NIS_GAME_WINNER];
    GameInfoManager* gameInfo = GameInfoManager::Instance();
    int winnerTeam
        = gameInfo->GetCurrentGameInfo()->GetTeam(winnerSide);
    return gameInfo->mIsOnlineMode && gameInfo->IsInMode1()
        && NetTournManager::Instance()->IsCupWinningGame(winnerTeam);
}

/**
 * Address/Size: 0x80284A58 | size: 0x64
 */
Presentation* GetPresentation()
{
    static Presentation instance;
    return &instance;
}

/**
 * Address/Size: 0x80284ABC | size: 0x58
 */
Presentation::~Presentation()
{
}

/**
 * Address/Size: 0x80284B14 | size: 0x1AC
 */
Presentation::Presentation()
    : InterpreterCore(100)
{
    mByPassWasSkipped = false;
    mSkipPressed = false;
    mInsideByPass = false;
    mByPassing = false;
    mWaitingForCharacterDirectionSince = 0.0f;
    mTimeInFunction = 0.0f;
    mDisplayLetterBox = 0.0f;
    mLetterBoxDuration = 0.0f;
    mLetterBoxEnabled = false;
    mOverlayDelay = 0.0f;
    mOverlayDisplayLength = 0.0f;
    mOverlayDisplayed = false;
    mOverlayToDisplay = -2;
    mNisLoadedBits = 0;
    mUnidentified141 = false;
    mUnidentified142 = false;
    mUnidentified143 = true;
    mRandomSeed = nlDefaultSeed;
    mHighlightsLeft = 0;
    mByPassNumber = 0;
    mSkipPastByPass = -1;
    mUnidentified156 = false;
    mUnidentified157 = false;
    mUnidentified159 = false;
    mUnidentified160 = false;
    mUnidentified161 = true;
    mUnidentified162 = false;
    mUnidentified163 = false;
    mUnidentified164 = true;
    mQueuedFunction[0] = '\0';
    mQueuedFilter[0] = '\0';

    unsigned long fileSize = 0;
    void* byteCode = nlLoadEntireFile("art/Scripts/presentation.byte_code",
        &fileSize, 0x20, AllocateStart, 0, 0, 0);
    LoadByteCode(byteCode);

    nlStrNCpy(mCurrentFunction, idleFun, 64);
    mInterruptWipe[0] = '\0';
    mIsAllowedToSkip[0] = true;
    mIsAllowedToSkip[1] = true;
    mIsAllowedToSkip[2] = true;
    mIsAllowedToSkip[3] = true;

    gNetworkMessageRegistry->RegisterReceiver(30, this);
    gNetworkMessageRegistry->RegisterReceiver(31, this);
    mUnidentified02C = 0;
}

/**
 * Address/Size: 0x80284CC0 | size: 0xE8
 */
void Presentation::UpdateAllowedToSkip()
{
    mUnidentified142 = true;

    NetworkSessionControl& session = *g_pNetworkSessionBase;
    if (session.GetSessionMode() == 0)
    {
        mIsAllowedToSkip[0] = true;
        mIsAllowedToSkip[1] = true;
        mIsAllowedToSkip[2] = true;
        mIsAllowedToSkip[3] = true;
        return;
    }

    mIsAllowedToSkip[0] = false;
    mIsAllowedToSkip[1] = false;
    mIsAllowedToSkip[2] = false;
    mIsAllowedToSkip[3] = false;

    GameInfoManager* info;
    int pad;
    NetworkPeerChannel* channel;
    NetworkPeer* peer;
    int i;
    peer = g_pNetworkSessionBase->GetLocalPeer();
    for (i = 0; i < (int)peer->mPlayerCount; i++)
    {
        channel = peer->GetNetworkPeerChannel(i);
        pad = channel->mGlobalPadIndex;
        info = nlSingleton<GameInfoManager>::Instance();
        int side = NisPlayer::Instance()->mWinnerSide[NIS_GAME_WINNER];
        if (side
            == info->GetPlayingSide(channel->GetNetworkPeerChannelId()))
        {
            mIsAllowedToSkip[pad] = true;
        }
    }
}

/**
 * Address/Size: 0x80284DA8 | size: 0x3E4
 */
bool Presentation::DetectSkipPress()
{
    if (nlSingleton<GameInfoManager>::Instance()->IsInMode2())
    {
        return false;
    }

    if (Config::Global().Get<bool>("no_presentation_skip", false))
    {
        if (nlStrCmp<char>(mCurrentFunction, "PlayHighlight") != 0)
        {
            return false;
        }
    }

    if (DuringEndOfGamePresentation(this) & (mTimeInFunction <= 1.2f))
    {
        return false;
    }

    if (nlTaskManager::m_pInstance->mCurrentState != 0x10
        && nlTaskManager::m_pInstance->mCurrentState != 0x8)
    {
        return false;
    }

    bool networkGame = g_pNetworkSessionBase->GetSessionMode();
    if (networkGame)
    {
        if (nlStrCmp<char>(mCurrentFunction, "GameBegin") == 0)
        {
            return false;
        }
    }

    for (int i = 0; i < 4; i++)
    {
        if (!mIsAllowedToSkip[i])
        {
            continue;
        }
        if (g_pPadManager->GetPad(i) == 0)
        {
            continue;
        }
        if (g_pPadManager->GetPad(i)->PlatJustReleased(7, true))
        {
            return true;
        }
    }

    return false;
}

/**
 * Address/Size: 0x8028518C | size: 0x588
 */
void Presentation::Finish()
{
    bool playHighlight = false;
    bool fadeToStrikerTimes = false;

    if (strcmp("PlayHighlight", mCurrentFunction) == 0 || sLoopPresentation)
    {
        fadeToStrikerTimes = true;
        if (g_pNetworkSessionBase->GetSessionMode() != 0)
        {
            if (mHighlightsLeft > 0)
            {
                if (!mByPassWasSkipped)
                {
                    playHighlight = true;
                }
                mHighlightsLeft--;
            }
        }
        else if (GetTweakBool("/user/dosoak", false) == true)
        {
            if (mHighlightsLeft > 0)
            {
                if (!mByPassWasSkipped)
                {
                    playHighlight = true;
                }
                mHighlightsLeft--;
            }
        }
        else if (!mByPassWasSkipped)
        {
            playHighlight = true;
        }
    }

    if (playHighlight)
    {
        Call("PlayHighlight", "");
    }
    else if (fadeToStrikerTimes)
    {
        Call("FadeToStrikerTimes", "");
    }
    else
    {
        if (mCurrentFunction == strstr(mCurrentFunction, "Goal")
            || mCurrentFunction == strstr(mCurrentFunction, "MegastrikeEnd"))
        {
            if (g_pGame->m_eGameState != 3)
            {
                if (!mUnidentified159)
                {
                    g_pGame->ChangeGameState(1);
                }
                else
                {
                    g_pGame->fn_8005DF38();
                }
            }
        }

        if (mQueuedFunction[0] == '\0')
        {
            if (DuringEndOfGamePresentation(this))
            {
                NisPlayer::Instance()->fn_8027ED18();
                g_pGame->mUnidentified49C.mEvent02.Queue(
                    Function<FnVoidVoid>());
                nlTaskManager::SetNextState(1);
            }
            else
            {
                if (nlStrCmp<char>(mCurrentFunction, "GameBegin") == 0)
                {
                    g_pGame->ChangeGameState(1);
                    FixedUpdateTask* task = GetFixedUpdateTask();
                    task->mUnidentified38 = true;
                }
                nlTaskManager::SetNextState(2);
            }
        }
    }

    Call(idleFun, "");

    if (mQueuedFunction[0] != '\0')
    {
        char queuedFunction[64];
        char queuedFilter[32];
        nlStrNCpy(queuedFunction, mQueuedFunction, 64);
        nlStrNCpy(queuedFilter, mQueuedFilter, 32);
        mQueuedFunction[0] = '\0';
        mQueuedFilter[0] = '\0';
        Call(queuedFunction, queuedFilter);
    }

    if (strcmp(mCurrentFunction, idleFun) == 0)
    {
        ReplayChoreo::Instance().Reset();
    }
}

/**
 * Address/Size: 0x80285714 | size: 0x44
 */
void fn_80285714(Presentation* state, u32 from, u32 to)
{
    if (to == 1)
    {
        g_pOverlayManager->GetScene((SceneList)0x66)->SetVisible(false);
    }
}

/**
 * Address/Size: 0x80285758 | size: 0x6C4
 */
void Presentation::Update(float deltaTime)
{
    if (nlSingleton<UnidentifiedCameraEffects>::s_pInstance != 0)
    {
        nlSingleton<UnidentifiedCameraEffects>::s_pInstance->Update(deltaTime);
    }

    if (gpNumberDisplay != 0)
    {
        gpNumberDisplay->Update(deltaTime);
    }

    NisPlayer::Instance()->fn_8027CA44();

    mUnidentified15C -= deltaTime;
    if (mUnidentified15C < 0.0f)
    {
        mUnidentified15C = 0.0f;
    }

    mTimeInFunction += deltaTime;

    if (mDisplayLetterBox > 0.0f)
    {
        mDisplayLetterBox -= deltaTime;
        if (mDisplayLetterBox <= 0.0f)
        {
            int replayTime = -30;
            fn_801959F0(&ReplayChoreo::Instance(),
                ReplayManager::Instance()->fn_8018A16C(replayTime));
            mDisplayLetterBox = 0.0f;
        }
    }

    if (!IsDuringGamePauseState())
    {
        if (nlStrCmp<char>(mCurrentFunction, "GameBegin") == 0)
        {
            if (nlTaskManager::m_pInstance->mCurrentState != 0x10)
            {
                glDiscardFrame(1);
            }
        }

        if (m_RunState == 3)
        {
            Run();
        }

        fn_80195868(&ReplayChoreo::Instance(), deltaTime);
        ReplayCamera::UpdateTweakMode();

        bool skipPastByPass = false;
        if (!mSkipPressed)
        {
            mSkipPressed = DetectSkipPress();
            if (mSkipPastByPass >= mByPassNumber)
            {
                tDebugPrintManager::Print(DC_NETWORK,
                    "SkipPastBypass # %d >= Bypass# %d, so setting mSkipPressed = true\n",
                    mSkipPastByPass, mByPassNumber);
                mSkipPressed = true;
                skipPastByPass = true;
            }
        }

        if (mSkipPressed && mInsideByPass && !IsNumberDisplayCounting())
        {
            mByPassing = true;
            mSkipPressed = false;

            if (!skipPastByPass)
            {
                SendSkipNis();
            }

            tDebugPrintManager::Print(DC_NETWORK, "Bypassing...\n");
            g_pGame->mUnidentified49C.mEvent04.Queue(
                Function<FnVoidVoid>());
        }
    }

    if (m_RunState == 2)
    {
        Finish();
    }

    Wiper::Instance().Render();
    UpdateAndRenderLetterBox();

    if (mUnidentified163 == true)
    {
        RLView* view = GetLayerView(eCLV_FrontEnd);
        RLView* previous = (RLView*)g_ShapeRenderer.m_eView;
        g_ShapeRenderer.m_eView = (GLView*)view;
        nlColour colour = sLetterBoxColour;
        g_ShapeRenderer.DrawRectangle2D(0.0f, 0.0f, glGetOrthographicWidth(),
            glGetOrthographicHeight(), -2.0f, colour, 0);
        g_ShapeRenderer.m_eView = (GLView*)previous;
    }

    if (IsDuringGamePauseState())
    {
        return;
    }

    if (mOverlayToDisplay == -2)
    {
        return;
    }

    if (!mOverlayDisplayed)
    {
        mOverlayDelay -= deltaTime;
        if (mOverlayDelay <= 0.0)
        {
            static_cast<OverlayManager*>(g_pOverlayManager)->SetVisible((SceneList)mOverlayToDisplay, true, true);
            if (mOverlayToDisplay == 0x5F)
            {
                fn_801E2564(g_pOverlayManager);
            }
            mOverlayDisplayed = true;
            mOverlayDelay = 0.0f;
        }
    }
    else if (mOverlayDisplayLength != -15.0f)
    {
        mOverlayDisplayLength -= deltaTime;
        if (mOverlayDisplayLength <= 0.0)
        {
            if (mOverlayDisplayed)
            {
                static_cast<OverlayManager*>(g_pOverlayManager)->SetVisible((SceneList)mOverlayToDisplay, false, false);
            }
            mOverlayDisplayed = false;
            mOverlayToDisplay = -2;
            mOverlayDisplayLength = 0.0f;
            mOverlayDelay = 0.0f;
        }
    }
}

/**
 * Address/Size: 0x80285E1C | size: 0x4
 */
void Presentation::fn_80285E1C()
{
}

/**
 * Address/Size: 0x80285E20 | size: 0xCC
 */
bool IsIdleAndNoShotInProgress(Presentation* presentation)
{
    if (nlStrCmp<char>(idleFun, presentation->mCurrentFunction) != 0)
    {
        return false;
    }

    if (g_pTeams[0]->GetCaptain()->m_eActionState == ACTION_SHOT
        || g_pTeams[0]->GetCaptain()->m_eActionState == ACTION_SHOOT_TO_SCORE
        || g_pTeams[1]->GetCaptain()->m_eActionState == ACTION_SHOT
        || g_pTeams[1]->GetCaptain()->m_eActionState == ACTION_SHOOT_TO_SCORE)
    {
        return false;
    }

    return true;
}

/**
 * Address/Size: 0x80285EEC | size: 0x178
 */
bool DuringEndOfGamePresentation(Presentation* presentation)
{
    return nlStrCmp<char>("GameEndNoSuddenDeath", presentation->mCurrentFunction) == 0
        || nlStrCmp<char>("GameEndSuddenDeath", presentation->mCurrentFunction) == 0
        || nlStrCmp<char>("GameEndMegaStrike", presentation->mCurrentFunction) == 0
        || nlStrCmp<char>("PlayHighlight", presentation->mCurrentFunction) == 0
        || nlStrCmp<char>("FadeToStrikerTimes", presentation->mCurrentFunction) == 0;
}

/**
 * Address/Size: 0x80286064 | size: 0x224
 */
void Presentation::Call(
    const char* functionName, const char* nisFilter)
{
    if (nlSingleton<UnidentifiedCameraEffects>::s_pInstance != 0)
    {
        nlSingleton<UnidentifiedCameraEffects>::s_pInstance->Reset();
    }

    if (nlStrCmp<char>(idleFun, mCurrentFunction) != 0
        && nlStrCmp<char>(idleFun, functionName) != 0)
    {
        nlStrNCpy(mQueuedFunction, functionName, 64);
        nlStrNCpy(mQueuedFilter, nisFilter, 32);
        return;
    }

    nlStrNCpy(mCurrentFunction, functionName, 64);
    mInterruptWipe[0] = '\0';
    tDebugPrintManager::Print(DC_NETWORK, "Call(%s)\n", mCurrentFunction);

    mSkipPressed = false;
    mInsideByPass = false;
    mByPassing = false;
    mTimeInFunction = 0.0f;

    NisPlayer::Instance()->SetExtraNameFilter(nisFilter);
    InterpreterCore::Reset();
    CallFunction(nlStringHash(functionName));
}

/**
 * Address/Size: 0x80286288 | size: 0x10
 */
void Presentation::PlayHighlights()
{
    Call("PlayHighlight", "");
}

/**
 * Address/Size: 0x80286298 | size: 0x908
 */
void Presentation::RegisterEventListeners()
{
    {
        Function<GoalScoredData*> callback(
            BindMember(this, &Presentation::OnGoalScored));
        UnidentifiedFindEvent<GoalScoredData>("GoalScored", -1)
            ->Add(callback, 0, -1);
    }
    {
        Function<GoalieSaveData*> callback(
            BindMember(this, &Presentation::OnGoalieSave));
        UnidentifiedFindEvent<GoalieSaveData>("GoalieSave", -1)
            ->Add(callback, 0, -1);
    }
    {
        Function<FnVoidVoid> callback(
            BindMember(this, &Presentation::OnKickoff));
        UnidentifiedFindEvent<UnidentifiedEventNoData>("Kickoff", -1)
            ->Add(callback, 0, -1);
    }
    {
        Function<FnVoidVoid> callback(
            BindMember(this, &Presentation::OnSuddenDeath));
        UnidentifiedFindEvent<UnidentifiedEventNoData>("SuddenDeath", -1)
            ->Add(callback, 0, -1);
    }
    {
        Function<FnVoidVoid> callback(
            BindMember(this, &Presentation::OnCharacterDirectionEnd));
        UnidentifiedFindEvent<UnidentifiedEventNoData>(
            "CharacterDirectionEnd", -1)->Add(callback, 0, -1);
    }
    {
        Function<cPlayer*> callback(
            BindMember(this, &Presentation::OnMegaStrikeIntro));
        UnidentifiedFindEvent<cPlayer>("MegaStrikeIntro", -1)
            ->Add(callback, 0, -1);
    }
    {
        Function<MegaStrikeEndData*> callback(
            BindMember(this, &Presentation::OnMegaStrikeEnd));
        UnidentifiedFindEvent<MegaStrikeEndData>("MegastrikeEnd", -1)
            ->Add(callback, 0, -1);
    }
}

/**
 * Address/Size: 0x80286BA0 | size: 0x4
 */
void Presentation::OnGoalieSave(GoalieSaveData* data)
{
}

/**
 * Address/Size: 0x80286BA4 | size: 0x500
 */
void Presentation::OnGoalScored(GoalScoredData* data)
{
    if (g_pGame == 0)
    {
        return;
    }

    if (data->uGoalType != 6)
    {
        gpNumberDisplay->BeginScoreUpdate();
    }

    mUnidentified159 = false;
    mDisplayLetterBox = 0.0f;

    if (Config::Global().Get<bool>("no_presentation", false))
    {
        g_pGame->ChangeGameState(1);
        return;
    }

    if (IsNetworkOrRecordedGame())
    {
        mRandomSeed = gNetworkRandomSeed;
    }

    NisPlayer::Instance()->mWinnerSide[NIS_GOAL_WINNER] = data->uTeamIndex;

    cTeam* scoringTeam = g_pTeams[data->uTeamIndex];
    int scoreDifference
        = scoringTeam->m_nScore - scoringTeam->GetOtherTeam()->m_nScore;
    bool scoreTied = scoreDifference == 0;
    bool closeGoal = scoreDifference == 1
        || scoreDifference == data->uNumGoalsScored;
    bool suddenDeath = g_pGame->m_eGameState == 6;

    if (data->uGoalType == 5)
    {
        data->pLastTouch[data->uTeamIndex]->fn_8001E168();
    }
    else
    {
        data->pScorer->fn_8001E168();
    }

    const char* filter = "high";
    const char* functionName = "GoalCelebration";
    PlayGoalEffects("Goal_score");

    if (!mUnidentified160 || suddenDeath || closeGoal || scoreTied)
    {
        mUnidentified160 = true;
    }
    else
    {
        mUnidentified160 = false;
        filter = "low";
    }

    mUnidentified156 = false;
    mUnidentified157 = false;

    if (!suddenDeath && !mUnidentified161
        && !nlSingleton<GameInfoManager>::Instance()->IsInMode4()
        && nlRandomf(1.0f, &mRandomSeed) < 0.22f)
    {
        mUnidentified161 = true;
        mUnidentified156 = true;
        if (mUnidentified162 == true)
        {
            mUnidentified157 = true;
            mUnidentified162 = false;
        }
        else
        {
            mUnidentified162 = true;
        }
    }
    else
    {
        mUnidentified161 = false;
    }

    if (nlSingleton<GameInfoManager>::Instance()->IsInMode4()
        && g_pStrikerChallenge->mCondition == 2
        && g_pTeams[1]->m_nScore > 0)
    {
        NisPlayer* nisPlayer = NisPlayer::Instance();
        nisPlayer->mWinnerSide[NIS_GAME_WINNER] = 1;
        mUnidentified156 = false;
        functionName = "GameEndSuddenDeath";
    }
    else if (!scoreTied && g_pGame != 0
        && (unsigned int)(10.0f
                * (g_pGame->GetGameDuration() - g_pGame->GetGameTime()))
            == 0)
    {
        int awayScore = g_pTeams[1]->m_nScore;
        int homeScore = g_pTeams[0]->m_nScore;
        NisPlayer* nisPlayer = NisPlayer::Instance();
        nisPlayer->mWinnerSide[NIS_GAME_WINNER] = homeScore < awayScore;
        mUnidentified156 = false;
        functionName = "GameEndSuddenDeath";
    }
    else if (nlSingleton<GameInfoManager>::Instance()
                 ->GetCurrentSettings()
                 ->GameLimitType
        == 1)
    {
        int teamScore
            = g_pTeams[data->pScorer->m_pTeam->m_nSide]->m_nScore;
        if (teamScore
            >= nlSingleton<GameInfoManager>::Instance()
                   ->GetCurrentSettings()
                   ->GoalLimit)
        {
            int awayScore = g_pTeams[1]->m_nScore;
            int homeScore = g_pTeams[0]->m_nScore;
            NisPlayer* nisPlayer = NisPlayer::Instance();
            nisPlayer->mWinnerSide[NIS_GAME_WINNER]
                = homeScore < awayScore;
            mUnidentified156 = false;
            functionName = "GameEndSuddenDeath";
        }
    }

    Call(functionName, filter);

    scoreTied = false;
    mIsAllowedToSkip[0] = scoreTied;
    mIsAllowedToSkip[1] = scoreTied;
    mIsAllowedToSkip[2] = scoreTied;
    mIsAllowedToSkip[3] = scoreTied;

    int pad;
    NetworkPeer* peer;
    NetworkPeerChannel* channel;
    int i;
    peer = g_pNetworkSessionBase->GetLocalPeer();
    for (i = 0; i < (int)peer->mPlayerCount; i++)
    {
        channel = peer->GetNetworkPeerChannel(i);
        pad = channel->mGlobalPadIndex;
        if (data->uTeamIndex
            == nlSingleton<GameInfoManager>::Instance()->GetPlayingSide(
                channel->GetNetworkPeerChannelId()))
        {
            mIsAllowedToSkip[pad] = true;
            scoreTied = true;
        }
    }

    if (!scoreTied && g_pNetworkSessionBase->GetSessionMode() == 0)
    {
        mIsAllowedToSkip[0] = true;
        mIsAllowedToSkip[1] = true;
        mIsAllowedToSkip[2] = true;
        mIsAllowedToSkip[3] = true;
    }
}

/**
 * Address/Size: 0x802870A4 | size: 0x94
 */
void Presentation::OnKickoff()
{
    EffectsGroup* endGame;
    EffectsGroup* score;
    EffectsGroup* suddenDeath;

    suddenDeath
        = EmissionManager::Instance()->GetEffectsGroup("Goal_suddendeath");
    score = EmissionManager::Instance()->GetEffectsGroup("Goal_score");
    endGame = EmissionManager::Instance()->GetEffectsGroup("Goal_endgame");

    EmissionManager::Instance()->Destroy(suddenDeath);
    EmissionManager::Instance()->Destroy(score);
    EmissionManager::Instance()->Destroy(endGame);
}

/**
 * Address/Size: 0x80287138 | size: 0xC
 */
void Presentation::OnSuddenDeath()
{
    PlayGoalEffects("Goal_suddendeath");
}

/**
 * Address/Size: 0x80287144 | size: 0x134
 */
void Presentation::OnMegaStrikeIntro(cPlayer* player)
{
    if (g_pGame == 0)
    {
        return;
    }

    gpNumberDisplay->ResetGoalCount();
    mUnidentified159 = true;
    PauseSuddenDeathMusic();

    if (IsNetworkOrRecordedGame())
    {
        mRandomSeed = gNetworkRandomSeed;
    }

    Call("MegastrikeIntro", "high");

    bool hasLocalPlayer = false;
    mIsAllowedToSkip[0] = hasLocalPlayer;
    mIsAllowedToSkip[1] = hasLocalPlayer;
    mIsAllowedToSkip[2] = hasLocalPlayer;
    mIsAllowedToSkip[3] = hasLocalPlayer;

    NetworkPeer* peer = g_pNetworkSessionBase->GetLocalPeer();
    for (int i = 0; i < (int)peer->mPlayerCount; i++)
    {
        hasLocalPlayer = true;
        NetworkPeerChannel* channel = peer->GetNetworkPeerChannel(i);
        mIsAllowedToSkip[channel->mGlobalPadIndex] = true;
    }

    if (!hasLocalPlayer && g_pNetworkSessionBase->GetSessionMode() == 0)
    {
        mIsAllowedToSkip[0] = true;
        mIsAllowedToSkip[1] = true;
        mIsAllowedToSkip[2] = true;
        mIsAllowedToSkip[3] = true;
    }

    NisPlayer::Instance()->mMaxNumBallsVisible
        = (int)((cFielder*)player)->mUnidentified390;
}

/**
 * Address/Size: 0x80287278 | size: 0x370
 */
void Presentation::HandleMegaStrikeResult(MegaStrikeEndData* data)
{
    ReplayManager::Instance()->fn_801895B0();

    if (g_pGame == 0)
    {
        return;
    }

    s8 goalValue;
    s8 defendingSide;
    s8 goals;
    s8 attempts;
    cPlayer* player;
    u32 unknown;

    unknown = data->unknown_08;
    goalValue = data->goalValue;
    defendingSide = data->defendingSide;
    goals = data->goals;
    bool noGoals = goals == 0;
    attempts = data->attempts;
    player = data->pPlayer;

    mMegaStrikeResult.pPlayer = player;
    mMegaStrikeResult.attempts = attempts;
    mMegaStrikeResult.goals = goals;
    mMegaStrikeResult.defendingSide = defendingSide;
    mMegaStrikeResult.goalValue = goalValue;
    mMegaStrikeResult.unknown_08 = unknown;
    mUnidentified159 = noGoals;

    if (IsNetworkOrRecordedGame())
    {
        mRandomSeed = gNetworkRandomSeed;
    }

    int scoringSide = 1 - data->defendingSide;
    int teamScore;
    const char* gameEndFunction;
    ((GoalOverlay*)g_pOverlayManager->GetScene((SceneList)0x5F))
        ->fn_801F17D0(scoringSide);
    NisPlayer::Instance()->mWinnerSide[NIS_GOAL_WINNER] = scoringSide;

    if (nlSingleton<GameInfoManager>::Instance()->IsInMode4()
        && g_pStrikerChallenge->mCondition == 2
        && data->pPlayer->m_pTeam->m_nSide == 1 && data->goals > 0)
    {
        gameEndFunction = "GameEndMegaStrike";
        NisPlayer* nisPlayer = NisPlayer::Instance();
        nisPlayer->mWinnerSide[NIS_GAME_WINNER] = 1;
        mUnidentified158 = true;
        mUnidentified156 = false;
        Call(gameEndFunction, "high");
    }
    else if (g_pGame != 0
        && (unsigned int)(10.0f
                * (g_pGame->GetGameDuration() - g_pGame->GetGameTime()))
            == 0
        && data->goals != 0)
    {
        gameEndFunction = "GameEndMegaStrike";
        NisPlayer* nisPlayer = NisPlayer::Instance();
        nisPlayer->mWinnerSide[NIS_GAME_WINNER] = scoringSide;
        mUnidentified158 = true;
        mUnidentified157 = false;
        Call(gameEndFunction, "high");
    }
    else
    {
        if (nlSingleton<GameInfoManager>::Instance()
                ->GetCurrentSettings()
                ->GameLimitType
            == 1)
        {
            teamScore
                = g_pTeams[data->pPlayer->m_pTeam->m_nSide]->m_nScore;
            if (data->goals + teamScore
                >= nlSingleton<GameInfoManager>::Instance()
                       ->GetCurrentSettings()
                       ->GoalLimit)
            {
                gameEndFunction = "GameEndMegaStrike";
                NisPlayer* nisPlayer = NisPlayer::Instance();
                nisPlayer->mWinnerSide[NIS_GAME_WINNER] = scoringSide;
                mUnidentified158 = true;
                mUnidentified157 = false;
                Call(gameEndFunction, "high");
                goto setupSkipVotes;
            }
        }

        const char* functionName = "MegastrikeEnd";
        const char* filter = "high";
        bool hasGoals = data->goals != 0;
        mUnidentified156 = false;
        mUnidentified158 = hasGoals;
        mUnidentified157 = !hasGoals;
        if (nlSingleton<GameInfoManager>::Instance()->IsInMode4()
            || nlSingleton<GameInfoManager>::Instance()
                   ->mIsInStrikers101Mode)
        {
            mUnidentified157 = false;
        }
        Call(functionName, filter);
    }

setupSkipVotes:
    GameInfoManager* info;
    int pad;
    bool hasLocalWinner = false;
    mIsAllowedToSkip[0] = hasLocalWinner;
    mIsAllowedToSkip[1] = hasLocalWinner;
    mIsAllowedToSkip[2] = hasLocalWinner;
    mIsAllowedToSkip[3] = hasLocalWinner;

    NetworkPeer* peer = g_pNetworkSessionBase->GetLocalPeer();
    for (int i = 0; i < (int)peer->mPlayerCount; i++)
    {
        NetworkPeerChannel* channel = peer->GetNetworkPeerChannel(i);
        pad = channel->mGlobalPadIndex;
        info = nlSingleton<GameInfoManager>::Instance();
        if (scoringSide
            == info->GetPlayingSide(channel->GetNetworkPeerChannelId()))
        {
            mIsAllowedToSkip[pad] = true;
            hasLocalWinner = true;
        }
    }

    if (!hasLocalWinner && g_pNetworkSessionBase->GetSessionMode() == 0)
    {
        mIsAllowedToSkip[0] = true;
        mIsAllowedToSkip[1] = true;
        mIsAllowedToSkip[2] = true;
        mIsAllowedToSkip[3] = true;
    }
}

/**
 * Address/Size: 0x802875E8 | size: 0x4
 */
void Presentation::OnMegaStrikeEnd(MegaStrikeEndData* data)
{
}

/**
 * Address/Size: 0x802875EC | size: 0x60
 */
void RestoreWorldRendering(Presentation* presentation)
{
    presentation->mUnidentified143 = true;
    SetRenderWorldEffects(1);
    SetWorldNPCsVisible(true);

    BasicStadium* stadium = BasicStadium::GetCurrentStadium();
    stadium->m_pHighRangeTweaks = stadium->m_pStadiumHighRangeTweaks;

    gMegastrikeBackgroundOverlay.Start(
        0.0f, 0.0f, NisPlayer::Instance()->mUnidentified34238);
}

/**
 * Address/Size: 0x8028764C | size: 0x18
 */
void Presentation::OnCharacterDirectionEnd()
{
    if (g_pGame == 0)
    {
        return;
    }
    mWaitingForCharacterDirectionSince = 0.0f;
}

/**
 * Address/Size: 0x80287664 | size: 0x90
 */
void Presentation::ReceiveNisLoaded(u32 machineBits)
{
    tDebugPrintManager::Print(DC_NETWORK,
        "Received NIS Loaded current bitfield %x going to OR %x\n",
        mNisLoadedBits, machineBits);

    mNisLoadedBits |= machineBits;

    tDebugPrintManager::Print(DC_NETWORK,
        "Set machine NIS Loaded bitfield to %x at frame %d\n", mNisLoadedBits,
        gInputManager->mFrameProvider->GetFrame());
}

/**
 * Address/Size: 0x802876F4 | size: 0x178
 */
void Presentation::SendSkipNis()
{
    if (!g_pNetworkSession->IsLiveNetworkGame())
    {
        return;
    }

    if (g_pNetworkSessionBase->GetMachineRoster()->GetTopology() == 0)
    {
        tDebugPrintManager::Print(DC_NETWORK,
            "Sending NetworkSkipNIS message bypass# %d in peer-peer mode\n",
            mByPassNumber);

        NetworkMessageType30 message(mByPassNumber);
        u8 buffer[12];
        int size = gNetworkMessageRegistry->Serialize(&message, buffer, 10);
        int machines = g_pNetworkSessionBase->GetNumMachines();
        for (s8 machine = 0; machine < machines; machine++)
        {
            g_pNetworkSessionBase->Send(machine, buffer, size, true);
        }
    }
    else
    {
        tDebugPrintManager::Print(DC_NETWORK,
            "Sending NetworkSkipNISClient message bypass# %d in client-server mode\n",
            mByPassNumber);

        u8 buffer[12];
        NetworkMessageType31 message(mByPassNumber);
        int size = gNetworkMessageRegistry->Serialize(&message, buffer, 10);
        g_pNetworkSessionBase->Send(0, buffer, size, true);
    }
}

/**
 * Address/Size: 0x8028786C | size: 0x244
 */
int Presentation::ProcessMessage(NetworkMessage* message)
{
    int machines;
    s8 target;
    int size;
    NetworkMessage* receivedMessage = message;
    NetworkMachineRoster* roster = g_pNetworkSessionBase->GetMachineRoster();
    s8 machine = roster->MachineIdxFromConnection(receivedMessage->mSource);
    if (machine < 0 || machine >= roster->GetMachineCount())
    {
        tDebugPrintManager::Print(DC_NETWORK,
            "Discarded message type %d because from unknown connection %x\n",
            (u8)receivedMessage->GetType(), receivedMessage->mSource);
        return 1;
    }

    if (NetTournManager::Instance()->mTournamentMachineMappingActive)
    {
        s8 tournamentIndex = machine;
        machine = NetTournManager::Instance()->TournamentIdxToMachineIdx(machine);
        if (machine < 0 || machine >= g_pNetworkSessionBase->GetNumMachines())
        {
            tDebugPrintManager::Print(DC_NETWORK,
                "Discarded message type %d.  TournamentIdxToMachineIdx changed ID %d to ID %d, but invalid\n",
                (u8)receivedMessage->GetType(), tournamentIndex, machine);
            return 1;
        }
    }

    switch ((u8)receivedMessage->GetType())
    {
    case 30:
        tDebugPrintManager::Print(DC_NETWORK,
            "Received SkipNIS message from machine %d bypass# %d\n", machine,
            ((NetworkMessageType30*)receivedMessage)->mUnidentified08);
        mSkipPastByPass
            = ((NetworkMessageType30*)receivedMessage)->mUnidentified08;
        break;

    case 31:
        if (g_pNetworkSessionBase->GetLocalMachineId() == 0)
        {
            tDebugPrintManager::Print(DC_NETWORK,
                "Host relaying origin machine %d sending NetworkSkipNIS message bypass# %d to all\n",
                machine,
                ((NetworkMessageType31*)receivedMessage)->mUnidentified08);

            u8 buffer[12];
            NetworkMessageType30 relay(
                ((NetworkMessageType31*)receivedMessage)->mUnidentified08);
            size = gNetworkMessageRegistry->Serialize(&relay, buffer, 10);
            machines = g_pNetworkSessionBase->GetNumMachines();
            for (target = 0; target < machines; target++)
            {
                g_pNetworkSessionBase->Send(target, buffer, size, true);
            }
        }
        break;
    }

    return 1;
}

/**
 * Address/Size: 0x80287AB0 | size: 0x7C
 */
bool fn_80287AB0(Presentation* state)
{
    if (IsNetworkOrRecordedGame())
    {
        int machines = g_pNetworkSessionBase->GetNumMachines();
        for (int machine = 0; machine < machines; machine++)
        {
            if ((state->mNisLoadedBits & (1 << machine)) == 0)
            {
                return false;
            }
        }
        return true;
    }

    return false;
}

/**
 * Address/Size: 0x80287B2C | size: 0x8
 */
u32* fn_80287B2C(Presentation* state)
{
    return &state->mRandomSeed;
}

/**
 * Address/Size: 0x80287B34 | size: 0x48
 */
bool fn_80287B34(Presentation* state)
{
    return nlStrCmp<char>("GoalCelebration", state->mCurrentFunction) == 0;
}

/**
 * Address/Size: 0x80287B7C | size: 0x94
 */
bool fn_80287B7C(Presentation* state)
{
    return nlStrCmp<char>("MegastrikeEnd", state->mCurrentFunction) == 0
        || nlStrCmp<char>("GameEndMegaStrike", state->mCurrentFunction) == 0;
}

static inline bool IsSynchronizedNisFunction(Presentation* state)
{
    if (nlTaskManager::m_pInstance->mCurrentState != 2)
    {
        return false;
    }

    return nlStrCmp<char>("GoalCelebration", state->mCurrentFunction) == 0
        || nlStrCmp<char>("MegastrikeEnd", state->mCurrentFunction) == 0
        || nlStrCmp<char>("MegastrikeIntro", state->mCurrentFunction) == 0
        || nlStrCmp<char>("GameEndSuddenDeath", state->mCurrentFunction) == 0
        || nlStrCmp<char>("GameEndNoSuddenDeath", state->mCurrentFunction) == 0
        || nlStrCmp<char>("GameEndMegaStrike", state->mCurrentFunction) == 0;
}

/**
 * Address/Size: 0x80287C10 | size: 0x2A4
 */
void Presentation::WaitForNisLoaded()
{
    bool loaded = false;

    if (gNetworkInputRecording->mPlaybackReady)
    {
        if (IsSynchronizedNisFunction(this))
        {
            int machines = g_pNetworkSessionBase->GetNumMachines();
            loaded = true;
            for (int machine = 0; machine < machines; machine++)
            {
                if ((mNisLoadedBits & (1 << machine)) == 0)
                {
                    loaded = false;
                }
            }

            if (loaded)
            {
                NisPlayer::Instance()->fn_8027CCEC();
            }
        }
        else
        {
            loaded = NisPlayer::Instance()->fn_8027CB44();
        }
    }
    else if (NisPlayer::Instance()->fn_8027CB44())
    {
        loaded = true;
    }

    if (!loaded)
    {
        StopWithUndo();
    }
}

/**
 * Address/Size: 0x80287EB4 | size: 0x634
 */
void Presentation::PlayNis()
{
    if (mByPassing)
    {
        tDebugPrintManager::Print(DC_NETWORK, "Bypassing PlayNIS\n");
        return;
    }

    bool loaded = false;
    if (gNetworkInputRecording->mPlaybackReady)
    {
        if (IsSynchronizedNisFunction(this))
        {
            int machines = g_pNetworkSessionBase->GetNumMachines();
            loaded = true;
            for (int machine = 0; machine < machines; machine++)
            {
                if ((mNisLoadedBits & (1 << machine)) == 0)
                {
                    loaded = false;
                }
            }

            if (loaded)
            {
                NisPlayer::Instance()->fn_8027CCEC();
            }
        }
        else
        {
            loaded = NisPlayer::Instance()->fn_8027CB44();
        }
    }
    else if (NisPlayer::Instance()->fn_8027CB44())
    {
        if (IsNetworkOrRecordedGame() && IsSynchronizedNisFunction(this))
        {
            int machines = g_pNetworkSessionBase->GetNumMachines();
            if (!mUnidentified141)
            {
                if (nlStrCmp<char>(mCurrentFunction, mInterruptWipe) == 0)
                {
                    tDebugPrintManager::Print(DC_NETWORK,
                        "Not doing SendNISLoadedCustomDeterm because only want to send once for %s\n",
                        mCurrentFunction);
                }
                else
                {
                    nlStrNCpy<char>(mInterruptWipe, mCurrentFunction,
                        sizeof(mInterruptWipe));
                    s8 machine
                        = g_pNetworkSessionBase->GetLocalMachineId();
                    u8 machineBit = 1 << machine;
                    g_pGame->fn_80059D80(machineBit);
                    mUnidentified141 = true;
                }
            }

            loaded = true;
            for (int machine = 0; machine < machines; machine++)
            {
                if ((mNisLoadedBits & (1 << machine)) == 0)
                {
                    loaded = false;
                }
            }

            if (loaded == true)
            {
                mRandomSeed = gNetworkRandomSeed;
            }
        }
        else
        {
            loaded = true;
        }
    }

    if (loaded)
    {
        tDebugPrintManager::Print(DC_NETWORK,
            "Starting NIS DoPlay at frame %d\n",
            gInputManager->mFrameProvider->GetFrame());
        NisPlayer::Instance()->Play();
        nlTaskManager::SetNextState(0x10);
        mNisLoadedBits = 0;
        mUnidentified141 = false;
        tDebugPrintManager::Print(
            DC_NETWORK, "Resetting NIS Loaded bitfield\n");
    }
    else
    {
        StopWithUndo();
    }
}

/**
 * Address/Size: 0x802884E8 | size: 0x230
 */
void Presentation::LoadNis(const char* name, int variant)
{
    char fullName[64];
    int winnerTeam;
    bool isCupWinner;
    if (sUseCupPersonaTrophy)
    {
        isCupWinner = true;
    }
    else
    {
        GameInfoManager* gameInfo;
        short winnerSide;
        winnerSide
            = (short)NisPlayer::Instance()->mWinnerSide[NIS_GAME_WINNER];
        gameInfo = nlSingleton<GameInfoManager>::Instance();
        winnerTeam
            = gameInfo->GetCurrentGameInfo()->GetTeam(winnerSide);
        isCupWinner = false;
        if (gameInfo->mIsOnlineMode && gameInfo->IsInMode1()
            && NetTournManager::Instance()->IsCupWinningGame(winnerTeam))
        {
            isCupWinner = true;
        }
    }

    if (isCupWinner)
    {
        const char* trophyName;
        if (sUseCupPersonaTrophy)
        {
            trophyName = GetCupPersonaTrophyNames()[sCupPersonaTrophy];
        }
        else
        {
            int cupPersona = NetTournManager::Instance()->GetCupPersona();
            trophyName = GetCupPersonaTrophyNames()[cupPersona];
        }
        nlSNPrintf(fullName, sizeof(fullName), "%s_%s", trophyName, name);
    }
    else
    {
        if (sUseCupTrophy)
        {
            isCupWinner = true;
        }
        else
        {
            GameInfoManager* gameInfo;
            short winnerSide;
            winnerSide
                = (short)NisPlayer::Instance()
                      ->mWinnerSide[NIS_GAME_WINNER];
            gameInfo = nlSingleton<GameInfoManager>::Instance();
            winnerTeam = gameInfo->GetCurrentGameInfo()->GetTeam(winnerSide);
            isCupWinner = false;
            if (gameInfo->IsInMode3()
                && g_pCupManager->IsCupWinningGame(winnerTeam))
            {
                isCupWinner = true;
            }
        }

        if (isCupWinner)
        {
            const char* trophyName;
            if (sUseCupTrophy)
            {
                trophyName = GetCupTrophyNames()[sCupTrophy];
            }
            else
            {
                int cup = g_pCupManager->GetCurrentMode();
                trophyName = GetCupTrophyNames()[cup];
            }
            nlSNPrintf(
                fullName, sizeof(fullName), "%s_%s", trophyName, name);
        }
        else
        {
            return;
        }
    }

    if (!mByPassing)
    {
        NisPlayer::Instance()->Load(fullName, NIS_TARGET_NONE,
            NIS_NO_STADIUM_OFFSET, NIS_NO_FILTER, NIS_DO_NOT_CARE, variant,
            -1);
    }
}

/**
 * Address/Size: 0x80288718 | size: 0x2C0
 */
void Presentation::PlayOverlay(
    const char* name, float delay, float length)
{
    if (nlSingleton<GameInfoManager>::Instance()->mIsInStrikers101Mode)
    {
        return;
    }

    if (mOverlayDisplayed)
    {
        static_cast<OverlayManager*>(g_pOverlayManager)
            ->SetVisible((SceneList)mOverlayToDisplay, false, false);
    }

    mOverlayDisplayed = false;
    mOverlayToDisplay = SCENE_INVALID;
    mOverlayDisplayLength = 0.0f;
    mOverlayDelay = 0.0f;

    if (nlStrCmp<char>("goal", name) == 0)
    {
        mOverlayToDisplay = (SceneList)0x5F;
        mOverlayDelay = delay;
        mOverlayDisplayLength = length;
        mOverlayDisplayed = false;
        return;
    }

    if (nlStrCmp<char>("highlight", name) == 0)
    {
        mOverlayToDisplay = (SceneList)0x5F;
        mOverlayDelay = delay;
        mOverlayDisplayLength = length;
        mOverlayDisplayed = false;
        GoalOverlay* scene = static_cast<GoalOverlay*>(
            g_pOverlayManager->GetScene((SceneList)0x5F));
        scene->SetHighlightNumber(ReplayChoreo::Instance().fn_80195CBC());
        return;
    }

    if (nlStrCmp<char>("end", name) == 0)
    {
        mOverlayToDisplay = (SceneList)0x5F;
        mOverlayDelay = delay;
        mOverlayDisplayLength = length;
        mOverlayDisplayed = false;
        GoalOverlay* scene = static_cast<GoalOverlay*>(
            g_pOverlayManager->GetScene((SceneList)0x5F));
        scene->DoMatchEndOverlay();
        return;
    }

    if (nlStrCmp<char>("cup", name) == 0)
    {
        mOverlayToDisplay = (SceneList)0x5F;
        mOverlayDelay = delay;
        mOverlayDisplayLength = length;
        mOverlayDisplayed = false;
        GoalOverlay* scene = static_cast<GoalOverlay*>(
            g_pOverlayManager->GetScene((SceneList)0x5F));
        if (sUseCupTrophy)
        {
            scene->DoCupWinOverlay(sCupTrophy);
        }
        else
        {
            scene->DoCupWinOverlay(-1);
        }
        return;
    }

    if (nlStrCmp<char>("megastrike", name) == 0)
    {
        mOverlayToDisplay = (SceneList)0x5F;
        mOverlayDelay = delay;
        mOverlayDisplayLength = length;
        mOverlayDisplayed = false;
    }
}

/**
 * Address/Size: 0x802889D8 | size: 0x64
 */
void Presentation::StopOverlay()
{
    if (mOverlayDisplayed)
    {
        static_cast<OverlayManager*>(g_pOverlayManager)->SetVisible((SceneList)mOverlayToDisplay, false, false);
    }
    mOverlayDisplayed = false;
    mOverlayToDisplay = -2;
    mOverlayDisplayLength = 0.0f;
    mOverlayDelay = 0.0f;
}

/**
 * Address/Size: 0x80288A3C | size: 0x1A4
 */
void Presentation::UpdateAndRenderLetterBox()
{
    if (IsWidescreen())
    {
        return;
    }

    float height = glGetOrthographicHeight();

    static signed char letterBoxSizeValid;
    static float letterBoxSize;
    if (!letterBoxSizeValid)
    {
        letterBoxSize = 0.5f * (height - height * (fn_80112E0C() / fn_80112E14()));
        letterBoxSizeValid = true;
    }

    if (mLetterBoxEnabled)
    {
        mLetterBoxDuration += 0.05f;
    }
    else
    {
        mLetterBoxDuration -= 0.05f;
    }

    if (mLetterBoxDuration < 0.0f)
    {
        mLetterBoxDuration = 0.0f;
    }

    if (mLetterBoxDuration > 1.0f)
    {
        mLetterBoxDuration = 1.0f;
    }

    if (mLetterBoxDuration > 0.0f)
    {
        RLView* view = GetLayerView(eCLV_FrontEnd);
        static const nlColour black = { { 0x00, 0x00, 0x00, 0xFF } };
        GLView* previous = g_ShapeRenderer.m_eView;
        g_ShapeRenderer.m_eView = (GLView*)view;
        nlColour colour = black;
        g_ShapeRenderer.DrawRectangle2D(0.0f, 0.0f,
            glGetOrthographicWidth(), letterBoxSize * mLetterBoxDuration,
            -2.0f, colour, 0);
        g_ShapeRenderer.DrawRectangle2D(0.0f,
            height - letterBoxSize * mLetterBoxDuration,
            glGetOrthographicWidth(), letterBoxSize * mLetterBoxDuration,
            -2.0f, colour, 0);
        g_ShapeRenderer.m_eView = previous;
    }
}

/**
 * Address/Size: 0x80288BE0 | size: 0x134
 */
void Presentation::Reset()
{
    mIsAllowedToSkip[0] = true;
    mIsAllowedToSkip[1] = true;
    mIsAllowedToSkip[2] = true;
    mIsAllowedToSkip[3] = true;

    Call(idleFun, "");

    mNisLoadedBits = 0;
    mUnidentified141 = false;
    mHighlightsLeft = 0;
    mByPassNumber = 0;
    mSkipPastByPass = -1;
    mUnidentified142 = false;
    mUnidentified143 = true;
    mUnidentified156 = false;
    mUnidentified157 = false;
    mUnidentified159 = false;
    mUnidentified160 = false;
    mUnidentified161 = true;
    mUnidentified162 = false;

    tDebugPrintManager::Print(DC_NETWORK,
        "Resetting NIS Loaded bitfield at frame %d\n",
        gInputManager->mFrameProvider->GetFrame());

    mQueuedFunction[0] = '\0';
    mQueuedFilter[0] = '\0';
    mOverlayDisplayed = false;
    StopOverlay();

    Wiper::Instance().Reset();
    NisPlayer::Instance()->Reset();
    ReplayChoreo::Instance().Reset();
    ReplayManager::Instance()->Flush();

    mUnidentified163 = false;
    mUnidentified164 = true;
}

/**
 * Address/Size: 0x80288D14 | size: 0x100
 */
void Presentation::UpdateBallGlow(int level)
{
    if (level >= 0)
    {
        mBallGlowLevel = (float)level;
    }
    else if (g_pBall != 0)
    {
        mBallGlowLevel = fn_800155A0(g_pBall, 0);
    }
    else
    {
        mBallGlowLevel = 0.0f;
    }

    fn_801BA358();

    if (mBallGlowLevel < 1.0f)
    {
        fn_801B9DAC("ball_shot_windup_glow_0");
    }
    else if (mBallGlowLevel < 2.0f)
    {
        fn_801B9DAC("ball_shot_windup_glow_1");
    }
    else if (mBallGlowLevel < 3.0f)
    {
        fn_801B9DAC("ball_shot_windup_glow_2");
    }
    else if (mBallGlowLevel < 4.0f)
    {
        fn_801B9DAC("ball_shot_windup_glow_3");
    }
    else
    {
        fn_801B9DAC("ball_shot_windup_glow_max");
    }
}

/**
 * Address/Size: 0x80288E14 | size: 0xCC
 */
bool Presentation::ClearSkipVotes()
{
    int waiting = 0;
    for (int pad = 0; pad < 4; pad++)
    {
        if (!mIsAllowedToSkip[pad]
            && nlSingleton<GameInfoManager>::Instance()->GetPlayingSide(pad)
                != -1)
        {
            waiting++;
        }
    }

    if (waiting == 0)
    {
        return false;
    }

    for (int pad = 0; pad < 4; pad++)
    {
        mIsAllowedToSkip[pad] = !mIsAllowedToSkip[pad];
    }
    return true;
}

/**
 * Address/Size: 0x80288EE0 | size: 0x118
 */
void Presentation::PlayGoalEffects(const char* effects)
{
    EffectsGroup* group
        = EmissionManager::Instance()->GetEffectsGroup(effects);

    for (int side = 0; side < 2; side++)
    {
        for (int end = 0; end < 2; end++)
        {
            EmissionController* controller
                = EmissionManager::Instance()->Create(group, 2, true, 0);

            nlVector3 position;
            position.x = 20.93f;
            position.y = -2.85f;
            position.z = 3.2f;
            if (side == 1)
            {
                position.x *= -1.0f;
            }
            if (end == 1)
            {
                position.y *= -1.0f;
            }
            controller->SetPosition(position);
        }
    }
}

#include "src/Game/Render/Presentation_interp.cpp"
