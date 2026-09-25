#include "Game/SH/SHNavigation.h"
#include "Game/GameSceneManager.h"
#include "Game/SH/SHCredits.h"
#include "Game/Render/RLViewLayers.h"
#include "Game/FE/FEAudio.h"

#include "Game/BasicStadium.h"
#include "Game/FE/feFinder.h"
#include "Game/FE/feFinder.inl"
#include "Game/FE/feInput.h"
#include "Game/FE/feMusic.h"
#include "Game/FE/fePackage.h"
#include "Game/FE/feScene.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlTextInstance.h"
#include "Game/Sys/movie.h"
#include "NL/gl/glPlat.h"
#include "NL/nlFile.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"
#include "Game/FE/FEAudio.h"
#include "Game/Render/RLViewLayers.h"
#include "Game/SH/SHNavigation.h"



SceneList CreditScene::mNextScene = (SceneList)13;

CreditScene::CreditScene()
    : mAreCreditsOver(false)
    , mFinalMessageDisplayed(false)
    , mFadeStarted(false)
    , mPhase(0)
{
    SetPointerEnabled(0);
    mTimeElapsed = 0.0f;

    for (int i = 0; i < 20; ++i)
    {
        m_pTextLines[i] = 0;
        mLineOnScreen[i] = false;
        mCenteredLine[i] = false;
    }
}

CreditScene::~CreditScene()
{
    SetPointerEnabled(1);
    BasicStadium* pStadium = BasicStadium::GetCurrentStadium();
    pStadium->m_bRenderingEnabled = true;
}

void CreditScene::SceneCreated()
{
    SetupForPhase();
    FEMusic::StopStream();
}

void CreditScene::Update(float fDeltaT)
{
    switch (mPhase)
    {
    case 2:
        UpdateForCredits(fDeltaT);
        break;
    case 3:
        BaseSceneHandler::Update(fDeltaT);
        UpdateForCopyrightMessage(fDeltaT);
        break;
    case 0:
        BaseSceneHandler::Update(fDeltaT);
        UpdateForNintendoLogo(fDeltaT);
        break;
    case 1:
        MoviePlayerScene::Update(fDeltaT);
        break;
    }
}

void CreditScene::DisplayFinalMessage()
{
    FEPresentation* presentation = mFEScene->m_pFEPackage->GetPresentation();
    TLTextInstance* pText = FEFinder<TLTextInstance, 3>::Find(presentation, "CREDITS", "Layer", "Final Message");
    pText->m_bVisible = true;

    mFinalMessageDisplayed = true;
}

void CreditScene::SetupForPhase()
{
    mFadeStarted = false;
    mTimeElapsed = 0.0f;

    switch (mPhase)
    {
    case 1:
        if (IsWidescreen())
        {
            mPresentation->SetActiveSlide("NLG", true);
        }
        else
        {
            mPresentation->SetActiveSlide("NLG 4:3", true);
        }
        if (glx_GetVideoMode() == 1)
        {
            SetMovieDetails("art/movies/nlgintro_pal.thp", true, false);
        }
        else
        {
            SetMovieDetails("art/movies/nlgintrowide.thp", true, false);
        }
        {
            BasicStadium* pStadium = BasicStadium::GetCurrentStadium();
            pStadium->m_bRenderingEnabled = false;
        }
        break;
    case 0:
        mPresentation->SetActiveSlide("NINTENDO", true);
        mPresentation->m_currentSlide->Update(0.0f);
        FEAudio::PlayAnimAudioEvent(0xF394C076, 0, 0, 1);
        BasicStadium::GetCurrentStadium()->m_bRenderingEnabled = false;
        break;
    case 2:
        SetupForCredits();
        BasicStadium::GetCurrentStadium()->m_bRenderingEnabled = false;
        break;
    case 3:
        mPresentation->SetActiveSlide("COPYRIGHTS", true);
        mPresentation->m_currentSlide->Update(0.0f);
        BasicStadium::GetCurrentStadium()->m_bRenderingEnabled = false;
        break;
    case 4:
        FEAudio::PlayAnimAudioEvent(0xBB142B94, 0, 0, 1);
        GameSceneManager::Instance()->Push(mNextScene, SCREEN_NOTHING, true);
        if (mNextScene == (SceneList)13)
        {
            FEMusic::StartStreamIfDifferent(1);
        }
        else
        {
            FEMusic::StartStreamIfDifferent(0);
        }
        mNextScene = (SceneList)13;
        {
            BasicStadium* pStadium = BasicStadium::GetCurrentStadium();
            pStadium->m_bRenderingEnabled = true;
        }
        break;
    default:
        {
            BasicStadium* pStadium = BasicStadium::GetCurrentStadium();
            pStadium->m_bRenderingEnabled = true;
        }
        break;
    }
}

