#include "NL/nlDLListContainer.inl"
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/FE/feCamera.h"
#include "Game/FE/feModelManager.h"
#include "Game/Camera/animcam.h"
#include "Game/Render/StadiumLoading.h"
#include "Game/GameObjectLighting.h"
#include "Game/TweakFileLoader.h"
#include "NL/MemAlloc.h"

int gFEWorldLoadState;

void BeginLoadFEWorld()
{
    if (FEModelManager::Instance() == 0)
        FEModelManager::s_pInstance = new (8, false) FEModelManager;
    gFEWorldLoadState = FE_WORLD_LOADING_DATA;
    AllocatorStack[AllocatorStackDepth++] = &VirtualAllocator;
    CurrentAllocator = &VirtualAllocator;
    BeginLoadStadium("art/fe/environments/main", true);
    --AllocatorStackDepth;
    AllocatorStack[AllocatorStackDepth] = 0;
    CurrentAllocator = AllocatorStack[AllocatorStackDepth - 1];
    gTweakFileLoader.LoadFileAsync("ini/stadiums/FEWorld.ini", "");
}

bool FinishLoadFEWorld()
{
    if (gFEWorldLoadState == FE_WORLD_LOADING_DATA)
    {
        if (IsStadiumResourceDataLoaded())
        {
            gFEWorldLoadState = FE_WORLD_LOADING_RESOURCES;
            BeginLoadStadiumTemporaryResources();
        }
        return false;
    }
    else if (gFEWorldLoadState == FE_WORLD_LOADING_RESOURCES)
    {
        if (FinishLoadStadiumResources())
        {
            gFEWorldLoadState = FE_WORLD_LOADING_EFFECTS;
            BeginLoadStadiumEffects();
        }
        return false;
    }
    if (!FinishLoadStadiumEffects())
        return false;
    if (!gTweakFileLoader.ProcessLoadedFiles())
        return false;
    InitializeGameObjectLighting();
    SetGameObjectLightTexture(GetGameObjectLightRamp());
    return true;
}

void DestroyFEWorld()
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
