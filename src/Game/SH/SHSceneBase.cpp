#include "Game/SH/SHSceneBase.h"

#include "Game/DB/CharacterInfo.h"
#include "Game/FE/feFinder.h"
#include "Game/FE/feInput.h"
#include "Game/FE/fePackage.h"
#include "Game/FE/fePresentation.h"
#include "Game/FE/feScene.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlSlide.h"
#include "Game/FE/tlTextInstance.h"
#include "Game/GameInfo.h"
#include "NL/nlMath.h"
#include "NL/nlPrint.h"
#include "NL/nlLocalization.h"
#include "NL/nlString.h"

// Text crossfade helpers owned by the 0x801E2xxx translation unit.
extern "C" void fn_801E2F50(UnidentifiedTextFader* fader, TLInstance* instance, int value0, int value1, int value2);
extern "C" void fn_801E3A88(UnidentifiedTextFader* fader, void* strings);
extern "C" void fn_801E3B60(UnidentifiedTextFader* fader, const char* text);
extern "C" void fn_801E3DB4(UnidentifiedTextFader* fader, float dt);
extern "C" void fn_801E4460(UnidentifiedTextFader* fader, TLInstance* instance);

// StrikerTimes texture streamer owned by the 0x801BFxxx translation unit.
extern "C" void fn_801BF1AC(UnidentifiedStrikerTimesImage* image, const char* name, int value);
extern "C" bool fn_801BF218(UnidentifiedStrikerTimesImage* image, bool value);

// Scroll widget owned by the 0x8022Fxxx/0x80230xxx translation units.
extern "C" void fn_8022F858(UnidentifiedScrollWidget* widget);
extern "C" bool fn_8022FD80(UnidentifiedScrollWidget* widget, int direction, int value);
extern "C" void fn_80230468(UnidentifiedScrollWidget* widget, TU80300104Event event, float dt);
extern "C" void fn_802308D0(UnidentifiedScrollWidget* widget, TLInstance* instance);
extern "C" void fn_80230B90(UnidentifiedScrollWidget* widget, int mode);
extern "C" void fn_80230DE0(UnidentifiedScrollWidget* widget, int value);

extern "C" void fn_801CBCA0(unsigned long hash, int value0, int value1, int value2);
extern "C" TLInstance* fn_8030677C(FEPresentation* pPresentation, unsigned long Level1, unsigned long Level2,
    unsigned long Level3, unsigned long Level4, unsigned long Level5, unsigned long Level6);
extern "C" void fn_801CC9B0(TU80219248Component* component, int value0, int value1);
extern "C" bool fn_80253E18();
extern "C" void fn_80253474();
extern "C" void fn_802534BC(int value0, int value1);

extern "C" nlVector2 fn_802197FC(int pad, u8* valid);

class UnidentifiedPadRumbleSource
{
public:
    virtual void Virtual00();
    virtual void Virtual04();
    virtual void Virtual08();
    virtual void Virtual0C();
    virtual void Virtual10();
    virtual void Virtual14();
    virtual void Virtual18();
    virtual void Virtual1C();
    virtual void Virtual20();
    virtual void Virtual24(int button, int value);
};

extern void* lbl_806E1E28;
extern "C" UnidentifiedPadRumbleSource* fn_802C082C(void* owner, int pad);

struct UnidentifiedHeadlineVariants
{
    /* 0x00 */ u8 mUnidentified00[0x6D];
    /* 0x6D */ s8 mUnidentified6D;
};

extern UnidentifiedHeadlineVariants* lbl_806E0FA0;

extern bool lbl_806DC704;
extern bool lbl_806E0F8B;
extern int lbl_806E18B0;
extern TLComponentInstance* lbl_80578450[4];
extern TLComponentInstance lbl_80580030;
extern TLInstance lbl_80580248;

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
        result = &lbl_80580030;
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

// Same contract as FEFinder<T, N>::Find: an unfound item is a null result, a
// found one is the requested instance type.
template <typename T>
static inline T* CastFound(TLInstance* found)
{
    if (found == 0)
        return 0;
    return (T*)found;
}

inline TLInstance* UnidentifiedSHSceneBase::FindCurrentInstance(const char* item)
{
    TLInstance* result;
    unsigned long itemHash = nlStringLowerHash(item);
    result = FEFinder<TLInstance, 2>::_Find(mPresentation->GetActiveSlide(), nlStringLowerHash("Layer"), itemHash, 0, 0, 0, 0);
    if (result == 0)
        return 0;
    return result;
}

