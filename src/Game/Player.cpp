#include <stddef.h>
#include <float.h>
#include <math.h>

#include "Game/Player.h"

#include "Game/AI/AIPad.h"
#include "Game/AI/AiUtil.h"
#include "Game/AI/AvoidableObject.h"
#include "Game/AI/DesireSteering.h"
#include "Game/AI/DesireReceivePass.h"
#include "Game/AI/HeadTrack.h"
#include "Game/AI/FuzzyAIRuntime.h"
#include "Game/AI/ShotMeter.h"
#include "Game/AI/Scripts/ScriptQuestions.h"
#include "Game/AI/SpaceSearch.h"
#include "Game/AnimInventory.h"
#include "Game/Ball.h"
#include "Game/Render/BulletBill.h"
#include "Game/DebugWriteCache.h"
#include "Game/DB/StatsTracker.h"
#include "Game/EventDataTypes.h"
#include "Game/EventRegistry.h"
#include "Game/Effects/EmissionManager.h"
#include "Game/FormationDefines.h"
#include "Game/GameTweaks.h"
#include "Game/Goalie.h"
#include "Game/PoseAccumulator.h"
#include "Game/Physics/PhysicsAIBall.h"
#include "Game/Physics/PhysicsCharacter.h"
#include "Game/Physics/PhysicsColumn.h"
#include "Game/Physics/PhysicsPatch.h"
#include "Game/Render/NPCManager.h"
#include "Game/Render/PeachPhoto.h"
#include "Game/Render/ChainChomp.h"
#include "Game/Render/BirdoEgg.h"
#include "Game/Render/tu_801B43F8.h"
#include "Game/Render/ShootToScoreMeter.h"
#include "Game/RumbleActions.h"
#include "Game/SAnim/pnFeather.h"
#include "Game/SAnim/pnSAnimController.h"
#include "Game/SAnim/pnSingleAxisBlender.h"
#include "Game/Sys/audio.h"
#include "Game/Task/FixedUpdateTask.h"
#include "NL/nlMain.h"
#include "NL/nlString.h"
#include "Game/NetworkInput.h"
#include "Game/Render/YoshiEggObject.h"
#include "Game/Render/ThwompObject.h"
#include "Game/Render/HammerObject.h"
#include "Game/Render/KoopaShellObject.h"
#include "NL/nlPolygonRegion.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/Audio/RegistryPools.h"

extern "C" cPlayer* fn_80096514(
    cPlayer* pSelf, cTeam* pTeam, int nNumPlayers,
    nlVector3* pPosition, bool bParam);
extern "C" void fn_801BCC38(cCharacter*);
extern "C" void fn_801BCE2C(cCharacter*);
extern "C" void fn_801BCAD4(cCharacter*);
extern "C" void fn_801BCC9C(cCharacter*);
extern "C" void fn_8002E3F8(cFielder*);
extern FuzzyVariant fvNotSet;
extern float g_fFixedUpdateTick;
extern "C" float fn_800DAD84(const nlVector3&, const nlVector3&, unsigned short,
                             float, const nlVector2&, const nlVector2&, bool, bool);
extern "C" UnidentifiedFuzzyRuntimeBase* fn_800E30AC(cTeam*);
extern "C" UnidentifiedVariant_80054AB8 fn_80099660(
    UnidentifiedFuzzyRuntimeBase*, const char*, cPlayer*);
extern "C" float fn_800DF028(cFielder*);
extern "C" void fn_801B73B8(cPlayer*, bool);
extern "C" bool fn_8003E8A0(const cFielder*);
extern "C" bool fn_8003E948(const cFielder*);
extern "C" bool fn_8003E99C(const cFielder*);
extern "C" void fn_8003F1E8(cFielder*);
extern "C" void fn_80031C3C(cFielder*, float);
extern "C" void fn_80015B38(cBall*, bool);

static u16 g_aOOIConstraint = (u16)DegreesToAngle(180.0f);
static u8 sbNoBallPickups;
static int g_nHeadTiltMax = DegreesToAngle(45.0f);
static int g_nHeadSpinMax = DegreesToAngle(85.0f);
static int lbl_806E0D7C = DegreesToAngle(65.0f);
static int lbl_806E0D80 = DegreesToAngle(65.0f);
static int lbl_806E0D84 = DegreesToAngle(70.0f);
static int lbl_806E0D88 = DegreesToAngle(10.0f);
static int lbl_806E0D8C = DegreesToAngle(30.0f);
static int lbl_806E0D90 = DegreesToAngle(45.0f);
static int lbl_806E0D94 = DegreesToAngle(10.0f);
static int lbl_806E0D98 = DegreesToAngle(45.0f);
static int lbl_806E0D9C = DegreesToAngle(75.0f);
static int lbl_806E0DA0 = DegreesToAngle(85.0f);

cPlayer::cPlayer(int nPlayerID, eCharacterClass characterClass,
                 const int* nModelID, cSHierarchy* hierarchy, cAnimInventory* animInventory,
                 const CharacterPhysicsData* physData, float fPhysCapsuleHeight, float fPhysCapsuleRadius,
                 AnimRetargetList* animRetargetList, int nIndex, eClassTypes classType)
    : cCharacter(characterClass, nModelID, hierarchy, animInventory, physData,
                 fPhysCapsuleHeight, fPhysCapsuleRadius, animRetargetList, nIndex, classType)
{
    mUnidentified1E4.m_ID = nPlayerID;
    m_pController = NULL;
    m_pBall = NULL;
    m_pTeam = NULL;
    mUnidentified318 = 0;
    {
        cSHierarchy* pHierarchy = m_pPoseAccumulator->m_BaseSHierarchy;
        m_nBallJointIndex = pHierarchy->GetNodeIndexByID(nlStringLowerHash("ball"));
    }
    {
        cSHierarchy* pHierarchy = m_pPoseAccumulator->m_BaseSHierarchy;
        m_nRightFootJointIndex = pHierarchy->GetNodeIndexByID(nlStringLowerHash("bip01 r foot"));
    }
    {
        cSHierarchy* pHierarchy = m_pPoseAccumulator->m_BaseSHierarchy;
        m_nLeftFootJointIndex = pHierarchy->GetNodeIndexByID(nlStringLowerHash("bip01 l foot"));
    }
    {
        cSHierarchy* pHierarchy = m_pPoseAccumulator->m_BaseSHierarchy;
        m_nLeftHandJointIndex = pHierarchy->GetNodeIndexByID(nlStringLowerHash("bip01 l hand"));
    }
    {
        cSHierarchy* pHierarchy = m_pPoseAccumulator->m_BaseSHierarchy;
        m_nRightHandJointIndex = pHierarchy->GetNodeIndexByID(nlStringLowerHash("bip01 r hand"));
    }
    m_pReceivePassLayer = new cPN_Feather(m_pPoseAccumulator->m_BaseSHierarchy, NULL, 0);
    cSHierarchy* pHierarchy = m_pPoseAccumulator->m_BaseSHierarchy;
    int spineNode = pHierarchy->GetNodeIndexByID(nlStringLowerHash("bip01 spine1"));
    m_pReceivePassLayer->SetNodeWeight(spineNode, 1.0f, 0.2f);
    m_pReceivePassLayer->SetNodeWeight(m_nBallJointIndex, 1.0f);
    m_pAILayer = m_pReceivePassLayer->GetChildPtr(0);
    m_pPowerupLayer = new cPN_Feather(m_pPoseAccumulator->m_BaseSHierarchy, NULL, 0);
    m_pPowerupLayer->SetChild(0, m_pReceivePassLayer);
    m_pPoseTree = m_pPowerupLayer;
    m_pSpaceSearch = NULL;
    SetAnimState(0, true, 0.2f, false, false);
    PostPhysicsUpdate();
    mUnidentified1E4.m_tSwapFacingTimer.SetSeconds(0.0f);
    ClearSwapControllerTimer();
    if (classType == FIELDER)
        mUnidentified320 = new (nlMalloc(sizeof(AvoidableFielder), 8, false)) AvoidableFielder((cFielder*)this);
    else if (classType == GOALIE)
        mUnidentified320 = new (nlMalloc(sizeof(AvoidableGoalie), 8, false)) AvoidableGoalie(this);
    else
        mUnidentified320 = NULL;
}

