#include "Game/Audio/Transition.h"

void Transition::Update(float dt, float multiplier)
{
    valid = enabled;
    enabled = false;
    duration = multiplier;
    if (value == target)
    {
        elapsed = 0.0f;
        return;
    }

    valid = true;
    if (elapsed <= dt)
    {
        value = target;
        elapsed = 0.0f;
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
