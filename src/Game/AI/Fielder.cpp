#include "Game/AI/Fielder.h"
#include "Game/AI/FielderDesireMachine.h"
#include "Game/AI/FielderInput.h"
#include "Game/AI/UnidentifiedAvoidanceObject.h"

#include "Game/AI/FuzzyVariant.h"
#include "Game/AI/ShotMeter.h"
#include "Game/Ball.h"
#include "Game/CharacterTweaks.h"
#include "Game/EventDataTypes.h"
#include "Game/Field.h"
#include "Game/GameInfo.h"
#include "Game/Goalie.h"
#include "Game/MathHelpers.h"
#include "Game/Net.h"
#include "Game/Physics/PhysicsCharacter.h"
#include "Game/Render/NPCManager.h"
#include "Game/SAnim/pnSAnimController.h"
#include "Game/Team.h"
#include "math.h"
#include "Game/DB/StadiumInfo.h"
#include "unclassified/tu_80177498.h"

struct UnidentifiedPadAction
{
    u8 mUnidentified00[0x08];
    bool mUnidentified08;
    u8 mUnidentified09[0x9B];
    int mUnidentifiedA4;
    u8 mUnidentifiedA8[0x3C];
    float mUnidentifiedE4;
    void* mUnidentifiedE8;

    bool IsUnidentifiedState(int nState) const
    {
        return mUnidentified08 && mUnidentifiedA4 == nState;
    }
};

extern "C" UnidentifiedPadAction* fn_80319FC0(void* pParam, int nAction);
extern "C" UnidentifiedPadAction* fn_80319F94(void* pParam, int nAction);
extern "C" bool fn_80319FEC(void* pParam, int nAction);
extern "C" float fn_8002BFA8(PlayerTweaks* pTweaks, float fTime);
extern "C" float fn_8002D020(PlayerTweaks* pTweaks);
extern "C" float fn_8002D050(PlayerTweaks* pTweaks);
extern "C" bool fn_8002F310(cFielder* pFielder);
extern "C" void fn_80060608(void* pParam, cFielder* pFielder);
extern "C" void fn_800ED92C(unsigned long soundID);
extern "C" bool fn_8001E168(const cCharacter* pCharacter);
extern "C" void fn_8003E354(cFielder* pFielder);
extern "C" void fn_80080BFC(Goalie* pGoalie, float fDeltaT);
extern "C" void fn_800BED24(
    UnidentifiedPadAction* pAction, unsigned short* pParam);
extern unsigned char lbl_806E0C61;
extern void* lbl_806E0C94;
extern float lbl_806E3418;
extern float lbl_806E3420;
extern float lbl_806E3424;
extern float lbl_806E3428;
extern float lbl_806E342C;

static LooseBallContactAnimInfo gOneTimerIdleGroundContactAnims[4] = {
    { 0x38, 9.0f, 0xE000, 0x2000 },
    { 0x39, 9.0f, 0xA000, 0xE000 },
    { 0x3B, 9.0f, 0x6000, 0xA000 },
    { 0x3A, 9.0f, 0x2000, 0x6000 },
};

static LooseBallContactAnimInfo gOneTimerIdleVolleyContactAnims[4] = {
    { 0x44, 4.0f, 0xE000, 0x2000 },
    { 0x45, 4.0f, 0xA000, 0xE000 },
    { 0x47, 4.0f, 0x6000, 0xA000 },
    { 0x46, 4.0f, 0x2000, 0x6000 },
};

static LooseBallContactAnimInfo gOneTimerLeadGroundContactAnims[2] = {
    { 0x48, 6.0f, 0xC000, 0x4000 },
    { 0x49, 6.0f, 0x4000, 0xC000 },
};

