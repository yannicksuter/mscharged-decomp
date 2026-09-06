#include "Game/Audio/AudioEffect_802F98F4.h"

AudioEffectBase_802F98F4::AudioEffectBase_802F98F4(const char*)
    : m_Enabled(false)
    , m_Parameters()
    , m_CurrentParameter(0)
    , m_ResultParameter(0)
{
}

void AudioEffectBase_802F98F4::fn_802F9974(float dt)
{
    fn_802F9B5C();
    nlDLListIterator<AudioEffectParameter_802F69A8*> iterator = m_Parameters.Begin();
    while (iterator.hasNext())
    {
        AudioEffectParameter_802F69A8* state = *iterator;
        state->fn_802F6A60(dt);
        fn_802F9B60(m_ResultParameter, state);
        if (state->fn_802F69E8())
        {
            fn_802F9B64(state);
            m_Parameters.Remove(&iterator);
            fn_802F9B68(state);
        }
        iterator.next();
    }
    fn_802F9B8C();
}
