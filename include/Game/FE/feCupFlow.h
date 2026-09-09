#ifndef GAME_SH_CUP_SCENE_HELPERS_H
#define GAME_SH_CUP_SCENE_HELPERS_H

void CycleCupPage(int currentPage, bool advance);
void CycleCupRoundPage(int currentPage, bool advance);
void ShowFirstCupPage();
void ShowCurrentCupRoundPage();
void HandleCupBack(bool fromSubPage);
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
void SetCupTrophiesVisible(bool visible);
void SetLockedTrophyVisibility(bool visible);

extern bool gMainMenuInputResetPending;

void ShowCupStartOptions();

void StartNewCup();

#endif // GAME_SH_CUP_SCENE_HELPERS_H
