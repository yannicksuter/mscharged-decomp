#include "Game/OverlayHandlerStrikerTimes.h"

#include "Game/BaseGameSceneManager.h"
#include "Game/DB/BasicGameInfo.h"
#include "Game/DB/CharacterInfo.h"
#include "Game/DB/StatsTracker.h"
#include "Game/FE/feAsyncImage.h"
#include "Game/FE/feFinder.h"
#include "Game/FE/feFinder.inl"
#include "Game/FE/feHelpFuncs.h"
#include "Game/FE/feInput.h"
#include "Game/FE/fePackage.h"
#include "Game/FE/fePresentation.h"
#include "Game/FE/feScene.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlImageInstance.h"
#include "Game/FE/tlSlide.h"
#include "Game/FE/tlTextInstance.h"
#include "Game/GameInfo.h"
#include "Game/NetworkSession.h"
#include "Game/OverlayManager.h"
#include "NL/gl/glStruct.h"
#include "NL/nlBind.h"
#include "NL/nlFormat.h"
#include "NL/nlLocalization.h"
#include "NL/nlMath.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"

int gStrikerTimesCountdownSeconds = 10;

StrikerTimesOverlay::StrikerTimesOverlay()
    : BaseOverlayHandler(1, POSITION_ALL)
    , mHeadlineScroller(0)
    , mStoryScroller(1)
    , mCountdownTimer(1.0f,
          Function<FETimer*>(Bind<void>(MemFun(&StrikerTimesOverlay::OnCountdownTick), this, Placeholder<0>())))
    , mCountdownTicked(false)
    , mCountdownSeconds(gStrikerTimesCountdownSeconds)
    , mArticleImage(0)
    , mButtonsHidden(false)
    , mInputDelay(0.0f)
{
    mStoryScroller.SetEndBehavior(2);
    mStoryScroller.SetScrollMode(1);
    mIsNetworkGame = g_pNetworkSessionBase->GetNumMachines() > 1;
    mCountdownTimer.SetEnabled(mIsNetworkGame);
    mArticleImage = new (0x20, true) AsyncImage("art/fe/StrikerTimesUI.res", 0);
}

StrikerTimesOverlay::~StrikerTimesOverlay()
{
    if (mHeadlineScroller != 0)
    {
        delete mHeadlineScroller;
    }
    if (mArticleImage != 0)
    {
        delete mArticleImage;
    }
}

void StrikerTimesOverlay::Update(float dt)
{
    if (!mButtonsHidden)
    {
        FEPresentation* presentation = mFEScene->m_pFEPackage->GetPresentation();
        TLInstance* buttons = FEFinder<TLInstance, 1>::Find(presentation->m_currentSlide, "Layer", "buttons");
        if (mIsNetworkGame)
        {
            buttons->m_bVisible = false;
        }
        mButtonsHidden = true;
    }

    BaseSceneHandler::Update(dt);
    mHeadlineScroller->Update(dt);
    mArticleImage->Update(true);
    mStoryScroller.SetScrollDirection(0);
    mStoryScroller.Update(dt);

    if (mIsNetworkGame)
    {
        mCountdownTimer.Update(dt);
        if (mCountdownTicked)
        {
            TLTextInstance* timerText = FEFinder<TLTextInstance, 3>::Find(mPresentation->m_currentSlide, "Layer", "TimerText");
            char buffer[8];
            nlSNPrintf(buffer, 8, "%d", mCountdownSeconds);
            nlStrToWcs(buffer, mTimerText, 8);
            timerText->SetString(mTimerText);
        }
    }
    else
    {
        TLSlide* slide = mPresentation->m_currentSlide;
        f32 end = slide->GetStartTime() + slide->GetDuration();
        if (mInputDelay < end + 2.0f)
        {
            mInputDelay += dt;
            return;
        }
    }

    if ((mIsNetworkGame && mCountdownSeconds <= 0) || (!mIsNetworkGame && g_pFEInput->JustPressed(FE_ALL_PADS, 0x1E, true, 0)))
    {
        g_pOverlayManager->Push((SceneList)91, SCREEN_NOTHING, true);
    }
    else if (g_pFEInput->IsPressed(FE_ALL_PADS, 0xE, true, 0) || g_pFEInput->IsPressed(FE_ALL_PADS, 0xD, true, 0))
    {
    }
}

void StrikerTimesOverlay::OnCountdownTick(FETimer* timer)
{
    mCountdownTicked = true;
    --mCountdownSeconds;
}

