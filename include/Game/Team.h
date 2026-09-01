#ifndef GAME_TEAM_H
#define GAME_TEAM_H

#include "types.h"
#include "Game/AI/Powerups.h"
#include "NL/nlMath.h"

class cNet;
class cFielder;
class cPlayer;
class cGlobalPad;
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
    int GetNumAssignedControllers();
    void PreUpdate(float fDeltaT);
    bool CalculateFormationPosition(nlVector3& v3DestPosition,
        cFielder* pFielder, bool bInPosition,
        float fBallPosFormationWeight);
    PowerUpTeamType GetPowerUpByIndex(int index) const;
    int SetCurrentPowerUp(
        ePowerUpType eNewPowerUpType, int nnumOfPowerups);
    void SetDifficulty(int difficulty, int param2, bool param3);
    void fn_800A607C();

public:
    /* 0x00 */ int m_nSide;
    /* 0x04 */ int m_nScore;

public:
    /* 0x08 */ float mfPowerupMeter;

    /* 0x0C */ float mUnidentified00C;

private:
    /* 0x10 */ u8 mUnidentified010[0x08];

public:
    /* 0x18 */ u32 field_0x18;

private:
    /* 0x1C */ u8 mUnidentified01C[0x6C];

public:
    /* 0x88 */ cFielder* mpBestBallInterceptor;
    /* 0x8C */ PowerUpTeamType m_ePowerupList[2];
    /* 0xA4 */ cPlayer* m_pPlayers[5];
    /* 0xB8 */ cFielder* m_pAIOrderedFielders[4];
    /* 0xC8 */ cFielder* m_pBallInterceptOrderedFielders[4];
    /* 0xD8 */ cFielder* mUnidentified0D8[4];
    /* 0xE8 */ cNet* m_pNet;
    /* 0xEC */ FormationManager* m_pFormationManager;
    /* 0xF0 */ void* mUnidentified0F0;
};

extern cTeam* g_pTeams[];

#endif // GAME_TEAM_H
