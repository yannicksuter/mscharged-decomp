#include "Game/Render/FlyingCamera.h"

#include "Game/AI/Fielder.h"
#include "Game/AsyncLoading.h"
#include "Game/Event.h"
#include "Game/EventRegistry.h"
#include "Game/MathHelpers.h"
#include "Game/ReplayManager.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/WorldTriggers.h"
#include "NL/nlMath.h"
#include "NL/nlSlotPool.h"

struct FlyingCameraPool
{
    FlyingCameraPool(void* entries)
        : mFreeList((SlotPoolEntry*)entries)
        , mEntries((FlyingCamera*)entries)
    {
        Reset();
    }

    ~FlyingCameraPool()
    {
    }

    void Reset()
    {
        for (int i = 0; i < 10 - 1; ++i)
        {
            ((SlotPoolEntry*)&mEntries[i])->next
                = (SlotPoolEntry*)(&mEntries[i] + 1);
        }
        ((SlotPoolEntry*)&mEntries[10 - 1])->next = 0;
    }

    void Free(FlyingCamera* camera)
    {
        SlotPoolEntry* entry = (SlotPoolEntry*)camera;
        entry->next = mFreeList;
        mFreeList = entry;
    }

    void Allocate(FlyingCamera*& camera)
    {
        if (mFreeList == 0)
        {
            camera = 0;
        }
        else
        {
            camera = (FlyingCamera*)mFreeList;
            mFreeList = mFreeList->next;
        }
    }

    SlotPoolEntry* mFreeList;
    FlyingCamera* mEntries;
};

FlyingCamera* gFlyingCameras[10];
nlVector3 gFlyingCameraTargetPosition;

float lbl_806DCE18 = 0.03f;
float lbl_806DCE1C = -0.88f;
float lbl_806DCE20 = 1.0f;
float lbl_806DCE24 = -1.0f;
float lbl_806DCE28 = 1.0f;
float lbl_806DCE2C = 40.0f;
float lbl_806DCE30 = 3.0f;
float lbl_806DCE34 = 10.0f;
float lbl_806DCE38 = 10.0f;
int gNextFlyingCameraFlashIndex = 100;
float gHammerGrowScale = 1.4f;
float gHammerGrowDuration = 0.15f;

unsigned int gFlyingCameraCount;
cFielder* gFlyingCameraTarget;
u16 gFlyingCameraAngle;
float gTimeUntilNextFlyingCameraFlash;
unsigned int gFlyingCameraFlashesRemaining;
UnidentifiedOwnerConnection gPeachCameraFlashConnection;
UnidentifiedOwnerConnection gResetEffectsConnection;
UnidentifiedOwnerConnection gMegaStrikeMeterEndConnection;
FlyingCamera gFlyingCameraStorage[10];
FlyingCameraPool gFlyingCameraPool(gFlyingCameraStorage);

char sPeachCameraFlashEventName[] = "PeachCameraFlash";
char sResetEffectsEventName[] = "ResetEffects";
char sMegaStrikeMeterEndEventName[] = "MegaStrikeMeterEnd";

void OnPeachCameraFlash(void*);
void OnResetFlyingCameras(void*);

void UnidentifiedRandomizeFlyingCamera(FlyingCamera* camera)
{
    camera->mPositionGain = lbl_806DCE18 + nlRandomf(0.006f, &nlDefaultSeed);
    camera->mPositionDamping = lbl_806DCE1C + nlRandomf(0.006f, &nlDefaultSeed);
    camera->mIntegralGain = lbl_806DCE20 + nlRandomf(0.2f, &nlDefaultSeed);
    camera->mOrbitRadius = 3.5f + nlRandomf(1.0f, &nlDefaultSeed);
    camera->mHeightOffset = 4.5f + nlRandomf(1.0f, &nlDefaultSeed);
}

static inline float MaxOf(float a, float b)
{
    if (a >= b)
        return a;
    return b;
}

static inline float MinOf(float a, float b)
{
    if (a <= b)
        return a;
    return b;
}

