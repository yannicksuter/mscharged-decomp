#ifndef GAME_AUDIO_AUDIO_BANK_LOADER_H
#define GAME_AUDIO_AUDIO_BANK_LOADER_H

#include "Game/Audio/AudioResourceLoader.h"

class nlChunk;

class AudioBankLoader : public AudioResourceLoader
{
public:
    AudioBankLoader(AudioResourceLoadOwner* owner)
        : AudioResourceLoader(owner)
        , m_Chunk23200(0)
        , m_Chunk23200Entries(0)
    {
    }

    virtual void ParseChunk(nlChunk* chunk);
    virtual ~AudioBankLoader() { }
    virtual void Load(const char* name) = 0;

protected:
    void* m_Chunk23200;
    void* m_Chunk23200Entries;
};

#endif // GAME_AUDIO_AUDIO_BANK_LOADER_H
