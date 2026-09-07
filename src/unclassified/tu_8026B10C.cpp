#include "unclassified/tu_8026B10C.h"
#include "Game/Sys/debug.h"

#include "Game/BaseGameSceneManager.h"
#include "Game/FE/feInput.h"
#include "Game/FE/fePopupMenu.h"
#include "Game/FE/tlInstance.h"
#include "Game/FE/feFinder.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlTextInstance.h"
#include "Game/FE/tlSlide.h"
#include "Game/GameInfo.h"
#include "Game/NetworkDraft.h"
#include "Game/NetworkSession.h"
#include "Game/tu_801360A4.h"
#include "NL/nlBind.h"
#include "NL/nlLocalizationLookup.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"
#include "unclassified/tu_802196B0.h"
#include "unclassified/tu_80252180.h"

extern "C" void fn_80306208(UnidentifiedTimer_8030616C* timer, bool enabled);
extern "C" void fn_80306224(UnidentifiedTimer_8030616C* timer, float dt);
extern "C" void fn_801CC9B0(TU80219248Component* component, TLComponentInstance* instance, int value);
extern "C" bool fn_8025BDA0();
extern "C" void fn_801CBCA0(unsigned long hash, int value0, int value1, int value2);
extern int fn_802AA91C(
    unsigned short* buffer, unsigned long size, const unsigned short* format, ...);
extern TLComponentInstance* lbl_80578450[4];
extern TLComponentInstance lbl_80580030;
const char* lbl_806DED40[2] = { "home_group", "away_group" };
int lbl_806DED48 = 30;
extern int lbl_806DE668[2];
extern unsigned int lbl_806E18B0;
extern BaseGameSceneManager* lbl_806E1838;

TU8026B10CScene::TU8026B10CScene()
    : mUnidentified01C(false)
    , mUnidentified434(false)
    , mUnidentified435(false)
    , mUnidentified436(false)
    , mUnidentified5A0((void*)2)
    , mUnidentified65C(0)
    , mUnidentified6B4(0)
    , mUnidentified750(1.0f,
          Function<UnidentifiedTimer_8030616C*>(
              Bind<void>(MemFun(&TU8026B10CScene::fn_8026C308), this, Placeholder<0>())))
    , mUnidentified76C(false)
    , mUnidentified770(lbl_806DED48)
{
    for (int i = 0; i < 4; ++i)
    {
        mUnidentified664[i] = -1;
    }

    mUnidentified438[0].mContext = (void*)0;
    mUnidentified438[0].mSpeakerEnabled = false;
    mUnidentified438[1].mContext = (void*)1;
    mUnidentified438[1].mSpeakerEnabled = false;
    mUnidentified5A0.mSpeakerEnabled = false;

    for (int i = 0; i < 4; ++i)
    {
        mUnidentified6A4[i] = 0;
        for (int j = 0; j < 4; ++j)
        {
            mUnidentified674[i].c[j] = 0xFF;
        }
    }

    for (int i = 0; i < 3; ++i)
    {
        mUnidentified6B8[0][i] = -1;
        mUnidentified6B8[1][i] = -1;
    }
    fn_80306208(&mUnidentified750, true);
}

TU8026B10CScene::~TU8026B10CScene()
{
    TU80252180Scene* object = fn_80253E18();
    if (object != 0)
    {
        object->mUnidentified054->m_bVisible = false;
    }
}

