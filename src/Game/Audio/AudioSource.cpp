#include "Game/Audio/AudioSource.h"

#include "Game/Audio/AudioBackend.h"
#include "Game/Audio/Plat3dSoundSrc.h"
#include "NL/nlFileGC.h"
#include "NL/nlMath.h"
#include "NL/nlRing.h"
#include "revolution/mix.h"
#include "revolution/os/OSInterrupt.h"

unsigned int gResidentVoiceDropCount;
unsigned int gStreamVoiceDropCount;
unsigned int gAudioStreamSourceCount;
unsigned int gAudioSampleSourceCount;
unsigned int gAudioSourceCount;
unsigned int gAudioStreamChannelCount;
AXPBLPF sVoiceLowPassFilter;

SlotPool<AudioSampleSource> gAudioSampleSourcePool(64, 16);
SlotPool<AudioReadQueueEntry> gAudioReadQueueEntryPool(32, 16);

void SetVoiceInputVolume(AXVPB* voice, float value)
{
    MIXSetInput(voice, (int)(10.0f * value));
}

void SetVoiceMixVolume(AXVPB* voice, float value)
{
    u16 volume = 0;
    if (value >= 1.0f)
        volume = 0x8000;
    else if (value > 0.0f)
        volume = (u16)(int)(32768.0f * value);
    AXPBMIX mix = voice->pb.mix;
    mix.vL = volume;
    mix.vR = volume;
    AXSetVoiceMix(voice, &mix);
}

void SetVoicePitch(AXVPB* voice, float ratio, float value)
{
    float pitch = nlFastExp2(value * nlFastLog2(1.0594631f));
    AXSetVoiceSrcRatio(voice, pitch * ratio);
}

void SetVoicePan(AXVPB* voice, float value)
{
    MIXSetPan(voice, ((int)(127.0f * value) - 1) / 2 + 64);
}

void SetVoiceSurroundPan(AXVPB* voice, float value)
{
    MIXSetSPan(voice, g_pAudioBackend->m_OutputMode == 3
            ? ((int)(127.0f * value) - 1) / 2 + 64
            : 127);
}

void SetVoiceInterauralDelay(AXVPB* voice, int value)
{
    u16 rShift = value <= 0 ? 0 : value;
    u16 lShift = -value <= 0 ? 0 : -value;
    AXSetVoiceItdTarget(voice, lShift, rShift);
}

void SetVoiceLowPassFilter(AXVPB* voice, bool enabled, unsigned int frequency, bool unchanged)
{
    if (enabled && !unchanged)
        AXGetLpfCoefs(frequency, &sVoiceLowPassFilter.a0, &sVoiceLowPassFilter.b0);
    if (voice->pb.lpf.on && enabled)
    {
        AXSetVoiceLpfCoefs(voice, sVoiceLowPassFilter.a0, sVoiceLowPassFilter.b0);
    }
    else
    {
        sVoiceLowPassFilter.on = enabled;
        sVoiceLowPassFilter.yn1 = 0;
        AXSetVoiceLpf(voice, &sVoiceLowPassFilter);
    }
}

void SetVoiceAuxiliaryVolume(AXVPB* voice, int auxiliary, int value)
{
    switch (auxiliary)
    {
    case 0:
        MIXSetAuxA(voice, value);
        break;
    case 1:
        MIXSetAuxB(voice, value);
        break;
    }
}

void AudioSource::SetSpatialParameters(Plat3dSoundSrc* source)
{
    if (HasVoice())
    {
        SetPan(source->m_Unknown20);
        SetSurroundPan(source->m_Unknown24);
        SetInterauralDelay((int)source->m_Unknown44 >> 24);
    }
}

void AudioSource::UpdateState()
{
    if (m_Unknown10 == 5)
    {
        m_Unknown04 = 3;
    }
    else
    {
        m_Unknown04 = m_Unknown10;
    }
    if (m_Unknown04 == 6 && HasVoice() && !WasVoiceDropped())
        ReleaseVoice(true);
}

void AudioSource::SetControllerSpeaker(bool enabled, unsigned int channel)
{
    m_Unknown14_18 = enabled;
    m_Unknown14_19 = channel;
    AXVPB* voice = GetVoice();
    MIXRmtSetVolumes(voice, 0,
        m_Unknown14_19 == 0 ? 0 : -960,
        m_Unknown14_19 == 1 ? 0 : -960,
        m_Unknown14_19 == 2 ? 0 : -960,
        m_Unknown14_19 == 3 ? 0 : -960,
        -960, -960, -960, -960);
    AXSetVoiceRmtOn(voice, true);
}

