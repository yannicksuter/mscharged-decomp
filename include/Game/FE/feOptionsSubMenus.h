#ifndef _FEOPTIONSSUBMENUS_H_
#define _FEOPTIONSSUBMENUS_H_

#include "Game/BaseSceneHandler.h"
#include "unclassified/tu_80219248.h"
#include "unclassified/tu_8022EF84.h"

class TLComponentInstance;
class TLInstance;

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

    /* 0x01C */ int mUnidentified1C;
    /* 0x020 */ int mUnidentified20;
    /* 0x024 */ bool mUnidentified24;
    /* 0x025 */ u8 mPadding25[3];
}; // size 0x28

class OptionsAudioMenuV2 : public OptionsSubMenu
{
public:
    OptionsAudioMenuV2(int value);
    virtual ~OptionsAudioMenuV2();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();

    void fn_801D474C();
    void fn_801D4E9C(int setting);
    void fn_801D4F70(int index, void* context);
    void fn_801D5108(int index, void* context);
    void fn_801D5278(int index, void* context);
    void fn_801D575C(int index);
    void fn_801D57D8(int index);
    void fn_801D583C();
    void fn_801D58EC(int setting);

    /* 0x028 */ int mUnidentified28;
    /* 0x02C */ TU8022EF84Component mNavigation;
    /* 0x104 */ TLComponentInstance* mButtons[6];
    /* 0x11C */ TLComponentInstance* mSaveButton;
    /* 0x120 */ TLInstance* mVolumeBars[3][10];
    /* 0x198 */ TU80219248Component mButtonComponents[6];
    /* 0x5D0 */ TU80219248Component mSaveButtonComponent;
    /* 0x684 */ bool mUnidentified684;
    /* 0x685 */ bool mUnidentified685;
    /* 0x686 */ bool mUnidentified686;
    /* 0x687 */ u8 mPadding687;
    /* 0x688 */ int mSettings[3];
    /* 0x694 */ int mBackupSettings[3];
    /* 0x6A0 */ u8 mUnidentified6A0[0x60];
    /* 0x700 */ int mState;
}; // size 0x704

class OptionsVisualMenuV2 : public OptionsSubMenu
{
public:
    OptionsVisualMenuV2(int value);
    virtual ~OptionsVisualMenuV2();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();

    void fn_801D6E80();
    void fn_801D7948(int index, void* context);
    void fn_801D7A0C(int index, void* context);
    void fn_801D7AA8(int index, void* context);
    void fn_801D7EC8(int index, void* context);
    void fn_801D7F9C(int index, void* context);
    void fn_801D8048(int index, void* context);
    void fn_801D8458(int index);
    void fn_801D84D4(int index);
    void fn_801D8538();

    /* 0x028 */ int mUnidentified28;
    /* 0x02C */ TU8022EF84Component mNavigation;
    /* 0x104 */ TLComponentInstance* mButtons[5];
    /* 0x118 */ TLComponentInstance* mZoomButtons[2];
    /* 0x120 */ TLComponentInstance* mSaveButton;
    /* 0x124 */ TU80219248Component mButtonComponents[5];
    /* 0x4A8 */ TU80219248Component mZoomButtonComponents[2];
    /* 0x610 */ TU80219248Component mSaveButtonComponent;
    /* 0x6C4 */ bool mUnidentified6C4;
    /* 0x6C5 */ bool mUnidentified6C5;
    /* 0x6C6 */ bool mUnidentified6C6;
    /* 0x6C7 */ u8 mPadding6C7;
    /* 0x6C8 */ int mSettings[2];
    /* 0x6D0 */ u8 mUnidentified6D0[0x20];
    /* 0x6F0 */ int mState;
    /* 0x6F4 */ int mBackupSettings[2];
}; // size 0x6FC

#endif // _FEOPTIONSSUBMENUS_H_
