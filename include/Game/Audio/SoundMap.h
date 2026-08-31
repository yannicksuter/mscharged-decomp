#ifndef GAME_AUDIO_SOUND_MAP_H
#define GAME_AUDIO_SOUND_MAP_H

#include "NL/nlAVLTree.h"
#include "types.h"

class nlChunk;

struct SoundCue
{
    u32 field_0x0;
    u32 field_0x4;
    u32 field_0x8;
    u32 field_0xC;
    u32 field_0x10;
};

class SoundCueCompare_802EE294
{
public:
    int operator()(SoundCue* cue1, SoundCue* cue2) const
    {
        if (cue1->field_0x0 < cue2->field_0x0)
            return -1;
        if (cue2->field_0x0 < cue1->field_0x0)
            return 1;
        if (cue1->field_0x4 < cue2->field_0x4)
            return -1;
        if (cue2->field_0x4 < cue1->field_0x4)
            return 1;
        if (cue1->field_0x8 < cue2->field_0x8)
            return -1;
        if (cue2->field_0x8 < cue1->field_0x8)
            return 1;
        if (cue1->field_0xC < cue2->field_0xC)
            return -1;
        return cue2->field_0xC < cue1->field_0xC;
    }
};

typedef nlAVLTree<SoundCue*, SoundCue*, SoundCueCompare_802EE294>
    SoundCueTree_802EE294;
typedef nlAVLTreeIterator<SoundCue*, SoundCue*, SoundCueCompare_802EE294>
    SoundCueTreeIterator_802EE294;

class SoundMap
{
public:
    u32 fn_802EE178(u32 field0, u32 field4, u32 field8, u32 fieldC);
    void fn_802EE20C();
    static SoundMap* fn_802EE458(nlChunk* chunk);

    u32 m_CueCount;
    SoundCue* m_Cues;
    SoundCueTree_802EE294* m_CueTree;
};

#endif // GAME_AUDIO_SOUND_MAP_H
