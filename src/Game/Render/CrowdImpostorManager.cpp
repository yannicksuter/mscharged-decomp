#include "Game/Render/CrowdImpostorManager.h"
#include "Game/Render/Frustum.h"
#include "NL/gl/glView.h"

#include <math.h>

#include "Game/MathHelpers.h"
#include "Game/Render/ImpostorManager.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/TweakConfig.h"
#include "Game/TweakValue.h"
#include "Game/TweakValueFloat.h"
#include "NL/nlDLListContainer.h"
#include "NL/nlMemory.h"
#include "NL/platvmath.h"

static TweakValueFloat sfDistanceBetweenCrowdRows(
    "sfDistanceBetweenCrowdRows", "/Render/Crowd/Layout", 0.75f);
static TweakValueFloat sfDistanceBetweenCrowdMembers(
    "sfDistanceBetweenCrowdMembers", "/Render/Crowd/Layout", 0.75f);
static TweakValueFloat sfVerticalJitterFraction(
    "sfVerticalJitterFraction", "/Render/Crowd/Layout", 0.4f);
static TweakValueFloat sfHorizontalJitterFraction(
    "sfHorizontalJitterFraction", "/Render/Crowd/Layout", 0.4f);
static TweakValueFloat sfImpostorWidth(
    "sfImpostorWidth", "/Render/Crowd", 1.0f);
static TweakValueFloat sfImpostorHeight(
    "sfImpostorHeight", "/Render/Crowd", 1.5f);

static bool sCrowdRegistrationDisabled;
static int sNumGeneratedCrowdMembers;
static int sNumVisibleCrowdMembers;

class CrowdPointCallback
{
public:
    CrowdPointCallback(
        CrowdLayoutObject* object)
    {
        mObject = object;
        mFirst = true;
    }

    virtual void Place(nlVector4 point);

    /* 0x04 */ CrowdLayoutObject* mObject;
    /* 0x08 */ bool mFirst;
    /* 0x09 */ u8 mPadding009[3];
    /* 0x0C */ CrowdLayoutRecord* mLayout;
}; // size: 0x10

extern "C" nlMatrix4* fn_802D8BAC(
    CrowdLayoutObject* object)
{
    return &object->mTransform;
}

CrowdImpostorManager* GetCrowdImpostorManager()
{
    static CrowdImpostorManager manager;
    return &manager;
}

CrowdImpostorManager::~CrowdImpostorManager()
{
}

void CrowdImpostorManager::AddObject(CrowdLayoutObject* object, bool enabled)
{
    if (sCrowdRegistrationDisabled)
        return;

    if (enabled)
    {
        mEnabledObjects.AddEnd(object);
        ++mEnabledObjectCount;
    }

    if (object->mIsOcclusionVolume != 0)
    {
        mOcclusionObjects.AddEnd(object);
        ++mOcclusionObjectCount;
    }
    else
    {
        mPrimaryObjects.AddEnd(object);
        ++mPrimaryObjectCount;
    }
}

void CrowdImpostorManager::AddCharacter(ImpostorCharacter* character)
{
    mCharacters.AddEnd(character);
}

void CrowdImpostorManager::GenerateCrowd(int reload)
{
    if (reload == 0)
        LoadTweakConfigFile("ini/Crowd.ini", "/Render", false);

    sNumGeneratedCrowdMembers = 0;
    if (mPrimaryObjects.m_Head != 0)
        ImpostorManager::GetInstance()->SetEnabled(true);

    mLayouts = new (8, false)
        CrowdLayoutRecord[mPrimaryObjectCount];
    mInverseMatrices
        = new (8, false) nlMatrix4[mOcclusionObjectCount];

    nlDLListIterator<CrowdLayoutObject*> occlusionIt
        = mOcclusionObjects.Begin();
    while (occlusionIt.m_Curr != 0)
    {
        CrowdLayoutObject* object
            = occlusionIt.m_Curr->entry;
        nlInvertMatrix(*mInverseMatrices,
            *object->UnidentifiedVirtual10());
        occlusionIt.Step();
    }

    nlDLListIterator<CrowdLayoutObject*> objectIt
        = mPrimaryObjects.Begin();
    while (objectIt.m_Curr != 0)
    {
        CrowdLayoutObject* object = objectIt.m_Curr->entry;
        CrowdPointCallback callback(object);
        callback.mLayout = GetCrowdImpostorManager()->AllocateLayout();
        callback.mLayout->mObject = object;
        sNumGeneratedCrowdMembers += object->PlacePoints(&callback,
            sfDistanceBetweenCrowdRows.value,
            sfDistanceBetweenCrowdMembers.value);
        objectIt.Step();
    }
}

