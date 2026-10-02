#include "Game/World/WorldDrawable.h"
#include "Game/World/WorldVisibility.h"
#include "Game/World/WorldAnimObjects.h"
#include "Game/World/worldanim.h"
#include "Game/World/WorldPhysicsDescription.h"

#include "Game/Physics/PhysicsBox.h"
#include "Game/Physics/PhysicsCapsule.h"
#include "Game/Physics/PhysicsFinitePlane.h"
#include "Game/Physics/PhysicsPlane.h"
#include "Game/Physics/PhysicsSphere.h"
#include "Game/Debug/ShapeRender.h"
#include "Game/Drawable/DrawableObj.h"
#include "Game/Effects/EmissionController.h"
#include "Game/Effects/EmissionManager.h"
#include "Game/GL/GLInventory.h"
#include "Game/GL/GLVertexAnim.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/World.h"
#include "Game/World/WorldEffect.h"
#include "Game/Render/Frustum.h"
#include "NL/gl/glMemory.h"
#include "NL/gl/glModel.h"
#include "NL/gl/glState.h"
#include "NL/gl/glView.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"

#include <math.h>

void InitializeWorldAnimObject(WorldAnimObject* pAnimObject,
    WorldObjectLoadContext* pContext)
{
    WorldAnimManager* pManager
        = &pContext->m_pWorld->mWorldAnimManager;
    pAnimObject->m_pAnimController
        = pManager->GetOrCreateController(pAnimObject->m_uHashID);

    u8* pData = pContext->GetParentData();
    pAnimObject->m_pAnimationHashes = (unsigned long*)(pData
        + pAnimObject->m_nBindings * sizeof(WorldAnimBinding));
    pAnimObject->m_pAnimController->m_pWorldAnimObject = pAnimObject;

    pManager->BindHierarchy(
        pAnimObject->m_pAnimController, pAnimObject->m_uHierarchyHash);
    pAnimObject->m_pAnimController->SetWorldMatrix(
        *((DrawableObject*)pAnimObject)->GetWorldMatrix());

    if (pAnimObject->m_nAnimations == 0)
    {
        pAnimObject->m_pAnimController->SetAnimation(
            nlStringLowerHash("idle"), pAnimObject->m_ePlayMode);
    }
    else if (pAnimObject->m_nAnimations == 1)
    {
        pAnimObject->m_pAnimController->SetAnimation(
            pAnimObject->m_pAnimationHashes[0], pAnimObject->m_ePlayMode);
    }
    else
    {
        unsigned long animationIndex
            = nlRandom(pAnimObject->m_nAnimations, &nlDefaultSeed) - 1;
        pAnimObject->m_pAnimController->SetAnimation(
            pAnimObject->m_pAnimationHashes[animationIndex], PM_HOLD);
    }

    pAnimObject->m_pAnimController->SetAnimationSpeed(
        pAnimObject->m_fAnimationSpeed);
    pAnimObject->m_pAnimController->SetAnimationTime(
        pAnimObject->m_fAnimationTime);
    pAnimObject->m_pBindings
        = (WorldAnimBinding*)pContext->GetParentData();
}

void BindWorldAnimObjectDrawables(WorldAnimObject* pAnimObject)
{
    if (pAnimObject->m_pBindings != 0)
    {
        for (int bindingIndex = 0; bindingIndex < pAnimObject->m_nBindings;
             ++bindingIndex)
        {
            WorldDrawable* pDrawable
                = (WorldDrawable*)pAnimObject->m_pWorld
                      ->FindDrawableObject(
                          pAnimObject->m_pBindings[bindingIndex].m_uDrawableHash);
            if (pDrawable != 0)
            {
                pDrawable->m_nAnimNode
                    = pAnimObject->m_pAnimController->GetNodeIndexByID(
                        pAnimObject->m_pBindings[bindingIndex].m_uNodeHash);
                pDrawable->m_pAnimController
                    = pAnimObject->m_pAnimController;
            }
        }
    }
}

void SelectRandomWorldAnimation(WorldAnimObject* pAnimObject)
{
    if (pAnimObject->m_nAnimations > 1)
    {
        unsigned long animationIndex
            = nlRandom(pAnimObject->m_nAnimations, &nlDefaultSeed) - 1;
        pAnimObject->m_pAnimController->SetAnimation(
            pAnimObject->m_pAnimationHashes[animationIndex], PM_HOLD);
    }
}

void* DestroyWorldAnimObject(void* pObject, int shouldDelete)
{
    if (pObject != 0 && shouldDelete > 0)
    {
        ::operator delete(pObject);
    }
    return pObject;
}

nlMatrix4* GetWorldAnimObjectMatrix(
    WorldDrawable* pObject)
{
    return &pObject->mWorldMatrix;
}

