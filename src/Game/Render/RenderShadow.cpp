#include "Game/Render/RenderShadow.h"

#include "Game/BasicStadium.h"
#include "Game/Debug/ShapeRender.h"
#include "Game/Drawable/DrawableModel.h"
#include "Game/Drawable/DrawableObj.h"
#include "Game/Render/RLView.h"
#include "Game/Render/Frustum.h"
#include "NL/gl/gl.h"
#include "NL/gl/glDraw3.h"
#include "NL/gl/glMatrix.h"
#include "NL/gl/glState.h"

struct BallShadowParams
{
    /* 0x00 */ float fReferenceHeight;
    /* 0x04 */ float fRadius0;
    /* 0x08 */ float fRadius1;
    /* 0x0C */ int nAlpha0;
    /* 0x10 */ int nAlpha1;
    /* 0x14 */ nlColour colour;
};

extern "C" {
extern float lbl_806DCCA4;
extern float lbl_806DCCA8;
extern float lbl_806DCCAC;

RLView* fn_8027261C();
void fn_80273A4C(eCLV, const glModel*, unsigned long);
}


int GetShadowPartitionCount();
void SetShadowPartitionEnabled(int partition, bool enabled);
GLView* GetShadowPartitionView(int partition);
u32 GetShadowPartitionTexture(int partition);
void SetShadowPartitionCamera(int partition, const nlMatrix4& view,
    const nlMatrix4& projection);
extern "C" void fn_80184C3C(
    GLView* pView, const ProjectedShadowParams& params);

static float g_fBallShadowH = 4.0f;
static float g_fBallShadowR0 = 0.35f;
static float g_fBallShadowR1 = 0.65f;
static int g_nBallShadowA0 = 10;
static int g_nBallShadowA1 = 50;
static bool g_bBallGlow = true;
static bool g_bPlanarShadows = true;
static bool g_bProjectedShadows = true;
static float g_fBallGlowH = 4.0f;
static float g_fBallGlowR0 = 2.0f;
static float g_fBallGlowR1 = 2.0f;
static int g_nBallGlowA0 = 165;
static int g_nBallGlowA1 = 10;
static float sfPlanarShadowOpacity = 0.3f;
static float sfCoPlanarZ = 0.1f;
int MaxProjectedShadows;
static u8 g_bShadowBlobs;
static u8 g_bShadowPositionOverride;
static RLView* g_CharacterShadowView;
static const unsigned long CoPlanarTexture = glGetTexture("global/black_coplanar");
static const unsigned long LightRampTexture = glGetTexture("global/lightramp");
static const unsigned long BlackTexture = glGetTexture("global/black");
const unsigned long WhiteTexture = glGetTexture("global/white");
static float g_AntiFlimmer = 0.015625f
    + (BasicStadium::GetCurrentStadium() != 0
              ? BasicStadium::GetCurrentStadium()->m_shadowHeight
              : 0.0f);
static int lbl_806E1480;
static u8 g_bShadowBounds;
static bool g_bShadowBoundingBox;
static bool g_bCoPlanarBlend;
static bool g_bSkipUntransformed;
static int g_Alpha[3] = { 180, 80, 32 };

static inline void CastDirectional(nlVector3& p, const nlVector3& lightPos)
{
    nlVector3 V;
    nlVector3 Q;

    V.x = 0.0f;
    V.y = 0.0f;
    V.z = 1.0f;
    nlVec3Set(Q, p.x, p.y, p.z);
    nlVector3 L = lightPos;

    nlVec3Scale(L, nlRecipSqrt(L.GetLengthSq3D(), false));

    float num = nlVec3DotProduct(V, Q);
    float den = nlVec3DotProduct(V, L);
    float t = -(num / den);

    p.z = Q.z + t * L.z;
    p.x = Q.x + t * L.x;
    p.y = Q.y + t * L.y;
}

static void DrawBallShadow(
    const nlVector3& vPosition, const BallShadowParams& p, bool bGlow);

void SetPlanarShadowOpacity(float opacity)
{
    sfPlanarShadowOpacity = opacity;
}

float GetPlanarShadowOpacity()
{
    return sfPlanarShadowOpacity;
}

