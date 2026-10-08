#include "Game/Audio/Transition.h"

void Transition::Update(float dt, float outputModifier)
{
    valid = enabled;
    enabled = false;
    modifier = outputModifier;
    if (value == target)
    {
        remainingTime = 0.0f;
        return;
    }

    valid = true;
    if (remainingTime <= dt)
    {
        value = target;
        remainingTime = 0.0f;
        return;
    }

    float t = dt / remainingTime;
    float delta = t * (target - value);
    remainingTime -= dt;
    value += delta;
}

float Transition::GetValue()
{
    return modifier * value;
}
