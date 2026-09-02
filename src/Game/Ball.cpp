#include <stddef.h>

#include "Game/Ball.h"

#include "Game/AI/DesireReceivePass.h"
#include "Game/AI/Fielder.h"
#include "Game/BallTrail.h"
#include "Game/CharacterTweaks.h"
#include "Game/Drawable/DrawableObj.h"
#include "Game/Effects/EmissionManager.h"
#include "Game/Game.h"
#include "Game/GameInfo.h"
#include "Game/Net.h"
#include "Game/ObjectBlur.h"
#include "Game/Physics/PhysicsAIBall.h"
#include "Game/Physics/PhysicsFakeBall.h"
#include "Game/Player.h"
#include "Game/PoseAccumulator.h"
#include "Game/Render/NPCManager.h"
#include "Game/SHierarchy.h"
#include "Game/Sys/audio.h"
#include "Game/Team.h"
#include "NL/nlMain.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"
#include "unclassified/tu_80199E84.h"
#include "unclassified/tu_801A5F10.h"
#include "unclassified/tu_801B6188.h"

struct DebugFieldType
{
    unsigned short size;
    unsigned short unknown;
    void* writer;
};

struct UnidentifiedBallRuntime
{
    u8 mUnidentified00[0x04];
    nlVector3* m_pPosition;
    u8 mUnidentified08[0x14];
    unsigned int mUnidentified1C;
    u8 mUnidentified20[0x24];
    unsigned int mUnidentified44;
};

struct UnidentifiedGameState
{
    u8 mUnidentified00[0x28];
    u32 mUnidentified28;
    u32 mUnidentified2C;
    u32 mUnidentified30;
    u8 mUnidentified34[0x0C];
    bool mUnidentified40;
};

extern "C" DebugFieldType lbl_80533C98[];
extern "C" LiveBallTrail lbl_8056B518[];
extern "C" unsigned int lbl_806E0C10;
extern "C" void fn_80015C38(cBall*, int);
extern "C" float fn_8002BFA8(PlayerTweaks*, float);
extern "C" void fn_80035544(cFielder*);
extern "C" void fn_80036594(cFielder*, cFielder*, int);
extern "C" bool fn_80038538(cFielder*);
extern "C" bool fn_80038660(cFielder*);
extern "C" bool fn_8003886C(cFielder*);
extern "C" void fn_8003CAA8(cFielder*, cPlayer*);
extern "C" PlayerTweaks* fn_8003E6E4(cFielder*);
extern "C" bool fn_8003E74C(cFielder*);
extern "C" float fn_800DEFD4(cFielder*);
extern "C" void fn_800156F8(cBall*, cPlayer*);
extern "C" void fn_8001AA6C(LiveBallTrail*, float);
extern "C" void fn_80096CDC(cPlayer*, cBall*);
extern "C" void fn_80097358(cPlayer*, float);
extern "C" void fn_801B79A4(const char*, int);
extern "C" void fn_801B7A28(cBall*);
extern "C" void fn_802ECC54(void*, void*);
extern "C" DrawableObject* fn_8027725C(unsigned long);
extern "C" UnidentifiedBallRuntime* fn_802ECB68(void*);
extern "C" unsigned short fn_80338EBC(DebugWriteCache*, const char*);
extern "C" void fn_80338F78(DebugWriteCache*);
extern "C" void fn_80338F88(
    DebugWriteCache*, int, unsigned short, unsigned int, const char*);
extern "C" void* fn_8033930C(
    DebugWriteCache*, unsigned short, void*, unsigned int);
extern "C" void fn_80339450(
    DebugWriteCache*, unsigned short, void*, void*);

float Exp(float);

cBall* g_pBall = NULL;

static const nlVector3 v3Zero = { 0.0f, 0.0f, 0.0f };
static nlMatrix3 m3Ident
    = { 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f };

static float lbl_806DB514 = 2.5f;
extern float lbl_806DB510;
extern float lbl_806DB54C;
extern float lbl_806DB578;
extern float lbl_806DB57C;
extern float lbl_806DB580;
extern float lbl_806DB584;
extern float lbl_806DB588;
extern float lbl_806E31C0;
extern float lbl_806E31C4;
extern float lbl_806E31C8;
extern float lbl_806E31CC;
extern float lbl_806E31D0;
extern float lbl_806E31D4;
extern float lbl_806E31D8;
extern float lbl_806E31DC;
extern float lbl_806E31E0;
static unsigned short lbl_806DB5C0 = 0xFFFF;
extern unsigned char lbl_806E0BCC;

cBall::cBall()
    : m_tLightningTimer(0.0f)
    , m_tShotTimer(0.0f)
    , m_tNoPickupTimer(0.0f)
    , m_tPassTargetTimer(0.0f)
    , mtStuckInRiotTimer(0.0f)
    , mtNoChargeLossTimer(0.0f)
    , mtShotClockTimer(0.0f)
{
    m_bVisible = true;
    m_bBallPathChangeCount = 0;
    m_bBallDeflectCount = 0;
    m_fTotalPassTime = 0.0f;
    m_uGoalType = 4;
    m_uVoiceID = 0;
    m_CurrentGlowEffect = 0;
    mfChargeValue = 0.0f;
    mfSkillShotTime = 0.0f;
    meBallState = 0;
    mePrevBallState = 0;
    m_pOwner = NULL;
    m_pPrevOwner = NULL;
    m_pLastTouch = NULL;
    m_pPassTarget = NULL;
    m_pShooter = NULL;
    mpDamageTarget = NULL;
    m_iConsecutiveVolleyPasses = 0;

    m_tNoPickupTimer.SetSeconds(0.0f);
    m_tLightningTimer.SetSeconds(0.0f);
    m_tShotTimer.SetSeconds(0.0f);
    m_tPassTargetTimer.SetSeconds(0.0f);
    mtNoChargeLossTimer.SetSeconds(0.0f);
    mtStuckInRiotTimer.SetSeconds(0.0f);
    mtShotClockTimer.SetSeconds(0.0f);

    mnShotClockTeam = -1;
    mbStuckInRiotDone = false;
    mbBallOnFire = false;
    mbBallFrozen = false;

    m_v3Position.x = 0.0f;
    m_v3Position.y = 0.0f;
    m_v3Position.z = 0.18f;
    m_v3PrevPosition = m_v3Position;
    m_v3PassIntercept.x = 0.0f;
    m_v3PassIntercept.y = 0.0f;
    m_v3PassIntercept.z = 0.0f;
    m_qOrientation.z = 0.0f;
    m_qOrientation.y = 0.0f;
    m_qOrientation.x = 0.0f;
    m_qOrientation.w = 1.0f;
    m_v3Velocity.x = 0.0f;
    m_v3Velocity.y = 0.0f;
    m_v3Velocity.z = 0.0f;
    m_v3ShotTarget.x = 0.0f;
    m_v3ShotTarget.y = 0.0f;
    m_v3ShotTarget.z = 0.0f;
    m_v3ShotOrigin.x = 0.0f;
    m_v3ShotOrigin.y = 0.0f;
    m_v3ShotOrigin.z = 0.0f;

    m_pBlurHandler = NULL;
    mUnidentifiedF0 = 0;
    m_pDrawableBall = fn_8027725C(nlStringHash("gameplay/ball"));

    m_pPhysicsBall = new (8, false) PhysicsAIBall(0.18f);
    m_pPhysicsBall->m_pAIBall = this;
    m_pPhysicsBall->SetPosition(
        m_v3Position, PhysicsObject::WORLD_COORDINATES);
    m_v3ShotOrigin = m_v3Position;
    m_pPhysicsBall->SetLinearVelocity(m_v3Velocity);
    m_pPhysicsBall->SetAngularVelocity(v3Zero);

    mUnidentifiedEC = fn_802ECB68(lbl_806E201C);
    mUnidentifiedEC->mUnidentified44 |= 0x00800000;
    mUnidentifiedEC->m_pPosition = &m_v3Position;
    mUnidentifiedEC->mUnidentified1C |= 0x8000;
}