void TU8026B10CScene::SceneCreated()
{
    NetworkDraft::Instance();
    mUnidentified654[0] = FEFinder<TLComponentInstance, 4>::Find(
        mPresentation->GetActiveSlide(), InlineHasher("Layer"), InlineHasher("home"));
    if (mUnidentified654[0] == 0)
    {
        mUnidentified654[0] = &lbl_80580030;
    }
    mUnidentified654[1] = FEFinder<TLComponentInstance, 4>::Find(
        mPresentation->GetActiveSlide(), InlineHasher("Layer"), InlineHasher("away"));
    if (mUnidentified654[1] == 0)
    {
        mUnidentified654[1] = &lbl_80580030;
    }
    mUnidentified654[0]->SetActiveSlide("controllers", true, false);
    mUnidentified654[1]->SetActiveSlide("controllers", true, false);

    TU80252180Scene* object = fn_80253E18();
    if (object != 0)
    {
        fn_802534BC(object, 0x20, true);
        mUnidentified65C = fn_80253D70(object, 0x20);
    }
    mUnidentified65C->m_bVisible = false;

    int machine = 0;
    bool guest = false;
    int groups = 0;
    for (int i = 0; i < 4; ++i)
    {
        lbl_80578450[i]->SetActiveSlide("waiting", true, false);
        char controller[16];
        char friendName[16];
        nlSNPrintf(controller, sizeof(controller), "controller%d", i);
        nlSNPrintf(friendName, sizeof(friendName), "friend_%d", i);

        TLComponentInstance* home = FEFinder<TLComponentInstance, 4>::Find(
            mUnidentified654[0], nlStringLowerHash("controllers"),
            nlStringLowerHash(lbl_806DED40[0]), nlStringLowerHash(controller), 0, 0, 0);
        TLComponentInstance* homeOver = FEFinder<TLComponentInstance, 4>::Find(
            mUnidentified654[0], nlStringLowerHash("over"),
            nlStringLowerHash(lbl_806DED40[0]), nlStringLowerHash(controller), 0, 0, 0);
        TLComponentInstance* away = FEFinder<TLComponentInstance, 4>::Find(
            mUnidentified654[1], nlStringLowerHash("controllers"),
            nlStringLowerHash(lbl_806DED40[1]), nlStringLowerHash(controller), 0, 0, 0);
        TLComponentInstance* awayOver = FEFinder<TLComponentInstance, 4>::Find(
            mUnidentified654[1], nlStringLowerHash("over"),
            nlStringLowerHash(lbl_806DED40[1]), nlStringLowerHash(controller), 0, 0, 0);
        TLTextInstance* text = FEFinder<TLTextInstance, 3>::Find(
            mPresentation->GetActiveSlide(), InlineHasher("Layer"), InlineHasher(friendName));

        FEFinder<TLTextInstance, 3>::Find(home->GetActiveSlide(), InlineHasher("Text"));
        FEFinder<TLTextInstance, 3>::Find(homeOver->GetActiveSlide(), InlineHasher("Text"));
        FEFinder<TLTextInstance, 3>::Find(away->GetActiveSlide(), InlineHasher("Text"));
        FEFinder<TLTextInstance, 3>::Find(awayOver->GetActiveSlide(), InlineHasher("Text"));

        if (machine < mUnidentified020.mMachineCount)
        {
            if (!guest)
            {
                UnidentifiedDraftEntry entry = mUnidentified020.mEntries[machine];
                nlStrNCpy(mUnidentified6D0[i], entry.mName, 14);
                mUnidentified684[i].mUnidentified00 = machine;
                mUnidentified684[i].mUnidentified04 = false;
                if (entry.mUnidentified7F != 0)
                {
                    ++groups;
                    guest = true;
                    if (groups == 1)
                    {
                        nlColourSet(mUnidentified674[i], 237, 0, 12, 255);
                    }
                    else
                    {
                        nlColourSet(mUnidentified674[i], 34, 98, 171, 255);
                    }
                    text->SetAssetColour(mUnidentified674[i]);
                }
                else
                {
                    guest = false;
                    ++machine;
                }
            }
            else
            {
                nlStrNCpy(mUnidentified6D0[i], LookupLocString("ONLINE_CONTROLLERS_GUEST"), 14);
                guest = false;
                mUnidentified684[i].mUnidentified00 = machine;
                ++machine;
                mUnidentified684[i].mUnidentified04 = true;
                if (groups == 1)
                {
                    nlColourSet(mUnidentified674[i], 237, 0, 12, 255);
                }
                else
                {
                    nlColourSet(mUnidentified674[i], 34, 98, 171, 255);
                }
                text->SetAssetColour(mUnidentified674[i]);
            }
            text->SetString(mUnidentified6D0[i]);
            ++mUnidentified6B4;
        }
        else
        {
            text->m_bVisible = false;
        }
        home->m_bVisible = false;
        homeOver->m_bVisible = false;
        away->m_bVisible = false;
        awayOver->m_bVisible = false;
    }

    mUnidentified660 = FEFinder<TLTextInstance, 3>::Find(
        mPresentation->GetActiveSlide(), InlineHasher("Layer"), InlineHasher("Text"));
    if (mUnidentified660 == 0)
    {
        mUnidentified660 = &UnidentifiedFallbackTextInstance;
    }
    TLTextInstance* timer = static_cast<TLTextInstance*>(fn_80253E18()->mUnidentified054);
    fn_80253E18()->mUnidentified054->m_bVisible = true;
    fn_802AA91C(mUnidentified740, 8, (const unsigned short*)L"%d", mUnidentified770);
    timer->SetString(mUnidentified740);
    memset(lbl_806E1194->mUnidentified018, 0, sizeof(lbl_806E1194->mUnidentified018));
    fn_801CBCA0(0x7EF9225A, 0, 0, 1);
}

