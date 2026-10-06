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

extern "C" Goalie* fn_800D66A0(cFielder* pFielder);
extern "C" Goalie* fn_800D66C4(cFielder* pFielder);
extern "C" cFielder* fn_800D674C(cPlayer* player);
extern "C" float fn_800DDF54(cPlayer* pCandidateFielder, cPlayer* pTargetFielder);
extern "C" float fn_800DED80(cPlayer* pPlayer);
extern "C" cFielder* fn_800DF790(cTeam* pTeam);

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
float CloseToSideline(const nlVector3& v3Position, const nlVector2* vDistanceConfidence, bool bInvert, nlVector2* pUnidentified);
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
extern "C" cTeam* fn_800D6670(cFielder*);
extern "C" cTeam* fn_800D6688(cFielder*);
extern "C" cFielder* fn_800D6708(cTeam*);
extern "C" cFielder* fn_800D671C(cTeam*);
extern "C" cFielder* fn_800D6734(cFielder*);
extern "C" void* fn_800D673C(void*);
extern "C" cPlayer* fn_800D6744(cBall*);
extern "C" float fn_800D6A90(cFielder*);
extern "C" float fn_800D6AF0(cFielder*);
extern "C" float fn_800D6BD8(cFielder*);
extern "C" float fn_800D6CD4(cPlayer*, cPlayer*);
extern "C" float fn_800D6D14(cPlayer*, cPlayer*);
extern "C" float fn_800D6D78(cPlayer*);
extern "C" float fn_800D74D8(cFielder*);
extern "C" float fn_800D763C(cFielder*);
extern "C" float fn_800D76B8(cFielder*);
extern "C" float fn_800D7734(cFielder*);
extern "C" float fn_800D77B0(cFielder*);
extern "C" float fn_800D782C(cFielder*);
extern "C" float fn_800D7878(cFielder*);
extern "C" float fn_800D78C4(cFielder*);
extern "C" float fn_800D7910(cFielder*);
extern "C" float fn_800D795C(cFielder*, int);
extern "C" float fn_800D7988(int, cFielder*);
extern "C" float fn_800D79F4(int, cFielder*);
extern "C" float fn_800D7A70(cFielder*);
extern "C" float fn_800D7AB8(cFielder*);
extern "C" float fn_800D7B00(cFielder*);
extern "C" float fn_800D82C0(cFielder*);
extern "C" float fn_800D84F8(cFielder*);
extern "C" float fn_800D85F8(cFielder*);
extern "C" float fn_800D8764(cFielder*, int);
extern "C" float fn_800D8834(cFielder*, int);
extern "C" float fn_800D88B4(cFielder*);
extern "C" float fn_800D8970(cFielder*);
extern "C" float fn_800D8A9C(cFielder*);
extern "C" float fn_800D8BAC(cFielder*);
extern "C" float fn_800D912C(cFielder*);
extern "C" float fn_800D91BC(cFielder*);
extern "C" float fn_800D924C(cFielder*);
extern "C" float fn_800D92DC(cFielder*);
extern "C" float fn_800D9368(cFielder*);
extern "C" float fn_800D93F4(cFielder*);
extern "C" float fn_800D9480(cFielder*);
extern "C" float fn_800D96F4(cFielder*);
extern "C" float fn_800D9A38(cFielder*);
extern "C" float fn_800D9B0C(cFielder*);
extern "C" float fn_800D9B74(cFielder*);
extern "C" float fn_800D9BDC(cFielder*);
extern "C" float fn_800D9C24(cFielder*);
extern "C" float fn_800D9D04(cFielder*);
extern "C" float fn_800D9D78(cPlayer*);
extern "C" float fn_800D9DD8(cPlayer*);
extern "C" float fn_800D9FC8(cFielder*);
extern "C" float fn_800DA050(cFielder*);
extern "C" float fn_800DA0C8(cFielder*);
extern "C" float fn_800DA130(cFielder*);
extern "C" float fn_800DA310(cFielder*);
extern "C" float fn_800DA330(cFielder*);
extern "C" float fn_800DA518(cFielder*);
float NearToFormationPosition(cFielder* pFielder, nlVector3* pPosition);
extern "C" float fn_800DA91C(cFielder*);
extern "C" float fn_800DACF4(cPlayer*);
extern "C" float fn_800DAD3C(cBall*);
extern "C" float fn_800DAFCC(const nlVector3& vFrom, const nlVector3& vTo,
    cPlayer* pIgnorePlayer1, cPlayer* pIgnorePlayer2, float fTeamWeight,
    float fOpponentWeight, float fGoalieWeight, float fPredictionTime);
