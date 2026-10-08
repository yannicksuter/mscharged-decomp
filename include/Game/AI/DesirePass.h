#ifndef GAME_AI_DESIREPASS_H
#define GAME_AI_DESIREPASS_H

#include "Game/AI/Desire.h"

class FuzzyRuntimeContext;

class DesirePreparePass : public Desire
{
public:
    DesirePreparePass(int state, void* function)
        : Desire(state, NativeTransitionFunc(function))
    {
    }

    virtual bool Initialize(void*);
    virtual void Cleanup();
    virtual void Update(DesireUpdate*, float);
    virtual inline void SyncLog(void*, DebugWriteCache*);
    virtual inline void RegisterDebugFields(void*, DebugWriteCache*);

private:
    cPlayer* mpPassTarget;
    bool mbVolleyPass;
    float mfAbortThreshold;
    SpaceSearch* m_pSpaceSearch;
};

class DesirePass : public Desire
{
public:
    DesirePass(int state, void* function)
        : Desire(state, NativeTransitionFunc(function))
    {
    }

    virtual bool Initialize(void*);
    virtual void Cleanup();
    virtual void Update(DesireUpdate*, float);
    virtual inline void SyncLog(void*, DebugWriteCache*);
    virtual inline void RegisterDebugFields(void*, DebugWriteCache*);

private:
    cPlayer* mpPassTarget;
    bool mbVolleyPass;
};

DesireUpdate TransDesireLooseBallContact(
    FuzzyRuntimeContext* fielder,
    FuzzyRuntimeContext* action);

#endif // GAME_AI_DESIREPASS_H
