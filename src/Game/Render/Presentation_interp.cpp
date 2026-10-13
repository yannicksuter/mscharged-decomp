#include "Game/Task/GameTaskState.h"
#include "Game/Game.h"

// Stadium-effect control was inserted at ID 63 after EU rev1, shifting the
// subsequent script function IDs by one.
enum PresentationFunction
{
    PRESENTATION_SET_NUMBER_DISPLAY_VISIBLE = 62,
#if !defined(VERSION_R4QP01_REV1)
    PRESENTATION_SET_STADIUM_EFFECTS_ENABLED,
#endif
    PRESENTATION_SET_WAIT_TIME,
    PRESENTATION_SET_TEAM_LOGO,
    PRESENTATION_IS_CUP_WINNER,
    PRESENTATION_SHOULD_PLAY_LOSER_REACTION,
    PRESENTATION_IS_LOSER_REACTION_OUTRAGED,
    PRESENTATION_DID_MEGASTRIKE_SCORE,
    PRESENTATION_IS_MEGASTRIKE_ATTEMPT_IN_RANGE,
    PRESENTATION_IS_LAST_MEGASTRIKE_ATTEMPT_GOAL,
    PRESENTATION_IS_LAST_MEGASTRIKE_ATTEMPT_WITHOUT_GOALS,
    PRESENTATION_IS_MEGASTRIKE_ATTEMPT_NOT_GOAL,
    PRESENTATION_RANDOM_CHOICE,
    PRESENTATION_IS_CUP_PERSONA_WINNER,
    PRESENTATION_STOP_JUMBOTRON,
    PRESENTATION_STOP_NIS_CUE,
    PRESENTATION_STOP_OVERLAY,
    PRESENTATION_STOP_AND_RESET_JUMBOTRON,
    PRESENTATION_UNLOCK_FRAME_RATE,
    PRESENTATION_WAIT_FOR_AUTO_REPLAY,
    PRESENTATION_WAIT_FOR_NIS_LOADED,
    PRESENTATION_WAIT_FOR_CHARACTER_DIRECTION,
    PRESENTATION_WAIT_FOR_REPLAY_SAVED,
    PRESENTATION_WAIT_FOR_REPLAY_LOADED,
    PRESENTATION_WAIT_FOR_NIS,
    PRESENTATION_WAIT_FOR_TIMER,
    PRESENTATION_IS_TEAM_ZERO_GOAL_WINNER,
    PRESENTATION_WIPE,
};

/**
 * Address/Size: 0x80288FF8 | size: 0x1444
 */
