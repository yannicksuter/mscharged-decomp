#include "revolution/ax.h"
#include "revolution/axfx.h"

#include "Game/Audio/Reverb.h"
#include "Game/Audio/AudioBackend_8035B8E8.h"
#include "Game/Audio/AudioConfig_8035B240.h"
#include "Game/Audio/AudioSource_8035EF08.h"
#include "NL/nlString.h"

#include <float.h>

static float lbl_806DF9F8 = 5.0f;
static float lbl_806DF9FC = 0.1f;
static float lbl_806DFA00 = 0.5f;
static float lbl_806DFA04 = 0.5f;
static float lbl_806DFA08 = 0.3f;
static float lbl_806DFA0C = 1.0f;

SlotPool<ReverbParameter_80363D60> ReverbParameter_80363D60::s_Pool(16, 16);
SlotPool<Reverb> Reverb::s_Pool(16, 16);

bool lbl_806E2250;
float lbl_806E2254;

extern AuxEffectMap_8035952C* lbl_806E21B8;
extern "C" void fn_802F1A84(void*, AudioSource_8035C234**, unsigned int*);

inline ReverbParameter_80363D60::ReverbParameter_80363D60()
    : tempDisableFX(false)
    , time(lbl_806DF9F8)
    , preDelay(lbl_806DF9FC)
    , damping(lbl_806DFA00)
    , coloration(lbl_806DFA04)
    , crosstalk(lbl_806DFA08)
    , mix(lbl_806DFA0C)
    , auxvol(0.0f)
{
}

static void fn_803632F8(void* channels, void*)
{
    AXFXReverbHiCallback(channels, &lbl_806E2020->m_Unknown454.m_Reverb);
}

static void fn_80363304(void* channels, void*)
{
    AXFXReverbHiCallbackDpl2(channels, &lbl_806E2020->m_Unknown454.m_ReverbDpl2);
}

Reverb::Reverb()
    : AudioEffectBase_802F98F4("Reverb")
    , m_Initial()
    , m_Final()
{
    AXAuxCallback callback;
    void* context = 0;
    AudioBackend_8035B8E8* platform = lbl_806E2020;
    void* reverb = &platform->m_Unknown454;
    switch (lbl_806E21B8->fn_80359590(1))
    {
    case 0:
        AXGetAuxACallback(&callback, &context);
        if (platform->m_OutputMode == 3)
        {
            if (callback != fn_80363304)
                AXRegisterAuxACallback(fn_80363304, reverb);
        }
        else if (callback != fn_803632F8)
        {
            AXRegisterAuxACallback(fn_803632F8, reverb);
        }
        break;
    case 1:
        AXGetAuxBCallback(&callback, &context);
        if (platform->m_OutputMode == 3)
        {
            if (callback != fn_80363304)
                AXRegisterAuxBCallback(fn_80363304, reverb);
        }
        else if (callback != fn_803632F8)
        {
            AXRegisterAuxBCallback(fn_803632F8, reverb);
        }
        break;
    }
    m_CurrentParameter = &m_Initial;
    m_ResultParameter = &m_Final;
}

void Reverb::fn_802F6930(unsigned int definition, void*, bool negate,
    AudioEffectParameter_802F69A8** output)
{
    ConfigNode_8035B240* node = ConfigFindDefinition_8035B240(definition);
    ReverbParameter_80363D60* parameter = new ReverbParameter_80363D60;
    *output = parameter;
    if (lbl_806E2250)
    {
        m_Final.time = lbl_806DF9F8;
        m_Final.preDelay = lbl_806DF9FC;
        m_Final.damping = lbl_806DFA00;
        m_Final.coloration = lbl_806DFA04;
        m_Final.crosstalk = lbl_806DFA08;
        m_Final.mix = lbl_806DFA0C;
        m_Final.auxvol = lbl_806E2254;
        *parameter = m_Final;
        return;
    }

    ConfigValue_8035B240 value;
    value.m_Raw = node->fn_8035B240(nlStringLowerHash("tempDisableFX"));
    parameter->tempDisableFX = value.m_Words.m_Value != 0;
    value.m_Raw = node->fn_8035B240(nlStringLowerHash("time"));
    parameter->time = value.m_Float;
    value.m_Raw = node->fn_8035B240(nlStringLowerHash("preDelay"));
    parameter->preDelay = value.m_Float;
    value.m_Raw = node->fn_8035B240(nlStringLowerHash("damping"));
    parameter->damping = value.m_Float;
    value.m_Raw = node->fn_8035B240(nlStringLowerHash("coloration"));
    parameter->coloration = value.m_Float;
    value.m_Raw = node->fn_8035B240(nlStringLowerHash("crosstalk"));
    parameter->crosstalk = value.m_Float;
    value.m_Raw = node->fn_8035B240(nlStringLowerHash("mix"));
    parameter->mix = value.m_Float;
    value.m_Raw = node->fn_8035B240(nlStringLowerHash("auxvol"));
    parameter->auxvol = value.m_Float;
    parameter->auxvol = negate ? 1.0f - parameter->auxvol : parameter->auxvol;
}

