#include "Game/AI/Scripts/ScriptQuestions.h"
#include "Game/AI/Desire.h"
#include "Game/AI/DesireReceivePass.h"
#include "Game/AI/Scripts/ScriptCaching.h"
#include "Game/FormationDefines.h"
#include "Game/AI/AiUtil.h"
#include "Game/AI/AvoidController.h"
#include "Game/AI/AIPad.h"
#include "Game/AI/AvoidableObject.h"
#include "Game/AI/ShotMeter.h"
#include "Game/AI/SkillTweaks.h"
#include "Game/AI/TeamPlayMachine.h"
#include "Game/AI/SpaceSearch.h"
#include "Game/AI/Fuzzy.h"
#include "Game/Game.h"
#include "Game/GameInfo.h"
#include "Game/GameTweaks.h"
#include "Game/Goalie.h"
#include "Game/CharacterTweaks.h"
#include "Game/CharacterTemplate.h"
#include "Game/Field.h"
#include "Game/MathHelpers.h"
#include "Game/Net.h"
#include "Game/ScriptTuning.h"
#include "Game/Physics/PhysicsCharacter.h"
#include "Game/SAnim/pnSAnimController.h"
#include "Game/Sys/debug.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "types.h"
#include "NL/nlPrint.h"
extern cTeam* g_pCurrentlyUpdatingTeam;
extern cBall* g_pScriptBall;
extern cBall* g_pBall;
extern cTeam* g_pScriptOtherTeam;
extern cTeam* g_pScriptCurrentTeam;
extern cFielder* g_pScriptBallOwner;
extern "C" float fn_8002E1B0(cFielder* pFielder);
extern "C" float fn_800A0508(cFielder* pFielder, bool bParam1, bool bParam2);
extern "C" AvoidController* fn_8002E144(cFielder* pFielder);
extern "C" float fn_8000F558(AvoidController* pController, eAvoidableThings eThings);
extern "C" bool fn_800381B4(cFielder* pFielder, nlVector3* pOutPos);
extern "C" float fn_8002CE14(PlayerTweaks* pTweaks);
extern "C" float fn_8002C328(PlayerTweaks* pTweaks);
extern "C" float fn_8002BFB8(PlayerTweaks* pTweaks);
extern "C" float fn_8002C254(const PlayerTweaks* pTweaks);
extern "C" float fn_8002BE18(PlayerTweaks* pTweaks);
extern "C" float fn_8002BE38(PlayerTweaks* pTweaks);
extern "C" float fn_8002BE64(PlayerTweaks* pTweaks);
extern "C" float fn_8002BE84(const PlayerTweaks* pTweaks);
extern "C" float fn_800DB298(const nlVector3&, const nlVector3&, cFielder*,
    float, float, float, float, cPlayer*);
extern "C" float fn_800DAFCC(const nlVector3&, const nlVector3&, cPlayer*,
    cPlayer*, float, float, float, float);

static float CloseToGoaliePosition(const nlVector3& v3FromPos, const nlVector3& v3GoaliePos);
static float FarToGoaliePosition(const nlVector3& v3FromPos, const nlVector3& v3GoaliePos);
static float InBetween(const nlVector3& v3InBetweenPos, const nlVector3& v3A, const nlVector3& v3B);
extern "C" float fn_800A0508(cFielder* pFielder, bool bIsChipShot, bool bWasPerfectPass);
extern "C" float fn_8004028C(cFielder*);
extern "C" bool fn_8002F858(cFielder*, bool);
extern "C" float fn_8002BE38(PlayerTweaks*);
extern "C" float fn_8002BE18(PlayerTweaks*);
extern "C" float fn_8002BE84(const PlayerTweaks*);
extern "C" float fn_8002BE64(PlayerTweaks*);
extern "C" Desire* fn_8002E08C(cFielder* pFielder, int nAction);
extern "C" float fn_800156A8(cBall*);
extern float (*lbl_806DF564)();
extern "C" const nlVector3& fn_80040234(cFielder*);
nlVector2 lbl_806DC3D8 = { 10.0f, 10.0f };
nlVector2 lbl_806DC3E0 = { 21845.0f, 0.0f };
static TweakFloatBinding lbl_8056DB30("Ideal Distance", "Game/Player", &lbl_806DC3D8.x);
static TweakFloatBinding lbl_8056DB50("Ideal Range", gLastTweakCategory, &lbl_806DC3D8.y);
static TweakFloatBinding lbl_8056DB70("Min Angle", gLastTweakCategory, &lbl_806DC3E0.x);
static TweakFloatBinding lbl_8056DB90("Max Angle", gLastTweakCategory, &lbl_806DC3E0.y);
extern "C" bool fn_80099CE8(int);
extern "C" bool fn_8002EDC8(cFielder*, int);
extern "C" float fn_8002C800(PlayerTweaks*);
extern "C" float fn_8003C40C(cFielder*, unsigned short);
float lbl_806DC3E8 = 100000000000.0f;
float lbl_806DC3EC = -100000000000.0f;
extern "C" float fn_80030750(cFielder*);

extern "C" float fn_800DA7A8(cFielder* pFielder, nlVector3* pPosition);
float CloseToFormationPosition(cFielder* pFielder);
float FarToFormationPosition(cFielder* pFielder);
extern "C" float fn_800DE40C(cPlayer* pUpfieldPlayer, cPlayer* pFromPlayer);
extern "C" float fn_800DDF54(cPlayer* pCandidateFielder, cPlayer* pTargetFielder);
extern "C" float fn_800DE0A8(cPlayer* pPlayer);
extern "C" float fn_800DED80(cPlayer* pPlayer);

float GenerateFilteredRandom()
{
    return nlRandomf(1.0f);
}

float RandomChance(float fChance)
{
    return FGREATER(fChance, GenerateFilteredRandom());
}

extern "C" cTeam* fn_800D6670(cFielder* pFielder)
{
    if (pFielder != NULL)
    {
        return pFielder->GetTeam();
    }
    return NULL;
}

extern "C" cTeam* fn_800D6688(cFielder* pFielder)
{
    if (pFielder != NULL)
    {
        return pFielder->GetTeam()->GetOtherTeam();
    }
    return NULL;
}

extern "C" Goalie* fn_800D66A0(cFielder* pFielder)
{
    if (pFielder != NULL)
    {
        return fn_800D6670(pFielder)->GetGoalie();
    }
    return NULL;
}

extern "C" Goalie* fn_800D66C4(cFielder* pFielder)
{
    if (pFielder != NULL)
    {
        return fn_800D6688(pFielder)->GetGoalie();
    }
    return NULL;
}

extern "C" cFielder* fn_800D6708(cTeam* team)
{
    if (team != NULL)
    {
        return team->GetCaptain();
    }
    return NULL;
}

extern "C" cFielder* fn_800D671C(cTeam* team)
{
    if (team != NULL)
    {
        return team->mpBestBallInterceptor;
    }
    return NULL;
}

extern "C" cFielder* fn_800D6734(cFielder* pFielder)
{
    return NULL;
}

extern "C" void* fn_800D673C(void*)
{
    return NULL;
}

extern "C" cPlayer* fn_800D6744(cBall* ball)
{
    return ball->m_pPassTarget;
}

extern "C" cFielder* fn_800D674C(cPlayer* player)
{
    return player->GetClosestOpponentFielder(NULL, true);
}

float BallOwner(cPlayer* player)
{
    if (player == NULL)
    {
        return 0.0f;
    }

    if (player->m_pBall != NULL)
    {
        return 1.0f;
    }

    return 0.0f;
}

float BallOwnerT(cTeam* team)
{
    if (team == NULL)
    {
        return 0.0f;
    }

    u8 isOwnerOnTeam = 0;
    cPlayer* pOwner = g_pBall->m_pOwner;
    if (pOwner != NULL && pOwner->m_pTeam == team)
    {
        isOwnerOnTeam = 1;
    }

    return isOwnerOnTeam ? 1.0f : 0.0f;
}

float LastBallOwner(cPlayer* player)
{
    if (player == NULL)
    {
        return 0.0f;
    }

    if (g_pBall->m_pPrevOwner == player)
    {
        return 1.0f;
    }

    return 0.0f;
}

float Striker(cFielder* fielder)
{
    if (fielder == NULL)
    {
        return 0.0f;
    }

    bool bUnidentified = false;
    if (fielder->m_eClassType == FIELDER && fielder->IsStriker())
    {
        bUnidentified = true;
    }

    if (bUnidentified)
    {
        return 1.0f;
    }

    return 0.0f;
}

float Winger(cFielder* fielder)
{
    if (fielder == NULL)
    {
        return 0.0f;
    }

    bool bUnidentified = false;
    if (fielder->m_eClassType == FIELDER && fielder->IsWinger())
    {
        bUnidentified = true;
    }

    if (bUnidentified)
    {
        return 1.0f;
    }

    return 0.0f;
}

float Midfield(cFielder* fielder)
{
    if (fielder == NULL)
    {
        return 0.0f;
    }

    bool bUnidentified = false;
    if (fielder->m_eClassType == FIELDER && fielder->IsMidField())
    {
        bUnidentified = true;
    }

    if (bUnidentified)
    {
        return 1.0f;
    }

    return 0.0f;
}

float Defence(cFielder* fielder)
{
    if (fielder == NULL)
    {
        return 0.0f;
    }

    bool bUnidentified = false;
    if (fielder->m_eClassType == FIELDER && fielder->IsDefense())
    {
        bUnidentified = true;
    }

    if (bUnidentified)
    {
        return 1.0f;
    }

    return 0.0f;
}

float Captain(cFielder* fielder)
{
    if (fielder == NULL)
    {
        return 0.0f;
    }

    if (fielder->fn_8001E168())
    {
        return 1.0f;
    }

    return 0.0f;
}

float GoalieType(cPlayer* player)
{
    if (player == NULL)
    {
        return 0.0f;
    }

    if (player->m_eClassType == GOALIE)
    {
        return 1.0f;
    }

    return 0.0f;
}

float FielderType(cPlayer* player)
{
    if (player == NULL)
    {
        return 0.0f;
    }

    if (player->m_eClassType == FIELDER)
    {
        return 1.0f;
    }

    return 0.0f;
}

float Marking(cFielder* pMarking, cPlayer* pMarked)
{
    if (pMarking == NULL)
    {
        return 0.0f;
    }

    if (pMarked == NULL)
    {
        return 0.0f;
    }

    if (pMarking->fn_800306F4((cFielder*)pMarked))
    {
        return 1.0f;
    }

    return 0.0f;
}

extern "C" float fn_800D6A90(cFielder* pFielder)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }

    float fSpeed = pFielder->mUnidentified024.m_fActualSpeed;
    float fAttribute = fn_8002CE14(pFielder->GetTweaks());
    return NormalizeVal(fSpeed, 0.7f * fAttribute, 2.0f);
}

extern "C" float fn_800D6AF0(cFielder* pFielder)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }

    float fSpeed = pFielder->mUnidentified024.m_fActualSpeed;
    float fBaseSpeed = fn_8002CE14(pFielder->GetTweaks());
    float fAttribute;
    if (pFielder->m_pBall != NULL)
    {
        fAttribute = fn_8002C328(pFielder->GetTweaks());
    }
    else
    {
        fAttribute = fn_8002BFB8(pFielder->GetTweaks());
    }

    float fInterpolated = Interpolate(fBaseSpeed, fAttribute, 0.3f);
    if (fSpeed < fInterpolated)
    {
        return NormalizeVal(fSpeed, fInterpolated - (fAttribute - fBaseSpeed),
            fInterpolated);
    }

    return NormalizeVal(fSpeed, fInterpolated + (fAttribute - fBaseSpeed),
        fInterpolated);
}

extern "C" float fn_800D6BD8(cFielder* pFielder)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }

    float fSpeed = pFielder->mUnidentified024.m_fActualSpeed;
    float fBaseSpeed = fn_8002CE14(pFielder->GetTweaks());
    float fAttribute;
    if (pFielder->m_pBall != NULL)
    {
        fAttribute = fn_8002C328(pFielder->GetTweaks());
    }
    else
    {
        fAttribute = fn_8002BFB8(pFielder->GetTweaks());
    }

    float fSecondary = fn_8002C254(pFielder->GetTweaks());
    float fFirst = Interpolate(fBaseSpeed, fAttribute, 0.45f);
    float fSecond = Interpolate(fAttribute, fSecondary, 0.45f);
    return NormalizeVal(fSpeed, fFirst, fSecond);
}

extern "C" float fn_800D6CD4(cPlayer* pPlayer1, cPlayer* pPlayer2)
{
    if (pPlayer1 == NULL)
    {
        return 0.0f;
    }

    if (pPlayer2 == NULL)
    {
        return 0.0f;
    }

    if (pPlayer1->m_pTeam == pPlayer2->m_pTeam)
    {
        return 1.0f;
    }

    return 0.0f;
}

extern "C" float fn_800D6D14(cPlayer* pPlayer1, cPlayer* pPlayer2)
{
    if (pPlayer1 == NULL)
    {
        return 0.0f;
    }

    if (pPlayer2 == NULL)
    {
        return 0.0f;
    }

    cTeam* pTeam = pPlayer1->m_pTeam;
    if (pTeam == pPlayer2->m_pTeam->GetOtherTeam())
    {
        return 1.0f;
    }

    return 0.0f;
}

extern "C" float fn_800D6D78(cPlayer* pPlayer)
{
    if (pPlayer == NULL)
    {
        return 0.0f;
    }

    if (pPlayer->fn_8001E160())
    {
        return 1.0f;
    }

    return 0.0f;
}

float OnTheGround(cPlayer* player)
{
    if (player == NULL)
    {
        return 0.0f;
    }

    float fFirstHeight = player->GetJointPosition(player->mUnidentified0E4).z;
    float fSecondHeight = player->GetJointPosition(player->mUnidentified0E8).z;
    float fMinHeight = FMIN(fFirstHeight, fSecondHeight);

    return NormalizeVal(fMinHeight, g_pGame->m_pFuzzyTweaks->mUnidentified424,
        g_pGame->m_pFuzzyTweaks->mUnidentified434);
}

static inline float IsPassInPlay(cBall* pBall)
{
    if (pBall->m_fTotalPassTime > 0.0f)
    {
        float fElapsedTime = pBall->m_tPassTargetTimer.GetSeconds() / pBall->m_fTotalPassTime;
        return (1.0f - fElapsedTime);
    }
    return 0.0f;
}

float StrategicBallOwner(cFielder* pFielder)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }

    float fScore;
    if (pFielder->m_pBall != NULL)
    {
        fScore = 1.0f;
    }
    else if (ReceivingPass(pFielder))
    {
        fScore = InterpolateClamped(0.7f, 0.95f, IsPassInPlay(g_pBall));
    }
    else
    {
        float fPassValid
            = g_pBall->UnidentifiedHasPassTarget() ? 1.0f : 0.0f;
        if (!fPassValid && pFielder == pFielder->m_pTeam->mpBestBallInterceptor)
        {
            fScore = InterpolateClamped(0.7f, 0.95f, AbleToInterceptBall(pFielder));
        }
        else if (g_pBall->m_tLightningTimer.m_uPackedTime != 0 && pFielder == g_pBall->m_pPrevOwner)
        {
            fScore = 0.4f;
        }
        else
        {
            fScore = InterpolateClamped(0.0f, 0.699f, AbleToInterceptBall(pFielder));
        }
    }
    return fScore;
}

float UserControlled(cFielder* fielder)
{
    if (fielder == NULL)
    {
        return 0.0f;
    }

    bool bHasGlobalPad = fielder->GetGlobalPad() != NULL;
    if (bHasGlobalPad)
    {
        return 1.0f;
    }

    return 0.0f;
}

static float InBetween(const nlVector3& v3InBetweenPos,
    const nlVector3& v3A, const nlVector3& v3B)
{
    nlVector3 v3Intercept = GetClosestPointOnLineABFromPointC(v3A, v3B, v3InBetweenPos);
    if (nlNear(v3A, v3Intercept) || nlNear(v3B, v3Intercept))
        return 0.0f;
    nlVector2 deltaA;
    deltaA.x = v3A.x - v3Intercept.x;
    deltaA.y = v3A.y - v3Intercept.y;
    float distA = nlVec2Length(deltaA);
    FuzzyTweaks* pFuzzyTweaks = g_pGame->m_pFuzzyTweaks;
    float fMaxConeWidth = InterpolateRangeClamped(pFuzzyTweaks->mUnidentified6A4,
        pFuzzyTweaks->mUnidentified6B4, pFuzzyTweaks->mUnidentified684,
        pFuzzyTweaks->mUnidentified694, distA);
    nlVector2 delta;
    delta.x = v3Intercept.x - v3InBetweenPos.x;
    delta.y = v3Intercept.y - v3InBetweenPos.y;
    return InterpolateRangeClamped(1.0f, 0.0f, 0.0f, fMaxConeWidth, nlVec2Length(delta));
}

