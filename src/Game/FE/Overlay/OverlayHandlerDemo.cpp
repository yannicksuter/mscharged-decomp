#include "Game/OverlayHandlerDemo.h"
#include "Game/BaseSceneHandler.h"

DemoOverlay::DemoOverlay()
    : BaseOverlayHandler(-1)
{
}

DemoOverlay::~DemoOverlay()
{
}

void DemoOverlay::Update(float fDeltaT)
{
    BaseSceneHandler::Update(fDeltaT);
}

void DemoOverlay::SceneCreated()
{
    this->SetVisible(false);
}
