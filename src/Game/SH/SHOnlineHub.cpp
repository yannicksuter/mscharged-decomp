#include <dwc/dwc_account.h>
#include <dwc/dwc_friend.h>
#include <dwc/dwc_nastime.h>

#include "Game/FE/FEAudio.h"
#include "Game/FE/feHelpFuncs_decl.h"
#include "NL/nlPrint.h"
#include "Game/OnlineMatchmaking.h"
#include "Game/NetworkLobby.h"

#include "Game/GameSceneManager.h"
#include "Game/FE/feInlineHasher.h"
#include "Game/FE/feFinder_impl.h"
#include "Game/FE/feInput.h"
#include "Game/FE/feMusic.h"
#include "Game/FE/fePackage.h"
#include "Game/FE/fePopupMenu.h"
#include "Game/FE/feScene.h"
#include "Game/FE/feTextureResource.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlImageInstance.h"
#include "Game/FE/tlTextInstance.h"
#include "Game/GameInfo.h"
#include "Game/NetworkSession.h"
#include "Game/NetworkStatsManager.h"
#include "Game/Render/FrontEndPresentation.h"
#include "Game/Render/RLViewLayers.h"
#include "Game/FriendManager.h"
#include "NL/nlBind.h"
#include "NL/nlFunction.inl"
#include "NL/nlFormat.h"
#include "NL/nlLocalizationLookup.h"
#include "NL/nlstring_tmpl.h"
#include "Game/FE/feDPD.h"
#include "Game/SH/SHNavigation.h"
#include "Game/SH/SHHallOfFame.h"
#include "Game/FE/feOnlineError.h"
#include "Game/MiiManager.h"
#include "Game/FE/tlDefault.h"
#include "Game/SH/SHOnlineHub.h"

typedef BasicString<unsigned short, Detail::TempStringAllocator> WideString;

static inline void ShowOnlineHubDialog(SHOnlineHub* hub, ePopupMenu type);

static inline void UpdateOnlineHubMiiIcon(SHOnlineHub* hub)
{
    TLImageInstance* image = FEFinder<TLImageInstance, 2>::FindOrDefault(hub->mPresentation->m_currentSlide, "Layer", "summary", "Mii_btn", "Mii");
    unsigned long texture = g_pMiiManager->mIconTextureIds[0];
    image->SetAssetVisible(true);
    int profile = GameInfoManager::Instance()->GetSaveSlotName(gNetworkSaveSlotIndex);
    if (profile >= 0)
    {
        bool valid = g_pMiiManager->CreateIcon(profile, 0, (RFLExpression)0);
        image->m_pTextureResource->SetTextureHandle(texture);
        image->SetAssetVisible(valid);
    }
}

SHOnlineHub::SHOnlineHub()
    : mInitialized(false)
    , mRefreshTimer(0.0f)
    , mPopupActive(false)
    , mHasStrikerOfTheDay(false)
    , mState(0)
    , mPressedItem(0)
{
    for (int i = 0; i < 4; ++i)
        mButtons[i].mContext = (void*)i;
    mHelpButton.mContext = (void*)4;
    mPointsStats.Reset();
    mRankStats.Reset();
    mStrikerOfTheDay.mName[0] = 0;
    mStrikerOfTheDay.mProfileId = 0;
    memset(mStrikerOfTheDay.mData, 0, sizeof(mStrikerOfTheDay.mData));
    mStrikerOfTheDayStats.Reset();
    mBackButton.SetPushBackScene(false);
    mBackButton.SetPopScene(false);
    SetOnlineTwoLocalPlayers(false);
    gOnlineLocalControllerIndices[1] = -1;
}

SHOnlineHub::~SHOnlineHub()
{
}

