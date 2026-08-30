#include <stddef.h>

#include "Game/Player.h"

#include "Game/AI/AIPad.h"
#include "Game/AnimInventory.h"
#include "Game/Ball.h"
#include "Game/PoseAccumulator.h"
#include "Game/Physics/PhysicsBall.h"
#include "Game/Physics/PhysicsCharacter.h"
#include "Game/SAnim/pnSAnimController.h"
#include "Game/SAnim/pnSingleAxisBlender.h"
#include "NL/globalpad.h"

extern "C" void fn_80015C38(cBall*, int);

void cPlayer::SetAnimID(int animID)
{
    cCharacter::SetAnimID(animID);
    eBallRotationMode ballRotationMode
        = (eBallRotationMode)m_pAnimInventory->GetBallRotationMode(animID);
    if (m_eCharacterClass != MYSTERY)
    {
        m_eBallRotationMode = ballRotationMode;
        if (m_pBall != NULL)
        {
            m_ResetBaseBallOrientation = true;
        }
    }
    else
    {
        m_ResetBaseBallOrientation = true;
        m_eBallRotationMode = BRM_ANIMATED;
    }
}

void cPlayer::CollideWithBallCallback(cBall* pBall)
{
    if (pBall->m_pOwner == NULL)
    {
        pBall->m_pLastTouch = this;
    }
}

void cPlayer::CollideWithCharacterCallback(CollisionPlayerPlayerData* pData)
{
}

bool cPlayer::IsOnSameTeam(cPlayer* other)
{
    if ((other != NULL) && (other->m_pTeam == m_pTeam))
    {
        return true;
    }
    return false;
}

cGlobalPad* cPlayer::GetGlobalPad()
{
    if (m_pController != NULL)
    {
        return m_pController->m_pGlobalPad;
    }
    return NULL;
}

void cPlayer::ReleaseBall(int nParam)
{
    m_pPhysicsCharacter->ReleaseObject();
    g_pBall->ClearOwner();
    fn_80015C38(g_pBall, nParam);
    if (!g_pBall->m_bVisible)
    {
        g_pBall->m_bVisible = true;
    }
}

void cPlayer::CollideWithWallCallback(const CollisionPlayerWallData* pData)
{
    if (this != g_pBall->m_pOwner)
    {
        return;
    }
    if (m_eBallRotationMode != BRM_ANIMATED)
    {
        return;
    }
    if (m_eCharacterClass == MYSTERY)
    {
        m_ResetBaseBallOrientation = true;
        m_eBallRotationMode = BRM_ANIMATED;
    }
    else
    {
        m_eBallRotationMode = BRM_MATCH_VELOCITY;
        if (m_pBall != NULL)
        {
            m_ResetBaseBallOrientation = true;
        }
    }
}

cPN_SingleAxisBlender* cPlayer::CreateSingleAxisBlender(
    const int* pSABAnims,
    int nNumSABAnims,
    int nPrimaryAnim,
    void (*fWeightCB)(unsigned int, cPN_SingleAxisBlender*),
    float fWeightSeek,
    cPN_SAnimController* pSynchingController,
    float fInitialWeight)
{
    cPN_SAnimController* pNewCurrentAnimController;
    pNewCurrentAnimController = NULL;

    cPN_SingleAxisBlender* pSAB = new cPN_SingleAxisBlender(
        nNumSABAnims, fWeightCB, (unsigned int)this, fWeightSeek);

    const int* pAnims = pSABAnims;
    for (int i = 0; i < nNumSABAnims; i++)
    {
        cPN_SAnimController* pNewController
            = NewAnimController(*pAnims, false, false, NULL, 0);
        if (pSynchingController != NULL)
        {
            pNewController->m_bIsSynchronized = true;
            pSynchingController->m_pSynchronizedController = pNewController;
            pSynchingController = pNewController;
        }
        pSAB->SetChild(i, pNewController);
        if (i == nPrimaryAnim)
        {
            pNewCurrentAnimController = pNewController;
        }
        pAnims++;
    }

    SetAnimID(pSABAnims[nPrimaryAnim]);
    m_pCurrentAnimController = pNewCurrentAnimController;
    fWeightCB((unsigned int)this, pSAB);
    pSAB->m_fSmoothedWeight = fInitialWeight;

    return pSAB;
}

void cPlayer::PostPhysicsUpdate()
{
    cCharacter::PostPhysicsUpdate();

    if (m_pBall != NULL)
    {
        nlVector3 jointPos = GetJointPosition(m_nBallJointIndex);
        float scale = mUnidentified0A0;
        if (scale > 1.0f)
        {
            jointPos.z -= (scale - 1.0f)
                        * g_pBall->m_pPhysicsBall->GetRadius();
        }
        m_pPhysicsCharacter->m_SubObject.SetSubObjectPosition(
            jointPos, PhysicsObject::WORLD_COORDINATES);
    }
}

void cPlayer::PreUpdate(float dt)
{
    cCharacter::PreUpdate(dt);
    m_bCanTestController = true;
}

void cPlayer::PrePhysicsUpdate(float dt)
{
    m_pPoseAccumulator->SetBuildNodeMatrixCallback(
        m_nHeadJointIndex, NULL, 0, 0);
    cCharacter::PrePhysicsUpdate(dt);
}

void cPlayer::SetNoPickUpTime(float NewNoPickUpTime)
{
    m_pPhysicsCharacter->m_CanCollideWithBall = (NewNoPickUpTime <= 0.0f);
    m_tNoPickupTimer.SetSeconds(NewNoPickUpTime);
}