cFielder::cFielder(int nPlayerID, int nTeamID, eCharacterClass cc,
    const int* nModelID, cSHierarchy* pHierarchy,
    cAnimInventory* pAnimInventory,
    const CharacterPhysicsData* pCharacterPhysicsData, PlayerTweaks* pCharTweaks,
    PlayerTweaks* pUnidentifiedTweaks,
    AnimRetargetList* pAnimRetargetList, int nIndex)
    : cPlayer(nPlayerID, cc, nModelID, pHierarchy, pAnimInventory,
          pCharacterPhysicsData, pCharTweaks->mUnidentified004,
          fn_8002BFA8(pCharTweaks, 1.0f), pAnimRetargetList, nIndex, FIELDER)
    , mUnidentified330(false, -1.0f)
    , mUnidentified338(0)
    , mUnidentified33A(false)
    , mUnidentified33C(2)
    , mUnidentified340(0.0f)
    , mUnidentified344(0.0f)
    , mUnidentified348(false)
    , mUnidentified34C(0.0f)
    , mUnidentified350(v3Zero)
    , mUnidentified35C(0.0f)
    , mUnidentified360(false)
    , bYoshiInWindup(false)
    , bIsModified(false)
    , mActionLooseBallPassVars()
    , mUnidentified368(0.0f)
    , mUnidentified36C(0)
    , mUnidentified370(false)
    , mUnidentified371(false)
    , mUnidentified374()
    , mUnidentified37C(0)
    , mUnidentified380(false)
    , mActionRunningWBVars()
    , mUnidentified388(0)
    , mUnidentified38C(false)
    , mUnidentified38D(true)
    , mUnidentified390(0.0f)
    , mUnidentified394(0.0f)
    , mUnidentified398(-1.0f)
    , mUnidentified39C(-1.0f)
    , mUnidentified3A0(-1.0f)
    , mUnidentified3A4(-1.0f)
    , mUnidentified3A8(-1.0f)
    , mUnidentified3AC(0.0f)
    , mUnidentified3B0(0.0f)
    , mUnidentified3B4(0.0f)
    , mUnidentified3B8(false)
    , mUnidentified3BC(0.0f)
    , mUnidentified3C0(0.0f)
    , mUnidentified3C4(0.0f)
    , mUnidentified3C8(0.0f)
    , mUnidentified3CC(0.0f)
    , mUnidentified3D0(0.0f)
    , mUnidentified3D4(0.0f)
    , mUnidentified3D8(0)
    , mUnidentified3DA(0)
    , mUnidentified3DC(false)
    , mUnidentified3DD(false)
    , mUnidentified3E0(0.0f)
    , mUnidentified3E4(0.0f)
    , mUnidentified3E8(0.0f)
    , mUnidentified3EC(0.0f)
    , mUnidentified3F0(0)
    , mUnidentified3F4(0.0f)
    , mUnidentified3F8(0.0f)
    , mUnidentified3FC(0.0f)
    , mUnidentified404(0.0f)
    , mUnidentified408(0.0f)
    , mUnidentified40C(0.0f)
    , mUnidentified410()
    , mUnidentified424(false)
    , m_tMoveToTurboTimer(0.0f)
    , mtPostDekeTimer(0.0f)
    , mtPowerupThrowTime(0.0f)
{
    mfAirInterceptHeight[0] = -1.0f;
    mfAirInterceptHeight[1] = -1.0f;
    m_bHasBeenUpdated = false;
    m_eActionState = ACTION_NEED_ACTION;
    m_bInPosition = false;
    m_nPowerupAnimID = -1;
    m_eRole = (eRole)0;
    mbWasHitByPowerupThisFrame = false;
    mbTangible = true;
    mbIgnorePadSwitchRelease = false;
    for (int i = 0; i < 4; i++)
    {
        m_pMark[i] = 0;
    }
    m_tMoveToTurboTimer.UnidentifiedClear();
    mtPostDekeTimer.UnidentifiedClear();
    mtPowerupThrowTime.UnidentifiedClear();
    muInvincibleStatus = 0;
    mUnidentified478 = 0;
    mUnidentified32C = pCharTweaks;
    m_pTweaks = pCharTweaks;
    mUnidentified328 = pUnidentifiedTweaks;

    m_pShotMeter = new (8, false) ShotMeter();
    mUnidentified428 = new (8, false) UnidentifiedFielderInput(this,
        new (8, false) UnidentifiedFielderDesireMachine(),
        new (8, false) UnidentifiedFuzzyRuntime());
    mUnidentified428->mUnidentified18->UnidentifiedVirtual2();

    bIsModified = false;
    if (m_eCharacterClass == (eCharacterClass)6)
    {
        mUnidentified400 = new (8, false) WaluigiWallManager_80178400();
    }
    else
    {
        mUnidentified400 = 0;
    }

    if (m_eCharacterClass == (eCharacterClass)19)
    {
        mUnidentified420 = lbl_806E1608->fn_801A9D20();
    }
    else
    {
        mUnidentified420 = 0;
    }
}

