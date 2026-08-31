#ifndef _SPACESEARCH_H_
#define _SPACESEARCH_H_

#include "Game/Goalie.h"
#include "Game/Net.h"
#include "Game/Player.h"
#include "Game/Team.h"
#include "NL/nlMath.h"

enum eFieldDirection
{
    DIR_NONE = 0,
    DIR_UPFIELD = 1,
    DIR_DOWNFIELD = 2,
    DIR_TOWARD_TARGET = 3,
    DIR_AWAYFROM_TARGET = 4,
    DIR_CUSTOM = 5,
    NUM_DIRECTIONS = 6,
};

class SpaceSearch
{
public:
    SpaceSearch(cPlayer* pPlayer)
    {
        m_fNetDirection = pPlayer->m_pTeam->m_pNet->m_fDirection;
        m_bDebugOn = false;
        m_bDrawSearchSpace = false;
        m_fMaxRadius = 0.0f;
    }

    SpaceSearch(float fNetDirection)
    {
        m_fNetDirection = fNetDirection;
        m_bDebugOn = false;
        m_bDrawSearchSpace = false;
        m_fMaxRadius = 0.0f;
    }

    virtual ~SpaceSearch();
    virtual float EvaluatePosition(
        const nlVector3& v3TestPosition,
        const nlVector3& v3CenterPos,
        eFieldDirection eSearchDir,
        unsigned short aDirection)
        = 0;

    float FindBestPosition(
        nlVector3& v3Dest,
        const nlVector3& v3CenterPos,
        eFieldDirection eSearchDir,
        const nlVector3* pv3TargetOrDirection,
        float fMaxRadius,
        unsigned short aSearchCone);

    /* 0x04 */ float m_fNetDirection;
    /* 0x08 */ bool m_bDebugOn;
    /* 0x09 */ bool m_bDrawSearchSpace;
    /* 0x0C */ float m_fMaxRadius;
}; // total size: 0x10

class SSearchCutAndBreak : public SpaceSearch
{
public:
    SSearchCutAndBreak(cPlayer* pPlayer);
    virtual float EvaluatePosition(
        const nlVector3& v3TestPosition,
        const nlVector3& v3CenterPos,
        eFieldDirection eSearchDir,
        unsigned short aDirection);

    /* 0x10 */ cPlayer* m_pPlayer;
}; // total size: 0x14

class SSearchOpenLane : public SpaceSearch
{
public:
    SSearchOpenLane(cPlayer* pPlayer1, cPlayer* pPlayer2);
    virtual float EvaluatePosition(
        const nlVector3& position,
        const nlVector3& v3CenterPos,
        eFieldDirection eSearchDir,
        unsigned short aDirection);

    /* 0x10 */ cPlayer* m_pBallOwner;
    /* 0x14 */ cPlayer* m_pPassTarget;
    /* 0x18 */ nlVector3 m_v3OtherPos;
    /* 0x24 */ bool m_bOtherPosIsTarget;
}; // total size: 0x28

class SSearchIdealShot : public SpaceSearch
{
public:
    SSearchIdealShot(cPlayer* pBallOwner)
        : SpaceSearch(pBallOwner->m_pTeam->m_pNet->m_fDirection)
        , m_SSearchOpenLane(pBallOwner, NULL)
    {
        m_pGoalie = pBallOwner->m_pTeam->GetOtherTeam()->GetGoalie();
    }
    virtual float EvaluatePosition(
        const nlVector3& position,
        const nlVector3& v3CenterPos,
        eFieldDirection eSearchDir,
        unsigned short aDirection);

    /* 0x10 */ Goalie* m_pGoalie;
    /* 0x14 */ SSearchOpenLane m_SSearchOpenLane;
}; // total size: 0x3C

class SSearchRunToNet : public SpaceSearch
{
public:
    SSearchRunToNet(cPlayer* pPlayer);
    virtual ~SSearchRunToNet() { }
    virtual float EvaluatePosition(
        const nlVector3& v3TestPosition,
        const nlVector3& v3CenterPos,
        eFieldDirection eSearchDir,
        unsigned short aDirection);

    /* 0x10 */ SSearchIdealShot m_SSearchIdealShot;
}; // total size: 0x4C

class SSearchBestPass : public SpaceSearch
{
public:
    SSearchBestPass(cPlayer* pBallOwner, cPlayer* pPassTarget,
        bool bAllowLeadPass, bool bIsPerfectPass, float fPassSpeed);
    virtual ~SSearchBestPass() { }
    virtual float EvaluatePosition(
        const nlVector3& position,
        const nlVector3& v3OtherPosition,
        eFieldDirection eSearchDir,
        unsigned short aDirection);

    /* 0x10 */ nlVector3 m_v3PassDirection;
    /* 0x1C */ float m_fPassSpeed;
    /* 0x20 */ bool m_bAllowLeadPass;
    /* 0x21 */ bool m_bIsPerfectPass;
    /* 0x24 */ cPlayer* m_pBallOwner;
    /* 0x28 */ cPlayer* m_pPassTarget;
}; // total size: 0x2C

#endif // _SPACESEARCH_H_
