#include "unclassified/tu_801FADB4.h"

#include "Game/FE/fePresentation.h"
#include "Game/FE/tlSlide.h"
#include "Game/Sys/audio.h"

TU801FADB4Overlay::TU801FADB4Overlay()
    : BaseOverlayHandler(2, POSITION_ALL)
    , mUnidentified25(false)
    , mUnidentified26(false)
    , mUnidentified28(-1)
    , mUnidentified2C(0)
    , mUnidentified30(0.0f)
    , mUnidentified34(0.0f)
    , mUnidentified3C(0)
{
}

TU801FADB4Overlay::~TU801FADB4Overlay()
{
}

void TU801FADB4Overlay::Update(float fDeltaT)
{
    BaseSceneHandler::Update(fDeltaT);

    if (mUnidentified25)
    {
        TLSlide* slide = mPresentation->GetActiveSlide();
        if (slide->GetCurrentTime() >= slide->m_start + slide->m_duration)
        {
            mUnidentified26 = true;
        }

        mUnidentified30 += fDeltaT;
        if (mUnidentified30 >= mUnidentified34 && mUnidentified2C < 4)
        {
            fn_800EBBFC(15, 0x97E84AE4, 0, 0);
            ++mUnidentified2C;
            mUnidentified30 = 0.0f;
        }
    }
}

void TU801FADB4Overlay::fn_801FB12C()
{
    switch (mUnidentified3C)
    {
    case 2:
        mPresentation->SetActiveSlide("COUNTDOWN3", true);
        mUnidentified34 = 0.3f;
        break;
    case 1:
        mPresentation->SetActiveSlide("COUNTDOWN2", true);
        mUnidentified34 = 0.5f;
        break;
    default:
        mPresentation->SetActiveSlide("COUNTDOWN", true);
        mUnidentified34 = 0.6f;
        break;
    }

    mUnidentified25 = true;
    mUnidentified30 = 0.0f;
    fn_800EBBFC(15, 0x97E84AE4, 0, 0);
    mUnidentified2C = 1;
}
