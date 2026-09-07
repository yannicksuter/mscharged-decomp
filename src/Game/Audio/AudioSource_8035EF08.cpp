#include "Game/Audio/AudioSource_8035EF08.h"

#include "Game/Audio/AudioBackend_8035B8E8.h"
#include "Game/Audio/Plat3dSoundSrc.h"
#include "NL/nlFileGC.h"
#include "NL/nlMath.h"
#include "NL/nlRing.h"
#include "revolution/mix.h"
#include "revolution/os/OSInterrupt.h"

unsigned int lbl_806E2210;
unsigned int lbl_806E2214;
unsigned int lbl_806E2218;
unsigned int lbl_806E221C;
unsigned int lbl_806E2220;
unsigned int lbl_806E2224;
AXPBLPF lbl_806E2230;

SlotPool<AudioSampleSource_8035F474> lbl_80585C20(64, 16);
SlotPool<AudioReadQueueEntry_80361258> lbl_80585C98(32, 16);

extern void* lbl_806E21E8;
extern "C" void fn_8035CA84();
extern "C" AsyncEntry* fn_803675D4();

extern "C" void fn_8035EF08(AXVPB* voice, float value)
{
    MIXSetInput(voice, (int)(10.0f * value));
}

extern "C" void fn_8035EF3C(AXVPB* voice, float value)
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

extern "C" void fn_8035F06C(AXVPB* voice, float ratio, float value)
{
    float pitch = nlFastExp2(value * nlFastLog2(1.0594631f));
    AXSetVoiceSrcRatio(voice, pitch * ratio);
}

extern "C" void fn_8035F0D8(AXVPB* voice, float value)
{
    MIXSetPan(voice, ((int)(127.0f * value) - 1) / 2 + 64);
}

extern "C" void fn_8035F120(AXVPB* voice, float value)
{
    MIXSetSPan(voice, lbl_806E2020->m_OutputMode == 3
            ? ((int)(127.0f * value) - 1) / 2 + 64
            : 127);
}

extern "C" void fn_8035F180(AXVPB* voice, int value)
{
    u16 rShift = value <= 0 ? 0 : value;
    u16 lShift = -value <= 0 ? 0 : -value;
    AXSetVoiceItdTarget(voice, lShift, rShift);
}

extern "C" void fn_8035F1AC(AXVPB* voice, bool enabled, unsigned int frequency, bool unchanged)
{
    if (enabled && !unchanged)
        AXGetLpfCoefs(frequency, &lbl_806E2230.a0, &lbl_806E2230.b0);
    if (voice->pb.lpf.on && enabled)
    {
        AXSetVoiceLpfCoefs(voice, lbl_806E2230.a0, lbl_806E2230.b0);
    }
    else
    {
        lbl_806E2230.on = enabled;
        lbl_806E2230.yn1 = 0;
        AXSetVoiceLpf(voice, &lbl_806E2230);
    }
}

extern "C" void fn_8035F24C(AXVPB* voice, int auxiliary, int value)
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

void AudioSource_8035C234::fn_3C(Plat3dSoundSrc* source)
{
    if (fn_64())
    {
        fn_4C(source->m_Unknown20);
        fn_50(source->m_Unknown24);
        fn_54((int)source->m_Unknown44 >> 24);
    }
}

void AudioSource_8035C234::fn_0C()
{
    if (m_Unknown10 == 5)
    {
        m_Unknown04 = 3;
    }
    else
    {
        m_Unknown04 = m_Unknown10;
    }
    if (m_Unknown04 == 6 && fn_64() && !fn_60())
        fn_6C(true);
}

void AudioSource_8035C234::fn_8035F3AC(bool enabled, unsigned int channel)
{
    m_Unknown14_18 = enabled;
    m_Unknown14_19 = channel;
    AXVPB* voice = fn_68();
    MIXRmtSetVolumes(voice, 0,
        m_Unknown14_19 == 0 ? 0 : -960,
        m_Unknown14_19 == 1 ? 0 : -960,
        m_Unknown14_19 == 2 ? 0 : -960,
        m_Unknown14_19 == 3 ? 0 : -960,
        -960, -960, -960, -960);
    AXSetVoiceRmtOn(voice, true);
}

AudioSampleSource_8035F474::AudioSampleSource_8035F474()
    : m_Unknown18(0)
    , m_Unknown1C(0)
    , m_Unknown20(0)
    , m_Unknown24(0)
    , m_Unknown28(false)
{
    ++lbl_806E221C;
    ++lbl_806E2220;
}

