#include "Game/GameInfo.h"
#include "NL/nlBasicString.inl"
#include "Game/FE/fePresentation.inl"
#include "Game/SH/SHOnlineConnectionQuality.h"
#include "Game/FE/FEAudio.h"
#include "Game/Sys/debug.h"

#include "Game/GameSceneManager.h"
#include "Game/NetworkMessages.h"
#include "Game/NetworkSession.h"
#include "Game/NetworkLobby.h"
#include "Game/FriendManager.h"
#include "Game/FE/feFinder.inl"
#include "Game/FE/feInput.h"
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


const char* sConnectionDecisionComponentNames[2] = { "ACCEPT", "REJECT" };

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

void OnlineConnectionQualityScene::InitializeInput()
{
    typedef Detail::MemFunImpl<void, void (OnlineConnectionQualityScene::*)(int, void*)> PointerMethod;
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

void OnlineConnectionQualityScene::OnDecisionPointerPress(int index, void* context)
{
    mUnidentified2F4[0]->m_bVisible = false;
    mUnidentified18C[0].Disable();
    mUnidentified2F4[1]->m_bVisible = false;
    mUnidentified18C[1].Disable();

    TLComponentInstance* component = FEFinder<TLComponentInstance, 4>::FindOrDefault(mPresentation->m_currentSlide, "Layer", "WAITING");
    component->m_bVisible = true;

    if (!mUnidentified031)
    {
        mUnidentified031 = true;
        for (int i = 0; i < 4; ++i)
        {
            gFEPointerInstances[i]->SetActiveSlide("waiting", true, false);
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

void OnlineConnectionQualityScene::SceneCreated()
{
    for (int i = 0; i < 2; ++i)
    {
        TLComponentInstance* component = FEFinder<TLComponentInstance, 4>::FindOrDefault(mPresentation->m_currentSlide, "Layer", sConnectionDecisionComponentNames[i]);
        mUnidentified2F4[i] = component;
    }

    FEFinder<TLComponentInstance, 4>::Find(mPresentation->m_currentSlide, "Layer", "QUALITY");

    TLComponentInstance* component = FEFinder<TLComponentInstance, 4>::FindOrDefault(mPresentation->m_currentSlide, "Layer", "WAITING");
    component->m_bVisible = false;

    TLTextInstance* timer = FEFinder<TLTextInstance, 3>::Find(mPresentation->m_currentSlide, "Layer", "TIMER");
    typedef BasicString<unsigned short, Detail::TempStringAllocator> WideBasicString;
    timer->SetString(nlStrNCpy(mUnidentified044,
        Format(WideBasicString(LookupLocString("ONLINE_CONNECTION_QUALITY_TIME")),
            mUnidentified180).c_str(), 128));

    UpdateConnectionQuality();
    SHNavigation* scene = GetNavigationScene();
    if (scene != 0)
    {
        scene->SetButtons(0, true);
    }
    for (int i = 0; i < 4; ++i)
    {
        gFEPointerInstances[i]->SetActiveSlide("waiting", true, false);
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
        if (slide->GetCurrentTime() < slide->m_start + slide->m_duration)
        {
            return;
        }
        InitializeInput();
        mUnidentified030 = true;
        for (int i = 0; i < 4; ++i)
        {
            gFEPointerInstances[i]->SetActiveSlide("cursor", true, false);
        }
    }

    mUnidentified144.Update(dt);
    if (mUnidentified17C)
    {
        TLTextInstance* timer = FEFinder<TLTextInstance, 3>::Find(mPresentation->m_currentSlide, "Layer", "TIMER");
        typedef BasicString<unsigned short, Detail::TempStringAllocator> WideBasicString;
        timer->SetString(nlStrNCpy(mUnidentified044,
            Format(WideBasicString(LookupLocString("ONLINE_CONNECTION_QUALITY_TIME")),
                mUnidentified180).c_str(), 128));
        mUnidentified17C = false;
    }
    if (mUnidentified180 <= 0)
    {
        mUnidentified2F4[0]->m_bVisible = false;
        mUnidentified18C[0].Disable();
        mUnidentified2F4[1]->m_bVisible = false;
        mUnidentified18C[1].Disable();
        TLComponentInstance* component = FEFinder<TLComponentInstance, 4>::FindOrDefault(mPresentation->m_currentSlide, "Layer", "WAITING");
        component->m_bVisible = true;
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
        if (GameSceneManager::Instance()->GetSceneType(GameSceneManager::Instance()->GetCurrentScene()) != (SceneList)10)
        {
            FEPopupMenu* popup = static_cast<FEPopupMenu*>(
                GameSceneManager::Instance()->Push((SceneList)10, SCREEN_NOTHING, false));
            popup->Create((ePopupMenu)0x60,
                Function<FnVoidVoid>(Bind<void>(MemFun(&OnlineConnectionQualityScene::CloseConnectionsAndReturn), this)));
            mUnidentified300 = true;
        }
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
        NetworkMachineRoster* machines = g_pNetworkSessionBase->GetMachineRoster();
        bool rejected = false;
        for (int i = 0; i < machines->GetMachineCount(); ++i)
        {
            if (mUnidentified03C[i] == 0)
            {
                rejected = true;
                break;
            }
        }
        if (rejected)
        {
            mUnidentified034 = 0;
            NetMessageConnectionDecision message;
            message.mAccepted = false;
            message.mMachineIndex = 0;
            g_pNetworkSession->SendConnectionDecisionToEveryone(&message);
        }
        else
        {
            bool accepted = true;
            if (mUnidentified180 > 0)
            {
                NetworkMachineRoster* machines = g_pNetworkSessionBase->GetMachineRoster();
                for (int i = 0; i < machines->GetMachineCount(); ++i)
                {
                    if (mUnidentified03C[i] != 1)
                    {
                        accepted = false;
                        break;
                    }
                }
            }
            if (accepted)
            {
                mUnidentified034 = 1;
                NetworkLobby* lobby = g_pNetworkSession->GetOnlineLobby();
                bool value = !IsOnlineRankedMatch();
                NetworkDraftMachineInfo* info = lobby->GetLocalMachineInfo();
                g_pNetworkSession->SendDraftToEveryone(lobby->GetPlayerCount(), info, false, value);
            }
        }
    }

    if (mUnidentified034 == 0)
    {
        if (mUnidentified038 == 0)
        {
            mUnidentified160.SetEnabled(true);
        }
        else if (GameSceneManager::Instance()->GetSceneType(GameSceneManager::Instance()->GetCurrentScene()) != (SceneList)10)
        {
            FEPopupMenu* popup = static_cast<FEPopupMenu*>(
                GameSceneManager::Instance()->Push((SceneList)10, SCREEN_NOTHING, false));
            popup->Create((ePopupMenu)0x72,
                Function<FnVoidVoid>(Bind<void>(MemFun(&OnlineConnectionQualityScene::CloseConnectionsAndReturn), this)));
            mUnidentified300 = true;
        }
    }
}

void OnlineConnectionQualityScene::OnDecisionPointerEnter(int index, void* context)
{
    ++mUnidentified020[index];
    mUnidentified2F4[(int)context]->SetActiveSlide("OVER", true, false);
    mUnidentified18C[(int)context].SetPointerState(1, index);
    FEAudio::PlayAnimAudioEvent(0xDE912775, 0, 0, true);
}

void OnlineConnectionQualityScene::OnDecisionPointerLeave(int index, void* context)
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
