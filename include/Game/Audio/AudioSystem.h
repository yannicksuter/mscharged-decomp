#ifndef GAME_AUDIO_AUDIO_SYSTEM_H
#define GAME_AUDIO_AUDIO_SYSTEM_H

#include "Game/Audio/AudioGlobals.h"
#include "NL/nlMath.h"
#include "NL/nlDLListContainer.h"
#include "NL/CircularQueue.h"
#include "types.h"

class AudioBundleManager;
class XSoundHandle;
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

    virtual bool IsAsyncLoading();
    virtual bool IsInitialized();
    virtual bool IsIdle();
    virtual void Shutdown();
    virtual void SetResourcePath(const char* path);

    bool fn_802ECDC8(float dt, Plat3dSoundSrc& source);

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
    /* 0x2E0 */ unsigned int m_Unknown2E0;
};

class AudioListener
{
public:
    AudioListener()
        : m_HasTransform(true), m_Enabled(true), m_TransformValid(true)
    {
        nlVec3Set(m_Position, 0.0f, 0.0f, 0.0f);
        nlVec3Set(m_View, 0.0f, 1.0f, 0.0f);
        nlVec3Set(m_Up, 0.0f, 0.0f, 1.0f);
    }

    virtual void SetHasTransform(bool);
    virtual bool HasTransform();
    virtual bool IsTransformValid();
    virtual void SetTransformValid(bool valid);
    virtual void SetEnabled(bool enabled);
    virtual bool IsEnabled();
    virtual void Update(float) = 0;

    /* 0x04 */ nlVector3 m_Position;
    /* 0x10 */ nlVector3 m_View;
    /* 0x1C */ nlVector3 m_Up;
    /* 0x28 */ bool m_HasTransform;
    /* 0x29 */ bool m_Enabled;
    /* 0x2A */ bool m_TransformValid;
    /* 0x2B */ u8 m_Unknown2B;
};

void DumpAudioSystem(AudioSystem* audio, const char* path);

void FlushAudio(AudioSystem* audio, int param2, bool param3);

static AudioBundleManager* GetAudioBundleManager()
{
    return g_pAudioSystem->GetBundleManager();
}

#endif // GAME_AUDIO_AUDIO_SYSTEM_H
