#include "Game/SH/SHNavigation.h"
#include "Game/FE/feOptionsSubMenus.h"
#include "Game/FE/FEAudio.h"

#include "Game/DB/SaveLoad.h"
#include "Game/DB/UserOptions.h"
#include "Game/FE/fePresentation.h"
#include "Game/FE/feDPD.h"
#include "Game/FE/feInput.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlInstance.h"
#include "Game/FE/tlSlide.h"
#include "Game/BaseGameSceneManager.h"
#include "Game/GameSceneManager.h"
#include "Game/GameInfo.h"
#include "NL/nlColour.h"
#include "NL/nlConfig.h"
#include "NL/nlFormat.h"
#include "NL/nlLocalizationLookup.h"
#include "NL/nlPrint.h"
#include "Game/FE/feFinder.h"
#include "Game/FE/feInlineHasher.h"
#include "Game/FE/feFinder_impl.h"
#include "Game/FE/tlTextInstance.h"
#include "Game/FE/feHelpFuncs_decl.h"
#include "Game/FE/feScene.h"
#include "Game/FE/fePackage.h"
#include "Game/Render/RLViewLayers.h"
#include "NL/nlFunction.inl"
#include "NL/nlBind.h"
#include "NL/nlMemory.h"
#include "NL/nlTask.h"
#include <string.h>

OptionsAudioMenuV2::OptionsAudioMenuV2(int mode)
    : mOverlayMode(mode)
    , mNavigation()
    , mPointerButtonsInitialized(false)
    , mIntroSoundPlayed(false)
    , mSaveStarted(false)
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

void OptionsAudioMenuV2::SceneCreated()
{
    TLComponentInstance* backButton = 0;
    FEPresentation* presentation = mPresentation;
    SHNavigation* navigation = GetNavigationScene();
    if (navigation != 0)
    {
        navigation->HideButtons();
        backButton = navigation->GetButton(4);
        mSaveButton = navigation->GetButton(0x20);
    }
    mNavigation.SetButtonInstance(backButton);

    for (int i = 0; i < 4; ++i)
        GetPointerInstance(i)->SetActiveSlide("waiting", true, false);

    if (mOverlayMode == 0)
    {
        FEFinder<TLInstance, 2>::Find<>(presentation,
            "OPTIONS_IN", "Layer", "blackbox")->m_bVisible = false;
        FEFinder<TLInstance, 2>::Find<>(presentation,
            "OPTIONS_OUT", "Layer", "blackbox")->m_bVisible = false;
    }

    TLComponentInstance* right = FEFinder<TLComponentInstance, 4>::Find<>(presentation,
        "OPTIONS_IN", "Layer", "visual_options", "scrollbar_right");
    TLComponentInstance* left = FEFinder<TLComponentInstance, 4>::Find<>(presentation,
        "OPTIONS_IN", "Layer", "visual_options", "scrollbar_left");
    mButtons[0] = FEFinder<TLComponentInstance, 4>::Find<>(left->GetActiveSlide(), "down_arrow");
    mButtons[1] = FEFinder<TLComponentInstance, 4>::Find<>(right->GetActiveSlide(), "up_arrow");
    mButtons[2] = FEFinder<TLComponentInstance, 4>::Find<>(left->GetActiveSlide(), "down_arrow2");
    mButtons[3] = FEFinder<TLComponentInstance, 4>::Find<>(right->GetActiveSlide(), "up_arrow2");
    mButtons[4] = FEFinder<TLComponentInstance, 4>::Find<>(left->GetActiveSlide(), "down_arrow3");
    mButtons[5] = FEFinder<TLComponentInstance, 4>::Find<>(right->GetActiveSlide(), "up_arrow3");

    TLInstance* volume[3];
    volume[0] = FEFinder<TLInstance, 2>::Find<>(presentation,
        "OPTIONS_IN", "Layer", "visual_options", "MUSIC VOLUME");
    volume[1] = FEFinder<TLInstance, 2>::Find<>(presentation,
        "OPTIONS_IN", "Layer", "visual_options", "SFX VOLUME");
    volume[2] = FEFinder<TLInstance, 2>::Find<>(presentation,
        "OPTIONS_IN", "Layer", "visual_options", "VOX VOLUME");
    for (int i = 0; i < 10; ++i)
    {
        char name[12];
        if (i == 0)
            nlSNPrintf(name, 12, "whitebox");
        else
            nlSNPrintf(name, 12, "whitebox%d", i + 1);
        mVolumeBars[0][i] = FEFinder<TLInstance, 2>::Find<>(volume[0], name);
        mVolumeBars[1][i] = FEFinder<TLInstance, 2>::Find<>(volume[1], name);
        mVolumeBars[2][i] = FEFinder<TLInstance, 2>::Find<>(volume[2], name);
    }

    UpdateVolumeLevelText(0);
    UpdateVolumeLevelText(1);
    UpdateVolumeLevelText(2);
    UpdateVolumeBars(0);
    UpdateVolumeBars(1);
    UpdateVolumeBars(2);
    MarkUnusedVolumeButtons();
}

