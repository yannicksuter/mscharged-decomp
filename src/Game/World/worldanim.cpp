#include "Game/World/worldanim.h"
#include "Game/World/WorldAnimObjects.h"

#include "NL/nlMemory.h"

extern "C" void fn_8030B038(cPoseAccumulator*, const cPoseNode*,
    const nlMatrix4*);

class WorldAnimBinder
{
public:
    void BindControllerObjects(const unsigned long& uHashID,
        WorldAnimController** ppController);
};

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
    if (m_pHierarchyInventory != 0)
    {
        m_pHierarchyInventory->Clear();
        delete m_pHierarchyInventory;
        m_pHierarchyInventory = 0;
    }
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

inline AnimationSet* WorldAnimManager::GetOrCreateAnimationSet(
    unsigned long uHierarchyHash)
{
    AnimationSet** ppAnimationSet;
    AnimationSet* pAnimationSet;
    if (m_animationSetMap.FindGet(
            uHierarchyHash, &ppAnimationSet))
    {
        pAnimationSet = *ppAnimationSet;
    }
    else
    {
        AnimationSet* pNewAnimationSet
            = new (nlMalloc(sizeof(AnimationSet), 8, false))
                AnimationSet();
        m_animationSetMap.Add(uHierarchyHash, pNewAnimationSet);
        pAnimationSet = pNewAnimationSet;
    }

    return pAnimationSet;
}

AnimationSet* WorldAnimManager::LoadHierarchy(nlChunk* pChunk)
{
    m_pHierarchyInventory->ParseChunk(pChunk);
    cSHierarchy* pHierarchy = m_pHierarchyInventory->Find(0);
    AnimationSet* pAnimationSet = GetOrCreateAnimationSet(pHierarchy->GetHashID());
    pAnimationSet->m_pHierarchy = pHierarchy;
    return pAnimationSet;
}

void WorldAnimManager::LoadAnimationSet(
    AnimationSet* pAnimationSet, nlChunk* pChunk)
{
    pAnimationSet->m_animInventory.ParseChunk(pChunk);
}

void WorldAnimManager::BindObjects()
{
    WorldAnimBinder binder;
    m_animationControllerMap.Walk(
        &binder, &WorldAnimBinder::BindControllerObjects);
}

void WorldAnimBinder::BindControllerObjects(const unsigned long&,
    WorldAnimController** ppController)
{
    BindWorldAnimObjectDrawables((*ppController)->m_pWorldAnimObject);
}

inline void WorldAnimManager::UpdateControllers(float fDeltaT)
{
    WorldAnimUpdate update;
    update.m_fDeltaT = fDeltaT;
    m_animationControllerMap.Walk(
        &update, &WorldAnimUpdate::Update);
}

void WorldAnimManager::Update(float fDeltaT)
{
    UpdateControllers(fDeltaT);
    m_fTime += fDeltaT;
}

void WorldAnimUpdate::Update(const unsigned long&,
    WorldAnimController** ppController)
{
    float fDeltaT = m_fDeltaT;
    WorldAnimController* pController = *ppController;
    if (pController->m_pPoseTree != 0)
    {
        spCurrentWorldAnimController = pController;
        pController->m_pPoseTree->Update(fDeltaT);
        if (pController->m_pPoseTree->get_fTime()
            != pController->m_pPoseTree->GetPreviousTime())
        {
            fn_8030B038(pController->m_pPoseAccumulator,
                pController->m_pPoseTree, &pController->m_worldMatrix);
        }
        if (pController->m_pPoseTree->UnidentifiedAtEnd()
            && pController->m_pWorldAnimObject != 0)
        {
            SelectRandomWorldAnimation(pController->m_pWorldAnimObject);
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
