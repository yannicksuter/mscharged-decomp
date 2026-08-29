#include "Game/FE/fePresentation.h"

#include "Game/FE/feFinder.h"
#include "Game/FE/tlSlide.h"
#include "NL/nlString.h"

void FEPresentation::SetActiveSlide(const char* slideName, bool resetTime)
{
    u32 hash = nlStringLowerHash(slideName);
    TLSlide* slide = FindItemByHashID<TLSlide>(m_slides, hash);
    if (resetTime || m_currentSlide != slide)
    {
        m_fadeDuration = 0.0f;
    }
    m_currentSlide = slide;
}

void FEPresentation::Update(float deltaTime)
{
    if (m_currentSlide != 0)
    {
        f32 end;
        m_fadeDuration += deltaTime;
        end = m_currentSlide->m_duration;
        end += m_currentSlide->m_start;
        if (m_fadeDuration > end)
        {
            switch (m_currentSlide->m_uPlayMode)
            {
            case TLPM_LOOPING:
                m_fadeDuration = m_fadeDuration - end;
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
