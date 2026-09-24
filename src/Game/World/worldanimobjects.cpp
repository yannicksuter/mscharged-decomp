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

struct WorldPhysicsOwner_80342170
{
    u8 m_pad00[0x80];
    PhysicsObject* m_pPhysicsObject;
};

struct WorldAnimBinding_803438FC
{
    unsigned long m_uDrawableHash;
    unsigned long m_uNodeHash;
};

WorldVisibilityNode* FindWorldVisibilityNode(
    WorldVertexAnimDrawable_80343E3C*, WorldVisibilityNode*);

extern "C" PhysicsObject* fn_80341EEC(
    WorldPhysicsDescription_80341EEC* pDescription,
    CollisionSpace* pCollisionSpace);
extern "C" void fn_80342170(WorldPhysicsOwner_80342170* pOwner);
extern "C" EffectsGroup* fn_802E7D54(
    EmissionManager*, unsigned long);

extern "C" void fn_803437C8(WorldAnimObject_803437C8* pObject,
    WorldObjectLoadContext* pContext)
{
    WorldAnimManager* pManager
        = &pContext->m_pWorld->mWorldAnimManager;
    pObject->m_pAnimController
        = pManager->GetOrCreateController(pObject->m_uHashID);

    u8* pData = pContext->GetParentData();
    pObject->m_pAnimationHashes = (unsigned long*)(pData
        + pObject->m_nBindings * sizeof(WorldAnimBinding_803438FC));
    pObject->m_pAnimController->m_pWorldAnimObject = pObject;

    pManager->BindHierarchy(
        pObject->m_pAnimController, pObject->m_uHierarchyHash);
    pObject->m_pAnimController->SetWorldMatrix(
        *((DrawableObject*)pObject)->GetWorldMatrix());

    if (pObject->m_nAnimations == 0)
    {
        pObject->m_pAnimController->SetAnimation(
            nlStringLowerHash("idle"), pObject->m_ePlayMode);
    }
    else if (pObject->m_nAnimations == 1)
    {
        pObject->m_pAnimController->SetAnimation(
            pObject->m_pAnimationHashes[0], pObject->m_ePlayMode);
    }
    else
    {
        unsigned long nAnimation
            = nlRandom(pObject->m_nAnimations, &nlDefaultSeed) - 1;
        pObject->m_pAnimController->SetAnimation(
            pObject->m_pAnimationHashes[nAnimation], PM_HOLD);
    }

    pObject->m_pAnimController->SetAnimationSpeed(
        pObject->m_fAnimationSpeed);
    pObject->m_pAnimController->SetAnimationTime(
        pObject->m_fAnimationTime);
    pObject->m_pBindings
        = (WorldAnimBinding_803438FC*)pContext->GetParentData();
}

extern "C" void fn_803438FC(WorldAnimObject_803437C8* pObject)
{
    if (pObject->m_pBindings != 0)
    {
        for (int i = 0; i < pObject->m_nBindings; ++i)
        {
            WorldAnimDrawable_80343A40* pDrawable
                = (WorldAnimDrawable_80343A40*)pObject->m_pWorld
                      ->FindDrawableObject(
                          pObject->m_pBindings[i].m_uDrawableHash);
            if (pDrawable != 0)
            {
                pDrawable->m_nAnimNode
                    = pObject->m_pAnimController->GetNodeIndexByID(
                        pObject->m_pBindings[i].m_uNodeHash);
                pDrawable->m_pAnimController
                    = pObject->m_pAnimController;
            }
        }
    }
}

extern "C" void fn_803439A4(WorldAnimObject_803437C8* pObject)
{
    if (pObject->m_nAnimations > 1)
    {
        unsigned long nAnimation
            = nlRandom(pObject->m_nAnimations, &nlDefaultSeed) - 1;
        pObject->m_pAnimController->SetAnimation(
            pObject->m_pAnimationHashes[nAnimation], PM_HOLD);
    }
}

extern "C" void* fn_80343A00(void* pObject, int shouldDelete)
{
    if (pObject != 0 && shouldDelete > 0)
    {
        ::operator delete(pObject);
    }
    return pObject;
}

extern "C" nlMatrix4* fn_80343A40(
    WorldAnimDrawable_80343A40* pObject)
{
    return &pObject->m_transform;
}

