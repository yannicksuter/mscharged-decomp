#include "Game/SH/SHNavigation.h"
#include "Game/FE/feHelpFuncs.h"
#include "Game/SH/SHSceneBase.h"
#include "Game/FE/tlImageInstance.h"
#include "Game/FE/FEAudio.h"

#include "Game/DB/CharacterInfo.h"
#include "Game/DB/GameProgress.h"
#include "Game/FE/feFinder.h"
#include "Game/FE/feInput.h"
#include "Game/FE/fePackage.h"
#include "Game/FE/fePresentation.h"
#include "Game/FE/feScene.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlSlide.h"
#include "Game/FE/tlTextInstance.h"
#include "Game/GameInfo.h"
#include "NL/globalpad.h"
#include "NL/nlMath.h"
#include "NL/nlPrint.h"
#include "NL/nlLocalization.h"
#include "NL/nlString.h"
#include "Game/FE/feDPD.h"

// Text crossfade helpers owned by the 0x801E2xxx translation unit.
extern "C" void fn_801E2F50(UnidentifiedTextFader* fader, TLInstance* instance, int value0, int value1, int value2);
extern "C" void fn_801E3A88(UnidentifiedTextFader* fader, void* strings);
extern "C" void fn_801E3B60(UnidentifiedTextFader* fader, const char* text);
extern "C" void fn_801E3DB4(UnidentifiedTextFader* fader, float dt);
extern "C" void fn_801E4460(UnidentifiedTextFader* fader, TLInstance* instance);

// Scroll widget owned by the 0x8022Fxxx/0x80230xxx translation units.

class SHNavigation;


extern bool lbl_806DC704;
extern bool lbl_806E0F8B;

static inline TLInstance* FindInstance(TLSlide* slide, const char* item)
{
    TLInstance* result;
    unsigned long itemHash = nlStringLowerHash(item);
    result = FEFinder<TLInstance, 2>::_Find(slide, nlStringLowerHash("Layer"), itemHash, 0, 0, 0, 0);
    if (result == 0)
        return 0;
    return result;
}

static inline TLComponentInstance* FindComponent(TLSlide* slide, const char* item)
{
    TLComponentInstance* result = (TLComponentInstance*)FindInstance(slide, item);
    if (result == 0)
        result = &gDefaultTLComponentInstance;
    return result;
}

typedef void (UnidentifiedSHSceneBase::*UnidentifiedSHSceneCallback)(int, void*);

struct UnidentifiedSHSceneCallbackRef
{
    UnidentifiedSHSceneCallbackRef(UnidentifiedSHSceneCallback callback)
        : mCallback(callback)
    {
    }

    UnidentifiedSHSceneCallback mCallback;
};

struct UnidentifiedSHSceneBinding
{
    UnidentifiedSHSceneCallback mCallback;
    UnidentifiedSHSceneBase* mTarget;
    bool mUnidentified10;
    bool mUnidentified11;

    UnidentifiedSHSceneBinding(UnidentifiedSHSceneCallbackRef callback, UnidentifiedSHSceneBase* target)
        : mCallback(callback.mCallback)
        , mTarget(target)
    {
    }

    void operator()(int index, void* context) const
    {
        (mTarget->*mCallback)(index, context);
    }
};

static inline UnidentifiedSHSceneBinding BindSHSceneAction(UnidentifiedSHSceneCallbackRef callback, UnidentifiedSHSceneBase* target)
{
    return UnidentifiedSHSceneBinding(callback, target);
}

inline TLInstance* UnidentifiedSHSceneBase::FindCurrentInstance(const char* item)
{
    TLInstance* result;
    unsigned long itemHash = nlStringLowerHash(item);
    result = FEFinder<TLInstance, 2>::_Find(mPresentation->m_currentSlide, nlStringLowerHash("Layer"), itemHash, 0, 0, 0, 0);
    if (result == 0)
        return 0;
    return result;
}

