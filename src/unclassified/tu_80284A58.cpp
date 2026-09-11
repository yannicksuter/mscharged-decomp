#include "unclassified/tu_80284A58.h"

#include "Game/Ball.h"
#include "Game/BaseGameSceneManager.h"
#include "Game/BasicStadium.h"
#include "Game/Debug/ShapeRender.h"
#include "Game/Effects/EmissionController.h"
#include "Game/Effects/EmissionManager.h"
#include "Game/FE/feManager.h"
#include "Game/Game.h"
#include "Game/GameInfo.h"
#include "Game/InputManager.h"
#include "Game/NetworkMessageRegistry.h"
#include "Game/NetworkMessages.h"
#include "Game/NetTournManager.h"
#include "Game/NetworkSession.h"
#include "Game/NisPlayer.h"
#include "Game/ReplayChoreo.h"
#include "Game/ReplayManager.h"
#include "Game/Render/RLView.h"
#include "Game/Render/RLViewLayers.h"
#include "Game/Render/StadiumLoading.h"
#include "Game/Render/Wiper.h"
#include "Game/Sys/debug.h"
#include "Game/Task/BeginFrameTask.h"
#include "Game/Task/FixedUpdateTask.h"
#include "Game/Task/GameRenderTask.h"
#include "Game/AI/Fielder.h"
#include "Game/Camera/tu_800F9460.h"
#include "Game/Team.h"
#include "unclassified/tu_801AE530.h"
#include "Game/TweakQuery.h"
#include "NL/gl/gl.h"
#include "NL/globalpad.h"
#include "NL/nlConfig.h"
#include "NL/nlFile.h"
#include "NL/nlMath.h"
#include "NL/nlString.h"
#include "NL/nlTask.h"
#include "unclassified/tu_801A6824.h"

extern "C"
{
    void fn_801B9DAC(const char* name);
    void fn_801BA358();
    void fn_801E230C(void* manager, int scene, bool visible, bool immediate);
    void fn_801E2564(void* manager);
    void fn_8027CA44(NisPlayer* player);
    void fn_8027ED18(NisPlayer* player);
    void fn_80195868(ReplayChoreo* choreo, float deltaTime);
    void fn_801959F0(ReplayChoreo* choreo, int quality);
    int fn_8018A16C(ReplayManager* manager, float time);
    float fn_800155A0(void* ball, int index);
}

// Retail keeps the "a goal countdown is on screen" test as a small inline
// predicate: inlining it by hand at the single call site changes Update's
// codegen, so it is a real source-level helper.
static inline bool IsNumberDisplayCounting()
{
    return gpNumberDisplay->mUnidentified025
        && gpNumberDisplay->mUnidentified018 > 0.0f;
}

static const char* sIdleFunction = "Idle";
static bool sLoopPresentation;
static nlColour sLetterBoxColour = { { 0x00, 0x00, 0x00, 0xFF } };

static inline bool IsDuringGamePauseState()
{
    return !FrontEnd::m_bGameOver
        && GetFixedUpdateTask()->mfFrameLockTime <= 0.0f
        && nlTaskManager::m_pInstance->mCurrentState == 1;
}

/**
 * Address/Size: 0x80284A58 | size: 0x64
 */
UnidentifiedPresentationState* GetPresentation()
{
    static UnidentifiedPresentationState instance;
    return &instance;
}

/**
 * Address/Size: 0x80284ABC | size: 0x58
 */
UnidentifiedPresentationState::~UnidentifiedPresentationState()
{
}

/**
 * Address/Size: 0x80284B14 | size: 0x1AC
 */
UnidentifiedPresentationState::UnidentifiedPresentationState()
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

    nlStrNCpy(mCurrentFunction, sIdleFunction, 64);
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
void UnidentifiedPresentationState::UpdateAllowedToSkip()
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

    NetworkPeer* peer = g_pNetworkSessionBase->GetLocalPeer();
    for (int i = 0; i < (int)peer->mPlayerCount; i++)
    {
        NetworkPeerChannel* channel = peer->GetNetworkPeerChannel(i);
        int pad = channel->mGlobalPadIndex;
        int side = NisPlayer::Instance()->mWinnerSide[NIS_GAME_WINNER];
        if (side
            == nlSingleton<GameInfoManager>::Instance()->GetPlayingSide(
                channel->GetNetworkPeerChannelId()))
        {
            mIsAllowedToSkip[pad] = true;
        }
    }
}

