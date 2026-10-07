#ifndef GAME_AUDIO_CATEGORYVOLUME_H
#define GAME_AUDIO_CATEGORYVOLUME_H

#include "Game/Audio/AudioEffect.h"
#include "NL/nlSlotPool.h"

class CategoryVolumeParameter
    : public AudioEffectParameter
{
public:
    CategoryVolumeParameter();
    virtual ~CategoryVolumeParameter();

    static void* operator new(unsigned long)
    {
        CategoryVolumeParameter* parameter = 0;
        s_Pool.Allocate(parameter);
        return parameter;
    }

    static void operator delete(void* pointer)
    {
        s_Pool.Free((CategoryVolumeParameter*)pointer);
    }

    float m_Volume;
    u32 m_Category;

    static SlotPool<CategoryVolumeParameter> s_Pool;
};

class CategoryVolume : public AudioEffectBase
{
public:
    CategoryVolume()
        : AudioEffectBase("CategoryVolume")
    {
        m_CurrentParameter = &m_Initial;
        m_ResultParameter = &m_Final;
    }

    virtual void CreateParameter(u32 definition, const void* context, bool negate,
        AudioEffectParameter** output);
    virtual void BeginBlend();
    virtual void BlendParameter(AudioEffectParameter* destination,
        AudioEffectParameter* source);

    static void* operator new(unsigned long)
    {
        CategoryVolume* effect = 0;
        s_Pool.Allocate(effect);
        return effect;
    }

    static void operator delete(void* pointer)
    {
        s_Pool.Free((CategoryVolume*)pointer);
    }

    CategoryVolumeParameter m_Initial;
    CategoryVolumeParameter m_Final;

    static SlotPool<CategoryVolume> s_Pool;
};

#endif // GAME_AUDIO_CATEGORYVOLUME_H
