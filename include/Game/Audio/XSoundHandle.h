#ifndef GAME_AUDIO_XSOUND_HANDLE_H
#define GAME_AUDIO_XSOUND_HANDLE_H

#include "NL/nlMath.h"
#include "types.h"

struct XSoundOwner
{
    XSoundOwner()
    {
        count.references = 0;
        nlVec3Set(m_Unknown04.m_Value, 0.0f, 0.0f, 0.0f);
        count.field_8000 = count.field_4000 = count.field_2000 = count.field_1000 = false;
    }

    virtual ~XSoundOwner() { }

    /* 0x04 */ union
    {
        nlVector3 m_Value;
        const nlVector3* m_Pointer;
    } m_Unknown04;
    /* 0x10 */ float m_Unknown10;
    /* 0x14 */ float m_Unknown14;
    /* 0x18 */ float m_Unknown18;
    /* 0x1C */ union
    {
        u32 m_Unknown1C;
        u32 m_ReferencesAndFlags;
        struct
        {
            s32 references : 16;
            u32 field_8000 : 1;
            u32 field_4000 : 1;
            u32 field_2000 : 1;
            u32 field_1000 : 1;
            u32 field_0FFF : 12;
        } count;
    };
};

class XSoundHandle;
struct LocalSliderSet_802F1758;
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
    virtual bool Prepare(bool) = 0;
    virtual void Stop(u8, void*) = 0;
    virtual void Pause() = 0;
    virtual void Resume() = 0;
    virtual void SetCallbackEnabled(u8 enabled);
    virtual int IsCallbackEnabled();
    virtual void Release() = 0;
    virtual void Update(float dt);
    virtual bool IsValid() = 0;
    virtual void SetCue(unsigned int** slot, unsigned int cueIndex);

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
    LocalSliderSet_802F1758* m_LocalSliders;
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
