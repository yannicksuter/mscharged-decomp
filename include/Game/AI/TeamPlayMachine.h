#ifndef GAME_AI_TEAM_PLAY_MACHINE_H
#define GAME_AI_TEAM_PLAY_MACHINE_H

#include "Game/AI/Desire.h"
#include "Game/AI/UnidentifiedStringHash.h"
#include "Game/AI/Variant.h"

class cTeam;
class UnidentifiedVariantCollection;

struct Unidentified8002E1A4Result
{
    u8 mUnidentified000[0x0C];
    unsigned long mUnidentified00C;
    void* mUnidentified010;
};

class UnidentifiedScriptMachine
{
public:
    UnidentifiedScriptMachine(
        int, bool, UnidentifiedFielderInput*, const char*);
    virtual ~UnidentifiedScriptMachine();

    virtual bool UnidentifiedVirtual1() const
    {
        return mUnidentified004 == 0;
    }
    virtual void UnidentifiedVirtual2();
    virtual void UnidentifiedVirtual3(float deltaTime);
    virtual void UnidentifiedVirtual4(bool);
    virtual shdStateMachine* UnidentifiedVirtual5(
        int, UnidentifiedVariantCollection*, bool);
    virtual void UnidentifiedVirtual6();
    virtual void UnidentifiedVirtual7();
    virtual void UnidentifiedVirtual8()
    {
    }

    void UnidentifiedAddState(int, shdStateMachine*, bool);

    shdStateMachine* mUnidentified004;
    shdStateMachine* mUnidentified008;
    UnidentifiedStateTransition mUnidentified00C;
    int mUnidentified014;
    UnidentifiedVariantCollection mUnidentified018;
    UnidentifiedFielderInput* mUnidentified064;
    bool mUnidentified068;
    u8 mPadding069[3];
    shdStateMachine** mUnidentified06C;
    shdStateMachine** mUnidentified070;
    int mUnidentified074;
    char mUnidentified078[0x40];
};

class TeamPlayMachine : public UnidentifiedScriptMachine
{
public:
    TeamPlayMachine();
    virtual ~TeamPlayMachine();

    virtual void UnidentifiedVirtual2();
    virtual void UnidentifiedVirtual3(float deltaTime);
    virtual void UnidentifiedVirtual7();
    virtual void UnidentifiedVirtual8();
};

class UnidentifiedTeamDesire : public shdStateMachine
{
public:
    UnidentifiedTeamDesire(
        int state, const UnidentifiedStateTransition& transition);
    virtual ~UnidentifiedTeamDesire()
    {
    }

    virtual bool UnidentifiedInitialize(void*) = 0;
    virtual bool UnidentifiedReinitialize(void*);
    virtual void UnidentifiedCleanup();
    virtual void UnidentifiedUpdate(UnidentifiedDesireUpdate*, float);
    virtual void UnidentifiedSetContext(UnidentifiedScriptMachine*);

protected:
    cTeam* m_pTeam;
};

class TutorialMegastrikeDesire : public UnidentifiedTeamDesire
{
public:
    TutorialMegastrikeDesire(
        int state, UnidentifiedStateTransition transition)
        : UnidentifiedTeamDesire(state, transition)
    {
    }

    virtual ~TutorialMegastrikeDesire();

    virtual bool UnidentifiedInitialize(void*);
    virtual void UnidentifiedCleanup();
    virtual void UnidentifiedUpdate(UnidentifiedDesireUpdate*, float);
};

#endif // GAME_AI_TEAM_PLAY_MACHINE_H
