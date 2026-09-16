#include "Game/SH/SHStrikerCupAwards.h"
#include "NL/nlFunction.inl"
#include "Game/FE/feHelpFuncs_decl.h"
#include "Game/SH/SHNavigation.h"
#include "Game/FE/FEAudio.h"
#include "Game/DB/GameProgress.h"
#include "Game/DB/CharacterInfo.h"
#include "Game/DB/StatsTracker.h"
#include "Game/FE/feFinder.inl"
#include "Game/FE/feCupFlow.h"
#include "Game/FE/feInput.h"
#include "Game/FE/fePackage.h"
#include "Game/FE/feScene.h"
#include "Game/FE/tlTextInstance.h"
#include "Game/FE/tlImageInstance.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"

#include "Game/FE/fePresentation.h"
#include "Game/FE/tlComponentInstance.h"
#include "NL/nlBind.h"
#include "Game/FE/feDPD.h"

extern "C" void fn_80207724(int);
extern "C" void fn_802088B4();
extern "C" void fn_80207060(bool);

static inline TLImageInstance* FindCupImage(FEPresentation* presentation,
    const char* name)
{
    return FEFinder<TLImageInstance, 2>::FindOrDefault(
        presentation, "art", "Layer", name);
}

StrikerCupAwardsScene::StrikerCupAwardsScene(int cupPage)
    : mScrollOffset(0)
    , mCupPage(cupPage)
    , mPointerButtonsInitialized(false)
    , mPreviousPagePressed(false)
    , mNextPagePressed(false)
    , mUserTeamPosition(0)
    , mScrollBar()
    , mBackButton()
    , mPlayButton()
    , mDescriptionScroller(0)
    , mPlayButtonInstance(0)
    , mTransitionState(0)
{
    mPlayButton.mContext = 0;

    mPointerHoverCounts[0] = 0;
    mPointerHoverCounts[1] = 0;
    mPointerHoverCounts[2] = 0;
    mPointerHoverCounts[3] = 0;

    mTeamButtons[0].mContext = 0;
    mTeamButtons[0].mSpeakerEnabled = false;
    mTeamButtons[1].mContext = (void*)1;
    mTeamButtons[1].mSpeakerEnabled = false;
    mTeamButtons[2].mContext = (void*)2;
    mTeamButtons[2].mSpeakerEnabled = false;
    mTeamButtons[3].mContext = (void*)3;
    mTeamButtons[3].mSpeakerEnabled = false;
}

StrikerCupAwardsScene::~StrikerCupAwardsScene()
{
}

