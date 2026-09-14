#include <dwc/dwc_account.h>
#include <dwc/dwc_friend.h>
#include "Game/main.h"
#include "Game/SH/SHOnlineFriends.h"
#include "Game/SH/SHOnlineInvitePlayers.h"
#include "Game/FE/FEAudio.h"
#include "Game/FE/feHelpFuncs_decl.h"
#include "Game/OnlineMatchmaking.h"
#include "Game/NetworkLobby.h"

#include "Game/GameSceneManager.h"
#include "Game/FE/feInlineHasher.inl"
#include "Game/FE/feFinderFind_impl.h"
#include "Game/FE/feInput.h"
#include "Game/FE/fePopupMenu.h"
#include "Game/FE/feTextureResource.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlImageInstance.h"
#include "Game/FE/tlTextInstance.h"
#include "Game/GameInfo.h"
#include "Game/NetworkSession.h"
#include "Game/Render/Presentation.h"
#include "Game/FriendManager.h"
#include "NL/nlBind.h"
#include "NL/nlFunction.inl"
#include "NL/nlFormat.h"
#include "NL/nlLocalizationLookup.h"
#include "NL/nlPrint.h"
#include "NL/nlstring_tmpl.h"
#include "Game/FE/feDPD.h"
#include "Game/SH/SHNavigation.h"
#include "Game/FE/feOnlineError.h"
#include "Game/MiiManager.h"

#include <stdlib.h>
#include "Game/FE/UnidentifiedTLDefault.h"

static const int sFriendStatusSortOrder[12] = { 5, 4, 0, 3, 6, 7, 8, 10, 9, 1, 2, 11 };
typedef BasicString<unsigned short, Detail::TempStringAllocator> WideString;

SHOnlineFriends::SHOnlineFriends()
    : mUnidentified001C(64)
    , mUnidentified0020(0)
    , mUnidentified0024(0)
    , mUnidentified0028(0)
    , mUnidentified002C(0)
    , mUnidentified0030(false)
    , mUnidentified0031(false)
    , mUnidentified0034(0.5f)
    , mUnidentified2FA8(false)
    , mUnidentified2FAC(0)
{
    for (int i = 0; i < 4; ++i)
        mUnidentified0038[i].mContext = (void*)i;
    for (int i = 0; i < 64; ++i)
        mUnidentified2EA8[i] = &mUnidentified08A8[i];
    mUnidentified04BC.SetPopScene(false);
}

SHOnlineFriends::~SHOnlineFriends()
{
    SetOnlineFriendSelectionMode(false);
}

void SHOnlineFriends::UpdateScrollRange()
{
    mUnidentified0024 = 0;
    for (int i = 0; i < 64; ++i)
    {
        if (!mUnidentified2EA8[i]->mVisible)
            break;
        ++mUnidentified0024;
    }
    if (!IsOnlineFriendSelectionMode())
        ++mUnidentified0024;
    if (mUnidentified0024 > 4)
        mUnidentified0024 -= 4;
    else
        mUnidentified0024 = 0;
    mUnidentified0308.ResetScrolling();
    mUnidentified0308.SetRange(mUnidentified0024);
    mUnidentified0308.SetValue(mUnidentified0020);
}

void SHOnlineFriends::UpdateFriend(int index)
{
    FEOnlinePlayerRow* row = &mUnidentified08A8[index];
    DWCAccFriendData* data = (DWCAccFriendData*)GameInfoManager::Instance()->GetUnknown0x40(gNetworkSaveSlotIndex, index);
    u16* name = GameInfoManager::Instance()->GetSavedFriendName(gNetworkSaveSlotIndex, index);
    row->mFriendIndex = index;
    int type = DWC_GetFriendDataType(data);
    if (DWC_IsValidFriendData(data) && type != 0)
    {
        row->mVisible = true;
    }
    else
    {
        row->mVisible = false;
        return;
    }
    char status[256];
    switch (DWC_GetFriendStatus((DWCFriendData*)data, status))
    {
    case 0:
        row->mStatus = 8;
        break;
    case 1:
        if (g_pFriendManager->GetFriendStatusPayload(index)->mStatus == 1)
        {
            if (row->mStatus != 2)
                FEAudio::PlayAnimAudioEvent(0xCC2C93F1, 0, 0, 1);
            row->mStatus = 2;
        }
        else
            row->mStatus = 3;
        break;
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
        row->mStatus = 3;
        break;
    }
    if (type == 1 || type == 2)
        row->mStatus = 7;
    NetworkLeaderboardCategory* category = NetworkStatsManager::Instance()->GetCategory(4);
    int player = category->FindPlayer(((int*)data)[1]);
    if (player != -1)
    {
        row->mStats = category->mMetadata[player];
        memcpy(row->mMiiData, category->mPlayers[player].mData, sizeof(row->mMiiData));
    }
    else
        row->mStats.Reset();
    nlStrNCpy(row->mName, name, 14);
}