extern "C" void fn_80186354(DrawableObject* arg0)
{
    if (arg0->GetWorldMatrix().m43 >= 0.0f)
    {
        BallShadowParams p;
        p.fReferenceHeight = g_fBallShadowH;
        p.fRadius0 = g_fBallShadowR0;
        p.fRadius1 = g_fBallShadowR1;
        p.nAlpha0 = g_nBallShadowA0;
        p.nAlpha1 = g_nBallShadowA1;
        p.colour.c[0] = 0xFF;
        p.colour.c[1] = 0xFF;
        p.colour.c[2] = 0xFF;
        p.colour.c[3] = 0xFF;
        DrawBallShadow(
            *(const nlVector3*)&arg0->GetWorldMatrix().e2[3][0], p, false);

        if (g_bBallGlow)
        {
            p.fReferenceHeight = g_fBallGlowH;
            p.fRadius0 = g_fBallGlowR0;
            p.fRadius1 = g_fBallGlowR1;
            p.nAlpha0 = g_nBallGlowA0;
            p.nAlpha1 = g_nBallGlowA1;

            int red;
            int blue;
            int green;
            float scale = arg0->snapshotScale;
            if (scale < 1.0f)
            {
                red = 180;
                blue = 200;
                green = 15;
            }
            else if (scale >= 1.0f && scale < 2.0f)
            {
                red = 200;
                blue = 25;
                green = 25;
            }
            else if (scale >= 2.0f && scale < 3.0f)
            {
                red = 200;
                blue = 15;
                green = 100;
            }
            else if (scale >= 3.0f && scale < 4.0f)
            {
                red = 200;
                blue = 10;
                green = 200;
            }
            else
            {
                red = 200;
                blue = 200;
                green = 200;
            }
            p.colour.c[0] = red;
            p.colour.c[1] = green;
            p.colour.c[2] = blue;
            p.colour.c[3] = 0xFF;
            DrawBallShadow(
                *(const nlVector3*)&arg0->GetWorldMatrix().e2[3][0], p, true);
        }
    }
}

static void DrawBallShadow(
    const nlVector3& vPosition, const BallShadowParams& p, bool bGlow)
{
    f32 frac = vPosition.z / p.fReferenceHeight;
    if (frac < 0.0f)
    {
        frac = 0.0f;
    }
    if (frac > 1.0f)
    {
        frac = 1.0f;
    }

    f32 fX0, fY0, fY1, fX1;
    f32 half_dim = (1.0f - frac) * p.fRadius0 + frac * p.fRadius1;
    f32 fAlpha
        = (1.0f - frac) * (f32)p.nAlpha0 + frac * (f32)p.nAlpha1;
    s32 alpha = (s32)fAlpha;
    if (alpha < 0)
    {
        alpha = 0;
    }
    if (alpha > 0xFF)
    {
        alpha = 0xFF;
    }

    BasicStadium* stadium = BasicStadium::GetCurrentStadium();
    float height = 0.0f;
    if (stadium != 0)
    {
        height = stadium->m_shadowHeight;
    }

    nlVector3 position;
    nlVec3Set(position, vPosition.x, vPosition.y, 0.015625f + height);

    nlColour c = p.colour;
    fY0 = position.y - half_dim;
    fX0 = position.x - half_dim;

    glQuad3 quad;
    c.c[3] = (u8)alpha;

    fY1 = position.y + half_dim;
    fX1 = position.x + half_dim;

    quad.m_pos[0].x = fX0;
    quad.m_pos[0].y = fY0;
    quad.m_pos[0].z = position.z;
    quad.m_pos[1].x = fX0;
    quad.m_pos[1].y = fY1;
    quad.m_pos[1].z = position.z;
    quad.m_pos[2].x = fX1;
    quad.m_pos[2].y = fY1;
    quad.m_pos[2].z = position.z;
    quad.m_pos[3].x = fX1;
    quad.m_pos[3].y = fY0;
    quad.m_pos[3].z = position.z;

    quad.m_uv[0].x = 1.0f;
    quad.m_uv[0].y = 1.0f;
    quad.m_uv[1].x = 0.0f;
    quad.m_uv[1].y = 1.0f;
    quad.m_uv[2].x = 0.0f;
    quad.m_uv[2].y = 0.0f;
    quad.m_uv[3].x = 1.0f;
    quad.m_uv[3].y = 0.0f;

    quad.m_colour[3] = c;
    quad.m_colour[2] = c;
    quad.m_colour[1] = c;
    quad.m_colour[0] = c;

    glSetDefaultState(true);
    glSetRasterState(GLS_AlphaBlend, bGlow ? 3 : 1);
    glSetRasterState(GLS_AlphaTest, 1);
    glSetRasterState(GLS_Culling, 0);
    glSetRasterState(GLS_DepthWrite, 0);
    glSetCurrentRasterState(glHandleizeRasterState());

    glSetCurrentTexture(glGetTexture(bGlow ? "global/light_blob"
                                        : "global/ball_shadow"),
        GLTT_Diffuse);
    glSetTextureState(GLTS_DiffuseWrap, 3);
    glSetCurrentTextureState(glHandleizeTextureState());

    const glModel* pModel = quad.GetModel();
    fn_80273A4C(eCLV_Particles, pModel, 0);
}

