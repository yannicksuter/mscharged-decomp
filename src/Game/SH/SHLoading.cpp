#include "Game/HBMManager.h"

#include "Game/SH/SHLoading.h"

#include "Game/BaseGameSceneManager.h"
#include "Game/BaseSceneHandler.h"
#include "Game/DB/StadiumInfo.h"
#include "Game/DB/CharacterInfo.inl"
#include "Game/DB/GameProgress.h"
#include "Game/FE/tlImageInstance.h"
#include "NL/nlLocalizationLookup.h"
#include "NL/nlFormat.h"
#include <string.h>
#include "Game/FE/feFinder.h"
#include "Game/FE/feFinder.inl"
#include "Game/FE/fePackage.h"
#include "Game/FE/fePresentation.h"
#include "Game/FE/feScene.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlSlide.h"
#include "Game/FE/tlTextInstance.h"
#include "Game/GameInfo.h"
#include "Game/NetTournManager.h"
#include "Game/Render/FrontEndPresentation.h"
#include "NL/nlString.h"
#include "NL/nlTask.h"
#include "Game/Render/RLViewLayers.h"
#include "NL/nlPrint.h"
#include "Game/FE/FEAudio.h"

SuperLoadingScene::SuperLoadingScene()
    : mType(TT_INVALID)
{
    gpHBMManager->SetBlocked(true);
}

SuperLoadingScene::~SuperLoadingScene()
{
    gpHBMManager->SetBlocked(false);
}

void SuperLoadingScene::Update(float fDeltaT)
{
    BaseSceneHandler::Update(fDeltaT);

    if (mType == TT_3D_TRANSITION)
    {
        if (!FrontEndPresentation::GetInstance()->IsActive())
        {
            nlTaskManager::SetNextState(0x200000);
        }
    }
    else
    {
        TLSlide* slide = mFEScene->m_pFEPackage->GetPresentation()->m_currentSlide;
        float duration = slide->m_duration;
        float start = slide->m_start;
        float time = slide->m_time;
        if (time >= start + duration)
        {
            if (mType == TT_IN)
            {
                nlTaskManager::SetNextState(0x200000);
            }
            else if (mType == TT_OUT)
            {
                g_pOverlayManager->Pop();
            }
        }
    }
}

BaseLoadingScene::BaseLoadingScene()
    : mTransitionActive(false)
    , mWidescreen(false)
{
}

BaseLoadingScene::~BaseLoadingScene()
{
}

void BaseLoadingScene::Update(float dt)
{
    BaseSceneHandler::Update(dt);

    if (mTransitionActive)
    {
        TLSlide* slide = mTransitionComponent->GetActiveSlide();
        float duration = slide->m_duration;
        float start = slide->m_start;
        float time = slide->m_time;
        if (time >= start + duration)
        {
            mTransitionComponent->m_bVisible = false;
            mTransitionActive = false;
        }
    }
}

MatchLoadingScene::MatchLoadingScene()
{
}

MatchLoadingScene::~MatchLoadingScene()
{
}

void MatchLoadingScene::Update(float dt)
{
    BaseSceneHandler::Update(dt);

    if (mTransitionActive)
    {
        TLSlide* slide = mTransitionComponent->GetActiveSlide();
        float duration = slide->m_duration;
        float start = slide->m_start;
        float time = slide->m_time;
        if (time >= start + duration)
        {
            mTransitionComponent->m_bVisible = false;
            mTransitionActive = false;
        }
    }
}

inline void MatchLoadingScene::DisplayBestOfText(TLTextInstance* text, int numGames)
{
    const unsigned short* unformatted = g_pLocalization->GetString("BEST_OF_X");
    unsigned short games[4];
    nlSNPrintf(games, 4, (const unsigned short*)L"%d", numGames);
    {
        typedef BasicString<unsigned short, Detail::TempStringAllocator> WideString;
        WideString formatted = Format(WideString(unformatted), games);
        memcpy(mTextBuffers[0], formatted.c_str(), 128);
        text->SetString(mTextBuffers[0]);
    }
    DisplayStadiumName(mTextInstances[1]);
    mTextInstances[2]->m_bVisible = false;
    mTextInstances[3]->m_bVisible = false;
    mTextInstances[4]->m_bVisible = false;
    mTextInstances[5]->m_bVisible = false;
}

