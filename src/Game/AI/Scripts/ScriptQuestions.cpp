#include "Game/AI/Scripts/ScriptQuestions.h"
#include "Game/AI/Scripts/ScriptCaching.h"
#include "Game/FormationDefines.h"
#include "Game/AI/AiUtil.h"
#include "Game/AI/AvoidController.h"
#include "Game/AI/ShotMeter.h"
#include "Game/AI/Fuzzy.h"
#include "Game/Game.h"
#include "Game/GameInfo.h"
#include "Game/GameTweaks.h"
#include "Game/Goalie.h"
#include "Game/CharacterTweaks.h"
#include "Game/Field.h"
#include "Game/MathHelpers.h"
#include "Game/Net.h"
#include "Game/ScriptTuning.h"
#include "types.h"
extern cTeam* g_pCurrentlyUpdatingTeam;
extern cBall* g_pScriptBall;
extern cBall* g_pBall;
extern cTeam* g_pScriptOtherTeam;
extern cTeam* g_pScriptCurrentTeam;
extern cFielder* g_pScriptBallOwner;
extern "C" bool fn_8001E168(const cCharacter* pCharacter);
extern "C" float fn_800DB298(const nlVector3&, const nlVector3&, cFielder*,
    float, float, float, float, bool);
extern "C" float fn_800DAFCC(const nlVector3&, const nlVector3&, cPlayer*,
    cPlayer*, float, float, float, float);

static const nlVector2 g_vOpenToAdjust = { 0.0f, 0.8f };
static const nlVector2 g_vPassCloseToDoneConfidence = { 0.0f, 0.5f };
static const nlVector2 g_vStallingConfidenceTime = { 1.0f, 8.0f };
static const nlVector2 v2Zero = { 0.0f, 0.0f };

static float CloseToGoaliePosition(const nlVector3& v3FromPos, const nlVector3& v3GoaliePos);
static float FarToGoaliePosition(const nlVector3& v3FromPos, const nlVector3& v3GoaliePos);

static inline float IsPassInPlay(cBall* pBall)
{
    if (pBall->m_fTotalPassTime > 0.0f)
    {
        float fElapsedTime = pBall->m_tPassTargetTimer.GetSeconds() / pBall->m_fTotalPassTime;
        return (1.0f - fElapsedTime);
    }
    return 0.0f;
}

static float FacingMark(cFielder* fielder)
{
    return Facing(fielder, g_pScriptCurrentMark);
}

static float FacingMe(cFielder* fielder)
{
    return Facing(fielder, g_pScriptCurrentFielder);
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

static float FarToMyNetB(cBall* ball)
{
    return FarToPlayersNet(ball, g_pScriptCurrentFielder);
}

static float NearToMyNetB(cBall* ball)
{
    return NearToPlayersNet(ball, g_pScriptCurrentFielder);
}

static float CloseToMyNetB(cBall* ball)
{
    return CloseToPlayersNet(ball, g_pScriptCurrentFielder);
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

    if (g_pScriptBall->m_pPrevOwner == player)
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

    if (fn_8001E168(fielder))
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

static float FielderType(cPlayer* player)
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

float GoalieOutOfPosition(cFielder* pFielder)
{
    nlVector3 goalieNetPos;
    cPlayer* pGoalie;

    if (pFielder == NULL)
    {
        return 0.0f;
    }

    pGoalie = (cPlayer*)pFielder->m_pTeam->GetOtherTeam()->GetGoalie();
    float halfNetWidth = cNet::m_fNetWidth / 2.0f;
    goalieNetPos = pGoalie->m_v3Position;
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
        nlVec3DistanceSquared2D(pFielder->m_v3Position, offNetLocation), true);

    float goalieDistance = nlSqrt(
        nlVec3DistanceSquared2D(pGoalie->m_v3Position, goalieNetPos), true);

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

float LikelyToScore(cFielder* pFielder)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }

    cNet* pNet = pFielder->m_pTeam->GetOtherNet();
    return fn_800DB298(pFielder->m_v3Position, pNet->m_v3NetLocation,
        pFielder, 0.0f, 0.2f, 1.0f, 0.0f, false);
}

float Open(cFielder* pFielder)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }

    return OpenPosition(
        pFielder->m_v3Position,
        pFielder->m_pTeam->GetOtherTeam(),
        NULL,
        NULL);
}

float WideOpen(cFielder* pFielder)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }

    return WideOpenPosition(
        pFielder->m_v3Position,
        pFielder->m_pTeam->GetOtherTeam(),
        pFielder);
}

float OpenToTheirNet(cFielder* pFielder)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }

    cTeam* pOtherTeam = pFielder->m_pTeam->GetOtherTeam();

    return OpenToPosition(pFielder->m_v3Position, pFielder->GetAIOffNetLocation(NULL), pOtherTeam, pFielder, NULL, true);
}

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

    float fResult = fn_800DAFCC(pFromFielder->m_v3Position,
        pToFielder->m_v3Position, pFromFielder, pToFielder,
        0.5f, 1.0f, 1.0f, 0.0f);
    return NormalizeVal(fResult, g_vOpenToAdjust);
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

float UserControlledT(cTeam* team)
{
    if (!team)
    {
        return 0.0f;
    }

    bool bHasAssignedController = team->GetNumAssignedControllers() > 0;
    if (bHasAssignedController)
    {
        return 1.0f;
    }

    return 0.0f;
}

float GonnaGetBall(cTeam* team)
{
    if (team == NULL)
    {
        return 0.0f;
    }

    cFielder* players[2];
    players[0] = team->m_pBallInterceptOrderedFielders[0];
    players[1] = team->GetOtherTeam()->m_pBallInterceptOrderedFielders[0];
    float score[2] = {
        FMAX(BallOwner(players[0]), FMIN(ChasingBall(players[0]), (NearToBall(players[0]) + (AbleToInterceptBall(players[0]) + ClosingTo(players[0], g_pBall))) / 3.0f)),
        FMAX(BallOwner(players[1]), FMIN(ChasingBall(players[1]), (NearToBall(players[1]) + (AbleToInterceptBall(players[1]) + ClosingTo(players[1], g_pBall))) / 3.0f))
    };
    float fScore = 0.0f;
    float total_score;

    total_score = score[0] + score[1];
    if (total_score > 0.0f)
    {
        fScore = score[0] / total_score;
    }

    return fScore;
}
