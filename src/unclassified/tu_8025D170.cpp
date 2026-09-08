#include "unclassified/tu_8025D170.h"
#include "Game/Sys/debug.h"

#include "Game/BaseGameSceneManager.h"
#include "Game/GameInfo.h"
#include "Game/FE/feFinder.h"
#include "Game/FE/feInput.h"
#include "Game/FE/feMusic.h"
#include "Game/FE/fePackage.h"
#include "Game/FE/fePopupMenu.h"
#include "Game/FE/feScene.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlSlide.h"
#include "Game/NetworkMessages.h"
#include "Game/NetworkSession.h"
#include "Game/Render/Presentation.h"
#include "Game/tu_801360A4.h"
#include "NL/nlBind.h"
#include "NL/nlLocalization.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"
#include "unclassified/tu_802196B0.h"
#include "unclassified/tu_80252180.h"

extern "C" void fn_801CBCA0(unsigned long hash, int value0, int value1, int value2);
extern "C" Presentation* fn_801FEEAC();
extern "C" bool fn_8025BDA0();
extern "C" void fn_8025BDAC(int value);
extern "C" void fn_8025BDC4(int index);
extern TLComponentInstance* lbl_80578450[4];
extern unsigned int lbl_806E18B0;
extern bool lbl_806E1009;
extern TLComponentInstance lbl_80580030;
extern "C" void fn_80253348(TU80252180Scene* scene, int value, bool enabled);
extern "C" void fn_801CC9B0(TU80219248Component* component, TLComponentInstance* instance, int value);
extern "C" int fn_80261770(int error, bool connected, int value);
extern "C" void fn_80302420(TLComponentInstance* instance, bool value);
extern "C" GameInfoManager* fn_80077224();
extern int lbl_806E20E0;
extern u16 lbl_8058436C[14];
extern u8 lbl_80584384[0x4C];
extern "C" const unsigned short* fn_801CA950(nlLocalization* localization, const char* name);
extern "C" void fn_80242B24(TU8025D170Row* row, TLComponentInstance* instance,
    u16* name, int nameSize, u16* description, int descriptionSize, int index, bool value);

SHOnlineInvitePlayers::SHOnlineInvitePlayers()
    : mUnidentified01C(true)
    , mUnidentified01D(false)
    , mUnidentified1E(false)
    , mUnidentified30(false)
    , mUnidentified34(0.0f)
    , mUnidentified38(-1)
    , mUnidentified3C(0.0f)
    , mUnidentified40(false)
    , mUnidentified41(false)
    , mUnidentified42(false)
    , mUnidentified94C(0)
    , mUnidentified970(0)
{
    mUnidentified20[0] = 0;
    mUnidentified20[1] = 0;
    mUnidentified20[2] = 0;
    mUnidentified20[3] = 0;
    for (int i = 0; i < 4; ++i)
    {
        mUnidentified39C[i].mContext = (void*)i;
        mUnidentified67C[i].mContext = (void*)i;
    }
}

SHOnlineInvitePlayers::~SHOnlineInvitePlayers()
{
}

void SHOnlineInvitePlayers::fn_8025D3C8()
{
    int i;
    for (i = 0; i < mUnidentified94C; ++i)
    {
        if (mUnidentified950[i].mUnidentified00)
        {
            if (mUnidentified950[i].mUnidentified01)
                fn_8025F8A4(1, i);
            else
                fn_8025F8A4(0, i);
        }
        else if (mUnidentified950[i].mUnidentified01)
            fn_8025F8A4(6, i);
        else
            fn_8025F8A4(5, i);
    }
    if (mUnidentified01C && i < 4)
    {
        if (mUnidentified34 > 0.0f)
            fn_8025F8A4(4, i);
        else if (lbl_806E1194->mOwnStatus.mStatus == 2)
            fn_8025F8A4(3, i);
        else
            fn_8025F8A4(2, i);
        ++i;
    }
    for (; i < 4; ++i)
        fn_8025F8A4(7, i);
}

