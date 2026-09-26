#ifndef GAME_AUDIO_SOUND_INSTANCE_POOL_H
#define GAME_AUDIO_SOUND_INSTANCE_POOL_H

template <typename T>
class SlotPool;
struct SoundInstance;

extern SlotPool<SoundInstance> sSoundInstancePool;

#endif // GAME_AUDIO_SOUND_INSTANCE_POOL_H
