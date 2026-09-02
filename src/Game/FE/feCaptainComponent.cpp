#include "Game/FE/feCaptainComponent.h"

#include "Game/FE/tlComponentInstance.h"

TU801DA134Component::TU801DA134Component()
    : mComponent(0)
    , mUnidentified08(0)
    , mUnidentified14(-1)
{
}

TU801DA134Component::~TU801DA134Component()
{
}

void TU801DA134Component::fn_801DA88C()
{
    if (mComponent != 0)
    {
        mComponent->SetActiveSlide("in", false, false);
        mComponent->m_bVisible = true;
    }

    if (mUnidentified08 != 0)
    {
        mUnidentified08->m_bVisible = true;
    }

    mUnidentified24 = 1;
    fn_801DC824(1, 1, 1);
    mUnidentified08->SetActiveSlide("Slide1", true, false);
}

int TU801DA134Component::fn_801DCD74(int index)
{
    return mSidekicks[index];
}

void TU801DA134Component::fn_801DCD84(int value)
{
    mUnidentified14 = value;
}

void TU801DA134Component::fn_801DCD8C(int index, int value)
{
    mSidekicks[index] = value;
}

void UnidentifiedTextFader::fn_801E423C(int value)
{
    mUnidentified1C = value;
}

void UnidentifiedTextFader::fn_801E4244(int value)
{
    mUnidentified14 = value;
}

void UnidentifiedTextFader::fn_801E424C(int value)
{
    mUnidentified20 = value;
}
