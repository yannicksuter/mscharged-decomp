#ifndef GAME_WORLD_H
#define GAME_WORLD_H

#include "NL/nlAVLTree.h"
#include "NL/nlDLListContainer.h"
#include "NL/nlListContainer.h"
#include "Game/World/worldanim.h"
#include "Game/World/WorldObjectLoadContext.h"
#include "types.h"

class DrawableObject;
struct glModel;
class GLView;
class GLResourcePool;
class World;
class nlChunk;
struct WorldVisibilityNode;

class WorldListObject0_80340AC8;
class WorldListObject1_80340AC8;
class WorldUpdateObject_80341BC8;
class WorldEffect;

class World
{
public:
    World(GLResourcePool* pResource);
    virtual ~World();

    virtual void InitializeObjects();
    virtual void Render();
    virtual void Update(float fDeltaT, bool bUpdateState);
    virtual void UpdateAnimations(float fDeltaT);
    virtual DrawableObject* HandleObjectCreation(
        unsigned long uType, WorldObjectLoadContext* pContext) = 0;
    virtual void UnidentifiedVirtual20() { }
    virtual void HandleUnknownChunk(nlChunk* pChunk);

    void AddDrawableObject(DrawableObject* pDrawableObject);
    void RemoveDrawableObject(DrawableObject* pObject);
    bool LoadData(void* pData0, unsigned long uSize0, void* pData1,
        unsigned long uSize1, bool bKeepData);
    bool LoadChunks(nlChunk* pChunk, unsigned long uSize);
    bool LoadObjects(
        void* pData, unsigned long uSize, bool bKeepData);
    DrawableObject* CreateObject(
        unsigned long uType, WorldObjectLoadContext* pContext);
    bool ResolveModel(glModel*& pMaterial);
    DrawableObject* FindDrawableObject(unsigned long uHashID);
    void AddEffect(WorldEffect* pEffect);
    void UpdateEffects(float fDeltaT);
    void ResetEffects();
    void TriggerEffects(unsigned long uType);

    /* 0x04 */ nlDLListContainer<WorldListObject0_80340AC8*> m_objectList0;
    /* 0x0C */ nlDLListContainer<WorldListObject1_80340AC8*> m_objectList1;
    /* 0x14 */ nlDLListContainer<WorldUpdateObject_80341BC8*> m_updateObjects;
    /* 0x1C */ nlListContainer<WorldEffect*> m_worldEffects;
    /* 0x28 */ GLResourcePool* m_pResource;
    /* 0x2C */ WorldAnimManager mWorldAnimManager;
    /* 0x54 */ nlAVLTree<unsigned long, DrawableObject*,
        DefaultKeyCompare<unsigned long> > m_drawableMap;
    /* 0x64 */ u8* m_pOwnedData;
    /* 0x68 */ GLView* m_pOpaqueView;
    /* 0x6C */ GLView* m_pAlphaView;
    /* 0x70 */ bool m_bRenderingEnabled;
    /* 0x74 */ AnimationSet* m_pAnimationSet;
    /* 0x78 */ WorldVisibilityNode* m_pVisibilityTree;
};

typedef char World_size_check[sizeof(World) == 0x7C ? 1 : -1];

#endif // GAME_WORLD_H
