#include "Game/FE/feSlideMenu.h"

#include "NL/nlString.h"

FESlideMenu::FESlideMenu(TLComponentInstance* pWorkPres)
{
    m_size = 0;
    m_currentSlide = 0;
    m_doWrapAround = 0;
    m_pMenuComp = pWorkPres;
    m_lockInput = 0;
    m_callbackParam = 0;
    mLastChosenSlide = -1;
    mLastRandomSlide = -1;
    mNumCyclesRemaining = 0;
    mRandDeltaTime = 0.0f;
}

inline FESlideMenu::MenuItem::MenuItem()
{
}

FESlideMenu::~FESlideMenu()
{
}

FESlideMenu::MenuItem* FESlideMenu::AddMenuItem(const char* name)
{
    MenuItem* item = &m_menuItems[m_size];
    m_menuItems[m_size].ItemSlide = nlStringLowerHash(name);
    m_size++;
    return item;
}

FESlideMenu::MenuItem* FESlideMenu::AddMenuItem(const char* name, const Function<FnVoidVoid>& func)
{
    MenuItem* item = &m_menuItems[m_size];
    m_menuItems[m_size].ItemSlide = nlStringLowerHash(name);
    m_menuItems[m_size].ItemCBFuncs[0] = func;
    m_size++;
    return item;
}

bool FESlideMenu::ApplyFunction()
{
    if (m_lockInput)
    {
        return false;
    }

    MenuItem* item = &m_menuItems[m_currentSlide];
    if (item->ItemCBFuncs[ON_APPLY])
    {
        runCallBack();
        return true;
    }
    return false;
}

void FESlideMenu::SetSlideByIndex(unsigned char index)
{
    if (m_currentSlide == index || index >= m_size)
        return;

    m_currentSlide = index;
    OnHighlight();
    m_pMenuComp->SetActiveSlide(m_menuItems[m_currentSlide].ItemSlide, true, false);
}

bool FESlideMenu::NextItem()
{
    if (m_lockInput)
    {
        return false;
    }

    bool didChange = true;

    if (m_currentSlide == m_size - 1)
    {
        if (m_doWrapAround)
        {
            m_currentSlide = 0;
        }
        else
        {
            didChange = false;
        }
    }
    else
    {
        m_currentSlide++;
    }

    if (didChange)
    {
        m_pMenuComp->SetActiveSlide(m_menuItems[m_currentSlide].ItemSlide, true, false);
        OnHighlight();
        return true;
    }

    return false;
}

bool FESlideMenu::PrevItem()
{
    if (m_lockInput)
    {
        return false;
    }

    u8 idx = m_currentSlide;
    bool changed = true;

    if (idx == 0)
    {
        if (m_doWrapAround)
        {
            m_currentSlide = m_size - 1;
        }
        else
        {
            changed = false;
        }
    }
    else
    {
        m_currentSlide = idx - 1;
    }

    if (changed)
    {
        m_pMenuComp->SetActiveSlide(m_menuItems[m_currentSlide].ItemSlide, true, false);
        OnHighlight();
        return true;
    }

    return false;
}

void FESlideMenu::UpdatePresentation()
{
    m_pMenuComp->SetActiveSlide(m_menuItems[m_currentSlide].ItemSlide, true, false);
}

inline FESlideMenu::MenuItem::~MenuItem()
{
}
