#include "Game/Goalie.h"

#include "Game/AI/AiUtil.h"
#include "Game/AI/AIPad.h"
#include "Game/AI/Fielder.h"
#include "Game/AI/FielderActions.h"
#include "Game/AI/FilteredRandom.h"
#include "Game/AI/GoalieLooseBall.h"
#include "Game/AI/Powerups.h"
#include "Game/AnimInventory.h"
#include "Game/Ball.h"
#include "Game/BallTrail.h"
#include "Game/BaseGameSceneManager.h"
#include "Game/CharacterTriggers.h"
#include "Game/CharacterTweaks.h"
#include "Game/Field.h"
#include "Game/Effects/EmissionController.h"
#include "Game/MathHelpers.h"
#include "Game/Net.h"
#include "Game/Physics/PhysicsCharacter.h"
#include "Game/Physics/PhysicsBall.h"
#include "Game/Physics/PhysicsFakeBall.h"
#include "Game/SAnim/pnBlender.h"
#include "Game/SAnim/pnSAnimController.h"
#include "Game/SAnim/pnFeather.h"
#include "Game/SAnim/pnSingleAxisBlender.h"
#include "Game/Team.h"
#include "NL/globalpad.h"
#include "math.h"

static const nlVector3 v3Zero = { 0.0f, 0.0f, 0.0f };
extern const nlVector3 lbl_804DC054 = { 0.0f, 0.0f, 1.0f };
static int gOffplayDejected[5] = { 0x99, 0x9A, 0x9B, 0x9C, 0x9D };

struct UnidentifiedGoalieActionState
{
    u8 mUnidentified00[0x18];
    int mUnidentified18;
    u8 mUnidentified1C[0x04];
    bool mUnidentified20;
    u8 mUnidentified21[0x07];
    unsigned int mUnidentified28;
    unsigned int mUnidentified2C;
    unsigned int mUnidentified30;
};

struct UnidentifiedGoalieSkillTweaks
{
    u8 mUnidentified000[0x194];
    float* mpLooseBallChaseDistance;
};

struct UnidentifiedPowerupManager
{
    u8 mUnidentified00[0x2C];
    PowerupBase* mUnidentified2C;
};

struct UnidentifiedFESceneState
{
    u8 mUnidentified000[0x74];
    int mUnidentified074;
};

struct UnidentifiedMegaBallState
{
    u8 mUnidentified000[0x1C];
    unsigned int mUnidentified01C;
    u8 mUnidentified020[0x08];
    bool mUnidentified028;
    bool mUnidentified029;
    u8 mUnidentified02A[0x3E];
    bool mUnidentified068;
};

extern UnidentifiedGoalieActionState* lbl_806E0C94;
extern cCharacter* lbl_806E0C34;
extern float lbl_806DBB08;
extern float lbl_806DBBE0;
extern float lbl_806DBBE4;
extern float lbl_806DBBE8;
extern float lbl_806DBBEC;
extern float lbl_806DBC2C;
extern float lbl_806DBC30;
extern float lbl_806DBC34;
extern float lbl_806DBC60;
extern float lbl_806DBC64;
extern float lbl_806DBC68;
extern float gfRepositionThreshold;
extern float lbl_806DBC6C;
extern float lbl_806DBC70;
extern float lbl_806DBC80;
extern float lbl_806DBC84;
extern float lbl_806DBC88;
extern float lbl_806DBC8C;
extern unsigned char lbl_806DBC90;
extern float lbl_806DBC94;
extern float lbl_806DBC98;
extern float lbl_806DBCA0;
extern float lbl_806DBCA4;
extern float lbl_806DBCA8;
extern float lbl_806DBCAC;
extern float lbl_806DBCB0;
extern float lbl_806DBCB4;
extern float lbl_806DBCB8;
extern float lbl_806DBCBC;
extern float lbl_806DBCC0;
extern float lbl_806DBCC4;
extern float lbl_806DBCC8;
extern float lbl_806DBCCC;
extern float lbl_806DBCD0;
extern float lbl_806DBCDC;
extern float lbl_806DBCE0;
extern float lbl_806DBCE4;
extern float lbl_806DBCE8;
extern float lbl_806DBCEC;
extern int lbl_806DBCF0;
extern unsigned int lbl_806DBCF4;
extern float lbl_806DBCF8;
extern float lbl_806DBCFC;
extern float lbl_806DBD00;
extern float lbl_806DBD04;
extern float lbl_806DBD08;
extern float lbl_806DBD0C;
extern float lbl_806DBD10;
extern float lbl_806DBD14;
extern float lbl_806DBD18;
extern float lbl_806DBD1C;
extern float lbl_806DBD20;
extern float lbl_806DBD24;
extern float lbl_806DBD28;
extern float lbl_806DBD2C;
extern float lbl_806DBD30;
extern float lbl_806DBD34;
extern float lbl_806DBD38;
extern float lbl_806DBD3C;
extern float lbl_806DBD40;
extern float lbl_806DBD44;
extern float lbl_806E3A3C;
extern unsigned char lbl_806E0D10;
extern unsigned char lbl_806E0D11;
extern unsigned char lbl_806E0D18;
extern unsigned char lbl_806E0D19;
extern unsigned char lbl_806E0D1A;
extern unsigned char lbl_806E0D22;
extern void* lbl_806E0E00;
extern nlVector4 lbl_8056D3B0;
extern unsigned char lbl_806E0D20;
extern unsigned char lbl_806E0D21;
extern UnidentifiedPowerupManager* lbl_806E1608;
extern BaseGameSceneManager* lbl_806E1860;
extern void* lbl_806E20D8;

extern "C" void fn_800797DC(
    Goalie* pGoalie, int nParam, float fDeltaT, float fParam);
extern "C" void fn_8007B680(Goalie* pGoalie, bool bParam);
extern "C" void fn_8007F430(Goalie* pGoalie);
extern "C" void fn_80013898(cBall* pBall);
extern "C" float fn_800776B4();
extern "C" float fn_800E02B8(cTeam* pTeam);
extern "C" void fn_8005DB44(
    UnidentifiedGoalieActionState* pState, unsigned int nParam, bool bParam);
extern "C" LiveBallTrail* fn_8001B284(unsigned int nIndex);
extern "C" EmissionController* fn_801B64E8(
    Goalie* pGoalie, const char* szEffectName, int nParam);
extern "C" void fn_8001AA0C(LiveBallTrail* pBallTrail, bool bParam);
extern "C" UnidentifiedMegaBallState* fn_801A7620(unsigned int nIndex);
extern "C" void fn_801A6B64(
    UnidentifiedMegaBallState* pState, bool bParam);
extern "C" void fn_801A6D44(UnidentifiedMegaBallState* pState,
    bool bParam, float fParam1, float fParam2, float fParam3, float fParam4);
extern "C" void fn_801A6DC4(UnidentifiedMegaBallState* pState);
extern "C" UnidentifiedMegaBallState* fn_801A76BC(unsigned int nIndex);
extern "C" void fn_801A7610(UnidentifiedMegaBallState* pState);
extern "C" float fn_801A78B8(UnidentifiedMegaBallState* pState1,
    UnidentifiedMegaBallState* pState2);
extern "C" void fn_800EDCE8(cPlayer* pPlayer);
extern "C" void fn_801B93E8(cCharacter* pCharacter);
extern "C" bool fn_8007BF68(Goalie* pGoalie, bool bParam);
extern "C" bool fn_8007C590(Goalie* pGoalie);
extern "C" bool fn_8007D644(Goalie* pGoalie);
extern "C" bool fn_8003EA6C(cFielder* pFielder);
extern "C" void fn_8007EA90(Goalie* pGoalie);
extern "C" void fn_8007E940(Goalie* pGoalie, float fFudgeDist);
extern "C" float fn_8007ECB4(Goalie* pGoalie, float fTimeToContact,
    unsigned int uSaveType, bool bFromTakeoff, bool bFindFailSave);
extern "C" float fn_8007EDF4(Goalie* pGoalie, float fParam);
extern "C" bool fn_8007B9A0(
    Goalie* pGoalie, const nlVector3& v3Position, float fParam);
extern "C" bool fn_8007B9EC(
    Goalie* pGoalie, const nlVector3& v3Position);
extern "C" bool fn_8007B214(Goalie* pGoalie);
extern "C" float fn_8007ACB8(Goalie* pGoalie,
    const nlVector3& v3TargetPosition, float fParam1, float fParam2);
extern "C" float fn_8007BEEC(Goalie* pGoalie, cFielder* pTarget);
extern "C" void fn_800809D0(
    Goalie* pGoalie, cFielder* pTarget, bool bParam);
extern "C" void fn_8001EF78(cCharacter* pCharacter, float fParam);
extern "C" bool fn_8007C904(
    Goalie* pGoalie, const nlVector3& v3Position);
extern "C" void fn_80015C38(cBall* pBall, int nParam);
extern "C" bool fn_80016768(cBall* pBall);
extern "C" float fn_800156A8(cBall* pBall);
extern "C" void fn_8007DCD8(Goalie* pGoalie, bool bParam);
extern "C" void fn_8007EB90(Goalie* pGoalie);
extern "C" bool fn_800976C4(Goalie* pGoalie);
extern "C" void fn_80098098(Goalie* pGoalie);
extern "C" void fn_8004F204(cFielder* pFielder);
extern "C" void fn_8005E408(
    void* pManager, const PlayerAttackData* pData);
extern "C" void fn_8005E604(
    void* pManager, const PlayerAttackData* pData);
extern "C" void fn_8005E800(
    void* pManager, const PlayerAttackData* pData);
extern "C" void fn_80080638(
    Goalie* pGoalie, cFielder* pFielder, bool bParam);
extern "C" void fn_80080BFC(Goalie* pGoalie, float fDeltaT);
extern "C" void fn_800EBBFC(
    unsigned int nParam0, unsigned int nParam1, int nParam2, int nParam3);
extern "C" bool fn_80331C04(
    cGlobalPad* pGlobalPad, int nButton, bool bRemap);
extern "C" void fn_80098468(cPlayer* pPlayer, float fParam);
extern "C" void fn_80097574(
    cPlayer* pPlayer, int nNodeIndex, int nAnimID, float fParam);
extern "C" void fn_8009591C(cPlayer* pPlayer, bool bParam);
extern "C" cFielder* fn_8009664C(
    Goalie* pGoalie, const nlVector3& v3Position, bool bParam);
extern "C" UnidentifiedGoalieSkillTweaks* fn_800A636C(void* pTeam);
extern "C" cPlayer* fn_800A6A84(cTeam* pTeam, int nIndex);
extern "C" void fn_80139D1C(int nParam, void* pParam);
extern "C" void fn_801BABEC(cPlayer* pPlayer);
extern "C" void fn_801BAF0C(cPlayer* pPlayer);
extern "C" int fn_80338BF0(void* pParam);
extern "C" bool fn_8003E7F8(cFielder* pFielder);
extern "C" bool fn_8003E84C(cFielder* pFielder);
extern "C" void fn_8007EB5C(Goalie* pGoalie);
extern "C" SaveData* fn_80093848(bool bParam);
extern "C" float fn_8016D52C(float fHeight, float fEndTime,
    nlVector3& v3Position, nlVector3& v3Velocity, float& fTargetHeight,
    bool bParam);
extern "C" bool fn_8016EA10(const nlVector3& v3Position,
    float fSpeed1, float fSpeed2, nlVector3& v3TargetPosition,
    nlVector3& v3TargetVelocity, float& fInterceptTime,
    float& fClosestDistance, float fMaxTime);
extern "C" SaveData* fn_800925C0(
    SaveBlendInfo* pBlendInfo, const nlVector3* pLocalPosition);
extern "C" SaveData* fn_80092644(SaveData* pSaveData,
    SaveBlendInfo* pBlendInfo, const nlVector3* pLocalPosition);
extern "C" SaveData* fn_80093780(int nAnimID);
extern "C" void fn_800156F8(cBall* pBall, cPlayer* pPlayer);
extern "C" void fn_80097358(cPlayer* pPlayer, float fParam);
extern "C" float fn_800DEB04(cFielder* pFielder);
extern "C" void fn_8016EEC8();
extern "C" void fn_8016F06C();
extern "C" void fn_801B968C(cCharacter* pCharacter);
extern "C" void fn_8008CED8(Goalie* pGoalie, float fTargetTime,
    const nlVector3& v3TargetPosition,
    const nlVector3& v3TargetVelocity);
extern "C" void fn_8008685C(Goalie* pGoalie, float fDeltaT);

float OpenTo(cPlayer* pFromFielder, cPlayer* pToFielder);

inline void Goalie::InitActionPassInterceptSave()
{
    SetGoalieAction(GOALIEACTION_SAVE, 0);
    PlayBlendedAnims(mBlendInfo.mfStartTime, 1.5f, -1);
    m_pPhysicsCharacter->m_CanCollideWithBall = true;
    mnOffplayPending = GOALIE_OFFPLAY_NONE;
    mbBallImpacted = false;
    mbIsDown = true;
    MakeExertEvent();
}

inline void Goalie::InitActionPursueBallCarrier()
{
    SetGoalieAction(GOALIEACTION_PURSUE_BALL_CARRIER, 0);
    mpLooseBallInfo = &LooseBallAnims::mTrapBallInfo;
    mbPlayMiss = false;
}

inline void Goalie::InitActionPursueBallPounce()
{
    SetGoalieAction(GOALIEACTION_PURSUE_BALL_POUNCE, 0);
    SetAnimState(mpLooseBallInfo->mnAnimID, true, 0.2f, false, false);

    GoalieTweaks* pTweaks = (GoalieTweaks*)m_pTweaks;
    InitMovementFromAnimSeek(pTweaks->fRunningDirectionSeekSpeed,
        pTweaks->fRunningDirectionSeekFalloff);

    mbPickedUp = false;
    mbIsDown = true;
}

inline void Goalie::InitActionPursueRecover()
{
    SetGoalieAction(GOALIEACTION_DIVE_RECOVER, 0);

    int animID = 0x98;
    if (m_pBall == 0)
    {
        animID = 0x97;
    }

    SetAnimState(animID, true, 0.2f, false, false);
    InitMovementFromAnim(0, v3Zero, 1.0f, false);

    mbPickedUp = false;
    mbIsDown = true;
}

inline void Goalie::InitActionLooseBallCatch()
{
    SetGoalieAction(GOALIEACTION_LOOSEBALL_CATCH, 0);
    mv3LocalContactPosition.x = 0.2f;
    mbIsDown = true;
    mpSaveData = GoalieSave::FindBestSave(mBlendInfo,
        mv3LocalContactPosition,
        mv3LocalContactVelocity,
        mfTargetTime,
        false,
        0x80001,
        true);
    mpLooseBallInfo = 0;
    mMoveDirection = GOALIEDIR_IDLE;

    if (mpSaveData == 0)
    {
        InitActionLooseBallSetup();
    }
}

void Goalie::ActionLooseBallCatch(float deltaTime)
{
    float fMilestoneTime;
    mfTargetTime -= deltaTime;

    if (m_eAnimID == 5)
    {
        fMilestoneTime = mBlendInfo.mfMilestoneTime[2];
        float targetTime = mfTargetTime;
        if (targetTime <= fMilestoneTime + 0.01f)
        {
            float clampedValue = fMilestoneTime - targetTime;
            clampedValue
                = nlMaxEquals(clampedValue, mBlendInfo.mfStartTime);
            PlayBlendedAnims(clampedValue, 1.5f, -1);
        }
    }
    else
    {
        if (mpSaveData == 0
            || m_pCurrentAnimController->m_fTime > 0.95f)
        {
            if (m_pBall == 0)
            {
                InitActionMove(false);
                return;
            }
            InitActionMoveWB();
            return;
        }

        if (g_pBall->m_pOwner != 0)
        {
            return;
        }

        if (!m_pCurrentAnimController->TestTrigger(
                mpSaveData->mfMilestonePercent[2]))
        {
            return;
        }

        const nlVector3& leftHandPos
            = GetJointPosition(m_nLeftHandJointIndex);
        const nlVector3& rightHandPos
            = GetJointPosition(m_nRightHandJointIndex);

        float distSqLeft
            = CalculateDistanceSquared(g_pBall->m_v3Position, leftHandPos);
        if (distSqLeft < 1.0f
            || CalculateDistanceSquared(
                   g_pBall->m_v3Position, rightHandPos)
                < 1.0f)
        {
            PickupBall(g_pBall);
            m_pPhysicsCharacter->m_CanCollideWithGoalLine = true;
            m_pPhysicsCharacter->m_CanCollideWithWall = true;
            mbPickedUp = true;
            EmitGoalieCatch(this, "goalie_catch", false);
        }
    }
}

void Goalie::ActionLooseBallDesperate(float fDeltaT)
{
    cBall* pBall = g_pBall;
    const nlVector3& v3BallPosition = pBall->m_v3Position;
    int animID = m_eAnimID;
    const LooseBallInfo* pInfo = mpLooseBallInfo;
    nlVector3 v3GuessBallPos;
    nlVector3 v3GuessBallPosElse;
    nlVector2 v2CurrentDelta;
    nlVector2 v2GuessDelta;

    if (pInfo->mnAnimID == animID)
    {
        cPN_SAnimController* pAnim = m_pCurrentAnimController;
        bool bAnimDone = false;
        if (pAnim->m_ePlayMode == PM_HOLD && pAnim->m_fTime == 1.0f)
        {
            bAnimDone = true;
        }

        if (bAnimDone)
        {
            if (animID == 0x85)
            {
                InitActionPursueRecover();
                return;
            }
            if (m_pBall == 0)
            {
                InitActionMove(false);
                return;
            }
            InitActionMoveWB();
            return;
        }

        if (pBall->m_pOwner == 0)
        {
            float fPickupTime = pInfo->mfPickupTime;
            float fAnimTime = pAnim->m_fTime;
            if (fAnimTime < fPickupTime)
            {
                bool bWallBlocked = mfWallBlock > 0.0f;
                if (bWallBlocked)
                {
                    return;
                }

                float fRatio = fAnimTime / fPickupTime;
                float fPickupDuration
                    = fPickupTime * pInfo->mfAnimDuration;
                float fTimeUntilPickup = fPickupDuration
                                       - pInfo->mfAnimDuration
                                           * fAnimTime;
                float fGoalLineX = cField::GetGoalLineX(1U);
                float fTimeScale = fTimeUntilPickup * lbl_806DBC84;
                float fLimit = fGoalLineX - 0.2f;

                nlVec3ScaleAdd(v3GuessBallPos, fTimeScale,
                    g_pBall->m_v3Velocity, v3BallPosition);
                if ((float)fabs(v3GuessBallPos.x) > fLimit)
                {
                    float fClampedX;
                    if (v3GuessBallPos.x > 0.0f)
                    {
                        fClampedX = fLimit;
                    }
                    else
                    {
                        fClampedX = -fLimit;
                    }
                    if ((float)fabs(v3BallPosition.x) < fLimit)
                    {
                        float fBallX = pBall->m_v3Position.x;
                        float fBallY = pBall->m_v3Position.y;
                        float fDX = fBallX - fClampedX;
                        float fDY = fBallY - v3GuessBallPos.y;
                        float fDXOrig = fBallX - v3GuessBallPos.x;
                        float fNewY = fBallY - (fDX * fDY) / fDXOrig;
                        v3GuessBallPos.y = fNewY;
                    }
                    v3GuessBallPos.x = fClampedX;
                }
                TrackTarget(v3GuessBallPos, fRatio,
                    fDeltaT * lbl_806DBC88);
                CheckForLimbEndZoneCollision();
                return;
            }

            const nlVector3& v3BallJoint
                = GetJointPosition(m_nBallJointIndex);
            if (CalculateDistanceSquared(
                    pBall->m_v3Position, v3BallJoint)
                < 0.36f)
            {
                fn_8007B680(this, false);
            }
            return;
        }

        if (m_pBall != 0)
        {
            return;
        }
        SetGoalieAction(GOALIEACTION_PURSUE_BALL_POUNCE, 0);
        mbPlayMiss = false;
        mbIsDown = true;
        return;
    }

    if (muBallDeflectCount != pBall->m_bBallDeflectCount
        || mnOffplayPending != GOALIE_OFFPLAY_NONE
        || pBall->m_pOwner != 0)
    {
        InitActionMove(false);
        return;
    }

    mfTargetTime = mfTargetTime - fDeltaT;
    fn_800797DC(this, 2, fDeltaT, 0.0f);
    CheckForLimbEndZoneCollision();

    const LooseBallInfo* pInfoE = mpLooseBallInfo;
    cBall* pBallE = g_pBall;
    float fCatchRadSq;
    float fCatchRadius = 1.0f + pInfoE->mfPickupDistance;
    float fPickupTimeE = pInfoE->mfPickupTime;
    float fAnimDurE = pInfoE->mfAnimDuration;
    float fTimeProduct = fPickupTimeE * fAnimDurE;
    fCatchRadSq = fCatchRadius * fCatchRadius;
    nlVec3ScaleAdd(v3GuessBallPosElse, fTimeProduct,
        pBallE->m_v3Velocity, pBall->m_v3Position);

    if (mfTargetTime < 0.02f
        || (float)fabs(pBall->m_v3Position.x)
            > cField::GetGoalLineX(1U) - 1.0f
        || (nlVec2Set(v2CurrentDelta,
                m_v3Position.x - pBall->m_v3Position.x,
                m_v3Position.y - pBall->m_v3Position.y),
               nlVec2LengthSquared(v2CurrentDelta))
            < fCatchRadSq
        || (nlVec2Set(v2GuessDelta,
                m_v3Position.x - v3GuessBallPosElse.x,
                m_v3Position.y - v3GuessBallPosElse.y),
               nlVec2LengthSquared(v2GuessDelta))
            < fCatchRadSq)
    {
        PlayNewAnim(mpLooseBallInfo->mnAnimID);
        InitMovementFromAnim(0, v3Zero, 1.0f, false);
    }
}

void Goalie::ActionLooseBallPickup(float fDeltaT)
{
    float fTimeLeft = m_pCurrentAnimController->m_fTime;

    if (fTimeLeft > 0.97f)
    {
        if (g_pBall->m_pOwner != this && mfWaitTime > 0.0f
            && mpLooseBallInfo->mAnimType != LOOSEBALL_ANIM_KICK)
        {
            m_tNoPickupTimer.SetSeconds(0.0f);
        }
        else
        {
            if (m_eAnimID == 0x85)
            {
                InitActionPursueRecover();
                return;
            }

            if (m_pBall == 0)
            {
                InitActionMove(false);
                return;
            }

            InitActionMoveWB();
            return;
        }
    }

    if (m_pBall == 0)
    {
        bool bWallBlock = mfWallBlock > 0.0f;
        if (bWallBlock)
        {
            InitActionMove(false);
            return;
        }
    }

    if (mpLooseBallInfo->mAnimType == LOOSEBALL_ANIM_KICK)
    {
        if (mpPassTarget != 0)
        {
            float fDeltaX
                = mpPassTarget->m_v3Position.x - m_v3Position.x;
            float fDeltaY
                = mpPassTarget->m_v3Position.y - m_v3Position.y;
            float fAngle = nlATan2f(fDeltaY, fDeltaX);
            m_aDesiredFacingDirection
                = (u16)(s32)(10430.378f * fAngle);
        }
        else
        {
            unsigned short dir;
            if (m_v3Position.x > 0.0f)
            {
                dir = 0x8000;
            }
            else
            {
                dir = 0;
            }
            m_aDesiredFacingDirection = dir;
        }

        unsigned short aNewFacingDirection = SeekDirection(
            m_aActualFacingDirection,
            m_aDesiredFacingDirection,
            150000.0f,
            2000.0f,
            fDeltaT);
        SetFacingDirection(aNewFacingDirection, true);
    }

    bool bUnidentifiedCondition = true;
    bool bActionStateActive = false;
    if (lbl_806E0C94->mUnidentified20
        || lbl_806E0C94->mUnidentified18 == 3)
    {
        bActionStateActive = true;
    }

    if (!bActionStateActive
        && mnOffplayPending == GOALIE_OFFPLAY_NONE)
    {
        bUnidentifiedCondition = false;
    }

    if (g_pBall->m_pOwner != this && mfWaitTime > 0.0f
        && !bUnidentifiedCondition)
    {
        TacklePlayer(g_pBall->m_pOwner);
        StealBall(g_pBall->m_pOwner);

        float fNoPickupTime = m_tNoPickupTimer.GetSeconds();
        if (fNoPickupTime > 0.0f)
        {
            const nlVector3& pickupPos
                = GetJointPosition(m_nBallJointIndex);
            nlVector3 v3TargetPos = pickupPos;

            float fGoallineX = cField::GetGoalLineX(1U);
            float fDeltaPos = 0.0f;
            if (v3TargetPos.x > fGoallineX)
            {
                fDeltaPos = fGoallineX - v3TargetPos.x;
            }
            else if (v3TargetPos.x < -fGoallineX)
            {
                fDeltaPos = -fGoallineX - v3TargetPos.x;
            }

            if (fDeltaPos != 0.0f)
            {
                v3TargetPos.x += fDeltaPos;

                nlVector3 v3MyPos = m_v3Position;
                v3MyPos.x += fDeltaPos;
                SetPosition(v3MyPos);
            }

            float fBlend;
            float fPercent = fNoPickupTime / mfWaitTime;
            fBlend = 1.0f - fPercent;
            v3TargetPos.x = fBlend * v3TargetPos.x
                          + fPercent * g_pBall->m_v3Position.x;
            v3TargetPos.y = fBlend * v3TargetPos.y
                          + fPercent * g_pBall->m_v3Position.y;
            v3TargetPos.z = fBlend * v3TargetPos.z
                          + fPercent * g_pBall->m_v3Position.z;
            g_pBall->SetPosition(v3TargetPos);

            nlVector3 v3BallVel = g_pBall->m_v3Velocity;
            float fSpeedSq = v3BallVel.x * v3BallVel.x
                           + v3BallVel.y * v3BallVel.y
                           + v3BallVel.z * v3BallVel.z;
            if (fSpeedSq > 64.0f)
            {
                v3BallVel.x = 0.3f * v3BallVel.x;
                v3BallVel.y = 0.3f * v3BallVel.y;
                v3BallVel.z = 0.3f * v3BallVel.z;
                g_pBall->SetVelocity(
                    v3BallVel, SPINTYPE_NONE, 0);
            }
        }
        else
        {
            PickupBall(g_pBall);
            mbPickedUp = true;
            m_pPhysicsCharacter->m_CanCollideWithGoalLine = true;
            m_pPhysicsCharacter->m_CanCollideWithWall = true;

            if (mUnidentified254 != 0
                && m_eAnimID != 2 && m_eAnimID != 3)
            {
                fn_8007EB90(this);
                InitActionMove(true);
                return;
            }
        }
    }

    if (g_pBall->m_pOwner != 0
        && g_pBall->m_pOwner != this)
    {
        if (bUnidentifiedCondition
            || IsOnSameTeam(g_pBall->m_pOwner))
        {
            InitActionMove(false);
            return;
        }

        SetGoalieAction(GOALIEACTION_PURSUE_BALL_POUNCE, 0);
        mbPlayMiss = false;
        mbIsDown = true;
        return;
    }

    if (IsPassThreat())
    {
        InitActionMove(true);
        return;
    }

    if (m_pBall == 0 && mfWaitTime <= 0.0f)
    {
        if (fTimeLeft >= mpLooseBallInfo->mfPickupTime)
        {
            if (mpLooseBallInfo->mAnimType == LOOSEBALL_ANIM_KICK)
            {
                if (!m_pCurrentAnimController->TestTrigger(
                        mpLooseBallInfo->mfPickupTime))
                {
                    return;
                }

                const nlVector3& pickupPos
                    = GetJointPosition(m_nBallJointIndex);
                if (!bUnidentifiedCondition
                    && (CalculateDistanceSquared(
                            g_pBall->m_v3Position, pickupPos) < 1.0f
                        || CalculateDistanceSquared(
                               g_pBall->m_v3Position, m_v3Position)
                            < 2.25f))
                {
                    fn_8007B214(this);
                    return;
                }

                InitActionMove(true);
                return;
            }

            if (!bUnidentifiedCondition)
            {
                const nlVector3& pickupPos
                    = GetJointPosition(m_nBallJointIndex);
                if (CalculateDistanceSquared(
                        g_pBall->m_v3Position, pickupPos) < 1.0f)
                {
                    fn_8007B214(this);
                }
            }
            return;
        }

        if (bUnidentifiedCondition)
        {
            return;
        }

        float fPercent = (fTimeLeft - mfTargetTime)
                       / (mpLooseBallInfo->mfPickupTime - mfTargetTime);
        fPercent = nlMaxEquals(fPercent, 0.0f);
        fPercent = nlMinEquals(fPercent, 1.0f);

        float fInterpFactor
            = fPercent * (fPercent * ((-2.0f * fPercent) + 3.0f));
        if (!(fInterpFactor < 0.99f))
        {
            return;
        }

        FakeBallWorld::GetPredictedBallPosition(
            mpLooseBallInfo->mfAnimDuration
                * (mpLooseBallInfo->mfPickupTime - fTimeLeft),
            mv3TargetPosition,
            mv3TargetVelocity);
        TrackTarget(mv3TargetPosition,
            fInterpFactor,
            fDeltaT * lbl_806DBC8C);
        CheckForLimbEndZoneCollision();
    }
}

