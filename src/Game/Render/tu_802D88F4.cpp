#include "Game/Render/tu_802D88F4.h"
#include "Game/Render/tu_802DCDB4.h"

#include <math.h>

#include "Game/MathHelpers.h"
#include "Game/Render/ImpostorManager.h"
#include "NL/glx/GXMaterialCrystalTweaks.h"
#include "NL/nlDLListContainer.h"
#include "NL/nlMemory.h"
#include "NL/platvmath.h"

static GXMaterialFloatTweak_804F4190 sfDistanceBetweenCrowdRows(
    "sfDistanceBetweenCrowdRows", "/Render/Crowd/Layout", 0.75f);
static GXMaterialFloatTweak_804F4190 sfDistanceBetweenCrowdMembers(
    "sfDistanceBetweenCrowdMembers", "/Render/Crowd/Layout", 0.75f);
static GXMaterialFloatTweak_804F4190 sfVerticalJitterFraction(
    "sfVerticalJitterFraction", "/Render/Crowd/Layout", 0.4f);
static GXMaterialFloatTweak_804F4190 sfHorizontalJitterFraction(
    "sfHorizontalJitterFraction", "/Render/Crowd/Layout", 0.4f);
static GXMaterialFloatTweak_804F4190 sfImpostorWidth(
    "sfImpostorWidth", "/Render/Crowd", 1.0f);
static GXMaterialFloatTweak_804F4190 sfImpostorHeight(
    "sfImpostorHeight", "/Render/Crowd", 1.5f);

static bool sCrowdRegistrationDisabled;
static int sNumGeneratedCrowdMembers;
static int sNumVisibleCrowdMembers;

struct UnidentifiedCrowdLayoutRecord_802D88F4
{
    /* 0x00 */ nlVector3 mBoundsMin;
    /* 0x0C */ nlVector3 mBoundsMax;
    /* 0x18 */ UnidentifiedRenderObject_801A4188* mObject;
    /* 0x1C */ int mFirstImpostor;
    /* 0x20 */ int mNumImpostors;
}; // size: 0x24

class UnidentifiedCrowdPointCallback_802D9F64
{
public:
    UnidentifiedCrowdPointCallback_802D9F64(
        UnidentifiedRenderObject_801A4188* object)
    {
        mObject = object;
        mFirst = true;
    }

    virtual void Place(const nlVector4& point);

    /* 0x04 */ UnidentifiedRenderObject_801A4188* mObject;
    /* 0x08 */ bool mFirst;
    /* 0x09 */ u8 mPadding009[3];
    /* 0x0C */ UnidentifiedCrowdLayoutRecord_802D88F4* mLayout;
}; // size: 0x10

class UnidentifiedCrowdManager_802D88F4
{
public:
    UnidentifiedCrowdManager_802D88F4()
    {
        mLayouts = 0;
        mPrimaryObjectCount = 0;
        mNumLayouts = 0;
        mNumAngles = 0;
    }
    ~UnidentifiedCrowdManager_802D88F4();

    UnidentifiedCrowdLayoutRecord_802D88F4* AllocateLayout()
    {
        if (mNumLayouts == mPrimaryObjectCount)
            return 0;

        return &mLayouts[mNumLayouts++];
    }

    /* 0x00 */ nlDLListSlotPool<ImpostorCharacter*> mCharacters;
    /* 0x1C */ nlDLListSlotPool<UnidentifiedRenderObject_801A4188*> mPrimaryObjects;
    /* 0x38 */ nlDLListSlotPool<UnidentifiedRenderObject_801A4188*> mOcclusionObjects;
    /* 0x54 */ nlDLListSlotPool<UnidentifiedRenderObject_801A4188*> mEnabledObjects;
    /* 0x70 */ nlMatrix4* mInverseMatrices;
    /* 0x74 */ UnidentifiedCrowdLayoutRecord_802D88F4* mLayouts;
    /* 0x78 */ int mNumLayouts;
    /* 0x7C */ int mPrimaryObjectCount;
    /* 0x80 */ int mEnabledObjectCount;
    /* 0x84 */ int mOcclusionObjectCount;
    /* 0x88 */ int mNumAngles;
    /* 0x8C */ int mNumTweaks;
    /* 0x90 */ CrowdTweak_801A4188* mTweaks[5];
}; // size: 0xA4