int SHOnlineFriends::CompareFriendStatus(const void* a, const void* b)
{
    const FEOnlinePlayerRow* first = *(const FEOnlinePlayerRow* const*)a;
    const FEOnlinePlayerRow* second = *(const FEOnlinePlayerRow* const*)b;
    int secondOrder = sFriendStatusSortOrder[second->mStatus];
    int firstOrder = sFriendStatusSortOrder[first->mStatus];
    if (firstOrder > secondOrder)
        return 1;
    return firstOrder < secondOrder ? -1 : 0;
}

void SHOnlineFriends::InitializeButtons()
{
    FEPointerListener::Callback over(Bind<void>(MemFun(&SHOnlineFriends::OnPointerEnter), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback off(Bind<void>(MemFun(&SHOnlineFriends::OnPointerLeave), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback down(Bind<void>(MemFun(&SHOnlineFriends::OnPointerPress), this, Placeholder<0>(), Placeholder<1>()));
    for (int i = 0; i < 4; ++i)
    {
        mUnidentified0038[i].SetInstanceBounds(mUnidentified0618[i], true, -24.0f, 10.0f, 0.7f, 0.55f);
        mUnidentified0038[i].SetPointerEnterCallback(over);
        mUnidentified0038[i].SetPointerLeaveCallback(off);
        mUnidentified0038[i].SetPointerPressCallback(down);
    }
    if (!mUnidentified0308.mInitialized)
        mUnidentified0308.Initialize();
}

void SHOnlineFriends::OnPointerPress(int index, void* context)
{
    int item = (int)context;
    mUnidentified002C = item;
    mUnidentified0031 = true;
    FEAudio::PlayAnimAudioEvent(0xF0AFD586, 0, 0, 1);
    bool change = false;
    if (item == 0 && !IsOnlineFriendSelectionMode())
    {
        if (g_pFriendManager->CountFriends() >= 64)
        {
            if (GameSceneManager::Instance()->GetSceneType(GameSceneManager::Instance()->GetCurrentScene()) != 10)
            {
                FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push((SceneList)10, SCREEN_NOTHING, false);
                popup->Create((ePopupMenu)104, Bind<void>(MemFun(&SHOnlineFriends::OnDialogDismissed), this));
                mUnidentified2FA8 = true;
            }
        }
        else
            change = true;
    }
    else if (!IsOnlineFriendSelectionMode())
    {
        g_pFriendManager->SetOwnStatusInitial(0);
        if (GameSceneManager::Instance()->GetSceneType(GameSceneManager::Instance()->GetCurrentScene()) != 10)
        {
            FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push((SceneList)10, SCREEN_NOTHING, false);
            popup->Create((ePopupMenu)106,
                Bind<void>(MemFun(&SHOnlineFriends::DeleteFriend), this, item),
                Bind<void>(MemFun(&SHOnlineFriends::CancelDeleteFriend), this));
            mUnidentified2FA8 = true;
        }
    }
    else
    {
        int selected = item + mUnidentified0020;
        if (!IsOnlineFriendSelectionMode())
            --selected;
        u8 region = *(u8*)GameInfoManager::Instance()->GetUnknown0xA40(
            gNetworkSaveSlotIndex, mUnidentified2EA8[selected]->mFriendIndex);
        if (region != GetOnlineRegion())
        {
            if (GameSceneManager::Instance()->GetSceneType(GameSceneManager::Instance()->GetCurrentScene()) != 10)
            {
                FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push((SceneList)10, SCREEN_NOTHING, false);
                popup->Create((ePopupMenu)105, Bind<void>(MemFun(&SHOnlineFriends::OnDialogDismissed), this));
                mUnidentified2FA8 = true;
            }
        }
        else
            change = true;
    }
    if (change)
    {
        mUnidentified2FAC = 2;
        SHNavigation* scene = GetNavigationScene();
        if (scene != 0)
            scene->HideButtons();
        for (int i = 0; i < 4; ++i)
            gFEPointerInstances[i]->SetActiveSlide("waiting", true, false);
        mPresentation->SetActiveSlide("out", true);
        mPresentation->Update(0.0f);
    }
}

void SHOnlineFriends::OnPointerEnter(int index, void* context)
{
    int item = (int)context;
    ++mUnidentified0028;
    mUnidentified0618[item]->SetActiveSlide("over", true, false);
    mUnidentified0038[item].SetPointerState(1, index);
    FEAudio::PlayAnimAudioEvent(0xF6EB899E, 0, 0, 1);
}

void SHOnlineFriends::OnPointerLeave(int index, void* context)
{
    int item = (int)context;
    --mUnidentified0028;
    mUnidentified0618[item]->SetActiveSlide("off", true, false);
    mUnidentified0038[item].SetPointerState(0, index);
}

inline void SHOnlineFriends::UpdateVisibleRows()
{
    for (int i = 0; i < 4; ++i)
    {
        if (i == 0 && !IsOnlineFriendSelectionMode())
        {
            UpdateAddFriendRow();
            continue;
        }
        int selected = i + mUnidentified0020;
        if (!IsOnlineFriendSelectionMode())
            --selected;
        UpdateOnlinePlayerRow(mUnidentified2EA8[selected], mUnidentified0618[i], mUnidentified0628[i], 32, mUnidentified0728[i], 48, i, mUnidentified0030);
        if (!mUnidentified2EA8[selected]->mVisible || (IsOnlineFriendSelectionMode() && mUnidentified2EA8[selected]->mStatus != 2))
            mUnidentified0038[i].Disable();
        else
            mUnidentified0038[i].Enable();
    }
}

static inline void RefreshFriends(SHOnlineFriends* self)
{
    for (int i = 0; i < 64; ++i)
        self->UpdateFriend(i);
    for (int i = 0; i < 64; ++i)
        self->mUnidentified2EA8[i] = &self->mUnidentified08A8[i];
    qsort(self->mUnidentified2EA8, 64, sizeof(self->mUnidentified2EA8[0]), SHOnlineFriends::CompareFriendStatus);
}

void SHOnlineFriends::DeleteFriend(int index)
{
    mUnidentified2FA8 = false;
    int selected = index + mUnidentified0020;
    if (!IsOnlineFriendSelectionMode())
        --selected;
    g_pFriendManager->DeleteFriend(mUnidentified2EA8[selected]->mFriendIndex);
    mUnidentified2EA8[selected]->mStatus = 11;
    if (mUnidentified0020 == mUnidentified0024 && mUnidentified0020 > 0)
        --mUnidentified0020;
    RefreshFriends(this);
    UpdateScrollRange();
    UpdateVisibleRows();
    g_pFriendManager->SetOwnStatusInitial(1);
}

void SHOnlineFriends::CancelDeleteFriend()
{
    mUnidentified2FA8 = false;
    g_pFriendManager->SetOwnStatusInitial(1);
}

void SHOnlineFriends::UpdateFriendCode()
{
    WideString format;
    WideString string;
    u16 friendKey[14];
    g_pFriendManager->GetOwnFriendKeyString(friendKey);
    nlStrNCpy(mUnidentified0598, friendKey, 14);
    format = WideString(LookupLocString("ONLINE_FRIEND_CODE_YOURS"));
    string = Format(format, mUnidentified0598);
    memcpy(mUnidentified0598, string.c_str(), sizeof(mUnidentified0598));
    mUnidentified0594->SetString(mUnidentified0598);
}

void SHOnlineFriends::OnDialogDismissed()
{
    mUnidentified2FA8 = false;
}

void SHOnlineFriends::OnErrorDismissed()
{
    mUnidentified2FA8 = false;
    GameSceneManager::Instance()->Pop();
    FEAudio::PlayAnimAudioEvent(0x4430B152, 0, 0, 1);
    Presentation::GetInstance()->Call("TransitionOnlineMatchToMainMenu");
}

void SHOnlineFriends::SceneCreated()
{
    TLTextInstance* text = FEFinder<TLTextInstance, 3>::Find(mPresentation->m_currentSlide,
        InlineHasher("Layer"),
        InlineHasher("Group"),
        InlineHasher("FRIEND CODE"));
    mUnidentified0594 = text != 0 ? text : &UnidentifiedTLTextDefault::sInstance;
    for (int i = 0; i < 4; ++i)
    {
        char name[9];
        nlSNPrintf(name, sizeof(name), "FRIEND_%d", i);
        TLComponentInstance* instance = FEFinder<TLComponentInstance, 4>::Find(mPresentation->m_currentSlide,
            InlineHasher("Layer"),
            InlineHasher("Group"),
            InlineHasher(name));
        mUnidentified0618[i] = instance != 0 ? instance : &UnidentifiedTLComponentDefault::sInstance;
    }
    TLComponentInstance* title = FEFinder<TLComponentInstance, 4>::Find(mPresentation->m_currentSlide,
        InlineHasher("Layer"),
        InlineHasher("Group"),
        InlineHasher("TITLE2"));
    if (title == 0)
        title = &UnidentifiedTLComponentDefault::sInstance;
    if (IsOnlineFriendSelectionMode())
    {
        title->SetActiveSlide("friends2", true, false);
        mUnidentified0594->m_bVisible = false;
    }
    else
    {
        title->SetActiveSlide("friends", true, false);
        UpdateFriendCode();
    }
    RefreshFriends(this);
    UpdateVisibleRows();
    TLComponentInstance* scrollbar = FEFinder<TLComponentInstance, 4>::Find(mPresentation->m_currentSlide,
        InlineHasher("Layer"),
        InlineHasher("Group"),
        InlineHasher("scrollbar"));
    if (scrollbar == 0)
        scrollbar = &UnidentifiedTLComponentDefault::sInstance;
    mUnidentified0308.SetComponent(scrollbar);
    mUnidentified0308.SetValue(mUnidentified0020);
    UpdateScrollRange();
    for (int i = 0; i < 4; ++i)
        gFEPointerInstances[i]->SetActiveSlide("waiting", true, false);
    SHNavigation* scene = GetNavigationScene();
    TLComponentInstance* done = 0;
    if (scene != 0)
    {
        scene->SetButtons(0, true);
        done = scene->GetButton(4);
    }
    mUnidentified04BC.SetButtonInstance(done);
    FEAudio::PlayAnimAudioEvent(0xBB142B94, 0, 0, 1);
}

void SHOnlineFriends::UpdateAddFriendRow()
{
    const char* slides[2] = { "off", "over" };
    TLInstance* views[2];
    for (int i = 0; i < 2; ++i)
    {
        views[i] = FEFinder<TLComponentInstance, 4>::Find(mUnidentified0618[0],
            nlStringLowerHash(slides[i]),
            nlStringLowerHash("FRIEND_0"),
            0,
            0,
            0,
            0);
        if (views[i] == 0)
            views[i] = &UnidentifiedTLGroupDefault::sInstance;
    }
    for (int i = 0; i < 2; ++i)
    {
        TLComponentInstance* component = FEFinder<TLComponentInstance, 4>::Find(views[i], InlineHasher("cancel"));
        if (component == 0)
            component = &UnidentifiedTLComponentDefault::sInstance;
        component->m_bVisible = false;
    }
    for (int i = 0; i < 2; ++i)
    {
        TLComponentInstance* component = FEFinder<TLComponentInstance, 4>::Find(views[i], InlineHasher("SEARCHING_ADD"));
        if (component == 0)
            component = &UnidentifiedTLComponentDefault::sInstance;
        component->SetActiveSlide("ADD", true, false);
        component->m_bVisible = true;
    }
    for (int i = 0; i < 2; ++i)
    {
        TLTextInstance* name = FEFinder<TLTextInstance, 3>::Find(views[i], InlineHasher("NAME"));
        if (name == 0)
            name = &UnidentifiedTLTextDefault::sInstance;
        name->m_bVisible = false;
    }
    const char* hidden[] = { "STATUS", "GUEST_HOME_AWAY", "PLAYER CLASS" };
    for (int j = 0; j < 3; ++j)
    {
        for (int i = 0; i < 2; ++i)
        {
            TLComponentInstance* component = FEFinder<TLComponentInstance, 4>::Find(views[i], InlineHasher(hidden[j]));
            if (component == 0)
                component = &UnidentifiedTLComponentDefault::sInstance;
            component->m_bVisible = false;
        }
    }
    const char* stats[2] = { "RANK", "RECORD" };
    for (int j = 0; j < 2; ++j)
    {
        for (int i = 0; i < 2; ++i)
        {
            TLTextInstance* text = FEFinder<TLTextInstance, 3>::Find(views[i],
                InlineHasher("STATS"),
                InlineHasher("Slide1"),
                InlineHasher(stats[j]),
                InlineHasher(stats[j]));
            if (text == 0)
                text = &UnidentifiedTLTextDefault::sInstance;
            text->m_bVisible = false;
        }
    }
    for (int i = 0; i < 2; ++i)
    {
        TLImageInstance* image = FEFinder<TLImageInstance, 2>::Find(views[i], InlineHasher("00_dummy_texture"));
        if (image == 0)
            image = &UnidentifiedTLImageDefault::sInstance;
        image->m_bVisible = false;
    }
    const char* images[] = { "Mii", "logo_32x32", "shoulders", "Online_Mii_select_background" };
    for (int j = 0; j < 4; ++j)
    {
        for (int i = 0; i < 2; ++i)
        {
            TLImageInstance* image = FEFinder<TLImageInstance, 2>::Find(views[i],
                InlineHasher("Mii_btn"),
                InlineHasher(images[j]));
            if (image == 0)
                image = &UnidentifiedTLImageDefault::sInstance;
            image->m_bVisible = false;
            image->SetAssetVisible(false);
        }
    }
}

static const char* const sOnlinePlayerSearchSlides[5] = { "SEARCHING", "CONNECTING", "INVITE", "ADD", "OFF" };
static const char* const sOnlinePlayerStatusSlides[12] = {
    "WAITING FOR FRIEND", "WAITING_FOR_OPPONENT", "AVAILABLE", "BUSY", "CHOOSING_CAPTAIN", "CHOOSING_SIDEKICKS", "CAPTAINNAMES", "ESTABLISHING", "OFFLINE", "DECLINED", "INVITING", "OFFLINE"
};
static const char* const sOnlinePlayerSideSlides[4] = { "HOME", "HOME", "AWAY", "GUEST" };

const char* GetOnlineCaptainSlideName(unsigned int captain)
{
    switch (captain)
    {
    case 0:
        return "MARIO";
    case 1:
        return "BOWSER";
    case 2:
        return "DAISY";
    case 3:
        return "DONKEYKONG";
    case 4:
        return "LUIGI";
    case 5:
        return "PEACH";
    case 6:
        return "WALUIGI";
    case 7:
        return "WARIO";
    case 8:
        return "YOSHI";
    case 9:
        return "BOWSERJR";
    case 10:
        return "DIDDYKONG";
    case 11:
        return "PETEY";
    default:
        return 0;
    }
}

void UpdateOnlinePlayerRow(FEOnlinePlayerRow* row, TLComponentInstance* instance,
    u16* name, int nameSize, u16* description, int descriptionSize, int index, bool value)
{
    TLInstance* views[2];
    views[0] = FEFinder<TLComponentInstance, 4>::Find(instance,
        nlStringLowerHash("off"),
        nlStringLowerHash("FRIEND_0"),
        0,
        0,
        0,
        0);
    views[1] = FEFinder<TLComponentInstance, 4>::Find(instance,
        nlStringLowerHash("over"),
        nlStringLowerHash("FRIEND_0"),
        0,
        0,
        0,
        0);
    for (int i = 0; i < 2; ++i)
    {
        if (views[i] == 0)
            views[i] = &UnidentifiedTLGroupDefault::sInstance;
        views[i]->m_bVisible = row->mVisible;
    }
    for (int i = 0; i < 2; ++i)
    {
        TLComponentInstance* cancel = FEFinder<TLComponentInstance, 4>::Find(views[i], InlineHasher("cancel"));
        if (cancel == 0)
            cancel = &UnidentifiedTLComponentDefault::sInstance;
        cancel->m_bVisible = row->mShowCancel;
    }
    for (int i = 0; i < 2; ++i)
    {
        TLComponentInstance* searching = FEFinder<TLComponentInstance, 4>::Find(views[i], InlineHasher("SEARCHING_ADD"));
        if (searching == 0)
            searching = &UnidentifiedTLComponentDefault::sInstance;
        searching->SetActiveSlide(sOnlinePlayerSearchSlides[row->mSearchState], false, false);
    }
    for (int i = 0; i < 2; ++i)
    {
        TLTextInstance* nameText = FEFinder<TLTextInstance, 3>::Find(views[i], InlineHasher("NAME"));
        if (nameText == 0)
            nameText = &UnidentifiedTLTextDefault::sInstance;
        nameText->SetString(row->mName);
        nameText->m_bVisible = row->mSearchState == 4;
    }
    for (int i = 0; i < 2; ++i)
    {
        TLComponentInstance* status = FEFinder<TLComponentInstance, 4>::Find(views[i], InlineHasher("STATUS"));
        if (status == 0)
            status = &UnidentifiedTLComponentDefault::sInstance;
        status->SetActiveSlide(sOnlinePlayerStatusSlides[row->mStatus], false, false);
        status->m_bVisible = row->mSearchState == 4;
        if (row->mStatus == 6)
        {
            TLComponentInstance* names = FEFinder<TLComponentInstance, 4>::FindOrDefault(status, InlineHasher("NAMES"), InlineHasher(0UL), InlineHasher(0UL), InlineHasher(0UL), InlineHasher(0UL), InlineHasher(0UL));
            names->SetActiveSlide(GetOnlineCaptainSlideName(row->mCaptain), false, false);
        }
    }
    bool show = row->mSearchState == 4
             && row->mStatus != 7 && row->mStatus != 9 && row->mStatus != 10;
    for (int i = 0; i < 2; ++i)
    {
        TLComponentInstance* guest = FEFinder<TLComponentInstance, 4>::Find(views[i], InlineHasher("GUEST_HOME_AWAY"));
        if (guest == 0)
            guest = &UnidentifiedTLComponentDefault::sInstance;
        guest->SetActiveSlide(sOnlinePlayerSideSlides[row->mSide], true, false);
        guest->m_bVisible = show && row->mSide != 0;
    }
    for (int i = 0; i < 2; ++i)
    {
        TLComponentInstance* playerClass = FEFinder<TLComponentInstance, 4>::Find(views[i], InlineHasher("PLAYER CLASS"));
        if (playerClass == 0)
            playerClass = &UnidentifiedTLComponentDefault::sInstance;
        playerClass->m_bVisible = false;
    }
    WideString string = Format(WideString(LookupLocString("ONLINE_RANKING")), row->mStats.mDisplayRank);
    nlStrNCpy(name, string.c_str(), nameSize);
    for (int i = 0; i < 2; ++i)
    {
        TLTextInstance* rank = FEFinder<TLTextInstance, 3>::Find(views[i],
            InlineHasher("STATS"),
            InlineHasher("Slide1"),
            InlineHasher("RANK"),
            InlineHasher("RANK"));
        if (rank == 0)
            rank = &UnidentifiedTLTextDefault::sInstance;
        rank->SetString(name);
        rank->m_bVisible = show && !row->mGuest;
    }
    string = Format(WideString(LookupLocString("ONLINE_SLOT_STATS")), row->mStats.mWins, row->mStats.mLosses, row->mStats.mScore);
    nlStrNCpy(description, string.c_str(), descriptionSize);
    for (int i = 0; i < 2; ++i)
    {
        TLTextInstance* record = FEFinder<TLTextInstance, 3>::Find(views[i],
            InlineHasher("STATS"),
            InlineHasher("Slide1"),
            InlineHasher("RECORD"),
            InlineHasher("RECORD"));
        if (record == 0)
            record = &UnidentifiedTLTextDefault::sInstance;
        record->SetString(description);
        record->m_bVisible = show && !row->mGuest;
    }
    for (int i = 0; i < 2; ++i)
    {
        TLImageInstance* dummy = FEFinder<TLImageInstance, 2>::Find(views[i], InlineHasher("00_dummy_texture"));
        if (dummy == 0)
            dummy = &UnidentifiedTLImageDefault::sInstance;
        dummy->m_bVisible = show;
    }
    for (int i = 0; i < 2; ++i)
    {
        TLImageInstance* background = FEFinder<TLImageInstance, 2>::Find(views[i],
            InlineHasher("Mii_btn"),
            InlineHasher("Online_Mii_select_background"));
        if (background == 0)
            background = &UnidentifiedTLImageDefault::sInstance;
        background->SetAssetVisible(show);
    }
    bool valid = MiiManager::Instance()->CreateIcon((const RFLStoreData*)row->mMiiData, index, (RFLExpression)0);
    unsigned long texture = MiiManager::Instance()->GetIconTextureId(index);
    nlColour colour = { 0, 0, 0, 255 };
    for (int i = 0; i < 2; ++i)
    {
        TLImageInstance* image = FEFinder<TLImageInstance, 2>::Find(views[i], InlineHasher("Mii_btn"), InlineHasher("Mii"));
        image->m_pTextureResource->SetTextureHandle(texture);
        image->SetAssetVisible(valid && show && value);
        image->m_bVisible = valid && show && value;
        if (row->mGuest)
            image->SetAssetColour(colour);
    }
    for (int i = 0; i < 2; ++i)
    {
        TLImageInstance* shoulders = FEFinder<TLImageInstance, 2>::Find(views[i], InlineHasher("Mii_btn"), InlineHasher("shoulders"));
        shoulders->SetAssetVisible(false);
        shoulders->m_bVisible = false;
    }
}

void SHOnlineFriends::Update(float dt)
{
    BaseSceneHandler::Update(dt);
    mUnidentified0031 = false;
    if (mUnidentified2FA8 && !g_pFEInput->HasInputLock(this))
        return;
    if (mUnidentified2FAC == 0 || mUnidentified2FAC == 2 || mUnidentified2FAC == 3)
    {
        TLSlide* slide = mPresentation->m_currentSlide;
        if (slide->m_time < slide->m_start + slide->m_duration)
        {
            for (int i = 0; i < 4; ++i)
                GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
            return;
        }
        if (mUnidentified2FAC == 0)
        {
            SHNavigation* scene = GetNavigationScene();
            if (scene != 0)
                scene->SetButtons(4, true);
            mUnidentified2FAC = 1;
            InitializeButtons();
            mUnidentified0030 = true;
        }
        else if (mUnidentified2FAC == 2)
        {
            if (!IsOnlineFriendSelectionMode())
                GameSceneManager::Instance()->Push(SCENE_ONLINE_FRIEND_CODE_ENTRY, SCREEN_FORWARD, true);
            else
            {
                SHOnlineInvitePlayers* scene = (SHOnlineInvitePlayers*)GameSceneManager::Instance()->Push((SceneList)44, SCREEN_FORWARD, true);
                scene->mIsHost = true;
                scene->mStartFriendServer = false;
                SetOnlineFriendSelectionMode(false);
                GameInfoManager* gameInfo = GameInfoManager::Instance();
                int selected = mUnidentified2EA8[mUnidentified002C + mUnidentified0020]->mFriendIndex;
                u8 stadium = gameInfo->GetStadium();
                const CheatSettings* rules = reinterpret_cast<const CheatSettings*>(gameInfo->GetActiveRules());
                const GameplaySettings* settings = reinterpret_cast<const GameplaySettings*>(gameInfo->GetCurrentSettings());
                g_pFriendManager->SetOwnStatusHostInvitingPlayer(selected, settings, rules, stadium);
            }
            return;
        }
        else if (mUnidentified2FAC == 3)
        {
            if (IsOnlineFriendSelectionMode())
            {
                GameSceneManager::Instance()->Push((SceneList)44, SCREEN_NOTHING, true);
                SetOnlineFriendSelectionMode(false);
            }
            else
            {
                GameSceneManager::Instance()->Push((SceneList)40, SCREEN_NOTHING, true);
                FEAudio::PlayAnimAudioEvent(0x37A9934D, 0, 0, 1);
            }
            return;
        }
    }
    if (!IsOnlineFriendSelectionMode() && !GameSceneManager::Instance()->IsOnStack((SceneList)10) && g_pFriendManager->FindHostInvitation())
    {
        g_pFriendManager->mReturnScene = 47;
        g_pFriendManager->mPreviousRankedMode = 0;
        GameSceneManager::Instance()->Push((SceneList)52, SCREEN_FORWARD, true);
        return;
    }
    if (!NetworkStatsManager::Instance()->RefreshFriendStats_80131B50())
    {
        if (g_pNetworkSession->mDWCLastError == 0)
            g_pNetworkSession->ReadAndClearDWCError();
        int error = GetOnlineErrorPopup(g_pNetworkSession->mDWCErrorCode, true, 111);
        if (GameSceneManager::Instance()->GetSceneType(GameSceneManager::Instance()->GetCurrentScene()) != 10)
        {
            FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push((SceneList)10, SCREEN_NOTHING, false);
            popup->Create((ePopupMenu)error, Bind<void>(MemFun(&SHOnlineFriends::OnErrorDismissed), this));
            mUnidentified2FA8 = true;
        }
        return;
    }
    mUnidentified0034 -= dt;
    if (mUnidentified0034 <= 0.0f)
    {
        RefreshFriends(this);
        UpdateVisibleRows();
        mUnidentified0034 = 0.5f;
    }
    for (int i = 0; i < 4; ++i)
    {
        TLComponentInstance* controller = GetPointerInstance(i);
        if ((unsigned int)i != gFEControllerIndex)
        {
            controller->SetActiveSlide("waiting", true, false);
            continue;
        }
        controller->SetActiveSlide("cursor", true, false);
        u8 valid = true;
        FEPointerEvent event;
        event.mIndex = i;
        event.mPosition = GetPointerPosition(i, &valid);
        event.mPressed = g_pFEInput->JustPressed((eFEINPUT_PAD)i, 30, true, 0);
        event.mReleased = g_pFEInput->JustReleased((eFEINPUT_PAD)i, 30, true, 0);
        mUnidentified0308.Update(event, dt);
        for (int j = 0; j < 4; ++j)
            mUnidentified0038[j].HandlePointerEvent(&event);
        if (mUnidentified0031)
            return;
        if (mUnidentified04BC.UpdateBackButton(event, dt))
        {
            for (int j = 0; j < 4; ++j)
                GetPointerInstance(j)->SetActiveSlide("waiting", true, false);
            mUnidentified2FAC = 3;
            SHNavigation* scene = GetNavigationScene();
            if (scene != 0)
                scene->HideButtons();
            mPresentation->SetActiveSlide("out", true);
            mPresentation->Update(0.0f);
            return;
        }
    }
    if (mUnidentified0308.IsScrolling(1, 1))
    {
        ++mUnidentified0020;
        UpdateVisibleRows();
    }
    else if (mUnidentified0308.IsScrolling(0, 1))
    {
        --mUnidentified0020;
        UpdateVisibleRows();
    }
}

#include "Game/MiiManager.inl"

#include "Game/FE/feFinderDefault_impl.h"