void Goalie::fn_80083750(float)
{
    cPN_SAnimController* pController
        = (cPN_SAnimController*)mUnidentified2F0->GetChild(1);
    bool bAnimDone = false;
    if (pController == 0
        || (bAnimDone = (pController->m_ePlayMode == PM_HOLD
                && pController->m_fTime == 1.0f)))
    {
        fn_801B93E8(this);
        fn_80097648(0.06f);
        mUnidentified1F6 = false;
        mUnidentified1F5 = false;
        mUnidentified1F0 = 0.0f;
        SetGoalieAction(GOALIEACTION_DIVE_RECOVER, 0);
        mbIsDown = true;
        SetAnimState(0x8F, false, 0.0f, false, false);
        InitMovementFromAnim(0, v3Zero, 1.0f, false);
    }
    else
    {
        int nNodeIndex = m_nBip01JointIndex_0xA4;
        float fWeight
            = nlMinEquals(1.0f, pController->m_fTime / 0.2f);
        do
        {
            mUnidentified2F0->SetNodeWeight(nNodeIndex, fWeight);
        } while ((nNodeIndex
                     = m_pPoseAccumulator->m_pHierarchy->GetParent(nNodeIndex))
            >= 0);

        float fZ = mfTargetDist * (1.0f - fWeight);
        nlVector3 v3Position = m_v3Position;
        v3Position.z = fZ;
        SetPosition(v3Position);
        SetVelocity(v3Zero);
    }
}

void Goalie::fn_800838F8(float fDeltaT)
{
    SetVelocity(v3Zero);
    if (mFreezeTimer.Countdown(fDeltaT, 0.0f))
    {
        fn_8007F430(this);
    }
}

void Goalie::fn_80083DE0(float fDeltaT)
{
    if (m_v3Position.z > 0.0f)
    {
        nlVector3 v3Position = m_v3Position;
        v3Position.z -= fDeltaT * lbl_806DBC94;
        if (v3Position.z < 0.0f)
        {
            v3Position.z = 0.0f;
        }
        SetPosition(v3Position);
    }

    CheckForLimbEndZoneCollision();

    cPN_SAnimController* pAnim = m_pCurrentAnimController;
    bool bAnimDone = false;
    if (pAnim->m_ePlayMode == PM_HOLD
        && pAnim->m_fTime == 1.0f)
    {
        bAnimDone = true;
    }
    if (bAnimDone)
    {
        if (m_pBall != 0)
        {
            ReleaseBall(false);
        }
        InitActionMove(true);
    }
}

void Goalie::fn_80084568(unsigned int nIndex, float)
{
    UnidentifiedMegaBallState* pState = fn_801A7620(nIndex);
    LiveBallTrail* pBallTrail = fn_8001B284(nIndex);

    pBallTrail->velocity = v3Zero;
    nlVector3 v3Unidentified2;
    nlVector3 v3Unidentified = v3Zero;
    if (lbl_806E0D19 == true)
    {
        v3Unidentified2 = pBallTrail->mUnidentified028;
        float fInvLength
            = nlRecipSqrt(v3Unidentified2.GetLengthSq3D(), true);
        nlVec3Scale(v3Unidentified2, fInvLength);
        float fUnidentified = 2.0f + nlRandomf(1.0f);
        nlVec3Scale(v3Unidentified2, fUnidentified);
        pBallTrail->mUnidentified028 = v3Unidentified2;
    }
    else
    {
        pBallTrail->mUnidentified028 = v3Unidentified;
    }

    fn_801A6B64(pState, false);

    float fParam;
    if (mfMegaAccuracy < 0.001f)
    {
        fParam = lbl_806DBC2C;
    }
    else if (mfMegaAccuracy < 0.999f)
    {
        fParam = lbl_806DBC30;
    }
    else
    {
        fParam = lbl_806DBC34;
    }

    fn_801A6D44(
        pState, false, fParam, lbl_806DBC64, fParam, lbl_806DBC68);
    if (lbl_806E0D18)
    {
        pState->mUnidentified028 = true;
    }
    pState->mUnidentified029 = true;
}

bool Goalie::fn_80084724(unsigned int nParam, float* pScore)
{
    UnidentifiedMegaBallState* pState = fn_801A7620(nParam);
    if (!pState->mUnidentified029)
    {
        return false;
    }

    *pScore = 0.0f;
    for (unsigned int i = 0; i < 10; i++)
    {
        UnidentifiedMegaBallState* pCandidate = fn_801A76BC(i);
        if (!pCandidate->mUnidentified029)
        {
            continue;
        }

        if (!pCandidate->mUnidentified068)
        {
            fn_800EDCE8(this);
            fn_800EBBFC(0, 0xCC36B742, 0, 0);
            fn_800EBBFC(0, 0x1B662C5F, 0, 0);
            fn_801A7610(pCandidate);
            continue;
        }

        float fCandidateScore = fn_801A78B8(pState, pCandidate);
        if (fCandidateScore > 0.0f)
        {
            if (fCandidateScore > *pScore)
            {
                *pScore = fCandidateScore;
            }
            fn_801A7610(pCandidate);
            return true;
        }
    }
    return false;
}

void Goalie::fn_80084840(UnidentifiedMegaBallState* pState)
{
    LiveBallTrail* pBallTrail = fn_8001B284(pState->mUnidentified01C);
    if (!mbShouldMiss)
    {
        return;
    }

    nlVector3 v3TargetPosition;
    nlVector3 v3Velocity;
    nlVector3 v3Unidentified = v3Zero;
    nlVector3 v3Unidentified2;

    float fDirection;
    if (m_v3Position.x > 0.0f)
    {
        fDirection = 1.0f;
    }
    else
    {
        fDirection = -1.0f;
    }

    float fNetWidth = cNet::m_fNetWidth;
    float fYLimit = 0.5f * fNetWidth - 0.7f;
    v3TargetPosition.x
        = fDirection * (cField::GetGoalLineX(1) + 0.5f);
    float fY = pBallTrail->position.y;
    fY = nlMaxEquals(fY, -fYLimit);
    fY = nlMinEquals(fY, fYLimit);
    v3TargetPosition.y = fY;

    float fZLimit = cNet::m_fNetHeight - 0.8f;
    float fDistance = (float)fabs(
        v3TargetPosition.x - pBallTrail->position.x);
    float fZ = pBallTrail->position.z - 0.7f * fDistance;
    v3TargetPosition.z
        = nlMinEquals(nlMaxEquals(fZ, 0.25f), fZLimit);

    mbCheckForMegaGoal = true;
    fn_800EBBFC(0, 0x5CD383D8, 0, 0);

    nlVec3Sub(v3Velocity, v3TargetPosition, pBallTrail->position);
    nlVec3Scale(v3Velocity, 10.0f);
    pBallTrail->velocity = v3Velocity;

    v3Unidentified2 = lbl_804DC054;
    nlVec3CrossProduct(
        v3Unidentified, v3Unidentified2, v3Velocity);
    float fUnidentified = 2.0f + nlRandomf(1.0f);
    nlVec3Scale(v3Unidentified, fUnidentified);
    pBallTrail->mUnidentified028 = v3Unidentified;

    if (!lbl_806E0D1A)
    {
        v3TargetPosition.x *= 1.5f;
        pBallTrail->position = v3TargetPosition;
    }
}

void Goalie::fn_80084AE0(UnidentifiedMegaBallState* pState)
{
    if (m_pBall == 0)
    {
        LiveBallTrail* pBallTrail
            = fn_8001B284(pState->mUnidentified01C);
        EmissionController* pController
            = fn_801B64E8(this, "mega_ball_explode", 0);
        pController->SetPosition(pBallTrail->position);
        pController->SetVelocity(v3Zero);

        nlVector3 v3Velocity = v3Zero;
        pBallTrail->position = v3Zero;
        pBallTrail->velocity = v3Velocity;
        fn_8001AA0C(pBallTrail, false);

        fn_800EBBFC(0, 0xE335EFF5, 0, 0);
        fn_800EBBFC(0, 0x848EBDEB, 0, 0);
        fn_80139D1C(1, GetGlobalPad());
    }

    for (unsigned int i = 0;
         i < lbl_806E0C94->mUnidentified28; i++)
    {
        UnidentifiedMegaBallState* pCurrentState = fn_801A7620(i);
        if (pCurrentState->mUnidentified029
            && pCurrentState->mUnidentified068)
        {
            fn_801A6DC4(pCurrentState);
            pCurrentState->mUnidentified028 = false;
            pCurrentState->mUnidentified029 = false;
        }
    }
}

void Goalie::fn_80084C3C(bool bParam)
{
    if (bParam)
    {
        fn_80013898(g_pBall);
        g_pBall->m_bBallPathChangeCount = true;
        g_pBall->SetVelocity(v3Zero, SPINTYPE_NONE, 0);
        g_pBall->m_pPhysicsBall->m_bCollideWithGoalies = true;
        g_pBall->m_pPhysicsBall->m_bCollideWithFielders = true;
        g_pBall->m_pPhysicsBall->EnableCollisions();
        g_pBall->m_pPhysicsBall->m_gravity = lbl_806E3A3C;
        PhysicsBall* pPhysicsBall = g_pBall->m_pPhysicsBall;
        pPhysicsBall->mUnidentified054 = false;
        pPhysicsBall->mUnidentified064 = 0.0f;
    }
}

void Goalie::fn_80084CE0()
{
    if (mUnidentified529)
    {
        BaseSceneHandler* pSceneHandler
            = lbl_806E1860->GetScene((SceneList)0x69);
        if (pSceneHandler != 0)
        {
            UnidentifiedFESceneState* pScene
                = (UnidentifiedFESceneState*)pSceneHandler->mFEScene;
            bool bUnidentifiedCondition;
            if (pScene != 0 && pScene->mUnidentified074 == 6)
            {
                bUnidentifiedCondition = true;
            }
            else
            {
                bUnidentifiedCondition = false;
            }
            if (bUnidentifiedCondition)
            {
                lbl_806E1860->Pop();
                mUnidentified529 = false;
            }
        }
    }
}

void Goalie::fn_80084D70(int nCurTarget, float fScore)
{
    muMegaReadyToSave++;
    mUnidentified4D0[nCurTarget] = 2;
    mUnidentified4F8[nCurTarget] = fScore;
}

void Goalie::fn_80084D94(float fParam)
{
    float fTeamValue = fn_800E02B8(m_pTeam);
    float fInterpolated = InterpolateRangeClamped(
        0.0f, 1.0f, 1.0f, 0.2f, fTeamValue);

    float fChance = 0.0f;
    if (fParam < 0.001f)
    {
        fChance += lbl_806DBBE8;
    }
    else if (fParam < 0.999f)
    {
        fChance += lbl_806DBBE4;
    }
    else
    {
        fChance += lbl_806DBBE0;
    }
    fChance += fInterpolated * lbl_806DBBEC;

    for (unsigned int i = 0; i < lbl_806E0C94->mUnidentified28; i++)
    {
        if (nlRandomf(5.0f) < fChance)
        {
            lbl_806E0C94->mUnidentified30++;
            fn_8005DB44(lbl_806E0C94,
                lbl_806E0C94->mUnidentified2C, true);
        }
        else
        {
            fn_8005DB44(lbl_806E0C94,
                lbl_806E0C94->mUnidentified2C, false);
        }
        lbl_806E0C94->mUnidentified2C++;
    }
}

void Goalie::MoveDirectionCB(
    unsigned int nParam, cPN_SingleAxisBlender* blender)
{
    Goalie* pGoalie = (Goalie*)nParam;
    float result = 0.0f;
    if (pGoalie->mv3LocalNavTarget.y < 0.0f)
    {
        result = 1.0f;
    }
    blender->m_fDesiredWeight = result;
}

void Goalie::MoveWeightCB(
    unsigned int nParam, cPN_SingleAxisBlender* blender)
{
    Goalie* pGoalie = (Goalie*)nParam;
    blender->m_fDesiredWeight
        = (s32)(u16)abs_s16(pGoalie->maLocalAngle) / 32768.0f;
}

void Goalie::StrafeSynchronizedSpeedCallback(
    unsigned int nParam, cPN_SAnimController* controller)
{
    Goalie* pGoalie = (Goalie*)nParam;
    controller->m_fPlaybackSpeedScale = pGoalie->mfSpeedScale;
}

void Goalie::ActionSaveSetup(float deltaTime)
{
    float deflectResult = CheckForDelflectAwayFromNet();

    if (deflectResult < 0.0f)
    {
        return;
    }

    if (deflectResult > 0.0f)
    {
        InitActionSaveSetup(false);
        return;
    }

    if (mnOffplayPending != 0)
    {
        InitActionMove(true);
        return;
    }

    mfWaitTime -= deltaTime;
    if (mfWaitTime < lbl_806DBD04)
    {
        InitActionSave();
    }
}

void Goalie::ActionSaveReposition(float deltaTime)
{
    if (mnOffplayPending != GOALIE_OFFPLAY_NONE)
    {
        InitActionMove(true);
        return;
    }

    mfWaitTime -= deltaTime;

    bool shouldReposition = false;
    float closeDistSq
        = gfRepositionThreshold * gfRepositionThreshold;
    float repositionLimit = 1.3f;
    if (mbTryLobSave)
    {
        repositionLimit = 0.8f;
    }
    float repositionLimitSq
        = repositionLimit * repositionLimit;

    nlVector2 distance;
    distance.x = m_v3Position.x - mv3NavTarget.x;
    distance.y = m_v3Position.y - mv3NavTarget.y;
    float distSq = nlVec2LengthSquared(distance);
    if ((distSq < closeDistSq)
        || (distSq > mfTargetDist
            && distSq < repositionLimitSq))
    {
        shouldReposition = true;
    }

    mfTargetDist = distSq;

    float deflectResult = CheckForDelflectAwayFromNet();
    if (deflectResult < 0.0f)
    {
        return;
    }

    if (mfWaitTime <= lbl_806DBD04
        || deflectResult > 0.0f || shouldReposition)
    {
        InitActionSaveSetup(false);
        return;
    }

    if (mfWaitTime < lbl_806DBCBC)
    {
        PlayNewAnim(7);
        GoalieTweaks* pTweaks = (GoalieTweaks*)m_pTweaks;
        InitMovementFromAnimSeek(
            pTweaks->fRunningDirectionSeekSpeed,
            pTweaks->fRunningDirectionSeekFalloff);
        return;
    }

    float ballDx = g_pBall->m_v3Position.x - m_v3Position.x;
    float ballDy = g_pBall->m_v3Position.y - m_v3Position.y;
    float angle = nlATan2f(ballDy, ballDx);
    m_aDesiredFacingDirection
        = (u16)(s32)(10430.378f * angle);

    fn_800797DC(
        this, 0, deltaTime, gfRepositionThreshold);
}

void Goalie::CheckForLimbEndZoneCollision()
{
    nlVector3 v3HeadCopy;
    const nlVector3& v3LHandPos
        = GetJointPosition(m_nHeadJointIndex);
    v3HeadCopy = v3LHandPos;

    float fHeadAdjustment = 0.0f;
    float fAbsX = (float)fabs(v3HeadCopy.x);
    bool bAdjustY = false;
    float fAbsY = (float)fabs(v3HeadCopy.y);
    bool bCanCollideWithGoalLine
        = m_pPhysicsCharacter->m_CanCollideWithGoalLine;
    float fXAdjustment = 0.0f;
    float fYAdjustment = 0.0f;
    float fLimit = cField::GetGoalLineX(1U);
    float fHalf = 0.5f;
    float fNetWidth = cNet::m_fNetWidth;
    fLimit -= fHalf;
    float fNetY = fHalf * fNetWidth;
    float fYLimit = fNetY - fHalf;

    if (fAbsX > fLimit)
    {
        if (fAbsX > fLimit + 2.0f)
        {
            fHeadAdjustment = fAbsX - (fLimit + 2.0f);
        }

        if (fAbsY > fYLimit || bCanCollideWithGoalLine)
        {
            float fXDiff = fAbsX - fLimit;
            float fYDiff = fAbsY - fYLimit;
            if (fXDiff < fYDiff || bCanCollideWithGoalLine)
            {
                fXAdjustment = fXDiff;
            }
            else
            {
                fYAdjustment = fYDiff;
                bAdjustY = true;
            }
        }
        else
        {
            bAdjustY = true;
        }
    }

    const nlVector3& v3RHandPos
        = GetJointPosition(m_nRightHandJointIndex);
    fAbsY = cField::GetGoalLineX(1U) - 0.4f;
    fAbsX = fNetY - 0.4f;
    fn_800883D4(bAdjustY, fXAdjustment, fYAdjustment,
        v3RHandPos, fAbsY, fAbsX);

    const nlVector3& v3LHandPos2
        = GetJointPosition(m_nLeftHandJointIndex);
    fn_800883D4(bAdjustY, fXAdjustment, fYAdjustment,
        v3LHandPos2, fAbsY, fAbsX);

    const nlVector3& v3RFootPos
        = GetJointPosition(m_nRightFootJointIndex);
    fn_800883D4(bAdjustY, fXAdjustment, fYAdjustment,
        v3RFootPos, fAbsY, fAbsX);

    const nlVector3& v3LFootPos
        = GetJointPosition(m_nLeftFootJointIndex);
    fn_800883D4(bAdjustY, fXAdjustment, fYAdjustment,
        v3LFootPos, fAbsY, fAbsX);

    fXAdjustment += fHeadAdjustment;
    if (fXAdjustment > 0.0f || fYAdjustment > 0.0f)
    {
        nlVector3 v3AdjPos = m_v3Position;
        if (v3AdjPos.x > 0.0f)
        {
            fXAdjustment *= -1.0f;
        }
        if (v3AdjPos.y > 0.0f)
        {
            fYAdjustment *= -1.0f;
        }
        v3AdjPos.x
            = v3AdjPos.x + (fXAdjustment + fHeadAdjustment);
        v3AdjPos.y += fYAdjustment;
        SetPosition(v3AdjPos);
    }
}

void Goalie::fn_800883D4(bool& bAdjustY,
    float& fXAdjustment, float& fYAdjustment,
    const nlVector3& v3JointPosition,
    float fXLimit, float fYLimit)
{
    float fAbsX = (float)fabs(v3JointPosition.x);
    float fAbsY = (float)fabs(v3JointPosition.y);
    bool bCanCollideWithGoalLine
        = m_pPhysicsCharacter->m_CanCollideWithGoalLine;

    if (fAbsX > fXLimit)
    {
        if (fAbsY > fYLimit || bCanCollideWithGoalLine)
        {
            float fYDiff = fAbsY - fYLimit;
            if (bAdjustY)
            {
                if (fYDiff > fYAdjustment)
                {
                    fYAdjustment = fYDiff;
                }
                return;
            }

            float fXDiff = fAbsX - fXLimit;
            if (fXAdjustment > 0.0f
                || fXDiff < fYDiff || bCanCollideWithGoalLine)
            {
                if (fXDiff > fXAdjustment)
                {
                    fXAdjustment = fXDiff;
                }
                return;
            }

            fYAdjustment = fYDiff;
            bAdjustY = true;
        }
    }
}

void Goalie::ActionSave(float fDeltaT)
{
    bool bState26
        = mGoalieActionState == GOALIEACTION_UNIDENTIFIED_26;
    CheckForLimbEndZoneCollision();

    SaveData* pSaveData = mpSaveData;
    float fTakeoffTime = pSaveData->mfMilestonePercent[1];
    float fCrouchTime = pSaveData->mfMilestonePercent[0];
    float fDX = m_pCurrentAnimController->m_fTime;

    if (fTakeoffTime <= 0.0f)
    {
        float fGoalTime = pSaveData->mfMilestonePercent[2];
        fTakeoffTime = 0.7f * fGoalTime;
        fCrouchTime = 0.4f * fGoalTime;
    }

    if (!bState26
        && fDX <= fTakeoffTime && m_pBall == 0)
    {
        float deflectResult = CheckForDelflectAwayFromNet();
        if (deflectResult < 0.0f)
        {
            return;
        }
        if (deflectResult > 0.0f)
        {
            if (fDX < fCrouchTime)
            {
                mGoalieActionState = GOALIEACTION_SAVE_REPOSITION;
            }
            else
            {
                mGoalieActionState = GOALIEACTION_PRE_CROUCH;
            }
            mbTryLobSave = false;
            InitActionSaveSetup(false);
            return;
        }
    }

    if (m_pBall != 0
        && mpSaveData->muSaveType == 4 && mbPickedUp)
    {
        if (++mBallsLaunched <= lbl_806DBCF4)
        {
            if (m_pBall != 0
                && mBallsLaunched == lbl_806DBCF4)
            {
                fn_8008DEF4(1.0f);
            }
        }
    }

    if (mbDoHeadTrack)
    {
        nlVector3 v3BallDir;
        nlVector3 v3Facing;
        v3Facing.z = m_m4WorldMatrix.e2[0][2];
        v3Facing.y = m_m4WorldMatrix.e2[0][1];
        v3Facing.x = m_m4WorldMatrix.e2[0][0];

        v3BallDir.x
            = g_pBall->m_v3Position.x - m_v3Position.x;
        v3BallDir.y
            = g_pBall->m_v3Position.y - m_v3Position.y;
        v3BallDir.z = 0.0f;
        if (nlVec3LengthSquared(v3BallDir) < 9.0f
            || nlVec3DotProduct(v3BallDir, v3Facing) < 0.0f)
        {
            mbDoHeadTrack = false;
        }
    }

    if (fDX < mpSaveData->mfMilestonePercent[2])
    {
        float t = fDX / mpSaveData->mfMilestonePercent[2];
        t = nlMaxEquals(t, 0.0f);
        t = nlMinEquals(t, 1.0f);
        short delta = (short)(m_aDesiredFacingDirection
                              - m_aActualFacingDirection);
        int adjustedDelta
            = ((int)(1024.0f
                     * (t * (t * ((-2.0f * t) + 3.0f))))
                  * delta)
            / 1024;
        unsigned short newFacing
            = adjustedDelta + m_aActualFacingDirection;
        SetFacingDirection(newFacing, true);
    }

    if (fDX > mpSaveData->mfMilestonePercent[1])
    {
        float fNetWidth = cNet::m_fNetWidth;
        float fBallY = nlAbs(g_pBall->m_v3Position.y);
        float fNetY = 0.5f * fNetWidth;
        if (fBallY < fNetY
            || nlAbs(g_pBall->m_v3Position.x)
                < cField::GetGoalLineX(1U))
        {
            bool bBallThreat = true;
            if (g_pBall->m_tShotTimer.m_uPackedTime == 0)
            {
                bBallThreat = false;
                if ((g_pBall->m_unk_0xA4 == 5
                        || g_pBall->m_unk_0xA4 == 3)
                    && g_pBall->m_pPassTarget != 0)
                {
                    bBallThreat = true;
                }
            }

            if (bBallThreat && g_pBall->m_pOwner != this
                && (mpSaveData->muSaveType & 0x80003) != 0
                && !fn_80016768(g_pBall))
            {
                bool bState8Shot = false;
                if (g_pBall->m_tShotTimer.m_uPackedTime != 0
                    && g_pBall->m_unk_0xA4 == 8)
                {
                    bState8Shot = true;
                }

                if (!bState8Shot
                    || g_pBall->m_bBallPathChangeCount == 0)
                {
                    const nlVector3& v3LHand
                        = GetJointPosition(m_nLeftHandJointIndex);
                    const nlVector3& v3RHand
                        = GetJointPosition(m_nRightHandJointIndex);
                    GoalieTweaks* pTweaks
                        = (GoalieTweaks*)m_pTweaks;
                    float fCatchDistance
                        = pTweaks->fSaveCatchTolerance;
                    float fCatchDistanceSq
                        = fCatchDistance * fCatchDistance;
                    float distSqL = CalculateDistanceSquared(
                        g_pBall->m_v3Position, v3LHand);

                    if (distSqL < fCatchDistanceSq
                        || CalculateDistanceSquared(
                               g_pBall->m_v3Position, v3RHand)
                               < fCatchDistanceSq)
                    {
                        TacklePlayer(g_pBall->m_pOwner);
                        fn_8007DCD8(this, false);
                        PickupBall(g_pBall);
                        m_pPhysicsCharacter->m_CanCollideWithGoalLine
                            = true;
                        m_pPhysicsCharacter->m_CanCollideWithWall
                            = true;
                        EmitGoalieCatch(
                            this, "goalie_catch", false);
                        mbBallImpacted = true;
                    }
                }
            }
        }
    }

    if (bState26)
    {
        return;
    }

    if ((mpSaveData->muSaveType & 0x80001) != 0
        && fDX > mpSaveData->mfMilestonePercent[3]
        && m_pBall == 0)
    {
        InitActionDiveRecover();
        return;
    }

    if (m_pCurrentAnimController->m_fTime > 0.95f)
    {
        InitActionDiveRecover();
    }
}

