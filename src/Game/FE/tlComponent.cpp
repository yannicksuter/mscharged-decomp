#include "Game/FE/tlComponent.h"

#include "Game/FE/tlSlide.h"
#include "NL/nlString.h"

static TLSlide* FindSlideByHash_80301D68(TLSlide* slides, unsigned long hash)
{
    if (slides == 0)
    {
        return 0;
    }

    TLSlide* slide = slides->m_next;
    for (;;)
    {
        unsigned long id = slide->m_hash;
        TLSlide* next = slide->m_next;
        if (hash == id)
        {
            return slide;
        }
        if (slide == slides)
        {
            break;
        }
        slide = next;
    }
    return 0;
}

static inline void SetActiveSlide_80301D68(
    TLComponent* component, TLSlide* slide, bool arg2, bool arg3)
{
    if (slide != 0)
    {
        if (arg2 || slide != component->m_pActiveSlide)
        {
            if (!arg3)
            {
                slide->m_time = 0.0f;
            }
        }
    }

    component->m_pActiveSlide = slide;
    if (slide != 0)
    {
        slide->Update(0.0f);
    }
}

TLComponent::TLComponent()
{
    m_type = FEOT_COMPONENT;
}

void TLComponent::SetActiveSlide(const char* name, bool arg2, bool arg3)
{
    unsigned long hash = nlStringLowerHash(name);
    TLSlide* slide = FindSlideByHash_80301D68(pChildren, hash);
    SetActiveSlide_80301D68(this, slide, arg2, arg3);
}

void TLComponent::SetActiveSlide(unsigned long hash, bool arg2, bool arg3)
{
    SetActiveSlide_80301D68(this, FindSlideByHash_80301D68(pChildren, hash), arg2, arg3);
}

void TLComponent::SetActiveSlide(TLSlide* slide, bool arg2, bool arg3)
{
    SetActiveSlide_80301D68(this, slide, arg2, arg3);
}
