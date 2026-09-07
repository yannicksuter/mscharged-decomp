#include "Game/SH/SHPausePostGame.h"

#include "Game/FE/feManager.h"
#include "Game/GameInfo.h"

extern "C" void SetPointerEnabled(int value);

void PausePostGameScene::OnSelectChangeTeams()
{
    GameInfoManager::s_pInstance->unknown_0x71C8 = 2;
    FrontEnd::ReturnToFE();
    SetPointerEnabled(0);
}

void PausePostGameScene::OnSelectQuit()
{
    FrontEnd::ReturnToFE();
    SetPointerEnabled(0);
}