void TU8026B10CScene::fn_8026C308(UnidentifiedTimer_8030616C* timer)
{
    mUnidentified76C = true;
    if (mUnidentified770 > 0)
    {
        --mUnidentified770;
        if (mUnidentified770 <= 5 && mUnidentified770 > 0)
        {
            if (mUnidentified770 == 1)
            {
                fn_801CBCA0(0x09AA8790, 0, 0, 1);
            }
            else
            {
                fn_801CBCA0(0xFF48403F, 0, 0, 1);
            }
        }
    }
}

void TU8026B10CScene::Update(float fDeltaT)
{
    BaseSceneHandler::Update(fDeltaT);
    if (mUnidentified01C && !g_pFEInput->HasInputLock(this))
    {
        return;
    }
    fn_80306224(&mUnidentified750, fDeltaT);
    if (mUnidentified76C)
    {
        TLTextInstance* timer = static_cast<TLTextInstance*>(fn_80253E18()->mUnidentified054);
        fn_80253E18()->mUnidentified054->m_bVisible = true;
        fn_802AA91C(mUnidentified740, 8, (const unsigned short*)L"%d", mUnidentified770);
        timer->SetString(mUnidentified740);
        mUnidentified76C = false;
    }

    if (mUnidentified435)
    {
        TLSlide* slide = mUnidentified65C->GetActiveSlide();
        if (slide->GetCurrentTime() >= slide->m_start + slide->m_duration)
        {
            fn_801CC9B0(&mUnidentified5A0, mUnidentified65C, 0);
            mUnidentified5A0.mDisabled = false;
            mUnidentified435 = false;
        }
    }

    if (!mUnidentified434)
    {
        TLSlide* slide = mPresentation->GetActiveSlide();
        if (slide->GetCurrentTime() < slide->m_start + slide->m_duration)
        {
            return;
        }
        fn_8026C9D4();
        fn_8026DEC0();
        mUnidentified434 = true;
        for (int pad = 0; pad < 4; ++pad)
        {
            TLComponentInstance* controller = lbl_80578450[pad];
            int index = fn_8026E1AC(pad);
            if (index == -1)
            {
                controller->SetActiveSlide("waiting", true, false);
            }
            else if (mUnidentified664[index] == -1)
            {
                controller->SetActiveSlide("holding", true, false);
            }
            else
            {
                controller->SetActiveSlide("cursor", true, false);
            }
        }
    }

    UnidentifiedMachineRoster* roster = g_pNetworkSessionBase->GetMachineRoster();
    bool disconnected = false;
    for (int i = 0; i < roster->GetMachineCount(); ++i)
    {
        if (roster->GetMachineAid(i) == 0)
        {
            disconnected = true;
            break;
        }
    }
    if (disconnected)
    {
        g_pNetworkSession->fn_801216F0()->CloseConnectionsAndReset();
        TU80252180Scene* object = fn_80253E18();
        if (object != 0)
        {
            object->mUnidentified054->m_bVisible = false;
        }
        if (lbl_806E1838->GetSceneType(lbl_806E1838->GetCurrentScene()) != (SceneList)10)
        {
            FEPopupMenu* popup = static_cast<FEPopupMenu*>(
                lbl_806E1838->Push((SceneList)10, SCREEN_NOTHING, false));
            popup->Create((ePopupMenu)0x60,
                Function<FnVoidVoid>(Bind<void>(MemFun(&TU8026B10CScene::fn_8026EC64), this)));
            mUnidentified01C = true;
        }
        return;
    }

    if (mUnidentified770 == 0 && mUnidentified020.mMachineIndex == 0 && !mUnidentified436)
    {
        for (int i = 0; i < mUnidentified6B4; ++i)
        {
            if (mUnidentified664[i] == -1)
            {
                int home = 0;
                for (int j = 0; j < 4; ++j)
                {
                    if (mUnidentified664[j] == 0)
                    {
                        ++home;
                    }
                }
                int away = 0;
                for (int j = 0; j < 4; ++j)
                {
                    if (mUnidentified664[j] == 1)
                    {
                        ++away;
                    }
                }
                mUnidentified664[i] = home > away ? 1 : 0;
            }
        }
        fn_8026DB5C(0, (void*)2);
        return;
    }

    for (int pad = 0; pad < 4; ++pad)
    {
        TLComponentInstance* controller = lbl_80578450[pad];
        int index = fn_8026E1AC(pad);
        if (index == -1)
        {
            controller->SetActiveSlide("waiting", true, false);
            continue;
        }
        u8 valid = 1;
        TU80300104Event event;
        event.mIndex = pad;
        event.mPosition = fn_802197FC(pad, &valid);
        event.mFlag0 = g_pFEInput->JustPressed((eFEINPUT_PAD)pad, 0x1E, true, 0);
        mUnidentified5A0.fn_80219608(&event);
        mUnidentified438[0].fn_80219608(&event);
        mUnidentified438[1].fn_80219608(&event);
        if (mUnidentified436)
        {
            return;
        }
        if (mUnidentified664[index] != -1 && !g_pFEInput->IsConnected((eFEINPUT_PAD)pad))
        {
            fn_8026E1AC(pad);
            mUnidentified6A4[pad] = 0;
            NetworkMessageType25_8050B778 message;
            message.mUnidentified08 = mUnidentified020.mMachineIndex;
            message.mUnidentified09 = -1;
            message.mUnidentified0B = 0;
            if (fn_8025BDA0())
            {
                message.mUnidentified0A = pad == lbl_806DE668[1];
            }
            else
            {
                message.mUnidentified0A = 0;
            }
            g_pNetworkSession->SendSidesChangedToHost(&message);
        }
        if (mUnidentified664[index] == -1)
        {
            controller->SetActiveSlide("holding", true, false);
        }
        else
        {
            controller->SetActiveSlide("cursor", true, false);
        }
    }
}