AudioSampleSource::AudioSampleSource()
    : m_Unknown18(0)
    , m_Unknown1C(0)
    , m_Unknown20(0)
    , m_Unknown24(0)
    , m_Unknown28(false)
{
    ++gAudioSampleSourceCount;
    ++gAudioSourceCount;
}

void AudioSampleSource::Initialize(AudioSourceInfo* info)
{
    m_Unknown08 = info;
    m_Unknown20 = SPGetSoundEntry(info->m_Unknown18->m_Unknown18.m_SoundTable, info->m_Unknown00);
    m_Unknown1C = AXAcquireVoice(15, OnVoiceDropped, (unsigned long)this);
    if (m_Unknown1C == 0)
        DumpAudioMemory();
    SPPrepareSound(m_Unknown20, m_Unknown1C, m_Unknown20->sampleRate);
    m_Unknown0C = (float)m_Unknown20->sampleRate / 32000.0f;
    MIXInitChannel(m_Unknown1C, 0, 0, -960, -960, -960, 64, 127, 0);
    m_Unknown10 = 1;
    m_Unknown04 = 1;
}

inline void AudioSampleSource::operator delete(void* pointer)
{
    gAudioSampleSourcePool.Free((AudioSampleSource*)pointer);
}

AudioSampleSource::~AudioSampleSource()
{
    ReleaseVoice(true);
    --gAudioSampleSourceCount;
    --gAudioSourceCount;
}

void AudioSampleSource::Update()
{
    switch (m_Unknown10)
    {
    case 5:
        if (m_Unknown14_00 == 1)
        {
            AXSetVoiceLoop(m_Unknown1C, false);
        }
        else
        {
            AXSetVoiceLoop(m_Unknown1C, true);
            AXSetVoiceLoopAddr(m_Unknown1C,
                (m_Unknown1C->pb.addr.currentAddressHi << 16) | m_Unknown1C->pb.addr.currentAddressLo);
        }
        AXSetVoiceState(m_Unknown1C, AX_VOICE_RUN);
        m_Unknown10 = 4;
        break;
    case 4:
        if (m_Unknown1C->pb.state == AX_VOICE_STOP)
        {
            m_Unknown10 = 6;
        }
        else
        {
            unsigned int address = (m_Unknown1C->pb.addr.currentAddressHi << 16) | m_Unknown1C->pb.addr.currentAddressLo;
            if (address < m_Unknown18)
            {
                ++m_Unknown14_0C;
                if (!(m_Unknown14_0C < m_Unknown14_00 || m_Unknown14_00 == 0xFFFF))
                {
                    AXSetVoiceLoop(m_Unknown1C, false);
                    AXSetVoiceLoopAddr(m_Unknown1C, (unsigned long)g_pAudioSilenceBuffer * 2);
                }
            }
            m_Unknown18 = address;
        }
        break;
    case 6:
        m_Unknown10 = 1;
        break;
    }
}

bool AudioSampleSource::Play(unsigned int value)
{
    m_Unknown14_00 = value;
    m_Unknown10 = 5;
    return true;
}

void AudioSampleSource::Stop()
{
    switch (m_Unknown10)
    {
    case 4:
    {
        bool enabled = OSDisableInterrupts();
        if (m_Unknown1C != 0)
        {
            AXSetVoiceState(m_Unknown1C, AX_VOICE_STOP);
            AXSetVoiceLoopAddr(m_Unknown1C, (unsigned long)g_pAudioSilenceBuffer * 2);
        }
        OSRestoreInterrupts(enabled);
        break;
    }
    case 3:
    case 7:
        m_Unknown10 = 6;
        m_Unknown04 = 6;
        break;
    }
}

bool AudioSampleSource::Pause()
{
    m_Unknown24 = ((m_Unknown1C->pb.addr.currentAddressHi << 16) | m_Unknown1C->pb.addr.currentAddressLo) + 320;
    AXSetVoiceEndAddr(m_Unknown1C, m_Unknown24);
    AXSetVoiceLoop(m_Unknown1C, false);
    m_Unknown10 = 7;
    m_Unknown04 = 7;
    return true;
}

