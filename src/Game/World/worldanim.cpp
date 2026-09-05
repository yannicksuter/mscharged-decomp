#include "Game/World/worldanim.h"

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
#include "NL/gl/glMemory.h"
#include "NL/gl/glModel.h"
#include "NL/gl/glState.h"
#include "NL/gl/glView.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"

#include <math.h>

extern "C" void fn_8030B038(cPoseAccumulator*, const cPoseNode*,
    const nlMatrix4*);
extern "C" void fn_803438FC(WorldAnimObject_803437C8*);
extern "C" void fn_803439A4(WorldAnimObject_803437C8*);
extern "C" u8* fn_80340550(WorldObjectLoadContext_8034136C*);
extern "C" void fn_802E4358(EmissionController*);
extern "C" float fn_802E5A68(EmissionController*);
extern "C" EffectsGroup* fn_802E7D54(
    EmissionManager*, unsigned long);
extern "C" EmissionController* fn_802E7FE4(EmissionManager*,
    EffectsGroup*, int, bool, bool);
extern "C" bool fn_802DD1EC(
    const void*, const nlVector3&, float);

struct WorldPhysicsDescription_80341EEC
{
    /* 0x00 */ nlMatrix4 m_transform;
    /* 0x40 */ unsigned long m_uType;
    /* 0x44 */ float m_f44;
    /* 0x48 */ float m_f48;
    /* 0x4C */ float m_f4C;
    /* 0x50 */ float m_f50;
};

struct WorldPhysicsOwner_80342170
{
    u8 m_pad00[0x80];
    PhysicsObject* m_pPhysicsObject;
};

struct WorldAnimDrawable_80343A40
{
    u8 m_pad00[0x10];
    World* m_pWorld;
    int m_nAnimNode;
    WorldAnimController* m_pAnimController;
    u8 m_pad1C[0x04];
    nlMatrix4 m_transform;
    float m_fRadius;
    void* m_pModel;
    u8 m_pad68[0x0C];
    void* m_pEmissionController;
    u8 m_pad78[0x08];
    PhysicsObject* m_pPhysicsObject;
};

struct WorldAnimBinding_803438FC
{
    unsigned long m_uDrawableHash;
    unsigned long m_uNodeHash;
};

class WorldAnimObject_803437C8
{
public:
    /* 0x00 */ void* m_pVTable;
    /* 0x04 */ unsigned long m_uHashID;
    /* 0x08 */ u8 m_pad08[0x08];
    /* 0x10 */ World* m_pWorld;
    /* 0x14 */ int m_nAnimNode;
    /* 0x18 */ WorldAnimController* m_pAnimController;
    /* 0x1C */ u8 m_pad1C[0x44];
    /* 0x60 */ int m_nBindings;
    /* 0x64 */ unsigned long m_uHierarchyHash;
    /* 0x68 */ WorldAnimBinding_803438FC* m_pBindings;
    /* 0x6C */ u8 m_pad6C[0x04];
    /* 0x70 */ int m_nAnimations;
    /* 0x74 */ unsigned long* m_pAnimationHashes;
    /* 0x78 */ u8 m_pad78[0x08];
    /* 0x80 */ ePlayMode m_ePlayMode;
    /* 0x84 */ float m_fAnimationSpeed;
    /* 0x88 */ float m_fAnimationTime;
};

class WorldAnimEffect_803441C8
{
public:
    /* 0x00 */ void* m_pVTable;
    /* 0x04 */ u8 m_pad04[0x0C];
    /* 0x10 */ World* m_pWorld;
    /* 0x14 */ int m_nAnimNode;
    /* 0x18 */ WorldAnimController* m_pAnimController;
    /* 0x1C */ u8 m_pad1C[0x44];
    /* 0x60 */ float m_fEmissionInterval;
    /* 0x64 */ u8 m_pad64[0x04];
    /* 0x68 */ float m_fEmissionRadius;
    /* 0x6C */ u8 m_pad6C[0x04];
    /* 0x70 */ unsigned long m_uEffectHash;
    /* 0x74 */ unsigned long m_uProbability;
    /* 0x78 */ int m_nEmissionCount;
    /* 0x7C */ int m_nTimingMode;
    /* 0x80 */ float m_fEmissionTime;
    /* 0x84 */ float m_fPreviousEmissionTime;
    /* 0x88 */ u8 m_pad88[0x08];
    /* 0x90 */ int m_nRemainingEmissions;
    /* 0x94 */ int m_nEmissionID;
    /* 0x98 */ int m_bActive;
    /* 0x9C */ bool m_bAlwaysVisible;
};

