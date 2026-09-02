#include "Game/SH/SHCredits.h"

#include "Game/BasicStadium.h"
#include "Game/FE/feFinder.h"
#include "Game/FE/feInput.h"
#include "Game/FE/fePackage.h"
#include "Game/FE/feScene.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlTextInstance.h"
#include "NL/nlFile.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"

extern BaseGameSceneManager* lbl_806E1838;
extern TLComponentInstance lbl_80580030;

extern "C" void fn_801CBCA0(unsigned long hash, int value0, int value1, int value2);
extern "C" void fn_801FC2B4(int value);
extern "C" void fn_801FC3F0();
extern "C" void fn_80253284(int value);
extern "C" bool fn_80273B00();
extern "C" int fn_803693B4();
extern "C" void fn_80371198();

SceneList CreditScene::mNextScene = (SceneList)13;

inline TLComponentInstance* CreditScene::GetWhiteFadeComponent()
{
    TLComponentInstance* result = (TLComponentInstance*)FEFinder<TLComponentInstance, 2>::_Find(
        mPresentation->m_currentSlide, nlStringLowerHash("Layer"), nlStringLowerHash("WHITE FADE"), 0, 0, 0, 0);
    if (result == 0)
    {
        result = &lbl_80580030;
    }
    return result;
}

