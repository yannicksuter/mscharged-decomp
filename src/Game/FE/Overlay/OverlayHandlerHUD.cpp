#include "Game/OverlayHandlerHUD.h"
#include "NL/nlBasicString.inl"
#include "Game/AI/Fielder.h"

#include "Game/DB/GameProgress.h"
#include "Game/DB/CharacterInfo.h"
#include "Game/DB/StatsTracker.h"
#include "Game/FE/feHelpFuncs.h"
#include "Game/FE/feAsyncImage.h"
#include "Game/FE/feFinder.inl"
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

extern "C" bool fn_8003E8A0(const cFielder*);
extern "C" bool fn_8003E948(const cFielder*);
extern "C" bool fn_8003E99C(const cFielder*);

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
    mUnidentified1A8.fn_801EAD2C(fDeltaT);
    mUnidentified028.fn_801EAF50(fDeltaT);
    mUnidentified0AC.fn_801E9198(fDeltaT);
    if (nlSingleton<GameInfoManager>::Instance()->GetCurrentSettings()->GameLimitType == 0)
    {
        mUnidentified140.fn_801E9FEC(fDeltaT);
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
    mUnidentified140.fn_801E9AF0(presentation);
    mUnidentified1A8.fn_801EA808(presentation);
    mUnidentified028.fn_801EB108(presentation);
    mUnidentified054.fn_801E8CD0(presentation);
    mUnidentified0AC.fn_801EB5CC(presentation, &mUnidentified054);
    SetTeamIcons();
    if (nlSingleton<GameInfoManager>::Instance()->mIsInStrikers101Mode)
    {
        mUnidentified140.m_pTextInstanceClock[0]->m_bVisible = false;
        mUnidentified140.m_pTextInstanceClock[1]->m_bVisible = false;
    }
    mPresentation->SetActiveSlide(HUD_SLIDE_OUT_NAME, true);
    mUnidentified1A8.fn_801E99F0();
}

void HUDOverlay::SetSlideIn()
{
    mPresentation->SetActiveSlide(HUD_SLIDE_IN_NAME, true);
}

void HUDOverlay::SetSlideOut()
{
    mPresentation->SetActiveSlide(HUD_SLIDE_OUT_NAME, true);
}

void UnidentifiedHUD_801E8CD0::fn_801E8CD0(FEPresentation* presentation)
{
    TLImageInstance* pImageInstance;

    pImageInstance = FEFinder<TLImageInstance, 2>::Find<FEPresentation>(
        presentation, ART_SLIDE_NAME, "Layer", "star");
    pImageInstance->m_bVisible = false;
    m_pStar = pImageInstance->m_pTextureResource;

    pImageInstance = FEFinder<TLImageInstance, 2>::Find<FEPresentation>(
        presentation, ART_SLIDE_NAME, "Layer", "mega");
    pImageInstance->m_bVisible = false;
    mUnidentified04 = pImageInstance->m_pTextureResource;

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
            mUnidentified28[i] = pImageInstance->m_pTextureResource;
        }
        else
        {
            mUnidentified28[i] = 0;
        }
    }
}

