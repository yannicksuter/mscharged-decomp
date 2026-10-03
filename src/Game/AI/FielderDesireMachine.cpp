#include "Game/AI/FielderDesireMachine.h"
#include "Game/AI/FielderDesireTransitions.h"
#include "Game/AI/TeamPlayMachine.h"
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
        : Desire(33, UnsetTransitionFunc(g_UnsetTransitionFunc))
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
    : UnidentifiedScriptMachine(36, true, false, 0)
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
void FielderDesireMachine::UnidentifiedVirtual2()
{
    UnidentifiedScriptMachine::UnidentifiedVirtual2();

    DesireCutAndBreak* cutAndBreak
        = new (8, false) DesireCutAndBreak(1, (void*)TransDesireBallOwner);
    UnidentifiedAddState(1, cutAndBreak, false);

    DesireDefendPos* defendPos
        = new (8, false) DesireDefendPos(2, (void*)TransDesireDefendPos);
    UnidentifiedAddState(2, defendPos, false);

    DesireDeke* deke
        = new (8, false) DesireDeke(3, (void*)TransDesireActionDone);
    UnidentifiedAddState(3, deke, false);

    DesireDoNothing* doNothing = new (8, false) DesireDoNothing();
    UnidentifiedAddState(33, doNothing, false);

    DesireFinishAction* finishAction
        = new (8, false) DesireFinishAction(21, (void*)TransDesireActionDone);
    UnidentifiedAddState(21, finishAction, false);

    DesireGetInPosition* getInPosition
        = new (8, false) DesireGetInPosition(4, (void*)TransDesireBallOwner);
    UnidentifiedAddState(4, getInPosition, false);

    DesireGetOpen* getOpen
        = new (8, false) DesireGetOpen(5, (void*)fn_800B4DC0);
    UnidentifiedAddState(5, getOpen, false);

    DesireHit* hit = new (8, false) DesireHit(6, (void*)TransDesireActionDone);
    UnidentifiedAddState(6, hit, false);

    DesireInterceptBall* interceptBall = new (8, false) DesireInterceptBall(7);
    UnidentifiedAddState(7, interceptBall, false);

    DesireMark* mark
        = new (8, false) DesireMark(8, (void*)TransDesireBallOwner);
    UnidentifiedAddState(8, mark, false);

    DesireMegaStrike* megaStrike = new (8, false) DesireMegaStrike(32);
    UnidentifiedAddState(32, megaStrike, false);

    DesirePass* pass
        = new (8, false) DesirePass(14, (void*)TransDesireActionDone);
    UnidentifiedAddState(14, pass, false);

    DesirePreparePass* preparePass
        = new (8, false) DesirePreparePass(18, (void*)TransDesireNotBallOwner);
    UnidentifiedAddState(18, preparePass, false);

    DesireReceivePass* receivePass = new (8, false) DesireReceivePass();
    UnidentifiedAddState(22, receivePass, false);

    DesireRunToNet* runToNet = new (8, false) DesireRunToNet();
    UnidentifiedAddState(9, runToNet, false);

    DesireRunUpfield* runUpfield
        = new (8, false) DesireRunUpfield(10, (void*)TransDesireBallOwner);
    UnidentifiedAddState(10, runUpfield, false);

    DesireRunDownfield* runDownfield
        = new (8, false) DesireRunDownfield(11, (void*)TransDesireBallOwner);
    UnidentifiedAddState(11, runDownfield, false);

    DesireRunInDirection* runInDirection
        = new (8, false) DesireRunInDirection(12, (void*)TransDesireBallOwner);
    UnidentifiedAddState(12, runInDirection, false);

    DesireRunToTarget* runToTarget
        = new (8, false) DesireRunToTarget(13, (void*)fn_800B38AC);
    UnidentifiedAddState(13, runToTarget, false);

    DesireShoot* shoot
        = new (8, false) DesireShoot(15, (void*)TransDesireNotBallOwner);
    UnidentifiedAddState(15, shoot, false);

    DesireSlideAttack* slideAttack = new (8, false) DesireSlideAttack();
    UnidentifiedAddState(16, slideAttack, false);

    DesireUserControlled* userControlled
        = new (8, false) DesireUserControlled();
    UnidentifiedAddState(20, userControlled, false);

    DesireWait* wait = new (8, false) DesireWait(31);
    UnidentifiedAddState(31, wait, false);

    DesireWindupShot* windupShot = new (8, false) DesireWindupShot(19);
    UnidentifiedAddState(19, windupShot, false);

    DesireStar* star = new (8, false) DesireStar(24);
    UnidentifiedAddState(24, star, true);

    DesireMushroom* mushroom = new (8, false) DesireMushroom(25);
    UnidentifiedAddState(25, mushroom, true);

    DesireSlippery* slippery = new (8, false) DesireSlippery(26);
    UnidentifiedAddState(26, slippery, true);

    DesireGooey* gooey = new (8, false) DesireGooey();
    UnidentifiedAddState(27, gooey, true);

    DesireShrink* shrink = new (8, false) DesireShrink(28);
    UnidentifiedAddState(28, shrink, true);

    DesireFrozen* frozen = new (8, false) DesireFrozen(29);
    UnidentifiedAddState(29, frozen, true);

    DesireConfused* confused = new (8, false) DesireConfused(30);
    UnidentifiedAddState(30, confused, true);

    DesireSuperPower* superPower = new (8, false) DesireSuperPower();
    UnidentifiedAddState(23, superPower, true);

    DesireUsePowerup* usePowerup = new (8, false) DesireUsePowerup();
    UnidentifiedAddState(17, usePowerup, true);

    DesireSteering* steering = new (8, false) DesireSteering();
    UnidentifiedAddState(34, steering, true);

    UnidentifiedDesire35* desire35 = new (8, false) UnidentifiedDesire35();
    UnidentifiedAddState(35, desire35, true);
}

