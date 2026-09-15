#include "Game/SH/SHCupFinalRounds.h"
#include "NL/nlFunction.inl"
#include "Game/FE/feHelpFuncs_decl.h"
#include "Game/SH/SHNavigation.h"
#include "Game/FE/FEAudio.h"

#include "Game/FE/fePresentation.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlImageInstance.h"
#include "Game/FE/tlTextInstance.h"
#include "Game/FE/feFinder.inl"
#include "Game/FE/feScene.h"
#include "Game/FE/fePackage.h"
#include "Game/FE/feInput.h"
#include "Game/FE/feCupFlow.h"
#include "NL/nlBind.h"
#include "NL/nlPrint.h"
#include "Game/FE/feDPD.h"
#include "Game/FE/fePopupMenu.h"
#include "Game/GameSceneManager.h"
#include "Game/DB/GameProgress.h"
#include "Game/DB/CharacterInfo.h"
#include "Game/SH/SHGameResults.h"
#include "NL/nlString.h"

class SHNavigation;

extern "C" void fn_80208950(TLComponentInstance*, unsigned short*, unsigned long);
extern "C" void fn_80207724(int);
extern "C" void fn_802088B4();
extern "C" void fn_80207060(bool);

static inline TLImageInstance* FindCupImage(FEPresentation* presentation, const char* name)
{
    return FEFinder<TLImageInstance, TLAT_IMAGE>::FindOrDefault(
        presentation, "art", "Layer", name);
}

CupFinalRoundsScene::CupFinalRoundsScene()
    : mPointerButtonsInitialized(false)
    , mPreviousPagePressed(false)
    , mNextPagePressed(false)
    , mBackButton()
    , mMatchupButtons()
    , mBracketButton()
    , mBracketButtonInstance(0)
    , mTransitionState(0)
{
    mBracketButton.mContext = 0;
    mPointerHoverCounts[0] = 0;
    mPointerHoverCounts[1] = 0;
    mPointerHoverCounts[2] = 0;
    mPointerHoverCounts[3] = 0;
    mMatchupButtons[0].mContext = 0;
    mMatchupButtons[1].mContext = (void*)1;
    mMatchupButtons[2].mContext = (void*)2;
}

CupFinalRoundsScene::~CupFinalRoundsScene()
{
}

void CupFinalRoundsScene::SceneCreated()
{
    CupManager* cupManager = g_pCupManager;
    FEPresentation* presentation = mFEScene->m_pFEPackage->GetPresentation();
    TLComponentInstance* title = FEFinder<TLComponentInstance, TLAT_COMPONENT>::FindOrDefault(
        presentation->m_currentSlide, "Layer", "TITLE2");
    fn_80208950(title, mTitleText, 64);

    for (int i = 0; i < 3; ++i)
    {
        BasicGameInfo* game = cupManager->GetMatchupInfo(2, (short)i, 0);
        char matchupName[8];
        nlSNPrintf(matchupName, sizeof(matchupName), "game_%d", i);
        mMatchupInstances[i] = FEFinder<TLComponentInstance, TLAT_COMPONENT>::FindOrDefault(
            presentation->m_currentSlide, "Layer", matchupName);
        UnidentifiedTLGroupInstance* group =
            FEFinder<UnidentifiedTLGroupInstance, TLAT_GROUP>::FindOrDefault(
                mMatchupInstances[i], "Slide1", matchupName);
        PopulateMatchup(game, group, i);
        group = FEFinder<UnidentifiedTLGroupInstance, TLAT_GROUP>::FindOrDefault(
            mMatchupInstances[i], "Slide2", matchupName);
        PopulateMatchup(game, group, i);
    }

    UpdateRoundHighlight();
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
    fn_80207724(3);
    for (int i = 0; i < 4; ++i)
    {
        GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
    }
}

