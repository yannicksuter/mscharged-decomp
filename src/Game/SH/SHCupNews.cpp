#include "Game/SH/SHCupNews.h"
#include "Game/FE/FEAudio.h"

#include "Game/GameSceneManager.h"
#include "Game/DB/CharacterInfo.h"
#include "Game/DB/GameProgress.h"
#include "Game/FE/feMusic.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/Render/FrontEndPresentation.h"
#include "Game/FE/feDPD.h"
#include "Game/FE/feCupFlow.h"
#include "NL/nlFormat.h"
#include "NL/nlLocalization.h"
#include "NL/nlPrint.h"

static const int sCupRankRanges[4][3][2] = {
    { { 0, 0 }, { 1, 2 }, { 3, 3 } },
    { { 0, 1 }, { 2, 4 }, { 5, 6 } },
    { { 0, 2 }, { 3, 6 }, { 7, 9 } },
    { { 0, 0 }, { 0, 0 }, { 0, 0 } },
};

int GetCupState(CupManager* cupManager)
{
    return cupManager->mState;
}

void CupNewsScene::OnDoneTransitionComplete()
{
    SHStrikerTimesBase::OnDoneTransitionComplete();

    for (int i = 0; i < 4; ++i)
    {
        GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
    }

    if (mDisplayMode == 1)
    {
        GameSceneManager::Instance()->Pop();
        FEAudio::PlayAnimAudioEvent(0xD276AFE5, 0, 0, 1);
        FrontEndPresentation::GetInstance()->Call("TransitionToStrikerCupHub");
    }
    else if (mDisplayMode == 0)
    {
        GameSceneManager::Instance()->Pop();
        FEAudio::PlayAnimAudioEvent(0xD276AFE5, 0, 0, 1);
        FrontEndPresentation::GetInstance()->Call("TransitionToInitialStrikerCupHub");
    }
    else if (mDisplayMode == 7)
    {
        GameSceneManager::Instance()->Pop();
        AdvanceCupAwardPresentation();
    }
    else if (mDisplayMode == 6)
    {
        GameSceneManager::Instance()->Pop();
        FinishCupAwardPresentation();
    }
    else if (mShowAwardsOnClose)
    {
        GameSceneManager::Instance()->Pop();
        BeginCupAwardPresentation();
        mShowAwardsOnClose = false;
    }
    else if (mShowWinnerRewardsOnClose)
    {
        GameSceneManager::Instance()->Pop();
        ShowCupTrophyRewardsPopup();
        mShowWinnerRewardsOnClose = false;
    }
    else
    {
        GameSceneManager::Instance()->Push((SceneList)31, SCREEN_NOTHING, true);
    }
}

void CupNewsScene::Update(float dt)
{
    SHStrikerTimesBase::Update(dt);
}