void SHOnlineHub::SceneCreated()
{
    FEPresentation* presentation = mFEScene->m_pFEPackage->GetPresentation();
    for (int i = 0; i < 4; ++i)
    {
        GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
        mHoverCounts[i] = 0;
    }
    for (int i = 0; i < 4; ++i)
    {
        mButtonInstances[i] = FEFinder<TLComponentInstance, 4>::FindOrDefault(presentation->m_currentSlide, "Layer", sOnlineHubButtonNames[i]);
    }
    TLComponentInstance* help = FEFinder<TLComponentInstance, 4>::FindOrDefault(presentation->m_currentSlide, "Layer", "HELP_BUTTON");
    help->SetActiveSlide(IsWidescreen() ? "16:9" : "4:3", true, false);
    mHelpButtonInstance = FEFinder<TLComponentInstance, 4>::FindOrDefault(help, "HELP");
    SHNavigation* scene = GetNavigationScene();
    TLComponentInstance* done = 0;
    if (scene != 0)
    {
        scene->HideButtons();
        done = scene->GetButton(4);
        scene->SetBackButtonText(1);
    }
    mBackButton.SetButtonInstance(done);
    g_pNetworkSessionBase->SetSessionState(2);
    UpdateFriendAndSeasonText();
    UpdateLocalStats();
    UpdateStrikerOfTheDay();
    FEMusic::StartStreamIfDifferent(8);
    g_pFriendManager->SetOwnStatusInitial(1);
}

void SHOnlineHub::Update(float dt)
{
    BaseSceneHandler::Update(dt);
    if (mPopupActive && !g_pFEInput->HasInputLock(this))
        return;
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
            SHNavigation* scene = GetNavigationScene();
            if (scene != 0)
            {
                scene->SetButtons(4, true);
                scene->SetBackButtonText(1);
            }
            InitializeButtons();
            mInitialized = true;
            for (int i = 0; i < 4; ++i)
                GetPointerInstance(i)->SetActiveSlide("cursor", true, false);
            UpdateOnlineHubMiiIcon(this);
            UpdateStrikerOfTheDay();
            mState = 1;
        }
        else if (mState == 2)
        {
            switch (mPressedItem)
            {
            case 0:
                if (g_pNetworkSessionBase->GetSessionMode() == 2)
                    GameSceneManager::Instance()->Push((SceneList)42, SCREEN_FORWARD, true);
                break;
            case 1: GameSceneManager::Instance()->Push((SceneList)41, SCREEN_FORWARD, true); break;
            case 2: GameSceneManager::Instance()->Push((SceneList)46, SCREEN_FORWARD, true); break;
            case 3: GameSceneManager::Instance()->Push((SceneList)47, SCREEN_FORWARD, true); break;
            }
            return;
        }
        else if (mState == 3)
        {
            FEAudio::PlayAnimAudioEvent(0x4430B152, 0, 0, 1);
            GameSceneManager::Instance()->Pop();
            FrontEndPresentation::GetInstance()->Call("TransitionOnlineMatchToMainMenu");
            return;
        }
    }
    if (!GameSceneManager::Instance()->IsOnStack((SceneList)10) && g_pFriendManager->FindHostInvitation())
    {
        FriendManager* friendManager = g_pFriendManager;
        friendManager->mReturnScene = 40;
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
    for (int i = 0; i < 4; ++i)
    {
        u8 valid = true;
        FEPointerEvent event;
        event.mIndex = i;
        event.mPosition = GetPointerPosition(i, &valid);
        event.mPressed = g_pFEInput->JustPressed((eFEINPUT_PAD)i, 30, true, 0);
        TLComponentInstance* pointer = GetPointerInstance(i);
        if ((unsigned int)i != gFEControllerIndex)
        {
            pointer->SetActiveSlide("waiting", true, false);
            continue;
        }
        for (int j = 0; j < 4; ++j)
            mButtons[j].HandlePointerEvent(&event);
        if (mState != 1)
            return;
        mHelpButton.HandlePointerEvent(&event);
        if (mBackButton.UpdateBackButton(event, dt))
        {
            mState = 3;
            SHNavigation* scene = GetNavigationScene();
            if (scene != 0)
                scene->HideButtons();
            mPresentation->SetActiveSlide("out", true);
            mPresentation->Update(0.0f);
            return;
        }
    }
    mRefreshTimer += dt;
    if (mRefreshTimer >= 1.0f)
    {
        UpdateFriendAndSeasonText();
        UpdateLocalStats();
        UpdateStrikerOfTheDay();
        UpdateOnlineHubMiiIcon(this);
        mRefreshTimer = 0.0f;
    }
}

