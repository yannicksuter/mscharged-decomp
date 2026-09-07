#include "revolution/wpad/WPAD.h"

#include "Game/Audio/AudioEffects_803622F0.h"
#include "Game/Audio/AudioConfig_8035B240.h"
#include "Game/Audio/AudioSource_8035EF08.h"
#include "NL/nlString.h"
#include "revolution/os/OSInterrupt.h"

static bool lbl_806DF9E0 = true;
static unsigned int lbl_806E2238 = nlStringLowerHash("PauseOnZero");
static unsigned int lbl_806E223C = nlStringLowerHash("StopOnZero");

SlotPool<VolumeParameter_80362EC8> VolumeParameter_80362EC8::s_Pool(16, 16);
SlotPool<Volume> Volume::s_Pool(16, 16);
static unsigned char lbl_80585DD0[sizeof(ControllerSpeaker) * 4];
nlArrayAllocator<ControllerSpeaker> ControllerSpeaker::s_Allocator((ControllerSpeaker*)lbl_80585DD0, 4);

extern "C" void fn_802F1A84(void*, AudioSource_8035C234**, unsigned int*);

extern "C" void fn_803622F0(bool enabled)
{
    lbl_806DF9E0 = enabled;
}

extern "C" AudioEffectFactory_803622F8* fn_803622F8()
{
    static AudioEffectFactory_803622F8 factory;
    return &factory;
}

void AudioEffectFactory_803622F8::fn_0C()
{
}

void AudioEffectFactory_803622F8::fn_10(float)
{
}

void AudioEffectFactory_803622F8::fn_1C(AudioEffectBase_802F98F4* effect)
{
    delete effect;
}

bool AudioEffectFactory_803622F8::fn_20()
{
    return true;
}

void ControllerSpeaker::fn_802F98F0(void*)
{
}

void ControllerSpeaker::fn_802F98EC()
{
    if (lbl_806DF9E0)
    {
        if (--m_Unknown40 < 0)
            m_Unknown40 = 0;
        if (m_Unknown40 == 0)
            WPADControlSpeaker(m_Unknown3C, WPAD_SPEAKER_MUTE, 0);
    }
}

void ControllerSpeaker::fn_802F692C(void* handle)
{
    if (lbl_806DF9E0)
    {
        if (m_Unknown40 == 0)
            WPADControlSpeaker(m_Unknown3C, WPAD_SPEAKER_UNMUTE, 0);
        ++m_Unknown40;
        bool enabled = OSDisableInterrupts();
        AudioSource_8035C234* voices[8];
        unsigned int count;
        fn_802F1A84(handle, voices, &count);
        for (unsigned int i = 0; i < count; ++i)
            voices[i]->fn_8035F3AC(true, m_Unknown3C);
        OSRestoreInterrupts(enabled);
    }
}

void ControllerSpeaker::fn_802F9B64(AudioEffectParameter_802F69A8*)
{
}

void ControllerSpeaker::fn_802F9B60(AudioEffectParameter_802F69A8*, AudioEffectParameter_802F69A8*)
{
}

void ControllerSpeaker::fn_802F9B5C()
{
}

void ControllerSpeaker::fn_802F6930(unsigned int, void* context, bool,
    AudioEffectParameter_802F69A8** output)
{
    *output = &m_Parameter;
    m_Unknown3C = *(int*)context - 1;
}

void Volume::fn_802F9B64(AudioEffectParameter_802F69A8* parameter)
{
    m_Initial.m_Unknown10 += ((VolumeParameter_80362EC8*)parameter)->m_Unknown10;
}

void Volume::fn_802F9B60(AudioEffectParameter_802F69A8* destination,
    AudioEffectParameter_802F69A8* source)
{
    VolumeParameter_80362EC8* destinationParameter = (VolumeParameter_80362EC8*)destination;
    VolumeParameter_80362EC8* sourceParameter = (VolumeParameter_80362EC8*)source;
    float amount;
    if (sourceParameter->m_State.m_Flags.bytes[0])
    {
        amount = *(float*)sourceParameter->m_State.m_Current.pointer;
        amount = amount >= 0.0f ? amount : 0.0f;
        amount = amount <= 1.0f ? amount : 1.0f;
    }
    else
    {
        amount = sourceParameter->m_State.m_Target.scalar;
        if (amount)
        {
            amount = sourceParameter->m_State.m_Current.scalar / amount;
            amount = amount >= 0.0f ? amount : 0.0f;
            amount = amount <= 1.0f ? amount : 1.0f;
        }
        else
        {
            amount = 1.0f;
        }
    }
    destinationParameter->m_Unknown10 += sourceParameter->m_Unknown10 * amount;
    destinationParameter->m_Unknown14_00 |= sourceParameter->m_Unknown14_00;
    destinationParameter->m_Unknown14_01 |= sourceParameter->m_Unknown14_01;
}

void Volume::fn_802F9B5C()
{
    m_Final = m_Initial;
}

void Volume::fn_802F6930(unsigned int definition, void* context, bool negate,
    AudioEffectParameter_802F69A8** output)
{
    ConfigNode_8035B240* node = ConfigFindDefinition_8035B240(definition);
    VolumeParameter_80362EC8* parameter = new VolumeParameter_80362EC8;
    ConfigValue_8035B240* argument = (ConfigValue_8035B240*)context;
    *output = parameter;
    unsigned int key;
    ConfigValue_8035B240 value;
    if (argument->m_Words.m_Type == 2)
    {
        key = 0xCE5C5677;
        value.m_Raw = node->fn_8035B240(key);
        parameter->m_Unknown10 = value.m_Float;
    }
    else if (argument->m_Words.m_Type == 1)
    {
        parameter->m_Unknown10 = argument->m_Float;
    }
    else
    {
        parameter->m_Unknown10 = (float)(int)argument->m_Words.m_Value;
    }
    parameter->m_Unknown10 = negate ? -parameter->m_Unknown10 : parameter->m_Unknown10;
    key = lbl_806E2238;
    value.m_Raw = node->fn_8035B240(key);
    parameter->m_Unknown14_00 = value.m_Words.m_Value;
    key = lbl_806E223C;
    value.m_Raw = node->fn_8035B240(key);
    parameter->m_Unknown14_01 = value.m_Words.m_Value;
}

AudioEffectFactory_803622F8::~AudioEffectFactory_803622F8()
{
}

VolumeParameter_80362EC8::~VolumeParameter_80362EC8()
{
}

ControllerSpeakerParameter_80362EF0::~ControllerSpeakerParameter_80362EF0()
{
}

Volume::~Volume()
{
}

ControllerSpeaker::~ControllerSpeaker()
{
}
