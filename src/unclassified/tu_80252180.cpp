#include "unclassified/tu_80252180.h"

#include "Game/DB/CharacterInfo.h"
#include "Game/FE/feFinder.h"
#include "Game/FE/fePresentation.h"
#include "Game/FE/feScene.h"
#include "Game/FE/feSceneManager.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlInstance.h"
#include "Game/FE/tlSlide.h"
#include "Game/FE/tlTextInstance.h"
#include "Game/GameInfo.h"
#include "Game/Render/Presentation.h"
#include "NL/nlBasicString.h"
#include "NL/nlColour.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"
#include "NL/nlTask.h"
#include "unclassified/tu_802196B0.h"

struct PlatPadManager;

extern void* lbl_806E1E28;
extern PlatPadManager* lbl_806E2478;
extern TLComponentInstance* lbl_80578450[4];
extern TLComponentInstance lbl_80580030;
bool lbl_806DE828 = true;
extern bool lbl_806E18F0;
NLString lbl_806E18F4;
extern char lbl_8051F7B0[];
extern char lbl_8051F7BC[];
extern char lbl_8051F7C8[];
extern char lbl_8051F7D4[];
extern char lbl_8051F7E0[];
extern char lbl_8051F7EC[];
extern char lbl_8051F7F8[];
extern char lbl_8051F804[];
extern char lbl_8051F810[];
extern char lbl_806DE878[4];
extern char lbl_806DE87C[6];
extern char lbl_806DE884[4];
extern char lbl_806DE888[5];
extern char lbl_806DE890[4];
extern char lbl_806DE898[4];
extern char lbl_806DE8A0[4];
extern char lbl_806DE8A8[4];
extern char lbl_806DE8C0[7];

extern "C"
{
    void* fn_80077224();
    Presentation* fn_801FEEAC();
    void fn_801CA610(TLInstance* instance, bool visible);
    TLSlide* fn_801CA63C(FEPresentation* presentation);
    bool fn_801CA64C(void* object);
    void* fn_801CA650(void* object);
    void* fn_801CA668();
    TLInstance* fn_802281EC(TLSlide* slide, const InlineHasher& level1,
        InlineHasher level2 = InlineHasher(0UL), InlineHasher level3 = InlineHasher(0UL),
        InlineHasher level4 = InlineHasher(0UL), InlineHasher level5 = InlineHasher(0UL),
        InlineHasher level6 = InlineHasher(0UL));
    void* fn_802C082C(void* manager, int index);
    void fn_80375DF8(PlatPadManager* manager, int channel, bool enabled);
    void fn_802547E8(TU80252180Scene* scene, int value);
    void fn_80254084(int index, TLComponentInstance* component);
    TLComponentInstance* fn_80254098(TLSlide* slide, const InlineHasher& level1,
        InlineHasher level2 = InlineHasher(0UL), InlineHasher level3 = InlineHasher(0UL),
        InlineHasher level4 = InlineHasher(0UL), InlineHasher level5 = InlineHasher(0UL),
        InlineHasher level6 = InlineHasher(0UL));
    TLComponentInstance* fn_80254170(TLSlide* slide, const InlineHasher& level1,
        InlineHasher level2 = InlineHasher(0UL), InlineHasher level3 = InlineHasher(0UL),
        InlineHasher level4 = InlineHasher(0UL), InlineHasher level5 = InlineHasher(0UL),
        InlineHasher level6 = InlineHasher(0UL));
    TLSlide* fn_80254238(TLSlide* slide, const InlineHasher& level1,
        InlineHasher level2 = InlineHasher(0UL), InlineHasher level3 = InlineHasher(0UL),
        InlineHasher level4 = InlineHasher(0UL), InlineHasher level5 = InlineHasher(0UL),
        InlineHasher level6 = InlineHasher(0UL));
}

