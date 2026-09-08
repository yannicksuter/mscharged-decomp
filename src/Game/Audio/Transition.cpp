#include "Game/Audio/Transition.h"

extern "C" const float lbl_806E65A8;

void Transition::Update(float dt, float multiplier)
{
    valid = enabled;
    enabled = false;
    duration = multiplier;
    if (value == target)
    {
        elapsed = lbl_806E65A8;
        return;
    }

    valid = true;
    if (elapsed <= dt)
    {
        value = target;
        elapsed = lbl_806E65A8;
        return;
    }

    float t = dt / elapsed;
    float delta = t * (target - value);
    elapsed -= dt;
    value += delta;
}

float Transition::GetValue()
{
    return duration * value;
}
