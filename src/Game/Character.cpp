#include "Game/AI/ShotMeter.h"
#include "Game/Field.h"
#include "Game/Game.h"
#include "Game/Goalie.h"
#include "Game/GameInfo.h"
#include "Game/GameTweaks.h"
#include "Game/Terrain.h"
#include "Game/Render/ElectricFence.h"
#include "Game/Render/NPCManager.h"
#include "Game/Render/ShootToScoreMeter.h"
#include "Game/Physics/PhysicsAIBall.h"
#include "Game/Character.h"

#include "Game/AI/HeadTrack.h"
#include "Game/AI/AiUtil.h"
#include "Game/AI/Fielder.h"
#include "Game/AI/Powerups.h"
#include "Game/AI/FielderActions.h"
#include "Game/Audio/GameStreams.h"
#include "Game/DB/StadiumInfo.h"
#include "Game/Sys/audio.h"
#include "Game/TweakRegistry.h"
#include "Game/Team.h"
#include "Game/AnimInventory.h"
#include "Game/CharacterTweaks.h"
#include "Game/Ball.h"
#include "Game/Render/BirdoEgg.h"
#include "Game/Render/YoshiEggObject.h"
#include "Game/Physics/PhysicsYoshiEgg.h"
#include "Game/Render/BulletBill.h"
#include "Game/Physics/PhysicsShockwave.h"
#include "Game/Render/KoopaShellObject.h"
#include "Game/ExcitementSystem.h"
#include "Game/UnidentifiedBitPacker.h"
#include "Game/CharacterTriggers.h"
#include "Game/SAnim/pnBlender.h"
#include "Game/Blinker.h"
#include "Game/CharacterEffects.h"
#include "Game/DB/CharacterInfo.h"
#include "Game/EventRegistry.h"
#include "Game/GameEventQueue.h"
#include "Game/DebugWriteCache.h"
#include "Game/Effects/EmissionController.h"
#include "Game/Effects/EmissionManager.h"
#include "Game/ObjectBlur.h"
#include "Game/GL/GLInventory.h"
#include "Game/GL/ShaderSkinMesh.h"
#include "Game/Physics/CollisionSpace.h"
#include "Game/Physics/PhysicsCharacter.h"
#include "Game/Physics/PhysicsColumn.h"
#include "Game/Physics/PhysicsGoalie.h"
#include "Game/PoseAccumulator.h"
#include "Game/SHierarchy.h"
#include "Game/SAnim/pnSAnimController.h"
#include "NL/nlBind_impl.h"
#include "NL/nlMain.h"
#include "NL/nlMath.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"
#include "NL/gl/glMemory.h"
#include "NL/gl/glState.h"
#include "NL/gl/glTexture.h"
#include "NL/gl/glTextureManager.h"
#include "NL/gl/glMaterialParameters.h"
#include "NL/glx/GXCharacterDamageMaterialProgram.h"
#include "math.h"
#include <stddef.h>
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/Audio/RegistryPools.h"

extern PhysicsWorld* g_PhysicsWorld;

static const nlVector3 v3Zero = { 0.0f, 0.0f, 0.0f };

void cCharacter::fn_80022D3C(float fParam0, float fParam1)
{
    mUnidentified1A4 = fParam0;
    mUnidentified1AC = fParam1;
    if (fParam0 == 0.0f)
    {
        mUnidentified1A8 = fParam1;
    }
}

void cCharacter::fn_80022D58(float fDeltaT)
{
    mUnidentified1A8 += mUnidentified1A4 * fDeltaT;
    if (mUnidentified1A4 > 0.0f && mUnidentified1A8 > mUnidentified1AC)
    {
        mUnidentified1A8 = mUnidentified1AC;
    }
    if (mUnidentified1A4 < 0.0f && mUnidentified1A8 < mUnidentified1AC)
    {
        mUnidentified1A8 = mUnidentified1AC;
    }
}

void cCharacter::fn_80022DAC(unsigned long uTextureID)
{
    mUnidentified100 = uTextureID;
    mUnidentified10C.value = glGetTextureManager()->GetTextureIndex(mUnidentified100);
}

void cCharacter::fn_80022DE8(unsigned long uTextureID)
{
    mUnidentified104 = uTextureID;
    mUnidentified110.value = glGetTextureManager()->GetTextureIndex(mUnidentified104);
}

void cCharacter::fn_80022E24(unsigned long uTextureID)
{
    mUnidentified108 = uTextureID;
    mUnidentified114.value = glGetTextureManager()->GetTextureIndex(mUnidentified108);
}

struct UnidentifiedCharacterObject_8001C158
{
    u32 mUnidentified00;
};

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

void cCharacter::fn_8001F1D8()
{
    m_MinDirt += 0.5f + nlRandomf(0.5f);
    if (m_MinDirt > 1.0f)
    {
        m_MinDirt = 1.0f;
    }
}

void cCharacter::PerformBlinking(GLSkinMesh* skinMesh, glModel* model) const
{
    Blinker* pBlinker = m_pBlinker;
    if (pBlinker != 0)
    {
        pBlinker->Blink(model);
    }
}

void cCharacter::StopPlayingAllTrackedSFX()
{
}

void cCharacter::AddRandomDirt()
{
    m_Dirt += 0.5f + nlRandomf(0.5f);
    if (m_Dirt > 1.0f)
    {
        m_Dirt = 1.0f;
    }
}