void ClearCharacterShadowsUpdated()
{
    int shadowIndex;
    for (shadowIndex = 0; shadowIndex < GetShadowPartitionCount(); shadowIndex++)
    {
        SetShadowPartitionEnabled(shadowIndex, false);
    }
}

static void RenderBlobShadow(const nlVector3& vPosition,
    const nlVector3* pPoints, int index, const int* uvOrder,
    const nlColour* pColour);

void RenderProjectedShadow(const ProjectedShadowParams& params)
{
    nlVector3 vDir;
    nlVector3 vTemp;
    nlVector3 p[4];
    nlVector3 dir;
    nlVector3 light;
    nlVector3 vLight;
    float radius;
    nlColour c;
    nlColour colour;
    nlMatrix4 mLight;

    if (g_bShadowBlobs)
    {
        RenderBlobShadow(params.vPosition, 0, -1, 0, 0);
        return;
    }

    radius = params.fRadius;

    if (g_bShadowPositionOverride)
    {
        float z;
        float y;
        float x;
        z = lbl_806DCCAC;
        y = lbl_806DCCA8;
        x = lbl_806DCCA4;
        nlVec3Set(vLight, x, y, z);
    }
    else
    {
        float z;
        float y;
        float x;
        z = params.vLight.z;
        y = params.vLight.y;
        x = params.vLight.x;
        nlVec3Set(vLight, x, y, z);
    }

    {
        nlVec3Set(vDir, -vLight.x, -vLight.y, -vLight.z);
        nlVec3Scale(vDir,
            nlRecipSqrt(vDir.GetLengthSq3D(), false));

        nlVector3 vUp = { 0.0f, 0.0f, 1.0f };
        nlVector3 vRight;

        vTemp = params.vPosition;
        vTemp.z += 0.5f * params.fHeight;

        nlVec3CrossProduct(vRight, vDir, vUp);

        nlVec3Scale(vRight,
            nlRecipSqrt(vRight.GetLengthSq3D(), true));
        nlVec3CrossProduct(vUp, vRight, vDir);
        nlVec3Scale(vUp,
            nlRecipSqrt(vUp.GetLengthSq3D(), true));

        nlVec3ScaleAdd(p[0], radius, vRight, vTemp);
        nlVec3ScaleAdd(p[1], -radius, vRight, vTemp);
        nlVec3ScaleAdd(p[0], -radius, vUp, p[0]);
        nlVec3ScaleAdd(p[1], -radius, vUp, p[1]);
        nlVec3ScaleAdd(p[2], -radius, vRight, vTemp);
        nlVec3ScaleAdd(p[3], radius, vRight, vTemp);
        nlVec3ScaleAdd(p[2], radius, vUp, p[2]);
        nlVec3ScaleAdd(p[3], radius, vUp, p[3]);
    }

    nlColourSet(c, 0x40, 0x40, 0xFF, 0xFF);

    if (g_bShadowBounds)
    {
        nlColour colour = c;
        g_ShapeRenderer.DrawLine3D(p[0], p[1], colour, false);
        g_ShapeRenderer.DrawLine3D(p[1], p[2], colour, false);
        g_ShapeRenderer.DrawLine3D(p[2], p[3], colour, false);
        g_ShapeRenderer.DrawLine3D(p[3], p[0], colour, false);
        g_ShapeRenderer.DrawLine3D(p[0], p[2], colour, false);
        g_ShapeRenderer.DrawLine3D(p[1], p[3], colour, false);
    }

    {
        nlVector3* pPoint = p;
        for (int i = 0; i < 4; i++, pPoint++)
        {
            CastDirectional(*pPoint, vDir);
            pPoint->z = g_AntiFlimmer;
        }
    }

    if (g_bShadowBounds)
    {
        nlColourSet(c, 0x40, 0xFF, 0x40, 0xFF);
        nlColour colour = c;
        g_ShapeRenderer.DrawLine3D(p[0], p[1], colour, false);
        g_ShapeRenderer.DrawLine3D(p[1], p[2], colour, false);
        g_ShapeRenderer.DrawLine3D(p[2], p[3], colour, false);
        g_ShapeRenderer.DrawLine3D(p[3], p[0], colour, false);
        g_ShapeRenderer.DrawLine3D(p[0], p[2], colour, false);
        g_ShapeRenderer.DrawLine3D(p[1], p[3], colour, false);

        dir = vTemp;
        CastDirectional(dir, vDir);
        g_ShapeRenderer.DrawLine3D(vTemp, dir, c, false);
    }

    {
        float newAntiFlimmer = GetCoPlanarZ();
        float oldAntiFlimmer = g_AntiFlimmer;
        g_AntiFlimmer = newAntiFlimmer;

        *(u32*)&colour = 0;
        colour.c[3] = (u8)(g_Alpha[0] * params.fScalar);

        RenderBlobShadow(
            params.vPosition, p, params.nPartitionIndex, 0, &colour);
        g_AntiFlimmer = oldAntiFlimmer;
    }

    if (g_bShadowBounds)
    {
        nlVec3Set(light,
            params.vLight.x, params.vLight.y, params.vLight.z);

        mLight.SetIdentity();
        mLight.m41 = light.x;
        mLight.m42 = light.y;
        mLight.m43 = light.z;
        mLight.m44 = 1.0f;

        c.c[0] = 0xFF;
        c.c[1] = 0xFF;
        c.c[2] = 0x40;
        c.c[3] = 0xFF;
        g_ShapeRenderer.DrawSpherePrimitive(mLight, 0.5f, c);
    }
}