void UpdateFlyingCamera(FlyingCamera* camera, float dt)
{
    nlQuaternion facing;
    nlQuaternion targetOrientation;
    nlVector3 direction;
    nlVector3 flatDirection;
    nlVector3 targetPosition;
    nlVector3 delta;
    nlVector3 directChange;
    nlVector3 previousDelta;
    nlVector3 accumulatedChange;
    float sine;
    float cosine;

    nlVec3Sub(direction, camera->mTargetPosition, camera->mPosition);
    flatDirection = direction;
    flatDirection.z = 0.0f;

    fn_802B549C(facing, camera->mAngle);

    if (flatDirection.x * flatDirection.x
            + flatDirection.y * flatDirection.y
            + flatDirection.z * flatDirection.z
        < 0.001f)
    {
        fn_802B549C(targetOrientation, 0x4000);
    }
    else
    {
        GetRotationBetweenVectors(
            targetOrientation, flatDirection, direction);
    }

    nlMultQuat(targetOrientation, targetOrientation, facing);

    float orientationBlend = lbl_806DCE2C * dt;
    orientationBlend = orientationBlend <= 1.0f
                         ? orientationBlend
                         : 1.0f;
    nlQuatNLerp(camera->mOrientation, targetOrientation, camera->mOrientation, orientationBlend);

    nlSinCos(&sine, &cosine, camera->mAngle);

    targetPosition.x = cosine * camera->mOrbitRadius + camera->mTargetPosition.x;
    targetPosition.y = sine * camera->mOrbitRadius + camera->mTargetPosition.y;
    targetPosition.z = camera->mTargetPosition.z + camera->mHeightOffset;
    nlVec3Sub(delta, targetPosition, camera->mPosition);
    float rate = 50.0f * dt;
    nlVec3Scale(directChange, delta, camera->mPositionGain * rate);
    nlVec3Add(camera->mPositionIntegral, camera->mPositionIntegral, delta);

    float minAccumulatedChange = lbl_806DCE24;
    float maxAccumulatedChange = lbl_806DCE28;
    camera->mPositionIntegral.x = MinOf(
        MaxOf(camera->mPositionIntegral.x, minAccumulatedChange),
        maxAccumulatedChange);
    camera->mPositionIntegral.y = MinOf(
        MaxOf(camera->mPositionIntegral.y, minAccumulatedChange),
        maxAccumulatedChange);
    camera->mPositionIntegral.z = MinOf(
        MaxOf(camera->mPositionIntegral.z, minAccumulatedChange),
        maxAccumulatedChange);

    nlVec3Scale(accumulatedChange, camera->mPositionIntegral, camera->mIntegralGain * rate * 0.001f);
    nlVec3Sub(previousDelta, camera->mPreviousPosition, camera->mPosition);
    camera->mPreviousPosition = camera->mPosition;
    nlVec3ScaleAdd(camera->mPosition, camera->mPositionDamping * rate, previousDelta, camera->mPosition);
    nlVec3Add(camera->mPosition, camera->mPosition, accumulatedChange);
    nlVec3Add(camera->mPosition, camera->mPosition, directChange);

    camera->mPosition.x = MinOf(
        MaxOf(camera->mPosition.x, -30.0f), 30.0f);
    camera->mPosition.y = MinOf(
        MaxOf(camera->mPosition.y, -30.0f), 30.0f);
    camera->mPosition.z = MinOf(
        MaxOf(camera->mPosition.z, -30.0f), 30.0f);
}

void ResetFlyingCameras()
{
    SetFlyingCameraCount(0, 0, 1.0f);
}

static inline DrawableFlyingCamera* GetDrawableFlyingCamera(int index)
{
    return &ReplayManager::Instance()->mRender->_2298[index];
}

void UpdateFlyingCameras(float dt)
{
    bool shouldReset = true;

    if (gFlyingCameraTarget != 0 && gFlyingCameraTarget->m_eClassType == FIELDER
        && fn_8003877C(gFlyingCameraTarget))
    {
        gFlyingCameraTarget = 0;
    }

    nlVector3 targetPosition;
    if (gFlyingCameraTarget == 0)
    {
        nlVec3Set(targetPosition, gFlyingCameraTargetPosition.x,
            gFlyingCameraTargetPosition.y, lbl_806DCE38);
    }
    else
    {
        shouldReset = false;
        targetPosition = gFlyingCameraTarget->mUnidentified024.m_v3Position;
        gFlyingCameraTargetPosition = targetPosition;

        if (gNextFlyingCameraFlashIndex < gFlyingCameraCount)
        {
            gTimeUntilNextFlyingCameraFlash -= dt;
            if (gTimeUntilNextFlyingCameraFlash <= 0.0f)
            {
                while (gNextFlyingCameraFlashIndex < gFlyingCameraCount
                       && gFlyingCameraFlashesRemaining != 0)
                {
                    nlVector3 flashPosition = { -0.8f, 0.0f, 0.1f };
                    RotateVector(flashPosition, flashPosition,
                        gFlyingCameras[gNextFlyingCameraFlashIndex]->mOrientation);
                    nlVec3Add(flashPosition, flashPosition,
                        gFlyingCameras[gNextFlyingCameraFlashIndex]->mPosition);

                    DrawableFlyingCamera* drawableCamera = 0;
                    if (ReplayManager::Instance()->mRender != 0)
                    {
                        drawableCamera = GetDrawableFlyingCamera(
                            gNextFlyingCameraFlashIndex);
                    }
                    EmitCameraFlash(flashPosition, drawableCamera);

                    ++gNextFlyingCameraFlashIndex;
                    --gFlyingCameraFlashesRemaining;
                }

                gTimeUntilNextFlyingCameraFlash = 0.01f
                                                + nlRandomf(0.02f, &nlDefaultSeed);
                gFlyingCameraFlashesRemaining = nlRandom(1, &nlDefaultSeed) + 1;
            }
        }

        if (dt <= 0.0f)
        {
            return;
        }
    }

    float angleAdvance = 6553.6f * dt;
    gFlyingCameraAngle += (s32)(angleAdvance * lbl_806DCE30);

    for (unsigned int i = 0; i < gFlyingCameraCount; ++i)
    {
        gFlyingCameras[i]->mAngle = gFlyingCameraAngle
                               + (u16)((i * 0xFFFF) / gFlyingCameraCount);
        gFlyingCameras[i]->mTargetPosition = targetPosition;
        UpdateFlyingCamera(gFlyingCameras[i], dt);

        float resetHeightThreshold = lbl_806DCE38 - 0.5f;
        if (gFlyingCameras[i]->mPosition.z < resetHeightThreshold)
        {
            shouldReset = false;
        }
    }

    if (shouldReset)
    {
        SetFlyingCameraCount(0, 0, 1.0f);
    }
}

