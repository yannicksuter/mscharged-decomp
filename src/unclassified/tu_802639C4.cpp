#include "unclassified/tu_802639C4.h"

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
#include "NL/nlString.h"

extern TLComponentInstance lbl_80580030;
extern TLImageInstance lbl_80580248;
extern "C" bool fn_80273B00();
extern "C" u8 SCGetLanguage();
extern "C" void fn_801CBC54(int slotId, unsigned long cueId, const void* debugName, void* context);

TU802639C4Scene::TU802639C4Scene()
    : mUnidentified1C(0.0f)
    , mUnidentified20(255.0f)
    , mUnidentified24(false)
    , mUnidentified30(0)
    , mUnidentified34(false)
    , mUnidentified35(false)
{
    mUnidentified28 = 1;
}

TU802639C4Scene::~TU802639C4Scene()
{
}

void TU802639C4Scene::Update(float fDeltaT)
{
    BaseSceneHandler::Update(fDeltaT);
    if (mUnidentified28 == 0)
    {
        TLSlide* slide = mPresentation->GetActiveSlide();
        if (slide->m_time >= slide->m_start + slide->m_duration)
        {
            mUnidentified28 = 3;
            mUnidentified1C = 0.0f;
            fn_80263E1C();
        }
    }
    else if (mUnidentified28 == 1)
    {
        if (mUnidentified24)
        {
            mUnidentified20 -= (2 * 255) * fDeltaT;
            if (mUnidentified20 <= 0.0f)
                mUnidentified20 = 0.0f;
            nlColour colour = { 255, 255, 255, 0 };
            colour.c[3] = (unsigned char)mUnidentified20;
            mUnidentified2C->SetAssetColour(colour);
            if (mUnidentified20 <= 0.0f)
            {
                mUnidentified1C = 0.0f;
                mUnidentified28 = 2;
                fn_80263E1C();
            }
        }
        else
        {
            mUnidentified1C += fDeltaT;
            if (mUnidentified1C >= 1.5f)
            {
                if (g_pFEInput->JustPressed((eFEINPUT_PAD)8, 0x20, true, 0)
                    || g_pFEInput->JustPressed((eFEINPUT_PAD)8, 0x08, true, 0)
                    || g_pFEInput->JustPressed((eFEINPUT_PAD)8, 0x1E, true, 0)
                    || g_pFEInput->JustPressed((eFEINPUT_PAD)8, 0x1F, true, 0)
                    || g_pFEInput->JustPressed((eFEINPUT_PAD)8, 0x2A, true, 0)
                    || g_pFEInput->JustPressed((eFEINPUT_PAD)8, 0x2B, true, 0)
                    || g_pFEInput->JustPressed((eFEINPUT_PAD)8, 0x28, true, 0)
                    || g_pFEInput->JustPressed((eFEINPUT_PAD)8, 0x29, true, 0))
                    mUnidentified24 = true;
                if (mUnidentified1C >= 15.5f)
                    mUnidentified24 = true;
            }
        }
    }
    else if (mUnidentified28 == 2)
    {
        TLSlide* slide = mPresentation->GetActiveSlide();
        if (slide->m_time >= slide->m_start + slide->m_duration)
        {
            mUnidentified1C = 0.0f;
            if (GetRegion() == 0)
                mUnidentified28 = 0;
            else
                mUnidentified28 = 3;
            fn_80263E1C();
        }
    }
    else if (mUnidentified28 == 3)
    {
        TLSlide* slide = mPresentation->GetActiveSlide();
        if (slide->m_time >= slide->m_start + slide->m_duration)
        {
            mUnidentified28 = 4;
            mUnidentified1C = 0.0f;
            g_pAudioSystem->GetBundleManager()->GetSoundMap()->UnloadBank(0x17);
        }
    }
    else if (mUnidentified28 == 4)
    {
        if (mUnidentified1C >= 0.0f)
        {
            mUnidentified1C += fDeltaT;
            if (mUnidentified1C >= 0.5f)
            {
                fn_80263E1C();
                mUnidentified1C = -1.0f;
            }
        }
    }
    if (mUnidentified34)
    {
        TLSlide* slide = mUnidentified30->GetActiveSlide();
        if (slide->m_time >= slide->m_start + slide->m_duration)
        {
            mUnidentified30->m_bVisible = false;
            mUnidentified34 = false;
        }
    }
}

