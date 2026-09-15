#include "NL/plat/PlatPadManager.h"
#include "Game/HBMManager.h"
#include "Game/FE/feManager.h"
#include "Game/Sys/debug.h"

#include <math.h>

#include "Game/AI/AIPad.h"
#include "Game/Audio/RegistryPools.h"
#include "Game/Ball.h"
#include "Game/BaseGameSceneManager.h"
#include "Game/Camera/CameraMan.h"
#include "Game/Camera/animcam.h"
#include "Game/Event.h"
#include "Game/FE/feInput.h"
#include "Game/FE/feMusic.h"
#include "Game/FE/fePopupMenu.h"
#include "Game/FE/feSceneManager.h"
#include "Game/Game.h"
#include "Game/GameInfo.h"
#include "Game/MathHelpers.h"
#include "Game/NetworkSession.h"
#include "Game/Render/ShootToScoreArrow.h"
#include "Game/SH/SHPause.h"
#include "Game/Sys/audio.h"
#include "Game/TweakRegistry.h"
#include "Game/TweakValue.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/main.h"

#include "NL/glx/glxSwap.h"
#include "NL/globalpad.h"
#include "NL/nlAVLTree.h"
#include "NL/nlConfig.h"
#include "NL/nlFunction.h"
#include "NL/nlPrint.h"
#include "NL/nlTask.h"
#include "NL/plat/PlatPadManager.h"
#include "Game/RumbleActions.h"
#include "Game/FE/feDPD.h"
#include "Game/SH/SHNavigation.h"
#include "unclassified/tu_80284A58.h"
#include "Game/InputManager.h"
#include "Game/OverlayManager.h"

typedef nlAVLTree<unsigned int, UnidentifiedEventBase*,
    DefaultKeyCompare<unsigned int> >
    UnidentifiedEventRegistry;

extern "C"
{
    void RestoreWorldRendering(UnidentifiedPresentationState* presentation);
    bool DuringEndOfGamePresentation(UnidentifiedPresentationState* presentation);
    void GoalieOnGameOver();
    void GetMaxRemoteAccelDelta(cAIPad* pad, int index, nlVector3* out);

    extern UnidentifiedEventRegistry* g_pEventRegistry;
    extern float g_AllActorsHidden;
}

cAnimCamera* FrontEnd::m_pPauseMenuCamera = 0;
bool FrontEnd::m_bGameOver = false;
bool FrontEnd::m_bInPauseMenuState = false;
float FrontEnd::m_fDemoTimeElapsed = 0.0f;
unsigned char FrontEnd::m_ctrlConnectedState[4];
float FrontEnd::m_pauseDelay = 0.0f;

static unsigned char AlreadyStartedStrikers101Menu;
unsigned char g_JaapAndJacksNastyHackBecauseWeDoNotKnowDifferenceBetweenPausePauseAndPostGamePause;

eFEState FrontEnd::m_feStateCurrent = eFE_INVALID;
eFEState FrontEnd::m_feStatePending = eFE_INVALID;
eFEState FrontEnd::m_feStatePrevious = eFE_INVALID;
unsigned int FrontEnd::m_lastTaskState = -1;
eFEINPUT_PAD FrontEnd::m_hitStartPad = FE_ALL_PADS;
FrontEnd::MenuEnterType FrontEnd::m_menuType = MET_INVALID;

static unsigned char DontCheckForControllerRemovalHack = 1;

bool FrontEnd::Initialize()
{
    Reset();
    return true;
}

void FrontEnd::Destroy()
{
    m_feStateCurrent = eFE_INVALID;
    m_feStatePending = eFE_INVALID;
}

