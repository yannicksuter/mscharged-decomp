#include "unclassified/tu_80259B88.h"

#include "Game/BaseGameSceneManager.h"
#include "Game/GameInfo.h"
#include "Game/FE/feFinder.h"
#include "Game/FE/feInput.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlSlide.h"
#include "Game/FE/tlTextInstance.h"
#include "Game/tu_801360A4.h"
#include "NL/nlAlgorithm.h"
#include "NL/nlBasicString.h"
#include "NL/nlBind.h"
#include "NL/nlFunction.h"
#include "NL/nlLocalization.h"
#include "NL/nlString.h"
#include "unclassified/tu_802196B0.h"
#include "unclassified/tu_80252180.h"

extern "C" int fn_8004F594(int channel, const char* format, ...);
extern "C" void fn_801C3BEC();
extern "C" void fn_801CBCA0(unsigned long hash, int value0, int value1, int value2);
extern "C" void fn_801CC9B0(
    TU80219248Component* component, TLComponentInstance* instance, int value);
extern int fn_802AA91C(
    unsigned short* buffer, unsigned long size, const unsigned short* format, ...);
extern "C" bool fn_8025BD88();
extern "C" void fn_8025BD94(bool value);
extern "C" void fn_80261CE0();
extern "C" void fn_80306208(UnidentifiedTimer_8030616C* timer, bool enabled);
extern "C" void fn_80306224(UnidentifiedTimer_8030616C* timer, float fDeltaT);
extern "C" void* memcpy(void* dest, const void* src, unsigned long count);
extern BaseGameSceneManager* lbl_806E1838;
extern TLComponentInstance* lbl_80578450[4];
extern TLComponentInstance lbl_80580030;
extern unsigned short lbl_8058436C[];
extern unsigned int lbl_806E18B0;
extern void* lbl_806E18F8;
extern int lbl_806DE668[2];

typedef BasicString<unsigned short, Detail::TempStringAllocator> WideBasicString;

typedef void (TU8025BE74Scene::*TU8025BE74Callback)();

struct TU8025BE74CallbackRef
{
    TU8025BE74CallbackRef(TU8025BE74Callback callback)
        : mCallback(callback)
    {
    }

    TU8025BE74Callback mCallback;
};

struct TU8025BE74Binding
{
    TU8025BE74Callback mCallback;
    TU8025BE74Scene* mTarget;
    bool mUnidentified10;

    TU8025BE74Binding(TU8025BE74CallbackRef callback, TU8025BE74Scene* target)
        : mCallback(callback.mCallback)
        , mTarget(target)
    {
    }

    void operator()()
    {
        (mTarget->*mCallback)();
    }
};

static inline TU8025BE74Binding BindTU8025BE74Callback(
    TU8025BE74CallbackRef callback, TU8025BE74Scene* target)
{
    return TU8025BE74Binding(callback, target);
}

class UnidentifiedScene_8025AF0C : public BaseSceneHandler
{
public:
    /* 0x1C */ int mUnidentified1C;
    /* 0x20 */ float mUnidentified20;
};

template <typename T>
static inline T* CastFound(TLInstance* found)
{
    if (found == 0)
    {
        return 0;
    }
    return (T*)found;
}

static inline const unsigned short* LookupLocString(const char* id)
{
    nlLocalization* localization = g_pLocalization;
    unsigned long hash = nlStringLowerHash(id);
    if (localization->m_LookupTable == 0)
    {
        return LocalizationTableNotFound;
    }

    nlLocalization::StringLookup* lookup
        = nlBSearch<nlLocalization::StringLookup, unsigned long>(
            hash, localization->m_LookupTable, (int)localization->m_pFile->StringCount);
    if (lookup != 0)
    {
        return localization->m_FirstString + lookup->StringOffset;
    }

    return MissingLocString;
}

