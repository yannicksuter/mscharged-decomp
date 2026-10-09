#include "Game/AI/FielderDesireTypes.h"
#include "NL/nlDLListContainer.inl"
#include "Game/AI/FielderDesireMachine.h"
#include "Game/AI/FielderDesireTransitions.h"
#include "Game/AI/ScriptMachine.h"
#include "Game/AI/AIContext.h"

#include "Game/AI/AiUtil.h"
#include "Game/AI/DesirePass.h"
#include "Game/AI/DesireReceivePass.h"
#include "Game/AI/DesireRunToNet.h"
#include "Game/AI/DesireShoot.h"
#include "Game/AI/DesireSlideAttack.h"
#include "Game/AI/DesireSteering.h"
#include "Game/AI/DesireSuperPower.h"
#include "Game/AI/DesireUsePowerup.h"
#include "Game/AI/DesireUserControlled.h"
#include "Game/AI/Fielder.h"
#include "Game/AI/DesireUpdate.h"
#include "Game/AI/Scripts/ScriptQuestions.h"
#include "Game/Formation.h"
#include "Game/Game.h"
#include "Game/Team.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"

char gKickoffNeutralName[] = "Kickoff Neutral";
char gKickoffAdvantageName[] = "Kickoff Advantage";

float gBehindGoalLineRunTimeLimit = 2.0f;
float gBehindGoalLineRunSpeed = 2.0f;
float gBehindGoalLineRunAvoidanceCoeff[2] = { 0.4f, 0.0f };

class DesireDoNothing : public Desire
{
public:
    DesireDoNothing()
        : Desire(FIELDER_DESIRE_DO_NOTHING, UnsetTransitionFunc(g_UnsetTransitionFunc))
    {
    }

    virtual ~DesireDoNothing();
};

inline cFielder* FielderDesireMachine::GetFielder() const
{
    return static_cast<cFielder*>(mAIContext->mData.pPlayer);
}

/**
 * Offset/Address/Size: 0x0 | 0x800D4E2C | size: 0x4C
 */
FielderDesireMachine::FielderDesireMachine()
    : ScriptMachine(36, true, false, 0)
{
}

/**
 * Offset/Address/Size: 0x4C | 0x800D4E78 | size: 0x58
 */
FielderDesireMachine::~FielderDesireMachine()
{
}

/**
 * Offset/Address/Size: 0xA4 | 0x800D4ED0 | size: 0xC14
 */
