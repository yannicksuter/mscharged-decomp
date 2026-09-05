#include "unclassified/tu_80254084.h"

#include "Game/BaseGameSceneManager.h"
#include "Game/DB/tu_8010A40C.h"
#include "Game/FE/feFinder.h"
#include "Game/FE/feInput.h"
#include "Game/FE/fePackage.h"
#include "Game/FE/feScene.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlTextInstance.h"
#include "Game/GameInfo.h"
#include "Game/NetworkDraft.h"
#include "Game/NetworkSession.h"
#include "Game/SH/SHSceneBase.h"
#include "Game/TweakValue.h"
#include "Game/UnidentifiedStaticStorage.h"
#include "Game/tu_801360A4.h"
#include "NL/nlAlgorithm.h"
#include "NL/nlBind.h"
#include "NL/nlFormat.h"
#include "NL/nlLocalization.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"
#include "unclassified/tu_802196B0.h"
#include "unclassified/tu_8022EF84.h"
#include "unclassified/tu_802492E4.h"
#include "unclassified/tu_80252180.h"

struct TU80257D0CState
{
    void* mUnidentified00;
    int mUnidentified04;
    int mUnidentified08;
    int mUnidentified0C;
};

extern TLComponentInstance* lbl_80578450[4];
extern TU80257D0CState lbl_8057848C;
extern TLComponentInstance lbl_80580030;
extern TLSlide lbl_80580358;
extern char lbl_8051F810[];
extern char lbl_8051F81C[];
extern char lbl_8051F828[];
extern char lbl_8051F834[];
extern char lbl_8051F840[];
extern char lbl_8051F850[];
extern char lbl_806DE840[5];
extern char lbl_806DE848[5];
extern char lbl_806DE884[4];
extern char lbl_806DE888[5];
extern char lbl_806DE890[4];
extern char lbl_806DE898[4];
extern char lbl_806DE8A0[4];
extern char lbl_806DE8A8[4];
extern char lbl_806DE8B0[4];
extern char lbl_806DE8B8[4];
extern char lbl_806DE8C0[7];
extern char lbl_806DE8C8[7];
extern char lbl_806DE8D0[7];
extern char lbl_806DE8D8[7];
extern char lbl_806DE8E0[7];
extern char lbl_806DE8E8[7];
extern char lbl_806DE8F0[7];
extern char lbl_806DE8F8[7];
extern char lbl_806DE900[7];
extern char lbl_8051FB78[];
extern char lbl_8051FB84[9];
extern char lbl_8051FB90[9];
extern char lbl_8051FB9C[23];
extern char lbl_8051FBB4[34];
extern char lbl_8051FBD8[33];
extern char lbl_8051FBFC[19];
extern char lbl_8051FC10[12];
extern char lbl_8051FC1C[15];
extern char lbl_8051FC2C[9];
extern char lbl_8051FC38[10];
extern char lbl_8051FC44[10];
extern char lbl_8051FC50[18];
extern char lbl_8051FC64[19];
extern char lbl_8051FC78[14];
extern char lbl_806DE910[8];
extern char lbl_806DE918[6];
extern char lbl_806DE920[8];
extern char lbl_806DE928[2];
extern char lbl_806DE92C[7];
extern char lbl_806DE934[4];
extern char lbl_806DE938[4];
extern char lbl_806DE940[8];
extern char lbl_806DE948[7];
extern char lbl_806DE950[8];
extern char lbl_806DE958[7];
extern char lbl_806DE960[7];
extern char lbl_806DE968[7];
extern char lbl_806DE978[8];
extern char lbl_806DE980[6];
extern char lbl_806DE988[6];
extern char lbl_806DE990[7];
extern char lbl_806DE998[2];
extern char lbl_806DE99C[7];
extern char lbl_806DE9A4[4];
extern unsigned int lbl_806E18B0;
extern unsigned char lbl_806E18C8;
extern int lbl_806E18CC;
extern int lbl_806E18D0;
extern unsigned char lbl_806E18D4;
extern unsigned char lbl_806E18D5;
extern unsigned char lbl_806E18D6;
extern BaseGameSceneManager* lbl_806E1838;
extern int lbl_806DE668[2];
static int s_nTournamentLowBound = 3;
static int s_nTournamentHiBound = 6;

extern "C" void* memcpy(void* dest, const void* src, unsigned long count);

typedef BasicString<unsigned short, Detail::TempStringAllocator> WideBasicString;

struct TU802555F8UnlockEntry
{
    const char* mStringId;
    unsigned int mUnlockFlag;
};

struct TU802555F8FunctionEntry
{
    const char* mStringId;
    bool (*mIsUnlocked)();
};

struct TU802555F8ChallengeEntry
{
    const char* mStringId;
    int mChallenge;
};

extern TU802555F8UnlockEntry lbl_8051F958[];
extern TU802555F8FunctionEntry lbl_8051FA68[];
extern TU802555F8ChallengeEntry lbl_8051FB18[];

