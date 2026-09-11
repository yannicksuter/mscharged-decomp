#include "Game/OverlayHandlerHUD.h"

#include "Game/FE/fePresentation.h"
#include "Game/Team.h"

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

void HUDOverlay::ResetScores()
{
    mUnidentified1A8.fn_801E99F0();
}

void HUDOverlay::UpdateScore()
{
    mUnidentified1A8.mNewScore[0] = g_pTeams[0]->m_nScore;
    mUnidentified1A8.mNewScore[1] = g_pTeams[1]->m_nScore;
}

void HUDOverlay::DisplayNewScore()
{
    for (int team = 0; team < 2; team++)
    {
        for (int flare = 0; flare < 2; flare++)
        {
            if (mNumFlareCycles[team][flare] != -1)
            {
                mNumFlareCycles[team][flare] = 20;
            }
        }
    }
}