static float InPassingLane(cFielder* pFielder, cPlayer* pPassTarget, float fPotentialBallSpeed)
{
    float fBallSpeed = fPotentialBallSpeed;
    float fScore = 0.0f;
    if (g_pBall->m_pPrevOwner == NULL || pPassTarget == NULL || !g_pBall->HasActivePassTarget())
    {
        fScore = 0.0f;
    }
    else
    {
        float fRange = InBetween(pFielder->GetPosition(),
            g_pBall->m_pPrevOwner->GetPosition(), g_pBall->m_v3PassIntercept);

        if (fRange > 0.0f)
        {
            if (fBallSpeed <= 0.0f)
            {
                nlPolar polar;
                nlCartesianToPolar(polar, g_pBall->m_v3Velocity.x, g_pBall->m_v3Velocity.y);
                fBallSpeed = polar.r;
            }

            nlVector3 v3Between2 = GetClosestPointOnLineABFromPointC(
                g_pBall->m_v3Position, g_pBall->m_v3PassIntercept, pFielder->mUnidentified024.m_v3Position);
            float fDistBall = nlSqrt(nlVec3DistanceSquared2D(g_pBall->m_v3Position, v3Between2), true);
            float fTime = fDistBall / fBallSpeed;
            fTime *= fn_8002E1B0(pFielder);

            float fDist3 = nlSqrt(nlVec3DistanceSquared2D(pFielder->mUnidentified024.m_v3Position, v3Between2), true);
            FuzzyTweaks* pFuzzyTweaks2 = g_pGame->m_pFuzzyTweaks;
            float fResult = NormalizeVal(fDist3, fTime + pFuzzyTweaks2->mUnidentified3E4, fTime);
            fScore = FMIN(fResult, fRange);
        }
    }

    return fScore;
}

float InPassingLane(cFielder* pFielder)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }

    if (!g_pBall->UnidentifiedHasPassTarget())
    {
        return 0.0f;
    }

    if (pFielder->IsOnSameTeam(g_pBall->m_pPassTarget))
    {
        return 0.0f;
    }

    return InPassingLane(pFielder, g_pBall->m_pPassTarget, 0.0f);
}

extern "C" float fn_800D74D8(cFielder* pFielder)
{
    if (pFielder == NULL)
        return 0.0f;
    cBall* pBall = g_pBall;
    if (pBall->m_tLightningTimer.m_uPackedTime == 0)
        return 0.0f;
    if (pBall->m_pPrevOwner == NULL)
        return 0.0f;
    nlVector3 v3BetweenIntercept = GetClosestPointOnLineABFromPointC(
        pBall->m_v3Position, pBall->m_pPrevOwner->GetAIOffNetLocation(NULL), pFielder->mUnidentified024.m_v3Position);
    float fBallDistance = nlSqrt(nlVec3DistanceSquared2D(g_pBall->m_v3Position, v3BetweenIntercept), true);
    nlPolar pBallSpeedPolar;
    nlCartesianToPolar(pBallSpeedPolar, g_pBall->m_v3Velocity.x, g_pBall->m_v3Velocity.y);
    float fPossibleFielderDistance = fBallDistance / pBallSpeedPolar.r;
    fPossibleFielderDistance *= fn_8002C254(pFielder->GetTweaks());
    float fDistance = nlSqrt(nlVec3DistanceSquared2D(pFielder->mUnidentified024.m_v3Position,
        v3BetweenIntercept), true);
    return NormalizeVal(fDistance, fPossibleFielderDistance + g_pGame->m_pFuzzyTweaks->mUnidentified3F4,
        fPossibleFielderDistance);
}

extern "C" float fn_800D763C(cFielder* pFielder)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }

    if (pFielder->m_eClassType == FIELDER)
    {
        return FMIN(FMAX(pFielder->GetTweaks()->fPassing,
            0.0f), 1.0f);
    }

    return 1.0f;
}

extern "C" float fn_800D76B8(cFielder* pFielder)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }

    if (pFielder->m_eClassType == FIELDER)
    {
        return FMIN(FMAX(pFielder->GetTweaks()->fShooting,
            0.0f), 1.0f);
    }

    return 1.0f;
}

extern "C" float fn_800D7734(cFielder* pFielder)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }

    if (pFielder->m_eClassType == FIELDER)
    {
        return FMIN(FMAX(pFielder->GetTweaks()->mUnidentified074,
            0.0f), 1.0f);
    }

    return 1.0f;
}

extern "C" float fn_800D77B0(cFielder* pFielder)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }

    if (pFielder->m_eClassType == FIELDER)
    {
        return FMIN(FMAX(pFielder->GetTweaks()->mUnidentified054,
            0.0f), 1.0f);
    }

    return 1.0f;
}

extern "C" float fn_800D782C(cFielder* fielder)
{
    if (fielder == NULL)
    {
        return 0.0f;
    }

    PlayerTweaks* pTweaks = fielder->GetTweaks();
    return InterpolateRangeClamped(0.0f, 1.0f, 0.5f, 1.0f, fn_8002BE64(pTweaks));
}

extern "C" float fn_800D7878(cFielder* fielder)
{
    if (fielder == NULL)
    {
        return 0.0f;
    }

    PlayerTweaks* pTweaks = fielder->GetTweaks();
    return InterpolateRangeClamped(0.0f, 1.0f, 0.5f, 1.0f, fn_8002BE84(pTweaks));
}

extern "C" float fn_800D78C4(cFielder* fielder)
{
    if (fielder == NULL)
    {
        return 0.0f;
    }

    PlayerTweaks* pTweaks = fielder->GetTweaks();
    return InterpolateRangeClamped(0.0f, 1.0f, 0.5f, 1.0f, fn_8002BE18(pTweaks));
}

extern "C" float fn_800D7910(cFielder* fielder)
{
    if (fielder == NULL)
    {
        return 0.0f;
    }

    PlayerTweaks* pTweaks = fielder->GetTweaks();
    return InterpolateRangeClamped(0.0f, 1.0f, 0.5f, 1.0f, fn_8002BE38(pTweaks));
}

extern "C" float fn_800D795C(cFielder* pFielder, int unidentified)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }

    if (unidentified == pFielder->mUnidentified024.m_eCharacterClass)
    {
        return 1.0f;
    }

    return 0.0f;
}

extern "C" float fn_800D7988(int nAction, cFielder* pFielder)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }

    Desire* pDesire = fn_8002E08C(pFielder, nAction);
    int bActive = 0;
    if ((pDesire != NULL) && pDesire->mUnidentifiedActive)
    {
        bActive = 1;
    }

    if (bActive != 0)
    {
        return 1.0f;
    }

    return 0.0f;
}

static const nlVector2 lbl_806E41E0 = { 3000.0f, 0.0f };


extern "C" float fn_800D79F4(int nAction, cFielder* pFielder)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }

    float fScore = 0.0f;
    Desire* pDesire = fn_8002E08C(pFielder, nAction);
    if (pDesire != NULL)
    {
        float fStartTime = pDesire->mUnidentified014;
        fScore = NormalizeVal(lbl_806DF564() - fStartTime, lbl_806E41E0);
    }
    return fScore;
}

extern "C" float fn_800D7A70(cFielder* pFielder)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }

    shdStateMachine* pState = fn_8002E1A4(pFielder)->mUnidentified004;
    if (pState != NULL)
    {
        pState->mUnidentifiedTimer.GetSeconds();
    }
    return 1.0f;
}

extern "C" float fn_800D7AB8(cFielder* pFielder)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }

    if (fn_8002F858(pFielder, false))
    {
        return 1.0f;
    }

    return 0.0f;
}

extern "C" float fn_800D7B00(cFielder* pFielder)
{
    if (pFielder == NULL)
        return 0.0f;
    float fScore = 0.0f;
    float fOwnerRadius = 0.0f;
    cFielder* pOwner = g_pBall->GetOwnerFielder();
    if (pOwner != NULL)
    {
        fOwnerRadius = pOwner->mUnidentified320->GetRadius();
        if (pOwner->fn_800345EC(pFielder))
            return 0.0f;
    }
    const nlVector3& vTarget = pOwner != NULL ? pOwner->mUnidentified024.m_v3Position
        : pFielder->m_pTeam->GetBallInterceptPosition(pFielder->mUnidentified1E4.m_ID);
    if (pFielder->m_eActionState != ACTION_SLIDE_ATTACK && vTarget.z <= 0.35f)
    {
        float fDuration = fn_8002C800(pFielder->GetTweaks());
        float fSpeed = fn_8003C40C(pFielder, pFielder->mUnidentified024.m_aActualFacingDirection);
        float fDistance = nlSqrt(nlVec3DistanceSquared2D(pFielder->mUnidentified024.m_v3Position, vTarget), true);
        float fRadius = pFielder->mUnidentified320->GetRadius();
        fDistance -= fRadius + fOwnerRadius;
        fScore = NormalizeVal(fDistance / fSpeed, 4.3f * fDuration, 0.08f);
    }
    if (pOwner != NULL && pOwner->fn_8003E74C() && !pFielder->fn_8003E74C())
        fScore = InterpolateRangeClamped(fScore, 0.33f * fScore, 1.0f, 0.0f, fn_800DDF54(pFielder, pOwner));
    return fScore;
}

float AbleToInterceptBall(cPlayer* pPlayer)
{
    if (pPlayer == NULL)
        return 0.0f;
    float fScore = 0.0f;
    if (!Incapacitated(pPlayer))
    {
        if (pPlayer->HasBall())
            fScore = 1.0f;
        else if (pPlayer->m_eClassType == FIELDER)
        {
            cFielder* pFielder = (cFielder*)pPlayer;
            float fInterceptTime = pFielder->m_pTeam->mfBallInTimes[pFielder->mUnidentified1E4.m_ID];
            float fInterceptScore = NormalizeVal(fInterceptTime,
                g_pGame->m_pFuzzyTweaks->mUnidentified464, g_pGame->m_pFuzzyTweaks->mUnidentified474);
            const nlVector3& v3Target = fInterceptScore < 0.35f ? g_pBall->m_v3Position
                : pFielder->m_pTeam->GetBallInterceptPosition(pFielder->mUnidentified1E4.m_ID);
            float fDistance = nlSqrt(nlVec3DistanceSquared2D(
                pFielder->mUnidentified024.m_v3Position, v3Target), true);
            float fClosenessScore = NormalizeVal(fDistance,
                g_pGame->m_pFuzzyTweaks->mUnidentified484, g_pGame->m_pFuzzyTweaks->mUnidentified494);
            fScore = fInterceptScore * g_pGame->m_pFuzzyTweaks->mUnidentified454
                + fClosenessScore * (1.0f - g_pGame->m_pFuzzyTweaks->mUnidentified454);
            bool bHasGlobalPad = pFielder->GetGlobalPad() != NULL;
            if (bHasGlobalPad)
            {
                float fClosingScore = ClosingTo(pFielder, g_pBall);
                float fFacingScore = fn_800DE0A8(pFielder);
                fScore = FMIN(1.0f, fScore * InterpolateClamped(1.0f, 1.6f,
                    fClosingScore / 2.0f + fFacingScore / 2.0f));
            }
        }
        else if (pPlayer->m_eClassType == GOALIE)
        {
            if (((Goalie*)pPlayer)->IsBusy())
                fScore = CloseToBall(pPlayer);
        }
        fScore = FMAX(0.00001f, fScore);
    }
    return fScore;
}

extern "C" float fn_800D82C0(cFielder* pFielder)
{
    if (pFielder == NULL)
        return 0.0f;
    float fScore = 0.0f;
    if (!Incapacitated(pFielder))
    {
        if (pFielder->m_pBall != NULL)
            fScore = 1.0f;
        else
        {
            float fInterceptTime = pFielder->m_pTeam->mfBallInTimes[pFielder->mUnidentified1E4.m_ID];
            float fInterceptScore = NormalizeVal(fInterceptTime,
                g_pGame->m_pFuzzyTweaks->mUnidentified464, g_pGame->m_pFuzzyTweaks->mUnidentified474);
            const nlVector3& v3Target = fInterceptScore < 0.35f ? g_pBall->m_v3Position
                : pFielder->m_pTeam->GetBallInterceptPosition(pFielder->mUnidentified1E4.m_ID);
            float fDistance = nlSqrt(nlVec3DistanceSquared2D(
                pFielder->mUnidentified024.m_v3Position, v3Target), true);
            float fClosenessScore = NormalizeVal(fDistance,
                g_pGame->m_pFuzzyTweaks->mUnidentified484, g_pGame->m_pFuzzyTweaks->mUnidentified494);
            fScore = fInterceptScore * g_pGame->m_pFuzzyTweaks->mUnidentified444
                + fClosenessScore * (1.0f - g_pGame->m_pFuzzyTweaks->mUnidentified444);
            if (fScore == 0.0f)
                tDebugPrintManager::Print((eDEBUG_CHANNEL)4,
                    "AbleToInterceptBall should never return 0! Debug yer code.\n");
        }
    }
    return fScore;
}

extern "C" float fn_800D84F8(cFielder* pFielder)
{
    if (pFielder == NULL)
        return 0.0f;
    float fScore = 0.0f;
    if (pFielder->fn_8001E168() && !pFielder->fn_8003E6EC())
    {
        int powerup = pFielder->m_pTeam->GetPowerUpByIndex(0).eType;
        if (fn_80099CE8(powerup) && fn_8002EDC8(pFielder, powerup))
            fScore = 1.0f;
        else if (pFielder->m_pTeam->fn_800A6560())
        {
            powerup = pFielder->m_pTeam->GetPowerUpByIndex(1).eType;
            if (fn_80099CE8(powerup) && fn_8002EDC8(pFielder, powerup))
                fScore = 1.0f;
        }
    }
    return fScore;
}

extern "C" float fn_800D85F8(cFielder* pFielder)
{
    if (pFielder == NULL)
        return 0.0f;
    int first = pFielder->m_pTeam->GetPowerUpByIndex(0).eType;
    int second = pFielder->m_pTeam->GetPowerUpByIndex(1).eType;
    if (pFielder->fn_8001E168())
    {
        if (first != -1 || (pFielder->m_pTeam->fn_800A6560() && second != -1))
            return (fn_8002EDC8(pFielder, first) || fn_8002EDC8(pFielder, second)) ? 1.0f : 0.0f;
        return 0.0f;
    }
    if ((first >= 0 && first < 9)
        || (pFielder->m_pTeam->fn_800A6560() && second >= 0 && second < 9))
        return (fn_8002EDC8(pFielder, first) || fn_8002EDC8(pFielder, second)) ? 1.0f : 0.0f;
    return 0.0f;
}

extern "C" float fn_800D8764(cFielder* pFielder, int powerup)
{
    if (pFielder == NULL)
        return 0.0f;
    float fScore = 0.0f;
    int first = pFielder->m_pTeam->GetPowerUpByIndex(0).eType;
    int second = pFielder->m_pTeam->GetPowerUpByIndex(1).eType;
    if (first == powerup || (pFielder->m_pTeam->fn_800A6560() && second == powerup))
        fScore = fn_8002EDC8(pFielder, powerup);
    return fScore;
}

extern "C" float fn_800D8834(cFielder* pFielder, int ePowerup)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }
    int powerup = ePowerup;
    if (ePowerup >= 9 && ePowerup <= 20)
    {
        powerup = 9;
    }
    SkillTweaks* pSkillTweaks = fn_800A636C(pFielder->m_pTeam);
    return pSkillTweaks->PowerupUsageChance[pFielder->m_pTeam->mpCurrentSituation][powerup]->GetValue();
}

extern "C" float fn_800D88B4(cFielder* pFielder)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }
    int nPowerups = 0;
    for (int i = 0; i < 2; i++)
    {
        switch (pFielder->m_pTeam->GetPowerUpByIndex(i).eType)
        {
        case 0:
        case 1:
        case 2:
        case 3:
        case 5:
            nPowerups++;
            break;
        }
    }
    return NormalizeVal((float)nPowerups, 0.0f, 2.0f);
}

extern "C" float fn_800D8970(cFielder* pFielder)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }
    int nPowerups = 0;
    for (int i = 0; i < 2; i++)
    {
        switch (pFielder->m_pTeam->GetPowerUpByIndex(i).eType)
        {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 5:
            break;
        default:
            nPowerups++;
            break;
        }
    }
    return NormalizeVal((float)nPowerups, 0.0f, 2.0f);
}

