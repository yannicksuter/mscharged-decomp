#include "revolution/types.h"
#include "NL/nlDLListContainer.inl"
#include "Game/Audio/AudioSource.h"
#include "Game/Audio/AudioResourcePlatform.h"

#include "Game/Audio/AudioBackend.h"
#include "Game/Audio/AudioGlobals.h"
#include "Game/Audio/Plat3dSoundSrc.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "NL/nlArrayAllocator.h"
#include "NL/nlFileGC.h"
#include "NL/nlMath.h"
#include "NL/nlPrint.h"
#include "NL/nlRing.h"
#include "revolution/mix.h"

#include <string.h>

unsigned int gResidentVoiceDropCount;
unsigned int gStreamVoiceDropCount;
unsigned int gAudioStreamSourceCount;
unsigned int gAudioSampleSourceCount;
unsigned int gAudioSourceCount;
unsigned int gAudioStreamStarveCount;

SlotPool<AudioSampleSource> gAudioSampleSourcePool(64, 16);
SlotPool<AudioMonoStreamSource> gAudioMonoStreamSourcePool(16, 16);
SlotPool<AudioStereoStreamSource> gAudioStereoStreamSourcePool(16, 16);
SlotPool<AudioReadQueueEntry> gAudioReadQueueEntryPool(32, 16);

struct AudioReadCallbackEntry
{
    AudioReadState* m_State;
    AudioStreamChannel* m_Channel;
};

AudioReadCallbackEntry gAudioReadCallbackStorage[24];
nlArrayAllocator<AudioReadCallbackEntry> gAudioReadCallbackAllocator(gAudioReadCallbackStorage, 24);
AXPBLPF sVoiceLowPassFilter;

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
        SetPan(source->m_Pan);
        SetSurroundPan(source->m_SurroundPan);
        SetInterauralDelay((int)source->m_SpatialBits >> 24);
    }
}

void AudioSource::UpdateState()
{
    if (m_InternalState == 5)
    {
        m_State = 3;
    }
    else
    {
        m_State = m_InternalState;
    }
    if (m_State == 6 && HasVoice() && !WasVoiceDropped())
        ReleaseVoice(true);
}

void AudioSource::SetControllerSpeaker(bool enabled, unsigned int channel)
{
    m_ControllerSpeakerEnabled = enabled;
    m_ControllerSpeakerChannel = channel;
    AXVPB* voice = GetVoice();
    MIXRmtSetVolumes(voice, 0,
        (m_ControllerSpeakerChannel != 0) * -960,
        (m_ControllerSpeakerChannel != 1) * -960,
        (m_ControllerSpeakerChannel != 2) * -960,
        (m_ControllerSpeakerChannel != 3) * -960,
        -960, -960, -960, -960);
    AXSetVoiceRmtOn(voice, true);
}

AudioSampleSource::AudioSampleSource()
    : m_LastVoiceAddress(0)
    , m_Voice(0)
    , m_SoundEntry(0)
    , m_PauseAddress(0)
    , m_VoiceDropped(false)
{
    ++gAudioSampleSourceCount;
    ++gAudioSourceCount;
}

