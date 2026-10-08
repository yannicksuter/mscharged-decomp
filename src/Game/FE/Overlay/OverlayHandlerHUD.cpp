#include "NL/nlDLListContainer.inl"
#include "Game/OverlayHandlerHUD.h"
#include "Game/AI/Fielder.h"

#include "Game/DB/GameProgress.h"
#include "Game/DB/CharacterInfo.h"
#include "Game/DB/StatsTracker.h"
#include "Game/FE/feHelpFuncs.h"
#include "Game/FE/feAsyncImage.h"
#include "Game/FE/feInlineHasher.h"
#include "Game/FE/feFinder_impl.h"
#include "Game/FE/fePackage.h"
#include "Game/FE/fePresentation.h"
#include "Game/FE/feScene.h"
#include "Game/GameInfo.h"
#include "Game/Game.h"
#include "Game/Task/FixedUpdateTask.h"
#include "Game/Render/RLViewLayers.h"
#include "Game/Team.h"
#include "NL/nlMemory.h"
#include "NL/nlFormat.h"
#include "NL/nlLocalizationLookup.h"
#include <string.h>
#include "NL/nlPrint.h"
#include "NL/nlString.h"


static unsigned char g_hudVisible = 1;
static const char* POWER_UP_IMAGE_NAMES[2][2] = { { "left_powerup1", "left_powerup2" }, { "right_powerup1", "right_powerup2" } };
static const char* HUD_TEAM_NAMES[2] = { "left_team_hud", "right_team_hud" };
static const char* HUD_NAMES[2] = { "left hud", "right hud" };
static const char* PAD_NAMES[2] = { "pad_1", "pad_2" };
static const char* POWER_UP_TEXT_NAMES[2][2] = { { "POWERUP NUMBER LEFT 1", "POWERUP NUMBER LEFT 2" }, { "POWERUP NUMBER RIGHT 1", "POWERUP NUMBER RIGHT 2" } };
static const char* POWERBAR_NAMES[2] = { "powerbar_left", "powerbar_right" };
static const char* POWERBAR_CONTAINER_NAMES[2] = { "powerbar_left_container", "powerbar_right_container" };
static const char* HUD_SLIDE_IN_NAME = "Slide1";
static const char* HUD_SLIDE_OUT_NAME = "out";
static const char* ART_SLIDE_NAME = "art";
static const char* LAYER_NAME = "Layer";
static unsigned long HUD_SLIDE_IN_HASH = nlStringLowerHash(HUD_SLIDE_IN_NAME);
static unsigned long HUD_SLIDE_OUT_HASH = nlStringLowerHash(HUD_SLIDE_OUT_NAME);

HUDOverlay::HUDOverlay()
    : BaseOverlayHandler(2)
{
}

HUDOverlay::~HUDOverlay()
{
    delete mAsyncImage[0];
    delete mAsyncImage[1];
}

void HUDOverlay::Update(float fDeltaT)
{
    BaseSceneHandler::Update(fDeltaT);
    mAsyncImage[0]->Update(true);
    mAsyncImage[1]->Update(true);
    if (!g_hudVisible)
    {
        SetVisible(false);
    }
    mScoreDisplay.Update(fDeltaT);
    mCaptainMeter.Update(fDeltaT);
    mPowerUpDisplay.DisplayPowerUps(fDeltaT);
    if (nlSingleton<GameInfoManager>::Instance()->GetCurrentSettings()->GameLimitType == 0)
    {
        mClock.Update(fDeltaT);
    }
}

void HUDOverlay::SceneCreated()
{
    FEPresentation* presentation = mFEScene->m_pFEPackage->GetPresentation();
    TLComponentInstance* leftIn = FEFinder<TLComponentInstance, 4>::Find<FEPresentation>(
        presentation, HUD_SLIDE_IN_NAME, LAYER_NAME, HUD_TEAM_NAMES[0]);
    TLComponentInstance* rightIn = FEFinder<TLComponentInstance, 4>::Find<FEPresentation>(
        presentation, HUD_SLIDE_IN_NAME, LAYER_NAME, HUD_TEAM_NAMES[1]);
    TLComponentInstance* leftOut = FEFinder<TLComponentInstance, 4>::Find<FEPresentation>(
        presentation, HUD_SLIDE_OUT_NAME, LAYER_NAME, HUD_TEAM_NAMES[0]);
    TLComponentInstance* rightOut = FEFinder<TLComponentInstance, 4>::Find<FEPresentation>(
        presentation, HUD_SLIDE_OUT_NAME, LAYER_NAME, HUD_TEAM_NAMES[1]);
    if (IsWidescreen())
    {
        leftIn->SetActiveSlide("16:9", true, false);
        rightIn->SetActiveSlide("16:9", true, false);
        leftOut->SetActiveSlide("16:9", true, false);
        rightOut->SetActiveSlide("16:9", true, false);
    }
    else
    {
        leftIn->SetActiveSlide("4:3", true, false);
        rightIn->SetActiveSlide("4:3", true, false);
        leftOut->SetActiveSlide("4:3", true, false);
        rightOut->SetActiveSlide("4:3", true, false);
    }
    mClock.Init(presentation);
    mScoreDisplay.Init(presentation);
    mCaptainMeter.Init(presentation);
    mPowerUpTextures.LoadHUDTextures(presentation);
    mPowerUpDisplay.Init(presentation, &mPowerUpTextures);
    SetTeamIcons();
    if (nlSingleton<GameInfoManager>::Instance()->mIsInStrikers101Mode)
    {
        mClock.m_pTextInstanceClock[0]->m_bVisible = false;
        mClock.m_pTextInstanceClock[1]->m_bVisible = false;
    }
    mPresentation->SetActiveSlide(HUD_SLIDE_OUT_NAME, true);
    mScoreDisplay.ResetScores();
}

