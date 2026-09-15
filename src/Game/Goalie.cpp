#include "NL/plat/PlatPadManager.h"

#include "Game/Sys/audio.h"
#include "Game/Goalie.h"
#include "NL/nlString.h"

#include "Game/AI/AiUtil.h"
#include "Game/AI/AIPad.h"
#include "Game/AI/Fielder.h"
#include "Game/AI/FielderActions.h"
#include "Game/AI/FuzzyVariant.h"
#include "Game/AI/FuzzyAIRuntime.h"
#include "Game/AI/FilteredRandom.h"
#include "Game/AI/Scripts/ScriptQuestions.h"
#include "Game/AI/SkillTweaks.h"
#include "Game/Task/FixedUpdateTask.h"
#include "Game/Physics/PhysicsWaluigiWall.h"
#include "Game/AI/GoalieLooseBall.h"
#include "Game/AI/HeadTrack.h"
#include "Game/BasicStadium.h"
#include "Game/Ball.h"
#include "Game/CharacterTemplate.h"
#include "Game/CharacterTweaks.h"
#include "Game/CharacterTriggers.h"
#include "Game/Camera/CameraMan.h"
#include "Game/Drawable/DrawableCharacter.h"
#include "Game/DB/CharacterInfo.h"
#include "Game/DB/GameProgress.h"
#include "Game/DB/StadiumInfo.h"
#include "Game/DebugWriteCache.h"
#include "Game/Effects/EmissionManager.h"
#include "Game/Field.h"
#include "Game/Game.h"
#include "Game/GameInfo.h"
#include "Game/MathHelpers.h"
#include "Game/Net.h"
#include "Game/Physics/PhysicsCharacter.h"
#include "Game/Physics/PhysicsAIBall.h"
#include "Game/Physics/PhysicsFakeBall.h"
#include "Game/Physics/PhysicsPatch.h"
#include "Game/EventDataTypes.h"
#include "Game/PoseAccumulator.h"
#include "Game/Render/ShootToScoreArrow.h"
#include "Game/RumbleActions.h"
#include "Game/SAnim/pnBlender.h"
#include "Game/SAnim/pnFeather.h"
#include "Game/SAnim/pnSAnimController.h"
#include "Game/SAnim/pnSingleAxisBlender.h"
#include "Game/SHierarchy.h"
#include "Game/Task/GameRenderTask.h"
#include "Game/Team.h"
#include "NL/globalpad.h"
#include "NL/nlMain.h"
#include "NL/nlPrint.h"
#include "Game/Weather.h"
#include "unclassified/tu_801A6AAC.h"
#include "Game/NetworkPeer.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/Audio/RegistryPools.h"

#include <math.h>

extern "C" void fn_8005D354(
    cGame* pGame, const GoalieSaveData* pData);
extern "C" void fn_8005D948(
    void* pGame, const GoalieSaveData* pData);
extern "C" void fn_8005E9FC(
    void* pManager, const PlayerAttackData* pData);
extern "C" void fn_8003C5D8(
    cFielder* pFielder, bool bParam, unsigned short aDirection);
extern "C" void fn_8003C6E0(cFielder* pFielder);
extern "C" bool fn_8003877C(cFielder* pFielder);
extern "C" void fn_8001B314(unsigned int nNumTrails);
extern "C" void fn_8005DB7C();
extern "C" void fn_8007F534(Goalie* pGoalie);
class UnidentifiedFuzzyRuntimeBase;
extern "C" cPlayer* fn_80096F54(cPlayer*, bool);
extern "C" UnidentifiedFuzzyRuntimeBase* fn_800A695C(cTeam*);
extern "C" UnidentifiedVariant_80054AB8 fn_80082140(
    UnidentifiedFuzzyRuntimeBase*, const char*, cPlayer*);
extern float lbl_806DC7C8;
extern nlVector4 lbl_8056D3B0;
u8 lbl_806E0D13;
extern cTeam* g_pCurrentlyUpdatingTeam;

static const nlVector3 v3Zero = { 0.0f, 0.0f, 0.0f };

bool Goalie::mbPosGoalieNetCheck;
bool Goalie::mbNegGoalieNetCheck;
u8 Goalie::mbActionDataSetup;
float Goalie::mfGoalieStepDist = -1.0f;
float Goalie::mfGoalieStrafeDist = -1.0f;
float Goalie::mfGoalieRunDist = -1.0f;
float lbl_806DBB14 = 0.8f;
float lbl_806DBB18 = 6.0f;
float lbl_806DBB1C = 0.5f;
float lbl_806DBB20 = 35.0f;
float lbl_806DBB24 = 70.0f;
float lbl_806DBB28 = 13.0f;
float lbl_806DBB2C = 9.5f;
float lbl_806DBB30 = 6.0f;
float lbl_806DBB34 = 0.5f;
float lbl_806DBB38 = 0.6f;
float lbl_806DBB3C = 2.33f;
float lbl_806DBB40 = 3.8f;
float lbl_806DBB44 = 3.5f;
float lbl_806DBB48 = 5.0f;
float lbl_806DBB4C = 5.0f;
float lbl_806DBB50 = 11.0f;
float lbl_806DBB54 = 0.8f;
float lbl_806DBB58 = 8.0f;
float lbl_806DBB5C = 2.5f;
float lbl_806DBB60 = 95.0f;
float lbl_806DBB64 = 60.0f;
float lbl_806DBB68 = 2.2f;
float lbl_806DBB6C = 3.6f;
float lbl_806DBB70 = 0.05f;
float lbl_806DBB74 = 0.3f;
float lbl_806DBB78 = 0.1f;
float lbl_806DBB7C = 8.0f;
float lbl_806DBB80 = 4.0f;
float lbl_806DBB84 = 1.6f;
float lbl_806DBB88 = 6.0f;
float lbl_806DBB8C = 2.0f;
float lbl_806DBB90 = 2.5f;
float lbl_806DBB94 = 10.0f;
float lbl_806DBB98 = 20.0f;
float lbl_806DBB9C = 8.0f;
float lbl_806DBBA0 = 12.0f;
float lbl_806DBBA4 = 0.6f;
float lbl_806DBBA8 = 0.2f;
float lbl_806DBBAC = 40.0f;
float lbl_806DBBB0 = 0.2f;
float lbl_806DBBB4 = 0.5f;
float lbl_806DBBB8 = 90.0f;
float gfRepositionThreshold = 0.15f;
bool gbEnableBallGoalieSweepTest = true;

extern "C" UnidentifiedVariant_80054AB8 fn_80082150(
    UnidentifiedFuzzyRuntimeBase*, cPlayer*, const char*);

inline void Goalie::UnidentifiedResetState()
{
    mGoalieActionState = GOALIEACTION_MOVE;
    mPrevGoalieActionState = GOALIEACTION_MOVE;
    mUrgency = URGENCY_LOW;
    mnSubstate = 1;
    mMoveDirection = GOALIEDIR_IDLE;
    mCrouchType = GOALIECROUCH_SHOT;
    mPursueDekeType = 0;
    mPursueDekeState = 0;
    mfSwitchTime = 1.0f;
    muSaveType = 0xFFFF;
    mfWaitTime = 0.0f;
    mfTimeTilSave = 0.0f;
    mfDelayTime = 0.0f;
    mfWallBlock = 0.0f;
    muWallID = 0;
    mbPlayMiss = false;
    mbShouldMiss = false;
    mbStunEffectActive = false;
    mbDoIntercept = false;
    mbDoNavigate = false;
    mbDoHeadTrack = false;
    mbBallImpacted = false;
    mbNoUserControl = false;
    mbIsPosed = false;
    mbIsDown = false;
    mbPickedUp = false;
    mbRecalcSave = false;
    mbCheckForMegaGoal = false;
    mbMegaUserSave = false;
    mbGrabMonty = false;
    mbTryLobSave = false;
    nlVec3Set(mv3LocalContactPosition, 0.0f, 0.0f, 0.0f);
    nlVec3Set(mv3LocalContactVelocity, 0.0f, 0.0f, 0.0f);
    nlVec3Set(mv3TargetPosition, 0.0f, 0.0f, 0.0f);
    nlVec3Set(mv3TargetVelocity, 0.0f, 0.0f, 0.0f);
    nlVec3Set(mv3NavTarget, 0.0f, 0.0f, 0.0f);
    nlVec3Set(mv3LocalNavTarget, 0.0f, 0.0f, 0.0f);
    maLocalAngle = 0;
    maInitialAngle = 0;
    maSaveAngle = 0;
    mfTargetTime = 0.0f;
    mfTargetDist = 0.0f;
    mfSpeedScale = 1.0f;
    mfBallCharge = 0.0f;
    mfNextBallTime = 0.0f;
    mfMegaAccuracy = 0.0f;
    mfMegaTargetTime = 0.0f;
    muBallChangeCount = 0;
    muBallDeflectCount = 0;
    mnOffplayPending = GOALIE_OFFPLAY_NONE;
    muMegaAnimState = 0;
    muMegaStoreTexID = 0;
    muMegaNextTarget = 0;
    muMegaReadyToSave = 0;
    mBallsLaunched = 0;
    mLowLobAnim = 0;
    mFreezeTimer.UnidentifiedClear();
    mMegaMachine = -1;
    mpPassTarget = 0;
    mpShooter = 0;
    mpTarget = 0;
    mpMonty = 0;
    mpSkillShooter = 0;
    mpSaveData = 0;
    mBlendInfo.fn_80091704();
    mFatigue.Reset();
}

inline void Goalie::InitGoalieActionData()
{
    if (!mbActionDataSetup)
    {
        nlVector3 v3Trans;
        GetJointPositionFuture(&v3Trans, 0x17, -1, 1.0f, true, true, false, true);
        mfGoalieStepDist = fabsf(0.6f * v3Trans.y);
        GetJointPositionFuture(&v3Trans, 0x12, -1, 1.0f, true, true, false, true);
        mfGoalieStrafeDist = fabsf(0.6f * v3Trans.y);
        mfGoalieRunDist = fabsf(1.8f * v3Trans.y);
        mbActionDataSetup = true;
    }
}

Goalie::Goalie(eCharacterClass gcc, const int* pTemplate,
    cSHierarchy* pHierarchy, cAnimInventory* pAnimInventory,
    const CharacterPhysicsData* pPhysicsData, GoalieTweaks* pTweaks,
    AnimRetargetList* pAnimRetargetList, int nIndex)
    : cPlayer(4, gcc, pTemplate, pHierarchy, pAnimInventory, pPhysicsData,
          pTweaks->fPhysCapsuleHeight, pTweaks->fPhysCapsuleHeight,
          pAnimRetargetList, nIndex, GOALIE)
    , m_pTweaks(pTweaks)
{
    UnidentifiedResetState();
    mUnidentified4C8 = 0;
    mpLooseBallInfo = 0;
    mUnidentified528 = true;
    mUnidentified529 = false;
    m_pTweaks->fn_800277A0();
    InitActionMove(false);
    GoalieSave::InitData(this);
    LooseBallAnims::Init(this);
    InitGoalieActionData();
    mBlendInfo.fn_80091704();
    mFatigue.Reset();
    mFatigue.mfRecoverRate = pTweaks->fFatigueRecoverRate;
    m_pHeadTrack->m_bTrackOOI = true;
}

void Goalie::Unknown10(const nlVector3& v3Position, unsigned short aDirection)
{
    cPlayer::Unknown10(v3Position, aDirection);
    m_pPhysicsCharacter->m_CanCollideWithGoalLine = true;
    m_pPhysicsCharacter->m_CanCollideWithWall = true;
    CleanupStun();
    UnidentifiedResetState();
    mbPosGoalieNetCheck = false;
    mbNegGoalieNetCheck = false;
    mpLooseBallInfo = 0;
    fn_80084C3C(false);
}

extern "C" UnidentifiedVariant_80054AB8 fn_80082140(
    UnidentifiedFuzzyRuntimeBase* runtime, const char* name, cPlayer* player)
{
    return fn_80082150(runtime, player, name);
}

cPlayer* Goalie::FindOpenPassTarget()
{
    cPlayer* pPassTarget = 0;
    bool bUnidentified = GetGlobalPad() != 0;
    if (bUnidentified)
    {
        pPassTarget = fn_80096F54(this, false);
    }
    else
    {
        UnidentifiedVariant_80054AB8 vBestPassTarget = fn_80082140(
            fn_800A695C(m_pTeam), "BestPassTarget", this);

        if (vBestPassTarget.UnidentifiedGetFloat(4) >= 0.5f)
        {
            pPassTarget = vBestPassTarget.mData.pPlayer;
        }
    }

    if (pPassTarget != 0 && pPassTarget->m_eClassType == FIELDER
        && ((cFielder*)pPassTarget)->IsFallenDown())
    {
        pPassTarget = 0;
    }

    return pPassTarget;
}

void Goalie::UnidentifiedVirtual1C()
{
    fn_80097648(-1.0f);
    SetAnimState(5, false, 0.0f, false, false);
    m_pCurrentAnimController->SetTime(0.0f);
    InitMovementNone(0.0f, 0.0f);
}

Goalie::~Goalie()
{
    GoalieSave::ClearData();
    LooseBallAnims::Destroy();

    if (mUnidentified4C8 != 0)
    {
        cCameraManager::Remove(*mUnidentified4C8);
        delete mUnidentified4C8;
        mUnidentified4C8 = 0;

        lbl_806DC7C8 = -1.0f;
        DrawableCharacter::RenderAllCharacters();

        if (g_pBall != 0)
        {
            g_pBall->m_bVisible = true;
        }
    }
}

extern "C" float fn_800776B4()
{
    return lbl_806DBB1C;
}

bool Goalie::IsTargetViable(cPlayer* pTarget)
{
    if ((float)fabs(pTarget->mUnidentified024.m_v3Position.x)
            > (float)fabs(static_cast<cPlayer*>(this)->mUnidentified024.m_v3Position.x)
        && fabsf(pTarget->mUnidentified024.m_v3Position.y) < cField::GetPenaltyBoxY())
    {
        return false;
    }

    return true;
}

extern "C" float fn_8007ACB8(Goalie* pGoalie,
    const nlVector3& v3TargetPosition, float fParam1, float fParam2)
{
    cTeam* pOtherTeam = pGoalie->m_pTeam->GetOtherTeam();
    float fParam2Squared = fParam2 * fParam2;
    float fResult = 0.0f;

    for (int i = 0; i < 4; ++i)
    {
        cFielder* pFielder = pOtherTeam->GetFielder(i);
        if (pFielder->IsFallenDown() || fn_8003877C(pFielder))
            continue;

        if (v3TargetPosition.x
                * pFielder->mUnidentified024.m_v3Position.x
            > 0.0f)
        {
            nlVector2 v2Distance;
            v2Distance.x = pFielder->mUnidentified024.m_v3Position.x
                         - v3TargetPosition.x;
            v2Distance.y = pFielder->mUnidentified024.m_v3Position.y
                         - v3TargetPosition.y;
            float fDistanceSquared = nlVec2LengthSquared(v2Distance);
            if (fDistanceSquared < fParam2Squared)
            {
                float fInfluence;
                if (fDistanceSquared <= fParam1)
                {
                    fResult = 1.0f;
                    break;
                }
                else
                {
                    float fDistance = nlSqrt(fDistanceSquared, true);
                    fInfluence = InterpolateRange(
                        1.0f, 0.0f, fParam1, fParam2, fDistance);
                    fResult = 1.0f
                            - (1.0f - fResult) * (1.0f - fInfluence);
                }
            }
        }
    }

    return fResult;
}

bool Goalie::IsInsideNetArea(const nlVector3& v3Target)
{
    float fMargin = m_pTweaks->fSaveIgnoreMargin;
    if ((float)fabsf(v3Target.x) > cField::GetGoalLineX(1U) - 1.0f)
    {
        float fNetWidth = cNet::GetNetWidth();
        double fAbsTargetY = fabs(v3Target.y);
        if ((float)fAbsTargetY < 0.5f * fNetWidth + fMargin
            && v3Target.z < fMargin + cNet::GetNetHeight())
            return true;
    }
    return false;
}

/**
 * Offset/Address/Size: 0x5638 | 0x8007C864 | size: 0xA0
 */
bool Goalie::IsOpponentInSTS()
{
    cFielder* pFielder = g_pBall->GetOwnerFielder();
    if ((pFielder != NULL) && !IsOnSameTeam(pFielder)
        && (pFielder->m_eActionState == ACTION_SHOOT_TO_SCORE)
        && !pFielder->IsInvincible())
    {
        return true;
    }

    return false;
}

bool Goalie::fn_8007D740()
{
    if (GameInfoManager::Instance()->IsInMode4())
    {
        switch (g_pStrikerChallenge->mCurrentChallenge)
        {
        case 0:
        case 1:
        case 2:
        case 3:
            return true;
        default:
            return false;
        }
    }

    return false;
}

bool Goalie::IsPassThreat()
{
    cPlayer* pPassTarget = g_pBall->m_pPassTarget;
    if (pPassTarget != NULL)
    {
        if (!IsOnSameTeam(pPassTarget))
        {
            nlVector2 v2Unidentified;
            v2Unidentified.x = m_pTeam->m_pNet->m_v3NetLocation.x
                - g_pBall->m_v3PassIntercept.x;
            v2Unidentified.y = m_pTeam->m_pNet->m_v3NetLocation.y
                - g_pBall->m_v3PassIntercept.y;

            if (nlGetLengthSquared2D(v2Unidentified.x, v2Unidentified.y)
                < nlGetLengthSquared1D(lbl_806DBB2C))
            {
                mpPassTarget = pPassTarget;
                muBallDeflectCount = g_pBall->m_bBallDeflectCount;
                return true;
            }
        }
    }
    return false;
}

bool Goalie::IsCloseToPlane(const nlVector3& rPos1,
    const nlVector3& rPos2, float fThreshold)
{
    nlVector3 v3Dir;
    nlVector4 plane;

    nlVector3* pBallPos = &g_pBall->m_v3Position;

    nlVector2 v2Unidentified;
    v2Unidentified.x = pBallPos->x - rPos1.x;
    v2Unidentified.y = pBallPos->y - rPos1.y;
    float fUnidentified
        = nlGetLengthSquared2D(v2Unidentified.x, v2Unidentified.y);
    if (fUnidentified > nlGetLengthSquared1D(0.01f))
    {
        v3Dir.x = v2Unidentified.y;
        v3Dir.y = rPos1.x - pBallPos->x;
        v3Dir.z = 0.0f;
    }
    else
    {
        float fUnidentified2
            = nlGetLengthSquared1D(g_pBall->m_v3Velocity.y);
        float fUnidentified3
            = nlGetLengthSquared1D(g_pBall->m_v3Velocity.x);
        if (fUnidentified3 + fUnidentified2
            > nlGetLengthSquared1D(0.01f))
        {
            v3Dir.x = g_pBall->m_v3Velocity.y;
            v3Dir.y = -g_pBall->m_v3Velocity.x;
        }
        else
        {
            nlVector2 v2Unidentified2;
            v2Unidentified2.x = rPos2.x - rPos1.x;
            v2Unidentified2.y = rPos2.y - rPos1.y;
            return nlVec2LengthSquared(v2Unidentified2)
                < nlGetLengthSquared1D(fThreshold);
        }
    }

    MakePerpendicularPlane(*pBallPos, v3Dir, plane, 0.0f);

    float distance = (rPos2.x * plane.x) + (rPos2.y * plane.y)
        + (rPos2.z * plane.z) - plane.w;
    float absDistance = (float)fabsf(distance);

    if (absDistance <= fThreshold)
    {
        return true;
    }
    return false;
}

void Goalie::MakeExertEvent()
{
    GoalieSaveData pSaveData;

    pSaveData.pGoalie = this;
    pSaveData.v3BallVelocity = v3Zero;
    pSaveData.fWowFactor = 0.0f;
    pSaveData.isSTS = 0;

    pSaveData.saveType = g_pBall->m_uGoalType;
    pSaveData.pShooter = g_pBall->m_pShooter;

    if (mpSaveData != 0)
    {
        pSaveData.padding = mpSaveData->muSaveType;
    }
    else
    {
        pSaveData.padding = 3;
    }

    fn_8005D948(g_pGame, &pSaveData);
}

void Goalie::MakeSaveEvent(bool bIsSTS)
{
    GoalieSaveData pSaveData;

    pSaveData.pGoalie = this;
    pSaveData.v3BallVelocity = g_pBall->m_v3Velocity;

    GoalieTweaks* pTweaks = (GoalieTweaks*)m_pTweaks;
    pSaveData.fWowFactor = 1.0f / pTweaks->fShotFatigueMax;

    pSaveData.isSTS = bIsSTS;

    pSaveData.saveType = g_pBall->m_uGoalType;
    pSaveData.pShooter = g_pBall->m_pShooter;

    if (mpSaveData != 0)
    {
        pSaveData.padding = mpSaveData->muSaveType;
        pSaveData.fWowFactor *= mpSaveData->mfFatigueValue;
    }
    else
    {
        pSaveData.padding = 3;
        pSaveData.fWowFactor
            *= ((GoalieTweaks*)m_pTweaks)->fShotFatigueDefault;
    }

    fn_8005D354(g_pGame, &pSaveData);

    if (mUnidentified1E4.m_tFireTimer.m_uPackedTime == 0)
    {
        PlaySound(9, 0xC65200C7, 0, 0);
    }
    else
    {
        PlaySound(9, 0x528D7B6A, 0, 0);
    }

    if (fn_800155A0(g_pBall, 0) >= 4.0f
        && GetStadiumUnknown0x10(GameInfoManager::Instance()->GetStadium()))
    {
        unsigned long uParam = 0x3ACB01E9;
        if (m_pTeam->m_nSide == 0)
        {
            uParam = 0x09999280;
        }
        PlayCrowdReaction(uParam);
    }
}

void Goalie::SetGoalieAction(
    eGoalieActionState newGoalieState, int newSubstate)
{
    CleanGoalieAction();
    mPrevGoalieActionState = mGoalieActionState;
    mGoalieActionState = newGoalieState;
    mnSubstate = newSubstate;
}

void Goalie::SaveBlendCallback(
    unsigned int nParam, cPN_SAnimController* pAnimCtrl)
{
    Goalie* pThis = reinterpret_cast<Goalie*>(nParam & ~3U);
    unsigned int saveDataIndex = nParam & 3U;

    SaveData* pSaveData = pThis->mBlendInfo.mpSaveData[saveDataIndex];
    if (pSaveData == 0)
    {
        return;
    }

    float fTime = pAnimCtrl->m_fTime;
    int milestoneIndex = 0;
    float fMilestonePercent;

    while (milestoneIndex < 4
           && fTime >= (fMilestonePercent = pSaveData->mfMilestonePercent[milestoneIndex]))
    {
        milestoneIndex++;
    }

    pAnimCtrl->m_fPlaybackSpeedScale
        = pThis->mBlendInfo.mfMilestoneScale[saveDataIndex][milestoneIndex];
}

