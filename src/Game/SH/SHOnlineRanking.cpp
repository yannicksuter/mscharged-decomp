#include "Game/SH/SHOnlineRanking.h"

#include "Game/FE/FEAudio.h"
#include "Game/FE/feDPD.h"
#include "Game/FE/feFinder.inl"
#include "Game/FE/feHelpFuncs_decl.h"
#include "Game/FE/feInput.h"
#include "Game/FE/feOnlineError.h"
#include "Game/FE/fePageControls.h"
#include "Game/FE/fePopupMenu.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlTextInstance.h"
#include "Game/FriendManager.h"
#include "Game/GameSceneManager.h"
#include "Game/NetworkSession.h"
#include "Game/NetworkStatsManager.h"
#include "Game/Render/FrontEndPresentation.h"
#include "Game/SH/SHNavigation.h"
#include "NL/nlBind.h"
#include "NL/nlFormat.h"
#include "NL/nlFunction.inl"
#include "NL/nlLocalizationLookup.h"
#include "NL/nlstring_tmpl.h"

#include <string.h>

typedef BasicString<unsigned short, Detail::TempStringAllocator> WideString;

static const char* sLeaderboardTextNames[5] = {
    "stat_0", "stat_1", "stat_2_INFO", "stat_2_INFO2", "stat_3"
};

static int sLeaderboardCategories[5] = { 2, 5, 3, 0, 1 };

SHOnlineRanking::SHOnlineRanking()
    : mUnidentified30(false)
    , mMyRank(-1)
    , mFirstVisibleRank(0)
    , mRowButtons()
    , mScrollBar()
    , mBackButton()
    , mTransitionState(0)
    , mErrorPopupOpen(false)
{
    mPointerHoverCounts[0] = 0;
    mPointerHoverCounts[1] = 0;
    mPointerHoverCounts[2] = 0;
    mPointerHoverCounts[3] = 0;

    for (int i = 0; i < 10; ++i)
    {
        mRowButtons[i].mContext = (void*)i;
        mRowButtons[i].mSpeakerEnabled = false;
    }

    mBackButton.SetPopScene(false);
}

SHOnlineRanking::~SHOnlineRanking()
{
}

inline void SHOnlineRanking::UpdateRows()
{
    mHighlight->m_bVisible = false;
    for (int i = 0; i < 10; ++i)
    {
        if (PopulateRow(i, i + mFirstVisibleRank))
        {
            mRowInstances[i]->m_bVisible = true;
        }
        else
        {
            mRowInstances[i]->m_bVisible = false;
        }
    }
}

void SHOnlineRanking::SceneCreated()
{
    FEPresentation* presentation = mPresentation;
    for (int i = 0; i < 10; ++i)
    {
        char name[8];
        nlSNPrintf(name, sizeof(name), "team_%d", i);
        mRowInstances[i] = FEFinder<TLComponentInstance, 4>::FindOrDefault(
            presentation->m_currentSlide, "Layer", "Group", name);
    }

    mHighlight = FEFinder<TLComponentInstance, 4>::FindOrDefault(
        presentation->m_currentSlide, "Layer", "Group", "highlight");

    u16* friendCodeInput = g_pFriendManager->mFriendCodeInput;
    mLeaderboardSelection = friendCodeInput[0];
    memset(friendCodeInput, 0, sizeof(g_pFriendManager->mFriendCodeInput));
    int categoryIndex = sLeaderboardCategories[mLeaderboardSelection];
    mLeaderboardCategory = categoryIndex;

    NetworkLeaderboardCategory* category
        = NetworkStatsManager::Instance()->GetCategory(categoryIndex);
    int count = category->mCount;
    TLComponentInstance* scrollBar = FEFinder<TLComponentInstance, 4>::Find(
        mPresentation->m_currentSlide, "Layer", "Group", "scrollbar");
    mScrollBar.SetComponent(scrollBar);
    mScrollBar.SetRange(count - 10);
    mScrollBar.SetValue(mFirstVisibleRank);

    mMyRank = NetworkStatsManager::Instance()
                  ->GetCategory(categoryIndex)
                  ->mFirstRank;
    UpdateRows();
    UpdateHeader();

    for (int i = 0; i < 4; ++i)
    {
        GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
    }

    SHNavigation* navigation = GetNavigationScene();
    TLComponentInstance* backButton = 0;
    if (navigation != 0)
    {
        navigation->HideButtons();
        backButton = navigation->GetButton(4);
        mPageControls = navigation->GetPageControls();
    }
    mBackButton.SetButtonInstance(backButton);
    FEAudio::PlayAnimAudioEvent(0xBB142B94, 0, 0, 1);
}

