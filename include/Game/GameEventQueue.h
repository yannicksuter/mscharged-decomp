#ifndef GAME_GAME_EVENT_QUEUE_H
#define GAME_GAME_EVENT_QUEUE_H

#include "Game/Event.h"
#include "Game/EventDataTypes.h"

struct UnidentifiedEventData_80065E10;
struct GoalScoredData;
struct UnidentifiedEventData_80066008;
struct ReceiveBallData;
struct UnidentifiedEventData_800663A8;
struct GoalieSaveData;
struct ShotAtGoalData;
struct PlayerAttackData;
struct CollisionPowerupStatsData;
struct CollisionCrowdData;
struct CollisionChainPlayerData;
struct UnidentifiedEventData_80066E14;
struct UnidentifiedEventData_80066F18;
struct MegaStrikeMeterData;
struct LightningStrikeData;
struct MegaStrikeEndData;
struct PeachPhotoData;
struct UnidentifiedEventData_800676D8;
struct UnidentifiedEventData_800678D0;

class UnidentifiedGameEventQueue
{
public:
    UnidentifiedGameEventQueue();

    UnidentifiedQueuedEvent<UnidentifiedEventNoData> mEvent00;
    UnidentifiedQueuedEvent<UnidentifiedEventNoData> mEvent01;
    UnidentifiedQueuedEvent<UnidentifiedEventNoData> mEvent02;
    UnidentifiedQueuedEvent<UnidentifiedEventNoData> mEvent03;
    UnidentifiedQueuedEvent<UnidentifiedEventNoData> mEvent04;
    UnidentifiedQueuedEvent<UnidentifiedEventData_80065E10> mEvent05;
    ImmediateEvent<GoalScoredData> mEvent06;
    UnidentifiedQueuedEvent<UnidentifiedEventNoData> mEvent07;
    UnidentifiedQueuedEvent<UnidentifiedEventData_80066008> mEvent08;
    ImmediateEvent<UnidentifiedEventNoData> mEvent09;
    ImmediateEvent<UnidentifiedEventNoData> mEvent10;
    UnidentifiedQueuedEvent<UnidentifiedEventNoData> mEvent11;
    UnidentifiedQueuedEvent<UnidentifiedEventNoData> mEvent12;
    UnidentifiedQueuedEvent<UnidentifiedEventNoData> mEvent13;
    ImmediateEvent<void(int, int)> mEvent14;
    ImmediateEvent<ReceiveBallData> mEvent15;
    ImmediateEvent<UnidentifiedEventData_800663A8> mEvent16;
    ImmediateEvent<GoalieSaveData> mEvent17;
    ImmediateEvent<GoalieSaveData> mEvent18;
    ImmediateEvent<GoalieSaveData> mEvent19;
    ImmediateEvent<GoalieSaveData> mEvent20;
    ImmediateEvent<GoalieSaveData> mEvent21;
    UnidentifiedQueuedEvent<ShotAtGoalData> mEvent22;
    ImmediateEvent<ShotAtGoalData> mEvent23;
    ImmediateEvent<PlayerAttackData> mEvent24;
    ImmediateEvent<PlayerAttackData> mEvent25;
    ImmediateEvent<PlayerAttackData> mEvent26;
    ImmediateEvent<PlayerAttackData> mEvent27;
    UnidentifiedQueuedEvent<PlayerAttackData> mEvent28;
    UnidentifiedQueuedEvent<PlayerAttackData> mEvent29;
    UnidentifiedQueuedEvent<CollisionPlayerWallData> mEvent30;
    UnidentifiedQueuedEvent<CollisionPowerupStatsData> mEvent31;
    UnidentifiedQueuedEvent<CollisionCrowdData> mEvent32;
    UnidentifiedQueuedEvent<CollisionChainPlayerData> mEvent33;
    UnidentifiedQueuedEvent<CollisionWindDebrisPlayerData> mEvent34;
    UnidentifiedQueuedEvent<UnidentifiedEventData_80066E14> mEvent35;
    UnidentifiedQueuedEvent<ShotAtGoalData> mEvent36;
    UnidentifiedQueuedEvent<ShotAtGoalData> mEvent37;
    UnidentifiedQueuedEvent<UnidentifiedEventData_80066F18> mEvent38;
    UnidentifiedQueuedEvent<UnidentifiedEventNoData> mEvent39;
    ImmediateEvent<MegaStrikeMeterData> mEvent40;
    ImmediateEvent<MegaStrikeMeterData> mEvent41;
    ImmediateEvent<MegaStrikeMeterData> mEvent42;
    ImmediateEvent<UnidentifiedEventNoData> mEvent43;
    UnidentifiedQueuedEvent<LightningStrikeData> mEvent44;
    UnidentifiedQueuedEvent<UnidentifiedEventNoData> mEvent45;
    ImmediateEvent<cPlayer> mEvent46;
    ImmediateEvent<MegaStrikeEndData> mEvent47;
    ImmediateEvent<UnidentifiedEventNoData> mEvent48;
    ImmediateEvent<UnidentifiedEventNoData> mEvent49;
    ImmediateEvent<UnidentifiedEventNoData> mEvent50;
    ImmediateEvent<UnidentifiedEventNoData> mEvent51;
    ImmediateEvent<UnidentifiedEventNoData> mEvent52;
    ImmediateEvent<UnidentifiedEventNoData> mEvent53;
    ImmediateEvent<PeachPhotoData> mEvent54;
    ImmediateEvent<PeachPhotoData> mEvent55;
    ImmediateEvent<PeachPhotoData> mEvent56;
    ImmediateEvent<PeachPhotoData> mEvent57;
    ImmediateEvent<cPlayer> mEvent58;
    ImmediateEvent<cPlayer> mEvent59;
    ImmediateEvent<cPlayer> mEvent60;
    ImmediateEvent<cPlayer> mEvent61;
    ImmediateEvent<cPlayer> mEvent62;
    ImmediateEvent<CollisionBulletBillData> mEvent63;
    ImmediateEvent<cFielder> mEvent64;
    UnidentifiedQueuedEvent<UnidentifiedEventNoData> mEvent65;
    UnidentifiedQueuedEvent<UnidentifiedEventNoData> mEvent66;
    UnidentifiedQueuedEvent<UnidentifiedEventNoData> mEvent67;
    UnidentifiedQueuedEvent<UnidentifiedEventData_800676D8> mEvent68;
    UnidentifiedQueuedEvent<UnidentifiedEventNoData> mEvent69;
    UnidentifiedQueuedEvent<UnidentifiedEventNoData> mEvent70;
    ImmediateEvent<CharacterImpactEvent> mEvent71;
    ImmediateEvent<CharacterImpactEvent> mEvent72;
    ImmediateEvent<CharacterImpactEvent> mEvent73;
    UnidentifiedQueuedEvent<UnidentifiedEventData_800678D0> mEvent74;
};

#endif // GAME_GAME_EVENT_QUEUE_H
