#include <revolution/net.h>
#include "NL/plat/SocketNetwork.h"
#include "Game/SH/SHOnlineLogin.h"
#include "Game/FE/feOnlineError.h"
#include "Game/FE/FEAudio.h"
#include "Game/Sys/debug.h"

#include "Game/GameSceneManager.h"
#include "Game/FE/feFinder.inl"
#include "Game/FE/feInput.h"
#include "Game/FE/fePackage.h"
#include "Game/FE/fePopupMenu.h"
#include "Game/FE/feScene.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/Render/FrontEndPresentation.h"
#include "Game/FriendManager.h"
#include "NL/nlBind.h"
#include "NL/nlFunction.inl"
#include "NL/nlPrint.h"
#include "Game/SH/SHNavigation.h"
#include <stdlib.h>
#include "Game/FE/feDPD.h"
#include "Game/FE/UnidentifiedTLDefault.h"

SHOnlineLogin::SHOnlineLogin()
    : mPopupActive(false)
    , mState(0)
    , mElapsedTime(0.0f)
    , mSlideCompleteTime(0.0f)
{
}

SHOnlineLogin::~SHOnlineLogin()
{
    g_pNetworkSession->SetLoginListener(0);
}

void SHOnlineLogin::SceneCreated()
{
    FEPresentation* presentation = mFEScene->m_pFEPackage->GetPresentation();
    for (int i = 0; i < 4; ++i)
        GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
    mLoginComponent = FEFinder<TLComponentInstance, 4>::FindOrDefault(
        presentation->m_currentSlide, "Layer", "INVITATION", "LOGIN");
    mLoginComponent->SetActiveSlide("CONNECTING", false, false);
    SHNavigation* scene = GetNavigationScene();
    if (scene != 0)
        scene->SetButtons(0, true);
    g_pNetworkSession->SetLoginListener(this);
    FEAudio::PlayAnimAudioEvent(0x71D9CD2F, "FE_LOGIN", this, true);
}

static void CreateOnlineLoginErrorPopup(FEPopupMenu* menu, ePopupMenu popup, SHOnlineLogin* login);

void SHOnlineLogin::Update(float fDeltaT)
{
    BaseSceneHandler::Update(fDeltaT);
    mElapsedTime += fDeltaT;
    if (mPopupActive && !g_pFEInput->HasInputLock(this))
        return;
    switch (mState)
    {
    case 0:
    {
        TLSlide* slide = mPresentation->m_currentSlide;
        if (mSlideCompleteTime > 0.0f)
        {
            if (mElapsedTime > mSlideCompleteTime + 0.3f)
            {
                SocketNetworkStartupAsync();
                mState = 1;
            }
        }
        else if (slide->GetCurrentTime() >= slide->GetStartTime() + slide->GetDuration())
            mSlideCompleteTime = mElapsedTime;
        break;
    }
    case 1:
        if (SocketNetworkIsStartupComplete())
        {
            if (SocketNetworkIsStarted())
            {
                mState = 3;
                g_pNetworkSession->StartLoginThread();
                mLoginComponent->SetActiveSlide("LOGGING", false, false);
            }
            else
            {
                int popup = GetOnlineErrorPopup(NETGetStartupErrorCode(SocketNetworkGetLastError()), true, 0x78);
                if (GameSceneManager::Instance()->GetSceneType(GameSceneManager::Instance()->GetCurrentScene()) != (SceneList)0xA)
                {
                    FEPopupMenu* menu = (FEPopupMenu*)GameSceneManager::Instance()->Push((SceneList)0xA, SCREEN_NOTHING, false);
                    CreateOnlineLoginErrorPopup(menu, (ePopupMenu)popup, this);
                    mPopupActive = true;
                }
                mState = 7;
                FEAudio::StopAnimAudioEvent(0x71D9CD2F, this);
            }
        }
        break;
    case 3:
        if (g_pNetworkSession->IsLoginThreadComplete())
            mState = 2;
        break;
    case 2:
        if (!g_pNetworkSession->mUnidentified24A4)
        {
            int popup = GetOnlineErrorPopup(g_pNetworkSession->mDWCErrorCode, true, 0x74);
            if (GameSceneManager::Instance()->GetSceneType(GameSceneManager::Instance()->GetCurrentScene()) != (SceneList)0xA)
            {
                FEPopupMenu* menu = (FEPopupMenu*)GameSceneManager::Instance()->Push((SceneList)0xA, SCREEN_NOTHING, false);
                CreateOnlineLoginErrorPopup(menu, (ePopupMenu)popup, this);
                mPopupActive = true;
            }
            mState = 8;
            FEAudio::StopAnimAudioEvent(0x71D9CD2F, this);
        }
        else
            mState = 4;
        break;
    case 6:
        GameSceneManager::Instance()->Push((SceneList)0x28, SCREEN_FORWARD, true);
        FEAudio::StopAnimAudioEvent(0x71D9CD2F, this);
        FEAudio::PlayAnimAudioEvent(0x37A9934D, 0, 0, true);
        break;
    case 4:
    case 5:
    case 7:
    case 8:
    case 9:
        break;
    }
}

static void CreateOnlineLoginErrorPopup(FEPopupMenu* menu, ePopupMenu popup, SHOnlineLogin* login)
{
    menu->Create(popup, Function<FnVoidVoid>(Bind<void>(MemFun(&SHOnlineLogin::OnErrorDismissed), login)));
}

