#include "Game/FE/Overlay/OverlayHandlerDefensivePlay.h"
#include "Game/Audio/UnidentifiedRegistryPools.h"

#include "Game/AI/Fielder.h"
#include "Game/DB/CharacterInfo.h"
#include "Game/FE/feFinder.inl"
#include "Game/FE/fePresentation.h"
#include "Game/FE/tlSlide.h"
#include "Game/Sys/audio.h"
#include "Game/Team.h"
#include "NL/nlPrint.h"

DefensivePlayOverlay::DefensivePlayOverlay()
    : BaseOverlayHandler(2, POSITION_ALL)
    , mCountdownStarted(false)
    , mCountdownComplete(false)
    , mPlayerIndex(-1)
    , mCountdownSoundCount(0)
    , mCountdownSoundTimer(0.0f)
    , mCountdownSoundInterval(0.0f)
    , mCountdownSpeed(0)
{
}

DefensivePlayOverlay::~DefensivePlayOverlay()
{
}

void DefensivePlayOverlay::Update(float fDeltaT)
{
    BaseSceneHandler::Update(fDeltaT);

    if (mCountdownStarted)
    {
        TLSlide* countdownSlide = mPresentation->m_currentSlide;
        if (countdownSlide->GetCurrentTime() >= countdownSlide->GetStartTime() + countdownSlide->GetDuration())
        {
            mCountdownComplete = true;
        }

        mCountdownSoundTimer += fDeltaT;
        if (mCountdownSoundTimer >= mCountdownSoundInterval && mCountdownSoundCount < 4)
        {
            PlaySound(15, 0x97E84AE4, 0, 0);
            ++mCountdownSoundCount;
            mCountdownSoundTimer = 0.0f;
        }
    }
}

void DefensivePlayOverlay::SceneCreated()
{
    TLComponentInstance* playersComponent = FEFinder<TLComponentInstance, TLAT_COMPONENT>::FindOrDefault(
        mPresentation, "STEP_1", "Layer", "PLAYERS");
    if (mPlayerIndex < 0)
    {
        playersComponent->m_bVisible = false;
    }
    else
    {
        char playerSlideName[4];
        nlSNPrintf(playerSlideName, sizeof(playerSlideName), "P%d", mPlayerIndex + 1);
        playersComponent->SetActiveSlide(playerSlideName, true, false);
    }

    TLTextInstance* playerText = FEFinder<TLTextInstance, TLAT_TEXT>::FindOrDefault(playersComponent->GetActiveSlide(), "PLAYER");
    cFielder* defendingCaptain = mGoalie->GetTeam()->GetCaptain();
    cFielder* attackingCaptain = mGoalie->GetTeam()->GetOtherTeam()->GetCaptain();
    nlColour teamColour = GetTeamColour(*defendingCaptain->mUnidentified11C, *attackingCaptain->mUnidentified11C, true);
    playerText->SetAssetColour(teamColour);
}

void DefensivePlayOverlay::StartCountdown()
{
    switch (mCountdownSpeed)
    {
    case 2:
        mPresentation->SetActiveSlide("COUNTDOWN3", true);
        mCountdownSoundInterval = 0.3f;
        break;
    case 1:
        mPresentation->SetActiveSlide("COUNTDOWN2", true);
        mCountdownSoundInterval = 0.5f;
        break;
    default:
        mPresentation->SetActiveSlide("COUNTDOWN", true);
        mCountdownSoundInterval = 0.6f;
        break;
    }

    mCountdownStarted = true;
    mCountdownSoundTimer = 0.0f;
    PlaySound(15, 0x97E84AE4, 0, 0);
    mCountdownSoundCount = 1;
}
