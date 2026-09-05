#include "Game/FE/feOptionsSubMenus.h"

#include "Game/DB/SaveLoad.h"
#include "Game/DB/UserOptions.h"
#include "Game/FE/fePresentation.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlInstance.h"
#include "Game/GameInfo.h"
#include "NL/nlColour.h"
#include "NL/nlConfig.h"

extern "C" void fn_801CBCA0(unsigned long hash, int value0, int value1, int value2);
class TU80252180Scene;
extern "C" TU80252180Scene* fn_80253E18();
extern "C" void fn_802534BC(TU80252180Scene* scene, int value, bool enabled);

static Config lbl_80578320(Config::ALLOCATE_HIGH, 0x2800, 0x400);

OptionsAudioMenuV2::OptionsAudioMenuV2(int value)
    : mUnidentified28(value)
    , mNavigation()
    , mUnidentified684(false)
    , mUnidentified685(false)
    , mUnidentified686(false)
    , mState(0)
{
    for (int i = 0; i < 6; ++i)
    {
        mButtonComponents[i].mContext = (void*)i;
        mButtonComponents[i].mSpeakerEnabled = false;
    }

    AudioSettings* settings = (AudioSettings*)GameInfoManager::Instance()->GetUnknown0xA0();
    mSettings[0] = settings->MusicVolume;
    mSettings[1] = settings->SFXVolume;
    mSettings[2] = settings->VoiceVolume;
    mBackupSettings[0] = mSettings[0];
    mBackupSettings[1] = mSettings[1];
    mBackupSettings[2] = mSettings[2];
    mNavigation.fn_801D2BE0(false);
}

OptionsAudioMenuV2::~OptionsAudioMenuV2()
{
}

void OptionsAudioMenuV2::fn_801D4E9C(int setting)
{
    int volume = 0;
    nlColour selected;
    nlColour unselected;
    nlColourSet(selected, 0xA9, 0xD0, 0x46, 0xFF);
    nlColourSet(unselected, 0, 0, 0, 0xFF);

    if (setting == 0)
    {
        volume = mSettings[0];
    }
    else if (setting == 1)
    {
        volume = mSettings[1];
    }
    else if (setting == 2)
    {
        volume = mSettings[2];
    }

    for (int i = 0; i < 10; ++i)
    {
        if (i < volume)
        {
            mVolumeBars[setting][i]->SetAssetColour(selected);
        }
        else
        {
            mVolumeBars[setting][i]->SetAssetColour(unselected);
        }
    }
}

void OptionsAudioMenuV2::fn_801D4F70(int index, void* context)
{
    unsigned int item = (unsigned int)context;
    if (!mButtonComponents[item].fn_802192FC(2, -1))
    {
        bool enabled;
        switch (item)
        {
        case 0:
            enabled = mSettings[0] > 0;
            break;
        case 1:
            enabled = mSettings[0] < 10;
            break;
        case 2:
            enabled = mSettings[1] > 0;
            break;
        case 3:
            enabled = mSettings[1] < 10;
            break;
        case 4:
            enabled = mSettings[2] > 0;
            break;
        case 5:
            enabled = mSettings[2] < 10;
            break;
        default:
            enabled = false;
            break;
        }

        if (!enabled)
        {
            return;
        }

        mButtonComponents[item].fn_802195B4(index);
        if (!mButtonComponents[item].fn_802192FC(1, index))
        {
            mButtons[item]->SetActiveSlide("over", true, false);
            fn_801CBCA0(0x96DEB5C3, 0, 0, 1);
        }
        mButtonComponents[item].mValues[index] = 1;
    }
}

void OptionsAudioMenuV2::fn_801D5108(int index, void* context)
{
    unsigned int item = (unsigned int)context;
    if (!mButtonComponents[item].fn_802192FC(2, -1))
    {
        bool enabled;
        switch (item)
        {
        case 0:
            enabled = mSettings[0] > 0;
            break;
        case 1:
            enabled = mSettings[0] < 10;
            break;
        case 2:
            enabled = mSettings[1] > 0;
            break;
        case 3:
            enabled = mSettings[1] < 10;
            break;
        case 4:
            enabled = mSettings[2] > 0;
            break;
        case 5:
            enabled = mSettings[2] < 10;
            break;
        default:
            enabled = false;
            break;
        }

        if (!enabled)
        {
            return;
        }

        if (!mButtonComponents[item].fn_802192FC(1, index))
        {
            mButtons[item]->SetActiveSlide("off", true, false);
        }
        mButtonComponents[item].mValues[index] = 0;
    }
}

void OptionsAudioMenuV2::fn_801D575C(int index)
{
    mSaveButtonComponent.mValues[index] = 1;
    if (!mSaveButtonComponent.fn_802192FC(1, index))
    {
        mSaveButton->SetActiveSlide("over", true, false);
        fn_801CBCA0(0xAA73EF33, 0, 0, 1);
    }
}

void OptionsAudioMenuV2::fn_801D57D8(int index)
{
    mSaveButtonComponent.mValues[index] = 0;
    if (!mSaveButtonComponent.fn_802192FC(1, index))
    {
        mSaveButton->SetActiveSlide("off", true, false);
    }
}

