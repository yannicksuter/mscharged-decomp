#ifndef _TRANSITION_H_
#define _TRANSITION_H_

#include "types.h"

struct Transition
{
    Transition()
    {
        value = 0.0f;
        valid = true;
        target = 0.0f;
        remainingTime = -1.0f;
        modifier = 1.0f;
        minimum = 0.0f;
        maximum = 1.0f;
        enabled = true;
    }
    ~Transition() { }

    virtual float GetValue();
    virtual void Update(float dt, float outputModifier);

    void SetTarget(float newTarget, float transitionTime)
    {
        if (newTarget < minimum)
            target = minimum;
        else if (newTarget > maximum)
            target = maximum;
        else
            target = newTarget;
        remainingTime = transitionTime;
    }

    void Reset(float initialValue, float minimumValue, float maximumValue)
    {
        target = initialValue;
        value = initialValue;
        minimum = minimumValue;
        maximum = maximumValue;
        valid = true;
    }

    float value;
    u8 valid;
    u8 pad_09[3];
    float target;
    float remainingTime;
    float modifier;
    float minimum;
    float maximum;
    u8 enabled;
    u8 pad_21[3];
};

#endif
