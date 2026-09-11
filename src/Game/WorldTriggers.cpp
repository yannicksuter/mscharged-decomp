#include "Game/WorldTriggers.h"

#include "Game/CharacterTriggers.h"
#include "Game/Effects/EmissionController.h"
#include "Game/Effects/EmissionManager.h"
#include "Game/Game.h"
#include "Game/ReplayManager.h"
#include "Game/UnidentifiedStaticStorage.h"

class EffectsGroup;

struct WorldTriggerTransform_801BE238
{
    nlQuaternion mUnidentified00;
    nlVector3 mUnidentified10;
};


extern "C" void fn_801BE234(cSAnim* arg0, unsigned int uParam)
{
    CharacterTriggerHandler(arg0, uParam);
}

static const nlVector3 sCameraFlashOffset = { -0.8f, 0.0f, 0.1f };

static void UpdateCameraFlash(EmissionController& controller)
{
    if (g_pGame == 0 || g_pGame->m_eGameState == 4)
    {
        return;
    }

    if (controller.m_Replaying == 0
        && ReplayManager::Instance()->mRender != 0)
    {
        WorldTriggerTransform_801BE238* transform = (WorldTriggerTransform_801BE238*)controller.m_uUserData;
        nlVector3 position = sCameraFlashOffset;
        RotateVector(position, position, transform->mUnidentified00);
        nlVec3Add(position, position, transform->mUnidentified10);
        controller.SetPosition(position);
    }
}

void EmitCameraFlash(const nlVector3& position, void* transform)
{
    const char* groupName = "camera_flashes";
    EffectsGroup* group = EmissionManager::Instance()->GetEffectsGroup(groupName);
    EmissionController* controller = EmissionManager::Instance()->Create(group, 2, true, 0);
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
