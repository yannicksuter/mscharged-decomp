#ifndef GAME_AUDIO_XSOUND_HANDLE_H
#define GAME_AUDIO_XSOUND_HANDLE_H

#include "types.h"

struct XSoundOwner
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

class XSoundHandle;
typedef void (*XSoundHitMarkerCallback)(
    void*, XSoundHandle*, void*);

class XSoundHandle
{
public:
    XSoundHandle(void* value1, XSoundOwner* owner,
        void* value2, XSoundHitMarkerCallback callback,
        void* callbackContext);
    virtual ~XSoundHandle();

    virtual bool Play(bool) = 0;
    virtual bool Prepare(u8) = 0;
    virtual void Stop(u8, void*) = 0;
    virtual void Pause() = 0;
    virtual void Resume() = 0;
    virtual void SetCallbackEnabled(u8 enabled);
    virtual u8 IsCallbackEnabled();
    virtual void Release() = 0;
    virtual void Update(float dt);
    virtual bool IsValid() = 0;
    virtual void SetCue(u32** slot, u32 cueIndex);

    void FormatState(char* buffer, u32 size);
    void OnHitMarker(void* value);
    void PrintState();

    u32** m_Slot;
    u32 m_CueIndex;
    s32 m_State;
    u8 m_CallbackEnabled;
    u8 m_Unknown11[3];
    XSoundOwner* m_Owner;
    float m_PreviousTime;
    float m_CurrentTime;
    u32 m_Unknown20;
    XSoundHitMarkerCallback m_Callback;
    void* m_CallbackContext;
};

struct AudioParameter
{
    u8 m_Unknown00[0xC];
    float m_Value;
    float m_Time;
    u8 m_Unknown14[0x4];
    float m_Min;
    float m_Max;
};

AudioParameter* GetSoundParameter(XSoundHandle* handle, unsigned long index);

#endif // GAME_AUDIO_XSOUND_HANDLE_H
