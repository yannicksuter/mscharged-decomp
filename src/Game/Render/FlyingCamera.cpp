#include "Game/Render/FlyingCamera.h"

#include "Game/AI/Fielder.h"
#include "Game/Event.h"
#include "Game/EventRegistry.h"
#include "Game/MathHelpers.h"
#include "Game/ReplayManager.h"
#include "Game/WorldTriggers.h"
#include "NL/nlMath.h"
#include "NL/nlSlotPool.h"

struct FlyingCameraPool
{
    SlotPoolEntry* mFreeList;
    FlyingCamera* mEntries;

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

extern void* gPeachCameraFlashConnection;
extern void* gResetEffectsConnection;
extern void* gMegaStrikeMeterEndConnection;
extern FlyingCameraPool gFlyingCameraPool;

extern const float lbl_806E5020;
extern const float lbl_806E5024;
extern const float lbl_806E5028;
extern const float lbl_806E502C;
extern const float lbl_806E5030;
extern const float lbl_806E5034;
extern const float lbl_806E5038;
extern const float lbl_806E503C;
extern const float lbl_806E5040;
extern const float lbl_806E5044;
extern const float lbl_806E5048;
extern const float lbl_806E504C;
extern const float lbl_806E5050;
extern const float lbl_806E5054;

char sPeachCameraFlashEventName[] = "PeachCameraFlash";
char sResetEffectsEventName[] = "ResetEffects";
char sMegaStrikeMeterEndEventName[] = "MegaStrikeMeterEnd";

extern "C" bool fn_8003877C(cFielder* fielder);
void OnPeachCameraFlash(void*);
void OnResetFlyingCameras(void*);

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
    flatDirection.z = lbl_806E5034;

    fn_802B549C(facing, camera->mAngle);

    if (flatDirection.x * flatDirection.x
            + flatDirection.y * flatDirection.y
            + flatDirection.z * flatDirection.z
        < lbl_806E5038)
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
    orientationBlend = orientationBlend <= lbl_806E502C
                         ? orientationBlend
                         : lbl_806E502C;
    nlQuatNLerp(camera->mOrientation, targetOrientation, camera->mOrientation, orientationBlend);

    nlSinCos(&sine, &cosine, camera->mAngle);

    targetPosition.x = cosine * camera->mOrbitRadius + camera->mTargetPosition.x;
    targetPosition.y = sine * camera->mOrbitRadius + camera->mTargetPosition.y;
    targetPosition.z = camera->mTargetPosition.z + camera->mHeightOffset;
    nlVec3Sub(delta, targetPosition, camera->mPosition);
    float rate = lbl_806E503C * dt;
    float directScale = camera->mPositionGain * rate;
    nlVec3Add(camera->mPositionIntegral, camera->mPositionIntegral, delta);

    nlVec3Scale(directChange, delta, directScale);

    float minAccumulatedChange = lbl_806DCE24;
    float maxAccumulatedChange = lbl_806DCE28;
    camera->mPositionIntegral.x = nlMinEquals(
        nlMaxEquals(camera->mPositionIntegral.x, minAccumulatedChange),
        maxAccumulatedChange);
    camera->mPositionIntegral.y = nlMinEquals(
        nlMaxEquals(camera->mPositionIntegral.y, minAccumulatedChange),
        maxAccumulatedChange);
    camera->mPositionIntegral.z = nlMinEquals(
        nlMaxEquals(camera->mPositionIntegral.z, minAccumulatedChange),
        maxAccumulatedChange);

    float previousBlend = camera->mPositionDamping * rate;
    float accumulatedScale = camera->mIntegralGain * rate * lbl_806E5038;
    nlVec3Sub(previousDelta, camera->mPreviousPosition, camera->mPosition);
    nlVec3Scale(accumulatedChange, camera->mPositionIntegral, accumulatedScale);
    camera->mPreviousPosition = camera->mPosition;
    nlVec3ScaleAdd(
        camera->mPosition, previousBlend, previousDelta, camera->mPosition);
    nlVec3Add(camera->mPosition, camera->mPosition, accumulatedChange);
    nlVec3Add(camera->mPosition, camera->mPosition, directChange);

    camera->mPosition.x = nlMinEquals(
        nlMaxEquals(camera->mPosition.x, lbl_806E5040), lbl_806E5044);
    camera->mPosition.y = nlMinEquals(
        nlMaxEquals(camera->mPosition.y, lbl_806E5040), lbl_806E5044);
    camera->mPosition.z = nlMinEquals(
        nlMaxEquals(camera->mPosition.z, lbl_806E5040), lbl_806E5044);
}