void HUDOverlay::SetSlideIn()
{
    mPresentation->SetActiveSlide(HUD_SLIDE_IN_NAME, true);
}

void HUDOverlay::SetSlideOut()
{
    mPresentation->SetActiveSlide(HUD_SLIDE_OUT_NAME, true);
}

void HUDPowerUpTextures::LoadHUDTextures(FEPresentation* presentation)
{
    TLImageInstance* pImageInstance;

    pImageInstance = FEFinder<TLImageInstance, 2>::Find<FEPresentation>(
        presentation, ART_SLIDE_NAME, "Layer", "star");
    pImageInstance->m_bVisible = false;
    m_pStar = pImageInstance->m_pTextureResource;

    pImageInstance = FEFinder<TLImageInstance, 2>::Find<FEPresentation>(
        presentation, ART_SLIDE_NAME, "Layer", "mega");
    pImageInstance->m_bVisible = false;
    m_pMega = pImageInstance->m_pTextureResource;

    pImageInstance = FEFinder<TLImageInstance, 2>::Find<FEPresentation>(
        presentation, ART_SLIDE_NAME, "Layer", "shell_green");
    pImageInstance->m_bVisible = false;
    m_pShellGreen = pImageInstance->m_pTextureResource;

    pImageInstance = FEFinder<TLImageInstance, 2>::Find<FEPresentation>(
        presentation, ART_SLIDE_NAME, "Layer", "shell_red");
    pImageInstance->m_bVisible = false;
    m_pShellRed = pImageInstance->m_pTextureResource;

    pImageInstance = FEFinder<TLImageInstance, 2>::Find<FEPresentation>(
        presentation, ART_SLIDE_NAME, "Layer", "banana");
    pImageInstance->m_bVisible = false;
    m_pBanana = pImageInstance->m_pTextureResource;

    pImageInstance = FEFinder<TLImageInstance, 2>::Find<FEPresentation>(
        presentation, ART_SLIDE_NAME, "Layer", "mushroom");
    pImageInstance->m_bVisible = false;
    m_pMushroom = pImageInstance->m_pTextureResource;

    pImageInstance = FEFinder<TLImageInstance, 2>::Find<FEPresentation>(
        presentation, ART_SLIDE_NAME, "Layer", "shell_blue");
    pImageInstance->m_bVisible = false;
    m_pShellBlue = pImageInstance->m_pTextureResource;

    pImageInstance = FEFinder<TLImageInstance, 2>::Find<FEPresentation>(
        presentation, ART_SLIDE_NAME, "Layer", "shell_spike");
    pImageInstance->m_bVisible = false;
    m_pShellSpike = pImageInstance->m_pTextureResource;

    pImageInstance = FEFinder<TLImageInstance, 2>::Find<FEPresentation>(
        presentation, ART_SLIDE_NAME, "Layer", "bobomb");
    pImageInstance->m_bVisible = false;
    m_pBobomb = pImageInstance->m_pTextureResource;

    pImageInstance = FEFinder<TLImageInstance, 2>::Find<FEPresentation>(
        presentation, ART_SLIDE_NAME, "Layer", "chomp");
    pImageInstance->m_bVisible = false;
    m_pChomp = pImageInstance->m_pTextureResource;

    for (int i = 0; i < 12; i++)
    {
        char name[64];
        nlSNPrintf(name, sizeof(name), "ability_%s", GetCharacterInfo(GetCharacterIndexFromCaptain(i)).mName);
        pImageInstance = FEFinder<TLImageInstance, 2>::Find<FEPresentation>(
            presentation, ART_SLIDE_NAME, "Layer", name);
        if (pImageInstance)
        {
            m_pCaptainAbility[i] = pImageInstance->m_pTextureResource;
        }
        else
        {
            m_pCaptainAbility[i] = 0;
        }
    }
}

