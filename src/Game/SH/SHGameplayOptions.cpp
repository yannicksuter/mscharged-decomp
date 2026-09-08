#include "Game/SH/SHGameplayOptions.h"
#include "Game/FE/feHelpFuncs.h"
#include "Game/FE/FEAudio.h"

#include "Game/GameSceneManager.h"
#include "Game/FE/feFinder.inl"
#include "Game/FE/feInput.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlTextInstance.h"
#include "Game/GameInfo.h"
#include "Game/Render/Presentation.h"
#include "Game/FriendManager.h"
#include "NL/nlBind.h"
#include "NL/nlFormat.h"
#include "NL/nlLocalizationLookup.h"
#include "NL/nlPrint.h"
#include "Game/FE/feDPD.h"
#include "Game/SH/SHNavigation.h"
#include "Game/SH/SHOptionsCheatsList.h"

#include <string.h>


static const int lbl_804E8540[5] = { 1, 2, 3, 4, 5 };
static const int lbl_804E8554[5] = { 1, 3, 5, 7, 9 };
static const int lbl_804E8568[8] = { 3, 4, 5, 6, 7, 8, 9, 10 };
static const int lbl_804E8588[4] = { 120, 180, 240, 300 };
static const char* const lbl_804E8598[3] = { "off", "over", "down" };

typedef BasicString<unsigned short, Detail::TempStringAllocator> WideString;

SHGameplayOptions::SHGameplayOptions()
    : mUnidentified15B8(1)
    , mUnidentified15BC(false)
    , mUnidentified15BD(true)
    , mUnidentified15FC(0)
{
    for (int i = 0; i < 24; ++i)
    {
        mOptionButtons[i].mContext = (void*)i;
        mOptionButtons[i].mSpeakerEnabled = false;
    }
    for (int i = 0; i < 3; ++i)
    {
        mCheatButtons[i].mContext = (void*)i;
        mCheatButtons[i].mSpeakerEnabled = false;
    }
    for (int i = 0; i < 4; ++i)
        mPointerInsideCounts[i] = 0;
    mUnidentified15E8 = 0;
    mUnidentified15EC = 5;
    mUnidentified15F0 = 10;
    mUnidentified15F4 = 20;
    mUnidentified15F8 = 12;
    GameInfoManager* gameInfo = GameInfoManager::Instance();
    if (gameInfo->mIsOnlineMode && !gameInfo->mOnlineRankedMatch)
    {
        mSettings = reinterpret_cast<const GameplaySettings&>(gameInfo->mUserInfo.mUnidentified4C);
        mPowerupSettings = reinterpret_cast<const PowerupSettings&>(GameInfoManager::Instance()->mUserInfo.mUnidentified68);
    }
    else
    {
        mSettings = reinterpret_cast<const GameplaySettings&>(gameInfo->mUserInfo.mGameplayOptions);
        mPowerupSettings = reinterpret_cast<const PowerupSettings&>(GameInfoManager::Instance()->mUserInfo.mPowerupOptions);
    }
    mNavigation.SetPushBackScene(false);
    mNavigation.SetPopScene(false);
}

SHGameplayOptions::~SHGameplayOptions()
{
}