void SHOnlineInvitePlayers::fn_8025D520()
{
    TU80300104Base::Callback over(Bind<void>(MemFun(&SHOnlineInvitePlayers::fn_8025DE24), this, Placeholder<0>(), Placeholder<1>()));
    TU80300104Base::Callback off(Bind<void>(MemFun(&SHOnlineInvitePlayers::fn_8025DEBC), this, Placeholder<0>(), Placeholder<1>()));
    TU80300104Base::Callback select(Bind<void>(MemFun(&SHOnlineInvitePlayers::fn_8025DF3C), this, Placeholder<0>(), Placeholder<1>()));
    TU80300104Base::Callback cancelOver(Bind<void>(MemFun(&SHOnlineInvitePlayers::fn_8025DFAC), this, Placeholder<0>(), Placeholder<1>()));
    TU80300104Base::Callback cancelOff(Bind<void>(MemFun(&SHOnlineInvitePlayers::fn_8025E044), this, Placeholder<0>(), Placeholder<1>()));
    TU80300104Base::Callback cancelSelect(Bind<void>(MemFun(&SHOnlineInvitePlayers::fn_8025E0C4), this, Placeholder<0>(), Placeholder<1>()));
    TU80300104Base::Callback startOver(Bind<void>(MemFun(&SHOnlineInvitePlayers::fn_8025E128), this, Placeholder<0>(), Placeholder<1>()));
    TU80300104Base::Callback startOff(Bind<void>(MemFun(&SHOnlineInvitePlayers::fn_8025E19C), this, Placeholder<0>(), Placeholder<1>()));
    TU80300104Base::Callback startSelect(Bind<void>(MemFun(&SHOnlineInvitePlayers::fn_8025E1F8), this, Placeholder<0>(), Placeholder<1>()));
    mUnidentified2D4.fn_803007C0(startOver);
    mUnidentified2D4.fn_80300864(startOff);
    mUnidentified2D4.fn_803009AC(startSelect);
    for (int i = 0; i < 4; ++i)
    {
        feVector3 position = mUnidentified38C[i]->GetAssetPosition();
        mUnidentified39C[i].fn_80300D74(mUnidentified38C[i], true, -24.0f, 10.0f, 0.7f, 0.55f);
        mUnidentified39C[i].fn_803007C0(over);
        mUnidentified39C[i].fn_80300864(off);
        mUnidentified39C[i].fn_803009AC(select);
        mUnidentified67C[i].fn_80300D74(mUnidentified66C[i], true, position.f.x, position.f.y, 1.0f, 1.4f);
        mUnidentified67C[i].fn_803007C0(cancelOver);
        mUnidentified67C[i].fn_80300864(cancelOff);
        mUnidentified67C[i].fn_803009AC(cancelSelect);
    }
}

void SHOnlineInvitePlayers::fn_8025DE24(int index, void* context)
{
    fn_801CBCA0(0xF6EB899E, 0, 0, 1);
    ++mUnidentified20[(int)context];
    mUnidentified38C[(int)context]->SetActiveSlide("over", true, false);
    mUnidentified39C[(int)context].mValues[index] = 1;
}

void SHOnlineInvitePlayers::fn_8025DEBC(int index, void* context)
{
    --mUnidentified20[(int)context];
    mUnidentified38C[(int)context]->SetActiveSlide("off", true, false);
    mUnidentified39C[(int)context].mValues[index] = 0;
}

void SHOnlineInvitePlayers::fn_8025DF3C(int index, void* context)
{
    fn_801CBCA0(0xF0AFD586, 0, 0, 1);
    fn_8025BDAC(1);
    fn_8025BDC4((int)context);
    lbl_806E1838->Push((SceneList)0x2F, SCREEN_FORWARD, true);
}

void SHOnlineInvitePlayers::fn_8025DFAC(int index, void* context)
{
    fn_801CBCA0(0xAA73EF35, 0, 0, 1);
    ++mUnidentified20[(int)context];
    mUnidentified66C[(int)context]->SetActiveSlide("over", true, false);
    mUnidentified67C[(int)context].mValues[index] = 1;
}

void SHOnlineInvitePlayers::fn_8025E044(int index, void* context)
{
    --mUnidentified20[(int)context];
    mUnidentified66C[(int)context]->SetActiveSlide("off", true, false);
    mUnidentified67C[(int)context].mValues[index] = 0;
}