static inline int GetAnimID(SaveBlendInfo& blend, int index)
{
    return blend.mpSaveData[index]->mnAnimID;
}

cPoseNode* Goalie::SetupBlender(bool bPrimary, const float* fStartPercent,
    int nMainAnimID, int nMilestone)
{
    float fBlend;
    int index1;
    cPN_SAnimController* pSaveController1;
    int index2;

    if (bPrimary)
    {
        fBlend = mBlendInfo.mfSaveBlendPrimary;
        index1 = 0;
        index2 = 1;
    }
    else
    {
        fBlend = mBlendInfo.mfSaveBlendSecondary;
        index1 = 2;
        index2 = 3;
    }
    int animID = GetAnimID(mBlendInfo, index1);
    pSaveController1 = NewAnimController(animID, false, false, SaveBlendCallback, index1 + (unsigned int)this);
    pSaveController1->m_fPlaybackSpeedScale
        = mBlendInfo.mfMilestoneScale[index1][nMilestone];
    if (fStartPercent[index1] > 0.0f)
    {
        pSaveController1->SetTime(fStartPercent[index1]);
    }
    cPoseNode* result = pSaveController1;
    if (nMainAnimID == animID)
    {
        m_pCurrentAnimController = pSaveController1;
    }
    else
    {
        pSaveController1->m_bIgnoreTriggers = true;
    }
    if (fBlend >= 0.001f)
    {
        animID = GetAnimID(mBlendInfo, index2);
        cPN_SAnimController* pSaveController2
            = NewAnimController(animID, false, false, SaveBlendCallback, index2 + (unsigned int)this);
        pSaveController2->m_fPlaybackSpeedScale
            = mBlendInfo.mfMilestoneScale[index2][nMilestone];
        if (fStartPercent[index2] > 0.0f)
        {
            pSaveController2->SetTime(fStartPercent[index2]);
        }
        if (nMainAnimID == animID)
        {
            m_pCurrentAnimController = pSaveController2;
        }
        else
        {
            pSaveController2->m_bIgnoreTriggers = true;
        }

        cPN_SingleAxisBlender* pPoseNode
            = new cPN_SingleAxisBlender(2, 0, 0, 0.1f);
        pPoseNode->m_fDesiredWeight = fBlend;
        pPoseNode->m_fSmoothedWeight = fBlend;
        pPoseNode->SetChild(0, pSaveController1);
        pPoseNode->SetChild(1, pSaveController2);
        result = pPoseNode;
    }
    return result;
}

void Goalie::PlayBlendedAnims(
    float fStartTime, float fParam2, int nMilestone)
{
    static float fDefaultStartPercent[] = { 0.4f, 0.7f };

    cPoseNode* pMainNode;
    cPoseNode* pNode1;
    cPoseNode* pNode2;
    float fStartPercent[4];
    int nMainAnimID;
    int milestone;

    if (mUnidentified1E4.m_tFireTimer.m_uPackedTime != 0)
    {
        fn_80097648(0.1f);
    }

    if (mBlendInfo.mfSaveBlendComposite <= 0.001f
        && mBlendInfo.mfSaveBlendPrimary <= 0.001f)
    {
        SetAnimState(mpSaveData->mnAnimID, true, 0.2f, false, false);

        if (nMilestone >= 0 && nMilestone < 2)
        {
            fStartTime = mBlendInfo.mfMilestoneTime[nMilestone];
            if (fStartTime <= 0.0f)
            {
                float* pDefaultStartPercent = fDefaultStartPercent;
                fStartTime = pDefaultStartPercent[nMilestone]
                           * (mpSaveData->mfDuration
                               * mpSaveData->mfMilestonePercent[2]);
            }
        }

        if (fStartTime > 0.0f && fStartTime < mpSaveData->mfDuration)
        {
            cPN_SAnimController* pController = m_pCurrentAnimController;
            float fAnimTime = fStartTime / mpSaveData->mfDuration;
            pController->SetTime(fAnimTime);
        }
    }
    else
    {
        nMainAnimID = mpSaveData->mnAnimID;

        if (nMilestone >= 0)
        {
            milestone = nMilestone;

            if (mBlendInfo.mfMilestoneTime[nMilestone] > 0.0f)
            {
                for (int i = 0; i < 4; i++)
                {
                    SaveData* pData = mBlendInfo.mpSaveData[i];
                    if (pData != 0)
                    {
                        fStartPercent[i]
                            = pData->mfMilestonePercent[nMilestone];
                    }
                }
            }
            else
            {
                float* pDefaultStartPercent = fDefaultStartPercent;
                float fDefaultStart = pDefaultStartPercent[nMilestone];

                for (int i = 0; i < 4; i++)
                {
                    SaveData* pData = mBlendInfo.mpSaveData[i];
                    if (pData != 0)
                    {
                        fStartPercent[i]
                            = fDefaultStart * pData->mfMilestonePercent[2];
                    }
                }
            }
        }
        else if (fStartTime > 0.0f)
        {
            float fPrevMilestone = 0.0f;

            milestone = 0;
            while (milestone < 4
                   && fStartTime >= mBlendInfo.mfMilestoneTime[milestone])
            {
                if (mBlendInfo.mfMilestoneTime[milestone] > 0.0f)
                {
                    fPrevMilestone
                        = mBlendInfo.mfMilestoneTime[milestone];
                }
                milestone++;
            }

            fStartTime = NormalizeVal(fStartTime, fPrevMilestone, mBlendInfo.mfMilestoneTime[milestone]);

            int prevMilestone = milestone - 1;

            for (int i = 0; i < 4; i++)
            {
                SaveData* pData = mBlendInfo.mpSaveData[i];
                if (pData != 0)
                {
                    float fStart = 0.0f;
                    if (milestone > 0)
                    {
                        fStart
                            = pData->mfMilestonePercent[prevMilestone];
                    }

                    fStartPercent[i] = Interpolate(fStart,
                        pData->mfMilestonePercent[milestone],
                        fStartTime);
                }
            }
        }
        else
        {
            fStartPercent[0] = 0.0f;
            fStartPercent[1] = 0.0f;
            fStartPercent[2] = 0.0f;
            fStartPercent[3] = 0.0f;
            milestone = 0;
        }

        pNode1
            = SetupBlender(true, fStartPercent, nMainAnimID, milestone);

        if (mBlendInfo.mfSaveBlendComposite >= 0.001f)
        {
            pNode2 = SetupBlender(
                false, fStartPercent, nMainAnimID, milestone);
            cPN_SingleAxisBlender* pBlend
                = new cPN_SingleAxisBlender(2, 0, 0, 0.1f);

            pBlend->m_fDesiredWeight
                = mBlendInfo.mfSaveBlendComposite;
            pBlend->m_fSmoothedWeight
                = mBlendInfo.mfSaveBlendComposite;
            pBlend->SetChild(0, pNode1);
            pBlend->SetChild(1, pNode2);

            pMainNode = pBlend;
        }
        else
        {
            pMainNode = pNode1;
        }

        cPN_Blender* pBlender
            = new cPN_Blender(m_pAILayer[0], pMainNode, 0.1f);

        m_pAILayer[0] = pBlender;
        SetAnimID(nMainAnimID);
    }

    if (mbShouldMiss)
    {
        InitMovementFromAnim(0, v3Zero, 1.0f, true);
    }
    else
    {
        nlVector3 v3AnimMoveAdjust;
        nlVec3Sub(v3AnimMoveAdjust, mv3LocalContactPosition, mBlendInfo.mv3BlendedSavePos);
        GetWorldPoint(v3AnimMoveAdjust, v3AnimMoveAdjust, v3Zero, mUnidentified024.m_aDesiredFacingDirection);

        float fMaxLengthSq = fParam2 * fParam2;
        float fLengthSq = nlVec3LengthSquared(v3AnimMoveAdjust);
        if (fLengthSq > fMaxLengthSq)
        {
            float fScale = fParam2 * nlRecipSqrt(fLengthSq, true);
            nlVec3Scale(v3AnimMoveAdjust, v3AnimMoveAdjust, fScale);
        }

        InitMovementFromAnim(
            0, v3AnimMoveAdjust, mpSaveData->mfMilestonePercent[2], true);
    }
}

void Goalie::PlayNewAnim(int nAnimID)
{
    if (nAnimID == m_eAnimID)
    {
        cPN_SAnimController* pController = m_pCurrentAnimController;
        bool bSkipSetAnimState = false;

        if (pController->m_ePlayMode == PM_HOLD
            && pController->m_fTime == 1.0f)
        {
            bSkipSetAnimState = true;
        }

        if (!bSkipSetAnimState)
        {
            return;
        }
    }

    SetAnimState(nAnimID, true, 0.2f, false, false);
}

void Goalie::CleanGoalieAction()
{
    switch (mGoalieActionState)
    {
    case GOALIEACTION_MOVE:
        break;

    case GOALIEACTION_SAVE_REPOSITION:
        mbDoNavigate = false;
        break;

    case GOALIEACTION_SAVE:
        if (mpSaveData != 0 && mpSaveData->mnRecoverAnimID < 0)
        {
            mpSaveData = 0;
        }
        muSaveType = 0xFFFF;
        mbShouldMiss = false;
        mbTryLobSave = false;
        mbNoUserControl = false;
        mpPassTarget = 0;
        break;

    case GOALIEACTION_DIVE_RECOVER:
        mpSaveData = 0;
        break;

    case GOALIEACTION_STS_RECOVER:
        mpSaveData = 0;
        CleanupStun();
        break;

    case GOALIEACTION_PURSUE_BALL_POUNCE:
        mbPlayMiss = false;
        break;

    case GOALIEACTION_LOOSEBALL_PICKUP:
        mbPlayMiss = false;
        mbNoUserControl = false;
        mbPickedUp = false;
        break;

    case GOALIEACTION_UNIDENTIFIED_21:
        mbNoUserControl = false;
        break;

    case GOALIEACTION_OFFPLAY:
        mnOffplayPending = GOALIE_OFFPLAY_NONE;
        break;

    case GOALIEACTION_UNIDENTIFIED_25:
        m_pPhysicsCharacter->m_CanCollideWithBall = true;
        break;

    case GOALIEACTION_UNIDENTIFIED_26:
        fn_8007F534(this);
        break;

    case GOALIEACTION_UNIDENTIFIED_27:
        fn_80097648(-1.0f);
        mUnidentified1E4.m_bSkipAnimUpdate = false;
        mUnidentified1E4.m_fSkipTimer = 0.0f;
        mUnidentified1E4.m_bForceFeatherUpdate = false;
        fn_801B93E8(this);
        break;

    case GOALIEACTION_UNIDENTIFIED_28:
        fn_8007F430();
        break;

    case GOALIEACTION_UNIDENTIFIED_29:
        m_pPhysicsCharacter->m_CanCollideWithBall = true;
        break;

    case GOALIEACTION_UNIDENTIFIED_30:
        m_pPhysicsCharacter->m_CanCollideWithBall = true;
        break;

    case GOALIEACTION_UNIDENTIFIED_31:
        fn_8007F44C();
        break;

    case GOALIEACTION_UNIDENTIFIED_33:
        mpShooter = 0;
        break;

    case GOALIEACTION_STS_ATTACK:
        mpShooter = 0;
        break;

    case GOALIEACTION_UNIDENTIFIED_37:
        break;

    default:
        break;
    }
}

int Goalie::ChooseRunAnim(
    short nAngle, const nlVector3& rTargetPos, float fThreshold)
{
    int nCurrentAnimID = m_eAnimID;
    unsigned short nAbsAngle;
    nlVector2 v3Delta;
    v3Delta.x = rTargetPos.x - mUnidentified024.m_v3Position.x;
    v3Delta.y = rTargetPos.y - mUnidentified024.m_v3Position.y;

    if (nlGetLengthSquared2D(v3Delta.x, v3Delta.y)
        < nlGetLengthSquared1D(fThreshold))
    {
        mMoveDirection = GOALIEDIR_IDLE;
        return 5;
    }

    nAbsAngle = (u16)abs_s16(nAngle);

    mMoveDirection = GOALIEDIR_FORWARD;

    if (((nCurrentAnimID == 0x22) || (nCurrentAnimID == 0x23))
        && (m_pCurrentAnimController->m_fTime < 0.92f))
    {
        return nCurrentAnimID;
    }
    if (nAbsAngle <= 0x2AF8 || nCurrentAnimID == 0x24)
    {
        return 0x24;
    }
    if (nAngle > 0)
    {
        return 0x23;
    }

    return 0x22;
}

void Goalie::fn_8007F430()
{
    mUnidentified1E4.m_bSkipAnimUpdate = false;
    mUnidentified1E4.m_fSkipTimer = 0.0f;
    g_pBall->mbBallFrozen = false;
}

void Goalie::fn_8007F44C()
{
    if (mpMonty != 0)
    {
        if (mpMonty->m_eActionState == ACTION_UNKNOWN_34
            && !mpMonty->mUnidentified424)
        {
            if (mbGrabMonty
                && mpMonty->mUnidentified024.m_v3Position.z > -0.25f)
            {
                fn_8003C5D8(mpMonty, true,
                    mUnidentified024.m_aActualFacingDirection + 0x9FF6);
                PlaySound(mUnidentified318, 0x4AE0B399, 0, 0);
            }
            else
            {
                fn_8003C6E0(mpMonty);
            }
        }
        mpMonty = 0;
    }

    if (m_pBall != 0)
    {
        ReleaseBall(0);
    }

    if (!g_pBall->m_bVisible)
    {
        g_pBall->m_bVisible = true;
    }
    mbGrabMonty = false;
}

extern "C" void fn_8007F534(Goalie* pGoalie)
{
    if (pGoalie->mnSubstate == 10)
    {
        return;
    }

    pGoalie->fn_80084CE0();
    pGoalie->mbMegaUserSave = false;
    pGoalie->mMegaMachine = -1;

    if (pGoalie->mnSubstate == 9)
    {
        DrawableCharacter::RenderAllCharacters();
        if (g_pGame->mbCaptainShotToScoreOn)
        {
            fn_8005DB7C();
        }
        SetRenderWorldEffects(1);
        g_pGame->fn_800586C0();
        return;
    }

    DetInput* pGlobalPad = pGoalie->GetGlobalPad();
    if (pGlobalPad != 0)
    {
        cGlobalPad* pPad = ((NetworkPeerChannel*)pGlobalPad->m_pMyUser)->GetLocalChannelPad();
        if (pPad != 0)
        {
            g_pPlatPadManager->SetDPDEnabled(pPad->m_padIndex, false);
        }
    }

    if (pGoalie->mUnidentified4C8 != 0)
    {
        cCameraManager::Remove(*pGoalie->mUnidentified4C8);
        delete pGoalie->mUnidentified4C8;
        pGoalie->mUnidentified4C8 = 0;
    }

    fn_801A6DD8();
    fn_801A7800();
    lbl_806DC7C8 = -1.0f;
    DrawableCharacter::RenderAllCharacters();
    UnFreezeEveryoneButCaptain(0);
    pGoalie->fn_80084C3C(true);
    fn_8001B314(0);
    WorldDarkening::Instance().Fade(100.0f, 0.0f);
    if (g_pGame->mbCaptainShotToScoreOn)
    {
        fn_8005DB7C();
    }
    g_pGame->mpWeatherManager->Resume();
    SetRenderWorldEffects(1);
    g_pGame->fn_800586C0();

    pGoalie->mUnidentified178 = 1.0f;
    pGoalie->SetPosition(pGoalie->mv3NavTarget);

    char effectName[100];
    nlSNPrintf(effectName, sizeof(effectName), "%s_mega_bg",
        pGoalie->mpShooter->mUnidentified11C->mName);
    EffectsGroup* pEffectsGroup
        = EmissionManager::Instance()->GetEffectsGroup(effectName);
    EmissionManager::Instance()->Kill(pEffectsGroup);

    if (BasicStadium::GetCurrentStadium() != 0)
    {
        fn_80278860(BasicStadium::GetCurrentStadium(), 1);
    }
}

void Goalie::CleanupStun()
{
    if (mbStunEffectActive)
    {
        KillDaze(this);
        mbStunEffectActive = false;
    }
}

void Goalie::fn_8007EA90()
{
    bool bUnidentified;
    if (fn_800976C4() && mUnidentified1E4.m_nFeatherAnimID == 0xAA)
    {
        bUnidentified = true;
    }
    else
    {
        bUnidentified = false;
    }

    if (!bUnidentified)
    {
        SetPowerupAnimState(m_nSpine1JointIndex, 0xAA, 0.08f);
        PlaySound(9, 0x528D7B6A, 0, 0);
    }
}

bool Goalie::fn_8007EB10()
{
    if (fn_800976C4() && mUnidentified1E4.m_nFeatherAnimID == 0xAA)
    {
        return true;
    }

    return false;
}

void Goalie::fn_8007EB5C()
{
    EmitDaze(this);
    mbStunEffectActive = true;
}

void Goalie::fn_8007EB90()
{
    ReleaseBall(0);
    const cBall* pBall = g_pBall;
    nlVector3 v3Velocity;
    v3Velocity.y = pBall->m_v3Position.y > 0.0f ? 1.0f : -1.0f;
    float fRandomX = nlRandomf(0.5f);
    v3Velocity.x = pBall->m_v3Position.x < 0.0f ? fRandomX : -fRandomX;
    nlVec3Scale(v3Velocity, 6.0f + nlRandomf(2.0f));
    v3Velocity.z = 8.0f + nlRandomf(2.0f);
    g_pBall->m_tNoPickupTimer.SetSeconds(0.7f);
    g_pBall->SetVelocity(v3Velocity, SPINTYPE_NONE, NULL);
    bool bHumanControlled = GetGlobalPad() != NULL;
    if (bHumanControlled)
    {
        PlayRumbleAction(1, GetGlobalPad());
        SwapController(false);
    }
}

void Goalie::ChooseSwatAnim(int nParam)
{
    fn_80097648(0.1f);
    if (nParam != 0)
    {
        fn_8009750C();
    }
}

void Goalie::fn_8007FE28(int nTeamSide)
{
    Goalie* pGoalie = g_pTeams[nTeamSide]->GetGoalie();
    Goalie* pOtherGoalie = g_pTeams[1 - nTeamSide]->GetGoalie();

    if (g_pGame->m_eGameState != 1)
    {
        pGoalie->InitActionOffplay(GOALIE_OFFPLAY_GOAL_FOR);
        pOtherGoalie->InitActionOffplay(GOALIE_OFFPLAY_GOAL_AGAINST);
    }

    pGoalie->mFatigue.Reset();
    pOtherGoalie->mFatigue.Reset();
}

PhysicsGoalie* Goalie::GetPhysicsGoalie()
{
    return (PhysicsGoalie*)m_pPhysicsCharacter;
}

float Goalie::CalcSaveParameters(float fTimeToContact,
    unsigned int uSaveType, bool bFromTakeoff, bool bFindFailSave)
{
    float fTime = fTimeToContact;

    if (mbShouldMiss)
    {
        fTime += ((GoalieTweaks*)m_pTweaks)->fSaveMissDelay;
        mpSaveData = NULL;
    }
    else
    {
        mpSaveData = GoalieSave::FindBestSave(mBlendInfo,
            mv3LocalContactPosition,
            mv3LocalContactVelocity,
            fTime,
            false,
            uSaveType,
            bFromTakeoff);
    }

    if (mpSaveData != NULL)
    {
        mbPlayMiss = false;
    }
    else
    {
        if (!mbShouldMiss && !bFindFailSave)
        {
            return -1.0f;
        }

        mpSaveData = GoalieSave::FindBestSave(mBlendInfo,
            mv3LocalContactPosition,
            mv3LocalContactVelocity,
            5.0f,
            true,
            0xFFFC,
            false);
        mbPlayMiss = true;
    }

    const float fDT
        = (mBlendInfo.mv3BlendedSavePos.x
              - mv3LocalContactPosition.x)
        / mv3LocalContactVelocity.x;

    fTime += fDT;

    nlVec3ScaleAdd(mv3LocalContactPosition, fDT,
        mv3LocalContactVelocity, mv3LocalContactPosition);

    return fTime;
}

void Goalie::SetDesiredSaveFacing(const nlVector3& v3BallPosition)
{
    if (mUnidentified024.m_v3Position.x > (cField::GetGoalLineX(1U) - 0.1f))
    {
        mUnidentified024.m_aDesiredFacingDirection = 0x8000;
        return;
    }

    if (mUnidentified024.m_v3Position.x < (0.1f - cField::GetGoalLineX(1U)))
    {
        mUnidentified024.m_aDesiredFacingDirection = 0;
        return;
    }

    nlVector3 v3Facing;
    nlVector3 v3G2Ball;
    nlVector3 v3BallVelocity;
    nlVector3 v3G2Post1;
    nlVector3 v3G2Post2;

    nlVec3Set(v3G2Ball,
        v3BallPosition.x - mUnidentified024.m_v3Position.x,
        v3BallPosition.y - mUnidentified024.m_v3Position.y,
        v3BallPosition.z - mUnidentified024.m_v3Position.z);

    float fBallOffMagSq = nlVec3DotProduct(v3G2Ball, v3G2Ball);
    nlVec3Scale(v3G2Ball, nlRecipSqrt(fBallOffMagSq, true));

    cBall* pBall = g_pBall;
    float fBallVelocityMagSq = nlVec3DotProduct(
        pBall->m_v3Velocity, pBall->m_v3Velocity);
    if (fBallVelocityMagSq > 0.01f)
    {
        nlVec3Scale(v3BallVelocity, pBall->m_v3Velocity, -nlRecipSqrt(fBallVelocityMagSq, true));
        nlVecLerp(v3G2Ball, v3G2Ball, v3BallVelocity, 0.5f);
    }

    m_pTeam->m_pNet->GetPostLocation(v3G2Post1, 0, 0.5f);
    m_pTeam->m_pNet->GetPostLocation(v3G2Post2, 1, 0.5f);

    nlVec3Sub(v3G2Post1, v3G2Post1, mUnidentified024.m_v3Position);
    nlVec3Sub(v3G2Post2, v3G2Post2, mUnidentified024.m_v3Position);

    float fLeftDot = nlVec3DotProduct(v3G2Ball, v3G2Post1);
    float fRightDot = nlVec3DotProduct(v3G2Ball, v3G2Post2);

    if ((fLeftDot > 0.0f) || (fRightDot > 0.0f))
    {
        if (fLeftDot > fRightDot)
        {
            nlVec3Set(v3Facing,
                v3G2Post1.y,
                -v3G2Post1.x,
                0.0f);

            if (nlVec3DotProduct(v3Facing, v3G2Post2) > 0.0f)
            {
                nlVec3Scale(v3Facing, -1.0f);
            }
        }
        else
        {
            nlVec3Set(v3Facing,
                v3G2Post2.y,
                -v3G2Post2.x,
                0.0f);

            if (nlVec3DotProduct(v3Facing, v3G2Post1) > 0.0f)
            {
                nlVec3Scale(v3Facing, -1.0f);
            }
        }
    }
    else
    {
        v3Facing = v3G2Ball;
    }

    if (fBallOffMagSq < 1.44f)
    {
        nlVector3 v3BallToGoal;
        nlVec3Sub(v3BallToGoal,
            v3BallPosition,
            m_pTeam->m_pNet->m_v3NetLocation);
        nlVec3Normalize(v3Facing, v3Facing);
        nlVec3Normalize(v3BallToGoal, v3BallToGoal);

        nlVecLerp(v3Facing, v3Facing, v3BallToGoal, 0.5f);
    }

    mUnidentified024.m_aDesiredFacingDirection = nlVector3ToAngle(v3Facing);
}

