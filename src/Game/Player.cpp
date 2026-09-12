#include <stddef.h>

#include "Game/Player.h"

#include "Game/AI/AIPad.h"
#include "Game/AI/DesireSteering.h"
#include "Game/AI/SpaceSearch.h"
#include "Game/AnimInventory.h"
#include "Game/Ball.h"
#include "Game/DebugWriteCache.h"
#include "Game/EventDataTypes.h"
#include "Game/FormationDefines.h"
#include "Game/PoseAccumulator.h"
#include "Game/Physics/PhysicsAIBall.h"
#include "Game/Physics/PhysicsCharacter.h"
#include "Game/Render/NPCManager.h"
#include "Game/SAnim/pnFeather.h"
#include "Game/SAnim/pnSAnimController.h"
#include "Game/SAnim/pnSingleAxisBlender.h"
#include "Game/Sys/audio.h"
#include "NL/nlMain.h"
#include "Game/NetworkInput.h"
#include "unclassified/tu_801B535C.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/Audio/UnidentifiedRegistryPools.h"

extern "C" cPlayer* fn_80096514(
    cPlayer* pSelf, cTeam* pTeam, int nNumPlayers,
    nlVector3* pPosition, bool bParam);
extern "C" void fn_801BCC38(cCharacter*);
extern "C" void fn_801BCE2C(cCharacter*);
extern "C" void fn_80095DF4(cPlayer* self, float fDeltaT);
extern "C" void fn_80097BE0(unsigned int, unsigned int, cPoseAccumulator*, unsigned int, int);

void cPlayer::SetSpaceSearch(SpaceSearch* pSpaceSearch)
{
    if (m_pSpaceSearch != NULL)
    {
        delete m_pSpaceSearch;
    }
    m_pSpaceSearch = pSpaceSearch;
}

void cPlayer::Update(float fDeltaT)
{
    if (m_pController != NULL)
    {
        m_UserControlledTime += fDeltaT;
    }
    else
    {
        m_UserControlledTime = 0.0f;
    }

    fn_80095DF4(this, fDeltaT);

    FieldLocToAILoc(
        m_v3AIPosition, mUnidentified024.m_v3Position, (eTeamSide)m_pTeam->m_nSide);
}

