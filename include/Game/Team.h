#ifndef GAME_TEAM_H
#define GAME_TEAM_H

#include "types.h"
#include "Game/AI/Powerups.h"

class cNet;
class cFielder;
class cPlayer;

class cTeam
{
public:
    cFielder* GetFielder(int nIndex);
    cPlayer* GetPlayer(int nIndex);
    cFielder* GetCaptain();
    cTeam* GetOtherTeam();
    cNet* GetOtherNet();
    PowerUpTeamType GetPowerUpByIndex(int index) const;
    bool SetCurrentPowerUp(
        ePowerUpType eNewPowerUpType, int nnumOfPowerups);

public:
    /* 0x00 */ int m_nSide;
    /* 0x04 */ int m_nScore;

private:
    /* 0x08 */ u8 mUnidentified008[0xE0];

public:
    /* 0xE8 */ cNet* m_pNet;
};

extern cTeam* g_pTeams[];

#endif // GAME_TEAM_H
