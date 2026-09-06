#include "unclassified/tu_8026F444.h"

#include "Game/BaseGameSceneManager.h"
#include "Game/NetworkMessages.h"
#include "Game/NetworkSession.h"
#include "Game/tu_801360A4.h"
#include "Game/FE/feFinder.h"
#include "Game/FE/feInput.h"
#include "Game/FE/fePopupMenu.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlTextInstance.h"
#include "NL/nlBind.h"
#include "NL/nlFormat.h"
#include "NL/nlLocalizationLookup.h"
#include "NL/nlString.h"
#include "unclassified/tu_802196B0.h"
#include "unclassified/tu_80245DB4.h"
#include "unclassified/tu_80252180.h"
#include "unclassified/tu_8025D170.h"
#include "unclassified/tu_80326844.h"

extern TLComponentInstance* lbl_80578450[4];
extern TLComponentInstance lbl_80580030;
extern unsigned int lbl_806E18B0;
extern BaseGameSceneManager* lbl_806E1838;

extern "C" void fn_80306208(UnidentifiedTimer_8030616C* timer, bool enabled);
extern "C" void fn_80306224(UnidentifiedTimer_8030616C* timer, float dt);
extern "C" bool fn_8025BD88();
extern "C" int fn_8004F594(int channel, const char* format, ...);
extern "C" void fn_801CBCA0(
    unsigned long cueId, const void* debugName, void* context, bool restartable);

const char* lbl_806DEDB8[2] = { "ACCEPT", "REJECT" };

TU8026F444Scene::TU8026F444Scene()
    : mUnidentified030(false)
    , mUnidentified031(false)
    , mUnidentified034(2)
    , mUnidentified038(2)
    , mUnidentified144(1.0f,
          Function<UnidentifiedTimer_8030616C*>(
              Bind<void>(MemFun(&TU8026F444Scene::fn_8026F80C), this, Placeholder<0>())))
    , mUnidentified160(1.0f,
          Function<UnidentifiedTimer_8030616C*>(
              Bind<void>(MemFun(&TU8026F444Scene::fn_8026F874), this, Placeholder<0>())))
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
    fn_80306208(&mUnidentified160, false);
}

TU8026F444Scene::~TU8026F444Scene()
{
    TU80252180Scene* scene = fn_80253E18();
    if (scene != 0)
    {
        fn_802533F0(scene);
    }
}

void TU8026F444Scene::fn_8026F7F8(NetMessageCheckConnection* message)
{
    mUnidentified184[0] = message->mUnidentified08[0];
    mUnidentified184[1] = message->mUnidentified08[1];
}

void TU8026F444Scene::fn_8026F80C(UnidentifiedTimer_8030616C* timer)
{
    mUnidentified17C = true;
    if (mUnidentified180 > 0)
    {
        --mUnidentified180;
        if (mUnidentified180 <= 5 && mUnidentified180 > 0)
        {
            if (mUnidentified180 == 1)
            {
                fn_801CBCA0(0x09AA8790, 0, 0, true);
            }
            else
            {
                fn_801CBCA0(0xFF48403F, 0, 0, true);
            }
        }
    }
}

void TU8026F444Scene::fn_8026F874(UnidentifiedTimer_8030616C* timer)
{
    fn_80271768();
}

void TU8026F444Scene::fn_8026F878()
{
    TU80300104Base::Callback over(
        Bind<void>(MemFun(&TU8026F444Scene::fn_80271640), this, Placeholder<0>(), Placeholder<1>()));
    TU80300104Base::Callback off(
        Bind<void>(MemFun(&TU8026F444Scene::fn_802716E0), this, Placeholder<0>(), Placeholder<1>()));
    TU80300104Base::Callback select(
        Bind<void>(MemFun(&TU8026F444Scene::fn_8026FBB0), this, Placeholder<0>(), Placeholder<1>()));

    for (int i = 0; i < 2; ++i)
    {
        mUnidentified18C[i].fn_80300D74(
            mUnidentified2F4[i], true, 0.0f, 0.0f, 1.0f, 1.0f);
        mUnidentified18C[i].fn_803007C0(over);
        mUnidentified18C[i].fn_80300864(off);
        mUnidentified18C[i].fn_803009AC(select);
    }
}

