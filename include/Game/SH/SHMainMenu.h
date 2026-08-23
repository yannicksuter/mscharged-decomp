#ifndef GAME_SH_SH_MAIN_MENU_H
#define GAME_SH_SH_MAIN_MENU_H

#include "Game/BaseSceneHandler.h"

class SHMainMenu : public BaseSceneHandler
{
public:
    SHMainMenu();
    virtual ~SHMainMenu();
    virtual void Update(float dt);
    virtual void SceneCreated();

    static const u32 NUM_ITEMS = 7;

    /* 0x01C */ u8 mUnknown1C[0x90];
    /* 0x0AC */ u8 mMenuItems[NUM_ITEMS][0xB4];
    /* 0x598 */ u8 mUnknown598[0xF0];
}; // size 0x688

#endif // GAME_SH_SH_MAIN_MENU_H
