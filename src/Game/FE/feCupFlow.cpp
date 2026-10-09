#include "Game/GameSceneManager.h"
#include "Game/FE/feCupFlow.h"

#include "Game/BaseGameSceneManager.h"
#include "Game/GameInfo.h"
#include "Game/DB/GameProgress.h"
#include "Game/DB/SaveLoad.h"
#include "Game/FE/feDPD.h"
#include "Game/FE/FEAudio.h"
#include "Game/FE/feHelpFuncs_decl.h"
#include "Game/FE/feInlineHasher.h"
#include "Game/FE/feInput.h"
#include "Game/FE/feFinder_impl.h"
#include "Game/FE/feMusic.h"
#include "Game/FE/fePopupMenu.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/Render/FrontEndPresentation.h"
#include "Game/Render/StadiumWorldObjects.h"
#include "Game/SH/SHCupNews.h"
#include "Game/SH/SHNavigation.h"
#include "Game/TweakQuery.h"
#include "NL/nlFormat.h"
#include "NL/nlLocalizationLookup.h"
#include "NL/nlstring_tmpl.h"
#include "NL/nlFunction.inl"

bool gMainMenuInputResetPending;
int gCupAwardModelCount;
StadiumCupTrophyDrawable* gCupAwardModels[10];
unsigned int gFEControllerIndex;
bool gFEPointerEnabled[4];

extern const int sCupPageOrder[3] = { CUP_PAGE_STANDINGS, CUP_PAGE_GOLDEN_BOOT, CUP_PAGE_BRICK_WALL };
extern const int sCupRoundPageOrderThree[3] = { CUP_PAGE_FINAL_ROUNDS, CUP_PAGE_KNOCKOUT, CUP_PAGE_SCHEDULE };
extern const int sCupRoundPageOrderTwo[2] = { CUP_PAGE_KNOCKOUT, CUP_PAGE_SCHEDULE };

struct CupTrophyUnlock
{
    unsigned long key;
    unsigned int flag;
};

typedef BasicString<unsigned short, Detail::TempStringAllocator> WideString;


CupTrophyUnlock gCupTrophyUnlockFlags[] = {
    { nlStringHash("0"), 0x001 },
    { nlStringHash("1"), 0x008 },
    { nlStringHash("2"), 0x010 },
    { nlStringHash("3"), 0x002 },
    { nlStringHash("4"), 0x020 },
    { nlStringHash("5"), 0x040 },
    { nlStringHash("6"), 0x004 },
    { nlStringHash("7"), 0x080 },
    { nlStringHash("8"), 0x100 },
};

static void CycleCupStatsPage(int currentPage, bool advance);

void CycleCupPage(int currentPage, bool advance)
{
    if ((unsigned int)(currentPage - CUP_PAGE_STANDINGS) <= 2)
    {
        CycleCupStatsPage(currentPage, advance);
    }
    else
    {
        CycleCupRoundPage(currentPage, advance);
    }
}

static void CycleCupStatsPage(int currentPage, bool advance)
{
    int currentIndex = 0;
    for (int i = 0; i < 3; ++i)
    {
        if (currentPage == sCupPageOrder[i])
        {
            currentIndex = i;
            break;
        }
    }

    int nextIndex = currentIndex - 1;
    if (advance)
    {
        nextIndex = currentIndex + 1;
    }
    if (nextIndex >= 3)
    {
        nextIndex = 0;
    }
    else if (nextIndex < 0)
    {
        nextIndex = 2;
    }

    switch (sCupPageOrder[nextIndex])
    {
    case CUP_PAGE_STANDINGS:
        GameSceneManager::Instance()->Push(SCENE_CUP_STANDINGS, SCREEN_NOTHING, true);
        break;
    case CUP_PAGE_GOLDEN_BOOT:
        GameSceneManager::Instance()->Push(SCENE_CUP_GOLDEN_BOOT, SCREEN_NOTHING, true);
        break;
    case CUP_PAGE_BRICK_WALL:
        GameSceneManager::Instance()->Push(SCENE_CUP_BRICK_WALL, SCREEN_NOTHING, true);
        break;
    }
}

