#ifndef GAME_FE_PRESENTATION_H
#define GAME_FE_PRESENTATION_H

#include "types.h"

class TLSlide;

class FEPresentation
{
public:
    void Update(float deltaTime);
    TLSlide* GetActiveSlide() const
    {
        return m_currentSlide;
    }
    void SetActiveSlide(const char* slideName, bool resetTime);

    /* 0x00 */ TLSlide* m_slides;
    /* 0x04 */ TLSlide* m_currentSlide;
    /* 0x08 */ f32 m_fadeDuration;
}; // size 0x0C

#endif // GAME_FE_PRESENTATION_H