void Goalie::ActionLooseBallPursueRolling(float deltaTime)
{
    fn_800797DC(this, 1, deltaTime, 0.2f + mfGoalieStepDist);
    CheckForLimbEndZoneCollision();

    bool bWallBlocked = mfWallBlock > 0.0f;
    if (bWallBlocked || (mnOffplayPending)
        || (!IsLooseBallClose(*fn_800A636C(lbl_806E0E00)
                                  ->mpLooseBallChaseDistance))
        || ((g_pBall->m_pOwner != 0)
            && (g_pBall->m_pOwner != this)))
    {
        InitActionMove(true);
        return;
    }

    InitActionLooseBallSetup();
}

void Goalie::ActionLooseBallSetup(float fDeltaT)
{
    bool bWallBlocked = mfWallBlock > 0.0f;
    if (bWallBlocked || (mnOffplayPending)
        || (!IsLooseBallClose(*fn_800A636C(lbl_806E0E00)
                                  ->mpLooseBallChaseDistance))
        || ((g_pBall->m_pOwner != 0)
            && (g_pBall->m_pOwner != this)))
    {
        InitActionMove(true);
        return;
    }

    InitActionLooseBallSetup();
}

void Goalie::RunWeightCB(
    unsigned int nParam, cPN_SingleAxisBlender* blender)
{
    const Goalie* pGoalie = (Goalie*)nParam;

    s16 diff = (s16)(pGoalie->m_aDesiredFacingDirection
                     - pGoalie->m_aActualFacingDirection);

    s16 minClampedDiff;
    if (diff < -0x31C4)
    {
        minClampedDiff = -0x31C4;
    }
    else
    {
        minClampedDiff = diff;
    }

    s16 clampedDiff;
    if (minClampedDiff > 0x31C4)
    {
        clampedDiff = 0x31C4;
    }
    else
    {
        clampedDiff = minClampedDiff;
    }

    blender->m_fDesiredWeight
        = (float)(clampedDiff + 0x31C4) / 25480.0f;
}

void Goalie::fn_80087434(
    unsigned int nParam, cPN_SAnimController* controller)
{
    Goalie* pGoalie = (Goalie*)nParam;
    GoalieTweaks* pTweaks = (GoalieTweaks*)pGoalie->m_pTweaks;
    float fDesiredSpeed = pGoalie->m_fDesiredSpeed;
    controller->m_fPlaybackSpeedScale = InterpolateRangeClamped(
        lbl_806DBD2C, lbl_806DBD30,
        pTweaks->fJoggingSpeed, pTweaks->fRunningSpeed,
        fDesiredSpeed);
}

void Goalie::StartRunBlend()
{
    if (m_eAnimID == 0x1D)
    {
        return;
    }

    int runAnims[] = { 0x1F, 0x1D, 0x1E };
    cPN_SingleAxisBlender* pRunSAB
        = CreateSingleAxisBlender(runAnims, 3, 1, RunWeightCB,
            0.15f, 0, 0.0f);

    cPN_SAnimController* pPrevCtrlr = 0;
    for (int i = 0; i < 3; i++)
    {
        cPN_SAnimController* pCtrlr
            = (cPN_SAnimController*)pRunSAB->GetChild(i);
        if (pPrevCtrlr == 0)
        {
            pCtrlr->m_fSynchronizedWeight = 0.0f;
            pCtrlr->m_pPlaybackSpeedCallback = fn_80087434;
            pCtrlr->m_nPlaybackSpeedCallbackParam
                = (unsigned int)this;
        }
        else
        {
            pCtrlr->m_bIsSynchronized = true;
            pPrevCtrlr->m_pSynchronizedController = pCtrlr;
        }
        pPrevCtrlr = pCtrlr;
    }

    *m_pAILayer = new cPN_Blender(*m_pAILayer, pRunSAB, 0.1f);
    InitMovementFromAnimSeek(60000.0f, 4000.0f);
}

void Goalie::ActionMoveWB(float fDeltaT)
{
    if (mUnidentified254 != 0)
    {
        if (m_pBall != 0)
        {
            fn_8007EB90(this);
        }
        if (!fn_800976C4(this))
        {
            fn_8007EA90(this);
        }
        SetNoPickUpTime(0.4f);
        mbDoHeadTrack = false;
    }

    if (m_pBall == 0)
    {
        InitActionMove(false);
        return;
    }

    if (m_eAnimID == 0x10)
    {
        bool isAnimDone = false;
        cPN_SAnimController* pCtrl = m_pCurrentAnimController;
        if (pCtrl->m_ePlayMode == PM_HOLD)
        {
            if (1.0f == pCtrl->m_fTime)
            {
                isAnimDone = true;
            }
        }
        if (!isAnimDone)
        {
            return;
        }
    }

    if (mnSubstate == 6)
    {
        bool isAnimDone = false;
        cPN_SAnimController* pCtrl = m_pCurrentAnimController;
        if (pCtrl->m_ePlayMode == PM_HOLD)
        {
            if (1.0f == pCtrl->m_fTime)
            {
                isAnimDone = true;
            }
        }
        if (isAnimDone)
        {
            mnSubstate = 0;
        }
        else
        {
            return;
        }
    }

    if (m_pController != 0
        && (mfWaitTime > 0.0f || lbl_806E0D22))
    {
        mfWaitTime -= fDeltaT;

        float stickMag = m_pController->GetMovementStickMagnitude();
        if (stickMag > 0.0f)
        {
            mfTargetTime = 0.0f;

            float penaltyBoxX;
            float penaltyBoxY;
            float goalLineX;
            penaltyBoxY = cField::GetPenaltyBoxY()
                        + lbl_806DBD44 - 0.5f;
            penaltyBoxX
                = cField::GetPenaltyBoxX(1U) - lbl_806DBD44;
            penaltyBoxX = 0.5f + penaltyBoxX;
            goalLineX = cField::GetGoalLineX(1U)
                      - lbl_806DBD40 - 0.5f;

            u16 direction = m_pController->GetMovementStickDirection();
            m_aDesiredFacingDirection = direction;

            GoalieTweaks* pTweaks = (GoalieTweaks*)m_pTweaks;
            float jogging = pTweaks->fJoggingSpeed;
            float running = pTweaks->fRunningSpeed;
            float speedScale = stickMag * (running - jogging);
            m_fDesiredSpeed = jogging + speedScale;

            float posX = m_v3Position.x;
            float posY = m_v3Position.y;
            u16 dir = m_aDesiredFacingDirection;

            if ((float)fabs(posX) < penaltyBoxX)
            {
                if (posX > 0.0f)
                {
                    dir = (u16)(dir + 0x8000);
                }

                u16 d = dir;
                if (d < 0x1C18 || d > 0xE3E7)
                {
                    m_fDesiredSpeed = 0.0f;
                }
                else if (d < 0x43E8)
                {
                    dir = 0x43E8;
                }
                else if (d > 0xBC17)
                {
                    dir = 0xBC17;
                }

                if (posX > 0.0f)
                {
                    dir += 0x8000;
                }
            }
            else if ((float)fabs(posX) > goalLineX)
            {
                if (posX < 0.0f)
                {
                    dir += 0x8000;
                }

                u16 d = dir;
                if (d < 0x1C18 || d > 0xE3E7)
                {
                    m_fDesiredSpeed = 0.0f;
                }
                else if (d < 0x43E8)
                {
                    dir = 0x43E8;
                }
                else if (d > 0xBC17)
                {
                    dir = 0xBC17;
                }

                if (posX < 0.0f)
                {
                    dir += 0x8000;
                }
            }

            if ((float)fabs(posY) > penaltyBoxY)
            {
                if (posY < 0.0f)
                {
                    dir = (u16)(dir + 0x8000);
                }

                u16 d = dir;
                if (d < 0x23E8 || d > 0xFC17)
                {
                    dir = 0xFC17;
                }
                else if (d < 0x5C18)
                {
                    m_fDesiredSpeed = 0.0f;
                }
                else if (d < 0x83E8)
                {
                    dir = 0x83E8;
                }

                if (posY < 0.0f)
                {
                    dir += 0x8000;
                }
            }

            if (m_fDesiredSpeed > 0.0f)
            {
                m_aDesiredFacingDirection = dir;
            }
            else
            {
                m_aDesiredFacingDirection
                    = m_aActualFacingDirection;
            }
        }
        else
        {
            mfTargetTime += fDeltaT;
            m_fDesiredSpeed = 0.0f;
            m_aDesiredFacingDirection = m_aActualFacingDirection;
        }

        {
            bool bClamped = false;

            float fAbsX = (float)fabs(m_v3Position.x);
            if (fAbsX
                < cField::GetPenaltyBoxX(1U) - lbl_806DBD44)
            {
                u16 dirVal;
                if (m_v3Position.x > 0.0f)
                {
                    dirVal = 0;
                }
                else
                {
                    dirVal = 0x8000;
                }
                m_aDesiredFacingDirection = dirVal;
                bClamped = true;
            }
            else
            {
                float fAbsGoalLineX
                    = (float)fabs(m_v3Position.x);
                if (fAbsGoalLineX
                    > cField::GetGoalLineX(1U) - lbl_806DBD40)
                {
                    u16 dirVal;
                    if (m_v3Position.x < 0.0f)
                    {
                        dirVal = 0;
                    }
                    else
                    {
                        dirVal = 0x8000;
                    }
                    m_aDesiredFacingDirection = dirVal;
                    bClamped = true;
                }
            }

            float fAbsY = (float)fabs(m_v3Position.y);
            if (fAbsY
                > cField::GetPenaltyBoxY() + lbl_806DBD44)
            {
                u16 yDir;
                if (m_v3Position.y > 0.0f)
                {
                    yDir = 0xC000;
                }
                else
                {
                    yDir = 0x4000;
                }

                if (bClamped)
                {
                    u16 currentDir = m_aDesiredFacingDirection;
                    s16 diff = (s16)(yDir - currentDir);
                    s16 scaledDiff = (s16)(diff * 0.5f);
                    currentDir = (u16)(currentDir + scaledDiff);
                    m_aDesiredFacingDirection = currentDir;
                }
                else
                {
                    m_aDesiredFacingDirection = yDir;
                }
                bClamped = true;
            }

            if (bClamped && m_fDesiredSpeed < 0.001f)
            {
                GoalieTweaks* pTweaks
                    = (GoalieTweaks*)m_pTweaks;
                m_fDesiredSpeed = pTweaks->fRunningSpeed;
            }
        }

        fn_80098098(this);

        if (GetGlobalPad()->JustPressed(0x1C, true))
        {
            m_eLastPadAction = 0x32;
            InitActionPass(false);
            return;
        }

        if (GetGlobalPad()->JustPressed(0x1B, true))
        {
            m_eLastPadAction = 0x32;
            InitActionPass(true);
            return;
        }

        if (mfTargetTime > 1.0f)
        {
            float x = m_v3Position.x;
            nlVector3 v3Facing;
            nlVector3 v3Center = m_v3Position;
            float m02 = m_m4WorldMatrix.e2[0][2];
            float m01 = m_m4WorldMatrix.e2[0][1];
            float m00 = m_m4WorldMatrix.e2[0][0];
            nlVec3Set(v3Facing, m00, m01, m02);
            float dist = nlSqrt(nlGetLengthSquared3D(
                x, m_v3Position.y, m_v3Position.z), true);
            float invDist = -1.0f / dist;
            v3Center.x = invDist * m_v3Position.x;
            v3Center.y = invDist * m_v3Position.y;
            v3Center.z = invDist * m_v3Position.z;

            float dot = nlVec3DotProduct(v3Facing, v3Center);
            if (dot > 0.5)
            {
                mfTargetTime = 0.0f;
                PlayNewAnim(0x0B);
                InitMovementFromAnim(0, v3Zero, 1.0f, false);
                fn_800EBBFC(9, 0x87F93D32, 0, 0);
            }
        }

        if (m_fDesiredSpeed > 0.01f)
        {
            StartRunBlend();
        }
        else
        {
            if (m_eAnimID == 0x0B)
            {
                return;
            }
            PlayNewAnim(6);
            InitMovementFromAnim(0, v3Zero, 1.0f, false);
        }
        return;
    }

    mfWaitTime = 0.0f;

    switch (mnSubstate)
    {
    case 0:
        mnSubstate = 4;
        return;

    case 5:
    {
        bool isAnimDone = false;
        cPN_SAnimController* pCtrl = m_pCurrentAnimController;
        if (pCtrl->m_ePlayMode == PM_HOLD)
        {
            if (1.0f == pCtrl->m_fTime)
            {
                isAnimDone = true;
            }
        }
        if (!isAnimDone)
        {
            return;
        }
        mnSubstate = 4;
        return;
    }

    case 4:
    {
        float goalLineX = cField::GetGoalLineX(1U);
        float posX = m_v3Position.x;

        if (mbDoNavigate)
        {
            float absX = (float)fabs(posX);
            if (absX < goalLineX - lbl_806DBD38
                || absX > goalLineX - lbl_806DBD34
                || (float)fabs(m_v3Position.y) > lbl_806DBD3C)
            {
                float boundarySpan = lbl_806DBD38 + lbl_806DBD34;
                float targetOffset = 0.5f * boundarySpan;
                float targetMagnitude = goalLineX - targetOffset;
                float targetX;
                if (posX > 0.0f)
                {
                    targetX = targetMagnitude;
                }
                else
                {
                    targetX = -targetMagnitude;
                }

                float angle = nlATan2f(
                    -m_v3Position.y, targetX - posX);
                m_aDesiredFacingDirection
                    = (u16)(s32)(10430.378f * angle);
                GoalieTweaks* pTweaks
                    = (GoalieTweaks*)m_pTweaks;
                m_fDesiredSpeed = pTweaks->fRunningSpeed;
                StartRunBlend();
                return;
            }

            mbDoNavigate = false;
            return;
        }

        float angle = nlATan2f(-m_v3Position.y, -posX);
        m_aDesiredFacingDirection
            = (u16)(s32)(10430.378f * angle);
        u16 diff = (u16)abs_s16((s16)(m_aDesiredFacingDirection
                                     - m_aActualFacingDirection));
        if (diff > 0xDAC)
        {
            GoalieTweaks* pTweaks = (GoalieTweaks*)m_pTweaks;
            m_fDesiredSpeed = pTweaks->fRunningSpeed;
            StartRunBlend();
            return;
        }

        mnSubstate = 7;
        return;
    }

    case 7:
        InitActionPass(true);
        return;

    default:
        return;
    }
}

void Goalie::InitActionSnapBall()
{
    SetGoalieAction(GOALIEACTION_SNAP_BALL, 0);
    SetAnimState(0x86, true, 0.2f, false, false);
    InitMovementFromAnim(0, v3Zero, 1.0f, false);
    mfWaitTime = 0.1f;
    SetNoPickUpTime(mfWaitTime);
    mbDoHeadTrack = false;
    mbIsDown = false;
}

void Goalie::fn_80090858(eGoalieOffplayType offplayType)
{
    mnOffplayPending = offplayType;
    mbPickedUp = false;
}

void Goalie::fn_800908F8()
{
    cPlayer* pShooter = g_pBall->m_pShooter;
    if (pShooter != 0 && g_pBall->m_unk_0xA4 == 8)
    {
        mpSkillShooter = pShooter;
        switch (pShooter->m_eCharacterClass)
        {
        case (eCharacterClass)12:
        case (eCharacterClass)14:
        case (eCharacterClass)15:
        case (eCharacterClass)17:
        case (eCharacterClass)19:
            mbNoUserControl = true;
            break;
        default:
            break;
        }
        return;
    }

    mpSkillShooter = 0;
}

bool Goalie::fn_80090958(bool bParam)
{
    cPlayer* pSkillShooter = mpSkillShooter;
    if (pSkillShooter == 0)
    {
        return false;
    }

    bool bState8Shot;
    cBall* pBall;
    int characterClass = pSkillShooter->m_eCharacterClass;
    if (characterClass == 15)
    {
        pBall = g_pBall;
        bState8Shot = false;
        if (pBall->m_tShotTimer.m_uPackedTime != 0
            && pBall->m_unk_0xA4 == 8)
        {
            bState8Shot = true;
        }

        if (bState8Shot)
        {
            GoalieTweaks* pTweaks = (GoalieTweaks*)m_pTweaks;
            float fParam = InterpolateRangeClamped(
                pTweaks->mUnidentified2A8,
                pTweaks->mUnidentified2B8,
                1.0f, 4.0f, mfBallCharge);
            fn_80097358(this, fParam);
            if (bParam)
            {
                fn_8007EA90(this);
                if (m_pBall != 0)
                {
                    fn_8007EB90(this);
                }
                SetNoPickUpTime(0.4f);
                mbDoHeadTrack = false;
                mpSkillShooter = 0;
                return false;
            }
            return false;
        }
    }

    if (characterClass == 12)
    {
        if (mGoalieActionState == GOALIEACTION_UNIDENTIFIED_32
            && m_pCurrentAnimController->m_fTime < 0.2f)
        {
            mpSkillShooter = 0;
            return false;
        }

        float fParam = lbl_806DBC70;
        mbDoHeadTrack = false;
        SetGoalieAction(GOALIEACTION_UNIDENTIFIED_32, 0);
        if (m_pBall != 0)
        {
            fn_8007EB90(this);
        }
        SetAnimState(0xAE, true, 0.2f, false, false);
        InitMovementFromAnim(0, v3Zero, 1.0f, false);
        if (fParam > 0.0f)
        {
            m_pCurrentAnimController->SetTime(fParam);
            fn_801BAF0C(this);
            fn_801BABEC(this);
        }
        mbIsDown = true;
        m_pPhysicsCharacter->m_CanCollideWithGoalLine = false;
        m_pPhysicsCharacter->m_CanCollideWithBall = false;
    }
    else if (characterClass == 14)
    {
        if (mGoalieActionState == GOALIEACTION_UNIDENTIFIED_25
            && m_pCurrentAnimController->m_fTime < 0.1f)
        {
            mpSkillShooter = 0;
            return false;
        }

        if (m_pBall != 0)
        {
            ReleaseBall(false);
        }

        PowerupBase* pPowerup = lbl_806E1608->mUnidentified2C;
        if (pPowerup != 0 && pPowerup->m_unk20)
        {
            fn_800156F8(g_pBall, mpSkillShooter);
        }
        SetNoPickUpTime(0.2f);

        if (mGoalieActionState == GOALIEACTION_SAVE)
        {
            bool bLeft = !m_pAnimInventory->GetMirrored(m_eAnimID);
            mpSaveData = GoalieSave::GetSTSSpinMissData(bLeft);
            PlayNewAnim(mpSaveData->mnAnimID);
            if (bParam)
            {
                m_pCurrentAnimController->SetTime(0.16853933f);
            }
        }
        else
        {
            PlayNewAnim(0xAB);
        }

        InitMovementFromAnim(0, v3Zero, 1.0f, false);
        mbIsDown = true;
        fn_8007EB5C(this);
        SetGoalieAction(GOALIEACTION_UNIDENTIFIED_25, 0);
        m_pPhysicsCharacter->m_CanCollideWithGoalLine = false;
    }
    else if (characterClass == 17)
    {
        pBall = g_pBall;
        bState8Shot = false;
        if (pBall->m_tShotTimer.m_uPackedTime != 0
            && pBall->m_unk_0xA4 == 8)
        {
            bState8Shot = true;
        }

        if (bState8Shot)
        {
            if (mGoalieActionState == GOALIEACTION_UNIDENTIFIED_27
                && m_pCurrentAnimController->m_fTime < 0.2f)
            {
                mpSkillShooter = 0;
                return false;
            }

            fn_8008E130();
            fn_800156F8(g_pBall, mpSkillShooter);
            SetNoPickUpTime(0.2f);
        }
    }

    fn_800EBBFC(
        mpSkillShooter->mUnidentified318, 0xB721918A, 0, 0);
    mbDoHeadTrack = false;
    mpSkillShooter = 0;
    return true;
}

