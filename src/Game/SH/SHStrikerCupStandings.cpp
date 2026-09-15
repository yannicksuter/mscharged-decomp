#include "Game/SH/SHNavigation.h"
#include "NL/nlFunction.inl"
#include "Game/FE/feHelpFuncs_decl.h"
#include "Game/SH/SHStrikerCupStandings.h"

#include "Game/DB/CharacterInfo.h"
#include "Game/DB/GameProgress.h"
#include "Game/DB/StatsTracker.h"
#include "Game/GameSceneManager.h"
#include "Game/Render/RLViewLayers.h"
#include "Game/FE/FEAudio.h"
#include "Game/FE/feCupFlow.h"
#include "Game/FE/feDPD.h"
#include "Game/FE/feFinder.inl"
#include "Game/FE/feInput.h"
#include "Game/FE/fePackage.h"
#include "Game/FE/fePageControls.h"
#include "Game/FE/fePresentation.h"
#include "Game/FE/fePopupMenu.h"
#include "Game/FE/feScene.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlImageInstance.h"
#include "Game/FE/tlTextInstance.h"
#include "NL/nlBind.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"

class SHNavigation;

extern "C" void fn_80208950(TLComponentInstance*, unsigned short*, unsigned long);
extern "C" void fn_80207724(int);
extern "C" void fn_802088B4();
extern "C" void fn_80207060(bool);

static inline TLImageInstance* FindCupImage(FEPresentation* presentation,
    const char* name)
{
    return FEFinder<TLImageInstance, TLAT_IMAGE>::FindOrDefault(
        presentation, "art", "Layer", name);
}

StrikerCupStandingsScene::StrikerCupStandingsScene()
    : mScrollOffset(0)
    , mUserTeamPosition(0)
    , mPointerButtonsInitialized(false)
    , mPreviousPagePressed(false)
    , mNextPagePressed(false)
    , mScrollBar()
    , mBackButton()
    , mHelpButton()
    , mBracketButton()
    , mTeamButtons()
    , mBracketButtonInstance(0)
    , mTransitionState(0)
{
    mBracketButton.mContext = 0;
    mHelpButton.mContext = (void*)1;

    mPointerHoverCounts[0] = 0;
    mPointerHoverCounts[1] = 0;
    mPointerHoverCounts[2] = 0;
    mPointerHoverCounts[3] = 0;

    for (int i = 0; i < 5; ++i)
    {
        mTeamButtons[i].mContext = (void*)i;
        mTeamButtons[i].mSpeakerEnabled = false;
    }
}

StrikerCupStandingsScene::~StrikerCupStandingsScene()
{
}

