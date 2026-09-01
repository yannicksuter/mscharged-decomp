#ifndef GAME_AUDIO_XSOUND_HANDLE_802ED74C_H
#define GAME_AUDIO_XSOUND_HANDLE_802ED74C_H

#include "types.h"

struct XSoundOwner_802ED74C
{
    u8 m_Unknown00[0x1C];
    union
    {
        u32 m_ReferencesAndFlags;
        struct
        {
            s32 references : 16;
            u32 flags : 16;
        } count;
    };
};

class XSoundHandle_802ED74C;
typedef void (*XSoundHitMarkerCallback_802ED74C)(
    void*, XSoundHandle_802ED74C*, void*);

class XSoundHandle_802ED74C
{
public:
    XSoundHandle_802ED74C(void* value1, XSoundOwner_802ED74C* owner,
        void* value2, XSoundHitMarkerCallback_802ED74C callback,
        void* callbackContext);
    virtual ~XSoundHandle_802ED74C();

    virtual bool fn_802ED74C_3(bool) = 0;
    virtual bool fn_802ED74C_4(u8) = 0;
    virtual void fn_802ED74C_5(u8, void*) = 0;
    virtual void fn_802ED74C_6() = 0;
    virtual void fn_802ED74C_7() = 0;
    virtual void fn_800ED8C0(u8 enabled);
    virtual bool fn_802ECE24();
    virtual void fn_802ED74C_10() = 0;
    virtual void fn_802ECE04(float dt);
    virtual void fn_802ED74C_12() = 0;
    virtual void fn_802ED8BC(u32** slot, u32 cueIndex);

    void fn_802ED88C(char* buffer, u32 size);
    void fn_802ED8D0(void* value);
    void fn_802ED904();

    u32** m_Slot;
    u32 m_CueIndex;
    u32 m_State;
    u8 m_CallbackEnabled;
    u8 m_Unknown11[3];
    XSoundOwner_802ED74C* m_Owner;
    float m_PreviousTime;
    float m_CurrentTime;
    u32 m_Unknown20;
    XSoundHitMarkerCallback_802ED74C m_Callback;
    void* m_CallbackContext;
};

#endif // GAME_AUDIO_XSOUND_HANDLE_802ED74C_H