void MatchLoadingScene::DisplayOnlineInfo()
{
    int numGames = GameInfoManager::Instance()->GetCurrentSettings()->NumGames;
    DisplayBestOfText(mTextInstances[0], numGames);
}

void MatchLoadingScene::DisplayStadiumName(TLTextInstance* stadiumText)
{
    const char* stringID = GetStadiumTickerStringID(GameInfoManager::Instance()->GetStadium());
    stadiumText->SetStringId(stringID);
}

void SuperLoadingScene::SceneCreated()
{
    FEPresentation* pres = mFEScene->m_pFEPackage->GetPresentation();
    if (mType == TT_IN)
    {
        pres->SetActiveSlide("appear", true);
    }
    else if (mType == TT_OUT)
    {
        pres->SetActiveSlide("disappear", true);
    }
    else if (mType == TT_3D_TRANSITION)
    {
        pres->SetActiveSlide("3dtransition", true);

        GameInfoManager* gameInfo = GameInfoManager::Instance();
        if (gameInfo->mIsOnlineMode)
        {
            if (gameInfo->mCurrentMode == 0)
            {
                FEAudio::PlayAnimAudioEvent(0x7FEC4468, 0, 0, 1);
                FrontEndPresentation::GetInstance()->Call("StartOnlineGrudgeMatchSequence");
            }
            else if (NetTournManager::Instance()->mState != 0)
            {
                FrontEndPresentation::GetInstance()->Call("TransitionOnlineTournamentToGame");
            }
        }
        else if (gameInfo->mCurrentMode == 0)
        {
            FrontEndPresentation::GetInstance()->Call("StartGrudgeMatchSequence");
        }
        else if (gameInfo->mCurrentMode == 3)
        {
            FrontEndPresentation::GetInstance()->Call("StartCupMatchSequence");
        }
    }
}

void BaseLoadingScene::SceneCreated()
{
    mTransitionComponent = FEFinder<TLComponentInstance, TLAT_COMPONENT>::FindOrDefault(mPresentation->m_currentSlide, "Layer", "no home");
    mTransitionComponent->m_bVisible = false;

    if (IsWidescreen())
    {
        mWidescreen = true;
        mTransitionComponent->SetActiveSlide("widescreen", true, false);
    }

    gpHBMManager->SetBlocked(false);
}

void BaseLoadingScene::OnHomeButtonPressed()
{
    if (mFEScene == 0 || mFEScene->mState != 6 || mTransitionActive)
    {
        return;
    }

    mTransitionComponent->m_bVisible = true;
    if (mWidescreen)
    {
        mTransitionComponent->SetActiveSlide("widescreen", true, false);
    }
    else
    {
        mTransitionComponent->SetActiveSlide("Slide1", true, false);
    }
    mTransitionActive = true;
}