bool cFielder::CanGetElectrocuted(
    const CollisionPlayerWallData* eventData)
{
    if (!fn_8002F310(this))
    {
        return false;
    }

    switch (m_eActionState)
    {
    case (eFielderActionState)0:
    case (eFielderActionState)5:
    case (eFielderActionState)6:
    case (eFielderActionState)23:
    case (eFielderActionState)25:
    case (eFielderActionState)26:
    case (eFielderActionState)27:
    case (eFielderActionState)31:
    case (eFielderActionState)34:
    case (eFielderActionState)35:
    {
        if (IsFallenDown())
        {
            float netPostRadius = cNet::GetPostRadius();
            float netWidth = cNet::GetNetWidth();
            float minYElectrocutionPosition
                = netWidth * lbl_806E3424 + netPostRadius;
            float netHeight = cNet::GetNetHeight();
            nlVector3 jointPos
                = GetJointPosition(m_nBip01JointIndex_0xA4);
            if ((float)fabs(eventData->contactPoint.y)
                    > minYElectrocutionPosition
                || (float)fabs(jointPos.z) > netHeight)
            {
                return true;
            }
        }
        break;
    }
    default:
    {
        float netPostRadius = cNet::GetPostRadius();
        float netWidth = cNet::GetNetWidth();
        float minYElectrocutionPosition
            = netWidth * lbl_806E3424 + netPostRadius;
        float netHeight = cNet::GetNetHeight();
        nlVector3 jointPos
            = GetJointPosition(m_nBip01JointIndex_0xA4);
        if ((float)fabs(eventData->contactPoint.y)
                > minYElectrocutionPosition
            || (float)fabs(jointPos.z) > netHeight)
        {
            bool bUnidentified = false;
            if (m_eCharacterClass == MARIO
                && fn_80319FEC(
                    mUnidentified428->mUnidentified18, 0x17))
            {
                bUnidentified = true;
            }

            if (bUnidentified && mUnidentified3DC)
            {
                fn_80060608(lbl_806E0C94, this);
                fn_8005001C(true);
                return false;
            }

            if ((m_eCharacterClass == MARIO
                    || m_eCharacterClass == DONKEYKONG
                    || m_eCharacterClass == (eCharacterClass)0x11)
                && m_eActionState == (eFielderActionState)1)
            {
                if (m_pCurrentAnimController->m_fTime > lbl_806E3428
                    || m_v3Position.z > lbl_806E3418)
                {
                    return true;
                }
            }

            if (lbl_806E0C61 != 0
                || GameInfoManager::Instance()->IsRule0x4Equal3())
            {
                if (m_eActionState != (eFielderActionState)2)
                {
                    return true;
                }
            }
        }
        else
        {
            return false;
        }
        break;
    }
    }

    return false;
}

