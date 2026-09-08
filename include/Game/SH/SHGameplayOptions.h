#ifndef GAME_SH_SHGAMEPLAYOPTIONS_H
#define GAME_SH_SHGAMEPLAYOPTIONS_H

#include "Game/BaseSceneHandler.h"
#include "Game/DB/UserOptions.h"
#include "Game/FE/feBackButton.h"

struct FEPageControls;
class TLComponentInstance;

class SHGameplayOptions : public BaseSceneHandler
{
public:
    SHGameplayOptions();
    virtual ~SHGameplayOptions();
    virtual void SceneCreated();
    virtual void Update(float dt);
    void fn_80235928();
    void fn_80235CE4(bool value);
    void fn_80235FE0();
    void fn_802365F0(int item);
    void fn_80236ADC(int type, int value);
    void fn_80236E54();
    void fn_802378F8(int index, void* context);
    void fn_802379D0(int index, void* context);
    void fn_80237A80(int index, void* context);
    void fn_80237C7C(int index, void* context);
    void fn_80237D34(int index, void* context);
    void fn_80237DE0(int index, void* context);
    void fn_80237E70(int index, void* context);
    void fn_80237EF8(int index, void* context);
    void fn_80237F68(int index, void* context);
    void fn_80238050();
    void UpdateCheatText();

    /* 0x001C */ FEPointerButton mOptionButtons[24];
    /* 0x10FC */ FEPointerButton mCheatButtons[3];
    /* 0x1318 */ TLComponentInstance* mOptionInstances[24];
    /* 0x1378 */ TLComponentInstance* mCheatInstances[3];
    /* 0x1384 */ TLComponentInstance* mDoneButtonInstance;
    /* 0x1388 */ TLInstance* mUnidentified1388;
    /* 0x138C */ TLInstance* mUnidentified138C;
    /* 0x1390 */ TLInstance* mUnidentified1390;
    /* 0x1394 */ TLInstance* mUnidentified1394;
    /* 0x1398 */ FEBackButton mNavigation;
    /* 0x1470 */ FEPointerButton mDoneButton;
    /* 0x1524 */ FEPageControls* mPageControls;
    /* 0x1528 */ int mPointerInsideCounts[4];
    /* 0x1538 */ unsigned short mUnidentified1538[32];
    /* 0x1578 */ unsigned short mUnidentified1578[32];
    /* 0x15B8 */ int mUnidentified15B8;
    /* 0x15BC */ bool mUnidentified15BC;
    /* 0x15BD */ bool mUnidentified15BD;
    /* 0x15C0 */ GameplaySettings mSettings;
    /* 0x15DC */ PowerupSettings mPowerupSettings;
    /* 0x15E8 */ int mUnidentified15E8;
    /* 0x15EC */ int mUnidentified15EC;
    /* 0x15F0 */ int mUnidentified15F0;
    /* 0x15F4 */ int mUnidentified15F4;
    /* 0x15F8 */ int mUnidentified15F8;
    /* 0x15FC */ int mUnidentified15FC;
}; // size 0x1600

#endif // GAME_SH_SHGAMEPLAYOPTIONS_H
