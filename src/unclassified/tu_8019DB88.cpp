#include "types.h"

struct State_8019DB88
{
    bool enabled;
};

extern "C" void fn_8019DB88(State_8019DB88*)
{
}

extern "C" void fn_8019DB8C(State_8019DB88* state)
{
    state->enabled = true;
}

extern "C" void fn_8019DB98(State_8019DB88* state)
{
    state->enabled = false;
}
