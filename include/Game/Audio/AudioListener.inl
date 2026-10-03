#ifndef GAME_AUDIO_AUDIO_LISTENER_INL
#define GAME_AUDIO_AUDIO_LISTENER_INL

#include "Game/Audio/AudioSystem.h"

void AudioListener::SetEnabled(bool enabled)
{
    m_Enabled = enabled;
}

void AudioListener::SetTransformValid(bool valid)
{
    m_TransformValid = valid;
}

#endif // GAME_AUDIO_AUDIO_LISTENER_INL