void cCharacter::fn_8001F1C0(int nParam)
{
    mUnidentified16C = nParam;
    if (nParam == 2)
    {
        m_MinDirt = 0.0f;
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

bool cCharacter::fn_8001E2C0(const EffectsGroup* effectGroup) const
{
    return EmissionManager::Instance()->IsDying(
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
    mUnidentified024.m_v3Velocity = velocity;
    m_pPhysicsCharacter->SetCharacterVelocityXY(mUnidentified024.m_v3Velocity);
}

void cCharacter::SetPosition(const nlVector3& position)
{
    mUnidentified024.m_v3Position = position;
    mUnidentified024.m_v3PrevPosition = mUnidentified024.m_v3Position;
    m_pPhysicsCharacter->SetCharacterPositionXY(mUnidentified024.m_v3Position);
}

void cCharacter::Unknown8(unsigned short aDirection, bool bParam)
{
    mUnidentified024.m_aDesiredFacingDirection = aDirection;
    if (bParam)
    {
        mUnidentified024.m_aDesiredMovementDirection = aDirection;
    }
}

void cCharacter::SetFacingDirection(
    unsigned short dir, bool bSetMovementDirection)
{
    mUnidentified024.m_aPrevFacingDirection = mUnidentified024.m_aActualFacingDirection;
    mUnidentified024.m_aActualFacingDirection = dir;
    m_pPhysicsCharacter->SetFacingDirection(dir);
    if (bSetMovementDirection)
    {
        mUnidentified024.m_aActualMovementDirection = dir;
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
    if (m_eAnimID != animID)
    {
        m_eAnimID = animID;
        ExcitementSystem& unidentifiedSystem = ExcitementSystem::fn_80196644();
        nlVector3 unidentifiedDelta;
        nlVec3Sub(unidentifiedDelta, mUnidentified024.m_v3Position, g_pBall->m_v3Position);
        if (nlVec3LengthSquared(unidentifiedDelta) < unidentifiedSystem.mUnidentified028)
        {
            if (m_eClassType == GOALIE)
            {
                u8 unidentifiedValue = unidentifiedSystem.mUnidentified0B2[(u16)m_eAnimID];
                if (unidentifiedValue != 0)
                {
                    unidentifiedSystem.mUnidentified02C += unidentifiedValue;
                    unidentifiedSystem.mUnidentified02E++;
                }
            }
            else if (m_eClassType == FIELDER)
            {
                u8 unidentifiedValue = unidentifiedSystem.mUnidentified030[(u16)m_eAnimID];
                if (unidentifiedValue != 0)
                {
                    unidentifiedSystem.mUnidentified02C += unidentifiedValue;
                    unidentifiedSystem.mUnidentified02E++;
                }
            }
        }
    }
}

void cCharacter::Unknown7(float dt)
{
}

void cCharacter::PreUpdate(float dt)
{
}

inline void cCharacter::CreateWorldMatrix()
{
    nlMakeRotationMatrixZ(m_m4WorldMatrix,
        0.0000958738f * (float)mUnidentified024.m_aActualFacingDirection);
    m_m4WorldMatrix.e2[3][0] = mUnidentified024.m_v3Position.x;
    m_m4WorldMatrix.e2[3][1] = mUnidentified024.m_v3Position.y;
    m_m4WorldMatrix.e2[3][2] = mUnidentified024.m_v3Position.z;
}

void cCharacter::PostPhysicsUpdate()
{
    mUnidentified024.m_v3PrevPosition = mUnidentified024.m_v3Position;
    m_pPhysicsCharacter->GetCharacterPositionXY(&mUnidentified024.m_v3Position);
    m_pPhysicsCharacter->GetCharacterVelocityXY(&mUnidentified024.m_v3Velocity);

    mUnidentified024.m_fActualSpeed = nlGetLength2D(mUnidentified024.m_v3Velocity.x, mUnidentified024.m_v3Velocity.y);

    CreateWorldMatrix();

    m_pPoseAccumulator->Pose(*m_pPoseTree, m_m4WorldMatrix);
    m_pPhysicsCharacter->UpdatePose(
        m_pPoseAccumulator, mUnidentified024.m_v3Position.z, false);
}

void cCharacter::Unknown10(const nlVector3& v3Position, unsigned short aDirection)
{
    mUnidentified024.UnidentifiedReset();
    UnidentifiedVirtual1C();
    m_pHeadTrack->UnidentifiedReset();
    m_pPhysicsCharacter->Unknown0();
    SetPosition(v3Position);
    mUnidentified024.m_v3PrevPosition = v3Position;
    mUnidentified024.m_aDesiredFacingDirection = aDirection;
    SetFacingDirection(aDirection, false);
    mUnidentified024.m_aPrevFacingDirection = mUnidentified024.m_aDesiredFacingDirection;
    mUnidentified024.m_aActualMovementDirection = mUnidentified024.m_aDesiredFacingDirection;
    mUnidentified024.m_aDesiredMovementDirection = mUnidentified024.m_aDesiredFacingDirection;
    SetVelocity(v3Zero);
    mUnidentified024.m_fActualSpeed = 0.0f;
    mUnidentified024.m_fDesiredSpeed = 0.0f;
    CreateWorldMatrix();
    m_pPoseAccumulator->Pose(*m_pPoseTree, m_m4WorldMatrix);
    m_pPhysicsCharacter->UpdatePose(m_pPoseAccumulator, 0.0f, false);
    m_pPhysicsCharacter->UpdatePose(m_pPoseAccumulator, 0.0f, false);
    mUnidentified17E = false;
    mUnidentified17F = false;
    mUnidentified180 = false;
    mUnidentified181 = false;
    mUnidentified182 = false;
    m_bIsUsingElectrocutionTexture = false;
    m_ModelType = 0;
    mUnidentified178 = 1.0f;
    mUnidentified17C = true;
    ResetEffects();
    EndBlur();
    fn_8001C574();
}

void cCharacter::InitMovementStrafing(float fDirectionSeekSpeed,
    float fDirectionSeekFalloff, float fAccel, float fDecel)
{
    mUnidentified024.m_eMovementState = MOVEMENT_STRAFING;
    mUnidentified024.m_fDirectionSeekSpeed = fDirectionSeekSpeed;
    mUnidentified024.m_fDirectionSeekFalloff = fDirectionSeekFalloff;
    mUnidentified024.m_fAccel = fAccel;
    mUnidentified024.m_fDecel = fDecel;
}

void cCharacter::InitMovementRunningNoTurn(float fAccel, float fDecel)
{
    mUnidentified024.m_eMovementState = MOVEMENT_RUNNING_NO_TURN;
    mUnidentified024.m_fAccel = fAccel;
    mUnidentified024.m_fDecel = fDecel;
}

void cCharacter::InitMovementRunning(float fDirectionSeekSpeed,
    float fDirectionSeekFalloff, float fAccel, float fDecel)
{
    mUnidentified024.m_eMovementState = MOVEMENT_RUNNING;
    mUnidentified024.m_fDirectionSeekSpeed = fDirectionSeekSpeed;
    mUnidentified024.m_fDirectionSeekFalloff = fDirectionSeekFalloff;
    mUnidentified024.m_fAccel = fAccel;
    mUnidentified024.m_fDecel = fDecel;
}

void cCharacter::InitMovementNone(
    float fDirectionSeekSpeed, float fDirectionSeekFalloff)
{
    mUnidentified024.m_eMovementState = MOVEMENT_NONE;
    mUnidentified024.m_fDirectionSeekSpeed = fDirectionSeekSpeed;
    mUnidentified024.m_fDirectionSeekFalloff = fDirectionSeekFalloff;
}

void cCharacter::InitMovementFromAnimSeek(
    float fDirectionSeekSpeed, float fDirectionSeekFalloff)
{
    mUnidentified024.m_eMovementState = MOVEMENT_FROM_ANIM_SEEK;
    mUnidentified024.m_fDirectionSeekSpeed = fDirectionSeekSpeed;
    mUnidentified024.m_fDirectionSeekFalloff = fDirectionSeekFalloff;
}

void cCharacter::InitMovementFromAnim(short fDirectionSeekSpeed,
    const nlVector3& v3AnimMoveAdjust, float fAdjustEndTime, bool bBlended)
{
    mUnidentified024.m_eMovementState = MOVEMENT_FROM_ANIM;
    mUnidentified024.m_nAnimTurnAdjust = fDirectionSeekSpeed;
    mUnidentified024.m_v3AnimMoveAdjust = v3AnimMoveAdjust;
    mUnidentified024.m_fAnimAdjustBeginTime = m_pCurrentAnimController->m_fTime;
    mUnidentified024.m_fAnimAdjustEndTime = fAdjustEndTime;
    mUnidentified024.m_bFromAnimBlended = bBlended;
}

void cCharacter::InitMovementDecelerateExponential(float fDecel)
{
    mUnidentified024.m_eMovementState = MOVEMENT_DECELERATE_EXPONENTIAL;
    mUnidentified024.m_fDecel = fDecel;
}

void cCharacter::InitMovementCoast()
{
    mUnidentified024.m_eMovementState = MOVEMENT_COAST;
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
    float dx = position.x - mUnidentified024.m_v3Position.x;
    float dy = position.y - mUnidentified024.m_v3Position.y;
    float angleRad = nlATan2f(dy, dx);
    float angle16 = 10430.378f * angleRad;
    u16 targetAngle = (u16)(s32)angle16;

    return (s16)(targetAngle - mUnidentified024.m_aActualFacingDirection);
}

void cCharacter::AttachEffect(EmissionController* pEmissionController)
{
    pEmissionController->m_uUserData = (u32)this;
    pEmissionController->SetPoseAccumulator(*m_pPoseAccumulator);
    pEmissionController->SetAnimController(*m_pCurrentAnimController);
    pEmissionController->m_aFacing = mUnidentified024.m_aActualFacingDirection;
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
    cache->AddField(type, gDebugFieldTypes[type].size, \
        (u8*)&(field) - (u8*)&(base), name)

void cCharacter::Unknown11(void* context, DebugWriteCache* cache)
{
    if (lbl_806DB604 == 0xFFFF)
    {
        lbl_806DB604 = cache->BeginType("DetChar");
        REGISTER_CHARACTER_FIELD(14, mUnidentified024.m_eCharacterClass,
            mUnidentified024.m_eCharacterClass, "m_eCharacterClass");
        REGISTER_CHARACTER_FIELD(14, mUnidentified024.m_eCharacterClass,
            mUnidentified024.m_eMovementState, "m_eMovementState");
        REGISTER_CHARACTER_FIELD(16, mUnidentified024.m_eCharacterClass,
            mUnidentified024.m_bFromAnimBlended, "m_bFromAnimBlended");
        REGISTER_CHARACTER_FIELD(16, mUnidentified024.m_eCharacterClass,
            mUnidentified024.m_bOnScreen, "m_bOnScreen");
        REGISTER_CHARACTER_FIELD(22, mUnidentified024.m_eCharacterClass,
            mUnidentified024.m_v3Position, "m_v3Position");
        REGISTER_CHARACTER_FIELD(22, mUnidentified024.m_eCharacterClass,
            mUnidentified024.m_v3PrevPosition, "m_v3PrevPosition");
        REGISTER_CHARACTER_FIELD(22, mUnidentified024.m_eCharacterClass,
            mUnidentified024.m_v3Velocity, "m_v3Velocity");
        REGISTER_CHARACTER_FIELD(22, mUnidentified024.m_eCharacterClass,
            mUnidentified024.m_v3PrevVelocity, "m_v3PrevVelocity");
        REGISTER_CHARACTER_FIELD(19, mUnidentified024.m_eCharacterClass,
            mUnidentified024.m_aDesiredFacingDirection, "m_aDesiredFacingDirection");
        REGISTER_CHARACTER_FIELD(19, mUnidentified024.m_eCharacterClass,
            mUnidentified024.m_aActualFacingDirection, "m_aActualFacingDirection");
        REGISTER_CHARACTER_FIELD(19, mUnidentified024.m_eCharacterClass,
            mUnidentified024.m_aPrevFacingDirection, "m_aPrevFacingDirection");
        REGISTER_CHARACTER_FIELD(19, mUnidentified024.m_eCharacterClass,
            mUnidentified024.m_aDesiredMovementDirection, "m_aDesiredMovementDirection");
        REGISTER_CHARACTER_FIELD(19, mUnidentified024.m_eCharacterClass,
            mUnidentified024.m_aActualMovementDirection, "m_aActualMovementDirection");
        REGISTER_CHARACTER_FIELD(17, mUnidentified024.m_eCharacterClass,
            mUnidentified024.m_fAnimAdjustBeginTime, "m_fAnimAdjustBeginTime");
        REGISTER_CHARACTER_FIELD(17, mUnidentified024.m_eCharacterClass,
            mUnidentified024.m_fAnimAdjustEndTime, "m_fAnimAdjustEndTime");
        REGISTER_CHARACTER_FIELD(17, mUnidentified024.m_eCharacterClass,
            mUnidentified024.m_fDirectionSeekSpeed, "m_fDirectionSeekSpeed");
        REGISTER_CHARACTER_FIELD(17, mUnidentified024.m_eCharacterClass,
            mUnidentified024.m_fDirectionSeekFalloff, "m_fDirectionSeekFalloff");
        REGISTER_CHARACTER_FIELD(17, mUnidentified024.m_eCharacterClass,
            mUnidentified024.m_fAccel, "m_fAccel");
        REGISTER_CHARACTER_FIELD(17, mUnidentified024.m_eCharacterClass,
            mUnidentified024.m_fDecel, "m_fDecel");
        REGISTER_CHARACTER_FIELD(17, mUnidentified024.m_eCharacterClass,
            mUnidentified024.m_fDesiredSpeed, "m_fDesiredSpeed");
        REGISTER_CHARACTER_FIELD(17, mUnidentified024.m_eCharacterClass,
            mUnidentified024.m_fActualSpeed, "m_fActualSpeed");
        REGISTER_CHARACTER_FIELD(17, mUnidentified024.m_eCharacterClass,
            mUnidentified024.m_fLeanAmount, "m_fLeanAmount");
        REGISTER_CHARACTER_FIELD(10, mUnidentified024.m_eCharacterClass,
            mUnidentified024.m_nAnimTurnAdjust, "m_nAnimTurnAdjust");
        REGISTER_CHARACTER_FIELD(22, mUnidentified024.m_eCharacterClass,
            mUnidentified024.m_v3AnimMoveAdjust, "m_v3AnimMoveAdjust");
        REGISTER_CHARACTER_FIELD(17, mUnidentified024.m_eCharacterClass,
            mUnidentified024.m_fPlayerScale, "m_fPlayerScale");
        REGISTER_CHARACTER_FIELD(17, mUnidentified024.m_eCharacterClass,
            mUnidentified024.m_fMovementScale, "m_fMovementScale");
        REGISTER_CHARACTER_FIELD(17, mUnidentified024.m_eCharacterClass,
            mUnidentified024.m_fDesiredPlayerScale, "m_fDesiredPlayerScale");
        REGISTER_CHARACTER_FIELD(17, mUnidentified024.m_eCharacterClass,
            mUnidentified024.m_fDesiredMovementScale, "m_fDesiredMovementScale");
        REGISTER_CHARACTER_FIELD(20, mUnidentified024.m_eCharacterClass,
            mUnidentified024.m_tScaleTimer, "m_tScaleTimer");
        cache->EndType();
    }
    cache->ChecksumData(lbl_806DB604, &mUnidentified024.m_eCharacterClass, context);
    cache->WriteData(lbl_806DB604, &mUnidentified024.m_eCharacterClass,
        offsetof(cCharacter, m_pAnimInventory) - offsetof(cCharacter, mUnidentified024.m_eCharacterClass));

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
        lbl_806DB602 = cache->BeginType("CharAnim");
        REGISTER_CHARACTER_FIELD(17, state, state.m_fFrame, "m_fFrame");
        REGISTER_CHARACTER_FIELD(17, state, state.m_fTotalFrames, "m_fTotalFrames");
        REGISTER_CHARACTER_FIELD(17, state, state.m_fPlaybackSpeedScale, "m_fPlaybackSpeedScale");
        REGISTER_CHARACTER_FIELD(17, state, state.m_fTime, "m_fTime");
        REGISTER_CHARACTER_FIELD(9, state, state.m_nHashID, "m_nHashID");
        REGISTER_CHARACTER_FIELD(2, state, state.m_nHS, "m_nHS");
        REGISTER_CHARACTER_FIELD(17, state, state.m_fDuration, "m_fDuration");
        REGISTER_CHARACTER_FIELD(9, state, state.m_nNumRootKeys, "m_nNumRootKeys");
        REGISTER_CHARACTER_FIELD(17, state, state.m_fLinearSpeed, "m_fLinearSpeed");
        cache->EndType();
    }
    cache->ChecksumData(lbl_806DB602, &state, context);
    cache->WriteData(lbl_806DB602, &state, sizeof(state));

    cHeadTrack* headTrack = m_pHeadTrack;
    if (lbl_806DBD68 == 0xFFFF)
    {
        lbl_806DBD68 = cache->BeginType("HeadTrack");
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
        cache->EndType();
    }
    cache->ChecksumData(lbl_806DBD68, headTrack, context);
    cache->WriteData(lbl_806DBD68, headTrack, sizeof(cHeadTrack));
}

#undef REGISTER_CHARACTER_FIELD

void cCharacter::Unknown12(RunningChecksum* pChecksum)
{
    pChecksum->ChecksumData(&mUnidentified024.m_eCharacterClass, sizeof(mUnidentified024.m_eCharacterClass));
    pChecksum->ChecksumData(&mUnidentified024.m_eMovementState, sizeof(mUnidentified024.m_eMovementState));
    pChecksum->ChecksumData(&mUnidentified024.m_bOnScreen, sizeof(mUnidentified024.m_bOnScreen));
    pChecksum->ChecksumData(&mUnidentified024.m_v3Position, sizeof(mUnidentified024.m_v3Position));
    pChecksum->ChecksumData(&mUnidentified024.m_v3Velocity, sizeof(mUnidentified024.m_v3Velocity));
    pChecksum->ChecksumData(&mUnidentified024.m_aDesiredFacingDirection, sizeof(mUnidentified024.m_aDesiredFacingDirection));
    pChecksum->ChecksumData(&mUnidentified024.m_aActualFacingDirection, sizeof(mUnidentified024.m_aActualFacingDirection));
    pChecksum->ChecksumData(&mUnidentified024.m_aDesiredMovementDirection, sizeof(mUnidentified024.m_aDesiredMovementDirection));
    pChecksum->ChecksumData(&mUnidentified024.m_aActualMovementDirection, sizeof(mUnidentified024.m_aActualMovementDirection));
    pChecksum->ChecksumData(&mUnidentified024.m_fAccel, sizeof(mUnidentified024.m_fAccel));
    pChecksum->ChecksumData(&mUnidentified024.m_fDecel, sizeof(mUnidentified024.m_fDecel));
    pChecksum->ChecksumData(&mUnidentified024.m_fDesiredSpeed, sizeof(mUnidentified024.m_fDesiredSpeed));
    pChecksum->ChecksumData(&mUnidentified024.m_fActualSpeed, sizeof(mUnidentified024.m_fActualSpeed));
    pChecksum->ChecksumData(&mUnidentified024.m_nAnimTurnAdjust, sizeof(mUnidentified024.m_nAnimTurnAdjust));
}

static inline Blinker* MakeBlinker(eCharacterClass cc)
{
    const char* szBaseName = GetCharacterInfo(cc).mName;
    char eyesName0[64];
    char eyesName1[64];
    char eyesName2[64];
    nlSNPrintf(eyesName0, 64, "%s/%s_eye0", szBaseName, szBaseName);
    nlSNPrintf(eyesName1, 64, "%s/%s_eye1", szBaseName, szBaseName);
    nlSNPrintf(eyesName2, 64, "%s/%s_eye2", szBaseName, szBaseName);
    unsigned long texture0 = glGetTexture(eyesName0);
    unsigned long texture1 = glGetTexture(eyesName1);
    unsigned long texture2 = glGetTexture(eyesName2);
    if (glTextureLoad(texture0) && glTextureLoad(texture1)
        && glTextureLoad(texture2))
    {
        return new (8, false) Blinker(texture0, texture1, texture2);
    }
    return 0;
}

cCharacter::cCharacter(eCharacterClass cc, const int* nModelID,
    cSHierarchy* pHierarchy, cAnimInventory* pAnimInventory,
    const CharacterPhysicsData* pPhysicsData, float fPhysicsCapsuleHeight,
    float fPhysicsCapsuleWidth, AnimRetargetList* pAnimRetargetList,
    int nIndex, eClassTypes eNewClassType)
    : m_pPhysicsData(pPhysicsData)
    , m_ModelType(0)
    , m_pPhysicsCharacter(0)
    , m_pAnimInventory(pAnimInventory)
    , m_pPoseAccumulator(0)
    , m_pPoseTree(0)
    , m_pAILayer(0)
    , m_pCurrentAnimController(0)
    , m_eAnimID(0)
    , m_pAnimRetargetList(pAnimRetargetList)
    , m_szEffectsName(0)
    , m_eClassType(eNewClassType)
    , m_bIsUsingElectrocutionTexture(false)
    , mUnidentified0F8(0)
    , mUnidentified0FC(0)
    , mUnidentified100(0)
    , mUnidentified104(0)
    , mUnidentified108(0)
    , mUnidentified118(false)
    , mUnidentified120(nIndex)
    , m_Dirt(0.0f)
    , m_MinDirt(0.0f)
    , m_pBlurHandler(0)
    , m_pBlinker(0)
    , mUnidentified178(1.0f)
    , mUnidentified17C(true)
    , mUnidentified17D(false)
    , mUnidentified17E(false)
    , mUnidentified17F(false)
    , mUnidentified180(false)
    , mUnidentified181(false)
    , mUnidentified182(false)
    , mUnidentified1A4(0.0f)
    , mUnidentified1A8(0.0f)
    , mUnidentified1AC(0.0f)
    , mUnidentified1C0(16, 16)
{
    cSHierarchy* hierarchy0;
    mUnidentified024.m_eCharacterClass = cc;
    mUnidentified11C = &GetCharacterInfo(cc);
    if (pPhysicsData != 0)
    {
        if (eNewClassType == GOALIE)
        {
            PhysicsGoalie* goalie = new (8, false)
                PhysicsGoalie(fPhysicsCapsuleWidth, fPhysicsCapsuleHeight);
            m_pPhysicsCharacter = goalie;
        }
        else
        {
            m_pPhysicsCharacter = new (8, false)
                PhysicsCharacter(fPhysicsCapsuleWidth, fPhysicsCapsuleHeight);
        }
        m_pPhysicsCharacter->m_pAICharacter = this;
    }
    m_m4WorldMatrix.SetIdentity();
    SetPosition(v3Zero);
    mUnidentified024.m_v3Velocity = v3Zero;
    m_pPhysicsCharacter->SetCharacterVelocityXY(mUnidentified024.m_v3Velocity);

    for (int i = 0; i < 4; ++i)
    {
        if (nModelID[i] != 0)
        {
            GLInventory& glInventory = *glGetCurrentResourcePool()->m_inventory;
            m_pSkinMesh[i] = glInventory.MakeSkinMesh(
                nModelID[i], pHierarchy);
        }
        else
        {
            m_pSkinMesh[i] = 0;
        }
    }

    m_pPoseAccumulator = new (8, false) cPoseAccumulator(pHierarchy, true);
    if (pPhysicsData != 0)
    {
        m_pPhysicsCharacter->AddBoneVolumes(g_PhysicsWorld,
            g_CollisionSpace, m_pPoseAccumulator, m_pPhysicsData, 0x80, 0x20);
        m_szEffectsName = 0;
    }
    m_pHeadTrack = new (8, false) cHeadTrack();
    hierarchy0 = m_pPoseAccumulator->m_BaseSHierarchy;
    m_nHeadJointIndex = hierarchy0->GetNodeIndexByID(nlStringLowerHash("bip01 head"));
    hierarchy0 = m_pPoseAccumulator->m_BaseSHierarchy;
    m_nBip01JointIndex_0xA4 = hierarchy0->GetNodeIndexByID(nlStringLowerHash("bip01"));
    hierarchy0 = m_pPoseAccumulator->m_BaseSHierarchy;
    m_nSpine1JointIndex = hierarchy0->GetNodeIndexByID(nlStringLowerHash("bip01 spine1"));
    hierarchy0 = m_pPoseAccumulator->m_BaseSHierarchy;
    mUnidentified0E4 = hierarchy0->GetNodeIndexByID(
        nlStringLowerHash("bip01 l foot"));
    hierarchy0 = m_pPoseAccumulator->m_BaseSHierarchy;
    mUnidentified0E8 = hierarchy0->GetNodeIndexByID(
        nlStringLowerHash("bip01 r foot"));
    mUnidentified0F8 = new (8, false) UnidentifiedCharacterObject_8001C158;
    m_pEffectsTexturing = 0;
    m_pBlinker = MakeBlinker(mUnidentified024.m_eCharacterClass);
    nlVec3Set(m_v3ScreenPosition, 0.0f, 0.0f, 0.0f);
    mUnidentified194 = v3Zero;
    mUnidentified184.z = 0.0f;
    mUnidentified184.y = 0.0f;
    mUnidentified184.x = 0.0f;
    mUnidentified184.w = 1.0f;
    mUnidentified1A0 = 0.0f;
    for (int i = 0; i < 4; ++i)
    {
        unknown_0x018[i] = false;
    }
}

extern "C" void fn_80022968(CollisionChainPlayerData* pEventData)
{
    if (pEventData->pFielder != NULL && pEventData->pChain != NULL)
    {
        if (!pEventData->pFielder->UnidentifiedInvinciblePowerups())
        {
            pEventData->pFielder->CollideWithChainCallback(pEventData->pChain);
        }
    }
}
extern "C" void fn_800229F0(CollisionWindDebrisPlayerData* pEventData)
{
    if (pEventData->pFielder != NULL && pEventData->pDebris != NULL)
    {
        if (!pEventData->pFielder->UnidentifiedInvinciblePowerups())
        {
            pEventData->pFielder->fn_8003295C(pEventData->pDebris);
        }
    }
}
extern "C" void fn_80022A78(CollisionThwompPlayerData* pEventData)
{
    if (pEventData->target->m_eClassType == FIELDER)
    {
        ((cFielder*)pEventData->target)->fn_80032CB8(pEventData);
    }
}
extern "C" void fn_80020B8C(cFielder* pFielder)
{
    pFielder->fn_80047240(pFielder,
        (unsigned short)(pFielder->mUnidentified024.m_aActualFacingDirection + 0x8000),
        0, false, false);
}
extern "C" void GoalieOnGameOver();
extern "C" void fn_8002276C()
{
    for (int i = 0; i < 2; i++)
    {
        if (g_pTeams[i] != NULL)
        {
            g_pTeams[i]->StopGameplayEffectsAndSounds();
        }
    }
    GoalieOnGameOver();
}
extern "C" void fn_800227C8()
{
    if (g_pGame != NULL)
    {
        g_pGame->ChangeGameState(5);
        PlaySound(10, 0xA21ADED3UL, NULL, NULL);
    }
}
extern "C" void fn_80022810(UnidentifiedEventData_8006701C*)
{
    if (g_pGame != NULL)
    {
        g_pGame->fn_80058704();
    }
}
extern "C" void fn_80022908()
{
    if (g_pGame != NULL)
    {
        ShootToScoreMeter::instance.fn_801AF97C();
        if (g_pGame->IsGameplayOrOvertime())
        {
            g_pGame->fn_800586C0();
        }
    }
}
extern "C" void fn_8002E5F4(cFielder*, int);
extern "C" void fn_80022824(cPlayer*)
{
    if (g_pGame != NULL)
    {
        for (int i = 0; i < 2; i++)
        {
            cTeam* pTeam = g_pTeams[i];
            for (int j = 0; j < 4; j++)
            {
                cFielder* pFielder = pTeam->GetFielder(j);
                if (pFielder->fn_8003EA6C())
                {
                    fn_8002E5F4(pFielder, 0);
                }
                else if (pFielder->mUnidentified024.m_eCharacterClass == 13
                    && pFielder->m_eActionState == ACTION_UNKNOWN_32)
                {
                    pFielder->EndDesire();
                    pFielder->EndAction();
                }
                else if (pFielder->mUnidentified024.m_eCharacterClass == 19
                    && pFielder->m_eActionState == ACTION_UNKNOWN_32)
                {
                    pFielder->EndDesire();
                    pFielder->EndAction();
                }
            }
        }
        gNPCManager->fn_801AA348();
    }
}
extern "C" void fn_80022664(CollisionPlayerPlayerData* pEventData)
{
    if (pEventData->player1 != NULL)
    {
        pEventData->player1->CollideWithCharacterCallback(pEventData);
    }
}
extern "C" void fn_8002268C(CollisionPlayerWallData* pEventData)
{
    if (pEventData->pPlayer != NULL)
    {
        pEventData->pPlayer->CollideWithWallCallback(pEventData);
    }
}
extern "C" void fn_800226B4(CollisionPlayerBallData* pEventData)
{
    if (pEventData->pPlayer != NULL && pEventData->pBall != NULL)
    {
        pEventData->pPlayer->CollideWithBallCallback(pEventData->pBall);
        pEventData->pBall->CollideWithCharacterCallback(pEventData->pPlayer, pEventData->velocity);

        PhysicsAIBall* pPhysicsBall = pEventData->pBall->m_pPhysicsBall;
        if (pPhysicsBall->mbUseMagnusEffect)
        {
            nlVector3 v3AngVel;
            pPhysicsBall->GetAngularVelocity(&v3AngVel);
            nlVec3Scale(v3AngVel, 0.6f);
            pPhysicsBall->SetAngularVelocity(v3AngVel);
        }
    }
}
extern "C" void fn_80015B38(cBall*, bool);
extern "C" void fn_80022614(UnidentifiedEventData04*)
{
    if (g_pBall->m_tLightningTimer.m_uPackedTime != 0)
    {
        PlaySound(10, 0xE07B30E9UL, NULL, NULL);
    }
    fn_80015B38(g_pBall, false);
}
extern "C" void fn_80014494(cBall*);
extern "C" void fn_80022594(CollisionBallGroundData* pEventData)
{
    if (pEventData->pBall != NULL
        && (pEventData->pBall->meBallState == 5 || pEventData->pBall->meBallState == 4 || pEventData->bIsShot))
    {
        fn_80014494(pEventData->pBall);
    }
    PlaySound(11, 0x94AC3A20UL, NULL, NULL);
    SetLastSoundParameter(6, pEventData->fVecZComponent);
}
extern "C" void fn_800145A4(cBall*);
extern "C" void fn_800224DC(CollisionBallWallData* pEventData)
{
    if (pEventData->pBall != NULL)
    {
        fn_800145A4(pEventData->pBall);
        if (GameInfoManager::Instance()->GetStadium() != 0xB
            || fabsf(pEventData->pBall->m_v3Position.x) > cField::GetGoalLineX(1U) - 2.5f)
        {
            if (EmitElectricFenceBallEffect(pEventData->position, pEventData->normal, (unsigned long)pEventData->pBall, false))
            {
                PlaySound(10, 0x2AA0C2A9UL, NULL, NULL);
            }
        }
    }
}
extern "C" void fn_80021B68(CollisionBallShellData* pEventData)
{
    fn_80015B38(pEventData->pBall, false);
    pEventData->pPowerup->m_pTarget = NULL;
}
extern "C" void fn_80021BA8(CollisionBallChainData* pEventData)
{
    fn_80015B38(pEventData->pBall, false);
}
extern "C" void fn_80021BB4(CollisionBallGoalpostData* pEventData)
{
    if (g_pBall != NULL)
    {
        EmissionManager* unidentifiedManager = EmissionManager::Instance();
        EffectsGroup* pGroup = unidentifiedManager->GetEffectsGroup("ball_impact");
        EmissionController* pControl = unidentifiedManager->Create(pGroup, 0, true, 0);
        pControl->SetPosition(pEventData->v3CollisionPosition);
        fn_80015B38(g_pBall, false);
        if (g_pBall->m_pPrevOwner != NULL
            && GetStadiumUnknown0x10(GameInfoManager::Instance()->GetStadium()))
        {
            PlayCrowdReaction(g_pBall->m_pPrevOwner->m_pTeam->m_nSide == HOME
                    ? 0xEF359B65UL : 0xC6A6B1CEUL);
        }
        PlaySound(10, 0xEFE291A8UL, NULL, NULL);
    }
}
extern "C" bool fn_8002F1E0(cFielder*);
extern "C" void fn_80021E30(CollisionKoopaShotBallPlayerData* pEventData)
{
    if (!fn_8002F1E0(pEventData->player))
    {
        return;
    }
    if (pEventData->player->fn_8003E73C())
    {
        pEventData->player->fn_8004D480(v3Zero);
        return;
    }

    cBall* pBall = g_pBall;
    cFielder* pFielder = pEventData->player;
    nlVector4 plane;
    nlVector3 v3Velocity;
    if (nlGetLengthSquared2D(pBall->m_v3Velocity.x, pBall->m_v3Velocity.y) < 0.01f)
    {
        MakePerpendicularPlane(pBall->m_v3Position,
            (unsigned short)(pEventData->shell->mOwner->mUnidentified024.m_aActualFacingDirection + 0x4000), plane, 0.0f);
        nlVec3Scale(v3Velocity, *(const nlVector3*)&plane, 40.0f);
    }
    else
    {
        nlVec3Set(v3Velocity, pBall->m_v3Velocity.y, -pBall->m_v3Velocity.x, 0.0f);
        MakePerpendicularPlane(pBall->m_v3Position, v3Velocity, plane, 0.0f);
    }
    if (nlPlaneSide(pFielder->mUnidentified024.m_v3Position, plane) < 0.0f)
    {
        v3Velocity.x = -v3Velocity.x;
        v3Velocity.y = -v3Velocity.y;
    }
    v3Velocity.x += pBall->m_v3Velocity.x;
    v3Velocity.y += pBall->m_v3Velocity.y;
    unsigned short aDirection = (unsigned short)(s32)(nlATan2f(v3Velocity.y, v3Velocity.x) * 10430.378f);
    nlVector3 v3Position;
    nlVec3ScaleAdd(v3Position, 0.015f, v3Velocity, pFielder->mUnidentified024.m_v3Position);
    pEventData->player->SetPosition(v3Position);
    if (pEventData->player->fn_80047240(pEventData->shell->mOwner, aDirection, 2, false, false))
    {
        pEventData->player->PlayAttackReactionSounds(gGameTweaks.m_pGameTweaks->fShootToScoreBallHitReactionVolume.GetValue());
    }
}
extern "C" void fn_800156F8(cBall*, cPlayer*);
extern "C" void fn_80022050(CollisionBirdoShotBallPlayerData* pEventData)
{
    if (fn_8002F1E0(pEventData->player))
    {
        if (pEventData->player->fn_8003E73C())
        {
            pEventData->player->fn_8004D480(v3Zero);
        }
        else
        {
            cBall* pBall = g_pBall;
            cFielder* pFielder = pEventData->player;
            nlVector4 plane;
            nlVector3 v3Velocity;
            if (nlGetLengthSquared2D(pBall->m_v3Velocity.x, pBall->m_v3Velocity.y) < 0.01f)
            {
                MakePerpendicularPlane(pBall->m_v3Position,
                    (unsigned short)(pEventData->egg->mShooter->mUnidentified024.m_aActualFacingDirection + 0x4000), plane, 0.0f);
                nlVec3Scale(v3Velocity, *(const nlVector3*)&plane, 40.0f);
            }
            else
            {
                nlVec3Set(v3Velocity, pBall->m_v3Velocity.y, -pBall->m_v3Velocity.x, 0.0f);
                MakePerpendicularPlane(pBall->m_v3Position, v3Velocity, plane, 0.0f);
            }
            if (nlPlaneSide(pFielder->mUnidentified024.m_v3Position, plane) < 0.0f)
            {
                v3Velocity.x = -v3Velocity.x;
                v3Velocity.y = -v3Velocity.y;
            }
            v3Velocity.x += pBall->m_v3Velocity.x;
            v3Velocity.y += pBall->m_v3Velocity.y;
            unsigned short aDirection = (unsigned short)(s32)(nlATan2f(v3Velocity.y, v3Velocity.x) * 10430.378f);
            nlVector3 v3Position;
            nlVec3ScaleAdd(v3Position, 0.015f, v3Velocity, pFielder->mUnidentified024.m_v3Position);
            pEventData->player->SetPosition(v3Position);
            if (pEventData->player->fn_80047240(pEventData->egg->mShooter, aDirection, 2, false, false))
            {
                pEventData->player->PlayAttackReactionSounds(gGameTweaks.m_pGameTweaks->fShootToScoreBallHitReactionVolume.GetValue());
            }
        }
    }
    fn_800156F8(g_pBall, pEventData->egg->mShooter);
}
extern "C" void fn_80021D70(CollisionKoopaShellGoalieData* pEventData)
{
    ((Goalie*)pEventData->goalie)->fn_80090958(pEventData->shell->mOwner != NULL);
    pEventData->shell->Deactivate(false);
    fn_80015B38(g_pBall, false);
}
extern "C" void fn_80021DCC(CollisionBirdoEggGoalieData* pEventData)
{
    ((Goalie*)pEventData->goalie)->fn_80090958(pEventData->egg->mShooter != NULL);
    PlaySound(pEventData->egg->mShooter->mUnidentified318, 0x16BA5AE9UL, NULL, NULL);
}
extern "C" bool fn_8002F1E0(cFielder*);
extern "C" void fn_80022280(UnidentifiedEventData16* pEventData)
{
    if (fn_8002F1E0(pEventData->pFielder))
    {
        cBall* pBall = pEventData->pBall;
        if (pBall->m_pPrevOwner != NULL)
        {
            cFielder* pFielder = pEventData->pFielder;
            nlVector4 plane;
            nlVector3 v3Velocity;
            if (nlGetLengthSquared2D(pBall->m_v3Velocity.x, pBall->m_v3Velocity.y) < 0.01f)
            {
                MakePerpendicularPlane(g_pBall->m_v3Position,
                    (unsigned short)(g_pBall->m_pShooter->mUnidentified024.m_aActualFacingDirection + 0x4000), plane, 0.0f);
                nlVec3Scale(v3Velocity, *(const nlVector3*)&plane, 40.0f);
            }
            else
            {
                nlVec3Set(v3Velocity, pBall->m_v3Velocity.y, -pBall->m_v3Velocity.x, 0.0f);
                MakePerpendicularPlane(pEventData->pBall->m_v3Position, v3Velocity, plane, 0.0f);
            }
            if (nlPlaneSide(pFielder->mUnidentified024.m_v3Position, plane) < 0.0f)
            {
                v3Velocity.x = -v3Velocity.x;
                v3Velocity.y = -v3Velocity.y;
            }
            v3Velocity.x += pBall->m_v3Velocity.x;
            v3Velocity.y += pBall->m_v3Velocity.y;
            unsigned short aDirection = (unsigned short)(s32)(nlATan2f(v3Velocity.y, v3Velocity.x) * 10430.378f);
            nlVector3 v3Position;
            nlVec3ScaleAdd(v3Position, 0.015f, v3Velocity, pFielder->mUnidentified024.m_v3Position);
            pEventData->pFielder->SetPosition(v3Position);
            if (pEventData->pFielder->fn_80047240(pEventData->pBall->m_pPrevOwner, aDirection, 2, false, false))
            {
                pEventData->pFielder->PlayAttackReactionSounds(gGameTweaks.m_pGameTweaks->fShootToScoreBallHitReactionVolume.GetValue());
            }

            pEventData->pFielder->SetNoPickUpTime(0.06f);
            Goalie* pGoalie = pEventData->pBall->m_pShooter->m_pTeam->GetOtherTeam()->GetGoalie();
            pGoalie->mpSkillShooter = NULL;
            fn_80015C38(pEventData->pBall, 6);
        }
    }
    if (pEventData->pBall != NULL && pEventData->pFielder != NULL)
    {
        pEventData->pBall->m_pLastTouch = pEventData->pFielder;
    }
}
extern "C" void fn_80021C98(CollisionPowerupWallData* pEventData)
{
    const nlVector3& pos = pEventData->position;
    const nlVector3& nrm = pEventData->normal;
    unsigned long powerupID = (unsigned long)pEventData->pPowerup;
    if (GameInfoManager::Instance()->GetStadium() != 0xB
        || fabsf(pos.x) > cField::GetGoalLineX(1U) - 2.5f)
    {
        EmissionManager* unidentifiedManager = EmissionManager::Instance();
        if (!unidentifiedManager->IsPlaying(powerupID, unidentifiedManager->GetEffectsGroup("electric_fence")))
        {
            EmitElectricFenceBallEffect(pos, nrm, powerupID, false);
            PowerupBase::PlayPowerupSound(pEventData->eType, PowerupBase::PWRUP_SOUND_BOUNCE_WALL, pos, 0.0f, NULL);
        }
    }
}
extern "C" void fn_80021924(CollisionPlayerBananaData* pEventData)
{
    if (pEventData->pPlayer != NULL)
    {
        bool bIsWeaponSuccessful = pEventData->pPlayer->CollideWithBananaCallback(pEventData->v3CollisionLocation);
        if (pEventData->pThrower != NULL && bIsWeaponSuccessful
            && !pEventData->pThrower->IsOnSameTeam(pEventData->pPlayer))
        {
            CollisionPowerupStatsData* pStatsData = NULL;
            g_CollisionPowerupStatsDataPool.Allocate(pStatsData);
            pStatsData->mUnidentified08 = pEventData->pThrower;
            pStatsData->mUnidentified0C = pEventData->nThrowerPadID;
            cPlayer* pPlayer = pEventData->pPlayer;
            if (pPlayer->m_eClassType == FIELDER)
            {
                pStatsData->mUnidentified00 = pPlayer;
                bool bHasPad = pPlayer->GetGlobalPad() != NULL;
                pStatsData->mUnidentified04 = bHasPad ? pPlayer->GetGlobalPad()->GetPadID() : -1;
            }
            else
            {
                pStatsData->mUnidentified00 = NULL;
                pStatsData->mUnidentified04 = -1;
            }
            g_pGame->mUnidentified49C.mEvent31.Queue(pStatsData,
                Function<CollisionPowerupStatsData*>(fn_80025A14));
        }
    }
}
extern "C" void fn_800216C4(CollisionPlayerShellData* pEventData)
{
    if (pEventData->pPlayer != NULL)
    {
        bool bIsWeaponSuccessful = pEventData->pPlayer->CollideWithShellCallback(
            (ePowerupSize)pEventData->eSize, (bool)pEventData->bIsExploder,
            pEventData->v3CollisionLocation, pEventData->v3CollisionVelocity);
        if (pEventData->pThrower != NULL && bIsWeaponSuccessful
            && !pEventData->pThrower->IsOnSameTeam(pEventData->pPlayer))
        {
            CollisionPowerupStatsData* pStatsData = NULL;
            g_CollisionPowerupStatsDataPool.Allocate(pStatsData);
            pStatsData->mUnidentified08 = pEventData->pThrower;
            pStatsData->mUnidentified0C = (s32)(s8)pEventData->nThrowerPadID;
            cPlayer* pPlayer = pEventData->pPlayer;
            if (pPlayer->m_eClassType == FIELDER)
            {
                pStatsData->mUnidentified00 = pPlayer;
                bool bHasPad = pPlayer->GetGlobalPad() != NULL;
                pStatsData->mUnidentified04 = bHasPad ? pPlayer->GetGlobalPad()->GetPadID() : -1;
            }
            else
            {
                pStatsData->mUnidentified00 = NULL;
                pStatsData->mUnidentified04 = -1;
            }
            g_pGame->mUnidentified49C.mEvent31.Queue(pStatsData,
                Function<CollisionPowerupStatsData*>(fn_80025A14));
        }
    }
}
extern "C" void fn_80025A14(CollisionPowerupStatsData* data)
{
    g_CollisionPowerupStatsDataPool.Free(data);
}

extern "C" void fn_80021484(CollisionPlayerFreezeData* pEventData)
{
    if (pEventData->pPlayer != NULL)
    {
        bool bIsWeaponSuccessful = pEventData->pPlayer->CollideWithFreezeCallback();
        if (pEventData->pThrower != NULL && bIsWeaponSuccessful
            && !pEventData->pThrower->IsOnSameTeam(pEventData->pPlayer))
        {
            CollisionPowerupStatsData* pStatsData = NULL;
            g_CollisionPowerupStatsDataPool.Allocate(pStatsData);
            pStatsData->mUnidentified08 = pEventData->pThrower;
            pStatsData->mUnidentified0C = pEventData->nThrowerPadID;
            cPlayer* pPlayer = pEventData->pPlayer;
            if (pPlayer->m_eClassType == FIELDER)
            {
                pStatsData->mUnidentified00 = pPlayer;
                bool bHasPad = pPlayer->GetGlobalPad() != NULL;
                pStatsData->mUnidentified04 = bHasPad ? pPlayer->GetGlobalPad()->GetPadID() : -1;
            }
            else
            {
                pStatsData->mUnidentified00 = NULL;
                pStatsData->mUnidentified04 = -1;
            }
            g_pGame->mUnidentified49C.mEvent31.Queue(pStatsData,
                Function<CollisionPowerupStatsData*>(fn_80025A14));
        }
    }
}
extern "C" void fn_8002147C(UnidentifiedEventData24* pEventData)
{
    ((cFielder*)pEventData->mUnidentified0C)->EndAction();
}
float lbl_806DB5F0 = 0.8f;

extern "C" void fn_800212A0(CharacterImpactEvent* pEventData)
{
    if (g_pGame != NULL && g_pGame->IsGameplayOrOvertime())
    {
        for (int i = 0; i < 2; i++)
        {
            cTeam* pTeam = g_pTeams[i];
            if (pTeam != NULL)
            {
                for (int j = 0; j < 4; j++)
                {
                    cFielder* pFielder = pTeam->GetFielder(j);
                    if (!pFielder->IsInvincible() && pFielder->mbTangible
                        && !pFielder->fn_8003E74C() && pEventData->pCharacter != pFielder)
                    {
                        nlVector3 v3Delta;
                        nlVec3Set(v3Delta, pEventData->v3Position.x - pFielder->mUnidentified024.m_v3Position.x,
                            pEventData->v3Position.y - pFielder->mUnidentified024.m_v3Position.y,
                            pEventData->v3Position.z - pFielder->mUnidentified024.m_v3Position.z);
                        float fDistance = nlVec3Length(v3Delta);
                        if (fDistance < pEventData->fMagnitude)
                        {
                            if (fDistance >= pEventData->fMagnitude * lbl_806DB5F0)
                            {
                                pFielder->fn_800470B4(pFielder, (cPlayer*)pEventData->pCharacter);
                            }
                            else
                            {
                                pFielder->CollideWithBobombCallback(pEventData->v3Position, pEventData->fMagnitude);
                            }
                        }
                    }
                }
                Goalie* pGoalie = pTeam->GetGoalie();
                nlVector3 v3Delta;
                nlVec3Set(v3Delta, pEventData->v3Position.x - pGoalie->mUnidentified024.m_v3Position.x,
                            pEventData->v3Position.y - pGoalie->mUnidentified024.m_v3Position.y,
                            pEventData->v3Position.z - pGoalie->mUnidentified024.m_v3Position.z);
                if (nlVec3LengthSquared(v3Delta) < pEventData->fMagnitude * pEventData->fMagnitude)
                {
                    pGoalie->fn_8008EC2C();
                }
            }
        }
    }
}
float lbl_806DB5EC = 0.5f;

extern "C" void fn_80021120(CharacterImpactEvent* pEventData)
{
    if (g_pGame != NULL && g_pGame->IsGameplayOrOvertime())
    {
        for (int i = 0; i < 2; i++)
        {
            if (g_pTeams[i] != NULL)
            {
                cTeam* pTeam = g_pTeams[i];
                for (int j = 0; j < 4; j++)
                {
                    cFielder* pFielder = pTeam->GetFielder(j);
                    if (!pFielder->IsInvincible() && pFielder->mbTangible
                        && !pFielder->fn_8003E74C() && pEventData->pCharacter != pFielder)
                    {
                        nlVector3 v3Delta;
                        nlVec3Set(v3Delta, pEventData->v3Position.x - pFielder->mUnidentified024.m_v3Position.x,
                            pEventData->v3Position.y - pFielder->mUnidentified024.m_v3Position.y,
                            pEventData->v3Position.z - pFielder->mUnidentified024.m_v3Position.z);
                        float fDistance = nlVec3Length(v3Delta);
                        if (fDistance < pEventData->fMagnitude)
                        {
                            if (fDistance >= pEventData->fMagnitude * lbl_806DB5EC)
                            {
                                pFielder->fn_800470B4(pFielder, (cPlayer*)pEventData->pCharacter);
                            }
                            else
                            {
                                pFielder->fn_8004D480(pFielder->mUnidentified024.m_v3Velocity);
                            }
                        }
                    }
                }
            }
        }
    }
}
extern "C" void fn_80020E20(ReceiveBallData* pEventData)
{
    cPlayer* pReceiver = pEventData->pReceiver;
    if (pReceiver == NULL)
        return;
    if (pReceiver->IsOnSameTeam(g_pBall->m_pPrevOwner))
        return;

    cTeam* pTeam = pReceiver->GetTeam();
    pTeam->mtMarkTimer.UnidentifiedClear();
    pTeam->mtRoleTimer.UnidentifiedClear();
    cTeam* pOtherTeam = pTeam->GetOtherTeam();
    pOtherTeam->mtMarkTimer.UnidentifiedClear();
    pOtherTeam = pTeam->GetOtherTeam();
    pOtherTeam->mtRoleTimer.UnidentifiedClear();
}
extern "C" void fn_80020E1C(UnidentifiedEventData_80066008*)
{
}
bool lbl_806E0C20;
bool lbl_806E0C21;

extern "C" void fn_80020CDC(GoalScoredData* pEventData)
{
    if (pEventData != NULL && pEventData->uGoalType != 6)
    {
        PlaySound(11, 0x8CEE6665UL, NULL, NULL);
    }
    if (!GetTweakBool("/user/no_presentation", false)
        && g_pTeams[0] != NULL && g_pTeams[1] != NULL)
    {
        for (s32 i = 0; i < 2; i++)
        {
            cTeam* pTeam = g_pTeams[i];
            if (lbl_806E0C20)
            {
                pTeam->ClearAllPowerUps();
            }
            else if (lbl_806E0C21
                && pEventData->uTeamIndex == pTeam->m_nSide)
            {
                pTeam->ClearAllPowerUps();
            }
            for (s32 j = 0; j < 5; j++)
            {
                cPlayer* pPlayer = pTeam->GetPlayer(j);
                bool bUnidentified = pPlayer->GetGlobalPad() != NULL;
                if (bUnidentified)
                {
                    pPlayer->SetAIPad(NULL);
                }
            }
        }
    }
}

extern "C" void fn_80020E04(MegaStrikeEndData* pEventData)
{
    if (pEventData->goals > 0)
    {
        fn_80020CDC(NULL);
    }
}
extern "C" void fn_80021050(LightningStrikeData* pEventData)
{
    if (g_pGame != NULL && g_pGame->IsGameplayOrOvertime())
    {
        if (nlSqrt(nlVec3DistanceSquared2D(g_pBall->m_v3Position,
                pEventData->position), true) < pEventData->radius
            && g_pBall->m_pOwner == NULL
            && g_pBall->meBallState != 10 && g_pBall->meBallState != 9)
        {
            fn_80015C38(g_pBall, 9);
        }
        CreateLightningShockwave(&pEventData->position, pEventData->radius);
    }
}
extern "C" void fn_80020EE8(CollisionBulletBillData* pEventData)
{
    if (pEventData->bulletBill->active)
    {
        cFielder* pUnidentified0 = (cFielder*)pEventData->player;
        cFielder* pUnidentified1 = pEventData->bulletBill->target;
        if (pUnidentified1 != pUnidentified0)
        {
            if (pUnidentified0->fn_8003E74C() || pUnidentified0->fn_8003E6FC())
            {
                CollisionBulletBillData data = { pEventData->player, pEventData->bulletBill };
                g_pGame->fn_80060BFC(data);
            }
            else if (pUnidentified0->fn_800470B4(pUnidentified0, pUnidentified1))
            {
                PlayOwnedSound(pUnidentified0->mUnidentified318, 0xFD0DC03DUL,
                    (XSoundOwner*)g_pBall->mUnidentifiedEC, NULL, NULL);
            }
        }
    }
}

extern "C" void fn_80020FB8(CollisionBulletBillData* pEventData)
{
    if (pEventData->bulletBill->active)
    {
        pEventData->bulletBill->target->CollideWithFreezeCallback();
    }
}

extern "C" void fn_80020FD4(CollisionBulletBillData* pEventData)
{
    if (g_pGame != NULL && pEventData->bulletBill->active)
    {
        CreateBulletBillShockwave(pEventData->bulletBill);
        pEventData->bulletBill->Hide(false);
        PlayOwnedSound(pEventData->bulletBill->target->mUnidentified318,
            0xFD0DC03DUL, (XSoundOwner*)g_pBall->mUnidentifiedEC, NULL, NULL);
    }
}
extern "C" void fn_80020C70(UnidentifiedEventData_80066A04* pEventData)
{
    if (GetStadiumUnknown0x10(GameInfoManager::Instance()->GetStadium())
        && pEventData->mUnidentified08 != NULL)
    {
        PlayCrowdReaction(pEventData->mUnidentified08->m_pTeam->m_nSide == HOME
                ? 0x5087D7C9UL : 0xA7E73452UL);
    }
}

extern "C" void fn_80020BB0(PlayerAttackData* pEventData)
{
    if (GetStadiumUnknown0x10(GameInfoManager::Instance()->GetStadium())
        && pEventData->pTarget != NULL && pEventData->pAttacker != NULL
        && !pEventData->mUnidentified10)
    {
        if (pEventData->pAttacker->fn_8001E168()
            && pEventData->pTarget->fn_8001E168())
        {
            PlayCrowdReaction(0x3648CBA4UL);
        }
        else if (pEventData->mUnidentified0C == 2)
        {
            PlayCrowdReaction(pEventData->pAttacker->m_pTeam->m_nSide == HOME
                    ? 0xF2B4508FUL : 0x5F30D098UL);
        }
    }
}
extern "C" void fn_80022B1C(UnidentifiedEventData26* pEventData)
{
    cCharacter* pCharacter = (cCharacter*)pEventData->mUnidentified18;
    if (pCharacter->m_eClassType == FIELDER)
    {
        cFielder* pFielder = (cFielder*)pCharacter;
        if (!pFielder->IsFallenDown())
        {
            pFielder->fn_8004D480(v3Zero);
        }
        PlaySound(pEventData->pFielder->mUnidentified318, 0x52641B7BUL, NULL, NULL);
    }
    else if (pCharacter->m_eClassType == GOALIE)
    {
        Goalie* pGoalie = (Goalie*)pCharacter;
        if (pGoalie->mGoalieActionState != GOALIEACTION_UNIDENTIFIED_32)
        {
            pGoalie->fn_80090320(0.0f);
            PlaySound(pEventData->pFielder->mUnidentified318, 0xFD0DC03DUL, NULL, NULL);
        }
    }
}
extern "C" void fn_80022B04(UnidentifiedEventData24* pEventData)
{
    pEventData->mUnidentified0C->fn_80099074(pEventData);
}
float lbl_806DB5F4 = 1.5f;
extern "C" void fn_80060FF4(cGame*, const CharacterImpactEvent*);
extern "C" void fn_800367B4(cFielder*);
extern "C" void fn_80022A98(UnidentifiedEventData26* pEventData)
{
    CharacterImpactEvent event;
    event.v3Position = pEventData->v3Position;
    event.v3Position.z = 0.0f;
    event.fMagnitude = lbl_806DB5F4;
    event.pCharacter = pEventData->pFielder;
    fn_80060FF4(g_pGame, &event);
    fn_800367B4(pEventData->pFielder);
}
extern "C" void fn_801BAF0C(cPlayer* pCharacter);

extern "C" void fn_80022BD8(UnidentifiedEventData34* pEventData)
{
    cPlayer* pPlayer = pEventData->mUnidentified00;
    if (pPlayer->m_eClassType == FIELDER)
    {
        cFielder* pFielder = (cFielder*)pPlayer;
        if (pFielder->IsInvincible())
        {
            fn_8002E5F4(pEventData->mUnidentified04, 0);
        }
        else if (!pFielder->IsFallenDown())
        {
            if (pFielder->fn_8003E74C())
            {
                pFielder->InitActionShellReact(pEventData->mUnidentified08->mPosition,
                    pEventData->mUnidentified04->mUnidentified024.m_v3Velocity);
            }
            else if (pFielder->IsCharacterInAir(pEventData->mUnidentified08->mPhysics->GetRadius())
                || (pFielder->m_eActionState == 0x1D && pFielder->mUnidentified024.m_eCharacterClass == HAMMERBROS)
                || (pFielder->m_eActionState == 1 && pFielder->mUnidentified024.m_eCharacterClass == PEACH)
                || (pFielder->m_eActionState == 1 && pFielder->mUnidentified024.m_eCharacterClass == WALUIGI)
                || (pFielder->m_eActionState == 1 && pFielder->mUnidentified024.m_eCharacterClass == 13))
            {
                pFielder->InitActionBombReact(pEventData->mUnidentified08->mPosition, 0.0f);
                fn_801BAF0C(pFielder);
            }
            else
            {
                pFielder->fn_8004D480(pEventData->mUnidentified04->mUnidentified024.m_v3Velocity);
            }
        }
    }
}
extern "C" void fn_80098750();

extern "C" void fn_8001FE80()
{
    UnidentifiedFindEvent<CollisionChainPlayerData>("CollisionChainPlayer", -1)->Add(Function<CollisionChainPlayerData*>(fn_80022968), 0, -1);
    UnidentifiedFindEvent<CollisionWindDebrisPlayerData>("CollisionWindDebrisPlayer", -1)->Add(Function<CollisionWindDebrisPlayerData*>(fn_800229F0), 0, -1);
    UnidentifiedFindEvent<CollisionThwompPlayerData>("CollisionThwompPlayer", -1)->Add(Function<CollisionThwompPlayerData*>(fn_80022A78), 0, -1);
    UnidentifiedFindEvent<cFielder>("KnockYoshiTongue", -1)->Add(Function<cFielder*>(fn_80020B8C), 0, -1);
    UnidentifiedFindEvent<UnidentifiedEventNoData>("GameOver", -1)->Add(Function<FnVoidVoid>(fn_8002276C), 0, -1);
    UnidentifiedFindEvent<UnidentifiedEventNoData>("Kickoff", -1)->Add(Function<FnVoidVoid>(fn_800227C8), 0, -1);
    UnidentifiedFindEvent<UnidentifiedEventData_8006701C>("MegaStrikeMeterStart", -1)->Add(Function<UnidentifiedEventData_8006701C*>(fn_80022810), 0, -1);
    UnidentifiedFindEvent<UnidentifiedEventNoData>("MegaStrikeMeterEnd", -1)->Add(Function<FnVoidVoid>(fn_80022908), 0, -1);
    UnidentifiedFindEvent<cPlayer>("MegaStrikeIntro", -1)->Add(Function<cPlayer*>(fn_80022824), 0, -1);
    UnidentifiedFindEvent<CollisionPlayerPlayerData>("CollisionPlayerPlayer", -1)->Add(Function<CollisionPlayerPlayerData*>(fn_80022664), 0, -1);
    UnidentifiedFindEvent<CollisionPlayerWallData>("CollisionPlayerWall", -1)->Add(Function<CollisionPlayerWallData*>(fn_8002268C), 0, -1);
    UnidentifiedFindEvent<CollisionPlayerBallData>("CollisionPlayerBall", -1)->Add(Function<CollisionPlayerBallData*>(fn_800226B4), 0, -1);
    UnidentifiedFindEvent<UnidentifiedEventData04>("CollisionBallNetmesh", -1)->Add(Function<UnidentifiedEventData04*>(fn_80022614), 0, -1);
    UnidentifiedFindEvent<CollisionBallGroundData>("CollisionBallGround", -1)->Add(Function<CollisionBallGroundData*>(fn_80022594), 0, -1);
    UnidentifiedFindEvent<CollisionBallWallData>("CollisionBallWall", -1)->Add(Function<CollisionBallWallData*>(fn_800224DC), 0, -1);
    UnidentifiedFindEvent<CollisionBallShellData>("CollisionBallShell", -1)->Add(Function<CollisionBallShellData*>(fn_80021B68), 0, -1);
    UnidentifiedFindEvent<CollisionBallChainData>("CollisionBallChain", -1)->Add(Function<CollisionBallChainData*>(fn_80021BA8), 0, -1);
    UnidentifiedFindEvent<CollisionBallGoalpostData>("CollisionBallGoalpost", -1)->Add(Function<CollisionBallGoalpostData*>(fn_80021BB4), 0, -1);
    UnidentifiedFindEvent<CollisionKoopaShotBallPlayerData>("CollisionKoopaShotBallPlayer", -1)->Add(Function<CollisionKoopaShotBallPlayerData*>(fn_80021E30), 0, -1);
    UnidentifiedFindEvent<CollisionBirdoShotBallPlayerData>("CollisionBirdoShotBallPlayer", -1)->Add(Function<CollisionBirdoShotBallPlayerData*>(fn_80022050), 0, -1);
    UnidentifiedFindEvent<CollisionKoopaShellGoalieData>("CollisionKoopaShellGoalie", -1)->Add(Function<CollisionKoopaShellGoalieData*>(fn_80021D70), 0, -1);
    UnidentifiedFindEvent<CollisionBirdoEggGoalieData>("CollisionBirdoEggGoalie", -1)->Add(Function<CollisionBirdoEggGoalieData*>(fn_80021DCC), 0, -1);
    UnidentifiedFindEvent<UnidentifiedEventData16>("CollisionHammerbroShotBallPlayer", -1)->Add(Function<UnidentifiedEventData16*>(fn_80022280), 0, -1);
    UnidentifiedFindEvent<CollisionPowerupWallData>("CollisionPowerupWall", -1)->Add(Function<CollisionPowerupWallData*>(fn_80021C98), 0, -1);
    UnidentifiedFindEvent<CollisionPlayerBananaData>("CollisionPlayerBanana", -1)->Add(Function<CollisionPlayerBananaData*>(fn_80021924), 0, -1);
    UnidentifiedFindEvent<CollisionPlayerShellData>("CollisionPlayerShell", -1)->Add(Function<CollisionPlayerShellData*>(fn_800216C4), 0, -1);
    UnidentifiedFindEvent<CollisionPlayerFreezeData>("CollisionPlayerFreeze", -1)->Add(Function<CollisionPlayerFreezeData*>(fn_80021484), 0, -1);
    UnidentifiedFindEvent<UnidentifiedEventData24>("CollisionTongue", -1)->Add(Function<UnidentifiedEventData24*>(fn_8002147C), 0, -1);
    UnidentifiedFindEvent<CharacterImpactEvent>("MontyReappear", -1)->Add(Function<CharacterImpactEvent*>(fn_800212A0), 0, -1);
    UnidentifiedFindEvent<CharacterImpactEvent>("HammerBroHammer", -1)->Add(Function<CharacterImpactEvent*>(fn_80021120), 0, -1);
    UnidentifiedFindEvent<CharacterImpactEvent>("WarioGroundPound", -1)->Add(Function<CharacterImpactEvent*>(fn_80021120), 0, -1);
    UnidentifiedFindEvent<ReceiveBallData>("ReceiveBall", -1)->Add(Function<ReceiveBallData*>(fn_80020E20), 0, -1);
    UnidentifiedFindEvent<UnidentifiedEventData_80066008>("DirectionBegin", -1)->Add(Function<UnidentifiedEventData_80066008*>(fn_80020E1C), 0, -1);
    UnidentifiedFindEvent<GoalScoredData>("GoalScored", -1)->Add(Function<GoalScoredData*>(fn_80020CDC), 0, -1);
    UnidentifiedFindEvent<MegaStrikeEndData>("MegastrikeEnd", -1)->Add(Function<MegaStrikeEndData*>(fn_80020E04), 0, -1);
    UnidentifiedFindEvent<LightningStrikeData>("LightningStrike", -1)->Add(Function<LightningStrikeData*>(fn_80021050), 0, -1);
    UnidentifiedFindEvent<CollisionBulletBillData>("CollisionBulletBillPlayer", -1)->Add(Function<CollisionBulletBillData*>(fn_80020EE8), 0, -1);
    UnidentifiedFindEvent<CollisionBulletBillData>("CollisionBulletBillFreeze", -1)->Add(Function<CollisionBulletBillData*>(fn_80020FB8), 0, -1);
    UnidentifiedFindEvent<CollisionBulletBillData>("ExplosionBulletBill", -1)->Add(Function<CollisionBulletBillData*>(fn_80020FD4), 0, -1);
    UnidentifiedFindEvent<CollisionBulletBillData>("BulletBillExplode", -1)->Add(Function<CollisionBulletBillData*>(fn_80020FD4), 0, -1);
    UnidentifiedFindEvent<UnidentifiedEventData_80066A04>("PowerupStats", -1)->Add(Function<UnidentifiedEventData_80066A04*>(fn_80020C70), 0, -1);
    UnidentifiedFindEvent<PlayerAttackData>("AttackSuccess", -1)->Add(Function<PlayerAttackData*>(fn_80020BB0), 0, -1);
    UnidentifiedFindEvent<UnidentifiedEventData26>("CollisionHammerPlayer", -1)->Add(Function<UnidentifiedEventData26*>(fn_80022B1C), 0, -1);
    UnidentifiedFindEvent<UnidentifiedEventData24>("CollisionPatchPlayer", -1)->Add(Function<UnidentifiedEventData24*>(fn_80022B04), 0, -1);
    UnidentifiedFindEvent<UnidentifiedEventData26>("CollisionHammerGround", -1)->Add(Function<UnidentifiedEventData26*>(fn_80022A98), 0, -1);
    UnidentifiedFindEvent<UnidentifiedEventData34>("CollisionEggPlayer", -1)->Add(Function<UnidentifiedEventData34*>(fn_80022BD8), 0, -1);
    fn_80098750();
}

cCharacter::~cCharacter()
{
    EmissionManager::Instance()->Destroy((unsigned long)this, 0);
    m_pEffectsTexturing = 0;
    if (m_pPhysicsData != 0)
    {
        delete m_pPoseTree;
    }
    delete m_pPoseAccumulator;
    for (int i = 0; i < 4; ++i)
    {
        if (m_pSkinMesh[i] != 0)
        {
            delete m_pSkinMesh[i];
        }
    }
    if (m_pPhysicsCharacter != 0)
    {
        delete m_pPhysicsCharacter;
    }
    delete m_pHeadTrack;
    delete mUnidentified0F8;
    if (m_pBlinker != 0)
    {
        delete m_pBlinker;
    }
    mUnidentified1C0.Clear();
    mUnidentified1C0.GetAllocator()->FreeBlocks();
}

unsigned int lbl_806E0C24;

void cCharacter::GetJointPositionFuture(nlVector3* v3Out, int nAnimIndex,
    int nJointIndex, float fTime, bool bAddRootTrans, bool bAddRootRot,
    bool bUsePrevPosition, bool bParam4)
{
    bool unidentifiedPose = nJointIndex >= 0;
    bool unidentifiedCache = bParam4 && unidentifiedPose;
    if (unidentifiedCache)
    {
        if (nJointIndex > 50)
            unidentifiedCache = false;
        if (fTime > 1.0f)
            unidentifiedCache = false;
    }

    nlMatrix4 m4RootMat;
    m4RootMat.SetIdentity();
    cPoseAccumulator poseAccumulator(m_pPoseAccumulator->m_BaseSHierarchy, false);
    cSAnim* pAnim = m_pAnimInventory->GetAnim(nAnimIndex);
    AnimRetarget* pRetarget = 0;
    if (m_pAnimRetargetList != 0)
        pRetarget = m_pAnimRetargetList->GetAnimRetargetWithSignature(pAnim);
    cPN_SAnimController animController(pAnim, pRetarget, PM_CYCLIC, 0, 0, false);
    animController.m_bMirror = m_pAnimInventory->GetMirrored(nAnimIndex);
    animController.SetTime(fTime);

    UnidentifiedBitPacker unidentifiedKey;
    if (unidentifiedCache)
    {
        unidentifiedKey.UnidentifiedPack(nAnimIndex, 0, 178);
        unidentifiedKey.UnidentifiedPack(nJointIndex, 0, 50);
        unidentifiedKey.UnidentifiedPack(fTime, 0.0f, 1.0f, 0.01f);
        nlVector3* unidentifiedValue;
        if (mUnidentified1C0.FindGet(unidentifiedKey.UnidentifiedGet(), &unidentifiedValue))
        {
            ++lbl_806E0C24;
            *v3Out = *unidentifiedValue;
            unidentifiedPose = false;
        }
    }
    if (unidentifiedPose)
    {
        poseAccumulator.Pose(animController, m4RootMat);
        const nlMatrix4& m4NodeMatrix = poseAccumulator.GetNodeMatrix(nJointIndex);
        *v3Out = *(nlVector3*)&m4NodeMatrix.e2[3][0];
        if (unidentifiedCache)
        {
            mUnidentified1C0.Add(unidentifiedKey.UnidentifiedGet(), *v3Out);
            nlVector3* unidentifiedValue;
            mUnidentified1C0.Find(unidentifiedKey.UnidentifiedGet(), &unidentifiedValue, 0);
        }
    }
    if (bAddRootRot)
    {
        u16 aCurRotation = 0;
        if (bUsePrevPosition)
            aCurRotation = mUnidentified024.m_aActualFacingDirection;
        u16 aRootRotation;
        animController.GetRootRot(&aRootRotation);
        aCurRotation += aRootRotation;
        nlMakeRotationMatrixZ(m4RootMat, 0.0000958738f * (float)aCurRotation);
    }
    if (bAddRootTrans)
    {
        u16 aCurRotation = 0;
        if (bUsePrevPosition)
            aCurRotation = mUnidentified024.m_aActualFacingDirection;
        nlVector3 v3RootVelocity;
        animController.GetRootTrans(&v3RootVelocity, aCurRotation,
            mUnidentified024.m_fMovementScale);
        v3RootVelocity.z = 0.0f;
        if (bUsePrevPosition)
        {
            v3RootVelocity.x += mUnidentified024.m_v3Position.x;
            v3RootVelocity.y += mUnidentified024.m_v3Position.y;
        }
        if (nJointIndex < 0)
        {
            *v3Out = v3RootVelocity;
            return;
        }
        m4RootMat.SetTranslation(v3RootVelocity);
    }
    nlMatrix3 unidentifiedRotation;
    unidentifiedRotation.e2[0][0] = m4RootMat.e2[0][0];
    unidentifiedRotation.e2[0][1] = m4RootMat.e2[0][1];
    unidentifiedRotation.e2[0][2] = m4RootMat.e2[0][2];
    unidentifiedRotation.e2[1][0] = m4RootMat.e2[1][0];
    unidentifiedRotation.e2[1][1] = m4RootMat.e2[1][1];
    unidentifiedRotation.e2[1][2] = m4RootMat.e2[1][2];
    unidentifiedRotation.e2[2][0] = m4RootMat.e2[2][0];
    unidentifiedRotation.e2[2][1] = m4RootMat.e2[2][1];
    unidentifiedRotation.e2[2][2] = m4RootMat.e2[2][2];
    nlVec3Scale(*v3Out, mUnidentified024.m_fDesiredPlayerScale);
    nlMultVectorMatrix(*v3Out, *v3Out, unidentifiedRotation);
    nlVec3Add(*v3Out, *v3Out, m4RootMat.GetTranslation());
}

void cCharacter::GetCurrentAnimFuture(int nJointIndex, float fTime,
    nlVector3& v3Out, nlVector3& v3FutureRoot, unsigned short& outFacing)
{
    cPN_SAnimController* pAnim = m_pCurrentAnimController;
    float savedPrevTime = pAnim->m_fPrevTime;
    float savedTime = pAnim->m_fTime;
    pAnim->SetTime(fTime);

    outFacing = mUnidentified024.m_aActualFacingDirection;
    m_pCurrentAnimController->GetRootTrans(&v3FutureRoot, outFacing,
        mUnidentified024.m_fMovementScale);
    unsigned short rootRot;
    m_pCurrentAnimController->GetRootRot(&rootRot);
    outFacing += rootRot;
    v3FutureRoot.x += mUnidentified024.m_v3Position.x;
    v3FutureRoot.y += mUnidentified024.m_v3Position.y;
    v3FutureRoot.z = 0.0f;
    if (nJointIndex < 0)
    {
        v3Out = v3FutureRoot;
    }
    else
    {
        cPoseAccumulator pAccumulator(m_pPoseAccumulator->m_BaseSHierarchy, true);
        pAccumulator.m_Scale = mUnidentified024.m_fPlayerScale;
        nlMatrix4 m;
        nlMakeRotationMatrixZ(m, 0.0000958738f * (float)outFacing);
        m.e2[3][0] = v3FutureRoot.x;
        m.e2[3][1] = v3FutureRoot.y;
        m.e2[3][2] = v3FutureRoot.z;
        m.e2[3][3] = 1.0f;
        pAccumulator.Pose(*m_pCurrentAnimController, m);
        v3Out = *(nlVector3*)&pAccumulator.GetNodeMatrix(nJointIndex).e2[3][0];
    }
    pAnim = m_pCurrentAnimController;
    pAnim->SetTime(savedPrevTime);
    pAnim = m_pCurrentAnimController;
    pAnim->SetTime(savedTime);
}

static inline AnimRetarget* GetCharacterAnimRetarget(const cCharacter* character,
    const cSAnim* pSAnim)
{
    AnimRetarget* result = 0;
    if (character->m_pAnimRetargetList != 0)
        result = character->m_pAnimRetargetList->GetAnimRetargetWithSignature(pSAnim);
    return result;
}

s16 cCharacter::CalcAnimTurnAdjust(unsigned short aFacingDirection,
    unsigned short aDesiredFacingDirection, int nAnimID, float fParam)
{
    unsigned short aAnimRot;
    cSAnim* const pAnim = m_pAnimInventory->GetAnim(nAnimID);
    cPN_SAnimController* pAnimController = new cPN_SAnimController(
        pAnim,
        GetCharacterAnimRetarget(this, pAnim),
        m_pAnimInventory->GetPlayMode(nAnimID),
        NULL,
        0,
        m_pAnimInventory->GetMirrored(nAnimID));

    pAnimController->SetTime(0.0f);
    pAnimController->SetTime(fParam);
    pAnimController->GetRootRot(&aAnimRot);
    unsigned short aFinalFacingDirection = aFacingDirection + aAnimRot;
    delete pAnimController;
    return (signed short)(aDesiredFacingDirection - aFinalFacingDirection);
}

inline float ClampMin(float speedRatio, const float min);
inline float ClampMax(float speedRatio, const float max);

void cCharacter::MatchAnimSpeedToCharacterSpeed(unsigned int nParam,
    cPN_SAnimController* pController)
{
    cFielder* fielder = (cFielder*)nParam;
    if (fielder->mUnidentified024.m_eMovementState != MOVEMENT_FROM_ANIM
        && fielder->mUnidentified024.m_eMovementState != MOVEMENT_FROM_ANIM_SEEK)
    {
        pController->m_fPlaybackSpeedScale = ClampMax(ClampMin(
            fielder->mUnidentified024.m_fActualSpeed / pController->m_pSAnim->m_fLinearSpeed,
            0.6f), 1.4f);
    }
}

cPN_SAnimController* cCharacter::NewAnimController(int animID, bool bRestartCyclic, bool bForceMirrorSwap, void (*funcPlaybackSpeedCallback)(unsigned int, cPN_SAnimController*), unsigned int nPlaybackSpeedCallbackParam)
{
    bool restartCyclic = bRestartCyclic;
    bool forceMirrorSwap = bForceMirrorSwap;
    void (*playbackSpeedCallback)(unsigned int, cPN_SAnimController*) = funcPlaybackSpeedCallback;
    unsigned int playbackSpeedCallbackParam = (unsigned int)this;

    if (m_pAnimInventory->GetMatchCharacterSpeed(animID))
    {
        playbackSpeedCallback = MatchAnimSpeedToCharacterSpeed;
    }
    else if (funcPlaybackSpeedCallback != 0)
    {
        playbackSpeedCallbackParam = nPlaybackSpeedCallbackParam;
    }

    bool bMirrorSwap = false;
    float startTime = 0.0f;

    if (m_pCurrentAnimController != 0)
    {
        if (m_eClassType == FIELDER)
        {
            if (restartCyclic || m_pAnimInventory->GetPlayMode(m_eAnimID) == PM_HOLD)
            {
                if (m_pAnimInventory->GetPlayMode(animID) == PM_CYCLIC)
                {
                    if (m_pAnimInventory->GetEndPhase(m_eAnimID) == GOOFY_FOOT)
                    {
                        bMirrorSwap = true;
                    }
                    else if (m_pAnimInventory->GetMirrored(m_eAnimID))
                    {
                        bMirrorSwap = true;
                    }
                }
                else if (m_pAnimInventory->GetPlayMode(animID) == PM_HOLD)
                {
                    bMirrorSwap = forceMirrorSwap;
                }
            }
            else if (m_pAnimInventory->GetPlayMode(m_eAnimID) == PM_CYCLIC)
            {
                if (m_pAnimInventory->GetPlayMode(animID) == PM_CYCLIC)
                {
                    startTime = m_pCurrentAnimController->m_fTime;
                    if (m_pAnimInventory->GetMirrored(m_eAnimID) != m_pAnimInventory->GetMirrored(animID))
                    {
                        bMirrorSwap = true;
                    }
                }
                else if (forceMirrorSwap)
                {
                    bMirrorSwap = true;
                }
            }
        }
        else
        {
            if (restartCyclic || m_pAnimInventory->GetPlayMode(m_eAnimID) == PM_HOLD)
            {
                if (m_pAnimInventory->GetPlayMode(animID) == PM_CYCLIC)
                {
                    if (m_pAnimInventory->GetEndPhase(m_eAnimID) == LEFT_FOOT_DOWN)
                    {
                        startTime = 0.5f;
                    }
                    else
                    {
                        startTime = 0.0f;
                    }
                }
            }
            else if (m_pAnimInventory->GetPlayMode(m_eAnimID) == PM_CYCLIC)
            {
                if (m_pAnimInventory->GetPlayMode(animID) == PM_CYCLIC)
                {
                    startTime = m_pCurrentAnimController->m_fTime;
                    if (m_pAnimInventory->GetEndPhase(m_eAnimID) != m_pAnimInventory->GetEndPhase(animID))
                    {
                        startTime += 0.5f;
                        if (startTime >= 1.0f)
                        {
                            startTime -= 1.0f;
                        }
                    }
                }
            }
        }
    }

    cSAnim* anim = m_pAnimInventory->GetAnim(animID);
    cPN_SAnimController* controller = new cPN_SAnimController(
        anim, GetCharacterAnimRetarget(this, anim),
        m_pAnimInventory->GetPlayMode(animID), playbackSpeedCallback,
        playbackSpeedCallbackParam,
        bMirrorSwap ? !m_pAnimInventory->GetMirrored(animID) : m_pAnimInventory->GetMirrored(animID));
    controller->SetTime(startTime);

    return controller;
}

inline float ClampMin(float speedRatio, const float min)
{
    if (speedRatio >= min)
    {
        return speedRatio;
    }
    return min;
}

inline float ClampMax(float speedRatio, const float max)
{
    if (speedRatio <= max)
    {
        return speedRatio;
    }
    return max;
}

void cCharacter::SetAnimState(int animID, bool useBlendTime,
    float fNonDefaultBlendTime, bool bRestartCyclic, bool bForceMirrorSwap)
{
    float finalBlendTime;
    cPN_SAnimController* newController;
    cPN_Blender* blender;
    if (useBlendTime)
        finalBlendTime = m_pAnimInventory->GetBlendTime(animID);
    else
        finalBlendTime = fNonDefaultBlendTime;
    newController = NewAnimController(animID, bRestartCyclic, bForceMirrorSwap, 0, 0);
    if (m_pAILayer[0] != 0 && finalBlendTime != 0.0f)
    {
        if (m_pAILayer[0]->GetType() == 0)
        {
            blender = (cPN_Blender*)m_pAILayer[0];
            if (blender->GetNumChildren() == 2 && blender->m_fBlendTime < 0.001f)
            {
                m_pAILayer[0] = blender->GetChild(0);
                blender->SetChild(0, 0);
                delete blender;
            }
        }
        blender = new cPN_Blender(m_pAILayer[0], newController, finalBlendTime);
    }
    else
    {
        delete m_pAILayer[0];
        blender = (cPN_Blender*)newController;
    }
    m_pAILayer[0] = blender;
    m_pCurrentAnimController = newController;
    SetAnimID(animID);
    if (m_eClassType == FIELDER && mUnidentified024.m_eCharacterClass == 13)
    {
        mUnidentified17E = false;
        mUnidentified17F = false;
        fn_8001EFE4((cFielder*)this, 0);
    }
}

int lbl_806E0C28;

extern "C" void fn_8001EFE4(cFielder* character, bool bParam)
{
    bool unidentifiedEnd = false;
    if (character->mUnidentified180 && !bParam)
        unidentifiedEnd = true;
    character->mUnidentified180 = bParam;
    if (bParam)
    {
        if (lbl_806E0C28 == 0)
        {
            lbl_806E0C28 = character->m_pPoseAccumulator->m_BaseSHierarchy->GetNodeIndexByID(
                nlStringLowerHash("bip01 r prop"));
        }
        character->mUnidentified184 = character->m_pPoseAccumulator->GetNodeQuaternion(lbl_806E0C28);
        const nlMatrix4& m = character->m_pPoseAccumulator->GetNodeMatrix(lbl_806E0C28);
        character->mUnidentified194 = *(nlVector3*)&m.e2[3][0];
        character->mUnidentified1A0 = nlVec3Length(*(nlVector3*)&m.e2[0][0]);
    }
    if (unidentifiedEnd)
        fn_801B8CF4(character->mUnidentified194);
}

extern "C" float fn_8002BFA8(PlayerTweaks*, float);

nlVector3 g_v3PrevJointPosition = { 0.0f, 0.0f, 0.0f };
unsigned char lbl_806E0C22;
float lbl_806DB5F8 = 15.0f;

inline void cCharacter::UnidentifiedSetScale(float unidentifiedScale)
{
    mUnidentified024.m_fPlayerScale = unidentifiedScale;
    m_pPoseAccumulator->m_Scale = unidentifiedScale;
    float unidentifiedRadius;
    if (m_eClassType == FIELDER)
        unidentifiedRadius = fn_8002BFA8(((cFielder*)this)->GetTweaks(), 1.0f);
    else if (m_eClassType == GOALIE)
        unidentifiedRadius = ((Goalie*)this)->m_pTweaks->fPhysCapsuleRadius;
    unidentifiedRadius *= unidentifiedScale;
    m_pPhysicsCharacter->m_pPlayerPlayerColumn->SetRadius(unidentifiedRadius);
    m_pPhysicsCharacter->SetBoneVolumeScale(unidentifiedScale);
}

void cCharacter::fn_8001EE74(float fParam0, float fParam1, float fParam2)
{
    mUnidentified024.m_fDesiredPlayerScale = fParam0;
    if (fParam2 > 0.0f)
    {
        mUnidentified024.m_fDesiredMovementScale = fParam2;
    }
    else
    {
        mUnidentified024.m_fDesiredMovementScale = mUnidentified024.m_fMovementScale;
    }
    if (fParam1 > 0.0f)
    {
        mUnidentified024.m_tScaleTimer.SetSeconds(fParam1);
    }
    else
    {
        mUnidentified024.m_tScaleTimer.UnidentifiedClear();
        UnidentifiedSetScale(fParam0);
        mUnidentified024.m_fMovementScale = mUnidentified024.m_fDesiredMovementScale;
    }
}

void cCharacter::fn_8001EF6C(float movementScale)
{
    mUnidentified024.m_fMovementScale = movementScale;
}

void cCharacter::fn_8001DCE0(unsigned short aDirection)
{
    mUnidentified024.m_aDesiredMovementDirection = aDirection;
    if (mUnidentified024.m_eMovementState != MOVEMENT_STRAFING)
    {
        mUnidentified024.m_aDesiredFacingDirection = aDirection;
    }
}

void cCharacter::fn_8001EF78(float fParam)
{
    m_pPoseTree = m_pPoseTree->Update(fParam);
}

void cCharacter::Update(float fDeltaT)
{
    if (fDeltaT > 0.0f)
    {
        if (mUnidentified024.m_tScaleTimer.m_uPackedTime != 0)
        {
            if (!mUnidentified024.m_tScaleTimer.Countdown(fDeltaT, 0.0f))
            {
                float unidentifiedFraction = fDeltaT / mUnidentified024.m_tScaleTimer.GetSeconds();
                unidentifiedFraction = nlMinEquals(unidentifiedFraction, 1.0f);
                UnidentifiedSetScale(Interpolate(mUnidentified024.m_fPlayerScale,
                    mUnidentified024.m_fDesiredPlayerScale, unidentifiedFraction));
                mUnidentified024.m_fMovementScale = Interpolate(mUnidentified024.m_fMovementScale,
                    mUnidentified024.m_fDesiredMovementScale, unidentifiedFraction);
            }
            else
            {
                UnidentifiedSetScale(mUnidentified024.m_fDesiredPlayerScale);
                mUnidentified024.m_tScaleTimer.UnidentifiedClear();
                mUnidentified024.m_fMovementScale = mUnidentified024.m_fDesiredMovementScale;
            }
        }
        fn_8001EF78(fDeltaT);
        UpdateMovementState(fDeltaT);
    }
    if (m_bIsUsingElectrocutionTexture)
    {
        EmissionManager* unidentifiedManager = EmissionManager::Instance();
        if (!unidentifiedManager->IsPlaying((unsigned long)this,
                unidentifiedManager->GetEffectsGroup("electrocution")))
        {
            if (m_bIsUsingElectrocutionTexture)
                m_pEffectsTexturing = 0;
            m_bIsUsingElectrocutionTexture = false;
        }
    }
    if (lbl_806E0C22 == 1)
    {
        float unidentifiedRate = 1.0f / lbl_806DB5F8;
        if (m_Dirt > 0.0f)
        {
            m_Dirt -= unidentifiedRate * fDeltaT;
            m_Dirt = nlMaxEquals(0.0f, m_Dirt);
        }
        if (m_MinDirt > 0.0f)
        {
            m_MinDirt -= unidentifiedRate * fDeltaT;
            m_MinDirt = nlMaxEquals(0.0f, m_MinDirt);
        }
    }
    if (fDeltaT > 0.0f && m_pBlurHandler != 0)
    {
        bool bIsZero = nlNear(v3Zero.x, g_v3PrevJointPosition.x)
            && nlNear(v3Zero.y, g_v3PrevJointPosition.y)
            && nlNear(v3Zero.z, g_v3PrevJointPosition.z);
        if (bIsZero)
            g_v3PrevJointPosition = mUnidentified024.m_v3Position;
        nlVector3 jointPosition;
        nlVector3 forwardVector;
        if (m_eClassType == FIELDER)
        {
            cSHierarchy* hierarchy = m_pPoseAccumulator->m_BaseSHierarchy;
            const nlMatrix4& nodeMatrix = m_pPoseAccumulator->GetNodeMatrix(
                hierarchy->GetNodeIndexByID(nlStringLowerHash("bip01 spine1")));
            jointPosition = *(nlVector3*)&nodeMatrix.e2[3][0];
            nlVec3Set(forwardVector,
                mUnidentified024.m_v3Position.x - mUnidentified024.m_v3PrevPosition.x,
                mUnidentified024.m_v3Position.y - mUnidentified024.m_v3PrevPosition.y,
                mUnidentified024.m_v3Position.z - mUnidentified024.m_v3PrevPosition.z);
        }
        g_v3PrevJointPosition = jointPosition;
        m_pBlurHandler->AddViewOrientedPoint(jointPosition, forwardVector);
    }
}

bool cCharacter::ShouldStartCrossBlend(int nAnimID)
{
    float fCrossBlendTime = 0.5f * m_pAnimInventory->GetBlendTime(nAnimID);
    return (1.0f - m_pCurrentAnimController->get_fTime()) * m_pCurrentAnimController->m_pSAnim->GetDuration() <= fCrossBlendTime;
}

void cCharacter::UnidentifiedVirtual1C()
{
    SetAnimState(0, false, 0.0f, false, false);
    m_pCurrentAnimController->SetTime(0.0f);
    InitMovementNone(0.0f, 0.0f);
}

void cCharacter::fn_8001C510(int modelType)
{
    if (modelType == 0 || m_pSkinMesh[modelType] != NULL)
    {
        m_ModelType = modelType;
    }
}

bool cCharacter::fn_8001C534(int modelType)
{
    return m_pSkinMesh[modelType] != NULL;
}

void cCharacter::fn_8001C574()
{
    for (int i = 0; i < 4; ++i)
    {
        unknown_0x018[i] = false;
    }
}

void cCharacter::PoseSkinMesh(cPoseAccumulator* pPoseAccumulator, int modelType)
{
    if (m_pSkinMesh[modelType] == NULL)
    {
        modelType = 0;
    }
    if (!unknown_0x018[modelType])
    {
        m_pSkinMesh[modelType]->Pose(pPoseAccumulator);
        unknown_0x018[modelType] = true;
    }
}

extern "C" bool fn_80014D38(cBall*);
extern "C" bool fn_800392D8(cFielder*);
extern float lbl_806E0E40;
float lbl_806DB5D8 = 12.5f;
float lbl_806DB5DC = 0.4f;
float lbl_806DB5E0 = 0.5f;
float lbl_806DB5E4 = 5.0f;
float lbl_806DB5E8 = 7.0f;
float lbl_806E0C18;
float lbl_806E0C1C;

static inline float CharacterAnimSmoothStep(float x)
{
    return (x * (x * x)) * (x * (6.0f * x + (-15.0f)) + 10.0f);
}

bool cCharacter::fn_8001E160()
{
    return mUnidentified024.m_bOnScreen;
}

bool cCharacter::fn_8001E168() const
{
    return mUnidentified11C->mCaptainId != -1;
}

bool cCharacter::fn_8001E184()
{
    return fabsf(m_v3ScreenPosition.x) <= 0.95f
        && fabsf(m_v3ScreenPosition.y) <= 0.95f
        && fabsf(m_v3ScreenPosition.z) <= 1.0f;
}

void cCharacter::fn_8001E304(float fSpeed, float fDeltaT)
{
    mUnidentified024.m_fActualSpeed = SeekSpeed(mUnidentified024.m_fActualSpeed,
        fSpeed, mUnidentified024.m_fAccel, mUnidentified024.m_fDecel, fDeltaT);
    if (mUnidentified024.m_fActualSpeed > 25.0f)
    {
        mUnidentified024.m_fActualSpeed = 25.0f;
    }
    nlPolarToCartesian(mUnidentified024.m_v3Velocity.x,
        mUnidentified024.m_v3Velocity.y, mUnidentified024.m_aActualMovementDirection,
        mUnidentified024.m_fActualSpeed);
}

void cCharacter::UpdateMovementState(float fDeltaT)
{
    float fDesiredSpeed = mUnidentified024.m_fDesiredSpeed;
    cFielder* pFielder = NULL;

    if (m_eClassType == FIELDER)
    {
        pFielder = (cFielder*)this;
        int shotState = pFielder->m_pShotMeter->m_eShotMeterState;
        bool isCharging = shotState == SHOT_METER_ACTIVE || shotState == SHOT_METER_STS_ACTIVE;
        if (!isCharging)
        {
            fDesiredSpeed = pFielder->GetSpeedPowerupAdjusted(mUnidentified024.m_fDesiredSpeed);
        }
    }

    switch (mUnidentified024.m_eMovementState)
    {
    case MOVEMENT_COAST:
    {
        nlVector3 unidentifiedVelocity = mUnidentified024.m_v3Velocity;
        float mag = unidentifiedVelocity.GetLengthSq2D();
        if (mag > 625.0f)
        {
            nlPolar polar;
            nlCartesianToPolar(polar, mUnidentified024.m_v3Velocity.x, mUnidentified024.m_v3Velocity.y);
            nlPolarToCartesian(mUnidentified024.m_v3Velocity.x, mUnidentified024.m_v3Velocity.y, polar.a, 25.0f);
        }
        if (fabsf(mUnidentified024.m_v3Position.y) > 100.0f || fabsf(mUnidentified024.m_v3Position.x) > 100.0f)
        {
            nlVector3 unidentifiedZero;
            nlVec3Set(unidentifiedZero, 0.0f, 0.0f, 0.0f);
            SetVelocity(unidentifiedZero);
        }
        break;
    }

    case MOVEMENT_DECELERATE_EXPONENTIAL:
    {
        mUnidentified024.m_fActualSpeed = SeekSpeedExponential(mUnidentified024.m_fActualSpeed, fDesiredSpeed, mUnidentified024.m_fDecel, fDeltaT);
        nlPolarToCartesian(mUnidentified024.m_v3Velocity.x, mUnidentified024.m_v3Velocity.y, mUnidentified024.m_aActualMovementDirection, mUnidentified024.m_fActualSpeed);
        break;
    }

    case MOVEMENT_FROM_ANIM:
    {
        cPoseNode* pSourceNode;
        if (mUnidentified024.m_bFromAnimBlended)
        {
            pSourceNode = *m_pAILayer;
        }
        else
        {
            pSourceNode = m_pCurrentAnimController;
        }

        s16 nAdjust = 0;
        float adjustTime = mUnidentified024.m_fAnimAdjustEndTime - mUnidentified024.m_fAnimAdjustBeginTime;
        nlVector3 v3ConsumedMove;
        nlVec3Set(v3ConsumedMove, 0.0f, 0.0f, 0.0f);

        if (adjustTime > 0.0f)
        {
            float smoothStep1 = CharacterAnimSmoothStep((m_pCurrentAnimController->m_fTime - mUnidentified024.m_fAnimAdjustBeginTime) / adjustTime);
            smoothStep1 = (smoothStep1 <= 1.0f) ? smoothStep1 : 1.0f;

            float smoothStep2 = CharacterAnimSmoothStep((m_pCurrentAnimController->m_fPrevTime - mUnidentified024.m_fAnimAdjustBeginTime) / adjustTime);
            smoothStep2 = (smoothStep2 <= 1.0f) ? smoothStep2 : 1.0f;

            if (smoothStep2 < 1.0f)
            {
                float fAdjustPercent = (smoothStep1 - smoothStep2) / (1.0f - smoothStep2);
                nAdjust = (s16)((float)mUnidentified024.m_nAnimTurnAdjust * fAdjustPercent);
                mUnidentified024.m_nAnimTurnAdjust -= nAdjust;

                nlVec3Scale(v3ConsumedMove, mUnidentified024.m_v3AnimMoveAdjust, fAdjustPercent);
                nlVec3Sub(mUnidentified024.m_v3AnimMoveAdjust, mUnidentified024.m_v3AnimMoveAdjust, v3ConsumedMove);
            }
        }

        u16 aRootRotation;
        pSourceNode->GetRootRot(&aRootRotation);
        u16 prevFacing = mUnidentified024.m_aActualFacingDirection;
        u16 newFacing = prevFacing + aRootRotation + (u16)nAdjust;
        SetFacingDirection(newFacing, true);

        nlVector3 v3RootTrans;
        pSourceNode->GetRootTrans(&v3RootTrans, mUnidentified024.m_aPrevFacingDirection, mUnidentified024.m_fMovementScale);
        nlVec3Add(v3RootTrans, v3RootTrans, v3ConsumedMove);
        mUnidentified024.m_v3Velocity.x = v3RootTrans.x / fDeltaT;
        mUnidentified024.m_v3Velocity.y = v3RootTrans.y / fDeltaT;

        nlPolar aSpeed;
        nlCartesianToPolar(aSpeed, mUnidentified024.m_v3Velocity.x, mUnidentified024.m_v3Velocity.y);
        mUnidentified024.m_fActualSpeed = aSpeed.r;
        break;
    }

    case MOVEMENT_FROM_ANIM_SEEK:
    {
        u16 aNewFacingDirection = SeekDirection(mUnidentified024.m_aActualFacingDirection, mUnidentified024.m_aDesiredFacingDirection, mUnidentified024.m_fDirectionSeekSpeed, mUnidentified024.m_fDirectionSeekFalloff, fDeltaT);
        SetFacingDirection(aNewFacingDirection, true);

        nlVector3 v3RootTrans;
        m_pCurrentAnimController->GetRootTrans(&v3RootTrans, mUnidentified024.m_aPrevFacingDirection, mUnidentified024.m_fMovementScale);
        mUnidentified024.m_v3Velocity.x = v3RootTrans.x / fDeltaT;
        mUnidentified024.m_v3Velocity.y = v3RootTrans.y / fDeltaT;
        break;
    }

    case MOVEMENT_NONE:
    {
        u16 aNewFacingDirection = SeekDirection(mUnidentified024.m_aActualFacingDirection, mUnidentified024.m_aDesiredFacingDirection, mUnidentified024.m_fDirectionSeekSpeed, mUnidentified024.m_fDirectionSeekFalloff, fDeltaT);
        SetFacingDirection(aNewFacingDirection, true);

        mUnidentified024.m_fActualSpeed = 0.0f;
        nlPolarToCartesian(mUnidentified024.m_v3Velocity.x, mUnidentified024.m_v3Velocity.y, mUnidentified024.m_aActualMovementDirection, mUnidentified024.m_fActualSpeed);
        break;
    }

    case MOVEMENT_RUNNING:
    {
        u16 aNewFacingDirection = SeekDirection(mUnidentified024.m_aActualMovementDirection, mUnidentified024.m_aDesiredMovementDirection, mUnidentified024.m_fDirectionSeekSpeed, mUnidentified024.m_fDirectionSeekFalloff, fDeltaT);

        int delta = (s16)(aNewFacingDirection - mUnidentified024.m_aActualMovementDirection);
        int maxDelta = (int)(fDeltaT * mUnidentified024.m_fDirectionSeekSpeed);
        int sign = delta >> 31;
        int absDelta = (sign ^ delta) - sign;

        int unidentifiedLeanThreshold = 182;
        if (absDelta <= unidentifiedLeanThreshold)
        {
            mUnidentified024.m_fLeanAmount = 0.0f;
        }
        else
        {
            mUnidentified024.m_fLeanAmount = NormalizeVal((float)absDelta, (float)unidentifiedLeanThreshold, (float)maxDelta);
            if (delta < 0)
            {
                mUnidentified024.m_fLeanAmount = -mUnidentified024.m_fLeanAmount;
            }
        }

        mUnidentified024.m_aActualMovementDirection = aNewFacingDirection;
        if (fDesiredSpeed < 0.1f)
        {
            u16 unidentifiedFacing = SeekDirection(mUnidentified024.m_aActualFacingDirection, mUnidentified024.m_aDesiredFacingDirection, mUnidentified024.m_fDirectionSeekSpeed, mUnidentified024.m_fDirectionSeekFalloff, fDeltaT);
            SetFacingDirection(unidentifiedFacing, false);
        }
        else
        {
            SetFacingDirection(aNewFacingDirection, true);
        }

        fn_8001E304(fDesiredSpeed, fDeltaT);
        break;
    }

    case MOVEMENT_RUNNING_NO_TURN:
    {
        fn_8001E304(fDesiredSpeed, fDeltaT);
        break;
    }

    case MOVEMENT_STRAFING:
    {
        u16 aNewFacingDirection = SeekDirection(mUnidentified024.m_aActualFacingDirection, mUnidentified024.m_aDesiredFacingDirection, mUnidentified024.m_fDirectionSeekSpeed, mUnidentified024.m_fDirectionSeekFalloff, fDeltaT);
        SetFacingDirection(aNewFacingDirection, false);

        mUnidentified024.m_aActualMovementDirection = SeekDirection(mUnidentified024.m_aActualMovementDirection, mUnidentified024.m_aDesiredMovementDirection, mUnidentified024.m_fDirectionSeekSpeed, mUnidentified024.m_fDirectionSeekFalloff, fDeltaT);

        fn_8001E304(fDesiredSpeed, fDeltaT);
        break;
    }

    case MOVEMENT_UNUSED:
    default:
        break;
    }

    if (pFielder != NULL && !fn_80014D38(g_pBall)
        && fn_800392D8(pFielder) && pFielder->m_eActionState != 28)
    {
        float unidentifiedSlide = g_pGame->mpTerrain->GetSlideFactor();
        unidentifiedSlide += pFielder->fn_8003E72C() ? lbl_806E0E40 : 0.0f;
        if (unidentifiedSlide > 1.0f)
        {
            unidentifiedSlide = 1.0f;
        }
        float unidentifiedBlend = InterpolateClamped(0.0f,
            gGameTweaks.mFielderTweaks->mUnidentified464, unidentifiedSlide);
        nlVector2 unidentifiedDelta;
        nlVec2Sub(unidentifiedDelta, *(const nlVector2*)&mUnidentified024.m_v3PrevVelocity,
            *(const nlVector2*)&mUnidentified024.m_v3Velocity);
        float unidentifiedLengthSquared = nlVec2LengthSquared(unidentifiedDelta);
        if (unidentifiedLengthSquared > lbl_806DB5D8 * lbl_806DB5D8)
        {
            float unidentifiedScale = lbl_806DB5D8 * nlRecipSqrt(unidentifiedLengthSquared, true);
            nlVec2Set(unidentifiedDelta,
                unidentifiedScale * unidentifiedDelta.x,
                unidentifiedScale * unidentifiedDelta.y);
        }
        nlVector2 unidentifiedPosition = *(const nlVector2*)&mUnidentified024.m_v3Position;
        nlVec2Set(unidentifiedPosition,
            fDeltaT * unidentifiedDelta.x + unidentifiedPosition.x,
            fDeltaT * unidentifiedDelta.y + unidentifiedPosition.y);
        nlVec2Set(*(nlVector2*)&mUnidentified024.m_v3Position, unidentifiedPosition.x, unidentifiedPosition.y);
        if (m_pPhysicsCharacter->m_CanCollideWithWall && unidentifiedSlide > 0.0f)
        {
            cField::FixOutOfBoundsPosition(mUnidentified024.m_v3Position, 0.0f, false);
            m_pPhysicsCharacter->SetCharacterPositionXY(mUnidentified024.m_v3Position);
        }
        PhysicsAIBall* unidentifiedBall = g_pBall->m_pPhysicsBall;
        if (unidentifiedBall->mbUseTiltForce || unidentifiedBall->mbUseWindForce)
        {
            nlVector3 unidentifiedForce;
            float unidentifiedForceScale;
            if (unidentifiedBall->mbUseTiltForce && unidentifiedBall->mbUseWindForce)
            {
                nlVec3Add(unidentifiedForce, unidentifiedBall->mv3TiltForce, unidentifiedBall->mv3WindForce);
                unidentifiedForceScale = InterpolateClamped(
                    lbl_806E0C1C + lbl_806DB5E0, lbl_806E0C18 + lbl_806DB5DC,
                    pFielder->GetTweaks()->mUnidentified064);
            }
            else if (unidentifiedBall->mbUseTiltForce)
            {
                unidentifiedForce = unidentifiedBall->mv3TiltForce;
                unidentifiedForceScale = InterpolateClamped(lbl_806DB5E0, lbl_806DB5DC,
                    pFielder->GetTweaks()->mUnidentified064);
            }
            else
            {
                unidentifiedForce = unidentifiedBall->mv3WindForce;
                unidentifiedForceScale = InterpolateClamped(lbl_806E0C1C, lbl_806E0C18,
                    pFielder->GetTweaks()->mUnidentified064);
            }
            unidentifiedForceScale = InterpolateRangeClamped(0.0f, unidentifiedForceScale,
                lbl_806DB5E4, lbl_806DB5E8, nlVec3Length(mUnidentified024.m_v3Velocity));
            unidentifiedForceScale = fDeltaT * unidentifiedForceScale;
            nlVec2Set(*(nlVector2*)&mUnidentified024.m_v3Position,
                unidentifiedForceScale * unidentifiedForce.x + mUnidentified024.m_v3Position.x,
                unidentifiedForceScale * unidentifiedForce.y + mUnidentified024.m_v3Position.y);
            if (GameInfoManager::Instance()->GetStadium() != 11 && m_pPhysicsCharacter->m_CanCollideWithWall)
            {
                cField::FixOutOfBoundsPosition(mUnidentified024.m_v3Position, 0.0f, false);
            }
            else if (m_pPhysicsCharacter->m_CanCollideWithGoalLine && !pFielder->fn_800344B0())
            {
                cField::FixOutOfBoundsX(mUnidentified024.m_v3Position, false, 0.0f);
            }
            m_pPhysicsCharacter->SetCharacterPositionXY(mUnidentified024.m_v3Position);
        }
        nlVecLerp(mUnidentified024.m_v3PrevVelocity, mUnidentified024.m_v3Velocity, mUnidentified024.m_v3PrevVelocity, unidentifiedBlend);
    }
    else
    {
        mUnidentified024.m_v3PrevVelocity = mUnidentified024.m_v3Velocity;
    }
    if (mUnidentified024.m_fActualSpeed > 0.01f)
    {
        nlPolar pMovement;
        nlCartesianToPolar(pMovement, mUnidentified024.m_v3Velocity.x, mUnidentified024.m_v3Velocity.y);
        mUnidentified024.m_aActualMovementDirection = pMovement.a;
    }
    else
    {
        mUnidentified024.m_aActualMovementDirection = mUnidentified024.m_aActualFacingDirection;
    }
    m_pPhysicsCharacter->SetCharacterVelocityXY(mUnidentified024.m_v3Velocity);
}

unsigned long lbl_8056B7B0[10] = {
    nlStringLowerHash("NLG_DIFFUSE"),
    nlStringLowerHash("NLG_DETAIL"),
    nlStringLowerHash("NLG_SPECULAR"),
    nlStringLowerHash("NLG_BUMPMAP"),
    nlStringLowerHash("NLG_NORMALMAP"),
    nlStringLowerHash("NLG_SHADOW"),
    nlStringLowerHash("NLG_SELFILLUM"),
    nlStringLowerHash("NLG_GLOSS"),
    nlStringLowerHash("NLG_RAMP"),
    nlStringLowerHash("NLG_MASK"),
};

void cCharacter::fn_80022E60()
{
    if (mUnidentified118)
    {
        return;
    }
    for (int i = 0; i < 4; ++i)
    {
        if (m_pSkinMesh[i] != NULL)
        {
            int modelIndex = m_pSkinMesh[i]->m_Unknown0C;
            for (int j = 0; j < 2; ++j)
            {
                m_pSkinMesh[i]->m_Unknown0C = j;
                if (m_pSkinMesh[i]->GetModel() != NULL)
                {
                    for (glModelPacket* packet = m_pSkinMesh[i]->GetModel()->packets;
                         packet < m_pSkinMesh[i]->GetModel()->packets
                                      + m_pSkinMesh[i]->GetModel()->numPackets;
                         ++packet)
                    {
                        for (int k = 0; k < 10; ++k)
                        {
                            if (glHasMaterialParameter(packet, lbl_8056B7B0[k]))
                            {
                                unsigned long texture = glGetMaterialUnsignedParameter(packet, lbl_8056B7B0[k]);
                                unsigned long resolvedTexture = glGetTextureManager()->GetTextureIndex(texture);
                                glSetMaterialTextureIndexParameter(packet, lbl_8056B7B0[k], &resolvedTexture);
                            }
                        }
                        if (packet->materialProgram == GXCharacterDamageMaterialProgram::Instance)
                        {
                            glGetTextureManager()->ResolveTextureIndex(
                                &static_cast<GXCharacterDamageParameters*>(packet->materialParameters)->damage1Texture);
                            glGetTextureManager()->ResolveTextureIndex(
                                &static_cast<GXCharacterDamageParameters*>(packet->materialParameters)->damage2Texture);
                            glGetTextureManager()->ResolveTextureIndex(
                                &static_cast<GXCharacterDamageParameters*>(packet->materialParameters)->megaTexture);
                        }
                    }
                }
            }
            m_pSkinMesh[i]->m_Unknown0C = modelIndex;
        }
    }
    mUnidentified118 = true;
}
