#include "NL/plat/PlatPadManager.h"

#include "Game/Sys/audio.h"
#include "Game/Goalie.h"
#include "NL/nlString.h"

#include "Game/AI/AiUtil.h"
#include "Game/AI/Fielder.h"
#include "Game/AI/FielderActions.h"
#include "Game/AI/FuzzyVariant.h"
#include "Game/AI/GoalieLooseBall.h"
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
#include "Game/Effects/EmissionManager.h"
#include "Game/Field.h"
#include "Game/Game.h"
#include "Game/GameInfo.h"
#include "Game/MathHelpers.h"
#include "Game/Net.h"
#include "Game/Physics/PhysicsCharacter.h"
#include "Game/PoseAccumulator.h"
#include "Game/Render/ShootToScoreArrow.h"
#include "Game/RumbleActions.h"
#include "Game/SAnim/pnBlender.h"
#include "Game/SAnim/pnSAnimController.h"
#include "Game/SAnim/pnSingleAxisBlender.h"
#include "Game/SHierarchy.h"
#include "Game/Task/GameRenderTask.h"
#include "Game/Team.h"
#include "NL/globalpad.h"
#include "NL/nlMain.h"
#include "NL/nlPrint.h"
#include "unclassified/tu_800A9B78.h"
#include "unclassified/tu_801A6AAC.h"
#include "Game/NetworkPeer.h"

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
extern "C" void fn_80278860(BasicStadium* pStadium, int nParam);
extern "C" void fn_8007F534(Goalie* pGoalie);
class UnidentifiedFuzzyRuntimeBase;
extern "C" cPlayer* fn_80096F54(cPlayer*, bool);
extern "C" UnidentifiedFuzzyRuntimeBase* fn_800A695C(cTeam*);
extern "C" UnidentifiedVariant_80054AB8 fn_80082140(
    UnidentifiedFuzzyRuntimeBase*, const char*, cPlayer*);
extern float lbl_806DBB74;
extern float lbl_806DBB78;
extern float lbl_806DC7C8;

static const nlVector3 v3Zero = { 0.0f, 0.0f, 0.0f };

bool Goalie::mbPosGoalieNetCheck;
bool Goalie::mbNegGoalieNetCheck;
float lbl_806DBB1C = 0.5f;
float lbl_806DBB2C = 9.5f;

extern "C" UnidentifiedVariant_80054AB8 fn_80082150(
    UnidentifiedFuzzyRuntimeBase*, cPlayer*, const char*);

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

        float fConfidence;
        if (vBestPassTarget.ExtraData.IsSet(4))
        {
            fConfidence = vBestPassTarget.ExtraData.Get(4)->mData.f;
        }
        else
        {
            fConfidence = 0.0f;
        }

        if (fConfidence >= 0.5f)
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
    f32 fMargin = ((GoalieTweaks*)m_pTweaks)->fSaveIgnoreMargin;
    f32 fNetWidth;
    double fAbsTargetY;

    if ((float)fabsf(v3Target.x) > (cField::GetGoalLineX(1U) - 1.0f)
        && ((fNetWidth = cNet::m_fNetWidth),
            (fAbsTargetY = __fabs(v3Target.y)),
            (float)fAbsTargetY < (0.5f * fNetWidth + fMargin))
        && v3Target.z < (fMargin + cNet::m_fNetHeight))
    {
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

    if (m_tFireTimer.m_uPackedTime == 0)
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

    while (milestoneIndex < 4
           && fTime >= pSaveData->mfMilestonePercent[milestoneIndex])
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

    if (m_tFireTimer.m_uPackedTime != 0)
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
        m_bSkipAnimUpdate = false;
        m_fSkipTimer = 0.0f;
        m_bForceFeatherUpdate = false;
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
    m_bSkipAnimUpdate = false;
    m_fSkipTimer = 0.0f;
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
    g_pGame->mUnidentified10DC->fn_800AA5F8();
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
    if (fn_800976C4() && m_nFeatherAnimID == 0xAA)
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
    if (fn_800976C4() && m_nFeatherAnimID == 0xAA)
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

extern "C" void fn_8007EB90(Goalie* pGoalie)
{
    pGoalie->ReleaseBall(0);

    cBall* pBall = g_pBall;
    nlVector3 v3BallVelocity;
    float fYVelocity;
    if (pBall->m_v3Position.y > 0.0f)
    {
        fYVelocity = 1.0f;
    }
    else
    {
        fYVelocity = -1.0f;
    }
    v3BallVelocity.y = fYVelocity;

    float fXVelocity = nlRandomf(0.5f);
    fXVelocity = pBall->m_v3Position.x < 0.0f
                   ? fXVelocity
                   : -fXVelocity;
    v3BallVelocity.x = fXVelocity;

    float fVelocityScale = 6.0f + nlRandomf(2.0f);
    nlVec3Scale(v3BallVelocity, fVelocityScale);
    v3BallVelocity.z = 8.0f + nlRandomf(2.0f);

    g_pBall->m_tNoPickupTimer.SetSeconds(0.7f);
    g_pBall->SetVelocity(v3BallVelocity, SPINTYPE_NONE, 0);

    bool bHumanControlled = pGoalie->GetGlobalPad() != 0;
    if (bHumanControlled)
    {
        PlayRumbleAction(1, pGoalie->GetGlobalPad());
        pGoalie->SwapController(false);
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
        nlVec3WeightedSum(
            v3G2Ball, 0.5f, v3G2Ball, 0.5f, v3BallVelocity);
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
        float fLengthSq = nlVec3LengthSquared(v3Facing);

        float fRecip = nlRecipSqrt(fLengthSq, true);
        nlVec3Scale(v3Facing, fRecip);

        float fRecip2 = nlRecipSqrt(
            nlVec3LengthSquared(v3BallToGoal), true);
        nlVec3Scale(v3BallToGoal, fRecip2);

        nlVec3WeightedSum(
            v3Facing, 0.5f, v3Facing, 0.5f, v3BallToGoal);
    }

    mUnidentified024.m_aDesiredFacingDirection = (s16)(nlATan2f(
                                          v3Facing.y, v3Facing.x)
                                      * (32768.0f / 3.14159265f));
}

void Goalie::TrackTarget(
    const nlVector3& v3Target, float fRatio, float fParam3)
{
    nlVector3 v3FutureBallPos;
    nlVector3 v3FuturePos;
    unsigned short aRot;

    GetCurrentAnimFuture(m_nBallJointIndex, mpLooseBallInfo->mfPickupTime, v3FutureBallPos, v3FuturePos, aRot);

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
