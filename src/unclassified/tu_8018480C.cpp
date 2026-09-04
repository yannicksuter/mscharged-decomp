#include "Game/Task/ParticleUpdateTask.h"

#include "Game/Task/GameRenderTask.h"

u8 lbl_806DCC98 = 1;

extern "C" ParticleUpdateTask* fn_80184AA4();

extern "C" void fn_8018480C()
{
}

extern "C" bool fn_80184810()
{
    return nlTaskManager::m_pInstance->mCurrentState != 1;
}

extern "C" bool fn_8018482C()
{
    bool result = true;
    if (!lbl_806DCC98)
    {
        result = false;
    }
    if (!g_bRenderWorld)
    {
        result = false;
    }
    return result;
}

extern "C" void fn_80184854()
{
}

extern "C" void fn_80184858()
{
    fn_80184AA4()->mCanRender = Function0<bool>(fn_8018482C);
    fn_80184AA4()->mCanUpdate = Function0<bool>(fn_80184810);
    fn_80184AA4()->mBeforeUpdate = Function0<void>(fn_80184854);
}

extern "C" ParticleUpdateTask* fn_80184AA4()
{
    return ParticleUpdateTask::sInstance;
}
