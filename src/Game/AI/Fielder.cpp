#include "Game/AI/Fielder.h"
#include "Game/DetInput.h"
#include "Game/Audio/GameStreams.h"
#include "Game/RumbleActions.h"
#include "Game/AI/FielderDesireMachine.h"
#include "Game/AI/FielderInput.h"
#include "Game/AI/AIPad.h"
#include "Game/AI/AiUtil.h"
#include "Game/AI/FielderActions.h"
#include "Game/AI/DesireSteering.h"
#include "Game/AI/DesireReceivePass.h"
#include "Game/AI/DesireUsePowerup.h"
#include "Game/AI/HeadTrack.h"
#include "Game/AI/AvoidableObject.h"
#include "NL/nlMain.h"
#include "NL/nlString.h"
#include "NL/nlSlotPool.h"

#include "Game/AI/FuzzyVariant.h"
#include "Game/AI/ShotMeter.h"
#include "Game/Ball.h"
#include "Game/Render/BulletBill.h"
#include "Game/CharacterTweaks.h"
#include "Game/DebugWriteCache.h"
#include "Game/DB/StatsTracker.h"
#include "Game/EventDataTypes.h"
#include "Game/Field.h"
#include "Game/Game.h"
#include "Game/GameInfo.h"
#include "Game/GameTweaks.h"
#include "Game/Goalie.h"
#include "Game/MathHelpers.h"
#include "Game/Net.h"
#include "Game/Physics/PhysicsCharacter.h"
#include "Game/Physics/PhysicsColumn.h"
#include "Game/Physics/PhysicsAIBall.h"
#include "Game/Physics/PhysicsPatch.h"
#include "Game/Sys/audio.h"
#include "Game/Render/NPCManager.h"
#include "Game/Render/ChainChomp.h"
#include "Game/SAnim/pnFeather.h"
#include "Game/SAnim/pnSAnimController.h"
#include "Game/Team.h"
#include "Game/Task/FixedUpdateTask.h"
#include "math.h"
#include <stddef.h>
#include "Game/DB/StadiumInfo.h"
#include "Game/Physics/PhysicsWaluigiWall.h"

extern "C" shdStateMachine* fn_80319FC0(UnidentifiedScriptMachine*, int);
extern "C" shdStateMachine* fn_80319F94(UnidentifiedScriptMachine*, int);
extern "C" bool fn_80319FEC(UnidentifiedScriptMachine*, int);
extern "C" void fn_80319E58(UnidentifiedScriptMachine*, int);
extern "C" shdStateMachine* fn_80319E84(
    UnidentifiedScriptMachine*, int, UnidentifiedVariantCollection*, bool);
extern "C" void fn_80316968(shdStateMachine*);
extern "C" float fn_8002C0AC(PlayerTweaks*);
extern "C" float fn_8002CF10(PlayerTweaks*);
extern "C" float fn_8002C180(PlayerTweaks*);
extern "C" float fn_8002CF24(PlayerTweaks*);
extern "C" void fn_80039F24(cFielder*);
extern "C" void fn_8003A178(cFielder*);
extern "C" void fn_80039CF0(cFielder*, int);
extern "C" void fn_8003EAC0(cFielder*, float);
extern "C" float fn_800DDF54(cPlayer*, cPlayer*);
extern "C" float fn_8002BFA8(PlayerTweaks* pTweaks, float fTime);
extern "C" float fn_8002D020(PlayerTweaks* pTweaks);
extern "C" float fn_8002D050(PlayerTweaks* pTweaks);
extern "C" float fn_8002D038(PlayerTweaks* pTweaks);
extern "C" float fn_8002C254(const PlayerTweaks* pTweaks);
extern "C" float fn_8002BE38(PlayerTweaks* pTweaks);
extern "C" float fn_8002BE84(const PlayerTweaks* pTweaks);
extern "C" float fn_8002C758(PlayerTweaks* pTweaks);
extern "C" float fn_8002C780(PlayerTweaks* pTweaks);
extern "C" float fn_800156A8(cBall* pBall);
extern "C" void fn_800154FC(cBall* pBall, float fCharge);
extern "C" int fn_8002E9FC(cFielder*, cFielder*, float);
extern "C" void fn_80036594(cFielder*, cFielder*, int);
extern "C" void fn_80035194(cFielder*, nlVector3&, nlVector3&, int);
extern "C" void fn_8005EED0(cGame*, ShotAtGoalData*);
extern "C" void fn_8005ED64(void*, void*);
extern "C" void fn_80139D1C(int, DetInput*);
extern "C" bool fn_8002F310(cFielder* pFielder);
extern "C" void fn_80060608(void* pParam, cFielder* pFielder);
extern "C" void fn_800ED92C(unsigned long soundID);
extern "C" void fn_80097358(cPlayer*, float);
extern "C" void fn_8003E354(cFielder* pFielder);
extern "C" void fn_80080BFC(Goalie* pGoalie, float fDeltaT);
extern FuzzyVariant fvNotSet;
extern unsigned char lbl_806E0C61;
extern unsigned char lbl_806E0C62;
extern float lbl_806E3418;
extern float lbl_806E3420;
extern float lbl_806E3424;
extern float lbl_806E3428;
extern float lbl_806E342C;

float lbl_806DB6E8 = 0.22f;
float lbl_806DB74C = 0.2f;
float lbl_806DB750 = 5.0f;
float lbl_806DB754 = 0.6f;
float lbl_806DB760 = 0.2f;
float lbl_806DB764 = 0.6f;
float lbl_806DB768 = 0.1f;
float lbl_806DB76C = 1.0f;
float lbl_806DB770 = 0.6f;
float lbl_806DB774 = 0.1f;
float lbl_806DB778 = 0.2f;
float lbl_806DB77C = 0.6f;
float lbl_806DB780 = 1.0f;
float lbl_806DB784 = 0.6f;
float lbl_806DB788 = 4.5f;
float lbl_806DB790 = 1.1f;
float lbl_806DB794 = 1.1f;
float lbl_806DB798 = 1.0f;
float lbl_806DB7B8 = 27.5f;
float lbl_806DB7BC = 35.0f;
float lbl_806DB7C0 = 25.0f;
float lbl_806DB7C4 = 45.0f;
float lbl_806DB7C8 = 0.2f;
float lbl_806DB7CC = 1.2f;
float lbl_806DB7D0 = 25.0f;
float lbl_806DB7D4 = 5.0f;
float lbl_806DB7D8 = 13.0f;
float lbl_806DB7DC = 18.0f;
float lbl_806DB7E0 = 1.33f;
float lbl_806DB7E4 = 1.75f;
float lbl_806DB7E8 = 12.5f;
float lbl_806DB7EC = 23.5f;
float lbl_806DB7F0 = 0.2f;
float lbl_806DB7F4 = 0.33f;
float lbl_806DB7FC = 0.2f;
float lbl_806DB800 = -0.425f;
float lbl_806DB804 = 0.425f;
bool lbl_806DB808 = true;
float lbl_806DB80C = 5.0f;
float lbl_806DB810 = 0.9f;
float lbl_806DB81C = 8.3f;
float lbl_806DB820 = 12.075f;
float lbl_806DB824 = 8.875f;
float lbl_806DB828 = 20.0f;
bool lbl_806DB830 = true;
float lbl_806DB834 = 1.5f;
float lbl_806DB838 = 1.5f;
bool lbl_806E0C53;
bool lbl_806E0C58;
bool lbl_806E0C59;

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
    , mActionRunningVars()
    , mActionRunningWBVars()
    , mUnidentified388(0)
    , bAttackSucceeded(false)
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
    , mUnidentified3E8()
    , mUnidentified3F4(0.0f)
    , mUnidentified3F8()
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
    if (mUnidentified024.m_eCharacterClass == (eCharacterClass)6)
    {
        mUnidentified3F8.mUnidentified08
            = new (8, false) WaluigiWallManager();
    }
    else
    {
        mUnidentified3F8.mUnidentified08 = 0;
    }

    if (mUnidentified024.m_eCharacterClass == (eCharacterClass)19)
    {
        mUnidentified420 = gNPCManager->fn_801A9D20();
    }
    else
    {
        mUnidentified420 = 0;
    }
}

void cFielder::UnidentifiedVirtual1C()
{
    fn_80097648(-1.0f);
    SetAnimState(0, false, 0.0f, false, false);
    m_pCurrentAnimController->SetTime(0.0f);
    InitMovementNone(0.0f, 0.0f);
}

cFielder::~cFielder()
{
    CleanUpAction(ACTION_NEED_ACTION);
    if (mUnidentified024.m_eCharacterClass == (eCharacterClass)6)
    {
        delete mUnidentified3F8.mUnidentified08;
    }
    if (mUnidentified420 != 0)
    {
        mUnidentified420->Hide(true);
    }
    delete m_pShotMeter;
    mUnidentified428->fn_8030F74C(true, true);
    delete mUnidentified428;
}

float cFielder::fn_8002E058()
{
    return mUnidentified3A8;
}

extern "C" Desire* fn_8002E08C(cFielder* pFielder, int nAction)
{
    shdStateMachine* pAction
        = fn_80319F94(pFielder->mUnidentified428->mUnidentified18, nAction);
    if (pAction == 0)
    {
        pAction = fn_80319FC0(
            pFielder->mUnidentified428->mUnidentified18, nAction);
    }
    return (Desire*)pAction;
}