void SetWorldAnimObjectMatrix(WorldDrawable* pObject,
    const nlMatrix4* pTransform)
{
    pObject->mWorldMatrix = *pTransform;
}

void ReleaseWorldAnimObjectResources(void*)
{
}

static nlMatrix4 s_worldVisibilityIdentityMatrix;

nlMatrix4* GetWorldVisibilityDrawableMatrix(void*)
{
    s_worldVisibilityIdentityMatrix.SetIdentity();
    return &s_worldVisibilityIdentityMatrix;
}

nlMatrix4* WorldDrawable::GetWorldMatrix()
{
    if (m_pAnimController != 0)
    {
        return &m_pAnimController->GetNodeMatrix(m_nAnimNode);
    }
    return &mWorldMatrix;
}

bool WorldDrawable::IsVisibleInFrustum(const nlVector4* pCullData) const
{
    FrustumResult result;
    if (m_pAnimController != 0)
    {
        if (m_pAnimController->GetMorphWeight(m_nAnimNode) == 0.0f)
        {
            return false;
        }

        float fRadius = m_fBoundingRadius;
        nlMatrix4& worldMatrix
            = m_pAnimController->GetNodeMatrix(m_nAnimNode);
        result = ClassifySphereInFrustum(pCullData,
            (const nlVector3*)&worldMatrix.e2[3][0], fRadius);
    }
    else
    {
        float fRadius = m_fBoundingRadius;
        nlMatrix4& worldMatrix
            = *const_cast<WorldDrawable*>(this)->GetWorldMatrix();
        result = ClassifySphereInFrustum(pCullData,
            (const nlVector3*)&worldMatrix.e2[3][0], fRadius);
    }
    return result != FRUSTUM_OUTSIDE;
}

void WorldDrawable::Draw()
{
    DrawToView(0);
}

inline bool WorldDrawable::ResolveVertexAnim(GLVertexAnim*& pVertexAnim)
{
    unsigned long uAnimationHash = GetModel()->GetID();
    GLResourcePool* pPool = glGetCurrentResourcePool();
    pVertexAnim = pPool->m_inventory->GetVertexAnim(uAnimationHash);
    return pVertexAnim != 0;
}

inline float WorldAnimManager::GetFrame(int nFrames, int nFrameRate) const
{
    float fFrameRate = (float)nFrameRate;
    float fTime = m_fTime;
    float fDuration = (float)nFrames / fFrameRate;
    float fFrameTime = fTime / fDuration;
    float fFrame = (float)nFrames * (fFrameTime - floorf(fFrameTime));
    return fFrame;
}

void WorldDrawable::DrawToView(GLView* pView)
{
    GLView* pAlphaView;
    glModel* pModel = GetModel();
    GLVertexAnim* pVertexAnim;
    if (ResolveVertexAnim(pVertexAnim))
    {
        float fFrame = m_pWorldContext->mWorldAnimManager.GetFrame(
            pVertexAnim->GetNumFrames(), 30);
        pModel = pVertexAnim->GetModel((int)fFrame);
        UpdateModelMaterials(pModel);
    }

    if (m_pAnimController != 0)
    {
        glModelSetMatrix(pModel,
            m_pAnimController->GetNodeMatrix(m_nAnimNode));
    }
    else
    {
        glModelSetMatrix(pModel, *GetWorldMatrix());
    }

    GLView* pOpaqueView = m_pWorldContext->m_pOpaqueView;
    pAlphaView = m_pWorldContext->m_pAlphaView;
    if (pAlphaView == 0)
    {
        pAlphaView = pView;
    }

    for (unsigned long packetIndex = 0; packetIndex < pModel->numPackets;
         ++packetIndex)
    {
        glModelPacket* pPacket = &pModel->packets[packetIndex];
        if (pView != 0)
        {
            pView->AttachPacket(pPacket, 0);
        }
        else if (glGetRasterState(
                     pPacket->rasterState, GLS_AlphaBlend)
            == 0)
        {
            pOpaqueView->AttachPacket(pPacket, 0);
        }
        else
        {
            pAlphaView->AttachPacket(pPacket, 1);
        }
    }
}

void WorldDrawable::Initialize(WorldObjectLoadContext* pContext)
{
    glModel*& pModel = m_pModel;
    pContext->m_pWorld->ResolveModel(pModel);
    m_pModel = glModelDupNoStreams(
        (glModel*)m_pModel, true,
        pContext->m_pWorld->m_pResource);
}

void InitializeWorldVisibilityDrawable(
    WorldVisibilityDrawable* pDrawable,
    WorldObjectLoadContext* pContext)
{
    glModel*& pModel = pDrawable->m_pModel;
    pContext->m_pWorld->ResolveModel(pModel);
    pDrawable->m_pModel = glModelDupNoStreams(pDrawable->GetModel(),
        true, pContext->m_pWorld->m_pResource);

    pDrawable->m_pVisibilityNode = FindWorldVisibilityNode(
        pDrawable, pContext->m_pWorld->m_pVisibilityTree);
}

