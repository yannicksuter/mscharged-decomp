#ifndef GAME_AUDIO_AUDIO_BANK_TABLE_H
#define GAME_AUDIO_AUDIO_BANK_TABLE_H

#include "Game/Audio/AudioResourceLoader.h"

class nlChunk;
struct AudioBankGroup;

struct AudioResourceName
{
    u32 field_00;
    const char* name;
};

struct AudioResourceSource
{
    u32 field_00;
    u32 field_04;
    AudioResourceName* resource;
    AudioBankGroup* field_0C;
    AudioResourceLoadOwner* field_10;
    u8 field_14;
    u8 useCompactCallback;
    u8 pad_16[2];
};

struct AudioBankGroup
{
    u32 field_00;
    u32 field_04;
    u32 count_08;
    AudioResourceSource** records_0C;
    u8 field_10;
    u8 pad_11[3];
};

struct AudioBankTable
{
    static AudioBankTable* ParseChunk(nlChunk* chunk);
    void Initialize();
    void SelectGroup(unsigned int index);
    void ClearSelectedGroups();
    void LoadBank(int slotId, unsigned long cueId,
        AudioResourceLoadCallback callback, void* context,
        MemoryAllocator* allocator);
    void UnloadBank(unsigned int index);
    void UnloadAllBanks();

    u32 count_00;
    AudioBankGroup* records_04;
    u32 count_08;
    AudioResourceSource* records_0C;
    u32 count_10;
    AudioResourceName* records_14;
};

#endif