float ReallyCloseToBall(cPlayer* pPlayer)
{
    if (pPlayer == NULL)
    {
        return 0.0f;
    }

    return NormalizeVal(g_pGame->m_fCachedBallPlayerDistances[pPlayer->mUnidentified120],
        g_pGame->m_pFuzzyTweaks->mUnidentified104,
        g_pGame->m_pFuzzyTweaks->mUnidentified114);
}

float CloseToBall(cPlayer* pPlayer)
{
    if (pPlayer == NULL)
    {
        return 0.0f;
    }

    return NormalizeVal(g_pGame->m_fCachedBallPlayerDistances[pPlayer->mUnidentified120],
        g_pGame->m_pFuzzyTweaks->mUnidentified124,
        g_pGame->m_pFuzzyTweaks->mUnidentified134);
}

extern "C" float fn_800D8A9C(cFielder* pFielder)
{
    if (pFielder == NULL)
        return 0.0f;
    cTeam* pOtherTeam;
    int index;
    {
        cTeam* pTeam = pFielder->GetTeam();
        pOtherTeam = pTeam->GetOtherTeam();
        index = -1;
        for (int i = 0; i < 4; i++)
        {
            if (pTeam->GetBallInterceptFielder(i) == pFielder)
            {
                index = i;
                break;
            }
        }
    }
    for (int i = 0; i < 4; i++)
    {
        cFielder* pOpponent = pOtherTeam->GetBallInterceptFielder(i);
        float fScore = AbleToInterceptBall(pFielder);
        if (AbleToInterceptBall(pOpponent) > fScore)
            index++;
        else
            break;
    }
    return (float)(7 - index) / 7.0f;
}

extern "C" float fn_800D8BAC(cFielder* pFielder)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }
    int index = -1;
    for (int i = 0; i < 4; i++)
    {
        if (pFielder->m_pTeam->GetBallInterceptFielder(i) == pFielder)
        {
            index = i;
            break;
        }
    }
    return (float)(3 - index) / 3.0f;
}

float NearToBall(cPlayer* pPlayer)
{
    if (pPlayer == NULL)
    {
        return 0.0f;
    }

    return NormalizeVal(g_pGame->m_fCachedBallPlayerDistances[pPlayer->mUnidentified120],
        g_pGame->m_pFuzzyTweaks->mUnidentified144,
        g_pGame->m_pFuzzyTweaks->mUnidentified154);
}

float FarToBall(cPlayer* pPlayer)
{
    if (pPlayer == NULL)
    {
        return 0.0f;
    }

    return NormalizeVal(g_pGame->m_fCachedBallPlayerDistances[pPlayer->mUnidentified120],
        g_pGame->m_pFuzzyTweaks->mUnidentified164,
        g_pGame->m_pFuzzyTweaks->mUnidentified174);
}

float CloseToMyNet(cPlayer* pPlayer)
{
    if (pPlayer == NULL)
    {
        return 0.0f;
    }

    FuzzyTweaks* pFuzzyTweaks = g_pGame->m_pFuzzyTweaks;
    float fMaxDist = pFuzzyTweaks->mUnidentified194;
    float fMinDist = pFuzzyTweaks->mUnidentified184;
    const nlVector3& v3DefNetPos = pPlayer->GetAIDefNetLocation(NULL);
    nlVector2 v2Diff;
    v2Diff.x = v3DefNetPos.x - pPlayer->mUnidentified024.m_v3Position.x;
    v2Diff.y = v3DefNetPos.y - pPlayer->mUnidentified024.m_v3Position.y;
    float fDist = nlSqrt(v2Diff.x * v2Diff.x + v2Diff.y * v2Diff.y, true);
    return NormalizeVal(fDist, fMinDist, fMaxDist);
}

float NearToMyNet(cPlayer* pPlayer)
{
    if (pPlayer == NULL)
    {
        return 0.0f;
    }

    FuzzyTweaks* pFuzzyTweaks = g_pGame->m_pFuzzyTweaks;
    float fMaxDist = pFuzzyTweaks->mUnidentified1B4;
    float fMinDist = pFuzzyTweaks->mUnidentified1A4;
    const nlVector3& v3DefNetPos = pPlayer->GetAIDefNetLocation(NULL);
    nlVector2 v2Diff;
    v2Diff.x = v3DefNetPos.x - pPlayer->mUnidentified024.m_v3Position.x;
    v2Diff.y = v3DefNetPos.y - pPlayer->mUnidentified024.m_v3Position.y;
    float fDist = nlSqrt(v2Diff.x * v2Diff.x + v2Diff.y * v2Diff.y, true);
    return NormalizeVal(fDist, fMinDist, fMaxDist);
}

float FarToMyNet(cPlayer* pPlayer)
{
    if (pPlayer == NULL)
    {
        return 0.0f;
    }

    FuzzyTweaks* pFuzzyTweaks = g_pGame->m_pFuzzyTweaks;
    float fMaxDist = pFuzzyTweaks->mUnidentified1D4;
    float fMinDist = pFuzzyTweaks->mUnidentified1C4;
    const nlVector3& v3DefNetPos = pPlayer->GetAIDefNetLocation(NULL);
    nlVector2 v2Diff;
    v2Diff.x = v3DefNetPos.x - pPlayer->mUnidentified024.m_v3Position.x;
    v2Diff.y = v3DefNetPos.y - pPlayer->mUnidentified024.m_v3Position.y;
    float fDist = nlSqrt(v2Diff.x * v2Diff.x + v2Diff.y * v2Diff.y, true);
    return NormalizeVal(fDist, fMinDist, fMaxDist);
}

float CloseToTheirNet(cPlayer* pPlayer)
{
    if (pPlayer == NULL)
    {
        return 0.0f;
    }

    FuzzyTweaks* pFuzzyTweaks = g_pGame->m_pFuzzyTweaks;
    float fMaxDist = pFuzzyTweaks->mUnidentified194;
    float fMinDist = pFuzzyTweaks->mUnidentified184;
    const nlVector3& v3OffNetPos = pPlayer->GetAIOffNetLocation(NULL);
    nlVector2 v2Diff;
    v2Diff.x = v3OffNetPos.x - pPlayer->mUnidentified024.m_v3Position.x;
    v2Diff.y = v3OffNetPos.y - pPlayer->mUnidentified024.m_v3Position.y;
    float fDist = nlSqrt(v2Diff.x * v2Diff.x + v2Diff.y * v2Diff.y, true);
    return NormalizeVal(fDist, fMinDist, fMaxDist);
}

float NearToTheirNet(cPlayer* pPlayer)
{
    if (pPlayer == NULL)
    {
        return 0.0f;
    }

    FuzzyTweaks* pFuzzyTweaks = g_pGame->m_pFuzzyTweaks;
    float fMaxDist = pFuzzyTweaks->mUnidentified1B4;
    float fMinDist = pFuzzyTweaks->mUnidentified1A4;
    const nlVector3& v3OffNetPos = pPlayer->GetAIOffNetLocation(NULL);
    nlVector2 v2Diff;
    v2Diff.x = v3OffNetPos.x - pPlayer->mUnidentified024.m_v3Position.x;
    v2Diff.y = v3OffNetPos.y - pPlayer->mUnidentified024.m_v3Position.y;
    float fDist = nlSqrt(v2Diff.x * v2Diff.x + v2Diff.y * v2Diff.y, true);
    return NormalizeVal(fDist, fMinDist, fMaxDist);
}

float FarToTheirNet(cPlayer* pPlayer)
{
    if (pPlayer == NULL)
    {
        return 0.0f;
    }

    FuzzyTweaks* pFuzzyTweaks = g_pGame->m_pFuzzyTweaks;
    float fMaxDist = pFuzzyTweaks->mUnidentified1D4;
    float fMinDist = pFuzzyTweaks->mUnidentified1C4;
    const nlVector3& v3OffNetPos = pPlayer->GetAIOffNetLocation(NULL);
    nlVector2 v2Diff;
    v2Diff.x = v3OffNetPos.x - pPlayer->mUnidentified024.m_v3Position.x;
    v2Diff.y = v3OffNetPos.y - pPlayer->mUnidentified024.m_v3Position.y;
    float fDist = nlSqrt(v2Diff.x * v2Diff.x + v2Diff.y * v2Diff.y, true);
    return NormalizeVal(fDist, fMinDist, fMaxDist);
}

extern "C" float fn_800D912C(cFielder* pFielder)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }

    float fSum = 0.0f;
    for (int i = 0; i < 4; i++)
    {
        cFielder* pMate = pFielder->m_pTeam->GetFielder(i);
        if (pMate != pFielder)
        {
            fSum += CloseTo(pFielder, pMate);
        }
    }

    return fSum / 3.0f;
}

extern "C" float fn_800D91BC(cFielder* pFielder)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }

    float fSum = 0.0f;
    for (int i = 0; i < 4; i++)
    {
        cFielder* pMate = pFielder->m_pTeam->GetFielder(i);
        if (pMate != pFielder)
        {
            fSum += NearTo(pFielder, pMate);
        }
    }

    return fSum / 3.0f;
}

extern "C" float fn_800D924C(cFielder* pFielder)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }

    float fSum = 0.0f;
    for (int i = 0; i < 4; i++)
    {
        cFielder* pMate = pFielder->m_pTeam->GetFielder(i);
        if (pMate != pFielder)
        {
            fSum += FarTo(pFielder, pMate);
        }
    }

    return fSum / 3.0f;
}

extern "C" float fn_800D92DC(cFielder* pFielder)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }

    float fSum = 0.0f;
    for (int i = 0; i < 4; i++)
    {
        cFielder* pOpponent = pFielder->m_pTeam->GetOtherTeam()->GetFielder(i);
        fSum += CloseTo(pFielder, pOpponent);
    }

    return fSum / 4.0f;
}

extern "C" float fn_800D9368(cFielder* pFielder)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }

    float fSum = 0.0f;
    for (int i = 0; i < 4; i++)
    {
        cFielder* pOpponent = pFielder->m_pTeam->GetOtherTeam()->GetFielder(i);
        fSum += NearTo(pFielder, pOpponent);
    }

    return fSum / 4.0f;
}

extern "C" float fn_800D93F4(cFielder* pFielder)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }

    float fSum = 0.0f;
    for (int i = 0; i < 4; i++)
    {
        cFielder* pOpponent = pFielder->m_pTeam->GetOtherTeam()->GetFielder(i);
        fSum += FarTo(pFielder, pOpponent);
    }

    return fSum / 4.0f;
}

extern "C" float fn_800D9480(cFielder* pFielder)
{
    if (pFielder == NULL)
        return 0.0f;
    float fScore = 0.0f;
    for (int i = 0; i < 4; i++)
    {
        cFielder* pOpponent = pFielder->m_pTeam->GetOtherTeam()->GetFielder(i);
        float fNear = NearTo(pFielder, pOpponent);
        if (!Incapacitated(pOpponent) && fNear >= 0.4f)
        {
            float fClosing = ClosingTo(pFielder, pOpponent);
            fScore += fNear * g_pGame->m_pFuzzyTweaks->mUnidentified4A4
                + fClosing * (1.0f - g_pGame->m_pFuzzyTweaks->mUnidentified4A4);
        }
    }
    fScore *= 0.5f;
    return FMIN(FMAX(fScore, 0.0f), 1.0f);
}

extern "C" float fn_800D96F4(cFielder* pFielder)
{
    if (pFielder == NULL)
        return 0.0f;
    float fScore = 0.0f;
    for (int i = 0; i < 5; i++)
    {
        cPlayer* pOpponent = pFielder->m_pTeam->GetOtherTeam()->GetPlayer(i);
        bool bAttacking = false;
        if (pOpponent->m_eClassType == GOALIE)
        {
            Goalie* pGoalie = (Goalie*)pOpponent;
            bAttacking = pGoalie->mGoalieActionState == GOALIEACTION_PURSUE_BALL_CARRIER
                || pGoalie->mGoalieActionState == GOALIEACTION_PURSUE_BALL_POUNCE;
        }
        else if (pOpponent->m_eClassType == FIELDER)
        {
            cFielder* pOtherFielder = (cFielder*)pOpponent;
            bAttacking = pOtherFielder->m_eActionState == ACTION_SLIDE_ATTACK
                || pOtherFielder->m_eActionState == ACTION_HIT;
        }
        if (bAttacking)
        {
            float fFacing = fn_800DDF54(pOpponent, pFielder);
            float fClosing = ClosingTo(pFielder, pOpponent);
            fClosing = FMIN(NearTo(pFielder, pOpponent), fClosing);
            fClosing = FMAX(CloseTo(pFielder, pOpponent), fClosing);
            float fAngleWeight = 0.2f;
            float fClosingWeight = 0.8f;
            fScore += fFacing * fAngleWeight + fClosing * fClosingWeight;
        }
    }
    return FMIN(FMAX(fScore, 0.0f), 1.0f);
}

extern "C" float fn_800D9A38(cFielder* pFielder)
{
    if (pFielder == NULL)
        return 0.0f;
    nlVector3 vRepulsion = fn_8002E144(pFielder)->GetLastRepulsionVector(AVOID_GOALIES);
    float fMagnitude = nlVec3Length(vRepulsion);
    float fScore = NormalizeVal(fMagnitude, g_pGame->m_pFuzzyTweaks->mUnidentified4B4,
        g_pGame->m_pFuzzyTweaks->mUnidentified4C4);
    lbl_806DC3E8 = FMIN(fMagnitude, lbl_806DC3E8);
    lbl_806DC3EC = FMAX(fMagnitude, lbl_806DC3EC);
    return fScore;
}

extern "C" float fn_800D9B0C(cFielder* pFielder)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }

    float fAvoid = fn_8000F558(fn_8002E144(pFielder), AVOID_FIELDERS);
    return FMIN(FMAX(fAvoid, 0.0f), 1.0f);
}

extern "C" float fn_800D9B74(cFielder* pFielder)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }

    float fAvoid = fn_8000F558(fn_8002E144(pFielder), AVOID_POWERUPS);
    return FMIN(FMAX(fAvoid, 0.0f), 1.0f);
}

extern "C" float fn_800D9BDC(cFielder* pFielder)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }

    if ((fn_8002E144(pFielder)->m_CurrentlyAvoiding & AVOID_SIDELINES) != 0)
    {
        return 1.0f;
    }

    return 0.0f;
}

extern "C" float fn_800D9C24(cFielder* pFielder)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }

    float fAvoid = fn_8000F558(fn_8002E144(pFielder), AVOID_EVERYTHING);
    return FMIN(FMAX(fAvoid, 0.0f), 1.0f);
}

float Invincible(cFielder* pFielder)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }

    if (pFielder->IsInvincible())
    {
        return 1.0f;
    }

    return 0.0f;
}

extern "C" float fn_800D9D04(cFielder* pFielder)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }

    if (pFielder->IsInvincibleChars())
    {
        return 1.0f;
    }

    return 0.0f;
}

extern "C" float fn_800D9D78(cPlayer* pPlayer)
{
    if (pPlayer == NULL)
    {
        return 0.0f;
    }
    if (pPlayer->m_eClassType == FIELDER && ((cFielder*)pPlayer)->IsFallenDown())
    {
        return pPlayer->m_pCurrentAnimController->get_fTime();
    }
    return 1.0f;
}

extern "C" float fn_800D9DD8(cPlayer* pPlayer)
{
    if (pPlayer == NULL)
    {
        return 0.0f;
    }

    float fScore = 0.0f;
    if (pPlayer->m_eClassType == GOALIE)
    {
        fScore = 1.0f;
    }
    else if (pPlayer->m_eClassType == FIELDER)
    {
        cFielder* pFielder = (cFielder*)pPlayer;
        nlVector3 v3Position = pFielder->mUnidentified024.m_v3Position;
        bool bOutOfBounds = cField::FixOutOfBoundsPosition(v3Position, -1.0f, true);
        bool bIncapacitated = pFielder->fn_80038918() || pFielder->fn_800344B0()
            || pFielder->m_eActionState == 35
            || pFielder->m_eActionState == ACTION_ELECTROCUTION || bOutOfBounds;
        fScore = bIncapacitated ? 1.0f : 0.0f;
    }
    return fScore;
}

float Incapacitated(cPlayer* pPlayer)
{
    if (pPlayer == NULL)
    {
        return 0.0f;
    }

    float fScore = 0.0f;
    if (pPlayer->m_eClassType == GOALIE)
    {
        Goalie* pGoalie = (Goalie*)pPlayer;
        fScore = (pGoalie->IsRecovering() || !pGoalie->IsBusy()) ? 1.0f : 0.0f;
    }
    else if (pPlayer->m_eClassType == FIELDER)
    {
        cFielder* pFielder = (cFielder*)pPlayer;
        fScore = (pFielder->fn_80038918() || pFielder->IsFallenDown()) ? 1.0f : 0.0f;
    }
    return fScore;
}

