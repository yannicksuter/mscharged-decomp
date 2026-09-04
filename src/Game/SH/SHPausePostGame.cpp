#include "Game/SH/SHPausePostGame.h"

#include "Game/FE/feManager.h"
#include "Game/GameInfo.h"

extern "C" void fn_80253284(int value);

void PausePostGameScene::OnSelectChangeTeams()
{
    GameInfoManager::s_pInstance->unknown_0x71C8 = 2;
    FrontEnd::ReturnToFE();
    fn_80253284(0);
}

void PausePostGameScene::OnSelectQuit()
{
    FrontEnd::ReturnToFE();
    fn_80253284(0);
}
