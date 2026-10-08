#ifndef GAME_GAME_EVENT_QUEUE_H
#define GAME_GAME_EVENT_QUEUE_H

#include "Game/Event.h"
#include "Game/EventDataTypes.h"

struct NISData;
struct GoalScoredData;
struct CharacterDirectionData;
struct ReceiveBallData;
struct PassBallData;
struct GoalieSaveData;
struct ShotAtGoalData;
struct PlayerAttackData;
struct CollisionPowerupStatsData;
struct CollisionCrowdData;
struct CollisionChainPlayerData;
struct CollisionExplosionFragmentPlayerData;
struct PenaltyData;
struct MegaStrikeMeterData;
struct LightningStrikeData;
struct MegaStrikeEndData;
struct PeachPhotoData;
struct ExplodableExplodeData;
struct PowerupAcquireData;

class GameEventQueue
{
public:
    GameEventQueue();

    QueuedEvent<NoEventData> mPauseGameEvent;
    QueuedEvent<NoEventData> mResumingGameEvent;
    QueuedEvent<NoEventData> mGameOverEvent;
    QueuedEvent<NoEventData> mGameIsWonEvent;
    QueuedEvent<NoEventData> mPresentationBypassEvent;
    QueuedEvent<NISData> mNISEvent;
    ImmediateEvent<GoalScoredData> mGoalScoredEvent;
    QueuedEvent<NoEventData> mEnterStartScreenEvent;
    QueuedEvent<CharacterDirectionData> mDirectionBeginEvent;
    ImmediateEvent<NoEventData> mCharacterDirectionEndEvent;
    ImmediateEvent<NoEventData> mResetEffectsEvent;
    QueuedEvent<NoEventData> mGetReadyForKickoffEvent;
    QueuedEvent<NoEventData> mKickoffEvent;
    QueuedEvent<NoEventData> mSuddenDeathEvent;
    ImmediateEvent<void(int, int)> mBallStateChangeEvent;
    ImmediateEvent<ReceiveBallData> mReceiveBallEvent;
    ImmediateEvent<PassBallData> mPassBallEvent;
    ImmediateEvent<GoalieSaveData> mGoalieSaveEvent;
    ImmediateEvent<GoalieSaveData> mGoalieKickEvent;
    ImmediateEvent<GoalieSaveData> mCollisionBallGoalieEvent;
    ImmediateEvent<GoalieSaveData> mGoalieCatchEvent;
    ImmediateEvent<GoalieSaveData> mGoalieExertEvent;
    QueuedEvent<ShotAtGoalData> mShotAtGoalEvent;
    ImmediateEvent<ShotAtGoalData> mWindupShotEvent;
    ImmediateEvent<PlayerAttackData> mGoalieDekeAttackAttemptEvent;
    ImmediateEvent<PlayerAttackData> mGoalieDekeAttackSuccessEvent;
    ImmediateEvent<PlayerAttackData> mGoalieSlamAttackAttemptEvent;
    ImmediateEvent<PlayerAttackData> mGoalieSlamAttackSuccessEvent;
    QueuedEvent<PlayerAttackData> mAttackAttemptEvent;
    QueuedEvent<PlayerAttackData> mAttackSuccessEvent;
    QueuedEvent<CollisionPlayerWallData> mCharGetElectrocutedEvent;
    QueuedEvent<CollisionPowerupStatsData> mEvent31;
    QueuedEvent<CollisionCrowdData> mCollisionCrowdEvent;
    QueuedEvent<CollisionChainPlayerData> mCollisionChainPlayerEvent;
    QueuedEvent<CollisionWindDebrisPlayerData> mCollisionWindDebrisPlayerEvent;
    QueuedEvent<CollisionExplosionFragmentPlayerData> mCollisionExplosionFragmentPlayerEvent;
    QueuedEvent<ShotAtGoalData> mChainNisStartEvent;
    QueuedEvent<ShotAtGoalData> mChainNisEndEvent;
    QueuedEvent<PenaltyData> mPenaltyEvent;
    QueuedEvent<NoEventData> mAwardPowerupStuffEvent;
    ImmediateEvent<MegaStrikeMeterData> mMegaStrikeMeterStartEvent;
    ImmediateEvent<MegaStrikeMeterData> mMegaStrikeMeterFirstEvent;
    ImmediateEvent<MegaStrikeMeterData> mMegaStrikeMeterSecondEvent;
    ImmediateEvent<NoEventData> mMegaStrikeMeterEndEvent;
    QueuedEvent<LightningStrikeData> mLightningStrikeEvent;
    QueuedEvent<NoEventData> mMegaStrikeStartEvent;
    ImmediateEvent<cPlayer> mMegaStrikeIntroEvent;
    ImmediateEvent<MegaStrikeEndData> mMegaStrikeEndEvent;
    ImmediateEvent<NoEventData> mShotPresentationEvent;
    ImmediateEvent<NoEventData> mShotPresentationEndEvent;
    ImmediateEvent<NoEventData> mCaptainClashPresentationEvent;
    ImmediateEvent<NoEventData> mCaptainClashPresentationEndEvent;
    ImmediateEvent<NoEventData> mWindupPresentationEvent;
    ImmediateEvent<NoEventData> mWindupPresentationEndEvent;
    ImmediateEvent<PeachPhotoData> mPeachCameraFlashEvent;
    ImmediateEvent<PeachPhotoData> mPeachFlashEvent;
    ImmediateEvent<PeachPhotoData> mPeachCamerasDownEvent;
    ImmediateEvent<PeachPhotoData> mPeachCamerasAwayEvent;
    ImmediateEvent<cPlayer> mWaluigiWallStartEvent;
    ImmediateEvent<cPlayer> mWaluigiWallEndEvent;
    ImmediateEvent<cPlayer> mWaluigiWallAbortEvent;
    ImmediateEvent<cPlayer> mWarioGasStartEvent;
    ImmediateEvent<cPlayer> mWarioGasEndEvent;
    ImmediateEvent<CollisionBulletBillData> mBulletBillExplodeEvent;
    ImmediateEvent<cFielder> mSuperPresentationEvent;
    QueuedEvent<NoEventData> mStatsPowerupHitDataEvent;
    QueuedEvent<NoEventData> mCameraRumbleStartEvent;
    QueuedEvent<NoEventData> mCameraRumbleEndEvent;
    QueuedEvent<ExplodableExplodeData> mExplodableExplodeEvent;
    QueuedEvent<NoEventData> mExplodableExplosionEndEvent;
    QueuedEvent<NoEventData> mSilenceAllSoundsEvent;
    ImmediateEvent<CharacterImpactEvent> mMontyReappearEvent;
    ImmediateEvent<CharacterImpactEvent> mHammerBroHammerEvent;
    ImmediateEvent<CharacterImpactEvent> mWarioGroundPoundEvent;
    QueuedEvent<PowerupAcquireData> mPowerupAcquireEvent;
};

#endif // GAME_GAME_EVENT_QUEUE_H