extern "C" float fn_800D9FC8(cFielder* pFielder)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }

    if (pFielder->IsStuck())
    {
        return 1.0f;
    }

    return 0.0f;
}

float FallenDown(cFielder* pFielder)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }

    if (pFielder->IsFallenDown())
    {
        return 1.0f;
    }

    return 0.0f;
}

extern "C" float fn_800DA050(cFielder* pFielder)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }

    int bFlag = 0;
    if (pFielder->fn_800344B0() || pFielder->IsShattered())
    {
        bFlag = 1;
    }

    return (bFlag != 0) ? 1.0f : 0.0f;
}

extern "C" float fn_800DA0C8(cFielder* pFielder)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }

    float fScore = 0.0f;
    DesireGooey* pDesire = (DesireGooey*)fn_8002E08C(pFielder, 27);
    if (pDesire != NULL && pDesire->UnidentifiedIsActive())
    {
        fScore = pDesire->fn_800BD1F0();
    }
    return fScore;
}

extern "C" float fn_800DA130(cFielder* pFielder)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }

    float fResult = 0.0f;
    Desire* pDesire = fn_8002E08C(pFielder, 0x1E);
    if ((pDesire != NULL) && pDesire->mUnidentifiedActive)
    {
        fResult = 1.0f;
    }

    return fResult;
}

float GoalieOutOfPosition(cFielder* pFielder)
{
    nlVector3 goalieNetPos;
    cPlayer* pGoalie;

    if (pFielder == NULL)
    {
        return 0.0f;
    }

    pGoalie = (cPlayer*)pFielder->m_pTeam->GetOtherTeam()->GetGoalie();
    float halfNetWidth = 0.5f * cNet::GetNetWidth();
    goalieNetPos = pGoalie->mUnidentified024.m_v3Position;
    goalieNetPos.x = pGoalie->m_pTeam->m_pNet->m_v3NetLocation.x;

    float goalieY = goalieNetPos.y;
    if (goalieY < -halfNetWidth)
    {
        goalieNetPos.y = -halfNetWidth;
    }
    else if (goalieY > halfNetWidth)
    {
        goalieNetPos.y = halfNetWidth;
    }

    const nlVector3& offNetLocation = pFielder->GetAIOffNetLocation(NULL);

    float fielderDistance = nlSqrt(
        nlVec3DistanceSquared2D(pFielder->mUnidentified024.m_v3Position, offNetLocation), true);

    nlVector2 goalieDelta;
    nlVec2Sub(goalieDelta, *(const nlVector2*)&pGoalie->GetPosition(),
        *(const nlVector2*)&goalieNetPos);
    float goalieDistance = nlVec2Length(goalieDelta);

    if (!((double)fielderDistance > 0.0))
    {
        fielderDistance = 0.1f;
    }

    if (!((double)goalieDistance > 0.0))
    {
        goalieDistance = 0.1f;
    }

    return NormalizeVal(goalieDistance / fielderDistance,
        g_pGame->m_pFuzzyTweaks->mUnidentified554,
        g_pGame->m_pFuzzyTweaks->mUnidentified564);
}

extern "C" float fn_800DA310(cFielder* pFielder)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }

    return fn_800A0508(pFielder, false, false);
}

extern "C" float fn_800DA330(cFielder* pFielder)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }

    return fn_800A0508(pFielder, true, false);
}

float PositionIsInFrontOfNet(const nlVector3& v3Position, const cNet* pNet)
{
    float sideSign;
    nlVector3 diff;
    nlVec3Set(diff,
        v3Position.x - pNet->m_v3NetLocation.x,
        v3Position.y - pNet->m_v3NetLocation.y,
        v3Position.z - pNet->m_v3NetLocation.z);
    sideSign = pNet->m_fDirection;
    nlVec3Scale(diff, diff, sideSign);

    nlPolar polar;
    nlCartesianToPolar(polar, diff);

    float angleRad = 0.005493164f * (u16)(polar.a - 0x8000);
    if (angleRad > 180.0f)
    {
        FuzzyTweaks* pFuzzyTweaks = g_pGame->m_pFuzzyTweaks;
        float complementaryMidAngle = 360.0f - pFuzzyTweaks->fFrontOfNetMidAngle;
        if (angleRad < complementaryMidAngle)
        {
            return InterpolateRangeClamped(0.0f, pFuzzyTweaks->fFrontOfNetMidScore,
                360.0f - pFuzzyTweaks->fFrontOfNetMaxAngle, complementaryMidAngle, angleRad);
        }
        return InterpolateRangeClamped(1.0f, pFuzzyTweaks->fFrontOfNetMidScore,
            360.0f, complementaryMidAngle, angleRad);
    }

    FuzzyTweaks* pFuzzyTweaks = g_pGame->m_pFuzzyTweaks;
    bool bGreaterThanMid = (angleRad > *pFuzzyTweaks->fFrontOfNetMidAngle.m_pValue);
    if (bGreaterThanMid)
    {
        return InterpolateRangeClamped(0.0f, pFuzzyTweaks->fFrontOfNetMidScore,
            pFuzzyTweaks->fFrontOfNetMaxAngle, pFuzzyTweaks->fFrontOfNetMidAngle, angleRad);
    }
    return InterpolateRangeClamped(1.0f, pFuzzyTweaks->fFrontOfNetMidScore,
        0.0f, pFuzzyTweaks->fFrontOfNetMidAngle, angleRad);
}

float InFrontOfTheirNet(cFielder* pFielder)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }

    cTeam* pOtherTeam = pFielder->m_pTeam->GetOtherTeam();
    cNet* pNet = pOtherTeam->m_pNet;
    return PositionIsInFrontOfNet(pFielder->mUnidentified024.m_v3Position, pNet);
}

float InFrontOfMyNet(cFielder* pFielder)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }

    return PositionIsInFrontOfNet(pFielder->mUnidentified024.m_v3Position, pFielder->m_pTeam->m_pNet);
}

extern "C" float fn_800DA518(cFielder* pFielder)
{
    if (pFielder == NULL)
        return 0.0f;
    if (InOffensiveZone(pFielder) > 0.5f)
    {
        nlVector3 v3FormationPos;
        if (fn_800381B4(pFielder, &v3FormationPos))
            v3FormationPos = pFielder->mUnidentified024.m_v3Position;
        SSearchCutAndBreak search(pFielder);
        nlVector3 v3BestPosition;
        return search.FindBestPosition(v3BestPosition, v3FormationPos,
            (eFieldDirection)0, NULL, 8.0f, 0x8000);
    }
    return 0.0f;
}

static float CloseToFormationPosition(cFielder* pFielder, const nlVector3& vPosition)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }
    nlVector3 v3FormationPos;
    fn_800381B4(pFielder, &v3FormationPos);
    FuzzyTweaks* pFuzzyTweaks = g_pGame->m_pFuzzyTweaks;
    float fMaxDist = pFuzzyTweaks->mUnidentified254;
    float fMinDist = pFuzzyTweaks->mUnidentified244;
    return NormalizeVal(nlSqrt(nlVec3DistanceSquared2D(v3FormationPos,
        vPosition), true), fMinDist, fMaxDist);
}

float CloseToFormationPosition(cFielder* pFielder)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }
    return CloseToFormationPosition(pFielder, pFielder->GetPosition());
}

float NearToFormationPosition(cFielder* pFielder)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }
    return fn_800DA7A8(pFielder, &pFielder->mUnidentified024.m_v3Position);
}

extern "C" float fn_800DA7A8(cFielder* pFielder, nlVector3* pPosition)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }
    nlVector3 v3FormationPos;
    fn_800381B4(pFielder, &v3FormationPos);
    FuzzyTweaks* pFuzzyTweaks = g_pGame->m_pFuzzyTweaks;
    float fMaxDist = pFuzzyTweaks->mUnidentified274;
    float fMinDist = pFuzzyTweaks->mUnidentified264;
    return NormalizeVal(nlSqrt(nlVec3DistanceSquared2D(v3FormationPos,
        *pPosition), true), fMinDist, fMaxDist);
}

static float FarToFormationPosition(cFielder* pFielder, const nlVector3& vPosition)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }
    nlVector3 v3FormationPos;
    fn_800381B4(pFielder, &v3FormationPos);
    FuzzyTweaks* pFuzzyTweaks = g_pGame->m_pFuzzyTweaks;
    float fMaxDist = pFuzzyTweaks->mUnidentified294;
    float fMinDist = pFuzzyTweaks->mUnidentified284;
    return NormalizeVal(nlSqrt(nlVec3DistanceSquared2D(v3FormationPos,
        vPosition), true), fMinDist, fMaxDist);
}

float FarToFormationPosition(cFielder* pFielder)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }
    return FarToFormationPosition(pFielder, pFielder->GetPosition());
}

extern "C" float fn_800DA91C(cFielder* pFielder)
{
    if (pFielder == NULL)
        return 0.0f;
    float fScore = 0.0f;
    for (int i = 0; i < 4; i++)
    {
        cFielder* pOpponent = pFielder->m_pTeam->GetOtherTeam()->GetFielder(i);
        float fUpfieldScore = fn_800DE40C(pFielder, pOpponent);
        float fInterceptScore = InBetweenMyNetAnd(pOpponent, pFielder);
        float fProximityScore = NearTo(pFielder, pOpponent);
        float fMaxScore = FMAX(fProximityScore, fInterceptScore);
        float fWeight = 1.0f;
        fScore += fWeight * (fWeight - fMaxScore) + fUpfieldScore * fMaxScore;
        fScore -= fInterceptScore;
    }
    return NormalizeVal(fScore, 0.0f, 4.0f);
}

extern "C" float fn_800DACF4(cPlayer* pPlayer)
{
    if (pPlayer == NULL)
    {
        return 0.0f;
    }

    if (cField::IsOnField(pPlayer->mUnidentified024.m_v3Position) == false)
    {
        return 1.0f;
    }

    return 0.0f;
}

extern "C" float fn_800DAD3C(cBall* ball)
{
    if (ball == NULL)
    {
        return 0.0f;
    }

    if (cField::IsOnField(ball->m_v3Position) == false)
    {
        return 1.0f;
    }

    return 0.0f;
}

extern "C" float fn_800DAD84(const nlVector3& vFrom, const nlVector3& vTo,
    unsigned short aDirection, const nlVector2* pDistanceRange,
    const nlVector2* pAngleRange, bool bDistancePeak, bool bRequireInRange,
    float fDistanceWeight)
{
    if (pAngleRange == NULL)
        pAngleRange = &lbl_806DC3E0;
    if (pDistanceRange == NULL)
        pDistanceRange = &lbl_806DC3D8;
    nlVector2 diff;
    nlVec2Set(diff, vTo.x - vFrom.x, vTo.y - vFrom.y);
    float fDistance = nlVec2Length(diff);
    float fDistanceScore;
    if (bDistancePeak)
    {
        float fPeak = pDistanceRange->x;
        float fWidth = pDistanceRange->y;
        fDistanceScore = fDistance < fPeak
            ? NormalizeVal(fDistance, fPeak - fWidth, fPeak)
            : NormalizeVal(fDistance, fPeak + fWidth, fPeak);
    }
    else
        fDistanceScore = NormalizeVal(fDistance, *pDistanceRange);
    float dx = diff.x;
    float dy = diff.y;
    s16 aDelta = nlAngleDiff(aDirection, RadToAng16(nlATan2f(dy, dx)));
    s16 aAbsDelta = abs_ang16(aDelta);
    float fAngleScore = NormalizeVal((float)aAbsDelta, *pAngleRange);
    if (bRequireInRange
        && (fDistance < FMIN(pDistanceRange->x, pDistanceRange->y)
            || fDistance > FMAX(pDistanceRange->x, pDistanceRange->y)
            || (float)aAbsDelta < FMIN(pAngleRange->x, pAngleRange->y)
            || (float)aAbsDelta > FMAX(pAngleRange->x, pAngleRange->y)))
    {
        return 0.0f;
    }
    return fDistanceWeight * fDistanceScore + (1.0f - fDistanceWeight) * fAngleScore;
}

static const nlVector2 lbl_806E4258 = { 10922.5f, 0.0f };


extern "C" float fn_800DAFCC(const nlVector3& vFrom, const nlVector3& vTo,
    cPlayer* pIgnorePlayer1, cPlayer* pIgnorePlayer2, float fTeamWeight,
    float fOpponentWeight, float fGoalieWeight, float fPredictionTime)
{
    nlVector3 diff;
    nlVec3Sub(diff, vTo, vFrom);
    float fClosedScore = 0.0f;
    float fDistance = nlVec3Length(diff);
    if (nlNear(fDistance, 0.0f))
        return 1.0f;
    unsigned short aDirection = nlVector3ToAngle(diff);
    for (int team = 0; team < 2; team++)
    {
        cTeam* pTeam = g_pTeams[team];
        for (int i = 0; i <= 4; i++)
        {
            cPlayer* pPlayer = pTeam->GetPlayer(i);
            if (pPlayer == pIgnorePlayer1 || pPlayer == pIgnorePlayer2)
                continue;
            nlVector3 vPosition;
            if (fPredictionTime)
                nlVec3ScaleAdd(vPosition, fPredictionTime,
                    pPlayer->mUnidentified024.m_v3Velocity, pPlayer->mUnidentified024.m_v3Position);
            else
                vPosition = pPlayer->mUnidentified024.m_v3Position;
            nlVector2 distanceRange = { 0.0f, 0.0f };
            distanceRange.y = fDistance;
            float fScore = fn_800DAD84(vFrom, vPosition, aDirection, &distanceRange,
                &lbl_806E4258, false, true, 0.2f);
            if (pIgnorePlayer1 != NULL && pIgnorePlayer1->m_pTeam == pPlayer->m_pTeam)
                fScore *= fTeamWeight;
            else if (pPlayer->m_eClassType == FIELDER)
                fScore *= fOpponentWeight;
            else if (pPlayer->m_eClassType == GOALIE)
                fScore *= fGoalieWeight;
            fClosedScore += fScore;
            if (fClosedScore > 1.0f)
                break;
        }
    }
    fClosedScore = FMIN(FMAX(fClosedScore, 0.0f), 1.0f);
    return 1.0f - fClosedScore;
}

extern "C" float fn_800DB298(const nlVector3& vFrom, const nlVector3& vTo,
    cFielder* pIgnorePlayer1, float fTeamWeight, float fOpponentWeight,
    float fGoalieWeight, float fPredictionTime, cPlayer* pIgnorePlayer2)
{
    nlVector3 diff;
    nlVec3Sub(diff, vTo, vFrom);
    float fClosedScore = 0.0f;
    float fDistance = nlVec3Length(diff);
    if (nlNear(fDistance, 0.0f))
        return 1.0f;
    unsigned short aDirection = nlVector3ToAngle(diff);
    float fWidth = cNet::GetNetWidth() + cNet::GetPostRadius();
    nlVector2 post1;
    nlVector2 post2;
    nlVec2Set(post1, vTo.x, 0.0f);
    post1.y += fWidth;
    nlVec2Set(post2, vTo.x, 0.0f);
    post2.y -= fWidth;
    float dx1 = post1.x - vFrom.x;
    float dy1 = post1.y - vFrom.y;
    unsigned short aPost1 = RadToAng16(nlATan2f(dy1, dx1));
    float dx2 = post2.x - vFrom.x;
    float dy2 = post2.y - vFrom.y;
    unsigned short aPost2 = RadToAng16(nlATan2f(dy2, dx2));
    s16 absDelta1 = abs_ang16(nlAngleDiff(aDirection, aPost1));
    s16 absDelta2 = abs_ang16(nlAngleDiff(aDirection, aPost2));
    nlVector2 angleRange = { 0.0f, 0.0f };
    angleRange.x = absDelta1 < absDelta2 ? (float)absDelta2 : (float)absDelta1;
    for (int team = 0; team < 2; team++)
    {
        cTeam* pTeam = g_pTeams[team];
        for (int i = 0; i <= 4; i++)
        {
            cPlayer* pPlayer = pTeam->GetPlayer(i);
            if (pPlayer == pIgnorePlayer1 || pPlayer == pIgnorePlayer2)
                continue;
            nlVector3 vPosition;
            if (fPredictionTime)
                nlVec3ScaleAdd(vPosition, fPredictionTime,
                    pPlayer->mUnidentified024.m_v3Velocity, pPlayer->mUnidentified024.m_v3Position);
            else
                vPosition = pPlayer->mUnidentified024.m_v3Position;
            nlVector2 distanceRange = { 0.0f, 0.0f };
            distanceRange.y = fDistance;
            float fScore = fn_800DAD84(vFrom, vPosition, aDirection, &distanceRange,
                &angleRange, false, true, 0.2f);
            if (pIgnorePlayer1 != NULL && pIgnorePlayer1->m_pTeam == pPlayer->m_pTeam)
                fScore *= fTeamWeight;
            else if (pPlayer->m_eClassType == FIELDER)
                fScore *= fOpponentWeight;
            else if (pPlayer->m_eClassType == GOALIE)
                fScore *= fGoalieWeight;
            fClosedScore += fScore;
            if (fClosedScore > 1.0f)
                break;
        }
    }
    fClosedScore = FMIN(FMAX(fClosedScore, 0.0f), 1.0f);
    return 1.0f - fClosedScore;
}