void SHGameplayOptions::SceneCreated()
{
    FEPresentation* presentation = mPresentation;
    mUnidentified1388 = FEFinder<TLInstance, 2>::Find(presentation,
        nlStringLowerHash("OPTIONS"), nlStringLowerHash("Layer"), nlStringLowerHash("GOALS"), 0, 0, 0);
    mUnidentified138C = FEFinder<TLInstance, 2>::Find(presentation,
        nlStringLowerHash("OPTIONS"), nlStringLowerHash("Layer"), nlStringLowerHash("MINUTES"), 0, 0, 0);
    mUnidentified1390 = FEFinder<TLInstance, 2>::Find(presentation,
        nlStringLowerHash("OPTIONS"), nlStringLowerHash("Layer"), nlStringLowerHash("SKILL LEVEL"), 0, 0, 0);
    mUnidentified1394 = FEFinder<TLInstance, 2>::Find(presentation,
        nlStringLowerHash("OPTIONS"), nlStringLowerHash("Layer"), nlStringLowerHash("BEST OF SERIES"), 0, 0, 0);
    mOptionInstances[10] = FEFinder<TLComponentInstance, 4>::Find(presentation,
        nlStringLowerHash("OPTIONS"), nlStringLowerHash("Layer"), nlStringLowerHash("BTN_GOALS"), 0, 0, 0);
    mOptionInstances[11] = FEFinder<TLComponentInstance, 4>::Find(presentation,
        nlStringLowerHash("OPTIONS"), nlStringLowerHash("Layer"), nlStringLowerHash("BTN_TIME"), 0, 0, 0);
    for (int i = 0; i < 5; ++i)
    {
        char name[16];
        nlSNPrintf(name, sizeof(name), "BUTTON_%d", i);
        mOptionInstances[i] = FEFinder<TLComponentInstance, 4>::Find(mUnidentified1390, InlineHasher(name));
    }
    for (int i = 5; i < 10; ++i)
    {
        char name[16];
        nlSNPrintf(name, sizeof(name), "BUTTON_%d", i - 5);
        mOptionInstances[i] = FEFinder<TLComponentInstance, 4>::Find(mUnidentified1394, InlineHasher(name));
    }
    for (int i = 12; i < 20; ++i)
    {
        char name[16];
        nlSNPrintf(name, sizeof(name), "BUTTON_%d", i - 12);
        mOptionInstances[i] = FEFinder<TLComponentInstance, 4>::Find(mUnidentified1388, InlineHasher(name));
    }
    for (int i = 20; i < 24; ++i)
    {
        char name[16];
        nlSNPrintf(name, sizeof(name), "BUTTON_%d", i - 18);
        mOptionInstances[i] = FEFinder<TLComponentInstance, 4>::Find(mUnidentified138C, InlineHasher(name));
    }
    mCheatInstances[0] = FEFinder<TLComponentInstance, 4>::Find(presentation,
        nlStringLowerHash("CHEATS"), nlStringLowerHash("Layer"), nlStringLowerHash("cheat_0"), 0, 0, 0);
    mCheatInstances[1] = FEFinder<TLComponentInstance, 4>::Find(presentation,
        nlStringLowerHash("CHEATS"), nlStringLowerHash("Layer"), nlStringLowerHash("cheat_1"), 0, 0, 0);
    mCheatInstances[2] = FEFinder<TLComponentInstance, 4>::Find(presentation,
        nlStringLowerHash("CHEATS"), nlStringLowerHash("Layer"), nlStringLowerHash("cheat_2"), 0, 0, 0);
    TLComponentInstance* done = 0;
    SHNavigation* scene = GetNavigationScene();
    if (scene != 0)
    {
        scene->HideButtons();
        done = scene->GetButton(4);
        mDoneButtonInstance = scene->GetButton(64);
        mPageControls = &scene->mPageControls;
        mPageControls->SetButtonState(1, true, false);
        mPageControls->SetButtonState(0, true, false);
    }
    mNavigation.SetButtonInstance(done);
    fn_80235FE0();
    presentation->SetActiveSlide("OPTIONS_IN", true);
}

