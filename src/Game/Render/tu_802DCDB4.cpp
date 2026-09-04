#include "Game/Render/tu_802DCDB4.h"

#include "Game/SAnim/pnSAnimController.h"
#include "Game/TweakRegistry.h"
#include "NL/gl/glMemory.h"
#include "NL/nlList.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"
#include "NL/platvmath.h"
#include "unclassified/tu_801A4188.h"

class WorldNPCManager_802DD4F0;

class WorldNPCModelList_802DE138
    : public ListContainerBase<ImpostorModel_802DAEE0*,
          NewAdapter<ListEntry<ImpostorModel_802DAEE0*> > >
{
public:
    typedef void (WorldNPCModelList_802DE138::*EntryCallback)(
        ListEntry<ImpostorModel_802DAEE0*>*);

    void UnidentifiedClear()
    {
        EntryCallback callback = &WorldNPCModelList_802DE138::fn_802DE138;
        nlWalkList(m_Head, this, callback);
        m_Head = 0;
        m_Tail = 0;
    }

    void fn_802DE138(ListEntry<ImpostorModel_802DAEE0*>* entry);
};

class WorldNPCManager_802DD4F0
{
public:
    WorldNPCManager_802DD4F0();
    virtual ~WorldNPCManager_802DD4F0();
    virtual ImpostorModel_802DAEE0* fn_802DDD88(
        unsigned long templateHash, const nlMatrix4& transform);

    void fn_802DD790();
    void fn_802DD818(TweakNode_8052BEB0* entry, const char* name);
    void fn_802DDB54();
    bool fn_802DDC1C();
    void fn_802DDF3C(GLView* view);
    void fn_802DDFE8(float dt);

    /* 0x004 */ bool mUnidentified004;
    /* 0x005 */ u8 mPadding005[3];
    /* 0x008 */ CrowdModelCollection_801A4188* mModelCollection;
    /* 0x00C */ CrowdCharacterDefinition_801A4188 mTemplates[50];
    /* 0x4BC */ bool mSelectedTemplates[50];
    /* 0x4EE */ u8 mPadding4EE[2];
    /* 0x4F0 */ CrowdCharacterDefinition_801A4188 mLoadTemplates[50];
    /* 0x9A0 */ ImpostorModel_802DAEE0* mLoadedModels[50];
    /* 0xA68 */ int mNumTemplates;
    /* 0xA6C */ int mNumLoadTemplates;
    /* 0xA70 */ int mNumLoadedModels;
    /* 0xA74 */ WorldNPCModelList_802DE138 mWorldNPCs;
    /* 0xA80 */ nlListContainer<WorldNPC_802DE058*> mPendingWorldNPCs;
    /* 0xA8C */ bool mTemplatesLoaded;
    /* 0xA8D */ bool mModelsLoaded;
    /* 0xA8E */ u8 mPaddingA8E[2];
    /* 0xA90 */ void (*mModelCallback)(ImpostorModel_802DAEE0*, glModel*);
    /* 0xA94 */ bool (*mRenderFilter)(ImpostorModel_802DAEE0*);
}; // size: 0xA98

// Serialized world-NPC record used by the stadium/world object stream. The
// retail binary does not preserve the original class name.
class WorldNPC_802DE058
{
public:
    virtual ~WorldNPC_802DE058();
    virtual void fn_802DE124();
    virtual nlMatrix4* fn_802DE1C8();
    virtual void fn_802DE1D0(const nlMatrix4& transform);
    virtual void fn_80278E94();
    virtual void fn_802DE058();

    /* 0x04 */ u8 mUnidentified004[0x1C];
    /* 0x20 */ nlMatrix4 mTransform;
    /* 0x60 */ unsigned long mTemplateHash;
    /* 0x64 */ u8 mUnidentified064[0x0C];
}; // size: 0x70

extern "C"
{
    void fn_802C6CAC(const char* fileName, const char* category, bool reload);
    bool lbl_806E1F90;
    WorldNPCManager_802DD4F0* lbl_806E1F94;
}