void AudioSampleSource::Initialize(AudioSourceInfo* info)
{
    m_SourceInfo = info;
    m_SoundEntry = SPGetSoundEntry(((AudioMemoryLoader*)info->m_BankLoader)->m_SoundTable, info->m_SoundIndex);
    m_Voice = AXAcquireVoice(15, OnVoiceDropped, (unsigned long)this);
    if (m_Voice == 0)
        DumpAudioMemory();
    SPPrepareSound(m_SoundEntry, m_Voice, m_SoundEntry->sampleRate);
    m_SampleRateRatio = (float)m_SoundEntry->sampleRate / 32000.0f;
    MIXInitChannel(m_Voice, 0, 0, -960, -960, -960, 64, 127, 0);
    m_InternalState = 1;
    m_State = 1;
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
    switch (m_InternalState)
    {
    case 5:
        if (m_PlayCount == 1)
        {
            AXSetVoiceLoop(m_Voice, false);
        }
        else
        {
            AXSetVoiceLoop(m_Voice, true);
            AXSetVoiceLoopAddr(m_Voice,
                (m_Voice->pb.addr.currentAddressHi << 16) | m_Voice->pb.addr.currentAddressLo);
        }
        AXSetVoiceState(m_Voice, AX_VOICE_RUN);
        m_InternalState = 4;
        break;
    case 4:
        if (m_Voice->pb.state == AX_VOICE_STOP)
        {
            m_InternalState = 6;
        }
        else
        {
            unsigned int address = (m_Voice->pb.addr.currentAddressHi << 16) | m_Voice->pb.addr.currentAddressLo;
            if (address < m_LastVoiceAddress)
            {
                ++m_PlayIteration;
                bool canLoop = m_PlayIteration < m_PlayCount || m_PlayCount == 0xFFFF;
                if (!canLoop)
                {
                    AXSetVoiceLoop(m_Voice, false);
                    AXSetVoiceLoopAddr(m_Voice, (unsigned long)g_pAudioSilenceBuffer * 2);
                }
            }
            m_LastVoiceAddress = address;
        }
        break;
    case 6:
        m_InternalState = 1;
        break;
    case 0:
        return;
    case 1:
    case 3:
    case 7:
        break;
    }
}

bool AudioSampleSource::Play(unsigned int value)
{
    m_PlayCount = value;
    m_InternalState = 5;
    return true;
}

void AudioSampleSource::Stop()
{
    switch (m_InternalState)
    {
    case 4:
    {
        bool enabled = OSDisableInterrupts();
        if (m_Voice != 0)
        {
            AXSetVoiceState(m_Voice, AX_VOICE_STOP);
            AXSetVoiceLoopAddr(m_Voice, (unsigned long)g_pAudioSilenceBuffer * 2);
        }
        OSRestoreInterrupts(enabled);
        break;
    }
    case 3:
    case 7:
        m_InternalState = 6;
        m_State = 6;
        break;
    }
}

bool AudioSampleSource::Pause()
{
    m_PauseAddress = ((m_Voice->pb.addr.currentAddressHi << 16) | m_Voice->pb.addr.currentAddressLo) + 320;
    AXSetVoiceEndAddr(m_Voice, m_PauseAddress);
    AXSetVoiceLoop(m_Voice, false);
    m_InternalState = 7;
    m_State = 7;
    return true;
}

bool AudioSampleSource::Resume()
{
    AXSetVoiceCurrentAddr(m_Voice, m_PauseAddress);
    AXSetVoiceEndAddr(m_Voice, m_SoundEntry->endAddr);
    AXSetVoiceLoop(m_Voice, m_PlayIteration < m_PlayCount || m_PlayCount == 0xFFFF);
    AXSetVoiceSrcRatio(m_Voice, (float)m_SoundEntry->sampleRate / 32000.0f);
    AXSetVoiceState(m_Voice, AX_VOICE_RUN);
    m_InternalState = 4;
    m_State = 4;
    return true;
}

void AudioSampleSource::OnVoiceDropped(void* pointer)
{
    AXVPB* voice = (AXVPB*)pointer;
    AudioSampleSource* source = (AudioSampleSource*)voice->userContext;
    source->m_VoiceDropped = true;
    source->ReleaseVoice(false);
    ++gResidentVoiceDropCount;
    source->m_InternalState = 6;
    source->m_State = 6;
}

void AudioSampleSource::ReleaseVoice(bool release)
{
    if (m_Voice != 0)
    {
        AXSetVoiceState(m_Voice, AX_VOICE_STOP);
        MIXReleaseChannel(m_Voice);
        if (release)
            AXFreeVoice(m_Voice);
        m_Voice = 0;
    }
}

