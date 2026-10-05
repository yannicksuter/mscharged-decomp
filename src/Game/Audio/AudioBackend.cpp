#include "Game/Audio/AudioBackend.h"
#include "Game/Audio/AudioSource.h"
#include "Game/Audio/AudioResourcePlatform.h"

#include "Game/Audio/AudioEffect.h"
#include "Game/Audio/Delay.h"
#include "Game/Audio/AudioSystem.h"
#include "NL/nlMemory.h"
#include "NL/nlFileGC.h"
#include "NL/plat/PlatPadManager.h"
#include "revolution/ai.h"
#include "revolution/ax.h"
#include "revolution/mix.h"
#include "revolution/os.h"
#include "revolution/wpad.h"

#include <string.h>
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/Audio/RegistryPools.h"

static bool sDoubleMixUpdate = true;

AudioSource* g_pLastCreatedAudioSource;
u32 gAudioMemorySize;
void* g_pAudioSilenceBuffer;

void OnControllerSpeakerEnabled(WPADChannel chan, WPADResult result);
void OnControllerSpeakerReady(WPADChannel chan, WPADResult result);
void OnAudioControllerConnection(int chan, int deviceType, int state);
void ServiceControllerSpeakers(OSAlarm*, OSContext*);

AudioBackend::AudioBackend()
    : m_Sources(64, 16)
    , m_OutputMode(-1)
    , m_MixControllerSpeakersToMain(true)
{
    memset(m_ControllerSpeakerFlags, 0, sizeof(m_ControllerSpeakerFlags));
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
        AXFXReverbHiShutdownDpl2(&m_ReverbEffect.m_ReverbDpl2);
        AXFXDelayExpShutdownDpl2(&m_DelayEffect.m_DelayDpl2);
    }
    else
    {
        AXFXReverbHiShutdown(&m_ReverbEffect.m_Reverb);
        AXFXDelayShutdown(&m_DelayEffect.m_Delay);
    }
}

static inline void* CreateSilenceBuffer()
{
    void* buffer = nlMalloc(0x500, 32, false);
    memset(buffer, 0, 0x500);
    return buffer;
}

bool AudioBackend::Initialize()
{
    AIInit(0);
    AXInit();
    MIXInit();
    AXRegisterCallback(UpdateAudioSources);

    g_pAudioSilenceBuffer = CreateSilenceBuffer();

    AllocatorStack[AllocatorStackDepth++] = &VirtualAllocator;
    CurrentAllocator = &VirtualAllocator;
    void* memory = nlMalloc(gAudioMemorySize, 8, false);
    --AllocatorStackDepth;
    AllocatorStack[AllocatorStackDepth] = 0;
    CurrentAllocator = AllocatorStack[AllocatorStackDepth - 1];
    m_AudioAllocator.Initialize(memory, gAudioMemorySize);
    InitializeAudioStreamBlockPool();
    SetOutputMode(1);

    OSCreateAlarm(&m_ControllerSpeakerAlarm);
    u32 ticks = OSNanosecondsToTicks(6666667);
    OSSetPeriodicAlarm(&m_ControllerSpeakerAlarm, ticks, ticks, ServiceControllerSpeakers);
    memset(m_ControllerSpeakerEncoders, 0, sizeof(m_ControllerSpeakerEncoders));
    for (int chan = 0; chan < 4; ++chan)
        OnAudioControllerConnection(chan, 0, 0);
    g_pPlatPadManager->deviceChanged.Add(OnAudioControllerConnection, 0, -1);
    InitializeAuxEffects();
    return true;
}

void AudioBackend::SuspendControllerSpeakers()
{
    OSCancelAlarm(&m_ControllerSpeakerAlarm);
    for (int i = 0; i < 4; ++i)
        m_ControllerSpeakerFlags[i] = (m_ControllerSpeakerFlags[i] & 0x7FFFFFFF) | 0x40000000;
}

