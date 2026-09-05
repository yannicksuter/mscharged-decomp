#include "Game/Character.h"

#include "Game/AI/HeadTrack.h"
#include "Game/Blinker.h"
#include "Game/CharacterEffects.h"
#include "Game/DebugWriteCache.h"
#include "Game/Effects/EmissionController.h"
#include "Game/Effects/EmissionManager.h"
#include "Game/ObjectBlur.h"
#include "Game/Physics/PhysicsCharacter.h"
#include "Game/PoseAccumulator.h"
#include "Game/SAnim/pnSAnimController.h"
#include "NL/nlMain.h"
#include "math.h"
#include <stddef.h>

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

void cCharacter::PrePhysicsUpdate()
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

u16 lbl_806DB602 = 0xFFFF;
u16 lbl_806DB604 = 0xFFFF;
extern u16 lbl_806DBD68;

struct UnidentifiedCharacterAnimState
{
    float m_fFrame;
    float m_fTotalFrames;
    float m_fPlaybackSpeedScale;
    float m_fTime;
    unsigned int m_nHashID;
    u32 m_nHS;
    float m_fDuration;
    unsigned int m_nNumRootKeys;
    float m_fLinearSpeed;
};

#define REGISTER_CHARACTER_FIELD(type, base, field, name) \
    fn_80338F88(cache, type, lbl_80533C98[type].size, \
        (u8*)&(field) - (u8*)&(base), name)

