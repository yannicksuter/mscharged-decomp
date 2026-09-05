#include "unclassified/tu_802582C4.h"

#include "Game/BaseGameSceneManager.h"
#include "Game/FE/feFinder.h"
#include "Game/FE/feInput.h"
#include "Game/FE/fePresentation.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlSlide.h"
#include "Game/FE/tlTextInstance.h"
#include "NL/globalpad.h"
#include "NL/nlBind.h"
#include "NL/nlString.h"
#include "unclassified/tu_802196B0.h"
#include "unclassified/tu_80252180.h"
#include "unclassified/tu_80300104.h"

extern "C" void fn_8022F848(UnidentifiedScrollWidget* widget, bool enabled);
extern "C" void fn_8022F858(UnidentifiedScrollWidget* widget);
extern "C" bool fn_8022FD80(UnidentifiedScrollWidget* widget, int direction, int value);
extern "C" void fn_80230468(UnidentifiedScrollWidget* widget, TU80300104Event event, float fDeltaT);
extern "C" void fn_802308D0(UnidentifiedScrollWidget* widget, TLInstance* instance);
extern "C" void fn_80230B90(UnidentifiedScrollWidget* widget, int mode);
extern "C" void fn_80230DE0(UnidentifiedScrollWidget* widget, int value);
extern "C" void fn_80238234(BaseSceneHandler* scene);
extern "C" bool fn_801CCD30(int cheat);
extern "C" bool fn_801CCDB8(int cheat);
extern "C" bool fn_801CCE30(int cheat);
extern "C" void fn_801CBCA0(unsigned long hash, int value0, int value1, int value2);
extern "C" const char* fn_801CCA08(int cheat);
extern "C" const char* fn_801CCA84(int cheat);
extern "C" const char* fn_801CCB00(int cheat);
extern "C" const char* fn_801CCB68(int cheat);
extern "C" const char* fn_801CCBD0(int cheat);
extern "C" const char* fn_801CCC80(int cheat);

extern TLComponentInstance* lbl_80578450[4];
extern BaseGameSceneManager* lbl_806E1838;
extern unsigned int lbl_806E18B0;
extern void* lbl_806E1E28;
extern "C" cGlobalPad* fn_802C082C(void* owner, int pad);

template <typename T>
static inline T* CastFound(TLInstance* found)
{
    if (found == 0)
    {
        return 0;
    }
    return (T*)found;
}

TU802582C4Scene::TU802582C4Scene()
    : mComponents()
    , mScrollWidget()
    , mUnidentified568(false)
    , mNavigation()
    , mUnidentified654(false)
    , mUnidentified658(0)
    , mUnidentified65C(0)
    , mUnidentified660(0)
    , mUnidentified664(-1)
{
    g_pFEInput->PushExclusiveInputLock(this, -1);

    for (int i = 0; i < 5; ++i)
    {
        mComponents[i].mContext = (void*)i;
        mComponents[i].mIgnoreInputLock = true;
        mComponents[i].mSpeakerEnabled = false;
    }

    fn_8022F848(&mScrollWidget, true);

    mUnidentified644[0] = 0;
    mUnidentified644[1] = 0;
    mUnidentified644[2] = 0;
    mUnidentified644[3] = 0;

    mNavigation.fn_801D2BE8(false);
    mNavigation.fn_801D2BE0(false);
    mNavigation.mIgnoreInputLock = true;
}

TU802582C4Scene::~TU802582C4Scene()
{
    g_pFEInput->PopExclusiveInputLock(this);

    TU80252180Scene* scene = fn_80253E18();
    if (scene != 0)
    {
        fn_802534BC(scene, mUnidentified654, true);
    }
}

void TU802582C4Scene::fn_802584CC(int index, void* context)
{
    bool unlocked = false;
    int item = (int)context;

    if (mUnidentified664 == 0 && fn_801CCD30(item + mUnidentified65C))
    {
        unlocked = true;
    }
    else if (mUnidentified664 == 2 && fn_801CCDB8(item + mUnidentified65C))
    {
        unlocked = true;
    }
    else if (mUnidentified664 == 1 && fn_801CCE30(item + mUnidentified65C))
    {
        unlocked = true;
    }

    if (!unlocked && mComponents[item].mValues[index] == 1)
    {
        --mUnidentified644[index];
        mUnidentified554[item]->SetActiveSlide("off", true, false);
        mComponents[item].mValues[index] = 0;
    }
    else if (unlocked && mComponents[item].mValues[index] == 0)
    {
        fn_802597E8(index, context);
    }
}

