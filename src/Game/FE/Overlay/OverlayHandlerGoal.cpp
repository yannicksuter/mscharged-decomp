#include "Game/OverlayHandlerGoal.h"
#include "NL/nlBasicString.inl"

#include "Game/DB/GameProgress.h"
#include "Game/DB/CharacterInfo.h"
#include "Game/EventDataTypes.h"
#include "Game/EventRegistry.h"
#include "Game/FE/feFinder.inl"
#include "Game/FE/feHelpFuncs_decl.h"
#include "Game/FE/fePresentation.inl"
#include "Game/FE/fePackage.h"
#include "Game/FE/feScene.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlSlide.h"
#include "Game/FE/tlTextInstance.h"
#include "Game/Game.h"
#include "Game/GameInfo.h"
#include "Game/Player.h"
#include "Game/NisPlayer.h"
#include "Game/Team.h"
#include "Game/Render/RLViewLayers.h"
#include "NL/nlFormat.h"
#include "NL/nlFunction.inl"
#include "NL/nlLocalizationLookup.h"
#include "NL/nlPrint.h"
#include <cmath>
#include <string.h>

#include "Game/UnidentifiedStaticStorage.h"

GoalOverlay::~GoalOverlay()
{
}

void GoalOverlay::Update(float fDeltaT)
{
    BaseSceneHandler::Update(fDeltaT);

    if (!mIsInOvertime && g_pGame->m_eGameState == 6)
    {
        mIsInOvertime = true;
    }
}

void GoalOverlay::fn_801F178C(GoalScoredData* data)
{
    int playerIndex;
    if (data->uGoalType == 5)
    {
        playerIndex = data->pLastTouch[data->uTeamIndex]->mUnidentified1E4.m_ID;
    }
    else
    {
        playerIndex = data->pScorer->mUnidentified1E4.m_ID;
    }
    UpdateGoalInfo(data->uTeamIndex, playerIndex, false, data->uNumGoalsScored);
}

void GoalOverlay::fn_801F17D0(int homeAway)
{
    if (g_pGame->mbCaptainShotToScoreOn)
    {
        UpdateGoalInfo(homeAway, 0, true, g_pGame->mUnidentified030);
    }
}

void GoalOverlay::fn_801F17F4(MegaStrikeEndData*)
{
}

void GoalOverlay::Restart()
{
    if (mIsCreated)
    {
        const char* slideName = IsWidescreen() ? "widescreen" : "normal";
        mPresentation->SetActiveSlide(slideName, true);
        mPresentation->m_fadeDuration = mPresentation->m_currentSlide->m_start;
    }
}

void GoalOverlay::Reset()
{
    mCaptainGoals[0] = 0;
    mCaptainGoals[1] = 0;
    mSidekickGoals[0] = 0;
    mSidekickGoals[1] = 0;
    mIsInOvertime = false;

    if (GameInfoManager::Instance()->IsInMode4())
    {
        mCaptainGoals[0] = g_pStrikerChallenge->mScore[0];
        mCaptainGoals[1] = g_pStrikerChallenge->mScore[1];
    }
}

GoalOverlay::GoalOverlay()
    : BaseOverlayHandler(0x18, POSITION_BOTTOM)
{
    mIsCreated = false;
    mIsInOvertime = false;
    UnidentifiedFindEvent<UnidentifiedEventNoData>("GameOver", -1)->Add(Function<FnVoidVoid>(Bind<void>(MemFun(&GoalOverlay::Reset), this)), 0, -1);
    UnidentifiedFindEvent<GoalScoredData>("GoalScored", -1)->Add(Function<GoalScoredData*>(Bind<void>(MemFun(&GoalOverlay::fn_801F178C), this, placeholder0)), 0, -1);
    UnidentifiedFindEvent<MegaStrikeEndData>("MegastrikeEnd", -1)->Add(Function<MegaStrikeEndData*>(Bind<void>(MemFun(&GoalOverlay::fn_801F17F4), this, placeholder0)), 0, -1);

    if (GameInfoManager::Instance()->IsInFriendlyMode() || GameInfoManager::Instance()->IsInMode1())
    {
        mHasSniperCup = true;
    }
    else
    {
        mHasSniperCup = GameInfoManager::Instance()->HasTrophy(TROPHY_SNIPER_CUP);
    }
    Reset();
}

