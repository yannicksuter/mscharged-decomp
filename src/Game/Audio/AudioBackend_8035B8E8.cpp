#include "Game/Audio/AudioBackend_8035B8E8.h"
#include "Game/Audio/AudioSource_8035EF08.h"

#include "Game/Audio/AudioEffect_802F98F4.h"
#include "Game/Audio/AudioLoadMode_806E201C.h"
#include "NL/nlMemory.h"
#include "revolution/ax.h"
#include "revolution/mix.h"
#include "revolution/os.h"
#include "revolution/wpad.h"

#include <string.h>

static bool lbl_806DF9A8 = true;
static const int lbl_806DF9AC = 1;
static const int lbl_806DF9B0 = 0;
static const int lbl_806DF9B4 = 1;
static const int lbl_806DF9B8 = 0;

AudioSource_8035C234* lbl_806E21E0;
u32 lbl_806E21E4;
void* lbl_806E21E8;

extern AuxEffectMap_8035952C* lbl_806E21B8;

extern "C" void fn_8035DEC4(const char* path);
extern "C" void fn_802ECA78(AudioLoadMode_806E201C*, const char* path);
extern "C" void fn_803638BC(AXFX_REVERBHI*);
extern void fn_8035A3EC(AXFX_DELAY*);
extern void nlSeek(nlFile*, unsigned int, unsigned long);

extern "C" void fn_8035BF90(WPADChannel chan, WPADResult result);
extern "C" void fn_8035BFAC(WPADChannel chan, WPADResult result);
extern "C" void fn_8035C988(OSAlarm*, OSContext*);

extern "C" void* fn_8035D568(unsigned long size)
{
    return nlMalloc(size, 8, false);
}

extern "C" void fn_8035D574(void* pointer)
{
    nlFree(pointer);
}

AudioBackend_8035B8E8::AudioBackend_8035B8E8()
    : m_Unknown004(64, 16)
    , m_OutputMode(-1)
    , m_Unknown450(true)
{
    memset(m_Unknown698, 0, sizeof(m_Unknown698));
}

