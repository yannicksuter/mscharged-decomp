#include "Game/Audio/AudioSource.h"
#include "Game/Audio/AudioConfig.h"
#include "revolution/ax.h"
#include "revolution/axfx.h"

#include "Game/Audio/AudioEffect.h"
#include "Game/Audio/AudioBackend.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "NL/nlSlotPool.h"
#include "NL/nlString.h"

#include <float.h>

struct DelaySettings
{
    u32 m_Delay[3];
    u32 m_Feedback[3];
    u32 m_Output[3];
};

static u32 sDelayLeft = 300;
static u32 sDelayRight = 300;
static u32 sDelaySurround = 300;
static u32 sDelayFeedbackLeft = 50;
static u32 sDelayFeedbackRight = 50;
static u32 sDelayFeedbackSurround = 50;
static u32 sDelayOutputLeft = 100;
static u32 sDelayOutputRight = 100;
static u32 sDelayOutputSurround = 100;

class DelayParameter : public AudioEffectParameter
{
public:
    DelayParameter();
    virtual ~DelayParameter() { }

    static void* operator new(unsigned long)
    {
        DelayParameter* parameter = 0;
        s_Pool.Allocate(parameter);
        return parameter;
    }

    static void operator delete(void* pointer)
    {
        s_Pool.Free((DelayParameter*)pointer);
    }

    DelaySettings m_Settings;
    float m_AuxVolume;

    static SlotPool<DelayParameter> s_Pool;
};

inline DelayParameter::DelayParameter()
    : m_AuxVolume(0.0f)
{
    m_Settings.m_Delay[0] = sDelayLeft;
    m_Settings.m_Delay[1] = sDelayRight;
    m_Settings.m_Delay[2] = sDelaySurround;
    m_Settings.m_Feedback[0] = sDelayFeedbackLeft;
    m_Settings.m_Feedback[1] = sDelayFeedbackRight;
    m_Settings.m_Feedback[2] = sDelayFeedbackSurround;
    m_Settings.m_Output[0] = sDelayOutputLeft;
    m_Settings.m_Output[1] = sDelayOutputRight;
    m_Settings.m_Output[2] = sDelayOutputSurround;
}

class Delay : public AudioEffectBase
{
public:
    Delay();
    virtual ~Delay() { }
    virtual void CreateParameter(unsigned int definition, void* context, bool negate,
        AudioEffectParameter** output);
    virtual void BeginBlend()
    {
        m_Final = m_Initial;
    }
    virtual void BlendParameter(AudioEffectParameter* destination,
        AudioEffectParameter* source);
    virtual void EndBlend();
    virtual void OnParameterFinished(AudioEffectParameter*) { }
    virtual void OnSoundStarted(void*);
    virtual void ApplyToSound(void* handle);

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

    DelayParameter m_Initial;
    DelayParameter m_Final;

    static SlotPool<Delay> s_Pool;
};

static const int sDelayEffectId = 0;
static const int sDelaySendEffectId = 0;

SlotPool<DelayParameter> DelayParameter::s_Pool(16, 16);
SlotPool<Delay> Delay::s_Pool(16, 16);

bool gDelayOverrideEnabled;
float gDelayOverrideVolume;

static void ProcessDelay(void* channels, void*)
{
    AXFXDelayCallback(channels, &g_pAudioBackend->m_DelayEffect.m_Delay);
}

static void ProcessDelayDpl2(void* channels, void*)
{
    AXFXDelayExpCallbackDpl2((AXFX_BUFFERUPDATE_DPL2*)channels,
        &g_pAudioBackend->m_DelayEffect.m_DelayDpl2);
}

Delay::Delay()
    : AudioEffectBase("Delay")
    , m_Initial()
    , m_Final()
{
    AXAuxCallback callback;
    void* context = 0;
    void* delayEffect = &g_pAudioBackend->m_DelayEffect;
    AudioBackend* platform = g_pAudioBackend;
    switch (g_pAuxEffectMap->GetAuxiliary(sDelayEffectId))
    {
    case 0:
        AXGetAuxACallback(&callback, &context);
        if (platform->m_OutputMode == 3)
        {
            if (callback != (AXAuxCallback)ProcessDelayDpl2)
                AXRegisterAuxACallback(
                    (AXAuxCallback)ProcessDelayDpl2, delayEffect);
        }
        else if (callback != ProcessDelay)
        {
            AXRegisterAuxACallback(ProcessDelay, delayEffect);
        }
        break;
    case 1:
        AXGetAuxBCallback(&callback, &context);
        if (platform->m_OutputMode == 3)
        {
            if (callback != (AXAuxCallback)ProcessDelayDpl2)
                AXRegisterAuxBCallback(
                    (AXAuxCallback)ProcessDelayDpl2, delayEffect);
        }
        else if (callback != ProcessDelay)
        {
            AXRegisterAuxBCallback(ProcessDelay, delayEffect);
        }
        break;
    }

    m_CurrentParameter = &m_Initial;
    m_ResultParameter = &m_Final;
}