cBall::~cBall()
{
    fn_80015C38(this, 0);
    ClearBallEffects();

    if (lbl_806E0BCC || GameInfoManager::Instance()->IsRule0x4Equal5())
    {
        mfChargeValue = 0.5f;
    }
    else
    {
        mfChargeValue = 0.0f;
    }

    float fMaxCharge = lbl_806DB510 * 0.5f;
    if (mfChargeValue >= fMaxCharge)
    {
        mfChargeValue = fMaxCharge;
    }
    else if (mfChargeValue < 0.0f)
    {
        mfChargeValue = 0.0f;
    }

    fn_801B7A28(this);
    fn_800EC12C(mUnidentifiedF0, this);
    mUnidentifiedF0 = 0;
    fn_802ECC54(lbl_806E201C, mUnidentifiedEC);
    delete m_pPhysicsBall;
}

void cBall::ClearOwner()
{
    m_pPrevOwner = m_pOwner;
    fn_80096CDC(m_pOwner, NULL);
    m_pOwner = NULL;
    m_pPhysicsBall->EnableCollisions();
    m_v3PrevPosition = m_v3Position;
    m_pPhysicsBall->GetPosition(&m_v3Position);
    m_pPhysicsBall->GetLinearVelocity(&m_v3Velocity);
    ++m_bBallPathChangeCount;
}

void cBall::ClearBallEffects()
{
    if (m_pBlurHandler != NULL)
    {
        m_pBlurHandler->Die(0.5f);
        m_pBlurHandler = NULL;
    }
    fn_801B79A4("skillshot_ball_meteor", 0);
    fn_801B79A4("skillshot_ball_drybones", 0);
    fn_801B79A4("skillshot_ball_boo", 0);
}