TU80252180Scene::TU80252180Scene()
    : mUnidentified058(true)
{
    mUnidentified1E5 = false;
    mUnidentified1E6 = false;
    mUnidentified1E7 = false;
    mUnidentified1E8 = false;
    mUnidentified1E0 = 0;
    mUnidentified1E4 = false;

    for (int channel = 0; channel < 4; ++channel)
    {
        if (fn_802C082C(lbl_806E1E28, channel) != 0)
        {
            fn_80375DF8(lbl_806E2478, channel, true);
        }
    }
}

TU80252180Scene::~TU80252180Scene()
{
    for (int channel = 0; channel < 4; ++channel)
    {
        if (fn_802C082C(lbl_806E1E28, channel) != 0)
        {
            fn_80375DF8(lbl_806E2478, channel, false);
        }
        lbl_80578450[channel] = 0;
    }
}

void TU80252180Scene::SceneCreated()
{
    TLSlide* layer = fn_80254238(fn_801CA63C(GetPresentation()), InlineHasher("Layer"));

    for (int index = 0; index < 4; ++index)
    {
        char name[64];
        nlSNPrintf(name, sizeof(name), lbl_8051F7B0, index);
        mUnidentified01C[index] = fn_80254170(layer, InlineHasher(name));
        fn_80254084(index, mUnidentified01C[index]);
        mUnidentified01C[index]->SetActiveSlide("waiting", true, false);
    }

    mUnidentified034 = fn_80254098(layer, InlineHasher("back"));
    mUnidentified038 = fn_80254098(layer, InlineHasher(lbl_8051F7BC));
    mUnidentified03C = fn_80254098(layer, InlineHasher(lbl_8051F7C8));
    mUnidentified040 = fn_80254098(layer, InlineHasher("done"));
    mUnidentified044 = fn_80254098(layer, InlineHasher(lbl_8051F7D4));
    mUnidentified048 = fn_80254098(layer, InlineHasher(lbl_8051F7E0));
    mUnidentified04C = fn_80254098(layer, InlineHasher(lbl_8051F7EC));
    fn_801CA610(mUnidentified04C, false);
    mUnidentified050 = fn_80254098(layer, InlineHasher("no home"));
    fn_801CA610(mUnidentified050, false);

    TLComponentInstance* timer = fn_80254098(layer, InlineHasher(lbl_8051F7F8));
    if (fn_801CA64C(fn_801CA650(fn_80077224())))
    {
        mUnidentified02C = fn_80254098(layer, InlineHasher("+16:9"));
        mUnidentified030 = fn_80254098(layer, InlineHasher("-16:9"));
        fn_801CA610(fn_80254098(layer, InlineHasher("+")), false);
        fn_801CA610(fn_80254098(layer, InlineHasher("-")), false);
        timer->SetActiveSlide("16:9", true, false);
        mUnidentified1E8 = true;
        mUnidentified050->SetActiveSlide(lbl_8051F804, true, false);
    }
    else
    {
        mUnidentified02C = fn_80254098(layer, InlineHasher("+"));
        mUnidentified030 = fn_80254098(layer, InlineHasher("-"));
        fn_801CA610(fn_80254098(layer, InlineHasher("+16:9")), false);
        fn_801CA610(fn_80254098(layer, InlineHasher("-16:9")), false);
        timer->SetActiveSlide(lbl_806DE878, true, false);
    }

    mUnidentified058.fn_802385C8(mUnidentified030);
    mUnidentified058.fn_802385C0(mUnidentified02C);

    mUnidentified054
        = fn_802281EC(timer->GetActiveSlide(), InlineHasher(lbl_806DE87C));
    fn_801CA610(mUnidentified054, false);
    fn_802533F0(this);
    if (fn_801CA668() != 0)
    {
        fn_80253F30(this);
    }
}

