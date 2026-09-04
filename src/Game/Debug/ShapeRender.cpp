#include "Game/Debug/ShapeRender.h"

#include "unclassified/tu_802B7798.h"

#include "Game/UnidentifiedStaticStorage.h"

#include "NL/gl/glDraw2.h"
#include "NL/gl/glMemory.h"
#include "NL/gl/glState.h"
#include "NL/gl/glView.h"

ShapeRender g_ShapeRenderer;
static unsigned char g_bWire;
const u32 WhiteTexture = glGetTexture("global/white");

extern "C" void fn_802BC83C(const ShapeRender*, const PrimitiveShape&,
    const nlMatrix4&, bool, const nlColour&);
extern "C" void fn_802BD2C8(PrimitiveShape*, int, void*);
extern "C" void fn_802C8280(const char* name);
extern "C" void fn_802C8288();

void ShapeRender::CreateBoxGeometry(PrimitiveShape& prim)
{
    static int ind_vert[24] = {
        0,
        2,
        3,
        1,
        4,
        5,
        7,
        6,
        0,
        1,
        5,
        4,
        1,
        3,
        7,
        5,
        3,
        2,
        6,
        7,
        2,
        0,
        4,
        6,
    };
    static int ind_uv[24] = {
        1,
        3,
        2,
        0,
        0,
        1,
        3,
        2,
        0,
        1,
        3,
        2,
        0,
        1,
        3,
        2,
        0,
        1,
        3,
        2,
        0,
        1,
        3,
        2,
    };
    static nlVector3 data_vert[8] = {
        { -0.5f, -0.5f, -0.5f },
        { 0.5f, -0.5f, -0.5f },
        { -0.5f, 0.5f, -0.5f },
        { 0.5f, 0.5f, -0.5f },
        { -0.5f, -0.5f, 0.5f },
        { 0.5f, -0.5f, 0.5f },
        { -0.5f, 0.5f, 0.5f },
        { 0.5f, 0.5f, 0.5f },
    };
    static nlVector2 data_uv[4] = {
        { 0.0f, 0.0f },
        { 1.0f, 0.0f },
        { 0.0f, 1.0f },
        { 1.0f, 1.0f },
    };
    static nlVector3 data_norm[24] = {
        { 0.0f, 0.0f, -1.0f },
        { 0.0f, 0.0f, -1.0f },
        { 0.0f, 0.0f, -1.0f },
        { 0.0f, 0.0f, -1.0f },

        { 0.0f, 0.0f, 1.0f },
        { 0.0f, 0.0f, 1.0f },
        { 0.0f, 0.0f, 1.0f },
        { 0.0f, 0.0f, 1.0f },

        { 0.0f, -1.0f, 0.0f },
        { 0.0f, -1.0f, 0.0f },
        { 0.0f, -1.0f, 0.0f },
        { 0.0f, -1.0f, 0.0f },

        { 1.0f, 0.0f, 0.0f },
        { 1.0f, 0.0f, 0.0f },
        { 1.0f, 0.0f, 0.0f },
        { 1.0f, 0.0f, 0.0f },

        { 0.0f, 1.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f },
        { 0.0f, 1.0f, 0.0f },

        { -1.0f, 0.0f, 0.0f },
        { -1.0f, 0.0f, 0.0f },
        { -1.0f, 0.0f, 0.0f },
        { -1.0f, 0.0f, 0.0f },
    };
    static int tri_map[6] = { 0, 1, 2, 3, 0, 2 };

    prim.position = (nlVector3*)glResourceAlloc(
        36 * sizeof(nlVector3), GLM_VertexData, m_Unknown00);
    prim.normal = (nlVector3*)glResourceAlloc(
        36 * sizeof(nlVector3), GLM_VertexData, m_Unknown00);
    prim.texcoord = (nlVector2*)glResourceAlloc(
        36 * sizeof(nlVector2), GLM_VertexData, m_Unknown00);
    prim.vertCount = 36;

    int i;
    int iQuad;
    nlVector3* pdst = prim.position;
    nlVector3* ndst = prim.normal;
    nlVector2* tdst = prim.texcoord;
    nlVector3* psrc[4];
    nlVector3* nsrc[4];
    nlVector2* tsrc[4];

    for (iQuad = 0; iQuad < 6; iQuad++)
    {
        for (i = 0; i < 4; i++)
        {
            psrc[i] = &data_vert[ind_vert[iQuad * 4 + i]];
            nsrc[i] = &data_norm[iQuad * 4 + i];
            tsrc[i] = &data_uv[ind_uv[iQuad * 4 + i]];
        }

        for (i = 0; i < 6; i += 2)
        {
            *pdst = *psrc[tri_map[i]];
            *ndst = *nsrc[tri_map[i]];
            *tdst = *tsrc[tri_map[i]];

            pdst[1] = *psrc[tri_map[i + 1]];
            ndst[1] = *nsrc[tri_map[i + 1]];
            tdst[1] = *tsrc[tri_map[i + 1]];

            pdst += 2;
            ndst += 2;
            tdst += 2;
        }
    }
}

