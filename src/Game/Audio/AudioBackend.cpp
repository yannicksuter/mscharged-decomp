#include "Game/Audio/AudioBackend.h"
#include "Game/Audio/AudioStreamSource.h"
#include "Game/Audio/AudioResourcePlatform.h"

#include "Game/Audio/AudioEffect.h"
#include "Game/Audio/AudioSystem.h"
#include "NL/nlMemory.h"
#include "NL/nlFileGC.h"
#include "revolution/ax.h"
#include "revolution/mix.h"
#include "revolution/os.h"
#include "revolution/wpad.h"

#include <string.h>
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/Audio/RegistryPools.h"

static bool sDoubleMixUpdate = true;
static const int sDpl2ReverbAuxiliary = 1;
static const int sDpl2DelayAuxiliary = 0;
static const int sReverbAuxiliary = 1;
static const int sDelayAuxiliary = 0;

AudioSource* g_pAudioSourceList;
u32 gAudioSourceListCount;
void* g_pAudioSilenceBuffer;

void OnControllerSpeakerEnabled(WPADChannel chan, WPADResult result);
void OnControllerSpeakerReady(WPADChannel chan, WPADResult result);
void ServiceControllerSpeakers(OSAlarm*, OSContext*);

void* AllocateAudioEffectMemory(unsigned long size)
{
    return nlMalloc(size, 8, false);
}

void FreeAudioEffectMemory(void* pointer)
{
    nlFree(pointer);
}

AudioBackend::AudioBackend()
    : m_Unknown004(64, 16)
    , m_OutputMode(-1)
    , m_Unknown450(true)
{
    memset(m_Unknown698, 0, sizeof(m_Unknown698));
}

AudioBackend::~AudioBackend()
{
    if (g_pAuxEffectMap != 0)
    {
        delete g_pAuxEffectMap;
        g_pAuxEffectMap = 0;
    }
    if (m_OutputMode == 3)
    {
        AXFXReverbHiShutdownDpl2(&m_Unknown454.m_ReverbDpl2);
        AXFXDelayExpShutdownDpl2(&m_DelayEffect.m_DelayDpl2);
    }
    else
    {
        AXFXReverbHiShutdown(&m_Unknown454.m_Reverb);
        AXFXDelayShutdown(&m_DelayEffect.m_Delay);
    }
}

void AudioBackend::SuspendControllerSpeakers()
{
    OSCancelAlarm(&m_Unknown668);
    for (int i = 0; i < 4; ++i)
        m_Unknown698[i] = (m_Unknown698[i] & 0x7FFFFFFF) | 0x40000000;
}

void AudioBackend::ResumeControllerSpeakers()
{
    for (int i = 0; i < 4; ++i)
    {
        if (WPADProbe(i, 0) == WPAD_ERR_OK)
            WPADControlSpeaker(i, 1, OnControllerSpeakerEnabled);
    }
    OSCreateAlarm(&m_Unknown668);
    u32 ticks = OSNanosecondsToTicks(6666667);
    OSSetPeriodicAlarm(&m_Unknown668, ticks, ticks, ServiceControllerSpeakers);
}

void OnAudioControllerConnection(WPADChannel chan, int, int state)
{
    AudioBackend* self = g_pAudioBackend;
    if (state == 1 || state == 2)
    {
        WPADControlSpeaker(chan, 1, OnControllerSpeakerEnabled);
    }
    else
    {
        self->m_Unknown698[chan] = (self->m_Unknown698[chan] & 0x7FFFFFFF) | 0x40000000;
        memset(&self->m_Unknown6A8[chan], 0, sizeof(WENCInfo));
    }
}

void OnControllerSpeakerEnabled(WPADChannel chan, WPADResult result)
{
    if (result == WPAD_ERR_OK)
        WPADControlSpeaker(chan, 4, OnControllerSpeakerReady);
}

void OnControllerSpeakerReady(WPADChannel chan, WPADResult result)
{
    AudioBackend* self = g_pAudioBackend;
    if (result == WPAD_ERR_OK)
    {
        self->m_Unknown698[chan] |= 0xC0000000;
        WPADControlSpeaker(chan, 2, 0);
    }
}

