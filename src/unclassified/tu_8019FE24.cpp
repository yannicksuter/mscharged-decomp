#include "unclassified/tu_8019FE24.h"

#include "Game/AI/Fielder.h"
#include "Game/Event.h"
#include "Game/MathHelpers.h"
#include "Game/ReplayManager.h"
#include "Game/WorldTriggers.h"
#include "NL/nlAVLTree.h"
#include "NL/nlSlotPool.h"

typedef nlAVLTree<unsigned int, UnidentifiedEventBase*,
    DefaultKeyCompare<unsigned int> >
    UnidentifiedEventRegistry;

struct UnidentifiedFlyingCameraPool
{
    SlotPoolEntry* mFreeList;
    Object_8019FE24* mEntries;

    void Free(Object_8019FE24* object)
    {
        SlotPoolEntry* entry = (SlotPoolEntry*)object;
        entry->next = mFreeList;
        mFreeList = entry;
    }

    void Allocate(Object_8019FE24*& object)
    {
        if (mFreeList == 0)
        {
            object = 0;
        }
        else
        {
            object = (Object_8019FE24*)mFreeList;
            mFreeList = mFreeList->next;
        }
    }
};

extern "C"
{
    Object_8019FE24* lbl_80571C08[10];
    nlVector3 lbl_80571C30;

    float lbl_806DCE18 = 0.03f;
    float lbl_806DCE1C = -0.88f;
    float lbl_806DCE20 = 1.0f;
    float lbl_806DCE24 = -1.0f;
    float lbl_806DCE28 = 1.0f;
    float lbl_806DCE2C = 40.0f;
    float lbl_806DCE30 = 3.0f;
    float lbl_806DCE34 = 10.0f;
    float lbl_806DCE38 = 10.0f;
    int lbl_806DCE3C = 100;
    float lbl_806DCE40 = 1.4f;
    float lbl_806DCE44 = 0.15f;

    unsigned int lbl_806E1538;
    cFielder* lbl_806E153C;
    u16 lbl_806E1540;
    float lbl_806E1544;
    unsigned int lbl_806E1548;

    extern void* lbl_806E154C;
    extern void* lbl_806E1550;
    extern void* lbl_806E1554;
    extern UnidentifiedFlyingCameraPool lbl_806E1558;
    extern UnidentifiedEventRegistry* lbl_806E1D90;

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

    char lbl_80511DF0[] = "PeachCameraFlash";
    char lbl_80511E04[] = "ResetEffects";
    char lbl_80511E14[] = "MegaStrikeMeterEnd";

    bool fn_8003877C(cFielder* fielder);
    void fn_802B549C(nlQuaternion& quaternion, u16 angle);
    void fn_801A0C84(void*);
    void fn_801A0CD4(void*);
}

static inline void UnidentifiedRegisterEventCallback(
    const char* name, void (*callback)(void*), void** connection)
{
    Function<void*> function(callback);
    unsigned int hash = fn_802B289C(name, -1);
    UnidentifiedEventBase** foundEvent = 0;
    lbl_806E1D90->Find(hash, &foundEvent, 0);
    UnidentifiedEventBase* event = foundEvent != 0 ? *foundEvent : 0;
    ((UnidentifiedTypedEvent<void>*)event)
        ->Add(function, (unsigned int)connection, -1);
}