void StrikerCupStandingsScene::SceneCreated()
{
    int numTeams;
    CupManager* cupManager;
    FEPresentation* presentation;

    presentation = mFEScene->m_pFEPackage->GetPresentation();
    cupManager = g_pCupManager;
    numTeams = cupManager->GetNumPlayingTeams();

    TLComponentInstance* title =
        FEFinder<TLComponentInstance, TLAT_COMPONENT>::FindOrDefault(
            presentation->m_currentSlide, "Layer", "stats_screen", "TITLE2");
    fn_80208950(title, mTitleText, 64);

    TLComponentInstance* help =
        FEFinder<TLComponentInstance, TLAT_COMPONENT>::FindOrDefault(
            presentation->m_currentSlide, "Layer", "HELP_BUTTON");
    if (IsWidescreen())
        help->SetActiveSlide("16:9", true, false);
    else
        help->SetActiveSlide("4:3", true, false);
    mHelpButtonInstance =
        FEFinder<TLComponentInstance, TLAT_COMPONENT>::FindOrDefault(
            help->GetActiveSlide(), "HELP");

    StatsTracker::Instance()->GetSortedTeamStats(
        cupManager->pGetTeamStatsByIndex(0), numTeams, mStandingsIndices,
        numTeams);

    for (int i = 0; i < 5; ++i)
    {
        char rowName[8];
        nlSNPrintf(rowName, sizeof(rowName), "Team_%d", i);
        mTeamRows[i] =
            FEFinder<TLComponentInstance, TLAT_COMPONENT>::FindOrDefault(
                presentation->m_currentSlide, "Layer", "stats_screen", rowName);
    }

    for (int i = 0; i < numTeams; ++i)
    {
        TeamStats stats = cupManager->GetTeamStatsByIndex(
            (u16)mStandingsIndices[i]);
        if (stats.mTeamIndex == cupManager->GetUserSelectedCupTeam() && i >= 5)
        {
            mUserTeamPosition = i;
            mScrollOffset = i - 4;
            break;
        }
        if (stats.mTeamIndex == cupManager->GetUserSelectedCupTeam())
            mUserTeamPosition = i;
    }

    mWhiteLine = FEFinder<TLComponentInstance, TLAT_COMPONENT>::FindOrDefault(
        presentation->m_currentSlide, "Layer", "stats_screen", "white_line");
    mHighlight = FEFinder<TLComponentInstance, TLAT_COMPONENT>::FindOrDefault(
        presentation->m_currentSlide, "Layer", "stats_screen", "highlight");
    mMovingHighlight =
        FEFinder<TLComponentInstance, TLAT_COMPONENT>::FindOrDefault(
            presentation->m_currentSlide, "Layer", "stats_screen",
            "highlite_moving");
    TLComponentInstance* scrollBar =
        FEFinder<TLComponentInstance, TLAT_COMPONENT>::Find<>(
            presentation->m_currentSlide, "Layer", "stats_screen", "scrollbar");
    mScrollBar.SetComponent(scrollBar);
    mScrollBar.SetRange(numTeams - 5);
    mScrollBar.SetValue(mScrollOffset);

    UpdateStandings();

    char slideName[24];
    nlSNPrintf(slideName, sizeof(slideName), "slide%d",
        mUserTeamPosition - mScrollOffset);
    mMovingHighlight->SetActiveSlide(slideName, true, false);

    for (int i = 0; i < 4; ++i)
        GetPointerInstance(i)->SetActiveSlide("waiting", true, false);

    SHNavigation* navigation = GetNavigationScene();
    TLComponentInstance* backButton = 0;
    if (navigation != 0)
    {
        backButton = navigation->GetButton(4);
        mBracketButtonInstance = navigation->GetButton(16);
        mPageControls = navigation->GetPageControls();
        mPageControls->SetButtonState(1, true, true);
        mPageControls->SetButtonState(0, true, true);
    }
    mBackButton.SetButtonInstance(backButton);
    fn_80207724(4);
}

