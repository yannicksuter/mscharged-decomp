#ifndef GAME_AI_TEAM_PLAY_MACHINE_H
#define GAME_AI_TEAM_PLAY_MACHINE_H

#include "Game/AI/Desire.h"
#include "Game/AI/Variant.h"

class cTeam;

struct Unidentified8002E1A4Result
{
    u8 mUnidentified000[0x0C];
    unsigned long mUnidentified00C;
    unsigned long mUnidentified010;
};

struct UnidentifiedStringHash
{
    UnidentifiedStringHash(const char* name);

    unsigned long mHash;
    unsigned long mUnidentified004;
};

class UnidentifiedVariantCollection
{
public:
    UnidentifiedVariantCollection();
    ~UnidentifiedVariantCollection();

    void Set(int index, const Variant& value);

private:
    void* mUnidentifiedValues[19];
};

class UnidentifiedScriptMachine
{
public:
    UnidentifiedScriptMachine(
        int, bool, bool, const char*);
    virtual ~UnidentifiedScriptMachine();

    virtual bool UnidentifiedVirtual1() const;
    virtual void UnidentifiedVirtual2();
    virtual void UnidentifiedVirtual3(float deltaTime);
    virtual void UnidentifiedVirtual4(void* context);
    virtual void* UnidentifiedVirtual5(
        int, UnidentifiedVariantCollection*, bool);
    virtual void UnidentifiedVirtual6();
    virtual void UnidentifiedVirtual7();
    virtual void UnidentifiedVirtual8();

    void UnidentifiedAddState(int, shdStateMachine*, bool);

private:
    u8 mUnidentified004[0xB4];
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
    virtual void UnidentifiedUpdate(UnidentifiedDesireUpdate*);
    virtual void UnidentifiedSetContext(UnidentifiedDesireContext*);

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
    virtual void UnidentifiedUpdate(UnidentifiedDesireUpdate*);
};

#endif // GAME_AI_TEAM_PLAY_MACHINE_H
