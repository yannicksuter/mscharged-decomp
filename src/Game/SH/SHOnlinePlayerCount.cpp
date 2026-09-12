#include "NL/nlSingleton.inl"
#include "Game/SH/SHOnlinePlayerCount.h"
#include "Game/DB/GameProgress.h"
#include "Game/FE/FEAudio.h"
#include "Game/FE/feFinder.h"
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

static int s_nTournamentLowBound = 3;
static int s_nTournamentHiBound = 6;

SHOnlinePlayerCount::SHOnlinePlayerCount(int mode)
    : mBackButton()
    , mButtons()
    , mMode(mode)
    , mButtonsInitialized(false)
    , mState(0)
    , mNextScene(-2)
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

    if (mButtonCount > 0)
    {
        const char* groupName = "Group";
        const char* layerName = "Layer";
        for (int i = 0; i < mButtonCount; ++i)
        {
            char name[6];
            nlSNPrintf(name, sizeof(name), "BTN_%d", i);
            TLComponentInstance* button = FEFinder<TLComponentInstance, 4>::Find(
                    presentation->m_currentSlide, nlStringLowerHash(layerName),
                    nlStringLowerHash(groupName), nlStringLowerHash(name), 0, 0, 0);
            if (button == 0)
            {
                button = &UnidentifiedTLComponentDefault::sInstance;
            }
            mButtonInstances[i] = button;
        }
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

    int state = mState;
    if (state == 0 || (unsigned int)(state - 2) <= 1)
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

        if (state == 0)
        {
            SHNavigation* scene = GetNavigationScene();
            if (scene != 0)
            {
                scene->SetButtons(4, true);
            }
            InitializeButtons();
            mButtonsInitialized = true;
            mState = 1;
        }
        else if (state == 3)
        {
            GameSceneManager::Instance()->Push((SceneList)0x28, SCREEN_NOTHING, true);
            FEAudio::PlayAnimAudioEvent(0x37A9934D, 0, 0, 1);
            return;
        }
        else if (state == 2)
        {
            if (mNextScene == 0x1B)
            {
                FEAudio::PlayAnimAudioEvent(0xC385EFFB, 0, 0, 1);
            }
            GameSceneManager::Instance()->Push((SceneList)mNextScene, SCREEN_NOTHING, true);
            return;
        }
    }

    if (!GameSceneManager::Instance()->IsOnStack((SceneList)0xA)
        && g_pFriendManager->FindHostInvitation())
    {
        int invitationScene = 0x2A;
        if (mMode == 0)
        {
            invitationScene = 0x29;
        }
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
            mState = 3;
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
    FEPointerListener::Callback buttonOver(
        Bind<void>(MemFun(&SHOnlinePlayerCount::OnButtonPointerEnter), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback buttonOff(
        Bind<void>(MemFun(&SHOnlinePlayerCount::OnButtonPointerLeave), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback buttonSelect(
        Bind<void>(MemFun(&SHOnlinePlayerCount::OnButtonPointerPress), this, Placeholder<0>(), Placeholder<1>()));

    for (int i = 0; i < mButtonCount; ++i)
    {
        mButtons[i].SetInstanceBounds(
            mButtonInstances[i], true, 0.0f, 0.0f, 1.0f, 0.5f);
        mButtons[i].SetPointerEnterCallback(buttonOver);
        mButtons[i].SetPointerLeaveCallback(buttonOff);
        mButtons[i].SetPointerPressCallback(buttonSelect);
    }
}

void SHOnlinePlayerCount::OnButtonPointerEnter(int index, void* context)
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

void SHOnlinePlayerCount::OnButtonPointerLeave(int index, void* context)
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

void SHOnlinePlayerCount::OnButtonPointerPress(int, void* context)
{
    FEAudio::PlayAnimAudioEvent(0xF0AFD586, 0, 0, 1);
    mState = 2;

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
    case 0:
        InitializeOnlineMatch(false, false, mMode == 0);
        if (g_pNetworkSession->GetSessionMode() == 2)
        {
            if (mMode == 0)
            {
                mNextScene = 0x31;
            }
            else
            {
                mNextScene = 0x1B;
            }
        }
        else
        {
            GameSceneManager::Instance()->Push((SceneList)0x18, SCREEN_FORWARD, true);
        }
        break;
    case 1:
        InitializeOnlineMatch(true, false, mMode == 0);
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

