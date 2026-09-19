#include <revolution/sc.h>
#include "Game/FE/fePresentation.inl"
#include "Game/Audio/RegistryPools.h"

#include "Game/SH/SHBootLoading.h"
#include "Game/Render/RLViewLayers.h"
#include "Game/FE/FEAudio.h"

#include "Game/Audio/AudioBankTable.h"
#include "Game/Audio/AudioBundleManager.h"
#include "Game/Audio/AudioSystem.h"
#include "Game/FE/feFinder.inl"
#include "Game/FE/feInput.h"
#include "Game/FE/feScene.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlImageInstance.h"
#include "Game/main.h"
#include "NL/nlColour.h"
#include "NL/nlLocalization.h"
#include "NL/nlString.h"
#include "Game/FE/FEAudio.h"
#include "Game/Render/RLViewLayers.h"
#include "Game/FE/UnidentifiedTLDefault.h"

BootLoadingScene::BootLoadingScene()
    : mElapsedTime(0.0f)
    , mStrapAlpha(255.0f)
    , mStrapDismissed(false)
    , mHomeButtonWarning(0)
    , mHomeButtonWarningActive(false)
    , mWidescreen(false)
{
    mPhase = 1;
}

BootLoadingScene::~BootLoadingScene()
{
}

void BootLoadingScene::Update(float fDeltaT)
{
    BaseSceneHandler::Update(fDeltaT);
    if (mPhase == 0)
    {
        TLSlide* slide = mPresentation->m_currentSlide;
        if (slide->GetCurrentTime() >= slide->GetStartTime() + slide->GetDuration())
        {
            mPhase = 3;
            mElapsedTime = 0.0f;
            SetPhaseSlide();
        }
    }
    else if (mPhase == 1)
    {
        if (mStrapDismissed)
        {
            int fadeRate = 2 * 255;
            mStrapAlpha -= fadeRate * fDeltaT;
            if (mStrapAlpha <= 0.0f)
                mStrapAlpha = 0.0f;
            nlColour colour = { 255, 255, 255, 0 };
            colour.c[3] = (unsigned char)mStrapAlpha;
            mStrapImage->SetAssetColour(colour);
            if (mStrapAlpha <= 0.0f)
            {
                mElapsedTime = 0.0f;
                mPhase = 2;
                SetPhaseSlide();
            }
        }
        else
        {
            mElapsedTime += fDeltaT;
            if (mElapsedTime >= 1.5f)
            {
                if (g_pFEInput->JustPressed((eFEINPUT_PAD)8, 0x20, true, 0)
                    || g_pFEInput->JustPressed((eFEINPUT_PAD)8, 0x08, true, 0)
                    || g_pFEInput->JustPressed((eFEINPUT_PAD)8, 0x1E, true, 0)
                    || g_pFEInput->JustPressed((eFEINPUT_PAD)8, 0x1F, true, 0)
                    || g_pFEInput->JustPressed((eFEINPUT_PAD)8, 0x2A, true, 0)
                    || g_pFEInput->JustPressed((eFEINPUT_PAD)8, 0x2B, true, 0)
                    || g_pFEInput->JustPressed((eFEINPUT_PAD)8, 0x28, true, 0)
                    || g_pFEInput->JustPressed((eFEINPUT_PAD)8, 0x29, true, 0))
                    mStrapDismissed = true;
                if (mElapsedTime >= 15.5f)
                    mStrapDismissed = true;
            }
        }
    }
    else if (mPhase == 2)
    {
        TLSlide* slide = mPresentation->m_currentSlide;
        if (slide->GetCurrentTime() >= slide->GetStartTime() + slide->GetDuration())
        {
            mElapsedTime = 0.0f;
            if (GetRegion() == 0)
                mPhase = 0;
            else
                mPhase = 3;
            SetPhaseSlide();
        }
    }
    else if (mPhase == 3)
    {
        TLSlide* slide = mPresentation->m_currentSlide;
        if (slide->GetCurrentTime() >= slide->GetStartTime() + slide->GetDuration())
        {
            mPhase = 4;
            mElapsedTime = 0.0f;
            g_pAudioSystem->GetBundleManager()->GetSoundMap()->UnloadBank(0x17);
        }
    }
    else if (mPhase == 4)
    {
        if (mElapsedTime >= 0.0f)
        {
            mElapsedTime += fDeltaT;
            if (mElapsedTime >= 0.5f)
            {
                SetPhaseSlide();
                mElapsedTime = -1.0f;
            }
        }
    }
    if (mHomeButtonWarningActive)
    {
        TLSlide* slide = mHomeButtonWarning->GetActiveSlide();
        if (slide->GetCurrentTime() >= slide->GetStartTime() + slide->GetDuration())
        {
            mHomeButtonWarning->m_bVisible = false;
            mHomeButtonWarningActive = false;
        }
    }
}

