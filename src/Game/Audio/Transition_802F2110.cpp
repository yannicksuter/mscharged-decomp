#include "Game/Audio/Transition_802F2110.h"

void Transition_802F2110::fn_802EB5BC(float dt, float multiplier)
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
    elapsed -= dt;
    float delta = t * (target - value);
    value += delta;
}

float Transition_802F2110::fn_802EB634()
{
    return duration * value;
}