static const char* lbl_806DF450 = "/Render/WorldNPCs";

static inline void nlPlaneNormalize(nlVector4& in, nlVector4& out)
{
    float inverseLength = nlRecipSqrt(
        in.x * in.x + in.y * in.y + in.z * in.z, true);
    nlVec4Set(out, inverseLength * in.x, inverseLength * in.y,
        inverseLength * in.z, inverseLength * in.w);
}

extern "C" void fn_802DCDB4(nlVector4* pPlanes,
    const nlMatrix4& projectionMatrix, const nlMatrix4& viewMatrix)
{
    nlMatrix4 projection = projectionMatrix;
    nlMatrix4 viewProjection;

    float m33 = projection.m33;
    float m43 = projection.m43;
    projection.m43 = projection.m34;
    projection.m34 = m43;
    projection.m33 = m33 - 1.0f;

    nlMultMatrices(viewProjection, viewMatrix, projection);

    pPlanes[0].x = viewProjection.m14 - viewProjection.m11;
    pPlanes[0].y = viewProjection.m24 - viewProjection.m21;
    pPlanes[0].z = viewProjection.m34 - viewProjection.m31;
    pPlanes[0].w = viewProjection.m44 - viewProjection.m41;

    pPlanes[1].x = viewProjection.m14 + viewProjection.m11;
    pPlanes[1].y = viewProjection.m24 + viewProjection.m21;
    pPlanes[1].z = viewProjection.m34 + viewProjection.m31;
    pPlanes[1].w = viewProjection.m44 + viewProjection.m41;

    pPlanes[2].x = viewProjection.m14 + viewProjection.m12;
    pPlanes[2].y = viewProjection.m24 + viewProjection.m22;
    pPlanes[2].z = viewProjection.m34 + viewProjection.m32;
    pPlanes[2].w = viewProjection.m44 + viewProjection.m42;

    pPlanes[3].x = viewProjection.m14 - viewProjection.m12;
    pPlanes[3].y = viewProjection.m24 - viewProjection.m22;
    pPlanes[3].z = viewProjection.m34 - viewProjection.m32;
    pPlanes[3].w = viewProjection.m44 - viewProjection.m42;

    pPlanes[4].x = viewProjection.m13;
    pPlanes[4].y = viewProjection.m23;
    pPlanes[4].z = viewProjection.m33;
    pPlanes[4].w = viewProjection.m43;

    pPlanes[5].x = viewProjection.m14 - viewProjection.m13;
    pPlanes[5].y = viewProjection.m24 - viewProjection.m23;
    pPlanes[5].z = viewProjection.m34 - viewProjection.m33;
    pPlanes[5].w = viewProjection.m44 - viewProjection.m43;

    for (unsigned int i = 0; i < 6; ++i)
    {
        nlPlaneNormalize(pPlanes[i], pPlanes[i]);
    }
}