void SHOnlineLogin::OnLoginResult(int result)
{
    if (result == 1)
    {
        int popup = GetOnlineErrorPopup(g_pNetworkSession->mDWCErrorCode, true, 0x74);
        if (GameSceneManager::Instance()->GetSceneType(GameSceneManager::Instance()->GetCurrentScene()) != (SceneList)0xA)
        {
            FEPopupMenu* menu = (FEPopupMenu*)GameSceneManager::Instance()->Push((SceneList)0xA, SCREEN_NOTHING, false);
            CreateOnlineLoginErrorPopup(menu, (ePopupMenu)popup, this);
            mPopupActive = true;
        }
        mState = 8;
        FEAudio::StopAnimAudioEvent(0x71D9CD2F, this);
        return;
    }
    if (result == 2)
    {
        if (GameSceneManager::Instance()->GetSceneType(GameSceneManager::Instance()->GetCurrentScene()) != (SceneList)0xA)
        {
            FEPopupMenu* menu = (FEPopupMenu*)GameSceneManager::Instance()->Push((SceneList)0xA, SCREEN_NOTHING, false);
            CreateOnlineLoginErrorPopup(menu, (ePopupMenu)0x70, this);
            mPopupActive = true;
        }
        mState = 8;
        FEAudio::StopAnimAudioEvent(0x71D9CD2F, this);
        return;
    }
    mLoginComponent->SetActiveSlide("GETTING", false, false);
    if (!g_pNetworkSession->RequestLoginRankings())
    {
        if (GameSceneManager::Instance()->GetSceneType(GameSceneManager::Instance()->GetCurrentScene()) != (SceneList)0xA)
        {
            FEPopupMenu* menu = (FEPopupMenu*)GameSceneManager::Instance()->Push((SceneList)0xA, SCREEN_NOTHING, false);
            CreateOnlineLoginErrorPopup(menu, (ePopupMenu)0x6F, this);
            mPopupActive = true;
        }
        mState = 9;
        FEAudio::StopAnimAudioEvent(0x71D9CD2F, this);
        return;
    }
    g_pFriendManager->SetOwnStatusInitial(1);
    g_pFriendManager->SynchronizeFriends();
    mState = 5;
}

void SHOnlineLogin::OnStatsResult(bool success)
{
    if (!success)
    {
        g_pNetworkSession->ReadAndClearDWCError();
        int popup = GetOnlineErrorPopup(g_pNetworkSession->mDWCErrorCode, true, 0x6F);
        if (GameSceneManager::Instance()->GetSceneType(GameSceneManager::Instance()->GetCurrentScene()) != (SceneList)0xA)
        {
            FEPopupMenu* menu = (FEPopupMenu*)GameSceneManager::Instance()->Push((SceneList)0xA, SCREEN_NOTHING, false);
            CreateOnlineLoginErrorPopup(menu, (ePopupMenu)popup, this);
            mPopupActive = true;
        }
        FEAudio::StopAnimAudioEvent(0x71D9CD2F, this);
        return;
    }
    mLoginComponent->SetActiveSlide("SUCCESS", false, false);
    mState = 6;
}

void SHOnlineLogin::OnErrorDismissed()
{
    mPopupActive = false;
    GameSceneManager::Instance()->Pop();
    FEAudio::PlayAnimAudioEvent(0x4430B152, 0, 0, true);
    FrontEndPresentation::GetInstance()->Call("TransitionOnlineMatchToMainMenu");
}

struct OnlineErrorPopupRange
{
    int mMinCode;
    int mMaxCode;
    int mPopup;
};

static OnlineErrorPopupRange sOnlineErrorPopupRanges[] = {
    {20102, 20108, 0x74},
    {20109, 20109, 0x74},
    {20111, 20999, 0x74},
    {20101, 20101, 0x75},
    {23000, 23999, 0x75},
    {20110, 20110, 0x76},
    {29000, 29000, 0x77},
    {29001, 29001, 0x7C},
    {20100, 20100, 0x78},
    {50100, 50499, 0x78},
    {51000, 51099, 0x78},
    {51300, 51399, 0x78},
    {51400, 51499, 0x78},
    {52000, 52099, 0x78},
    {52100, 52199, 0x78},
    {52200, 52299, 0x78},
    {80430, 80430, 0x79},
    {24000, 24999, 0x7A},
    {25000, 25999, 0x7A},
    {31000, 31999, 0x7A},
    {54000, 54099, 0x7A},
    {60000, 99999, 0x7A},
};

int GetOnlineErrorPopup(int error, bool connected, int value)
{
    error = _abs(error);
    gOnlineErrorCode = error;
    bool found = false;
    for (int i = 0; i < 22; ++i)
    {
        if (error >= sOnlineErrorPopupRanges[i].mMinCode && error <= sOnlineErrorPopupRanges[i].mMaxCode)
        {
            value = sOnlineErrorPopupRanges[i].mPopup;
            found = true;
            break;
        }
    }
    if (value == 0x7A && !connected)
        value = 0x7B;
    if (!found)
        tDebugPrintManager::Print(DC_NETWORK, "Warning failed to find standard error msg for code %d\n", error);
    if (connected)
        g_pNetworkSession->mUnidentified2494 = true;
    return value;
}