void TU80252180Scene::Update(float fDeltaT)
{
    BaseSceneHandler::Update(fDeltaT);
    mVisible = lbl_806DE828;

    for (int index = 0; index < 4; ++index)
    {
        if (mUnidentified01C[index] == 0)
        {
            mUnidentified01C[index]->m_bVisible = false;
            continue;
        }

        u16 angle;
        u8 valid = true;
        nlVector2 position = fn_80219824(index, &angle, &valid);
        if (valid)
        {
            mUnidentified01C[index]->SetAssetPosition(position.x, position.y, 0.0f);
            mUnidentified01C[index]->SetAssetRotation(
                0.0f, 0.0f, AngUnitsToRad_fromUnsignedShort((u16)-angle));
        }
        mUnidentified01C[index]->m_bVisible = valid && !lbl_806E18F0;
    }

    if (mUnidentified1E5)
    {
        if (mUnidentified1E6)
        {
            mUnidentified04C->m_bVisible = true;
            mUnidentified1E6 = false;
            const char* functionName = lbl_806E18F4.c_str();
            fn_801FEEAC()->Call(functionName);
        }

        if (mUnidentified04C->GetActiveSlide()->m_time >= 0.6f)
        {
            mUnidentified04C->m_bVisible = false;
            mUnidentified1E5 = false;
            lbl_806DE828 = !lbl_806E18F0;
        }
    }

    if (mUnidentified1E7)
    {
        TLSlide* slide = mUnidentified050->GetActiveSlide();
        if (slide->m_time >= slide->m_duration + slide->m_start)
        {
            mUnidentified050->m_bVisible = false;
            mUnidentified1E7 = false;

            if ((nlTaskManager::m_pInstance->mCurrentState & 4) == 0)
            {
                mUnidentified02C->m_bVisible = mUnidentified1E4 & 1;
                mUnidentified030->m_bVisible = (mUnidentified1E4 >> 1) & 1;
                mUnidentified034->m_bVisible = (mUnidentified1E4 >> 2) & 1;
                mUnidentified038->m_bVisible = (mUnidentified1E4 >> 3) & 1;
                mUnidentified03C->m_bVisible = (mUnidentified1E4 >> 4) & 1;
                mUnidentified040->m_bVisible = (mUnidentified1E4 >> 5) & 1;
                mUnidentified044->m_bVisible = (mUnidentified1E4 >> 6) & 1;
                mUnidentified048->m_bVisible = (mUnidentified1E4 >> 7) & 1;
            }
        }
    }
}

extern "C" void fn_80253284(bool value)
{
    if (lbl_806DE828 != value)
    {
        if (value != 0)
        {
            for (int channel = 0; channel < 4; ++channel)
            {
                if (fn_802C082C(lbl_806E1E28, channel) != 0)
                {
                    fn_80375DF8(lbl_806E2478, channel, true);
                }
            }
        }
        else
        {
            for (int channel = 0; channel < 4; ++channel)
            {
                if (fn_802C082C(lbl_806E1E28, channel) != 0)
                {
                    fn_80375DF8(lbl_806E2478, channel, false);
                }
            }
        }
        lbl_806DE828 = value;
    }

    lbl_806E18F0 = !lbl_806DE828;
}

extern "C" void fn_80253348(TU80252180Scene* scene, int mask, bool visible)
{
    if (visible)
    {
        scene->mUnidentified1E4 |= mask;
    }
    else
    {
        scene->mUnidentified1E4 &= ~mask;
    }

    scene->mUnidentified02C->m_bVisible = scene->mUnidentified1E4 & 1;
    scene->mUnidentified030->m_bVisible = (scene->mUnidentified1E4 >> 1) & 1;
    scene->mUnidentified034->m_bVisible = (scene->mUnidentified1E4 >> 2) & 1;
    scene->mUnidentified038->m_bVisible = (scene->mUnidentified1E4 >> 3) & 1;
    scene->mUnidentified03C->m_bVisible = (scene->mUnidentified1E4 >> 4) & 1;
    scene->mUnidentified040->m_bVisible = (scene->mUnidentified1E4 >> 5) & 1;
    scene->mUnidentified044->m_bVisible = (scene->mUnidentified1E4 >> 6) & 1;
    scene->mUnidentified048->m_bVisible = (scene->mUnidentified1E4 >> 7) & 1;
}

