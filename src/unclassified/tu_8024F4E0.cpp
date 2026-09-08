// MSL math.h defines abs/labs; include it before stdlib.h.
#include <math.h>
#include "unclassified/tu_8024F4E0.h"

#include "Game/SH/SHNavigation.h"
#include "Game/GameSceneManager.h"
#include "Game/DB/SaveLoad.h"
#include "Game/FE/FEAudio.h"
#include "Game/FE/feFinder.h"
#include "Game/FE/feInput.h"
#include "Game/FE/fePopupMenu.h"
#include "Game/FE/fePresentation.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlSlide.h"
#include "Game/FE/tlTextInstance.h"
#include "Game/FriendManager.h"
#include "NL/nlBind.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"
#include "Game/FE/feDPD.h"

#include <stdlib.h>
#include <string.h>
#include "NL/nlstring_tmpl.h"

extern char lbl_806DE80C[];

TU80250754Scene::TU80250754Scene()
    : mUnidentified001C(0)
    , mUnidentified0020(false)
    , mUnidentified0024(-1)
    , mUnidentified0058()
    , mUnidentified08C8()
    , mUnidentified1138()
    , mUnidentified1274(false)
    , mUnidentified1278(0)
{
    const unsigned short* empty = (const unsigned short*)L"";

    for (int i = 0; i < 12; ++i)
    {
        mUnidentified0058[i].mContext = (void*)i;
    }

    for (int i = 0; i < 12; ++i)
    {
        mUnidentified08C8[i].mContext = (void*)i;
        nlStrNCpy(mUnidentified0028[i], empty, 2);
    }

    mUnidentified1138.SetPopScene(false);
}

TU80250754Scene::~TU80250754Scene()
{
}

