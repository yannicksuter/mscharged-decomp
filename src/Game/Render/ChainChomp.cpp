#include "Game/Render/ChainChomp.h"

#include "Game/AI/AiUtil.h"
#include "Game/AI/Powerups.h"
#include "Game/Ball.h"
#include "Game/BasicStadium.h"
#include "Game/Drawable/ShadowProp.h"
#include "Game/Effects/EmissionManager.h"
#include "Game/Game.h"
#include "Game/GameTweaks.h"
#include "Game/Physics/PhysicsObject.h"
#include "Game/PoseAccumulator.h"
#include "Game/SAnim/pnSAnimController.h"
#include "NL/gl/glState.h"
#include "NL/nlColour.h"

struct UnidentifiedChainCollisionData
{
    void* mUnidentified00;
    PowerupBase* pPowerup;
};

extern "C" void* fn_8027267C(int);

extern const float lbl_806E4EF0;
extern const float lbl_806E4EFC;
extern const float lbl_806E4F00;
extern const float lbl_806E4F04;
extern const float lbl_806E4F08;
extern const float lbl_806E4F24;
extern const float lbl_806E4F2C;
extern const float lbl_806E4F58;
extern const float lbl_806E4F5C;
extern const float lbl_806E4F60;
extern const float lbl_806E4F64;
extern const float lbl_806E4F68;
extern unsigned char lbl_806E14F4;
extern void* lbl_806E14F8;
extern int lbl_80573CA8[];
extern const nlVector3 lbl_804DCDD0;
extern const nlVector3 lbl_804DCDDC;
extern const char lbl_80511C5C[];
extern const char lbl_80511CF0[];
extern const char lbl_806DCDB0[7];
extern "C" float fn_800A9274(void*);
extern "C" EffectsGroup* fn_802E7CDC(EmissionManager*, const char*);

float lbl_806DCD94 = 100.0f;
float lbl_806DCD98 = 100.0f;

/**
 * Offset/Address/Size: 0x0 | 0x8019C988 | size: 0x10
 */
bool ChainChomp::IsHidden() const
{
    return meChainChompState == CHAIN_STATE_HIDDEN;
}

extern "C" bool fn_8019C998(const ChainChomp* pChomp)
{
    return pChomp->meChainChompState == CHAIN_STATE_UNIDENTIFIED_5;
}

/**
 * Offset/Address/Size: 0x0 | 0x8019C9AC | size: 0x11C
 */
void ChainChomp::Leave()
{
    meChainChompState = CHAIN_STATE_LEAVE;

    float fSpeedScale = InterpolateRangeClamped(lbl_806E4EFC,
        lbl_806E4F00, lbl_806E4EF0, lbl_806E4F04,
        fn_800A9274(g_pGame->mUnidentified10D8));
    mfDesiredSpeed = mUnidentifiedA0 * fSpeedScale;
    SetAnimState(*mpIdleAnim, lbl_806E4EF0, PM_CYCLIC);

    nlVector3 v3TargetPosition;
    if (lbl_806E4F08 * g_pBall->m_v3Velocity.x
            + g_pBall->m_v3Position.x
        < lbl_806E4EF0)
    {
        v3TargetPosition.x = lbl_806E4F5C;
    }
    else
    {
        v3TargetPosition.x = lbl_806E4F60;
    }

    if (mv3Position.y < lbl_806E4EF0)
    {
        v3TargetPosition.y = lbl_806E4F64;
    }
    else
    {
        v3TargetPosition.y = lbl_806E4F68;
    }
    v3TargetPosition.z = lbl_806E4EF0;

    float fTargetX = v3TargetPosition.x - mv3Position.x;
    float fTargetY = v3TargetPosition.y - mv3Position.y;
    float fAngle = nlATan2f(fTargetY, fTargetX);
    maDesiredFacingDirection = (u16)(s32)(lbl_806E4F24 * fAngle);

    PowerupBase::PlayPowerupSound(POWER_UP_CHAIN_CHOMP,
        PowerupBase::PWRUP_SOUND_IN_EFFECT, mv3Position, lbl_806E4EF0, 0);
}