void TU8026F444Scene::fn_8026FBB0(int index, void* context)
{
    mUnidentified2F4[0]->m_bVisible = false;
    mUnidentified18C[0].fn_80206B54();
    mUnidentified2F4[1]->m_bVisible = false;
    mUnidentified18C[1].fn_80206B54();

    TLComponentInstance* component = FEFinder<TLComponentInstance, 4>::Find(
        mPresentation->GetActiveSlide(), InlineHasher("Layer"), InlineHasher("WAITING"));
    if (component == 0)
    {
        component = &lbl_80580030;
    }
    component->m_bVisible = true;

    if (!mUnidentified031)
    {
        mUnidentified031 = true;
        for (int i = 0; i < 4; ++i)
        {
            lbl_80578450[i]->SetActiveSlide("waiting", true, false);
        }

        UnidentifiedMachineRoster* roster = lbl_806E20D8->GetMachineRoster();
        bool isHost = roster->GetLocalMachineIndex() == 0;
        bool accepted = false;
        switch ((int)context)
        {
        case 0:
            accepted = true;
            mUnidentified038 = 1;
            fn_801CBCA0(0xF0AFD586, 0, 0, true);
            break;
        case 1:
            accepted = false;
            mUnidentified038 = 0;
            fn_801CBCA0(0x6F6A3A07, 0, 0, true);
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
            NetworkMessageType27_8050B750 message;
            message.mUnidentified08 = accepted;
            message.mUnidentified09 = machineIndex;
            lbl_806E10EC->SendConnectionDecisionToHost(&message);
        }
    }
}

void TU8026F444Scene::fn_8026FF28(NetworkMessageType27_8050B750* message)
{
    lbl_806E20D8->GetMachineRoster()->GetLocalMachineIndex();
    s8 machine = message->mUnidentified09;
    if (machine == 0)
    {
        if (message->mUnidentified08)
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
        if (message->mUnidentified08)
        {
            mUnidentified03C[machine] = 1;
        }
        else
        {
            mUnidentified03C[machine] = 0;
        }
    }
}

void TU8026F444Scene::SceneCreated()
{
    for (int i = 0; i < 2; ++i)
    {
        TLComponentInstance* component = FEFinder<TLComponentInstance, 4>::Find(
            mPresentation->GetActiveSlide(), InlineHasher("Layer"), InlineHasher(lbl_806DEDB8[i]));
        if (component == 0)
        {
            component = &lbl_80580030;
        }
        mUnidentified2F4[i] = component;
    }

    FEFinder<TLComponentInstance, 4>::Find(
        mPresentation->GetActiveSlide(), InlineHasher("Layer"), InlineHasher("QUALITY"));

    TLComponentInstance* component = FEFinder<TLComponentInstance, 4>::Find(
        mPresentation->GetActiveSlide(), InlineHasher("Layer"), InlineHasher("WAITING"));
    if (component == 0)
    {
        component = &lbl_80580030;
    }
    component->m_bVisible = false;

    TLTextInstance* timer = FEFinder<TLTextInstance, 3>::Find(
        mPresentation->GetActiveSlide(), InlineHasher("Layer"), InlineHasher("TIMER"));
    typedef BasicString<unsigned short, Detail::TempStringAllocator> WideBasicString;
    timer->SetString(nlStrNCpy(mUnidentified044,
        Format(WideBasicString(LookupLocString("ONLINE_CONNECTION_QUALITY_TIME")),
            mUnidentified180).c_str(), 128));

    fn_80270870();
    TU80252180Scene* scene = fn_80253E18();
    if (scene != 0)
    {
        fn_802534BC(scene, 0, true);
    }
    for (int i = 0; i < 4; ++i)
    {
        lbl_80578450[i]->SetActiveSlide("waiting", true, false);
    }
    fn_801CBCA0(0xBB142B94, 0, 0, true);
}

