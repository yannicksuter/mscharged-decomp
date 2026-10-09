#include "NL/nlDLListContainer.inl"
#include "Game/AI/TeamPlayMachine.h"
#include "Game/AI/Desire.h"

#include "Game/AI/DesireUpdate.h"
#include "Game/DB/GameProgress.h"
#include "Game/Game.h"
#include "Game/GameInfo.h"
#include "NL/nlMemory.h"

enum eTeamPlayState
{
    TEAM_PLAY_NONE = -1,
    TEAM_PLAY_KICKOFF = 1,
    TEAM_PLAY_TUTORIAL_MEGA_STRIKE = 5,
};

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
            TutorialMegastrikeDesire(TEAM_PLAY_TUTORIAL_MEGA_STRIKE, TransitionFunc(g_UnsetTransitionFunc));
    AddState(TEAM_PLAY_TUTORIAL_MEGA_STRIKE, desire, false);
}

void TeamPlayMachine::Update(float deltaTime)
{
    ScriptMachine::Update(deltaTime);
}

void TeamPlayMachine::SelectState()
{
    FuzzyVariantCollection values;
    int state = TEAM_PLAY_NONE;

    if (g_pGame->m_eGameState == GS_KICKOFF)
    {
        values.Set(7, FuzzyVariant(gKickoffStateTimeLimit));
        state = TEAM_PLAY_KICKOFF;
    }
    else if (GameInfoManager::Instance()->IsInMode4()
        && g_pStrikerChallenge->mCurrentChallenge == 2)
    {
        state = TEAM_PLAY_TUTORIAL_MEGA_STRIKE;
    }

    if (state != TEAM_PLAY_NONE)
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
