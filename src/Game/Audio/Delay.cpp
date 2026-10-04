#include "Game/Audio/AudioSource.h"
#include "Game/Audio/AudioConfig.h"
#include "revolution/ax.h"
#include "revolution/axfx.h"

#include "Game/Audio/Delay.h"
#include "Game/Audio/AudioBackend.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "NL/nlString.h"

#include <float.h>

static u32 sDelayLeft = 300;
static u32 sDelayRight = 300;
static u32 sDelaySurround = 300;
static u32 sDelayFeedbackLeft = 50;
static u32 sDelayFeedbackRight = 50;
static u32 sDelayFeedbackSurround = 50;
static u32 sDelayOutputLeft = 100;
static u32 sDelayOutputRight = 100;
static u32 sDelayOutputSurround = 100;

inline DelayParameter::DelayParameter()
    : m_AuxVolume(0.0f)
{
    m_Delay[0] = sDelayLeft;
    m_Delay[1] = sDelayRight;
    m_Delay[2] = sDelaySurround;
    m_Feedback[0] = sDelayFeedbackLeft;
    m_Feedback[1] = sDelayFeedbackRight;
    m_Feedback[2] = sDelayFeedbackSurround;
    m_Output[0] = sDelayOutputLeft;
    m_Output[1] = sDelayOutputRight;
    m_Output[2] = sDelayOutputSurround;
}

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
    AudioBackend* platform = g_pAudioBackend;
    AXFX_DELAY* delayEffect = platform->GetDelay();
    switch (g_pAuxEffectMap->GetAuxiliary(0))
    {
    case 0:
        AXGetAuxACallback(&callback, &context);
        if (platform->m_OutputMode == 3)
        {
            if (callback != ProcessDelayDpl2)
                AXRegisterAuxACallback(ProcessDelayDpl2, delayEffect);
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
            if (callback != ProcessDelayDpl2)
                AXRegisterAuxBCallback(ProcessDelayDpl2, delayEffect);
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

void Delay::CreateParameter(unsigned int definition, const void*, bool negate,
    AudioEffectParameter** output)
{
    AudioConfigNode* node = ConfigFindDefinition(definition);

    DelayParameter* parameter = new DelayParameter;
    *output = parameter;

    if (gDelayOverrideEnabled)
    {
        m_Final.m_Delay[0] = sDelayLeft;
        m_Final.m_Delay[1] = sDelayRight;
        m_Final.m_Delay[2] = sDelaySurround;
        m_Final.m_Feedback[0] = sDelayFeedbackLeft;
        m_Final.m_Feedback[1] = sDelayFeedbackRight;
        m_Final.m_Feedback[2] = sDelayFeedbackSurround;
        m_Final.m_Output[0] = sDelayOutputLeft;
        m_Final.m_Output[1] = sDelayOutputRight;
        m_Final.m_Output[2] = sDelayOutputSurround;
        m_Final.m_AuxVolume = gDelayOverrideVolume;

        *parameter = m_Final;
        return;
    }

    parameter->m_Delay[0] = node->Get(nlStringLowerHash("delayL")).m_Words.m_Value;
    parameter->m_Delay[1] = node->Get(nlStringLowerHash("delayR")).m_Words.m_Value;
    parameter->m_Delay[2] = node->Get(nlStringLowerHash("delayS")).m_Words.m_Value;
    parameter->m_Feedback[0] = node->Get(nlStringLowerHash("feedbackL")).m_Words.m_Value;
    parameter->m_Feedback[1] = node->Get(nlStringLowerHash("feedbackR")).m_Words.m_Value;
    parameter->m_Feedback[2] = node->Get(nlStringLowerHash("feedbackS")).m_Words.m_Value;
    parameter->m_Output[0] = node->Get(nlStringLowerHash("outputL")).m_Words.m_Value;
    parameter->m_Output[1] = node->Get(nlStringLowerHash("outputR")).m_Words.m_Value;
    parameter->m_Output[2] = node->Get(nlStringLowerHash("outputS")).m_Words.m_Value;
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
        destinationParameter->m_Delay[i]
            = sourceParameter->m_Delay[i];
        destinationParameter->m_Feedback[i]
            = sourceParameter->m_Feedback[i];
        destinationParameter->m_Output[i]
            = sourceParameter->m_Output[i];
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
    int auxIndex = g_pAuxEffectMap->GetAuxiliary(0);
    for (u32 i = 0; i < count; ++i)
        voices[i]->SetAuxiliaryVolume(auxIndex, volume);
}

inline void DelayParameter::ApplySettings(AXFX_DELAY* delay)
{
    for (u16 i = 0; i < 3; ++i)
    {
        u32 value = m_Delay[i];
        value = value >= 1 ? value : 1;
        delay->delay[i] = value <= 750 ? value : 750;
        value = m_Feedback[i];
        delay->feedback[i] = value <= 99 ? value : 99;
        value = m_Output[i];
        delay->output[i] = value <= 100 ? value : 100;
    }
}

void Delay::OnSoundStarted(void*)
{
    AXFX_DELAY* delay = g_pAudioBackend->GetDelay();
    if (g_pAudioBackend->m_OutputMode == 3)
    {
        m_Final.ApplySettings(delay);
        AXFXDelayExpSettingsDpl2((AXFX_DELAY_EXP_DPL2*)delay);
    }
    else
    {
        m_Final.ApplySettings(delay);
        AXFXDelaySettings(delay);
    }
}

void Delay::EndBlend()
{
    if (m_Parameters.m_Head == 0 && m_Final.m_AuxVolume < FLT_MIN)
        m_Enabled = true;
}
