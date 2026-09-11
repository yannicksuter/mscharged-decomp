#ifndef GAME_AUDIO_AUDIO_SOURCE_H
#define GAME_AUDIO_AUDIO_SOURCE_H

#include "revolution/sp.h"
#include "NL/nlFile.h"
#include "NL/nlSlotPool.h"

class Plat3dSoundSrc;
class AudioReadState;
struct AudioStreamChannel;

struct AudioSourceData
{
    unsigned int m_Unknown00;
    unsigned int m_Unknown04;
    unsigned char m_Unknown08;
};

struct AudioSourceResource
{
    unsigned char m_Unknown00[0x10];
    AudioSourceData* m_Unknown10;
    void* m_Unknown14;
    union
    {
        SPSoundTable* m_SoundTable;
        nlFile* m_File;
    } m_Unknown18;
};

struct AudioSourceInfo
{
    unsigned int m_Unknown00;
    unsigned int m_Unknown04;
    unsigned int m_Unknown08;
    unsigned int m_Unknown0C;
    unsigned int m_Unknown10;
    unsigned int m_Unknown14;
    AudioSourceResource* m_Unknown18;
};

class AudioSource
{
public:
    AudioSource()
    {
        m_Unknown04 = 0;
        m_Unknown08 = 0;
        m_Unknown10 = 0;
        m_Unknown14_00 = 0;
        m_Unknown14_0C = 1;
        m_Unknown14_18 = 0;
    }
    virtual ~AudioSource() { }
    virtual void UpdateState();
    virtual unsigned int GetState();
    virtual bool IsResident();
    virtual bool IsStream();
    virtual bool Prepare() = 0;
    virtual bool Play(unsigned int) = 0;
    virtual void Stop() = 0;
    virtual bool Pause() = 0;
    virtual bool Resume() = 0;
    virtual void SetInputVolume(float) = 0;
    virtual void SetMixVolume(float) = 0;
    virtual void SetPitch(float) = 0;
    virtual void SetSpatialParameters(Plat3dSoundSrc*);
    virtual void Initialize(AudioSourceInfo*) = 0;
    virtual void Update() = 0;
    virtual bool IsLooping();
    virtual void SetPan(float) = 0;
    virtual void SetSurroundPan(float) = 0;
    virtual void SetInterauralDelay(int) = 0;
    virtual void SetLowPassFilter(bool, unsigned int, bool) = 0;
    virtual void SetAuxiliaryVolume(int, int) = 0;
    virtual bool WasVoiceDropped() = 0;
    virtual bool HasVoice() = 0;
    virtual AXVPB* GetVoice() = 0;
    virtual void ReleaseVoice(bool) = 0;

    void SetControllerSpeaker(bool, unsigned int);

    /* 0x04 */ int m_Unknown04;
    /* 0x08 */ AudioSourceInfo* m_Unknown08;
    /* 0x0C */ float m_Unknown0C;
    /* 0x10 */ int m_Unknown10;
    /* 0x14 */ unsigned int m_Unknown14_00 : 12;
    unsigned int m_Unknown14_0C : 12;
    unsigned int m_Unknown14_18 : 1;
    unsigned int m_Unknown14_19 : 2;
    unsigned int m_Unknown14_1B : 5;
};

class AudioSampleSource : public AudioSource
{
public:
    AudioSampleSource();
    virtual ~AudioSampleSource();
    virtual bool Prepare();
    virtual bool Play(unsigned int);
    virtual void Stop();
    virtual bool Pause();
    virtual bool Resume();
    virtual void SetInputVolume(float);
    virtual void SetMixVolume(float);
    virtual void SetPitch(float);
    virtual void Initialize(AudioSourceInfo*);
    virtual void Update();
    virtual void SetPan(float);
    virtual void SetSurroundPan(float);
    virtual void SetInterauralDelay(int);
    virtual void SetLowPassFilter(bool, unsigned int, bool);
    virtual void SetAuxiliaryVolume(int, int);
    virtual bool WasVoiceDropped();
    virtual bool HasVoice();
    virtual AXVPB* GetVoice();
    virtual void ReleaseVoice(bool);