void TU802639C4Scene::fn_80263E1C()
{
    switch (mUnidentified28)
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
        fn_801CBC54(0x17, 0xDE83984E, 0, 0);
        break;
    }
    if (mUnidentified30 != 0)
    {
        float time = mUnidentified30->GetActiveSlide()->m_time;
        mUnidentified30 = FEFinder<TLComponentInstance, 4>::Find(mPresentation->GetActiveSlide(),
            InlineHasher("Layer"), InlineHasher("no home"));
        if (mUnidentified30 == 0)
            mUnidentified30 = &lbl_80580030;
        if (mUnidentified35)
            mUnidentified30->SetActiveSlide("widescreen", true, false);
        else
            mUnidentified30->SetActiveSlide("Slide1", true, false);
        if (mUnidentified34)
        {
            TLSlide* slide = mUnidentified30->GetActiveSlide();
            if (time < slide->m_start + slide->m_duration)
                mUnidentified30->m_bVisible = true;
        }
        mUnidentified30->Update(time);
    }
}

void TU802639C4Scene::fn_80264060()
{
    if (mFEScene != 0 && mFEScene->mState == 6)
    {
        if (mUnidentified34)
            return;
        if (mUnidentified28 != 1 || !(mUnidentified1C <= 2.0f))
        {
            mUnidentified30->m_bVisible = true;
            if (mUnidentified35)
                mUnidentified30->SetActiveSlide("widescreen", true, false);
            else
                mUnidentified30->SetActiveSlide("Slide1", true, false);
            mUnidentified34 = true;
        }
    }
}

