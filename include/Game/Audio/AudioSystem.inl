#ifndef GAME_AUDIO_AUDIO_SYSTEM_INL
#define GAME_AUDIO_AUDIO_SYSTEM_INL

#include "Game/Audio/AudioBundleManager.h"
#include "Game/Audio/AudioSystem.h"

inline bool AudioSystem::IsInitialized()
{
    return m_BundleManager != 0 && m_BundleManager->IsInitialized();
}

inline bool AudioSystem::IsAsyncLoading()
{
    return m_AsyncLoading;
}

#endif // GAME_AUDIO_AUDIO_SYSTEM_INL
