#ifndef GAME_UNIDENTIFIED_TWEAK_ACTION_H
#define GAME_UNIDENTIFIED_TWEAK_ACTION_H

#include "NL/nlFunction.h"

// Placeholder for the tweak-tree action registered by name and category with a
// callback; its retail constructor is the empty function at 0x800F3A10.
struct UnidentifiedTweakAction
{
    UnidentifiedTweakAction(const char* name, const char* category,
        const Function0<void>& action);
};

#endif // GAME_UNIDENTIFIED_TWEAK_ACTION_H
