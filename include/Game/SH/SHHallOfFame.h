#ifndef GAME_SH_HALL_OF_FAME_H
#define GAME_SH_HALL_OF_FAME_H

class TLComponentInstance;

void StopHallOfFameTrophyEffects();
void SetHallOfFameBreadcrumbs(int mode, TLComponentInstance* breadcrumbs);
void CycleHallOfFameCup(int mode, bool advance);
void ShowHallOfFameTrophy(int camera);
void ShowHallOfFamePlayerCard(int camera);
void CycleHallOfFameDetailPage(int mode, bool advance);
void LeaveHallOfFamePage(int mode);
unsigned int GetHallOfFameUnlockFlag(int mode, int item);
unsigned int GetHallOfFameTrophyID(int mode, int item);
int GetHallOfFamePlayerCardIndex();
unsigned int GetHallOfFamePlayerUnlockFlag(unsigned int camera);
void PreloadHallOfFameImages();
void UpdateHallOfFameImagePreload(float fDeltaT);
void ResetHallOfFameImagePreload();
bool IsHallOfFameImagePreloadStarted();
bool IsHallOfFameImagePreloadPending();

#endif // GAME_SH_HALL_OF_FAME_H
