#include "Game/Render/WorldNPC.h"

#include "Game/SAnim/pnSAnimController.h"
#include "Game/TweakRegistry.h"
#include "Game/TweakConfig.h"
#include "NL/gl/glMemory.h"
#include "NL/nlList.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"
#include "NL/platvmath.h"
#include "Game/Render/CrowdImpostors.h"
#include "NL/nlstring_tmpl.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/World/WorldObject.h"

bool gDisableWorldNPCs;
WorldNPCManager* gpWorldNPCManager;

static const char* sWorldNPCTweakPath = "/Render/WorldNPCs";

inline void WorldNPCManager::RegisterObject(WorldNPC* npc)
{
    unsigned long templateHash;
    bool found;
    int index;

    found = false;
    index = 0;
    templateHash = npc->mTemplateHash;
    for (; index < mNumTemplates; ++index)
    {
        if (templateHash
            == nlStringLowerHash(mTemplates[index].mName))
        {
            found = true;
            break;
        }
    }
    if (found)
    {
        mSelectedTemplates[index] = true;
    }
    mPendingWorldNPCs.AddEnd(npc);
}

static inline bool FindLoadedTemplate(const WorldNPCManager& manager,
    unsigned long templateHash, int& index)
{
    bool found = false;
    index = 0;
    for (; index < manager.mNumLoadTemplates; ++index)
    {
        if (templateHash
            == nlStringLowerHash(manager.mLoadTemplates[index].mName))
        {
            found = true;
            break;
        }
    }
    return found;
}

static inline void nlPlaneNormalize(nlVector4& in, nlVector4& out)
{
    float inverseLength = nlRecipSqrt(
        in.x * in.x + in.y * in.y + in.z * in.z, true);
    nlVec4Set(out, inverseLength * in.x, inverseLength * in.y,
        inverseLength * in.z, inverseLength * in.w);
}