void HUDPowerUpDisplay::DisplayPowerUps(float fDeltaT)
{
    FETextureResource* pTextureResource[2];
    for (int team = 0; team < 2; team++)
    {
        int numPowerUps = 0;
        for (int i = 0; i < 2; i++)
        {
            int num = g_pTeams[team]->GetPowerUpByIndex(i).nnumOfPowerups;
            switch (g_pTeams[team]->GetPowerUpByIndex(i).eType)
            {
            case -1:
                pTextureResource[i] = 0;
                break;
            case 0:
                pTextureResource[i] = m_pPowerUpTextures->m_pShellGreen;
                break;
            case 2:
                pTextureResource[i] = m_pPowerUpTextures->m_pShellSpike;
                break;
            case 3:
                pTextureResource[i] = m_pPowerUpTextures->m_pShellBlue;
                break;
            case 1:
                pTextureResource[i] = m_pPowerUpTextures->m_pShellRed;
                break;
            case 7:
                pTextureResource[i] = m_pPowerUpTextures->m_pMushroom;
                break;
            case 4:
                pTextureResource[i] = m_pPowerUpTextures->m_pBanana;
                break;
            case 5:
                pTextureResource[i] = m_pPowerUpTextures->m_pBobomb;
                break;
            case 8:
                pTextureResource[i] = m_pPowerUpTextures->m_pStar;
                break;
            case 6:
                pTextureResource[i] = m_pPowerUpTextures->m_pChomp;
                break;
            case 9:
                pTextureResource[i] = m_pPowerUpTextures->m_pCaptainAbility[0];
                break;
            case 11:
                pTextureResource[i] = m_pPowerUpTextures->m_pCaptainAbility[3];
                break;
            case 10:
                pTextureResource[i] = m_pPowerUpTextures->m_pCaptainAbility[5];
                break;
            case 12:
                pTextureResource[i] = m_pPowerUpTextures->m_pCaptainAbility[1];
                break;
            case 13:
                pTextureResource[i] = m_pPowerUpTextures->m_pCaptainAbility[4];
                break;
            case 14:
                pTextureResource[i] = m_pPowerUpTextures->m_pCaptainAbility[2];
                break;
            case 15:
                pTextureResource[i] = m_pPowerUpTextures->m_pCaptainAbility[7];
                break;
            case 16:
                pTextureResource[i] = m_pPowerUpTextures->m_pCaptainAbility[6];
                break;
            case 17:
                pTextureResource[i] = m_pPowerUpTextures->m_pCaptainAbility[9];
                break;
            case 18:
                pTextureResource[i] = m_pPowerUpTextures->m_pCaptainAbility[10];
                break;
            case 19:
                pTextureResource[i] = m_pPowerUpTextures->m_pCaptainAbility[8];
                break;
            case 20:
                pTextureResource[i] = m_pPowerUpTextures->m_pCaptainAbility[11];
                break;
            }
            if (!pTextureResource[i])
            {
                m_pImagePowerUps[0][team][i]->m_bVisible = false;
                m_pImagePowerUps[1][team][i]->m_bVisible = false;
                mNumFlareCycles[team][i] = -1;
                m_pImageFlares[team][i]->m_bVisible = false;
                m_pPowerUpPads[team][i]->SetActiveSlide("no pup", true, false);
                m_pPowerupTextComponents[team][i]->SetActiveSlide("1", true, false);
                m_pPowerupTextComponents[team][i]->SetActiveSlide("1", true, false);
            }
            else
            {
                numPowerUps++;
                if (g_pTeams[team]->GetPowerUpByIndex(i).bIsNew && mNumFlareCycles[team][i] == -1)
                {
                    m_pPowerUpPads[team][i]->SetActiveSlide("get pup", true, false);
                    m_pImageFlares[team][i]->m_bVisible = true;
                    mNumFlareCycles[team][i] = 20;
                }
                else if (mNumFlareCycles[team][i] != -1)
                {
                    TLSlide* activeSlide = m_pComponentFlares[team][i]->GetActiveSlide();
                    if (activeSlide && activeSlide->GetCurrentTime() >= activeSlide->GetStartTime() + activeSlide->GetDuration() - 0.1f)
                    {
                        m_pPowerUpPads[team][i]->SetActiveSlide("no pup", true, false);
                        m_pImagePowerUps[0][team][i]->m_bVisible = true;
                        m_pImagePowerUps[1][team][i]->m_bVisible = true;
                        m_pImageFlares[team][i]->m_bVisible = false;
                        g_pTeams[team]->SetIsPowerUpNew(i, false);
                        mNumFlareCycles[team][i] = -1;
                    }
                }
            }
            TLImageInstance* pImageInstance = m_pImagePowerUps[0][team][i];
            if (pTextureResource[i])
            {
                pImageInstance->m_pTextureResource = pTextureResource[i];
            }
            pImageInstance = m_pImagePowerUps[1][team][i];
            if (pTextureResource[i])
            {
                pImageInstance->m_pTextureResource = pTextureResource[i];
            }
            if (mNumFlareCycles[team][i] == -1 && pTextureResource[i])
            {
                m_pImagePowerUps[0][team][i]->m_bVisible = true;
                m_pImagePowerUps[1][team][i]->m_bVisible = true;
                if (i == 0)
                {
                    cFielder* pCaptain = g_pTeams[team]->GetCaptain();
                    if (pCaptain && (IsBowserSuperPowerActive(pCaptain) || IsWaluigiSuperPowerActive(pCaptain)
                                       || pCaptain->IsWarioSuperPowerActive() || pCaptain->IsPeteySuperPowerActive()))
                    {
                        m_pImagePowerUps[0][team][i]->m_bVisible = false;
                        m_pImagePowerUps[1][team][i]->m_bVisible = false;
                    }
                }
            }
            if (mNumFlareCycles[team][i] != -1 || (unsigned int)num <= 1)
            {
                m_pPowerupTextComponents[team][i]->SetActiveSlide("1", true, false);
            }
            else if (num == 3)
            {
                m_pPowerupTextComponents[team][i]->SetActiveSlide("X3", true, false);
            }
            else if (num == 5)
            {
                m_pPowerupTextComponents[team][i]->SetActiveSlide("X5", true, false);
                m_pPowerupTextComponents[team][i]->SetActiveSlide("X5", true, false);
            }
        }
        for (int i = 0; i < 2; i++)
        {
            if (i < numPowerUps)
            {
                m_pComponentFlares[team][i]->SetActiveSlide("Slide1", false, false);
            }
            else
            {
                m_pComponentFlares[team][i]->SetActiveSlide("out", false, false);
            }
        }
        mHasPowerUps[team] = numPowerUps != 0;
        m_pBlinkers[team]->m_bVisible = mHasPowerUps[team];
        if (mHasTwoPowerUps[team] && numPowerUps == 1)
        {
            m_pPowerUpComponents[team][0]->SetActiveSlide("move", true, false);
        }
        mHasTwoPowerUps[team] = numPowerUps == 2;
    }
}

