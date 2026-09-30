#ifndef GAME_AUDIO_AUDIO_RESOURCE_RUNTIME_H
#define GAME_AUDIO_AUDIO_RESOURCE_RUNTIME_H

#include "types.h"
#include "NL/nlRegistry.h"

class AudioConfigNode;
class AudioBundleManager;
class AudioEffectFactory;
class AudioScriptRuntime;

// Sub-object the runtime holds so that destroying the runtime returns both
// shared sound pools' blocks. Only its position (0x1C) and the two frees its
// destructor performs are visible in the DOL.
struct UnidentifiedAudioPoolOwner
{
    ~UnidentifiedAudioPoolOwner();
};

// Registry owner for the audio bundle's packed configuration tree. It supplies
// the container and node storage from the two shared sound pools and owns the
// script runtime the bundle drives.
class AudioResourceRuntime : public RegistryOwner
{
public:
    AudioResourceRuntime();

    virtual RegistryContainer* AllocContainer();
    virtual RegistryNode* AllocNode();
    virtual void* AllocItem(unsigned int size);
    virtual void FreeContainer(void* container);
    virtual void FreeNode(void* node);
    virtual void FreeItem(void* data);
    virtual ~AudioResourceRuntime();

    void LoadScriptData(void* data, unsigned int size);
    void UnidentifiedApplyEffectSet(u32 bindingKey, u32 effectSetKey,
        bool inverted, void* owner);
    void UnidentifiedApplyEffectSet(u32 bindingKey, u32 effectSetKey,
        bool inverted, float duration);
    AudioConfigNode* GetConfigRoot() const { return m_ConfigRoot; }

    /* 0x1C */ UnidentifiedAudioPoolOwner m_Pools;
    /* 0x20 */ AudioConfigNode* m_ConfigRoot;
    /* 0x24 */ AudioScriptRuntime* m_Script;
    /* 0x28 */ AudioEffectFactory* m_EffectFactory;
}; // size: 0x2C

extern AudioResourceRuntime* g_pAudioResourceRuntime;

extern "C" void fn_802F4904(AudioResourceRuntime* runtime, float deltaTime);
extern "C" void fn_802F4958(AudioResourceRuntime* runtime);
void NotifyAudioSoundStarted(AudioResourceRuntime* runtime, u32 hash, u32 instance);
void NotifyAudioSoundStopped(AudioResourceRuntime* runtime, u32 instance);
extern "C" bool fn_802F4E84(const u32* hash, bool invert, void* owner);

#endif // GAME_AUDIO_AUDIO_RESOURCE_RUNTIME_H