void FrontEnd::Reset()
{
    m_feStateCurrent = eFE_INVALID;
    m_feStatePending = eFE_INVALID;
    m_pPauseMenuCamera = 0;
    m_hitStartPad = FE_ALL_PADS;
    m_menuType = MET_INVALID;
    m_bGameOver = false;
    m_bInPauseMenuState = false;
    m_fDemoTimeElapsed = 0.0f;
    m_pauseDelay = 0.25f;
    AlreadyStartedStrikers101Menu = 0;
    DontCheckForControllerRemovalHack = 0;

    {
        Function<FnVoidVoid> callback(OnGetReadyForKickoff);
        UnidentifiedEventBase** foundEvent;
        unsigned int hash;
        hash = HashEventName("GetReadyForKickoff", -1);
        foundEvent = 0;
        g_pEventRegistry->Find(hash, &foundEvent, 0);
        UnidentifiedEventBase* event = foundEvent != 0 ? *foundEvent : 0;
        ((UnidentifiedTypedEvent<UnidentifiedEventNoData>*)event)
            ->Add(callback, 0, -1);
    }

    {
        Function<FnVoidVoid> callback(OnPresentationBypass);
        UnidentifiedEventBase** foundEvent;
        unsigned int hash;
        hash = HashEventName("PresentationBypass", -1);
        foundEvent = 0;
        g_pEventRegistry->Find(hash, &foundEvent, 0);
        UnidentifiedEventBase* event = foundEvent != 0 ? *foundEvent : 0;
        ((UnidentifiedTypedEvent<UnidentifiedEventNoData>*)event)
            ->Add(callback, 0, -1);
    }

    {
        Function<FnVoidVoid> callback(OnGameOver);
        UnidentifiedEventBase** foundEvent;
        unsigned int hash;
        hash = HashEventName("GameOver", -1);
        foundEvent = 0;
        g_pEventRegistry->Find(hash, &foundEvent, 0);
        UnidentifiedEventBase* event = foundEvent != 0 ? *foundEvent : 0;
        ((UnidentifiedTypedEvent<UnidentifiedEventNoData>*)event)
            ->Add(callback, 0, -1);
    }
}

static const float sPauseCameraAnimationSpeed[1] = { 0.3f };

void FrontEnd::SetControllerState()
{
    for (int i = 0; i < 4; i++)
    {
        m_ctrlConnectedState[i] = g_pFEInput->IsConnected((eFEINPUT_PAD)i);
    }
}

void FrontEnd::EnterStartScreen(bool bStraightToKickoff)
{
    bool isInStrikers101 = false;
    if (nlSingleton<GameInfoManager>::Instance()->mIsInStrikers101Mode
        || nlSingleton<GameInfoManager>::Instance()->IsInMode4())
    {
        isInStrikers101 = true;
    }
    AlreadyStartedStrikers101Menu = 0;
    g_pGame->BeginGame(false, isInStrikers101);
    m_feStatePending = eFE_WAIT_FOR_LOAD;
}

void FrontEnd::ExitWinnerScreen()
{
    cCameraManager::PopCamera();
    delete m_pPauseMenuCamera;
    m_pPauseMenuCamera = 0;
    if (!nlSingleton<GameInfoManager>::Instance()->mIsInStrikers101Mode
        && !nlSingleton<GameInfoManager>::Instance()->IsInMode4())
    {
        g_AllActorsHidden = 0.5f;
    }
    g_JaapAndJacksNastyHackBecauseWeDoNotKnowDifferenceBetweenPausePauseAndPostGamePause
        = 1;
    DontCheckForControllerRemovalHack = 0;
    nlTaskManager::SetNextState(2);
}

