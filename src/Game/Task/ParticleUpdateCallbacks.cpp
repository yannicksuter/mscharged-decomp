#include "Game/Task/ParticleUpdateCallbacks.h"

#include "Game/Task/GameRenderTask.h"
#include "Game/Task/ParticleUpdateTask.h"
#include "Game/UnidentifiedStaticStorage.h"

bool g_bRenderParticles = true;

ParticleUpdateTask* GetParticleUpdateTask();

void ParticleUpdateNoOp()
{
}

bool CanUpdateParticles()
{
    return nlTaskManager::m_pInstance->mCurrentState != 1;
}

bool CanRenderParticles()
{
    bool result = true;
    if (!g_bRenderParticles)
    {
        result = false;
    }
    if (!g_bRenderWorld)
    {
        result = false;
    }
    return result;
}

void BeforeParticleUpdate()
{
}

void InitializeParticleUpdateCallbacks()
{
    GetParticleUpdateTask()->mCanRender = Function0<bool>(CanRenderParticles);
    GetParticleUpdateTask()->mCanUpdate = Function0<bool>(CanUpdateParticles);
    GetParticleUpdateTask()->mBeforeUpdate = Function0<void>(BeforeParticleUpdate);
}

ParticleUpdateTask* GetParticleUpdateTask()
{
    return ParticleUpdateTask::sInstance;
}
