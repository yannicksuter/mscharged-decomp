#ifndef GAME_AI_TEAM_PLAY_MACHINE_H
#define GAME_AI_TEAM_PLAY_MACHINE_H

#include "Game/AI/Desire.h"
#include "Game/AI/TransitionFunc.h"
#include "Game/AI/Variant.h"
#include "Game/AI/ScriptMachine.h"

class cTeam;

class TeamPlayMachine : public ScriptMachine
{
public:
    TeamPlayMachine();
    virtual ~TeamPlayMachine();

    virtual void Initialize();
    virtual void Update(float deltaTime);
    virtual void SelectState();
};

class TeamDesire : public shdStateMachine
{
public:
    TeamDesire(
        int state, TransitionFunc& transition);
    virtual ~TeamDesire()
    {
    }

    virtual bool Initialize(void*) = 0;
    virtual bool Reinitialize(void*);
    virtual void Cleanup();
    virtual void Update(DesireUpdate*, float);
    virtual void SetContext(ScriptMachine*);

protected:
    cTeam* m_pTeam;
};

class TutorialMegastrikeDesire : public TeamDesire
{
public:
    TutorialMegastrikeDesire(
        int state, TransitionFunc transition)
        : TeamDesire(state, transition)
    {
    }

    virtual ~TutorialMegastrikeDesire();

    virtual bool Initialize(void*);
    virtual void Cleanup();
    virtual void Update(DesireUpdate*, float);
};

#endif // GAME_AI_TEAM_PLAY_MACHINE_H
