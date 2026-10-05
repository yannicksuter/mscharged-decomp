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