void* AudioBackend::AllocateAudioMemory(unsigned long size)
{
    void* pointer = m_Unknown434.Allocate(size, 32, false);
    if (pointer == 0)
    {
        DumpAudioBankMemory("BankUsage.txt");
        DumpAudioSystem(g_pAudioSystem, "AudioDump.txt");
    }
    return pointer;
}

void AudioBackend::FreeAudioMemory(void* pointer)
{
    m_Unknown434.Free(pointer);
}

void AudioReadState::SetInputVolume(float value)
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
        if (channel->m_Unknown04 != 0)
            SetVoiceInputVolume(channel->m_Unknown04, value);
    }
    OSRestoreInterrupts(enabled);
}

void AudioBackend::ServiceReadQueue(float)
{
    bool enabled = OSDisableInterrupts();
    while (m_Unknown024.m_Head != 0)
    {
        AudioRead* request = m_Unknown024.GetHead();
        if (request->m_Unknown1B)
        {
            CancelAudioReads(request->m_Unknown14);
        }
        else
        {
            nlSeek(request->m_Unknown00, request->m_Unknown04, 0);
            AsyncEntry* result = nlReadAsync(request->m_Unknown00, request->m_Unknown08, request->m_Unknown18, request->m_Unknown0C, request->m_Unknown10, 0);
            TrackAudioRead(request->m_Unknown14, result);
        }
        m_Unknown024.Deallocate(m_Unknown024.RemoveStart(), 0);
    }
    OSRestoreInterrupts(enabled);
}

void UpdateAudioSources()
{
    AudioBackend* self = g_pAudioBackend;
    MIXUpdateSettings();
    if (sDoubleMixUpdate)
        MIXUpdateSettings();
    for (ListEntry<AudioSource*>* entry = self->m_Unknown004.m_Head;
        entry != 0;
        entry = entry->next)
    {
        entry->entry->Update();
    }
}

AudioSource* AudioBackend::CreateSource(AudioSourceInfo* info, XSoundOwner*)
{
    AudioSource* source = 0;
    if (info->m_Unknown18->m_Unknown10->m_Unknown08 != 0)
    {
        if (info->m_Unknown10 == 1)
            source = new AudioReadState_8035D154;
        else if (info->m_Unknown10 == 2)
            source = new AudioReadState_80361920;
    }
    else
    {
        source = new AudioSampleSource;
    }
    source->Initialize(info);
    bool enabled = OSDisableInterrupts();
    m_Unknown004.AddEnd(source);
    OSRestoreInterrupts(enabled);
    g_pAudioSourceList = source;
    return source;
}

void AudioBackend::ReleaseSource(AudioSource* source)
{
    bool enabled = OSDisableInterrupts();
    m_Unknown004.RemoveEntry(source);
    OSRestoreInterrupts(enabled);
    delete source;
}

void AudioReadState::SetPitch(float value)
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
        if (channel->m_Unknown04 != 0)
            SetVoicePitch(channel->m_Unknown04, m_Unknown0C, value);
        OSRestoreInterrupts(enabled);
    }
}

void AudioBackend::QueueRead(nlFile* file, unsigned int offset,
    void* buffer, unsigned int size, ReadAsyncCallback callback,
    unsigned long userParam, AudioReadState* state)
{
    bool enabled = OSDisableInterrupts();
    AudioRead* request = m_Unknown024.AllocateAtEnd(0);
    request->m_Unknown00 = file;
    request->m_Unknown04 = offset;
    request->m_Unknown08 = buffer;
    request->m_Unknown18 = size;
    request->m_Unknown0C = callback;
    request->m_Unknown10 = userParam;
    request->m_Unknown14 = state;
    request->m_Unknown1B = false;
    OSRestoreInterrupts(enabled);
}

void AudioReadState::SetMixVolume(float value)
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
        if (channel->m_Unknown04 != 0)
            SetVoiceMixVolume(channel->m_Unknown04, value);
    }
    OSRestoreInterrupts(enabled);
}

void AudioBackend::QueueReadCancellation(AudioReadState* state)
{
    bool enabled = OSDisableInterrupts();
    AudioRead* request = m_Unknown024.AllocateAtEnd(0);
    request->m_Unknown1B = true;
    request->m_Unknown14 = state;
    OSRestoreInterrupts(enabled);
}

