#include "Game/AI/AiUtil.h"
#include "Game/AI/Fielder.h"
#include "Game/BulletBill.h"
#include "Game/Physics/PhysicsBulletBill.h"
#include "Game/Physics/PhysicsObject.h"

#include "NL/nlMemory.h"
#include "types.h"

extern "C" void fn_8019ABB8(BulletBillObject*, bool);
extern "C" void fn_802B549C(BulletBillObject*, u16);

extern const float lbl_806E4EE0;
extern const float lbl_806E4EE4;

extern "C" BulletBillObject* fn_8019A710(BulletBillObject* state,
    RenderObject* drawable, u32 value, float radius, float parameter)
{
    state->unknown_28 = value;
    state->drawable = drawable;
    state->scale = lbl_806E4EE0;
    state->unknown_30 = lbl_806E4EE0;
    state->unknown_34 = lbl_806E4EE4;
    state->active = false;
    state->target = 0;
    state->position.x = lbl_806E4EE4;
    state->position.y = lbl_806E4EE4;
    state->position.z = lbl_806E4EE4;
    state->velocity.x = lbl_806E4EE4;
    state->velocity.y = lbl_806E4EE4;
    state->velocity.z = lbl_806E4EE4;
    state->orientation.z = lbl_806E4EE4;
    state->orientation.y = lbl_806E4EE4;
    state->orientation.x = lbl_806E4EE4;
    state->orientation.w = lbl_806E4EE0;

    PhysicsObject* physics = new (8, false)
        PhysicsBulletBill(state, radius, parameter);
    state->physics = physics;
    physics->DisableCollisions();
    return state;
}

extern "C" BulletBillObject* fn_8019A7E4(
    BulletBillObject* state, int shouldDelete)
{
    if (state != 0)
    {
        delete state->physics;
        if (shouldDelete > 0)
        {
            ::operator delete(state);
        }
    }
    return state;
}

extern "C" void fn_8019A854(BulletBillObject* state, float deltaTime)
{
    if (!state->active)
    {
        return;
    }

    const nlVector3& currentPosition = state->position;
    const nlVector3& currentVelocity = state->velocity;

    nlVector3 position;
    position.z = currentPosition.z + deltaTime * currentVelocity.z;
    position.y = currentPosition.y + deltaTime * currentVelocity.y;
    position.x = currentPosition.x + deltaTime * currentVelocity.x;
    state->position = position;
    state->physics->SetPosition(
        state->position, PhysicsObject::WORLD_COORDINATES);

    if (state->unknown_34 > lbl_806E4EE4)
    {
        state->unknown_34 -= deltaTime;
        if (state->unknown_34 <= lbl_806E4EE4)
        {
            state->unknown_34 = lbl_806E4EE4;
            state->scale = state->unknown_30;
        }
        else
        {
            float percent = deltaTime / state->unknown_34;
            if (percent > lbl_806E4EE0)
            {
                percent = lbl_806E4EE0;
            }
            state->scale =
                Interpolate(state->scale, state->unknown_30, percent);
        }
    }

    if (state->target != 0)
    {
        fn_802B549C(state, state->target->m_aActualFacingDirection);
    }
}

extern "C" void fn_8019AD18(BulletBillObject* state)
{
    fn_8019ABB8(state, true);

    state->position.x = lbl_806E4EE4;
    state->position.y = lbl_806E4EE4;
    state->position.z = lbl_806E4EE4;
    state->velocity.x = lbl_806E4EE4;
    state->velocity.y = lbl_806E4EE4;
    state->velocity.z = lbl_806E4EE4;
    state->orientation.z = lbl_806E4EE4;
    state->orientation.y = lbl_806E4EE4;
    state->orientation.x = lbl_806E4EE4;
    state->orientation.w = lbl_806E4EE0;
    state->scale = lbl_806E4EE0;
    state->unknown_30 = lbl_806E4EE0;
    state->unknown_34 = lbl_806E4EE4;
    state->target = 0;
}