AudioStreamChannel::AudioStreamChannel()
{
    m_Buffer = 0;
    m_BufferAddress = 0;
    m_ReadPosition = 0;
    m_VoiceDropped = 0;
}

AudioStreamChannel::~AudioStreamChannel()
{
    if (m_Voice != 0)
    {
        AXSetVoiceState(m_Voice, AX_VOICE_STOP);
        MIXReleaseChannel(m_Voice);
        AXFreeVoice(m_Voice);
        m_Voice = 0;
    }
    g_pAudioBackend->FreeAudioMemory(m_Buffer);
}

void AudioStreamChannel::PrepareVoice(AudioStreamHeader* header)
{
    unsigned int size = m_ReadState->m_SourceInfo->m_BankLoader->m_Chunk23200->m_StreamBlockSize * 2;
    unsigned int start = (m_BufferAddress + 1) * 2;
    unsigned int end = (m_BufferAddress + size - 1) * 2;
    AXPBADDR addr;
    addr.loopFlag = 1;
    addr.format = 0;
    addr.loopAddressHi = start >> 16;
    unsigned short startLow = start;
    addr.loopAddressLo = startLow;
    addr.endAddressHi = end >> 16;
    addr.endAddressLo = end;
    addr.currentAddressHi = start >> 16;
    addr.currentAddressLo = startLow;
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
    AXSetVoiceSrcType(m_Voice, AX_SRC_TYPE_LINEAR);
    AXSetVoiceSrcRatio(m_Voice, m_ReadState->m_SampleRateRatio);
    AXSetVoiceType(m_Voice, AX_VOICE_STREAM);
    AXSetVoiceAddr(m_Voice, &addr);
    AXSetVoiceAdpcm(m_Voice, &adpcm);
}

static inline unsigned int GetVoiceLoopAddress(AXVPB* voice)
{
    return (voice->pb.addr.loopAddressHi << 16) | voice->pb.addr.loopAddressLo;
}

static inline unsigned int GetVoiceEndAddress(AXVPB* voice)
{
    return (voice->pb.addr.endAddressHi << 16) | voice->pb.addr.endAddressLo;
}

static inline unsigned int GetVoiceCurrentAddress(AXVPB* voice)
{
    return (voice->pb.addr.currentAddressHi << 16) | voice->pb.addr.currentAddressLo;
}

inline nlFile* AudioReadState::GetStreamFile()
{
    return ((AudioFileLoader*)m_SourceInfo->m_BankLoader)->GetFile();
}

inline unsigned int AudioReadState::GetStreamBlockSize()
{
    return m_SourceInfo->m_BankLoader->m_Chunk23200->m_StreamBlockSize;
}

inline unsigned int AudioStreamChannel::GetBufferSize()
{
    return m_ReadState->GetStreamBlockSize() * 2;
}

inline void AudioStreamChannel::AdvanceReadPosition(unsigned int size)
{
    m_ReadPosition += size;
    m_ReadPosition %= GetBufferSize();
}

inline void AudioReadState::QueueStreamRead(unsigned int offset, void* buffer, unsigned int size,
    ReadAsyncCallback callback, unsigned long userParam)
{
    g_pAudioBackend->QueueRead(GetStreamFile(), offset, buffer, size, callback, userParam, this);
    ++m_PendingReadCount;
}

inline void AudioReadState::CompleteRead()
{
    --m_PendingReadCount;
    nlGetCurrentAsyncRead();

    AudioReadQueueEntry* entry = nlRingRemoveStart(&m_ReadQueue);
    gAudioReadQueueEntryPool.Free(entry);
}

void OnAudioStreamReadComplete(nlFile*, void*, unsigned int, unsigned long userParam)
{
    ((AudioStreamChannel*)userParam)->m_ReadState->CompleteRead();
}