TU80259B88Scene::TU80259B88Scene()
    : mUnidentified1C(false)
    , mUnidentified20(0)
    , mNavigation()
    , mUnidentifiedFC()
    , mUnidentified1B0()
    , mUnidentified29C(false)
    , mUnidentified29D(false)
    , mUnidentified29E(false)
{
    fn_8025BD94(true);

    mUnidentified28C[0] = 0;
    mUnidentified28C[1] = 0;
    mUnidentified28C[2] = 0;
    mUnidentified28C[3] = 0;
    mUnidentified284 = lbl_806E18B0;
    mUnidentified288 = lbl_806DE668[1];

    WideBasicString string(LookupLocString("ONLINE_CONTROLLERS_GUEST"));
    memcpy(mUnidentified26C, string.c_str(), sizeof(mUnidentified26C));
    mNavigation.fn_801D2BE0(false);
}

TU80259B88Scene::~TU80259B88Scene()
{
}

void TU80259B88Scene::fn_80259C04()
{
    if (mUnidentified264->m_bVisible == true)
    {
        if (mUnidentified288 == -1)
        {
            mUnidentified29D = false;
            mUnidentified264->m_bVisible = false;
            mUnidentifiedFC.mDisabled = true;
            TU80300104Event event;
            mUnidentifiedFC.mPreviousEvents[0] = event;
            mUnidentifiedFC.mPreviousEvents[1] = event;
            mUnidentifiedFC.mPreviousEvents[2] = event;
            mUnidentifiedFC.mPreviousEvents[3] = event;

            for (int i = 0; i < 4; ++i)
            {
                if (mUnidentifiedFC.mValues[i] == 1)
                {
                    --mUnidentified28C[i];
                    mUnidentifiedFC.mValues[i] = 0;
                }
            }
        }
    }
    else if (mUnidentified288 != -1)
    {
        fn_801CBCA0(0x2AB04562, 0, 0, 1);
        mUnidentified264->m_bVisible = true;
        mUnidentified264->SetActiveSlide("in", true, false);
        mUnidentified29D = true;
    }
}

void TU80259B88Scene::fn_80259DB4(int index, void*)
{
    unsigned int which = index;
    if (mUnidentified288 != -1)
    {
        bool valid = mUnidentified284 == which || mUnidentified288 == which;
        if (!valid)
            return;
    }

    if (mUnidentified284 == which)
        return;

    if (!mUnidentified1B0.fn_802192FC(1, which))
    {
        mUnidentified268->SetActiveSlide("over", true, false);
        fn_801CBCA0(0xAA73EF33, 0, 0, 1);
    }

    mUnidentified1B0.mValues[which] = 1;
    ++mUnidentified28C[which];
    mUnidentified1B0.fn_802195B4(index);
}

void TU80259B88Scene::fn_80259E94(int index, void*)
{
    unsigned int which = index;
    if (mUnidentified288 != -1)
    {
        bool valid = mUnidentified284 == which || mUnidentified288 == which;
        if (!valid)
            return;
    }

    if (mUnidentified284 == which)
        return;

    if (!mUnidentified1B0.fn_802192FC(1, which))
        mUnidentified268->SetActiveSlide("controllers", true, false);

    mUnidentified1B0.mValues[which] = 0;
    --mUnidentified28C[which];
}

void TU80259B88Scene::fn_80259F54(int index, void*)
{
    TLComponentInstance* controller = lbl_80578450[index];
    unsigned int which = index;
    if (mUnidentified288 != -1)
    {
        bool valid = mUnidentified284 == which || mUnidentified288 == which;
        if (!valid)
            return;
    }

    if (mUnidentified284 == which)
        return;

    TLInstance* selected = FEFinder<TLInstance, 3>::Find(mUnidentified268,
        nlStringLowerHash("controllers"),
        nlStringLowerHash("home_group"),
        nlStringLowerHash("controller1"),
        0,
        0,
        0);
    TLTextInstance* text = FEFinder<TLTextInstance, 1>::Find(
        selected, nlStringLowerHash("Text"), 0, 0, 0, 0, 0);
    text->SetString(mUnidentified26C);

    TLInstance* highlighted = FEFinder<TLInstance, 3>::Find(mUnidentified268,
        nlStringLowerHash("over"),
        nlStringLowerHash("home_group"),
        nlStringLowerHash("controller1"),
        0,
        0,
        0);
    text = FEFinder<TLTextInstance, 1>::Find(
        highlighted, nlStringLowerHash("Text"), 0, 0, 0, 0, 0);
    text->SetString(mUnidentified26C);

    bool already = mUnidentified284 == which || mUnidentified288 == which;
    if (!already)
    {
        mUnidentified288 = index;
        controller->SetActiveSlide("A", true, false);
        selected->m_bVisible = true;
        highlighted->m_bVisible = true;
        fn_801CBCA0(0xB3586309, 0, 0, 1);
    }
    else
    {
        if (which == mUnidentified288)
            mUnidentified288 = -1;
        controller->SetActiveSlide("holding", true, false);
        selected->m_bVisible = false;
        highlighted->m_bVisible = false;
    }

    fn_80259C04();
}

