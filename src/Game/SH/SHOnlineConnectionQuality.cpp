#include "Game/GameInfo.h"
#include "Game/FE/fePresentation.inl"
#include "Game/SH/SHOnlineConnectionQuality.h"
#include "Game/FE/FEAudio.h"
#include "Game/Sys/debug.h"

const char* sConnectionDecisionComponentNames[2] = { "ACCEPT", "REJECT" };

#include "Game/GameSceneManager.h"
#include "Game/NetworkMessages.h"
#include "Game/NetworkSession.h"
#include "Game/NetworkLobby.h"
#include "Game/FriendManager.h"
#include "Game/FE/feInlineHasher.h"
#include "Game/FE/feFinder_impl.h"
#include "Game/FE/feInput.h"
#include "Game/FE/fePointer.inl"
#include "Game/FE/fePopupMenu.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlTextInstance.h"
#include "NL/nlBind.h"
#include "NL/nlFunction.inl"
#include "NL/nlFormat.h"
#include "NL/nlLocalizationLookup.h"
#include "NL/nlString.h"
#include "Game/FE/feDPD.h"
#include "Game/OnlineMatchmaking.h"
#include "Game/SH/SHNavigation.h"
#include "Game/SH/SHOnlineInvitePlayers.h"
#include "NL/plat/TransportConnection.h"
#include "NL/nlstring_tmpl.h"
#include "Game/FE/FEAudio.h"
#include "Game/FE/UnidentifiedTLDefault.h"

static inline void UpdateConnectionQualityTimerText(
    OnlineConnectionQualityScene* scene, TLTextInstance* timer)
{
    typedef BasicString<unsigned short, Detail::TempStringAllocator> WideBasicString;
    timer->SetString(nlStrNCpy(scene->mUnidentified044,
        Format(WideBasicString(LookupLocString("ONLINE_CONNECTION_QUALITY_TIME")),
            scene->mUnidentified180).c_str(), 128));
}

OnlineConnectionQualityScene::OnlineConnectionQualityScene()
    : mUnidentified030(false)
    , mUnidentified031(false)
    , mUnidentified034(2)
    , mUnidentified038(2)
    , mUnidentified144(1.0f,
          Function<FETimer*>(
              Bind<void>(MemFun(&OnlineConnectionQualityScene::OnCountdownTick), this, Placeholder<0>())))
    , mUnidentified160(1.0f,
          Function<FETimer*>(
              Bind<void>(MemFun(&OnlineConnectionQualityScene::OnReturnTimer), this, Placeholder<0>())))
    , mUnidentified17C(false)
    , mUnidentified180(30)
    , mUnidentified300(false)
{
    mUnidentified020[0] = 0;
    mUnidentified020[1] = 0;
    mUnidentified020[2] = 0;
    mUnidentified020[3] = 0;
    mUnidentified18C[0].mContext = (void*)0;
    mUnidentified18C[0].mIgnoreInputLock = true;
    mUnidentified18C[1].mContext = (void*)1;
    mUnidentified18C[1].mIgnoreInputLock = true;
    mUnidentified03C[0] = 2;
    mUnidentified184[0] = 0;
    mUnidentified03C[1] = 2;
    mUnidentified184[1] = 0;
    mUnidentified160.SetEnabled(false);
}

OnlineConnectionQualityScene::~OnlineConnectionQualityScene()
{
    SHNavigation* scene = GetNavigationScene();
    if (scene != 0)
    {
        scene->RestoreButtonVisibility();
    }
}

void OnlineConnectionQualityScene::OnCheckConnection(NetMessageCheckConnection* message)
{
    mUnidentified184[0] = message->mProfileIds[0];
    mUnidentified184[1] = message->mProfileIds[1];
}

void OnlineConnectionQualityScene::OnCountdownTick(FETimer* timer)
{
    mUnidentified17C = true;
    if (mUnidentified180 > 0)
    {
        --mUnidentified180;
        if (mUnidentified180 <= 5 && mUnidentified180 > 0)
        {
            if (mUnidentified180 == 1)
            {
                FEAudio::PlayAnimAudioEvent(0x09AA8790, 0, 0, true);
            }
            else
            {
                FEAudio::PlayAnimAudioEvent(0xFF48403F, 0, 0, true);
            }
        }
    }
}

void OnlineConnectionQualityScene::OnReturnTimer(FETimer* timer)
{
    CloseConnectionsAndReturn();
}

