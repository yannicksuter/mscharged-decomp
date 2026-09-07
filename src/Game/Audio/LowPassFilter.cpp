#include "Game/Audio/AudioSource.h"
#include "Game/Audio/AudioConfig.h"
#include "Game/Audio/AudioEffect.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "NL/nlSlotPool.h"
#include "NL/nlString.h"

class LowPassFilterParameter : public AudioEffectParameter
{
public:
    LowPassFilterParameter();
    virtual ~LowPassFilterParameter() { }

    static void* operator new(unsigned long)
    {
        LowPassFilterParameter* parameter = 0;
        s_Pool.Allocate(parameter);
        return parameter;
    }

    static void operator delete(void* pointer)
    {
        s_Pool.Free((LowPassFilterParameter*)pointer);
    }

    u32 m_On;
    u32 m_Frequency;

    static SlotPool<LowPassFilterParameter> s_Pool;
};

class LowPassFilter : public AudioEffectBase
{
public:
    virtual ~LowPassFilter() { }
    virtual void CreateParameter(unsigned int definition, void* context,
        bool disabled, AudioEffectParameter** output);
    virtual void BeginBlend();
    virtual void BlendParameter(AudioEffectParameter* destination,
        AudioEffectParameter* source);
    virtual void EndBlend();
    virtual void OnParameterFinished(AudioEffectParameter* parameter);
    virtual void ApplyToSound(void* handle);

    static void* operator new(unsigned long)
    {
        LowPassFilter* effect = 0;
        s_Pool.Allocate(effect);
        return effect;
    }

    static void operator delete(void* pointer)
    {
        s_Pool.Free((LowPassFilter*)pointer);
    }

    LowPassFilterParameter m_Initial;
    LowPassFilterParameter m_Final;
    u32 m_FilterCount;

    static SlotPool<LowPassFilter> s_Pool;
};

static u32 sLowPassFilterFrequency = 1000;
static u32 sLowPassFilterEnabled = 1;

SlotPool<LowPassFilterParameter>
    LowPassFilterParameter::s_Pool(16, 16);
SlotPool<LowPassFilter> LowPassFilter::s_Pool(16, 16);

bool gLowPassFilterOverrideEnabled;

LowPassFilterParameter::LowPassFilterParameter()
    : m_On(0)
    , m_Frequency(16000)
{
}

void LowPassFilter::CreateParameter(unsigned int definition, void*, bool disabled,
    AudioEffectParameter** output)
{
    LowPassFilterParameter* parameter
        = new LowPassFilterParameter;
    *output = parameter;

    if (gLowPassFilterOverrideEnabled)
    {
        m_Final.m_On = sLowPassFilterEnabled;
        m_Final.m_Frequency = sLowPassFilterFrequency;
        *parameter = m_Final;
        return;
    }

    AudioConfigNode* node = ConfigFindDefinition(definition);

    parameter->m_On = (u16)node->Get(nlStringLowerHash("on")).m_Words.m_Value;

    u32 frequency = node->Get(nlStringLowerHash("freq")).m_Words.m_Value;
    parameter->m_Frequency = disabled ? 0 : 16000 - frequency;
}

void LowPassFilter::BeginBlend()
{
    if (m_Initial.m_On != 0)
    {
        m_Final = m_Initial;
        m_FilterCount = 1;
    }
    else
    {
        m_Final.m_Frequency = 0;
        m_Final.m_On = 0;
        m_FilterCount = 0;
    }
}

void LowPassFilter::BlendParameter(
    AudioEffectParameter* destination,
    AudioEffectParameter* source)
{
    LowPassFilterParameter* destinationParameter
        = (LowPassFilterParameter*)destination;
    LowPassFilterParameter* sourceParameter
        = (LowPassFilterParameter*)source;

    float amount;
    if (sourceParameter->m_State.m_Flags.bytes[0])
    {
        amount = *(float*)sourceParameter->m_State.m_Current.pointer;
    }
    else if (sourceParameter->m_State.m_Target.scalar != 0.0f)
    {
        amount = sourceParameter->m_State.m_Current.scalar
               / sourceParameter->m_State.m_Target.scalar;
    }
    else
    {
        amount = 1.0f;
    }

    amount = amount >= 0.0f ? amount : 0.0f;
    amount = amount <= 1.0f ? amount : 1.0f;

    s32 adjustment;
    if (sourceParameter->m_Frequency != 0)
    {
        adjustment = (u32)(sourceParameter->m_Frequency * amount);
    }
    else
    {
        adjustment = -(s32)(16000.0f * amount);
        if ((u32)-adjustment > destinationParameter->m_Frequency)
            adjustment = -(s32)destinationParameter->m_Frequency;
    }

    destinationParameter->m_Frequency += adjustment;
    if (sourceParameter->m_On > destinationParameter->m_On)
        destinationParameter->m_On = sourceParameter->m_On;
    ++m_FilterCount;
}

void LowPassFilter::EndBlend()
{
    m_Final.m_Frequency /= m_FilterCount;
    if (m_Parameters.m_Head == 0 && m_Initial.m_On == 0)
        m_Enabled = true;
}

void LowPassFilter::ApplyToSound(void* handle)
{
    AudioSource* voices[8];
    unsigned int count;
    GetSoundSources(handle, voices, &count);

    for (u16 i = 0; i < count; ++i)
    {
        u32 frequency = 16000 - m_Final.m_Frequency;
        voices[i]->SetLowPassFilter(m_Final.m_On != 0,
            frequency > 16000 ? 16000 : frequency,
            false);
    }
}

void LowPassFilter::OnParameterFinished(AudioEffectParameter* parameter)
{
    LowPassFilterParameter* filterParameter
        = (LowPassFilterParameter*)parameter;
    m_Initial.m_Frequency = filterParameter->m_Frequency;
    m_Initial.m_On = m_Initial.m_Frequency != 0;
}

template struct UnidentifiedStaticStorage<UnidentifiedStaticTag>;