void CycleCupRoundPage(int currentPage, bool advance)
{
    int currentIndex = 0;
    int pageCount = 0;
    const int* pages = 0;

    int roundType = CupManager::s_pInstance->mState == CUP_STATE_NOT_QUALIFIED
                      ? CUP_ROUND_LEAGUE
                      : CupManager::s_pInstance->GetCurrentRoundType();

    if (roundType != CUP_ROUND_LEAGUE)
    {
        switch (roundType)
        {
        case CUP_ROUND_KNOCKOUT:
            pageCount = 2;
            pages = sCupRoundPageOrderTwo;
            break;
        case CUP_ROUND_FINALS:
            pageCount = 3;
            pages = sCupRoundPageOrderThree;
            break;
        }

        for (int i = 0; i < pageCount; ++i)
        {
            if (currentPage == pages[i])
            {
                currentIndex = i;
                break;
            }
        }

        if (advance == true)
        {
            ++currentIndex;
        }
        else
        {
            --currentIndex;
        }
        if (currentIndex >= pageCount)
        {
            currentIndex = 0;
        }
        else if (currentIndex < 0)
        {
            currentIndex = pageCount - 1;
        }

        switch (pages[currentIndex])
        {
        case CUP_PAGE_SCHEDULE:
            GameSceneManager::Instance()->Push(SCENE_CUP_SCHEDULE, SCREEN_NOTHING, true);
            break;
        case CUP_PAGE_KNOCKOUT:
            GameSceneManager::Instance()->Push(SCENE_CUP_KNOCKOUT, SCREEN_NOTHING, true);
            break;
        case CUP_PAGE_FINAL_ROUNDS:
            GameSceneManager::Instance()->Push(SCENE_CUP_FINAL_ROUNDS, SCREEN_NOTHING, true);
            break;
        }
    }
}

void ShowFirstCupPage()
{
    GameSceneManager::Instance()->Push(SCENE_CUP_STANDINGS, SCREEN_NOTHING, true);
}

void ShowCurrentCupRoundPage()
{
    int scene = SCENE_INVALID;
    int roundType = CupManager::s_pInstance->mState == CUP_STATE_NOT_QUALIFIED
                      ? CUP_ROUND_LEAGUE
                      : CupManager::s_pInstance->GetCurrentRoundType();

    switch (roundType)
    {
    case CUP_ROUND_LEAGUE:
        scene = SCENE_CUP_SCHEDULE;
        break;
    case CUP_ROUND_KNOCKOUT:
        scene = SCENE_CUP_KNOCKOUT;
        break;
    case CUP_ROUND_FINALS:
        scene = SCENE_CUP_FINAL_ROUNDS;
        break;
    }

    GameSceneManager::Instance()->Push((SceneList)scene, SCREEN_NOTHING, true);
}