void CupFinalRoundsScene::Update(float fDeltaT)
{
    BaseSceneHandler::Update(fDeltaT);
    if (mTransitionState == 0 || mTransitionState == 2 || mTransitionState == 3)
    {
        TLSlide* slide = mPresentation->m_currentSlide;
        if (slide->GetCurrentTime() < slide->GetStartTime() + slide->GetDuration())
        {
            for (int i = 0; i < 4; ++i)
            {
                GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
            }
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
                CycleCupPage(3, false);
                return;
            }
            if (mNextPagePressed)
            {
                CycleCupPage(3, true);
                return;
            }
            fn_80207060(false);
            return;
        }
        else if (mTransitionState == 3)
        {
            HandleCupBack(3);
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
                || mPageControls->mPointerInside[0] || mPageControls->mPointerInside[1])
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
        mPageControls->Update(event, fDeltaT);
        mBracketButton.HandlePointerEvent(&event);
        for (int j = 0; j < 3; ++j)
        {
            mMatchupButtons[j].HandlePointerEvent(&event);
        }
        if (mBackButton.UpdateBackButton(event, fDeltaT))
        {
            mTransitionState = 3;
            SHNavigation* navigation = GetNavigationScene();
            if (navigation != 0)
                navigation->HideButtons();
            mPresentation->SetActiveSlide("out", true);
            return;
        }

        if (g_pFEInput->m_InputLockDepth == 0)
        {
            if (mPageControls->IsButtonPressed(1) || mPageControls->IsButtonPressed(0))
            {
                FEAudio::PlayAnimAudioEvent(0x304FDD1E, 0, 0, 1);
                FEAudio::PlayAnimAudioEvent(0x375C885A, 0, 0, 1);
                mTransitionState = 2;
                mPresentation->SetActiveSlide("out", true);
                if (mPageControls->IsButtonPressed(1))
                    mPreviousPagePressed = true;
                else if (mPageControls->IsButtonPressed(0))
                    mNextPagePressed = true;
                return;
            }
        }
    }
}

void CupFinalRoundsScene::PopulateMatchup(BasicGameInfo* game, TLInstance* group, int index)
{
    CupManager* cupManager = g_pCupManager;
    int userTeam = cupManager->GetUserSelectedCupTeam();
    int teams[2] = { game->mTeamIndex[0], game->mTeamIndex[1] };
    short homeScore = game->mFinalScore[0];
    short awayScore = game->mFinalScore[1];
    TeamStats homeStats = cupManager->GetTeamStats(teams[0]);
    int homeType = homeStats.mUnidentified18;
    TeamStats awayStats = cupManager->GetTeamStats(teams[1]);
    int awayType = awayStats.mUnidentified18;
    if (teams[0] == userTeam)
        homeType = 6;
    else if (teams[1] == userTeam)
        awayType = 6;

    TLComponentInstance* homeComponent =
        FEFinder<TLComponentInstance, TLAT_COMPONENT>::FindOrDefault(group, "team_1");
    TLComponentInstance* awayComponent =
        FEFinder<TLComponentInstance, TLAT_COMPONENT>::FindOrDefault(group, "team_2");
    TLComponentInstance* homeScoreComponent =
        FEFinder<TLComponentInstance, TLAT_COMPONENT>::FindOrDefault(
            homeComponent->GetActiveSlide(), "score");
    TLComponentInstance* awayScoreComponent =
        FEFinder<TLComponentInstance, TLAT_COMPONENT>::FindOrDefault(
            awayComponent->GetActiveSlide(), "score");
    TLComponentInstance* homeTeamComponent =
        FEFinder<TLComponentInstance, TLAT_COMPONENT>::FindOrDefault(
            homeComponent->GetActiveSlide(), "team");
    TLComponentInstance* awayTeamComponent =
        FEFinder<TLComponentInstance, TLAT_COMPONENT>::FindOrDefault(
            awayComponent->GetActiveSlide(), "team");
    TLComponentInstance* homeConfidence =
        FEFinder<TLComponentInstance, TLAT_COMPONENT>::FindOrDefault(
            homeComponent->GetActiveSlide(), "confidence");
    TLComponentInstance* awayConfidence =
        FEFinder<TLComponentInstance, TLAT_COMPONENT>::FindOrDefault(
            awayComponent->GetActiveSlide(), "confidence");

    if (index != cupManager->GetCurrentRoundNumber())
    {
        homeConfidence->m_bVisible = false;
        awayConfidence->m_bVisible = false;
    }

    if (homeScore == 0 && awayScore == 0)
    {
        homeScoreComponent->m_bVisible = false;
        awayScoreComponent->m_bVisible = false;
        homeConfidence->SetActiveSlide(GetCupTeamSlide(homeType), true, false);
        awayConfidence->SetActiveSlide(GetCupTeamSlide(awayType), true, false);
        homeTeamComponent->SetActiveSlide("on", true, false);
        awayTeamComponent->SetActiveSlide("on", true, false);
    }
    else
    {
        TLTextInstance* homeScoreText =
            FEFinder<TLTextInstance, TLAT_TEXT>::FindOrDefault(
                homeScoreComponent->GetActiveSlide(), "score");
        TLTextInstance* awayScoreText =
            FEFinder<TLTextInstance, TLAT_TEXT>::FindOrDefault(
                awayScoreComponent->GetActiveSlide(), "score");
        char homeScoreBuffer[4];
        nlSNPrintf(homeScoreBuffer, sizeof(homeScoreBuffer), "%d", homeScore);
        nlStrToWcs(homeScoreBuffer, mScoreText[index][0], 4);
        homeScoreText->SetString(mScoreText[index][0]);
        char awayScoreBuffer[4];
        nlSNPrintf(awayScoreBuffer, sizeof(awayScoreBuffer), "%d", awayScore);
        nlStrToWcs(awayScoreBuffer, mScoreText[index][1], 4);
        awayScoreText->SetString(mScoreText[index][1]);
        if (homeScore > awayScore)
        {
            homeTeamComponent->SetActiveSlide("on", true, false);
            awayTeamComponent->SetActiveSlide("lost", true, false);
        }
        else
        {
            awayTeamComponent->SetActiveSlide("on", true, false);
            homeTeamComponent->SetActiveSlide("lost", true, false);
        }
    }

    TLImageInstance* homeLogo = FEFinder<TLImageInstance, TLAT_IMAGE>::FindOrDefault(
        homeTeamComponent->GetActiveSlide(), "00_dummy_texture");
    TLImageInstance* awayLogo = FEFinder<TLImageInstance, TLAT_IMAGE>::FindOrDefault(
        awayTeamComponent->GetActiveSlide(), "00_dummy_texture");
    if (teams[0] != -1)
    {
        char imageName[64];
        nlSNPrintf(imageName, sizeof(imageName), "LOGOS_TEAMS_%s_knockout",
            GetCharacterInfo(GetCharacterIndexFromCaptain(teams[0])).mName);
        TLImageInstance* image = FindCupImage(
            mFEScene->m_pFEPackage->GetPresentation(), imageName);
        if (image->m_pTextureResource != 0)
            homeLogo->m_pTextureResource = image->m_pTextureResource;
    }
    if (teams[1] != -1)
    {
        char imageName[64];
        nlSNPrintf(imageName, sizeof(imageName), "LOGOS_TEAMS_%s_knockout",
            GetCharacterInfo(GetCharacterIndexFromCaptain(teams[1])).mName);
        TLImageInstance* image = FindCupImage(
            mFEScene->m_pFEPackage->GetPresentation(), imageName);
        if (image->m_pTextureResource != 0)
            awayLogo->m_pTextureResource = image->m_pTextureResource;
    }
}

