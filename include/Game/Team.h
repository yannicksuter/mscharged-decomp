#ifndef GAME_TEAM_H
#define GAME_TEAM_H

#include "types.h"
#include "Game/AI/Powerups.h"

class cNet;
class cFielder;
class cPlayer;
class FormationManager;

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
    /* 0x08 */ u8 mUnidentified008[0x10];

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
