#include "unclassified/tu_8024F4E0.h"

#include "Game/FE/feFinder.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlTextInstance.h"
#include "Game/tu_801360A4.h"
#include "NL/nlBind.h"
#include "NL/nlString.h"

#include <string.h>

extern TLComponentInstance lbl_80580138;
extern "C" void fn_801CBCA0(unsigned long hash, int value0, int value1, int value2);

TU80250754Scene::~TU80250754Scene()
{
}

void TU80250754Scene::fn_8024F570()
{
    TU80300104Base::Callback padSelect(
        Bind<void>(MemFun(&TU80250754Scene::fn_80251328), this, Placeholder<0>(), Placeholder<1>()));
    TU80300104Base::Callback padOver(
        Bind<void>(MemFun(&TU80250754Scene::fn_8024FDD4), this, Placeholder<0>(), Placeholder<1>()));
    TU80300104Base::Callback padOff(
        Bind<void>(MemFun(&TU80250754Scene::fn_8024FE6C), this, Placeholder<0>(), Placeholder<1>()));
    TU80300104Base::Callback codeSelect(
        Bind<void>(MemFun(&TU80250754Scene::fn_8024FEEC), this, Placeholder<0>(), Placeholder<1>()));
    TU80300104Base::Callback codeOver(
        Bind<void>(MemFun(&TU80250754Scene::fn_8025005C), this, Placeholder<0>(), Placeholder<1>()));
    TU80300104Base::Callback codeOff(
        Bind<void>(MemFun(&TU80250754Scene::fn_80250100), this, Placeholder<0>(), Placeholder<1>()));

    for (int i = 0; i < 12; ++i)
    {
        float scale = 0.8f;
        TLComponentInstance* positionInstance =
            (TLComponentInstance*)FEFinder<TLComponentInstance, 3>::_Find<TLSlide>(
                mPresentation->GetActiveSlide(), nlStringLowerHash("Layer"),
                nlStringLowerHash("Group"), nlStringLowerHash("PAD"), 0, 0, 0);
        if (positionInstance == 0)
        {
            positionInstance = &lbl_80580138;
        }

        feVector3 position = positionInstance->GetAssetPosition();
        if (i == 11)
        {
            scale = 0.7f;
        }
        mUnidentified0058[i].fn_80300D74(
            mUnidentified1210[i], true, position.f.x, position.f.y, scale, scale);
        mUnidentified0058[i].fn_803009AC(padSelect);
        mUnidentified0058[i].fn_803007C0(padOver);
        mUnidentified0058[i].fn_80300864(padOff);
    }

    for (int i = 0; i < 12; ++i)
    {
        TLComponentInstance* positionInstance =
            (TLComponentInstance*)FEFinder<TLComponentInstance, 3>::_Find<TLSlide>(
                mPresentation->GetActiveSlide(), nlStringLowerHash("Layer"),
                nlStringLowerHash("Group"), nlStringLowerHash("CODE"), 0, 0, 0);
        if (positionInstance == 0)
        {
            positionInstance = &lbl_80580138;
        }

        feVector3 position = positionInstance->GetAssetPosition();
        mUnidentified08C8[i].fn_80300D74(
            mUnidentified1240[i], true, position.f.x, position.f.y, 0.8f, 0.8f);
        mUnidentified08C8[i].fn_803009AC(codeSelect);
        mUnidentified08C8[i].fn_803007C0(codeOver);
        mUnidentified08C8[i].fn_80300864(codeOff);
    }
}

void TU80250754Scene::fn_8024FDD4(int index, void* context)
{
    unsigned int item = (unsigned int)context;
    ++mUnidentified001C;
    mUnidentified1210[item]->SetActiveSlide("over", true, false);
    mUnidentified0058[item].mValues[index] = 1;
    fn_801CBCA0(0x0E2B7F90, 0, 0, 1);
}

void TU80250754Scene::fn_8024FE6C(int index, void* context)
{
    unsigned int item = (unsigned int)context;
    --mUnidentified001C;
    mUnidentified1210[item]->SetActiveSlide("off", true, false);
    mUnidentified0058[item].mValues[index] = 0;
}

void TU80250754Scene::fn_8024FEEC(int, void* context)
{
    fn_801CBCA0(0x3021A1EE, 0, 0, 1);

    int item = (int)context;
    if (item >= 12)
    {
        item = 11;
    }
    if (item < 0)
    {
        item = 0;
    }

    if (mUnidentified0024 != item)
    {
        mUnidentified1240[item]->SetActiveSlide("DOWN", true, false);
        mUnidentified1240[mUnidentified0024]->SetActiveSlide("OFF", true, false);

        mUnidentified08C8[item].mDisabled = true;
        TU80300104Event event;
        mUnidentified08C8[item].mPreviousEvents[0] = event;
        mUnidentified08C8[item].mPreviousEvents[1] = event;
        mUnidentified08C8[item].mPreviousEvents[2] = event;
        mUnidentified08C8[item].mPreviousEvents[3] = event;

        mUnidentified08C8[mUnidentified0024].mDisabled = false;
        mUnidentified0024 = item;
    }

    --mUnidentified001C;
}