void CreditScene::MoviePlayerVirtual3C()
{
    if ((unsigned int)(mPhase - 1) <= 1)
    {
        ++mPhase;
        SetupForPhase();
    }
}

inline void CopyCreditLine(CreditScene& scene, int i, const char* pToken)
{
    if (pToken[0] == '+')
    {
        const unsigned char* pSrc;
        unsigned int count;
        count = 64;
        pSrc = (const unsigned char*)" ";
        int k = 0;
        while (count-- && (scene.mStrings[i][k] = *pSrc) != 0)
        {
            ++pSrc;
            ++k;
        }
        scene.mStrings[i][63] = 0;
    }
    else
    {
        const unsigned char* pSrc = (const unsigned char*)pToken;
        int ch;
        unsigned int count;
        count = 64;
        ch = 0;
        while (count-- && (scene.mStrings[i][ch] = pSrc[ch]) != 0)
        {
            ++ch;
        }
        scene.mStrings[i][63] = 0;
    }
}

inline void CreditScene::CreditParser::Load()
{
    mFileData = (char*)nlLoadEntireFile("credits.txt", &mFileSize, 0x20, AllocateEnd, 0, 0, 0);
    mParser.StartParsing(mFileData, mFileSize, "\t\r\n");
}

void CreditScene::SetupForCredits()
{
    if (glx_GetVideoMode() == 1)
    {
        SetMovieDetails("art/movies/credits_pal.thp", true, false);
    }
    else
    {
        SetMovieDetails("art/movies/credits.thp", true, false);
    }
    if (IsWidescreen())
    {
        mPresentation->SetActiveSlide("CREDITS", true);
    }
    else
    {
        mPresentation->SetActiveSlide("Credits 4:3", true);
    }
    mPresentation->Update(0.0f);

    FEPresentation* presentation = mFEScene->m_pFEPackage->GetPresentation();
    TLTextInstance* pFinalText = FEFinder<TLTextInstance, 3>::Find(presentation, "CREDITS", "Layer", "Final Message");
    pFinalText->m_bVisible = false;

    mCreditParser.Load();

    nlVector2 boxsize = { 1280.0f, 480.0f };

    for (int i = 0; i < 20; ++i)
    {
        char lineName[8];
        nlSNPrintf(lineName, sizeof(lineName), "line%d", i + 1);
        m_pTextLines[i] = FEFindTextInstance(presentation->m_currentSlide, "Layer", lineName);

        m_pTextLines[i]->SetAssetScale(0.75f, 0.75f, 1.0f);

        TLTextInstance* pText = m_pTextLines[i];
        pText->m_OverloadFlags |= 0x10;
        pText->m_DrawOptions |= 0x10;
        pText->m_DrawOptions &= ~0x1000;

        pText = m_pTextLines[i];
        pText->m_OverloadedAttributes.BoxSize = boxsize;
        pText->m_OverloadFlags |= 0x4;

        feVector3 position = m_pTextLines[i]->GetAssetPosition();
        m_pTextLines[i]->SetAssetPosition(
            position.f.x, (float)(-250 - i * 25), position.f.z);
    }
}

