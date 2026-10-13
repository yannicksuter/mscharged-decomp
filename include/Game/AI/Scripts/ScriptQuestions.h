#ifndef _SCRIPTQUESTIONS_H_
#define _SCRIPTQUESTIONS_H_

#include "NL/nlMath.h"
#include "Game/Player.h"
#include "Game/Ball.h"
#include "Game/Team.h"
#include "Game/AI/Fielder.h"
#include "Game/AI/Scripts/ScriptDefines.h"

class cGame;


enum eScriptFielderDesire
{
    edNone = 0,
    edCutAndBreak = 1,
    edBlockPass = 6,
    edBlockShot = 6,
    edDeke = 2,
    edGetInPosition = 3,
    edGetOpen = 4,
    edHeavyAttack = 5,
    edInterceptBall = 6,
    edMark = 7,
    edProtectBall = 8,
    edRunToNet = 9,
    edRunUpfield = 10,
    edRunDownfield = 11,
    edRunToLocation = 12,
    edPass = 13,
    edShoot = 14,
    edSlideAttack = 15,
    edSupportBallDefensive = 16,
    edSupportBallOffensive = 17,
    edUsePowerup = 18,
    edWindupPass = 19,
    edWindupShot = 20,
    edUserControl = 22,
    edOneTimer = 24,
    edPostWhistle = 25,
    edWait = 28,
};

Goalie* MyGoalie(cFielder* pFielder);
Goalie* TheirGoalie(cFielder* pFielder);
cFielder* ClosestOpponentOf(cPlayer* player);
float Facing(cPlayer* pCandidateFielder, cPlayer* pTargetFielder);
float PursuingBall(cPlayer* pPlayer);
cFielder* GetStrategicBallCarrier(cTeam* pTeam);

float DistanceAndAngleConfidence(const nlVector3& vFrom, const nlVector3& vTo,
    unsigned short aDirection, const nlVector2* pDistanceRange,
    const nlVector2* pAngleRange, bool bDistancePeak, bool bRequireInRange,
    float fDistanceWeight);

float CloseToFormationPosition(cFielder* pFielder);
float CloseToMyGoalie(cPlayer*);
float DoingS2S(cFielder*);
float FarToFormationPosition(cFielder* pFielder);
float FarToMyGoalie(cPlayer*);
float FielderType(cPlayer*);
float InFrontOfMyNet(cFielder*);
float NearToMyGoalie(cPlayer*);
float ReallyCloseToBall(cPlayer*);
float ReallyHigh(cBall*);

float SeparatingFrom(cPlayer*, cBall*);

float InOffensiveZoneOfPlayer(cBall* pBall, cPlayer* pPlayer);
float InDefensiveZoneOfPlayer(cBall* pBall, cPlayer* pPlayer);
float InOffensiveZone(cPlayer* pPlayer);
float InDefensiveZone(cPlayer* pPlayer);
float InOffensiveZone(const nlVector3& v3Position, eTeamSide teamside);
float Difficult(cTeam* pTeam);
float TimeFarFromOver(cGame* pGame);
float TimeNearlyOver(cGame* pGame);
float TimeCloseToOver(cGame* pGame);
float Stalling(cTeam* pTeam);
float Loose(cTeam* pTeam);
float Defensive(cTeam* pTeam);
float Offensive(cTeam* pTeam);
float Winning(cTeam* team);
float Tied(cTeam* team);
float Losing(cTeam* team);
float UserControlledT(cTeam* team);
float Passive(cTeam* team);
float Moderate(cTeam* team);
float AggressiveT(cTeam* team);
float Ownerless(cBall* ball);
float High(cBall* ball);
float ReceivingPass(cFielder* pFielder);
float InControlOfBall(cFielder* fielder);
float OutOfNet(Goalie* pGoalie);
float SeparatingFrom(cPlayer* pFielder1, cPlayer* pFielder2);
float ClosingTo(cPlayer* pPlayer, cBall* pBall);
float ClosingTo(cPlayer* pFielder1, cPlayer* pFielder2);
float CloseToSideline(cFielder* pFielder);
float NearToSideline(const nlVector3& v3Position);
float CloseToSideline(const nlVector3& v3Position, const nlVector2* vDistanceConfidence, bool bInvert, nlVector2* pNormal);
float FarToTheirGoalie(cPlayer* pPlayer);
float NearToTheirGoalie(cPlayer* pPlayer);
float CloseToTheirGoalie(cPlayer* pPlayer);
float FarTo(cPlayer* pPlayer1, cPlayer* pPlayer2);
float NearTo(cPlayer* pPlayer1, cPlayer* pPlayer2);
float CloseTo(cPlayer* pPlayer1, cPlayer* pPlayer2);
float OpenTo(cPlayer* pFromFielder, cPlayer* pToFielder);
float InBetweenMyNetAnd(cFielder* pFielder, cBall* pBall);
float InBetweenMyNetAnd(cFielder* pFielder, cFielder* pOtherFielder);
float NearToFormationPosition(cFielder* pFielder);
float InFrontOfTheirNet(cFielder* pFielder);
float PositionIsInFrontOfNet(const nlVector3& v3Position, const cNet* pNet);
float PositionOpenness(const nlVector3& v3Position, cTeam* pOpponentTeam,
    cPlayer* pCurrentPlayer, const nlVector2* vOpenRadius, bool bIgnoreIncapacitated,
    float fPredictionTime);
