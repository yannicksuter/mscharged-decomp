#include "Game/Audio/AudioRuntimeGroup_802F98F4.h"

AudioRuntimeGroup_802F98F4::AudioRuntimeGroup_802F98F4(const char*)
    : m_Unknown04(false)
    , m_Unknown08()
    , m_Unknown24(0)
    , m_Unknown28(0)
{
}

void AudioRuntimeGroup_802F98F4::fn_802F9974(float dt)
{
    fn_802F9B5C();
    nlDLListIterator<AudioState_802F69A8*> iterator = m_Unknown08.Begin();
    while (iterator.hasNext())
    {
        AudioState_802F69A8* state = *iterator;
        state->fn_802F6A60(dt);
        fn_802F9B60(m_Unknown28, state);
        if (state->fn_802F69E8())
        {
            fn_802F9B64(state);
            m_Unknown08.Remove(&iterator);
            fn_802F9B68(state);
        }
        iterator.next();
    }
    fn_802F9B8C();
}
