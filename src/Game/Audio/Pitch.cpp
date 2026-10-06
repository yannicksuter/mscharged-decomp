#include "Game/Audio/Pitch.h"

static float sPitchSemitones = 1.0f;

SlotPool<PitchParameter> PitchParameter::s_Pool(16, 16);
SlotPool<Pitch> Pitch::s_Pool(16, 16);

static u32 sSemitonesKey = nlStringLowerHash("semitones");
bool gPitchOverrideEnabled;

PitchParameter::PitchParameter()
    : m_Semitones(0.0f)
{
}

void Pitch::CreateParameter(unsigned int definition, const void*, bool negate,
    AudioEffectParameter** output)
{
    PitchParameter* parameter = new PitchParameter;
    *output = parameter;

    if (gPitchOverrideEnabled)
    {
        m_Final.m_Semitones = sPitchSemitones;
        *parameter = m_Final;
        return;
    }

    AudioConfigNode* node = ConfigFindDefinition(definition);
    u32 semitonesKey = sSemitonesKey;
    parameter->m_Semitones = node->Get(semitonesKey).m_Float;
    parameter->m_Semitones = negate ? -parameter->m_Semitones : parameter->m_Semitones;
}

void Pitch::BlendParameter(AudioEffectParameter* destination,
    AudioEffectParameter* source)
{
    PitchParameter* destinationParameter = (PitchParameter*)destination;
    PitchParameter* sourceParameter = (PitchParameter*)source;

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

void Pitch::EndBlend()
{
    if (m_Parameters.m_Head == 0 && nlNear(m_Initial.m_Semitones, 0.0f))
        m_Enabled = true;
}

void Pitch::ApplyToSound(void* handle)
{
    AudioSource* voices[8];
    unsigned int count;
    GetSoundSources(handle, voices, &count);
    for (u16 i = 0; i < count; ++i)
    {
        float semitones = m_Final.m_Semitones;
        semitones = semitones >= -100.0f ? semitones : -100.0f;
        semitones = semitones <= 100.0f ? semitones : 100.0f;
        voices[i]->SetPitch(semitones);
    }
}

void Pitch::OnParameterFinished(AudioEffectParameter* parameter)
{
    m_Initial.m_Semitones += ((PitchParameter*)parameter)->m_Semitones;
}

PitchParameter::~PitchParameter()
{
}

void Pitch::BeginBlend()
{
    m_Final = m_Initial;
}