static void RenderBlobShadow(const nlVector3& vPosition,
    const nlVector3* pPoints, int index, const int* uvOrder,
    const nlColour* pColour)
{
    static int alpha = 0x80;
    static float half_w = 0.625f;
    static float half_h = 0.625f;

    glQuad3 quad;
    nlColour c;
    nlColour cfade;
    unsigned long texture;

    nlVector3 sp10;
    nlVec3Set(sp10, vPosition.x, vPosition.y, g_AntiFlimmer);

    if (pColour == 0)
    {
        nlColourSet(c, 0xFF, 0xFF, 0xFF, (u8)alpha);
        cfade = c;
    }
    else
    {
        c = *pColour;
        cfade = c;
        cfade.c[3] = (u8)g_Alpha[2];
    }

    if (pPoints == 0)
    {
        texture = glGetTexture("global/shadeblob");

        nlVec3Set(quad.m_pos[0],
            sp10.x - half_w, sp10.y - half_h, sp10.z);
        nlVec3Set(quad.m_pos[1],
            sp10.x - half_w, sp10.y + half_h, sp10.z);
        nlVec3Set(quad.m_pos[2],
            sp10.x + half_w, sp10.y + half_h, sp10.z);
        nlVec3Set(quad.m_pos[3],
            sp10.x + half_w, sp10.y - half_h, sp10.z);

        quad.m_uv[0].x = 1.0f;
        quad.m_uv[0].y = 1.0f;
        quad.m_uv[1].x = 0.0f;
        quad.m_uv[1].y = 1.0f;
        quad.m_uv[2].x = 0.0f;
        quad.m_uv[2].y = 0.0f;
        quad.m_uv[3].x = 1.0f;
        quad.m_uv[3].y = 0.0f;
    }
    else
    {
        texture = GetShadowPartitionTexture(index);

        quad.m_pos[0] = pPoints[0];
        quad.m_pos[1] = pPoints[1];
        quad.m_pos[2] = pPoints[2];
        quad.m_pos[3] = pPoints[3];

        int idx;
        nlVector2* pUV;

        if (uvOrder == 0)
        {
            idx = 0;
        }
        else
        {
            idx = uvOrder[0];
        }
        pUV = &quad.m_uv[idx];
        pUV->x = 1.0f;
        pUV->y = 1.0f;

        if (uvOrder == 0)
        {
            idx = 1;
        }
        else
        {
            idx = uvOrder[1];
        }
        pUV = &quad.m_uv[idx];
        pUV->x = 0.0f;
        pUV->y = 1.0f;

        if (uvOrder == 0)
        {
            idx = 2;
        }
        else
        {
            idx = uvOrder[2];
        }
        pUV = &quad.m_uv[idx];
        pUV->x = 0.0f;
        pUV->y = 0.0f;

        if (uvOrder == 0)
        {
            idx = 3;
        }
        else
        {
            idx = uvOrder[3];
        }
        pUV = &quad.m_uv[idx];
        pUV->x = 1.0f;
        pUV->y = 0.0f;
    }

    quad.m_colour[1] = c;
    quad.m_colour[0] = c;
    quad.m_colour[3] = cfade;
    quad.m_colour[2] = cfade;

    glSetDefaultState(true);
    glSetRasterState(GLS_AlphaBlend, 1);
    glSetRasterState(GLS_Culling, 0);
    glSetRasterState(GLS_DepthWrite, 0);
    glSetCurrentRasterState(glHandleizeRasterState());
    glSetCurrentTexture(texture, GLTT_Diffuse);
    glSetTextureState(GLTS_DiffuseWrap, 3);
    glSetCurrentTextureState(glHandleizeTextureState());

    quad.Attach((eGLView)g_CharacterShadowView, 0);
}

