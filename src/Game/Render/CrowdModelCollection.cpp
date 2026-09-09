#include "Game/World/WorldVisibility.h"
#include "NL/nlCompressedFile.h"
#include "Game/Render/CrowdModelCollection.h"

#include "Game/Inventory.h"
#include "Game/Render/Frustum.h"
#include "Game/Render/ImpostorCharacter.h"
#include "Game/SHierarchy.h"
#include "NL/MemAlloc.h"
#include "NL/gl/gl.h"
#include "NL/gl/glMemory.h"
#include "NL/gl/glTexture.h"
#include "NL/nlFile.h"
#include "NL/nlMemory.h"

#include <string.h>


static bool sLoadCrowdModelsSynchronously;
static nlChunk* spWorldVisibilityChunk;
static char sCrowdAnimationCompressionExtension[] = ".zlib";

CrowdModelCollection::CrowdModelCollection()
{
    mTexturesLoaded = false;
    mModelsLoaded = false;
    mHierarchiesLoaded = false;
    mAnimationsLoaded = false;
    mCurrentAnimationInventory = 0;
    mHierarchyInventory = 0;
    mNumDefinitions = 0;
    mDefinitions = 0;
    mLoadIndex = 0;
    mTextureBundleData = 0;
    mUnidentified020 = 0;
    mAnimationData = 0;
    mHierarchyData = 0;
    mModels = 0;
}

CrowdModelCollection::~CrowdModelCollection()
{
    Clear();
}

void CrowdModelCollection::Clear()
{
    if (mModels != 0)
    {
        delete[] mModels;
        mModels = 0;
    }

    for (int i = 0; i < mNumDefinitions; ++i)
    {
        delete mAnimationInventories[i];
    }

    if (mAnimationInventories != 0)
    {
        delete[] mAnimationInventories;
        mAnimationInventories = 0;
    }

    delete mHierarchyInventory;

    mHierarchyInventory = 0;
    mLoadIndex = 0;
    mHierarchiesLoaded = false;
    mAnimationsLoaded = false;
    mModelsLoaded = false;
    mTexturesLoaded = false;
}

void CrowdModelCollection::Initialize(CrowdCharacterDefinition* definitions, int count)
{
    mDefinitions = definitions;
    mNumDefinitions = count;
    mModels = new (8, false) ImpostorModel*[count];
    mAnimationInventories = new (8, false) cInventory<cSAnim>*[count];

    for (int i = 0; i < count; ++i)
    {
        mModels[i] = 0;
        mAnimationInventories[i] = 0;
    }
}

bool CrowdModelCollection::HasMoreModels()
{
    return mLoadIndex < mNumDefinitions;
}