void BootLoadingScene::SetPhaseSlide()
{
    switch (mPhase)
    {
    case 0:
        mPresentation->SetActiveSlide("ESRB", true);
        break;
    case 1:
        mPresentation->SetActiveSlide("strap", true);
        break;
    case 2:
        mPresentation->SetActiveSlide("nunchuk", true);
        break;
    case 4:
        mPresentation->SetActiveSlide("Slide1", true);
        break;
    case 3:
        mPresentation->SetActiveSlide("NLG", true);
        FEAudio::PlaySound(0x17, 0xDE83984E, 0, 0);
        break;
    }
    if (mHomeButtonWarning != 0)
    {
        float time = mHomeButtonWarning->GetActiveSlide()->m_time;
        mHomeButtonWarning = FEFinder<TLComponentInstance, 4>::FindOrDefault(
            mPresentation->GetActiveSlide(), "Layer", "no home");
        if (mWidescreen)
            mHomeButtonWarning->SetActiveSlide("widescreen", true, false);
        else
            mHomeButtonWarning->SetActiveSlide("Slide1", true, false);
        if (mHomeButtonWarningActive)
        {
            TLSlide* slide = mHomeButtonWarning->GetActiveSlide();
            if (time < slide->GetStartTime() + slide->GetDuration())
                mHomeButtonWarning->m_bVisible = true;
        }
        mHomeButtonWarning->Update(time);
    }
}

void BootLoadingScene::ShowHomeButtonWarning()
{
    if (mFEScene == 0 || mFEScene->mState != 6 || mHomeButtonWarningActive)
    {
        return;
    }

    if (mPhase != 1 || !(mElapsedTime <= 2.0f))
    {
        mHomeButtonWarning->m_bVisible = true;
        if (mWidescreen)
            mHomeButtonWarning->SetActiveSlide("widescreen", true, false);
        else
            mHomeButtonWarning->SetActiveSlide("Slide1", true, false);
        mHomeButtonWarningActive = true;
    }
}

bool BootLoadingScene::IsBootScreenPending()
{
    FEPresentation* presentation = mPresentation;
    unsigned char alpha;
    switch (mPhase)
    {
    case 0:
    {
        nlColour colour = FEFinder<TLInstance, 2>::Find(presentation, nlStringLowerHash("ESRB"),
            nlStringLowerHash("Layer"), nlStringLowerHash("Text2"), 0, 0, 0)->GetAssetColour();
        alpha = colour.c[3];
        break;
    }
    case 1:
        alpha = 0;
        break;
    case 2:
    {
        nlColour colour = FEFinder<TLInstance, 2>::Find(presentation, nlStringLowerHash("nunchuk"),
            nlStringLowerHash("Layer"), nlStringLowerHash("nunchuk"), 0, 0, 0)->GetAssetColour();
        alpha = colour.c[3];
        break;
    }
    case 3:
    {
        nlColour colour = FEFinder<TLInstance, 2>::Find(presentation, nlStringLowerHash("NLG"),
            nlStringLowerHash("Layer"), nlStringLowerHash("nlgameslogo"), 0, 0, 0)->GetAssetColour();
        alpha = colour.c[3];
        break;
    }
    default:
        alpha = 255;
        break;
    }
    return alpha != 255;
}

