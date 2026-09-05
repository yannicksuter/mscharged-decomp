#include "revolution/ax.h"
#include "revolution/axfx.h"

#include "Game/Audio/AudioEffect_802F98F4.h"
#include "Game/Audio/AudioBackend_8035B8E8.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "NL/nlSlotPool.h"
#include "NL/nlString.h"

#include <float.h>

class ConfigNode_80359F80
{
public:
    virtual ~ConfigNode_80359F80();
    virtual unsigned long long fn_80359F80(const u32& key);
};

struct ConfigSystem_80359F80
{
    u8 m_Pad00[0x20];
    ConfigNode_80359F80* m_Root;
};

union ConfigValue_80359F80
{
    unsigned long long m_Raw;
    float m_Float;
    struct
    {
        u32 m_Value;
        u32 m_Type;
    } m_Words;
};

class AudioVoice_8035A438
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
    virtual void fn_50();
    virtual void fn_5C(int, int);
};

struct DelaySettings_80359F80
{
    u32 m_Delay[3];
    u32 m_Feedback[3];
    u32 m_Output[3];
};

static u32 lbl_806DF918 = 300;
static u32 lbl_806DF91C = 300;
static u32 lbl_806DF920 = 300;
static u32 lbl_806DF924 = 50;
static u32 lbl_806DF928 = 50;
static u32 lbl_806DF92C = 50;
static u32 lbl_806DF930 = 100;
static u32 lbl_806DF934 = 100;
static u32 lbl_806DF938 = 100;

class DelayParameter_80359F80 : public AudioEffectParameter_802F69A8
{
public:
    DelayParameter_80359F80();
    virtual ~DelayParameter_80359F80() { }

    static void* operator new(unsigned long)
    {
        DelayParameter_80359F80* parameter = 0;
        s_Pool.Allocate(parameter);
        return parameter;
    }

    static void operator delete(void* pointer)
    {
        s_Pool.Free((DelayParameter_80359F80*)pointer);
    }

    DelaySettings_80359F80 m_Settings;
    float m_AuxVolume;

    static SlotPool<DelayParameter_80359F80> s_Pool;
};

inline DelayParameter_80359F80::DelayParameter_80359F80()
    : m_AuxVolume(0.0f)
{
    m_Settings.m_Delay[0] = lbl_806DF918;
    m_Settings.m_Delay[1] = lbl_806DF91C;
    m_Settings.m_Delay[2] = lbl_806DF920;
    m_Settings.m_Feedback[0] = lbl_806DF924;
    m_Settings.m_Feedback[1] = lbl_806DF928;
    m_Settings.m_Feedback[2] = lbl_806DF92C;
    m_Settings.m_Output[0] = lbl_806DF930;
    m_Settings.m_Output[1] = lbl_806DF934;
    m_Settings.m_Output[2] = lbl_806DF938;
}

class Delay : public AudioEffectBase_802F98F4
{
public:
    Delay();
    virtual ~Delay() { }
    virtual void fn_802F6930(unsigned int definition, void* context, bool negate,
        AudioEffectParameter_802F69A8** output);
    virtual void fn_802F9B5C()
    {
        m_Final = m_Initial;
    }
    virtual void fn_802F9B60(AudioEffectParameter_802F69A8* destination,
        AudioEffectParameter_802F69A8* source);
    virtual void fn_802F9B8C();
    virtual void fn_802F9B64(AudioEffectParameter_802F69A8*) { }
    virtual void fn_802F692C(void*);
    virtual void fn_802F98F0(void* handle);

    static void* operator new(unsigned long)
    {
        Delay* effect = 0;
        s_Pool.Allocate(effect);
        return effect;
    }

    static void operator delete(void* pointer)
    {
        s_Pool.Free((Delay*)pointer);
    }

    DelayParameter_80359F80 m_Initial;
    DelayParameter_80359F80 m_Final;

    static SlotPool<Delay> s_Pool;
};

static const int lbl_806DF93C = 0;
static const int lbl_806DF980 = 0;

SlotPool<DelayParameter_80359F80> DelayParameter_80359F80::s_Pool(16, 16);
SlotPool<Delay> Delay::s_Pool(16, 16);

bool lbl_806E21C8;
float lbl_806E21CC;

extern AuxEffectMap_8035952C* lbl_806E21B8;
extern ConfigSystem_80359F80* lbl_806E202C;

extern "C" void fn_802F1A84(
    void* handle, AudioVoice_8035A438** voices, u32* count);