void FrontEnd::EnterMenuState(FrontEnd::MenuEnterType menuType)
{
    int i;
    cGlobalPad* globalPad;
    nlTaskManager* taskManager;

    m_bInPauseMenuState = true;
    m_menuType = menuType;
    m_feStatePrevious = m_feStateCurrent;
    taskManager = nlTaskManager::m_pInstance;
    unsigned int lastTaskState = taskManager->mCurrentState;
    if (lastTaskState != taskManager->mPendingState)
    {
        lastTaskState = taskManager->mPendingState;
    }
    m_lastTaskState = lastTaskState;
    PauseAllAudio();
    FEMusic::StartStreamIfDifferent(3);
    SetPointerEnabled(true);
    for (i = 0; i < 4; i++)
    {
        globalPad = g_pFEInput->GetGlobalPad((eFEINPUT_PAD)i);
        if (globalPad != 0)
        {
            StopRumbleAction(globalPad);
        }
    }
    nlTaskManager::SetNextState(1);
    if (g_pOverlayManager->IsOnStack(SCENE_SUPER_LOADING))
    {
        g_pOverlayManager->Pop();
        nlSingleton<FESceneManager>::Instance()->ForceImmediateStackProcessing();
    }
    switch (m_menuType)
    {
    case MET_PAUSE:
        if (nlSingleton<GameInfoManager>::Instance()->mIsInStrikers101Mode)
        {
            g_pOverlayManager->Push((SceneList)0x54, SCREEN_NOTHING, false);
        }
        else if (nlSingleton<GameInfoManager>::Instance()->IsInMode4()
                 && !AlreadyStartedStrikers101Menu)
        {
            g_pOverlayManager->Push((SceneList)0x67, SCREEN_FORWARD, false);
        }
        else
        {
            g_pOverlayManager->Push((SceneList)0x50, SCREEN_NOTHING, false);
        }
        PauseMenuScene::mControllingInput = FE_ALL_PADS;
        break;

    case MET_CHOOSESIDES:
        g_pOverlayManager->Push((SceneList)0x51, SCREEN_NOTHING, false);
        PauseMenuScene::mControllingInput = FE_ALL_PADS;
        break;

    case MET_CONNECTIONLOST:
    case MET_SYNCERROR:
    case MET_QUEUEOVERFLOW:
    {
        ePopupMenu popupType;
        FEPopupMenu* popup = (FEPopupMenu*)g_pOverlayManager->Push(
            (SceneList)0xA, SCREEN_NOTHING, false);

        switch (m_menuType)
        {
        case MET_CONNECTIONLOST:
            popupType = (ePopupMenu)0x60;
            break;
        case MET_SYNCERROR:
            popupType = (ePopupMenu)0x63;
            break;
        case MET_QUEUEOVERFLOW:
            popupType = (ePopupMenu)0x64;
            break;
        default:
            break;
        }
        popup->Create(popupType, Function<FnVoidVoid>(PopupNetworkErrorOverlayCallback));
        WorldDarkening::Instance().Fade(100.0f, 1.0f);
        break;
    }

    case MET_END:
    default:
        break;
    }
    m_feStatePending = eFE_PROCESS_MENU_INPUT;
    g_pGame->SendPauseGameEvent();
}

void FrontEnd::PopupNetworkErrorOverlayCallback()
{
    tDebugPrintManager::Print(DC_NETWORK, "Popup Network Error Overlay Callback\n");
    if (g_pGame->mbCaptainShotToScoreOn)
    {
        GoalieOnGameOver();
        RestoreWorldRendering(GetPresentation());
    }
    ReturnToFE();
}

void FrontEnd::ExitMenuState()
{
    if (!nlSingleton<FESceneManager>::Instance()->AreAllScenesValid())
    {
        return;
    }

    m_bInPauseMenuState = false;
    SetPointerEnabled(false);
    m_menuType = MET_INVALID;
    m_feStatePending = m_feStatePrevious;
    nlTaskManager::m_pInstance->mLocked = false;
    nlTaskManager::SetNextState(m_lastTaskState);
    g_pOverlayManager->Pop();
    g_pGame->SendResumingGameEvent();
    m_pauseDelay = 0.25f;
    FEMusic::StopStream();
    ResumeAllAudio();
}