void RenderCharacterIntoTexture(const ProjectedShadowParams& params)
{
    nlVector3 up = { 0.0f, 0.0f, 1.0f };
    nlVector3 targetPos;
    nlVector3 viewDir;
    nlVector3 eyePos;
    nlVector3 vTemp;
    nlVector3 vDir;
    nlVector3 shadowPos;
    nlVector3 vLight;

    targetPos = params.vPosition;
    targetPos.z += 0.5f * params.fHeight;

    if (g_bShadowPositionOverride)
    {
        float z;
        float y;
        float x;
        z = lbl_806DCCAC;
        y = lbl_806DCCA8;
        x = lbl_806DCCA4;
        nlVec3Set(shadowPos, x, y, z);
    }
    else
    {
        float z;
        float y;
        float x;
        z = params.vLight.z;
        y = params.vLight.y;
        x = params.vLight.x;
        nlVec3Set(shadowPos, x, y, z);
    }

    nlVec3Set(viewDir, -shadowPos.x, -shadowPos.y, -shadowPos.z);
    nlVec3Scale(viewDir, nlRecipSqrt(viewDir.GetLengthSq3D(), false));
    nlVec3Set(viewDir, -viewDir.x, -viewDir.y, -viewDir.z);

    nlVec3ScaleAdd(eyePos, 8.0f, viewDir, targetPos);

    nlMatrix4 view;
    glMatrixLookAt(view, eyePos, targetPos, up);

    nlMatrix4 projection;
    float radius = 2.0f * params.fRadius;
    glMatrixOrthographicCentered(projection, radius, radius, 4.0f, 12.0f);

    SetShadowPartitionCamera(params.nPartitionIndex, view, projection);
    SetShadowPartitionEnabled(params.nPartitionIndex, true);
    fn_80184C3C(GetShadowPartitionView(params.nPartitionIndex), params);
    GetShadowPartitionView(params.nPartitionIndex)->AttachModel(params.pModel, 0);

    if (g_bShadowBounds)
    {
        GLView* unknownView = g_ShapeRenderer.m_eView;
        g_ShapeRenderer.m_eView = GetShadowPartitionView(params.nPartitionIndex);

        vTemp = params.vPosition;
        vTemp.z += 0.5f * params.fHeight;

        if (g_bShadowPositionOverride)
        {
            float z;
            float y;
            float x;
            z = lbl_806DCCAC;
            y = lbl_806DCCA8;
            x = lbl_806DCCA4;
            nlVec3Set(vLight, x, y, z);
        }
        else
        {
            float z;
            float y;
            float x;
            z = params.vLight.z;
            y = params.vLight.y;
            x = params.vLight.x;
            nlVec3Set(vLight, x, y, z);
        }

        nlVec3Set(vDir, -vLight.x, -vLight.y, -vLight.z);
        nlVec3Scale(vDir, nlRecipSqrt(vDir.GetLengthSq3D(), false));

        nlVector3 vUp = { 0.0f, 0.0f, 1.0f };
        nlVector3 vRight;
        nlVec3CrossProduct(vRight, vDir, vUp);
        float radius = params.fRadius;
        nlVec3Scale(vRight,
            nlRecipSqrt(vRight.GetLengthSq3D(), true));
        nlVec3CrossProduct(vUp, vRight, vDir);
        nlVec3Scale(vUp, nlRecipSqrt(vUp.GetLengthSq3D(), true));

        nlVector3 p[4];
        nlVec3ScaleAdd(p[0], radius, vRight, vTemp);
        nlVec3ScaleAdd(p[1], -radius, vRight, vTemp);
        nlVec3ScaleAdd(p[0], -radius, vUp, p[0]);
        nlVec3ScaleAdd(p[1], -radius, vUp, p[1]);
        nlVec3ScaleAdd(p[2], -radius, vRight, vTemp);
        nlVec3ScaleAdd(p[3], radius, vRight, vTemp);
        nlVec3ScaleAdd(p[2], radius, vUp, p[2]);
        nlVec3ScaleAdd(p[3], radius, vUp, p[3]);

        nlColour c = { 0xFF, 0xFF, 0xFF, 0xFF };
        g_ShapeRenderer.DrawLine3D(p[0], p[1], c, false);
        g_ShapeRenderer.DrawLine3D(p[1], p[2], c, false);
        g_ShapeRenderer.DrawLine3D(p[2], p[3], c, false);
        g_ShapeRenderer.DrawLine3D(p[3], p[0], c, false);
        g_ShapeRenderer.DrawLine3D(p[0], p[2], c, false);
        g_ShapeRenderer.DrawLine3D(p[1], p[3], c, false);

        g_ShapeRenderer.m_eView = unknownView;
    }
}