void HUDOverlay::SetTeamIcons()
{
    const char* filename = "art/fe/CaptainIconsUI.res";
    mAsyncImage[0] = new (8, false) AsyncImage(filename, 0);
    TLComponentInstance* pCompLeft = FEFinder<TLComponentInstance, 4>::Find<FEPresentation>(
        mPresentation, HUD_SLIDE_IN_NAME, LAYER_NAME, HUD_TEAM_NAMES[0]);
    mAsyncImage[0]->SetImageInstance(FEFinder<TLImageInstance, 2>::Find<TLSlide>(
        pCompLeft->GetActiveSlide(), HUD_NAMES[0], "mario_left"));
    mAsyncImage[1] = new (8, false) AsyncImage(filename, 0);
    TLComponentInstance* pCompRight = FEFinder<TLComponentInstance, 4>::Find<FEPresentation>(
        mPresentation, HUD_SLIDE_IN_NAME, LAYER_NAME, HUD_TEAM_NAMES[1]);
    mAsyncImage[1]->SetImageInstance(FEFinder<TLImageInstance, 2>::Find(
        pCompRight, HUD_NAMES[1], "mario_right"));
    for (int i = 0; i < 2; i++)
    {
        char path[64];
        nlSNPrintf(path, sizeof(path), "fe/captain_icons/captain_icons_%s",
            GetTeamName((eTeamID)nlSingleton<GameInfoManager>::Instance()->GetTeam(i)));
        mAsyncImage[i]->QueueLoad(path, true);
    }
}

void HUDOverlay::UpdateScore()
{
    mScoreDisplay.mNewScore[0] = g_pTeams[0]->m_nScore;
    mScoreDisplay.mNewScore[1] = g_pTeams[1]->m_nScore;
}

void HUDOverlay::DisplayNewScore()
{
    for (int team = 0; team < 2; team++)
    {
        for (int flare = 0; flare < 2; flare++)
        {
            if (mPowerUpDisplay.mNumFlareCycles[team][flare] != -1)
            {
                mPowerUpDisplay.mNumFlareCycles[team][flare] = 20;
            }
        }
    }
}

void HUDOverlay::ResetScores()
{
    mScoreDisplay.ResetScores();
}

void HUDScoreDisplay::ResetScores()
{
    for (int i = 0; i < 2; i++)
    {
        mScore[i] = 0;
        mNewScore[i] = 0;
        if (nlSingleton<GameInfoManager>::Instance()->IsInMode4())
        {
            mScore[i] = g_pStrikerChallenge->mScore[i];
            mNewScore[i] = g_pStrikerChallenge->mScore[i];
        }
        char scoreString[16];
        nlSNPrintf(scoreString, sizeof(scoreString), "%d", mScore[i]);
        nlStrToWcs(scoreString, mScoreBuffer[i], 32);
        m_pTextInstanceScore[0][i]->SetString(mScoreBuffer[i]);
        m_pTextInstanceScore[1][i]->SetString(mScoreBuffer[i]);
        mStartScoreAnimation[i] = false;
        mScoreUpdateDelay[i] = 0.0f;
    }
}