/**
 * Offset/Address/Size: 0x0 | 0x8019CAC8 | size: 0xE0
 */
void ChainChomp::Move(float fDeltaT)
{
    nlPolar aChainSpeed;
    nlVector3 v3NewVelocity;
    float fDesiredSpeed;

    nlCartesianToPolar(aChainSpeed, mv3Velocity);

    if (aChainSpeed.r < 0.05f)
    {
        aChainSpeed.a = maFacingDirection;
    }

    fDesiredSpeed = mfDesiredSpeed;
    float fNewSpeed = SeekSpeed(
        aChainSpeed.r, fDesiredSpeed, lbl_806DCD94, lbl_806DCD98, fDeltaT);

    maFacingDirection = SeekDirection(
        aChainSpeed.a, maDesiredFacingDirection, 60000.0f, 3000.0f, fDeltaT);

    nlPolarToCartesian(
        v3NewVelocity.x, v3NewVelocity.y, maFacingDirection, fNewSpeed);
    v3NewVelocity.z = mv3Velocity.z;
    mv3Velocity = v3NewVelocity;
}

extern "C" void fn_8019CBA8(void*)
{
}

extern "C" void fn_8019CBAC(void*)
{
}

extern "C" void fn_8019CBB0(UnidentifiedChainCollisionData& collision)
{
    PowerupBase* pPowerup = collision.pPowerup;
    if (pPowerup->m_eType != POWER_UP_SPINY_SHELL)
    {
        pPowerup->m_bShouldDestroy = true;
    }
}

/**
 * Offset/Address/Size: 0x0 | 0x8019CBCC | size: 0x2AC
 */