void GoalOverlay::SceneCreated()
{
    mIsCreated = true;
    const char* slideName = IsWidescreen() ? "widescreen" : "normal";
    mPresentation->SetActiveSlide(slideName, true);
}

void GoalOverlay::UpdateGoalInfo(int homeAway, int playerIndex, bool isCaptainS2S, int numGoals)
{
    TLSlide* slide = mFEScene->GetPackage()->GetPresentation()->GetActiveSlide();
    TLTextInstance* pText = FEFinder<TLTextInstance, 3>::Find<TLSlide>(slide, "Layer", "Name");
    GameInfoManager* const gameInfo = GameInfoManager::Instance();
    eTeamID team = (eTeamID)gameInfo->GetTeam((short)homeAway);
    GameInfoManager::Instance()->GetTeam(0);
    GameInfoManager::Instance()->GetTeam(1);
    int otherSide = !homeAway;

    float time = g_pGame->GetGameTime();
    if (!StatsTracker::Instance()->IsOvertime())
    {
        if (time > g_pGame->GetGameDuration())
        {
            time = g_pGame->GetGameDuration();
        }
    }
    float remainingTime = g_pGame->GetGameDuration() - time;
    unsigned long minutes = (unsigned long)(time / 60.0f);
    float fSeconds = time - (float)(minutes * 60);
    unsigned long seconds;
    if (remainingTime >= 30.0)
    {
        seconds = (int)std::ceil(fSeconds);
    }
    else
    {
        seconds = (int)std::floor(fSeconds);
    }
    if (seconds == 60)
    {
        seconds = 0;
        minutes++;
    }

    BasicString<unsigned short, Detail::TempStringAllocator> formatted;
    unsigned short minutesWideString[16];
    unsigned short secondsWideString[16];
    nlSNPrintf(minutesWideString, 32, (const unsigned short*)L"%d", minutes);
    int oldScore[2] = { mCaptainGoals[0] + mSidekickGoals[0], mCaptainGoals[1] + mSidekickGoals[1] };
    bool isMatchEnd = mIsInOvertime;
    if (playerIndex > 0)
    {
        mSidekickGoals[homeAway] += numGoals;
    }
    else
    {
        mCaptainGoals[homeAway] += numGoals;
    }
    int score[2] = { mCaptainGoals[0] + mSidekickGoals[0], mCaptainGoals[1] + mSidekickGoals[1] };
    if (gameInfo->GetCurrentSettings()->GameLimitType == 1
        && score[homeAway] >= gameInfo->GetCurrentSettings()->GoalLimit)
    {
        isMatchEnd = true;
    }
    if (isCaptainS2S == true)
    {
        if (numGoals == 0)
            SetWinnerTitle(otherSide, false, 0);
        else
            SetWinnerTitle(homeAway, isMatchEnd, numGoals);
    }
    else
    {
        SetWinnerTitle(homeAway, isMatchEnd, 0);
    }

    if (seconds < 10)
    {
        secondsWideString[0] = '0';
        nlSNPrintf(secondsWideString + 1, 32, (const unsigned short*)L"%d", seconds);
    }
    else
    {
        nlSNPrintf(secondsWideString, 32, (const unsigned short*)L"%d", seconds);
    }
    formatted = Format(BasicString<unsigned short, Detail::TempStringAllocator>(g_pLocalization->GetString("CLOCK")),
        minutesWideString, secondsWideString);
    memcpy(mClockBuffer, formatted.c_str(), sizeof(mClockBuffer));
    pText = FEFinder<TLTextInstance, 3>::Find<TLSlide>(slide, "Layer", "Time");
    pText->SetString(mClockBuffer);
    if (GameInfoManager::Instance()->GetCurrentSettings()->GameLimitType == 0
        && !GameInfoManager::Instance()->IsInMode4())
        pText->SetVisible(true);
    else
        pText->SetVisible(false);

    if (isCaptainS2S == true)
    {
        BasicString<unsigned short, Detail::TempStringAllocator> captain(g_pLocalization->GetString(GetLOCTeamName(team)));
        if (numGoals == 0)
        {
            formatted = BasicString<unsigned short, Detail::TempStringAllocator>(g_pLocalization->GetString("MEGASTRIKE_SHUTOUT"));
        }
        else if (numGoals == 1)
        {
            formatted = Format(BasicString<unsigned short, Detail::TempStringAllocator>(g_pLocalization->GetString("MEGASTRIKE_SCORE_SG")), captain);
        }
        else
        {
            const unsigned short* unformatted = g_pLocalization->GetString("MEGASTRIKE_SCORE_PL");
            char numGoalsString[32];
            nlSNPrintf(numGoalsString, sizeof(numGoalsString), "%d", numGoals);
            unsigned short goalsWideString[32];
            nlStrToWcs(numGoalsString, goalsWideString, 32);
            formatted = Format(BasicString<unsigned short, Detail::TempStringAllocator>(unformatted), captain, goalsWideString);
        }
    }
    else if (mIsInOvertime)
    {
        formatted = BasicString<unsigned short, Detail::TempStringAllocator>(g_pLocalization->GetString("GOAL_MATCH_WINNER"));
    }
    else if (GameInfoManager::Instance()->GetCurrentSettings()->GameLimitType == 1
        && score[homeAway] >= GameInfoManager::Instance()->GetCurrentSettings()->GoalLimit)
    {
        formatted = BasicString<unsigned short, Detail::TempStringAllocator>(g_pLocalization->GetString("GOAL_MATCH_WINNER_GOALS"));
    }
    else if (!mHasSniperCup && gameInfo->HasTrophy(TROPHY_SNIPER_CUP) == true)
    {
        formatted = BasicString<unsigned short, Detail::TempStringAllocator>(g_pLocalization->GetString("GOAL_UNLOCKED_SNIPER"));
        mHasSniperCup = true;
    }
    else if (oldScore[0] == 0 && oldScore[1] == 0)
    {
        formatted = BasicString<unsigned short, Detail::TempStringAllocator>(g_pLocalization->GetString("GOAL_FIRST"));
    }
    else if (score[0] == score[1])
    {
        formatted = BasicString<unsigned short, Detail::TempStringAllocator>(g_pLocalization->GetString("GOAL_EQUALIZER"));
    }
    else if ((oldScore[0] >= oldScore[1] && score[0] < score[1]) || (oldScore[1] >= oldScore[0] && score[1] < score[0]))
    {
        const unsigned short* unformatted = g_pLocalization->GetString("GOAL_LEAD");
        const unsigned short* captain = g_pLocalization->GetString(GetLOCTeamName(team));
        formatted = Format(BasicString<unsigned short, Detail::TempStringAllocator>(unformatted), captain);
    }
    else if (playerIndex == 0 && mCaptainGoals[homeAway] == 3)
    {
        const unsigned short* unformatted = g_pLocalization->GetString("GOAL_CAPTAIN_3_GOALS");
        const unsigned short* captain = g_pLocalization->GetString(GetLOCTeamName(team));
        formatted = Format(BasicString<unsigned short, Detail::TempStringAllocator>(unformatted), captain);
    }
    else if (playerIndex == 0)
    {
        const unsigned short* unformatted = g_pLocalization->GetString("GOAL_CAPTAIN");
        const unsigned short* captain = g_pLocalization->GetString(GetLOCTeamName(team));
        char numGoalsString[32];
        nlSNPrintf(numGoalsString, sizeof(numGoalsString), "%d", mCaptainGoals[homeAway]);
        unsigned short goalsWideString[32];
        nlStrToWcs(numGoalsString, goalsWideString, 32);
        formatted = Format(BasicString<unsigned short, Detail::TempStringAllocator>(unformatted), goalsWideString, captain);
    }
    else
    {
        const unsigned short* unformatted = g_pLocalization->GetString("GOAL_SIDEKICK");
        const unsigned short* captain = g_pLocalization->GetString(GetLOCTeamName(team));
        char numGoalsString[32];
        nlSNPrintf(numGoalsString, sizeof(numGoalsString), "%d", mSidekickGoals[homeAway]);
        unsigned short goalsWideString[32];
        nlStrToWcs(numGoalsString, goalsWideString, 32);
        formatted = Format(BasicString<unsigned short, Detail::TempStringAllocator>(unformatted), goalsWideString, captain);
    }
    memcpy(mDescriptionBuffer, formatted.c_str(), sizeof(mDescriptionBuffer));
    pText = FEFinder<TLTextInstance, 3>::Find<TLSlide>(slide, "Layer", "Description");
    MakeTextBoxReallyWide(*pText);
    pText->SetString(mDescriptionBuffer);
}

