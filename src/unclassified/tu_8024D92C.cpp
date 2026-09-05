#include "unclassified/tu_8024D92C.h"

#include "Game/FE/feFinder.h"
#include "Game/FE/feInput.h"
#include "Game/FE/fePackage.h"
#include "Game/FE/fePresentation.h"
#include "Game/FE/feScene.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlImageInstance.h"
#include "Game/FE/tlSlide.h"
#include "Game/FE/tlTextInstance.h"
#include "NL/nlBasicString.h"
#include "NL/nlLocalization.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"
#include "unclassified/tu_802196B0.h"
#include "unclassified/tu_802492E4.h"

extern TLComponentInstance lbl_80580030;
extern TLInstance lbl_80580248;
extern TLComponentInstance* lbl_80578450[4];
extern unsigned int lbl_806E18B0;
extern char lbl_8051EEF8[];
extern char lbl_8051EF04[];
extern char lbl_806DE704[6];
extern char lbl_806DE710[8];
extern char lbl_806DE718[2];
extern char lbl_806DE71C[7];
extern char lbl_806DE794[5];
extern const char* lbl_806DE700;

extern bool fn_8010FE54(unsigned int flag);
extern "C" void fn_801CBCA0(unsigned long hash, int value0, int value1, int value2);
extern "C" const unsigned short* fn_801CA950(nlLocalization* localization, const char* name);
extern "C" TLSlide* fn_801CA63C(FEPresentation* presentation);
extern "C" FEPackage* fn_801CF848(FEScene* scene);
extern "C" const unsigned short* fn_801CF604(
    const BasicString<unsigned short, Detail::TempStringAllocator>* string);
extern "C" void* memcpy(void* dest, const void* src, unsigned long count);
void fn_802172C4();
extern "C" void fn_802534BC(void* object, int value, bool enabled);
extern "C" TLComponentInstance* fn_80253D70(void* object, int index);
extern "C" void* fn_80253E18();
extern "C" void fn_8024DE5C(TU8024D92CScene* scene);
extern "C" void fn_8024EBCC(TU8024D92CScene* scene);

template <typename T>
static inline T* CastFound(TLInstance* found)
{
    if (found == 0)
        return 0;
    return (T*)found;
}

TU8024D92CScene::TU8024D92CScene(int mode)
    : mMode(mode)
    , mNavigation()
    , mUnidentified10C(false)
    , mUnidentified390(lbl_806DE700, 0)
    , mUnidentified430(lbl_806DE700, 0)
{
    mUnidentified4D0 = false;
    mUnidentified4D1 = false;
    mUnidentified0FC[0] = 0;
    mUnidentified0FC[1] = 0;
    mUnidentified0FC[2] = 0;
    mUnidentified0FC[3] = 0;

    mUnidentified020 = fn_8024A0E4();
    mUnidentified10D = fn_8010FE54(fn_8024A0EC(mUnidentified020));
}

TU8024D92CScene::~TU8024D92CScene()
{
}

void TU8024D92CScene::SceneCreated()
{
    fn_802172C4();

    void* object = fn_80253E18();
    TLComponentInstance* screen = 0;
    if (object != 0)
    {
        fn_802534BC(object, 4, true);
        screen = fn_80253D70(object, 4);
    }
    mNavigation.fn_8022F194(screen);

    FEPresentation* presentation = mFEScene->m_pFEPackage->GetPresentation();
    TLComponentInstance* playerCard = CastFound<TLComponentInstance>(
        FEFinder<TLComponentInstance, 2>::_Find<TLSlide>(presentation->GetActiveSlide(),
            nlStringLowerHash(lbl_806DE704), nlStringLowerHash(lbl_8051EEF8), 0, 0, 0, 0));
    if (playerCard == 0)
    {
        playerCard = &lbl_80580030;
    }

    if (mUnidentified10D)
    {
        playerCard->SetActiveSlide(lbl_806DE794, true, false);
    }
    else
    {
        playerCard->SetActiveSlide(lbl_8051EF04, true, false);
    }

    fn_8024DE5C(this);
    fn_8024EBCC(this);
}

