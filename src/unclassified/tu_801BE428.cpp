#include "Game/Ball.h"
#include "Game/BallTrail.h"
#include "Game/Character.h"
#include "Game/Drawable/DrawableCharacter.h"
#include "Game/Effects/EmissionController.h"
#include "Game/Game.h"
#include "Game/Player.h"
#include "Game/PoseAccumulator.h"
#include "Game/Render/ImpostorCharacter.h"
#include "Game/ReplayManager.h"
#include "Game/SHierarchy.h"
#include "Game/Team.h"
#include "NL/nlTask.h"

extern float lbl_806DB5A4;

static const nlVector3 sZeroVelocity = { 0.0f, 0.0f, 0.0f };

extern "C" DrawableCharacter* fn_801BE428(cCharacter* character)
{
    if (character->m_eClassType == FIELDER)
    {
        cPlayer* player = (cPlayer*)character;
        int characterIndex
            = player->m_ID + 4 * player->m_pTeam->m_nSide;
        return &ReplayManager::Instance()
                    ->mRender->mCharacters[characterIndex];
    }
    else
    {
        cPlayer* player = (cPlayer*)character;
        int teamSide = player->m_pTeam->m_nSide;
        return &ReplayManager::Instance()
                    ->mRender->mCharacters[teamSide + 8];
    }
}

extern "C" void fn_801BE4A4(EmissionController& controller)
{
    if (g_pGame == 0 || g_pGame->m_eGameState == 4)
    {
        return;
    }

    if (ReplayManager::Instance()->mRender != 0)
    {
        cCharacter* character = (cCharacter*)controller.m_uUserData;
        DrawableCharacter* drawableCharacter = fn_801BE428(character);
        controller.SetPosition(drawableCharacter->position);
        controller.SetVelocity(drawableCharacter->velocity);
        controller.SetPoseAccumulator(
            *drawableCharacter->poseAccumulator);
        controller.SetAnimController(
            drawableCharacter->GetAnimController());
    }
}

extern "C" void fn_801BE594(
    EmissionController& controller, cCharacter* character)
{
    if (g_pGame == 0 || g_pGame->m_eGameState == 4)
    {
        return;
    }

    if (ReplayManager::Instance()->mRender != 0)
    {
        DrawableCharacter* drawableCharacter = fn_801BE428(character);
        controller.SetPosition(drawableCharacter->position);
        controller.SetVelocity(drawableCharacter->velocity);
        controller.SetPoseAccumulator(
            *drawableCharacter->poseAccumulator);
    }
}

void UpdateEmitterFromCharacter(EmissionController& controller)
{
    if (g_pGame == 0 || g_pGame->m_eGameState == 4)
    {
        return;
    }

    if (ReplayManager::Instance()->mRender != 0)
    {
        fn_801BE4A4(controller);

        cCharacter* character
            = DrawableCharacter::OnlyRenderingOneCharacter();
        if (character != 0
            && (cCharacter*)controller.m_uUserData != character)
        {
            controller.Die();
        }
    }
}

void UpdateEmitterPoseFromCharacter(EmissionController& controller)
{
    if (g_pGame == 0 || g_pGame->m_eGameState == 4)
    {
        return;
    }

    if (ReplayManager::Instance()->mRender != 0)
    {
        cCharacter* character = (cCharacter*)controller.m_uUserData;
        DrawableCharacter* drawableCharacter = fn_801BE428(character);
        controller.SetPoseAccumulator(
            *drawableCharacter->poseAccumulator);
        controller.SetAnimController(
            drawableCharacter->GetAnimController());
    }
}

void UpdateEmitterFromBall(EmissionController& controller)
{
    if (g_pGame == 0 || g_pGame->m_eGameState == 4)
    {
        return;
    }

    if (ReplayManager::Instance()->mRender != 0)
    {
        if (!g_pGame->IsGameplayOrOvertime()
            && nlTaskManager::m_pInstance->mCurrentState != 8)
        {
            controller.m_bVisible = ReplayManager::Instance()
                                            ->mRender->mBall.mFlags.bits.visible;
        }

        ReplayManager* manager = ReplayManager::Instance();
        controller.SetPosition(manager->mRender->mBall.mPosition);
        manager = ReplayManager::Instance();
        controller.SetVelocity(manager->mRender->mBall.mVelocity);
    }
}

extern "C" void fn_801BE950(EmissionController& controller)
{
    if (g_pGame == 0 || g_pGame->m_eGameState == 4)
    {
        return;
    }

    if (controller.m_GlView == 0
        && ReplayManager::Instance()->mRender != 0)
    {
        LiveBallTrail* trail
            = (LiveBallTrail*)controller.m_uUserData;
        if (trail->visible)
        {
            controller.SetPosition(trail->position);
            controller.SetVelocity(trail->velocity);
        }
        else
        {
            controller.Die();
        }
    }
}