float WidePositionOpenness(const nlVector3& v3Position, cTeam* pOpponentTeam,
    cPlayer* pCurrentPlayer, bool bIgnoreIncapacitated, float fPredictionTime);
float PositionDistanceConfidence(const nlVector3& vFrom, const nlVector3& vTo,
    float fMin, float fMax);
float PositionShotDistance(const nlVector3& vPosition, const nlVector3& vOffNetPosition,
    float fShooting);
float GoalieOutOfPosition(cFielder* pFielder);
float LikelyToScore(cFielder* pFielder);
float PlayerShotDistance(cFielder* pFielder);
float FallenDown(cFielder* pFielder);
float Frozen(cFielder* pFielder);
float Incapacitated(cPlayer* pPlayer);
float Invincible(cFielder* pFielder);
float Pressured(cFielder* pFielder);
float FarToTheirNet(cPlayer* pPlayer);
float NearToTheirNet(cPlayer* pPlayer);
float CloseToTheirNet(cPlayer* pPlayer);
float FarToMyNet(cPlayer* pPlayer);
float NearToMyNet(cPlayer* pPlayer);
float CloseToMyNet(cPlayer* pPlayer);
float FarToBall(cPlayer* pPlayer);
float NearToBall(cPlayer* pPlayer);
float CloseToBall(cPlayer* pPlayer);
float AbleToInterceptBall(cPlayer* pPlayer);
float Shooter(cFielder* fielder);
float InPassingLane(cFielder* pFielder);
float UserControlled(cFielder* fielder);
float StrategicBallOwner(cFielder* pFielder);
float OnTheGround(cPlayer* player);
float OnTheirTeam(cFielder* fielder);
float Marking(cFielder* pMarking, cPlayer* pMarked);
float GoalieType(cPlayer* player);
float Captain(cFielder* fielder);
float Defence(cFielder* fielder);
float Midfield(cFielder* fielder);
float Winger(cFielder* fielder);
float Striker(cFielder* fielder);
float LastBallOwner(cPlayer* player);
float BallOwnerT(cTeam* team);
float BallOwner(cPlayer* player);

template <typename T>
nlVector3& PositionOf(T pObject)
{
    return pObject->m_v3Position;
}


