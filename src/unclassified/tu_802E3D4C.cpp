#include "types.h"

struct Effect_802E3D4C
{
    u32 unknown_00;
    float value_04;
    u8 pad_08[8];
    float value_10;
};

struct EffectEntry_802E3D4C
{
    u32 unknown_00;
    union
    {
        u32 index;
        Effect_802E3D4C* effect;
    } reference;
    u8 pad_08[0x50];
};

struct State_802E3D4C
{
    u32 unknown_00;
    EffectEntry_802E3D4C* entries;
    u32 count;
};

extern const float lbl_806E6550;
extern const float lbl_806E6554;

extern "C" bool fn_802E3D4C(const State_802E3D4C* state)
{
    u32 count = state->count;
    for (u32 i = 0; i < count; ++i)
    {
        if (state->entries[i].reference.effect->value_04 >= lbl_806E6550)
        {
            return true;
        }
    }

    for (u32 i = 0; i < count; ++i)
    {
        if (state->entries[i].reference.effect->value_10 >= lbl_806E6554)
        {
            return true;
        }
    }

    return false;
}

extern "C" void fn_802E42FC(
    State_802E3D4C* state, Effect_802E3D4C** table)
{
    for (u32 i = 0; i < state->count; ++i)
    {
        state->entries[i].reference.effect =
            table[state->entries[i].reference.index];
    }
}
