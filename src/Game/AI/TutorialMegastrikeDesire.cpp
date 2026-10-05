#include "Game/AI/TeamPlayMachine.h"

#include "Game/AI/Fielder.h"
#include "Game/Team.h"

char lbl_80504000[] = "TutorialMegastrikeDesire";

void TutorialMegastrikeDesire::Update(
    DesireUpdate*, float)
{
}

void TutorialMegastrikeDesire::Cleanup()
{
}

TutorialMegastrikeDesire::~TutorialMegastrikeDesire()
{
}

bool TutorialMegastrikeDesire::Initialize(void*)
{
    const char* name = lbl_80504000;

    for (int i = 0; i < 4; ++i)
    {
        ScriptMachine* state = fn_8002E1A4(m_pTeam->GetFielder(i));
        ScriptTransitionFunc value(name);
        state->mTransition.mValue.mFuncHash = value.mValue.mFuncHash;
        state->mTransition.mValue.mNativeFunc = value.mValue.mNativeFunc;
    }

    return true;
}
