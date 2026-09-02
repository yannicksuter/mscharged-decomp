#include "Game/FE/fePopupMenu.h"

#include "Game/FE/FEAudio.h"
#include "Game/FE/feInput.h"
#include "NL/nlMemory.h"

extern unsigned char* lbl_806E18D8;
extern "C" bool fn_80253E18();
extern "C" void fn_802533F0();

char* optionNames[3] = { "button_1", "button_2", "button_3" };

FEPopupMenu::FEPopupMenu()
    : mMenuDisplayed(false)
    , mUnidentified99D(false)
    , mMenuCreated(false)
    , mRunCallBack(false)
    , mUnknownA1F(false)
    , mUnidentified9A1(false)
    , mUnidentified9A2(false)
    , mUnidentified9A3(false)
    , mHighlightedOption(0)
    , mUnidentified9A8(0)
    , mAcceptDelayTime(0.0f)
    , mUnidentifiedC08(0)
    , mUnidentifiedC0C(8)
    , mUnknownA64()
    , mType(INVALID_TYPE)
    , mUnknownAA4(true)
    , mUnknownAA5(false)
    , mUnidentifiedC4C(false)
    , mUnidentifiedC50(0.0f)
    , mUnidentifiedC54(0.0f)
    , mUnidentifiedC58(false)
{
    mPopup.numOptions = 0;
    mPopup.pMessage = 0;
    mPopup.pOptionLabels[0] = 0;
    mPopup.pOptionLabels[1] = 0;
    mPopup.pOptionLabels[2] = 0;

    mControllerComponents[0].mContext = 0;
    mControllerComponents[0].mIgnoreInputLock = true;
    mOptionInstances[0] = 0;
    mUnidentified9D0[0] = 0;

    mControllerComponents[1].mContext = (void*)1;
    mControllerComponents[1].mIgnoreInputLock = true;
    mOptionInstances[1] = 0;
    mUnidentified9D0[1] = 0;

    mControllerComponents[2].mContext = (void*)2;
    mControllerComponents[2].mIgnoreInputLock = true;
    mOptionInstances[2] = 0;
    mUnidentified9D0[2] = 0;

    for (int i = 0; i < 4; ++i)
    {
        mUnidentifiedBF8[i] = 0;
    }

    g_pFEInput->PushExclusiveInputLock(this, 10);
    FEAudio::EnableSounds(false);
}

FEPopupMenu::~FEPopupMenu()
{
    for (int optionIndex = 0; optionIndex < mPopup.numOptions; ++optionIndex)
    {
        delete mPopup.pOptionLabels[optionIndex];
    }

    if (mPopup.pMessage != 0)
    {
        delete mPopup.pMessage;
    }

    g_pFEInput->PopExclusiveInputLock(this);
    FEAudio::EnableSounds(true);

    if (!mUnidentified9A3)
    {
        lbl_806E18D8[0x92] = false;
    }

    if (fn_80253E18())
    {
        fn_802533F0();
    }

    if (mRunCallBack == true)
    {
        Function<FnVoidVoid>& callback = callBacks[mHighlightedOption];
        callback();
    }
    else if (mUnknownA1F != false)
    {
        mUnknownA64();
    }
}
