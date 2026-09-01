#include "unclassified/tu_8022EF84.h"

#include "Game/BaseGameSceneManager.h"
#include "Game/FE/feFinder.h"
#include "Game/FE/tlComponentInstance.h"
#include "NL/nlBind.h"
#include "NL/nlString.h"

extern "C" void fn_801CBCA0(unsigned long hash, int value0, int value1, int value2);
extern "C" bool fn_80273B00();
extern "C" TLInstance* fn_803068F4(TLInstance* instance, unsigned long level1,
    unsigned long level2, unsigned long level3, unsigned long level4,
    unsigned long level5, unsigned long level6);

extern BaseGameSceneManager* lbl_806E1838;
extern BaseGameSceneManager* lbl_806E1860;
extern TLInstance lbl_80580248;

/**
 * Offset/Address/Size: 0x0 | 0x8022EF84 | size: 0x84
 */
TU8022EF84Component::TU8022EF84Component()
    : TU80219248Component((void*)0)
    , mUnidentifiedB4(SCENE_INVALID)
    , mUnidentifiedB8(0.0f)
    , mUnidentifiedC8(0)
    , mUnidentifiedCC(false)
    , mUnidentifiedCD(false)
    , mUnidentifiedCE(false)
    , mUnidentifiedCF(true)
    , mUnidentifiedD0(true)
    , mUnidentifiedD1(false)
{
    mUnidentifiedD2[0] = false;
    mUnidentifiedD2[1] = false;
    mUnidentifiedD2[2] = false;
    mUnidentifiedD2[3] = false;
}

/**
 * Offset/Address/Size: 0x84 | 0x8022F008 | size: 0x5C
 */
TU8022EF84Component::~TU8022EF84Component()
{
}

/**
 * Offset/Address/Size: 0xE0 | 0x8022F064 | size: 0xA0
 */
void TU8022EF84Component::fn_80301BA8(int index, void* context)
{
    if (!fn_802192FC(1, index))
    {
        mUnidentifiedC8->SetActiveSlide("over", true, false);
        fn_801CBCA0(0xACCDCA48, 0, 0, 1);
    }
    mValues[index] = 1;
    TU80219248Component::fn_80301BA8(index, context);
}

/**
 * Offset/Address/Size: 0x180 | 0x8022F104 | size: 0x10
 */
void TU8022EF84Component::fn_80301C28(int index, void*)
{
    mUnidentifiedD2[index] = true;
}

/**
 * Offset/Address/Size: 0x190 | 0x8022F114 | size: 0x4C
 */
void TU8022EF84Component::fn_80301C68(int index, void* context)
{
    TU80219248Component::fn_80301C68(index, context);
    fn_801CBCA0(0x6F6A3A07, 0, 0, 1);
    mUnidentifiedCC = true;
}

/**
 * Offset/Address/Size: 0x1DC | 0x8022F160 | size: 0x34
 */
void TU8022EF84Component::fn_80301D28(int index, void* context)
{
    TU80219248Component::fn_80301D28(index, context);
    mUnidentifiedCC = false;
}

/**
 * Offset/Address/Size: 0x210 | 0x8022F194 | size: 0x14C
 */
void TU8022EF84Component::fn_8022F194(TLComponentInstance* instance)
{
    if (fn_80273B00())
    {
        instance->SetActiveSlide("16:9", true, false);
    }
    else
    {
        instance->SetActiveSlide("4:3", true, false);
    }

    mUnidentifiedBC = instance->GetAssetPosition();
    mUnidentifiedC8 = (TLComponentInstance*)FEFinder<TLComponentInstance, 2>::_Find<TLSlide>(
        instance->GetActiveSlide(), nlStringLowerHash("back"), 0, 0, 0, 0, 0);
}

/**
 * Offset/Address/Size: 0x35C | 0x8022F2E0 | size: 0x2B0
 */
bool TU8022EF84Component::fn_8022F2E0(TU80300104Event event, float)
{
    if (mDisabled)
    {
        return false;
    }

    if (!mUnidentifiedD1)
    {
        mUnidentifiedD1 = true;

        TU80300104Base::Callback callback(Bind<void>(
            MemFun(&TU8022EF84Component::fn_80301C28), this, Placeholder<0>(), Placeholder<1>()));
        fn_80300908(callback);

        TLInstance* found = fn_803068F4(mUnidentifiedC8,
            nlStringLowerHash("over"),
            nlStringLowerHash("list_high_250x60"),
            0,
            0,
            0,
            0);
        TLInstance* over;
        if (found == 0)
        {
            over = &lbl_80580248;
        }
        else
        {
            over = found;
        }

        feVector3 position = mUnidentifiedC8->GetAssetPosition();
        fn_80300D74(over, true, position.f.x, position.f.y, 1.0f, 1.0f);
    }

    mUnidentifiedD2[event.mIndex] = false;
    fn_80219608(&event);

    BaseGameSceneManager* manager = lbl_806E1838;
    if (manager == 0)
    {
        manager = lbl_806E1860;
    }

    if (mUnidentifiedCC && mUnidentifiedCF)
    {
        if (mUnidentifiedB4 != SCENE_INVALID)
        {
            manager->Push((SceneList)mUnidentifiedB4, SCREEN_BACK, true);
        }
        mUnidentifiedCC = false;
        return true;
    }

    if (mUnidentifiedCC && !mUnidentifiedCF)
    {
        if (mUnidentifiedD0)
        {
            manager->Pop();
        }
        mUnidentifiedCC = false;
        return true;
    }

    return false;
}

/**
 * Offset/Address/Size: 0x60C | 0x8022F590 | size: 0x98
 */
void TU8022EF84Component::fn_80301CE8(int index, void* context)
{
    if (!fn_802192FC(1, index))
    {
        mUnidentifiedB8 = 0.0f;
        mUnidentifiedC8->SetActiveSlide("off", true, false);
        mUnidentifiedCE = false;
    }
    mValues[index] = 0;
    TU80219248Component::fn_80301CE8(index, context);
}
