#include <stddef.h>

#include "Game/Team.h"

#include "Game/AI/Fielder.h"
#include "Game/Formation.h"
#include "Game/GameInfo.h"
#include "Game/Goalie.h"
#include "Game/Net.h"
#include "Game/Player.h"

cTeam* g_pTeams[2] = { NULL, NULL };

/**
 * Offset/Address/Size: 0x700 | 0x800A6434 | size: 0x1C
 */
void cTeam::ClearAllPowerUps()
{
    m_ePowerupList[0].eType = POWER_UP_NONE;
    m_ePowerupList[0].nnumOfPowerups = 0;
    m_ePowerupList[1].eType = POWER_UP_NONE;
    m_ePowerupList[1].nnumOfPowerups = 0;
}

/**
 * Offset/Address/Size: 0xB10 | 0x800A6844 | size: 0x1C
 */
PowerUpTeamType cTeam::GetCurrentPowerUp() const
{
    return m_ePowerupList[0];
}

/**
 * Offset/Address/Size: 0xB2C | 0x800A6860 | size: 0x58
 */
PowerUpTeamType cTeam::GetPowerUpByIndex(int index) const
{
    PowerUpTeamType eDummy;
    if (index >= 0)
    {
        return m_ePowerupList[index];
    }
    eDummy.eType = POWER_UP_NONE;
    eDummy.nnumOfPowerups = 0;
    return eDummy;
}

/**
 * Offset/Address/Size: 0xB84 | 0x800A68B8 | size: 0x18
 */
void cTeam::SetIsPowerUpNew(int index, bool isNew)
{
    if (index >= 0)
    {
        m_ePowerupList[index].bIsNew = isNew;
    }
}

/**
 * Offset/Address/Size: 0xB9C | 0x800A68D0 | size: 0x58
 */
int cTeam::SetCurrentPowerUp(
    ePowerUpType eNewPowerUpType, int nnumOfPowerups)
{
    unsigned char bGivenNewPowerup = 0;
    for (int a = 0; a < 2; ++a)
    {
        if (m_ePowerupList[a].eType == POWER_UP_NONE && !bGivenNewPowerup)
        {
            m_ePowerupList[a].eType = eNewPowerUpType;
            bGivenNewPowerup = 1;
            m_ePowerupList[a].nnumOfPowerups = nnumOfPowerups;
            m_ePowerupList[a].bIsNew = 1;
        }
    }
    return bGivenNewPowerup;
}

/**
 * Offset/Address/Size: 0xBF4 | 0x800A6928 | size: 0x24
 */
void cTeam::SetPlayer(cPlayer* pPlayer, int nIndex)
{
    m_pPlayers[nIndex] = pPlayer;
    if (nIndex < 4)
    {
        m_pAIOrderedFielders[nIndex] = (cFielder*)pPlayer;
        m_pBallInterceptOrderedFielders[nIndex] = (cFielder*)pPlayer;
        mUnidentified0D8[nIndex] = (cFielder*)pPlayer;
    }
}

/**
 * Offset/Address/Size: 0xC18 | 0x800A694C | size: 0x8
 */
void cTeam::SetGoalie(Goalie* pGoalie)
{
    m_pPlayers[4] = pGoalie;
}

/**
 * Offset/Address/Size: 0xC20 | 0x800A6954 | size: 0x8
 */
Goalie* cTeam::GetGoalie()
{
    return (Goalie*)m_pPlayers[4];
}

/**
 * Offset/Address/Size: 0xCC8 | 0x800A69FC | size: 0x78
 */
int cTeam::GetNumAssignedControllers()
{
    int mySide, numAssignedControllers;
    unsigned short i;
    short playingSide;

    numAssignedControllers = 0;
    for (i = 0; i < 16; i++)
    {
        mySide = m_nSide;
        playingSide = GameInfoManager::Instance()->GetPlayingSide(i);
        if (playingSide == mySide)
        {
            numAssignedControllers++;
        }
    }
    return numAssignedControllers;
}

/**
 * Offset/Address/Size: 0xD40 | 0x800A6A74 | size: 0x10
 */
cFielder* cTeam::GetFielder(int nIndex)
{
    return (cFielder*)m_pPlayers[nIndex];
}

/**
 * Offset/Address/Size: 0xD50 | 0x800A6A84 | size: 0x10
 */
cPlayer* cTeam::GetPlayer(int nIndex)
{
    return m_pPlayers[nIndex];
}

/**
 * Offset/Address/Size: 0xD60 | 0x800A6A94 | size: 0x18
 */
cTeam* cTeam::GetOtherTeam()
{
    return g_pTeams[m_nSide == HOME ? AWAY : HOME];
}

/**
 * Offset/Address/Size: 0xD78 | 0x800A6AAC | size: 0x1C
 */
cNet* cTeam::GetOtherNet()
{
    return g_pTeams[m_nSide == HOME ? AWAY : HOME]->m_pNet;
}

/**
 * Offset/Address/Size: 0xEF8 | 0x800A6C2C | size: 0x68
 */
void cTeam::PreUpdate(float fDeltaT)
{
    for (int i = 0; i < 5; i++)
    {
        m_pPlayers[i]->PreUpdate(fDeltaT);
    }
}

/**
 * Offset/Address/Size: 0x1CD4 | 0x800A7A08 | size: 0x8
 */
bool cTeam::CalculateFormationPosition(nlVector3& v3DestPosition,
    cFielder* pFielder, bool bInPosition,
    float fBallPosFormationWeight)
{
    return m_pFormationManager->CalculateFielderPosition(
        v3DestPosition, pFielder, bInPosition, fBallPosFormationWeight);
}

/**
 * Offset/Address/Size: 0x2AC4 | 0x800A87F8 | size: 0x8
 */
cFielder* cTeam::GetCaptain()
{
    return (cFielder*)m_pPlayers[0];
}

/**
 * Offset/Address/Size: 0x2ACC | 0x800A8800 | size: 0x8
 */
cFielder* cTeam::GetStriker() const
{
    return m_pAIOrderedFielders[0];
}

/**
 * Offset/Address/Size: 0x2AD4 | 0x800A8808 | size: 0x7C
 */
cFielder* cTeam::GetFrontMostFielder()
{
    cFielder* pFielder;
    cFielder* pFrontMostFielder = NULL;

    for (int i_fielder = 0; i_fielder < 4; i_fielder++)
    {
        pFielder = (cFielder*)m_pPlayers[i_fielder];
        if ((pFrontMostFielder == NULL)
            || (pFielder->m_v3AIPosition.x > pFrontMostFielder->m_v3AIPosition.x))
        {
            pFrontMostFielder = pFielder;
        }
    }

    return pFrontMostFielder;
}

/**
 * Offset/Address/Size: 0x2B50 | 0x800A8884 | size: 0x7C
 */
cFielder* cTeam::GetRearMostFielder()
{
    cFielder* pFielder;
    cFielder* pRearMostFielder = NULL;

    for (int i_fielder = 0; i_fielder < 4; i_fielder++)
    {
        pFielder = (cFielder*)m_pPlayers[i_fielder];
        if ((pRearMostFielder == NULL)
            || (pFielder->m_v3AIPosition.x < pRearMostFielder->m_v3AIPosition.x))
        {
            pRearMostFielder = pFielder;
        }
    }

    return pRearMostFielder;
}
