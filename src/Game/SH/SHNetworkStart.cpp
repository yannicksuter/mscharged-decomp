#include "Game/SH/SHNetworkStart.h"

#include "Game/FE/feFinder.h"
#include "Game/FE/feMusic.h"
#include "Game/FE/tlComponentInstance.h"
#include "NL/nlString.h"

extern bool lbl_806E1888;
extern bool lbl_806E1889;

namespace FEMusic
{
extern unsigned long mCurrentMusicCue_806E1878;
}

extern "C" void fn_801CBCE8(unsigned long cueId, void* context);
extern "C" void fn_801CBCEC(unsigned long cueId, void* context);

void UnidentifiedNetworkStartScene::fn_801FDF00(TLComponentInstance* component)
{
    component->SetActiveSlide("off", true, false);
    component->Update(0.0f);
}

void UnidentifiedNetworkStartScene::fn_801FDEB8(TLComponentInstance* component)
{
    component->SetActiveSlide("on", true, false);
    component->Update(0.0f);
}

void UnidentifiedNetworkStartScene::fn_801FC680(int state)
{
    TLSlide* activeSlide = mPresentation->m_currentSlide;
    unsigned long buttonsHash = nlStringLowerHash("BUTTONS");
    unsigned long layerHash = nlStringLowerHash("Layer");
    TLComponentInstance* buttons = (TLComponentInstance*)FEFinder<TLComponentInstance, 2>::_Find(
        activeSlide,
        layerHash,
        buttonsHash,
        0,
        0,
        0,
        0);

    bool visible = true;
    if (state == 0)
    {
        buttons->SetActiveSlide("A AND B", true, false);
    }
    else if (state == 1)
    {
        buttons->SetActiveSlide("A", true, false);
    }
    else if (state == 2)
    {
        buttons->SetActiveSlide("B", true, false);
    }
    else if (state == -1)
    {
        visible = false;
    }
    buttons->m_bVisible = visible;
}

extern "C" void fn_801FC4DC()
{
    lbl_806E1888 = false;
}

extern "C" void fn_801FC4C8()
{
    lbl_806E1888 = false;
    lbl_806E1889 = true;
}

extern "C" bool fn_801FC464()
{
    switch (FEMusic::mCurrentMusicCue_806E1878)
    {
    case (int)0xAE597F5E:
    case 0x2341D569:
    case 0x2447F290:
    case 0x244A44AE:
        return true;
    default:
        return false;
    }
}

extern "C" void fn_801FC454()
{
    fn_801CBCEC(FEMusic::mCurrentMusicCue_806E1878, (void*)FEMusic::StartStreamIfDifferent);
}

extern "C" void fn_801FC444()
{
    fn_801CBCE8(FEMusic::mCurrentMusicCue_806E1878, (void*)FEMusic::StartStreamIfDifferent);
}
