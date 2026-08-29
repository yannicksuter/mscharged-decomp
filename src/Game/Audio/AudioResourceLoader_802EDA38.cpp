#include "Game/Audio/AudioResourceLoader_802EDA38.h"

#include "NL/MemAlloc.h"

AudioResourceLoader_802EDA38::AudioResourceLoader_802EDA38(
    AudioResourceLoadOwner_802EDA54* owner)
    : m_Owner(owner)
    , m_Allocator(CurrentAllocator)
{
}

void AudioResourceLoader_802EDA38::fn_802EDA54()
{
    AudioResourceLoadOwner_802EDA54* owner = m_Owner;
    owner->m_Completed = true;
    if (owner->m_Callback != 0)
    {
        owner->m_Callback(owner, owner->m_CallbackParam);
    }
}