void HUDOverlay::SwapPowerUps(int homeAway)
{
}

void HUDClock::Init(FEPresentation* presentation)
{
    typedef BasicString<unsigned short, Detail::TempStringAllocator> WideString;
    m_pTextInstanceClock[0] = FEFinder<TLTextInstance, 3>::Find<FEPresentation>(
        presentation, HUD_SLIDE_IN_NAME, LAYER_NAME, "clock elements", "clock");
    m_pTextInstanceClock[1] = FEFinder<TLTextInstance, 3>::Find<FEPresentation>(
        presentation, HUD_SLIDE_OUT_NAME, LAYER_NAME, "clock elements", "clock");
    m_pTextInstanceGameType = FEFinder<TLTextInstance, 3>::Find<FEPresentation>(
        presentation, HUD_SLIDE_IN_NAME, LAYER_NAME, "clock elements", "gametype");
    if (m_pTextInstanceClock[0])
    {
        mOriginalClockColour = m_pTextInstanceClock[0]->GetColour();
    }
    mSuddenDeath[0] = FEFinder<TLComponentInstance, 4>::Find<FEPresentation>(
        presentation, HUD_SLIDE_IN_NAME, LAYER_NAME, "clock elements", "SUDDEN DEATH");
    mSuddenDeath[1] = FEFinder<TLComponentInstance, 4>::Find<FEPresentation>(
        presentation, HUD_SLIDE_OUT_NAME, LAYER_NAME, "clock elements", "SUDDEN DEATH");
    mSuddenDeath[0]->m_bVisible = false;
    mSuddenDeath[1]->m_bVisible = false;
    if (nlSingleton<GameInfoManager>::Instance()->GetCurrentSettings()->GameLimitType == 1)
    {
        char goalLimit[4];
        unsigned short goalLimitWide[4];
        int numGoals = nlSingleton<GameInfoManager>::Instance()->GetCurrentSettings()->GoalLimit;
        nlSNPrintf(goalLimit, sizeof(goalLimit), "%d", numGoals);
        nlStrToWcs(goalLimit, goalLimitWide, 4);
        WideString unformatted(g_pLocalization->GetString("HUD_FIRST_TO"));
        WideString formatted = Format(unformatted, goalLimitWide);
        memcpy(mClockBuffer, formatted.c_str(), sizeof(mClockBuffer));
        m_pTextInstanceGameType->SetString(mClockBuffer);
        m_pTextInstanceClock[0]->m_bVisible = false;
        m_pTextInstanceClock[1]->m_bVisible = false;
    }
    else
    {
        m_pTextInstanceClock[0]->m_bVisible = true;
        m_pTextInstanceClock[1]->m_bVisible = true;
        m_pTextInstanceGameType->m_bVisible = false;
    }
}

