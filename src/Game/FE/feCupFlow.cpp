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

bool gMainMenuInputResetPending;
int gCupAwardModelCount;
StadiumCupTrophyDrawable* gCupAwardModels[10];
unsigned int gFEControllerIndex;
bool gFEPointerEnabled[4];

extern const int sCupPageOrder[3] = { 4, 5, 6 };
extern const int sCupRoundPageOrderThree[3] = { 3, 2, 1 };
extern const int sCupRoundPageOrderTwo[2] = { 2, 1 };

struct CupTrophyUnlock_8051B770
{
    unsigned long key;
    unsigned int flag;
};

typedef BasicString<unsigned short, Detail::TempStringAllocator> WideString;


CupTrophyUnlock_8051B770 lbl_8051B770[] = {
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

static void fn_80206CF8(int currentPage, bool advance);

void CycleCupPage(int currentPage, bool advance)
{
    if ((unsigned int)(currentPage - 4) <= 2)
    {
        fn_80206CF8(currentPage, advance);
    }
    else
    {
        CycleCupRoundPage(currentPage, advance);
    }
}

static void fn_80206CF8(int currentPage, bool advance)
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
    case 4:
        GameSceneManager::Instance()->Push((SceneList)36, SCREEN_NOTHING, true);
        break;
    case 5:
        GameSceneManager::Instance()->Push((SceneList)37, SCREEN_NOTHING, true);
        break;
    case 6:
        GameSceneManager::Instance()->Push((SceneList)38, SCREEN_NOTHING, true);
        break;
    }
}

void CycleCupRoundPage(int currentPage, bool advance)
{
    int currentIndex = 0;
    int pageCount = 0;
    const int* pages = 0;

    int roundType = g_pCupManager->mState == 0x10
                      ? 0
                      : g_pCupManager->GetCurrentRoundType();

    if (roundType != 0)
    {
        switch (roundType)
        {
        case 1:
            pageCount = 2;
            pages = sCupRoundPageOrderTwo;
            break;
        case 2:
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
        case 1:
            GameSceneManager::Instance()->Push((SceneList)32, SCREEN_NOTHING, true);
            break;
        case 2:
            GameSceneManager::Instance()->Push((SceneList)34, SCREEN_NOTHING, true);
            break;
        case 3:
            GameSceneManager::Instance()->Push((SceneList)35, SCREEN_NOTHING, true);
            break;
        }
    }
}

void ShowFirstCupPage()
{
    GameSceneManager::Instance()->Push((SceneList)36, SCREEN_NOTHING, true);
}

void ShowCurrentCupRoundPage()
{
    int scene = -2;
    int roundType = g_pCupManager->mState == 0x10
                      ? 0
                      : g_pCupManager->GetCurrentRoundType();

    switch (roundType)
    {
    case 0:
        scene = 32;
        break;
    case 1:
        scene = 34;
        break;
    case 2:
        scene = 35;
        break;
    }

    GameSceneManager::Instance()->Push((SceneList)scene, SCREEN_NOTHING, true);
}