class UnidentifiedCrowdViewProvider_802D9D00
{
public:
    virtual void UnidentifiedVirtual08() = 0;
    virtual void UnidentifiedVirtual0C() = 0;
    virtual void UnidentifiedVirtual10() = 0;
    virtual void UnidentifiedVirtual14() = 0;
    virtual void UnidentifiedVirtual18() = 0;
    virtual void UnidentifiedVirtual1C() = 0;
    virtual const nlVector4* UnidentifiedVirtual20() = 0;
};

struct UnidentifiedCrowdView_802D9D00
{
    /* 0x00 */ u8 mPadding000[0x50];
    /* 0x50 */ UnidentifiedCrowdViewProvider_802D9D00* mProvider;
};

extern "C" void fn_802C6CAC(
    const char* fileName, const char* category, bool reload);
extern "C" u16 fn_802D75AC(u16 target, int count);
extern "C" void fn_802D8950(
    UnidentifiedRenderObject_801A4188* object, nlVector4* corners);
extern "C" bool fn_802D8A94(
    UnidentifiedRenderObject_801A4188* object, const nlVector3* point);
extern "C" int fn_802D9950(UnidentifiedRenderObject_801A4188* object,
    UnidentifiedCrowdPointCallback_802D9F64* callback, float rowSpacing,
    float memberSpacing);

extern "C" void fn_802D88F4(
    UnidentifiedRenderObject_801A4188* object)
{
    fn_802D911C(fn_802D8BB4(), object, false);
}

extern "C" void fn_802D892C(UnidentifiedRenderObject_801A4188*)
{
}

extern "C" void fn_802D8930(
    UnidentifiedRenderObject_801A4188* object)
{
    object->mUnidentified070 = 0;
    object->mUnidentified060 = 0.0f;
    object->mUnidentified064 = 0.0f;
    object->mUnidentified068 = 0.0f;
    object->mUnidentified06C = 0.0f;
}

extern "C" void fn_802D8950(
    UnidentifiedRenderObject_801A4188* object, nlVector4* corners)
{
    nlVec4Set(corners[1], 0.0f, 0.0f, 0.0f, 1.0f);
    nlVec4Set(corners[0], object->mUnidentified060, 0.0f, 0.0f, 1.0f);

    float offset = 0.5f
        * (object->mUnidentified060 - object->mUnidentified064);
    nlVec4Set(corners[2], object->mUnidentified06C + offset,
        object->mUnidentified068, 0.0f, 1.0f);
    nlVec4Set(corners[3], object->mUnidentified064
            + object->mUnidentified06C + offset,
        object->mUnidentified068, 0.0f, 1.0f);
}

extern "C" void fn_802D89D4(
    const nlVector4* points, nlVector4* boundsMin, nlVector4* boundsMax)
{
    *boundsMin = points[0];
    *boundsMax = points[0];

    for (int i = 1; i < 4; ++i)
    {
        if (points[i].x < boundsMin->x)
            boundsMin->x = points[i].x;
        if (points[i].y < boundsMin->y)
            boundsMin->y = points[i].y;
        if (points[i].z < boundsMin->z)
            boundsMin->z = points[i].z;

        if (points[i].x > boundsMax->x)
            boundsMax->x = points[i].x;
        if (points[i].y > boundsMax->y)
            boundsMax->y = points[i].y;
        if (points[i].z > boundsMax->z)
            boundsMax->z = points[i].z;
    }
}

extern "C" bool fn_802D8A94(
    UnidentifiedRenderObject_801A4188* object, const nlVector3* point)
{
    nlVector4 corners[4];
    nlVector4 boundsMin;
    nlVector4 boundsMax;
    fn_802D8950(object, corners);
    fn_802D89D4(corners, &boundsMin, &boundsMax);

    float depth = 10.0f;
    if (point->x > boundsMin.x && point->x < boundsMax.x
        && point->y > boundsMin.y && point->y < boundsMax.y
        && point->z > -depth && point->z < depth)
    {
        return true;
    }
    return false;
}

extern "C" nlMatrix4* fn_802D8BAC(
    UnidentifiedRenderObject_801A4188* object)
{
    return &object->mTransform;
}

extern "C" UnidentifiedCrowdManager_802D88F4* fn_802D8BB4()
{
    static UnidentifiedCrowdManager_802D88F4 manager;
    return &manager;
}

UnidentifiedCrowdManager_802D88F4::~UnidentifiedCrowdManager_802D88F4()
{
}

