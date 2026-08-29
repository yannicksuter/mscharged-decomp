#include "Game/FE/feRender.h"

#include "Game/FE/fePackage.h"
#include "Game/FE/fePresentation.h"
#include "Game/FE/feScene.h"
#include "Game/FE/feTextureResource.h"
#include "Game/FE/tlComponent.h"
#include "Game/FE/tlImageInstance.h"
#include "Game/FE/tlTextInstance.h"
#include "Game/FE/tlSlide.h"
#include "NL/gl/glDraw3.h"
#include "NL/gl/glMatrix.h"
#include "NL/gl/glState.h"
#include "NL/nlString.h"
#include "NL/platvmath.h"

static nlFloatColour s_currentAssetColour;
static const nlVector3 s_quadPositions[4] = {
    { -50.0f, 50.0f, 0.0f },
    { -50.0f, -50.0f, 0.0f },
    { 50.0f, -50.0f, 0.0f },
    { 50.0f, 50.0f, 0.0f },
};
FEScene* FERender::m_pRenderScene;

typedef void (*MovieRenderCallback)(eGLView, unsigned long, const nlFloatColour&, const nlVector2*, const nlVector2*);
static MovieRenderCallback s_movieRenderCallback;
static const unsigned long grabTex = nlStringLowerHash("target/grab_texture");
static const unsigned long movieTex = nlStringLowerHash("movie");

extern "C" float fn_80302210(const TLInstance* instance);
extern "C" float fn_80302230(const TLInstance* instance);
extern "C" float fn_80302250(const TLInstance* instance);
extern "C" float fn_80302270(const TLInstance* instance);

void FERender::Initialize()
{
}

void FERender::PushTransformMatrix(const TLInstance* instance, const nlMatrix4& parentMatrix, nlMatrix4& combinedMatrix)
{
    const feVector3& tlPosition = instance->GetPosition();
    const feVector3& tlPivot = instance->GetPivot();
    const feVector3& tlRotation = instance->GetRotation();
    const feVector3& tlScale = instance->GetScale();
    unsigned long flags = 0;

    if (tlPosition.f.x == 0.0f && tlPosition.f.y == 0.0f && tlPosition.f.z == 0.0f)
    {
        flags |= 2;
    }
    if (tlPivot.f.x == 0.0f && tlPivot.f.y == 0.0f && tlPivot.f.z == 0.0f)
    {
        flags |= 1;
    }
    if (tlRotation.f.x == 0.0f && tlRotation.f.y == 0.0f && tlRotation.f.z == 0.0f)
    {
        flags |= 4;
    }
    if (tlScale.f.x == 1.0f && tlScale.f.y == 1.0f && tlScale.f.z == 1.0f)
    {
        flags |= 8;
    }

    if ((flags & 0xF) == 0xF)
    {
        combinedMatrix = parentMatrix;
        return;
    }

    nlMatrix4 scalePivotMatrix;
    if ((flags & 1) == 0 || (flags & 8) == 0)
    {
        nlMatrix4 scaleMatrix;
        nlMatrix4 pivotMatrix;
        nlMakeScaleMatrix(scaleMatrix, tlScale.f.x, tlScale.f.y, tlScale.f.z);
        nlMakeTranslationMatrix(pivotMatrix, -tlPivot.f.x, -tlPivot.f.y, -tlPivot.f.z);
        nlMultMatrices(scalePivotMatrix, pivotMatrix, scaleMatrix);
    }
    else
    {
        scalePivotMatrix.SetIdentity();
    }

    nlMatrix4 localMatrix;
    if ((flags & 4) == 0)
    {
        nlMatrix4 rotationMatrix;
        nlMakeRotationMatrixEulerAngles(rotationMatrix, tlRotation.f.x, tlRotation.f.y, tlRotation.f.z);
        nlMultMatrices(localMatrix, scalePivotMatrix, rotationMatrix);
    }
    else
    {
        localMatrix = scalePivotMatrix;
    }

    if ((flags & 2) == 0)
    {
        localMatrix.m41 += tlPosition.f.x;
        localMatrix.m42 += tlPosition.f.y;
        localMatrix.m43 += tlPosition.f.z;
    }
    localMatrix.m43 *= -1.0f;
    nlMultMatrices(combinedMatrix, localMatrix, parentMatrix);
}

