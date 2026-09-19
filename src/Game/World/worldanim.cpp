#include "Game/World/WorldVisibility.h"
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

extern "C" void fn_8030B038(cPoseAccumulator*, const cPoseNode*,
    const nlMatrix4*);
extern "C" void fn_803438FC(WorldAnimObject_803437C8*);
extern "C" void fn_803439A4(WorldAnimObject_803437C8*);
extern "C" void fn_802E4358(EmissionController*);
extern "C" EffectsGroup* fn_802E7D54(
    EmissionManager*, unsigned long);

struct WorldPhysicsOwner_80342170
{
    u8 m_pad00[0x80];
    PhysicsObject* m_pPhysicsObject;
};

class WorldAnimBindCallback_80342BDC
{
public:
    void fn_80342BDC(const unsigned long& uHashID,
        WorldAnimController** ppController);
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

extern "C" void fn_80342170(WorldPhysicsOwner_80342170* pOwner)
{
    if (pOwner->m_pPhysicsObject != 0)
    {
        delete pOwner->m_pPhysicsObject;
        pOwner->m_pPhysicsObject = 0;
    }
}

static WorldAnimController* spCurrentWorldAnimController;

class WorldAnimUpdate
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

void WorldAnimManager::Clear()
{
    delete m_pHierarchyInventory;
    m_pHierarchyInventory = 0;
    m_animationSetMap.DeleteValues();
    m_animationControllerMap.DeleteValues();
}

void WorldAnimManager::BindHierarchy(
    WorldAnimController* pController, unsigned long uHierarchyHash)
{
    pController->m_pAnimationSet = FindAnimationSet(uHierarchyHash);

    pController->m_pPoseAccumulator
        = new (nlMalloc(sizeof(cPoseAccumulator), 8, false))
            cPoseAccumulator(
                pController->m_pAnimationSet->m_pHierarchy, false);
}

WorldAnimController* WorldAnimManager::GetOrCreateController(
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

WorldAnimController* WorldAnimManager::FindController(
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

void WorldAnimManager::BindObjects()
{
    WorldAnimBindCallback_80342BDC callback;
    m_animationControllerMap.Walk(
        &callback, &WorldAnimBindCallback_80342BDC::fn_80342BDC);
}

void WorldAnimBindCallback_80342BDC::fn_80342BDC(const unsigned long&,
    WorldAnimController** ppController)
{
    fn_803438FC((*ppController)->m_pWorldAnimObject);
}

void WorldAnimManager::Update(float fDeltaT)
{
    WorldAnimUpdate update;
    update.m_fDeltaT = fDeltaT;
    m_animationControllerMap.Walk(
        &update, &WorldAnimUpdate::Update);
    m_fTime += fDeltaT;
}

void WorldAnimUpdate::Update(const unsigned long&,
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