void StrikerCupAwardsScene::SceneCreated()
{
    FEPresentation* presentation = mFEScene->m_pFEPackage->GetPresentation();
    CupManager* cupManager = g_pCupManager;
    int teamCount = cupManager->GetNumPlayingTeams();
    PlayerStats stats[10];

    for (int team = 0; team < teamCount; ++team)
    {
        stats[team] = cupManager->GetTeamStatsByIndex(team).mPlayerTotalStats;
    }

    if (mCupPage == 5)
    {
        StatsTracker::Instance()->GetSortedStats(
            stats, teamCount, mAwardTeamIndices, teamCount, STATS_GOALS_FOR, SORT_DESCENDING);
    }
    else
    {
        StatsTracker::Instance()->GetSortedStats(
            stats, teamCount, mAwardTeamIndices, teamCount, STATS_0C, SORT_ASCENDING);
    }

    for (int row = 0; row < 4; ++row)
    {
        char rowName[8];
        nlSNPrintf(rowName, sizeof(rowName), "TEAM_%d", row);
        mTeamRows[row] = FEFinder<TLComponentInstance, 4>::FindOrDefault<TLSlide>(
            presentation->m_currentSlide, "Layer", "award_screen", rowName);
    }

    mUserTeamHighlight = FEFinder<TLComponentInstance, 4>::FindOrDefault<TLSlide>(
        presentation->m_currentSlide, "Layer", "award_screen", "highlite_moving");

    for (int team = 0; team < teamCount; ++team)
    {
        TeamStats teamStats = cupManager->GetTeamStatsByIndex(mAwardTeamIndices[team]);
        if (teamStats.mTeamIndex == cupManager->GetUserSelectedCupTeam() && team >= 4)
        {
            mUserTeamPosition = team;
            mScrollOffset = team - 3;
            break;
        }
        if (teamStats.mTeamIndex == cupManager->GetUserSelectedCupTeam())
        {
            mUserTeamPosition = team;
        }
    }

    UpdateAwards();

    TLTextInstance* description = FEFinder<TLTextInstance, 3>::FindOrDefault<TLSlide>(
        presentation->m_currentSlide, "Layer", "award_screen", "Description_clip_right");
    description->m_bVisible = false;

    SHNavigation* navigation = GetNavigationScene();
    TLComponentInstance* backButton = 0;
    if (navigation != 0)
    {
        backButton = navigation->GetButton(4);
        mPlayButtonInstance = navigation->GetButton(0x10);
        mPageControls = navigation->GetPageControls();
        mPageControls->SetButtonState(1, true, true);
        mPageControls->SetButtonState(0, true, true);
    }

    mBackButton.SetButtonInstance(backButton);
    fn_80207724(mCupPage);

    TLComponentInstance* scrollBar = FEFinder<TLComponentInstance, 4>::Find<TLSlide>(
        presentation->m_currentSlide, "Layer", "award_screen", "scrollbar");
    mScrollBar.SetComponent(scrollBar);
    mScrollBar.SetRange(teamCount - 4);
    mScrollBar.SetValue(mScrollOffset);

    for (int pad = 0; pad < 4; ++pad)
    {
        GetPointerInstance(pad)->SetActiveSlide("waiting", true, false);
    }
}

void StrikerCupAwardsScene::Update(float deltaTime)
{
    BaseSceneHandler::Update(deltaTime);

    if (!mScrollBar.mInitialized)
    {
        mScrollBar.Initialize();
    }

    g_pCupManager->GetNumPlayingTeams();

    int state = mTransitionState;
    if (state == 0 || (unsigned int)(state - 2) <= 1)
    {
        TLSlide* slide = mPresentation->m_currentSlide;
        if (slide->GetCurrentTime() < slide->GetStartTime() + slide->GetDuration())
        {
            for (int pad = 0; pad < 4; ++pad)
            {
                GetPointerInstance(pad)->SetActiveSlide("waiting", true, false);
            }
            return;
        }

        if (state == 0)
        {
            if (!mPointerButtonsInitialized)
            {
                GetNavigationScene()->SetButtons(0x1F, false);
                fn_802088B4();
                InitializePointerButtons();
                mPointerButtonsInitialized = true;
            }
            mTransitionState = 1;
        }
        else if (state == 2)
        {
            if (mPreviousPagePressed)
            {
                CycleCupPage(mCupPage, false);
                return;
            }
            if (mNextPagePressed)
            {
                CycleCupPage(mCupPage, true);
                return;
            }
            fn_80207060(false);
            return;
        }
        else if (state == 3)
        {
            HandleCupBack(mCupPage);
            return;
        }
    }

    for (int pad = 0; pad < 4; ++pad)
    {
        TLComponentInstance* pointer = GetPointerInstance(pad);
        if (g_pFEInput->m_InputLockDepth == 0)
        {
            if (pad != gFEControllerIndex)
            {
                pointer->SetActiveSlide("waiting", true, false);
                continue;
            }

            if (mPointerHoverCounts[pad] > 0 || mBackButton.mPointerInside[pad]
                || mPageControls->mPointerInside[0] || mPageControls->mPointerInside[1])
            {
                pointer->SetActiveSlide("A", true, false);
            }
            else
            {
                pointer->SetActiveSlide("cursor", true, false);
            }
        }

        u8 valid = 1;
        FEPointerEvent event;
        event.mIndex = pad;
        event.mPosition = GetPointerPosition(pad, &valid);
        event.mPressed = g_pFEInput->JustPressed((eFEINPUT_PAD)pad, 0x1E, true, 0);
        event.mReleased = g_pFEInput->JustReleased((eFEINPUT_PAD)pad, 0x1E, true, 0);

        for (int row = 0; row < 4; ++row)
        {
            mTeamButtons[row].HandlePointerEvent(&event);
        }

        mPageControls->Update(event, deltaTime);
        mPlayButton.HandlePointerEvent(&event);
        mScrollBar.Update(event, deltaTime);

        if (mBackButton.UpdateBackButton(event, deltaTime))
        {
            mTransitionState = 3;
            SHNavigation* navigation = GetNavigationScene();
            if (navigation != 0)
            {
                navigation->HideButtons();
            }
            mPresentation->SetActiveSlide("out", true);
            return;
        }

        if (mPageControls->IsButtonPressed(1) || mPageControls->IsButtonPressed(0))
        {
            FEAudio::PlayAnimAudioEvent(0x375C885A, 0, 0, true);
            FEAudio::PlayAnimAudioEvent(0x304FDD1E, 0, 0, true);
            mTransitionState = 2;
            GetNavigationScene();
            mPresentation->SetActiveSlide("out", true);

            if (mPageControls->IsButtonPressed(1))
            {
                mPreviousPagePressed = true;
            }
            else if (mPageControls->IsButtonPressed(0))
            {
                mNextPagePressed = true;
            }
            return;
        }
    }

    if (mScrollBar.IsScrolling(1, true))
    {
        ++mScrollOffset;
        UpdateAwards();
    }
    else if (mScrollBar.IsScrolling(0, true))
    {
        --mScrollOffset;
        UpdateAwards();
    }
}