void TU80250754Scene::fn_8024F570()
{
    FEPointerListener::Callback padSelect(
        Bind<void>(MemFun(&TU80250754Scene::fn_80251328), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback padOver(
        Bind<void>(MemFun(&TU80250754Scene::fn_8024FDD4), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback padOff(
        Bind<void>(MemFun(&TU80250754Scene::fn_8024FE6C), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback codeSelect(
        Bind<void>(MemFun(&TU80250754Scene::fn_8024FEEC), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback codeOver(
        Bind<void>(MemFun(&TU80250754Scene::fn_8025005C), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback codeOff(
        Bind<void>(MemFun(&TU80250754Scene::fn_80250100), this, Placeholder<0>(), Placeholder<1>()));

    for (int i = 0; i < 12; ++i)
    {
        float scale = 0.8f;
        TLInstance* positionInstance =
            FEFinder<TLInstance, 5>::_Find<TLSlide>(
                mPresentation->m_currentSlide, nlStringLowerHash("Layer"),
                nlStringLowerHash("Group"), nlStringLowerHash("PAD"), 0, 0, 0);
        if (positionInstance == 0)
        {
            positionInstance = &gDefaultTLGroupInstance;
        }

        feVector3 position = positionInstance->GetAssetPosition();
        if (i == 11)
        {
            scale = 0.7f;
        }
        mUnidentified0058[i].SetInstanceBounds(
            mUnidentified1210[i], true, position.f.x, position.f.y, scale, scale);
        mUnidentified0058[i].SetPointerPressCallback(padSelect);
        mUnidentified0058[i].SetPointerEnterCallback(padOver);
        mUnidentified0058[i].SetPointerLeaveCallback(padOff);
    }

    for (int i = 0; i < 12; ++i)
    {
        TLInstance* positionInstance =
            FEFinder<TLInstance, 5>::_Find<TLSlide>(
                mPresentation->m_currentSlide, nlStringLowerHash("Layer"),
                nlStringLowerHash("Group"), nlStringLowerHash("CODE"), 0, 0, 0);
        if (positionInstance == 0)
        {
            positionInstance = &gDefaultTLGroupInstance;
        }

        feVector3 position = positionInstance->GetAssetPosition();
        mUnidentified08C8[i].SetInstanceBounds(
            mUnidentified1240[i], true, position.f.x, position.f.y, 0.8f, 0.8f);
        mUnidentified08C8[i].SetPointerPressCallback(codeSelect);
        mUnidentified08C8[i].SetPointerEnterCallback(codeOver);
        mUnidentified08C8[i].SetPointerLeaveCallback(codeOff);
    }
}

void TU80250754Scene::fn_8024FDD4(int index, void* context)
{
    unsigned int item = (unsigned int)context;
    ++mUnidentified001C;
    mUnidentified1210[item]->SetActiveSlide("over", true, false);
    mUnidentified0058[item].SetPointerState(1, index);
    FEAudio::PlayAnimAudioEvent(0x0E2B7F90, 0, 0, 1);
}

void TU80250754Scene::fn_8024FE6C(int index, void* context)
{
    unsigned int item = (unsigned int)context;
    --mUnidentified001C;
    mUnidentified1210[item]->SetActiveSlide("off", true, false);
    mUnidentified0058[item].SetPointerState(0, index);
}

void TU80250754Scene::fn_8024FEEC(int, void* context)
{
    FEAudio::PlayAnimAudioEvent(0x3021A1EE, 0, 0, 1);

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
        FEPointerEvent event;
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
        mUnidentified08C8[item].SetPointerState(1, index);
        FEAudio::PlayAnimAudioEvent(0xFFC8A55D, 0, 0, 1);
    }
}

void TU80250754Scene::fn_80250100(int index, void* context)
{
    unsigned int item = (unsigned int)context;
    if (item != mUnidentified0024)
    {
        --mUnidentified001C;
        mUnidentified1240[item]->SetActiveSlide("off", true, false);
        mUnidentified08C8[item].SetPointerState(0, index);
    }
}

void TU80250754Scene::fn_8025018C()
{
    unsigned short* friendCode =
        g_pFriendManager->mFriendCodeInput;
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
                FEPointerEvent event;
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
            text = &gDefaultTLTextInstance;
        }
        text->SetString(mUnidentified0028[item]);

        text = FEFinder<TLTextInstance, 3>::Find(
            mUnidentified1240[item], nlStringLowerHash("over"),
            nlStringLowerHash("BOX"), nlStringLowerHash("NUMBER"), 0, 0, 0);
        if (text == 0)
        {
            text = &gDefaultTLTextInstance;
        }
        text->SetString(mUnidentified0028[item]);

        text = FEFinder<TLTextInstance, 3>::Find(
            mUnidentified1240[item], nlStringLowerHash("down"),
            nlStringLowerHash("BOX"), nlStringLowerHash("NUMBER"), 0, 0, 0);
        if (text == 0)
        {
            text = &gDefaultTLTextInstance;
        }
        text->SetString(mUnidentified0028[item]);
    }

    if (!foundEmpty && mUnidentified0024 != 11)
    {
        mUnidentified1240[11]->SetActiveSlide("DOWN", true, false);
        mUnidentified1240[mUnidentified0024]->SetActiveSlide("OFF", true, false);

        mUnidentified08C8[11].mDisabled = true;
        FEPointerEvent event;
        mUnidentified08C8[11].mPreviousEvents[0] = event;
        mUnidentified08C8[11].mPreviousEvents[1] = event;
        mUnidentified08C8[11].mPreviousEvents[2] = event;
        mUnidentified08C8[11].mPreviousEvents[3] = event;

        mUnidentified08C8[mUnidentified0024].mDisabled = false;
        mUnidentified0024 = 11;
    }

    memset(friendCode, 0, sizeof(g_pFriendManager->mFriendCodeInput));
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

            FEPointerEvent event;
            mUnidentified0058[10].mPreviousEvents[0] = event;
            mUnidentified0058[10].mPreviousEvents[1] = event;
            mUnidentified0058[10].mPreviousEvents[2] = event;
            mUnidentified0058[10].mPreviousEvents[3] = event;
            valid = false;
        }
    }

    if (valid)
    {
        FEAudio::PlayAnimAudioEvent(0xCC2C93F1, 0, 0, 1);
    }
}

void TU80250754Scene::fn_80250718()
{
    g_pFriendManager->SetOwnStatusInitial(true);
    mUnidentified1274 = false;
}

void TU80250754Scene::SceneCreated()
{
    char name[12];

    for (int i = 0; i < 12; ++i)
    {
        if (i == 10)
        {
            nlSNPrintf(name, sizeof(name), "button_ok");
        }
        else if (i == 11)
        {
            nlSNPrintf(name, sizeof(name), "BS");
        }
        else
        {
            nlSNPrintf(name, sizeof(name), "BUTTON_%d", i);
        }

        TLComponentInstance* component = FEFinder<TLComponentInstance, 4>::Find(
            mPresentation->m_currentSlide, nlStringLowerHash("Layer"),
            nlStringLowerHash("Group"), nlStringLowerHash("PAD"),
            nlStringLowerHash(name), 0, 0);
        if (component == 0)
        {
            component = &gDefaultTLComponentInstance;
        }
        mUnidentified1210[i] = component;
    }

    for (int i = 0; i < 12; ++i)
    {
        nlSNPrintf(name, sizeof(name), "CODE_BOX_%d", i);
        TLComponentInstance* component = FEFinder<TLComponentInstance, 4>::Find(
            mPresentation->m_currentSlide, nlStringLowerHash("Layer"),
            nlStringLowerHash("Group"), nlStringLowerHash("CODE"),
            nlStringLowerHash(name), 0, 0);
        if (component == 0)
        {
            component = &gDefaultTLComponentInstance;
        }
        mUnidentified1240[i] = component;
    }

    mUnidentified1270 = FEFinder<TLComponentInstance, 5>::Find(mPresentation,
        nlStringLowerHash("out"), nlStringLowerHash("Layer"),
        nlStringLowerHash("Group"), nlStringLowerHash("PAD"),
        nlStringLowerHash("button_ok"), 0);

    for (int i = 0; i < 12; ++i)
    {
        int item = i;
        if (item < 0)
        {
            item = mUnidentified0024;
        }
        nlStrNCpy(mUnidentified0028[item], (const unsigned short*)L"", 2);

        TLTextInstance* text = FEFinder<TLTextInstance, 3>::Find(
            mUnidentified1240[item], nlStringLowerHash("off"),
            nlStringLowerHash("BOX"), nlStringLowerHash("NUMBER"), 0, 0, 0);
        if (text == 0)
        {
            text = &gDefaultTLTextInstance;
        }
        text->SetString(mUnidentified0028[item]);

        text = FEFinder<TLTextInstance, 3>::Find(
            mUnidentified1240[item], nlStringLowerHash("over"),
            nlStringLowerHash("BOX"), nlStringLowerHash("NUMBER"), 0, 0, 0);
        if (text == 0)
        {
            text = &gDefaultTLTextInstance;
        }
        text->SetString(mUnidentified0028[item]);

        text = FEFinder<TLTextInstance, 3>::Find(
            mUnidentified1240[item], nlStringLowerHash("down"),
            nlStringLowerHash("BOX"), nlStringLowerHash("NUMBER"), 0, 0, 0);
        if (text == 0)
        {
            text = &gDefaultTLTextInstance;
        }
        text->SetString(mUnidentified0028[item]);
    }

    fn_8025018C();
    fn_802505E8();

    for (int i = 0; i < 4; ++i)
    {
        gFEPointerInstances[i]->SetActiveSlide("waiting", true, false);
    }

    SHNavigation* object = GetNavigationScene();
    TLComponentInstance* screen = 0;
    if (object != 0)
    {
        object->SetButtons(0, true);
        screen = object->GetButton(4);
    }
    mUnidentified1138.SetButtonInstance(screen);

    FEAudio::PlayAnimAudioEvent(0xBB142B94, 0, 0, 1);
}

void TU80250754Scene::Update(float fDeltaT)
{
    BaseSceneHandler::Update(fDeltaT);

    if (SaveEnabled && InOperation)
    {
        return;
    }

    if (mUnidentified1274 && !g_pFEInput->HasInputLock(this))
    {
        return;
    }

    int state = mUnidentified1278;
    if (state == 0 || (unsigned int)(state - 2) <= 1)
    {
        TLSlide* slide = mPresentation->m_currentSlide;
        if (slide->GetCurrentTime() < slide->m_duration + slide->m_start)
        {
            for (int pad = 0; pad < 4; ++pad)
            {
                gFEPointerInstances[pad]->SetActiveSlide("waiting", true, false);
            }
            return;
        }

        if (state == 0)
        {
            SHNavigation* object = GetNavigationScene();
            if (object != 0)
            {
                object->SetButtons(4, true);
            }
            mUnidentified1278 = 1;
            fn_8024F570();
            mUnidentified0020 = true;
        }
        else if (state == 2)
        {
            GameSceneManager::Instance()->Push((SceneList)0x2F, SCREEN_FORWARD, true);
            return;
        }
        else if (state == 3)
        {
            GameSceneManager::Instance()->Push((SceneList)0x2F, SCREEN_BACK, true);
            return;
        }
    }

    if (!GameSceneManager::Instance()->IsOnStack((SceneList)0xA)
        && g_pFriendManager->FindHostInvitation())
    {
        g_pFriendManager->mReturnScene = 0x30;
        g_pFriendManager->mPreviousRankedMode = 0;
        unsigned short* friendCode =
            g_pFriendManager->mFriendCodeInput;
        for (int i = 0; i < 12; ++i)
        {
            friendCode[i] = mUnidentified0028[i][0];
        }
        GameSceneManager::Instance()->Push(SCENE_ONLINE_INVITE_RESPONSE, SCREEN_FORWARD, true);
        return;
    }

    for (int pad = 0; pad < 4; ++pad)
    {
        TLComponentInstance* controller = gFEPointerInstances[pad];
        if (g_pFEInput->m_InputLockDepth == 0)
        {
            if (pad != gFEControllerIndex)
            {
                controller->SetActiveSlide("waiting", true, false);
                continue;
            }

            if (mUnidentified001C > 0 || mUnidentified1138.mPointerInside[pad])
            {
                controller->SetActiveSlide("A", true, false);
            }
            else
            {
                controller->SetActiveSlide("cursor", true, false);
            }
        }

        unsigned char valid = 1;
        FEPointerEvent event;
        event.mIndex = pad;
        event.mPosition = GetPointerPosition(pad, &valid);
        event.mPressed
            = g_pFEInput->JustPressed((eFEINPUT_PAD)pad, 0x1E, true, 0);
        event.mReleased
            = g_pFEInput->JustReleased((eFEINPUT_PAD)pad, 0x1E, true, 0);

        for (int i = 0; i < 12; ++i)
        {
            mUnidentified0058[i].HandlePointerEvent(&event);
        }
        for (int i = 0; i < 12; ++i)
        {
            mUnidentified08C8[i].HandlePointerEvent(&event);
        }

        if (mUnidentified1138.UpdateBackButton(event, fDeltaT))
        {
            mUnidentified1278 = 3;
            SHNavigation* object = GetNavigationScene();
            if (object != 0)
            {
                object->HideButtons();
            }
            mPresentation->SetActiveSlide("out", true);
            mPresentation->Update(0.0f);
            return;
        }
    }
}

void TU80250754Scene::fn_80251328(int, void* context)
{
    unsigned int item = (unsigned int)context;

    if (item == 10)
    {
        FEAudio::PlayAnimAudioEvent(0xF0AFD586, 0, 0, 1);

        unsigned long long friendKey = 0;
        for (int i = 0; i < 12; ++i)
        {
            nlWcsToStr(mUnidentified0028[i], lbl_806DE80C, 4);
            friendKey += atoi(lbl_806DE80C) * pow(10.0, 11 - i);
        }

        int error = -1;
        if (g_pFriendManager->AddFriendKey(friendKey, &error))
        {
            SaveLoad::StartSave(true);
            mUnidentified1278 = 2;

            SHNavigation* object = GetNavigationScene();
            if (object != 0)
            {
                object->HideButtons();
            }

            for (int i = 0; i < 4; ++i)
            {
                gFEPointerInstances[i]->SetActiveSlide("waiting", true, false);
            }
            mPresentation->SetActiveSlide("out", true);
            mPresentation->Update(0.0f);
        }
        else
        {
            g_pFriendManager->SetOwnStatusInitial(false);

            if (GameSceneManager::Instance()->GetSceneType(GameSceneManager::Instance()->GetCurrentScene())
                != (SceneList)10)
            {
                FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push(
                    (SceneList)10, SCREEN_NOTHING, false);
                Function<FnVoidVoid> callback(
                    Bind<void>(MemFun(&TU80250754Scene::fn_80250718), this));
                popup->Create((ePopupMenu)error, callback);
                mUnidentified1274 = true;
            }

            FEAudio::EnableSounds(true);
            FEAudio::PlayAnimAudioEvent(0xD642865E, 0, 0, 1);
            FEAudio::EnableSounds(false);

            if (mUnidentified0024 != 0)
            {
                mUnidentified1240[0]->SetActiveSlide("DOWN", true, false);
                mUnidentified1240[mUnidentified0024]->SetActiveSlide(
                    "OFF", true, false);

                mUnidentified08C8[0].mDisabled = true;
                FEPointerEvent event;
                mUnidentified08C8[0].mPreviousEvents[0] = event;
                mUnidentified08C8[0].mPreviousEvents[1] = event;
                mUnidentified08C8[0].mPreviousEvents[2] = event;
                mUnidentified08C8[0].mPreviousEvents[3] = event;

                mUnidentified08C8[mUnidentified0024].mDisabled = false;
                mUnidentified0024 = 0;
            }
        }
    }
    else if (item == 11)
    {
        if (mUnidentified0028[mUnidentified0024][0] == 0
            && mUnidentified0024 > 0)
        {
            int nextItem = mUnidentified0024 - 1;
            if (nextItem >= 12)
            {
                nextItem = 11;
            }
            if (nextItem < 0)
            {
                nextItem = 0;
            }

            if (mUnidentified0024 != nextItem)
            {
                mUnidentified1240[nextItem]->SetActiveSlide(
                    "DOWN", true, false);
                mUnidentified1240[mUnidentified0024]->SetActiveSlide(
                    "OFF", true, false);

                mUnidentified08C8[nextItem].mDisabled = true;
                FEPointerEvent event;
                mUnidentified08C8[nextItem].mPreviousEvents[0] = event;
                mUnidentified08C8[nextItem].mPreviousEvents[1] = event;
                mUnidentified08C8[nextItem].mPreviousEvents[2] = event;
                mUnidentified08C8[nextItem].mPreviousEvents[3] = event;

                mUnidentified08C8[mUnidentified0024].mDisabled = false;
                mUnidentified0024 = nextItem;
            }
        }

        nlStrNCpy(
            mUnidentified0028[mUnidentified0024], (const unsigned short*)L"", 2);

        TLTextInstance* text = FEFinder<TLTextInstance, 3>::Find(
            mUnidentified1240[mUnidentified0024], nlStringLowerHash("off"),
            nlStringLowerHash("BOX"), nlStringLowerHash("NUMBER"), 0, 0, 0);
        if (text == 0)
        {
            text = &gDefaultTLTextInstance;
        }
        text->SetString(mUnidentified0028[mUnidentified0024]);

        text = FEFinder<TLTextInstance, 3>::Find(
            mUnidentified1240[mUnidentified0024], nlStringLowerHash("over"),
            nlStringLowerHash("BOX"), nlStringLowerHash("NUMBER"), 0, 0, 0);
        if (text == 0)
        {
            text = &gDefaultTLTextInstance;
        }
        text->SetString(mUnidentified0028[mUnidentified0024]);

        text = FEFinder<TLTextInstance, 3>::Find(
            mUnidentified1240[mUnidentified0024], nlStringLowerHash("down"),
            nlStringLowerHash("BOX"), nlStringLowerHash("NUMBER"), 0, 0, 0);
        if (text == 0)
        {
            text = &gDefaultTLTextInstance;
        }
        text->SetString(mUnidentified0028[mUnidentified0024]);
    }
    else
    {
        if (item == 9)
        {
            nlStrNCpy(mUnidentified0028[mUnidentified0024],
                (const unsigned short*)L"0", 2);
        }
        else
        {
            unsigned short character[2];
            nlSNPrintf(character, 2, (const unsigned short*)L"%d", item + 1);
            nlStrNCpy(mUnidentified0028[mUnidentified0024], character, 2);
        }

        TLTextInstance* text = FEFinder<TLTextInstance, 3>::Find(
            mUnidentified1240[mUnidentified0024], nlStringLowerHash("off"),
            nlStringLowerHash("BOX"), nlStringLowerHash("NUMBER"), 0, 0, 0);
        if (text == 0)
        {
            text = &gDefaultTLTextInstance;
        }
        text->SetString(mUnidentified0028[mUnidentified0024]);

        text = FEFinder<TLTextInstance, 3>::Find(
            mUnidentified1240[mUnidentified0024], nlStringLowerHash("over"),
            nlStringLowerHash("BOX"), nlStringLowerHash("NUMBER"), 0, 0, 0);
        if (text == 0)
        {
            text = &gDefaultTLTextInstance;
        }
        text->SetString(mUnidentified0028[mUnidentified0024]);

        text = FEFinder<TLTextInstance, 3>::Find(
            mUnidentified1240[mUnidentified0024], nlStringLowerHash("down"),
            nlStringLowerHash("BOX"), nlStringLowerHash("NUMBER"), 0, 0, 0);
        if (text == 0)
        {
            text = &gDefaultTLTextInstance;
        }
        text->SetString(mUnidentified0028[mUnidentified0024]);

        if (mUnidentified0024 < 11)
        {
            int nextItem = mUnidentified0024 + 1;
            if (nextItem >= 12)
            {
                nextItem = 11;
            }
            if (nextItem < 0)
            {
                nextItem = 0;
            }

            if (mUnidentified0024 != nextItem)
            {
                mUnidentified1240[nextItem]->SetActiveSlide(
                    "DOWN", true, false);
                mUnidentified1240[mUnidentified0024]->SetActiveSlide(
                    "OFF", true, false);

                mUnidentified08C8[nextItem].mDisabled = true;
                FEPointerEvent event;
                mUnidentified08C8[nextItem].mPreviousEvents[0] = event;
                mUnidentified08C8[nextItem].mPreviousEvents[1] = event;
                mUnidentified08C8[nextItem].mPreviousEvents[2] = event;
                mUnidentified08C8[nextItem].mPreviousEvents[3] = event;

                mUnidentified08C8[mUnidentified0024].mDisabled = false;
                mUnidentified0024 = nextItem;
            }
        }
    }

    fn_802505E8();

    switch (item)
    {
    case 0: FEAudio::PlayAnimAudioEvent(0xD95E4CC9, 0, 0, 1); break;
    case 1: FEAudio::PlayAnimAudioEvent(0xD95E4CCA, 0, 0, 1); break;
    case 2: FEAudio::PlayAnimAudioEvent(0xD95E4CCB, 0, 0, 1); break;
    case 3: FEAudio::PlayAnimAudioEvent(0xD95E4CCC, 0, 0, 1); break;
    case 4: FEAudio::PlayAnimAudioEvent(0xD95E4CCD, 0, 0, 1); break;
    case 5: FEAudio::PlayAnimAudioEvent(0xD95E4CCE, 0, 0, 1); break;
    case 6: FEAudio::PlayAnimAudioEvent(0xD95E4CCF, 0, 0, 1); break;
    case 7: FEAudio::PlayAnimAudioEvent(0xD95E4CD0, 0, 0, 1); break;
    case 8: FEAudio::PlayAnimAudioEvent(0xD95E4CD1, 0, 0, 1); break;
    case 9: FEAudio::PlayAnimAudioEvent(0xD95E4CC8, 0, 0, 1); break;
    case 11: FEAudio::PlayAnimAudioEvent(0xB4BD572B, 0, 0, 1); break;
    }
}
