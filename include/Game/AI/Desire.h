#ifndef GAME_AI_DESIRE_H
#define GAME_AI_DESIRE_H

#include "Game/AI/DesireUpdate.h"
#include "Game/AI/TransitionFunc.h"
#include "Game/AI/shdStateMachine.h"
#include "Game/DebugWriteCache.h"
#include "NL/nlMath.h"
#include "NL/nlTimer.h"
#include "types.h"

class DebugWriteCache;
class AIContext;
class cFielder;
class cPlayer;
class cBall;
class Desire;
class SpaceSearch;
class UnidentifiedScriptMachine;
class FuzzyRuntimeBase;
typedef UnidentifiedVariant_80054AB8 DesireUpdate;

extern "C" Desire* fn_8002E08C(cFielder*, int);

class Desire : public shdStateMachine
{
public:
    Desire(int, TransitionFunc&);
    virtual ~Desire()
    {
    }

    virtual bool Initialize(void*);
    virtual bool Reinitialize(void*);
    virtual void Cleanup();
    virtual void Update(DesireUpdate*, float);
    virtual void SetContext(UnidentifiedScriptMachine*);
    virtual void UnidentifiedVirtual7(void*, DebugWriteCache*);
    virtual void UnidentifiedVirtual8(void*, DebugWriteCache*);

protected:
    cFielder* m_pFielder;
    nlVector3 mvDesiredPosition;
    int mTurboRequest;
    Timer mThinkTimer;
};

class DesireFinishAction : public Desire
{
public:
    DesireFinishAction(int state, void* function)
        : Desire(state, NativeTransitionFunc(function))
    {
    }

    virtual ~DesireFinishAction();

    virtual bool Initialize(void*);
    virtual void Update(DesireUpdate*, float);
};

class DesireWait : public Desire
{
public:
    DesireWait(int state)
        : Desire(state, UnsetTransitionFunc(g_UnsetTransitionFunc))
    {
    }

    virtual ~DesireWait();

    virtual bool Initialize(void*);
    virtual void Update(DesireUpdate*, float);
};

class DesireCutAndBreak : public Desire
{
public:
    DesireCutAndBreak(int state, void* function)
        : Desire(state, NativeTransitionFunc(function))
    {
    }

    virtual ~DesireCutAndBreak();

    virtual bool Initialize(void*);
    virtual void Cleanup();
    virtual void Update(DesireUpdate*, float);
    virtual void UnidentifiedVirtual7(void*, DebugWriteCache*);
    virtual void UnidentifiedVirtual8(void*, DebugWriteCache*);

private:
    void* mUnidentifiedA4;
};

class DesireDeke : public Desire
{
public:
    DesireDeke(int state, void* function)
        : Desire(state, NativeTransitionFunc(function))
    {
    }

    virtual ~DesireDeke();

    virtual bool Initialize(void*);
    virtual void Cleanup();
    virtual void Update(DesireUpdate*, float);
    virtual void UnidentifiedVirtual7(void*, DebugWriteCache*);
    virtual void UnidentifiedVirtual8(void*, DebugWriteCache*);

private:
    void* mUnidentifiedA4;
};

class DesireHit : public Desire
{
public:
    DesireHit(int state, void* function)
        : Desire(state, NativeTransitionFunc(function))
    {
    }

    virtual ~DesireHit();

    virtual bool Initialize(void*);
    virtual void Update(DesireUpdate*, float);
    virtual void UnidentifiedVirtual7(void*, DebugWriteCache*);
    virtual void UnidentifiedVirtual8(void*, DebugWriteCache*);
};

class DesireInterceptBall : public Desire
{
public:
    DesireInterceptBall(int state)
        : Desire(state, ScriptTransitionFunc("TransDesireInterceptBall"))
    {
    }

    virtual ~DesireInterceptBall();

    virtual bool Initialize(void*);
    virtual void Cleanup();
    virtual void Update(DesireUpdate*, float);
    virtual void UnidentifiedVirtual7(void*, DebugWriteCache*);
    virtual void UnidentifiedVirtual8(void*, DebugWriteCache*);

private:
    u8 mUnidentifiedA4[0x08];
};

extern "C" DesireUpdate fn_800B4DC0(AIContext* input);

class DesireGetOpen : public Desire
{
public:
    DesireGetOpen(int state, void* function)
        : Desire(state, NativeTransitionFunc(function))
    {
    }

    virtual ~DesireGetOpen();

    virtual bool Initialize(void*);
    virtual void Cleanup();
    virtual void Update(DesireUpdate*, float);
    virtual void UnidentifiedVirtual7(void*, DebugWriteCache*);
    virtual void UnidentifiedVirtual8(void*, DebugWriteCache*);

private:
    void* mUnidentifiedA4;
};

extern "C" DesireUpdate fn_800B38AC(AIContext* input, UnidentifiedFuzzyRuntimeValue* context);

class DesireRunToTarget : public Desire
{
public:
    DesireRunToTarget(int state, void* function)
        : Desire(state, NativeTransitionFunc(function))
    {
    }

