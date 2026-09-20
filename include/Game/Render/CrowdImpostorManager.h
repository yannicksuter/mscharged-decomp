#ifndef GAME_RENDER_CROWD_IMPOSTOR_MANAGER_H
#define GAME_RENDER_CROWD_IMPOSTOR_MANAGER_H

#include "Game/Render/Impostor.h"
#include "Game/Render/ImpostorCharacter.h"
#include "Game/TweakValue.h"
#include "Game/World/WorldObject_80278E94.h"
#include "NL/nlMath.h"
#include "NL/nlDLListContainer.h"
#include "types.h"

class CrowdPointCallback;
class GLView;
struct WorldObjectLoadContext;

class CrowdLayoutObject : public WorldObject_80278E94
{
public:
    virtual ~CrowdLayoutObject() {}
    virtual void ReleaseResources();
    virtual nlMatrix4* GetWorldMatrix() { return &mTransform; }
    virtual void SetWorldMatrix(const nlMatrix4& transform);
    virtual void UnidentifiedVirtual1C(WorldObjectLoadContext* context);

    void Initialize();
    void GetCorners(nlVector4* corners);
    bool ContainsLocalPoint(const nlVector3* point);
    int PlacePoints(CrowdPointCallback* callback, float rowSpacing, float memberSpacing);

    /* 0x04 */ u8 mUnidentified004[0x1C];
    /* 0x20 */ nlMatrix4 mTransform;
    /* 0x60 */ float mStartWidth;
    /* 0x64 */ float mEndWidth;
    /* 0x68 */ float mLength;
    /* 0x6C */ float mEndOffset;
    /* 0x70 */ unsigned int mIsOcclusionVolume;
    /* 0x74 */ u8 mUnidentified074[0x0C];
}; // size: 0x80

class CrowdSidelineFilter
{
public:
    virtual bool IsVisible(const Impostor* impostor);

    /* 0x04 */ TweakFloatBinding mSidelineCullingDistance;
}; // size: 0x14

struct CrowdLayoutRecord
{
    /* 0x00 */ nlVector3 mBoundsMin;
    /* 0x0C */ nlVector3 mBoundsMax;
    /* 0x18 */ CrowdLayoutObject* mObject;
    /* 0x1C */ int mFirstImpostor;
    /* 0x20 */ int mNumImpostors;
}; // size: 0x24

class CrowdImpostorManager
{
public:
    CrowdImpostorManager()
    {
        mLayouts = 0;
        mPrimaryObjectCount = 0;
        mNumLayouts = 0;
        mNumAngles = 0;
    }
    ~CrowdImpostorManager();

    void AddObject(CrowdLayoutObject* object, bool enabled);
    void AddCharacter(ImpostorCharacter* character);
    void GenerateCrowd(int reload);
    void Clear();
    void AddVisibilityFilter(CrowdSidelineFilter* filter);
    void UpdateCrowdVisibility(GLView* view);
    void ReleaseCrowdImpostors();
    bool IsObjectEnabled(CrowdLayoutObject* object);

    CrowdLayoutRecord* AllocateLayout()
    {
        if (mNumLayouts == mPrimaryObjectCount)
            return 0;

        return &mLayouts[mNumLayouts++];
    }

    /* 0x00 */ nlDLListSlotPool<ImpostorCharacter*> mCharacters;
    /* 0x1C */ nlDLListSlotPool<CrowdLayoutObject*> mPrimaryObjects;
    /* 0x38 */ nlDLListSlotPool<CrowdLayoutObject*> mOcclusionObjects;
    /* 0x54 */ nlDLListSlotPool<CrowdLayoutObject*> mEnabledObjects;
    /* 0x70 */ nlMatrix4* mInverseMatrices;
    /* 0x74 */ CrowdLayoutRecord* mLayouts;
    /* 0x78 */ int mNumLayouts;
    /* 0x7C */ int mPrimaryObjectCount;
    /* 0x80 */ int mEnabledObjectCount;
    /* 0x84 */ int mOcclusionObjectCount;
    /* 0x88 */ int mNumAngles;
    /* 0x8C */ int mNumVisibilityFilters;
    /* 0x90 */ CrowdSidelineFilter* mVisibilityFilters[5];
}; // size: 0xA4

CrowdImpostorManager* GetCrowdImpostorManager();
void GetCrowdLayoutBounds(const nlVector4* points, nlVector4* boundsMin, nlVector4* boundsMax);

#endif // GAME_RENDER_CROWD_IMPOSTOR_MANAGER_H
