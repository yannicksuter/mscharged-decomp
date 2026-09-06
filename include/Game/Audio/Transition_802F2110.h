#ifndef _TRANSITION_802F2110_H_
#define _TRANSITION_802F2110_H_

#include "types.h"

struct Transition_802F2110
{
    virtual float fn_802EB634();
    virtual void fn_802EB5BC(float dt, float multiplier);

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
