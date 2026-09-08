#include "Game/SH/SHOnlineInviteStatus.h"
#include "Game/FE/FEAudio.h"

#include "Game/GameSceneManager.h"
#include "Game/FE/feFinder.inl"
#include "Game/FE/feInput.h"
#include "Game/FE/fePackage.h"
#include "Game/FE/fePopupMenu.h"
#include "Game/FE/feScene.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlTextInstance.h"
#include "Game/NetworkSession.h"
#include "Game/NetworkLobby.h"
#include "Game/Render/Presentation.h"
#include "Game/FriendManager.h"
#include "NL/nlBind.h"
#include "Game/FE/feDPD.h"
#include "Game/SH/SHNavigation.h"
#include "Game/SH/SHOnlineInvitePlayers.h"
#include "Game/FE/feOnlineError.h"

SHOnlineInviteStatus::SHOnlineInviteStatus()
    : mStatus(0)
    , mReturnDelay(0.0f)
    , mCanCancel(false)
    , mPopupActive(false)
    , mElapsedTime(0.0f)
    , mPointersInitialized(0)
{
}

SHOnlineInviteStatus::~SHOnlineInviteStatus()
{
}

void SHOnlineInviteStatus::SceneCreated()
{
    FEPresentation* presentation = mFEScene->m_pFEPackage->GetPresentation();
    TLTextInstance* title = FEFinder<TLTextInstance, 3>::Find(mPresentation->m_currentSlide,
        InlineHasher("Layer"), InlineHasher("INVITATION"), InlineHasher("TITLE"));
    if (title == 0)
        title = &gDefaultTLTextInstance;
    title->SetStringId("ONLINE_INVITATION_TITLE");
    mStatusInstance = FEFinder<TLComponentInstance, 4>::Find(presentation->m_currentSlide,
        InlineHasher("Layer"), InlineHasher("INVITATION"), InlineHasher("LOGIN"));
    if (mStatusInstance == 0)
        mStatusInstance = &gDefaultTLComponentInstance;
    switch (mStatus)
    {
    case 1:
        mStatusInstance->SetActiveSlide("ENTERING LOBBY", false, false);
        break;
    case 2:
    {
        mStatusInstance->SetActiveSlide("DECLINED", false, false);
        TLTextInstance* text = FEFinder<TLTextInstance, 3>::Find(mStatusInstance->GetActiveSlide(), InlineHasher("INVITE"));
        if (text == 0)
            text = &gDefaultTLTextInstance;
        text->SetStringId("LOC_ONLINE_CANCELED_INVITATION");
        break;
    }
    case 3:
        mStatusInstance->SetActiveSlide("DECLINED", false, false);
        break;
    default:
        mStatusInstance->SetActiveSlide("CONNECTING", false, false);
        break;
    }
    SHNavigation* scene = GetNavigationScene();
    bool canCancel = false;
    if (mStatus == 1)
    {
        NetworkLobby* lobby = g_pNetworkSession->GetOnlineLobby();
        canCancel = lobby != 0 && lobby->CanCancelMatchmaking();
    }
    mCanCancel = canCancel;
    if (mCanCancel)
    {
        mBackButton.SetBackScene(g_pFriendManager->mReturnScene);
        scene->SetButtons(4, true);
    }
    else
        scene->SetButtons(0, true);
    mBackButton.SetButtonInstance(scene->GetButton(4));
    if (mCanCancel)
        mBackButton.Enable();
    else
        mBackButton.Disable();
    for (int i = 0; i < 4; ++i)
        gFEPointerInstances[i]->SetActiveSlide("waiting", true, false);
}