void GoalOverlay::SetHighlightNumber(int highlight)
{
    SetWinnerTitle(2, true, 0);
    TLSlide* slide = mPresentation->GetActiveSlide();
    TLTextInstance* pText = FEFinder<TLTextInstance, 3>::Find<TLSlide>(slide, "Layer", "Description");
    MakeTextBoxReallyWide(*pText);
    if (highlight == 0)
    {
        pText->SetStringId("HIGHLIGHTS1");
        return;
    }

    BasicString<unsigned short, Detail::TempStringAllocator> unformatted(g_pLocalization->GetString("HIGHLIGHTS2"));
    char highlightString[32];
    nlSNPrintf(highlightString, sizeof(highlightString), "%d", highlight + 1);
    unsigned short highlightWideString[32];
    nlStrToWcs(highlightString, highlightWideString, 16);
    BasicString<unsigned short, Detail::TempStringAllocator> formatted(Format(unformatted, highlightWideString));
    memcpy(mDescriptionBuffer, formatted.c_str(), sizeof(mDescriptionBuffer));
    pText->SetString(mDescriptionBuffer);
}

void GoalOverlay::DoMatchEndOverlay()
{
    SetWinnerTitle(2, true, 0);
    BasicString<unsigned short, Detail::TempStringAllocator> formatted;
    int winner = NisPlayer::Instance()->mWinnerSide[NIS_GAME_WINNER];
    eTeamID winnerID = (eTeamID)GameInfoManager::Instance()->GetTeam((short)winner);
    eTeamID loserID = (eTeamID)GameInfoManager::Instance()->GetTeam((short)((winner + 1) % 2));
    if (GameInfoManager::Instance()->IsInFriendlyMode())
    {
        int scoreLeft = StatsTracker::Instance()->mNumGamesWon[0];
        int scoreRight = StatsTracker::Instance()->mNumGamesWon[1];
        int winsNeeded = GameInfoManager::Instance()->GetCurrentSettings()->NumGames / 2 + 1;
        unsigned short scoreLeftWideString[4];
        unsigned short scoreRightWideString[4];
        eTeamID winnerID;
        if (scoreLeft > scoreRight)
        {
            winnerID = (eTeamID)GameInfoManager::Instance()->GetTeam(0);
            nlSNPrintf(scoreLeftWideString, 4, (const unsigned short*)L"%d", scoreLeft);
            nlSNPrintf(scoreRightWideString, 4, (const unsigned short*)L"%d", scoreRight);
        }
        else
        {
            winnerID = (eTeamID)GameInfoManager::Instance()->GetTeam(1);
            nlSNPrintf(scoreLeftWideString, 4, (const unsigned short*)L"%d", scoreRight);
            nlSNPrintf(scoreRightWideString, 4, (const unsigned short*)L"%d", scoreLeft);
        }
        const CharacterInfo& character = GetCharacterInfo(GetCharacterIndexFromCaptain(winnerID));
        if ((scoreLeft > scoreRight && scoreLeft == winsNeeded) || (scoreRight > scoreLeft && scoreRight == winsNeeded))
        {
            BasicString<unsigned short, Detail::TempStringAllocator> unformatted(g_pLocalization->GetString("STH_DOMINATION_WINS_SERIES_0"));
            formatted = Format(unformatted, g_pLocalization->GetString(character.mDisplayNameKey), scoreLeftWideString, scoreRightWideString);
        }
        else if (scoreLeft == scoreRight)
        {
            BasicString<unsigned short, Detail::TempStringAllocator> unformatted(g_pLocalization->GetString("STH_DOMINATION_TIED_SERIES_0"));
            formatted = Format(unformatted, scoreLeftWideString, scoreRightWideString);
        }
        else
        {
            BasicString<unsigned short, Detail::TempStringAllocator> unformatted(g_pLocalization->GetString("STH_DOMINATION_LEADS_SERIES_0"));
            formatted = Format(unformatted, g_pLocalization->GetString(character.mDisplayNameKey), scoreLeftWideString, scoreRightWideString);
        }
    }
    else if (GameInfoManager::Instance()->IsInMode4() && g_pStrikerChallenge->mCondition == 2 && g_pTeams[1]->m_nScore > 0)
    {
        BasicString<unsigned short, Detail::TempStringAllocator> unformatted(g_pLocalization->GetString("SHUTOUT_FAILED"));
        formatted = Format(unformatted, g_pLocalization->GetString(GetLOCCharacterName(winnerID)));
    }
    else
    {
        BasicString<unsigned short, Detail::TempStringAllocator> unformatted(g_pLocalization->GetString("END_GAME_OVERLAY_MATCH"));
        formatted = Format(unformatted, g_pLocalization->GetString(GetLOCCharacterName(winnerID)),
            g_pLocalization->GetString(GetLOCCharacterName(loserID)));
    }
    TLSlide* slide = mPresentation->GetActiveSlide();
    TLTextInstance* pText = FEFinder<TLTextInstance, 3>::Find<TLSlide>(slide, "Layer", "Description");
    MakeTextBoxReallyWide(*pText);
    memcpy(mDescriptionBuffer, formatted.c_str(), sizeof(mDescriptionBuffer));
    mDescriptionBuffer[127] = 0;
    pText->SetString(mDescriptionBuffer);
}

