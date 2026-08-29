#include "Game/BaseSceneHandler.h"

#include "Game/FE/feScene.h"

#include "NL/nlDLRing.h"

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
