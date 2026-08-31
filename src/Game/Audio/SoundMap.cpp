#include "Game/Audio/SoundMap.h"

#include "Game/SAnim.h"
#include "NL/nlMemory.h"

extern "C" void fn_8004F594(int category, const char* format, ...);
extern "C" const char* fn_802B9568(void* stringTable, u32 value);

extern void* lbl_806E1DC8;

static inline const char* GetSoundCueString(u32 value)
{
    if (value != 0)
        return fn_802B9568(lbl_806E1DC8, value);
    return "--";
}

u32 SoundMap::fn_802EE178(
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

void SoundMap::fn_802EE20C()
{
    delete m_CueTree;
    m_CueTree = 0;
}

SoundMap* SoundMap::fn_802EE458(nlChunk* chunk)
{
    const char* fieldCString;
    const char* field8String;
    const char* field4String;
    SoundCueTreeIterator_802EE294::Entry* current;

    nlChunk* mapChunk = chunk->GetFirstChunk();
    SoundMap* map = (SoundMap*)mapChunk->GetData();
    nlChunk* cuesChunk = mapChunk->GetNextChunk();
    map->m_Cues = (SoundCue*)cuesChunk->GetData();

    map->m_CueTree = new (8, false) SoundCueTree_802EE294;
    for (u32 i = 0; i < map->m_CueCount; ++i)
    {
        SoundCue* cue = &map->m_Cues[i];
        map->m_CueTree->Add(cue, cue);
    }

    fn_8004F594(10, "SoundMap: %d cues\n", map->m_CueCount);

    SoundCueTreeIterator_802EE294* iterator =
        map->m_CueTree->GetIterator();
    while (iterator->IsValid())
    {
        fieldCString = GetSoundCueString(
            iterator->Current()->value->field_0xC);
        field8String = GetSoundCueString(
            iterator->Current()->value->field_0x8);
        field4String = GetSoundCueString(
            iterator->Current()->value->field_0x4);
        current = iterator->Current();
        fn_8004F594(10, " SoundCue %d = %s (%u),%s,%s,%s\n",
            iterator->Current()->value->field_0x10,
            GetSoundCueString(current->value->field_0x0),
            current->value->field_0x0, field4String, field8String,
            fieldCString);
        iterator->Next();
    }
    if (iterator != 0)
        delete iterator;
    return map;
}