void StrikerCupStandingsScene::Update(float fDeltaT)
{
    BaseSceneHandler::Update(fDeltaT);
    if (!mScrollBar.mInitialized)
        mScrollBar.Initialize();

    g_pCupManager->GetNumPlayingTeams();
    if (mTransitionState == 0 || mTransitionState == 2 || mTransitionState == 3)
    {
        TLSlide* slide = mPresentation->m_currentSlide;
        if (slide->GetCurrentTime() < slide->GetStartTime() + slide->GetDuration())
        {
            for (int i = 0; i < 4; ++i)
                GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
            return;
        }
        if (mTransitionState == 0)
        {
            if (!mPointerButtonsInitialized)
            {
                SHNavigation* navigation = GetNavigationScene();
                navigation->SetButtons(0x1F, false);
                fn_802088B4();
                InitializePointerButtons();
                mPointerButtonsInitialized = true;
            }
            mTransitionState = 1;
        }
        else if (mTransitionState == 2)
        {
            if (mPreviousPagePressed)
            {
                CycleCupPage(4, false);
                return;
            }
            if (mNextPagePressed)
            {
                CycleCupPage(4, true);
                return;
            }
            fn_80207060(false);
            return;
        }
        else if (mTransitionState == 3)
        {
            HandleCupBack(4);
            return;
        }
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
                pointer->SetActiveSlide("A", true, false);
            else
                pointer->SetActiveSlide("cursor", true, false);
        }

        u8 valid = 1;
        FEPointerEvent event;
        event.mIndex = i;
        event.mPosition = GetPointerPosition(i, &valid);
        event.mPressed = g_pFEInput->JustPressed((eFEINPUT_PAD)i, 30, true, 0);
        event.mReleased = g_pFEInput->JustReleased((eFEINPUT_PAD)i, 30, true, 0);
        for (int j = 0; j < 5; ++j)
            mTeamButtons[j].HandlePointerEvent(&event);
        mHelpButton.HandlePointerEvent(&event);
        mPageControls->Update(event, fDeltaT);
        mBracketButton.HandlePointerEvent(&event);
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

        if (g_pFEInput->m_InputLockDepth == 0)
        {
            if (mPageControls->IsButtonPressed(1)
                || mPageControls->IsButtonPressed(0))
            {
                FEAudio::PlayAnimAudioEvent(0x304FDD1E, 0, 0, 1);
                FEAudio::PlayAnimAudioEvent(0x375C885A, 0, 0, 1);
                mTransitionState = 2;
                mPresentation->SetActiveSlide("out", true);
                mPresentation->Update(0.0f);
                if (mPageControls->IsButtonPressed(1))
                    mPreviousPagePressed = true;
                else if (mPageControls->IsButtonPressed(0))
                    mNextPagePressed = true;
                return;
            }
        }
    }

    if (mScrollBar.IsScrolling(1, true))
    {
        ++mScrollOffset;
        UpdateStandings();
    }
    else if (mScrollBar.IsScrolling(0, true))
    {
        --mScrollOffset;
        UpdateStandings();
    }
}

void StrikerCupStandingsScene::UpdateStandings()
{
    bool userVisible = false;
    for (int i = 0; i < 5; ++i)
    {
        if (PopulateTeamRow(i, i + mScrollOffset))
            userVisible = true;
    }

    CupManager* cupManager = g_pCupManager;
    int numTeams = cupManager->GetNumPlayingTeams();
    for (int i = 0; i < numTeams; ++i)
    {
        TeamStats stats = cupManager->GetTeamStatsByIndex(
            (u16)mStandingsIndices[i]);
        if (stats.mTeamIndex == cupManager->GetUserSelectedCupTeam())
        {
            mUserTeamPosition = i;
            break;
        }
    }

    char slideName[24];
    nlSNPrintf(slideName, sizeof(slideName), "slide%d",
        mUserTeamPosition - mScrollOffset);
    mMovingHighlight->SetActiveSlide(slideName, true, false);
    mHighlight->m_bVisible = userVisible;

    int linePosition = g_pCupManager->fn_8010D600() - mScrollOffset;
    if (linePosition <= 5)
    {
        char lineName[2];
        nlSNPrintf(lineName, sizeof(lineName), "%d", linePosition);
        mWhiteLine->SetActiveSlide(lineName, true, false);
        mWhiteLine->m_bVisible = true;
    }
    else
    {
        mWhiteLine->m_bVisible = false;
    }
}