extern "C" void fn_801BE9EC(EmissionController& controller)
{
    if (g_pGame == 0 || g_pGame->m_eGameState == 4)
    {
        return;
    }

    if (controller.m_GlView == 0)
    {
        cBall* ball = (cBall*)controller.m_uUserData;
        if (ball->m_v3Position.z > lbl_806DB5A4)
        {
            nlVector3 position;
            ball->PredictLandingSpotAndTime(
                position, 0, 0, lbl_806DB5A4);
            controller.SetPosition(position);
            controller.SetVelocity(sZeroVelocity);
        }
    }
}

extern "C" void fn_801BEA7C(EmissionController& controller)
{
    if (g_pGame == 0 || g_pGame->m_eGameState == 4)
    {
        return;
    }

    if (controller.m_GlView == 0
        && ReplayManager::Instance()->mRender != 0)
    {
        cCharacter* character = (cCharacter*)controller.m_uUserData;
        DrawableCharacter* drawableCharacter = fn_801BE428(character);
        controller.SetPosition(drawableCharacter->position);
        controller.SetVelocity(drawableCharacter->velocity);
        controller.SetPoseAccumulator(
            *drawableCharacter->poseAccumulator);

        nlMatrix4& matrix
            = drawableCharacter->poseAccumulator->GetNodeMatrix(
                character->m_nHeadJointIndex);
        nlVector3 direction;
        nlVec3Set(direction, matrix.m21, matrix.m22, matrix.m23);
        nlVector3 adjustedDirection = direction;
        adjustedDirection.z = -0.01f;
        nlVec3Scale(adjustedDirection,
            nlRecipSqrt(
                nlVec3DotProduct(adjustedDirection, adjustedDirection),
                true));
        if (nlVec3DotProduct(direction, adjustedDirection) > 0.9f)
        {
            direction = adjustedDirection;
        }
        controller.SetDirection(direction);
    }
}

extern "C" void fn_801BEC38(EmissionController& controller)
{
    if (g_pGame == 0 || g_pGame->m_eGameState == 4)
    {
        return;
    }

    if (controller.m_GlView == 0
        && ReplayManager::Instance()->mRender != 0)
    {
        cCharacter* character = (cCharacter*)controller.m_uUserData;
        DrawableCharacter* drawableCharacter = fn_801BE428(character);
        controller.SetPosition(drawableCharacter->position);
        controller.SetVelocity(drawableCharacter->velocity);
        controller.SetPoseAccumulator(
            *drawableCharacter->poseAccumulator);

        nlMatrix4& matrix
            = drawableCharacter->poseAccumulator->GetNodeMatrix(0);
        nlVector3 direction;
        nlVec3Set(direction, matrix.m11 * -1.0f,
            matrix.m12 * -1.0f, matrix.m13);
        controller.SetDirection(direction);
    }
}

extern "C" void fn_801BED5C(EmissionController& controller)
{
    if (g_pGame == 0 || g_pGame->m_eGameState == 4)
    {
        return;
    }

    if (controller.m_GlView == 0
        && ReplayManager::Instance()->mRender != 0)
    {
        cCharacter* character = (cCharacter*)controller.m_uUserData;
        DrawableCharacter* drawableCharacter = fn_801BE428(character);
        controller.SetPosition(drawableCharacter->position);
        controller.SetVelocity(drawableCharacter->velocity);
        controller.SetPoseAccumulator(
            *drawableCharacter->poseAccumulator);

        drawableCharacter->poseAccumulator->m_BaseSHierarchy
            ->GetNodeIndexByID(character->m_nHeadJointIndex);
        nlMatrix4& matrix
            = drawableCharacter->poseAccumulator->GetNodeMatrix(0);
        nlVector3 direction;
        nlVec3Set(direction, matrix.m11, matrix.m12, matrix.m13);
        controller.SetDirection(direction);
    }
}

extern "C" void fn_801BEE8C(EmissionController& controller,
    ImpostorModel_802DAEE0* model)
{
    if (g_pGame == 0 || g_pGame->m_eGameState == 4)
    {
        return;
    }

    if (ReplayManager::Instance()->mRender != 0)
    {
        nlVector3 direction;
        nlVector3 velocity = { 0.0f, 0.0f, 0.0f };
        controller.SetPosition(model->mWorldMatrix.GetTranslation());
        controller.SetVelocity(velocity);
        controller.SetPoseAccumulator(*model->mPoseAccumulator);
        nlVec3Set(direction, model->mWorldMatrix.m11,
            model->mWorldMatrix.m12, model->mWorldMatrix.m13);
        controller.SetDirection(direction);
    }
}