void TU8026B10CScene::fn_8026C9D4()
{
    mUnidentified438[0].fn_80300D74(mUnidentified654[0], true, 0.0f, 0.0f, 1.0f, 1.0f);
    mUnidentified438[1].fn_80300D74(mUnidentified654[1], true, 0.0f, 0.0f, 1.0f, 1.0f);

    TU80300104Base::Callback callback(
        Bind<void>(MemFun(&TU8026B10CScene::fn_8026D4D4), this, Placeholder<0>(), Placeholder<1>()));
    mUnidentified438[0].fn_803007C0(callback);
    mUnidentified438[1].fn_803007C0(callback);
    callback = TU80300104Base::Callback(
        Bind<void>(MemFun(&TU8026B10CScene::fn_8026D614), this, Placeholder<0>(), Placeholder<1>()));
    mUnidentified438[0].fn_80300864(callback);
    mUnidentified438[1].fn_80300864(callback);
    callback = TU80300104Base::Callback(
        Bind<void>(MemFun(&TU8026B10CScene::fn_8026D6E0), this, Placeholder<0>(), Placeholder<1>()));
    mUnidentified438[0].fn_80300908(callback);
    mUnidentified438[1].fn_80300908(callback);

    TU80300104Base::Callback callback2(
        Bind<void>(MemFun(&TU8026B10CScene::fn_8026D85C), this, Placeholder<0>(), Placeholder<1>()));
    mUnidentified438[0].fn_803009AC(callback2);
    mUnidentified438[1].fn_803009AC(callback2);

    callback = TU80300104Base::Callback(
        Bind<void>(MemFun(&TU8026B10CScene::fn_8026D984), this, Placeholder<0>(), Placeholder<1>()));
    mUnidentified5A0.fn_803007C0(callback);
    callback = TU80300104Base::Callback(
        Bind<void>(MemFun(&TU8026B10CScene::fn_8026DAE0), this, Placeholder<0>(), Placeholder<1>()));
    mUnidentified5A0.fn_80300864(callback);
    callback = TU80300104Base::Callback(
        Bind<void>(MemFun(&TU8026B10CScene::fn_8026DA2C), this, Placeholder<0>(), Placeholder<1>()));
    mUnidentified5A0.fn_80300908(callback);
    callback2 = TU80300104Base::Callback(
        Bind<void>(MemFun(&TU8026B10CScene::fn_8026DB5C), this, Placeholder<0>(), Placeholder<1>()));
    mUnidentified5A0.fn_803009AC(callback2);
    mUnidentified5A0.fn_80206B54();
}

