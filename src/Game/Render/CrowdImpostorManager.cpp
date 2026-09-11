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

    virtual void Place(const nlVector4& point);

    /* 0x04 */ CrowdLayoutObject* mObject;
    /* 0x08 */ bool mFirst;
    /* 0x09 */ u8 mPadding009[3];
    /* 0x0C */ CrowdLayoutRecord* mLayout;
}; // size: 0x10

extern "C" void fn_802D88F4(
    CrowdLayoutObject* object)
{
    GetCrowdImpostorManager()->AddObject( object, false);
}

extern "C" void fn_802D892C(CrowdLayoutObject*)
{
}

void CrowdLayoutObject::Initialize()
{
    mIsOcclusionVolume = 0;
    mStartWidth = 0.0f;
    mEndWidth = 0.0f;
    mLength = 0.0f;
    mEndOffset = 0.0f;
}

void CrowdLayoutObject::GetCorners(nlVector4* corners)
{
    nlVec4Set(corners[1], 0.0f, 0.0f, 0.0f, 1.0f);
    nlVec4Set(corners[0], mStartWidth, 0.0f, 0.0f, 1.0f);

    float offset = 0.5f
        * (mStartWidth - mEndWidth);
    nlVec4Set(corners[2], mEndOffset + offset,
        mLength, 0.0f, 1.0f);
    nlVec4Set(corners[3], mEndWidth
            + mEndOffset + offset,
        mLength, 0.0f, 1.0f);
}

void GetCrowdLayoutBounds(
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

bool CrowdLayoutObject::ContainsLocalPoint(const nlVector3* point)
{
    nlVector4 corners[4];
    nlVector4 boundsMin;
    nlVector4 boundsMax;
    GetCorners(corners);
    GetCrowdLayoutBounds(corners, &boundsMin, &boundsMax);

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
    mPrimaryObjects.Clear();
    mPrimaryObjects.m_Allocator.FreeBlocks();
    mPrimaryObjectCount = 0;

    mOcclusionObjects.Clear();
    mOcclusionObjects.m_Allocator.FreeBlocks();
    mOcclusionObjectCount = 0;

    mEnabledObjects.Clear();
    mEnabledObjects.m_Allocator.FreeBlocks();
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

static inline void InterpolateCrowdPoint(nlVector4& result,
    const nlVector4& first, const nlVector4& second, float amount)
{
    float inverse = 1.0f - amount;
    result.x = inverse * first.x + amount * second.x;
    result.y = inverse * first.y + amount * second.y;
    result.z = inverse * first.z + amount * second.z;
    result.w = inverse * first.w + amount * second.w;
}

int CrowdLayoutObject::PlacePoints(CrowdPointCallback* callback, float rowSpacing, float memberSpacing)
{
    nlVector4 corners[4];
    GetCorners(corners);

    int total = 0;
    int numRows = (int)floor(mLength / rowSpacing) + 1;
    float rowOffset = 0.5f
        * (mLength - (numRows - 1) * rowSpacing)
        / mLength;

    for (int row = 0; row < numRows; ++row)
    {
        float rowAmount
            = row * rowSpacing / mLength + rowOffset;
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

void CrowdImpostorManager::ReleaseCrowdImpostors()
{
    Impostor* impostors = ImpostorManager::GetInstance()->mImpostors;
    sNumVisibleCrowdMembers = 0;

    for (int layoutIndex = 0; layoutIndex < mNumLayouts;
        ++layoutIndex)
    {
        CrowdLayoutRecord& layout
            = mLayouts[layoutIndex];
        bool found = false;
        nlDLListIterator<CrowdLayoutObject*> objectIt
            = mEnabledObjects.Begin();
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

void CrowdPointCallback::Place(
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

    CrowdImpostorManager* manager = GetCrowdImpostorManager();
    nlDLListIterator<CrowdLayoutObject*> occlusionIt
        = manager->mOcclusionObjects.Begin();
    bool occluded = false;
    while (occlusionIt.m_Curr != 0)
    {
        CrowdLayoutObject* object = occlusionIt.m_Curr->entry;
        nlVector4 occlusionPoint;
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
    nlVector4 facing = { 0.0f, -1.0f, 0.0f, 0.0f };
    nlMultVectorMatrix(facing, *mObject->UnidentifiedVirtual10());
    u16 angle = QuantizeImpostorAngle(nlVector3ToAngle(*(nlVector3*)&facing),
        numAngles);

    int impostorIndex = -1;
    Impostor* impostor
        = ImpostorManager::GetInstance()->AllocImpostor(&impostorIndex);
    if (impostor == 0)
        return;

    impostor->Set(character, *(nlVector3*)&worldPoint, angle,
        sfImpostorWidth.value, sfImpostorHeight.value);

    nlDLListIterator<CrowdLayoutObject*> enabledIt
        = GetCrowdImpostorManager()->mEnabledObjects.Begin();
    bool enabled = false;
    while (enabledIt.m_Curr != 0)
    {
        if (mObject == enabledIt.m_Curr->entry)
        {
            enabled = true;
            break;
        }
        enabledIt.Step();
    }
    if (enabled)
        impostor->mUnidentified02C = true;

    if (mFirst)
    {
        mLayout->mFirstImpostor = impostorIndex;
        mLayout->mNumImpostors = 0;
    }
    ++mLayout->mNumImpostors;
    CrowdLayoutRecord* layout = mLayout;

    nlVector3 boundsMin = *(nlVector3*)&worldPoint;
    boundsMin.x -= sfImpostorWidth.value;
    boundsMin.y -= sfImpostorWidth.value;
    nlVector3 boundsMax = *(nlVector3*)&worldPoint;
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
