#include "Game/FE/tlComponent.h"

#include "Game/FE/feFinder.h"
#include "Game/FE/tlSlide.h"
#include "NL/nlString.h"

TLComponent::TLComponent()
{
    m_type = FEOT_COMPONENT;
}

void TLComponent::SetActiveSlide(const char* name, bool arg2, bool arg3)
{
    unsigned long hash = nlStringLowerHash(name);
    SetActiveSlide(hash, arg2, arg3);
}

void TLComponent::SetActiveSlide(unsigned long hash, bool arg2, bool arg3)
{
    TLSlide* slide = FindItemByHashID<TLSlide>(pChildren, hash);
    if (slide != 0)
    {
        if (arg2 || slide != m_pActiveSlide)
        {
            if (!arg3)
            {
                slide->m_time = 0.0f;
            }
        }
    }

    m_pActiveSlide = slide;
    if (slide != 0)
    {
        slide->Update(0.0f);
    }
}

void TLComponent::SetActiveSlide(TLSlide* slide, bool arg2, bool arg3)
{
    if (slide != 0)
    {
        if (arg2 || slide != m_pActiveSlide)
        {
            if (!arg3)
            {
                slide->m_time = 0.0f;
            }
        }
    }

    m_pActiveSlide = slide;
    if (slide != 0)
    {
        slide->Update(0.0f);
    }
}