void TU8026B10CScene::fn_8026D4D4(int index, void* context)
{
    int value = mUnidentified664[fn_8026E1AC(index)];
    if (value != -1 && context != (void*)value)
    {
        return;
    }
    if (value == -1)
    {
        int count = 0;
        fn_8026E1AC(index);
        for (int i = 0; i < mUnidentified6B4; ++i)
        {
            if ((int)context == mUnidentified664[i])
            {
                ++count;
            }
        }
        if (count >= mUnidentified6B4 - 1)
        {
            return;
        }
    }
    if (!mUnidentified438[(int)context].fn_802192FC(1, index))
    {
        mUnidentified654[(int)context]->SetActiveSlide("over", true, false);
        fn_801CBCA0(0xAA73EF33, 0, 0, 1);
    }
    mUnidentified438[(int)context].mValues[index] = 1;
    ++mUnidentified6A4[index];
    mUnidentified438[(int)context].fn_802195B4(index);
    fn_801CBCA0(0x19E7B6AE, 0, 0, 1);
}

void TU8026B10CScene::fn_8026D614(int index, void* context)
{
    int value = mUnidentified664[fn_8026E1AC(index)];
    if (value != -1 && context != (void*)value)
    {
        return;
    }
    if (!mUnidentified438[(int)context].fn_802192FC(1, index))
    {
        mUnidentified654[(int)context]->SetActiveSlide("controllers", true, false);
    }
    mUnidentified438[(int)context].mValues[index] = 0;
    --mUnidentified6A4[index];
}

void TU8026B10CScene::fn_8026D6E0(int index, void* context)
{
    int slot = fn_8026E1AC(index);
    if (mUnidentified438[(int)context].mValues[index] == 0)
    {
        if (mUnidentified664[slot] != -1)
        {
            return;
        }
        int count = 0;
        fn_8026E1AC(index);
        for (int i = 0; i < mUnidentified6B4; ++i)
        {
            if ((int)context == mUnidentified664[i])
            {
                ++count;
            }
        }
        if (count < mUnidentified6B4 - 1)
        {
            fn_8026D4D4(index, context);
        }
    }
    else if (mUnidentified438[(int)context].mValues[index] == 1 && mUnidentified664[slot] == -1)
    {
        int count = 0;
        fn_8026E1AC(index);
        for (int i = 0; i < mUnidentified6B4; ++i)
        {
            if ((int)context == mUnidentified664[i])
            {
                ++count;
            }
        }
        if (count >= mUnidentified6B4 - 1)
        {
            if (!mUnidentified438[(int)context].fn_802192FC(1, index))
            {
                mUnidentified654[(int)context]->SetActiveSlide("controllers", true, false);
            }
            mUnidentified438[(int)context].mValues[index] = 0;
            --mUnidentified6A4[index];
        }
    }
}