void cBall::CollideWithCharacterCallback(
    cPlayer* pCharacter, const nlVector3& v3PreBallVelocity)
{
    bool bCanDamage;
    switch (meBallState)
    {
    case 6:
        bCanDamage = nlSqrt(m_v3Velocity.GetLengthSq3D(), true)
            > lbl_806DB578;
        break;
    case 8:
        bCanDamage = true;
        break;
    default:
        bCanDamage = false;
        break;
    }

    if (bCanDamage && pCharacter->m_eClassType == FIELDER)
    {
        cFielder* pCharacterFielder = (cFielder*)pCharacter;
        nlVector3 v3BallDirection;
        nlVec3Sub(v3BallDirection, m_v3Position,
            m_pPrevOwner->m_v3Position);
        unsigned short aBallDirection
            = (unsigned short)(int)(lbl_806E31C4
                * nlATan2f(v3BallDirection.y, v3BallDirection.x));
        bool bReactToHit = true;
        bool bDeflectBall = true;

        bool bInvincible = !fn_8003886C(pCharacterFielder)
            && (pCharacterFielder->muInvincibleStatus & 4) != 0;
        if (!bInvincible)
        {
            if (pCharacter->m_pBall != NULL)
            {
                pCharacter->ReleaseBall(0);
            }

            cPlayer* pShooter = m_pShooter;
            bool bLightningBall = m_tLightningTimer.m_uPackedTime != 0
                && meBallState == 8 && pShooter != NULL;
            if (bLightningBall
                && pShooter->m_eCharacterClass == (eCharacterClass)0xF)
            {
                fn_80097358(pCharacter, lbl_806E31C8);
            }
            else if (bLightningBall
                && pShooter->m_eCharacterClass == (eCharacterClass)0x11)
            {
                fn_800156F8(this, pShooter);
                pCharacterFielder->fn_800451B0(pShooter->m_v3Position);
                if (fn_801B6278(
                        GameInfoManager::Instance()->GetStadium()))
                {
                    unsigned long soundID = 0xCE269987;
                    if (pCharacter->m_pTeam->m_nSide == 0)
                    {
                        soundID = 0x5089F33E;
                    }
                    fn_800ED92C(soundID);
                }
                bReactToHit = false;
                bDeflectBall = false;
            }
            else if (mbBallOnFire)
            {
                fn_80097358(pCharacter, lbl_806E31C8);
            }

            if (bReactToHit)
            {
                int nReact = 0;
                float fSpeed
                    = nlSqrt(m_v3Velocity.GetLengthSq3D(), true);
                if (fSpeed > lbl_806DB580)
                {
                    nReact = 2;
                }
                else if (fSpeed > lbl_806DB57C)
                {
                    nReact = 1;
                }
                pCharacterFielder->fn_80047240(m_pPrevOwner,
                    aBallDirection, nReact, false, false);
            }
        }
        else
        {
            pCharacter->SetNoPickUpTime(lbl_806E31CC);
        }

        if (bDeflectBall)
        {
            if (m_pOwner != NULL)
            {
                fn_80015C38(this, 2);
            }
            else
            {
                bool bPassTarget = (meBallState == 5
                                       || meBallState == 3)
                    && m_pPassTarget != NULL;
                if (bPassTarget
                    && fn_800DEFD4((cFielder*)m_pPassTarget))
                {
                    cFielder* pFielder;
                    if (m_pPassTarget != NULL
                        && m_pPassTarget->m_eClassType == FIELDER)
                    {
                        pFielder = (cFielder*)m_pPassTarget;
                    }
                    else
                    {
                        pFielder = NULL;
                    }
                    DesireReceivePass* pReceivePass
                        = (DesireReceivePass*)fn_8002E08C(pFielder, 22);
                    if (pReceivePass == NULL
                        || !pReceivePass->UnidentifiedIsActive()
                        || pReceivePass->meDesireSubState != 0)
                    {
                        fn_80015C38(this, 0);
                    }
                }
                else
                {
                    fn_80015C38(this, 0);
                }
            }

            m_tNoPickupTimer.SetSeconds(0.0f);

            nlVector3 v3Velocity;
            if (nlVec3DotProduct(v3BallDirection, m_v3Velocity) > 0.0f)
            {
                nlVec3Scale(v3Velocity, m_v3Velocity, lbl_806E31D0);
                nlVector3 v3CharacterToBall;
                nlVec3Sub(v3CharacterToBall, m_v3Position,
                    pCharacter->m_v3Position);
                if (nlVec3DotProduct(v3CharacterToBall, v3Velocity) < 0.0f)
                {
                    m_v3Position = m_v3PrevPosition;
                    m_pPhysicsBall->SetPosition(m_v3PrevPosition,
                        PhysicsObject::WORLD_COORDINATES);
                    m_pPhysicsBall->SetRotation(m3Ident);
                    FakeBallWorld::InvalidateBallCache();
                    ++m_bBallPathChangeCount;
                }
            }
            else
            {
                nlVec3Scale(v3Velocity, m_v3Velocity, lbl_806E31D4);
            }

            v3Velocity.z += lbl_806E31C0 + nlRandomf(lbl_806E31D8);
            v3Velocity.y += lbl_806E31DC + nlRandomf(lbl_806E31E0);

            nlVector3 v3AngularVelocity;
            m_pPhysicsBall->GetAngularVelocity(&v3AngularVelocity);
            SetVelocity(v3Velocity, SPINTYPE_PARAMETER,
                &v3AngularVelocity);
        }
    }

    if (meBallState == 4 && pCharacter->m_eClassType == FIELDER)
    {
        cFielder* pFielder = (cFielder*)pCharacter;
        if (pFielder->m_eActionState < ACTION_LOOSE_BALL_PASS
            || pFielder->m_eActionState > ACTION_LOOSE_BALL_SHOT)
        {
            if (m_pOwner != NULL)
            {
                fn_80015C38(this, 2);
            }
            else
            {
                bool bPassTarget = (meBallState == 5
                                       || meBallState == 3)
                    && m_pPassTarget != NULL;
                if (bPassTarget
                    && fn_800DEFD4((cFielder*)m_pPassTarget))
                {
                    cFielder* pPassTarget;
                    if (m_pPassTarget != NULL
                        && m_pPassTarget->m_eClassType == FIELDER)
                    {
                        pPassTarget = (cFielder*)m_pPassTarget;
                    }
                    else
                    {
                        pPassTarget = NULL;
                    }
                    DesireReceivePass* pReceivePass
                        = (DesireReceivePass*)fn_8002E08C(
                            pPassTarget, 22);
                    if (pReceivePass == NULL
                        || !pReceivePass->UnidentifiedIsActive()
                        || pReceivePass->meDesireSubState != 0)
                    {
                        fn_80015C38(this, 0);
                    }
                }
                else
                {
                    fn_80015C38(this, 0);
                }
            }
        }
    }

    bool bPassTarget = (meBallState == 5 || meBallState == 3)
        && m_pPassTarget != NULL;
    if (m_tShotTimer.m_uPackedTime != 0 || bPassTarget)
    {
        if (pCharacter->m_eClassType == FIELDER)
        {
            m_uGoalType = 3;
        }

        if (m_pOwner != NULL)
        {
            fn_80015C38(this, 2);
        }
        else
        {
            bool bHasPassTarget = (meBallState == 5
                                      || meBallState == 3)
                && m_pPassTarget != NULL;
            if (bHasPassTarget
                && fn_800DEFD4((cFielder*)m_pPassTarget))
            {
                cFielder* pFielder;
                if (m_pPassTarget != NULL
                    && m_pPassTarget->m_eClassType == FIELDER)
                {
                    pFielder = (cFielder*)m_pPassTarget;
                }
                else
                {
                    pFielder = NULL;
                }
                DesireReceivePass* pReceivePass
                    = (DesireReceivePass*)fn_8002E08C(pFielder, 22);
                if (pReceivePass == NULL
                    || !pReceivePass->UnidentifiedIsActive()
                    || pReceivePass->meDesireSubState != 0)
                {
                    fn_80015C38(this, 0);
                }
            }
            else
            {
                fn_80015C38(this, 0);
            }
        }
    }

    cFielder* pOwnerFielder = (cFielder*)m_pOwner;
    if (pOwnerFielder == NULL
        || pOwnerFielder->m_eClassType != FIELDER)
    {
        pOwnerFielder = NULL;
    }

    if (pOwnerFielder != NULL && pOwnerFielder != pCharacter
        && pCharacter->m_eClassType == FIELDER)
    {
        cFielder* pCharacterFielder = (cFielder*)pCharacter;
        fn_8003CAA8(pOwnerFielder, pCharacter);

        if (!pCharacterFielder->IsOnSameTeam(pOwnerFielder))
        {
            if (fn_80038660(pCharacterFielder))
            {
                nlVector3 v3ContactLocation
                    = pCharacter->m_v3Position;
                nlVector3 v3PhysicsRadialSpot;
                float fRadius = fn_8002BFA8(
                    fn_8003E6E4(pCharacterFielder),
                    pCharacter->mUnidentified0A0);
                nlPolarToCartesian(v3PhysicsRadialSpot.x,
                    v3PhysicsRadialSpot.y,
                    pCharacter->m_aActualFacingDirection, fRadius);
                v3PhysicsRadialSpot.z = 0.0f;
                nlVec3Add(v3ContactLocation, v3ContactLocation,
                    v3PhysicsRadialSpot);

                s16 nHitterContactLocationFacingDelta
                    = pCharacter->GetFacingDeltaToPosition(
                        v3ContactLocation);
                u16 absFacingDelta
                    = nHitterContactLocationFacingDelta < 0
                    ? -nHitterContactLocationFacingDelta
                    : nHitterContactLocationFacingDelta;
                if (absFacingDelta < 0x2000)
                {
                    if (fn_80038660(pOwnerFielder))
                    {
                        s16 nHitteeContactLocationFacingDelta
                            = pOwnerFielder->GetFacingDeltaToPosition(
                                v3ContactLocation);
                        u16 absOwnerFacingDelta
                            = nHitteeContactLocationFacingDelta < 0
                            ? -nHitteeContactLocationFacingDelta
                            : nHitteeContactLocationFacingDelta;
                        if (absOwnerFacingDelta < 0x2000)
                        {
                            if (fn_8003E6E4(pOwnerFielder)
                                    ->mUnidentified064
                                < fn_8003E6E4(pCharacterFielder)
                                      ->mUnidentified064)
                            {
                                pOwnerFielder->InitActionSlideAttackReact(
                                    pCharacterFielder, false);
                                fn_80035544(pCharacterFielder);
                                pCharacterFielder->PickupBall(g_pBall);
                            }
                            else if (fn_8003E6E4(pOwnerFielder)
                                         ->mUnidentified064
                                > fn_8003E6E4(pCharacterFielder)
                                      ->mUnidentified064)
                            {
                                pCharacterFielder
                                    ->InitActionSlideAttackReact(
                                        pOwnerFielder, false);
                                fn_80035544(pOwnerFielder);
                            }
                            else if (pOwnerFielder->m_fActualSpeed
                                < pCharacterFielder->m_fActualSpeed)
                            {
                                pOwnerFielder->InitActionSlideAttackReact(
                                    pCharacterFielder, false);
                                fn_80035544(pCharacterFielder);
                                pCharacterFielder->PickupBall(g_pBall);
                            }
                            else
                            {
                                pCharacterFielder
                                    ->InitActionSlideAttackReact(
                                        pOwnerFielder, false);
                                fn_80035544(pOwnerFielder);
                            }
                        }
                        else
                        {
                            pOwnerFielder->InitActionSlideAttackReact(
                                pCharacterFielder, false);
                            fn_80036594(
                                pCharacterFielder, pOwnerFielder, 2);
                            fn_80035544(pCharacterFielder);
                            if (pCharacterFielder->CanPickupBall(
                                    g_pBall, false))
                            {
                                pCharacterFielder->PickupBall(g_pBall);
                            }
                        }
                    }
                    else
                    {
                        pOwnerFielder->InitActionSlideAttackReact(
                            pCharacterFielder, false);
                        fn_80036594(
                            pCharacterFielder, pOwnerFielder, 2);
                        fn_80035544(pCharacterFielder);
                        if (pCharacterFielder->CanPickupBall(
                                g_pBall, false))
                        {
                            pCharacterFielder->PickupBall(g_pBall);
                        }
                    }
                }
            }
            else if (fn_80038660(pOwnerFielder)
                && !fn_80038538(pCharacterFielder))
            {
                pCharacterFielder->InitActionSlideAttackReact(
                    pOwnerFielder, false);
                fn_80035544(pOwnerFielder);
            }
        }
        else
        {
            if (fn_80038660(pOwnerFielder)
                && !fn_8003E74C(pOwnerFielder))
            {
                bool bInvincible = !fn_8003886C(pOwnerFielder)
                    && (pOwnerFielder->muInvincibleStatus & 1) != 0;
                if (!bInvincible)
                {
                    pOwnerFielder->fn_8004D238();
                }
            }
            if (fn_80038660(pCharacterFielder)
                && !fn_8003E74C(pCharacterFielder))
            {
                bool bInvincible = !fn_8003886C(pCharacterFielder)
                    && (pCharacterFielder->muInvincibleStatus & 1) != 0;
                if (!bInvincible)
                {
                    pCharacterFielder->fn_8004D238();
                }
            }
        }
    }

    if (m_pOwner == NULL)
    {
        m_pLastTouch = pCharacter;
        FakeBallWorld::InvalidateBallCache();
        ++m_bBallDeflectCount;
    }

    ++m_bBallPathChangeCount;

    if (pCharacter->m_eClassType == FIELDER)
    {
        m_v3ShotOrigin = m_v3Position;
    }
}