static void fn_80359D74(void* channels, void*)
{
    AXFXDelayCallback(channels, &lbl_806E2020->m_DelayEffect.m_Delay);
}

static void fn_80359D80(void* channels, void*)
{
    AXFXDelayExpCallbackDpl2((AXFX_BUFFERUPDATE_DPL2*)channels,
        &lbl_806E2020->m_DelayEffect.m_DelayDpl2);
}

Delay::Delay()
    : AudioEffectBase_802F98F4("Delay")
    , m_Initial()
    , m_Final()
{
    AXAuxCallback callback;
    void* context = 0;
    void* delayEffect = &lbl_806E2020->m_DelayEffect;
    AudioBackend_8035B8E8* platform = lbl_806E2020;
    switch (lbl_806E21B8->fn_80359590(lbl_806DF93C))
    {
    case 0:
        AXGetAuxACallback(&callback, &context);
        if (platform->m_OutputMode == 3)
        {
            if (callback != (AXAuxCallback)fn_80359D80)
                AXRegisterAuxACallback(
                    (AXAuxCallback)fn_80359D80, delayEffect);
        }
        else if (callback != fn_80359D74)
        {
            AXRegisterAuxACallback(fn_80359D74, delayEffect);
        }
        break;
    case 1:
        AXGetAuxBCallback(&callback, &context);
        if (platform->m_OutputMode == 3)
        {
            if (callback != (AXAuxCallback)fn_80359D80)
                AXRegisterAuxBCallback(
                    (AXAuxCallback)fn_80359D80, delayEffect);
        }
        else if (callback != fn_80359D74)
        {
            AXRegisterAuxBCallback(fn_80359D74, delayEffect);
        }
        break;
    }

    m_CurrentParameter = &m_Initial;
    m_ResultParameter = &m_Final;
}

void Delay::fn_802F6930(unsigned int definition, void*, bool negate,
    AudioEffectParameter_802F69A8** output)
{
    u32 definitionKey = definition;
    u32 key = 0xD2894EC5;
    ConfigValue_80359F80 value;
    value.m_Raw = lbl_806E202C->m_Root->fn_80359F80(key);
    ConfigNode_80359F80* node
        = (ConfigNode_80359F80*)value.m_Words.m_Value;
    value.m_Raw = node->fn_80359F80(definitionKey);
    node = (ConfigNode_80359F80*)value.m_Words.m_Value;

    DelayParameter_80359F80* parameter = new DelayParameter_80359F80;
    *output = parameter;

    if (lbl_806E21C8)
    {
        m_Final.m_Settings.m_Delay[0] = lbl_806DF918;
        m_Final.m_Settings.m_Delay[1] = lbl_806DF91C;
        m_Final.m_Settings.m_Delay[2] = lbl_806DF920;
        m_Final.m_Settings.m_Feedback[0] = lbl_806DF924;
        m_Final.m_Settings.m_Feedback[1] = lbl_806DF928;
        m_Final.m_Settings.m_Feedback[2] = lbl_806DF92C;
        m_Final.m_Settings.m_Output[0] = lbl_806DF930;
        m_Final.m_Settings.m_Output[1] = lbl_806DF934;
        m_Final.m_Settings.m_Output[2] = lbl_806DF938;
        m_Final.m_AuxVolume = lbl_806E21CC;

        parameter->m_State = m_Final.m_State;
        for (u32 i = 0; i < 3; ++i)
        {
            parameter->m_Settings.m_Delay[i]
                = m_Final.m_Settings.m_Delay[i];
            parameter->m_Settings.m_Feedback[i]
                = m_Final.m_Settings.m_Feedback[i];
            parameter->m_Settings.m_Output[i]
                = m_Final.m_Settings.m_Output[i];
        }
        parameter->m_AuxVolume = m_Final.m_AuxVolume;
        return;
    }

    key = nlStringLowerHash("delayL");
    value.m_Raw = node->fn_80359F80(key);
    parameter->m_Settings.m_Delay[0] = value.m_Words.m_Value;
    key = nlStringLowerHash("delayR");
    value.m_Raw = node->fn_80359F80(key);
    parameter->m_Settings.m_Delay[1] = value.m_Words.m_Value;
    key = nlStringLowerHash("delayS");
    value.m_Raw = node->fn_80359F80(key);
    parameter->m_Settings.m_Delay[2] = value.m_Words.m_Value;
    key = nlStringLowerHash("feedbackL");
    value.m_Raw = node->fn_80359F80(key);
    parameter->m_Settings.m_Feedback[0] = value.m_Words.m_Value;
    key = nlStringLowerHash("feedbackR");
    value.m_Raw = node->fn_80359F80(key);
    parameter->m_Settings.m_Feedback[1] = value.m_Words.m_Value;
    key = nlStringLowerHash("feedbackS");
    value.m_Raw = node->fn_80359F80(key);
    parameter->m_Settings.m_Feedback[2] = value.m_Words.m_Value;
    key = nlStringLowerHash("outputL");
    value.m_Raw = node->fn_80359F80(key);
    parameter->m_Settings.m_Output[0] = value.m_Words.m_Value;
    key = nlStringLowerHash("outputR");
    value.m_Raw = node->fn_80359F80(key);
    parameter->m_Settings.m_Output[1] = value.m_Words.m_Value;
    key = nlStringLowerHash("outputS");
    value.m_Raw = node->fn_80359F80(key);
    parameter->m_Settings.m_Output[2] = value.m_Words.m_Value;
    key = nlStringLowerHash("auxvol");
    value.m_Raw = node->fn_80359F80(key);
    parameter->m_AuxVolume
        = negate ? 1.0f - value.m_Float : value.m_Float;
}