extern "C" void fn_80343A48(WorldAnimDrawable_80343A40* pObject,
    const nlMatrix4* pTransform)
{
    pObject->m_transform = *pTransform;
}

extern "C" void fn_80343ADC(void*)
{
}

static nlMatrix4 s_worldAnimIdentityMatrix;

extern "C" nlMatrix4* fn_80343AE0(void*)
{
    s_worldAnimIdentityMatrix.SetIdentity();
    return &s_worldAnimIdentityMatrix;
}

nlMatrix4* WorldDrawable::GetWorldMatrix()
{
    if (m_pAnimController != 0)
    {
        return &m_pAnimController->GetNodeMatrix(m_nAnimNode);
    }
    return &mWorldMatrix;
}

bool WorldDrawable::V6(const nlVector4* pCullData) const
{
    FrustumResult result;
    if (m_pAnimController != 0)
    {
        if (m_pAnimController->GetMorphWeight(m_nAnimNode) == 0.0f)
        {
            return false;
        }

        float fRadius = m_fBoundingRadius;
        nlMatrix4& matrix
            = m_pAnimController->GetNodeMatrix(m_nAnimNode);
        result = ClassifySphereInFrustum(pCullData,
            (const nlVector3*)&matrix.e2[3][0], fRadius);
    }
    else
    {
        float fRadius = m_fBoundingRadius;
        nlMatrix4& matrix
            = *const_cast<WorldDrawable*>(this)->GetWorldMatrix();
        result = ClassifySphereInFrustum(pCullData,
            (const nlVector3*)&matrix.e2[3][0], fRadius);
    }
    return result != FRUSTUM_OUTSIDE;
}

extern "C" void fn_80343C00(WorldAnimDrawable_80343A40* pObject)
{
    ((DrawableObject*)pObject)->V8(0);
}