void TU802582C4Scene::fn_80258604(int index, void* context)
{
    bool unlocked = false;
    unsigned int which = index;
    unsigned int item = (unsigned int)context;

    if (mUnidentified664 == 0 && fn_801CCD30(item + mUnidentified65C))
    {
        unlocked = true;
    }
    else if (mUnidentified664 == 2 && fn_801CCDB8(item + mUnidentified65C))
    {
        unlocked = true;
    }
    else if (mUnidentified664 == 1 && fn_801CCE30(item + mUnidentified65C))
    {
        unlocked = true;
    }

    if (unlocked)
    {
        --mUnidentified644[index];
        if (!mComponents[item].fn_802192FC(1, which))
        {
            mUnidentified554[item]->SetActiveSlide("off", true, false);
        }
        mComponents[item].mValues[which] = 0;
    }
}

void TU802582C4Scene::fn_80258730(int item)
{
    const char* strings[2] = { "CHEATS_LOCKED", "CHEATS_LOCKED_DESC" };
    const char* slideNames[3] = { "off", "over", "down" };
    bool descriptionVisible = true;

    if (mUnidentified664 == 0)
    {
        if (fn_801CCD30(item + mUnidentified65C))
        {
            strings[0] = fn_801CCA08(item + mUnidentified65C);
            strings[1] = fn_801CCA84(item + mUnidentified65C);
        }
        if (item + mUnidentified65C == 0)
        {
            descriptionVisible = false;
        }
    }
    else if (mUnidentified664 == 2)
    {
        if (fn_801CCDB8(item + mUnidentified65C))
        {
            strings[0] = fn_801CCB00(item + mUnidentified65C);
            strings[1] = fn_801CCB68(item + mUnidentified65C);
        }
        if (item + mUnidentified65C == 0)
        {
            descriptionVisible = false;
        }
    }
    else if (mUnidentified664 == 1)
    {
        if (fn_801CCE30(item + mUnidentified65C))
        {
            strings[0] = fn_801CCBD0(item + mUnidentified65C);
            strings[1] = fn_801CCC80(item + mUnidentified65C);
        }
        if (item + mUnidentified65C == 0)
        {
            descriptionVisible = false;
        }
    }

    const char* challenge = "CHALLENGE_0";
    const char* stat0 = "stat_0";
    const char* stat1 = "stat_1";
    for (int i = 0; i < 3; ++i)
    {
        TLTextInstance* nameText = FEFinder<TLTextInstance, 3>::Find(
            mUnidentified554[item], nlStringLowerHash(slideNames[i]), nlStringLowerHash(challenge), nlStringLowerHash(stat0), 0, 0, 0);
        TLTextInstance* descriptionText = FEFinder<TLTextInstance, 3>::Find(
            mUnidentified554[item], nlStringLowerHash(slideNames[i]), nlStringLowerHash(challenge), nlStringLowerHash(stat1), 0, 0, 0);

        nameText->SetStringId(strings[0]);
        descriptionText->SetStringId(strings[1]);
        descriptionText->m_bVisible = descriptionVisible;
    }
}

