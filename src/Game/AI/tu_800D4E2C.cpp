#include "Game/AI/FielderDesireMachine.h"
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
#include "Game/AI/FuzzyVariant.h"
#include "Game/AI/Scripts/ScriptQuestions.h"
#include "Game/Formation.h"
#include "Game/Game.h"
#include "Game/Team.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"

struct UnidentifiedFielderContext
{
    u8 mUnidentified000[0x08];
    cFielder* mFielder;
};

extern "C" void fn_80316968(shdStateMachine*);
extern "C" void fn_80316980(shdStateMachine*, bool);
extern "C" void fn_80317010(
    shdStateMachine*, UnidentifiedVariant_80054AB8*, bool, float);
extern "C" void fn_80319E84(
    UnidentifiedScriptMachine*, int, UnidentifiedVariantCollection*, bool);
extern "C" bool fn_80319FEC(UnidentifiedScriptMachine*, int);

extern const nlVector3 lbl_804DC388;

char lbl_80502C28[] = "Kickoff Neutral";
char lbl_80502C38[] = "Kickoff Advantage";

float lbl_806DC3B0 = 2.0f;
float lbl_806DC3B4 = 2.0f;
float lbl_806DC3B8[2] = { 0.4f, 0.0f };
int lbl_806DC3C0 = 1;
int lbl_806DC3C4 = 0;
int lbl_806DC3C8 = 1;
int lbl_806DC3CC = 0;
int lbl_806DC3D0 = 1;
int lbl_806DC3D4 = 0;

extern "C" UnidentifiedVariant_80054AB8 fn_800D6298(
    UnidentifiedFielderContext*);
extern "C" UnidentifiedVariant_80054AB8 fn_800D6330(
    UnidentifiedFielderContext*);
extern "C" UnidentifiedVariant_80054AB8 fn_800D63C8(
    UnidentifiedFielderContext*);
extern "C" UnidentifiedVariant_80054AB8 fn_800B38AC(
    UnidentifiedFielderContext*);
extern "C" UnidentifiedVariant_80054AB8 fn_800B4DC0(
    UnidentifiedFielderContext*);
extern "C" UnidentifiedVariant_80054AB8 fn_800B7B1C(
    AIContext*);

class UnidentifiedDesire33 : public Desire
{
public:
    UnidentifiedDesire33(TransitionFunc transition)
        : Desire(33, transition)
    {
    }

    virtual ~UnidentifiedDesire33();
};

inline cFielder* UnidentifiedFielderDesireMachine::GetFielder() const
{
    return static_cast<cFielder*>(mAIContext->mData.pPlayer);
}

/**
 * Offset/Address/Size: 0x0 | 0x800D4E2C | size: 0x4C
 */
UnidentifiedFielderDesireMachine::UnidentifiedFielderDesireMachine()
    : UnidentifiedScriptMachine(36, true, false, 0)
{
}

/**
 * Offset/Address/Size: 0x4C | 0x800D4E78 | size: 0x58
 */
UnidentifiedFielderDesireMachine::~UnidentifiedFielderDesireMachine()
{
}

/**
 * Offset/Address/Size: 0xA4 | 0x800D4ED0 | size: 0xC14
 */
