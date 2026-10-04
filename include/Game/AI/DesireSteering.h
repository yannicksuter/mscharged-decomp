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
    PSS_TIMED_SEEKING = 3,
    PSS_UNIDENTIFIED_4 = 4,
};

class DesireSteering;

extern float gSteeringHistoryDuration;
extern bool gForceSidelineAvoidance;

void ResetSteeringHistory(DesireSteering*);
void ResetSteeringAvoidance(DesireSteering*);
void ResetSteeringTargets(DesireSteering*);
extern "C" void fn_800C5DBC(DesireSteering*, float);
void AddSteeringTarget(
    DesireSteering*, const nlVector3&, float, float);
void SetTimedSteeringTarget(
    DesireSteering*, const nlVector3&, unsigned short, float, float);
const nlVector3* GetSteeringTargetPosition(DesireSteering*);
void SeekTimedSteeringTarget(
    DesireSteering*, const nlVector3&, float, float);
void SeekSteeringTarget(
    DesireSteering*, const nlVector3&, eTurboRequest, float, float);
extern "C" void fn_800C6FDC(DesireSteering*, float);
eStrafeDirection GetSteeringStrafeDirection(
    DesireSteering*, unsigned short, unsigned short);

class DesireSteering : public Desire
{
    friend class cFielder;

public:
    DesireSteering();
    virtual ~DesireSteering();

    virtual bool Initialize(void*);
    virtual void Cleanup();
    virtual void Update(DesireUpdate*, float);
    virtual inline void UnidentifiedVirtual7(void*, DebugWriteCache*);
    virtual inline void UnidentifiedVirtual8(void*, DebugWriteCache*);

    void SetAvoidanceMultiplier(float fAvoidanceMult) { m_fAvoidanceMult = fAvoidanceMult; }

private:
    friend void ResetSteeringHistory(DesireSteering*);
    friend void ResetSteeringAvoidance(DesireSteering*);
    friend void ResetSteeringTargets(DesireSteering*);
    friend void fn_800C5DBC(DesireSteering*, float);
    friend void AddSteeringTarget(
        DesireSteering*, const nlVector3&, float, float);
    friend void SetTimedSteeringTarget(DesireSteering*, const nlVector3&,
        unsigned short, float, float);
    friend const nlVector3* GetSteeringTargetPosition(DesireSteering*);
    friend void SeekTimedSteeringTarget(
        DesireSteering*, const nlVector3&, float, float);
    friend void SeekSteeringTarget(DesireSteering*, const nlVector3&,
        eTurboRequest, float, float);
    friend void fn_800C6FDC(DesireSteering*, float);
    friend eStrafeDirection GetSteeringStrafeDirection(
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


class UnidentifiedDesire35 : public Desire
{
public:
    UnidentifiedDesire35()
        : Desire(35, UnsetTransitionFunc(g_UnsetTransitionFunc))
    {
    }

    virtual bool Initialize(void*);
    virtual void Cleanup();
    virtual void Update(DesireUpdate*, float);
};

#endif // GAME_AI_DESIRE_STEERING_H