void HUDClock::Update(float fDeltaT)
{
    typedef BasicString<unsigned short, Detail::TempStringAllocator> WideString;
    bool isOvertime = nlSingleton<StatsTracker>::Instance()->IsOvertime();
    float fTime = g_pGame->GetGameTime();
    float overtimeTime = 59999.0f;
    float fRemainingTime = g_pGame->GetGameDuration() - fTime;
    fTime -= g_pGame->GetGameDuration();
    overtimeTime = (fTime > overtimeTime) ? overtimeTime : fTime;
    unsigned long time = (unsigned long)fRemainingTime;
    unsigned long remainingTime = (unsigned long)(isOvertime ? overtimeTime : (float)time);
    unsigned long newMinutes = remainingTime / 60;
    unsigned long newSeconds = remainingTime - newMinutes * 60;
    unsigned long newTenths = 0;
    GetFixedUpdateTask();
    if (fRemainingTime <= 30.0f || isOvertime)
    {
        if (!mClockColourChanged)
        {
            mClockColourChanged = true;
            nlColour clockColour;
            nlColourSet(clockColour, 0xCC, 0x33, 0x33, 0xFF);
            m_pTextInstanceClock[0]->SetAssetColour(clockColour);
            m_pTextInstanceClock[1]->SetAssetColour(clockColour);
        }
    }
    if (newMinutes == 0 && fRemainingTime < 30.0f && !isOvertime)
    {
        newTenths = (unsigned long)((fRemainingTime - (float)newSeconds) * 10.0f);
    }
    if (!isOvertime && (float)remainingTime == g_pGame->GetGameDuration() && mClockColourChanged)
    {
        mClockColourChanged = false;
        mOvertimeSFXPlayed = false;
        m_pTextInstanceClock[0]->SetAssetColour(mOriginalClockColour);
        m_pTextInstanceClock[1]->SetAssetColour(mOriginalClockColour);
        m_pTextInstanceClock[0]->m_bVisible = true;
        m_pTextInstanceClock[1]->m_bVisible = true;
    }
    if (isOvertime)
    {
        if (!mOvertimeSFXPlayed)
        {
            mOvertimeSFXPlayed = true;
        }
        mSuddenDeath[0]->m_bVisible = true;
        mSuddenDeath[1]->m_bVisible = true;
        m_pTextInstanceClock[0]->m_bVisible = false;
        m_pTextInstanceClock[1]->m_bVisible = false;
    }
    else
    {
        mSuddenDeath[0]->m_bVisible = false;
        mSuddenDeath[1]->m_bVisible = false;
        m_pTextInstanceClock[0]->m_bVisible = true;
        m_pTextInstanceClock[1]->m_bVisible = true;
    }
    if (newSeconds != mSeconds || newMinutes != mMinutes || newTenths != mTenths)
    {
        WideString unformatted;
        WideString formatted;
        mSeconds = newSeconds;
        mMinutes = newMinutes;
        mTenths = newTenths;
        unsigned short minutesWideString[8];
        unsigned short secondsWideString[8];
        if (mMinutes == 0 && fRemainingTime < 30.0f && !isOvertime)
        {
            nlSNPrintf(minutesWideString, 8, (const unsigned short*)L"%d", newSeconds);
            nlSNPrintf(secondsWideString, 8, (const unsigned short*)L"%d", newTenths);
            unformatted = WideString(g_pLocalization->GetString("CLOCK2"));
            formatted = Format(unformatted, minutesWideString, secondsWideString);
        }
        else
        {
            if (mSeconds < 10)
            {
                nlSNPrintf(secondsWideString, 8, (const unsigned short*)L"0%d", newSeconds);
            }
            else
            {
                nlSNPrintf(secondsWideString, 8, (const unsigned short*)L"%d", newSeconds);
            }
            nlSNPrintf(minutesWideString, 8, (const unsigned short*)L"%d", newMinutes);
            unformatted = WideString(g_pLocalization->GetString("CLOCK"));
            formatted = Format(unformatted, minutesWideString, secondsWideString);
        }
        memcpy(mClockBuffer, formatted.c_str(), sizeof(mClockBuffer));
        m_pTextInstanceClock[0]->SetString(mClockBuffer);
        m_pTextInstanceClock[1]->SetString(mClockBuffer);
    }
}

void HUDScoreDisplay::Init(FEPresentation* presentation)
{
    mPresentation = presentation;
    TLComponentInstance* leftIn = FEFinder<TLComponentInstance, 4>::Find<FEPresentation>(
        presentation, HUD_SLIDE_IN_NAME, LAYER_NAME, "clock elements", "left_score");
    TLComponentInstance* leftOut = FEFinder<TLComponentInstance, 4>::Find<FEPresentation>(
        presentation, HUD_SLIDE_OUT_NAME, LAYER_NAME, "clock elements", "left_score");
    TLComponentInstance* rightIn = FEFinder<TLComponentInstance, 4>::Find<FEPresentation>(
        presentation, HUD_SLIDE_IN_NAME, LAYER_NAME, "clock elements", "right_score");
    TLComponentInstance* rightOut = FEFinder<TLComponentInstance, 4>::Find<FEPresentation>(
        presentation, HUD_SLIDE_OUT_NAME, LAYER_NAME, "clock elements", "right_score");
    if (leftIn)
    {
        m_pTextInstanceScore[0][0] = FEFinder<TLTextInstance, 3>::Find<TLSlide>(leftIn->GetActiveSlide(), "scoretext");
    }
    if (leftOut)
    {
        m_pTextInstanceScore[1][0] = FEFinder<TLTextInstance, 3>::Find<TLSlide>(leftOut->GetActiveSlide(), "scoretext");
    }
    if (rightIn)
    {
        m_pTextInstanceScore[0][1] = FEFinder<TLTextInstance, 3>::Find<TLSlide>(rightIn->GetActiveSlide(), "scoretext");
    }
    if (rightOut)
    {
        m_pTextInstanceScore[1][1] = FEFinder<TLTextInstance, 3>::Find<TLSlide>(rightOut->GetActiveSlide(), "scoretext");
    }
    m_pTextInstanceScore[0][0]->m_bVisible = false;
    m_pTextInstanceScore[1][0]->m_bVisible = false;
    m_pTextInstanceScore[0][1]->m_bVisible = false;
    m_pTextInstanceScore[1][1]->m_bVisible = false;
    leftIn->m_bVisible = false;
    leftOut->m_bVisible = false;
    rightIn->m_bVisible = false;
    rightOut->m_bVisible = false;
    if (nlSingleton<GameInfoManager>::Instance()->IsInMode4())
    {
        mScore[0] = g_pTeams[0]->m_nScore;
        mScore[1] = g_pTeams[1]->m_nScore;
    }
}

