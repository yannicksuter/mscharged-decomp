#ifndef GAME_AUDIO_AUDIO_RESOURCE_LOADER_H
#define GAME_AUDIO_AUDIO_RESOURCE_LOADER_H

#include "types.h"

class MemoryAllocator;
class AudioBankLoader;
class SoundMap;
struct AudioResourceSource;

struct AudioResourceLoadOwner;
typedef void (*AudioResourceLoadCallback)(
    AudioResourceLoadOwner*, void*);

struct AudioResourceLoadOwner
{
    AudioResourceLoadOwner()
        : m_Source(0)
        , m_LoadedData(0)
        , m_Completed(false)
        , m_ResourceObject(0)
        , m_Loader(0)
        , m_SoundMap(0)
        , m_Callback(0)
        , m_CallbackParam(0)
        , m_Unknown20(true)
        , m_Allocator(0)
    {
    }

    AudioResourceSource* m_Source;
    void* m_LoadedData;
    u8 m_Completed;
    u8 m_Unknown09[3];
    void* m_ResourceObject;
    AudioBankLoader* m_Loader;
    SoundMap* m_SoundMap;
    AudioResourceLoadCallback m_Callback;
    void* m_CallbackParam;
    u8 m_Unknown20;
    u8 m_Unknown21[3];
    MemoryAllocator* m_Allocator;
};

void UnloadAudioResource(AudioResourceLoadOwner* state);
void LoadAudioResource(AudioResourceLoadOwner* state,
    const char* name, AudioResourceLoadCallback field18,
    void* field1C, MemoryAllocator* allocator);

class AudioResourceLoader
{
public:
    AudioResourceLoader(AudioResourceLoadOwner* owner);

    virtual void Unload() = 0;
    virtual void OnLoadComplete();

protected:
    AudioResourceLoadOwner* m_Owner;
    u32 m_Unknown08;
    MemoryAllocator* m_Allocator;
};

class nlChunk;
struct ResourceBundle;
ResourceBundle* ParseAudioResourceBundle(nlChunk* chunk);

#endif // GAME_AUDIO_AUDIO_RESOURCE_LOADER_H
