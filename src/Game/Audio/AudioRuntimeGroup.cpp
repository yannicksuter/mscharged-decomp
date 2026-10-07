#include "NL/nlDLListContainer.inl"
#include "Game/Audio/AudioEffect.h"

AudioEffectBase::AudioEffectBase(const char*)
    : m_Finished(false)
    , m_Parameters()
    , m_CurrentParameter(0)
    , m_ResultParameter(0)
{
}

void AudioEffectBase::Update(float dt)
{
    BeginBlend();
    nlDLListIterator<AudioEffectParameter*> iterator;
    iterator = m_Parameters.Begin();
    while (iterator.hasNext())
    {
        AudioEffectParameter* state = *iterator;
        state->Update(dt);
        BlendParameter(m_ResultParameter, state);
        if (state->IsFinished())
        {
            OnParameterFinished(state);
            m_Parameters.RemoveEntry(iterator.next());
            ReleaseParameter(state);
        }
        iterator.Step();
    }
    EndBlend();
}
