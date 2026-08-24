#include "Game/Character.h"

#include "Game/Blinker.h"
#include "Game/CharacterEffects.h"
#include "Game/Effects/EmissionController.h"
#include "Game/Effects/EmissionManager.h"
#include "Game/ObjectBlur.h"
#include "Game/Physics/PhysicsCharacter.h"
#include "Game/PoseAccumulator.h"
#include "Game/SAnim/pnSAnimController.h"
#include "math.h"

void cCharacter::SetElectrocutionTextureEnabled(bool isEnabled)
{
    if ((m_bIsUsingElectrocutionTexture == false) && (isEnabled != false))
    {
        m_pEffectsTexturing = fxGetTexturing(eFXTex_Electrocution);
    }

    if ((m_bIsUsingElectrocutionTexture != false) && (isEnabled == false))
    {
        m_pEffectsTexturing = 0;
    }

    m_bIsUsingElectrocutionTexture = isEnabled;
}

void cCharacter::PerformBlinking(GLSkinMesh* skinMesh, glModel* model) const
{
    Blinker* pBlinker = m_pBlinker;
    if (pBlinker != 0)
    {
        pBlinker->Blink(model);
    }
}

void cCharacter::UpdateBlinking(float fDeltaT)
{
    Blinker* pBlinker = m_pBlinker;
    if (pBlinker != 0)
    {
        pBlinker->Update(fDeltaT);
    }
}

bool cCharacter::IsPlayingEffect(const EffectsGroup* effectGroup) const
{
    return EmissionManager::Instance()->IsPlaying(
        (unsigned long)this, effectGroup);
}

void cCharacter::EndEffect(const EffectsGroup* effectGroup)
{
    EmissionManager::Instance()->Kill((unsigned long)this, effectGroup);
}

void cCharacter::KillEffect(const EffectsGroup* effectGroup)
{
    EmissionManager::Instance()->Destroy(
        (unsigned long)this, effectGroup);
}

void cCharacter::SetVelocity(const nlVector3& velocity)
{
    m_v3Velocity = velocity;
    m_pPhysicsCharacter->SetCharacterVelocityXY(m_v3Velocity);
}

void cCharacter::SetPosition(const nlVector3& position)
{
    m_v3Position = position;
    m_v3PrevPosition = m_v3Position;
    m_pPhysicsCharacter->SetCharacterPositionXY(m_v3Position);
}

void cCharacter::SetFacingDirection(
    unsigned short dir, bool bSetMovementDirection)
{
    m_aPrevFacingDirection = m_aActualFacingDirection;
    m_aActualFacingDirection = dir;
    m_pPhysicsCharacter->SetFacingDirection(dir);
    if (bSetMovementDirection)
    {
        m_aActualMovementDirection = dir;
    }
}

float cCharacter::SeekSpeedExponential(float currentValue, float targetValue,
    float responsiveness, float deltaTime)
{
    float adjustment;
    float distance;
    float difference;

    difference = targetValue - currentValue;
    distance = fabs(difference);

    if (distance > 0.1f)
    {
        adjustment = distance
                   - (1.0f
                       / ((responsiveness * deltaTime) + (1.0f / distance)));
        if (difference > 0.0f)
        {
            return currentValue + adjustment;
        }
        return currentValue - adjustment;
    }

    return targetValue;
}

void cCharacter::ResetEffects()
{
    EmissionManager::Instance()->Destroy((unsigned long)this, 0);
    m_pEffectsTexturing = 0;
}

void cCharacter::PrePhysicsUpdate(float dt)
{
}

void cCharacter::SetAnimID(int animID)
{
}

void cCharacter::PreUpdate(float dt)
{
}

void cCharacter::PostPhysicsUpdate()
{
    m_v3PrevPosition = m_v3Position;
    m_pPhysicsCharacter->GetCharacterPositionXY(&m_v3Position);
    m_pPhysicsCharacter->GetCharacterVelocityXY(&m_v3Velocity);

    m_fActualSpeed = nlGetLength2D(m_v3Velocity.x, m_v3Velocity.y);

    float angleRad = 0.0000958738f * (float)m_aActualFacingDirection;
    nlMakeRotationMatrixZ(m_m4WorldMatrix, angleRad);

    m_m4WorldMatrix.e2[3][0] = m_v3Position.x;
    m_m4WorldMatrix.e2[3][1] = m_v3Position.y;
    m_m4WorldMatrix.e2[3][2] = m_v3Position.z;

    m_pPoseAccumulator->Pose(*m_pPoseTree, m_m4WorldMatrix);
    m_pPhysicsCharacter->UpdatePose(
        m_pPoseAccumulator, m_v3Position.z, false);
}