void SHGameplayOptions::Update(float dt)
{
    BaseSceneHandler::Update(dt);
    if (mUnidentified15FC == 0 || (mUnidentified15FC >= 2 && mUnidentified15FC <= 4))
    {
        TLSlide* slide = mPresentation->m_currentSlide;
        if (slide->m_time < slide->m_start + slide->m_duration)
        {
            for (int i = 0; i < 4; ++i)
                gFEPointerInstances[i]->SetActiveSlide("waiting", true, false);
            return;
        }
        if (mUnidentified15FC == 0)
        {
            SHNavigation* scene = GetNavigationScene();
            if (scene != 0)
                scene->SetButtons(79, true);
            mUnidentified15FC = 1;
            fn_80235928();
        }
        else if (mUnidentified15FC == 2)
        {
            fn_80238050();
            return;
        }
        else if (mUnidentified15FC == 3)
        {
            GameInfoManager* gameInfo = GameInfoManager::Instance();
            if (gameInfo->mIsOnlineMode && !gameInfo->mOnlineRankedMatch)
            {
                if (gameInfo->mOnlineTwoLocalPlayers)
                    GameSceneManager::Instance()->Push(SCENE_ONLINE_GUEST_CONTROLLER_SELECT, SCREEN_NOTHING, true);
                else
                    GameSceneManager::Instance()->Push((SceneList)42, SCREEN_NOTHING, true);
            }
            else
            {
                FEAudio::PlayAnimAudioEvent(0x4430B152, 0, 0, 1);
                GameSceneManager::Instance()->Pop();
                Presentation::GetInstance()->Call("TransitionGrudgeMatchToMainMenu");
            }
            return;
        }
        else if (mUnidentified15FC == 4)
        {
            mUnidentified15FC = 0;
            mPresentation->SetActiveSlide("IN", true);
            mPresentation->Update(0.0f);
            return;
        }
    }
    if (!mUnidentified15BC)
    {
        fn_80236E54();
        mUnidentified15BC = true;
    }
    GameInfoManager* gameInfo = GameInfoManager::Instance();
    if (gameInfo->mIsOnlineMode && !gameInfo->mOnlineRankedMatch
        && !GameSceneManager::Instance()->IsOnStack((SceneList)10)
        && g_pFriendManager->FindHostInvitation())
    {
        if (GameSceneManager::Instance()->IsOnStack((SceneList)28))
            GameSceneManager::Instance()->Pop();
        g_pFriendManager->mReturnScene = 27;
        g_pFriendManager->mPreviousRankedMode = 0;
        GameSceneManager::Instance()->Push((SceneList)52, SCREEN_FORWARD, true);
        return;
    }
    for (int i = 0; i < 4; ++i)
    {
        TLComponentInstance* cursor = gFEPointerInstances[i];
        if (g_pFEInput->m_InputLockDepth == 0)
        {
            if ((unsigned int)i != gFEControllerIndex)
            {
                cursor->SetActiveSlide("waiting", true, false);
                continue;
            }
            if (mPointerInsideCounts[i] > 0 || mNavigation.mPointerInside[i]
                || mPageControls->mPointerInside[1] || mPageControls->mPointerInside[0])
                cursor->SetActiveSlide("A", true, false);
            else
                cursor->SetActiveSlide("cursor", true, false);
        }
        u8 valid = true;
        FEPointerEvent event;
        event.mIndex = i;
        event.mPosition = GetPointerPosition(i, &valid);
        event.mPressed = g_pFEInput->JustPressed((eFEINPUT_PAD)i, 30, true, 0);
        for (int j = 0; j < 24; ++j)
            mOptionButtons[j].HandlePointerEvent(&event);
        for (int j = 0; j < 3; ++j)
            mCheatButtons[j].HandlePointerEvent(&event);
        mPageControls->Update(event, dt);
        if (mPageControls->mPointerPressed[0] || mPageControls->mPadPressed[0]
            || mPageControls->mPointerPressed[1] || mPageControls->mPadPressed[1])
        {
            FEAudio::PlayAnimAudioEvent(0x375C885A, 0, 0, 1);
            FEAudio::PlayAnimAudioEvent(0xEA7AD449, 0, 0, 1);
            mUnidentified15FC = 4;
            mPresentation->SetActiveSlide("OPTIONS_OUT", true);
            mPresentation->Update(0.0f);
            SHNavigation* scene = GetNavigationScene();
            if (scene != 0)
            {
                if (mPageControls->mPointerPressed[0] || mPageControls->mPadPressed[0])
                    scene->SetButtons(9, false);
                else
                    scene->SetButtons(10, false);
            }
            return;
        }
        if (mNavigation.UpdateBackButton(event, dt))
        {
            mUnidentified15FC = 3;
            SHNavigation* scene = GetNavigationScene();
            if (scene != 0)
                scene->HideButtons();
            mPresentation->SetActiveSlide("OPTIONS_OUT", true);
            return;
        }
        mDoneButton.HandlePointerEvent(&event);
    }
}