void OptionsAudioMenuV2::MarkUnusedVolumeButtons()
{
    for (int i = 0; i < 6; ++i)
    {
        if (!IsVolumeButtonEnabled(i))
            mButtons[i]->SetActiveSlide("unused", true, false);
    }
}

void OptionsAudioMenuV2::Update(float fDeltaT)
{
    if (g_pFEInput->m_InputLockDepth != 0)
        return;

    if (!mIntroSoundPlayed && mOverlayMode == 1)
    {
        mIntroSoundPlayed = true;
        FEAudio::PlayAnimAudioEvent(0xBB142B94, 0, 0, 1);
    }

    BaseSceneHandler::Update(fDeltaT);

    if (mState == 0 || mState == 2 || mState == 3)
    {
        TLSlide* slide = mPresentation->m_currentSlide;
        if (slide->GetCurrentTime() < slide->GetStartTime() + slide->GetDuration())
        {
            for (int i = 0; i < 4; ++i)
                GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
            return;
        }

        if (mState == 0)
        {
            SHNavigation* navigation = GetNavigationScene();
            if (navigation != 0)
            {
                navigation->SetButtons(0x24, true);
                navigation->SetDoneButtonText(1);
            }
            mState = 1;
        }
        else if (mState == 2)
        {
            return;
        }
        else if (mState == 3)
        {
            if (mOverlayMode == 0)
                GameSceneManager::Instance()->Push((SceneList)13, SCREEN_NOTHING, true);
            else
                g_pOverlayManager->Push((SceneList)0x50, SCREEN_NOTHING, true);
            return;
        }
    }

    if (!mPointerButtonsInitialized)
    {
        InitializePointerButtons();
        mPointerButtonsInitialized = true;
    }

    for (int i = 0; i < 4; ++i)
    {
        TLComponentInstance* pointer = GetPointerInstance(i);
        if (mOverlayMode == 0 && (unsigned int)i != gFEControllerIndex)
        {
            pointer->SetActiveSlide("waiting", true, false);
            continue;
        }

        pointer->SetActiveSlide("cursor", true, false);

        unsigned char valid = true;
        FEPointerEvent event;
        event.mIndex = i;
        event.mPosition = GetPointerPosition(i, &valid);
        event.mPressed = g_pFEInput->JustPressed((eFEINPUT_PAD)i, 0x1E, true, 0);

        if (mNavigation.UpdateBackButton(event, fDeltaT))
        {
            AudioSettings* settings = GameInfoManager::Instance()->GetAudioSettings();
            settings->MusicVolume = mBackupSettings[0];
            settings->SFXVolume = mBackupSettings[1];
            settings->VoiceVolume = mBackupSettings[2];
            settings->ApplySettings();
            FEAudio::PlayAnimAudioEvent(0x304FDD1E, 0, 0, 1);
            mState = 3;
            SHNavigation* navigation = GetNavigationScene();
            if (navigation != 0)
                navigation->SetButtons(0, true);
            mPresentation->SetActiveSlide("OPTIONS_OUT", true);
            return;
        }

        for (int j = 0; j < 6; ++j)
            mButtonComponents[j].HandlePointerEvent(&event);
        mSaveButtonComponent.HandlePointerEvent(&event);
        if (mSaveStarted)
            break;
    }
}