void ResetFlyingCameras()
{
    SetFlyingCameraCount(0, 0, lbl_806E502C);
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
            if (gTimeUntilNextFlyingCameraFlash <= lbl_806E5034)
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
                        int cameraIndex = gNextFlyingCameraFlashIndex;
                        ReplayManager* pReplayManager
                            = ReplayManager::Instance();
                        drawableCamera
                            = &pReplayManager->mRender->_2298[cameraIndex];
                    }
                    EmitCameraFlash(flashPosition, drawableCamera);

                    ++gNextFlyingCameraFlashIndex;
                    --gFlyingCameraFlashesRemaining;
                }

                gTimeUntilNextFlyingCameraFlash = lbl_806E5048
                             + nlRandomf(lbl_806E504C, &nlDefaultSeed);
                gFlyingCameraFlashesRemaining = nlRandom(1, &nlDefaultSeed) + 1;
            }
        }

        if (dt <= lbl_806E5034)
        {
            return;
        }
    }

    float angleAdvance = lbl_806E5050 * dt;
    gFlyingCameraAngle += (s32)(angleAdvance * lbl_806DCE30);

    for (unsigned int i = 0; i < gFlyingCameraCount; ++i)
    {
        gFlyingCameras[i]->mAngle = gFlyingCameraAngle
                               + (u16)((i * 0xFFFF) / gFlyingCameraCount);
        gFlyingCameras[i]->mTargetPosition = targetPosition;
        UpdateFlyingCamera(gFlyingCameras[i], dt);

        float resetHeightThreshold = lbl_806DCE38 - lbl_806E5054;
        if (gFlyingCameras[i]->mPosition.z < resetHeightThreshold)
        {
            shouldReset = false;
        }
    }

    if (shouldReset)
    {
        SetFlyingCameraCount(0, 0, lbl_806E502C);
    }
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
            camera->mPositionGain = lbl_806DCE18
                         + nlRandomf(lbl_806E5020, &nlDefaultSeed);
            camera->mPositionDamping = lbl_806DCE1C
                         + nlRandomf(lbl_806E5020, &nlDefaultSeed);
            camera->mIntegralGain = lbl_806DCE20
                         + nlRandomf(lbl_806E5024, &nlDefaultSeed);
            camera->mOrbitRadius = lbl_806E5028
                         + nlRandomf(lbl_806E502C, &nlDefaultSeed);
            camera->mHeightOffset = lbl_806E5030
                         + nlRandomf(lbl_806E502C, &nlDefaultSeed);

            camera->mOrientation.z = lbl_806E5034;
            camera->mOrientation.y = lbl_806E5034;
            camera->mOrientation.x = lbl_806E5034;
            camera->mOrientation.w = lbl_806E502C;
            camera->mPosition.x = lbl_806E5034;
            camera->mPosition.y = lbl_806E5034;
            camera->mPosition.z = lbl_806E502C;
            camera->mPreviousPosition.x = lbl_806E5034;
            camera->mPreviousPosition.y = lbl_806E5034;
            camera->mPreviousPosition.z = lbl_806E5034;
            camera->mPositionIntegral.x = lbl_806E5034;
            camera->mPositionIntegral.y = lbl_806E5034;
            camera->mPositionIntegral.z = lbl_806E5034;
            camera->mTargetPosition.x = lbl_806E5034;
            camera->mTargetPosition.y = lbl_806E5034;
            camera->mTargetPosition.z = lbl_806E5034;
        }

        gFlyingCameras[i] = camera;
    }

    nlVector3 initialPosition = { 0.0f, 0.0f, 0.0f };
    initialPosition.z = lbl_806DCE34;

    gFlyingCameraTarget = fielder;
    if (fielder != 0)
    {
        gFlyingCameraTargetPosition = fielder->mUnidentified024.m_v3Position;
    }

    if (fielder != 0)
    {
        initialPosition.x = fielder->mUnidentified024.m_v3Position.x;
        initialPosition.y = fielder->mUnidentified024.m_v3Position.y;
    }

    for (unsigned int i = 0; i < (unsigned int)count; ++i)
    {
        gFlyingCameras[i]->mPosition = initialPosition;
        gFlyingCameras[i]->mPreviousPosition = initialPosition;
        nlVec3Set(gFlyingCameras[i]->mPositionIntegral,
            lbl_806E5034, lbl_806E5034, lbl_806E5034);
        gFlyingCameras[i]->mOrbitRadius = orbitRadius;
    }

    gFlyingCameraCount = count;
    gNextFlyingCameraFlashIndex = 100;

    if (count != 0)
    {
        if (gPeachCameraFlashConnection == 0)
        {
            UnidentifiedFindEvent<void>(sPeachCameraFlashEventName, -1)->Add(Function<void*>(OnPeachCameraFlash), (unsigned int)&gPeachCameraFlashConnection, -1);
        }
        if (gResetEffectsConnection == 0)
        {
            UnidentifiedFindEvent<void>(sResetEffectsEventName, -1)->Add(Function<void*>(OnResetFlyingCameras), (unsigned int)&gResetEffectsConnection, -1);
        }
        if (gMegaStrikeMeterEndConnection == 0)
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
        gTimeUntilNextFlyingCameraFlash = lbl_806E5034;
        UpdateFlyingCameras(lbl_806E5034);
    }
}

void OnResetFlyingCameras(void*)
{
    if (gFlyingCameraCount != 0)
    {
        SetFlyingCameraCount(0, 0, lbl_806E502C);
    }
}