void SHOnlineInvitePlayers::fn_8025E0C4(int, void*)
{
    fn_801CBCA0(0x970D6164, 0, 0, 1);
    lbl_806E1194->SetOwnStatusInitial_80136FA4(0);
    mUnidentified38 = -1;
    mUnidentified34 = 0.0f;
    fn_8025D3C8();
}

void SHOnlineInvitePlayers::fn_8025E128(int index, void*)
{
    mUnidentified388->SetActiveSlide("over", true, false);
    mUnidentified2D4.mValues[index] = 1;
    fn_801CBCA0(0xAA73EF34, 0, 0, 1);
    mUnidentified41 = true;
}

void SHOnlineInvitePlayers::fn_8025E19C(int index, void*)
{
    mUnidentified388->SetActiveSlide("off", true, false);
    mUnidentified2D4.mValues[index] = 0;
    mUnidentified41 = false;
}

void SHOnlineInvitePlayers::fn_8025E1F8(int, void*)
{
    mUnidentified44.fn_80206B54();
    mUnidentified2D4.fn_80206B54();
    mUnidentified40 = true;
    g_pNetworkSession->fn_801216F0()->SetFriendHostInviting_80135208();
    for (int i = 0; i < 4; ++i)
        lbl_80578450[i]->SetActiveSlide("waiting", true, false);
}

void SHOnlineInvitePlayers::fn_8025E394()
{
    mUnidentified1E = false;
    if (g_pNetworkSession->fn_80120368())
    {
        lbl_806E1838->Pop();
        fn_801CBCA0(0x4430B152, 0, 0, 1);
        fn_801FEEAC()->Call("TransitionOnlineMatchToMainMenu");
        return;
    }
    fn_8025D3C8();
}