extern "C" float fn_800DB678(const nlVector3& v3Position, cTeam* pOpponentTeam,
    cPlayer* pCurrentPlayer, const nlVector2* vOpenRadius, bool bIgnoreIncapacitated,
    float fPredictionTime)
{
    cPlayer* pPlayer;
    float fTotalScore = 0.0f;
    float fWeight = 1.0f;
    float fCurrentRadius = 0.0f;
    if (pCurrentPlayer != NULL)
        pCurrentPlayer->m_pPhysicsCharacter->GetRadius(&fCurrentRadius);
    bool bDefaultOpenRadius = false;
    if (vOpenRadius == NULL)
        bDefaultOpenRadius = true;
    int opponentSide = pOpponentTeam->m_nSide;
    int currentSide = pOpponentTeam->GetOtherTeam()->m_nSide;
    for (int team = 0; team < 2 && fTotalScore < 1.0f; team++)
    {
        cTeam* pTeam = g_pTeams[team == 0 ? opponentSide : currentSide];
        for (int player = 0; player < 5 && fTotalScore < 1.0f; player++)
        {
            if (pCurrentPlayer != NULL)
                pPlayer = g_pGame->fn_8005B780(pCurrentPlayer->mUnidentified120, pTeam->m_nSide, player);
            else
                pPlayer = pTeam->GetPlayer(player);
            if (pPlayer == pCurrentPlayer)
                continue;
            if (bIgnoreIncapacitated && Incapacitated(pPlayer))
                continue;
            float fPlayerRadius;
            pPlayer->m_pPhysicsCharacter->GetRadius(&fPlayerRadius);
            nlVector3 v3PlayerPosition;
            if (fPredictionTime)
                nlVec3ScaleAdd(v3PlayerPosition, fPredictionTime,
                    pPlayer->mUnidentified024.m_v3Velocity, pPlayer->mUnidentified024.m_v3Position);
            else
                v3PlayerPosition = pPlayer->mUnidentified024.m_v3Position;
            float distance = nlSqrt(nlVec3DistanceSquared2D(v3Position, v3PlayerPosition), true);
            distance -= fCurrentRadius + fPlayerRadius;
            float fScore;
            if (bDefaultOpenRadius)
                fScore = NormalizeVal(distance, g_pGame->m_pFuzzyTweaks->mUnidentified644,
                    g_pGame->m_pFuzzyTweaks->mUnidentified654);
            else
                fScore = NormalizeVal(distance, *vOpenRadius);
            if (pPlayer->IsOnSameTeam(pCurrentPlayer))
                fScore *= 0.3f;
            else if (pPlayer->m_eClassType == GOALIE)
                fScore *= 1.5f;
            if (fScore > 0.0f)
            {
                fTotalScore += fWeight * fScore;
                fWeight *= 0.5f;
            }
        }
    }
    return FMIN(FMAX(1.0f - fTotalScore, 0.0f), 1.0f);
}

extern "C" float fn_800DBA68(const nlVector3& v3Position, cTeam* pOpponentTeam,
    cPlayer* pCurrentPlayer, bool bIgnoreIncapacitated, float fPredictionTime)
{
    nlVector2 radius;
    nlVec2Set(radius, g_pGame->m_pFuzzyTweaks->mUnidentified664,
        g_pGame->m_pFuzzyTweaks->mUnidentified674);
    return fn_800DB678(v3Position, pOpponentTeam, pCurrentPlayer, &radius,
        bIgnoreIncapacitated, fPredictionTime);
}

extern "C" float fn_800DBAB0(cFielder* pFielder)
{
    if (pFielder == NULL)
        return 0.0f;
    return fn_800DB678(pFielder->mUnidentified024.m_v3Position,
        pFielder->m_pTeam->GetOtherTeam(), pFielder, NULL, true, 0.0f);
}

extern "C" float fn_800DBB0C(cFielder* pFielder)
{
    if (pFielder == NULL)
        return 0.0f;
    return fn_800DBA68(pFielder->mUnidentified024.m_v3Position,
        pFielder->m_pTeam->GetOtherTeam(), pFielder, true, 0.0f);
}

extern "C" float fn_800DBB88(cFielder* pFielder)
{
    if (pFielder == NULL)
        return 0.0f;
    return fn_800DAFCC(pFielder->mUnidentified024.m_v3Position,
        pFielder->GetAIOffNetLocation(NULL), pFielder, NULL, 0.0f, 0.2f, 1.0f, 0.0f);
}

float LikelyToScore(cFielder* pFielder)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }

    cNet* pNet = pFielder->m_pTeam->GetOtherNet();
    return fn_800DB298(pFielder->mUnidentified024.m_v3Position, pNet->m_v3NetLocation,
        pFielder, 0.0f, 0.2f, 1.0f, 0.0f, NULL);
}

float InBetweenMyNetAnd(cFielder* pFielder, cFielder* pOtherFielder)
{
    if (pFielder == NULL)
        return 0.0f;
    if (pOtherFielder == NULL)
        return 0.0f;
    return InBetween(pFielder->GetPosition(),
        pOtherFielder->GetPosition(), pFielder->m_pTeam->m_pNet->m_v3NetLocation);
}

extern "C" float fn_800DBEF4(cFielder* pFielder, cFielder* pOtherFielder)
{
    if (pFielder == NULL)
        return 0.0f;
    if (pOtherFielder == NULL)
        return 0.0f;
    return InBetween(pFielder->GetPosition(),
        pOtherFielder->GetPosition(), pFielder->m_pTeam->GetOtherNet()->m_v3NetLocation);
}

extern "C" float fn_800DC19C(cFielder* pFielder, cBall* pBall)
{
    if (pFielder == NULL)
        return 0.0f;
    if (pBall == NULL)
        return 0.0f;
    return InBetween(pFielder->GetPosition(),
        pBall->GetPosition(), pFielder->m_pTeam->m_pNet->m_v3NetLocation);
}

extern "C" float fn_800DC434(cFielder* pFielder, cBall* pBall)
{
    if (pFielder == NULL)
        return 0.0f;
    if (pBall == NULL)
        return 0.0f;
    return InBetween(pFielder->GetPosition(),
        pBall->GetPosition(), pFielder->m_pTeam->GetOtherNet()->m_v3NetLocation);
}

static const nlVector2 g_vOpenToAdjust = { 0.0f, 0.8f };


float OpenTo(cPlayer* pFromFielder, cPlayer* pToFielder)
{
    if (pFromFielder == NULL)
    {
        return 0.0f;
    }

    if (pToFielder == NULL)
    {
        return 0.0f;
    }

    float fResult = fn_800DAFCC(pFromFielder->mUnidentified024.m_v3Position,
        pToFielder->mUnidentified024.m_v3Position, pFromFielder, pToFielder,
        0.5f, 1.0f, 1.0f, 0.0f);
    return NormalizeVal(fResult, g_vOpenToAdjust);
}

float CloseTo(cPlayer* pPlayer1, cPlayer* pPlayer2)
{
    if (pPlayer1 == NULL)
    {
        return 0.0f;
    }

    if (pPlayer2 == NULL)
    {
        return 0.0f;
    }

    float fScore;
    if (pPlayer1->m_eClassType == GOALIE)
    {
        fScore = NormalizeVal(
            g_pGame->fn_8005B748(pPlayer2->mUnidentified120, pPlayer1->mUnidentified120),
            g_pGame->m_pFuzzyTweaks->mUnidentified2A4, g_pGame->m_pFuzzyTweaks->mUnidentified2B4);
    }
    else if (pPlayer2->m_eClassType == GOALIE)
    {
        fScore = NormalizeVal(
            g_pGame->fn_8005B748(pPlayer1->mUnidentified120, pPlayer2->mUnidentified120),
            g_pGame->m_pFuzzyTweaks->mUnidentified2A4, g_pGame->m_pFuzzyTweaks->mUnidentified2B4);
    }
    else
    {
        nlVector2 vConfidence;
        if (pPlayer1->IsOnSameTeam(pPlayer2))
        {
            nlVec2Set(vConfidence, g_pGame->m_pFuzzyTweaks->mUnidentified044,
                g_pGame->m_pFuzzyTweaks->mUnidentified054);
        }
        else
        {
            nlVec2Set(vConfidence, g_pGame->m_pFuzzyTweaks->mUnidentified0A4,
                g_pGame->m_pFuzzyTweaks->mUnidentified0B4);
        }

        fScore = NormalizeVal(
            g_pGame->fn_8005B748(pPlayer1->mUnidentified120, pPlayer2->mUnidentified120),
            vConfidence);
    }
    return fScore;
}

float NearTo(cPlayer* pPlayer1, cPlayer* pPlayer2)
{
    if (pPlayer1 == NULL)
    {
        return 0.0f;
    }

    if (pPlayer2 == NULL)
    {
        return 0.0f;
    }

    float fScore;
    if (pPlayer1->m_eClassType == GOALIE)
    {
        fScore = NormalizeVal(
            g_pGame->fn_8005B748(pPlayer2->mUnidentified120, pPlayer1->mUnidentified120),
            g_pGame->m_pFuzzyTweaks->mUnidentified2C4, g_pGame->m_pFuzzyTweaks->mUnidentified2D4);
    }
    else if (pPlayer2->m_eClassType == GOALIE)
    {
        fScore = NormalizeVal(
            g_pGame->fn_8005B748(pPlayer1->mUnidentified120, pPlayer2->mUnidentified120),
            g_pGame->m_pFuzzyTweaks->mUnidentified2C4, g_pGame->m_pFuzzyTweaks->mUnidentified2D4);
    }
    else
    {
        nlVector2 vConfidence;
        if (pPlayer1->IsOnSameTeam(pPlayer2))
        {
            nlVec2Set(vConfidence, g_pGame->m_pFuzzyTweaks->mUnidentified064,
                g_pGame->m_pFuzzyTweaks->mUnidentified074);
        }
        else
        {
            nlVec2Set(vConfidence, g_pGame->m_pFuzzyTweaks->mUnidentified0C4,
                g_pGame->m_pFuzzyTweaks->mUnidentified0D4);
        }

        fScore = NormalizeVal(
            g_pGame->fn_8005B748(pPlayer1->mUnidentified120, pPlayer2->mUnidentified120),
            vConfidence);
    }
    return fScore;
}

float FarTo(cPlayer* pPlayer1, cPlayer* pPlayer2)
{
    if (pPlayer1 == NULL)
    {
        return 0.0f;
    }

    if (pPlayer2 == NULL)
    {
        return 0.0f;
    }

    float fScore;
    if (pPlayer1->m_eClassType == GOALIE)
    {
        fScore = NormalizeVal(
            g_pGame->fn_8005B748(pPlayer2->mUnidentified120, pPlayer1->mUnidentified120),
            g_pGame->m_pFuzzyTweaks->mUnidentified2E4, g_pGame->m_pFuzzyTweaks->mUnidentified2F4);
    }
    else if (pPlayer2->m_eClassType == GOALIE)
    {
        fScore = NormalizeVal(
            g_pGame->fn_8005B748(pPlayer1->mUnidentified120, pPlayer2->mUnidentified120),
            g_pGame->m_pFuzzyTweaks->mUnidentified2E4, g_pGame->m_pFuzzyTweaks->mUnidentified2F4);
    }
    else
    {
        nlVector2 vConfidence;
        if (pPlayer1->IsOnSameTeam(pPlayer2))
        {
            nlVec2Set(vConfidence, g_pGame->m_pFuzzyTweaks->mUnidentified084,
                g_pGame->m_pFuzzyTweaks->mUnidentified094);
        }
        else
        {
            nlVec2Set(vConfidence, g_pGame->m_pFuzzyTweaks->mUnidentified0E4,
                g_pGame->m_pFuzzyTweaks->mUnidentified0F4);
        }

        fScore = NormalizeVal(
            g_pGame->fn_8005B748(pPlayer1->mUnidentified120, pPlayer2->mUnidentified120),
            vConfidence);
    }
    return fScore;
}

extern "C" float fn_800DCB4C(const nlVector3* pA, const nlVector3* pB)
{
    float fMax = g_pGame->m_pFuzzyTweaks->mUnidentified2D4;
    float fMin = g_pGame->m_pFuzzyTweaks->mUnidentified2C4;
    float fDist = nlSqrt(nlVec3DistanceSquared2D(*pA, *pB), true);
    return NormalizeVal(fDist, fMin, fMax);
}

float CloseToMyGoalie(cPlayer* pPlayer)
{
    cPlayer* pGoalie = pPlayer->m_pTeam->GetGoalie();
    FuzzyTweaks* pFuzzyTweaks = g_pGame->m_pFuzzyTweaks;
    float fMaxDist = pFuzzyTweaks->mUnidentified2B4;
    float fMinDist = pFuzzyTweaks->mUnidentified2A4;
    float fDist = nlSqrt(nlVec3DistanceSquared2D(pPlayer->mUnidentified024.m_v3Position,
        pGoalie->mUnidentified024.m_v3Position), true);
    return NormalizeVal(fDist, fMinDist, fMaxDist);
}

float NearToMyGoalie(cPlayer* pPlayer)
{
    cPlayer* pGoalie = pPlayer->m_pTeam->GetGoalie();
    FuzzyTweaks* pFuzzyTweaks = g_pGame->m_pFuzzyTweaks;
    float fMaxDist = pFuzzyTweaks->mUnidentified2D4;
    float fMinDist = pFuzzyTweaks->mUnidentified2C4;
    float fDist = nlSqrt(nlVec3DistanceSquared2D(pPlayer->mUnidentified024.m_v3Position,
        pGoalie->mUnidentified024.m_v3Position), true);
    return NormalizeVal(fDist, fMinDist, fMaxDist);
}

float FarToMyGoalie(cPlayer* pPlayer)
{
    cPlayer* pGoalie = pPlayer->m_pTeam->GetGoalie();
    FuzzyTweaks* pFuzzyTweaks = g_pGame->m_pFuzzyTweaks;
    float fMaxDist = pFuzzyTweaks->mUnidentified2F4;
    float fMinDist = pFuzzyTweaks->mUnidentified2E4;
    float fDist = nlSqrt(nlVec3DistanceSquared2D(pPlayer->mUnidentified024.m_v3Position,
        pGoalie->mUnidentified024.m_v3Position), true);
    return NormalizeVal(fDist, fMinDist, fMaxDist);
}

float CloseToTheirGoalie(cPlayer* pPlayer)
{
    cPlayer* pGoalie = pPlayer->m_pTeam->GetOtherTeam()->GetGoalie();
    FuzzyTweaks* pFuzzyTweaks = g_pGame->m_pFuzzyTweaks;
    float fMaxDist = pFuzzyTweaks->mUnidentified2B4;
    float fMinDist = pFuzzyTweaks->mUnidentified2A4;
    float fDist = nlSqrt(nlVec3DistanceSquared2D(pPlayer->mUnidentified024.m_v3Position,
        pGoalie->mUnidentified024.m_v3Position), true);
    return NormalizeVal(fDist, fMinDist, fMaxDist);
}

float NearToTheirGoalie(cPlayer* pPlayer)
{
    cPlayer* pGoalie = pPlayer->m_pTeam->GetOtherTeam()->GetGoalie();
    FuzzyTweaks* pFuzzyTweaks = g_pGame->m_pFuzzyTweaks;
    float fMaxDist = pFuzzyTweaks->mUnidentified2D4;
    float fMinDist = pFuzzyTweaks->mUnidentified2C4;
    float fDist = nlSqrt(nlVec3DistanceSquared2D(pPlayer->mUnidentified024.m_v3Position,
        pGoalie->mUnidentified024.m_v3Position), true);
    return NormalizeVal(fDist, fMinDist, fMaxDist);
}

float FarToTheirGoalie(cPlayer* pPlayer)
{
    cPlayer* pGoalie = pPlayer->m_pTeam->GetOtherTeam()->GetGoalie();
    FuzzyTweaks* pFuzzyTweaks = g_pGame->m_pFuzzyTweaks;
    float fMaxDist = pFuzzyTweaks->mUnidentified2F4;
    float fMinDist = pFuzzyTweaks->mUnidentified2E4;
    float fDist = nlSqrt(nlVec3DistanceSquared2D(pPlayer->mUnidentified024.m_v3Position,
        pGoalie->mUnidentified024.m_v3Position), true);
    return NormalizeVal(fDist, fMinDist, fMaxDist);
}