void AudioStreamChannel::OnVoiceDropped(void* pointer)
{
    AXVPB* voice = (AXVPB*)pointer;
    AudioStreamChannel* channel = (AudioStreamChannel*)voice->userContext;
    channel->m_VoiceDropped = 1;
    if (channel->m_Voice != 0)
    {
        AXSetVoiceState(channel->m_Voice, AX_VOICE_STOP);
        MIXReleaseChannel(channel->m_Voice);
        channel->m_Voice = 0;
    }
    channel->m_ReadState->Stop();
}

void AudioStreamChannel::ReleaseVoice(bool release)
{
    if (m_Voice != 0)
    {
        AXSetVoiceState(m_Voice, AX_VOICE_STOP);
        MIXReleaseChannel(m_Voice);
        if (release)
            AXFreeVoice(m_Voice);
        m_Voice = 0;
    }
}

AudioReadState::AudioReadState()
{
    m_StreamDataSize = 0;
    m_StreamReadPosition = 0;
    m_PendingReadCount = 0;
    m_StreamEndPosition = 0;
    m_EndAddressSet = 0;
    m_ReadQueue = 0;
    m_PendingState = 3;
    ++gAudioStreamSourceCount;
    ++gAudioSourceCount;
}

AudioReadState::~AudioReadState()
{
    --gAudioStreamSourceCount;
    --gAudioSourceCount;
}

inline unsigned int AudioReadState::GetStreamDataStart()
{
    return GetChannelCount() * sizeof(AudioStreamHeader) + GetStreamHeaderOffset();
}

inline unsigned int AudioReadState::GetChannelDataOffset(AudioStreamChannel* channel)
{
    return (GetStreamDataStart() + m_StreamReadPosition * GetChannelCount()) + (channel - GetFirstChannel()) * GetStreamBlockSize();
}

void OnAudioStreamHeaderRead(nlFile*, void* buffer, unsigned int, unsigned long userParam)
{
    AudioReadCallbackEntry* entry = (AudioReadCallbackEntry*)userParam;
    if (entry->m_State->m_PendingState == 6)
    {
        entry->m_State->CompleteRead();
        gAudioReadCallbackAllocator.Free(entry);
    }
    else if (buffer == 0)
    {
        entry->m_State->Stop();
    }
    else
    {
        AudioStreamHeader* header = (AudioStreamHeader*)buffer;
        entry->m_State->m_StreamDataSize = header->num_adpcm_nibbles / 2;
        entry->m_State->m_SampleRateRatio = (float)header->sample_rate / 32000.0f;
        entry->m_Channel->PrepareVoice(header);

        entry->m_State->QueueStreamRead(entry->m_State->GetChannelDataOffset(entry->m_Channel), entry->m_Channel->GetBuffer(),
            entry->m_State->GetStreamBlockSize(), OnAudioStreamPrimeRead, (unsigned long)entry);
        entry->m_Channel->AdvanceReadPosition(entry->m_State->GetStreamBlockSize());
        entry->m_State->CompleteRead();
    }
    if (buffer != 0)
        g_pAudioStreamBlockPool->Free((AudioStreamBlock*)buffer);
}

inline void AudioReadState::OnChannelPrepared(AudioStreamChannel* channel)
{
    if (channel == GetFirstChannel() + GetChannelCount() - 1)
    {
        m_InternalState = 3;
        switch (m_PendingState)
        {
        case 3:
            break;
        case 5:
        {
            AudioStreamChannel* voiceChannel = GetChannelIterator();
            while ((voiceChannel = GetNextChannel(voiceChannel)) != 0)
                AXSetVoiceState(voiceChannel->m_Voice, AX_VOICE_RUN);
            m_InternalState = 4;
            break;
        }
        case 1:
            m_InternalState = 3;
            break;
        }
    }
}

