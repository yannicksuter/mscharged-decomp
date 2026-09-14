#include <dwc/dwc_account.h>
#include <dwc/dwc_friend.h>
#include <dwc/dwc_nastime.h>

#include "Game/SH/SHOnlineHub.h"
#include "Game/FE/FEAudio.h"
#include "Game/FE/feHelpFuncs_decl.h"
#include "NL/nlPrint.h"
#include "Game/OnlineMatchmaking.h"
#include "Game/NetworkLobby.h"

#include "Game/GameSceneManager.h"
#include "Game/FE/feFinder.inl"
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
#include "Game/Render/Presentation.h"
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
#include "Game/FE/UnidentifiedTLDefault.h"

static const char* const sOnlineHubButtonNames[4] = {
    "BTN_UNRANKED", "BTN_RANKED", "BTN_LEADERBOARD", "BTN_FRIENDS"
};

typedef BasicString<unsigned short, Detail::TempStringAllocator> WideString;

SHOnlineHub::SHOnlineHub()
    : mUnidentified4B4(false)
    , mUnidentified588(0.0f)
    , mUnidentified58C(false)
    , mUnidentified58D(false)
    , mUnidentified890(0)
    , mUnidentified894(0)
{
    for (int i = 0; i < 4; ++i)
        mUnidentified020[i].mContext = (void*)i;
    mUnidentified300.mContext = (void*)4;
    mUnidentified590.Reset();
    mUnidentified5A8.Reset();
    mUnidentified5C0.mName[0] = 0;
    mUnidentified5C0.mProfileId = 0;
    memset(mUnidentified5C0.mData, 0, sizeof(mUnidentified5C0.mData));
    mUnidentified628.Reset();
    mUnidentified3B8.SetPushBackScene(false);
    mUnidentified3B8.SetPopScene(false);
    SetOnlineTwoLocalPlayers(false);
    gOnlineLocalControllerIndices[1] = -1;
}

SHOnlineHub::~SHOnlineHub()
{
}

void SHOnlineHub::OnPointerPress(int index, void* context)
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
            else if (GameSceneManager::Instance()->GetSceneType(GameSceneManager::Instance()->GetCurrentScene()) != 10)
            {
                FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push((SceneList)10, SCREEN_NOTHING, false);
                popup->Create((ePopupMenu)113, Bind<void>(MemFun(&SHOnlineHub::OnDialogDismissed), this));
                mUnidentified58C = true;
            }
        }
        break;
    case 1:
    case 2:
        change = true;
        break;
    case 3:
        if (g_pNetworkSessionBase->GetSessionMode() == 2)
            change = true;
        break;
    case 4:
        g_pFriendManager->SetOwnStatusInitial(0);
        if (GameSceneManager::Instance()->GetSceneType(GameSceneManager::Instance()->GetCurrentScene()) != 10)
        {
            FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push((SceneList)10, SCREEN_NOTHING, false);
            popup->Create((ePopupMenu)58, Bind<void>(MemFun(&SHOnlineHub::OnDialogDismissed), this));
            mUnidentified58C = true;
        }
        break;
    }
    if (change)
    {
        mUnidentified894 = item;
        for (int i = 0; i < 4; ++i)
            gFEPointerInstances[i]->SetActiveSlide("waiting", true, false);
        mUnidentified890 = 2;
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
    mUnidentified58C = false;
}

void SHOnlineHub::OnErrorDismissed()
{
    mUnidentified58C = false;
    GameSceneManager::Instance()->Pop();
    FEAudio::PlayAnimAudioEvent(0x4430B152, 0, 0, 1);
    Presentation::GetInstance()->Call("TransitionOnlineMatchToMainMenu");
}

