#include "NL/nlMemory.h"

#include "types.h"

struct UnidentifiedWeatherState
{
    void* vtable;
    s32 field04;
    s32 field08;
    u8 field0C;
    u8 padding0D[3];
    float field10;
    float field14;
    float field18;
};

struct UnidentifiedPool
{
    u8 padding00[0x0C];
    void* freeList;
};

struct UnidentifiedPoolEntry
{
    UnidentifiedPoolEntry* next;
};

struct UnidentifiedWeatherExtendedStateA
{
    void* vtable;
    s32 field04;
    s32 field08;
    u8 field0C;
    u8 padding0D[3];
    u32 field10;
    u32 field14;
    s32 field18;
    float field1C;
    s32 field20;
    s32 field24;
};

struct UnidentifiedWeatherExtendedStateB
{
    void* vtable;
    s32 field04;
    s32 field08;
    u8 field0C;
    u8 padding0D[3];
    u32 field10;
    u32 field14;
    s16 field18;
    u8 padding1A[2];
    float field1C;
    s32 field20;
    s32 field24;
};

extern "C" u8 lbl_80500F90[];
extern "C" float lbl_806DBF28;
extern "C" s32 lbl_806DBF64;
extern "C" float lbl_806DBF68;
extern "C" float lbl_806DBF80;
extern "C" float lbl_806DBF84;

extern "C" void fn_800AD400(UnidentifiedWeatherState*);
extern "C" void fn_800AEDAC(UnidentifiedWeatherState*, int);
extern "C" void fn_802B1D4C(void*, unsigned long);

extern "C" void fn_800AA688(UnidentifiedWeatherState* state)
{
    state->field04 = 0;
    state->vtable = lbl_80500F90;
    state->field08 = 0;
    state->field0C = 0;
}

extern "C" void fn_800AA6A8(UnidentifiedWeatherState* state)
{
    state->field08 = 1;
}

extern "C" void fn_800AA7C4(UnidentifiedWeatherState* state)
{
    state->field08 = 0;
}

extern "C" void fn_800AA8C8(UnidentifiedWeatherState* state)
{
    state->field08 = 0;
    state->field0C = 0;
}

extern "C" float fn_800AA94C()
{
    return lbl_806DBF68;
}

extern "C" void fn_800AA954(UnidentifiedWeatherExtendedStateA* state)
{
    u32 value = state->field14;
    state->field08 = 0;
    state->field0C = 0;
    state->field10 = value != 0;
    state->field14 = 0;
    state->field18 = lbl_806DBF64;
}

extern "C" float fn_800AB23C()
{
    return lbl_806DBF28;
}

extern "C" void fn_800AB244(UnidentifiedWeatherExtendedStateB* state)
{
    u32 value = state->field14;
    state->field08 = 0;
    state->field0C = 0;
    state->field10 = value != 0;
    state->field14 = 0;
    state->field18 = 0;
    state->field1C = 0.0f;
    state->field20 = -1;
    state->field24 = -1;
}

extern "C" float fn_800AC6C0(const UnidentifiedWeatherState* state)
{
    return state->field18;
}

extern "C" void fn_800AC6C8(UnidentifiedWeatherState* state)
{
    state->field08 = 0;
    state->field0C = 0;
}

extern "C" void fn_800ACC58(UnidentifiedWeatherState* state)
{
    state->field08 = 1;
}

extern "C" void fn_800AD3E8(UnidentifiedWeatherState* state, bool initialize)
{
    state->field08 = 0;
    if (initialize)
    {
        fn_800AD400(state);
    }
}

extern "C" void fn_800AD7A4(UnidentifiedWeatherState* state)
{
    state->field10 = lbl_806DBF80;
    state->field14 = lbl_806DBF84;
    state->field18 = 0.0f;
    state->field08 = 0;
    state->field0C = 0;
}

extern "C" float fn_800AE260(const UnidentifiedWeatherState* state)
{
    if (state->field10 <= 0.0f)
    {
        return 1.0f;
    }
    return 0.0f;
}

extern "C" void fn_800AE27C(UnidentifiedWeatherState* state)
{
    fn_800AEDAC(state, 0);
    state->field08 = 0;
    state->field0C = 0;
}

extern "C" void fn_800AFEE4(UnidentifiedPool* pool, UnidentifiedPoolEntry* entry)
{
    entry->next = (UnidentifiedPoolEntry*)pool->freeList;
    pool->freeList = entry;
}

extern "C" void* fn_800AFEF4(unsigned long size, unsigned int alignment, bool fromEnd)
{
    return nlMalloc(size, alignment, fromEnd);
}

extern "C" float fn_800AFFD8()
{
    return 0.0f;
}

extern "C" void* fn_800B019C(void* object, int destroy)
{
    if (object != 0 && destroy > 0)
    {
        fn_802B1D4C(object, 0x14);
    }
    return object;
}

extern "C" void* fn_800B01E0(void* object, int destroy)
{
    if (object != 0 && destroy > 0)
    {
        fn_802B1D4C(object, 0x14);
    }
    return object;
}

extern "C" void* fn_800B0224(void* object, int destroy)
{
    if (object != 0 && destroy > 0)
    {
        fn_802B1D4C(object, 0x14);
    }
    return object;
}
