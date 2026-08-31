#ifndef GAME_AI_DESIRE_H
#define GAME_AI_DESIRE_H

#include "NL/nlMath.h"
#include "NL/nlTimer.h"
#include "types.h"

class DebugWriteCache;
class cFielder;
class cPlayer;
class Desire;
class SpaceSearch;
class Variant;
struct UnidentifiedDesireUpdate;

extern "C" Desire* fn_8002E08C(cFielder*, int);

struct UnidentifiedStateTransition
{
    int mUnidentifiedHash;
    void* mUnidentifiedFunction;
};

struct UnidentifiedDesireContext
{
    u8 mUnidentified000[0x64];
    Variant* mUnidentifiedValue;
};

class shdStateMachine
{
public:
    shdStateMachine(int, const UnidentifiedStateTransition&);
    virtual ~shdStateMachine();

    virtual bool UnidentifiedInitialize(void*) = 0;
    virtual bool UnidentifiedReinitialize(void*) = 0;
    virtual void UnidentifiedCleanup() = 0;
    virtual void UnidentifiedUpdate(UnidentifiedDesireUpdate*, float) = 0;
    virtual void UnidentifiedReset();
    virtual void UnidentifiedSetContext(UnidentifiedDesireContext*);

protected:
    int mUnidentifiedState;
    bool mUnidentifiedActive;
    u8 mPadding009[3];
    Timer mUnidentifiedTimer;
    u8 mUnidentified014[0x64];
    float mUnidentified078;
    float mUnidentified07C;
    float mUnidentified080;
    float mUnidentified084;
};

class Desire : public shdStateMachine
{
public:
    Desire(int, const UnidentifiedStateTransition&);
    virtual ~Desire()
    {
    }

    virtual bool UnidentifiedInitialize(void*);
    virtual bool UnidentifiedReinitialize(void*);
    virtual void UnidentifiedCleanup();
    virtual void UnidentifiedUpdate(UnidentifiedDesireUpdate*, float);
    virtual void UnidentifiedSetContext(UnidentifiedDesireContext*);
    virtual void UnidentifiedVirtual7(void*, DebugWriteCache*);
    virtual void UnidentifiedVirtual8(void*, DebugWriteCache*);

protected:
    cFielder* mUnidentifiedFielder;
    nlVector3 mvDesiredPosition;
    int mTurboRequest;
    Timer mThinkTimer;
};

class DesireFinishAction : public Desire
{
public:
    virtual ~DesireFinishAction();

    virtual bool UnidentifiedInitialize(void*);
    virtual void UnidentifiedUpdate(UnidentifiedDesireUpdate*, float);
};

class DesireWait : public Desire
{
public:
    virtual ~DesireWait();

    virtual bool UnidentifiedInitialize(void*);
    virtual void UnidentifiedUpdate(UnidentifiedDesireUpdate*, float);
};

class DesireCutAndBreak : public Desire
{
public:
    virtual ~DesireCutAndBreak();

    virtual bool UnidentifiedInitialize(void*);
    virtual void UnidentifiedCleanup();
    virtual void UnidentifiedUpdate(UnidentifiedDesireUpdate*, float);
    virtual void UnidentifiedVirtual7(void*, DebugWriteCache*);
    virtual void UnidentifiedVirtual8(void*, DebugWriteCache*);

private:
    void* mUnidentifiedA4;
};

class DesireDeke : public Desire
{
public:
    virtual ~DesireDeke();

    virtual bool UnidentifiedInitialize(void*);
    virtual void UnidentifiedCleanup();
    virtual void UnidentifiedUpdate(UnidentifiedDesireUpdate*, float);
    virtual void UnidentifiedVirtual7(void*, DebugWriteCache*);
    virtual void UnidentifiedVirtual8(void*, DebugWriteCache*);

private:
    void* mUnidentifiedA4;
};

class DesireHit : public Desire
{
public:
    virtual ~DesireHit();

    virtual bool UnidentifiedInitialize(void*);
    virtual void UnidentifiedUpdate(UnidentifiedDesireUpdate*, float);
    virtual void UnidentifiedVirtual7(void*, DebugWriteCache*);
    virtual void UnidentifiedVirtual8(void*, DebugWriteCache*);
};

class DesireGetOpen : public Desire
{
public:
    virtual ~DesireGetOpen();

