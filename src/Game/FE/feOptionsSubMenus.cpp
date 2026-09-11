#include "Game/SH/SHNavigation.h"
#include "Game/FE/feOptionsSubMenus.h"
#include "Game/FE/FEAudio.h"

#include "Game/DB/SaveLoad.h"
#include "Game/DB/UserOptions.h"
#include "Game/FE/fePresentation.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlInstance.h"
#include "Game/GameInfo.h"
#include "NL/nlColour.h"
#include "NL/nlConfig.h"
#include "Game/FE/FEAudio.h"
#include "Game/SH/SHNavigation.h"

class SHNavigation;

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

    AudioSettings* settings = GameInfoManager::Instance()->GetAudioSettings();
    mSettings[0] = settings->MusicVolume;
    mSettings[1] = settings->SFXVolume;
    mSettings[2] = settings->VoiceVolume;
    mBackupSettings[0] = mSettings[0];
    mBackupSettings[1] = mSettings[1];
    mBackupSettings[2] = mSettings[2];
    mNavigation.SetPopScene(false);
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
    if (mButtonComponents[item].HasOtherPointerState(2, -1)
        || !UnidentifiedVolumeButtonEnabled(item))
    {
        return;
    }

    mButtonComponents[item].PlayHoverFeedback(index);
    if (!mButtonComponents[item].HasOtherPointerState(1, index))
    {
        mButtons[item]->SetActiveSlide("over", true, false);
        FEAudio::PlayAnimAudioEvent(0x96DEB5C3, 0, 0, 1);
    }
    mButtonComponents[item].SetPointerState(1, index);
}

void OptionsAudioMenuV2::fn_801D5108(int index, void* context)
{
    unsigned int item = (unsigned int)context;
    if (mButtonComponents[item].HasOtherPointerState(2, -1)
        || !UnidentifiedVolumeButtonEnabled(item))
    {
        return;
    }

    if (!mButtonComponents[item].HasOtherPointerState(1, index))
    {
        mButtons[item]->SetActiveSlide("off", true, false);
    }
    mButtonComponents[item].SetPointerState(0, index);
}

void OptionsAudioMenuV2::fn_801D575C(int index)
{
    mSaveButtonComponent.SetPointerState(1, index);
    if (!mSaveButtonComponent.HasOtherPointerState(1, index))
    {
        mSaveButton->SetActiveSlide("over", true, false);
        FEAudio::PlayAnimAudioEvent(0xAA73EF33, 0, 0, 1);
    }
}

void OptionsAudioMenuV2::fn_801D57D8(int index)
{
    mSaveButtonComponent.SetPointerState(0, index);
    if (!mSaveButtonComponent.HasOtherPointerState(1, index))
    {
        mSaveButton->SetActiveSlide("off", true, false);
    }
}

void OptionsAudioMenuV2::fn_801D583C()
{
    mState = 3;
    SHNavigation* object = GetNavigationScene();
    if (object != 0)
    {
        object->SetButtons(0, true);
    }
    mPresentation->SetActiveSlide("OPTIONS_OUT", true);
    mUnidentified686 = true;
    mSaveButton->SetActiveSlide("down", true, false);
    FEAudio::PlayAnimAudioEvent(0xF0AFD586, 0, 0, 1);
    FEAudio::PlayAnimAudioEvent(0x304FDD1E, 0, 0, 1);
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

    VisualSettings settings = *GameInfoManager::Instance()->GetVisualOptions();
    mSettings[0] = !settings.mIsAutoZoomCamera;
    mSettings[1] = (int)(10.0f * settings.mCameraZoomLevel);
    mNavigation.SetPopScene(false);
    mBackupSettings[0] = mSettings[0];
    mBackupSettings[1] = mSettings[1];
}

OptionsVisualMenuV2::~OptionsVisualMenuV2()
{
}

void OptionsVisualMenuV2::fn_801D7948(int index, void* context)
{
    unsigned int item = (unsigned int)context;
    if (!mButtonComponents[item].HasOtherPointerState(2, -1))
    {
        mButtonComponents[item].PlayHoverFeedback(index);
        if (!mButtonComponents[item].HasOtherPointerState(1, index))
        {
            mButtons[item]->SetActiveSlide("over", true, false);
            FEAudio::PlayAnimAudioEvent(0x96DEB5C3, 0, 0, 1);
        }
        mButtonComponents[item].SetPointerState(1, index);
    }
}

void OptionsVisualMenuV2::fn_801D7A0C(int index, void* context)
{
    unsigned int item = (unsigned int)context;
    if (!mButtonComponents[item].HasOtherPointerState(2, -1))
    {
        if (!mButtonComponents[item].HasOtherPointerState(1, index))
        {
            mButtons[item]->SetActiveSlide("off", true, false);
        }
        mButtonComponents[item].SetPointerState(0, index);
    }
}

void OptionsVisualMenuV2::fn_801D7EC8(int index, void* context)
{
    unsigned int item = (unsigned int)context;
    if ((unsigned int)mSettings[0] == item
        || mZoomButtonComponents[item].HasOtherPointerState(2, -1))
    {
        return;
    }

    mZoomButtonComponents[item].PlayHoverFeedback(index);
    if (!mZoomButtonComponents[item].HasOtherPointerState(1, index))
    {
        mZoomButtons[item]->SetActiveSlide("over", true, false);
        FEAudio::PlayAnimAudioEvent(0xF6EB899E, 0, 0, 1);
    }
    mZoomButtonComponents[item].SetPointerState(1, index);
}

void OptionsVisualMenuV2::fn_801D7F9C(int index, void* context)
{
    unsigned int item = (unsigned int)context;
    if ((unsigned int)mSettings[0] == item
        || mZoomButtonComponents[item].HasOtherPointerState(2, -1))
    {
        return;
    }
    if (!mZoomButtonComponents[item].HasOtherPointerState(1, index))
    {
        mZoomButtons[item]->SetActiveSlide("off", true, false);
    }
    mZoomButtonComponents[item].SetPointerState(0, index);
}

void OptionsVisualMenuV2::fn_801D8458(int index)
{
    mSaveButtonComponent.SetPointerState(1, index);
    if (!mSaveButtonComponent.HasOtherPointerState(1, index))
    {
        mSaveButton->SetActiveSlide("over", true, false);
        FEAudio::PlayAnimAudioEvent(0xAA73EF33, 0, 0, 1);
    }
}

void OptionsVisualMenuV2::fn_801D84D4(int index)
{
    mSaveButtonComponent.SetPointerState(0, index);
    if (!mSaveButtonComponent.HasOtherPointerState(1, index))
    {
        mSaveButton->SetActiveSlide("off", true, false);
    }
}

void OptionsVisualMenuV2::fn_801D8538()
{
    mState = 3;
    SHNavigation* object = GetNavigationScene();
    if (object != 0)
    {
        object->SetButtons(0, true);
    }
    mPresentation->SetActiveSlide("OPTIONS_OUT", true);
    mPresentation->Update(0.0f);
    mSaveButton->SetActiveSlide("down", true, false);
    FEAudio::PlayAnimAudioEvent(0xF0AFD586, 0, 0, 1);
    FEAudio::PlayAnimAudioEvent(0x304FDD1E, 0, 0, 1);
    mUnidentified6C6 = true;
    SaveLoad::StartSave(false);
}