void Delay::CreateParameter(unsigned int definition, void*, bool negate,
    AudioEffectParameter** output)
{
    AudioConfigNode* node = ConfigFindDefinition(definition);

    DelayParameter* parameter = new DelayParameter;
    *output = parameter;

    if (gDelayOverrideEnabled)
    {
        m_Final.m_Settings.m_Delay[0] = sDelayLeft;
        m_Final.m_Settings.m_Delay[1] = sDelayRight;
        m_Final.m_Settings.m_Delay[2] = sDelaySurround;
        m_Final.m_Settings.m_Feedback[0] = sDelayFeedbackLeft;
        m_Final.m_Settings.m_Feedback[1] = sDelayFeedbackRight;
        m_Final.m_Settings.m_Feedback[2] = sDelayFeedbackSurround;
        m_Final.m_Settings.m_Output[0] = sDelayOutputLeft;
        m_Final.m_Settings.m_Output[1] = sDelayOutputRight;
        m_Final.m_Settings.m_Output[2] = sDelayOutputSurround;
        m_Final.m_AuxVolume = gDelayOverrideVolume;

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

    parameter->m_Settings.m_Delay[0] = node->Get(nlStringLowerHash("delayL")).m_Words.m_Value;
    parameter->m_Settings.m_Delay[1] = node->Get(nlStringLowerHash("delayR")).m_Words.m_Value;
    parameter->m_Settings.m_Delay[2] = node->Get(nlStringLowerHash("delayS")).m_Words.m_Value;
    parameter->m_Settings.m_Feedback[0] = node->Get(nlStringLowerHash("feedbackL")).m_Words.m_Value;
    parameter->m_Settings.m_Feedback[1] = node->Get(nlStringLowerHash("feedbackR")).m_Words.m_Value;
    parameter->m_Settings.m_Feedback[2] = node->Get(nlStringLowerHash("feedbackS")).m_Words.m_Value;
    parameter->m_Settings.m_Output[0] = node->Get(nlStringLowerHash("outputL")).m_Words.m_Value;
    parameter->m_Settings.m_Output[1] = node->Get(nlStringLowerHash("outputR")).m_Words.m_Value;
    parameter->m_Settings.m_Output[2] = node->Get(nlStringLowerHash("outputS")).m_Words.m_Value;
    parameter->m_AuxVolume = node->Get(nlStringLowerHash("auxvol")).m_Float;
    parameter->m_AuxVolume = negate ? 1.0f - parameter->m_AuxVolume : parameter->m_AuxVolume;
}

void Delay::BlendParameter(AudioEffectParameter* destination,
    AudioEffectParameter* source)
{
    DelayParameter* destinationParameter
        = (DelayParameter*)destination;
    DelayParameter* sourceParameter
        = (DelayParameter*)source;
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

void SetDefaultDelaySettings(AXFX_DELAY* delay)
{
    delay->delay[0] = sDelayLeft;
    delay->delay[1] = sDelayRight;
    delay->delay[2] = sDelaySurround;
    delay->feedback[0] = sDelayFeedbackLeft;
    delay->feedback[1] = sDelayFeedbackRight;
    delay->feedback[2] = sDelayFeedbackSurround;
    delay->output[0] = sDelayOutputLeft;
    delay->output[1] = sDelayOutputRight;
    delay->output[2] = sDelayOutputSurround;
}

void Delay::ApplyToSound(void* handle)
{
    AudioSource* voices[8];
    unsigned int count;
    GetSoundSources(handle, voices, &count);

    int volume = (int)(-960.0f * (1.0f - m_Final.m_AuxVolume));
    int auxIndex = g_pAuxEffectMap->GetAuxiliary(sDelaySendEffectId);
    for (u32 i = 0; i < count; ++i)
        voices[i]->SetAuxiliaryVolume(auxIndex, volume);
}

void Delay::OnSoundStarted(void*)
{
    AXFX_DELAY* delay = &g_pAudioBackend->m_DelayEffect.m_Delay;
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

    if (g_pAudioBackend->m_OutputMode == 3)
        AXFXDelayExpSettingsDpl2(
            (AXFX_DELAY_EXP_DPL2*)&g_pAudioBackend->m_DelayEffect);
    else
        AXFXDelaySettings(delay);
}

void Delay::EndBlend()
{
    if (m_Parameters.m_Head == 0 && m_Final.m_AuxVolume < FLT_MIN)
        m_Enabled = true;
}