void AudioBackend::ResumeControllerSpeakers()
{
    for (int i = 0; i < 4; ++i)
    {
        if (WPADProbe(i, 0) == WPAD_ERR_OK)
            WPADControlSpeaker(i, 1, OnControllerSpeakerEnabled);
    }
    OSCreateAlarm(&m_ControllerSpeakerAlarm);
    u32 ticks = OSNanosecondsToTicks(6666667);
    OSSetPeriodicAlarm(&m_ControllerSpeakerAlarm, ticks, ticks, ServiceControllerSpeakers);
}

void OnAudioControllerConnection(int chan, int, int state)
{
    AudioBackend* self = g_pAudioBackend;
    if (state == 1 || state == 2)
    {
        WPADControlSpeaker(chan, 1, OnControllerSpeakerEnabled);
    }
    else
    {
        self->m_ControllerSpeakerFlags[chan] = (self->m_ControllerSpeakerFlags[chan] & 0x7FFFFFFF) | 0x40000000;
        memset(&self->m_ControllerSpeakerEncoders[chan], 0, sizeof(WENCInfo));
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
        self->m_ControllerSpeakerFlags[chan] |= 0xC0000000;
        WPADControlSpeaker(chan, 2, 0);
    }
}

void AudioBackend::Shutdown()
{
    gAudioSampleSourcePool.FreeBlocks();
    gAudioMonoStreamSourcePool.FreeBlocks();
    gAudioStereoStreamSourcePool.FreeBlocks();
    gAudioReadQueueEntryPool.FreeBlocks();
    if (m_OutputMode == 3)
    {
        AXFXReverbHiShutdownDpl2(&m_ReverbEffect.m_ReverbDpl2);
        AXFXDelayExpShutdownDpl2(&m_DelayEffect.m_DelayDpl2);
    }
    else
    {
        AXFXReverbHiShutdown(&m_ReverbEffect.m_Reverb);
        AXFXDelayShutdown(&m_DelayEffect.m_Delay);
    }
    BasicSlotPool<ListEntry<AudioSource*> >* sourcePool = &m_Sources.m_Allocator;
    sourcePool->FreeBlocks();
}

void* AudioBackend::AllocateAudioMemory(unsigned long size)
{
    void* pointer = m_AudioAllocator.Allocate(size, 32, false);
    if (pointer == 0)
    {
        DumpAudioBankMemory("BankUsage.txt");
        DumpAudioSystem(g_pAudioSystem, "AudioDump.txt");
    }
    return pointer;
}

void AudioBackend::FreeAudioMemory(void* pointer)
{
    m_AudioAllocator.Free(pointer);
}

void AudioBackend::ServiceReadQueue(float)
{
    bool enabled = OSDisableInterrupts();
    while (m_ReadQueue.m_Head != 0)
    {
        AudioReadRequest* request = m_ReadQueue.GetHead();
        if (request->m_Cancel)
        {
            CancelAudioReads(request->m_State);
        }
        else
        {
            nlSeek(request->m_File, request->m_Offset, 0);
            AsyncEntry* result = nlReadAsync(request->m_File, request->m_Buffer, request->m_Size, request->m_Callback, request->m_UserParam, 0);
            TrackAudioRead(request->m_State, result);
        }
        m_ReadQueue.DeleteEntry(m_ReadQueue.RemoveStart());
    }
    OSRestoreInterrupts(enabled);
}

void UpdateAudioSources()
{
    AudioBackend* self = g_pAudioBackend;
    MIXUpdateSettings();
    if (sDoubleMixUpdate)
        MIXUpdateSettings();
    for (ListEntry<AudioSource*>* entry = self->m_Sources.m_Head;
        entry != 0;
        entry = entry->next)
    {
        entry->entry->Update();
    }
}

