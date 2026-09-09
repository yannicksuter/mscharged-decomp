#ifndef GAME_AI_FIELDER_INPUT_H
#define GAME_AI_FIELDER_INPUT_H

#include "Game/AI/FuzzyAIRuntime.h"
#include "Game/AI/FuzzyVariant.h"
#include "Game/AI/TeamPlayMachine.h"

class UnidentifiedFielderInput : public FuzzyVariant
{
public:
    UnidentifiedFielderInput(cPlayer* value,
        UnidentifiedScriptMachine* machine,
        UnidentifiedFuzzyRuntimeBase* runtime)
        : FuzzyVariant(value)
        , mTimers(16, 16)
    {
        mUnidentified14 = runtime;
        mUnidentified18 = machine;
        if (mUnidentified18 != 0)
        {
            mUnidentified18->mUnidentified064 = this;
        }
        if (mUnidentified14 != 0)
        {
            mUnidentified14->mValue = this;
        }
    }

    UnidentifiedFielderInput(cGame* value,
        UnidentifiedScriptMachine* machine,
        UnidentifiedFuzzyRuntimeBase* runtime)
        : FuzzyVariant(value)
        , mTimers(16, 16)
    {
        mUnidentified14 = runtime;
        mUnidentified18 = machine;
        if (mUnidentified18 != 0)
        {
            mUnidentified18->mUnidentified064 = this;
        }
        if (mUnidentified14 != 0)
        {
            mUnidentified14->mValue = this;
        }
    }

    UnidentifiedFielderInput(cTeam* value,
        UnidentifiedScriptMachine* machine,
        UnidentifiedFuzzyRuntimeBase* runtime)
        : FuzzyVariant(value)
        , mTimers(16, 16)
    {
        mUnidentified14 = runtime;
        mUnidentified18 = machine;
        if (mUnidentified18 != 0)
        {
            mUnidentified18->mUnidentified064 = this;
        }
        if (mUnidentified14 != 0)
        {
            mUnidentified14->mValue = this;
        }
    }

    ~UnidentifiedFielderInput();

    void fn_8030F74C(bool deleteOwner, bool deleteController);
    void fn_8030F800(bool updateController, float dt);
    unsigned long fn_8030F9B4(unsigned long key, bool concurrent) const;
    Timer* fn_8030F9BC(unsigned long key);
    Timer* fn_8030FA10(unsigned long key, float seconds);
    bool fn_8030FB7C(unsigned long key);

    UnidentifiedFuzzyRuntimeBase* mUnidentified14;
    UnidentifiedScriptMachine* mUnidentified18;
    nlAVLTreeSlotPool<unsigned long, Timer,
        DefaultKeyCompare<unsigned long> > mTimers;
};

#endif // GAME_AI_FIELDER_INPUT_H
