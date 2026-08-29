#include "NL/nlMemory.h"
#include "NL/nlPrint.h"
#include "types.h"

struct Owner_802ED74C
{
    u8 pad_00[0x1C];
    union
    {
        u32 field_1C;
        struct
        {
            s32 references : 16;
            u32 flags : 16;
        } count;
    };
};

struct State_802ED74C
{
    void** vtable;
    u32** field_04;
    u32 field_08;
    u32 field_0C;
    u8 field_10;
    u8 pad_11[3];
    Owner_802ED74C* owner;
    float field_18;
    float field_1C;
    u32 field_20;
    void (*callback)(void*, State_802ED74C*, void*);
    void* callbackContext;
};

extern void* lbl_8052F4F0[14];
extern char lbl_8052F4A0[0x2B];
extern char lbl_8052F4CC[0x21];
extern char lbl_806DF4C8[4];
extern float lbl_806E65B8;
extern void* lbl_806E201C;

extern "C" void fn_8004F594(int category, const char* format, ...);
extern "C" void fn_802ECC54(void* audioSystem);

extern "C" State_802ED74C* fn_802ED74C(State_802ED74C* state,
    void* value1, Owner_802ED74C* owner, void* value2,
    void (*callback)(void*, State_802ED74C*, void*),
    void* callbackContext)
{
    state->vtable = lbl_8052F4F0;
    state->field_04 = 0;
    state->field_08 = 0xFFFF;
    state->field_0C = 0;
    state->field_10 = false;
    state->owner = owner;
    state->field_18 = lbl_806E65B8;
    state->field_1C = lbl_806E65B8;
    state->field_20 = 0;
    state->callback = callback;
    state->callbackContext = callbackContext;

    typedef void (*InitializeFunc)(State_802ED74C*, void*, void*);
    ((InitializeFunc)state->vtable[13])(state, value1, value2);

    if (state->owner != 0)
    {
        state->owner->count.references++;
    }
    return state;
}

extern "C" State_802ED74C* fn_802ED7F0(
    State_802ED74C* state, int destroy)
{
    if (state != 0)
    {
        state->vtable = lbl_8052F4F0;
        if (state->owner != 0)
        {
            state->owner->count.references--;

            if ((state->owner->field_1C >> 12) & 1)
            {
                fn_802ECC54(lbl_806E201C);
                state->owner = 0;
            }
        }
        if (destroy > 0)
            operator delete(state);
    }
    return state;
}

extern "C" void fn_802ED88C(
    State_802ED74C* state, char* buffer, u32 size)
{
    nlSNPrintf(buffer, size, lbl_8052F4CC, **state->field_04, state->field_08, state->field_0C, state);
}

extern "C" void fn_802ED8BC(
    State_802ED74C* state, u32 value, u32 index)
{
    state->field_04 = (u32**)value;
    state->field_08 = index;
    state->field_0C = 1;
}

extern "C" void fn_802ED8D0(State_802ED74C* state, void* value)
{
    if (state->callback != 0)
    {
        state->callback(state->callbackContext, state, value);
        return;
    }
    fn_8004F594(10, lbl_8052F4A0, value);
}

extern "C" void fn_802ED904(State_802ED74C* state)
{
    char buffer[0x100];
    nlSNPrintf(buffer, sizeof(buffer), lbl_8052F4CC, **state->field_04, state->field_08, state->field_0C);
    fn_8004F594(10, lbl_806DF4C8, buffer);
}