bool AudioSampleSource::Resume()
{
    AXSetVoiceCurrentAddr(m_Unknown1C, m_Unknown24);
    AXSetVoiceEndAddr(m_Unknown1C, m_Unknown20->endAddr);
    AXSetVoiceLoop(m_Unknown1C, m_Unknown14_0C < m_Unknown14_00 || m_Unknown14_00 == 0xFFFF);
    AXSetVoiceSrcRatio(m_Unknown1C, (float)m_Unknown20->sampleRate / 32000.0f);
    AXSetVoiceState(m_Unknown1C, AX_VOICE_RUN);
    m_Unknown10 = 4;
    m_Unknown04 = 4;
    return true;
}

void AudioSampleSource::OnVoiceDropped(void* pointer)
{
    AXVPB* voice = (AXVPB*)pointer;
    AudioSampleSource* source = (AudioSampleSource*)voice->userContext;
    source->m_Unknown28 = true;
    source->ReleaseVoice(false);
    ++gResidentVoiceDropCount;
    source->m_Unknown10 = 6;
    source->m_Unknown04 = 6;
}

void AudioSampleSource::ReleaseVoice(bool release)
{
    if (m_Unknown1C != 0)
    {
        AXSetVoiceState(m_Unknown1C, AX_VOICE_STOP);
        MIXReleaseChannel(m_Unknown1C);
        if (release)
            AXFreeVoice(m_Unknown1C);
        m_Unknown1C = 0;
    }
}

AudioStreamChannel::AudioStreamChannel()
{
    m_Unknown08 = 0;
    m_Unknown14 = 0;
    m_Unknown10_00 = 0;
    m_Unknown10_1F = 0;
}

AudioStreamChannel::~AudioStreamChannel()
{
    if (m_Unknown04 != 0)
    {
        AXSetVoiceState(m_Unknown04, AX_VOICE_STOP);
        MIXReleaseChannel(m_Unknown04);
        AXFreeVoice(m_Unknown04);
        m_Unknown04 = 0;
    }
    g_pAudioBackend->FreeAudioMemory(m_Unknown08);
}

void AudioStreamChannel::PrepareVoice(AudioStreamHeader* header)
{
    unsigned int size = m_Unknown00->m_Unknown08->m_Unknown18->m_Unknown10->m_Unknown04 * 2;
    unsigned int start = (m_Unknown14 + 1) * 2;
    unsigned int end = (m_Unknown14 + size - 1) * 2;
    AXPBADDR addr;
    addr.loopFlag = 1;
    addr.format = 0;
    addr.loopAddressHi = start >> 16;
    addr.loopAddressLo = start;
    addr.endAddressHi = end >> 16;
    addr.endAddressLo = end;
    addr.currentAddressHi = start >> 16;
    addr.currentAddressLo = start;
    AXPBADPCM adpcm;
    unsigned short* coefficients = &adpcm.a[0][0];
    for (int i = 0; i < 16; ++i)
    {
        coefficients[i] = header->coef[i];
    }
    adpcm.gain = header->gain;
    adpcm.pred_scale = header->ps;
    adpcm.yn1 = header->yn1;
    adpcm.yn2 = header->yn2;
    AXSetVoiceSrcType(m_Unknown04, AX_SRC_TYPE_LINEAR);
    AXSetVoiceSrcRatio(m_Unknown04, m_Unknown00->m_Unknown0C);
    AXSetVoiceType(m_Unknown04, AX_VOICE_STREAM);
    AXSetVoiceAddr(m_Unknown04, &addr);
    AXSetVoiceAdpcm(m_Unknown04, &adpcm);
}

void OnAudioStreamReadComplete(nlFile*, void*, unsigned int, unsigned long userParam)
{
    AudioReadState* state = ((AudioStreamChannel*)userParam)->m_Unknown00;
    --state->m_Unknown20_00;
    nlGetCurrentAsyncRead();

    AudioReadQueueEntry* entry = state->m_Unknown24->m_next;
    if (entry == state->m_Unknown24)
        state->m_Unknown24 = 0;
    else
        state->m_Unknown24->m_next = entry->m_next;
    gAudioReadQueueEntryPool.Free(entry);
}

void AudioStreamChannel::OnVoiceDropped(void* pointer)
{
    AXVPB* voice = (AXVPB*)pointer;
    AudioStreamChannel* channel = (AudioStreamChannel*)voice->userContext;
    channel->m_Unknown10_1F = 1;
    if (channel->m_Unknown04 != 0)
    {
        AXSetVoiceState(channel->m_Unknown04, AX_VOICE_STOP);
        MIXReleaseChannel(channel->m_Unknown04);
        channel->m_Unknown04 = 0;
    }
    channel->m_Unknown00->Stop();
}

