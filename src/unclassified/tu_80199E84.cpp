#include "Game/Physics/PhysicsBirdoEgg.h"

#include "unclassified/tu_80199E84.h"

#include "NL/nlMath.h"
#include "NL/nlMemory.h"
#include "types.h"

extern "C" void fn_8019A434(State_80199E84*, bool);
extern const nlVector3 lbl_804DCD90;
extern const nlVector3 lbl_804DCD9C;
extern const float lbl_806DCD7C;
extern const float lbl_806E4ED0;
extern const float lbl_806E4ED4;
extern const float lbl_806E4EDC;

extern "C" State_80199E84* fn_80199E84(
    State_80199E84* state, void* drawable)
{
    state->unknown_20 = lbl_806DCD7C;
    state->drawable = drawable;
    state->unknown_24 = lbl_806DCD7C;
    state->unknown_28 = lbl_806E4ED0;
    state->unknown_2C = lbl_806E4ED0;
    state->visible = false;
    state->unknown_3C = 0;
    state->unknown_40 = lbl_804DCD90;
    state->position = lbl_804DCD9C;
    state->unknown_10 = lbl_806E4ED0;
    state->orientation.z = lbl_806E4ED0;
    state->orientation.y = lbl_806E4ED0;
    state->orientation.x = lbl_806E4ED0;
    state->orientation.w = lbl_806E4ED4;

    PhysicsBirdoEgg* physics = new (8, false) PhysicsBirdoEgg(
        (BirdoEggObject*)state, lbl_806DCD7C);
    state->physics = physics;
    physics->SetPosition(
        lbl_804DCD9C, PhysicsObject::WORLD_COORDINATES);
    physics->DisableCollisions();
    return state;
}

extern "C" State_80199E84* fn_80199F6C(
    State_80199E84* state, int shouldDelete)
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

extern "C" void fn_8019A248(
    State_80199E84* state, const nlVector3& position)
{
    state->position = position;
    state->physics->SetPosition(
        state->position, PhysicsObject::WORLD_COORDINATES);
}

extern "C" float fn_8019A574(const State_80199E84* state)
{
    return state->unknown_20 / lbl_806DCD7C;
}

extern "C" void fn_8019A584(State_80199E84* state)
{
    fn_8019A434(state, true);

    state->unknown_10 = lbl_806E4ED0;
    state->position = lbl_804DCD9C;
    state->orientation.z = lbl_806E4ED0;
    state->orientation.y = lbl_806E4ED0;
    state->orientation.x = lbl_806E4ED0;
    state->orientation.w = lbl_806E4ED4;
    state->unknown_20 = lbl_806E4ED4;
    state->unknown_24 = lbl_806E4ED4;
    state->unknown_28 = lbl_806E4ED0;
    state->unknown_3C = 0;
    state->unknown_2C = lbl_806E4ED0;

    state->physics->SetRadius(lbl_806E4EDC);
    state->physics->SetPosition(
        state->position, PhysicsObject::WORLD_COORDINATES);
}
