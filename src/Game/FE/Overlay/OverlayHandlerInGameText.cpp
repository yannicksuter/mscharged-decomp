#include "Game/OverlayHandlerInGameText.h"

#include "Game/BaseGameSceneManager.h"
#include "Game/DB/BasicGameInfo.h"
#include "Game/DB/StatsTracker.h"
#include "Game/FE/feFinder.h"
#include "Game/FE/feInput.h"
#include "Game/FE/fePackage.h"
#include "Game/FE/fePresentation.h"
#include "Game/FE/feScene.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlSlide.h"
#include "Game/FE/tlTextInstance.h"
#include "Game/Game.h"
#include "Game/GameInfo.h"
#include "Game/Team.h"
#include "NL/nlFormat.h"
#include "NL/nlLocalization.h"
#include "NL/nlPrint.h"
#include "NL/nlSingleton.h"
#include "NL/nlString.h"
#include "NL/nlTask.h"

extern "C" void* memcpy(void* dest, const void* src, unsigned long count);
extern "C" TLInstance* fn_8030677C(FEPresentation* pPresentation, unsigned long Level1,
    unsigned long Level2, unsigned long Level3, unsigned long Level4,
    unsigned long Level5, unsigned long Level6);
extern "C" int fn_80273B00();
extern "C" void fn_801E230C(
    BaseGameSceneManager* manager, SceneList scene, bool visibility,
    bool overrideStateSettings);
extern BaseGameSceneManager* lbl_806E1860;

const char* GetLOCTeamName(eTeamID team);

template <typename T>
static inline T* FindInPresentation(FEPresentation* presentation,
    unsigned long Level1, unsigned long Level2, unsigned long Level3)
{
    TLInstance* result = fn_8030677C(
        presentation, Level1, Level2, Level3, 0, 0, 0);
    if (result == 0)
    {
        return 0;
    }
    return (T*)result;
}

static inline const unsigned short* LookupLocHash(const char* stringId)
{
    nlLocalization* loc = g_pLocalization;
    unsigned long key = nlStringLowerHash(stringId);
    if (loc->m_LookupTable == 0)
    {
        return LocalizationTableNotFound;
    }

    nlLocalization::StringLookup* entry
        = nlBSearch<nlLocalization::StringLookup, unsigned long>(
            key, loc->m_LookupTable, (int)loc->m_pFile->StringCount);
    if (entry != 0)
    {
        return loc->m_FirstString + entry->StringOffset;
    }
    return MissingLocString;
}

static char* TEAM_SLIDE_NAMES[8] = {
    "DAISY",
    "DK",
    "LUIGI",
    "MARIO",
    "PEACH",
    "WALUIGI",
    "WARIO",
    "YOSHI",
};

static const char* OVERLAY_HANDLER_LAYER_NAME = "Layer";

static const InGameTextEntry IGTTable[8] = {
    { SLIDE_NAME_TEXT_GOAL, "GOAL!", 0 },
    { SLIDE_NAME_TEXT_KICKOFF, "KICKOFF!", 0 },
    { SLIDE_NAME_TEXT_WINNER, "WINNER!", 1 },
    { SLIDE_NAME_TEXT_PAUSE, "Pause", 1 },
    { SLIDE_NAME_TEXT_TIE, "TIE!", 1 },
    { SLIDE_NAME_TEXT_LOADING, "LOADING...", 1 },
    { SLIDE_NAME_TEXT_SHOOT, "Shoot!", 2 },
    { SLIDE_NAME_TEXT_REPLAY, "REPLAY", 8 },
};

InGameTextOverlay::InGameTextOverlay()
    : BaseOverlayHandler(2, POSITION_ALL)
{
    mCurrentSlideName = SLIDE_NAME_INVALID;
    mPendingSlideName = SLIDE_NAME_INVALID;
    SetVisible(false);
}

InGameTextOverlay::~InGameTextOverlay()
{
}

void InGameTextOverlay::SetSlide(OverlaySlideName slideName)
{
    mPendingSlideName = slideName;
    if (mCurrentSlideName != mPendingSlideName)
    {
        mFEScene->m_pFEPackage->GetPresentation()->SetActiveSlide(
            IGTTable[mPendingSlideName].mSlideName, true);
        TLSlide* CurrentSlide
            = mFEScene->m_pFEPackage->GetPresentation()->m_currentSlide;
        if (CurrentSlide != 0)
        {
            CurrentSlide->m_time = 0.0f;
            CurrentSlide->m_start = 0.0f;
            CurrentSlide->Update(0.0f);
        }
        if (mCurrentSlideName != SLIDE_NAME_INVALID)
        {
            mFEScene->m_pFEPackage->GetPresentation()->SetActiveSlide(
                IGTTable[mCurrentSlideName].mSlideName, true);
        }
    }
}

