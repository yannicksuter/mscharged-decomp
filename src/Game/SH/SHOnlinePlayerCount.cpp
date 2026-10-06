#include "NL/nlSingleton.inl"
#include "NL/nlFunction.inl"
#include "Game/SH/SHOnlinePlayerCount.h"
#include "Game/DB/GameProgress.h"
#include "Game/FE/FEAudio.h"
#include "Game/FE/feInlineHasher.h"
#include "Game/FE/feFinder_impl.h"
#include "Game/FE/feInput.h"
#include "Game/FE/fePackage.h"
#include "Game/FE/feScene.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/feDPD.h"
#include "Game/GameSceneManager.h"
#include "Game/GameInfo.h"
#include "Game/NetworkDraft.h"
#include "Game/NetworkSession.h"
#include "Game/OnlineMatchmaking.h"
#include "Game/SH/SHNavigation.h"
#include "Game/TweakValue.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/FriendManager.h"
#include "NL/nlBind.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"
#include "Game/TweakValue.inl"

static int s_nTournamentLowBound = 3;
static int s_nTournamentHiBound = 6;

SHOnlinePlayerCount::SHOnlinePlayerCount(int mode)
    : mBackButton()
    , mButtons()
    , mMode(mode)
    , mButtonsInitialized(false)
    , mTransitionState(StateEntering)
    , mNextScene(SCENE_INVALID)
{
    mBackButton.SetPopScene(false);

    mButtonCount = 2;
    for (int i = 0; i < mButtonCount; ++i)
    {
        mButtons[i].mContext = (void*)i;
    }

    mPointerInsideCount[0] = 0;
    mPointerInsideCount[1] = 0;
    mPointerInsideCount[2] = 0;
    mPointerInsideCount[3] = 0;
}

SHOnlinePlayerCount::~SHOnlinePlayerCount()
{
}

void SHOnlinePlayerCount::SceneCreated()
{
    FEPresentation* presentation = mFEScene->m_pFEPackage->GetPresentation();

    for (int i = 0; i < 4; ++i)
    {
        GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
    }

    for (int i = 0; i < mButtonCount; ++i)
    {
        char name[6];
        nlSNPrintf(name, sizeof(name), "BTN_%d", i);
        mButtonInstances[i] = FEFinder<TLComponentInstance, 4>::FindOrDefault<TLSlide>(
            presentation->m_currentSlide, "Layer", "Group", name);
    }

    SHNavigation* scene = GetNavigationScene();
    TLComponentInstance* screen = 0;
    if (scene != 0)
    {
        scene->HideButtons();
        screen = scene->GetButton(4);
    }
    mBackButton.SetButtonInstance(screen);

    FEAudio::PlayAnimAudioEvent(0xBB142B94, 0, 0, 1);
}