bool TU802639C4Scene::fn_80264120()
{
    FEPresentation* presentation = mPresentation;
    unsigned char alpha;
    switch (mUnidentified28)
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

void TU802639C4Scene::SceneCreated()
{
    FEPresentation* presentation = mPresentation;
    if (g_pLocalization->m_CurrentLanguage == nlLocalization::LangJapanese)
    {
        if (fn_80273B00())
        {
            mUnidentified2C = FEFinder<TLImageInstance, 2>::Find(presentation, nlStringLowerHash("strap"),
                nlStringLowerHash("Layer"), nlStringLowerHash("strap_16_9_jp"), 0, 0, 0);
            TLImageInstance* image = FEFinder<TLImageInstance, 2>::Find(presentation, nlStringLowerHash("strap"),
                nlStringLowerHash("Layer"), nlStringLowerHash("strap_jp"), 0, 0, 0);
            if (image == 0)
                image = &lbl_80580248;
            image->m_bVisible = false;
            mUnidentified35 = true;
        }
        else
        {
            mUnidentified2C = FEFinder<TLImageInstance, 2>::Find(presentation, nlStringLowerHash("strap"),
                nlStringLowerHash("Layer"), nlStringLowerHash("strap_jp"), 0, 0, 0);
            TLImageInstance* image = FEFinder<TLImageInstance, 2>::Find(presentation, nlStringLowerHash("strap"),
                nlStringLowerHash("Layer"), nlStringLowerHash("strap_16_9_jp"), 0, 0, 0);
            if (image == 0)
                image = &lbl_80580248;
            image->m_bVisible = false;
        }
    }
    else if (fn_80273B00())
    {
        mUnidentified2C = FEFinder<TLImageInstance, 2>::Find(presentation, nlStringLowerHash("strap"),
            nlStringLowerHash("Layer"), nlStringLowerHash("strap_16_9_us"), 0, 0, 0);
        TLImageInstance* image = FEFinder<TLImageInstance, 2>::Find(presentation, nlStringLowerHash("strap"),
            nlStringLowerHash("Layer"), nlStringLowerHash("strap_us"), 0, 0, 0);
        if (image == 0)
            image = &lbl_80580248;
        image->m_bVisible = false;
        mUnidentified35 = true;
    }
    else
    {
        mUnidentified2C = FEFinder<TLImageInstance, 2>::Find(presentation, nlStringLowerHash("strap"),
            nlStringLowerHash("Layer"), nlStringLowerHash("strap_us"), 0, 0, 0);
        TLImageInstance* image = FEFinder<TLImageInstance, 2>::Find(presentation, nlStringLowerHash("strap"),
            nlStringLowerHash("Layer"), nlStringLowerHash("strap_16_9_us"), 0, 0, 0);
        if (image == 0)
            image = &lbl_80580248;
        image->m_bVisible = false;
    }
    TLImageInstance* image = 0;
    switch (g_Language)
    {
    case nlLocalization::LangFrench:
    case nlLocalization::LangNAFrench:
        if (fn_80273B00())
            image = FEFinder<TLImageInstance, 2>::Find(presentation, nlStringLowerHash("art"),
                nlStringLowerHash("Layer"), nlStringLowerHash("strap_16_9_French"), 0, 0, 0);
        else
            image = FEFinder<TLImageInstance, 2>::Find(presentation, nlStringLowerHash("art"),
                nlStringLowerHash("Layer"), nlStringLowerHash("strap_French"), 0, 0, 0);
        break;
    case nlLocalization::LangGerman:
        if (fn_80273B00())
            image = FEFinder<TLImageInstance, 2>::Find(presentation, nlStringLowerHash("art"),
                nlStringLowerHash("Layer"), nlStringLowerHash("strap_16_9_German"), 0, 0, 0);
        else
            image = FEFinder<TLImageInstance, 2>::Find(presentation, nlStringLowerHash("art"),
                nlStringLowerHash("Layer"), nlStringLowerHash("strap_German"), 0, 0, 0);
        break;
    case nlLocalization::LangSpanish:
    case nlLocalization::LangNASpanish:
        if (fn_80273B00())
            image = FEFinder<TLImageInstance, 2>::Find(presentation, nlStringLowerHash("art"),
                nlStringLowerHash("Layer"), nlStringLowerHash("strap_16_9_Spanish"), 0, 0, 0);
        else
            image = FEFinder<TLImageInstance, 2>::Find(presentation, nlStringLowerHash("art"),
                nlStringLowerHash("Layer"), nlStringLowerHash("strap_Spanish"), 0, 0, 0);
        break;
    case nlLocalization::LangItalian:
        if (fn_80273B00())
            image = FEFinder<TLImageInstance, 2>::Find(presentation, nlStringLowerHash("art"),
                nlStringLowerHash("Layer"), nlStringLowerHash("strap_16_9_Italian"), 0, 0, 0);
        else
            image = FEFinder<TLImageInstance, 2>::Find(presentation, nlStringLowerHash("art"),
                nlStringLowerHash("Layer"), nlStringLowerHash("strap_Italian"), 0, 0, 0);
        break;
    }
    if (SCGetLanguage() == 6)
    {
        if (fn_80273B00())
            image = FEFinder<TLImageInstance, 2>::Find(presentation, nlStringLowerHash("art"),
                nlStringLowerHash("Layer"), nlStringLowerHash("strap_16_9_Dutch"), 0, 0, 0);
        else
            image = FEFinder<TLImageInstance, 2>::Find(presentation, nlStringLowerHash("art"),
                nlStringLowerHash("Layer"), nlStringLowerHash("strap_Dutch"), 0, 0, 0);
    }
    if (image != 0 && image->m_pTextureResource != 0)
        mUnidentified2C->m_pTextureResource = image->m_pTextureResource;
    fn_80263E1C();
    mUnidentified30 = FEFinder<TLComponentInstance, 4>::Find(presentation->GetActiveSlide(),
        InlineHasher("Layer"), InlineHasher("no home"));
    if (mUnidentified30 == 0)
        mUnidentified30 = &lbl_80580030;
    mUnidentified30->m_bVisible = false;
    TLComponentInstance* component;
    component = FEFinder<TLComponentInstance, 4>::Find(presentation, nlStringLowerHash("Slide1"),
        nlStringLowerHash("Layer"), nlStringLowerHash("no home"), 0, 0, 0);
    if (component == 0)
        component = &lbl_80580030;
    component->m_bVisible = false;
    component = FEFinder<TLComponentInstance, 4>::Find(presentation, nlStringLowerHash("ESRB"),
        nlStringLowerHash("Layer"), nlStringLowerHash("no home"), 0, 0, 0);
    if (component == 0)
        component = &lbl_80580030;
    component->m_bVisible = false;
    component = FEFinder<TLComponentInstance, 4>::Find(presentation, nlStringLowerHash("strap"),
        nlStringLowerHash("Layer"), nlStringLowerHash("no home"), 0, 0, 0);
    if (component == 0)
        component = &lbl_80580030;
    component->m_bVisible = false;
    component = FEFinder<TLComponentInstance, 4>::Find(presentation, nlStringLowerHash("nunchuk"),
        nlStringLowerHash("Layer"), nlStringLowerHash("no home"), 0, 0, 0);
    if (component == 0)
        component = &lbl_80580030;
    component->m_bVisible = false;
    component = FEFinder<TLComponentInstance, 4>::Find(presentation, nlStringLowerHash("NLG"),
        nlStringLowerHash("Layer"), nlStringLowerHash("no home"), 0, 0, 0);
    if (component == 0)
        component = &lbl_80580030;
    component->m_bVisible = false;
    if (mUnidentified35)
        mUnidentified30->SetActiveSlide("widescreen", true, false);
    else
        mUnidentified30->SetActiveSlide("Slide1", true, false);
}