extern "C" void fn_802D911C(
    UnidentifiedCrowdManager_802D88F4* manager,
    UnidentifiedRenderObject_801A4188* object, bool enabled)
{
    if (sCrowdRegistrationDisabled)
        return;

    if (enabled)
    {
        manager->mEnabledObjects.AddEnd(object);
        ++manager->mEnabledObjectCount;
    }

    if (object->mUnidentified070 != 0)
    {
        manager->mOcclusionObjects.AddEnd(object);
        ++manager->mOcclusionObjectCount;
    }
    else
    {
        manager->mPrimaryObjects.AddEnd(object);
        ++manager->mPrimaryObjectCount;
    }
}

extern "C" void fn_802D932C(
    UnidentifiedCrowdManager_802D88F4* manager,
    ImpostorCharacter* character)
{
    manager->mCharacters.AddEnd(character);
}

extern "C" void fn_802D93E8(
    UnidentifiedCrowdManager_802D88F4* manager, int reload)
{
    if (reload == 0)
        fn_802C6CAC("ini/Crowd.ini", "/Render", false);

    sNumGeneratedCrowdMembers = 0;
    if (manager->mPrimaryObjects.m_Head != 0)
        ImpostorManager::GetInstance()->SetEnabled(true);

    manager->mLayouts = new (8, false)
        UnidentifiedCrowdLayoutRecord_802D88F4[manager->mPrimaryObjectCount];
    manager->mInverseMatrices
        = new (8, false) nlMatrix4[manager->mOcclusionObjectCount];

    nlDLListIterator<UnidentifiedRenderObject_801A4188*> occlusionIt
        = manager->mOcclusionObjects.Begin();
    while (occlusionIt.m_Curr != 0)
    {
        UnidentifiedRenderObject_801A4188* object
            = occlusionIt.m_Curr->entry;
        nlInvertMatrix(*manager->mInverseMatrices,
            *object->UnidentifiedVirtual10());
        occlusionIt.Step();
    }

    nlDLListIterator<UnidentifiedRenderObject_801A4188*> objectIt
        = manager->mPrimaryObjects.Begin();
    while (objectIt.m_Curr != 0)
    {
        UnidentifiedRenderObject_801A4188* object = objectIt.m_Curr->entry;
        UnidentifiedCrowdPointCallback_802D9F64 callback(object);
        callback.mLayout = fn_802D8BB4()->AllocateLayout();
        callback.mLayout->mObject = object;
        sNumGeneratedCrowdMembers += fn_802D9950(object, &callback,
            sfDistanceBetweenCrowdRows.value,
            sfDistanceBetweenCrowdMembers.value);
        objectIt.Step();
    }
}

extern "C" void fn_802D9708(
    UnidentifiedCrowdManager_802D88F4* manager)
{
    manager->mPrimaryObjects.Clear();
    manager->mPrimaryObjects.m_Allocator.FreeBlocks();
    manager->mPrimaryObjectCount = 0;

    manager->mOcclusionObjects.Clear();
    manager->mOcclusionObjects.m_Allocator.FreeBlocks();
    manager->mOcclusionObjectCount = 0;

    manager->mEnabledObjects.Clear();
    manager->mEnabledObjects.m_Allocator.FreeBlocks();
    manager->mEnabledObjectCount = 0;

    manager->mCharacters.Clear();
    manager->mCharacters.m_Allocator.FreeBlocks();
    manager->mNumTweaks = 0;

    if (manager->mLayouts != 0)
    {
        delete manager->mLayouts;
        manager->mLayouts = 0;
    }

    manager->mNumLayouts = 0;
    if (manager->mInverseMatrices != 0)
        delete manager->mInverseMatrices;
}

static inline void InterpolateCrowdPoint(nlVector4& result,
    const nlVector4& first, const nlVector4& second, float amount)
{
    float inverse = 1.0f - amount;
    result.x = inverse * first.x + amount * second.x;
    result.y = inverse * first.y + amount * second.y;
    result.z = inverse * first.z + amount * second.z;
    result.w = inverse * first.w + amount * second.w;
}