    virtual ~DesireRunToTarget();

    virtual bool Initialize(void*);
    virtual void Cleanup();
    virtual void Update(DesireUpdate*, float);
    virtual void UnidentifiedVirtual7(void*, DebugWriteCache*);
    virtual void UnidentifiedVirtual8(void*, DebugWriteCache*);

    cBall* GetTargetBall() const { return m_pTargetBall; }

private:
    cFielder* m_pTargetFielder;
    cBall* m_pTargetBall;
    nlVector3 m_vTargetPos;
    int m_eDirection;
    float m_fDistOffset;
    float m_fUrgency;
    float m_fSpeedCoeff;
    float m_fAvoidanceCoeff;
};

class DesireRunInDirection : public Desire
{
public:
    float GetMaxDistance() const { return m_fMaxDistance; }
    float GetDistanceTravelled() const { return m_fDistTravelled; }
    cFielder* GetTarget() const { return m_pTarget; }
    DesireRunInDirection(int state, void* function)
        : Desire(state, NativeTransitionFunc(function))
    {
    }

    virtual ~DesireRunInDirection();

    virtual bool Initialize(void*);
    virtual void Cleanup();
    virtual void Update(DesireUpdate*, float);
    virtual void UnidentifiedVirtual7(void*, DebugWriteCache*);
    virtual void UnidentifiedVirtual8(void*, DebugWriteCache*);

private:
    unsigned short m_aDirection;
    float m_fMaxDistance;
    float m_fDistTravelled;
    float m_fSpeed;
    int m_eFieldDirection;
    cFielder* m_pTarget;
};

class DesireRunDownfield : public Desire
{
public:
    DesireRunDownfield(int state, void* function)
        : Desire(state, NativeTransitionFunc(function))
    {
    }

    virtual ~DesireRunDownfield();

    virtual bool Initialize(void*);
    virtual void Update(DesireUpdate*, float);
    virtual void UnidentifiedVirtual7(void*, DebugWriteCache*);
    virtual void UnidentifiedVirtual8(void*, DebugWriteCache*);
};

class DesireRunUpfield : public Desire
{
public:
    DesireRunUpfield(int state, void* function)
        : Desire(state, NativeTransitionFunc(function))
    {
    }

    virtual ~DesireRunUpfield();

    virtual bool Initialize(void*);
    virtual void Update(DesireUpdate*, float);
    virtual void UnidentifiedVirtual7(void*, DebugWriteCache*);
    virtual void UnidentifiedVirtual8(void*, DebugWriteCache*);
};

class DesireGetInPosition : public Desire
{
public:
    DesireGetInPosition(int state, void* function)
        : Desire(state, NativeTransitionFunc(function))
    {
    }

    virtual ~DesireGetInPosition();

    virtual bool Initialize(void*);
    virtual void Update(DesireUpdate*, float);
    virtual void UnidentifiedVirtual7(void*, DebugWriteCache*);
    virtual void UnidentifiedVirtual8(void*, DebugWriteCache*);
};

class DesireMark : public Desire
{
public:
    DesireMark(int state, void* function)
        : Desire(state, NativeTransitionFunc(function))
    {
    }

    virtual bool Initialize(void*);
    virtual void Update(DesireUpdate*, float);
    virtual inline void UnidentifiedVirtual7(void*, DebugWriteCache*);
    virtual inline void UnidentifiedVirtual8(void*, DebugWriteCache*);
};

DesireUpdate TransDesireDefendPos(AIContext* input);

class DesireDefendPos : public Desire
{
public:
    DesireDefendPos(int state, void* function)
        : Desire(state, NativeTransitionFunc(function))
    {
    }

    virtual bool Initialize(void*);
    virtual void Cleanup();
    virtual void Update(DesireUpdate*, float);
    virtual inline void UnidentifiedVirtual7(void*, DebugWriteCache*);
    virtual inline void UnidentifiedVirtual8(void*, DebugWriteCache*);
};

class DesireMegaStrike : public Desire
{
public:
    DesireMegaStrike(int state)
        : Desire(state, ScriptTransitionFunc("TransDesireMegastrikeMeter"))
    {
    }

    virtual inline ~DesireMegaStrike();

    virtual bool Initialize(void*);
    virtual void Cleanup();
    virtual void Update(DesireUpdate*, float);
    virtual inline void UnidentifiedVirtual7(void*, DebugWriteCache*);
    virtual inline void UnidentifiedVirtual8(void*, DebugWriteCache*);

private:
    bool UpdateAIButtonPress(DesireUpdate*, float);

    int mnRequestedBalls;
    float mfAccuracyScore;
    float mfPrevMeterPosition;
    float mfFirstPressDelay;
    int mnPressStage;
};

class DesireStar : public Desire
{
public:
    DesireStar(int state)
        : Desire(state, UnsetTransitionFunc(g_UnsetTransitionFunc))
    {
    }

    virtual ~DesireStar();

