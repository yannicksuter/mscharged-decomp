#include "Game/SH/SHCupKnockout.h"

#include "Game/DB/CharacterInfo.h"
#include "Game/DB/CupInterface.h"
#include "Game/DB/GameProgress.h"
#include "Game/FE/FEAudio.h"
#include "Game/FE/feCupFlow.h"
#include "Game/FE/feDPD.h"
#include "Game/FE/feFinder.inl"
#include "Game/FE/feHelpFuncs_decl.h"
#include "Game/FE/feInput.h"
#include "Game/FE/fePackage.h"
#include "Game/FE/fePageControls.h"
#include "Game/FE/fePopupMenu.h"
#include "Game/FE/fePresentation.h"
#include "Game/FE/feScene.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlImageInstance.h"
#include "Game/FE/tlTextInstance.h"
#include "Game/GameSceneManager.h"
#include "Game/NetTournManager.h"
#include "Game/NetworkSession.h"
#include "Game/SH/SHGameResults.h"
#include "Game/SH/SHNavigation.h"
#include "NL/nlBind.h"
#include "NL/nlFunction.inl"
#include "NL/nlString.h"
#include "NL/nlstring_tmpl.h"

extern "C" void fn_80208950(TLComponentInstance*, unsigned short*, unsigned long);
extern "C" void fn_80207724(int);
extern "C" void fn_802088B4();
extern "C" void fn_80207060(bool);

CupKnockoutScene::CupKnockoutScene()
    : mUnidentified2DC(true)
    , mTournament(0)
    , mUnidentified2E8(0.0f)
    , mStartSeconds(-1)
    , mUnidentified2F0(false)
    , mUnidentified2F1(false)
    , mUnidentified2F2(false)
    , mUnidentified2F3(false)
    , mUnidentified2F4(false)
    , mUnidentified2F5(false)
    , mUnidentified2F6(false)
    , mPointerButtonsInitialized(false)
    , mPreviousPagePressed(false)
    , mNextPagePressed(false)
    , mBackButton()
    , mMatchupButtons()
    , mBracketButton()
    , mBracketButtonInstance(0)
    , mTransitionState(0)
{
    if (g_pNetworkSessionBase->GetSessionMode())
        mNetworkTournament = true;
    else
        mNetworkTournament = false;

    mBracketButton.mContext = 0;
    mPointerHoverCounts[0] = 0;
    mPointerHoverCounts[1] = 0;
    mPointerHoverCounts[2] = 0;
    mPointerHoverCounts[3] = 0;
}

CupKnockoutScene::~CupKnockoutScene()
{
}

void CupKnockoutScene::SceneCreated()
{
    FEPresentation* presentation = mFEScene->m_pFEPackage->GetPresentation();
    char matchupName[8];
    char timerText[8];
    TLSlide* currentSlide;
    TLTextInstance* timer;
    int startSeconds;
    TLComponentInstance* title =
        FEFinder<TLComponentInstance, TLAT_COMPONENT>::FindOrDefault(
            presentation->m_currentSlide, "Layer", "tournament_screens", "TITLE2");
    fn_80208950(title, mTitleText, 64);

    if (mNetworkTournament)
    {
        mTournament = NetTournManager::Instance();
        TLComponentInstance* scroll =
            FEFinder<TLComponentInstance, TLAT_COMPONENT>::FindOrDefault(
                presentation->m_currentSlide, "Layer", "scroll_horizontal");
        scroll->m_bVisible = false;

        NetTournManager* tournament = NetTournManager::Instance();
        if (!tournament->mWaitingToStartGames)
            startSeconds = -1;
        else
        {
            startSeconds = (int)tournament->mTimeToStartGames;
            if (startSeconds < 0)
                startSeconds = 0;
        }
        mStartSeconds = startSeconds;

        currentSlide = mPresentation->m_currentSlide;
        timer = FEFinder<TLTextInstance, TLAT_TEXT>::Find(
            currentSlide, "Layer", "TimerText");
        if (startSeconds == -1)
            timer->m_bVisible = false;
        else
        {
            timer->m_bVisible = true;
            nlSNPrintf(timerText, sizeof(timerText), "%d", startSeconds);
            nlStrToWcs(timerText, mStartTimerText, 8);
            timer->SetString(mStartTimerText);
        }
    }
    else
    {
        mTournament = g_pCupManager;
        currentSlide = mPresentation->m_currentSlide;
        timer = FEFinder<TLTextInstance, TLAT_TEXT>::Find(
            currentSlide, "Layer", "TimerText");
        timer->m_bVisible = false;
    }

    for (int i = 0; i < 7; ++i)
    {
        nlSNPrintf(matchupName, sizeof(matchupName), "game_%d", i);
        mMatchupInstances[i] =
            FEFinder<TLComponentInstance, TLAT_COMPONENT>::FindOrDefault(
                presentation, "IN", "Layer", "tournament_screens", matchupName);
        mOutgoingMatchupInstances[i] =
            FEFinder<TLComponentInstance, TLAT_COMPONENT>::FindOrDefault(
                presentation, "OUT", "Layer", "tournament_screens", matchupName);
        mMatchupButtons[i].mContext = (void*)i;
    }

    PopulateBracket();
    SHNavigation* navigation = GetNavigationScene();
    TLComponentInstance* backButton = 0;
    if (navigation != 0)
    {
        backButton = navigation->GetButton(4);
        mBracketButtonInstance = navigation->GetButton(16);
        mPageControls = navigation->GetPageControls();
        if (!mNetworkTournament)
        {
            mPageControls->SetButtonState(1, true, true);
            mPageControls->SetButtonState(0, true, true);
        }
    }
    mBackButton.SetButtonInstance(backButton);
    if (mNetworkTournament)
    {
        mBackButton.Disable();
        mBracketButton.Disable();
    }
    else
    {
        fn_80207724(2);
    }

    mUnidentified2E8 = 0.0f;
    UpdateRoundHighlight();
    for (int i = 0; i < 4; ++i)
        GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
}