void StrikerCupAwardsScene::UpdateAwards()
{
    bool userTeamVisible = false;
    for (int row = 0; row < 4; ++row)
    {
        if (PopulateTeamRow(row, row + mScrollOffset))
        {
            userTeamVisible = true;
        }
    }

    CupManager* cupManager = g_pCupManager;
    int teamCount = cupManager->GetNumPlayingTeams();
    for (int team = 0; team < teamCount; ++team)
    {
        TeamStats stats = cupManager->GetTeamStatsByIndex(mAwardTeamIndices[team]);
        if (stats.mTeamIndex == cupManager->GetUserSelectedCupTeam())
        {
            mUserTeamPosition = team;
            break;
        }
    }

    mUserTeamHighlight->m_bVisible = userTeamVisible;
}

bool StrikerCupAwardsScene::PopulateTeamRow(int row, int teamPosition)
{
    CupManager* cupManager = g_pCupManager;
    TLComponentInstance* rowInstance;
    u16 statValue;
    if (row >= cupManager->GetNumPlayingTeams())
    {
        mTeamRows[row]->m_bVisible = false;
        mTeamRows[row]->m_bVisible = false;
        return false;
    }

    TeamStats teamStats;
    teamStats = cupManager->GetTeamStatsByIndex(mAwardTeamIndices[teamPosition]);
    PlayerStats playerStats = teamStats.mPlayerTotalStats;
    rowInstance = mTeamRows[row];

    if (mCupPage == 5)
    {
        statValue = playerStats.mNumGoalsFor;
    }
    else
    {
        statValue = playerStats.unknown_0x12;
    }

    TLImageInstance* captainImage = FEFinder<TLImageInstance, 2>::FindOrDefault(
        rowInstance, "Team_content", "left", "captain");

    char captainName[24];
    nlSNPrintf(captainName, sizeof(captainName), "captain_%s_s",
        GetCharacterInfo(GetCharacterIndexFromCaptain(teamStats.mTeamIndex)).mName);

    TLImageInstance* sourceImage = FindCupImage(
        mFEScene->m_pFEPackage->GetPresentation(), captainName);
    if (sourceImage->m_pTextureResource != 0)
    {
        captainImage->m_pTextureResource = sourceImage->m_pTextureResource;
    }

    char statText[4];
    nlSNPrintf(statText, sizeof(statText), "%d", statValue);
    nlStrToWcs(statText, mStatText[row], 4);
    TLTextInstance* statTextInstance = FEFinder<TLTextInstance, 3>::FindOrDefault(
        rowInstance, "Team_content", "stat_0");
    statTextInstance->SetString(mStatText[row]);

    if (teamStats.mTeamIndex == cupManager->GetUserSelectedCupTeam())
    {
        char slideName[12];
        nlSNPrintf(slideName, sizeof(slideName), "Slide%d", row);
        mUserTeamHighlight->SetActiveSlide(slideName, true, false);
        return true;
    }

    return false;
}

