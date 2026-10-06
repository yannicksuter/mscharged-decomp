#ifndef GAME_AUDIO_AUDIO_SOURCE_H
#define GAME_AUDIO_AUDIO_SOURCE_H

#include "revolution/sp.h"
#include "revolution/os/OSInterrupt.h"
#include "NL/nlFile.h"
#include "NL/nlSlotPool.h"

class Plat3dSoundSrc;
class AudioReadState;
class AudioBankLoader;
struct AudioStreamChannel;

struct AudioSourceData
{
    unsigned int m_SourceCount;
    unsigned int m_StreamBlockSize;
    unsigned char m_IsStream;
};

struct AudioSourceInfo
{
    unsigned int m_SoundIndex;
    unsigned int m_StreamOffset;
    unsigned int m_Unknown08;
    unsigned int m_Unknown0C;
    unsigned int m_ChannelCount;
    unsigned int m_Unknown14;
    AudioBankLoader* m_BankLoader;
};

class AudioSource
{
public:
    AudioSource()
    {
        m_State = 0;
        m_SourceInfo = 0;
        m_InternalState = 0;
        m_PlayCount = 0;
        m_PlayIteration = 1;
        m_ControllerSpeakerEnabled = 0;
    }
    virtual ~AudioSource();
    virtual void UpdateState();
    virtual int GetState() { return m_State; }
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

    /* 0x04 */ int m_State;
    /* 0x08 */ AudioSourceInfo* m_SourceInfo;
    /* 0x0C */ float m_SampleRateRatio;
    /* 0x10 */ int m_InternalState;
    /* 0x14 */ unsigned int m_PlayCount : 12;
    unsigned int m_PlayIteration : 12;
    unsigned int m_ControllerSpeakerEnabled : 1;
    unsigned int m_ControllerSpeakerChannel : 2;
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
    virtual bool WasVoiceDropped();
    virtual AXVPB* GetVoice();
    virtual void ReleaseVoice(bool);
    virtual void SetInputVolume(float);
    virtual void SetMixVolume(float);
    virtual void SetPitch(float);
    virtual void SetPan(float);
    virtual void SetInterauralDelay(int);
    virtual void SetSurroundPan(float);
    virtual void SetLowPassFilter(bool, unsigned int, bool);
    virtual bool HasVoice();
    virtual void SetAuxiliaryVolume(int, int);
    virtual void Initialize(AudioSourceInfo*);
    virtual void Update();

    static void* operator new(unsigned long);
    static void operator delete(void* pointer);
    static void OnVoiceDropped(void*);

    /* 0x18 */ unsigned int m_LastVoiceAddress;
    /* 0x1C */ AXVPB* m_Voice;
    /* 0x20 */ SPSoundEntry* m_SoundEntry;
    /* 0x24 */ unsigned int m_PauseAddress;
    /* 0x28 */ bool m_VoiceDropped;
};

struct AudioReadQueueEntry
{
    AsyncEntry* m_Request;
    AudioReadQueueEntry* m_next;
};

extern SlotPool<AudioReadQueueEntry> gAudioReadQueueEntryPool;

struct AudioStreamHeader
{
    unsigned long num_samples;
    unsigned long num_adpcm_nibbles;
    unsigned long sample_rate;
    unsigned short loop_flag;
    unsigned short format;
    unsigned long sa;
    unsigned long ea;
    unsigned long ca;
    unsigned short coef[16];
    unsigned short gain;
    unsigned short ps;
    unsigned short yn1;
    unsigned short yn2;
    unsigned short lps;
    unsigned short lyn1;
    unsigned short lyn2;
    unsigned short pad[11];
};

struct AudioStreamChannel
{
    AudioStreamChannel();
    ~AudioStreamChannel();
    void PrepareVoice(AudioStreamHeader*);
    void ReleaseVoice(bool);
    unsigned int GetBufferSize();
    void AdvanceReadPosition(unsigned int size);
    void* GetBuffer() { return m_Buffer; }
    static void OnVoiceDropped(void*);

    /* 0x00 */ AudioReadState* m_ReadState;
    /* 0x04 */ AXVPB* m_Voice;
    /* 0x08 */ void* m_Buffer;
    /* 0x0C */ unsigned int m_PauseAddress;
    /* 0x10 */ unsigned int m_ReadPosition : 31;
    unsigned int m_VoiceDropped : 1;
    /* 0x14 */ unsigned int m_BufferAddress;
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
    virtual unsigned int GetStreamHeaderOffset() = 0;
    virtual unsigned int GetChannelCount() = 0;
    virtual AudioStreamChannel* GetFirstChannel() = 0;
    virtual AudioStreamChannel* GetChannelIterator() = 0;
    virtual AudioStreamChannel* GetNextChannel(AudioStreamChannel*) = 0;