extern "C" int fn_802D9950(UnidentifiedRenderObject_801A4188* object,
    UnidentifiedCrowdPointCallback_802D9F64* callback, float rowSpacing,
    float memberSpacing)
{
    nlVector4 corners[4];
    fn_802D8950(object, corners);

    int total = 0;
    int numRows = (int)floor(object->mUnidentified068 / rowSpacing) + 1;
    float rowOffset = 0.5f
        * (object->mUnidentified068 - (numRows - 1) * rowSpacing)
        / object->mUnidentified068;

    for (int row = 0; row < numRows; ++row)
    {
        float rowAmount
            = row * rowSpacing / object->mUnidentified068 + rowOffset;
        nlVector4 left;
        nlVector4 right;
        InterpolateCrowdPoint(left, corners[0], corners[2], rowAmount);
        InterpolateCrowdPoint(right, corners[1], corners[3], rowAmount);

        float dx = left.x - right.x;
        float dy = left.y - right.y;
        float dz = left.z - right.z;
        float rowLength = nlSqrt(dx * dx + dy * dy + dz * dz, true);
        int numMembers = (int)floor(rowLength / memberSpacing) + 1;
        float memberOffset
            = 0.5f * (rowLength - (numMembers - 1) * memberSpacing)
            / rowLength;

        for (int member = 0; member < numMembers; ++member)
        {
            float memberAmount
                = member * memberSpacing / rowLength + memberOffset;
            nlVector4 point;
            InterpolateCrowdPoint(
                point, left, right, memberAmount);
            callback->Place(point);
            ++total;
        }
    }

    return total;
}

extern "C" void fn_802D9CD8(
    UnidentifiedCrowdManager_802D88F4* manager,
    CrowdTweak_801A4188* tweak)
{
    if (manager->mNumTweaks >= 5)
        return;

    manager->mTweaks[manager->mNumTweaks] = tweak;
    ++manager->mNumTweaks;
}

extern "C" void fn_802D9D00(
    UnidentifiedCrowdManager_802D88F4* manager, void* view)
{
    Impostor* impostors = ImpostorManager::GetInstance()->mImpostors;
    ImpostorManager::GetInstance()->ResetSpriteSlots();
    sNumVisibleCrowdMembers = 0;

    UnidentifiedCrowdView_802D9D00* crowdView
        = (UnidentifiedCrowdView_802D9D00*)view;
    for (int layoutIndex = 0; layoutIndex < manager->mNumLayouts;
        ++layoutIndex)
    {
        UnidentifiedCrowdLayoutRecord_802D88F4& layout
            = manager->mLayouts[layoutIndex];
        const nlVector4* visibilityView
            = crowdView->mProvider->UnidentifiedVirtual20();
        if (!fn_802DD05C(visibilityView, &layout.mBoundsMin,
                &layout.mBoundsMax, 0))
        {
            continue;
        }

        for (int i = 0; i < layout.mNumImpostors; ++i)
        {
            Impostor* impostor
                = &impostors[layout.mFirstImpostor + i];
            if (impostor->mUnidentified02C)
                continue;

            bool release = true;
            for (int tweak = 0; tweak < manager->mNumTweaks; ++tweak)
            {
                if (!manager->mTweaks[tweak]
                         ->UnidentifiedVirtual08(impostor))
                {
                    release = false;
                    break;
                }
            }
            if (release)
                impostor->Release();
        }
        sNumVisibleCrowdMembers += layout.mNumImpostors;
    }
}

extern "C" void fn_802D9E34(
    UnidentifiedCrowdManager_802D88F4* manager)
{
    Impostor* impostors = ImpostorManager::GetInstance()->mImpostors;
    sNumVisibleCrowdMembers = 0;

    for (int layoutIndex = 0; layoutIndex < manager->mNumLayouts;
        ++layoutIndex)
    {
        UnidentifiedCrowdLayoutRecord_802D88F4& layout
            = manager->mLayouts[layoutIndex];
        bool found = false;
        nlDLListIterator<UnidentifiedRenderObject_801A4188*> objectIt
            = manager->mEnabledObjects.Begin();
        while (objectIt.m_Curr != 0)
        {
            if (layout.mObject == objectIt.m_Curr->entry)
            {
                found = true;
                break;
            }
            objectIt.Step();
        }

        if (!found)
            continue;

        for (int i = 0; i < layout.mNumImpostors; ++i)
            impostors[layout.mFirstImpostor + i].Release();
        sNumVisibleCrowdMembers += layout.mNumImpostors;
    }
}

