#include "unclassified/tu_801F6A24.h"

#include "Game/FE/tlComponentInstance.h"
#include "NL/nlMath.h"

static const char* lbl_8051A700[10] = {
    "01",
    "02",
    "03",
    "04",
    "05",
    "06",
    "07",
    "08",
    "09",
    "10",
};

TU801F6A24Overlay::TU801F6A24Overlay()
    : BaseOverlayHandler(3, POSITION_ALL)
    , mUnidentified30(0)
    , mUnidentified34(false)
    , mUnidentified35(true)
{
}

TU801F6A24Overlay::~TU801F6A24Overlay()
{
}

void TU801F6A24Overlay::fn_801F6D94(float fParam1)
{
    mUnidentified28->SetActiveSlide(lbl_8051A700[(int)(fParam1 - 1.0f)], true, false);
    mUnidentified28->m_bVisible = true;
    mUnidentified2C->SetActiveSlide("green", true, false);
    mUnidentified30 = 0;
}

void TU801F6A24Overlay::fn_801F6E18(float)
{
    mUnidentified2C->SetActiveSlide("green", true, false);
}

void TU801F6A24Overlay::fn_801F6E2C(cFielder* pParam1)
{
    mUnidentified30 = pParam1;
    mUnidentified28->m_bVisible = false;
    mUnidentified2C->SetActiveSlide("slide1", true, false);
    mUnidentified36 = false;
    mUnidentified35 = true;
}

void TU801F6A24Overlay::fn_801F6E8C(const nlVector3& vParam1)
{
    if (mUnidentified35 == true)
    {
        mUnidentified28->SetAssetPosition(vParam1.x - 80.0f, 50.0f + vParam1.y, 0.0f);
        mUnidentified2C->SetAssetPosition(vParam1.x - 80.0f, 50.0f + vParam1.y, 0.0f);
    }
    else
    {
        mUnidentified28->SetAssetPosition(80.0f + vParam1.x, 50.0f + vParam1.y, 0.0f);
        mUnidentified2C->SetAssetPosition(80.0f + vParam1.x, 50.0f + vParam1.y, 0.0f);
    }
}