void TU8026B10CScene::fn_8026D85C(int index, void* context)
{
    int slot = fn_8026E1AC(index);
    if (mUnidentified664[slot] != -1 && context != (void*)mUnidentified664[slot])
    {
        return;
    }
    if (mUnidentified664[slot] == -1)
    {
        int count = 0;
        fn_8026E1AC(index);
        for (int i = 0; i < mUnidentified6B4; ++i)
        {
            if ((int)context == mUnidentified664[i])
            {
                ++count;
            }
        }
        if (count >= mUnidentified6B4 - 1)
        {
            return;
        }
    }
    int side = -1;
    if (mUnidentified664[slot] == -1)
    {
        side = (int)context;
    }
    fn_801CBCA0(0xB3586309, 0, 0, 1);
    NetworkMessageType25_8050B778 message;
    message.mUnidentified08 = mUnidentified020.mMachineIndex;
    message.mUnidentified09 = side;
    message.mUnidentified0B = 0;
    if (fn_8025BDA0())
    {
        message.mUnidentified0A = index == lbl_806DE668[1];
    }
    else
    {
        message.mUnidentified0A = 0;
    }
    g_pNetworkSession->SendSidesChangedToHost(&message);
}

void TU8026B10CScene::fn_8026D984(int index, void* context)
{
    if (mUnidentified020.mMachineIndex != 0)
    {
        return;
    }
    ++mUnidentified6A4[index];
    mUnidentified5A0.mValues[index] = 1;
    mUnidentified5A0.fn_802195B4(index);
    if (!mUnidentified5A0.fn_802192FC(1, index))
    {
        mUnidentified65C->SetActiveSlide("over", true, false);
        fn_801CBCA0(0xAA73EF33, 0, 0, 1);
    }
}

void TU8026B10CScene::fn_8026DA2C(int index, void* context)
{
    if (mUnidentified5A0.mValues[index] == 0)
    {
        fn_8026D984(index, context);
    }
}

void TU8026B10CScene::fn_8026DAE0(int index, void* context)
{
    if (mUnidentified020.mMachineIndex != 0)
    {
        return;
    }
    --mUnidentified6A4[index];
    mUnidentified5A0.mValues[index] = 0;
    if (!mUnidentified5A0.fn_802192FC(1, index))
    {
        mUnidentified65C->SetActiveSlide("off", true, false);
    }
}

void TU8026B10CScene::fn_8026DB5C(int index, void* context)
{
    if (mUnidentified020.mMachineIndex != 0)
    {
        return;
    }
    mUnidentified438[0].fn_80206B54();
    mUnidentified438[1].fn_80206B54();
    mUnidentified5A0.fn_80206B54();
    for (int i = 0; i < 4; ++i)
    {
        GameInfoManager::Instance()->SetPlayingSide(i, mUnidentified664[i]);
        lbl_80578450[i]->SetActiveSlide("waiting", true, false);
    }
    mUnidentified020.mUnidentified0A = 0;
    for (int i = 0; i < 4; ++i)
    {
        if (mUnidentified664[i] != -1)
        {
            int machine = mUnidentified684[i].mUnidentified00;
            if (!mUnidentified684[i].mUnidentified04)
            {
                mUnidentified020.mUnidentified0B.mData[machine * 2] = mUnidentified664[i];
            }
            else
            {
                mUnidentified020.mUnidentified0B.mData[machine * 2 + 1] = mUnidentified664[i];
            }
        }
    }
    g_pNetworkSession->SendDraftToEveryone(&mUnidentified020);
    mUnidentified436 = true;
    fn_801CBCA0(0xF0AFD586, 0, 0, 1);
}

