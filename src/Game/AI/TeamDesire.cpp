#include "Game/AI/TeamPlayMachine.h"

UnidentifiedTeamDesire::UnidentifiedTeamDesire(
    int state, const UnidentifiedStateTransition& transition)
    : shdStateMachine(state, transition)
{
    mUnidentified080 = 0.33f;
    mUnidentified084 = 1.0f;
}

void UnidentifiedTeamDesire::UnidentifiedSetContext(
    UnidentifiedDesireContext* context)
{
    shdStateMachine::UnidentifiedSetContext(context);
    if (context != 0)
    {
        m_pTeam = (cTeam*)context->mUnidentifiedValue->mData.pointer;
    }
    else
    {
        m_pTeam = 0;
    }
}

bool UnidentifiedTeamDesire::UnidentifiedReinitialize(void*)
{
    return true;
}

void UnidentifiedTeamDesire::UnidentifiedCleanup()
{
}

void UnidentifiedTeamDesire::UnidentifiedUpdate(UnidentifiedDesireUpdate*)
{
}
