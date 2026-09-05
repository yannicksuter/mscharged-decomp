#ifndef GAME_WORLD_H
#define GAME_WORLD_H

#include "NL/nlAVLTree.h"
#include "NL/nlDLListContainer.h"
#include "NL/nlListContainer.h"
#include "Game/World/worldanim.h"
#include "types.h"

class DrawableObject;
struct glModel;
class GLView;
class ResourceInterface_802CC094;
class World;
class nlChunk;

class WorldListObject0_80340AC8;
class WorldListObject1_80340AC8;
class WorldUpdateObject_80341BC8;
class WorldEffect_80341D40;

struct WorldObjectLoadContext_8034136C
{
    /* 0x00 */ u8* m_pObject;
    /* 0x04 */ World* m_pWorld;
    /* 0x08 */ unsigned long m_uNumObjectsLoaded;
    /* 0x0C */ u8* m_pParent;
};

class World
{
public:
    World(ResourceInterface_802CC094* pResource);
    virtual ~World();

    virtual void fn_80341934();
    virtual void Render();
    virtual void Update(float fDeltaT, bool bUpdateState);
    virtual void fn_80341BC0(float fDeltaT);
    virtual DrawableObject* HandleObjectCreation(
        unsigned long uType, WorldObjectLoadContext_8034136C* pContext) = 0;
    virtual void UnidentifiedVirtual20() { }
    virtual void HandleUnknownChunk(nlChunk* pChunk);

    void AddDrawableObject(DrawableObject* pDrawableObject);
    void RemoveDrawableObject(DrawableObject* pObject);
    bool fn_80340F48(void* pData0, unsigned long uSize0, void* pData1,
        unsigned long uSize1, bool bKeepData);
    bool fn_80340FD4(nlChunk* pChunk, unsigned long uSize);
    bool fn_8034136C(
        void* pData, unsigned long uSize, bool bKeepData);
    DrawableObject* fn_803415E8(
        unsigned long uType, WorldObjectLoadContext_8034136C* pContext);
    bool fn_803418C4(glModel*& pMaterial);
    DrawableObject* FindDrawableObject(unsigned long uHashID);
    void fn_80341D40(WorldEffect_80341D40* pEffect);
    void fn_80341DBC(float fDeltaT);
    void fn_80341E1C();
    void fn_80341E68(unsigned long uType);

    /* 0x04 */ nlDLListContainer<WorldListObject0_80340AC8*> m_objectList0;
    /* 0x0C */ nlDLListContainer<WorldListObject1_80340AC8*> m_objectList1;
    /* 0x14 */ nlDLListContainer<WorldUpdateObject_80341BC8*> m_updateObjects;
    /* 0x1C */ nlListContainer<WorldEffect_80341D40*> m_worldEffects;
    /* 0x28 */ ResourceInterface_802CC094* m_pResource;
    /* 0x2C */ WorldAnimManager mWorldAnimManager_02C;
    /* 0x54 */ nlAVLTree<unsigned long, DrawableObject*,
        DefaultKeyCompare<unsigned long> > m_drawableMap;
    /* 0x64 */ u8* m_pOwnedData;
    /* 0x68 */ GLView* m_pView68;
    /* 0x6C */ GLView* m_pView6C;
    /* 0x70 */ bool m_bRenderingEnabled;
    /* 0x74 */ AnimationSet* mAnimationSet_74;
    /* 0x78 */ void* m_pRenderable;
};

typedef char World_size_check[sizeof(World) == 0x7C ? 1 : -1];

#endif // GAME_WORLD_H