void TU80259B88Scene::fn_8025A1A8(int index, void*)
{
    ++mUnidentified28C[index];
    mUnidentifiedFC.mValues[index] = 1;
    mUnidentifiedFC.fn_802195B4(index);
    if (!mUnidentifiedFC.fn_802192FC(1, index))
    {
        mUnidentified264->SetActiveSlide("over", true, false);
        fn_801CBCA0(0xAA73EF33, 0, 0, 1);
    }
}

void TU80259B88Scene::fn_8025A244(int index, void*)
{
    if (mUnidentifiedFC.mValues[index] != 0)
        return;

    ++mUnidentified28C[index];
    mUnidentifiedFC.mValues[index] = 1;
    mUnidentifiedFC.fn_802195B4(index);
    if (!mUnidentifiedFC.fn_802192FC(1, index))
    {
        mUnidentified264->SetActiveSlide("over", true, false);
        fn_801CBCA0(0xAA73EF33, 0, 0, 1);
    }
}

void TU80259B88Scene::fn_8025A2EC(int, void*)
{
    {
        mUnidentified1B0.mDisabled = true;
        TU80300104Event event;
        mUnidentified1B0.mPreviousEvents[0] = event;
        mUnidentified1B0.mPreviousEvents[1] = event;
        mUnidentified1B0.mPreviousEvents[2] = event;
        mUnidentified1B0.mPreviousEvents[3] = event;
    }
    {
        mUnidentifiedFC.mDisabled = true;
        TU80300104Event event;
        mUnidentifiedFC.mPreviousEvents[0] = event;
        mUnidentifiedFC.mPreviousEvents[1] = event;
        mUnidentifiedFC.mPreviousEvents[2] = event;
        mUnidentifiedFC.mPreviousEvents[3] = event;
    }

    for (int i = 0; i < 4; ++i)
    {
        lbl_80578450[i]->SetActiveSlide("waiting", true, false);
    }

    mUnidentified29E = true;
    mUnidentified20 = 2;

    TU80252180Scene* object = fn_80253E18();
    if (object != 0)
    {
        fn_80253474(object);
    }

    mPresentation->SetActiveSlide("out", true);
    mPresentation->Update(0.0f);
}