void FERender::RenderTimeLineAsset(TLInstance* pTLInstance, float fCurrentTime, const nlMatrix4& parentMatrix)
{
    if (!pTLInstance->IsValidAtTime(fCurrentTime))
    {
        return;
    }
    if (!pTLInstance->m_bVisible)
    {
        return;
    }
    if (!pTLInstance->m_component->m_attributes.bVisible)
    {
        return;
    }

    nlMatrix4 combinedMatrix;
    PushTransformMatrix(pTLInstance, parentMatrix, combinedMatrix);

    for (unsigned long i = 0; i < 4; i++)
    {
        s_currentAssetColour.c[i] = (pTLInstance->GetColour().c[i] * s_currentAssetColour.c[i]) / 255.0f;
    }

    switch (pTLInstance->m_type)
    {
    case TLAT_IMAGE:
        RenderImageInstance((const TLImageInstance*)pTLInstance, combinedMatrix);
        break;
    case TLAT_TEXT:
    {
        nlMatrix4 textMatrix;
        nlMultMatrices(textMatrix, combinedMatrix, m_pRenderScene->m_matView);
        TLTextInstance* textInstance = (TLTextInstance*)pTLInstance;
        textInstance->SetMatrix(&textMatrix);
        nlColour colour;
        for (unsigned long i = 0; i < 4; i++)
        {
            colour.c[i] = (unsigned char)(255.0f * s_currentAssetColour.c[i]);
        }
        textInstance->Render((eGLView)m_pRenderScene->m_uRenderView, colour);
        break;
    }
    case TLAT_COMPONENT:
    {
        TLComponent* component = pTLInstance->m_component;
        if (component != 0)
        {
            TLSlide* slide = component->GetActiveSlide();
            if (slide != 0 && slide->m_instances != 0)
            {
                TLInstance* curr = slide->m_instances->m_next;
                for (;;)
                {
                    TLInstance* next = curr->m_next;
                    nlFloatColour colour = s_currentAssetColour;
                    RenderTimeLineAsset(curr, slide->m_time, combinedMatrix);
                    s_currentAssetColour = colour;
                    if (curr == slide->m_instances)
                    {
                        break;
                    }
                    curr = next;
                }
            }
        }
        break;
    }
    default:
        break;
    }

    if (pTLInstance->pChildren != 0)
    {
        TLInstance* curr = pTLInstance->pChildren->m_next;
        for (;;)
        {
            TLInstance* next = curr->m_next;
            nlFloatColour colour = s_currentAssetColour;
            RenderTimeLineAsset(curr, fCurrentTime, combinedMatrix);
            s_currentAssetColour = colour;
            if (curr == pTLInstance->pChildren)
            {
                break;
            }
            curr = next;
        }
    }
}

extern "C" void fn_802FC280()
{
}

void FERender::RenderScene(FEScene* scene)
{
    if (scene == 0)
    {
        return;
    }

    m_pRenderScene = scene;
    s_currentAssetColour.c[0] = 1.0f;
    s_currentAssetColour.c[1] = 1.0f;
    s_currentAssetColour.c[2] = 1.0f;
    s_currentAssetColour.c[3] = 1.0f;

    nlMatrix4 identity;
    identity.SetIdentity();

    FEPresentation* presentation = scene->m_pFEPackage->GetPresentation();
    if (presentation != 0 && presentation->m_slides != 0)
    {
        TLSlide* slide = presentation->m_currentSlide;
        if (slide != 0 && slide->m_instances != 0)
        {
            TLInstance* curr = slide->m_instances->m_next;
            for (;;)
            {
                TLInstance* next = curr->m_next;
                nlFloatColour colour = s_currentAssetColour;
                RenderTimeLineAsset(curr, slide->m_time, identity);
                s_currentAssetColour = colour;
                if (curr == slide->m_instances)
                {
                    break;
                }
                curr = next;
            }
        }
    }
    m_pRenderScene = 0;
}

