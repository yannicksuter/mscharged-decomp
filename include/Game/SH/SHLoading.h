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

class AsyncLoadingScene : public BaseSceneHandler
{
public:
    AsyncLoadingScene();
    virtual ~AsyncLoadingScene();
    virtual void Update(float dt);
    virtual void SceneCreated();
    virtual void ShowHomeButtonWarning();

    /* 0x1C */ TLComponentInstance* mHomeButtonWarning;
    /* 0x20 */ bool mHomeButtonWarningActive;
    /* 0x21 */ bool mWidescreen;
    /* 0x22 */ unsigned char mPadding22[2];
}; // size 0x24

class WidescreenLoadingScene : public AsyncLoadingScene
{
public:
    WidescreenLoadingScene();
    virtual ~WidescreenLoadingScene();
    virtual void Update(float dt);
    virtual void SceneCreated();

    void SetStadiumText(TLTextInstance* stadiumText);

    /* 0x024 */ TLTextInstance* mTextInstances[6];
    /* 0x03C */ unsigned short mTextBuffers[5][128];
}; // size 0x53C

#endif // GAME_SH_SH_LOADING_H
