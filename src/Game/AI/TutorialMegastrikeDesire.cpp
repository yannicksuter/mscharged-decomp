#include "Game/AI/TeamPlayMachine.h"

#include "Game/AI/Fielder.h"
#include "Game/Team.h"

extern "C" Unidentified8002E1A4Result* fn_8002E1A4(cFielder*);

char lbl_80504000[] = "TutorialMegastrikeDesire";

void TutorialMegastrikeDesire::UnidentifiedUpdate(
    UnidentifiedDesireUpdate*)
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
        Unidentified8002E1A4Result* state = fn_8002E1A4(m_pTeam->GetFielder(i));
        UnidentifiedStringHash value(name);
        state->mUnidentified00C = value.mHash;
        state->mUnidentified010 = value.mUnidentified004;
    }

    return true;
}
