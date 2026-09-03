#ifndef GAME_SH_SH_NETWORK_START_H
#define GAME_SH_SH_NETWORK_START_H

#include "Game/BaseSceneHandler.h"

class TLComponentInstance;

class UnidentifiedNetworkStartScene : public BaseSceneHandler
{
public:
    void fn_801FC680(int state);
    void fn_801FDEB8(TLComponentInstance* component);
    void fn_801FDF00(TLComponentInstance* component);
};

#endif // GAME_SH_SH_NETWORK_START_H
