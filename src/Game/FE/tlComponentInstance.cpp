#include "Game/FE/tlComponentInstance.h"

#include "Game/FE/tlSlide.h"

void TLComponentInstance::Update(float dt)
{
    TLSlide* slide = m_component->m_pActiveSlide;
    if (slide != 0)
    {
        slide->Update(dt);
    }
}

void TLComponentInstance::SetActiveSlide(const char* name, bool arg2, bool arg3)
{
    m_component->SetActiveSlide(name, arg2, arg3);
}

void TLComponentInstance::SetActiveSlide(unsigned long hash, bool arg2, bool arg3)
{
    m_component->SetActiveSlide(hash, arg2, arg3);
}

void TLComponentInstance::SetActiveSlide(TLSlide* slide, bool arg2, bool arg3)
{
    m_component->SetActiveSlide(slide, arg2, arg3);
}

TLSlide* TLComponentInstance::GetActiveSlide()
{
    return m_component->m_pActiveSlide;
}