extern "C" void fn_80014494(cBall* pBall)
{
    if (pBall->m_pOwner != NULL)
    {
        fn_80015C38(pBall, 2);
        return;
    }

    bool bPassTarget = (pBall->meBallState == 5
                           || pBall->meBallState == 3)
                    && pBall->m_pPassTarget != NULL;
    if (bPassTarget)
    {
        if (fn_800DEFD4((cFielder*)pBall->m_pPassTarget))
        {
            cPlayer* pPassTarget = pBall->m_pPassTarget;
            cFielder* pFielder;
            if (pPassTarget != NULL
                && pPassTarget->m_eClassType == FIELDER)
            {
                pFielder = (cFielder*)pPassTarget;
            }
            else
            {
                pFielder = NULL;
            }

            DesireReceivePass* pReceivePass
                = (DesireReceivePass*)fn_8002E08C(pFielder, 22);
            if (pReceivePass != NULL
                && pReceivePass->UnidentifiedIsActive()
                && pReceivePass->meDesireSubState == 0)
            {
                return;
            }

            fn_80015C38(pBall, 0);
            return;
        }
    }

    fn_80015C38(pBall, 0);
}

extern "C" void fn_8001458C(cBall* pBall)
{
    if (pBall->meBallState != 10)
    {
        fn_80015C38(pBall, 10);
    }
}

