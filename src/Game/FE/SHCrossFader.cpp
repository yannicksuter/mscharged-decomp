#include "Game/FE/SHCrossFader.h"
#include "Game/FE/FEAudio.h"
#include "Game/FE/fePresentation.h"
#include "Game/FE/feInput.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlInstance.h"
#include "Game/FE/tlSlide.h"
#include "NL/nlColour.h"
#include "NL/nlPrint.h"
#include "Game/FE/feFinder.h"
#include "Game/FE/feInlineHasher.h"
#include "Game/FE/feScene.h"
#include "Game/FE/fePackage.h"
#include "Game/Render/RLViewLayers.h"
#include "NL/nlMemory.h"
#include "NL/nlTask.h"

CrossFaderScene::CrossFaderScene()
    : mNumImages(0)
    , mCurrentImage(0)
    , mImageInstances(0)
    , mTimer(0.0f)
    , mAlpha(0.0f)
    , mFadeState(-1)
    , mFadeToBlackTimer(0.0f)
{
    mWidescreen = IsWidescreen();
}

CrossFaderScene::~CrossFaderScene()
{
    if (mImageInstances != 0)
    {
        delete[] mImageInstances;
        mImageInstances = 0;
    }
}

void CrossFaderScene::SceneCreated()
{
    char tempstring[64];
    nlColour colour;

    FEPresentation* presentation = mFEScene->m_pFEPackage->GetPresentation();
    mNumImages = 0;

    do
    {
        nlSNPrintf(tempstring, 64, "screen%d", mNumImages);
        TLImageInstance* image = FEFinder<TLImageInstance, 2>::Find(presentation, "Slide1", "Layer", tempstring);
        if (image == 0)
            break;
        mNumImages++;
    } while (true);

    mImageInstances = (TLInstance**)nlMalloc(mNumImages * sizeof(TLInstance*), 8, false);

    for (int i = 0; i < mNumImages; i++)
    {
        nlSNPrintf(tempstring, 64, "screen%d", i);
        mImageInstances[i] = FEFinder<TLImageInstance, 2>::Find(presentation, "Slide1", "Layer", tempstring);
        if (i == 0)
            nlColourSet(colour, 0, 0, 0, 255);
        else
            nlColourSet(colour, 255, 255, 255, 0);
        mImageInstances[i]->SetAssetColour(colour);
    }

    mCurrentImageInstance = FEFinder<TLImageInstance, 2>::Find(presentation, "Slide1", "Layer", "whitebackground");
    mFadeState = 0;
    mCurrentImage = 0;
    FEFinder<TLImageInstance, 2>::Find(presentation, "Slide1", "Layer", "whitebackground2")->m_bVisible = false;
    mHomeMessage = FEFinder<TLComponentInstance, 4>::Find(presentation, "Slide1", "Layer", "no home");
    mHomeMessage->m_bVisible = false;
    if (mWidescreen)
        mHomeMessage->SetActiveSlide("widescreen", true, false);
}

void CrossFaderScene::Update(float fDeltaT)
{
    const nlColour colWhite = { { 255, 255, 255, 255 } };
    const nlColour colTransparent = { { 255, 255, 255, 0 } };
    BaseSceneHandler::Update(fDeltaT);
    if (mFadeState != 4 && g_pFEInput->JustPressed((eFEINPUT_PAD)8, 0x2E, true, 0))
    {
        TLSlide* slide = mHomeMessage->GetActiveSlide();
        if (!mHomeMessage->m_bVisible || slide->GetCurrentTime() == slide->GetStartTime() + slide->GetDuration())
        {
            mHomeMessage->m_bVisible = true;
            if (mWidescreen)
                mHomeMessage->SetActiveSlide("widescreen", true, false);
            else
                mHomeMessage->SetActiveSlide("Slide1", true, false);
        }
    }

    switch (mFadeState)
    {
    case 0:
        for (int i = 0; i < mNumImages; i++)
        {
            if (i == 0)
                mImageInstances[i]->SetAssetColour(colWhite);
            else
                mImageInstances[i]->SetAssetColour(colTransparent);
        }
        mCurrentImageInstance->SetAssetColour(colWhite);
        mAlpha = 255.0f;
        mFadeState = 1;
        break;
    case 1:
    {
        int speed = 510;
        mAlpha = mAlpha - speed * fDeltaT;
        if (mAlpha <= 0.0f)
        {
            mAlpha = 0.0f;
            mFadeState = 2;
        }
        nlColour colour = { { 255, 255, 255, 0 } };
        colour.c[3] = (u8)(int)mAlpha;
        mCurrentImageInstance->SetAssetColour(colour);
        break;
    }
    case 2:
    {
        static bool triggeraudioload = true;
        if (triggeraudioload)
        {
            switch (mCurrentImage)
            {
            case 2:
                break;
            case 0:
                FEAudio::PlayAnimAudioEvent(0xF394C076, 0, 0, 1);
                break;
            case 1:
                FEAudio::PlayAnimAudioEvent(0xDE83984E, 0, 0, 1);
                break;
            }
            triggeraudioload = false;
        }
        mTimer += fDeltaT;
        if (mTimer >= 2.0f)
        {
            if (mCurrentImage < mNumImages - 1)
            {
                mFadeState = 3;
            }
            else
            {
                mFadeState = 4;
                for (int i = 0; i < mNumImages - 1; ++i)
                    mImageInstances[i]->m_bVisible = false;
                mCurrentImageInstance->m_bVisible = false;
            }
            triggeraudioload = true;
            mTimer = 0.0f;
            mAlpha = 0.0f;
        }
        break;
    }
    case 3:
    {
        int speed = 849;
        mAlpha = mAlpha + speed * fDeltaT;
        if (mAlpha >= 255.0f)
            mAlpha = 255.0f;
        nlColour colour = { { 255, 255, 255, 0 } };
        colour.c[3] = (u8)(int)mAlpha;
        mCurrentImageInstance->SetAssetColour(colour);
        // R4QE01 loads this threshold ahead of the alpha reload, which under
        // GC/3.0a5 only a value defined after the SetAssetColour call does;
        // the identical test in state 4 uses the literal directly.
        float maxAlpha = 255.0f;
        if (mAlpha >= maxAlpha)
        {
            mImageInstances[mCurrentImage]->SetAssetColour(colTransparent);
            mCurrentImage++;
            mImageInstances[mCurrentImage]->SetAssetColour(colWhite);
            mFadeState = 1;
        }
        break;
    }
    case 4:
    {
        int speed = 1020;
        mAlpha = mAlpha + speed * fDeltaT;
        if (mAlpha >= 255.0f)
            mAlpha = 255.0f;
        nlColour colour = { { 255, 255, 255, 0 } };
        colour.c[3] = (u8)(int)(255.0f - mAlpha);
        mImageInstances[mCurrentImage]->SetAssetColour(colour);
        if (mAlpha >= 255.0f)
        {
            mFadeToBlackTimer += fDeltaT;
            if (mFadeToBlackTimer >= 0.2f)
            {
                nlTaskManager::SetNextState(0x00080000);
                mFadeToBlackTimer = 0.0f;
            }
        }
        break;
    }
    }
}

#include "Game/FE/feFinder_impl.h"