void Goalie::TrackTarget(
    const nlVector3& v3Target, float fRatio, float fParam3)
{
    nlVector3 v3FutureBallPos;
    nlVector3 v3FuturePos;
    unsigned short aRot;

    GetCurrentAnimFuture(m_nBallJointIndex, mpLooseBallInfo->GetPickupTime(), v3FutureBallPos, v3FuturePos, aRot);

    float fZero = 0.0f;
    float fDeltaY = v3Target.y - v3FutureBallPos.y;
    float fAngleDeltaY = v3Target.y - mUnidentified024.m_v3Position.y;
    float fDeltaX = v3Target.x - v3FutureBallPos.x;
    float fAngleDeltaX = v3Target.x - mUnidentified024.m_v3Position.x;

    nlVector3 v3Velocity;
    v3Velocity.y = fDeltaY;
    v3Velocity.x = fDeltaX;
    v3Velocity.z = fZero;

    float fAngleToTarget = nlATan2f(fAngleDeltaY, fAngleDeltaX);

    unsigned short aAngleToTarget
        = (u16)(s32)(10430.378f * fAngleToTarget);
    float fFutureAngleDeltaX
        = v3FutureBallPos.x - mUnidentified024.m_v3Position.x;
    float fFutureAngleDeltaY
        = v3FutureBallPos.y - mUnidentified024.m_v3Position.y;
    s16 aDiff = (s16)(aAngleToTarget
                      - (u16)(s32)(10430.378f
                                   * nlATan2f(fFutureAngleDeltaY, fFutureAngleDeltaX)));
    s32 iTurn = ((s32)(1024.0f * fRatio) * aDiff) / 1024;
    SetFacingDirection((u16)(iTurn + mUnidentified024.m_aActualFacingDirection), true);

    nlVec3Scale(v3Velocity, fRatio);

    v3Velocity.x = nlMinEquals(
        nlMaxEquals(v3Velocity.x, -fParam3), fParam3);
    v3Velocity.y = nlMinEquals(
        nlMaxEquals(v3Velocity.y, -fParam3), fParam3);

    nlVec3Add(v3FuturePos, v3Velocity, mUnidentified024.m_v3Position);

    SetPosition(v3FuturePos);
}

void Goalie::TacklePlayer(cPlayer* pPlayer)
{
    cFielder* pFielder = static_cast<cFielder*>(pPlayer);
    if (pPlayer != 0 && pPlayer->m_eClassType == FIELDER
        && !pFielder->IsFallenDown() && !pFielder->fn_8003E6FC())
    {
        PlaySound(9, 0x06024E5D, 0, 0);

        bool bHadBall = false;
        if (pPlayer->m_pBall != 0)
        {
            pPlayer->ReleaseBall(false);
            bHadBall = true;
        }

        if (IsOnSameTeam(pFielder))
        {
            if (bHadBall)
            {
                pFielder->EndDesire();
                pFielder->EndAction();
            }
            return;
        }

        pFielder->InitActionSlideAttackReact(this, false);
    }
}

void Goalie::fn_80080BFC(float fDeltaT)
{
    if (mPursueDekeState != 2)
    {
        return;
    }
    if (mpTarget == 0)
    {
        return;
    }
    if (mpTarget->m_eActionState != (eFielderActionState)0x23)
    {
        return;
    }

    mfTargetTime += fDeltaT;
    if (mfTargetTime < lbl_806DBB78)
    {
        int nGoalieJointIndex;
        if (mpLooseBallInfo->mAnimType == LOOSEBALL_ANIM_ATTACK)
        {
            nGoalieJointIndex = m_nRightHandJointIndex;
        }
        else
        {
            nGoalieJointIndex = m_nRightFootJointIndex;
        }

        nlVector3 v3TargetJointPosition = mpTarget->GetJointPosition(
            mpTarget->m_pPoseAccumulator->m_BaseSHierarchy
                ->m_nPelvisNodeIndex);
        nlVector3 v3TargetPosition
            = mpTarget->mUnidentified024.m_v3Position;
        nlVector3 v3GoalieJointPosition
            = GetJointPosition(nGoalieJointIndex);

        float fOffset = lbl_806DBB74
                      + (v3GoalieJointPosition.z - v3TargetJointPosition.z);
        if (fOffset > 0.0f)
        {
            v3TargetPosition.z += fOffset;
            mpTarget->SetPosition(v3TargetPosition);
        }
    }
}

void Goalie::StealBall(cPlayer* pPlayer)
{
    if (pPlayer == 0)
    {
        return;
    }
    if (pPlayer->m_eClassType != FIELDER)
    {
        return;
    }
    if (pPlayer->m_pBall == 0)
    {
        return;
    }

    pPlayer->ReleaseBall(false);

    cFielder* pFielder = static_cast<cFielder*>(pPlayer);
    if (pFielder->fn_8002E060() == FIELDERDESIRE_FINISH_ACTION)
    {
        return;
    }

    pFielder->EndDesire();
    pFielder->EndAction();
}

void Goalie::WhackSTSPlayer(cFielder* pFielder)
{
    if (pFielder == 0)
    {
        return;
    }
    if (pFielder->fn_8003E6FC())
    {
        return;
    }

    pFielder->fn_80047240(
        pFielder, mUnidentified024.m_aActualFacingDirection, 2, true, false);
    PlaySound(pFielder->mUnidentified318, 0x3642C41B, 0, 0);

    mbDoHeadTrack = false;

    nlVector3 v3BallVel;
    const float fBallVelMult = -0.5f;
    nlVec3Scale(v3BallVel, mUnidentified024.m_v3Position, fBallVelMult);

    float yRand = nlRandomf(5.0f);
    if (nlRandom(100) > 50)
    {
        yRand *= -1.0f;
    }
    v3BallVel.y += yRand;

    v3BallVel.z = 4.0f + nlRandomf(2.0f);

    g_pBall->SetVelocity(v3BallVel, SPINTYPE_FORWARD, 0);
    g_pBall->m_tNoPickupTimer.SetSeconds(0.12f);

    PlayerAttackData data;
    data.pAttacker = this;
    data.nAttackerPadID = -1;
    data.pTarget = pFielder;
    data.mUnidentified0C = 2;
    data.mUnidentified10 = false;
    fn_8005E9FC(g_pGame, &data);
}

void Goalie::Unknown12(RunningChecksum* pChecksum)
{
    cPlayer::Unknown12(pChecksum);
    pChecksum->ChecksumData(
        &mGoalieActionState, sizeof(mGoalieActionState));
    pChecksum->ChecksumData(&mUrgency, sizeof(mUrgency));
    pChecksum->ChecksumData(&mnSubstate, sizeof(mnSubstate));
    pChecksum->ChecksumData(&mMoveDirection, sizeof(mMoveDirection));
    pChecksum->ChecksumData(&mCrouchType, sizeof(mCrouchType));
    pChecksum->ChecksumData(&muSaveType, sizeof(muSaveType));
    pChecksum->ChecksumData(&mfWaitTime, sizeof(mfWaitTime));
    pChecksum->ChecksumData(&mfTimeTilSave, sizeof(mfTimeTilSave));
    pChecksum->ChecksumData(&mbPlayMiss, sizeof(mbPlayMiss));
    pChecksum->ChecksumData(&mbShouldMiss, sizeof(mbShouldMiss));
    pChecksum->ChecksumData(
        &mv3LocalContactPosition, sizeof(mv3LocalContactPosition));
    pChecksum->ChecksumData(
        &mv3LocalContactVelocity, sizeof(mv3LocalContactVelocity));
    pChecksum->ChecksumData(
        &mv3TargetPosition, sizeof(mv3TargetPosition));
    pChecksum->ChecksumData(
        &mv3TargetVelocity, sizeof(mv3TargetVelocity));
    pChecksum->ChecksumData(&mfTargetTime, sizeof(mfTargetTime));
    pChecksum->ChecksumData(&mfTargetDist, sizeof(mfTargetDist));
}

extern "C" void GoalieOnGameOver()
{
    cPlayer* pPlayer = g_pBall->m_pOwner;
    g_pBall->m_tNoPickupTimer.SetSeconds(3.0f);

    if (pPlayer != NULL)
    {
        pPlayer->ReleaseBall(false);

        if (pPlayer->m_eClassType == GOALIE)
        {
            Goalie* pGoalie = static_cast<Goalie*>(pPlayer);
            pGoalie->InitActionMove(false);
        }
        else if (pPlayer->m_eClassType == FIELDER)
        {
            cFielder* pFielder = static_cast<cFielder*>(pPlayer);
            pFielder->fn_8002E0FC();
            pFielder->EndAction();
        }
    }

    Goalie* pHomeGoalie = static_cast<Goalie*>(g_pCharacters[8]);
    pHomeGoalie->InitActionMove(false);

    Goalie* pAwayGoalie = static_cast<Goalie*>(g_pCharacters[9]);
    pAwayGoalie->InitActionMove(false);
}

extern "C" UnidentifiedVariant_80054AB8 fn_800821B0(
    UnidentifiedFuzzyRuntimeBase*, const unsigned int&, cPlayer*);

extern "C" UnidentifiedVariant_80054AB8 fn_80082150(
    UnidentifiedFuzzyRuntimeBase* runtime, cPlayer* player, const char* name)
{
    unsigned int functionHash = nlStringHash(name);
    return fn_800821B0(runtime, functionHash, player);
}


void Goalie::Update(float dt)
{
    SetPlayerAudioController(this);
    bool bWallBlock = UnidentifiedWallBlocked();
    if (bWallBlock)
        mfWallBlock -= dt;
    if (!mUnidentified1E4.m_bSkipActionUpdate)
        cPlayer::Update(dt);

    if (mbDoHeadTrack)
    {
        if (m_pBall == 0)
        {
            m_pHeadTrack->m_bTrackOOI = true;
            m_pHeadTrack->m_v3OOI = g_pBall->m_v3Position;
        }
        else if (mGoalieActionState == GOALIEACTION_LOOSEBALL_PICKUP
            && mpLooseBallInfo != 0
            && mpLooseBallInfo->mAnimType == LOOSEBALL_ANIM_KICK
            && mpPassTarget != 0)
        {
            m_pHeadTrack->m_bTrackOOI = true;
            m_pHeadTrack->m_v3OOI = mpPassTarget->mUnidentified024.m_v3Position;
        }
        else
            m_pHeadTrack->m_bTrackOOI = false;
    }
    else
        m_pHeadTrack->m_bTrackOOI = false;

    UpdateActionState(dt);
    if (!mUnidentified1E4.m_bSkipActionUpdate)
        mFatigue.Update(dt);
    if (!mUnidentified1E4.m_bSkipAnimUpdate)
    {
        if (fn_8007EB10() && mUnidentified1E4.m_tFireTimer.m_uPackedTime == 0)
            fn_80097648(0.1f);
        cCharacter::Update(dt);
    }
    else if (mUnidentified1E4.m_bForceFeatherUpdate && fn_800976C4())
    {
        cCharacter::Update(0.0f);
        m_pPowerupLayer->SetChild(1, m_pPowerupLayer->GetChild(1)->Update(dt));
    }
    if (mGoalieActionState != GOALIEACTION_UNIDENTIFIED_26 && mUnidentified529)
        fn_80084CE0();
    if (mUnidentified024.m_v3Position.x < 1.0f && m_pTeam->m_nSide == 1)
    {
        nlVector3 v3Position = mUnidentified024.m_v3Position;
        v3Position.x = 1.0f;
        SetPosition(v3Position);
    }
    else if (mUnidentified024.m_v3Position.x > -1.0f && m_pTeam->m_nSide == 0)
    {
        nlVector3 v3Position = mUnidentified024.m_v3Position;
        v3Position.x = -1.0f;
        SetPosition(v3Position);
    }
    if (!mbIsPosed)
    {
        PostPhysicsUpdate();
        mbIsPosed = true;
    }
}

void Goalie::UpdateActionState(float fDeltaTime)
{
    switch (mGoalieActionState)
    {
    case GOALIEACTION_MOVE:
        ActionMove(fDeltaTime);
        break;
    case GOALIEACTION_MOVE_WB:
        ActionMoveWB(fDeltaTime);
        break;
    case GOALIEACTION_SAVE_SETUP:
        ActionSaveSetup(fDeltaTime);
        break;
    case GOALIEACTION_SAVE_REPOSITION:
        ActionSaveReposition(fDeltaTime);
        break;
    case GOALIEACTION_SAVE:
        ActionSave(fDeltaTime);
        break;
    case GOALIEACTION_MISS_CHIP_SHOT:
        ActionChipShotStumble(fDeltaTime);
        break;
    case GOALIEACTION_DIVE_RECOVER:
        ActionDiveRecover(fDeltaTime);
        break;
    case GOALIEACTION_STS_RECOVER:
        ActionSTSRecover(fDeltaTime);
        break;
    case GOALIEACTION_PASS:
        ActionPass(fDeltaTime);
        break;
    case GOALIEACTION_PASS_INTERCEPT:
        ActionPassIntercept(fDeltaTime);
        break;
    case GOALIEACTION_PRE_CROUCH:
        ActionPreCrouch(fDeltaTime);
        break;
    case GOALIEACTION_PURSUE_BALL_CARRIER:
        ActionPursueBallCarrier(fDeltaTime);
        break;
    case GOALIEACTION_PURSUE_BALL_POUNCE:
        ActionPursueBallPounce(fDeltaTime);
        break;
    case GOALIEACTION_UNIDENTIFIED_13:
        fn_8008A610(fDeltaTime);
        break;
    case GOALIEACTION_LOOSEBALL_SETUP:
        ActionLooseBallSetup(fDeltaTime);
        break;
    case GOALIEACTION_LOOSEBALL_CATCH:
        ActionLooseBallCatch(fDeltaTime);
        break;
    case GOALIEACTION_LOOSEBALL_PICKUP:
        ActionLooseBallPickup(fDeltaTime);
        break;
    case GOALIEACTION_LOOSEBALL_PURSUE_BOUNCING:
        ActionLooseBallPursueBouncing(fDeltaTime);
        break;
    case GOALIEACTION_LOOSEBALL_PURSUE_ROLLING:
        ActionLooseBallPursueRolling(fDeltaTime);
        break;
    case GOALIEACTION_LOOSEBALL_DESPERATE:
        ActionLooseBallDesperate(fDeltaTime);
        break;
    case GOALIEACTION_UNIDENTIFIED_20:
        fn_8008D210(fDeltaTime);
        break;
    case GOALIEACTION_UNIDENTIFIED_21:
        fn_8008DAB4(fDeltaTime);
        break;
    case GOALIEACTION_OFFPLAY:
        ActionOffplay(fDeltaTime);
        break;
    case GOALIEACTION_SNAP_BALL:
        ActionSnapBall(fDeltaTime);
        break;
    case GOALIEACTION_GRAB_BALL:
        ActionGrabBall(fDeltaTime);
        break;
    case GOALIEACTION_UNIDENTIFIED_25:
        fn_8008B718(fDeltaTime);
        break;
    case GOALIEACTION_UNIDENTIFIED_26:
        fn_80084EB0(fDeltaTime);
        break;
    case GOALIEACTION_UNIDENTIFIED_27:
        fn_80083750(fDeltaTime);
        break;
    case GOALIEACTION_UNIDENTIFIED_28:
        fn_800838F8(fDeltaTime);
        break;
    case GOALIEACTION_UNIDENTIFIED_29:
        fn_800891E8(fDeltaTime);
        break;
    case GOALIEACTION_UNIDENTIFIED_30:
        fn_80083DE0(fDeltaTime);
        break;
    case GOALIEACTION_UNIDENTIFIED_31:
        fn_80083960(fDeltaTime);
        break;
    case GOALIEACTION_UNIDENTIFIED_32:
        fn_8008895C(fDeltaTime);
        break;
    case GOALIEACTION_UNIDENTIFIED_33:
        fn_80088A94(fDeltaTime);
        break;
    case GOALIEACTION_STS_ATTACK_SETUP:
        ActionSTSAttackSetup(fDeltaTime);
        break;
    case GOALIEACTION_STS_ATTACK:
        ActionSTSAttack(fDeltaTime);
        break;
    case GOALIEACTION_UNIDENTIFIED_36:
        fn_8008E69C(fDeltaTime);
        break;
    default:
        break;
    }
}

bool Goalie::fn_8007B9A0(const nlVector3& v3Position, float fRange)
{
    if (nlVec3DistanceSquared2D(m_pTeam->m_pNet->m_v3NetLocation, v3Position)
        < nlGetLengthSquared1D(fRange))
        return true;
    return false;
}

void Goalie::fn_8007CB78(bool bBlocked, unsigned int uWallID)
{
    if (bBlocked)
    {
        muWallID = uWallID;
        mfWallBlock = 0.2f;
    }
    else
    {
        muWallID = 0;
        mfWallBlock = 0.0f;
    }
}

bool Goalie::IsWithinPounceRange()
{
    if (fn_8007D740())
        return false;
    if (mUnidentified1E4.m_tFireTimer.m_uPackedTime != 0)
        return false;
    cFielder* pFielder = g_pBall->GetOwnerFielder();
    if (pFielder != 0 && !IsOnSameTeam(pFielder) && pFielder->mbTangible)
    {
        if ((int)pFielder->m_eActionState == ACTION_SHOOT_TO_SCORE
            || (int)pFielder->m_eActionState == ACTION_SHOT)
            return false;
        float range = LooseBallAnims::mTrapBallInfo.mfPickupDistance;
        range += lbl_806DBB54;
        range *= range;
        if (range > nlVec3DistanceSquared2D(mUnidentified024.m_v3Position,
                        pFielder->mUnidentified024.m_v3Position)
            || range > nlVec3DistanceSquared2D(mUnidentified024.m_v3Position,
                           g_pBall->m_v3Position))
            return true;
    }
    return false;
}

bool Goalie::IsOpponentBallCarrierInRange()
{
    if (mUnidentified1E4.m_tFireTimer.m_uPackedTime != 0)
        return false;
    cFielder* pFielder = g_pBall->GetOwnerFielder();
    if (pFielder != 0 && !IsOnSameTeam(pFielder))
    {
        if ((int)pFielder->m_eActionState == ACTION_SHOOT_TO_SCORE
            || (int)pFielder->m_eActionState == ACTION_SHOT)
            return false;
        const nlVector3& v3Position = pFielder->mUnidentified024.m_v3Position;
        if (fn_8007B9A0(v3Position, lbl_806DBB50))
        {
            if (nlVec3DistanceSquared2D(mUnidentified024.m_v3Position,
                    v3Position)
                    < nlGetLengthSquared1D(lbl_806DBB44)
                && !FindSTSMissData(v3Position))
                return true;
        }
    }
    return false;
}

bool Goalie::IsLooseBallTowardNet()
{
    nlVector3 v3BallVel;
    nlVector3 v3Post1;
    nlVector3 v3Post2;

    if (g_pBall->m_pOwner != NULL)
    {
        return false;
    }

    v3BallVel = g_pBall->m_v3Velocity;
    if (nlVec3LengthSquared(v3BallVel) < 0.01f)
    {
        return false;
    }

    m_pTeam->m_pNet->GetPostLocation(v3Post1, 0, 0.0f);
    m_pTeam->m_pNet->GetPostLocation(v3Post2, 1, 0.0f);
    nlVec3Add(v3BallVel, v3BallVel, g_pBall->m_v3Position);
    return IsPointInCone(v3BallVel, g_pBall->m_v3Position, v3Post1, v3Post2);
}

void Goalie::ChooseDesperationAnim(f32 fFudgeDist)
{
    mpLooseBallInfo = LooseBallAnims::GetDesperationInfo(0);

    if (nlVec3DistanceSquared2D(mUnidentified024.m_v3Position, mv3TargetPosition)
        > nlGetLengthSquared1D(mpLooseBallInfo->mfPickupDistance + fFudgeDist))
    {
        mpLooseBallInfo = LooseBallAnims::GetDesperationInfo(1);
        const nlVector3& rPos = mUnidentified024.m_v3Position;

        if (nlVec3DistanceSquared2D(rPos, mv3TargetPosition)
            < nlGetLengthSquared1D(mpLooseBallInfo->mfPickupDistance + fFudgeDist))
        {
            GetLocalPoint(mv3LocalContactPosition, mv3TargetPosition, rPos, mUnidentified024.m_aActualFacingDirection);

            f32 fSlope;
            if (mv3LocalContactPosition.x > 0.01f)
            {
                fSlope = mv3LocalContactPosition.y / mv3LocalContactPosition.x;
            }
            else if (mv3LocalContactPosition.y < 0.0f)
            {
                fSlope = -10.0f;
            }
            else
            {
                fSlope = 10.0f;
            }

            if ((f32)fabs(fSlope) > 1.5f)
            {
                mpLooseBallInfo = (fSlope < 0.0f) ? LooseBallAnims::GetDesperationInfo(2)
                                                  : LooseBallAnims::GetDesperationInfo(3);
            }
        }
    }
}

