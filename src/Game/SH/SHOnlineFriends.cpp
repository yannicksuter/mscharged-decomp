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
#include "Game/FE/feInlineHasher.h"
#include "Game/FE/feFinderFind_impl.h"
#include "Game/FE/feFinderDefault_impl.h"
#include "Game/FE/feInput.h"
#include "Game/FE/fePopupMenu.h"
#include "Game/FE/feTextureResource.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlImageInstance.h"
#include "Game/FE/tlTextInstance.h"
#include "Game/FE/fePointer.inl"
#include "Game/GameInfo.h"
#include "Game/NetworkSession.h"
#include "Game/Render/FrontEndPresentation.h"
#include "Game/FriendManager.h"
#include "Game/GameInfo.inl"
#include "NL/nlBind.h"
#include "NL/nlFunction.inl"
#include "NL/nlFormat.h"
#include "NL/nlLocalizationLookup.h"
#include "NL/nlPrint.h"
#include "NL/nlstring_tmpl.h"
#include "Game/FE/feDPD.h"
#include "Game/SH/SHNavigation.h"
#include "Game/FE/feOnlineError.h"

#include <stdlib.h>
#include "Game/FE/UnidentifiedTLDefault.h"

typedef BasicString<unsigned short, Detail::TempStringAllocator> WideString;

SHOnlineFriends::SHOnlineFriends()
    : mUnidentified001C(64)
    , mScrollOffset(0)
    , mScrollRange(0)
    , mUnidentified0028(0)
    , mPressedItem(0)
    , mInitialized(false)
    , mPressHandled(false)
    , mRefreshTimer(0.5f)
    , mPopupActive(false)
    , mUnidentified2FAC(0)
{
    for (int i = 0; i < 4; ++i)
        mRowButtons[i].mContext = (void*)i;
    for (int i = 0; i < 64; ++i)
        mSortedFriendRows[i] = &mFriendRows[i];
    mBackButton.SetPopScene(false);
}

SHOnlineFriends::~SHOnlineFriends()
{
    SetOnlineFriendSelectionMode(false);
}

void SHOnlineFriends::UpdateScrollRange()
{
    mScrollRange = 0;
    for (int i = 0; i < 64; ++i)
    {
        if (!mSortedFriendRows[i]->mVisible)
            break;
        ++mScrollRange;
    }
    if (!IsOnlineFriendSelectionMode())
        ++mScrollRange;
    if (mScrollRange > 4)
        mScrollRange -= 4;
    else
        mScrollRange = 0;
    mScrollBar.ResetScrolling();
    mScrollBar.SetRange(mScrollRange);
    mScrollBar.SetValue(mScrollOffset);
}

