#include "Game/Audio/AudioEffect.h"

AudioEffectBase::AudioEffectBase(const char*)
    : m_Enabled(false)
    , m_Parameters()
    , m_CurrentParameter(0)
    , m_ResultParameter(0)
{
}

void AudioEffectBase::Update(float dt)
{
    BeginBlend();
    nlDLListIterator<AudioEffectParameter*> iterator = m_Parameters.Begin();
    while (iterator.hasNext())
    {
        AudioEffectParameter* state = *iterator;
        state->Update(dt);
        BlendParameter(m_ResultParameter, state);
        if (state->IsFinished())
        {
            OnParameterFinished(state);
            m_Parameters.Remove(&iterator);
            ReleaseParameter(state);
        }
        iterator.next();
    }
    EndBlend();
}
