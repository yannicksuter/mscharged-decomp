#ifndef GAME_TWEAK_BINDING_FLOAT_H
#define GAME_TWEAK_BINDING_FLOAT_H
#include "Game/TweakValueFloat.h"
inline float TweakType<float>::ReadOwned(TweakValueBase* value)
{
    return ((OwnedValue*)value)->value;
}
#endif