float CloseToSideline(const nlVector3& v3Position, const nlVector2* vDistanceConfidence,
    bool bInvert, nlVector2* pNormal)
{
    bool bDefaultDistanceConfidence = false;
    if (vDistanceConfidence == NULL)
        bDefaultDistanceConfidence = true;
    float fScore = bInvert ? 1.0f : 0.0f;
    for (int i = 0; i < 4; i++)
    {
        const sSideLinePlane& sideline = cField::GetSideline(i);
        nlVector3 v3SidelinePosition = v3Position;
        v3SidelinePosition.z = 0.0f;
        if (sideline.vNormal.x == 0.0f)
        {
            v3SidelinePosition.y = sideline.fDistance * sideline.vNormal.y;
        }
        else
        {
            v3SidelinePosition.x = sideline.fDistance * sideline.vNormal.x;
        }

        float fSidelineScore;
        if (bDefaultDistanceConfidence)
        {
            FuzzyTweaks* pTweaks = g_pGame->m_pFuzzyTweaks;
            float fMax = pTweaks->mUnidentified314;
            float fMin = pTweaks->mUnidentified304;
            fSidelineScore = NormalizeVal(nlSqrt(nlVec3DistanceSquared2D(v3SidelinePosition,
                v3Position), true), fMin, fMax);
        }
        else
        {
            fSidelineScore = NormalizeVal(nlSqrt(nlVec3DistanceSquared2D(v3SidelinePosition,
                v3Position), true), *vDistanceConfidence);
        }
        if (bInvert)
        {
            if (fSidelineScore < fScore)
            {
                fScore = fSidelineScore;
                if (pNormal != NULL)
                    *pNormal = sideline.vNormal;
            }
        }
        else if (fSidelineScore > fScore)
        {
            fScore = fSidelineScore;
            if (pNormal != NULL)
                *pNormal = sideline.vNormal;
        }
    }
    return fScore;
}

float NearToSideline(const nlVector3& v3Position)
{
    nlVector2 vDistanceConfidence;
    nlVec2Set(vDistanceConfidence,
        g_pGame->m_pFuzzyTweaks->mUnidentified324,
        g_pGame->m_pFuzzyTweaks->mUnidentified334);
    return CloseToSideline(v3Position, &vDistanceConfidence, false, NULL);
}

float CloseToSideline(cFielder* pFielder)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }

    return CloseToSideline(pFielder->mUnidentified024.m_v3Position, NULL, false, NULL);
}

extern "C" float fn_800DD234(cFielder* pFielder)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }

    float fMax = g_pGame->m_pFuzzyTweaks->mUnidentified334;
    float fMin = g_pGame->m_pFuzzyTweaks->mUnidentified324;
    nlVector2 v2Range;
    v2Range.x = fMin;
    v2Range.y = fMax;
    return CloseToSideline(pFielder->mUnidentified024.m_v3Position, &v2Range, false, NULL);
}

extern "C" float fn_800DD294(cFielder* pFielder)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }

    float fMax = g_pGame->m_pFuzzyTweaks->mUnidentified354;
    float fMin = g_pGame->m_pFuzzyTweaks->mUnidentified344;
    nlVector2 v2Range;
    v2Range.x = fMin;
    v2Range.y = fMax;
    return CloseToSideline(pFielder->mUnidentified024.m_v3Position, &v2Range, true, NULL);
}

extern "C" float fn_800DD2F4(cBall* ball)
{
    if (ball == NULL)
    {
        return 0.0f;
    }

    return CloseToSideline(ball->m_v3Position, NULL, false, NULL);
}

extern "C" float fn_800DD31C(cFielder* pFielder)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }

    float fMax = g_pGame->m_pFuzzyTweaks->mUnidentified334;
    float fMin = g_pGame->m_pFuzzyTweaks->mUnidentified324;
    nlVector2 v2Range;
    v2Range.x = fMin;
    v2Range.y = fMax;
    return CloseToSideline(pFielder->mUnidentified024.m_v3PrevVelocity, &v2Range, false, NULL);
}

extern "C" float fn_800DD37C(cFielder* pFielder)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }

    float fMax = g_pGame->m_pFuzzyTweaks->mUnidentified354;
    float fMin = g_pGame->m_pFuzzyTweaks->mUnidentified344;
    nlVector2 v2Range;
    v2Range.x = fMin;
    v2Range.y = fMax;
    return CloseToSideline(pFielder->mUnidentified024.m_v3PrevVelocity, &v2Range, true, NULL);
}

extern "C" float fn_800DD3DC(const nlVector3& vFrom, const nlVector3& vTo,
    float fMin, float fMax)
{
    nlVector2 diff;
    diff.x = vFrom.x - vTo.x;
    diff.y = vFrom.y - vTo.y;
    return NormalizeVal(nlSqrt(diff.x * diff.x + diff.y * diff.y, true), fMin, fMax);
}

static const nlVector2 lbl_806E4270 = { 2.5f, 0.4f };


extern "C" float fn_800DD45C(cFielder* pFielder)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }

    return NormalizeVal(fn_8004028C(pFielder), lbl_806E4270);
}

static const nlVector2 lbl_806E4278 = { 10.0f, 1.0f };


extern "C" float fn_800DD494(cFielder* pFielder)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }

    return NormalizeVal(fn_8004028C(pFielder), lbl_806E4278);
}

static const nlVector2 lbl_806E4280 = { 4.0f, 10.0f };


extern "C" float fn_800DD4CC(cFielder* pFielder)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }

    return NormalizeVal(fn_8004028C(pFielder), lbl_806E4280);
}

extern "C" float fn_800DD504(cPlayer* pPlayer, cFielder* pFielder)
{
    if (pPlayer == NULL)
    {
        return 0.0f;
    }
    if (pFielder == NULL)
    {
        return 0.0f;
    }
    const nlVector3& vPosition = fn_80040234(pFielder);
    nlVector2 diff;
    diff.x = pPlayer->mUnidentified024.m_v3Position.x - vPosition.x;
    diff.y = pPlayer->mUnidentified024.m_v3Position.y - vPosition.y;
    float fDistance = nlSqrt(diff.x * diff.x + diff.y * diff.y, true);
    return NormalizeVal(fDistance - pFielder->mUnidentified320->GetRadius(), lbl_806E4270);
}

extern "C" float fn_800DD5C4(cPlayer* pPlayer, cFielder* pFielder)
{
    if (pPlayer == NULL)
    {
        return 0.0f;
    }
    if (pFielder == NULL)
    {
        return 0.0f;
    }
    const nlVector3& vPosition = fn_80040234(pFielder);
    nlVector2 diff;
    diff.x = pPlayer->mUnidentified024.m_v3Position.x - vPosition.x;
    diff.y = pPlayer->mUnidentified024.m_v3Position.y - vPosition.y;
    float fDistance = nlSqrt(diff.x * diff.x + diff.y * diff.y, true);
    return NormalizeVal(fDistance - pFielder->mUnidentified320->GetRadius(), lbl_806E4278);
}

extern "C" float fn_800DD684(cPlayer* pPlayer, cFielder* pFielder)
{
    if (pPlayer == NULL)
    {
        return 0.0f;
    }
    if (pFielder == NULL)
    {
        return 0.0f;
    }
    const nlVector3& vPosition = fn_80040234(pFielder);
    nlVector2 diff;
    diff.x = pPlayer->mUnidentified024.m_v3Position.x - vPosition.x;
    diff.y = pPlayer->mUnidentified024.m_v3Position.y - vPosition.y;
    float fDistance = nlSqrt(diff.x * diff.x + diff.y * diff.y, true);
    return NormalizeVal(fDistance - pFielder->mUnidentified320->GetRadius(), lbl_806E4280);
}

extern "C" float fn_800DD744(cFielder* pFielder)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }
    float fScore = 0.0f;
    nlVector2 normal;
    if (CloseToSideline(pFielder->mUnidentified024.m_v3Position, NULL, false, &normal))
    {
        nlVector2 facing;
        nlSinCos(&facing.y, &facing.x, pFielder->mUnidentified024.m_aActualFacingDirection);
        fScore = FMAX(0.0f, normal.x * facing.x + normal.y * facing.y);
    }
    return fScore;
}

extern "C" float fn_800DD7F4(cFielder* pFielder)
{
    if (pFielder == NULL)
        return 0.0f;
    return FMIN(fn_800D9BDC(pFielder), FMAX(fn_800D9B74(pFielder), fn_800D9B0C(pFielder)));
}

extern "C" float fn_800DD944(cPlayer* pPlayer)
{
    if (pPlayer == NULL)
    {
        return 0.0f;
    }

    if (pPlayer->m_eClassType == FIELDER)
    {
        if (((cFielder*)pPlayer)->fn_8003E6EC())
        {
            return 1.0f;
        }
        return 0.0f;
    }
    return 0.0f;
}

extern "C" float fn_800DD99C(cFielder* pFielder)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }

    if (pFielder->m_eActionState == 0x1D)
    {
        return 1.0f;
    }

    return 0.0f;
}

extern "C" float fn_800DD9C8(cFielder* pFielder, cPlayer* pTarget)
{
    if (pFielder == NULL)
        return 0.0f;
    if (pTarget == NULL)
        return 0.0f;
    bool bHasGlobalPad = pFielder->GetGlobalPad() != NULL;
    if (!bHasGlobalPad && pTarget->m_eClassType == FIELDER
        && ((cFielder*)pTarget)->fn_800345EC(pFielder))
        return 0.0f;
    float fDistance = nlSqrt(nlVec3DistanceSquared2D(pFielder->mUnidentified024.m_v3Position,
        pTarget->mUnidentified024.m_v3Position), true);
    float fRange = fn_80030750(pFielder);
    return NormalizeVal(fDistance, 0.5f + fRange, 0.66f * fRange);
}

extern "C" float fn_800DDAC0(const nlVector3& vPosition, const nlVector3& vOffNetPosition,
    float fShooting)
{
    float fMin = InterpolateClamped(g_pGame->m_pFuzzyTweaks->mUnidentified514,
        g_pGame->m_pFuzzyTweaks->mUnidentified534, fShooting);
    float fMax = InterpolateClamped(g_pGame->m_pFuzzyTweaks->mUnidentified524,
        g_pGame->m_pFuzzyTweaks->mUnidentified544, fShooting);
    return NormalizeVal(nlSqrt(nlVec3DistanceSquared2D(vPosition, vOffNetPosition), true), fMin, fMax);
}

float PlayerShotDistance(cFielder* pFielder)
{
    if (pFielder == NULL)
        return 0.0f;
    nlVector3 v3Position = pFielder->mUnidentified024.m_v3Position;
    if (pFielder->m_pBall != NULL)
        v3Position = pFielder->mUnidentified024.m_v3Position;
    else if (ReceivingPass(pFielder) || fn_800DED80(pFielder) >= 0.2f)
        v3Position = pFielder->m_pTeam->GetBallInterceptPosition(pFielder->mUnidentified1E4.m_ID);
    float fScore = 0.0f;
    if (v3Position.x * pFielder->m_pTeam->GetOtherNet()->m_fDirection > 0.0f)
    {
        float fShooting = pFielder->GetTweaks()->fShooting;
        const nlVector3& netLocation = pFielder->GetAIOffNetLocation(&v3Position);
        fScore = fn_800DDAC0(v3Position, netLocation, fShooting);
    }
    return fScore;
}

extern "C" float fn_800DDD70(cFielder* pFielder)
{
    if (pFielder == NULL)
        return 0.0f;
    nlVector2 aiPos = *(const nlVector2*)&pFielder->mUnidentified024.m_v3Position;
    if (pFielder->m_pTeam->m_nSide == AWAY)
        nlVec2Scale(aiPos, aiPos, -1.0f);
    float fScore = 0.0f;
    if (!pFielder->fn_8001E168() && aiPos.x > 0.0f)
    {
        Goalie* pGoalie = fn_800D66C4(pFielder);
        nlVector2 diff;
        nlVec2Sub(diff, *(const nlVector2*)&pGoalie->GetPosition(),
            *(const nlVector2*)&pFielder->GetPosition());
        float fDistance = nlSqrt(diff.x * diff.x + diff.y * diff.y, true);
        switch (pFielder->mUnidentified024.m_eCharacterClass)
        {
        case 14:
        case 15:
        case 17:
        case 19:
            fScore = 1.0f;
            break;
        case 12:
            if (aiPos.x > 0.0f)
                fScore = fn_800DAFCC(pFielder->mUnidentified024.m_v3Position,
                    pFielder->m_pTeam->GetOtherNet()->m_v3NetLocation, pFielder, NULL,
                    1.0f, 1.0f, 0.0f, 0.0f);
            break;
        case 13:
            if (fDistance > 12.5f)
                fScore = NormalizeVal(fDistance, 15.0f, 12.5f);
            else
                fScore = NormalizeVal(fDistance, 7.0f, 12.5f);
            break;
        case 16:
            fScore = NormalizeVal(fDistance, 7.0f, 4.5f);
            break;
        case 18:
            fScore = fDistance > 12.0f ? NormalizeVal(fDistance, 18.0f, 12.0f)
                                     : NormalizeVal(fDistance, 7.0f, 12.0f);
            break;
        }
    }
    return fScore;
}

static float Facing(unsigned short facingAngle, const nlVector3& direction)
{
    nlPolar p;
    nlCartesianToPolar(p, direction);
    s16 nFacingDelta = nlAngleDiff(facingAngle, p.a);
    nFacingDelta = (u16)(nFacingDelta < 0 ? -nFacingDelta : nFacingDelta);
    int nFullConfidence = g_pGame->m_pFuzzyTweaks->nFacingFullConfidenceAngle;
    if (nFacingDelta < nFullConfidence)
        return 1.0f;
    if (nFacingDelta > g_pGame->m_pFuzzyTweaks->nFacingNoConfidenceAngle)
        return 0.0f;
    float fRange = (float)(g_pGame->m_pFuzzyTweaks->nFacingNoConfidenceAngle
        - nFullConfidence);
    return 1.0f - (float)(nFacingDelta - nFullConfidence) / fRange;
}

extern "C" float fn_800DDF54(cPlayer* pCandidateFielder, cPlayer* pTargetFielder)
{
    if (pCandidateFielder == NULL)
        return 0.0f;
    if (pTargetFielder == NULL)
        return 0.0f;
    nlVector3 v3Direction;
    nlVec3Sub(v3Direction, pTargetFielder->GetPosition(),
        pCandidateFielder->GetPosition());
    unsigned short facingAngle = pCandidateFielder->mUnidentified024.m_aActualFacingDirection;
    if (pCandidateFielder->m_pController != NULL
        && pCandidateFielder->m_pController->GetMovementStickMagnitude() > 0.001f)
        facingAngle = pCandidateFielder->m_pController->GetMovementStickDirection();
    return Facing(facingAngle, v3Direction);
}

extern "C" float fn_800DE0A8(cPlayer* pPlayer)
{
    if (pPlayer == NULL)
        return 0.0f;
    nlVector3 v3Direction;
    nlVec3Sub(v3Direction, g_pBall->GetPosition(), pPlayer->GetPosition());
    unsigned short facingAngle = pPlayer->mUnidentified024.m_aActualFacingDirection;
    if (pPlayer->m_pController != NULL && pPlayer->m_pController->GetMovementStickMagnitude() > 0.001f)
        facingAngle = pPlayer->m_pController->GetMovementStickDirection();
    return Facing(facingAngle, v3Direction);
}

extern "C" float fn_800DE1F0(cPlayer* pCandidateFielder, cPlayer* pTargetFielder)
{
    if (pCandidateFielder == NULL)
        return 0.0f;
    if (pTargetFielder == NULL)
        return 0.0f;
    nlVector3 v3Direction;
    nlVec3Sub(v3Direction, pTargetFielder->GetPosition(),
        pCandidateFielder->GetPosition());
    float fScore = Facing(pCandidateFielder->mUnidentified024.m_aActualFacingDirection, v3Direction);
    nlVec3Sub(v3Direction, pCandidateFielder->GetPosition(),
        pTargetFielder->GetPosition());
    fScore += Facing(pTargetFielder->mUnidentified024.m_aActualFacingDirection, v3Direction);
    fScore *= 0.5f;
    return fScore;
}

extern "C" float fn_800DE40C(cPlayer* pUpfieldPlayer, cPlayer* pFromPlayer)
{
    if (pUpfieldPlayer == NULL)
    {
        return 0.0f;
    }
    if (pFromPlayer == NULL)
    {
        return 0.0f;
    }
    nlVector3 vUpfieldPos = pUpfieldPlayer->mUnidentified024.m_v3Position;
    nlVector3 vFromPos = pFromPlayer->mUnidentified024.m_v3Position;
    float fDelta = (vUpfieldPos.x - vFromPos.x)
        * AIsgn(pUpfieldPlayer->m_pTeam->GetOtherNet()->m_v3NetLocation.x);
    return NormalizeVal(fDelta, 0.0f,
        g_pGame->m_pFuzzyTweaks->mUnidentified594);
}