extern "C" void fn_80184C3C(
    GLView* pView, const ProjectedShadowParams& params)
{
    nlVector3 p[4];
    nlVector3 vDir;
    nlVector3 vTemp;
    nlVector3 vLight;
    float radius;

    if (g_bShadowPositionOverride)
    {
        float z;
        float y;
        float x;
        z = lbl_806DCCAC;
        y = lbl_806DCCA8;
        x = lbl_806DCCA4;
        nlVec3Set(vLight, x, y, z);
    }
    else
    {
        float z;
        float y;
        float x;
        z = params.vLight.z;
        y = params.vLight.y;
        x = params.vLight.x;
        nlVec3Set(vLight, x, y, z);
    }

    {
        nlVec3Set(vDir, -vLight.x, -vLight.y, -vLight.z);
        nlVec3Scale(vDir, nlRecipSqrt(vDir.GetLengthSq3D(), false));
        nlVector3 vUp = { 0.0f, 0.0f, 1.0f };
        nlVector3 vRight;

        vTemp = params.vPosition;
        vTemp.z += 0.5f * params.fHeight;
        radius = params.fRadius;

        nlVec3CrossProduct(vRight, vDir, vUp);

        nlVec3Scale(
            vRight, nlRecipSqrt(vRight.GetLengthSq3D(), true));
        nlVec3CrossProduct(vUp, vRight, vDir);
        nlVec3Scale(
            vUp, nlRecipSqrt(vUp.GetLengthSq3D(), true));

        nlVec3ScaleAdd(p[0], radius, vRight, vTemp);
        nlVec3ScaleAdd(p[1], -radius, vRight, vTemp);
        nlVec3ScaleAdd(p[0], -radius, vUp, p[0]);
        nlVec3ScaleAdd(p[1], -radius, vUp, p[1]);
        nlVec3ScaleAdd(p[2], -radius, vRight, vTemp);
        nlVec3ScaleAdd(p[3], radius, vRight, vTemp);
        nlVec3ScaleAdd(p[2], radius, vUp, p[2]);
        nlVec3ScaleAdd(p[3], radius, vUp, p[3]);
    }

    {
        nlVector3* pPoint = p;
        for (int i = 0; i < 4; i++, pPoint++)
        {
            CastDirectional(*pPoint, vDir);
            pPoint->z = g_AntiFlimmer;
        }
    }

    glSetDefaultState(true);
    glSetRasterState(GLS_DepthWrite, 1);
    glSetRasterState(GLS_AlphaBlend, 1);
    glSetRasterState(GLS_Culling, 0);
    glSetCurrentRasterState(glHandleizeRasterState());
    glSetCurrentTexture(WhiteTexture, GLTT_Diffuse);
    glSetTextureState(GLTS_DiffuseWrap, 0);
    glSetCurrentTextureState(glHandleizeTextureState());

    nlColour colour = { 0xFF, 0xFF, 0x00, 0x00 };
    colour.c[3] = (u8)lbl_806E1480;
    glQuad3 quad;
    nlVec2Set(quad.m_uv[0], 0.0f, 0.0f);
    nlVec2Set(quad.m_uv[1], 0.0f, 1.0f);
    nlVec2Set(quad.m_uv[2], 1.0f, 1.0f);
    nlVec2Set(quad.m_uv[3], 1.0f, 0.0f);
    quad.m_pos[0] = p[0];
    quad.m_pos[1] = p[1];
    quad.m_pos[2] = p[2];
    quad.m_pos[3] = p[3];
    quad.SetColour(colour);
    glAttachQuad3((eGLView)pView, 1, &quad);
}