void OnAudioStreamPrimeRead(nlFile* file, void* buffer, unsigned int size, unsigned long userParam)
{
    AudioReadCallbackEntry* entry = (AudioReadCallbackEntry*)userParam;
    AudioReadState* state = entry->m_State;
    if (state->m_PendingState == 6)
    {
        state->CompleteRead();
        gAudioReadCallbackAllocator.Free(entry);
        return;
    }

    OnAudioStreamReadComplete(file, buffer, size, (unsigned long)entry->m_Channel);
    entry->m_State->m_StreamReadPosition = size;
    entry->m_State->OnChannelPrepared(entry->m_Channel);
    gAudioReadCallbackAllocator.Free(entry);
}

void AudioReadState::Initialize(AudioSourceInfo* info)
{
    m_SourceInfo = info;
    AudioStreamChannel* channel = GetChannelIterator();
    while ((channel = GetNextChannel(channel)) != 0)
    {
        channel->m_ReadState = this;
        channel->m_Buffer = g_pAudioBackend->AllocateAudioMemory(m_SourceInfo->m_BankLoader->m_Chunk23200->m_StreamBlockSize * 2);
        if (channel->m_Buffer == 0)
            DumpAudioMemory();
        channel->m_Voice = AXAcquireVoice(31, AudioStreamChannel::OnVoiceDropped, (unsigned long)channel);
        MIXInitChannel(channel->m_Voice, 0, 0, -960, -960, -960, 64, 127, 0);
        channel->m_BufferAddress = (unsigned int)channel->m_Buffer;
    }
    SetPan(0.0f);
    m_InternalState = 1;
}

bool AudioReadState::Prepare()
{
    AudioStreamChannel* channel = GetChannelIterator();
    while ((channel = GetNextChannel(channel)) != 0)
    {
        AudioReadCallbackEntry* entry = gAudioReadCallbackAllocator.Allocate();
        entry->m_State = this;
        entry->m_Channel = channel;
        int index = channel - GetFirstChannel();
        AudioStreamBlock* header = g_pAudioStreamBlockPool->Allocate();
        unsigned int offset = GetStreamHeaderOffset() + index * sizeof(AudioStreamHeader);
        QueueStreamRead(offset, header, sizeof(AudioStreamHeader), OnAudioStreamHeaderRead, (unsigned long)entry);
    }
    SetInputVolume(-96.0f);
    m_InternalState = 2;
    return true;
}

bool AudioReadState::Play(unsigned int value)
{
    m_PlayCount = value;
    switch (m_InternalState)
    {
    case 1:
        Prepare();
    case 2:
        m_PendingState = 5;
        break;
    case 3:
    {
        AudioStreamChannel* channel = GetChannelIterator();
        while ((channel = GetNextChannel(channel)) != 0)
            AXSetVoiceState(channel->m_Voice, AX_VOICE_RUN);
        m_InternalState = 4;
        break;
    }
    default:
        return false;
    }
    return true;
}

void AudioReadState::Stop()
{
    switch (m_InternalState)
    {
    case 4:
    {
        bool enabled;
        AudioStreamChannel* channel = GetChannelIterator();
        while ((channel = GetNextChannel(channel)) != 0)
        {
            enabled = OSDisableInterrupts();
            if (channel->m_Voice != 0)
                AXSetVoiceState(channel->m_Voice, AX_VOICE_STOP);
            OSRestoreInterrupts(enabled);
        }
    }
    case 2:
        m_PendingState = 6;
        if (m_PendingReadCount != 0)
            g_pAudioBackend->QueueReadCancellation(this);
        m_InternalState = 6;
        break;
    case 3:
    case 7:
        m_InternalState = 6;
        break;
    case 5:
        break;
    }
}

static inline unsigned int GetChannelReadSpace(AudioStreamChannel* channel, unsigned int readPos)
{
    return channel->GetBufferSize() - readPos;
}

static inline unsigned int GetAlignedStreamReadSize(const unsigned int& length)
{
    return nlAlignUp(length, 32);
}