void FielderDesireMachine::Initialize()
{
    ScriptMachine::Initialize();

    DesireCutAndBreak* cutAndBreak
        = new (8, false) DesireCutAndBreak(FIELDER_DESIRE_CUT_AND_BREAK, (void*)TransDesireBallOwner);
    AddState(FIELDER_DESIRE_CUT_AND_BREAK, cutAndBreak, false);

    DesireDefendPos* defendPos
        = new (8, false) DesireDefendPos(FIELDER_DESIRE_DEFEND_POS, (void*)TransDesireDefendPos);
    AddState(FIELDER_DESIRE_DEFEND_POS, defendPos, false);

    DesireDeke* deke
        = new (8, false) DesireDeke(FIELDER_DESIRE_DEKE, (void*)TransDesireActionDone);
    AddState(FIELDER_DESIRE_DEKE, deke, false);

    DesireDoNothing* doNothing = new (8, false) DesireDoNothing();
    AddState(FIELDER_DESIRE_DO_NOTHING, doNothing, false);

    DesireFinishAction* finishAction
        = new (8, false) DesireFinishAction(FIELDER_DESIRE_FINISH_ACTION, (void*)TransDesireActionDone);
    AddState(FIELDER_DESIRE_FINISH_ACTION, finishAction, false);

    DesireGetInPosition* getInPosition
        = new (8, false) DesireGetInPosition(FIELDER_DESIRE_GET_IN_POSITION, (void*)TransDesireBallOwner);
    AddState(FIELDER_DESIRE_GET_IN_POSITION, getInPosition, false);

    DesireGetOpen* getOpen
        = new (8, false) DesireGetOpen(FIELDER_DESIRE_GET_OPEN, (void*)TransDesireGetOpen);
    AddState(FIELDER_DESIRE_GET_OPEN, getOpen, false);

    DesireHit* hit = new (8, false) DesireHit(FIELDER_DESIRE_HIT, (void*)TransDesireActionDone);
    AddState(FIELDER_DESIRE_HIT, hit, false);

    DesireInterceptBall* interceptBall = new (8, false) DesireInterceptBall(FIELDER_DESIRE_INTERCEPT_BALL);
    AddState(FIELDER_DESIRE_INTERCEPT_BALL, interceptBall, false);

    DesireMark* mark
        = new (8, false) DesireMark(FIELDER_DESIRE_MARK, (void*)TransDesireBallOwner);
    AddState(FIELDER_DESIRE_MARK, mark, false);

    DesireMegaStrike* megaStrike = new (8, false) DesireMegaStrike(FIELDER_DESIRE_MEGA_STRIKE);
    AddState(FIELDER_DESIRE_MEGA_STRIKE, megaStrike, false);

    DesirePass* pass
        = new (8, false) DesirePass(FIELDER_DESIRE_PASS, (void*)TransDesireActionDone);
    AddState(FIELDER_DESIRE_PASS, pass, false);

    DesirePreparePass* preparePass
        = new (8, false) DesirePreparePass(FIELDER_DESIRE_PREPARE_PASS, (void*)TransDesireNotBallOwner);
    AddState(FIELDER_DESIRE_PREPARE_PASS, preparePass, false);

    DesireReceivePass* receivePass = new (8, false) DesireReceivePass();
    AddState(FIELDER_DESIRE_RECEIVE_PASS, receivePass, false);

    DesireRunToNet* runToNet = new (8, false) DesireRunToNet();
    AddState(FIELDER_DESIRE_RUN_TO_NET, runToNet, false);

    DesireRunUpfield* runUpfield
        = new (8, false) DesireRunUpfield(FIELDER_DESIRE_RUN_UPFIELD, (void*)TransDesireBallOwner);
    AddState(FIELDER_DESIRE_RUN_UPFIELD, runUpfield, false);

    DesireRunDownfield* runDownfield
        = new (8, false) DesireRunDownfield(FIELDER_DESIRE_RUN_DOWNFIELD, (void*)TransDesireBallOwner);
    AddState(FIELDER_DESIRE_RUN_DOWNFIELD, runDownfield, false);

    DesireRunInDirection* runInDirection
        = new (8, false) DesireRunInDirection(FIELDER_DESIRE_RUN_IN_DIRECTION, (void*)TransDesireBallOwner);
    AddState(FIELDER_DESIRE_RUN_IN_DIRECTION, runInDirection, false);

    DesireRunToTarget* runToTarget
        = new (8, false) DesireRunToTarget(FIELDER_DESIRE_RUN_TO_TARGET, (void*)TransDesireRunToTarget);
    AddState(FIELDER_DESIRE_RUN_TO_TARGET, runToTarget, false);

    DesireShoot* shoot
        = new (8, false) DesireShoot(FIELDER_DESIRE_SHOOT, (void*)TransDesireNotBallOwner);
    AddState(FIELDER_DESIRE_SHOOT, shoot, false);

    DesireSlideAttack* slideAttack = new (8, false) DesireSlideAttack();
    AddState(FIELDER_DESIRE_SLIDE_ATTACK, slideAttack, false);

    DesireUserControlled* userControlled
        = new (8, false) DesireUserControlled();
    AddState(FIELDER_DESIRE_USER_CONTROLLED, userControlled, false);

    DesireWait* wait = new (8, false) DesireWait(FIELDER_DESIRE_WAIT);
    AddState(FIELDER_DESIRE_WAIT, wait, false);

    DesireWindupShot* windupShot = new (8, false) DesireWindupShot(FIELDER_DESIRE_WINDUP_SHOT);
    AddState(FIELDER_DESIRE_WINDUP_SHOT, windupShot, false);

    DesireStar* star = new (8, false) DesireStar(FIELDER_DESIRE_STAR);
    AddState(FIELDER_DESIRE_STAR, star, true);

    DesireMushroom* mushroom = new (8, false) DesireMushroom(FIELDER_DESIRE_MUSHROOM);
    AddState(FIELDER_DESIRE_MUSHROOM, mushroom, true);

    DesireSlippery* slippery = new (8, false) DesireSlippery(FIELDER_DESIRE_SLIPPERY);
    AddState(FIELDER_DESIRE_SLIPPERY, slippery, true);

    DesireGooey* gooey = new (8, false) DesireGooey();
    AddState(FIELDER_DESIRE_GOOEY, gooey, true);

    DesireShrink* shrink = new (8, false) DesireShrink(FIELDER_DESIRE_SHRINK);
    AddState(FIELDER_DESIRE_SHRINK, shrink, true);

    DesireFrozen* frozen = new (8, false) DesireFrozen(FIELDER_DESIRE_FROZEN);
    AddState(FIELDER_DESIRE_FROZEN, frozen, true);

    DesireConfused* confused = new (8, false) DesireConfused(FIELDER_DESIRE_CONFUSED);
    AddState(FIELDER_DESIRE_CONFUSED, confused, true);

    DesireSuperPower* superPower = new (8, false) DesireSuperPower();
    AddState(FIELDER_DESIRE_SUPER_POWER, superPower, true);

    DesireUsePowerup* usePowerup = new (8, false) DesireUsePowerup();
    AddState(FIELDER_DESIRE_USE_POWERUP, usePowerup, true);

    DesireSteering* steering = new (8, false) DesireSteering();
    AddState(FIELDER_DESIRE_STEERING, steering, true);

    DesireWaluigiWall* desire35 = new (8, false) DesireWaluigiWall();
    AddState(FIELDER_DESIRE_WALUIGI_WALL, desire35, true);
}