bool ShouldShadowBeUpdated(const ProjectedShadowParams& params)
{
    nlVector3 position = params.vPosition;
    position.z += 0.625f * params.fHeight;

    float radius = 2.0f * params.fRadius;
    RLView* view = fn_8027261C();
    bool visible
        = ClassifySphereInFrustum(view->m_Interface->GetShadowMatrix(), &position, radius);
    unsigned long interval;
    if (visible)
    {
        interval = params.nVisibleInterval;
    }
    else
    {
        interval = params.nInvisibleInterval;
    }

    unsigned long frame
        = params.nPartitionIndex + (unsigned long)glGetCurrentFrame();
    if (frame % interval != 0)
    {
        return 0;
    }
    return 1;
}

extern "C" float fn_80184B08()
{
    float previous = g_AntiFlimmer;
    BasicStadium* stadium = BasicStadium::GetCurrentStadium();
    float height = 0.0f;
    if (stadium != 0)
    {
        height = stadium->m_shadowHeight;
    }
    g_AntiFlimmer = height + 0.015625f;
    return previous;
}

extern "C" float fn_80184AF8(float antiFlimmer)
{
    float previous = g_AntiFlimmer;
    g_AntiFlimmer = antiFlimmer;
    return previous;
}

RLView* SetCharacterShadowView(RLView* view)
{
    RLView* previous = g_CharacterShadowView;
    g_CharacterShadowView = view;
    return previous;
}

extern "C" void fn_80184ADC()
{
    MaxProjectedShadows = 10;
}

void SetCoPlanarZ(float z)
{
    sfCoPlanarZ = z;
}

float GetCoPlanarZ()
{
    return sfCoPlanarZ;
}


/**
 * Address/Size: 0x80186524 | size: 0x12C
 *
 * Flattens a transform onto the ground plane along the stadium's shadow light
 * direction.
 */
extern "C" void fn_80186524(nlMatrix4& out, const nlMatrix4& in)
{
    const nlVector3& light = BasicStadium::GetCurrentStadium()->m_shadowLightPosition;
    float x = -light.x / light.z;
    float y = -light.y / light.z;

    out.m11 = x * in.m13 + in.m11;
    out.m21 = x * in.m23 + in.m21;
    out.m31 = x * in.m33 + in.m31;
    out.m41 = x * in.m43 + in.m41;
    out.m12 = y * in.m13 + in.m12;
    out.m22 = y * in.m23 + in.m22;
    out.m32 = y * in.m33 + in.m32;
    out.m42 = y * in.m43 + in.m42;
    out.m13 = 0.0f;
    out.m23 = 0.0f;
    out.m33 = 0.0f;
    out.m43 = 0.0f;
    out.m14 = 0.0f;
    out.m24 = 0.0f;
    out.m34 = 0.0f;
    out.m44 = 1.0f;
}

/**
 * Address/Size: 0x80186650 | size: 0x1BC
 *
 * Projects the model's bounding box onto the ground plane and returns the
 * screen-space extent the flattened corners span.
 */
extern "C" void fn_80186650(const glModel* model, const nlMatrix4& transform,
    float* minX, float* maxX, float* minY, float* maxY,
    unsigned long boundingBoxCacheKey)
{
    AABBDimensions dimensions;
    GetAABBDimensions(model, dimensions, boundingBoxCacheKey);

    nlVector4 corners[8];
    nlVec4Set(corners[0], dimensions.mMin.x, dimensions.mMin.y, dimensions.mMin.z, 1.0f);
    nlVec4Set(corners[1], dimensions.mMin.x, dimensions.mMin.y, dimensions.mMax.z, 1.0f);
    nlVec4Set(corners[2], dimensions.mMin.x, dimensions.mMax.y, dimensions.mMin.z, 1.0f);
    nlVec4Set(corners[3], dimensions.mMin.x, dimensions.mMax.y, dimensions.mMax.z, 1.0f);
    nlVec4Set(corners[4], dimensions.mMax.x, dimensions.mMin.y, dimensions.mMin.z, 1.0f);
    nlVec4Set(corners[5], dimensions.mMax.x, dimensions.mMin.y, dimensions.mMax.z, 1.0f);
    nlVec4Set(corners[6], dimensions.mMax.x, dimensions.mMax.y, dimensions.mMin.z, 1.0f);
    nlVec4Set(corners[7], dimensions.mMax.x, dimensions.mMax.y, dimensions.mMax.z, 1.0f);

    nlMatrix4 projection;
    fn_80186524(projection, transform);

    for (int i = 0; i < 8; i++)
    {
        nlVector4 projected;
        nlMultVectorMatrix(projected, corners[i], projection);
        corners[i] = projected;

        if (i == 0 || corners[i].x < *minX)
            *minX = corners[i].x;
        if (i == 0 || corners[i].x > *maxX)
            *maxX = corners[i].x;
        if (i == 0 || corners[i].y < *minY)
            *minY = corners[i].y;
        if (i == 0 || corners[i].y > *maxY)
            *maxY = corners[i].y;
    }
}