void GoalOverlay::SetWinnerTitle(int homeAway, bool isMatchEnd, int numGoals)
{
    int score[2] = { g_pTeams[0]->m_nScore, g_pTeams[1]->m_nScore };
    score[homeAway] += numGoals;
    int scoreLeft = score[0];
    int scoreRight = score[1];
    char scoreLeftString[32];
    char scoreRightString[32];
    nlSNPrintf(scoreLeftString, sizeof(scoreLeftString), "%d", scoreLeft);
    nlSNPrintf(scoreRightString, sizeof(scoreRightString), "%d", scoreRight);
    unsigned short scoreLeftWideString[32];
    unsigned short scoreRightWideString[32];
    nlStrToWcs(scoreLeftString, scoreLeftWideString, 32);
    nlStrToWcs(scoreRightString, scoreRightWideString, 32);
    BasicString<unsigned short, Detail::TempStringAllocator> formatted;
    if (homeAway == 2)
        homeAway = scoreLeft <= scoreRight ? 1 : 0;
    int otherSide = !homeAway;
    const unsigned short* unformatted;
    if (isMatchEnd)
    {
        if (GameInfoManager::Instance()->IsInMode4() && g_pStrikerChallenge->mCondition == 2 && g_pTeams[1]->m_nScore > 0)
        {
            homeAway = 1;
            unformatted = g_pLocalization->GetString("POPUP_CHALLENGE_FAILED");
        }
        else if (GameInfoManager::Instance()->IsInMode4() && g_pStrikerChallenge->mCurrentChallenge == 2
            && g_pTeams[0]->m_nScore == g_pTeams[1]->m_nScore)
        {
            homeAway = 1;
            unformatted = g_pLocalization->GetString("POPUP_TUTORIAL_FAILED");
        }
        else
        {
            unformatted = g_pLocalization->GetString("HIGHLIGHTS_TITLE");
        }
    }
    else if (score[homeAway] > score[otherSide])
        unformatted = g_pLocalization->GetString("GOAL_TITLE_LEADS");
    else if (score[homeAway] < score[otherSide])
        unformatted = g_pLocalization->GetString("GOAL_TITLE_TRAILS");
    else
        unformatted = g_pLocalization->GetString("GOAL_TITLE_TIED");
    eTeamID winningTeam = (eTeamID)GameInfoManager::Instance()->GetTeam((short)homeAway);
    if (GameInfoManager::Instance()->IsInMode4() && g_pStrikerChallenge->mCondition == 2 && g_pTeams[1]->m_nScore > 0)
    {
        formatted = BasicString<unsigned short, Detail::TempStringAllocator>(g_pLocalization->GetString("POPUP_CHALLENGE_FAILED"));
    }
    else if (scoreLeft > scoreRight)
    {
        formatted = Format(BasicString<unsigned short, Detail::TempStringAllocator>(unformatted),
            g_pLocalization->GetString(GetLOCTeamName(winningTeam)), scoreLeftWideString, scoreRightWideString);
    }
    else
    {
        formatted = Format(BasicString<unsigned short, Detail::TempStringAllocator>(unformatted),
            g_pLocalization->GetString(GetLOCTeamName(winningTeam)), scoreRightWideString, scoreLeftWideString);
    }
    memcpy(mScoresBuffer, formatted.c_str(), sizeof(mScoresBuffer));
    TLSlide* slide = mPresentation->GetActiveSlide();
    TLTextInstance* pText = FEFinder<TLTextInstance, 3>::Find<TLSlide>(slide, "Layer", "Name");
    pText->SetString(mScoresBuffer);
    pText = FEFinder<TLTextInstance, 3>::Find<TLSlide>(slide, "Layer", "Time");
    if (!isMatchEnd && GameInfoManager::Instance()->GetCurrentSettings()->GameLimitType == 0
        && !GameInfoManager::Instance()->IsInMode4())
        pText->m_bVisible = true;
    else
        pText->m_bVisible = false;
    TLComponentInstance* component = FEFinder<TLComponentInstance, 4>::Find<TLSlide>(slide, "Layer", "lowerthirdgraphic");
    if (component != NULL)
        component->SetActiveSlide(GetTeamName(winningTeam), true, false);
}

