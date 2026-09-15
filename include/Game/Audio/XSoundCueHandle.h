#ifndef GAME_AUDIO_XSOUNDCUEHANDLE_H
#define GAME_AUDIO_XSOUNDCUEHANDLE_H

#include "NL/nlSlotPool.h"
#include "types.h"
#include "Game/Audio/XSoundHandle.h"

class PlaybackBackend_802F2C3C;

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

struct CueDefinition_802F1758
{
    const char* name;
    u8 pad_04[8];
    u8 useSlider;
    u8 pad_0D[7];
    u32 sliderIndex;
    u8 pad_18[4];
    u32 activeCount;
    u32 maximumCount;
    void* field_24;
};

struct CueDefinitionTable_802F1758
{
    u8 pad_00[0xC];
    CueDefinition_802F1758* definitions;
};

struct CueResource_802F1758
{
    u8 pad_00[0xC];
    CueDefinitionTable_802F1758* cues;
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
    inline void* SelectSound();
    LocalSliderSet_802F1758* GetLocalSliders() const
    {
        return m_LocalSliders;
    }
    static void* operator new(unsigned long size);
    static void operator delete(void* handle);
    CueDefinition_802F1758* definition;
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
