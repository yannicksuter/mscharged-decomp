#include "Game/FE/Overlay/OverlayHandlerSuperAbility.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/Task/FixedUpdateTask.h"
#include "Game/FE/feFinder.inl"
#include "Game/FE/feHelpFuncs_decl.h"
#include "Game/Render/RLViewLayers.h"
#include "NL/nlPrint.h"

float gSuperAbilityDisplayTime = 2.0f;
static const char* sSuperAbilityResource = "art/fe/superabilityUI.res";
eTeamID gSuperAbilityTeam;

SuperAbilityOverlay::SuperAbilityOverlay()
    : BaseOverlayHandler(1, POSITION_ALL)
    , mText(0)
    , mImage(sSuperAbilityResource, 0)
    , mImageLoaded(false)
{
}

SuperAbilityOverlay::~SuperAbilityOverlay()
{
}

void SuperAbilityOverlay::SetVisible(bool visible)
{
    bool frameLocked = GetFixedUpdateTask()->mfFrameLockTime > 0.0f;
    if (!frameLocked && nlTaskManager::m_pInstance->mCurrentState != 2
        && nlTaskManager::m_pInstance->mPendingState != 2)
    {
        mVisible = visible;
    }
}

void SuperAbilityOverlay::Update(float dt)
{
    BaseSceneHandler::Update(dt);
    if (mTimeRemaining > 0.0f)
    {
        mTimeRemaining -= dt;
        if (mTimeRemaining <= 0.0f)
        {
            mTimeRemaining = 0.0f;
            mVisible = false;
        }
    }
    if (!mImageLoaded)
    {
        mImageLoaded = mImage.Update(true);
        if (mImageLoaded)
        {
            mPresentation->m_fadeDuration = mPresentation->m_currentSlide->m_start;
            FEFinder<TLImageInstance, TLAT_IMAGE>::Find<>(mPresentation->m_currentSlide, "Layer", "00_dummy_texture");
            mVisible = true;
        }
    }
}

void SuperAbilityOverlay::SceneCreated()
{
    if (IsWidescreen())
        mPresentation->SetActiveSlide("Slide1", true);
    else
        mPresentation->SetActiveSlide("4:3", true);
    mText = FEFinder<TLComponentInstance, TLAT_COMPONENT>::Find<>(mPresentation->m_currentSlide, "Layer", "TEXT");
    mVisible = false;
}

void SuperAbilityOverlay::Start()
{
    mVisible = false;
    mTimeRemaining = gSuperAbilityDisplayTime;
    mImageLoaded = false;
    mText->SetActiveSlide(GetTeamName(gSuperAbilityTeam), true, false);
    mImage.SetImageInstance(FEFinder<TLImageInstance, TLAT_IMAGE>::Find<>(mPresentation->m_currentSlide, "Layer", "00_dummy_texture"));
    char path[64];
    nlSNPrintf(path, sizeof(path), "fe/super_abilities/%s_super_image", GetTeamName(gSuperAbilityTeam));
    mImage.QueueLoad(path, false);
}