bool StrikerCupStandingsScene::PopulateTeamRow(int row, int teamPosition)
{
    bool isUserTeam = false;
    CupManager* cupManager = g_pCupManager;
    if (row >= cupManager->GetNumPlayingTeams())
    {
        mTeamRows[row]->m_bVisible = false;
        return false;
    }

    TeamStats stats = cupManager->GetTeamStatsByIndex(
        (u16)mStandingsIndices[teamPosition]);

    TLImageInstance* offCaptain =
        FEFinder<TLImageInstance, TLAT_IMAGE>::FindOrDefault(
            mTeamRows[row], "off", "CHALLENGE_0", "00_dummy_texture");
    char imageName[24];
    nlSNPrintf(imageName, sizeof(imageName), "captain_%s_s",
        GetCharacterInfo(GetCharacterIndexFromCaptain(stats.mTeamIndex)).mName);
    TLImageInstance* offImage = FindCupImage(
        mFEScene->m_pFEPackage->GetPresentation(), imageName);
    if (offImage->m_pTextureResource != 0)
        offCaptain->m_pTextureResource = offImage->m_pTextureResource;

    TLImageInstance* overCaptain = FEFinder<TLImageInstance, TLAT_IMAGE>::FindOrDefault(
        mTeamRows[row], "over", "CHALLENGE_0", "00_dummy_texture");
    char overImageName[24];
    nlSNPrintf(overImageName, sizeof(overImageName), "captain_%s_s",
        GetCharacterInfo(GetCharacterIndexFromCaptain(stats.mTeamIndex)).mName);
    TLImageInstance* overImage = FindCupImage(
        mFEScene->m_pFEPackage->GetPresentation(), overImageName);
    if (overImage->m_pTextureResource != 0)
        overCaptain->m_pTextureResource = overImage->m_pTextureResource;

    for (int i = 0; i < 7; ++i)
    {
        int value = 0;
        switch (i)
        {
        case 0:
            value = teamPosition + 1;
            break;
        case 1:
            value = stats.unknown_0x10;
            break;
        case 2:
            value = stats.unknown_0x12;
            break;
        case 3:
            value = stats.unknown_0x14;
            break;
        case 4:
            value = stats.mPlayerTotalStats.mNumGoalsFor;
            break;
        case 5:
            value = stats.mPlayerTotalStats.unknown_0x12;
            break;
        case 6:
            value = stats.unknown_0x16;
            break;
        }

        char number[4];
        nlSNPrintf(number, sizeof(number), "%d", value);
        nlStrToWcs(number, mStatText[row][i], 4);

        char statName[8];
        nlSNPrintf(statName, sizeof(statName), "stat_%d", i);
        TLTextInstance* text =
            FEFinder<TLTextInstance, TLAT_TEXT>::FindOrDefault(
                mTeamRows[row], "off", "CHALLENGE_0", statName);
        text->SetString(mStatText[row][i]);
        text = FEFinder<TLTextInstance, TLAT_TEXT>::FindOrDefault(
            mTeamRows[row], "over", "CHALLENGE_0", statName);
        text->SetString(mStatText[row][i]);
    }

    if (stats.mTeamIndex == cupManager->GetUserSelectedCupTeam())
    {
        char positionName[12];
        nlSNPrintf(positionName, sizeof(positionName), "position%d", row + 1);
        mHighlight->SetActiveSlide(positionName, true, false);
        isUserTeam = true;
    }
    return isUserTeam;
}