void MatchLoadingScene::SceneCreated()
{
    BaseLoadingScene::SceneCreated();

    GameInfoManager* gameInfo = GameInfoManager::Instance();
    FEPresentation* pres = mFEScene->GetPackage()->GetPresentation();
    int homeTeam = gameInfo->GetTeam(0);
    int awayTeam = gameInfo->GetTeam(1);
    const CharacterInfo& homeCharacter = GetCharacterInfo(GetCharacterIndexFromCaptain(homeTeam));
    const CharacterInfo& awayCharacter = GetCharacterInfo(GetCharacterIndexFromCaptain(awayTeam));

    TLComponentInstance* controllerInstances[2][4];
    for (int side = 0; side < 2; ++side)
    {
        for (int controller = 0; controller < 4; ++controller)
        {
            char name[32];
            if (side == 0)
                nlSNPrintf(name, sizeof(name), "HOME_CONTROLLER_%d", controller + 1);
            else if (side == 1)
                nlSNPrintf(name, sizeof(name), "AWAY_CONTROLLER_%d", controller + 1);
            controllerInstances[side][controller] = FEFinder<TLComponentInstance, TLAT_COMPONENT>::FindOrDefault(pres, "Layer", name);
            controllerInstances[side][controller]->SetVisible(false);
        }
    }

    if (!gameInfo->IsOnline())
    {
        int homeController = 0;
        int awayController = 0;
        for (int controller = 0; controller < 4; ++controller)
        {
            short side = gameInfo->GetPlayingSide(controller);
            TLComponentInstance* instance = 0;
            if (side == 0)
            {
                instance = controllerInstances[side][homeController];
                ++homeController;
            }
            else if (side == 1)
            {
                instance = controllerInstances[side][awayController];
                ++awayController;
            }
            if (instance != 0)
            {
                instance->SetVisible(true);
                TLTextInstance* text = FEFinder<TLTextInstance, TLAT_TEXT>::FindOrDefault(instance->GetActiveSlide(), "PLAYER1");
                switch (controller)
                {
                case 0: text->SetStringId("P1"); break;
                case 1: text->SetStringId("P2"); break;
                case 2: text->SetStringId("P3"); break;
                case 3: text->SetStringId("P4"); break;
                }
            }
        }
    }

    TLComponentInstance* homeName = FEFinder<TLComponentInstance, TLAT_COMPONENT>::FindOrDefault(pres, "Layer", "HOME NAMES");
    TLComponentInstance* awayName = FEFinder<TLComponentInstance, TLAT_COMPONENT>::FindOrDefault(pres, "Layer", "AWAY NAMES");
    homeName->SetActiveSlide(homeCharacter.GetName(), true, false);
    awayName->SetActiveSlide(awayCharacter.GetName(), true, false);

    mTextInstances[0] = FEFinder<TLTextInstance, TLAT_TEXT>::FindOrDefault(pres, "Layer", "TOP TEXT");
    mTextInstances[1] = FEFinder<TLTextInstance, TLAT_TEXT>::FindOrDefault(pres, "Layer", "BOTTOM TEXT");
    mTextInstances[2] = FEFinder<TLTextInstance, TLAT_TEXT>::FindOrDefault(pres, "Layer", "HOME_STATS1");
    mTextInstances[3] = FEFinder<TLTextInstance, TLAT_TEXT>::FindOrDefault(pres, "Layer", "HOME_STATS2");
    mTextInstances[4] = FEFinder<TLTextInstance, TLAT_TEXT>::FindOrDefault(pres, "Layer", "AWAY_STATS1");
    mTextInstances[5] = FEFinder<TLTextInstance, TLAT_TEXT>::FindOrDefault(pres, "Layer", "AWAY_STATS2");

    if (GameInfoManager::Instance()->IsOnline())
        DisplayOnlineInfo();
    else if (GameInfoManager::Instance()->IsInMode3())
        DisplayCupInfo();
    else if (GameInfoManager::Instance()->IsInMode4())
        DisplayChallengeInfo();
    else
        DisplayFriendlyInfo();

    TLComponentInstance* versus = FEFinder<TLComponentInstance, TLAT_COMPONENT>::Find<>(GetPresentation()->GetActiveSlide(), "Layer", "VS");
    if (g_pLocalization->GetCurrentLanguage() == nlLocalization::LangSpanish
        || g_pLocalization->GetCurrentLanguage() == nlLocalization::LangNASpanish)
        versus->SetActiveSlide("contra", true, false);
    else
        versus->SetActiveSlide("Slide1", true, false);

    if (homeTeam != 4)
        SetTeamLogo(0, homeCharacter);
    if (awayTeam != 0)
        SetTeamLogo(1, awayCharacter);
}