extern "C" void fn_800145A4(cBall* pBall)
{
    if (pBall->m_tNoPickupTimer.m_uPackedTime != 0)
    {
        return;
    }

    if (pBall->meBallState == 4)
    {
        return;
    }

    if (pBall->m_pOwner != NULL)
    {
        fn_80015C38(pBall, 2);
        return;
    }

    bool bPassTarget = (pBall->meBallState == 5
                           || pBall->meBallState == 3)
                    && pBall->m_pPassTarget != NULL;
    if (bPassTarget)
    {
        if (fn_800DEFD4((cFielder*)pBall->m_pPassTarget))
        {
            cPlayer* pPassTarget = pBall->m_pPassTarget;
            cFielder* pFielder;
            if (pPassTarget != NULL
                && pPassTarget->m_eClassType == FIELDER)
            {
                pFielder = (cFielder*)pPassTarget;
            }
            else
            {
                pFielder = NULL;
            }

            DesireReceivePass* pReceivePass
                = (DesireReceivePass*)fn_8002E08C(pFielder, 22);
            if (pReceivePass != NULL
                && pReceivePass->UnidentifiedIsActive()
                && pReceivePass->meDesireSubState == 0)
            {
                return;
            }

            fn_80015C38(pBall, 0);
            return;
        }
    }

    fn_80015C38(pBall, 0);
}

static inline float clampAbove(float minVal, float x)
{
    if (minVal >= x)
    {
        return minVal;
    }
    return x;
}

void cBall::PostPhysicsUpdate(float fDeltaT)
{
    m_v3PrevPosition = m_v3Position;
    m_pPhysicsBall->GetPosition(&m_v3Position);
    m_pPhysicsBall->GetLinearVelocity(&m_v3Velocity);

    bool bCanDamage;
    switch (meBallState)
    {
    case 6:
        if (nlSqrt(m_v3Velocity.GetLengthSq3D(), true)
            > lbl_806DB578)
        {
            bCanDamage = true;
        }
        else
        {
            bCanDamage = false;
        }
        break;
    case 8:
        bCanDamage = true;
        break;
    default:
        bCanDamage = false;
        break;
    }

    if (bCanDamage && mpDamageTarget != NULL)
    {
        nlVector3 v3HitSpot;
        nlVector3 v3CurPos;
        nlVector3 targetDelta;
        nlVector3 currentDelta;
        nlVector3 v3PrevPos;
        float fPercent;
        nlVector3 v3BallVel;
        float fPrevZVel;

        v3HitSpot = mpDamageTarget->GetJointPosition(
            mpDamageTarget->m_pPoseAccumulator->m_pHierarchy
                ->m_nPelvisNodeIndex);
        v3HitSpot.z = clampAbove(0.3f, v3HitSpot.z + 0.05f);

        v3CurPos = m_v3Position;
        v3PrevPos = m_v3PrevPosition;

        if (v3CurPos.z < 0.3f)
        {
            v3CurPos.z = 0.3f;
        }

        if (v3PrevPos.z < 0.3f)
        {
            v3PrevPos.z = 0.3f;
        }

        nlVec3Set(targetDelta, v3HitSpot.x - v3PrevPos.x,
            v3HitSpot.y - v3PrevPos.y, v3HitSpot.z - v3PrevPos.z);
        nlVec3Set(currentDelta, v3CurPos.x - v3PrevPos.x,
            v3CurPos.y - v3PrevPos.y, v3CurPos.z - v3PrevPos.z);

        float targetDist = nlSqrt(targetDelta.GetLengthSq3D(), true);
        float currentDist = nlSqrt(currentDelta.GetLengthSq3D(), true);

        fPercent = 0.5f;
        if (targetDist < currentDist)
        {
            nlVec3Scale(currentDelta, targetDist / targetDist);
        }
        else
        {
            nlVec3Scale(targetDelta, currentDist / targetDist);
        }

        if (targetDist < 5.0f)
        {
            fPercent += 0.5f * (1.0f - targetDist / 5.0f);
        }

        nlVecLerp(currentDelta, currentDelta, targetDelta, fPercent);
        nlVec3Add(v3CurPos, v3PrevPos, currentDelta);

        m_v3Position = v3CurPos;
        m_pPhysicsBall->SetPosition(
            v3CurPos, PhysicsObject::WORLD_COORDINATES);
        m_pPhysicsBall->SetRotation(m3Ident);

        FakeBallWorld::InvalidateBallCache();
        m_bBallPathChangeCount = m_bBallPathChangeCount + 1;

        fPrevZVel = m_v3Velocity.z;
        const nlVector3& ballVelocity = m_v3Velocity;
        float distanceSq = currentDelta.GetLengthSq3D();
        float projectedScale
            = nlVec3DotProduct(ballVelocity, currentDelta) / distanceSq;
        nlVec3Scale(v3BallVel, currentDelta, projectedScale);
        v3BallVel.z = fPrevZVel;

        float speedSq = v3BallVel.GetLengthSq3D();
        if (speedSq < 400.0f)
        {
            float speed = nlSqrt(speedSq, true);
            nlVec3Scale(v3BallVel, 20.0f / speed);
        }

        if (v3CurPos.z < 0.4f && v3BallVel.z < 0.0f)
        {
            v3BallVel.z = 0.0f;
        }

        m_v3Velocity = v3BallVel;
        m_pPhysicsBall->SetLinearVelocity(v3BallVel);
    }

    UpdateOrientation(fDeltaT);

    bool bUnidentified = m_tLightningTimer.m_uPackedTime != 0
        && meBallState == 8 && m_pShooter != NULL
        && m_pShooter->m_eCharacterClass == (eCharacterClass)0x10;
    if (bUnidentified)
    {
        cFielder* pFielder = (cFielder*)m_pShooter;
        if (pFielder->m_eActionState == (eFielderActionState)0x21
            && pFielder->mUnidentified41C)
        {
            nlVector3 v3JointPosition = pFielder->GetJointPosition(
                pFielder->m_nBip01JointIndex_0xA4);
            nlVector3 v3Delta;
            v3Delta.x = v3JointPosition.x - pFielder->m_v3Position.x;
            v3Delta.y = v3JointPosition.y - pFielder->m_v3Position.y;
            float fDistance
                = nlSqrt(v3Delta.x * v3Delta.x + v3Delta.y * v3Delta.y,
                    true);
            float fHeight
                = pFielder->m_v3Position.z - v3JointPosition.z;

            nlVector3 v3Velocity = m_v3Velocity;
            pFielder->SetFacingDirection(
                (unsigned short)(int)(10430.378f
                    * nlATan2f(v3Velocity.y, v3Velocity.x)),
                true);
            pFielder->SetVelocity(v3Velocity);

            v3Velocity.z = 0.0f;
            float fRecipLength
                = nlRecipSqrt(v3Velocity.GetLengthSq3D(), true);
            nlVec3Scale(v3Velocity, v3Velocity, fRecipLength);

            nlVector3 v3Position;
            nlVec3ScaleAdd(
                v3Position, -fDistance, v3Velocity, m_v3Position);
            v3Position.z += fHeight;
            if (v3Position.z < 0.0f)
            {
                v3Position.z = 0.0f;
            }
            pFielder->SetPosition(v3Position);
        }
    }

    if (m_pBlurHandler != NULL)
    {
        m_pBlurHandler->AddViewOrientedPoint(
            m_v3Position, m_v3Velocity);
    }

    KoopaShellObject* pKoopaShell = lbl_806E1608->mUnidentified02C;
    if (pKoopaShell != NULL && pKoopaShell->mVisible)
    {
        pKoopaShell->mVelocity = m_v3Velocity;
        fn_801A65D0(pKoopaShell, m_v3Position);
    }

    State_80199E84* pState = lbl_806E1608->mUnidentified028;
    if (pState != NULL && pState->visible)
    {
        pState->unknown_40 = m_v3Velocity;
        fn_8019A248(pState, m_v3Position);
    }
}

