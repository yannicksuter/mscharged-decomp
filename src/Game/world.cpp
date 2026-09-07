#include "Game/World.h"

#include "Game/Drawable/DrawableObj.h"
#include "Game/Effects/EmissionManager.h"
#include "Game/GL/GLInventory.h"
#include "Game/SAnim.h"
#include "NL/gl/glMemory.h"
#include "NL/gl/glTexture.h"
#include "NL/gl/glView.h"
#include "NL/nlPrint.h"

class WorldUpdateObject_80341BC8
{
public:
    virtual ~WorldUpdateObject_80341BC8();
    virtual void UnidentifiedVirtual0C();
    virtual void UnidentifiedVirtual10();
    virtual void UnidentifiedVirtual14();
    virtual void UnidentifiedVirtual18();
    virtual void Update(float fDeltaT);
};

class WorldEffect_80341D40
{
public:
    u8 m_pad00[0x60];
    /* 0x60 */ float m_fDuration;
    u8 m_pad64[0x14];
    /* 0x78 */ unsigned long m_uState;
    /* 0x7C */ unsigned long m_uType;
    /* 0x80 */ float m_fTime;
    /* 0x84 */ float m_fUnidentified084;
    u8 m_pad88[0x08];
    /* 0x90 */ unsigned long m_uPreviousState;
};

extern "C" void fn_802C81FC(
    nlChunk*, unsigned long, unsigned long, ResourceInterface_802CC094*);
extern "C" void* fn_802DC9EC(nlChunk*);
extern "C" void fn_802DCD18(void*, const nlVector4*, bool);
extern "C" void fn_80343DE4(
    DrawableObject*, WorldObjectLoadContext_8034136C*);
extern "C" void fn_80343E3C(
    DrawableObject*, WorldObjectLoadContext_8034136C*);
extern "C" void fn_80344144(
    DrawableObject*, WorldObjectLoadContext_8034136C*);
extern "C" void fn_80344308(WorldEffect_80341D40*, float);
extern int nlPrintf(const char*, ...);

World::World(ResourceInterface_802CC094* pResource)
    : m_pResource(pResource)
    , m_pOwnedData(0)
    , m_pView68(0)
    , m_pView6C(0)
{
    m_pRenderable = 0;
    m_bRenderingEnabled = true;
}

World::~World()
{
    typedef nlAVLTreeIterator<unsigned long, DrawableObject*,
        DefaultKeyCompare<unsigned long> > DrawableIterator;

    m_objectList0.Clear();
    m_objectList1.Clear();
    m_updateObjects.Clear();

    DrawableIterator* pIterator = m_drawableMap.GetIterator();
    while (pIterator->IsValid())
    {
        DrawableObject* pObject = pIterator->Current()->value;
        pObject->V1();
        if ((pObject->m_uObjectCreationFlags & 1) == 0)
        {
            delete pObject;
        }
        pIterator->Next();
    }
    delete pIterator;

    mWorldAnimManager_02C.fn_80342324();
    UnidentifiedVirtual20();
    mWorldAnimManager_02C.fn_80342328();
    delete m_pOwnedData;
}

void World::AddDrawableObject(DrawableObject* pDrawableObject)
{
    m_drawableMap.Add(pDrawableObject->GetHashID(), pDrawableObject);

    if (pDrawableObject->m_uObjectCreationFlags & 2)
    {
        m_objectList0.AddEnd(
            (WorldListObject0_80340AC8*)pDrawableObject);
    }

    if (pDrawableObject->m_uObjectCreationFlags & 4)
    {
        m_objectList1.AddEnd(
            (WorldListObject1_80340AC8*)pDrawableObject);
        if (pDrawableObject->m_uObjectCreationFlags & 8)
        {
            m_updateObjects.AddEnd(
                (WorldUpdateObject_80341BC8*)pDrawableObject);
        }
    }
}

void World::RemoveDrawableObject(DrawableObject* pObject)
{
    m_drawableMap.Remove(pObject->GetHashID());
    unsigned long uFlags = pObject->m_uObjectCreationFlags;

    if (uFlags & 2)
    {
        nlDLListIterator<WorldListObject0_80340AC8*> iterator
            = m_objectList0.Begin();
        while (iterator.hasNext())
        {
            if (*iterator == (WorldListObject0_80340AC8*)pObject)
            {
                m_objectList0.Remove(&iterator);
                return;
            }
            iterator.next();
        }
    }

    if (uFlags & 4)
    {
        nlDLListIterator<WorldListObject1_80340AC8*> iterator
            = m_objectList1.Begin();
        while (iterator.hasNext())
        {
            if (*iterator == (WorldListObject1_80340AC8*)pObject)
            {
                m_objectList1.Remove(&iterator);
                break;
            }
            iterator.next();
        }

        if (pObject->m_uObjectCreationFlags & 4)
        {
            nlDLListIterator<WorldUpdateObject_80341BC8*> updateIterator
                = m_updateObjects.Begin();
            while (updateIterator.hasNext())
            {
                if (*updateIterator
                    == (WorldUpdateObject_80341BC8*)pObject)
                {
                    m_updateObjects.Remove(&updateIterator);
                    return;
                }
                updateIterator.next();
            }
        }
    }
}