void MatchLoadingScene::DisplayFriendlyInfo()
{
    int numGames = GameInfoManager::Instance()->GetCurrentSettings()->NumGames;
    DisplayBestOfText(mTextInstances[0], numGames);
}

void MatchLoadingScene::DisplayCupInfo()
{
    typedef BasicString<unsigned short, Detail::TempStringAllocator> WideString;
    CupManager* cup = CupManager::Instance();
    DisplayStadiumName(mTextInstances[1]);
    GameInfoManager* gameInfo = GameInfoManager::Instance();
    int homeTeam = gameInfo->GetTeam(0);
    int awayTeam = gameInfo->GetTeam(1);
    if (cup->GetCurrentRoundType() == 2)
    {
        mTextInstances[2]->SetVisible(false);
        mTextInstances[3]->SetVisible(false);
        mTextInstances[4]->SetVisible(false);
        mTextInstances[5]->SetVisible(false);
    }
    else
    {
        int homeRank = cup->GetTeamRank(homeTeam);
        int awayRank = cup->GetTeamRank(awayTeam);
        unsigned short homeRankString[4], awayRankString[4];
        nlSNPrintf(homeRankString, 4, (const unsigned short*)L"%d", homeRank + 1);
        nlSNPrintf(awayRankString, 4, (const unsigned short*)L"%d", awayRank + 1);
        const unsigned short* unformatted = g_pLocalization->GetString("LOADING_SCREEN_RANK");
        WideString homeRankText = Format(WideString(unformatted), homeRankString);
        memcpy(mTextBuffers[1], homeRankText.c_str(), sizeof(mTextBuffers[1]));
        mTextInstances[2]->SetString(mTextBuffers[1]);
        WideString awayRankText = Format(WideString(unformatted), awayRankString);
        memcpy(mTextBuffers[3], awayRankText.c_str(), sizeof(mTextBuffers[3]));
        mTextInstances[4]->SetString(mTextBuffers[3]);

        unformatted = g_pLocalization->GetString("ROAD_HUB_TEAM_RECORD_STATS");
        WideString homeRecord, awayRecord;
        TeamStats homeStats = cup->GetTeamStats(homeTeam);
        TeamStats awayStats = cup->GetTeamStats(awayTeam);
        unsigned short wins[2][4], losses[2][4], suddenDeathLosses[2][4];
        nlSNPrintf(wins[0], 4, (const unsigned short*)L"%d", homeStats.unknown_0x10);
        nlSNPrintf(losses[0], 4, (const unsigned short*)L"%d", homeStats.unknown_0x12);
        nlSNPrintf(suddenDeathLosses[0], 4, (const unsigned short*)L"%d", homeStats.unknown_0x14);
        nlSNPrintf(wins[1], 4, (const unsigned short*)L"%d", awayStats.unknown_0x10);
        nlSNPrintf(losses[1], 4, (const unsigned short*)L"%d", awayStats.unknown_0x12);
        nlSNPrintf(suddenDeathLosses[1], 4, (const unsigned short*)L"%d", awayStats.unknown_0x14);
        homeRecord = Format(WideString(unformatted), wins[0], losses[0], suddenDeathLosses[0]);
        awayRecord = Format(WideString(unformatted), wins[1], losses[1], suddenDeathLosses[1]);
        memcpy(mTextBuffers[2], homeRecord.c_str(), sizeof(mTextBuffers[2]));
        mTextInstances[3]->SetString(mTextBuffers[2]);
        memcpy(mTextBuffers[4], awayRecord.c_str(), sizeof(mTextBuffers[4]));
        mTextInstances[5]->SetString(mTextBuffers[4]);
    }

    const unsigned short* cupName;
    int cupType = cup->GetCurrentMode();
    if (cupType == 0)
        cupName = g_pLocalization->GetString("FIRE_CUP");
    else if (cupType == 1)
        cupName = g_pLocalization->GetString("CRYSTAL_CUP");
    else if (cupType == 2)
        cupName = g_pLocalization->GetString("STRIKER_CUP");
    WideString formatted;
    int roundType = CupManager::Instance()->GetCurrentRoundType();
    FEPresentation* pres = mFEScene->GetPackage()->GetPresentation();
    if (roundType == 0)
    {
        int numRounds = CupManager::Instance()->GetNumRegularRounds();
        int round = CupManager::Instance()->GetCurrentRoundNumber() + 1;
        char total[4], current[4];
        unsigned short totalWide[4], currentWide[4];
        nlSNPrintf(total, 4, "%d", numRounds);
        nlSNPrintf(current, 4, "%d", round);
        nlStrToWcs(total, totalWide, 4);
        nlStrToWcs(current, currentWide, 4);
        formatted = Format(WideString(g_pLocalization->GetString("CUP_LOADING_QUALIFY")), cupName, currentWide, totalWide);
    }
    else if (roundType == 1)
    {
        int numRounds = CupManager::Instance()->GetNumPlayoffRounds();
        int round = CupManager::Instance()->GetCurrentRoundNumber();
        const unsigned short* unformatted;
        if (round == numRounds - 3)
            unformatted = g_pLocalization->GetString("CUP_LOADING_QUARTER");
        else if (round == numRounds - 2)
            unformatted = g_pLocalization->GetString("CUP_LOADING_SEMI");
        else
            unformatted = g_pLocalization->GetString("CUP_LOADING_FINAL");
        formatted = Format(WideString(unformatted), cupName);
    }
    else if (roundType == 2)
    {
        unsigned short game[4];
        nlSNPrintf(game, 4, (const unsigned short*)L"%d", CupManager::Instance()->GetCurrentRoundNumber() + 1);
        formatted = Format(WideString(g_pLocalization->GetString("CUP_LOADING_CHAMPIONSHIP")), cupName, game);
    }
    memcpy(mTextBuffers[0], formatted.c_str(), 128);
    mTextInstances[0]->SetString(mTextBuffers[0]);
}