float Goalie::CalcTimeToPlane(float fPlaneOffset)
{
    SetDesiredSaveFacing(g_pBall->m_v3Position);
    float time = -1.0f;
    unsigned short desiredFacing = mUnidentified024.m_aDesiredFacingDirection;
    const nlVector3& pos = mUnidentified024.m_v3Position;
    if (time < 0.0f)
    {
        nlVector4 plane;
        MakePerpendicularPlane(pos, desiredFacing, plane, fPlaneOffset);
        time = FakeBallWorld::GetPredictedPlaneIntersectTime(plane, mv3TargetPosition, mv3TargetVelocity);
    }
    if (time > 0.0f && fabsf(mv3TargetPosition.x) > cField::GetGoalLineX(1U))
        return -1.0f;
    if (time > 0.0f)
    {
        GetLocalPoint(mv3LocalContactPosition, mv3TargetPosition, pos, desiredFacing);
        GetLocalPoint(mv3LocalContactVelocity, mv3TargetVelocity, v3Zero, desiredFacing);
    }
    return time;
}

inline void Goalie::UnidentifiedStunResponse()
{
    mbDoHeadTrack = false;
    SetGoalieAction(GOALIEACTION_STS_RECOVER, 0);
    mbIsDown = true;
    SetAnimState(0x7C, true, 0.2f, false, false);
    GoalieTweaks* pTweaks = m_pTweaks;
    mfWaitTime = pTweaks->fGoalieStunTimeMin;
    float fRange = pTweaks->fGoalieStunTimeMax.GetValue() - mfWaitTime;
    if (fRange > 0.0f)
        mfWaitTime += nlRandomf(fRange);
    InitMovementFromAnim(0, v3Zero, 1.0f, false);
    fn_8007EB5C();
    bool bHumanControlled = GetGlobalPad() != 0;
    if (bHumanControlled)
    {
        PlayRumbleAction(1, GetGlobalPad());
        SwapController(false);
    }
}

void Goalie::fn_80080EFC()
{
    UnidentifiedStunResponse();
}

bool Goalie::fn_800779D0()
{
    if (!mbStunEffectActive && !g_pBall->mbBallOnFire
        && (g_pBall->UnidentifiedGetGoalType() == 0 || g_pBall->UnidentifiedGetGoalType() == 7
            || g_pBall->UnidentifiedGetGoalType() == 1 || g_pBall->UnidentifiedGetGoalType() == 2))
    {
        cPlayer* pScorer = g_pGame->m_pScorer;
        bool bUnidentified = false;
        if (GameInfoManager::Instance()->IsInMode4()
            && g_pStrikerChallenge->mCaptain == 7 && m_pTeam->m_nSide == 0)
            bUnidentified = true;
        if (bUnidentified
            || (pScorer != 0 && !pScorer->IsOnSameTeam(this)
                && pScorer->m_eClassType == FIELDER))
        {
            float fSpeedSquared = nlVec3LengthSquared(g_pBall->m_v3Velocity);
            float fLimitSquared = nlGetLengthSquared1D(lbl_806DBB20);
            if (bUnidentified
                || (!g_pBall->UnidentifiedState7Shot()
                        && g_pBall->m_tLightningTimer.m_uPackedTime != 0
                        && pScorer == g_pBall->m_pShooter
                        && fSpeedSquared > fLimitSquared
                        && mFatigue.GetEnergyLevel() < lbl_806DBB24))
            {
                if (mpSaveData != 0
                    && (fSpeedSquared > fLimitSquared || (mpSaveData->muSaveType & 0xA)))
                {
                    fn_8008ED44(false);
                    return true;
                }
                UnidentifiedStunResponse();
                return true;
            }
        }
    }
    return false;
}

float Goalie::CheckForDelflectAwayFromNet()
{
    if (muBallDeflectCount != g_pBall->m_bBallDeflectCount)
    {
        nlVector3 v3TargetPosition;
        nlVector4 plane;
        nlVector3 localVelocity;
        float netX = m_pTeam->m_pNet->m_v3NetLocation.x;
        if (netX < 0.0f)
        {
            plane.y = 0.0f;
            plane.x = 1.0f;
            plane.z = 0.0f;
            plane.w = 0.2f + netX;
        }
        else
        {
            plane.y = 0.0f;
            plane.x = -1.0f;
            plane.z = 0.0f;
            plane.w = 0.2f + -netX;
        }
        float result = FakeBallWorld::GetPredictedPlaneIntersectTime(
            plane, v3TargetPosition, localVelocity);
        if (result <= 0.0f || !IsInsideNetArea(v3TargetPosition))
        {
            InitActionMove(false);
            result = -1.0f;
        }
        else
            g_pBall->m_v3ShotTarget = v3TargetPosition;
        return result;
    }
    return 0.0f;
}

bool Goalie::CheckForLooseBallShotInProgress()
{
    if (g_pBall->m_pOwner == 0)
    {
        const nlVector3& v3BallPos = g_pBall->m_v3Position;
        float looseBallShotDistance = m_pTweaks->fLooseBallShotDistance;
        if (nlVec3DistanceSquared2D(v3BallPos, m_pTeam->m_pNet->m_v3NetLocation)
            < nlGetLengthSquared1D(looseBallShotDistance))
        {
            cTeam* pOtherTeam = m_pTeam->GetOtherTeam();
            cFielder* pFielder = 0;
            float fClosestDistSq = 0.0f;
            for (int i = 0; i < 4; ++i)
            {
                cFielder* pOtherFielder = pOtherTeam->GetFielder(i);
                if (pOtherFielder->m_eActionState == ACTION_LOOSE_BALL_SHOT)
                {
                    float fDistSq = nlVec3DistanceSquared2D(v3BallPos,
                        pOtherFielder->mUnidentified024.m_v3Position);
                    if (pFielder == 0 || fDistSq < fClosestDistSq)
                    {
                        pFielder = pOtherFielder;
                        fClosestDistSq = fDistSq;
                    }
                }
            }
            if (FindSTSMissData(v3BallPos))
                return false;
            mpShooter = pFielder;
            if (pFielder != 0)
            {
                const nlVector3& v3GoaliePos = mUnidentified024.m_v3Position;
                if (nlVec3DistanceSquared2D(v3BallPos, v3GoaliePos) > fClosestDistSq)
                {
                    if (IsCloseToPlane(mv3TargetPosition, v3GoaliePos, 1.5f))
                    {
                        InitActionPreCrouch(GOALIECROUCH_LOOSEBALL);
                        return true;
                    }
                    mUrgency = URGENCY_HIGH;
                }
            }
        }
    }
    return false;
}

bool Goalie::fn_8007C73C()
{
    if (mUnidentified1E4.m_tFireTimer.m_uPackedTime != 0)
        return false;
    cFielder* pFielder = g_pBall->GetOwnerFielder();
    if (pFielder != 0 && (int)pFielder->mUnidentified024.m_eCharacterClass == 0x12
        && !pFielder->fn_8003E6FC()
        && (int)pFielder->m_eActionState == 0x20
        && pFielder->m_pCurrentAnimController->m_fTime < 0.55f)
    {
        float fDistSq = nlVec3DistanceSquared2D(pFielder->mUnidentified024.m_v3Position,
            mUnidentified024.m_v3Position);
        if (fDistSq < nlGetLengthSquared1D(lbl_806DBB18))
        {
            nlVector3 v3ToGoalie;
            nlVec3Sub(v3ToGoalie, mUnidentified024.m_v3Position,
                pFielder->mUnidentified024.m_v3Position);
            if (nlVec3DotProduct(pFielder->mUnidentified024.m_v3Velocity, v3ToGoalie) > 0.0f
                || fDistSq < 2.25f)
            {
                mpMonty = pFielder;
                return true;
            }
        }
    }
    return false;
}

inline bool Goalie::UnidentifiedOpponentShooting()
{
    cFielder* pShooter = g_pBall->GetOwnerFielder();
    if (pShooter != NULL && !IsOnSameTeam(pShooter)
        && ((int)pShooter->m_eActionState == ACTION_SHOOT_TO_SCORE
            || (int)pShooter->m_eActionState == ACTION_SHOT))
        return true;
    return false;
}

float Goalie::IsSoloBreakaway()
{
    float fScore = 0.0f;
    cFielder* pFldr = g_pBall->GetOwnerFielder();
    if (pFldr != NULL && !IsOnSameTeam(pFldr))
    {
        if (UnidentifiedOpponentShooting())
            return fScore;
        float fDirection = pFldr->m_pTeam->m_pNet->m_v3NetLocation.x < 0.0f ? 1.0f : -1.0f;
        fScore = InterpolateRangeClamped(0.4f, 1.0f, -2.0f, 8.0f,
            fDirection * pFldr->mUnidentified024.m_v3Position.x);
        if (fScore > 0.8f)
        {
            cTeam* pTeam = pFldr->m_pTeam;
            for (int i = 0; i < 4; ++i)
            {
                cFielder* pBuddy = pTeam->GetFielder(i);
                if (pBuddy == pFldr)
                    continue;
                fScore *= InterpolateRangeClamped(1.0f, 0.4f, -2.0f, 8.0f,
                    fDirection * pBuddy->mUnidentified024.m_v3Position.x);
            }
        }
    }
    return fScore;
}



extern "C" bool fn_80016768(cBall* pBall);

bool Goalie::PreCollideWithBallCallback(const dContact& contact)
{
    if (g_pBall->meBallState == 8 && !g_pBall->m_bVisible)
        return false;
    switch (mGoalieActionState)
    {
    case GOALIEACTION_LOOSEBALL_PURSUE_ROLLING:
        InitActionLooseBallPickup(0.3f, true);
        return false;
    case GOALIEACTION_LOOSEBALL_PICKUP:
        if (InitiatePickup())
            return false;
        break;
    case GOALIEACTION_UNIDENTIFIED_30:
        return false;
    case GOALIEACTION_UNIDENTIFIED_25:
        if (mpSkillShooter != NULL
            && (int)mpSkillShooter->mUnidentified024.m_eCharacterClass == 0xC)
            return false;
        break;
    case GOALIEACTION_UNIDENTIFIED_31:
        if (mpMonty != NULL && mbGrabMonty)
            return false;
        break;
    case GOALIEACTION_SAVE_SETUP:
    case GOALIEACTION_SAVE_REPOSITION:
    case GOALIEACTION_SAVE:
        if (!mbBallImpacted && fn_80016768(g_pBall))
        {
            mbBallImpacted = true;
            fn_800908F8();
            fn_80090958(false);
            return false;
        }
        break;
    default:
        break;
    }
    return true;
}

bool Goalie::FindSTSMissData(const nlVector3& rPos)
{
    cFielder* pCaptain = m_pTeam->GetOtherTeam()->GetCaptain();
    cFielder* pWaluigi;
    if ((int)pCaptain->mUnidentified024.m_eCharacterClass == 6)
        pWaluigi = pCaptain;
    else
    {
        pCaptain = m_pTeam->GetCaptain();
        if ((int)pCaptain->mUnidentified024.m_eCharacterClass == 6)
            pWaluigi = pCaptain;
        else
            return false;
    }
    if (pWaluigi != NULL)
    {
        const nlVector3& v3GoaliePos = GetPosition();
        PhysicsWaluigiWall* pWall = NULL;
        if (muWallID != 0)
        {
            pWall = pWaluigi->mUnidentified3F8.mUnidentified08->FindWall(muWallID);
            if (pWall == NULL)
                muWallID = 0;
        }
        unsigned int i = 0;
        while (i < 20)
        {
            if (i != 0 || pWall == NULL)
            {
                pWall = pWaluigi->mUnidentified3F8.mUnidentified08->GetWall(i);
                ++i;
            }
            if (pWall != NULL)
            {
                if (v3GoaliePos.x * pWall->GetPosition().x > 0.0f
                    && nlVec3DistanceSquared2D(rPos, v3GoaliePos) > 0.01f)
                {
                    nlVector3 v3Normal;
                    nlVec3Set(v3Normal, rPos.y - v3GoaliePos.y,
                        v3GoaliePos.x - rPos.x, 0.0f);
                    nlVector4 plane;
                    const nlVector3& v3End1 = pWall->GetStartPoint();
                    const nlVector3& v3End2 = pWall->GetEndPoint();
                    MakePerpendicularPlane(v3GoaliePos, v3Normal, plane, 0.0f);
                    float fSide1 = nlPlaneSide(v3End1, plane);
                    float fSide2 = nlPlaneSide(v3End2, plane);
                    if (fSide1 * fSide2 < 0.0f)
                    {
                        nlVec3Set(v3Normal, v3End1.y - v3End2.y,
                            v3End2.x - v3End1.x, 0.0f);
                        MakePerpendicularPlane(v3End1, v3Normal, plane, 0.0f);
                        fSide1 = nlPlaneSide(v3GoaliePos, plane);
                        fSide2 = nlPlaneSide(rPos, plane);
                        if (fSide1 * fSide2 < 0.0f)
                        {
                            fn_8007CB78(true, pWall->GetID());
                            return true;
                        }
                    }
                }
                pWall = NULL;
            }
        }
    }
    return false;
}

extern "C" float fn_800DEB04(cFielder* pFielder);