void AdvanceCupFlow(bool pad)
{
    CupManager* cupManager = CupManager::s_pInstance;
    if (cupManager->GetCurrentRoundNumber() == -5)
    {
        if (cupManager->mState == CUP_STATE_WON)
        {
            if (cupManager->GetCurrentMode() == CUP_FIRE)
            {
                cupManager->SetMode(CUP_CRYSTAL);
                cupManager->mState = CUP_STATE_NONE;
                cupManager->DetermineNextMatchups(19);
                CupNewsScene* scene = (CupNewsScene*)GameSceneManager::Instance()->Push(
                    SCENE_CUP_NEWS, SCREEN_NOTHING, true);
                scene->SetDisplayMode(NEWS_NEXT_CUP);
                SaveLoad::StartSave(false);
            }
            else if (cupManager->GetCurrentMode() == CUP_CRYSTAL)
            {
                cupManager->SetMode(CUP_STRIKER);
                cupManager->mState = CUP_STATE_NONE;
                cupManager->DetermineNextMatchups(19);
                CupNewsScene* scene = (CupNewsScene*)GameSceneManager::Instance()->Push(
                    SCENE_CUP_NEWS, SCREEN_NOTHING, true);
                scene->SetDisplayMode(NEWS_NEXT_CUP);
                SaveLoad::StartSave(false);
            }
            else
            {
                FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push(
                    SCENE_POPUP_MENU, SCREEN_NOTHING, true);
                popup->Create(POPUP_STRIKER_CUP_COMPLETE, Function<FnVoidVoid>(ResetCupFlow));
            }
        }
        else
        {
            FEMusic::StartStreamIfDifferent(9);
            CupManager::s_pInstance->RestoreCupRecord();
            CupManager::s_pInstance->RestartCupSeries();
            GameSceneManager::Instance()->Push(SCENE_ROAD_TO_STRIKERS_CUP, SCREEN_NOTHING, true);
            SaveLoad::StartSave(false);
        }
    }
    else
    {
        CupManager* currentCup = CupManager::s_pInstance;
        GameInfoManager* currentGame = GameInfoManager::Instance();
        bool home = currentGame->GetTeam(0)
                    == currentCup->GetUserSelectedCupTeam();
        GameInfoManager::Instance()->ResetPlayingSides();
        if (home)
        {
            GameInfoManager::Instance()->SetPlayingSide((unsigned short)pad, 0);
        }
        else
        {
            GameInfoManager::Instance()->SetPlayingSide((unsigned short)pad, 1);
        }

        if (GetTweakBool("/user/cup_cheat", false))
        {
            GameSceneManager::Instance()->Push(SCENE_CUP_CHEATER, SCREEN_FORWARD, true);
        }
        else
        {
            FrontEndPresentation::GetInstance()->Call("TransitionCupToChooseSides");
            GameSceneManager::Instance()->Pop();
        }
    }
    SHNavigation* navigation = GetNavigationScene();
    if (navigation)
    {
        navigation->HideButtons();
    }
}

void HandleCupBack(int fromSubPage)
{
    if (!fromSubPage)
    {
        bool saveEnabled = SaveEnabled;
        short roundNumber = CupManager::s_pInstance->GetCurrentRoundNumber();

        if (roundNumber == -5)
        {
            FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push(
                SCENE_POPUP_MENU, SCREEN_NOTHING, false);
            if (saveEnabled)
            {
                popup->Create(POPUP_LEAVE_CUP_SAVE,
                              Function<FnVoidVoid>(ExitCupToMainMenu),
                              Function<FnVoidVoid>(RequestMainMenuInputReset));
            }
            else
            {
                popup->Create(POPUP_LEAVE_CUP_QUIT,
                              Function<FnVoidVoid>(ExitCupToMainMenu),
                              Function<FnVoidVoid>(RequestMainMenuInputReset));
            }
        }
        else
        {
            FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push(
                SCENE_POPUP_MENU, SCREEN_NOTHING, false);
            if (saveEnabled)
            {
                popup->Create(POPUP_LEAVE_CUP_SAVE_OR_RESTART,
                              Function<FnVoidVoid>(ExitCupToMainMenu),
                              Function<FnVoidVoid>(ShowCupSavePrompt),
                              Function<FnVoidVoid>(RequestMainMenuInputReset));
            }
            else
            {
                popup->Create(POPUP_LEAVE_CUP_QUIT_OR_RESTART,
                              Function<FnVoidVoid>(ExitCupToMainMenu),
                              Function<FnVoidVoid>(ShowCupSavePrompt),
                              Function<FnVoidVoid>(RequestMainMenuInputReset));
            }
        }
    }
    else
    {
        GameSceneManager::Instance()->Push(SCENE_ROAD_TO_STRIKERS_CUP, SCREEN_BACK, true);
    }
}

