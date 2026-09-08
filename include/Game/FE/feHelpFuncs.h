#ifndef GAME_FE_FEHELPFUNCS_H
#define GAME_FE_FEHELPFUNCS_H

#include "Game/Character.h"
#include "Game/DB/BasicGameInfo.h"

class TLTextInstance;
class TLComponentInstance;
class FEPointerButton;
class FEModelHandle;

const char* GetLOCCharacterName(eTeamID teamid);
const char* GetLOCTeamName(eTeamID teamID);
eCharacterClass ConvertToCharacterClass(eTeamID teamID);
eCharacterClass ConvertToCharacterClass(eSidekickID sidekickID);
const char* GetTeamName(eTeamID teamID);
const char* GetSidekickName(eSidekickID sidekickID);
eTeamID ConvertToTeamID(const char* name);
eSidekickID ConvertToSidekickID(const char* name);
eStadiumID ConvertToStadiumID(const char* name);
const char* GetLOCModeName(int mode);

class FECharacterSound
{
public:
    static unsigned long GetCaptainAcceptSound(eTeamID teamID);
    static unsigned long GetSidekickAcceptSound(eSidekickID sidekickID);
};

void ResetCharacterIdleAnimation(FEModelHandle* model);
void UpdateCharacterIdleAnimations(float dt);

const char* GetLOCEnvironmentCheatName(int cheat);
const char* GetLOCEnvironmentCheatDescription(int cheat);
const char* GetLOCPlayerCheatName(int cheat);
const char* GetLOCPlayerCheatDescription(int cheat);
const char* GetLOCPowerupCheatName(int cheat);
const char* GetLOCPowerupCheatDescription(int cheat);
bool IsEnvironmentCheatUnlocked(int cheat);
bool IsPlayerCheatUnlocked(int cheat);
bool IsPowerupCheatUnlocked(int cheat);

void MakeTextBoxReallyWide(TLTextInstance& textInstance);
void EnableAutoPressed();
void SetBreadcrumbs(int numBreadcrumbs, int currentBreadcrumb);
void SetPlayButtonBounds(FEPointerButton* component, TLComponentInstance* instance);
void SetDoneButtonBounds(FEPointerButton* component, TLComponentInstance* instance, int value);

namespace TakeGameMemSnapshot
{
extern unsigned char gTakenSnapshot;
extern float gTimeElapsed;

void WriteToDisk();
void ResetTimers();
void Update(float dt);
} // namespace TakeGameMemSnapshot

#endif // GAME_FE_FEHELPFUNCS_H