inline TLComponentInstance* UnidentifiedSHSceneBase::FindCurrentComponent(const char* item)
{
    TLComponentInstance* result = (TLComponentInstance*)FindCurrentInstance(item);
    if (result == 0)
        return &gDefaultTLComponentInstance;
    return result;
}

UnidentifiedSHSceneBase::~UnidentifiedSHSceneBase()
{
}

void UnidentifiedSHSceneBase::SHSceneVirtual2C(unsigned int transition)
{
    mUnidentified28 = transition;
    if (transition - 0xB <= 2)
    {
        mUnidentified106 = true;
        mUnidentified2C = 3;
    }
    else
    {
        mUnidentified106 = false;
        mUnidentified2C = 0;
    }
}

void UnidentifiedSHSceneBase::fn_8026932C()
{
    FEPresentation* presentation = mPresentation;
    if (mUnidentified28 == 0xD)
        return;
    if (mUnidentified28 == 0xC)
        return;
    if (mUnidentified28 == 0xB)
        return;
    if (mUnidentified28 == 0xA)
    {
        if (mUnidentified2C == 2)
        {
            mUnidentified2C = 1;
            presentation->SetActiveSlide("headline pic", true);
            presentation->Update(presentation->m_currentSlide->m_start + presentation->m_currentSlide->m_duration);
        }
        else if (mUnidentified2C == 3)
        {
            mUnidentified2C = 2;
            presentation->SetActiveSlide("story", true);
            presentation->Update(0.0f);
        }
    }
    else if (mUnidentified2C == 2)
    {
        mUnidentified2C = 1;
        presentation->SetActiveSlide("headline pic", true);
        presentation->Update(presentation->m_currentSlide->m_start + presentation->m_currentSlide->m_duration);
    }
}

void UnidentifiedSHSceneBase::fn_8026942C()
{
    FEPresentation* presentation = mPresentation;
    if ((unsigned int)(mUnidentified28 - 0xB) <= 2)
    {
        mUnidentified106 = true;
    }
    else if (mUnidentified28 == 0xA)
    {
        if (mUnidentified2C == 2)
        {
            mUnidentified106 = GameInfoManager::Instance()->mIsOnlineMode == 0;
            mUnidentified2C = 3;
            presentation->SetActiveSlide("game summary", true);
        }
        else if (mUnidentified2C == 1)
        {
            mUnidentified2C = 2;
            presentation->SetActiveSlide("story", true);
            presentation->Update(0.0f);
        }
    }
    else if (mUnidentified2C == 1)
    {
        mUnidentified106 = true;
        mUnidentified2C = 2;
        presentation->SetActiveSlide("story", true);
        presentation->Update(0.0f);
    }
}

void UnidentifiedSHSceneBase::SHSceneVirtual30()
{
}

void UnidentifiedSHSceneBase::fn_80269524()
{
    if (!mUnidentified420.mInitialized)
    {
        mUnidentified420.SetComponent(FindCurrentComponent("scrollbar"));
        mUnidentified420.Initialize();
    }
    FEPointerListener::Callback callback(BindSHSceneAction(&UnidentifiedSHSceneBase::fn_8026ABF0, this));
    mComponent.SetPointerEnterCallback(callback);
    callback = FEPointerListener::Callback(BindSHSceneAction(&UnidentifiedSHSceneBase::fn_8026AD50, this));
    mComponent.SetPointerLeaveCallback(callback);
    FEPointerListener::Callback callback2(BindSHSceneAction(&UnidentifiedSHSceneBase::fn_8026AE98, this));
    mComponent.SetPointerPressCallback(callback2);
    SetDoneButtonBounds(&mComponent, 0, 0);
}

