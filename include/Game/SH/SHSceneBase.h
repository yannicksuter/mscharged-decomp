#ifndef GAME_SH_SH_SCENE_BASE_H
#define GAME_SH_SH_SCENE_BASE_H

#include "Game/BaseSceneHandler.h"

class UnidentifiedSHSceneBase : public BaseSceneHandler
{
public:
    UnidentifiedSHSceneBase();
    virtual ~UnidentifiedSHSceneBase();
    virtual void Update(float dt);
    virtual void SceneCreated();
    virtual void SHSceneVirtual2C();
    virtual void SHSceneVirtual30();
    virtual void SHSceneVirtual34() { }
    virtual void SHSceneVirtual38();

    /* 0x01C */ u8 mUnidentified1C[0x5D4 - 0x1C];
}; // size 0x5D4

#endif // GAME_SH_SH_SCENE_BASE_H