void CrowdImpostorManager::Clear()
{
    BasicSlotPool<DLListEntry<CrowdLayoutObject*> >* objectPool;

    mPrimaryObjects.Clear();
    objectPool = &mPrimaryObjects.m_Allocator;
    objectPool->FreeBlocks();
    mPrimaryObjectCount = 0;

    mOcclusionObjects.Clear();
    objectPool = &mOcclusionObjects.m_Allocator;
    objectPool->FreeBlocks();
    mOcclusionObjectCount = 0;

    mEnabledObjects.Clear();
    objectPool = &mEnabledObjects.m_Allocator;
    objectPool->FreeBlocks();
    mEnabledObjectCount = 0;

    mCharacters.Clear();
    mCharacters.m_Allocator.FreeBlocks();
    mNumVisibilityFilters = 0;

    if (mLayouts != 0)
    {
        delete mLayouts;
        mLayouts = 0;
    }

    mNumLayouts = 0;
    if (mInverseMatrices != 0)
        delete mInverseMatrices;
}

static inline void InterpolateCrowdPoint(const nlVector4& first,
    const nlVector4& second, float amount, nlVector4& result)
{
    nlVec4Set(result,
        (1.0f - amount) * first.x + amount * second.x,
        (1.0f - amount) * first.y + amount * second.y,
        (1.0f - amount) * first.z + amount * second.z,
        (1.0f - amount) * first.w + amount * second.w);
}

int CrowdLayoutObject::PlacePoints(CrowdPointCallback* callback, float rowSpacing, float memberSpacing)
{
    nlVector4 corners[4];
    int total = 0;
    GetCorners(corners);

    int numRows = (int)floorf(mLength / rowSpacing) + 1;

    for (int row = 0; row < numRows; ++row)
    {
        float rowStep = rowSpacing / mLength;
        float occupiedLength = (numRows - 1) * rowSpacing;
        float rowOffset = 0.5f
            * (mLength - occupiedLength)
            / mLength;
        float rowAmount
            = row * rowStep + rowOffset;
        nlVector4 right;
        nlVector4 left;
        InterpolateCrowdPoint(corners[3], corners[0], rowAmount, left);
        InterpolateCrowdPoint(corners[2], corners[1], rowAmount, right);

        float rowLength = nlSqrt(
            CalculateDistanceSquared(*(const nlVector3*)&left,
                *(const nlVector3*)&right),
            true);
        int numMembers = (int)floorf(rowLength / memberSpacing) + 1;
        float memberStep = memberSpacing / rowLength;

        for (int member = 0; member < numMembers; ++member)
        {
            float occupiedWidth = (numMembers - 1) * memberSpacing;
            float memberOffset
                = 0.5f * (rowLength - occupiedWidth)
                / rowLength;
            float memberAmount
                = member * memberStep + memberOffset;
            nlVector4 point;
            InterpolateCrowdPoint(
                right, left, memberAmount, point);
            callback->Place(point);
            ++total;
        }
    }

    return total;
}

void CrowdImpostorManager::AddVisibilityFilter(CrowdSidelineFilter* filter)
{
    if (mNumVisibilityFilters >= 5)
        return;

    mVisibilityFilters[mNumVisibilityFilters] = filter;
    ++mNumVisibilityFilters;
}