unsigned char FERender::RenderImageInstance(const TLImageInstance* pTLImageInstance, const nlMatrix4& matrix)
{
    nlColour colour;
    for (unsigned long i = 0; i < 4; i++)
    {
        colour.c[i] = (unsigned char)(255.0f * s_currentAssetColour.c[i]);
    }

    FETextureResource* pTexRes = pTLImageInstance->m_pTextureResource;
    if (!pTexRes->IsValid())
    {
        return 1;
    }

    unsigned long textureHandle = pTexRes->GetTextureHandle();
    float halfPixelU = 0.5f / (float)pTexRes->m_uWidth;
    float halfPixelV = 0.5f / (float)pTexRes->m_uHeight;
    float left = fn_80302210(pTLImageInstance);
    float top = fn_80302230(pTLImageInstance);
    float bottom = 1.0f - (fn_80302270(pTLImageInstance) + top);
    float right = left + fn_80302250(pTLImageInstance);
    top = 1.0f - fn_80302230(pTLImageInstance);

    glSetDefaultState(false);
    glSetRasterState(GLS_Culling, 0);
    if (textureHandle != grabTex && textureHandle != movieTex)
    {
        glSetRasterState(GLS_AlphaBlend, pTLImageInstance->field_0x94);
        glSetRasterState(GLS_AlphaTest, 1);
        glSetRasterState(GLS_AlphaTestRef, 0);
    }
    glSetCurrentRasterState(glHandleizeRasterState());

    nlMatrix4 matTM = matrix;
    matTM.m41 += m_pRenderScene->m_matView.m41;
    matTM.m42 += m_pRenderScene->m_matView.m42;
    matTM.m43 += m_pRenderScene->m_matView.m43;
    unsigned long matrixHandle = glAllocMatrix();
    if (matrixHandle != 0xFFFFFFFF)
    {
        glSetMatrix(matrixHandle, matTM);
    }
    glSetCurrentMatrix(matrixHandle);

    if (textureHandle == movieTex && s_movieRenderCallback != 0)
    {
        nlVector2 pos[4];
        nlVector2 uv[4];
        pos[0].x = s_quadPositions[0].x;
        pos[0].y = s_quadPositions[0].y;
        pos[1].x = s_quadPositions[1].x;
        pos[1].y = s_quadPositions[1].y;
        pos[2].x = s_quadPositions[2].x;
        pos[2].y = s_quadPositions[2].y;
        pos[3].x = s_quadPositions[3].x;
        pos[3].y = s_quadPositions[3].y;
        uv[0].x = left + halfPixelU;
        uv[0].y = bottom + halfPixelV;
        uv[1].x = left + halfPixelU;
        uv[1].y = top - halfPixelV;
        uv[2].x = right - halfPixelU;
        uv[2].y = top - halfPixelV;
        uv[3].x = right - halfPixelU;
        uv[3].y = bottom + halfPixelV;
        s_movieRenderCallback((eGLView)m_pRenderScene->m_uRenderView, textureHandle, s_currentAssetColour, pos, uv);
    }
    else
    {
        glSetCurrentTexture(textureHandle, GLTT_Diffuse);
        glQuad3 quad;
        quad.m_pos[0] = s_quadPositions[0];
        quad.m_pos[1] = s_quadPositions[1];
        quad.m_pos[2] = s_quadPositions[2];
        quad.m_pos[3] = s_quadPositions[3];
        quad.m_uv[0].x = left + halfPixelU;
        quad.m_uv[0].y = bottom + halfPixelV;
        quad.m_uv[1].x = left + halfPixelU;
        quad.m_uv[1].y = top - halfPixelV;
        quad.m_uv[2].x = right - halfPixelU;
        quad.m_uv[2].y = top - halfPixelV;
        quad.m_uv[3].x = right - halfPixelU;
        quad.m_uv[3].y = bottom + halfPixelV;
        quad.SetColour(colour);
        glAttachQuad3((eGLView)m_pRenderScene->m_uRenderView, 0, 1, &quad);
    }

    return 1;
}