void SHOnlineHub::UpdateFriendAndSeasonText()
{
    int friends = 0;
    int online = 0;
    if (g_pNetworkSessionBase->GetSessionMode() == 1)
        return;
    for (int i = 0; i < 64; ++i)
    {
        DWCAccFriendData* data = (DWCAccFriendData*)GameInfoManager::Instance()->GetUnknown0x40(gNetworkSaveSlotIndex, i);
        int type = DWC_GetFriendDataType(data);
        if (DWC_IsValidFriendData(data) && type == DWC_FRIENDDATA_GS_PROFILE_ID)
        {
            char status[256];
            ++friends;
            switch (DWC_GetFriendStatus((DWCFriendData*)data, status))
            {
            case DWC_STATUS_ONLINE:
            case DWC_STATUS_PLAYING:
            case DWC_STATUS_MATCH_ANYBODY:
            case DWC_STATUS_MATCH_FRIEND:
            case DWC_STATUS_MATCH_SC_CL:
            case DWC_STATUS_MATCH_SC_SV:
                ++online;
                break;
            }
        }
    }
    TLTextInstance* text = FEFinder<TLTextInstance, 3>::FindOrDefault(mPresentation->m_currentSlide, "Layer", "subheading2");
    u16 onlineText[4];
    u16 friendsText[4];
    nlSNPrintf(onlineText, 4, (const u16*)L"%d", online);
    nlSNPrintf(friendsText, 4, (const u16*)L"%d", friends);
    WideString friendString = Format(WideString(LookupLocString("ONLINE_HUB_FRIENDS")), onlineText, friendsText);
    memcpy(mFriendsText, friendString.c_str(), sizeof(mFriendsText));
    text->SetString(mFriendsText);
    DWCDate date;
    DWCTime time;
    GetAdjustedNetworkDate(&date, &time);
    NetworkSeasonDate current = { date.month, date.mday };
    int year = date.year;
    int boundary = FindNetworkSeasonBoundary(&sNetworkSeasonDateTable, current);
    int elapsed = GetDaysSinceSeasonBoundary(&sNetworkSeasonDateTable, boundary, current, year) + 1;
    int days = GetDaysUntilNextSeasonBoundary(&sNetworkSeasonDateTable, boundary, year) - elapsed;
    int hours = 23 - time.hour;
    int minutes = 60 - time.min;
    if (minutes == 60)
    {
        minutes = 0;
        ++hours;
        if (hours == 24)
        {
            hours = 0;
            ++days;
        }
    }
    text = FEFinder<TLTextInstance, 3>::Find<>(mPresentation->m_currentSlide, "Layer", "subheading");
    WideString string = Format(WideString(LookupLocString("ONLINE_HUB_DAYS_REMAIN")), days, hours, minutes);
    memcpy(mDaysRemainText, string.c_str(), sizeof(mDaysRemainText));
    text->SetString(mDaysRemainText);
}

void SHOnlineHub::UpdateLocalStats()
{
    if (NetworkStatsManager::Instance()->GetLocalStats(0) != 0)
        mRankStats = *NetworkStatsManager::Instance()->GetLocalStats(0);
    if (NetworkStatsManager::Instance()->GetLocalStats(1) != 0)
        mPointsStats = *NetworkStatsManager::Instance()->GetLocalStats(1);
    TLComponentInstance* summary = FEFinder<TLComponentInstance, 4>::Find<>(mPresentation->m_currentSlide, "Layer", "summary");
    TLTextInstance* text = FEFinder<TLTextInstance, 3>::Find<>(summary, "name");
    nlStrNCpy(mPlayerNameText, gNetworkMiiNameWide, 24);
    text->SetString(mPlayerNameText);
    text = FEFinder<TLTextInstance, 3>::Find<>(summary, "therecord");
    WideString points = Format(WideString(LookupLocString("ONLINE_HUB_SOTD_POINTS_TODAY")), mPointsStats.mScore);
    nlStrNCpy(mTodayPointsText, points.c_str(), 48);
    text->SetString(mTodayPointsText);
    text = FEFinder<TLTextInstance, 3>::Find<>(summary, "Rank");
    WideString rank = Format(WideString(LookupLocString("ONLINE_HUB_CURRENT_RANK")), mRankStats.mDisplayRank);
    nlStrNCpy(mRankText, rank.c_str(), 48);
    text->SetString(mRankText);
}