extern "C" FrustumResult_802DD05C fn_802DD05C(const nlVector4* pPlanes,
    const nlVector3* pBoundsMin, const nlVector3* pBoundsMax,
    unsigned long* pPlaneMask)
{
    const nlVector4* pNormalPlanes;
    unsigned long planeMask = pPlaneMask != 0 ? *pPlaneMask : 0;
    if (planeMask == 0x3F)
    {
        return FRUSTUM_INSIDE;
    }

    FrustumResult_802DD05C result = FRUSTUM_INSIDE;
    unsigned long planeBit = 1;
    pNormalPlanes = pPlanes;
    for (int i = 0; i < 6;
        ++i, planeBit <<= 1, ++pPlanes, ++pNormalPlanes)
    {
        if ((planeBit & planeMask) != 0)
        {
            continue;
        }

        nlVector3 positive;
        nlVector3 negative;
        if (pPlanes->x >= 0.0f)
        {
            positive.x = pBoundsMax->x;
            negative.x = pBoundsMin->x;
        }
        else
        {
            positive.x = pBoundsMin->x;
            negative.x = pBoundsMax->x;
        }
        if (pPlanes->y >= 0.0f)
        {
            positive.y = pBoundsMax->y;
            negative.y = pBoundsMin->y;
        }
        else
        {
            positive.y = pBoundsMin->y;
            negative.y = pBoundsMax->y;
        }
        if (pPlanes->z >= 0.0f)
        {
            positive.z = pBoundsMax->z;
            negative.z = pBoundsMin->z;
        }
        else
        {
            positive.z = pBoundsMin->z;
            negative.z = pBoundsMax->z;
        }

        float positiveDistance
            = nlVec3DotProduct(positive,
                  *(const nlVector3*)pNormalPlanes)
            + pPlanes->w;
        if (positiveDistance < 0.0f)
        {
            if (pPlaneMask != 0)
            {
                *pPlaneMask = planeMask;
            }
            return FRUSTUM_OUTSIDE;
        }

        float negativeDistance
            = nlVec3DotProduct(negative,
                  *(const nlVector3*)pNormalPlanes)
            + pPlanes->w;
        if (negativeDistance < 0.0f)
        {
            result = FRUSTUM_INTERSECTING;
        }
        else
        {
            planeMask |= planeBit;
        }
    }

    if (pPlaneMask != 0)
    {
        *pPlaneMask = planeMask;
    }
    return result;
}

extern "C" FrustumResult_802DD05C fn_802DD1EC(const nlVector4* pPlanes,
    const nlVector3* pPosition, float fRadius)
{
    FrustumResult_802DD05C result = FRUSTUM_INSIDE;
    const nlVector4* pPlaneDistances = pPlanes;
    for (int i = 0; i < 6; ++i)
    {
        float distance
            = nlVec3DotProduct(*pPosition,
                  *(const nlVector3*)&pPlanes[i])
            + pPlaneDistances[i].w;
        if (distance < -fRadius)
        {
            return FRUSTUM_OUTSIDE;
        }
        if (distance < fRadius)
        {
            result = FRUSTUM_INTERSECTING;
        }
    }
    return result;
}

extern "C" void fn_802DD2F4(
    const nlVector4* pPlanes, nlVector4* pCorners)
{
    static const int planeIndices[4][2]
        = { { 2, 0 }, { 2, 1 }, { 3, 1 }, { 3, 0 } };

    for (int end = 0; end < 2; ++end)
    {
        const nlVector4& endPlane = pPlanes[end == 0 ? 4 : 5];
        for (int side = 0; side < 4; ++side)
        {
            const nlVector4& firstPlane = pPlanes[planeIndices[side][0]];
            const nlVector4& secondPlane = pPlanes[planeIndices[side][1]];

            nlMatrix4 planeMatrix;
            planeMatrix.m11 = endPlane.x;
            planeMatrix.m12 = firstPlane.x;
            planeMatrix.m13 = secondPlane.x;
            planeMatrix.m14 = 0.0f;
            planeMatrix.m21 = endPlane.y;
            planeMatrix.m22 = firstPlane.y;
            planeMatrix.m23 = secondPlane.y;
            planeMatrix.m24 = 0.0f;
            planeMatrix.m31 = endPlane.z;
            planeMatrix.m32 = firstPlane.z;
            planeMatrix.m33 = secondPlane.z;
            planeMatrix.m34 = 0.0f;
            planeMatrix.m41 = 0.0f;
            planeMatrix.m42 = 0.0f;
            planeMatrix.m43 = 0.0f;
            planeMatrix.m44 = 1.0f;

            nlVector4 distances;
            distances.x = -endPlane.w;
            distances.y = -firstPlane.w;
            distances.z = -secondPlane.w;
            distances.w = 0.0f;

            nlMatrix4 inverse;
            nlInvertMatrix(inverse, planeMatrix);
            nlVector4 corner;
            nlMultVectorMatrix(corner, distances, inverse);
            pCorners[end * 4 + side] = corner;
        }
    }
}