void SHOnlinePlayerCount::Update(float fDeltaT)
{
    BaseSceneHandler::Update(fDeltaT);

    int state = mTransitionState;
    if (state == StateEntering || (unsigned int)(state - StateForward) <= 1)
    {
        TLSlide* slide = mPresentation->m_currentSlide;
        if (slide->GetCurrentTime() < slide->GetStartTime() + slide->GetDuration())
        {
            for (int pad = 0; pad < 4; ++pad)
            {
                GetPointerInstance(pad)->SetActiveSlide("waiting", true, false);
            }
            return;
        }

        if (state == StateEntering)
        {
            SHNavigation* scene = GetNavigationScene();
            if (scene != 0)
            {
                scene->SetButtons(4, true);
            }
            InitializeButtons();
            mButtonsInitialized = true;
            mTransitionState = StateInteractive;
        }
        else if (state == StateBack)
        {
            GameSceneManager::Instance()->Push(SCENE_ONLINE_MENU, SCREEN_NOTHING, true);
            FEAudio::PlayAnimAudioEvent(0x37A9934D, 0, 0, 1);
            return;
        }
        else if (state == StateForward)
        {
            if (mNextScene == SCENE_GAMEPLAY_OPTIONS)
            {
                FEAudio::PlayAnimAudioEvent(0xC385EFFB, 0, 0, 1);
            }
            GameSceneManager::Instance()->Push((SceneList)mNextScene, SCREEN_NOTHING, true);
            return;
        }
    }

    if (!GameSceneManager::Instance()->IsOnStack(SCENE_POPUP_MENU)
        && g_pFriendManager->FindHostInvitation())
    {
        SceneList invitationScene = (mMode == ModeRanked) ? (SceneList)0x29 : (SceneList)0x2A;
        FriendManager* friendManager = g_pFriendManager;
        friendManager->mReturnScene = invitationScene;
        friendManager->mPreviousRankedMode = 0;
        GameSceneManager::Instance()->Push(SCENE_ONLINE_INVITE_RESPONSE, SCREEN_FORWARD, true);
        return;
    }

    for (int pad = 0; pad < 4; ++pad)
    {
        TLComponentInstance* controller = GetPointerInstance(pad);
        if (g_pFEInput->m_InputLockDepth == 0)
        {
            if ((unsigned int)pad != gFEControllerIndex)
            {
                controller->SetActiveSlide("waiting", true, false);
                continue;
            }

            if (mPointerInsideCount[pad] > 0 || mBackButton.mPointerInside[pad])
            {
                controller->SetActiveSlide("A", true, false);
            }
            else
            {
                controller->SetActiveSlide("cursor", true, false);
            }
        }

        unsigned char valid = 1;
        FEPointerEvent event;
        event.mIndex = pad;
        event.mPosition = GetPointerPosition(pad, &valid);
        event.mPressed
            = g_pFEInput->JustPressed((eFEINPUT_PAD)pad, 0x1E, true, 0);
        event.mReleased
            = g_pFEInput->JustReleased((eFEINPUT_PAD)pad, 0x1E, true, 0);

        for (int i = 0; i < mButtonCount; ++i)
        {
            mButtons[i].HandlePointerEvent(&event);
        }

        if (mBackButton.UpdateBackButton(event, fDeltaT))
        {
            mTransitionState = StateBack;
            SHNavigation* scene = GetNavigationScene();
            if (scene != 0)
            {
                scene->HideButtons();
            }
            mPresentation->SetActiveSlide("out", true);
            mPresentation->Update(0.0f);
            return;
        }
    }
}

