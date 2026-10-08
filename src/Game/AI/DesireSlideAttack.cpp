#include "Game/AI/DesireSlideAttack.h"

#include "Game/AI/AiUtil.h"
#include "Game/AI/AvoidController.h"
#include "Game/AI/DesireUpdate.inl"
#include "Game/AI/Fielder.h"
#include "Game/AI/Scripts/ScriptQuestions.h"
#include "Game/Ball.h"
#include "Game/DebugWriteCache.h"
#include "Game/Team.h"
#include "NL/nlMath.h"
#include <stddef.h>

static float sSlideAttackTargetLeadTime = 0.25f;
static unsigned short sDesireSlideAttackType = 0xFFFF;

/**
 * Offset/Address/Size: 0x0 | 0x800C7DDC | size: 0xB8
 */
bool DesireSlideAttack::Initialize(void* context)
{
    bool result = Desire::Initialize(context);
    UserControlledT(m_pFielder->m_pTeam);

    FuzzyVariantCollection* params
        = (FuzzyVariantCollection*)context;
    mpTarget = (cFielder*)params->Get(14)->mData.pPlayer;
    if (mpTarget == NULL)
    {
        mpTarget = (cFielder*)params->Get(0)->mData.pPlayer;
    }

    if (mpTarget == NULL)
    {
        m_pFielder->InitActionSlideAttack(NULL, -1.0f, 0);
        meDesireSubState = DESIRE_SLIDE_ATTACKING;
    }
    else
    {
        meDesireSubState = DESIRE_SLIDE_APPROACH;
    }
    return result;
}

/**
 * Offset/Address/Size: 0xB8 | 0x800C7E94 | size: 0x638
 */
void DesireSlideAttack::Update(
    DesireUpdate* update, float)
{
    cFielder* pFielder = m_pFielder;
    nlVector3 v3VictimPosition;
    float fBallClosingSpeed;

    switch (meDesireSubState)
    {
    case DESIRE_SLIDE_APPROACH:
    {
        if (mpTarget == NULL || mpTarget != g_pBall->m_pOwner)
        {
            update->SetDesireFinished();
            return;
        }

        if (fn_800D7B00(pFielder) >= 0.5f)
        {
            pFielder->InitActionSlideAttack(mpTarget, -1.0f, 0);
            meDesireSubState = DESIRE_SLIDE_ATTACKING;
            break;
        }

        v3VictimPosition.x = mpTarget->m_DetChar.m_v3Position.x
                           + sSlideAttackTargetLeadTime * mpTarget->m_DetChar.m_v3Velocity.x;
        v3VictimPosition.y = mpTarget->m_DetChar.m_v3Position.y
                           + sSlideAttackTargetLeadTime * mpTarget->m_DetChar.m_v3Velocity.y;
        v3VictimPosition.z = 0.0f;
        pFielder->AddDesiredPosition(v3VictimPosition, 1.5f, 1.0f);
        pFielder->GetAvoidController()->UseMinimumAvoidance(mpTarget);
        break;
    }
    case DESIRE_SLIDE_ATTACKING:
    {
        mMaxDuration = 5.0f;
        if (pFielder->m_DetPlayer.m_tSlideAttackTimer.m_uPackedTime != 0)
        {
            if (!pFielder->bAttackSucceeded)
            {
                float fBallSpeed = nlSqrt(
                    g_pBall->m_v3Velocity.x * g_pBall->m_v3Velocity.x
                    + g_pBall->m_v3Velocity.y * g_pBall->m_v3Velocity.y
                    + g_pBall->m_v3Velocity.z * g_pBall->m_v3Velocity.z,
                    true);
                if (fBallSpeed > 0.05f)
                {
                    cBall* const pBall = g_pBall;
                    const nlVector3& ballVelocity = pBall->m_v3Velocity;
                    fBallClosingSpeed = GetClosingSpeed2D(
                        pFielder->GetJointPosition(
                            pFielder->m_nLeftFootJointIndex),
                        pFielder->m_DetChar.m_v3Velocity,
                        pBall->m_v3Position, ballVelocity);
                    if (fBallClosingSpeed < 0.0f
                        && nlRandomf(1.0f) > 0.5f)
                    {
                        pFielder->m_DetPlayer.m_tSlideAttackTimer.SetSeconds(0.0f);
                        meDesireSubState = DESIRE_SLIDE_RECOVER;
                    }
                }
            }
        }
        else
        {
            meDesireSubState = DESIRE_SLIDE_RECOVER;
        }
        break;
    }
    case DESIRE_SLIDE_RECOVER:
    {
        if (pFielder->IsActionDone())
        {
            update->SetDesireFinished();
        }
        break;
    }
    }
}

/**
 * Offset/Address/Size: 0x6F0 | 0x800C84CC | size: 0x4
 */
void DesireSlideAttack::Cleanup()
{
}

/**
 * Offset/Address/Size: 0x6F4 | 0x800C84D0 | size: 0x5C
 */
inline DesireSlideAttack::~DesireSlideAttack()
{
}

/**
 * Offset/Address/Size: 0x750 | 0x800C852C | size: 0x110
 */
inline void DesireSlideAttack::RegisterDebugFields(
    void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field
        = cache->BeginType("DesireSlideAttack");
    Desire::RegisterDebugFields(field, cache);
    cache->AddField(15, gDebugFieldTypes[15].size,
        (u8*)&mpTarget - (u8*)&mvDesiredPosition, "mpTarget");
    cache->AddField(14, gDebugFieldTypes[14].size,
        (u8*)&meDesireSubState - (u8*)&mvDesiredPosition,
        "meDesireSubState");
    cache->EndType();
}

/**
 * Offset/Address/Size: 0x860 | 0x800C863C | size: 0xC0
 */
inline void DesireSlideAttack::SyncLog(
    void* context, DebugWriteCache* cache)
{
    if (sDesireSlideAttackType == 0xFFFF)
    {
        RegisterDebugFields(&sDesireSlideAttackType, cache);
    }

    unsigned int offset = (u8*)&mvDesiredPosition - (u8*)this;
    void* data = cache->WriteData(sDesireSlideAttackType,
        (u8*)this + offset, sizeof(DesireSlideAttack) - offset);
    if (data != NULL)
    {
        DesireSlideAttack* copy
            = (DesireSlideAttack*)((u8*)data - offset);
        *(int*)&copy->mpTarget
            = mpTarget == NULL ? -1 : mpTarget->m_nCharacterIndex;
        cache->ChecksumData(sDesireSlideAttackType, data, context);
    }
}
