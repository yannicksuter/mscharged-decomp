#ifndef GAME_AUDIO_LOWPASSFILTER_INL
#define GAME_AUDIO_LOWPASSFILTER_INL

#include "Game/Audio/LowPassFilter.h"

inline LowPassFilter::LowPassFilter()
    : AudioEffectBase("LowPassFilter")
{
    m_CurrentParameter = &m_Initial;
    m_ResultParameter = &m_Final;
    m_Initial.m_On = 0;
    m_Initial.m_Frequency = 16000;
}

#endif // GAME_AUDIO_LOWPASSFILTER_INL