void SHOnlineFriends::UpdateFriend(int index)
{
    FEOnlinePlayerRow* row = &mFriendRows[index];
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
        if (g_pFriendManager->GetFriendStatusPayload(index)->mHeader.mStatus == 1)
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

inline void SHOnlineFriends::UpdateVisibleRows()
{
    for (int i = 0; i < 4; ++i)
    {
        if (i == 0 && !IsOnlineFriendSelectionMode())
        {
            UpdateAddFriendRow();
            continue;
        }
        int selected = i + mScrollOffset;
        if (!IsOnlineFriendSelectionMode())
            --selected;
        UpdateOnlinePlayerRow(mSortedFriendRows[selected], mRowInstances[i], mRankText[i], 32, mRecordText[i], 48, i, mInitialized);
        if (!mSortedFriendRows[selected]->mVisible || (IsOnlineFriendSelectionMode() && mSortedFriendRows[selected]->mStatus != 2))
            mRowButtons[i].Disable();
        else
            mRowButtons[i].Enable();
    }
}

static inline void RefreshFriends(SHOnlineFriends* self)
{
    for (int i = 0; i < 64; ++i)
        self->UpdateFriend(i);
    for (int i = 0; i < 64; ++i)
        self->mSortedFriendRows[i] = &self->mFriendRows[i];
    qsort(self->mSortedFriendRows, 64, sizeof(self->mSortedFriendRows[0]), SHOnlineFriends::CompareFriendStatus);
}

void SHOnlineFriends::SceneCreated()
{
    mFriendCodeText = FEFinder<TLTextInstance, 3>::FindOrDefault(
        mPresentation->m_currentSlide, "Layer", "Group", "FRIEND CODE");
    for (int i = 0; i < 4; ++i)
    {
        char name[9];
        nlSNPrintf(name, sizeof(name), "FRIEND_%d", i);
        mRowInstances[i] = FEFinder<TLComponentInstance, 4>::FindOrDefault(
            mPresentation->m_currentSlide, "Layer", "Group", name);
    }
    TLComponentInstance* title = FEFinder<TLComponentInstance, 4>::FindOrDefault(
        mPresentation->m_currentSlide, "Layer", "Group", "TITLE2");
    if (IsOnlineFriendSelectionMode())
    {
        title->SetActiveSlide("friends2", true, false);
        mFriendCodeText->m_bVisible = false;
    }
    else
    {
        title->SetActiveSlide("friends", true, false);
        UpdateFriendCode();
    }
    RefreshFriends(this);
    UpdateVisibleRows();
    TLComponentInstance* scrollbar = FEFinder<TLComponentInstance, 4>::FindOrDefault(
        mPresentation->m_currentSlide, "Layer", "Group", "scrollbar");
    mScrollBar.SetComponent(scrollbar);
    mScrollBar.SetValue(mScrollOffset);
    UpdateScrollRange();
    for (int i = 0; i < 4; ++i)
        GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
    SHNavigation* scene = GetNavigationScene();
    TLComponentInstance* done = 0;
    if (scene != 0)
    {
        scene->SetButtons(0, true);
        done = scene->GetButton(4);
    }
    mBackButton.SetButtonInstance(done);
    FEAudio::PlayAnimAudioEvent(0xBB142B94, 0, 0, 1);
}

void SHOnlineFriends::Update(float dt)
{
    BaseSceneHandler::Update(dt);
    mPressHandled = false;
    if (mPopupActive && !g_pFEInput->HasInputLock(this))
        return;
    if (mUnidentified2FAC == 0 || mUnidentified2FAC == 2 || mUnidentified2FAC == 3)
    {
        TLSlide* slide = mPresentation->m_currentSlide;
        if (slide->GetCurrentTime() < slide->GetStartTime() + slide->GetDuration())
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
            mInitialized = true;
        }
        else if (mUnidentified2FAC == 2)
        {
            if (!IsOnlineFriendSelectionMode())
                GameSceneManager::Instance()->Push(SCENE_ONLINE_FRIEND_CODE_ENTRY, SCREEN_FORWARD, true);
            else
                StartFriendInvite();
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
        FriendManager* friendManager = g_pFriendManager;
        friendManager->mReturnScene = 47;
        friendManager->mPreviousRankedMode = 0;
        GameSceneManager::Instance()->Push((SceneList)52, SCREEN_FORWARD, true);
        return;
    }
    if (!NetworkStatsManager::Instance()->RefreshFriendStats_80131B50())
    {
        if (g_pNetworkSession->mDWCLastError == 0)
            g_pNetworkSession->ReadAndClearDWCError();
        ShowError(GetOnlineErrorPopup(g_pNetworkSession->mDWCErrorCode, true, 111));
        return;
    }
    mRefreshTimer -= dt;
    if (mRefreshTimer <= 0.0f)
    {
        RefreshFriends(this);
        UpdateVisibleRows();
        mRefreshTimer = 0.5f;
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
        mScrollBar.Update(event, dt);
        for (int j = 0; j < 4; ++j)
            mRowButtons[j].HandlePointerEvent(&event);
        if (mPressHandled)
            return;
        if (mBackButton.UpdateBackButton(event, dt))
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
    if (mScrollBar.IsScrolling(1, 1))
    {
        ++mScrollOffset;
        UpdateVisibleRows();
    }
    else if (mScrollBar.IsScrolling(0, 1))
    {
        --mScrollOffset;
        UpdateVisibleRows();
    }
}

void SHOnlineFriends::UpdateAddFriendRow()
{
    TLInstance* off = FEFinder<TLInstance, 5>::FindOrDefault(mRowInstances[0], "off", "FRIEND_0");
    TLInstance* over = FEFinder<TLInstance, 5>::FindOrDefault(mRowInstances[0], "over", "FRIEND_0");

    FEFinder<TLInstance, 4>::FindOrDefault(off, "cancel")->SetVisible(false);
    FEFinder<TLInstance, 4>::FindOrDefault(over, "cancel")->SetVisible(false);

    TLComponentInstance* addOff = (TLComponentInstance*)FEFinder<TLInstance, 4>::FindOrDefault(off, "SEARCHING_ADD");
    addOff->SetActiveSlide("ADD", true, false);
    addOff->SetVisible(true);
    TLComponentInstance* addOver = (TLComponentInstance*)FEFinder<TLInstance, 4>::FindOrDefault(over, "SEARCHING_ADD");
    addOver->SetActiveSlide("ADD", true, false);
    addOver->SetVisible(true);

    FEFinder<TLTextInstance, 3>::FindOrDefault(off, "NAME")->SetVisible(false);
    FEFinder<TLTextInstance, 3>::FindOrDefault(over, "NAME")->SetVisible(false);
    FEFinder<TLInstance, 4>::FindOrDefault(off, "STATUS")->SetVisible(false);
    FEFinder<TLInstance, 4>::FindOrDefault(over, "STATUS")->SetVisible(false);
    FEFinder<TLInstance, 4>::FindOrDefault(off, "GUEST_HOME_AWAY")->SetVisible(false);
    FEFinder<TLInstance, 4>::FindOrDefault(over, "GUEST_HOME_AWAY")->SetVisible(false);
    FEFinder<TLInstance, 4>::FindOrDefault(off, "PLAYER CLASS")->SetVisible(false);
    FEFinder<TLInstance, 4>::FindOrDefault(over, "PLAYER CLASS")->SetVisible(false);

    FEFinder<TLTextInstance, 3>::FindOrDefault(off, "STATS", "Slide1", "RANK", "RANK")->SetVisible(false);
    FEFinder<TLTextInstance, 3>::FindOrDefault(over, "STATS", "Slide1", "RANK", "RANK")->SetVisible(false);
    FEFinder<TLTextInstance, 3>::FindOrDefault(off, "STATS", "Slide1", "RECORD", "RECORD")->SetVisible(false);
    FEFinder<TLTextInstance, 3>::FindOrDefault(over, "STATS", "Slide1", "RECORD", "RECORD")->SetVisible(false);

    FEFinder<TLImageInstance, 2>::FindOrDefault(off, "00_dummy_texture")->SetVisible(false);
    FEFinder<TLImageInstance, 2>::FindOrDefault(over, "00_dummy_texture")->SetVisible(false);

    TLImageInstance* miiOff = FEFinder<TLImageInstance, 2>::FindOrDefault(off, "Mii_btn", "Mii");
    miiOff->SetVisible(false);
    miiOff->SetAssetVisible(false);
    TLImageInstance* miiOver = FEFinder<TLImageInstance, 2>::FindOrDefault(over, "Mii_btn", "Mii");
    miiOver->SetVisible(false);
    miiOver->SetAssetVisible(false);

    TLImageInstance* logoOff = FEFinder<TLImageInstance, 2>::FindOrDefault(off, "Mii_btn", "logo_32x32");
    logoOff->SetVisible(false);
    logoOff->SetAssetVisible(false);
    TLImageInstance* logoOver = FEFinder<TLImageInstance, 2>::FindOrDefault(over, "Mii_btn", "logo_32x32");
    logoOver->SetVisible(false);
    logoOver->SetAssetVisible(false);

    TLImageInstance* shouldersOff = FEFinder<TLImageInstance, 2>::FindOrDefault(off, "Mii_btn", "shoulders");
    shouldersOff->SetVisible(false);
    shouldersOff->SetAssetVisible(false);
    TLImageInstance* shouldersOver = FEFinder<TLImageInstance, 2>::FindOrDefault(over, "Mii_btn", "shoulders");
    shouldersOver->SetVisible(false);
    shouldersOver->SetAssetVisible(false);

    TLImageInstance* backgroundOff = FEFinder<TLImageInstance, 2>::FindOrDefault(off, "Mii_btn", "Online_Mii_select_background");
    backgroundOff->SetVisible(false);
    backgroundOff->SetAssetVisible(false);
    TLImageInstance* backgroundOver = FEFinder<TLImageInstance, 2>::FindOrDefault(over, "Mii_btn", "Online_Mii_select_background");
    backgroundOver->SetVisible(false);
    backgroundOver->SetAssetVisible(false);
}

static const int sFriendStatusSortOrder[12] = { 5, 4, 0, 3, 6, 7, 8, 10, 9, 1, 2, 11 };

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
    typedef Detail::MemFunImpl<void, void (SHOnlineFriends::*)(int, void*)> PointerMethod;
    typedef BindExp3<void, PointerMethod, SHOnlineFriends*, Placeholder<0>, Placeholder<1> > PointerBinding;

    FEPointerListener::Callback over(PointerBinding(MemFun(&SHOnlineFriends::OnPointerEnter), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback off(PointerBinding(MemFun(&SHOnlineFriends::OnPointerLeave), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback down(PointerBinding(MemFun(&SHOnlineFriends::OnPointerPress), this, Placeholder<0>(), Placeholder<1>()));
    for (int i = 0; i < 4; ++i)
    {
        mRowButtons[i].SetInstanceBounds(mRowInstances[i], true, -24.0f, 10.0f, 0.7f, 0.55f);
        mRowButtons[i].SetPointerEnterCallback(over);
        mRowButtons[i].SetPointerLeaveCallback(off);
        mRowButtons[i].SetPointerPressCallback(down);
    }
    if (!mScrollBar.mInitialized)
        mScrollBar.Initialize();
}

inline void SHOnlineFriends::ShowDialog(int type)
{
    if (GameSceneManager::Instance()->GetSceneType(GameSceneManager::Instance()->GetCurrentScene()) != 10)
    {
        FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push((SceneList)10, SCREEN_NOTHING, false);
        popup->Create((ePopupMenu)type, Bind<void>(MemFun(&SHOnlineFriends::OnDialogDismissed), this));
        mPopupActive = true;
    }
}

inline void SHOnlineFriends::ConfirmDeleteFriend(int index)
{
    if (GameSceneManager::Instance()->GetSceneType(GameSceneManager::Instance()->GetCurrentScene()) != 10)
    {
        FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push((SceneList)10, SCREEN_NOTHING, false);
        popup->Create((ePopupMenu)106,
            Bind<void>(MemFun(&SHOnlineFriends::DeleteFriend), this, index),
            Bind<void>(MemFun(&SHOnlineFriends::CancelDeleteFriend), this));
        mPopupActive = true;
    }
}

void SHOnlineFriends::OnPointerPress(int index, void* context)
{
    int selected;
    u8 region;
    int item = (int)context;
    mPressedItem = item;
    mPressHandled = true;
    FEAudio::PlayAnimAudioEvent(0xF0AFD586, 0, 0, 1);
    bool change = false;
    if (item == 0 && !IsOnlineFriendSelectionMode())
    {
        if (g_pFriendManager->CountFriends() >= 64)
            ShowDialog(104);
        else
            change = true;
    }
    else if (!IsOnlineFriendSelectionMode())
    {
        g_pFriendManager->SetOwnStatusInitial(0);
        ConfirmDeleteFriend(item);
    }
    else
    {
        selected = item + mScrollOffset;
        if (!IsOnlineFriendSelectionMode())
            --selected;
        region = *(u8*)GameInfoManager::Instance()->GetUnknown0xA40(
            gNetworkSaveSlotIndex, mSortedFriendRows[selected]->mFriendIndex);
        bool differentRegion = region != GetOnlineRegion();
        if (differentRegion)
            ShowDialog(105);
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
            GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
        mPresentation->SetActiveSlide("out", true);
        mPresentation->Update(0.0f);
    }
}

void SHOnlineFriends::OnPointerEnter(int index, void* context)
{
    int item = (int)context;
    ++mUnidentified0028;
    mRowInstances[item]->SetActiveSlide("over", true, false);
    mRowButtons[item].SetPointerState(1, index);
    FEAudio::PlayAnimAudioEvent(0xF6EB899E, 0, 0, 1);
}

void SHOnlineFriends::OnPointerLeave(int index, void* context)
{
    int item = (int)context;
    --mUnidentified0028;
    mRowInstances[item]->SetActiveSlide("off", true, false);
    mRowButtons[item].SetPointerState(0, index);
}

void SHOnlineFriends::DeleteFriend(int index)
{
    mPopupActive = false;
    int selected = index + mScrollOffset;
    if (!IsOnlineFriendSelectionMode())
        --selected;
    g_pFriendManager->DeleteFriend(mSortedFriendRows[selected]->mFriendIndex);
    mSortedFriendRows[selected]->mStatus = 11;
    if (mScrollOffset == mScrollRange && mScrollOffset > 0)
        --mScrollOffset;
    RefreshFriends(this);
    UpdateScrollRange();
    UpdateVisibleRows();
    g_pFriendManager->SetOwnStatusInitial(1);
}

void SHOnlineFriends::CancelDeleteFriend()
{
    mPopupActive = false;
    g_pFriendManager->SetOwnStatusInitial(1);
}

void SHOnlineFriends::UpdateFriendCode()
{
    WideString format;
    WideString string;
    u16 friendKey[14];
    g_pFriendManager->GetOwnFriendKeyString(friendKey);
    nlStrNCpy(mFriendCodeBuffer, friendKey, 14);
    format = WideString(LookupLocString("ONLINE_FRIEND_CODE_YOURS"));
    string = Format(format, mFriendCodeBuffer);
    memcpy(mFriendCodeBuffer, string.c_str(), sizeof(mFriendCodeBuffer));
    mFriendCodeText->SetString(mFriendCodeBuffer);
}

void SHOnlineFriends::OnDialogDismissed()
{
    mPopupActive = false;
}

void SHOnlineFriends::OnErrorDismissed()
{
    mPopupActive = false;
    GameSceneManager::Instance()->Pop();
    FEAudio::PlayAnimAudioEvent(0x4430B152, 0, 0, 1);
    FrontEndPresentation::GetInstance()->Call("TransitionOnlineMatchToMainMenu");
}

inline void SHOnlineFriends::ShowError(int error)
{
    if (GameSceneManager::Instance()->GetSceneType(GameSceneManager::Instance()->GetCurrentScene()) != 10)
    {
        FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push((SceneList)10, SCREEN_NOTHING, false);
        popup->Create((ePopupMenu)error,
            Function<FnVoidVoid>(Bind<void>(MemFun(&SHOnlineFriends::OnErrorDismissed), this)));
        mPopupActive = true;
    }
}

inline void SHOnlineFriends::StartFriendInvite()
{
    SHOnlineInvitePlayers* scene = (SHOnlineInvitePlayers*)GameSceneManager::Instance()->Push((SceneList)44, SCREEN_FORWARD, true);
    scene->mIsHost = true;
    scene->mStartFriendServer = false;
    SetOnlineFriendSelectionMode(false);
    GameInfoManager* gameInfo = GameInfoManager::Instance();
    int selected = mSortedFriendRows[mPressedItem + mScrollOffset]->mFriendIndex;
    GetFriendManager()->SetOwnStatusHostInvitingPlayer(selected,
        reinterpret_cast<const GameplaySettings*>(gameInfo->GetCurrentSettings()),
        reinterpret_cast<const CheatSettings*>(gameInfo->GetActiveRules()), gameInfo->GetStadium());
}