void UpdateCupBreadcrumbs(int currentPage)
{
    int currentIndex = 0;
    int pageCount = 0;
    const int* pages = 0;
    SHNavigation* navigation = GetNavigationScene();
    TLComponentInstance* breadcrumbs = navigation->GetButton(NAVIGATION_BUTTON_BREADCRUMBS);

    if (currentPage == CUP_PAGE_NONE)
    {
        breadcrumbs->m_bVisible = false;
    }
    else
    {
        if ((unsigned int)(currentPage - CUP_PAGE_STANDINGS) <= 2)
        {
            pageCount = 3;
            pages = sCupPageOrder;
        }
        else
        {
            int roundType = CupManager::s_pInstance->mState == CUP_STATE_NOT_QUALIFIED
                                ? 0
                                : CupManager::s_pInstance->GetCurrentRoundType();
            switch (roundType)
            {
            case CUP_ROUND_LEAGUE:
                navigation->SetButtonVisibility(NAVIGATION_BUTTON_BREADCRUMBS, false);
                breadcrumbs->m_bVisible = false;
                return;
            case CUP_ROUND_KNOCKOUT:
                pageCount = 2;
                pages = sCupRoundPageOrderTwo;
                break;
            case CUP_ROUND_FINALS:
                pageCount = 3;
                pages = sCupRoundPageOrderThree;
                break;
            }
        }

        for (int i = 0; i < pageCount; ++i)
        {
            if (currentPage == pages[i])
            {
                currentIndex = i;
                break;
            }
        }
        SetBreadcrumbs(pageCount, currentIndex);
    }
}

void ExitCupToMainMenu()
{
    FEAudio::PlayAnimAudioEvent(0x4430B152, 0, 0, true);
    GameSceneManager::Instance()->Pop();
    FrontEndPresentation::GetInstance()->Call("TransitionStrikerCupToMainMenu");
}

void ShowCupStartOptions()
{
    gMainMenuInputResetPending = false;
    FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push(
        SCENE_POPUP_MENU, SCREEN_NOTHING, false);
    popup->Create(POPUP_CONTINUE_OR_NEW_CUP,
                  Function<FnVoidVoid>(ContinueStrikerCup),
                  Function<FnVoidVoid>(ShowNewCupPrompt),
                  Function<FnVoidVoid>(RequestMainMenuInputReset));
}

void ShowNewCupPrompt()
{
    FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push(
        SCENE_POPUP_MENU, SCREEN_NOTHING, false);
    popup->Create(POPUP_CONFIRM_NEW_CUP,
                  Function<FnVoidVoid>(StartNewCup),
                  Function<FnVoidVoid>(ShowCupStartOptions));
}

void ShowCupSavePrompt()
{
    FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push(
        SCENE_POPUP_MENU, SCREEN_NOTHING, false);
    popup->Create(POPUP_RESTART_CUP,
                  Function<FnVoidVoid>(SaveAndShowCupHub),
                  Function<FnVoidVoid>(ShowCupExitPopup));
}

void ShowCupExitPopup()
{
    HandleCupBack(false);
}

void RequestMainMenuInputReset()
{
    gMainMenuInputResetPending = true;
}

void StartNewCup()
{
    SHNavigation* navigation = GetNavigationScene();
    if (navigation)
    {
        navigation->SetButtons(NAVIGATION_BUTTON_NONE, true);
    }
    for (int i = 0; i < 4; ++i)
    {
        GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
    }

    CupManager::s_pInstance->mState = CUP_STATE_NONE;
    CupManager::s_pInstance->ResetCupRecord();
    CupManager::s_pInstance->SetMode(CUP_NONE);
    CupManager::s_pInstance->mGameInProgress = false;
    GameSceneManager::Instance()->Pop();
    SaveLoad::StartSave(false);
    FEAudio::PlayAnimAudioEvent(0x5854D494, 0, 0, true);
    FrontEndPresentation::GetInstance()->Call("TransitionFromMainMenu");
    gNextFETransition = "TransitionMainMenuToNewStrikerCup";
}