void TU8024D92CScene::Update(float fDeltaT)
{
    if (!mUnidentified4D0 || !mUnidentified4D1)
    {
        if (!mUnidentified4D0)
        {
            mUnidentified4D0 = mUnidentified390.Update(true);
        }
        if (!mUnidentified4D1)
        {
            mUnidentified4D1 = mUnidentified430.Update(true);
        }
        if (mUnidentified4D0 && mUnidentified4D1)
        {
            fn_801CBCA0(0x12057B21, 0, 0, 1);
        }
        return;
    }

    BaseSceneHandler::Update(fDeltaT);

    if (!mUnidentified10C)
    {
        TLSlide* slide = mPresentation->GetActiveSlide();
        if (slide->m_time < slide->m_duration + slide->m_start)
        {
            return;
        }
        mUnidentified10C = true;
    }

    for (unsigned int pad = 0; pad < 4; ++pad)
    {
        TLComponentInstance* controller = lbl_80578450[pad];
        bool processInput;
        if (g_pFEInput->m_InputLockDepth == 0)
        {
            if (pad != lbl_806E18B0)
            {
                controller->SetActiveSlide(lbl_806DE710, true, false);
                processInput = false;
            }
            else
            {
                if (mUnidentified0FC[pad] > 0)
                {
                    controller->SetActiveSlide(lbl_806DE718, true, false);
                }
                else
                {
                    controller->SetActiveSlide(lbl_806DE71C, true, false);
                }
                processInput = true;
            }
        }
        else
        {
            processInput = true;
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

            if (mNavigation.fn_8022F2E0(event, fDeltaT))
            {
                fn_80249BF4(mMode);
                return;
            }
        }
    }
}

