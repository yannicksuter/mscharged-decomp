#include "Game/Render/depthoffield.h"

#include "NL/gl/glDraw2.h"
#include "NL/gl/glState.h"
#include "NL/gl/glView.h"
#include "NL/nlColour.h"
#include "NL/nlMath.h"

typedef float Mtx[3][4];
typedef float Mtx44[4][4];

extern "C"
{
    GLView* fn_8027267C(int index);
    void GXGetProjectionv(float* projection);
    void GXSetProjection(const Mtx44 projection, int type);
    void GXSetProjectionv(const float* projection);
    void GXGetViewportv(float* viewport);
    void GXProject(float x, float y, float z, const Mtx matrix,
        const float* projection, const float* viewport, float* screenX,
        float* screenY, float* screenZ);
    void PSMTXIdentity(Mtx matrix);
    void* memcpy(void* destination, const void* source, u32 size);
}

extern u8 lbl_806DCDE0;

DepthOfFieldManager::DepthOfFieldManager()
    : m_bOn(false)
    , m_bDebugView(false)
    , m_fDistanceFromCamera(16.0f)
    , m_fIntensity(1.0f)
    , mUnidentified0C(0)
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

    GLView* view = fn_8027267C(22);
    view->m_Target = 8;

    if (!m_bOn)
    {
        return;
    }

    if (!lbl_806DCDE0)
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
    Mtx identity;
    glPoly2 poly;

    GXGetProjectionv(savedProjection);

    memcpy(&projectionMatrix,
        fn_8027267C(6)->m_Interface->GetProjectionMatrix(),
        sizeof(projectionMatrix));
    GXSetProjection(projectionMatrix.e2, 0);

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
    poly.Attach(fn_8027267C(23), 0, 0);
}
