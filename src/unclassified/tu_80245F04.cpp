#include "unclassified/tu_80245F04.h"
#include "Game/FE/tlTextInstance.h"
#include "Game/FE/FEAudio.h"

#include "Game/BaseGameSceneManager.h"
#include "Game/FE/tlInstance.h"
#include "Game/SH/SHNavigation.h"
#include "Game/FE/FEAudio.h"

extern BaseGameSceneManager* g_pGameSceneManager;


/**
 * Offset/Address/Size: 0x344 | 0x80246248 | size: 0x80
 */
TU80245F04Scene::~TU80245F04Scene()
{
    SHNavigation* scene = GetNavigationScene();
    if (scene != 0)
    {
        scene->mTimer->m_bVisible = false;
    }
}

/**
 * Offset/Address/Size: 0x7BC | 0x802466C0 | size: 0x5C
 */
void TU80245F04Scene::fn_802466C0()
{
    mUnidentified1FC = false;
    FEAudio::PlayAnimAudioEvent(0x37A9934D, 0, 0, 1);
    g_pGameSceneManager->Push((SceneList)40, SCREEN_BACK, true);
}
