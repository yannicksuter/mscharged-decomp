#ifndef GAME_AUDIO_AUDIO_BACKEND_8035B8E8_H
#define GAME_AUDIO_AUDIO_BACKEND_8035B8E8_H

#include "revolution/axfx.h"
#include "revolution/os/OSAlarm.h"
#include "revolution/wenc.h"

#include "NL/MemAlloc.h"
#include "NL/nlArrayAllocator.h"
#include "NL/nlFile.h"
#include "NL/nlList.h"

class AudioSource_8035C234;
class AudioReadState_80361258;
struct AudioSourceInfo_8035C298;

struct AudioRead_8035C620
{
    nlFile* m_Unknown00;
    unsigned int m_Unknown04;
    void* m_Unknown08;
    ReadAsyncCallback m_Unknown0C;
    unsigned long m_Unknown10;
    AudioReadState_80361258* m_Unknown14;
    unsigned int m_Unknown18 : 31;
    unsigned int m_Unknown1B : 1;
};

class AudioBackendBase_8035CABC
{
public:
    AudioBackendBase_8035CABC();
    virtual ~AudioBackendBase_8035CABC() { }
    virtual bool fn_8035B8E8_1() = 0;
    virtual void fn_8035BFDC() = 0;
};

class AudioBackend_8035B8E8 : public AudioBackendBase_8035CABC
{
public:
    AudioBackend_8035B8E8();
    virtual ~AudioBackend_8035B8E8();
    virtual bool fn_8035B8E8_1();
    virtual void fn_8035BFDC();

    void fn_8035BE04();
    void fn_8035BE74();
    void* fn_8035C0B4(unsigned long size);
    void fn_8035C114(void* pointer);
    void fn_8035C11C();
    AudioSource_8035C234* fn_8035C298(AudioSourceInfo_8035C298*);
    void fn_8035C51C(AudioSource_8035C234* source);
    void fn_8035C620(nlFile* file, unsigned int offset, void* buffer,
        unsigned int size, ReadAsyncCallback callback, unsigned long userParam,
        AudioReadState_80361258* state);
    void fn_8035C734(AudioReadState_80361258* state);
    void fn_8035C818(unsigned int mode);
    void fn_8035C894();

    /* 0x004 */ nlListSlotPool<AudioSource_8035C234*> m_Unknown004;
    /* 0x024 */ ListContainerBase<AudioRead_8035C620,
        nlStaticArrayAllocator<ListEntry<AudioRead_8035C620>, 32> >
        m_Unknown024;
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

extern AudioBackend_8035B8E8* lbl_806E2020;

inline AudioBackendBase_8035CABC::AudioBackendBase_8035CABC()
{
    lbl_806E2020 = static_cast<AudioBackend_8035B8E8*>(this);
}

#endif // GAME_AUDIO_AUDIO_BACKEND_8035B8E8_H