void FrontEnd::Update(float fTimeDelta)
{
    if (g_pOverlayManager == 0)
    {
        return;
    }

    m_pauseDelay -= fTimeDelta;
    if (m_pauseDelay < 0.0f)
    {
        m_pauseDelay = 0.0f;
    }

    if (nlSingleton<GameInfoManager>::Instance()->IsInMode2())
    {
        UpdateForDemoMode(fTimeDelta);
    }
    else
    {
        UpdateForGame(fTimeDelta);
    }

    static_cast<OverlayManager*>(g_pOverlayManager)->Update(fTimeDelta);
    m_feStateCurrent = m_feStatePending;

    switch (m_feStateCurrent)
    {
    case eFE_WAIT_FOR_LOAD:
    case eFE_INGAME:
        break;

    case eFE_PRE_GAME_START:
        g_pBall->m_bVisible = true;
        if ((nlSingleton<GameInfoManager>::Instance()->mIsInStrikers101Mode
                || nlSingleton<GameInfoManager>::Instance()->IsInMode4())
            && !AlreadyStartedStrikers101Menu)
        {
            bool bPauseDelayElapsed = m_pauseDelay <= 0.0f;
            if (bPauseDelayElapsed)
            {
                EnterMenuState(MET_PAUSE);
                m_lastTaskState = 2;
                m_feStatePrevious = eFE_INGAME;
                AlreadyStartedStrikers101Menu = 1;
            }
        }
        else
        {
            m_feStatePending = eFE_INGAME;
        }
        m_bGameOver = false;
        break;

    case eFE_END_GAME:
        nlTaskManager::SetNextState(1);
        g_pBall->m_bVisible = false;
        m_pPauseMenuCamera
            = new (nlMalloc(sizeof(cAnimCamera), 8, false)) cAnimCamera();
        m_pPauseMenuCamera->SelectCameraAnimation("pause");
        cCameraManager::PushCamera(m_pPauseMenuCamera);
        m_pPauseMenuCamera->m_fAnimationSpeed = sPauseCameraAnimationSpeed[0];
        m_feStatePending = eFE_WAIT_USER_END_GAME_INPUT;
        break;

    case eFE_PROCESS_MENU_INPUT:
        if (nlTaskManager::m_pInstance->mCurrentState == 1)
        {
            nlTaskManager::m_pInstance->mLocked = true;
        }
        break;

    default:
        break;
    }
}

void FrontEnd::UpdateForDemoMode(float fDeltaT)
{
    m_fDemoTimeElapsed += fDeltaT;
    if (!(m_fDemoTimeElapsed < 3.0f))
    {
        static_cast<OverlayManager*>(g_pOverlayManager)->ShowDemoSlide();
        if (g_pFEInput->JustPressed(FE_ALL_PADS, 0x3F, true, 0))
        {
            ReturnToFE();
            m_fDemoTimeElapsed = 0.0f;
        }
        else
        {
            if (g_e3_Build || GetTweakBool("/user/dosoak", false))
            {
                return;
            }
            {
                static float maxBackendDemoTime;
                static signed char init;

                if (!init)
                {
                    maxBackendDemoTime = GetConfigFloat(
                        Config::Global(), "be_demo_mode_time_out", 60.0f);
                    init = 1;
                }

                if (m_fDemoTimeElapsed >= maxBackendDemoTime)
                {
                    ReturnToFE();
                    m_fDemoTimeElapsed = 0.0f;
                }
            }
        }
    }
}

static TweakValueBool g_bAllowNetworkPausing(
    "g_bAllowNetworkPausing", "Network", false);

