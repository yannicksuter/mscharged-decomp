#ifndef GAME_SH_SH_LOADING_H
#define GAME_SH_SH_LOADING_H

#include "Game/BaseSceneHandler.h"

class TLComponentInstance;
class TLTextInstance;

class SuperLoadingScene : public BaseSceneHandler
{
public:
    enum TransitionType
    {
        TT_INVALID = -1,
        TT_IN = 0,
        TT_OUT = 1,
        TT_3D_TRANSITION = 2,
    };

    SuperLoadingScene();
    virtual ~SuperLoadingScene();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();

    /* 0x1C */ TransitionType mType;
}; // size 0x20

class LoadingScene_801CDB4C : public BaseSceneHandler
{
public:
    LoadingScene_801CDB4C();
    virtual ~LoadingScene_801CDB4C();
    virtual void Update(float dt);
    virtual void SceneCreated();
    virtual void fn_801CE274();

    /* 0x1C */ TLComponentInstance* mTransitionComponent;
    /* 0x20 */ bool mTransitionActive;
    /* 0x21 */ bool mWidescreen;
    /* 0x22 */ unsigned char mPadding22[2];
}; // size 0x24

class LoadingScene_801CDC2C : public LoadingScene_801CDB4C
{
public:
    LoadingScene_801CDC2C();
    virtual ~LoadingScene_801CDC2C();
    virtual void Update(float dt);
    virtual void SceneCreated();

    /* 0x024 */ TLTextInstance* mTextInstances[6];
    /* 0x03C */ unsigned short mTextBuffers[5][128];
}; // size 0x53C

#endif // GAME_SH_SH_LOADING_H