bool World::fn_80340F48(void* pData0, unsigned long uSize0, void* pData1,
    unsigned long uSize1, bool bKeepData)
{
    mAnimationSet_74 = 0;
    if (pData0 != 0)
    {
        fn_80340FD4((nlChunk*)pData0, uSize0);
    }
    fn_80340FD4((nlChunk*)pData1, uSize1);
    if (bKeepData)
    {
        m_pOwnedData = (u8*)pData1;
    }
    else
    {
        m_pOwnedData = 0;
    }
    return true;
}

bool World::fn_80340FD4(nlChunk* pChunk, unsigned long uSize)
{
    nlChunk* pCurrent = pChunk->GetFirstChunk();
    nlChunk* pEnd = pChunk->GetLastChunk();

    while (pCurrent != pEnd)
    {
        switch (pCurrent->GetID())
        {
        case 0x00026000:
            fn_8034136C(pCurrent->GetData(),
                pCurrent->GetSize()
                    - ((u8*)pCurrent->GetData()
                        - ((u8*)pCurrent + sizeof(nlChunk))),
                false);
            break;
        case 0x80008000:
            mAnimationSet_74
                = mWorldAnimManager_02C.LoadHierarchy(pCurrent);
            break;
        case 0x80007000:
            mWorldAnimManager_02C.LoadAnimationSet(
                mAnimationSet_74, pCurrent);
            break;
        case 0x80026100:
            m_pRenderable = fn_802DC9EC(pCurrent);
            break;
        case 0x00024100:
            fn_802CDD78(pCurrent->GetData(),
                pCurrent->GetSize()
                    - ((u8*)pCurrent->GetData()
                        - ((u8*)pCurrent + sizeof(nlChunk))),
                m_pResource, false);
            break;
        case 0x8001B000:
        case 0x8001B100:
            fn_802C81FC(pCurrent, 0, 0, m_pResource);
            break;
        default:
            HandleUnknownChunk(pCurrent);
            break;
        }
        pCurrent = pCurrent->GetNextChunk();
    }
    return true;
}

bool World::fn_8034136C(
    void* pData, unsigned long uSize, bool bKeepData)
{
    unsigned long i = 0;

    if (bKeepData)
    {
        m_pOwnedData = (u8*)pData;
    }
    else
    {
        m_pOwnedData = 0;
    }

    WorldObjectLoadContext_8034136C context;
    context.m_pObject = (u8*)pData + 0x10;
    context.m_pWorld = this;
    context.m_uNumObjectsLoaded = 0;
    context.m_pParent = 0;

    for (; i < *(unsigned long*)pData; ++i)
    {
        unsigned long uType = *(unsigned long*)(context.m_pObject + 8);
        if (uType == 0x10)
        {
            u8* pParent = context.m_pObject;
            context.m_pObject += *(unsigned long*)(context.m_pObject + 0xC);
            context.m_pParent = pParent;
            continue;
        }

        DrawableObject* pObject;
        if (uType < 0x10000)
        {
            pObject = fn_803415E8(uType, &context);
        }
        else
        {
            pObject = HandleObjectCreation(uType, &context);
        }

        pObject->m_pWorldContext = this;
        if (pObject != 0)
        {
            AddDrawableObject(pObject);
        }
        context.m_pParent = 0;
    }

    mWorldAnimManager_02C.fn_80342A74();
    return true;
}

DrawableObject* World::fn_803415E8(
    unsigned long uType, WorldObjectLoadContext_8034136C* pContext)
{
    DrawableObject* pObject = 0;
    unsigned long uSize = 0;

    switch (uType)
    {
    case 0x101:
        pObject = (DrawableObject*)pContext->m_pObject;
        fn_80343DE4(pObject, pContext);
        uSize = 0x70;
        break;
    case 0x102:
        pObject = (DrawableObject*)pContext->m_pObject;
        fn_80343E3C(pObject, pContext);
        uSize = 0x30;
        break;
    case 0x103:
        pObject = (DrawableObject*)pContext->m_pObject;
        pObject->m_uObjectCreationFlags |= 4;
        fn_80344144(pObject, pContext);
        uSize = 0x90;
        break;
    case 0x104:
        pObject = (DrawableObject*)pContext->m_pObject;
        uSize = 0x60;
        break;
    case 0x106:
        pObject = (DrawableObject*)pContext->m_pObject;
        uSize = 0x90;
        break;
    case 0x107:
        pObject = (DrawableObject*)pContext->m_pObject;
        uSize = 0x80;
        break;
    case 0x108:
        pObject = (DrawableObject*)pContext->m_pObject;
        uSize = 0x70;
        break;
    case 0x109:
        pObject = (DrawableObject*)pContext->m_pObject;
        uSize = 0xA0;
        break;
    default:
        break;
    }

    if (pObject != 0)
    {
        pContext->m_pObject += uSize;
        ++pContext->m_uNumObjectsLoaded;
    }
    return pObject;
}

