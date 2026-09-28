#include "Game/AI/TeamPlayMachine.h"

#include "Game/AI/Fielder.h"
#include "Game/Team.h"

char lbl_80504000[] = "TutorialMegastrikeDesire";

void TutorialMegastrikeDesire::Update(
    DesireUpdate*, float)
{
}

void TutorialMegastrikeDesire::UnidentifiedCleanup()
{
}

TutorialMegastrikeDesire::~TutorialMegastrikeDesire()
{
}

bool TutorialMegastrikeDesire::UnidentifiedInitialize(void*)
{
    const char* name = lbl_80504000;

    for (int i = 0; i < 4; ++i)
    {
        UnidentifiedScriptMachine* state = fn_8002E1A4(m_pTeam->GetFielder(i));
        ScriptTransitionFunc value(name);
        state->mTransition.mFuncHash = value.mFuncHash;
        state->mTransition.mNativeFunc = value.mNativeFunc;
    }

    return true;
}
