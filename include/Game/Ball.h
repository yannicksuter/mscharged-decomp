#ifndef GAME_BALL_H
#define GAME_BALL_H

#include "NL/nlMath.h"
#include "NL/nlTimer.h"

#include "Game/CharacterTriggers.h"

enum eSpinType
{
    SPINTYPE_NONE = 0,
    SPINTYPE_BACK = 1,
    SPINTYPE_FORWARD = 2,
    SPINTYPE_ROLLING = 3,
    SPINTYPE_PARAMETER = 4,
};

class cFielder;
class cPlayer;
class BlurHandler;
class DebugWriteCache;
class DrawableObject;
class PhysicsAIBall;
class RunningChecksum;
struct UnidentifiedBallRuntime;

class cBall
{
public:
    cBall();
    ~cBall();

    void ClearOwner();
    void ClearBallEffects();
    void CollideWithCharacterCallback(
        cPlayer* pCharacter, const nlVector3& v3PreBallVelocity);
    void PostPhysicsUpdate(float fDeltaT);
    void UpdateOrientation(float fDeltaT);
    void WarpTo(const nlVector3& toPos);
    void SetPassTarget(cPlayer* passTargetPlayer, const nlVector3& pos,
        bool bVolley);
    void SetPassTargetTimer(float seconds);
    float PredictLandingSpotAndTime(nlVector3& v3Dest,
        int* pNumSolutions, float* pTimes, float fHeight);
    void KillBlurHandler();
    void ClearBallBlur();
    void SetOwner(cPlayer* pOwner);
    void SetPosition(const nlVector3& pos);
    void SetVelocity(const nlVector3& velocity, eSpinType spin,
        const nlVector3* pAngularVelocity);
    void Shoot(cPlayer* pShooter, const nlVector3& v3Dir,
        const nlVector3& v3Spin, eSpinType spinType, int nBallState,
        bool bParam6);
    void ShootRelease(const nlVector3& v3Velocity, eSpinType SpinType);
    void ShootAtFast(nlVector3& v3Vel, const nlVector3& v3Target,
        float fDesiredTime);
    void Update(float fDeltaT);
    void SyncLog(void* context, DebugWriteCache* cache);
    void fn_8001A898(RunningChecksum* runningChecksum);
    nlVector3* GetAIVelocity() const;
    nlVector3* GetDrawablePosition() const;
    float fn_80014F38(float fScale) const;
    cFielder* GetOwnerFielder();
    cPlayer* GetOwnerGoalie();
    cFielder* GetPassTargetFielder() const;
    void InitiateBallBlur(
        eBallShotEffectType effectType, cPlayer* pPlayer);
    bool GetInNet(int& nSide);

    /* 0x00 */ bool m_bVisible;
    /* 0x01 */ u8 mPadding001[0x03];
    /* 0x04 */ u32 m_bBallPathChangeCount;
    /* 0x08 */ u32 m_bBallDeflectCount;
    /* 0x0C */ Timer m_tLightningTimer;
    /* 0x14 */ Timer m_tShotTimer;
    /* 0x1C */ Timer m_tNoPickupTimer;
    /* 0x24 */ Timer m_tPassTargetTimer;
    /* 0x2C */ Timer mtStuckInRiotTimer;
    /* 0x34 */ Timer mtNoChargeLossTimer;
    /* 0x3C */ Timer mtShotClockTimer;
    /* 0x44 */ int mnShotClockTeam;
    /* 0x48 */ bool mbStuckInRiotDone;
    /* 0x49 */ bool mbBallOnFire;
    /* 0x4A */ bool mbBallFrozen;
    /* 0x4B */ u8 mPadding04B;
    /* 0x4C */ float m_fTotalPassTime;
    /* 0x50 */ int m_iConsecutiveVolleyPasses;
    /* 0x54 */ nlVector3 m_v3Position;
    /* 0x60 */ nlVector3 m_v3PrevPosition;
    /* 0x6C */ nlVector3 m_v3Velocity;
    /* 0x78 */ nlVector3 m_v3PassIntercept;
    /* 0x84 */ nlQuaternion m_qOrientation;
    /* 0x94 */ nlVector3 m_v3ShotTarget;
    /* 0xA0 */ nlVector3 m_v3ShotOrigin;
    /* 0xAC */ unsigned int m_uGoalType;
    /* 0xB0 */ unsigned int m_uVoiceID;
    /* 0xB4 */ unsigned int m_CurrentGlowEffect;
    /* 0xB8 */ int meBallState;
    /* 0xBC */ int mePrevBallState;
    /* 0xC0 */ float mfChargeValue;
    /* 0xC4 */ float mfSkillShotTime;
    /* 0xC8 */ cPlayer* m_pOwner;
    /* 0xCC */ cPlayer* m_pPrevOwner;
    /* 0xD0 */ cPlayer* m_pLastTouch;
    /* 0xD4 */ cPlayer* m_pPassTarget;
    /* 0xD8 */ cPlayer* m_pShooter;
    /* 0xDC */ cPlayer* mpDamageTarget;
    /* 0xE0 */ BlurHandler* m_pBlurHandler;
    /* 0xE4 */ DrawableObject* m_pDrawableBall;
    /* 0xE8 */ PhysicsAIBall* m_pPhysicsBall;
    /* 0xEC */ UnidentifiedBallRuntime* mUnidentifiedEC;
    /* 0xF0 */ unsigned long mUnidentifiedF0;
}; // total size: 0xF4

extern "C" void fn_80015C38(cBall* pBall, int nBallState);
extern cBall* g_pBall;

#endif // GAME_BALL_H