void SHOnlineHub::UpdateStrikerOfTheDay()
{
    NetworkLeaderboardCategory* category = NetworkStatsManager::Instance()->GetCategory(1);
    if (category != 0)
    {
        if (category->mCount >= 1 && category->mMetadata[0].mScore > 0)
        {
            mStrikerOfTheDay.CopyFrom(category->mPlayers[0]);
            mStrikerOfTheDayStats = category->mMetadata[0];
            mHasStrikerOfTheDay = true;
        }
        else
        {
            mStrikerOfTheDay.mName[0] = 0;
            mStrikerOfTheDay.mProfileId = 0;
            memset(mStrikerOfTheDay.mData, 0, sizeof(mStrikerOfTheDay.mData));
            mStrikerOfTheDayStats.Reset();
            mHasStrikerOfTheDay = false;
        }
    }
    TLComponentInstance* summary = FEFinder<TLComponentInstance, 4>::Find<>(mPresentation->m_currentSlide, "Layer", "summary");
    TLTextInstance* text = FEFinder<TLTextInstance, 3>::Find<>(summary, "therecord2");
    if (mHasStrikerOfTheDay)
    {
        WideString string = Format(WideString(LookupLocString("ONLINE_HUB_SOTD_POINTS")), mStrikerOfTheDayStats.mScore);
        nlStrNCpy(mStrikerPointsText, string.c_str(), 48);
        text->SetString(mStrikerPointsText);
        text->m_bVisible = true;
    }
    else
        text->m_bVisible = false;
    text = FEFinder<TLTextInstance, 3>::Find<>(summary, "sotd description");
    if (mHasStrikerOfTheDay)
    {
        WideString string = Format(WideString(LookupLocString("ONLINE_HUB_SOTD_DESCRIPTION")), mStrikerOfTheDay.mName);
        nlStrNCpy(mStrikerDescriptionText, string.c_str(), 128);
        text->SetString(mStrikerDescriptionText);
        text->m_bVisible = true;
    }
    else
        text->m_bVisible = false;
    bool valid = false;
    if (mHasStrikerOfTheDay)
        valid = g_pMiiManager->CreateIcon((const RFLStoreData*)mStrikerOfTheDay.mData, 1, (RFLExpression)0);
    TLImageInstance* image = FEFinder<TLImageInstance, 2>::FindOrDefault(mPresentation->m_currentSlide, "Layer", "summary", "Mii_btn2", "Mii");
    unsigned long texture = g_pMiiManager->mIconTextureIds[1];
    image->SetAssetVisible(valid && mInitialized);
    image->m_pTextureResource->SetTextureHandle(texture);
}

