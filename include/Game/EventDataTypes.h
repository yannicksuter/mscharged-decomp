#ifndef GAME_EVENT_DATA_TYPES_H
#define GAME_EVENT_DATA_TYPES_H

#include "Game/AI/Powerups.h"
#include "NL/nlMath.h"
#include "NL/nlSlotPool.h"
#include "types.h"

class BulletBillObject;
class ChainChomp;
class CrowdRiot;
class NetMesh;
class PhysicsObject;
class PhysicsPatch;
class PhysicsSphere_80175F8C;
class cBall;
class cCharacter;
class cFielder;
class cPlayer;
struct KoopaShellObject;

struct CharacterImpactEvent
{
    nlVector3 v3Position;
    float fMagnitude;
    cCharacter* pCharacter;
};

struct GoalScoredData
{
    /* 0x00 */ unsigned int uTeamIndex : 8;
    /* 0x00 */ unsigned int uNumGoalsScored : 8;
    /* 0x00 */ unsigned int uGoalType : 16;
    /* 0x04 */ nlVector3 v3ShotPosition;
    /* 0x10 */ cPlayer* pScorer;
    /* 0x14 */ cPlayer* pAssister;
    /* 0x18 */ cPlayer* pLastTouch[2];
}; // total size: 0x20

struct MegaStrikeEndData
{
    /* 0x00 */ cPlayer* pPlayer;
    /* 0x04 */ s8 attempts;
    /* 0x05 */ s8 goals;
    /* 0x06 */ s8 defendingSide;
    /* 0x07 */ s8 goalValue;
};

struct UnidentifiedEventData_8006701C
{
    /* 0x00 */ cFielder* pFielder;
    /* 0x04 */ float fMeterValue;
    /* 0x08 */ nlVector3 v3Position;
}; // total size: 0x14

enum eReceiveBallResult
{
    RECEIVEBALL_LOOSE_PICKUP = 0,
    RECEIVEBALL_PASS_COMPLETE = 1,
    RECEIVEBALL_PASS_INTERCEPT = 2,
};

struct ReceiveBallData
{
    /* 0x00 */ cPlayer* pReceiver;
    /* 0x04 */ eReceiveBallResult eResult;
}; // total size: 0x08

struct CollisionPlayerPlayerData
{
    /* 0x00 */ cPlayer* player1;
    /* 0x04 */ cPlayer* player2;
    /* 0x08 */ nlVector3 velocity1;
    /* 0x14 */ nlVector3 velocity2;
}; // total size: 0x20

struct CollisionChainPlayerData
{
    /* 0x00 */ cFielder* pFielder;
    /* 0x04 */ ChainChomp* pChain;
}; // total size: 0x8

class UnidentifiedNPC_801B43F8;

struct CollisionWindDebrisPlayerData
{
    /* 0x00 */ cFielder* pFielder;
    /* 0x04 */ UnidentifiedNPC_801B43F8* pDebris;
}; // total size: 0x8

struct CollisionThwompPlayerData
{
    /* 0x00 */ void* source;
    /* 0x04 */ void* sourceValue;
    /* 0x08 */ cCharacter* target;
}; // total size: 0xC

struct CollisionBulletBillData
{
    /* 0x00 */ cCharacter* player;
    /* 0x04 */ BulletBillObject* bulletBill;
}; // total size: 0x8

struct CollisionChainPowerupData
{
    /* 0x00 */ ChainChomp* pChain;
    /* 0x04 */ PowerupBase* pPowerup;
}; // total size: 0x8

struct CollisionPlayerWallData
{
    /* 0x00 */ cPlayer* pPlayer;
    /* 0x04 */ nlVector3 contactPoint;
    /* 0x10 */ nlVector3 wallNormal;
}; // total size: 0x1C

struct LightningStrikeData
{
    /* 0x00 */ nlVector3 position;
    /* 0x0C */ float radius;
}; // total size: 0x10

