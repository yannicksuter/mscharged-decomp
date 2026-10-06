#include "revolution/types.h"
#include "NL/nlDLListContainer.inl"
#include "revolution/ax.h"
#include "revolution/axfx.h"

#include "Game/Audio/Reverb.h"
#include "Game/Audio/AudioBackend.h"
#include "Game/Audio/AudioConfig.h"
#include "Game/Audio/AudioSource.h"
#include "Game/Audio/XSoundCueHandle.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "NL/nlString.h"

#include <float.h>

static float sReverbTime = 5.0f;
static float sReverbPreDelay = 0.1f;
static float sReverbDamping = 0.5f;
static float sReverbColoration = 0.5f;
static float sReverbCrosstalk = 0.3f;
static float sReverbMix = 1.0f;

SlotPool<ReverbParameter> ReverbParameter::s_Pool(16, 16);
SlotPool<Reverb> Reverb::s_Pool(16, 16);

bool gReverbOverrideEnabled;
float gReverbOverrideVolume;

inline ReverbParameter::ReverbParameter()
    : tempDisableFX(false)
    , time(sReverbTime)
    , preDelay(sReverbPreDelay)
    , damping(sReverbDamping)
    , coloration(sReverbColoration)
    , crosstalk(sReverbCrosstalk)
    , mix(sReverbMix)
    , auxvol(0.0f)
{
}

static void ProcessReverb(void* channels, void*)
{
    AXFXReverbHiCallback(channels, g_pAudioBackend->GetReverb());
}

static void ProcessReverbDpl2(void* channels, void*)
{
    AXFXReverbHiCallbackDpl2(channels, g_pAudioBackend->GetReverbDpl2());
}

Reverb::Reverb()
    : AudioEffectBase("Reverb")
    , m_Initial()
    , m_Final()
{
    AXAuxCallback callback;
    void* context = 0;
    AudioBackend* platform = g_pAudioBackend;
    AXFX_REVERBHI* reverb = platform->GetReverb();
    switch (g_pAuxEffectMap->GetAuxiliary(1))
    {
    case 0:
        AXGetAuxACallback(&callback, &context);
        if (platform->m_OutputMode == 3)
        {
            if (callback != ProcessReverbDpl2)
                AXRegisterAuxACallback(ProcessReverbDpl2, reverb);
        }
        else if (callback != ProcessReverb)
        {
            AXRegisterAuxACallback(ProcessReverb, reverb);
        }
        break;
    case 1:
        AXGetAuxBCallback(&callback, &context);
        if (platform->m_OutputMode == 3)
        {
            if (callback != ProcessReverbDpl2)
                AXRegisterAuxBCallback(ProcessReverbDpl2, reverb);
        }
        else if (callback != ProcessReverb)
        {
            AXRegisterAuxBCallback(ProcessReverb, reverb);
        }
        break;
    }
    m_CurrentParameter = &m_Initial;
    m_ResultParameter = &m_Final;
}

void Reverb::CreateParameter(unsigned int definition, const void*, bool negate,
    AudioEffectParameter** output)
{
    AudioConfigNode* node = ConfigFindDefinition(definition);
    ReverbParameter* parameter = new ReverbParameter;
    *output = parameter;
    if (gReverbOverrideEnabled)
    {
        m_Final.time = sReverbTime;
        m_Final.preDelay = sReverbPreDelay;
        m_Final.damping = sReverbDamping;
        m_Final.coloration = sReverbColoration;
        m_Final.crosstalk = sReverbCrosstalk;
        m_Final.mix = sReverbMix;
        m_Final.auxvol = gReverbOverrideVolume;
        *parameter = m_Final;
        return;
    }

    parameter->tempDisableFX =
        node->Get(nlStringLowerHash("tempDisableFX")).m_Words.m_Value != 0;
    parameter->time = node->Get(nlStringLowerHash("time")).m_Float;
    parameter->preDelay = node->Get(nlStringLowerHash("preDelay")).m_Float;
    parameter->damping = node->Get(nlStringLowerHash("damping")).m_Float;
    parameter->coloration = node->Get(nlStringLowerHash("coloration")).m_Float;
    parameter->crosstalk = node->Get(nlStringLowerHash("crosstalk")).m_Float;
    parameter->mix = node->Get(nlStringLowerHash("mix")).m_Float;
    parameter->auxvol = node->Get(nlStringLowerHash("auxvol")).m_Float;
    parameter->auxvol = negate ? 1.0f - parameter->auxvol : parameter->auxvol;
}

void Reverb::BlendParameter(AudioEffectParameter* destination,
    AudioEffectParameter* source)
{
    ReverbParameter* destinationParameter = (ReverbParameter*)destination;
    ReverbParameter* sourceParameter = (ReverbParameter*)source;
    destinationParameter->time = sourceParameter->time;
    destinationParameter->preDelay = sourceParameter->preDelay;
    destinationParameter->damping = sourceParameter->damping;
    destinationParameter->coloration = sourceParameter->coloration;
    destinationParameter->crosstalk = sourceParameter->crosstalk;
    destinationParameter->mix = sourceParameter->mix;
    destinationParameter->auxvol = sourceParameter->auxvol;
}

void SetDefaultReverbSettings(AXFX_REVERBHI* reverb)
{
    reverb->time = sReverbTime;
    reverb->preDelay = sReverbPreDelay;
    reverb->damping = sReverbDamping;
    reverb->coloration = sReverbColoration;
    reverb->crosstalk = sReverbCrosstalk;
    reverb->mix = sReverbMix;
}

void ReverbParameter::ApplySettings(AXFX_REVERBHI* reverb)
{
    float value = time >= 0.0f ? time : 0.0f;
    reverb->time = value <= 10.0f ? value : 10.0f;
    value = preDelay >= 0.0f ? preDelay : 0.0f;
    reverb->preDelay = value <= 0.1f ? value : 0.1f;
    value = damping >= 0.0f ? damping : 0.0f;
    reverb->damping = value <= 1.0f ? value : 1.0f;
    value = coloration >= 0.0f ? coloration : 0.0f;
    reverb->coloration = value <= 1.0f ? value : 1.0f;
    value = crosstalk >= 0.0f ? crosstalk : 0.0f;
    reverb->crosstalk = value <= 1.0f ? value : 1.0f;
    value = mix >= 0.0f ? mix : 0.0f;
    reverb->mix = value <= 1.0f ? value : 1.0f;
}

void Reverb::ApplyToSound(void* handle)
{
    AudioSource* voices[8];
    unsigned int count;
    GetSoundSources(handle, voices, &count);
    int volume = (int)(-960.0f * (1.0f - m_Final.auxvol));
    int auxIndex = g_pAuxEffectMap->GetAuxiliary(1);
    for (unsigned int i = 0; i < count; ++i)
        voices[i]->SetAuxiliaryVolume(auxIndex, volume);
}

void Reverb::OnSoundStarted(void*)
{
    AXFX_REVERBHI* reverb = g_pAudioBackend->GetReverb();
    if (g_pAudioBackend->m_OutputMode == 3)
    {
        m_Final.ApplySettings(reverb);
        AXFXReverbHiSettingsDpl2((AXFX_REVERBHI_DPL2*)reverb);
    }
    else
    {
        m_Final.ApplySettings(reverb);
        AXFXReverbHiSettings(reverb);
    }
}

void Reverb::EndBlend()
{
    if (m_Parameters.m_Head == 0 && m_Final.auxvol < FLT_MIN)
        m_Enabled = true;
}