void BootLoadingScene::SceneCreated()
{
    FEPresentation* presentation = mPresentation;
    if (g_pLocalization->m_CurrentLanguage == nlLocalization::LangJapanese)
    {
        if (IsWidescreen())
        {
            mStrapImage = FEFinder<TLImageInstance, 2>::Find(presentation, nlStringLowerHash("strap"),
                nlStringLowerHash("Layer"), nlStringLowerHash("strap_16_9_jp"), 0, 0, 0);
            TLImageInstance* image = FEFinder<TLImageInstance, 2>::Find(presentation, nlStringLowerHash("strap"),
                nlStringLowerHash("Layer"), nlStringLowerHash("strap_jp"), 0, 0, 0);
            if (image == 0)
                image = &UnidentifiedTLImageDefault::sInstance;
            image->m_bVisible = false;
            mWidescreen = true;
        }
        else
        {
            mStrapImage = FEFinder<TLImageInstance, 2>::Find(presentation, nlStringLowerHash("strap"),
                nlStringLowerHash("Layer"), nlStringLowerHash("strap_jp"), 0, 0, 0);
            TLImageInstance* image = FEFinder<TLImageInstance, 2>::Find(presentation, nlStringLowerHash("strap"),
                nlStringLowerHash("Layer"), nlStringLowerHash("strap_16_9_jp"), 0, 0, 0);
            if (image == 0)
                image = &UnidentifiedTLImageDefault::sInstance;
            image->m_bVisible = false;
        }
    }
    else if (IsWidescreen())
    {
        mStrapImage = FEFinder<TLImageInstance, 2>::Find(presentation, nlStringLowerHash("strap"),
            nlStringLowerHash("Layer"), nlStringLowerHash("strap_16_9_us"), 0, 0, 0);
        TLImageInstance* image = FEFinder<TLImageInstance, 2>::Find(presentation, nlStringLowerHash("strap"),
            nlStringLowerHash("Layer"), nlStringLowerHash("strap_us"), 0, 0, 0);
        if (image == 0)
            image = &UnidentifiedTLImageDefault::sInstance;
        image->m_bVisible = false;
        mWidescreen = true;
    }
    else
    {
        mStrapImage = FEFinder<TLImageInstance, 2>::Find(presentation, nlStringLowerHash("strap"),
            nlStringLowerHash("Layer"), nlStringLowerHash("strap_us"), 0, 0, 0);
        TLImageInstance* image = FEFinder<TLImageInstance, 2>::Find(presentation, nlStringLowerHash("strap"),
            nlStringLowerHash("Layer"), nlStringLowerHash("strap_16_9_us"), 0, 0, 0);
        if (image == 0)
            image = &UnidentifiedTLImageDefault::sInstance;
        image->m_bVisible = false;
    }
    TLImageInstance* image = 0;
    switch (g_Language)
    {
    case nlLocalization::LangFrench:
    case nlLocalization::LangNAFrench:
        if (IsWidescreen())
            image = FEFinder<TLImageInstance, 2>::Find(presentation, nlStringLowerHash("art"),
                nlStringLowerHash("Layer"), nlStringLowerHash("strap_16_9_French"), 0, 0, 0);
        else
            image = FEFinder<TLImageInstance, 2>::Find(presentation, nlStringLowerHash("art"),
                nlStringLowerHash("Layer"), nlStringLowerHash("strap_French"), 0, 0, 0);
        break;
    case nlLocalization::LangGerman:
        if (IsWidescreen())
            image = FEFinder<TLImageInstance, 2>::Find(presentation, nlStringLowerHash("art"),
                nlStringLowerHash("Layer"), nlStringLowerHash("strap_16_9_German"), 0, 0, 0);
        else
            image = FEFinder<TLImageInstance, 2>::Find(presentation, nlStringLowerHash("art"),
                nlStringLowerHash("Layer"), nlStringLowerHash("strap_German"), 0, 0, 0);
        break;
    case nlLocalization::LangSpanish:
    case nlLocalization::LangNASpanish:
        if (IsWidescreen())
            image = FEFinder<TLImageInstance, 2>::Find(presentation, nlStringLowerHash("art"),
                nlStringLowerHash("Layer"), nlStringLowerHash("strap_16_9_Spanish"), 0, 0, 0);
        else
            image = FEFinder<TLImageInstance, 2>::Find(presentation, nlStringLowerHash("art"),
                nlStringLowerHash("Layer"), nlStringLowerHash("strap_Spanish"), 0, 0, 0);
        break;
    case nlLocalization::LangItalian:
        if (IsWidescreen())
            image = FEFinder<TLImageInstance, 2>::Find(presentation, nlStringLowerHash("art"),
                nlStringLowerHash("Layer"), nlStringLowerHash("strap_16_9_Italian"), 0, 0, 0);
        else
            image = FEFinder<TLImageInstance, 2>::Find(presentation, nlStringLowerHash("art"),
                nlStringLowerHash("Layer"), nlStringLowerHash("strap_Italian"), 0, 0, 0);
        break;
    }
    if (SCGetLanguage() == 6)
    {
        if (IsWidescreen())
            image = FEFinder<TLImageInstance, 2>::Find(presentation, nlStringLowerHash("art"),
                nlStringLowerHash("Layer"), nlStringLowerHash("strap_16_9_Dutch"), 0, 0, 0);
        else
            image = FEFinder<TLImageInstance, 2>::Find(presentation, nlStringLowerHash("art"),
                nlStringLowerHash("Layer"), nlStringLowerHash("strap_Dutch"), 0, 0, 0);
    }
    if (image != 0 && image->m_pTextureResource != 0)
        mStrapImage->m_pTextureResource = image->m_pTextureResource;
    SetPhaseSlide();
    mHomeButtonWarning = FEFinder<TLComponentInstance, 4>::Find(presentation->m_currentSlide,
        InlineHasher("Layer"), InlineHasher("no home"));
    if (mHomeButtonWarning == 0)
        mHomeButtonWarning = &UnidentifiedTLComponentDefault::sInstance;
    mHomeButtonWarning->m_bVisible = false;
    TLComponentInstance* component;
    component = FEFinder<TLComponentInstance, 4>::Find(presentation, nlStringLowerHash("Slide1"),
        nlStringLowerHash("Layer"), nlStringLowerHash("no home"), 0, 0, 0);
    if (component == 0)
        component = &UnidentifiedTLComponentDefault::sInstance;
    component->m_bVisible = false;
    component = FEFinder<TLComponentInstance, 4>::Find(presentation, nlStringLowerHash("ESRB"),
        nlStringLowerHash("Layer"), nlStringLowerHash("no home"), 0, 0, 0);
    if (component == 0)
        component = &UnidentifiedTLComponentDefault::sInstance;
    component->m_bVisible = false;
    component = FEFinder<TLComponentInstance, 4>::Find(presentation, nlStringLowerHash("strap"),
        nlStringLowerHash("Layer"), nlStringLowerHash("no home"), 0, 0, 0);
    if (component == 0)
        component = &UnidentifiedTLComponentDefault::sInstance;
    component->m_bVisible = false;
    component = FEFinder<TLComponentInstance, 4>::Find(presentation, nlStringLowerHash("nunchuk"),
        nlStringLowerHash("Layer"), nlStringLowerHash("no home"), 0, 0, 0);
    if (component == 0)
        component = &UnidentifiedTLComponentDefault::sInstance;
    component->m_bVisible = false;
    component = FEFinder<TLComponentInstance, 4>::Find(presentation, nlStringLowerHash("NLG"),
        nlStringLowerHash("Layer"), nlStringLowerHash("no home"), 0, 0, 0);
    if (component == 0)
        component = &UnidentifiedTLComponentDefault::sInstance;
    component->m_bVisible = false;
    if (mWidescreen)
        mHomeButtonWarning->SetActiveSlide("widescreen", true, false);
    else
        mHomeButtonWarning->SetActiveSlide("Slide1", true, false);
}
