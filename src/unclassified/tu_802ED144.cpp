#include "Game/Audio/SoundMap.h"
#include "Game/SAnim.h"
#include "NL/MemAlloc.h"
#include "NL/nlFile.h"
#include "NL/nlMemory.h"
#include "NL/nlPrint.h"
#include "types.h"

struct Count_802ED144
{
    u32 field_00;
    u32 count;
};

struct Source_802ED144
{
    u8 pad_00[8];
    Count_802ED144* count;
    u8 pad_0C[9];
    u8 useCompactCallback;
};

struct Callback_802ED144
{
    void** vtable;
    void* owner;
    u32 field_08;
    MemoryAllocator* allocator;
    u32 field_10;
    u32 field_14;
    u32 field_18;
    u32 field_1C;
};

struct State_802ED144
{
    Source_802ED144* source;
    void* fileData;
    u8 finished;
    u8 pad_09[3];
    void* object_0C;
    Callback_802ED144* callback;
    SoundMap* soundMap;
    void (*completion)(State_802ED144*, void*);
    void* completionContext;
    u32 field_20;
    MemoryAllocator* allocator;
};

struct AudioSystem_802ED144
{
    void** vtable;
};

extern MemoryAllocator* AllocatorStack[16];
extern unsigned int AllocatorStackDepth;

extern void* lbl_80534C80[7];
extern void* lbl_80534C9C[7];
extern char lbl_8052F490[0xC];
extern AudioSystem_802ED144* lbl_806E201C;

extern "C" Callback_802ED144* fn_802EDA38(
    Callback_802ED144*, State_802ED144*);
extern "C" void* fn_802F0860(nlChunk* chunk);

static inline void PushAllocator_802ED144(MemoryAllocator* allocator)
{
    AllocatorStack[AllocatorStackDepth++] = allocator;
    CurrentAllocator = allocator;
}

static inline void PopAllocator_802ED144()
{
    --AllocatorStackDepth;
    AllocatorStack[AllocatorStackDepth] = 0;
    CurrentAllocator = AllocatorStack[AllocatorStackDepth - 1];
}

extern "C" void fn_802ED144(
    void* data, unsigned long, State_802ED144* state)
{
    PushAllocator_802ED144(state->allocator);
    state->fileData = data;

    nlChunk* outer = (nlChunk*)data;
    nlChunk* chunk = (nlChunk*)outer->GetData();
    nlChunk* end = outer->GetNextChunk();
    while (chunk != end)
    {
        switch (chunk->GetID())
        {
        case 0x80023000:
            state->soundMap = SoundMap::fn_802EE458(chunk);
            break;
        case 0x80023300:
        case 0x23701:
            state->object_0C = fn_802F0860(chunk);
            break;
        case 0x80023200:
        case 0x23703:
        case 0x23705:
        case 0x23706:
        {
            typedef void (*ProcessFunc)(Callback_802ED144*, nlChunk*);
            ((ProcessFunc)state->callback->vtable[4])(
                state->callback, chunk);
            break;
        }
        default:
            break;
        }
        chunk = chunk->GetNextChunk();
    }

    PopAllocator_802ED144();
    typedef void (*FinishFunc)(Callback_802ED144*, u32);
    ((FinishFunc)state->callback->vtable[6])(
        state->callback, state->source->count->count);
}

extern "C" void fn_802ED37C(State_802ED144* state)
{
    PushAllocator_802ED144(state->allocator);
    if (state->soundMap != 0)
    {
        state->soundMap->fn_802EE20C();
        state->soundMap = 0;
    }
    if (state->callback != 0)
    {
        typedef void (*ShutdownFunc)(Callback_802ED144*);
        typedef void (*DestroyFunc)(Callback_802ED144*, int);
        ((ShutdownFunc)state->callback->vtable[2])(state->callback);
        if (state->callback != 0)
            ((DestroyFunc)state->callback->vtable[5])(
                state->callback, 1);
        state->callback = 0;
    }
    if (state->object_0C != 0)
        state->object_0C = 0;
    nlFree(state->fileData);
    state->fileData = 0;
    state->finished = false;
    PopAllocator_802ED144();
}

extern "C" u32 fn_802ED478(State_802ED144* state,
    u32 field0, u32 field4, u32 field8, u32 fieldC)
{
    if (state->finished)
        return state->soundMap->fn_802EE178(
            field0, field4, field8, fieldC);
    return 0xFFFF;
}

extern "C" void fn_802ED498(State_802ED144* state,
    const char* name, u32 field18, u32 field1C,
    MemoryAllocator* allocator)
{
    state->allocator = allocator != 0 ? allocator : CurrentAllocator;
    PushAllocator_802ED144(state->allocator);

    Callback_802ED144* callback;
    if (state->source->useCompactCallback)
    {
        callback = (Callback_802ED144*)nlMalloc(0x1C, 8, false);
        if (callback != 0)
        {
            fn_802EDA38(callback, state);
            callback->field_10 = 0;
            callback->field_14 = 0;
            callback->vtable = lbl_80534C80;
            callback->field_18 = 0;
        }
    }
    else
    {
        callback = (Callback_802ED144*)nlMalloc(0x20, 8, false);
        if (callback != 0)
        {
            fn_802EDA38(callback, state);
            callback->field_10 = 0;
            callback->field_14 = 0;
            callback->vtable = lbl_80534C9C;
            callback->field_18 = 0;
            callback->field_1C = 0;
        }
    }
    state->callback = callback;
    PopAllocator_802ED144();

    state->completion = (void (*)(State_802ED144*, void*))field18;
    state->completionContext = (void*)field1C;

    char path[0x80];
    nlSNPrintf(path, sizeof(path), lbl_8052F490, (char*)lbl_806E201C + 0x4A, name);

    typedef bool (*IsAsyncFunc)(AudioSystem_802ED144*);
    if (((IsAsyncFunc)lbl_806E201C->vtable[2])(lbl_806E201C))
    {
        nlLoadEntireFileAsync(path, (LoadAsyncCallback)fn_802ED144, state, 0x20, AllocateStart, 0, 0, state->allocator);
        return;
    }

    unsigned long size;
    void* data = nlLoadEntireFile(path, &size, 0x20, AllocateStart, 0, 0, state->allocator);
    fn_802ED144(data, size, state);
}
