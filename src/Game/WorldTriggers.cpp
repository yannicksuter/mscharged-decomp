#include "NL/nlDLListContainer.inl"
#include "Game/WorldTriggers.h"

#include "Game/CharacterTriggers.h"
#include "Game/Drawable/DrawableFlyingCamera.h"
#include "Game/Effects/EmissionController.h"
#include "Game/Effects/EmissionManager.h"
#include "Game/Game.h"
#include "Game/ReplayManager.h"
#include "Game/UnidentifiedStaticStorage.h"

class EffectsGroup;

void CharacterAnimTriggerCallback(cSAnim* anim, unsigned int uParam)
{
    CharacterTriggerHandler(anim, uParam);
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
        DrawableFlyingCamera* flyingCamera = (DrawableFlyingCamera*)controller.m_uUserData;
        nlVector3 position = sCameraFlashOffset;
        RotateVector(position, position, flyingCamera->mOrientation);
        nlVec3Add(position, position, flyingCamera->mPosition);
        controller.SetPosition(position);
    }
}

void EmitCameraFlash(const nlVector3& position, void* flyingCamera)
{
    const char* groupName = "camera_flashes";
    EffectsGroup* group = EmissionManager::Instance()->GetEffectsGroup(groupName);
    EmissionController* controller = EmissionManager::Instance()->Create(group, 2, true, 0);
    nlVector3 velocity = { 0.0f, 0.0f, 0.0f };
    controller->SetVelocity(velocity);
    controller->m_fGround = 0.02f;
    controller->SetPosition(position);

    if (flyingCamera != 0)
    {
        controller->SetUpdateCallback(
            Function1<void, EmissionController&>(UpdateCameraFlash));
        controller->m_uUserData = (u32)flyingCamera;
    }
}