extern "C" float fn_800DBB0C(cFielder*);
extern "C" float fn_800DBB88(cFielder*);
extern "C" float fn_800DBEF4(cFielder*, cFielder*);
extern "C" float fn_800DC19C(cFielder*, cBall*);
extern "C" float fn_800DC434(cFielder*, cBall*);
extern "C" float fn_800DCB4C(const nlVector3*, const nlVector3*);
extern "C" float fn_800DD234(cFielder*);
extern "C" float fn_800DD294(cFielder*);
extern "C" float fn_800DD2F4(cBall*);
extern "C" float fn_800DD31C(cFielder*);
extern "C" float fn_800DD37C(cFielder*);
extern "C" float fn_800DD45C(cFielder*);
extern "C" float fn_800DD494(cFielder*);
extern "C" float fn_800DD4CC(cFielder*);
extern "C" float fn_800DD504(cPlayer*, cFielder*);
extern "C" float fn_800DD5C4(cPlayer*, cFielder*);
extern "C" float fn_800DD684(cPlayer*, cFielder*);
extern "C" float fn_800DD744(cFielder*);
extern "C" float fn_800DD7F4(cFielder*);
extern "C" float fn_800DD944(cPlayer*);
extern "C" float fn_800DD99C(cFielder*);
extern "C" float fn_800DD9C8(cFielder*, cPlayer*);
extern "C" float fn_800DDD70(cFielder*);
extern "C" float fn_800DE1F0(cPlayer*, cPlayer*);
extern "C" float fn_800DE4B0(cPlayer*, cPlayer*);
extern "C" float fn_800DE71C(cPlayer*);
extern "C" float fn_800DE7D8(Goalie*);
extern "C" float fn_800DE804(cBall*, cTeam*);
extern "C" float fn_800DE8CC(cBall*, cTeam*);
extern "C" float fn_800DE994(cBall*, cTeam*);
extern "C" float fn_800DEAB4(cFielder*);
extern "C" float fn_800DEBBC(cPlayer*);
extern "C" float fn_800DEBF4(cFielder*);
extern "C" float fn_800DEC88(cFielder*);
extern "C" float fn_800DED3C(cFielder*);
extern "C" float fn_800DF0B8(cFielder*);
extern "C" float fn_800DF118(cFielder*);
extern "C" float fn_800DF1B8(cFielder*);
extern "C" float fn_800DF2C0(cFielder*);
extern "C" float fn_800DF390(cPlayer*);
extern "C" float fn_800DF474(cFielder*);
extern "C" float fn_800DF590(cBall*);
extern "C" float fn_800DF838(cPlayer*);
extern "C" float fn_800DFF1C();
extern "C" float fn_800DFF60();
extern "C" float fn_800E0034();
extern "C" float fn_800E00F8();
extern "C" float fn_800E0470(cPlayer*);
extern "C" float fn_800E05A4(cBall*, cPlayer*);
extern "C" float fn_800E06F4(cPlayer*);


extern "C" float fn_800DBAB0(cFielder* pFielder);
extern "C" float fn_800DE0A8(cPlayer* pPlayer);
extern "C" float fn_800DE40C(cPlayer* pUpfieldPlayer, cPlayer* pFromPlayer);
extern "C" float fn_800DEB04(cFielder* pFielder);
extern "C" float fn_800DF028(cFielder* pFielder);
extern "C" float fn_800DF888(cTeam* team);

#endif // _SCRIPTQUESTIONS_H_
