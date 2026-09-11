// MSL math.h defines abs/labs; include it before stdlib.h.
#include <math.h>
#include "Game/SH/SHOnlineFriendCodeEntry.h"

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

SHOnlineFriendCodeEntry::SHOnlineFriendCodeEntry()
    : mHoverCount(0)
    , mButtonsInitialized(false)
    , mSelectedDigit(-1)
    , mKeypadButtons()
    , mDigitButtons()
    , mBackButton()
    , mPopupActive(false)
    , mState(0)
{
    const unsigned short* empty = (const unsigned short*)L"";

    for (int i = 0; i < 12; ++i)
    {
        mKeypadButtons[i].mContext = (void*)i;
    }

    for (int i = 0; i < 12; ++i)
    {
        mDigitButtons[i].mContext = (void*)i;
        nlStrNCpy(mDigits[i], empty, 2);
    }

    mBackButton.SetPopScene(false);
}

SHOnlineFriendCodeEntry::~SHOnlineFriendCodeEntry()
{
}

void SHOnlineFriendCodeEntry::InitializeButtons()
{
    FEPointerListener::Callback padSelect(
        Bind<void>(MemFun(&SHOnlineFriendCodeEntry::OnKeypadPointerPress), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback padOver(
        Bind<void>(MemFun(&SHOnlineFriendCodeEntry::OnKeypadPointerEnter), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback padOff(
        Bind<void>(MemFun(&SHOnlineFriendCodeEntry::OnKeypadPointerLeave), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback codeSelect(
        Bind<void>(MemFun(&SHOnlineFriendCodeEntry::OnDigitPointerPress), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback codeOver(
        Bind<void>(MemFun(&SHOnlineFriendCodeEntry::OnDigitPointerEnter), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback codeOff(
        Bind<void>(MemFun(&SHOnlineFriendCodeEntry::OnDigitPointerLeave), this, Placeholder<0>(), Placeholder<1>()));

    for (int i = 0; i < 12; ++i)
    {
        float scale = 0.8f;
        TLInstance* positionInstance =
            FEFinder<TLInstance, 5>::_Find<TLSlide>(
                mPresentation->m_currentSlide, nlStringLowerHash("Layer"),
                nlStringLowerHash("Group"), nlStringLowerHash("PAD"), 0, 0, 0);
        if (positionInstance == 0)
        {
            positionInstance = &UnidentifiedTLGroupDefault::sInstance;
        }

        feVector3 position = positionInstance->GetAssetPosition();
        if (i == 11)
        {
            scale = 0.7f;
        }
        mKeypadButtons[i].SetInstanceBounds(
            mKeypadInstances[i], true, position.f.x, position.f.y, scale, scale);
        mKeypadButtons[i].SetPointerPressCallback(padSelect);
        mKeypadButtons[i].SetPointerEnterCallback(padOver);
        mKeypadButtons[i].SetPointerLeaveCallback(padOff);
    }

    for (int i = 0; i < 12; ++i)
    {
        TLInstance* positionInstance =
            FEFinder<TLInstance, 5>::_Find<TLSlide>(
                mPresentation->m_currentSlide, nlStringLowerHash("Layer"),
                nlStringLowerHash("Group"), nlStringLowerHash("CODE"), 0, 0, 0);
        if (positionInstance == 0)
        {
            positionInstance = &UnidentifiedTLGroupDefault::sInstance;
        }

        feVector3 position = positionInstance->GetAssetPosition();
        mDigitButtons[i].SetInstanceBounds(
            mDigitInstances[i], true, position.f.x, position.f.y, 0.8f, 0.8f);
        mDigitButtons[i].SetPointerPressCallback(codeSelect);
        mDigitButtons[i].SetPointerEnterCallback(codeOver);
        mDigitButtons[i].SetPointerLeaveCallback(codeOff);
    }
}

void SHOnlineFriendCodeEntry::OnKeypadPointerEnter(int index, void* context)
{
    unsigned int item = (unsigned int)context;
    ++mHoverCount;
    mKeypadInstances[item]->SetActiveSlide("over", true, false);
    mKeypadButtons[item].SetPointerState(1, index);
    FEAudio::PlayAnimAudioEvent(0x0E2B7F90, 0, 0, 1);
}

void SHOnlineFriendCodeEntry::OnKeypadPointerLeave(int index, void* context)
{
    unsigned int item = (unsigned int)context;
    --mHoverCount;
    mKeypadInstances[item]->SetActiveSlide("off", true, false);
    mKeypadButtons[item].SetPointerState(0, index);
}

void SHOnlineFriendCodeEntry::OnDigitPointerPress(int, void* context)
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

    if (mSelectedDigit != item)
    {
        mDigitInstances[item]->SetActiveSlide("DOWN", true, false);
        mDigitInstances[mSelectedDigit]->SetActiveSlide("OFF", true, false);

        mDigitButtons[item].mDisabled = true;
        FEPointerEvent event;
        mDigitButtons[item].mPreviousEvents[0] = event;
        mDigitButtons[item].mPreviousEvents[1] = event;
        mDigitButtons[item].mPreviousEvents[2] = event;
        mDigitButtons[item].mPreviousEvents[3] = event;

        mDigitButtons[mSelectedDigit].mDisabled = false;
        mSelectedDigit = item;
    }

    --mHoverCount;
}

void SHOnlineFriendCodeEntry::OnDigitPointerEnter(int index, void* context)
{
    unsigned int item = (unsigned int)context;
    if (item != mSelectedDigit)
    {
        ++mHoverCount;
        mDigitInstances[item]->SetActiveSlide("over", true, false);
        mDigitButtons[item].SetPointerState(1, index);
        FEAudio::PlayAnimAudioEvent(0xFFC8A55D, 0, 0, 1);
    }
}

void SHOnlineFriendCodeEntry::OnDigitPointerLeave(int index, void* context)
{
    unsigned int item = (unsigned int)context;
    if (item != mSelectedDigit)
    {
        --mHoverCount;
        mDigitInstances[item]->SetActiveSlide("off", true, false);
        mDigitButtons[item].SetPointerState(0, index);
    }
}

void SHOnlineFriendCodeEntry::RestoreFriendCodeInput()
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

            if (mSelectedDigit != item)
            {
                mDigitInstances[item]->SetActiveSlide("DOWN", true, false);
                mDigitInstances[mSelectedDigit]->SetActiveSlide(
                    "OFF", true, false);

                mDigitButtons[item].mDisabled = true;
                FEPointerEvent event;
                mDigitButtons[item].mPreviousEvents[0] = event;
                mDigitButtons[item].mPreviousEvents[1] = event;
                mDigitButtons[item].mPreviousEvents[2] = event;
                mDigitButtons[item].mPreviousEvents[3] = event;

                mDigitButtons[mSelectedDigit].mDisabled = false;
                mSelectedDigit = item;
            }
            foundEmpty = true;
        }

        character[0] = friendCode[i];
        int item = i;
        if (item < 0)
        {
            item = mSelectedDigit;
        }
        nlStrNCpy(mDigits[item], character, 2);

        TLTextInstance* text = FEFinder<TLTextInstance, 3>::Find(
            mDigitInstances[item], nlStringLowerHash("off"),
            nlStringLowerHash("BOX"), nlStringLowerHash("NUMBER"), 0, 0, 0);
        if (text == 0)
        {
            text = &UnidentifiedTLTextDefault::sInstance;
        }
        text->SetString(mDigits[item]);

        text = FEFinder<TLTextInstance, 3>::Find(
            mDigitInstances[item], nlStringLowerHash("over"),
            nlStringLowerHash("BOX"), nlStringLowerHash("NUMBER"), 0, 0, 0);
        if (text == 0)
        {
            text = &UnidentifiedTLTextDefault::sInstance;
        }
        text->SetString(mDigits[item]);

        text = FEFinder<TLTextInstance, 3>::Find(
            mDigitInstances[item], nlStringLowerHash("down"),
            nlStringLowerHash("BOX"), nlStringLowerHash("NUMBER"), 0, 0, 0);
        if (text == 0)
        {
            text = &UnidentifiedTLTextDefault::sInstance;
        }
        text->SetString(mDigits[item]);
    }

    if (!foundEmpty && mSelectedDigit != 11)
    {
        mDigitInstances[11]->SetActiveSlide("DOWN", true, false);
        mDigitInstances[mSelectedDigit]->SetActiveSlide("OFF", true, false);

        mDigitButtons[11].mDisabled = true;
        FEPointerEvent event;
        mDigitButtons[11].mPreviousEvents[0] = event;
        mDigitButtons[11].mPreviousEvents[1] = event;
        mDigitButtons[11].mPreviousEvents[2] = event;
        mDigitButtons[11].mPreviousEvents[3] = event;

        mDigitButtons[mSelectedDigit].mDisabled = false;
        mSelectedDigit = 11;
    }

    memset(friendCode, 0, sizeof(g_pFriendManager->mFriendCodeInput));
}

void SHOnlineFriendCodeEntry::UpdateConfirmButton()
{
    mKeypadInstances[10]->m_bVisible = true;
    mKeypadButtons[10].mDisabled = false;
    mOutConfirmButtonInstance->m_bVisible = true;

    bool valid = true;
    for (int i = 0; i < 12; ++i)
    {
        if (mDigits[i][0] == 0)
        {
            mOutConfirmButtonInstance->m_bVisible = false;
            mKeypadInstances[10]->m_bVisible = false;
            mKeypadButtons[10].mDisabled = true;

            FEPointerEvent event;
            mKeypadButtons[10].mPreviousEvents[0] = event;
            mKeypadButtons[10].mPreviousEvents[1] = event;
            mKeypadButtons[10].mPreviousEvents[2] = event;
            mKeypadButtons[10].mPreviousEvents[3] = event;
            valid = false;
        }
    }

    if (valid)
    {
        FEAudio::PlayAnimAudioEvent(0xCC2C93F1, 0, 0, 1);
    }
}

void SHOnlineFriendCodeEntry::OnAddFriendErrorDismissed()
{
    g_pFriendManager->SetOwnStatusInitial(true);
    mPopupActive = false;
}

void SHOnlineFriendCodeEntry::SceneCreated()
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
            component = &UnidentifiedTLComponentDefault::sInstance;
        }
        mKeypadInstances[i] = component;
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
            component = &UnidentifiedTLComponentDefault::sInstance;
        }
        mDigitInstances[i] = component;
    }

    mOutConfirmButtonInstance = FEFinder<TLComponentInstance, 5>::Find(mPresentation,
        nlStringLowerHash("out"), nlStringLowerHash("Layer"),
        nlStringLowerHash("Group"), nlStringLowerHash("PAD"),
        nlStringLowerHash("button_ok"), 0);

    for (int i = 0; i < 12; ++i)
    {
        int item = i;
        if (item < 0)
        {
            item = mSelectedDigit;
        }
        nlStrNCpy(mDigits[item], (const unsigned short*)L"", 2);

        TLTextInstance* text = FEFinder<TLTextInstance, 3>::Find(
            mDigitInstances[item], nlStringLowerHash("off"),
            nlStringLowerHash("BOX"), nlStringLowerHash("NUMBER"), 0, 0, 0);
        if (text == 0)
        {
            text = &UnidentifiedTLTextDefault::sInstance;
        }
        text->SetString(mDigits[item]);

        text = FEFinder<TLTextInstance, 3>::Find(
            mDigitInstances[item], nlStringLowerHash("over"),
            nlStringLowerHash("BOX"), nlStringLowerHash("NUMBER"), 0, 0, 0);
        if (text == 0)
        {
            text = &UnidentifiedTLTextDefault::sInstance;
        }
        text->SetString(mDigits[item]);

        text = FEFinder<TLTextInstance, 3>::Find(
            mDigitInstances[item], nlStringLowerHash("down"),
            nlStringLowerHash("BOX"), nlStringLowerHash("NUMBER"), 0, 0, 0);
        if (text == 0)
        {
            text = &UnidentifiedTLTextDefault::sInstance;
        }
        text->SetString(mDigits[item]);
    }

    RestoreFriendCodeInput();
    UpdateConfirmButton();

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
    mBackButton.SetButtonInstance(screen);

    FEAudio::PlayAnimAudioEvent(0xBB142B94, 0, 0, 1);
}

void SHOnlineFriendCodeEntry::Update(float fDeltaT)
{
    BaseSceneHandler::Update(fDeltaT);

    if (SaveEnabled && InOperation)
    {
        return;
    }

    if (mPopupActive && !g_pFEInput->HasInputLock(this))
    {
        return;
    }

    int state = mState;
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
            mState = 1;
            InitializeButtons();
            mButtonsInitialized = true;
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
        g_pFriendManager->mReturnScene = SCENE_ONLINE_FRIEND_CODE_ENTRY;
        g_pFriendManager->mPreviousRankedMode = 0;
        unsigned short* friendCode =
            g_pFriendManager->mFriendCodeInput;
        for (int i = 0; i < 12; ++i)
        {
            friendCode[i] = mDigits[i][0];
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

            if (mHoverCount > 0 || mBackButton.mPointerInside[pad])
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
            mKeypadButtons[i].HandlePointerEvent(&event);
        }
        for (int i = 0; i < 12; ++i)
        {
            mDigitButtons[i].HandlePointerEvent(&event);
        }

        if (mBackButton.UpdateBackButton(event, fDeltaT))
        {
            mState = 3;
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

void SHOnlineFriendCodeEntry::OnKeypadPointerPress(int, void* context)
{
    unsigned int item = (unsigned int)context;

    if (item == 10)
    {
        FEAudio::PlayAnimAudioEvent(0xF0AFD586, 0, 0, 1);

        static char buffer[4] = "0";
        char* digit = buffer;
        unsigned long long friendKey = 0;
        for (int i = 0; i < 12; ++i)
        {
            nlWcsToStr(mDigits[i], digit, 4);
            friendKey += atoi(digit) * pow(10.0, 11 - i);
        }

        int error = -1;
        if (g_pFriendManager->AddFriendKey(friendKey, &error))
        {
            SaveLoad::StartSave(true);
            mState = 2;

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
                popup->Create((ePopupMenu)error,
                    Function<FnVoidVoid>(Bind<void>(MemFun(&SHOnlineFriendCodeEntry::OnAddFriendErrorDismissed), this)));
                mPopupActive = true;
            }

            FEAudio::EnableSounds(true);
            FEAudio::PlayAnimAudioEvent(0xD642865E, 0, 0, 1);
            FEAudio::EnableSounds(false);

            if (mSelectedDigit != 0)
            {
                mDigitInstances[0]->SetActiveSlide("DOWN", true, false);
                mDigitInstances[mSelectedDigit]->SetActiveSlide(
                    "OFF", true, false);

                mDigitButtons[0].mDisabled = true;
                FEPointerEvent event;
                mDigitButtons[0].mPreviousEvents[0] = event;
                mDigitButtons[0].mPreviousEvents[1] = event;
                mDigitButtons[0].mPreviousEvents[2] = event;
                mDigitButtons[0].mPreviousEvents[3] = event;

                mDigitButtons[mSelectedDigit].mDisabled = false;
                mSelectedDigit = 0;
            }
        }
    }
    else if (item == 11)
    {
        if (mDigits[mSelectedDigit][0] == 0
            && mSelectedDigit > 0)
        {
            int nextItem = mSelectedDigit - 1;
            if (nextItem >= 12)
            {
                nextItem = 11;
            }
            if (nextItem < 0)
            {
                nextItem = 0;
            }

            if (mSelectedDigit != nextItem)
            {
                mDigitInstances[nextItem]->SetActiveSlide(
                    "DOWN", true, false);
                mDigitInstances[mSelectedDigit]->SetActiveSlide(
                    "OFF", true, false);

                mDigitButtons[nextItem].mDisabled = true;
                FEPointerEvent event;
                mDigitButtons[nextItem].mPreviousEvents[0] = event;
                mDigitButtons[nextItem].mPreviousEvents[1] = event;
                mDigitButtons[nextItem].mPreviousEvents[2] = event;
                mDigitButtons[nextItem].mPreviousEvents[3] = event;

                mDigitButtons[mSelectedDigit].mDisabled = false;
                mSelectedDigit = nextItem;
            }
        }

        nlStrNCpy(
            mDigits[mSelectedDigit], (const unsigned short*)L"", 2);

        TLTextInstance* text = FEFinder<TLTextInstance, 3>::Find(
            mDigitInstances[mSelectedDigit], nlStringLowerHash("off"),
            nlStringLowerHash("BOX"), nlStringLowerHash("NUMBER"), 0, 0, 0);
        if (text == 0)
        {
            text = &UnidentifiedTLTextDefault::sInstance;
        }
        text->SetString(mDigits[mSelectedDigit]);

        text = FEFinder<TLTextInstance, 3>::Find(
            mDigitInstances[mSelectedDigit], nlStringLowerHash("over"),
            nlStringLowerHash("BOX"), nlStringLowerHash("NUMBER"), 0, 0, 0);
        if (text == 0)
        {
            text = &UnidentifiedTLTextDefault::sInstance;
        }
        text->SetString(mDigits[mSelectedDigit]);

        text = FEFinder<TLTextInstance, 3>::Find(
            mDigitInstances[mSelectedDigit], nlStringLowerHash("down"),
            nlStringLowerHash("BOX"), nlStringLowerHash("NUMBER"), 0, 0, 0);
        if (text == 0)
        {
            text = &UnidentifiedTLTextDefault::sInstance;
        }
        text->SetString(mDigits[mSelectedDigit]);
    }
    else
    {
        if (item == 9)
        {
            nlStrNCpy(mDigits[mSelectedDigit],
                (const unsigned short*)L"0", 2);
        }
        else
        {
            unsigned short character[2];
            nlSNPrintf(character, 2, (const unsigned short*)L"%d", item + 1);
            nlStrNCpy(mDigits[mSelectedDigit], character, 2);
        }

        TLTextInstance* text = FEFinder<TLTextInstance, 3>::Find(
            mDigitInstances[mSelectedDigit], nlStringLowerHash("off"),
            nlStringLowerHash("BOX"), nlStringLowerHash("NUMBER"), 0, 0, 0);
        if (text == 0)
        {
            text = &UnidentifiedTLTextDefault::sInstance;
        }
        text->SetString(mDigits[mSelectedDigit]);

        text = FEFinder<TLTextInstance, 3>::Find(
            mDigitInstances[mSelectedDigit], nlStringLowerHash("over"),
            nlStringLowerHash("BOX"), nlStringLowerHash("NUMBER"), 0, 0, 0);
        if (text == 0)
        {
            text = &UnidentifiedTLTextDefault::sInstance;
        }
        text->SetString(mDigits[mSelectedDigit]);

        text = FEFinder<TLTextInstance, 3>::Find(
            mDigitInstances[mSelectedDigit], nlStringLowerHash("down"),
            nlStringLowerHash("BOX"), nlStringLowerHash("NUMBER"), 0, 0, 0);
        if (text == 0)
        {
            text = &UnidentifiedTLTextDefault::sInstance;
        }
        text->SetString(mDigits[mSelectedDigit]);

        if (mSelectedDigit < 11)
        {
            int nextItem = mSelectedDigit + 1;
            if (nextItem >= 12)
            {
                nextItem = 11;
            }
            if (nextItem < 0)
            {
                nextItem = 0;
            }

            if (mSelectedDigit != nextItem)
            {
                mDigitInstances[nextItem]->SetActiveSlide(
                    "DOWN", true, false);
                mDigitInstances[mSelectedDigit]->SetActiveSlide(
                    "OFF", true, false);

                mDigitButtons[nextItem].mDisabled = true;
                FEPointerEvent event;
                mDigitButtons[nextItem].mPreviousEvents[0] = event;
                mDigitButtons[nextItem].mPreviousEvents[1] = event;
                mDigitButtons[nextItem].mPreviousEvents[2] = event;
                mDigitButtons[nextItem].mPreviousEvents[3] = event;

                mDigitButtons[mSelectedDigit].mDisabled = false;
                mSelectedDigit = nextItem;
            }
        }
    }

    UpdateConfirmButton();

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