void MatchLoadingScene::DisplayChallengeInfo()
{
    char objective[64];
    if (g_pStrikerChallenge->mCurrentChallenge < 10)
        nlSNPrintf(objective, sizeof(objective), "tutorial_objective_%s", g_pStrikerChallenge->GetTitle());
    else
        nlSNPrintf(objective, sizeof(objective), "objective_%s", g_pStrikerChallenge->GetTitle());
    mTextInstances[0]->SetStringId(g_pStrikerChallenge->GetDifficulty());
    mTextInstances[1]->SetStringId(objective);
    mTextInstances[2]->m_bVisible = false;
    mTextInstances[3]->m_bVisible = false;
    mTextInstances[4]->m_bVisible = false;
    mTextInstances[5]->m_bVisible = false;
}

void MatchLoadingScene::SetTeamLogo(int side, CharacterInfo character)
{
    TLImageInstance* image;
    if (side == 0)
        image = FEFinder<TLImageInstance, TLAT_IMAGE>::Find(mPresentation, "Slide1", "Layer", "logos_TEAM_LUIGI");
    else
        image = FEFinder<TLImageInstance, TLAT_IMAGE>::Find(mPresentation, "Slide1", "Layer", "logos_TEAM_MARIO");
    char name[25];
    nlSNPrintf(name, sizeof(name), "logos_TEAM_%s", character.mName);
    TLImageInstance* source = FEFinder<TLImageInstance, TLAT_IMAGE>::Find(mPresentation, "art", "Layer", name);
    if (source->m_pTextureResource != 0)
        image->m_pTextureResource = source->m_pTextureResource;
}

#include "Game/BaseSceneHandler.inl"

#include "Game/FE/feFinder_impl.h"