void TU80259B88Scene::SceneCreated()
{
    TLComponentInstance* screen = 0;
    TU80252180Scene* object = fn_80253E18();
    if (object != 0)
    {
        fn_80253474(object);
        screen = fn_80253D70(object, 4);
        mUnidentified264 = fn_80253D70(object, 0x20);
    }

    mNavigation.fn_8022F194(screen);
    mUnidentified264->m_bVisible = false;

    TLComponentInstance* sideGroup = CastFound<TLComponentInstance>(
        FEFinder<TLComponentInstance, 4>::_Find<TLSlide>(mPresentation->GetActiveSlide(),
            nlStringLowerHash("Layer"),
            nlStringLowerHash("home"),
            0,
            0,
            0,
            0));
    if (sideGroup == 0)
    {
        sideGroup = &lbl_80580030;
    }
    mUnidentified268 = sideGroup;
    mUnidentified268->SetActiveSlide("controllers", true, false);

    for (int i = 0; i < 4; ++i)
    {
        lbl_80578450[i]->SetActiveSlide("waiting", true, false);
    }

    TLInstance* homeController = FEFinder<TLInstance, 3>::Find(mUnidentified268,
        nlStringLowerHash("controllers"),
        nlStringLowerHash("home_group"),
        nlStringLowerHash("controller0"),
        0,
        0,
        0);
    TLTextInstance* text = FEFinder<TLTextInstance, 1>::Find(
        homeController, nlStringLowerHash("Text"), 0, 0, 0, 0, 0);
    homeController->m_bVisible = true;
    text->SetString(lbl_8058436C);

    TLInstance* homeOver = FEFinder<TLInstance, 3>::Find(mUnidentified268,
        nlStringLowerHash("over"),
        nlStringLowerHash("home_group"),
        nlStringLowerHash("controller0"),
        0,
        0,
        0);
    text = FEFinder<TLTextInstance, 1>::Find(
        homeOver, nlStringLowerHash("Text"), 0, 0, 0, 0, 0);
    homeOver->m_bVisible = true;
    text->SetString(lbl_8058436C);

    homeController = FEFinder<TLInstance, 3>::Find(mUnidentified268,
        nlStringLowerHash("controllers"),
        nlStringLowerHash("home_group"),
        nlStringLowerHash("controller1"),
        0,
        0,
        0);
    text = FEFinder<TLTextInstance, 1>::Find(
        homeController, nlStringLowerHash("Text"), 0, 0, 0, 0, 0);
    homeController->m_bVisible = mUnidentified288 != -1;
    text->SetString(mUnidentified26C);

    homeOver = FEFinder<TLInstance, 3>::Find(mUnidentified268,
        nlStringLowerHash("over"),
        nlStringLowerHash("home_group"),
        nlStringLowerHash("controller1"),
        0,
        0,
        0);
    text = FEFinder<TLTextInstance, 1>::Find(
        homeOver, nlStringLowerHash("Text"), 0, 0, 0, 0, 0);
    homeOver->m_bVisible = mUnidentified288 != -1;
    text->SetString(mUnidentified26C);

    fn_801CBCA0(0xBB142B94, 0, 0, 1);
}

