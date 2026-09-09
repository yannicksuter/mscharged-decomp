#include "Game/AI/TeamPlayMachine.h"

#include "Game/AI/Fielder.h"
#include "Game/Team.h"

char lbl_80504000[] = "TutorialMegastrikeDesire";

void TutorialMegastrikeDesire::Update(
    UnidentifiedDesireUpdate*, float)
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
        UnidentifiedStringHash value(name);
        state->mUnidentified00C.mUnidentifiedHash = value.mUnidentifiedHash;
        state->mUnidentified00C.mUnidentifiedFunction = value.mUnidentifiedFunction;
    }

    return true;
}
