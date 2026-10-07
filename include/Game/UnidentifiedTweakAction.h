#ifndef GAME_UNIDENTIFIED_TWEAK_ACTION_H
#define GAME_UNIDENTIFIED_TWEAK_ACTION_H

#include "NL/nlFunction.h"

// Callers supply a tweak name, category and callback; the retail constructor
// at 0x800F3A10 returns without storing them or registering an action.
struct UnidentifiedTweakAction
{
    UnidentifiedTweakAction(const char* name, const char* category,
        const Function0<void>& action);
};

#endif // GAME_UNIDENTIFIED_TWEAK_ACTION_H
