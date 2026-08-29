#ifndef GAME_AUDIO_AUDIO_RESOURCE_LOADER_802EDA38_H
#define GAME_AUDIO_AUDIO_RESOURCE_LOADER_802EDA38_H

#include "types.h"

class MemoryAllocator;
class AudioBankLoader_802EF6B0;
class SoundMap;
struct AudioResourceSource_802ED144;

struct AudioResourceLoadOwner_802EDA54;
typedef void (*AudioResourceLoadCallback_802EDA54)(
    AudioResourceLoadOwner_802EDA54*, void*);

struct AudioResourceLoadOwner_802EDA54
{
    AudioResourceSource_802ED144* m_Source;
    void* m_LoadedData;
    u8 m_Completed;
    u8 m_Unknown09[3];
    void* m_ResourceObject;
    AudioBankLoader_802EF6B0* m_Loader;
    SoundMap* m_SoundMap;
    AudioResourceLoadCallback_802EDA54 m_Callback;
    void* m_CallbackParam;
    u32 m_Unknown20;
    MemoryAllocator* m_Allocator;
};

class AudioResourceLoader_802EDA38
{
public:
    AudioResourceLoader_802EDA38(AudioResourceLoadOwner_802EDA54* owner);

    virtual void fn_802EDA38_0() = 0;
    virtual void fn_802EDA54();

protected:
    AudioResourceLoadOwner_802EDA54* m_Owner;
    u32 m_Unknown08;
    MemoryAllocator* m_Allocator;
};

#endif // GAME_AUDIO_AUDIO_RESOURCE_LOADER_802EDA38_H