void OptionsAudioMenuV2::fn_801D583C()
{
    mState = 3;
    TU80252180Scene* object = fn_80253E18();
    if (object != 0)
    {
        fn_802534BC(object, 0, true);
    }
    mPresentation->SetActiveSlide("OPTIONS_OUT", true);
    mUnidentified686 = true;
    mSaveButton->SetActiveSlide("down", true, false);
    fn_801CBCA0(0xF0AFD586, 0, 0, 1);
    fn_801CBCA0(0x304FDD1E, 0, 0, 1);
    SaveLoad::StartSave(false);
}

OptionsVisualMenuV2::OptionsVisualMenuV2(int value)
    : mUnidentified28(value)
    , mNavigation()
    , mUnidentified6C4(false)
    , mUnidentified6C5(false)
    , mUnidentified6C6(false)
    , mState(0)
{
    for (int i = 0; i < 5; ++i)
    {
        mButtonComponents[i].mContext = (void*)i;
        mButtonComponents[i].mSpeakerEnabled = false;
    }
    for (int i = 0; i < 2; ++i)
    {
        mZoomButtonComponents[i].mContext = (void*)i;
        mZoomButtonComponents[i].mSpeakerEnabled = false;
    }

    VisualSettings settings = *(VisualSettings*)GameInfoManager::Instance()->GetUnknown0xB8();
    mSettings[0] = !settings.mIsAutoZoomCamera;
    mSettings[1] = (int)(10.0f * settings.mCameraZoomLevel);
    mNavigation.fn_801D2BE0(false);
    mBackupSettings[0] = mSettings[0];
    mBackupSettings[1] = mSettings[1];
}

OptionsVisualMenuV2::~OptionsVisualMenuV2()
{
}

void OptionsVisualMenuV2::fn_801D7948(int index, void* context)
{
    unsigned int item = (unsigned int)context;
    if (!mButtonComponents[item].fn_802192FC(2, -1))
    {
        mButtonComponents[item].fn_802195B4(index);
        if (!mButtonComponents[item].fn_802192FC(1, index))
        {
            mButtons[item]->SetActiveSlide("over", true, false);
            fn_801CBCA0(0x96DEB5C3, 0, 0, 1);
        }
        mButtonComponents[item].mValues[index] = 1;
    }
}

void OptionsVisualMenuV2::fn_801D7A0C(int index, void* context)
{
    unsigned int item = (unsigned int)context;
    if (!mButtonComponents[item].fn_802192FC(2, -1))
    {
        if (!mButtonComponents[item].fn_802192FC(1, index))
        {
            mButtons[item]->SetActiveSlide("off", true, false);
        }
        mButtonComponents[item].mValues[index] = 0;
    }
}

void OptionsVisualMenuV2::fn_801D7EC8(int index, void* context)
{
    unsigned int item = (unsigned int)context;
    if ((unsigned int)mSettings[0] != item)
    {
        if (!mZoomButtonComponents[item].fn_802192FC(2, -1))
        {
            mZoomButtonComponents[item].fn_802195B4(index);
            if (!mZoomButtonComponents[item].fn_802192FC(1, index))
            {
                mZoomButtons[item]->SetActiveSlide("over", true, false);
                fn_801CBCA0(0xF6EB899E, 0, 0, 1);
            }
            mZoomButtonComponents[item].mValues[index] = 1;
        }
    }
}

void OptionsVisualMenuV2::fn_801D7F9C(int index, void* context)
{
    unsigned int item = (unsigned int)context;
    if ((unsigned int)mSettings[0] != item)
    {
        if (!mZoomButtonComponents[item].fn_802192FC(2, -1))
        {
            if (!mZoomButtonComponents[item].fn_802192FC(1, index))
            {
                mZoomButtons[item]->SetActiveSlide("off", true, false);
            }
            mZoomButtonComponents[item].mValues[index] = 0;
        }
    }
}

void OptionsVisualMenuV2::fn_801D8458(int index)
{
    mSaveButtonComponent.mValues[index] = 1;
    if (!mSaveButtonComponent.fn_802192FC(1, index))
    {
        mSaveButton->SetActiveSlide("over", true, false);
        fn_801CBCA0(0xAA73EF33, 0, 0, 1);
    }
}

void OptionsVisualMenuV2::fn_801D84D4(int index)
{
    mSaveButtonComponent.mValues[index] = 0;
    if (!mSaveButtonComponent.fn_802192FC(1, index))
    {
        mSaveButton->SetActiveSlide("off", true, false);
    }
}

void OptionsVisualMenuV2::fn_801D8538()
{
    mState = 3;
    TU80252180Scene* object = fn_80253E18();
    if (object != 0)
    {
        fn_802534BC(object, 0, true);
    }
    mPresentation->SetActiveSlide("OPTIONS_OUT", true);
    mPresentation->Update(0.0f);
    mSaveButton->SetActiveSlide("down", true, false);
    fn_801CBCA0(0xF0AFD586, 0, 0, 1);
    fn_801CBCA0(0x304FDD1E, 0, 0, 1);
    mUnidentified6C6 = true;
    SaveLoad::StartSave(false);
}