void TU8026B10CScene::fn_8026DEC0()
{
    bool assigned = true;
    bool home = false;
    bool away = false;
    for (int i = 0; i < mUnidentified6B4; ++i)
    {
        if (mUnidentified664[i] == -1)
        {
            assigned = false;
            break;
        }
        if (mUnidentified664[i] == 0)
        {
            home = true;
        }
        else if (mUnidentified664[i] == 1)
        {
            away = true;
        }
    }
    mUnidentified660->m_bVisible = !assigned;
    if (mUnidentified020.mMachineIndex != 0)
    {
        mUnidentified5A0.fn_80206B54();
        return;
    }
    bool ready = assigned && home && away;
    if (mUnidentified65C->m_bVisible == true)
    {
        if (!ready)
        {
            mUnidentified435 = false;
            mUnidentified65C->m_bVisible = false;
            mUnidentified5A0.fn_80206B54();
            for (int i = 0; i < 4; ++i)
            {
                if (mUnidentified5A0.mValues[i] == 1)
                {
                    --mUnidentified6A4[i];
                    mUnidentified5A0.mValues[i] = 0;
                }
            }
        }
    }
    else if (ready == true)
    {
        fn_801CBCA0(0x2AB04562, 0, 0, 1);
        mUnidentified65C->m_bVisible = true;
        mUnidentified65C->SetActiveSlide("in", true, false);
        mUnidentified435 = true;
    }
}

int TU8026B10CScene::fn_8026E1AC(int pad)
{
    int guest = -1;
    if (fn_8025BDA0())
    {
        guest = lbl_806DE668[1];
    }
    for (int i = 0; i < 4; ++i)
    {
        if (mUnidentified020.mMachineIndex == mUnidentified684[i].mUnidentified00)
        {
            if (pad == lbl_806E18B0 && !mUnidentified684[i].mUnidentified04)
            {
                return i;
            }
            if (pad == guest && mUnidentified684[i].mUnidentified04)
            {
                return i;
            }
        }
    }
    return -1;
}

void TU8026B10CScene::fn_8026E338(NetworkMessageType25_8050B778* message)
{
    if (message->mUnidentified0B == 0)
    {
        if (mUnidentified436)
        {
            return;
        }
        int index = -1;
        int machine = (s8)message->mUnidentified08;
        unsigned int guest = message->mUnidentified0A;
        for (int i = 0; i < 4; ++i)
        {
            if (machine == mUnidentified684[i].mUnidentified00
                && guest == mUnidentified684[i].mUnidentified04)
            {
                index = i;
                break;
            }
        }
        int side = (s8)message->mUnidentified09;
        if (side != -1)
        {
            int count = 0;
            fn_8026E1AC(index);
            for (int i = 0; i < mUnidentified6B4; ++i)
            {
                if (side == mUnidentified664[i])
                {
                    ++count;
                }
            }
            if (count >= mUnidentified6B4 - 1)
            {
                return;
            }
        }
        NetworkMessageType25_8050B778 response(*message);
        response.mUnidentified0B = 1;
        g_pNetworkSession->SendSidesChangedToEveryone(&response);
    }
    else
    {
        int index = -1;
        int machine = (s8)message->mUnidentified08;
        unsigned int guest = message->mUnidentified0A;
        for (int i = 0; i < 4; ++i)
        {
            if (machine == mUnidentified684[i].mUnidentified00
                && guest == mUnidentified684[i].mUnidentified04)
            {
                index = i;
                break;
            }
        }
        int oldSide = mUnidentified664[index];
        mUnidentified664[index] = (s8)message->mUnidentified09;
        fn_8026E6C4((s8)message->mUnidentified09, oldSide, index);
        fn_8026DEC0();
        if (mUnidentified020.mMachineIndex == (s8)message->mUnidentified08)
        {
            TLComponentInstance* controller = lbl_80578450[index];
            if (mUnidentified664[index] == -1)
            {
                controller->SetActiveSlide("holding", true, false);
            }
            else
            {
                controller->SetActiveSlide("cursor", true, false);
            }
        }
        char friendName[16];
        nlSNPrintf(friendName, sizeof(friendName), "friend_%d", index);
        FEFinder<TLTextInstance, 3>::Find(mPresentation->GetActiveSlide(),
            InlineHasher("Layer"), InlineHasher(friendName))->m_bVisible = mUnidentified664[index] == -1;
    }
}