extern "C" int fn_8002E9FC(cFielder* pFielder,
    cFielder* pFielderCollidedWith, float attackIntensity)
{
    float fUnidentified0 = pFielder->m_pTweaks->mUnidentified064;
    float fUnidentified1 = pFielderCollidedWith->m_pTweaks->mUnidentified064;
    int nUnidentified = 1;
    if (lbl_806E0C62 || GameInfoManager::Instance()->IsRule0x8Equal1())
        return 2;

    if (fUnidentified1 < 0.0f && fUnidentified0 >= 0.0f)
        return 0;
    if (fUnidentified0 < 0.0f && fUnidentified1 >= 0.0f)
        return 2;

    attackIntensity -= 0.5f;
    float fUnidentified2 = fUnidentified1 - fUnidentified0 + attackIntensity;
    if (fUnidentified2 <= lbl_806DB800)
        nUnidentified = 0;
    else if (fUnidentified2 >= lbl_806DB804)
        nUnidentified = 2;

    if (fn_80319FEC(pFielderCollidedWith->mUnidentified428->mUnidentified18, 0x19))
    {
        nUnidentified = 2;
    }
    else
    {
        bool bUnidentified0 = false;
        bool bFrozen = ((DesireFrozen*)fn_80319FC0(
                           pFielderCollidedWith->mUnidentified428->mUnidentified18, 0x1D))
                          ->IsUnidentifiedState(1)
            || ((DesireFrozen*)fn_80319FC0(
                    pFielderCollidedWith->mUnidentified428->mUnidentified18, 0x1D))
                   ->IsUnidentifiedState(2);
        if (!bFrozen && (pFielderCollidedWith->muInvincibleStatus & 0x1F) == 0x1F)
            bUnidentified0 = true;
        if (bUnidentified0)
        {
            nUnidentified = 2;
        }
        else
        {
            bool bUnidentified1 = false;
            bool bFrozen = ((DesireFrozen*)fn_80319FC0(
                               pFielderCollidedWith->mUnidentified428->mUnidentified18, 0x1D))
                              ->IsUnidentifiedState(1)
                || ((DesireFrozen*)fn_80319FC0(
                        pFielderCollidedWith->mUnidentified428->mUnidentified18, 0x1D))
                       ->IsUnidentifiedState(2);
            if (!bFrozen && (pFielderCollidedWith->muInvincibleStatus & 1))
                bUnidentified1 = true;
            if (bUnidentified1)
            {
                nUnidentified = 2;
            }
            else
            {
                if (fn_800344DC(pFielderCollidedWith, &pFielder->mUnidentified024.m_v3Position)
                    || pFielderCollidedWith->fn_8003E74C())
                    nUnidentified = 2;
            }
        }
    }
    return nUnidentified;
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
            float netWidth = m_pTeam->m_pNet->GetNetWidth();
            float minYElectrocutionPosition
                = netWidth * lbl_806E3424 + netPostRadius;
            float netHeight = m_pTeam->m_pNet->GetNetHeight();
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
        float netWidth = m_pTeam->m_pNet->GetNetWidth();
        float minYElectrocutionPosition
            = netWidth * lbl_806E3424 + netPostRadius;
        float netHeight = m_pTeam->m_pNet->GetNetHeight();
        nlVector3 jointPos
            = GetJointPosition(m_nBip01JointIndex_0xA4);
        if ((float)fabs(eventData->contactPoint.y)
                > minYElectrocutionPosition
            || (float)fabs(jointPos.z) > netHeight)
        {
            bool bUnidentified = false;
            if (mUnidentified024.m_eCharacterClass == MARIO
                && fn_80319FEC(
                    mUnidentified428->mUnidentified18, 0x17))
            {
                bUnidentified = true;
            }

            if (bUnidentified && mUnidentified3DC)
            {
                fn_80060608(g_pGame, this);
                fn_8005001C(true);
                return false;
            }

            if ((mUnidentified024.m_eCharacterClass == MARIO
                    || mUnidentified024.m_eCharacterClass == DONKEYKONG
                    || mUnidentified024.m_eCharacterClass == (eCharacterClass)0x11)
                && m_eActionState == (eFielderActionState)1)
            {
                if (m_pCurrentAnimController->m_fTime > lbl_806E3428
                    || mUnidentified024.m_v3Position.z > lbl_806E3418)
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
                ->mHomeMegastrikeEnabled
            && m_pTeam->m_nSide == 0)
        {
            bUnidentified0 = true;
        }

        bool bUnidentified1 = false;
        if (bUnidentified0
            || (GameInfoManager::Instance()
                    ->GetCurrentSettings()
                    ->mAwayMegastrikeEnabled
                && m_pTeam->m_nSide == 1))
        {
            bUnidentified1 = true;
        }

        bool bUnidentified2 = fn_8001E168();
        if (bUnidentified1 && bUnidentified2)
        {
            float fRadius = lbl_806E3420;
            m_pPhysicsCharacter->GetRadius(&fRadius);

            float fOffset = lbl_806E342C + fRadius;
            float fMinX;
            float fMaxX = fOffset + mUnidentified024.m_v3Position.x;
            fMinX = mUnidentified024.m_v3Position.x - fOffset;
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

extern "C" UnidentifiedVariant_80054AB8 fn_80041AFC(
    InterpreterCore* runtime, const char* name, cFielder* fielder)
{
    return fn_80041B0C(runtime, fielder, name);
}

cFielder* cFielder::DoFindBestHitTarget()
{
    UnidentifiedVariant_80054AB8 vBestTarget = fn_80041AFC(
        fn_800E30A8(this), "BestHitTarget", this);
    if (vBestTarget.IsPointerType())
    {
        return (cFielder*)vBestTarget.mData.pPlayer;
    }
    return 0;
}

bool cFielder::fn_8003E6EC() const
{
    return fn_80319FEC(mUnidentified428->mUnidentified18, 0x17);
}

void cFielder::SetSlideAttackSuccessFlag()
{
    bAttackSucceeded = true;
}

UnidentifiedFuzzyRuntimeBase* cFielder::fn_8002E198() const
{
    return mUnidentified428->mUnidentified14;
}

bool cFielder::fn_8003E6FC() const
{
    return fn_80319FEC(mUnidentified428->mUnidentified18, 0x18);
}

bool cFielder::fn_8003E70C() const
{
    return fn_80319FEC(mUnidentified428->mUnidentified18, 0x1E);
}

bool cFielder::fn_8003E71C() const
{
    return fn_80319FEC(mUnidentified428->mUnidentified18, 0x19);
}

bool cFielder::fn_8003E72C() const
{
    return fn_80319FEC(mUnidentified428->mUnidentified18, 0x1A);
}

bool cFielder::fn_8003E73C() const
{
    return fn_80319FEC(mUnidentified428->mUnidentified18, 0x1C);
}

bool cFielder::fn_8003E74C() const
{
    bool result = false;
    if (fn_8003E7F8() || fn_8003E84C())
    {
        result = true;
    }
    return result;
}

bool cFielder::fn_8003E7F8() const
{
    bool result = false;
    if (mUnidentified024.m_eCharacterClass == (eCharacterClass)0
        && fn_80319FEC(mUnidentified428->mUnidentified18, 0x17))
    {
        result = true;
    }
    return result;
}

bool cFielder::fn_8003E84C() const
{
    bool result = false;
    if (mUnidentified024.m_eCharacterClass == (eCharacterClass)4
        && fn_80319FEC(mUnidentified428->mUnidentified18, 0x17))
    {
        result = true;
    }
    return result;
}

extern "C" bool fn_8003E8A0(const cFielder* pFielder)
{
    bool result = false;
    if (pFielder->mUnidentified024.m_eCharacterClass == (eCharacterClass)1
        && fn_80319FEC(pFielder->mUnidentified428->mUnidentified18, 0x17))
    {
        result = true;
    }
    return result;
}

bool cFielder::fn_8003E8F4() const
{
    bool result = false;
    if (mUnidentified024.m_eCharacterClass == LUIGI
        && fn_80319FEC(mUnidentified428->mUnidentified18, 0x17))
    {
        result = true;
    }
    return result;
}

bool cFielder::fn_8003E9F0() const
{
    bool result = false;
    if (mUnidentified024.m_eCharacterClass == (eCharacterClass)0xB
        && fn_80319FEC(mUnidentified428->mUnidentified18, 0x17))
    {
        result = true;
    }
    return result;
}

bool cFielder::fn_8003EA44() const
{
    bool result = false;
    if (mUnidentified024.m_eCharacterClass == HAMMERBROS
        && m_eActionState == (eFielderActionState)0x1D)
    {
        result = true;
    }
    return result;
}

bool cFielder::fn_8003EA6C() const
{
    bool result = false;
    if (mUnidentified024.m_eCharacterClass == TOAD
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
        bool bExcluded = fn_8003EA44();
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
            = mUnidentified024.m_eCharacterClass == DONKEYKONG
           && fn_80319FEC(mUnidentified428->mUnidentified18, 0x17);
        if (!bExcluded)
        {
            bCondition4 = true;
        }
    }

    if (bCondition4)
    {
        bool bExcluded
            = mUnidentified024.m_eCharacterClass == WALUIGI
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
        DesireFrozen* pAction = (DesireFrozen*)
            fn_80319FC0(mUnidentified428->mUnidentified18, 0x1D);
        bool bActionActive = false;
        if (pAction != 0 && pAction->mUnidentifiedActive
            && pAction->meFrozenState != 0)
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
        DesireConfused* pAction = (DesireConfused*)
            fn_80319F94(mUnidentified428->mUnidentified18, 0x1E);
        if (pAction == 0)
        {
            pAction = (DesireConfused*)fn_80319FC0(
                mUnidentified428->mUnidentified18, 0x1E);
        }
        pAction->fn_800BED24(&aParam);
    }

    cCharacter::Unknown8(aParam, bParam);
}

void cFielder::fn_8003057C(int nParam)
{
    DesireSteering* pAction = (DesireSteering*)
        fn_80319F94(mUnidentified428->mUnidentified18, 0x22);
    if (pAction == 0)
    {
        pAction = (DesireSteering*)fn_80319FC0(
            mUnidentified428->mUnidentified18, 0x22);
    }
    pAction->m_ThingsToAvoid = nParam;
}

void cFielder::fn_800305DC(float fParam)
{
    DesireSteering* pAction = (DesireSteering*)
        fn_80319F94(mUnidentified428->mUnidentified18, 0x22);
    if (pAction == 0)
    {
        pAction = (DesireSteering*)fn_80319FC0(
            mUnidentified428->mUnidentified18, 0x22);
    }
    pAction->m_fAvoidanceMult = fParam;
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

void cFielder::CollideWithCharacterCallback(CollisionPlayerPlayerData* pData)
{
    cPlayer* pPlayerCollidedWith = pData->player2;
    if (pPlayerCollidedWith->m_eClassType != FIELDER)
        return;

    cFielder* pFielderCollidedWith = (cFielder*)pPlayerCollidedWith;
    TestCollisionForInvicibility(pFielderCollidedWith);

    if (!IsOnSameTeam(pFielderCollidedWith))
    {
        if (IsFallenDown())
            return;

        u8 gotHit = pFielderCollidedWith->IsHitting();
        if (gotHit)
        {
            u8 hitteeIsHitter = 1;
            u8 bAlsoHitting = IsHitting();
            if (bAlsoHitting)
            {
                if (pFielderCollidedWith->m_pTweaks->mUnidentified064
                    < m_pTweaks->mUnidentified064)
                {
                    hitteeIsHitter = 0;
                }
                else if (pFielderCollidedWith->m_pTweaks->mUnidentified064
                    > m_pTweaks->mUnidentified064)
                {
                    hitteeIsHitter = 1;
                }
                else
                {
                    float fHitTime = fn_8002D038(pFielderCollidedWith->m_pTweaks)
                        / pFielderCollidedWith->m_pCurrentAnimController
                              ->m_pSAnim->m_nNumKeys;
                    float fMyHitTime = fabsf(m_pCurrentAnimController->m_fTime - fHitTime);
                    float fOtherHitTime = fabsf(pFielderCollidedWith->m_pCurrentAnimController->m_fTime - fHitTime);
                    if (fMyHitTime <= fOtherHitTime)
                        hitteeIsHitter = 0;
                }
            }

            if (!hitteeIsHitter)
                return;

            float thisRadius, otherRadius;
            m_pPhysicsCharacter->m_pPlayerPlayerColumn->GetRadius(&thisRadius);
            pFielderCollidedWith->m_pPhysicsCharacter->m_pPlayerPlayerColumn->GetRadius(&otherRadius);
            float combinedRadius = thisRadius + otherRadius;
            if (fabsf(mUnidentified024.m_v3Position.x) > thisRadius + cField::GetGoalLineX(1U))
                return;

            float sinVal, cosVal;
            nlSinCos(&sinVal, &cosVal, pFielderCollidedWith->mUnidentified024.m_aActualFacingDirection);
            nlVector3 adjustedPosition;
            adjustedPosition = pFielderCollidedWith->mUnidentified024.m_v3Position;
            adjustedPosition.x += cosVal * combinedRadius;
            adjustedPosition.y += sinVal * combinedRadius;

            float closingSpeed = GetClosingSpeed(adjustedPosition, pData->velocity1,
                pFielderCollidedWith->mUnidentified024.m_v3Position, v3Zero);
            float fRunningSpeed = fn_8002C254(m_pTweaks);
            float attackIntensity = NormalizeVal(closingSpeed, -fRunningSpeed, fRunningSpeed);
            int nUnidentified = fn_8002E9FC(this, pFielderCollidedWith, attackIntensity);
            u8 canPickup = 0;
            if (m_pBall != 0 && (attackIntensity >= lbl_806DB7FC || fn_8003E74C()))
            {
                if (lbl_806DB808)
                {
                    if (nUnidentified == 2 || fn_8003E74C())
                        canPickup = 1;
                }
                else
                {
                    canPickup = 1;
                }
            }

            if (canPickup)
            {
                PlaySound(0, 0xE89BA529, 0, 0);
            }
            else
            {
                switch (nUnidentified)
                {
                case 0:
                    PlaySound(0, 0x057208DA, 0, 0);
                    break;
                case 1:
                    PlaySound(0, 0xECE94BBB, 0, 0);
                    break;
                case 2:
                    PlaySound(0, 0xE8120AC5, 0, 0);
                    break;
                }
            }

            fn_80047240(pFielderCollidedWith,
                pFielderCollidedWith->mUnidentified024.m_aActualFacingDirection,
                nUnidentified, canPickup != 0, true);
            PlayerAttackData* pAttackData = g_PlayerAttackDataPool.Allocate();
            pAttackData->pAttacker = pFielderCollidedWith;
            u8 bHasGlobalPad = pFielderCollidedWith->GetGlobalPad() != 0;
            pAttackData->nAttackerPadID = bHasGlobalPad
                ? pFielderCollidedWith->GetGlobalPad()->GetPadID() : -1;
            pAttackData->pTarget = this;
            pAttackData->mUnidentified0C = nUnidentified;
            pAttackData->mUnidentified10 = false;
            fn_8005ED64(g_pGame, pAttackData);
            fn_80139D1C(2, pFielderCollidedWith->GetGlobalPad());
        }
        else if (pFielderCollidedWith->fn_80038660() && m_eActionState != ACTION_HIT)
        {
            s16 nHitteeToHitterFacingDelta = pFielderCollidedWith->GetFacingDeltaToPosition(mUnidentified024.m_v3Position);
            s16 nHitterToHitteeFacingDelta = GetFacingDeltaToPosition(pFielderCollidedWith->mUnidentified024.m_v3Position);
            u8 isThisSlideAttacking = fn_80038660();
            if (isThisSlideAttacking)
            {
                if (m_pTweaks->mUnidentified064
                    < pFielderCollidedWith->m_pTweaks->mUnidentified064)
                {
                    bool bHadBall = m_pBall != 0;
                    bool bUnidentified = g_pBall->m_v3Position.z > 0.66f;
                    InitActionSlideAttackReact(pFielderCollidedWith, false);
                    pFielderCollidedWith->bAttackSucceeded = true;
                    if (bHadBall && !bUnidentified)
                    {
                        pFielderCollidedWith->PickupBall(g_pBall);
                        fn_80036594(pFielderCollidedWith, this, 2);
                    }
                    else
                    {
                        fn_80036594(pFielderCollidedWith, this, 3);
                    }
                }
                else if (m_pTweaks->mUnidentified064
                    > pFielderCollidedWith->m_pTweaks->mUnidentified064)
                {
                    bool bHadBall = pFielderCollidedWith->m_pBall != 0;
                    bool bUnidentified = g_pBall->m_v3Position.z > 0.66f;
                    pFielderCollidedWith->InitActionSlideAttackReact(this, false);
                    bAttackSucceeded = true;
                    if (bHadBall && !bUnidentified)
                    {
                        PickupBall(g_pBall);
                        fn_80036594(this, pFielderCollidedWith, 2);
                    }
                    else
                    {
                        fn_80036594(this, pFielderCollidedWith, 3);
                    }
                }
                else if (mUnidentified024.m_fActualSpeed < pFielderCollidedWith->mUnidentified024.m_fActualSpeed)
                {
                    bool bHadBall = m_pBall != 0;
                    bool bUnidentified = g_pBall->m_v3Position.z > 0.66f;
                    InitActionSlideAttackReact(pFielderCollidedWith, false);
                    pFielderCollidedWith->bAttackSucceeded = true;
                    if (bHadBall && !bUnidentified)
                    {
                        pFielderCollidedWith->PickupBall(g_pBall);
                        fn_80036594(pFielderCollidedWith, this, 2);
                    }
                    else
                    {
                        fn_80036594(pFielderCollidedWith, this, 3);
                    }
                }
                else
                {
                    bool bHadBall = pFielderCollidedWith->m_pBall != 0;
                    bool bUnidentified = g_pBall->m_v3Position.z > 0.66f;
                    pFielderCollidedWith->InitActionSlideAttackReact(this, false);
                    bAttackSucceeded = true;
                    if (bHadBall && !bUnidentified)
                    {
                        PickupBall(g_pBall);
                        fn_80036594(this, pFielderCollidedWith, 2);
                    }
                    else
                    {
                        fn_80036594(this, pFielderCollidedWith, 3);
                    }
                }
            }
            else
            {
                bool bHadBall = m_pBall != 0;
                bool bUnidentified = g_pBall->m_v3Position.z > 0.66f;
                InitActionSlideAttackReact(pFielderCollidedWith, false);
                pFielderCollidedWith->bAttackSucceeded = true;
                if (bHadBall && !bUnidentified)
                {
                    pFielderCollidedWith->PickupBall(g_pBall);
                    fn_80036594(pFielderCollidedWith, this, 2);
                }
                else
                {
                    fn_80036594(pFielderCollidedWith, this, 3);
                }
            }
        }
        else if (m_eActionState == ACTION_LOOSE_BALL_PASS
            || m_eActionState == ACTION_LOOSE_BALL_SHOT)
        {
            nlVector3 v3Position = mUnidentified024.m_v3Position;
            float thisRadius, otherRadius;
            pFielderCollidedWith->m_pPhysicsCharacter->GetRadius(&otherRadius);
            m_pPhysicsCharacter->GetRadius(&thisRadius);
            nlVector2 v2Delta;
            nlVec2Set(v2Delta, pFielderCollidedWith->mUnidentified024.m_v3Position.x - v3Position.x,
                pFielderCollidedWith->mUnidentified024.m_v3Position.y - v3Position.y);
            float fOverlap = thisRadius + otherRadius - nlVec2Length(v2Delta);
            if (fOverlap <= 0.0f)
                return;

            nlVector3 v3BallDelta;
            nlVec3Set(v3BallDelta, g_pBall->m_v3Position.x - mUnidentified024.m_v3Position.x,
                g_pBall->m_v3Position.y - mUnidentified024.m_v3Position.y, 0.0f);
            nlVector3 v3PlayerDelta;
            nlVec3Set(v3PlayerDelta, pFielderCollidedWith->mUnidentified024.m_v3Position.x - mUnidentified024.m_v3Position.x,
                pFielderCollidedWith->mUnidentified024.m_v3Position.y - mUnidentified024.m_v3Position.y, 0.0f);
            nlVector3 v3Projection;
            nlVec3Scale(v3Projection, v3BallDelta,
                nlVec3DotProduct(v3PlayerDelta, v3BallDelta) / nlVec3LengthSquared(v3BallDelta));
            nlVector3 v3Direction;
            nlVec3Sub(v3Direction, v3PlayerDelta, v3Projection);
            if (nlVec3LengthSquared(v3Direction) < 0.001f)
            {
                v3Direction.x = v3BallDelta.y;
                v3Direction.y = -v3BallDelta.x;
            }
            nlVec3Scale(v3Direction, v3Direction,
                nlRecipSqrt(nlVec3LengthSquared(v3Direction), true));
            nlVec3ScaleAdd(v3Direction, fOverlap, v3Direction,
                pFielderCollidedWith->mUnidentified024.m_v3Position);
            pFielderCollidedWith->SetPosition(v3Direction);
        }
    }
    else if (pFielderCollidedWith->fn_80038660()
        && !pFielderCollidedWith->IsFallenDown()
        && !pFielderCollidedWith->fn_8003E74C())
    {
        bool bFrozen = ((DesireFrozen*)fn_80319FC0(
                           pFielderCollidedWith->mUnidentified428->mUnidentified18, 0x1D))
                          ->IsUnidentifiedState(1)
            || ((DesireFrozen*)fn_80319FC0(
                    pFielderCollidedWith->mUnidentified428->mUnidentified18, 0x1D))
                   ->IsUnidentifiedState(2);
        bool bUnidentified = !bFrozen && (pFielderCollidedWith->muInvincibleStatus & 1);
        if (!bUnidentified)
            pFielderCollidedWith->fn_8004D238();
    }
}

void cFielder::CollideWithWallCallback(
    const CollisionPlayerWallData* eventData)
{
    cPlayer::CollideWithWallCallback(eventData);

    DesireFrozen* pAction = (DesireFrozen*)
        fn_80319FC0(mUnidentified428->mUnidentified18, 0x1D);
    bool bActionActive = false;
    if (pAction != 0 && pAction->mUnidentifiedActive
        && pAction->meFrozenState != 0)
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
        float distance = (float)fabs(mUnidentified024.m_v3Position.y);
        distance -= fn_8002BFA8(m_pTweaks, lbl_806E3418);
        if (distance > cField::GetSidelineY(1) + lbl_806E3424)
        {
            fn_80046244();
        }
    }
}

bool cFielder::IsStuck() const
{
    return ((DesireFrozen*)fn_80319FC0(mUnidentified428->mUnidentified18, 0x1D))
               ->IsUnidentifiedState(1)
        || ((DesireFrozen*)fn_80319FC0(mUnidentified428->mUnidentified18, 0x1D))
               ->IsUnidentifiedState(2);
}

void cFielder::fn_80099074(const UnidentifiedEventData24* eventData)
{
    int type = eventData->mUnidentified10->m_Type;
    if (type == 1)
    {
        if (eventData->mUnidentified10->m_pOwner != this
            && mUnidentified1E4.m_tFireTimer.m_uPackedTime == 0
            && m_eActionState != ACTION_ELECTROCUTION
            && !IsStuck() && !IsInvincible())
        {
            if (m_eActionState == ACTION_UNKNOWN_34)
            {
                fn_80097358(this, lbl_806DB788);
                return;
            }
            if (m_pBall != 0)
            {
                ReleaseBall(0);
                ShootBallDueToContact(eventData->mUnidentified10->m_Velocity);
            }
            fn_8004E11C(lbl_806DB788);
            fn_80139D1C(2, GetGlobalPad());
        }
    }
    else if (type == 0)
    {
        if (eventData->mUnidentified10->m_pOwner != this
            && !IsStuck() && !IsInvincible())
        {
            UnidentifiedVariantCollection params;
            params.Set(7, FuzzyVariant(lbl_806DB750));
            fn_80319E84(mUnidentified428->mUnidentified18, 0x1E, &params,
                fn_80319FEC(mUnidentified428->mUnidentified18, 0x1E));
        }
    }
    else if (type == 2)
    {
        if (eventData->mUnidentified10->m_pOwner != this
            && !fn_800344B0() && m_eActionState != ACTION_UNKNOWN_34
            && !IsInvincible())
        {
            fn_80045AEC(eventData->mUnidentified10);
        }
        else if (eventData->mUnidentified10->m_pTarget != 0
            && eventData->mUnidentified10->m_pTarget == this)
        {
            eventData->mUnidentified10->fn_80173AF4();
        }
    }
    else if (type == 5 || type == 4 || type == 11)
    {
        if (eventData->mUnidentified10->m_pOwner != this
            && !fn_800344B0() && !IsInvincible())
        {
            PhysicsPatchInfo* info = GetPhysicsPatchInfo(type);
            UnidentifiedVariantCollection params;
            if (type == 5)
            {
                params.Set(0, FuzzyVariant(info->mFriction));
                params.Set(1, FuzzyVariant(lbl_806DB760));
                params.Set(2, FuzzyVariant(lbl_806DB764));
                params.Set(3, FuzzyVariant(lbl_806DB768));
            }
            else if (type == 4)
            {
                params.Set(0, FuzzyVariant(info->mFriction));
                params.Set(1, FuzzyVariant(lbl_806DB76C));
                params.Set(2, FuzzyVariant(lbl_806DB770));
                params.Set(3, FuzzyVariant(lbl_806DB774));
            }
            else if (type == 11)
            {
                params.Set(0, FuzzyVariant(lbl_806DB77C));
                params.Set(1, FuzzyVariant(lbl_806DB778));
                params.Set(2, FuzzyVariant(lbl_806DB780));
                params.Set(3, FuzzyVariant(lbl_806DB784));
            }
            if (type == 4)
            {
                AddRandomDirt();
                fn_8001F1C0(1);
            }
            fn_80319E84(mUnidentified428->mUnidentified18, 0x1B, &params,
                fn_80319FEC(mUnidentified428->mUnidentified18, 0x1B));
        }
    }
    else if (type == 3)
    {
        if (eventData->mUnidentified10->m_pOwner != this
            && !fn_800344B0() && !IsInvincible())
        {
            AddRandomDirt();
            fn_8001F1C0(1);
            nlVector3 v3Unidentified = eventData->mUnidentified10->m_Velocity;
            v3Unidentified.z = 0.0f;
            if (nlVec3LengthSquared(v3Unidentified) == 0.0f)
            {
                nlVec3Set(v3Unidentified,
                    mUnidentified024.m_v3Position.x - eventData->mUnidentified10->m_pOwner->mUnidentified024.m_v3Position.x,
                    mUnidentified024.m_v3Position.y - eventData->mUnidentified10->m_pOwner->mUnidentified024.m_v3Position.y,
                    0.0f);
            }
            nlVec3Scale(v3Unidentified,
                nlRecipSqrt(nlVec3LengthSquared(v3Unidentified), false));
            nlPolar polar;
            nlCartesianToPolar(polar, v3Unidentified);
            fn_80047240(eventData->mUnidentified10->m_pOwner,
                polar.a, 1, false, false);
        }
    }
    else if (type == 7)
    {
        if (eventData->mUnidentified10->m_pOwner != this
            && !fn_800344B0() && !IsInvincible())
        {
            cPlayer* pOwner = eventData->mUnidentified10->m_pOwner;
            if (lbl_806E0C59 && IsOnSameTeam(pOwner))
            {
                return;
            }
            fn_800470B4(this, eventData->mUnidentified10->m_pOwner);
            if (fn_80319FEC(mUnidentified428->mUnidentified18, 0x1C))
            {
                return;
            }
            UnidentifiedVariantCollection params;
            params.Set(14, FuzzyVariant(pOwner));
            fn_80319E84(mUnidentified428->mUnidentified18, 0x1C, &params, false);
        }
    }
    else if (type == 6)
    {
        cFielder* pOwner = (cFielder*)eventData->mUnidentified10->m_pOwner;
        if (pOwner != this && !IsFallenDown())
        {
            if (fn_8003E74C() || IsInvincibleChars())
            {
                if (!pOwner->IsInvincibleChars())
                {
                    pOwner->fn_80047240(this,
                        pOwner->mUnidentified024.m_aActualFacingDirection + 0x8000, 1, false, false);
                    PlaySound(0, 0xECE94BBB, 0, 0);
                }
                else
                {
                    fn_80047240(pOwner, pOwner->mUnidentified024.m_aActualFacingDirection, 0, false, false);
                    PlaySound(pOwner->mUnidentified318, 0x9E87FEBC, 0, 0);
                    fn_80139D1C(2, pOwner->GetGlobalPad());
                }
            }
            else if (!IsOnSameTeam(pOwner))
            {
                fn_80047240(pOwner, pOwner->mUnidentified024.m_aActualFacingDirection, 1, false, true);
                PlaySound(pOwner->mUnidentified318, 0x9E87FEBC, 0, 0);
                fn_80139D1C(2, pOwner->GetGlobalPad());
            }
        }
    }
    else if (type == 12)
    {
        fn_80319E84(mUnidentified428->mUnidentified18, 0x19, 0, true);
    }
    else if (type == 8 || type == 9)
    {
        if (!fn_800344B0() && !IsInvincible()
            && m_eActionState != (eFielderActionState)0x18
            && m_eActionState != (eFielderActionState)0x23)
        {
            fn_80139D1C(3, GetGlobalPad());
            nlVector3 v3Unidentified = mUnidentified024.m_v3Velocity;
            v3Unidentified.z = 25.0f;
            fn_80044148(v3Unidentified);
            fn_80097358(this, 5.0f);
        }
    }
    else if (type == 10)
    {
        if (!fn_800344B0() && !fn_8003E6FC()
            && !fn_8003EA6C() && !IsInvincible()
            && !IsCharacterInAir(eventData->mUnidentified10->GetRadius()))
        {
            fn_80139D1C(3, GetGlobalPad());
            nlVector3 v3Start;
            nlVec3ScaleAdd(v3Start, -100.0f,
                eventData->mUnidentified10->fn_80173CCC(),
                eventData->mUnidentified10->GetPosition());
            nlVector3 v3End;
            nlVec3ScaleAdd(v3End, 100.0f,
                eventData->mUnidentified10->fn_80173CCC(),
                eventData->mUnidentified10->GetPosition());
            nlVector3 v3Unidentified = GetClosestPointOnLineABFromPointC(
                v3Start, v3End, mUnidentified024.m_v3Position);
            nlVec3Set(v3Unidentified,
                mUnidentified024.m_v3Position.x - v3Unidentified.x,
                mUnidentified024.m_v3Position.y - v3Unidentified.y, 0.0f);
            nlVec3Scale(v3Unidentified,
                nlRecipSqrt(nlVec3LengthSquared(v3Unidentified), false));
            InitActionElectrocution(mUnidentified024.m_v3Position, v3Unidentified, false);
        }
    }
}

void cFielder::ClearPassTargetIfAmThePassTarget()
{
    cBall* pBall = g_pBall;
    if (pBall->UnidentifiedHasPassTarget())
    {
        if (pBall->m_pPassTarget == this)
        {
            if (pBall->m_pOwner != 0)
            {
                fn_80015C38(pBall, 2);
            }
            else
            {
                fn_80015C38(pBall, 0);
            }
        }
    }
}

bool cFielder::fn_800344B0() const
{
    switch (m_eActionState)
    {
    case 3:
    case 0x18:
        return true;
    default:
        return false;
    }
}

extern "C" bool fn_800344DC(cFielder* pFielder, const nlVector3* position)
{
    bool bUnidentified = false;
    bool bFrozen = ((DesireFrozen*)fn_80319FC0(
                       pFielder->mUnidentified428->mUnidentified18, 0x1D))
                      ->IsUnidentifiedState(1)
        || ((DesireFrozen*)fn_80319FC0(
                pFielder->mUnidentified428->mUnidentified18, 0x1D))
               ->IsUnidentifiedState(2);
    if (!bFrozen && (pFielder->muInvincibleStatus & 2))
        bUnidentified = true;

    s16 facingDelta = pFielder->GetFacingDeltaToPosition(*position);
    bool result = false;
    if (bUnidentified && (u16)(facingDelta < 0 ? -facingDelta : facingDelta) < 0x4000)
        result = true;
    return result;
}

bool cFielder::fn_800345EC(cFielder* pOtherFielder) const
{
    if (pOtherFielder->mUnidentified024.m_eCharacterClass == DAISY
        && pOtherFielder->m_eActionState == 1)
    {
        return IsCharacterInAir(pOtherFielder->mUnidentified024.m_fPlayerScale);
    }
    if (mUnidentified024.m_eCharacterClass == DAISY && m_eActionState == 1)
        return false;
    if (mUnidentified024.m_eCharacterClass == TOAD && m_eActionState == 1)
        return false;
    if (mUnidentified024.m_eCharacterClass == WARIO && m_eActionState == 0x1E)
        return false;
    if (pOtherFielder->mUnidentified024.m_eCharacterClass == WALUIGI
        && pOtherFielder->m_eActionState == ACTION_SLIDE_ATTACK)
    {
        float fPlayerScale = pOtherFielder->mUnidentified024.m_fPlayerScale;
        fPlayerScale = 0.5f * fPlayerScale;
        return IsCharacterInAir(fPlayerScale);
    }
    if (mUnidentified024.m_eCharacterClass == WALUIGI && m_eActionState == ACTION_SLIDE_ATTACK)
        return false;
    if (pOtherFielder->mUnidentified024.m_eCharacterClass == (eCharacterClass)0x10
        && pOtherFielder->m_eActionState == ACTION_SLIDE_ATTACK)
    {
        float fPlayerScale = pOtherFielder->mUnidentified024.m_fPlayerScale;
        fPlayerScale = 0.6f * fPlayerScale;
        return IsCharacterInAir(fPlayerScale);
    }
    if (mUnidentified024.m_eCharacterClass == (eCharacterClass)0x10
        && m_eActionState == ACTION_SLIDE_ATTACK)
        return false;

    float leftFootZ = GetJointPosition(m_nLeftFootJointIndex).z;
    float rightFootZ = GetJointPosition(m_nRightFootJointIndex).z;
    bool bRunning = false;
    eFielderActionState eActionState = m_eActionState;
    if (eActionState == ACTION_RUNNING || eActionState == 0x13
        || IsRunningWithBall())
    {
        bRunning = true;
    }
    if (bRunning)
        leftFootZ = rightFootZ = 0.0f;

    nlVector3 v3Unidentified0, v3Unidentified1;
    m_pPhysicsCharacter->GetBonePositions(
        PHYSBONE_FIELDER_HEAD, v3Unidentified0, v3Unidentified1);
    leftFootZ = nlMinEquals(nlMinEquals(leftFootZ, rightFootZ), v3Unidentified0.z)
        - 0.15f;
    if (leftFootZ < 0.0f)
        leftFootZ = 0.0f;

    float fUnidentified1 = pOtherFielder->GetJointPosition(
                                          pOtherFielder->m_nLeftFootJointIndex)
                              .z;
    float fUnidentified2 = pOtherFielder->GetJointPosition(
                                          pOtherFielder->m_nRightFootJointIndex)
                              .z;
    nlVector3 v3Unidentified2, v3Unidentified3;
    pOtherFielder->m_pPhysicsCharacter->GetBonePositions(
        PHYSBONE_FIELDER_HEAD, v3Unidentified2, v3Unidentified3);
    float fUnidentified3 = nlMaxEquals(
        nlMaxEquals(fUnidentified1, fUnidentified2), v3Unidentified2.z);
    fUnidentified3 += 0.15f;
    if (leftFootZ > fUnidentified3)
        return true;
    return false;
}

bool cFielder::fn_80034894(cFielder* pOtherFielder) const
{
    switch (m_eActionState)
    {
    case ACTION_LOOSE_BALL_PASS:
    case ACTION_LOOSE_BALL_SHOT:
    {
        float fUnidentified0 = m_pTweaks->mUnidentified064;
        float fUnidentified1 = pOtherFielder->m_pTweaks->mUnidentified064;
        if (fUnidentified1 > fUnidentified0)
            return false;
        if (fUnidentified0 > fUnidentified1)
            return true;

        switch (pOtherFielder->m_eActionState)
        {
        case ACTION_LOOSE_BALL_PASS:
        case ACTION_LOOSE_BALL_SHOT:
        case ACTION_ONETIMER:
        case ACTION_RECEIVE_PASS:
        {
            float fAnimTime = m_pCurrentAnimController->m_fTime;
            float fUnidentified2 = mUnidentified368 - fAnimTime;
            if (fUnidentified2 > 0.0f
                && fUnidentified2 <= pOtherFielder->mUnidentified368
                                         - pOtherFielder->m_pCurrentAnimController->m_fTime)
            {
                return true;
            }
            break;
        }
        default:
            return true;
        }
        break;
    }
    default:
        break;
    }
    return false;
}

bool cFielder::IsRunning() const
{
    bool bRunning = false;
    if (m_eActionState == ACTION_RUNNING || m_eActionState == 0x13
        || IsRunningWithBall())
    {
        bRunning = true;
    }
    return bRunning;
}

void cFielder::CleanUpAction(eFielderActionState actionState)
{
    switch (m_eActionState)
    {
    case ACTION_HIT:
        if (m_pController != 0)
        {
            m_pController->ResetAccelerationHistory();
        }
        if (mUnidentified024.m_eCharacterClass == (eCharacterClass)8)
        {
            m_pHeadTrack->UnidentifiedReset();
            for (int i = 0; i < 60; ++i)
            {
                PhysicsPatch* pPatch = lbl_806E12C8->fn_801745B8(i);
                if (pPatch != 0 && pPatch->m_Type == 6)
                {
                    pPatch->Unknown0();
                }
            }
        }
        break;

    case 0:
        m_ModelType = CharModel_Rigid;
        mUnidentified024.m_v3Position.z = 0.0f;
        mUnidentified024.m_v3Velocity.z = 0.0f;
        fn_801B93E8(this);
        break;

    case 1:
        fn_80039F24(this);
        break;

    case ACTION_ELECTROCUTION:
        m_ModelType = CharModel_Rigid;
        mUnidentified024.m_v3Position.z = 0.0f;
        mUnidentified024.m_v3Velocity.z = 0.0f;
        fn_801B93E8(this);
        break;

    case 3:
        mUnidentified17C = true;
        mUnidentified024.m_v3Position.z = 0.0f;
        mUnidentified024.m_v3Velocity.z = 0.0f;
        if (GameInfoManager::Instance()->GetStadium() == 0x0B)
        {
            m_pPhysicsCharacter->m_CanCollideWithGoalLine = true;
            m_pPhysicsCharacter->m_CanCollideWithWall = true;
        }
        break;

    case 0x18:
        mUnidentified024.m_v3Position.z = 0.0f;
        mUnidentified024.m_v3Velocity.z = 0.0f;
        break;

    case ACTION_LOOSE_BALL_PASS:
        m_pPhysicsCharacter->m_CanCollideWithWall = true;
        SetNoPickUpTime(0.0f);
        bIsModified = false;
        break;

    case ACTION_LOOSE_BALL_SHOT:
        m_pPhysicsCharacter->m_CanCollideWithWall = true;
        SetNoPickUpTime(0.0f);
        bIsModified = false;
        break;

    case ACTION_ONETIMER:
        EndBlur();
        bIsModified = false;
        break;

    case ACTION_PASS:
        mUnidentified36C = 0;
        bIsModified = false;
        break;

    case ACTION_RUNNING:
        mActionRunningVars.eLastStrafeDirection = STRAFE_IDLE;
        m_tMoveToTurboTimer.UnidentifiedClear();
        if (fn_8003EA6C())
        {
            if (fn_80319FEC(mUnidentified428->mUnidentified18, 0x17))
            {
                fn_80316968(fn_8002E08C(this, 0x17));
            }
        }
        break;

    case 0x13:
    {
        PlayerTweaks* pTweaks = m_pTweaks;
        InitMovementRunning(fn_8002C0AC(pTweaks),
            fn_8002CF10(pTweaks), fn_8002C180(pTweaks),
            fn_8002CF24(pTweaks));
        mActionRunningVars.eLastStrafeDirection = STRAFE_IDLE;
        mUnidentified374.mUnidentified04 = 0.0f;
        m_tMoveToTurboTimer.UnidentifiedClear();
        mUnidentified374 = UnidentifiedFielderPair374();
        break;
    }

    case ACTION_RUNNING_WB:
        mUnidentified1E4.m_eLastPadAction = 50;
        if (fn_8003EA6C())
        {
            if (fn_80319FEC(mUnidentified428->mUnidentified18, 0x17))
            {
                fn_80316968(fn_8002E08C(this, 0x17));
            }
        }
        break;

    case ACTION_UNKNOWN_15:
        fn_8003A178(this);
        break;

    case ACTION_SLIDE_ATTACK:
        fn_801BB5DC(this, 0);
        StopSound(0x2AE03886, this);
        break;

    case 0x1C:
        KillDaze(this);
        if (fn_8003E8A0(this))
        {
            fn_801B97DC(this);
        }
        if (mUnidentified024.m_eCharacterClass == (eCharacterClass)0x12 && !mbTangible)
        {
            m_pPhysicsCharacter->m_CanCollideWithBall = true;
            m_pPhysicsCharacter->m_CanCollideWithCharacters = true;
            if (m_pBall != 0)
            {
                g_pBall->m_pPhysicsBall->mbCanCollideGoalie = true;
                g_pBall->m_pPhysicsBall->mbCanCollidePlayer = true;
            }
            mbTangible = true;
            if (m_pBall != 0)
            {
                m_pBall->m_bVisible = true;
            }
            mUnidentified17C = true;
            mUnidentified178 = 1.0f;
        }
        break;

    case ACTION_UNKNOWN_30:
        fn_8004BF58(actionState);
        break;

    case ACTION_UNKNOWN_32:
        fn_8004EC40();
        break;

    case 0x21:
    {
        Goalie* pGoalie = m_pTeam->GetOtherTeam()->GetGoalie();
        pGoalie->m_pPhysicsCharacter->m_CanCollideWithBall = true;
        g_pBall->m_pPhysicsBall->mbCanCollideGoalie = true;
        g_pBall->m_pPhysicsBall->mbCanCollidePlayer = true;
        mUnidentified410.mUnidentified0C = false;
        nlVector3 v3Position = mUnidentified024.m_v3Position;
        if (v3Position.z != 0.0f)
        {
            v3Position.z = 0.0f;
            SetPosition(v3Position);
        }
        fn_80039CF0(this, 0);
        break;
    }

    case ACTION_UNKNOWN_34:
        fn_8004F180();
        break;

    case 0x23:
        if (actionState != 3 && actionState != 0x18)
        {
            mUnidentified024.m_v3Position.z = 0.0f;
            mUnidentified024.m_v3Velocity.z = 0.0f;
        }
        if (GameInfoManager::Instance()->GetStadium() == 0x0B)
        {
            m_pPhysicsCharacter->m_CanCollideWithGoalLine = true;
            m_pPhysicsCharacter->m_CanCollideWithWall = true;
        }
        break;

    default:
        break;
    }

    m_eActionState = ACTION_NEED_ACTION;
}

void cFielder::ShootBallDueToContact(const nlVector3& v3IncomingVelocity)
{
    if (m_eActionState == ACTION_SHOOT_TO_SCORE || m_eActionState == ACTION_SHOT)
    {
        g_pBall->ShootRelease(v3Zero, SPINTYPE_NONE);
        return;
    }

    nlVector3 v3ReleaseVelocity;
    nlVec3Add(v3ReleaseVelocity, v3IncomingVelocity, mUnidentified024.m_v3Velocity);
    if (nlVec3LengthSquared(v3IncomingVelocity) < 0.001f * 0.001f
        || nlVec3LengthSquared(mUnidentified024.m_v3Velocity) < 0.001f * 0.001f
        || nlVec3LengthSquared(v3ReleaseVelocity) < 0.001f * 0.001f)
    {
        nlVector3 v3ReleaseVelocity;
        nlPolarToCartesian(v3ReleaseVelocity.x, v3ReleaseVelocity.y,
            mUnidentified024.m_aActualFacingDirection, 2.0f);
        v3ReleaseVelocity.z = 0.5f;
        g_pBall->ShootRelease(v3ReleaseVelocity, SPINTYPE_NONE);
        return;
    }

    nlVec3Normalize(v3ReleaseVelocity, v3ReleaseVelocity);
    nlVec3Scale(v3ReleaseVelocity, v3ReleaseVelocity, 2.0f + mUnidentified024.m_fActualSpeed);
    v3ReleaseVelocity.z = 0.5f;
    g_pBall->ShootRelease(v3ReleaseVelocity, SPINTYPE_NONE);
}

void cFielder::ShootBallDueToContact(unsigned short aShootDirection)
{
    nlVector3 v3ReleaseVelocity;
    float fRadius = mUnidentified024.m_fActualSpeed;
    fRadius = 2.0f + fRadius;
    nlPolarToCartesian(v3ReleaseVelocity.x, v3ReleaseVelocity.y,
        aShootDirection, fRadius);
    v3ReleaseVelocity.z = 0.5f;

    g_pBall->ShootRelease(v3ReleaseVelocity, SPINTYPE_NONE);
}

void cFielder::DoClearBall()
{
    nlVector3 v3Target;
    nlVector3 v3ClearBallVelocity;
    float fPositionValue = lbl_806DB7F0 * InterpolateRangeClamped(0.0f, 1.0f,
        cField::GetGoalLineX(1U), 0.0f, nlAbs(mUnidentified024.m_v3Position.x));
    float fBallChargeValue = (1.0f - lbl_806DB7F0) * fn_800156A8(g_pBall);
    float fDesiredTime = Interpolate(lbl_806DB7E0, lbl_806DB7E4,
        fBallChargeValue + fPositionValue);
    float fShotMeterValue = lbl_806DB7F4 * m_pShotMeter->m_fSpeedValue;
    float fPlayerValue = (1.0f - lbl_806DB7F4) * fn_8002BE38(m_pTweaks);
    float fClearDistance = Interpolate(lbl_806DB7E8, lbl_806DB7EC,
        fShotMeterValue + fPlayerValue);

    u16 aClearingAngle = mUnidentified024.m_aActualFacingDirection;
    if (m_pController != NULL)
    {
        if (m_pController->GetMovementStickMagnitude() > 0.01f)
        {
            aClearingAngle = m_pController->GetMovementStickDirection();
        }
    }
    else
    {
        aClearingAngle = (u16)nlRandom(0xFFFF);
        for (int i = 0; i < 4; i++)
        {
            cFielder* pFielder = m_pTeam->m_pAIOrderedFielders[i];
            if (pFielder != this && pFielder->CanReceivePass()
                && AIsgn(pFielder->mUnidentified024.m_v3Position.x) != AIsgn(mUnidentified024.m_v3Position.x))
            {
                if (nlSqrt(nlVec3DistanceSquared2D(pFielder->mUnidentified024.m_v3Position,
                        mUnidentified024.m_v3Position), true) > 0.5f * fClearDistance)
                {
                    nlVector3 v3Direction;
                    nlVec3Sub(v3Direction, pFielder->mUnidentified024.m_v3Position, mUnidentified024.m_v3Position);
                    nlPolar pDirection;
                    nlCartesianToPolar(pDirection, v3Direction);
                    aClearingAngle = pDirection.a;
                }
            }
        }
    }

    if (!lbl_806E0C53)
    {
        nlVector3 v3Top;
        nlVector3 v3Bottom;
        nlVector3 v3Net;
        v3Top.x = v3Bottom.x = 5.0f * AIsgn(m_pTeam->GetOtherNet()->m_v3NetLocation.x);
        v3Top.y = cField::GetSidelineY(1);
        v3Bottom.y = cField::GetSidelineY(0);
        v3Top.z = v3Bottom.z = 0.0f;
        nlVec3Sub(v3Net, m_pTeam->GetOtherNet()->m_v3NetLocation, mUnidentified024.m_v3Position);
        nlVec3Sub(v3Top, v3Top, mUnidentified024.m_v3Position);
        nlVec3Sub(v3Bottom, v3Bottom, mUnidentified024.m_v3Position);

        nlPolar pClearingTopAngle;
        nlPolar pClearingBottomAngle;
        nlPolar pNet;
        nlCartesianToPolar(pClearingTopAngle, v3Top);
        nlCartesianToPolar(pClearingBottomAngle, v3Bottom);
        nlCartesianToPolar(pNet, v3Net);
        s16 nDelta = nlAngleDiff(pNet.a, aClearingAngle);
        s16 nTopDelta = nlAngleDiff(pNet.a, pClearingTopAngle.a);
        s16 nBottomDelta = nlAngleDiff(pNet.a, pClearingBottomAngle.a);
        if (abs_ang16(nDelta) < abs_ang16(nTopDelta)
            && abs_ang16(nDelta) < abs_ang16(nBottomDelta))
        {
            nlVector3 v3TopPost;
            nlVector3 v3BottomPost;
            m_pTeam->GetOtherNet()->GetPostLocation(v3TopPost, 0, 0.0f);
            v3BottomPost = v3TopPost;
            v3BottomPost.y = -v3TopPost.y;
            nlVec3Sub(v3TopPost, v3TopPost, mUnidentified024.m_v3Position);
            nlVec3Sub(v3BottomPost, v3BottomPost, mUnidentified024.m_v3Position);
            nlPolar pTopPost;
            nlPolar pBottomPost;
            nlCartesianToPolar(pTopPost, v3TopPost);
            nlCartesianToPolar(pBottomPost, v3BottomPost);
            s16 nTopPostDelta = nlAngleDiff(pNet.a, pTopPost.a);
            s16 nBottomPostDelta = nlAngleDiff(pNet.a, pBottomPost.a);
            if (abs_ang16(nDelta) < abs_ang16(nTopPostDelta)
                && abs_ang16(nDelta) < abs_ang16(nBottomPostDelta))
            {
                if (abs_ang16(nTopPostDelta) < abs_ang16(nBottomPostDelta))
                {
                    aClearingAngle = pTopPost.a;
                }
                else
                {
                    aClearingAngle = pBottomPost.a;
                }
            }
        }
        else
        {
            nBottomDelta = nlAngleDiff(pClearingBottomAngle.a, aClearingAngle);
            nTopDelta = nlAngleDiff(pClearingTopAngle.a, aClearingAngle);
            aClearingAngle = abs_ang16(nTopDelta) < abs_ang16(nBottomDelta)
                ? pClearingTopAngle.a : pClearingBottomAngle.a;
        }
    }

    nlVector3 v3Direction;
    nlPolarToCartesian(v3Direction.x, v3Direction.y, aClearingAngle, fClearDistance);
    v3Direction.z = 0.0f;
    nlVec3Add(v3Target, mUnidentified024.m_v3Position, v3Direction);
    if (m_pBall != NULL)
    {
        ReleaseBall(1);
    }
    if (lbl_806DB830 && m_eClassType == FIELDER)
    {
        float fCharge = Interpolate(lbl_806DB834, lbl_806DB838,
            InterpolateRangeClamped(0.0f, 1.0f, 0.5f, 1.0f, fn_8002BE38(m_pTweaks)));
        fn_800154FC(g_pBall, fCharge + fn_800155A0(g_pBall, 0));
    }
    g_pBall->ShootAtFast(v3ClearBallVelocity, v3Target, fDesiredTime);
    g_pBall->ShootRelease(v3ClearBallVelocity, SPINTYPE_BACK);
    SetNoPickUpTime(0.2f);
}

void cFielder::DoFindBestShotTarget(nlVector3& v3PositionOut, float& fShotSpeed, int nParam)
{
    cBall* pBall = g_pBall;
    Goalie* pGoalie = m_pTeam->GetOtherTeam()->GetGoalie();

    float kBallAllowance = 0.18f + cNet::m_fNetPostRadius;
    kBallAllowance += gGameTweaks.m_pGameTweaks->fShotPostOffset;
    float fDist2NetSide = 0.5f * cNet::m_fNetWidth - kBallAllowance;
    cNet* pNet = m_pTeam->GetOtherNet();
    nlVector3 v3Target;
    v3Target.x = pNet->m_v3NetLocation.x;
    v3Target.y = nlMinEquals(nlMaxEquals(pBall->m_v3Position.y, -fDist2NetSide), fDist2NetSide);
    v3Target.z = nlMinEquals(nlMaxEquals(pBall->m_v3Position.z, 0.18f),
        cNet::m_fNetHeight - kBallAllowance);
    float fShotDist = nlSqrt(nlVec3DistanceSquared2D(pBall->m_v3Position, v3Target), true);

    if (nParam == 8 && (mUnidentified024.m_eCharacterClass == 14 || mUnidentified024.m_eCharacterClass == 12))
    {
        if (mUnidentified024.m_eCharacterClass == 14)
        {
            fShotSpeed = lbl_806DB7B8;
        }
        else if (mUnidentified024.m_eCharacterClass == 12)
        {
            fShotSpeed = lbl_806DB7BC;
        }
    }
    else if (nParam == 7)
    {
        fShotSpeed = InterpolateRangeClamped(lbl_806DB81C, lbl_806DB820,
            lbl_806DB824, lbl_806DB828, fShotDist);
    }
    else
    {
        float speedFactor = InterpolateRangeClamped(0.0f, 1.0f, 18.0f, 6.0f, fShotDist);
        float fCharge = fn_800156A8(g_pBall);
        float fShotMinSpeed = fn_8002C780(m_pTweaks);
        float fShotMaxSpeed = fn_8002C758(m_pTweaks);
        fShotSpeed = Interpolate(fShotMinSpeed, fShotMaxSpeed, fCharge);
    }

    float fAbsBallX = fabsf(pBall->m_v3Position.x);
    float fAimValue = m_pShotMeter->GetShotAimValue();
    float fAbsAimValue = fabsf(fAimValue);
    float fAbsBallY = fabsf(pBall->m_v3Position.y);

    if (fAbsBallY < 1.5f + fDist2NetSide
        && (fAbsBallX > fabsf(pGoalie->mUnidentified024.m_v3Position.x)
            || fAbsBallX > cField::GetGoalLineX(1U) - 1.5f))
    {
        v3PositionOut.x = 1.005f * pNet->m_v3NetLocation.x;
        v3PositionOut.y = 0.9f * v3Target.y;
        v3PositionOut.z = v3Target.z + nlRandomf(0.2f);
        if (fShotDist < 2.0f)
        {
            fShotSpeed = 12.0f;
        }
        if (fAbsBallY < fDist2NetSide
            && pBall->m_v3Position.z < cNet::m_fNetHeight - kBallAllowance)
        {
            v3PositionOut.x = 1.1f * pNet->m_v3NetLocation.x;
        }
    }
    else
    {
        nlVector3 v3Post1Delta;
        nlVector3 v3Post2Delta;
        nlVector3 v3GoalieDelta;
        float fNetBaseY = pNet->m_v3NetLocation.y;
        nlVector3 v3Post1 = pNet->m_v3NetLocation;
        nlVector3 v3Post2 = pNet->m_v3NetLocation;
        v3Post1.y = fNetBaseY - fDist2NetSide;
        v3Post2.y = fNetBaseY + fDist2NetSide;

        nlVec3Sub(v3Post1Delta, v3Post1, pBall->m_v3Position);
        nlVec3Sub(v3Post2Delta, v3Post2, pBall->m_v3Position);
        nlVec3Sub(v3GoalieDelta, pGoalie->mUnidentified024.m_v3Position, pBall->m_v3Position);

        u16 aAngPost1 = nlVector3ToAngle(v3Post1Delta);
        u16 aAngPost2 = nlVector3ToAngle(v3Post2Delta);
        u16 aAngGoalie = nlVector3ToAngle(v3GoalieDelta);
        u16 uAbsP1G = (u16)abs_s16(nlAngleDiff(aAngPost1, aAngGoalie));
        u16 uAbsP2G = (u16)abs_s16(nlAngleDiff(aAngPost2, aAngGoalie));
        u16 uAbsP1P2 = (u16)abs_s16(nlAngleDiff(aAngPost1, aAngPost2));

        v3PositionOut.x = 1.005f * pNet->m_v3NetLocation.x;

        float fProbability;
        if (fAbsAimValue > 0.01f)
        {
            fProbability = 0.5f - 0.5f * fAimValue;
        }
        else if (uAbsP1G >= uAbsP1P2)
        {
            fProbability = 1.0f;
        }
        else if (uAbsP2G >= uAbsP1P2)
        {
            fProbability = 0.0f;
        }
        else
        {
            float fAngToNet = nlATan2f(pNet->m_v3NetLocation.y - pBall->m_v3Position.y,
                pNet->m_v3NetLocation.x - pBall->m_v3Position.x);
            u16 angle2Net = (u16)(s32)(10430.378f * fAngToNet);
            if (pNet->m_v3NetLocation.x < 0.0f)
            {
                angle2Net += 0x8000;
            }
            s16 sAng2Net = (s16)angle2Net;
            if ((u16)abs_s16(sAng2Net) > 0x2000)
            {
                float fGD1Sq = nlVec3DistanceSquared2D(pGoalie->mUnidentified024.m_v3Position, v3Post1);
                float fGD2Sq = nlVec3DistanceSquared2D(pGoalie->mUnidentified024.m_v3Position, v3Post2);
                fProbability = nlMinEquals(nlMaxEquals(fGD1Sq / (fGD1Sq + fGD2Sq), 0.03f), 0.97f);
            }
            else if (3 * uAbsP1G < uAbsP2G || 3 * uAbsP2G < uAbsP1G)
            {
                if (uAbsP1G < uAbsP2G)
                {
                    fProbability = nlMaxEquals(0.05f, 0.5f * (int)(3 * uAbsP1G - uAbsP2G) / (int)(uAbsP1G + uAbsP2G));
                }
                else
                {
                    fProbability = nlMinEquals(0.95f, 1.0f - 0.5f * (int)(3 * uAbsP2G - uAbsP1G) / (int)(uAbsP1G + uAbsP2G));
                }
            }
            else
            {
                float fAngleLimit = 8192.0f;
                fProbability = InterpolateRangeClamped(0.15f, 0.85f, -fAngleLimit, fAngleLimit, -(float)(s32)sAng2Net);
            }
        }

        if (nlRandomf(1.0f) < fProbability)
        {
            v3PositionOut.y = pBall->m_v3Position.y + v3Post1Delta.y;
            float fDistPost1Sq = nlVec3LengthSquared(v3Post1Delta);
            float fDistPost2Sq = nlVec3LengthSquared(v3Post2Delta);
            if (fDistPost1Sq < fDistPost2Sq)
            {
                v3PositionOut.x = 0.985f * pNet->m_v3NetLocation.x;
            }
        }
        else
        {
            v3PositionOut.y = pBall->m_v3Position.y + v3Post2Delta.y;
            float fDistPost2Sq = nlVec3LengthSquared(v3Post2Delta);
            float fDistPost1Sq = nlVec3LengthSquared(v3Post1Delta);
            if (fDistPost2Sq < fDistPost1Sq)
            {
                v3PositionOut.x = 0.985f * pNet->m_v3NetLocation.x;
            }
        }

        if (nParam == 8 && (mUnidentified024.m_eCharacterClass == 14 || mUnidentified024.m_eCharacterClass == 12))
        {
            v3PositionOut.z = lbl_806DB754 * cNet::m_fNetHeight;
            v3PositionOut.y = 0.0f;
        }
        else if (bIsModified)
        {
            v3PositionOut.z = cNet::m_fNetHeight - kBallAllowance;
            nlVector3 v3Direction = v3Zero;
            nlVector3 v3BallPosition = pBall->m_v3Position;
            v3BallPosition.z = v3PositionOut.z;
            nlVec3Sub(v3Direction, v3BallPosition, v3PositionOut);
            if (nlVec3LengthSquared(v3Direction) > 0.01f)
            {
                nlVec3Scale(v3Direction, v3Direction,
                    nlRecipSqrt(nlVec3LengthSquared(v3Direction), true));
                float fDistance = InterpolateRangeClamped(lbl_806DB80C, 0.36f,
                    lbl_806DB828, lbl_806DB824, fShotDist);
                float fOffset = fDistance * lbl_806DB810
                    + nlRandomf(fDistance * (1.0f - lbl_806DB810));
                nlVec3Scale(v3Direction, v3Direction, fOffset);
                nlVec3Add(v3PositionOut, v3PositionOut, v3Direction);
            }
        }
        else
        {
            float fHeightVariance = InterpolateRangeClamped(1.0f, 0.2f, 1.0f, 0.0f, m_pTweaks->fShooting);
            float fHeightAllowance = 0.18f + gGameTweaks.m_pGameTweaks->fShotHeightOffsetFromPost;
            float fAllowableHeight = cNet::m_fNetHeight - 2.0f * fHeightAllowance;
            float fMinimumHeight = (1.0f - fHeightVariance) * fAllowableHeight;
            v3PositionOut.z = fMinimumHeight + fHeightAllowance + nlRandomf(fHeightVariance * fAllowableHeight);
        }
    }
}

void cFielder::DoRegularShooting(bool bParam)
{
    nlVector3 v3BallVelocity;
    nlVector3 v3Target;
    int nBallState = 6;
    bool bHideBall = false;

    if (m_pShotMeter->m_eShotMeterState == SHOT_METER_STS_RELEASED)
    {
        nBallState = 8;
        if (fn_80319FEC(mUnidentified428->mUnidentified18, 0x1C))
        {
            fn_80319E58(mUnidentified428->mUnidentified18, 0x1C);
        }
        if (mUnidentified024.m_eCharacterClass == 14 || mUnidentified024.m_eCharacterClass == 12)
        {
            bHideBall = true;
            g_pBall->m_pPhysicsBall->fn_8013FE00();
        }
    }
    else if (bIsModified)
    {
        nBallState = 7;
    }

    float fCharge = Interpolate(lbl_806DB790, lbl_806DB794,
        InterpolateRangeClamped(0.0f, 1.0f, 0.5f, 1.0f, fn_8002BE84(m_pTweaks)));
    fn_800154FC(g_pBall, fCharge + fn_800155A0(g_pBall, 0));
    fn_80035194(this, v3BallVelocity, v3Target, nBallState);

    if (nBallState == 8)
    {
        g_pBall->m_uGoalType = 2;
    }
    else if (m_eActionState == ACTION_ONETIMER
        || m_eActionState == ACTION_LATE_ONETIMER_FROM_VOLLEY
        || (m_eActionState == ACTION_UNKNOWN_15
            && mUnidentified1E4.m_tBallPossessionTimer.GetSeconds() < 0.1f))
    {
        g_pBall->m_uGoalType = 1;
    }
    else
    {
        g_pBall->m_uGoalType = 0;
    }

    if (m_pBall != NULL)
    {
        ReleaseBall(nBallState);
    }
    if (bHideBall)
    {
        g_pBall->m_bVisible = false;
    }
    g_pBall->m_v3ShotTarget = v3Target;

    eSpinType spinType;
    nlVector3 v3AngVel;
    if (nBallState == 7)
    {
        spinType = SPINTYPE_BACK;
        v3AngVel = v3Zero;
    }
    else
    {
        float fSpinScale = InterpolateRangeClamped(lbl_806DB7C8, lbl_806DB7CC,
            lbl_806DB7C0, lbl_806DB7C4, nlVec3Length(v3BallVelocity));
        spinType = SPINTYPE_PARAMETER;
        float fXSpin = 8.0f * fSpinScale;
        float fZSpin = 15.0f * fSpinScale;
        float fHalfXSpin = 0.5f * fXSpin;
        v3AngVel.x = fHalfXSpin - nlRandomf(fXSpin);
        v3AngVel.y = 0.0f;

        if (!m_pTeam->GetOtherTeam()->GetGoalie()->mbShouldMiss
            && m_pTweaks->fShooting > 0.25f && m_pTweaks->fShooting < 0.75f)
        {
            float fMinDistanceSq = lbl_806DB7D8 * lbl_806DB7D8;
            float fDistanceSq = nlVec3DistanceSquared2D(
                mUnidentified024.m_v3Position, m_pTeam->GetOtherNet()->m_v3NetLocation);
            if (fDistanceSq > fMinDistanceSq)
            {
                float fDistanceValue = InterpolateRangeClamped(0.2f, 1.0f,
                    lbl_806DB7D8, lbl_806DB7DC, nlSqrt(fDistanceSq, true));
                v3AngVel.y = -fSpinScale * fDistanceValue
                    * (lbl_806DB7D0 + nlRandomf(lbl_806DB7D4));
            }
        }
        v3AngVel.z = fZSpin + nlRandomf(fZSpin);

        nlVector3 v3Delta;
        nlVec3Sub(v3Delta, v3Target, g_pBall->m_v3Position);
        bool bNegZSpin = false;
        if (fabsf(v3Delta.x) < fabsf(v3Delta.y))
        {
            if (v3Delta.x * v3Delta.y > 0.0f)
            {
                bNegZSpin = true;
            }
        }
        else
        {
            if (v3Target.x * v3Target.y > 0.0f)
            {
                bNegZSpin = true;
            }
        }
        if (bNegZSpin)
        {
            v3AngVel.z *= -1.0f;
        }
        RotateVectorZAxis(v3AngVel, v3AngVel, mUnidentified024.m_aActualFacingDirection);
        if (m_eActionState == ACTION_ONETIMER)
        {
            nlVec3Scale(v3AngVel, 0.4f);
        }

        if ((g_pBall->m_uGoalType == 0 || g_pBall->m_uGoalType == 2)
            && m_pTweaks->fShooting > 0.25f)
        {
            if (m_eActionState == ACTION_LOOSE_BALL_SHOT)
            {
                switch (m_eAnimID)
                {
                case 0x34:
                case 0x35:
                case 0x36:
                case 0x37:
                case 0x38:
                case 0x39:
                case 0x3A:
                case 0x3B:
                {
                    float fCharge = fn_800156A8(g_pBall);
                    PhysicsBall* pPhysicsBall = g_pBall->m_pPhysicsBall;
                    pPhysicsBall->mbUseMagnusEffect = true;
                    pPhysicsBall->mfChargeBonus = fCharge;
                    break;
                }
                }
            }
            else
            {
                float fCharge = fn_800156A8(g_pBall);
                PhysicsBall* pPhysicsBall = g_pBall->m_pPhysicsBall;
                pPhysicsBall->mbUseMagnusEffect = true;
                pPhysicsBall->mfChargeBonus = fCharge;
            }
        }
    }

    g_pBall->Shoot(this, v3BallVelocity, v3AngVel, spinType, nBallState, bParam);
    SetNoPickUpTime(0.2f);
    if (nBallState == 8 && mUnidentified024.m_eCharacterClass == 16)
    {
        fn_8004ED64();
    }
    if (g_pGame->IsGameplayOrOvertime())
    {
        ShotAtGoalData* pShotData = g_ShotAtGoalDataPool.Allocate();
        pShotData->pShooter = this;
        fn_8005EED0(g_pGame, pShotData);
        if (nBallState != 8)
        {
            ePlayerStats stat = STATS_00;
            float fCharge = fn_800156A8(g_pBall);
            if (fCharge >= 0.8f)
            {
                stat = STATS_02;
            }
            else if (fCharge >= 0.4f)
            {
                stat = STATS_01;
            }
            StatsTracker::Instance()->TrackStat(stat, m_pTeam->m_nSide, mUnidentified1E4.m_ID, 0, 0, 0, 0);
        }
    }
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
    CleanUpAction(actionState);
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
    if (mUnidentified1E4.m_tFireTimer.m_uPackedTime != 0)
    {
        return true;
    }

    if (fn_800344B0())
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
        if (mUnidentified024.m_eCharacterClass == DAISY)
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
    DesireFrozen* pAction = (DesireFrozen*)
        fn_80319FC0(mUnidentified428->mUnidentified18, 0x1D);
    bool bActionActive = false;
    if (pAction != 0 && pAction->mUnidentifiedActive
        && pAction->meFrozenState != 0)
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
    if (mUnidentified024.m_eCharacterClass != TOAD && !fn_80038918())
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

bool cFielder::fn_80038660() const
{
    if (!fn_80038918() && m_eActionState == ACTION_SLIDE_ATTACK)
        return true;
    return false;
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
    if (IsStuck())
    {
        return false;
    }

    if (IsFallenDown())
    {
        return false;
    }

    bool bUnidentified = false;
    if (mUnidentified024.m_eCharacterClass == TOAD
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

bool cFielder::InitDesire(eFielderDesireState eDesireType, float fConfidence,
    float fDuration, const FuzzyVariant& opt1, const FuzzyVariant& opt2)
{
    UnidentifiedVariantCollection params;
    params.Set(7, FuzzyVariant(fDuration));
    params.Set(4, FuzzyVariant(fConfidence));
    params.Set(0, FuzzyVariant(opt1));
    params.Set(1, FuzzyVariant(opt2));

    bool bDesireInitSuccess = mUnidentified428->mUnidentified18
        ->UnidentifiedVirtual5(eDesireType, &params, true) != 0;
    return bDesireInitSuccess;
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

    DesireFrozen* pAction = (DesireFrozen*)
        fn_80319FC0(mUnidentified428->mUnidentified18, 0x1D);
    bool bActionActive = false;
    if (pAction != 0 && pAction->mUnidentifiedActive
        && pAction->meFrozenState != 0)
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

void cFielder::Update(float fDeltaT)
{
    SetPlayerAudioController(this);
    fn_8003EAC0(this, fDeltaT);
    cPlayer::Update(fDeltaT);
    mUnidentified428->fn_8030F800(true, fDeltaT);

    if (!mUnidentified1E4.m_bSkipActionUpdate)
    {
        UpdateActionState(fDeltaT);
        UpdateHeadTracking(fDeltaT);
    }
    else
    {
        SetPosition(mUnidentified024.m_v3PrevPosition);
    }

    if (!mUnidentified1E4.m_bSkipAnimUpdate)
    {
        cCharacter::Update(fDeltaT);
    }
    else if (mUnidentified1E4.m_bForceFeatherUpdate && fn_800976C4())
    {
        cCharacter::Update(0.0f);
        m_pPowerupLayer->SetChild(
            1, m_pPowerupLayer->GetChild(1)->Update(fDeltaT));
    }

    if (mUnidentified024.m_eCharacterClass == (eCharacterClass)6
        && mUnidentified3F8.mUnidentified08 != 0)
    {
        mUnidentified3F8.mUnidentified08->Update(fDeltaT);
    }
    UpdateController(fDeltaT);
    m_bHasBeenUpdated = true;
}

ePowerUpType cFielder::GetPowerupType() const
{
    return ((DesireUsePowerup*)fn_80319FC0(
        mUnidentified428->mUnidentified18, 0x11))->GetPowerupType();
}

void cFielder::UseTeamPowerup(cFielder* pTarget)
{
    ((DesireUsePowerup*)fn_80319FC0(
        mUnidentified428->mUnidentified18, 0x11))->fn_800D3968(
        pTarget, POWER_UP_NONE, true);
}

void cFielder::StartRunning()
{
    if (!IsRunning() && !IsRunningWithBall())
    {
        if (m_pBall != 0)
        {
            InitActionRunningWB(false);
            return;
        }
        InitActionRunning();
    }
}

void cFielder::UpdateActionState(float dt)
{
    switch (m_eActionState)
    {
    case 1:
        fn_80044BEC(dt);
        break;
    case ACTION_ELECTROCUTION:
        ActionElectrocution(dt);
        break;
    case 3:
        fn_8004643C(dt);
        break;
    case 0x18:
        fn_80045C74(dt);
        break;
    case ACTION_HIT:
        ActionHit(dt);
        break;
    case 0:
        fn_80043C18(dt);
        break;
    case 0x23:
        fn_80044290(dt);
        break;
    case 5:
    case ACTION_HIT_REACT:
        fn_800474FC(dt);
        break;
    case ACTION_LATE_ONETIMER_FROM_VOLLEY:
        ActionLateOneTimerFromVolley(dt);
        break;
    case ACTION_IDLE_TURN:
        ActionIdleTurn(dt);
        break;
    case ACTION_LOOSE_BALL_PASS:
        fn_80048484(dt);
        break;
    case ACTION_LOOSE_BALL_SHOT:
        fn_800486DC(dt);
        break;
    case ACTION_ONETIMER:
        fn_80049EA0(dt);
        break;
    case ACTION_ONETOUCH_PASS_FROM_VOLLEY:
        ActionOneTouchPassFromVolley(dt);
        break;
    case ACTION_PASS:
        ActionPass(dt);
        break;
    case ACTION_POST_WHISTLE:
        ActionPostWhistle(dt);
        break;
    case 0x11:
        ActionSquishReact(dt);
        break;
    case ACTION_RUNNING:
        ActionRunning(dt);
        break;
    case ACTION_RUNNING_WB:
        ActionRunningWB(dt);
        break;
    case 0x13:
        fn_8004B2E4(dt);
        break;
    case ACTION_UNKNOWN_15:
        fn_8004C02C(dt);
        break;
    case ACTION_SLIDE_ATTACK:
        fn_8004C88C(dt);
        break;
    case ACTION_SLIDE_ATTACK_REACT:
        ActionSlideAttackReact(dt);
        break;
    case ACTION_BOMB_REACT:
        ActionBombReact(dt);
        break;
    case 0x1B:
        ActionShellReact(dt);
        break;
    case 0x1A:
        ActionSTSHitReact(dt);
        break;
    case 0x1C:
        ActionBananaReact(dt);
        break;
    case 0x1D:
        if (!g_pGame->IsGameplayOrOvertime() && ShouldStartCrossBlend(4))
        {
            StartRunning();
        }
        break;
    case ACTION_WAIT:
        ActionWait(dt);
        break;
    case ACTION_UNKNOWN_30:
        fn_8004BB80(dt);
        break;
    case ACTION_UNKNOWN_31:
        fn_8004E228();
        break;
    case ACTION_UNKNOWN_32:
        fn_8004EAB4(dt);
        break;
    case 0x21:
        fn_8004EE48(dt);
        break;
    case ACTION_UNKNOWN_34:
        fn_8004F2FC(dt);
        break;
    }
}

void cFielder::TestCollisionForInvicibility(cFielder* pOpponent)
{
    cFielder* pReactee = NULL;
    cFielder* pAttacker = NULL;
    if (!mbTangible)
        return;
    if (!pOpponent->mbTangible)
        return;
    if (fn_80038918())
        return;
    if (pOpponent->fn_80038918())
        return;

    bool bUnidentified = false;
    if (pOpponent->m_pBall != NULL)
    {
        if (fn_80038660() && !fn_8003E74C())
        {
            float fUnidentified = 0.18f
                + fn_8002BFA8(m_pTweaks, mUnidentified024.m_fPlayerScale) + lbl_806DB74C;
            if (nlVec3DistanceSquared2D(mUnidentified024.m_v3Position, g_pBall->m_v3Position)
                < fUnidentified * fUnidentified)
                bUnidentified = true;
        }
    }
    else if (m_pBall != NULL)
    {
        if (pOpponent->fn_80038660() && !pOpponent->fn_8003E74C())
        {
            float fUnidentified = 0.18f
                + fn_8002BFA8(pOpponent->m_pTweaks, pOpponent->mUnidentified024.m_fPlayerScale)
                + lbl_806DB74C;
            if (nlVec3DistanceSquared2D(pOpponent->mUnidentified024.m_v3Position, g_pBall->m_v3Position)
                < fUnidentified * fUnidentified)
                bUnidentified = true;
        }
    }

    if (fn_800345EC(pOpponent))
        return;
    if (pOpponent->fn_800345EC(this))
        return;

    if (IsInvincible() && !pOpponent->IsInvincible())
    {
        pReactee = pOpponent;
        pAttacker = this;
    }
    else if (pOpponent->IsInvincible() && !IsInvincible())
    {
        pReactee = this;
        pAttacker = pOpponent;
    }
    else if (fn_8003E74C() && !pOpponent->fn_8003E74C()
        && !fn_80038918() && !bUnidentified
        && !pOpponent->fn_8003E74C() && !pOpponent->fn_800344B0())
    {
        pReactee = pOpponent;
        pAttacker = this;
        pOpponent->fn_8004D480(v3Zero);

        PlayerAttackData* pAttackData = g_PlayerAttackDataPool.Allocate();
        pAttackData->pAttacker = this;
        u8 bHasGlobalPad = GetGlobalPad() != NULL;
        pAttackData->nAttackerPadID = bHasGlobalPad ? GetGlobalPad()->GetPadID() : -1;
        pAttackData->pTarget = pOpponent;
        pAttackData->mUnidentified0C = 2;
        pAttackData->mUnidentified10 = false;
        fn_8005ED64(g_pGame, pAttackData);
    }
    else if (pOpponent->fn_8003E74C() && !fn_8003E74C()
        && !pOpponent->fn_80038918() && !bUnidentified && !fn_800344B0())
    {
        pReactee = this;
        pAttacker = pOpponent;
        fn_8004D480(v3Zero);

        PlayerAttackData* pAttackData = g_PlayerAttackDataPool.Allocate();
        pAttackData->pAttacker = pOpponent;
        u8 bHasGlobalPad = pOpponent->GetGlobalPad() != NULL;
        pAttackData->nAttackerPadID = bHasGlobalPad ? pOpponent->GetGlobalPad()->GetPadID() : -1;
        pAttackData->pTarget = this;
        pAttackData->mUnidentified0C = 2;
        pAttackData->mUnidentified10 = false;
        fn_8005ED64(g_pGame, pAttackData);
    }
    else if (IsInvincibleChars() && !pOpponent->IsInvincibleChars())
    {
        pReactee = pOpponent;
        pAttacker = this;
    }
    else if (pOpponent->IsInvincibleChars() && !IsInvincibleChars())
    {
        pReactee = this;
        pAttacker = pOpponent;
    }

    if (pReactee == NULL)
        return;
    if (pReactee->IsFallenDown())
        return;

    fn_800470B4(pReactee, pAttacker);
    g_pBall->m_tNoPickupTimer.SetSeconds(0.0f);
    if (pAttacker->CanPickupBall(g_pBall, pAttacker->fn_80038660()))
        pAttacker->PickupBall(g_pBall);
}

void cFielder::UpdateHeadTracking(float fDeltaT)
{
    m_pHeadTrack->m_fSmoothTime = lbl_806DB6E8;

    if ((fn_8003E8A0(this) || fn_8003E9F0()) && mUnidentified3DC)
    {
        if (lbl_806E0C58)
        {
            cPlayer* pUnidentified = g_pBall->m_pOwner;
            if (pUnidentified != 0)
            {
                if (IsOnSameTeam(pUnidentified)
                    || nlVec3DistanceSquared2D(pUnidentified->mUnidentified024.m_v3Position,
                           mUnidentified024.m_v3Position) > 36.0f
                    || fn_800DDF54(this, pUnidentified) < 0.6f)
                {
                    pUnidentified = 0;
                }
            }
            if (pUnidentified == 0)
            {
                pUnidentified = DoFindBestHitTarget();
            }

            nlVector3 v3Unidentified;
            if (pUnidentified == 0)
            {
                nlVector3 v3Unidentified0 = GetJointPosition(m_nHeadJointIndex);
                const nlMatrix4& m4Unidentified
                    = m_pPoseAccumulator->GetNodeMatrix(m_nHeadJointIndex);
                nlVector3 v3Unidentified1;
                nlVec3Set(v3Unidentified1,
                    m4Unidentified.m11, m4Unidentified.m12, m4Unidentified.m13);
                nlVec3ScaleAdd(v3Unidentified, 5.0f,
                    v3Unidentified1, v3Unidentified0);
            }
            else
            {
                v3Unidentified = pUnidentified->mUnidentified024.m_v3Position;
            }
            v3Unidentified.z = lbl_806DB798;
            m_pHeadTrack->m_v3OOI = v3Unidentified;
            m_pHeadTrack->m_bTrackOOI = true;
        }
        else
        {
            m_pHeadTrack->m_bTrackOOI = false;
        }
        return;
    }

    if (fn_8003E74C() && m_pBall == 0)
    {
        cPlayer* pUnidentified = DoFindBestHitTarget();
        if (pUnidentified != 0)
        {
            m_pHeadTrack->m_v3OOI
                = pUnidentified->GetJointPosition(pUnidentified->m_nBip01JointIndex_0xA4);
            m_pHeadTrack->m_bTrackOOI = true;
            return;
        }
    }

    if (mUnidentified024.m_eCharacterClass == (eCharacterClass)0x10)
    {
        m_pHeadTrack->m_bTrackOOI = false;
        return;
    }

    if (fn_80319FEC(mUnidentified428->mUnidentified18, 0x1E)
        && !IsFallenDown()
        && (mUnidentified024.m_eCharacterClass != (eCharacterClass)0xC || m_pBall == 0))
    {
        float fUnidentified = (int)g_pGame->GetGameTime();
        nlVector3 v3Unidentified;
        if (g_pGame->GetGameTime() - fUnidentified < 0.25f
            || (g_pGame->GetGameTime() - fUnidentified > 0.5f
                && g_pGame->GetGameTime() - fUnidentified < 0.75f))
        {
            v3Unidentified = m_pTeam->m_pNet->m_v3NetLocation;
        }
        else
        {
            v3Unidentified = m_pTeam->GetOtherNet()->m_v3NetLocation;
        }
        v3Unidentified.z = 5.0f + nlRandomf(30.0f);
        m_pHeadTrack->m_v3OOI = v3Unidentified;
        m_pHeadTrack->m_bTrackOOI = true;
        return;
    }

    switch (m_eActionState)
    {
    case ACTION_HIT:
        if (mUnidentified024.m_eCharacterClass == (eCharacterClass)8)
        {
            m_pHeadTrack->m_fSmoothTime = 0.005f;
        }
    case 29:
        if (mUnidentified024.m_eCharacterClass == (eCharacterClass)9)
        {
            m_pHeadTrack->m_fSmoothTime = 0.005f;
        }
    case ACTION_NEED_ACTION:
    case 0:
    case 1:
    case ACTION_ELECTROCUTION:
    case 5:
    case ACTION_HIT_REACT:
    case ACTION_LATE_ONETIMER_FROM_VOLLEY:
    case ACTION_SHOT:
    case ACTION_SHOOT_TO_SCORE:
    case ACTION_ONETOUCH_PASS_FROM_VOLLEY:
    case ACTION_UNKNOWN_15:
    case ACTION_SLIDE_ATTACK_REACT:
    case ACTION_BOMB_REACT:
    case ACTION_SHELL_REACT:
    case ACTION_BANANA_REACT:
    case 28:
    case ACTION_UNKNOWN_31:
    case ACTION_UNKNOWN_32:
    case 33:
    case ACTION_UNKNOWN_34:
    case 35:
        m_pHeadTrack->m_bTrackOOI = false;
        break;

    case 3:
    case 24:
        m_pHeadTrack->m_bTrackOOI = true;
        if (mUnidentified34C > 0.0f)
        {
            nlVector3 v3Unidentified = mUnidentified024.m_v3Position;
            v3Unidentified.z -= 20.0f;
            m_pHeadTrack->m_v3OOI = v3Unidentified;
        }
        else
        {
            m_pHeadTrack->m_v3OOI = g_pBall->m_v3Position;
        }
        break;

    case ACTION_UNKNOWN_30:
        m_pHeadTrack->m_v3OOI = m_pTeam->GetOtherNet()->m_v3NetLocation;
        m_pHeadTrack->m_bTrackOOI = true;
        break;

    case ACTION_ONETIMER:
        switch (m_eAnimID)
        {
        case 0x3C:
        case 0x3D:
        case 0x3E:
        case 0x3F:
        case 0x40:
        case 0x41:
        case 0x42:
        case 0x43:
            m_pHeadTrack->m_bTrackOOI = false;
            return;
        default:
            break;
        }

        if (m_pCurrentAnimController->m_fTime > mUnidentified368)
        {
            m_pHeadTrack->m_bTrackOOI = false;
        }
        else
        {
            m_pHeadTrack->m_v3OOI = g_pBall->m_v3Position;
            m_pHeadTrack->m_bTrackOOI = true;
        }
        break;

    case ACTION_RECEIVE_PASS:
        if (m_pCurrentAnimController->m_fTime > 0.5f * mUnidentified368)
        {
            m_pHeadTrack->m_bTrackOOI = false;
        }
        else
        {
            m_pHeadTrack->m_v3OOI = g_pBall->m_v3Position;
            m_pHeadTrack->m_bTrackOOI = true;
        }
        break;

    case ACTION_LOOSE_BALL_PASS:
    case ACTION_LOOSE_BALL_SHOT:
        if (m_pCurrentAnimController->m_fTime > mUnidentified368)
        {
            m_pHeadTrack->m_bTrackOOI = false;
        }
        else
        {
            m_pHeadTrack->m_v3OOI = g_pBall->m_v3Position;
            m_pHeadTrack->m_bTrackOOI = true;
        }
        break;

    case ACTION_PASS:
    case 19:
    case ACTION_SLIDE_ATTACK:
        if (m_pBall == 0)
        {
            if (m_eAnimID != 0x27)
            {
                m_pHeadTrack->m_v3OOI = g_pBall->m_v3Position;
                m_pHeadTrack->m_bTrackOOI = true;
                break;
            }
        }
        m_pHeadTrack->m_bTrackOOI = false;
        break;

    case ACTION_RUNNING_WB:
        if (!gNPCManager->mpChainChomp->IsHidden())
        {
            m_pHeadTrack->m_v3OOI = gNPCManager->mpChainChomp->mv3Position;
            m_pHeadTrack->m_bTrackOOI = true;
        }
        else
        {
            m_pHeadTrack->m_bTrackOOI = false;
        }
        break;

    case ACTION_IDLE_TURN:
    case ACTION_RUNNING:
        if (!gNPCManager->mpChainChomp->IsHidden())
        {
            m_pHeadTrack->m_v3OOI = gNPCManager->mpChainChomp->mv3Position;
        }
        else
        {
            m_pHeadTrack->m_v3OOI = g_pBall->m_v3Position;
        }
        m_pHeadTrack->m_bTrackOOI = true;
        break;

    case ACTION_POST_WHISTLE:
    {
        cPlayer* pScorer = g_pGame->m_pScorer;
        if (pScorer != 0)
        {
            m_pHeadTrack->m_v3OOI = pScorer->mUnidentified024.m_v3Position;
        }
        else
        {
            m_pHeadTrack->m_v3OOI = g_pBall->m_v3Position;
        }
        m_pHeadTrack->m_bTrackOOI = true;
        break;
    }

    case ACTION_WAIT:
        m_pHeadTrack->m_v3OOI = g_pBall->m_v3Position;
        m_pHeadTrack->m_bTrackOOI = true;
        break;
    }
}

void cFielder::UpdateController(float fDeltaT)
{
    bool bUnidentified = false;
    if (GetGlobalPad() != NULL
        && GetGlobalPad()->IsPressed(PAD_SWITCH, true))
    {
        int nUnidentified = GetGlobalPad()->GetButtonStateTicks(PAD_SWITCH, true);
        if (nUnidentified * FixedUpdateTask::GetPhysicsUpdateTick() > 0.33f
            && m_pTeam->GetCaptain() != this)
        {
            GetGlobalPad()->ResetButtonStateTicks(PAD_SWITCH, true);
            m_pTeam->GetCaptain()->mbIgnorePadSwitchRelease = true;
            bUnidentified = true;
        }
    }

    if (g_pGame->IsGameplayOrOvertime())
    {
        if (GetGlobalPad() != NULL
            && ((GetGlobalPad()->JustPressed(PAD_SWITCH, true)
                    && !(GetGlobalPad() != NULL
                            ? GetGlobalPad()->IsPressed(0x17, true)
                            : false))
                || bUnidentified))
        {
            switch (m_eActionState)
            {
            case ACTION_NEED_ACTION:
            case ACTION_PASS:
            case ACTION_POST_WHISTLE:
            case (eFielderActionState)0x13:
            {
                if (IsStuck())
                {
                    if (m_pBall == NULL)
                    {
                        SwapController(bUnidentified);
                    }
                }
                break;
            }

            case ACTION_SHOT:
            case ACTION_SHOOT_TO_SCORE:
            case ACTION_RUNNING_WB:
            case ACTION_UNKNOWN_30:
            {
                if (IsStuck())
                {
                    if (m_pBall == NULL)
                    {
                        SwapController(bUnidentified);
                        break;
                    }
                }

                if (m_pBall == NULL)
                {
                    SwapController(bUnidentified);
                }
                break;
            }

            case (eFielderActionState)0x00:
            case (eFielderActionState)0x01:
            case ACTION_ELECTROCUTION:
            case (eFielderActionState)0x03:
            case ACTION_HIT:
            case (eFielderActionState)0x05:
            case ACTION_HIT_REACT:
            case ACTION_IDLE_TURN:
            case ACTION_LATE_ONETIMER_FROM_VOLLEY:
            case ACTION_LOOSE_BALL_PASS:
            case ACTION_LOOSE_BALL_SHOT:
            case ACTION_ONETIMER:
            case ACTION_ONETOUCH_PASS_FROM_VOLLEY:
            case ACTION_RECEIVE_PASS:
            case ACTION_RUNNING:
            case ACTION_UNKNOWN_15:
            case ACTION_SLIDE_ATTACK:
            case ACTION_SLIDE_ATTACK_REACT:
            case (eFielderActionState)0x18:
            case ACTION_BOMB_REACT:
            case ACTION_SHELL_REACT:
            case ACTION_BANANA_REACT:
            case (eFielderActionState)0x1C:
            case (eFielderActionState)0x1D:
            case ACTION_UNKNOWN_31:
            case ACTION_UNKNOWN_32:
            case (eFielderActionState)0x21:
            case ACTION_UNKNOWN_34:
            case (eFielderActionState)0x23:
            case ACTION_WAIT:
                if (m_pBall == NULL)
                {
                    SwapController(bUnidentified);
                }
                break;
            }
        }

        if (GetGlobalPad() != NULL
            && GetGlobalPad()->JustReleased(PAD_SWITCH, true))
        {
            mbIgnorePadSwitchRelease = false;
        }
    }
}

void cFielder::Unknown10(
    const nlVector3& v3Position, unsigned short aDirection)
{
    cPlayer::Unknown10(v3Position, aDirection);
    mUnidentified428->mUnidentified18->UnidentifiedVirtual4(false);
    CleanUpAction(ACTION_NEED_ACTION);
    mtPowerupThrowTime.UnidentifiedClear();
    m_tMoveToTurboTimer.UnidentifiedClear();
    ClearPowerupAnimState(true);

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
    mUnidentified178 = 1.0f;
    mUnidentified330.mUnidentified00 = false;
    mUnidentified330.mUnidentified04 = -1.0f;
    mUnidentified338 = 0;
    mUnidentified33A = false;
    mUnidentified33C = 2;
    mUnidentified340 = 0.0f;
    mUnidentified344 = 0.0f;
    mUnidentified348 = false;
    mUnidentified34C = 0.0f;
    mUnidentified350 = v3Zero;
    mUnidentified35C = 0.0f;
    mUnidentified360 = false;
    bYoshiInWindup = false;
    mUnidentified390 = 0.0f;
    mUnidentified394 = 0.0f;
    mUnidentified398 = -1.0f;
    bIsModified = false;
    mActionLooseBallPassVars.passTarget = 0;
    mUnidentified368 = 0.0f;
    mUnidentified36C = 0;
    mUnidentified370 = false;
    mUnidentified371 = false;
    mActionRunningVars.eLastStrafeDirection = STRAFE_IDLE;
    mActionRunningVars.bFirstCycleOfTurbo = false;
    mActionRunningWBVars.bWaitForAnimToFinish = false;
    mActionRunningWBVars.bCuePitch = false;
    mUnidentified388 = 0;
    bAttackSucceeded = false;
    mUnidentified38D = true;
    mUnidentified3D8 = 0;
    mUnidentified3DA = 0;
    mUnidentified3DC = false;
    mUnidentified3DD = false;
    mUnidentified3E0 = 0.0f;
    mUnidentified3E4 = 0.0f;
    mUnidentified3E8.nextFireballTime = 0.0f;
    mUnidentified3E8.fireballStageTime = 0.0f;
    mUnidentified3E8.fireballStageNum = 0;
    mUnidentified3E8.fn_800504A4();
    mUnidentified3F8.mUnidentified00 = 0.0f;
    mUnidentified3F8.mUnidentified04 = 0.0f;
    mUnidentified3F8.fn_800504A8();
    nlVec3Set(mUnidentified410.mUnidentified00, 0.0f, 0.0f, 0.0f);
    mUnidentified410.mUnidentified0C = false;
    mUnidentified424 = false;
    mUnidentified39C = -1.0f;
    mUnidentified3A0 = -1.0f;
    mUnidentified3A4 = -1.0f;
    mUnidentified3A8 = -1.0f;
    mUnidentified3AC = 0.0f;
    mUnidentified3B0 = 0.0f;
    mUnidentified3B4 = 0.0f;
    mUnidentified3B8 = false;
    mUnidentified3BC = 0.0f;
    mUnidentified3C0 = 0.0f;
    mUnidentified3C4 = 0.0f;
    mUnidentified3C8 = 0.0f;
    mUnidentified3CC = 0.0f;
    mUnidentified3D0 = 0.0f;
    mUnidentified3D4 = 0.0f;
    InitDesire(
        (eFielderDesireState)0x1F, 0.5f, -1.0f, fvNotSet, fvNotSet);
    InitActionWait();
}

void cFielder::ResetEffects()
{
    cCharacter::ResetEffects();
    if (mUnidentified3F8.mUnidentified08 != 0)
    {
        mUnidentified3F8.mUnidentified08->ClearWalls();
    }
    if (mUnidentified420 != 0)
    {
        mUnidentified420->Hide(true);
    }
}

u16 lbl_806DB842 = 0xFFFF;
u16 lbl_806DB844 = 0xFFFF;
u16 lbl_806DB846 = 0xFFFF;
u16 lbl_806DB848 = 0xFFFF;
u16 lbl_806DB84A = 0xFFFF;
u16 lbl_806DB84C = 0xFFFF;
u16 lbl_806DB84E = 0xFFFF;
u16 lbl_806DB850 = 0xFFFF;
u16 lbl_806DB852 = 0xFFFF;
u16 lbl_806DB854 = 0xFFFF;
u16 lbl_806DB856 = 0xFFFF;
u16 lbl_806DB858 = 0xFFFF;
u16 lbl_806DB85A = 0xFFFF;
u16 lbl_806DB85C = 0xFFFF;
u16 lbl_806DB85E = 0xFFFF;
u16 lbl_806DB860 = 0xFFFF;
u16 lbl_806DB862 = 0xFFFF;
u16 lbl_806DB864 = 0xFFFF;
u16 lbl_806DB866 = 0xFFFF;
u16 lbl_806DB868 = 0xFFFF;

extern u16 lbl_806DC048;

struct UnidentifiedFielderDesireState
{
    u32 m_nTransitionFuncHash;
    u32 m_nLastActiveTime;
    float m_fMaxDuration;
    float m_fMinDuration;
    float m_fAge;
};

#define REGISTER_FIELDER_FIELD(type, base, field, name) \
    cache->AddField(type, gDebugFieldTypes[type].size, \
        (u8*)&(field) - (u8*)&(base), name)

void cFielder::Unknown11(void* context, DebugWriteCache* cache)
{
    cPlayer::Unknown11(context, cache);

    if (lbl_806DB842 == 0xFFFF)
    {
        lbl_806DB842 = cache->BeginType("DetFielder");
        REGISTER_FIELDER_FIELD(16, m_bHasBeenUpdated,
            m_bHasBeenUpdated, "m_bHasBeenUpdated");
        REGISTER_FIELDER_FIELD(14, m_bHasBeenUpdated,
            m_eActionState, "m_eActionState");
        REGISTER_FIELDER_FIELD(20, m_bHasBeenUpdated,
            m_tMoveToTurboTimer, "m_tMoveToTurboTimer");
        REGISTER_FIELDER_FIELD(20, m_bHasBeenUpdated,
            mtPostDekeTimer, "mtPostDekeTimer");
        REGISTER_FIELDER_FIELD(16, m_bHasBeenUpdated,
            m_bInPosition, "m_bInPosition");
        REGISTER_FIELDER_FIELD(17, m_bHasBeenUpdated,
            mfAirInterceptHeight[0], "mfAirInterceptHeight[0]");
        REGISTER_FIELDER_FIELD(17, m_bHasBeenUpdated,
            mfAirInterceptHeight[1], "mfAirInterceptHeight[1]");
        REGISTER_FIELDER_FIELD(8, m_bHasBeenUpdated,
            m_nPowerupAnimID, "m_nPowerupAnimID");
        REGISTER_FIELDER_FIELD(20, m_bHasBeenUpdated,
            mtPowerupThrowTime, "mtPowerupThrowTime");
        REGISTER_FIELDER_FIELD(8, m_bHasBeenUpdated,
            muInvincibleStatus, "muInvincibleStatus");
        REGISTER_FIELDER_FIELD(14, m_bHasBeenUpdated,
            m_eRole, "m_eRole");
        for (int i = 0; i < 4; i++)
        {
            REGISTER_FIELDER_FIELD(15, m_bHasBeenUpdated,
                m_pMark[i], "m_pMark[i]");
        }
        REGISTER_FIELDER_FIELD(16, m_bHasBeenUpdated,
            mbWasHitByPowerupThisFrame, "mbWasHitByPowerupThisFrame");
        REGISTER_FIELDER_FIELD(16, m_bHasBeenUpdated,
            mbTangible, "mbTangible");
        REGISTER_FIELDER_FIELD(16, m_bHasBeenUpdated,
            mbIgnorePadSwitchRelease, "mbIgnorePadSwitchRelease");
        cache->EndType();
    }

    void* data = cache->WriteData(lbl_806DB842, &m_bHasBeenUpdated,
        offsetof(cFielder, mUnidentified478) - offsetof(cFielder, m_bHasBeenUpdated));
    if (data != 0)
    {
        cFielder** marks = (cFielder**)((u8*)data
            + offsetof(cFielder, m_pMark) - offsetof(cFielder, m_bHasBeenUpdated));
        for (int i = 0; i < 4; i++)
        {
            marks[i] = (cFielder*)(m_pMark[i] == 0
                ? -1
                : m_pMark[i]->mUnidentified120);
        }
        cache->ChecksumData(lbl_806DB842, data, context);
    }

    if (lbl_806DB844 == 0xFFFF)
    {
        lbl_806DB844 = cache->BeginType("ActCrowdVars");
        REGISTER_FIELDER_FIELD(16, mUnidentified330,
            mUnidentified330.mUnidentified00, "bHasBeenSuckedToMiddle");
        REGISTER_FIELDER_FIELD(17, mUnidentified330,
            mUnidentified330.mUnidentified04, "fStuckInRiotTime");
        cache->EndType();
    }
    cache->ChecksumData(lbl_806DB844, &mUnidentified330, context);
    cache->WriteData(lbl_806DB844, &mUnidentified330, sizeof(mUnidentified330));

    if (lbl_806DB846 == 0xFFFF)
    {
        lbl_806DB846 = cache->BeginType("ActDekeVars");
        REGISTER_FIELDER_FIELD(19, mUnidentified338,
            mUnidentified338, "aDekeDir");
        REGISTER_FIELDER_FIELD(16, mUnidentified338,
            mUnidentified33A, "bIsReset");
        REGISTER_FIELDER_FIELD(8, mUnidentified338,
            mUnidentified33C, "nDPadDownCounter");
        cache->EndType();
    }
    cache->ChecksumData(lbl_806DB846, &mUnidentified338, context);
    cache->WriteData(lbl_806DB846, &mUnidentified338,
        offsetof(cFielder, mUnidentified340) - offsetof(cFielder, mUnidentified338));

    if (lbl_806DB848 == 0xFFFF)
    {
        lbl_806DB848 = cache->BeginType("ActElectVars");
        REGISTER_FIELDER_FIELD(17, mUnidentified340,
            mUnidentified340, "electrocutionTime");
        REGISTER_FIELDER_FIELD(17, mUnidentified340,
            mUnidentified344, "electrocutionLiftTime");
        REGISTER_FIELDER_FIELD(16, mUnidentified340,
            mUnidentified348, "bIsGroundElectrocution");
        cache->EndType();
    }
    cache->ChecksumData(lbl_806DB848, &mUnidentified340, context);
    cache->WriteData(lbl_806DB848, &mUnidentified340,
        offsetof(cFielder, mUnidentified34C) - offsetof(cFielder, mUnidentified340));

    if (lbl_806DB84A == 0xFFFF)
    {
        lbl_806DB84A = cache->BeginType("ActFallVars");
        REGISTER_FIELDER_FIELD(17, mUnidentified34C,
            mUnidentified34C, "fallingTime");
        REGISTER_FIELDER_FIELD(22, mUnidentified34C,
            mUnidentified350, "v3SuckToSpot");
        cache->EndType();
    }
    cache->ChecksumData(lbl_806DB84A, &mUnidentified34C, context);
    cache->WriteData(lbl_806DB84A, &mUnidentified34C,
        offsetof(cFielder, mUnidentified35C) - offsetof(cFielder, mUnidentified34C));

    if (lbl_806DB84C == 0xFFFF)
    {
        lbl_806DB84C = cache->BeginType("ActHitVars");
        REGISTER_FIELDER_FIELD(17, mUnidentified35C,
            mUnidentified35C, "fHitDistance");
        cache->EndType();
    }
    cache->ChecksumData(lbl_806DB84C, &mUnidentified35C, context);
    cache->WriteData(lbl_806DB84C, &mUnidentified35C, sizeof(mUnidentified35C));

    if (lbl_806DB84E == 0xFFFF)
    {
        lbl_806DB84E = cache->BeginType("ActHitReactVars");
        REGISTER_FIELDER_FIELD(16, mUnidentified360,
            mUnidentified360, "bDoFrameLock");
        cache->EndType();
    }
    cache->ChecksumData(lbl_806DB84E, &mUnidentified360, context);
    cache->WriteData(lbl_806DB84E, &mUnidentified360, sizeof(mUnidentified360));

    if (lbl_806DB850 == 0xFFFF)
    {
        lbl_806DB850 = cache->BeginType("ActSuperVars");
        REGISTER_FIELDER_FIELD(16, bYoshiInWindup,
            bYoshiInWindup, "bYoshiInWindup");
        cache->EndType();
    }
    cache->ChecksumData(lbl_806DB850, &bYoshiInWindup, context);
    cache->WriteData(lbl_806DB850, &bYoshiInWindup, sizeof(bYoshiInWindup));

    if (lbl_806DB852 == 0xFFFF)
    {
        lbl_806DB852 = cache->BeginType("ActShootPassCommon");
        REGISTER_FIELDER_FIELD(16, bIsModified,
            bIsModified, "bIsModified");
        cache->EndType();
    }
    cache->ChecksumData(lbl_806DB852, &bIsModified, context);
    cache->WriteData(lbl_806DB852, &bIsModified, sizeof(bIsModified));

    if (lbl_806DB854 == 0xFFFF)
    {
        lbl_806DB854 = cache->BeginType("ActLooseBallPass");
        REGISTER_FIELDER_FIELD(15, mActionLooseBallPassVars,
            mActionLooseBallPassVars.passTarget, "passTarget");
        cache->EndType();
    }
    data = cache->WriteData(lbl_806DB854,
        &mActionLooseBallPassVars, sizeof(mActionLooseBallPassVars));
    if (data != 0)
    {
        UnidentifiedFielderAction364* copy = (UnidentifiedFielderAction364*)data;
        copy->passTarget = (cFielder*)(mActionLooseBallPassVars.passTarget == 0
            ? -1
            : mActionLooseBallPassVars.passTarget->mUnidentified120);
        cache->ChecksumData(lbl_806DB854, data, context);
    }

    if (lbl_806DB856 == 0xFFFF)
    {
        lbl_806DB856 = cache->BeginType("ActOneTimerVars");
        REGISTER_FIELDER_FIELD(17, mUnidentified368,
            mUnidentified368, "fOneTimerAnimTime");
        cache->EndType();
    }
    cache->ChecksumData(lbl_806DB856, &mUnidentified368, context);
    cache->WriteData(lbl_806DB856, &mUnidentified368, sizeof(mUnidentified368));

    if (lbl_806DB858 == 0xFFFF)
    {
        lbl_806DB858 = cache->BeginType("ActPassingVars");
        REGISTER_FIELDER_FIELD(15, mUnidentified36C,
            mUnidentified36C, "pPassTarget");
        REGISTER_FIELDER_FIELD(16, mUnidentified36C,
            mUnidentified370, "bAllowLeadPass");
        REGISTER_FIELDER_FIELD(16, mUnidentified36C,
            mUnidentified371, "bIsOneTouchPass");
        cache->EndType();
    }
    data = cache->WriteData(lbl_806DB858, &mUnidentified36C,
        offsetof(cFielder, mUnidentified374) - offsetof(cFielder, mUnidentified36C));
    if (data != 0)
    {
        cFielder* copy = (cFielder*)((u8*)data - offsetof(cFielder, mUnidentified36C));
        copy->mUnidentified36C = (cPlayer*)(mUnidentified36C == 0
            ? -1
            : mUnidentified36C->mUnidentified120);
        cache->ChecksumData(lbl_806DB858, data, context);
    }

    if (lbl_806DB85A == 0xFFFF)
    {
        lbl_806DB85A = cache->BeginType("ActRunPassVars");
        REGISTER_FIELDER_FIELD(8, mUnidentified374,
            mUnidentified374.mUnidentified00, "nHeldTicks");
        REGISTER_FIELDER_FIELD(17, mUnidentified374,
            mUnidentified374.mUnidentified04, "fSpeed");
        cache->EndType();
    }
    cache->ChecksumData(lbl_806DB85A, &mUnidentified374, context);
    cache->WriteData(lbl_806DB85A, &mUnidentified374, sizeof(mUnidentified374));

    if (lbl_806DB85C == 0xFFFF)
    {
        lbl_806DB85C = cache->BeginType("ActRunningVars");
        REGISTER_FIELDER_FIELD(14, mActionRunningVars,
            mActionRunningVars.eLastStrafeDirection, "eLastStrafeDirection");
        REGISTER_FIELDER_FIELD(16, mActionRunningVars,
            mActionRunningVars.bFirstCycleOfTurbo, "bFirstCycleOfTurbo");
        cache->EndType();
    }
    cache->ChecksumData(lbl_806DB85C, &mActionRunningVars, context);
    cache->WriteData(lbl_806DB85C, &mActionRunningVars, sizeof(mActionRunningVars));

    if (lbl_806DB85E == 0xFFFF)
    {
        lbl_806DB85E = cache->BeginType("ActRunningWBVars");
        REGISTER_FIELDER_FIELD(16, mActionRunningWBVars,
            mActionRunningWBVars.bWaitForAnimToFinish, "bWaitForAnimToFinish");
        REGISTER_FIELDER_FIELD(16, mActionRunningWBVars,
            mActionRunningWBVars.bCuePitch, "bCuePitch");
        cache->EndType();
    }
    cache->ChecksumData(lbl_806DB85E, &mActionRunningWBVars, context);
    cache->WriteData(lbl_806DB85E, &mActionRunningWBVars, sizeof(mActionRunningWBVars));

    if (lbl_806DB860 == 0xFFFF)
    {
        lbl_806DB860 = cache->BeginType("ActSlideAttack");
        REGISTER_FIELDER_FIELD(14, mUnidentified388,
            mUnidentified388, "eSlideAttackState");
        REGISTER_FIELDER_FIELD(16, mUnidentified388,
            bAttackSucceeded, "bAttackSucceeded");
        REGISTER_FIELDER_FIELD(16, mUnidentified388,
            mUnidentified38D, "bIsReset");
        cache->EndType();
    }
    cache->ChecksumData(lbl_806DB860, &mUnidentified388, context);
    cache->WriteData(lbl_806DB860, &mUnidentified388,
        offsetof(cFielder, mUnidentified390) - offsetof(cFielder, mUnidentified388));

    if (lbl_806DB862 == 0xFFFF)
    {
        lbl_806DB862 = cache->BeginType("ActMegaStrikeMeter");
        REGISTER_FIELDER_FIELD(17, mUnidentified390,
            mUnidentified390, "fNumBalls");
        REGISTER_FIELDER_FIELD(17, mUnidentified390,
            mUnidentified394, "fAccuracy");
        REGISTER_FIELDER_FIELD(17, mUnidentified390,
            mUnidentified398, "fReceivedTimestamp");
        cache->EndType();
    }
    cache->ChecksumData(lbl_806DB862, &mUnidentified390, context);
    cache->WriteData(lbl_806DB862, &mUnidentified390,
        offsetof(cFielder, mUnidentified39C) - offsetof(cFielder, mUnidentified390));

    if (lbl_806DB864 == 0xFFFF)
    {
        lbl_806DB864 = cache->BeginType("ActStunned");
        REGISTER_FIELDER_FIELD(10, mUnidentified3D8,
            mUnidentified3D8, "angAccel");
        REGISTER_FIELDER_FIELD(10, mUnidentified3D8,
            mUnidentified3DA, "angVel");
        cache->EndType();
    }
    cache->ChecksumData(lbl_806DB864, &mUnidentified3D8, context);
    cache->WriteData(lbl_806DB864, &mUnidentified3D8,
        offsetof(cFielder, mUnidentified3DC) - offsetof(cFielder, mUnidentified3D8));

    if (lbl_806DB866 == 0xFFFF)
    {
        lbl_806DB866 = cache->BeginType("ActBowserSuper");
        REGISTER_FIELDER_FIELD(17, mUnidentified3E8,
            mUnidentified3E8.nextFireballTime, "nextFireballTime");
        REGISTER_FIELDER_FIELD(17, mUnidentified3E8,
            mUnidentified3E8.fireballStageTime, "fireballStageTime");
        REGISTER_FIELDER_FIELD(9, mUnidentified3E8,
            mUnidentified3E8.fireballStageNum, "fireballStageNum");
        cache->EndType();
    }
    cache->ChecksumData(lbl_806DB866, &mUnidentified3E8, context);
    cache->WriteData(lbl_806DB866, &mUnidentified3E8, sizeof(mUnidentified3E8));

    if (lbl_806DB868 == 0xFFFF)
    {
        lbl_806DB868 = cache->BeginType("ActWarioSuper");
        REGISTER_FIELDER_FIELD(17, mUnidentified3F4,
            mUnidentified3F4, "nextGasTime");
        cache->EndType();
    }
    cache->ChecksumData(lbl_806DB868, &mUnidentified3F4, context);
    cache->WriteData(lbl_806DB868, &mUnidentified3F4, sizeof(mUnidentified3F4));

    DesireSteering* steering = (DesireSteering*)fn_8002E08C(this, 34);
    fn_8000F324(steering->m_pAvoidance, context, cache);

    for (int i = 0; i < 36; i++)
    {
        Desire* desire = fn_8002E08C(this, i);
        if (desire != 0 && desire->UnidentifiedIsActive())
        {
            UnidentifiedFielderDesireState state;
            const UnidentifiedStateTransition& transition
                = !desire->mUnidentified070.UnidentifiedIsUnset()
                ? desire->mUnidentified070
                : desire->mUnidentified068;
            state.m_nTransitionFuncHash = transition.mUnidentifiedHash;
            state.m_nLastActiveTime = (u32)desire->mUnidentified014;
            state.m_fMaxDuration = desire->mUnidentified078;
            state.m_fMinDuration = desire->mUnidentified07C;
            state.m_fAge = desire->mUnidentifiedTimer.GetSeconds();
            if (lbl_806DC048 == 0xFFFF)
            {
                lbl_806DC048 = cache->BeginType("FielderDesireShdState");
                REGISTER_FIELDER_FIELD(2, state,
                    state.m_nTransitionFuncHash, "m_nTransitionFuncHash");
                REGISTER_FIELDER_FIELD(2, state,
                    state.m_nLastActiveTime, "m_nLastActiveTime");
                REGISTER_FIELDER_FIELD(17, state,
                    state.m_fMaxDuration, "m_fMaxDuration");
                REGISTER_FIELDER_FIELD(17, state,
                    state.m_fMinDuration, "m_fMinDuration");
                REGISTER_FIELDER_FIELD(17, state, state.m_fAge, "m_fAge");
                cache->EndType();
            }
            cache->ChecksumData(lbl_806DC048, &state, context);
            cache->WriteData(lbl_806DC048, &state, sizeof(state));
            desire->UnidentifiedVirtual7(context, cache);
        }
    }
}

#undef REGISTER_FIELDER_FIELD

void cFielder::Unknown12(RunningChecksum* pChecksum)
{
    cPlayer::Unknown12(pChecksum);
    pChecksum->ChecksumData(&m_eActionState, sizeof(m_eActionState));
    pChecksum->ChecksumData(&m_eRole, sizeof(m_eRole));
}

UnidentifiedScriptMachine* fn_8002E1A4(cFielder* pFielder)
{
    return pFielder->mUnidentified428->mUnidentified18;
}

eFielderDesireState cFielder::fn_8002E060()
{
    UnidentifiedScriptMachine* machine = mUnidentified428->mUnidentified18;
    if (machine != 0 && machine->mUnidentified004 != 0)
    {
        return (eFielderDesireState)machine->mUnidentified004->mUnidentifiedState;
    }
    return (eFielderDesireState)-1;
}

void cFielder::SetPosition(const nlVector3& v3Position)
{
    cCharacter::SetPosition(v3Position);
}

void Desire::UnidentifiedVirtual7(void*, DebugWriteCache*)
{
}

PlayerTweaks* cFielder::GetTweaks() const
{
    return m_pTweaks;
}

extern "C" void fn_803198F4(UnidentifiedScriptMachine* machine);

void cFielder::EndDesire()
{
    UnidentifiedScriptMachine* machine = mUnidentified428->mUnidentified18;
    if (machine != 0)
    {
        fn_803198F4(machine);
    }
}

extern "C" UnidentifiedVariant_80054AB8 fn_80041B6C(
    void*, const unsigned int&, cFielder*);

extern "C" UnidentifiedVariant_80054AB8 fn_80041B0C(
    void* runtime, cFielder* fielder, const char* name)
{
    unsigned int functionHash = nlStringHash(name);
    return fn_80041B6C(runtime, functionHash, fielder);
}

extern "C" void fn_8004257C(PenaltyData* data)
{
    g_PenaltyDataPool.Free(data);
}

bool FuzzyVariant::IsPointerType() const
{
    return ((mType == FT_POINTER || mType == FT_STRING)
        || ((unsigned int)(mType - FT_PLAYER)
            <= (unsigned int)(FT_BALL - FT_PLAYER)));
}

void cFielder::fn_80036A38(int nParam, float fAmount)
{
    if (fAmount > 0.0f)
    {
        m_pTeam->IncrementPowerupMeter(fAmount, this, false);
    }
}

extern "C" void fn_80319DA0(UnidentifiedScriptMachine* machine);

void cFielder::fn_8002E0FC()
{
    UnidentifiedScriptMachine* machine = mUnidentified428->mUnidentified18;
    if (machine != 0)
    {
        fn_803198F4(machine);
        fn_80319DA0(mUnidentified428->mUnidentified18);
    }
}

int cFielder::fn_8002E9D0() const
{
    UnidentifiedScriptMachine* machine = mUnidentified428->mUnidentified18;
    if (machine != 0 && machine->mUnidentified008 != 0)
    {
        return machine->mUnidentified008->mUnidentifiedState;
    }
    return -1;
}

bool cFielder::fn_8003499C() const
{
    bool result = false;
    int state;
    if (mUnidentified428->mUnidentified18 != 0
        && mUnidentified428->mUnidentified18->mUnidentified004 != 0)
    {
        state = mUnidentified428->mUnidentified18->mUnidentified004
                    ->UnidentifiedGetState();
    }
    else
    {
        state = -1;
    }
    if (state == 0x16)
    {
        result = ((DesireReceivePass*)
            mUnidentified428->mUnidentified18->mUnidentified004)->fn_800C0E54();
    }
    return result;
}
