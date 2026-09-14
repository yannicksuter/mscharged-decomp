#include "Game/Audio/AudioBackend.h"
#include "Game/Audio/AudioBundleManager.h"
#include "Game/Audio/AudioBankTable.h"

#include "Game/Audio/AudioSystem.h"

#include "NL/nlChunk.h"
#include "NL/nlFile.h"
#include "NL/nlMemory.h"
#include "NL/nlPrint.h"

AudioBundleManager::AudioBundleManager()
    : m_Backend(0)
    , m_Initialized(false)
    , m_Chunk13100(0)
    , m_Chunk13400(0)
    , m_Chunk13500(0)
    , m_Runtime()
    , m_Loaded(false)
{
    m_Backend = new (8, false) AudioBackend;
}

bool AudioBundleManager::Initialize()
{
    return m_Backend->Initialize();
}

void AudioBundleManager::OnBundleLoaded(
    void* data, unsigned long, void* manager)
{
    nlChunk* chunk = (nlChunk*)data;
    AudioBundleManager* self = (AudioBundleManager*)manager;
    self->m_LoadedData = data;

    nlChunk* end = chunk->GetLastChunk();
    chunk = chunk->GetFirstChunk();
    while (chunk != end)
    {
        self->ParseChunk(chunk);
        chunk = chunk->GetNextChunk();
    }
    self->OnLoadComplete();
}

void AudioBundleManager::ParseChunk(nlChunk* chunk)
{
    switch (chunk->GetID())
    {
    case 0x23800:
        break;
    case 0x13500:
        m_Chunk13500 = AudioBankTable::ParseChunk(chunk);
        m_Chunk13500->Initialize();
        break;
    case 0x13400:
        m_Chunk13400 = ParseAudioCalculationTable(chunk);
        break;
    case 0x13100:
        m_Chunk13100 = ParseAudioSliderTable(chunk);
        break;
    case 0x21200:
        if (m_Runtime.Load(
                chunk->GetData(),
                chunk->GetSize()
                    - ((u8*)chunk->GetData()
                        - (u8*)chunk->GetUnalignedData()),
                false))
        {
            m_Runtime.m_ConfigRoot = (AudioConfigNode*)m_Runtime.GetRoot();
        }
        break;
    case 0x23704:
        m_Runtime.LoadScriptData(
            chunk->GetData(),
            chunk->GetSize()
                - ((u8*)chunk->GetData()
                    - (u8*)chunk->GetUnalignedData()));
        break;
    }
}

void AudioBundleManager::Load(const char* path)
{
    char fileName[128];
    nlSNPrintf(fileName, sizeof(fileName), "%s%s", path, "nlxgs.bun");

    if (g_pAudioSystem->IsAsyncLoading())
    {
        nlLoadEntireFileAsync(fileName, OnBundleLoaded, this, 0x20, AllocateStart, 0, 0, 0);
        return;
    }

    unsigned long size;
    void* data = nlLoadEntireFile(
        fileName, &size, 0x20, AllocateStart, 0, 0, 0);
    m_LoadedData = data;

    nlChunk* outerChunk = (nlChunk*)data;
    nlChunk* end = outerChunk->GetNextChunk();
    nlChunk* chunk = outerChunk->GetFirstChunk();
    while (chunk != end)
    {
        ParseChunk(chunk);
        chunk = chunk->GetNextChunk();
    }
    OnLoadComplete();
}