AudioBackend_8035B8E8::~AudioBackend_8035B8E8()
{
    delete lbl_806E21B8;
    lbl_806E21B8 = 0;
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

void AudioBackend_8035B8E8::fn_8035BE04()
{
    OSCancelAlarm(&m_Unknown668);
    for (int i = 0; i < 4; ++i)
        m_Unknown698[i] = (m_Unknown698[i] & 0x7FFFFFFF) | 0x40000000;
}

void AudioBackend_8035B8E8::fn_8035BE74()
{
    for (int i = 0; i < 4; ++i)
    {
        if (WPADProbe(i, 0) == WPAD_ERR_OK)
            WPADControlSpeaker(i, 1, fn_8035BF90);
    }
    OSCreateAlarm(&m_Unknown668);
    u32 ticks = OSNanosecondsToTicks(6666667);
    OSSetPeriodicAlarm(&m_Unknown668, ticks, ticks, fn_8035C988);
}

extern "C" void fn_8035BF40(WPADChannel chan, int, int state)
{
    AudioBackend_8035B8E8* self = lbl_806E2020;
    if (state == 1 || state == 2)
    {
        WPADControlSpeaker(chan, 1, fn_8035BF90);
    }
    else
    {
        self->m_Unknown698[chan] = (self->m_Unknown698[chan] & 0x7FFFFFFF) | 0x40000000;
        memset(&self->m_Unknown6A8[chan], 0, sizeof(WENCInfo));
    }
}

extern "C" void fn_8035BF90(WPADChannel chan, WPADResult result)
{
    if (result == WPAD_ERR_OK)
        WPADControlSpeaker(chan, 4, fn_8035BFAC);
}

extern "C" void fn_8035BFAC(WPADChannel chan, WPADResult result)
{
    AudioBackend_8035B8E8* self = lbl_806E2020;
    if (result == WPAD_ERR_OK)
    {
        self->m_Unknown698[chan] |= 0xC0000000;
        WPADControlSpeaker(chan, 2, 0);
    }
}

void* AudioBackend_8035B8E8::fn_8035C0B4(unsigned long size)
{
    void* pointer = m_Unknown434.Allocate(size, 32, false);
    if (pointer == 0)
    {
        fn_8035DEC4("BankUsage.txt");
        fn_802ECA78(lbl_806E201C, "AudioDump.txt");
    }
    return pointer;
}

void AudioBackend_8035B8E8::fn_8035C114(void* pointer)
{
    m_Unknown434.Free(pointer);
}

void AudioBackend_8035B8E8::fn_8035C11C()
{
    bool enabled = OSDisableInterrupts();
    while (m_Unknown024.m_Head != 0)
    {
        AudioRead_8035C620* request = &m_Unknown024.m_Head->entry;
        if (request->m_Unknown1B)
        {
            fn_80361308(request->m_Unknown14);
        }
        else
        {
            nlSeek(request->m_Unknown00, request->m_Unknown04, 0);
            AsyncEntry* result = nlReadAsync(request->m_Unknown00, request->m_Unknown08, request->m_Unknown18, request->m_Unknown0C, request->m_Unknown10, 0);
            fn_80361258(request->m_Unknown14, result);
        }
        m_Unknown024.Deallocate(m_Unknown024.RemoveStart(), 0);
    }
    OSRestoreInterrupts(enabled);
}

extern "C" void fn_8035C234()
{
    AudioBackend_8035B8E8* self = lbl_806E2020;
    MIXUpdateSettings();
    if (lbl_806DF9A8)
        MIXUpdateSettings();
    for (ListEntry<AudioSource_8035C234*>* entry = self->m_Unknown004.m_Head;
        entry != 0;
        entry = entry->next)
    {
        entry->entry->fn_44();
    }
}

void AudioBackend_8035B8E8::fn_8035C51C(AudioSource_8035C234* source)
{
    bool enabled = OSDisableInterrupts();
    m_Unknown004.RemoveEntry(source);
    OSRestoreInterrupts(enabled);
    delete source;
}

void AudioBackend_8035B8E8::fn_8035C620(nlFile* file, unsigned int offset,
    void* buffer, unsigned int size, ReadAsyncCallback callback,
    unsigned long userParam, AudioReadState_80361258* state)
{
    bool enabled = OSDisableInterrupts();
    AudioRead_8035C620 request;
    request.m_Unknown00 = file;
    request.m_Unknown04 = offset;
    request.m_Unknown08 = buffer;
    request.m_Unknown0C = callback;
    request.m_Unknown10 = userParam;
    request.m_Unknown14 = state;
    request.m_Unknown18 = size;
    request.m_Unknown1B = false;
    m_Unknown024.AddEnd(request);
    OSRestoreInterrupts(enabled);
}

void AudioBackend_8035B8E8::fn_8035C734(AudioReadState_80361258* state)
{
    bool enabled = OSDisableInterrupts();
    AudioRead_8035C620 request;
    request.m_Unknown14 = state;
    request.m_Unknown1B = true;
    m_Unknown024.AddEnd(request);
    OSRestoreInterrupts(enabled);
}

void AudioBackend_8035B8E8::fn_8035C818(unsigned int mode)
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

void AudioBackend_8035B8E8::fn_8035C894()
{
    AXFXSetHooks(fn_8035D568, fn_8035D574);
    if (lbl_806E21B8 == 0)
        lbl_806E21B8 = new (8, false) AuxEffectMap_8035952C;
    if (m_OutputMode == 3)
    {
        lbl_806E21B8->fn_80359544(lbl_806DF9AC);
        fn_803638BC(&m_Unknown454.m_Reverb);
        AXFXReverbHiInitDpl2(&m_Unknown454.m_ReverbDpl2);
        lbl_806E21B8->fn_80359544(lbl_806DF9B0);
        fn_8035A3EC(&m_DelayEffect.m_Delay);
        AXFXDelayExpInitDpl2(&m_DelayEffect.m_DelayDpl2);
    }
    else
    {
        lbl_806E21B8->fn_80359544(lbl_806DF9B4);
        fn_803638BC(&m_Unknown454.m_Reverb);
        AXFXReverbHiInit(&m_Unknown454.m_Reverb);
        lbl_806E21B8->fn_80359544(lbl_806DF9B8);
        fn_8035A3EC(&m_DelayEffect.m_Delay);
        AXFXDelayInit(&m_DelayEffect.m_Delay);
    }
}

extern "C" void fn_8035C988(OSAlarm*, OSContext*)
{
    s16 samples[40] = { 0 };
    u8 encoded[20];
    AudioBackend_8035B8E8* self = lbl_806E2020;
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

extern "C" void fn_8035CA84()
{
    fn_8035DEC4("BankUsage.txt");
    fn_802ECA78(lbl_806E201C, "AudioDump.txt");
}