void TU802582C4Scene::Update(float fDeltaT)
{
    BaseSceneHandler::Update(fDeltaT);

    int state = mUnidentified658;
    if (state == 0 || (unsigned int)(state - 2) <= 1)
    {
        TLSlide* slide = mPresentation->GetActiveSlide();
        if (slide->GetCurrentTime() < slide->m_start + slide->m_duration)
        {
            for (int pad = 0; pad < 4; ++pad)
            {
                lbl_80578450[pad]->SetActiveSlide("waiting", true, false);
            }
            return;
        }

        if (state == 0)
        {
            fn_802534BC(fn_80253E18(), 4, true);
            if (!mUnidentified568)
            {
                fn_80259330();
                mUnidentified568 = true;
            }
            mUnidentified658 = 1;
        }
        else if (state == 2)
        {
            lbl_806E1838->Pop();
            fn_80238234(lbl_806E1838->GetScene((SceneList)0x1B));
            return;
        }
        else if (state == 3)
        {
            lbl_806E1838->Pop();
            return;
        }
    }

    for (int pad = 0; pad < 4; ++pad)
    {
        TLComponentInstance* controller = lbl_80578450[pad];
        if ((unsigned int)pad != lbl_806E18B0)
        {
            controller->SetActiveSlide("waiting", true, false);
            continue;
        }

        if (mUnidentified644[pad] > 0)
        {
            controller->SetActiveSlide("A", true, false);
        }
        else
        {
            controller->SetActiveSlide("cursor", true, false);
        }

        unsigned char valid = 1;
        TU80300104Event event;
        event.mIndex = pad;
        event.mPosition = fn_802197FC(pad, &valid);
        fn_802C082C(lbl_806E1E28, pad)->Unidentified24(0x1E, true);
        event.mFlag0 = g_pFEInput->JustPressed((eFEINPUT_PAD)pad, 0x1E, true, 0);
        event.mFlag1 = g_pFEInput->JustReleased((eFEINPUT_PAD)pad, 0x1E, true, 0);

        if (mNavigation.fn_8022F2E0(event, fDeltaT))
        {
            mUnidentified658 = 3;
            fn_80253474(fn_80253E18());
            mPresentation->SetActiveSlide("out", true);
            return;
        }

        for (int i = 0; i < 5; ++i)
        {
            mComponents[i].fn_80219608(&event);
        }
        fn_80230468(&mScrollWidget, event, fDeltaT);
    }

    if (fn_8022FD80(&mScrollWidget, 1, 1))
    {
        ++mUnidentified65C;
        for (int i = 0; i < 5; ++i)
        {
            fn_80258730(i);
        }
    }
    else if (fn_8022FD80(&mScrollWidget, 0, 1))
    {
        --mUnidentified65C;
        for (int i = 0; i < 5; ++i)
        {
            fn_80258730(i);
        }
    }
}

void TU802582C4Scene::SceneCreated()
{
    TU80252180Scene* scene = fn_80253E18();
    TLComponentInstance* screen = 0;
    if (scene != 0)
    {
        mUnidentified654 = scene->mUnidentified1E4;
        fn_80253474(scene);
        screen = fn_80253D70(scene, 4);
    }
    mNavigation.fn_8022F194(screen);

    TLSlide* slide = mPresentation->GetActiveSlide();
    int itemCount = 0;
    TLComponentInstance* title = CastFound<TLComponentInstance>(
        FEFinder<TLComponentInstance, 2>::_Find<TLSlide>(slide,
            nlStringLowerHash("Layer"),
            nlStringLowerHash("TITLE2"),
            0,
            0,
            0,
            0));

    if (mUnidentified664 == 0)
    {
        itemCount = 6;
        title->SetActiveSlide("environment", true, false);
    }
    else if (mUnidentified664 == 2)
    {
        itemCount = 5;
        title->SetActiveSlide("player", true, false);
    }
    else if (mUnidentified664 == 1)
    {
        itemCount = 12;
        title->SetActiveSlide("pups", true, false);
    }

    int scrollRange = itemCount - 5;
    mUnidentified554[0] = CastFound<TLComponentInstance>(
        FEFinder<TLComponentInstance, 2>::_Find<TLSlide>(slide,
            nlStringLowerHash("Layer"),
            nlStringLowerHash("cheat_0"),
            0,
            0,
            0,
            0));
    mUnidentified554[1] = CastFound<TLComponentInstance>(
        FEFinder<TLComponentInstance, 2>::_Find<TLSlide>(slide,
            nlStringLowerHash("Layer"),
            nlStringLowerHash("cheat_1"),
            0,
            0,
            0,
            0));
    mUnidentified554[2] = CastFound<TLComponentInstance>(
        FEFinder<TLComponentInstance, 2>::_Find<TLSlide>(slide,
            nlStringLowerHash("Layer"),
            nlStringLowerHash("cheat_2"),
            0,
            0,
            0,
            0));
    mUnidentified554[3] = CastFound<TLComponentInstance>(
        FEFinder<TLComponentInstance, 2>::_Find<TLSlide>(slide,
            nlStringLowerHash("Layer"),
            nlStringLowerHash("cheat_3"),
            0,
            0,
            0,
            0));
    mUnidentified554[4] = CastFound<TLComponentInstance>(
        FEFinder<TLComponentInstance, 2>::_Find<TLSlide>(slide,
            nlStringLowerHash("Layer"),
            nlStringLowerHash("cheat_4"),
            0,
            0,
            0,
            0));

    TLComponentInstance* scrollbar = CastFound<TLComponentInstance>(
        FEFinder<TLComponentInstance, 2>::_Find<TLSlide>(slide,
            nlStringLowerHash("Layer"),
            nlStringLowerHash("scrollbar"),
            0,
            0,
            0,
            0));
    fn_802308D0(&mScrollWidget, scrollbar);
    fn_80230B90(&mScrollWidget, scrollRange);
    fn_80230DE0(&mScrollWidget, mUnidentified65C);

    for (int i = 0; i < 5; ++i)
    {
        fn_80258730(i);
    }
}