void UnidentifiedCrowdPointCallback_802D9F64::Place(
    const nlVector4& point)
{
    float horizontalJitter = sfDistanceBetweenCrowdMembers.value
        * sfHorizontalJitterFraction.value;
    float verticalJitter = sfDistanceBetweenCrowdRows.value
        * sfVerticalJitterFraction.value;

    nlVector4 localPoint;
    localPoint.x = point.x
        + nlRandomf(-horizontalJitter, horizontalJitter, &nlDefaultSeed);
    localPoint.y = point.y
        + nlRandomf(-verticalJitter, verticalJitter, &nlDefaultSeed);
    localPoint.z = point.z;
    localPoint.w = 1.0f;

    nlVector4 worldPoint;
    nlMultVectorMatrix(
        worldPoint, localPoint, *mObject->UnidentifiedVirtual10());

    UnidentifiedCrowdManager_802D88F4* manager = fn_802D8BB4();
    nlDLListIterator<UnidentifiedRenderObject_801A4188*> occlusionIt
        = manager->mOcclusionObjects.Begin();
    while (occlusionIt.m_Curr != 0)
    {
        nlVector4 occlusionPoint;
        nlMultVectorMatrix(
            occlusionPoint, worldPoint, manager->mInverseMatrices[0]);
        if (fn_802D8A94(occlusionIt.m_Curr->entry,
                (nlVector3*)&occlusionPoint))
        {
            return;
        }
        occlusionIt.Step();
    }

    int numCharacters
        = nlDLRingCountElements(manager->mCharacters.m_Head);
    int characterIndex = nlRandom(numCharacters, &nlDefaultSeed);
    nlDLListIterator<ImpostorCharacter*> characterIt
        = manager->mCharacters.Begin();
    while (characterIndex-- > 0)
        characterIt.Step();
    ImpostorCharacter* character = characterIt.m_Curr->entry;

    if (manager->mNumAngles == 0)
        manager->mNumAngles = character->mNumAngles;

    static const nlVector4 facing = { 0.0f, -1.0f, 0.0f, 0.0f };
    nlVector4 worldFacing;
    nlMultVectorMatrix(
        worldFacing, facing, *mObject->UnidentifiedVirtual10());
    u16 angle = fn_802D75AC(nlVector3ToAngle(*(nlVector3*)&worldFacing),
        manager->mNumAngles);

    int impostorIndex = -1;
    Impostor* impostor
        = ImpostorManager::GetInstance()->AllocImpostor(&impostorIndex);
    if (impostor == 0)
        return;

    impostor->Set(character, *(nlVector3*)&worldPoint, angle,
        sfImpostorWidth.value, sfImpostorHeight.value);

    nlDLListIterator<UnidentifiedRenderObject_801A4188*> enabledIt
        = fn_802D8BB4()->mEnabledObjects.Begin();
    while (enabledIt.m_Curr != 0)
    {
        if (mObject == enabledIt.m_Curr->entry)
        {
            impostor->mUnidentified02C = true;
            break;
        }
        enabledIt.Step();
    }

    if (mFirst)
    {
        mLayout->mFirstImpostor = impostorIndex;
        mLayout->mNumImpostors = 0;
    }
    ++mLayout->mNumImpostors;

    nlVector3 boundsMin = { worldPoint.x - sfImpostorWidth.value,
        worldPoint.y - sfImpostorWidth.value, worldPoint.z };
    nlVector3 boundsMax = { worldPoint.x + sfImpostorWidth.value,
        worldPoint.y + sfImpostorWidth.value,
        worldPoint.z + sfImpostorHeight.value };

    if (mFirst)
    {
        mLayout->mBoundsMin = boundsMin;
        mLayout->mBoundsMax = boundsMax;
        mFirst = false;
        return;
    }

    if (boundsMin.x < mLayout->mBoundsMin.x)
        mLayout->mBoundsMin.x = boundsMin.x;
    if (boundsMin.y < mLayout->mBoundsMin.y)
        mLayout->mBoundsMin.y = boundsMin.y;
    if (boundsMin.z < mLayout->mBoundsMin.z)
        mLayout->mBoundsMin.z = boundsMin.z;
    if (boundsMax.x > mLayout->mBoundsMax.x)
        mLayout->mBoundsMax.x = boundsMax.x;
    if (boundsMax.y > mLayout->mBoundsMax.y)
        mLayout->mBoundsMax.y = boundsMax.y;
    if (boundsMax.z > mLayout->mBoundsMax.z)
        mLayout->mBoundsMax.z = boundsMax.z;
}