extern "C" void fn_802533F0(TU80252180Scene* scene)
{
    scene->mUnidentified02C->m_bVisible = scene->mUnidentified1E4 & 1;
    scene->mUnidentified030->m_bVisible = (scene->mUnidentified1E4 >> 1) & 1;
    scene->mUnidentified034->m_bVisible = (scene->mUnidentified1E4 >> 2) & 1;
    scene->mUnidentified038->m_bVisible = (scene->mUnidentified1E4 >> 3) & 1;
    scene->mUnidentified03C->m_bVisible = (scene->mUnidentified1E4 >> 4) & 1;
    scene->mUnidentified040->m_bVisible = (scene->mUnidentified1E4 >> 5) & 1;
    scene->mUnidentified044->m_bVisible = (scene->mUnidentified1E4 >> 6) & 1;
    scene->mUnidentified048->m_bVisible = (scene->mUnidentified1E4 >> 7) & 1;
}

extern "C" void fn_80253474(TU80252180Scene* scene)
{
    scene->mUnidentified02C->m_bVisible = false;
    scene->mUnidentified030->m_bVisible = false;
    scene->mUnidentified034->m_bVisible = false;
    scene->mUnidentified038->m_bVisible = false;
    scene->mUnidentified03C->m_bVisible = false;
    scene->mUnidentified040->m_bVisible = false;
    scene->mUnidentified044->m_bVisible = false;
    scene->mUnidentified048->m_bVisible = false;
}

