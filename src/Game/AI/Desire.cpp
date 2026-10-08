#include "Game/AI/Desire.h"
#include "Game/AI/Fielder.h"
#include "Game/AI/AIContext.h"
#include "Game/AI/ScriptMachine.h"
#include "Game/Sys/debug.h"

unsigned short gFielderDesireStateDebugType = 0xFFFF;
float lbl_806DC04C = 60.0f;
float lbl_806DC050 = 0.3f;
#pragma explicit_zero_data on
float lbl_806DC054 = 0.0f;
#pragma explicit_zero_data off

Desire::Desire(int state, TransitionFunc& transition)
    : shdStateMachine(state, transition)
    , mThinkTimer()
{
    m_pFielder = 0;
    mvDesiredPosition.x = 0.0f;
    mvDesiredPosition.y = 0.0f;
    mvDesiredPosition.z = 0.0f;
    mTurboRequest = 0;
    mDefaultMinDuration = 0.33f;
    mDefaultMaxDuration = 1.0f;
}

void Desire::SetContext(ScriptMachine* context)
{
    shdStateMachine::SetContext(context);
    if (context != 0)
    {
        m_pFielder
            = (cFielder*)context->mAIContext->mData.pointer;
    }
    else
    {
        m_pFielder = 0;
    }
}

bool Desire::Initialize(void*)
{
    return true;
}

bool Desire::Reinitialize(void* context)
{
    Cleanup();
    mAgeTimer.SetSeconds(lbl_806DC054);
    return Initialize(context);
}

bool DesireFinishAction::Initialize(void*)
{
    mMaxDuration = lbl_806DC04C;
    return true;
}

void DesireFinishAction::Update(DesireUpdate* update, float)
{
    if (update->mData.i == 2)
    {
        tDebugPrintManager::Print(DC_AI,
            "** WARNING! DesireFinishAction has expired after %f seconds, probably a bug!\n",
            mAgeTimer.GetSeconds());
    }
    fn_80098098(m_pFielder);
}

bool DesireWait::Initialize(void*)
{
    mMaxDuration = lbl_806DC050;
    return true;
}

void DesireWait::Update(DesireUpdate*, float)
{
    m_pFielder->SetThingsToAvoid(0);
    m_pFielder->AddDesiredPosition(m_pFielder->mUnidentified024.m_v3Position, 1.0f, 1.0f);
}

void DestroyDesire(Desire* desire)
{
    delete desire;
}

DesireFinishAction::~DesireFinishAction()
{
}

DesireWait::~DesireWait()
{
}

void Desire::Cleanup()
{
}