WorldNPCManager_802DD4F0::WorldNPCManager_802DD4F0()
    : mUnidentified004(false)
    , mNumTemplates(0)
    , mNumLoadTemplates(0)
    , mNumLoadedModels(0)
    , mTemplatesLoaded(false)
    , mModelsLoaded(false)
    , mModelCallback(0)
{
    mModelCollection = new (8, false) CrowdModelCollection_801A4188;
    lbl_806E1F94 = this;
}

WorldNPCManager_802DD4F0::~WorldNPCManager_802DD4F0()
{
    delete mModelCollection;

    for (int i = 0; i < mNumLoadedModels; ++i)
    {
        delete mLoadedModels[i];
    }

    mWorldNPCs.UnidentifiedClear();
    mPendingWorldNPCs.Clear();
    lbl_806E1F94 = 0;
}

void WorldNPCManager_802DD4F0::fn_802DD790()
{
    fn_802C6CAC("ini/WorldNPCs.ini", lbl_806DF450, false);
    TweakEntry_8052BF00* entry
        = fn_802C4504(fn_802C0E30(), lbl_806DF450, true);

    for (TweakNode_8052BEB0* child = entry->m_ChildHead; child != 0;
        child = child->m_Next)
    {
        fn_802DD818(child, fn_802C3FDC(child));
    }
    mTemplatesLoaded = true;
}

struct WorldNPCStringValue_802DD818
{
    u8 mUnidentified000[0x0C];
    const char* mValue;
};

void WorldNPCManager_802DD4F0::fn_802DD818(
    TweakNode_8052BEB0* entry, const char* name)
{
    const char* hierarchyFile = 0;
    const char* animationFile = 0;
    const char* hierarchyName = 0;
    const char* textureBundleFile = 0;
    const char* modelFile = 0;

    for (TweakNode_8052BEB0* child
         = ((TweakEntry_8052BF00*)entry)->m_ChildHead;
         child != 0; child = child->m_Next)
    {
        const char* value
            = ((WorldNPCStringValue_802DD818*)child->m_Value)->mValue;
        if (nlStrICmp(fn_802C3FDC(child), "AnimationFile") == 0)
        {
            animationFile = value;
        }
        if (nlStrICmp(fn_802C3FDC(child), "HierarchyFile") == 0)
        {
            hierarchyFile = value;
        }
        if (nlStrICmp(fn_802C3FDC(child), "HierarchyName") == 0)
        {
            hierarchyName = value;
        }
        if (nlStrICmp(fn_802C3FDC(child), "TextureBundleFile") == 0)
        {
            textureBundleFile = value;
        }
        if (nlStrICmp(fn_802C3FDC(child), "ModelFile") == 0)
        {
            modelFile = value;
        }
    }

    if (hierarchyFile != 0 && animationFile != 0 && hierarchyName != 0
        && textureBundleFile != 0 && modelFile != 0)
    {
        CrowdCharacterDefinition_801A4188& definition
            = mTemplates[mNumTemplates];
        definition.mUnidentified00 = name;
        definition.mUnidentified04 = animationFile;
        definition.mUnidentified08 = hierarchyFile;
        definition.mUnidentified0C = hierarchyName;
        definition.mUnidentified10 = textureBundleFile;
        definition.mUnidentified14 = modelFile;
        mSelectedTemplates[mNumTemplates] = false;
        ++mNumTemplates;
    }
}

void WorldNPCManager_802DD4F0::fn_802DDB54()
{
    for (int i = 0; i < mNumTemplates; ++i)
    {
        if (mSelectedTemplates[i])
        {
            mLoadTemplates[mNumLoadTemplates] = mTemplates[i];
            ++mNumLoadTemplates;
        }
    }

    fn_802DBDA0(mModelCollection, mLoadTemplates, mNumLoadTemplates);
    if (fn_802DBF5C(mModelCollection))
    {
        fn_802DBF7C(mModelCollection);
    }
}