extern "C" bool fn_80014D38(cBall* pBall)
{
    bool bPassLockedIn = false;
    bool bPassTarget = (pBall->meBallState == 5
                           || pBall->meBallState == 3)
                    && pBall->m_pPassTarget != NULL;
    if (bPassTarget)
    {
        cPlayer* pPassTarget = pBall->m_pPassTarget;
        cFielder* pFielder;
        if (pPassTarget != NULL
            && pPassTarget->m_eClassType == FIELDER)
        {
            pFielder = (cFielder*)pPassTarget;
        }
        else
        {
            pFielder = NULL;
        }
        DesireReceivePass* pReceivePass
            = (DesireReceivePass*)fn_8002E08C(pFielder, 22);
        if (pReceivePass != NULL
            && pReceivePass->UnidentifiedIsActive())
        {
            bPassLockedIn = pReceivePass->meDesireSubState != 0;
        }
    }

    if (bPassLockedIn != pBall->m_pPhysicsBall->mbPassLockedIn)
    {
        pBall->m_pPhysicsBall->mbPassLockedIn = bPassLockedIn;
        FakeBallWorld::InvalidateBallCache();
        ++pBall->m_bBallPathChangeCount;
    }
    return bPassLockedIn;
}

extern "C" bool fn_80014E20(cBall* pBall)
{
    switch (pBall->meBallState)
    {
    case 6:
        return nlSqrt(pBall->m_v3Velocity.GetLengthSq3D(), true)
            > lbl_806DB578;
    case 8:
        return true;
    default:
        return false;
    }
}

extern "C" bool fn_80014EA4(
    cBall* pBall, const EffectsGroup* pEffectsGroup)
{
    return EmissionManager::Instance()->IsPlaying(
        (unsigned long)pBall, pEffectsGroup);
}

extern "C" float fn_800156A8(cBall* pBall)
{
    float fParam = pBall->mfChargeValue - 1.0f;
    float fResult = 0.0f;
    if (fParam > 0.0f)
    {
        fResult = fParam / 3.0f;
    }

    fResult = fResult >= 0.0f ? fResult : 0.0f;
    return fResult <= 1.0f ? fResult : 1.0f;
}

void cBall::ClearBallBlur()
{
    if (m_pBlurHandler != NULL)
    {
        m_pBlurHandler->Die(0.5f);
        m_pBlurHandler = NULL;
    }
}

extern "C" bool fn_80016768(cBall* pBall)
{
    return pBall->m_tLightningTimer.m_uPackedTime != 0
        && pBall->meBallState == 8 && pBall->m_pShooter != NULL
        && pBall->m_pShooter->m_eCharacterClass
        == (eCharacterClass)0x11;
}

extern "C" bool fn_800167A8(cBall* pBall)
{
    return pBall->m_tLightningTimer.m_uPackedTime != 0
        && pBall->meBallState == 8 && pBall->m_pShooter != NULL
        && pBall->m_pShooter->m_eCharacterClass
        == (eCharacterClass)0x10;
}

extern "C" bool fn_800167E8(cBall* pBall)
{
    return pBall->m_tLightningTimer.m_uPackedTime != 0;
}

void cBall::SetOwner(cPlayer* pOwner)
{
    m_pOwner = pOwner;
    fn_80096CDC(pOwner, this);
    m_pLastTouch = pOwner;
    fn_80015C38(this, 2);

    if (pOwner->m_eClassType != GOALIE)
    {
        g_pGame->SetPotentialScorer(pOwner);
    }

    m_pPhysicsBall->mbUseMagnusEffect = false;
    m_pPhysicsBall->mfChargeBonus = 0.0f;
}

void cBall::SetPosition(const nlVector3& pos)
{
    m_v3Position = pos;
    m_pPhysicsBall->SetPosition(pos, PhysicsObject::WORLD_COORDINATES);
    m_pPhysicsBall->SetRotation(m3Ident);
    FakeBallWorld::InvalidateBallCache();
    ++m_bBallPathChangeCount;
}

void cBall::ShootRelease(const nlVector3& v3Velocity, eSpinType SpinType)
{
    SetVelocity(v3Velocity, SpinType, NULL);
    m_tNoPickupTimer.SetSeconds(0.1f);
    PhysicsBall* pPhysicsBall = m_pPhysicsBall;
    pPhysicsBall->mbUseMagnusEffect = false;
    pPhysicsBall->mfChargeBonus = 0.0f;
}

void cBall::ShootAtFast(nlVector3& v3Vel, const nlVector3& v3Target,
    float fDesiredTime)
{
    float k = lbl_806DB584 * m_pPhysicsBall->mfBallAirResistance;
    float g = lbl_806DB588 * m_pPhysicsBall->m_gravity;
    float eToTheNegativeKT = Exp(-k * fDesiredTime);
    float kSquaredOverOneMinusEToTheNegativeKT
        = (k * k) / (1.0f - eToTheNegativeKT);
    float oneOverK = 1.0f / k;

    v3Vel.x = kSquaredOverOneMinusEToTheNegativeKT
        * (oneOverK * (v3Target.x - m_v3Position.x));
    v3Vel.y = kSquaredOverOneMinusEToTheNegativeKT
        * (oneOverK * (v3Target.y - m_v3Position.y));
    v3Vel.z = kSquaredOverOneMinusEToTheNegativeKT
            * (oneOverK
                * (v3Target.z - m_v3Position.z - g * fDesiredTime / k))
        + g / k;
}

