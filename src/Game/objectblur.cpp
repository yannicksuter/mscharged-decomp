#include "Game/ObjectBlur.h"

#include "Game/Camera/CameraMan.h"
#include "Game/Effects/EmissionManager.h"
#include "Game/GL/GLTexturedColourMeshWriter.h"
#include "Game/MathHelpers.h"
#include "Game/ReplayManager.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "NL/gl/gl.h"
#include "NL/gl/glMatrix.h"
#include "NL/gl/glState.h"
#include "NL/gl/glView.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"

BlurHandler* BlurManager::m_activeBlurHandler = 0;

SlotPool<BlurHandler> BlurHandler::m_BlurHandlerSlotPool(0x10, 0x10);

static float fFlimmerOffset = 0.01f;

static inline void SetTextureBinding(glTextureBinding* binding, u32 texture)
{
    binding->texture = texture;
    binding->textureIndex = 0xFFFF;
    binding->SetWrapS(true);
    binding->SetWrapT(true);
    binding->unknown07 = 0;
}

static inline void SetTexcoord(
    GLTexturedColourMeshWriter& mesh, float u, float v)
{
    short texU = (short)(u * 1024.0f);
    short texV = (short)(v * 1024.0f);
    mesh.Texcoord(texU, texV);
}

static inline void glViewAttachModel(
    GLView* view, unsigned long layer, const glModel* model)
{
    view->AttachModel(model, layer);
}

bool BlurHandler::ConstructViewOrientedPoints(
    nlVector3& topPoint, nlVector3& bottomPoint, nlVector3 position,
    const nlVector3& forwardVector)
{
    nlVector3 viewVector;
    nlVector3 normalizedForward;
    nlVector3 perpendicular;

    float sLen1 = nlVec3LengthSquared(forwardVector);
    if (sLen1 < 0.5f)
    {
        return false;
    }

    float invLen = nlRecipSqrt(sLen1, 1);
    nlVec3Scale(normalizedForward, forwardVector, invLen);

    cCameraManager::GetViewVector(viewVector);
    if (nlVec3DotProduct(viewVector, normalizedForward) < 0.99f)
    {
        nlVec3CrossProduct(perpendicular, normalizedForward, viewVector);
        nlVec3Normalize(perpendicular, perpendicular);
        nlVec3Scale(perpendicular, m_fLineWidth);
    }
    else
    {
        if (m_pLastPoint != 0)
        {
            nlVec3Sub(perpendicular, m_pLastPoint->v3Top, m_pLastPoint->v3Bottom);
            nlVec3Scale(perpendicular, 0.5f);
        }
        else
        {
            return false;
        }
    }

    nlVec3Add(topPoint, position, perpendicular);
    nlVec3Sub(bottomPoint, position, perpendicular);

    return true;
}

void BlurHandler::AddViewOrientedPoint(
    const nlVector3& position, const nlVector3& forwardVector)
{
    nlVector3 topPoint, bottomPoint;

    if (ConstructViewOrientedPoints(topPoint, bottomPoint, position, forwardVector))
    {
        m_pointFinal.v3Top = topPoint;
        m_pointFinal.v3Bottom = bottomPoint;

        if (m_pLastPoint != 0)
        {
            if (CalculateDistanceSquared(m_pLastPoint->v3Top, m_pointFinal.v3Top)
                < 0.0025000002f)
            {
                return;
            }
        }

        BlurPointEntry* entry = &m_pointRingBuffer[m_nInsertIndex];
        entry->v3Top = m_pointFinal.v3Top;
        entry->v3Bottom = m_pointFinal.v3Bottom;

        m_pLastPoint = &m_pointRingBuffer[m_nInsertIndex];
        m_nInsertIndex = m_nInsertIndex + 1;
        m_nInsertIndex %= m_maxPositionEntries;

        if (m_nInsertIndex == m_nTrailEndPointer)
        {
            m_nTrailEndPointer = m_nTrailEndPointer + 1;
            m_nTrailEndPointer %= m_maxPositionEntries;
        }
    }
    else
    {
        if (m_nInsertIndex != m_nTrailEndPointer)
        {
            m_nTrailEndPointer = m_nTrailEndPointer + 1;
            m_nTrailEndPointer %= m_maxPositionEntries;
        }
    }
}

