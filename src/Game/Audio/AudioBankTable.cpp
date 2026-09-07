#include "NL/nlDebugString.h"
#include "Game/Audio/AudioBankTable.h"
#include "Game/Sys/debug.h"

#include "NL/nlChunk.h"
#include "NL/nlMemory.h"

AudioBankTable* AudioBankTable::ParseChunk(nlChunk* chunk)
{
    nlChunk* current = chunk->GetFirstChunk();
    AudioBankTable* table = (AudioBankTable*)current->GetData();
    AudioResourceSource* oldRecords = table->records_0C;

    current = current->GetNextChunk();
    table->records_04 = (AudioBankGroup*)current->GetData();
    for (u32 i = 0; i < table->count_00; ++i)
    {
        current = current->GetNextChunk();
        table->records_04[i].records_0C = (AudioResourceSource**)current->GetData();
    }

    current = current->GetNextChunk();
    table->records_0C = (AudioResourceSource*)current->GetData();
    s32 delta = (u8*)oldRecords - (u8*)table->records_0C;

    current = current->GetNextChunk();
    table->records_14 = (AudioResourceName*)current->GetData();
    for (u32 i = 0; i < table->count_10; ++i)
    {
        current = current->GetNextChunk();
        table->records_14[i].name = (const char*)current->GetData();
    }

    for (u32 i = 0; i < table->count_00; ++i)
    {
        AudioBankGroup* group = &table->records_04[i];
        for (u32 j = 0; j < group->count_08; ++j)
        {
            group->records_0C[j] = (AudioResourceSource*)((u8*)group->records_0C[j] - delta);
            group->records_0C[j]->field_0C = group;
        }
    }
    return table;
}

void AudioBankTable::Initialize()
{
    for (u32 i = 0; i < count_00; ++i)
    {
        AudioBankGroup* group = &records_04[i];
        for (u32 j = 0; j < group->count_08; ++j)
        {
            AudioResourceSource* source = group->records_0C[j];
            source->field_10 = new (8, false) AudioResourceLoadOwner;
            source->field_10->m_Source = source;
        }
    }
}

void AudioBankTable::SelectGroup(u32 index)
{
    records_04[index].field_10 = true;
}

void AudioBankTable::ClearSelectedGroups()
{
    for (u32 i = 0; i < count_00; ++i)
        records_04[i].field_10 = false;
}

void AudioBankTable::LoadBank(int slotId, unsigned long cueId,
    AudioResourceLoadCallback callback, void* context,
    MemoryAllocator* allocator)
{
    AudioResourceSource* source = &records_0C[cueId];
    AudioResourceName* resource = &records_14[slotId];
    tDebugPrintManager::Print(DC_SOUND, "Loading Bank %s into slot %s::%s\n", resource->name,
        nlLookupDebugString(g_pDebugStringTable, source->field_0C->field_04),
        nlLookupDebugString(g_pDebugStringTable, source->field_04));
    source->resource = resource;
    source->field_14 = true;
    LoadAudioResource(source->field_10, resource->name, callback, context, allocator);
}

void AudioBankTable::UnloadBank(u32 index)
{
    AudioResourceSource* source = &records_0C[index];
    source->field_14 = false;
    source->resource = 0;
    if (source->field_10->m_Completed)
        UnloadAudioResource(source->field_10);
}

void AudioBankTable::UnloadAllBanks()
{
    for (u32 i = 0; i < count_08; ++i)
    {
        AudioResourceSource* source = &records_0C[i];
        source->field_14 = false;
        source->resource = 0;
        if (source->field_10->m_Completed)
            UnloadAudioResource(source->field_10);
    }
}
