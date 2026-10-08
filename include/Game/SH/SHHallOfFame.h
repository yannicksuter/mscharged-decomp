#ifndef GAME_SH_HALL_OF_FAME_H
#define GAME_SH_HALL_OF_FAME_H

class TLComponentInstance;

enum eHallOfFameMode
{
    HOF_FIRE_CUP = 0,
    HOF_STRIKER_CUP = 1,
    HOF_CRYSTAL_CUP = 2,
    HOF_FIRE_CUP_HISTORY = 4,
    HOF_CRYSTAL_CUP_HISTORY = 5,
    HOF_STRIKER_CUP_HISTORY = 6,
    HOF_FIRE_BRICK_WALL_HISTORY = 7,
    HOF_FIRE_GOLDEN_BOOT_HISTORY = 8,
    HOF_CRYSTAL_BRICK_WALL_HISTORY = 9,
    HOF_CRYSTAL_GOLDEN_BOOT_HISTORY = 10,
    HOF_STRIKER_BRICK_WALL_HISTORY = 11,
    HOF_STRIKER_GOLDEN_BOOT_HISTORY = 12,
    HOF_PLAYER_CARD = 13,
    HOF_TROPHY_SUMMARY = 14,
    HOF_UNLOCK_SUMMARY = 15,
    HOF_CHALLENGE_SUMMARY = 16,
};

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
