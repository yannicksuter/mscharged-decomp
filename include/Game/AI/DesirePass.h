#ifndef GAME_AI_DESIREPASS_H
#define GAME_AI_DESIREPASS_H

#include "Game/AI/Desire.h"

class UnidentifiedFuzzyRuntimeValue;

class DesirePreparePass : public Desire
{
public:
    DesirePreparePass(int state, void* function)
        : Desire(state, NativeTransitionFunc(function))
    {
    }

    virtual bool UnidentifiedInitialize(void*);
    virtual void UnidentifiedCleanup();
    virtual void Update(DesireUpdate*, float);
    virtual inline void UnidentifiedVirtual7(void*, DebugWriteCache*);
    virtual inline void UnidentifiedVirtual8(void*, DebugWriteCache*);

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

    virtual bool UnidentifiedInitialize(void*);
    virtual void UnidentifiedCleanup();
    virtual void Update(DesireUpdate*, float);
    virtual inline void UnidentifiedVirtual7(void*, DebugWriteCache*);
    virtual inline void UnidentifiedVirtual8(void*, DebugWriteCache*);

private:
    cPlayer* mpPassTarget;
    bool mbVolleyPass;
};

DesireUpdate TransDesireLooseBallContact(
    UnidentifiedFuzzyRuntimeValue* fielder,
    UnidentifiedFuzzyRuntimeValue* action);

#endif // GAME_AI_DESIREPASS_H
