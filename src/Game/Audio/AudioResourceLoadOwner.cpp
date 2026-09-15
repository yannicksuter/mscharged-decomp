#include "Game/Audio/AudioBankTable.h"
#include "Game/Audio/RegistryPools.h"
#include "Game/Audio/AudioSystem.h"
#include "Game/Audio/AudioResourcePlatform.h"
#include "Game/Audio/SoundMap.h"
#include "NL/nlChunk.h"
#include "NL/MemAlloc.h"
#include "NL/nlFile.h"
#include "NL/nlMemory.h"
#include "NL/nlPrint.h"
#include "types.h"


static inline void PushAllocator(MemoryAllocator* allocator)
{
    AllocatorStack[AllocatorStackDepth++] = allocator;
    CurrentAllocator = allocator;
}

static inline void PopAllocator()
{
    --AllocatorStackDepth;
    AllocatorStack[AllocatorStackDepth] = 0;
    CurrentAllocator = AllocatorStack[AllocatorStackDepth - 1];
}

void OnAudioResourceBundleLoaded(
    void* data, unsigned long, AudioResourceLoadOwner* state)
{
    PushAllocator(state->m_Allocator);
    state->m_LoadedData = data;

    nlChunk* outer = (nlChunk*)data;
    nlChunk* chunk = (nlChunk*)outer->GetData();
    while (chunk != outer->GetLastChunk())
    {
        switch (chunk->GetID())
        {
        case 0x80023000:
            state->m_SoundMap = SoundMap::ParseChunk(chunk);
            break;
        case 0x80023300:
            state->m_ResourceObject = ParseAudioResourceBundle(chunk);
            break;
        case 0x23701:
            state->m_ResourceObject = ParseAudioResourceBundle(chunk);
            break;
        case 0x80023200:
        case 0x23703:
        case 0x23705:
        case 0x23706:
        {
            state->m_Loader->ParseChunk(chunk);
            break;
        }
        default:
            break;
        }
        chunk = chunk->GetNextChunk();
    }

    PopAllocator();
    state->m_Loader->Load(state->m_Source->resource->name);
}

void UnloadAudioResource(AudioResourceLoadOwner* state)
{
    PushAllocator(state->m_Allocator);
    if (state->m_SoundMap != 0)
    {
        state->m_SoundMap->Unload();
        state->m_SoundMap = 0;
    }
    if (state->m_Loader != 0)
    {
        state->m_Loader->Unload();
        delete state->m_Loader;
        state->m_Loader = 0;
    }
    if (state->m_ResourceObject != 0)
        state->m_ResourceObject = 0;
    nlFree(state->m_LoadedData);
    state->m_LoadedData = 0;
    state->m_Completed = false;
    PopAllocator();
}

u32 FindAudioResourceCue(AudioResourceLoadOwner* state,
    u32 field0, u32 field4, u32 field8, u32 fieldC)
{
    if (state->m_Completed)
        return state->m_SoundMap->FindCue(
            field0, field4, field8, fieldC);
    return 0xFFFF;
}

void LoadAudioResource(AudioResourceLoadOwner* state,
    const char* name, AudioResourceLoadCallback field18, void* field1C,
    MemoryAllocator* allocator)
{
    if (allocator != 0)
        state->m_Allocator = allocator;
    else
        state->m_Allocator = CurrentAllocator;
    PushAllocator(state->m_Allocator);

    AudioBankLoader* loader;
    if (state->m_Source->useCompactCallback)
    {
        loader = new (8, false) AudioFileLoader(
            state);
        state->m_Loader = loader;
    }
    else
    {
        loader = new (8, false) AudioMemoryLoader(
            state);
        state->m_Loader = loader;
    }
    PopAllocator();

    state->m_Callback = field18;
    state->m_CallbackParam = field1C;

    char path[0x80];
    nlSNPrintf(path, sizeof(path), "%s%s.resbun", (char*)g_pAudioSystem + 0x4A, name);

    if (g_pAudioSystem->IsAsyncLoading())
    {
        nlLoadEntireFileAsync(path, (LoadAsyncCallback)OnAudioResourceBundleLoaded, state, 0x20, AllocateStart, 0, 0, state->m_Allocator);
        return;
    }

    unsigned long size;
    void* data = nlLoadEntireFile(path, &size, 0x20, AllocateStart, 0, 0, state->m_Allocator);
    OnAudioResourceBundleLoaded(data, size, state);
}
