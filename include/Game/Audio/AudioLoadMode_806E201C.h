#ifndef GAME_AUDIO_AUDIO_LOAD_MODE_806E201C_H
#define GAME_AUDIO_AUDIO_LOAD_MODE_806E201C_H

#include "NL/nlMath.h"
#include "NL/nlSlotPool.h"
#include "types.h"

class AudioBundleManager_802EDA7C;
class XSoundHandle_802ED74C;
struct AudioHandleState_800EBF78;

struct AudioValues_801098E4
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

struct AudioValuesOwner_801098E4
{
    /* 0x00 */ u8 m_unk00[0x8];
    /* 0x08 */ AudioValues_801098E4* m_unk08;
};

struct AudioRuntime_801098E4
{
    /* 0x00 */ u8 m_unk00[0x10];
    /* 0x10 */ AudioValuesOwner_801098E4* m_unk10;
};

class AudioLoadMode_806E201C
{
public:
    AudioLoadMode_806E201C();
    ~AudioLoadMode_806E201C();

    virtual bool fn_806E201C_0();
    virtual bool fn_800ED85C();
    virtual bool fn_802EBF78();
    virtual void Shutdown();
    virtual void SetResourcePath(const char* path);

    void PauseTrackedSound(
        const unsigned long& key, XSoundHandle_802ED74C** handle);
    void ResumeTrackedSound(
        const unsigned long& key, AudioHandleState_800EBF78* state);

    AudioBundleManager_802EDA7C* GetBundleManager() const
    {
        return m_BundleManager;
    }

    class AudioListener_802EBD54* GetListener() const
    {
        return m_Listener;
    }

    /* 0x004 */ SlotPoolBase m_SoundInstancePool;
    /* 0x01C */ u32 m_Unknown1C;
    /* 0x020 */ SlotPoolBase m_SoundOwnerPool;
    /* 0x038 */ u32 m_Unknown38;
    /* 0x03C */ class AudioListener_802EBD54* m_Listener;
    /* 0x040 */ void* m_Unknown40;
    /* 0x044 */ void* m_ActiveSoundList;
    /* 0x048 */ bool m_Unknown48;
    /* 0x049 */ bool m_AsyncLoading;
    /* 0x04A */ char m_ResourcePath[0x80];
    /* 0x0CA */ u8 m_PadCA[2];
    /* 0x0CC */ union
    {
        AudioRuntime_801098E4* m_unkCC;
        AudioBundleManager_802EDA7C* m_BundleManager;
    };
    /* 0x0D0 */ void* m_UnknownD0;
    /* 0x0D4 */ void* m_UnknownD4;
    /* 0x0D8 */ void* m_UnknownD8;
    /* 0x0DC */ u32 m_UnknownDC;
    /* 0x0E0 */ u8 m_UnknownE0[0x200];
    /* 0x2E0 */ void* m_Unknown2E0;
};

class AudioListener_802EBD54
{
public:
    virtual ~AudioListener_802EBD54();
    virtual void fn_802ECD24();
    virtual void fn_802ECD14();
    virtual void SetTransformValid(bool valid);
    virtual void SetEnabled(bool enabled);
    virtual void fn_802ECD1C();

    /* 0x04 */ nlVector3 m_Position;
    /* 0x10 */ nlVector3 m_View;
    /* 0x1C */ nlVector3 m_Up;
    /* 0x28 */ bool m_HasTransform;
    /* 0x29 */ bool m_Enabled;
    /* 0x2A */ bool m_TransformValid;
    /* 0x2B */ u8 m_Unknown2B[0x25];
};

extern AudioLoadMode_806E201C* lbl_806E201C;

#endif // GAME_AUDIO_AUDIO_LOAD_MODE_806E201C_H
