#ifndef GAME_CAMERA_TU_800F9460_H
#define GAME_CAMERA_TU_800F9460_H

#include "NL/nlMath.h"
#include "NL/nlSingleton.h"
#include "types.h"

class GameplayCamera;
class cFielder;
struct CollisionThwompPlayerData;
struct GoalScoredData;
struct UnidentifiedEventData_8006649C;
struct PlayerAttackData;
struct UnidentifiedEventData_8006701C;

class UnidentifiedCameraEffects
    : public nlSingleton<UnidentifiedCameraEffects>
{
public:
    UnidentifiedCameraEffects();

    void RegisterEventListeners();
    void Update(float deltaTime);
    void UnidentifiedNoOp();
    void Reset();
    void UpdateCameraFlags();
    bool AreFieldersClear() const;
    bool IsPassTargetClear() const;
    float CalculateZoomScale(float deltaTime) const;
    void UpdateTransition(float deltaTime);
    bool IsTransitionActive() const;
    nlVector3 RotateCameraVector(const nlVector3& vector) const;
    void AdjustCameraVectors(
        float deltaTime, nlVector3* camera, nlVector3* target) const;
    nlVector3 CalculateTargetOffset(const GameplayCamera* camera) const;

    void OnGoalScored(GoalScoredData* eventData);
    void ResetForPresentation(void* context);
    void OnShotPresentation();
    void OnShotPresentationEnd();
    void OnCaptainClashPresentation();
    void OnCaptainClashPresentationEnd();
    void OnWindupPresentation();
    void OnWindupPresentationEnd();
    void OnMegaStrikeMeterStart(
        UnidentifiedEventData_8006701C* eventData);
    void OnMegaStrikeMeterEnd();
    void OnGoalieSave(UnidentifiedEventData_8006649C* eventData);
    void OnCollisionThwompPlayer(CollisionThwompPlayerData* eventData);
    void OnGoalieDekeAttackAttempt(
        PlayerAttackData* eventData);
    void OnGoalieDekeAttackSuccess(
        PlayerAttackData* eventData);
    void OnGoalieSlamAttackAttempt(
        PlayerAttackData* eventData);
    void OnGoalieSlamAttackSuccess(
        PlayerAttackData* eventData);

    /* 0x00 */ u32 mCameraFlags;
    /* 0x04 */ float mFlagUpdateTimer;
    /* 0x08 */ float mTransitionBlend;
    /* 0x0C */ float mZoomStart;
    /* 0x10 */ bool mRotateCamera;
    /* 0x11 */ bool mTrackSecondaryPlayer;
    /* 0x12 */ u8 mPadding012[2];
    /* 0x14 */ float mRotationDegrees;
    /* 0x18 */ float mTransitionTime;
    /* 0x1C */ float mTransitionInTime;
    /* 0x20 */ float mTransitionOutTime;
    /* 0x24 */ float mTransitionHoldTime;
    /* 0x28 */ bool mOwnsTimeScale;
    /* 0x29 */ bool mRestoreTimeScale;
    /* 0x2A */ bool mUseRealTime;
    /* 0x2B */ u8 mPadding02B;
    /* 0x2C */ cFielder* mPrimaryPlayer;
    /* 0x30 */ cFielder* mSecondaryPlayer;
    /* 0x34 */ float mZoomScale;
    /* 0x38 */ float mTransitionScale;
}; // total size: 0x3C

#endif // GAME_CAMERA_TU_800F9460_H