void cPlayer::Unknown10(const nlVector3& v3Position, unsigned short aDirection)
{
    cCharacter::Unknown10(v3Position, aDirection);
    mUnidentified1E4.UnidentifiedReset();
    mUnidentified1E4.m_ResetBaseBallOrientation = true;
    FieldLocToAILoc(mUnidentified1E4.m_v3AIPosition, v3Position, (eTeamSide)m_pTeam->m_nSide);
    ClearSwapControllerTimer();
    if (m_pBall == NULL)
        mUnidentified1E4.m_tBallUnPossessionTimer.SetSeconds(2.5f);
    InitActionPostWhistle();
}

cPlayer::~cPlayer()
{
    EndBlur();
    if (m_pBall != NULL)
    {
        ReleaseBall(0);
    }
    delete m_pSpaceSearch;
    delete mUnidentified320;
}

void cPlayer::SetSpaceSearch(SpaceSearch* pSpaceSearch)
{
    if (m_pSpaceSearch != NULL)
    {
        delete m_pSpaceSearch;
    }
    m_pSpaceSearch = pSpaceSearch;
}

extern "C" void fn_800957E4(cPlayer* pPlayer, cTeam* pTeam)
{
    pPlayer->m_pTeam = pTeam;
    pPlayer->mUnidentified318 = pTeam->m_nSide == 0 ? 1 : 5;
    bool bSidekick = !pPlayer->fn_8001E168();
    if (bSidekick && pPlayer->m_eClassType == FIELDER)
    {
        pPlayer->mUnidentified318 += pPlayer->mUnidentified1E4.m_ID;
    }
    else if (pPlayer->m_eClassType == GOALIE)
    {
        pPlayer->mUnidentified318 = 9;
    }
}

extern "C" void fn_80095870(cPlayer* pPlayer)
{
    if (pPlayer->m_pController != NULL)
    {
        bool bMoving = pPlayer->m_pController->GetMovementStickMagnitude() > 0.0f;
        unsigned short aStickDirection = pPlayer->m_pController->GetMovementStickDirection();
        if (bMoving)
        {
            pPlayer->Unknown8(aStickDirection, true);
        }
        else
        {
            pPlayer->mUnidentified024.m_aDesiredFacingDirection
                = pPlayer->mUnidentified024.m_aActualFacingDirection;
        }
    }
    if (pPlayer->m_eAnimID >= 0x56 || pPlayer->m_eAnimID < 0x52)
    {
        pPlayer->mUnidentified024.m_aDesiredMovementDirection
            = pPlayer->mUnidentified024.m_aDesiredFacingDirection;
    }
}

u8 cPlayer::SwapController(bool bParam)
{
    if (mUnidentified1E4.m_bCanTestController)
    {
        if (!g_pBall->UnidentifiedHasPassTarget()
            || g_pBall->m_pPassTarget != this)
        {
            cPlayer* pSwapPlayer = NULL;
            float fSwapScore = -1.0f;
            int iPadID = GetGlobalPad()->GetPadID();
            if (bParam)
            {
                pSwapPlayer = m_pTeam->GetPlayer(0);
                if (pSwapPlayer->m_eClassType == FIELDER)
                {
                    if (pSwapPlayer->m_pController != NULL || pSwapPlayer == this)
                        return 0;
                    if (!((cFielder*)pSwapPlayer)->CanReceivePass()
                        && !((cFielder*)pSwapPlayer)->fn_8003EA6C())
                        return 0;
                }
            }
            if (!bParam)
            {
                for (int i = 0; i < 4; i++)
                {
                    cPlayer* pPotentialSwapPlayer = m_pTeam->GetPlayer(i);
                    if (pPotentialSwapPlayer != this && pPotentialSwapPlayer->GetGlobalPad() == NULL
                        && !((cFielder*)pPotentialSwapPlayer)->IsFallenDown()
                        && !((cFielder*)pPotentialSwapPlayer)->IsStuck()
                        && !((cFielder*)pPotentialSwapPlayer)->IsShattered())
                    {
                        if (pSwapPlayer == NULL
                            || (pPotentialSwapPlayer->m_tSwapControllerTimer[iPadID].GetSeconds() == 0.0f
                                && pSwapPlayer->m_tSwapControllerTimer[iPadID].GetSeconds() > 0.0f))
                        {
                            pSwapPlayer = pPotentialSwapPlayer;
                            continue;
                        }
                        float fSwapPlayerTime = pSwapPlayer->m_tSwapControllerTimer[iPadID].GetSeconds();
                        if (pPotentialSwapPlayer->m_tSwapControllerTimer[iPadID].GetSeconds() <= fSwapPlayerTime)
                        {
                            if (fSwapScore < 0.0f)
                            {
                                fSwapScore = fn_80099660(fn_800E30AC(m_pTeam),
                                                         "AssignControllerScore",
                                                         pSwapPlayer)
                                                 .mData.f;
                            }
                            float fPotentialScore = fn_80099660(fn_800E30AC(m_pTeam),
                                                                "AssignControllerScore",
                                                                pPotentialSwapPlayer)
                                                        .mData.f;
                            if (fPotentialScore > fSwapScore)
                            {
                                pSwapPlayer = pPotentialSwapPlayer;
                                fSwapScore = fPotentialScore;
                            }
                        }
                    }
                }
            }
            if (pSwapPlayer == NULL && m_eClassType == GOALIE)
            {
                if (m_pTeam->GetCaptain()->GetGlobalPad() == NULL)
                {
                    pSwapPlayer = m_pTeam->GetCaptain();
                }
                else
                {
                    for (int i = 0; i < 4; i++)
                    {
                        cFielder* pCandidate = m_pTeam->GetFielder(i);
                        if (pCandidate != this && pCandidate->GetGlobalPad() == NULL)
                            pSwapPlayer = pCandidate;
                    }
                }
            }
            if (pSwapPlayer == NULL)
            {
                for (int i = 0; i < 4; i++)
                {
                    cFielder* pCandidate = m_pTeam->GetBallInterceptFielder(i);
                    if (pCandidate != this && pCandidate->GetGlobalPad() == NULL
                        && pCandidate->CanReceivePass() == true)
                    {
                        pSwapPlayer = pCandidate;
                        break;
                    }
                }
                if (pSwapPlayer == NULL)
                {
                    for (int i = 0; i < 4; i++)
                    {
                        cFielder* pCandidate = m_pTeam->GetBallInterceptFielder(i);
                        if (pCandidate != this && pCandidate->GetGlobalPad() == NULL
                            && !pCandidate->IsShattered())
                        {
                            pSwapPlayer = pCandidate;
                            break;
                        }
                    }
                }
                if (pSwapPlayer == NULL)
                    return 0;
            }
            if (mUnidentified024.m_fActualSpeed > 1.0f)
            {
                pSwapPlayer->mUnidentified1E4.m_tSwapFacingTimer.SetSeconds(gGameTweaks.m_pGameTweaks->fSwapFacingTime);
                pSwapPlayer->mUnidentified1E4.m_aSwapFacingDirection = pSwapPlayer->mUnidentified024.m_aActualMovementDirection;
            }
            m_tSwapControllerTimer[iPadID].SetSeconds(gGameTweaks.m_pGameTweaks->fSwapControllerTime);
            pSwapPlayer->SetAIPad(m_pController);
            pSwapPlayer->mUnidentified1E4.m_bCanTestController = false;
            SetAIPad(NULL);
            return 1;
        }
    }
    return 0;
}