void SHOnlineInvitePlayers::SceneCreated()
{
    mUnidentified94C = 0;
    if (mUnidentified01C)
    {
        if (mUnidentified01D)
            g_pNetworkSession->fn_801216F0()->StartFriendServer();
        else
            mUnidentified3C = 30.0f;
        mUnidentified94C = 1;
        mUnidentified950[0].mUnidentified00 = true;
        mUnidentified950[0].mUnidentified01 = false;
        mUnidentified950[0].mUnidentified04 = 0;
        if (fn_8025BDA0())
        {
            mUnidentified950[mUnidentified94C].mUnidentified00 = true;
            mUnidentified950[mUnidentified94C].mUnidentified01 = true;
            mUnidentified950[mUnidentified94C].mUnidentified04 = 0;
            ++mUnidentified94C;
        }
        mUnidentified44.fn_801E6CB4(5);
    }

    TLComponentInstance* scrollbar = (TLComponentInstance*)FEFinder<TLComponentInstance, 4>::_Find(
        mPresentation->GetActiveSlide(), nlStringLowerHash("Layer"), nlStringLowerHash("scrollbar"), 0, 0, 0, 0);
    if (scrollbar == 0)
        scrollbar = &lbl_80580030;
    mUnidentified11C.fn_802308D0(scrollbar);
    mUnidentified11C.fn_80230B90(0);
    mUnidentified11C.fn_80230DE0(0);
    mUnidentified42 = mUnidentified01C && !(mUnidentified34 > 0.0f) && lbl_806E1194->mOwnStatus.mStatus != 2;
    TU80252180Scene* screen = fn_80253E18();
    fn_802534BC(screen, 0x24, true);
    mUnidentified2D0 = fn_80253D70(screen, 4);
    mUnidentified388 = fn_80253D70(screen, 0x20);
    mUnidentified44.fn_8022F194(mUnidentified2D0);
    if (mUnidentified42)
    {
        mUnidentified2D0->m_bVisible = true;
        mUnidentified44.fn_80260748();
    }
    else
    {
        fn_80253348(screen, 4, false);
        mUnidentified44.fn_80206B54();
    }
    fn_801CC9B0(&mUnidentified2D4, mUnidentified388, 0);
    fn_80253348(screen, 0x20, false);
    mUnidentified2D4.fn_80206B54();

    FEPresentation* presentation = mFEScene->m_pFEPackage->GetPresentation();
    for (int i = 0; i < 4; ++i)
    {
        char name[0x10];
        nlSNPrintf(name, sizeof(name), "FRIEND_%d", i);
        mUnidentified38C[i] = (TLComponentInstance*)FEFinder<TLComponentInstance, 4>::_Find(
            presentation->GetActiveSlide(), nlStringLowerHash("Layer"), nlStringLowerHash(name), 0, 0, 0, 0);
        TLComponentInstance* over = FEFinder<TLComponentInstance, 4>::Find(mUnidentified38C[i],
            nlStringLowerHash("over"), nlStringLowerHash("FRIEND_0"), 0, 0, 0, 0);
        TLComponentInstance* off = FEFinder<TLComponentInstance, 4>::Find(mUnidentified38C[i],
            nlStringLowerHash("off"), nlStringLowerHash("FRIEND_0"), 0, 0, 0, 0);
        mUnidentified66C[i] = (TLComponentInstance*)FEFinder<TLComponentInstance, 4>::_Find(
            over, nlStringLowerHash("cancel"), 0, 0, 0, 0, 0);
        TLComponentInstance* image = (TLComponentInstance*)FEFinder<TLComponentInstance, 4>::_Find(
            over, nlStringLowerHash("Mii_btn"), nlStringLowerHash("logo_32x32"), 0, 0, 0, 0);
        image->m_bVisible = false;
        fn_80302420(image, false);
        image = (TLComponentInstance*)FEFinder<TLComponentInstance, 4>::_Find(
            off, nlStringLowerHash("Mii_btn"), nlStringLowerHash("logo_32x32"), 0, 0, 0, 0);
        image->m_bVisible = false;
        fn_80302420(image, false);
    }
    TLComponentInstance* timer = (TLComponentInstance*)FEFinder<TLComponentInstance, 4>::_Find(
        presentation->GetActiveSlide(), nlStringLowerHash("Layer"), nlStringLowerHash("timer"), 0, 0, 0, 0);
    timer->m_bVisible = false;
    fn_8025ED98();
    fn_8025D3C8();
    fn_801CBCA0(0xD5048410, 0, 0, 1);
    FEMusic::StartStreamIfDifferent(8);
}

bool SHOnlineInvitePlayers::fn_8025ED98()
{
    NetworkLobby_80133634* lobby = g_pNetworkSession->fn_801216F0();
    if (lobby->GetMachineCount() == 0)
        return false;
    if (!lobby->AllMachineInfoReceived())
        return false;
    if (mUnidentified970 == lobby->GetMachineCount())
        return false;

    tDebugPrintManager::Print(DC_NETWORK,
        "SHOnlineInvitePlayers lobby numPeers changed from %d to %d\n",
        mUnidentified970, lobby->GetMachineCount());
    mUnidentified970 = lobby->GetMachineCount();
    mUnidentified94C = 0;
    for (int i = 0; i < lobby->GetMachineCount(); ++i)
    {
        mUnidentified950[mUnidentified94C].mUnidentified00 = i == lobby->GetLocalMachineIndex();
        mUnidentified950[mUnidentified94C].mUnidentified01 = false;
        UnidentifiedDraftEntry* entry = lobby->GetMachineInfo(i);
        mUnidentified950[mUnidentified94C].mUnidentified04 = entry;
        ++mUnidentified94C;
        if (entry != 0 && entry->mUnidentified7F)
        {
            mUnidentified950[mUnidentified94C].mUnidentified00 = i == lobby->GetLocalMachineIndex();
            mUnidentified950[mUnidentified94C].mUnidentified01 = true;
            mUnidentified950[mUnidentified94C].mUnidentified04 = entry;
            ++mUnidentified94C;
        }
    }
    if (lobby->GetMachineCount() >= 2 && mUnidentified01C && lbl_806E1009)
    {
        mUnidentified2D4.fn_80260748();
        mUnidentified388->m_bVisible = true;
        mUnidentified44.fn_80206B54();
        mUnidentified2D0->m_bVisible = false;
    }
    return true;
}

