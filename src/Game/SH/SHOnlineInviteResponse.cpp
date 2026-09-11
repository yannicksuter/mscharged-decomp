#include "Game/SH/SHOnlineInviteResponse.h"
#include "Game/SH/SHOnlineInviteStatus.h"
#include "Game/FE/FEAudio.h"

#include "Game/GameSceneManager.h"
#include "Game/GameInfo.h"
#include "Game/Sys/debug.h"
#include "Game/FE/feFinder.inl"
#include "Game/FE/feInput.h"
#include "Game/FE/fePackage.h"
#include "Game/FE/feScene.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlTextInstance.h"
#include "Game/NetworkSession.h"
#include "Game/NetworkLobby.h"
#include "Game/FriendManager.h"
#include "NL/nlBind.h"
#include "NL/nlBasicString.h"
#include "NL/nlFormat.h"
#include "NL/nlLocalization.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"
#include "Game/FE/feDPD.h"
#include "Game/SH/SHNavigation.h"
#include "Game/SH/SHOnlinePlayerCount.h"
#include "Game/SH/SHOnlineGuestControllerSelect.h"
#include "NL/nlstring_tmpl.h"
#include "Game/FE/UnidentifiedTLDefault.h"

SHOnlineInviteResponse::~SHOnlineInviteResponse()
{
}

