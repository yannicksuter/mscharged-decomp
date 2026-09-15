#ifndef GAME_AUDIO_XSOUNDCUEHANDLE_H
#define GAME_AUDIO_XSOUNDCUEHANDLE_H

#include "Game/Audio/AudioResourceBundle.h"
#include "Game/Audio/XSoundHandle.h"
#include "NL/nlSlotPool.h"
#include "types.h"

class AudioSource;

struct SliderState_802F1758
{
    void** vtable;
    float value;
    u8 valid;
    u8 pad_09[0x1F];
};

struct LocalSliderSet_802F1758
{
    u32 field_00;
    SliderState_802F1758* sliders;
    void* owner;
};

struct SoundInstance_802F1758;

class XSoundCueHandle : public XSoundHandle
{
public:
    XSoundCueHandle(void* resource, XSoundOwner* owner, unsigned int cueIndex,
        XSoundHitMarkerCallback callback, void* context);
    virtual ~XSoundCueHandle();
    virtual bool Play(bool callbackEnabled);
    virtual bool Prepare(bool callbackEnabled);
    virtual void Stop(u8 callbackEnabled, void* value);
    virtual void Pause();
    virtual void Resume();
    virtual void Release();
    virtual void Update(float dt);
    virtual bool IsValid() { return true; }
    void UpdateSlider(float dt);
    inline AudioVoiceDefinition* SelectSound();
    LocalSliderSet_802F1758* GetLocalSliders() const
    {
        return m_LocalSliders;
    }
    static void* operator new(unsigned long size);
    static void operator delete(void* handle);
    AudioCueDefinition* definition;
    SoundInstance_802F1758* instance;
    SliderState_802F1758* slider;
    float sliderValue;
    struct
    {
        u32 savedState : 16;
        u32 flag8000 : 1;
        u32 playWhenPrepared : 1;
        u32 reservedFlags : 14;
    } bits;
};
extern SlotPool<XSoundCueHandle> sSoundCueHandlePool;
inline void* XSoundCueHandle::operator new(unsigned long size)
{
    return sSoundCueHandlePool.Allocate();
}
inline void XSoundCueHandle::operator delete(void* handle)
{
    sSoundCueHandlePool.Free((XSoundCueHandle*)handle);
}
#endif
