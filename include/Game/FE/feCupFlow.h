#ifndef GAME_FE_CUP_FLOW_H
#define GAME_FE_CUP_FLOW_H

extern bool gMainMenuInputResetPending;

void ShowCupStartOptions();
void StartNewCup();
void SetLockedTrophyVisibility(bool visible);

void CycleCupPage(int currentPage, bool advance);
void CycleCupRoundPage(int currentPage, bool advance);
void ShowFirstCupPage();
void ShowCurrentCupRoundPage();
extern "C" void fn_80207B8C();
void RequestMainMenuInputReset();
void SaveAndShowCupHub();
const char* GetCupWaitingSlide();
void ShowCupHub();

#endif // GAME_FE_CUP_FLOW_H