void HUDScoreDisplay::Update(float fDeltaT)
{
    unsigned long slideInHash = HUD_SLIDE_IN_HASH;
    for (int i = 0; i < 2; i++)
    {
        TLSlide* currentSlide = mPresentation->m_currentSlide;
        if (slideInHash == currentSlide->m_hash && mScoreUpdateDelay[i] > 0.0f
            && currentSlide->GetCurrentTime() >= currentSlide->GetStartTime() + currentSlide->GetDuration())
        {
            if (mStartScoreAnimation[i])
            {
                TLComponentInstance* pScoreComp = FEFinder<TLComponentInstance, 4>::Find<FEPresentation>(
                    mPresentation, HUD_SLIDE_IN_NAME, LAYER_NAME, "clock elements", i == 0 ? "left_score" : "right_score");
                if (pScoreComp)
                {
                    TLSlide* activeSlide = pScoreComp->GetActiveSlide();
                    float endTime = activeSlide->GetStartTime() + activeSlide->GetDuration();
                    if (pScoreComp->GetActiveSlide()->GetCurrentTime() >= endTime)
                    {
                        pScoreComp->SetActiveSlide("Slide1", true, false);
                        mStartScoreAnimation[i] = false;
                    }
                }
                else
                {
                    mStartScoreAnimation[i] = false;
                }
            }
            if (!mStartScoreAnimation[i])
            {
                mScoreUpdateDelay[i] -= fDeltaT;
            }
            if (mScoreUpdateDelay[i] <= 0.0f)
            {
                mScoreUpdateDelay[i] = 0.0f;
                mScore[i]++;
                char scoreString[16];
                nlSNPrintf(scoreString, sizeof(scoreString), "%d", mScore[i]);
                nlStrToWcs(scoreString, mScoreBuffer[i], 32);
                m_pTextInstanceScore[0][i]->SetString(mScoreBuffer[i]);
                m_pTextInstanceScore[1][i]->SetString(mScoreBuffer[i]);
            }
        }
    }
}

void HUDCaptainMeter::Update(float fDeltaT)
{
    for (int i = 0; i < 2; i++)
    {
        cFielder* pCaptain = g_pTeams[i]->GetCaptain();
        if (pCaptain && (IsBowserSuperPowerActive(pCaptain) || IsWaluigiSuperPowerActive(pCaptain)
                           || pCaptain->IsWarioSuperPowerActive() || pCaptain->IsPeteySuperPowerActive()))
        {
            m_pMeter[i]->m_bVisible = true;
            m_pPowerBarContainer[i]->m_bVisible = false;
            m_pPowerUpPad[i]->SetActiveSlide("tank", false, true);
            mMeterShown[i] = true;
            float fScale = mMeterFullScale[i] * pCaptain->GetSuperPowerTankFraction();
            feVector3 scale = m_pMeterFill[i]->GetScale();
            if (fScale > mMeterFullScale[i])
            {
                fScale = mMeterFullScale[i];
            }
            else if (fScale <= 0.0f)
            {
                fScale = 0.01f;
                m_pPowerUpPad[i]->SetActiveSlide("no pup", true, false);
                mMeterShown[i] = false;
            }
            m_pMeterFill[i]->SetAssetScale(fScale, scale.f.y, scale.f.z);
        }
        else
        {
            if (mMeterShown[i])
            {
                m_pPowerUpPad[i]->SetActiveSlide("no pup", true, false);
                mMeterShown[i] = false;
            }
            m_pMeter[i]->m_bVisible = false;
            m_pPowerBarContainer[i]->m_bVisible = false;
        }
    }
}