void SHOnlineInvitePlayers::Update(float fDeltaT)
{
    BaseSceneHandler::Update(fDeltaT);
    if (mUnidentified1E && !g_pFEInput->HasInputLock(this))
        return;
    if (!mUnidentified30)
    {
        TLSlide* slide = mPresentation->GetActiveSlide();
        if (!(slide->m_time >= slide->m_duration + slide->m_start))
            return;
        fn_8025D520();
        mUnidentified30 = true;
        fn_8025D3C8();
    }
    if (fn_8025ED98())
        fn_8025D3C8();
    if (mUnidentified42)
    {
        if (!(mUnidentified01C && !(mUnidentified34 > 0.0f) && lbl_806E1194->mOwnStatus.mStatus != 2))
        {
            mUnidentified2D0->m_bVisible = false;
            mUnidentified44.fn_80206B54();
            mUnidentified42 = false;
        }
    }
    else if (mUnidentified01C && !(mUnidentified34 > 0.0f) && lbl_806E1194->mOwnStatus.mStatus != 2)
    {
        mUnidentified2D0->m_bVisible = true;
        mUnidentified44.fn_80260748();
        mUnidentified42 = true;
    }
    for (unsigned int i = 0; i < 4; ++i)
    {
        TLComponentInstance* controller = lbl_80578450[i];
        if (g_pFEInput->m_InputLockDepth == 0)
        {
            if (i != lbl_806E18B0)
            {
                controller->SetActiveSlide("waiting", true, false);
                continue;
            }
            if (mUnidentified20[i] > 0 || mUnidentified44.mUnidentifiedD2[i] || mUnidentified41)
                controller->SetActiveSlide("A", true, false);
            else
                controller->SetActiveSlide("cursor", true, false);
        }
        unsigned char valid = 1;
        TU80300104Event event;
        event.mIndex = i;
        event.mPosition = fn_802197FC(i, &valid);
        event.mFlag0 = g_pFEInput->JustPressed((eFEINPUT_PAD)i, 0x1E, true, 0);
        mUnidentified2D4.fn_80219608(&event);
        if (!mUnidentified40)
        {
            if (mUnidentified44.fn_8022F2E0(event, fDeltaT))
            {
                fn_801CBCA0(0x64B85E8D, 0, 0, 1);
                NetworkLobby_80133634* lobby = g_pNetworkSession->fn_801216F0();
                if (lobby != 0)
                    lobby->StopFriendServer_801350E0();
                lbl_806E1194->SetOwnStatusInitial_80136FA4(1);
                return;
            }
            if (mUnidentified01C)
            {
                for (int j = 0; j < 4; ++j)
                {
                    mUnidentified39C[j].fn_80219608(&event);
                    mUnidentified67C[j].fn_80219608(&event);
                }
            }
        }
    }
    if (lbl_806E1194->mOwnStatus.mStatus == 2 && lbl_806E1194->HasFriendDeclined_80136C68())
    {
        mUnidentified38 = lbl_806E1194->mFriendStatusIndex;
        lbl_806E1194->SetOwnStatusInitial_80136FA4(0);
        mUnidentified34 = 3.0f;
        fn_8025D3C8();
    }
    if (mUnidentified34 > 0.0f)
    {
        mUnidentified34 -= fDeltaT;
        if (mUnidentified34 <= 0.0f)
        {
            mUnidentified34 = 0.0f;
            mUnidentified38 = -1;
            fn_8025D3C8();
        }
    }
    else if (mUnidentified3C > 0.0f && (mUnidentified3C -= fDeltaT) <= 0.0f
        && lbl_806E1194->mOwnStatus.mStatus == 2
        && lbl_806E1194->GetFriendInvitationResponse_80136D30() == 0)
    {
        lbl_806E1194->SetOwnStatusInitial_80136FA4(0);
        mUnidentified38 = -1;
        mUnidentified34 = 0.0f;
        int popup = fn_80261770(0x13A2E, false, 0x79);
        if (lbl_806E1838->GetSceneType(lbl_806E1838->GetCurrentScene()) != (SceneList)0xA)
        {
            FEPopupMenu* menu = (FEPopupMenu*)lbl_806E1838->Push((SceneList)0xA, SCREEN_NOTHING, false);
            menu->Create((ePopupMenu)popup, Function<FnVoidVoid>(Bind<void>(MemFun(&SHOnlineInvitePlayers::fn_8025E394), this)));
            mUnidentified1E = true;
        }
        return;
    }
    NetworkLobby_80133634* lobby = g_pNetworkSession->fn_801216F0();
    if (lobby->mMatchFailed)
    {
        lobby->CloseConnectionsAndReset();
        lbl_806E1194->SetOwnStatusInitial_80136FA4(0);
        int popup = fn_80261770(g_pNetworkSession->mDWCErrorCode, g_pNetworkSession->fn_80120368(), 0x5B);
        if (lbl_806E1838->GetSceneType(lbl_806E1838->GetCurrentScene()) != (SceneList)0xA)
        {
            FEPopupMenu* menu = (FEPopupMenu*)lbl_806E1838->Push((SceneList)0xA, SCREEN_NOTHING, false);
            menu->Create((ePopupMenu)popup, Function<FnVoidVoid>(Bind<void>(MemFun(&SHOnlineInvitePlayers::fn_8026066C), this)));
            mUnidentified1E = true;
        }
    }
    else if (lobby->mUnidentified052)
    {
        lobby->CloseConnectionsAndReset();
        lobby->StartFriendServer();
        lobby->mUnidentified052 = false;
    }
    else if (lobby->mCancelRequested)
    {
        if (lbl_806E1194->mOwnStatus.mStatus == 2)
        {
            mUnidentified38 = lbl_806E1194->mFriendStatusIndex;
            lbl_806E1194->SetOwnStatusInitial_80136FA4(0);
            mUnidentified34 = 3.0f;
            fn_8025D3C8();
        }
        lobby->mCancelRequested = false;
    }
}

