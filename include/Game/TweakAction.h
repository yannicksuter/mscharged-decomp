#ifndef GAME_TWEAK_ACTION_H
#define GAME_TWEAK_ACTION_H

#include "NL/nlFunction.h"

// Callers supply a tweak name, category and callback; the retail constructor
// at 0x800F3A10 returns without storing them or registering an action.
struct TweakAction
{
    TweakAction(const char* name, const char* category,
        const Function0<void>& action);
};

#endif // GAME_TWEAK_ACTION_H