extern "C" void fn_8024DE5C(TU8024D92CScene* scene)
{
    typedef BasicString<unsigned short, Detail::TempStringAllocator> WideBasicString;

    WideBasicString title;
    WideBasicString description;
    WideBasicString name;

    FEPresentation* presentation = fn_801CF848(scene->mFEScene)->GetPresentation();
    TLTextInstance* titleText = FEFinder<TLTextInstance, 3>::Find<TLSlide>(
        fn_801CA63C(presentation), InlineHasher("Layer"), InlineHasher("player card"),
        InlineHasher("title"));
    TLTextInstance* nameText = FEFinder<TLTextInstance, 3>::Find<TLSlide>(
        fn_801CA63C(presentation), InlineHasher("Layer"), InlineHasher("player card"),
        InlineHasher("name"));
    TLTextInstance* descriptionText = FEFinder<TLTextInstance, 3>::Find<TLSlide>(
        fn_801CA63C(presentation), InlineHasher("Layer"), InlineHasher("player card"),
        InlineHasher("description"));

    switch (scene->mUnidentified020)
    {
    case 0:
        title = fn_801CA950(g_pLocalization, "PLAYER_CARDS_MARIO_TITLE");
        name = fn_801CA950(g_pLocalization, "NAME_MARIO");
        if (scene->mUnidentified10D)
            description = fn_801CA950(g_pLocalization, "PLAYER_CARDS_MARIO_DESCRIPTION");
        else
            description = fn_801CA950(g_pLocalization, "PLAYER_CARDS_MARIO_LOCKED");
        break;
    case 1:
        title = fn_801CA950(g_pLocalization, "PLAYER_CARDS_LUIGI_TITLE");
        name = fn_801CA950(g_pLocalization, "NAME_LUIGI");
        if (scene->mUnidentified10D)
            description = fn_801CA950(g_pLocalization, "PLAYER_CARDS_LUIGI_DESCRIPTION");
        else
            description = fn_801CA950(g_pLocalization, "PLAYER_CARDS_LUIGI_LOCKED");
        break;
    case 2:
        title = fn_801CA950(g_pLocalization, "PLAYER_CARDS_DONKEYKONG_TITLE");
        name = fn_801CA950(g_pLocalization, "NAME_DK");
        if (scene->mUnidentified10D)
            description
                = fn_801CA950(g_pLocalization, "PLAYER_CARDS_DONKEYKONG_DESCRIPTION");
        else
            description = fn_801CA950(g_pLocalization, "PLAYER_CARDS_DONKEYKONG_LOCKED");
        break;
    case 3:
        title = fn_801CA950(g_pLocalization, "PLAYER_CARDS_PEACH_TITLE");
        name = fn_801CA950(g_pLocalization, "NAME_PEACH");
        if (scene->mUnidentified10D)
            description = fn_801CA950(g_pLocalization, "PLAYER_CARDS_PEACH_DESCRIPTION");
        else
            description = fn_801CA950(g_pLocalization, "PLAYER_CARDS_PEACH_LOCKED");
        break;
    case 4:
        title = fn_801CA950(g_pLocalization, "PLAYER_CARDS_DAISY_TITLE");
        name = fn_801CA950(g_pLocalization, "NAME_DAISY");
        if (scene->mUnidentified10D)
            description = fn_801CA950(g_pLocalization, "PLAYER_CARDS_DAISY_DESCRIPTION");
        else
            description = fn_801CA950(g_pLocalization, "PLAYER_CARDS_DAISY_LOCKED");
        break;
    case 5:
        title = fn_801CA950(g_pLocalization, "PLAYER_CARDS_WARIO_TITLE");
        name = fn_801CA950(g_pLocalization, "NAME_WARIO");
        if (scene->mUnidentified10D)
            description = fn_801CA950(g_pLocalization, "PLAYER_CARDS_WARIO_DESCRIPTION");
        else
            description = fn_801CA950(g_pLocalization, "PLAYER_CARDS_WARIO_LOCKED");
        break;
    case 6:
        title = fn_801CA950(g_pLocalization, "PLAYER_CARDS_WALUIGI_TITLE");
        name = fn_801CA950(g_pLocalization, "NAME_WALUIGI");
        if (scene->mUnidentified10D)
            description = fn_801CA950(g_pLocalization, "PLAYER_CARDS_WALUIGI_DESCRIPTION");
        else
            description = fn_801CA950(g_pLocalization, "PLAYER_CARDS_WALUIGI_LOCKED");
        break;
    case 7:
        title = fn_801CA950(g_pLocalization, "PLAYER_CARDS_YOSHI_TITLE");
        name = fn_801CA950(g_pLocalization, "NAME_YOSHI");
        if (scene->mUnidentified10D)
            description = fn_801CA950(g_pLocalization, "PLAYER_CARDS_YOSHI_DESCRIPTION");
        else
            description = fn_801CA950(g_pLocalization, "PLAYER_CARDS_YOSHI_LOCKED");
        break;
    case 8:
        title = fn_801CA950(g_pLocalization, "PLAYER_CARDS_BOWSER_TITLE");
        name = fn_801CA950(g_pLocalization, "NAME_BOWSER");
        if (scene->mUnidentified10D)
            description = fn_801CA950(g_pLocalization, "PLAYER_CARDS_BOWSER_DESCRIPTION");
        else
            description = fn_801CA950(g_pLocalization, "PLAYER_CARDS_BOWSER_LOCKED");
        break;
    case 9:
        title = fn_801CA950(g_pLocalization, "PLAYER_CARDS_PETEY_TITLE");
        name = fn_801CA950(g_pLocalization, "NAME_PETEY");
        if (scene->mUnidentified10D)
            description = fn_801CA950(g_pLocalization, "PLAYER_CARDS_PETEY_DESCRIPTION");
        else
            description = fn_801CA950(g_pLocalization, "PLAYER_CARDS_PETEY_LOCKED");
        break;
    case 10:
        title = fn_801CA950(g_pLocalization, "PLAYER_CARDS_BOWSERJR_TITLE");
        name = fn_801CA950(g_pLocalization, "NAME_BOWSERJR");
        if (scene->mUnidentified10D)
            description = fn_801CA950(g_pLocalization, "PLAYER_CARDS_BOWSERJR_DESCRIPTION");
        else
            description = fn_801CA950(g_pLocalization, "PLAYER_CARDS_BOWSERJR_LOCKED");
        break;
    case 11:
        title = fn_801CA950(g_pLocalization, "PLAYER_CARDS_DIDDYKONG_TITLE");
        name = fn_801CA950(g_pLocalization, "NAME_DIDDYKONG");
        if (scene->mUnidentified10D)
            description
                = fn_801CA950(g_pLocalization, "PLAYER_CARDS_DIDDYKONG_DESCRIPTION");
        else
            description = fn_801CA950(g_pLocalization, "PLAYER_CARDS_DIDDYKONG_LOCKED");
        break;
    }

    memcpy(scene->mUnidentified10E, fn_801CF604(&title),
        sizeof(scene->mUnidentified10E));
    titleText->SetString(scene->mUnidentified10E);
    memcpy(scene->mUnidentified34E, fn_801CF604(&name), sizeof(scene->mUnidentified34E));
    nameText->SetString(scene->mUnidentified34E);
    memcpy(scene->mUnidentified14E, fn_801CF604(&description),
        sizeof(scene->mUnidentified14E));
    descriptionText->SetString(scene->mUnidentified14E);
}