void cBall::WarpTo(const nlVector3& toPos)
{
    m_v3Position = toPos;
    m_pPhysicsBall->SetPosition(toPos, PhysicsObject::WORLD_COORDINATES);
    m_pPhysicsBall->SetRotation(m3Ident);
    FakeBallWorld::InvalidateBallCache();
    m_bBallPathChangeCount = m_bBallPathChangeCount + 1;
    m_v3PrevPosition = toPos;
}

void cBall::SetPassTarget(
    cPlayer* passTargetPlayer, const nlVector3& pos, bool bVolley)
{
    m_pPassTarget = passTargetPlayer;
    m_v3PassIntercept = pos;
}

void cBall::SetPassTargetTimer(float seconds)
{
    m_tPassTargetTimer.SetSeconds(seconds);
    if (m_fTotalPassTime == 0.0f)
    {
        m_fTotalPassTime = seconds;
    }
}

void cBall::KillBlurHandler()
{
    if (m_pBlurHandler != NULL)
    {
        m_pBlurHandler->Die(0.f);
        m_pBlurHandler = NULL;
    }
}

extern "C" void fn_800189C4(cBall* pBall)
{
    fn_800EC12C(pBall->mUnidentifiedF0, pBall);
    pBall->mUnidentifiedF0 = 0;
}

void cBall::SyncLog(void* context, DebugWriteCache* cache)
{
    if (lbl_806DB5C0 == 0xFFFF)
    {
        lbl_806DB5C0 = fn_80338EBC(cache, "DetBall");
        fn_80338F88(cache, 16, lbl_80533C98[16].size, 0,
            "m_bVisible");
        fn_80338F88(cache, 9, lbl_80533C98[9].size,
            (u8*)&m_bBallPathChangeCount - (u8*)this,
            "m_bBallPathChangeCount");
        fn_80338F88(cache, 9, lbl_80533C98[9].size,
            (u8*)&m_bBallDeflectCount - (u8*)this,
            "m_bBallDeflectCount");
        fn_80338F88(cache, 20, lbl_80533C98[20].size,
            (u8*)&m_tShotTimer - (u8*)this, "m_tShotTimer");
        fn_80338F88(cache, 20, lbl_80533C98[20].size,
            (u8*)&m_tLightningTimer - (u8*)this,
            "m_tLightningTimer");
        fn_80338F88(cache, 20, lbl_80533C98[20].size,
            (u8*)&m_tNoPickupTimer - (u8*)this,
            "m_tNoPickupTimer");
        fn_80338F88(cache, 20, lbl_80533C98[20].size,
            (u8*)&m_tPassTargetTimer - (u8*)this,
            "m_tPassTargetTimer");
        fn_80338F88(cache, 20, lbl_80533C98[20].size,
            (u8*)&mtNoChargeLossTimer - (u8*)this,
            "mtNoChargeLossTimer");
        fn_80338F88(cache, 20, lbl_80533C98[20].size,
            (u8*)&mtStuckInRiotTimer - (u8*)this,
            "mtStuckInRiotTimer");
        fn_80338F88(cache, 20, lbl_80533C98[20].size,
            (u8*)&mtShotClockTimer - (u8*)this,
            "mtShotClockTimer");
        fn_80338F88(cache, 8, lbl_80533C98[8].size,
            (u8*)&mnShotClockTeam - (u8*)this, "mnShotClockTeam");
        fn_80338F88(cache, 16, lbl_80533C98[16].size,
            (u8*)&mbStuckInRiotDone - (u8*)this,
            "mbStuckInRiotDone");
        fn_80338F88(cache, 16, lbl_80533C98[16].size,
            (u8*)&mbBallOnFire - (u8*)this, "mbBallOnFire");
        fn_80338F88(cache, 16, lbl_80533C98[16].size,
            (u8*)&mbBallFrozen - (u8*)this, "mbBallFrozen");
        fn_80338F88(cache, 17, lbl_80533C98[17].size,
            (u8*)&m_fTotalPassTime - (u8*)this, "m_fTotalPassTime");
        fn_80338F88(cache, 8, lbl_80533C98[8].size,
            (u8*)&m_iConsecutiveVolleyPasses - (u8*)this,
            "m_iConsecutiveVolleyPasses");
        fn_80338F88(cache, 22, lbl_80533C98[22].size,
            (u8*)&m_v3Position - (u8*)this, "m_v3Position");
        fn_80338F88(cache, 22, lbl_80533C98[22].size,
            (u8*)&m_v3PrevPosition - (u8*)this, "m_v3PrevPosition");
        fn_80338F88(cache, 22, lbl_80533C98[22].size,
            (u8*)&m_v3Velocity - (u8*)this, "m_v3Velocity");
        fn_80338F88(cache, 22, lbl_80533C98[22].size,
            (u8*)&m_v3PassIntercept - (u8*)this, "m_v3PassIntercept");
        fn_80338F88(cache, 24, lbl_80533C98[24].size,
            (u8*)&m_qOrientation - (u8*)this, "m_qOrientation");
        fn_80338F88(cache, 22, lbl_80533C98[22].size,
            (u8*)&m_v3ShotTarget - (u8*)this, "m_v3ShotTarget");
        fn_80338F88(cache, 22, lbl_80533C98[22].size,
            (u8*)&m_v3ShotOrigin - (u8*)this, "m_v3ShotOrigin");
        fn_80338F88(cache, 2, lbl_80533C98[2].size,
            (u8*)&m_uGoalType - (u8*)this, "m_uGoalType");
        fn_80338F88(cache, 2, lbl_80533C98[2].size,
            (u8*)&m_uVoiceID - (u8*)this, "m_uVoiceID");
        fn_80338F88(cache, 2, lbl_80533C98[2].size,
            (u8*)&m_CurrentGlowEffect - (u8*)this,
            "m_CurrentGlowEffect");
        fn_80338F88(cache, 17, lbl_80533C98[17].size,
            (u8*)&mfChargeValue - (u8*)this, "mfChargeValue");
        fn_80338F88(cache, 17, lbl_80533C98[17].size,
            (u8*)&mfSkillShotTime - (u8*)this, "mfSkillShotTime");
        fn_80338F88(cache, 14, lbl_80533C98[14].size,
            (u8*)&meBallState - (u8*)this, "meBallState");
        fn_80338F88(cache, 14, lbl_80533C98[14].size,
            (u8*)&mePrevBallState - (u8*)this, "mePrevBallState");
        fn_80338F88(cache, 15, lbl_80533C98[15].size,
            (u8*)&m_pOwner - (u8*)this, "m_pOwner");
        fn_80338F88(cache, 15, lbl_80533C98[15].size,
            (u8*)&m_pPrevOwner - (u8*)this, "m_pPrevOwner");
        fn_80338F88(cache, 15, lbl_80533C98[15].size,
            (u8*)&m_pLastTouch - (u8*)this, "m_pLastTouch");
        fn_80338F88(cache, 15, lbl_80533C98[15].size,
            (u8*)&m_pPassTarget - (u8*)this, "m_pPassTarget");
        fn_80338F88(cache, 15, lbl_80533C98[15].size,
            (u8*)&m_pShooter - (u8*)this, "m_pShooter");
        fn_80338F88(cache, 15, lbl_80533C98[15].size,
            (u8*)&mpDamageTarget - (u8*)this, "mpDamageTarget");
        fn_80338F78(cache);
    }

    cBall* copy = (cBall*)fn_8033930C(
        cache, lbl_806DB5C0, this, offsetof(cBall, m_pBlurHandler));
    if (copy != NULL)
    {
        *(int*)&copy->m_pOwner
            = m_pOwner == NULL ? -1 : m_pOwner->mUnidentified120;
        *(int*)&copy->m_pPrevOwner
            = m_pPrevOwner == NULL ? -1 : m_pPrevOwner->mUnidentified120;
        *(int*)&copy->m_pLastTouch
            = m_pLastTouch == NULL ? -1 : m_pLastTouch->mUnidentified120;
        *(int*)&copy->m_pPassTarget
            = m_pPassTarget == NULL ? -1 : m_pPassTarget->mUnidentified120;
        *(int*)&copy->m_pShooter
            = m_pShooter == NULL ? -1 : m_pShooter->mUnidentified120;
        *(int*)&copy->mpDamageTarget
            = mpDamageTarget == NULL ? -1 : mpDamageTarget->mUnidentified120;
        fn_80339450(cache, lbl_806DB5C0, copy, context);
    }
}