void StrikerTimesOverlay::SceneCreated()
{
    FEPresentation* presentation;
    TLTextInstance* bodyText;
    TLTextInstance* headlineText;
    char storyKey[64];
    char headlineKey[64];
    unsigned short leaderWinsText[4];
    unsigned short trailerWinsText[4];
    unsigned short winnerScoreText[4];
    unsigned short loserScoreText[4];
    unsigned short gamesPlayedText[4];

    EnableAutoPressed();

    BasicGameInfo* gameInfo = GameInfoManager::Instance()->GetCurrentGameInfo();
    int homeCaptain = gameInfo->GetTeam(0);
    int awayCaptain = gameInfo->GetTeam(1);
    int homeScore = gameInfo->GetFinalScore(0);
    int awayScore = gameInfo->GetFinalScore(1);
    int winnerScore = homeScore > awayScore ? homeScore : awayScore;
    int loserScore = homeScore < awayScore ? homeScore : awayScore;
    int storyVariant = -1;
    int headlineVariant = -1;
    int imageVariant = -1;
    char leaderWinsString[4];
    char trailerWinsString[4];
    char winnerScoreString[4];
    char loserScoreString[4];
    char gamesPlayedString[4];
    int winnerCaptain = homeScore > awayScore ? homeCaptain : awayCaptain;
    int loserCaptain = homeScore < awayScore ? homeCaptain : awayCaptain;
    int homeWins;
    int awayWins;
    const CharacterInfo& winnerInfo = GetCharacterInfo(GetCharacterIndexFromCaptain(winnerCaptain));
    const CharacterInfo& loserInfo = GetCharacterInfo(GetCharacterIndexFromCaptain(loserCaptain));

    BasicString<unsigned short, Detail::TempStringAllocator> headline;
    BasicString<unsigned short, Detail::TempStringAllocator> story;
    BasicString<unsigned short, Detail::TempStringAllocator> headlineFormat;
    BasicString<unsigned short, Detail::TempStringAllocator> storyFormat;

    static_cast<OverlayManager*>(GetOverlayManager())->GetStrikerTimesVariants(&storyVariant, &headlineVariant, &imageVariant);
    if (storyVariant == -1)
    {
        storyVariant = nlRandom(4, &nlDefaultSeed);
    }
    if (headlineVariant == -1)
    {
        headlineVariant = 0;
    }
    if (imageVariant == -1)
    {
        imageVariant = nlRandom(10, &nlDefaultSeed);
    }
    static_cast<OverlayManager*>(GetOverlayManager())->SetStrikerTimesVariants(storyVariant, headlineVariant, imageVariant);

    homeWins = nlSingleton<StatsTracker>::Instance()->GetNumGamesWon(0);
    awayWins = nlSingleton<StatsTracker>::Instance()->GetNumGamesWon(1);
    int gamesPlayed = homeWins + awayWins;
    int leaderWins = homeWins > awayWins ? homeWins : awayWins;
    int trailerWins = homeWins > awayWins ? awayWins : homeWins;

    nlSNPrintf(leaderWinsString, 4, "%d", leaderWins);
    nlStrToWcs(leaderWinsString, leaderWinsText, 4);
    nlSNPrintf(trailerWinsString, 4, "%d", trailerWins);
    nlStrToWcs(trailerWinsString, trailerWinsText, 4);
    nlSNPrintf(winnerScoreString, 4, "%d", winnerScore);
    nlStrToWcs(winnerScoreString, winnerScoreText, 4);
    nlSNPrintf(loserScoreString, 4, "%d", loserScore);
    nlStrToWcs(loserScoreString, loserScoreText, 4);
    nlSNPrintf(gamesPlayedString, 4, "%d", gamesPlayed);
    nlStrToWcs(gamesPlayedString, gamesPlayedText, 4);

    int margin = winnerScore - loserScore;
    if (margin <= 2)
    {
        nlSNPrintf(storyKey, 64, "ST_DOMINATION_CLOSE_GAME_%d", storyVariant);
    }
    else if (margin <= 4)
    {
        nlSNPrintf(storyKey, 64, "ST_DOMINATION_ONE_SIDED_GAME_%d", storyVariant);
    }
    else
    {
        nlSNPrintf(storyKey, 64, "ST_DOMINATION_BLOWOUT_GAME_%d", storyVariant);
    }
    storyFormat = BasicString<unsigned short, Detail::TempStringAllocator>(g_pLocalization->GetString(storyKey));
    story = Format(storyFormat, g_pLocalization->GetString(winnerInfo.GetDisplayNameKey()),
        g_pLocalization->GetString(loserInfo.GetDisplayNameKey()), winnerScoreText, loserScoreText, gamesPlayedText);

    const CharacterInfo& leaderInfo = GetCharacterInfo(GetCharacterIndexFromCaptain(homeWins > awayWins ? homeCaptain : awayCaptain));
    GetCharacterInfo(GetCharacterIndexFromCaptain(homeWins > awayWins ? awayCaptain : homeCaptain));

    int winsNeeded = GameInfoManager::Instance()->GetCurrentSettings()->NumGames / 2 + 1;
    if ((homeWins > awayWins && homeWins == winsNeeded) || (awayWins > homeWins && awayWins == winsNeeded))
    {
        nlSNPrintf(headlineKey, 64, "STH_DOMINATION_WINS_SERIES_%d", headlineVariant);
        headlineFormat = BasicString<unsigned short, Detail::TempStringAllocator>(g_pLocalization->GetString(headlineKey));
        headline = Format(headlineFormat, g_pLocalization->GetString(leaderInfo.GetDisplayNameKey()), leaderWinsText, trailerWinsText);
    }
    else if (homeWins == awayWins)
    {
        nlSNPrintf(headlineKey, 64, "STH_DOMINATION_TIED_SERIES_%d", headlineVariant);
        headlineFormat = BasicString<unsigned short, Detail::TempStringAllocator>(g_pLocalization->GetString(headlineKey));
        headline = Format(headlineFormat, leaderWinsText, trailerWinsText);
    }
    else
    {
        nlSNPrintf(headlineKey, 64, "STH_DOMINATION_LEADS_SERIES_%d", headlineVariant);
        headlineFormat = BasicString<unsigned short, Detail::TempStringAllocator>(g_pLocalization->GetString(headlineKey));
        headline = Format(headlineFormat, g_pLocalization->GetString(leaderInfo.GetDisplayNameKey()), leaderWinsText, trailerWinsText);
    }

    presentation = mFEScene->GetPackage()->GetPresentation();

    bodyText = FEFinder<TLTextInstance, 3>::FindOrDefault(presentation->GetActiveSlide(), "Layer", "BODY");
    mStoryScroller.ApplyNewTextInstancePointer(bodyText, -1, -1, 0);
    mStoryScroller.SetDisplayMessage(story);

    TLTextInstance* clipText = (TLTextInstance*)FEFinder<TLInstance, 3>::FindOrDefault(presentation->GetActiveSlide(), "Layer", "Description_clip");
    mStoryScroller.SetClippingTextInstance(clipText);
    clipText->SetVisible(false);

    headlineText = FEFinder<TLTextInstance, 3>::FindOrDefault(presentation->GetActiveSlide(), "Layer", "HEADLINE");
    glGetScreenInfo();
    mHeadlineScroller = new (8, false) FEScrollText(0);
    mHeadlineScroller->ApplyNewTextInstancePointer(headlineText, -17, 240, 0);
    mHeadlineScroller->SetDisplayMessage(headline);

    if (!mIsNetworkGame)
    {
        FEFinder<TLTextInstance, 3>::FindOrDefault(presentation->GetActiveSlide(), "Layer", "TimerText")->SetVisible(false);
        FEFinder<TLImageInstance, 2>::FindOrDefault(presentation->GetActiveSlide(), "Layer", "metre_yellow_centre")->SetVisible(false);
    }

    ((TLComponentInstance*)FEFinder<TLInstance, 4>::FindOrDefault(presentation->GetActiveSlide(), "Layer", "OK"))->SetActiveSlide("ON", true, false);
    mHeadlineScroller->SetScrollSpeed(150.0f);
    SetArticleImage(winnerCaptain, imageVariant);
}

void StrikerTimesOverlay::SetArticleImage(int captain, int variant)
{
    if (captain != 0)
    {
        captain = 0;
    }
    const CharacterInfo& info = GetCharacterInfo(GetCharacterIndexFromCaptain(captain));
    TLImageInstance* image = FEFinder<TLImageInstance, 2>::FindOrDefault(mPresentation->m_currentSlide, "Layer", "00_dummy_texture");
    mArticleImage->mImageInstance = image;
    char path[128];
    nlSNPrintf(path, 128, "fe/striker_times_textures/%spositivevice%d", info.mName, variant);
    mArticleImage->QueueLoad(path, false);
}