bool cFielder::CanDoCaptainShootToScore()
{
    if (GameInfoManager::Instance()->IsRule0x8Equal4())
    {
        return false;
    }

    if (g_pBall->GetOwnerFielder() != 0)
    {
        bool bUnidentified0 = false;
        if (GameInfoManager::Instance()
                ->GetCurrentSettings()
                ->unknown_0x16
            && m_pTeam->m_nSide == 0)
        {
            bUnidentified0 = true;
        }

        bool bUnidentified1 = false;
        if (bUnidentified0
            || (GameInfoManager::Instance()
                    ->GetCurrentSettings()
                    ->unknown_0x17
                && m_pTeam->m_nSide == 1))
        {
            bUnidentified1 = true;
        }

        bool bUnidentified2 = fn_8001E168(this);
        if (bUnidentified1 && bUnidentified2)
        {
            float fRadius = lbl_806E3420;
            m_pPhysicsCharacter->GetRadius(&fRadius);

            float fOffset = lbl_806E342C + fRadius;
            float fMinX;
            float fMaxX = fOffset + m_v3Position.x;
            fMinX = m_v3Position.x - fOffset;
            bool bUnidentified3
                = fMaxX * m_pTeam->GetOtherNet()->m_fDirection
                >= lbl_806E3420;
            bool bUnidentified4 = true;
            if (!bUnidentified3)
            {
                bool bUnidentified5
                    = fMinX
                        * m_pTeam->GetOtherNet()->m_fDirection
                    >= lbl_806E3420;
                if (!bUnidentified5)
                {
                    bUnidentified4 = false;
                }
            }

            if (bUnidentified4)
            {
                return true;
            }
        }
    }

    return false;
}

bool cFielder::fn_8003E8F4() const
{
    bool result = false;
    if (m_eCharacterClass == LUIGI
        && fn_80319FEC(mUnidentified428->mUnidentified18, 0x17))
    {
        result = true;
    }
    return result;
}

bool cFielder::fn_8003EA6C() const
{
    bool result = false;
    if (m_eCharacterClass == TOAD
        && fn_80319FEC(mUnidentified428->mUnidentified18, 0x17))
    {
        result = true;
    }
    return result;
}

bool cFielder::CanReceivePass()
{
    bool bCanReceivePass = false;
    bool bCondition6 = false;
    bool bCondition5 = false;
    bool bCondition4 = false;
    bool bCondition3 = false;
    bool bCondition2 = false;
    bool bCondition1 = false;
    bool bCondition0 = false;

    if (!IsFallenDown())
    {
        bool bAllowedAction = true;
        unsigned int nActionIndex
            = (unsigned int)(m_eActionState - 1);
        if (nActionIndex <= 0x1F
            && ((1U << nActionIndex) & 0x90000001U) != 0)
        {
            bAllowedAction = false;
        }

        if (bAllowedAction)
        {
            bCondition0 = true;
        }
    }

    if (bCondition0
        && m_eActionState != (eFielderActionState)0x21)
    {
        bCondition1 = true;
    }

    if (bCondition1)
    {
        bool bExcluded = false;
        if (m_eCharacterClass == HAMMERBROS
            && m_eActionState == (eFielderActionState)0x1D)
        {
            bExcluded = true;
        }
        if (!bExcluded)
        {
            bCondition2 = true;
        }
    }

    if (bCondition2)
    {
        bool bExcluded = fn_8003EA6C();
        if (!bExcluded)
        {
            bCondition3 = true;
        }
    }

    if (bCondition3)
    {
        bool bExcluded
            = m_eCharacterClass == DONKEYKONG
           && fn_80319FEC(mUnidentified428->mUnidentified18, 0x17);
        if (!bExcluded)
        {
            bCondition4 = true;
        }
    }

    if (bCondition4)
    {
        bool bExcluded
            = m_eCharacterClass == WALUIGI
           && fn_80319FEC(mUnidentified428->mUnidentified18, 0x17);
        if (!bExcluded)
        {
            bCondition5 = true;
        }
    }

    if (bCondition5)
    {
        bool bExcluded = fn_8003E8F4();
        if (!bExcluded)
        {
            bCondition6 = true;
        }
    }

    if (bCondition6)
    {
        UnidentifiedPadAction* pAction
            = fn_80319FC0(mUnidentified428->mUnidentified18, 0x1D);
        bool bActionActive = false;
        if (pAction != 0 && pAction->mUnidentified08
            && pAction->mUnidentifiedA4 != 0)
        {
            bActionActive = true;
        }
        if (!bActionActive)
        {
            bCanReceivePass = true;
        }
    }

    return bCanReceivePass;
}