/**
 * Address/Size: 0x8018680C | size: 0x1A0
 *
 * Draws the flattened bounding box of a model as one white co-planar quad.
 */
extern "C" void fn_8018680C(eCLV layer, const glModel* model,
    const nlMatrix4& transform, unsigned long boundingBoxCacheKey)
{
    float minX;
    float maxX;
    float minY;
    float maxY;
    float z = sfCoPlanarZ;
    fn_80186650(model, transform, &minX, &maxX, &minY, &maxY,
        boundingBoxCacheKey);

    glQuad3 quad;
    quad.m_pos[0].x = maxY;
    quad.m_pos[0].y = maxX;
    quad.m_pos[0].z = z;
    quad.m_pos[1].x = minY;
    quad.m_pos[1].y = maxX;
    quad.m_pos[1].z = z;
    quad.m_pos[2].x = minY;
    quad.m_pos[2].y = minX;
    quad.m_pos[2].z = z;
    quad.m_pos[3].x = maxY;
    quad.m_pos[3].y = minX;
    quad.m_pos[3].z = z;
    if (maxY + minY < 0.0f)
    {
        quad.m_pos[0].x = minY;
        quad.m_pos[1].x = maxY;
        quad.m_pos[2].x = maxY;
        quad.m_pos[3].x = minY;
    }

    glSetDefaultState(false);
    glSetRasterState((eGLState)6, g_bCoPlanarBlend ? 0 : 3);
    glSetRasterState((eGLState)0, 0);
    glSetRasterState((eGLState)1, 0);
    glSetCurrentRasterState(glHandleizeRasterState());
    glSetCurrentTexture(WhiteTexture, (eGLTextureType)0);

    quad.m_uv[0].x = 0.0f;
    quad.m_uv[0].y = 0.0f;
    quad.m_uv[1].x = 0.0f;
    quad.m_uv[1].y = 0.0f;
    quad.m_uv[2].x = 0.0f;
    quad.m_uv[2].y = 0.0f;
    quad.m_uv[3].x = 0.0f;
    quad.m_uv[3].y = 0.0f;
    quad.SetColour(0xAA, 0xAA, 0xAA, 0xFF);
    quad.Attach((eGLView)GetLayerView(layer), 0);
}

/**
 * Address/Size: 0x801869AC | size: 0x2D8
 *
 * Draws one model's ground shadow: the flattened bounding box for the
 * projected pass and the co-planar geometry for the planar pass.
 */
extern "C" void fn_801869AC(const glModel* model, const nlMatrix4& transform,
    int hasTransform, unsigned long boundingBoxCacheKey, const void* owner,
    float opacity)
{
    if (g_bShadowBoundingBox)
        RenderBoundingBox(model, transform);

    if (!g_bPlanarShadows)
        return;

    if (g_bProjectedShadows)
    {
        if (g_bSkipUntransformed && transform.m41 == 0.0f
            && transform.m42 == 0.0f && transform.m43 == 0.0f)
        {
            return;
        }

        nlMatrix4 projected;
        if (hasTransform != 0)
        {
            projected = transform;
        }
        else
        {
            nlMatrix4 modelMatrix;
            glModelGetMatrix(model, modelMatrix);
            nlMultMatrices(projected, transform, modelMatrix);
        }
        fn_8018680C((eCLV)0x1B, model, projected, boundingBoxCacheKey);
    }

    nlMatrix4 modelMatrix;
    glModelGetMatrix(model, modelMatrix);
}