void UnidentifiedHUD_801E9198::fn_801E9198(float fDeltaT)
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
                pTextureResource[i] = mUnidentified8C->m_pShellGreen;
                break;
            case 2:
                pTextureResource[i] = mUnidentified8C->m_pShellSpike;
                break;
            case 3:
                pTextureResource[i] = mUnidentified8C->m_pShellBlue;
                break;
            case 1:
                pTextureResource[i] = mUnidentified8C->m_pShellRed;
                break;
            case 7:
                pTextureResource[i] = mUnidentified8C->m_pMushroom;
                break;
            case 4:
                pTextureResource[i] = mUnidentified8C->m_pBanana;
                break;
            case 5:
                pTextureResource[i] = mUnidentified8C->m_pBobomb;
                break;
            case 8:
                pTextureResource[i] = mUnidentified8C->m_pStar;
                break;
            case 6:
                pTextureResource[i] = mUnidentified8C->m_pChomp;
                break;
            case 9:
                pTextureResource[i] = mUnidentified8C->mUnidentified28[0];
                break;
            case 11:
                pTextureResource[i] = mUnidentified8C->mUnidentified28[3];
                break;
            case 10:
                pTextureResource[i] = mUnidentified8C->mUnidentified28[5];
                break;
            case 12:
                pTextureResource[i] = mUnidentified8C->mUnidentified28[1];
                break;
            case 13:
                pTextureResource[i] = mUnidentified8C->mUnidentified28[4];
                break;
            case 14:
                pTextureResource[i] = mUnidentified8C->mUnidentified28[2];
                break;
            case 15:
                pTextureResource[i] = mUnidentified8C->mUnidentified28[7];
                break;
            case 16:
                pTextureResource[i] = mUnidentified8C->mUnidentified28[6];
                break;
            case 17:
                pTextureResource[i] = mUnidentified8C->mUnidentified28[9];
                break;
            case 18:
                pTextureResource[i] = mUnidentified8C->mUnidentified28[10];
                break;
            case 19:
                pTextureResource[i] = mUnidentified8C->mUnidentified28[8];
                break;
            case 20:
                pTextureResource[i] = mUnidentified8C->mUnidentified28[11];
                break;
            }
            if (!pTextureResource[i])
            {
                m_pImagePowerUps[0][team][i]->m_bVisible = false;
                m_pImagePowerUps[1][team][i]->m_bVisible = false;
                mNumFlareCycles[team][i] = -1;
                mUnidentified20[team][i]->m_bVisible = false;
                mUnidentified50[team][i]->SetActiveSlide("no pup", true, false);
                m_pPowerupTextComponents[team][i]->SetActiveSlide("1", true, false);
                m_pPowerupTextComponents[team][i]->SetActiveSlide("1", true, false);
            }
            else
            {
                numPowerUps++;
                if (g_pTeams[team]->GetPowerUpByIndex(i).bIsNew && mNumFlareCycles[team][i] == -1)
                {
                    mUnidentified50[team][i]->SetActiveSlide("get pup", true, false);
                    mUnidentified20[team][i]->m_bVisible = true;
                    mNumFlareCycles[team][i] = 20;
                }
                else if (mNumFlareCycles[team][i] != -1)
                {
                    TLSlide* activeSlide = m_pComponentFlares[team][i]->GetActiveSlide();
                    if (activeSlide && activeSlide->GetCurrentTime() >= activeSlide->GetStartTime() + activeSlide->GetDuration() - 0.1f)
                    {
                        mUnidentified50[team][i]->SetActiveSlide("no pup", true, false);
                        m_pImagePowerUps[0][team][i]->m_bVisible = true;
                        m_pImagePowerUps[1][team][i]->m_bVisible = true;
                        mUnidentified20[team][i]->m_bVisible = false;
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
                    if (pCaptain && (fn_8003E8A0(pCaptain) || fn_8003E948(pCaptain)
                                       || fn_8003E99C(pCaptain) || pCaptain->fn_8003E9F0()))
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
        mUnidentified88[team] = numPowerUps != 0;
        mUnidentified70[team]->m_bVisible = mUnidentified88[team];
        if (mUnidentified8A[team] && numPowerUps == 1)
        {
            mUnidentified60[team][0]->SetActiveSlide("move", true, false);
        }
        mUnidentified8A[team] = numPowerUps == 2;
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
    mUnidentified1A8.mNewScore[0] = g_pTeams[0]->m_nScore;
    mUnidentified1A8.mNewScore[1] = g_pTeams[1]->m_nScore;
}

void HUDOverlay::DisplayNewScore()
{
    for (int team = 0; team < 2; team++)
    {
        for (int flare = 0; flare < 2; flare++)
        {
            if (mUnidentified0AC.mNumFlareCycles[team][flare] != -1)
            {
                mUnidentified0AC.mNumFlareCycles[team][flare] = 20;
            }
        }
    }
}

void HUDOverlay::ResetScores()
{
    mUnidentified1A8.fn_801E99F0();
}

void UnidentifiedHUD_801E99F0::fn_801E99F0()
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

void UnidentifiedHUD_801E9AF0::fn_801E9AF0(FEPresentation* presentation)
{
    typedef BasicString<unsigned short, Detail::TempStringAllocator> WideString;
    m_pTextInstanceClock[0] = FEFinder<TLTextInstance, 3>::Find<FEPresentation>(
        presentation, HUD_SLIDE_IN_NAME, LAYER_NAME, "clock elements", "clock");
    m_pTextInstanceClock[1] = FEFinder<TLTextInstance, 3>::Find<FEPresentation>(
        presentation, HUD_SLIDE_OUT_NAME, LAYER_NAME, "clock elements", "clock");
    mUnidentified64 = FEFinder<TLTextInstance, 3>::Find<FEPresentation>(
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
        mUnidentified64->SetString(mClockBuffer);
        m_pTextInstanceClock[0]->m_bVisible = false;
        m_pTextInstanceClock[1]->m_bVisible = false;
    }
    else
    {
        m_pTextInstanceClock[0]->m_bVisible = true;
        m_pTextInstanceClock[1]->m_bVisible = true;
        mUnidentified64->m_bVisible = false;
    }
}

void UnidentifiedHUD_801E9AF0::fn_801E9FEC(float fDeltaT)
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

void UnidentifiedHUD_801E99F0::fn_801EA808(FEPresentation* presentation)
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

void UnidentifiedHUD_801E99F0::fn_801EAD2C(float fDeltaT)
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

void UnidentifiedHUD_801EB108::fn_801EAF50(float fDeltaT)
{
    for (int i = 0; i < 2; i++)
    {
        cFielder* pCaptain = g_pTeams[i]->GetCaptain();
        if (pCaptain && (fn_8003E8A0(pCaptain) || fn_8003E948(pCaptain)
                           || fn_8003E99C(pCaptain) || pCaptain->fn_8003E9F0()))
        {
            mUnidentified00[i]->m_bVisible = true;
            mUnidentified08[i]->m_bVisible = false;
            mUnidentified20[i]->SetActiveSlide("tank", false, true);
            mUnidentified28[i] = true;
            float fScale = mUnidentified18[i] * pCaptain->fn_8004FFF8();
            feVector3 scale = mUnidentified10[i]->GetScale();
            if (fScale > mUnidentified18[i])
            {
                fScale = mUnidentified18[i];
            }
            else if (fScale <= 0.0f)
            {
                fScale = 0.01f;
                mUnidentified20[i]->SetActiveSlide("no pup", true, false);
                mUnidentified28[i] = false;
            }
            mUnidentified10[i]->SetAssetScale(fScale, scale.f.y, scale.f.z);
        }
        else
        {
            if (mUnidentified28[i])
            {
                mUnidentified20[i]->SetActiveSlide("no pup", true, false);
                mUnidentified28[i] = false;
            }
            mUnidentified00[i]->m_bVisible = false;
            mUnidentified08[i]->m_bVisible = false;
        }
    }
}

void UnidentifiedHUD_801EB108::fn_801EB108(FEPresentation* presentation)
{
    for (int i = 0; i < 2; i++)
    {
        TLInstance* pPowerBarContainer = FEFinder<TLComponentInstance, 4>::Find<TLSlide>(
            presentation->m_currentSlide, LAYER_NAME, POWERBAR_CONTAINER_NAMES[i]);
        if (pPowerBarContainer == 0)
        {
            pPowerBarContainer = &UnidentifiedTLComponentDefault::sInstance;
        }
        mUnidentified08[i] = (TLComponentInstance*)pPowerBarContainer;
        TLComponentInstance* pComp = FEFinder<TLComponentInstance, 4>::Find<TLSlide>(
            presentation->m_currentSlide, LAYER_NAME, HUD_TEAM_NAMES[i]);
        mUnidentified20[i] = FEFinder<TLComponentInstance, 4>::Find<TLSlide>(
            pComp->GetActiveSlide(), HUD_NAMES[i], PAD_NAMES[0]);
        mUnidentified00[i] = FEFinder<TLComponentInstance, 4>::Find(
            mUnidentified20[i], "tank", "the metre", "metre");
        mUnidentified10[i] = FEFinder<TLImageInstance, 2>::Find(mUnidentified00[i], "white_8x8");
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
        mUnidentified10[i]->SetAssetColour(colour);
        TLInstance* pPowerBar = FEFinder<TLComponentInstance, 4>::Find<FEPresentation>(
            presentation, HUD_SLIDE_OUT_NAME, LAYER_NAME, POWERBAR_NAMES[i]);
        if (pPowerBar == 0)
        {
            pPowerBar = &UnidentifiedTLComponentDefault::sInstance;
        }
        pPowerBar->m_bVisible = false;
        if (mUnidentified00[i])
        {
            feVector3 scale = mUnidentified10[i]->GetScale();
            mUnidentified18[i] = scale.f.x;
        }
        else
        {
            mUnidentified18[i] = 0.0f;
        }
    }
}

void UnidentifiedHUD_801E9198::fn_801EB5CC(FEPresentation* presentation, UnidentifiedHUD_801E8CD0* textures)
{
    mUnidentified8C = textures;
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
                        pPowerUpImage = &UnidentifiedTLImageDefault::sInstance;
                    }
                    m_pImagePowerUps[1][team][i] = (TLImageInstance*)pPowerUpImage;
                }
                else
                {
                    m_pImagePowerUps[1][team][i] = m_pImagePowerUps[0][team][i];
                }
                mUnidentified60[team][i] = pComp;
            }
            mUnidentified70[team] = FEFinder<TLComponentInstance, 4>::Find<TLSlide>(
                pTeamComp->GetActiveSlide(), HUD_NAMES[team], "blinker");
            mUnidentified70[team]->m_bVisible = false;
            mUnidentified50[team][i] = FEFinder<TLComponentInstance, 4>::Find<TLSlide>(
                pTeamComp->GetActiveSlide(), HUD_NAMES[team], PAD_NAMES[i]);
            TLComponentInstance* pElectric = FEFinder<TLComponentInstance, 4>::Find(
                mUnidentified50[team][i], "get pup", "electric");
            TLComponentInstance* pFlare = FEFinder<TLComponentInstance, 4>::Find<TLSlide>(
                pElectric->GetActiveSlide(), "flare");
            if (pFlare)
            {
                m_pComponentFlares[team][i] = pFlare;
                mUnidentified20[team][i] = FEFinder<TLImageInstance, 2>::Find<TLSlide>(pFlare->GetActiveSlide(), "flare");
                mUnidentified20[team][i]->m_bVisible = false;
            }
            m_pPowerupTextComponents[team][i] = FEFinder<TLComponentInstance, 4>::Find<TLSlide>(
                pTeamComp->GetActiveSlide(), HUD_NAMES[team], POWER_UP_TEXT_NAMES[team][i]);
        }
    }
}

#include "Game/UnidentifiedStaticStorage.h"