struct WorldVertexAnimNode_80343F78
{
    /* 0x00 */ u8 m_pad00[0x1C];
    /* 0x1C */ int m_nModelHashes;
    /* 0x20 */ unsigned long* m_pModelHashes;
    /* 0x24 */ WorldVertexAnimNode_80343F78* m_pChildren[2];
};

struct WorldVertexAnimDrawable_80343E3C
{
    /* 0x00 */ u8 m_pad00[0x10];
    /* 0x10 */ World* m_pWorld;
    /* 0x14 */ u8 m_pad14[0x0C];
    /* 0x20 */ glModel* m_pModel;
    /* 0x24 */ WorldVertexAnimNode_80343F78* m_pVertexAnimNode;
};

extern "C" WorldVertexAnimNode_80343F78* fn_80343F78(
    WorldVertexAnimDrawable_80343E3C*, WorldVertexAnimNode_80343F78*);

extern "C" void fn_80344404(WorldAnimEffect_803441C8*);
extern "C" void fn_803445F0(
    WorldAnimEffect_803441C8*, EmissionController*);

struct WorldAnimState_8034412C
{
    u8 m_pad00[0x24];
    struct State
    {
        u8 m_pad00[0x18];
        unsigned long m_uState;
    }* m_pState;
};

extern "C" void fn_80341EE8(void*)
{
}

extern "C" PhysicsObject* fn_80341EEC(
    WorldPhysicsDescription_80341EEC* pDescription,
    CollisionSpace* pCollisionSpace)
{
    PhysicsObject* pPhysicsObject = 0;
    switch (pDescription->m_uType)
    {
    case 0:
        pPhysicsObject
            = new (nlMalloc(sizeof(PhysicsBox), 8, false)) PhysicsBox(
                pCollisionSpace, 0, pDescription->m_f48,
                pDescription->m_f44, pDescription->m_f4C);
        pPhysicsObject->SetWorldMatrix(pDescription->m_transform);
        break;
    case 1:
        pPhysicsObject
            = new (nlMalloc(sizeof(PhysicsSphere), 8, false)) PhysicsSphere(
                pCollisionSpace, 0, pDescription->m_f50);
        pPhysicsObject->SetWorldMatrix(pDescription->m_transform);
        break;
    case 2:
        pPhysicsObject
            = new (nlMalloc(sizeof(PhysicsCapsule), 8, false))
                PhysicsCapsule(pCollisionSpace, 0,
                    pDescription->m_f50, pDescription->m_f4C);
        pPhysicsObject->SetWorldMatrix(pDescription->m_transform);
        break;
    case 4:
    {
        const float* m = (const float*)&pDescription->m_transform;
        nlVector3 position = { m[12], m[13], m[14] };
        nlVector3 axis0 = { 0.5f * pDescription->m_f44 * m[0],
            0.5f * pDescription->m_f44 * m[1],
            0.5f * pDescription->m_f44 * m[2] };
        nlVector3 axis1 = { 0.5f * pDescription->m_f48 * m[4],
            0.5f * pDescription->m_f48 * m[5],
            0.5f * pDescription->m_f48 * m[6] };
        pPhysicsObject
            = new (nlMalloc(sizeof(PhysicsFinitePlane), 8, false))
                PhysicsFinitePlane(pCollisionSpace, position, axis0,
                    axis1, true, -1.0f);
        break;
    }
    case 6:
    {
        const float* m = (const float*)&pDescription->m_transform;
        float distance
            = m[8] * m[12] + m[9] * m[13] + m[10] * m[14];
        pPhysicsObject
            = new (nlMalloc(sizeof(PhysicsPlane), 8, false)) PhysicsPlane(
                pCollisionSpace, m[8], m[9], m[10], distance);
        break;
    }
    }

    pPhysicsObject->SetCategory(0xFF);
    pPhysicsObject->SetCollide(0xFF);
    return pPhysicsObject;
}