WorldVisibilityNode* FindWorldVisibilityNode(
    WorldVisibilityDrawable* pDrawable,
    WorldVisibilityNode* pNode)
{
    int index;
    for (index = 0; index < pNode->mNumModelHashes; ++index)
    {
        if (pNode->mModelHashes[index] == pDrawable->GetModel()->GetID())
        {
            return pNode;
        }
    }

    for (index = 0; index < 2; ++index)
    {
        if (pNode->mChildren[index] != 0)
        {
            WorldVisibilityNode* pFound = FindWorldVisibilityNode(
                pDrawable, pNode->mChildren[index]);
            if (pFound != 0)
            {
                return pFound;
            }
        }
    }
    return 0;
}

void DrawWorldVisibilityDrawable(
    WorldVisibilityDrawable* pDrawable)
{
    GLView* pOpaqueView = pDrawable->m_pWorld->m_pOpaqueView;
    GLView* pAlphaView = pDrawable->m_pWorld->m_pAlphaView;
    if (pAlphaView == 0)
    {
        pAlphaView = pOpaqueView;
    }

    for (unsigned long packetIndex = 0;
         packetIndex < pDrawable->GetModel()->numPackets; ++packetIndex)
    {
        glModelPacket* pPacket = &pDrawable->GetModel()->packets[packetIndex];
        if (glGetRasterState(
                pPacket->rasterState, GLS_AlphaBlend)
            == 0)
        {
            pOpaqueView->AttachPacket(pPacket, 0);
        }
        else
        {
            pAlphaView->AttachPacket(pPacket, 1);
        }
    }
}

bool IsWorldVisibilityDrawableVisible(
    WorldVisibilityDrawable* pDrawable)
{
    return pDrawable->m_pVisibilityNode->mVisible == 1;
}

void InitializeWorldPhysicsDrawable(
    WorldPhysicsDrawable* pDrawable,
    WorldObjectLoadContext*)
{
    pDrawable->m_pPhysicsObject = CreatePhysicsPrimitive(
        &pDrawable->m_Description, 0);
}

void ReleaseWorldPhysicsDrawableResources(WorldPhysicsDrawable* pDrawable)
{
    ReleaseWorldPhysicsObject(pDrawable);
}

void* DestroyWorldVisibilityDrawable(void* pObject, int shouldDelete)
{
    if (pObject != 0 && shouldDelete > 0)
    {
        ::operator delete(pObject);
    }
    return pObject;
}

void SetWorldVisibilityDrawableMatrix(void*)
{
}

void ReleaseWorldVisibilityDrawableResources(void*)
{
}

static bool s_drawEffectBounds;

void InitializeWorldEffect(WorldEffect* pEffect,
    WorldObjectLoadContext* pContext)
{
    pEffect->m_bActive = true;
    if (pEffect->m_nTimingMode == 0)
    {
        float fEmissionInterval = pEffect->m_fEmissionInterval;
        pEffect->m_fEmissionTime
            = fEmissionInterval + 1.0f;
    }
    else
    {
        pEffect->m_fEmissionTime = 0.0f;
    }
    pEffect->m_fPreviousEmissionTime = pEffect->m_fEmissionTime;
    pEffect->m_nRemainingEmissions = pEffect->m_nEmissionCount;
    pContext->m_pWorld->AddEffect(pEffect);
}

void ReleaseWorldEffectResources(WorldEffect* pEffect)
{
    nlDLListIterator<EmissionController*> controllerIterator
        = EmissionManager::Instance()->GetContainer()->Begin();
    DLListEntry<EmissionController*>* pHead = controllerIterator.m_Head;
    DLListEntry<EmissionController*>* pCurrent = controllerIterator.m_Curr;
    while (pCurrent != 0)
    {
        EmissionController* pController = pCurrent->entry;
        if (pController->m_uUserData == (u32)pEffect)
        {
            pController->ClearParticles();
            pController->mUpdateCallback.Clear();
        }
        if (nlDLRingIsEnd(pHead, pCurrent) || pCurrent == 0)
        {
            pCurrent = 0;
        }
        else
        {
            pCurrent = pCurrent->m_next;
        }
    }
}

