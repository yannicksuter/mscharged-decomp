#include "Game/Audio/AudioBankLoader.h"
#include "NL/nlChunk.h"
#include "types.h"

struct AudioBankEntry_802EF6B0
{
    u8 pad_00[0x18];
    void* owner;
};

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
    m_Chunk23200 = header->GetData();
    nlChunk* definitions = header->GetLastChunk();
    void* entries = definitions->GetData();
    m_Chunk23200Entries = entries;
    u32 i = 0;
    u32 entryOffset = 0;
    while (i < *(u32*)m_Chunk23200)
    {
        AudioBankEntry_802EF6B0* entry = (AudioBankEntry_802EF6B0*)(
            (u8*)m_Chunk23200Entries + entryOffset);
        entry->owner = this;
        i++;
        entryOffset += sizeof(AudioBankEntry_802EF6B0);
    }
}
