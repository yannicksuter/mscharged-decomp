#ifndef GAME_AI_DESIRE_H
#define GAME_AI_DESIRE_H

#include "NL/nlMath.h"
#include "NL/nlTimer.h"
#include "types.h"

class DebugWriteCache;
class cFielder;
class Variant;
struct UnidentifiedDesireUpdate;

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
    virtual void UnidentifiedUpdate(UnidentifiedDesireUpdate*) = 0;
    virtual void UnidentifiedReset();
    virtual void UnidentifiedSetContext(UnidentifiedDesireContext*);

protected:
    u8 mUnidentified004[0x08];
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
    virtual ~Desire();

    virtual bool UnidentifiedInitialize(void*);
    virtual bool UnidentifiedReinitialize(void*);
    virtual void UnidentifiedCleanup();
    virtual void UnidentifiedUpdate(UnidentifiedDesireUpdate*);
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
    virtual void UnidentifiedUpdate(UnidentifiedDesireUpdate*);
};

class DesireWait : public Desire
{
public:
    virtual ~DesireWait();

    virtual bool UnidentifiedInitialize(void*);
    virtual void UnidentifiedUpdate(UnidentifiedDesireUpdate*);
};

class DesireCutAndBreak : public Desire
{
public:
    virtual ~DesireCutAndBreak();

    virtual bool UnidentifiedInitialize(void*);
    virtual void UnidentifiedCleanup();
    virtual void UnidentifiedUpdate(UnidentifiedDesireUpdate*);
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
    virtual void UnidentifiedUpdate(UnidentifiedDesireUpdate*);
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
    virtual void UnidentifiedUpdate(UnidentifiedDesireUpdate*);
    virtual void UnidentifiedVirtual7(void*, DebugWriteCache*);
    virtual void UnidentifiedVirtual8(void*, DebugWriteCache*);
};

class DesireGetOpen : public Desire
{
public:
    virtual ~DesireGetOpen();

    virtual bool UnidentifiedInitialize(void*);
    virtual void UnidentifiedCleanup();
    virtual void UnidentifiedUpdate(UnidentifiedDesireUpdate*);
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
    virtual void UnidentifiedUpdate(UnidentifiedDesireUpdate*);
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
    virtual void UnidentifiedUpdate(UnidentifiedDesireUpdate*);
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
    virtual void UnidentifiedUpdate(UnidentifiedDesireUpdate*);
    virtual void UnidentifiedVirtual7(void*, DebugWriteCache*);
    virtual void UnidentifiedVirtual8(void*, DebugWriteCache*);
};

class DesireRunUpfield : public Desire
{
public:
    virtual ~DesireRunUpfield();

    virtual bool UnidentifiedInitialize(void*);
    virtual void UnidentifiedUpdate(UnidentifiedDesireUpdate*);
    virtual void UnidentifiedVirtual7(void*, DebugWriteCache*);
    virtual void UnidentifiedVirtual8(void*, DebugWriteCache*);
};

class DesireGetInPosition : public Desire
{
public:
    virtual ~DesireGetInPosition();

    virtual bool UnidentifiedInitialize(void*);
    virtual void UnidentifiedUpdate(UnidentifiedDesireUpdate*);
    virtual void UnidentifiedVirtual7(void*, DebugWriteCache*);
    virtual void UnidentifiedVirtual8(void*, DebugWriteCache*);
};

#endif // GAME_AI_DESIRE_H
