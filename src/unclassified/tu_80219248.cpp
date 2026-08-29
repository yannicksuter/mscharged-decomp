#include "unclassified/tu_80219248.h"

#include "Game/FE/feInput.h"
#include "NL/nlString.h"

extern "C" int fn_8013A12C(cGlobalPad* pad);
extern "C" void fn_80139D68(int action, cGlobalPad* pad);
extern "C" void fn_802F49AC(unsigned long* hash, int index);

TU80219248Component::TU80219248Component()
    : TU80300104Component(0)
{
    mSpeakerEnabled = true;
    mValues[0] = 0;
    mValues[1] = 0;
    mValues[2] = 0;
    mValues[3] = 0;
}

TU80219248Component::TU80219248Component(void* context)
    : TU80300104Component(context)
{
    mSpeakerEnabled = true;
    mValues[0] = 0;
    mValues[1] = 0;
    mValues[2] = 0;
    mValues[3] = 0;
}

bool TU80219248Component::fn_802192FC(int value, unsigned int which) const
{
    for (unsigned int i = 0; i < 4; i++)
    {
        if (i != which && value == mValues[i])
        {
            return true;
        }
    }
    return false;
}

void TU80219248Component::fn_80301BE8(int index, void* context)
{
    if (mCallback0)
    {
        mCallback0(index, context);
    }
}

void TU80219248Component::fn_80301BA8(int index, void* context)
{
    if (mCallback1)
    {
        mCallback1(index, context);
    }

    if (mSpeakerEnabled)
    {
        if (!fn_8013A12C(g_pFEInput->GetGlobalPad((eFEINPUT_PAD)index)))
        {
            fn_80139D68(5, g_pFEInput->GetGlobalPad((eFEINPUT_PAD)index));
        }
    }
}

void TU80219248Component::fn_80301CE8(int index, void* context)
{
    if (mCallback2)
    {
        mCallback2(index, context);
    }
}

void TU80219248Component::fn_80301C28(int index, void* context)
{
    if (mCallback3)
    {
        mCallback3(index, context);
    }
}

void TU80219248Component::fn_80301C68(int index, void* context)
{
    if (mCallback4)
    {
        mCallback4(index, context);
    }
}

void TU80219248Component::fn_80301D28(int index, void* context)
{
    if (mCallback5)
    {
        mCallback5(index, context);
    }
}

void TU80219248Component::fn_80301CA8(int index, void* context)
{
    if (mCallback6)
    {
        mCallback6(index, context);
    }
}

void TU80219248Component::fn_802195B4(int index)
{
    if (!fn_8013A12C(g_pFEInput->GetGlobalPad((eFEINPUT_PAD)index)))
    {
        fn_80139D68(5, g_pFEInput->GetGlobalPad((eFEINPUT_PAD)index));
    }
}

void TU80219248Component::fn_80219608(const TU80300104Event* event)
{
    unsigned long hash = nlStringLowerHash("ControllerSpeaker");
    fn_802F49AC(&hash, event->mIndex + 1);
    fn_80300594(event);
}
