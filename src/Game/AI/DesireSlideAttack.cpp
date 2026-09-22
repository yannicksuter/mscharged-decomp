#include "Game/AI/DesireSlideAttack.h"

#include "Game/AI/AiUtil.h"
#include "Game/AI/AvoidController.h"
#include "Game/AI/DesireUpdate.h"
#include "Game/AI/Fielder.h"
#include "Game/AI/FuzzyVariant.h"
#include "Game/AI/Scripts/ScriptQuestions.h"
#include "Game/Ball.h"
#include "Game/DebugWriteCache.h"
#include "Game/Team.h"
#include "NL/nlMath.h"
#include <stddef.h>

extern "C" AvoidController* fn_8002E144(cFielder*);
extern "C" void fn_800401C0(
    cFielder*, const nlVector3&, float, float);
extern "C" float fn_800D7B00(cFielder*);
static float lbl_806DC238 = 0.25f;
static unsigned short sDesireSlideAttackType = 0xFFFF;

/**
 * Offset/Address/Size: 0x0 | 0x800C7DDC | size: 0xB8
 */
bool DesireSlideAttack::UnidentifiedInitialize(void* context)
{
    bool result = Desire::UnidentifiedInitialize(context);
    UserControlledT(mUnidentifiedFielder->m_pTeam);

    UnidentifiedVariantCollection* params
        = (UnidentifiedVariantCollection*)context;
    mpTarget = (cFielder*)params->Get(14)->mData.pPlayer;
    if (mpTarget == NULL)
    {
        mpTarget = (cFielder*)params->Get(0)->mData.pPlayer;
    }

    if (mpTarget == NULL)
    {
        mUnidentifiedFielder->InitActionSlideAttack(NULL, 0, -1.0f);
        meDesireSubState = 1;
    }
    else
    {
        meDesireSubState = 0;
    }
    return result;
}

/**
 * Offset/Address/Size: 0xB8 | 0x800C7E94 | size: 0x638
 */
void DesireSlideAttack::Update(
    UnidentifiedDesireUpdate* update, float)
{
    cFielder* pFielder = mUnidentifiedFielder;
    nlVector3 v3VictimPosition;
    float fBallClosingSpeed;

    switch (meDesireSubState)
    {
    case 0:
    {
        if (mpTarget == NULL || mpTarget != g_pBall->m_pOwner)
        {
            update->SetDesireFinished();
            return;
        }

        if (fn_800D7B00(pFielder) >= 0.5f)
        {
            pFielder->InitActionSlideAttack(mpTarget, 0, -1.0f);
            meDesireSubState = 1;
            break;
        }

        v3VictimPosition.x = mpTarget->mUnidentified024.m_v3Position.x
                           + lbl_806DC238 * mpTarget->mUnidentified024.m_v3Velocity.x;
        v3VictimPosition.y = mpTarget->mUnidentified024.m_v3Position.y
                           + lbl_806DC238 * mpTarget->mUnidentified024.m_v3Velocity.y;
        v3VictimPosition.z = 0.0f;
        fn_800401C0(pFielder, v3VictimPosition, 1.5f, 1.0f);
        fn_8002E144(pFielder)->UseMinimumAvoidance(mpTarget);
        break;
    }
    case 1:
    {
        mUnidentified078 = 5.0f;
        if (pFielder->mUnidentified1E4.m_tSlideAttackTimer.m_uPackedTime != 0)
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
                        pFielder->mUnidentified024.m_v3Velocity,
                        pBall->m_v3Position, ballVelocity);
                    if (fBallClosingSpeed < 0.0f
                        && nlRandomf(1.0f) > 0.5f)
                    {
                        pFielder->mUnidentified1E4.m_tSlideAttackTimer.SetSeconds(0.0f);
                        meDesireSubState = 2;
                    }
                }
            }
        }
        else
        {
            meDesireSubState = 2;
        }
        break;
    }
    case 2:
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
void DesireSlideAttack::UnidentifiedCleanup()
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
inline void DesireSlideAttack::UnidentifiedVirtual8(
    void* field, DebugWriteCache* cache)
{
    *(unsigned short*)field
        = cache->BeginType("DesireSlideAttack");
    Desire::UnidentifiedVirtual8(field, cache);
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
inline void DesireSlideAttack::UnidentifiedVirtual7(
    void* context, DebugWriteCache* cache)
{
    if (sDesireSlideAttackType == 0xFFFF)
    {
        UnidentifiedVirtual8(&sDesireSlideAttackType, cache);
    }

    unsigned int offset = (u8*)&mvDesiredPosition - (u8*)this;
    void* data = cache->WriteData(sDesireSlideAttackType,
        (u8*)this + offset, sizeof(DesireSlideAttack) - offset);
    if (data != NULL)
    {
        DesireSlideAttack* copy
            = (DesireSlideAttack*)((u8*)data - offset);
        *(int*)&copy->mpTarget
            = mpTarget == NULL ? -1 : mpTarget->mUnidentified120;
        cache->ChecksumData(sDesireSlideAttackType, data, context);
    }
}
