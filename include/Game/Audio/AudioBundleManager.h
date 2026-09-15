#ifndef GAME_AUDIO_AUDIO_BUNDLE_MANAGER_H
#define GAME_AUDIO_AUDIO_BUNDLE_MANAGER_H

#include "types.h"
#include "Game/Audio/AudioResourceRuntime.h"

class nlChunk;
class AudioBackend;
struct AudioBankTable;

class AudioBundleManager
{
public:
    AudioBundleManager();

    virtual bool IsLoaded() { return m_Loaded; }
    virtual bool IsInitialized() { return m_Initialized; }
    virtual void Load(const char* path);
    virtual bool Initialize();
    virtual void Shutdown() = 0;
    virtual void Update(float deltaTime) = 0;
    virtual void OnLoadStarted() { }
    virtual void ParseChunk(nlChunk* chunk);
    virtual void OnLoadComplete() { m_Loaded = true; }

    AudioBankTable* GetSoundMap() const { return m_Chunk13500; }
    void* GetSliderTable() const { return m_Chunk13100; }
    void* GetCalculationTable() const { return m_Chunk13400; }
    AudioBackend* GetBackend() const { return m_Backend; }

protected:
    static void OnBundleLoaded(
        void* data, unsigned long size, void* manager);

    AudioBackend* m_Backend;
    bool m_Initialized;
    u8 m_Unknown09[3];
    void* m_Chunk13100;
    void* m_Chunk13400;
    AudioBankTable* m_Chunk13500;
    AudioResourceRuntime m_Runtime;
    bool m_Loaded;
    u8 m_Unknown45[3];
    void* m_LoadedData;
};

class UnidentifiedAudioBundleManager_802ECD34 : public AudioBundleManager
{
public:
    UnidentifiedAudioBundleManager_802ECD34()
        : m_Unknown4C(0)
        , m_RpcController(0)
    {
    }

    virtual bool Initialize();
    virtual void Shutdown();
    virtual void Update(float dt);
    virtual void ParseChunk(nlChunk* chunk);
    virtual ~UnidentifiedAudioBundleManager_802ECD34();

private:
    void* m_Unknown4C;
    void* m_RpcController;
};

#endif // GAME_AUDIO_AUDIO_BUNDLE_MANAGER_H
