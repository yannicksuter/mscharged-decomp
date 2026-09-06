#ifndef GAME_AUDIO_AUDIO_BANK_TABLE_802EB644_H
#define GAME_AUDIO_AUDIO_BANK_TABLE_802EB644_H

#include "Game/Audio/AudioResourceLoader_802EDA38.h"

class nlChunk;
struct AudioBankGroup_802EB644;

struct AudioResourceName_802ED144
{
    u32 field_00;
    const char* name;
};

struct AudioResourceSource_802ED144
{
    u32 field_00;
    u32 field_04;
    AudioResourceName_802ED144* resource;
    AudioBankGroup_802EB644* field_0C;
    AudioResourceLoadOwner_802EDA54* field_10;
    u8 field_14;
    u8 useCompactCallback;
    u8 pad_16[2];
};

struct AudioBankGroup_802EB644
{
    u32 field_00;
    u32 field_04;
    u32 count_08;
    AudioResourceSource_802ED144** records_0C;
    u8 field_10;
    u8 pad_11[3];
};

struct AudioBankTable_802EB644
{
    static AudioBankTable_802EB644* fn_802EB644(nlChunk* chunk);
    void fn_802EBADC();
    void fn_802EBBA4(u32 index);
    void fn_802EBBBC();
    void fn_802EBBF0(int slotId, unsigned long cueId,
        AudioResourceLoadCallback_802EDA54 callback, void* context,
        MemoryAllocator* allocator);
    void fn_802EBC9C(u32 index);
    void fn_802EBCCC();

    u32 count_00;
    AudioBankGroup_802EB644* records_04;
    u32 count_08;
    AudioResourceSource_802ED144* records_0C;
    u32 count_10;
    AudioResourceName_802ED144* records_14;
};

#endif
