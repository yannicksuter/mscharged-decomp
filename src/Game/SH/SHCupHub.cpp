#include "Game/SH/SHNavigation.h"
#include "Game/SH/SHCupHub.h"
#include "Game/FE/FEAudio.h"

#include "Game/DB/GameProgress.h"
#include "Game/FE/fePresentation.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/feDPD.h"

class SHNavigation;


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
    if (g_pCupManager->mUnidentified8680 == 0x10
        || g_pCupManager->GetCurrentRoundType() == 0)
    {
        mUnidentified67D = false;
    }
}

CupHubScene::~CupHubScene()
{
}

void CupHubScene::fn_80203980(int index, void* context)
{
    if (context == 0 && !mRulesComponent.HasOtherPointerState(1, index))
    {
        mRulesButton->SetActiveSlide("over", true, false);
        FEAudio::PlayAnimAudioEvent(0xAA73EF34, 0, 0, 1);
        mRulesComponent.SetPointerState(1, index);
    }
}

void CupHubScene::fn_80203A10(int index, void* context)
{
    if (context == 0 && !mRulesComponent.HasOtherPointerState(1, index))
    {
        mRulesButton->SetActiveSlide("off", true, false);
        mRulesComponent.SetPointerState(0, index);
    }
}

void CupHubScene::fn_80203A88(int, void* context)
{
    mUnidentified67C = true;
    for (int i = 0; i < 4; ++i)
    {
        gFEPointerInstances[i]->SetActiveSlide("waiting", true, false);
    }

    if (context == 0)
    {
        FEAudio::PlayAnimAudioEvent(0x6E5C794C, 0, 0, 1);
        FEAudio::PlayAnimAudioEvent(0x2ECB0035, 0, 0, 1);
        mUnidentified894 = 2;

        SHNavigation* object = GetNavigationScene();
        if (object != 0)
        {
            object->HideButtons();
        }

        mPresentation->SetActiveSlide("GAME", true);
    }
}
