#include "Game/AI/Fielder.h"

#include "Game/AI/ShotMeter.h"
#include "Game/Ball.h"
#include "Game/CharacterTweaks.h"
#include "Game/EventDataTypes.h"
#include "Game/Field.h"
#include "Game/GameInfo.h"
#include "Game/Net.h"
#include "Game/Physics/PhysicsCharacter.h"
#include "Game/SAnim/pnSAnimController.h"
#include "Game/Team.h"
#include "math.h"

struct UnidentifiedFielderInput
{
    u8 mUnidentified00[0x18];
    void* mUnidentified18;
};

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
extern "C" bool fn_8002F310(cFielder* pFielder);
extern "C" void fn_8001DCCC(
    cFielder* pFielder, unsigned short aParam, bool bParam);
extern "C" void fn_8005001C(cFielder* pFielder, bool bParam);
extern "C" void fn_80060608(void* pParam, cFielder* pFielder);
extern "C" void fn_800ED92C(unsigned long soundID);
extern "C" bool fn_801B6278(int stadium);
extern "C" bool fn_8001E168(const cCharacter* pCharacter);
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

void cFielder::PreUpdate(float fTime)
{
    cPlayer::PreUpdate(fTime);
    m_bHasBeenUpdated = false;
    mbWasHitByPowerupThisFrame = false;
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
                fn_8005001C(this, true);
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
        bool bExcluded
            = m_eCharacterClass == TOAD
           && fn_80319FEC(mUnidentified428->mUnidentified18, 0x17);
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
        bool bExcluded
            = m_eCharacterClass == LUIGI
           && fn_80319FEC(mUnidentified428->mUnidentified18, 0x17);
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

void cFielder::fn_8002FDC4(unsigned short aParam, bool bParam)
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

    fn_8001DCCC(this, aParam, bParam);
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
        if (mUnidentified464[i] == 0)
        {
            mUnidentified464[i] = pParam;
            return;
        }
    }
}

void cFielder::fn_800306DC()
{
    for (int i = 0; i < 4; i++)
    {
        mUnidentified464[i] = 0;
    }
}

bool cFielder::fn_800306F4(cFielder* pParam)
{
    for (int i = 0; i < 4; i++)
    {
        if (mUnidentified464[i] == pParam)
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
        if (fn_801B6278(stadium))
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