bool World::fn_803418C4(glModel*& pMaterial)
{
    unsigned long uHashID = (unsigned long)pMaterial;
    pMaterial = m_pResource->m_inventory->GetModel(uHashID);
    if (pMaterial == 0)
    {
        nlPrintf(
            "Warning: Failed to find GL model to match world object 0x%08x\n",
            uHashID);
        return false;
    }
    return true;
}

void World::HandleUnknownChunk(nlChunk* pChunk)
{
    nlPrintf("Unknown Chunk = 0x%08x\n", pChunk->GetID());
}

void World::fn_80341934()
{
    typedef nlAVLTreeIterator<unsigned long, DrawableObject*,
        DefaultKeyCompare<unsigned long> > DrawableIterator;

    DrawableIterator* pIterator = m_drawableMap.GetIterator();
    while (pIterator->IsValid())
    {
        pIterator->Current()->value->V4(this);
        pIterator->Next();
    }
    if (pIterator != 0)
    {
        delete pIterator;
    }
}

void World::Render()
{
    if (m_pRenderable != 0)
    {
        fn_802DCD18(m_pRenderable,
            m_pView68->m_Interface->GetShadowMatrix(), false);
    }

    if (m_bRenderingEnabled)
    {
        nlDLListIterator<WorldListObject0_80340AC8*> iterator
            = m_objectList0.Begin();
        while (iterator.hasNext())
        {
            if (((DrawableObject*)*iterator)
                    ->V6(m_pView68->m_Interface->GetShadowMatrix()))
            {
                ((DrawableObject*)*iterator)->Draw();
            }
            iterator.next();
        }
    }
}

void World::fn_80341BC0(float fDeltaT)
{
    mWorldAnimManager_02C.fn_80342BE8(fDeltaT);
}

void World::Update(float fDeltaT, bool bUpdateState)
{
    if (bUpdateState)
    {
        fn_80341BC0(fDeltaT);
    }

    if (EmissionManager::Instance() != 0)
    {
        nlListIterator<WorldEffect_80341D40*> iterator
            = m_worldEffects.Begin();
        while (iterator.IsValid())
        {
            fn_80344308(iterator.Current(), fDeltaT);
            iterator.Next();
        }
    }

    nlDLListIterator<WorldUpdateObject_80341BC8*> iterator
        = m_updateObjects.Begin();
    while (iterator.hasNext())
    {
        (*iterator)->Update(fDeltaT);
        iterator.next();
    }
}

DrawableObject* World::FindDrawableObject(unsigned long uHashID)
{
    DrawableObject** foundValue;
    if (!m_drawableMap.FindGet(uHashID, &foundValue))
    {
        return 0;
    }
    return *foundValue;
}

void World::fn_80341D40(WorldEffect_80341D40* pEffect)
{
    m_worldEffects.AddStart(pEffect);
}

void World::fn_80341DBC(float fDeltaT)
{
    if (EmissionManager::Instance() != 0)
    {
        nlListIterator<WorldEffect_80341D40*> iterator
            = m_worldEffects.Begin();
        while (iterator.IsValid())
        {
            fn_80344308(iterator.Current(), fDeltaT);
            iterator.Next();
        }
    }
}

void World::fn_80341E1C()
{
    nlListIterator<WorldEffect_80341D40*> iterator
        = m_worldEffects.Begin();
    while (iterator.IsValid())
    {
        WorldEffect_80341D40* pEffect = iterator.Current();
        pEffect->m_fTime = 0.0f;
        pEffect->m_uPreviousState = pEffect->m_uState;
        pEffect->m_fUnidentified084 = 0.0f;
        if (pEffect->m_uType == 0)
        {
            pEffect->m_fTime = 1.0f + pEffect->m_fDuration;
        }
        iterator.Next();
    }
}

void World::fn_80341E68(unsigned long uType)
{
    nlListIterator<WorldEffect_80341D40*> iterator
        = m_worldEffects.Begin();
    while (iterator.IsValid())
    {
        WorldEffect_80341D40* pEffect = iterator.Current();
        if (uType == pEffect->m_uType)
        {
            pEffect->m_uPreviousState = pEffect->m_uState;
            pEffect->m_fTime = 1.0f + pEffect->m_fDuration;
        }
        iterator.Next();
    }
}
