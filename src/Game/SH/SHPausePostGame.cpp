#include "Game/SH/SHPausePostGame.h"

#include "Game/DB/CharacterInfo.h"
#include "Game/DB/StatsTracker.h"
#include "Game/FE/feInlineHasher.h"
#include "Game/FE/feFinder_impl.h"
#include "Game/FE/feManager.h"
#include "Game/FE/feMusic.h"
#include "Game/FE/fePackage.h"
#include "Game/FE/fePopupMenu.h"
#include "Game/FE/feScene.h"
#include "Game/Game.h"
#include "Game/GameInfo.h"
#include "Game/MatchSeries.h"
#include "Game/NetTournManager.h"
#include "Game/NetworkSession.h"
#include "Game/OverlayManager.h"
#include "Game/SH/SHNavigation.h"
#include "Game/Task/GameRenderTask.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/main.h"
#include "NL/glx/glxSwap.h"
#include "NL/nlBind.h"
#include "NL/nlConfig.h"
#include "NL/nlFormat.h"
#include "NL/nlLocalizationLookup.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"

int lbl_806DD818 = 10;

void PausePostGameScene::OnSelectQuit()
{
    FrontEnd::ReturnToFE();
    SetPointerEnabled(false);
}

PausePostGameScene::PausePostGameScene(int mode)
    : mMode(mode)
    , mControllingInput(FE_ALL_PADS)
    , mTimer(1.0f, Function<FETimer*>(Bind<void>(MemFun(&PausePostGameScene::OnCountdownTick), this, Placeholder<0>())))
    , mTimerTicked(false)
    , mSummaryDisplayed(false)
    , mCountdownSeconds(lbl_806DD818)
{
    mIsMultiplayer = g_pNetworkSessionBase->GetNumMachines() > 1;
    mTimer.SetEnabled(mIsMultiplayer);
}

PausePostGameScene::~PausePostGameScene()
{
    if (mMode == 1)
        g_bRenderWorld = true;
}

void PausePostGameScene::SceneCreated()
{
    FEPresentation* presentation = mFEScene->m_pFEPackage->GetPresentation();
    SetPointerEnabled(true);
    if (mMode == 1 && g_e3_Build)
        g_bRenderWorld = false;
    if (mMode == 0 && GameInfoManager::Instance()->IsInFriendlyMode())
        BuildStoryArticle();
    if (mMode == 0)
        FEMusic::StartStreamIfDifferent(13);
    SHStrikerTimesBase::SceneCreated();
}

void PausePostGameScene::OnCountdownTick(FETimer* timer)
{
    mTimerTicked = true;
    --mCountdownSeconds;
}

bool IsCurrentSeriesComplete()
{
    int wins0 = StatsTracker::Instance()->mNumGamesWon[0];
    int wins1 = StatsTracker::Instance()->mNumGamesWon[1];
    int needed = GameInfoManager::Instance()->GetCurrentSettings()->NumGames / 2 + 1;
    if ((wins0 > wins1 && wins0 == needed) || (wins1 > wins0 && wins1 == needed))
        return true;
    return false;
}

void ContinuePostGame(bool online)
{
    const GameplaySettings* settings = GameInfoManager::Instance()->GetCurrentSettings();
    if (IsCurrentSeriesComplete())
    {
        if (!online)
        {
            StatsTracker* tracker = StatsTracker::Instance();
            tracker->mNumGamesWon[0] = 0;
            tracker->mNumGamesWon[1] = 0;
            SetPointerEnabled(true);
            FEPopupMenu* popup = static_cast<FEPopupMenu*>(g_pOverlayManager->Push((SceneList)10, SCREEN_NOTHING, false));
            popup->Create((ePopupMenu)53, Function<FnVoidVoid>(PausePostGameScene::OnSelectRematch),
                Function<FnVoidVoid>(PausePostGameScene::OnSelectChangeTeams), Function<FnVoidVoid>(PausePostGameScene::OnSelectQuit));
        }
        else
        {
            FEMusic::StopStream();
            FrontEnd::ReturnToFE();
        }
    }
    else
    {
        if (GetConfigBool(Config::Global(), "save_stats", false))
        {
            StatsTracker* tracker = StatsTracker::Instance();
            tracker->WriteStats(g_pGame->GetGameTime(), -1.0f, 0);
        }
        StatsTracker::Instance()->ResetCurrentStats();
        FrontEnd::ExitWinnerScreen();
        glxSwapSetBlack(true);
        if (online)
            g_pNetworkSession->RematchGame();
        else
            RestartSinglePlayerGame();
        FEMusic::StopStream();
        g_pGame->BeginGame(true, false);
        FrontEnd::m_bGameOver = false;
    }
}