inline TLComponentInstance* UnidentifiedSHSceneBase::FindCurrentComponent(const char* item)
{
    TLComponentInstance* result = (TLComponentInstance*)FindCurrentInstance(item);
    if (result == 0)
        result = &lbl_80580030;
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
            presentation->Update(presentation->GetActiveSlide()->m_start + presentation->GetActiveSlide()->m_duration);
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
        presentation->Update(presentation->GetActiveSlide()->m_start + presentation->GetActiveSlide()->m_duration);
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
            mUnidentified106 = GameInfoManager::Instance()->unknown_0x120 == 0;
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
    if (!mUnidentified420.mUnidentified00[0x18])
    {
        fn_802308D0(&mUnidentified420, FindCurrentComponent("scrollbar"));
        fn_8022F858(&mUnidentified420);
    }
    TU80300104Base::Callback callback(BindSHSceneAction(&UnidentifiedSHSceneBase::fn_8026ABF0, this));
    mComponent.fn_803007C0(callback);
    callback = TU80300104Base::Callback(BindSHSceneAction(&UnidentifiedSHSceneBase::fn_8026AD50, this));
    mComponent.fn_80300864(callback);
    TU80300104Base::Callback callback2(BindSHSceneAction(&UnidentifiedSHSceneBase::fn_8026AE98, this));
    mComponent.fn_803009AC(callback2);
    fn_801CC9B0(&mComponent, 0, 0);
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
    TU80300104Event event;
    mComponent.mPreviousEvents[0] = event;
    mComponent.mPreviousEvents[1] = event;
    mComponent.mPreviousEvents[2] = event;
    mComponent.mPreviousEvents[3] = event;
}

void UnidentifiedSHSceneBase::SceneCreated()
{
    fn_802308D0(&mUnidentified420, FindCurrentComponent("scrollbar"));
    if ((unsigned int)(mUnidentified28 - 0xB) <= 2)
    {
        fn_80230B90(&mUnidentified420, 0);
        mUnidentified109 = true;
    }
    else if (mUnidentified28 == 0xA)
    {
        fn_80230B90(&mUnidentified420, 2);
    }
    else
    {
        fn_80230B90(&mUnidentified420, 1);
    }
    fn_80230DE0(&mUnidentified420, 0);

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
        TLInstance* more = CastFound<TLInstance>(fn_8030677C(mPresentation, nlStringLowerHash("story"),
            nlStringLowerHash("Layer"), nlStringLowerHash("more"), 0, 0, 0));
        if (more == 0)
            more = &lbl_80580030;
        more->m_bVisible = false;
    }

    if (mUnidentified28 != 8 && mUnidentified28 != 0xD)
    {
        if (fn_80253E18())
            fn_802534BC(0, 1);
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
        TLInstance* logo = CastFound<TLInstance>(fn_8030677C(mPresentation, nlStringLowerHash("logo"),
            nlStringLowerHash("Layer"), itemHash, logoHash, 0, 0));
        mUnidentified380.mUnidentified08 = logo;
        fn_801BF1AC(&mUnidentified380, buffer, 0);
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
        mUnidentified109 = fn_801BF218(&mUnidentified380, true);
        return;
    }
    BaseSceneHandler::Update(dt);
    if (mUnidentified28 != 0xD && mUnidentified28 != 0xC && mUnidentified28 != 0xB)
    {
        if (!mUnidentified107)
            fn_8026A63C();
        fn_801BF218(&mUnidentified240, true);
        fn_801BF218(&mUnidentified2E0, true);
    }
    if (!mUnidentified108)
    {
        mUnidentified108 = true;
        fn_801CBCA0(0xAFE4352B, 0, 0, 1);
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
                lbl_80578450[pad]->SetActiveSlide("waiting", true, false);
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
        FEPresentation* presentation = mPresentation;
        TLSlide* slide = presentation->m_currentSlide;
        if (slide->m_time >= slide->m_start + slide->m_duration)
        {
            mUnidentified2C = 1;
            presentation->SetActiveSlide("headline pic", true);
            presentation->Update(0.0f);
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
        fn_801CBCA0(0x2AB04562, 0, 0, 1);
        TLSlide* slide = first;
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
        TLComponentInstance* instance = lbl_80578450[pad];
        if (mUnidentified28 != 0xC && pad != lbl_806E18B0)
        {
            instance->SetActiveSlide("waiting", true, false);
        }
        else
        {
            instance->SetActiveSlide("cursor", true, false);
            u8 valid = 1;
            TU80300104Event event;
            event.mIndex = pad;
            event.mPosition = fn_802197FC(pad, &valid);
            fn_802C082C(lbl_806E1E28, pad)->Virtual24(0x1E, 1);
            event.mFlag0 = g_pFEInput->JustPressed((eFEINPUT_PAD)pad, 0x1E, true, 0);
            event.mFlag1 = g_pFEInput->JustReleased((eFEINPUT_PAD)pad, 0x1E, true, 0);
            mComponent.fn_80219608(&event);
            if (mUnidentified30)
                return;
            fn_80230468(&mUnidentified420, event, dt);
            if (fn_8022FD80(&mUnidentified420, 1, 1))
            {
                fn_8026942C();
            }
            else if (fn_8022FD80(&mUnidentified420, 0, 1))
            {
                fn_8026932C();
            }
        }
    }
}

