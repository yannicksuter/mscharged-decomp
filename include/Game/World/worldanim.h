#ifndef GAME_WORLD_WORLDANIM_H
#define GAME_WORLD_WORLDANIM_H

#include "Game/Inventory.h"
#include "Game/PoseAccumulator.h"
#include "Game/SHierarchy.h"
#include "Game/SAnim/pnSAnimController.h"
#include "NL/nlAVLTree.h"

class WorldAnimObject_803437C8;

class AnimationSet
{
public:
    AnimationSet()
        : m_pHierarchy(0)
    {
    }

    ~AnimationSet()
    {
        m_animInventory.Clear();
    }

    /* 0x00 */ cSHierarchy* m_pHierarchy;
    /* 0x04 */ cInventory<cSAnim> m_animInventory;
};

class WorldAnimController
{
public:
    WorldAnimController()
        : m_pPoseAccumulator(0)
        , m_pPoseTree(0)
        , m_pAnimationSet(0)
        , mUnidentified4C(0)
        , m_pWorldAnimObject(0)
    {
        m_worldMatrix.SetIdentity();
    }


    ~WorldAnimController()
    {
        delete m_pPoseAccumulator;
        if (m_pPoseTree != 0)
        {
            delete m_pPoseTree;
        }
    }

    float GetAnimationTime();
    void SetAnimationTime(float fTime);
    nlMatrix4& GetNodeMatrix(int nNode) const;
    int GetNodeIndexByID(unsigned long uHashID) const;
    float GetMorphWeight(int nChannel) const;
    void SetAnimation(unsigned long uHashID, ePlayMode playMode);
    void SetAnimationSpeed(float fSpeed);
    void SetWorldMatrix(const nlMatrix4& worldMatrix);

    /* 0x00 */ cPoseAccumulator* m_pPoseAccumulator;
    /* 0x04 */ cPN_SAnimController* m_pPoseTree;
    /* 0x08 */ AnimationSet* m_pAnimationSet;
    /* 0x0C */ nlMatrix4 m_worldMatrix;
    /* 0x4C */ void* mUnidentified4C;
    /* 0x50 */ WorldAnimObject_803437C8* m_pWorldAnimObject;
};

class WorldAnimManager
{
public:
    WorldAnimManager();
    ~WorldAnimManager();

    void fn_80342324();
    void Clear();
    void BindHierarchy(
        WorldAnimController* pController, unsigned long uHierarchyHash);
    AnimationSet* FindAnimationSet(unsigned long uHashID)
    {
        AnimationSet** ppAnimationSet;
        if (m_animationSetMap.FindGet(uHashID, &ppAnimationSet))
        {
            return *ppAnimationSet;
        }
        return 0;
    }
    WorldAnimController* GetOrCreateController(unsigned long uHashID);
    WorldAnimController* FindController(unsigned long uHashID);
    AnimationSet* LoadHierarchy(nlChunk* pChunk);
    void LoadAnimationSet(AnimationSet* pAnimationSet, nlChunk* pChunk);
    void BindObjects();
    void BindControllerObjects(const unsigned long& uHashID,
        WorldAnimController** ppController);
    void Update(float fDeltaT);

    /* 0x00 */ cInventory<cSHierarchy>* m_pHierarchyInventory;
    /* 0x04 */ nlAVLTree<unsigned long, AnimationSet*,
        DefaultKeyCompare<unsigned long> > m_animationSetMap;
    /* 0x14 */ nlAVLTree<unsigned long, WorldAnimController*,
        DefaultKeyCompare<unsigned long> > m_animationControllerMap;
    /* 0x24 */ float m_fTime;
};

typedef char AnimationSet_size_check[sizeof(AnimationSet) == 0x20 ? 1 : -1];
typedef char WorldAnimController_size_check[
    sizeof(WorldAnimController) == 0x54 ? 1 : -1];
typedef char WorldAnimManager_size_check[
    sizeof(WorldAnimManager) == 0x28 ? 1 : -1];

#endif // GAME_WORLD_WORLDANIM_H
