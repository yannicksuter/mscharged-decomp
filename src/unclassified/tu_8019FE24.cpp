#include "unclassified/tu_8019FE24.h"

#include "Game/AI/Fielder.h"
#include "Game/MathHelpers.h"

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

    int lbl_806E1538;
    cFielder* lbl_806E153C;
    u16 lbl_806E1540;
    float lbl_806E1544;
    int lbl_806E1548;

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

    extern const nlVector3 lbl_804DCE20 = { -0.8f, 0.0f, 0.1f };

    void fn_802B549C(nlQuaternion& quaternion, u16 angle);
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
    nlQuatNLerp(object->orientation, targetOrientation,
        object->orientation, orientationBlend);

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

extern "C" void fn_801A0C84()
{
    if (lbl_806E1538 != 0)
    {
        lbl_806DCE3C = 0;
        lbl_806E1548 = nlRandom(1, &nlDefaultSeed) + 1;
        lbl_806E1544 = lbl_806E5034;
        fn_801A0208(lbl_806E5034);
    }
}

extern "C" void fn_801A0CD4()
{
    if (lbl_806E1538 != 0)
    {
        fn_801A0500(0, 0, lbl_806E502C);
    }
}