void TU80259B88Scene::Update(float fDeltaT)
{
    BaseSceneHandler::Update(fDeltaT);

    if (mUnidentified29D)
    {
        TLSlide* slide = mUnidentified264->GetActiveSlide();
        if (slide->GetCurrentTime() >= slide->m_duration + slide->m_start)
        {
            fn_801CC9B0(&mUnidentifiedFC, mUnidentified264, 0);
            mUnidentifiedFC.mDisabled = false;
            mUnidentified29D = false;
        }
    }

    if (!lbl_806E1838->IsOnStack((SceneList)0xA)
        && !mUnidentified1C
        && lbl_806E1194->FindHostInvitation_80136AB0())
    {
        fn_8025BD94(false);
        int response = fn_8025BD88();
        UnidentifiedFriendManager_801360A4* friendManager = lbl_806E1194;
        friendManager->mUnidentified00C = 0x2B;
        friendManager->mUnidentified010 = response;
        lbl_806E1838->Push((SceneList)0x34, SCREEN_FORWARD, true);
        return;
    }

    int state = mUnidentified20;
    if (state == 0 || (unsigned int)(state - 2) <= 1)
    {
        TLSlide* slide = mPresentation->GetActiveSlide();
        if (slide->GetCurrentTime() < slide->m_duration + slide->m_start)
        {
            for (int pad = 0; pad < 4; ++pad)
            {
                lbl_80578450[pad]->SetActiveSlide("waiting", true, false);
            }
            return;
        }

        if (state == 0)
        {
            TU80252180Scene* scene = fn_80253E18();
            if (scene != 0)
            {
                fn_802534BC(scene, 0x24, true);
            }
            fn_8025B204();
            fn_80259C04();
            mUnidentified29C = true;
            mUnidentified20 = 1;

            if (mUnidentified288 != -1)
            {
                fn_801CBCA0(0x2AB04562, 0, 0, 1);
                mUnidentified264->m_bVisible = true;
                mUnidentified264->SetActiveSlide("in", true, false);
                mUnidentified29D = true;
            }
        }
        else if (state == 2)
        {
            lbl_806DE668[0] = mUnidentified284;
            lbl_806DE668[1] = mUnidentified288;

            if (mUnidentified1C)
            {
                fn_8004F594(16, "Respond invitation With Guest\n");
                fn_80261CE0();
            }
            else if (fn_8025BD88())
            {
                lbl_806E1838->Push((SceneList)0x31, SCREEN_FORWARD, true);
            }
            else
            {
                fn_801CBCA0(0x94A22E0E, 0, 0, 1);
                lbl_806E1838->Push((SceneList)0x1B, SCREEN_FORWARD, true);
            }
            fn_801C3BEC();
            return;
        }
        else if (state == 3)
        {
            if (mUnidentified1C)
            {
                lbl_806E1838->Push((SceneList)0x34, SCREEN_NOTHING, true);
            }
            else
            {
                SceneList nextScene = fn_8025BD88() ? (SceneList)0x29 : (SceneList)0x2A;
                lbl_806E1838->Push(nextScene, SCREEN_NOTHING, true);
            }
            fn_8025BD94(false);
            return;
        }
    }

    if (mUnidentified1C && !lbl_806E1194->ValidateHostInvitation_80136BC0())
    {
        fn_8025BD94(false);
        lbl_806E1838->Push((SceneList)0x35, SCREEN_FORWARD, true);
        UnidentifiedScene_8025AF0C* scene
            = (UnidentifiedScene_8025AF0C*)lbl_806E1838->GetScene((SceneList)0x35);
        scene->mUnidentified1C = 2;
        scene->mUnidentified20 = 2.0f;
        return;
    }

    for (int pad = 0; pad < 4; ++pad)
    {
        unsigned char valid = 1;
        TLComponentInstance* controller = lbl_80578450[pad];
        TU80300104Event event;
        event.mIndex = pad;
        event.mPosition = fn_802197FC(pad, &valid);
        event.mFlag0
            = g_pFEInput->JustPressed((eFEINPUT_PAD)pad, 0x1E, true, 0);

        mUnidentifiedFC.fn_80219608(&event);
        mUnidentified1B0.fn_80219608(&event);

        if (mNavigation.fn_8022F2E0(event, fDeltaT))
        {
            mUnidentified20 = 3;
            TU80252180Scene* scene = fn_80253E18();
            if (scene != 0)
            {
                fn_80253474(scene);
            }
            mPresentation->SetActiveSlide("out", true);
            mPresentation->Update(0.0f);
            return;
        }

        if (mUnidentified29E)
        {
            return;
        }

        bool playing
            = mUnidentified284 == (unsigned int)pad
           || mUnidentified288 == (unsigned int)pad;
        if (playing
            && !g_pFEInput->IsConnected((eFEINPUT_PAD)pad)
            && (unsigned int)pad != mUnidentified284)
        {
            controller->SetActiveSlide("holding", true, false);
            if ((unsigned int)pad == mUnidentified288)
            {
                mUnidentified288 = -1;
            }
            mUnidentified28C[pad] = 0;

            TLInstance* homeController = FEFinder<TLInstance, 3>::Find(mUnidentified268,
                nlStringLowerHash("controllers"),
                nlStringLowerHash("home_group"),
                nlStringLowerHash("controller1"),
                0,
                0,
                0);
            homeController->m_bVisible = false;

            TLInstance* homeOver = FEFinder<TLInstance, 3>::Find(mUnidentified268,
                nlStringLowerHash("over"),
                nlStringLowerHash("home_group"),
                nlStringLowerHash("controller1"),
                0,
                0,
                0);
            homeOver->m_bVisible = false;
            fn_80259C04();
        }

        playing
            = mUnidentified284 == (unsigned int)pad
           || mUnidentified288 == (unsigned int)pad;
        if (!playing)
        {
            controller->SetActiveSlide("holding", true, false);
        }
        else if (mNavigation.mUnidentifiedD2[pad] || mUnidentified28C[pad] > 0)
        {
            controller->SetActiveSlide("A", true, false);
        }
        else
        {
            controller->SetActiveSlide("cursor", true, false);
        }
    }
}

