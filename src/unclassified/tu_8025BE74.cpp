#include "unclassified/tu_80259B88.h"

#include "Game/BaseGameSceneManager.h"
#include "Game/DB/StadiumInfo.h"
#include "Game/FE/feFinder.h"
#include "Game/FE/feInput.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlSlide.h"
#include "Game/FE/tlTextInstance.h"
#include "Game/tu_801360A4.h"
#include "NL/nlBasicString.h"
#include "NL/nlBind.h"
#include "NL/nlFormat.h"
#include "NL/nlFunction.h"
#include "NL/nlLocalization.h"
#include "unclassified/tu_802196B0.h"
#include "unclassified/tu_80252180.h"

extern "C" void fn_801CBCA0(unsigned long hash, int value0, int value1, int value2);
extern int fn_802AA91C(unsigned short* buffer, unsigned long size,
    const unsigned short* format, ...);
extern "C" void fn_80306208(UnidentifiedTimer_8030616C* timer, bool enabled);
extern "C" void fn_80306224(UnidentifiedTimer_8030616C* timer, float fDeltaT);
extern "C" void* memcpy(void* dest, const void* src, unsigned long count);
extern BaseGameSceneManager* lbl_806E1838;
extern TLComponentInstance* lbl_80578450[4];
extern unsigned int lbl_806E18B0;

typedef BasicString<unsigned short, Detail::TempStringAllocator> WideBasicString;

TU8025BE74Scene::TU8025BE74Scene()
    : mUnidentified1C()
    , mUnidentifiedD4(false)
    , mUnidentified3D6(false)
    , mUnidentified3D8(1.0f,
          Function<UnidentifiedTimer_8030616C*>(Bind<void>(
              MemFun(&TU8025BE74Scene::fn_8025C084), this, Placeholder<0>())))
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

void TU8025BE74Scene::fn_8025C084(UnidentifiedTimer_8030616C* timer)
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

extern "C" UnidentifiedFriendManager_801360A4* fn_801CA678();
extern "C" TLSlide* fn_801CA63C(FEPresentation* presentation);
extern "C" TLComponentInstance* fn_801CA680(int index);
extern "C" const unsigned short* fn_801CA950(nlLocalization* localization, const char* name);
extern "C" const unsigned short* fn_801CF604(const WideBasicString* string);
extern "C" const char* fn_801CCA08(int value);
extern "C" const char* fn_801CCBD0(int value);
extern "C" const char* fn_801CCB00(int value);
extern "C" TLComponentInstance* fn_801CA76C(TLSlide* slide,
    InlineHasher Level1, InlineHasher Level2, InlineHasher Level3,
    InlineHasher Level4 = InlineHasher(0UL), InlineHasher Level5 = InlineHasher(0UL),
    InlineHasher Level6 = InlineHasher(0UL));
extern "C" TLTextInstance* fn_802281EC(TLSlide* slide,
    InlineHasher Level1, InlineHasher Level2, InlineHasher Level3,
    InlineHasher Level4 = InlineHasher(0UL), InlineHasher Level5 = InlineHasher(0UL),
    InlineHasher Level6 = InlineHasher(0UL));
extern "C" WideBasicString fn_801CFE3C(const WideBasicString& string,
    const unsigned short* const& t0);

