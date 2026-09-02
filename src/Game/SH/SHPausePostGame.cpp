#include "Game/SH/SHPausePostGame.h"

#include "Game/GameInfo.h"

extern "C" void fn_801C48CC();
extern "C" void fn_80253284(int value);

void PausePostGameScene::OnSelectChangeTeams()
{
    GameInfoManager::s_pInstance->unknown_0x71C8 = 2;
    fn_801C48CC();
    fn_80253284(0);
}

void PausePostGameScene::OnSelectQuit()
{
    fn_801C48CC();
    fn_80253284(0);
}