extern bool fn_8010FE54(unsigned int flag);
extern "C" void fn_801CBCA0(unsigned long hash, int value0, int value1, int value2);
extern "C" void fn_8025BD7C(bool value);
extern "C" void fn_8025BD94(bool value);
extern "C" void fn_8022F858(UnidentifiedScrollWidget* widget);
extern "C" bool fn_8022FD80(UnidentifiedScrollWidget* widget, int direction, int value);
extern "C" void fn_80230468(UnidentifiedScrollWidget* widget, TU80300104Event event, float fDeltaT);
extern "C" void fn_802308D0(UnidentifiedScrollWidget* widget, TLInstance* instance);
extern "C" void fn_80230B90(UnidentifiedScrollWidget* widget, int mode);
extern "C" void fn_80230DE0(UnidentifiedScrollWidget* widget, int value);
extern "C" void fn_80238A1C(TU802384AC* object, int index, bool enabled, bool visible);
extern "C" void fn_80255FE8(TU802554B4Scene* scene);
extern "C" void fn_80256664(TU802554B4Scene* scene, int index, const char* stringId, bool unlocked);

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
            hash, localization->m_LookupTable,
            (int)localization->m_pFile->StringCount);
    if (lookup != 0)
    {
        return localization->m_FirstString + lookup->StringOffset;
    }

    return MissingLocString;
}

extern "C" void fn_80254084(int index, TLComponentInstance* component)
{
    lbl_80578450[index] = component;
}

extern "C" TLComponentInstance* fn_80254098(TLSlide* slide,
    const InlineHasher& level1, InlineHasher level2, InlineHasher level3,
    InlineHasher level4, InlineHasher level5, InlineHasher level6)
{
    TLComponentInstance* result = (TLComponentInstance*)FEFinder<TLComponentInstance, 4>::_Find(
        slide, level1.m_Hash, level2.m_Hash, level3.m_Hash, level4.m_Hash,
        level5.m_Hash, level6.m_Hash);
    if (result == 0)
    {
        result = &lbl_80580030;
    }
    return result;
}

extern "C" TLComponentInstance* fn_80254170(TLSlide* slide,
    const InlineHasher& level1, InlineHasher level2, InlineHasher level3,
    InlineHasher level4, InlineHasher level5, InlineHasher level6)
{
    TLComponentInstance* result = (TLComponentInstance*)FEFinder<TLComponentInstance, 4>::_Find(
        slide, level1.m_Hash, level2.m_Hash, level3.m_Hash, level4.m_Hash,
        level5.m_Hash, level6.m_Hash);
    if (result == 0)
    {
        return 0;
    }
    return result;
}

extern "C" TLSlide* fn_80254238(TLSlide* slide,
    const InlineHasher& level1, InlineHasher level2, InlineHasher level3,
    InlineHasher level4, InlineHasher level5, InlineHasher level6)
{
    TLSlide* result = (TLSlide*)FEFinder<TLSlide, 4>::_Find(
        slide, level1.m_Hash, level2.m_Hash, level3.m_Hash, level4.m_Hash,
        level5.m_Hash, level6.m_Hash);
    if (result == 0)
    {
        result = &lbl_80580358;
    }
    return result;
}

extern "C" void fn_80254310(TU80252180Scene* scene, bool enabled)
{
    scene->mUnidentified1E4 = 0;

    TLComponentInstance* component = (TLComponentInstance*)FEFinder<TLComponentInstance, 4>::_Find(
        scene->mUnidentified034->GetActiveSlide(), nlStringLowerHash(lbl_806DE840),
        0, 0, 0, 0, 0);
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

    component = (TLComponentInstance*)FEFinder<TLComponentInstance, 4>::_Find(
        scene->mUnidentified034->GetActiveSlide(), nlStringLowerHash(lbl_806DE840),
        0, 0, 0, 0, 0);
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
        nlStringLowerHash(lbl_806DE848), 0, 0, 0);
    if (text3 == 0)
    {
        text3 = &UnidentifiedFallbackTextInstance;
    }

    TLTextInstance* text4 = FEFinder<TLTextInstance, 4>::Find(scene->mUnidentified040,
        nlStringLowerHash(lbl_806DE888), nlStringLowerHash(lbl_806DE8A0),
        nlStringLowerHash(lbl_806DE848), 0, 0, 0);
    if (text4 == 0)
    {
        text4 = &UnidentifiedFallbackTextInstance;
    }

    TLTextInstance* text5 = FEFinder<TLTextInstance, 4>::Find(scene->mUnidentified040,
        nlStringLowerHash(lbl_806DE890), nlStringLowerHash(lbl_806DE8A0),
        nlStringLowerHash(lbl_806DE848), 0, 0, 0);
    if (text5 == 0)
    {
        text5 = &UnidentifiedFallbackTextInstance;
    }

    text3->SetStringId(lbl_806DE8A8);
    text4->SetStringId(lbl_806DE8A8);
    text5->SetStringId(lbl_806DE8A8);
}

extern "C" void fn_802547E8(TU80252180Scene* scene, int value)
{
    TLTextInstance* text0;
    TLTextInstance* text1;
    TLTextInstance* text2;
    TLTextInstance* result = FEFinder<TLTextInstance, 4>::Find(scene->mUnidentified03C,
        nlStringLowerHash(lbl_806DE884), nlStringLowerHash(lbl_806DE8A0),
        nlStringLowerHash(lbl_806DE8B0), 0, 0, 0);
    text0 = result == 0 ? &UnidentifiedFallbackTextInstance : result;

    result = FEFinder<TLTextInstance, 4>::Find(scene->mUnidentified03C,
        nlStringLowerHash(lbl_806DE888), nlStringLowerHash(lbl_806DE8A0),
        nlStringLowerHash(lbl_806DE8B0), 0, 0, 0);
    text1 = result == 0 ? &UnidentifiedFallbackTextInstance : result;

    result = FEFinder<TLTextInstance, 4>::Find(scene->mUnidentified03C,
        nlStringLowerHash(lbl_806DE890), nlStringLowerHash(lbl_806DE8A0),
        nlStringLowerHash(lbl_806DE8B0), 0, 0, 0);
    text2 = result == 0 ? &UnidentifiedFallbackTextInstance : result;

    switch (value)
    {
    case 0:
        text0->SetStringId(lbl_8051F81C);
        text1->SetStringId(lbl_8051F81C);
        text2->SetStringId(lbl_8051F81C);
        break;
    case 1:
        text0->SetStringId(lbl_8051F828);
        text1->SetStringId(lbl_8051F828);
        text2->SetStringId(lbl_8051F828);
        break;
    case 2:
        text0->SetStringId(lbl_8051F834);
        text1->SetStringId(lbl_8051F834);
        text2->SetStringId(lbl_8051F834);
        break;
    case 3:
        text0->SetStringId(lbl_8051F840);
        text1->SetStringId(lbl_8051F840);
        text2->SetStringId(lbl_8051F840);
        break;
    }
}

