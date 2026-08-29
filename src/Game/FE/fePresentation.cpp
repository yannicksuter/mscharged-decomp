#include "Game/FE/fePresentation.h"

#include "Game/FE/feFinder.h"
#include "Game/FE/tlSlide.h"
#include "NL/nlString.h"

void FEPresentation::SetActiveSlide(const char* slideName, bool arg2)
{
    u32 hash = nlStringLowerHash(slideName);
    TLSlide* slide = FindItemByHashID<TLSlide>(m_slides, hash);

    if (arg2 || m_currentSlide != slide)
    {
        m_fadeDuration = 0.0f;
    }
    m_currentSlide = slide;
}

void FEPresentation::Update(float deltaTime)
{
    if (m_currentSlide != 0)
    {
        float currentTime = m_fadeDuration;
        currentTime += deltaTime;
        m_fadeDuration = currentTime;
        float duration = m_currentSlide->m_duration;
        float start = m_currentSlide->m_start;
        float end = start + duration;
        if (currentTime > end)
        {
            switch (m_currentSlide->m_uPlayMode)
            {
            case TLPM_LOOPING:
                m_fadeDuration = currentTime - end;
                break;
            case TLPM_STOP_AT_END:
                m_fadeDuration = end;
                break;
            default:
                break;
            }
        }
        m_currentSlide->m_time = m_fadeDuration;
        m_currentSlide->Update(deltaTime);
    }
}
