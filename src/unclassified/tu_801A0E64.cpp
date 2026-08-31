#include "unclassified/tu_801A0E64.h"

#include "Game/AI/AiUtil.h"
#include "Game/AI/Fielder.h"
#include "Game/Drawable/RenderObject.h"
#include "Game/Physics/PhysicsObject.h"
#include "Game/Physics/PhysicsSphere.h"
#include "NL/nlSlotPool.h"
#include "ode/objects.h"

class UnidentifiedHammerObjectMember030
{
public:
    virtual ~UnidentifiedHammerObjectMember030();
};

extern "C"
{
    extern SlotPoolEntry* lbl_806E12A0;

    extern float lbl_806DCE40;
    extern float lbl_806DCE44;
    float lbl_806DCE48 = 25.0f;
    float lbl_806DCE4C = 5.0f;
    float lbl_806DCE50 = 105.0f;
    float lbl_806DCE54 = 165.0f;

    extern const float lbl_806E5058;
    extern const float lbl_806E505C;
    extern const float lbl_806E5060;
    extern const float lbl_806E5064;
    extern const float lbl_806E5068;
    extern const float lbl_806E506C;
    extern const float lbl_806E5070;
    extern const float lbl_806E5074;

    nlVector4 lbl_80511E28 = { 1.0f, 0.0f, 0.0f, 0.0f };

    extern PhysicsSphere_801700D8* fn_801700D8(PhysicsSphere_801700D8*);
    extern void fn_80170760(PhysicsObject*);
    extern void fn_8017076C(PhysicsObject*);
    extern RenderObject* fn_80276360(int, int);
    extern void fn_801BCA5C(const nlVector3*);
}

static const nlVector3 lbl_804DCE30 = { 0.0f, 0.0f, 0.0f };
static const nlVector3 lbl_804DCE3C = { 0.0f, -20.0f, -5.0f };

static inline void SetVelocity(HammerObject* object, const nlVector3& velocity)
{
    object->_028->SetLinearVelocity(velocity);

    dQuaternion bodyOrientation;
    nlVector3 forward;
    nlVector3 planarVelocity;
    nlVector3 angularVelocity;
    nlQuaternion orientation;

    nlVec3Set(angularVelocity,
        lbl_806E505C,
        lbl_806DCE48,
        lbl_806E505C);
    nlVec3Set(planarVelocity,
        velocity.x,
        velocity.y,
        lbl_806E505C);

    if (planarVelocity.GetLengthSq3D() > lbl_806E5060)
    {
        forward = *(nlVector3*)&lbl_80511E28;
        GetRotationBetweenVectors(orientation, forward, planarVelocity);
    }
    else
    {
        orientation.z = lbl_806E505C;
        orientation.y = lbl_806E505C;
        orientation.x = lbl_806E505C;
        orientation.w = lbl_806E5058;
    }

    bodyOrientation[0] = orientation.x;
    bodyOrientation[1] = orientation.y;
    bodyOrientation[2] = orientation.z;
    bodyOrientation[3] = orientation.w;
    dBodySetQuaternion(object->_028->m_bodyID, bodyOrientation);
    object->_028->SetAngularVelocity(angularVelocity);
    object->_000 = orientation;
}

static inline void Reset(HammerObject* object)
{
    object->_025 = false;
    object->_028->SetPosition(
        lbl_804DCE3C, PhysicsObject::WORLD_COORDINATES);
    SetVelocity(object, lbl_804DCE30);
    object->_038 = object->_028->GetLinearVelocity();
    object->_028->DisableCollisions();
    fn_80170760(object->_028);

    object->_024 = false;
    object->_018 = lbl_806E5058;
    object->_01C = lbl_806E505C;
    object->_014 = lbl_806E5058;
    object->_028->SetRadius(object->_010 * lbl_806E5058);
    object->_034 = 0;
    object->_044 = lbl_806E505C;
    object->_048 = lbl_806E505C;
    delete object->_030;
    object->_030 = 0;
}

extern "C" HammerObject* fn_801A0E64(
    HammerObject* object, int index, float radius)
{
    object->_010 = radius;
    object->_014 = lbl_806E5058;
    object->_018 = lbl_806E5058;
    object->_01C = lbl_806E505C;
    object->_020 = index;
    object->_024 = false;
    object->_025 = false;
    object->_034 = 0;
    object->_044 = lbl_806E505C;
    object->_048 = lbl_806E505C;

    PhysicsSphere_801700D8* physics;
    if (lbl_806E12A0 == 0)
    {
        physics = 0;
    }
    else
    {
        physics = (PhysicsSphere_801700D8*)lbl_806E12A0;
        lbl_806E12A0 = lbl_806E12A0->next;
    }
    if (physics != 0)
    {
        physics = fn_801700D8(physics);
    }
    object->_028 = physics;
    physics->_038 = object;
    object->_028->SetPosition(
        lbl_804DCE3C, PhysicsObject::WORLD_COORDINATES);
    object->_02C = fn_80276360(2, index);
    object->_030 = 0;
    Reset(object);
    return object;
}

