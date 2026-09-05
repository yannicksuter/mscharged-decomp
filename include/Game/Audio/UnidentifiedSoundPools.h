#ifndef GAME_AUDIO_UNIDENTIFIED_SOUND_POOLS_H
#define GAME_AUDIO_UNIDENTIFIED_SOUND_POOLS_H

#include "NL/nlSlotPool.h"
#include "types.h"

// Every translation unit that requests sounds through Game/Sys/audio.h emits
// the same guarded initializers for two shared template static slot pools
// (entry sizes 0x14 and 0x40, both constructed with (16, 16)) ahead of the
// UnidentifiedStaticStorage state, and no retained code allocates from them.
// The stripped DOL does not reveal the original template, its specialization
// or its entry types; the names here are explicitly unidentified placeholders.

struct UnidentifiedSoundPoolEntry14
{
    u8 mUnidentified[0x14];
};

struct UnidentifiedSoundPoolEntry40
{
    u8 mUnidentified[0x40];
};

template <typename T>
struct UnidentifiedSoundPools
{
    static SlotPool<UnidentifiedSoundPoolEntry14> sUnidentifiedPool14;
    static SlotPool<UnidentifiedSoundPoolEntry40> sUnidentifiedPool40;
};

struct UnidentifiedSoundPoolTag;

template <typename T>
SlotPool<UnidentifiedSoundPoolEntry14>
    UnidentifiedSoundPools<T>::sUnidentifiedPool14(16, 16);

template <typename T>
SlotPool<UnidentifiedSoundPoolEntry40>
    UnidentifiedSoundPools<T>::sUnidentifiedPool40(16, 16);

#endif // GAME_AUDIO_UNIDENTIFIED_SOUND_POOLS_H