void AudioSampleSource_8035F474::fn_40(AudioSourceInfo_8035C298* info)
{
    m_Unknown08 = info;
    m_Unknown20 = SPGetSoundEntry(info->m_Unknown18->m_Unknown18.m_SoundTable, info->m_Unknown00);
    m_Unknown1C = AXAcquireVoice(15, fn_8035F96C, (unsigned long)this);
    if (m_Unknown1C == 0)
        fn_8035CA84();
    SPPrepareSound(m_Unknown20, m_Unknown1C, m_Unknown20->sampleRate);
    m_Unknown0C = (float)m_Unknown20->sampleRate / 32000.0f;
    MIXInitChannel(m_Unknown1C, 0, 0, -960, -960, -960, 64, 127, 0);
    m_Unknown10 = 1;
    m_Unknown04 = 1;
}

inline void AudioSampleSource_8035F474::operator delete(void* pointer)
{
    lbl_80585C20.Free((AudioSampleSource_8035F474*)pointer);
}

AudioSampleSource_8035F474::~AudioSampleSource_8035F474()
{
    fn_6C(true);
    --lbl_806E221C;
    --lbl_806E2220;
}

void AudioSampleSource_8035F474::fn_44()
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
                    AXSetVoiceLoopAddr(m_Unknown1C, (unsigned long)lbl_806E21E8 * 2);
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

bool AudioSampleSource_8035F474::fn_20(unsigned int value)
{
    m_Unknown14_00 = value;
    m_Unknown10 = 5;
    return true;
}