void TU8026F444Scene::fn_80270870()
{
    UnidentifiedMachineRoster* roster = lbl_806E20D8->GetMachineRoster();
    unsigned int value = 0;
    for (int i = 0; i < roster->GetMachineCount(); ++i)
    {
        UnidentifiedTransportConnection* connection
            = (UnidentifiedTransportConnection*)roster->GetMachineAid(i);
        if (connection != 0 && connection != (UnidentifiedTransportConnection*)-1)
        {
            value = value < connection->mUnidentified068 ? connection->mUnidentified068 : value;
        }
    }

    TLComponentInstance* component = FEFinder<TLComponentInstance, 4>::Find(
        mPresentation->GetActiveSlide(), InlineHasher("Layer"), InlineHasher("QUALITY"),
        InlineHasher("RATING"), InlineHasher("stars"));
    if (component == 0)
    {
        component = &lbl_80580030;
    }
    mUnidentified2FC = component;
    value >>= 1;
    if (value > 200)
    {
        component->SetActiveSlide("1", true, false);
    }
    else if (value > 160)
    {
        component->SetActiveSlide("2", true, false);
    }
    else if (value > 80)
    {
        component->SetActiveSlide("3", true, false);
    }
    else if (value != 0)
    {
        component->SetActiveSlide("4", true, false);
    }
    else
    {
        component->SetActiveSlide("0", true, false);
    }
}

void TU8026F444Scene::Update(float dt)
{
    BaseSceneHandler::Update(dt);
    if (mUnidentified300 && !g_pFEInput->HasInputLock(this))
    {
        return;
    }
    fn_80306224(&mUnidentified160, dt);
    if (mUnidentified160.mEnabled)
    {
        return;
    }
    if (!mUnidentified030)
    {
        TLSlide* slide = mPresentation->GetActiveSlide();
        if (slide->GetCurrentTime() < slide->m_start + slide->m_duration)
        {
            return;
        }
        fn_8026F878();
        mUnidentified030 = true;
        for (int i = 0; i < 4; ++i)
        {
            lbl_80578450[i]->SetActiveSlide("cursor", true, false);
        }
    }

    fn_80306224(&mUnidentified144, dt);
    if (mUnidentified17C)
    {
        TLTextInstance* timer = FEFinder<TLTextInstance, 3>::Find(
            mPresentation->GetActiveSlide(), InlineHasher("Layer"), InlineHasher("TIMER"));
        typedef BasicString<unsigned short, Detail::TempStringAllocator> WideBasicString;
        timer->SetString(nlStrNCpy(mUnidentified044,
            Format(WideBasicString(LookupLocString("ONLINE_CONNECTION_QUALITY_TIME")),
                mUnidentified180).c_str(), 128));
        mUnidentified17C = false;
    }
    if (mUnidentified180 <= 0)
    {
        mUnidentified2F4[0]->m_bVisible = false;
        mUnidentified18C[0].fn_80206B54();
        mUnidentified2F4[1]->m_bVisible = false;
        mUnidentified18C[1].fn_80206B54();
        TLComponentInstance* component = FEFinder<TLComponentInstance, 4>::Find(
            mPresentation->GetActiveSlide(), InlineHasher("Layer"), InlineHasher("WAITING"));
        if (component == 0)
        {
            component = &lbl_80580030;
        }
        component->m_bVisible = true;
    }

    UnidentifiedMachineRoster* roster = lbl_806E20D8->GetMachineRoster();
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
        if (lbl_806E1838->GetSceneType(lbl_806E1838->GetCurrentScene()) != (SceneList)10)
        {
            FEPopupMenu* popup = static_cast<FEPopupMenu*>(
                lbl_806E1838->Push((SceneList)10, SCREEN_NOTHING, false));
            popup->Create((ePopupMenu)0x60,
                Function<FnVoidVoid>(Bind<void>(MemFun(&TU8026F444Scene::fn_80271768), this)));
            mUnidentified300 = true;
        }
        return;
    }

    fn_80270870();
    for (unsigned int pad = 0; pad < 4; ++pad)
    {
        TLComponentInstance* controller = lbl_80578450[pad];
        if (pad != lbl_806E18B0)
        {
            controller->SetActiveSlide("waiting", true, false);
        }
        else
        {
            u8 valid = 1;
            TU80300104Event event;
            event.mIndex = pad;
            event.mPosition = fn_802197FC(pad, &valid);
            event.mFlag0 = g_pFEInput->JustPressed((eFEINPUT_PAD)pad, 0x1E, true, 0);
            event.mFlag1 = g_pFEInput->JustReleased((eFEINPUT_PAD)pad, 0x1E, true, 0);
            for (int i = 0; i < 2; ++i)
            {
                mUnidentified18C[i].fn_80219608(&event);
            }
        }
    }

    bool isHost = roster->GetLocalMachineIndex() == 0;
    if (mUnidentified034 == 2 && isHost)
    {
        UnidentifiedMachineRoster* machines = lbl_806E20D8->GetMachineRoster();
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
            NetworkMessageType27_8050B750 message;
            message.mUnidentified08 = false;
            message.mUnidentified09 = 0;
            lbl_806E10EC->SendConnectionDecisionToEveryone(&message);
        }
        else
        {
            bool accepted = true;
            if (mUnidentified180 > 0)
            {
                UnidentifiedMachineRoster* machines = lbl_806E20D8->GetMachineRoster();
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
                NetworkLobby_80133634* lobby = lbl_806E10EC->fn_801216F0();
                bool value = !fn_8025BD88();
                UnidentifiedDraftEntry* info = lobby->GetLocalMachineInfo();
                lbl_806E10EC->SendDraftToEveryone(lobby->GetPlayerCount(), info, false, value);
            }
        }
    }

    if (mUnidentified034 == 0)
    {
        if (mUnidentified038 == 0)
        {
            fn_80306208(&mUnidentified160, true);
        }
        else if (lbl_806E1838->GetSceneType(lbl_806E1838->GetCurrentScene()) != (SceneList)10)
        {
            FEPopupMenu* popup = static_cast<FEPopupMenu*>(
                lbl_806E1838->Push((SceneList)10, SCREEN_NOTHING, false));
            popup->Create((ePopupMenu)0x72,
                Function<FnVoidVoid>(Bind<void>(MemFun(&TU8026F444Scene::fn_80271768), this)));
            mUnidentified300 = true;
        }
    }
}