void CrowdImpostorManager::UpdateCrowdVisibility(GLView* view)
{
    Impostor* impostors = ImpostorManager::GetInstance()->mImpostors;
    ImpostorManager::GetInstance()->ResetSpriteSlots();
    sNumVisibleCrowdMembers = 0;

    for (int layoutIndex = 0; layoutIndex < mNumLayouts;
        ++layoutIndex)
    {
        CrowdLayoutRecord& layout
            = mLayouts[layoutIndex];
        const nlVector4* visibilityView
            = view->m_Interface->GetShadowMatrix();
        if (!ClassifyBoxInFrustum(visibilityView, &layout.mBoundsMin,
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

            bool visible = true;
            for (int filter = 0; filter < mNumVisibilityFilters; ++filter)
            {
                if (!mVisibilityFilters[filter]
                         ->IsVisible(impostor))
                {
                    visible = false;
                    break;
                }
            }
            if (visible)
                impostor->Release();
        }
        sNumVisibleCrowdMembers += layout.mNumImpostors;
    }
}

inline bool CrowdImpostorManager::IsObjectEnabled(CrowdLayoutObject* object)
{
    nlDLListIterator<CrowdLayoutObject*> objectIt
        = mEnabledObjects.Begin();
    while (objectIt.hasNext())
    {
        if (object == *objectIt)
            return true;
        objectIt.Step();
    }
    return false;
}

void CrowdImpostorManager::ReleaseCrowdImpostors()
{
    Impostor* impostors = ImpostorManager::GetInstance()->mImpostors;
    sNumVisibleCrowdMembers = 0;

    for (int layoutIndex = 0; layoutIndex < mNumLayouts;
        ++layoutIndex)
    {
        CrowdLayoutRecord& layout
            = mLayouts[layoutIndex];
        if (!IsObjectEnabled(layout.mObject))
            continue;

        for (int i = 0; i < layout.mNumImpostors; ++i)
            impostors[layout.mFirstImpostor + i].Release();
        sNumVisibleCrowdMembers += layout.mNumImpostors;
    }
}

void CrowdPointCallback::Place(
    nlVector4 point)
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

    CrowdImpostorManager* manager = GetCrowdImpostorManager();
    nlDLListIterator<CrowdLayoutObject*> occlusionIt
        = manager->mOcclusionObjects.Begin();
    nlVector4 occlusionPoint;
    bool occluded;
    for (;;)
    {
        if (!occlusionIt.hasNext())
        {
            occluded = false;
            break;
        }
        CrowdLayoutObject* object = *occlusionIt;
        nlMultVectorMatrix(
            occlusionPoint, worldPoint, manager->mInverseMatrices[0]);
        if (object->ContainsLocalPoint((nlVector3*)&occlusionPoint))
        {
            occluded = true;
            break;
        }
        occlusionIt.Step();
    }
    if (occluded)
        return;

    manager = GetCrowdImpostorManager();
    int numCharacters
        = nlDLRingCountElements(manager->mCharacters.m_Head);
    int characterIndex = nlRandom(numCharacters, &nlDefaultSeed);
    nlDLListIterator<ImpostorCharacter*> characterIt
        = manager->mCharacters.Begin();
    while (characterIndex-- > 0)
        characterIt.Step();
    ImpostorCharacter* character = characterIt.m_Curr->entry;

    if (GetCrowdImpostorManager()->mNumAngles == 0)
        GetCrowdImpostorManager()->mNumAngles = character->mNumAngles;

    int numAngles = GetCrowdImpostorManager()->mNumAngles;
    nlVector4 transformedFacing;
    nlVector4 facing = { 0.0f, -1.0f, 0.0f, 0.0f };
    nlMultVectorMatrix(transformedFacing, facing,
        *mObject->UnidentifiedVirtual10());
    facing = transformedFacing;
    u16 angle = QuantizeImpostorAngle(nlVector3ToAngle(*(nlVector3*)&facing),
        numAngles);

    int impostorIndex = -1;
    Impostor* impostor
        = ImpostorManager::GetInstance()->AllocImpostor(&impostorIndex);
    if (impostor == 0)
        return;

    impostor->Set(character, *(nlVector3*)&worldPoint, angle,
        sfImpostorWidth.value, sfImpostorHeight.value);

    if (GetCrowdImpostorManager()->IsObjectEnabled(mObject))
        impostor->mUnidentified02C = true;

    if (mFirst)
    {
        mLayout->mFirstImpostor = impostorIndex;
        mLayout->mNumImpostors = 0;
    }
    ++mLayout->mNumImpostors;
    CrowdLayoutRecord* layout = mLayout;

    nlVector3 boundsMax;
    nlVector3 boundsMin = *(const nlVector3*)&worldPoint;
    boundsMin.x -= sfImpostorWidth.value;
    boundsMin.y -= sfImpostorWidth.value;
    boundsMax = *(const nlVector3*)&worldPoint;
    boundsMax.x += sfImpostorWidth.value;
    boundsMax.y += sfImpostorWidth.value;
    boundsMax.z += sfImpostorHeight.value;

    if (mFirst)
    {
        layout->mBoundsMin = boundsMin;
        layout->mBoundsMax = boundsMax;
        mFirst = false;
        return;
    }

    if (boundsMin.x < layout->mBoundsMin.x)
        layout->mBoundsMin.x = boundsMin.x;
    if (boundsMin.y < layout->mBoundsMin.y)
        layout->mBoundsMin.y = boundsMin.y;
    if (boundsMin.z < layout->mBoundsMin.z)
        layout->mBoundsMin.z = boundsMin.z;
    if (boundsMax.x > layout->mBoundsMax.x)
        layout->mBoundsMax.x = boundsMax.x;
    if (boundsMax.y > layout->mBoundsMax.y)
        layout->mBoundsMax.y = boundsMax.y;
    if (boundsMax.z > layout->mBoundsMax.z)
        layout->mBoundsMax.z = boundsMax.z;
}
