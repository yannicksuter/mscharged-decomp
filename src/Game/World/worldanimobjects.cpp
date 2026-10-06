#include "NL/nlDLListContainer.inl"
#include "Game/World/WorldDrawable.h"
#include "Game/World/WorldVisibility.h"
#include "Game/World/WorldAnimObjects.h"
#include "Game/World/worldanim.h"
#include "Game/World/WorldPhysics.h"
#include "Game/World/WorldPhysicsDescription.h"

#include "Game/Physics/PhysicsBox.h"
#include "Game/Physics/PhysicsCapsule.h"
#include "Game/Physics/PhysicsFinitePlane.h"
#include "Game/Physics/PhysicsPlane.h"
#include "Game/Physics/PhysicsSphere.h"
#include "Game/GL/GLInventory.h"
#include "Game/GL/GLVertexAnim.h"
#include "Game/World.h"
#include "Game/Render/Frustum.h"
#include "NL/gl/glMemory.h"
#include "NL/gl/glModel.h"
#include "NL/gl/glState.h"
#include "NL/gl/glView.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"

#include <math.h>

void WorldAnimObject::Initialize(WorldObjectLoadContext* pContext)
{
    WorldAnimManager* pManager
        = &pContext->m_pWorld->mWorldAnimManager;
    m_pAnimController = pManager->GetOrCreateController(m_uHashID);

    u8* pData = pContext->GetParentData();
    m_pAnimationHashes = (unsigned long*)(pData
        + m_nBindings * sizeof(WorldAnimBinding));
    m_pAnimController->m_pWorldAnimObject = this;

    pManager->BindHierarchy(
        m_pAnimController, m_uHierarchyHash);
    m_pAnimController->SetWorldMatrix(*GetWorldMatrix());

    if (m_nAnimations == 0)
    {
        m_pAnimController->SetAnimation(
            nlStringLowerHash("idle"), m_ePlayMode);
    }
    else if (m_nAnimations == 1)
    {
        m_pAnimController->SetAnimation(
            m_pAnimationHashes[0], m_ePlayMode);
    }
    else
    {
        unsigned long animationIndex
            = nlRandom(m_nAnimations, &nlDefaultSeed) - 1;
        m_pAnimController->SetAnimation(
            m_pAnimationHashes[animationIndex], PM_HOLD);
    }

    m_pAnimController->SetAnimationSpeed(m_fAnimationSpeed);
    m_pAnimController->SetAnimationTime(m_fAnimationTime);
    m_pBindings = (WorldAnimBinding*)pContext->GetParentData();
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

WorldAnimObject::~WorldAnimObject()
{
}

nlMatrix4* WorldAnimObject::GetWorldMatrix()
{
    return &mWorldMatrix;
}

void WorldAnimObject::SetWorldMatrix(const nlMatrix4& transform)
{
    mWorldMatrix = transform;
}

void WorldAnimObject::ReleaseResources()
{
}

static nlMatrix4 s_worldVisibilityIdentityMatrix;

nlMatrix4* WorldVisibilityDrawable::GetWorldMatrix()
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

void WorldVisibilityDrawable::Draw()
{
    GLView* pOpaqueView = m_pWorld->m_pOpaqueView;
    GLView* pAlphaView = m_pWorld->m_pAlphaView;
    if (pAlphaView == 0)
    {
        pAlphaView = pOpaqueView;
    }

    for (unsigned long packetIndex = 0;
         packetIndex < GetModel()->numPackets; ++packetIndex)
    {
        glModelPacket* pPacket = &GetModel()->packets[packetIndex];
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

bool WorldVisibilityDrawable::IsVisible()
{
    return m_pVisibilityNode->mVisible == 1;
}

void InitializeWorldPhysicsDrawable(
    WorldPhysicsDrawable* pDrawable,
    WorldObjectLoadContext*)
{
    pDrawable->m_pPhysicsObject = CreatePhysicsPrimitive(
        &pDrawable->m_Description, 0);
}

void WorldPhysicsDrawable::ReleaseResources()
{
    ReleaseWorldPhysicsObject(this);
}

WorldVisibilityDrawable::~WorldVisibilityDrawable()
{
}

void WorldVisibilityDrawable::SetWorldMatrix(const nlMatrix4&)
{
}

void WorldVisibilityDrawable::ReleaseResources()
{
}