void ContinueStrikerCup()
{
    GameSceneManager::Instance()->Pop();
    FEAudio::PlayAnimAudioEvent(0xB19DBC20, 0, 0, true);
    CupManager::s_pInstance->UpdateCurrentCup();
    BasicGameInfo* currentGame = CupManager::s_pInstance->GetCurrentGameInfo();
    GameInfoManager* gameInfo = GameInfoManager::Instance();
    gameInfo->mGameInfo[gameInfo->mCurrentMode] = currentGame;

    SHNavigation* navigation = GetNavigationScene();
    if (navigation)
    {
        navigation->SetButtons(NAVIGATION_BUTTON_NONE, true);
    }
    if (CupManager::s_pInstance->mGameInProgress)
    {
        SavePreGameUnlockState();
        CupManager::s_pInstance->ForfeitCurrentGame();
        CupManager::s_pInstance->mGameInProgress = false;
        CupManager::s_pInstance->AwardGoalTrophies();
        SaveLoad::StartSave(false);
        FrontEndPresentation::GetInstance()->Call("TransitionFromMainMenu");
        gNextFETransition = "TransitionMainMenuToContinueStrikerCupForfeit";
    }
    else
    {
        FrontEndPresentation::GetInstance()->Call("TransitionFromMainMenu");
        gNextFETransition = "TransitionMainMenuToContinueStrikerCup";
    }
    for (int i = 0; i < 4; ++i)
    {
        GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
    }
}

void SaveAndShowCupHub()
{
    FEMusic::StartStreamIfDifferent(9);
    CupManager::s_pInstance->RestoreCupRecord();
    CupManager::s_pInstance->RestartCupSeries();
    GameSceneManager::Instance()->Push(SCENE_ROAD_TO_STRIKERS_CUP, SCREEN_NOTHING, true);
    SaveLoad::StartSave(false);
}

const char* GetCupTeamSlide(int teamType)
{
    return "user";
}

void ShowCupRulesPopup()
{
    if (GetTweakBool("Rendering/Misc/Screenshot Mode", false))
    {
        return;
    }

    FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push(
        SCENE_POPUP_MENU, SCREEN_NOTHING, false);
    int cupMode = CupManager::s_pInstance->GetCurrentMode();
    int roundType = CupManager::s_pInstance->GetCurrentRoundType();
    int menuType = -1;
    if (roundType == CUP_ROUND_LEAGUE && cupMode == 0)
    {
        menuType = POPUP_FIRE_QUAL_RULES;
    }
    else if (roundType == CUP_ROUND_LEAGUE && cupMode == 1)
    {
        menuType = POPUP_CRYSTAL_QUAL_RULES;
    }
    else if (roundType == CUP_ROUND_LEAGUE && cupMode == 2)
    {
        menuType = POPUP_STRIKER_QUAL_RULES;
    }
    else if (roundType == CUP_ROUND_KNOCKOUT && cupMode == 0)
    {
        menuType = POPUP_FIRE_ELIM_RULES;
    }
    else if (roundType == CUP_ROUND_KNOCKOUT && cupMode == 1)
    {
        menuType = POPUP_CRYSTAL_ELIM_RULES;
    }
    else if (roundType == CUP_ROUND_KNOCKOUT && cupMode == 2)
    {
        menuType = POPUP_STRIKER_ELIM_RULES;
    }
    else if (roundType == CUP_ROUND_FINALS && cupMode == 0)
    {
        menuType = POPUP_FIRE_FINAL_RULES;
    }
    else if (roundType == CUP_ROUND_FINALS && cupMode == 1)
    {
        menuType = POPUP_CRYSTAL_FINAL_RULES;
    }
    else if (roundType == CUP_ROUND_FINALS && cupMode == 2)
    {
        menuType = POPUP_STRIKER_FINAL_RULES;
    }
    popup->Create((ePopupMenu)menuType,
                  Function<FnVoidVoid>(FEPopupMenu::Nothing));
}

void ShowCupHub()
{
    GameSceneManager::Instance()->Push(SCENE_ROAD_TO_STRIKERS_CUP, SCREEN_NOTHING, false);
}

void BeginCupAwardPresentation()
{
    int firstStatistic = 0;
    int secondStatistic = 0;
    int firstTeam = CupManager::s_pInstance->GetGoalsForLeader(&firstStatistic);
    int secondTeam = CupManager::s_pInstance->GetGoalsAgainstLeader(&secondStatistic);
    int userTeam = CupManager::s_pInstance->GetUserSelectedCupTeam();

    if (secondTeam == userTeam)
    {
        FrontEndPresentation::GetInstance()->Call("TransitionCupToLeftAward");
    }
    else if (firstTeam == userTeam)
    {
        FrontEndPresentation::GetInstance()->Call("TransitionCupToRightAward");
    }
    else
    {
        GameSceneManager::Instance()->Push(SCENE_ROAD_TO_STRIKERS_CUP, SCREEN_NOTHING, false);
    }
}