inline void AudioReadState::QueueFullChannelRead(AudioStreamChannel* channel)
{
    unsigned int size = GetAlignedStreamReadSize(GetStreamBlockSize());
    unsigned int space = GetChannelReadSpace(channel, channel->m_ReadPosition);
    unsigned int readPos = channel->m_ReadPosition;
    unsigned int offset = GetChannelDataOffset(channel);
    if (space < size)
    {
        QueueStreamRead(offset, (char*)channel->GetBuffer() + readPos, space,
            OnAudioStreamReadComplete, (unsigned long)channel);
        channel->AdvanceReadPosition(space);
        unsigned int remainder = size - space;
        QueueStreamRead(offset + space, channel->GetBuffer(), remainder,
            OnAudioStreamReadComplete, (unsigned long)channel);
        channel->AdvanceReadPosition(remainder);
    }
    else
    {
        QueueStreamRead(offset, (char*)channel->GetBuffer() + readPos, size,
            OnAudioStreamReadComplete, (unsigned long)channel);
        channel->AdvanceReadPosition(size);
    }
}

inline void AudioReadState::QueuePartialChannelRead(AudioStreamChannel* channel, unsigned int size)
{
    unsigned int space = GetChannelReadSpace(channel, channel->m_ReadPosition);
    unsigned int readPos = channel->m_ReadPosition;
    unsigned int offset = GetChannelDataOffset(channel);
    if (space < size)
    {
        QueueStreamRead(offset, (char*)channel->GetBuffer() + readPos, space,
            OnAudioStreamReadComplete, (unsigned long)channel);
        channel->AdvanceReadPosition(space);
        QueueStreamRead(offset + space, channel->GetBuffer(), size - space,
            OnAudioStreamReadComplete, (unsigned long)channel);
        channel->AdvanceReadPosition(size - space);
    }
    else
    {
        QueueStreamRead(offset, (char*)channel->GetBuffer() + readPos, size,
            OnAudioStreamReadComplete, (unsigned long)channel);
        channel->AdvanceReadPosition(size);
    }
}

static inline unsigned int GetStreamEndAddress(AXVPB* voice, unsigned int endPosition)
{
    unsigned int address = GetVoiceLoopAddress(voice);
    address += endPosition * 2;
    return address;
}

static inline bool NeedsStreamRead(AudioStreamChannel* first)
{
    AXVPB* voice = first->m_Voice;
    unsigned int played = (GetVoiceCurrentAddress(voice) - GetVoiceLoopAddress(voice)) / 2;
    int playPos = played - played % 32;
    int readPos = first->m_ReadPosition;
    int blockSize = first->m_ReadState->GetStreamBlockSize();

    if (playPos < readPos)
        return readPos - playPos < blockSize;
    return playPos - readPos > blockSize;
}

