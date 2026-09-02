#include "Game/OverlayHandlerHUD.h"

#include "Game/FE/fePresentation.h"

static const char* HUD_SLIDE_IN_NAME = "Slide1";
static const char* HUD_SLIDE_OUT_NAME = "out";

void HUDOverlay::SwapPowerUps(int homeAway)
{
}

void HUDOverlay::SetSlideOut()
{
    mPresentation->SetActiveSlide(HUD_SLIDE_OUT_NAME, true);
}

void HUDOverlay::SetSlideIn()
{
    mPresentation->SetActiveSlide(HUD_SLIDE_IN_NAME, true);
}
