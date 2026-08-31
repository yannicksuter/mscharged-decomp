#include "Game/BasicStadium.h"
#include "Game/AI/Powerups.h"
#include "Game/Drawable/DrawablePowerup.h"
#include "NL/gl/glDraw3.h"
#include "NL/gl/glModel.h"
#include "NL/gl/glState.h"
#include "NL/nlMath.h"
#include "NL/nlString.h"

struct UnidentifiedStaticState
{
    UnidentifiedStaticState()
        : value(0)
    {
    }

    void* value;
};

template <typename T>
struct UnidentifiedStaticStorage
{
    static UnidentifiedStaticState state;
};

struct UnidentifiedStaticTag;

template <typename T>
UnidentifiedStaticState UnidentifiedStaticStorage<T>::state;

class RenderObject
{
public:
    virtual void slot08();
    virtual void slot0C();
    virtual void* GetShadowSource();
    virtual void SetTransform(const nlMatrix4*);
    virtual void slot18();
    virtual void Draw();

    char _04[0x60];
    glModel* model;
    char _68[8];
    u32 flags;
};

struct ShadowHeight
{
    char _00[0x98];
    float height;
};

extern "C" RenderObject* fn_8027725C(u32);
extern "C" void fn_802B549C(nlQuaternion*, u16);
extern "C" void fn_801869AC(void*, void*, int, int, const DrawablePowerup*, float);
extern "C" u32 fn_8027262C();

u8 sDrawPowerupShadows = 1;
u8 sUseModelPowerupShadows = 1;

namespace
{
inline char* GetName(int idx)
{
    static char powerupLookup[] = "powerup_generated_xxx";

    if (idx < 10)
    {
        powerupLookup[18] = idx + '0';
        powerupLookup[19] = '\0';
        powerupLookup[20] = '\0';
    }
    else if (idx < 100)
    {
        powerupLookup[18] = (idx / 10) + '0';
        powerupLookup[19] = (idx % 10) + '0';
        powerupLookup[20] = '\0';
    }
    else
    {
        powerupLookup[18] = (idx / 100) + '0';
        powerupLookup[19] = ((idx % 100) / 10) + '0';
        powerupLookup[20] = (idx % 10) + '0';
    }

    return powerupLookup;
}
} // namespace

static void DrawShadow(float radius, float x, float y, float z)
{
    int maxHeight = 10;
    float frac = z / maxHeight;

    if (frac < 0.0f)
    {
        frac = 0.0f;
    }

    if (frac > 1.0f)
    {
        frac = 1.0f;
    }

    float shadowRadius = (float)((1.0f - frac) * (1.75 * radius) + frac * (4.0f * radius));
    int alpha = (int)(150.0f * (1.0f - frac) + 48.0f * frac);

    if (alpha < 0)
    {
        alpha = 0;
    }

    if (alpha > 0xFF)
    {
        alpha = 0xFF;
    }

    ShadowHeight* shadowHeight =
        reinterpret_cast<ShadowHeight*>(BasicStadium::GetCurrentStadium());
    float height = 0.0f;
    if (shadowHeight != 0)
    {
        height = shadowHeight->height;
    }
    height = 0.015625f + height;

    nlVector3 position;
    position.x = x;
    position.y = y;
    position.z = height;

    float minX;
    float minY = position.y - shadowRadius;
    minX = position.x - shadowRadius;
    float maxY = position.y + shadowRadius;
    float maxX = position.x + shadowRadius;

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

    quad.m_uv[0].x = 1.0f;
    quad.m_uv[0].y = 1.0f;
    quad.m_uv[1].x = 0.0f;
    quad.m_uv[1].y = 1.0f;
    quad.m_uv[2].x = 0.0f;
    quad.m_uv[2].y = 0.0f;
    quad.m_uv[3].x = 1.0f;
    quad.m_uv[3].y = 0.0f;

    u8 colour[4];
    colour[0] = 0xFF;
    colour[1] = 0xFF;
    colour[2] = 0xFF;
    colour[3] = (u8)alpha;

    u32 packed = *(u32*)colour;
    *(u32*)&quad.m_colour[3] = packed;
    *(u32*)&quad.m_colour[2] = packed;
    *(u32*)&quad.m_colour[1] = packed;
    *(u32*)&quad.m_colour[0] = packed;

    glSetDefaultState(true);
    glSetRasterState((eGLState)5, 1);
    glSetRasterState((eGLState)6, 0);
    glSetRasterState((eGLState)1, 0);
    glSetCurrentRasterState(glHandleizeRasterState());

    glSetCurrentTexture(glGetTexture("global/ball_shadow"), (eGLTextureType)0);
    glSetTextureState((eGLTextureState)0, 3);
    glSetCurrentTextureState(glHandleizeTextureState());

    quad.Attach((eGLView)fn_8027262C(), 0);
}