struct CollisionBallWallData
{
    /* 0x00 */ cBall* pBall;
    /* 0x04 */ bool bIsPerfect;
    /* 0x05 */ bool bIsShot;
    /* 0x08 */ nlVector3 position;
    /* 0x14 */ nlVector3 normal;
    /* 0x20 */ float fCollisionVecLen;
}; // total size: 0x24

struct UnidentifiedEventData_80066B08
{
    /* 0x00 */ nlVector3 mUnidentified00;
    /* 0x0C */ PhysicsObject* mUnidentified0C;
    /* 0x10 */ CrowdRiot* mUnidentified10;
}; // total size: 0x14
typedef UnidentifiedEventData_80066B08 CollisionCrowdData;

struct CollisionPowerupGroundData
{
    /* 0x00 */ nlVector3 position;
    /* 0x0C */ float fVecZComponent;
    /* 0x10 */ ePowerUpType eType;
}; // total size: 0x14

struct CollisionBallGroundData
{
    /* 0x00 */ cBall* pBall;
    /* 0x04 */ bool bIsShot;
    /* 0x08 */ nlVector3 position;
    /* 0x14 */ nlVector3 normal;
    /* 0x20 */ float fVecZComponent;
}; // total size: 0x24

struct CollisionPowerupWallData
{
    /* 0x00 */ PowerupBase* pPowerup;
    /* 0x04 */ ePowerupSize eSize;
    /* 0x08 */ ePowerUpType eType;
    /* 0x0C */ nlVector3 position;
    /* 0x18 */ nlVector3 normal;
}; // total size: 0x24

struct CollisionPlayerBallData
{
    /* 0x00 */ cPlayer* pPlayer;
    /* 0x04 */ cBall* pBall;
    /* 0x08 */ nlVector3 velocity;
    /* 0x14 */ int boneID;
}; // total size: 0x18

struct CollisionKoopaShellGoalieData
{
    /* 0x00 */ KoopaShellObject* shell;
    /* 0x04 */ cCharacter* goalie;
}; // total size: 0x8

struct CollisionKoopaShotBallPlayerData
{
    /* 0x00 */ KoopaShellObject* shell;
    /* 0x04 */ cFielder* player;
}; // total size: 0x8

struct BirdoEggObject;

struct CollisionBirdoShotBallPlayerData
{
    /* 0x00 */ BirdoEggObject* egg;
    /* 0x04 */ cFielder* player;
}; // total size: 0x8

struct CollisionBirdoEggGoalieData
{
    /* 0x00 */ BirdoEggObject* egg;
    /* 0x04 */ cCharacter* goalie;
}; // total size: 0x8

struct CollisionKoopaShellEndData
{
    /* 0x00 */ void* shell;
    /* 0x04 */ bool cracked;
}; // total size: 0x8

struct CollisionBirdoEggEndData
{
    /* 0x00 */ BirdoEggObject* egg;
    /* 0x04 */ bool cracked;
}; // total size: 0x8

struct CollisionBallShellData
{
    /* 0x00 */ cBall* pBall;
    /* 0x04 */ PowerupBase* pPowerup;
    /* 0x08 */ nlVector3 v3CollisionVelocity;
}; // total size: 0x14

struct CollisionBallChainData
{
    /* 0x00 */ ChainChomp* pChain;
    /* 0x04 */ cBall* pBall;
}; // total size: 0x8

struct CollisionPlayerShellData
{
    /* 0x00 */ cFielder* pPlayer;
    /* 0x04 */ cFielder* pThrower;
    /* 0x08 */ u8 nThrowerPadID;
    /* 0x09 */ u8 bIsExploder;
    /* 0x0C */ int eSize;
    /* 0x10 */ nlVector3 v3CollisionLocation;
    /* 0x1C */ nlVector3 v3CollisionVelocity;
}; // total size: 0x28