void TU8025BE74Scene::SceneCreated()
{
    UnidentifiedFriendStatusPayload* payload = fn_801CA678()->GetFriendStatusPayload_80136A88(
        fn_801CA678()->fn_8025D150());
    const PowerupSettings& settings = payload->mPowerupSettings;
    TLSlide* slide = fn_801CA63C(GetPresentation());
    mUnidentifiedD0 = fn_801CA76C(slide, "Layer", "PREVIEW", "BTN_1");
    for (int i = 0; i < 4; ++i)
    {
        fn_801CA680(i)->SetActiveSlide("waiting", true, false);
    }

    TLTextInstance* text = fn_802281EC(slide, "Layer", "PREVIEW", "OPTIONS", "OPTION_0");
    unsigned short value0[4];
    unsigned short value1[4];
    fn_802AA91C(value0, 4, (const unsigned short*)L"%d", payload->mGameplaySettings.m_unk10);
    WideBasicString string = Format(WideBasicString(fn_801CA950(g_pLocalization,
        "ONLINE_PREVIEW_OPTION_0")), value0);
    memcpy(mUnidentifiedD6, fn_801CF604(&string), sizeof(mUnidentifiedD6));
    text->SetString(mUnidentifiedD6);

    text = fn_802281EC(slide, "Layer", "PREVIEW", "OPTIONS", "OPTION_1");
    const char* id;
    int value;
    if (payload->mGameplaySettings.m_unk04 == 0)
    {
        id = "X_MINUTES";
        value = payload->mGameplaySettings.GameTime / 60;
    }
    else
    {
        id = "X_GOALS";
        value = payload->mGameplaySettings.m_unk0C;
    }
    fn_802AA91C(value1, 4, (const unsigned short*)L"%d", value);
    const unsigned short* format = fn_801CA950(g_pLocalization, "ONLINE_PREVIEW_OPTION_1");
    string = Format(WideBasicString(fn_801CA950(g_pLocalization, id)), value1);
    string = fn_801CFE3C(WideBasicString(format), fn_801CF604(&string));
    memcpy(mUnidentified156, fn_801CF604(&string), sizeof(mUnidentified156));
    text->SetString(mUnidentified156);

    text = fn_802281EC(slide, "Layer", "PREVIEW", "OPTIONS", "OPTION_2");
    int stadium = payload->mUnidentified34;
    string = fn_801CFE3C(WideBasicString(fn_801CA950(g_pLocalization, "ONLINE_PREVIEW_OPTION_2")),
        fn_801CA950(g_pLocalization, GetStadiumTickerStringID(stadium)));
    memcpy(mUnidentified1D6, fn_801CF604(&string), sizeof(mUnidentified1D6));
    text->SetString(mUnidentified1D6);

    text = fn_802281EC(slide, "Layer", "PREVIEW", "OPTIONS", "OPTION_3");
    id = fn_801CCA08(settings.fn_8025D158());
    string = fn_801CFE3C(WideBasicString(fn_801CA950(g_pLocalization, "ONLINE_PREVIEW_OPTION_3")),
        fn_801CA950(g_pLocalization, id));
    memcpy(mUnidentified256, fn_801CF604(&string), sizeof(mUnidentified256));
    text->SetString(mUnidentified256);

    text = fn_802281EC(slide, "Layer", "PREVIEW", "OPTIONS", "OPTION_4");
    id = fn_801CCBD0(settings.fn_8025D160());
    string = fn_801CFE3C(WideBasicString(fn_801CA950(g_pLocalization, "ONLINE_PREVIEW_OPTION_4")),
        fn_801CA950(g_pLocalization, id));
    memcpy(mUnidentified2D6, fn_801CF604(&string), sizeof(mUnidentified2D6));
    text->SetString(mUnidentified2D6);

    text = fn_802281EC(slide, "Layer", "PREVIEW", "OPTIONS", "OPTION_5");
    id = fn_801CCB00(settings.fn_8025D168());
    string = fn_801CFE3C(WideBasicString(fn_801CA950(g_pLocalization, "ONLINE_PREVIEW_OPTION_5")),
        fn_801CA950(g_pLocalization, id));
    memcpy(mUnidentified356, fn_801CF604(&string), sizeof(mUnidentified356));
    text->SetString(mUnidentified356);

    fn_801CBCA0(0xBB142B94, 0, 0, 1);
    fn_80306208(&mUnidentified3D8, true);
}

void TU8025BE74Scene::fn_8025CFC0(int index, void*)
{
    mUnidentifiedD0->SetActiveSlide("over", true, false);
    mUnidentified1C.mValues[index] = 1;
    lbl_80578450[index]->SetActiveSlide("A", true, false);
    fn_801CBCA0(0xDE912775, 0, 0, 1);
}

void TU8025BE74Scene::fn_8025D04C(int index, void*)
{
    mUnidentifiedD0->SetActiveSlide("off", true, false);
    mUnidentified1C.mValues[index] = 0;
    lbl_80578450[index]->SetActiveSlide("cursor", true, false);
}

void TU8025BE74Scene::fn_8025D0C0(int index, void*)
{
    mUnidentifiedD0->SetActiveSlide("down", true, false);
    mUnidentified1C.mValues[index] = 2;
    fn_801CBCA0(0xF0AFD586, 0, 0, 1);
    mUnidentified3F4 = 2;
    mPresentation->SetActiveSlide("out", true);
    mPresentation->Update(0.0f);
}
