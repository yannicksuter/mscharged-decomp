#ifndef GAME_AUDIO_SOUNDINSTANCE_802F1758_H
#define GAME_AUDIO_SOUNDINSTANCE_802F1758_H

#include "Game/Audio/RpcList_802EFB70.h"
#include "types.h"

struct SoundInstance_802F1758;

extern "C" void fn_802F2A74(SoundInstance_802F1758*);
extern SlotPool<SoundInstance_802F1758> lbl_8057FAA8;

struct SoundInstance_802F1758
{
    ~SoundInstance_802F1758()
    {
        fn_802F2A74(this);
    }

    static void operator delete(void* instance)
    {
        lbl_8057FAA8.Free((SoundInstance_802F1758*)instance);
    }

    void* owner;
    void* definition;
    void* voices;
    RpcList_802EFB70 rpcEntries;
    s32 state;
    float previousTime;
    float currentTime;
    void* activeRpc;
    float transitionTime;
    u8 pad_28[0x48];
    float field_70;
    float field_74;
    SoundInstance_802F1758* nextInstance;
};

#endif