    nlFile* GetStreamFile();
    unsigned int GetStreamBlockSize();
    unsigned int GetStreamDataStart();
    unsigned int GetChannelDataOffset(AudioStreamChannel* channel);
    void QueueStreamRead(unsigned int offset, void* buffer, unsigned int size,
        ReadAsyncCallback callback, unsigned long userParam);
    void QueuePartialChannelRead(AudioStreamChannel* channel, unsigned int size);
    void CompleteRead();
    void QueueFullChannelRead(AudioStreamChannel* channel);
    void OnChannelPrepared(AudioStreamChannel* channel);

    /* 0x18 */ unsigned int m_StreamDataSize;
    /* 0x1C */ unsigned int m_StreamReadPosition;
    /* 0x20 */ signed int m_PendingReadCount : 7;
    unsigned int m_StreamEndPosition : 24;
    bool m_EndAddressSet : 1;
    /* 0x24 */ AudioReadQueueEntry* m_ReadQueue;
    /* 0x28 */ int m_PendingState;
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
void OnAudioStreamHeaderRead(nlFile*, void*, unsigned int, unsigned long);
void OnAudioStreamPrimeRead(nlFile*, void*, unsigned int, unsigned long);
void TrackAudioRead(AudioReadState*, AsyncEntry*);
void CancelAudioReads(AudioReadState*);
void OnAudioReadCancelled(nlFile*, void*, unsigned int, unsigned long, ReadAsyncCallback);

extern SlotPool<AudioSampleSource> gAudioSampleSourcePool;

inline void* AudioSampleSource::operator new(unsigned long)
{
    return gAudioSampleSourcePool.Allocate();
}

inline bool AudioSource::IsResident()
{
    return true;
}

inline bool AudioSource::IsStream()
{
    return false;
}

template <unsigned int ChannelCount>
class AudioStreamChannels : public AudioReadState
{
public:
    virtual ~AudioStreamChannels() { }

    virtual AXVPB* GetVoice();

    virtual bool WasVoiceDropped();

    virtual void ReleaseVoice(bool release);

    virtual unsigned int GetChannelCount();

    virtual AudioStreamChannel* GetFirstChannel();

    virtual AudioStreamChannel* GetChannelIterator();

    virtual AudioStreamChannel* GetNextChannel(AudioStreamChannel* channel);

    virtual bool HasVoice();

    AudioStreamChannel m_Channels[ChannelCount];
};

class AudioMonoStreamSource : public AudioStreamChannels<1>
{
public:
    virtual ~AudioMonoStreamSource();
    virtual void SetPan(float value);
    virtual void SetInterauralDelay(int value);
    virtual unsigned int GetStreamHeaderOffset();

    static void* operator new(unsigned long);
    static void operator delete(void*);
};

class AudioStereoStreamSource : public AudioStreamChannels<2>
{
public:
    inline virtual ~AudioStereoStreamSource();
    virtual bool Prepare();
    virtual void SetPan(float);
    virtual void SetInterauralDelay(int);
    virtual unsigned int GetStreamHeaderOffset();

    static void* operator new(unsigned long);
    static void operator delete(void*);
};

extern SlotPool<AudioMonoStreamSource> gAudioMonoStreamSourcePool;
extern SlotPool<AudioStereoStreamSource> gAudioStereoStreamSourcePool;

inline void* AudioMonoStreamSource::operator new(unsigned long)
{
    return gAudioMonoStreamSourcePool.Allocate();
}

inline void AudioMonoStreamSource::operator delete(void* pointer)
{
    gAudioMonoStreamSourcePool.Free((AudioMonoStreamSource*)pointer);
}

inline void* AudioStereoStreamSource::operator new(unsigned long)
{
    return gAudioStereoStreamSourcePool.Allocate();
}

inline void AudioStereoStreamSource::operator delete(void* pointer)
{
    gAudioStereoStreamSourcePool.Free((AudioStereoStreamSource*)pointer);
}

#include "Game/Audio/AudioSource.inl"

#endif // GAME_AUDIO_AUDIO_SOURCE_H