void TU80250754Scene::fn_8025005C(int index, void* context)
{
    unsigned int item = (unsigned int)context;
    if (item != mUnidentified0024)
    {
        ++mUnidentified001C;
        mUnidentified1240[item]->SetActiveSlide("over", true, false);
        mUnidentified08C8[item].mValues[index] = 1;
        fn_801CBCA0(0xFFC8A55D, 0, 0, 1);
    }
}

void TU80250754Scene::fn_80250100(int index, void* context)
{
    unsigned int item = (unsigned int)context;
    if (item != mUnidentified0024)
    {
        --mUnidentified001C;
        mUnidentified1240[item]->SetActiveSlide("off", true, false);
        mUnidentified08C8[item].mValues[index] = 0;
    }
}

void TU80250754Scene::fn_8025018C()
{
    unsigned short* friendCode =
        (unsigned short*)lbl_806E1194->mUnidentified018;
    unsigned short character[2];
    character[1] = 0;
    bool foundEmpty = false;

    for (int i = 0; i < 12; ++i)
    {
        if (friendCode[i] == 0 && !foundEmpty)
        {
            int item = i;
            if (item >= 12)
            {
                item = 11;
            }
            if (item < 0)
            {
                item = 0;
            }

            if (mUnidentified0024 != item)
            {
                mUnidentified1240[item]->SetActiveSlide("DOWN", true, false);
                mUnidentified1240[mUnidentified0024]->SetActiveSlide(
                    "OFF", true, false);

                mUnidentified08C8[item].mDisabled = true;
                TU80300104Event event;
                mUnidentified08C8[item].mPreviousEvents[0] = event;
                mUnidentified08C8[item].mPreviousEvents[1] = event;
                mUnidentified08C8[item].mPreviousEvents[2] = event;
                mUnidentified08C8[item].mPreviousEvents[3] = event;

                mUnidentified08C8[mUnidentified0024].mDisabled = false;
                mUnidentified0024 = item;
            }
            foundEmpty = true;
        }

        character[0] = friendCode[i];
        int item = i;
        if (item < 0)
        {
            item = mUnidentified0024;
        }
        nlStrNCpy(mUnidentified0028[item], character, 2);

        TLTextInstance* text = FEFinder<TLTextInstance, 3>::Find(
            mUnidentified1240[item], nlStringLowerHash("off"),
            nlStringLowerHash("BOX"), nlStringLowerHash("NUMBER"), 0, 0, 0);
        if (text == 0)
        {
            text = &UnidentifiedFallbackTextInstance;
        }
        text->SetString(mUnidentified0028[item]);

        text = FEFinder<TLTextInstance, 3>::Find(
            mUnidentified1240[item], nlStringLowerHash("over"),
            nlStringLowerHash("BOX"), nlStringLowerHash("NUMBER"), 0, 0, 0);
        if (text == 0)
        {
            text = &UnidentifiedFallbackTextInstance;
        }
        text->SetString(mUnidentified0028[item]);

        text = FEFinder<TLTextInstance, 3>::Find(
            mUnidentified1240[item], nlStringLowerHash("down"),
            nlStringLowerHash("BOX"), nlStringLowerHash("NUMBER"), 0, 0, 0);
        if (text == 0)
        {
            text = &UnidentifiedFallbackTextInstance;
        }
        text->SetString(mUnidentified0028[item]);
    }

    if (!foundEmpty && mUnidentified0024 != 11)
    {
        mUnidentified1240[11]->SetActiveSlide("DOWN", true, false);
        mUnidentified1240[mUnidentified0024]->SetActiveSlide("OFF", true, false);

        mUnidentified08C8[11].mDisabled = true;
        TU80300104Event event;
        mUnidentified08C8[11].mPreviousEvents[0] = event;
        mUnidentified08C8[11].mPreviousEvents[1] = event;
        mUnidentified08C8[11].mPreviousEvents[2] = event;
        mUnidentified08C8[11].mPreviousEvents[3] = event;

        mUnidentified08C8[mUnidentified0024].mDisabled = false;
        mUnidentified0024 = 11;
    }

    memset(friendCode, 0, sizeof(lbl_806E1194->mUnidentified018));
}

void TU80250754Scene::fn_802505E8()
{
    mUnidentified1210[10]->m_bVisible = true;
    mUnidentified0058[10].mDisabled = false;
    mUnidentified1270->m_bVisible = true;

    bool valid = true;
    for (int i = 0; i < 12; ++i)
    {
        if (mUnidentified0028[i][0] == 0)
        {
            mUnidentified1270->m_bVisible = false;
            mUnidentified1210[10]->m_bVisible = false;
            mUnidentified0058[10].mDisabled = true;

            TU80300104Event event;
            mUnidentified0058[10].mPreviousEvents[0] = event;
            mUnidentified0058[10].mPreviousEvents[1] = event;
            mUnidentified0058[10].mPreviousEvents[2] = event;
            mUnidentified0058[10].mPreviousEvents[3] = event;
            valid = false;
        }
    }

    if (valid)
    {
        fn_801CBCA0(0xCC2C93F1, 0, 0, 1);
    }
}