void cFielder::Unknown8(unsigned short aParam, bool bParam)
{
    if (fn_80319FEC(mUnidentified428->mUnidentified18, 0x1E))
    {
        UnidentifiedPadAction* pAction
            = fn_80319F94(mUnidentified428->mUnidentified18, 0x1E);
        if (pAction == 0)
        {
            pAction = fn_80319FC0(
                mUnidentified428->mUnidentified18, 0x1E);
        }
        fn_800BED24(pAction, &aParam);
    }

    cCharacter::Unknown8(aParam, bParam);
}

void cFielder::fn_8003057C(void* pParam)
{
    UnidentifiedPadAction* pAction
        = fn_80319F94(mUnidentified428->mUnidentified18, 0x22);
    if (pAction == 0)
    {
        pAction = fn_80319FC0(mUnidentified428->mUnidentified18, 0x22);
    }
    pAction->mUnidentifiedE8 = pParam;
}

void cFielder::fn_800305DC(float fParam)
{
    UnidentifiedPadAction* pAction
        = fn_80319F94(mUnidentified428->mUnidentified18, 0x22);
    if (pAction == 0)
    {
        pAction = fn_80319FC0(mUnidentified428->mUnidentified18, 0x22);
    }
    pAction->mUnidentifiedE4 = fParam;
}

void cFielder::fn_8003063C(PlayerTweaks* pParam)
{
    if (pParam != 0)
    {
        m_pTweaks = pParam;
    }
    else
    {
        if (m_pTweaks != 0)
        {
            delete m_pTweaks;
        }
        m_pTweaks = mUnidentified32C;
    }
}

void cFielder::fn_800306A0(cFielder* pParam)
{
    for (int i = 0; i < 4; i++)
    {
        if (m_pMark[i] == 0)
        {
            m_pMark[i] = pParam;
            return;
        }
    }
}

void cFielder::fn_800306DC()
{
    for (int i = 0; i < 4; i++)
    {
        m_pMark[i] = 0;
    }
}

bool cFielder::fn_800306F4(cFielder* pParam)
{
    for (int i = 0; i < 4; i++)
    {
        if (m_pMark[i] == pParam)
        {
            return true;
        }
    }
    return false;
}

void cFielder::CollideWithWallCallback(
    const CollisionPlayerWallData* eventData)
{
    cPlayer::CollideWithWallCallback(eventData);

    UnidentifiedPadAction* pAction
        = fn_80319FC0(mUnidentified428->mUnidentified18, 0x1D);
    bool bActionActive = false;
    if (pAction != 0 && pAction->mUnidentified08
        && pAction->mUnidentifiedA4 != 0)
    {
        bActionActive = true;
    }

    bool bShellReact;
    if (!bActionActive
        && m_eActionState == (eFielderActionState)0x16)
    {
        bShellReact = true;
    }
    else
    {
        bShellReact = false;
    }
    if (bShellReact)
    {
        s16 facingDelta
            = (s16)GetFacingDeltaToPosition(eventData->contactPoint);
        int absFacingDelta;
        if (facingDelta < 0)
        {
            absFacingDelta = -facingDelta;
        }
        else
        {
            absFacingDelta = facingDelta;
        }

        if ((u16)absFacingDelta < 0x2000)
        {
            fn_8004D238();
        }
    }

    if (CanGetElectrocuted(eventData))
    {
        InitActionElectrocution(
            eventData->contactPoint, eventData->wallNormal, true);

        int stadium = GameInfoManager::Instance()->GetStadium();
        if (GetStadiumUnknown0x10(stadium))
        {
            unsigned long soundID = 0xCE269987;
            if (m_pTeam->m_nSide == 0)
            {
                soundID = 0x5089F33E;
            }
            fn_800ED92C(soundID);
        }
    }
    else if (m_eActionState != (eFielderActionState)3
             && GameInfoManager::Instance()->GetStadium() == 0x0B)
    {
        float distance = (float)fabs(m_v3Position.y);
        distance -= fn_8002BFA8(m_pTweaks, lbl_806E3418);
        if (distance > cField::GetSidelineY(1) + lbl_806E3424)
        {
            fn_80046244();
        }
    }
}