bool Goalie::CheckForSTSAttack()
{
    if (fn_8007D740())
        return false;
    if (IsOpponentInSTS())
    {
        cFielder* pOppFielder = g_pBall->GetOwnerFielder();
        float fAnimScale = pOppFielder->m_pCurrentAnimController->m_pSAnim->GetDuration();
        float fCurrentAnimTime = fAnimScale * pOppFielder->m_pCurrentAnimController->get_fTime();
        float fTriggerTime = 0.5f * fAnimScale;
        const LooseBallInfo* pInfo = &LooseBallAnims::mAttackSTSInfo;
        float fPickupDuration = pInfo->mfPickupTime * pInfo->mfAnimDuration;
        if (fCurrentAnimTime + fPickupDuration < fTriggerTime)
        {
            const nlVector3& v3OwnerPos = pOppFielder->mUnidentified024.m_v3Position;
            if (FindSTSMissData(v3OwnerPos))
                return false;
            float ownerDistSq = nlVec3DistanceSquared2D(v3OwnerPos, mUnidentified024.m_v3Position);
            float fCloseDistSq = nlGetLengthSquared1D(pInfo->mfPickupDistance + lbl_806DBB68);
            float fMaxDistSq = nlGetLengthSquared1D(pInfo->mfPickupDistance + lbl_806DBB6C);
            bool bInNetZone = IsInsideNetArea(v3OwnerPos);
            nlVector3 v3GoalPos = m_pTeam->m_pNet->m_v3NetLocation;
            float halfWidth = 0.5f * cNet::GetNetWidth();
            float clampedY = nlMaxEquals(v3OwnerPos.y, -halfWidth);
            clampedY = nlMinEquals(clampedY, halfWidth);
            v3GoalPos.y = clampedY;
            float distSqFielder = nlVec3DistanceSquared2D(v3GoalPos, v3OwnerPos);
            float distSqGoalie = nlVec3DistanceSquared2D(v3GoalPos, mUnidentified024.m_v3Position);
            static FilteredRandomChance randgenSTS;
            if (bInNetZone || distSqFielder < distSqGoalie
                || ownerDistSq <= fCloseDistSq
                || (ownerDistSq <= fMaxDistSq && randgenSTS.genrand(0.01f * lbl_806DBB70)))
            {
                float fEndTime = 0.25f * fAnimScale;
                float fStartTime = fEndTime - fCurrentAnimTime;
                float fPickupDuration2 = pInfo->mfPickupTime * pInfo->mfAnimDuration;
                InitActionSTSAttackSetup(fStartTime - fPickupDuration2);
                return true;
            }
        }
    }
    else if (g_pBall->GetOwnerFielder() != NULL)
    {
        cFielder* pFielder = g_pBall->GetOwnerFielder();
        if (IsOnSameTeam(pFielder) || !pFielder->mbTangible || pFielder->IsInvincible())
            return false;
        float fDifficulty = Difficult(m_pTeam);
        float fLower = InterpolateRangeClamped(0.05f, 0.25f, 1.0f, 0.2f, fDifficulty);
        float fUpper = (int)pFielder->mUnidentified024.m_eCharacterClass == 0xD ? 1.0f : 0.7f;
        float fShotMeter = fn_800DEB04(pFielder);
        if ((fShotMeter > fLower && fShotMeter < fUpper)
            || ((int)pFielder->mUnidentified024.m_eCharacterClass == 0xD
                && (int)pFielder->m_eActionState == 0x20))
        {
            if (nlRandomf(lbl_806DBB5C) < fDifficulty)
            {
                const nlVector3& v3OwnerPos = pFielder->mUnidentified024.m_v3Position;
                if (fn_8007B9A0(v3OwnerPos, lbl_806DBB50))
                {
                    float fRange = InterpolateRangeClamped(lbl_806DBB4C, lbl_806DBB48,
                        1.0f, 0.2f, fDifficulty);
                    if (nlVec3DistanceSquared2D(mUnidentified024.m_v3Position, v3OwnerPos)
                        < nlGetLengthSquared1D(fRange))
                    {
                        if (FindSTSMissData(v3OwnerPos))
                            return false;
                        InitActionSTSAttack();
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

bool Goalie::CanInterceptPass()
{
    GoalieTweaks* pTweaks = m_pTweaks;
    float fInterceptRangeSq = nlGetLengthSquared1D(pTweaks->fInterceptSaveTolerance);
    SkillTweaks* pSkillTweaks = fn_800A636C(g_pCurrentlyUpdatingTeam);
    bool bDisableIntercept = pSkillTweaks->fGoalieCanInterceptPass < 0.5f;
    if (bDisableIntercept)
        return false;
    if (mpPassTarget != NULL && !IsOnSameTeam(mpPassTarget)
        && muBallDeflectCount == g_pBall->m_bBallDeflectCount)
    {
        if (g_pBall->m_tPassTargetTimer.GetSeconds() > 5.0f)
            return false;
        const nlVector3& goaliePosition = mUnidentified024.m_v3Position;
        cBall* pBall = g_pBall;
        mpSaveData = NULL;
        unsigned short saveAngle;
        unsigned int uSaveType;
        if (nlVec3DistanceSquared2D(pBall->m_v3Position, goaliePosition)
            < nlVec3DistanceSquared2D(pBall->m_v3Position, mpPassTarget->mUnidentified024.m_v3Position))
        {
            nlVector3 v3PassNorm;
            nlVec3Scale(v3PassNorm, g_pBall->m_v3Velocity, -1.0f);
            v3PassNorm.z = 0.0f;
            nlVector4 v4Plane;
            MakePerpendicularPlane(goaliePosition, v3PassNorm, v4Plane, lbl_806DBB1C);
            mfTimeTilSave = FakeBallWorld::GetPredictedPlaneIntersectTime(v4Plane,
                mv3TargetPosition, mv3TargetVelocity);
            saveAngle = nlVector3ToAngle(v3PassNorm);
            uSaveType = 0xFFFF;
            if (mUnidentified1E4.m_tFireTimer.m_uPackedTime != 0)
                uSaveType = 0xFFFC;
            GetLocalPoint(mv3LocalContactPosition, mv3TargetPosition, mUnidentified024.m_v3Position, saveAngle);
            GetLocalPoint(mv3LocalContactVelocity, mv3TargetVelocity, v3Zero, saveAngle);
            mpSaveData = GoalieSave::FindBestSave(mBlendInfo, mv3LocalContactPosition,
                mv3LocalContactVelocity, mfTimeTilSave, false, uSaveType, false);
            if (mpSaveData != NULL
                && CalculateDistanceSquared(mBlendInfo.mv3BlendedSavePos, mv3LocalContactPosition) > fInterceptRangeSq)
                mpSaveData = NULL;
        }
        if (mpSaveData == NULL)
        {
            float fLeadTime = 2.0f * FixedUpdateTask::GetPhysicsUpdateTick();
            mfTimeTilSave = g_pBall->m_tPassTargetTimer.GetSeconds() - fLeadTime;
            if (mfTimeTilSave > 0.05f)
            {
                FakeBallWorld::GetPredictedBallPosition(mfTimeTilSave,
                    mv3TargetPosition, mv3TargetVelocity);
                nlVector3 v3Normal;
                float dx = mv3TargetPosition.x - mUnidentified024.m_v3Position.x;
                float dy = mUnidentified024.m_v3Position.y - mv3TargetPosition.y;
                nlVec3Set(v3Normal, dy, dx, 0.0f);
                if (nlVec3DotProduct(v3Normal, g_pBall->m_v3Velocity) > 0.0f)
                    nlVec3Scale(v3Normal, -1.0f);
                saveAngle = nlVector3ToAngle(v3Normal);
                uSaveType = 0xFFFF;
                if (mUnidentified1E4.m_tFireTimer.m_uPackedTime != 0)
                    uSaveType = 0xFFFC;
                GetLocalPoint(mv3LocalContactPosition, mv3TargetPosition, mUnidentified024.m_v3Position, saveAngle);
                GetLocalPoint(mv3LocalContactVelocity, mv3TargetVelocity, v3Zero, saveAngle);
                mpSaveData = GoalieSave::FindBestSave(mBlendInfo, mv3LocalContactPosition,
                    mv3LocalContactVelocity, mfTimeTilSave, false, uSaveType, false);
                if (mpSaveData != NULL
                    && CalculateDistanceSquared(mBlendInfo.mv3BlendedSavePos, mv3LocalContactPosition) > fInterceptRangeSq)
                    mpSaveData = NULL;
            }
        }
        if (mpSaveData != NULL)
        {
            float milestone2 = mBlendInfo.mfMilestoneTime[2];
            if (milestone2 <= mfTimeTilSave)
            {
                mBlendInfo.mfStartTime = 0.0f;
                mfWaitTime = mfTimeTilSave - milestone2;
            }
            else
            {
                mBlendInfo.mfStartTime = nlMinEquals(milestone2 - mfTimeTilSave,
                    mBlendInfo.mfMilestoneTime[1]);
                mfWaitTime = 0.0f;
            }
            mUnidentified024.m_aDesiredFacingDirection = saveAngle;
            return true;
        }
    }
    return false;
}

extern "C" float fn_800DF888(cTeam* pTeam);

bool Goalie::IsLooseBallClose(float fDistFromBox)
{
    if (mUnidentified1E4.m_tNoPickupTimer.m_uPackedTime != 0)
        return false;
    const nlVector3& v3BallPos = g_pBall->m_v3Position;
    const nlVector3& v3NetPos = m_pTeam->m_pNet->m_v3NetLocation;
    if (v3NetPos.x * v3BallPos.x < 0.0f)
        return false;
    if (!fn_8007B9A0(v3BallPos, lbl_806DBB28))
        return false;
    bool bBallIsLoose = true;
    bool bPass = false;
    if ((g_pBall->meBallState == 5 || g_pBall->meBallState == 3)
        && g_pBall->m_pPassTarget != NULL)
        bPass = true;
    if (bPass)
    {
        cBall* pBall = g_pBall;
        cPlayer* pPassTarget = pBall->m_pPassTarget;
        if (!IsLooseBallTowardNet()
            || nlVec3DistanceSquared2D(mUnidentified024.m_v3Position, pBall->m_v3PassIntercept)
                > nlVec3DistanceSquared2D(pPassTarget->mUnidentified024.m_v3Position, pBall->m_v3PassIntercept))
            bBallIsLoose = false;
    }
    if (g_pBall->m_pOwner == NULL && bBallIsLoose)
    {
        if (FindSTSMissData(v3BallPos))
            return false;
        float goalLineX = cField::GetGoalLineX(1U);
        float penaltyY = cField::GetPenaltyBoxY();
        float absBallX = (float)fabs(v3BallPos.x);
        float absBallY = (float)fabs(v3BallPos.y);
        if (absBallX > goalLineX - 2.0f && absBallY < penaltyY)
            return true;
        cTeam* pOtherTeam = m_pTeam->GetOtherTeam();
        cFielder* pOtherInterceptor = pOtherTeam->mpBestBallInterceptor;
        cFielder* pInterceptor = m_pTeam->mpBestBallInterceptor;
        bool bInterceptorDown = pInterceptor->IsFallenDown() || fn_8003877C(pInterceptor);
        bool bOtherInterceptorDown = pOtherInterceptor->IsFallenDown() || fn_8003877C(pOtherInterceptor);
        if (bInterceptorDown && bOtherInterceptorDown)
            return true;
        float fBallInTime = m_pTeam->mfBallInTimes[pInterceptor->mUnidentified1E4.m_ID];
        float fOtherBallInTime = pOtherTeam->mfBallInTimes[pOtherInterceptor->mUnidentified1E4.m_ID];
        if (bInterceptorDown || fOtherBallInTime < 0.08f + fBallInTime)
        {
            float fGoalieTime = nlSqrt(CalculateDistanceSquared(mUnidentified024.m_v3Position,
                pOtherTeam->UnidentifiedInterceptPosition(pOtherInterceptor->mUnidentified1E4.m_ID)), true) / 8.0f;
            if (fOtherBallInTime < 0.08f + fGoalieTime)
                return false;
        }
        else if ((absBallX < goalLineX - 3.0f || absBallY > penaltyY)
            && fn_800DF888(m_pTeam) > lbl_806DBB34)
            return false;
        nlVector3 v3ProjectedPosition;
        nlVec3ScaleAdd(v3ProjectedPosition, 0.3f, g_pBall->m_v3Velocity, v3BallPos);
        float netSideSign = m_pTeam->m_pNet->m_fDirection;
        if (v3ProjectedPosition.x * netSideSign > cField::GetGoalLineX(1U))
            return true;
        if (fn_8007B9A0(v3ProjectedPosition, lbl_806DBB28))
            return true;
    }
    return false;
}

void Goalie::ExecutePounce(cPlayer* pPlayer, bool bCheckHitDistance)
{
    cFielder* pFldr = static_cast<cFielder*>(pPlayer);
    bool bDoHit = !pFldr->IsFallenDown() && !pFldr->IsInvincible();
    if (bDoHit && bCheckHitDistance
        && (int)pFldr->mUnidentified024.m_eCharacterClass != 1)
    {
        float fPlayerRadius;
        float fGoalieRadius;
        m_pPhysicsCharacter->GetRadius(&fGoalieRadius);
        pFldr->m_pPhysicsCharacter->GetRadius(&fPlayerRadius);
        float fMinHitDistance = 0.5f + (fGoalieRadius + fPlayerRadius);
        bDoHit = nlVec3DistanceSquared2D(mUnidentified024.m_v3Position,
                     pFldr->mUnidentified024.m_v3Position)
            < nlGetLengthSquared1D(fMinHitDistance);
    }
    if ((int)pFldr->m_eActionState == ACTION_SHOOT_TO_SCORE
        || (int)pFldr->m_eActionState == ACTION_SHOT)
        bDoHit = true;
    bool bGetBall = false;
    if (pPlayer->m_pBall != NULL && g_pBall->m_v3Position.z < 1.0f)
        bGetBall = true;
    if (bDoHit)
    {
        TacklePlayer(pPlayer);
        if (bGetBall && pPlayer->m_pBall != NULL)
            StealBall(pPlayer);
    }
    else
        StealBall(pPlayer);
    if (bGetBall)
    {
        PickupBall(g_pBall);
        mbPickedUp = true;
        EmitGoalieCatch(this, "goalie_catch", false);
        if (mGoalieActionState != GOALIEACTION_PURSUE_BALL_POUNCE)
        {
            SetGoalieAction(GOALIEACTION_PURSUE_BALL_POUNCE, 0);
            mbIsDown = true;
            PlayNewAnim(0x31);
            InitMovementFromAnimSeek(m_pTweaks->fRunningDirectionSeekSpeed,
                m_pTweaks->fRunningDirectionSeekFalloff);
        }
    }
}

unsigned int Goalie::FindDumpDirection(unsigned short aDesired, bool bConstrain)
{
    if (GameInfoManager::Instance()->IsInMode4())
    {
        switch (g_pStrikerChallenge->mCurrentChallenge)
        {
        case 0:
        case 1:
        case 2:
        case 3:
        {
            nlVector3 v3Local;
            nlVec3Set(v3Local, (float)fabs(mUnidentified024.m_v3Position.x), 0.0f, 0.0f);
            nlVector3 v3World;
            nlVector3 v3Side1;
            nlVec3Set(v3Side1, 0.0f, cField::GetSidelineY(1U), 0.0f);
            nlVector3 v3Side2;
            nlVec3Set(v3Side2, 0.0f, -cField::GetSidelineY(1U), 0.0f);
            GetWorldPoint(v3World, v3Local, mUnidentified024.m_v3Position, aDesired);
            if (nlVec3DistanceSquared2D(v3World, v3Side1)
                < nlVec3DistanceSquared2D(v3World, v3Side2))
            {
                float dx = v3Side1.x - mUnidentified024.m_v3Position.x;
                float dy = v3Side1.y - mUnidentified024.m_v3Position.y;
                return RadToAng16(nlATan2f(dy, dx));
            }
            float dx = v3Side2.x - mUnidentified024.m_v3Position.x;
            float dy = v3Side2.y - mUnidentified024.m_v3Position.y;
            return RadToAng16(nlATan2f(dy, dx));
        }
        }
    }
    unsigned int aCur = mUnidentified024.m_aActualFacingDirection;
    if (bConstrain)
    {
        short aDiff = (short)(aDesired - aCur);
        short aAbsDiff = aDiff < 0 ? -aDiff : aDiff;
        if ((unsigned short)aAbsDiff > 0x1554)
        {
            if (aDiff > 0)
                aCur += 0x1554;
            else
                aCur -= 0x1554;
        }
        else
            aCur = aDesired;
    }
    if (mUnidentified024.m_v3Position.x < 0.0f)
        aCur = (unsigned short)(aCur + 0x8000);
    if ((unsigned short)aCur < 0x5550)
        aCur = 0x5550;
    else if ((unsigned short)aCur > 0xAAB0)
        aCur = (unsigned short)-0x5550;
    if (mUnidentified024.m_v3Position.x < 0.0f)
        aCur = (unsigned short)(aCur + 0x8000);
    return aCur;
}

bool Goalie::ShouldReposition()
{
    if (mfWaitTime < 0.07f && lbl_806E0D13)
        return false;
    mUrgency = URGENCY_HIGH;
    bool bDesiredDirSet = false;
    if (!mbShouldMiss && 0.2f + mBlendInfo.mv3BlendedSavePos.z < mv3LocalContactPosition.z)
    {
        cBall* pBall = g_pBall;
        float fDistance = 0.5f;
        float fHeight = 1.5f;
        if (mbTryLobSave)
        {
            fDistance = lbl_806DBBA8;
            fHeight = lbl_8056D3B0.z;
        }
        nlVector3 v3ContactVel;
        float fTargetHeight;
        float fDropTime = FakeBallWorld::GetPredictedHeightLimitTime(fHeight, 0.04f,
            mv3NavTarget, v3ContactVel, fTargetHeight, true);
        nlVector3 v3BallVel = g_pBall->m_v3Velocity;
        float fBallSpeed = nlSqrt(v3BallVel.GetLengthSq2D(), true);
        if (fBallSpeed > 0.1f)
        {
            v3BallVel.z = 0.0f;
            nlVec3ScaleAdd(mv3NavTarget, lbl_806DBBA4 / fBallSpeed,
                v3BallVel, mv3NavTarget);
        }
        float fGoalGapDist = cField::GetGoalLineX(1U) - 0.2f;
        float fBoxGapDist = 0.5f + cField::GetPenaltyBoxX(1U);
        float fTargetX;
        bool bCalcIntersect = false;
        float navAbsX = (float)fabs(mv3NavTarget.x);
        if (navAbsX > fGoalGapDist)
        {
            fTargetX = mUnidentified024.m_v3Position.x > 0.0f ? fGoalGapDist : -fGoalGapDist;
            bCalcIntersect = true;
        }
        else if (navAbsX < fBoxGapDist)
        {
            FakeBallWorld::GetPredictedHeightLimitTime(fHeight, 0.25f + fDropTime,
                mv3NavTarget, v3ContactVel, fTargetHeight, true);
            if ((float)fabs(mv3NavTarget.x) < fBoxGapDist)
                fTargetX = mUnidentified024.m_v3Position.x > 0.0f ? fBoxGapDist : -fBoxGapDist;
            bCalcIntersect = true;
        }
        if (bCalcIntersect)
        {
            if ((float)fabs(v3ContactVel.x) > 0.5f)
            {
                float ballX = pBall->m_v3Position.x;
                mv3NavTarget.y = pBall->m_v3Position.y + (fTargetX - ballX)
                        * (mv3NavTarget.y - pBall->m_v3Position.y) / (mv3NavTarget.x - ballX);
            }
            mv3NavTarget.x = fTargetX;
        }
        nlVector2 v2Ball;
        nlVector2 v2Target;
        nlVec2Set(v2Target, mv3NavTarget.x - mUnidentified024.m_v3Position.x,
            mv3NavTarget.y - mUnidentified024.m_v3Position.y);
        nlVec2Set(v2Ball, pBall->m_v3Position.x - mUnidentified024.m_v3Position.x,
            pBall->m_v3Position.y - mUnidentified024.m_v3Position.y);
        mUnidentified024.m_aDesiredFacingDirection = RadToAng16(nlATan2f(v2Ball.y, v2Ball.x));
        if (v2Target.x * v2Target.x + v2Target.y * v2Target.y > nlGetLengthSquared1D(fDistance))
        {
            if (mbTryLobSave || mfWaitTime > 0.5f)
                mUrgency = URGENCY_LOW;
            else
                mUrgency = URGENCY_MED;
            return true;
        }
        bDesiredDirSet = true;
    }
    if ((float)fabs(mBlendInfo.mv3BlendedSavePos.y) > gfRepositionThreshold)
    {
        if (!bDesiredDirSet)
        {
            cBall* pBall = g_pBall;
            float dx = pBall->m_v3Position.x - mUnidentified024.m_v3Position.x;
            float dy = pBall->m_v3Position.y - mUnidentified024.m_v3Position.y;
            mUnidentified024.m_aDesiredFacingDirection = RadToAng16(nlATan2f(dy, dx));
        }
        GetWorldPoint(mv3NavTarget, mBlendInfo.mv3BlendedSavePos,
            mUnidentified024.m_v3Position, mUnidentified024.m_aDesiredFacingDirection);
        mv3NavTarget.z = 0.0f;
        return true;
    }
    return false;
}

inline void Goalie::CheckForBallOnHead()
{
    if (mUnidentified1E4.m_tFireTimer.m_uPackedTime == 0 && !g_pGame->mUnidentified020
        && g_pGame->GetGameState() != 3 && mnOffplayPending == GOALIE_OFFPLAY_NONE
        && g_pBall->m_tNoPickupTimer.m_uPackedTime == 0
        && g_pBall->m_pOwner == NULL && g_pBall->m_pPassTarget == NULL
        && g_pBall->m_v3Position.z > 0.8f)
    {
        InitActionSnapBall();
    }
}

extern "C" void fn_80015B38(cBall* pBall, bool bParam);
extern "C" float fn_800156A8(cBall* pBall);

bool Goalie::InitiatePickup()
{
    if (mpLooseBallInfo->mAnimType == LOOSEBALL_ANIM_KICK)
    {
        if (mbPickedUp)
            return false;
        if (m_pBall == NULL)
        {
            mbNoUserControl = true;
            cFielder* pFldr = g_pBall->GetOwnerFielder();
            if (pFldr != NULL)
            {
                if (IsOnSameTeam(pFldr))
                {
                    InitActionMove(false);
                    return false;
                }
                TacklePlayer(pFldr);
                StealBall(pFldr);
            }
            else if (g_pBall->m_tNoPickupTimer.m_uPackedTime != 0)
                return false;
            nlVector3 v3Pos = g_pBall->m_v3Position;
            PickupBall(g_pBall);
            m_pPhysicsCharacter->m_CanCollideWithGoalLine = true;
            m_pPhysicsCharacter->m_CanCollideWithWall = true;
            mbPickedUp = true;
            g_pBall->SetPosition(v3Pos);
            if (mUnidentified1E4.m_tFireTimer.m_uPackedTime != 0 && m_eAnimID != 3 && m_eAnimID != 2)
            {
                fn_8007EB90();
                InitActionMove(false);
                return false;
            }
            mpPassTarget = FindOpenPassTarget();
            if (mpPassTarget != NULL)
                mbDoHeadTrack = true;
            else
                mbDoHeadTrack = false;
            fn_80015B38(g_pBall, false);
            return true;
        }
    }
    else if (mfWaitTime <= 0.0f && g_pBall->m_tNoPickupTimer.m_uPackedTime == 0)
    {
        mfWaitTime = 0.1f;
        SetNoPickUpTime(mfWaitTime);
        nlVector3 v3BallVel = g_pBall->m_v3Velocity;
        float fSpeedSq = v3BallVel.x * v3BallVel.x + v3BallVel.y * v3BallVel.y + v3BallVel.z * v3BallVel.z;
        if (fSpeedSq > 64.0f)
        {
            nlVec3Scale(v3BallVel, 0.5f);
            g_pBall->SetVelocity(v3BallVel, SPINTYPE_NONE, NULL);
        }
        mbDoHeadTrack = false;
        fn_80015B38(g_pBall, false);
        return true;
    }
    return false;
}

void Goalie::InitiatePanicGrab(cPlayer* pPlayer)
{
    if (pPlayer != NULL)
    {
        if (IsOnSameTeam(pPlayer))
            return;
        cFielder* pFielder = static_cast<cFielder*>(pPlayer);
        if (!pFielder->IsFallenDown() && !pFielder->IsInvincible())
            TacklePlayer(pPlayer);
    }
    int nAnimID = mpLooseBallInfo->mnAnimID;
    if (nAnimID != m_eAnimID)
    {
        PlayNewAnim(nAnimID);
        m_pCurrentAnimController->SetTime(mpLooseBallInfo->GetPickupTime() * 0.5f);
        InitMovementFromAnim(0, v3Zero, 1.0f, false);
    }
    if (m_pBall == NULL)
    {
        StealBall(g_pBall->m_pOwner);
        PickupBall(g_pBall);
        mbPickedUp = true;
        EmitGoalieCatch(this, "goalie_catch", false);
    }
}

bool Goalie::fn_8007BC40()
{
    if (g_pBall->GetOwnerFielder() != NULL
        && !g_pBall->GetOwnerFielder()->IsOnSameTeam(this))
    {
        float fChance = InterpolateClamped(lbl_806DBB60, lbl_806DBB64, fn_800156A8(g_pBall));
        if (nlRandomf(100.0f) < fChance)
        {
            cFielder* pFielder = g_pBall->GetOwnerFielder();
            bool bUnidentifiedSecond = pFielder->UnidentifiedClass17Or2Or6();
            float fDistance = bUnidentifiedSecond ? lbl_806DBB40 : lbl_806DBB3C;
            if (!pFielder->IsFallenDown() && !fn_8003877C(pFielder)
                && nlVec3DistanceSquared2D(g_pBall->m_v3Position, mUnidentified024.m_v3Position)
                    < nlGetLengthSquared1D(fDistance))
            {
                if (FindSTSMissData(pFielder->mUnidentified024.m_v3Position))
                    return false;
                if ((int)pFielder->m_eActionState == 1)
                {
                    float fThreshold = fn_8007BEEC(pFielder);
                    float fCurrentTime = pFielder->m_pCurrentAnimController->m_fTime;
                    int nType = 0;
                    if ((int)pFielder->mUnidentified024.m_eCharacterClass == 3
                        || (int)pFielder->mUnidentified024.m_eCharacterClass == 1
                        || (int)pFielder->mUnidentified024.m_eCharacterClass == 0xB
                        || (int)pFielder->mUnidentified024.m_eCharacterClass == 0xC)
                        nType = 5;
                    else if ((int)pFielder->mUnidentified024.m_eCharacterClass == 0xD
                        || (int)pFielder->mUnidentified024.m_eCharacterClass == 9
                        || (int)pFielder->mUnidentified024.m_eCharacterClass == 7)
                    {
                        if (fCurrentTime < fThreshold)
                            nType = 2;
                    }
                    else if ((int)pFielder->mUnidentified024.m_eCharacterClass == 2
                        || (int)pFielder->mUnidentified024.m_eCharacterClass == 6
                        || (int)pFielder->mUnidentified024.m_eCharacterClass == 0x11)
                    {
                        float fFrame = pFielder->m_pCurrentAnimController->m_fTime
                            * pFielder->m_pCurrentAnimController->m_pSAnim->m_nNumKeys;
                        if (fFrame >= lbl_806DBB58)
                            nType = 4;
                        else
                            nType = 3;
                    }
                    else if ((int)pFielder->mUnidentified024.m_eCharacterClass == 0xF
                        || (int)pFielder->mUnidentified024.m_eCharacterClass == 5
                        || (int)pFielder->mUnidentified024.m_eCharacterClass == 0xA
                        || (int)pFielder->mUnidentified024.m_eCharacterClass == 8)
                    {
                        if (fCurrentTime < fThreshold)
                            nType = 1;
                    }
                    else if ((int)pFielder->mUnidentified024.m_eCharacterClass == 0x10)
                        nType = 6;
                    else if ((int)pFielder->mUnidentified024.m_eCharacterClass == 0x12)
                    {
                        if (fCurrentTime < fThreshold)
                            nType = 7;
                        else
                            return false;
                    }
                    fn_8008BBB0(pFielder, nType);
                    return true;
                }
            }
        }
    }
    return false;
}

float Goalie::fn_8007BEEC(cFielder* pTarget)
{
    float fTime;
    switch (pTarget->mUnidentified024.m_eCharacterClass)
    {
    case (eCharacterClass)8: fTime = 0.9f; break;
    case (eCharacterClass)5: fTime = 0.67f; break;
    case (eCharacterClass)7: fTime = 0.4f; break;
    case (eCharacterClass)10: fTime = 0.66f; break;
    case (eCharacterClass)9: fTime = 0.37f; break;
    case (eCharacterClass)15: fTime = 0.63f; break;
    case (eCharacterClass)13: fTime = 0.73f; break;
    case (eCharacterClass)18: fTime = 0.47f; break;
    case (eCharacterClass)16: fTime = 0.0f; break;
    case (eCharacterClass)2:
    case (eCharacterClass)6:
    case (eCharacterClass)17: fTime = 0.0f; break;
    case (eCharacterClass)0:
    case (eCharacterClass)1:
    case (eCharacterClass)3:
    case (eCharacterClass)4:
    case (eCharacterClass)11:
    case (eCharacterClass)12:
    case (eCharacterClass)14:
    case (eCharacterClass)19:
    default: fTime = 1.0f; break;
    }
    return fTime;
}

extern "C" void fn_80097358(cPlayer* pPlayer, float fParam);

void Goalie::fn_80099074(const UnidentifiedEventData24* pData)
{
    PhysicsPatch* pPatch = pData->mUnidentified10;
    int type = pPatch->m_Type;
    if (type == 1 || type == 8 || type == 9)
    {
        GoalieTweaks* pTweaks = m_pTweaks;
        float fTime = Interpolate(pTweaks->mUnidentified2A8, pTweaks->mUnidentified2B8,
            fn_800156A8(g_pBall));
        fn_80097358(this, fTime);
        bool bPlayAnimation = false;
        bool bDropBall = false;
        switch (mGoalieActionState)
        {
        case GOALIEACTION_MOVE:
        case GOALIEACTION_MOVE_WB:
        case GOALIEACTION_SAVE_REPOSITION:
        case GOALIEACTION_PRE_CROUCH:
        case GOALIEACTION_PURSUE_BALL_CARRIER:
        case GOALIEACTION_LOOSEBALL_SETUP:
        case GOALIEACTION_LOOSEBALL_PURSUE_BOUNCING:
        case GOALIEACTION_LOOSEBALL_PURSUE_ROLLING:
        case GOALIEACTION_SNAP_BALL:
        case GOALIEACTION_GRAB_BALL:
            bPlayAnimation = true;
            bDropBall = true;
            break;
        }
        if (bPlayAnimation)
            fn_8007EA90();
        if (bDropBall && m_pBall != NULL)
        {
            fn_8007EB90();
            SetNoPickUpTime(0.4f);
            mbDoHeadTrack = false;
        }
    }
    else if (type == 6)
    {
        cFielder* pFielder = static_cast<cFielder*>(pPatch->m_pOwner);
        if (!pFielder->IsInvincible() && !IsOnSameTeam(pFielder))
            pFielder->fn_80047240(this,
                pFielder->mUnidentified024.m_aActualFacingDirection + 0x8000, 0, false, false);
    }
}


void Goalie::CollideWithBallCallback(cBall* pBall)
{
    cPlayer::CollideWithBallCallback(pBall);
    if (g_pGame->mUnidentified020 || g_pGame->GetGameState() == 3
        || mnOffplayPending != GOALIE_OFFPLAY_NONE || UnidentifiedOffplayState())
    {
        mbBallImpacted = true;
        return;
    }

    if (pBall->m_pOwner != this)
    {
        if (!mbBallImpacted)
        {
            bool bPlayAnimation = false;
            bool bSkip = false;
            const bool bLightningShot = pBall->UnidentifiedState8Shot();
            if (bLightningShot && mpSkillShooter == NULL)
            {
                switch (mGoalieActionState)
                {
                case GOALIEACTION_MISS_CHIP_SHOT:
                case GOALIEACTION_DIVE_RECOVER:
                case GOALIEACTION_PASS:
                case GOALIEACTION_PASS_INTERCEPT:
                case GOALIEACTION_PURSUE_BALL_POUNCE:
                case GOALIEACTION_UNIDENTIFIED_13:
                case GOALIEACTION_LOOSEBALL_DESPERATE:
                case GOALIEACTION_UNIDENTIFIED_21:
                case GOALIEACTION_SNAP_BALL:
                case GOALIEACTION_GRAB_BALL:
                case GOALIEACTION_UNIDENTIFIED_27:
                case GOALIEACTION_UNIDENTIFIED_28:
                case GOALIEACTION_UNIDENTIFIED_29:
                case GOALIEACTION_UNIDENTIFIED_30:
                case GOALIEACTION_UNIDENTIFIED_31:
                case GOALIEACTION_UNIDENTIFIED_32:
                case GOALIEACTION_UNIDENTIFIED_33:
                case GOALIEACTION_STS_ATTACK_SETUP:
                case GOALIEACTION_STS_ATTACK:
                case GOALIEACTION_UNIDENTIFIED_36:
                    fn_800908F8();
                    break;
                case GOALIEACTION_MOVE:
                case GOALIEACTION_SAVE_SETUP:
                case GOALIEACTION_SAVE_REPOSITION:
                case GOALIEACTION_STS_RECOVER:
                case GOALIEACTION_PRE_CROUCH:
                case GOALIEACTION_PURSUE_BALL_CARRIER:
                case GOALIEACTION_LOOSEBALL_SETUP:
                case GOALIEACTION_LOOSEBALL_CATCH:
                case GOALIEACTION_LOOSEBALL_PICKUP:
                case GOALIEACTION_LOOSEBALL_PURSUE_BOUNCING:
                case GOALIEACTION_LOOSEBALL_PURSUE_ROLLING:
                case GOALIEACTION_UNIDENTIFIED_20:
                    fn_800908F8();
                    bPlayAnimation = true;
                    break;
                case GOALIEACTION_MOVE_WB:
                case GOALIEACTION_SAVE:
                case GOALIEACTION_UNIDENTIFIED_25:
                case GOALIEACTION_UNIDENTIFIED_26:
                case GOALIEACTION_UNIDENTIFIED_37:
                    bSkip = true;
                    break;
                }
            }
            if (!bSkip)
            {
                if (mpSkillShooter != NULL)
                {
                    mfBallCharge = fn_800155A0(pBall, 0);
                    fn_80090958(false);
                }
                else if (pBall->mbBallOnFire)
                {
                    fn_80097358(this, m_pTweaks->mUnidentified2A8);
                    if (bPlayAnimation)
                        fn_8007EA90();
                }
            }
        }

        switch (mGoalieActionState)
        {
        case GOALIEACTION_MOVE:
        case GOALIEACTION_LOOSEBALL_SETUP:
        case GOALIEACTION_LOOSEBALL_PURSUE_BOUNCING:
            CheckForBallOnHead();
            break;
        case GOALIEACTION_LOOSEBALL_PURSUE_ROLLING:
            InitActionLooseBallPickup(0.3f, true);
            break;
        case GOALIEACTION_LOOSEBALL_PICKUP:
            InitiatePickup();
            break;
        case GOALIEACTION_LOOSEBALL_CATCH:
            if (pBall->m_pOwner == NULL)
            {
                PlaySound(9, 0xC65200C7, NULL, NULL);
                PickupBall(pBall);
                m_pPhysicsCharacter->m_CanCollideWithGoalLine = true;
                m_pPhysicsCharacter->m_CanCollideWithWall = true;
                mbPickedUp = true;
                EmitGoalieCatch(this, "goalie_catch", false);
                if (mUnidentified1E4.m_tFireTimer.m_uPackedTime != 0)
                {
                    fn_8007EB90();
                    InitActionMove(false);
                }
            }
            break;
        case GOALIEACTION_LOOSEBALL_DESPERATE:
            InitiatePanicGrab(pBall->m_pOwner);
            break;
        case GOALIEACTION_UNIDENTIFIED_20:
            InitActionMove(false);
            CheckForBallOnHead();
            break;
        case GOALIEACTION_UNIDENTIFIED_21:
        {
            if (mbBallImpacted)
                return;
            mbBallImpacted = true;
            if (mpSaveData->muSaveType & 0x80003)
            {
                TacklePlayer(pBall->GetOwnerFielder());
                StealBall(g_pBall->GetOwnerFielder());
                MakeSaveEvent(false);
                PickupBall(pBall);
                m_pPhysicsCharacter->m_CanCollideWithGoalLine = true;
                m_pPhysicsCharacter->m_CanCollideWithWall = true;
                EmitGoalieCatch(this, "goalie_catch", false);
                break;
            }
            if (pBall->UnidentifiedState7Shot())
            {
                GoalieSaveData data;
                data.saveType = pBall->m_uGoalType;
                data.pShooter = pBall->m_pShooter;
                data.pGoalie = this;
                g_pGame->mUnidentified49C.mEvent19.Deliver(&data);
            }
            EmitGoalieCatch(this, "goalie_deflect", false);
            float fDistSquared = nlGetLengthSquared1D(lbl_806DBBB4);
            if (CalculateDistanceSquared(pBall->m_v3Position, GetJointPosition(m_nLeftHandJointIndex)) < fDistSquared
                || CalculateDistanceSquared(pBall->m_v3Position, GetJointPosition(m_nRightHandJointIndex)) < fDistSquared)
            {
                mbNoUserControl = true;
                PickupBall(pBall);
                m_pPhysicsCharacter->m_CanCollideWithGoalLine = true;
                m_pPhysicsCharacter->m_CanCollideWithWall = true;
                mbPickedUp = true;
            }
            else
                fn_8008DEF4(1.0f);
            break;
        }
        case GOALIEACTION_MISS_CHIP_SHOT:
            if (m_eAnimID != 0x7D && m_pCurrentAnimController->m_fTime < 0.5555556f)
            {
                SetAnimState(0x7D, true, 0.2f, false, false);
                InitMovementFromAnim(0, v3Zero, 1.0f, false);
                EmitGoalieCatch(this, "goalie_deflect", false);
            }
            break;
        case GOALIEACTION_SAVE:
        {
            if (mbBallImpacted)
                return;
            mbBallImpacted = true;
            if (mpSaveData != NULL && !(mpSaveData->muSaveType & 0x80003))
            {
                GoalieSaveData data;
                data.saveType = pBall->m_uGoalType;
                data.pShooter = pBall->m_pShooter;
                data.pGoalie = this;
                g_pGame->mUnidentified49C.mEvent19.Deliver(&data);
            }
            if (mpSkillShooter != NULL && mpSaveData != NULL)
            {
                mfBallCharge = fn_800155A0(pBall, 0);
                if (fn_80090958(false))
                    break;
            }
            else if (pBall->mbBallOnFire)
                fn_80097358(this, m_pTweaks->mUnidentified2A8);
            if (mpSaveData != NULL && (mpSaveData->muSaveType & 0x80003))
            {
                float fNetWidth = cNet::GetNetWidth();
                if ((float)fabsf(g_pBall->m_v3Position.y) < 0.5f * fNetWidth
                    || (float)fabsf(g_pBall->m_v3Position.x) < cField::GetGoalLineX(1U))
                {
                    TacklePlayer(pBall->GetOwnerFielder());
                    StealBall(g_pBall->GetOwnerFielder());
                    MakeSaveEvent(false);
                    PickupBall(pBall);
                    EmitGoalieCatch(this, "goalie_catch", false);
                }
                m_pPhysicsCharacter->m_CanCollideWithGoalLine = true;
                m_pPhysicsCharacter->m_CanCollideWithWall = true;
                break;
            }
            if (!mbShouldMiss)
            {
                MakeSaveEvent(false);
                TacklePlayer(pBall->GetOwnerFielder());
                StealBall(g_pBall->GetOwnerFielder());
            }
            EmitGoalieCatch(this, "goalie_deflect", false);
            if (pBall->m_tNoPickupTimer.m_uPackedTime == 0)
                pBall->m_tNoPickupTimer.SetSeconds(0.08f);
            if (mpSaveData != NULL && mpSaveData->muSaveType == 4)
            {
                float fNetWidth = cNet::GetNetWidth();
                double fAbsBallY = __fabs(pBall->m_v3Position.y);
                if ((float)fAbsBallY < 0.5f * fNetWidth
                    || (float)fabsf(pBall->m_v3Position.x) < cField::GetGoalLineX(1U))
                {
                    float fDistSquared = nlGetLengthSquared1D(lbl_806DBBB4);
                    if (CalculateDistanceSquared(pBall->m_v3Position, GetJointPosition(m_nLeftHandJointIndex)) < fDistSquared
                        || CalculateDistanceSquared(pBall->m_v3Position, GetJointPosition(m_nRightHandJointIndex)) < fDistSquared)
                    {
                        mbNoUserControl = true;
                        PickupBall(pBall);
                        m_pPhysicsCharacter->m_CanCollideWithGoalLine = true;
                        m_pPhysicsCharacter->m_CanCollideWithWall = true;
                        mbPickedUp = true;
                        mBallsLaunched = 0;
                        return;
                    }
                    else
                        fn_8008DEF4(1.0f);
                }
            }
            else if (pBall->m_tLightningTimer.m_uPackedTime == 0 && mpSaveData != NULL
                && !(mpSaveData->muSaveType & 0x80003))
            {
                nlVector3 v3Forward;
                nlVector3 v3NewVel;
                nlVector3 v3AngularVelocity;
                nlVec3Set(v3Forward, m_m4WorldMatrix.m11, m_m4WorldMatrix.m12, m_m4WorldMatrix.m13);
                nlVector3 v3BallRelative;
                nlVec3Sub(v3BallRelative, pBall->m_v3Position, mUnidentified024.m_v3Position);
                if (nlVec3DotProduct(v3BallRelative, v3Forward) > 0.0f)
                {
                    float fForwardScale = 1.0f + nlRandomf(1.0f);
                    float fVelScale = 0.75f + nlRandomf(0.75f);
                    nlVec3Scale(v3Forward, fForwardScale);
                    nlVec3ScaleAdd(v3NewVel, fVelScale, mUnidentified024.m_v3Velocity, v3Forward);
                    v3NewVel.z = 3.0f + nlRandomf(2.0f);
                    pBall->m_pPhysicsBall->GetAngularVelocity(&v3AngularVelocity);
                    nlVec3Scale(v3AngularVelocity, lbl_806DBB38);
                    pBall->SetVelocity(v3NewVel, SPINTYPE_PARAMETER, &v3AngularVelocity);
                }
            }
            fn_800779D0();
            break;
        }
        case GOALIEACTION_PURSUE_BALL_CARRIER:
        case GOALIEACTION_PURSUE_BALL_POUNCE:
        {
            cFielder* pFielder = pBall->GetOwnerFielder();
            if (pFielder != NULL && !IsOnSameTeam(pFielder))
                ExecutePounce(pFielder, true);
            break;
        }
        case GOALIEACTION_STS_RECOVER:
        case GOALIEACTION_UNIDENTIFIED_25:
        case GOALIEACTION_UNIDENTIFIED_27:
        case GOALIEACTION_UNIDENTIFIED_28:
        case GOALIEACTION_UNIDENTIFIED_29:
        case GOALIEACTION_UNIDENTIFIED_30:
        case GOALIEACTION_UNIDENTIFIED_32:
            if (pBall->m_pOwner == NULL)
                fn_8008DEF4(0.5f);
            mbBallImpacted = true;
            break;
        }
    }
    switch (pBall->meBallState)
    {
    case 1:
    case 3:
    case 5:
    case 6:
    case 7:
    case 8:
        fn_80015B38(pBall, false);
        break;
    }
}

extern "C" void fn_801BAF0C(cPlayer* pPlayer);
extern "C" void fn_8005E604(void* pManager, const PlayerAttackData* pData);

void Goalie::fn_80080638(cFielder* pFielder, bool bParam)
{
    if (pFielder != NULL && !pFielder->IsFallenDown() && !pFielder->fn_8003E6FC())
    {
        PlaySound(9, 0x06024E5D, NULL, NULL);
        bool bReleased = false;
        if (IsOnSameTeam(pFielder))
        {
            if (!bParam)
            {
                if (pFielder->m_pBall != NULL)
                {
                    pFielder->ReleaseBall(0);
                    bReleased = true;
                }
                pFielder->InitActionSlideAttackReact(this, false);
                mbDoHeadTrack = false;
            }
        }
        else
        {
            nlVector3 v3Velocity;
            nlVec3Set(v3Velocity, m_m4WorldMatrix.m11, m_m4WorldMatrix.m12, m_m4WorldMatrix.m13);
            float fScale = 1.0f;
            mbDoHeadTrack = false;
            if (pFielder->m_pBall != NULL)
            {
                pFielder->ReleaseBall(0);
                bReleased = true;
            }
            if (mGoalieActionState != GOALIEACTION_UNIDENTIFIED_13)
                mPursueDekeType = 0;
            switch (mPursueDekeType)
            {
            case 0:
            case 3:
            case 5:
            case 6:
                if (v3Velocity.x * mUnidentified024.m_v3Position.x > 0.0f)
                {
                    unsigned short aDirection;
                    if (pFielder->mUnidentified024.m_v3Position.y - mUnidentified024.m_v3Position.y > 0.0f)
                        aDirection = 0x4000;
                    else
                        aDirection = 0xC000;
                    pFielder->fn_80047240(this, aDirection, 1, false, false);
                    if (bReleased)
                    {
                        nlVector3 v3BallVelocity;
                        nlPolarToCartesian(v3BallVelocity.x, v3BallVelocity.y, aDirection,
                            lbl_806DBB94 + nlRandomf(lbl_806DBB98 - lbl_806DBB94));
                        v3BallVelocity.z = lbl_806DBB9C + nlRandomf(lbl_806DBBA0 - lbl_806DBB9C);
                        SetNoPickUpTime(0.3f);
                        g_pBall->m_tNoPickupTimer.SetSeconds(0.3f);
                        g_pBall->SetVelocity(v3BallVelocity, SPINTYPE_NONE, NULL);
                    }
                    return;
                }
                fScale = lbl_806DBB7C + nlRandomf(lbl_806DBB80);
                v3Velocity.z = lbl_806DBB84;
                break;
            case 1:
            case 2:
                fScale = lbl_806DBB88 + nlRandomf(lbl_806DBB8C);
                v3Velocity.z = lbl_806DBB90;
                break;
            case 4:
            case 7:
                break;
            }
            nlVec3Scale(v3Velocity, fScale);
            pFielder->fn_80044148(v3Velocity);
            fn_801BAF0C(pFielder);
            PlayRumbleAction(3, pFielder->GetGlobalPad());
        }
        if (bReleased)
        {
            cBall* pBall = g_pBall;
            nlVector3 v3Velocity;
            v3Velocity.y = pBall->m_v3Position.y > 0.0f ? 1.0f : -1.0f;
            float fRandomX = nlRandomf(0.5f);
            v3Velocity.x = pBall->m_v3Position.x < 0.0f ? fRandomX : -fRandomX;
            nlVec3Scale(v3Velocity, 6.0f + nlRandomf(2.0f));
            v3Velocity.z = 8.0f + nlRandomf(2.0f);
            SetNoPickUpTime(0.3f);
            g_pBall->m_tNoPickupTimer.SetSeconds(0.3f);
            g_pBall->SetVelocity(v3Velocity, SPINTYPE_NONE, NULL);
        }
    }
}

void Goalie::fn_800809D0(cFielder* pTarget, bool bParam)
{
    if (pTarget->IsFallenDown() || IsOnSameTeam(pTarget) || pTarget->fn_8003E6FC())
        return;

    if (mPursueDekeState == 0)
    {
        switch (mPursueDekeType)
        {
        case 0:
        case 3:
        case 5:
        case 6:
            mpLooseBallInfo = &LooseBallAnims::mLooseBallKickInfo[2];
        case 1:
        case 2:
            if (mpTarget == pTarget)
            {
                if (bParam && (m_pCurrentAnimController->get_fTime() < mpLooseBallInfo->GetPickupTime() - 0.06f
                        || mpLooseBallInfo->mnAnimID != m_eAnimID))
                {
                    SetAnimState(mpLooseBallInfo->mnAnimID, false, 0.02f, false, false);
                    m_pCurrentAnimController->SetTime(mpLooseBallInfo->GetPickupTime() - 0.06f);
                }
                mPursueDekeState = 2;
                fn_80080638(pTarget, false);
                mfTargetTime = 0.0f;
                fn_80080BFC(0.0f);
                PlayerAttackData data;
                data.pAttacker = this;
                data.nAttackerPadID = -1;
                data.pTarget = pTarget;
                data.mUnidentified0C = 2;
                data.mUnidentified10 = false;
                fn_8005E604(g_pGame, &data);
            }
            else
                TacklePlayer(pTarget);
            break;
        case 4:
        case 7:
            break;
        }
    }
    else if (mPursueDekeState == 1)
    {
        if (mpTarget == pTarget)
        {
            mPursueDekeState = 2;
            fn_80080638(pTarget, false);
            mfTargetTime = 0.0f;
            fn_80080BFC(0.0f);
            PlayerAttackData data;
            data.pAttacker = this;
            data.nAttackerPadID = -1;
            data.pTarget = pTarget;
            data.mUnidentified0C = 2;
            data.mUnidentified10 = false;
            fn_8005E604(g_pGame, &data);
        }
        else
            TacklePlayer(pTarget);
    }
}

extern "C" void fn_8005D550(void* pManager, const GoalieSaveData* pData);

void Goalie::DoPassRelease()
{
    if (m_pBall == NULL && mGoalieActionState == GOALIEACTION_LOOSEBALL_PICKUP
        && ((int)m_eAnimID == 3 || (int)m_eAnimID == 2))
    {
        nlVector3 v3AnimBallPos = GetJointPosition(m_nBallJointIndex);
        if (CalculateDistanceSquared(v3AnimBallPos, g_pBall->m_v3Position) < 0.36f)
            InitiatePickup();
        else
            return;
    }
    if (m_pBall == NULL)
        return;

    if (mGoalieActionState == GOALIEACTION_LOOSEBALL_PICKUP)
    {
        mpPassTarget = FindOpenPassTarget();
        if (mpPassTarget != NULL)
        {
            float dx = mpPassTarget->mUnidentified024.m_v3Position.x - mUnidentified024.m_v3Position.x;
            float dy = mpPassTarget->mUnidentified024.m_v3Position.y - mUnidentified024.m_v3Position.y;
            unsigned short aTarget = RadToAng16(nlATan2f(dy, dx));
            short aDiff = (short)(aTarget - FindDumpDirection(aTarget, false));
            if ((unsigned short)abs_s16(aDiff) > 0x2AA8 || !IsTargetViable(mpPassTarget))
                mpPassTarget = NULL;
        }
    }

    bool bIsKick = false;
    switch (m_eAnimID)
    {
    case 2:
    case 3:
    {
        bIsKick = true;
        GoalieSaveData data;
        data.saveType = g_pBall->m_uGoalType;
        data.pShooter = g_pBall->m_pShooter;
        data.pGoalie = this;
        fn_8005D550(g_pGame, &data);
        PlaySound(0, 0x03197C5C, NULL, NULL);
        break;
    }
    default:
        PlaySound(0, 0x66EB98CE, NULL, NULL);
        break;
    }

    if (mpPassTarget != NULL)
    {
        OpenTo(this, mpPassTarget);
        bool bLob = UnidentifiedWallBlocked();
        if (m_eAnimID == 2)
            bLob = true;
        else if (GetGlobalPad() != NULL && GetGlobalPad()->IsPressed(0x17, true))
            bLob = true;
        float fPassSpeedMin = m_pTweaks->fPassGroundSpeedMin;
        float fPassSpeedMax = m_pTweaks->fPassGroundSpeedMax;
        DoRegularPassing(mpPassTarget, bLob, true, false, false, fPassSpeedMin, fPassSpeedMax);
        if (bIsKick)
            fn_801B74C8(this);
        return;
    }

    nlVector3 v3Velocity;
    nlVector3 v3Direction;
    GoalieTweaks* pTweaks = m_pTweaks;
    eSpinType spinType = nlRandom(2) != 0 ? SPINTYPE_FORWARD : SPINTYPE_BACK;
    float fPercent = nlRandomf(1.0f);
    float fShotSpeed = Interpolate(pTweaks->fKickVelocityMin, pTweaks->fKickVelocityMax, fPercent);
    float fShotAng = Interpolate(pTweaks->fKickAngleMin, pTweaks->fKickAngleMax, fPercent);
    unsigned short aDesired;
    if (GetGlobalPad() != NULL)
    {
        if (m_pController->GetMovementStickMagnitude() > 0.001f)
            aDesired = m_pController->GetMovementStickDirection();
        else
        {
            nlVector3 v3Desired = v3Zero;
            float fYPos = cField::GetPenaltyBoxY() + nlRandomf(1.0f);
            v3Desired.y = mUnidentified024.m_v3Position.y > 0.0f ? fYPos : -fYPos;
            float dx = v3Desired.x - mUnidentified024.m_v3Position.x;
            float dy = v3Desired.y - mUnidentified024.m_v3Position.y;
            aDesired = RadToAng16(nlATan2f(dy, dx));
        }
    }
    else
    {
        nlVector3 v3Desired = v3Zero;
        float fYPos = cField::GetPenaltyBoxY() + nlRandomf(1.0f);
        v3Desired.y = mUnidentified024.m_v3Position.y > 0.0f ? fYPos : -fYPos;
        float dx = v3Desired.x - mUnidentified024.m_v3Position.x;
        float dy = v3Desired.y - mUnidentified024.m_v3Position.y;
        aDesired = RadToAng16(nlATan2f(dy, dx));
    }
    unsigned int aDump = FindDumpDirection(aDesired, true);

    float fSin;
    float fCos;
    nlSinCos(&fSin, &fCos, aDump);
    v3Direction.x = fCos;
    v3Direction.y = fSin;
    v3Direction.z = 0.0f;
    unsigned short aShot = RadToAng16((3.1415927f * fShotAng) / 180.0f);
    nlSinCos(&fSin, &fCos, aShot);
    float fXYMag = fCos * fShotSpeed;
    v3Velocity.x = v3Direction.x * fXYMag;
    v3Velocity.y = v3Direction.y * fXYMag;
    v3Velocity.z = fSin * fShotSpeed;
    PlayRumbleAction(1, GetGlobalPad());
    ReleaseBall(0);
    g_pBall->ShootRelease(v3Velocity, spinType);
    SetNoPickUpTime(0.25f);
    g_pBall->m_tNoPickupTimer.SetSeconds(0.15f);
    if (bIsKick)
        fn_801B73B8(this, false);
}

extern "C" UnidentifiedVariant_80054AB8* fn_80312360(
    UnidentifiedFuzzyRuntimeBase*, FunctionEntryPoint*, int, void*, void*);

extern "C" UnidentifiedVariant_80054AB8 fn_800821B0(
    UnidentifiedFuzzyRuntimeBase* runtime, const unsigned int& hash, cPlayer* player)
{
    unsigned int functionHash = hash;
    return UnidentifiedVariant_80054AB8(fn_80312360(
        runtime, runtime->FindFunctionEntryPoint(functionHash), 1, player, NULL));
}


u16 lbl_806DBBD0 = 0xFFFF;
extern u16 lbl_806DBD50;
extern u16 lbl_806DBD58;

#define REGISTER_GOALIE_FIELD(type, base, field, name) \
    cache->AddField(type, gDebugFieldTypes[type].size, \
        (u8*)&(field) - (u8*)&(base), name)

inline void GoalieFatigue::UnidentifiedSyncLog(void* context, DebugWriteCache* cache)
{
    if (lbl_806DBD50 == 0xFFFF)
    {
        lbl_806DBD50 = cache->BeginType("GoalieFatigue");
        REGISTER_GOALIE_FIELD(17, mfEnergyLevel, mfEnergyLevel, "mfEnergyLevel");
        REGISTER_GOALIE_FIELD(17, mfEnergyLevel, mfRecoverRate, "mfRecoverRate");
        REGISTER_GOALIE_FIELD(17, mfEnergyLevel, mfTimeSinceLastSave, "mfTimeSinceLastSave");
        REGISTER_GOALIE_FIELD(17, mfEnergyLevel, mfHotStreakTimer, "mfHotStreakTimer");
        cache->EndType();
    }
    cache->ChecksumData(lbl_806DBD50, &mfEnergyLevel, context);
    cache->WriteData(lbl_806DBD50, &mfEnergyLevel, sizeof(GoalieFatigue));
}

inline void SaveBlendInfo::UnidentifiedSyncLog(void* context, DebugWriteCache* cache)
{
    if (lbl_806DBD58 == 0xFFFF)
    {
        lbl_806DBD58 = cache->BeginType("SaveBlendInfo");
        REGISTER_GOALIE_FIELD(17, mfStartTime, mfStartTime, "mfStartTime");
        for (int milestone = 0; milestone < 5; milestone++)
        {
            REGISTER_GOALIE_FIELD(17, mfStartTime, mfMilestoneTime[milestone], "mfMilestoneTime[milestone]");
        }
        int milestone;
        for (int blend = 0; blend < 4; blend++)
        {
            for (milestone = 0; milestone < 5; milestone++)
            {
                REGISTER_GOALIE_FIELD(17, mfStartTime, mfMilestoneScale[blend][milestone], "mfMilestoneScale[blend][milestone]");
            }
            REGISTER_GOALIE_FIELD(15, mfStartTime, mpSaveData[blend], "mpSaveData[blend]");
        }
        REGISTER_GOALIE_FIELD(17, mfStartTime, mfSaveBlendPrimary, "mfSaveBlendPrimary");
        REGISTER_GOALIE_FIELD(17, mfStartTime, mfSaveBlendSecondary, "mfSaveBlendSecondary");
        REGISTER_GOALIE_FIELD(17, mfStartTime, mfSaveBlendComposite, "mfSaveBlendComposite");
        REGISTER_GOALIE_FIELD(22, mfStartTime, mv3BlendedSavePos, "mv3BlendedSavePos");
        cache->EndType();
    }
    SaveBlendInfo* data = (SaveBlendInfo*)cache->WriteData(lbl_806DBD58, &mfStartTime, sizeof(SaveBlendInfo));
    if (data != NULL)
    {
        for (int blend = 0; blend < 4; blend++)
            ((int*)data->mpSaveData)[blend] = mpSaveData[blend] == NULL ? -1 : mpSaveData[blend]->mnAnimID;
        cache->ChecksumData(lbl_806DBD58, data, context);
    }
}

void Goalie::Unknown11(void* context, DebugWriteCache* cache)
{
    cPlayer::Unknown11(context, cache);
    if (lbl_806DBBD0 == 0xFFFF)
    {
        lbl_806DBBD0 = cache->BeginType("DetGoalie");
        REGISTER_GOALIE_FIELD(14, mGoalieActionState, mGoalieActionState, "mGoalieActionState");
        REGISTER_GOALIE_FIELD(14, mGoalieActionState, mPrevGoalieActionState, "mPrevGoalieActionState");
        REGISTER_GOALIE_FIELD(14, mGoalieActionState, mUrgency, "mUrgency");
        REGISTER_GOALIE_FIELD(8, mGoalieActionState, mnSubstate, "mnSubstate");
        REGISTER_GOALIE_FIELD(14, mGoalieActionState, mMoveDirection, "mMoveDirection");
        REGISTER_GOALIE_FIELD(14, mGoalieActionState, mCrouchType, "mCrouchType");
        REGISTER_GOALIE_FIELD(14, mGoalieActionState, mPursueDekeType, "mPursueDekeType");
        REGISTER_GOALIE_FIELD(14, mGoalieActionState, mPursueDekeState, "mPursueDekeState");
        REGISTER_GOALIE_FIELD(17, mGoalieActionState, mfSwitchTime, "mfSwitchTime");
        REGISTER_GOALIE_FIELD(9, mGoalieActionState, muSaveType, "muSaveType");
        REGISTER_GOALIE_FIELD(17, mGoalieActionState, mfWaitTime, "mfWaitTime");
        REGISTER_GOALIE_FIELD(17, mGoalieActionState, mfTimeTilSave, "mfTimeTilSave");
        REGISTER_GOALIE_FIELD(17, mGoalieActionState, mfDelayTime, "mfDelayTime");
        REGISTER_GOALIE_FIELD(17, mGoalieActionState, mfWallBlock, "mfWallBlock");
        REGISTER_GOALIE_FIELD(9, mGoalieActionState, muWallID, "muWallID");
        REGISTER_GOALIE_FIELD(16, mGoalieActionState, mbPlayMiss, "mbPlayMiss");
        REGISTER_GOALIE_FIELD(16, mGoalieActionState, mbShouldMiss, "mbShouldMiss");
        REGISTER_GOALIE_FIELD(16, mGoalieActionState, mbStunEffectActive, "mbStunEffectActive");
        REGISTER_GOALIE_FIELD(16, mGoalieActionState, mbDoIntercept, "mbDoIntercept");
        REGISTER_GOALIE_FIELD(16, mGoalieActionState, mbDoNavigate, "mbDoNavigate");
        REGISTER_GOALIE_FIELD(16, mGoalieActionState, mbDoHeadTrack, "mbDoHeadTrack");
        REGISTER_GOALIE_FIELD(16, mGoalieActionState, mbBallImpacted, "mbBallImpacted");
        REGISTER_GOALIE_FIELD(16, mGoalieActionState, mbNoUserControl, "mbNoUserControl");
        REGISTER_GOALIE_FIELD(16, mGoalieActionState, mbIsPosed, "mbIsPosed");
        REGISTER_GOALIE_FIELD(16, mGoalieActionState, mbIsDown, "mbIsDown");
        REGISTER_GOALIE_FIELD(16, mGoalieActionState, mbPickedUp, "mbPickedUp");
        REGISTER_GOALIE_FIELD(16, mGoalieActionState, mbRecalcSave, "mbRecalcSave");
        REGISTER_GOALIE_FIELD(16, mGoalieActionState, mbCheckForMegaGoal, "mbCheckForMegaGoal");
        REGISTER_GOALIE_FIELD(16, mGoalieActionState, mbMegaUserSave, "mbMegaUserSave");
        REGISTER_GOALIE_FIELD(16, mGoalieActionState, mbGrabMonty, "mbGrabMonty");
        REGISTER_GOALIE_FIELD(16, mGoalieActionState, mbTryLobSave, "mbTryLobSave");
        REGISTER_GOALIE_FIELD(22, mGoalieActionState, mv3LocalContactPosition, "mv3LocalContactPosition");
        REGISTER_GOALIE_FIELD(22, mGoalieActionState, mv3LocalContactVelocity, "mv3LocalContactVelocity");
        REGISTER_GOALIE_FIELD(22, mGoalieActionState, mv3TargetPosition, "mv3TargetPosition");
        REGISTER_GOALIE_FIELD(22, mGoalieActionState, mv3TargetVelocity, "mv3TargetVelocity");
        REGISTER_GOALIE_FIELD(22, mGoalieActionState, mv3NavTarget, "mv3NavTarget");
        REGISTER_GOALIE_FIELD(22, mGoalieActionState, mv3LocalNavTarget, "mv3LocalNavTarget");
        REGISTER_GOALIE_FIELD(10, mGoalieActionState, maLocalAngle, "maLocalAngle");
        REGISTER_GOALIE_FIELD(19, mGoalieActionState, maInitialAngle, "maInitialAngle");
        REGISTER_GOALIE_FIELD(19, mGoalieActionState, maSaveAngle, "maSaveAngle");
        REGISTER_GOALIE_FIELD(17, mGoalieActionState, mfTargetTime, "mfTargetTime");
        REGISTER_GOALIE_FIELD(17, mGoalieActionState, mfTargetDist, "mfTargetDist");
        REGISTER_GOALIE_FIELD(17, mGoalieActionState, mfSpeedScale, "mfSpeedScale");
        REGISTER_GOALIE_FIELD(17, mGoalieActionState, mfBallCharge, "mfBallCharge");
        REGISTER_GOALIE_FIELD(17, mGoalieActionState, mfNextBallTime, "mfNextBallTime");
        REGISTER_GOALIE_FIELD(17, mGoalieActionState, mfMegaAccuracy, "mfMegaAccuracy");
        REGISTER_GOALIE_FIELD(17, mGoalieActionState, mfMegaTargetTime, "mfMegaTargetTime");
        REGISTER_GOALIE_FIELD(9, mGoalieActionState, muBallChangeCount, "muBallChangeCount");
        REGISTER_GOALIE_FIELD(9, mGoalieActionState, muBallDeflectCount, "muBallDeflectCount");
        REGISTER_GOALIE_FIELD(14, mGoalieActionState, mnOffplayPending, "mnOffplayPending");
        REGISTER_GOALIE_FIELD(9, mGoalieActionState, muMegaAnimState, "muMegaAnimState");
        REGISTER_GOALIE_FIELD(2, mGoalieActionState, muMegaStoreTexID, "muMegaStoreTexID");
        REGISTER_GOALIE_FIELD(9, mGoalieActionState, muMegaNextTarget, "muMegaNextTarget");
        REGISTER_GOALIE_FIELD(9, mGoalieActionState, muMegaReadyToSave, "muMegaReadyToSave");
        REGISTER_GOALIE_FIELD(9, mGoalieActionState, mBallsLaunched, "mBallsLaunched");
        REGISTER_GOALIE_FIELD(8, mGoalieActionState, mLowLobAnim, "mLowLobAnim");
        REGISTER_GOALIE_FIELD(20, mGoalieActionState, mFreezeTimer, "mFreezeTimer");
        REGISTER_GOALIE_FIELD(4, mGoalieActionState, mMegaMachine, "mMegaMachine");
        REGISTER_GOALIE_FIELD(15, mGoalieActionState, mpPassTarget, "mpPassTarget");
        REGISTER_GOALIE_FIELD(15, mGoalieActionState, mpShooter, "mpShooter");
        REGISTER_GOALIE_FIELD(15, mGoalieActionState, mpTarget, "mpTarget");
        REGISTER_GOALIE_FIELD(15, mGoalieActionState, mpMonty, "mpMonty");
        REGISTER_GOALIE_FIELD(15, mGoalieActionState, mpSkillShooter, "mpSkillShooter");
        REGISTER_GOALIE_FIELD(15, mGoalieActionState, mpSaveData, "mpSaveData");
        cache->EndType();
    }
    void* data = cache->WriteData(lbl_806DBBD0, &mGoalieActionState,
        offsetof(Goalie, mBlendInfo) - offsetof(Goalie, mGoalieActionState));
    if (data != NULL)
    {
        *(int*)((u8*)data + offsetof(Goalie, mpPassTarget) - offsetof(Goalie, mGoalieActionState))
            = mpPassTarget == NULL ? -1 : mpPassTarget->mUnidentified120;
        *(int*)((u8*)data + offsetof(Goalie, mpShooter) - offsetof(Goalie, mGoalieActionState))
            = mpShooter == NULL ? -1 : mpShooter->mUnidentified120;
        *(int*)((u8*)data + offsetof(Goalie, mpTarget) - offsetof(Goalie, mGoalieActionState))
            = mpTarget == NULL ? -1 : mpTarget->mUnidentified120;
        *(int*)((u8*)data + offsetof(Goalie, mpMonty) - offsetof(Goalie, mGoalieActionState))
            = mpMonty == NULL ? -1 : mpMonty->mUnidentified120;
        *(int*)((u8*)data + offsetof(Goalie, mpSkillShooter) - offsetof(Goalie, mGoalieActionState))
            = mpSkillShooter == NULL ? -1 : mpSkillShooter->mUnidentified120;
        *(int*)((u8*)data + offsetof(Goalie, mpSaveData) - offsetof(Goalie, mGoalieActionState))
            = mpSaveData == NULL ? -1 : mpSaveData->mnAnimID;
        cache->ChecksumData(lbl_806DBBD0, data, context);
    }
    mBlendInfo.UnidentifiedSyncLog(context, cache);
    mFatigue.UnidentifiedSyncLog(context, cache);
}

#undef REGISTER_GOALIE_FIELD

inline unsigned char Goalie::ClampToGoalCone(nlVector3& v3Position, float fDistFromEnd)
{
    float fGoalLineX = cField::GetGoalLineX(1U);
    float fAbsY = (float)fabsf(v3Position.y);
    float fSidelineY = cField::GetSidelineY(1U);
    float fXLimit = fGoalLineX - ((fAbsY * fDistFromEnd) / fSidelineY);
    if (v3Position.x > fXLimit)
    {
        v3Position.x = fXLimit;
        return true;
    }
    if (v3Position.x < -fXLimit)
    {
        v3Position.x = -fXLimit;
        return true;
    }
    return false;
}

void Goalie::FindDesiredGoaliePosition(nlVector3& pos, nlVector3& dir, nlVector3& focus, unsigned short& ang, const nlVector3* pThreatPos)
{
    float fMinDist = 0.5f;
    cNet* pNet = m_pTeam->m_pNet;
    nlVector3 targetPos;
    nlVector3 goalPos;
    nlVector3 desiredVec;
    float goalieDist;
    float goalLine;
    float targetDist;
    float fNetY;
    nlVector3 desiredPos;

    float fDirection = pNet->m_fDirection;
    nlVec3Set(goalPos, 0.5f * fDirection + pNet->GetGoalLineX(), 0.0f, 0.0f);

    if (pThreatPos == NULL)
    {
        cBall* pBall = g_pBall;
        if (pBall->m_pOwner != NULL)
        {
            cPlayer* pOwner = pBall->m_pOwner;
            nlVec3ScaleAdd(targetPos, 0.18f, pOwner->mUnidentified024.m_v3Velocity, pBall->m_v3Position);
        }
        else
        {
            nlVec3ScaleAdd(targetPos, 0.18f, pBall->m_v3Velocity, pBall->m_v3Position);
        }
        targetPos.z = 0.0f;
    }
    else
    {
        targetPos = *pThreatPos;
    }

    ClampToGoalCone(targetPos, 3.0f);

    nlVec3Sub(desiredVec, targetPos, goalPos);

    goalLine = cField::GetGoalLineX(1U) - 0.5f;
    goalieDist = nlSqrt(nlVec3LengthSquared(desiredVec), true);
    targetDist = goalieDist - 0.5f;
    goalieDist = IsSoloBreakaway();

    if (goalieDist > 0.8f)
    {
        goalieDist = Interpolate(2.5f, 8.0f, 5.0000005f * (goalieDist - 0.8f));
        mUrgency = URGENCY_MED;
    }
    else if (targetDist > 23.0f)
    {
        goalieDist = 8.0f;
    }
    else if (targetDist > 19.0f)
    {
        goalieDist = 2.5f + (5.5f * (targetDist - 19.0f) / 4.0f);
    }
    else if (targetDist > 12.0f)
    {
        goalieDist = 2.5f;
    }
    else if (targetDist > 7.0f)
    {
        goalieDist = 4.0f + (-1.5f * (targetDist - 7.0f) / 5.0f);
    }
    else
    {
        goalieDist = targetDist - 3.0f;
    }

    if (goalieDist > targetDist - 3.0f)
    {
        goalieDist = targetDist - 3.0f;
    }
    if (goalieDist < fMinDist)
    {
        goalieDist = fMinDist;
    }

    goalieDist += 0.5f;
    targetDist += 0.5f;
    nlVec3ScaleAdd(desiredPos, goalieDist / targetDist, desiredVec, goalPos);

    if (fabsf(mUnidentified024.m_v3Position.x) > cField::GetGoalLineX(1U))
    {
        float fNetWidth = cNet::GetNetWidth();
        fNetY = 0.5f * fNetWidth - 1.0f;
        desiredPos.y = nlMinEquals(nlMaxEquals(desiredPos.y, -fNetY), fNetY);
        desiredPos.x = goalLine * pNet->m_fDirection;
        nlVec3Sub(desiredVec, desiredPos, mUnidentified024.m_v3Position);
    }

    ang = nlVector3ToAngle(desiredVec);

    desiredPos.x = nlMinEquals(nlMaxEquals(desiredPos.x, -goalLine), goalLine);

    const nlVector3& rPos = mUnidentified024.m_v3Position;
    nlVec3WeightedSum(pos, 0.8f, desiredPos, 0.2f, rPos);
    dir = desiredVec;
    focus = targetPos;
}


void Goalie::DoNavigation(float fDeltaT, float fIdleDistance, Goalie::eNaviMode naviMode)
{
    u16 deltaToBall;
    cBall* pBall;
    s16 aGoalie2Ball;
    u16 absBallAngleDiff;
    unsigned int nParam;
    int eAnimID;
    int nCurrentAnimID;
    int nFinalAnim;
    unsigned int aFinalDir;
    bool bNeedChange;
    u16 desiredAng;

    pBall = g_pBall;
    nCurrentAnimID = m_eAnimID;
    eAnimID = nCurrentAnimID;
    f32 fTime = m_pCurrentAnimController->m_fTime;

    GetLocalPoint(mv3LocalNavTarget, mv3NavTarget, mUnidentified024.m_v3Position, mUnidentified024.m_aActualFacingDirection);

    float distSq = mv3LocalNavTarget.x * mv3LocalNavTarget.x + mv3LocalNavTarget.y * mv3LocalNavTarget.y;

    if (naviMode == NAVI_FACE_BALL)
    {
        float dx = pBall->GetPosition().x - mUnidentified024.m_v3Position.x;
        float dy = pBall->GetPosition().y - mUnidentified024.m_v3Position.y;
        mUnidentified024.m_aDesiredFacingDirection = RadToAng16(nlATan2f(dy, dx));
    }

    aGoalie2Ball = (s16)(mUnidentified024.m_aDesiredFacingDirection - mUnidentified024.m_aActualFacingDirection);
    absBallAngleDiff = (u16)abs_s16(aGoalie2Ball);

    if (distSq < fIdleDistance * fIdleDistance)
    {
        int nNewAnim;
        if (absBallAngleDiff > 0x2AA8)
        {
            int idleTurns[2][2] = {
                { 0x0D, 0x0F },
                { 0x0C, 0x0E },
            };
            int neg = (aGoalie2Ball <= 0) ? 1 : 0;
            int big = (absBallAngleDiff >= 0x5FFA) ? 1 : 0;
            nNewAnim = idleTurns[neg][big];
        }
        else
        {
            nNewAnim = 0x05;
        }

        bNeedChange = false;
        if (mnSubstate != 1 || (bNeedChange = (m_pCurrentAnimController->UnidentifiedAtEnd())))
        {
            bNeedChange = false;
            if (nNewAnim != m_eAnimID || (bNeedChange = (m_pCurrentAnimController->UnidentifiedAtEnd())))
            {
                SetAnimState(nNewAnim, true, 0.2f, false, false);
            }
        }

        if (nNewAnim == 0x05)
        {
            InitMovementFromAnimSeek(182.04445f * lbl_806DBBB8, 2730.6667f);
        }
        else
        {
            InitMovementFromAnimSeek(32768.0f, 2730.6667f);
        }

        if (mnSubstate != 1)
        {
            mnSubstate = 1;
        }
        mMoveDirection = GOALIEDIR_IDLE;
        return;
    }

    float runDistSq = nlGetLengthSquared1D(mfGoalieRunDist);
    float urgentDistSq = nlGetLengthSquared1D(lbl_806DBB14);

    desiredAng = nlVector3ToAngle(mv3LocalNavTarget);

    if (mUrgency != URGENCY_LOW && mMoveDirection != GOALIEDIR_SIDE && (distSq > runDistSq || (mUrgency == URGENCY_HIGH && distSq > urgentDistSq)))
    {
        unsigned int aBaseDir;
        bool bDoBackward;
        bool bDoSeek;

        mnSubstate = 4;

        aBaseDir = (desiredAng + mUnidentified024.m_aActualFacingDirection) & 0xFFFF;
        aFinalDir = aBaseDir;
        deltaToBall = abs_ang16(nlAngleDiff(desiredAng, (u16)aGoalie2Ball));
        nFinalAnim = 0x24;

        if (naviMode == NAVI_FACE_DESIRED && absBallAngleDiff < 0x3FFC)
        {
            if (deltaToBall > 0x4E34)
            {
                aFinalDir += 0x8000;
                nFinalAnim = 0x25;
            }
            else if (deltaToBall > 0x31C4)
            {
                if (mv3LocalNavTarget.y > 0.0f)
                {
                    aFinalDir = aBaseDir - 0x4000;
                    nFinalAnim = 0x21;
                }
                else
                {
                    aFinalDir = aBaseDir + 0x4000;
                    nFinalAnim = 0x20;
                }
            }
        }
        else if (naviMode == NAVI_FACE_BALL)
        {
            bDoBackward = false;

            if (mMoveDirection == GOALIEDIR_IDLE || mMoveDirection == GOALIEDIR_BACKWARD)
            {
                float ballToNavDistSq = nlVec3DistanceSquared2D(pBall->GetPosition(), mv3NavTarget);

                if (distSq < ballToNavDistSq)
                {
                    float dx = pBall->GetPosition().x - mv3NavTarget.x;
                    float dy = pBall->GetPosition().y - mv3NavTarget.y;
                    int ballAng = RadToAng16(nlATan2f(dy, dx));
                    s16 diff = (s16)(aFinalDir + ballAng + 0x8000);
                    if ((u16)abs_s16(diff) < 0x1554)
                    {
                        bDoBackward = true;
                    }
                }
            }

            if (mMoveDirection == GOALIEDIR_BACKWARD || mMoveDirection == GOALIEDIR_FRONT2BACK)
            {
                if (deltaToBall > 0x3FFC)
                {
                    bDoBackward = true;
                }
            }
            else
            {
                if (deltaToBall > 0x5550)
                {
                    bDoBackward = true;
                }
            }

            if (bDoBackward)
            {
                if (!mbDoIntercept || (mMoveDirection != GOALIEDIR_FORWARD && mMoveDirection != GOALIEDIR_BACK2FRONT))
                {
                    aFinalDir += 0x8000;
                    nFinalAnim = 0x25;
                }
            }
        }

        s16 finalDiff = (s16)(aFinalDir - mUnidentified024.m_aActualFacingDirection);
        u16 absFinalDiff = (u16)abs_s16(finalDiff);

        bDoSeek = true;

        if (nFinalAnim == 0x24)
        {
            switch (mMoveDirection)
            {
            case GOALIEDIR_IDLE:
                mMoveDirection = GOALIEDIR_FORWARD;
                if (absFinalDiff < 0x3552)
                {
                    mUnidentified024.m_aDesiredFacingDirection = aFinalDir;
                    eAnimID = 0x24;
                }
                else
                {
                    if (absFinalDiff > 0x6388)
                    {
                        eAnimID = 0x22;
                        if (aGoalie2Ball > 0)
                            eAnimID = 0x23;
                    }
                    else
                    {
                        eAnimID = 0x22;
                        if (finalDiff > 0)
                            eAnimID = 0x23;
                    }
                    mUnidentified024.m_aDesiredFacingDirection = aFinalDir;
                    mfSwitchTime = 0.5f;
                }
                break;

            case GOALIEDIR_FORWARD:
                mUnidentified024.m_aDesiredFacingDirection = aFinalDir;
                if (absFinalDiff < 0x3FFC)
                {
                    if ((nCurrentAnimID == 0x23 || nCurrentAnimID == 0x22) && fTime < mfSwitchTime)
                        break;
                    eAnimID = 0x24;
                }
                else
                {
                    if (absFinalDiff > 0x6388)
                    {
                        eAnimID = 0x22;
                        if (aGoalie2Ball > 0)
                            eAnimID = 0x23;
                    }
                    else
                    {
                        eAnimID = 0x22;
                        if (finalDiff > 0)
                            eAnimID = 0x23;
                    }
                    mfSwitchTime = 0.5f;
                }
                break;

            case GOALIEDIR_BACKWARD:
                mMoveDirection = GOALIEDIR_BACK2FRONT;
                mUnidentified024.m_aDesiredFacingDirection = (u16)(aFinalDir + 0x8000);
                bDoSeek = false;
                if (absFinalDiff < 0x1FFE)
                {
                    eAnimID = 0x2B;
                    mfSwitchTime = 0.42857143f;
                }
                else
                {
                    eAnimID = 0x28;
                    if (finalDiff < 0)
                        eAnimID = 0x29;
                    mfSwitchTime = 0.5714286f;
                }
                break;

            case GOALIEDIR_SIDE:
                break;

            case GOALIEDIR_BACK2FRONT:
                bDoSeek = false;
                if (fTime > mfSwitchTime)
                    mMoveDirection = GOALIEDIR_FORWARD;
                break;

            case GOALIEDIR_FRONT2BACK:
                if (fTime > mfSwitchTime)
                    bDoSeek = true;
                else
                    bDoSeek = false;
                if (fTime < mfSwitchTime)
                    mMoveDirection = GOALIEDIR_FORWARD;
                else
                    mMoveDirection = GOALIEDIR_BACKWARD;
                break;
            }
        }
        else if (nFinalAnim == 0x25)
        {
            switch (mMoveDirection)
            {
            case GOALIEDIR_IDLE:
                mMoveDirection = GOALIEDIR_BACKWARD;
                if (absFinalDiff < 0x3FFC)
                {
                    mUnidentified024.m_aDesiredFacingDirection = aFinalDir;
                    eAnimID = 0x25;
                }
                else
                {
                    if (absFinalDiff < 0x6388)
                    {
                        eAnimID = 0x26;
                        if (finalDiff > 0)
                            eAnimID = 0x27;
                    }
                    else
                    {
                        eAnimID = 0x26;
                        if (aGoalie2Ball > 0)
                            eAnimID = 0x27;
                    }
                    mMoveDirection = GOALIEDIR_FRONT2BACK;
                    mUnidentified024.m_aDesiredFacingDirection = (u16)(aFinalDir + 0x8000);
                    bDoSeek = false;
                    mfSwitchTime = 0.71428573f;
                }
                break;

            case GOALIEDIR_FORWARD:
                mMoveDirection = GOALIEDIR_FRONT2BACK;
                mUnidentified024.m_aDesiredFacingDirection = (u16)(aFinalDir + 0x8000);
                bDoSeek = false;
                if (absFinalDiff < 0x1FFE)
                {
                    eAnimID = 0x2A;
                    mfSwitchTime = 0.5f;
                }
                else
                {
                    if (absFinalDiff < 0x6388)
                    {
                        eAnimID = 0x26;
                        if (finalDiff > 0)
                            eAnimID = 0x27;
                    }
                    else
                    {
                        eAnimID = 0x26;
                        if (aGoalie2Ball > 0)
                            eAnimID = 0x27;
                    }
                    mfSwitchTime = 0.71428573f;
                }
                break;

            case GOALIEDIR_BACKWARD:
                mUnidentified024.m_aDesiredFacingDirection = aFinalDir;
                eAnimID = 0x25;
                break;

            case GOALIEDIR_SIDE:
                break;

            case GOALIEDIR_BACK2FRONT:
                bDoSeek = false;
                if (fTime < mfSwitchTime)
                    mMoveDirection = GOALIEDIR_BACKWARD;
                else
                    mMoveDirection = GOALIEDIR_FORWARD;
                break;

            case GOALIEDIR_FRONT2BACK:
                if (fTime > mfSwitchTime)
                    bDoSeek = true;
                else
                    bDoSeek = false;
                if (fTime > mfSwitchTime)
                    mMoveDirection = GOALIEDIR_BACKWARD;
                break;
            }
        }
        else
        {
            eAnimID = nFinalAnim;
            mMoveDirection = GOALIEDIR_SIDE;
            mUnidentified024.m_aDesiredFacingDirection = aFinalDir;
        }

        bNeedChange = false;
        if (eAnimID != m_eAnimID || (bNeedChange = (m_pCurrentAnimController->UnidentifiedAtEnd())))
        {
            SetAnimState(eAnimID, true, 0.2f, false, false);
        }

        float fPlaybackSpeedScale = 1.2f;
        if (eAnimID == 0x25)
            fPlaybackSpeedScale *= m_pTweaks->fSaveBackRunTimeScale;

        if (bDoSeek)
        {
            InitMovementFromAnimSeek(m_pTweaks->fRunningDirectionSeekSpeed, m_pTweaks->fRunningDirectionSeekFalloff);
        }
        else
        {
            InitMovementFromAnim(0, v3Zero, 1.0f, false);
        }
        m_pCurrentAnimController->m_fPlaybackSpeedScale = fPlaybackSpeedScale;
        return;
    }

    u16 aNavDir = SeekDirection(mUnidentified024.m_aActualFacingDirection, mUnidentified024.m_aDesiredFacingDirection, m_pTweaks->fRunningDirectionSeekSpeed, m_pTweaks->fRunningDirectionSeekFalloff, fDeltaT);
    SetFacingDirection(aNavDir, true);

    GetLocalPoint(mv3LocalNavTarget, mv3NavTarget, mUnidentified024.m_v3Position, aNavDir);

    bNeedChange = false;
    int* nLeftAnims = 0;
    int* nRightAnims = 0;

    maLocalAngle = nlVector3ToAngle(mv3LocalNavTarget);

    static int gStepLAnims[3] = { 0x1B, 0x1A, 0x1C };
    static int gStepRAnims[3] = { 0x18, 0x17, 0x19 };
    static int gStrafeLAnims[3] = { 0x14, 0x15, 0x16 };
    static int gStrafeRAnims[3] = { 0x11, 0x12, 0x13 };

    float strafeDistSq = nlGetLengthSquared1D(mfGoalieStrafeDist);

    if (distSq < strafeDistSq && mUrgency == URGENCY_LOW && mnSubstate != 3)
    {
        if (mnSubstate != 2)
        {
            mnSubstate = 2;
            bNeedChange = true;
            nLeftAnims = gStepLAnims;
            nRightAnims = gStepRAnims;
        }
    }
    else
    {
        bool bDoStrafe = true;
        if (mMoveDirection == GOALIEDIR_SIDE)
        {
            if (nlVec3DistanceSquared2D(mv3NavTarget, mUnidentified024.m_v3Position)
                <= nlVec3DistanceSquared2D(mv3NavTarget, mUnidentified024.m_v3PrevPosition))
            {
                bool bAnimDone = m_pCurrentAnimController->UnidentifiedAtEnd();
                if (!bAnimDone)
                {
                    bDoStrafe = false;
                }
            }
        }

        if (bDoStrafe && mnSubstate != 3)
        {
            mnSubstate = 3;
            bNeedChange = true;
            nLeftAnims = gStrafeLAnims;
            nRightAnims = gStrafeRAnims;
        }
    }

    if (mnSubstate == 3)
    {
        f32 fDist = nlSqrt(distSq, true);
        f32 fSpeedScale = InterpolateRangeClamped(0.5f, 2.0f, mfGoalieStepDist, 2.0f * mfGoalieStrafeDist, fDist);

        f32 fHighEnergyLimit = ((GoalieTweaks*)m_pTweaks)->fGetupEnergyHigh;

        if (mFatigue.GetEnergyLevel() < fHighEnergyLimit && fSpeedScale > 0.8f)
        {
            f32 fNewScale = InterpolateRangeClamped(0.8f, fSpeedScale, ((GoalieTweaks*)m_pTweaks)->fGetupEnergyLow, fHighEnergyLimit, mFatigue.GetEnergyLevel());
            fSpeedScale = fNewScale;
        }

        mfSpeedScale = SeekSpeed(mfSpeedScale, fSpeedScale, 10.0f, 5.0f, fDeltaT);
    }

    if (!bNeedChange)
    {
        return;
    }

    mMoveDirection = GOALIEDIR_IDLE;

    cPN_SingleAxisBlender* pSAB_L = CreateSingleAxisBlender(nLeftAnims, 3, 1, MoveWeightCB, 0.1f, 0, 0.0f);
    cPN_SingleAxisBlender* pSAB_R = CreateSingleAxisBlender(nRightAnims, 3, 1, MoveWeightCB, 0.1f, 0, 0.0f);

    cPN_SingleAxisBlender* pSAB = pSAB_L;
    nParam = (unsigned int)this;
    cPN_SAnimController* pPrevCtrlr = 0;
    int j;

    for (j = 0; j < 2; j++)
    {
        for (int i = 0; i < 3; i++)
        {
            cPN_SAnimController* pCtrlr = (cPN_SAnimController*)pSAB->GetChild(i);

            if (i != 1 || j > 0)
            {
                pCtrlr->m_bIgnoreTriggers = true;
            }

            if (mnSubstate == 3)
            {
                if (pPrevCtrlr == 0)
                {
                    pCtrlr->m_pSynchronizedWeightCallback = StrafeSynchronizedSpeedCallback;
                    pCtrlr->m_nSynchronizedWeightCallbackParam = (unsigned int)this;
                    pCtrlr->m_fSynchronizedWeight = 0.0f;
                }
                else
                {
                    pCtrlr->m_bIsSynchronized = true;
                    pPrevCtrlr->m_pSynchronizedController = pCtrlr;
                }
                pPrevCtrlr = pCtrlr;
            }
        }
        pSAB = pSAB_R;
    }

    cPN_SingleAxisBlender* pDirBlender = new cPN_SingleAxisBlender(2, MoveDirectionCB, nParam, 0.1f);

    pDirBlender->SetChild(0, pSAB_L);
    pDirBlender->SetChild(1, pSAB_R);

    MoveWeightCB(nParam, pSAB_L);
    MoveWeightCB(nParam, pSAB_R);
    MoveDirectionCB(nParam, pDirBlender);

    cPN_Blender* pBlender = new cPN_Blender(*m_pAILayer, pDirBlender, 0.1f);

    *m_pAILayer = (cPoseNode*)pBlender;
    InitMovementFromAnim(0, v3Zero, 1.0f, true);
}

extern "C" bool fn_800977A4(cFielder* pFielder, float fHeight);

void Goalie::CollideWithCharacterCallback(CollisionPlayerPlayerData* pData)
{
    cPlayer* pPlayer = pData->player2;
    if (pPlayer->m_eClassType != FIELDER)
        return;
    cPlayer::CollideWithCharacterCallback(pData);
    cFielder* pFldr = static_cast<cFielder*>(pPlayer);
    if ((int)pFldr->mUnidentified024.m_eCharacterClass == 0x12
        && mGoalieActionState != GOALIEACTION_UNIDENTIFIED_30
        && pFldr->IsInvincibleChars() && pFldr->m_eActionState == 0x20
        && pFldr->mUnidentified178 > 0.4f && !fn_800977A4(pFldr, 0.0f))
    {
        fn_8008ED44(true);
        return;
    }
    if ((int)pFldr->mUnidentified024.m_eCharacterClass == 0x13
        && mGoalieActionState != GOALIEACTION_UNIDENTIFIED_29
        && pFldr->m_eActionState == 0x20)
        return;

    float fHeight = 0.25f + GetJointPosition(m_nHeadJointIndex).z;
    if (!pFldr->mbTangible || pFldr->IsFrozen() || pFldr->IsCharacterInAir(fHeight)
        || pFldr->IsInvincible() || pFldr->fn_8003E6FC())
        return;

    switch (mGoalieActionState)
    {
    case GOALIEACTION_MOVE_WB:
    {
        if (!IsOnSameTeam(pPlayer) && pPlayer->m_eClassType == FIELDER
            && !pFldr->IsFallenDown() && m_eAnimID != 8 && m_eAnimID != 10 && m_eAnimID != 9)
        {
            float x = pPlayer->mUnidentified024.m_v3Position.x - mUnidentified024.m_v3Position.x;
            float y = pPlayer->mUnidentified024.m_v3Position.y - mUnidentified024.m_v3Position.y;
            unsigned short aHit = RadToAng16(nlATan2f(y, x));
            bool bHitReactResult = pFldr->fn_80047240(this, aHit, 1, false, false);
            if (bHitReactResult)
                PlayRumbleAction(1, GetGlobalPad());
            mnSubstate = 6;
            nlVector3 v3LocalPos;
            GetLocalPoint(v3LocalPos, pPlayer->mUnidentified024.m_v3Position,
                mUnidentified024.m_v3Position, mUnidentified024.m_aActualFacingDirection);
            int anim;
            if (fabsf(v3LocalPos.x / v3LocalPos.y) > 1.0f)
                anim = 8;
            else if (v3LocalPos.y > 0.0f)
                anim = 10;
            else
                anim = 9;
            PlayNewAnim(anim);
            InitMovementFromAnim(0, v3Zero, 1.0f, false);
            if (bHitReactResult)
                PlaySound(9, 0x06024E5D, NULL, NULL);
        }
        break;
    }
    case GOALIEACTION_PURSUE_BALL_CARRIER:
    case GOALIEACTION_PURSUE_BALL_POUNCE:
        if (!mbPlayMiss && !IsOnSameTeam(pPlayer))
            ExecutePounce(pPlayer, false);
        break;
    case GOALIEACTION_MOVE:
    {
        int nJoint = m_nRightFootJointIndex;
        float fHeadHeight = 0.4f + GetJointPosition(m_nHeadJointIndex).z;
        if (pPlayer->GetJointPosition(nJoint).z > fHeadHeight)
            return;
    }
    case GOALIEACTION_SAVE:
    case GOALIEACTION_PASS:
    case GOALIEACTION_LOOSEBALL_SETUP:
    case GOALIEACTION_LOOSEBALL_CATCH:
    case GOALIEACTION_LOOSEBALL_PICKUP:
    case GOALIEACTION_LOOSEBALL_PURSUE_BOUNCING:
    case GOALIEACTION_LOOSEBALL_PURSUE_ROLLING:
        if (!IsOnSameTeam(pPlayer))
            TacklePlayer(pPlayer);
        break;
    case GOALIEACTION_LOOSEBALL_DESPERATE:
        if (pPlayer == g_pBall->m_pOwner)
            InitiatePanicGrab(pPlayer);
        else if (!IsOnSameTeam(pPlayer))
            TacklePlayer(pPlayer);
        break;
    case GOALIEACTION_UNIDENTIFIED_20:
    case GOALIEACTION_UNIDENTIFIED_21:
        if (!IsOnSameTeam(pPlayer))
            TacklePlayer(pPlayer);
        break;
    }
}

extern "C" void fn_800180F4(cBall* pBall, nlVector3* pPosition, float fTime);

bool Goalie::fn_8007BF68(bool bParam)
{
    if (g_pBall->m_pOwner != NULL)
        return false;
    mbTryLobSave = false;
    cBall* pBall = g_pBall;
    if (pBall->m_v3Position.x * mUnidentified024.m_v3Position.x < 0.0f)
        return false;
    if (pBall->m_v3Position.z < 1.5f && pBall->m_v3Velocity.z < 1.0f)
        return false;
    nlVector3 v3LandingPos;
    int nNumSolutions;
    float fTimes[2];
    pBall->PredictLandingSpotAndTime(v3LandingPos, &nNumSolutions, fTimes, 0.23f);
    if (nNumSolutions > 0)
    {
        float fTime = nNumSolutions > 1 ? nlMaxEquals(fTimes[0], fTimes[1]) : fTimes[0];
        if (fTime < lbl_806DBBB0)
            return false;
        fn_800180F4(g_pBall, &v3LandingPos, fTime);
        bool bShouldMiss = mbShouldMiss;
        if (bParam)
        {
            bShouldMiss = false;
            if (fTime < 1.5f
                && nlVec3DistanceSquared2D(mUnidentified024.m_v3Position, v3LandingPos) > 16.0f
                && nlRandomf(1.0f) < 0.1f)
                bShouldMiss = true;
        }
        float fGoalLine = cField::GetGoalLineX(v3LandingPos.x);
        if ((float)fabs(v3LandingPos.x) > 0.1f + (float)fabs(fGoalLine))
        {
            float fDiffX = v3LandingPos.x - pBall->m_v3Position.x;
            float fY = pBall->m_v3Position.y;
            if ((float)fabs(fDiffX) > 0.01f)
                fY += (v3LandingPos.y - fY) * (fGoalLine - pBall->m_v3Position.x) / fDiffX;
            if ((float)fabs(fY) < 0.5f * cNet::GetNetWidth() + 0.18f)
            {
                nlVector4 v4Plane;
                if (fGoalLine < 0.0f)
                    nlVec4Set(v4Plane, 1.0f, 0.0f, 0.0f, 0.1f + fGoalLine);
                else
                    nlVec4Set(v4Plane, -1.0f, 0.0f, 0.0f, 0.1f + -fGoalLine);
                nlVector3 v3Position;
                nlVector3 v3Velocity;
                fn_8016EEC8();
                float fContactTime = FakeBallWorld::GetPredictedPlaneIntersectTime(v4Plane, v3Position, v3Velocity);
                if (fContactTime > 0.0f && IsInsideNetArea(v3Position))
                {
                    const nlVector3 v3Down = { 0.0f, 0.0f, -1.0f };
                    nlVector3 v3Normal;
                    nlVec3Normalize(v3Normal, v3Velocity);
                    unsigned short aAngle = DegreesToAngle(lbl_806DBBAC);
                    float fCos = nlSin(aAngle + 0x4000);
                    if (nlVec3DotProduct(v3Normal, v3Down) > fCos)
                    {
                        float fHeight;
                        fTime = FakeBallWorld::GetPredictedHeightLimitTime(2.5f, lbl_806DBBB0,
                            v3Position, v3Velocity, fHeight, true);
                        fn_8016F06C();
                        if (fHeight < 3.0f)
                            return false;
                        if (fTime > lbl_806DBBB0)
                        {
                            mbShouldMiss = bShouldMiss;
                            fn_8008CED8(fTime, v3Position, v3Velocity);
                            return true;
                        }
                        return false;
                    }
                    fn_8016F06C();
                    return false;
                }
                fn_8016F06C();
            }
        }
        float fCatchRange = 2.0f + lbl_806DBB30;
        if (nlVec3DistanceSquared2D(m_pTeam->m_pNet->m_v3NetLocation, v3LandingPos)
            < nlGetLengthSquared1D(fCatchRange))
        {
            nlVector3 v3Position;
            nlVector3 v3Velocity;
            float fHeight;
            float fLimit = 1.5f;
            fn_8016EEC8();
            float fContactTime = FakeBallWorld::GetPredictedHeightLimitTime(fLimit, lbl_806DBBB0,
                v3Position, v3Velocity, fHeight, true);
            fn_8016F06C();
            if (fHeight < 3.0f)
                return false;
            if (fContactTime > lbl_806DBBB0
                && nlVec3DistanceSquared2D(m_pTeam->m_pNet->m_v3NetLocation, v3Position)
                    < nlGetLengthSquared1D(lbl_806DBB30))
            {
                const nlVector3 v3Down = { 0.0f, 0.0f, -1.0f };
                nlVector3 v3Normal;
                nlVec3Normalize(v3Normal, v3Velocity);
                unsigned short aAngle = DegreesToAngle(lbl_806DBBAC);
                float fCos = nlSin(aAngle + 0x4000);
                if (nlVec3DotProduct(v3Normal, v3Down) > fCos)
                {
                    mbShouldMiss = bShouldMiss;
                    fn_8008CED8(fContactTime, v3Position, v3Velocity);
                    return true;
                }
            }
        }
    }
    return false;
}
