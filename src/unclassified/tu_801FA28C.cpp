#include "unclassified/tu_801FA28C.h"
#include "Game/SH/SHNavigation.h"
#include "Game/FE/FEAudio.h"

#include "Game/BaseGameSceneManager.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/main.h"
#include "NL/nlLocalization.h"
#include "Game/FE/feDPD.h"
#include "Game/FE/FEAudio.h"
#include "Game/SH/SHNavigation.h"

class SHNavigation;

extern BaseGameSceneManager* g_pOverlayManager;

TU801FA324Overlay::~TU801FA324Overlay()
{
    SHNavigation* scene = GetNavigationScene();
    if (scene != 0)
    {
        scene->ResetButtons(true);
        mUnidentified0DC->m_bVisible = false;
    }
}

TU801FA324Overlay::TU801FA324Overlay()
    : BaseOverlayHandler(1, POSITION_ALL)
    , mControllerComponent()
    , mUnidentified0E4(false)
    , mUnidentified0E5(false)
    , mUnidentified0E8("art/fe/controllermapui.res", 0)
{
    if ((GetRegion() == 0
            && g_pLocalization->m_CurrentLanguage == nlLocalization::LangEnglish)
        || g_pLocalization->m_CurrentLanguage == nlLocalization::LangJapanese)
    {
        mUnidentified0E6 = true;
    }
    else
    {
        mUnidentified0E6 = false;
    }
}

void TU801FA324Overlay::fn_801FAB00(int index, void*)
{
    if (!mControllerComponent.HasOtherPointerState(1, index))
    {
        mUnidentified0DC->SetActiveSlide("over", true, false);
        FEAudio::PlayAnimAudioEvent(0xAA73EF33, 0, 0, 1);
    }

    mControllerComponent.SetPointerState(1, index);
}

void TU801FA324Overlay::fn_801FAB88(int index, void*)
{
    if (!mControllerComponent.HasOtherPointerState(1, index))
    {
        mUnidentified0DC->SetActiveSlide("off", true, false);
    }

    mControllerComponent.SetPointerState(0, index);
}

void TU801FA324Overlay::fn_801FABF8(int index, void*)
{
    mUnidentified0DC->SetActiveSlide("down", true, false);
    mControllerComponent.SetPointerState(2, index);
    mUnidentified0E5 = true;
    FEAudio::PlayAnimAudioEvent(0x9F9BF00F, 0, 0, 1);

    for (int i = 0; i < 4; ++i)
    {
        gFEPointerInstances[i]->SetActiveSlide("waiting", true, false);
    }

    g_pOverlayManager->Push((SceneList)80, SCREEN_BACK, true);
}