void ShapeRender::CreateHemisphereGeometry(PrimitiveShape& prim)
{
    nlVector3 vNormal;
    nlVector3* pdst;
    nlVector3* ndst;
    nlVector2* tdst;
    int nRing;
    int angle0;
    int angle1;
    int angle;
    int angle90;
    float ring0;
    float ring1;
    float z0;
    float z1;
    int nSegment;
    float x0;
    float y0;
    float x1;
    float y1;
    float lengthSquared;
    float invLen;

    prim.vertCount = 150;
    prim.position = (nlVector3*)glResourceAlloc(
        150 * sizeof(nlVector3), GLM_VertexData, m_Unknown00);
    prim.normal = (nlVector3*)glResourceAlloc(
        150 * sizeof(nlVector3), GLM_VertexData, m_Unknown00);
    prim.texcoord = (nlVector2*)glResourceAlloc(
        150 * sizeof(nlVector2), GLM_VertexData, m_Unknown00);

    pdst = prim.position;
    ndst = prim.normal;
    tdst = prim.texcoord;

    for (nRing = 0; nRing < 5; nRing++)
    {
        float fAngle;

        fAngle = (float)nRing;
        fAngle *= 0.31415927f;
        angle0 = (int)(fAngle * 10430.378f);
        z0 = 0.5f * nlSin((u16)angle0);

        fAngle = (float)(nRing + 1);
        fAngle *= 0.31415927f;
        angle1 = (int)(fAngle * 10430.378f);
        z1 = 0.5f * nlSin((u16)angle1);

        ring0 = nlSin((u16)((u16)angle0 + 0x4000));
        ring1 = nlSin((u16)((u16)angle1 + 0x4000));

        for (nSegment = 0; nSegment < 15; nSegment++)
        {
            float fSegmentAngle;

            fSegmentAngle = (float)nSegment;
            angle =
                (int)((fSegmentAngle *= 0.44879895f) * 10430.378f);

            x0 = 0.5f * (ring0 * nlSin((u16)angle));

            angle90 = (u16)angle + 0x4000;
            y0 = 0.5f * (ring0 * nlSin((u16)angle90));

            x1 = 0.5f * (ring1 * nlSin((u16)angle));
            y1 = 0.5f * (ring1 * nlSin((u16)angle90));

            vNormal.x = x0;
            vNormal.y = y0;
            vNormal.z = z0;

            lengthSquared = vNormal.GetLengthSq3D();
            invLen = nlRecipSqrt(lengthSquared, true);

            pdst->x = x0;
            nlVec3Scale(vNormal, invLen);
            pdst->y = y0;
            pdst->z = z0;
            *ndst = vNormal;

            tdst->x = (float)nSegment / 14.0f;
            tdst->y = (float)nRing / 5.0f;

            vNormal.x = x1;
            vNormal.y = y1;
            vNormal.z = z1;

            lengthSquared = vNormal.GetLengthSq3D();
            invLen = nlRecipSqrt(lengthSquared, true);

            pdst[1].x = x1;
            nlVec3Scale(vNormal, invLen);
            pdst[1].y = y1;
            pdst[1].z = z1;
            ndst[1] = vNormal;

            tdst[1].x = (float)nSegment / 14.0f;
            tdst[1].y = (float)(nRing + 1) / 5.0f;

            pdst += 2;
            ndst += 2;
            tdst += 2;
        }
    }
}