static inline void ShowWaitingForDecisions(OnlineConnectionQualityScene* scene)
{
    for (int i = 0; i < 2; ++i)
    {
        scene->mUnidentified2F4[i]->m_bVisible = false;
        scene->mUnidentified18C[i].Disable();
    }

    TLComponentInstance* component = FEFinder<TLComponentInstance, 4>::FindOrDefault(scene->mPresentation->m_currentSlide, "Layer", "WAITING");
    component->m_bVisible = true;
}

void OnlineConnectionQualityScene::SceneCreated()
{
    for (int i = 0; i < 2; ++i)
    {
        TLComponentInstance* component = FEFinder<TLComponentInstance, 4>::FindOrDefault(mPresentation->m_currentSlide, "Layer", sConnectionDecisionComponentNames[i]);
        mUnidentified2F4[i] = component;
    }

    FEFinder<TLComponentInstance, 4>::FindOrDefault(mPresentation->m_currentSlide, "Layer", "QUALITY");

    TLComponentInstance* component = FEFinder<TLComponentInstance, 4>::FindOrDefault(mPresentation->m_currentSlide, "Layer", "WAITING");
    component->m_bVisible = false;

    TLTextInstance* timer = FEFinder<TLTextInstance, 3>::Find(mPresentation->m_currentSlide, "Layer", "TIMER");
    UpdateConnectionQualityTimerText(this, timer);

    UpdateConnectionQuality();
    SHNavigation* scene = GetNavigationScene();
    if (scene != 0)
    {
        scene->SetButtons(0, true);
    }
    for (int i = 0; i < 4; ++i)
    {
        GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
    }
    FEAudio::PlayAnimAudioEvent(0xBB142B94, 0, 0, true);
}

void OnlineConnectionQualityScene::UpdateConnectionQuality()
{
    NetworkMachineRoster* roster = g_pNetworkSessionBase->GetMachineRoster();
    unsigned int value = 0;
    for (int i = 0; i < roster->GetMachineCount(); ++i)
    {
        TransportConnection* connection
            = (TransportConnection*)roster->GetMachineAid(i);
        if (connection != 0 && connection != (TransportConnection*)-1)
        {
            value = value >= connection->mRoundTripTimeMS ? value : connection->mRoundTripTimeMS;
        }
    }

    TLComponentInstance* component = FEFinder<TLComponentInstance, 4>::FindOrDefault(mPresentation->m_currentSlide, "Layer", "QUALITY", "RATING", "stars");
    mUnidentified2FC = component;
    unsigned int latency = value >> 1;
    if (latency > 200)
    {
        component->SetActiveSlide("1", true, false);
    }
    else if (latency > 160)
    {
        component->SetActiveSlide("2", true, false);
    }
    else if (latency > 80)
    {
        component->SetActiveSlide("3", true, false);
    }
    else if (latency != 0)
    {
        component->SetActiveSlide("4", true, false);
    }
    else
    {
        component->SetActiveSlide("0", true, false);
    }
}

static inline bool IsAnyConnectionRejected(OnlineConnectionQualityScene* scene)
{
    NetworkMachineRoster* roster = g_pNetworkSessionBase->GetMachineRoster();
    for (int i = 0; i < roster->GetMachineCount(); ++i)
    {
        if (scene->mUnidentified03C[i] == 0)
        {
            return true;
        }
    }
    return false;
}

static inline bool AreAllConnectionsAccepted(OnlineConnectionQualityScene* scene)
{
    NetworkMachineRoster* roster = g_pNetworkSessionBase->GetMachineRoster();
    for (int i = 0; i < roster->GetMachineCount(); ++i)
    {
        if (scene->mUnidentified03C[i] != 1)
        {
            return false;
        }
    }
    return true;
}

static inline void SendLobbyDraft()
{
    NetworkLobby* lobby = g_pNetworkSession->GetOnlineLobby();
    bool unranked = !IsOnlineRankedMatch();
    g_pNetworkSession->SendDraftToEveryone(lobby->GetPlayerCount(), lobby->GetLocalMachineInfo(), false, unranked);
}

