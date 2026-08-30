#include "Game/WorldTriggers.h"

#include "Game/Effects/EmissionController.h"
#include "Game/Effects/EmissionManager.h"
#include "Game/Game.h"

class EffectsGroup;

struct WorldTriggerTransform_801BE238
{
    nlQuaternion mUnidentified00;
    nlVector3 mUnidentified10;
};

struct WorldTriggerState_80188C5C
{
    u8 mUnidentified0000[0x755C];
    void* mUnidentified755C;
};

extern "C" void fn_801B66C0(const char* name);
extern "C" WorldTriggerState_80188C5C* fn_80188C5C();
extern "C" EffectsGroup* fn_802E7CDC(EmissionManager*, const char*);
extern "C" EmissionController* fn_802E7FE4(
    EmissionManager*, EffectsGroup*, int, bool, bool);

void EmitGeneric(const char* name)
{
    fn_801B66C0(name);
}

static const nlVector3 sCameraFlashOffset = { -0.8f, 0.0f, 0.1f };

static void UpdateCameraFlash(EmissionController& controller)
{
    if (g_pGame != 0)
    {
        if (g_pGame->m_eGameState == 4)
        {
            return;
        }
        if (controller.m_GlView == 0
            && fn_80188C5C()->mUnidentified755C != 0)
        {
            WorldTriggerTransform_801BE238* transform = (WorldTriggerTransform_801BE238*)controller.m_uUserData;
            nlVector3 position = sCameraFlashOffset;
            RotateVector(position, position, transform->mUnidentified00);
            nlVec3Add(position, position, transform->mUnidentified10);
            controller.SetPosition(position);
        }
    }
}

void EmitCameraFlash(const nlVector3& position, void* transform)
{
    const char* groupName = "camera_flashes";
    EffectsGroup* group = fn_802E7CDC(EmissionManager::Instance(), groupName);
    EmissionController* controller = fn_802E7FE4(EmissionManager::Instance(), group, 2, true, false);
    nlVector3 velocity = { 0.0f, 0.0f, 0.0f };
    controller->SetVelocity(velocity);
    controller->m_fGround = 0.02f;
    controller->SetPosition(position);

    if (transform != 0)
    {
        controller->SetUpdateCallback(
            Function1<void, EmissionController&>(UpdateCameraFlash));
        controller->m_uUserData = (u32)transform;
    }
}