extern "C" void fn_802534BC(TU80252180Scene* scene, int value, bool enabled)
{
    scene->mUnidentified1E4 = 0;

    TLComponentInstance* component = FEFinder<TLComponentInstance, 4>::Find(
        scene->mUnidentified034->GetActiveSlide(), InlineHasher("back"));
    if (component == 0)
    {
        component = &lbl_80580030;
    }
    component->SetActiveSlide(lbl_806DE884, true, false);

    if (enabled)
    {
        scene->mUnidentified02C->SetActiveSlide(lbl_806DE884, true, false);
        scene->mUnidentified030->SetActiveSlide(lbl_806DE884, true, false);
    }

    scene->mUnidentified03C->SetActiveSlide(lbl_806DE884, true, false);
    scene->mUnidentified040->SetActiveSlide(lbl_806DE884, true, false);
    scene->mUnidentified044->SetActiveSlide(lbl_806DE884, true, false);
    scene->mUnidentified048->SetActiveSlide(lbl_806DE884, true, false);
    fn_802547E8(scene, 0);

    component = FEFinder<TLComponentInstance, 4>::Find(
        scene->mUnidentified034->GetActiveSlide(), InlineHasher("back"));
    if (component == 0)
    {
        component = &lbl_80580030;
    }

    TLTextInstance* text0 = FEFinder<TLTextInstance, 4>::Find(component,
        nlStringLowerHash(lbl_806DE884), nlStringLowerHash(lbl_8051F810), 0, 0, 0, 0);
    if (text0 == 0)
    {
        text0 = &UnidentifiedFallbackTextInstance;
    }

    TLTextInstance* text1 = FEFinder<TLTextInstance, 4>::Find(component,
        nlStringLowerHash(lbl_806DE888), nlStringLowerHash(lbl_8051F810), 0, 0, 0, 0);
    if (text1 == 0)
    {
        text1 = &UnidentifiedFallbackTextInstance;
    }

    TLTextInstance* text2 = FEFinder<TLTextInstance, 4>::Find(component,
        nlStringLowerHash(lbl_806DE890), nlStringLowerHash(lbl_8051F810), 0, 0, 0, 0);
    if (text2 == 0)
    {
        text2 = &UnidentifiedFallbackTextInstance;
    }

    text0->SetStringId(lbl_806DE898);
    text1->SetStringId(lbl_806DE898);
    text2->SetStringId(lbl_806DE898);

    TLTextInstance* text3 = FEFinder<TLTextInstance, 4>::Find(scene->mUnidentified040,
        nlStringLowerHash(lbl_806DE884), nlStringLowerHash(lbl_806DE8A0),
        nlStringLowerHash("done"), 0, 0, 0);
    if (text3 == 0)
    {
        text3 = &UnidentifiedFallbackTextInstance;
    }

    TLTextInstance* text4 = FEFinder<TLTextInstance, 4>::Find(scene->mUnidentified040,
        nlStringLowerHash(lbl_806DE888), nlStringLowerHash(lbl_806DE8A0),
        nlStringLowerHash("done"), 0, 0, 0);
    if (text4 == 0)
    {
        text4 = &UnidentifiedFallbackTextInstance;
    }

    TLTextInstance* text5 = FEFinder<TLTextInstance, 4>::Find(scene->mUnidentified040,
        nlStringLowerHash(lbl_806DE890), nlStringLowerHash(lbl_806DE8A0),
        nlStringLowerHash("done"), 0, 0, 0);
    if (text5 == 0)
    {
        text5 = &UnidentifiedFallbackTextInstance;
    }

    text3->SetStringId(lbl_806DE8A8);
    text4->SetStringId(lbl_806DE8A8);
    text5->SetStringId(lbl_806DE8A8);

    scene->mUnidentified1E4 = value;
    scene->mUnidentified02C->m_bVisible = scene->mUnidentified1E4 & 1;
    scene->mUnidentified030->m_bVisible = (scene->mUnidentified1E4 >> 1) & 1;
    scene->mUnidentified034->m_bVisible = (scene->mUnidentified1E4 >> 2) & 1;
    scene->mUnidentified038->m_bVisible = (scene->mUnidentified1E4 >> 3) & 1;
    scene->mUnidentified03C->m_bVisible = (scene->mUnidentified1E4 >> 4) & 1;
    scene->mUnidentified040->m_bVisible = (scene->mUnidentified1E4 >> 5) & 1;
    scene->mUnidentified044->m_bVisible = (scene->mUnidentified1E4 >> 6) & 1;
    scene->mUnidentified048->m_bVisible = (scene->mUnidentified1E4 >> 7) & 1;
    fn_802547E8(scene, 0);

    component = FEFinder<TLComponentInstance, 4>::Find(
        scene->mUnidentified034->GetActiveSlide(), InlineHasher("back"));
    if (component == 0)
    {
        component = &lbl_80580030;
    }

    text0 = FEFinder<TLTextInstance, 4>::Find(component,
        nlStringLowerHash(lbl_806DE884), nlStringLowerHash(lbl_8051F810), 0, 0, 0, 0);
    if (text0 == 0)
    {
        text0 = &UnidentifiedFallbackTextInstance;
    }

    text1 = FEFinder<TLTextInstance, 4>::Find(component,
        nlStringLowerHash(lbl_806DE888), nlStringLowerHash(lbl_8051F810), 0, 0, 0, 0);
    if (text1 == 0)
    {
        text1 = &UnidentifiedFallbackTextInstance;
    }

    text2 = FEFinder<TLTextInstance, 4>::Find(component,
        nlStringLowerHash(lbl_806DE890), nlStringLowerHash(lbl_8051F810), 0, 0, 0, 0);
    if (text2 == 0)
    {
        text2 = &UnidentifiedFallbackTextInstance;
    }

    text0->SetStringId(lbl_806DE898);
    text1->SetStringId(lbl_806DE898);
    text2->SetStringId(lbl_806DE898);

    text3 = FEFinder<TLTextInstance, 4>::Find(scene->mUnidentified040,
        nlStringLowerHash(lbl_806DE884), nlStringLowerHash(lbl_806DE8A0),
        nlStringLowerHash("done"), 0, 0, 0);
    if (text3 == 0)
    {
        text3 = &UnidentifiedFallbackTextInstance;
    }

    text4 = FEFinder<TLTextInstance, 4>::Find(scene->mUnidentified040,
        nlStringLowerHash(lbl_806DE888), nlStringLowerHash(lbl_806DE8A0),
        nlStringLowerHash("done"), 0, 0, 0);
    if (text4 == 0)
    {
        text4 = &UnidentifiedFallbackTextInstance;
    }

    text5 = FEFinder<TLTextInstance, 4>::Find(scene->mUnidentified040,
        nlStringLowerHash(lbl_806DE890), nlStringLowerHash(lbl_806DE8A0),
        nlStringLowerHash("done"), 0, 0, 0);
    if (text5 == 0)
    {
        text5 = &UnidentifiedFallbackTextInstance;
    }

    text3->SetStringId(lbl_806DE8A8);
    text4->SetStringId(lbl_806DE8A8);
    text5->SetStringId(lbl_806DE8A8);
}