void ShowCupBrickWallNews()
{
    CupNewsScene* scene = (CupNewsScene*)GameSceneManager::Instance()->Push(
        SCENE_CUP_NEWS, SCREEN_NOTHING, false);
    scene->SetDisplayMode(NEWS_BRICK_WALL);
}

void AdvanceCupAwardPresentation()
{
    int statistic = 0;
    int team = CupManager::s_pInstance->GetGoalsForLeader(&statistic);
    if (team == CupManager::s_pInstance->GetUserSelectedCupTeam())
    {
        FrontEndPresentation::GetInstance()->Call("TransitionCupLeftToRightAward");
    }
    else
    {
        FrontEndPresentation::GetInstance()->Call("TransitionCupToCentreAward");
    }
}

void ShowCupGoldenBootNews()
{
    CupNewsScene* scene = (CupNewsScene*)GameSceneManager::Instance()->Push(
        SCENE_CUP_NEWS, SCREEN_NOTHING, false);
    scene->SetDisplayMode(NEWS_GOLDEN_BOOT);
}

void FinishCupAwardPresentation()
{
    FrontEndPresentation::GetInstance()->Call("TransitionCupToCentreAward");
}

void ShowCupAwardRewardsPopup()
{
    bool showRewards = false;
    int menuType = -1;
    switch (CupManager::s_pInstance->GetCurrentMode())
    {
    case CUP_FIRE:
        showRewards = HasWastelandsUnlockFlags() && WasWastelandsLockedBeforeGame();
        menuType = POPUP_WIN_FIRECUP_BOOT_WALL_REWARDS;
        break;
    case CUP_CRYSTAL:
        showRewards = HasDumpUnlockFlags() && WasDumpLockedBeforeGame();
        menuType = POPUP_WIN_CRYSTALCUP_BOOT_WALL_REWARDS;
        break;
    case CUP_STRIKER:
        showRewards = HasGalacticStadiumUnlockFlags() && WasGalacticStadiumLockedBeforeGame();
        menuType = POPUP_WIN_STRIKERCUP_BOOT_WALL_REWARDS;
        break;
    default:
        showRewards = false;
        break;
    }

    if (showRewards)
    {
        FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push(
            SCENE_POPUP_MENU, SCREEN_NOTHING, false);
        popup->Create((ePopupMenu)menuType, Function<FnVoidVoid>(ShowCupHub));
    }
    else
    {
        GameSceneManager::Instance()->Push(SCENE_ROAD_TO_STRIKERS_CUP, SCREEN_NOTHING, false);
    }
}

void ShowCupTrophyRewardsPopup()
{
    bool showRewards = false;
    int menuType = -1;
    switch (CupManager::s_pInstance->GetCurrentMode())
    {
    case CUP_FIRE:
        showRewards = IsUnlockFlagSet(1) && WereUnlockFlagsClearBeforeGame(1);
        menuType = POPUP_WIN_FIRECUP_MAINCUP_REWARDS;
        break;
    case CUP_CRYSTAL:
        showRewards = IsUnlockFlagSet(2) && WereUnlockFlagsClearBeforeGame(2);
        menuType = POPUP_WIN_CRYSTALCUP_MAINCUP_REWARDS;
        break;
    case CUP_STRIKER:
        showRewards = IsUnlockFlagSet(4) && WereUnlockFlagsClearBeforeGame(4);
        menuType = POPUP_WIN_STRIKERCUP_MAINCUP_REWARDS;
        break;
    default:
        showRewards = false;
        break;
    }

    if (showRewards)
    {
        FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push(
            SCENE_POPUP_MENU, SCREEN_NOTHING, false);
        popup->Create((ePopupMenu)menuType, Function<FnVoidVoid>(ShowCupHub));
    }
    else
    {
        GameSceneManager::Instance()->Push(SCENE_ROAD_TO_STRIKERS_CUP, SCREEN_NOTHING, false);
    }
}

