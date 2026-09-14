#include "Game/UnidentifiedStaticStorage.h"
#include "Game/FE/feCamera.h"
#include "Game/FE/feModelManager.h"
#include "Game/Camera/animcam.h"
#include "Game/Render/StadiumLoading.h"
#include "Game/GameObjectLighting.h"
#include "Game/TweakFileLoader.h"
#include "NL/MemAlloc.h"

extern "C" void fn_80182EC0(unsigned int);

extern "C"
{
    int lbl_806E1890;
}

extern "C" void fn_801FE81C()
{
    if (FEModelManager::Instance() == 0)
        FEModelManager::s_pInstance = new (8, false) FEModelManager;
    lbl_806E1890 = 1;
    AllocatorStack[AllocatorStackDepth++] = &VirtualAllocator;
    CurrentAllocator = &VirtualAllocator;
    BeginLoadStadium("art/fe/environments/main", true);
    --AllocatorStackDepth;
    AllocatorStack[AllocatorStackDepth] = 0;
    CurrentAllocator = AllocatorStack[AllocatorStackDepth - 1];
    gTweakFileLoader.LoadFileAsync("ini/stadiums/FEWorld.ini", "");
}

extern "C" bool fn_801FE8EC()
{
    if (lbl_806E1890 == 1)
    {
        if (IsStadiumResourceDataLoaded())
        {
            lbl_806E1890 = 2;
            BeginLoadStadiumTemporaryResources();
        }
        return false;
    }
    else if (lbl_806E1890 == 2)
    {
        if (FinishLoadStadiumResources())
        {
            lbl_806E1890 = 3;
            BeginLoadStadiumEffects();
        }
        return false;
    }
    if (!FinishLoadStadiumEffects())
        return false;
    if (!gTweakFileLoader.ProcessLoadedFiles())
        return false;
    InitializeGameObjectLighting();
    fn_80182EC0(GetGameObjectLightRamp());
    return true;
}

extern "C" void fn_801FE99C()
{
    if (FEModelManager::Instance() != 0)
    {
        delete FEModelManager::s_pInstance;
        FEModelManager::s_pInstance = 0;
    }
    DestroyStadium();
    while (GetNextCamera() != 0)
        delete cCameraManager::PopCamera();
}

void PushPresentationCamera(const char* name, void (*callback)(eCameraMessage), float duration, bool deleteCurrentCamera)
{
    CurrentAllocator = &VirtualAllocator;
    unsigned int index = AllocatorStackDepth++;
    AllocatorStack[index] = CurrentAllocator;
    cAnimCamera* camera = new (nlMalloc(sizeof(cAnimCamera), 8, false)) cAnimCamera;
    --AllocatorStackDepth;
    AllocatorStack[AllocatorStackDepth] = 0;
    CurrentAllocator = AllocatorStack[AllocatorStackDepth - 1];
    camera->SelectCameraAnimation(name);
    camera->m_fAnimationSpeed = 1.0f;
    if (duration == 0.0f)
    {
        if (deleteCurrentCamera)
            PopPresentationCamera(callback, duration);
        cCameraManager::PushCamera(camera);
    }
    else
    {
        cCameraManager::PushCameraWithTransition(camera, duration, eCT_EASE_IN, callback, deleteCurrentCamera);
    }
}

void PopPresentationCamera(void (*callback)(eCameraMessage), float duration)
{
    cBaseCamera* camera;
    if (duration == 0.0f)
        camera = cCameraManager::PopCamera();
    else
        camera = cCameraManager::PopCameraWithTransition(duration, eCT_EASE_IN, callback);
    delete camera;
}