void Reverb::fn_802F9B60(AudioEffectParameter_802F69A8* destination,
    AudioEffectParameter_802F69A8* source)
{
    ReverbParameter_80363D60* destinationParameter = (ReverbParameter_80363D60*)destination;
    ReverbParameter_80363D60* sourceParameter = (ReverbParameter_80363D60*)source;
    destinationParameter->time = sourceParameter->time;
    destinationParameter->preDelay = sourceParameter->preDelay;
    destinationParameter->damping = sourceParameter->damping;
    destinationParameter->coloration = sourceParameter->coloration;
    destinationParameter->crosstalk = sourceParameter->crosstalk;
    destinationParameter->mix = sourceParameter->mix;
    destinationParameter->auxvol = sourceParameter->auxvol;
}

extern "C" void fn_803638BC(AXFX_REVERBHI* reverb)
{
    reverb->time = lbl_806DF9F8;
    reverb->preDelay = lbl_806DF9FC;
    reverb->damping = lbl_806DFA00;
    reverb->coloration = lbl_806DFA04;
    reverb->crosstalk = lbl_806DFA08;
    reverb->mix = lbl_806DFA0C;
}

void Reverb::fn_802F98F0(void* handle)
{
    AudioSource_8035C234* voices[8];
    unsigned int count;
    fn_802F1A84(handle, voices, &count);
    int volume = (int)(-960.0f * (1.0f - m_Final.auxvol));
    int auxIndex = lbl_806E21B8->fn_80359590(1);
    for (unsigned int i = 0; i < count; ++i)
        voices[i]->fn_5C(auxIndex, volume);
}

void Reverb::fn_802F692C(void*)
{
    AXFX_REVERBHI* reverb = &lbl_806E2020->m_Unknown454.m_Reverb;
    if (lbl_806E2020->m_OutputMode == 3)
    {
        float value = m_Final.time >= 0.0f ? m_Final.time : 0.0f;
        reverb->time = value <= 10.0f ? value : 10.0f;
        value = m_Final.preDelay >= 0.0f ? m_Final.preDelay : 0.0f;
        reverb->preDelay = value <= 0.1f ? value : 0.1f;
        value = m_Final.damping >= 0.0f ? m_Final.damping : 0.0f;
        reverb->damping = value <= 1.0f ? value : 1.0f;
        value = m_Final.coloration >= 0.0f ? m_Final.coloration : 0.0f;
        reverb->coloration = value <= 1.0f ? value : 1.0f;
        value = m_Final.crosstalk >= 0.0f ? m_Final.crosstalk : 0.0f;
        reverb->crosstalk = value <= 1.0f ? value : 1.0f;
        value = m_Final.mix >= 0.0f ? m_Final.mix : 0.0f;
        reverb->mix = value <= 1.0f ? value : 1.0f;
        AXFXReverbHiSettingsDpl2((AXFX_REVERBHI_DPL2*)reverb);
    }
    else
    {
        float value = m_Final.time >= 0.0f ? m_Final.time : 0.0f;
        reverb->time = value <= 10.0f ? value : 10.0f;
        value = m_Final.preDelay >= 0.0f ? m_Final.preDelay : 0.0f;
        reverb->preDelay = value <= 0.1f ? value : 0.1f;
        value = m_Final.damping >= 0.0f ? m_Final.damping : 0.0f;
        reverb->damping = value <= 1.0f ? value : 1.0f;
        value = m_Final.coloration >= 0.0f ? m_Final.coloration : 0.0f;
        reverb->coloration = value <= 1.0f ? value : 1.0f;
        value = m_Final.crosstalk >= 0.0f ? m_Final.crosstalk : 0.0f;
        reverb->crosstalk = value <= 1.0f ? value : 1.0f;
        value = m_Final.mix >= 0.0f ? m_Final.mix : 0.0f;
        reverb->mix = value <= 1.0f ? value : 1.0f;
        AXFXReverbHiSettings(reverb);
    }
}

void Reverb::fn_802F9B8C()
{
    if (m_Parameters.m_Head == 0 && m_Final.auxvol < FLT_MIN)
        m_Enabled = true;
}