void SHGameplayOptions::fn_80235928()
{
    SHNavigation* scene = GetNavigationScene();
    if (mUnidentified15B8 == 1)
    {
        mPresentation->SetActiveSlide("OPTIONS", true);
        mPresentation->Update(0.0f);
        mUnidentified15B8 = 0;
        mPageControls->SetButtonState(1, true, true);
        mPageControls->SetButtonState(0, false, false);
        mPageControls->ClearButtonHighlight(1);
        SetBreadcrumbs(2, 0);
        if (scene != 0)
            scene->SetButtonVisibility(2, false);
    }
    else
    {
        mPresentation->SetActiveSlide("CHEATS", true);
        mPresentation->Update(0.0f);
        mUnidentified15B8 = 1;
        mPageControls->SetButtonState(0, true, true);
        mPageControls->SetButtonState(1, false, false);
        mPageControls->ClearButtonHighlight(0);
        SetBreadcrumbs(2, 1);
        UpdateCheatText();
        if (scene != 0)
            scene->SetButtonVisibility(1, false);
    }
    bool options = mUnidentified15B8 == 0;
    bool cheats = mUnidentified15B8 == 1;
    for (int i = 0; i < 24; ++i)
    {
        if (options)
            mOptionButtons[i].Enable();
        else
        {
            if (mOptionButtons[i].HasOtherPointerState(1, -1))
            {
                for (int j = 0; j < 4; ++j)
                    mOptionButtons[i].SetPointerState(0, j);
                mOptionInstances[i]->SetActiveSlide("off", true, false);
            }
            mOptionButtons[i].Disable();
        }
    }
    for (int i = 0; i < 3; ++i)
    {
        if (cheats)
            mCheatButtons[i].Enable();
        else
        {
            if (mCheatButtons[i].HasOtherPointerState(1, -1))
            {
                for (int j = 0; j < 4; ++j)
                    mCheatButtons[i].SetPointerState(0, j);
                mCheatInstances[i]->SetActiveSlide("off", true, false);
            }
            mCheatButtons[i].Disable();
        }
    }
    for (int i = 0; i < 4; ++i)
        mPointerInsideCounts[i] = 0;
    if (mUnidentified15B8 == 0)
        fn_80235CE4(mUnidentified15BD);
}

void SHGameplayOptions::fn_80235CE4(bool value)
{
    mUnidentified15BD = value;
    bool other = !value;
    mUnidentified1388->m_bVisible = value;
    mUnidentified138C->m_bVisible = other;
    for (int i = 12; i < 20; ++i)
    {
        if (mUnidentified15BD)
            mOptionButtons[i].Enable();
        else
            mOptionButtons[i].Disable();
    }
    for (int i = 20; i < 24; ++i)
    {
        if (other)
            mOptionButtons[i].Enable();
        else
            mOptionButtons[i].Disable();
    }
}

void SHGameplayOptions::fn_80235FE0()
{
    int type = mSettings.WinBy;
    int skill = mSettings.SkillLevel;
    int series = mSettings.BestSeries;
    int time = mSettings.GameTime;
    int goals = mSettings.GameGoals;
    int value = type == 1 ? goals : time / 60;
    TLComponentInstance* instance = FEFinder<TLComponentInstance, 4>::Find(mPresentation,
        nlStringLowerHash("OPTIONS"), nlStringLowerHash("Layer"), nlStringLowerHash("SKILL LEVEL SETTINGS"), 0, 0, 0);
    switch (skill)
    {
    case 1: instance->SetActiveSlide("ROOKIE", true, false); break;
    case 2: instance->SetActiveSlide("PROFESSIONAL", true, false); break;
    case 3: instance->SetActiveSlide("SUPERSTAR", true, false); break;
    case 4: instance->SetActiveSlide("LEGEND", true, false); break;
    case 5: instance->SetActiveSlide("MEGASTRIKER", true, false); break;
    }
    TLTextInstance* text = FEFinder<TLTextInstance, 3>::Find(mPresentation,
        nlStringLowerHash("OPTIONS"), nlStringLowerHash("Layer"), nlStringLowerHash("SERIES SETTING"), 0, 0, 0);
    unsigned short number[4];
    nlSNPrintf(number, 4, (const unsigned short*)L"%d", series);
    {
        WideString string = Format(WideString(LookupLocString("OPTIONS_BEST_OF")), number);
        memcpy(mUnidentified1538, string.c_str(), sizeof(mUnidentified1538));
        text->SetString(mUnidentified1538);
    }
    mUnidentified15BD = type == 1;
    mUnidentified15F0 = type == 1 ? 10 : 11;
    mOptionInstances[mUnidentified15F0]->SetActiveSlide("down", true, false);
    for (int j = 0; j < 4; ++j)
        mOptionButtons[mUnidentified15F0].SetPointerState(2, j);
    fn_80236ADC(type, value);
    for (int i = 0; i < 5; ++i)
    {
        if (skill == lbl_804E8540[i])
        {
            mUnidentified15E8 = i;
            mOptionInstances[i]->SetActiveSlide("down", true, false);
            for (int j = 0; j < 4; ++j)
                mOptionButtons[i].SetPointerState(2, j);
            break;
        }
    }
    for (int i = 0; i < 5; ++i)
    {
        if (series == lbl_804E8554[i])
        {
            mUnidentified15EC = i + 5;
            mOptionInstances[mUnidentified15EC]->SetActiveSlide("down", true, false);
            for (int j = 0; j < 4; ++j)
                mOptionButtons[mUnidentified15EC].SetPointerState(2, j);
            break;
        }
    }
    for (int i = 0; i < 8; ++i)
    {
        if (goals == lbl_804E8568[i])
        {
            mUnidentified15F8 = i + 12;
            mOptionInstances[mUnidentified15F8]->SetActiveSlide("down", true, false);
            for (int j = 0; j < 4; ++j)
                mOptionButtons[mUnidentified15F8].SetPointerState(2, j);
            break;
        }
    }
    for (int i = 0; i < 4; ++i)
    {
        if (time == lbl_804E8588[i])
        {
            mUnidentified15F4 = i + 20;
            mOptionInstances[mUnidentified15F4]->SetActiveSlide("down", true, false);
            for (int j = 0; j < 4; ++j)
                mOptionButtons[mUnidentified15F4].SetPointerState(2, j);
            break;
        }
    }
}

