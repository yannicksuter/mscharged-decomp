#ifndef GAME_TEAM_H
#define GAME_TEAM_H

#include "types.h"
#include "Game/AI/Powerups.h"
#include "NL/nlMath.h"
#include "NL/nlTimer.h"

class cNet;
class cFielder;
class cPlayer;
class cGlobalPad;
class Goalie;
class FormationManager;
class UnidentifiedFielderInput;

enum eTeamSide
{
    NO_SIDE = -1,
    HOME = 0,
    AWAY = 1,
    HOME_AWAY = 2,
};

enum eSituation
{
    SITUATION_OFFENSE = 0,
    SITUATION_DEFENSE = 1,
    SITUATION_LOOSE = 2,
    NUM_SITUATIONS = 3,
};

enum eTeamStyle
{
    TEAM_STYLE_AGGRESSIVE = 0,
    TEAM_STYLE_MODERATE = 1,
    TEAM_STYLE_PASSIVE = 2,
    NUM_TEAM_STYLES = 3,
};

class DebugWriteCache;
class RunningChecksum;

class cTeam
{
public:
    cTeam(int nSide);
    ~cTeam();
    void ClearAllPowerUps();
    void ClearCurrentPowerUp();
    bool fn_800A6560();
    bool TogglePowerup(bool bIsSilent);
    bool IncrementPowerupMeter(
        float fAdjustAmount, cFielder* pFielder, bool param3);
    bool fn_800A6764() const;
    PowerUpTeamType GetCurrentPowerUp() const;
    void SetIsPowerUpNew(int index, bool isNew);
    void SetPlayer(cPlayer* pPlayer, int nIndex);
    void SetGoalie(Goalie* pGoalie);
    cFielder* GetFielder(int nIndex);
    cFielder* GetBallInterceptFielder(int i) { return m_pBallInterceptOrderedFielders[i]; }
    cPlayer* GetPlayer(int nIndex);
    cPlayer* GetControlledPlayer(cGlobalPad* pController);
    cFielder* GetCaptain();
    cFielder* GetStriker() const;
    cFielder* GetFrontMostFielder();
    cFielder* GetRearMostFielder();
    cTeam* GetOtherTeam();
    Goalie* GetGoalie();
    cNet* GetOtherNet();
    nlVector3 GetAIOffNetLocation(const nlVector3* v3ReferencePos);
    nlVector3 GetAIDefNetLocation(const nlVector3* v3ReferencePos);
    int GetNumAssignedControllers();
    void PreUpdate(float fDeltaT);
    void Update(float fDeltaT);
    void UpdateTeamAI(float fDeltaT);
    bool AssignSituation();
    void AssignMarks(bool bForceReMark);
    void UpdateControllers();
    void ResetCharacters();
    void StopGameplayEffectsAndSounds();
    bool CalculateFormationPosition(nlVector3& v3DestPosition,
        cFielder* pFielder, bool bInPosition,
        float fBallPosFormationWeight);
    void CalculateNewBallInterceptTimes();
    PowerUpTeamType GetPowerUpByIndex(int index) const;
    int SetCurrentPowerUp(
        ePowerUpType eNewPowerUpType, int nnumOfPowerups);
    void SetDifficulty(int difficulty, int param2, bool param3);
    void fn_800A607C();
    void fn_800A6248();
    void fn_800A7998();
    void fn_800A8098();
    void fn_800A8900(void* context, DebugWriteCache* cache);
    void fn_800A8DE8(RunningChecksum* runningChecksum);
    float fn_800A8EC0();
    float fn_800A8F20();
    float fn_800A8F80();
    float fn_800A8FE0();

public:
    /* 0x00 */ int m_nSide;
    /* 0x04 */ int m_nScore;

public:
    /* 0x08 */ float mfPowerupMeter;

    /* 0x0C */ float mUnidentified00C;

private:
    /* 0x10 */ float mUnidentified010;

public:
    /* 0x14 */ float mfPowerupTimer;
    /* 0x18 */ eSituation mpCurrentSituation;
    /* 0x1C */ eTeamStyle meCurrentTeamStyle;

public:
    /* 0x20 */ Timer mtTeamStyleTimer;
    /* 0x28 */ Timer mtMarkTimer;
    /* 0x30 */ Timer mtRoleTimer;
    /* 0x38 */ Timer mtDefensiveZoneTimer;
    /* 0x40 */ Timer mtToggleTimer;
    /* 0x48 */ float mfBallInTimes[4];

private:
    /* 0x58 */ nlVector3 mvBallInterceptPosition[4];

public:
    /* 0x88 */ cFielder* mpBestBallInterceptor;
    /* 0x8C */ PowerUpTeamType m_ePowerupList[2];
    /* 0xA4 */ cPlayer* m_pPlayers[5];
    /* 0xB8 */ cFielder* m_pAIOrderedFielders[4];
    /* 0xC8 */ cFielder* m_pBallInterceptOrderedFielders[4];
    /* 0xD8 */ cFielder* mUnidentified0D8[4];
    /* 0xE8 */ cNet* m_pNet;
    /* 0xEC */ FormationManager* m_pFormationManager;
    /* 0xF0 */ UnidentifiedFielderInput* mUnidentified0F0;
    /* 0xF4 */ u32 mUnidentified0F4;
};

extern cTeam* g_pTeams[];

class SkillTweaks;
SkillTweaks* fn_800A636C(cTeam* pTeam);
float fn_800A6388(cTeam*);

#endif // GAME_TEAM_H