AudioSource* AudioBackend::CreateSource(AudioSourceInfo* info, XSoundOwner*)
{
    AudioSource* source = 0;
    if (info->m_BankLoader->m_Chunk23200->m_IsStream != 0)
    {
        if (info->m_ChannelCount == 1)
            source = new AudioMonoStreamSource;
        else if (info->m_ChannelCount == 2)
            source = new AudioStereoStreamSource;
    }
    else
    {
        source = new AudioSampleSource;
    }
    source->Initialize(info);
    bool enabled = OSDisableInterrupts();
    m_Sources.AddEnd(source);
    OSRestoreInterrupts(enabled);
    g_pLastCreatedAudioSource = source;
    return source;
}

void AudioBackend::ReleaseSource(AudioSource* source)
{
    bool enabled = OSDisableInterrupts();
    m_Sources.RemoveEntry(source);
    OSRestoreInterrupts(enabled);
    delete source;
}

void AudioBackend::QueueRead(nlFile* file, unsigned int offset,
    void* buffer, unsigned int size, ReadAsyncCallback callback,
    unsigned long userParam, AudioReadState* state)
{
    bool enabled = OSDisableInterrupts();
    AudioReadRequest* request = m_ReadQueue.AllocateAtEnd(0);
    request->m_File = file;
    request->m_Offset = offset;
    request->m_Buffer = buffer;
    request->m_Size = size;
    request->m_Callback = callback;
    request->m_UserParam = userParam;
    request->m_State = state;
    request->m_Cancel = false;
    OSRestoreInterrupts(enabled);
}

void AudioBackend::QueueReadCancellation(AudioReadState* state)
{
    bool enabled = OSDisableInterrupts();
    AudioReadRequest* request = m_ReadQueue.AllocateAtEnd(0);
    request->m_Cancel = true;
    request->m_State = state;
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

void AudioBackend::InitializeAuxEffects()
{
    AXFXSetHooks(AllocateAudioEffectMemory, FreeAudioEffectMemory);
    if (g_pAuxEffectMap == 0)
    {
        void* storage = nlMalloc(sizeof(AuxEffectMap), 8, false);
        new (storage) AuxEffectMap;
        g_pAuxEffectMap = static_cast<AuxEffectMap*>(storage);
    }
    if (m_OutputMode == 3)
    {
        g_pAuxEffectMap->AssignAuxiliary(1);
        SetDefaultReverbSettings(&m_ReverbEffect.m_Reverb);
        AXFXReverbHiInitDpl2(&m_ReverbEffect.m_ReverbDpl2);
        g_pAuxEffectMap->AssignAuxiliary(0);
        SetDefaultDelaySettings(&m_DelayEffect.m_Delay);
        AXFXDelayExpInitDpl2(&m_DelayEffect.m_DelayDpl2);
    }
    else
    {
        g_pAuxEffectMap->AssignAuxiliary(1);
        SetDefaultReverbSettings(&m_ReverbEffect.m_Reverb);
        AXFXReverbHiInit(&m_ReverbEffect.m_Reverb);
        g_pAuxEffectMap->AssignAuxiliary(0);
        SetDefaultDelaySettings(&m_DelayEffect.m_Delay);
        AXFXDelayInit(&m_DelayEffect.m_Delay);
    }
}

void ServiceControllerSpeakers(OSAlarm*, OSContext*)
{
    AudioBackend* self = g_pAudioBackend;
    s16 samples[40] = { 0 };
    u8 encoded[20];
    u32* speakerState = self->m_ControllerSpeakerFlags;
    WENCInfo* encoder = self->m_ControllerSpeakerEncoders;
    int chan;
    bool advance = false;
    for (chan = 0; chan < 4; ++chan, ++speakerState, ++encoder)
    {
        if (AXRmtGetSamples(chan, samples, 40) != 40)
            continue;
        advance = true;
        if ((*speakerState >> 31) == 0)
            continue;
        BOOL enabled = OSDisableInterrupts();
        if (WPADCanSendStreamData(chan))
        {
            bool reuse = !((*speakerState >> 30) & 1);
            *speakerState &= ~0x40000000;
            WENCGetEncodeData(encoder, reuse, samples, 40, encoded);
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