/**
 * Offset/Address/Size: 0xCB8 | 0x800D5AE4 | size: 0x54
 */
void FielderDesireMachine::Reset(bool deleting)
{
    UnidentifiedScriptMachine::Reset(deleting);
    if (!deleting)
    {
        fn_80319E84(this, 34, 0, false);
    }
}

/**
 * Offset/Address/Size: 0xD0C | 0x800D5B38 | size: 0x2C0
 */
void FielderDesireMachine::Update(float deltaTime)
{
    Desire* frozen = fn_8002E08C(GetFielder(), 29);
    if (frozen->IsActive())
    {
        UnidentifiedVariant_80054AB8 result;
        UpdateStateMachine(frozen, &result, true, deltaTime);
        if (result.mData.pointer != 0)
        {
            RequestStateMachineDeactivation(frozen);
        }
        return;
    }

    bool forceUserControl = lbl_806E0C50
        || (lbl_806E0C51 && GetFielder()->m_pTeam->m_nSide == HOME)
        || (lbl_806E0C52 && GetFielder()->m_pTeam->m_nSide == AWAY);
    bool waitForController = false;
    if (forceUserControl)
    {
        bool hasController = GetFielder()->GetGlobalPad();
        if (!hasController)
        {
            waitForController = true;
        }
    }

    if (!fn_80319FEC(this, 34))
    {
        fn_80319E84(this, 34, 0, false);
    }

    if (!waitForController && g_pGame->IsGameplayOrOvertime()
        && !UserControlledT(GetFielder()->m_pTeam)
        && !fn_80319FEC(this, 17) && fn_800D85F8(GetFielder()))
    {
        UnidentifiedVariantCollection params;
        params.Set(10, FuzzyVariant(FT_POINTER, (void*)TransDesireUsePowerup));
        fn_80319E84(this, 17, &params, false);
    }

    UnidentifiedScriptMachine::Update(deltaTime);
    if (GetFielder()->m_eActionState == ACTION_NEED_ACTION)
    {
        GetFielder()->StartRunning();
    }
}