void SHOnlineInviteStatus::Update(float fDeltaT)
{
    BaseSceneHandler::Update(fDeltaT);
    if (mPopupActive && !g_pFEInput->HasInputLock(this))
        return;
    mElapsedTime += fDeltaT;
    NetworkLobby* lobby = g_pNetworkSession->GetOnlineLobby();
    switch (mStatus)
    {
    case 1:
        if (lobby->mMatchFailed || lobby->mCancelRequested)
        {
            lobby->CloseConnectionsAndReset();
            mStatus = 2;
            mReturnDelay = 2.0f;
            mElapsedTime = 0.0f;
            mStatusInstance->SetActiveSlide("DECLINED", false, false);
            TLTextInstance* text = FEFinder<TLTextInstance, 3>::Find(mStatusInstance->GetActiveSlide(), InlineHasher("INVITE"));
            if (text == 0)
                text = &gDefaultTLTextInstance;
            text->SetStringId("LOC_ONLINE_CANCELED_INVITATION");
        }
        else if (gOnlineFourMachineFriendLobby && lobby->AreAllConnectionsReady())
        {
            SHOnlineInvitePlayers* scene = (SHOnlineInvitePlayers*)GameSceneManager::Instance()->Push((SceneList)0x2C, SCREEN_NOTHING, true);
            scene->mIsHost = false;
            scene->mStartFriendServer = true;
        }
        break;
    case 3:
        if (!g_pFriendManager->ValidateHostInvitation() || mElapsedTime >= 30.0f)
        {
            g_pFriendManager->SetOwnStatusInitial(1);
            g_pFriendManager->SetOwnStatusAvailable();
            GameSceneManager::Instance()->Push((SceneList)g_pFriendManager->mReturnScene, SCREEN_BACK, true);
            FEAudio::PlayAnimAudioEvent(0x37A9934D, 0, 0, true);
        }
        break;
    }
    if (mPointersInitialized == 0)
    {
        if (mCanCancel)
        {
            for (int i = 0; i < 4; ++i)
            {
                if ((unsigned int)i == gFEControllerIndex)
                    gFEPointerInstances[i]->SetActiveSlide("cursor", true, false);
                else
                    gFEPointerInstances[i]->SetActiveSlide("waiting", true, false);
            }
        }
        mPointersInitialized = 1;
    }
    if (mCanCancel)
    {
        bool canCancel = false;
        if (mStatus == 1)
        {
            NetworkLobby* lobby = g_pNetworkSession->GetOnlineLobby();
            canCancel = lobby != 0 && lobby->CanCancelMatchmaking();
        }
        if (!canCancel)
        {
            GetNavigationScene()->SetButtons(0, true);
            mBackButton.Disable();
            mCanCancel = false;
        }
    }
    else
    {
        bool canCancel = false;
        if (mStatus == 1)
        {
            NetworkLobby* lobby = g_pNetworkSession->GetOnlineLobby();
            canCancel = lobby != 0 && lobby->CanCancelMatchmaking();
        }
        if (canCancel)
        {
            GetNavigationScene()->SetButtons(4, true);
            mBackButton.Enable();
            mCanCancel = true;
        }
    }
    for (int i = 0; i < 4; ++i)
    {
        u8 valid = true;
        FEPointerEvent event;
        event.mIndex = i;
        event.mPosition = GetPointerPosition(i, &valid);
        event.mPressed = g_pFEInput->JustPressed((eFEINPUT_PAD)i, 0x1E, true, 0);
        event.mReleased = g_pFEInput->JustReleased((eFEINPUT_PAD)i, 0x1E, true, 0);
        if (mBackButton.UpdateBackButton(event, fDeltaT))
        {
            g_pFriendManager->SetOwnStatusAvailable();
            NetworkLobby* lobby = g_pNetworkSession->GetOnlineLobby();
            if (lobby != 0 && lobby->CanCancelMatchmaking())
                lobby->CancelMatchmaking();
            return;
        }
    }
    if (mReturnDelay != 0.0f && mElapsedTime >= mReturnDelay)
    {
        if (g_pNetworkSession->RequiresDisconnectAfterError())
        {
            int popup = GetOnlineErrorPopup(g_pNetworkSession->mDWCErrorCode, true, 0x5B);
            if (GameSceneManager::Instance()->GetSceneType(GameSceneManager::Instance()->GetCurrentScene()) != (SceneList)0xA)
            {
                FEPopupMenu* menu = (FEPopupMenu*)GameSceneManager::Instance()->Push((SceneList)0xA, SCREEN_NOTHING, false);
                menu->Create((ePopupMenu)popup, Function<FnVoidVoid>(Bind<void>(MemFun(&SHOnlineInviteStatus::OnConnectionErrorDismissed), this)));
                mPopupActive = true;
            }
        }
        else
        {
            g_pFriendManager->SetOwnStatusAvailable();
            GameSceneManager::Instance()->Push((SceneList)g_pFriendManager->mReturnScene, SCREEN_BACK, true);
            FEAudio::PlayAnimAudioEvent(0x37A9934D, 0, 0, true);
        }
    }
}

void SHOnlineInviteStatus::OnConnectionErrorDismissed()
{
    mPopupActive = false;
    GameSceneManager::Instance()->Pop();
    FEAudio::PlayAnimAudioEvent(0x4430B152, 0, 0, true);
    Presentation::GetInstance()->Call("TransitionOnlineMatchToMainMenu");
}