void BlurHandler::Die(float timeToDie)
{
    m_bDying = true;
    m_fDyingTimer = timeToDie;
    m_fTimeToDie = timeToDie;
}

static const nlColour kWhite = { 0xFF, 0xFF, 0xFF, 0xFF };

void BlurHandler::RenderMesh(unsigned long uTexID)
{
    u32 count;
    u32 uPointIndex;
    u32 matHandle;
    float nonAdditiveAlpha;
    BlurPointEntry* entry;

    count = 0;
    if (m_nTrailEndPointer != m_nInsertIndex)
    {
        if (m_nTrailEndPointer < m_nInsertIndex)
        {
            count = m_nInsertIndex - m_nTrailEndPointer;
        }
        else
        {
            count = m_maxPositionEntries - m_nTrailEndPointer;
            count += m_nInsertIndex;
        }
    }

    if (count < 2)
    {
        return;
    }

    GLTexturedColourMeshWriter mesh;

    glSetDefaultState(true);

    nlMatrix4 matWorld;
    matWorld.SetIdentity();
    matWorld.m43 += fFlimmerOffset;

    matHandle = glAllocMatrix();
    if (matHandle != (u32)-1)
    {
        glSetMatrix(matHandle, matWorld);
    }
    glSetCurrentMatrix(matHandle);

    if (m_bAdditive)
    {
        glSetRasterState(GLS_AlphaBlend, 2);
    }
    else
    {
        glSetRasterState(GLS_AlphaBlend, 1);
    }
    glSetRasterState(GLS_AlphaTest, 1);
    glSetRasterState(GLS_AlphaTestRef, 0);
    glSetRasterState(GLS_DepthWrite, 0);
    glSetRasterState(GLS_Culling, 0);
    glSetCurrentRasterState(glHandleizeRasterState());

    nlColour colour = kWhite;
    nonAdditiveAlpha = 0.0f;

    if (m_bAdditive)
    {
        nlColourSet(colour, 0xFF, 0xFF, 0xFF, 0xFF);
        if (m_bDying)
        {
            int fadeVal = 255.0f * (m_fDyingTimer / m_fTimeToDie);
            nlColourSeReversed(colour, fadeVal, fadeVal, fadeVal, fadeVal);
        }
    }
    else if (m_bDying)
    {
        nonAdditiveAlpha = 90.0f * (m_fDyingTimer / m_fTimeToDie);
    }
    else
    {
        nonAdditiveAlpha = 90.0f;
    }

    uPointIndex = m_nTrailEndPointer;
    float texU = 0.0f;
    float texUIncrement = 1.0f / (float)count;

    if (mesh.Begin(count * 2, GLP_TriStrip, 0))
    {
        SetTextureBinding(
            (glTextureBinding*)mesh.GetModel()->packets->materialParameters,
            uTexID);

        for (u32 i = 0; i < count; i++)
        {
            entry = &m_pointRingBuffer[uPointIndex];
            if (!m_bAdditive)
            {
                colour.c[3] = nonAdditiveAlpha * texU;
            }

            if ((i == 0 || i == count - 1) && !m_bDying)
            {
                int pointIndexA;
                int pointIndexB;
                if (i == 0)
                {
                    pointIndexB = uPointIndex + 1;
                    pointIndexA = uPointIndex;
                    if (pointIndexB == m_maxPositionEntries)
                    {
                        pointIndexB = 0;
                    }
                }
                else
                {
                    pointIndexA = uPointIndex - 1;
                    pointIndexB = uPointIndex;
                    if (pointIndexA < 0)
                    {
                        pointIndexA = m_maxPositionEntries - 1;
                    }
                }

                float blendPct
                    = ReplayManager::Instance()->mRender->mFrameBlendPercent;

                nlVector3 v3Top;
                nlVector3 v3Bottom;
                nlVecLerp(
                    v3Top, m_pointRingBuffer[pointIndexA].v3Top,
                    m_pointRingBuffer[pointIndexB].v3Top, blendPct);
                nlVecLerp(
                    v3Bottom, m_pointRingBuffer[pointIndexA].v3Bottom,
                    m_pointRingBuffer[pointIndexB].v3Bottom, blendPct);

                mesh.Colour(colour);
                SetTexcoord(mesh, texU, 0.0f);
                mesh.Vertex(v3Top);

                mesh.Colour(colour);
                SetTexcoord(mesh, texU, 1.0f);
                mesh.Vertex(v3Bottom);
            }
            else
            {
                mesh.Colour(colour);
                SetTexcoord(mesh, texU, 0.0f);
                mesh.Vertex(entry->v3Top);

                mesh.Colour(colour);
                SetTexcoord(mesh, texU, 1.0f);
                mesh.Vertex(entry->v3Bottom);
            }

            uPointIndex++;
            if (uPointIndex == (u32)m_maxPositionEntries)
            {
                uPointIndex = 0;
            }
            texU += texUIncrement;
        }

        if (!mesh.End())
        {
            return;
        }
        glViewAttachModel(
            (GLView*)EmissionManager::Instance()->mContext, 5, mesh.GetModel());
    }
}

