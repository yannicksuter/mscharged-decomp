#ifndef GAME_AUDIO_AUDIO_RESOURCE_PLATFORM_8035DE28_H
#define GAME_AUDIO_AUDIO_RESOURCE_PLATFORM_8035DE28_H

#include "Game/Audio/AudioResourceLoader_802EDA38.h"

class nlChunk;
class nlFile;

class AudioBankLoader_802EF6B0 : public AudioResourceLoader_802EDA38
{
public:
    AudioBankLoader_802EF6B0(AudioResourceLoadOwner_802EDA54* owner)
        : AudioResourceLoader_802EDA38(owner)
        , m_Chunk23200(0)
        , m_Chunk23200Entries(0)
    {
    }

    virtual void fn_802EF6B0(nlChunk* chunk);
    virtual ~AudioBankLoader_802EF6B0() { }
    virtual void fn_8035E3CC(const char* name) = 0;

protected:
    void* m_Chunk23200;
    void* m_Chunk23200Entries;
};

class AudioFileLoader_8035E7B4 : public AudioBankLoader_802EF6B0
{
public:
    AudioFileLoader_8035E7B4(AudioResourceLoadOwner_802EDA54* owner)
        : AudioBankLoader_802EF6B0(owner)
        , m_File(0)
    {
    }

    virtual void fn_802EDA38_0();
    virtual void fn_8035E3CC(const char* name);

private:
    nlFile* m_File;
};

class AudioMemoryLoader_8035E3CC : public AudioBankLoader_802EF6B0
{
public:
    AudioMemoryLoader_8035E3CC(AudioResourceLoadOwner_802EDA54* owner)
        : AudioBankLoader_802EF6B0(owner)
        , m_SoundTable(0)
        , m_Data(0)
    {
    }

    virtual void fn_802EDA38_0();
    virtual void fn_802EF6B0(nlChunk* chunk);
    virtual void fn_8035E3CC(const char* name);

    void* m_SoundTable;
    void* m_Data;
};

#endif // GAME_AUDIO_AUDIO_RESOURCE_PLATFORM_8035DE28_H