void CrowdModelCollection::BeginNextModelLoad()
{
    mTexturesLoaded = false;
    mModelsLoaded = false;
    mHierarchiesLoaded = false;
    mAnimationsLoaded = false;

    if (mHierarchyInventory == 0)
    {
        mHierarchyInventory
            = new (8, false) cInventory<cSHierarchy>;
    }

    CrowdCharacterDefinition& definition
        = mDefinitions[mLoadIndex];

    mTextureBundleData = 0;
    mTextureBundleSize = 0;
    if (sLoadCrowdModelsSynchronously)
    {
        glLoadTextureBundle(
            definition.mTextureBundleFile, glGetCurrentResourcePool());
        mTextureBundleData = (void*)-1;
    }
    else
    {
        glBeginLoadTextureBundle(definition.mTextureBundleFile, TextureBundleLoadCallback,
            this, glGetCurrentResourcePool());
    }

    mModelData = 0;
    mModelSize = 0;
    mModelHash = 0;
    if (sLoadCrowdModelsSynchronously)
    {
        unsigned long numModels;
        unsigned int* models = (unsigned int*)glLoadModel(
            definition.mModelFile, &numModels, glGetCurrentResourcePool());
        mModelHash = *models;
    }
    else
    {
        glBeginLoadModel(definition.mModelFile, ModelLoadCallback,
            this, glGetCurrentResourcePool());
    }

    mHierarchyData = 0;
    mHierarchySize = 0;
    CurrentAllocator = &StandardAllocator;
    AllocatorStack[AllocatorStackDepth++] = &StandardAllocator;
    if (sLoadCrowdModelsSynchronously)
    {
        mHierarchyData = nlLoadEntireFile(
            definition.mHierarchyFile, &mHierarchySize,
            0x20, AllocateStart, 0, 0, 0);
    }
    else
    {
        nlLoadEntireFileAsync(definition.mHierarchyFile, HierarchyLoadCallback,
            this, 0x20, AllocateStart, 0, 0, 0);
    }

    mAnimationData = 0;
    mAnimationSize = 0;
    CurrentAllocator = &StandardAllocator;
    AllocatorStack[AllocatorStackDepth++] = &StandardAllocator;
    mCurrentAnimationInventory = 0;

    const char* path = definition.mAnimationFile;
    if (sLoadCrowdModelsSynchronously)
    {
        if (strstr(path, sCrowdAnimationCompressionExtension) == 0)
        {
            mAnimationData = nlLoadEntireFile(path,
                &mAnimationSize, 0x20, AllocateStart,
                0, 0, 0);
        }
    }
    else if (strstr(path, sCrowdAnimationCompressionExtension) != 0)
    {
        nlLoadCompressedFileAsync(path, AnimationLoadCallback, this, 0x20,
            AllocateStart, 0x10000, 0, 0, 0, 0, 0);
    }
    else
    {
        nlLoadEntireFileAsync(path, AnimationLoadCallback, this,
            0x20, AllocateStart, 0, 0, 0);
    }
}

bool CrowdModelCollection::UpdateModelLoad()
{
    if (!mTexturesLoaded)
    {
        if (mTextureBundleData == (void*)-1)
        {
            mTexturesLoaded = true;
        }
        else if (mTextureBundleData == 0)
        {
            mTexturesLoaded = false;
        }
        else
        {
            glEndLoadTextureBundle(mTextureBundleData,
                mTextureBundleSize,
                glGetCurrentResourcePool(), 0);
            nlFree(mTextureBundleData);
            mTextureBundleData = 0;
            mTexturesLoaded = true;
        }
    }

    if (!mModelsLoaded)
    {
        if (mModelHash != 0)
        {
            mModelsLoaded = true;
        }
        else if (mModelData == 0)
        {
            mModelsLoaded = false;
        }
        else
        {
            unsigned long numModels = 0;
            unsigned int* models = (unsigned int*)glEndLoadModel(
                mModelData, mModelSize,
                &numModels, glGetCurrentResourcePool());
            nlFree(mModelData);
            mModelData = 0;
            mModelHash = *models;
            mModelsLoaded = true;
        }
    }

    if (!mHierarchiesLoaded)
    {
        if (mHierarchyData == 0)
        {
            mHierarchiesLoaded = false;
        }
        else
        {
            mHierarchyInventory->AddFile(
                (char*)mHierarchyData,
                mHierarchySize);

            --AllocatorStackDepth;
            AllocatorStack[AllocatorStackDepth] = 0;
            CurrentAllocator = AllocatorStack[AllocatorStackDepth - 1];
            mHierarchiesLoaded = true;
        }
    }

    if (!mAnimationsLoaded)
    {
        if (mAnimationData == 0)
        {
            mAnimationsLoaded = false;
        }
        else
        {
            mCurrentAnimationInventory
                = new (8, false) cInventory<cSAnim>;
            mCurrentAnimationInventory->AddFile(
                (char*)mAnimationData,
                mAnimationSize);
            mAnimationInventories[mLoadIndex]
                = mCurrentAnimationInventory;

            --AllocatorStackDepth;
            AllocatorStack[AllocatorStackDepth] = 0;
            CurrentAllocator = AllocatorStack[AllocatorStackDepth - 1];
            mAnimationsLoaded = true;
        }
    }

    return mTexturesLoaded
        && mModelsLoaded
        && mHierarchiesLoaded
        && mAnimationsLoaded;
}