void AudioReadState::Update()
{
    switch (m_InternalState)
    {
    case 6:
        m_InternalState = m_PendingReadCount == 0 ? 1 : 6;
        break;
    case 4:
    {
        AXVPB* voice = GetFirstChannel()->m_Voice;
        if (GetVoiceCurrentAddress(voice) > GetVoiceEndAddress(voice))
            Stop();
        if (GetFirstChannel()->m_Voice->pb.state == AX_VOICE_STOP)
        {
            Stop();
            break;
        }
        if (m_EndAddressSet)
            break;

        voice = GetFirstChannel()->m_Voice;
        if ((GetVoiceCurrentAddress(voice) - GetVoiceLoopAddress(voice)) / 2 < m_StreamEndPosition)
        {
            AudioStreamChannel* channel = GetChannelIterator();
            while ((channel = GetNextChannel(channel)) != 0)
            {
                unsigned int end = GetStreamEndAddress(channel->m_Voice, m_StreamEndPosition);
                AXSetVoiceLoop(channel->m_Voice, false);
                AXSetVoiceLoopAddr(channel->m_Voice, (unsigned long)g_pAudioSilenceBuffer * 2);
                AXSetVoiceEndAddr(channel->m_Voice, end);
                m_EndAddressSet = true;
            }
            break;
        }

        AudioStreamChannel* first = GetFirstChannel();
        bool needRead = NeedsStreamRead(first);
        if (!needRead)
            break;
        if (m_StreamEndPosition != 0)
            break;
        if (m_PendingReadCount > 2)
        {
            nlPrintf("\t\t\t\t\t\t******* STARVE *******\n");
            ++gAudioStreamStarveCount;
            Stop();
            break;
        }

        unsigned int remaining = m_StreamDataSize - m_StreamReadPosition;
        if (remaining <= GetStreamBlockSize())
        {
            AudioStreamChannel* channel = GetChannelIterator();
            while ((channel = GetNextChannel(channel)) != 0)
                QueuePartialChannelRead(channel, GetAlignedStreamReadSize(remaining));
            m_StreamReadPosition = 0;
            bool loop = m_PlayIteration < m_PlayCount || m_PlayCount == 0xFFFF;
            if (!loop)
            {
                m_StreamEndPosition = GetFirstChannel()->m_ReadPosition;
                unsigned int end = nlAlignUp(m_StreamEndPosition, GetStreamBlockSize());
                channel = GetChannelIterator();
                while ((channel = GetNextChannel(channel)) != 0)
                    memset((char*)channel->m_Buffer + m_StreamEndPosition, 0, end - m_StreamEndPosition);
                m_StreamEndPosition = end;
            }
            else
            {
                m_PlayIteration++;
            }
        }
        else
        {
            AudioStreamChannel* channel = GetChannelIterator();
            while ((channel = GetNextChannel(channel)) != 0)
                QueueFullChannelRead(channel);
            m_StreamReadPosition += GetStreamBlockSize();
        }
        break;
    }
    case 2:
    case 3:
    case 5:
    case 7:
        break;
    }
}

bool AudioReadState::Pause()
{
    if (m_InternalState != 4)
        return false;
    AudioStreamChannel* channel = GetChannelIterator();
    while ((channel = GetNextChannel(channel)) != 0)
    {
        channel->m_PauseAddress = (channel->m_Voice->pb.addr.currentAddressHi << 16) | channel->m_Voice->pb.addr.currentAddressLo;
        AXSetVoiceState(channel->m_Voice, AX_VOICE_STOP);
    }
    m_InternalState = 7;
    return true;
}

bool AudioReadState::Resume()
{
    if (m_InternalState != 7)
        return false;
    AudioStreamChannel* channel = GetChannelIterator();
    while ((channel = GetNextChannel(channel)) != 0)
    {
        AXSetVoiceCurrentAddr(channel->m_Voice, channel->m_PauseAddress);
        AXSetVoiceState(channel->m_Voice, AX_VOICE_RUN);
    }
    m_InternalState = 4;
    return true;
}

void TrackAudioRead(AudioReadState* state, AsyncEntry* request)
{
    AudioReadQueueEntry* entry = gAudioReadQueueEntryPool.Allocate();
    entry->m_Request = request;
    nlRingAddEnd(&state->m_ReadQueue, entry);
}

void CancelAudioReads(AudioReadState* state)
{
    if (state->m_ReadQueue == 0)
        return;
    AudioReadQueueEntry* pending = 0;
    bool cancelled = false;
    while (state->m_ReadQueue != 0)
    {
        AudioReadQueueEntry* entry = nlRingGetStart(state->m_ReadQueue);
        if (cancelled || nlAsyncReadBusy(entry->m_Request))
        {
            nlRingRemoveStart(&state->m_ReadQueue);
            nlRingAddEnd(&pending, entry);
        }
        else
        {
            cancelled = true;
            nlCancelAsyncRead(entry->m_Request, OnAudioReadCancelled);
        }
    }
    state->m_ReadQueue = pending;
}

bool AudioStereoStreamSource::Prepare()
{
    return AudioReadState::Prepare();
}