void AudioStreamChannel::ReleaseVoice(bool release)
{
    if (m_Unknown04 != 0)
    {
        AXSetVoiceState(m_Unknown04, AX_VOICE_STOP);
        MIXReleaseChannel(m_Unknown04);
        if (release)
            AXFreeVoice(m_Unknown04);
        m_Unknown04 = 0;
    }
}

AudioReadState::AudioReadState()
{
    m_Unknown18 = 0;
    m_Unknown1C = 0;
    m_Unknown20_00 = 0;
    m_Unknown20_07 = 0;
    m_Unknown20_1F = 0;
    m_Unknown24 = 0;
    m_Unknown28 = 3;
    ++gAudioStreamSourceCount;
    ++gAudioSourceCount;
}

AudioReadState::~AudioReadState()
{
    --gAudioStreamSourceCount;
    --gAudioSourceCount;
}

void AudioReadState::Initialize(AudioSourceInfo* info)
{
    m_Unknown08 = info;
    AudioStreamChannel* channel = GetChannelIterator();
    while ((channel = GetNextChannel(channel)) != 0)
    {
        channel->m_Unknown00 = this;
        channel->m_Unknown08 = g_pAudioBackend->AllocateAudioMemory(m_Unknown08->m_Unknown18->m_Unknown10->m_Unknown04 * 2);
        if (channel->m_Unknown08 == 0)
            DumpAudioMemory();
        channel->m_Unknown04 = AXAcquireVoice(31, AudioStreamChannel::OnVoiceDropped, (unsigned long)channel);
        MIXInitChannel(channel->m_Unknown04, 0, 0, -960, -960, -960, 64, 127, 0);
        channel->m_Unknown14 = (unsigned int)channel->m_Unknown08;
    }
    SetPan(0.0f);
    m_Unknown10 = 1;
}

bool AudioReadState::Play(unsigned int value)
{
    m_Unknown14_00 = value;
    switch (m_Unknown10)
    {
    case 1:
        Prepare();
    case 2:
        m_Unknown28 = 5;
        break;
    case 3:
    {
        AudioStreamChannel* channel = GetChannelIterator();
        while ((channel = GetNextChannel(channel)) != 0)
            AXSetVoiceState(channel->m_Unknown04, AX_VOICE_RUN);
        m_Unknown10 = 4;
        break;
    }
    default:
        return false;
    }
    return true;
}

void AudioReadState::Stop()
{
    switch (m_Unknown10)
    {
    case 4:
    {
        bool enabled;
        AudioStreamChannel* channel = GetChannelIterator();
        while ((channel = GetNextChannel(channel)) != 0)
        {
            enabled = OSDisableInterrupts();
            if (channel->m_Unknown04 != 0)
                AXSetVoiceState(channel->m_Unknown04, AX_VOICE_STOP);
            OSRestoreInterrupts(enabled);
        }
    }
    case 2:
        m_Unknown28 = 6;
        if (m_Unknown20_00 != 0)
            g_pAudioBackend->QueueReadCancellation(this);
        m_Unknown10 = 6;
        break;
    case 3:
    case 7:
        m_Unknown10 = 6;
        break;
    case 5:
        break;
    }
}

bool AudioReadState::Pause()
{
    if (m_Unknown10 != 4)
        return false;
    AudioStreamChannel* channel = GetChannelIterator();
    while ((channel = GetNextChannel(channel)) != 0)
    {
        channel->m_Unknown0C = (channel->m_Unknown04->pb.addr.currentAddressHi << 16) | channel->m_Unknown04->pb.addr.currentAddressLo;
        AXSetVoiceState(channel->m_Unknown04, AX_VOICE_STOP);
    }
    m_Unknown10 = 7;
    return true;
}

bool AudioReadState::Resume()
{
    if (m_Unknown10 != 7)
        return false;
    AudioStreamChannel* channel = GetChannelIterator();
    while ((channel = GetNextChannel(channel)) != 0)
    {
        AXSetVoiceCurrentAddr(channel->m_Unknown04, channel->m_Unknown0C);
        AXSetVoiceState(channel->m_Unknown04, AX_VOICE_RUN);
    }
    m_Unknown10 = 4;
    return true;
}

void TrackAudioRead(AudioReadState* state, AsyncEntry* request)
{
    AudioReadQueueEntry* entry = gAudioReadQueueEntryPool.Allocate();
    entry->m_Unknown00 = request;
    nlRingAddEnd(&state->m_Unknown24, entry);
}

