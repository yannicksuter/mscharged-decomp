#ifndef GAME_AUDIO_AUDIO_SYSTEM_H
#define GAME_AUDIO_AUDIO_SYSTEM_H

#include "Game/Audio/AudioGlobals.h"
#include "Game/Audio/AudioBundleManager.h"
#include "Game/Audio/AudioListener.h"
#include "NL/nlMath.h"
#include "NL/nlDLListContainer.h"
#include "NL/CircularQueue.h"
#include "types.h"

class XSoundHandle;
class XSoundCueHandle;
class Plat3dSoundSrc;
struct XSoundOwner;
struct AudioHandleState;

struct AudioValues
{
    /* 0x00 */ u8 m_unk00[0x5C];
    /* 0x5C */ float m_unk5C;
    /* 0x60 */ float m_unk60;
    /* 0x64 */ u8 m_unk64[0x4];
    /* 0x68 */ float m_unk68;
    /* 0x6C */ float m_unk6C;
    /* 0x70 */ u8 m_unk70[0x14];
    /* 0x84 */ float m_unk84;
    /* 0x88 */ float m_unk88;
    /* 0x8C */ u8 m_unk8C[0x4];
    /* 0x90 */ float m_unk90;
    /* 0x94 */ float m_unk94;
    /* 0x98 */ u8 m_unk98[0x14];
    /* 0xAC */ float m_unkAC;
    /* 0xB0 */ float m_unkB0;
    /* 0xB4 */ u8 m_unkB4[0x4];
    /* 0xB8 */ float m_unkB8;
    /* 0xBC */ float m_unkBC;
};

struct AudioValuesOwner
{
    /* 0x00 */ u8 m_unk00[0x8];
    /* 0x08 */ AudioValues* m_unk08;
};

struct AudioRuntime
{
    /* 0x00 */ u8 m_unk00[0x10];
    /* 0x10 */ AudioValuesOwner* m_unk10;
};

class AudioSystem
{
public:
    AudioSystem();
    ~AudioSystem();

    virtual inline bool IsAsyncLoading();
    virtual inline bool IsInitialized();
    virtual bool IsIdle();
    virtual void Shutdown();
    virtual void SetResourcePath(const char* path);

    bool UpdateSoundSource(float dt, Plat3dSoundSrc& source);

    void PauseTrackedSound(
        const unsigned long& key, XSoundHandle** handle);
    void ResumeTrackedSound(
        const unsigned long& key, AudioHandleState* state);

    AudioBundleManager* GetBundleManager() const
    {
        return m_BundleManager;
    }

    class AudioListener* GetListener() const
    {
        return m_Listener;
    }

    /* 0x004 */ nlDLListSlotPool<Plat3dSoundSrc> m_SoundInstancePool;
    /* 0x020 */ nlDLListSlotPool<XSoundOwner*> m_SoundOwnerPool;
    /* 0x03C */ class AudioListener* m_Listener;
    /* 0x040 */ nlDLListContainer<XSoundHandle*> m_ActiveSoundList;
    /* 0x048 */ bool m_Unknown48;
    /* 0x049 */ bool m_AsyncLoading;
    /* 0x04A */ char m_ResourcePath[0x80];
    /* 0x0CA */ u8 m_PadCA[2];
    /* 0x0CC */ union
    {
        AudioRuntime* m_unkCC;
        AudioBundleManager* m_BundleManager;
    };
    /* 0x0D0 */ StaticCircularQueue<XSoundHandle*, 128> m_UnknownD0;
    /* 0x2E0 */ unsigned int m_OwnedSoundCount;
};

XSoundCueHandle* CreateAudioSoundHandle(AudioSystem* audio, int slotId,
    XSoundOwner* owner, unsigned long cueId, int value1, int value2,
    int value3, int callback, int context);
void UpdateAudioSystem(AudioSystem* audio, float dt);
void PrintAudioSystem(AudioSystem* audio);
void DumpAudioSystem(AudioSystem* audio, const char* path);

void FlushAudio(AudioSystem* audio, int callbackEnabled, bool force);
Plat3dSoundSrc* CreateAudioSoundOwner(AudioSystem* audio);

static AudioBundleManager* GetAudioBundleManager()
{
    return g_pAudioSystem->GetBundleManager();
}

#endif // GAME_AUDIO_AUDIO_SYSTEM_H