/**
 * Offset/Address/Size: 0xCB8 | 0x800D5AE4 | size: 0x54
 */
void FielderDesireMachine::Reset(bool deleting)
{
    ScriptMachine::Reset(deleting);
    if (!deleting)
    {
        ActivateConcurrentState(this, FIELDER_DESIRE_STEERING, 0, false);
    }
}

/**
 * Offset/Address/Size: 0xD0C | 0x800D5B38 | size: 0x2C0
 */
void FielderDesireMachine::Update(float deltaTime)
{
    Desire* frozen = GetFielderDesire(GetFielder(), FIELDER_DESIRE_FROZEN);
    if (frozen->IsActive())
    {
        DesireUpdate result;
        UpdateStateMachine(frozen, &result, true, deltaTime);
        if (result.mData.pointer != 0)
        {
            RequestStateMachineDeactivation(frozen);
        }
        return;
    }

    bool forceUserControl = gForceUserControl
        || (gForceHomeUserControl && GetFielder()->m_pTeam->m_nSide == HOME)
        || (gForceAwayUserControl && GetFielder()->m_pTeam->m_nSide == AWAY);
    bool waitForController = false;
    if (forceUserControl)
    {
        bool hasController = GetFielder()->GetGlobalPad();
        if (!hasController)
        {
            waitForController = true;
        }
    }

    if (!IsConcurrentStateActive(this, FIELDER_DESIRE_STEERING))
    {
        ActivateConcurrentState(this, FIELDER_DESIRE_STEERING, 0, false);
    }

    if (!waitForController && g_pGame->IsGameplayOrOvertime()
        && !UserControlledT(GetFielder()->m_pTeam)
        && !IsConcurrentStateActive(this, FIELDER_DESIRE_USE_POWERUP) && fn_800D85F8(GetFielder()))
    {
        FuzzyVariantCollection params;
        params.Set(10, FuzzyVariant(FT_POINTER, (void*)TransDesireUsePowerup));
        ActivateConcurrentState(this, FIELDER_DESIRE_USE_POWERUP, &params, false);
    }

    ScriptMachine::Update(deltaTime);
    if (GetFielder()->m_eActionState == ACTION_NEED_ACTION)
    {
        GetFielder()->StartRunning();
    }
}

/**
 * Offset/Address/Size: 0xFCC | 0x800D5DF8 | size: 0x3C4
 */