void SHOnlineInviteResponse::InitializeButtons()
{
    FEPointerListener::Callback over(Bind<void>(MemFun(&SHOnlineInviteResponse::OnPointerEnter), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback off(Bind<void>(MemFun(&SHOnlineInviteResponse::OnPointerLeave), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback select(Bind<void>(MemFun(&SHOnlineInviteResponse::OnPointerPress), this, Placeholder<0>(), Placeholder<1>()));
    for (int i = 0; i < 4; ++i)
    {
        mButtons[i].SetInstanceBounds(mButtonInstances[i], true, 0.0f, 0.0f, 1.0f, 0.5f);
        mButtons[i].SetPointerEnterCallback(over);
        mButtons[i].SetPointerLeaveCallback(off);
        mButtons[i].SetPointerPressCallback(select);
    }
}

void JoinOnlineFriendInvitation()
{
    int index = g_pFriendManager->mHostInvitationIndex;
    FriendStatusPayload* payload = g_pFriendManager->GetFriendStatusPayload(index);
    GameInfoManager::Instance()->mNoCheatSettings = payload->mGameplaySettings;
    GameInfoManager::Instance()->mRulesA = payload->mPowerupSettings;
    InitializeOnlineMatch(HasOnlineTwoLocalPlayers(), 0, false);
    g_pNetworkSession->GetOnlineLobby()->ConnectToFriendServer(index);
    GameSceneManager::Instance()->Push(SCENE_ONLINE_INVITE_STATUS, SCREEN_FORWARD, true);
    SHOnlineInviteStatus* scene = (SHOnlineInviteStatus*)GameSceneManager::Instance()->GetScene(SCENE_ONLINE_INVITE_STATUS);
    scene->mStatus = 1;
    scene->mReturnDelay = 0.0f;
}

SHOnlineInviteResponse::SHOnlineInviteResponse()
    : mSelectedAction(0)
    , mButtonsInitialized(0)
    , mState(0)
{
    for (int i = 0; i < 4; ++i)
        mButtons[i].mContext = (void*)i;
    for (int i = 0; i < 4; ++i)
    {
        GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
        mHoverCounts[i] = 0;
    }
}

void SHOnlineInviteResponse::SceneCreated()
{
    FEPresentation* presentation = mFEScene->m_pFEPackage->GetPresentation();
    for (int i = 0; i < 4; ++i)
    {
        char name[6];
        nlSNPrintf(name, sizeof(name), "BTN_%d", i + 1);
        mButtonInstances[i] = FEFinder<TLComponentInstance, 4>::Find(presentation->m_currentSlide,
            InlineHasher("Layer"), InlineHasher("INVITATION"), InlineHasher(name));
        if (mButtonInstances[i] == 0)
            mButtonInstances[i] = &UnidentifiedTLComponentDefault::sInstance;
    }
    int index = g_pFriendManager->mHostInvitationIndex;
    const unsigned short* name = GameInfoManager::Instance()->GetSavedFriendName(gNetworkSaveSlotIndex, index);
    typedef BasicString<unsigned short, Detail::TempStringAllocator> WideBasicString;
    WideBasicString string = Format(WideBasicString(g_pLocalization->GetString("ONLINE_INVITATION_INVITATION")), name);
    nlStrNCpy(mInvitationText, string.c_str(), 0x80);
    FEFinder<TLTextInstance, 3>::Find(presentation->m_currentSlide, InlineHasher("Layer"),
        InlineHasher("INVITATION"), InlineHasher("INVITE"))->SetString(mInvitationText);
    g_pFriendManager->SetOwnStatusReceivedInvitation(index);
    SHNavigation* scene = GetNavigationScene();
    if (scene != 0)
        scene->SetButtons(0, true);
    SetOnlineRankedMatch(false);
    FEAudio::PlayAnimAudioEvent(0xBB142B94, 0, 0, 1);
}

void SHOnlineInviteResponse::Update(float fDeltaT)
{
    BaseSceneHandler::Update(fDeltaT);
    if (mState == 0 || mState == 2 || mState == 3)
    {
        TLSlide* slide = mPresentation->m_currentSlide;
        if (slide->GetCurrentTime() < slide->GetStartTime() + slide->GetDuration())
        {
            for (int i = 0; i < 4; ++i)
                GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
            return;
        }
        if (mState == 0)
        {
            InitializeButtons();
            mButtonsInitialized = 1;
            mState = 1;
        }
        else if (mState == 2)
        {
            ApplySelectedAction();
            return;
        }
    }
    if (!g_pFriendManager->ValidateHostInvitation())
    {
        for (int i = 0; i < 4; ++i)
            GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
        GameSceneManager::Instance()->Push(SCENE_ONLINE_INVITE_STATUS, SCREEN_FORWARD, true);
        SHOnlineInviteStatus* scene = (SHOnlineInviteStatus*)GameSceneManager::Instance()->GetScene(SCENE_ONLINE_INVITE_STATUS);
        scene->mStatus = 2;
        scene->mReturnDelay = 2.0f;
        FEAudio::PlayAnimAudioEvent(0xBB142B94, 0, 0, 1);
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
            controller->SetActiveSlide("cursor", true, false);
        }
        u8 valid = true;
        FEPointerEvent event;
        event.mIndex = pad;
        event.mPosition = GetPointerPosition(pad, &valid);
        event.mPressed = g_pFEInput->JustPressed((eFEINPUT_PAD)pad, 0x1E, true, 0);
        event.mReleased = g_pFEInput->JustReleased((eFEINPUT_PAD)pad, 0x1E, true, 0);
        for (int i = 0; i < 4; ++i)
            mButtons[i].HandlePointerEvent(&event);
    }
}

void SHOnlineInviteResponse::OnPointerEnter(int index, void* context)
{
    ++mHoverCounts[index];
    if (!mButtons[(int)context].HasOtherPointerState(1, index))
    {
        mButtons[(int)context].SetPointerState(1, index);
        mButtonInstances[(int)context]->SetActiveSlide("over", true, false);
        FEAudio::PlayAnimAudioEvent(0xDE912775, 0, 0, 1);
    }
}

void SHOnlineInviteResponse::OnPointerLeave(int index, void* context)
{
    --mHoverCounts[index];
    if (!mButtons[(int)context].HasOtherPointerState(1, index))
    {
        mButtons[(int)context].SetPointerState(0, index);
        mButtonInstances[(int)context]->SetActiveSlide("off", true, false);
    }
}

void SHOnlineInviteResponse::OnPointerPress(int, void* context)
{
    for (int i = 0; i < 4; ++i)
        GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
    mSelectedAction = (int)context;
    mState = 2;
    mPresentation->SetActiveSlide("out", true);
    mPresentation->Update(0.0f);
    FEAudio::PlayAnimAudioEvent(0xF0AFD586, 0, 0, 1);
    FEAudio::PlayAnimAudioEvent(0xBB142B94, 0, 0, 1);
}

void SHOnlineInviteResponse::ApplySelectedAction()
{
    switch (mSelectedAction)
    {
    case 0:
        tDebugPrintManager::Print(DC_NETWORK, "Respond invitation Alone\n");
        SetOnlineTwoLocalPlayers(false);
        JoinOnlineFriendInvitation();
        break;
    case 1:
        GameSceneManager::Instance()->Push(SCENE_ONLINE_GUEST_CONTROLLER_SELECT, SCREEN_FORWARD, true);
        ((SHOnlineGuestControllerSelect*)GameSceneManager::Instance()->GetScene(SCENE_ONLINE_GUEST_CONTROLLER_SELECT))->mRespondingToInvitation = true;
        break;
    case 2:
    {
        g_pFriendManager->SetOwnStatusDecline(g_pFriendManager->mHostInvitationIndex);
        GameSceneManager::Instance()->Push(SCENE_ONLINE_INVITE_STATUS, SCREEN_FORWARD, true);
        SHOnlineInviteStatus* scene = (SHOnlineInviteStatus*)GameSceneManager::Instance()->GetScene(SCENE_ONLINE_INVITE_STATUS);
        scene->mStatus = 3;
        scene->mReturnDelay = 0.0f;
        tDebugPrintManager::Print(DC_NETWORK, "Respond invitation Decline\n");
        break;
    }
    case 3:
        tDebugPrintManager::Print(DC_NETWORK, "Preview Invitation\n");
        GameSceneManager::Instance()->Push(SCENE_ONLINE_INVITE_PREVIEW, SCREEN_FORWARD, true);
        break;
    }
}