void cFielder::ShootBallDueToContact(const nlVector3& v3IncomingVelocity)
{
    if (m_eActionState == ACTION_SHOOT_TO_SCORE || m_eActionState == ACTION_SHOT)
    {
        g_pBall->ShootRelease(v3Zero, SPINTYPE_NONE);
        return;
    }

    nlVector3 v3ReleaseVelocity;
    nlVec3Add(v3ReleaseVelocity, v3IncomingVelocity, m_v3Velocity);
    if (nlVec3LengthSquared(v3IncomingVelocity) < 0.001f * 0.001f
        || nlVec3LengthSquared(m_v3Velocity) < 0.001f * 0.001f
        || nlVec3LengthSquared(v3ReleaseVelocity) < 0.001f * 0.001f)
    {
        nlVector3 v3ReleaseVelocity;
        nlPolarToCartesian(v3ReleaseVelocity.x, v3ReleaseVelocity.y,
            m_aActualFacingDirection, 2.0f);
        v3ReleaseVelocity.z = 0.5f;
        g_pBall->ShootRelease(v3ReleaseVelocity, SPINTYPE_NONE);
        return;
    }

    nlVec3Scale(v3ReleaseVelocity, v3ReleaseVelocity,
        nlRecipSqrt(nlVec3LengthSquared(v3ReleaseVelocity), true));
    nlVec3Scale(v3ReleaseVelocity, v3ReleaseVelocity, 2.0f + m_fActualSpeed);
    v3ReleaseVelocity.z = 0.5f;
    g_pBall->ShootRelease(v3ReleaseVelocity, SPINTYPE_NONE);
}

void cFielder::ShootBallDueToContact(unsigned short aShootDirection)
{
    nlVector3 v3ReleaseVelocity;
    nlPolarToCartesian(v3ReleaseVelocity.x, v3ReleaseVelocity.y,
        aShootDirection, 2.0f + m_fActualSpeed);
    v3ReleaseVelocity.z = 0.5f;

    g_pBall->ShootRelease(v3ReleaseVelocity, SPINTYPE_NONE);
}

void cFielder::DoResetShotMeter(float fTime)
{
    m_pShotMeter->Reset(this);
    m_pShotMeter->m_fTime = fTime;
}

bool cFielder::IsActionDone() const
{
    return (u8)(m_eActionState == ACTION_NEED_ACTION);
}

void cFielder::SetAction(eFielderActionState actionState)
{
    CleanUpAction();
    m_eActionState = actionState;
}

const LooseBallContactAnimInfo* GetOneTimerIdleGroundContactAnims()
{
    return gOneTimerIdleGroundContactAnims;
}

int GetNumOneTimerIdleGroundContactAnims()
{
    return sizeof(gOneTimerIdleGroundContactAnims) / sizeof(gOneTimerIdleGroundContactAnims[0]);
}

const LooseBallContactAnimInfo* GetOneTimerIdleVolleyContactAnims()
{
    return gOneTimerIdleVolleyContactAnims;
}

int GetNumOneTimerIdleVolleyContactAnims()
{
    return sizeof(gOneTimerIdleVolleyContactAnims) / sizeof(gOneTimerIdleVolleyContactAnims[0]);
}

const LooseBallContactAnimInfo* GetOneTimerLeadGroundContactAnims()
{
    return gOneTimerLeadGroundContactAnims;
}

int GetNumOneTimerLeadGroundContactAnims()
{
    return sizeof(gOneTimerLeadGroundContactAnims) / sizeof(gOneTimerLeadGroundContactAnims[0]);
}

