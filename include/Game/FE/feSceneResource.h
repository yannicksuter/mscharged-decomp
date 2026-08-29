#ifndef GAME_FE_SCENE_RESOURCE_H
#define GAME_FE_SCENE_RESOURCE_H

#include "Game/FE/feResourceManager.h"

class FEScene;

class FESceneResource : public FEResourceHandle
{
public:
    FESceneResource();

    /* 0x18 */ FEScene* m_pFESceneContext;
    /* 0x1C */ u32 field_0x1C;
}; // size 0x20

#endif // GAME_FE_SCENE_RESOURCE_H
