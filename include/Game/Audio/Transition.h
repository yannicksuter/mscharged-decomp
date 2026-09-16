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
        elapsed = -1.0f;
        duration = 1.0f;
        minimum = 0.0f;
        maximum = 1.0f;
        enabled = true;
    }
    ~Transition() { }

    virtual float GetValue();
    virtual void Update(float dt, float multiplier);

    void SetTarget(float newTarget, float transitionTime)
    {
        if (newTarget < minimum)
            target = minimum;
        else if (newTarget > maximum)
            target = maximum;
        else
            target = newTarget;
        elapsed = transitionTime;
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
    float elapsed;
    float duration;
    float minimum;
    float maximum;
    u8 enabled;
    u8 pad_21[3];
};

#endif
