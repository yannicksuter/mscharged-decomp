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

    QueuedEvent<UnidentifiedEventNoData> mPauseGameEvent;
    QueuedEvent<UnidentifiedEventNoData> mResumingGameEvent;
    QueuedEvent<UnidentifiedEventNoData> mGameOverEvent;
    QueuedEvent<UnidentifiedEventNoData> mGameIsWonEvent;
    QueuedEvent<UnidentifiedEventNoData> mPresentationBypassEvent;
    QueuedEvent<NISData> mNISEvent;
    ImmediateEvent<GoalScoredData> mGoalScoredEvent;
    QueuedEvent<UnidentifiedEventNoData> mEnterStartScreenEvent;
    QueuedEvent<CharacterDirectionData> mDirectionBeginEvent;
    ImmediateEvent<UnidentifiedEventNoData> mCharacterDirectionEndEvent;
    ImmediateEvent<UnidentifiedEventNoData> mResetEffectsEvent;
    QueuedEvent<UnidentifiedEventNoData> mGetReadyForKickoffEvent;
    QueuedEvent<UnidentifiedEventNoData> mKickoffEvent;
    QueuedEvent<UnidentifiedEventNoData> mSuddenDeathEvent;
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
    QueuedEvent<UnidentifiedEventNoData> mAwardPowerupStuffEvent;
    ImmediateEvent<MegaStrikeMeterData> mMegaStrikeMeterStartEvent;
    ImmediateEvent<MegaStrikeMeterData> mMegaStrikeMeterFirstEvent;
    ImmediateEvent<MegaStrikeMeterData> mMegaStrikeMeterSecondEvent;
    ImmediateEvent<UnidentifiedEventNoData> mMegaStrikeMeterEndEvent;
    QueuedEvent<LightningStrikeData> mLightningStrikeEvent;
    QueuedEvent<UnidentifiedEventNoData> mMegaStrikeStartEvent;
    ImmediateEvent<cPlayer> mMegaStrikeIntroEvent;
    ImmediateEvent<MegaStrikeEndData> mMegaStrikeEndEvent;
    ImmediateEvent<UnidentifiedEventNoData> mShotPresentationEvent;
    ImmediateEvent<UnidentifiedEventNoData> mShotPresentationEndEvent;
    ImmediateEvent<UnidentifiedEventNoData> mCaptainClashPresentationEvent;
    ImmediateEvent<UnidentifiedEventNoData> mCaptainClashPresentationEndEvent;
    ImmediateEvent<UnidentifiedEventNoData> mWindupPresentationEvent;
    ImmediateEvent<UnidentifiedEventNoData> mWindupPresentationEndEvent;
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
    QueuedEvent<UnidentifiedEventNoData> mStatsPowerupHitDataEvent;
    QueuedEvent<UnidentifiedEventNoData> mCameraRumbleStartEvent;
    QueuedEvent<UnidentifiedEventNoData> mCameraRumbleEndEvent;
    QueuedEvent<ExplodableExplodeData> mExplodableExplodeEvent;
    QueuedEvent<UnidentifiedEventNoData> mExplodableExplosionEndEvent;
    QueuedEvent<UnidentifiedEventNoData> mSilenceAllSoundsEvent;
    ImmediateEvent<CharacterImpactEvent> mMontyReappearEvent;
    ImmediateEvent<CharacterImpactEvent> mHammerBroHammerEvent;
    ImmediateEvent<CharacterImpactEvent> mWarioGroundPoundEvent;
    QueuedEvent<PowerupAcquireData> mPowerupAcquireEvent;
};

#endif // GAME_GAME_EVENT_QUEUE_H