extern "C" float fn_800DE4B0(cPlayer* pDownfieldPlayer, cPlayer* pFromPlayer)
{
    if (pDownfieldPlayer == NULL)
    {
        return 0.0f;
    }
    if (pFromPlayer == NULL)
    {
        return 0.0f;
    }
    nlVector3 vDownfieldPos = pDownfieldPlayer->mUnidentified024.m_v3Position;
    nlVector3 vFromPos = pFromPlayer->mUnidentified024.m_v3Position;
    float fDelta = (vFromPos.x - vDownfieldPos.x)
        * AIsgn(pDownfieldPlayer->m_pTeam->GetOtherNet()->m_v3NetLocation.x);
    return NormalizeVal(fDelta, 0.0f,
        g_pGame->m_pFuzzyTweaks->mUnidentified5A4);
}

float ClosingTo(cPlayer* pFielder1, cPlayer* pFielder2)
{
    if (pFielder1 == NULL)
    {
        return 0.0f;
    }

    if (pFielder2 == NULL)
    {
        return 0.0f;
    }

    float fClosingSpeed = GetClosingSpeed2D(
        pFielder1->GetPosition(),
        pFielder1->GetVelocity(),
        pFielder2->GetPosition(),
        pFielder2->GetVelocity());
    return NormalizeVal(fClosingSpeed, 0.0f, g_pGame->m_pFuzzyTweaks->mUnidentified5B4);
}

float ClosingTo(cPlayer* pPlayer, cBall* pBall)
{
    if (pPlayer == NULL)
    {
        return 0.0f;
    }

    if (pBall == NULL)
    {
        return 0.0f;
    }

    float fClosingSpeed = GetClosingSpeed2D(
        pPlayer->GetPosition(),
        pPlayer->GetVelocity(),
        pBall->m_v3Position,
        pBall->m_v3Velocity);
    return NormalizeVal(fClosingSpeed, 0.0f, g_pGame->m_pFuzzyTweaks->mUnidentified5B4);
}

float SeparatingFrom(cPlayer* pFielder1, cPlayer* pFielder2)
{
    if (pFielder1 == NULL)
    {
        return 0.0f;
    }

    if (pFielder2 == NULL)
    {
        return 0.0f;
    }

    float fClosingSpeed = GetClosingSpeed2D(
        pFielder1->mUnidentified024.m_v3Position,
        pFielder1->mUnidentified024.m_v3Velocity,
        pFielder2->mUnidentified024.m_v3Position,
        pFielder2->mUnidentified024.m_v3Velocity);
    return NormalizeVal(fClosingSpeed, 0.0f, -g_pGame->m_pFuzzyTweaks->mUnidentified5C4);
}

float SeparatingFrom(cPlayer* pPlayer, cBall* pBall)
{
    if (pPlayer == NULL)
    {
        return 0.0f;
    }

    if (pBall == NULL)
    {
        return 0.0f;
    }

    float fClosingSpeed = GetClosingSpeed2D(
        pPlayer->mUnidentified024.m_v3Position,
        pPlayer->mUnidentified024.m_v3Velocity,
        pBall->m_v3Position,
        pBall->m_v3Velocity);
    return NormalizeVal(fClosingSpeed, 0.0f, -g_pGame->m_pFuzzyTweaks->mUnidentified5C4);
}

extern "C" float fn_800DE71C(cPlayer* pPlayer)
{
    if (pPlayer == NULL)
    {
        return 0.0f;
    }

    FuzzyTweaks* pFuzzyTweaks = g_pGame->m_pFuzzyTweaks;
    float fMaxDist = pFuzzyTweaks->mUnidentified584;
    float fMinDist = pFuzzyTweaks->mUnidentified574;
    const nlVector3& netLocation = pPlayer->GetAIDefNetLocation(NULL);
    nlVector2 diff;
    diff.x = netLocation.x - pPlayer->mUnidentified024.m_v3Position.x;
    diff.y = netLocation.y - pPlayer->mUnidentified024.m_v3Position.y;
    return NormalizeVal(nlSqrt(diff.x * diff.x + diff.y * diff.y, true),
        fMinDist, fMaxDist);
}

extern "C" float fn_800DE7D8(Goalie* pGoalie)
{
    if (pGoalie == NULL)
    {
        return 0.0f;
    }

    if (pGoalie->mbIsDown)
    {
        return 1.0f;
    }

    return 0.0f;
}

extern "C" float fn_800DE804(cBall* ball, cTeam* team)
{
    if (ball == NULL)
    {
        return 0.0f;
    }
    if (team == NULL)
    {
        return 0.0f;
    }

    FuzzyTweaks* pFuzzyTweaks = g_pGame->m_pFuzzyTweaks;
    float fMaxDist = pFuzzyTweaks->mUnidentified1F4;
    float fMinDist = pFuzzyTweaks->mUnidentified1E4;
    const nlVector3& netLocation = team->GetAIDefNetLocation(&ball->m_v3Position);
    nlVector2 diff;
    diff.x = ball->m_v3Position.x - netLocation.x;
    diff.y = ball->m_v3Position.y - netLocation.y;
    return NormalizeVal(nlSqrt(diff.x * diff.x + diff.y * diff.y, true),
        fMinDist, fMaxDist);
}

extern "C" float fn_800DE8CC(cBall* ball, cTeam* team)
{
    if (ball == NULL)
    {
        return 0.0f;
    }
    if (team == NULL)
    {
        return 0.0f;
    }

    FuzzyTweaks* pFuzzyTweaks = g_pGame->m_pFuzzyTweaks;
    float fMaxDist = pFuzzyTweaks->mUnidentified214;
    float fMinDist = pFuzzyTweaks->mUnidentified204;
    const nlVector3& netLocation = team->GetAIDefNetLocation(&ball->m_v3Position);
    nlVector2 diff;
    diff.x = ball->m_v3Position.x - netLocation.x;
    diff.y = ball->m_v3Position.y - netLocation.y;
    return NormalizeVal(nlSqrt(diff.x * diff.x + diff.y * diff.y, true),
        fMinDist, fMaxDist);
}

extern "C" float fn_800DE994(cBall* ball, cTeam* team)
{
    if (ball == NULL)
    {
        return 0.0f;
    }
    if (team == NULL)
    {
        return 0.0f;
    }

    FuzzyTweaks* pFuzzyTweaks = g_pGame->m_pFuzzyTweaks;
    float fMaxDist = pFuzzyTweaks->mUnidentified234;
    float fMinDist = pFuzzyTweaks->mUnidentified224;
    const nlVector3& netLocation = team->GetAIDefNetLocation(&ball->m_v3Position);
    nlVector2 diff;
    diff.x = ball->m_v3Position.x - netLocation.x;
    diff.y = ball->m_v3Position.y - netLocation.y;
    return NormalizeVal(nlSqrt(diff.x * diff.x + diff.y * diff.y, true),
        fMinDist, fMaxDist);
}

float InControlOfBall(cFielder* fielder)
{
    if (fielder == NULL)
    {
        return 0.0f;
    }

    if (fielder != g_pBall->m_pOwner)
    {
        return 0.0f;
    }

    return NormalizeVal(g_pGame->m_fCachedBallPlayerDistances[fielder->mUnidentified120],
        g_pGame->m_pFuzzyTweaks->mUnidentified3C4,
        g_pGame->m_pFuzzyTweaks->mUnidentified3D4);
}

extern "C" float fn_800DEAB4(cFielder* pFielder)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }

    if (pFielder->m_eClassType == FIELDER)
    {
        if (pFielder->GetActionState() == ACTION_UNKNOWN_15
            || pFielder->GetActionState() == ACTION_UNKNOWN_30
            || pFielder->GetActionState() == ACTION_SHOOT_TO_SCORE
            || pFielder->GetActionState() == ACTION_SHOT)
        {
            return 1.0f;
        }
    }

    return 0.0f;
}

extern "C" float fn_800DEB04(cFielder* pFielder)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }

    ShotMeter* pMeter = pFielder->m_pShotMeter;
    bool bActive = pMeter->UnidentifiedIsCharging();
    if (bActive)
    {
        switch (pMeter->m_eShotMeterState)
        {
        case SHOT_METER_INACTIVE:
            return 0.0f;
        case SHOT_METER_ACTIVE:
        case SHOT_METER_STS_ACTIVE:
            return FMIN(FMAX(pMeter->GetTime() / pMeter->GetTotalDuration(), 0.0f), 1.0f);
        default:
            return 1.0f;
        }
    }
    return 0.0f;
}

extern "C" float fn_800DEBBC(cPlayer* pPlayer)
{
    if (pPlayer == NULL)
    {
        return 0.0f;
    }

    if (pPlayer->m_eClassType == FIELDER && ((cFielder*)pPlayer)->m_eActionState == ACTION_SHOOT_TO_SCORE)
    {
        return 1.0f;
    }

    return 0.0f;
}

extern "C" float fn_800DEBF4(cFielder* pFielder)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }

    ShotMeter* pMeter = pFielder->m_pShotMeter;
    bool bActive = pMeter->UnidentifiedIsCharging();
    if (bActive)
    {
        switch (pMeter->m_eShotMeterState)
        {
        case SHOT_METER_STS_ACTIVE:
            return 1.0f;
        default:
            return FMIN(FMAX(pMeter->GetTime() / pMeter->UnidentifiedGetShotDuration(), 0.0f), 1.0f);
        }
    }
    return 0.0f;
}

extern "C" float fn_800DEC88(cFielder* pFielder)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }

    ShotMeter* pMeter = pFielder->m_pShotMeter;
    bool bActive = pMeter->UnidentifiedIsCharging();
    if (bActive)
    {
        switch (pMeter->m_eShotMeterState)
        {
        case SHOT_METER_STS_ACTIVE:
            return FMIN(FMAX((pMeter->GetTime() - pMeter->UnidentifiedGetShotDuration())
                                / (pMeter->GetTotalDuration() - pMeter->UnidentifiedGetShotDuration()),
                            0.0f),
                1.0f);
        case SHOT_METER_STS_TRANSISTION:
        case SHOT_METER_STS_RELEASED:
            return 1.0f;
        default:
            return 0.0f;
        }
    }
    return 0.0f;
}

extern "C" float fn_800DED3C(cFielder* pFielder)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }

    if (pFielder->fn_8003E71C())
    {
        return 1.0f;
    }

    return 0.0f;
}

extern "C" float fn_800DED80(cPlayer* pPlayer)
{
    if (pPlayer == NULL)
        return 0.0f;
    float fScore = 0.0f;
    if (pPlayer->m_eClassType == GOALIE)
    {
        float fClose = CloseToBall(pPlayer);
        float fClosing = ClosingTo(pPlayer, g_pBall);
        float fOwner = BallOwner(pPlayer);
        fScore = FMIN(fClose, FMIN(AbleToInterceptBall(pPlayer), FMAX(fClosing, fOwner)));
    }
    else if (pPlayer->m_eClassType == FIELDER)
    {
        cFielder* pFielder = (cFielder*)pPlayer;
        DesireRunToTarget* pDesire = pFielder->fn_8002E060() == 13
            ? (DesireRunToTarget*)fn_8002E08C(pFielder, 13) : NULL;
        if ((pDesire != NULL && pDesire->GetTargetBall() != NULL)
            || pFielder->fn_8002E060() == 7 || pFielder->fn_8002E060() == 16
            || pFielder->m_eActionState == ACTION_SLIDE_ATTACK)
            fScore = AbleToInterceptBall(pFielder);
        else if (pFielder->fn_8002E060() == 20)
            fScore = FMIN(FMAX(1.5f * AbleToInterceptBall(pFielder), 0.0f), 1.0f);
    }
    return fScore;
}

float DoingS2S(cFielder* pFielder)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }

    if (pFielder->m_eActionState == ACTION_SHOOT_TO_SCORE)
    {
        return 1.0f;
    }

    return 0.0f;
}

float ReceivingPass(cFielder* pFielder)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }

    float fScore = 0.0f;
    if (pFielder->fn_8002E060() == 22)
    {
        fScore = 1.0f;
    }

    return fScore;
}

extern "C" float fn_800DF028(cFielder* pFielder)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }

    float fScore = 0.0f;
    DesireReceivePass* pDesire = (DesireReceivePass*)fn_8002E08C(pFielder, 22);
    if (pFielder->m_eActionState == ACTION_ONETIMER
        || pFielder->m_eActionState == ACTION_LATE_ONETIMER_FROM_VOLLEY
        || (pDesire != NULL && pDesire->UnidentifiedIsActive() && pDesire->IsOneTouchShot()))
    {
        fScore = 1.0f;
    }
    return fScore;
}

extern "C" float fn_800DF0B8(cFielder* pFielder)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }

    float fResult = 0.0f;
    if ((pFielder->m_eClassType == FIELDER) && pFielder->fn_8003499C())
    {
        fResult = 1.0f;
    }

    return fResult;
}

extern "C" float fn_800DF118(cFielder* pFielder)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }

    float fScore = 0.0f;
    if (ReceivingPass(pFielder) && !pFielder->fn_8003499C())
    {
        fScore = 1.0f;
    }
    return fScore;
}

extern "C" float fn_800DF1B8(cFielder* pFielder)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }

    float fScore = 0.0f;
    if (ReceivingPass(pFielder))
    {
        float fReaction = 1.0f - fn_800A636C(g_pCurrentlyUpdatingTeam)->GetReaction(NULL);
        float fPassDeadZone = g_pGame->m_pFuzzyTweaks->mUnidentified6C4;
        float fDeadZone = fReaction * fPassDeadZone;
        float fMaxValue = FMAX(0.1f, fDeadZone);
        fScore = NormalizeVal(IsPassInPlay(g_pBall), 0.0f, fMaxValue);
    }
    return fScore;
}

static const nlVector2 g_vPassCloseToDoneConfidence = { 0.0f, 0.5f };


extern "C" float fn_800DF2C0(cFielder* pFielder)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }

    float fScore = 0.0f;
    if (pFielder->m_pBall != NULL)
    {
        fScore = 1.0f;
    }
    else if (ReceivingPass(pFielder))
    {
        fScore = NormalizeVal(IsPassInPlay(g_pBall), g_vPassCloseToDoneConfidence);
    }
    return fScore;
}

extern "C" float fn_800DF390(cPlayer* pPlayer)
{
    if (pPlayer == NULL)
    {
        return 0.0f;
    }

    float fScore = 0.0f;
    if (pPlayer->m_eClassType == FIELDER && ((cFielder*)pPlayer)->fn_8003499C())
    {
        float fReaction = 1.0f - fn_800A636C(g_pCurrentlyUpdatingTeam)->GetReaction(NULL);
        float fPassDeadZone = g_pGame->m_pFuzzyTweaks->mUnidentified6C4;
        float fDeadZone = fReaction * fPassDeadZone;
        float fMaxValue = FMAX(0.1f, fDeadZone);
        fScore = NormalizeVal(IsPassInPlay(g_pBall), 0.0f, fMaxValue);
    }
    return fScore;
}

extern "C" float fn_800DF474(cFielder* pFielder)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }

    float fScore = 0.0f;
    if (ReceivingPass(pFielder) && !pFielder->fn_8003499C())
    {
        float fReaction = 1.0f - fn_800A636C(g_pCurrentlyUpdatingTeam)->GetReaction(NULL);
        float fPassDeadZone = g_pGame->m_pFuzzyTweaks->mUnidentified6C4;
        float fDeadZone = fReaction * fPassDeadZone;
        float fMaxValue = FMAX(0.1f, fDeadZone);
        fScore = NormalizeVal(IsPassInPlay(g_pBall), 0.0f, fMaxValue);
    }
    return fScore;
}

extern "C" float fn_800DF590(cBall* pBall)
{
    if (pBall == NULL)
    {
        return 0.0f;
    }

    float fScore = fn_800156A8(pBall);
    if (fScore < 0.0f || fScore > 1.0f)
    {
        fScore = FMIN(FMAX(fScore, 0.0f), 1.0f);
        nlPrintf("Ball charge bonus value is out of range: %f.\n", fScore);
    }
    return fScore;
}

float High(cBall* ball)
{
    if (ball == NULL)
    {
        return 0.0f;
    }

    return NormalizeVal(ball->m_v3Position.z,
        g_pGame->m_pFuzzyTweaks->mUnidentified364,
        g_pGame->m_pFuzzyTweaks->mUnidentified374);
}