void TU80259B88Scene::fn_8025B204()
{
    TLInstance* homeAwayBox = CastFound<TLInstance>(
        FEFinder<TLInstance, 2>::_Find<TLSlide>(mUnidentified268->GetActiveSlide(),
            nlStringLowerHash("home_group"),
            nlStringLowerHash("home_away_box"),
            0,
            0,
            0,
            0));
    mUnidentified1B0.fn_80300D74(homeAwayBox, true, 0.0f, 0.0f, 1.0f, 1.0f);

    TU80300104Base::Callback callback(Bind<void>(
        MemFun(&TU80259B88Scene::fn_80259DB4), this, Placeholder<0>(), Placeholder<1>()));
    mUnidentified1B0.fn_803007C0(callback);
    callback = TU80300104Base::Callback(Bind<void>(
        MemFun(&TU80259B88Scene::fn_80259E94), this, Placeholder<0>(), Placeholder<1>()));
    mUnidentified1B0.fn_80300864(callback);

    TU80300104Base::Callback selectCallback(Bind<void>(
        MemFun(&TU80259B88Scene::fn_80259F54), this, Placeholder<0>(), Placeholder<1>()));
    mUnidentified1B0.fn_803009AC(selectCallback);
    mUnidentified1B0.mSpeakerEnabled = false;

    callback = TU80300104Base::Callback(Bind<void>(
        MemFun(&TU80259B88Scene::fn_8025A1A8), this, Placeholder<0>(), Placeholder<1>()));
    mUnidentifiedFC.fn_803007C0(callback);
    callback = TU80300104Base::Callback(Bind<void>(
        MemFun(&TU80259B88Scene::fn_8025BC24), this, Placeholder<0>(), Placeholder<1>()));
    mUnidentifiedFC.fn_80300864(callback);
    callback = TU80300104Base::Callback(Bind<void>(
        MemFun(&TU80259B88Scene::fn_8025A244), this, Placeholder<0>(), Placeholder<1>()));
    mUnidentifiedFC.fn_80300908(callback);
    selectCallback = TU80300104Base::Callback(Bind<void>(
        MemFun(&TU80259B88Scene::fn_8025A2EC), this, Placeholder<0>(), Placeholder<1>()));
    mUnidentifiedFC.fn_803009AC(selectCallback);

    {
        mUnidentifiedFC.mDisabled = true;
        TU80300104Event event;
        mUnidentifiedFC.mPreviousEvents[0] = event;
        mUnidentifiedFC.mPreviousEvents[1] = event;
        mUnidentifiedFC.mPreviousEvents[2] = event;
        mUnidentifiedFC.mPreviousEvents[3] = event;
    }
}

void TU80259B88Scene::fn_8025BC24(int index, void*)
{
    --mUnidentified28C[index];
    mUnidentifiedFC.mValues[index] = 0;
    if (!mUnidentifiedFC.fn_802192FC(1, index))
    {
        mUnidentified264->SetActiveSlide("in", true, false);
    }
}

extern "C" void fn_8025BD7C(bool value)
{
    GameInfoManager::Instance()->unknown_0x124 = value;
}

extern "C" bool fn_8025BD88()
{
    return GameInfoManager::Instance()->unknown_0x124;
}

extern "C" void fn_8025BD94(bool value)
{
    GameInfoManager::Instance()->unknown_0x125 = value;
}

extern "C" bool fn_8025BDA0()
{
    return GameInfoManager::Instance()->unknown_0x125;
}

extern "C" void fn_8025BDAC(bool value)
{
    GameInfoManager::Instance()->unknown_0x126 = value;
}

extern "C" bool fn_8025BDB8()
{
    return GameInfoManager::Instance()->unknown_0x126;
}

extern "C" void fn_8025BDC4(void* context)
{
    lbl_806E18F8 = context;
}