void cCharacter::Unknown11(void* context, DebugWriteCache* cache)
{
    if (lbl_806DB604 == 0xFFFF)
    {
        lbl_806DB604 = fn_80338EBC(cache, "DetChar");
        REGISTER_CHARACTER_FIELD(14, m_eCharacterClass,
            m_eCharacterClass, "m_eCharacterClass");
        REGISTER_CHARACTER_FIELD(14, m_eCharacterClass,
            m_eMovementState, "m_eMovementState");
        REGISTER_CHARACTER_FIELD(16, m_eCharacterClass,
            m_bFromAnimBlended, "m_bFromAnimBlended");
        REGISTER_CHARACTER_FIELD(16, m_eCharacterClass,
            m_bOnScreen, "m_bOnScreen");
        REGISTER_CHARACTER_FIELD(22, m_eCharacterClass,
            m_v3Position, "m_v3Position");
        REGISTER_CHARACTER_FIELD(22, m_eCharacterClass,
            m_v3PrevPosition, "m_v3PrevPosition");
        REGISTER_CHARACTER_FIELD(22, m_eCharacterClass,
            m_v3Velocity, "m_v3Velocity");
        REGISTER_CHARACTER_FIELD(22, m_eCharacterClass,
            m_v3PrevVelocity, "m_v3PrevVelocity");
        REGISTER_CHARACTER_FIELD(19, m_eCharacterClass,
            m_aDesiredFacingDirection, "m_aDesiredFacingDirection");
        REGISTER_CHARACTER_FIELD(19, m_eCharacterClass,
            m_aActualFacingDirection, "m_aActualFacingDirection");
        REGISTER_CHARACTER_FIELD(19, m_eCharacterClass,
            m_aPrevFacingDirection, "m_aPrevFacingDirection");
        REGISTER_CHARACTER_FIELD(19, m_eCharacterClass,
            m_aDesiredMovementDirection, "m_aDesiredMovementDirection");
        REGISTER_CHARACTER_FIELD(19, m_eCharacterClass,
            m_aActualMovementDirection, "m_aActualMovementDirection");
        REGISTER_CHARACTER_FIELD(17, m_eCharacterClass,
            m_fAnimAdjustBeginTime, "m_fAnimAdjustBeginTime");
        REGISTER_CHARACTER_FIELD(17, m_eCharacterClass,
            m_fAnimAdjustEndTime, "m_fAnimAdjustEndTime");
        REGISTER_CHARACTER_FIELD(17, m_eCharacterClass,
            m_fDirectionSeekSpeed, "m_fDirectionSeekSpeed");
        REGISTER_CHARACTER_FIELD(17, m_eCharacterClass,
            m_fDirectionSeekFalloff, "m_fDirectionSeekFalloff");
        REGISTER_CHARACTER_FIELD(17, m_eCharacterClass,
            m_fAccel, "m_fAccel");
        REGISTER_CHARACTER_FIELD(17, m_eCharacterClass,
            m_fDecel, "m_fDecel");
        REGISTER_CHARACTER_FIELD(17, m_eCharacterClass,
            m_fDesiredSpeed, "m_fDesiredSpeed");
        REGISTER_CHARACTER_FIELD(17, m_eCharacterClass,
            m_fActualSpeed, "m_fActualSpeed");
        REGISTER_CHARACTER_FIELD(17, m_eCharacterClass,
            m_fLeanAmount, "m_fLeanAmount");
        REGISTER_CHARACTER_FIELD(10, m_eCharacterClass,
            m_nAnimTurnAdjust, "m_nAnimTurnAdjust");
        REGISTER_CHARACTER_FIELD(22, m_eCharacterClass,
            m_v3AnimMoveAdjust, "m_v3AnimMoveAdjust");
        REGISTER_CHARACTER_FIELD(17, m_eCharacterClass,
            m_fPlayerScale, "m_fPlayerScale");
        REGISTER_CHARACTER_FIELD(17, m_eCharacterClass,
            m_fMovementScale, "m_fMovementScale");
        REGISTER_CHARACTER_FIELD(17, m_eCharacterClass,
            m_fDesiredPlayerScale, "m_fDesiredPlayerScale");
        REGISTER_CHARACTER_FIELD(17, m_eCharacterClass,
            m_fDesiredMovementScale, "m_fDesiredMovementScale");
        REGISTER_CHARACTER_FIELD(20, m_eCharacterClass,
            m_tScaleTimer, "m_tScaleTimer");
        fn_80338F78(cache);
    }
    fn_80339450(cache, lbl_806DB604, &m_eCharacterClass, context);
    fn_8033930C(cache, lbl_806DB604, &m_eCharacterClass,
        offsetof(cCharacter, m_pAnimInventory) - offsetof(cCharacter, m_eCharacterClass));

    UnidentifiedCharacterAnimState state;
    state.m_fFrame = m_pCurrentAnimController->m_fTime
        * (float)m_pCurrentAnimController->m_pSAnim->m_nNumKeys;
    state.m_fTotalFrames = (float)m_pCurrentAnimController->m_pSAnim->m_nNumKeys;
    state.m_fPlaybackSpeedScale = m_pCurrentAnimController->m_fPlaybackSpeedScale;
    state.m_fTime = m_pCurrentAnimController->m_fTime;
    cSAnim* anim = m_pCurrentAnimController->m_pSAnim;
    state.m_nHashID = anim->GetHashID();
    state.m_nHS = anim->m_nHierarchySignature;
    state.m_fDuration = anim->GetDuration();
    state.m_nNumRootKeys = (unsigned int)(float)anim->m_nNumKeys;
    state.m_fLinearSpeed = anim->m_fLinearSpeed;

    if (lbl_806DB602 == 0xFFFF)
    {
        lbl_806DB602 = fn_80338EBC(cache, "CharAnim");
        REGISTER_CHARACTER_FIELD(17, state, state.m_fFrame, "m_fFrame");
        REGISTER_CHARACTER_FIELD(17, state, state.m_fTotalFrames, "m_fTotalFrames");
        REGISTER_CHARACTER_FIELD(17, state, state.m_fPlaybackSpeedScale, "m_fPlaybackSpeedScale");
        REGISTER_CHARACTER_FIELD(17, state, state.m_fTime, "m_fTime");
        REGISTER_CHARACTER_FIELD(9, state, state.m_nHashID, "m_nHashID");
        REGISTER_CHARACTER_FIELD(2, state, state.m_nHS, "m_nHS");
        REGISTER_CHARACTER_FIELD(17, state, state.m_fDuration, "m_fDuration");
        REGISTER_CHARACTER_FIELD(9, state, state.m_nNumRootKeys, "m_nNumRootKeys");
        REGISTER_CHARACTER_FIELD(17, state, state.m_fLinearSpeed, "m_fLinearSpeed");
        fn_80338F78(cache);
    }
    fn_80339450(cache, lbl_806DB602, &state, context);
    fn_8033930C(cache, lbl_806DB602, &state, sizeof(state));

    cHeadTrack* headTrack = m_pHeadTrack;
    if (lbl_806DBD68 == 0xFFFF)
    {
        lbl_806DBD68 = fn_80338EBC(cache, "HeadTrack");
        REGISTER_CHARACTER_FIELD(26, *headTrack,
            headTrack->m_m4HeadMatrix, "m_m4HeadMatrix");
        REGISTER_CHARACTER_FIELD(22, *headTrack,
            headTrack->m_v3OOI, "m_v3OOI");
        REGISTER_CHARACTER_FIELD(16, *headTrack,
            headTrack->m_bTrackOOI, "m_bTrackOOI");
        REGISTER_CHARACTER_FIELD(17, *headTrack,
            headTrack->m_fHeadSpin, "m_fHeadSpin");
        REGISTER_CHARACTER_FIELD(17, *headTrack,
            headTrack->m_fHeadTilt, "m_fHeadTilt");
        REGISTER_CHARACTER_FIELD(17, *headTrack,
            headTrack->m_fDesiredHeadSpin, "m_fDesiredHeadSpin");
        REGISTER_CHARACTER_FIELD(17, *headTrack,
            headTrack->m_fDesiredHeadTilt, "m_fDesiredHeadTilt");
        REGISTER_CHARACTER_FIELD(17, *headTrack,
            headTrack->m_fHeadSpinSeekVel, "m_fHeadSpinSeekVel");
        REGISTER_CHARACTER_FIELD(17, *headTrack,
            headTrack->m_fHeadTiltSeekVel, "m_fHeadTiltSeekVel");
        REGISTER_CHARACTER_FIELD(17, *headTrack,
            headTrack->m_fSmoothTime, "mfSmoothTime");
        fn_80338F78(cache);
    }
    fn_80339450(cache, lbl_806DBD68, headTrack, context);
    fn_8033930C(cache, lbl_806DBD68, headTrack, sizeof(cHeadTrack));
}