extern "C" HammerObject* fn_801A10D0(
    HammerObject* object, int shouldDelete)
{
    if (object != 0)
    {
        if (object->_030 != 0)
        {
            delete object->_030;
        }
        if (object->_028 != 0)
        {
            delete object->_028;
        }
        if (shouldDelete > 0)
        {
            ::operator delete(object);
        }
    }
    return object;
}

extern "C" const nlVector3* fn_801A1168(const HammerObject* object)
{
    return &((HammerObject*)object)->_028->GetPosition();
}

extern "C" void fn_801A1170(
    HammerObject* object, const nlVector3* position)
{
    object->_028->SetPosition(
        *position, PhysicsObject::WORLD_COORDINATES);
}

extern "C" void fn_801A117C(
    HammerObject* object, const nlVector3* velocity)
{
    SetVelocity(object, *velocity);
}

extern "C" const nlQuaternion* fn_801A1298(HammerObject* object)
{
    if (object->_048 <= lbl_806E505C)
    {
        const float* orientation = dBodyGetQuaternion(object->_028->m_bodyID);
        nlVec4Set(*(nlVector4*)&object->_000, orientation[0], orientation[1], orientation[2], orientation[3]);
    }
    return &object->_000;
}

extern "C" void fn_801A1650(HammerObject* object, float time)
{
    object->_044 = time;
    object->_038 = object->_028->GetLinearVelocity();
    object->_028->DisableCollisions();
}

extern "C" void fn_801A16A4(HammerObject* object, float dt)
{
    if (!object->_024)
    {
        return;
    }

    if (object->_025)
    {
        Reset(object);
        return;
    }

    if (object->_044 > lbl_806E505C)
    {
        object->_044 -= dt;
        if (object->_044 <= lbl_806E505C)
        {
            object->_028->EnableCollisions();
            fn_80170760(object->_028);
            object->_028->SetLinearVelocity(object->_038);
        }
        else
        {
            fn_8017076C(object->_028);
            return;
        }
    }

    if (object->_048 > lbl_806E505C)
    {
        object->_048 -= dt;
        if (object->_048 <= lbl_806E505C)
        {
            fn_801BCA5C(&object->_028->GetPosition());
            Reset(object);
            return;
        }
        fn_8017076C(object->_028);
    }

    if (object->_01C > lbl_806E505C)
    {
        object->_01C -= dt;
        if (object->_01C <= lbl_806E505C)
        {
            object->_014 = object->_018;
            object->_01C = lbl_806E505C;
            object->_028->SetRadius(object->_010 * object->_018);
        }
        else
        {
            float fraction = dt / object->_01C;
            if (fraction > lbl_806E5058)
            {
                fraction = lbl_806E5058;
            }
            object->_014 = Interpolate(
                object->_014, object->_018, fraction);
            object->_028->SetRadius(object->_010 * object->_014);
        }
    }
}

extern "C" void fn_801A1B54(HammerObject* object, cFielder* fielder)
{
    object->_034 = fielder;
    object->_028->SetPosition(
        fielder->m_v3Position, PhysicsObject::WORLD_COORDINATES);
    object->_025 = false;
    object->_024 = true;
    object->_028->EnableCollisions();

    object->_018 = lbl_806E5058;
    object->_01C = lbl_806E505C;
    object->_014 = lbl_806E5058;
    object->_028->SetRadius(object->_010 * lbl_806E5058);

    float targetScale = lbl_806DCE40;
    float duration = lbl_806DCE44;
    object->_018 = targetScale;
    object->_01C = duration;
    if (duration <= lbl_806E505C)
    {
        object->_014 = object->_018;
        object->_028->SetRadius(object->_010 * object->_018);
    }

    nlVector3 velocity = { 0.0f, 0.0f, 0.0f };
    SetVelocity(object, velocity);
}

extern "C" void fn_801A1CFC(HammerObject* object, int)
{
    Reset(object);
}

extern "C" void fn_801A1ED0(HammerObject* object, bool emitEffect)
{
    if (object->_024)
    {
        object->_025 = true;
        if (emitEffect)
        {
            fn_801BCA5C(&object->_028->GetPosition());
        }
    }
}
