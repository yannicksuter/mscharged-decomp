#ifndef GAME_AUDIO_AUDIO_RESOURCE_RUNTIME_H
#define GAME_AUDIO_AUDIO_RESOURCE_RUNTIME_H

#include "types.h"
#include "Game/Audio/AudioRegistryOwner.h"

class AudioConfigNode;
class AudioBundleManager;
class AudioEffectFactory;
class AudioScriptRuntime;

// The audio bundle's packed configuration tree and script runtime.
class AudioResourceRuntime
{
public:
    AudioResourceRuntime();

    /* 0x00 */ AudioRegistryOwner m_Registry;

    void LoadScriptData(void* data, unsigned int size);
    void UnidentifiedApplyEffectSet(u32 bindingKey, u32 effectSetKey,
        bool inverted, void* owner);
    void UnidentifiedApplyEffectSet(u32 bindingKey, u32 effectSetKey,
        bool inverted, float duration);
    AudioConfigNode* GetConfigRoot() const { return m_ConfigRoot; }

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
