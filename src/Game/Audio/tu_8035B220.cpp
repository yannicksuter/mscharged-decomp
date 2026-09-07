#include "Game/Audio/AudioEffect_802F98F4.h"
#include "Game/Audio/AudioConfig_8035B240.h"
#include "NL/nlMath.h"
#include "NL/nlSlotPool.h"
#include "NL/nlString.h"

class AudioVoice_8035B4A4
{
public:
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
    virtual void fn_38(float);
};

class PitchParameter_8035B220 : public AudioEffectParameter_802F69A8
{
public:
    PitchParameter_8035B220();
    virtual ~PitchParameter_8035B220() { }

    static void* operator new(unsigned long)
    {
        PitchParameter_8035B220* parameter = 0;
        s_Pool.Allocate(parameter);
        return parameter;
    }

    static void operator delete(void* pointer)
    {
        s_Pool.Free((PitchParameter_8035B220*)pointer);
    }

    float m_Semitones;

    static SlotPool<PitchParameter_8035B220> s_Pool;
};

class Pitch : public AudioEffectBase_802F98F4
{
public:
    virtual ~Pitch() { }
    virtual void fn_802F6930(unsigned int definition, void* context,
        bool negate, AudioEffectParameter_802F69A8** output);
    virtual void fn_802F9B5C();
    virtual void fn_802F9B60(AudioEffectParameter_802F69A8* destination,
        AudioEffectParameter_802F69A8* source);
    virtual void fn_802F9B8C();
    virtual void fn_802F9B64(AudioEffectParameter_802F69A8* parameter);
    virtual void fn_802F98F0(void* handle);

    static void* operator new(unsigned long)
    {
        Pitch* effect = 0;
        s_Pool.Allocate(effect);
        return effect;
    }

    static void operator delete(void* pointer)
    {
        s_Pool.Free((Pitch*)pointer);
    }

    PitchParameter_8035B220 m_Initial;
    PitchParameter_8035B220 m_Final;

    static SlotPool<Pitch> s_Pool;
};

static float lbl_806DF9A0 = 1.0f;

SlotPool<PitchParameter_8035B220> PitchParameter_8035B220::s_Pool(16, 16);
SlotPool<Pitch> Pitch::s_Pool(16, 16);

static u32 lbl_806E21D8 = nlStringLowerHash("semitones");
bool lbl_806E21DC;

extern "C" void fn_802F1A84(
    void* handle, AudioVoice_8035B4A4** voices, u32* count);

PitchParameter_8035B220::PitchParameter_8035B220()
    : m_Semitones(0.0f)
{
}

void Pitch::fn_802F6930(unsigned int definition, void*, bool negate,
    AudioEffectParameter_802F69A8** output)
{
    PitchParameter_8035B220* parameter = new PitchParameter_8035B220;
    *output = parameter;

    if (lbl_806E21DC)
    {
        m_Final.m_Semitones = lbl_806DF9A0;
        *parameter = m_Final;
        return;
    }

    ConfigNode_8035B240* node = ConfigFindDefinition_8035B240(definition);
    u32 semitonesKey = lbl_806E21D8;
    ConfigValue_8035B240 value;
    value.m_Raw = node->fn_8035B240(semitonesKey);
    parameter->m_Semitones = value.m_Float;
    parameter->m_Semitones = negate ? -parameter->m_Semitones : parameter->m_Semitones;
}

void Pitch::fn_802F9B60(AudioEffectParameter_802F69A8* destination,
    AudioEffectParameter_802F69A8* source)
{
    PitchParameter_8035B220* destinationParameter = (PitchParameter_8035B220*)destination;
    PitchParameter_8035B220* sourceParameter = (PitchParameter_8035B220*)source;

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

    destinationParameter->m_Semitones += sourceParameter->m_Semitones * amount;
}

void Pitch::fn_802F9B8C()
{
    if (m_Parameters.m_Head == 0 && nlNear(m_Initial.m_Semitones, 0.0f))
        m_Enabled = true;
}

void Pitch::fn_802F98F0(void* handle)
{
    AudioVoice_8035B4A4* voices[8];
    u32 count;
    fn_802F1A84(handle, voices, &count);
    for (u16 i = 0; i < count; ++i)
    {
        float semitones = m_Final.m_Semitones;
        semitones = semitones >= -100.0f ? semitones : -100.0f;
        semitones = semitones <= 100.0f ? semitones : 100.0f;
        voices[i]->fn_38(semitones);
    }
}

void Pitch::fn_802F9B64(AudioEffectParameter_802F69A8* parameter)
{
    m_Initial.m_Semitones += ((PitchParameter_8035B220*)parameter)->m_Semitones;
}

void Pitch::fn_802F9B5C()
{
    m_Final = m_Initial;
}