void cCharacter::InitMovementStrafing(float fDirectionSeekSpeed,
    float fDirectionSeekFalloff, float fAccel, float fDecel)
{
    m_eMovementState = MOVEMENT_STRAFING;
    m_fDirectionSeekSpeed = fDirectionSeekSpeed;
    m_fDirectionSeekFalloff = fDirectionSeekFalloff;
    m_fAccel = fAccel;
    m_fDecel = fDecel;
}

void cCharacter::InitMovementRunningNoTurn(float fAccel, float fDecel)
{
    m_eMovementState = MOVEMENT_RUNNING_NO_TURN;
    m_fAccel = fAccel;
    m_fDecel = fDecel;
}

void cCharacter::InitMovementRunning(float fDirectionSeekSpeed,
    float fDirectionSeekFalloff, float fAccel, float fDecel)
{
    m_eMovementState = MOVEMENT_RUNNING;
    m_fDirectionSeekSpeed = fDirectionSeekSpeed;
    m_fDirectionSeekFalloff = fDirectionSeekFalloff;
    m_fAccel = fAccel;
    m_fDecel = fDecel;
}

void cCharacter::InitMovementNone(
    float fDirectionSeekSpeed, float fDirectionSeekFalloff)
{
    m_eMovementState = MOVEMENT_NONE;
    m_fDirectionSeekSpeed = fDirectionSeekSpeed;
    m_fDirectionSeekFalloff = fDirectionSeekFalloff;
}

void cCharacter::InitMovementFromAnimSeek(
    float fDirectionSeekSpeed, float fDirectionSeekFalloff)
{
    m_eMovementState = MOVEMENT_FROM_ANIM_SEEK;
    m_fDirectionSeekSpeed = fDirectionSeekSpeed;
    m_fDirectionSeekFalloff = fDirectionSeekFalloff;
}

void cCharacter::InitMovementFromAnim(short fDirectionSeekSpeed,
    const nlVector3& v3AnimMoveAdjust, float fAdjustEndTime, bool bBlended)
{
    m_eMovementState = MOVEMENT_FROM_ANIM;
    m_nAnimTurnAdjust = fDirectionSeekSpeed;
    m_v3AnimMoveAdjust = v3AnimMoveAdjust;
    m_fAnimAdjustBeginTime = m_pCurrentAnimController->m_fTime;
    m_fAnimAdjustEndTime = fAdjustEndTime;
    m_bFromAnimBlended = bBlended;
}

void cCharacter::InitMovementDecelerateExponential(float fDecel)
{
    m_eMovementState = MOVEMENT_DECELERATE_EXPONENTIAL;
    m_fDecel = fDecel;
}

void cCharacter::InitMovementCoast()
{
    m_eMovementState = MOVEMENT_COAST;
}

void cCharacter::EndBlur()
{
    if (m_pBlurHandler != 0)
    {
        m_pBlurHandler->Die(0.0f);
        m_pBlurHandler = 0;
    }
}

nlVector3& cCharacter::GetPrevJointPosition(int jointIndex)
{
    nlMatrix4& prevMatrix = m_pPoseAccumulator->m_PrevNodeMatrices[jointIndex];
    return *(nlVector3*)&prevMatrix.e2[3];
}

nlVector3& cCharacter::GetJointPosition(int jointIndex) const
{
    const nlMatrix4& poseMatrix = m_pPoseAccumulator->GetNodeMatrix(jointIndex);
    return *(nlVector3*)&poseMatrix.e2[3];
}

s16 cCharacter::GetFacingDeltaToPosition(const nlVector3& position)
{
    float dx = position.x - m_v3Position.x;
    float dy = position.y - m_v3Position.y;
    float angleRad = nlATan2f(dy, dx);
    float angle16 = 10430.378f * angleRad;
    u16 targetAngle = (u16)(s32)angle16;

    return (s16)(targetAngle - m_aActualFacingDirection);
}

void cCharacter::AttachEffect(EmissionController* pEmissionController)
{
    pEmissionController->m_uUserData = (u32)this;
    pEmissionController->SetPoseAccumulator(*m_pPoseAccumulator);
    pEmissionController->SetAnimController(*m_pCurrentAnimController);
    pEmissionController->m_aFacing = m_aActualFacingDirection;
}

GLSkinMesh* cCharacter::GetSkinMesh(int modelType) const
{
    GLSkinMesh* skinMesh = m_pSkinMesh[modelType];
    if (skinMesh != 0)
    {
        return skinMesh;
    }
    return m_pSkinMesh[0];
}