extern "C" void fn_80254A14(TU80252180Scene* scene, int value)
{
    TLTextInstance* text0;
    TLTextInstance* text1;
    TLTextInstance* text2;
    TLTextInstance* result = FEFinder<TLTextInstance, 4>::Find(scene->mUnidentified040,
        nlStringLowerHash(lbl_806DE884), nlStringLowerHash(lbl_806DE8A0),
        nlStringLowerHash(lbl_806DE848), 0, 0, 0);
    text0 = result == 0 ? &UnidentifiedFallbackTextInstance : result;

    result = FEFinder<TLTextInstance, 4>::Find(scene->mUnidentified040,
        nlStringLowerHash(lbl_806DE888), nlStringLowerHash(lbl_806DE8A0),
        nlStringLowerHash(lbl_806DE848), 0, 0, 0);
    text1 = result == 0 ? &UnidentifiedFallbackTextInstance : result;

    result = FEFinder<TLTextInstance, 4>::Find(scene->mUnidentified040,
        nlStringLowerHash(lbl_806DE890), nlStringLowerHash(lbl_806DE8A0),
        nlStringLowerHash(lbl_806DE848), 0, 0, 0);
    text2 = result == 0 ? &UnidentifiedFallbackTextInstance : result;

    switch (value)
    {
    case 0:
        text0->SetStringId(lbl_806DE8A8);
        text1->SetStringId(lbl_806DE8A8);
        text2->SetStringId(lbl_806DE8A8);
        break;
    case 1:
        text0->SetStringId(lbl_8051F850);
        text1->SetStringId(lbl_8051F850);
        text2->SetStringId(lbl_8051F850);
        break;
    }
}

extern "C" void fn_80254BD4(TU80252180Scene* scene, int value)
{
    TLComponentInstance* component = (TLComponentInstance*)FEFinder<TLComponentInstance, 4>::_Find(
        scene->mUnidentified034->GetActiveSlide(), nlStringLowerHash(lbl_806DE840),
        0, 0, 0, 0, 0);
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

    switch (value)
    {
    case 0:
        text0->SetStringId(lbl_806DE898);
        text1->SetStringId(lbl_806DE898);
        text2->SetStringId(lbl_806DE898);
        break;
    case 1:
        text0->SetStringId(lbl_806DE8B8);
        text1->SetStringId(lbl_806DE8B8);
        text2->SetStringId(lbl_806DE8B8);
        break;
    }
}

extern "C" void fn_80254E3C(TU80252180Scene* scene)
{
    scene->mUnidentified04C->SetActiveSlide(lbl_806DE8C0, true, false);

    TLComponentInstance* component = (TLComponentInstance*)FEFinder<TLComponentInstance, 4>::_Find(
        scene->mUnidentified04C->GetActiveSlide(), nlStringLowerHash(lbl_806DE8A0),
        nlStringLowerHash(lbl_806DE8C8), 0, 0, 0, 0);
    component->SetActiveSlide(lbl_806DE8C0, true, false);

    component = (TLComponentInstance*)FEFinder<TLComponentInstance, 4>::_Find(
        scene->mUnidentified04C->GetActiveSlide(), nlStringLowerHash(lbl_806DE8A0),
        nlStringLowerHash(lbl_806DE8D0), 0, 0, 0, 0);
    component->SetActiveSlide(lbl_806DE8C0, true, false);

    component = (TLComponentInstance*)FEFinder<TLComponentInstance, 4>::_Find(
        scene->mUnidentified04C->GetActiveSlide(), nlStringLowerHash(lbl_806DE8A0),
        nlStringLowerHash(lbl_806DE8D8), 0, 0, 0, 0);
    component->SetActiveSlide(lbl_806DE8C0, true, false);

    component = (TLComponentInstance*)FEFinder<TLComponentInstance, 4>::_Find(
        scene->mUnidentified04C->GetActiveSlide(), nlStringLowerHash(lbl_806DE8A0),
        nlStringLowerHash(lbl_806DE8E0), 0, 0, 0, 0);
    component->SetActiveSlide(lbl_806DE8C0, true, false);

    component = (TLComponentInstance*)FEFinder<TLComponentInstance, 4>::_Find(
        scene->mUnidentified04C->GetActiveSlide(), nlStringLowerHash(lbl_806DE8A0),
        nlStringLowerHash(lbl_806DE8E8), 0, 0, 0, 0);
    component->SetActiveSlide(lbl_806DE8C0, true, false);

    component = (TLComponentInstance*)FEFinder<TLComponentInstance, 4>::_Find(
        scene->mUnidentified04C->GetActiveSlide(), nlStringLowerHash(lbl_806DE8A0),
        nlStringLowerHash(lbl_806DE8F0), 0, 0, 0, 0);
    component->SetActiveSlide(lbl_806DE8C0, true, false);

    component = (TLComponentInstance*)FEFinder<TLComponentInstance, 4>::_Find(
        scene->mUnidentified04C->GetActiveSlide(), nlStringLowerHash(lbl_806DE8A0),
        nlStringLowerHash(lbl_806DE8F8), 0, 0, 0, 0);
    component->SetActiveSlide(lbl_806DE8C0, true, false);

    component = (TLComponentInstance*)FEFinder<TLComponentInstance, 4>::_Find(
        scene->mUnidentified04C->GetActiveSlide(), nlStringLowerHash(lbl_806DE8A0),
        nlStringLowerHash(lbl_806DE900), 0, 0, 0, 0);
    component->SetActiveSlide(lbl_806DE8C0, true, false);

    scene->mUnidentified1E5 = true;
    scene->mUnidentified1E6 = true;
}