void SHOnlineHub::InitializeButtons()
{
    typedef Detail::MemFunImpl<void, void (SHOnlineHub::*)(unsigned int, void*)> PointerMethod;
    typedef BindExp3<void, PointerMethod, SHOnlineHub*, Placeholder<0>, Placeholder<1> > PointerBinding;

    FEPointerListener::Callback over(PointerBinding(MemFun(&SHOnlineHub::OnPointerEnter), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback off(PointerBinding(MemFun(&SHOnlineHub::OnPointerLeave), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback down(PointerBinding(MemFun(&SHOnlineHub::OnPointerPress), this, Placeholder<0>(), Placeholder<1>()));
    for (int i = 0; i < 4; ++i)
    {
        mButtons[i].SetInstanceBounds(mButtonInstances[i], true, 0.0f, 0.0f, 1.0f, 1.0f);
        mButtons[i].SetPointerEnterCallback(over);
        mButtons[i].SetPointerLeaveCallback(off);
        mButtons[i].SetPointerPressCallback(down);
    }
    TLInstance* instance = FEFinder<TLInstance, 2>::FindOrDefault(mHelpButtonInstance, "OVER", "list_high_250x60");
    feVector3 position = mHelpButtonInstance->GetAssetPosition();
    mHelpButton.SetInstanceBounds(instance, true, position.f.x, position.f.y, 1.0f, 1.0f);
    mHelpButton.SetPointerEnterCallback(over);
    mHelpButton.SetPointerLeaveCallback(off);
    mHelpButton.SetPointerPressCallback(down);
}

void SHOnlineHub::OnPointerPress(unsigned int index, void* context)
{
    int item = (int)context;
    FEAudio::PlayAnimAudioEvent(0xF0AFD586, 0, 0, 1);
    bool change = false;
    switch (item)
    {
    case 0:
        if (g_pNetworkSessionBase->GetSessionMode() == 2)
        {
            if (g_pFriendManager->CountBuddies() > 0)
                change = true;
            else
                ShowOnlineHubDialog(this, (ePopupMenu)113);
        }
        break;
    case 1:
        change = true;
        break;
    case 2:
        change = true;
        break;
    case 3:
        if (g_pNetworkSessionBase->GetSessionMode() == 2)
            change = true;
        break;
    case 4:
        g_pFriendManager->SetOwnStatusInitial(0);
        ShowOnlineHubDialog(this, (ePopupMenu)58);
        break;
    }
    if (change)
    {
        mPressedItem = item;
        for (int i = 0; i < 4; ++i)
            GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
        mState = 2;
        SHNavigation* scene = GetNavigationScene();
        if (scene != 0)
            scene->HideButtons();
        mPresentation->SetActiveSlide("out", true);
        mPresentation->Update(0.0f);
    }
}

void SHOnlineHub::OnDialogDismissed()
{
    g_pFriendManager->SetOwnStatusInitial(1);
    mPopupActive = false;
}

void SHOnlineHub::OnErrorDismissed()
{
    mPopupActive = false;
    GameSceneManager::Instance()->Pop();
    FEAudio::PlayAnimAudioEvent(0x4430B152, 0, 0, 1);
    FrontEndPresentation::GetInstance()->Call("TransitionOnlineMatchToMainMenu");
}

void SHOnlineHub::OnPointerEnter(unsigned int index, void* context)
{
    unsigned int item = (unsigned int)context;
    ++mHoverCounts[index];
    if (item < 4)
    {
        if (!mButtons[item].HasOtherPointerState(1, index))
        {
            mButtons[item].SetPointerState(1, index);
            mButtonInstances[item]->SetActiveSlide("over", true, false);
            FEAudio::PlayAnimAudioEvent(0x96DEB5C3, 0, 0, 1);
        }
    }
    else if (!mHelpButton.HasOtherPointerState(1, index))
    {
        mHelpButton.SetPointerState(1, index);
        mHelpButtonInstance->SetActiveSlide("over", true, false);
        FEAudio::PlayAnimAudioEvent(0xACCDCA48, 0, 0, 1);
    }
}

void SHOnlineHub::OnPointerLeave(unsigned int index, void* context)
{
    unsigned int item = (unsigned int)context;
    --mHoverCounts[index];
    if (item < 4)
    {
        if (!mButtons[item].HasOtherPointerState(1, index))
        {
            mButtons[item].SetPointerState(0, index);
            mButtonInstances[item]->SetActiveSlide("off", true, false);
        }
    }
    else if (!mHelpButton.HasOtherPointerState(1, index))
    {
        mHelpButton.SetPointerState(0, index);
        mHelpButtonInstance->SetActiveSlide("off", true, false);
    }
}

static inline void ShowOnlineHubDialog(SHOnlineHub* hub, ePopupMenu type)
{
    if (GameSceneManager::Instance()->GetSceneType(GameSceneManager::Instance()->GetCurrentScene()) != 10)
    {
        FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push((SceneList)10, SCREEN_NOTHING, false);
        popup->Create(type, Bind<void>(MemFun(&SHOnlineHub::OnDialogDismissed), hub));
        hub->mPopupActive = true;
    }
}

inline void SHOnlineHub::ShowError(int error)
{
    if (GameSceneManager::Instance()->GetSceneType(GameSceneManager::Instance()->GetCurrentScene()) != 10)
    {
        FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push((SceneList)10, SCREEN_NOTHING, false);
        popup->Create((ePopupMenu)error,
            Function<FnVoidVoid>(Bind<void>(MemFun(&SHOnlineHub::OnErrorDismissed), this)));
        mPopupActive = true;
    }
}