void cPlayer::Update(float fDeltaT)
{
    if (m_pController != NULL)
    {
        mUnidentified1E4.m_UserControlledTime += fDeltaT;
    }
    else
    {
        mUnidentified1E4.m_UserControlledTime = 0.0f;
    }

    fn_80095DF4(fDeltaT);

    FieldLocToAILoc(
        mUnidentified1E4.m_v3AIPosition, mUnidentified024.m_v3Position, (eTeamSide)m_pTeam->m_nSide);
}

void cPlayer::fn_80095DF4(float fDeltaT)
{
    if (g_pGame->IsGameplayOrOvertime())
    {
        if (m_pBall != NULL)
        {
            mUnidentified1E4.m_tBallPossessionTimer.Countup(fDeltaT, 10.0f);
            StatsTracker::Instance()->TrackStat(STATS_16, m_pTeam->m_nSide, mUnidentified1E4.m_ID, 100.0f * fDeltaT, 0, 0, 0);
        }
        else
        {
            mUnidentified1E4.m_tBallUnPossessionTimer.Countup(fDeltaT, 10.0f);
        }
        mUnidentified1E4.m_tSwapFacingTimer.Countdown(fDeltaT, 0.0f);
        float fScaledDt = fDeltaT * (1.0f / FixedUpdateTask::GetTargetTimeScale());
        for (int iPadID = 0; iPadID < 16; iPadID++)
        {
            m_tSwapControllerTimer[iPadID].Countdown(fScaledDt, 0.0f);
        }
        if (mUnidentified024.m_fActualSpeed < 2.5f)
        {
            mUnidentified1E4.m_tInactivityTimer.Countup(fDeltaT, 10.0f);
        }
        else
        {
            mUnidentified1E4.m_tInactivityTimer.UnidentifiedClear();
        }
        if (mUnidentified1E4.m_fSkipTimer > 0.0f)
        {
            mUnidentified1E4.m_fSkipTimer -= fDeltaT;
            if (mUnidentified1E4.m_fSkipTimer <= 0.0f)
            {
                mUnidentified1E4.m_fSkipTimer = 0.0f;
                mUnidentified1E4.m_bSkipActionUpdate = false;
                mUnidentified1E4.m_bSkipAnimUpdate = false;
            }
        }
        if (!mUnidentified1E4.m_bSkipActionUpdate)
        {
            mUnidentified1E4.m_tSlideAttackTimer.Countdown(fDeltaT, 0.0f);
            if (mUnidentified1E4.m_tNoPickupTimer.m_uPackedTime != 0)
            {
                m_pPhysicsCharacter->m_CanCollideWithBall = mUnidentified1E4.m_tNoPickupTimer.Countdown(fDeltaT, 0.0f);
            }
            else
            {
                m_pPhysicsCharacter->m_CanCollideWithBall = true;
            }
            if (mUnidentified1E4.m_tFireTimer.m_uPackedTime != 0)
            {
                if (m_eClassType == FIELDER && FixedUpdateTask::GetTargetTimeScale() == 1.0f
                    && mUnidentified1E4.m_tFireTimer.GetSeconds() > 1.1f)
                {
                    float fWholeSeconds = floor(mUnidentified1E4.m_tFireTimer.GetSeconds());
                    if (mUnidentified1E4.m_tFireTimer.GetSeconds() - fWholeSeconds < 0.02f)
                    {
                        unsigned long sound = fn_8001E168() ? 0x3642C41B : 0x00E606A2;
                        PlaySound(mUnidentified318, sound, NULL, NULL);
                    }
                }
                if (mUnidentified1E4.m_tFireTimer.Countdown(fDeltaT, 0.0f))
                {
                    fn_800974B0();
                }
            }
        }
    }
}

extern "C" void fn_802B5370(nlQuaternion&, const nlVector3&, unsigned short);

void cPlayer::GetAnimatedBallOrientation(nlQuaternion& qRetval)
{
    float time = m_pCurrentAnimController->m_fTime;
    cSAnim* pSAnim = m_pCurrentAnimController->m_pSAnim;
    int sanimBallNode = m_pCurrentAnimController->RemapNode(m_nBallJointIndex);
    RotAccum* rot = &m_pPoseAccumulator->m_rot[m_nBallJointIndex];
    rot->q.x = 0.0f;
    rot->q.y = 0.0f;
    rot->q.z = 0.0f;
    rot->q.w = 1.0f;
    rot->quatAccumulatedWeight = 0.0f;
    rot->rotAroundZ = 0;
    rot->rotAroundZAccumulatedWeight = 0.0f;
    rot->bIdentity = true;
    pSAnim->BlendRot(m_nBallJointIndex, sanimBallNode, time, 1.0f, m_pPoseAccumulator, m_pCurrentAnimController->m_bMirror);
    nlQuaternion qAnimated = rot->q;
    nlQuaternion qFacing;
    nlVector3 v3Up = { 0.0f, 0.0f, 1.0f };
    nlQuaternion qOrient;
    nlQuaternion startInverse;
    fn_802B5370(qFacing, v3Up, (unsigned short)(int)(10430.378f * AngUnitsToRad_fromUnsignedShort(mUnidentified024.m_aActualFacingDirection)));
    nlMultQuat(qOrient, qFacing, qAnimated);
    if (mUnidentified1E4.m_ResetBaseBallOrientation)
    {
        startInverse.x = -qOrient.x;
        startInverse.y = -qOrient.y;
        startInverse.z = -qOrient.z;
        startInverse.w = qOrient.w;
        nlMultQuat(mUnidentified1E4.m_BaseBallOrientation, startInverse, g_pBall->m_qOrientation);
        mUnidentified1E4.m_ResetBaseBallOrientation = false;
    }
    nlMultQuat(qRetval, qOrient, mUnidentified1E4.m_BaseBallOrientation);
}

void cPlayer::SetAnimID(int animID)
{
    cCharacter::SetAnimID(animID);
    eBallRotationMode ballRotationMode
        = (eBallRotationMode)m_pAnimInventory->GetBallRotationMode(animID);
    if (mUnidentified024.m_eCharacterClass != MYSTERY)
    {
        mUnidentified1E4.m_eBallRotationMode = ballRotationMode;
        if (m_pBall != NULL)
        {
            mUnidentified1E4.m_ResetBaseBallOrientation = true;
        }
    }
    else
    {
        mUnidentified1E4.m_ResetBaseBallOrientation = true;
        mUnidentified1E4.m_eBallRotationMode = BRM_ANIMATED;
    }
}

float lbl_806DBD80 = 0.75f;
float lbl_806DBD84 = 0.4f;