void UnidentifiedFielderDesireMachine::UnidentifiedVirtual2()
{
    UnidentifiedScriptMachine::UnidentifiedVirtual2();

    TransitionFunc cutAndBreakTransition;
    DesireCutAndBreak* cutAndBreak = new (nlMalloc(
        sizeof(DesireCutAndBreak), 8, false))
        DesireCutAndBreak(1,
            *BindNativeTransitionFunc(&cutAndBreakTransition, (void*)fn_800D6298));
    UnidentifiedAddState(1, cutAndBreak, false);

    TransitionFunc defendPosTransition;
    DesireDefendPos* defendPos = new (nlMalloc(
        sizeof(DesireDefendPos), 8, false))
        DesireDefendPos(2,
            *BindNativeTransitionFunc(&defendPosTransition, (void*)fn_800B7B1C));
    UnidentifiedAddState(2, defendPos, false);

    TransitionFunc dekeTransition;
    DesireDeke* deke = new (nlMalloc(sizeof(DesireDeke), 8, false))
        DesireDeke(
            3, *BindNativeTransitionFunc(&dekeTransition, (void*)fn_800D63C8));
    UnidentifiedAddState(3, deke, false);

    UnidentifiedDesire33* desire33 = new (nlMalloc(
        sizeof(UnidentifiedDesire33), 8, false))
        UnidentifiedDesire33(TransitionFunc(g_UnsetTransitionFunc));
    UnidentifiedAddState(33, desire33, false);

    TransitionFunc finishActionTransition;
    DesireFinishAction* finishAction = new (nlMalloc(
        sizeof(DesireFinishAction), 8, false))
        DesireFinishAction(21,
            *BindNativeTransitionFunc(&finishActionTransition, (void*)fn_800D63C8));
    UnidentifiedAddState(21, finishAction, false);

    TransitionFunc getInPositionTransition;
    DesireGetInPosition* getInPosition = new (nlMalloc(
        sizeof(DesireGetInPosition), 8, false))
        DesireGetInPosition(4,
            *BindNativeTransitionFunc(&getInPositionTransition, (void*)fn_800D6298));
    UnidentifiedAddState(4, getInPosition, false);

    TransitionFunc getOpenTransition;
    DesireGetOpen* getOpen = new (nlMalloc(
        sizeof(DesireGetOpen), 8, false))
        DesireGetOpen(
            5, *BindNativeTransitionFunc(&getOpenTransition, (void*)fn_800B4DC0));
    UnidentifiedAddState(5, getOpen, false);

    TransitionFunc hitTransition;
    DesireHit* hit = new (nlMalloc(sizeof(DesireHit), 8, false))
        DesireHit(
            6, *BindNativeTransitionFunc(&hitTransition, (void*)fn_800D63C8));
    UnidentifiedAddState(6, hit, false);

    DesireInterceptBall* interceptBall = new (nlMalloc(
        sizeof(DesireInterceptBall), 8, false))
        DesireInterceptBall(
            7, ScriptTransitionFunc("TransDesireInterceptBall"));
    UnidentifiedAddState(7, interceptBall, false);

    TransitionFunc markTransition;
    DesireMark* mark = new (nlMalloc(sizeof(DesireMark), 8, false))
        DesireMark(
            8, *BindNativeTransitionFunc(&markTransition, (void*)fn_800D6298));
    UnidentifiedAddState(8, mark, false);

    DesireMegaStrike* megaStrike = new (nlMalloc(
        sizeof(DesireMegaStrike), 8, false))
        DesireMegaStrike(
            32, ScriptTransitionFunc("TransDesireMegastrikeMeter"));
    UnidentifiedAddState(32, megaStrike, false);

    TransitionFunc passTransition;
    DesirePass* pass = new (nlMalloc(sizeof(DesirePass), 8, false))
        DesirePass(
            14, *BindNativeTransitionFunc(&passTransition, (void*)fn_800D63C8));
    UnidentifiedAddState(14, pass, false);

    TransitionFunc preparePassTransition;
    DesirePreparePass* preparePass = new (nlMalloc(
        sizeof(DesirePreparePass), 8, false))
        DesirePreparePass(18,
            *BindNativeTransitionFunc(&preparePassTransition, (void*)fn_800D6330));
    UnidentifiedAddState(18, preparePass, false);

    DesireReceivePass* receivePass = new (nlMalloc(
        sizeof(DesireReceivePass), 8, false)) DesireReceivePass();
    UnidentifiedAddState(22, receivePass, false);

    DesireRunToNet* runToNet = new (nlMalloc(
        sizeof(DesireRunToNet), 8, false))
        DesireRunToNet(ScriptTransitionFunc("TransDesireRunToNet"));
    UnidentifiedAddState(9, runToNet, false);

    TransitionFunc runUpfieldTransition;
    DesireRunUpfield* runUpfield = new (nlMalloc(
        sizeof(DesireRunUpfield), 8, false))
        DesireRunUpfield(10,
            *BindNativeTransitionFunc(&runUpfieldTransition, (void*)fn_800D6298));
    UnidentifiedAddState(10, runUpfield, false);

    TransitionFunc runDownfieldTransition;
    DesireRunDownfield* runDownfield = new (nlMalloc(
        sizeof(DesireRunDownfield), 8, false))
        DesireRunDownfield(11,
            *BindNativeTransitionFunc(&runDownfieldTransition, (void*)fn_800D6298));
    UnidentifiedAddState(11, runDownfield, false);

    TransitionFunc runInDirectionTransition;
    DesireRunInDirection* runInDirection = new (nlMalloc(
        sizeof(DesireRunInDirection), 8, false))
        DesireRunInDirection(12,
            *BindNativeTransitionFunc(&runInDirectionTransition, (void*)fn_800D6298));
    UnidentifiedAddState(12, runInDirection, false);

    TransitionFunc runToTargetTransition;
    DesireRunToTarget* runToTarget = new (nlMalloc(
        sizeof(DesireRunToTarget), 8, false))
        DesireRunToTarget(13,
            *BindNativeTransitionFunc(&runToTargetTransition, (void*)fn_800B38AC));
    UnidentifiedAddState(13, runToTarget, false);

    TransitionFunc shootTransition;
    DesireShoot* shoot = new (nlMalloc(sizeof(DesireShoot), 8, false))
        DesireShoot(
            15, *BindNativeTransitionFunc(&shootTransition, (void*)fn_800D6330));
    UnidentifiedAddState(15, shoot, false);

    DesireSlideAttack* slideAttack = new (nlMalloc(
        sizeof(DesireSlideAttack), 8, false))
        DesireSlideAttack(TransitionFunc(g_UnsetTransitionFunc));
    UnidentifiedAddState(16, slideAttack, false);

    DesireUserControlled* userControlled = new (nlMalloc(
        sizeof(DesireUserControlled), 8, false))
        DesireUserControlled(TransitionFunc(g_UnsetTransitionFunc));
    UnidentifiedAddState(20, userControlled, false);

    DesireWait* wait = new (nlMalloc(sizeof(DesireWait), 8, false))
        DesireWait(31, TransitionFunc(g_UnsetTransitionFunc));
    UnidentifiedAddState(31, wait, false);

    DesireWindupShot* windupShot = new (nlMalloc(
        sizeof(DesireWindupShot), 8, false))
        DesireWindupShot(19, ScriptTransitionFunc("TransDesireWindup"));
    UnidentifiedAddState(19, windupShot, false);

    DesireStar* star = new (nlMalloc(sizeof(DesireStar), 8, false))
        DesireStar(24, TransitionFunc(g_UnsetTransitionFunc));
    UnidentifiedAddState(24, star, true);

    DesireMushroom* mushroom = new (nlMalloc(
        sizeof(DesireMushroom), 8, false))
        DesireMushroom(25, TransitionFunc(g_UnsetTransitionFunc));
    UnidentifiedAddState(25, mushroom, true);

    DesireSlippery* slippery = new (nlMalloc(
        sizeof(DesireSlippery), 8, false))
        DesireSlippery(26, TransitionFunc(g_UnsetTransitionFunc));
    UnidentifiedAddState(26, slippery, true);

    DesireGooey* gooey = new (nlMalloc(
        sizeof(DesireGooey), 8, false)) DesireGooey();
    UnidentifiedAddState(27, gooey, true);

    DesireShrink* shrink = new (nlMalloc(
        sizeof(DesireShrink), 8, false))
        DesireShrink(28, TransitionFunc(g_UnsetTransitionFunc));
    UnidentifiedAddState(28, shrink, true);

    DesireFrozen* frozen = new (nlMalloc(
        sizeof(DesireFrozen), 8, false))
        DesireFrozen(29, TransitionFunc(g_UnsetTransitionFunc));
    UnidentifiedAddState(29, frozen, true);

    DesireConfused* confused = new (nlMalloc(
        sizeof(DesireConfused), 8, false))
        DesireConfused(30, TransitionFunc(g_UnsetTransitionFunc));
    UnidentifiedAddState(30, confused, true);

    DesireSuperPower* superPower = new (nlMalloc(
        sizeof(DesireSuperPower), 8, false)) DesireSuperPower();
    UnidentifiedAddState(23, superPower, true);

    DesireUsePowerup* usePowerup = new (nlMalloc(
        sizeof(DesireUsePowerup), 8, false))
        DesireUsePowerup(TransitionFunc(g_UnsetTransitionFunc));
    UnidentifiedAddState(17, usePowerup, true);

    DesireSteering* steering = new (nlMalloc(
        sizeof(DesireSteering), 8, false)) DesireSteering();
    UnidentifiedAddState(34, steering, true);

    UnidentifiedDesire35* desire35 = new (nlMalloc(
        sizeof(UnidentifiedDesire35), 8, false))
        UnidentifiedDesire35(TransitionFunc(g_UnsetTransitionFunc));
    UnidentifiedAddState(35, desire35, true);
}