void CrowdModelCollection::CreateLoadedModel()
{
    ImpostorModel* model
        = new (8, false) ImpostorModel(
            *mHierarchyInventory->Find(
                const_cast<char*>(mDefinitions[mLoadIndex].mHierarchyName)),
            mModelHash, mCurrentAnimationInventory,
            glGetCurrentResourcePool());
    mModels[mLoadIndex] = model;
    ++mLoadIndex;
}

void CrowdModelCollection::TextureBundleLoadCallback(
    void* data, unsigned long size, void* userData)
{
    CrowdModelCollection* collection
        = (CrowdModelCollection*)userData;
    collection->mTextureBundleData = data;
    collection->mTextureBundleSize = size;
}

void CrowdModelCollection::ModelLoadCallback(
    void* data, unsigned long size, void* userData)
{
    CrowdModelCollection* collection
        = (CrowdModelCollection*)userData;
    collection->mModelData = data;
    collection->mModelSize = size;
}

void CrowdModelCollection::HierarchyLoadCallback(
    void* data, unsigned long size, void* userData)
{
    CrowdModelCollection* collection
        = (CrowdModelCollection*)userData;
    collection->mHierarchyData = data;
    collection->mHierarchySize = size;
}

void CrowdModelCollection::AnimationLoadCallback(
    void* data, unsigned long size, void* userData)
{
    CrowdModelCollection* collection
        = (CrowdModelCollection*)userData;
    collection->mAnimationData = data;
    collection->mAnimationSize = size;
}

WorldVisibilityNode* LoadWorldVisibilityNode(nlChunk* chunk)
{
    spWorldVisibilityChunk = chunk;
    WorldVisibilityNode* node
        = (WorldVisibilityNode*)chunk->GetData();

    if (node->mNumModelHashes != 0)
    {
        node->mModelHashes = node->mModelHashData;
    }

    for (int i = 0; i < 2; ++i)
    {
        if (node->mChildren[i] != 0)
        {
            spWorldVisibilityChunk = spWorldVisibilityChunk->GetNextChunk();
            node->mChildren[i] = LoadWorldVisibilityNode(spWorldVisibilityChunk);
        }
    }
    return node;
}

WorldVisibilityNode* LoadWorldVisibilityTree(nlChunk* chunk)
{
    return LoadWorldVisibilityNode((nlChunk*)chunk->GetData());
}

void UpdateWorldVisibilityNode(WorldVisibilityNode* node,
    const nlVector4* pPlanes, WorldVisibilityCallback callback,
    unsigned long planeMask, int state)
{
    FrustumResult result;
    if (state == 0)
    {
        result = FRUSTUM_OUTSIDE;
    }
    else if (state == 1)
    {
        result = FRUSTUM_INSIDE;
    }
    else
    {
        result = ClassifyBoxInFrustum(pPlanes,
            &node->mBoundsMin,
            &node->mBoundsMax, &planeMask);
    }

    if (result)
    {
        node->mVisible = 1;
        if (callback != 0)
        {
            callback(node);
        }
    }
    else
    {
        node->mVisible = 0;
    }

    for (int i = 0; i < 2; ++i)
    {
        if (node->mChildren[i] != 0)
        {
            UpdateWorldVisibilityNode(node->mChildren[i], pPlanes,
                callback, planeMask, result);
        }
    }
}

void UpdateWorldVisibility(WorldVisibilityNode* node,
    const nlVector4* pPlanes, WorldVisibilityCallback callback)
{
    UpdateWorldVisibilityNode(node, pPlanes, callback, 0, 2);
}