UnidentifiedSHSceneBase::UnidentifiedSHSceneBase()
    : mUnidentified1C(0xC)
    , mUnidentified20(1)
    , mUnidentified24(0)
    , mUnidentified28(-1)
    , mUnidentified2C(-1)
    , mUnidentified30(false)
    , mUnidentifiedFC(false)
    , mUnidentified100(0)
    , mUnidentified104(false)
    , mUnidentified105(false)
    , mUnidentified106(true)
    , mUnidentified107(false)
    , mUnidentified108(false)
    , mUnidentified109(false)
    , mUnidentified1C0(0)
    , mUnidentified200(0)
    , mUnidentified240("art/fe/StrikerTimesUI.res", 0)
    , mUnidentified2E0("art/fe/StrikerTimesUI.res", 0)
    , mUnidentified380("art/fe/StrikerTimesUI.res", 0)
{
    mComponent.mIgnoreInputLock = true;
    mComponent.mDisabled = true;
    FEPointerEvent event;
    mComponent.mPreviousEvents[0] = event;
    mComponent.mPreviousEvents[1] = event;
    mComponent.mPreviousEvents[2] = event;
    mComponent.mPreviousEvents[3] = event;
}

void UnidentifiedSHSceneBase::SceneCreated()
{
    mUnidentified420.SetComponent(FindCurrentComponent("scrollbar"));
    if ((unsigned int)(mUnidentified28 - 0xB) <= 2)
    {
        mUnidentified420.SetRange(0);
        mUnidentified109 = true;
    }
    else if (mUnidentified28 == 0xA)
    {
        mUnidentified420.SetRange(2);
    }
    else
    {
        mUnidentified420.SetRange(1);
    }
    mUnidentified420.SetValue(0);

    TLSlide* first = mPresentation->m_currentSlide;
    TLSlide* slide = first;
    do
    {
        FindInstance(slide, "TimerText")->m_bVisible = false;
        FindInstance(slide, "NetworkWait")->m_bVisible = false;
        FindComponent(slide, "done")->m_bVisible = false;
        slide = slide->m_next;
    } while (slide != first);

    if (mUnidentified28 != 0xA)
    {
        TLInstance* more = FEFinder<TLComponentInstance, 4>::Find(mPresentation, nlStringLowerHash("story"),
            nlStringLowerHash("Layer"), nlStringLowerHash("more"), 0, 0, 0);
        if (more == 0)
            more = &gDefaultTLComponentInstance;
        more->m_bVisible = false;
    }

    if (mUnidentified28 != 8 && mUnidentified28 != 0xD)
    {
        SHNavigation* scene = GetNavigationScene();
        if (scene != 0)
            scene->SetButtons(0, true);
    }

    char buffer[0x40];
    switch (g_pLocalization->m_CurrentLanguage)
    {
    case nlLocalization::LangFrench:
    case nlLocalization::LangNAFrench:
        nlSNPrintf(buffer, 0x40, "fe/striker_times_textures/st_logo_french");
        break;
    case nlLocalization::LangGerman:
        nlSNPrintf(buffer, 0x40, "fe/striker_times_textures/st_logo_german");
        break;
    case nlLocalization::LangSpanish:
    case nlLocalization::LangNASpanish:
        nlSNPrintf(buffer, 0x40, "fe/striker_times_textures/st_logo_spanish");
        break;
    case nlLocalization::LangItalian:
        nlSNPrintf(buffer, 0x40, "fe/striker_times_textures/st_logo_italian");
        break;
    default:
        mUnidentified109 = true;
        break;
    }

    if (!mUnidentified109)
    {
        unsigned long logoHash = nlStringLowerHash("st_logo");
        unsigned long itemHash = nlStringLowerHash("logo");
        TLInstance* logo = FEFinder<TLImageInstance, 2>::Find(mPresentation, nlStringLowerHash("logo"),
            nlStringLowerHash("Layer"), itemHash, logoHash, 0, 0);
        mUnidentified380.mImageInstance = (TLImageInstance*)logo;
        mUnidentified380.QueueLoad(buffer, false);
    }

    mPresentation->SetActiveSlide("in", true);
    mPresentation->Update(0.0f);
}