void AudioSampleSource_8035F474::fn_24()
{
    switch (m_Unknown10)
    {
    case 4:
    {
        bool enabled = OSDisableInterrupts();
        if (m_Unknown1C != 0)
        {
            AXSetVoiceState(m_Unknown1C, AX_VOICE_STOP);
            AXSetVoiceLoopAddr(m_Unknown1C, (unsigned long)lbl_806E21E8 * 2);
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

bool AudioSampleSource_8035F474::fn_28()
{
    m_Unknown24 = ((m_Unknown1C->pb.addr.currentAddressHi << 16) | m_Unknown1C->pb.addr.currentAddressLo) + 320;
    AXSetVoiceEndAddr(m_Unknown1C, m_Unknown24);
    AXSetVoiceLoop(m_Unknown1C, false);
    m_Unknown10 = 7;
    m_Unknown04 = 7;
    return true;
}

bool AudioSampleSource_8035F474::fn_2C()
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

void AudioSampleSource_8035F474::fn_8035F96C(void* pointer)
{
    AXVPB* voice = (AXVPB*)pointer;
    AudioSampleSource_8035F474* source = (AudioSampleSource_8035F474*)voice->userContext;
    source->m_Unknown28 = true;
    source->fn_6C(false);
    ++lbl_806E2210;
    source->m_Unknown10 = 6;
    source->m_Unknown04 = 6;
}

void AudioSampleSource_8035F474::fn_6C(bool release)
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

AudioStreamChannel_8035FA38::AudioStreamChannel_8035FA38()
{
    m_Unknown08 = 0;
    m_Unknown14 = 0;
    m_Unknown10_00 = 0;
    m_Unknown10_1F = 0;
}

AudioStreamChannel_8035FA38::~AudioStreamChannel_8035FA38()
{
    if (m_Unknown04 != 0)
    {
        AXSetVoiceState(m_Unknown04, AX_VOICE_STOP);
        MIXReleaseChannel(m_Unknown04);
        AXFreeVoice(m_Unknown04);
        m_Unknown04 = 0;
    }
    lbl_806E2020->fn_8035C114(m_Unknown08);
}

void AudioStreamChannel_8035FA38::fn_8035FAE0(AudioStreamHeader_8035FAE0* header)
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
    AXPBADPCM adpcm = header->m_Unknown1C;
    AXSetVoiceSrcType(m_Unknown04, AX_SRC_TYPE_LINEAR);
    AXSetVoiceSrcRatio(m_Unknown04, m_Unknown00->m_Unknown0C);
    AXSetVoiceType(m_Unknown04, AX_VOICE_STREAM);
    AXSetVoiceAddr(m_Unknown04, &addr);
    AXSetVoiceAdpcm(m_Unknown04, &adpcm);
}

extern "C" void fn_8035FC48(nlFile*, void*, unsigned int, unsigned long userParam)
{
    AudioReadState_80361258* state = ((AudioStreamChannel_8035FA38*)userParam)->m_Unknown00;
    --state->m_Unknown20_00;
    fn_803675D4();

    AudioReadQueueEntry_80361258* entry = state->m_Unknown24->m_next;
    if (entry == state->m_Unknown24)
        state->m_Unknown24 = 0;
    else
        state->m_Unknown24->m_next = entry->m_next;
    lbl_80585C98.Free(entry);
}

void AudioStreamChannel_8035FA38::fn_8035FCC0(void* pointer)
{
    AXVPB* voice = (AXVPB*)pointer;
    AudioStreamChannel_8035FA38* channel = (AudioStreamChannel_8035FA38*)voice->userContext;
    channel->m_Unknown10_1F = 1;
    if (channel->m_Unknown04 != 0)
    {
        AXSetVoiceState(channel->m_Unknown04, AX_VOICE_STOP);
        MIXReleaseChannel(channel->m_Unknown04);
        channel->m_Unknown04 = 0;
    }
    channel->m_Unknown00->fn_24();
}

void AudioStreamChannel_8035FA38::fn_8035FD2C(bool release)
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

AudioReadState_80361258::AudioReadState_80361258()
{
    m_Unknown18 = 0;
    m_Unknown1C = 0;
    m_Unknown20_00 = 0;
    m_Unknown20_07 = 0;
    m_Unknown20_1F = 0;
    m_Unknown24 = 0;
    m_Unknown28 = 3;
    ++lbl_806E2218;
    ++lbl_806E2220;
}

AudioReadState_80361258::~AudioReadState_80361258()
{
    --lbl_806E2218;
    --lbl_806E2220;
}

void AudioReadState_80361258::fn_40(AudioSourceInfo_8035C298* info)
{
    m_Unknown08 = info;
    AudioStreamChannel_8035FA38* channel = fn_7C();
    while ((channel = fn_80(channel)) != 0)
    {
        channel->m_Unknown00 = this;
        channel->m_Unknown08 = lbl_806E2020->fn_8035C0B4(info->m_Unknown18->m_Unknown10->m_Unknown04 * 2);
        if (channel->m_Unknown08 == 0)
            fn_8035CA84();
        channel->m_Unknown04 = AXAcquireVoice(31, AudioStreamChannel_8035FA38::fn_8035FCC0, (unsigned long)channel);
        MIXInitChannel(channel->m_Unknown04, 0, 0, -960, -960, -960, 64, 127, 0);
        channel->m_Unknown14 = (unsigned int)channel->m_Unknown08;
    }
    fn_4C(0.0f);
    m_Unknown10 = 1;
}

bool AudioReadState_80361258::fn_20(unsigned int value)
{
    m_Unknown14_00 = value;
    switch (m_Unknown10)
    {
    case 1:
        fn_1C();
    case 2:
        m_Unknown28 = 5;
        break;
    case 3:
    {
        AudioStreamChannel_8035FA38* channel = fn_7C();
        while ((channel = fn_80(channel)) != 0)
            AXSetVoiceState(channel->m_Unknown04, AX_VOICE_RUN);
        m_Unknown10 = 4;
        break;
    }
    default:
        return false;
    }
    return true;
}

void AudioReadState_80361258::fn_24()
{
    switch (m_Unknown10)
    {
    case 4:
    {
        bool enabled;
        AudioStreamChannel_8035FA38* channel = fn_7C();
        while ((channel = fn_80(channel)) != 0)
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
            lbl_806E2020->fn_8035C734(this);
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

bool AudioReadState_80361258::fn_28()
{
    if (m_Unknown10 != 4)
        return false;
    AudioStreamChannel_8035FA38* channel = fn_7C();
    while ((channel = fn_80(channel)) != 0)
    {
        channel->m_Unknown0C = (channel->m_Unknown04->pb.addr.currentAddressHi << 16) | channel->m_Unknown04->pb.addr.currentAddressLo;
        AXSetVoiceState(channel->m_Unknown04, AX_VOICE_STOP);
    }
    m_Unknown10 = 7;
    return true;
}

bool AudioReadState_80361258::fn_2C()
{
    if (m_Unknown10 != 7)
        return false;
    AudioStreamChannel_8035FA38* channel = fn_7C();
    while ((channel = fn_80(channel)) != 0)
    {
        AXSetVoiceCurrentAddr(channel->m_Unknown04, channel->m_Unknown0C);
        AXSetVoiceState(channel->m_Unknown04, AX_VOICE_RUN);
    }
    m_Unknown10 = 4;
    return true;
}

extern "C" void fn_80361258(AudioReadState_80361258* state, AsyncEntry* request)
{
    AudioReadQueueEntry_80361258* entry = lbl_80585C98.Allocate();
    entry->m_Unknown00 = request;
    nlRingAddEnd(&state->m_Unknown24, entry);
}

extern "C" void fn_80361308(AudioReadState_80361258* state)
{
    if (state->m_Unknown24 == 0)
        return;
    AudioReadQueueEntry_80361258* pending = 0;
    bool cancelled = false;
    while (state->m_Unknown24 != 0)
    {
        AudioReadQueueEntry_80361258* entry = state->m_Unknown24->m_next;
        if (cancelled || fn_80367B70(entry->m_Unknown00))
        {
            if (entry == state->m_Unknown24)
                state->m_Unknown24 = 0;
            else
                state->m_Unknown24->m_next = entry->m_next;
            nlRingAddEnd(&pending, entry);
        }
        else
        {
            fn_80367DAC(entry->m_Unknown00, fn_80361778);
            cancelled = true;
        }
    }
    state->m_Unknown24 = pending;
}

extern "C" void fn_80361778(nlFile* file, void* buffer, unsigned int size,
    unsigned long userParam, ReadAsyncCallback callback)
{
    callback(file, buffer, size, userParam);
}

void AudioSampleSource_8035F474::fn_5C(int auxiliary, int value)
{
    bool enabled = OSDisableInterrupts();
    if (!fn_64())
    {
        OSRestoreInterrupts(enabled);
        return;
    }
    fn_8035F24C(m_Unknown1C, auxiliary, value);
    OSRestoreInterrupts(enabled);
}

bool AudioSampleSource_8035F474::fn_64()
{
    return m_Unknown1C != 0;
}

void AudioSampleSource_8035F474::fn_58(bool on, unsigned int frequency, bool unchanged)
{
    bool enabled = OSDisableInterrupts();
    if (!fn_64())
    {
        OSRestoreInterrupts(enabled);
        return;
    }
    fn_8035F1AC(m_Unknown1C, on, frequency, unchanged);
    OSRestoreInterrupts(enabled);
}

void AudioSampleSource_8035F474::fn_50(float value)
{
    bool enabled = OSDisableInterrupts();
    if (!fn_64())
    {
        OSRestoreInterrupts(enabled);
        return;
    }
    fn_8035F120(m_Unknown1C, value);
    OSRestoreInterrupts(enabled);
}

void AudioSampleSource_8035F474::fn_54(int value)
{
    bool enabled = OSDisableInterrupts();
    if (!fn_64())
    {
        OSRestoreInterrupts(enabled);
        return;
    }
    fn_8035F180(m_Unknown1C, value);
    OSRestoreInterrupts(enabled);
}

void AudioSampleSource_8035F474::fn_4C(float value)
{
    bool enabled = OSDisableInterrupts();
    if (!fn_64())
    {
        OSRestoreInterrupts(enabled);
        return;
    }
    fn_8035F0D8(m_Unknown1C, value);
    OSRestoreInterrupts(enabled);
}

void AudioSampleSource_8035F474::fn_38(float value)
{
    bool enabled = OSDisableInterrupts();
    if (!fn_64())
    {
        OSRestoreInterrupts(enabled);
        return;
    }
    fn_8035F06C(m_Unknown1C, m_Unknown0C, value);
    OSRestoreInterrupts(enabled);
}

void AudioSampleSource_8035F474::fn_34(float value)
{
    bool enabled = OSDisableInterrupts();
    if (!fn_64())
    {
        OSRestoreInterrupts(enabled);
        return;
    }
    fn_8035EF3C(m_Unknown1C, value);
    OSRestoreInterrupts(enabled);
}

void AudioSampleSource_8035F474::fn_30(float value)
{
    bool enabled = OSDisableInterrupts();
    if (!fn_64())
    {
        OSRestoreInterrupts(enabled);
        return;
    }
    if (m_Unknown14_18)
        MIXRmtSetFader(m_Unknown1C, m_Unknown14_19, (int)(10.0f * value));
    if (lbl_806E2020->m_Unknown450 || !m_Unknown14_18)
        fn_8035EF08(m_Unknown1C, value);
    OSRestoreInterrupts(enabled);
}

AXVPB* AudioSampleSource_8035F474::fn_68()
{
    return m_Unknown1C;
}

bool AudioSampleSource_8035F474::fn_60()
{
    return m_Unknown28;
}

bool AudioSampleSource_8035F474::fn_1C()
{
    m_Unknown10 = 3;
    return false;
}