inline void CreditScene::UpdateForCopyrightMessage(float fDeltaT)
{
    TLComponentInstance* pWhiteFade = GetWhiteFadeComponent();
    mTimeElapsed += fDeltaT;
    if (mTimeElapsed >= 3.0f)
    {
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
}

inline void CreditScene::UpdateForNintendoLogo(float fDeltaT)
{
    TLComponentInstance* pWhiteFade = GetWhiteFadeComponent();
    mTimeElapsed += fDeltaT;
    if (mTimeElapsed >= 3.0f)
    {
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
}

CreditScene::CreditScene()
    : mAreCreditsOver(false)
    , mFinalMessageDisplayed(false)
    , mFadeStarted(false)
    , mPhase(0)
{
    fn_80253284(0);
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
    fn_80253284(1);
    BasicStadium* pStadium = BasicStadium::GetCurrentStadium();
    pStadium->mUnidentified070 = true;
}

void CreditScene::SceneCreated()
{
    SetupForPhase();
    fn_801FC3F0();
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
    case 1:
        MoviePlayerScene::Update(fDeltaT);
        break;
    case 0:
        BaseSceneHandler::Update(fDeltaT);
        UpdateForNintendoLogo(fDeltaT);
        break;
    }
}

void CreditScene::SetupForPhase()
{
    mFadeStarted = false;
    mTimeElapsed = 0.0f;

    switch (mPhase)
    {
    case 1:
        if (fn_80273B00())
        {
            mPresentation->SetActiveSlide("NLG", true);
        }
        else
        {
            mPresentation->SetActiveSlide("NLG 4:3", true);
        }
        if (fn_803693B4() == 1)
        {
            SetMovieDetails("art/movies/nlgintro_pal.thp", true, false);
        }
        else
        {
            SetMovieDetails("art/movies/nlgintrowide.thp", true, false);
        }
        BasicStadium::GetCurrentStadium()->mUnidentified070 = false;
        break;
    case 0:
        mPresentation->SetActiveSlide("NINTENDO", true);
        mPresentation->m_currentSlide->Update(0.0f);
        fn_801CBCA0(0xF394C076, 0, 0, 1);
        BasicStadium::GetCurrentStadium()->mUnidentified070 = false;
        break;
    case 2:
        SetupForCredits();
        BasicStadium::GetCurrentStadium()->mUnidentified070 = false;
        break;
    case 3:
        mPresentation->SetActiveSlide("COPYRIGHTS", true);
        mPresentation->m_currentSlide->Update(0.0f);
        BasicStadium::GetCurrentStadium()->mUnidentified070 = false;
        break;
    case 4:
        fn_801CBCA0(0xBB142B94, 0, 0, 1);
        lbl_806E1838->Push(mNextScene, SCREEN_NOTHING, true);
        if (mNextScene == (SceneList)13)
        {
            fn_801FC2B4(1);
        }
        else
        {
            fn_801FC2B4(0);
        }
        mNextScene = (SceneList)13;
        BasicStadium::GetCurrentStadium()->mUnidentified070 = true;
        break;
    default:
        BasicStadium::GetCurrentStadium()->mUnidentified070 = true;
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

void CreditScene::SetupForCredits()
{
    if (fn_803693B4() == 1)
    {
        SetMovieDetails("art/movies/credits_pal.thp", true, false);
    }
    else
    {
        SetMovieDetails("art/movies/credits.thp", true, false);
    }
    if (fn_80273B00())
    {
        mPresentation->SetActiveSlide("CREDITS", true);
    }
    else
    {
        mPresentation->SetActiveSlide("Credits 4:3", true);
    }
    mPresentation->Update(0.0f);

    FEPresentation* presentation = mFEScene->m_pFEPackage->GetPresentation();
    TLTextInstance* pFinalText = FEFinder<TLTextInstance, 3>::Find(presentation,
        nlStringLowerHash("CREDITS"),
        nlStringLowerHash("Layer"),
        nlStringLowerHash("Final Message"),
        0,
        0,
        0);
    pFinalText->m_bVisible = false;

    mCreditParser.mFileData = (char*)nlLoadEntireFile(
        "credits.txt", &mCreditParser.mFileSize, 0x20, AllocateEnd, 0, 0, 0);
    mCreditParser.mParser.StartParsing(
        mCreditParser.mFileData, mCreditParser.mFileSize, "\t\r\n");

    nlVector2 boxsize = { 1280.0f, 480.0f };
    int yOffset = 0;
    for (int i = 0; i < 20; ++i, yOffset += 25)
    {
        char lineName[8];
        nlSNPrintf(lineName, sizeof(lineName), "line%d", i + 1);
        m_pTextLines[i] = (TLTextInstance*)FEFinder<TLTextInstance, 2>::_Find(
            mPresentation->m_currentSlide, nlStringLowerHash("Layer"), nlStringLowerHash(lineName), 0, 0, 0, 0);

        m_pTextLines[i]->SetAssetScale(0.75f, 0.75f, 1.0f);
        m_pTextLines[i]->m_OverloadFlags |= 0x10;
        m_pTextLines[i]->m_DrawOptions |= 0x10;
        m_pTextLines[i]->m_DrawOptions &= ~0x1000;
        m_pTextLines[i]->m_OverloadedAttributes.BoxSize = boxsize;
        m_pTextLines[i]->m_OverloadFlags |= 0x4;

        feVector3 position = m_pTextLines[i]->GetAssetPosition();
        m_pTextLines[i]->SetAssetPosition(
            position.f.x, (float)(-250 - yOffset), position.f.z);
    }
}

static inline void CopyCreditLine(CreditScene& scene, int i, const char* pToken)
{
    if (pToken[0] == '+')
    {
        const unsigned char* pSrc = (const unsigned char*)" ";
        unsigned int count = 64;
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
        unsigned int count = 64;
        int k = 0;
        while (count-- && (scene.mStrings[i][k] = *pSrc) != 0)
        {
            ++pSrc;
            ++k;
        }
        scene.mStrings[i][63] = 0;
    }
}

void CreditScene::UpdateForCredits(float fDeltaT)
{
    MoviePlayerScene::Update(fDeltaT);

    float movement = 500.0f * (fDeltaT / 8.5f);
    float resetY = -250.0f;
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

            if (hasToken && mStrings[i][0] == '@')
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
                    position.f.x = -position.f.x;
                    mCenteredLine[i] = false;
                    m_pTextLines[i]->SetAssetPosition(
                        position.f.x, position.f.y, position.f.z);
                }
            }
        }
        else if (position.f.y >= 250.0f && mLineOnScreen[i])
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
        if (mAreCreditsOver)
        {
            mTimeElapsed += fDeltaT;
            if (mTimeElapsed >= 1.7 && !mFinalMessageDisplayed)
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
        fn_80371198();
    }
}