    static void* operator new(unsigned long);
    static void operator delete(void* pointer);
    static void OnVoiceDropped(void*);

    /* 0x18 */ unsigned int m_Unknown18;
    /* 0x1C */ AXVPB* m_Unknown1C;
    /* 0x20 */ SPSoundEntry* m_Unknown20;
    /* 0x24 */ unsigned int m_Unknown24;
    /* 0x28 */ bool m_Unknown28;
};

struct AudioReadQueueEntry
{
    AsyncEntry* m_Unknown00;
    AudioReadQueueEntry* m_next;
};

struct AudioStreamHeader
{
    unsigned int m_Unknown00;
    unsigned int m_Unknown04;
    unsigned int m_Unknown08;
    unsigned char m_Unknown0C[0x10];
    AXPBADPCM m_Unknown1C;
};

struct AudioStreamChannel
{
    AudioStreamChannel();
    ~AudioStreamChannel();
    void PrepareVoice(AudioStreamHeader*);
    void ReleaseVoice(bool);
    static void OnVoiceDropped(void*);

    /* 0x00 */ AudioReadState* m_Unknown00;
    /* 0x04 */ AXVPB* m_Unknown04;
    /* 0x08 */ void* m_Unknown08;
    /* 0x0C */ unsigned int m_Unknown0C;
    /* 0x10 */ unsigned int m_Unknown10_00 : 31;
    unsigned int m_Unknown10_1F : 1;
    /* 0x14 */ unsigned int m_Unknown14;
};

class AudioReadState : public AudioSource
{
public:
    AudioReadState();
    virtual ~AudioReadState();
    virtual bool Prepare();
    virtual bool Play(unsigned int);
    virtual void Stop();
    virtual bool Pause();
    virtual bool Resume();
    virtual void SetInputVolume(float);
    virtual void SetMixVolume(float);
    virtual void SetPitch(float);
    virtual void Initialize(AudioSourceInfo*);
    virtual void Update();
    virtual void SetSurroundPan(float);
    virtual void SetLowPassFilter(bool, unsigned int, bool);
    virtual void SetAuxiliaryVolume(int, int);
    virtual unsigned int UnidentifiedVirtual70() = 0;
    virtual unsigned int GetChannelCount() = 0;
    virtual AudioStreamChannel* GetFirstChannel() = 0;
    virtual AudioStreamChannel* GetChannelIterator() = 0;
    virtual AudioStreamChannel* GetNextChannel(AudioStreamChannel*) = 0;

    /* 0x18 */ unsigned int m_Unknown18;
    /* 0x1C */ unsigned int m_Unknown1C;
    /* 0x20 */ signed int m_Unknown20_00 : 7;
    unsigned int m_Unknown20_07 : 24;
    unsigned int m_Unknown20_1F : 1;
    /* 0x24 */ AudioReadQueueEntry* m_Unknown24;
    /* 0x28 */ int m_Unknown28;
};

void SetVoiceInputVolume(AXVPB*, float);
void SetVoiceMixVolume(AXVPB*, float);
void SetVoicePitch(AXVPB*, float, float);
void SetVoicePan(AXVPB*, float);
void SetVoiceSurroundPan(AXVPB*, float);
void SetVoiceInterauralDelay(AXVPB*, int);
void SetVoiceLowPassFilter(AXVPB*, bool, unsigned int, bool);
void SetVoiceAuxiliaryVolume(AXVPB*, int, int);
void OnAudioStreamReadComplete(nlFile*, void*, unsigned int, unsigned long);
void TrackAudioRead(AudioReadState*, AsyncEntry*);
void CancelAudioReads(AudioReadState*);
void OnAudioReadCancelled(nlFile*, void*, unsigned int, unsigned long, ReadAsyncCallback);

void GetSoundSources(void* handle, AudioSource** sources, unsigned int* count);

extern SlotPool<AudioSampleSource> gAudioSampleSourcePool;

inline void* AudioSampleSource::operator new(unsigned long)
{
    return gAudioSampleSourcePool.Allocate();
}

#endif // GAME_AUDIO_AUDIO_SOURCE_H