void OnlineConnectionQualityScene::Update(float dt)
{
    BaseSceneHandler::Update(dt);
    if (mUnidentified300 && !g_pFEInput->HasInputLock(this))
    {
        return;
    }
    mUnidentified160.Update(dt);
    if (mUnidentified160.mEnabled)
    {
        return;
    }
    if (!mUnidentified030)
    {
        TLSlide* slide = mPresentation->m_currentSlide;
        if (slide->GetCurrentTime() < slide->GetStartTime() + slide->GetDuration())
        {
            return;
        }
        InitializeInput();
        mUnidentified030 = true;
        for (int i = 0; i < 4; ++i)
        {
            GetPointerInstance(i)->SetActiveSlide("cursor", true, false);
        }
    }

    mUnidentified144.Update(dt);
    if (mUnidentified17C)
    {
        TLTextInstance* timer = FEFinder<TLTextInstance, 3>::Find(mPresentation->m_currentSlide, "Layer", "TIMER");
        UpdateConnectionQualityTimerText(this, timer);
        mUnidentified17C = false;
    }
    if (mUnidentified180 <= 0)
    {
        ShowWaitingForDecisions(this);
    }

    NetworkMachineRoster* roster = g_pNetworkSessionBase->GetMachineRoster();
    bool disconnected = false;
    for (int i = 0; i < roster->GetMachineCount(); ++i)
    {
        if (roster->GetMachineAid(i) == 0)
        {
            disconnected = true;
            break;
        }
    }
    if (disconnected)
    {
        ShowError(0x60);
        return;
    }

    UpdateConnectionQuality();
    for (unsigned int pad = 0; pad < 4; ++pad)
    {
        TLComponentInstance* controller = gFEPointerInstances[pad];
        if (pad != gFEControllerIndex)
        {
            controller->SetActiveSlide("waiting", true, false);
        }
        else
        {
            u8 valid = 1;
            FEPointerEvent event;
            event.mIndex = pad;
            event.mPosition = GetPointerPosition(pad, &valid);
            event.mPressed = g_pFEInput->JustPressed((eFEINPUT_PAD)pad, 0x1E, true, 0);
            event.mReleased = g_pFEInput->JustReleased((eFEINPUT_PAD)pad, 0x1E, true, 0);
            for (int i = 0; i < 2; ++i)
            {
                mUnidentified18C[i].HandlePointerEvent(&event);
            }
        }
    }

    bool isHost = roster->GetLocalMachineIndex() == 0;
    if (mUnidentified034 == 2 && isHost)
    {
        if (IsAnyConnectionRejected(this))
        {
            mUnidentified034 = 0;
            NetMessageConnectionDecision message;
            message.mAccepted = false;
            message.mMachineIndex = 0;
            g_pNetworkSession->SendConnectionDecisionToEveryone(&message);
        }
        else if (mUnidentified180 <= 0 || AreAllConnectionsAccepted(this))
        {
            mUnidentified034 = 1;
            SendLobbyDraft();
        }
    }

    if (mUnidentified034 == 0)
    {
        if (mUnidentified038 == 0)
        {
            mUnidentified160.SetEnabled(true);
        }
        else
        {
            ShowError(0x72);
        }
    }
}

