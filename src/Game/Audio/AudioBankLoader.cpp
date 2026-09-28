#include "Game/Audio/AudioSource.h"
#include "Game/Audio/AudioBankLoader.h"
#include "NL/nlChunk.h"
#include "types.h"

void AudioBankLoader::ParseChunk(nlChunk* chunk)
{
    switch (chunk->GetID())
    {
    case 0x80023200:
        break;
    default:
        return;
    }

    nlChunk* header = (nlChunk*)chunk->GetData();
    m_Chunk23200 = (AudioSourceData*)header->GetData();
    nlChunk* definitions = header->GetLastChunk();
    AudioSourceInfo* entries = (AudioSourceInfo*)definitions->GetData();
    m_Chunk23200Entries = entries;
    u32 i = 0;
    u32 entryOffset = 0;
    while (i < *(u32*)m_Chunk23200)
    {
        AudioSourceInfo* entry = (AudioSourceInfo*)(
            (u8*)m_Chunk23200Entries + entryOffset);
        entry->m_Unknown18 = this;
        i++;
        entryOffset += sizeof(AudioSourceInfo);
    }
}