void ShapeRender::CreateFlatCylinderEndGeometry(PrimitiveShape& prim)
{
    nlVector3 vNormal;
    int angle;
    int angle90;
    nlVector3* pdst;
    nlVector3* ndst;
    nlVector2* tdst;
    int nSegment;
    float z0;
    float angleFactor;
    float segmentFactor;
    float texDenom;
    float x0;
    float y0;
    float x1;
    float y1;
    float sinAngle;
    float invLen;

    prim.vertCount = 0x20;
    prim.position = (nlVector3*)glResourceAlloc(
        0x180, GLM_VertexData, m_Unknown00);
    prim.normal = (nlVector3*)glResourceAlloc(
        0x180, GLM_VertexData, m_Unknown00);
    prim.texcoord = (nlVector2*)glResourceAlloc(
        0x100, GLM_VertexData, m_Unknown00);

    float half = 0.5f;
    float one = 1.0f;
    z0 = 0.0f;
    angleFactor = 10430.378f;
    segmentFactor = 0.41887903f;
    texDenom = 15.0f;

    pdst = prim.position;
    ndst = prim.normal;
    tdst = prim.texcoord;

    for (nSegment = 0; nSegment < 0x10; nSegment++)
    {
        angle = (int)(angleFactor * ((float)nSegment * segmentFactor));

        sinAngle = nlSin((u16)angle);
        x0 = half * (one * sinAngle);

        angle90 = (u16)angle + 0x4000;
        y0 = half * (one * nlSin((u16)angle90));

        x1 = half * (z0 * nlSin((u16)angle));
        y1 = half * (z0 * nlSin((u16)angle90));

        vNormal.x = x0;
        vNormal.y = y0;
        vNormal.z = z0;

        invLen = nlRecipSqrt(vNormal.GetLengthSq3D(), true);

        pdst->x = x0;
        nlVec3Scale(vNormal, invLen);
        pdst->y = y0;
        pdst->z = z0;
        *ndst = vNormal;

        tdst->x = (float)nSegment / texDenom;
        tdst->y = z0;

        vNormal.x = x1;
        vNormal.y = y1;
        vNormal.z = z0;

        invLen = nlRecipSqrt(vNormal.GetLengthSq3D(), true);

        pdst[1].x = x1;
        pdst[1].y = y1;
        pdst[1].z = z0;
        nlVec3Scale(vNormal, invLen);
        ndst[1] = vNormal;

        tdst[1].x = (float)nSegment / texDenom;
        tdst[1].y = one;

        pdst += 2;
        ndst += 2;
        tdst += 2;
    }
}

void ShapeRender::CreateCylinderGeometry(PrimitiveShape& prim)
{
    nlVector3 vNormal;
    nlVector3* pdst;
    nlVector3* ndst;
    nlVector2* tdst;
    int nRing;
    int angle;
    int angle90;
    float z0;
    float z1;
    float x0;
    float y0;
    float x1;
    float y1;
    float invLen;

    prim.vertCount = 0x40;
    prim.position = (nlVector3*)glResourceAlloc(
        0x300, GLM_VertexData, m_Unknown00);
    prim.normal = (nlVector3*)glResourceAlloc(
        0x300, GLM_VertexData, m_Unknown00);
    prim.texcoord = (nlVector2*)glResourceAlloc(
        0x200, GLM_VertexData, m_Unknown00);

    pdst = prim.position;
    ndst = prim.normal;
    tdst = prim.texcoord;

    for (nRing = 0; nRing < 2; nRing++)
    {
        float ringScale = 0.5f;

        z0 = -0.5f + (float)nRing * ringScale;
        z1 = -0.5f + (float)(nRing + 1) * ringScale;

        nlSin((u16)((u16)(int)(10430.378f *
                               ((float)nRing * ringScale))
            + 0x4000));
        nlSin((u16)((u16)(int)(10430.378f *
                               ((float)(nRing + 1) * ringScale))
            + 0x4000));

        for (int nSegment = 0; nSegment < 0x10; nSegment++)
        {
            float fSegmentAngle;

            fSegmentAngle = (float)nSegment;
            angle =
                (int)((fSegmentAngle *= 0.41887903f) * 10430.378f);

            x0 = 0.5f * nlSin((u16)angle);

            angle90 = (u16)angle + 0x4000;
            y0 = 0.5f * nlSin((u16)angle90);

            x1 = 0.5f * nlSin((u16)angle);
            y1 = 0.5f * nlSin((u16)angle90);

            vNormal.x = x0;
            vNormal.y = y0;
            vNormal.z = z0;

            invLen = nlRecipSqrt(nlVec3LengthSquared(vNormal), true);

            pdst->x = x0;
            nlVec3Scale(vNormal, invLen);
            pdst->y = y0;
            pdst->z = z0;
            *ndst = vNormal;

            tdst->x = (float)nSegment / 15.0f;
            tdst->y = (float)nRing / 2.0f;

            vNormal.x = x1;
            vNormal.y = y1;
            vNormal.z = z1;

            invLen = nlRecipSqrt(nlVec3LengthSquared(vNormal), true);

            pdst[1].x = x1;
            nlVec3Scale(vNormal, invLen);
            pdst[1].y = y1;
            pdst[1].z = z1;
            ndst[1] = vNormal;

            tdst[1].x = (float)nSegment / 15.0f;
            tdst[1].y = (float)(nRing + 1) / 2.0f;

            pdst += 2;
            ndst += 2;
            tdst += 2;
        }
    }
}

