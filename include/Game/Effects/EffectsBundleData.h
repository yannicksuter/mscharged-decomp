#ifndef GAME_EFFECTS_EFFECTS_BUNDLE_DATA_H
#define GAME_EFFECTS_EFFECTS_BUNDLE_DATA_H

class EffectsGroup;
class EffectsTemplate;
class nlChunk;

struct EffectsBundleData
{
    typedef char* MemType;

    static bool IsValidChunkID(unsigned long id)
    {
        return (id & 0x80FFFFFF) == 0x80024000;
    }

    static EffectsBundleData* Initialize(nlChunk* bundle);
    void Destroy();

    /* 0x00 */ unsigned char unknown_0x00[8];
    /* 0x08 */ unsigned int mNumTemplates;
    /* 0x0C */ EffectsTemplate** mTemplates;
    /* 0x10 */ int mNumGroups;
    /* 0x14 */ EffectsGroup** mGroups;
}; // size: 0x18

#endif // GAME_EFFECTS_EFFECTS_BUNDLE_DATA_H