void WorldDrawable::V8(GLView* pView)
{
    unsigned long uAnimationHash = *(unsigned long*)m_pModel;
    GLVertexAnim* pVertexAnim
        = glGetCurrentResourcePool()->m_inventory->GetVertexAnim(uAnimationHash);
    glModel* pModel = m_pModel;
    if (pVertexAnim != 0)
    {
        int nFrames = (int)pVertexAnim->m_nNumFrames;
        float fNumFrames = (float)nFrames;
        float fDuration = fNumFrames / 30.0f;
        float fFrameTime
            = m_pWorldContext->mWorldAnimManager.m_fTime
            / fDuration;
        float fFrameFraction
            = fFrameTime - (float)floor(fFrameTime);
        int nFrame = (int)(fNumFrames * fFrameFraction);
        pModel = pVertexAnim->GetModel(nFrame);
        V7(pModel);
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
    GLView* pAlphaView = m_pWorldContext->m_pAlphaView;
    if (pAlphaView == 0)
    {
        pAlphaView = pView;
    }

    for (unsigned long i = 0; i < pModel->numPackets; ++i)
    {
        glModelPacket* pPacket = &pModel->packets[i];
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

extern "C" void fn_80343DE4(WorldAnimDrawable_80343A40* pObject,
    WorldObjectLoadContext* pContext)
{
    glModel*& pMaterial = pObject->m_pModel;
    pContext->m_pWorld->ResolveModel(pMaterial);
    pObject->m_pModel = glModelDupNoStreams(
        (glModel*)pObject->m_pModel, true,
        pContext->m_pWorld->m_pResource);
}

void CreateWorldVertexAnimDrawable(
    WorldVertexAnimDrawable_80343E3C* pObject,
    WorldObjectLoadContext* pContext)
{
    glModel*& pMaterial = pObject->m_pModel;
    pContext->m_pWorld->ResolveModel(pMaterial);
    pObject->m_pModel = glModelDupNoStreams(pObject->GetModel(),
        true, pContext->m_pWorld->m_pResource);

    pObject->m_pVertexAnimNode = FindWorldVisibilityNode(
        pObject, pContext->m_pWorld->m_pVisibilityTree);
}

WorldVisibilityNode* FindWorldVisibilityNode(
    WorldVertexAnimDrawable_80343E3C* pObject,
    WorldVisibilityNode* pNode)
{
    int i;
    for (i = 0; i < pNode->mNumModelHashes; ++i)
    {
        if (pNode->mModelHashes[i] == pObject->GetModel()->id)
        {
            return pNode;
        }
    }

    for (i = 0; i < 2; ++i)
    {
        if (pNode->mChildren[i] != 0)
        {
            WorldVisibilityNode* pFound = FindWorldVisibilityNode(
                pObject, pNode->mChildren[i]);
            if (pFound != 0)
            {
                return pFound;
            }
        }
    }
    return 0;
}

extern "C" void fn_80344088(
    WorldVertexAnimDrawable_80343E3C* pObject)
{
    GLView* pOpaqueView = pObject->m_pWorld->m_pOpaqueView;
    GLView* pAlphaView = pObject->m_pWorld->m_pAlphaView;
    if (pAlphaView == 0)
    {
        pAlphaView = pOpaqueView;
    }

    for (unsigned long i = 0; i < pObject->GetModel()->numPackets; ++i)
    {
        glModelPacket* pPacket = &pObject->GetModel()->packets[i];
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

bool IsWorldVertexAnimDrawableVisible(
    WorldVertexAnimDrawable_80343E3C* pObject)
{
    return pObject->m_pVertexAnimNode->mVisible == 1;
}

extern "C" void fn_80344144(
    WorldPhysicsDrawable_80534448* pObject,
    WorldObjectLoadContext*)
{
    pObject->m_pPhysicsObject = fn_80341EEC(
        &pObject->m_Description, 0);
}

extern "C" void fn_8034417C(WorldPhysicsOwner_80342170* pObject)
{
    fn_80342170(pObject);
}

extern "C" void* fn_80344180(void* pObject, int shouldDelete)
{
    if (pObject != 0 && shouldDelete > 0)
    {
        ::operator delete(pObject);
    }
    return pObject;
}

extern "C" void fn_803441C0(void*)
{
}

extern "C" void fn_803441C4(void*)
{
}

static bool s_drawEffectBounds;

extern "C" void fn_803441C8(WorldEffect* pEffect,
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

extern "C" void fn_80344218(WorldEffect* pEffect)
{
    nlDLListIterator<EmissionController*> iterator
        = EmissionManager::Instance()->GetContainer()->Begin();
    DLListEntry<EmissionController*>* head = iterator.m_Head;
    DLListEntry<EmissionController*>* current = iterator.m_Curr;
    while (current != 0)
    {
        EmissionController* pController = current->entry;
        if (pController->m_uUserData == (u32)pEffect)
        {
            pController->ClearParticles();
            pController->mUpdateCallback.Clear();
        }
        if (nlDLRingIsEnd(head, current) || current == 0)
        {
            current = 0;
        }
        else
        {
            current = current->m_next;
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

extern "C" void fn_8034470C(EmissionController& controller);
extern "C" void fn_80344798(EmissionController& controller);

void WorldEffect::Emit()
{
    EffectsGroup* pGroup
        = fn_802E7D54(EmissionManager::Instance(),
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
                Function1<void, EmissionController&>(fn_8034470C));
            pController->m_uUserData = (u32)this;
        }
        else
        {
            pController->SetUpdateCallback(
                Function1<void, EmissionController&>(fn_80344798));
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

extern "C" void fn_8034470C(EmissionController& controller)
{
    WorldEffect* pEffect
        = (WorldEffect*)controller.m_uUserData;
    if (pEffect != 0
        && pEffect->m_pAnimController->GetAnimationTime() != 0.0f)
    {
        nlMatrix4& matrix
            = pEffect->m_pAnimController->GetNodeMatrix(
                pEffect->m_nAnimNode);
        controller.SetPosition(*(nlVector3*)&matrix.e2[3][0]);
        controller.SetDirection(*(nlVector3*)&matrix.e2[2][0]);
        pEffect->UpdateVisibility(&controller);
    }
}

extern "C" void fn_80344798(EmissionController& controller)
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

extern "C" void* fn_803447B4(void* pObject, int shouldDelete)
{
    if (pObject != 0 && shouldDelete > 0)
    {
        ::operator delete(pObject);
    }
    return pObject;
}