struct CollisionPlayerFreezeData
{
    /* 0x00 */ cFielder* pPlayer;
    /* 0x04 */ cFielder* pThrower;
    /* 0x08 */ int nThrowerPadID;
    /* 0x0C */ int eSize;
}; // total size: 0x10

struct CollisionPlayerBananaData
{
    /* 0x00 */ cFielder* pPlayer;
    /* 0x04 */ cFielder* pThrower;
    /* 0x08 */ int nThrowerPadID;
    /* 0x0C */ nlVector3 v3CollisionLocation;
}; // total size: 0x18

struct UnidentifiedEventData_80066A04
{
    /* 0x00 */ cPlayer* mUnidentified00;
    /* 0x04 */ int mUnidentified04;
    /* 0x08 */ cPlayer* mUnidentified08;
    /* 0x0C */ int mUnidentified0C;
}; // total size: 0x10
typedef UnidentifiedEventData_80066A04 CollisionPowerupStatsData;

struct CollisionBallGoalpostData
{
    /* 0x00 */ nlVector3 v3CollisionVelocity;
    /* 0x0C */ nlVector3 v3CollisionPosition;
    /* 0x18 */ unsigned int uTeamIndex;
}; // total size: 0x1C

struct BallNetmeshEventData
{
    /* 0x00 */ NetMesh* netMesh;
    /* 0x04 */ nlVector3 collisionVelocity;
}; // total size: 0x10

struct ShotAtGoalData
{
    /* 0x00 */ cPlayer* pShooter;
}; // total size: 0x4

struct PenaltyData
{
    /* 0x00 */ cFielder* pFouler;
    /* 0x04 */ cFielder* pFoulee;
    /* 0x08 */ float fPenaltyWorth;
}; // total size: 0xC

struct NISData
{
    /* 0x00 */ const char* Type;
    /* 0x04 */ const char* Param;
}; // total size: 0x8

struct PowerupUsedEventData
{
    /* 0x00 */ u8 data[0xC];
}; // total size: 0xC

struct PowerupHitPlayerEventData
{
    /* 0x00 */ ePowerUpType Type;
    /* 0x04 */ cPlayer* Thrower;
    /* 0x08 */ cPlayer* Target;
}; // total size: 0xC

extern SlotPool<CollisionPlayerPlayerData> g_CollisionPlayerPlayerDataPool;
extern SlotPool<CollisionChainPlayerData> g_CollisionChainPlayerDataPool;
extern SlotPool<CollisionWindDebrisPlayerData> g_CollisionWindDebrisPlayerDataPool;
extern SlotPool<CollisionThwompPlayerData> g_CollisionThwompPlayerDataPool;
extern SlotPool<CollisionBulletBillData> g_CollisionBulletBillDataPool;
extern SlotPool<CollisionChainPowerupData> g_CollisionChainPowerupDataPool;
extern SlotPool<CollisionPlayerWallData> g_CollisionPlayerWallDataPool;
extern SlotPool<LightningStrikeData> g_LightningStrikeDataPool;
extern SlotPool<CollisionBallWallData> g_CollisionBallWallDataPool;
extern SlotPool<CollisionCrowdData> g_CollisionCrowdDataPool;
extern SlotPool<CollisionPowerupGroundData> g_CollisionPowerupGroundDataPool;
extern SlotPool<CollisionBallGroundData> g_CollisionBallGroundDataPool;
extern SlotPool<CollisionPowerupWallData> g_CollisionPowerupWallDataPool;
extern SlotPool<CollisionPlayerBallData> g_CollisionPlayerBallDataPool;
extern SlotPool<CollisionKoopaShellGoalieData> g_CollisionKoopaShellGoalieDataPool;
extern SlotPool<CollisionKoopaShotBallPlayerData> g_CollisionKoopaShotBallPlayerDataPool;
extern SlotPool<CollisionBirdoShotBallPlayerData> g_CollisionBirdoShotBallPlayerDataPool;
extern SlotPool<CollisionBirdoEggGoalieData> g_CollisionBirdoEggGoalieDataPool;
extern SlotPool<CollisionKoopaShellEndData> g_CollisionKoopaShellEndDataPool;
extern SlotPool<CollisionBirdoEggEndData> g_CollisionBirdoEggEndDataPool;
extern SlotPool<CollisionBallShellData> g_CollisionBallShellDataPool;
extern SlotPool<CollisionBallChainData> g_CollisionBallChainDataPool;
extern SlotPool<CollisionPlayerShellData> g_CollisionPlayerShellDataPool;
extern SlotPool<CollisionPlayerFreezeData> g_CollisionPlayerFreezeDataPool;
extern SlotPool<CollisionPlayerBananaData> g_CollisionPlayerBananaDataPool;
extern SlotPool<CollisionPowerupStatsData> g_CollisionPowerupStatsDataPool;
extern SlotPool<CollisionBallGoalpostData> g_CollisionBallGoalpostDataPool;
extern SlotPool<BallNetmeshEventData> g_BallNetmeshEventDataPool;
extern SlotPool<ShotAtGoalData> g_ShotAtGoalDataPool;
extern SlotPool<PenaltyData> g_PenaltyDataPool;
extern SlotPool<NISData> g_NISDataPool;
extern SlotPool<PowerupUsedEventData> g_PowerupUsedEventDataPool;
extern SlotPool<PowerupHitPlayerEventData> g_PowerupHitPlayerEventDataPool;