bool WorldNPCManager_802DD4F0::fn_802DDC1C()
{
    if (mNumLoadTemplates == 0)
    {
        return true;
    }
    if (!fn_802DC2A4(mModelCollection))
    {
        return false;
    }

    fn_802DC6F8(mModelCollection);
    mLoadedModels[mNumLoadedModels] = mModelCollection->models[mNumLoadedModels];
    ++mNumLoadedModels;

    if (fn_802DBF5C(mModelCollection))
    {
        fn_802DBF7C(mModelCollection);
        return false;
    }

    mModelsLoaded = true;
    for (ListEntry<WorldNPC_802DE058*>* entry = mPendingWorldNPCs.m_Head;
        entry != 0; entry = entry->next)
    {
        nlMatrix4 transform = entry->entry->mTransform;
        fn_802DDD88(entry->entry->mTemplateHash, transform);
    }
    return true;
}

ImpostorModel_802DAEE0* WorldNPCManager_802DD4F0::fn_802DDD88(
    unsigned long templateHash, const nlMatrix4& transform)
{
    bool found = false;
    int index = 0;
    for (; index < mNumLoadTemplates; ++index)
    {
        if (templateHash
            == nlStringLowerHash(mLoadTemplates[index].mUnidentified00))
        {
            found = true;
            break;
        }
    }

    ImpostorModel_802DAEE0* source
        = found ? mLoadedModels[index] : 0;
    ImpostorModel_802DAEE0* model
        = fn_802DB0AC(source, fn_802CC094());
    if (mModelCallback != 0)
    {
        model->mModelCallback = mModelCallback;
    }

    fn_802DB528(model, "idle", 0.0f, PM_CYCLIC);
    fn_802DB2B8(model, nlRandomf(0.0f, 1.0f, &nlDefaultSeed));
    model->mWorldMatrix = transform;
    mWorldNPCs.AddEnd(model);
    return model;
}

void WorldNPCManager_802DD4F0::fn_802DDF3C(GLView* view)
{
    if (lbl_806E1F90)
    {
        return;
    }

    for (ListEntry<ImpostorModel_802DAEE0*>* entry = mWorldNPCs.m_Head;
        entry != 0; entry = entry->next)
    {
        ImpostorModel_802DAEE0* model = entry->entry;
        if (mRenderFilter != 0 && !mRenderFilter(model))
        {
            continue;
        }
        model->UnidentifiedVirtual0C(view, 0);
    }
}

void WorldNPCManager_802DD4F0::fn_802DDFE8(float dt)
{
    for (ListEntry<ImpostorModel_802DAEE0*>* entry = mWorldNPCs.m_Head;
        entry != 0; entry = entry->next)
    {
        ImpostorModel_802DAEE0* model = entry->entry;
        model->mAnimController->Update(dt);
        fn_802DB26C(model);
    }
}

void WorldNPC_802DE058::fn_802DE058()
{
    WorldNPCManager_802DD4F0* manager = lbl_806E1F94;
    bool found = false;
    int index = 0;
    for (; index < manager->mNumTemplates; ++index)
    {
        if (mTemplateHash
            == nlStringLowerHash(manager->mTemplates[index].mUnidentified00))
        {
            found = true;
            break;
        }
    }
    if (found)
    {
        manager->mSelectedTemplates[index] = true;
    }
    manager->mPendingWorldNPCs.AddEnd(this);
}

void WorldNPC_802DE058::fn_802DE124()
{
}

inline void WorldNPCModelList_802DE138::fn_802DE138(
    ListEntry<ImpostorModel_802DAEE0*>* entry)
{
    delete entry->entry;
    delete entry;
}

WorldNPC_802DE058::~WorldNPC_802DE058()
{
}

nlMatrix4* WorldNPC_802DE058::fn_802DE1C8()
{
    return &mTransform;
}

void WorldNPC_802DE058::fn_802DE1D0(const nlMatrix4& transform)
{
    mTransform = transform;
}
