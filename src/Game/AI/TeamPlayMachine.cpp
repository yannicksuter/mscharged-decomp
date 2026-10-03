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

void TeamPlayMachine::UnidentifiedVirtual2()
{
    UnidentifiedScriptMachine::UnidentifiedVirtual2();

    TutorialMegastrikeDesire* desire =
        new (nlMalloc(sizeof(TutorialMegastrikeDesire), 8, false))
            TutorialMegastrikeDesire(5, TransitionFunc(g_UnsetTransitionFunc));
    UnidentifiedAddState(5, desire, false);
}

void TeamPlayMachine::Update(float deltaTime)
{
    UnidentifiedScriptMachine::Update(deltaTime);
}

void TeamPlayMachine::UnidentifiedVirtual7()
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
        UnidentifiedVirtual5(state, &values, true);
    }
    else
    {
        UnidentifiedScriptMachine::UnidentifiedVirtual7();
    }
}

TeamPlayMachine::TeamPlayMachine()
    : UnidentifiedScriptMachine(7, true, false, gTeamPlayMachineName)
{
}

void TeamPlayMachine::UnidentifiedVirtual8()
{
}
