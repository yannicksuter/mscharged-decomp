#ifndef _FEOPTIONSSUBMENUS_H_
#define _FEOPTIONSSUBMENUS_H_

#include "Game/BaseSceneHandler.h"
#include "Game/FE/fePointerButton.h"
#include "Game/FE/feBackButton.h"

class TLComponentInstance;
class TLInstance;
class TLTextInstance;

class OptionsSubMenu : public BaseSceneHandler
{
public:
    OptionsSubMenu()
        : mUnidentified1C(12)
        , mUnidentified20(1)
        , mUnidentified24(false)
    {
    }
    virtual ~OptionsSubMenu() { }
    int fn_801CAA10() const { return mUnidentified20; }

    /* 0x01C */ int mUnidentified1C;
    /* 0x020 */ int mUnidentified20;
    /* 0x024 */ bool mUnidentified24;
    /* 0x025 */ u8 mPadding25[3];
}; // size 0x28

class OptionsAudioMenuV2 : public OptionsSubMenu
{
public:
    OptionsAudioMenuV2(int mode);
    virtual ~OptionsAudioMenuV2();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();

    void InitializePointerButtons();
    void UpdateVolumeBars(int setting);
    void OnVolumeButtonPointerEnter(int index, void* context);
    void OnVolumeButtonPointerLeave(int index, void* context);
    void OnVolumeButtonPointerPress(int index, void* context);
    void OnSaveButtonPointerEnter(int index, void* context);
    void OnSaveButtonPointerLeave(int index, void* context);
    void OnSaveButtonPointerPress(int index, void* context);
    void UpdateVolumeLevelText(int setting);
    void MarkUnusedVolumeButtons();

    bool IsVolumeButtonEnabled(unsigned int item) const
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
        return enabled;
    }

    /* 0x028 */ int mOverlayMode;
    /* 0x02C */ FEBackButton mNavigation;
    /* 0x104 */ TLComponentInstance* mButtons[6];
    /* 0x11C */ TLComponentInstance* mSaveButton;
    /* 0x120 */ TLInstance* mVolumeBars[3][10];
    /* 0x198 */ FEPointerButton mButtonComponents[6];
    /* 0x5D0 */ FEPointerButton mSaveButtonComponent;
    /* 0x684 */ bool mPointerButtonsInitialized;
    /* 0x685 */ bool mIntroSoundPlayed;
    /* 0x686 */ bool mSaveStarted;
    /* 0x687 */ u8 mPadding687;
    /* 0x688 */ int mSettings[3];
    /* 0x694 */ int mBackupSettings[3];
    /* 0x6A0 */ u16 mFormattedSettings[3][16];
    /* 0x700 */ int mState;
}; // size 0x704

class OptionsVisualMenuV2 : public OptionsSubMenu
{
public:
    OptionsVisualMenuV2(int mode);
    virtual ~OptionsVisualMenuV2();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();

    void InitializePointerButtons();
    void UpdateZoomLevelText(TLTextInstance* text, const u16 (&number)[4], const u16* localized);
    void OnZoomLevelPointerEnter(int index, void* context);
    void OnZoomLevelPointerLeave(int index, void* context);
    void OnZoomLevelPointerPress(int index, void* context);
    void OnZoomModePointerEnter(int index, void* context);
    void OnZoomModePointerLeave(int index, void* context);
    void OnZoomModePointerPress(int index, void* context);
    void OnSaveButtonPointerEnter(int index, void* context);
    void OnSaveButtonPointerLeave(int index, void* context);
    void OnSaveButtonPointerPress(int index, void* context);

    /* 0x028 */ int mOverlayMode;
    /* 0x02C */ FEBackButton mNavigation;
    /* 0x104 */ TLComponentInstance* mButtons[5];
    /* 0x118 */ TLComponentInstance* mZoomButtons[2];
    /* 0x120 */ TLComponentInstance* mSaveButton;
    /* 0x124 */ FEPointerButton mButtonComponents[5];
    /* 0x4A8 */ FEPointerButton mZoomButtonComponents[2];
    /* 0x610 */ FEPointerButton mSaveButtonComponent;
    /* 0x6C4 */ bool mPointerButtonsInitialized;
    /* 0x6C5 */ bool mIntroSoundPlayed;
    /* 0x6C6 */ bool mSaveStarted;
    /* 0x6C7 */ u8 mPadding6C7;
    /* 0x6C8 */ int mSettings[2];
    /* 0x6D0 */ u16 mFormattedZoomLevel[16];
    /* 0x6F0 */ int mState;
    /* 0x6F4 */ int mBackupSettings[2];
}; // size 0x6FC

class Class_801D87C4 : public BaseSceneHandler
{
public:
    Class_801D87C4();
    virtual ~Class_801D87C4();
    virtual void Update(float dt);
    virtual void SceneCreated();

    /* 0x1C */ int mScreenCount;
    /* 0x20 */ int mCurrentScreen;
    /* 0x24 */ TLInstance** mScreens;
    /* 0x28 */ TLInstance* mBackground;
    /* 0x2C */ TLComponentInstance* mHomeMessage;
    /* 0x30 */ bool mWidescreen;
    /* 0x31 */ u8 mPadding31[3];
    /* 0x34 */ float mScreenTime;
    /* 0x38 */ float mFadeAlpha;
    /* 0x3C */ int mState;
    /* 0x40 */ float mEndTime;
}; // size 0x44

#endif // _FEOPTIONSSUBMENUS_H_
