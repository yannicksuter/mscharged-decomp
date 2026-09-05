#include "Game/Effects/EmitterCallbacks.h"
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

DrawableCharacter* GetReplayDrawableCharacter(cCharacter* character)
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

void UpdateEmitterFromCharacterUnculled(EmissionController& controller)
{
    if (g_pGame == 0 || g_pGame->m_eGameState == 4)
    {
        return;
    }

    if (ReplayManager::Instance()->mRender != 0)
    {
        cCharacter* character = (cCharacter*)controller.m_uUserData;
        DrawableCharacter* drawableCharacter = GetReplayDrawableCharacter(character);
        controller.SetPosition(drawableCharacter->position);
        controller.SetVelocity(drawableCharacter->velocity);
        controller.SetPoseAccumulator(
            *drawableCharacter->poseAccumulator);
        controller.SetAnimController(
            drawableCharacter->GetAnimController());
    }
}

void UpdateEmitterFromCharacterWithoutAnimController(
    EmissionController& controller, cCharacter* character)
{
    if (g_pGame == 0 || g_pGame->m_eGameState == 4)
    {
        return;
    }

    if (ReplayManager::Instance()->mRender != 0)
    {
        DrawableCharacter* drawableCharacter = GetReplayDrawableCharacter(character);
        controller.SetPosition(drawableCharacter->position);
        controller.SetVelocity(drawableCharacter->velocity);
        controller.SetPoseAccumulator(
            *drawableCharacter->poseAccumulator);
    }
}

void UpdateEmitterFromCharacter(EmissionController& ec)
{
    if (g_pGame == 0 || g_pGame->m_eGameState == 4)
    {
        return;
    }

    if (ReplayManager::Instance()->mRender != 0)
    {
        UpdateEmitterFromCharacterUnculled(ec);

        cCharacter* pCharacter
            = DrawableCharacter::OnlyRenderingOneCharacter();
        if (pCharacter != 0
            && (cCharacter*)ec.m_uUserData != pCharacter)
        {
            ec.Die();
        }
    }
}

void UpdateEmitterPoseFromCharacter(EmissionController& emitter)
{
    if (g_pGame == 0 || g_pGame->m_eGameState == 4)
    {
        return;
    }

    if (ReplayManager::Instance()->mRender != 0)
    {
        cCharacter* character = (cCharacter*)emitter.m_uUserData;
        DrawableCharacter* pChar = GetReplayDrawableCharacter(character);
        emitter.SetPoseAccumulator(
            *pChar->poseAccumulator);
        emitter.SetAnimController(
            pChar->GetAnimController());
    }
}

void UpdateEmitterFromBall(EmissionController& emitter)
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
            emitter.m_bVisible = ReplayManager::Instance()
                                     ->mRender->mBall.mFlags.bits.visible;
        }

        ReplayManager* manager = ReplayManager::Instance();
        emitter.SetPosition(manager->mRender->mBall.mPosition);
        manager = ReplayManager::Instance();
        emitter.SetVelocity(manager->mRender->mBall.mVelocity);
    }
}

void UpdateEmitterFromBallTrail(EmissionController& controller)
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

void UpdateEmitterFromBallLandingSpot(EmissionController& controller)
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

void UpdateEmitterFromCharacterHead(EmissionController& controller)
{
    if (g_pGame == 0 || g_pGame->m_eGameState == 4)
    {
        return;
    }

    if (controller.m_GlView == 0
        && ReplayManager::Instance()->mRender != 0)
    {
        cCharacter* character = (cCharacter*)controller.m_uUserData;
        DrawableCharacter* drawableCharacter = GetReplayDrawableCharacter(character);
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

void UpdateEmitterFromCharacterBackward(EmissionController& controller)
{
    if (g_pGame == 0 || g_pGame->m_eGameState == 4)
    {
        return;
    }

    if (controller.m_GlView == 0
        && ReplayManager::Instance()->mRender != 0)
    {
        cCharacter* character = (cCharacter*)controller.m_uUserData;
        DrawableCharacter* drawableCharacter = GetReplayDrawableCharacter(character);
        controller.SetPosition(drawableCharacter->position);
        controller.SetVelocity(drawableCharacter->velocity);
        controller.SetPoseAccumulator(
            *drawableCharacter->poseAccumulator);

        nlMatrix4& matrix
            = drawableCharacter->poseAccumulator->GetNodeMatrix(0);
        nlVector3 direction;
        nlVec3Set(direction, matrix.m11, matrix.m12, matrix.m13);
        direction.x *= -1.0f;
        direction.y *= -1.0f;
        controller.SetDirection(direction);
    }
}

void UpdateEmitterFromCharacterForward(EmissionController& controller)
{
    if (g_pGame == 0 || g_pGame->m_eGameState == 4)
    {
        return;
    }

    if (controller.m_GlView == 0
        && ReplayManager::Instance()->mRender != 0)
    {
        cCharacter* character = (cCharacter*)controller.m_uUserData;
        DrawableCharacter* drawableCharacter = GetReplayDrawableCharacter(character);
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

void UpdateEmitterFromImpostorModel(EmissionController& controller,
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
        nlVec3Set(direction, model->mWorldMatrix.m11, model->mWorldMatrix.m12, model->mWorldMatrix.m13);
        controller.SetDirection(direction);
    }
}