void fn_8019FE24(Object_8019FE24* object, float dt)
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

    nlVec3Sub(direction, object->_034, object->position);
    flatDirection = direction;
    flatDirection.z = lbl_806E5034;

    fn_802B549C(facing, object->angle);

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
    nlQuatNLerp(object->orientation, targetOrientation, object->orientation, orientationBlend);

    nlSinCos(&sine, &cosine, object->angle);

    targetPosition.x = cosine * object->_04C + object->_034.x;
    targetPosition.y = sine * object->_04C + object->_034.y;
    targetPosition.z = object->_034.z + object->_050;
    nlVec3Sub(delta, targetPosition, object->position);
    float rate = lbl_806E503C * dt;
    float directScale = object->_040 * rate;
    nlVec3Add(object->_028, object->_028, delta);

    nlVec3Scale(directChange, delta, directScale);

    float minAccumulatedChange = lbl_806DCE24;
    float maxAccumulatedChange = lbl_806DCE28;
    object->_028.x = nlMinEquals(
        nlMaxEquals(object->_028.x, minAccumulatedChange),
        maxAccumulatedChange);
    object->_028.y = nlMinEquals(
        nlMaxEquals(object->_028.y, minAccumulatedChange),
        maxAccumulatedChange);
    object->_028.z = nlMinEquals(
        nlMaxEquals(object->_028.z, minAccumulatedChange),
        maxAccumulatedChange);

    float previousBlend = object->_044 * rate;
    float accumulatedScale = object->_048 * rate * lbl_806E5038;
    nlVec3Sub(previousDelta, object->previousPosition, object->position);
    nlVec3Scale(accumulatedChange, object->_028, accumulatedScale);
    object->previousPosition = object->position;
    nlVec3ScaleAdd(
        object->position, previousBlend, previousDelta, object->position);
    nlVec3Add(object->position, object->position, accumulatedChange);
    nlVec3Add(object->position, object->position, directChange);

    object->position.x = nlMinEquals(
        nlMaxEquals(object->position.x, lbl_806E5040), lbl_806E5044);
    object->position.y = nlMinEquals(
        nlMaxEquals(object->position.y, lbl_806E5040), lbl_806E5044);
    object->position.z = nlMinEquals(
        nlMaxEquals(object->position.z, lbl_806E5040), lbl_806E5044);
}

void fn_801A01F8()
{
    fn_801A0500(0, 0, lbl_806E502C);
}

void fn_801A0208(float dt)
{
    bool bUnidentified = true;

    if (lbl_806E153C != 0 && lbl_806E153C->m_eClassType == FIELDER
        && fn_8003877C(lbl_806E153C))
    {
        lbl_806E153C = 0;
    }

    nlVector3 v3Unidentified;
    if (lbl_806E153C == 0)
    {
        nlVec3Set(v3Unidentified, lbl_80571C30.x, lbl_80571C30.y, lbl_806DCE38);
    }
    else
    {
        bUnidentified = false;
        v3Unidentified = lbl_806E153C->m_v3Position;
        lbl_80571C30 = v3Unidentified;

        if (lbl_806DCE3C < lbl_806E1538)
        {
            lbl_806E1544 -= dt;
            if (lbl_806E1544 <= lbl_806E5034)
            {
                while (lbl_806DCE3C < lbl_806E1538
                       && lbl_806E1548 != 0)
                {
                    nlVector3 v3Unidentified2 = { -0.8f, 0.0f, 0.1f };
                    RotateVector(v3Unidentified2, v3Unidentified2, lbl_80571C08[lbl_806DCE3C]->orientation);
                    nlVec3Add(v3Unidentified2, v3Unidentified2, lbl_80571C08[lbl_806DCE3C]->position);

                    DrawableFlyingCamera* pUnidentified = 0;
                    if (ReplayManager::Instance()->mRender != 0)
                    {
                        int nUnidentified = lbl_806DCE3C;
                        ReplayManager* pReplayManager
                            = ReplayManager::Instance();
                        pUnidentified
                            = &pReplayManager->mRender->_2298[nUnidentified];
                    }
                    EmitCameraFlash(v3Unidentified2, pUnidentified);

                    ++lbl_806DCE3C;
                    --lbl_806E1548;
                }

                lbl_806E1544 = lbl_806E5048
                             + nlRandomf(lbl_806E504C, &nlDefaultSeed);
                lbl_806E1548 = nlRandom(1, &nlDefaultSeed) + 1;
            }
        }

        if (dt <= lbl_806E5034)
        {
            return;
        }
    }

    float fUnidentified = lbl_806E5050 * dt;
    lbl_806E1540 += (s32)(fUnidentified * lbl_806DCE30);

    for (unsigned int i = 0; i < lbl_806E1538; ++i)
    {
        lbl_80571C08[i]->angle = lbl_806E1540
                               + (u16)((i * 0xFFFF) / lbl_806E1538);
        lbl_80571C08[i]->_034 = v3Unidentified;
        fn_8019FE24(lbl_80571C08[i], dt);

        float fUnidentified2 = lbl_806DCE38 - lbl_806E5054;
        if (lbl_80571C08[i]->position.z < fUnidentified2)
        {
            bUnidentified = false;
        }
    }

    if (bUnidentified)
    {
        fn_801A0500(0, 0, lbl_806E502C);
    }
}

