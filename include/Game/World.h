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

class WorldDrawable;
class WorldPhysicsDrawable;
class WorldUpdateObject;
class WorldEffect;

enum eWorldObjectType
{
    WORLD_OBJECT_DRAWABLE = 257,
    WORLD_OBJECT_VISIBILITY = 258,
    WORLD_OBJECT_PHYSICS = 259,
    WORLD_OBJECT_HELPER = 260,
    WORLD_OBJECT_ANIMATION = 262,
    WORLD_OBJECT_CROWD_LAYOUT = 263,
    WORLD_OBJECT_NPC = 264,
    WORLD_OBJECT_EFFECT = 265,
    WORLD_OBJECT_STADIUM_PHYSICS = 65536,
    WORLD_OBJECT_CUP_TROPHY = 65537,
    WORLD_OBJECT_STADIUM_DRAWABLE = 65538,
    WORLD_OBJECT_STADIUM_LIGHT = 65539,
    WORLD_OBJECT_ATTACK_SIDE_INDICATOR = 65540,
    WORLD_OBJECT_SOLAR_FLARE = 65541,
    WORLD_OBJECT_FE_MODEL_MARKER = 65542,
    WORLD_OBJECT_SHADOW_HEIGHT_MARKER = 65543,
    WORLD_OBJECT_TOGGLE = 65544,
    WORLD_OBJECT_SHADOW_VOLUME = 65545,
    WORLD_OBJECT_HIGH_RANGE = 65546,
};

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
    virtual void OnBeforeUnload() { }
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
    bool ResolveModel(glModel*& pModel) const;
    DrawableObject* FindDrawableObject(unsigned long uHashID);
    void AddEffect(WorldEffect* pEffect);
    void UpdateEffects(float fDeltaT);
    void ResetEffects();
    void TriggerEffects(unsigned long uType);

    /* 0x04 */ nlDLListContainer<WorldDrawable*> m_renderObjects;
    /* 0x0C */ nlDLListContainer<WorldPhysicsDrawable*> m_physicsObjects;
    /* 0x14 */ nlDLListContainer<WorldUpdateObject*> m_updateObjects;
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
