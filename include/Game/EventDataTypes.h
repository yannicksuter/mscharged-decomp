#ifndef GAME_EVENT_DATA_TYPES_H
#define GAME_EVENT_DATA_TYPES_H

#include "NL/nlMath.h"

class cPlayer;
class CrowdRiot;
class PhysicsObject;
class PhysicsPatch;
class PhysicsSphere_80175F8C;
class cFielder;
class cBall;
class ChainChomp;
class PowerupBase;

struct CollisionPlayerPlayerData
{
    /* 0x00 */ cPlayer* player1;
    /* 0x04 */ cPlayer* player2;
    /* 0x08 */ nlVector3 velocity1;
    /* 0x14 */ nlVector3 velocity2;
}; // total size: 0x20

template <typename T>
class SlotPool;
extern SlotPool<CollisionPlayerPlayerData> gCollisionPlayerPlayerDataPool;

struct ShotAtGoalData
{
    /* 0x00 */ cPlayer* pShooter;
}; // total size: 0x4
extern SlotPool<ShotAtGoalData> gShotAtGoalDataPool;

struct CollisionChainPlayerData
{
    /* 0x00 */ cFielder* pFielder;
    /* 0x04 */ ChainChomp* pChain;
}; // total size: 0x8
extern SlotPool<CollisionChainPlayerData> gCollisionChainPlayerDataPool;

struct UnidentifiedEventData_800673FC
{
    /* 0x00 */ cPlayer* mUnidentified00;
}; // total size: 0x4

struct UnidentifiedEventData_80066B08
{
    /* 0x00 */ nlVector3 mUnidentified00;
    /* 0x0C */ PhysicsObject* mUnidentified0C;
    /* 0x10 */ CrowdRiot* mUnidentified10;
}; // total size: 0x14

struct UnidentifiedEventData_80066A04
{
    /* 0x00 */ cPlayer* mUnidentified00;
    /* 0x04 */ int mUnidentified04;
    /* 0x08 */ void* mUnidentified08;
    /* 0x0C */ int mUnidentified0C;
}; // total size: 0x10

struct UnidentifiedEventData00;
struct UnidentifiedEventData02;
struct UnidentifiedEventData03;
struct UnidentifiedEventData04;
struct UnidentifiedEventData05;
struct UnidentifiedEventData06;
struct UnidentifiedEventData07;
struct UnidentifiedEventData08;
struct CollisionBallChainData
{
    /* 0x00 */ ChainChomp* pChain;
    /* 0x04 */ cBall* pBall;
}; // total size: 0x8
extern SlotPool<CollisionBallChainData> gCollisionBallChainDataPool;
struct UnidentifiedEventData10;
struct UnidentifiedEventData11;
struct UnidentifiedEventData12;
struct UnidentifiedEventData13;
struct UnidentifiedEventData14;
struct UnidentifiedEventData15;
struct UnidentifiedEventData16;
struct UnidentifiedEventData17;
struct UnidentifiedEventData18;
struct UnidentifiedEventData19;
struct UnidentifiedEventData20;
struct UnidentifiedEventData21;
struct UnidentifiedEventData22;
struct UnidentifiedEventData23;
struct UnidentifiedEventData24
{
    /* 0x00 */ unsigned char mUnidentified00[0x0C];
    /* 0x0C */ cPlayer* mUnidentified0C;
    /* 0x10 */ PhysicsPatch* mUnidentified10;
}; // total size: 0x14
extern SlotPool<UnidentifiedEventData24> lbl_80570138;

struct UnidentifiedEventData25;
struct UnidentifiedEventData26;
struct UnidentifiedEventData27;
struct UnidentifiedEventData28;
struct CollisionChainPowerupData
{
    /* 0x00 */ ChainChomp* pChain;
    /* 0x04 */ PowerupBase* pPowerup;
}; // total size: 0x8
extern SlotPool<CollisionChainPowerupData> gCollisionChainPowerupDataPool;
struct UnidentifiedEventData30;
struct UnidentifiedEventData31;
struct UnidentifiedEventData32
{
    void* source;
    void* sourceValue;
    void* target;
};
struct UnidentifiedEventData33;
struct UnidentifiedEventData34;
struct UnidentifiedEventData35;
struct UnidentifiedEventData36;
struct UnidentifiedEventData37;

struct UnidentifiedEventData38
{
    /* 0x00 */ PhysicsSphere_80175F8C* mUnidentified00;
    /* 0x04 */ PhysicsObject* mUnidentified04;
    /* 0x08 */ unsigned int mUnidentified08;
}; // total size: 0xC

struct CollisionPlayerWallData
{
    /* 0x00 */ cPlayer* pPlayer;
    /* 0x04 */ nlVector3 contactPoint;
    /* 0x10 */ nlVector3 wallNormal;
}; // total size: 0x1C

void FreeEventDataPools();

#endif // GAME_EVENT_DATA_TYPES_H
