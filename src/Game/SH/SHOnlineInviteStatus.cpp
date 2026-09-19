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
#include "Game/Render/FrontEndPresentation.h"
#include "Game/FriendManager.h"
#include "NL/nlBind.h"
#include "NL/nlFunction.inl"
#include "Game/FE/feDPD.h"
#include "Game/SH/SHNavigation.h"
#include "Game/SH/SHOnlineInvitePlayers.h"
#include "Game/FE/feOnlineError.h"
#include "Game/FE/UnidentifiedTLDefault.h"

SHOnlineInviteStatus::SHOnlineInviteStatus()
    : mStatus(0)
    , mReturnDelay(0.0f)
    , mCanCancel(false)
    , mPopupActive(false)
    , mElapsedTime(0.0f)
    , mPointersInitialized(false)
{
}

SHOnlineInviteStatus::~SHOnlineInviteStatus()
{
}

inline bool SHOnlineInviteStatus::CanCancel()
{
    if (mStatus != 1)
        return false;
    NetworkLobby* lobby = g_pNetworkSession->GetOnlineLobby();
    if (lobby != 0)
    {
        if (lobby->CanCancelMatchmaking())
            return true;
    }
    return false;
}

void SHOnlineInviteStatus::SceneCreated()
{
    FEPresentation* presentation = mFEScene->m_pFEPackage->GetPresentation();
    TLTextInstance* title = FEFinder<TLTextInstance, 3>::FindOrDefault<>(
        mPresentation->m_currentSlide, "Layer", "INVITATION", "TITLE");
    title->SetStringId("ONLINE_INVITATION_TITLE");
    mStatusInstance = FEFinder<TLComponentInstance, 4>::FindOrDefault<>(
        presentation->m_currentSlide, "Layer", "INVITATION", "LOGIN");
    switch (mStatus)
    {
    case 1:
        mStatusInstance->SetActiveSlide("ENTERING LOBBY", false, false);
        break;
    case 2:
    {
        mStatusInstance->SetActiveSlide("DECLINED", false, false);
        TLTextInstance* text = FEFinder<TLTextInstance, 3>::FindOrDefault<>(
            mStatusInstance->GetActiveSlide(), "INVITE");
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
    if (CanCancel())
        mCanCancel = true;
    else
        mCanCancel = false;
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
        GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
}

inline void SHOnlineInviteStatus::ShowConnectionError()
{
    FEPopupMenu* menu;
    int popup = GetOnlineErrorPopup(g_pNetworkSession->mDWCErrorCode, true, 0x5B);
    if (GameSceneManager::Instance()->GetSceneType(GameSceneManager::Instance()->GetCurrentScene()) != (SceneList)0xA)
    {
        menu = (FEPopupMenu*)GameSceneManager::Instance()->Push((SceneList)0xA, SCREEN_NOTHING, false);
        menu->Create((ePopupMenu)popup,
            Function<FnVoidVoid>(Bind<void>(MemFun(&SHOnlineInviteStatus::OnConnectionErrorDismissed), this)));
        mPopupActive = true;
    }
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
            TLTextInstance* text = FEFinder<TLTextInstance, 3>::FindOrDefault<>(
                mStatusInstance->GetActiveSlide(), "INVITE");
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
    if (!mPointersInitialized)
    {
        if (mCanCancel)
        {
            for (int i = 0; i < 4; ++i)
            {
                TLComponentInstance* pointer = GetPointerInstance(i);
                if ((unsigned int)i == gFEControllerIndex)
                    pointer->SetActiveSlide("cursor", true, false);
                else
                    pointer->SetActiveSlide("waiting", true, false);
            }
        }
        mPointersInitialized = true;
    }
    if (mCanCancel)
    {
        if (!CanCancel())
        {
            GetNavigationScene()->SetButtons(0, true);
            mBackButton.Disable();
            mCanCancel = false;
        }
    }
    else
    {
        if (CanCancel())
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
            lobby = g_pNetworkSession->GetOnlineLobby();
            if (lobby != 0 && lobby->CanCancelMatchmaking())
                lobby->CancelMatchmaking();
            return;
        }
    }
    if (mReturnDelay != 0.0f && mElapsedTime >= mReturnDelay)
    {
        if (g_pNetworkSession->RequiresDisconnectAfterError())
        {
            ShowConnectionError();
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
    FrontEndPresentation::GetInstance()->Call("TransitionOnlineMatchToMainMenu");
}
