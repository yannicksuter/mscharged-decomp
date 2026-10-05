#include "Game/AI/TeamPlayMachine.h"
#include "Game/AI/AIContext.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/Audio/RegistryPools.h"

TeamDesire::TeamDesire(
    int state, TransitionFunc& transition)
    : shdStateMachine(state, transition)
{
    mDefaultMinDuration = 0.33f;
    mDefaultMaxDuration = 1.0f;
}

void TeamDesire::SetContext(
    ScriptMachine* context)
{
    shdStateMachine::SetContext(context);
    if (context != 0)
    {
        m_pTeam = (cTeam*)context->mAIContext->mData.pointer;
    }
    else
    {
        m_pTeam = 0;
    }
}

bool TeamDesire::Reinitialize(void*)
{
    return true;
}

void TeamDesire::Cleanup()
{
}

void TeamDesire::Update(DesireUpdate*, float)
{
}