void CupFinalRoundsScene::UpdateRoundHighlight()
{
    FEPresentation* presentation = mFEScene->m_pFEPackage->GetPresentation();
    int activeGame = g_pCupManager->GetCurrentRoundNumber();
    for (int i = 0; i < 3; ++i)
    {
        char lightningName[18];
        nlSNPrintf(lightningName, sizeof(lightningName), "lightning_game_%d", i);
        TLComponentInstance* lightning =
            FEFinder<TLComponentInstance, TLAT_COMPONENT>::FindOrDefault(
                presentation->m_currentSlide, "Layer", lightningName);
        char gameName[8];
        nlSNPrintf(gameName, sizeof(gameName), "game_%d", i);
        FEFinder<TLComponentInstance, TLAT_COMPONENT>::Find<>(
            presentation->m_currentSlide, "Layer", gameName);
        if (i == activeGame)
        {
            lightning->m_bVisible = true;
            nlColour colour;
            nlColourSet(colour, 236, 204, 82, 255);
            TLTextInstance* slide1Number =
                FEFinder<TLTextInstance, TLAT_TEXT>::FindOrDefault(
                    mMatchupInstances[i], "Slide1", "GAME_NUMBER");
            slide1Number->SetAssetColour(colour);
            TLTextInstance* slide2Number =
                FEFinder<TLTextInstance, TLAT_TEXT>::FindOrDefault(
                    mMatchupInstances[i], "Slide2", "GAME_NUMBER");
            slide2Number->SetAssetColour(colour);
        }
        else
        {
            lightning->m_bVisible = false;
        }
    }
}