extern "C" void AdvanceCupFlow(bool pad)
{
    CupManager* cupManager = g_pCupManager;
    if (cupManager->GetCurrentRoundNumber() == -5)
    {
        if (cupManager->mState == 4)
        {
            if (cupManager->GetCurrentMode() == 0)
            {
                cupManager->SetMode(1);
                cupManager->mState = -1;
                cupManager->DetermineNextMatchups(19);
                CupNewsScene* scene = (CupNewsScene*)GameSceneManager::Instance()->Push(
                    (SceneList)39, SCREEN_NOTHING, true);
                scene->SetDisplayMode(1);
                SaveLoad::StartSave(false);
            }
            else if (cupManager->GetCurrentMode() == 1)
            {
                cupManager->SetMode(2);
                cupManager->mState = -1;
                cupManager->DetermineNextMatchups(19);
                CupNewsScene* scene = (CupNewsScene*)GameSceneManager::Instance()->Push(
                    (SceneList)39, SCREEN_NOTHING, true);
                scene->SetDisplayMode(1);
                SaveLoad::StartSave(false);
            }
            else
            {
                FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push(
                    (SceneList)10, SCREEN_NOTHING, true);
                popup->Create((ePopupMenu)56, Function<FnVoidVoid>(ResetCupFlow));
            }
        }
        else
        {
            FEMusic::StartStreamIfDifferent(9);
            g_pCupManager->RestoreCupRecord();
            g_pCupManager->RestartCupSeries();
            GameSceneManager::Instance()->Push((SceneList)31, SCREEN_NOTHING, true);
            SaveLoad::StartSave(false);
        }
    }
    else
    {
        CupManager* currentCup = g_pCupManager;
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
            GameSceneManager::Instance()->Push((SceneList)9, SCREEN_FORWARD, true);
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
        short roundNumber = g_pCupManager->GetCurrentRoundNumber();

        if (roundNumber == -5)
        {
            FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push(
                (SceneList)10, SCREEN_NOTHING, false);
            if (saveEnabled)
            {
                popup->Create((ePopupMenu)0,
                              Function<FnVoidVoid>(ExitCupToMainMenu),
                              Function<FnVoidVoid>(RequestMainMenuInputReset));
            }
            else
            {
                popup->Create((ePopupMenu)2,
                              Function<FnVoidVoid>(ExitCupToMainMenu),
                              Function<FnVoidVoid>(RequestMainMenuInputReset));
            }
        }
        else
        {
            FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push(
                (SceneList)10, SCREEN_NOTHING, false);
            if (saveEnabled)
            {
                popup->Create((ePopupMenu)1,
                              Function<FnVoidVoid>(ExitCupToMainMenu),
                              Function<FnVoidVoid>(ShowCupSavePrompt),
                              Function<FnVoidVoid>(RequestMainMenuInputReset));
            }
            else
            {
                popup->Create((ePopupMenu)3,
                              Function<FnVoidVoid>(ExitCupToMainMenu),
                              Function<FnVoidVoid>(ShowCupSavePrompt),
                              Function<FnVoidVoid>(RequestMainMenuInputReset));
            }
        }
    }
    else
    {
        GameSceneManager::Instance()->Push((SceneList)31, SCREEN_BACK, true);
    }
}