/**
 * Offset/Address/Size: 0xFCC | 0x800D5DF8 | size: 0x3C4
 */
void FielderDesireMachine::UnidentifiedVirtual7()
{
    int state = 0;
    UnidentifiedVariantCollection params;
    cFielder* fielder = GetFielder();

    if (lbl_806E0C50
        || (lbl_806E0C51 && fielder->m_pTeam->m_nSide == HOME)
        || (lbl_806E0C52 && fielder->m_pTeam->m_nSide == AWAY))
    {
        bool hasController = fielder->GetGlobalPad();
        if (!hasController)
        {
            fn_80319DA0(this);
            state = 31;
        }
        else
        {
            state = 20;
        }
    }
    else if (g_pGame->m_eGameState == 1)
    {
        state = 31;
    }
    else if (g_pGame->IsGameplayOrOvertime())
    {
        bool hasController = fielder->GetGlobalPad();
        if (hasController)
        {
            state = 20;
        }
        else
        {
            cFielder* outOfBoundsFielder = GetFielder();
            bool shouldRunToTarget;
            if ((outOfBoundsFielder->mUnidentified024.m_v3Position.x > 20.6f
                    || outOfBoundsFielder->mUnidentified024.m_v3Position.x < -20.6f)
                && !Incapacitated(outOfBoundsFielder)
                && !outOfBoundsFielder->fn_800344B0()
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
                state = 13;
                params.Set(7, FuzzyVariant(gBehindGoalLineRunTimeLimit));
                params.Set(13, FuzzyVariant(gBehindGoalLineRunSpeed));
                params.Set(2, FuzzyVariant(gBehindGoalLineRunAvoidanceCoeff[0]));

                nlVector3 position = gFielderDesireZeroVector;
                position.x = GetFielder()->mUnidentified024.m_v3Position.x;
                position.x -= 10.0f * AIsgn(position.x);
                params.Set(14, FuzzyVariant(FT_VECTOR, position));
            }
        }
    }
    else if (g_pGame->GetGameState() == 2)
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
        formation->m_Positions[GetFielder()->mUnidentified1E4.m_ID].GetLocationForTeam(
            *(nlVector2*)&position, team->m_nSide);
        position.z = 0.0f;
        state = 13;
        params.Set(14, FuzzyVariant(FT_VECTOR, position));
    }

    if (state != 0)
    {
        UnidentifiedVirtual5(state, &params, true);
    }
    else
    {
        UnidentifiedScriptMachine::UnidentifiedVirtual7();
    }
}

/**
 * Offset/Address/Size: 0x1390 | 0x800D61BC | size: 0x18
 */
shdStateMachine* FielderDesireMachine::UnidentifiedVirtual5(
    int state, UnidentifiedVariantCollection* params, bool force)
{
    if (state == 17)
    {
        return 0;
    }
    return UnidentifiedScriptMachine::UnidentifiedVirtual5(
        state, params, force);
}

/**
 * Offset/Address/Size: 0x13A8 | 0x800D61D4 | size: 0x5C
 */
void FielderDesireMachine::UnidentifiedVirtual6()
{
    if (mUnidentified004 != 0)
    {
        DeactivateStateMachine(mUnidentified004, true);
        if (mUnidentified004->GetState() != 21)
        {
            mUnidentified008 = mUnidentified004;
        }
    }
    mUnidentified004 = 0;
}

/**
 * Offset/Address/Size: 0x1404 | 0x800D6230 | size: 0xC
 */
void FielderDesireMachine::UnidentifiedVirtual8()
{
    GetFielder()->StartRunning();
}

/**
 * Offset/Address/Size: 0x1410 | 0x800D623C | size: 0x5C
 */
DesireDoNothing::~DesireDoNothing()
{
}