#undef REGISTER_CHARACTER_FIELD

void cCharacter::Unknown12(RunningChecksum* pChecksum)
{
    pChecksum->ChecksumData(&m_eCharacterClass, sizeof(m_eCharacterClass));
    pChecksum->ChecksumData(&m_eMovementState, sizeof(m_eMovementState));
    pChecksum->ChecksumData(&m_bOnScreen, sizeof(m_bOnScreen));
    pChecksum->ChecksumData(&m_v3Position, sizeof(m_v3Position));
    pChecksum->ChecksumData(&m_v3Velocity, sizeof(m_v3Velocity));
    pChecksum->ChecksumData(&m_aDesiredFacingDirection, sizeof(m_aDesiredFacingDirection));
    pChecksum->ChecksumData(&m_aActualFacingDirection, sizeof(m_aActualFacingDirection));
    pChecksum->ChecksumData(&m_aDesiredMovementDirection, sizeof(m_aDesiredMovementDirection));
    pChecksum->ChecksumData(&m_aActualMovementDirection, sizeof(m_aActualMovementDirection));
    pChecksum->ChecksumData(&m_fAccel, sizeof(m_fAccel));
    pChecksum->ChecksumData(&m_fDecel, sizeof(m_fDecel));
    pChecksum->ChecksumData(&m_fDesiredSpeed, sizeof(m_fDesiredSpeed));
    pChecksum->ChecksumData(&m_fActualSpeed, sizeof(m_fActualSpeed));
    pChecksum->ChecksumData(&m_nAnimTurnAdjust, sizeof(m_nAnimTurnAdjust));
}