TU802554B4Scene::TU802554B4Scene(int mode)
    : mMode(mode)
    , mNavigation()
    , mScrollWidget()
{
    mUnidentified6C4 = 0;
    mUnidentified6C8 = false;
    mUnidentified6C9 = false;
    mUnidentified6CA = false;
    mUnidentified6CC = 0;
    mUnidentified6B0[0] = 0;
    mUnidentified6B0[1] = 0;
    mUnidentified6B0[2] = 0;
    mUnidentified6B0[3] = 0;

    switch (mMode)
    {
    case 14:
        mUnidentified6C0 = 9;
        break;
    case 15:
        mUnidentified6C0 = 9;
        break;
    case 16:
        mUnidentified6C0 = 12;
        break;
    }
}

TU802554B4Scene::~TU802554B4Scene()
{
}

void TU802554B4Scene::SceneCreated()
{
    TU80252180Scene* scene = fn_80253E18();
    TLComponentInstance* screen = 0;
    TLComponentInstance* breadcrumbs = 0;
    if (scene != 0)
    {
        fn_802534BC(scene, 15, false);
        screen = fn_80253D70(scene, 4);
        breadcrumbs = fn_80253D70(scene, 8);
        mUnidentified0F8 = &scene->mUnidentified058;
        fn_80238A1C(mUnidentified0F8, 1, true, true);
        fn_80238A1C(mUnidentified0F8, 0, true, true);
    }

    mNavigation.fn_8022F194(screen);
    fn_802492E4(mMode, breadcrumbs);

    FEPresentation* presentation = mFEScene->m_pFEPackage->GetPresentation();
    TLComponentInstance* scrollbar = CastFound<TLComponentInstance>(
        FEFinder<TLComponentInstance, 4>::_Find<TLSlide>(presentation->GetActiveSlide(),
            nlStringLowerHash(lbl_806DE918), nlStringLowerHash(lbl_806DE910),
            nlStringLowerHash(lbl_8051FB78), 0, 0, 0));
    fn_802308D0(&mScrollWidget, scrollbar);
    fn_80230B90(&mScrollWidget, mUnidentified6C0 - 7);
    fn_80230DE0(&mScrollWidget, mUnidentified6C4);

    for (int i = 0; i < 4; i++)
    {
        lbl_80578450[i]->SetActiveSlide(lbl_806DE920, true, false);
    }

    fn_80255FE8(this);

    switch (mMode)
    {
    case 14:
        for (int i = 0; i < 7; i++)
        {
            fn_80256664(this, i, lbl_8051F958[i + mUnidentified6C4].mStringId,
                fn_8010FE54(lbl_8051F958[i + mUnidentified6C4].mUnlockFlag));
        }
        break;
    case 15:
        for (int i = 0; i < 7; i++)
        {
            fn_80256664(this, i, lbl_8051FA68[i + mUnidentified6C4].mStringId,
                lbl_8051FA68[i + mUnidentified6C4].mIsUnlocked());
        }
        break;
    case 16:
        for (int i = 0; i < 7; i++)
        {
            fn_80256664(this, i, lbl_8051FB18[i + mUnidentified6C4].mStringId,
                lbl_806E0FA0->IsUnlocked(lbl_8051FB18[i + mUnidentified6C4].mChallenge));
        }
        break;
    }
}