float ReallyHigh(cBall* ball)
{
    if (ball == NULL)
    {
        return 0.0f;
    }

    return NormalizeVal(ball->m_v3Position.z,
        g_pGame->m_pFuzzyTweaks->mUnidentified384,
        g_pGame->m_pFuzzyTweaks->mUnidentified394);
}

float Ownerless(cBall* ball)
{
    if (ball == NULL)
    {
        return 0.0f;
    }

    if (ball->m_pOwner == NULL)
    {
        return 1.0f;
    }

    return 0.0f;
}

float AggressiveT(cTeam* team)
{
    if (team == NULL)
    {
        return 0.0f;
    }

    if (team->meCurrentTeamStyle == TEAM_STYLE_AGGRESSIVE)
    {
        return 1.0f;
    }

    return 0.0f;
}

float Moderate(cTeam* team)
{
    if (team == NULL)
    {
        return 0.0f;
    }

    if (team->meCurrentTeamStyle == TEAM_STYLE_MODERATE)
    {
        return 1.0f;
    }

    return 0.0f;
}

float Passive(cTeam* team)
{
    if (team == NULL)
    {
        return 0.0f;
    }

    if (team->meCurrentTeamStyle == TEAM_STYLE_PASSIVE)
    {
        return 1.0f;
    }

    return 0.0f;
}

float UserControlledT(cTeam* team)
{
    if (team == NULL)
    {
        return 0.0f;
    }

    if (team->GetNumAssignedControllers() > 0)
    {
        return 1.0f;
    }

    return 0.0f;
}

extern "C" cFielder* fn_800DF790(cTeam* pTeam)
{
    float fBestScore = 0.0f;
    cFielder* pBestFielder = NULL;
    for (int i = 0; i < 4; i++)
    {
        float fScore = StrategicBallOwner(pTeam->GetFielder(i));
        if (fScore >= fBestScore)
        {
            pBestFielder = pTeam->GetFielder(i);
            fBestScore = fScore;
        }
    }
    return pBestFielder;
}

extern "C" float fn_800DF838(cPlayer* pPlayer)
{
    if (pPlayer == NULL)
    {
        return 0.0f;
    }

    float fScore = 0.0f;
    if (pPlayer->m_pBall != NULL)
    {
        fScore = NormalizeVal(pPlayer->mUnidentified1E4.m_tBallPossessionTimer.GetSeconds(), 1.0f, 5.0f);
    }
    return fScore;
}

extern "C" float fn_800DF888(cTeam* team)
{
    if (team == NULL)
        return 0.0f;
    cFielder* players[2];
    players[0] = team->mpBestBallInterceptor;
    players[1] = team->GetOtherTeam()->mpBestBallInterceptor;
    float score[2];
    {
        float fOwner = BallOwner(players[0]);
        float fReceiving = ReceivingPass(players[0]);
        float fClosing = ClosingTo(players[0], g_pBall);
        float fNear = NearToBall(players[0]);
        float fAble = AbleToInterceptBall(players[0]);
        float fIntercept = (fNear + (fAble + fClosing)) / 3.0f;
        fIntercept = FMIN(fn_800DED80(players[0]), fIntercept);
        fReceiving = FMAX(fReceiving, fIntercept);
        score[0] = FMAX(fOwner, fReceiving);
    }
    {
        float fOwner = BallOwner(players[1]);
        float fReceiving = ReceivingPass(players[1]);
        float fClosing = ClosingTo(players[1], g_pBall);
        float fNear = NearToBall(players[1]);
        float fAble = AbleToInterceptBall(players[1]);
        float fIntercept = (fNear + (fAble + fClosing)) / 3.0f;
        fIntercept = FMIN(fn_800DED80(players[1]), fIntercept);
        fReceiving = FMAX(fReceiving, fIntercept);
        score[1] = FMAX(fOwner, fReceiving);
    }
    return score[0] / FMAX(0.1f, score[0] + score[1]);
}

float Losing(cTeam* team)
{
    if (team == NULL)
    {
        return 0.0f;
    }

    cTeam* pOtherTeam = team->GetOtherTeam();
    int scoreDiff = team->m_nScore - pOtherTeam->m_nScore;

    return NormalizeVal((float)scoreDiff, 0.0f, -g_pGame->m_pFuzzyTweaks->mUnidentified6D4);
}

float Tied(cTeam* team)
{
    if (team == NULL)
    {
        return 0.0f;
    }

    cTeam* pOtherTeam = team->GetOtherTeam();
    int scoreDiff = team->m_nScore - pOtherTeam->m_nScore;
    int absScoreDiff = (scoreDiff < 0) ? -scoreDiff : scoreDiff;

    return NormalizeVal((float)absScoreDiff, g_pGame->m_pFuzzyTweaks->mUnidentified6F4, 0.0f);
}

float Winning(cTeam* team)
{
    if (team == NULL)
    {
        return 0.0f;
    }

    cTeam* pOtherTeam = team->GetOtherTeam();
    int scoreDiff = team->m_nScore - pOtherTeam->m_nScore;

    return NormalizeVal((float)scoreDiff, 0.0f, g_pGame->m_pFuzzyTweaks->mUnidentified6E4);
}

float Offensive(cTeam* pTeam)
{
    if (pTeam == NULL)
    {
        return 0.0f;
    }

    if (pTeam->mpCurrentSituation == SITUATION_OFFENSE)
    {
        return 1.0f;
    }

    return 0.0f;
}

float Defensive(cTeam* pTeam)
{
    if (pTeam == NULL)
    {
        return 0.0f;
    }

    if (pTeam->mpCurrentSituation == SITUATION_DEFENSE)
    {
        return 1.0f;
    }

    return 0.0f;
}

float Loose(cTeam* pTeam)
{
    if (pTeam == NULL)
    {
        return 0.0f;
    }

    if (pTeam->mpCurrentSituation == SITUATION_LOOSE)
    {
        return 1.0f;
    }

    return 0.0f;
}

float Stalling(cTeam* pTeam)
{
    if (pTeam == NULL)
    {
        return 0.0f;
    }

    float fDifficulty = Difficult(pTeam->GetOtherTeam());
    float fMinTime = InterpolateClamped(g_pGame->m_pFuzzyTweaks->mUnidentified774,
        g_pGame->m_pFuzzyTweaks->mUnidentified794, fDifficulty);
    float fMaxTime = InterpolateClamped(g_pGame->m_pFuzzyTweaks->mUnidentified784,
        g_pGame->m_pFuzzyTweaks->mUnidentified7A4, fDifficulty);
    return NormalizeVal(pTeam->mtDefensiveZoneTimer.GetSeconds(), fMinTime, fMaxTime);
}

extern "C" float fn_800DFF1C()
{
    if (!g_pBall->HasActivePassTarget())
    {
        return 0.0f;
    }

    return 1.0f;
}

extern "C" float fn_800DFF60()
{
    if (!g_pBall->HasActivePassTarget())
    {
        return 0.0f;
    }

    float fReaction = 1.0f - fn_800A636C(g_pCurrentlyUpdatingTeam)->GetReaction(NULL);
    float fPassDeadZone = g_pGame->m_pFuzzyTweaks->mUnidentified6C4;
    float fMaxValue = fReaction * fPassDeadZone;
    return NormalizeVal(IsPassInPlay(g_pBall), 0.0f, fMaxValue);
}

extern "C" float fn_800E0034()
{
    // Retail evaluates the shot distance without assigning it to the result.
    float fScore = 0.0f;
    cBall* pBall = g_pBall;
    if (pBall->m_tLightningTimer.m_uPackedTime != 0)
    {
        cPlayer* pPrevOwner = pBall->m_pPrevOwner;
        if (pPrevOwner != NULL && pPrevOwner->m_eClassType == FIELDER)
        {
            float fDistance = nlSqrt(nlVec3DistanceSquared2D(pBall->GetPosition(),
                pPrevOwner->mUnidentified024.m_v3Position), true);
            FMIN(FMAX(fDistance / g_pGame->m_pFuzzyTweaks->mUnidentified414, 0.0f), 1.0f);
        }
    }
    return fScore;
}

extern "C" float fn_800E00F8()
{
    float fInitialScore;
    if (g_pBall->UnidentifiedHasPassTarget())
    {
        fInitialScore = 1.0f;
    }
    else
    {
        fInitialScore = 0.0f;
    }
    float fScore = fInitialScore;
    if (fInitialScore > 0.0f)
    {
        if (g_pBall->m_pPrevOwner != NULL && g_pBall->m_pPrevOwner->m_eClassType == FIELDER)
        {
            fScore *= 2.0f;
        }
    }
    float fClampedScore = FMAX(fScore, 0.0f);
    fClampedScore = FMIN(fClampedScore, 1.0f);
    return fClampedScore;
}

float TimeCloseToOver(cGame* pGame)
{
    if (!pGame)
    {
        return 0.0f;
    }

    FuzzyTweaks* pTweaks = g_pGame->m_pFuzzyTweaks;
    return NormalizeVal(pGame->GetNormalizedGameTime(), pTweaks->mUnidentified704, 1.0f);
}

float TimeNearlyOver(cGame* pGame)
{
    if (!pGame)
    {
        return 0.0f;
    }

    FuzzyTweaks* pTweaks = g_pGame->m_pFuzzyTweaks;
    return NormalizeVal(pGame->GetNormalizedGameTime(), pTweaks->mUnidentified714, 1.0f);
}

float TimeFarFromOver(cGame* pGame)
{
    if (!pGame)
    {
        return 0.0f;
    }

    FuzzyTweaks* pTweaks = g_pGame->m_pFuzzyTweaks;
    return NormalizeVal(pGame->GetNormalizedGameTime(), 1.0f, pTweaks->mUnidentified724);
}

float Difficult(cTeam* pTeam)
{
    if (pTeam == NULL)
    {
        return 0.0f;
    }

    int diff = GameInfoManager::Instance()->mCurrentDifficulty[(s16)pTeam->m_nSide];
    if (diff == 7)
    {
        return 0.5f;
    }

    float fScore = FMAX((float)diff / 5.0f, 0.0f);
    return FMIN(fScore, 1.0f);
}

float InOffensiveZone(const nlVector3& v3Position, eTeamSide teamside)
{
    nlVector3 aiLoc;
    FieldLocToAILoc(aiLoc, v3Position, teamside);

    return NormalizeVal(aiLoc.x, g_pGame->m_pFuzzyTweaks->mUnidentified754,
        g_pGame->m_pFuzzyTweaks->mUnidentified764);
}

float InDefensiveZone(cPlayer* pPlayer)
{
    if (pPlayer == NULL)
    {
        return 0.0f;
    }

    nlVector3 aiLoc;
    FieldLocToAILoc(aiLoc, pPlayer->mUnidentified024.m_v3Position, (eTeamSide)pPlayer->m_pTeam->m_nSide);

    return NormalizeVal(aiLoc.x, g_pGame->m_pFuzzyTweaks->mUnidentified734,
        g_pGame->m_pFuzzyTweaks->mUnidentified744);
}

float InOffensiveZone(cPlayer* pPlayer)
{
    if (pPlayer == NULL)
    {
        return 0.0f;
    }

    nlVector3 aiLoc;
    FieldLocToAILoc(aiLoc, pPlayer->mUnidentified024.m_v3Position, (eTeamSide)pPlayer->m_pTeam->m_nSide);

    return NormalizeVal(aiLoc.x, g_pGame->m_pFuzzyTweaks->mUnidentified754,
        g_pGame->m_pFuzzyTweaks->mUnidentified764);
}

static float InDefensiveZone(const nlVector3& v3Position, eTeamSide teamside)
{
    nlVector3 aiLoc;
    FieldLocToAILoc(aiLoc, v3Position, teamside);
    return NormalizeVal(aiLoc.x, g_pGame->m_pFuzzyTweaks->mUnidentified734,
        g_pGame->m_pFuzzyTweaks->mUnidentified744);
}

extern "C" float fn_800E0470(cPlayer* pPlayer)
{
    if (pPlayer == NULL)
    {
        return 0.0f;
    }
    const nlVector3& playerPos = pPlayer->mUnidentified024.m_v3Position;
    return 1.0f - FMAX(
        InDefensiveZone(playerPos, HOME),
        InOffensiveZone(playerPos, HOME));
}

float InDefensiveZoneOfPlayer(cBall* pBall, cPlayer* pPlayer)
{
    if ((pBall == NULL) && (pPlayer != NULL))
    {
        return 0.0f;
    }

    nlVector3 aiLoc;
    FieldLocToAILoc(aiLoc, pBall->m_v3Position, (eTeamSide)pPlayer->m_pTeam->m_nSide);

    return NormalizeVal(aiLoc.x, g_pGame->m_pFuzzyTweaks->mUnidentified734,
        g_pGame->m_pFuzzyTweaks->mUnidentified744);
}

extern "C" float fn_800E05A4(cBall* pBall, cPlayer* pPlayer)
{
    if (pBall == NULL && pPlayer != NULL)
    {
        return 0.0f;
    }
    return 1.0f - FMAX(
        InDefensiveZone(pBall->m_v3Position, (eTeamSide)pPlayer->m_pTeam->m_nSide),
        InOffensiveZone(pBall->m_v3Position, (eTeamSide)pPlayer->m_pTeam->m_nSide));
}

float InOffensiveZoneOfPlayer(cBall* pBall, cPlayer* pPlayer)
{
    if ((pBall == NULL) && (pPlayer != NULL))
    {
        return 0.0f;
    }

    nlVector3 aiLoc;
    FieldLocToAILoc(aiLoc, pBall->m_v3Position, (eTeamSide)pPlayer->m_pTeam->m_nSide);

    return NormalizeVal(aiLoc.x, g_pGame->m_pFuzzyTweaks->mUnidentified754,
        g_pGame->m_pFuzzyTweaks->mUnidentified764);
}

static const nlVector2 lbl_806E42B0 = { 5.0f, 1.0f };


extern "C" float fn_800E06F4(cPlayer* pPlayer)
{
    if (pPlayer == NULL)
        return 0.0f;
    float fTotal = 0.0f;
    float fClose = NormalizeVal(g_pGame->m_fCachedBallPlayerDistances[pPlayer->mUnidentified120],
        lbl_806E42B0.x, lbl_806E42B0.y);
    if (fClose < 1.0f)
    {
        float fMinDistance = lbl_806E42B0.x;
        float fMaxDistance = lbl_806E42B0.y;
        for (int i = 0; i < 10; i++)
        {
            cPlayer* pOther = (cPlayer*)g_pCharacters[i];
            bool bUnavailable = false;
            if (pOther->m_eClassType == FIELDER)
                bUnavailable = ((cFielder*)pOther)->fn_800344B0() || ((cFielder*)pOther)->IsShattered();
            if (pOther == pPlayer || pOther->m_pBall != NULL
                || pOther->m_eClassType == GOALIE || bUnavailable)
                continue;
            float fOtherClose = NormalizeVal(g_pGame->m_fCachedBallPlayerDistances[i],
                fMinDistance, fMaxDistance);
            if (!pOther->IsOnSameTeam(pPlayer) && !Incapacitated(pOther))
                fOtherClose *= 0.2f;
            fTotal += fOtherClose;
            if (fTotal >= 3.6f)
            {
                fTotal = 3.6f;
                break;
            }
        }
    }
    return (1.0f - 0.5f * fClose) * (fTotal / 3.6f);
}

static float FarToMark(cFielder* fielder)
{
    return FarTo(fielder, g_pScriptCurrentMark);
}

static float NearToMark(cFielder* fielder)
{
    return NearTo(fielder, g_pScriptCurrentMark);
}

static float CloseToMark(cFielder* fielder)
{
    return CloseTo(fielder, g_pScriptCurrentMark);
}

static float OpenFromMe(cPlayer* fielder)
{
    return OpenTo(g_pScriptCurrentFielder, fielder);
}

static float OpenToMe(cPlayer* fielder)
{
    return OpenTo(fielder, g_pScriptCurrentFielder);
}

static float FarToMe(cPlayer* fielder)
{
    return FarTo(fielder, g_pScriptCurrentFielder);
}

static float NearToMe(cPlayer* fielder)
{
    return NearTo(fielder, g_pScriptCurrentFielder);
}

static float CloseToMe(cPlayer* fielder)
{
    return CloseTo(fielder, g_pScriptCurrentFielder);
}

static float OnMyTeam(cFielder* fielder)
{
    if (fielder == NULL)
    {
        return 0.0f;
    }

    if (g_pScriptCurrentTeam == fielder->m_pTeam)
    {
        return 1.0f;
    }

    return 0.0f;
}

float OnTheirTeam(cFielder* fielder)
{
    if (fielder == NULL)
    {
        return 0.0f;
    }

    if (g_pScriptOtherTeam == fielder->m_pTeam)
    {
        return 1.0f;
    }

    return 0.0f;
}