void SHGameplayOptions::fn_802365F0(int item)
{
    if (item >= 0 && item < 5)
    {
        int skill = lbl_804E8540[item];
        mSettings.SkillLevel = (GameplaySettings::eSkillLevel)skill;
        TLComponentInstance* instance = FEFinder<TLComponentInstance, 4>::Find(mPresentation,
            nlStringLowerHash("OPTIONS"), nlStringLowerHash("Layer"), nlStringLowerHash("SKILL LEVEL SETTINGS"), 0, 0, 0);
        switch (skill)
        {
        case 1: instance->SetActiveSlide("ROOKIE", true, false); break;
        case 2: instance->SetActiveSlide("PROFESSIONAL", true, false); break;
        case 3: instance->SetActiveSlide("SUPERSTAR", true, false); break;
        case 4: instance->SetActiveSlide("LEGEND", true, false); break;
        case 5: instance->SetActiveSlide("MEGASTRIKER", true, false); break;
        }
    }
    else if (item >= 5 && item < 10)
    {
        int series = lbl_804E8554[item - 5];
        mSettings.BestSeries = series;
        TLTextInstance* text = FEFinder<TLTextInstance, 3>::Find(mPresentation,
            nlStringLowerHash("OPTIONS"), nlStringLowerHash("Layer"), nlStringLowerHash("SERIES SETTING"), 0, 0, 0);
        unsigned short number[4];
        nlSNPrintf(number, 4, (const unsigned short*)L"%d", series);
        WideString string = Format(WideString(LookupLocString("OPTIONS_BEST_OF")), number);
        memcpy(mUnidentified1538, string.c_str(), sizeof(mUnidentified1538));
        text->SetString(mUnidentified1538);
    }
    else if (item == 10)
    {
        mSettings.WinBy = 1;
        fn_80236ADC(1, mSettings.GameGoals);
    }
    else if (item == 11)
    {
        mSettings.WinBy = 0;
        fn_80236ADC(0, mSettings.GameTime / 60);
    }
    else if (item >= 12 && item < 20)
    {
        mSettings.GameGoals = lbl_804E8568[item - 12];
        fn_80236ADC(1, mSettings.GameGoals);
    }
    else if (item >= 20 && item < 24)
    {
        mSettings.GameTime = lbl_804E8588[item - 20];
        fn_80236ADC(0, mSettings.GameTime / 60);
    }
}