/**
 * Address/Size: 0x80284DA8 | size: 0x3E4
 */
bool UnidentifiedPresentationState::DetectSkipPress()
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

    if (((NetworkSessionControl*)g_pNetworkSessionBase)->GetSessionMode() != 0)
    {
        if (nlStrCmp<char>(mCurrentFunction, "GameBegin") != 0)
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
        if (g_pPadManager->GetPad(i)->PlatJustPressed(7, true))
        {
            return true;
        }
    }

    return false;
}

/**
 * Address/Size: 0x8028518C | size: 0x588
 */
void UnidentifiedPresentationState::Finish()
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
                fn_8027ED18(NisPlayer::Instance());
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

    Call(sIdleFunction, "");

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

    if (strcmp(mCurrentFunction, sIdleFunction) == 0)
    {
        ReplayChoreo::Instance().Reset();
    }
}

/**
 * Address/Size: 0x80285714 | size: 0x44
 */
void fn_80285714(UnidentifiedPresentationState* state, u32 from, u32 to)
{
    if (to == 1)
    {
        g_pOverlayManager->GetScene((SceneList)0x66)->SetVisible(false);
    }
}

/**
 * Address/Size: 0x80285758 | size: 0x6C4
 */
void UnidentifiedPresentationState::Update(float deltaTime)
{
    if (nlSingleton<UnidentifiedCameraEffects>::s_pInstance != 0)
    {
        nlSingleton<UnidentifiedCameraEffects>::s_pInstance->Update(deltaTime);
    }

    if (gpNumberDisplay != 0)
    {
        gpNumberDisplay->fn_801AE728(deltaTime);
    }

    fn_8027CA44(NisPlayer::Instance());

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
            fn_801959F0(&ReplayChoreo::Instance(),
                fn_8018A16C(ReplayManager::Instance(), -30.0f));
            mDisplayLetterBox = 0.0f;
        }
    }

    if (!IsDuringGamePauseState())
    {
        if (nlStrCmp<char>(mCurrentFunction, "GameBegin") == 0)
        {
            if (nlTaskManager::m_pInstance->mCurrentState != 0x100)
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

            if (!skipPastByPass && g_pNetworkSession->IsLiveNetworkGame())
            {
                SendSkipNis();
            }

            tDebugPrintManager::Print(DC_NETWORK, "Bypassing...\n");
        }
    }

    if (m_RunState == 2)
    {
        Finish();
    }

    Wiper::Instance().Render();
    UpdateAndRenderLetterBox();

    if (mUnidentified163)
    {
        RLView* view = GetLayerView(eCLV_FrontEnd);
        RLView* previous = (RLView*)g_ShapeRenderer.m_eView;
        nlColour colour = sLetterBoxColour;
        g_ShapeRenderer.m_eView = (GLView*)view;
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
            fn_801E230C(g_pOverlayManager, mOverlayToDisplay, true, true);
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
                fn_801E230C(g_pOverlayManager, mOverlayToDisplay, false, false);
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
void fn_80285E1C()
{
}

/**
 * Address/Size: 0x80285E20 | size: 0xCC
 */
bool IsIdleAndNoShotInProgress(UnidentifiedPresentationState* presentation)
{
    if (nlStrCmp<char>(sIdleFunction, presentation->mCurrentFunction) != 0)
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
bool DuringEndOfGamePresentation(UnidentifiedPresentationState* presentation)
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
void UnidentifiedPresentationState::Call(
    const char* functionName, const char* nisFilter)
{
    if (nlSingleton<UnidentifiedCameraEffects>::s_pInstance != 0)
    {
        nlSingleton<UnidentifiedCameraEffects>::s_pInstance->Reset();
    }

    if (nlStrCmp<char>(sIdleFunction, mCurrentFunction) != 0
        && nlStrCmp<char>(sIdleFunction, functionName) != 0)
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
    Reset();
    CallFunction(nlStringHash(functionName));
}

/**
 * Address/Size: 0x80286288 | size: 0x10
 */
void fn_80286288(UnidentifiedPresentationState* state)
{
    state->Call("PlayHighlight", "");
}

/**
 * Address/Size: 0x80286BA0 | size: 0x4
 */
void UnidentifiedPresentationState::OnGoalieSave(void* data)
{
}

/**
 * Address/Size: 0x802870A4 | size: 0x94
 */
void UnidentifiedPresentationState::OnSuddenDeath(void* data)
{
    EffectsGroup* suddenDeath
        = EmissionManager::Instance()->GetEffectsGroup("Goal_suddendeath");
    EffectsGroup* score
        = EmissionManager::Instance()->GetEffectsGroup("Goal_score");
    EffectsGroup* endGame
        = EmissionManager::Instance()->GetEffectsGroup("Goal_endgame");

    EmissionManager::Instance()->Destroy(suddenDeath);
    EmissionManager::Instance()->Destroy(score);
    EmissionManager::Instance()->Destroy(endGame);
}

/**
 * Address/Size: 0x80287138 | size: 0xC
 */
void UnidentifiedPresentationState::OnMegaStrikeIntro(void* data)
{
    PlayGoalEffects("Goal_suddendeath");
}

/**
 * Address/Size: 0x802875E8 | size: 0x4
 */
void fn_802875E8()
{
}

/**
 * Address/Size: 0x802875EC | size: 0x60
 */
void RestoreWorldRendering(UnidentifiedPresentationState* presentation)
{
    presentation->mUnidentified143 = true;
    SetRenderWorldEffects(1);
    SetWorldNPCsVisible(true);

    BasicStadium* stadium = BasicStadium::GetCurrentStadium();
    stadium->m_pHighRangeTweaks = stadium->m_pStadiumHighRangeTweaks;

    gScreenOverlay_801A6824.Start(
        0.0f, 0.0f, NisPlayer::Instance()->mUnidentified34238);
}

/**
 * Address/Size: 0x8028764C | size: 0x18
 */
void UnidentifiedPresentationState::OnCharacterDirectionEnd(void* data)
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
void UnidentifiedPresentationState::ReceiveNisLoaded(u32 machineBits)
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
void UnidentifiedPresentationState::SendSkipNis()
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

        NetworkMessageType30 message;
        message.mUnidentified08 = mByPassNumber;
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

        NetworkMessageType31 message;
        message.mUnidentified08 = mByPassNumber;
        u8 buffer[12];
        int size = gNetworkMessageRegistry->Serialize(&message, buffer, 10);
        g_pNetworkSessionBase->Send(0, buffer, size, true);
    }
}

