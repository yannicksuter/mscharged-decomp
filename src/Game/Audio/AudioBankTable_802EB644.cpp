#include "Game/Audio/AudioBankTable_802EB644.h"
#include "Game/Sys/debug.h"

#include "Game/SAnim.h"
#include "NL/nlMemory.h"

extern "C" const char* fn_802B9568(void* stringTable, u32 value);

extern void* lbl_806E1DC8;

AudioBankTable_802EB644* AudioBankTable_802EB644::fn_802EB644(nlChunk* chunk)
{
    nlChunk* current = chunk->GetFirstChunk();
    AudioBankTable_802EB644* table = (AudioBankTable_802EB644*)current->GetData();
    AudioResourceSource_802ED144* oldRecords = table->records_0C;

    current = current->GetNextChunk();
    table->records_04 = (AudioBankGroup_802EB644*)current->GetData();
    for (u32 i = 0; i < table->count_00; ++i)
    {
        current = current->GetNextChunk();
        table->records_04[i].records_0C = (AudioResourceSource_802ED144**)current->GetData();
    }

    current = current->GetNextChunk();
    table->records_0C = (AudioResourceSource_802ED144*)current->GetData();
    s32 delta = (u8*)oldRecords - (u8*)table->records_0C;

    current = current->GetNextChunk();
    table->records_14 = (AudioResourceName_802ED144*)current->GetData();
    for (u32 i = 0; i < table->count_10; ++i)
    {
        current = current->GetNextChunk();
        table->records_14[i].name = (const char*)current->GetData();
    }

    for (u32 i = 0; i < table->count_00; ++i)
    {
        AudioBankGroup_802EB644* group = &table->records_04[i];
        for (u32 j = 0; j < group->count_08; ++j)
        {
            group->records_0C[j] = (AudioResourceSource_802ED144*)((u8*)group->records_0C[j] - delta);
            group->records_0C[j]->field_0C = group;
        }
    }
    return table;
}

void AudioBankTable_802EB644::fn_802EBADC()
{
    for (u32 i = 0; i < count_00; ++i)
    {
        AudioBankGroup_802EB644* group = &records_04[i];
        for (u32 j = 0; j < group->count_08; ++j)
        {
            AudioResourceSource_802ED144* source = group->records_0C[j];
            source->field_10 = new (8, false) AudioResourceLoadOwner_802EDA54;
            source->field_10->m_Source = source;
        }
    }
}

void AudioBankTable_802EB644::fn_802EBBA4(u32 index)
{
    records_04[index].field_10 = true;
}

void AudioBankTable_802EB644::fn_802EBBBC()
{
    for (u32 i = 0; i < count_00; ++i)
        records_04[i].field_10 = false;
}

void AudioBankTable_802EB644::fn_802EBBF0(int slotId, unsigned long cueId,
    AudioResourceLoadCallback_802EDA54 callback, void* context,
    MemoryAllocator* allocator)
{
    AudioResourceSource_802ED144* source = &records_0C[cueId];
    AudioResourceName_802ED144* resource = &records_14[slotId];
    tDebugPrintManager::Print(DC_SOUND, "Loading Bank %s into slot %s::%s\n", resource->name,
        fn_802B9568(lbl_806E1DC8, source->field_0C->field_04),
        fn_802B9568(lbl_806E1DC8, source->field_04));
    source->resource = resource;
    source->field_14 = true;
    fn_802ED498(source->field_10, resource->name, callback, context, allocator);
}

void AudioBankTable_802EB644::fn_802EBC9C(u32 index)
{
    AudioResourceSource_802ED144* source = &records_0C[index];
    source->field_14 = false;
    source->resource = 0;
    if (source->field_10->m_Completed)
        fn_802ED37C(source->field_10);
}

void AudioBankTable_802EB644::fn_802EBCCC()
{
    for (u32 i = 0; i < count_08; ++i)
    {
        AudioResourceSource_802ED144* source = &records_0C[i];
        source->field_14 = false;
        source->resource = 0;
        if (source->field_10->m_Completed)
            fn_802ED37C(source->field_10);
    }
}
