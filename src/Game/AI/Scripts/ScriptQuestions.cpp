#include "Game/AI/Scripts/ScriptQuestions.h"
#include "Game/AI/Desire.h"
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
extern "C" float fn_800DB298(const nlVector3&, const nlVector3&, cFielder*,
    float, float, float, float, bool);
extern "C" float fn_800DAFCC(const nlVector3&, const nlVector3&, cPlayer*,
    cPlayer*, float, float, float, float);

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

    float goalieDistance = nlSqrt(
        nlVec3DistanceSquared2D(pGoalie->mUnidentified024.m_v3Position, goalieNetPos), true);

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
    return fn_800DB298(pFielder->mUnidentified024.m_v3Position, pNet->m_v3NetLocation,
        pFielder, 0.0f, 0.2f, 1.0f, 0.0f, false);
}

float Open(cFielder* pFielder)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }

    return OpenPosition(
        pFielder->mUnidentified024.m_v3Position,
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
        pFielder->mUnidentified024.m_v3Position,
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

    return OpenToPosition(pFielder->mUnidentified024.m_v3Position, pFielder->GetAIOffNetLocation(NULL), pOtherTeam, pFielder, NULL, true);
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

    float fResult = fn_800DAFCC(pFromFielder->mUnidentified024.m_v3Position,
        pToFielder->mUnidentified024.m_v3Position, pFromFielder, pToFielder,
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

    if (team->GetNumAssignedControllers() > 0)
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

float GenerateFilteredRandom()
{
    return nlRandomf(1.0f);
}

float RandomChance(float fChance)
{
    return FGREATER(fChance, GenerateFilteredRandom());
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

extern "C" cFielder* fn_800D674C(cPlayer* player)
{
    return player->GetClosestOpponentFielder(NULL, true);
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

extern "C" float fn_800A0508(cFielder* pFielder, bool bIsChipShot, bool bWasPerfectPass);

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

float InFrontOfMyNet(cFielder* pFielder)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }

    return PositionIsInFrontOfNet(pFielder->mUnidentified024.m_v3Position, pFielder->m_pTeam->m_pNet);
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

float CloseToSideline(cFielder* pFielder)
{
    if (pFielder == NULL)
    {
        return 0.0f;
    }

    return CloseToSideline(pFielder->mUnidentified024.m_v3Position, NULL, false, NULL);
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

extern "C" float fn_8004028C(cFielder*);
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

extern "C" float fn_800DD2F4(cBall* ball)
{
    if (ball == NULL)
    {
        return 0.0f;
    }

    return CloseToSideline(ball->m_v3Position, NULL, false, NULL);
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

extern "C" bool fn_8002F858(cFielder*, bool);
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

extern "C" float fn_8002BE38(PlayerTweaks*);

extern "C" float fn_800D7910(cFielder* fielder)
{
    if (fielder == NULL)
    {
        return 0.0f;
    }

    PlayerTweaks* pTweaks = fielder->GetTweaks();
    return InterpolateRangeClamped(0.0f, 1.0f, 0.5f, 1.0f, fn_8002BE38(pTweaks));
}

extern "C" float fn_8002BE18(PlayerTweaks*);

extern "C" float fn_800D78C4(cFielder* fielder)
{
    if (fielder == NULL)
    {
        return 0.0f;
    }

    PlayerTweaks* pTweaks = fielder->GetTweaks();
    return InterpolateRangeClamped(0.0f, 1.0f, 0.5f, 1.0f, fn_8002BE18(pTweaks));
}

extern "C" float fn_8002BE84(PlayerTweaks*);

extern "C" float fn_800D7878(cFielder* fielder)
{
    if (fielder == NULL)
    {
        return 0.0f;
    }

    PlayerTweaks* pTweaks = fielder->GetTweaks();
    return InterpolateRangeClamped(0.0f, 1.0f, 0.5f, 1.0f, fn_8002BE84(pTweaks));
}

extern "C" float fn_8002BE64(PlayerTweaks*);

extern "C" float fn_800D782C(cFielder* fielder)
{
    if (fielder == NULL)
    {
        return 0.0f;
    }

    PlayerTweaks* pTweaks = fielder->GetTweaks();
    return InterpolateRangeClamped(0.0f, 1.0f, 0.5f, 1.0f, fn_8002BE64(pTweaks));
}

float InOffensiveZone(const nlVector3& v3Position, eTeamSide teamside)
{
    nlVector3 aiLoc;
    FieldLocToAILoc(aiLoc, v3Position, teamside);

    return NormalizeVal(aiLoc.x, g_pGame->m_pFuzzyTweaks->mUnidentified754,
        g_pGame->m_pFuzzyTweaks->mUnidentified764);
}

float NearToSideline(const nlVector3& v3Position)
{
    nlVector2 vDistanceConfidence;
    nlVec2Set(vDistanceConfidence,
        g_pGame->m_pFuzzyTweaks->mUnidentified324,
        g_pGame->m_pFuzzyTweaks->mUnidentified334);
    return CloseToSideline(v3Position, &vDistanceConfidence, false, NULL);
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
        fScore = NormalizeVal(pPlayer->m_tBallPossessionTimer.GetSeconds(), 1.0f, 5.0f);
    }
    return fScore;
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

extern "C" bool fn_8003E6EC(cFielder* pFielder);

extern "C" float fn_800DD944(cPlayer* pPlayer)
{
    if (pPlayer == NULL)
    {
        return 0.0f;
    }

    if (pPlayer->m_eClassType == FIELDER)
    {
        if (fn_8003E6EC((cFielder*)pPlayer))
        {
            return 1.0f;
        }
        return 0.0f;
    }
    return 0.0f;
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
