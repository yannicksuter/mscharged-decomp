#ifndef GAME_AUDIO_AUDIO_REGISTRY_OWNER_H
#define GAME_AUDIO_AUDIO_REGISTRY_OWNER_H

#include "NL/nlRegistry.h"

struct UnidentifiedAudioPoolOwner
{
    ~UnidentifiedAudioPoolOwner();
};

// Registry storage backed by the shared audio container and node pools.
class AudioRegistryOwner : public RegistryOwner
{
public:
    virtual void FreeItem(void* data);
    virtual void FreeNode(void* node);
    virtual void FreeContainer(void* container);
    virtual void* AllocItem(unsigned int size);
    virtual RegistryNode* AllocNode();
    virtual RegistryContainer* AllocContainer();

    /* 0x1C */ UnidentifiedAudioPoolOwner m_Pools;
}; // size: 0x20

#endif // GAME_AUDIO_AUDIO_REGISTRY_OWNER_H