void TU802554B4Scene::Update(float fDeltaT)
{
    BaseSceneHandler::Update(fDeltaT);

    int state = mUnidentified6CC;
    if (state == 0 || (unsigned int)(state - 2) <= 1)
    {
        TLSlide* slide = mPresentation->GetActiveSlide();
        if (slide->GetCurrentTime() < slide->m_duration + slide->m_start)
        {
            for (int pad = 0; pad < 4; pad++)
            {
                lbl_80578450[pad]->SetActiveSlide(lbl_806DE920, true, false);
            }
            return;
        }

        if (state == 0)
        {
            mUnidentified6CC = 1;
        }
        else if (state == 2)
        {
            if (mUnidentified6CA)
            {
                fn_80249A94(mMode, false);
                return;
            }
            if (mUnidentified6C9)
            {
                fn_80249A94(mMode, true);
            }
            return;
        }
        else if (state == 3)
        {
            fn_80249BF4(mMode);
            return;
        }
    }

    if (!mUnidentified6C8)
    {
        mUnidentified6C8 = true;
    }

    if (!mScrollWidget.mUnidentified00[0x18])
    {
        fn_8022F858(&mScrollWidget);
    }

    for (unsigned int pad = 0; pad < 4; pad++)
    {
        TLComponentInstance* controller = lbl_80578450[pad];
        bool processInput = true;
        if (g_pFEInput->m_InputLockDepth == 0)
        {
            if (pad != lbl_806E18B0)
            {
                controller->SetActiveSlide(lbl_806DE920, true, false);
                processInput = false;
            }
            else if (mUnidentified6B0[pad] > 0)
            {
                controller->SetActiveSlide(lbl_806DE928, true, false);
            }
            else
            {
                controller->SetActiveSlide(lbl_806DE92C, true, false);
            }
        }

        if (processInput)
        {
            unsigned char valid = 1;
            TU80300104Event event;
            event.mIndex = pad;
            event.mPosition = fn_802197FC(pad, &valid);
            event.mFlag0
                = g_pFEInput->JustPressed((eFEINPUT_PAD)pad, 0x1E, true, 0);
            event.mFlag1
                = g_pFEInput->JustReleased((eFEINPUT_PAD)pad, 0x1E, true, 0);

            mUnidentified0F8->fn_802385D0(event, fDeltaT);
            fn_80230468(&mScrollWidget, event, fDeltaT);

            if (mNavigation.fn_8022F2E0(event, fDeltaT))
            {
                mUnidentified6CC = 3;
                TU80252180Scene* scene = fn_80253E18();
                if (scene != 0)
                {
                    fn_80253474(scene);
                }
                mPresentation->SetActiveSlide(lbl_806DE934, true);
                return;
            }

            if ((mUnidentified0F8->mUnidentified005[1]
                    || mUnidentified0F8->mUnidentified009[1])
                || (mUnidentified0F8->mUnidentified005[0]
                    || mUnidentified0F8->mUnidentified009[0]))
            {
                fn_801CBCA0(0x375D885A, 0, 0, 1);
                fn_801CBCA0(0x3050DD1E, 0, 0, 1);
                mUnidentified6CC = 2;
                mPresentation->SetActiveSlide(lbl_806DE938, true);

                if (mUnidentified0F8->mUnidentified005[1]
                    || mUnidentified0F8->mUnidentified009[1])
                {
                    mUnidentified6CA = true;
                }
                else if (mUnidentified0F8->mUnidentified005[0]
                    || mUnidentified0F8->mUnidentified009[0])
                {
                    mUnidentified6C9 = true;
                }
                return;
            }
        }
    }

    if (fn_8022FD80(&mScrollWidget, 1, 1))
    {
        ++mUnidentified6C4;
        switch (mMode)
        {
        case 14:
            for (int i = 0; i < 7; i++)
            {
                fn_80256664(this, i, lbl_8051F958[i + mUnidentified6C4].mStringId,
                    fn_8010FE54(lbl_8051F958[i + mUnidentified6C4].mUnlockFlag));
            }
            break;
        case 15:
            for (int i = 0; i < 7; i++)
            {
                fn_80256664(this, i, lbl_8051FA68[i + mUnidentified6C4].mStringId,
                    lbl_8051FA68[i + mUnidentified6C4].mIsUnlocked());
            }
            break;
        case 16:
            for (int i = 0; i < 7; i++)
            {
                fn_80256664(this, i, lbl_8051FB18[i + mUnidentified6C4].mStringId,
                    lbl_806E0FA0->IsUnlocked(
                        lbl_8051FB18[i + mUnidentified6C4].mChallenge));
            }
            break;
        }
    }
    else if (fn_8022FD80(&mScrollWidget, 0, 1))
    {
        --mUnidentified6C4;
        switch (mMode)
        {
        case 14:
            for (int i = 0; i < 7; i++)
            {
                fn_80256664(this, i, lbl_8051F958[i + mUnidentified6C4].mStringId,
                    fn_8010FE54(lbl_8051F958[i + mUnidentified6C4].mUnlockFlag));
            }
            break;
        case 15:
            for (int i = 0; i < 7; i++)
            {
                fn_80256664(this, i, lbl_8051FA68[i + mUnidentified6C4].mStringId,
                    lbl_8051FA68[i + mUnidentified6C4].mIsUnlocked());
            }
            break;
        case 16:
            for (int i = 0; i < 7; i++)
            {
                fn_80256664(this, i, lbl_8051FB18[i + mUnidentified6C4].mStringId,
                    lbl_806E0FA0->IsUnlocked(
                        lbl_8051FB18[i + mUnidentified6C4].mChallenge));
            }
            break;
        }
    }
}

extern "C" void fn_80255FE8(TU802554B4Scene* scene)
{
    WideBasicString title;

    FEPresentation* presentation = scene->mFEScene->m_pFEPackage->GetPresentation();
    unsigned long subtitleComponentHash = nlStringLowerHash(lbl_8051FB84);
    unsigned long subtitleHash = nlStringLowerHash(lbl_8051FB90);
    unsigned long summaryHash = nlStringLowerHash(lbl_806DE910);
    TLTextInstance* titleText = CastFound<TLTextInstance>(
        FEFinder<TLTextInstance, 4>::_Find<TLSlide>(presentation->GetActiveSlide(),
            nlStringLowerHash(lbl_806DE918), summaryHash, subtitleHash,
            subtitleComponentHash, 0, 0));
    if (titleText == 0)
    {
        titleText = &UnidentifiedFallbackTextInstance;
    }

    switch (scene->mMode)
    {
    case 14:
        title = WideBasicString(LookupLocString(lbl_8051FB9C));
        break;
    case 15:
        title = WideBasicString(LookupLocString(lbl_8051FBB4));
        break;
    case 16:
        title = WideBasicString(LookupLocString(lbl_8051FBD8));
        break;
    }

    memcpy(scene->mUnidentified2B0, title.c_str(), sizeof(scene->mUnidentified2B0));
    titleText->SetString(scene->mUnidentified2B0);
}