void CreditScene::UpdateForCredits(float fDeltaT)
{
    MoviePlayerScene::Update(fDeltaT);

    float fraction = fDeltaT / 8.5f;
    float movement = 500.0f * fraction;
    const float resetY = -250.0f;
    const float topY = 250.0f;
    int numonscreen = 0;

    for (int i = 0; i < 20; ++i)
    {
        feVector3 position = m_pTextLines[i]->GetAssetPosition();
        if (position.f.y >= resetY && !mLineOnScreen[i])
        {
            bool hasToken;
            const char* pToken = mCreditParser.mParser.NextToken(false);
            if (pToken != 0)
            {
                CopyCreditLine(*this, i, pToken);
                mCreditParser.mParser.AdvanceLine();
                hasToken = true;
            }
            else
            {
                hasToken = false;
            }

            if (hasToken && mStrings[i][0] == L'@')
            {
                for (int j = 0; j < 20; ++j)
                {
                    mCenteredLine[j] = true;
                }
                pToken = mCreditParser.mParser.NextToken(false);
                if (pToken != 0)
                {
                    CopyCreditLine(*this, i, pToken);
                    mCreditParser.mParser.AdvanceLine();
                    hasToken = true;
                }
                else
                {
                    hasToken = false;
                }
            }

            if (hasToken)
            {
                m_pTextLines[i]->SetString(mStrings[i]);
                mLineOnScreen[i] = true;
                position.f.y += movement;
                m_pTextLines[i]->SetAssetPosition(
                    position.f.x, position.f.y, position.f.z);
                if (mCenteredLine[i])
                {
                    m_pTextLines[i]->m_DrawOptions = 0;
                    mCenteredLine[i] = false;
                    m_pTextLines[i]->SetAssetPosition(
                        -position.f.x, position.f.y, position.f.z);
                }
            }
        }
        else if (position.f.y >= topY && mLineOnScreen[i] == true)
        {
            mLineOnScreen[i] = false;
            position.f.y = resetY;
            m_pTextLines[i]->SetAssetPosition(
                position.f.x, position.f.y, position.f.z);
        }
        else
        {
            position.f.y += movement;
            m_pTextLines[i]->SetAssetPosition(
                position.f.x, position.f.y, position.f.z);
        }

        if (mLineOnScreen[i])
        {
            ++numonscreen;
        }
    }

    if (numonscreen == 0)
    {
        mAreCreditsOver = true;
    }

    if (!mFadeStarted)
    {
        bool quitcredits = false;
        if (mAreCreditsOver == true)
        {
            mTimeElapsed += fDeltaT;
            if (mTimeElapsed >= (double)1.7f && !mFinalMessageDisplayed)
            {
                quitcredits = true;
            }
        }
        else if (g_pFEInput->JustPressed(FE_ALL_PADS, 0x20, true, 0)
                 || g_pFEInput->JustPressed(FE_ALL_PADS, 0x1E, true, 0))
        {
            quitcredits = true;
        }

        if (quitcredits)
        {
            mFadeStarted = true;
            TLComponentInstance* pWhiteFade = GetWhiteFadeComponent();
            pWhiteFade->SetActiveSlide("FADEIN", true, false);
            pWhiteFade->Update(0.0f);
        }
    }
    else
    {
        TLComponentInstance* pWhiteFade = GetWhiteFadeComponent();
        if (mCreditParser.mFileData != 0)
        {
            nlFree(mCreditParser.mFileData);
            mCreditParser.mFileData = 0;
        }
        ++mPhase;
        SetupForPhase();
        MovieStop();
    }
}

void CreditScene::UpdateForCopyrightMessage(float fDeltaT)
{
    TLComponentInstance* pWhiteFade = GetWhiteFadeComponent();
    mTimeElapsed += fDeltaT;
    if (mTimeElapsed < 3.0f)
    {
        return;
    }
    if (!mFadeStarted)
    {
        pWhiteFade->SetActiveSlide("FADEIN", true, false);
        mFadeStarted = true;
    }
    else
    {
        ++mPhase;
        SetupForPhase();
    }
}

void CreditScene::UpdateForNintendoLogo(float fDeltaT)
{
    TLComponentInstance* pWhiteFade = GetWhiteFadeComponent();
    mTimeElapsed += fDeltaT;
    if (mTimeElapsed < 3.0f)
    {
        return;
    }
    if (!mFadeStarted)
    {
        pWhiteFade->SetActiveSlide("FADEIN", true, false);
        mFadeStarted = true;
    }
    else
    {
        ++mPhase;
        SetupForPhase();
    }
}

TLComponentInstance* CreditScene::GetWhiteFadeComponent()
{
    TLComponentInstance* result = FEFinder<TLComponentInstance, 2>::Find(
        mPresentation->m_currentSlide, nlStringLowerHash("Layer"), nlStringLowerHash("WHITE FADE"), 0, 0, 0, 0);
    if (result == 0)
    {
        result = &UnidentifiedTLComponentDefault::sInstance;
    }
    return result;
}