void FrontEnd::UpdateForGame(float fDeltaT)
{
    if (m_bGameOver)
    {
        return;
    }

    if (g_pNetworkSessionBase->GetNumMachines() > 1)
    {
        if (!m_bInPauseMenuState)
        {
            bool bPauseDelayElapsed = m_pauseDelay <= 0.0f;
            if (bPauseDelayElapsed
                && nlTaskManager::m_pInstance->mCurrentState != 1
                && nlTaskManager::m_pInstance->mPendingState
                       == nlTaskManager::m_pInstance->mCurrentState)
            {
                if (g_pNetworkSession->mPoppedOverlay != 3)
                {
                    switch (g_pNetworkSession->mPoppedOverlay)
                    {
                    case 0:
                        EnterMenuState(MET_CONNECTIONLOST);
                        break;
                    case 1:
                        EnterMenuState(MET_SYNCERROR);
                        break;
                    case 2:
                        EnterMenuState(MET_QUEUEOVERFLOW);
                        break;
                    default:
                        break;
                    }
                }
            }
        }

        if (!g_bAllowNetworkPausing)
        {
            return;
        }
    }

    if (!m_bInPauseMenuState)
    {
        bool bPauseDelayElapsed = m_pauseDelay <= 0.0f;
        if (bPauseDelayElapsed && !g_pGame->mbCaptainShotToScoreOn
            && nlTaskManager::m_pInstance->mCurrentState != 1
            && (nlTaskManager::m_pInstance->mCurrentState & 0x18) == 0
            && nlTaskManager::m_pInstance->mPendingState
                   == nlTaskManager::m_pInstance->mCurrentState
            && IsIdleAndNoShotInProgress(GetPresentation()))
        {
            int i;
            for (i = 0; i < 4; i++)
            {
                if (g_pFEInput->JustPressed((eFEINPUT_PAD)i, 0x2F, true, 0))
                {
                    bool moving;
                    if (g_pPlatPadManager->type[i] == 2)
                    {
                        nlVector3 stick;
                        GetMaxRemoteAccelDelta(GetAIPad(i), 4, &stick);
                        float absZ = (float)fabs(stick.z);
                        float absY = (float)fabs(stick.y);
                        float largest = nlMaxEquals(absY, absZ);
                        largest = nlMaxEquals((float)fabs(stick.x), largest);
                        if (largest > 0.15f)
                        {
                            moving = true;
                        }
                        else
                        {
                            moving = false;
                        }
                    }
                    else
                    {
                        moving = false;
                    }
                    if (!moving)
                    {
                        m_hitStartPad = (eFEINPUT_PAD)i;
                        EnterMenuState(MET_PAUSE);
                    }
                }
                if (m_bInPauseMenuState)
                {
                    break;
                }
            }
        }
    }

    if (DuringEndOfGamePresentation(GetPresentation()))
    {
        DontCheckForControllerRemovalHack = 1;
    }

    if (DontCheckForControllerRemovalHack)
    {
        return;
    }

    if (IsNetworkOrRecordedGame())
    {
        return;
    }

    if (m_bInPauseMenuState)
    {
        return;
    }

    bool bPauseDelayElapsed = m_pauseDelay <= 0.0f;
    if (!bPauseDelayElapsed)
    {
        return;
    }

    if (g_pGame->mbCaptainShotToScoreOn)
    {
        return;
    }

    if (nlTaskManager::m_pInstance->mCurrentState == 1)
    {
        return;
    }

    if ((nlTaskManager::m_pInstance->mCurrentState & 0x18) != 0)
    {
        return;
    }

    if (nlTaskManager::m_pInstance->mPendingState
        != nlTaskManager::m_pInstance->mCurrentState)
    {
        return;
    }

    if (nlSingleton<GameInfoManager>::Instance()->IsInMode4()
        && !AlreadyStartedStrikers101Menu)
    {
        return;
    }

    if (!IsIdleAndNoShotInProgress(GetPresentation()))
    {
        return;
    }

    bool curConnected;
    int i;
    for (i = 0; i < 4; i++)
    {
        curConnected = g_pFEInput->IsConnected((eFEINPUT_PAD)i) && IsFreeStylePad(i);
        if (m_ctrlConnectedState[i] == 1 && !curConnected)
        {
            if (nlSingleton<GameInfoManager>::Instance()->GetPlayingSide(
                    (unsigned short)i)
                != -1)
            {
                EnterMenuState(MET_CHOOSESIDES);
            }
        }
        m_ctrlConnectedState[i] = curConnected;
        if (m_bInPauseMenuState)
        {
            break;
        }
    }
}

void FrontEnd::ReturnToFE()
{
    gpHBMManager->mBlocked = true;
    glxSwapSetBlack(true);
    if (nlTaskManager::m_pInstance->mCurrentState == 1)
    {
        nlTaskManager::m_pInstance->mLocked = false;
        m_feStatePending = eFE_INVALID;
    }
    for (int i = 0; i < 4; i++)
    {
        g_pPadManager->GetPad(i)->StopRumble();
    }
    nlTaskManager::SetNextState(0x400000);
}

void FrontEnd::OnGameOver()
{
    m_bGameOver = true;
    m_feStatePending = eFE_END_GAME;
}

void FrontEnd::OnGetReadyForKickoff()
{
    m_feStatePending = eFE_PRE_GAME_START;
}

void FrontEnd::OnPresentationBypass()
{
    if (g_pOverlayManager != 0 && g_pOverlayManager->IsOnStack(SCENE_SUPER_LOADING))
    {
        g_pOverlayManager->Pop();
        nlSingleton<FESceneManager>::Instance()->ForceImmediateStackProcessing();
    }
}