void SHOnlineRanking::Update(float fDeltaT)
{
    BaseSceneHandler::Update(fDeltaT);

    if (mErrorPopupOpen && !g_pFEInput->HasInputLock(this))
        return;

    if (!mScrollBar.mInitialized)
        mScrollBar.Initialize();

    int state = mTransitionState;
    if (state == 0 || (unsigned int)(state - 2) <= 1)
    {
        TLSlide* slide = mPresentation->m_currentSlide;
        if (slide->GetCurrentTime()
            < slide->GetStartTime() + slide->GetDuration())
        {
            for (int i = 0; i < 4; ++i)
                GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
            return;
        }

        if (state == 0)
        {
            SHNavigation* navigation = GetNavigationScene();
            if (navigation != 0)
            {
                navigation->SetButtons(15, false);
                mPageControls->SetButtonState(1, true, true);
                mPageControls->SetButtonState(0, true, true);
            }

            {
                typedef Detail::MemFunImpl<void,
                    void (SHOnlineRanking::*)(int, void*)>
                    PointerMethod;
                typedef BindExp3<void, PointerMethod, SHOnlineRanking*,
                    Placeholder<0>, Placeholder<1> >
                    PointerBinding;
                FEPointerListener::Callback callbacks[3] = {
                    PointerBinding(MemFun(&SHOnlineRanking::OnRowPointerEnter),
                        this, Placeholder<0>(), Placeholder<1>()),
                    PointerBinding(MemFun(&SHOnlineRanking::OnRowPointerLeave),
                        this, Placeholder<0>(), Placeholder<1>()),
                    PointerBinding(MemFun(&SHOnlineRanking::OnRowPointerPress),
                        this, Placeholder<0>(), Placeholder<1>()),
                };
            }
            mTransitionState = 1;
        }
        else if (state == 3)
        {
            FEAudio::PlayAnimAudioEvent(0x37A9934D, 0, 0, 1);
            GameSceneManager::Instance()->Push(
                SCENE_ONLINE_MENU, SCREEN_NOTHING, true);
            return;
        }
    }

    if (NetworkStatsManager::Instance()->mLeaderboardRequestComplete)
    {
        NetworkStatsManager::Instance()->mLeaderboardRequestComplete = false;
        mMyRank = NetworkStatsManager::Instance()
                      ->GetCategory(mLeaderboardCategory)
                      ->mFirstRank;
        UpdateRows();
        return;
    }

    if (!GameSceneManager::Instance()->IsOnStack(SCENE_POPUP_MENU)
        && g_pFriendManager->FindHostInvitation())
    {
        FriendManager* friendManager = g_pFriendManager;
        friendManager->mReturnScene = SCENE_ONLINE_RANKING;
        friendManager->mPreviousRankedMode = 0;
        g_pFriendManager->mFriendCodeInput[0] = mLeaderboardSelection;
        GameSceneManager::Instance()->Push(
            SCENE_ONLINE_INVITE_RESPONSE, SCREEN_FORWARD, true);
        return;
    }

    if (!NetworkStatsManager::Instance()->RefreshFriendStats_80131B50())
    {
        if (g_pNetworkSession->mDWCLastError == 0)
            g_pNetworkSession->ReadAndClearDWCError();
        int error
            = GetOnlineErrorPopup(g_pNetworkSession->mDWCErrorCode, true, 111);
        if (GameSceneManager::Instance()->GetSceneType(
                GameSceneManager::Instance()->GetCurrentScene())
            != SCENE_POPUP_MENU)
        {
            FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push(
                SCENE_POPUP_MENU, SCREEN_NOTHING, false);
            popup->Create((ePopupMenu)error,
                Bind<void>(MemFun(&SHOnlineRanking::OnErrorDismissed), this));
            mErrorPopupOpen = true;
        }
        return;
    }

    for (int i = 0; i < 4; ++i)
    {
        TLComponentInstance* pointer = GetPointerInstance(i);
        if (g_pFEInput->m_InputLockDepth == 0)
        {
            if ((unsigned int)i != gFEControllerIndex)
            {
                pointer->SetActiveSlide("waiting", true, false);
                continue;
            }
            if (mPointerHoverCounts[i] > 0 || mBackButton.mPointerInside[i]
                || mPageControls->mPointerInside[0]
                || mPageControls->mPointerInside[1])
            {
                pointer->SetActiveSlide("A", true, false);
            }
            else
            {
                pointer->SetActiveSlide("cursor", true, false);
            }
        }

        u8 valid = true;
        FEPointerEvent event;
        event.mIndex = i;
        event.mPosition = GetPointerPosition(i, &valid);
        event.mPressed
            = g_pFEInput->JustPressed((eFEINPUT_PAD)i, 30, true, 0);
        event.mReleased
            = g_pFEInput->JustReleased((eFEINPUT_PAD)i, 30, true, 0);
        for (int row = 0; row < 10; ++row)
            mRowButtons[row].HandlePointerEvent(&event);
        mPageControls->Update(event, fDeltaT);
        mScrollBar.Update(event, fDeltaT);

        if (mBackButton.UpdateBackButton(event, fDeltaT))
        {
            mTransitionState = 3;
            SHNavigation* navigation = GetNavigationScene();
            if (navigation != 0)
                navigation->HideButtons();
            mPresentation->SetActiveSlide("out", true);
            mPresentation->Update(0.0f);
            return;
        }

        if (mPageControls->IsButtonPressed(1))
        {
            --mLeaderboardSelection;
            if (mLeaderboardSelection < 0)
                mLeaderboardSelection = 4;
            mLeaderboardCategory
                = sLeaderboardCategories[mLeaderboardSelection];
            SelectLeaderboardCategory();

            int categoryIndex = mLeaderboardCategory;
            int count = NetworkStatsManager::Instance()
                            ->GetCategory(categoryIndex)
                            ->mCount;
            TLComponentInstance* scrollBar
                = FEFinder<TLComponentInstance, 4>::Find(
                    mPresentation->m_currentSlide, "Layer", "Group",
                    "scrollbar");
            mScrollBar.SetComponent(scrollBar);
            mScrollBar.SetRange(count - 10);
            mScrollBar.SetValue(mFirstVisibleRank);
        }
        else if (mPageControls->IsButtonPressed(0))
        {
            ++mLeaderboardSelection;
            if (mLeaderboardSelection >= 5)
                mLeaderboardSelection = 0;
            mLeaderboardCategory
                = sLeaderboardCategories[mLeaderboardSelection];
            SelectLeaderboardCategory();

            int categoryIndex = mLeaderboardCategory;
            int count = NetworkStatsManager::Instance()
                            ->GetCategory(categoryIndex)
                            ->mCount;
            TLComponentInstance* scrollBar
                = FEFinder<TLComponentInstance, 4>::Find(
                    mPresentation->m_currentSlide, "Layer", "Group",
                    "scrollbar");
            mScrollBar.SetComponent(scrollBar);
            mScrollBar.SetRange(count - 10);
            mScrollBar.SetValue(mFirstVisibleRank);
        }
    }

    if (mScrollBar.IsScrolling(1, true))
    {
        ++mFirstVisibleRank;
        UpdateRows();
    }
    else if (mScrollBar.IsScrolling(0, true))
    {
        --mFirstVisibleRank;
        UpdateRows();
    }
}