extern "C" void fn_80256664(
    TU802554B4Scene* scene, int index, const char* stringId, bool unlocked)
{
    WideBasicString itemText;
    if (scene->mMode == 16)
    {
        WideBasicString itemName(LookupLocString(stringId));
        WideBasicString format(LookupLocString(lbl_8051FBFC));
        itemText = Format(format, itemName);
    }
    else
    {
        itemText = WideBasicString(LookupLocString(stringId));
    }

    char itemComponentName[8];
    nlSNPrintf(itemComponentName, sizeof(itemComponentName), lbl_806DE940, index);

    FEPresentation* presentation = scene->mFEScene->m_pFEPackage->GetPresentation();
    unsigned long stat0Hash = nlStringLowerHash(lbl_806DE948);
    unsigned long challenge0Hash = nlStringLowerHash(lbl_8051FC10);
    unsigned long itemHash = nlStringLowerHash(itemComponentName);
    unsigned long summaryHash = nlStringLowerHash(lbl_806DE910);
    TLTextInstance* rowText = CastFound<TLTextInstance>(
        FEFinder<TLTextInstance, 4>::_Find<TLSlide>(presentation->GetActiveSlide(),
            nlStringLowerHash(lbl_806DE918), summaryHash, itemHash, challenge0Hash,
            stat0Hash, 0));
    if (rowText == 0)
    {
        rowText = &UnidentifiedFallbackTextInstance;
    }

    unsigned long lockedUnlockedHash = nlStringLowerHash(lbl_8051FC1C);
    challenge0Hash = nlStringLowerHash(lbl_8051FC10);
    itemHash = nlStringLowerHash(itemComponentName);
    summaryHash = nlStringLowerHash(lbl_806DE910);
    TLComponentInstance* lockState = CastFound<TLComponentInstance>(
        FEFinder<TLComponentInstance, 4>::_Find<TLSlide>(presentation->GetActiveSlide(),
            nlStringLowerHash(lbl_806DE918), summaryHash, itemHash, challenge0Hash,
            lockedUnlockedHash, 0));
    if (lockState == 0)
    {
        lockState = &lbl_80580030;
    }

    if (unlocked)
    {
        if (scene->mMode == 14)
        {
            lockState->SetActiveSlide(lbl_806DE950, true, false);
        }
        else if (scene->mMode == 15)
        {
            lockState->SetActiveSlide(lbl_8051FC2C, true, false);
        }
        else
        {
            lockState->SetActiveSlide(lbl_8051FC38, true, false);
        }
    }
    else
    {
        if (scene->mMode == 14)
        {
            lockState->SetActiveSlide(lbl_8051FC44, true, false);
        }
        else if (scene->mMode == 15)
        {
            lockState->SetActiveSlide(lbl_806DE958, true, false);
        }
        else
        {
            lockState->SetActiveSlide(lbl_8051FC38, true, false);
        }
    }

    if (scene->mMode == 16)
    {
        unsigned long groupHash = nlStringLowerHash(lbl_806DE960);
        lockedUnlockedHash = nlStringLowerHash(lbl_8051FC1C);
        challenge0Hash = nlStringLowerHash(lbl_8051FC10);
        itemHash = nlStringLowerHash(itemComponentName);
        summaryHash = nlStringLowerHash(lbl_806DE910);
        TLTextInstance* statusText = CastFound<TLTextInstance>(
            FEFinder<TLTextInstance, 4>::_Find<TLSlide>(presentation->GetActiveSlide(),
                nlStringLowerHash(lbl_806DE918), summaryHash, itemHash,
                challenge0Hash, lockedUnlockedHash, groupHash));
        if (statusText == 0)
        {
            statusText = &UnidentifiedFallbackTextInstance;
        }

        if (unlocked)
        {
            statusText->SetStringId(lbl_8051FC50);
        }
        else
        {
            statusText->SetStringId(lbl_8051FC64);
        }
    }

    memcpy(scene->mUnidentified330[index], itemText.c_str(),
        sizeof(scene->mUnidentified330[index]));
    rowText->SetString(scene->mUnidentified330[index]);

    char tournamentName[16];
    nlSNPrintf(tournamentName, sizeof(tournamentName), lbl_8051FC78,
        index + scene->mUnidentified6C4 + 1);

    unsigned long numberHash = nlStringLowerHash(lbl_806DE968);
    challenge0Hash = nlStringLowerHash(lbl_8051FC10);
    itemHash = nlStringLowerHash(itemComponentName);
    summaryHash = nlStringLowerHash(lbl_806DE910);
    TLTextInstance* tournamentText = CastFound<TLTextInstance>(
        FEFinder<TLTextInstance, 4>::_Find<TLSlide>(presentation->GetActiveSlide(),
            nlStringLowerHash(lbl_806DE918), summaryHash, itemHash, challenge0Hash,
            numberHash, 0));
    TLTextInstance* displayedTournamentText = tournamentText == 0
        ? &UnidentifiedFallbackTextInstance
        : tournamentText;
    displayedTournamentText->SetStringId(tournamentName);
}

