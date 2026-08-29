#include "Game/Audio/AudioLoadMode_806E201C.h"
#include "Game/Audio/AudioResourcePlatform_8035DE28.h"
#include "Game/Audio/SoundMap.h"
#include "Game/SAnim.h"
#include "NL/MemAlloc.h"
#include "NL/nlFile.h"
#include "NL/nlMemory.h"
#include "NL/nlPrint.h"
#include "types.h"

struct AudioResourceName_802ED144
{
    u32 field_00;
    const char* name;
};

struct AudioResourceSource_802ED144
{
    u8 pad_00[8];
    AudioResourceName_802ED144* resource;
    u8 pad_0C[9];
    u8 useCompactCallback;
};

extern MemoryAllocator* AllocatorStack[16];
extern unsigned int AllocatorStackDepth;

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
    void* data, unsigned long, AudioResourceLoadOwner_802EDA54* state)
{
    PushAllocator_802ED144(state->m_Allocator);
    state->m_LoadedData = data;

    nlChunk* outer = (nlChunk*)data;
    nlChunk* chunk = (nlChunk*)outer->GetData();
    while (chunk != outer->GetNextChunk())
    {
        switch (chunk->GetID())
        {
        case 0x80023000:
            state->m_SoundMap = SoundMap::fn_802EE458(chunk);
            break;
        case 0x80023300:
            state->m_ResourceObject = fn_802F0860(chunk);
            break;
        case 0x23701:
            state->m_ResourceObject = fn_802F0860(chunk);
            break;
        case 0x80023200:
        case 0x23703:
        case 0x23705:
        case 0x23706:
        {
            state->m_Loader->fn_802EF6B0(chunk);
            break;
        }
        default:
            break;
        }
        chunk = chunk->GetNextChunk();
    }

    PopAllocator_802ED144();
    state->m_Loader->fn_8035E3CC(state->m_Source->resource->name);
}

extern "C" void fn_802ED37C(AudioResourceLoadOwner_802EDA54* state)
{
    PushAllocator_802ED144(state->m_Allocator);
    if (state->m_SoundMap != 0)
    {
        state->m_SoundMap->fn_802EE20C();
        state->m_SoundMap = 0;
    }
    if (state->m_Loader != 0)
    {
        state->m_Loader->fn_802EDA38_0();
        delete state->m_Loader;
        state->m_Loader = 0;
    }
    if (state->m_ResourceObject != 0)
        state->m_ResourceObject = 0;
    nlFree(state->m_LoadedData);
    state->m_LoadedData = 0;
    state->m_Completed = false;
    PopAllocator_802ED144();
}

extern "C" u32 fn_802ED478(AudioResourceLoadOwner_802EDA54* state,
    u32 field0, u32 field4, u32 field8, u32 fieldC)
{
    if (state->m_Completed)
        return state->m_SoundMap->fn_802EE178(
            field0, field4, field8, fieldC);
    return 0xFFFF;
}

extern "C" void fn_802ED498(AudioResourceLoadOwner_802EDA54* state,
    const char* name, u32 field18, u32 field1C,
    MemoryAllocator* allocator)
{
    if (allocator != 0)
        state->m_Allocator = allocator;
    else
        state->m_Allocator = CurrentAllocator;
    PushAllocator_802ED144(state->m_Allocator);

    AudioBankLoader_802EF6B0* loader;
    if (state->m_Source->useCompactCallback)
    {
        loader = new (8, false) AudioFileLoader_8035E7B4(
            state);
        state->m_Loader = loader;
    }
    else
    {
        loader = new (8, false) AudioMemoryLoader_8035E3CC(
            state);
        state->m_Loader = loader;
    }
    PopAllocator_802ED144();

    state->m_Callback = (AudioResourceLoadCallback_802EDA54)field18;
    state->m_CallbackParam = (void*)field1C;

    char path[0x80];
    nlSNPrintf(path, sizeof(path), "%s%s.resbun", (char*)lbl_806E201C + 0x4A, name);

    if (lbl_806E201C->fn_806E201C_0())
    {
        nlLoadEntireFileAsync(path, (LoadAsyncCallback)fn_802ED144, state, 0x20, AllocateStart, 0, 0, state->m_Allocator);
        return;
    }

    unsigned long size;
    void* data = nlLoadEntireFile(path, &size, 0x20, AllocateStart, 0, 0, state->m_Allocator);
    fn_802ED144(data, size, state);
}
