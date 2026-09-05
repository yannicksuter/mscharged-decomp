#include "unclassified/tu_8024D92C.h"

#include "Game/FE/feFinder.h"
#include "Game/FE/feInput.h"
#include "Game/FE/fePackage.h"
#include "Game/FE/fePresentation.h"
#include "Game/FE/feScene.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlSlide.h"
#include "NL/nlString.h"
#include "unclassified/tu_802196B0.h"
#include "unclassified/tu_802492E4.h"

extern TLComponentInstance lbl_80580030;
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