TU80257150Scene::TU80257150Scene(int mode)
    : mNavigation()
    , mComponents()
    , mMode(mode)
    , mUnidentified278(false)
    , mUnidentified280(0)
    , mUnidentified284(-2)
{
    mNavigation.fn_801D2BE0(false);

    mComponentCount = 2;
    for (int i = 0; i < mComponentCount; ++i)
    {
        mComponents[i].mContext = (void*)i;
    }

    mUnidentified268[0] = 0;
    mUnidentified268[1] = 0;
    mUnidentified268[2] = 0;
    mUnidentified268[3] = 0;
}

TU80257150Scene::~TU80257150Scene()
{
}

void TU80257150Scene::SceneCreated()
{
    FEPresentation* presentation = mFEScene->m_pFEPackage->GetPresentation();

    for (int i = 0; i < 4; ++i)
    {
        lbl_80578450[i]->SetActiveSlide(lbl_806DE978, true, false);
    }

    if (mComponentCount > 0)
    {
        const char* groupName = lbl_806DE980;
        const char* layerName = lbl_806DE988;
        for (int i = 0; i < mComponentCount; ++i)
        {
            char name[6];
            nlSNPrintf(name, sizeof(name), lbl_806DE990, i);
            TLComponentInstance* button = CastFound<TLComponentInstance>(
                FEFinder<TLComponentInstance, 4>::_Find<TLSlide>(
                    presentation->GetActiveSlide(), nlStringLowerHash(layerName),
                    nlStringLowerHash(groupName), nlStringLowerHash(name), 0, 0, 0));
            if (button == 0)
            {
                button = &lbl_80580030;
            }
            mButtonInstances[i] = button;
        }
    }

    TU80252180Scene* scene = fn_80253E18();
    TLComponentInstance* screen = 0;
    if (scene != 0)
    {
        fn_80253474(scene);
        screen = fn_80253D70(scene, 4);
    }
    mNavigation.fn_8022F194(screen);

    fn_801CBCA0(0xBB142B94, 0, 0, 1);
}

void TU80257150Scene::Update(float fDeltaT)
{
    BaseSceneHandler::Update(fDeltaT);

    int state = mUnidentified280;
    if (state == 0 || (unsigned int)(state - 2) <= 1)
    {
        TLSlide* slide = mPresentation->GetActiveSlide();
        if (slide->GetCurrentTime() < slide->m_start + slide->m_duration)
        {
            for (int pad = 0; pad < 4; ++pad)
            {
                lbl_80578450[pad]->SetActiveSlide(lbl_806DE978, true, false);
            }
            return;
        }

        if (state == 0)
        {
            TU80252180Scene* scene = fn_80253E18();
            if (scene != 0)
            {
                fn_802534BC(scene, 4, true);
            }
            fn_80257850();
            mUnidentified278 = true;
            mUnidentified280 = 1;
        }
        else if (state == 3)
        {
            lbl_806E1838->Push((SceneList)0x28, SCREEN_NOTHING, true);
            fn_801CBCA0(0x37A9934D, 0, 0, 1);
            return;
        }
        else if (state == 2)
        {
            if (mUnidentified284 == 0x1B)
            {
                fn_801CBCA0(0xC385EFFB, 0, 0, 1);
            }
            lbl_806E1838->Push((SceneList)mUnidentified284, SCREEN_NOTHING, true);
            return;
        }
    }

    if (!lbl_806E1838->IsOnStack((SceneList)0xA)
        && lbl_806E1194->FindHostInvitation_80136AB0())
    {
        int invitationScene = 0x2A;
        if (mMode == 0)
        {
            invitationScene = 0x29;
        }
        UnidentifiedFriendManager_801360A4* friendManager = lbl_806E1194;
        friendManager->mUnidentified00C = invitationScene;
        friendManager->mUnidentified010 = 0;
        lbl_806E1838->Push((SceneList)0x34, SCREEN_FORWARD, true);
        return;
    }

    for (int pad = 0; pad < 4; ++pad)
    {
        TLComponentInstance* controller = lbl_80578450[pad];
        if (g_pFEInput->m_InputLockDepth == 0)
        {
            if ((unsigned int)pad != lbl_806E18B0)
            {
                controller->SetActiveSlide(lbl_806DE978, true, false);
                continue;
            }

            if (mUnidentified268[pad] > 0 || mNavigation.mUnidentifiedD2[pad])
            {
                controller->SetActiveSlide(lbl_806DE998, true, false);
            }
            else
            {
                controller->SetActiveSlide(lbl_806DE99C, true, false);
            }
        }

        unsigned char valid = 1;
        TU80300104Event event;
        event.mIndex = pad;
        event.mPosition = fn_802197FC(pad, &valid);
        event.mFlag0
            = g_pFEInput->JustPressed((eFEINPUT_PAD)pad, 0x1E, true, 0);
        event.mFlag1
            = g_pFEInput->JustReleased((eFEINPUT_PAD)pad, 0x1E, true, 0);

        for (int i = 0; i < mComponentCount; ++i)
        {
            mComponents[i].fn_80219608(&event);
        }

        if (mNavigation.fn_8022F2E0(event, fDeltaT))
        {
            mUnidentified280 = 3;
            TU80252180Scene* scene = fn_80253E18();
            if (scene != 0)
            {
                fn_80253474(scene);
            }
            mPresentation->SetActiveSlide(lbl_806DE9A4, true);
            mPresentation->Update(0.0f);
            return;
        }
    }
}