float cPlayer::DoFlashLight(const nlVector3& Position, unsigned short aDirection,
                            float fAngleWeighting, float fIgnoreObjectCloserThanThis,
                            float fIgnoreObjectFartherThanThis)
{
    nlVector2 v2Difference;
    nlVec2Sub(v2Difference, *(const nlVector2*)&Position, *(const nlVector2*)&mUnidentified024.m_v3Position);
    float fDistBetween = nlSqrt(nlVec2LengthSquared(v2Difference), true);
    if (fDistBetween < fIgnoreObjectCloserThanThis
        || fDistBetween > fIgnoreObjectFartherThanThis)
    {
        fDistBetween *= 1.5f;
    }
    float dx = v2Difference.x;
    float dy = v2Difference.y;
    unsigned short aTarget = (unsigned short)(int)(10430.378f
                                                   * nlATan2f(dy, dx));
    short aDifference = nlAngleDiff(aDirection, aTarget);
    unsigned short aAbsolute = aDifference < 0 ? -aDifference : aDifference;
    short aAngle = aAbsolute;
    float fWeight = m_pBall != NULL ? lbl_806DBD80 : lbl_806DBD84;
    float fDistanceScore = InterpolateRangeClamped(0.0f, 1.0f, fIgnoreObjectCloserThanThis, fIgnoreObjectFartherThanThis, fDistBetween);
    float fAngleScore = InterpolateRangeClamped(1.0f, 0.0f, 21845.0f, 2048.0f, aAngle);
    return fDistanceScore * fWeight + fAngleScore * (1.0f - fWeight);
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

extern "C" cPlayer* fn_80096514(cPlayer* pSelf, cTeam* pTeam, int nNumPlayers,
                                nlVector3* pPosition, bool bParam)
{
    cPlayer* pClosestPlayer = NULL;
    float fClosestDistSquared = FLT_MAX;
    nlVector3 v3RefPos = pPosition == NULL ? pSelf->mUnidentified024.m_v3Position : *pPosition;
    nlVector3 v3Position = v3RefPos;
    for (int i = 0; i < nNumPlayers; i++)
    {
        cPlayer* pPlayer = pTeam->GetPlayer(i);
        if (pPlayer == pSelf)
        {
            continue;
        }
        if (bParam && Incapacitated(pPlayer))
        {
            continue;
        }
        nlVector2 v2Difference;
        v2Difference.x = v3Position.x - pPlayer->mUnidentified024.m_v3Position.x;
        v2Difference.y = v3Position.y - pPlayer->mUnidentified024.m_v3Position.y;
        float fDistSquared = v2Difference.x * v2Difference.x + v2Difference.y * v2Difference.y;
        if (fDistSquared < fClosestDistSquared)
        {
            pClosestPlayer = pPlayer;
            fClosestDistSquared = fDistSquared;
        }
    }
    return pClosestPlayer;
}

bool cPlayer::IsOnSameTeam(cPlayer* other)
{
    if ((other != NULL) && (other->m_pTeam == m_pTeam))
    {
        return true;
    }
    return false;
}

void cPlayer::PickupBall(cBall* pBall)
{
    ReceiveBallData data;
    if (!(m_eClassType == GOALIE && ((Goalie*)this)->mbNoUserControl)
        && GetGlobalPad() == NULL)
    {
        cPlayer* closest = NULL;
        bool bFindClosest = true;
        if (m_pTeam->GetGoalie()->GetGlobalPad() != NULL)
        {
            closest = m_pTeam->GetGoalie();
            bFindClosest = false;
        }
        if (bFindClosest)
        {
            float bestDistSq;
            for (int i = 0; i < 4; i++)
            {
                cPlayer* player = m_pTeam->GetPlayer(i);
                if (player->GetGlobalPad() != NULL)
                {
                    if (closest != NULL)
                    {
                        float distSq = nlVec3DistanceSquared2D(
                            player->mUnidentified024.m_v3Position,
                            mUnidentified024.m_v3Position);
                        bool bPassTarget = g_pBall->UnidentifiedPassState()
                            && g_pBall->m_pPassTarget != NULL;
                        if ((bPassTarget && closest == g_pBall->m_pPrevOwner) || distSq < bestDistSq)
                        {
                            bool bPlayerPassTarget = g_pBall->UnidentifiedPassState()
                                && g_pBall->m_pPassTarget != NULL;
                            if (!bPlayerPassTarget || player != g_pBall->m_pPrevOwner)
                            {
                                bestDistSq = distSq;
                                closest = player;
                            }
                        }
                        if (((cFielder*)player)->IsShattered() || ((cFielder*)player)->fn_800344B0())
                        {
                            closest = player;
                            break;
                        }
                    }
                    else
                    {
                        closest = player;
                    }
                }
            }
        }
        if (closest != NULL)
        {
            SetAIPad(closest->m_pController);
            closest->SetAIPad(NULL);
        }
    }
    data.pReceiver = this;
    data.eResult = RECEIVEBALL_LOOSE_PICKUP;
    if (pBall->m_pPassTarget != NULL)
    {
        if (pBall->m_pPassTarget == this)
            data.eResult = RECEIVEBALL_PASS_COMPLETE;
        else if (pBall->m_pPassTarget->m_pTeam != m_pTeam)
            data.eResult = RECEIVEBALL_PASS_INTERCEPT;
    }
    g_pGame->mUnidentified49C.mEvent15.Deliver(&data);
    if (data.eResult == RECEIVEBALL_PASS_COMPLETE)
    {
        bool bOneTouchShot = false;
        if (m_eClassType == FIELDER && fn_800DF028((cFielder*)this))
            bOneTouchShot = true;
        if (!bOneTouchShot)
        {
            fn_801B73B8(this, false);
            PlayRumbleAction(1, GetGlobalPad());
        }
    }
    else if (m_eClassType != GOALIE || ((Goalie*)this)->mGoalieActionState != 26)
    {
        PlayRumbleAction(1, GetGlobalPad());
    }
    if (m_eClassType == FIELDER)
        ((cFielder*)this)->m_pShotMeter->Abort();
    mUnidentified1E4.m_tBallPossessionTimer.UnidentifiedClear();
    mUnidentified1E4.m_tBallUnPossessionTimer.UnidentifiedClear();
    pBall->SetOwner(this);
    if (m_pBall != NULL)
    {
        m_pBall->m_pPhysicsBall->EnableCollisions();
        m_pPhysicsCharacter->ContainObject(m_pBall->m_pPhysicsBall);
    }
    if (m_pBall != NULL)
    {
        nlVector3 jointPos = GetJointPosition(m_nBallJointIndex);
        float scale = mUnidentified024.m_fPlayerScale;
        if (scale > 1.0f)
        {
            float radius = g_pBall->m_pPhysicsBall->GetRadius();
            jointPos.z -= radius * (scale - 1.0f);
        }
        m_pPhysicsCharacter->m_SubObject.SetSubObjectPosition(jointPos, PhysicsObject::WORLD_COORDINATES);
    }
    mUnidentified1E4.m_ResetBaseBallOrientation = true;
}

extern "C" cPlayer* fn_80096F54(cPlayer* pSelf, bool bVolleyPass)
{
    float fBestScore = 0.0f;
    cPlayer* pBestTarget = NULL;
    float fPassing = pSelf->m_eClassType == FIELDER
                       ? ((cFielder*)pSelf)->GetTweaks()->fPassing.GetValue()
                       : 0.5f;
    cFielder* pTarget;
    float fDistanceWeight;
    nlVector2 v2Distance;
    nlVector2 v2Angle;
    if (bVolleyPass)
    {
        fDistanceWeight = gGameTweaks.m_pGameTweaks->fVolleyPassDistanceWeight;
        v2Distance.x = InterpolateClamped(gGameTweaks.m_pGameTweaks->fVolleyPassIdealMinDistance,
                                          gGameTweaks.m_pGameTweaks->fVolleyPassIdealMaxDistance,
                                          fPassing);
        v2Distance.y = InterpolateClamped(gGameTweaks.m_pGameTweaks->fVolleyPassIdealMinRange,
                                          gGameTweaks.m_pGameTweaks->fVolleyPassIdealMaxRange,
                                          fPassing);
        v2Angle.x = (unsigned short)((int)(65536.0f * (float)(int)gGameTweaks.m_pGameTweaks->nVolleyPassMaximumAngle) / 360);
        v2Angle.y = (unsigned short)((int)(65536.0f * (float)(int)gGameTweaks.m_pGameTweaks->nVolleyPassMinimumAngle) / 360);
    }
    else
    {
        fDistanceWeight = gGameTweaks.m_pGameTweaks->fPassDistanceWeight;
        v2Distance.x = InterpolateClamped(gGameTweaks.m_pGameTweaks->fPassIdealMinDistance,
                                          gGameTweaks.m_pGameTweaks->fPassIdealMaxDistance,
                                          fPassing);
        v2Distance.y = InterpolateClamped(gGameTweaks.m_pGameTweaks->fPassIdealMinRange,
                                          gGameTweaks.m_pGameTweaks->fPassIdealMaxRange,
                                          fPassing);
        v2Angle.x = (unsigned short)((int)(65536.0f * (float)(int)gGameTweaks.m_pGameTweaks->nPassMaximumAngle) / 360);
        v2Angle.y = (unsigned short)((int)(65536.0f * (float)(int)gGameTweaks.m_pGameTweaks->nPassMinimumAngle) / 360);
    }
    for (int i = 0; i < 4; i++)
    {
        pTarget = pSelf->m_pTeam->GetAIOrderedFielder(i);
        if (pTarget == pSelf || !pTarget->CanReceivePass())
        {
            continue;
        }
        unsigned short aDirection = pSelf->mUnidentified024.m_aActualFacingDirection;
        if (pSelf->m_pController != NULL
            && pSelf->m_pController->GetMovementStickMagnitude() > 0.01f)
        {
            aDirection = pSelf->m_pController->GetMovementStickDirection();
        }
        float fScore = fn_800DAD84(pSelf->mUnidentified024.m_v3Position,
                                   pTarget->mUnidentified024.m_v3Position,
                                   aDirection,
                                   fDistanceWeight,
                                   v2Distance,
                                   v2Angle,
                                   true,
                                   false);
        if (pBestTarget == NULL || fScore > fBestScore)
        {
            pBestTarget = pTarget;
            fBestScore = fScore;
        }
    }
    return pBestTarget;
}

DetInput* cPlayer::GetGlobalPad()
{
    if (m_pController != NULL)
    {
        return m_pController->m_pGlobalPad;
    }
    return NULL;
}

cGlobalPad* cPlayer::fn_800972CC()
{
    DetInput* pGlobalPad
        = m_pController != NULL ? m_pController->m_pGlobalPad : NULL;
    cGlobalPad* pResult = NULL;
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

void cPlayer::SetAIPad(cAIPad* pPad)
{
    m_pController = pPad;
    if (m_eClassType != FIELDER)
    {
        return;
    }
    cFielder* pFielder = (cFielder*)this;
    if (m_pBall != NULL)
    {
        u8 bShotInProgress = true;
        eShotMeterState state;
        ShotMeter* pMeter = pFielder->m_pShotMeter;
        state = pMeter->m_eShotMeterState;
        if (state != SHOT_METER_ACTIVE && state != SHOT_METER_STS_ACTIVE)
            bShotInProgress = false;
        if (bShotInProgress || (pFielder->GetActionState() != ACTION_UNKNOWN_15 && pMeter->m_eShotMeterState == SHOT_METER_RELEASED))
        {
            pFielder->fn_8004B86C(false, false);
            return;
        }
    }
    if (m_pController != NULL && pFielder->fn_8002E060() < 20)
    {
        if (g_pGame->IsGameplayOrOvertime()
            && (pFielder->GetActionState() == ACTION_SHOOT_TO_SCORE
                || pFielder->GetActionState() == ACTION_SHOT))
        {
            pFielder->InitDesire(FIELDERDESIRE_FINISH_ACTION, 0.5f, -1.0f, fvNotSet, fvNotSet);
            return;
        }
        pFielder->EndDesire();
        return;
    }
    if (m_pController == NULL && pFielder->fn_8002E060() == 20)
    {
        if (g_pGame->IsGameplayOrOvertime()
            && (pFielder->fn_80038660()
                || pFielder->GetActionState() == ACTION_SHOOT_TO_SCORE
                || pFielder->GetActionState() == ACTION_SHOT))
        {
            pFielder->InitDesire(FIELDERDESIRE_FINISH_ACTION, 0.5f, -1.0f, fvNotSet, fvNotSet);
            return;
        }
        pFielder->EndDesire();
        return;
    }
    if (m_pController != NULL && pFielder->fn_8002E060() == 31 && g_pGame->m_eGameState == 1)
    {
        pFielder->EndDesire();
    }
}

extern "C" void fn_80097358(cPlayer* pPlayer, float fDuration)
{
    float fRemaining = pPlayer->mUnidentified1E4.m_tFireTimer.m_uPackedTime != 0
                         ? pPlayer->mUnidentified1E4.m_tFireTimer.GetSeconds()
                         : 0.0f;
    if (pPlayer->m_eClassType == GOALIE)
    {
        if (fRemaining <= 0.0f)
        {
            const EffectsGroup* pGroup = EmissionManager::Instance()->GetEffectsGroup("skillshot_hand_fire");
            if (pPlayer->IsPlayingEffect(pGroup))
            {
                pPlayer->KillEffect(pGroup);
            }
        }
        fn_801BCAD4(pPlayer);
        if (fDuration > 0.0f)
        {
            pPlayer->mUnidentified1E4.m_tFireTimer.SetSeconds(fDuration);
        }
    }
    else
    {
        fn_801BCC9C(pPlayer);
        pPlayer->AddRandomDirt();
        pPlayer->fn_8001F1C0(2);
        fn_8002E3F8((cFielder*)pPlayer);
        pPlayer->mUnidentified1E4.m_tFireTimer.SetSeconds(fDuration);
        if (fRemaining <= 0.0f)
        {
            unsigned long sound = pPlayer->fn_8001E168() ? 0x3642C41B : 0x00E606A2;
            PlaySound(pPlayer->mUnidentified318, sound, NULL, NULL);
        }
    }
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
    mUnidentified1E4.m_tFireTimer.m_unk0 = mUnidentified1E4.m_tFireTimer.m_uPackedTime != 0;
    mUnidentified1E4.m_tFireTimer.m_uPackedTime = 0;
}

void cPlayer::fn_8009750C()
{
    if (mUnidentified1E4.m_tFireTimer.m_uPackedTime != 0)
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
    mUnidentified1E4.m_nFeatherAnimID = animID;
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

bool cPlayer::fn_800976C4() const
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

extern "C" bool fn_800977A4(const cPlayer* pPlayer, float fParam)
{
    float leftFootZ = pPlayer->GetJointPosition(pPlayer->m_nLeftFootJointIndex).z;
    float rightFootZ = pPlayer->GetJointPosition(pPlayer->m_nRightFootJointIndex).z;
    float headZ = pPlayer->GetJointPosition(pPlayer->m_nHeadJointIndex).z;
    if (leftFootZ < fParam - 0.15f && rightFootZ < fParam - 0.15f
        && headZ < fParam - 0.15f)
    {
        return true;
    }
    return false;
}

void cPlayer::DoRegularPassing(cPlayer* pTeammate, bool bVolleyPass,
    bool bFindPosition, bool bPerfectPass, bool bParam4,
    float fMinPassSpeed, float fMaxPassSpeed)
{
    DesireReceivePass* pDesire = (DesireReceivePass*)fn_8002E08C((cFielder*)pTeammate, 22);
    pDesire->fn_800C22CC(this, bVolleyPass, bFindPosition, bPerfectPass, NULL, fMinPassSpeed, fMaxPassSpeed);
}

void cPlayer::SetPowerupAnimState(int animID)
{
    int nodeIndex;
    cPN_SAnimController* pController = NewAnimController(animID, false, false, NULL, 0);
    if (animID == 0x59 || animID == 0x5B)
    {
        cSHierarchy* pHierarchy = m_pPoseAccumulator->m_BaseSHierarchy;
        nodeIndex = pHierarchy->GetNodeIndexByID(nlStringLowerHash("bip01 l clavicle"));
    }
    else
    {
        cSHierarchy* pHierarchy = m_pPoseAccumulator->m_BaseSHierarchy;
        nodeIndex = pHierarchy->GetNodeIndexByID(nlStringLowerHash("bip01 r clavicle"));
    }
    m_pPowerupLayer->ClearNodeWeights();
    m_pPowerupLayer->SetNodeWeight(nodeIndex, 1.0f, 0.2f);
    if (m_pPowerupLayer->GetChild(1) != NULL)
    {
        delete m_pPowerupLayer->GetChild(1);
    }
    m_pPowerupLayer->SetChild(1, pController);
    m_pPowerupLayer->BeginBlendIn(0.133f);
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
    if (mUnidentified1E4.m_eBallRotationMode != BRM_ANIMATED)
    {
        return;
    }
    if (mUnidentified024.m_eCharacterClass != MYSTERY)
    {
        mUnidentified1E4.m_eBallRotationMode = BRM_MATCH_VELOCITY;
        if (m_pBall != NULL)
        {
            mUnidentified1E4.m_ResetBaseBallOrientation = true;
        }
    }
    else
    {
        mUnidentified1E4.m_ResetBaseBallOrientation = true;
        mUnidentified1E4.m_eBallRotationMode = BRM_ANIMATED;
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

void cPlayer::PlayerHeadTrackCallback(unsigned int nSelf, unsigned int nParam2,
                                      cPoseAccumulator* pPoseAccumulator, unsigned int nJointIndex, int nParentIndex)
{
    cPlayer& self = *(cPlayer*)nSelf;
    nlMatrix4& m4HeadMatrix = pPoseAccumulator->GetNodeMatrix(nJointIndex);
    int nHeadSpinMax = g_nHeadSpinMax;
    int nHeadTiltMax = g_nHeadTiltMax;
    eCharacterClass cc;
    eClassTypes classType = self.m_eClassType;
    if (classType == GOALIE)
    {
        nHeadSpinMax = lbl_806E0DA0;
        nHeadTiltMax = lbl_806E0D9C;
    }
    else
    {
        cc = self.mUnidentified024.m_eCharacterClass;
        if (cc == PEACH)
        {
            nHeadSpinMax = lbl_806E0D8C;
            nHeadTiltMax = lbl_806E0D88;
        }
        else if (cc == MARIO)
        {
            nHeadSpinMax = lbl_806E0D84;
        }
        else if (cc == DAISY)
        {
            nHeadSpinMax = lbl_806E0D90;
        }
        else if (cc == MYSTERY)
        {
            nHeadTiltMax = lbl_806E0D7C;
            nHeadSpinMax = lbl_806E0D80;
        }
        else if (cc == HAMMERBROS)
        {
            nHeadTiltMax = lbl_806E0D94;
            nHeadSpinMax = lbl_806E0D98;
        }
    }
    if (classType == GOALIE || (classType == FIELDER && !((cFielder*)&self)->IsStuck()))
    {
        self.m_pHeadTrack->Update(m4HeadMatrix, m4HeadMatrix, g_fFixedUpdateTick, g_aOOIConstraint, nHeadSpinMax, nHeadTiltMax);
        cHeadTrack* pHeadTrack = self.m_pHeadTrack;
        u16 headTilt = (u16)(int)pHeadTrack->m_fHeadTilt;
        u16 headSpin = (u16)(int)pHeadTrack->m_fHeadSpin;
        CalcHeadTrackMatrix(headSpin, headTilt, pPoseAccumulator, nJointIndex);
    }
}

void cPlayer::PreUpdate(float dt)
{
    cCharacter::PreUpdate(dt);
    mUnidentified1E4.m_bCanTestController = true;
}

void cPlayer::Unknown7(float)
{
    m_pPoseAccumulator->SetBuildNodeMatrixCallback(
        m_nHeadJointIndex, PlayerHeadTrackCallback, (unsigned int)this, 0);
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

bool cPlayer::CanPickupBall(cBall* pBall, bool bParam)
{
    if (pBall->meBallState == 10)
        return false;
    if (sbNoBallPickups)
        return false;
    if (mUnidentified1E4.m_tFireTimer.m_uPackedTime != 0)
        return false;
    if (!g_pGame->IsGameplayOrOvertime())
        return false;
    bool result = false;
    float fMaxPickupSpeed = 15.0f;
    float fSpeedSquared = pBall->m_v3Velocity.GetLengthSq3D();
    if (bParam)
    {
        fMaxPickupSpeed = 50.0f;
    }
    if (pBall->m_pOwner == NULL && pBall->m_tNoPickupTimer.m_uPackedTime == 0
        && fSpeedSquared <= nlGetLengthSquared1D(fMaxPickupSpeed)
        && mUnidentified1E4.m_tNoPickupTimer.m_uPackedTime == 0)
    {
        float fPlayerRadius = 0.0f;
        m_pPhysicsCharacter->m_pPlayerPlayerColumn->GetRadius(&fPlayerRadius);
        nlVector3 v3PrevPosition = mUnidentified024.m_v3PrevPosition;
        nlVector3 v3Position = mUnidentified024.m_v3Position;
        v3Position.z = 0.18f;
        v3PrevPosition.z = 0.18f;
        bool bCollision = TestCollision(fPlayerRadius, v3PrevPosition, v3Position, 0.18f, pBall->m_v3PrevPosition, pBall->m_v3Position);
        result = false;
        if (bCollision)
        {
            short aDifference = GetFacingDeltaToPosition(g_pBall->m_v3Position);
            unsigned short aAbsolute = aDifference < 0 ? -aDifference : aDifference;
            if (aAbsolute < 0x4000)
            {
                result = true;
            }
        }
    }
    return result;
}

void cPlayer::SetNoPickUpTime(float NewNoPickUpTime)
{
    m_pPhysicsCharacter->m_CanCollideWithBall = (NewNoPickUpTime <= 0.0f);
    mUnidentified1E4.m_tNoPickupTimer.SetSeconds(NewNoPickUpTime);
}

inline bool cPlayer::UnidentifiedPowerupPredicate()
{
    if (GetGlobalPad() != NULL)
    {
        if (m_pTeam->GetPowerUpByIndex(0).eType >= 9
            && m_pTeam->GetPowerUpByIndex(0).eType <= 20)
            return true;
        if (m_pTeam->GetPowerUpByIndex(0).eType == POWER_UP_NONE
            && m_pTeam->GetPowerUpByIndex(1).eType >= 9
            && m_pTeam->GetPowerUpByIndex(1).eType <= 20)
            return true;
    }
    return false;
}

extern "C" void fn_80098098(cPlayer* pSelf)
{
    if (pSelf->GetGlobalPad() == NULL)
        return;
    unsigned short aDirection = 0;
    bool bTogglePowerup;
    if (pSelf->m_pController->DetectLeftShake(&aDirection))
    {
        pSelf->m_pController->ResetAccelerationHistory();
        bTogglePowerup = true;
    }
    else
    {
        bTogglePowerup = false;
    }
    if (bTogglePowerup)
    {
        pSelf->m_pTeam->TogglePowerup(false);
        if (ShootToScoreMeter::instance.m_bMeterVisible)
        {
            cFielder* pOwner = g_pBall->GetOwnerFielder();
            if (pOwner != NULL && pOwner != pSelf && !pSelf->IsOnSameTeam(pOwner))
            {
                ShootToScoreMeter::instance.RumbleMeter(aDirection);
                PlayRumbleAction(1, pOwner->GetGlobalPad());
            }
        }
    }
    if (pSelf->GetGlobalPad()->JustPressed(PAD_AIM, true))
    {
        cFielder* pCaptain = pSelf->m_pTeam->GetCaptain();
        if ((fn_8003E8A0(pCaptain) || fn_8003E948(pCaptain)
             || fn_8003E99C(pCaptain) || pCaptain->fn_8003E9F0())
            && !pCaptain->mUnidentified3DC)
        {
            pCaptain->fn_80050284();
        }
        bool bCaptainPowerup = pSelf->UnidentifiedPowerupPredicate();
        if (bCaptainPowerup)
        {
            if (pSelf->m_eClassType == FIELDER)
                fn_8003F1E8((cFielder*)pSelf);
            else
                fn_8003F1E8(pSelf->m_pTeam->GetCaptain());
        }
        else if (pSelf->m_eClassType == FIELDER)
        {
            ((cFielder*)pSelf)->UseTeamPowerup(NULL);
        }
    }
    else if (!pSelf->GetGlobalPad()->IsPressed(PAD_AIM, true))
    {
        cFielder* pCaptain = pSelf->m_pTeam->GetCaptain();
        if (fn_8003E8A0(pCaptain) || fn_8003E948(pCaptain)
            || fn_8003E99C(pCaptain) || pCaptain->fn_8003E9F0())
        {
            bool bPressed = false;
            for (int i = 0; i < 4; i++)
            {
                cPlayer* pPlayer = pSelf->m_pTeam->GetPlayer(i);
                if (pPlayer->GetGlobalPad() != NULL)
                {
                    // Retail re-evaluates the pad inside the branch, matching
                    // an inlined predicate called twice.
                    if (pPlayer->GetGlobalPad() != NULL)
                    {
                        bPressed = bPressed || pPlayer->GetGlobalPad()->IsPressed(PAD_AIM, true);
                    }
                }
            }
            if (!bPressed)
                pCaptain->fn_8005001C(false);
        }
    }
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
        0x9F35CA0F,
        NULL,
        NULL);
}

extern "C" void fn_80098A84(UnidentifiedEventData_800673FC* pData)
{
    PlaySound(
        pData->mUnidentified00->mUnidentified318,
        0x85EF26D0,
        NULL,
        NULL);
}

extern "C" void fn_80099030(UnidentifiedEventData00*)
{
    if (gNPCManager != NULL)
    {
        gNPCManager->fn_801AA348();
        if (gNPCManager->mUnidentified024 != NULL)
        {
            gNPCManager->mUnidentified024->Deactivate(true);
        }
    }
}

void cPlayer::fn_80099074(const UnidentifiedEventData24*)
{
}

extern "C" void fn_80098AA0(UnidentifiedEventData_800673FC*);

extern "C" void fn_80098750()
{
    UnidentifiedFindEvent<UnidentifiedEventData_800673FC>("PeachFlash", -1)->Add(Function<UnidentifiedEventData_800673FC*>(fn_80098AA0), 0, -1);
    UnidentifiedFindEvent<UnidentifiedEventData_800673FC>("PeachCamerasDown", -1)->Add(Function<UnidentifiedEventData_800673FC*>(fn_80098A68), 0, -1);
    UnidentifiedFindEvent<UnidentifiedEventData_800673FC>("PeachCamerasAway", -1)->Add(Function<UnidentifiedEventData_800673FC*>(fn_80098A84), 0, -1);
    UnidentifiedFindEvent<UnidentifiedEventData00>("ResetEffects", -1)->Add(Function<UnidentifiedEventData00*>(fn_80099030), 0, -1);
}

float lbl_806DBD88 = 6.5f;
bool lbl_806DBD8C = true;
static nlVector3 lbl_804FF5C8[] = {
    { 0.156f, 0.973f, 0.0f },
    { 0.156f, 0.555f, 0.0f },
    { 0.004f, 0.223f, 0.0f },
    { 0.492f, 0.012f, 0.0f },
    { 0.508f, 0.055f, 0.0f },
    { 0.989f, 0.121f, 0.0f },
    { 0.910f, 0.640f, 0.0f },
    { 0.824f, 0.632f, 0.0f },
    { 0.824f, 0.972f, 0.0f },
};

extern "C" void fn_80098AA0(UnidentifiedEventData_800673FC* pData)
{
    nlPolygonRegion region;
    nlVector3 points[9];
    region.Allocate(9);
    float width = 2.0f * pData->mUnidentified10;
    float height = 2.0f * pData->mUnidentified14;
    for (int i = 0; i < 9; i++)
    {
        nlVector3& point = points[i];
        point = lbl_804FF5C8[i];
        point.x *= width;
        point.x -= width / 2.0f;
        point.x += pData->mUnidentified04.x;
        point.y = (1.0f - point.y) * height;
        point.y -= height / 2.0f;
        point.y += pData->mUnidentified04.y;
        region.AddPoint(point);
    }
    if (g_pGame != NULL && g_pGame->IsGameplayOrOvertime())
    {
        if (lbl_806DBD8C)
        {
            StartPeachPhoto(&gPeachPhotoState, &pData->mUnidentified04, lbl_806DBD88, pData->mUnidentified10, pData->mUnidentified14);
        }
        for (int i = 0; i < 2; i++)
        {
            cTeam* pTeam = g_pTeams[i];
            if (pTeam != NULL)
            {
                for (int j = 0; j < 4; j++)
                {
                    cFielder* pFielder = pTeam->GetFielder(j);
                    nlVector2 position;
                    position.x = pFielder->mUnidentified024.m_v3Position.x;
                    position.y = pFielder->mUnidentified024.m_v3Position.y;
                    if (region.ContainsPoint2D(position))
                        fn_80031C3C(pFielder, lbl_806DBD88);
                }
            }
        }
        nlVector2 ballPosition;
        ballPosition.x = g_pBall->m_v3Position.x;
        ballPosition.y = g_pBall->m_v3Position.y;
        if (region.ContainsPoint2D(ballPosition))
        {
            if (gNPCManager->mUnidentified02C != NULL && gNPCManager->mUnidentified02C->mVisible)
                gNPCManager->mUnidentified02C->Deactivate(false);
            if (gNPCManager->mpBirdoEgg != NULL && gNPCManager->mpBirdoEgg->mVisible)
                gNPCManager->mpBirdoEgg->Hide(false);
            g_pBall->SetVelocity(v3Zero, SPINTYPE_NONE, NULL);
            fn_80015B38(g_pBall, true);
        }
        g_pGame->BlowUpPowerups(region, lbl_806DBD88);
        if (gNPCManager != NULL)
        {
            ChainChomp* pChain;
            pChain = gNPCManager->GetChainChomp();
            if (pChain != NULL && !pChain->IsHidden() && !pChain->IsFrozen()
                && pChain->meChainChompState != CHAIN_STATE_FALL)
            {
                nlVector2 position;
                position.x = pChain->mv3Position.x;
                position.y = pChain->mv3Position.y;
                if (region.ContainsPoint2D(position))
                    gNPCManager->GetChainChomp()->Freeze(lbl_806DBD88, false);
            }
            for (unsigned int i = 0; i < gNPCManager->UnidentifiedCount054(); i++)
            {
                BulletBillObject* pBill = gNPCManager->fn_801A9D10(i);
                if (pBill != NULL && pBill->active)
                {
                    nlVector2 position;
                    position.x = pBill->position.x;
                    position.y = pBill->position.y;
                    if (region.ContainsPoint2D(position))
                        pBill->Hide(false);
                }
            }
            for (unsigned int i = 0; i < gNPCManager->fn_801AA32C(); i++)
            {
                HammerObject* pHammer = gNPCManager->fn_801AA3AC(i);
                if (pHammer != NULL && pHammer->mActive)
                {
                    const nlVector3* pPosition = pHammer->GetPosition();
                    nlVector2 position;
                    position.x = pPosition->x;
                    position.y = pPosition->y;
                    if (region.ContainsPoint2D(position))
                        pHammer->Freeze(lbl_806DBD88);
                }
            }
            for (unsigned int i = 0; i < 3; i++)
            {
                UnidentifiedNPC_801B43F8* pNPC = gNPCManager->fn_801A9DE0(i);
                if (pNPC != NULL && pNPC->mbIsVisible)
                {
                    nlVector2 position;
                    position.x = pNPC->mv3Position.x;
                    position.y = pNPC->mv3Position.y;
                    if (region.ContainsPoint2D(position))
                        pNPC->fn_801B4C14(lbl_806DBD88);
                }
            }
            for (unsigned int i = 0; i < 8; i++)
            {
                ThwompObject* pThwomp = gNPCManager->GetThwomp(i);
                if (pThwomp != NULL && pThwomp->mVisible)
                {
                    const nlVector3* pPosition = pThwomp->GetPosition();
                    nlVector2 position;
                    position.x = pPosition->x;
                    position.y = pPosition->y;
                    if (region.ContainsPoint2D(position))
                        pThwomp->Freeze(lbl_806DBD88);
                }
            }
            for (unsigned int i = 0; i < 60; i++)
            {
                PhysicsPatch* pPatch = lbl_806E12C8->fn_801745B8(i);
                if (pPatch != NULL && pPatch->m_bVisible)
                {
                    const nlVector3& v3Position = pPatch->GetPosition();
                    nlVector2 position;
                    position.x = v3Position.x;
                    position.y = v3Position.y;
                    if (region.ContainsPoint2D(position))
                        pPatch->fn_80173A10(lbl_806DBD88);
                }
            }
        }
        if (pData->mUnidentified00->m_pBall == NULL)
            PlaySound(pData->mUnidentified00->mUnidentified318, 0xBCA543B2, NULL, NULL);
    }
}

u16 lbl_806DBD96 = 0xFFFF;

#define REGISTER_PLAYER_FIELD(type, field) \
    cache->AddField(type, gDebugFieldTypes[type].size, (u8*)&mUnidentified1E4.field - (u8*)&mUnidentified1E4.m_ID, #field)

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
    cache->ChecksumData(lbl_806DBD96, &mUnidentified1E4.m_ID, context);
    cache->WriteData(lbl_806DBD96, &mUnidentified1E4.m_ID, offsetof(cPlayer, m_tSwapControllerTimer) - offsetof(cPlayer, mUnidentified1E4.m_ID));
}

#undef REGISTER_PLAYER_FIELD

void cPlayer::Unknown12(RunningChecksum* pChecksum)
{
    cCharacter::Unknown12(pChecksum);
    pChecksum->ChecksumData(&mUnidentified1E4.m_v3AIPosition, sizeof(mUnidentified1E4.m_v3AIPosition));
    pChecksum->ChecksumData(&mUnidentified1E4.m_eBallRotationMode, sizeof(mUnidentified1E4.m_eBallRotationMode));
    pChecksum->ChecksumData(&mUnidentified1E4.m_BaseBallOrientation, sizeof(mUnidentified1E4.m_BaseBallOrientation));
    pChecksum->ChecksumData(&mUnidentified1E4.m_tBallPossessionTimer, sizeof(mUnidentified1E4.m_tBallPossessionTimer));
    pChecksum->ChecksumData(&mUnidentified1E4.m_fShotStrengthTime, sizeof(mUnidentified1E4.m_fShotStrengthTime));
    pChecksum->ChecksumData(&mUnidentified1E4.m_tSlideAttackTimer, sizeof(mUnidentified1E4.m_tSlideAttackTimer));
    pChecksum->ChecksumData(&mUnidentified1E4.m_UserControlledTime, sizeof(mUnidentified1E4.m_UserControlledTime));
}

void cPlayer::InitActionPostWhistle()
{
}

extern "C" UnidentifiedVariant_80054AB8* fn_80312360(
    UnidentifiedFuzzyRuntimeBase*, FunctionEntryPoint*, int, void*, void*);
extern "C" UnidentifiedVariant_80054AB8 fn_800996D0(
    UnidentifiedFuzzyRuntimeBase*, const unsigned int&, cPlayer*);
extern "C" UnidentifiedVariant_80054AB8 fn_80099670(
    UnidentifiedFuzzyRuntimeBase*, cPlayer*, const char*);

extern "C" UnidentifiedVariant_80054AB8 fn_80099660(
    UnidentifiedFuzzyRuntimeBase* runtime, const char* name, cPlayer* player)
{
    return fn_80099670(runtime, player, name);
}

extern "C" UnidentifiedVariant_80054AB8 fn_80099670(
    UnidentifiedFuzzyRuntimeBase* runtime, cPlayer* player, const char* name)
{
    unsigned int functionHash = nlStringHash(name);
    return fn_800996D0(runtime, functionHash, player);
}

extern "C" UnidentifiedVariant_80054AB8 fn_800996D0(
    UnidentifiedFuzzyRuntimeBase* runtime, const unsigned int& functionHash, cPlayer* player)
{
    unsigned int localHash = functionHash;
    FunctionEntryPoint* entry = runtime->FindFunctionEntryPoint(localHash);
    return UnidentifiedVariant_80054AB8(fn_80312360(runtime, entry, 1, player, NULL));
}