void UnidentifiedSHSceneBase::fn_8026A63C()
{
    FEPresentation* presentation = mFEScene->m_pFEPackage->GetPresentation();

    TLTextInstance* headlineText = CastFound<TLTextInstance>(fn_8030677C(presentation, nlStringLowerHash("headline pic"), nlStringLowerHash("Layer"),
        nlStringLowerHash("HEADLINE"), 0, 0, 0));
    if (headlineText == 0)
        headlineText = &UnidentifiedFallbackTextInstance;

    TLTextInstance* descriptionText = CastFound<TLTextInstance>(fn_8030677C(presentation, nlStringLowerHash("story"), nlStringLowerHash("Layer"),
        nlStringLowerHash("Description_clip"), 0, 0, 0));
    if (descriptionText == 0)
        descriptionText = &UnidentifiedFallbackTextInstance;

    fn_801E2F50(&mUnidentified1C0, headlineText, -1, -1, -300);
    if (mUnidentifiedFC)
        fn_801E3A88(&mUnidentified1C0, &mUnidentifiedF4);
    else
        fn_801E3B60(&mUnidentified1C0, mUnidentified31);
    fn_801E4460(&mUnidentified1C0, descriptionText);
    descriptionText->m_bVisible = false;

    TLTextInstance* storyHeadline = CastFound<TLTextInstance>(fn_8030677C(presentation, nlStringLowerHash("story"), nlStringLowerHash("Layer"),
        nlStringLowerHash("HEADLINE"), 0, 0, 0));
    if (storyHeadline == 0)
        storyHeadline = &UnidentifiedFallbackTextInstance;
    fn_801E2F50(&mUnidentified200, storyHeadline, -1, -1, -300);
    if (mUnidentifiedFC)
        fn_801E3A88(&mUnidentified200, &mUnidentifiedF4);
    else
        fn_801E3B60(&mUnidentified200, mUnidentified31);
    fn_801E4460(&mUnidentified200, descriptionText);

    TLTextInstance* bodyText = CastFound<TLTextInstance>(fn_8030677C(presentation, nlStringLowerHash("story"), nlStringLowerHash("Layer"),
        nlStringLowerHash("BODY"), 0, 0, 0));
    if (bodyText == 0)
        bodyText = &UnidentifiedFallbackTextInstance;
    if (mUnidentifiedFC)
        bodyText->SetString(mUnidentifiedF8.c_str());
    else
        bodyText->SetStringId(mUnidentified71);

    mUnidentified1C0.mUnidentified24 = 150.0f;
    mUnidentified200.mUnidentified24 = 150.0f;
    mUnidentified1C0.mUnidentified2C = 2.0f;
    mUnidentified200.mUnidentified2C = 2.0f;

    TLInstance* storyTexture = CastFound<TLInstance>(fn_8030677C(mPresentation, nlStringLowerHash("story"), nlStringLowerHash("Layer"),
        nlStringLowerHash("00_dummy_texture"), 0, 0, 0));
    if (storyTexture == 0)
        storyTexture = &lbl_80580248;
    mUnidentified240.mUnidentified08 = storyTexture;

    TLInstance* headlineTexture = CastFound<TLInstance>(fn_8030677C(mPresentation, nlStringLowerHash("headline pic"), nlStringLowerHash("Layer"),
        nlStringLowerHash("00_dummy_texture"), 0, 0, 0));
    if (headlineTexture == 0)
        headlineTexture = &lbl_80580248;
    mUnidentified2E0.mUnidentified08 = headlineTexture;

    fn_801BF1AC(&mUnidentified2E0, mUnidentifiedB1, 0);
    fn_801BF1AC(&mUnidentified240, mUnidentifiedB1, 0);
    mUnidentified107 = true;
}

void UnidentifiedSHSceneBase::SHSceneVirtual38(int captain, int mood, int special)
{
    int variant = nlRandom(3, &nlDefaultSeed);
    if (mUnidentified28 == 8)
    {
        s8 stored = lbl_806E0FA0->mUnidentified6D;
        if (stored != -1)
            variant = stored;
        else
            lbl_806E0FA0->mUnidentified6D = variant;
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
    mComponent.mValues[index] = 1;
    TLComponentInstance* done = FindCurrentComponent("done");
    if (!mComponent.fn_802192FC(1, index))
    {
        done->SetActiveSlide("over", true, false);
        fn_801CBCA0(0xAA73EF33, 0, 0, 1);
    }
}

void UnidentifiedSHSceneBase::fn_8026AD50(int index, void* context)
{
    mComponent.mValues[index] = 0;
    TLComponentInstance* done = FindCurrentComponent("done");
    if (!mComponent.fn_802192FC(1, index))
    {
        done->SetActiveSlide("off", true, false);
    }
}

void UnidentifiedSHSceneBase::fn_8026AE98(int index, void* context)
{
    fn_801CBCA0(0xF0AFD586, 0, 0, 1);
    mUnidentified30 = true;
    mUnidentified100 = 2;
    mPresentation->SetActiveSlide("out", true);
    mPresentation->Update(0.0f);
    if (mUnidentified28 != 0xD)
    {
        fn_80253E18();
        fn_80253474();
    }
    if (mUnidentified28 == 8)
    {
        fn_801CBCA0(0x4861E03D, 0, 0, 1);
    }
    FindCurrentComponent("done")->SetActiveSlide("down", true, false);
}