void cPlayer::SetAnimID(int animID)
{
    cCharacter::SetAnimID(animID);
    eBallRotationMode ballRotationMode
        = (eBallRotationMode)m_pAnimInventory->GetBallRotationMode(animID);
    if (mUnidentified024.m_eCharacterClass != MYSTERY)
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

DetInput* cPlayer::GetGlobalPad()
{
    if (m_pController != NULL)
    {
        return m_pController->m_pGlobalPad;
    }
    return NULL;
}

void* cPlayer::fn_800972CC()
{
    DetInput* pGlobalPad
        = m_pController != NULL ? m_pController->m_pGlobalPad : NULL;
    void* pResult = NULL;
    if (pGlobalPad != NULL)
    {
        pResult = ((NetworkPeerChannel*)pGlobalPad->m_pMyUser)->GetLocalChannelPad();
    }
    return pResult;
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

void cPlayer::PlayAttackReactionSounds(float fScale)
{
}

void cPlayer::fn_80096CDC(cBall* pBall)
{
    if (m_pBall == NULL && pBall != NULL && m_eClassType == FIELDER)
    {
        DesireSteering* pDesire
            = (DesireSteering*)fn_8002E08C((cFielder*)this, 34);
        fn_800C574C(pDesire);
    }
    m_pBall = pBall;
}

void cPlayer::ClearPowerupAnimState(bool bIsEndGame)
{
    m_pPowerupLayer->BeginBlendOut(0.25f);
}

void cPlayer::fn_800974B0()
{
    if (m_eClassType == GOALIE)
    {
        fn_801BCC38(this);
    }
    else
    {
        fn_801BCE2C(this);
    }
    m_tFireTimer.m_unk0 = m_tFireTimer.m_uPackedTime != 0;
    m_tFireTimer.m_uPackedTime = 0;
}

void cPlayer::fn_8009750C()
{
    if (m_tFireTimer.m_uPackedTime != 0)
    {
        fn_800974B0();
    }
}

void cPlayer::SetPowerupAnimState(int nodeIndex, int animID, float blendTime)
{
    cPN_SAnimController* controller = NewAnimController(animID, false, false, NULL, 0);
    if (m_pPowerupLayer->GetChild(1) != NULL)
    {
        m_pPowerupLayer->BeginBlendOut(-1.0f);
    }
    m_pPowerupLayer->ClearNodeWeights();
    m_pPowerupLayer->SetNodeWeight(nodeIndex, 1.0f, 0.2f);
    m_pPowerupLayer->SetChild(1, controller);
    m_pPowerupLayer->BeginBlendIn(blendTime);
    m_nFeatherAnimID = animID;
}

void cPlayer::fn_80097648(float duration)
{
    if (m_pPowerupLayer->GetChild(1) != NULL)
    {
        if (duration <= 0.0f || m_pPowerupLayer->m_eFeatherBlendMode != FEATHER_BLEND_OUT)
        {
            m_pPowerupLayer->BeginBlendOut(duration);
        }
    }
}

bool cPlayer::fn_800976C4()
{
    return m_pPowerupLayer->GetChild(1) != NULL;
}

bool cPlayer::IsCharacterInAir(float fParam) const
{
    float leftFootZ = GetJointPosition(m_nLeftFootJointIndex).z;
    float rightFootZ = GetJointPosition(m_nRightFootJointIndex).z;
    float headZ = GetJointPosition(m_nHeadJointIndex).z;

    if (leftFootZ > fParam && rightFootZ > fParam && headZ > fParam)
    {
        return true;
    }
    return false;
}

cFielder* cPlayer::GetClosestOpponentFielder(
    nlVector3* pPosition, bool bParam)
{
    return (cFielder*)::fn_80096514(
        this, m_pTeam->GetOtherTeam(), 4, pPosition, bParam);
}

cPlayer* cPlayer::fn_800966AC(nlVector3* pPosition, bool bParam)
{
    return ::fn_80096514(
        this, m_pTeam->GetOtherTeam(), 5, pPosition, bParam);
}

cPlayer* cPlayer::fn_8009670C(nlVector3* pPosition, bool bParam)
{
    return ::fn_80096514(this, m_pTeam, 5, pPosition, bParam);
}

nlVector3 cPlayer::GetAIOffNetLocation(const nlVector3* v3ReferencePos)
{
    return m_pTeam->GetAIOffNetLocation(
        v3ReferencePos != NULL ? v3ReferencePos : &mUnidentified024.m_v3Position);
}

nlVector3 cPlayer::GetAIDefNetLocation(const nlVector3* v3ReferencePos)
{
    return m_pTeam->GetAIDefNetLocation(
        v3ReferencePos != NULL ? v3ReferencePos : &mUnidentified024.m_v3Position);
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
    if (mUnidentified024.m_eCharacterClass != MYSTERY)
    {
        m_eBallRotationMode = BRM_MATCH_VELOCITY;
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

cPN_SingleAxisBlender* cPlayer::CreateSingleAxisBlender(
    const int* pSABAnims,
    int nNumSABAnims,
    int nPrimaryAnim,
    void (*fWeightCB)(unsigned int, cPN_SingleAxisBlender*),
    float fWeightSeek,
    cPN_SAnimController* pSynchingController,
    float fInitialWeight)
{
    const int* pAnims;
    cPN_SAnimController* pNewCurrentAnimController = NULL;

    cPN_SingleAxisBlender* pSAB = new cPN_SingleAxisBlender(
        nNumSABAnims, fWeightCB, (unsigned int)this, fWeightSeek);

    pAnims = pSABAnims;
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
        float scale = mUnidentified024.m_fPlayerScale;
        if (scale > 1.0f)
        {
            float radius = g_pBall->m_pPhysicsBall->GetRadius();
            jointPos.z -= radius * (scale - 1.0f);
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

void cPlayer::Unknown7(float)
{
    m_pPoseAccumulator->SetBuildNodeMatrixCallback(
        m_nHeadJointIndex, fn_80097BE0, (unsigned int)this, 0);
    if (m_pBall != NULL)
    {
        m_pBall->m_pPhysicsBall->EnableCollisions();
        m_pPhysicsCharacter->ContainObject(m_pBall->m_pPhysicsBall);
    }
}

void cPlayer::PrePhysicsUpdate()
{
    m_pPoseAccumulator->SetBuildNodeMatrixCallback(
        m_nHeadJointIndex, NULL, 0, 0);
    cCharacter::PrePhysicsUpdate();
}

void cPlayer::SetNoPickUpTime(float NewNoPickUpTime)
{
    m_pPhysicsCharacter->m_CanCollideWithBall = (NewNoPickUpTime <= 0.0f);
    m_tNoPickupTimer.SetSeconds(NewNoPickUpTime);
}

void cPlayer::UnidentifiedVirtual1C()
{
    cCharacter::UnidentifiedVirtual1C();
    if (m_pPowerupLayer->GetChild(1) != NULL)
    {
        m_pPowerupLayer->BeginBlendOut(-1.0f);
    }
}

extern "C" void fn_80098A68(UnidentifiedEventData_800673FC* pData)
{
    PlaySound(
        pData->mUnidentified00->mUnidentified318,
        0x9F35CA0F, NULL, NULL);
}

extern "C" void fn_80098A84(UnidentifiedEventData_800673FC* pData)
{
    PlaySound(
        pData->mUnidentified00->mUnidentified318,
        0x85EF26D0, NULL, NULL);
}

extern "C" void fn_80099030(UnidentifiedEventData00*)
{
    if (gNPCManager != NULL)
    {
        gNPCManager->fn_801AA348();
        if (gNPCManager->mUnidentified024 != NULL)
        {
            gNPCManager->mUnidentified024->fn_801B59DC(true);
        }
    }
}

extern "C" void fn_80099074(void)
{
}

u16 lbl_806DBD96 = 0xFFFF;

#define REGISTER_PLAYER_FIELD(type, field) \
    cache->AddField(type, gDebugFieldTypes[type].size, \
        (u8*)&field - (u8*)&m_ID, #field)

void cPlayer::Unknown11(void* context, DebugWriteCache* cache)
{
    cCharacter::Unknown11(context, cache);
    if (lbl_806DBD96 == 0xFFFF)
    {
        lbl_806DBD96 = cache->BeginType("DetPlayer");
        REGISTER_PLAYER_FIELD(8, m_ID);
        REGISTER_PLAYER_FIELD(8, m_nFeatherAnimID);
        REGISTER_PLAYER_FIELD(16, m_bIsContactingWall);
        REGISTER_PLAYER_FIELD(17, m_fSkipTimer);
        REGISTER_PLAYER_FIELD(16, m_bSkipActionUpdate);
        REGISTER_PLAYER_FIELD(16, m_bSkipAnimUpdate);
        REGISTER_PLAYER_FIELD(16, m_bForceFeatherUpdate);
        REGISTER_PLAYER_FIELD(22, m_v3AIPosition);
        REGISTER_PLAYER_FIELD(14, m_eBallRotationMode);
        REGISTER_PLAYER_FIELD(16, m_ResetBaseBallOrientation);
        REGISTER_PLAYER_FIELD(24, m_BaseBallOrientation);
        REGISTER_PLAYER_FIELD(20, m_tBallPossessionTimer);
        REGISTER_PLAYER_FIELD(20, m_tBallUnPossessionTimer);
        REGISTER_PLAYER_FIELD(20, m_tNoPickupTimer);
        REGISTER_PLAYER_FIELD(17, m_fShotStrengthTime);
        REGISTER_PLAYER_FIELD(20, m_tSlideAttackTimer);
        REGISTER_PLAYER_FIELD(20, m_tLooseBallPassTimer);
        REGISTER_PLAYER_FIELD(20, m_tInactivityTimer);
        REGISTER_PLAYER_FIELD(20, m_tFireTimer);
        REGISTER_PLAYER_FIELD(16, m_bCanTestController);
        REGISTER_PLAYER_FIELD(14, m_eLastPadAction);
        REGISTER_PLAYER_FIELD(19, m_aSwapFacingDirection);
        REGISTER_PLAYER_FIELD(20, m_tSwapFacingTimer);
        REGISTER_PLAYER_FIELD(17, m_UserControlledTime);
        cache->EndType();
    }
    cache->ChecksumData(lbl_806DBD96, &m_ID, context);
    cache->WriteData(lbl_806DBD96, &m_ID,
        offsetof(cPlayer, m_tSwapControllerTimer) - offsetof(cPlayer, m_ID));
}

#undef REGISTER_PLAYER_FIELD

void cPlayer::Unknown12(RunningChecksum* pChecksum)
{
    cCharacter::Unknown12(pChecksum);
    pChecksum->ChecksumData(&m_v3AIPosition, sizeof(m_v3AIPosition));
    pChecksum->ChecksumData(&m_eBallRotationMode, sizeof(m_eBallRotationMode));
    pChecksum->ChecksumData(&m_BaseBallOrientation, sizeof(m_BaseBallOrientation));
    pChecksum->ChecksumData(&m_tBallPossessionTimer, sizeof(m_tBallPossessionTimer));
    pChecksum->ChecksumData(&m_fShotStrengthTime, sizeof(m_fShotStrengthTime));
    pChecksum->ChecksumData(&m_tSlideAttackTimer, sizeof(m_tSlideAttackTimer));
    pChecksum->ChecksumData(&m_UserControlledTime, sizeof(m_UserControlledTime));
}

void cPlayer::InitActionPostWhistle()
{
}
