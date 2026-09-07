#include "Game/Audio/AudioResourceLoader.h"

#include "NL/MemAlloc.h"

AudioResourceLoader::AudioResourceLoader(
    AudioResourceLoadOwner* owner)
    : m_Owner(owner)
    , m_Allocator(CurrentAllocator)
{
}

void AudioResourceLoader::OnLoadComplete()
{
    AudioResourceLoadOwner* owner = m_Owner;
    owner->m_Completed = true;
    if (owner->m_Callback != 0)
    {
        owner->m_Callback(owner, owner->m_CallbackParam);
    }
}