/**
 * Offset/Address/Size: 0xCB8 | 0x800D5AE4 | size: 0x54
 */
void UnidentifiedFielderDesireMachine::Reset(bool param)
{
    UnidentifiedScriptMachine::Reset(param);
    if (!param)
    {
        fn_80319E84(this, 34, 0, false);
    }
}

/**
 * Offset/Address/Size: 0xD0C | 0x800D5B38 | size: 0x2C0
 */
void UnidentifiedFielderDesireMachine::Update(float deltaTime)
{
    Desire* frozen = fn_8002E08C(GetFielder(), 29);
    if (frozen->UnidentifiedIsActive())
    {
        UnidentifiedVariant_80054AB8 result;
        fn_80317010(frozen, &result, true, deltaTime);
        if (result.mData.pointer != 0)
        {
            fn_80316968(frozen);
        }
        return;
    }

    bool kickoffOverride = lbl_806E0C50
        || (lbl_806E0C51 && GetFielder()->m_pTeam->m_nSide == HOME)
        || (lbl_806E0C52 && GetFielder()->m_pTeam->m_nSide == AWAY);
    bool waitForController = false;
    if (kickoffOverride)
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
        params.Set(10, FuzzyVariant(FT_POINTER, (void*)fn_800D2074));
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
void UnidentifiedFielderDesireMachine::UnidentifiedVirtual7()
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
                params.Set(7, FuzzyVariant(lbl_806DC3B0));
                params.Set(13, FuzzyVariant(lbl_806DC3B4));
                params.Set(2, FuzzyVariant(lbl_806DC3B8[0]));

                nlVector3 position = lbl_804DC388;
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
        if ((int)g_pGame->m_nLastTeamToScore == team->m_nSide)
        {
            formation = FormationManager::GetFormationSpec(
                (eFormation)nlStringHash(lbl_80502C28));
        }
        else
        {
            formation = FormationManager::GetFormationSpec(
                (eFormation)nlStringHash(lbl_80502C38));
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
shdStateMachine* UnidentifiedFielderDesireMachine::UnidentifiedVirtual5(
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
void UnidentifiedFielderDesireMachine::UnidentifiedVirtual6()
{
    if (mUnidentified004 != 0)
    {
        fn_80316980(mUnidentified004, true);
        if (mUnidentified004->UnidentifiedGetState() != 21)
        {
            mUnidentified008 = mUnidentified004;
        }
    }
    mUnidentified004 = 0;
}

/**
 * Offset/Address/Size: 0x1404 | 0x800D6230 | size: 0xC
 */
void UnidentifiedFielderDesireMachine::UnidentifiedVirtual8()
{
    GetFielder()->StartRunning();
}

/**
 * Offset/Address/Size: 0x1410 | 0x800D623C | size: 0x5C
 */
UnidentifiedDesire33::~UnidentifiedDesire33()
{
}

/**
 * Offset/Address/Size: 0x146C | 0x800D6298 | size: 0x98
 */
extern "C" UnidentifiedVariant_80054AB8 fn_800D6298(
    UnidentifiedFielderContext* context)
{
    if (BallOwner(context->mFielder))
    {
        return UnidentifiedVariant_80054AB8(FT_INT, lbl_806DC3C0);
    }
    return UnidentifiedVariant_80054AB8(FT_INT, lbl_806DC3C4);
}

/**
 * Offset/Address/Size: 0x1504 | 0x800D6330 | size: 0x98
 */
extern "C" UnidentifiedVariant_80054AB8 fn_800D6330(
    UnidentifiedFielderContext* context)
{
    if (!BallOwner(context->mFielder))
    {
        return UnidentifiedVariant_80054AB8(FT_INT, lbl_806DC3C8);
    }
    return UnidentifiedVariant_80054AB8(FT_INT, lbl_806DC3CC);
}

/**
 * Offset/Address/Size: 0x159C | 0x800D63C8 | size: 0x94
 */
extern "C" UnidentifiedVariant_80054AB8 fn_800D63C8(
    UnidentifiedFielderContext* context)
{
    if (context->mFielder->IsActionDone())
    {
        return UnidentifiedVariant_80054AB8(FT_INT, lbl_806DC3D0);
    }
    return UnidentifiedVariant_80054AB8(FT_INT, lbl_806DC3D4);
}
