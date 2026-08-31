#include "Game/AI/AiUtil.h"
#include "Game/Physics/PhysicsObject.h"

#include "NL/nlMemory.h"
#include "types.h"

struct Target_8019A710
{
    u8 pad_00[0x62];
    u16 value_62;
};

struct State_8019A710
{
    float unknown_00;
    float unknown_04;
    float unknown_08;
    float unknown_0C;
    nlVector3 position;
    nlVector3 velocity;
    u32 unknown_28;
    float unknown_2C;
    float unknown_30;
    float unknown_34;
    bool active;
    u8 pad_39[3];
    PhysicsObject* physics;
    void* unknown_40;
    Target_8019A710* target;
};

extern "C" void fn_8019ABB8(State_8019A710*, bool);
extern "C" PhysicsObject* fn_8014236C(
    void*, State_8019A710*, float, float);
extern "C" void fn_802B549C(State_8019A710*, u16);

extern const float lbl_806E4EE0;
extern const float lbl_806E4EE4;

extern "C" State_8019A710* fn_8019A710(State_8019A710* state,
    void* unknown, u32 value, float radius, float parameter)
{
    state->unknown_28 = value;
    state->unknown_40 = unknown;
    state->unknown_2C = lbl_806E4EE0;
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
    state->unknown_08 = lbl_806E4EE4;
    state->unknown_04 = lbl_806E4EE4;
    state->unknown_00 = lbl_806E4EE4;
    state->unknown_0C = lbl_806E4EE0;

    PhysicsObject* physics =
        (PhysicsObject*)nlMalloc(0x40, 8, false);
    if (physics != 0)
    {
        physics = fn_8014236C(physics, state, radius, parameter);
    }
    state->physics = physics;
    physics->DisableCollisions();
    return state;
}

extern "C" State_8019A710* fn_8019A7E4(
    State_8019A710* state, int shouldDelete)
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

extern "C" void fn_8019A854(State_8019A710* state, float deltaTime)
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
            state->unknown_2C = state->unknown_30;
        }
        else
        {
            float percent = deltaTime / state->unknown_34;
            if (percent > lbl_806E4EE0)
            {
                percent = lbl_806E4EE0;
            }
            state->unknown_2C =
                Interpolate(state->unknown_2C, state->unknown_30, percent);
        }
    }

    if (state->target != 0)
    {
        fn_802B549C(state, state->target->value_62);
    }
}

extern "C" void fn_8019AD18(State_8019A710* state)
{
    fn_8019ABB8(state, true);

    state->position.x = lbl_806E4EE4;
    state->position.y = lbl_806E4EE4;
    state->position.z = lbl_806E4EE4;
    state->velocity.x = lbl_806E4EE4;
    state->velocity.y = lbl_806E4EE4;
    state->velocity.z = lbl_806E4EE4;
    state->unknown_08 = lbl_806E4EE4;
    state->unknown_04 = lbl_806E4EE4;
    state->unknown_00 = lbl_806E4EE4;
    state->unknown_0C = lbl_806E4EE0;
    state->unknown_2C = lbl_806E4EE0;
    state->unknown_30 = lbl_806E4EE0;
    state->unknown_34 = lbl_806E4EE4;
    state->target = 0;
}
