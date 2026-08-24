#ifndef GAME_BASE_SCENE_HANDLER_H
#define GAME_BASE_SCENE_HANDLER_H

#include "types.h"

class FEPresentation;
class FEScene;
class TLInstance;

class BaseScreenHandler
{
public:
    virtual ~BaseScreenHandler() = 0;
    virtual u32 GetType() = 0;
    virtual void Update() = 0;
    virtual void OnActivate() = 0;

    /* 0x04 */ BaseScreenHandler* mNext;
    /* 0x08 */ BaseScreenHandler* mPrev;
    /* 0x0C */ TLInstance* mTLInstance;
    /* 0x10 */ FEScene* mFEScene;
}; // size 0x14

class BaseSceneHandler
{
public:
    BaseSceneHandler()
        : mVisible(true)
        , mScreenHandlerList(0)
        , mActiveScreenHandler(0)
        , mPresentation(0)
        , mFEScene(0)
    {
    }

    virtual ~BaseSceneHandler() { }
    virtual void Update(float dt);
    virtual void InitializeSubHandlers() { }
    virtual void AddScreenHandler(BaseScreenHandler* handler);
    virtual void RemoveScreenHandler(BaseScreenHandler* handler);
    virtual void SetPresentation(FEPresentation* presentation) { mPresentation = presentation; }
    virtual void OnActivate();
    virtual void SceneCreated() { }
    virtual void SetVisible(bool visible) { mVisible = visible; }

    /* 0x04 */ u32 mHashID;
    /* 0x08 */ bool mVisible;
    /* 0x0C */ BaseScreenHandler* mScreenHandlerList;
    /* 0x10 */ BaseScreenHandler* mActiveScreenHandler;
    /* 0x14 */ FEPresentation* mPresentation;
    /* 0x18 */ FEScene* mFEScene;
}; // size 0x1C

#endif // GAME_BASE_SCENE_HANDLER_H