void TU80257150Scene::fn_80257850()
{
    TU80300104Base::Callback buttonOver(
        Bind<void>(MemFun(&TU80257150Scene::fn_80257B9C), this, Placeholder<0>(), Placeholder<1>()));
    TU80300104Base::Callback buttonOff(
        Bind<void>(MemFun(&TU80257150Scene::fn_80257C60), this, Placeholder<0>(), Placeholder<1>()));
    TU80300104Base::Callback buttonSelect(
        Bind<void>(MemFun(&TU80257150Scene::fn_80257EA4), this, Placeholder<0>(), Placeholder<1>()));

    for (int i = 0; i < mComponentCount; ++i)
    {
        mComponents[i].fn_80300D74(
            mButtonInstances[i], true, 0.0f, 0.0f, 1.0f, 0.5f);
        mComponents[i].fn_803007C0(buttonOver);
        mComponents[i].fn_80300864(buttonOff);
        mComponents[i].fn_803009AC(buttonSelect);
    }
}

void TU80257150Scene::fn_80257B9C(int index, void* context)
{
    unsigned int item = (unsigned int)context;
    unsigned int which = index;
    ++mUnidentified268[index];
    if (!mComponents[item].fn_802192FC(1, which))
    {
        mComponents[item].mValues[which] = 1;
        mButtonInstances[item]->SetActiveSlide("over", true, false);
        fn_801CBCA0(0xF6EB899E, 0, 0, 1);
    }
}

void TU80257150Scene::fn_80257C60(int index, void* context)
{
    unsigned int item = (unsigned int)context;
    unsigned int which = index;
    --mUnidentified268[index];
    if (!mComponents[item].fn_802192FC(1, which))
    {
        mComponents[item].mValues[which] = 0;
        mButtonInstances[item]->SetActiveSlide("off", true, false);
    }
}

extern "C" void fn_80257D0C(bool value0, unsigned char value1, bool value2)
{
    if (value1)
    {
        GameInfoManager::GetInstance()->SetMode(1, true);
    }
    else
    {
        GameInfoManager::GetInstance()->SetMode(0, true);
    }

    fn_8025BD94(value0);
    fn_8025BD7C(value2);
    lbl_806E10EC->fn_80123E44(value1);
    lbl_806E18D4 = value0;

    GameInfoManager::GetInstance()->SetTeam(0, 0);
    GameInfoManager::GetInstance()->SetTeam(1, 0);
    GameInfoManager::GetInstance()->ResetPlayingSides();
    NetworkDraft::Instance()->Reset(false);

    if (!value0)
    {
        lbl_806DE668[0] = lbl_806E18B0;
        lbl_806DE668[1] = -1;
    }
    else if (lbl_806E10EC->OnlineVirtual0C() == 1)
    {
        lbl_806DE668[0] = 0;
        lbl_806DE668[1] = 1;
    }

    if (value1)
    {
        lbl_8057848C.mUnidentified04 = 0;
        lbl_8057848C.mUnidentified08 = 0;
        lbl_806E18C8 = 1;
        lbl_806E18D5 = 0;
        lbl_806E18D6 = 0;
        lbl_806E18CC = s_nTournamentHiBound;
        lbl_806E18D0 = s_nTournamentLowBound;
    }
    else if (value2)
    {
        lbl_8057848C.mUnidentified04 = 0;
        lbl_8057848C.mUnidentified08 = 0;
        lbl_806E18C8 = 1;
        lbl_806E18D5 = 0;
        lbl_806E18D6 = 0;
        lbl_806E18CC = 2;
        lbl_806E18D0 = 0;
    }
    else
    {
        lbl_806E18C8 = 0;
        lbl_806E18D5 = 0;
        lbl_806E18D6 = 1;
        lbl_806E18CC = 0;
        lbl_806E18D0 = 0;
    }
}

void TU80257150Scene::fn_80257EA4(int, void* context)
{
    fn_801CBCA0(0xF0AFD586, 0, 0, 1);
    mUnidentified280 = 2;

    TU80252180Scene* scene = fn_80253E18();
    if (scene != 0)
    {
        fn_80253474(scene);
    }

    mPresentation->SetActiveSlide(lbl_806DE9A4, true);
    mPresentation->Update(0.0f);

    int item = (int)context;
    switch (item)
    {
    case 0:
        fn_80257D0C(false, false, mMode == 0);
        if (lbl_806E10EC->OnlineVirtual0C() == 2)
        {
            if (mMode == 0)
            {
                mUnidentified284 = 0x31;
            }
            else
            {
                mUnidentified284 = 0x1B;
            }
        }
        else
        {
            lbl_806E1838->Push((SceneList)0x18, SCREEN_FORWARD, true);
        }
        break;
    case 1:
        fn_80257D0C(true, false, mMode == 0);
        if (lbl_806E10EC->OnlineVirtual0C() == 2)
        {
            mUnidentified284 = 0x2B;
        }
        else
        {
            lbl_806E1838->Push((SceneList)0x18, SCREEN_FORWARD, true);
        }
        break;
    default:
        break;
    }
}

static TweakValueIntImpl_804FD898 lbl_80578618(
    "s_nTournamentLowBound", "Network/DWCLobby", &s_nTournamentLowBound, true);
static TweakValueIntImpl_804FD898 lbl_80578638(
    "s_nTournamentHiBound", "Network/DWCLobby", &s_nTournamentHiBound, true);

template struct UnidentifiedStaticStorage<UnidentifiedStaticTag>;
