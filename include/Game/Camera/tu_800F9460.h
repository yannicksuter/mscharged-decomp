#ifndef GAME_CAMERA_TU_800F9460_H
#define GAME_CAMERA_TU_800F9460_H

#include "NL/nlMath.h"
#include "NL/nlSingleton.h"
#include "types.h"

class GameplayCamera;
class cFielder;
struct UnidentifiedEventData00;
struct UnidentifiedEventData32;
struct UnidentifiedEventData_80065F14;
struct UnidentifiedEventData_8006649C;
struct UnidentifiedEventData_80066748;
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

    void OnGoalScored(UnidentifiedEventData_80065F14* eventData);
    void ResetForPresentation(void* context);
    void OnShotPresentation(UnidentifiedEventData00* eventData);
    void OnShotPresentationEnd(UnidentifiedEventData00* eventData);
    void OnCaptainClashPresentation(UnidentifiedEventData00* eventData);
    void OnCaptainClashPresentationEnd(UnidentifiedEventData00* eventData);
    void OnWindupPresentation(UnidentifiedEventData00* eventData);
    void OnWindupPresentationEnd(UnidentifiedEventData00* eventData);
    void OnMegaStrikeMeterStart(
        UnidentifiedEventData_8006701C* eventData);
    void OnMegaStrikeMeterEnd(UnidentifiedEventData00* eventData);
    void OnGoalieSave(UnidentifiedEventData_8006649C* eventData);
    void OnCollisionThwompPlayer(UnidentifiedEventData32* eventData);
    void OnGoalieDekeAttackAttempt(
        UnidentifiedEventData_80066748* eventData);
    void OnGoalieDekeAttackSuccess(
        UnidentifiedEventData_80066748* eventData);
    void OnGoalieSlamAttackAttempt(
        UnidentifiedEventData_80066748* eventData);
    void OnGoalieSlamAttackSuccess(
        UnidentifiedEventData_80066748* eventData);

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
