#ifndef GAME_AI_DESIRE_STEERING_H
#define GAME_AI_DESIRE_STEERING_H

#include "Game/AI/AvoidController.h"
#include "Game/AI/Desire.h"
#include "Game/AI/Fielder.h"
#include "NL/nlPiecewiseLinearCurve.h"

enum ePositionSeekState
{
    PSS_ARRIVED = 0,
    PSS_NEAR_SEEKING = 1,
    PSS_FAR_SEEKING = 2,
    PSS_UNIDENTIFIED_3 = 3,
    PSS_UNIDENTIFIED_4 = 4,
};

class DesireSteering;

extern "C" void fn_800C574C(DesireSteering*);
extern "C" void fn_800C577C(DesireSteering*);
extern "C" void fn_800C5784(DesireSteering*);
extern "C" void fn_800C5DBC(DesireSteering*, float);
extern "C" void fn_800C60C4(
    DesireSteering*, const nlVector3&, float, float);
extern "C" void fn_800C61A4(
    DesireSteering*, const nlVector3&, unsigned short, float, float);
extern "C" const nlVector3* fn_800C61FC(DesireSteering*);
extern "C" void fn_800C6390(
    DesireSteering*, const nlVector3&, float, float);
extern "C" void fn_800C66A4(
    DesireSteering*, const nlVector3&, eTurboRequest, float, float);
extern "C" void fn_800C6FDC(DesireSteering*, float);
extern "C" eStrafeDirection fn_800C7348(
    DesireSteering*, unsigned short, unsigned short);

class DesireSteering : public Desire
{
    friend class cFielder;

public:
    DesireSteering();
    virtual ~DesireSteering();

    virtual bool UnidentifiedInitialize(void*);
    virtual void UnidentifiedCleanup();
    virtual void Update(UnidentifiedDesireUpdate*, float);
    virtual void UnidentifiedVirtual7(void*, DebugWriteCache*);
    virtual void UnidentifiedVirtual8(void*, DebugWriteCache*);

private:
    friend void fn_800C574C(DesireSteering*);
    friend void fn_800C577C(DesireSteering*);
    friend void fn_800C5784(DesireSteering*);
    friend void fn_800C5DBC(DesireSteering*, float);
    friend void fn_800C60C4(
        DesireSteering*, const nlVector3&, float, float);
    friend void fn_800C61A4(DesireSteering*, const nlVector3&,
        unsigned short, float, float);
    friend const nlVector3* fn_800C61FC(DesireSteering*);
    friend void fn_800C6390(
        DesireSteering*, const nlVector3&, float, float);
    friend void fn_800C66A4(DesireSteering*, const nlVector3&,
        eTurboRequest, float, float);
    friend void fn_800C6FDC(DesireSteering*, float);
    friend eStrafeDirection fn_800C7348(
        DesireSteering*, unsigned short, unsigned short);

    ePositionSeekState m_ePositionSeekState;
    AvoidController* m_pAvoidance;
    nlVector3 m_v3DesiredPos;
    nlVector3 m_v3LastDesiredPos;
    nlVector3 m_v3DesiredVel;
    nlVector3 m_v3TempDesiredPos;
    float m_fTotalWeight;
    float m_fUrgency;
    float m_fAvoidanceMult;
    int m_ThingsToAvoid;
    float m_fDesiredFacingDirection;
    float m_fFacingTotalWeight;
    float m_fDesiredArrivalTime;
    float m_fForcedArrivalRadius;
    UnidentifiedAvoidanceHistory m_AvoidanceHistory;
};

extern UnidentifiedUnsetTransition lbl_806E20B8;

class UnidentifiedDesire35 : public Desire
{
public:
    UnidentifiedDesire35()
        : Desire(35, UnidentifiedStateTransition(lbl_806E20B8))
    {
    }

    virtual ~UnidentifiedDesire35();
    virtual bool UnidentifiedInitialize(void*);
    virtual void UnidentifiedCleanup();
    virtual void Update(UnidentifiedDesireUpdate*, float);
};

#endif // GAME_AI_DESIRE_STEERING_H
