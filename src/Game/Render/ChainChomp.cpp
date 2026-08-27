#include "Game/Render/ChainChomp.h"

#include "Game/AI/AiUtil.h"
#include "Game/AI/Powerups.h"
#include "Game/Drawable/ShadowProp.h"
#include "Game/GameTweaks.h"
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
extern const float lbl_806E4F04;
extern const float lbl_806E4F2C;
extern const float lbl_806E4F58;
extern unsigned char lbl_806E14F4;
extern void* lbl_806E14F8;
extern int lbl_80573CA8[];
extern const char lbl_80511CF0[];

float lbl_806DCD94 = 100.0f;
float lbl_806DCD98 = 100.0f;

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

        GroundInfo* ground = fn_802772BC();
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

        ShadowQuad quad;
        quad.corners[0].x = minX;
        quad.corners[0].y = minY;
        quad.corners[0].z = position.z;
        quad.corners[1].x = minX;
        quad.corners[1].y = maxY;
        quad.corners[1].z = position.z;
        quad.corners[2].x = maxX;
        quad.corners[2].y = maxY;
        quad.corners[2].z = position.z;
        quad.corners[3].x = maxX;
        quad.corners[3].y = minY;
        quad.corners[3].z = position.z;

        quad.uv[0][0] = lbl_806E4F04;
        quad.uv[0][1] = lbl_806E4F04;
        quad.uv[1][0] = lbl_806E4EF0;
        quad.uv[1][1] = lbl_806E4F04;
        quad.uv[2][0] = lbl_806E4EF0;
        quad.uv[2][1] = lbl_806E4EF0;
        quad.uv[3][0] = lbl_806E4F04;
        quad.uv[3][1] = lbl_806E4EF0;

        quad.colors[3] = *(u32*)&c;
        quad.colors[2] = *(u32*)&c;
        quad.colors[1] = *(u32*)&c;
        quad.colors[0] = *(u32*)&c;

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
        fn_802C9664(&quad, renderContext, 0);
        break;
    case CHAIN_STATE_HIDDEN:
    case CHAIN_STATE_FALL:
    case CHAIN_STATE_UNIDENTIFIED_5:
        return;
    }
}
