#ifndef _TRANSITION_H_
#define _TRANSITION_H_

#include "types.h"

struct Transition
{
    virtual float GetValue();
    virtual void Update(float dt, float multiplier);

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