static inline void ResetCameraIntegral(FlyingCamera* camera)
{
    nlVec3Set(camera->mPositionIntegral, 0.0f, 0.0f, 0.0f);
}

void SetFlyingCameraCount(int count, cFielder* fielder, float orbitRadius)
{
    FlyingCamera** slot = &gFlyingCameras[count];
    for (unsigned int i = count; i < gFlyingCameraCount; ++slot, ++i)
    {
        if (*slot != 0)
        {
            gFlyingCameraPool.Free(*slot);
        }
        *slot = 0;
    }

    FlyingCamera* camera;
    unsigned int oldCount = gFlyingCameraCount;
    for (unsigned int i = oldCount; i < (unsigned int)count; ++i)
    {
        gFlyingCameraPool.Allocate(camera);

        if (camera != 0)
        {
            camera->mIndex = i;
            camera->mAngle = 0;
            camera->mVisible = true;
            UnidentifiedRandomizeFlyingCamera(camera);

            camera->mOrientation.z = 0.0f;
            camera->mOrientation.y = 0.0f;
            camera->mOrientation.x = 0.0f;
            camera->mOrientation.w = 1.0f;
            camera->mPosition.x = 0.0f;
            camera->mPosition.y = 0.0f;
            camera->mPosition.z = 1.0f;
            camera->mPreviousPosition.x = 0.0f;
            camera->mPreviousPosition.y = 0.0f;
            camera->mPreviousPosition.z = 0.0f;
            camera->mPositionIntegral.x = 0.0f;
            camera->mPositionIntegral.y = 0.0f;
            camera->mPositionIntegral.z = 0.0f;
            camera->mTargetPosition.x = 0.0f;
            camera->mTargetPosition.y = 0.0f;
            camera->mTargetPosition.z = 0.0f;
        }

        gFlyingCameras[i] = camera;
    }

    nlVector3 initialPosition = { 0.0f, 0.0f, 0.0f };
    initialPosition.z = lbl_806DCE34;

    SetFlyingCameraTarget(fielder);

    if (fielder != 0)
    {
        initialPosition.x = fielder->mUnidentified024.m_v3Position.x;
        initialPosition.y = fielder->mUnidentified024.m_v3Position.y;
    }

    for (unsigned int i = 0; i < (unsigned int)count; ++i)
    {
        gFlyingCameras[i]->mPosition = initialPosition;
        gFlyingCameras[i]->mPreviousPosition = initialPosition;
        ResetCameraIntegral(gFlyingCameras[i]);
        gFlyingCameras[i]->mOrbitRadius = orbitRadius;
    }

    gFlyingCameraCount = count;
    gNextFlyingCameraFlashIndex = 100;

    if (count != 0)
    {
        if (gPeachCameraFlashConnection.mOwner == 0)
        {
            UnidentifiedFindEvent<void>(sPeachCameraFlashEventName, -1)->Add(Function<void*>(OnPeachCameraFlash), (unsigned int)&gPeachCameraFlashConnection, -1);
        }
        if (gResetEffectsConnection.mOwner == 0)
        {
            UnidentifiedFindEvent<void>(sResetEffectsEventName, -1)->Add(Function<void*>(OnResetFlyingCameras), (unsigned int)&gResetEffectsConnection, -1);
        }
        if (gMegaStrikeMeterEndConnection.mOwner == 0)
        {
            UnidentifiedFindEvent<void>(sMegaStrikeMeterEndEventName, -1)->Add(Function<void*>(OnResetFlyingCameras), (unsigned int)&gMegaStrikeMeterEndConnection, -1);
        }
    }
}

FlyingCamera* GetFlyingCamera(int index)
{
    return gFlyingCameras[index];
}

void SetFlyingCameraTarget(cFielder* fielder)
{
    gFlyingCameraTarget = fielder;
    if (fielder != 0)
    {
        gFlyingCameraTargetPosition = fielder->mUnidentified024.m_v3Position;
    }
}

void OnPeachCameraFlash(void*)
{
    if (gFlyingCameraCount != 0)
    {
        gNextFlyingCameraFlashIndex = 0;
        gFlyingCameraFlashesRemaining = nlRandom(1, &nlDefaultSeed) + 1;
        gTimeUntilNextFlyingCameraFlash = 0.0f;
        UpdateFlyingCameras(0.0f);
    }
}

void OnResetFlyingCameras(void*)
{
    if (gFlyingCameraCount != 0)
    {
        SetFlyingCameraCount(0, 0, 1.0f);
    }
}
