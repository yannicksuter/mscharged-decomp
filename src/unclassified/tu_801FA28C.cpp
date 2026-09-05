#include "unclassified/tu_801FA28C.h"

#include "Game/BaseGameSceneManager.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/main.h"
#include "NL/nlLocalization.h"

extern "C" void fn_801CBCA0(unsigned long hash, int value0, int value1, int value2);
class TU80252180Scene;
extern "C" TU80252180Scene* fn_80253E18();
extern "C" void fn_80254310(TU80252180Scene* scene, bool enabled);

extern TLComponentInstance* lbl_80578450[4];
extern BaseGameSceneManager* lbl_806E1860;

TU801FA324Overlay::~TU801FA324Overlay()
{
    TU80252180Scene* scene = fn_80253E18();
    if (scene != 0)
    {
        fn_80254310(scene, true);
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
    if (!mControllerComponent.fn_802192FC(1, index))
    {
        mUnidentified0DC->SetActiveSlide("over", true, false);
        fn_801CBCA0(0xAA73EF33, 0, 0, 1);
    }

    mControllerComponent.mValues[index] = 1;
}

void TU801FA324Overlay::fn_801FAB88(int index, void*)
{
    if (!mControllerComponent.fn_802192FC(1, index))
    {
        mUnidentified0DC->SetActiveSlide("off", true, false);
    }

    mControllerComponent.mValues[index] = 0;
}

void TU801FA324Overlay::fn_801FABF8(int index, void*)
{
    mUnidentified0DC->SetActiveSlide("down", true, false);
    mControllerComponent.mValues[index] = 2;
    mUnidentified0E5 = true;
    fn_801CBCA0(0x9F9BF00F, 0, 0, 1);

    for (int i = 0; i < 4; ++i)
    {
        lbl_80578450[i]->SetActiveSlide("waiting", true, false);
    }

    lbl_806E1860->Push((SceneList)80, SCREEN_BACK, true);
}
