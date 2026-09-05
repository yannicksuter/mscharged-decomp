#include "Game/SH/SHCupHub.h"

#include "Game/DB/tu_8010A40C.h"
#include "Game/FE/fePresentation.h"
#include "Game/FE/tlComponentInstance.h"

extern "C" void fn_801CBCA0(unsigned long hash, int value0, int value1, int value2);
class TU80252180Scene;
extern "C" void fn_80253474(TU80252180Scene* scene);
extern "C" TU80252180Scene* fn_80253E18();

extern TLComponentInstance* lbl_80578450[4];

CupHubScene::CupHubScene()
    : mUnidentified300(false)
    , mUnidentified304(0)
    , mUnidentified67C(false)
    , mUnidentified67D(true)
    , mUnidentified67E(false)
    , mUnidentified67F(false)
    , mNavigationComponent()
    , mRulesButton(0)
    , mUnidentified890(false)
    , mUnidentified894(0)
{
    mRulesComponent.mContext = 0;
    mUnidentified680[0] = 0;
    mUnidentified680[1] = 0;
    mUnidentified680[2] = 0;
    mUnidentified680[3] = 0;

    for (int i = 0; i < 9; ++i)
    {
        for (int j = 0; j < 12; ++j)
        {
            mMatchupStates[i][j] = -1;
        }
    }

    for (int i = 0; i < 4; ++i)
    {
        mMatchupComponents[i].mSpeakerEnabled = false;
    }

    fn_80203B54();
    if (lbl_806E0F90->mUnidentified8680 == 0x10
        || lbl_806E0F90->GetCurrentRoundType() == 0)
    {
        mUnidentified67D = false;
    }
}

CupHubScene::~CupHubScene()
{
}

void CupHubScene::fn_80203980(int index, void* context)
{
    if (context == 0 && !mRulesComponent.fn_802192FC(1, index))
    {
        mRulesButton->SetActiveSlide("over", true, false);
        fn_801CBCA0(0xAA73EF34, 0, 0, 1);
        mRulesComponent.mValues[index] = 1;
    }
}

void CupHubScene::fn_80203A10(int index, void* context)
{
    if (context == 0 && !mRulesComponent.fn_802192FC(1, index))
    {
        mRulesButton->SetActiveSlide("off", true, false);
        mRulesComponent.mValues[index] = 0;
    }
}

void CupHubScene::fn_80203A88(int, void* context)
{
    mUnidentified67C = true;
    for (int i = 0; i < 4; ++i)
    {
        lbl_80578450[i]->SetActiveSlide("waiting", true, false);
    }

    if (context == 0)
    {
        fn_801CBCA0(0x6E5C794C, 0, 0, 1);
        fn_801CBCA0(0x2ECB0035, 0, 0, 1);
        mUnidentified894 = 2;

        TU80252180Scene* object = fn_80253E18();
        if (object != 0)
        {
            fn_80253474(object);
        }

        mPresentation->SetActiveSlide("GAME", true);
    }
}