    virtual bool UnidentifiedInitialize(void*);
    virtual void UnidentifiedCleanup();
    virtual void UnidentifiedUpdate(UnidentifiedDesireUpdate*, float);
    virtual void UnidentifiedVirtual7(void*, DebugWriteCache*);
    virtual void UnidentifiedVirtual8(void*, DebugWriteCache*);

private:
    void* mUnidentifiedA4;
};

class DesireRunToTarget : public Desire
{
public:
    virtual ~DesireRunToTarget();

    virtual bool UnidentifiedInitialize(void*);
    virtual void UnidentifiedCleanup();
    virtual void UnidentifiedUpdate(UnidentifiedDesireUpdate*, float);
    virtual void UnidentifiedVirtual7(void*, DebugWriteCache*);
    virtual void UnidentifiedVirtual8(void*, DebugWriteCache*);

private:
    cFielder* m_pTargetFielder;
    u8 mUnidentifiedA8[4];
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
    virtual ~DesireRunInDirection();

    virtual bool UnidentifiedInitialize(void*);
    virtual void UnidentifiedCleanup();
    virtual void UnidentifiedUpdate(UnidentifiedDesireUpdate*, float);
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
    virtual ~DesireRunDownfield();

    virtual bool UnidentifiedInitialize(void*);
    virtual void UnidentifiedUpdate(UnidentifiedDesireUpdate*, float);
    virtual void UnidentifiedVirtual7(void*, DebugWriteCache*);
    virtual void UnidentifiedVirtual8(void*, DebugWriteCache*);
};

class DesireRunUpfield : public Desire
{
public:
    virtual ~DesireRunUpfield();

    virtual bool UnidentifiedInitialize(void*);
    virtual void UnidentifiedUpdate(UnidentifiedDesireUpdate*, float);
    virtual void UnidentifiedVirtual7(void*, DebugWriteCache*);
    virtual void UnidentifiedVirtual8(void*, DebugWriteCache*);
};

class DesireGetInPosition : public Desire
{
public:
    virtual ~DesireGetInPosition();

    virtual bool UnidentifiedInitialize(void*);
    virtual void UnidentifiedUpdate(UnidentifiedDesireUpdate*, float);
    virtual void UnidentifiedVirtual7(void*, DebugWriteCache*);
    virtual void UnidentifiedVirtual8(void*, DebugWriteCache*);
};

class DesireMark : public Desire
{
public:
    virtual ~DesireMark();

    virtual bool UnidentifiedInitialize(void*);
    virtual void UnidentifiedUpdate(UnidentifiedDesireUpdate*, float);
    virtual void UnidentifiedVirtual7(void*, DebugWriteCache*);
    virtual void UnidentifiedVirtual8(void*, DebugWriteCache*);
};

class DesireDefendPos : public Desire
{
public:
    virtual ~DesireDefendPos();

    virtual bool UnidentifiedInitialize(void*);
    virtual void UnidentifiedCleanup();
    virtual void UnidentifiedUpdate(UnidentifiedDesireUpdate*, float);
    virtual void UnidentifiedVirtual7(void*, DebugWriteCache*);
    virtual void UnidentifiedVirtual8(void*, DebugWriteCache*);
};

class DesireMegaStrike : public Desire
{
public:
    virtual ~DesireMegaStrike();

    virtual bool UnidentifiedInitialize(void*);
    virtual void UnidentifiedCleanup();
    virtual void UnidentifiedUpdate(UnidentifiedDesireUpdate*, float);
    virtual void UnidentifiedVirtual7(void*, DebugWriteCache*);
    virtual void UnidentifiedVirtual8(void*, DebugWriteCache*);

private:
    bool fn_800B9D84(UnidentifiedDesireUpdate*, float);

    int mUnidentifiedA4;
    float mUnidentifiedA8;
    float mUnidentifiedAC;
    float mUnidentifiedB0;
    int mUnidentifiedB4;
};

class DesirePass : public Desire
{
public:
    virtual ~DesirePass();

    virtual bool UnidentifiedInitialize(void*);
    virtual void UnidentifiedCleanup();
    virtual void UnidentifiedUpdate(UnidentifiedDesireUpdate*, float);
    virtual void UnidentifiedVirtual7(void*, DebugWriteCache*);
    virtual void UnidentifiedVirtual8(void*, DebugWriteCache*);

private:
    cPlayer* mpPassTarget;
    bool mbVolleyPass;
};

