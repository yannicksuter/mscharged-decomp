#ifndef GAME_AUDIO_AUDIO_RESOURCE_PLATFORM_H
#define GAME_AUDIO_AUDIO_RESOURCE_PLATFORM_H

#include "Game/Audio/AudioBankLoader.h"
#include "NL/nlArrayAllocator.h"

class AudioBundleManager;
class nlChunk;
class nlFile;
struct SPSoundTable;

class AudioMemoryLoader : public AudioBankLoader
{
public:
    AudioMemoryLoader(AudioResourceLoadOwner* owner)
        : AudioBankLoader(owner)
        , m_SoundTable(0)
        , m_Data(0)
    {
    }

    virtual ~AudioMemoryLoader() { }

    virtual void Unload();
    virtual void ParseChunk(nlChunk* chunk);
    virtual void Load(const char* name);

    SPSoundTable* m_SoundTable;
    void* m_Data;
};

class AudioFileLoader : public AudioBankLoader
{
public:
    AudioFileLoader(AudioResourceLoadOwner* owner)
        : AudioBankLoader(owner)
        , m_File(0)
    {
    }

    virtual ~AudioFileLoader() { }

    virtual void Unload();
    virtual void Load(const char* name);

private:
    nlFile* m_File;
};

struct AudioStreamBlock
{
    unsigned char m_Unknown00[0x60];
};

extern nlArrayAllocator<AudioStreamBlock>* g_pAudioStreamBlockPool;

void DumpAudioBankMemory(const char* path);
bool InitializeAudioStreamBlockPool();

#endif // GAME_AUDIO_AUDIO_RESOURCE_PLATFORM_H
