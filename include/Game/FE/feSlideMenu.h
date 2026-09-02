#ifndef _FESLIDEMENU_H_
#define _FESLIDEMENU_H_

#include "Game/FE/tlComponentInstance.h"
#include "NL/nlFunction.h"

class FESlideMenu
{
public:
    struct MenuItem
    {
        MenuItem();
        ~MenuItem();

        /* 0x00 */ unsigned long ItemSlide;
        /* 0x04 */ Function<FnVoidVoid> ItemCBFuncs[2];
    }; // size 0x14

    void UpdatePresentation();
    bool PrevItem();
    bool NextItem();
    void SetSlideByIndex(unsigned char index);
    bool ApplyFunction();
    MenuItem* AddMenuItem(const char* name);
    MenuItem* AddMenuItem(const char* name, const Function<FnVoidVoid>& func);
    ~FESlideMenu();
    FESlideMenu(TLComponentInstance* pWorkPres);

    /* 0x000 */ MenuItem m_menuItems[16];
    /* 0x140 */ unsigned char m_size;
    /* 0x141 */ unsigned char m_currentSlide;
    /* 0x142 */ unsigned char m_doWrapAround;
    /* 0x144 */ TLComponentInstance* m_pMenuComp;
    /* 0x148 */ unsigned char m_lockInput;
    /* 0x14C */ void* m_callbackParam;
    /* 0x150 */ long mLastChosenSlide;
    /* 0x154 */ long mLastRandomSlide;
    /* 0x158 */ long mNumCyclesRemaining;
    /* 0x15C */ float mRandDeltaTime;
}; // size 0x160

#endif // _FESLIDEMENU_H_