#pragma dont_inline on
void CupNewsScene::SceneCreated()
{
    typedef BasicString<unsigned short, Detail::TempStringAllocator> String;

    CupManager* cupManager = CupManager::Instance();
    int captain = cupManager->GetUserSelectedCupTeam();
    bool formatOpponent = false;
    int opponentCaptain = cupManager->fn_8010AFA4();
    const CharacterInfo& character = GetCharacterInfo(GetCharacterIndexFromCaptain(captain));
    int rank = cupManager->fn_8010E460();
    int cup = cupManager->GetCurrentMode();
    int mood = -1;
    char cupName[0x10];

    switch (cup)
    {
    case 0:
        nlSNPrintf(cupName, 0x10, "FIRE");
        break;
    case 1:
        nlSNPrintf(cupName, 0x10, "CRYSTAL");
        break;
    case 2:
        nlSNPrintf(cupName, 0x10, "STRIKER");
        break;
    }

    switch (mDisplayMode)
    {
    case 0:
    case 1:
        nlSNPrintf(mStoryStringID, 0x40, "ST_%s_INTRO_%s_%d", cupName, character.GetName(), 0);
        nlSNPrintf(mHeadlineStringID, 0x40, "STH_%s_INTRO_%s_%d", cupName, character.GetName(), 0);
        mood = 1;
        break;
    case 2:
        for (int i = 0; i < 3; ++i)
        {
            int minimumRank = sCupRankRanges[cup][i][0];
            int maximumRank = sCupRankRanges[cup][i][1];
            if (rank < minimumRank || rank > maximumRank)
                continue;

            switch (i)
            {
            case 0:
                nlSNPrintf(mStoryStringID, 0x40, "ST_%s_EXCELLENT_%s_%d", cupName, character.GetName(), 0);
                nlSNPrintf(mHeadlineStringID, 0x40, "STH_%s_EXCELLENT_%s_%d", cupName, character.GetName(), 0);
                mood = 0;
                break;
            case 1:
                nlSNPrintf(mStoryStringID, 0x40, "ST_%s_MODERATE_%s_%d", cupName, character.GetName(), 0);
                nlSNPrintf(mHeadlineStringID, 0x40, "STH_%s_MODERATE_%s_%d", cupName, character.GetName(), 0);
                mood = 1;
                break;
            case 2:
                nlSNPrintf(mStoryStringID, 0x40, "ST_%s_POOR_%s_%d", cupName, character.GetName(), 0);
                nlSNPrintf(mHeadlineStringID, 0x40, "STH_%s_POOR_%s_%d", cupName, character.GetName(), 0);
                mood = 2;
                break;
            }
            break;
        }
        break;
    case 3:
        mShowAwardsOnClose = true;
        if (GetCupState(cupManager) == 0x10)
        {
            nlSNPrintf(mStoryStringID, 0x40, "ST_%s_NOT_QUALIFY_%s_%d", cupName, character.GetName(), 0);
            nlSNPrintf(mHeadlineStringID, 0x40, "STH_%s_NOT_QUALIFY_%s_%d", cupName, character.GetName(), 0);
            mood = 2;
        }
        else
        {
            nlSNPrintf(mStoryStringID, 0x40, "ST_%s_QUALIFY_%s_%d", cupName, character.GetName(), 0);
            nlSNPrintf(mHeadlineStringID, 0x40, "STH_%s_QUALIFY_%s_%d", cupName, character.GetName(), 0);
            cupManager->SetShowCupPhasePopup(true);
            mood = 0;
        }
        break;
    case 4:
        if (GetCupState(cupManager) == 0x11)
        {
            nlSNPrintf(mStoryStringID, 0x40, "ST_%s_ELIMINATED_%s_%d", cupName, character.GetName(), 0);
            nlSNPrintf(mHeadlineStringID, 0x40, "STH_%s_ELIMINATED_%s_%d", cupName, character.GetName(), 0);
            mood = 2;
        }
        else
        {
            nlSNPrintf(mStoryStringID, 0x40, "ST_%s_NOT_ELIMINATED_%s_%d", cupName, character.GetName(), 0);
            nlSNPrintf(mHeadlineStringID, 0x40, "STH_%s_NOT_ELIMINATED_%s_%d", cupName, character.GetName(), 0);
            cupManager->SetShowCupPhasePopup(true);
            formatOpponent = true;
            mood = 0;
        }
        break;
    case 5:
        if (GetCupState(cupManager) == 0x12)
        {
            nlSNPrintf(mStoryStringID, 0x40, "ST_%s_LOSE_CUP_%s_%d", cupName, character.GetName(), 0);
            nlSNPrintf(mHeadlineStringID, 0x40, "STH_%s_LOSE_CUP_%s_%d", cupName, character.GetName(), 0);
            formatOpponent = true;
            mood = 2;
        }
        else
        {
            nlSNPrintf(mStoryStringID, 0x40, "ST_%s_WIN_CUP_%s_%d", cupName, character.GetName(), 0);
            nlSNPrintf(mHeadlineStringID, 0x40, "STH_%s_WIN_CUP_%s_%d", cupName, character.GetName(), 0);
            formatOpponent = true;
            mood = 3;
            mShowWinnerRewardsOnClose = true;
        }
        break;
    case 6:
    {
        int statistic = 0;
        CupManager::Instance()->fn_8010D9C4(&statistic);
        String name(g_pLocalization->GetString(character.GetDisplayNameKey()));
        nlSNPrintf(mStoryStringID, 0x40, "ST_CUP_AWARDS_GOLDENBOOT_%s", cupName);
        nlSNPrintf(mHeadlineStringID, 0x40, "STH_CUP_AWARDS_GOLDENBOOT_%s", cupName);
        const unsigned short* story = g_pLocalization->GetString(mStoryStringID);
        mHeadlineText = Format(String(g_pLocalization->GetString(mHeadlineStringID)), name);
        mStoryText = Format(String(story), name);
        mUseCustomText = true;
        mood = 0;
        break;
    }
    case 7:
    {
        int statistic = 0;
        CupManager::Instance()->fn_8010DE2C(&statistic);
        String name(g_pLocalization->GetString(character.GetDisplayNameKey()));
        nlSNPrintf(mStoryStringID, 0x40, "ST_CUP_AWARDS_BRICK_%s", cupName);
        nlSNPrintf(mHeadlineStringID, 0x40, "STH_CUP_AWARDS_BRICK_%s", cupName);
        const unsigned short* story = g_pLocalization->GetString(mStoryStringID);
        mHeadlineText = Format(String(g_pLocalization->GetString(mHeadlineStringID)), name);
        mStoryText = Format(String(story), name);
        mUseCustomText = true;
        mood = 0;
        break;
    }
    }

    if (formatOpponent == true && !mUseCustomText)
    {
        const CharacterInfo& opponent = GetCharacterInfo(GetCharacterIndexFromCaptain(opponentCaptain));
        String headline(g_pLocalization->GetString(mHeadlineStringID));
        String story(g_pLocalization->GetString(mStoryStringID));
        String name(g_pLocalization->GetString(opponent.GetDisplayNameKey()));
        mHeadlineText = Format(headline, name);
        mStoryText = Format(story, name);
        mUseCustomText = true;
    }

    SetArticleImageName(captain, mood, -1);
    SHStrikerTimesBase::SceneCreated();

    switch (mood)
    {
    case 0:
        if (!FEMusic::IsPlayingCupResultStream())
            FEMusic::StartStreamIfDifferent(4);
        break;
    case 1:
        if (!FEMusic::IsPlayingCupResultStream())
            FEMusic::StartStreamIfDifferent(6);
        break;
    case 2:
        if (!FEMusic::IsPlayingCupResultStream())
            FEMusic::StartStreamIfDifferent(5);
        break;
    case 3:
        if (!FEMusic::IsPlayingCupResultStream())
            FEMusic::StartStreamIfDifferent(7);
        break;
    }
}
#pragma dont_inline reset

void CupNewsScene::SetDisplayMode(unsigned int transition)
{
    SHStrikerTimesBase::SetDisplayMode(transition);
}

CupNewsScene::~CupNewsScene()
{
}

CupNewsScene::CupNewsScene()
    : mShowAwardsOnClose(false)
    , mShowWinnerRewardsOnClose(false)
{
}