void WorldEffect::Update(float fDeltaT)
{
    if (fDeltaT != 0.0f && m_bActive)
    {
        bool bEmit = false;
        if (m_nTimingMode == 0)
        {
            if (m_nRemainingEmissions > 0
                || m_nRemainingEmissions == -1)
            {
                float fEmissionTime = m_fEmissionTime;
                float fEmissionInterval
                    = m_fEmissionInterval;
                fEmissionTime += fDeltaT;
                m_fEmissionTime = fEmissionTime;
                if (fEmissionInterval <= fEmissionTime
                    && nlRandom(100, &nlDefaultSeed)
                        < m_uProbability)
                {
                    bEmit = true;
                }
            }
        }
        else if (m_nRemainingEmissions > 0
            || m_nRemainingEmissions == -1)
        {
            float fEmissionTime = m_fEmissionTime;
            float fEmissionInterval = m_fEmissionInterval;
            if (fEmissionInterval <= fEmissionTime
                && nlRandom(100, &nlDefaultSeed)
                    < m_uProbability)
            {
                bEmit = true;
            }
        }

        if (bEmit)
        {
            Emit();
        }
    }
}

void WorldEffect::Emit()
{
    EffectsGroup* pGroup
        = fxGetGroup(EmissionManager::Instance(),
            m_uEffectHash);
    if (pGroup != 0)
    {
        EmissionController* pController
            = EmissionManager::Instance()->Create(pGroup,
                1, true, 0);
        m_fEmissionRadius = pController->GetBoundingRadius();

        nlVector3 velocity = { 0.0f, 0.0f, 0.0f };
        pController->SetVelocity(velocity);
        pController->m_fGround = 0.02f;

        nlMatrix4* pMatrix
            = ((DrawableObject*)this)->GetWorldMatrix();
        pController->SetPosition(
            *(nlVector3*)&pMatrix->e2[3][0]);
        pMatrix = ((DrawableObject*)this)->GetWorldMatrix();
        nlVector3 direction;
        nlVec3Set(direction, pMatrix->e2[2][0], pMatrix->e2[2][1],
            pMatrix->e2[2][2]);
        pController->SetDirection(direction);

        if (m_pAnimController != 0)
        {
            pController->SetUpdateCallback(
                Function1<void, EmissionController&>(
                    UpdateAnimatedWorldEffectController));
            pController->m_uUserData = (u32)this;
        }
        else
        {
            pController->SetUpdateCallback(
                Function1<void, EmissionController&>(
                    UpdateWorldEffectControllerVisibility));
            pController->m_uUserData = (u32)this;
        }
        m_nEmissionID = pController->m_Id;
    }
    else
    {
        m_nEmissionID = -1;
    }

    m_fPreviousEmissionTime = m_fEmissionTime;
    m_fEmissionTime = 0.0f;
    --m_nRemainingEmissions;
    if (m_nRemainingEmissions < -1)
    {
        m_nRemainingEmissions = -1;
    }
}

void UpdateAnimatedWorldEffectController(EmissionController& controller)
{
    WorldEffect* pEffect
        = (WorldEffect*)controller.m_uUserData;
    if (pEffect != 0
        && pEffect->m_pAnimController->GetAnimationTime() != 0.0f)
    {
        nlMatrix4& nodeMatrix
            = pEffect->m_pAnimController->GetNodeMatrix(
                pEffect->m_nAnimNode);
        controller.SetPosition(*(nlVector3*)&nodeMatrix.e2[3][0]);
        controller.SetDirection(*(nlVector3*)&nodeMatrix.e2[2][0]);
        pEffect->UpdateVisibility(&controller);
    }
}

void UpdateWorldEffectControllerVisibility(EmissionController& controller)
{
    WorldEffect* pEffect
        = (WorldEffect*)controller.m_uUserData;
    if (pEffect != 0)
    {
        pEffect->UpdateVisibility(&controller);
    }
}

static const nlColour s_effectBoundsColour
    = { 0xFF, 0xFF, 0x80, 0xFF };

void WorldEffect::UpdateVisibility(EmissionController* pController)
{
    const nlVector3& position = pController->GetPosition();
    float radius = m_fEmissionRadius;
    if (!m_bActive)
    {
        pController->m_bVisible = false;
    }
    else if (m_bAlwaysVisible == true)
    {
        pController->m_bVisible = true;
    }
    else
    {
        const nlVector4* pCullData = m_pWorld->m_pOpaqueView
                                         ->m_Interface->GetShadowMatrix();
        if (ClassifySphereInFrustum(pCullData, &position, radius)
                == FRUSTUM_OUTSIDE
            || !m_pWorld->m_bRenderingEnabled)
        {
            pController->m_bVisible = false;
        }
        else
        {
            pController->m_bVisible = true;
        }
    }

    if (s_drawEffectBounds)
    {
        nlColour colour = s_effectBoundsColour;
        g_ShapeRenderer.DrawSphere(position, colour, radius);
    }
}

void* DestroyWorldEffect(void* pObject, int shouldDelete)
{
    if (pObject != 0 && shouldDelete > 0)
    {
        ::operator delete(pObject);
    }
    return pObject;
}