extern "C" TLComponentInstance* fn_80253D70(TU80252180Scene* scene, int value)
{
    TLComponentInstance* component = 0;
    switch (value)
    {
    case 1:
        component = scene->mUnidentified02C;
        break;
    case 2:
        component = scene->mUnidentified030;
        break;
    case 4:
        component = scene->mUnidentified034;
        break;
    case 8:
        component = scene->mUnidentified038;
        break;
    case 0x10:
        component = scene->mUnidentified03C;
        break;
    case 0x20:
        component = scene->mUnidentified040;
        break;
    case 0x40:
        component = scene->mUnidentified044;
        break;
    case 0x80:
        component = scene->mUnidentified048;
        break;
    }
    return component;
}

extern "C" TU80252180Scene* fn_80253E18()
{
    return (TU80252180Scene*)FESceneManager::Instance()->m_topMostScene;
}

extern "C" void fn_80253E24(TU80252180Scene* scene)
{
    if (scene->mFEScene != 0 && scene->mFEScene->mState == 6 &&
        !scene->mUnidentified1E7)
    {
        if (!lbl_806DE828)
        {
            if ((nlTaskManager::m_pInstance->mCurrentState & 4) == 0)
            {
                scene->mUnidentified02C->m_bVisible = false;
                scene->mUnidentified030->m_bVisible = false;
                scene->mUnidentified034->m_bVisible = false;
                scene->mUnidentified038->m_bVisible = false;
                scene->mUnidentified03C->m_bVisible = false;
                scene->mUnidentified040->m_bVisible = false;
                scene->mUnidentified044->m_bVisible = false;
                scene->mUnidentified048->m_bVisible = false;
            }
            lbl_806DE828 = true;
            lbl_806E18F0 = true;
        }

        scene->mUnidentified050->m_bVisible = true;
        if (scene->mUnidentified1E8)
        {
            scene->mUnidentified050->SetActiveSlide(lbl_8051F804, true, false);
        }
        else
        {
            scene->mUnidentified050->SetActiveSlide(lbl_806DE8C0, true, false);
        }
        scene->mUnidentified1E7 = true;
    }
}

extern "C" void fn_80253F30(TU80252180Scene*)
{
    nlColour teamColours[2];

    int team0 = GameInfoManager::Instance()->GetTeam(0);
    int team1 = GameInfoManager::Instance()->GetTeam(1);
    const CharacterInfo& info0 = GetCharacterInfo(GetCharacterIndexFromCaptain(team0));
    const CharacterInfo& info1 = GetCharacterInfo(GetCharacterIndexFromCaptain(team1));

    unsigned long packedColour = GetTeamColour(info0, info1, true);
    nlColour colour0;
    colour0.c[0] = packedColour >> 24;
    colour0.c[1] = packedColour >> 16;
    colour0.c[2] = packedColour >> 8;
    colour0.c[3] = packedColour;
    teamColours[0] = colour0;

    packedColour = GetTeamColour(info1, info0, true);
    nlColour colour1;
    colour1.c[0] = packedColour >> 24;
    colour1.c[1] = packedColour >> 16;
    colour1.c[2] = packedColour >> 8;
    colour1.c[3] = packedColour;
    teamColours[1] = colour1;

    for (int index = 0; index < 4; ++index)
    {
        int side = GameInfoManager::Instance()->GetPlayingSide(index);
        if (side == 0 || side == 1)
        {
            fn_80219E08(index, teamColours[side]);
        }
    }
}