void SHOnlineHub::SceneCreated()
{
    FEPresentation* presentation = mFEScene->m_pFEPackage->GetPresentation();
    for (int i = 0; i < 4; ++i)
    {
        gFEPointerInstances[i]->SetActiveSlide("waiting", true, false);
        mUnidentified4B8[i] = 0;
    }
    for (int i = 0; i < 4; ++i)
    {
        TLComponentInstance* instance = FEFinder<TLComponentInstance, 4>::Find(presentation->m_currentSlide, "Layer", sOnlineHubButtonNames[i]);
        mUnidentified2F0[i] = instance != 0 ? instance : &UnidentifiedTLComponentDefault::sInstance;
    }
    TLComponentInstance* help = FEFinder<TLComponentInstance, 4>::FindOrDefault(presentation->m_currentSlide, "Layer", "HELP_BUTTON");
    help->SetActiveSlide(IsWidescreen() ? "16:9" : "4:3", true, false);
    TLComponentInstance* instance = FEFinder<TLComponentInstance, 4>::Find(help, "HELP");
    mUnidentified3B4 = instance != 0 ? instance : &UnidentifiedTLComponentDefault::sInstance;
    SHNavigation* scene = GetNavigationScene();
    TLComponentInstance* done = 0;
    if (scene != 0)
    {
        scene->HideButtons();
        done = scene->GetButton(4);
        scene->SetBackButtonText(1);
    }
    mUnidentified3B8.SetButtonInstance(done);
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
    if (mUnidentified58C && !g_pFEInput->HasInputLock(this))
        return;
    if (mUnidentified890 == 0 || mUnidentified890 == 2 || mUnidentified890 == 3)
    {
        TLSlide* slide = mPresentation->m_currentSlide;
        if (slide->m_time < slide->m_start + slide->m_duration)
        {
            for (int i = 0; i < 4; ++i)
                gFEPointerInstances[i]->SetActiveSlide("waiting", true, false);
            return;
        }
        if (mUnidentified890 == 0)
        {
            SHNavigation* scene = GetNavigationScene();
            if (scene != 0)
            {
                scene->SetButtons(4, true);
                scene->SetBackButtonText(1);
            }
            InitializeButtons();
            mUnidentified4B4 = true;
            for (int i = 0; i < 4; ++i)
                gFEPointerInstances[i]->SetActiveSlide("cursor", true, false);
            TLImageInstance* image = FEFinder<TLImageInstance, 2>::FindOrDefault(mPresentation->m_currentSlide, "Layer", "summary", "Mii_btn", "Mii");
            unsigned long texture = g_pMiiManager->mIconTextureIds[0];
            image->SetAssetVisible(true);
            int profile = GameInfoManager::Instance()->GetSaveSlotName(gNetworkSaveSlotIndex);
            if (profile >= 0)
            {
                bool valid = g_pMiiManager->CreateIcon(profile, 0, (RFLExpression)0);
                image->m_pTextureResource->SetTextureHandle(texture);
                image->SetAssetVisible(valid);
            }
            UpdateStrikerOfTheDay();
            mUnidentified890 = 1;
        }
        else if (mUnidentified890 == 2)
        {
            switch (mUnidentified894)
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
        else if (mUnidentified890 == 3)
        {
            FEAudio::PlayAnimAudioEvent(0x4430B152, 0, 0, 1);
            GameSceneManager::Instance()->Pop();
            Presentation::GetInstance()->Call("TransitionOnlineMatchToMainMenu");
            return;
        }
    }
    if (!GameSceneManager::Instance()->IsOnStack((SceneList)10) && g_pFriendManager->FindHostInvitation())
    {
        g_pFriendManager->mReturnScene = 40;
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
            popup->Create((ePopupMenu)error, Bind<void>(MemFun(&SHOnlineHub::OnErrorDismissed), this));
            mUnidentified58C = true;
        }
        return;
    }
    for (int i = 0; i < 4; ++i)
    {
        u8 valid = true;
        FEPointerEvent event;
        event.mIndex = i;
        event.mPosition = GetPointerPosition(i, &valid);
        event.mPressed = g_pFEInput->JustPressed((eFEINPUT_PAD)i, 30, true, 0);
        if ((unsigned int)i != gFEControllerIndex)
        {
            gFEPointerInstances[i]->SetActiveSlide("waiting", true, false);
            continue;
        }
        for (int j = 0; j < 4; ++j)
            mUnidentified020[j].HandlePointerEvent(&event);
        if (mUnidentified890 != 1)
            return;
        mUnidentified300.HandlePointerEvent(&event);
        if (mUnidentified3B8.UpdateBackButton(event, dt))
        {
            mUnidentified890 = 3;
            SHNavigation* scene = GetNavigationScene();
            if (scene != 0)
                scene->HideButtons();
            mPresentation->SetActiveSlide("out", true);
            mPresentation->Update(0.0f);
            return;
        }
    }
    mUnidentified588 += dt;
    if (mUnidentified588 >= 1.0f)
    {
        UpdateFriendAndSeasonText();
        UpdateLocalStats();
        UpdateStrikerOfTheDay();
        TLImageInstance* image = FEFinder<TLImageInstance, 2>::FindOrDefault(mPresentation->m_currentSlide, "Layer", "summary", "Mii_btn", "Mii");
        unsigned long texture = g_pMiiManager->mIconTextureIds[0];
        image->SetAssetVisible(true);
        int profile = GameInfoManager::Instance()->GetSaveSlotName(gNetworkSaveSlotIndex);
        if (profile >= 0)
        {
            bool valid = g_pMiiManager->CreateIcon(profile, 0, (RFLExpression)0);
            image->m_pTextureResource->SetTextureHandle(texture);
            image->SetAssetVisible(valid);
        }
        mUnidentified588 = 0.0f;
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
    memcpy(mUnidentified4C8, friendString.c_str(), sizeof(mUnidentified4C8));
    text->SetString(mUnidentified4C8);
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
    text = FEFinder<TLTextInstance, 3>::Find(mPresentation->m_currentSlide, "Layer", "subheading");
    WideString string = Format(WideString(LookupLocString("ONLINE_HUB_DAYS_REMAIN")), days, hours, minutes);
    memcpy(mUnidentified528, string.c_str(), sizeof(mUnidentified528));
    text->SetString(mUnidentified528);
}

void SHOnlineHub::UpdateLocalStats()
{
    if (NetworkStatsManager::Instance()->GetLocalStats(0) != 0)
        mUnidentified5A8 = *NetworkStatsManager::Instance()->GetLocalStats(0);
    if (NetworkStatsManager::Instance()->GetLocalStats(1) != 0)
        mUnidentified590 = *NetworkStatsManager::Instance()->GetLocalStats(1);
    TLComponentInstance* summary = FEFinder<TLComponentInstance, 4>::Find(mPresentation->m_currentSlide, "Layer", "summary");
    TLTextInstance* text = FEFinder<TLTextInstance, 3>::Find(summary, "name");
    nlStrNCpy(mUnidentified640, gNetworkMiiNameWide, 24);
    text->SetString(mUnidentified640);
    text = FEFinder<TLTextInstance, 3>::Find(summary, "therecord");
    WideString points = Format(WideString(LookupLocString("ONLINE_HUB_SOTD_POINTS_TODAY")), mUnidentified590.mScore);
    nlStrNCpy(mUnidentified6D0, points.c_str(), 48);
    text->SetString(mUnidentified6D0);
    text = FEFinder<TLTextInstance, 3>::Find(summary, "Rank");
    WideString rank = Format(WideString(LookupLocString("ONLINE_HUB_CURRENT_RANK")), mUnidentified5A8.mDisplayRank);
    nlStrNCpy(mUnidentified670, rank.c_str(), 48);
    text->SetString(mUnidentified670);
}

void SHOnlineHub::UpdateStrikerOfTheDay()
{
    NetworkLeaderboardCategory* category = NetworkStatsManager::Instance()->GetCategory(1);
    if (category != 0)
    {
        if (category->mCount >= 1 && category->mMetadata[0].mScore > 0)
        {
            mUnidentified5C0.CopyFrom(category->mPlayers[0]);
            mUnidentified628 = category->mMetadata[0];
            mUnidentified58D = true;
        }
        else
        {
            mUnidentified5C0.mName[0] = 0;
            mUnidentified5C0.mProfileId = 0;
            memset(mUnidentified5C0.mData, 0, sizeof(mUnidentified5C0.mData));
            mUnidentified628.Reset();
            mUnidentified58D = false;
        }
    }
    TLComponentInstance* summary = FEFinder<TLComponentInstance, 4>::Find(mPresentation->m_currentSlide, "Layer", "summary");
    TLTextInstance* text = FEFinder<TLTextInstance, 3>::Find(summary, "therecord2");
    if (mUnidentified58D)
    {
        WideString string = Format(WideString(LookupLocString("ONLINE_HUB_SOTD_POINTS")), mUnidentified628.mScore);
        nlStrNCpy(mUnidentified730, string.c_str(), 48);
        text->SetString(mUnidentified730);
        text->m_bVisible = true;
    }
    else
        text->m_bVisible = false;
    text = FEFinder<TLTextInstance, 3>::Find(summary, "sotd description");
    if (mUnidentified58D)
    {
        WideString string = Format(WideString(LookupLocString("ONLINE_HUB_SOTD_DESCRIPTION")), mUnidentified5C0.mName);
        nlStrNCpy(mUnidentified790, string.c_str(), 128);
        text->SetString(mUnidentified790);
        text->m_bVisible = true;
    }
    else
        text->m_bVisible = false;
    bool valid = false;
    if (mUnidentified58D)
        valid = g_pMiiManager->CreateIcon((const RFLStoreData*)mUnidentified5C0.mData, 1, (RFLExpression)0);
    TLImageInstance* image = FEFinder<TLImageInstance, 2>::FindOrDefault(mPresentation->m_currentSlide, "Layer", "summary", "Mii_btn2", "Mii");
    unsigned long texture = g_pMiiManager->mIconTextureIds[1];
    image->SetAssetVisible(valid && mUnidentified4B4);
    image->m_pTextureResource->SetTextureHandle(texture);
}

void SHOnlineHub::InitializeButtons()
{
    typedef Detail::MemFunImpl<void, void (SHOnlineHub::*)(int, void*)> PointerMethod;
    typedef BindExp3<void, PointerMethod, SHOnlineHub*, Placeholder<0>, Placeholder<1> > PointerBinding;

    FEPointerListener::Callback over(PointerBinding(MemFun(&SHOnlineHub::OnPointerEnter), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback off(PointerBinding(MemFun(&SHOnlineHub::OnPointerLeave), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback down(PointerBinding(MemFun(&SHOnlineHub::OnPointerPress), this, Placeholder<0>(), Placeholder<1>()));
    for (int i = 0; i < 4; ++i)
    {
        mUnidentified020[i].SetInstanceBounds(mUnidentified2F0[i], true, 0.0f, 0.0f, 1.0f, 1.0f);
        mUnidentified020[i].SetPointerEnterCallback(over);
        mUnidentified020[i].SetPointerLeaveCallback(off);
        mUnidentified020[i].SetPointerPressCallback(down);
    }
    TLInstance* instance = FEFinder<TLInstance, 2>::FindOrDefault(mUnidentified3B4, "OVER", "list_high_250x60");
    feVector3 position = mUnidentified3B4->GetAssetPosition();
    mUnidentified300.SetInstanceBounds(instance, true, position.f.x, position.f.y, 1.0f, 1.0f);
    mUnidentified300.SetPointerEnterCallback(over);
    mUnidentified300.SetPointerLeaveCallback(off);
    mUnidentified300.SetPointerPressCallback(down);
}

void SHOnlineHub::OnPointerEnter(int index, void* context)
{
    unsigned int item = (unsigned int)context;
    ++mUnidentified4B8[index];
    if (item < 4)
    {
        if (!mUnidentified020[item].HasOtherPointerState(1, index))
        {
            mUnidentified020[item].SetPointerState(1, index);
            mUnidentified2F0[item]->SetActiveSlide("over", true, false);
            FEAudio::PlayAnimAudioEvent(0x96DEB5C3, 0, 0, 1);
        }
    }
    else if (!mUnidentified300.HasOtherPointerState(1, index))
    {
        mUnidentified300.SetPointerState(1, index);
        mUnidentified3B4->SetActiveSlide("over", true, false);
        FEAudio::PlayAnimAudioEvent(0xACCDCA48, 0, 0, 1);
    }
}

void SHOnlineHub::OnPointerLeave(int index, void* context)
{
    unsigned int item = (unsigned int)context;
    --mUnidentified4B8[index];
    if (item < 4)
    {
        if (!mUnidentified020[item].HasOtherPointerState(1, index))
        {
            mUnidentified020[item].SetPointerState(0, index);
            mUnidentified2F0[item]->SetActiveSlide("off", true, false);
        }
    }
    else if (!mUnidentified300.HasOtherPointerState(1, index))
    {
        mUnidentified300.SetPointerState(0, index);
        mUnidentified3B4->SetActiveSlide("off", true, false);
    }
}