void PausePostGameScene::Update(float dt)
{
    SHStrikerTimesBase::Update(dt);
    if (mMode == 0)
    {
        TLInstance* instance = FEFinder<TLInstance, 2>::Find<TLSlide>(mPresentation->m_currentSlide, "Layer", "blackbox2");
        nlColour colour = instance->GetAssetColour();
        if (mState == 2)
            nlColourSet(colour, colour[0], colour[1], colour[2], 255);
        else
            nlColourSet(colour, colour[0], colour[1], colour[2], 178);
        instance->SetAssetColour(colour);
    }
    if (mIsMultiplayer)
        mTimer.Update(dt);
    if (!mSummaryDisplayed || mTimerTicked)
    {
        UpdateSummaryDisplay();
        mSummaryDisplayed = true;
        if (mTimerTicked)
            mTimerTicked = false;
    }
    if (mMode == 0 && mIsMultiplayer && mCountdownSeconds <= 0)
        OnDoneTransitionComplete();
}

void PausePostGameScene::UpdateSummaryDisplay()
{
    FEPresentation* presentation = mFEScene->m_pFEPackage->GetPresentation();
    unsigned long titleHash = nlStringLowerHash("title");
    unsigned long summaryHash = nlStringLowerHash("game summary");
    unsigned long layerHash = nlStringLowerHash("Layer");
    TLTextInstance* text = FEFinder<TLTextInstance, 3>::Find(presentation, nlStringLowerHash("game summary"), layerHash, summaryHash, titleHash, 0, 0);
    if (mMode == 0)
        text->SetStringId("GAME_RESULTS_TITLE");
    else if (mMode == 1)
        text->SetStringId("STATISTICS");
    if (mIsMultiplayer)
    {
        char buffer[8];
        nlSNPrintf(buffer, 8, "%d", mCountdownSeconds);
        nlStrToWcs(buffer, mCountdownText, 8);
        TLSlide* first = presentation->m_currentSlide;
        TLSlide* slide = first;
        do
        {
            TLTextInstance* timer = static_cast<TLTextInstance*>(GetNavigationScene()->mTimer);
            GetNavigationScene()->mTimer->m_bVisible = true;
            timer->SetString(mCountdownText);
            FEFinder<TLInstance, 2>::Find<TLSlide>(slide, "Layer", "NetworkWait")->m_bVisible = true;
            slide = slide->m_next;
        } while (slide != first);
    }
    mSummary.DisplayMatchSummary(TeamStats(*StatsTracker::Instance()->mCumulativeTeamStats[0]),
        TeamStats(*StatsTracker::Instance()->mCumulativeTeamStats[1]), presentation);
}

void PausePostGameScene::OnDoneTransitionComplete()
{
    if (mIsMultiplayer && mCountdownSeconds > 0)
        return;
    SHStrikerTimesBase::OnDoneTransitionComplete();
    if (mMode == 1)
    {
        BaseSceneHandler* scene = g_pOverlayManager->Push((SceneList)80, SCREEN_BACK, true);
        // The retail caller writes this byte in the returned pause scene.
        reinterpret_cast<u8*>(scene)[0x241] = true;
    }
    else if (mMode == 0)
    {
        SetPointerEnabled(false);
        SHNavigation* navigation = GetNavigationScene();
        navigation->mTimer->m_bVisible = false;
        if (NetTournManager::Instance()->mState != 0)
        {
            g_pOverlayManager->Push((SceneList)93, SCREEN_NOTHING, true);
        }
        else if (GameInfoManager::Instance()->mCurrentMode == 0)
        {
            if (g_e3_Build)
            {
                mMode = 2;
                FEPopupMenu* popup = static_cast<FEPopupMenu*>(g_pOverlayManager->Push((SceneList)10, SCREEN_NOTHING, false));
                popup->Create((ePopupMenu)142, Function<FnVoidVoid>(OnSelectQuit));
            }
            else if (mIsMultiplayer && IsOnlineRankedMatch())
            {
                SetPointerEnabled(true);
                g_pOverlayManager->Push((SceneList)93, SCREEN_NOTHING, true);
            }
            else
            {
                g_pOverlayManager->Pop();
                ContinuePostGame(mIsMultiplayer);
            }
        }
        else if (!GameInfoManager::Instance()->IsInMode4())
        {
            FEMusic::StopStream();
            FrontEnd::ReturnToFE();
        }
    }
}