// Shared functions and data from Game/AI/Scripts/ScriptQuestions.cpp.
cTeam* TeamOf(cFielder*);
cTeam* OtherTeamOf(cFielder*);
cFielder* CaptainOf(cTeam*);
cFielder* BestBallInterceptorOf(cTeam*);
cFielder* MarkOf(cFielder*);
extern "C" void* fn_800D673C(void*);
cPlayer* PassTargetOf(cBall*);
float MovingSlowly(cFielder*);
float JoggingSpeed(cFielder*);
float RunningSpeed(cFielder*);
float OnSameTeam(cPlayer*, cPlayer*);
float OnOpposingTeams(cPlayer*, cPlayer*);
float OnScreen(cPlayer*);
float InShotLane(cFielder*);
float PassingSkill(cFielder*);
float ShootingSkill(cFielder*);
float HittingSkill(cFielder*);
float SlideTacklingSkill(cFielder*);
float PlaymakerRating(cFielder*);
float PowerRating(cFielder*);
float OffensiveRating(cFielder*);
float DefensiveRating(cFielder*);
float CharacterClassIs(cFielder*, int);
float ActiveDesire(int, cFielder*);
float RecentlyActiveDesire(int, cFielder*);
extern "C" float fn_800D7A70(cFielder*);
float AbleToContactLooseBall(cFielder*);
float AbleToSlideTackleBall(cFielder*);
float AbleToInterceptBallForSwapController(cFielder*);
float AbleToUseSuperPower(cFielder*);
float AbleToUseAnyPowerup(cFielder*);
float AbleToUsePowerup(cFielder*, int);
float LikelyToUsePowerup(cFielder*, int);
float ProjectilePowerupStock(cFielder*);
float NonThrowablePowerupSlots(cFielder*);
float BallInterceptionRank(cFielder*);
float TeamBallInterceptionRank(cFielder*);
float CloseToTeammates(cFielder*);
float NearToTeammates(cFielder*);
float FarToTeammates(cFielder*);
float CloseToOpponents(cFielder*);
float NearToOpponents(cFielder*);
float FarToOpponents(cFielder*);
float PressureFromNearbyOpponents(cFielder*);
float Attacked(cFielder*);
float AvoidingGoalie(cFielder*);
float AvoidingFielders(cFielder*);
float AvoidingPowerups(cFielder*);
float AvoidingSidelines(cFielder*);
float AvoidingStuff(cFielder*);
float InvincibleToCharacters(cFielder*);
float FallRecoveryProgress(cPlayer*);
float Unavailable(cPlayer*);
float Stuck(cFielder*);
float FallingOrShattered(cFielder*);
float GooeySpeedScale(cFielder*);
float Confused(cFielder*);
float RegularShotScore(cFielder*);
float ChipShotScore(cFielder*);
float CutAndBreakOpenness(cFielder*);
float NearToFormationPosition(cFielder* pFielder, const nlVector3* pPosition);
float OnBreakaway(cFielder*);
float OffField(cPlayer*);
float BallOffField(cBall*);
float LaneOpenness(const nlVector3& vFrom, const nlVector3& vTo,
    cPlayer* pIgnorePlayer1, cPlayer* pIgnorePlayer2, float fTeamWeight,
    float fOpponentWeight, float fGoalieWeight, float fPredictionTime);
float WideOpen(cFielder*);
float OpenToTheirNet(cFielder*);
float InBetweenTheirNetAnd(cFielder*, cFielder*);
float InBetweenMyNetAndBall(cFielder*, cBall*);
float InBetweenTheirNetAndBall(cFielder*, cBall*);
float NearToGoaliePosition(const nlVector3* position, const nlVector3* goaliePosition);
float NearToSideline(cFielder*);
float FarFromSideline(cFielder*);
float BallCloseToSideline(cBall*);
float BallNearToSideline(cBall*);
float BallFarFromSideline(cBall*);
float CloseToDesiredPosition(cFielder*);
float NearToDesiredPosition(cFielder*);
float FarToDesiredPosition(cFielder*);
float CloseToPlayersDesiredPosition(cPlayer*, cFielder*);
float NearToPlayersDesiredPosition(cPlayer*, cFielder*);
float FarToPlayersDesiredPosition(cPlayer*, cFielder*);
float FacingSideline(cFielder*);
float StuckOnSidelines(cFielder*);
float SuperPowerActive(cPlayer*);
float SuperPowerAction(cFielder*);
float AtIdealDistanceForTackling(cFielder*, cPlayer*);
float AtIdealDistanceForSkillshot(cFielder*);
float FaceToFace(cPlayer*, cPlayer*);
float DownfieldFrom(cPlayer*, cPlayer*);
float AwayFromOwnNet(cPlayer*);
float GoalieDown(Goalie*);
float BallCloseToNet(cBall*, cTeam*);
float BallNearToNet(cBall*, cTeam*);
float BallFarToNet(cBall*, cTeam*);
float WindingUpOrShooting(cFielder*);
float DoingSkillshot(cPlayer*);
float ShotWindupProgress(cFielder*);
float SkillshotWindupProgress(cFielder*);
float MushroomActive(cFielder*);
float ReceivingVolleyPass(cFielder*);
float ReceivingGroundPass(cFielder*);
float ReceivingPassDelayed(cFielder*);
float PassCloseToDone(cFielder*);
float ReceivingVolleyPassDelayed(cPlayer*);
float ReceivingGroundPassDelayed(cFielder*);
float BallCharge(cBall*);
float BallPossessionDuration(cPlayer*);
float IsPassInPlay();
float IsPassInPlayDelayed();
float IsShotInPlay();
float HasPassTargetConfidence();
float InNeutralZone(cPlayer*);
float InNeutralZoneOfPlayer(cBall*, cPlayer*);
float BallCrowding(cPlayer*);


float Open(cFielder* pFielder);
float FacingBall(cPlayer* pPlayer);
float UpfieldFrom(cPlayer* pUpfieldPlayer, cPlayer* pFromPlayer);
float ShotMeterProgress(cFielder* pFielder);
float OneTouchShooting(cFielder* pFielder);
float TeamBallControlConfidence(cTeam* team);

#endif // _SCRIPTQUESTIONS_H_