void ShapeRender::DrawSpherePrimitive(const nlMatrix4& mat_world,
    float radius, const nlColour& colour) const
{
    nlMatrix4 mat_hemiTop;
    nlMatrix4 mat_hemiBottom;
    nlMatrix4 mat_rot;

    radius = radius / 0.5f;

    nlMakeScaleMatrix(mat_hemiTop, radius, radius, radius);
    nlMakeRotationMatrixX(mat_rot, 3.1415927f);
    nlMultMatrices(mat_hemiBottom, mat_hemiTop, mat_rot);
    nlMultMatrices(mat_hemiTop, mat_hemiTop, mat_world);
    nlMultMatrices(mat_hemiBottom, mat_hemiBottom, mat_world);

    fn_802BC83C(this, m_Hemisphere, mat_hemiTop, true, colour);
    fn_802BC83C(this, m_Hemisphere, mat_hemiBottom, true, colour);
}

void ShapeRender::DrawRectangle2D(float x, float y, float w, float h,
    float z, const nlColour& colour, int view) const
{
    glSetDefaultState(false);
    glSetRasterState(GLS_AlphaBlend, 1);
    glSetRasterState(GLS_AlphaTest, 1);
    glSetRasterState(GLS_AlphaTestRef, 0);
    glSetCurrentRasterState(glHandleizeRasterState());
    glSetCurrentTexture(glGetTexture("global/white"), GLTT_Diffuse);

    float right = y + h;
    float bottom = x + w;

    glPoly2 poly;
    poly.m_pos[0].x = x;
    poly.m_pos[0].y = y;

    glPoly2* pPoly = &poly;

    poly.m_pos[1].x = x;
    poly.m_pos[1].y = right;
    poly.m_pos[2].x = bottom;
    poly.m_pos[2].y = right;
    poly.m_pos[3].x = bottom;
    poly.m_pos[3].y = y;

    poly.m_colour[3] = colour;
    poly.m_colour[2] = poly.m_colour[3];
    poly.m_colour[1] = poly.m_colour[3];
    poly.m_colour[0] = poly.m_colour[3];

    poly.depth = z;

    GLView* v;
    if (view == -1)
    {
        v = GetDebugFontView();
    }
    else
    {
        v = m_eView;
    }
    pPoly->Attach(v, 0, 0);
}

void ShapeRender::Initialize(void* resource)
{
    if (!m_Initialized)
    {
        m_Unknown00 = resource;
        m_Initialized = true;
        fn_802C8280("ShapeRender");
        CreateBoxGeometry(m_Box);
        CreateCylinderGeometry(m_Cylinder);
        CreateHemisphereGeometry(m_Hemisphere);
        CreateFlatCylinderEndGeometry(m_FlatCylinderEnd);
        fn_802BD2C8(&m_Box, GLP_TriStrip, m_Unknown00);
        fn_802BD2C8(&m_Cylinder, GLP_TriStrip, m_Unknown00);
        fn_802BD2C8(&m_Hemisphere, GLP_TriStrip, m_Unknown00);
        fn_802BD2C8(&m_FlatCylinderEnd, GLP_TriStrip, m_Unknown00);
        m_pLightUserData = 0;
        fn_802C8288();
        m_eView = 0;
    }
}

template struct UnidentifiedStaticStorage<UnidentifiedStaticTag>;