BlurHandler* BlurManager::GetNewHandler(
    const char* szTextureName, float fLineWidth, int maxPositionEntries,
    bool bAdditive)
{
    BlurHandler* handler = 0;

    BlurHandler::m_BlurHandlerSlotPool.Allocate(handler);

    if (handler != 0)
    {
        handler->m_bAdditive = bAdditive;
        handler->m_fLineWidth = fLineWidth;
        handler->m_pLastPoint = 0;
        handler->m_bDying = false;
        handler->m_maxPositionEntries = maxPositionEntries;
        handler->m_nInsertIndex = 0;
        handler->m_nTrailEndPointer = 0;
        handler->m_uTexHashID = nlStringLowerHash(szTextureName);
        handler->m_pointRingBuffer = (BlurPointEntry*)nlMalloc(
            maxPositionEntries * sizeof(BlurPointEntry), 8, false);
    }

    nlDLRingAddEnd<BlurHandler>(&BlurManager::m_activeBlurHandler, handler);

    return handler;
}

void BlurManager::DestroyHandler(BlurHandler* handler, float timeToDie)
{
    if (timeToDie == 0.0f)
    {
        nlDLRingRemove<BlurHandler>(&m_activeBlurHandler, handler);
        if (handler != 0)
        {
            delete[] handler->m_pointRingBuffer;
            BlurHandler::m_BlurHandlerSlotPool.Free(handler);
        }
    }
    else
    {
        handler->m_bDying = true;
        handler->m_fDyingTimer = timeToDie;
        handler->m_fTimeToDie = timeToDie;
    }
}

void BlurManager::Update(float deltaTime)
{
    if (m_activeBlurHandler == 0)
    {
        return;
    }

    bool isEnd;
    BlurHandler* current = nlDLRingGetStart<BlurHandler>(m_activeBlurHandler);
    do
    {
        isEnd = nlDLRingIsEnd<BlurHandler>(m_activeBlurHandler, current);
        BlurHandler* next = current->m_next;

        if (current->m_bDying)
        {
            current->m_fDyingTimer -= deltaTime;
            if (current->m_fDyingTimer <= 0.0f)
            {
                current->m_fDyingTimer = 0.0f;
            }
        }

        bool shouldRemove;
        if (current->m_bDying && current->m_fDyingTimer <= 0.0f)
        {
            shouldRemove = true;
        }
        else
        {
            shouldRemove = false;
        }

        if (shouldRemove)
        {
            nlDLRingRemove<BlurHandler>(&m_activeBlurHandler, current);
            if (current != 0)
            {
                delete[] current->m_pointRingBuffer;
                BlurHandler::m_BlurHandlerSlotPool.Free(current);
            }
        }
        else
        {
            current->RenderMesh(current->m_uTexHashID);
        }

        current = next;
    } while (!isEnd);
}

void BlurManager::Shutdown()
{
    nlDeleteDLRing<BlurHandler>(&m_activeBlurHandler);
    m_activeBlurHandler = 0;
    BlurHandler::m_BlurHandlerSlotPool.FreeBlocks();
}
