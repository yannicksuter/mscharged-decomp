#include "Game/BaseSceneHandler.h"

#include "NL/nlDLRing.h"

class FEScene
{
public:
    void Update(float dt);
};

void BaseSceneHandler::Update(float dt)
{
    mFEScene->Update(dt);
}

void BaseSceneHandler::AddScreenHandler(BaseScreenHandler* handler)
{
    handler->mFEScene = mFEScene;
    nlDLRingAddEnd(&mScreenHandlerList, handler);
}

void BaseSceneHandler::RemoveScreenHandler(BaseScreenHandler*)
{
}

void BaseSceneHandler::OnActivate()
{
    if (mActiveScreenHandler != 0)
    {
        mActiveScreenHandler->OnActivate();
    }
}
