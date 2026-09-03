#include "unclassified/tu_80214DF8.h"

#include "Game/BaseGameSceneManager.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/NetworkStatsManager.h"
#include "Game/Render/Presentation.h"

extern BaseGameSceneManager* lbl_806E1838;
extern TLComponentInstance* lbl_80578450[4];

extern "C" void fn_801CBCA0(unsigned long hash, int value0, int value1, int value2);
extern "C" Presentation* fn_801FEEAC();

TU80214DF8Scene::TU80214DF8Scene()
    : mUnidentified30(false)
    , mUnidentified34(-1)
    , mUnidentified38(0)
    , mUnidentified1064()
    , mUnidentified176C()
    , mUnidentified1920()
    , mUnidentified19FC(0)
    , mUnidentified1A44(false)
{
    mUnidentified20[0] = 0;
    mUnidentified20[1] = 0;
    mUnidentified20[2] = 0;
    mUnidentified20[3] = 0;

    for (int i = 0; i < 10; ++i)
    {
        mUnidentified1064[i].mContext = (void*)i;
        mUnidentified1064[i].mSpeakerEnabled = false;
    }

    mUnidentified1920.fn_801D2BE0(false);
}

TU80214DF8Scene::~TU80214DF8Scene()
{
}

void TU80214DF8Scene::fn_80216048()
{
    fn_801CBCA0(0x375C885A, 0, 0, 1);
    fn_802168A0();

    int categoryIndex = mUnidentified105C;
    NetworkLeaderboardCategory* category =
        NetworkStatsManager_8012F378::Instance()->GetCategory(categoryIndex);
    mUnidentified34 = category->mFirstRank;

    if ((mUnidentified105C == 0 || mUnidentified105C == 2) && mUnidentified34 >= 0)
    {
        mUnidentified38 = mUnidentified34 - 5;
        categoryIndex = mUnidentified105C;
        int maximum = NetworkStatsManager_8012F378::Instance()
                          ->GetCategory(categoryIndex)
                          ->mCount
            - 10;
        if (mUnidentified38 > maximum)
        {
            mUnidentified38 = maximum;
        }
        if (mUnidentified38 < 0)
        {
            mUnidentified38 = 0;
        }
    }
    else
    {
        mUnidentified38 = 0;
    }

    mUnidentified1A28->m_bVisible = false;
    for (int i = 0; i < 10; ++i)
    {
        if (fn_80216170(i, i + mUnidentified38))
        {
            mUnidentified1A00[i]->m_bVisible = true;
        }
        else
        {
            mUnidentified1A00[i]->m_bVisible = false;
        }
    }
}

void TU80214DF8Scene::fn_80216808(int index, void*)
{
    ++mUnidentified20[index];
}

void TU80214DF8Scene::fn_80216820(int index, void*)
{
    --mUnidentified20[index];
}

void TU80214DF8Scene::fn_80216838(int, void*)
{
    for (int i = 0; i < 4; ++i)
    {
        lbl_80578450[i]->SetActiveSlide("waiting", true, false);
    }
}

void TU80214DF8Scene::fn_80216A90(int, void*)
{
    mUnidentified1A44 = false;
    lbl_806E1838->Pop();
    fn_801CBCA0(0x4430B152, 0, 0, 1);
    fn_801FEEAC()->Call("TransitionOnlineMatchToMainMenu");
}