void ChainChomp::DrawShadow(
    const cPoseAccumulator& pa, const nlMatrix4& worldMatrix)
{
    switch (meChainChompState)
    {
    default:
        if (lbl_806E14F4)
        {
            SkinAnimatedNPC::DrawShadow(mpLastModel, mWorldMatrix);
            return;
        }

        nlMatrix4& nodeMatrix = pa.GetNodeMatrix(4);
        nlVector3 v3ModelPosition;
        v3ModelPosition = nodeMatrix.GetTranslation();
        float x = v3ModelPosition.x;
        float y = v3ModelPosition.y;
        float half_dim;
        float frac;

        frac = lbl_806E4EF0;
        if (meChainChompState == CHAIN_STATE_RECOVER)
        {
            frac = lbl_806E4F04 - mpAnimController->get_fTime();
        }
        if (frac < lbl_806E4EF0)
        {
            frac = lbl_806E4EF0;
        }
        if (frac > lbl_806E4F04)
        {
            frac = lbl_806E4F04;
        }

        half_dim = lbl_806E4F04 - frac;
        float radius = lbl_8056CF08.m_pGameTweaks->fChainChompRadius;
        radius = lbl_806E4F58 * radius;
        float fAlpha = 150.0f * half_dim + 60.0f * frac;
        half_dim = lbl_806E4F2C * frac + radius * half_dim;

        int alpha = (int)fAlpha;
        if (alpha < 0)
        {
            alpha = 0;
        }
        if (alpha > 255)
        {
            alpha = 255;
        }

        GroundInfo* ground =
            reinterpret_cast<GroundInfo*>(BasicStadium::GetCurrentStadium());
        float groundHeight = lbl_806E4EF0;
        if (ground != 0)
        {
            groundHeight = ground->height;
        }

        nlVector3 position;
        position.x = x;
        position.y = y;
        position.z = 0.015625f + groundHeight;

        float minX;
        float minY = position.y - half_dim;
        minX = position.x - half_dim;
        float maxY = position.y + half_dim;
        float maxX = position.x + half_dim;

        nlColour c;
        c.c[0] = 255;
        c.c[1] = 255;
        c.c[2] = 255;
        c.c[3] = (unsigned char)alpha;

        glQuad3 quad;
        quad.m_pos[0].x = minX;
        quad.m_pos[0].y = minY;
        quad.m_pos[0].z = position.z;
        quad.m_pos[1].x = minX;
        quad.m_pos[1].y = maxY;
        quad.m_pos[1].z = position.z;
        quad.m_pos[2].x = maxX;
        quad.m_pos[2].y = maxY;
        quad.m_pos[2].z = position.z;
        quad.m_pos[3].x = maxX;
        quad.m_pos[3].y = minY;
        quad.m_pos[3].z = position.z;

        quad.m_uv[0].x = lbl_806E4F04;
        quad.m_uv[0].y = lbl_806E4F04;
        quad.m_uv[1].x = lbl_806E4EF0;
        quad.m_uv[1].y = lbl_806E4F04;
        quad.m_uv[2].x = lbl_806E4EF0;
        quad.m_uv[2].y = lbl_806E4EF0;
        quad.m_uv[3].x = lbl_806E4F04;
        quad.m_uv[3].y = lbl_806E4EF0;

        *(u32*)&quad.m_colour[3] = *(u32*)&c;
        *(u32*)&quad.m_colour[2] = *(u32*)&c;
        *(u32*)&quad.m_colour[1] = *(u32*)&c;
        *(u32*)&quad.m_colour[0] = *(u32*)&c;

        glSetDefaultState(true);
        glSetRasterState(GLS_AlphaBlend, 1);
        glSetRasterState(GLS_Culling, 0);
        glSetRasterState(GLS_DepthWrite, 0);
        glSetCurrentRasterState(glHandleizeRasterState());
        glSetCurrentTexture(glGetTexture(lbl_80511CF0), GLTT_Diffuse);
        glSetTextureState(GLTS_DiffuseWrap, 3);
        glSetCurrentTextureState(glHandleizeTextureState());

        void* renderContext;
        if (lbl_80573CA8[0] == 1)
        {
            renderContext = fn_8027267C(13);
        }
        else
        {
            renderContext = lbl_806E14F8;
        }
        quad.Attach((eGLView)(u32)renderContext, 0);
        break;
    case CHAIN_STATE_HIDDEN:
    case CHAIN_STATE_FALL:
    case CHAIN_STATE_UNIDENTIFIED_5:
        return;
    }
}

/**
 * Offset/Address/Size: 0x0 | 0x8019CE78 | size: 0x120
 */
void ChainChomp::Hide()
{
    if (mpInEffectSFX != 0)
    {
        mpInEffectSFX = 0;
    }

    EffectsGroup* pEffectsGroup =
        fn_802E7CDC(EmissionManager::Instance(), lbl_80511C5C);
    EmissionManager::Instance()->Destroy(
        reinterpret_cast<unsigned long>(this), pEffectsGroup);

    pEffectsGroup = fn_802E7CDC(EmissionManager::Instance(), lbl_806DCDB0);
    EmissionManager::Instance()->Destroy(
        reinterpret_cast<unsigned long>(this), pEffectsGroup);

    meChainChompState = CHAIN_STATE_HIDDEN;
    mfDesiredSpeed = lbl_806E4EF0;
    SetAnimState(*mpIdleAnim, lbl_806E4EF0, PM_CYCLIC);
    SetPosition(lbl_804DCDDC);
    maFacingDirection = 0;
    mv3Velocity = lbl_804DCDD0;
    mpPhysObj->DisableCollisions();

    mpThrower = 0;
    mnThrowerPadID = -1;
    mbIsVisible = false;
    mpTarget = 0;
    mtStateTimer.m_unk0 = mtStateTimer.m_uPackedTime != 0;
    mtStateTimer.m_uPackedTime = 0;
    mUnidentifiedA0 = lbl_806E4EF0;
}
