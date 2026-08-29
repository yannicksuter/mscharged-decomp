#ifndef GAME_TEAM_H
#define GAME_TEAM_H

#include "types.h"
#include "Game/AI/Powerups.h"

class cNet;
class cFielder;
class cPlayer;
class Goalie;
class FormationManager;

enum eTeamSide
{
    NO_SIDE = -1,
    HOME = 0,
    AWAY = 1,
    HOME_AWAY = 2,
};

class cTeam
{
public:
    void ClearAllPowerUps();
    void ClearCurrentPowerUp();
    cFielder* GetFielder(int nIndex);
    cPlayer* GetPlayer(int nIndex);
    cFielder* GetCaptain();
    cTeam* GetOtherTeam();
    Goalie* GetGoalie();
    cNet* GetOtherNet();
    void PreUpdate(float fDeltaT);
    PowerUpTeamType GetPowerUpByIndex(int index) const;
    bool SetCurrentPowerUp(
        ePowerUpType eNewPowerUpType, int nnumOfPowerups);
    void SetDifficulty(int difficulty, int param2, bool param3);
    void fn_800A607C();

public:
    /* 0x00 */ int m_nSide;
    /* 0x04 */ int m_nScore;

public:
    /* 0x08 */ float mfPowerupMeter;

private:
    /* 0x0C */ u8 mUnidentified00C[0x0C];

public:
    /* 0x18 */ u32 field_0x18;

private:
    /* 0x1C */ u8 mUnidentified01C[0x6C];

public:
    /* 0x88 */ cFielder* m_pBallInterceptOrderedFielders[4];

private:
    /* 0x98 */ u8 mUnidentified098[0x40];

public:
    /* 0xD8 */ cFielder* mUnidentified0D8[4];
    /* 0xE8 */ cNet* m_pNet;
    /* 0xEC */ FormationManager* m_pFormationManager;
};

extern cTeam* g_pTeams[];

#endif // GAME_TEAM_H
