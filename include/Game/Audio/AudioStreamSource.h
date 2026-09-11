#ifndef GAME_AUDIO_AUDIO_STREAM_SOURCE_H
#define GAME_AUDIO_AUDIO_STREAM_SOURCE_H

#include "Game/Audio/AudioSource.h"

template <unsigned int ChannelCount>
class UnidentifiedAudioChannels : public AudioReadState
{
public:
    virtual ~UnidentifiedAudioChannels() { }

    virtual bool WasVoiceDropped()
    {
        AudioStreamChannel* channel = GetChannelIterator();
        while ((channel = GetNextChannel(channel)) != 0)
        {
            if (channel->m_Unknown10_1F)
                return true;
        }
        return false;
    }

    virtual bool HasVoice()
    {
        bool hasVoice = false;
        AudioStreamChannel* channel = GetChannelIterator();
        while ((channel = GetNextChannel(channel)) != 0)
            hasVoice |= channel->m_Unknown04 != 0;
        return hasVoice;
    }

    virtual AXVPB* GetVoice()
    {
        return m_Channels[0].m_Unknown04;
    }

    virtual void ReleaseVoice(bool release)
    {
        AudioStreamChannel* channel = GetChannelIterator();
        while ((channel = GetNextChannel(channel)) != 0)
            channel->ReleaseVoice(release);
    }

    virtual unsigned int GetChannelCount()
    {
        return ChannelCount;
    }

    virtual AudioStreamChannel* GetFirstChannel()
    {
        return m_Channels;
    }

    virtual AudioStreamChannel* GetChannelIterator()
    {
        return m_Channels - 1;
    }

    virtual AudioStreamChannel* GetNextChannel(AudioStreamChannel* channel)
    {
        ++channel;
        if (channel >= m_Channels + ChannelCount)
            channel = 0;
        return channel;
    }

    AudioStreamChannel m_Channels[ChannelCount];
};

class AudioReadState_8035D154 : public UnidentifiedAudioChannels<1>
{
public:
    virtual ~AudioReadState_8035D154() { }
    virtual void SetPan(float value)
    {
        SetVoicePan(m_Channels[0].m_Unknown04, value);
    }
    virtual void SetInterauralDelay(int value)
    {
        SetVoiceInterauralDelay(m_Channels[0].m_Unknown04, value);
    }
    virtual unsigned int UnidentifiedVirtual70()
    {
        return m_Unknown08->m_Unknown04;
    }

    static void* operator new(unsigned long);
    static void operator delete(void*);
};

class AudioReadState_80361920 : public UnidentifiedAudioChannels<2>
{
public:
    virtual ~AudioReadState_80361920();
    virtual bool Prepare();
    virtual void SetPan(float);
    virtual void SetInterauralDelay(int);
    virtual unsigned int UnidentifiedVirtual70();

    static void* operator new(unsigned long);
    static void operator delete(void*);
};

extern SlotPool<AudioReadState_8035D154> lbl_80585C48;
extern SlotPool<AudioReadState_80361920> lbl_80585C70;

inline void* AudioReadState_8035D154::operator new(unsigned long)
{
    return lbl_80585C48.Allocate();
}

inline void AudioReadState_8035D154::operator delete(void* pointer)
{
    lbl_80585C48.Free((AudioReadState_8035D154*)pointer);
}

inline void* AudioReadState_80361920::operator new(unsigned long)
{
    return lbl_80585C70.Allocate();
}

inline void AudioReadState_80361920::operator delete(void* pointer)
{
    lbl_80585C70.Free((AudioReadState_80361920*)pointer);
}

#endif // GAME_AUDIO_AUDIO_STREAM_SOURCE_H