void SHOnlineRanking::SelectLeaderboardCategory()
{
    FEAudio::PlayAnimAudioEvent(0x375C885A, 0, 0, 1);
    UpdateHeader();

    int categoryIndex = mLeaderboardCategory;
    NetworkLeaderboardCategory* category
        = NetworkStatsManager::Instance()->GetCategory(categoryIndex);
    mMyRank = category->mFirstRank;

    if ((mLeaderboardCategory == 0 || mLeaderboardCategory == 2)
        && mMyRank >= 0)
    {
        mFirstVisibleRank = mMyRank - 5;
        int maximum = NetworkStatsManager::Instance()
                          ->GetCategory(categoryIndex)
                          ->mCount
            - 10;
        if (mFirstVisibleRank > maximum)
            mFirstVisibleRank = maximum;
        if (mFirstVisibleRank < 0)
            mFirstVisibleRank = 0;
    }
    else
    {
        mFirstVisibleRank = 0;
    }

    UpdateRows();
}

bool SHOnlineRanking::PopulateRow(int row, int leaderboardIndex)
{
    int categoryIndex = mLeaderboardCategory;
    NetworkLeaderboardCategory* category
        = NetworkStatsManager::Instance()->GetCategory(categoryIndex);
    if (leaderboardIndex >= category->mCount)
        return false;

    if (leaderboardIndex == mMyRank)
    {
        char slideName[3];
        nlSNPrintf(slideName, sizeof(slideName), "%d", row + 1);
        mHighlight->SetActiveSlide(slideName, true, false);
        mHighlight->m_bVisible = true;
    }

    NetworkStatsPlayer* player
        = &NetworkStatsManager::Instance()
               ->GetCategory(categoryIndex)
               ->mPlayers[leaderboardIndex];
    NetworkRankingMeta* metadata
        = &NetworkStatsManager::Instance()
               ->GetCategory(mLeaderboardCategory)
               ->mMetadata[leaderboardIndex];

    for (int stat = 0; stat < 5; ++stat)
    {
        switch (stat)
        {
        case 0:
            nlSNPrintf(mRowText[row][stat], 40, (const u16*)L"%d",
                metadata->mDisplayRank);
            break;
        case 1:
            nlSNPrintf(mRowText[row][stat], 40, (const u16*)L"%ls",
                player->mName);
            break;
        case 2:
            nlStrNCpy(mRowText[row][stat],
                Format(WideString(LookupLocString(
                           "ONLINE_LEADERBOARD_RECORD")),
                    metadata->mWins, metadata->mLosses)
                    .c_str(),
                40);
            break;
        case 3:
            nlStrNCpy(mRowText[row][stat],
                Format(WideString(LookupLocString(
                           "ONLINE_LEADERBOARD_TROPHY_STAT")),
                    42)
                    .c_str(),
                40);
            break;
        case 4:
            nlSNPrintf(mRowText[row][stat], 40, (const u16*)L"%d",
                metadata->mScore);
            break;
        }

        if (stat == 2 || stat == 3)
        {
            FEFinder<TLTextInstance, 3>::FindOrDefault(mRowInstances[row],
                "Team_0", "STAT_2", sLeaderboardTextNames[stat])
                ->SetString(mRowText[row][stat]);
        }
        else
        {
            FEFinder<TLTextInstance, 3>::FindOrDefault(mRowInstances[row],
                "Team_0", sLeaderboardTextNames[stat])
                ->SetString(mRowText[row][stat]);
        }
    }

    return true;
}