extern "C" void fn_8024EBCC(TU8024D92CScene* scene)
{
    FEPresentation* presentation;
    TLImageInstance* image0;
    TLImageInstance* image1;
    const char* name1;
    const char* name0;
    char path0[64];
    char path1[64];

    presentation = scene->mFEScene->m_pFEPackage->GetPresentation();
    {
        unsigned long playerCardHash;
        unsigned long imageHash;
        imageHash = nlStringLowerHash("00_dummy_texture");
        playerCardHash = nlStringLowerHash(lbl_8051EEF8);
        image0 = CastFound<TLImageInstance>(
            FEFinder<TLImageInstance, 3>::_Find<TLSlide>(presentation->GetActiveSlide(),
                nlStringLowerHash(lbl_806DE704), playerCardHash, imageHash, 0, 0, 0));
    }
    if (image0 == 0)
    {
        image0 = (TLImageInstance*)&lbl_80580248;
    }

    {
        unsigned long imageHash = nlStringLowerHash("00_dummy_texture_positions");
        unsigned long playerCardHash = nlStringLowerHash(lbl_8051EEF8);
        image1 = CastFound<TLImageInstance>(
            FEFinder<TLImageInstance, 3>::_Find<TLSlide>(presentation->GetActiveSlide(),
                nlStringLowerHash(lbl_806DE704), playerCardHash, imageHash, 0, 0, 0));
    }
    if (image1 == 0)
    {
        image1 = (TLImageInstance*)&lbl_80580248;
    }

    name0 = 0;
    name1 = 0;
    switch (scene->mUnidentified020)
    {
    case 0:
        name0 = "PLAYER_CARDS_MARIO";
        name1 = "PLAYER_CARDS_BACK_MARIO";
        break;
    case 1:
        name0 = "PLAYER_CARDS_LUIGI";
        name1 = "PLAYER_CARDS_BACK_LUIGI";
        break;
    case 2:
        name0 = "PLAYER_CARDS_DONKEYKONG";
        name1 = "PLAYER_CARDS_BACK_DONKEYKONG";
        break;
    case 3:
        name0 = "PLAYER_CARDS_PEACH";
        name1 = "PLAYER_CARDS_BACK_PEACH";
        break;
    case 4:
        name0 = "PLAYER_CARDS_DAISY";
        name1 = "PLAYER_CARDS_BACK_DAISY";
        break;
    case 5:
        name0 = "PLAYER_CARDS_WARIO";
        name1 = "PLAYER_CARDS_BACK_WARIO";
        break;
    case 6:
        name0 = "PLAYER_CARDS_WALUIGI";
        name1 = "PLAYER_CARDS_BACK_WALUIGI";
        break;
    case 7:
        name0 = "PLAYER_CARDS_YOSHI";
        name1 = "PLAYER_CARDS_BACK_YOSHI";
        break;
    case 8:
        name0 = "PLAYER_CARDS_BOWSER";
        name1 = "PLAYER_CARDS_BACK_BOWSER";
        break;
    case 9:
        name0 = "PLAYER_CARDS_PETEY";
        name1 = "PLAYER_CARDS_BACK_PETEY";
        break;
    case 10:
        name0 = "PLAYER_CARDS_BOWSERJR";
        name1 = "PLAYER_CARDS_BACK_BOWSERJR";
        break;
    case 11:
        name0 = "PLAYER_CARDS_DIDDYKONG";
        name1 = "PLAYER_CARDS_BACK_DIDDYKONG";
        break;
    }

    nlSNPrintf(path0, sizeof(path0), "fe/screens/images/%s", name0);
    nlSNPrintf(path1, sizeof(path1), "fe/screens/images/%s", name1);

    scene->mUnidentified390.mImageInstance = image0;
    scene->mUnidentified430.mImageInstance = image1;
    scene->mUnidentified390.QueueLoad(path0, false);
    scene->mUnidentified430.QueueLoad(path1, false);
}
