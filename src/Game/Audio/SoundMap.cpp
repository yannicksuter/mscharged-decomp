#include "NL/nlDebugString.h"
#include "Game/Audio/RegistryPools.h"
#include "Game/Audio/SoundMap.h"
#include "Game/Sys/debug.h"

#include "NL/nlChunk.h"
#include "NL/nlMemory.h"

u32 SoundMap::FindCue(
    u32 field0, u32 field4, u32 field8, u32 fieldC)
{
    SoundCue cue;
    cue.field_0x0 = field0;
    cue.field_0x4 = field4;
    cue.field_0x8 = field8;
    cue.field_0xC = fieldC;
    cue.field_0x10 = 0xFFFF;

    SoundCue** found;
    SoundCue* key = &cue;
    if (m_CueTree->FindGet(key, &found))
        return (*found)->field_0x10;
    return 0xFFFF;
}

void SoundMap::Unload()
{
    delete m_CueTree;
    m_CueTree = 0;
}

SoundMap* SoundMap::ParseChunk(nlChunk* chunk)
{
    nlChunk* mapChunk = chunk->GetFirstChunk();
    SoundMap* map = (SoundMap*)mapChunk->GetData();
    nlChunk* cuesChunk = mapChunk->GetNextChunk();
    map->m_Cues = (SoundCue*)cuesChunk->GetData();

    map->m_CueTree = new (8, false) SoundCueTree;
    for (u32 i = 0; i < map->m_CueCount; ++i)
    {
        SoundCue* cue = &map->m_Cues[i];
        map->m_CueTree->Add(cue, cue);
    }

    tDebugPrintManager::Print(DC_SOUND, "SoundMap: %d cues\n", map->m_CueCount);

    SoundCueTreeIterator* iterator =
        map->m_CueTree->GetIterator();
    while (iterator->IsValid())
    {
        tDebugPrintManager::Print(DC_SOUND, " SoundCue %d = %s (%u),%s,%s,%s\n",
            iterator->CurrentValue()->field_0x10,
            iterator->CurrentValue()->field_0x0 != 0
                ? nlLookupDebugString(g_pDebugStringTable, iterator->CurrentValue()->field_0x0) : "--",
            iterator->CurrentValue()->field_0x0,
            iterator->CurrentValue()->field_0x4 != 0
                ? nlLookupDebugString(g_pDebugStringTable, iterator->CurrentValue()->field_0x4) : "--",
            iterator->CurrentValue()->field_0x8 != 0
                ? nlLookupDebugString(g_pDebugStringTable, iterator->CurrentValue()->field_0x8) : "--",
            iterator->CurrentValue()->field_0xC != 0
                ? nlLookupDebugString(g_pDebugStringTable, iterator->CurrentValue()->field_0xC) : "--");
        iterator->Next();
    }
    if (iterator != 0)
        delete iterator;
    return map;
}