void SHOnlinePlayerCount::InitializeButtons()
{
    typedef Detail::MemFunImpl<void, void (SHOnlinePlayerCount::*)(unsigned int, void*)> PointerMethod;
    typedef BindExp3<void, PointerMethod, SHOnlinePlayerCount*, Placeholder<0>, Placeholder<1> > PointerBinding;

    FEPointerListener::Callback buttonOver(
        PointerBinding(MemFun(&SHOnlinePlayerCount::OnButtonPointerEnter), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback buttonOff(
        PointerBinding(MemFun(&SHOnlinePlayerCount::OnButtonPointerLeave), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback buttonSelect(
        PointerBinding(MemFun(&SHOnlinePlayerCount::OnButtonPointerPress), this, Placeholder<0>(), Placeholder<1>()));

    for (int i = 0; i < mButtonCount; ++i)
    {
        mButtons[i].SetInstanceBounds(
            mButtonInstances[i], true, 0.0f, 0.0f, 1.0f, 0.5f);
        mButtons[i].SetPointerEnterCallback(buttonOver);
        mButtons[i].SetPointerLeaveCallback(buttonOff);
        mButtons[i].SetPointerPressCallback(buttonSelect);
    }
}

void SHOnlinePlayerCount::OnButtonPointerEnter(unsigned int index, void* context)
{
    unsigned int item = (unsigned int)context;
    ++mPointerInsideCount[index];
    if (!mButtons[item].HasOtherPointerState(1, index))
    {
        mButtons[item].SetPointerState(1, index);
        mButtonInstances[item]->SetActiveSlide("over", true, false);
        FEAudio::PlayAnimAudioEvent(0xF6EB899E, 0, 0, 1);
    }
}

void SHOnlinePlayerCount::OnButtonPointerLeave(unsigned int index, void* context)
{
    unsigned int item = (unsigned int)context;
    --mPointerInsideCount[index];
    if (!mButtons[item].HasOtherPointerState(1, index))
    {
        mButtons[item].SetPointerState(0, index);
        mButtonInstances[item]->SetActiveSlide("off", true, false);
    }
}

void InitializeOnlineMatch(bool twoLocalPlayers, unsigned char tournament, bool ranked)
{
    if (tournament)
    {
        GameInfoManager::GetInstance()->SetMode(1, true);
    }
    else
    {
        GameInfoManager::GetInstance()->SetMode(0, true);
    }

    SetOnlineTwoLocalPlayers(twoLocalPlayers);
    SetOnlineRankedMatch(ranked);
    g_pNetworkSession->SetTournamentMode(tournament);
    gOnlineTwoLocalPlayers = twoLocalPlayers;

    GameInfoManager::GetInstance()->SetTeam(0, 0);
    GameInfoManager::GetInstance()->SetTeam(1, 0);
    GameInfoManager::GetInstance()->ResetPlayingSides();
    NetworkDraft::Instance()->Reset(false);

    if (!twoLocalPlayers)
    {
        gOnlineLocalControllerIndices[0] = gFEControllerIndex;
        gOnlineLocalControllerIndices[1] = -1;
    }
    else if (g_pNetworkSession->GetSessionMode() == 1)
    {
        gOnlineLocalControllerIndices[0] = 0;
        gOnlineLocalControllerIndices[1] = 1;
    }

    if (tournament)
    {
        gRejectedOpponentProfileIds.mHead = 0;
        gRejectedOpponentProfileIds.mCount = 0;
        gOnlineStartMatchmaking = 1;
        gOnlineSidekickChoiceSent = 0;
        gOnlineUnrankedMatch = 0;
        gOnlineMaxMatchmakingEntries = s_nTournamentHiBound;
        gOnlineMinMatchmakingEntries = s_nTournamentLowBound;
    }
    else if (ranked)
    {
        gRejectedOpponentProfileIds.mHead = 0;
        gRejectedOpponentProfileIds.mCount = 0;
        gOnlineStartMatchmaking = 1;
        gOnlineSidekickChoiceSent = 0;
        gOnlineUnrankedMatch = 0;
        gOnlineMaxMatchmakingEntries = 2;
        gOnlineMinMatchmakingEntries = 0;
    }
    else
    {
        gOnlineStartMatchmaking = 0;
        gOnlineSidekickChoiceSent = 0;
        gOnlineUnrankedMatch = 1;
        gOnlineMaxMatchmakingEntries = 0;
        gOnlineMinMatchmakingEntries = 0;
    }
}

void SHOnlinePlayerCount::OnButtonPointerPress(unsigned int, void* context)
{
    FEAudio::PlayAnimAudioEvent(0xF0AFD586, 0, 0, 1);
    mTransitionState = StateForward;

    SHNavigation* scene = GetNavigationScene();
    if (scene != 0)
    {
        scene->HideButtons();
    }

    mPresentation->SetActiveSlide("out", true);
    mPresentation->Update(0.0f);

    int item = (int)context;
    switch (item)
    {
    case ButtonOneLocalPlayer:
        InitializeOnlineMatch(false, false, mMode == ModeRanked);
        if (g_pNetworkSession->GetSessionMode() == 2)
        {
            if (mMode == ModeRanked)
            {
                mNextScene = 0x31;
            }
            else
            {
                mNextScene = SCENE_GAMEPLAY_OPTIONS;
            }
        }
        else
        {
            GameSceneManager::Instance()->Push((SceneList)0x18, SCREEN_FORWARD, true);
        }
        break;
    case ButtonTwoLocalPlayers:
        InitializeOnlineMatch(true, false, mMode == ModeRanked);
        if (g_pNetworkSession->GetSessionMode() == 2)
        {
            mNextScene = SCENE_ONLINE_GUEST_CONTROLLER_SELECT;
        }
        else
        {
            GameSceneManager::Instance()->Push((SceneList)0x18, SCREEN_FORWARD, true);
        }
        break;
    case 2:
    default:
        break;
    }
}

static TweakIntBinding sTournamentLowBoundTweak(
    "s_nTournamentLowBound", "Network/DWCLobby", &s_nTournamentLowBound, true);
static TweakIntBinding sTournamentHiBoundTweak(
    "s_nTournamentHiBound", "Network/DWCLobby", &s_nTournamentHiBound, true);
