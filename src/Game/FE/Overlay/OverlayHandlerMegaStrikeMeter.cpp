#include "Game/FE/Overlay/OverlayHandlerMegaStrikeMeter.h"

#include "Game/FE/tlComponentInstance.h"
#include "NL/nlMath.h"

#include "Game/BaseGameSceneManager.h"
#include "Game/FE/feFinder.inl"
#include "Game/FE/fePackage.h"
#include "Game/FE/feScene.h"
#include "NL/nlTask.h"

extern "C" float fn_800499EC(cFielder*, int);

static const char* sNumberSlides[10] = {
    "01",
    "02",
    "03",
    "04",
    "05",
    "06",
    "07",
    "08",
    "09",
    "10",
};

MegaStrikeMeterOverlay::MegaStrikeMeterOverlay()
    : BaseOverlayHandler(3, POSITION_ALL)
    , mFielder(0)
    , mRestoreVisibility(false)
    , mAlignLeft(true)
{
}

MegaStrikeMeterOverlay::~MegaStrikeMeterOverlay()
{
}

void MegaStrikeMeterOverlay::Update(float dt)
{
    BaseSceneHandler::Update(dt);
    if (mFielder != 0)
    {
        float value = fn_800499EC(mFielder, 1);
        mNumbers->SetActiveSlide(sNumberSlides[(int)(value - 1.0f)], true, false);
        mNumbers->m_bVisible = true;
    }
    if (nlTaskManager::m_pInstance->mCurrentState == 1
        && g_pOverlayManager->IsOnStack((SceneList)81))
    {
        if (mVisible == true)
        {
            SetVisible(false);
            mRestoreVisibility = true;
        }
    }
    else if (mRestoreVisibility == true)
    {
        SetVisible(true);
        mRestoreVisibility = false;
    }
}

void MegaStrikeMeterOverlay::SceneCreated()
{
    FEPresentation* presentation = mFEScene->m_pFEPackage->GetPresentation();
    mNumbers = FEFinder<TLComponentInstance, TLAT_COMPONENT>::Find<>(presentation->m_currentSlide, "Layer", "numbers");
    mBackground = FEFinder<TLComponentInstance, TLAT_COMPONENT>::Find<>(presentation->m_currentSlide, "Layer", "centre_value_background");
    mNumbers->SetAssetScale(0.65f, 0.65f, 1.0f);
    mBackground->SetAssetScale(0.65f, 0.65f, 1.0f);
}

void MegaStrikeMeterOverlay::SetFirstResult(float meterValue)
{
    mNumbers->SetActiveSlide(sNumberSlides[(int)(meterValue - 1.0f)], true, false);
    mNumbers->m_bVisible = true;
    mBackground->SetActiveSlide("green", true, false);
    mFielder = 0;
}

void MegaStrikeMeterOverlay::SetSecondResult(float)
{
    mBackground->SetActiveSlide("green", true, false);
}

void MegaStrikeMeterOverlay::Start(cFielder* fielder)
{
    mFielder = fielder;
    mNumbers->m_bVisible = false;
    mBackground->SetActiveSlide("slide1", true, false);
    mMegaStrikeStarted = false;
    mAlignLeft = true;
}

void MegaStrikeMeterOverlay::SetPosition(nlVector3 position)
{
    if (mAlignLeft == true)
    {
        mNumbers->SetAssetPosition(position.x - 80.0f, 50.0f + position.y, 0.0f);
        mBackground->SetAssetPosition(position.x - 80.0f, 50.0f + position.y, 0.0f);
    }
    else
    {
        mNumbers->SetAssetPosition(80.0f + position.x, 50.0f + position.y, 0.0f);
        mBackground->SetAssetPosition(80.0f + position.x, 50.0f + position.y, 0.0f);
    }
}
