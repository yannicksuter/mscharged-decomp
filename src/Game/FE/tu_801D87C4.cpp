#include "Game/SH/SHNavigation.h"
#include "Game/FE/feOptionsSubMenus.h"
#include "Game/FE/FEAudio.h"

#include "Game/DB/SaveLoad.h"
#include "Game/DB/UserOptions.h"
#include "Game/FE/fePresentation.h"
#include "Game/FE/feDPD.h"
#include "Game/FE/feInput.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlInstance.h"
#include "Game/FE/tlSlide.h"
#include "Game/BaseGameSceneManager.h"
#include "Game/GameSceneManager.h"
#include "Game/GameInfo.h"
#include "NL/nlColour.h"
#include "NL/nlConfig.h"
#include "NL/nlFormat.h"
#include "NL/nlLocalizationLookup.h"
#include "NL/nlPrint.h"
#include "Game/FE/feFinder.h"
#include "Game/FE/feInlineHasher.h"
#include "Game/FE/feFinder_impl.h"
#include "Game/FE/tlTextInstance.h"
#include "Game/FE/feHelpFuncs_decl.h"
#include "Game/FE/feScene.h"
#include "Game/FE/fePackage.h"
#include "Game/Render/RLViewLayers.h"
#include "NL/nlFunction.inl"
#include "NL/nlBind.h"
#include "NL/nlMemory.h"
#include "NL/nlTask.h"
#include <string.h>

Class_801D87C4::Class_801D87C4()
    : mScreenCount(0)
    , mCurrentScreen(0)
    , mScreens(0)
    , mScreenTime(0.0f)
    , mFadeAlpha(0.0f)
    , mState(-1)
    , mEndTime(0.0f)
{
    mWidescreen = IsWidescreen();
}

Class_801D87C4::~Class_801D87C4()
{
    if (mScreens != 0)
    {
        delete[] mScreens;
        mScreens = 0;
    }
}

void Class_801D87C4::SceneCreated()
{
    char buf[64];
    nlColour colour;

    FEPresentation* pPresentation = mFEScene->m_pFEPackage->GetPresentation();
    mScreenCount = 0;

    do
    {
        nlSNPrintf(buf, 64, "screen%d", mScreenCount);
        TLImageInstance* found = FEFinder<TLImageInstance, 2>::Find(pPresentation, "Slide1", "Layer", buf);
        if (found == 0)
            break;
        mScreenCount++;
    } while (true);

    mScreens = (TLInstance**)nlMalloc(mScreenCount * sizeof(TLInstance*), 8, false);

    for (int i = 0; i < mScreenCount; i++)
    {
        nlSNPrintf(buf, 64, "screen%d", i);
        mScreens[i] = FEFinder<TLImageInstance, 2>::Find(pPresentation, "Slide1", "Layer", buf);
        if (i == 0)
            nlColourSet(colour, 0, 0, 0, 255);
        else
            nlColourSet(colour, 255, 255, 255, 0);
        mScreens[i]->SetAssetColour(colour);
    }

    mBackground = FEFinder<TLImageInstance, 2>::Find(pPresentation, "Slide1", "Layer", "whitebackground");
    mState = 0;
    mCurrentScreen = 0;
    FEFinder<TLImageInstance, 2>::Find(pPresentation, "Slide1", "Layer", "whitebackground2")->m_bVisible = false;
    mHomeMessage = FEFinder<TLComponentInstance, 4>::Find(pPresentation, "Slide1", "Layer", "no home");
    mHomeMessage->m_bVisible = false;
    if (mWidescreen)
        mHomeMessage->SetActiveSlide("widescreen", true, false);
}

void Class_801D87C4::Update(float dt)
{
    const nlColour white = { { 255, 255, 255, 255 } };
    const nlColour clear = { { 255, 255, 255, 0 } };
    BaseSceneHandler::Update(dt);
    if (mState != 4 && g_pFEInput->JustPressed((eFEINPUT_PAD)8, 0x2E, true, 0))
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

    switch (mState)
    {
    case 0:
        for (int i = 0; i < mScreenCount; i++)
        {
            if (i == 0)
                mScreens[i]->SetAssetColour(white);
            else
                mScreens[i]->SetAssetColour(clear);
        }
        mBackground->SetAssetColour(white);
        mFadeAlpha = 255.0f;
        mState = 1;
        break;
    case 1:
    {
        int speed = 510;
        mFadeAlpha = mFadeAlpha - speed * dt;
        if (mFadeAlpha <= 0.0f)
        {
            mFadeAlpha = 0.0f;
            mState = 2;
        }
        nlColour col = { { 255, 255, 255, 0 } };
        col.c[3] = (u8)(int)mFadeAlpha;
        mBackground->SetAssetColour(col);
        break;
    }
    case 2:
    {
        static bool triggeraudioload = true;
        if (triggeraudioload)
        {
            switch (mCurrentScreen)
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
        mScreenTime += dt;
        if (mScreenTime >= 2.0f)
        {
            if (mCurrentScreen < mScreenCount - 1)
            {
                mState = 3;
            }
            else
            {
                mState = 4;
                for (int i = 0; i < mScreenCount - 1; ++i)
                    mScreens[i]->m_bVisible = false;
                mBackground->m_bVisible = false;
            }
            triggeraudioload = true;
            mScreenTime = 0.0f;
            mFadeAlpha = 0.0f;
        }
        break;
    }
    case 3:
    {
        int speed = 849;
        mFadeAlpha = mFadeAlpha + speed * dt;
        if (mFadeAlpha >= 255.0f)
            mFadeAlpha = 255.0f;
        nlColour col = { { 255, 255, 255, 0 } };
        col.c[3] = (u8)(int)mFadeAlpha;
        mBackground->SetAssetColour(col);
        // R4QE01 loads this threshold ahead of the alpha reload, which under
        // GC/3.0a5 only a value defined after the SetAssetColour call does;
        // the identical test in state 4 uses the literal directly.
        float maxAlpha = 255.0f;
        if (mFadeAlpha >= maxAlpha)
        {
            mScreens[mCurrentScreen]->SetAssetColour(clear);
            mCurrentScreen++;
            mScreens[mCurrentScreen]->SetAssetColour(white);
            mState = 1;
        }
        break;
    }
    case 4:
    {
        int speed = 1020;
        mFadeAlpha = mFadeAlpha + speed * dt;
        if (mFadeAlpha >= 255.0f)
            mFadeAlpha = 255.0f;
        nlColour col = { { 255, 255, 255, 0 } };
        col.c[3] = (u8)(int)(255.0f - mFadeAlpha);
        mScreens[mCurrentScreen]->SetAssetColour(col);
        if (mFadeAlpha >= 255.0f)
        {
            mEndTime += dt;
            if (mEndTime >= 0.2f)
            {
                nlTaskManager::SetNextState(0x00080000);
                mEndTime = 0.0f;
            }
        }
        break;
    }
    }
}