void UnidentifiedSHSceneBase::Update(float dt)
{
    if (lbl_806DC704 && lbl_806E0F8B)
        return;
    if (!mUnidentified109)
    {
        mUnidentified109 = mUnidentified380.Update(true);
        return;
    }
    BaseSceneHandler::Update(dt);
    if (mUnidentified28 != 0xD && mUnidentified28 != 0xC && mUnidentified28 != 0xB)
    {
        if (!mUnidentified107)
            fn_8026A63C();
        mUnidentified240.Update(true);
        mUnidentified2E0.Update(true);
    }
    if (!mUnidentified108)
    {
        mUnidentified108 = true;
        FEAudio::PlayAnimAudioEvent(0xAFE4352B, 0, 0, 1);
    }
    int state = mUnidentified100;
    if (state == 0 || (unsigned int)(state - 2) <= 1)
    {
        FEPresentation* presentation = mPresentation;
        TLSlide* slide = presentation->m_currentSlide;
        if (slide->m_time < slide->m_start + slide->m_duration)
        {
            for (int pad = 0; pad < 4; ++pad)
            {
                gFEPointerInstances[pad]->SetActiveSlide("waiting", true, false);
            }
            return;
        }
        if (state == 0)
        {
            if (!mUnidentified104)
            {
                fn_80269524();
                mUnidentified104 = true;
            }
            mUnidentified100 = 1;
            if ((unsigned int)(mUnidentified28 - 0xB) <= 2)
            {
                mPresentation->SetActiveSlide("game summary", true);
                mPresentation->Update(0.0f);
            }
            else if (mUnidentified28 == 0xA)
            {
                mPresentation->SetActiveSlide("logo", true);
                mPresentation->Update(0.0f);
            }
            else
            {
                mPresentation->SetActiveSlide("logo", true);
                mPresentation->Update(0.0f);
            }
            return;
        }
        if (state == 2)
        {
            SHSceneVirtual30();
            return;
        }
        if (state == 3)
        {
            SHSceneVirtual34();
            return;
        }
    }
    if (mUnidentified2C == 0)
    {
        TLSlide* slide = mPresentation->m_currentSlide;
        if (slide->m_time >= slide->m_start + slide->m_duration)
        {
            mUnidentified2C = 1;
            mPresentation->SetActiveSlide("headline pic", true);
            mPresentation->Update(0.0f);
        }
        else
        {
            return;
        }
    }
    if ((unsigned int)(mUnidentified2C - 1) <= 1)
    {
        fn_801E3DB4(&mUnidentified1C0, dt);
        mUnidentified200.mUnidentified28 = mUnidentified1C0.mUnidentified28;
        fn_801E3DB4(&mUnidentified200, 0.0f);
    }
    if (mUnidentified106 && !mUnidentified105)
    {
        FEPresentation* presentation = mPresentation;
        TLSlide* first = presentation->m_currentSlide;
        TLSlide* slide = first;
        FEAudio::PlayAnimAudioEvent(0x2AB04562, 0, 0, 1);
        do
        {
            TLComponentInstance* done = FindComponent(slide, "done");
            done->m_bVisible = true;
            done->SetActiveSlide("in", true, false);
            slide = slide->m_next;
        } while (slide != first);
        mComponent.mDisabled = false;
        mUnidentified105 = true;
    }
    for (int pad = 0; pad < 4; ++pad)
    {
        TLComponentInstance* instance = gFEPointerInstances[pad];
        if (mUnidentified28 != 0xC && pad != gFEControllerIndex)
        {
            instance->SetActiveSlide("waiting", true, false);
        }
        else
        {
            instance->SetActiveSlide("cursor", true, false);
            u8 valid = 1;
            FEPointerEvent event;
            event.mIndex = pad;
            event.mPosition = GetPointerPosition(pad, &valid);
            g_pPadManager->GetPad(pad)->GetButtonIndex(0x1E, true);
            event.mPressed = g_pFEInput->JustPressed((eFEINPUT_PAD)pad, 0x1E, true, 0);
            event.mReleased = g_pFEInput->JustReleased((eFEINPUT_PAD)pad, 0x1E, true, 0);
            mComponent.HandlePointerEvent(&event);
            if (mUnidentified30)
                return;
            mUnidentified420.Update(event, dt);
            if (mUnidentified420.IsScrolling(1, 1))
            {
                fn_8026942C();
            }
            else if (mUnidentified420.IsScrolling(0, 1))
            {
                fn_8026932C();
            }
        }
    }
}

