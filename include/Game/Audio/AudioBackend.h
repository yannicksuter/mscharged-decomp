#ifndef GAME_AUDIO_AUDIO_BACKEND_H
#define GAME_AUDIO_AUDIO_BACKEND_H

#include "Game/Audio/AudioGlobals.h"

#include "revolution/axfx.h"
#include "revolution/os/OSAlarm.h"
#include "revolution/wenc.h"

#include "NL/MemAlloc.h"
#include "NL/nlArrayAllocator.h"
#include "NL/nlFile.h"
#include "NL/nlList.h"

class AudioSource;
struct XSoundOwner;
class AudioReadState;
struct AudioSourceInfo;

struct AudioRead
{
    nlFile* m_Unknown00;
    unsigned int m_Unknown04;
    void* m_Unknown08;
    ReadAsyncCallback m_Unknown0C;
    unsigned long m_Unknown10;
    AudioReadState* m_Unknown14;
    unsigned int m_Unknown18 : 31;
    bool m_Unknown1B : 1;
};

class UnidentifiedAudioReadList
    : public ListContainerBase<AudioRead,
          nlStaticArrayAllocator<ListEntry<AudioRead>, 32> >
{
};

class AudioBackendBase
{
public:
    AudioBackendBase();
    virtual ~AudioBackendBase() { }
    virtual bool Initialize() = 0;
    virtual void Shutdown() = 0;
};

class AudioBackend : public AudioBackendBase
{
public:
    AudioBackend();
    virtual ~AudioBackend();
    virtual bool Initialize();
    virtual void Shutdown();

    void SuspendControllerSpeakers();
    void ResumeControllerSpeakers();
    void* AllocateAudioMemory(unsigned long size);
    void FreeAudioMemory(void* pointer);
    void ServiceReadQueue(float dt);
    AudioSource* CreateSource(AudioSourceInfo*, XSoundOwner* owner);
    void ReleaseSource(AudioSource* source);
    void QueueRead(nlFile* file, unsigned int offset, void* buffer,
        unsigned int size, ReadAsyncCallback callback, unsigned long userParam,
        AudioReadState* state);
    void QueueReadCancellation(AudioReadState* state);
    void SetOutputMode(unsigned int mode);
    void InitializeAuxEffects();

    /* 0x004 */ nlListSlotPool<AudioSource*> m_Unknown004;
    /* 0x024 */ UnidentifiedAudioReadList m_Unknown024;
    /* 0x434 */ MemoryAllocator m_Unknown434;
    /* 0x44C */ u32 m_OutputMode;
    /* 0x450 */ bool m_Unknown450;
    /* 0x451 */ u8 m_Pad451[3];
    /* 0x454 */ union
    {
        AXFX_REVERBHI m_Reverb;
        AXFX_REVERBHI_DPL2 m_ReverbDpl2;
    } m_Unknown454;
    /* 0x5E4 */ union
    {
        AXFX_DELAY m_Delay;
        AXFX_DELAY_EXP_DPL2 m_DelayDpl2;
    } m_DelayEffect;
    /* 0x668 */ OSAlarm m_Unknown668;
    /* 0x698 */ u32 m_Unknown698[4];
    /* 0x6A8 */ WENCInfo m_Unknown6A8[4];
};

inline AudioBackendBase::AudioBackendBase()
{
    g_pAudioBackend = static_cast<AudioBackend*>(this);
}

void DumpAudioMemory();
void UpdateAudioSources();

#endif // GAME_AUDIO_AUDIO_BACKEND_H