const LooseBallContactAnimInfo* cFielder::fn_80038230(
    const LooseBallContactAnimInfo* pBallContactAnimInfo,
    int nNumContactAnims, unsigned short aFutureFacingDirection,
    const nlVector3& v3FuturePosition, const nlVector3& v3OneTimerTarget,
    float fAngle)
{
    nlVector3 v3Unidentified;
    nlVec3Sub(v3Unidentified, v3OneTimerTarget, v3FuturePosition);
    u16 aNetAngle = nlVector3ToAngle(v3Unidentified) - aFutureFacingDirection;

    const LooseBallContactAnimInfo* pBestBallContactAnimInfo = NULL;
    for (int i = 0; i < nNumContactAnims; i++)
    {
        if (pBallContactAnimInfo[i].aIncomingAngleMin
            < pBallContactAnimInfo[i].aIncomingAngleMax)
        {
            if (aNetAngle >= pBallContactAnimInfo[i].aIncomingAngleMin
                && aNetAngle <= pBallContactAnimInfo[i].aIncomingAngleMax)
            {
                pBestBallContactAnimInfo = &pBallContactAnimInfo[i];
            }
        }
        else if (aNetAngle >= pBallContactAnimInfo[i].aIncomingAngleMin
            || aNetAngle <= pBallContactAnimInfo[i].aIncomingAngleMax)
        {
            pBestBallContactAnimInfo = &pBallContactAnimInfo[i];
        }
    }
    return pBestBallContactAnimInfo;
}

bool cFielder::IsFallenDown() const
{
    if (m_tFireTimer.m_uPackedTime != 0)
    {
        return true;
    }

    bool bUnidentified;
    switch (m_eActionState)
    {
    case (eFielderActionState)3:
    case (eFielderActionState)0x18:
        bUnidentified = true;
        break;
    default:
        bUnidentified = false;
        break;
    }
    if (bUnidentified)
    {
        return true;
    }

    if (m_eActionState == (eFielderActionState)0x21)
    {
        if (m_eAnimID != 0x81 || m_pCurrentAnimController->m_fTime < 0.3f)
        {
            return true;
        }
        return false;
    }

    if (m_eActionState == (eFielderActionState)0x22
        || m_eActionState == (eFielderActionState)0x23)
    {
        return true;
    }

    float fGetUpFrame = -1.0f;
    switch (m_eAnimID)
    {
    case 0x7F:
        fGetUpFrame = 44.0f;
        break;
    case 0x65:
        fGetUpFrame = 67.0f;
        break;
    case 0x66:
        fGetUpFrame = 64.0f;
        break;
    case 0x6A:
        fGetUpFrame = 30.0f;
        break;
    case 0x6E:
        fGetUpFrame = 43.0f;
        break;
    case 0x72:
        fGetUpFrame = 56.0f;
        break;
    case 0x6B:
    case 0x6D:
        fGetUpFrame = 30.0f;
        break;
    case 0x6F:
    case 0x71:
        fGetUpFrame = 45.0f;
        break;
    case 0x73:
    case 0x75:
        fGetUpFrame = 60.0f;
        break;
    case 0x6C:
        fGetUpFrame = 30.0f;
        break;
    case 0x70:
        fGetUpFrame = 43.0f;
        break;
    case 0x74:
        fGetUpFrame = 56.0f;
        break;
    case 0x5F:
        fGetUpFrame = 42.0f;
        break;
    case 0x61:
        fGetUpFrame = 46.0f;
        break;
    case 0x60:
    case 0x62:
        fGetUpFrame = 43.0f;
        break;
    case 0x63:
        fGetUpFrame = 46.0f;
        break;
    case 0x64:
        fGetUpFrame = 42.0f;
        break;
    case 0x56:
        fGetUpFrame = 108.0f;
        break;
    case 0x76:
    case 0x77:
    case 0x79:
    case 0x7A:
        fGetUpFrame = (float)m_pCurrentAnimController->m_pSAnim->m_nNumKeys;
        break;
    case 0x7C:
        fGetUpFrame = (float)m_pCurrentAnimController->m_pSAnim->m_nNumKeys;
        break;
    case 0x7D:
        fGetUpFrame = 55.0f;
        break;
    case 0x78:
    case 0x7B:
        fGetUpFrame = 29.0f;
        break;
    case 0x68:
        if (m_eCharacterClass == DAISY)
        {
            fGetUpFrame = (float)m_pCurrentAnimController->m_pSAnim->m_nNumKeys;
        }
        break;
    }

    return m_pCurrentAnimController->m_fTime
        < fGetUpFrame / m_pCurrentAnimController->m_pSAnim->m_nNumKeys;
}