void fn_801A0500(int count, cFielder* fielder, float duration)
{
    Object_8019FE24** slot = &lbl_80571C08[count];
    for (unsigned int i = count; i < lbl_806E1538; ++slot, ++i)
    {
        if (*slot != 0)
        {
            lbl_806E1558.Free(*slot);
        }
        *slot = 0;
    }

    Object_8019FE24* object;
    unsigned int oldCount = lbl_806E1538;
    for (unsigned int i = oldCount; i < (unsigned int)count; ++i)
    {
        lbl_806E1558.Allocate(object);

        if (object != 0)
        {
            object->index = i;
            object->angle = 0;
            object->visible = true;
            object->_040 = lbl_806DCE18
                         + nlRandomf(lbl_806E5020, &nlDefaultSeed);
            object->_044 = lbl_806DCE1C
                         + nlRandomf(lbl_806E5020, &nlDefaultSeed);
            object->_048 = lbl_806DCE20
                         + nlRandomf(lbl_806E5024, &nlDefaultSeed);
            object->_04C = lbl_806E5028
                         + nlRandomf(lbl_806E502C, &nlDefaultSeed);
            object->_050 = lbl_806E5030
                         + nlRandomf(lbl_806E502C, &nlDefaultSeed);

            object->orientation.z = lbl_806E5034;
            object->orientation.y = lbl_806E5034;
            object->orientation.x = lbl_806E5034;
            object->orientation.w = lbl_806E502C;
            object->position.x = lbl_806E5034;
            object->position.y = lbl_806E5034;
            object->position.z = lbl_806E502C;
            object->previousPosition.x = lbl_806E5034;
            object->previousPosition.y = lbl_806E5034;
            object->previousPosition.z = lbl_806E5034;
            object->_028.x = lbl_806E5034;
            object->_028.y = lbl_806E5034;
            object->_028.z = lbl_806E5034;
            object->_034.x = lbl_806E5034;
            object->_034.y = lbl_806E5034;
            object->_034.z = lbl_806E5034;
        }

        lbl_80571C08[i] = object;
    }

    nlVector3 v3Unidentified = { 0.0f, 0.0f, 0.0f };
    v3Unidentified.z = lbl_806DCE34;

    lbl_806E153C = fielder;
    if (fielder != 0)
    {
        lbl_80571C30 = fielder->m_v3Position;
    }

    if (fielder != 0)
    {
        v3Unidentified.x = fielder->m_v3Position.x;
        v3Unidentified.y = fielder->m_v3Position.y;
    }

    for (unsigned int i = 0; i < (unsigned int)count; ++i)
    {
        lbl_80571C08[i]->position = v3Unidentified;
        lbl_80571C08[i]->previousPosition = v3Unidentified;
        nlVec3Set(lbl_80571C08[i]->_028, lbl_806E5034, lbl_806E5034, lbl_806E5034);
        lbl_80571C08[i]->_04C = duration;
    }

    lbl_806E1538 = count;
    lbl_806DCE3C = 100;

    if (count != 0)
    {
        if (lbl_806E154C == 0)
        {
            UnidentifiedRegisterEventCallback(
                lbl_80511DF0, fn_801A0C84, &lbl_806E154C);
        }
        if (lbl_806E1550 == 0)
        {
            UnidentifiedRegisterEventCallback(
                lbl_80511E04, fn_801A0CD4, &lbl_806E1550);
        }
        if (lbl_806E1554 == 0)
        {
            UnidentifiedRegisterEventCallback(
                lbl_80511E14, fn_801A0CD4, &lbl_806E1554);
        }
    }
}

Object_8019FE24* fn_801A0C44(int index)
{
    return lbl_80571C08[index];
}

void fn_801A0C58(cFielder* fielder)
{
    lbl_806E153C = fielder;
    if (fielder != 0)
    {
        lbl_80571C30 = fielder->m_v3Position;
    }
}

extern "C" void fn_801A0C84(void*)
{
    if (lbl_806E1538 != 0)
    {
        lbl_806DCE3C = 0;
        lbl_806E1548 = nlRandom(1, &nlDefaultSeed) + 1;
        lbl_806E1544 = lbl_806E5034;
        fn_801A0208(lbl_806E5034);
    }
}

extern "C" void fn_801A0CD4(void*)
{
    if (lbl_806E1538 != 0)
    {
        fn_801A0500(0, 0, lbl_806E502C);
    }
}
