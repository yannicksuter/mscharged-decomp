#include "Game/Audio/AudioEffect_802F98F4.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "NL/nlSlotPool.h"
#include "NL/nlString.h"

class ConfigNode_8035AA04
{
public:
    virtual ~ConfigNode_8035AA04();
    virtual unsigned long long fn_8035AA04(const u32& key);
};

struct ConfigSystem_8035AA04
{
    u8 m_Pad00[0x20];
    ConfigNode_8035AA04* m_Root;
};

union ConfigValue_8035AA04
{
    unsigned long long m_Raw;
    float m_Float;
    struct
    {
        u32 m_Value;
        u32 m_Type;
    } m_Words;
};

class AudioVoice_8035AE30
{
public:
    virtual void fn_00();
    virtual void fn_04();
    virtual void fn_08();
    virtual void fn_0C();
    virtual void fn_10();
    virtual void fn_14();
    virtual void fn_18();
    virtual void fn_1C();
    virtual void fn_20();
    virtual void fn_24();
    virtual void fn_28();
    virtual void fn_2C();
    virtual void fn_30();
    virtual void fn_34();
    virtual void fn_38();
    virtual void fn_3C();
    virtual void fn_40();
    virtual void fn_44();
    virtual void fn_48();
    virtual void fn_4C();
    virtual void fn_50(bool, u32, bool);
};

class LowPassFilterParameter_8035A9E0 : public AudioEffectParameter_802F69A8
{
public:
    LowPassFilterParameter_8035A9E0();
    virtual ~LowPassFilterParameter_8035A9E0() { }

    static void* operator new(unsigned long)
    {
        LowPassFilterParameter_8035A9E0* parameter = 0;
        s_Pool.Allocate(parameter);
        return parameter;
    }

    static void operator delete(void* pointer)
    {
        s_Pool.Free((LowPassFilterParameter_8035A9E0*)pointer);
    }

    u32 m_On;
    u32 m_Frequency;

    static SlotPool<LowPassFilterParameter_8035A9E0> s_Pool;
};

class LowPassFilter : public AudioEffectBase_802F98F4
{
public:
    virtual ~LowPassFilter() { }
    virtual void fn_802F6930(unsigned int definition, void* context,
        bool disabled, AudioEffectParameter_802F69A8** output);
    virtual void fn_802F9B5C();
    virtual void fn_802F9B60(AudioEffectParameter_802F69A8* destination,
        AudioEffectParameter_802F69A8* source);
    virtual void fn_802F9B8C();
    virtual void fn_802F9B64(AudioEffectParameter_802F69A8* parameter);
    virtual void fn_802F98F0(void* handle);

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

    LowPassFilterParameter_8035A9E0 m_Initial;
    LowPassFilterParameter_8035A9E0 m_Final;
    u32 m_FilterCount;

    static SlotPool<LowPassFilter> s_Pool;
};

static u32 lbl_806DF988 = 1000;
static u32 lbl_806DF98C = 1;

SlotPool<LowPassFilterParameter_8035A9E0>
    LowPassFilterParameter_8035A9E0::s_Pool(16, 16);
SlotPool<LowPassFilter> LowPassFilter::s_Pool(16, 16);

bool lbl_806E21D0;

extern ConfigSystem_8035AA04* lbl_806E202C;

extern "C" void fn_802F1A84(
    void* handle, AudioVoice_8035AE30** voices, u32* count);

LowPassFilterParameter_8035A9E0::LowPassFilterParameter_8035A9E0()
    : m_On(0)
    , m_Frequency(16000)
{
}

void LowPassFilter::fn_802F6930(unsigned int definition, void*, bool disabled,
    AudioEffectParameter_802F69A8** output)
{
    LowPassFilterParameter_8035A9E0* parameter
        = new LowPassFilterParameter_8035A9E0;
    *output = parameter;

    if (lbl_806E21D0)
    {
        m_Final.m_On = lbl_806DF98C;
        m_Final.m_Frequency = lbl_806DF988;
        parameter->m_State = m_Final.m_State;
        parameter->m_On = m_Final.m_On;
        parameter->m_Frequency = m_Final.m_Frequency;
        return;
    }

    u32 definitionKey = definition;
    u32 key = 0xD2894EC5;
    ConfigValue_8035AA04 value;
    value.m_Raw = lbl_806E202C->m_Root->fn_8035AA04(key);
    ConfigNode_8035AA04* node
        = (ConfigNode_8035AA04*)value.m_Words.m_Value;
    value.m_Raw = node->fn_8035AA04(definitionKey);
    node = (ConfigNode_8035AA04*)value.m_Words.m_Value;

    key = nlStringLowerHash("on");
    value.m_Raw = node->fn_8035AA04(key);
    parameter->m_On = (u16)value.m_Words.m_Value;

    key = nlStringLowerHash("freq");
    value.m_Raw = node->fn_8035AA04(key);
    parameter->m_Frequency
        = disabled ? 0 : 16000 - value.m_Words.m_Value;
}

void LowPassFilter::fn_802F9B5C()
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

void LowPassFilter::fn_802F9B60(
    AudioEffectParameter_802F69A8* destination,
    AudioEffectParameter_802F69A8* source)
{
    LowPassFilterParameter_8035A9E0* destinationParameter
        = (LowPassFilterParameter_8035A9E0*)destination;
    LowPassFilterParameter_8035A9E0* sourceParameter
        = (LowPassFilterParameter_8035A9E0*)source;

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

void LowPassFilter::fn_802F9B8C()
{
    m_Final.m_Frequency /= m_FilterCount;
    if (m_Parameters.m_Head == 0 && m_Initial.m_On == 0)
        m_Enabled = true;
}

void LowPassFilter::fn_802F98F0(void* handle)
{
    AudioVoice_8035AE30* voices[8];
    u32 count;
    fn_802F1A84(handle, voices, &count);

    for (u16 i = 0; i < count; ++i)
    {
        u32 frequency = 16000 - m_Final.m_Frequency;
        voices[i]->fn_50(m_Final.m_On != 0,
            frequency > 16000 ? 16000 : frequency,
            false);
    }
}

void LowPassFilter::fn_802F9B64(AudioEffectParameter_802F69A8* parameter)
{
    LowPassFilterParameter_8035A9E0* filterParameter
        = (LowPassFilterParameter_8035A9E0*)parameter;
    m_Initial.m_Frequency = filterParameter->m_Frequency;
    m_Initial.m_On = m_Initial.m_Frequency != 0;
}

template struct UnidentifiedStaticStorage<UnidentifiedStaticTag>;