void CancelAudioReads(AudioReadState* state)
{
    if (state->m_Unknown24 == 0)
        return;
    AudioReadQueueEntry* pending = 0;
    bool cancelled = false;
    while (state->m_Unknown24 != 0)
    {
        AudioReadQueueEntry* entry = state->m_Unknown24->m_next;
        if (cancelled || nlAsyncReadBusy(entry->m_Unknown00))
        {
            if (entry == state->m_Unknown24)
                state->m_Unknown24 = 0;
            else
                state->m_Unknown24->m_next = entry->m_next;
            nlRingAddEnd(&pending, entry);
        }
        else
        {
            nlCancelAsyncRead(entry->m_Unknown00, OnAudioReadCancelled);
            cancelled = true;
        }
    }
    state->m_Unknown24 = pending;
}

void OnAudioReadCancelled(nlFile* file, void* buffer, unsigned int size,
    unsigned long userParam, ReadAsyncCallback callback)
{
    callback(file, buffer, size, userParam);
}

void AudioSampleSource::SetAuxiliaryVolume(int auxiliary, int value)
{
    bool enabled = OSDisableInterrupts();
    if (!HasVoice())
    {
        OSRestoreInterrupts(enabled);
        return;
    }
    SetVoiceAuxiliaryVolume(m_Unknown1C, auxiliary, value);
    OSRestoreInterrupts(enabled);
}

bool AudioSampleSource::HasVoice()
{
    return m_Unknown1C != 0;
}

void AudioSampleSource::SetLowPassFilter(bool on, unsigned int frequency, bool unchanged)
{
    bool enabled = OSDisableInterrupts();
    if (!HasVoice())
    {
        OSRestoreInterrupts(enabled);
        return;
    }
    SetVoiceLowPassFilter(m_Unknown1C, on, frequency, unchanged);
    OSRestoreInterrupts(enabled);
}

void AudioSampleSource::SetSurroundPan(float value)
{
    bool enabled = OSDisableInterrupts();
    if (!HasVoice())
    {
        OSRestoreInterrupts(enabled);
        return;
    }
    SetVoiceSurroundPan(m_Unknown1C, value);
    OSRestoreInterrupts(enabled);
}

void AudioSampleSource::SetInterauralDelay(int value)
{
    bool enabled = OSDisableInterrupts();
    if (!HasVoice())
    {
        OSRestoreInterrupts(enabled);
        return;
    }
    SetVoiceInterauralDelay(m_Unknown1C, value);
    OSRestoreInterrupts(enabled);
}

void AudioSampleSource::SetPan(float value)
{
    bool enabled = OSDisableInterrupts();
    if (!HasVoice())
    {
        OSRestoreInterrupts(enabled);
        return;
    }
    SetVoicePan(m_Unknown1C, value);
    OSRestoreInterrupts(enabled);
}

void AudioSampleSource::SetPitch(float value)
{
    bool enabled = OSDisableInterrupts();
    if (!HasVoice())
    {
        OSRestoreInterrupts(enabled);
        return;
    }
    SetVoicePitch(m_Unknown1C, m_Unknown0C, value);
    OSRestoreInterrupts(enabled);
}

void AudioSampleSource::SetMixVolume(float value)
{
    bool enabled = OSDisableInterrupts();
    if (!HasVoice())
    {
        OSRestoreInterrupts(enabled);
        return;
    }
    SetVoiceMixVolume(m_Unknown1C, value);
    OSRestoreInterrupts(enabled);
}

void AudioSampleSource::SetInputVolume(float value)
{
    bool enabled = OSDisableInterrupts();
    if (!HasVoice())
    {
        OSRestoreInterrupts(enabled);
        return;
    }
    if (m_Unknown14_18)
        MIXRmtSetFader(m_Unknown1C, m_Unknown14_19, (int)(10.0f * value));
    if (g_pAudioBackend->m_Unknown450 || !m_Unknown14_18)
        SetVoiceInputVolume(m_Unknown1C, value);
    OSRestoreInterrupts(enabled);
}

AXVPB* AudioSampleSource::GetVoice()
{
    return m_Unknown1C;
}

bool AudioSampleSource::WasVoiceDropped()
{
    return m_Unknown28;
}

bool AudioSampleSource::Prepare()
{
    m_Unknown10 = 3;
    return false;
}
