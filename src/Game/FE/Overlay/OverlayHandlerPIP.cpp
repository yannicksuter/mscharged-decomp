#include "Game/FE/Overlay/OverlayHandlerPIP.h"

#include "Game/NisPlayer.h"
#include "Game/FE/feFinder.inl"
#include "Game/FE/fePackage.h"
#include "Game/FE/feScene.h"
#include "Game/FE/feTextureResource.h"
#include "Game/FE/tlImageInstance.h"
#include "Game/Render/RLViewLayers.h"
#include "NL/gl/glState.h"

PIPOverlay::PIPOverlay()
    : BaseOverlayHandler(0x10, POSITION_BOTTOM_RIGHT)
    , mInactive(true)
    , mTimeRemaining(0.0f)
{
}

PIPOverlay::~PIPOverlay()
{
}

void PIPOverlay::SceneCreated()
{
}

void PIPOverlay::Update(float dt)
{
    FEPresentation* presentation = mFEScene->m_pFEPackage->GetPresentation();
    if (NisPlayer::Instance()->fn_8027E64C()
        && NisPlayer::Instance()->IsPIPOverlayMode() == true)
    {
        if (mInactive == true)
        {
            SetVisible(true);
            mInactive = false;
            if (IsWidescreen())
                presentation->SetActiveSlide("16:9", true);
            else
                presentation->SetActiveSlide("4:3", true);
            mTimeRemaining = NisPlayer::Instance()->fn_8027C064(1);
        }
        else if (mTimeRemaining > 0.0f)
        {
            mTimeRemaining -= dt;
            if (mTimeRemaining <= presentation->m_currentSlide->GetDuration())
            {
                mTimeRemaining = 0.0f;
                if (IsWidescreen())
                    presentation->SetActiveSlide("16:9 out", true);
                else
                    presentation->SetActiveSlide("4:3 out", true);
            }
        }
        TLImageInstance* image = FEFinder<TLImageInstance, TLAT_IMAGE>::Find<>(presentation->m_currentSlide, "Layer", "pip");
        unsigned long texture = glGetTexture("target/pip");
        image->m_pTextureResource->SetTextureHandle(texture);
    }
    else if (!mInactive)
    {
        mInactive = true;
        SetVisible(false);
    }
    BaseSceneHandler::Update(dt);
}
