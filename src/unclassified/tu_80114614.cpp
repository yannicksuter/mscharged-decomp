#include "NL/nlTask.h"

#include "types.h"

class FrontEndTask : public nlTask
{
public:
    FrontEndTask();

    virtual void Run(float dt);
    virtual const char* GetName() { return "Front End"; }

    void HandleE3IdleReset(float dt);

private:
    float mValue_20;
    float mValue_24;
    float mValue_28;
    float mValue_2C;
};

extern "C"
{
    f32 lbl_806E0FF0;
}

f32 g_fUpTime;
f32 g_fIdleGameTime;

extern "C" void fn_80114614(float value)
{
    lbl_806E0FF0 = value;
}

FrontEndTask::FrontEndTask()
    : mValue_20(0.0f)
    , mValue_24(0.0f)
    , mValue_28(0.0f)
    , mValue_2C(0.0f)
{
    mTimeDilated = false;
}

void FrontEndTask::Run(float dt)
{
    g_fUpTime += dt;
    HandleE3IdleReset(dt);
}
