#include "Game/Task/ParticleUpdateTask.h"

#include "Game/Effects/EmissionManager.h"

ParticleUpdateTask* ParticleUpdateTask::sInstance;

ParticleUpdateTask::ParticleUpdateTask()
    : mTimeScale(1.0f)
    , mResetPending(false)
    , mContext(0)
    , mParameter1(0)
    , mParameter2(0)
    , mRenderEnabled(true)
    , mUpdateEnabled(true)
{
    sInstance = this;
    mUnknownFlag = false;
}

void ParticleUpdateTask::SetTimeScale(float timeScale)
{
    mTimeScale = timeScale;
}

void ParticleUpdateTask::Run(float dt)
{
    if (mResetPending)
    {
        mResetPending = false;
    }

    if (mBeforeUpdate)
    {
        mBeforeUpdate();
    }

    bool update = true;
    if (mCanUpdate)
    {
        update = mCanUpdate();
    }
    if (mUpdateEnabled && update)
    {
        EmissionManager* manager = EmissionManager::Instance();
        manager->Update(dt * mTimeScale);
    }

    bool render = true;
    if (mCanRender)
    {
        render = mCanRender();
    }
    if (mRenderEnabled && render)
    {
        EmissionManager* manager = EmissionManager::Instance();
        manager->Render();
    }
}

void ParticleUpdateTask::Shutdown()
{
    if (EmissionManager::Instance() != 0)
    {
        EmissionManager::Instance()->Shutdown();
    }
}

void ParticleUpdateTask::Initialize(void* context, int parameter1, int parameter2)
{
    mContext = context;
    mParameter1 = parameter1;
    mParameter2 = parameter2;
    EmissionManager::Instance();
    fn_802E6C20(
        EmissionManager::Instance(), context, parameter1, parameter2);
}

void ParticleUpdateTask::StartLoading(bool first, bool second, bool third, bool fourth)
{
    fn_802E6620(first, second, third, fourth);
}

bool ParticleUpdateTask::FinishLoading(void* context)
{
    return fn_802E6774(context);
}
