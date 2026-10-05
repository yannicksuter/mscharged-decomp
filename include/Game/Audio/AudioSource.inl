#ifndef GAME_AUDIO_AUDIO_SOURCE_INL
#define GAME_AUDIO_AUDIO_SOURCE_INL

inline bool AudioSource::IsLooping()
{
    return m_PlayCount == 0xFFFF;
}

inline void AudioReadState::SetInputVolume(float value)
{
    bool enabled = OSDisableInterrupts();
    if (!HasVoice())
    {
        OSRestoreInterrupts(enabled);
        return;
    }
    AudioStreamChannel* channel = GetChannelIterator();
    while ((channel = GetNextChannel(channel)) != 0)
    {
        if (channel->m_Voice != 0)
            SetVoiceInputVolume(channel->m_Voice, value);
    }
    OSRestoreInterrupts(enabled);
}

inline void AudioReadState::SetMixVolume(float value)
{
    bool enabled = OSDisableInterrupts();
    if (!HasVoice())
    {
        OSRestoreInterrupts(enabled);
        return;
    }
    AudioStreamChannel* channel = GetChannelIterator();
    while ((channel = GetNextChannel(channel)) != 0)
    {
        if (channel->m_Voice != 0)
            SetVoiceMixVolume(channel->m_Voice, value);
    }
    OSRestoreInterrupts(enabled);
}

inline void AudioReadState::SetPitch(float value)
{
    bool enabled = OSDisableInterrupts();
    if (!HasVoice())
    {
        OSRestoreInterrupts(enabled);
        return;
    }
    AudioStreamChannel* channel = GetChannelIterator();
    while ((channel = GetNextChannel(channel)) != 0)
    {
        if (channel->m_Voice != 0)
            SetVoicePitch(channel->m_Voice, m_SampleRateRatio, value);
        OSRestoreInterrupts(enabled);
    }
}

inline void AudioReadState::SetSurroundPan(float value)
{
    bool enabled = OSDisableInterrupts();
    if (!HasVoice())
    {
        OSRestoreInterrupts(enabled);
        return;
    }
    AudioStreamChannel* channel = GetChannelIterator();
    while ((channel = GetNextChannel(channel)) != 0)
    {
        if (channel->m_Voice != 0)
            SetVoiceSurroundPan(channel->m_Voice, value);
    }
    OSRestoreInterrupts(enabled);
}

inline void AudioReadState::SetLowPassFilter(bool on, unsigned int frequency, bool unchanged)
{
    bool enabled = OSDisableInterrupts();
    if (!HasVoice())
    {
        OSRestoreInterrupts(enabled);
        return;
    }
    AudioStreamChannel* channel = GetChannelIterator();
    while ((channel = GetNextChannel(channel)) != 0)
    {
        if (channel->m_Voice != 0)
            SetVoiceLowPassFilter(channel->m_Voice, on, frequency, unchanged);
    }
    OSRestoreInterrupts(enabled);
}

inline void AudioReadState::SetAuxiliaryVolume(int auxiliary, int value)
{
    bool enabled = OSDisableInterrupts();
    if (!HasVoice())
    {
        OSRestoreInterrupts(enabled);
        return;
    }
    AudioStreamChannel* channel = GetChannelIterator();
    while ((channel = GetNextChannel(channel)) != 0)
    {
        if (channel->m_Voice != 0)
            SetVoiceAuxiliaryVolume(channel->m_Voice, auxiliary, value);
    }
    OSRestoreInterrupts(enabled);
}

template <unsigned int ChannelCount>
inline AXVPB* AudioStreamChannels<ChannelCount>::GetVoice()
{
    return m_Channels[0].m_Voice;
}

template <unsigned int ChannelCount>
inline bool AudioStreamChannels<ChannelCount>::WasVoiceDropped()
{
    AudioStreamChannel* channel = GetChannelIterator();
    while ((channel = GetNextChannel(channel)) != 0)
    {
        if (channel->m_VoiceDropped)
            return true;
    }
    return false;
}

template <unsigned int ChannelCount>
inline void AudioStreamChannels<ChannelCount>::ReleaseVoice(bool release)
{
    AudioStreamChannel* channel = GetChannelIterator();
    while ((channel = GetNextChannel(channel)) != 0)
        channel->ReleaseVoice(release);
}

template <unsigned int ChannelCount>
inline unsigned int AudioStreamChannels<ChannelCount>::GetChannelCount()
{
    return ChannelCount;
}

template <unsigned int ChannelCount>
inline AudioStreamChannel* AudioStreamChannels<ChannelCount>::GetFirstChannel()
{
    return m_Channels;
}

template <unsigned int ChannelCount>
inline AudioStreamChannel* AudioStreamChannels<ChannelCount>::GetChannelIterator()
{
    return m_Channels - 1;
}

template <unsigned int ChannelCount>
inline AudioStreamChannel* AudioStreamChannels<ChannelCount>::GetNextChannel(AudioStreamChannel* channel)
{
    ++channel;
    if (channel >= m_Channels + ChannelCount)
        channel = 0;
    return channel;
}

template <unsigned int ChannelCount>
inline bool AudioStreamChannels<ChannelCount>::HasVoice()
{
    bool hasVoice = false;
    AudioStreamChannel* channel = GetChannelIterator();
    while ((channel = GetNextChannel(channel)) != 0)
        hasVoice |= channel->m_Voice != 0;
    return hasVoice;
}

inline AudioMonoStreamSource::~AudioMonoStreamSource()
{
}

inline void AudioMonoStreamSource::SetPan(float value)
{
    SetVoicePan(m_Channels[0].m_Voice, value);
}

inline void AudioMonoStreamSource::SetInterauralDelay(int value)
{
    SetVoiceInterauralDelay(m_Channels[0].m_Voice, value);
}

inline unsigned int AudioMonoStreamSource::GetStreamHeaderOffset()
{
    return m_SourceInfo->m_StreamOffset;
}

inline AudioSource::~AudioSource()
{
}

#endif
