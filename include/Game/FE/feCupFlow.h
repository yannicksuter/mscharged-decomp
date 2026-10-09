#ifndef GAME_SH_CUP_SCENE_HELPERS_H
#define GAME_SH_CUP_SCENE_HELPERS_H

enum eCupPage
{
    CUP_PAGE_NONE = 0,
    CUP_PAGE_SCHEDULE = 1,
    CUP_PAGE_KNOCKOUT = 2,
    CUP_PAGE_FINAL_ROUNDS = 3,
    CUP_PAGE_STANDINGS = 4,
    CUP_PAGE_GOLDEN_BOOT = 5,
    CUP_PAGE_BRICK_WALL = 6,
};

class TLComponentInstance;
class StadiumCupTrophyDrawable;

void CycleCupPage(int currentPage, bool advance);
void CycleCupRoundPage(int currentPage, bool advance);
void ShowFirstCupPage();
void ShowCurrentCupRoundPage();
void HandleCupBack(int page);
void ShowCupExitPopup();
void RequestMainMenuInputReset();
void SaveAndShowCupHub();
const char* GetCupTeamSlide(int teamType);
void ShowCupRulesPopup();
void ShowCupHub();
void BeginCupAwardPresentation();
void ShowCupBrickWallNews();
void AdvanceCupAwardPresentation();
void ShowCupGoldenBootNews();
void FinishCupAwardPresentation();
void ShowCupAwardRewardsPopup();
void ShowCupTrophyRewardsPopup();
void RegisterCupTrophy(StadiumCupTrophyDrawable* trophy);
void SetCupTrophiesVisible(bool visible);
void SetLockedTrophyVisibility(bool visible);

extern bool gMainMenuInputResetPending;

void ShowCupStartOptions();

void StartNewCup();

void AdvanceCupFlow(bool pad);
void UpdateCupBreadcrumbs(int currentPage);
void ExitCupToMainMenu();
void ShowNewCupPrompt();
void ShowCupSavePrompt();
void ContinueStrikerCup();
void UpdatePlayButtonText();
void UpdateCupTitleText(TLComponentInstance* component, unsigned short* buffer, unsigned long capacity);
void ResetCupFlow();

#endif // GAME_SH_CUP_SCENE_HELPERS_H