void UnidentifiedSHSceneBase::fn_8026A63C()
{
    FEPresentation* presentation = mFEScene->m_pFEPackage->GetPresentation();

    TLTextInstance* headlineText = FEFinder<TLTextInstance, 3>::Find(presentation, nlStringLowerHash("headline pic"), nlStringLowerHash("Layer"),
        nlStringLowerHash("HEADLINE"), 0, 0, 0);
    if (headlineText == 0)
        headlineText = &gDefaultTLTextInstance;

    TLTextInstance* descriptionText = FEFinder<TLTextInstance, 3>::Find(presentation, nlStringLowerHash("story"), nlStringLowerHash("Layer"),
        nlStringLowerHash("Description_clip"), 0, 0, 0);
    if (descriptionText == 0)
        descriptionText = &gDefaultTLTextInstance;

    fn_801E2F50(&mUnidentified1C0, headlineText, -1, -1, -300);
    if (mUnidentifiedFC)
        fn_801E3A88(&mUnidentified1C0, &mUnidentifiedF4);
    else
        fn_801E3B60(&mUnidentified1C0, mUnidentified31);
    fn_801E4460(&mUnidentified1C0, descriptionText);
    descriptionText->m_bVisible = false;

    TLTextInstance* storyHeadline = FEFinder<TLTextInstance, 3>::Find(presentation, nlStringLowerHash("story"), nlStringLowerHash("Layer"),
        nlStringLowerHash("HEADLINE"), 0, 0, 0);
    if (storyHeadline == 0)
        storyHeadline = &gDefaultTLTextInstance;
    fn_801E2F50(&mUnidentified200, storyHeadline, -1, -1, -300);
    if (mUnidentifiedFC)
        fn_801E3A88(&mUnidentified200, &mUnidentifiedF4);
    else
        fn_801E3B60(&mUnidentified200, mUnidentified31);
    fn_801E4460(&mUnidentified200, descriptionText);

    TLTextInstance* bodyText = FEFinder<TLTextInstance, 3>::Find(presentation, nlStringLowerHash("story"), nlStringLowerHash("Layer"),
        nlStringLowerHash("BODY"), 0, 0, 0);
    if (bodyText == 0)
        bodyText = &gDefaultTLTextInstance;
    if (mUnidentifiedFC)
        bodyText->SetString(mUnidentifiedF8.c_str());
    else
        bodyText->SetStringId(mUnidentified71);

    mUnidentified1C0.mUnidentified24 = 150.0f;
    mUnidentified200.mUnidentified24 = 150.0f;
    mUnidentified1C0.mUnidentified2C = 2.0f;
    mUnidentified200.mUnidentified2C = 2.0f;

    TLInstance* storyTexture = FEFinder<TLInstance, 2>::Find(mPresentation, nlStringLowerHash("story"), nlStringLowerHash("Layer"),
        nlStringLowerHash("00_dummy_texture"), 0, 0, 0);
    if (storyTexture == 0)
        storyTexture = &gDefaultTLImageInstance;
    mUnidentified240.mImageInstance = (TLImageInstance*)storyTexture;

    TLInstance* headlineTexture = FEFinder<TLInstance, 2>::Find(mPresentation, nlStringLowerHash("headline pic"), nlStringLowerHash("Layer"),
        nlStringLowerHash("00_dummy_texture"), 0, 0, 0);
    if (headlineTexture == 0)
        headlineTexture = &gDefaultTLImageInstance;
    mUnidentified2E0.mImageInstance = (TLImageInstance*)headlineTexture;

    mUnidentified2E0.QueueLoad(mUnidentifiedB1, false);
    mUnidentified240.QueueLoad(mUnidentifiedB1, false);
    mUnidentified107 = true;
}