void TU8026B10CScene::fn_8026E6C4(int newSide, int oldSide, int index)
{
    int slot = -1;
    char controller[16];
    tDebugPrintManager::Print(DC_NETWORK, "DoChangeSides NewSide %d OldSide %d OnlineIndex %d\n",
        newSide, oldSide, index);
    if (oldSide == newSide)
    {
        return;
    }
    if (oldSide == -1)
    {
        for (int i = 0; i < 3; ++i)
        {
            if (mUnidentified6B8[newSide][i] == -1)
            {
                slot = i;
                mUnidentified6B8[newSide][i] = index;
                break;
            }
        }
        nlSNPrintf(controller, sizeof(controller), "controller%d", slot);
        TLComponentInstance* component = FEFinder<TLComponentInstance, 4>::Find(
            mUnidentified654[newSide], nlStringLowerHash("controllers"),
            nlStringLowerHash(lbl_806DED40[newSide]), nlStringLowerHash(controller), 0, 0, 0);
        TLComponentInstance* over = FEFinder<TLComponentInstance, 4>::Find(
            mUnidentified654[newSide], nlStringLowerHash("over"),
            nlStringLowerHash(lbl_806DED40[newSide]), nlStringLowerHash(controller), 0, 0, 0);
        TLTextInstance* text = FEFinder<TLTextInstance, 3>::Find(
            component->GetActiveSlide(), InlineHasher("Text"));
        if (text == 0)
        {
            text = &UnidentifiedFallbackTextInstance;
        }
        TLTextInstance* overText = FEFinder<TLTextInstance, 3>::Find(
            over->GetActiveSlide(), InlineHasher("Text"));
        if (overText == 0)
        {
            overText = &UnidentifiedFallbackTextInstance;
        }
        text->SetString(mUnidentified6D0[index]);
        overText->SetString(mUnidentified6D0[index]);
        text->SetAssetColour(mUnidentified674[index]);
        overText->SetAssetColour(mUnidentified674[index]);
        component->m_bVisible = true;
        over->m_bVisible = true;
    }
    else
    {
        for (int i = 0; i < 3; ++i)
        {
            if (mUnidentified6B8[oldSide][i] == index)
            {
                slot = i;
                mUnidentified6B8[oldSide][i] = -1;
                break;
            }
        }
        nlSNPrintf(controller, sizeof(controller), "controller%d", slot);
        TLComponentInstance* component = FEFinder<TLComponentInstance, 4>::Find(
            mUnidentified654[oldSide], nlStringLowerHash("controllers"),
            nlStringLowerHash(lbl_806DED40[oldSide]), nlStringLowerHash(controller), 0, 0, 0);
        TLComponentInstance* over = FEFinder<TLComponentInstance, 4>::Find(
            mUnidentified654[oldSide], nlStringLowerHash("over"),
            nlStringLowerHash(lbl_806DED40[oldSide]), nlStringLowerHash(controller), 0, 0, 0);
        FEFinder<TLTextInstance, 3>::Find(component->GetActiveSlide(), InlineHasher("Text"));
        FEFinder<TLTextInstance, 3>::Find(over->GetActiveSlide(), InlineHasher("Text"));
        component->m_bVisible = false;
        over->m_bVisible = false;
    }
}

void TU8026B10CScene::fn_8026EC64()
{
    mUnidentified01C = false;
    fn_801CBCA0(0x37A9934D, 0, 0, 1);
    lbl_806E1838->Push((SceneList)40, SCREEN_BACK, true);
}
