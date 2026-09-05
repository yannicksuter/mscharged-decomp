#include "unclassified/tu_80245F04.h"

#include "Game/BaseGameSceneManager.h"
#include "Game/FE/tlInstance.h"
#include "unclassified/tu_80252180.h"

extern BaseGameSceneManager* lbl_806E1838;

extern "C" void fn_801CBCA0(unsigned long hash, int value0, int value1, int value2);

/**
 * Offset/Address/Size: 0x344 | 0x80246248 | size: 0x80
 */
TU80245F04Scene::~TU80245F04Scene()
{
    TU80252180Scene* scene = fn_80253E18();
    if (scene != 0)
    {
        scene->mUnidentified054->m_bVisible = false;
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
