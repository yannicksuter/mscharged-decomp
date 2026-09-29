#ifndef GAME_SH_CUP_SCENE_HELPERS_H
#define GAME_SH_CUP_SCENE_HELPERS_H

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


// Shared functions and data from Game/FE/feCupFlow.cpp.
extern "C" void AdvanceCupFlow(bool);
extern "C" void UpdateCupBreadcrumbs(int);
extern "C" void ExitCupToMainMenu();
extern "C" void ShowNewCupPrompt();
extern "C" void ShowCupSavePrompt();
extern "C" void ContinueStrikerCup();
extern "C" void UpdatePlayButtonText();
extern "C" void UpdateCupTitleText(TLComponentInstance*, unsigned short*, unsigned long);
extern "C" void ResetCupFlow();

#endif // GAME_SH_CUP_SCENE_HELPERS_H