void Presentation::DoFunctionCall(unsigned int function)
{
    cCharacter* character;
    switch (function)
    {
    case 0:
        if (!g_e3_Build)
        {
            mSkipPressed = false;
            mByPassWasSkipped = false;
            mInsideByPass = true;
            tDebugPrintManager::Print(
                DC_NETWORK, "BeginByPass Bypass# %d\n", mByPassNumber);
        }
        break;
    case 1:
    {
        float target = *(float*)&m_SP[-1];
        float rate = *(float*)&m_SP[-2];
        m_SP -= 3;
        NisPlayer* nisPlayer = NisPlayer::Instance();
        gMegastrikeBackgroundOverlay.Start(
            rate, target, nisPlayer->mMegaStrikeSide);
        break;
    }
    case 2:
        ++lbl_806E2130;
        g_pGame->CleanupMegaStrikeGameplay();
        --lbl_806E2130;
        break;
    case 3:
    {
        int side;
        int player;
        cTeam** team = g_pTeams;
        for (side = 0; side < 2; side++, team++)
        {
            for (player = 0; player < 4; player++)
            {
                cFielder* fielder = (*team)->GetFielder(player);
                fielder->m_Dirt = 0.0f;
                fielder->m_nDamageType = DAMAGE_TEXTURE_STADIUM;
                fielder->m_MinDirt = 0.0f;
            }
        }
        break;
    }
    case 4:
    {
        {
            cFielder* captain = g_pTeams[0]->GetCaptain();
            captain->m_Dirt = 0.0f;
            captain->m_nDamageType = DAMAGE_TEXTURE_STADIUM;
        }
        {
            cFielder* captain = g_pTeams[0]->GetCaptain();
            captain->m_MinDirt = 0.0f;
        }
        {
            cFielder* captain = g_pTeams[1]->GetCaptain();
            captain->m_Dirt = 0.0f;
            captain->m_nDamageType = DAMAGE_TEXTURE_STADIUM;
        }
        {
            cFielder* captain = g_pTeams[1]->GetCaptain();
            captain->m_MinDirt = 0.0f;
        }
        break;
    }
    case 5:
        NisPlayer::Instance()->ClearSecondaryNis();
        break;
    case 6:
        gBlinkingEnabled = false;
        break;
    case 7:
        NisPlayer::Instance()->PreserveNisCueOnReset();
        break;
    case 8:
        ++m_SP;
        m_SP[-1] = NisPlayer::Instance()->AllowsPIP();
        break;
    case 9:
    {
        bool value = m_SP[-1] != 0;
        --m_SP;
        mDrawBlackOverlay = value;
        break;
    }
    case 10:
    {
        bool value = m_SP[-1] != 0;
        --m_SP;
        mChargeShadowsVisible = value;
        break;
    }
    case 11:
    {
        bool enabled = m_SP[-1] != 0;
        --m_SP;
        SetRenderWorldEffects(enabled);
        SetWorldNPCsVisible(enabled);
        if (enabled)
        {
            BasicStadium* stadium = BasicStadium::GetCurrentStadium();
            stadium->m_pHighRangeTweaks = stadium->m_pStadiumHighRangeTweaks;
        }
        else
        {
            BasicStadium* stadium = BasicStadium::GetCurrentStadium();
            stadium->m_pHighRangeTweaks
                = stadium->m_pMegastrikeHighRangeTweaks;
        }
        break;
    }
    case 12:
        gBlinkingEnabled = true;
        break;
    case 13:
        ReplayChoreo::Instance().Finish();
        break;
    case 14:
        if (!g_e3_Build)
        {
            if (mByPassing)
            {
                mByPassWasSkipped = true;
            }
            mInsideByPass = false;
            mByPassing = false;
            ++mByPassNumber;
            tDebugPrintManager::Print(
                DC_NETWORK, "EndByPass New Bypass# %d\n", mByPassNumber);
        }
        break;
    case 15:
        NisPlayer::Instance()->ResetToPIPOverlay();
        break;
    case 16:
    {
        float target = *(float*)&m_SP[-1];
        float rate = *(float*)&m_SP[-2];
        m_SP -= 3;
        NisPlayer* nisPlayer = NisPlayer::Instance();
        gMegastrikeBackgroundOverlay.Start(
            rate, target, nisPlayer->mMegaStrikeSide);
        break;
    }
    case 17:
        ++m_SP;
        m_SP[-1] = ClearSkipVotes();
        break;
    case 18:
        if (g_pNetworkSessionBase->GetSessionMode())
        {
            NetworkStatsManager::Instance()->CalculateAndReportGameResult(0);
        }
        break;
    case 19:
        UpdateAllowedToSkip();
        break;
    case 20:
    {
        bool enabled = m_SP[-1] != 0;
        --m_SP;
        NisPlayer::Instance()->EnableWorldDarkening(enabled);
        break;
    }
    case 21:
        ++m_SP;
        m_SP[-1] = ReplayChoreo::Instance().NumHighlights() > 0;
        break;
    case 22:
        SendSkipNis();
        break;
    case 23:
        StopWithUndo();
        break;
    case 24:
    {
        bool enabled = m_SP[-1] != 0;
        float height = *(float*)&m_SP[-2];
        float width = *(float*)&m_SP[-3];
        float verticalOffset = *(float*)&m_SP[-4];
        m_SP -= 4;
        SetHolotronDimensions(verticalOffset, width, height);
        SetHolotronCameraTrackingEnabled(enabled);
        break;
    }
    case 25:
        ++lbl_806E2130;
        gpNumberDisplay->mVisible = false;
        g_pGame->InitMegaStrikeGameplay();
        --lbl_806E2130;
        break;
    case 26:
    {
        int original = m_SP[-1];
        int side = NisPlayer::Instance()->mMegaStrikeSide;
        int team = GameInfoManager::Instance()
                       ->GetCurrentGameInfo()
                       ->GetTeam((short)side);
        m_SP[-1] = team == original;
        if (m_RunState == INTERPRETER_SUSPENDED)
        {
            m_SP[-1] = original;
        }
        break;
    }
    case 27:
    {
        int original = m_SP[-1];
        m_SP[-1] = original == GameInfoManager::Instance()->GetStadium();
        if (m_RunState == INTERPRETER_SUSPENDED)
        {
            m_SP[-1] = original;
        }
        break;
    }
    case 28:
        ++m_SP;
        m_SP[-1] = g_pGame->m_eGameState == GS_END_GAME;
        break;
    case 29:
        ++m_SP;
        m_SP[-1] = NisPlayer::Instance()->mMegaStrikeSide == 0;
        break;
    case 30:
        ++m_SP;
        m_SP[-1] = Config::Global().Get<bool>("no_weather", false) == true
            || GameInfoManager::Instance()->IsRule0x4Equal1() == true
            || GameInfoManager::Instance()->IsInMode4();
        break;
    case 31:
        if (g_pNetworkSession->IsLiveNetworkGame())
        {
            NetworkStatsManager::Instance()->ClearGameResultReported();
        }
        break;
    case 32:
        NisPlayer::Instance()->ReleaseNisCue();
        break;
    case 33:
    {
        int variant = m_SP[-1];
        const char* name = (const char*)m_SP[-2];
        m_SP -= 2;
        LoadNis(name, variant);
        break;
    }
    case 34:
        ReplayChoreo::Instance().LoadNextHighlight();
        break;
    case 35:
    {
        int animationClass = m_SP[-1];
        --m_SP;
        Jumbotron::instance.m_AnimationClass = animationClass;
        Jumbotron::instance.BeginLoad();
        break;
    }
    case 36:
    {
        int argument6 = m_SP[-1];
        int argument5 = m_SP[-2];
        NisWinnerType winner = (NisWinnerType)m_SP[-3];
        NisUseFilter filter = (NisUseFilter)m_SP[-4];
        NisUseStadiumOffset stadiumOffset = (NisUseStadiumOffset)m_SP[-5];
        NisTarget target = (NisTarget)m_SP[-6];
        const char* name = (const char*)m_SP[-7];
        m_SP -= 7;
        if (!mByPassing)
        {
            NisPlayer::Instance()->Load(name, target, stadiumOffset, filter,
                winner, argument5, argument6);
        }
        break;
    }
    case 37:
        BeginFrameTask::s_FramerateLocked = true;
        break;
    case 38:
        gpNumberDisplay->ShowAccumulatedScore();
        break;
    case 39:
        gpNumberDisplay->ShowScores();
        break;
    case 40:
        gpNumberDisplay->IncrementGoalCount();
        break;
    case 41:
        if (!mByPassing)
        {
            mWaitingForCharacterDirectionSince
                = GetFixedUpdateTask()->mSimulationTime;
            NisPlayer::Instance()->RandomizeBeginPositions();
        }
        break;
    case 42:
        break;
    case 43:
        if (ReplayManager::Instance()->IsSavingReplay() == true)
        {
            StopWithUndo();
        }
        else if (!mByPassing)
        {
            if (nlTaskManager::m_pInstance->mCurrentState != TASK_REPLAY
                && !IsDuringGamePauseState())
            {
                nlTaskManager::SetNextState(TASK_REPLAY);
            }
            ReplayChoreo::Instance().StartAutoReplay(false);
            static_cast<OverlayManager*>(g_pOverlayManager)
                ->SetCurrentTextOverlaySlide(SLIDE_NAME_TEXT_REPLAY);
            static_cast<OverlayManager*>(g_pOverlayManager)
                ->SetVisible(OVERLAY_IN_GAME_TEXT, true, true);
            static_cast<OverlayManager*>(g_pOverlayManager)
                ->mIsInHighlights = false;
        }
        break;
    case 44:
        PlaySound(11, 0x8CEE6665, 0, 0);
        if (g_pGame->m_eGameState == GS_END_GAME)
        {
            PlaySound(10, 0x42F55573, 0, 0);
        }
        break;
    case 45:
        if (ReplayManager::Instance()->IsLoadingReplay() == true)
        {
            StopWithUndo();
        }
        else if (!mByPassing)
        {
            if (nlTaskManager::m_pInstance->mCurrentState != TASK_REPLAY
                && !IsDuringGamePauseState())
            {
                nlTaskManager::SetNextState(TASK_REPLAY);
            }
            ReplayChoreo::Instance().StartAutoReplay(true);
            static_cast<OverlayManager*>(g_pOverlayManager)
                ->SetCurrentTextOverlaySlide(SLIDE_NAME_TEXT_REPLAY);
            static_cast<OverlayManager*>(g_pOverlayManager)
                ->SetVisible(OVERLAY_IN_GAME_TEXT, false, true);
            static_cast<OverlayManager*>(g_pOverlayManager)
                ->mIsInHighlights = true;
            StopOverlay();
            PlayOverlay("highlight", 0.5f, 30.0f);
        }
        break;
    case 46:
        if (!mByPassing)
        {
            mHighlightsLeft = ReplayChoreo::Instance().NumHighlights();
            if (mHighlightsLeft > 0)
            {
                --mHighlightsLeft;
                Call("PlayHighlight", "");
            }
        }
        break;
    case 47:
        Jumbotron::instance.WaitForLoad();
        Jumbotron::instance.BeginPlaying();
        break;
    case 48:
        PlayNis();
        break;
    case 49:
    {
        unsigned long cue = m_SP[-1];
        --m_SP;
        if (!g_e3_Build || cue != 0x625F1997)
        {
            NisPlayer::Instance()->PrepareNisCue(cue);
            NisPlayer::Instance()->StartNisCue();
        }
        break;
    }
    case 50:
    {
        float length = *(float*)&m_SP[-1];
        float delay = *(float*)&m_SP[-2];
        const char* name = (const char*)m_SP[-3];
        m_SP -= 3;
        PlayOverlay(name, delay, length);
        break;
    }
    case 51:
    {
        const char* param = (const char*)m_SP[-1];
        const char* type = (const char*)m_SP[-2];
        m_SP -= 2;
        NISData* data = g_NISDataPool.Allocate();
        data->Type = type;
        data->Param = param;
        g_pGame->QueueNIS(data);
        break;
    }
    case 52:
        gpNumberDisplay->mHoldUntilKickoff = false;
        break;
    case 53:
    {
        NisPlayer* nisPlayer = NisPlayer::Instance();
        nisPlayer->mCameraOverrun = 0.0f;
        break;
    }
    case 54:
        NisPlayer::Instance()->Reset();
        break;
    case 55:
    {
        u32* stack = m_SP;
        const char* filter = (const char*)*--stack;
        m_SP = stack;
        ScreenTransitionManager::Instance()->m_SelectedTransition = 0;
        ScreenTransitionManager::Instance()->SelectRandomTransition(filter);
        break;
    }
    case 56:
    {
        int replayTime = -30;
        ReplayChoreo::Instance().SaveHighlight(
            ReplayManager::Instance()->GetReplayExcitement(replayTime));
        break;
    }
    case 57:
    {
        int level = m_SP[-1];
        --m_SP;
        UpdateBallGlow(level);
        break;
    }
    case 58:
    {
        bool excited = m_SP[-1] != 0;
        --m_SP;
        if (excited)
        {
            SetCrowdImpostorsExcited();
        }
        else
        {
            SetCrowdImpostorsIdle();
        }
        break;
    }
    case 59:
        Pop();
        break;
    case 60:
    {
        float value = *(float*)&m_SP[-1];
        --m_SP;
        NisPlayer::Instance()->FadeWorldDarkening(value);
        break;
    }
    case 61:
    {
        float value = *(float*)&m_SP[-1];
        bool direction = m_SP[-2] != 0;
        m_SP -= 3;
        int side = NisPlayer::Instance()->mMegaStrikeSide == 0 ? 0 : 4;
        character = g_pCharacters[side];
        character->SetMegaBlend(value, direction ? 1.0f : 0.0f);
        switch (character->m_DetChar.m_eCharacterClass)
        {
        case PEACH:
        case WALUIGI:
        case YOSHI:
            gDisableHighRange = direction;
            break;
        }
        break;
    }
    case PRESENTATION_SET_NUMBER_DISPLAY_VISIBLE:
    {
        bool visible = m_SP[-1] != 0;
        --m_SP;
        gpNumberDisplay->mVisible = visible;
        break;
    }
#if !defined(VERSION_R4QP01_REV1)
    case PRESENTATION_SET_STADIUM_EFFECTS_ENABLED:
    {
        bool enabled = m_SP[-1] != 0;
        --m_SP;
        mNisStadiumEffectsEnabled = enabled;
        BasicStadium::GetCurrentStadium()->SetEffectsActive(55, enabled);
        break;
    }
#endif
    case PRESENTATION_SET_WAIT_TIME:
    {
        float value = *(float*)&m_SP[-1];
        --m_SP;
        mWaitTimeRemaining = value;
        break;
    }
    case PRESENTATION_SET_TEAM_LOGO:
    {
        NisWinnerType winner = (NisWinnerType)m_SP[-1];
        NisTarget target = (NisTarget)m_SP[-2];
        m_SP -= 2;
        NisPlayer::Instance()->SetTeamLogo(target, winner);
        break;
    }
    case PRESENTATION_IS_CUP_WINNER:
        ++m_SP;
        m_SP[-1] = IsCupWinner();
        break;
    case PRESENTATION_SHOULD_PLAY_LOSER_REACTION:
        ++m_SP;
        m_SP[-1] = mPlayLoserReaction;
        break;
    case PRESENTATION_IS_LOSER_REACTION_OUTRAGED:
        ++m_SP;
        m_SP[-1] = mLoserReactionOutraged;
        break;
    case PRESENTATION_DID_MEGASTRIKE_SCORE:
        ++m_SP;
        m_SP[-1] = mMegaStrikeScored;
        break;
    case PRESENTATION_IS_MEGASTRIKE_ATTEMPT_IN_RANGE:
    {
        int original = m_SP[-1];
        m_SP[-1] = (s8)mMegaStrikeResult.attempts > original;
        if (m_RunState == INTERPRETER_SUSPENDED)
        {
            m_SP[-1] = original;
        }
        break;
    }
    case PRESENTATION_IS_LAST_MEGASTRIKE_ATTEMPT_GOAL:
    {
        int original = m_SP[-1];
        m_SP[-1] = original == (s8)mMegaStrikeResult.attempts - 1
            && (g_pGame->GetMegaStrikeGoalMask() & (1 << original)) != 0;
        if (m_RunState == INTERPRETER_SUSPENDED)
        {
            m_SP[-1] = original;
        }
        break;
    }
    case PRESENTATION_IS_LAST_MEGASTRIKE_ATTEMPT_WITHOUT_GOALS:
    {
        int original = m_SP[-1];
        m_SP[-1] = original == (s8)mMegaStrikeResult.attempts - 1
            && g_pGame->GetMegaStrikeGoalMask() == 0;
        if (m_RunState == INTERPRETER_SUSPENDED)
        {
            m_SP[-1] = original;
        }
        break;
    }
    case PRESENTATION_IS_MEGASTRIKE_ATTEMPT_NOT_GOAL:
    {
        int original = m_SP[-1];
        m_SP[-1] = (g_pGame->GetMegaStrikeGoalMask() & (1 << original)) == 0;
        if (m_RunState == INTERPRETER_SUSPENDED)
        {
            m_SP[-1] = original;
        }
        break;
    }
    case PRESENTATION_RANDOM_CHOICE:
        ++m_SP;
        m_SP[-1] = nlRandomf(1.0f, &mRandomSeed) <= gPresentationRandomChoiceChance;
        break;
    case PRESENTATION_IS_CUP_PERSONA_WINNER:
        ++m_SP;
        m_SP[-1] = IsCupPersonaWinner();
        break;
    case PRESENTATION_STOP_JUMBOTRON:
        if (Jumbotron::instance.m_State == 4)
        {
            Jumbotron::instance.StopPlaying();
        }
        break;
    case PRESENTATION_STOP_NIS_CUE:
        NisPlayer::Instance()->StopNisCue();
        break;
    case PRESENTATION_STOP_OVERLAY:
        StopOverlay();
        break;
    case PRESENTATION_STOP_AND_RESET_JUMBOTRON:
        if (Jumbotron::instance.m_State == 4)
        {
            Jumbotron::instance.StopPlaying();
        }
        Jumbotron::instance.Reset();
        break;
    case PRESENTATION_UNLOCK_FRAME_RATE:
        BeginFrameTask::s_FramerateLocked = false;
        break;
    case PRESENTATION_WAIT_FOR_AUTO_REPLAY:
    {
        u32* stack = m_SP;
        const char* filter = (const char*)*--stack;
        m_SP = stack;
        if (!mByPassing)
        {
            WaitForAutoReplayCompletion(filter);
        }
        break;
    }
    case PRESENTATION_WAIT_FOR_NIS_LOADED:
        WaitForNisLoaded();
        break;
    case PRESENTATION_WAIT_FOR_CHARACTER_DIRECTION:
    {
        int maximumWait = 1;
        if (!mByPassing && mWaitingForCharacterDirectionSince > 0.0f
            && GetFixedUpdateTask()->mSimulationTime
                    - mWaitingForCharacterDirectionSince
                < maximumWait)
        {
            StopWithUndo();
        }
        break;
    }
    case PRESENTATION_WAIT_FOR_REPLAY_SAVED:
        if (ReplayManager::Instance()->IsSavingReplay() == true)
        {
            StopWithUndo();
        }
        break;
    case PRESENTATION_WAIT_FOR_REPLAY_LOADED:
        if (ReplayManager::Instance()->IsLoadingReplay() == true)
        {
            StopWithUndo();
        }
        break;
    case PRESENTATION_WAIT_FOR_NIS:
    {
        u32* stack = m_SP;
        const char* wipe = (const char*)*--stack;
        m_SP = stack;
        if (!mByPassing)
        {
            WaitForNisCompletion(wipe);
        }
        break;
    }
    case PRESENTATION_WAIT_FOR_TIMER:
        if (mWaitTimeRemaining > 0.0f)
        {
            StopWithUndo();
        }
        break;
    case PRESENTATION_IS_TEAM_ZERO_GOAL_WINNER:
        ++m_SP;
        m_SP[-1]
            = NisPlayer::Instance()->mWinnerSide[NIS_GOAL_WINNER] == 0;
        break;
    case PRESENTATION_WIPE:
    {
        u32* stack = m_SP;
        const char* wipe = (const char*)*--stack;
        m_SP = stack;
        if (!mByPassing)
        {
            mDrawBlackOverlay = false;
            Wiper::Instance().DoWipe(wipe);
            if (!Wiper::Instance().CutHasOccurred()
                && Wiper::Instance().WipeInProgress())
            {
                StopWithUndo();
            }
        }
        break;
    }
    default:
        nlBreak();
        break;
    }
}