void Delay::fn_802F9B60(AudioEffectParameter_802F69A8* destination,
    AudioEffectParameter_802F69A8* source)
{
    DelayParameter_80359F80* destinationParameter
        = (DelayParameter_80359F80*)destination;
    DelayParameter_80359F80* sourceParameter
        = (DelayParameter_80359F80*)source;
    for (u32 i = 0; i < 3; ++i)
    {
        destinationParameter->m_Settings.m_Delay[i]
            = sourceParameter->m_Settings.m_Delay[i];
        destinationParameter->m_Settings.m_Feedback[i]
            = sourceParameter->m_Settings.m_Feedback[i];
        destinationParameter->m_Settings.m_Output[i]
            = sourceParameter->m_Settings.m_Output[i];
    }
    destinationParameter->m_AuxVolume = sourceParameter->m_AuxVolume;
}

void fn_8035A3EC(AXFX_DELAY* delay)
{
    delay->delay[0] = lbl_806DF918;
    delay->delay[1] = lbl_806DF91C;
    delay->delay[2] = lbl_806DF920;
    delay->feedback[0] = lbl_806DF924;
    delay->feedback[1] = lbl_806DF928;
    delay->feedback[2] = lbl_806DF92C;
    delay->output[0] = lbl_806DF930;
    delay->output[1] = lbl_806DF934;
    delay->output[2] = lbl_806DF938;
}

void Delay::fn_802F98F0(void* handle)
{
    AudioVoice_8035A438* voices[8];
    u32 count;
    fn_802F1A84(handle, voices, &count);

    int volume = (int)(-960.0f * (1.0f - m_Final.m_AuxVolume));
    int auxIndex = lbl_806E21B8->fn_80359590(lbl_806DF980);
    for (u32 i = 0; i < count; ++i)
        voices[i]->fn_5C(auxIndex, volume);
}

void Delay::fn_802F692C(void*)
{
    AXFX_DELAY* delay = &lbl_806E2020->m_DelayEffect.m_Delay;
    for (u32 i = 0; i < 3; ++i)
    {
        u32 delayTime = m_Final.m_Settings.m_Delay[i];
        if (delayTime < 1)
            delayTime = 1;
        if (delayTime > 750)
            delayTime = 750;
        delay->delay[i] = delayTime;

        u32 feedback = m_Final.m_Settings.m_Feedback[i];
        if (feedback > 99)
            feedback = 99;
        delay->feedback[i] = feedback;

        u32 output = m_Final.m_Settings.m_Output[i];
        if (output > 100)
            output = 100;
        delay->output[i] = output;
    }

    if (lbl_806E2020->m_OutputMode == 3)
        AXFXDelayExpSettingsDpl2(
            (AXFX_DELAY_EXP_DPL2*)&lbl_806E2020->m_DelayEffect);
    else
        AXFXDelaySettings(delay);
}

void Delay::fn_802F9B8C()
{
    if (m_Parameters.m_Head == 0 && m_Final.m_AuxVolume < FLT_MIN)
        m_Enabled = true;
}

template struct UnidentifiedStaticStorage<UnidentifiedStaticTag>;