bool Goalie::IsTeammateHoardingBall()
{
    if (mUnidentified254 == 0)
    {
        cBall* pBall;
        cFielder* pOwner = g_pBall->GetOwnerFielder();
        if (pOwner != 0 && !fn_8003EA6C(pOwner)
            && IsOnSameTeam(pOwner))
        {
            float ownerX;
            float myX = m_v3Position.x;
            ownerX = pOwner->m_v3Position.x;
            pBall = g_pBall;
            if (myX * ownerX > 0.0f)
            {
                float absMyX = (float)fabs(myX);
                float absOwnerX = (float)fabs(ownerX);
                float threshold = absMyX - 0.5f;

                if (absOwnerX > threshold
                    || (float)fabs(pBall->m_v3Position.x) > threshold)
                {
                    float goalieRadius;
                    float ownerRadius;
                    m_pPhysicsCharacter->GetRadius(&goalieRadius);
                    pOwner->m_pPhysicsCharacter->GetRadius(&ownerRadius);
                    float distThresh
                        = goalieRadius + ownerRadius + 0.2f;
                    distThresh *= distThresh;

                    if (nlVec3DistanceSquared2D(
                            m_v3Position, pOwner->m_v3Position)
                            < distThresh
                        || nlVec3DistanceSquared2D(
                               m_v3Position, pBall->m_v3Position)
                            < distThresh)
                    {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

void Goalie::InitActionPostWhistle()
{
    if (m_pBall != 0)
    {
        ReleaseBall(false);
    }

    mnOffplayPending = GOALIE_OFFPLAY_NONE;
    mbPickedUp = false;
    SetAnimState(5, false, 0.2f, false, false);
    InitActionMove(false);
}

void Goalie::ActionDiveRecover(float fDeltaT)
{
    CheckForLimbEndZoneCollision();

    if (m_pBall == 0)
    {
        GoalieTweaks* pTweaks = (GoalieTweaks*)m_pTweaks;
        if (mFatigue.mfEnergyLevel < pTweaks->fGetupEnergyHigh)
        {
            float result = InterpolateRangeClamped(
                pTweaks->fGetupSpeedLow,
                1.0f,
                pTweaks->fGetupEnergyLow,
                pTweaks->fGetupEnergyHigh,
                mFatigue.mfEnergyLevel);
            m_pCurrentAnimController->m_fPlaybackSpeedScale = result;
        }

        if (ShouldStartCrossBlend(5))
        {
            InitActionMove(false);
        }
    }
    else
    {
        if (ShouldStartCrossBlend(6))
        {
            InitActionMoveWB();
        }
    }
}

void Goalie::ActionPass(float deltaTime)
{
    if (m_pBall != 0)
    {
        if (mpPassTarget != 0)
        {
            float dx = mpPassTarget->m_v3Position.x - m_v3Position.x;
            float dy = mpPassTarget->m_v3Position.y - m_v3Position.y;
            float angleRad = nlATan2f(dy, dx);

            m_aDesiredFacingDirection
                = (unsigned short)(s32)(10430.378f * angleRad);
        }
        else
        {
            if (m_pTeam->m_pNet->m_v3NetLocation.x > 0.0f)
            {
                m_aDesiredFacingDirection = 0x8000;
            }
            else
            {
                m_aDesiredFacingDirection = 0;
            }
        }

        GoalieTweaks* pTweaks = (GoalieTweaks*)m_pTweaks;
        float fThrowingDirectionSeekSpeed
            = pTweaks->fThrowingDirectionSeekSpeed;
        float fThrowingDirectionSeekFalloff
            = pTweaks->fThrowingDirectionSeekFalloff;
        unsigned short newFacing = SeekDirection(
            m_aActualFacingDirection,
            m_aDesiredFacingDirection,
            fThrowingDirectionSeekSpeed,
            fThrowingDirectionSeekFalloff,
            deltaTime);
        SetFacingDirection(newFacing, true);
    }
    if (ShouldStartCrossBlend(5))
    {
        InitActionMove(false);
    }
}

void Goalie::ActionPassIntercept(float deltaTime)
{
    if (muBallDeflectCount != g_pBall->m_bBallDeflectCount)
    {
        InitActionMove(true);
        return;
    }

    mfWaitTime -= deltaTime;

    switch (mnSubstate)
    {
    case 1:
        if (mfWaitTime <= 0.02f)
        {
            InitActionPassInterceptSave();
        }
        return;

    case 4:
    {
        u16 ballAngle;
        float dx = mv3TargetPosition.x - m_v3Position.x;
        float dy = mv3TargetPosition.y - m_v3Position.y;
        float angleToTarget = nlATan2f(dy, dx);
        u32 targetAngle = (u16)(s32)(10430.378f * angleToTarget);

        dx = g_pBall->m_v3Position.x - m_v3Position.x;
        dy = g_pBall->m_v3Position.y - m_v3Position.y;
        float angleToBall = nlATan2f(dy, dx);
        ballAngle = (u16)(s32)(10430.378f * angleToBall);

        s16 angleDiff
            = (s16)(targetAngle - m_aActualFacingDirection);
        int animID = ChooseRunAnim(angleDiff, mv3TargetPosition, 1.0f);

        s16 ballAngleDiff = (s16)(ballAngle - targetAngle);
        ballAngleDiff
            = ballAngleDiff < 0 ? -ballAngleDiff : ballAngleDiff;
        u16 absBallAngleDiff = (u16)ballAngleDiff;

        if (absBallAngleDiff > 0x4000 && animID != 5
            && (m_eAnimID == 5 || m_eAnimID == 0x25))
        {
            targetAngle += 0x8000;
        }

        if (mfWaitTime > 0.25f && animID != 5)
        {
            PlayNewAnim(animID);
            InitMovementFromAnim(0, v3Zero, 0.0f, false);

            GoalieTweaks* pTweaks = (GoalieTweaks*)m_pTweaks;
            float fRunningDirectionSeekSpeed
                = pTweaks->fRunningDirectionSeekSpeed;
            float fRunningDirectionSeekFalloff
                = pTweaks->fRunningDirectionSeekFalloff;
            unsigned short newFacing = SeekDirection(
                m_aActualFacingDirection,
                targetAngle,
                fRunningDirectionSeekSpeed,
                fRunningDirectionSeekFalloff,
                deltaTime);
            SetFacingDirection(newFacing, true);
            return;
        }

        if (CanInterceptPass())
        {
            if (mfWaitTime <= 0.02f)
            {
                InitActionPassInterceptSave();
                return;
            }

            mnSubstate = 1;
            PlayNewAnim(5);

            GoalieTweaks* pTweaks = (GoalieTweaks*)m_pTweaks;
            InitMovementFromAnimSeek(pTweaks->fRunningDirectionSeekSpeed,
                pTweaks->fRunningDirectionSeekFalloff);
            return;
        }

        float tmp = GoalieSave::mfCrouchDuration;
        if (g_pBall->m_tPassTargetTimer.GetSeconds() < tmp
            && IsCloseToPlane(
                mv3TargetPosition, m_v3Position, 1.2f))
        {
            if (mGoalieActionState == GOALIEACTION_STS_RECOVER)
            {
                return;
            }

            mbIsDown = false;
            mCrouchType = GOALIECROUCH_PASS;
            SetGoalieAction(GOALIEACTION_PRE_CROUCH, 0);
            PlayNewAnim(0x2C);
            InitMovementFromAnim(0, v3Zero, 0.0f, false);
            return;
        }

        mUrgency = URGENCY_HIGH;
        InitActionMove(true);
    }
    }
}

void Goalie::ActionPreCrouch(float deltaTime)
{
    nlVector3 targetPos = g_pBall->m_v3Position;

    if (!CheckForSTSAttack())
    {
        if (g_pBall->GetOwnerFielder() != 0)
        {
            cFielder* pOwnerFielder = g_pBall->GetOwnerFielder();
            if (IsOnSameTeam((cPlayer*)pOwnerFielder))
            {
                InitActionMove(false);
            }
            else if (IsWithinPounceRange())
            {
                if (!fn_8007D740())
                {
                    InitActionPursueBallCarrier();
                }

                if (!fn_8007D740())
                {
                    InitActionPursueBallPounce();

                    if (g_pBall->GetOwnerFielder() != 0
                        && (g_pBall->GetOwnerFielder()->m_eCharacterClass
                                == (eCharacterClass)0x05
                            || g_pBall->GetOwnerFielder()->m_eCharacterClass
                                   == (eCharacterClass)0x0A
                            || g_pBall->GetOwnerFielder()->m_eCharacterClass
                                   == (eCharacterClass)0x0F)
                        && g_pBall->m_v3Position.z > 0.66f)
                    {
                        mbPlayMiss = true;
                    }
                }
            }
            else
            {
                if (pOwnerFielder->m_eActionState != ACTION_UNKNOWN_15
                    && pOwnerFielder->m_eActionState
                           != ACTION_SHOOT_TO_SCORE
                    && pOwnerFielder->m_eActionState != ACTION_SHOT)
                {
                    InitActionMove(true);
                }
                else if (mCrouchType != GOALIECROUCH_SHOT)
                {
                    InitActionMove(true);
                }
            }
        }
        else if (g_pBall->m_pPassTarget == 0)
        {
            if (mpShooter == 0
                || mpShooter->m_eActionState != ACTION_LOOSE_BALL_SHOT
                || mCrouchType != GOALIECROUCH_LOOSEBALL)
            {
                InitActionMove(true);
            }
        }
        else
        {
            if (mCrouchType != GOALIECROUCH_PASS)
            {
                InitActionMove(true);
            }

            targetPos = g_pBall->m_pPassTarget->m_v3Position;
        }

        if (mGoalieActionState == GOALIEACTION_PRE_CROUCH)
        {
            float dx = targetPos.x - m_v3Position.x;
            float dy = targetPos.y - m_v3Position.y;
            float angle = nlATan2f(dy, dx);

            m_aDesiredFacingDirection
                = (unsigned short)(s32)(10430.378f * angle);

            unsigned short newFacing = SeekDirection(
                m_aActualFacingDirection,
                m_aDesiredFacingDirection,
                75000.0f,
                4000.0f,
                deltaTime);
            SetFacingDirection(newFacing, true);
        }
    }
}

void Goalie::ActionPursueBallCarrier(float fDeltaT)
{
    if (!CheckForSTSAttack())
    {
        if (fn_8007C73C())
        {
            CleanupStun();
            ChooseSwatAnim(1);
            SetGoalieAction((eGoalieActionState)0x1F, 0);
            m_fDesiredSpeed = 0.0f;
            m_fActualSpeed = 0.0f;
            SetVelocity(v3Zero);

            if (m_pBall != 0)
            {
                fn_80139D1C(1, GetGlobalPad());
                ReleaseBall(false);
            }

            SetNoPickUpTime(0.2f);
            if (GetGlobalPad() != 0)
            {
                fn_8009591C(this, false);
            }
            mbGrabMonty = false;
        }
        else
        {
            cFielder* pOwnerFielder = g_pBall->GetOwnerFielder();

            if (mnOffplayPending != 0 || lbl_806E0C94->mUnidentified20
                || lbl_806E0C94->mUnidentified18 == 3
                || pOwnerFielder == 0
                || IsOnSameTeam((cPlayer*)pOwnerFielder)
                || !IsOpponentBallCarrierInRange())
            {
                InitActionMove(true);
                return;
            }

            if (fn_8007BC40())
            {
                return;
            }

            bool bWallBlock = mfWallBlock > 0.0f;
            if (bWallBlock)
            {
                InitActionMove(false);
                return;
            }

            nlVector3& ballPos = g_pBall->m_v3Position;
            GetLocalPoint(mv3LocalContactPosition, ballPos, m_v3Position, m_aActualFacingDirection);

            nlVector3 ballDelta;
            nlVec3Set(ballDelta,
                ballPos.x - m_v3Position.x,
                ballPos.y - m_v3Position.y,
                ballPos.z - m_v3Position.z);

            nlVector3 desiredPos;
            nlVector3 desiredDir;
            nlVector3 desiredOffset;
            unsigned short desiredAngle;
            FindDesiredGoaliePosition(
                desiredPos, desiredDir, desiredOffset, desiredAngle, 0);

            float pickupDistance = mpLooseBallInfo->mfPickupDistance;
            float pounceRange = 0.5f * pickupDistance;
            float ballDistSq = ballDelta.GetLengthSq3D();
            float pickupDistSq = pickupDistance * pickupDistance;
            float pounceRangeSq = pounceRange * pounceRange;
            float thresholdDist;
            if (ballDistSq > pickupDistSq)
            {
                thresholdDist = pickupDistance;
            }
            else if (ballDistSq < pounceRangeSq)
            {
                thresholdDist = pounceRange;
            }
            else
            {
                thresholdDist = nlSqrt(ballDistSq, true);
            }

            float scale
                = -thresholdDist / nlSqrt(desiredDir.GetLengthSq3D(), true);

            float desiredZ = (scale * desiredDir.z) + desiredOffset.z;
            float desiredY = (scale * desiredDir.y) + desiredOffset.y;
            float desiredX = (scale * desiredDir.x) + desiredOffset.x;
            nlVec3Set(desiredPos, desiredX, desiredY, desiredZ);

            nlVector3 moveDir;
            float moveZ = desiredPos.z - m_v3Position.z;
            float moveY = desiredPos.y - m_v3Position.y;
            float moveX = desiredPos.x - m_v3Position.x;
            nlVec3Set(moveDir, moveX, moveY, moveZ);

            float dotProduct = (moveDir.x * ballDelta.x)
                             + (moveDir.y * ballDelta.y) + (moveDir.z * ballDelta.z);

            if (dotProduct > 0.0f)
            {
                ballDelta = moveDir;
            }

            float dx = ballDelta.x;
            float dy = ballDelta.y;
            float angle = nlATan2f(dy, dx);
            m_aDesiredFacingDirection
                = (u16)(s32)(10430.378f * angle);

            float pickupDistanceSq = mpLooseBallInfo->mfPickupDistance
                                   * mpLooseBallInfo->mfPickupDistance;

            nlVector3 opponentLocalPos;
            GetLocalPoint(opponentLocalPos, pOwnerFielder->m_v3Position, m_v3Position, m_aActualFacingDirection);

            nlVector2 dist1Delta;
            dist1Delta.x = mv3LocalContactPosition.x
                         - mpLooseBallInfo->mv3PickupPos.x;
            dist1Delta.y = mv3LocalContactPosition.y
                         - mpLooseBallInfo->mv3PickupPos.y;
            float dist1Sq = (dist1Delta.x * dist1Delta.x)
                          + (dist1Delta.y * dist1Delta.y);

            nlVector2 dist2Delta;
            dist2Delta.x
                = opponentLocalPos.x - mpLooseBallInfo->mv3PickupPos.x;
            dist2Delta.y
                = opponentLocalPos.y - mpLooseBallInfo->mv3PickupPos.y;
            float dist2Sq = (dist2Delta.x * dist2Delta.x)
                          + (dist2Delta.y * dist2Delta.y);

            float dist3Sq = nlGetLengthSquared2D(
                mv3LocalContactPosition.x, mv3LocalContactPosition.y);

            nlVector2 dist4Delta;
            dist4Delta.x
                = pOwnerFielder->m_v3Position.x - m_v3Position.x;
            dist4Delta.y
                = pOwnerFielder->m_v3Position.y - m_v3Position.y;
            float dist4Sq = (dist4Delta.x * dist4Delta.x)
                          + (dist4Delta.y * dist4Delta.y);

            if ((mv3LocalContactPosition.x < -0.35f)
                || (dist1Sq > 0.36f && dist2Sq > 0.36f
                    && dist3Sq > pickupDistanceSq
                    && dist4Sq > pickupDistanceSq))
            {
                s16 angleDiff = (s16)(m_aDesiredFacingDirection
                                      - m_aActualFacingDirection);
                int animID = ChooseRunAnim(angleDiff, ballPos, 1.0f);
                PlayNewAnim(animID);

                float speedScale = 1.5f;
                if (speedScale
                    != m_pCurrentAnimController->m_fPlaybackSpeedScale)
                {
                    m_pCurrentAnimController->m_fPlaybackSpeedScale
                        = speedScale;
                }

                GoalieTweaks* pTweaks = (GoalieTweaks*)m_pTweaks;
                InitMovementFromAnimSeek(
                    pTweaks->fRunningDirectionSeekSpeed,
                    pTweaks->fRunningDirectionSeekFalloff);
                return;
            }

            if (IsWithinPounceRange() && !fn_8007D740())
            {
                InitActionPursueBallPounce();

                if (g_pBall->GetOwnerFielder() != 0
                    && (g_pBall->GetOwnerFielder()->m_eCharacterClass
                            == (eCharacterClass)0x05
                        || g_pBall->GetOwnerFielder()->m_eCharacterClass
                               == (eCharacterClass)0x0A
                        || g_pBall->GetOwnerFielder()->m_eCharacterClass
                               == (eCharacterClass)0x0F)
                    && g_pBall->m_v3Position.z > 0.66f)
                {
                    mbPlayMiss = true;
                }
            }
        }
    }
}

void Goalie::ActionPursueBallPounce(float fDeltaT)
{
    if (fn_8007C73C())
    {
        CleanupStun();
        ChooseSwatAnim(1);
        SetGoalieAction((eGoalieActionState)0x1F, 0);
        m_fDesiredSpeed = 0.0f;
        m_fActualSpeed = 0.0f;
        SetVelocity(v3Zero);

        if (m_pBall != 0)
        {
            fn_80139D1C(1, GetGlobalPad());
            ReleaseBall(false);
        }

        SetNoPickUpTime(0.2f);
        if (GetGlobalPad() != 0)
        {
            fn_8009591C(this, false);
        }
        mbGrabMonty = false;
        return;
    }

    float animTime = m_pCurrentAnimController->m_fTime;

    if (m_pBall == 0)
    {
        cBall* pBall = g_pBall;
        cPlayer* pOwner = pBall->m_pOwner;

        if (pOwner == 0)
        {
            SetGoalieAction(GOALIEACTION_LOOSEBALL_PICKUP, 0);
            mbIsDown = true;
            mfTargetTime = 0.0f;
            mfWaitTime = -1.0f;
            return;
        }

        if (IsOnSameTeam(pOwner))
        {
            InitActionMove(true);
            return;
        }

        if (CalculateDistanceSquared(
                GetJointPosition(m_nBallJointIndex), pBall->m_v3Position)
                < 0.16000001f
            || CalculateDistanceSquared(
                   GetJointPosition(m_nLeftHandJointIndex), pBall->m_v3Position)
                   < 0.16000001f
            || CalculateDistanceSquared(
                   GetJointPosition(m_nRightHandJointIndex), pBall->m_v3Position)
                   < 0.16000001f)
        {
            ExecutePounce(pOwner, true);
            return;
        }

        float pickupTime = mpLooseBallInfo->mfPickupTime;
        if (animTime < pickupTime
            && g_pBall->m_tShotTimer.m_uPackedTime == 0)
        {
            bool bWallBlock = mfWallBlock > 0.0f;
            if (!bWallBlock)
            {
                float ratio
                    = nlMaxEquals(animTime / pickupTime, 0.0f);
                ratio = nlMinEquals(ratio, 1.0f);
                float interpValue
                    = ratio * (ratio * ((-2.0f * ratio) + 3.0f));

                if (interpValue < 0.99f && !mbPlayMiss)
                {
                    TrackTarget(g_pBall->m_v3Position, interpValue, fDeltaT * lbl_806DBD28);
                }
            }
        }
    }

    if (animTime > 0.95f)
    {
        if (m_eAnimID != 0x85)
        {
            CheckForLimbEndZoneCollision();

            if (m_pBall == 0)
            {
                GoalieTweaks* pTweaks = (GoalieTweaks*)m_pTweaks;
                if (mFatigue.mfEnergyLevel < pTweaks->fGetupEnergyHigh)
                {
                    float speed = InterpolateRangeClamped(
                        pTweaks->fGetupSpeedLow,
                        1.0f,
                        pTweaks->fGetupEnergyLow,
                        pTweaks->fGetupEnergyHigh,
                        mFatigue.mfEnergyLevel);
                    m_pCurrentAnimController->m_fPlaybackSpeedScale = speed;
                }

                if (ShouldStartCrossBlend(5))
                {
                    InitActionMove(false);
                    return;
                }
            }
            else if (ShouldStartCrossBlend(6))
            {
                InitActionMoveWB();
                return;
            }
        }
        else
        {
            InitActionPursueRecover();
        }
    }
}

void Goalie::fn_8008A610(float fDeltaT)
{
    if (mnOffplayPending != GOALIE_OFFPLAY_NONE
        || (mpTarget->m_pBall != 0
            && !fn_8007B9A0(
                this, mpTarget->m_v3Position, lbl_806DBC98)))
    {
        InitActionMove(false);
        return;
    }

    bool bAnimHeld
        = m_pCurrentAnimController->m_ePlayMode == PM_HOLD
       && m_pCurrentAnimController->m_fTime == 0.0f;
    if (bAnimHeld)
    {
        InitActionMove(false);
    }

    if (mPursueDekeState == 0)
    {
        bool bWallBlock = mfWallBlock > 0.0f;
        if (bWallBlock)
        {
            InitActionMove(false);
            return;
        }

        switch (mPursueDekeType)
        {
        case 1:
        case 2:
            if (mpTarget->m_eActionState != (eFielderActionState)1)
            {
                break;
            }

            float animFrames
                = (float)mpTarget->m_pCurrentAnimController->m_pSAnim
                      ->m_nNumKeys;
            float fResult = fn_8007BEEC(this, mpTarget);
            float threshold;
            threshold = fResult * animFrames;
            float pickupDuration = mpLooseBallInfo->mfPickupTime
                                 * mpLooseBallInfo->mfAnimDuration;
            float pickupFrames = 30.0f * pickupDuration;
            float targetFrame = mpTarget->m_pCurrentAnimController->m_fTime
                              * (float)mpTarget->m_pCurrentAnimController
                                    ->m_pSAnim->m_nNumKeys;
            if (targetFrame > threshold - pickupFrames)
            {
                mPursueDekeType = 0;
            }
            break;

        default:
            break;
        }

        switch (mPursueDekeType)
        {
        case 6:
        {
            if (mpTarget->m_eActionState != (eFielderActionState)1)
            {
                InitActionMove(false);
                return;
            }

            if (!mpTarget->mbTangible)
            {
                mPursueDekeType = 4;
                return;
            }

            mbDoIntercept = true;
            nlVector3 v3Direction;
            nlVector3 v3Focus;
            unsigned short aFacing;
            FindDesiredGoaliePosition(mv3TargetPosition, v3Direction, v3Focus, aFacing, 0);
            mv3NavTarget = mv3TargetPosition;
            m_aDesiredFacingDirection = aFacing;
            mUrgency = URGENCY_HIGH;
            fn_800797DC(
                this, 1, fDeltaT, 0.2f + mfGoalieStepDist);
            return;
        }

        case 0:
        case 3:
        case 5:
        {
            if (!mpTarget->mbTangible)
            {
                InitActionMove(false);
                return;
            }

            mpLooseBallInfo = &LooseBallAnims::mLooseBallKickInfo[1];
            mbDoNavigate = false;

            nlVector2 v2Delta;
            v2Delta.x = m_v3Position.x - mpTarget->m_v3Position.x;
            v2Delta.y = m_v3Position.y - mpTarget->m_v3Position.y;
            float distSq = nlVec2LengthSquared(v2Delta);
            float radius;
            mpTarget->m_pPhysicsCharacter->GetRadius(&radius);

            if (!mpTarget->mbTangible
                || distSq
                       > (mpLooseBallInfo->mfPickupDistance + radius
                             + lbl_806DBD10)
                             * (mpLooseBallInfo->mfPickupDistance + radius
                                 + lbl_806DBD10))
            {
                if (mpTarget->m_pBall == 0)
                {
                    InitActionMove(false);
                    return;
                }

                mv3NavTarget = mpTarget->m_v3Position;
                float dx = mv3NavTarget.x - m_v3Position.x;
                float dy = mv3NavTarget.y - m_v3Position.y;
                float angle = nlATan2f(dy, dx);
                m_aDesiredFacingDirection
                    = (u16)(s32)(10430.378f * angle);
                mUrgency = URGENCY_HIGH;
                fn_800797DC(this, 2, fDeltaT, gfRepositionThreshold);
                return;
            }

            const LooseBallInfo* pCloseInfo
                = &LooseBallAnims::mLooseBallKickInfo[2];
            float closeDistance = pCloseInfo->mfPickupDistance + radius;
            if (distSq <= closeDistance * closeDistance)
            {
                mpLooseBallInfo = pCloseInfo;
            }

            PlayNewAnim(mpLooseBallInfo->mnAnimID);
            InitMovementFromAnim(0, v3Zero, 1.0f, false);
            mPursueDekeState = 1;
            return;
        }

        case 7:
        {
            mbDoNavigate = false;

            nlVector2 v2Delta;
            v2Delta.x = m_v3Position.x - mpTarget->m_v3Position.x;
            v2Delta.y = m_v3Position.y - mpTarget->m_v3Position.y;
            float distSq = nlVec2LengthSquared(v2Delta);
            float radius;
            mpTarget->m_pPhysicsCharacter->GetRadius(&radius);

            if (distSq
                > (radius + lbl_806DBD14) * (radius + lbl_806DBD14))
            {
                if (mpTarget->m_pBall == 0)
                {
                    InitActionMove(false);
                    return;
                }

                mv3NavTarget = mpTarget->m_v3Position;
                float dx = mv3NavTarget.x - m_v3Position.x;
                float dy = mv3NavTarget.y - m_v3Position.y;
                float angle = nlATan2f(dy, dx);
                m_aDesiredFacingDirection
                    = (u16)(s32)(10430.378f * angle);
                mUrgency = URGENCY_HIGH;
                fn_800797DC(this, 2, fDeltaT, gfRepositionThreshold);
                return;
            }

            if (mpTarget->fn_8003E6FC())
            {
                return;
            }

            PlayNewAnim(0xAD);
            InitMovementFromAnim(0, v3Zero, 1.0f, false);
            SetGoalieAction((eGoalieActionState)0x1F, 0);
            mbGrabMonty = true;
            mpMonty = mpTarget;
            fn_8004F204(mpTarget);
            g_pBall->m_bBallPathChangeCount = 0;
            m_fDesiredSpeed = 0.0f;
            m_fActualSpeed = 0.0f;
            SetVelocity(v3Zero);
            fn_800EBBFC(mUnidentified318, 0x76520305, 0, 0);
            return;
        }

        case 1:
        case 2:
        {
            if (!mpTarget->mbTangible)
            {
                InitActionMove(false);
                return;
            }

            mbDoNavigate = false;
            nlVector2 v2Delta;
            v2Delta.x = m_v3Position.x - mpTarget->m_v3Position.x;
            v2Delta.y = m_v3Position.y - mpTarget->m_v3Position.y;
            float distSq = nlVec2LengthSquared(v2Delta);
            float radius;
            mpTarget->m_pPhysicsCharacter->GetRadius(&radius);

            if (distSq
                > (mpLooseBallInfo->mfPickupDistance + radius
                      + lbl_806DBD0C)
                      * (mpLooseBallInfo->mfPickupDistance + radius
                          + lbl_806DBD0C))
            {
                if (mpTarget->m_pBall == 0)
                {
                    InitActionMove(false);
                    return;
                }

                mv3NavTarget = mpTarget->m_v3Position;
                float dx = mv3NavTarget.x - m_v3Position.x;
                float dy = mv3NavTarget.y - m_v3Position.y;
                float angle = nlATan2f(dy, dx);
                m_aDesiredFacingDirection
                    = (u16)(s32)(10430.378f * angle);
                mUrgency = URGENCY_HIGH;
                fn_800797DC(this, 2, fDeltaT, gfRepositionThreshold);
                return;
            }

            PlayNewAnim(mpLooseBallInfo->mnAnimID);
            InitMovementFromAnim(0, v3Zero, 1.0f, false);
            mPursueDekeState = 1;
            return;
        }

        case 4:
        {
            nlVector3 v3Target = mpTarget->m_v3Position;
            if (mpTarget->mbTangible)
            {
                mPursueDekeType = 0;
            }

            if (mpTarget->m_eCharacterClass == (eCharacterClass)0x10)
            {
                nlVector3 v3Direction;
                nlSinCos(&v3Direction.y, &v3Direction.x, m_aActualFacingDirection);
                v3Direction.z = 0.0f;
                nlVec3ScaleAdd(v3Target, lbl_806DBD24, v3Direction, mpTarget->m_v3Position);
            }

            float dx = v3Target.x - m_v3Position.x;
            float dy = v3Target.y - m_v3Position.y;
            mv3NavTarget = v3Target;
            float angle = nlATan2f(dy, dx);
            m_aDesiredFacingDirection
                = (u16)(s32)(10430.378f * angle);
            mUrgency = URGENCY_HIGH;
            fn_800797DC(this, 2, fDeltaT, 0.3f);
            return;
        }
        }
    }
    else if (mPursueDekeState == 1)
    {
        if (!mpTarget->mbTangible)
        {
            float pickupTime = mpLooseBallInfo->mfPickupTime;
            float animTime = m_pCurrentAnimController->m_fTime;
            if (animTime < pickupTime - 0.1f)
            {
                mv3NavTarget = mpTarget->m_v3Position;
                float dx = mv3NavTarget.x - m_v3Position.x;
                float dy = mv3NavTarget.y - m_v3Position.y;
                float angle = nlATan2f(dy, dx);
                m_aDesiredFacingDirection
                    = (u16)(s32)(10430.378f * angle);
                mPursueDekeState = 0;
                fn_800797DC(this, 2, fDeltaT, 0.3f);
            }
            else
            {
                mPursueDekeState = 2;
            }
            return;
        }

        if (mpTarget->IsFallenDown())
        {
            return;
        }

        float animTime = m_pCurrentAnimController->m_fTime;
        float pickupTime = mpLooseBallInfo->mfPickupTime;
        float ratio = animTime / pickupTime;
        ratio = nlMaxEquals(ratio, 0.0f);
        ratio = nlMinEquals(ratio, 1.0f);
        float interpValue
            = ratio * (ratio * ((-2.0f * ratio) + 3.0f));

        if (interpValue < 0.99f)
        {
            bool bWallBlock = mfWallBlock > 0.0f;
            if (!bWallBlock)
            {
                TrackTarget(mpTarget->m_v3Position, interpValue, fDeltaT * lbl_806DBC8C);
            }
            return;
        }

        nlVector2 v2Delta;
        v2Delta.x = m_v3Position.x - mpTarget->m_v3Position.x;
        v2Delta.y = m_v3Position.y - mpTarget->m_v3Position.y;
        float distSq = nlVec2LengthSquared(v2Delta);
        float radius;
        mpTarget->m_pPhysicsCharacter->GetRadius(&radius);
        if (distSq
            < (mpLooseBallInfo->mfPickupDistance + radius + 0.8f)
                  * (mpLooseBallInfo->mfPickupDistance + radius + 0.8f))
        {
            fn_800809D0(this, mpTarget, false);
        }
    }
}

void Goalie::ActionOffplay(float fDeltaT)
{
    if (ShouldStartCrossBlend(0x99))
    {
        int animID;
        int currentAnimID = m_eAnimID;
        if (currentAnimID == 0x92 || currentAnimID == 0x94
            || currentAnimID == 0x96)
        {
            animID = 0x96;
        }
        else if (currentAnimID == 0x91 || currentAnimID == 0x93
                 || currentAnimID == 0x95)
        {
            animID = 0x95;
        }
        else
        {
            static FilteredRandomRange randgenDejected;
            int index = randgenDejected.genrand(5);
            animID = gOffplayDejected[index];
        }

        SetAnimState(animID, true, 0.2f, false, false);
        InitMovementFromAnim(0, v3Zero, 1.0f, false);
    }

    CheckForLimbEndZoneCollision();
}

void Goalie::ActionLooseBallPursueBouncing(float deltaTime)
{
    if (IsPassThreat() || mnOffplayPending != GOALIE_OFFPLAY_NONE
        || !IsLooseBallClose(0.0f) || g_pBall->m_pOwner != 0)
    {
        InitActionMove(true);
        return;
    }

    bool bWallBlock = mfWallBlock > 0.0f;
    if (bWallBlock)
    {
        InitActionMove(true);
        return;
    }

    if (muBallDeflectCount != g_pBall->m_bBallDeflectCount)
    {
        InitActionLooseBallSetup();
        return;
    }

    mfTargetTime -= deltaTime;
    if (mfTargetTime < 0.1f
        || (g_pBall->m_v3Position.z < 1.0f
            && g_pBall->m_v3Velocity.z < 3.0f))
    {
        InitActionLooseBallSetup();
        return;
    }

    nlVector3 v3TargetPos;
    nlVector3 v3TargetVel;
    FakeBallWorld::GetPredictedBallPosition(
        mfTargetTime, v3TargetPos, v3TargetVel);

    if (mUnidentified254 == 0)
    {
        nlVector2 delta;
        delta.x = m_v3Position.x - v3TargetPos.x;
        delta.y = m_v3Position.y - v3TargetPos.y;
        if (nlVec2LengthSquared(delta) < mfTargetDist)
        {
            PlayNewAnim(5);
            InitMovementFromAnim(0, v3Zero, 1.0f, false);

            GetLocalPoint(mv3LocalContactPosition, v3TargetPos, m_v3Position, m_aActualFacingDirection);
            GetLocalPoint(mv3LocalContactVelocity, v3TargetVel, v3Zero, m_aActualFacingDirection);

            InitActionLooseBallCatch();
            return;
        }
    }

    const nlVector3& pos = m_v3Position;
    float dx = v3TargetPos.x - pos.x;
    float dy = v3TargetPos.y - pos.y;
    float angle = nlATan2f(dy, dx);
    m_aDesiredFacingDirection = (u16)(s32)(10430.378f * angle);

    if (CalculateDistanceSquared(v3TargetPos, mv3TargetPosition)
        > mfTargetDist)
    {
        InitActionLooseBallSetup();
    }
    else if (m_eAnimID != 0x24)
    {
        PlayNewAnim(0x24);
        GoalieTweaks* pTweaks = (GoalieTweaks*)m_pTweaks;
        InitMovementFromAnimSeek(pTweaks->fRunningDirectionSeekSpeed,
            pTweaks->fRunningDirectionSeekFalloff);
    }

    CheckForLimbEndZoneCollision();
}

void Goalie::ActionSnapBall(float fDeltaT)
{
    unsigned short aRootRot;
    float fTimeLeft;
    nlVector3 v3TargetPos;
    nlVector3 v3RootPos;

    if (mnOffplayPending != GOALIE_OFFPLAY_NONE
        || lbl_806E0C94->mUnidentified20
        || lbl_806E0C94->mUnidentified18 == 3)
    {
        if (m_pBall != 0)
        {
            ReleaseBall(false);
        }
        InitActionMove(true);
        return;
    }

    if (mUnidentified254 == 0 && g_pBall->m_pOwner != this)
    {
        TacklePlayer(g_pBall->m_pOwner);
        StealBall(g_pBall->m_pOwner);

        fTimeLeft = m_tNoPickupTimer.GetSeconds();

        if (fTimeLeft > 0.0f)
        {
            GetCurrentAnimFuture(m_nBallJointIndex,
                m_pCurrentAnimController->m_fTime,
                v3TargetPos,
                v3RootPos,
                aRootRot);

            float interpFactor;
            float invInterpFactor;

            interpFactor = (1.0f / mfWaitTime) * (mfWaitTime - fTimeLeft);
            invInterpFactor = 1.0f - interpFactor;

            v3TargetPos.x = (invInterpFactor * g_pBall->m_v3Position.x)
                          + (interpFactor * v3TargetPos.x);
            v3TargetPos.y = (invInterpFactor * g_pBall->m_v3Position.y)
                          + (interpFactor * v3TargetPos.y);
            v3TargetPos.z = (invInterpFactor * g_pBall->m_v3Position.z)
                          + (interpFactor * v3TargetPos.z);

            g_pBall->SetPosition(v3TargetPos);
            return;
        }

        PickupBall(g_pBall);
        m_pPhysicsCharacter->m_CanCollideWithGoalLine = true;
        m_pPhysicsCharacter->m_CanCollideWithWall = true;
        mbPickedUp = true;
        return;
    }

    if (m_pBall == 0)
    {
        InitActionMove(true);
        return;
    }

    bool shouldMoveWB
        = m_pCurrentAnimController->m_ePlayMode == PM_HOLD
       && m_pCurrentAnimController->m_fTime == 1.0f;

    if (shouldMoveWB)
    {
        InitActionMoveWB();
    }
}

void Goalie::ActionGrabBall(float fDeltaT)
{
    bool bShouldInitMove = true;
    if (mUnidentified254 == 0)
    {
        bShouldInitMove = false;
        if (m_pCurrentAnimController->m_ePlayMode == PM_HOLD
            && m_pCurrentAnimController->m_fTime == 1.0f)
        {
            bShouldInitMove = true;
        }
    }

    if (bShouldInitMove)
    {
        if (m_pBall == 0)
        {
            InitActionMove(true);
            return;
        }
        InitActionMoveWB();
        return;
    }

    if (g_pBall->m_pOwner != this)
    {
        if (g_pBall->GetOwnerFielder() == 0)
        {
            InitActionMove(false);
            return;
        }

        float pickupTime = mpLooseBallInfo->mfPickupTime;
        float fTimeThreshold = 0.1f + pickupTime;
        float fCurrentTime = m_pCurrentAnimController->m_fTime;
        if (fCurrentTime < fTimeThreshold)
        {
            float fInterpFactor = fCurrentTime / fTimeThreshold;
            TrackTarget(g_pBall->m_v3Position, fInterpFactor, fDeltaT * lbl_806DBC8C);

            const nlVector3& jointPos
                = GetJointPosition(m_nBallJointIndex);

            nlVector3 delta;
            nlVec3Set(delta,
                g_pBall->m_v3Position.x - jointPos.x,
                g_pBall->m_v3Position.y - jointPos.y,
                g_pBall->m_v3Position.z - jointPos.z);

            if (nlGetLengthSquared3D(delta.x, delta.y, delta.z) < 0.25f)
            {
                StealBall(g_pBall->m_pOwner);
                PickupBall(g_pBall);
                m_pPhysicsCharacter->m_CanCollideWithGoalLine = true;
                m_pPhysicsCharacter->m_CanCollideWithWall = true;
                mbPickedUp = true;
                EmitGoalieCatch(this, "goalie_catch", false);
            }
        }
    }
}

void Goalie::fn_8008B718(float fDeltaT)
{
    nlVector3 pos;

    if (m_v3Position.z > 0.0f)
    {
        nlVector3 adjustedPos = m_v3Position;
        adjustedPos.z -= fDeltaT * lbl_806DBC94;
        if (adjustedPos.z < 0.0f)
        {
            adjustedPos.z = 0.0f;
        }
        SetPosition(adjustedPos);
    }

    cPN_SAnimController* pController = m_pCurrentAnimController;
    bool bShouldInitMove = false;
    if (pController->m_ePlayMode == PM_HOLD
        && pController->m_fTime == 1.0f)
    {
        bShouldInitMove = true;
    }

    if (bShouldInitMove)
    {
        InitActionMove(false);
        return;
    }

    pos = m_v3Position;
    float goalLineX = cField::GetGoalLineX(1U);
    float absX = (float)fabs(pos.x);
    float adjustedGoalLineX = goalLineX + 1.5f;

    if (absX > adjustedGoalLineX)
    {
        float radius;
        m_pPhysicsCharacter->GetRadius(&radius);

        float netWidth = cNet::m_fNetWidth;
        float netWidthAdjusted
            = (0.5f * netWidth) - radius
            - (absX - adjustedGoalLineX);

        pos.x = nlMinEquals(
            nlMaxEquals(pos.x, -adjustedGoalLineX), adjustedGoalLineX);
        pos.y = nlMinEquals(
            nlMaxEquals(pos.y, -netWidthAdjusted), netWidthAdjusted);
        SetPosition(pos);
    }
}

void Goalie::InitActionPass(bool useTarget)
{
    int animID;

    SetGoalieAction(GOALIEACTION_PASS, 0);
    mpPassTarget = 0;

    if (useTarget)
    {
        cPlayer* pPassTarget = FindOpenPassTarget();
        mpPassTarget = pPassTarget;

        if (mpPassTarget != 0 && IsTargetViable(mpPassTarget))
        {
            if (fn_8007C904(this, mpPassTarget->m_v3Position))
            {
                animID = 0;
            }
            else
            {
                GoalieTweaks* pTweaks = (GoalieTweaks*)m_pTweaks;

                nlVector2 v2Distance;
                v2Distance.x
                    = m_v3Position.x - mpPassTarget->m_v3Position.x;
                v2Distance.y
                    = m_v3Position.y - mpPassTarget->m_v3Position.y;
                float fDistanceSq = nlVec2LengthSquared(v2Distance);
                float fKickDistanceSq
                    = nlGetLengthSquared1D(pTweaks->fKickDistanceMin);
                float fOverhandThrowDistanceSq = nlGetLengthSquared1D(
                    pTweaks->fOverhandThrowDistanceMin);
                float fOpenTo = OpenTo(this, mpPassTarget);

                if (GetGlobalPad() != 0)
                {
                    if (fn_80331C04(GetGlobalPad(), 0x17, true))
                    {
                        animID = 2;
                    }
                    else if (fDistanceSq > fOverhandThrowDistanceSq
                             || fOpenTo < 0.85f)
                    {
                        animID = 0;
                    }
                    else
                    {
                        animID = 1;
                    }
                }
                else if (fDistanceSq > fKickDistanceSq)
                {
                    animID = 2;
                }
                else if (fDistanceSq > fOverhandThrowDistanceSq
                         || fOpenTo < 0.85f)
                {
                    animID = 0;
                }
                else
                {
                    animID = 1;
                }
            }
        }
        else
        {
            mpPassTarget = 0;
        }
    }

    if (mpPassTarget == 0)
    {
        nlVector3 v3Position = m_v3Position;
        float fXOffset;
        if (m_v3Position.x < 0.0f)
        {
            fXOffset = 4.0f;
        }
        else
        {
            fXOffset = -4.0f;
        }
        v3Position.x += fXOffset;
        animID = fn_8007C904(this, v3Position) ? 0 : 2;
    }

    SetAnimState(animID, true, 0.2f, false, false);
    GoalieTweaks* pTweaks = (GoalieTweaks*)m_pTweaks;
    InitMovementFromAnimSeek(pTweaks->fRunningDirectionSeekSpeed,
        pTweaks->fRunningDirectionSeekFalloff);
}

void Goalie::InitActionPreCrouch(eGoalieCrouchType crouchType)
{
    if (mGoalieActionState == GOALIEACTION_STS_RECOVER)
    {
        return;
    }

    mCrouchType = crouchType;
    mbIsDown = false;
    SetGoalieAction(GOALIEACTION_PRE_CROUCH, 0);
    PlayNewAnim(0x2C);
    InitMovementFromAnim(0, v3Zero, 1.0f, false);
}

void Goalie::fn_8008BBB0(
    cFielder* pTarget, int nPursueDekeType)
{
    if (fn_8007D740())
    {
        return;
    }

    SetGoalieAction(GOALIEACTION_UNIDENTIFIED_13, 0);
    mPursueDekeType = nPursueDekeType;
    mPursueDekeState = 0;
    mpTarget = pTarget;
    mbIsDown = false;

    switch (nPursueDekeType)
    {
    case 1:
    case 2:
        mpLooseBallInfo = &LooseBallAnims::mUnknownD0BC;
        break;
    default:
        break;
    }

    if (mUnidentified254 != 0)
    {
        fn_80097648(0.1f);
    }

    mUrgency = URGENCY_HIGH;
    mnSubstate = 1;
    fn_8008A610(0.0f);

    if (mGoalieActionState == GOALIEACTION_UNIDENTIFIED_13)
    {
        PlayerAttackData data;
        data.pAttacker = this;
        data.nAttackerPadID = -1;
        data.pTarget = mpTarget;
        data.mUnidentified0C = 2;
        data.mUnidentified10 = false;
        fn_8005E408(lbl_806E0C94, &data);
    }
}

void Goalie::InitActionLooseBallPickup(
    float fDistance, bool bStartPickup)
{
    SetGoalieAction(GOALIEACTION_LOOSEBALL_PICKUP, 0);
    SetAnimState(mpLooseBallInfo->mnAnimID, true, 0.2f, false, false);
    InitMovementFromAnim(0, v3Zero, 1.0f, false);
    mMoveDirection = GOALIEDIR_IDLE;
    mfTargetTime = 0.0f;
    mfWaitTime = -1.0f;
    mbPickedUp = false;
    mbIsDown = true;

    if (fDistance < mpLooseBallInfo->mfPickupDistance)
    {
        float fRemaining = mpLooseBallInfo->mfPickupDistance - fDistance;
        float fPickupTime = mpLooseBallInfo->mfPickupTime;
        mfTargetTime
            = fRemaining * fPickupTime / mpLooseBallInfo->mfPickupDistance;

        cPN_SAnimController* pController = m_pCurrentAnimController;
        float fNewAnimTime = mfTargetTime;
        pController->SetTime(fNewAnimTime);
    }

    if (bStartPickup)
    {
        fn_8007B214(this);
    }
}

void Goalie::InitActionSaveReposition()
{
    mv3NavTarget = mv3TargetPosition;
    mMoveDirection = GOALIEDIR_IDLE;
    SetGoalieAction(GOALIEACTION_SAVE_REPOSITION, 0);

    nlVector3 v3Delta;
    nlVec3Sub2D(v3Delta, m_v3Position, mv3NavTarget);
    mfTargetDist = nlGetLengthSquared2D(v3Delta.x, v3Delta.y);

    float fBallDy = g_pBall->m_v3Position.y - m_v3Position.y;
    float fBallDx = g_pBall->m_v3Position.x - m_v3Position.x;
    m_aDesiredFacingDirection
        = (u16)(s32)(nlATan2f(fBallDy, fBallDx) * 10430.378f);

    fn_800797DC(this, 0, 0.0f, gfRepositionThreshold);
    if (mfWaitTime > 0.4f)
    {
        mUrgency = URGENCY_MED;
    }
    else
    {
        mUrgency = URGENCY_HIGH;
    }
}

void Goalie::InitActionLooseBallPursueRolling()
{
    mv3NavTarget = mv3TargetPosition;
    if (mGoalieActionState != GOALIEACTION_LOOSEBALL_PURSUE_ROLLING)
    {
        SetGoalieAction(GOALIEACTION_LOOSEBALL_PURSUE_ROLLING, 0);
    }

    float fDy = mv3TargetPosition.y - m_v3Position.y;
    float fDx = mv3TargetPosition.x - m_v3Position.x;
    m_aDesiredFacingDirection
        = (u16)(s32)(nlATan2f(fDy, fDx) * 10430.378f);

    mv3NavTarget = mv3TargetPosition;
    mUrgency = URGENCY_MED;
    mbIsDown = false;
}

void Goalie::InitActionLooseBallSetup()
{
    if (fn_8007C590(this))
    {
        return;
    }

    if (!IsLooseBallClose(*fn_800A636C(lbl_806E0E00)
                ->mpLooseBallChaseDistance))
    {
        InitActionMove(true);
        return;
    }

    g_pBall->m_uGoalType = 4;

    m_pPhysicsCharacter->m_CanCollideWithBall = true;
    mbDoHeadTrack = true;
    mbPickedUp = false;
    mbIsDown = false;

    const nlVector3* pBallVelocity = &g_pBall->m_v3Velocity;
    nlVector3 v3BallPosition = g_pBall->m_v3Position;
    const nlVector3& v3NetBase = m_pTeam->m_pNet->m_v3NetLocation;
    muBallDeflectCount = g_pBall->m_bBallDeflectCount;
    mUnidentified3E4 = g_pBall->mUnidentified008;

    bool bInCone = fn_8007D644(this);
    float fBallSpeed = pBallVelocity->x * pBallVelocity->x
                     + pBallVelocity->y * pBallVelocity->y
                     + pBallVelocity->z * pBallVelocity->z;
    float fAbsBallX;

    if (fBallSpeed > lbl_806DBC80 * lbl_806DBC80 && bInCone)
    {
        float fAbsGoalieX = (float)fabs(m_v3Position.x);
        fAbsBallX = (float)fabs(v3BallPosition.x);
        if (fAbsBallX < fAbsGoalieX - 1.5f)
        {
            float fTimeTilSave
                = fn_8007EDF4(this, fn_800776B4());

            if (fTimeTilSave > 0.0f && fTimeTilSave < 2.0f)
            {
                if (mUnidentified254 != 0)
                {
                    muSaveType = 0x0000FFFC;
                }
                else
                {
                    muSaveType = 0x0000FFFF;
                }

                mbShouldMiss = false;
                mfTimeTilSave = fn_8007ECB4(
                    this, fTimeTilSave, muSaveType, false, false);

                if (mfTimeTilSave > 0.0f)
                {
                    if (1.0f + mBlendInfo.mv3BlendedSavePos.z
                        >= mv3LocalContactPosition.z)
                    {
                        mfWaitTime = mfTimeTilSave
                                   - mBlendInfo.mfMilestoneTime[2];
                        if (mfWaitTime < lbl_806DBD04)
                        {
                            InitActionSave();
                            return;
                        }

                        if (lbl_806DBC90 && ShouldReposition())
                        {
                            InitActionSaveReposition();
                            return;
                        }

                        SetGoalieAction(GOALIEACTION_SAVE_SETUP, 0);
                        SetAnimState(7, true, 0.2f, false, false);

                        GoalieTweaks* pTweaks
                            = (GoalieTweaks*)m_pTweaks;
                        InitMovementFromAnimSeek(
                            pTweaks->fSaveDirectionSeekSpeed,
                            pTweaks->fSaveDirectionSeekFalloff);
                        return;
                    }

                    mbShouldMiss = false;
                    if (fn_8007BF68(this, true))
                    {
                        return;
                    }
                }
            }
        }
    }

    if (pBallVelocity->z < 3.0f && v3BallPosition.z < 1.5f)
    {
        if (mUnidentified254 == 0 && bInCone
            && fBallSpeed > 0.25f
            && fn_8007B9A0(this, v3BallPosition, 5.0f))
        {
            nlVector3 v3GuessBallPos;
            nlVector3 v3GuessBallVel;
            mpLooseBallInfo = LooseBallAnims::GetDesperationInfo(1);
            FakeBallWorld::GetPredictedBallPosition(
                mpLooseBallInfo->mfPickupTime
                    * mpLooseBallInfo->mfAnimDuration,
                v3GuessBallPos,
                v3GuessBallVel);

            float fPanicLineX = cField::GetGoalLineX(1U) - 2.0f;
            float fAbsGuessX = (float)fabs(v3GuessBallPos.x);

            if (fAbsGuessX > fPanicLineX)
            {
                SetGoalieAction(
                    GOALIEACTION_LOOSEBALL_DESPERATE, 0);
                mbIsDown = true;

                if ((float)fabs(v3BallPosition.x) >= fPanicLineX)
                {
                    mv3TargetPosition = v3BallPosition;
                }
                else
                {
                    float fGoalLineX2 = cField::GetGoalLineX(1U);
                    if (fAbsGuessX < fGoalLineX2)
                    {
                        mv3TargetPosition = v3GuessBallPos;
                    }
                    else
                    {
                        mv3TargetPosition.x
                            = v3NetBase.x > 0.0f
                                ? fPanicLineX
                                : -fPanicLineX;
                        float fGuessY = v3GuessBallPos.y;
                        float fBallPosY = v3BallPosition.y;
                        float fBallPosX = v3BallPosition.x;
                        float fTargetX = mv3TargetPosition.x;
                        float fDiffY = fBallPosY - fGuessY;
                        float fGuessX = v3GuessBallPos.x;
                        float fDiffXTarget = fBallPosX - fTargetX;
                        float fDiffXOrig = fBallPosX - fGuessX;
                        mv3TargetPosition.y
                            = fBallPosY
                            - fDiffXTarget * fDiffY / fDiffXOrig;
                    }
                }

                mv3TargetPosition.z = 0.0f;
                fn_8007E940(this, 0.75f);
                PlayNewAnim(mpLooseBallInfo->mnAnimID);
                InitMovementFromAnim(0, v3Zero, 1.0f, false);
                return;
            }

            float fInterceptTime;
            float fClosestDist;
            bool bFound = fn_8016EA10(m_v3Position,
                1.0f,
                6.0f,
                mv3TargetPosition,
                mv3TargetVelocity,
                fInterceptTime,
                fClosestDist,
                3.0f);

            if (bFound && mv3TargetPosition.z < 1.0f
                && fClosestDist < 0.75f)
            {
                SetGoalieAction(
                    GOALIEACTION_LOOSEBALL_DESPERATE, 0);
                mbIsDown = true;

                float fLimitX = cField::GetGoalLineX(1U) - 0.5f;
                if ((float)fabs(mv3TargetPosition.x) > fLimitX)
                {
                    if ((float)fabs(v3BallPosition.x) > fLimitX)
                    {
                        mv3TargetPosition = v3BallPosition;
                    }
                    else
                    {
                        mv3TargetPosition.y = v3BallPosition.y
                                            - (v3BallPosition.x - fLimitX)
                                                  * (v3BallPosition.y
                                                      - mv3TargetPosition.y)
                                                  / (v3BallPosition.x
                                                      - mv3TargetPosition.x);
                        mv3TargetPosition.x
                            = v3NetBase.x > 0.0f
                                ? fLimitX
                                : -fLimitX;
                    }
                }

                fn_8007E940(this, 0.75f);
                float fAnimTime = mpLooseBallInfo->mfPickupTime
                                * mpLooseBallInfo->mfAnimDuration;
                mfTargetTime = fInterceptTime - fAnimTime;

                if (mfTargetTime < 0.02f)
                {
                    PlayNewAnim(mpLooseBallInfo->mnAnimID);
                    InitMovementFromAnim(0, v3Zero, 1.0f, false);
                    return;
                }

                mv3NavTarget = mv3TargetPosition;
                float fDyNav
                    = v3BallPosition.y - m_v3Position.y;
                float fDxNav
                    = v3BallPosition.x - m_v3Position.x;
                m_aDesiredFacingDirection
                    = (u16)(s32)(nlATan2f(fDyNav, fDxNav)
                                 * 10430.378f);

                if (mfTargetTime > 1.0f)
                {
                    mUrgency = URGENCY_LOW;
                }
                else if (mfTargetTime > 0.5f)
                {
                    mUrgency = URGENCY_MED;
                }
                else
                {
                    mUrgency = URGENCY_HIGH;
                }

                fn_800797DC(this, 2, 0.0f, 0.0f);
                return;
            }
        }

        fAbsBallX = (float)fabs(v3BallPosition.x);
        float fMinKickLine
            = 0.35f
                * (cField::GetGoalLineX(1U)
                    - cField::GetPenaltyBoxX(1U))
            + cField::GetPenaltyBoxX(1U);
        bool bDoGrab = false;

        for (int nPlayerIndex = 0; nPlayerIndex < 4;
            ++nPlayerIndex)
        {
            cPlayer* pPlayer
                = fn_800A6A84(m_pTeam, nPlayerIndex);
            if (pPlayer->GetGlobalPad() != 0)
            {
                bDoGrab = true;
                break;
            }
        }

        if (!IsLooseBallClose(0.0f))
        {
            cFielder* pOpponent
                = fn_8009664C(this, v3BallPosition, true);
            if (pOpponent != 0)
            {
                nlVector2 v2OpponentDistance;
                v2OpponentDistance.x
                    = pOpponent->m_v3Position.x - v3BallPosition.x;
                v2OpponentDistance.y
                    = pOpponent->m_v3Position.y - v3BallPosition.y;
                float fOppDistSq
                    = nlVec2LengthSquared(v2OpponentDistance);

                nlVector2 v2GoalieDistance;
                v2GoalieDistance.x
                    = m_v3Position.x - v3BallPosition.x;
                v2GoalieDistance.y
                    = m_v3Position.y - v3BallPosition.y;
                float fGoalieDistSq
                    = nlVec2LengthSquared(v2GoalieDistance);

                if (fGoalieDistSq > fOppDistSq)
                {
                    if (mGoalieActionState == GOALIEACTION_MOVE)
                    {
                        return;
                    }
                    InitActionMove(true);
                    return;
                }
            }
        }
        else if (mUnidentified254 != 0)
        {
            bDoGrab = false;
        }
        else if (fAbsBallX > fMinKickLine)
        {
            bDoGrab = true;
        }
        else
        {
            cPlayer* pPassTarget = FindOpenPassTarget();
            if (pPassTarget != 0)
            {
                nlVector3 v3BallDelta;
                nlVec3Sub(
                    v3BallDelta, v3BallPosition, m_v3Position);

                nlVector3 v3TargetDelta;
                nlVec3Sub(v3TargetDelta,
                    pPassTarget->m_v3Position,
                    m_v3Position);

                float fBallDist
                    = nlSqrt(v3BallDelta.GetLengthSq3D(), true);
                float fInvDist = 1.0f / fBallDist;
                nlVec3Scale(v3BallDelta, fInvDist);

                float fInvTargetDist = nlRecipSqrt(
                    v3TargetDelta.GetLengthSq3D(), true);
                nlVec3Scale(v3TargetDelta, fInvTargetDist);

                nlVector3 v3Right;
                nlVec3Set(v3Right,
                    m_m4WorldMatrix.m11,
                    m_m4WorldMatrix.m12,
                    m_m4WorldMatrix.m13);

                if (fBallDist < 1.2f)
                {
                    bDoGrab = true;
                }
                else
                {
                    float fDotBallTarget = nlVec3DotProduct(
                        v3BallDelta, v3TargetDelta);
                    if (fDotBallTarget < 0.7071f)
                    {
                        bDoGrab = true;
                    }
                    else
                    {
                        float fDotRight = nlVec3DotProduct(
                            v3BallDelta, v3Right);
                        if (fDotRight < 0.0f)
                        {
                            bDoGrab = true;
                        }
                        else
                        {
                            cFielder* pOpp = fn_8009664C(
                                this, v3BallPosition, true);
                            if (pOpp != 0)
                            {
                                nlVector2 v2OpponentDistance;
                                v2OpponentDistance.x
                                    = pOpp->m_v3Position.x
                                    - v3BallPosition.x;
                                v2OpponentDistance.y
                                    = pOpp->m_v3Position.y
                                    - v3BallPosition.y;
                                if (nlVec2LengthSquared(
                                        v2OpponentDistance)
                                    < fBallDist * fBallDist)
                                {
                                    bDoGrab = true;
                                }
                            }
                        }
                    }
                }
            }
        }

        if (!bDoGrab && mUnidentified254 == 0)
        {
            float fAbsGoalieX = (float)fabs(m_v3Position.x);
            float fAbsBallXPos = (float)fabs(v3BallPosition.x);
            float fAbsBallYDist
                = (float)fabs(v3BallPosition.y - m_v3Position.y);
            float fDiffX = fAbsBallXPos - fAbsGoalieX;
            u16 nAbsAngle = (u16)(s32)(nlATan2f(fAbsBallYDist, fDiffX) * 10430.378f);

            if (fAbsBallX > fMinKickLine
                || nAbsAngle < 0x4E34)
            {
                bDoGrab = true;
            }
        }

        if (bDoGrab)
        {
            FakeBallWorld::GetPredictedBallPosition(
                LooseBallAnims::mpLooseBallInfo->mfPickupTime
                    * LooseBallAnims::mpLooseBallInfo->mfAnimDuration,
                mv3TargetPosition,
                mv3TargetVelocity);

            GetLocalPoint(mv3LocalContactPosition,
                mv3TargetPosition,
                m_v3Position,
                m_aActualFacingDirection);

            nlVector3 v3CurLocalPos;
            GetLocalPoint(v3CurLocalPos,
                v3BallPosition,
                m_v3Position,
                m_aActualFacingDirection);

            bool bInFront = v3CurLocalPos.x >= 0.0f;
            mpLooseBallInfo = LooseBallAnims::FindLooseBallAnim(
                mv3LocalContactPosition, bInFront, 0.0f);
        }
        else
        {
            mpLooseBallInfo
                = &LooseBallAnims::mLooseBallKickInfo[1];
        }

        float fInterceptTime;
        float fClosestDist;
        bool bFound = fn_8016EA10(m_v3Position,
            1.0f,
            6.0f,
            mv3TargetPosition,
            mv3TargetVelocity,
            fInterceptTime,
            fClosestDist,
            3.0f);

        if (bFound && mv3TargetPosition.z < 1.0f
            && fClosestDist < 0.4f)
        {
            nlVector2 v2Distance;
            v2Distance.x
                = mv3TargetPosition.x - m_v3Position.x;
            v2Distance.y
                = mv3TargetPosition.y - m_v3Position.y;
            float fPickupDist = mpLooseBallInfo->mfPickupDistance;
            float fReachDist = 0.4f + fPickupDist;
            float fDistSq = nlVec2LengthSquared(v2Distance);
            float fReachDistSq
                = nlGetLengthSquared1D(fReachDist);
            float fAnimTime = mpLooseBallInfo->mfPickupTime
                            * mpLooseBallInfo->mfAnimDuration;
            float fTargetTime = fInterceptTime - fAnimTime;

            if (fDistSq <= fReachDistSq && fTargetTime < 0.02f)
            {
                float fDist = nlSqrt(fDistSq, true);
                InitActionLooseBallPickup(fDist, false);
                return;
            }

            InitActionLooseBallPursueRolling();
            return;
        }

        if (mGoalieActionState == GOALIEACTION_MOVE)
        {
            return;
        }
        InitActionMove(true);
        return;
    }

    int nNumSolutions;
    float pSolutions[2];
    CalcInterceptXY(m_v3Position,
        0.85f * ((GoalieTweaks*)m_pTweaks)->fRunningSpeed,
        0.5f,
        v3BallPosition,
        *pBallVelocity,
        nNumSolutions,
        pSolutions);

    if (nNumSolutions != 0)
    {
        float fBestTime;
        if (nNumSolutions == 2)
        {
            fBestTime = pSolutions[0] < pSolutions[1]
                          ? pSolutions[0]
                          : pSolutions[1];
        }
        else
        {
            fBestTime = pSolutions[0];
        }

        if (fBestTime < 5.0f)
        {
            nlVector3 v3IntPos;
            nlVector3 v3IntVel;
            float fTargetHeight;
            float fHeightTime = fn_8016D52C(
                3.0f, fBestTime, v3IntPos, v3IntVel, fTargetHeight, false);

            if (fHeightTime >= 0.0f)
            {
                if (fn_8007B9A0(this, v3IntPos, 8.0f))
                {
                    if (fTargetHeight < 3.0f)
                    {
                        SetGoalieAction(
                            GOALIEACTION_LOOSEBALL_PURSUE_BOUNCING,
                            0);
                        mv3TargetPosition = v3IntPos;
                        mfTargetTime = fHeightTime;
                        mfTargetDist = 1.4f;
                        mbIsDown = false;

                        float fDyFace
                            = v3IntPos.y - m_v3Position.y;
                        float fDxFace
                            = v3IntPos.x - m_v3Position.x;
                        m_aDesiredFacingDirection
                            = (u16)(s32)(nlATan2f(
                                             fDyFace, fDxFace)
                                         * 10430.378f);

                        s16 nAngDiff
                            = m_aDesiredFacingDirection
                            - m_aActualFacingDirection;
                        int nAnimID = ChooseRunAnim(
                            nAngDiff, v3IntPos, 1.0f);
                        PlayNewAnim(nAnimID);
                        InitMovementFromAnimSeek(
                            ((GoalieTweaks*)m_pTweaks)
                                ->fRunningDirectionSeekSpeed,
                            ((GoalieTweaks*)m_pTweaks)
                                ->fRunningDirectionSeekFalloff);
                        return;
                    }

                    mbShouldMiss = false;
                    fn_8008CED8(fHeightTime, v3IntPos, v3IntVel);
                    return;
                }

                if (mGoalieActionState == GOALIEACTION_MOVE)
                {
                    return;
                }
                InitActionMove(true);
                return;
            }
        }
    }

    SetGoalieAction(GOALIEACTION_LOOSEBALL_SETUP, 0);
    float fDyFace = v3BallPosition.y - m_v3Position.y;
    float fDxFace = v3BallPosition.x - m_v3Position.x;
    m_aDesiredFacingDirection
        = (u16)(s32)(nlATan2f(fDyFace, fDxFace) * 10430.378f);

    s16 nAngDiff
        = m_aDesiredFacingDirection - m_aActualFacingDirection;
    int nAnimID = ChooseRunAnim(nAngDiff, v3BallPosition, 1.0f);
    PlayNewAnim(nAnimID);
    InitMovementFromAnimSeek(
        ((GoalieTweaks*)m_pTweaks)->fRunningDirectionSeekSpeed,
        ((GoalieTweaks*)m_pTweaks)->fRunningDirectionSeekFalloff);
}

void Goalie::fn_8008CD08()
{
    unsigned short aNetFacing
        = m_v3Position.x > 0.0f ? 0x8000 : 0;
    float fDx
        = g_pBall->m_v3Position.x - mv3TargetPosition.x;
    float fDy
        = g_pBall->m_v3Position.y - mv3TargetPosition.y;
    float fGoalLimit
        = cField::GetGoalLineX(1U) - lbl_806DBCC8;
    float fAbsTargetX = (float)fabs(mv3TargetPosition.x);

    if (fDx * m_v3Position.x > 0.0f)
    {
        fDx = 0.0f;
    }

    if (fDx * fDx + fDy * fDy > 0.01f)
    {
        mUnidentified3BE
            = (u16)(s32)(nlATan2f(fDy, fDx) * 10430.378f);
        maInitialAngle
            = mUnidentified3BE < 0x8000 ? 0x4000 : 0xC000;
    }
    else
    {
        mUnidentified3BE = aNetFacing;
        maInitialAngle = mv3TargetPosition.y * m_v3Position.x > 0.0f
                           ? 0x4000
                           : 0xC000;
    }

    float fBlend = InterpolateRangeClamped(
        0.0f, 1.0f, fGoalLimit, fGoalLimit + 2.0f, fAbsTargetX);
    short aAngleDiff
        = (short)(maInitialAngle - mUnidentified3BE);
    maInitialAngle
        = mUnidentified3BE + (short)(s32)(fBlend * aAngleDiff);
}

void Goalie::fn_8008CED8(float fTargetTime,
    const nlVector3& v3TargetPosition,
    const nlVector3& v3TargetVelocity)
{
    SetGoalieAction(GOALIEACTION_UNIDENTIFIED_20, 0);
    mfWaitTime = fTargetTime;
    mfTargetTime = fTargetTime;
    mv3TargetPosition = v3TargetPosition;
    mv3TargetVelocity = v3TargetVelocity;

    if (mbShouldMiss)
    {
        float fAbsBallX = (float)fabs(g_pBall->m_v3Position.x);
        float fLimitX
            = cField::GetGoalLineX(1U) - lbl_806DBCFC;
        if (fAbsBallX < fLimitX)
        {
            nlVector2 v2Distance;
            v2Distance.x = m_v3Position.x - v3TargetPosition.x;
            v2Distance.y = m_v3Position.y - v3TargetPosition.y;
            if (nlVec2LengthSquared(v2Distance)
                > lbl_806DBD00 * lbl_806DBD00)
            {
                GetLocalPoint(mv3LocalContactPosition,
                    v3TargetPosition,
                    m_v3Position,
                    m_aActualFacingDirection);
                InitActionChipShotStumble(fTargetTime);
                return;
            }
        }
    }

    mbIsDown = false;
    mbTryLobSave = true;
    muBallDeflectCount = g_pBall->m_bBallDeflectCount;
    mUrgency = URGENCY_MED;
    mbDoHeadTrack = true;
    mnSubstate = 1;
    mMoveDirection = GOALIEDIR_IDLE;
    mpSaveData = 0;

    float fHeightRatio;
    if (mv3TargetPosition.z > 2.0f)
    {
        fHeightRatio = 1.0f;
    }
    else
    {
        float fParam2 = nlMaxEquals(
            lbl_806DBCC4, 0.1f + lbl_806DBCC0);
        fHeightRatio = fn_8007ACB8(this,
            mv3TargetPosition,
            lbl_806DBCC0,
            fParam2);
    }

    SaveData* pOtherAnim;
    if (mUnidentified254 != 0)
    {
        mLowLobAnim = 0x5D;
        pOtherAnim = fn_80093780(0x5B);
    }
    else
    {
        mLowLobAnim = nlRandomf(1.0f) < 0.5f ? 0x86 : 0x74;
        pOtherAnim = fn_80093780(0x2D);
    }

    SaveData* pAnim = fn_80093780(mLowLobAnim);
    float fPredictionHeight = Interpolate(pAnim->mv3SavePos.z,
        pOtherAnim->mv3SavePos.z,
        fHeightRatio);

    fn_8016EEC8();
    nlVector3 v3PredictedPosition;
    nlVector3 v3PredictedVelocity;
    float fTargetHeight;
    float fPredictedTime = fn_8016D52C(fPredictionHeight,
        0.2f,
        v3PredictedPosition,
        v3PredictedVelocity,
        fTargetHeight,
        true);
    fn_8016F06C();

    if (fPredictedTime > 0.0f)
    {
        mv3TargetPosition = v3PredictedPosition;
        mv3TargetVelocity = v3PredictedVelocity;
        mfWaitTime = fPredictedTime;
        mfTargetTime = fPredictedTime;
    }

    fn_8008CD08();

    unsigned short aNetFacing
        = m_v3Position.x > 0.0f ? 0x8000 : 0;
    nlVector3 v3Local = {
        -lbl_806DBCCC,
        lbl_806DBCD0,
        0.0f,
    };
    if ((short)(aNetFacing - maInitialAngle) < 0)
    {
        v3Local.y = -v3Local.y;
    }

    GetWorldPoint(mv3NavTarget,
        v3Local,
        mv3TargetPosition,
        maInitialAngle);
    mv3NavTarget.z = 0.0f;
    m_pPhysicsCharacter->m_CanCollideWithGoalLine = false;
    m_pPhysicsCharacter->m_CanCollideWithWall = false;
}

void Goalie::fn_8008D210(float fDeltaT)
{
    if (mnOffplayPending != GOALIE_OFFPLAY_NONE
        || g_pBall->GetOwnerFielder() != 0)
    {
        InitActionMove(false);
        return;
    }

    bool bPredictionChanged = false;
    float fMoveSpeed = 0.2f;

    if (muBallDeflectCount != g_pBall->m_bBallDeflectCount)
    {
        InitActionMove(false);
        return;
    }

    mfWaitTime -= fDeltaT;

    fn_8016EEC8();

    nlVector3 v3ObservedPosition;
    nlVector3 v3ObservedVelocity;
    float fTargetHeight;
    fn_8016D52C(mv3TargetPosition.z,
        0.04f,
        v3ObservedPosition,
        v3ObservedVelocity,
        fTargetHeight,
        true);

    nlVector3 v3PredictionDelta;
    nlVec3Sub(v3PredictionDelta,
        v3ObservedPosition,
        mv3TargetPosition);
    if (nlVec3LengthSquared(v3PredictionDelta) > 0.09f)
    {
        bPredictionChanged = true;
    }

    unsigned short aNetFacing
        = m_v3Position.x > 0.0f ? 0x8000 : 0;
    bool bPositiveSide
        = (short)(aNetFacing - maInitialAngle) > 0;

    if ((bPredictionChanged || mfWaitTime > 0.3f)
        && lbl_806E0D21 == 0)
    {
        float fHeightRatio;
        float fAbsTargetX = (float)fabs(mv3TargetPosition.x);
        if (fAbsTargetX > cField::GetGoalLineX(1U) - 4.0f)
        {
            fHeightRatio = 1.0f;
        }
        else
        {
            float fParam2 = nlMaxEquals(
                0.1f + lbl_806DBCC4, lbl_806DBCC0);
            fHeightRatio = fn_8007ACB8(this,
                mv3TargetPosition,
                lbl_806DBCC0,
                fParam2);
        }

        SaveData* pLowLobSave = fn_80093780(mLowLobAnim);
        SaveData* pHighLobSave = fn_80093780(0x5B);
        float fPredictionHeight = Interpolate(
            pLowLobSave->mv3SavePos.z,
            pHighLobSave->mv3SavePos.z,
            fHeightRatio);

        if (bPredictionChanged
            || (float)fabs(
                   fPredictionHeight - mv3TargetPosition.z)
                   > 0.5f)
        {
            nlVector3 v3PredictedPosition;
            nlVector3 v3PredictedVelocity;
            float fPredictedTime = fn_8016D52C(fPredictionHeight,
                0.08f,
                v3PredictedPosition,
                v3PredictedVelocity,
                fTargetHeight,
                true);
            if (fPredictedTime > 0.0f)
            {
                mfWaitTime = fPredictedTime;
                mv3TargetPosition = v3PredictedPosition;
                mv3TargetVelocity = v3PredictedVelocity;

                fn_8008CD08();

                bPositiveSide
                    = (short)(aNetFacing - maInitialAngle) > 0;
                nlVector3 v3Local = {
                    -lbl_806DBCCC,
                    -lbl_806DBCD0,
                    0.0f,
                };
                if (bPositiveSide)
                {
                    v3Local.y = lbl_806DBCD0;
                }

                GetWorldPoint(mv3NavTarget,
                    v3Local,
                    mv3TargetPosition,
                    maInitialAngle);
                mv3NavTarget.z = 0.0f;
                mpSaveData = 0;
            }
        }
    }

    bool bWallBlock = mfWallBlock > 0.0f;
    if (bWallBlock)
    {
        fMoveSpeed = 10.0f;
    }

    m_aDesiredFacingDirection
        = (u16)(s32)InterpolateRangeClamped(
            (float)mUnidentified3BE,
            (float)maInitialAngle,
            mfTargetTime,
            0.3f,
            mfWaitTime);
    fn_800797DC(this, 0, fDeltaT, fMoveSpeed);

    GetLocalPoint(mv3LocalContactPosition,
        mv3TargetPosition,
        m_v3Position,
        maInitialAngle);

    bool bUseShortSave = true;
    bool bSubstateOne = mnSubstate == 1;
    if (mUnidentified254 == 0 && lbl_806E0D20 == 0)
    {
        bUseShortSave = false;
    }

    bool bNeedsFallback = false;
    if (mpSaveData == 0 || mpSaveData->muSaveType != 4)
    {
        if (bUseShortSave)
        {
            nlVector2 v2Distance;
            v2Distance.x = m_v3Position.x - mv3NavTarget.x;
            v2Distance.y = m_v3Position.y - mv3NavTarget.y;
            if (nlVec2LengthSquared(v2Distance) < 2.25f)
            {
                if (bPositiveSide)
                {
                    mpSaveData = fn_80093780(0x61);
                }
                else
                {
                    mpSaveData = fn_80093780(0x5C);
                }
                mpSaveData = fn_80092644(mpSaveData,
                    &mBlendInfo,
                    &mv3LocalContactPosition);
                GetWorldPoint(mv3NavTarget,
                    mBlendInfo.mv3BlendedSavePos,
                    mv3TargetPosition,
                    maInitialAngle + 0x8000);
                mv3NavTarget.z = 0.0f;
            }
            else if (mfWaitTime <= 0.3f)
            {
                bNeedsFallback = true;
            }
        }
        else if (mpSaveData == 0
                 && (bSubstateOne || mfWaitTime <= 0.3f))
        {
            nlVector2 v2Distance;
            v2Distance.x = m_v3Position.x - mv3NavTarget.x;
            v2Distance.y = m_v3Position.y - mv3NavTarget.y;
            if (bSubstateOne
                || nlVec2LengthSquared(v2Distance) < 1.44f)
            {
                mpSaveData = fn_80093780(0x86);
                if (0.2f + mpSaveData->mv3SavePos.z
                    < mv3TargetPosition.z)
                {
                    if (bPositiveSide)
                    {
                        mpSaveData = fn_80093780(0x33);
                    }
                    else
                    {
                        mpSaveData = fn_80093780(0x2E);
                    }
                    mpSaveData = fn_80092644(mpSaveData,
                        &mBlendInfo,
                        &mv3LocalContactPosition);
                }
                else if (lbl_8056D3B0.z >= mv3TargetPosition.z)
                {
                    mpSaveData = fn_800925C0(
                        &mBlendInfo, &mv3LocalContactPosition);
                }
                else
                {
                    mpSaveData = fn_80092644(mpSaveData,
                        &mBlendInfo,
                        &mv3LocalContactPosition);
                }

                GetWorldPoint(mv3NavTarget,
                    mBlendInfo.mv3BlendedSavePos,
                    mv3TargetPosition,
                    maInitialAngle + 0x8000);
                mv3NavTarget.z = 0.0f;
            }
            else
            {
                bNeedsFallback = true;
            }
        }
    }

    if (mpSaveData == 0 && bNeedsFallback)
    {
        SaveData* pNearSave = fn_80093780(0x47);
        nlVector3 v3PredictedPosition;
        nlVector3 v3PredictedVelocity;
        float fPredictedTime = fn_8016D52C(
            pNearSave->mv3SavePos.z - 0.2f,
            0.08f,
            v3PredictedPosition,
            v3PredictedVelocity,
            fTargetHeight,
            true);
        if (fPredictedTime > 0.0f)
        {
            mfWaitTime = fPredictedTime;
            mv3TargetPosition = v3PredictedPosition;
            mv3TargetVelocity = v3PredictedVelocity;
        }

        nlVector3 v3Direction;
        nlVec3Sub(v3Direction, mv3TargetPosition, m_v3Position);
        nlVector3 v3PlaneNormal = {
            -v3Direction.y,
            v3Direction.x,
            0.0f,
        };
        nlVector4 plane;
        MakePerpendicularPlane(
            m_v3Position, v3PlaneNormal, plane, 0.0f);
        float fBallPlaneDistance
            = g_pBall->m_v3Position.x * plane.x
            + g_pBall->m_v3Position.y * plane.y
            + g_pBall->m_v3Position.z * plane.z - plane.w;
        if (fBallPlaneDistance <= 0.0f)
        {
            mpSaveData = pNearSave;
        }
        else
        {
            mpSaveData = fn_80093780(0x4A);
        }

        unsigned short aAnimAngle
            = (u16)(s32)(nlATan2f(mpSaveData->mv3SavePos.y,
                             mpSaveData->mv3SavePos.x)
                         * 10430.378f);
        unsigned short aTargetAngle
            = (u16)(s32)(nlATan2f(v3Direction.y, v3Direction.x)
                         * 10430.378f);
        maInitialAngle = aTargetAngle - aAnimAngle;

        GetLocalPoint(mv3LocalContactPosition,
            mv3TargetPosition,
            m_v3Position,
            maInitialAngle);
        mpSaveData = fn_80092644(mpSaveData,
            &mBlendInfo,
            &mv3LocalContactPosition);
        GetWorldPoint(mv3NavTarget,
            mBlendInfo.mv3BlendedSavePos,
            mv3TargetPosition,
            maInitialAngle + 0x8000);
        mv3NavTarget.z = 0.0f;
    }

    fn_8016F06C();

    if (mpSaveData == 0)
    {
        return;
    }

    if (!(mfWaitTime + 0.01f
            <= mBlendInfo.mfMilestoneTime[2]))
    {
        return;
    }

    nlVector2 v2SaveDistance;
    v2SaveDistance.x
        = mv3LocalContactPosition.x - mBlendInfo.mv3BlendedSavePos.x;
    v2SaveDistance.y
        = mv3LocalContactPosition.y - mBlendInfo.mv3BlendedSavePos.y;
    if (nlVec2LengthSquared(v2SaveDistance) > 9.0f)
    {
        InitActionMove(false);
        return;
    }

    SetGoalieAction(GOALIEACTION_UNIDENTIFIED_21, 0);
    m_aDesiredFacingDirection = maInitialAngle;

    float fStartTime = mBlendInfo.mfMilestoneTime[2]
                     - mfWaitTime - 0.01f;
    fStartTime = nlMaxEquals(fStartTime, 0.0f);
    fStartTime = nlMinEquals(
        fStartTime, mBlendInfo.mfMilestoneTime[1]);
    PlayBlendedAnims(
        mBlendInfo.mfStartTime = fStartTime, 2.5f, -1);
    mbBallImpacted = false;
    mBallsLaunched = 0;
    mbIsDown = true;
}

void Goalie::fn_8008DAB4(float fDeltaT)
{
    bool bActionStateActive = false;
    bool bUnidentifiedCondition = true;
    float fAnimTime = m_pCurrentAnimController->m_fTime;
    if (lbl_806E0C94->mUnidentified20
        || lbl_806E0C94->mUnidentified18 == 3)
    {
        bActionStateActive = true;
    }

    if (!bActionStateActive
        && mnOffplayPending == GOALIE_OFFPLAY_NONE)
    {
        bUnidentifiedCondition = false;
    }

    if (mbDoHeadTrack)
    {
        nlVector3 v3Facing;
        v3Facing.z = m_m4WorldMatrix.e2[0][2];
        v3Facing.y = m_m4WorldMatrix.e2[0][1];
        v3Facing.x = m_m4WorldMatrix.e2[0][0];

        nlVector3 v3BallDir;
        v3BallDir.x
            = g_pBall->m_v3Position.x - m_v3Position.x;
        v3BallDir.y
            = g_pBall->m_v3Position.y - m_v3Position.y;
        v3BallDir.z = 0.0f;
        if (nlVec3LengthSquared(v3BallDir) < 4.0f
            || nlVec3DotProduct(v3BallDir, v3Facing) < 0.0f)
        {
            mbDoHeadTrack = false;
        }
    }

    if (m_pBall == 0)
    {
        float fGoalTime = mpSaveData->mfMilestonePercent[2];
        if (fAnimTime < 0.5f * fGoalTime)
        {
            float t = 2.0f * fAnimTime / fGoalTime;
            t = nlMaxEquals(t, 0.0f);
            t = nlMinEquals(t, 1.0f);
            short delta = (short)(m_aDesiredFacingDirection
                                  - m_aActualFacingDirection);
            int adjustedDelta
                = ((int)(1024.0f
                         * (t * (t * ((-2.0f * t) + 3.0f))))
                      * delta)
                / 1024;
            unsigned short newFacing
                = adjustedDelta + m_aActualFacingDirection;
            SetFacingDirection(newFacing, true);
        }
    }

    if (!bUnidentifiedCondition
        && g_pBall->m_pOwner != this
        && mpSaveData->muSaveType != 4)
    {
        float fDX = lbl_806DBCDC * lbl_806DBCDC;
        const nlVector3& v3LHand
            = GetJointPosition(m_nLeftHandJointIndex);
        const nlVector3& v3RHand
            = GetJointPosition(m_nRightHandJointIndex);
        float distSqL = CalculateDistanceSquared(
            g_pBall->m_v3Position, v3LHand);

        if (distSqL < fDX
            || CalculateDistanceSquared(
                   g_pBall->m_v3Position, v3RHand)
                   < fDX)
        {
            TacklePlayer(g_pBall->m_pOwner);
            StealBall(g_pBall->m_pOwner);
            fn_8007DCD8(this, false);
            PickupBall(g_pBall);
            m_pPhysicsCharacter->m_CanCollideWithGoalLine = true;
            m_pPhysicsCharacter->m_CanCollideWithWall = true;
            EmitGoalieCatch(this, "goalie_catch", false);
            mbBallImpacted = true;
        }
    }

    if (mbBallImpacted)
    {
        if (mpSaveData->muSaveType == 4)
        {
            if (bUnidentifiedCondition)
            {
                if (m_pBall != 0)
                {
                    fn_8007EB90(this);
                }
            }
            else
            {
                if (++mBallsLaunched <= lbl_806DBCF4)
                {
                    if (m_pBall != 0
                        && mBallsLaunched == lbl_806DBCF4)
                    {
                        fn_8008DEF4(1.0f);
                    }
                }
                else
                {
                    CheckForLimbEndZoneCollision();
                }
            }
        }
        else
        {
            CheckForLimbEndZoneCollision();
        }
    }
    else if (fAnimTime
             > mpSaveData->mfMilestonePercent[2] + 0.2f)
    {
        CheckForLimbEndZoneCollision();
    }

    if (!bUnidentifiedCondition
        && (mpSaveData->muSaveType & 3) != 0
        && fAnimTime > mpSaveData->mfMilestonePercent[3] + 0.08f
        && m_pBall == 0)
    {
        InitActionDiveRecover();
        return;
    }

    cPN_SAnimController* pController
        = m_pCurrentAnimController;
    bool bAnimFinished = false;
    if (pController->m_ePlayMode == PM_HOLD
        && pController->m_fTime == 1.0f)
    {
        bAnimFinished = true;
    }

    if (bAnimFinished)
    {
        InitActionDiveRecover();
    }
}

void Goalie::fn_8008DEF4(float fParam)
{
    if (m_pBall != 0)
    {
        ReleaseBall(4);
    }
    else if (g_pBall->m_unk_0xA4 != 4)
    {
        fn_80015C38(g_pBall, 4);
    }

    unsigned int aLaunchDirection;
    unsigned short aActualFacingDirection
        = m_aActualFacingDirection;
    unsigned short aAngleRange
        = (unsigned short)(((int)(65536.0f
                               * (float)lbl_806DBCF0))
                              / 360);
    aLaunchDirection
        = (unsigned short)(aActualFacingDirection
            + (nlRandom(2
                   * (unsigned short)(((int)(65536.0f
                                          * (float)lbl_806DBCF0))
                                         / 360))
                - aAngleRange));

    if (m_v3Position.x > 0.0f)
    {
        aLaunchDirection = aLaunchDirection < 0x4000
            ? 0x4000
            : aLaunchDirection;
        aLaunchDirection
            = (unsigned short)aLaunchDirection > 0xC000
            ? 0xC000
            : (unsigned short)aLaunchDirection;
    }
    else
    {
        aLaunchDirection
            = (unsigned short)(aLaunchDirection + 0x8000);
        aLaunchDirection = aLaunchDirection < 0x4000
            ? 0x4000
            : aLaunchDirection;
        aLaunchDirection
            = (unsigned short)aLaunchDirection > 0xC000
            ? 0xC000
            : (unsigned short)aLaunchDirection;
        aLaunchDirection += 0x8000;
    }

    float fSpeedRange = lbl_806DBCE4 - lbl_806DBCE0;
    float fSpeed = lbl_806DBCE0
        + nlRandomf(nlMaxEquals(fSpeedRange, 0.0f));
    nlVector3 v3BallVelocity;
    v3BallVelocity.x = fParam * fSpeed;
    v3BallVelocity.y = 0.0f;
    v3BallVelocity.z = fParam
        * (lbl_806DBCE8
            + nlRandomf(lbl_806DBCEC - lbl_806DBCE8));
    GetWorldPoint(v3BallVelocity, v3BallVelocity, v3Zero,
        aLaunchDirection);
    g_pBall->m_tNoPickupTimer.SetSeconds(0.1f);
    fn_80098468(this, 0.1f);
    g_pBall->SetVelocity(
        v3BallVelocity, SPINTYPE_FORWARD, 0);
}

void Goalie::fn_8008E130()
{
    switch (mGoalieActionState)
    {
    case GOALIEACTION_UNIDENTIFIED_26:
        break;
    default:
    {
        SetGoalieAction(GOALIEACTION_UNIDENTIFIED_27, 0);
        m_pCurrentAnimController->m_fPlaybackSpeedScale
            = lbl_806DBC6C;

        int nNodeIndex = m_nBip01JointIndex_0xA4;
        mbIsDown = true;
        mUnidentified1F5 = true;
        mUnidentified1F0 = 0.0f;
        mUnidentified1F6 = true;
        fn_80097574(this, nNodeIndex, 0xAF, 0.0f);

        while (nNodeIndex >= 0)
        {
            mUnidentified2F0->SetNodeWeight(
                nNodeIndex, 0.0f);
            nNodeIndex
                = m_pPoseAccumulator->m_pHierarchy->GetParent(
                    nNodeIndex);
        }

        cCharacter* pPreviousCharacter = lbl_806E0C34;
        lbl_806E0C34 = this;
        fn_8001EF78(this, 0.0f);
        lbl_806E0C34 = pPreviousCharacter;

        SetVelocity(v3Zero);
        fn_801B968C(this);
        mbDoHeadTrack = false;

        nlVector3 v3Position = m_v3Position;
        mfTargetDist = 1.0f;
        v3Position.z = 1.0f;
        SetPosition(v3Position);

        if (m_pBall != 0)
        {
            fn_8007EB90(this);
        }

        switch (mGoalieActionState)
        {
        case GOALIEACTION_MOVE_WB:
        case GOALIEACTION_LOOSEBALL_CATCH:
        case GOALIEACTION_LOOSEBALL_PICKUP:
        case GOALIEACTION_SNAP_BALL:
            InitActionMove(false);
            break;
        default:
            break;
        }

        bool bHasGlobalPad = GetGlobalPad() != 0;
        if (bHasGlobalPad)
        {
            fn_8009591C(this, false);
        }
        break;
    }
    }
}

void Goalie::fn_8008E2D0()
{
    switch (mGoalieActionState)
    {
    case GOALIEACTION_UNIDENTIFIED_26:
        break;
    default:
        CleanupStun();
        ChooseSwatAnim(1);
        mPrevGoalieActionState = mGoalieActionState;
        mGoalieActionState = GOALIEACTION_UNIDENTIFIED_28;
        mFreezeTimer.SetSeconds(lbl_806DBCA0);
        mbIsDown = true;
        m_fDesiredSpeed = 0.0f;
        m_fActualSpeed = 0.0f;
        SetVelocity(v3Zero);
        mUnidentified1F5 = true;
        mUnidentified1F0 = 0.0f;
        if (GetGlobalPad() != 0)
        {
            fn_8009591C(this, false);
        }
        break;
    }
}

void Goalie::InitActionSTSAttackSetup(float fWaitTime)
{
    mbIsDown = false;
    mfWaitTime = fWaitTime;
    mfTargetTime = nlMaxEquals(fWaitTime, 0.25f);
    mpLooseBallInfo = &LooseBallAnims::mAttackSTSInfo;
    SetGoalieAction(GOALIEACTION_STS_ATTACK_SETUP, 0);
    mUrgency = URGENCY_LOW;
    ActionSTSAttackSetup(0.0f);

    PlayerAttackData data;
    data.pAttacker = this;
    data.nAttackerPadID = -1;
    data.pTarget = g_pBall->GetOwnerFielder();
    data.mUnidentified0C = 2;
    data.mUnidentified10 = false;
    fn_8005E800(lbl_806E0C94, &data);
}

void Goalie::InitActionSTSAttack()
{
    mpShooter = g_pBall->GetOwnerFielder();
    mbDoNavigate = false;

    nlVector2 v2Distance;
    v2Distance.x = m_v3Position.x - mpShooter->m_v3Position.x;
    v2Distance.y = m_v3Position.y - mpShooter->m_v3Position.y;
    float fDistanceSq = nlVec2LengthSquared(v2Distance);
    float fRadius;
    mpShooter->m_pPhysicsCharacter->GetRadius(&fRadius);

    bool bInRange = false;
    if (mpShooter->m_eCharacterClass != (eCharacterClass)13
        || mpShooter->m_eActionState != (eFielderActionState)32
        || fn_800DEB04(mpShooter) > 0.8f)
    {
        mpLooseBallInfo = &LooseBallAnims::mLooseBallKickInfo[2];
        float fPickupDist
            = mpLooseBallInfo->mfPickupDistance + fRadius;
        mfTargetDist = lbl_806DBD1C + fPickupDist;
        float fReachDistSq
            = nlGetLengthSquared1D(mfTargetDist);
        if (fDistanceSq < fReachDistSq)
        {
            bInRange = true;
        }
    }
    else
    {
        mpLooseBallInfo = &LooseBallAnims::mUnknownD0BC;
        float fPickupDist
            = mpLooseBallInfo->mfPickupDistance + fRadius;
        mfTargetDist = lbl_806DBD18 + fPickupDist;
        float fReachDistSq
            = nlGetLengthSquared1D(mfTargetDist);
        if (fDistanceSq < fReachDistSq)
        {
            bInRange = true;
        }
    }

    mv3NavTarget = mpShooter->m_v3Position;
    float deltaX = mv3NavTarget.x - m_v3Position.x;
    float deltaY = mv3NavTarget.y - m_v3Position.y;
    m_aDesiredFacingDirection
        = (u16)(s32)(10430.378f
            * nlATan2f(deltaY, deltaX));
    mUrgency = URGENCY_HIGH;
    SetGoalieAction(GOALIEACTION_UNIDENTIFIED_36, 0);

    if (bInRange)
    {
        mbIsDown = true;
        mbDoHeadTrack = false;
        mbPickedUp = false;
        SetGoalieAction(GOALIEACTION_STS_ATTACK, 0);
        SetAnimState(mpLooseBallInfo->mnAnimID,
            true, 0.2f, false, false);
        InitMovementFromAnimSeek(
            ((GoalieTweaks*)m_pTweaks)->fRunningDirectionSeekSpeed,
            ((GoalieTweaks*)m_pTweaks)->fRunningDirectionSeekFalloff);
    }
    else
    {
        mbIsDown = false;
        mbDoHeadTrack = true;
        SetAnimState(0x24, true, 0.2f, false, false);
        m_pCurrentAnimController->m_fPlaybackSpeedScale = 1.5f;
        m_aDesiredFacingDirection = m_aActualFacingDirection;
        InitMovementFromAnimSeek(
            ((GoalieTweaks*)m_pTweaks)->fRunningDirectionSeekSpeed,
            ((GoalieTweaks*)m_pTweaks)->fRunningDirectionSeekFalloff);
    }
}

void Goalie::fn_8008E69C(float fDeltaT)
{
    if (fn_8007C73C())
    {
        CleanupStun();
        ChooseSwatAnim(1);
        SetGoalieAction(GOALIEACTION_UNIDENTIFIED_31, 0);
        m_fDesiredSpeed = 0.0f;
        m_fActualSpeed = 0.0f;
        SetVelocity(v3Zero);

        if (m_pBall != 0)
        {
            fn_80139D1C(1, GetGlobalPad());
            ReleaseBall(0);
        }

        fn_80098468(this, 0.2f);
        if (GetGlobalPad() != 0)
        {
            fn_8009591C(this, false);
        }
        mbGrabMonty = false;
        return;
    }

    if (fn_8007BC40())
    {
        return;
    }

    if (mnOffplayPending != GOALIE_OFFPLAY_NONE
        || mpShooter != g_pBall->GetOwnerFielder()
        || fn_8007C904(this, mpShooter->m_v3Position))
    {
        InitActionMove(false);
        return;
    }

    if (IsOpponentInSTS() && CheckForSTSAttack())
    {
        return;
    }

    nlVector2 v2Distance;
    v2Distance.x = m_v3Position.x - mpShooter->m_v3Position.x;
    v2Distance.y = m_v3Position.y - mpShooter->m_v3Position.y;
    float fDistanceSq = nlVec2LengthSquared(v2Distance);
    float fTargetDistSq = nlGetLengthSquared1D(mfTargetDist);

    if (fDistanceSq < fTargetDistSq)
    {
        mbIsDown = true;
        mbDoHeadTrack = false;
        mbPickedUp = false;
        SetGoalieAction(GOALIEACTION_STS_ATTACK, 0);
        SetAnimState(mpLooseBallInfo->mnAnimID,
            true, 0.2f, false, false);
        InitMovementFromAnimSeek(
            ((GoalieTweaks*)m_pTweaks)->fRunningDirectionSeekSpeed,
            ((GoalieTweaks*)m_pTweaks)->fRunningDirectionSeekFalloff);
    }
    else
    {
        float dx = mpShooter->m_v3Position.x - m_v3Position.x;
        float dy = mpShooter->m_v3Position.y - m_v3Position.y;
        m_aDesiredFacingDirection
            = (u16)(s32)(10430.378f * nlATan2f(dy, dx));
    }
}

void Goalie::ActionSTSAttackSetup(float deltaTime)
{
    if (!IsOpponentInSTS())
    {
        InitActionMove(false);
        return;
    }

    if (fn_8007C904(
            this, g_pBall->GetOwnerFielder()->m_v3Position))
    {
        InitActionMove(false);
        return;
    }

    mfWaitTime -= deltaTime;
    if (mfWaitTime <= 0.0)
    {
        mbIsDown = true;
        mbDoHeadTrack = false;
        SetGoalieAction(GOALIEACTION_UNIDENTIFIED_33, 0);
        SetAnimState(mpLooseBallInfo->mnAnimID,
            true, 0.2f, false, false);
        InitMovementFromAnim(0, v3Zero, 1.0f, false);

        cFielder* pOwnerFielder = g_pBall->GetOwnerFielder();
        nlVector2 v2Distance;
        v2Distance.x
            = pOwnerFielder->m_v3Position.x - m_v3Position.x;
        v2Distance.y
            = pOwnerFielder->m_v3Position.y - m_v3Position.y;
        float distSq = nlVec2LengthSquared(v2Distance);
        mfTargetDist = nlSqrt(distSq, true)
            - mpLooseBallInfo->mfPickupDistance;
        mpShooter = pOwnerFielder;
        return;
    }

    cFielder* pOwnerFielder = g_pBall->GetOwnerFielder();
    int animID = 5;
    bool bWallBlock = mfWallBlock > 0.0f;
    if (!bWallBlock)
    {
        nlVector2 v2Distance;
        v2Distance.x
            = m_v3Position.x - pOwnerFielder->m_v3Position.x;
        v2Distance.y
            = m_v3Position.y - pOwnerFielder->m_v3Position.y;
        float pickupDistSq = nlGetLengthSquared1D(
            mpLooseBallInfo->mfPickupDistance);
        float distSq = nlVec2LengthSquared(v2Distance);
        if (distSq > pickupDistSq)
        {
            animID = 0x18;
        }
    }

    PlayNewAnim(animID);
    InitMovementFromAnim(0, v3Zero, 1.0f, false);

    GetLocalPoint(mv3LocalContactPosition,
        pOwnerFielder->m_v3Position, m_v3Position,
        m_aActualFacingDirection);

    float angle = nlATan2f(
        mv3LocalContactPosition.y, mv3LocalContactPosition.x);
    float progressRatio = (mfTargetTime - mfWaitTime) / mfTargetTime;
    s16 angleDeltaInt = (s16)(u16)(s32)(10430.378f * angle);

    progressRatio = nlMaxEquals(progressRatio, 0.0f);
    progressRatio = nlMinEquals(progressRatio, 1.0f);

    s32 multiplierInt = (s32)(1024.0f
        * (progressRatio
            * (progressRatio
                * ((-2.0f * progressRatio) + 3.0f))));
    s32 adjustedDelta = (multiplierInt * angleDeltaInt) / 1024;
    u16 newFacing = adjustedDelta + m_aActualFacingDirection;

    SetFacingDirection(newFacing, true);
    m_aDesiredFacingDirection = newFacing;
}

void Goalie::fn_8008895C(float deltaTime)
{
    if (m_v3Position.z > 0.0f)
    {
        nlVector3 position = m_v3Position;
        position.z -= deltaTime * lbl_806DBC94;
        if (position.z < 0.0f)
        {
            position.z = 0.0f;
        }
        SetPosition(position);
    }

    if (m_pCurrentAnimController->m_fTime > 0.95f)
    {
        InitActionMove(true);
    }
}

void Goalie::ActionSTSRecover(float deltaTime)
{
    mfWaitTime -= deltaTime;
    if (mfWaitTime <= 0.0f)
    {
        if (m_eAnimID != 0x7D)
        {
            PlayNewAnim(0x7D);
            InitMovementFromAnim(0, v3Zero, 1.0f, false);
        }
        else if (m_pCurrentAnimController->m_fTime > 0.95f)
        {
            InitActionMove(true);
        }
    }
}

void Goalie::fn_800891E8(float deltaTime)
{
    if (m_v3Position.z > 0.0f)
    {
        nlVector3 position = m_v3Position;
        position.z -= deltaTime * lbl_806DBC94;
        if (position.z < 0.0f)
        {
            position.z = 0.0f;
        }
        SetPosition(position);
    }

    cPN_SAnimController* pController
        = m_pCurrentAnimController;
    bool bShouldRecover = false;
    if (pController->m_ePlayMode == PM_HOLD
        && pController->m_fTime == 1.0f)
    {
        bShouldRecover = true;
    }

    if (bShouldRecover)
    {
        InitActionMove(true);
    }
}

void Goalie::ActionChipShotStumble(float deltaTime)
{
    bool bShouldRecover = false;
    if (m_pCurrentAnimController->m_ePlayMode == PM_HOLD
        && m_pCurrentAnimController->m_fTime == 1.0f)
    {
        bShouldRecover = true;
    }

    if (bShouldRecover)
    {
        if (m_eAnimID == 0x7D)
        {
            InitActionMove(true);
            return;
        }
        InitActionDiveRecover();
        return;
    }

    if (m_pCurrentAnimController->m_fTime
        < mpSaveData->mfMilestonePercent[2])
    {
        if (mUnidentified3E4 != g_pBall->mUnidentified008)
        {
            m_pPhysicsCharacter->m_CanCollideWithGoalLine = true;
            mbTryLobSave = false;
            InitActionSaveSetup(false);
            return;
        }
    }

    float x = mv3NavTarget.x;

    if ((float)fabs(x) > (0.5f + (float)fabs(m_v3Position.x))
        && m_pCurrentAnimController->m_fTime < 0.5f)
    {
        m_aDesiredFacingDirection
            = (u16)(s32)(10430.378f
                * nlATan2f(m_v3Position.y - mv3NavTarget.y,
                    m_v3Position.x - x));

        GoalieTweaks* pTweaks = (GoalieTweaks*)m_pTweaks;
        float fThrowingDirectionSeekSpeed
            = pTweaks->fThrowingDirectionSeekSpeed;
        float fThrowingDirectionSeekFalloff
            = pTweaks->fThrowingDirectionSeekFalloff;
        u16 newFacing = SeekDirection(
            m_aActualFacingDirection,
            m_aDesiredFacingDirection,
            fThrowingDirectionSeekSpeed,
            fThrowingDirectionSeekFalloff,
            deltaTime);
        SetFacingDirection(newFacing, true);
    }

    CheckForLimbEndZoneCollision();
}

void Goalie::fn_80088A94(float deltaTime)
{
    float animTime = m_pCurrentAnimController->m_fTime;
    float collisionDist = lbl_806DBD08;
    float collisionDistSq = collisionDist * collisionDist;

    if (IsOpponentInSTS())
    {
        if (animTime < mpLooseBallInfo->mfPickupTime)
        {
            u16 actualFacing = m_aActualFacingDirection;
            GetLocalPoint(mv3LocalContactPosition,
                mpShooter->m_v3Position, m_v3Position,
                actualFacing);

            if (animTime > 0.1f)
            {
                float t = (animTime - 0.1f)
                    / (mpLooseBallInfo->mfPickupTime - 0.1f);
                t = nlMaxEquals(t, 0.0f);
                t = nlMinEquals(t, 1.0f);
                float interp
                    = t * (t * ((-2.0f * t) + 3.0f));
                float angle = nlATan2f(
                    mv3LocalContactPosition.y,
                    mv3LocalContactPosition.x);
                s16 angleDeltaInt
                    = (s16)(u16)(s32)(10430.378f * angle);
                s32 multiplierInt = (s32)(1024.0f * interp);
                s32 adjustedDelta
                    = (multiplierInt * angleDeltaInt) / 1024;
                u16 aNewAng = actualFacing + adjustedDelta;

                SetFacingDirection(aNewAng, true);
                m_aDesiredFacingDirection = aNewAng;
            }

            if (animTime > 0.15f)
            {
                bool bWallBlock = mfWallBlock > 0.0f;
                if (!bWallBlock)
                {
                    float movementDuration;
                    float pickupWindow = mpLooseBallInfo->mfPickupTime;
                    movementDuration = mpLooseBallInfo->mfAnimDuration;
                    float stepScale = deltaTime
                        / (movementDuration
                            * (pickupWindow -= 0.15f));
                    nlVector3 movement = { 0.0f, 0.0f, 0.0f };

                    movement.x = mfTargetDist;
                    RotateVectorZAxis(
                        movement, movement, actualFacing);

                    float newZ = (stepScale * movement.z)
                        + m_v3Position.z;
                    float newY = (stepScale * movement.y)
                        + m_v3Position.y;
                    float newX = (stepScale * movement.x)
                        + m_v3Position.x;
                    movement.x = newX;
                    movement.y = newY;
                    movement.z = newZ;
                    SetPosition(movement);
                }
            }
        }

        if (animTime > 0.15f)
        {
            nlVector3 rightFootPos
                = GetJointPosition(m_nRightFootJointIndex);
            if (nlVec3DistanceSquared2D(rightFootPos,
                    mpShooter->m_v3Position)
                < collisionDistSq)
            {
                WhackSTSPlayer(mpShooter);
            }
        }
    }
    else
    {
        if (animTime <= 0.1f)
        {
            InitActionMove(true);
            return;
        }
    }

    if (animTime > 0.95f)
    {
        InitActionMove(true);
        return;
    }

    if ((mpShooter->m_eAnimID == 0x7C)
        && (animTime < 0.35f))
    {
        bool bWallBlock = mfWallBlock > 0.0f;
        if (!bWallBlock)
        {
            cFielder* pShooter = mpShooter;
            nlVector3& shooterPosition = pShooter->m_v3Position;
            nlVector3 rightFootPos
                = GetJointPosition(m_nRightFootJointIndex);
            float pushDist = lbl_806DBD08;
            float pushDistSq = pushDist * pushDist;
            float footDistSq = nlVec3DistanceSquared2D(
                rightFootPos, shooterPosition);

            if ((footDistSq < pushDistSq)
                || (footDistSq
                    > nlVec3DistanceSquared2D(
                        m_v3Position, shooterPosition)))
            {
                float radius;
                float shooterAbsX;
                mpShooter->m_pPhysicsCharacter->GetRadius(&radius);
                radius += 0.2f;
                shooterAbsX = (float)fabs(shooterPosition.x);

                nlVector3 pushVec;
                if (shooterAbsX
                    > (cField::GetGoalLineX(1U) - radius))
                {
                    pushVec.x = -lbl_806DBD08;
                    pushVec.y = 0.0f;
                    pushVec.z = 0.0f;
                    GetWorldPoint(pushVec, pushVec,
                        shooterPosition,
                        m_aActualFacingDirection);
                    pushVec.x += m_v3Position.x - rightFootPos.x;
                    pushVec.y += m_v3Position.y - rightFootPos.y;
                    SetPosition(pushVec);
                }
                else
                {
                    pushVec.x = lbl_806DBD08;
                    pushVec.y = 0.0f;
                    pushVec.z = 0.0f;
                    GetWorldPoint(pushVec, pushVec, rightFootPos,
                        m_aActualFacingDirection);
                    mpShooter->SetPosition(pushVec);
                }
            }
        }
    }
}

void Goalie::ActionSTSAttack(float deltaTime)
{
    if (fn_8007C73C())
    {
        CleanupStun();
        ChooseSwatAnim(1);
        SetGoalieAction(GOALIEACTION_UNIDENTIFIED_31, 0);
        m_fDesiredSpeed = 0.0f;
        m_fActualSpeed = 0.0f;
        SetVelocity(v3Zero);

        if (m_pBall != 0)
        {
            fn_80139D1C(1, GetGlobalPad());
            ReleaseBall(0);
        }

        fn_80098468(this, 0.2f);
        if (GetGlobalPad() != 0)
        {
            fn_8009591C(this, false);
        }
        mbGrabMonty = false;
        return;
    }

    if (fn_8007BC40())
    {
        return;
    }

    if (mnOffplayPending != GOALIE_OFFPLAY_NONE
        || fn_8007C904(this, mpShooter->m_v3Position))
    {
        InitActionMove(false);
        return;
    }

    if (IsOpponentInSTS() && CheckForSTSAttack())
    {
        return;
    }

    if (m_pCurrentAnimController->m_fTime > 0.95f)
    {
        InitActionMove(false);
        return;
    }

    if (!mbPickedUp)
    {
        bool bUnidentified = false;
        if ((g_pBall->m_unk_0xA4 == 5
                || g_pBall->m_unk_0xA4 == 3)
            && g_pBall->m_pPassTarget != 0)
        {
            bUnidentified = true;
        }

        if (bUnidentified && nlRandomf(100.0f) < lbl_806DBD20)
        {
            InitActionMove(false);
            return;
        }

        float fShotMeter = fn_800DEB04(mpShooter);
        if (!(fShotMeter > 0.0f && fShotMeter <= 1.0f)
            && g_pBall->m_tShotTimer.m_uPackedTime != 0)
        {
            InitActionMove(false);
            return;
        }

        float dx = mpShooter->m_v3Position.x - m_v3Position.x;
        float dy = mpShooter->m_v3Position.y - m_v3Position.y;
        m_aDesiredFacingDirection
            = (u16)(s32)(10430.378f * nlATan2f(dy, dx));

        if (!mpShooter->mbTangible)
        {
            if (m_pCurrentAnimController->m_fTime
                < mpLooseBallInfo->mfPickupTime - 0.1f)
            {
                InitActionMove(false);
                return;
            }

            mbPickedUp = true;
            return;
        }

        if (m_pCurrentAnimController->m_fTime
            >= mpLooseBallInfo->mfPickupTime)
        {
            nlVector2 v2Distance;
            v2Distance.x
                = m_v3Position.x - mpShooter->m_v3Position.x;
            v2Distance.y
                = m_v3Position.y - mpShooter->m_v3Position.y;
            float fDistanceSq = nlVec2LengthSquared(v2Distance);
            float fTargetDistSq = nlGetLengthSquared1D(mfTargetDist);
            if (fDistanceSq < fTargetDistSq)
            {
                fn_80080638(this, mpShooter, false);
                fn_80080BFC(this, 0.0f);

                PlayerAttackData data;
                data.pAttacker = this;
                data.nAttackerPadID = -1;
                data.pTarget = mpShooter;
                data.mUnidentified0C = 2;
                data.mUnidentified10 = false;
                fn_8005E604(lbl_806E0C94, &data);

                mbPickedUp = true;
            }
        }
        return;
    }

    if (mpLooseBallInfo->mAnimType == LOOSEBALL_ANIM_KICK
        && g_pBall->m_unk_0xA4 != 0
        && m_pCurrentAnimController->m_fTime > 0.4f
        && m_pCurrentAnimController->m_fTime < 0.45f)
    {
        InitActionMove(false);
    }
}

void Goalie::InitActionMove(bool bParam)
{
    if (m_pBall != 0)
    {
        InitActionMoveWB();
        return;
    }

    SetGoalieAction(GOALIEACTION_MOVE, 0);
    SetAnimState(5, true, 0.2f, false, false);
    InitMovementFromAnim(0, v3Zero, 1.0f, false);

    mnSubstate = 1;
    mMoveDirection = GOALIEDIR_IDLE;

    m_pPhysicsCharacter->m_CanCollideWithBall = true;
    mbShouldMiss = false;
    mbDoNavigate = false;
    m_pPhysicsCharacter->m_CanCollideWithGoalLine = true;
    m_pPhysicsCharacter->m_CanCollideWithWall = true;

    CleanupStun();

    mUrgency = URGENCY_LOW;
    mfSpeedScale = 1.0f;
    mbPosGoalieNetCheck = false;
    mbNegGoalieNetCheck = false;
    mbDoHeadTrack = true;
    mbBallImpacted = false;
    mbNoUserControl = false;
    mbPickedUp = false;

    if (bParam)
    {
        ActionMove(0.0f);
    }

    mbIsDown = false;
    if (g_pBall->m_tShotTimer.m_uPackedTime != 0)
    {
        if (!g_pBall->m_unk_0xA4 == true)
        {
            mpSkillShooter = 0;
        }
    }
    else
    {
        mpSkillShooter = 0;
    }
}

void Goalie::fn_8008EC2C()
{
    if (mGoalieActionState == GOALIEACTION_UNIDENTIFIED_31
        || mGoalieActionState == GOALIEACTION_UNIDENTIFIED_26)
    {
        return;
    }

    CleanupStun();
    ChooseSwatAnim(0);
    m_pPhysicsCharacter->m_CanCollideWithBall = false;
    SetGoalieAction(GOALIEACTION_UNIDENTIFIED_29, 0);
    mbIsDown = true;
    m_fDesiredSpeed = 0.0f;
    m_fActualSpeed = 0.0f;
    SetVelocity(v3Zero);
    PlayNewAnim(0xAC);
    InitMovementFromAnim(0, v3Zero, 1.0f, false);

    if (m_pBall != 0)
    {
        fn_80139D1C(1, GetGlobalPad());
        ReleaseBall(0);
    }

    if (GetGlobalPad() != 0)
    {
        fn_8009591C(this, false);
    }
}

void Goalie::fn_8008ED44(bool bParam)
{
    if (mGoalieActionState == GOALIEACTION_UNIDENTIFIED_31
        || mGoalieActionState == GOALIEACTION_UNIDENTIFIED_26)
    {
        return;
    }

    CleanupStun();
    ChooseSwatAnim(0);

    if (bParam)
    {
        SetGoalieAction(GOALIEACTION_UNIDENTIFIED_30, 0);
        PlayNewAnim(0xAB);
    }
    else
    {
        SetGoalieAction(GOALIEACTION_UNIDENTIFIED_25, 0);

        bool bMirrored = !m_pAnimInventory->GetMirrored(m_eAnimID);
        bool bUseSTSSpinMiss = false;
        bool bState8Shot = false;
        if (g_pBall->m_tShotTimer.m_uPackedTime != 0
            && g_pBall->m_unk_0xA4 == 8)
        {
            bState8Shot = true;
        }

        if (bState8Shot
            || (mpShooter != 0
                && (fn_8003E7F8(mpShooter)
                    || fn_8003E84C(mpShooter))))
        {
            mpSaveData = GoalieSave::GetSTSSpinMissData(bMirrored);
            bUseSTSSpinMiss = true;
        }
        else
        {
            mpSaveData = fn_80093848(bMirrored);
        }

        PlayNewAnim(mpSaveData->mnAnimID);
        if (bUseSTSSpinMiss)
        {
            fn_8007EB5C(this);
        }
    }

    mbIsDown = true;
    m_fDesiredSpeed = 0.0f;
    m_fActualSpeed = 0.0f;
    SetVelocity(v3Zero);
    m_pPhysicsCharacter->m_CanCollideWithGoalLine = false;
    m_pPhysicsCharacter->m_CanCollideWithBall = false;
    InitMovementFromAnim(0, v3Zero, 1.0f, false);

    if (m_pBall != 0)
    {
        fn_80139D1C(1, GetGlobalPad());
        ReleaseBall(0);
    }

    if (GetGlobalPad() != 0)
    {
        fn_8009591C(this, false);
    }

    fn_80098468(this, 0.2f);
}

void Goalie::fn_8008EF58()
{
    mbMegaUserSave = false;
    mUnidentified40C = -1;

    if (fn_80338BF0(lbl_806E20D8) > 1)
    {
        mbMegaUserSave = true;
        return;
    }

    for (int i = 0; i < 5; i++)
    {
        cPlayer* pPlayer = fn_800A6A84(m_pTeam, i);
        if (pPlayer->m_pController != 0)
        {
            mbMegaUserSave = true;
            return;
        }
    }
}

void Goalie::InitActionMoveWB()
{
    if (m_pBall == 0)
    {
        PickupBall(g_pBall);
    }

    SetGoalieAction(GOALIEACTION_MOVE_WB, 0);

    if (m_pCurrentAnimController->m_bMirror)
    {
        SetAnimState(0x10, true, 0.2f, false, false);
    }
    else
    {
        SetAnimState(6, true, 0.2f, false, false);
    }

    InitMovementFromAnim(0, v3Zero, 1.0f, false);

    GoalieTweaks* pTweaks = (GoalieTweaks*)m_pTweaks;
    mfWaitTime = pTweaks->fGoalieBallTime;
    mfTargetTime = 0.0f;
    mpPassTarget = 0;
    mbIsDown = false;
    mbDoNavigate = true;
    m_pPhysicsCharacter->m_CanCollideWithGoalLine = true;
    m_pPhysicsCharacter->m_CanCollideWithWall = true;
}

void Goalie::InitActionSaveSetup(bool bCanReposition)
{
    if (mGoalieActionState == GOALIEACTION_STS_RECOVER
        || mGoalieActionState == GOALIEACTION_UNIDENTIFIED_27
        || mGoalieActionState == GOALIEACTION_UNIDENTIFIED_28
        || mGoalieActionState == GOALIEACTION_UNIDENTIFIED_29
        || mGoalieActionState == GOALIEACTION_UNIDENTIFIED_30
        || mGoalieActionState == GOALIEACTION_UNIDENTIFIED_32
        || mGoalieActionState == GOALIEACTION_UNIDENTIFIED_37
        || mGoalieActionState == GOALIEACTION_UNIDENTIFIED_25)
    {
        cPlayer* pSkillShooter = g_pBall->m_pShooter;
        if (pSkillShooter != 0 && g_pBall->m_unk_0xA4 == 8)
        {
            mpSkillShooter = pSkillShooter;
            switch ((int)pSkillShooter->m_eCharacterClass)
            {
            case 12:
            case 14:
            case 15:
            case 17:
            case 19:
                mbNoUserControl = true;
                break;
            }
        }
        else
        {
            mpSkillShooter = 0;
        }
        return;
    }

    if ((mGoalieActionState == GOALIEACTION_PURSUE_BALL_POUNCE
            || mGoalieActionState == GOALIEACTION_LOOSEBALL_PICKUP)
        && m_pCurrentAnimController->m_fTime
            > 0.6f * mpLooseBallInfo->mfPickupTime)
    {
        return;
    }

    if (mGoalieActionState == GOALIEACTION_SAVE)
    {
        return;
    }

    if (fn_8007BF68(this, false))
    {
        return;
    }

    mUnidentified3E4 = g_pBall->mUnidentified008;
    muBallDeflectCount = g_pBall->m_bBallDeflectCount;
    mbDoHeadTrack = true;
    mbBallImpacted = false;
    mbIsDown = false;

    cPlayer* pSkillShooter = g_pBall->m_pShooter;
    if (pSkillShooter != 0 && g_pBall->m_unk_0xA4 == 8)
    {
        mpSkillShooter = pSkillShooter;
        switch ((int)pSkillShooter->m_eCharacterClass)
        {
        case 12:
        case 14:
        case 15:
        case 17:
        case 19:
            mbNoUserControl = true;
            break;
        }
    }
    else
    {
        mpSkillShooter = 0;
    }

    mnOffplayPending = GOALIE_OFFPLAY_NONE;
    SetGoalieAction(GOALIEACTION_SAVE_SETUP, 0);
    m_pPhysicsCharacter->m_CanCollideWithBall = true;

    float fTimeToContact = fn_8007EDF4(this, fn_800776B4());
    float fEnergyLevel = mFatigue.mfEnergyLevel;
    fEnergyLevel -= lbl_806DBCB4 * fn_800156A8(g_pBall);
    if (fEnergyLevel < lbl_806DBCB8)
    {
        fn_800EBBFC(9, 0x90A88490, 0, 0);
    }

    float fUnidentifiedRange = InterpolateRangeClamped(
        lbl_806DBCA4, lbl_806DBCA8,
        lbl_806DBCAC, lbl_806DBCB0, fEnergyLevel);
    float fTargetVelocitySq
        = nlVec3LengthSquared(mv3TargetVelocity);
    unsigned int uSaveType = 0xFFFC;

    if (mUnidentified254 == 0)
    {
        bool bState7Shot = g_pBall->m_tShotTimer.m_uPackedTime != 0
                        && g_pBall->m_unk_0xA4 == 7;
        if (bState7Shot)
        {
            uSaveType = 3;
        }
        else
        {
            bool bState8Shot
                = g_pBall->m_tShotTimer.m_uPackedTime != 0
               && g_pBall->m_unk_0xA4 == 8;
            if (!bState8Shot
                && fTargetVelocitySq
                    < fUnidentifiedRange * fUnidentifiedRange)
            {
                uSaveType = 0xFFFF;
            }
        }
    }
    muSaveType = uSaveType;

    if (fTimeToContact > 0.0f)
    {
        bool bFromTakeoff = false;
        if (mPrevGoalieActionState == GOALIEACTION_PRE_CROUCH
            || mPrevGoalieActionState == GOALIEACTION_PURSUE_BALL_CARRIER)
        {
            bFromTakeoff = true;
        }

        if (mbShouldMiss)
        {
            float absX = (float)fabs(g_pBall->m_v3Position.x);
            if (absX
                < cField::GetGoalLineX(1U) - lbl_806DBCFC)
            {
                nlVector2 v2Distance;
                v2Distance.y = m_v3Position.y - mv3TargetPosition.y;
                v2Distance.x = m_v3Position.x - mv3TargetPosition.x;
                if (nlVec2LengthSquared(v2Distance)
                    > lbl_806DBD00 * lbl_806DBD00)
                {
                    cBall* pBall = g_pBall;
                    bool bState7Shot
                        = pBall->m_tShotTimer.m_uPackedTime != 0
                       && pBall->m_unk_0xA4 == 7;
                    if (bState7Shot)
                    {
                        nlVector2 v2ShotDistance;
                        v2ShotDistance.y = m_v3Position.y
                                           - pBall->m_v3ShotTarget.y;
                        v2ShotDistance.x = m_v3Position.x
                                           - pBall->m_v3ShotTarget.x;
                        if (nlVec2LengthSquared(v2ShotDistance) > 6.25f)
                        {
                            static FilteredRandomChance randgenStumble;
                            GoalieTweaks* pGoalieTweaks
                                = (GoalieTweaks*)m_pTweaks;
                            if (randgenStumble.genrand(
                                    pGoalieTweaks
                                        ->fLobShotStumbleChance))
                            {
                                InitActionChipShotStumble(fTimeToContact);
                                return;
                            }
                        }
                    }
                }
            }
        }

        if (mUrgency == URGENCY_HIGH)
        {
            bFromTakeoff = true;
        }

        mfTimeTilSave = fn_8007ECB4(
            this, fTimeToContact, muSaveType, bFromTakeoff, false);
        if (mfTimeTilSave < 0.0f)
        {
            mfTimeTilSave = fn_8007ECB4(this, fTimeToContact,
                muSaveType | 0xFFFC, true, true);
        }

        float fMilestone2 = mBlendInfo.mfMilestoneTime[2];
        if (mbShouldMiss)
        {
            if (bFromTakeoff)
            {
                mBlendInfo.mfStartTime
                    = mBlendInfo.mfMilestoneTime[0];
            }
            else
            {
                mBlendInfo.mfStartTime = 0.0f;
            }
        }
        else if (bFromTakeoff)
        {
            if (fMilestone2 - mBlendInfo.mfMilestoneTime[0]
                <= mfTimeTilSave)
            {
                mBlendInfo.mfStartTime
                    = mBlendInfo.mfMilestoneTime[0];
            }
            else
            {
                float diff = fMilestone2 - mfTimeTilSave;
                float cap = mBlendInfo.mfMilestoneTime[1];
                float startTime;
                if (diff <= cap)
                {
                    startTime = diff;
                }
                else
                {
                    startTime = cap;
                }
                mBlendInfo.mfStartTime = startTime;
            }
        }
        else
        {
            if (fMilestone2 <= mfTimeTilSave)
            {
                mBlendInfo.mfStartTime = 0.0f;
            }
            else
            {
                float diff = fMilestone2 - mfTimeTilSave;
                float cap = mBlendInfo.mfMilestoneTime[1];
                float startTime;
                if (diff <= cap)
                {
                    startTime = diff;
                }
                else
                {
                    startTime = cap;
                }
                mBlendInfo.mfStartTime = startTime;
            }
        }

        mfWaitTime = mBlendInfo.mfStartTime
                   + (mfTimeTilSave - fMilestone2);
        if (mfWaitTime < lbl_806DBD04)
        {
            InitActionSave();
            return;
        }

        if (lbl_806DBC90 && bCanReposition && ShouldReposition())
        {
            mMoveDirection = GOALIEDIR_IDLE;
            SetGoalieAction(GOALIEACTION_SAVE_REPOSITION, 0);

            nlVector2 v2Distance;
            v2Distance.y = m_v3Position.y - mv3NavTarget.y;
            v2Distance.x = m_v3Position.x - mv3NavTarget.x;
            mfTargetDist = nlVec2LengthSquared(v2Distance);

            cBall* pBall = g_pBall;
            m_aDesiredFacingDirection = (u16)(s32)(10430.378f
                * nlATan2f(pBall->m_v3Position.y - m_v3Position.y,
                    pBall->m_v3Position.x - m_v3Position.x));
            fn_800797DC(this, 0, 0.0f, gfRepositionThreshold);
            return;
        }

        SetAnimState(7, true, 0.2f, false, false);
        GoalieTweaks* pGoalieTweaks = (GoalieTweaks*)m_pTweaks;
        InitMovementFromAnimSeek(
            pGoalieTweaks->fSaveDirectionSeekSpeed,
            pGoalieTweaks->fSaveDirectionSeekFalloff);
        return;
    }

    InitActionMove(false);
}

void Goalie::InitActionSave()
{
    float absX = (float)fabs(g_pBall->m_v3ShotTarget.x);
    if (absX > cField::GetGoalLineX(1U) - 0.2f
        && !IsInsideNetArea(g_pBall->m_v3ShotTarget))
    {
        InitActionMove(false);
        return;
    }

    SetGoalieAction(GOALIEACTION_SAVE, 0);
    mFatigue.RegisterShot(mpSaveData->mfFatigueValue);
    mbBallImpacted = false;
    mbIsDown = true;

    if (mbShouldMiss)
    {
        if (mpSaveData->mpFailAnimData != 0)
        {
            mpSaveData = mpSaveData->mpFailAnimData;
            SetAnimState(mpSaveData->mnAnimID,
                true, 0.2f, false, false);
            InitMovementFromAnim(0, v3Zero, 0.0f, false);

            if (mUnidentified254 != 0)
            {
                fn_80097648(0.1f);
            }
        }
        else
        {
            PlayBlendedAnims(0.0f, 1.5f, -1);
        }
    }
    else
    {
        PlayBlendedAnims(mBlendInfo.mfStartTime, 1.5f, -1);
    }

    MakeExertEvent();
}

void Goalie::fn_80090320(float fParam)
{
    mbDoHeadTrack = false;
    SetGoalieAction(GOALIEACTION_UNIDENTIFIED_32, 0);

    if (m_pBall != 0)
    {
        fn_8007EB90(this);
    }

    SetAnimState(0xAE, true, 0.2f, false, false);
    InitMovementFromAnim(0, v3Zero, 1.0f, false);

    if (fParam > 0.0f)
    {
        m_pCurrentAnimController->SetTime(fParam);
        fn_801BAF0C(this);
        fn_801BABEC(this);
    }

    mbIsDown = true;
}

void Goalie::InitActionChipShotStumble(float fTargetTime)
{
    SetGoalieAction(GOALIEACTION_MISS_CHIP_SHOT, 0);

    mUnidentified3E4 = g_pBall->mUnidentified008;

    nlVector2 v2Delta;
    v2Delta.x = m_v3Position.x - g_pBall->m_v3Position.x;
    v2Delta.y = m_v3Position.y - g_pBall->m_v3Position.y;
    bool bFar = nlGetLengthSquared2D(v2Delta.x, v2Delta.y) > 42.25f
        || fTargetTime > lbl_806DBCF8;
    bool bContactLow;
    if (mv3LocalContactPosition.y > 0.0f)
        bContactLow = false;
    else
        bContactLow = true;
    mpSaveData = GoalieSave::GetMissChipSaveData(bContactLow, bFar);

    mpLooseBallInfo = 0;
    SetAnimState(mpSaveData->mnAnimID, true, 0.2f, false, false);
    InitMovementFromAnim(0, v3Zero, 1.0f, false);
    m_pPhysicsCharacter->m_CanCollideWithGoalLine = false;

    cBall* pBall = g_pBall;
    nlVector3 v3Ball2Targ;
    nlVector3* const pV = &v3Ball2Targ;
    float shotX;
    float shotY;
    shotY = pBall->m_v3ShotTarget.y;
    shotX = pBall->m_v3ShotTarget.x;
    pV->x = shotX - pBall->m_v3Position.x;
    pV->y = shotY - pBall->m_v3Position.y;
    pV->z = pBall->m_v3ShotTarget.z - pBall->m_v3Position.z;
    float dist = nlSqrt(nlGetLengthSquared2D(pV->x, pV->y), true);

    if (dist > 0.5f)
    {
        float scale = (1.5f + dist) / dist;
        nlVec3ScaleAdd(
            mv3NavTarget, scale, *pV, pBall->m_v3ShotTarget);
    }
    else
    {
        mv3NavTarget = pBall->m_v3ShotTarget;
        float pushX;
        if (mv3NavTarget.x > 0.0f)
        {
            pushX = 1.5f;
        }
        else
        {
            pushX = -1.5f;
        }
        mv3NavTarget.x += pushX;
    }

    float maxY;
    float clampedY;
    float netWidth = cNet::m_fNetWidth;
    maxY = 0.5f * netWidth - 1.0f;
    clampedY = nlMaxEquals(mv3NavTarget.y, -maxY);
    clampedY = nlMinEquals(clampedY, maxY);
    mv3NavTarget.y = clampedY;

    mv3NavTarget.z = 0.0f;
    mbDoHeadTrack = false;
    mbIsDown = true;
}

void Goalie::InitActionDiveRecover()
{
    if (mbBallImpacted && mpSkillShooter != 0)
    {
        if (fn_80090958(true))
        {
            return;
        }
    }
    else if (mUnidentified254 != 0)
    {
        fn_80097358(this, -1.0f);
        if (mnOffplayPending == GOALIE_OFFPLAY_NONE)
        {
            fn_8007EA90(this);
        }
        if (m_pBall != 0)
        {
            fn_8007EB90(this);
        }
        SetNoPickUpTime(0.4f);
        mbDoHeadTrack = false;
    }

    mpSkillShooter = 0;

    if (mpSaveData != 0 && mpSaveData->mnRecoverAnimID >= 0)
    {
        mbDoHeadTrack = false;

        if (mnOffplayPending != GOALIE_OFFPLAY_NONE)
        {
            if (m_pBall != 0)
            {
                ReleaseBall(false);
            }

            int randomValue = nlRandom(2);
            SetGoalieAction(GOALIEACTION_OFFPLAY, 0);

            int animID;
            if (m_pAnimInventory->GetMirrored(m_eAnimID))
            {
                animID = randomValue == 0 ? 0x92 : 0x94;
            }
            else
            {
                animID = randomValue == 0 ? 0x91 : 0x93;
            }

            SetAnimState(animID, true, 0.2f, false, false);
            mnOffplayPending = GOALIE_OFFPLAY_NONE;
            mbIsDown = true;
        }
        else
        {
            SetGoalieAction(GOALIEACTION_DIVE_RECOVER, 0);
            SetAnimState(mpSaveData->mnRecoverAnimID,
                true, 0.2f, false, false);
            mbIsDown = true;
        }

        InitMovementFromAnim(0, v3Zero, 1.0f, false);
    }
    else
    {
        if (m_pBall == 0)
        {
            InitActionMove(true);
        }
        else
        {
            InitActionMoveWB();
        }
    }

    mbPickedUp = false;
}