void CupKnockoutScene::Update(float fDeltaT)
{
    BaseSceneHandler::Update(fDeltaT);
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
                CycleCupPage(2, false);
                return;
            }
            if (mNextPagePressed)
            {
                CycleCupPage(2, true);
                return;
            }
            fn_80207060(false);
            return;
        }
        else if (mTransitionState == 3)
        {
            HandleCupBack(2);
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
        for (int j = 0; j < 7; ++j)
            mMatchupButtons[j].HandlePointerEvent(&event);
        mPageControls->Update(event, fDeltaT);
        mBracketButton.HandlePointerEvent(&event);
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

void CupKnockoutScene::PopulateMatchup(
    BasicGameInfo* game, TLInstance* group, int index)
{
    int teams[2] = { game->mTeamIndex[0], game->mTeamIndex[1] };
    short homeScore = game->mFinalScore[0];
    short awayScore = game->mFinalScore[1];
    int userTeam = g_pCupManager->GetUserSelectedCupTeam();
    TeamStats homeStats = g_pCupManager->GetTeamStats(teams[0]);
    int homeType = homeStats.mUnidentified18;
    TeamStats awayStats = g_pCupManager->GetTeamStats(teams[1]);
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
    TLComponentInstance* timer =
        FEFinder<TLComponentInstance, TLAT_COMPONENT>::FindOrDefault(group, "TIMER");
    timer->m_bVisible = false;

    if (teams[0] == -1 || teams[1] == -1)
    {
        homeScoreComponent->m_bVisible = false;
        awayScoreComponent->m_bVisible = false;
        homeConfidence->m_bVisible = false;
        awayConfidence->m_bVisible = false;
        if (teams[0] == -1)
            homeTeamComponent->SetActiveSlide("off", true, false);
        else
            homeTeamComponent->SetActiveSlide("on", true, false);

        if (teams[1] == -1)
            awayTeamComponent->SetActiveSlide("off", true, false);
        else
            awayTeamComponent->SetActiveSlide("on", true, false);
    }
    else if (homeScore == 0 && awayScore == 0)
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
        homeScoreComponent->m_bVisible = true;
        awayScoreComponent->m_bVisible = true;
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
        homeConfidence->m_bVisible = false;
        awayConfidence->m_bVisible = false;
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
    SetTeamLogo(homeLogo, teams[0]);
    SetTeamLogo(awayLogo, teams[1]);
}

void CupKnockoutScene::PopulateNetworkMatchup(NetworkTournamentGame* tournamentGame,
    BasicGameInfo* game, TLInstance* group, int index)
{
    int teams[2] = { game->mTeamIndex[0], game->mTeamIndex[1] };
    short homeScore = game->mFinalScore[0];
    short awayScore = game->mFinalScore[1];

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
    TLComponentInstance* timer =
        FEFinder<TLComponentInstance, TLAT_COMPONENT>::FindOrDefault(group, "TIMER");

    bool finished = tournamentGame->IsFinished();
    int winnerSide = -1;
    tournamentGame->GetWinnerAndLoser(&winnerSide, 0);
    homeConfidence->m_bVisible = false;
    awayConfidence->m_bVisible = false;

    if (teams[0] == -1)
        homeTeamComponent->SetActiveSlide("off", true, false);
    else if (winnerSide == 1)
        homeTeamComponent->SetActiveSlide("lost", true, false);
    else
        homeTeamComponent->SetActiveSlide("on", true, false);

    if (teams[1] == -1)
        awayTeamComponent->SetActiveSlide("off", true, false);
    else if (winnerSide == 0)
        awayTeamComponent->SetActiveSlide("lost", true, false);
    else
        awayTeamComponent->SetActiveSlide("on", true, false);

    TLImageInstance* homeLogo = FEFinder<TLImageInstance, TLAT_IMAGE>::FindOrDefault(
        homeTeamComponent->GetActiveSlide(), "00_dummy_texture");
    TLImageInstance* awayLogo = FEFinder<TLImageInstance, TLAT_IMAGE>::FindOrDefault(
        awayTeamComponent->GetActiveSlide(), "00_dummy_texture");
    SetTeamLogo(homeLogo, teams[0]);
    SetTeamLogo(awayLogo, teams[1]);

    if (finished)
    {
        homeScoreComponent->m_bVisible = true;
        awayScoreComponent->m_bVisible = true;
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
    }
    else
    {
        homeScoreComponent->m_bVisible = false;
        awayScoreComponent->m_bVisible = false;
    }

    switch (tournamentGame->mState)
    {
    case NET_TOURN_GAME_NO_CONTEST:
    case NET_TOURN_GAME_STATE_10:
    case NET_TOURN_GAME_STATE_11:
        timer->SetActiveSlide("DNF", true, false);
        FEFinder<TLTextInstance, TLAT_TEXT>::FindOrDefault(timer, "timer")
            ->SetStringId("DNF");
        timer->m_bVisible = true;
        break;
    case NET_TOURN_GAME_HOME_ADVANCES:
    case NET_TOURN_GAME_AWAY_ADVANCES:
        timer->SetActiveSlide("DNF", true, false);
        FEFinder<TLTextInstance, TLAT_TEXT>::FindOrDefault(timer, "timer")
            ->SetStringId("TOURNAMENT_BY");
        timer->m_bVisible = true;
        break;
    case NET_TOURN_GAME_STATE_3:
    case NET_TOURN_GAME_STATE_4:
    case NET_TOURN_GAME_OVER:
        timer->m_bVisible = false;
        break;
    case NET_TOURN_GAME_READY:
    case NET_TOURN_GAME_IN_PROGRESS:
        switch (tournamentGame->mHomeUpdate)
        {
        case 1:
            timer->SetActiveSlide("loading", true, false);
            timer->m_bVisible = true;
            break;
        case 2:
        case 3:
        {
            int seconds = tournamentGame->mAwayUpdate;
            int minutes = seconds / 60;
            int remainder = seconds % 60;
            char time[32];
            if (remainder < 10)
                nlSNPrintf(time, sizeof(time), "%d:0%d", minutes, remainder);
            else
                nlSNPrintf(time, sizeof(time), "%d:%d", minutes, remainder);
            if (tournamentGame->mHomeUpdate == 3)
                timer->SetActiveSlide("Slide1", true, false);
            else
                timer->SetActiveSlide("time_remaining", true, false);
            nlStrToWcs(time, mGameTimerText[index], 32);
            FEFinder<TLTextInstance, TLAT_TEXT>::FindOrDefault(timer, "timer")
                ->SetString(mGameTimerText[index]);
            timer->m_bVisible = true;
            break;
        }
        case 0:
        default:
            timer->m_bVisible = false;
            break;
        }
        break;
    case NET_TOURN_GAME_EMPTY:
    case NET_TOURN_GAME_NO_PLAYERS:
    default:
        timer->m_bVisible = false;
        break;
    }
}

void CupKnockoutScene::SetTeamLogo(TLImageInstance* image, int team)
{
    if (team == -1)
        return;

    char imageName[24];
    nlSNPrintf(imageName, sizeof(imageName), "logos_TEAM_%s",
        GetCharacterInfo(GetCharacterIndexFromCaptain(team)).mName);
    FEPresentation* presentation = mFEScene->m_pFEPackage->GetPresentation();
    TLSlide* slideList = presentation->m_currentSlide;
    TLSlide* artSlide = 0;
    TLSlide* slide = slideList->m_next;
    while (slide != 0 && slide != slideList)
    {
        if (nlStrCmp<char>(slide->m_szName, "art") == 0)
        {
            artSlide = slide;
            break;
        }
        slide = slide->m_next;
    }
    TLImageInstance* source =
        FEFinder<TLImageInstance, TLAT_IMAGE>::FindOrDefault(
            artSlide, "Layer", imageName);
    if (source->m_pTextureResource != 0)
        image->m_pTextureResource = source->m_pTextureResource;
}

void CupKnockoutScene::PopulateBracket()
{
    FEPresentation* presentation = mFEScene->m_pFEPackage->GetPresentation();
    int numGames = mTournament->GetNumGames(1);
    int gameIndex;
    int firstGame = 7 - numGames;
    UnidentifiedTLGroupInstance* groups[2] = { 0, 0 };
    for (int i = 0; i < 7; ++i)
    {
        char gameName[4];
        nlSNPrintf(gameName, sizeof(gameName), "g_%d", i);
        groups[0] = FEFinder<UnidentifiedTLGroupInstance, TLAT_GROUP>::FindOrDefault(
            mMatchupInstances[i], "off", gameName);
        groups[1] = FEFinder<UnidentifiedTLGroupInstance, TLAT_GROUP>::FindOrDefault(
            mMatchupInstances[i], "over", gameName);

        gameIndex = i - firstGame;
        for (int j = 0; j < 2; ++j)
        {
            if (i < firstGame)
            {
                mOutgoingMatchupInstances[i]->m_bVisible = false;
                groups[j]->m_bVisible = false;
                mMatchupButtons[i].Disable();
            }
            else
            {
                BasicGameInfo* game = mTournament->GetGameInfo(1, gameIndex);
                if (mNetworkTournament)
                {
                    NetworkTournamentGame* tournamentGame =
                        mTournament->GetTournamentGame(1, gameIndex);
                    PopulateNetworkMatchup(tournamentGame, game, groups[j], i);
                }
                else
                {
                    PopulateMatchup(game, groups[j], i);
                }
            }
        }
    }
}

void CupKnockoutScene::UpdateRoundHighlight()
{
    FEPresentation* presentation = mFEScene->m_pFEPackage->GetPresentation();
    int currentRound = mTournament->GetCurrentRoundNumber();
    int numRounds = mTournament->GetNumPlayoffRounds();
    for (int i = 0; i < 7; ++i)
    {
        char lightningName[18];
        nlSNPrintf(lightningName, sizeof(lightningName), "lightning_game_%d", i);
        TLComponentInstance* incoming =
            FEFinder<TLComponentInstance, TLAT_COMPONENT>::FindOrDefault(
                presentation, "IN", "Layer", "tournament_screens", lightningName);
        TLComponentInstance* outgoing =
            FEFinder<TLComponentInstance, TLAT_COMPONENT>::FindOrDefault(
                presentation, "OUT", "Layer", "tournament_screens", lightningName);

        if (mTournament->GetCurrentRoundType() == 1
            && ((currentRound == numRounds - 3 && i < 4)
                || (currentRound == numRounds - 2 && i < 6 && i >= 4)
                || (currentRound == numRounds - 1 && i >= 6)))
        {
            incoming->m_bVisible = true;
            outgoing->m_bVisible = true;
        }
        else
        {
            incoming->m_bVisible = false;
            outgoing->m_bVisible = false;
        }
    }
}

void CupKnockoutScene::InitializePointerButtons()
{
    typedef Detail::MemFunImpl<void,
        void (CupKnockoutScene::*)(unsigned int, void*)> PointerMethod;
    typedef BindExp3<void, PointerMethod, CupKnockoutScene*, Placeholder<0>,
        Placeholder<1> > PointerBinding;

    FEPointerListener::Callback matchupOver(PointerBinding(
        MemFun(&CupKnockoutScene::OnMatchupPointerEnter), this,
        Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback matchupOff(PointerBinding(
        MemFun(&CupKnockoutScene::OnMatchupPointerLeave), this,
        Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback matchupSelect(PointerBinding(
        MemFun(&CupKnockoutScene::OnMatchupPointerPress), this,
        Placeholder<0>(), Placeholder<1>()));
    {
        FEPointerListener::Callback bracketOver(PointerBinding(
            MemFun(&CupKnockoutScene::OnBracketPointerEnter), this,
            Placeholder<0>(), Placeholder<1>()));
        FEPointerListener::Callback bracketOff(PointerBinding(
            MemFun(&CupKnockoutScene::OnBracketPointerLeave), this,
            Placeholder<0>(), Placeholder<1>()));
        FEPointerListener::Callback bracketSelect(PointerBinding(
            MemFun(&CupKnockoutScene::OnBracketPointerPress), this,
            Placeholder<0>(), Placeholder<1>()));

        SetPlayButtonBounds(&mBracketButton, mBracketButtonInstance);
        mBracketButton.SetPointerEnterCallback(bracketOver);
        mBracketButton.SetPointerLeaveCallback(bracketOff);
        mBracketButton.SetPointerPressCallback(bracketSelect);
    }

    for (int i = 0; i < 7; ++i)
    {
        mMatchupButtons[i].SetInstanceBounds(
            mMatchupInstances[i], false, 0.0f, 0.0f, 0.8f, 0.77f);
        mMatchupButtons[i].SetPointerEnterCallback(matchupOver);
        mMatchupButtons[i].SetPointerLeaveCallback(matchupOff);
        mMatchupButtons[i].SetPointerPressCallback(matchupSelect);
    }
}

void CupKnockoutScene::OnBracketPointerEnter(unsigned int index, void* context)
{
    ++mPointerHoverCounts[index];
    if (context == 0 && !mBracketButton.HasOtherPointerState(1, index))
    {
        mBracketButtonInstance->SetActiveSlide("over", true, false);
        FEAudio::PlayAnimAudioEvent(0xAA73EF34, 0, 0, 1);
        mBracketButton.SetPointerState(1, index);
    }
}

void CupKnockoutScene::OnBracketPointerLeave(unsigned int index, void* context)
{
    --mPointerHoverCounts[index];
    if (context == 0 && !mBracketButton.HasOtherPointerState(1, index))
    {
        mBracketButtonInstance->SetActiveSlide("off", true, false);
        mBracketButton.SetPointerState(0, index);
    }
}

void CupKnockoutScene::OnBracketPointerPress(unsigned int, void* context)
{
    mBracketPressed = true;
    for (int i = 0; i < 4; ++i)
        GetPointerInstance(i)->SetActiveSlide("waiting", true, false);

    switch ((int)context)
    {
    case 0:
        FEAudio::PlayAnimAudioEvent(0x6E5C794C, 0, 0, 1);
        FEAudio::PlayAnimAudioEvent(0x2ECB0035, 0, 0, 1);
        mTransitionState = 2;
        SHNavigation* navigation = GetNavigationScene();
        if (navigation != 0)
            navigation->HideButtons();
        mPresentation->SetActiveSlide("out", true);
        break;
    }
}

void CupKnockoutScene::OnMatchupPointerEnter(unsigned int index, void* context)
{
    unsigned int matchup = (unsigned int)context;
    ++mPointerHoverCounts[index];
    mMatchupInstances[matchup]->SetActiveSlide("over", true, false);
    FEAudio::PlayAnimAudioEvent(0x50204AFA, 0, 0, 1);
    mMatchupButtons[matchup].SetPointerState(1, index);
}

void CupKnockoutScene::OnMatchupPointerLeave(unsigned int index, void* context)
{
    unsigned int matchup = (unsigned int)context;
    --mPointerHoverCounts[index];
    mMatchupInstances[matchup]->SetActiveSlide("off", true, false);
    mMatchupButtons[matchup].SetPointerState(0, index);
}

void CupKnockoutScene::OnMatchupPointerPress(unsigned int, void* context)
{
    int numGames = mTournament->GetNumGames(1);
    FEAudio::PlayAnimAudioEvent(0x970D6164, 0, 0, 1);
    int matchup = (int)context - (7 - numGames);
    if (mTournament->HasGameBeenPlayed(1, matchup))
    {
        BasicGameInfo* game = mTournament->GetGameInfo(1, matchup);
        GameResultsScene* results = (GameResultsScene*)GameSceneManager::Instance()->Push(
            (SceneList)0x21, SCREEN_NOTHING, false);
        if (mNetworkTournament)
        {
            results->fn_8020A494(game, this,
                (UnidentifiedGameClock*)mTournament->GetTournamentGame(1, matchup));
            results->SetDisplayMode(0xD);
        }
        else
        {
            results->fn_8020A494(game, this, 0);
            results->SetDisplayMode(0xD);
        }
    }
    else
    {
        FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push(
            (SceneList)0xA, SCREEN_NOTHING, false);
        popup->Create((ePopupMenu)0x36, FEPopupMenu::Nothing);
    }
}