void AudioReadState::SetLowPassFilter(bool on, unsigned int frequency, bool unchanged)
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
        if (channel->m_Unknown04 != 0)
            SetVoiceLowPassFilter(channel->m_Unknown04, on, frequency, unchanged);
    }
    OSRestoreInterrupts(enabled);
}

void AudioBackend::SetOutputMode(unsigned int mode)
{
    if (mode == m_OutputMode)
        return;
    m_OutputMode = mode;
    u32 axMode;
    u32 mixMode;
    switch (mode)
    {
    case 0:
        axMode = 0;
        mixMode = 0;
        break;
    case 1:
    case 2:
        axMode = 0;
        mixMode = 1;
        break;
    case 3:
        axMode = 2;
        mixMode = 3;
        break;
    }
    AXSetMode(axMode);
    MIXSetSoundMode(mixMode);
}

void AudioReadState::SetSurroundPan(float value)
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
        if (channel->m_Unknown04 != 0)
            SetVoiceSurroundPan(channel->m_Unknown04, value);
    }
    OSRestoreInterrupts(enabled);
}

void AudioBackend::InitializeAuxEffects()
{
    AXFXSetHooks(AllocateAudioEffectMemory, FreeAudioEffectMemory);
    if (g_pAuxEffectMap == 0)
        g_pAuxEffectMap = new (8, false) AuxEffectMap;
    if (m_OutputMode == 3)
    {
        g_pAuxEffectMap->AssignAuxiliary(sDpl2ReverbAuxiliary);
        SetDefaultReverbSettings(&m_Unknown454.m_Reverb);
        AXFXReverbHiInitDpl2(&m_Unknown454.m_ReverbDpl2);
        g_pAuxEffectMap->AssignAuxiliary(sDpl2DelayAuxiliary);
        SetDefaultDelaySettings(&m_DelayEffect.m_Delay);
        AXFXDelayExpInitDpl2(&m_DelayEffect.m_DelayDpl2);
    }
    else
    {
        g_pAuxEffectMap->AssignAuxiliary(sReverbAuxiliary);
        SetDefaultReverbSettings(&m_Unknown454.m_Reverb);
        AXFXReverbHiInit(&m_Unknown454.m_Reverb);
        g_pAuxEffectMap->AssignAuxiliary(sDelayAuxiliary);
        SetDefaultDelaySettings(&m_DelayEffect.m_Delay);
        AXFXDelayInit(&m_DelayEffect.m_Delay);
    }
}

void ServiceControllerSpeakers(OSAlarm*, OSContext*)
{
    s16 samples[40] = { 0 };
    u8 encoded[20];
    AudioBackend* self = g_pAudioBackend;
    bool advance = false;
    for (int chan = 0; chan < 4; ++chan)
    {
        if (AXRmtGetSamples(chan, samples, 40) != 40)
            continue;
        advance = true;
        if ((self->m_Unknown698[chan] & 0x80000000) == 0)
            continue;
        BOOL enabled = OSDisableInterrupts();
        if (WPADCanSendStreamData(chan))
        {
            bool reuse = (self->m_Unknown698[chan] & 0x40000000) == 0;
            self->m_Unknown698[chan] &= ~0x40000000;
            WENCGetEncodeData(&self->m_Unknown6A8[chan], reuse, samples, 40, encoded);
            WPADSendStreamData(chan, encoded, 20);
        }
        OSRestoreInterrupts(enabled);
    }
    if (advance)
        AXRmtAdvancePtr(40);
}

void DumpAudioMemory()
{
    DumpAudioBankMemory("BankUsage.txt");
    DumpAudioSystem(g_pAudioSystem, "AudioDump.txt");
}

bool AudioSource::IsResident()
{
    return true;
}

bool AudioSource::IsStream()
{
    return false;
}

bool AudioSource::IsLooping()
{
    return m_Unknown14_00 == 0xFFFF;
}

void AudioReadState::SetAuxiliaryVolume(int auxiliary, int value)
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
        if (channel->m_Unknown04 != 0)
            SetVoiceAuxiliaryVolume(channel->m_Unknown04, auxiliary, value);
    }
    OSRestoreInterrupts(enabled);
}