void DrawablePowerup::Grab(int idx)
{
    PowerupBase* powerup = FindPowerUp(nlStringLowerHash(GetName(idx)));
    if (powerup != 0)
    {
        mType = powerup->m_eType;
        mVisible = powerup->m_unk20;
        mRadius = powerup->GetRadius();
        mOrientation = powerup->m_aOrientation;
        mPosition = powerup->m_v3Position;
        mScale = powerup->m_scale;
    }
    else
    {
        mVisible = false;
    }
}

void DrawablePowerup::Render(int idx) const
{
    FindPowerUp(nlStringLowerHash(GetName(idx)));
    RenderObject* object = fn_8027725C(nlStringLowerHash(GetName(idx)));

    if (object == 0)
    {
        return;
    }

    nlQuaternion orientation;
    float angle = 0.0000958738f * (float)mOrientation;
    fn_802B549C(&orientation, (u16)(int)(10430.378f * angle));

    if (mVisible)
    {
        object->flags |= 1;
    }
    else
    {
        object->flags &= ~1;
    }

    nlMatrix4 transform;
    nlQuatToMatrix(transform, orientation, true);
    transform.e2[3][0] = mPosition.x;
    transform.e2[3][1] = mPosition.y;
    transform.e2[3][2] = mPosition.z;
    transform.e2[3][3] = 1.0f;

    nlMatrix4 scale;
    nlMakeScaleMatrix(scale, mScale, mScale, mScale);

    nlMatrix4 world;
    nlMultMatrices(world, scale, transform);
    object->SetTransform(&world);

    if (!mVisible)
    {
        return;
    }

    object->Draw();
    if (sDrawPowerupShadows == 0)
    {
        return;
    }

    if (sUseModelPowerupShadows != 0)
    {
        glModel* model = object->model;
        void* source = object->GetShadowSource();
        glModel* geometry = glModelDupNoStreams(model, false, 0);
        fn_801869AC(geometry, source, 1, 0, this, 0.5f);
    }
    else
    {
        DrawShadow(mRadius, mPosition.x, mPosition.y, mPosition.z);
    }
}

void DrawablePowerup::Blend(const float* blendFactors, const DrawablePowerup& lhs, const DrawablePowerup& rhs)
{
    mVisible = lhs.mVisible && rhs.mVisible;
    if (!mVisible)
    {
        return;
    }

    float factor = blendFactors[2];
    mType = lhs.mType;
    mScale = (1.0f - factor) * lhs.mScale + factor * rhs.mScale;
    mRadius = lhs.mRadius;
    int orientationDelta = (int)(factor * (s16)(rhs.mOrientation - lhs.mOrientation));
    mOrientation = lhs.mOrientation + (s16)orientationDelta;
    nlVecLerp(mPosition, lhs.mPosition, rhs.mPosition, factor);
}

template struct UnidentifiedStaticStorage<UnidentifiedStaticTag>;