class DesirePreparePass : public Desire
{
public:
    virtual ~DesirePreparePass();

    virtual bool UnidentifiedInitialize(void*);
    virtual void UnidentifiedCleanup();
    virtual void UnidentifiedUpdate(UnidentifiedDesireUpdate*, float);
    virtual void UnidentifiedVirtual7(void*, DebugWriteCache*);
    virtual void UnidentifiedVirtual8(void*, DebugWriteCache*);

private:
    cPlayer* mpPassTarget;
    bool mbVolleyPass;
    float mfAbortThreshold;
    SpaceSearch* m_pSpaceSearch;
};

class DesireStar : public Desire
{
public:
    virtual ~DesireStar();

    virtual bool UnidentifiedInitialize(void*);
    virtual bool UnidentifiedReinitialize(void*);
    virtual void UnidentifiedCleanup();
    virtual void UnidentifiedUpdate(UnidentifiedDesireUpdate*, float);
    virtual void UnidentifiedVirtual7(void*, DebugWriteCache*);
    virtual void UnidentifiedVirtual8(void*, DebugWriteCache*);
};

class DesireMushroom : public Desire
{
public:
    virtual ~DesireMushroom();

    virtual bool UnidentifiedInitialize(void*);
    virtual bool UnidentifiedReinitialize(void*);
    virtual void UnidentifiedCleanup();
    virtual void UnidentifiedUpdate(UnidentifiedDesireUpdate*, float);
    virtual void UnidentifiedVirtual7(void*, DebugWriteCache*);
    virtual void UnidentifiedVirtual8(void*, DebugWriteCache*);
};

class DesireSlippery : public Desire
{
public:
    virtual ~DesireSlippery();

    virtual bool UnidentifiedInitialize(void*);
    virtual bool UnidentifiedReinitialize(void*);
    virtual void UnidentifiedCleanup();
    virtual void UnidentifiedUpdate(UnidentifiedDesireUpdate*, float);
    virtual void UnidentifiedVirtual7(void*, DebugWriteCache*);
    virtual void UnidentifiedVirtual8(void*, DebugWriteCache*);
};

class DesireGooey : public Desire
{
public:
    DesireGooey();
    virtual ~DesireGooey();

    virtual bool UnidentifiedInitialize(void*);
    virtual bool UnidentifiedReinitialize(void*);
    virtual void UnidentifiedCleanup();
    virtual void UnidentifiedUpdate(UnidentifiedDesireUpdate*, float);
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
    virtual ~DesireShrink();

    virtual bool UnidentifiedInitialize(void*);
    virtual void UnidentifiedCleanup();
    virtual void UnidentifiedUpdate(UnidentifiedDesireUpdate*, float);
    virtual void UnidentifiedVirtual7(void*, DebugWriteCache*);
    virtual void UnidentifiedVirtual8(void*, DebugWriteCache*);

    float fn_800BD75C();

private:
    float mfSlowPercentage;
};

class DesireFrozen : public Desire
{
public:
    virtual ~DesireFrozen();

    virtual bool UnidentifiedInitialize(void*);
    virtual bool UnidentifiedReinitialize(void*);
    virtual void UnidentifiedCleanup();
    virtual void UnidentifiedUpdate(UnidentifiedDesireUpdate*, float);
    virtual void UnidentifiedVirtual7(void*, DebugWriteCache*);
    virtual void UnidentifiedVirtual8(void*, DebugWriteCache*);

    void fn_800BE0BC(float, int);

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
    virtual ~DesireConfused();

    virtual bool UnidentifiedInitialize(void*);
    virtual bool UnidentifiedReinitialize(void*);
    virtual void UnidentifiedCleanup();
    virtual void UnidentifiedUpdate(UnidentifiedDesireUpdate*, float);
    virtual void UnidentifiedVirtual7(void*, DebugWriteCache*);
    virtual void UnidentifiedVirtual8(void*, DebugWriteCache*);

    void fn_800BED24(unsigned short*);

private:
    float mfConfusedPercentage;
    float mfConfusedDirection;
};

#endif // GAME_AI_DESIRE_H