void PausePostGameScene::BuildStoryArticle()
{
    BasicGameInfo* game = GameInfoManager::Instance()->GetCurrentGameInfo();
    int homeTeam = game->mTeamIndex[0];
    int homeScore = game->mFinalScore[0];
    int awayScore = game->mFinalScore[1];
    int awayTeam = game->mTeamIndex[1];
    int winScore = homeScore > awayScore ? homeScore : awayScore;
    int loseScore = homeScore < awayScore ? homeScore : awayScore;
    int winner = awayTeam;
    if (homeScore > awayScore)
        winner = homeTeam;
    int loser = awayTeam;
    if (homeScore < awayScore)
        loser = homeTeam;
    const CharacterInfo& winningCharacter = GetCharacterInfo(GetCharacterIndexFromCaptain(winner));
    const CharacterInfo& losingCharacter = GetCharacterInfo(GetCharacterIndexFromCaptain(loser));
    typedef BasicString<unsigned short, Detail::TempStringAllocator> String;
    char maxWinsText[4], minWinsText[4], winScoreText[4], loseScoreText[4], numGamesText[4];
    String headline;
    String body;
    mUseCustomText = true;
    int variant = nlRandom(4, &nlDefaultSeed);
    int wins0 = StatsTracker::Instance()->mNumGamesWon[0];
    int wins1 = StatsTracker::Instance()->mNumGamesWon[1];
    int numGames = wins0 + wins1;
    int maxWins = wins0 > wins1 ? wins0 : wins1;
    int minWins = wins0 > wins1 ? wins1 : wins0;
    unsigned short wMaxWins[4], wMinWins[4], wWinScore[4], wLoseScore[4], wNumGames[4];
    nlSNPrintf(maxWinsText, 4, "%d", maxWins);
    nlStrToWcs(maxWinsText, wMaxWins, 4);
    nlSNPrintf(minWinsText, 4, "%d", minWins);
    nlStrToWcs(minWinsText, wMinWins, 4);
    nlSNPrintf(winScoreText, 4, "%d", winScore);
    nlStrToWcs(winScoreText, wWinScore, 4);
    nlSNPrintf(loseScoreText, 4, "%d", loseScore);
    nlStrToWcs(loseScoreText, wLoseScore, 4);
    nlSNPrintf(numGamesText, 4, "%d", numGames);
    nlStrToWcs(numGamesText, wNumGames, 4);
    if (winScore - loseScore <= 2)
        nlSNPrintf(mStoryStringID, 64, "ST_DOMINATION_CLOSE_GAME_%d", variant);
    else if (winScore - loseScore <= 4)
        nlSNPrintf(mStoryStringID, 64, "ST_DOMINATION_ONE_SIDED_GAME_%d", variant);
    else
        nlSNPrintf(mStoryStringID, 64, "ST_DOMINATION_BLOWOUT_GAME_%d", variant);
    body = String(g_pLocalization->GetString(mStoryStringID));
    mStoryText = Format(body, g_pLocalization->GetString(winningCharacter.mDisplayNameKey),
        g_pLocalization->GetString(losingCharacter.mDisplayNameKey), wWinScore, wLoseScore, wNumGames);
    const CharacterInfo& seriesWinner = GetCharacterInfo(GetCharacterIndexFromCaptain(wins0 > wins1 ? homeTeam : awayTeam));
    const CharacterInfo& seriesLoser = GetCharacterInfo(GetCharacterIndexFromCaptain(wins0 > wins1 ? awayTeam : homeTeam));
    int needed = GameInfoManager::Instance()->GetCurrentSettings()->NumGames / 2 + 1;
    if ((wins0 > wins1 && wins0 == needed) || (wins1 > wins0 && wins1 == needed))
    {
        nlSNPrintf(mHeadlineStringID, 64, "STH_DOMINATION_WINS_SERIES_%d", 0);
        headline = String(g_pLocalization->GetString(mHeadlineStringID));
        mHeadlineText = Format(headline, g_pLocalization->GetString(seriesWinner.mDisplayNameKey), wMaxWins, wMinWins);
    }
    else if (wins0 == wins1)
    {
        nlSNPrintf(mHeadlineStringID, 64, "STH_DOMINATION_TIED_SERIES_%d", 0);
        headline = String(g_pLocalization->GetString(mHeadlineStringID));
        mHeadlineText = Format(headline, wMaxWins, wMinWins);
    }
    else
    {
        nlSNPrintf(mHeadlineStringID, 64, "STH_DOMINATION_LEADS_SERIES_%d", 0);
        headline = String(g_pLocalization->GetString(mHeadlineStringID));
        mHeadlineText = Format(headline, g_pLocalization->GetString(seriesWinner.mDisplayNameKey), wMaxWins, wMinWins);
    }
    SetArticleImageName(winner, 0, -1);
}

void PausePostGameScene::OnSelectRematch()
{
    const char* key = "save_stats";
    Config& cfg = Config::Global();
    if (cfg.Get<bool>(key, false))
    {
        StatsTracker* tracker = StatsTracker::s_pInstance;
        tracker->WriteStats(g_pGame->GetGameTime(), -1.0f, 0);
    }
    StatsTracker::Instance()->ResetCurrentStats();
    FrontEnd::ExitWinnerScreen();
    glxSwapSetBlack(true);
    RestartSinglePlayerGame();
    FEMusic::StopStream();
    g_pGame->BeginGame(true, false);
    FrontEnd::m_bGameOver = false;
    SetPointerEnabled(false);
}

void PausePostGameScene::OnSelectChangeTeams()
{
    GameInfoManager::s_pInstance->unknown_0x71C8 = 2;
    FrontEnd::ReturnToFE();
    SetPointerEnabled(false);
}
