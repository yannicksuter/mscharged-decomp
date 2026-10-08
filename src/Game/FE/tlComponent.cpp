#include "Game/FE/tlComponent.h"

#include "Game/FE/feFinder.h"
#include "Game/FE/tlSlide.h"
#include "NL/nlString.h"

TLComponent::TLComponent()
{
    m_type = FEOT_COMPONENT;
}

void TLComponent::SetActiveSlide(const char* name, bool forceRestart, bool preserveTime)
{
    unsigned long hash = nlStringLowerHash(name);
    SetActiveSlide(hash, forceRestart, preserveTime);
}

void TLComponent::SetActiveSlide(unsigned long hash, bool forceRestart, bool preserveTime)
{
    TLSlide* slide = FindItemByHashID<TLSlide>(pChildren, hash);
    if (slide != 0)
    {
        if (forceRestart || slide != m_pActiveSlide)
        {
            if (!preserveTime)
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

void TLComponent::SetActiveSlide(TLSlide* slide, bool forceRestart, bool preserveTime)
{
    if (slide != 0)
    {
        if (forceRestart || slide != m_pActiveSlide)
        {
            if (!preserveTime)
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
