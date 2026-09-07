#include "revolution/wpad/WPAD.h"

#include "Game/Audio/AudioEffects.h"
#include "Game/Audio/AudioConfig.h"
#include "Game/Audio/AudioSource.h"
#include "NL/nlString.h"
#include "revolution/os/OSInterrupt.h"

static bool sControllerSpeakerEnabled = true;
static unsigned int sPauseOnZeroKey = nlStringLowerHash("PauseOnZero");
static unsigned int sStopOnZeroKey = nlStringLowerHash("StopOnZero");

SlotPool<VolumeParameter> VolumeParameter::s_Pool(16, 16);
SlotPool<Volume> Volume::s_Pool(16, 16);
static unsigned char sControllerSpeakerStorage[sizeof(ControllerSpeaker) * 4];
nlArrayAllocator<ControllerSpeaker> ControllerSpeaker::s_Allocator((ControllerSpeaker*)sControllerSpeakerStorage, 4);

void SetControllerSpeakerEnabled(bool enabled)
{
    sControllerSpeakerEnabled = enabled;
}

AudioEffectFactory* GetAudioEffectFactory()
{
    static AudioEffectFactory factory;
    return &factory;
}

void AudioEffectFactory::Initialize()
{
}

void AudioEffectFactory::Update(float)
{
}

void AudioEffectFactory::ReleaseEffect(AudioEffectBase* effect)
{
    delete effect;
}

bool AudioEffectFactory::IsInitialized()
{
    return true;
}

void ControllerSpeaker::ApplyToSound(void*)
{
}

void ControllerSpeaker::OnSoundStopped()
{
    if (sControllerSpeakerEnabled)
    {
        if (--m_Unknown40 < 0)
            m_Unknown40 = 0;
        if (m_Unknown40 == 0)
            WPADControlSpeaker(m_Unknown3C, WPAD_SPEAKER_MUTE, 0);
    }
}

void ControllerSpeaker::OnSoundStarted(void* handle)
{
    if (sControllerSpeakerEnabled)
    {
        if (m_Unknown40 == 0)
            WPADControlSpeaker(m_Unknown3C, WPAD_SPEAKER_UNMUTE, 0);
        ++m_Unknown40;
        bool enabled = OSDisableInterrupts();
        AudioSource* voices[8];
        unsigned int count;
        GetSoundSources(handle, voices, &count);
        for (unsigned int i = 0; i < count; ++i)
            voices[i]->SetControllerSpeaker(true, m_Unknown3C);
        OSRestoreInterrupts(enabled);
    }
}

void ControllerSpeaker::OnParameterFinished(AudioEffectParameter*)
{
}

void ControllerSpeaker::BlendParameter(AudioEffectParameter*, AudioEffectParameter*)
{
}

void ControllerSpeaker::BeginBlend()
{
}

void ControllerSpeaker::CreateParameter(unsigned int, void* context, bool,
    AudioEffectParameter** output)
{
    *output = &m_Parameter;
    m_Unknown3C = *(int*)context - 1;
}

void Volume::OnParameterFinished(AudioEffectParameter* parameter)
{
    m_Initial.m_Unknown10 += ((VolumeParameter*)parameter)->m_Unknown10;
}

void Volume::BlendParameter(AudioEffectParameter* destination,
    AudioEffectParameter* source)
{
    VolumeParameter* destinationParameter = (VolumeParameter*)destination;
    VolumeParameter* sourceParameter = (VolumeParameter*)source;
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

void Volume::BeginBlend()
{
    m_Final = m_Initial;
}

void Volume::CreateParameter(unsigned int definition, void* context, bool negate,
    AudioEffectParameter** output)
{
    AudioConfigNode* node = ConfigFindDefinition(definition);
    VolumeParameter* parameter = new VolumeParameter;
    AudioConfigValue* argument = (AudioConfigValue*)context;
    *output = parameter;
    unsigned int key;
    if (argument->m_Words.m_Type == 2)
    {
        key = 0xCE5C5677;
        parameter->m_Unknown10 = node->Get(key).m_Float;
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
    key = sPauseOnZeroKey;
    parameter->m_Unknown14_00 = node->Get(key).m_Words.m_Value;
    key = sStopOnZeroKey;
    parameter->m_Unknown14_01 = node->Get(key).m_Words.m_Value;
}

AudioEffectFactory::~AudioEffectFactory()
{
}

VolumeParameter::~VolumeParameter()
{
}

ControllerSpeakerParameter::~ControllerSpeakerParameter()
{
}

Volume::~Volume()
{
}

ControllerSpeaker::~ControllerSpeaker()
{
}
