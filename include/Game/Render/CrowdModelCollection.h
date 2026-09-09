#ifndef GAME_RENDER_CROWD_MODEL_COLLECTION_H
#define GAME_RENDER_CROWD_MODEL_COLLECTION_H

#include "types.h"

class cSAnim;
class cSHierarchy;
class ImpostorModel;
template <typename T>
class cInventory;

struct CrowdCharacterDefinition
{
    const char* mName;
    const char* mAnimationFile;
    const char* mHierarchyFile;
    const char* mHierarchyName;
    const char* mTextureBundleFile;
    const char* mModelFile;
}; // size: 0x18

class CrowdModelCollection
{
public:
    CrowdModelCollection();
    ~CrowdModelCollection();

    void Clear();
    void Initialize(CrowdCharacterDefinition* definitions, int count);
    bool HasMoreModels();
    void BeginNextModelLoad();
    bool UpdateModelLoad();
    void CreateLoadedModel();

    static void TextureBundleLoadCallback(void* data, unsigned long size, void* userData);
    static void ModelLoadCallback(void* data, unsigned long size, void* userData);
    static void HierarchyLoadCallback(void* data, unsigned long size, void* userData);
    static void AnimationLoadCallback(void* data, unsigned long size, void* userData);

    /* 0x00 */ bool mTexturesLoaded;
    /* 0x01 */ bool mModelsLoaded;
    /* 0x02 */ bool mHierarchiesLoaded;
    /* 0x03 */ bool mAnimationsLoaded;
    /* 0x04 */ cInventory<cSAnim>* mCurrentAnimationInventory;
    /* 0x08 */ cInventory<cSHierarchy>* mHierarchyInventory;
    /* 0x0C */ int mNumDefinitions;
    /* 0x10 */ CrowdCharacterDefinition* mDefinitions;
    /* 0x14 */ int mLoadIndex;
    /* 0x18 */ void* mTextureBundleData;
    /* 0x1C */ unsigned long mTextureBundleSize;
    /* 0x20 */ void* mUnidentified020;
    /* 0x24 */ u32 mUnidentified024;
    /* 0x28 */ void* mAnimationData;
    /* 0x2C */ unsigned long mAnimationSize;
    /* 0x30 */ void* mModelData;
    /* 0x34 */ unsigned long mModelSize;
    /* 0x38 */ void* mHierarchyData;
    /* 0x3C */ unsigned long mHierarchySize;
    /* 0x40 */ unsigned long mModelHash;
    /* 0x44 */ u32 mUnidentified044;
    /* 0x48 */ ImpostorModel** mModels;
    /* 0x4C */ cInventory<cSAnim>** mAnimationInventories;
}; // size: 0x50

extern CrowdModelCollection gCrowdModelCollection;

#endif // GAME_RENDER_CROWD_MODEL_COLLECTION_H
