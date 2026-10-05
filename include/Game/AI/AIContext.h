#ifndef GAME_AI_AI_CONTEXT_H
#define GAME_AI_AI_CONTEXT_H

#include "Game/AI/FuzzyAIRuntime.h"
#include "Game/AI/DesireUpdate.h"
#include "Game/AI/ScriptMachine.h"

float GetTickerMilliseconds();

extern float (*gAIProfilingClock)();
extern float (*gAIActivityClock)();

class AIContext : public FuzzyVariant
{
public:
    AIContext(cPlayer* value,
        ScriptMachine* machine,
        FuzzyRuntimeBase* runtime)
        : FuzzyVariant(value)
        , mTimers(16, 16)
    {
        mRuntime = runtime;
        mScriptMachine = machine;
        if (mScriptMachine != 0)
        {
            mScriptMachine->mAIContext = this;
        }
        if (mRuntime != 0)
        {
            mRuntime->mAIContext = this;
        }
    }

    AIContext(cGame* value,
        ScriptMachine* machine,
        FuzzyRuntimeBase* runtime)
        : FuzzyVariant(value)
        , mTimers(16, 16)
    {
        mRuntime = runtime;
        mScriptMachine = machine;
        if (mScriptMachine != 0)
        {
            mScriptMachine->mAIContext = this;
        }
        if (mRuntime != 0)
        {
            mRuntime->mAIContext = this;
        }
    }

    AIContext(cTeam* value,
        ScriptMachine* machine,
        FuzzyRuntimeBase* runtime)
        : FuzzyVariant(value)
        , mTimers(16, 16)
    {
        mRuntime = runtime;
        mScriptMachine = machine;
        if (mScriptMachine != 0)
        {
            mScriptMachine->mAIContext = this;
        }
        if (mRuntime != 0)
        {
            mRuntime->mAIContext = this;
        }
    }

    ~AIContext();

    void Cleanup(bool deleteRuntime, bool deleteScriptMachine);
    void Update(bool updateScriptMachine, float deltaTime);
    unsigned long GetTimerKey(unsigned long key, unsigned long multiplier) const;
    Timer* FindTimer(unsigned long key);
    Timer* SetTimer(unsigned long key, float seconds);
    bool IsTimerRunning(unsigned long key);

    FuzzyRuntimeBase* mRuntime;
    ScriptMachine* mScriptMachine;
    nlAVLTreeSlotPool<unsigned long, Timer,
        DefaultKeyCompare<unsigned long> > mTimers;
};

#endif // GAME_AI_AI_CONTEXT_H