void SHOnlineRanking::OnRowPointerEnter(int index, void*)
{
    ++mPointerHoverCounts[index];
}

void SHOnlineRanking::OnRowPointerLeave(int index, void*)
{
    --mPointerHoverCounts[index];
}

void SHOnlineRanking::OnRowPointerPress(int, void*)
{
    for (int i = 0; i < 4; ++i)
        GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
}

void SHOnlineRanking::UpdateHeader()
{
    TLComponentInstance* title
        = FEFinder<TLComponentInstance, 4>::FindOrDefault(
            mPresentation->m_currentSlide, "Layer", "Group", "TITLE2");
    if (mLeaderboardCategory == 0)
    {
        title->SetActiveSlide("DAILY", true, false);
        SetBreadcrumbs(5, 3);
    }
    else if (mLeaderboardCategory == 1)
    {
        title->SetActiveSlide("DAILY LEADERS", true, false);
        SetBreadcrumbs(5, 4);
    }
    else if (mLeaderboardCategory == 2)
    {
        title->SetActiveSlide("MY RANKING", true, false);
        SetBreadcrumbs(5, 0);
    }
    else if (mLeaderboardCategory == 5)
    {
        title->SetActiveSlide("FRIENDS", true, false);
        SetBreadcrumbs(5, 1);
    }
    else if (mLeaderboardCategory == 3)
    {
        title->SetActiveSlide("TOP", true, false);
        SetBreadcrumbs(5, 2);
    }
}

void SHOnlineRanking::OnErrorDismissed()
{
    mErrorPopupOpen = false;
    GameSceneManager::Instance()->Pop();
    FEAudio::PlayAnimAudioEvent(0x4430B152, 0, 0, 1);
    FrontEndPresentation::GetInstance()->Call(
        "TransitionOnlineMatchToMainMenu");
}