void OnlineConnectionQualityScene::InitializeInput()
{
    typedef Detail::MemFunImpl<void, void (OnlineConnectionQualityScene::*)(unsigned int, void*)> PointerMethod;
    typedef BindExp3<void, PointerMethod, OnlineConnectionQualityScene*, Placeholder<0>, Placeholder<1> > PointerBinding;

    FEPointerListener::Callback over(
        PointerBinding(MemFun(&OnlineConnectionQualityScene::OnDecisionPointerEnter), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback off(
        PointerBinding(MemFun(&OnlineConnectionQualityScene::OnDecisionPointerLeave), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback select(
        PointerBinding(MemFun(&OnlineConnectionQualityScene::OnDecisionPointerPress), this, Placeholder<0>(), Placeholder<1>()));

    for (int i = 0; i < 2; ++i)
    {
        mUnidentified18C[i].SetInstanceBounds(
            mUnidentified2F4[i], true, 0.0f, 0.0f, 1.0f, 1.0f);
        mUnidentified18C[i].SetPointerEnterCallback(over);
        mUnidentified18C[i].SetPointerLeaveCallback(off);
        mUnidentified18C[i].SetPointerPressCallback(select);
    }
}

inline void OnlineConnectionQualityScene::ShowError(int error)
{
    if (GameSceneManager::Instance()->GetSceneType(GameSceneManager::Instance()->GetCurrentScene()) != 10)
    {
        FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push((SceneList)10, SCREEN_NOTHING, false);
        popup->Create((ePopupMenu)error,
            Function<FnVoidVoid>(Bind<void>(MemFun(&OnlineConnectionQualityScene::CloseConnectionsAndReturn), this)));
        mUnidentified300 = true;
    }
}

void OnlineConnectionQualityScene::OnDecisionPointerPress(unsigned int index, void* context)
{
    ShowWaitingForDecisions(this);

    if (!mUnidentified031)
    {
        mUnidentified031 = true;
        for (int i = 0; i < 4; ++i)
        {
            GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
        }

        NetworkMachineRoster* roster = g_pNetworkSessionBase->GetMachineRoster();
        bool isHost = roster->GetLocalMachineIndex() == 0;
        bool accepted = false;
        switch ((int)context)
        {
        case 0:
            accepted = true;
            mUnidentified038 = 1;
            FEAudio::PlayAnimAudioEvent(0xF0AFD586, 0, 0, true);
            break;
        case 1:
            accepted = false;
            mUnidentified038 = 0;
            FEAudio::PlayAnimAudioEvent(0x6F6A3A07, 0, 0, true);
            break;
        }

        if (isHost)
        {
            if (accepted)
            {
                mUnidentified03C[0] = 1;
            }
            else
            {
                mUnidentified03C[0] = 0;
            }
        }
        else
        {
            int machineIndex = roster->GetLocalMachineIndex();
            NetMessageConnectionDecision message;
            message.mAccepted = accepted;
            message.mMachineIndex = machineIndex;
            g_pNetworkSession->SendConnectionDecisionToHost(&message);
        }
    }
}

void OnlineConnectionQualityScene::OnConnectionDecision(NetMessageConnectionDecision* message)
{
    g_pNetworkSessionBase->GetMachineRoster()->GetLocalMachineIndex();
    s8 machine = message->mMachineIndex;
    if (machine == 0)
    {
        if (message->mAccepted)
        {
            mUnidentified034 = 1;
        }
        else
        {
            mUnidentified034 = 0;
        }
    }
    else
    {
        if (message->mAccepted)
        {
            mUnidentified03C[machine] = 1;
        }
        else
        {
            mUnidentified03C[machine] = 0;
        }
    }
}

void OnlineConnectionQualityScene::OnDecisionPointerEnter(unsigned int index, void* context)
{
    ++mUnidentified020[index];
    mUnidentified2F4[(int)context]->SetActiveSlide("OVER", true, false);
    mUnidentified18C[(int)context].SetPointerState(1, index);
    FEAudio::PlayAnimAudioEvent(0xDE912775, 0, 0, true);
}

void OnlineConnectionQualityScene::OnDecisionPointerLeave(unsigned int index, void* context)
{
    --mUnidentified020[index];
    mUnidentified2F4[(int)context]->SetActiveSlide("OFF", true, false);
    mUnidentified18C[(int)context].SetPointerState(0, index);
}

void OnlineConnectionQualityScene::CloseConnectionsAndReturn()
{
    mUnidentified300 = false;
    NetworkLobby* lobby = g_pNetworkSession->GetOnlineLobby();
    int machineIndex = lobby->GetLocalMachineIndex();
    bool isHost = machineIndex == 0;
    unsigned int profileId = 0;
    for (int i = 0; i < lobby->GetMachineCount(); ++i)
    {
        if (i != machineIndex)
        {
            profileId = mUnidentified184[i];
            break;
        }
    }
    g_pNetworkSession->GetOnlineLobby()->CloseConnectionsAndReset();

    if (IsOnlineRankedMatch())
    {
        if (gRejectedOpponentProfileIds.IsFull())
        {
            gRejectedOpponentProfileIds.Pop();
        }
        gRejectedOpponentProfileIds.Push(profileId);
        tDebugPrintManager::Print(DC_NETWORK,
            "Adding rejected PID %d to last rejected PIDS Q size now %d\n",
            profileId, gRejectedOpponentProfileIds.GetCount());
        GameSceneManager::Instance()->Push((SceneList)0x31, SCREEN_BACK, true);
    }
    else if (isHost)
    {
        SHOnlineInvitePlayers* scene = static_cast<SHOnlineInvitePlayers*>(
            GameSceneManager::Instance()->Push((SceneList)0x2C, SCREEN_NOTHING, true));
        scene->mIsHost = true;
        scene->mStartFriendServer = true;
    }
    else
    {
        g_pFriendManager->SetOwnStatusAvailable();
        GameSceneManager::Instance()->Push((SceneList)g_pFriendManager->mReturnScene, SCREEN_BACK, true);
    }
}