void StrikerCupAwardsScene::InitializePointerButtons()
{
    typedef Detail::MemFunImpl<void, void (StrikerCupAwardsScene::*)(unsigned int, void*)> PointerMethod;
    typedef BindExp3<void, PointerMethod, StrikerCupAwardsScene*, Placeholder<0>, Placeholder<1> > PointerBinding;

    FEPointerListener::Callback componentOver(
        PointerBinding(MemFun(&StrikerCupAwardsScene::OnButtonPointerEnter), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback componentOff(
        PointerBinding(MemFun(&StrikerCupAwardsScene::OnButtonPointerLeave), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback componentSelect(
        PointerBinding(MemFun(&StrikerCupAwardsScene::OnButtonPointerPress), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback teamOver(
        PointerBinding(MemFun(&StrikerCupAwardsScene::OnTeamPointerEnter), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback teamOff(
        PointerBinding(MemFun(&StrikerCupAwardsScene::OnTeamPointerLeave), this, Placeholder<0>(), Placeholder<1>()));

    SetPlayButtonBounds(&mPlayButton, mPlayButtonInstance);
    mPlayButton.SetPointerEnterCallback(componentOver);
    mPlayButton.SetPointerLeaveCallback(componentOff);
    mPlayButton.SetPointerPressCallback(componentSelect);

    for (int i = 0; i < 4; ++i)
    {
        mTeamButtons[i].mContext = (void*)i;
        mTeamButtons[i].SetInstanceBounds(
            mTeamRows[i], true, 0.0f, 0.0f, 1.0f, 1.0f);
        mTeamButtons[i].SetMaxX(200.0f);
        mTeamButtons[i].SetPointerEnterCallback(teamOver);
        mTeamButtons[i].SetPointerLeaveCallback(teamOff);
    }
}

void StrikerCupAwardsScene::OnButtonPointerEnter(unsigned int index, void* context)
{
    ++mPointerHoverCounts[index];
    if (context == 0 && !mPlayButton.HasOtherPointerState(1, index))
    {
        mPlayButtonInstance->SetActiveSlide("over", true, false);
        FEAudio::PlayAnimAudioEvent(0xAA73EF34, 0, 0, 1);
        mPlayButton.SetPointerState(1, index);
    }
}

void StrikerCupAwardsScene::OnButtonPointerLeave(unsigned int index, void* context)
{
    --mPointerHoverCounts[index];
    if (context == 0 && !mPlayButton.HasOtherPointerState(1, index))
    {
        mPlayButtonInstance->SetActiveSlide("off", true, false);
        mPlayButton.SetPointerState(0, index);
    }
}

void StrikerCupAwardsScene::OnButtonPointerPress(unsigned int, void* context)
{
    mPlayButtonPressed = true;
    for (int i = 0; i < 4; ++i)
    {
        GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
    }

    switch ((int)context)
    {
    case 0:
        FEAudio::PlayAnimAudioEvent(0x6E5C794C, 0, 0, 1);
        FEAudio::PlayAnimAudioEvent(0x2ECB0035, 0, 0, 1);
        mTransitionState = 2;

        SHNavigation* object = GetNavigationScene();
        if (object != 0)
        {
            object->HideButtons();
        }

        mPresentation->SetActiveSlide("out", true);
        break;
    }
}

void StrikerCupAwardsScene::OnTeamPointerEnter(unsigned int index, void* context)
{
    mTeamButtons[(unsigned int)context].SetPointerState(1, index);
}

void StrikerCupAwardsScene::OnTeamPointerLeave(unsigned int index, void* context)
{
    unsigned int item = (unsigned int)context;
    mTeamButtons[item].SetPointerState(0, index);
    mTeamRows[item]->SetActiveSlide("off", true, false);
    PopulateTeamRow(item, item + mScrollOffset);
}