    virtual bool Initialize(void*);
    virtual bool Reinitialize(void*);
    virtual void Cleanup();
    virtual void Update(DesireUpdate*, float);
    virtual void UnidentifiedVirtual7(void*, DebugWriteCache*);
    virtual void UnidentifiedVirtual8(void*, DebugWriteCache*);
};

class DesireMushroom : public Desire
{
public:
    DesireMushroom(int state)
        : Desire(state, UnsetTransitionFunc(g_UnsetTransitionFunc))
    {
    }

    virtual ~DesireMushroom();

    virtual bool Initialize(void*);
    virtual bool Reinitialize(void*);
    virtual void Cleanup();
    virtual void Update(DesireUpdate*, float);
    virtual void UnidentifiedVirtual7(void*, DebugWriteCache*);
    virtual void UnidentifiedVirtual8(void*, DebugWriteCache*);
};

class DesireSlippery : public Desire
{
public:
    DesireSlippery(int state)
        : Desire(state, UnsetTransitionFunc(g_UnsetTransitionFunc))
    {
    }

    virtual ~DesireSlippery();

    virtual bool Initialize(void*);
    virtual bool Reinitialize(void*);
    virtual void Cleanup();
    virtual void Update(DesireUpdate*, float);
    virtual void UnidentifiedVirtual7(void*, DebugWriteCache*);
    virtual void UnidentifiedVirtual8(void*, DebugWriteCache*);
};

class DesireGooey : public Desire
{
public:
    DesireGooey();
    virtual ~DesireGooey();

    virtual bool Initialize(void*);
    virtual bool Reinitialize(void*);
    virtual void Cleanup();
    virtual void Update(DesireUpdate*, float);
    virtual void UnidentifiedVirtual7(void*, DebugWriteCache*);
    virtual void UnidentifiedVirtual8(void*, DebugWriteCache*);

    float fn_800BD1F0();

private:
    float mfGooPercentage;
    float mfMaxGooEffect;
    float mUnidentifiedAC;
    float mfGooTime;
    float mf_NotRunning_SpeedScale;
    float mf_NotRunning_MovementScale;
};

class DesireShrink : public Desire
{
public:
    DesireShrink(int state)
        : Desire(state, UnsetTransitionFunc(g_UnsetTransitionFunc))
    {
    }

    virtual ~DesireShrink();

    virtual bool Initialize(void*);
    virtual void Cleanup();
    virtual void Update(DesireUpdate*, float);
    virtual void UnidentifiedVirtual7(void*, DebugWriteCache*);
    virtual void UnidentifiedVirtual8(void*, DebugWriteCache*);

    float fn_800BD75C();

private:
    float mfSlowPercentage;
};

class DesireFrozen : public Desire
{
    friend class cFielder;

public:
    DesireFrozen(int state)
        : Desire(state, UnsetTransitionFunc(g_UnsetTransitionFunc))
    {
    }

    virtual ~DesireFrozen();

    virtual bool Initialize(void*);
    virtual bool Reinitialize(void*);
    virtual void Cleanup();
    virtual void Update(DesireUpdate*, float);
    virtual void UnidentifiedVirtual7(void*, DebugWriteCache*);
    virtual void UnidentifiedVirtual8(void*, DebugWriteCache*);

    void fn_800BE0BC(float, int);

    bool IsUnidentifiedState(int nState) const
    {
        return mActive && meFrozenState == nState;
    }

private:
    void fn_800BE1AC(int);

    int meFrozenState;
    float mfPrevFrozenTime;
    int mePrevFrozenState;
    int mePrevActionState;
    bool mbWasDazed;
};

class DesireConfused : public Desire
{
public:
    DesireConfused(int state)
        : Desire(state, UnsetTransitionFunc(g_UnsetTransitionFunc))
    {
    }

    virtual ~DesireConfused();

    virtual bool Initialize(void*);
    virtual bool Reinitialize(void*);
    virtual void Cleanup();
    virtual void Update(DesireUpdate*, float);
    virtual void UnidentifiedVirtual7(void*, DebugWriteCache*);
    virtual void UnidentifiedVirtual8(void*, DebugWriteCache*);

    void fn_800BED24(unsigned short*);

private:
    float mfConfusedPercentage;
    float mfConfusedDirection;
};

inline void Desire::UnidentifiedVirtual8(void*, DebugWriteCache* cache)
{
    cache->AddField(22, gDebugFieldTypes[22].size, 0, "mvDesiredPosition");
    cache->AddField(14, gDebugFieldTypes[14].size, (u8*)&mTurboRequest - (u8*)&mvDesiredPosition, "mTurboRequest");
    cache->AddField(20, gDebugFieldTypes[20].size, (u8*)&mThinkTimer - (u8*)&mvDesiredPosition, "mThinkTimer");
}

int GetStateMachineState(const shdStateMachine*);
UnidentifiedVariantCollection* GetStateMachineParameters(shdStateMachine*);
float GetRunInDirectionMaxDistance(const DesireRunInDirection*);
float GetRunInDirectionDistanceTravelled(const DesireRunInDirection*);

#endif // GAME_AI_DESIRE_H
