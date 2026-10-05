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

class CrowdPointCallbackBase
{
public:
    CrowdPointCallbackBase(
        CrowdLayoutObject* object)
    {
        mObject = object;
    }

    virtual void Place(nlVector4 point) = 0;

    /* 0x04 */ CrowdLayoutObject* mObject;
}; // size: 0x08

class CrowdPointCallback : public CrowdPointCallbackBase
{
public:
    CrowdPointCallback(CrowdLayoutObject* object)
        : CrowdPointCallbackBase(object)
    {
        mFirst = true;
    }

    virtual void Place(nlVector4 point);
    void UpdateBounds(const nlVector4& worldPoint);

    /* 0x08 */ bool mFirst;
    /* 0x09 */ u8 mPadding009[3];
    /* 0x0C */ CrowdLayoutRecord* mLayout;
}; // size: 0x10

CrowdImpostorManager::CrowdImpostorManager()
{
    mLayouts = 0;
    mPrimaryObjectCount = 0;
    mNumLayouts = 0;
    mNumAngles = 0;
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

    nlDLListIterator<CrowdLayoutObject*> occlusionIt;
    occlusionIt = mOcclusionObjects.Begin();
    while (occlusionIt.m_Curr != 0)
    {
        CrowdLayoutObject* object
            = occlusionIt.m_Curr->entry;
        nlInvertMatrix(*mInverseMatrices,
            *object->GetWorldMatrix());
        occlusionIt.Step();
    }

    nlDLListIterator<CrowdLayoutObject*> objectIt;
    objectIt = mPrimaryObjects.Begin();
    while (objectIt.m_Curr != 0)
    {
        CrowdLayoutObject* object = objectIt.m_Curr->entry;
        CrowdPointCallback callback(object);
        callback.mLayout = GetCrowdImpostorManager()->AllocateLayout();
        callback.mLayout->mObject = object;
        sNumGeneratedCrowdMembers += object->PlacePoints(&callback,
            sfDistanceBetweenCrowdRows,
            sfDistanceBetweenCrowdMembers);
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

int CrowdLayoutObject::PlacePoints(CrowdPointCallback* callback, float rowSpacing, float memberSpacing)
{
    nlVector4 corners[4];
    int total = 0;
    GetCorners(corners);

    int numRows = (int)floorf(mLength / rowSpacing) + 1;

    for (int row = 0; row < numRows; ++row)
    {
        float rowStep = mLength;
        rowStep = rowSpacing / rowStep;
        float occupiedLength = (numRows - 1) * rowSpacing;
        float rowOffset = 0.5f
            * (mLength - occupiedLength)
            / mLength;
        float rowAmount[1] = { row * rowStep + rowOffset };
        nlVector4 right;
        nlVector4 left;
        nlVec4Scale(left, corners[0], rowAmount[0]);
        nlVec4ScaleAdd(left, 1.0f - rowAmount[0],
            corners[3], left);
        nlVec4Scale(right, corners[1], rowAmount[0]);
        nlVec4ScaleAdd(right, 1.0f - rowAmount[0],
            corners[2], right);

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
            float memberAmount[1] = { member * memberStep + memberOffset };
            nlVector4 point;
            nlVec4Scale(point, left, memberAmount[0]);
            nlVec4ScaleAdd(point, 1.0f - memberAmount[0],
                right, point);
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

static bool IsCrowdImpostorVisible(CrowdImpostorManager* manager, Impostor* impostor)
{
    for (int filter = 0; filter < manager->mNumVisibilityFilters; ++filter)
    {
        if (!manager->mVisibilityFilters[filter]->IsVisible(impostor))
            return false;
    }
    return true;
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
                = &impostors[i + layout.mFirstImpostor];
            if (impostor->mUnidentified02C)
                continue;

            if (IsCrowdImpostorVisible(this, impostor))
                impostor->Release();
        }
        sNumVisibleCrowdMembers += layout.mNumImpostors;
    }
}

inline bool CrowdImpostorManager::IsObjectEnabled(CrowdLayoutObject* object)
{
    nlDLListIterator<CrowdLayoutObject*> objectIt;
    objectIt = mEnabledObjects.Begin();
    while (objectIt.hasNext())
    {
        if (object == *objectIt)
            return true;
        objectIt.Step();
    }
    return false;
}

inline bool CrowdImpostorManager::IsPointOccluded(
    const nlVector4& worldPoint, nlVector4& localPoint)
{
    nlDLListIterator<CrowdLayoutObject*> occlusionIt;
    occlusionIt = mOcclusionObjects.Begin();
    while (occlusionIt.hasNext())
    {
        CrowdLayoutObject* object = *occlusionIt;
        nlMultVectorMatrix(localPoint, worldPoint, mInverseMatrices[0]);
        if (object->ContainsLocalPoint((nlVector3*)&localPoint))
            return true;
        occlusionIt.Step();
    }
    return false;
}

inline ImpostorCharacter* CrowdImpostorManager::GetCharacter(int index)
{
    nlDLListIterator<ImpostorCharacter*> characterIt;
    characterIt = mCharacters.Begin();
    while (index-- > 0)
        characterIt.Step();
    return characterIt.m_Curr->entry;
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
            impostors[i + layout.mFirstImpostor].Release();
        sNumVisibleCrowdMembers += layout.mNumImpostors;
    }
}

inline u16 CrowdLayoutObject::GetFacingAngle(int numAngles)
{
    nlVector4 facing = { 0.0f, -1.0f, 0.0f, 0.0f };
    nlMultVectorMatrix(facing, *GetWorldMatrix());
    return QuantizeImpostorAngle(nlVector3ToAngle(*(nlVector3*)&facing),
        numAngles);
}

inline void CrowdPointCallback::UpdateBounds(const nlVector4& worldPoint)
{
    CrowdLayoutRecord* layout = mLayout;

    nlVector3 center = *(const nlVector3*)&worldPoint;
    nlVector3 boundsMin;
    nlVector3 boundsMax;
    boundsMin = center;
    boundsMin.x -= sfImpostorWidth.value;
    boundsMin.y -= sfImpostorWidth.value;
    boundsMax = center;
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

void CrowdPointCallback::Place(
    nlVector4 point)
{
    float horizontalJitter;
    float verticalJitter = (
        horizontalJitter = sfDistanceBetweenCrowdMembers.value
            * sfHorizontalJitterFraction.value,
        sfDistanceBetweenCrowdRows.value * sfVerticalJitterFraction.value);

    nlVector4 localPoint;
    localPoint.x = point.x
        + nlRandomf(-horizontalJitter, horizontalJitter, &nlDefaultSeed);
    localPoint.y = point.y
        + nlRandomf(-verticalJitter, verticalJitter, &nlDefaultSeed);
    localPoint.z = point.z;
    localPoint.w = 1.0f;

    nlVector4 worldPoint;
    nlMultVectorMatrix(
        worldPoint, localPoint, *mObject->GetWorldMatrix());

    nlVector4 occlusionPoint;
    if (GetCrowdImpostorManager()->IsPointOccluded(worldPoint, occlusionPoint))
        return;

    CrowdImpostorManager* manager = GetCrowdImpostorManager();
    int numCharacters = nlDLRingCountElements(manager->mCharacters.m_Head);
    int characterIndex = nlRandom(numCharacters, &nlDefaultSeed);
    ImpostorCharacter* character = manager->GetCharacter(characterIndex);

    if (GetCrowdImpostorManager()->mNumAngles == 0)
        GetCrowdImpostorManager()->mNumAngles = character->mNumAngles;

    int numAngles = GetCrowdImpostorManager()->mNumAngles;
    u16 angle = mObject->GetFacingAngle(numAngles);

    int impostorIndex = -1;
    Impostor* impostor
        = ImpostorManager::GetInstance()->AllocImpostor(&impostorIndex);
    if (impostor == 0)
        return;

    impostor->Set(character, *(nlVector3*)&worldPoint,
        sfImpostorWidth, sfImpostorHeight, angle);

    if (GetCrowdImpostorManager()->IsObjectEnabled(mObject))
        impostor->mUnidentified02C = true;

    if (mFirst)
    {
        mLayout->mFirstImpostor = impostorIndex;
        mLayout->mNumImpostors = 0;
    }
    ++mLayout->mNumImpostors;
    UpdateBounds(worldPoint);
}