void cBall::fn_8001A898(RunningChecksum* runningChecksum)
{
    runningChecksum->ChecksumData(&m_v3Position, sizeof(m_v3Position));
    runningChecksum->ChecksumData(&m_v3Velocity, sizeof(m_v3Velocity));
    runningChecksum->ChecksumData(&m_qOrientation, sizeof(m_qOrientation));
    runningChecksum->ChecksumData(&meBallState, sizeof(meBallState));
    runningChecksum->ChecksumData(&mfChargeValue, sizeof(mfChargeValue));
    runningChecksum->ChecksumData(
        &mfSkillShotTime, sizeof(mfSkillShotTime));
}

nlVector3* cBall::GetAIVelocity() const
{
    cPlayer* temp_r4 = m_pOwner;
    if (temp_r4 != NULL)
    {
        return &(temp_r4->m_v3Velocity);
    }
    return (nlVector3*)&(m_v3Velocity);
}

nlVector3* cBall::GetDrawablePosition() const
{
    const nlMatrix4& mtx = m_pDrawableBall->GetWorldMatrix();
    return (nlVector3*)&(mtx.e2[3][0]);
}

float cBall::fn_80014F38(float fScale) const
{
    return fScale * (0.18f * lbl_806DB514);
}

cFielder* cBall::GetOwnerFielder()
{
    cPlayer* player = m_pOwner;
    if ((player == NULL) || (player->m_eClassType != FIELDER))
    {
        return NULL;
    }
    return (cFielder*)player;
}

cPlayer* cBall::GetOwnerGoalie()
{
    cPlayer* player = m_pOwner;
    if ((player == NULL) || (player->m_eClassType != GOALIE))
    {
        return NULL;
    }
    return player;
}

cFielder* cBall::GetPassTargetFielder() const
{
    cPlayer* player = m_pPassTarget;
    if ((player == NULL) || (player->m_eClassType != FIELDER))
    {
        return NULL;
    }
    return (cFielder*)player;
}

bool cBall::GetInNet(int& nSide)
{
    UnidentifiedGameState* gameState
        = (UnidentifiedGameState*)lbl_806E0C94;
    if (gameState->mUnidentified40 == 0)
    {
        if (m_pPhysicsBall->mbIsInsideNet)
        {
            float fDirection = g_pTeams[0]->m_pNet->m_fDirection;
            nSide = !(m_v3Position.x * fDirection > 1.0f);
            return true;
        }
    }
    else if (gameState->mUnidentified2C > gameState->mUnidentified28
             && gameState->mUnidentified30 != 0)
    {
        float fDirection = g_pTeams[0]->m_pNet->m_fDirection;
        nSide = !(m_v3Position.x * fDirection > 1.0f);
        return true;
    }

    return false;
}

LiveBallTrail::LiveBallTrail()
{
    drawable = NULL;
    mUnidentified038 = NULL;
    visible = false;
    orientation.z = 0.0f;
    orientation.y = 0.0f;
    orientation.x = 0.0f;
    orientation.w = 1.0f;
    position = v3Zero;
    velocity = v3Zero;
    mUnidentified028 = v3Zero;
}

LiveBallTrail::~LiveBallTrail()
{
    if (mUnidentified038 != NULL)
    {
        mUnidentified038->Die(lbl_806DB54C);
        mUnidentified038 = NULL;
    }
}

extern "C" LiveBallTrail* fn_8001B284(unsigned int nIndex)
{
    return &lbl_8056B518[nIndex];
}

extern "C" void fn_8001AA0C(LiveBallTrail* pBallTrail, bool bParam)
{
    pBallTrail->visible = bParam;
    if (!pBallTrail->visible)
    {
        EmissionManager::Instance()->Destroy(
            (unsigned long)pBallTrail, NULL);
        if (pBallTrail->mUnidentified038 != NULL)
        {
            pBallTrail->mUnidentified038->Die(lbl_806DB54C);
            pBallTrail->mUnidentified038 = NULL;
        }
    }
}

extern "C" void fn_8001B298(float fParam)
{
    LiveBallTrail* pBallTrail = lbl_8056B518;
    for (unsigned int i = 0; i < lbl_806E0C10; ++i)
    {
        if (pBallTrail->visible)
        {
            fn_8001AA6C(pBallTrail, fParam);
        }
        ++pBallTrail;
    }
}

extern "C" unsigned int fn_8001B30C()
{
    return lbl_806E0C10;
}
