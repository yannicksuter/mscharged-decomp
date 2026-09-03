#include "unclassified/tu_80245F04.h"

#include "Game/BaseGameSceneManager.h"

struct TU80253E18Nested
{
    /* 0x00 */ u8 mUnidentified00[0x8E];
    /* 0x8E */ bool mUnidentified8E;
};

struct TU80253E18Object
{
    /* 0x00 */ u8 mUnidentified00[0x54];
    /* 0x54 */ TU80253E18Nested* mUnidentified54;
};

extern BaseGameSceneManager* lbl_806E1838;

extern "C" void fn_801CBCA0(unsigned long hash, int value0, int value1, int value2);
extern "C" TU80253E18Object* fn_80253E18();

/**
 * Offset/Address/Size: 0x344 | 0x80246248 | size: 0x80
 */
TU80245F04Scene::~TU80245F04Scene()
{
    TU80253E18Object* object = fn_80253E18();
    if (object != 0)
    {
        object->mUnidentified54->mUnidentified8E = false;
    }
}

/**
 * Offset/Address/Size: 0x7BC | 0x802466C0 | size: 0x5C
 */
void TU80245F04Scene::fn_802466C0()
{
    mUnidentified1FC = false;
    fn_801CBCA0(0x37A9934D, 0, 0, 1);
    lbl_806E1838->Push((SceneList)40, SCREEN_BACK, true);
}