extern "C" void fn_80025A14(CollisionPowerupStatsData* data);

void FreeEventDataPools();

struct UnidentifiedEventData_800673FC
{
    /* 0x00 */ cPlayer* mUnidentified00;
}; // total size: 0x4

struct UnidentifiedEventData00;
struct UnidentifiedEventData04;
struct UnidentifiedEventData12;
struct UnidentifiedEventData15;
struct UnidentifiedEventData16
{
    /* 0x00 */ cFielder* pFielder;
    /* 0x04 */ cBall* pBall;
};
struct UnidentifiedEventData17;
struct UnidentifiedEventData19;
struct UnidentifiedEventData24
{
    /* 0x00 */ unsigned char mUnidentified00[0x0C];
    /* 0x0C */ cPlayer* mUnidentified0C;
    /* 0x10 */ PhysicsPatch* mUnidentified10;
}; // total size: 0x14

struct UnidentifiedEventData25;
struct UnidentifiedEventData26
{
    /* 0x00 */ nlVector3 v3Position;
    /* 0x0C */ nlVector3 v3Velocity;
    /* 0x18 */ void* mUnidentified18;
    /* 0x1C */ cFielder* pFielder;
}; // total size: 0x20
struct UnidentifiedEventData27;
struct UnidentifiedEventData28;
struct UnidentifiedEventData29;
struct UnidentifiedEventData30;
struct UnidentifiedEventData31;
struct UnidentifiedEventData33;
class UnidentifiedObject_801B535C;

struct UnidentifiedEventData34
{
    /* 0x00 */ cPlayer* mUnidentified00;
    /* 0x04 */ cFielder* mUnidentified04;
    /* 0x08 */ UnidentifiedObject_801B535C* mUnidentified08;
    /* 0x0C */ u32 mUnidentified0C;
    /* 0x10 */ u32 mUnidentified10;
}; // total size: 0x14
struct UnidentifiedEventData35;
struct UnidentifiedEventData36;
struct UnidentifiedEventData37;

struct UnidentifiedEventData38
{
    /* 0x00 */ PhysicsSphere_80175F8C* mUnidentified00;
    /* 0x04 */ PhysicsObject* mUnidentified04;
    /* 0x08 */ unsigned int mUnidentified08;
}; // total size: 0xC

#endif // GAME_EVENT_DATA_TYPES_H