void SHGameplayOptions::fn_80236ADC(int type, int value)
{
    TLComponentInstance* instance = FEFinder<TLComponentInstance, 4>::Find(mPresentation,
        nlStringLowerHash("OPTIONS"), nlStringLowerHash("Layer"), nlStringLowerHash("GAMEPLAYOPTIONS SETTING"), 0, 0, 0);
    TLTextInstance* text = FEFinder<TLTextInstance, 3>::Find(instance->GetActiveSlide(), InlineHasher("GAMEPLAY OPTIONS SETTING"));
    unsigned short number[4];
    nlSNPrintf(number, 4, (const unsigned short*)L"%d", value);
    const char* id = "X_GOALS";
    if (type == 0)
        id = "X_MINUTES";
    WideString string = Format(WideString(LookupLocString(id)), number);
    memcpy(mUnidentified1578, string.c_str(), sizeof(mUnidentified1578));
    text->SetString(mUnidentified1578);
}

void SHGameplayOptions::fn_80236E54()
{
    FEPointerListener::Callback over(Bind<void>(MemFun(&SHGameplayOptions::fn_802378F8), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback off(Bind<void>(MemFun(&SHGameplayOptions::fn_802379D0), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback down(Bind<void>(MemFun(&SHGameplayOptions::fn_80237A80), this, Placeholder<0>(), Placeholder<1>()));
    feVector3 skill = mUnidentified1390->GetAssetPosition();
    feVector3 series = mUnidentified1394->GetAssetPosition();
    feVector3 minutes = mUnidentified138C->GetAssetPosition();
    feVector3 goals = mUnidentified1388->GetAssetPosition();
    for (int i = 0; i < 24; ++i)
    {
        float x = 0.0f;
        float y = 0.0f;
        if (i >= 0 && i < 5) { x = skill.f.x; y = skill.f.y; }
        else if (i >= 5 && i < 10) { x = series.f.x; y = series.f.y; }
        else if (i >= 12 && i < 20) { x = goals.f.x; y = goals.f.y; }
        else if (i >= 20 && i < 24) { x = minutes.f.x; y = minutes.f.y; }
        if (i < 12)
            mOptionButtons[i].SetInstanceBounds(mOptionInstances[i], true, x, y, 1.0f, 1.0f);
        else
            mOptionButtons[i].SetInstanceBounds(mOptionInstances[i], true, x, y, 0.9f, 0.8f);
        mOptionButtons[i].SetPointerEnterCallback(over);
        mOptionButtons[i].SetPointerLeaveCallback(off);
        mOptionButtons[i].SetPointerPressCallback(down);
    }
    FEPointerListener::Callback cheatOver(Bind<void>(MemFun(&SHGameplayOptions::fn_80237C7C), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback cheatOff(Bind<void>(MemFun(&SHGameplayOptions::fn_80237D34), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback cheatDown(Bind<void>(MemFun(&SHGameplayOptions::fn_80237DE0), this, Placeholder<0>(), Placeholder<1>()));
    for (int i = 0; i < 3; ++i)
    {
        TLInstance* instance = FEFinder<TLInstance, 2>::Find(mCheatInstances[i],
            nlStringLowerHash("off"), nlStringLowerHash("CHALLENGE_0"), nlStringLowerHash("list_back_480x70 "), 0, 0, 0);
        feVector3 position = mCheatInstances[i]->GetAssetPosition();
        mCheatButtons[i].SetInstanceBounds(instance, true, position.f.x, position.f.y, 0.95f, 0.75f);
        mCheatButtons[i].SetPointerEnterCallback(cheatOver);
        mCheatButtons[i].SetPointerLeaveCallback(cheatOff);
        mCheatButtons[i].SetPointerPressCallback(cheatDown);
    }
    FEPointerListener::Callback nextOver(Bind<void>(MemFun(&SHGameplayOptions::fn_80237E70), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback nextOff(Bind<void>(MemFun(&SHGameplayOptions::fn_80237EF8), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback nextDown(Bind<void>(MemFun(&SHGameplayOptions::fn_80237F68), this, Placeholder<0>(), Placeholder<1>()));
    SetDoneButtonBounds(&mDoneButton, mDoneButtonInstance, true);
    mDoneButton.SetPointerEnterCallback(nextOver);
    mDoneButton.SetPointerLeaveCallback(nextOff);
    mDoneButton.SetPointerPressCallback(nextDown);
}

void SHGameplayOptions::fn_802378F8(int index, void* context)
{
    unsigned int item = (unsigned int)context;
    if (!mOptionButtons[item].HasOtherPointerState(2, -1))
    {
        ++mPointerInsideCounts[index];
        mOptionButtons[item].PlayHoverFeedback(index);
        if (!mOptionButtons[item].HasOtherPointerState(1, index))
        {
            mOptionInstances[item]->SetActiveSlide("over", true, false);
            FEAudio::PlayAnimAudioEvent(0xF6EB899E, 0, 0, 1);
            mOptionButtons[item].SetPointerState(1, index);
        }
    }
}

void SHGameplayOptions::fn_802379D0(int index, void* context)
{
    unsigned int item = (unsigned int)context;
    if (!mOptionButtons[item].HasOtherPointerState(2, -1))
    {
        --mPointerInsideCounts[index];
        if (!mOptionButtons[item].HasOtherPointerState(1, index))
        {
            mOptionInstances[item]->SetActiveSlide("off", true, false);
            mOptionButtons[item].SetPointerState(1, index);
        }
    }
}

void SHGameplayOptions::fn_80237A80(int index, void* context)
{
    unsigned int item = (unsigned int)context;
    if (mOptionButtons[item].HasOtherPointerState(2, -1))
        return;
    mOptionInstances[item]->SetActiveSlide("down", true, false);
    for (int j = 0; j < 4; ++j)
        mOptionButtons[item].SetPointerState(2, j);
    --mPointerInsideCounts[index];
    int previous = -1;
    if (item < 5)
    {
        previous = mUnidentified15E8;
        mUnidentified15E8 = item;
        FEAudio::PlayAnimAudioEvent(0xF0AFD586, 0, 0, 1);
    }
    else if (item >= 5 && item < 10)
    {
        previous = mUnidentified15EC;
        mUnidentified15EC = item;
        FEAudio::PlayAnimAudioEvent(0xF0AFD586, 0, 0, 1);
    }
    else if (item == 10 || item == 11)
    {
        fn_80235CE4(item == 10);
        previous = mUnidentified15F0;
        mUnidentified15F0 = item;
        FEAudio::PlayAnimAudioEvent(0x362F2841, 0, 0, 1);
    }
    else if (item >= 12 && item < 20)
    {
        previous = mUnidentified15F8;
        mUnidentified15F8 = item;
        FEAudio::PlayAnimAudioEvent(0xF0AFD586, 0, 0, 1);
    }
    else if (item >= 20 && item < 24)
    {
        previous = mUnidentified15F4;
        mUnidentified15F4 = item;
        FEAudio::PlayAnimAudioEvent(0xF0AFD586, 0, 0, 1);
    }
    fn_802365F0(item);
    mOptionInstances[previous]->SetActiveSlide("off", true, false);
    for (int j = 0; j < 4; ++j)
        mOptionButtons[previous].SetPointerState(0, j);
}

void SHGameplayOptions::fn_80237C7C(int index, void* context)
{
    unsigned int item = (unsigned int)context;
    ++mPointerInsideCounts[index];
    mCheatButtons[item].PlayHoverFeedback(index);
    if (!mCheatButtons[item].HasOtherPointerState(1, index))
    {
        mCheatInstances[item]->SetActiveSlide("over", true, false);
        FEAudio::PlayAnimAudioEvent(0xF6EB899E, 0, 0, 1);
        mCheatButtons[item].SetPointerState(1, index);
    }
}

void SHGameplayOptions::fn_80237D34(int index, void* context)
{
    unsigned int item = (unsigned int)context;
    --mPointerInsideCounts[index];
    if (!mCheatButtons[item].HasOtherPointerState(1, index))
    {
        mCheatInstances[item]->SetActiveSlide("off", true, false);
        mCheatButtons[item].SetPointerState(1, index);
    }
}

void SHGameplayOptions::fn_80237DE0(int index, void* context)
{
    SHOptionsCheatsList* scene = (SHOptionsCheatsList*)GameSceneManager::Instance()->Push((SceneList)28, SCREEN_NOTHING, false);
    scene->mCheatCategory = (int)context;
    scene->mSettings = &mPowerupSettings;
    FEAudio::PlayAnimAudioEvent(0xF0AFD586, 0, 0, 1);
    FEAudio::PlayAnimAudioEvent(0xBB142B94, 0, 0, 1);
}

void SHGameplayOptions::fn_80237E70(int index, void* context)
{
    ++mPointerInsideCounts[index];
    mDoneButton.SetPointerState(1, index);
    if (!mDoneButton.HasOtherPointerState(1, index))
    {
        mDoneButtonInstance->SetActiveSlide("over", true, false);
        FEAudio::PlayAnimAudioEvent(0xAA73EF33, 0, 0, 1);
    }
}

void SHGameplayOptions::fn_80237EF8(int index, void* context)
{
    --mPointerInsideCounts[index];
    mDoneButton.SetPointerState(0, index);
    if (!mDoneButton.HasOtherPointerState(1, index))
        mDoneButtonInstance->SetActiveSlide("off", true, false);
}

void SHGameplayOptions::fn_80237F68(int index, void* context)
{
    mUnidentified15FC = 2;
    mPresentation->SetActiveSlide("OPTIONS_OUT", true);
    SHNavigation* scene = GetNavigationScene();
    if (scene != 0)
        scene->SetButtons(64, true);
    mDoneButtonInstance->SetActiveSlide("down", true, false);
    FEAudio::PlayAnimAudioEvent(0xF0AFD586, 0, 0, 1);
    GameInfoManager* gameInfo = GameInfoManager::Instance();
    if (gameInfo->mIsOnlineMode && !gameInfo->mOnlineRankedMatch)
        FEAudio::PlayAnimAudioEvent(0x64B85E8D, 0, 0, 1);
    else
        FEAudio::PlayAnimAudioEvent(0x5BCD337B, 0, 0, 1);
}

void SHGameplayOptions::fn_80238050()
{
    GameInfoManager* gameInfo = GameInfoManager::Instance();
    if (gameInfo->mIsOnlineMode && !gameInfo->mOnlineRankedMatch)
    {
        reinterpret_cast<GameplaySettings&>(gameInfo->mUserInfo.mUnidentified4C) = mSettings;
        reinterpret_cast<GameplaySettings&>(GameInfoManager::Instance()->mNoCheatSettings) = mSettings;
        reinterpret_cast<PowerupSettings&>(GameInfoManager::Instance()->mUserInfo.mUnidentified68) = mPowerupSettings;
        reinterpret_cast<PowerupSettings&>(GameInfoManager::Instance()->mRulesA) = mPowerupSettings;
        GameSceneManager::Instance()->Push((SceneList)5, SCREEN_FORWARD, true);
    }
    else
    {
        reinterpret_cast<GameplaySettings&>(gameInfo->mUserInfo.mGameplayOptions) = mSettings;
        reinterpret_cast<PowerupSettings&>(GameInfoManager::Instance()->mUserInfo.mPowerupOptions) = mPowerupSettings;
        GameSceneManager::Instance()->Push((SceneList)2, SCREEN_FORWARD, true);
    }
}

void SHGameplayOptions::UpdateCheatText()
{
    const char* strings[6];
    strings[0] = GetLOCEnvironmentCheatName(mPowerupSettings.mEnvironmentCheat);
    strings[1] = GetLOCEnvironmentCheatDescription(mPowerupSettings.mEnvironmentCheat);
    strings[2] = GetLOCPowerupCheatName(mPowerupSettings.mCustomPowerups);
    strings[3] = GetLOCPowerupCheatDescription(mPowerupSettings.mCustomPowerups);
    strings[4] = GetLOCPlayerCheatName(mPowerupSettings.mPlayerCheat);
    strings[5] = GetLOCPlayerCheatDescription(mPowerupSettings.mPlayerCheat);
    const char* slides[3] = { lbl_804E8598[0], lbl_804E8598[1], lbl_804E8598[2] };
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            TLTextInstance* text0 = FEFinder<TLTextInstance, 3>::Find(mCheatInstances[i],
                nlStringLowerHash(slides[j]), nlStringLowerHash("CHALLENGE_0"), nlStringLowerHash("stat_0"), 0, 0, 0);
            TLTextInstance* text1 = FEFinder<TLTextInstance, 3>::Find(mCheatInstances[i],
                nlStringLowerHash(slides[j]), nlStringLowerHash("CHALLENGE_0"), nlStringLowerHash("stat_1"), 0, 0, 0);
            text0->SetStringId(strings[i * 2]);
            text1->SetStringId(strings[i * 2 + 1]);
        }
    }
}
