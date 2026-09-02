#include <stddef.h>

#include "Game/Ball.h"

#include "Game/Drawable/DrawableObj.h"
#include "Game/Game.h"
#include "Game/Net.h"
#include "Game/ObjectBlur.h"
#include "Game/Physics/PhysicsAIBall.h"
#include "Game/Physics/PhysicsFakeBall.h"
#include "Game/Player.h"
#include "Game/Team.h"
#include "NL/nlMain.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"

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

extern void* lbl_806E201C;

extern "C" DebugFieldType lbl_80533C98[];
extern "C" void fn_80015C38(cBall*, int);
extern "C" void fn_80096CDC(cPlayer*, cBall*);
extern "C" void fn_801B79A4(const char*, int);
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
extern float lbl_806DB584;
extern float lbl_806DB588;
static unsigned short lbl_806DB5C0 = 0xFFFF;

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

void cBall::ClearBallBlur()
{
    if (m_pBlurHandler != NULL)
    {
        m_pBlurHandler->Die(0.5f);
        m_pBlurHandler = NULL;
    }
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