void FielderDesireMachine::SelectState()
{
    int state = 0;
    FuzzyVariantCollection params;
    cFielder* fielder = GetFielder();

    if (gForceUserControl
        || (gForceHomeUserControl && fielder->m_pTeam->m_nSide == HOME)
        || (gForceAwayUserControl && fielder->m_pTeam->m_nSide == AWAY))
    {
        bool hasController = fielder->GetGlobalPad();
        if (!hasController)
        {
            DeactivateConcurrentStates(this);
            state = FIELDER_DESIRE_WAIT;
        }
        else
        {
            state = FIELDER_DESIRE_USER_CONTROLLED;
        }
    }
    else if (g_pGame->m_eGameState == GS_KICKOFF)
    {
        state = FIELDER_DESIRE_WAIT;
    }
    else if (g_pGame->IsGameplayOrOvertime())
    {
        bool hasController = fielder->GetGlobalPad();
        if (hasController)
        {
            state = FIELDER_DESIRE_USER_CONTROLLED;
        }
        else
        {
            cFielder* outOfBoundsFielder = GetFielder();
            bool shouldRunToTarget;
            if ((outOfBoundsFielder->m_DetChar.m_v3Position.x > 20.6f
                    || outOfBoundsFielder->m_DetChar.m_v3Position.x < -20.6f)
                && !Incapacitated(outOfBoundsFielder)
                && !outOfBoundsFielder->IsInFallAction()
                && !outOfBoundsFielder->IsShattered())
            {
                shouldRunToTarget = true;
            }
            else
            {
                shouldRunToTarget = false;
            }
            if (shouldRunToTarget)
            {
                state = FIELDER_DESIRE_RUN_TO_TARGET;
                params.Set(7, FuzzyVariant(gBehindGoalLineRunTimeLimit));
                params.Set(13, FuzzyVariant(gBehindGoalLineRunSpeed));
                params.Set(2, FuzzyVariant(gBehindGoalLineRunAvoidanceCoeff[0]));

                nlVector3 position = gFielderDesireZeroVector;
                position.x = GetFielder()->m_DetChar.m_v3Position.x;
                position.x -= 10.0f * AIsgn(position.x);
                params.Set(14, FuzzyVariant(FT_VECTOR, position));
            }
        }
    }
    else if (g_pGame->GetGameState() == GS_POST_GOAL)
    {
        cTeam* team = fielder->m_pTeam;
        FormationSpec* formation;
        if (g_pGame->IsLastTeamToScore(team->m_nSide))
        {
            formation = FormationManager::GetFormationSpec(
                (eFormation)nlStringHash(gKickoffNeutralName));
        }
        else
        {
            formation = FormationManager::GetFormationSpec(
                (eFormation)nlStringHash(gKickoffAdvantageName));
        }

        nlVector3 position;
        formation->m_Positions[GetFielder()->m_DetPlayer.m_ID].GetLocationForTeam(
            *(nlVector2*)&position, team->m_nSide);
        position.z = 0.0f;
        state = FIELDER_DESIRE_RUN_TO_TARGET;
        params.Set(14, FuzzyVariant(FT_VECTOR, position));
    }

    if (state != 0)
    {
        ActivateState(state, &params, true);
    }
    else
    {
        ScriptMachine::SelectState();
    }
}

/**
 * Offset/Address/Size: 0x1390 | 0x800D61BC | size: 0x18
 */
shdStateMachine* FielderDesireMachine::ActivateState(
    int state, FuzzyVariantCollection* params, bool force)
{
    if (state == FIELDER_DESIRE_USE_POWERUP)
    {
        return 0;
    }
    return ScriptMachine::ActivateState(
        state, params, force);
}

/**
 * Offset/Address/Size: 0x13A8 | 0x800D61D4 | size: 0x5C
 */
void FielderDesireMachine::DeactivateState()
{
    if (mActiveState != 0)
    {
        DeactivateStateMachine(mActiveState, true);
        if (mActiveState->GetState() != FIELDER_DESIRE_FINISH_ACTION)
        {
            mPreviousState = mActiveState;
        }
    }
    mActiveState = 0;
}

/**
 * Offset/Address/Size: 0x1404 | 0x800D6230 | size: 0xC
 */
void FielderDesireMachine::OnBudgetCheckFailed()
{
    GetFielder()->StartRunning();
}

/**
 * Offset/Address/Size: 0x1410 | 0x800D623C | size: 0x5C
 */
DesireDoNothing::~DesireDoNothing()
{
}