extern "C" void fn_8025BDCC(unsigned long long friendKey, u16* output)
{
    if (friendKey != 0)
    {
        fn_802AA91C(output,
            14,
            (const unsigned short*)L"%.6llu %.6llu",
            friendKey / 1000000,
            friendKey % 1000000);
    }
    else
    {
        fn_802AA91C(output, 14, (const unsigned short*)L"UNKNOWN");
    }
}

TU8025BE74Scene::TU8025BE74Scene()
    : mUnidentified1C()
    , mUnidentifiedD4(false)
    , mUnidentified3D6(false)
    , mUnidentified3D8(1.0f,
          Function<FnVoidVoid>(BindTU8025BE74Callback(
              TU8025BE74CallbackRef(&TU8025BE74Scene::fn_8025C084), this)))
{
    mUnidentified3F4 = 0;

    TU80252180Scene* object = fn_80253E18();
    if (object != 0)
    {
        fn_802534BC(object, 0, true);
    }
}

TU8025BE74Scene::~TU8025BE74Scene()
{
}

void TU8025BE74Scene::fn_8025C084()
{
    mUnidentified3D6 = true;
    fn_80306208(&mUnidentified3D8, false);
}

void TU8025BE74Scene::Update(float fDeltaT)
{
    BaseSceneHandler::Update(fDeltaT);
    fn_80306224(&mUnidentified3D8, fDeltaT);

    int state = mUnidentified3F4;
    if (state == 0 || (unsigned int)(state - 2) <= 1)
    {
        TLSlide* slide = mPresentation->GetActiveSlide();
        if (slide->GetCurrentTime() < slide->m_duration + slide->m_start)
        {
            for (int pad = 0; pad < 4; ++pad)
            {
                lbl_80578450[pad]->SetActiveSlide("waiting", true, false);
            }
            return;
        }

        if (state == 0)
        {
            TU80300104Base::Callback over(Bind<void>(
                MemFun(&TU8025BE74Scene::fn_8025CFC0), this, Placeholder<0>(), Placeholder<1>()));
            TU80300104Base::Callback off(Bind<void>(
                MemFun(&TU8025BE74Scene::fn_8025D04C), this, Placeholder<0>(), Placeholder<1>()));
            TU80300104Base::Callback select(Bind<void>(
                MemFun(&TU8025BE74Scene::fn_8025D0C0), this, Placeholder<0>(), Placeholder<1>()));

            mUnidentified1C.fn_80300D74(
                mUnidentifiedD0, true, 0.0f, 0.0f, 1.0f, 1.0f);
            mUnidentified1C.fn_803007C0(over);
            mUnidentified1C.fn_80300864(off);
            mUnidentified1C.fn_803009AC(select);
            mUnidentifiedD4 = true;
            mUnidentified3F4 = 1;
        }
        else if (state == 2)
        {
            lbl_806E1838->Push((SceneList)0x34, SCREEN_FORWARD, true);
            return;
        }
    }

    if (mUnidentified3D8.mEnabled)
    {
        return;
    }

    if (!lbl_806E1194->ValidateHostInvitation_80136BC0())
    {
        lbl_806E1838->Push((SceneList)0x35, SCREEN_FORWARD, true);
        UnidentifiedScene_8025AF0C* scene
            = (UnidentifiedScene_8025AF0C*)lbl_806E1838->GetScene((SceneList)0x35);
        scene->mUnidentified1C = 2;
        scene->mUnidentified20 = 2.0f;
        return;
    }

    for (int pad = 0; pad < 4; ++pad)
    {
        TLComponentInstance* controller = lbl_80578450[pad];
        if ((unsigned int)pad != lbl_806E18B0)
        {
            controller->SetActiveSlide("waiting", true, false);
        }
        else
        {
            unsigned char valid = 1;
            TU80300104Event event;
            event.mIndex = pad;
            event.mPosition = fn_802197FC(pad, &valid);
            event.mFlag0
                = g_pFEInput->JustPressed((eFEINPUT_PAD)pad, 0x1E, true, 0);
            mUnidentified1C.fn_80219608(&event);
        }
    }
}