extern "C" void UpdateCupBreadcrumbs(int currentPage)
{
    int currentIndex = 0;
    int pageCount = 0;
    const int* pages = 0;
    SHNavigation* navigation = GetNavigationScene();
    TLComponentInstance* breadcrumbs = navigation->GetButton(8);

    if (currentPage == 0)
    {
        breadcrumbs->m_bVisible = false;
    }
    else
    {
        if ((unsigned int)(currentPage - 4) <= 2)
        {
            pageCount = 3;
            pages = sCupPageOrder;
        }
        else
        {
            int roundType = g_pCupManager->mState == 0x10
                                ? 0
                                : g_pCupManager->GetCurrentRoundType();
            switch (roundType)
            {
            case 0:
                navigation->SetButtonVisibility(8, false);
                breadcrumbs->m_bVisible = false;
                return;
            case 1:
                pageCount = 2;
                pages = sCupRoundPageOrderTwo;
                break;
            case 2:
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

extern "C" void ExitCupToMainMenu()
{
    FEAudio::PlayAnimAudioEvent(0x4430B152, 0, 0, true);
    GameSceneManager::Instance()->Pop();
    FrontEndPresentation::GetInstance()->Call("TransitionStrikerCupToMainMenu");
}

void ShowCupStartOptions()
{
    gMainMenuInputResetPending = false;
    FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push(
        (SceneList)10, SCREEN_NOTHING, false);
    popup->Create((ePopupMenu)15,
                  Function<FnVoidVoid>(ContinueStrikerCup),
                  Function<FnVoidVoid>(ShowNewCupPrompt),
                  Function<FnVoidVoid>(RequestMainMenuInputReset));
}

extern "C" void ShowNewCupPrompt()
{
    FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push(
        (SceneList)10, SCREEN_NOTHING, false);
    popup->Create((ePopupMenu)16,
                  Function<FnVoidVoid>(StartNewCup),
                  Function<FnVoidVoid>(ShowCupStartOptions));
}

extern "C" void ShowCupSavePrompt()
{
    FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push(
        (SceneList)10, SCREEN_NOTHING, false);
    popup->Create((ePopupMenu)4,
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
        navigation->SetButtons(0, true);
    }
    for (int i = 0; i < 4; ++i)
    {
        GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
    }

    g_pCupManager->mState = -1;
    g_pCupManager->ResetCupRecord();
    g_pCupManager->SetMode(-1);
    g_pCupManager->mGameInProgress = false;
    GameSceneManager::Instance()->Pop();
    SaveLoad::StartSave(false);
    FEAudio::PlayAnimAudioEvent(0x5854D494, 0, 0, true);
    FrontEndPresentation::GetInstance()->Call("TransitionFromMainMenu");
    gNextFETransition = "TransitionMainMenuToNewStrikerCup";
}

extern "C" void ContinueStrikerCup()
{
    GameSceneManager::Instance()->Pop();
    FEAudio::PlayAnimAudioEvent(0xB19DBC20, 0, 0, true);
    g_pCupManager->UpdateCurrentCup();
    BasicGameInfo* currentGame = g_pCupManager->GetCurrentGameInfo();
    GameInfoManager* gameInfo = GameInfoManager::Instance();
    gameInfo->mGameInfo[gameInfo->mCurrentMode] = currentGame;

    SHNavigation* navigation = GetNavigationScene();
    if (navigation)
    {
        navigation->SetButtons(0, true);
    }
    if (g_pCupManager->mGameInProgress)
    {
        SavePreGameUnlockState();
        g_pCupManager->ForfeitCurrentGame();
        g_pCupManager->mGameInProgress = false;
        g_pCupManager->AwardGoalTrophies();
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
    g_pCupManager->RestoreCupRecord();
    g_pCupManager->RestartCupSeries();
    GameSceneManager::Instance()->Push((SceneList)31, SCREEN_NOTHING, true);
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
        (SceneList)10, SCREEN_NOTHING, false);
    int cupMode = g_pCupManager->GetCurrentMode();
    int roundType = g_pCupManager->GetCurrentRoundType();
    int menuType = -1;
    if (roundType == 0 && cupMode == 0)
    {
        menuType = 29;
    }
    else if (roundType == 0 && cupMode == 1)
    {
        menuType = 30;
    }
    else if (roundType == 0 && cupMode == 2)
    {
        menuType = 31;
    }
    else if (roundType == 1 && cupMode == 0)
    {
        menuType = 32;
    }
    else if (roundType == 1 && cupMode == 1)
    {
        menuType = 33;
    }
    else if (roundType == 1 && cupMode == 2)
    {
        menuType = 34;
    }
    else if (roundType == 2 && cupMode == 0)
    {
        menuType = 35;
    }
    else if (roundType == 2 && cupMode == 1)
    {
        menuType = 36;
    }
    else if (roundType == 2 && cupMode == 2)
    {
        menuType = 37;
    }
    popup->Create((ePopupMenu)menuType,
                  Function<FnVoidVoid>(FEPopupMenu::Nothing));
}

void ShowCupHub()
{
    GameSceneManager::Instance()->Push((SceneList)31, SCREEN_NOTHING, false);
}

void BeginCupAwardPresentation()
{
    int firstStatistic = 0;
    int secondStatistic = 0;
    int firstTeam = g_pCupManager->GetGoalsForLeader(&firstStatistic);
    int secondTeam = g_pCupManager->GetGoalsAgainstLeader(&secondStatistic);
    int userTeam = g_pCupManager->GetUserSelectedCupTeam();

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
        GameSceneManager::Instance()->Push((SceneList)31, SCREEN_NOTHING, false);
    }
}

void ShowCupBrickWallNews()
{
    CupNewsScene* scene = (CupNewsScene*)GameSceneManager::Instance()->Push(
        (SceneList)39, SCREEN_NOTHING, false);
    scene->SetDisplayMode(7);
}

void AdvanceCupAwardPresentation()
{
    int statistic = 0;
    int team = g_pCupManager->GetGoalsForLeader(&statistic);
    if (team == g_pCupManager->GetUserSelectedCupTeam())
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
        (SceneList)39, SCREEN_NOTHING, false);
    scene->SetDisplayMode(6);
}

void FinishCupAwardPresentation()
{
    FrontEndPresentation::GetInstance()->Call("TransitionCupToCentreAward");
}

void ShowCupAwardRewardsPopup()
{
    bool showRewards = false;
    int menuType = -1;
    switch (g_pCupManager->GetCurrentMode())
    {
    case 0:
        showRewards = HasWastelandsUnlockFlags() && WasWastelandsLockedBeforeGame();
        menuType = 41;
        break;
    case 1:
        showRewards = HasDumpUnlockFlags() && WasDumpLockedBeforeGame();
        menuType = 42;
        break;
    case 2:
        showRewards = HasGalacticStadiumUnlockFlags() && WasGalacticStadiumLockedBeforeGame();
        menuType = 43;
        break;
    default:
        showRewards = false;
        break;
    }

    if (showRewards)
    {
        FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push(
            (SceneList)10, SCREEN_NOTHING, false);
        popup->Create((ePopupMenu)menuType, Function<FnVoidVoid>(ShowCupHub));
    }
    else
    {
        GameSceneManager::Instance()->Push((SceneList)31, SCREEN_NOTHING, false);
    }
}

void ShowCupTrophyRewardsPopup()
{
    bool showRewards = false;
    int menuType = -1;
    switch (g_pCupManager->GetCurrentMode())
    {
    case 0:
        showRewards = IsUnlockFlagSet(1) && WereUnlockFlagsClearBeforeGame(1);
        menuType = 44;
        break;
    case 1:
        showRewards = IsUnlockFlagSet(2) && WereUnlockFlagsClearBeforeGame(2);
        menuType = 45;
        break;
    case 2:
        showRewards = IsUnlockFlagSet(4) && WereUnlockFlagsClearBeforeGame(4);
        menuType = 46;
        break;
    default:
        showRewards = false;
        break;
    }

    if (showRewards)
    {
        FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push(
            (SceneList)10, SCREEN_NOTHING, false);
        popup->Create((ePopupMenu)menuType, Function<FnVoidVoid>(ShowCupHub));
    }
    else
    {
        GameSceneManager::Instance()->Push((SceneList)31, SCREEN_NOTHING, false);
    }
}

extern "C" void UpdatePlayButtonText()
{
    CupManager* cupManager = g_pCupManager;
    SHNavigation* navigation = GetNavigationScene();
    if (navigation && cupManager->GetCurrentRoundNumber() == -5)
    {
        if (cupManager->mState == 4)
        {
            navigation->SetPlayButtonText(2);
        }
        else
        {
            navigation->SetPlayButtonText(1);
        }
    }
    else
    {
        navigation->SetPlayButtonText(3);
    }
}

extern "C" void UpdateCupTitleText(TLComponentInstance* component,
                              unsigned short* buffer, unsigned long capacity)
{
    TLTextInstance* title = FEFinder<TLTextInstance, 3>::Find<>(
        component->GetActiveSlide(), "TITLE");
    if (title == 0)
    {
        return;
    }

    const unsigned short* oldTitle = title->GetString();
    WideString formatted;
    switch (g_pCupManager->GetCurrentMode())
    {
    case 0:
    {
        formatted = Format(WideString(oldTitle),
                           g_pLocalization->GetString("FIRE_CUP"));
        break;
    }
    case 1:
    {
        formatted = Format(WideString(oldTitle),
                           g_pLocalization->GetString("CRYSTAL_CUP"));
        break;
    }
    case 2:
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
            if (lbl_8051B770[j].key == gCupAwardModels[i]->m_uCupTrophyKey)
            {
                flag = lbl_8051B770[j].flag;
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

extern "C" void ResetCupFlow()
{
    CupManager* cupManager = g_pCupManager;
    cupManager->mState = -1;
    cupManager->ResetCupRecord();
    cupManager->SetMode(-1);
    SaveLoad::StartSave(false);
    SHNavigation* navigation = GetNavigationScene();
    if (navigation)
    {
        navigation->SetButtons(0, true);
    }
    FrontEndPresentation::GetInstance()->Call("TransitionCupToChooseNewCaptain");
}
