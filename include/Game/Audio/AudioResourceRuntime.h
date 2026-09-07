#ifndef GAME_AUDIO_AUDIO_RESOURCE_RUNTIME_H
#define GAME_AUDIO_AUDIO_RESOURCE_RUNTIME_H

#include "types.h"

class AudioConfigNode;
class AudioBundleManager;

class AudioResourceRuntime
{
public:
    AudioResourceRuntime();

    virtual void* CreateRoot();
    virtual bool Load(void* data, unsigned int size, bool unknown);
    virtual void* GetLoadedRoot();
    virtual void* GetRoot();

    void LoadScriptData(void* data, unsigned int size);
    AudioConfigNode* GetConfigRoot() const { return m_ConfigRoot; }

private:
    friend class AudioBundleManager;

    u8 m_Unknown04[0x1C];
    AudioConfigNode* m_ConfigRoot;
    void* m_Unknown24;
    void* m_Unknown28;
};

extern AudioResourceRuntime* g_pAudioResourceRuntime;

#endif // GAME_AUDIO_AUDIO_RESOURCE_RUNTIME_H