void SHOnlineInvitePlayers::fn_8025F8A4(int value, int index)
{
    if (value == 0)
    {
        TU8025D170Row& row = mUnidentified974[index];
        row.fn_8025E438();
        nlStrNCpy(row.mUnidentified00, lbl_8058436C, 14);
        memcpy(row.mUnidentified1C, lbl_80584384, sizeof(row.mUnidentified1C));
        row.mUnidentified6C = 4;
        row.mUnidentified70 = 1;
        memset(&row.mUnidentified74, 0, sizeof(row.mUnidentified74));
        if (NetworkStatsManager_8012F378::Instance()->UsesEuropeanRankings())
        {
            if (NetworkStatsManager_8012F378::Instance()->fn_80260764(2) != 0)
                row.mUnidentified74 = *NetworkStatsManager_8012F378::Instance()->fn_80260764(2);
        }
        else
        {
            NetworkLeaderboardCategory* category = NetworkStatsManager_8012F378::Instance()->GetCategory(4);
            if (category != 0)
            {
                int player = category->FindPlayer(fn_80077224()->GetSaveSlot(lbl_806E20E0)->unknown_0x01C);
                if (player != -1)
                    row.mUnidentified74 = category->mMetadata[player];
            }
        }
        row.mUnidentified94 = true;
        fn_80242B24(&row, mUnidentified38C[index], mUnidentifiedBD4[index], 0x20,
            mUnidentifiedCD4[index], 0x30, index, mUnidentified30);
        mUnidentified39C[index].fn_80206B54();
        mUnidentified67C[index].fn_80206B54();
        return;
    }
    else if (value == 1 || value == 6)
    {
        TU8025D170Row& row = mUnidentified974[index];
        row.fn_8025E438();
        BasicString<unsigned short, Detail::TempStringAllocator> string(
            fn_801CA950(g_pLocalization, "ONLINE_CONTROLLERS_GUEST"));
        nlStrNCpy(row.mUnidentified00, string.c_str(), 14);
        if (value == 6)
            memcpy(row.mUnidentified1C, mUnidentified950[index].mUnidentified04->mUnidentified32, sizeof(row.mUnidentified1C));
        else
            memcpy(row.mUnidentified1C, lbl_80584384, sizeof(row.mUnidentified1C));
        row.mUnidentified6C = 4;
        row.mUnidentified70 = 1;
        row.mUnidentified95 = true;
        row.mUnidentified94 = true;
        fn_80242B24(&row, mUnidentified38C[index], mUnidentifiedBD4[index], 0x20,
            mUnidentifiedCD4[index], 0x30, index, mUnidentified30);
        mUnidentified39C[index].fn_80206B54();
        mUnidentified67C[index].fn_80206B54();
        return;
    }
    else if (value == 5)
    {
        TU8025D170Row& row = mUnidentified974[index];
        row.fn_8025E438();
        UnidentifiedDraftEntry* entry = mUnidentified950[index].mUnidentified04;
        nlStrNCpy(row.mUnidentified00, entry->mName, 14);
        memcpy(row.mUnidentified1C, entry->mUnidentified32, sizeof(row.mUnidentified1C));
        row.mUnidentified6C = 4;
        row.mUnidentified70 = 1;
        row.mUnidentified74 = entry->mHead;
        NetworkLeaderboardCategory* category = NetworkStatsManager_8012F378::Instance()->GetCategory(4);
        if (category != 0)
        {
            int player = category->FindPlayer(entry->mUnidentified18);
            if (player != -1)
                row.mUnidentified74.mDisplayRank = category->mMetadata[player].mDisplayRank;
        }
        row.mUnidentified94 = true;
        fn_80242B24(&row, mUnidentified38C[index], mUnidentifiedBD4[index], 0x20,
            mUnidentifiedCD4[index], 0x30, index, mUnidentified30);
        mUnidentified39C[index].fn_80206B54();
        mUnidentified67C[index].fn_80206B54();
        return;
    }
    else if (value == 2)
    {
        TU8025D170Row& row = mUnidentified974[index];
        row.fn_8025E438();
        row.mUnidentified6C = 2;
        row.mUnidentified94 = true;
        fn_80242B24(&row, mUnidentified38C[index], mUnidentifiedBD4[index], 0x20,
            mUnidentifiedCD4[index], 0x30, index, mUnidentified30);
        mUnidentified39C[index].fn_80260748();
        mUnidentified67C[index].fn_80206B54();
        fn_8025FF58(index);
        return;
    }
    else if (value == 3)
    {
        TU8025D170Row& row = mUnidentified974[index];
        row.fn_8025E438();
        int friendIndex = lbl_806E1194->fn_8026075C();
        nlStrNCpy(row.mUnidentified00,
            (const u16*)fn_80077224()->GetUnknown0x340(lbl_806E20E0, friendIndex), 14);
        row.mUnidentified6C = 4;
        row.mUnidentified70 = 10;
        row.mUnidentified74.fn_8025E41C();
        row.mUnidentified90 = 0;
        row.mUnidentified94 = true;
        row.mUnidentified95 = false;
        row.mUnidentified96 = true;
        fn_80242B24(&row, mUnidentified38C[index], mUnidentifiedBD4[index], 0x20,
            mUnidentifiedCD4[index], 0x30, index, mUnidentified30);
        mUnidentified39C[index].fn_80206B54();
        mUnidentified67C[index].fn_80260748();
        fn_8025FF58(index);
        return;
    }
    else if (value == 4)
    {
        TU8025D170Row& row = mUnidentified974[index];
        row.fn_8025E438();
        nlStrNCpy(row.mUnidentified00,
            (const u16*)fn_80077224()->GetUnknown0x340(lbl_806E20E0, mUnidentified38), 14);
        row.mUnidentified6C = 4;
        row.mUnidentified70 = 9;
        row.mUnidentified74.fn_8025E41C();
        row.mUnidentified90 = 0;
        row.mUnidentified94 = true;
        row.mUnidentified95 = false;
        row.mUnidentified96 = true;
        fn_80242B24(&row, mUnidentified38C[index], mUnidentifiedBD4[index], 0x20,
            mUnidentifiedCD4[index], 0x30, index, mUnidentified30);
        mUnidentified39C[index].fn_80206B54();
        mUnidentified67C[index].fn_80260748();
        fn_8025FF58(index);
        return;
    }
    else if (value == 7)
    {
        TU8025D170Row& row = mUnidentified974[index];
        row.fn_8025E438();
        row.mUnidentified94 = false;
        fn_80242B24(&row, mUnidentified38C[index], mUnidentifiedBD4[index], 0x20,
            mUnidentifiedCD4[index], 0x30, index, mUnidentified30);
        mUnidentified39C[index].fn_80206B54();
        mUnidentified67C[index].fn_80206B54();
        fn_8025FF58(index);
        return;
    }
}