extern "C" void fn_80342170(WorldPhysicsOwner_80342170* pOwner);

extern "C" void fn_8034417C(WorldPhysicsOwner_80342170* pObject)
{
    fn_80342170(pObject);
}

extern "C" void fn_80342170(WorldPhysicsOwner_80342170* pOwner)
{
    if (pOwner->m_pPhysicsObject != 0)
    {
        delete pOwner->m_pPhysicsObject;
        pOwner->m_pPhysicsObject = 0;
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

extern "C" void fn_803437C8(WorldAnimObject_803437C8* pObject,
    WorldObjectLoadContext_8034136C* pContext)
{
    WorldAnimManager* pManager
        = &pContext->m_pWorld->mWorldAnimManager_02C;
    pObject->m_pAnimController
        = pManager->fn_803426D8(pObject->m_uHashID);

    u8* pData = fn_80340550(pContext);
    pObject->m_pAnimationHashes = (unsigned long*)(pData
        + pObject->m_nBindings * sizeof(WorldAnimBinding_803438FC));
    pObject->m_pAnimController->m_pWorldAnimObject = pObject;

    pManager->fn_80342630(
        pObject->m_pAnimController, pObject->m_uHierarchyHash);
    pObject->m_pAnimController->SetWorldMatrix(
        ((DrawableObject*)pObject)->GetWorldMatrix());

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
        = (WorldAnimBinding_803438FC*)fn_80340550(pContext);
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

extern "C" nlMatrix4* fn_80343B14(
    WorldAnimDrawable_80343A40* pObject)
{
    if (pObject->m_pAnimController != 0)
    {
        return &pObject->m_pAnimController->GetNodeMatrix(
            pObject->m_nAnimNode);
    }
    return &pObject->m_transform;
}

extern "C" bool fn_80343B34(WorldAnimDrawable_80343A40* pObject,
    const void* pCullData)
{
    if (pObject->m_pAnimController != 0)
    {
        if (pObject->m_pAnimController->GetMorphWeight(
                pObject->m_nAnimNode)
            == 0.0f)
        {
            return false;
        }

        float fRadius = pObject->m_fRadius;
        nlMatrix4& matrix
            = pObject->m_pAnimController->GetNodeMatrix(
                pObject->m_nAnimNode);
        return fn_802DD1EC(pCullData,
            *(nlVector3*)&matrix.e2[3][0], fRadius);
    }

    float fRadius = pObject->m_fRadius;
    nlMatrix4& matrix
        = ((DrawableObject*)pObject)->GetWorldMatrix();
    return fn_802DD1EC(pCullData,
        *(nlVector3*)&matrix.e2[3][0], fRadius);
}

extern "C" void fn_80343C00(WorldAnimDrawable_80343A40* pObject)
{
    ((DrawableObject*)pObject)->V8(0);
}

extern "C" void fn_80343C14(
    WorldAnimDrawable_80343A40* pObject, GLView* pView)
{
    unsigned long uAnimationHash
        = *(unsigned long*)pObject->m_pModel;
    GLVertexAnim* pVertexAnim
        = (GLVertexAnim*)fn_802CC094()->m_inventory
              ->GetModel(uAnimationHash);
    glModel* pModel = (glModel*)pObject->m_pModel;
    if (pVertexAnim != 0)
    {
        int nFrames = (int)pVertexAnim->m_nNumFrames;
        float fNumFrames = (float)nFrames;
        float fDuration = fNumFrames / 30.0f;
        float fFrameTime
            = pObject->m_pWorld->mWorldAnimManager_02C.m_fTime
            / fDuration;
        float fFrameFraction
            = fFrameTime - (float)floor(fFrameTime);
        int nFrame = (int)(fNumFrames * fFrameFraction);
        pModel = pVertexAnim->GetModel(nFrame);
        ((DrawableObject*)pObject)->V7(pModel);
    }

    if (pObject->m_pAnimController != 0)
    {
        glModelSetMatrix(pModel,
            pObject->m_pAnimController->GetNodeMatrix(
                pObject->m_nAnimNode));
    }
    else
    {
        glModelSetMatrix(pModel,
            ((DrawableObject*)pObject)->GetWorldMatrix());
    }

    GLView* pOpaqueView = pObject->m_pWorld->m_pView68;
    GLView* pAlphaView = pObject->m_pWorld->m_pView6C;
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
    WorldObjectLoadContext_8034136C* pContext)
{
    GLMaterialList*& pMaterial
        = *(GLMaterialList**)&pObject->m_pModel;
    pContext->m_pWorld->fn_803418C4(pMaterial);
    pObject->m_pModel = glModelDupNoStreams(
        (glModel*)pObject->m_pModel, true,
        pContext->m_pWorld->m_pResource);
}

extern "C" void fn_80343E3C(
    WorldVertexAnimDrawable_80343E3C* pObject,
    WorldObjectLoadContext_8034136C* pContext)
{
    GLMaterialList*& pMaterial
        = *(GLMaterialList**)&pObject->m_pModel;
    pContext->m_pWorld->fn_803418C4(pMaterial);
    pObject->m_pModel = glModelDupNoStreams(pObject->m_pModel,
        true, pContext->m_pWorld->m_pResource);

    WorldVertexAnimNode_80343F78* pNode
        = (WorldVertexAnimNode_80343F78*)pContext->m_pWorld
              ->m_pRenderable;
    unsigned long uModelHash = pObject->m_pModel->unknown00;
    for (int i = 0; i < pNode->m_nModelHashes; ++i)
    {
        if (pNode->m_pModelHashes[i] == uModelHash)
        {
            pObject->m_pVertexAnimNode = pNode;
            return;
        }
    }

    WorldVertexAnimNode_80343F78* pFound = 0;
    for (int i = 0; i < 2 && pFound == 0; ++i)
    {
        WorldVertexAnimNode_80343F78* pChild
            = pNode->m_pChildren[i];
        if (pChild == 0)
        {
            continue;
        }

        for (int j = 0; j < pChild->m_nModelHashes; ++j)
        {
            if (pChild->m_pModelHashes[j] == uModelHash)
            {
                pFound = pChild;
                break;
            }
        }

        for (int j = 0; j < 2 && pFound == 0; ++j)
        {
            if (pChild->m_pChildren[j] != 0)
            {
                pFound = fn_80343F78(
                    pObject, pChild->m_pChildren[j]);
            }
        }
    }
    pObject->m_pVertexAnimNode = pFound;
}

extern "C" WorldVertexAnimNode_80343F78* fn_80343F78(
    WorldVertexAnimDrawable_80343E3C* pObject,
    WorldVertexAnimNode_80343F78* pNode)
{
    unsigned long uModelHash = pObject->m_pModel->unknown00;
    for (int i = 0; i < pNode->m_nModelHashes; ++i)
    {
        if (pNode->m_pModelHashes[i] == uModelHash)
        {
            return pNode;
        }
    }

    for (int i = 0; i < 2; ++i)
    {
        WorldVertexAnimNode_80343F78* pChild
            = pNode->m_pChildren[i];
        if (pChild != 0)
        {
            WorldVertexAnimNode_80343F78* pFound
                = fn_80343F78(pObject, pChild);
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
    GLView* pOpaqueView = pObject->m_pWorld->m_pView68;
    GLView* pAlphaView = pObject->m_pWorld->m_pView6C;
    if (pAlphaView == 0)
    {
        pAlphaView = pOpaqueView;
    }

    for (unsigned long i = 0; i < pObject->m_pModel->numPackets; ++i)
    {
        glModelPacket* pPacket = &pObject->m_pModel->packets[i];
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

extern "C" bool fn_8034412C(WorldAnimState_8034412C* pObject)
{
    return pObject->m_pState->m_uState == 1;
}

extern "C" void fn_80344144(
    WorldAnimDrawable_80343A40* pObject, void*)
{
    pObject->m_pPhysicsObject = fn_80341EEC(
        (WorldPhysicsDescription_80341EEC*)&pObject->m_transform, 0);
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
static const nlColour s_effectBoundsColour
    = { 0xFF, 0xFF, 0x80, 0xFF };

extern "C" void fn_803441C8(WorldAnimEffect_803441C8* pEffect,
    WorldObjectLoadContext_8034136C* pContext)
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
    pContext->m_pWorld->fn_80341D40(
        (WorldEffect_80341D40*)pEffect);
}

extern "C" void fn_80344218(WorldAnimEffect_803441C8* pEffect)
{
    nlDLListIterator<EmissionController*> iterator
        = EmissionManager::Instance()->GetContainer()->Begin();
    while (iterator.hasNext())
    {
        EmissionController* pController = *iterator;
        if (pController->m_uUserData == (u32)pEffect)
        {
            fn_802E4358(pController);
            pController->mUpdateCallback.Clear();
        }
        iterator.Step();
    }
}

extern "C" void fn_80344308(
    WorldAnimEffect_803441C8* pEffect, float fDeltaT)
{
    if (fDeltaT != 0.0f && pEffect->m_bActive)
    {
        bool bEmit = false;
        if (pEffect->m_nTimingMode == 0)
        {
            if (pEffect->m_nRemainingEmissions > 0
                || pEffect->m_nRemainingEmissions == -1)
            {
                float fEmissionTime
                    = pEffect->m_fEmissionTime + fDeltaT;
                pEffect->m_fEmissionTime = fEmissionTime;
                if (pEffect->m_fEmissionInterval <= fEmissionTime
                    && nlRandom(100, &nlDefaultSeed)
                        < pEffect->m_uProbability)
                {
                    bEmit = true;
                }
            }
        }
        else if ((pEffect->m_nRemainingEmissions > 0
                     || pEffect->m_nRemainingEmissions == -1)
            && pEffect->m_fEmissionInterval
                <= pEffect->m_fEmissionTime
            && nlRandom(100, &nlDefaultSeed)
                < pEffect->m_uProbability)
        {
            bEmit = true;
        }

        if (bEmit)
        {
            fn_80344404(pEffect);
        }
    }
}

extern "C" void fn_8034470C(EmissionController& controller);
extern "C" void fn_80344798(EmissionController& controller);

extern "C" void fn_80344404(WorldAnimEffect_803441C8* pEffect)
{
    EffectsGroup* pGroup
        = fn_802E7D54(EmissionManager::Instance(),
            pEffect->m_uEffectHash);
    if (pGroup != 0)
    {
        EmissionController* pController
            = fn_802E7FE4(EmissionManager::Instance(), pGroup,
                1, true, false);
        pEffect->m_fEmissionRadius = fn_802E5A68(pController);

        nlVector3 velocity = { 0.0f, 0.0f, 0.0f };
        pController->SetVelocity(velocity);
        pController->m_fGround = 0.02f;

        nlMatrix4* pMatrix
            = &((DrawableObject*)pEffect)->GetWorldMatrix();
        pController->SetPosition(
            *(nlVector3*)&pMatrix->e2[3][0]);
        pMatrix = &((DrawableObject*)pEffect)->GetWorldMatrix();
        nlVector3 direction
            = { pMatrix->e2[2][0], pMatrix->e2[2][1],
                  pMatrix->e2[2][2] };
        pController->SetDirection(direction);

        if (pEffect->m_pAnimController != 0)
        {
            Function1<void, EmissionController&> callback(
                fn_8034470C);
            pController->SetUpdateCallback(callback);
        }
        else
        {
            Function1<void, EmissionController&> callback(
                fn_80344798);
            pController->SetUpdateCallback(callback);
        }
        pController->m_uUserData = (u32)pEffect;
        pEffect->m_nEmissionID = pController->m_Id;
    }
    else
    {
        pEffect->m_nEmissionID = -1;
    }

    pEffect->m_fPreviousEmissionTime = pEffect->m_fEmissionTime;
    pEffect->m_fEmissionTime = 0.0f;
    --pEffect->m_nRemainingEmissions;
    if (pEffect->m_nRemainingEmissions < -1)
    {
        pEffect->m_nRemainingEmissions = -1;
    }
}

extern "C" void fn_8034470C(EmissionController& controller)
{
    WorldAnimEffect_803441C8* pEffect
        = (WorldAnimEffect_803441C8*)controller.m_uUserData;
    if (pEffect != 0
        && pEffect->m_pAnimController->GetAnimationTime() != 0.0f)
    {
        nlMatrix4& matrix
            = pEffect->m_pAnimController->GetNodeMatrix(
                pEffect->m_nAnimNode);
        controller.SetPosition(*(nlVector3*)&matrix.e2[3][0]);
        controller.SetDirection(*(nlVector3*)&matrix.e2[2][0]);
        fn_803445F0(pEffect, &controller);
    }
}

extern "C" void fn_80344798(EmissionController& controller)
{
    WorldAnimEffect_803441C8* pEffect
        = (WorldAnimEffect_803441C8*)controller.m_uUserData;
    if (pEffect != 0)
    {
        fn_803445F0(pEffect, &controller);
    }
}

extern "C" void fn_803445F0(WorldAnimEffect_803441C8* pEffect,
    EmissionController* pController)
{
    bool bVisible;
    if (!pEffect->m_bActive)
    {
        bVisible = false;
    }
    else if (pEffect->m_bAlwaysVisible == true)
    {
        bVisible = true;
    }
    else
    {
        const nlVector4* pCullData = pEffect->m_pWorld->m_pView68
                                         ->m_Interface->GetShadowMatrix();
        bVisible = fn_802DD1EC(pCullData,
                       pController->GetPosition(),
                       pEffect->m_fEmissionRadius)
            && pEffect->m_pWorld->m_bRenderingEnabled;
    }
    pController->m_bVisible = bVisible;

    if (s_drawEffectBounds)
    {
        g_ShapeRenderer.DrawSphere(pController->GetPosition(),
            s_effectBoundsColour, pEffect->m_fEmissionRadius);
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

static WorldAnimController* spCurrentWorldAnimController;

class WorldAnimUpdate_80342D7C
{
public:
    void Update(const unsigned long& uHashID,
        WorldAnimController** ppController);

    float m_fDeltaT;
};

WorldAnimManager::WorldAnimManager()
    : m_fTime(0.0f)
{
    m_pHierarchyInventory
        = new (nlMalloc(sizeof(cInventory<cSHierarchy>), 8, false))
            cInventory<cSHierarchy>();
}

WorldAnimManager::~WorldAnimManager()
{
}

void WorldAnimManager::fn_80342324()
{
}

void WorldAnimManager::fn_80342328()
{
    delete m_pHierarchyInventory;
    m_pHierarchyInventory = 0;
    m_animationSetMap.DeleteValues();
    m_animationControllerMap.DeleteValues();
}

void WorldAnimManager::fn_80342630(
    WorldAnimController* pController, unsigned long uHierarchyHash)
{
    AnimationSet** ppAnimationSet;
    pController->m_pAnimationSet
        = m_animationSetMap.FindGet(uHierarchyHash, &ppAnimationSet)
        ? *ppAnimationSet
        : 0;

    pController->m_pPoseAccumulator
        = new (nlMalloc(sizeof(cPoseAccumulator), 8, false))
            cPoseAccumulator(
                pController->m_pAnimationSet->m_pHierarchy, false);
}

WorldAnimController* WorldAnimManager::fn_803426D8(
    unsigned long uHashID)
{
    WorldAnimController** ppController;
    if (m_animationControllerMap.FindGet(uHashID, &ppController))
    {
        return *ppController;
    }

    WorldAnimController* pController
        = new (nlMalloc(sizeof(WorldAnimController), 8, false))
            WorldAnimController();
    m_animationControllerMap.Add(uHashID, pController);
    return pController;
}

WorldAnimController* WorldAnimManager::fn_803427B0(
    unsigned long uHashID)
{
    WorldAnimController** ppController;
    if (m_animationControllerMap.FindGet(uHashID, &ppController))
    {
        return *ppController;
    }
    return 0;
}

AnimationSet* WorldAnimManager::LoadHierarchy(nlChunk* pChunk)
{
    m_pHierarchyInventory->ParseChunks(pChunk, pChunk->GetNextChunk());
    cSHierarchy* pHierarchy = m_pHierarchyInventory->Find(0);

    AnimationSet** ppAnimationSet;
    AnimationSet* pAnimationSet;
    if (m_animationSetMap.FindGet(
            pHierarchy->GetHashID(), &ppAnimationSet))
    {
        pAnimationSet = *ppAnimationSet;
    }
    else
    {
        pAnimationSet
            = new (nlMalloc(sizeof(AnimationSet), 8, false))
                AnimationSet();
        m_animationSetMap.Add(pHierarchy->GetHashID(), pAnimationSet);
    }

    pAnimationSet->m_pHierarchy = pHierarchy;
    return pAnimationSet;
}

void WorldAnimManager::LoadAnimationSet(
    AnimationSet* pAnimationSet, nlChunk* pChunk)
{
    pAnimationSet->m_animInventory.ParseChunks(
        pChunk, pChunk->GetNextChunk());
}

void WorldAnimManager::fn_80342A74()
{
    m_animationControllerMap.Walk(
        this, &WorldAnimManager::fn_80342BDC);
}

void WorldAnimManager::fn_80342BDC(const unsigned long&,
    WorldAnimController** ppController)
{
    fn_803438FC((*ppController)->m_pWorldAnimObject);
}

void WorldAnimManager::fn_80342BE8(float fDeltaT)
{
    WorldAnimUpdate_80342D7C update;
    update.m_fDeltaT = fDeltaT;
    m_animationControllerMap.Walk(
        &update, &WorldAnimUpdate_80342D7C::Update);
    m_fTime += fDeltaT;
}

void WorldAnimUpdate_80342D7C::Update(const unsigned long&,
    WorldAnimController** ppController)
{
    WorldAnimController* pController = *ppController;
    if (pController->m_pPoseTree != 0)
    {
        spCurrentWorldAnimController = pController;
        pController->m_pPoseTree->Update(m_fDeltaT);
        if (pController->m_pPoseTree->m_fTime
            != pController->m_pPoseTree->m_fPrevTime)
        {
            fn_8030B038(pController->m_pPoseAccumulator,
                pController->m_pPoseTree, &pController->m_worldMatrix);
        }
        if (pController->m_pPoseTree->m_ePlayMode == PM_HOLD
            && pController->m_pPoseTree->m_fTime == 1.0f
            && pController->m_pWorldAnimObject != 0)
        {
            fn_803439A4(pController->m_pWorldAnimObject);
        }
        spCurrentWorldAnimController = 0;
    }
}

float WorldAnimController::GetAnimationTime()
{
    return m_pPoseTree->m_fTime;
}

void WorldAnimController::SetAnimationTime(float fTime)
{
    m_pPoseTree->SetTime(fTime);
}

nlMatrix4& WorldAnimController::GetNodeMatrix(int nNode) const
{
    return m_pPoseAccumulator->GetNodeMatrix(nNode);
}

int WorldAnimController::GetNodeIndexByID(
    unsigned long uHashID) const
{
    return m_pAnimationSet->m_pHierarchy->GetNodeIndexByID(uHashID);
}

float WorldAnimController::GetMorphWeight(int nChannel) const
{
    cPN_SAnimController* cntrl = m_pPoseTree;
    float fWeight;
    float fTime = cntrl->m_fTime;
    cntrl->m_pSAnim->fn_8030939C(
        nChannel, fTime, &fWeight);
    return fWeight;
}

void WorldAnimController::SetAnimation(
    unsigned long uHashID, ePlayMode playMode)
{
    cSAnim* anim
        = m_pAnimationSet->m_animInventory.Find((unsigned int)uHashID);
    if (m_pPoseTree != 0)
    {
        delete m_pPoseTree;
    }

    cPN_SAnimController* newController = new cPN_SAnimController(
        anim, 0, playMode, 0, 0, false);
    m_pPoseTree = newController;
}

void WorldAnimController::SetAnimationSpeed(float fSpeed)
{
    m_pPoseTree->m_fPlaybackSpeedScale = fSpeed;
}

void WorldAnimController::SetWorldMatrix(
    const nlMatrix4& worldMatrix)
{
    m_worldMatrix = worldMatrix;
}

template struct UnidentifiedStaticStorage<UnidentifiedStaticTag>;