void GoalOverlay::DoCupWinOverlay(int cup)
{
    const char* cupName;
    eTeamID winners;
    if (cup != -1)
    {
        NisPlayer* nisPlayer = NisPlayer::Instance();
        winners = (eTeamID)GameInfoManager::Instance()->GetTeam((short)nisPlayer->mWinnerSide[NIS_GAME_WINNER]);
    }
    else if (GameInfoManager::Instance()->IsInMode3())
    {
        winners = (eTeamID)g_pCupManager->GetUserSelectedCupTeam();
        cup = g_pCupManager->GetCurrentMode();
    }
    switch (cup)
    {
    case 0:
        cupName = "FIRE_CUP";
        break;
    case 1:
        cupName = "CRYSTAL_CUP";
        break;
    case 2:
        cupName = "STRIKER_CUP";
        break;
    }
    BasicString<unsigned short, Detail::TempStringAllocator> formatted(
        Format(BasicString<unsigned short, Detail::TempStringAllocator>(g_pLocalization->GetString("CUP_WIN_TITLE")),
            g_pLocalization->GetString(cupName)));
    memcpy(mScoresBuffer, formatted.c_str(), sizeof(mScoresBuffer));
    TLSlide* slide = mPresentation->GetActiveSlide();
    TLTextInstance* pText = FEFinder<TLTextInstance, 3>::Find<TLSlide>(slide, "Layer", "Name");
    pText->SetString(mScoresBuffer);
    formatted = Format(BasicString<unsigned short, Detail::TempStringAllocator>(g_pLocalization->GetString("CUP_WIN_TEXT")),
        g_pLocalization->GetString(GetLOCCharacterName(winners)));
    memcpy(mDescriptionBuffer, formatted.c_str(), sizeof(mDescriptionBuffer));
    pText = FEFinder<TLTextInstance, 3>::Find<TLSlide>(slide, "Layer", "Description");
    MakeTextBoxReallyWide(*pText);
    pText->SetString(mDescriptionBuffer);
    pText = FEFinder<TLTextInstance, 3>::Find<TLSlide>(slide, "Layer", "Time");
    pText->m_bVisible = false;
    TLComponentInstance* component = FEFinder<TLComponentInstance, 4>::Find<TLSlide>(slide, "Layer", "lowerthirdgraphic");
    if (component != NULL)
        component->SetActiveSlide(GetTeamName(winners), true, false);
}