void OptionsAudioMenuV2::InitializePointerButtons()
{
    typedef Detail::MemFunImpl<void, void (OptionsAudioMenuV2::*)(int, void*)> ButtonMethod;
    typedef BindExp3<void, ButtonMethod, OptionsAudioMenuV2*, Placeholder<0>, Placeholder<1> > ButtonBinding;
    FEPointerListener::Callback enter(ButtonBinding(
        MemFun(&OptionsAudioMenuV2::OnVolumeButtonPointerEnter), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback leave(ButtonBinding(
        MemFun(&OptionsAudioMenuV2::OnVolumeButtonPointerLeave), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback press(ButtonBinding(
        MemFun(&OptionsAudioMenuV2::OnVolumeButtonPointerPress), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback saveEnter(ButtonBinding(
        MemFun(&OptionsAudioMenuV2::OnSaveButtonPointerEnter), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback saveLeave(ButtonBinding(
        MemFun(&OptionsAudioMenuV2::OnSaveButtonPointerLeave), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback savePress(ButtonBinding(
        MemFun(&OptionsAudioMenuV2::OnSaveButtonPointerPress), this, Placeholder<0>(), Placeholder<1>()));

    TLInstance* right = FEFinder<TLInstance, 2>::Find(mPresentation,
        "OPTIONS_IN", "Layer", "visual_options", "scrollbar_right");
    TLInstance* left = FEFinder<TLInstance, 2>::Find(mPresentation,
        "OPTIONS_IN", "Layer", "visual_options", "scrollbar_left");
    feVector3 leftPosition = left->GetAssetPosition();
    feVector3 rightPosition = right->GetAssetPosition();
    for (int i = 0; i < 6; ++i)
    {
        if (i == 0 || i == 2 || i == 4)
            mButtonComponents[i].SetInstanceBounds(mButtons[i], false,
                leftPosition.f.x, leftPosition.f.y, 1.0f, 1.0f);
        else
            mButtonComponents[i].SetInstanceBounds(mButtons[i], false,
                rightPosition.f.x, rightPosition.f.y, 1.0f, 1.0f);
        mButtonComponents[i].SetPointerEnterCallback(enter);
        mButtonComponents[i].SetPointerLeaveCallback(leave);
        mButtonComponents[i].SetPointerPressCallback(press);
    }
    SetDoneButtonBounds(&mSaveButtonComponent, mSaveButton, 0);
    mSaveButtonComponent.SetPointerEnterCallback(saveEnter);
    mSaveButtonComponent.SetPointerLeaveCallback(saveLeave);
    mSaveButtonComponent.SetPointerPressCallback(savePress);
}

void OptionsAudioMenuV2::UpdateVolumeBars(int setting)
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

void OptionsAudioMenuV2::OnVolumeButtonPointerEnter(int index, void* context)
{
    unsigned int item = (unsigned int)context;
    if (mButtonComponents[item].HasOtherPointerState(2, -1)
        || !IsVolumeButtonEnabled(item))
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

void OptionsAudioMenuV2::OnVolumeButtonPointerLeave(int index, void* context)
{
    unsigned int item = (unsigned int)context;
    if (mButtonComponents[item].HasOtherPointerState(2, -1)
        || !IsVolumeButtonEnabled(item))
    {
        return;
    }

    if (!mButtonComponents[item].HasOtherPointerState(1, index))
    {
        mButtons[item]->SetActiveSlide("off", true, false);
    }
    mButtonComponents[item].SetPointerState(0, index);
}

void OptionsAudioMenuV2::OnVolumeButtonPointerPress(int index, void* context)
{
    unsigned int item = (unsigned int)context;
    if (mButtonComponents[item].HasOtherPointerState(2, -1)
        || !IsVolumeButtonEnabled(item))
        return;

    AudioSettings* settings = GameInfoManager::Instance()->GetAudioSettings();
    switch (item)
    {
    case 0:
        settings->MusicVolume = --mSettings[0];
        settings->ApplyMusicVolume();
        UpdateVolumeBars(0);
        UpdateVolumeLevelText(0);
        mButtons[1]->SetActiveSlide("off", true, false);
        FEAudio::PlayAnimAudioEvent(0x3021A1EE, 0, 0, 1);
        break;
    case 1:
        settings->MusicVolume = ++mSettings[0];
        settings->ApplyMusicVolume();
        UpdateVolumeBars(0);
        UpdateVolumeLevelText(0);
        mButtons[0]->SetActiveSlide("off", true, false);
        FEAudio::PlayAnimAudioEvent(0x3021A1EE, 0, 0, 1);
        break;
    case 2:
        settings->SFXVolume = --mSettings[1];
        settings->ApplySFXVolume();
        UpdateVolumeBars(1);
        UpdateVolumeLevelText(1);
        mButtons[3]->SetActiveSlide("off", true, false);
        FEAudio::PlayAnimAudioEvent(0x3021A1EE, 0, 0, 1);
        break;
    case 3:
        settings->SFXVolume = ++mSettings[1];
        settings->ApplySFXVolume();
        UpdateVolumeBars(1);
        UpdateVolumeLevelText(1);
        mButtons[2]->SetActiveSlide("off", true, false);
        FEAudio::PlayAnimAudioEvent(0x3021A1EE, 0, 0, 1);
        break;
    case 4:
        settings->VoiceVolume = --mSettings[2];
        settings->ApplyVoiceVolume();
        UpdateVolumeBars(2);
        UpdateVolumeLevelText(2);
        if (mOverlayMode == 0)
            FEAudio::PlayAnimAudioEvent(0x1F824C84, 0, 0, 1);
        else
            FEAudio::PlaySound(1, 0x270203ED, 0, 0);
        mButtons[5]->SetActiveSlide("off", true, false);
        break;
    case 5:
        settings->VoiceVolume = ++mSettings[2];
        settings->ApplyVoiceVolume();
        UpdateVolumeBars(2);
        UpdateVolumeLevelText(2);
        if (mOverlayMode == 0)
            FEAudio::PlayAnimAudioEvent(0x1F824C84, 0, 0, 1);
        else
            FEAudio::PlaySound(1, 0x270203ED, 0, 0);
        mButtons[4]->SetActiveSlide("off", true, false);
        break;
    }

    if (IsVolumeButtonEnabled(item))
        mButtons[item]->SetActiveSlide("slide1", true, false);
    else
        mButtons[item]->SetActiveSlide("unused", true, false);
}

void OptionsAudioMenuV2::OnSaveButtonPointerEnter(int index, void*)
{
    mSaveButtonComponent.SetPointerState(1, index);
    if (!mSaveButtonComponent.HasOtherPointerState(1, index))
    {
        mSaveButton->SetActiveSlide("over", true, false);
        FEAudio::PlayAnimAudioEvent(0xAA73EF33, 0, 0, 1);
    }
}

void OptionsAudioMenuV2::OnSaveButtonPointerLeave(int index, void*)
{
    mSaveButtonComponent.SetPointerState(0, index);
    if (!mSaveButtonComponent.HasOtherPointerState(1, index))
    {
        mSaveButton->SetActiveSlide("off", true, false);
    }
}

void OptionsAudioMenuV2::OnSaveButtonPointerPress(int, void*)
{
    mState = 3;
    SHNavigation* navigation = GetNavigationScene();
    if (navigation != 0)
    {
        navigation->SetButtons(0, true);
    }
    mPresentation->SetActiveSlide("OPTIONS_OUT", true);
    mSaveStarted = true;
    mSaveButton->SetActiveSlide("down", true, false);
    FEAudio::PlayAnimAudioEvent(0xF0AFD586, 0, 0, 1);
    FEAudio::PlayAnimAudioEvent(0x304FDD1E, 0, 0, 1);
    SaveLoad::StartSave(false);
}

void OptionsAudioMenuV2::UpdateVolumeLevelText(int setting)
{
    unsigned short number[4];
    if (setting == 0)
    {
        TLTextInstance* text = FEFindTextInstance(
            mPresentation->m_currentSlide, "Layer", "visual_options", "MUSIC SETTING");
        nlSNPrintf(number, 4, (const unsigned short*)L"%d", mSettings[0]);
        BasicString<unsigned short, Detail::TempStringAllocator> formatted = Format(
            BasicString<unsigned short, Detail::TempStringAllocator>(
                g_pLocalization->GetString("OPTIONS_AUDIO_MUSIC_LEVEL")), number);
        memcpy(mFormattedSettings[0], formatted.c_str(), 0x20);
        text->SetString(mFormattedSettings[0]);
    }
    else if (setting == 1)
    {
        TLTextInstance* text = FEFindTextInstance(
            mPresentation->m_currentSlide, "Layer", "visual_options", "SFX SETTING2");
        nlSNPrintf(number, 4, (const unsigned short*)L"%d", mSettings[1]);
        BasicString<unsigned short, Detail::TempStringAllocator> formatted = Format(
            BasicString<unsigned short, Detail::TempStringAllocator>(
                g_pLocalization->GetString("OPTIONS_AUDIO_SFX_LEVEL")), number);
        memcpy(mFormattedSettings[1], formatted.c_str(), 0x20);
        text->SetString(mFormattedSettings[1]);
    }
    else
    {
        TLTextInstance* text = FEFindTextInstance(
            mPresentation->m_currentSlide, "Layer", "visual_options", "VOX SETTING3");
        nlSNPrintf(number, 4, (const unsigned short*)L"%d", mSettings[2]);
        BasicString<unsigned short, Detail::TempStringAllocator> formatted = Format(
            BasicString<unsigned short, Detail::TempStringAllocator>(
                g_pLocalization->GetString("OPTIONS_AUDIO_VOX_LEVEL")), number);
        memcpy(mFormattedSettings[2], formatted.c_str(), 0x20);
        text->SetString(mFormattedSettings[2]);
    }
}

OptionsVisualMenuV2::OptionsVisualMenuV2(int mode)
    : mOverlayMode(mode)
    , mNavigation()
    , mPointerButtonsInitialized(false)
    , mIntroSoundPlayed(false)
    , mSaveStarted(false)
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
    mSettings[1] = (int)(4.0f * settings.mCameraZoomLevel);
    mNavigation.SetPopScene(false);
    mBackupSettings[0] = mSettings[0];
    mBackupSettings[1] = mSettings[1];
}

OptionsVisualMenuV2::~OptionsVisualMenuV2()
{
}

inline void OptionsVisualMenuV2::UpdateZoomLevelText(TLTextInstance* text, const u16 (&number)[4], const u16* localized)
{
    BasicString<unsigned short, Detail::TempStringAllocator> formatted = Format(
        BasicString<unsigned short, Detail::TempStringAllocator>(localized), number);
    memcpy(mFormattedZoomLevel, formatted.c_str(), 0x20);
    text->SetString(mFormattedZoomLevel);
}

void OptionsVisualMenuV2::SceneCreated()
{
    TLComponentInstance* backButton = 0;
    FEPresentation* presentation = mPresentation;
    SHNavigation* navigation = GetNavigationScene();
    if (navigation != 0)
    {
        navigation->HideButtons();
        backButton = navigation->GetButton(4);
        mSaveButton = navigation->GetButton(0x20);
    }
    mNavigation.SetButtonInstance(backButton);

    if (mOverlayMode == 0)
    {
        FEFinder<TLInstance, 2>::Find<>(presentation,
            "OPTIONS_IN", "Layer", "blackbox")->m_bVisible = false;
        FEFinder<TLInstance, 2>::Find<>(presentation,
            "OPTIONS_OUT", "Layer", "blackbox")->m_bVisible = false;
    }

    for (int i = 0; i < 4; ++i)
        GetPointerInstance(i)->SetActiveSlide("waiting", true, false);

    for (int i = 0; i < 5; ++i)
    {
        char name[12];
        nlSNPrintf(name, 12, "BUTTON_%d", i);
        mButtons[i] = FEFinder<TLComponentInstance, 4>::Find<>(presentation,
            "OPTIONS_IN", "Layer", "visual_options", "ZOOM LEVELS", name);
    }
    mZoomButtons[0] = FEFinder<TLComponentInstance, 4>::Find<>(presentation,
        "OPTIONS_IN", "Layer", "visual_options", "BTN_AUTOZOOM");
    mZoomButtons[1] = FEFinder<TLComponentInstance, 4>::Find<>(presentation,
        "OPTIONS_IN", "Layer", "visual_options", "MANUAL_ZOOM");

    mZoomButtons[mSettings[0]]->SetActiveSlide("down", true, false);
    FEPointerButton& zoomButton = mZoomButtonComponents[mSettings[0]];
    for (int i = 0; i < 4; ++i)
        zoomButton.SetPointerState(2, i);
    mButtons[mSettings[1]]->SetActiveSlide("down", true, false);
    FEPointerButton& button = mButtonComponents[mSettings[1]];
    for (int i = 0; i < 4; ++i)
        button.SetPointerState(2, i);

    TLTextInstance* text = FEFinder<TLTextInstance, 4>::Find(
        mPresentation->m_currentSlide, "Layer", "visual_options", "SERIES SETTING");
    unsigned short number[4];
    nlSNPrintf(number, 4, (const unsigned short*)L"%d", mSettings[1] + 1);
    UpdateZoomLevelText(text, number, g_pLocalization->GetString("OPTIONS_VISUAL_ZOOMLEVEL"));
}

void OptionsVisualMenuV2::Update(float fDeltaT)
{
    if (g_pFEInput->m_InputLockDepth != 0)
        return;

    if (!mIntroSoundPlayed && mOverlayMode == 1)
    {
        mIntroSoundPlayed = true;
        FEAudio::PlayAnimAudioEvent(0xBB142B94, 0, 0, 1);
    }

    BaseSceneHandler::Update(fDeltaT);

    if (mState == 0 || mState == 2 || mState == 3)
    {
        TLSlide* slide = mPresentation->m_currentSlide;
        if (slide->GetCurrentTime() < slide->GetStartTime() + slide->GetDuration())
        {
            for (int i = 0; i < 4; ++i)
                GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
            return;
        }

        if (mState == 0)
        {
            SHNavigation* navigation = GetNavigationScene();
            if (navigation != 0)
            {
                navigation->SetButtons(0x24, true);
                navigation->SetDoneButtonText(1);
            }
            mState = 1;
        }
        else if (mState == 2)
        {
            return;
        }
        else if (mState == 3)
        {
            if (mOverlayMode == 0)
                GameSceneManager::Instance()->Push((SceneList)13, SCREEN_NOTHING, true);
            else
                g_pOverlayManager->Push((SceneList)0x50, SCREEN_NOTHING, true);
            return;
        }
    }

    if (!mPointerButtonsInitialized)
    {
        InitializePointerButtons();
        mPointerButtonsInitialized = true;
    }

    for (int i = 0; i < 4; ++i)
    {
        TLComponentInstance* pointer = GetPointerInstance(i);
        if (mOverlayMode == 0 && (unsigned int)i != gFEControllerIndex)
        {
            pointer->SetActiveSlide("waiting", true, false);
            continue;
        }

        pointer->SetActiveSlide("cursor", true, false);

        unsigned char valid = true;
        FEPointerEvent event;
        event.mIndex = i;
        event.mPosition = GetPointerPosition(i, &valid);
        event.mPressed = g_pFEInput->JustPressed((eFEINPUT_PAD)i, 0x1E, true, 0);

        if (mNavigation.UpdateBackButton(event, fDeltaT))
        {
            GameInfoManager::Instance()->mUserInfo.mVisualOptions.mCameraZoomLevel = mBackupSettings[1] / 4.0;
            GameInfoManager::Instance()->mUserInfo.mVisualOptions.mIsAutoZoomCamera = mBackupSettings[0] == 0;
            FEAudio::PlayAnimAudioEvent(0x304FDD1E, 0, 0, 1);
            mState = 3;
            SHNavigation* navigation = GetNavigationScene();
            if (navigation != 0)
                navigation->SetButtons(0, true);
            mPresentation->SetActiveSlide("OPTIONS_OUT", true);
            mPresentation->Update(0.0f);
            return;
        }

        for (int j = 0; j < 5; ++j)
            mButtonComponents[j].HandlePointerEvent(&event);
        mZoomButtonComponents[0].HandlePointerEvent(&event);
        mZoomButtonComponents[1].HandlePointerEvent(&event);
        mSaveButtonComponent.HandlePointerEvent(&event);
        if (mSaveStarted)
            break;
    }
}

void OptionsVisualMenuV2::InitializePointerButtons()
{
    typedef Detail::MemFunImpl<void, void (OptionsVisualMenuV2::*)(int, void*)> ButtonMethod;
    typedef BindExp3<void, ButtonMethod, OptionsVisualMenuV2*, Placeholder<0>, Placeholder<1> > ButtonBinding;
    FEPointerListener::Callback enter(ButtonBinding(
        MemFun(&OptionsVisualMenuV2::OnZoomLevelPointerEnter), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback leave(ButtonBinding(
        MemFun(&OptionsVisualMenuV2::OnZoomLevelPointerLeave), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback press(ButtonBinding(
        MemFun(&OptionsVisualMenuV2::OnZoomLevelPointerPress), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback saveEnter(ButtonBinding(
        MemFun(&OptionsVisualMenuV2::OnSaveButtonPointerEnter), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback saveLeave(ButtonBinding(
        MemFun(&OptionsVisualMenuV2::OnSaveButtonPointerLeave), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback savePress(ButtonBinding(
        MemFun(&OptionsVisualMenuV2::OnSaveButtonPointerPress), this, Placeholder<0>(), Placeholder<1>()));

    TLInstance* levels = FEFinder<TLInstance, 2>::Find<>(mPresentation,
        "OPTIONS_IN", "Layer", "visual_options", "ZOOM LEVELS");
    feVector3 position = levels->GetAssetPosition();
    for (int i = 0; i < 5; ++i)
    {
        mButtonComponents[i].SetInstanceBounds(mButtons[i], true,
            position.f.x, position.f.y, 1.0f, 1.0f);
        mButtonComponents[i].SetPointerEnterCallback(enter);
        mButtonComponents[i].SetPointerLeaveCallback(leave);
        mButtonComponents[i].SetPointerPressCallback(press);
    }

    enter = FEPointerListener::Callback(ButtonBinding(
        MemFun(&OptionsVisualMenuV2::OnZoomModePointerEnter), this, Placeholder<0>(), Placeholder<1>()));
    leave = FEPointerListener::Callback(ButtonBinding(
        MemFun(&OptionsVisualMenuV2::OnZoomModePointerLeave), this, Placeholder<0>(), Placeholder<1>()));
    press = FEPointerListener::Callback(ButtonBinding(
        MemFun(&OptionsVisualMenuV2::OnZoomModePointerPress), this, Placeholder<0>(), Placeholder<1>()));
    for (int i = 0; i < 2; ++i)
    {
        mZoomButtonComponents[i].SetInstanceBounds(mZoomButtons[i], true,
            0.0f, 0.0f, 1.0f, 1.0f);
        mZoomButtonComponents[i].SetPointerEnterCallback(enter);
        mZoomButtonComponents[i].SetPointerLeaveCallback(leave);
        mZoomButtonComponents[i].SetPointerPressCallback(press);
    }
    SetDoneButtonBounds(&mSaveButtonComponent, mSaveButton, 0);
    mSaveButtonComponent.SetPointerEnterCallback(saveEnter);
    mSaveButtonComponent.SetPointerLeaveCallback(saveLeave);
    mSaveButtonComponent.SetPointerPressCallback(savePress);
}

void OptionsVisualMenuV2::OnZoomLevelPointerEnter(int index, void* context)
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

void OptionsVisualMenuV2::OnZoomLevelPointerLeave(int index, void* context)
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

void OptionsVisualMenuV2::OnZoomLevelPointerPress(int index, void* context)
{
    int item = (int)context;
    if (mButtonComponents[item].HasOtherPointerState(2, -1))
        return;

    FEAudio::PlayAnimAudioEvent(0x3021A1EE, 0, 0, 1);
    mButtons[mSettings[1]]->SetActiveSlide("off", true, false);
    mButtonComponents[mSettings[1]].ResetPointerStates();
    mButtons[item]->SetActiveSlide("down", true, false);
    for (int i = 0; i < 4; ++i)
        mButtonComponents[item].SetPointerState(2, i);
    mSettings[1] = item;
    GameInfoManager::Instance()->mUserInfo.mVisualOptions.mCameraZoomLevel = item / 4.0;

    TLTextInstance* text = FEFinder<TLTextInstance, 4>::Find(
        mPresentation->m_currentSlide, "Layer", "visual_options", "SERIES SETTING");
    unsigned short number[4];
    nlSNPrintf(number, 4, (const unsigned short*)L"%d", mSettings[1] + 1);
    UpdateZoomLevelText(text, number, g_pLocalization->GetString("OPTIONS_VISUAL_ZOOMLEVEL"));
}

void OptionsVisualMenuV2::OnZoomModePointerEnter(int index, void* context)
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

void OptionsVisualMenuV2::OnZoomModePointerLeave(int index, void* context)
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

void OptionsVisualMenuV2::OnZoomModePointerPress(int index, void* context)
{
    unsigned int item = (unsigned int)context;
    if ((unsigned int)mSettings[0] == item
        || mZoomButtonComponents[item].HasOtherPointerState(2, -1))
        return;

    mZoomButtons[mSettings[0]]->SetActiveSlide("off", true, false);
    mZoomButtonComponents[mSettings[0]].ResetPointerStates();
    mZoomButtons[item]->SetActiveSlide("down", true, false);
    for (int i = 0; i < 4; ++i)
        mZoomButtonComponents[item].SetPointerState(2, i);
    FEAudio::PlayAnimAudioEvent(0x362F2841, 0, 0, 1);
    mSettings[0] = item;
    GameInfoManager::Instance()->mUserInfo.mVisualOptions.mIsAutoZoomCamera = item == 0;

    TLTextInstance* text = FEFinder<TLTextInstance, 4>::Find(
        mPresentation->m_currentSlide, "Layer", "visual_options", "SERIES SETTING");
    unsigned short number[4];
    nlSNPrintf(number, 4, (const unsigned short*)L"%d", mSettings[1] + 1);
    UpdateZoomLevelText(text, number, g_pLocalization->GetString("OPTIONS_VISUAL_ZOOMLEVEL"));
}

void OptionsVisualMenuV2::OnSaveButtonPointerEnter(int index, void*)
{
    mSaveButtonComponent.SetPointerState(1, index);
    if (!mSaveButtonComponent.HasOtherPointerState(1, index))
    {
        mSaveButton->SetActiveSlide("over", true, false);
        FEAudio::PlayAnimAudioEvent(0xAA73EF33, 0, 0, 1);
    }
}

void OptionsVisualMenuV2::OnSaveButtonPointerLeave(int index, void*)
{
    mSaveButtonComponent.SetPointerState(0, index);
    if (!mSaveButtonComponent.HasOtherPointerState(1, index))
    {
        mSaveButton->SetActiveSlide("off", true, false);
    }
}

void OptionsVisualMenuV2::OnSaveButtonPointerPress(int, void*)
{
    mState = 3;
    SHNavigation* navigation = GetNavigationScene();
    if (navigation != 0)
    {
        navigation->SetButtons(0, true);
    }
    mPresentation->SetActiveSlide("OPTIONS_OUT", true);
    mPresentation->Update(0.0f);
    mSaveButton->SetActiveSlide("down", true, false);
    FEAudio::PlayAnimAudioEvent(0xF0AFD586, 0, 0, 1);
    FEAudio::PlayAnimAudioEvent(0x304FDD1E, 0, 0, 1);
    mSaveStarted = true;
    SaveLoad::StartSave(false);
}
