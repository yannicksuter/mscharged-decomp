#include "Game/FE/tlComponentInstance.h"

#include "Game/FE/tlSlide.h"

void TLComponentInstance::Update(float dt)
{
    TLSlide* slide = static_cast<TLComponent*>(m_component)->m_pActiveSlide;
    if (slide != 0)
    {
        slide->Update(dt);
    }
}

void TLComponentInstance::SetActiveSlide(const char* name, bool forceRestart, bool preserveTime)
{
    static_cast<TLComponent*>(m_component)->SetActiveSlide(name, forceRestart, preserveTime);
}

void TLComponentInstance::SetActiveSlide(unsigned long hash, bool forceRestart, bool preserveTime)
{
    static_cast<TLComponent*>(m_component)->SetActiveSlide(hash, forceRestart, preserveTime);
}

void TLComponentInstance::SetActiveSlide(TLSlide* slide, bool forceRestart, bool preserveTime)
{
    static_cast<TLComponent*>(m_component)->SetActiveSlide(slide, forceRestart, preserveTime);
}

TLSlide* TLComponentInstance::GetActiveSlide()
{
    return static_cast<TLComponent*>(m_component)->m_pActiveSlide;
}