void CupFinalRoundsScene::OnBracketPointerEnter(unsigned int index, void* context)
{
    ++mPointerHoverCounts[index];
    if (context == 0 && !mBracketButton.HasOtherPointerState(1, index))
    {
        mBracketButtonInstance->SetActiveSlide("over", true, false);
        FEAudio::PlayAnimAudioEvent(0xAA73EF34, 0, 0, 1);
        mBracketButton.SetPointerState(1, index);
    }
}

void CupFinalRoundsScene::OnBracketPointerLeave(unsigned int index, void* context)
{
    --mPointerHoverCounts[index];
    if (context == 0 && !mBracketButton.HasOtherPointerState(1, index))
    {
        mBracketButtonInstance->SetActiveSlide("off", true, false);
        mBracketButton.SetPointerState(0, index);
    }
}

void CupFinalRoundsScene::OnMatchupPointerEnter(unsigned int index, void* context)
{
    unsigned int matchup = (unsigned int)context;
    ++mPointerHoverCounts[index];
    mMatchupInstances[matchup]->SetActiveSlide("slide2", true, false);
    FEAudio::PlayAnimAudioEvent(0x50204AFA, 0, 0, 1);
    mMatchupButtons[matchup].SetPointerState(1, index);
}

void CupFinalRoundsScene::OnMatchupPointerLeave(unsigned int index, void* context)
{
    unsigned int matchup = (unsigned int)context;
    --mPointerHoverCounts[index];
    mMatchupInstances[matchup]->SetActiveSlide("slide1", true, false);
    mMatchupButtons[matchup].SetPointerState(0, index);
}

void CupFinalRoundsScene::InitializePointerButtons()
{
    typedef Detail::MemFunImpl<void, void (CupFinalRoundsScene::*)(unsigned int, void*)> PointerMethod;
    typedef BindExp3<void, PointerMethod, CupFinalRoundsScene*, Placeholder<0>, Placeholder<1> > PointerBinding;

    FEPointerListener::Callback bracketOver(
        PointerBinding(MemFun(&CupFinalRoundsScene::OnBracketPointerEnter), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback bracketOff(
        PointerBinding(MemFun(&CupFinalRoundsScene::OnBracketPointerLeave), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback bracketSelect(
        PointerBinding(MemFun(&CupFinalRoundsScene::OnBracketPointerPress), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback matchupOver(
        PointerBinding(MemFun(&CupFinalRoundsScene::OnMatchupPointerEnter), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback matchupOff(
        PointerBinding(MemFun(&CupFinalRoundsScene::OnMatchupPointerLeave), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback matchupSelect(
        PointerBinding(MemFun(&CupFinalRoundsScene::OnMatchupPointerPress), this, Placeholder<0>(), Placeholder<1>()));

    SetPlayButtonBounds(&mBracketButton, mBracketButtonInstance);
    mBracketButton.SetPointerEnterCallback(bracketOver);
    mBracketButton.SetPointerLeaveCallback(bracketOff);
    mBracketButton.SetPointerPressCallback(bracketSelect);

    for (int i = 0; i < 3; ++i)
    {
        mMatchupButtons[i].SetInstanceBounds(
            mMatchupInstances[i], true, 0.0f, 0.0f, 1.0f, 1.0f);
        mMatchupButtons[i].SetPointerEnterCallback(matchupOver);
        mMatchupButtons[i].SetPointerLeaveCallback(matchupOff);
        mMatchupButtons[i].SetPointerPressCallback(matchupSelect);
    }
}

void CupFinalRoundsScene::OnBracketPointerPress(unsigned int, void* context)
{
    mBracketPressed = true;
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

        SHNavigation* navigation = GetNavigationScene();
        if (navigation != 0)
        {
            navigation->HideButtons();
        }

        mPresentation->SetActiveSlide("out", true);
        break;
    }
}

void CupFinalRoundsScene::OnMatchupPointerPress(unsigned int, void* context)
{
    FEAudio::PlayAnimAudioEvent(0x970D6164, 0, 0, 1);
    BasicGameInfo* game = g_pCupManager->GetGameInfo(2, (int)context);
    if (game->mFinalScore[0] != 0 || game->mFinalScore[1] != 0)
    {
        GameResultsScene* results = (GameResultsScene*)GameSceneManager::Instance()->Push(
            (SceneList)0x21, SCREEN_NOTHING, false);
        results->fn_8020A494(game, this, 0);
        results->SetDisplayMode(0xD);
    }
    else
    {
        FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push(
            (SceneList)0xA, SCREEN_NOTHING, false);
        popup->Create((ePopupMenu)0x36, FEPopupMenu::Nothing);
    }
}