void UpdatePlayButtonText()
{
    CupManager* cupManager = CupManager::s_pInstance;
    SHNavigation* navigation = GetNavigationScene();
    if (navigation && cupManager->GetCurrentRoundNumber() == -5)
    {
        if (cupManager->mState == CUP_STATE_WON)
        {
            navigation->SetPlayButtonText(NAV_CONTINUE);
        }
        else
        {
            navigation->SetPlayButtonText(NAV_RESTART_CUP);
        }
    }
    else
    {
        navigation->SetPlayButtonText(NAV_PLAY_READY);
    }
}

void UpdateCupTitleText(TLComponentInstance* component, unsigned short* buffer, unsigned long capacity)
{
    TLTextInstance* title = FEFinder<TLTextInstance, 3>::Find<>(
        component->GetActiveSlide(), "TITLE");
    if (title == 0)
    {
        return;
    }

    const unsigned short* oldTitle = title->GetString();
    WideString formatted;
    switch (CupManager::s_pInstance->GetCurrentMode())
    {
    case CUP_FIRE:
    {
        formatted = Format(WideString(oldTitle),
                           g_pLocalization->GetString("FIRE_CUP"));
        break;
    }
    case CUP_CRYSTAL:
    {
        formatted = Format(WideString(oldTitle),
                           g_pLocalization->GetString("CRYSTAL_CUP"));
        break;
    }
    case CUP_STRIKER:
    {
        formatted = Format(WideString(oldTitle),
                           g_pLocalization->GetString("STRIKER_CUP"));
        break;
    }
    }

    nlStrNCpy(buffer, formatted.c_str(), capacity);
    title->SetString(buffer);

    TLTextInstance* second = FEFinder<TLTextInstance, 3>::FindOrDefault(
        component->GetActiveSlide(), "TITLE2");
    second->SetString(buffer);
    TLTextInstance* third = FEFinder<TLTextInstance, 3>::FindOrDefault(
        component->GetActiveSlide(), "TITLE3");
    third->SetString(buffer);
}

void RegisterCupTrophy(StadiumCupTrophyDrawable* object)
{
    if (gCupAwardModelCount == 9)
    {
        gCupAwardModelCount = 0;
    }
    gCupAwardModels[gCupAwardModelCount] = object;
    ++gCupAwardModelCount;
}

void SetCupTrophiesVisible(bool visible)
{
    for (int i = 0; i < gCupAwardModelCount; ++i)
    {
        float opacity = visible ? 1.0f : 0.0f;
        gCupAwardModels[i]->SetOpacity(opacity);
    }
}

void SetLockedTrophyVisibility(bool visible)
{
    for (int i = 0; i < gCupAwardModelCount; ++i)
    {
        unsigned int flag = 0x200000;
        for (int j = 0; j < gCupAwardModelCount; ++j)
        {
            if (gCupTrophyUnlockFlags[j].key == gCupAwardModels[i]->m_uCupTrophyKey)
            {
                flag = gCupTrophyUnlockFlags[j].flag;
                break;
            }
        }

        if (IsUnlockFlagSet(flag))
        {
            gCupAwardModels[i]->SetOpacity(1.0f);
        }
        else
        {
            float opacity = visible ? 1.0f : 0.0f;
            gCupAwardModels[i]->SetOpacity(opacity);
        }
    }
}

void ResetCupFlow()
{
    CupManager* cupManager = CupManager::s_pInstance;
    cupManager->mState = CUP_STATE_NONE;
    cupManager->ResetCupRecord();
    cupManager->SetMode(CUP_NONE);
    SaveLoad::StartSave(false);
    SHNavigation* navigation = GetNavigationScene();
    if (navigation)
    {
        navigation->SetButtons(NAVIGATION_BUTTON_NONE, true);
    }
    FrontEndPresentation::GetInstance()->Call("TransitionCupToChooseNewCaptain");
}