void StrikerCupStandingsScene::InitializePointerButtons()
{
    typedef Detail::MemFunImpl<void, void (StrikerCupStandingsScene::*)(unsigned int, void*)> PointerMethod;
    typedef BindExp3<void, PointerMethod, StrikerCupStandingsScene*, Placeholder<0>, Placeholder<1> > PointerBinding;

    FEPointerListener::Callback componentOver(
        PointerBinding(MemFun(&StrikerCupStandingsScene::OnButtonPointerEnter), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback componentOff(
        PointerBinding(MemFun(&StrikerCupStandingsScene::OnButtonPointerLeave), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback componentSelect(
        PointerBinding(MemFun(&StrikerCupStandingsScene::OnButtonPointerPress), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback teamOver(
        PointerBinding(MemFun(&StrikerCupStandingsScene::OnTeamPointerEnter), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback teamOff(
        PointerBinding(MemFun(&StrikerCupStandingsScene::OnTeamPointerLeave), this, Placeholder<0>(), Placeholder<1>()));

    SetPlayButtonBounds(&mBracketButton, mBracketButtonInstance);
    mBracketButton.SetPointerEnterCallback(componentOver);
    mBracketButton.SetPointerLeaveCallback(componentOff);
    mBracketButton.SetPointerPressCallback(componentSelect);

    TLImageInstance* upperOver = FEFinder<TLImageInstance, TLAT_IMAGE>::FindOrDefault(
        mHelpButtonInstance, "OVER", "list_high_250x60");
    feVector3 upperPosition = mHelpButtonInstance->GetAssetPosition();
    mHelpButton.SetInstanceBounds(
        upperOver, true, upperPosition.f.x, upperPosition.f.y, 1.0f, 1.0f);
    mHelpButton.SetPointerEnterCallback(componentOver);
    mHelpButton.SetPointerLeaveCallback(componentOff);
    mHelpButton.SetPointerPressCallback(componentSelect);

    for (int i = 0; i < 5; ++i)
    {
        feVector3 position = mTeamRows[i]->GetAssetPosition();
        TLInstance* over = FEFinder<TLInstance, TLAT_IMAGE>::FindOrDefault(
            mTeamRows[i], "over", "CHALLENGE_0", "list_back_480x70 ");
        mTeamButtons[i].SetInstanceBounds(
            over, true, position.f.x, position.f.y, 0.95f, 0.6f);
        mTeamButtons[i].SetPointerEnterCallback(teamOver);
        mTeamButtons[i].SetPointerLeaveCallback(teamOff);
    }
}

void StrikerCupStandingsScene::OnButtonPointerEnter(unsigned int index, void* context)
{
    ++mPointerHoverCounts[index];
    if (context == 0)
    {
        if (!mBracketButton.HasOtherPointerState(1, index))
        {
            mBracketButtonInstance->SetActiveSlide("over", true, false);
            FEAudio::PlayAnimAudioEvent(0xAA73EF34, 0, 0, 1);
            mBracketButton.SetPointerState(1, index);
        }
        return;
    }
    if (context == (void*)1)
    {
        if (!mHelpButton.HasOtherPointerState(1, index))
        {
            mHelpButtonInstance->SetActiveSlide("over", true, false);
            FEAudio::PlayAnimAudioEvent(0xACCDCA48, 0, 0, 1);
            mHelpButton.SetPointerState(1, index);
        }
    }
}

void StrikerCupStandingsScene::OnButtonPointerLeave(unsigned int index, void* context)
{
    --mPointerHoverCounts[index];
    if (context == 0)
    {
        if (!mBracketButton.HasOtherPointerState(1, index))
        {
            mBracketButtonInstance->SetActiveSlide("off", true, false);
            mBracketButton.SetPointerState(0, index);
        }
        return;
    }
    if (context == (void*)1)
    {
        if (!mHelpButton.HasOtherPointerState(1, index))
        {
            mHelpButtonInstance->SetActiveSlide("off", true, false);
            mHelpButton.SetPointerState(0, index);
        }
    }
}

void StrikerCupStandingsScene::OnButtonPointerPress(unsigned int, void* context)
{
    mBracketPressed = true;
    for (int i = 0; i < 4; ++i)
    {
        GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
    }

    switch ((unsigned int)context)
    {
    case 0:
    {
        FEAudio::PlayAnimAudioEvent(0x6E5C794C, 0, 0, 1);
        FEAudio::PlayAnimAudioEvent(0x2ECB0035, 0, 0, 1);
        mTransitionState = 2;

        SHNavigation* object = GetNavigationScene();
        if (object != 0)
        {
            object->HideButtons();
        }

        mPresentation->SetActiveSlide("out", true);
        mPresentation->Update(0.0f);
        break;
    }
    case 1:
    {
        FEAudio::PlayAnimAudioEvent(0xF0AFD586, 0, 0, 1);
        FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push(
            (SceneList)10, SCREEN_NOTHING, false);
        popup->Create((ePopupMenu)0x26, FEPopupMenu::Nothing);
        break;
    }
    }
}

void StrikerCupStandingsScene::OnTeamPointerEnter(unsigned int index, void* context)
{
    mTeamButtons[(unsigned int)context].SetPointerState(1, index);
}

void StrikerCupStandingsScene::OnTeamPointerLeave(unsigned int index, void* context)
{
    unsigned int item = (unsigned int)context;
    mTeamRows[item]->SetActiveSlide("off", true, false);
    mTeamButtons[item].SetPointerState(0, index);
}