void ExtractFrustumPlanes(nlVector4* pPlanes,
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

FrustumResult ClassifyBoxInFrustum(const nlVector4* pPlanes,
    const nlVector3* pBoundsMin, const nlVector3* pBoundsMax,
    unsigned long* pPlaneMask)
{
    const nlVector4* pNormalPlanes;
    unsigned long planeMask = pPlaneMask != 0 ? *pPlaneMask : 0;
    if (planeMask == 0x3F)
    {
        return FRUSTUM_INSIDE;
    }

    FrustumResult result = FRUSTUM_INSIDE;
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

FrustumResult ClassifySphereInFrustum(const nlVector4* pPlanes,
    const nlVector3* pPosition, float fRadius)
{
    FrustumResult result = FRUSTUM_INSIDE;
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

void GetFrustumCorners(
    const nlVector4* pPlanes, nlVector4* pCorners)
{
    const int planeIndices[4][2]
        = { { 2, 0 }, { 2, 1 }, { 3, 1 }, { 3, 0 } };

    for (int end = 0; end < 2; ++end)
    {
        int endPlaneIndex = 5;
        if (end == 0)
        {
            endPlaneIndex = 4;
        }
        const nlVector4& endPlane = pPlanes[endPlaneIndex];
        for (int side = 0; side < 4; ++side)
        {
            nlVector4& corner = pCorners[end * 4 + side];
            const nlVector4& firstPlane = pPlanes[planeIndices[side][0]];
            const nlVector4& secondPlane = pPlanes[planeIndices[side][1]];

            nlMatrix4 planeMatrix;
            planeMatrix.m11 = endPlane.x;
            planeMatrix.m21 = endPlane.y;
            planeMatrix.m31 = endPlane.z;
            planeMatrix.m41 = endPlane.w;
            planeMatrix.m12 = firstPlane.x;
            planeMatrix.m22 = firstPlane.y;
            planeMatrix.m32 = firstPlane.z;
            planeMatrix.m42 = firstPlane.w;
            planeMatrix.m13 = secondPlane.x;
            planeMatrix.m23 = secondPlane.y;
            planeMatrix.m33 = secondPlane.z;
            planeMatrix.m43 = secondPlane.w;
            planeMatrix.m14 = 0.0f;
            planeMatrix.m24 = 0.0f;
            planeMatrix.m34 = 0.0f;
            planeMatrix.m41 = 0.0f;
            planeMatrix.m42 = 0.0f;
            planeMatrix.m43 = 0.0f;
            planeMatrix.m44 = 1.0f;

            nlVector4 distances = { 0.0f, 0.0f, 0.0f, 0.0f };
            distances.x = -endPlane.w;
            distances.y = -firstPlane.w;
            distances.z = -secondPlane.w;

            nlMatrix4 inverse;
            nlInvertMatrix(inverse, planeMatrix);
            nlMultVectorMatrix(corner, distances, inverse);
            pCorners[end * 4 + side] = corner;
        }
    }
}

WorldNPCManager::WorldNPCManager()
    : mUnidentified004(false)
    , mNumTemplates(0)
    , mNumLoadTemplates(0)
    , mNumLoadedModels(0)
    , mTemplatesLoaded(false)
    , mModelsLoaded(false)
    , mModelCallback(0)
{
    mModelCollection = new (8, false) CrowdModelCollection;
    gpWorldNPCManager = this;
}

WorldNPCManager::~WorldNPCManager()
{
    delete mModelCollection;

    for (int i = 0; i < mNumLoadedModels; ++i)
    {
        delete mLoadedModels[i];
    }

    mWorldNPCs.Clear();
    mPendingWorldNPCs.Clear();
    gpWorldNPCManager = 0;
}

void WorldNPCManager::LoadTemplates(const char*)
{
    LoadTweakConfigFile("ini/WorldNPCs.ini", sWorldNPCTweakPath, false);
    TweakEntry* entry
        = FindOrCreateTweakPath(GetTweakRoot(), sWorldNPCTweakPath, true);

    for (TweakNode* child = entry->m_ChildHead; child != 0;
        child = child->m_Next)
    {
        AddTemplate(child, GetTweakNodeName(child));
    }
    mTemplatesLoaded = true;
}

static char sWorldNPCAnimationFileKey[] = "AnimationFile";
static char sWorldNPCHierarchyFileKey[] = "HierarchyFile";
static char sWorldNPCHierarchyNameKey[] = "HierarchyName";
static char sWorldNPCTextureBundleFileKey[] = "TextureBundleFile";
static char sWorldNPCModelFileKey[] = "ModelFile";

void WorldNPCManager::AddTemplate(
    TweakNode* entry, const char* name)
{
    const char* hierarchyFile = 0;
    const char* hierarchyName = 0;
    const char* animationFile = 0;
    const char* modelFile = 0;
    const char* textureBundleFile = 0;

    for (TweakNode* child
         = ((TweakEntry*)entry)->m_ChildHead;
         child != 0; child = child->m_Next)
    {
        if (nlStrICmp(GetTweakNodeName(child), sWorldNPCAnimationFileKey) == 0)
        {
            animationFile
                = static_cast<TweakValueString*>(child->m_Value)->m_Value;
        }
        if (nlStrICmp(GetTweakNodeName(child), sWorldNPCHierarchyFileKey) == 0)
        {
            hierarchyFile
                = static_cast<TweakValueString*>(child->m_Value)->m_Value;
        }
        if (nlStrICmp(GetTweakNodeName(child), sWorldNPCHierarchyNameKey) == 0)
        {
            hierarchyName
                = static_cast<TweakValueString*>(child->m_Value)->m_Value;
        }
        if (nlStrICmp(GetTweakNodeName(child), sWorldNPCTextureBundleFileKey) == 0)
        {
            textureBundleFile
                = static_cast<TweakValueString*>(child->m_Value)->m_Value;
        }
        if (nlStrICmp(GetTweakNodeName(child), sWorldNPCModelFileKey) == 0)
        {
            modelFile
                = static_cast<TweakValueString*>(child->m_Value)->m_Value;
        }
    }

    if (hierarchyFile != 0 && hierarchyName != 0 && animationFile != 0
        && modelFile != 0 && textureBundleFile != 0)
    {
        CrowdCharacterDefinition& definition
            = mTemplates[mNumTemplates];
        definition.mName = name;
        definition.mAnimationFile = animationFile;
        definition.mHierarchyFile = hierarchyFile;
        definition.mHierarchyName = hierarchyName;
        definition.mTextureBundleFile = textureBundleFile;
        definition.mModelFile = modelFile;
        mSelectedTemplates[mNumTemplates] = false;
        ++mNumTemplates;
    }
}

void WorldNPCManager::BeginModelLoading()
{
    for (int i = 0; i < mNumTemplates; ++i)
    {
        if (mSelectedTemplates[i])
        {
            mLoadTemplates[mNumLoadTemplates] = mTemplates[i];
            ++mNumLoadTemplates;
        }
    }

    mModelCollection->Initialize(mLoadTemplates, mNumLoadTemplates);
    if (mModelCollection->HasMoreModels())
    {
        mModelCollection->BeginNextModelLoad();
    }
}

bool WorldNPCManager::UpdateModelLoading()
{
    if (mNumLoadTemplates == 0)
    {
        return true;
    }
    if (mModelCollection->UpdateModelLoad())
    {
        mModelCollection->CreateLoadedModel();
        mLoadedModels[mNumLoadedModels] = mModelCollection->mModels[mNumLoadedModels];
        ++mNumLoadedModels;

        if (mModelCollection->HasMoreModels())
        {
            mModelCollection->BeginNextModelLoad();
            return false;
        }

        mModelsLoaded = true;
        for (ListEntry<WorldNPC*>* entry = mPendingWorldNPCs.m_Head;
            entry != 0; entry = entry->next)
        {
            WorldNPC* npc = entry->entry;
            nlMatrix4 transform = npc->mTransform;
            CreateNPC(npc->mTemplateHash, transform);
        }
        return true;
    }
    return false;
}

ImpostorModel* WorldNPCManager::CreateNPC(
    unsigned long templateHash, const nlMatrix4& transform)
{
    int index;
    bool found = FindLoadedTemplate(*this, templateHash, index);

    ImpostorModel* source
        = found ? mLoadedModels[index] : 0;
    ImpostorModel* model
        = source->Clone(glGetCurrentResourcePool());
    if (mModelCallback != 0)
    {
        model->mModelCallback = GetModelCallback();
    }

    model->PlayAnimation("idle", 0.0f, PM_CYCLIC);
    model->SetAnimationTime(nlRandomf(0.0f, 1.0f, &nlDefaultSeed));
    model->mWorldMatrix = transform;
    mWorldNPCs.AddEnd(model);
    return model;
}

void WorldNPCManager::Render(GLView* view)
{
    if (gDisableWorldNPCs)
    {
        return;
    }

    ListEntry<ImpostorModel*>* entry = mWorldNPCs.m_Head;
    while (entry != 0)
    {
        ImpostorModel* model = entry->entry;
        if (mRenderFilter != 0 && !mRenderFilter(model))
        {
            entry = entry->next;
            continue;
        }
        model->Render(view, 0);
        entry = entry->next;
    }
}

void WorldNPCManager::Update(float dt)
{
    for (ListEntry<ImpostorModel*>* entry = mWorldNPCs.m_Head;
        entry != 0; entry = entry->next)
    {
        ImpostorModel* model = entry->entry;
        model->mAnimController->Update(dt);
        model->EvaluatePose();
    }
}

void WorldNPC::Initialize(WorldObjectLoadContext*)
{
    WorldNPCManager* manager = gpWorldNPCManager;
    manager->RegisterObject(this);
}

void WorldNPC::ReleaseResources()
{
}

inline void WorldNPCModelList::DeleteModelEntry(
    ListEntry<ImpostorModel*>* entry)
{
    delete entry->entry;
    delete entry;
}

WorldNPC::~WorldNPC()
{
}

nlMatrix4* WorldNPC::GetWorldMatrix()
{
    return &mTransform;
}

void WorldNPC::SetWorldMatrix(const nlMatrix4& transform)
{
    mTransform = transform;
}