void TU8026F444Scene::fn_80271640(int index, void* context)
{
    ++mUnidentified020[index];
    mUnidentified2F4[(int)context]->SetActiveSlide("OVER", true, false);
    mUnidentified18C[(int)context].mValues[index] = 1;
    fn_801CBCA0(0xDE912775, 0, 0, true);
}

void TU8026F444Scene::fn_802716E0(int index, void* context)
{
    --mUnidentified020[index];
    mUnidentified2F4[(int)context]->SetActiveSlide("OFF", true, false);
    mUnidentified18C[(int)context].mValues[index] = 0;
}

void TU8026F444Scene::fn_80271768()
{
    mUnidentified300 = false;
    NetworkLobby_80133634* lobby = lbl_806E10EC->fn_801216F0();
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
    lbl_806E10EC->fn_801216F0()->CloseConnectionsAndReset();

    if (fn_8025BD88())
    {
        if (lbl_8057848C.mUnidentified08 >= lbl_8057848C.mUnidentified0C)
        {
            lbl_8057848C.UnidentifiedRemoveStart();
        }
        *lbl_8057848C.UnidentifiedAddEnd() = profileId;
        fn_8004F594(0x10,
            "Adding rejected PID %d to last rejected PIDS Q size now %d\n",
            profileId, lbl_8057848C.mUnidentified08);
        lbl_806E1838->Push((SceneList)0x31, SCREEN_BACK, true);
    }
    else if (isHost)
    {
        TU8025D170Scene* scene = static_cast<TU8025D170Scene*>(
            lbl_806E1838->Push((SceneList)0x2C, SCREEN_NOTHING, true));
        scene->mUnidentified01C = true;
        scene->mUnidentified01D = true;
    }
    else
    {
        lbl_806E1194->SetOwnStatusAvailable_801374A4();
        lbl_806E1838->Push((SceneList)lbl_806E1194->mUnidentified00C, SCREEN_BACK, true);
    }
}