void UnidentifiedSHSceneBase::SHSceneVirtual38(int captain, int mood, int special)
{
    int variant = nlRandom(3, &nlDefaultSeed);
    if (mUnidentified28 == 8)
    {
        s8 stored = g_pStrikerChallenge->mHeadlineVariant;
        if (stored != -1)
            variant = stored;
        else
            g_pStrikerChallenge->mHeadlineVariant = variant;
    }
    const CharacterInfo& info = GetCharacterInfo(GetCharacterIndexFromCaptain(captain));
    char name[0x10];
    if (special == 4)
    {
        nlSNPrintf(name, 0x10, "boo");
        variant = 0;
    }
    else if (special == 2)
    {
        nlSNPrintf(name, 0x10, "hammerbro");
        variant = 0;
    }
    else if (special == 8)
    {
        nlSNPrintf(name, 0x10, "kritter");
        variant = 0;
    }
    else if (captain == 3)
    {
        nlSNPrintf(name, 0x10, "dk");
    }
    else if (captain == 0xA)
    {
        nlSNPrintf(name, 0x10, "diddy");
    }
    else
    {
        nlSNPrintf(name, 0x10, "%s", info.mName);
    }
    switch (mood)
    {
    case 0:
        nlSNPrintf(mUnidentifiedB1, 0x40, "fe/striker_times_textures/%s_positive_0%d", name, variant);
        break;
    case 1:
        nlSNPrintf(mUnidentifiedB1, 0x40, "fe/striker_times_textures/%s_neutral_0%d", name, variant);
        break;
    case 2:
        nlSNPrintf(mUnidentifiedB1, 0x40, "fe/striker_times_textures/%s_negative_0%d", name, variant);
        break;
    case 3:
        nlSNPrintf(mUnidentifiedB1, 0x40, "fe/striker_times_textures/%s_positive_0%d", name, variant);
        break;
    }
}

void UnidentifiedSHSceneBase::fn_8026ABF0(int index, void* context)
{
    mComponent.SetPointerState(1, index);
    TLComponentInstance* done = FindCurrentComponent("done");
    if (!mComponent.HasOtherPointerState(1, index))
    {
        done->SetActiveSlide("over", true, false);
        FEAudio::PlayAnimAudioEvent(0xAA73EF33, 0, 0, 1);
    }
}

void UnidentifiedSHSceneBase::fn_8026AD50(int index, void* context)
{
    mComponent.SetPointerState(0, index);
    TLComponentInstance* done = FindCurrentComponent("done");
    if (!mComponent.HasOtherPointerState(1, index))
    {
        done->SetActiveSlide("off", true, false);
    }
}

void UnidentifiedSHSceneBase::fn_8026AE98(int index, void* context)
{
    FEAudio::PlayAnimAudioEvent(0xF0AFD586, 0, 0, 1);
    mUnidentified30 = true;
    mUnidentified100 = 2;
    mPresentation->SetActiveSlide("out", true);
    mPresentation->Update(0.0f);
    if (mUnidentified28 != 0xD)
    {
        GetNavigationScene()->HideButtons();
    }
    if (mUnidentified28 == 8)
    {
        FEAudio::PlayAnimAudioEvent(0x4861E03D, 0, 0, 1);
    }
    FindCurrentComponent("done")->SetActiveSlide("down", true, false);
}