void HUDCaptainMeter::Init(FEPresentation* presentation)
{
    for (int i = 0; i < 2; i++)
    {
        TLInstance* pPowerBarContainer = FEFinder<TLComponentInstance, 4>::Find<TLSlide>(
            presentation->m_currentSlide, LAYER_NAME, POWERBAR_CONTAINER_NAMES[i]);
        if (pPowerBarContainer == 0)
        {
            pPowerBarContainer = &TLComponentDefault::sInstance;
        }
        m_pPowerBarContainer[i] = (TLComponentInstance*)pPowerBarContainer;
        TLComponentInstance* pComp = FEFinder<TLComponentInstance, 4>::Find<TLSlide>(
            presentation->m_currentSlide, LAYER_NAME, HUD_TEAM_NAMES[i]);
        m_pPowerUpPad[i] = FEFinder<TLComponentInstance, 4>::Find<TLSlide>(
            pComp->GetActiveSlide(), HUD_NAMES[i], PAD_NAMES[0]);
        m_pMeter[i] = FEFinder<TLComponentInstance, 4>::Find(
            m_pPowerUpPad[i], "tank", "the metre", "metre");
        m_pMeterFill[i] = FEFinder<TLImageInstance, 2>::Find(m_pMeter[i], "white_8x8");
        nlColour colour;
        switch (nlSingleton<GameInfoManager>::Instance()->GetTeam(i))
        {
        case 1:
            nlColourSet(colour, 0xEE, 0x9A, 0x15, 0xFF);
            break;
        case 11:
            nlColourSet(colour, 0x8F, 0x72, 0x19, 0xFF);
            break;
        case 7:
            nlColourSet(colour, 0x52, 0xA7, 0x38, 0xFF);
            break;
        case 6:
            nlColourSet(colour, 0x66, 0x3A, 0x8F, 0xFF);
            break;
        default:
            nlColourSet(colour, 0xFF, 0xFF, 0xFF, 0xFF);
            break;
        }
        m_pMeterFill[i]->SetAssetColour(colour);
        TLInstance* pPowerBar = FEFinder<TLComponentInstance, 4>::Find<FEPresentation>(
            presentation, HUD_SLIDE_OUT_NAME, LAYER_NAME, POWERBAR_NAMES[i]);
        if (pPowerBar == 0)
        {
            pPowerBar = &TLComponentDefault::sInstance;
        }
        pPowerBar->m_bVisible = false;
        if (m_pMeter[i])
        {
            feVector3 scale = m_pMeterFill[i]->GetScale();
            mMeterFullScale[i] = scale.f.x;
        }
        else
        {
            mMeterFullScale[i] = 0.0f;
        }
    }
}

void HUDPowerUpDisplay::Init(FEPresentation* presentation, HUDPowerUpTextures* textures)
{
    m_pPowerUpTextures = textures;
    for (int team = 0; team < 2; team++)
    {
        for (int i = 0; i < 2; i++)
        {
            TLComponentInstance* pTeamComp = FEFinder<TLComponentInstance, 4>::Find<FEPresentation>(
                presentation, HUD_SLIDE_IN_NAME, LAYER_NAME, HUD_TEAM_NAMES[team]);
            TLComponentInstance* pComp = FEFinder<TLComponentInstance, 4>::Find<TLSlide>(
                pTeamComp->GetActiveSlide(), HUD_NAMES[team], POWER_UP_IMAGE_NAMES[team][i]);
            if (pComp)
            {
                m_pImagePowerUps[0][team][i] = FEFinder<TLImageInstance, 2>::Find(pComp, "Slide1", "powerupimage");
                if (i == 0)
                {
                    TLComponentInstance* pPulsar = FEFinder<TLComponentInstance, 4>::Find(pComp, "move", "pulsar");
                    TLInstance* pPowerUpImage = FEFinder<TLImageInstance, 2>::Find<TLSlide>(
                        pPulsar->GetActiveSlide(), "powerupimage");
                    if (pPowerUpImage == 0)
                    {
                        pPowerUpImage = &TLImageDefault::sInstance;
                    }
                    m_pImagePowerUps[1][team][i] = (TLImageInstance*)pPowerUpImage;
                }
                else
                {
                    m_pImagePowerUps[1][team][i] = m_pImagePowerUps[0][team][i];
                }
                m_pPowerUpComponents[team][i] = pComp;
            }
            m_pBlinkers[team] = FEFinder<TLComponentInstance, 4>::Find<TLSlide>(
                pTeamComp->GetActiveSlide(), HUD_NAMES[team], "blinker");
            m_pBlinkers[team]->m_bVisible = false;
            m_pPowerUpPads[team][i] = FEFinder<TLComponentInstance, 4>::Find<TLSlide>(
                pTeamComp->GetActiveSlide(), HUD_NAMES[team], PAD_NAMES[i]);
            TLComponentInstance* pElectric = FEFinder<TLComponentInstance, 4>::Find(
                m_pPowerUpPads[team][i], "get pup", "electric");
            TLComponentInstance* pFlare = FEFinder<TLComponentInstance, 4>::Find<TLSlide>(
                pElectric->GetActiveSlide(), "flare");
            if (pFlare)
            {
                m_pComponentFlares[team][i] = pFlare;
                m_pImageFlares[team][i] = FEFinder<TLImageInstance, 2>::Find<TLSlide>(pFlare->GetActiveSlide(), "flare");
                m_pImageFlares[team][i]->m_bVisible = false;
            }
            m_pPowerupTextComponents[team][i] = FEFinder<TLComponentInstance, 4>::Find<TLSlide>(
                pTeamComp->GetActiveSlide(), HUD_NAMES[team], POWER_UP_TEXT_NAMES[team][i]);
        }
    }
}

#include "Game/UnidentifiedStaticStorage.h"