/**
 * Address/Size: 0x8028786C | size: 0x244
 */
int UnidentifiedPresentationState::ProcessMessage(NetworkMessage* message)
{
    NetworkMachineRoster* roster = g_pNetworkSessionBase->GetMachineRoster();
    s8 machine = roster->MachineIdxFromConnection(message->mSource);
    if (machine < 0 || machine >= roster->GetMachineCount())
    {
        tDebugPrintManager::Print(DC_NETWORK,
            "Discarded message type %d because from unknown connection %x\n",
            (u8)message->GetType(), message->mSource);
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
                (u8)message->GetType(), tournamentIndex, machine);
            return 1;
        }
    }

    switch ((u8)message->GetType())
    {
    case 30:
        tDebugPrintManager::Print(DC_NETWORK,
            "Received SkipNIS message from machine %d bypass# %d\n", machine,
            ((NetworkMessageType30*)message)->mUnidentified08);
        mSkipPastByPass = ((NetworkMessageType30*)message)->mUnidentified08;
        break;

    case 31:
        if (g_pNetworkSessionBase->GetLocalMachineId() == 0)
        {
            tDebugPrintManager::Print(DC_NETWORK,
                "Host relaying origin machine %d sending NetworkSkipNIS message bypass# %d to all\n",
                machine, ((NetworkMessageType31*)message)->mUnidentified08);

            NetworkMessageType30 relay;
            relay.mUnidentified08
                = ((NetworkMessageType31*)message)->mUnidentified08;
            u8 buffer[12];
            int size
                = gNetworkMessageRegistry->Serialize(&relay, buffer, 10);
            int machines = g_pNetworkSessionBase->GetNumMachines();
            for (s8 target = 0; target < machines; target++)
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
bool fn_80287AB0(UnidentifiedPresentationState* state)
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
u32* fn_80287B2C(UnidentifiedPresentationState* state)
{
    return &state->mRandomSeed;
}

/**
 * Address/Size: 0x80287B34 | size: 0x48
 */
bool fn_80287B34(UnidentifiedPresentationState* state)
{
    return nlStrCmp<char>("GoalCelebration", state->mCurrentFunction) == 0;
}

/**
 * Address/Size: 0x80287B7C | size: 0x94
 */
bool fn_80287B7C(UnidentifiedPresentationState* state)
{
    return nlStrCmp<char>("MegastrikeEnd", state->mCurrentFunction) == 0
        || nlStrCmp<char>("GameEndMegaStrike", state->mCurrentFunction) == 0;
}

/**
 * Address/Size: 0x802889D8 | size: 0x64
 */
void UnidentifiedPresentationState::StopOverlay()
{
    if (mOverlayDisplayed)
    {
        fn_801E230C(g_pOverlayManager, mOverlayToDisplay, false, false);
    }
    mOverlayDisplayed = false;
    mOverlayToDisplay = -2;
    mOverlayDisplayLength = 0.0f;
    mOverlayDelay = 0.0f;
}

/**
 * Address/Size: 0x80288A3C | size: 0x1A4
 */
void UnidentifiedPresentationState::UpdateAndRenderLetterBox()
{
    if (IsWidescreen())
    {
        return;
    }

    float height = glGetOrthographicHeight();

    static bool letterBoxSizeValid;
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

    if (mLetterBoxDuration <= 0.0f)
    {
        return;
    }

    RLView* view = GetLayerView(eCLV_FrontEnd);
    static const nlColour black = { { 0x00, 0x00, 0x00, 0xFF } };
    nlColour colour = black;
    GLView* previous = g_ShapeRenderer.m_eView;
    g_ShapeRenderer.m_eView = (GLView*)view;
    g_ShapeRenderer.DrawRectangle2D(0.0f, 0.0f, glGetOrthographicWidth(),
        letterBoxSize * mLetterBoxDuration, -2.0f, colour, 0);
    g_ShapeRenderer.DrawRectangle2D(0.0f,
        height - letterBoxSize * mLetterBoxDuration, glGetOrthographicWidth(),
        letterBoxSize * mLetterBoxDuration, -2.0f, colour, 0);
    g_ShapeRenderer.m_eView = previous;
}

/**
 * Address/Size: 0x80288BE0 | size: 0x134
 */
void UnidentifiedPresentationState::Reset()
{
    mIsAllowedToSkip[0] = true;
    mIsAllowedToSkip[1] = true;
    mIsAllowedToSkip[2] = true;
    mIsAllowedToSkip[3] = true;

    Call(sIdleFunction, "");

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
void UnidentifiedPresentationState::UpdateBallGlow(int level)
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
bool UnidentifiedPresentationState::ClearSkipVotes()
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

    mIsAllowedToSkip[0] = !mIsAllowedToSkip[0];
    mIsAllowedToSkip[1] = !mIsAllowedToSkip[1];
    mIsAllowedToSkip[2] = !mIsAllowedToSkip[2];
    mIsAllowedToSkip[3] = !mIsAllowedToSkip[3];
    return true;
}

/**
 * Address/Size: 0x80288EE0 | size: 0x118
 */
void UnidentifiedPresentationState::PlayGoalEffects(const char* effects)
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
                position.x = 20.93f * -1.0f;
            }
            if (end == 1)
            {
                position.y = position.y * -1.0f;
            }
            controller->SetPosition(position);
        }
    }
}