void InGameTextOverlay::Update(float fDeltaT)
{
    BaseSceneHandler::Update(fDeltaT);
    if (mCurrentSlideName != mPendingSlideName)
    {
        mCurrentSlideName = mPendingSlideName;
        if (mCurrentSlideName == SLIDE_NAME_TEXT_REPLAY && fn_80273B00() == 0)
        {
            mPresentation->SetActiveSlide("REPLAY 4:3", true);
        }
        else
        {
            mFEScene->m_pFEPackage->GetPresentation()->SetActiveSlide(
                IGTTable[mCurrentSlideName].mSlideName, true);
        }

        mVisibilityMask = IGTTable[mCurrentSlideName].mTaskVisibility;
        if (mVisibilityMask & nlTaskManager::m_pInstance->mCurrentState)
        {
            if (mWasLastVisible)
            {
                SetVisible(true);
            }
        }
        else
        {
            mWasLastVisible = mVisible;
            SetVisible(false);
        }

        switch (mCurrentSlideName)
        {
        case SLIDE_NAME_TEXT_WINNER:
            DisplayFinalScore();
            break;
        }
    }

    if (mCurrentSlideName == SLIDE_NAME_TEXT_WINNER
        && g_pFEInput->JustPressed(FE_ALL_PADS, 0x1E, true, 0) && mVisible)
    {
        fn_801E230C(lbl_806E1860, (SceneList)90, false, false);
        lbl_806E1860->Push((SceneList)91, SCREEN_NOTHING, false);
    }
}

void InGameTextOverlay::SceneCreated()
{
}

void InGameTextOverlay::DisplayFinalScore()
{
    typedef BasicString<unsigned short, Detail::TempStringAllocator> WideString;

    int scoreLeft = g_pTeams[0]->m_nScore;
    int scoreRight = g_pTeams[1]->m_nScore;

    char scoreLeftString[4];
    char scoreRightString[4];
    nlSNPrintf(scoreLeftString, 4, "%d", scoreLeft);
    nlSNPrintf(scoreRightString, 4, "%d", scoreRight);

    unsigned short scoreLeftWideString[32];
    unsigned short scoreRightWideString[32];
    nlStrToWcs(scoreLeftString, scoreLeftWideString, 32);
    nlStrToWcs(scoreRightString, scoreRightWideString, 32);

    WideString formatted(Format(WideString(LookupLocHash("FINAL_SCORE")),
        scoreLeftWideString,
        scoreRightWideString));

    FEPresentation* presentation
        = mFEScene->m_pFEPackage->GetPresentation();
    TLTextInstance* pTextInstance;
    const char* WINNER_SLIDE_NAME
        = IGTTable[SLIDE_NAME_TEXT_WINNER].mSlideName;
    long winningSide;

    if (mCurrentSlideName == SLIDE_NAME_TEXT_WINNER)
    {
        pTextInstance = FindInPresentation<TLTextInstance>(presentation,
            nlStringLowerHash(WINNER_SLIDE_NAME),
            nlStringLowerHash(OVERLAY_HANDLER_LAYER_NAME),
            nlStringLowerHash("Score"));

        winningSide = scoreLeft > scoreRight ? 0 : 1;
        eTeamID winningTeam = (eTeamID)nlSingleton<GameInfoManager>::Instance()->GetTeam(
            (short)winningSide);

        const unsigned short* winnerNameLookup
            = LookupLocHash(GetLOCTeamName(winningTeam));
        WideString winnerNameWideString(winnerNameLookup);

        if (winningTeam == (eTeamID)0)
        {
            WideString space((const unsigned short*)L" ");
            winnerNameWideString = space.Append(winnerNameWideString);
        }

        WideString formattedName(Format(
            WideString(LookupLocHash("THE_WINNER")), winnerNameWideString.c_str()));

        TLTextInstance* winnerNameTextInstance
            = FindInPresentation<TLTextInstance>(presentation,
                nlStringLowerHash(WINNER_SLIDE_NAME),
                nlStringLowerHash(OVERLAY_HANDLER_LAYER_NAME),
                nlStringLowerHash("name"));

        memcpy(mWinnerBuffer, formattedName.c_str(), sizeof(mWinnerBuffer));
        winnerNameTextInstance->SetString(mWinnerBuffer);

        eTeamID team = (eTeamID)nlSingleton<GameInfoManager>::Instance()->GetTeam(0);
        TLComponentInstance* pComponentInstance
            = FindInPresentation<TLComponentInstance>(presentation,
                nlStringLowerHash(WINNER_SLIDE_NAME),
                nlStringLowerHash(OVERLAY_HANDLER_LAYER_NAME),
                nlStringLowerHash("left_face"));
        pComponentInstance->SetActiveSlide(TEAM_SLIDE_NAMES[team], true, false);

        team = (eTeamID)nlSingleton<GameInfoManager>::Instance()->GetTeam(1);
        pComponentInstance = FindInPresentation<TLComponentInstance>(presentation,
            nlStringLowerHash(WINNER_SLIDE_NAME),
            nlStringLowerHash(OVERLAY_HANDLER_LAYER_NAME),
            nlStringLowerHash("right_face"));
        pComponentInstance->SetActiveSlide(TEAM_SLIDE_NAMES[team], true, false);

        if (nlSingleton<GameInfoManager>::Instance()->mCurrentMode != 0)
        {
            if (g_pGame->m_eGameState == 6)
            {
                StatsTracker::Track(STATS_LOSS, winningSide, 0,
                    scoreLeft, scoreRight, 0, 0);
            }
            else
            {
                StatsTracker::Track(STATS_WIN, winningSide, 0,
                    scoreLeft, scoreRight, 0, 0);
            }
        }
        else
        {
            StatsTracker::Instance()->mNumGamesWon[winningSide]++;
        }
    }

    memcpy(mScoresBuffer, formatted.c_str(), sizeof(mScoresBuffer));
    pTextInstance->SetString(mScoresBuffer);
}