bool cFielder::fn_80038918() const
{
    UnidentifiedPadAction* pAction
        = fn_80319FC0(mUnidentified428->mUnidentified18, 0x1D);
    bool bActionActive = false;
    if (pAction != 0 && pAction->mUnidentified08
        && pAction->mUnidentifiedA4 != 0)
    {
        bActionActive = true;
    }
    return bActionActive;
}

bool cFielder::IsHitting() const
{
    const cPN_SAnimController* pAnimController = m_pCurrentAnimController;
    const float fAnimTime
        = pAnimController->m_fTime * pAnimController->m_pSAnim->m_nNumKeys;

    bool bUnidentified0 = false;
    if (m_eCharacterClass != TOAD && !fn_80038918())
    {
        bUnidentified0 = true;
    }
    bool bUnidentified1 = false;
    if (bUnidentified0 && m_eActionState == ACTION_HIT)
    {
        bUnidentified1 = true;
    }
    bool bUnidentified2 = false;
    if (bUnidentified1 && fAnimTime >= fn_8002D020(m_pTweaks))
    {
        bUnidentified2 = true;
    }
    bool isHitting = false;
    if (bUnidentified2 && fAnimTime <= fn_8002D050(m_pTweaks))
    {
        isHitting = true;
    }
    return isHitting;
}

bool cFielder::IsStriker() const
{
    return m_eRole == ROLE_STRIKER;
}

bool cFielder::IsWinger() const
{
    return m_eRole == ROLE_WINGER;
}

bool cFielder::IsMidField() const
{
    return m_eRole == ROLE_MIDFIELD;
}

bool cFielder::IsDefense() const
{
    return m_eRole == ROLE_DEFENCE;
}

bool cFielder::CanPickupBall(cBall* pBall, bool bParam)
{
    bool bUnidentified
        = fn_80319FC0(mUnidentified428->mUnidentified18, 0x1D)
              ->IsUnidentifiedState(1)
       || fn_80319FC0(mUnidentified428->mUnidentified18, 0x1D)
              ->IsUnidentifiedState(2);

    if (bUnidentified)
    {
        return false;
    }

    if (IsFallenDown())
    {
        return false;
    }

    bUnidentified = false;
    if (m_eCharacterClass == TOAD
        && fn_80319FEC(mUnidentified428->mUnidentified18, 0x17))
    {
        bUnidentified = true;
    }

    if (bUnidentified)
    {
        return false;
    }

    return cPlayer::CanPickupBall(pBall, bParam);
}

void cFielder::PreUpdate(float fTime)
{
    cPlayer::PreUpdate(fTime);
    m_bHasBeenUpdated = false;
    mbWasHitByPowerupThisFrame = false;
}

void cFielder::PrePhysicsUpdate()
{
    cPlayer::PrePhysicsUpdate();

    UnidentifiedPadAction* pAction
        = fn_80319FC0(mUnidentified428->mUnidentified18, 0x1D);
    bool bActionActive = false;
    if (pAction != 0 && pAction->mUnidentified08
        && pAction->mUnidentifiedA4 != 0)
    {
        bActionActive = true;
    }

    if (!bActionActive
        && (m_eActionState == ACTION_RECEIVE_PASS
            || m_eActionState == ACTION_ONETIMER
            || m_eActionState == ACTION_LOOSE_BALL_SHOT
            || m_eActionState == ACTION_LOOSE_BALL_PASS))
    {
        fn_8003E354(this);
    }

    Goalie* pGoalie = m_pTeam->GetOtherTeam()->GetGoalie();
    if (pGoalie->mGoalieActionState == GOALIEACTION_UNIDENTIFIED_13
        && pGoalie->mpTarget == this)
    {
        fn_80080BFC(pGoalie, 0.0f);
    }
}