void TU802582C4Scene::fn_80259330()
{
    TU80300104Base::Callback over(
        Bind<void>(MemFun(&TU802582C4Scene::fn_802597E8), this, Placeholder<0>(), Placeholder<1>()));
    TU80300104Base::Callback off(
        Bind<void>(MemFun(&TU802582C4Scene::fn_80258604), this, Placeholder<0>(), Placeholder<1>()));
    TU80300104Base::Callback down(
        Bind<void>(MemFun(&TU802582C4Scene::fn_802584CC), this, Placeholder<0>(), Placeholder<1>()));
    TU80300104Base::Callback select(
        Bind<void>(MemFun(&TU802582C4Scene::fn_80259924), this, Placeholder<0>(), Placeholder<1>()));

    const char* listBack = "list_back_480x70 ";
    const char* challenge = "CHALLENGE_0";
    const char* slide = "off";
    for (int i = 0; i < 5; ++i)
    {
        TLComponentInstance* instance = FEFinder<TLComponentInstance, 3>::Find(
            mUnidentified554[i],
            nlStringLowerHash(slide),
            nlStringLowerHash(challenge),
            nlStringLowerHash(listBack),
            0,
            0,
            0);
        feVector3 position = mUnidentified554[i]->GetAssetPosition();
        mComponents[i].fn_80300D74(
            instance, true, position.f.x, position.f.y, 1.0f, 0.5f);
        mComponents[i].fn_803007C0(over);
        mComponents[i].fn_80300864(off);
        mComponents[i].fn_80300908(down);
        mComponents[i].fn_803009AC(select);
    }

    if (!mScrollWidget.mUnidentified00[0x18])
    {
        fn_8022F858(&mScrollWidget);
    }
}

void TU802582C4Scene::fn_802597E8(int index, void* context)
{
    bool unlocked = false;
    unsigned int item = (unsigned int)context;
    unsigned int which = index;

    if (mUnidentified664 == 0 && fn_801CCD30(item + mUnidentified65C))
    {
        unlocked = true;
    }
    else if (mUnidentified664 == 2 && fn_801CCDB8(item + mUnidentified65C))
    {
        unlocked = true;
    }
    else if (mUnidentified664 == 1 && fn_801CCE30(item + mUnidentified65C))
    {
        unlocked = true;
    }

    if (unlocked)
    {
        ++mUnidentified644[index];
        mComponents[item].fn_802195B4(index);
        if (!mComponents[item].fn_802192FC(1, which))
        {
            mUnidentified554[item]->SetActiveSlide("over", true, false);
            fn_801CBCA0(0xF6EB899E, 0, 0, 1);
        }
        mComponents[item].mValues[which] = 1;
    }
}

void TU802582C4Scene::fn_80259924(int, void* context)
{
    bool unlocked = false;
    int item = (int)context;

    if (mUnidentified664 == 0 && fn_801CCD30(item + mUnidentified65C))
    {
        unlocked = true;
    }
    else if (mUnidentified664 == 2 && fn_801CCDB8(item + mUnidentified65C))
    {
        unlocked = true;
    }
    else if (mUnidentified664 == 1 && fn_801CCE30(item + mUnidentified65C))
    {
        unlocked = true;
    }

    if (unlocked)
    {
        mUnidentified554[item]->SetActiveSlide("down", true, false);

        if (mUnidentified664 == 0)
        {
            mUnidentified660->mUnidentified04 = item + mUnidentified65C;
        }
        else if (mUnidentified664 == 2)
        {
            mUnidentified660->mUnidentified08 = item + mUnidentified65C;
        }
        else if (mUnidentified664 == 1)
        {
            mUnidentified660->mUnidentified00 = item + mUnidentified65C;
        }

        fn_801CBCA0(0xF0AFD586, 0, 0, 1);
        fn_801CBCA0(0xBB142B94, 0, 0, 1);
        mUnidentified658 = 2;
        fn_80253474(fn_80253E18());
        mPresentation->SetActiveSlide("out", true);
    }
}
