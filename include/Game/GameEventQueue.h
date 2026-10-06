#ifndef GAME_GAME_EVENT_QUEUE_H
#define GAME_GAME_EVENT_QUEUE_H

#include "Game/Event.h"
#include "Game/EventDataTypes.h"

struct NISData;
struct GoalScoredData;
struct UnidentifiedEventData_80066008;
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

    UnidentifiedQueuedEvent<UnidentifiedEventNoData> mPauseGameEvent;
    UnidentifiedQueuedEvent<UnidentifiedEventNoData> mResumingGameEvent;
    UnidentifiedQueuedEvent<UnidentifiedEventNoData> mGameOverEvent;
    UnidentifiedQueuedEvent<UnidentifiedEventNoData> mGameIsWonEvent;
    UnidentifiedQueuedEvent<UnidentifiedEventNoData> mPresentationBypassEvent;
    UnidentifiedQueuedEvent<NISData> mNISEvent;
    ImmediateEvent<GoalScoredData> mGoalScoredEvent;
    UnidentifiedQueuedEvent<UnidentifiedEventNoData> mEnterStartScreenEvent;
    UnidentifiedQueuedEvent<UnidentifiedEventData_80066008> mDirectionBeginEvent;
    ImmediateEvent<UnidentifiedEventNoData> mCharacterDirectionEndEvent;
    ImmediateEvent<UnidentifiedEventNoData> mResetEffectsEvent;
    UnidentifiedQueuedEvent<UnidentifiedEventNoData> mGetReadyForKickoffEvent;
    UnidentifiedQueuedEvent<UnidentifiedEventNoData> mKickoffEvent;
    UnidentifiedQueuedEvent<UnidentifiedEventNoData> mSuddenDeathEvent;
    ImmediateEvent<void(int, int)> mBallStateChangeEvent;
    ImmediateEvent<ReceiveBallData> mReceiveBallEvent;
    ImmediateEvent<PassBallData> mPassBallEvent;
    ImmediateEvent<GoalieSaveData> mGoalieSaveEvent;
    ImmediateEvent<GoalieSaveData> mGoalieKickEvent;
    ImmediateEvent<GoalieSaveData> mCollisionBallGoalieEvent;
    ImmediateEvent<GoalieSaveData> mGoalieCatchEvent;
    ImmediateEvent<GoalieSaveData> mGoalieExertEvent;
    UnidentifiedQueuedEvent<ShotAtGoalData> mShotAtGoalEvent;
    ImmediateEvent<ShotAtGoalData> mWindupShotEvent;
    ImmediateEvent<PlayerAttackData> mGoalieDekeAttackAttemptEvent;
    ImmediateEvent<PlayerAttackData> mGoalieDekeAttackSuccessEvent;
    ImmediateEvent<PlayerAttackData> mGoalieSlamAttackAttemptEvent;
    ImmediateEvent<PlayerAttackData> mGoalieSlamAttackSuccessEvent;
    UnidentifiedQueuedEvent<PlayerAttackData> mAttackAttemptEvent;
    UnidentifiedQueuedEvent<PlayerAttackData> mAttackSuccessEvent;
    UnidentifiedQueuedEvent<CollisionPlayerWallData> mCharGetElectrocutedEvent;
    UnidentifiedQueuedEvent<CollisionPowerupStatsData> mEvent31;
    UnidentifiedQueuedEvent<CollisionCrowdData> mCollisionCrowdEvent;
    UnidentifiedQueuedEvent<CollisionChainPlayerData> mCollisionChainPlayerEvent;
    UnidentifiedQueuedEvent<CollisionWindDebrisPlayerData> mCollisionWindDebrisPlayerEvent;
    UnidentifiedQueuedEvent<CollisionExplosionFragmentPlayerData> mCollisionExplosionFragmentPlayerEvent;
    UnidentifiedQueuedEvent<ShotAtGoalData> mChainNisStartEvent;
    UnidentifiedQueuedEvent<ShotAtGoalData> mChainNisEndEvent;
    UnidentifiedQueuedEvent<PenaltyData> mPenaltyEvent;
    UnidentifiedQueuedEvent<UnidentifiedEventNoData> mAwardPowerupStuffEvent;
    ImmediateEvent<MegaStrikeMeterData> mMegaStrikeMeterStartEvent;
    ImmediateEvent<MegaStrikeMeterData> mMegaStrikeMeterFirstEvent;
    ImmediateEvent<MegaStrikeMeterData> mMegaStrikeMeterSecondEvent;
    ImmediateEvent<UnidentifiedEventNoData> mMegaStrikeMeterEndEvent;
    UnidentifiedQueuedEvent<LightningStrikeData> mLightningStrikeEvent;
    UnidentifiedQueuedEvent<UnidentifiedEventNoData> mMegaStrikeStartEvent;
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
    UnidentifiedQueuedEvent<UnidentifiedEventNoData> mStatsPowerupHitDataEvent;
    UnidentifiedQueuedEvent<UnidentifiedEventNoData> mCameraRumbleStartEvent;
    UnidentifiedQueuedEvent<UnidentifiedEventNoData> mCameraRumbleEndEvent;
    UnidentifiedQueuedEvent<ExplodableExplodeData> mExplodableExplodeEvent;
    UnidentifiedQueuedEvent<UnidentifiedEventNoData> mExplodableExplosionEndEvent;
    UnidentifiedQueuedEvent<UnidentifiedEventNoData> mSilenceAllSoundsEvent;
    ImmediateEvent<CharacterImpactEvent> mMontyReappearEvent;
    ImmediateEvent<CharacterImpactEvent> mHammerBroHammerEvent;
    ImmediateEvent<CharacterImpactEvent> mWarioGroundPoundEvent;
    UnidentifiedQueuedEvent<PowerupAcquireData> mPowerupAcquireEvent;
};

#endif // GAME_GAME_EVENT_QUEUE_H