void SHOnlineInvitePlayers::fn_8025FF58(int index)
{
    TLComponentInstance* over = FEFinder<TLComponentInstance, 4>::Find(mUnidentified38C[index],
        nlStringLowerHash("over"), nlStringLowerHash("FRIEND_0"), 0, 0, 0, 0);
    TLComponentInstance* off = FEFinder<TLComponentInstance, 4>::Find(mUnidentified38C[index],
        nlStringLowerHash("off"), nlStringLowerHash("FRIEND_0"), 0, 0, 0, 0);
    TLComponentInstance* image = (TLComponentInstance*)FEFinder<TLComponentInstance, 4>::_Find(
        over, nlStringLowerHash("Mii_btn"), nlStringLowerHash("Mii"), 0, 0, 0, 0);
    image->m_bVisible = false;
    fn_80302420(image, false);
    image = (TLComponentInstance*)FEFinder<TLComponentInstance, 4>::_Find(
        off, nlStringLowerHash("Mii_btn"), nlStringLowerHash("Mii"), 0, 0, 0, 0);
    image->m_bVisible = false;
    fn_80302420(image, false);
    image = (TLComponentInstance*)FEFinder<TLComponentInstance, 4>::_Find(
        over, nlStringLowerHash("Mii_btn"), nlStringLowerHash("logo_32x32"), 0, 0, 0, 0);
    image->m_bVisible = false;
    fn_80302420(image, false);
    image = (TLComponentInstance*)FEFinder<TLComponentInstance, 4>::_Find(
        off, nlStringLowerHash("Mii_btn"), nlStringLowerHash("logo_32x32"), 0, 0, 0, 0);
    image->m_bVisible = false;
    fn_80302420(image, false);
    image = (TLComponentInstance*)FEFinder<TLComponentInstance, 4>::_Find(
        over, nlStringLowerHash("Mii_btn"), nlStringLowerHash("shoulders"), 0, 0, 0, 0);
    image->m_bVisible = false;
    fn_80302420(image, false);
    image = (TLComponentInstance*)FEFinder<TLComponentInstance, 4>::_Find(
        off, nlStringLowerHash("Mii_btn"), nlStringLowerHash("shoulders"), 0, 0, 0, 0);
    image->m_bVisible = false;
    fn_80302420(image, false);
    image = (TLComponentInstance*)FEFinder<TLComponentInstance, 4>::_Find(
        over, nlStringLowerHash("Mii_btn"), nlStringLowerHash("Online_Mii_select_background"), 0, 0, 0, 0);
    image->m_bVisible = false;
    fn_80302420(image, false);
    image = (TLComponentInstance*)FEFinder<TLComponentInstance, 4>::_Find(
        off, nlStringLowerHash("Mii_btn"), nlStringLowerHash("Online_Mii_select_background"), 0, 0, 0, 0);
    image->m_bVisible = false;
    fn_80302420(image, false);
}

void SHOnlineInvitePlayers::fn_8026066C()
{
    mUnidentified1E = false;
    if (g_pNetworkSession->fn_80120368())
    {
        lbl_806E1838->Pop();
        fn_801CBCA0(0x4430B152, 0, 0, 1);
        fn_801FEEAC()->Call("TransitionOnlineMatchToMainMenu");
        return;
    }
    if (mUnidentified01C)
    {
        lbl_806E1838->Push((SceneList)5, SCREEN_BACK, true);
        return;
    }
    lbl_806E1194->SetOwnStatusAvailable_801374A4();
    lbl_806E1838->Push((SceneList)lbl_806E1194->mUnidentified00C, SCREEN_BACK, true);
}
