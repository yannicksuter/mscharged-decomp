#include "Game/Render/depthoffield.h"

#include <string.h>
#include <revolution/gx/GXTransform_fwd.h>
#include <revolution/mtx_fwd.h>

#include "Game/SharedStaticStorage.h"
#include "Game/Render/RLView.h"

#include "NL/gl/glDraw2.h"
#include "NL/gl/glState.h"
#include "NL/gl/glView.h"
#include "NL/nlColour.h"
#include "NL/nlMath.h"

u8 gDepthOfFieldEnabled = true;

DepthOfFieldManager::DepthOfFieldManager()
    : m_bOn(false)
    , m_bDebugView(false)
    , m_fDistanceFromCamera(16.0f)
    , m_fIntensity(1.0f)
    , m_pad0C(0)
{
}

DepthOfFieldManager DepthOfFieldManager::instance;
static const u32 DOFTexture = glGetTexture("target/dof");

void DepthOfFieldManager::Initialize()
{
}

void DepthOfFieldManager::TurnOn()
{
    m_bOn = true;
}

void DepthOfFieldManager::TurnOff()
{
    m_bOn = false;
}

void DepthOfFieldManager::Update()
{
    nlColour colour;
    float screenZ;
    float screenY;
    float screenX;
    nlVector3 position;

    GLView* view = GetLayerView(eCLV_UnsortedPerspective);
    view->m_Target = 8;

    if (!m_bOn)
    {
        return;
    }

    if (!gDepthOfFieldEnabled)
    {
        return;
    }

    float value = m_fDistanceFromCamera;
    if (nlAbs(value) < 0.001f)
    {
        value = 0.001f;
    }

    position.x = 0.0f;
    position.y = 0.0f;
    position.z = -value;

    float viewport[6] = { 0.0f, 0.0f, 640.0f, 448.0f, 0.0f, 1.0f };
    float projection[7];
    float savedProjection[7];
    nlMatrix4 projectionMatrix;
    float identity[3][4];
    glPoly2 poly;

    GXGetProjectionv(savedProjection);

    memcpy(&projectionMatrix,
        GetLayerView(eCLV_Shadowed)->m_Interface->GetProjectionMatrix(),
        sizeof(projectionMatrix));
    GXSetProjection(projectionMatrix.e2, GX_PERSPECTIVE);

    GXGetProjectionv(projection);
    GXGetViewportv(viewport);
    PSMTXIdentity(identity);
    GXProject(position.x, position.y, position.z, identity, projection, viewport, &screenX, &screenY, &screenZ);

    float depth = -screenZ * 16777215.0f;
    GXSetProjectionv(savedProjection);

    int alpha = (int)(255.5f * m_fIntensity);
    if (alpha < 0)
    {
        alpha = 0;
    }
    if (alpha > 255)
    {
        alpha = 255;
    }

    glSetDefaultState(false);
    glSetTextureState(GLTS_DiffuseWrap, 3);

    if (m_bDebugView)
    {
        unsigned long texture = glGetTexture("global/white");
        glSetCurrentTexture(texture, GLTT_Diffuse);
        glSetRasterState(GLS_AlphaBlend, 1);
        colour.c[0] = 0xFF;
        colour.c[1] = 0xC8;
        colour.c[2] = 0xC8;
        colour.c[3] = 0x80;
    }
    else
    {
        glSetCurrentTexture(DOFTexture, GLTT_Diffuse);
        colour.c[0] = 0xFF;
        colour.c[1] = 0xFF;
        colour.c[2] = 0xFF;
        colour.c[3] = (unsigned char)alpha;
        if (alpha != 0xFF)
        {
            glSetRasterState(GLS_AlphaBlend, 1);
        }
    }

    glSetRasterState(GLS_DepthTest, 1);
    glSetCurrentRasterState(glHandleizeRasterState());
    glSetCurrentTextureState(glHandleizeTextureState());

    poly.SetupRectangle(0.0f, 0.0f, 640.0f, 480.0f, depth);
    poly.SetColour(colour);
    poly.Attach(GetLayerView(eCLV_DepthOfField), 0, 0);
}
