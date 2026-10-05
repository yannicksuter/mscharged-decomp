#include "Game/AI/TeamPlayMachine.h"
#include "Game/AI/Desire.h"

#include "Game/AI/DesireUpdate.h"
#include "Game/DB/GameProgress.h"
#include "Game/Game.h"
#include "Game/GameInfo.h"
#include "NL/nlMemory.h"

float gKickoffStateTimeLimit = 1.1f;
char gTeamPlayMachineName[] = "TeamPlayMachine";

TeamPlayMachine::~TeamPlayMachine()
{
}

void TeamPlayMachine::Initialize()
{
    ScriptMachine::Initialize();

    TutorialMegastrikeDesire* desire =
        new (nlMalloc(sizeof(TutorialMegastrikeDesire), 8, false))
            TutorialMegastrikeDesire(5, TransitionFunc(g_UnsetTransitionFunc));
    AddState(5, desire, false);
}

void TeamPlayMachine::Update(float deltaTime)
{
    ScriptMachine::Update(deltaTime);
}

void TeamPlayMachine::SelectState()
{
    UnidentifiedVariantCollection values;
    int state = -1;

    if (g_pGame->m_eGameState == 1)
    {
        values.Set(7, FuzzyVariant(gKickoffStateTimeLimit));
        state = 1;
    }
    else if (GameInfoManager::Instance()->IsInMode4()
        && g_pStrikerChallenge->mCurrentChallenge == 2)
    {
        state = 5;
    }

    if (state != -1)
    {
        ActivateState(state, &values, true);
    }
    else
    {
        ScriptMachine::SelectState();
    }
}

TeamPlayMachine::TeamPlayMachine()
    : ScriptMachine(7, true, false, gTeamPlayMachineName)
{
}
