#include "Game/SH/SHGameplayOptions.h"
#include "NL/nlFunction.inl"
#include "Game/FE/feHelpFuncs_decl.h"
#include "Game/FE/FEAudio.h"

#include "Game/GameSceneManager.h"
#include "Game/FE/feInlineHasher.h"
#include "Game/FE/feFinder_impl.h"
#include "Game/FE/feInput.h"
#include "Game/FE/fePointer.inl"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlTextInstance.h"
#include "Game/GameInfo.h"
#include "Game/Render/FrontEndPresentation.h"
#include "Game/FriendManager.h"
#include "NL/nlBind.h"
#include "NL/nlFormat.h"
#include "NL/nlLocalizationLookup.h"
#include "NL/nlPrint.h"
#include "NL/globalpad.h"
#include "Game/FE/feDPD.h"
#include "Game/SH/SHNavigation.h"
#include "Game/SH/SHOptionsCheatsList.h"

#include <string.h>

static const int sSkillLevelOptions[5] = { 1, 2, 3, 4, 5 };
static const int sNumGamesOptions[5] = { 1, 3, 5, 7, 9 };
static const int sGoalLimitOptions[8] = { 3, 4, 5, 6, 7, 8, 9, 10 };
static const int sGameTimeOptions[4] = { 120, 180, 240, 300 };
typedef BasicString<unsigned short, Detail::TempStringAllocator> WideString;

SHGameplayOptions::SHGameplayOptions()
    : mViewIndex(1)
    , mInitialized(false)
    , mGoalLimitSelected(true)
    , mFlowState(0)
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
    mSelectedSkill = 0;
    mSelectedSeries = 5;
    mSelectedLimitType = 10;
    mSelectedTime = 20;
    mSelectedGoals = 12;
    GameInfoManager* gameInfo = GameInfoManager::Instance();
    if (gameInfo->UseAltRules())
    {
        mSettings = gameInfo->mUserInfo.mAltGameplayOptions;
        mPowerupSettings = GameInfoManager::Instance()->mUserInfo.mAltCheatOptions;
    }
    else
    {
        mSettings = gameInfo->mUserInfo.mGameplayOptions;
        mPowerupSettings = GameInfoManager::Instance()->mUserInfo.mCheatOptions;
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
    mGoalsSection = FEFinder<TLInstance, 2>::Find(presentation,
        "OPTIONS", "Layer", "GOALS", 0UL, 0UL, 0UL);
    mMinutesSection = FEFinder<TLInstance, 2>::Find(presentation,
        "OPTIONS", "Layer", "MINUTES", 0UL, 0UL, 0UL);
    mSkillSection = FEFinder<TLInstance, 2>::Find(presentation,
        "OPTIONS", "Layer", "SKILL LEVEL", 0UL, 0UL, 0UL);
    mSeriesSection = FEFinder<TLInstance, 2>::Find(presentation,
        "OPTIONS", "Layer", "BEST OF SERIES", 0UL, 0UL, 0UL);
    mOptionInstances[10] = FEFinder<TLComponentInstance, 4>::Find(presentation,
        "OPTIONS", "Layer", "BTN_GOALS", 0UL, 0UL, 0UL);
    mOptionInstances[11] = FEFinder<TLComponentInstance, 4>::Find(presentation,
        "OPTIONS", "Layer", "BTN_TIME", 0UL, 0UL, 0UL);
    int i;
    int button;
    for (button = 0, i = 0; i < 5; ++button, ++i)
    {
        char name[16];
        nlSNPrintf(name, sizeof(name), "BUTTON_%d", button);
        mOptionInstances[i] = FEFinder<TLComponentInstance, 4>::Find(mSkillSection, InlineHasher(name));
    }
    for (button = 0, i = 5; i < 10; ++button, ++i)
    {
        char name[16];
        nlSNPrintf(name, sizeof(name), "BUTTON_%d", button);
        mOptionInstances[i] = FEFinder<TLComponentInstance, 4>::Find(mSeriesSection, InlineHasher(name));
    }
    for (button = 0, i = 12; i < 20; ++button, ++i)
    {
        char name[16];
        nlSNPrintf(name, sizeof(name), "BUTTON_%d", button);
        mOptionInstances[i] = FEFinder<TLComponentInstance, 4>::Find(mGoalsSection, InlineHasher(name));
    }
    for (button = 2, i = 20; i < 24; ++button, ++i)
    {
        char name[16];
        nlSNPrintf(name, sizeof(name), "BUTTON_%d", button);
        mOptionInstances[i] = FEFinder<TLComponentInstance, 4>::Find(mMinutesSection, InlineHasher(name));
    }
    mCheatInstances[0] = FEFinder<TLComponentInstance, 4>::Find(presentation,
        "CHEATS", "Layer", "cheat_0", 0UL, 0UL, 0UL);
    mCheatInstances[1] = FEFinder<TLComponentInstance, 4>::Find(presentation,
        "CHEATS", "Layer", "cheat_1", 0UL, 0UL, 0UL);
    mCheatInstances[2] = FEFinder<TLComponentInstance, 4>::Find(presentation,
        "CHEATS", "Layer", "cheat_2", 0UL, 0UL, 0UL);
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
    InitializeSelections();
    presentation->SetActiveSlide("OPTIONS_IN", true);
}

void SHGameplayOptions::Update(float dt)
{
    BaseSceneHandler::Update(dt);
    TLSlide* slide;
    int state = mFlowState;
    if (state == 0 || (unsigned int)(state - 2) <= 2)
    {
        slide = mPresentation->m_currentSlide;
        if (slide->GetCurrentTime() < slide->GetStartTime() + slide->GetDuration())
        {
            for (int i = 0; i < 4; ++i)
                GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
            return;
        }
        if (state == 0)
        {
            SHNavigation* scene = GetNavigationScene();
            if (scene != 0)
                scene->SetButtons(79, true);
            mFlowState = 1;
            ToggleOptionsView();
        }
        else if (state == 2)
        {
            CommitSettings();
            return;
        }
        else if (state == 3)
        {
            GameInfoManager* gameInfo = GameInfoManager::Instance();
            if (gameInfo->UseAltRules())
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
                FrontEndPresentation::GetInstance()->Call("TransitionGrudgeMatchToMainMenu");
            }
            return;
        }
        else if (state == 4)
        {
            mFlowState = 0;
            mPresentation->SetActiveSlide("IN", true);
            mPresentation->Update(0.0f);
            return;
        }
    }
    if (!mInitialized)
    {
        InitializePointerButtons();
        mInitialized = true;
    }
    GameInfoManager* gameInfo = GameInfoManager::Instance();
    if (gameInfo->UseAltRules()
        && !GameSceneManager::Instance()->IsOnStack((SceneList)10)
        && g_pFriendManager->FindHostInvitation())
    {
        if (GameSceneManager::Instance()->IsOnStack(SCENE_OPTIONS_CHEATS_LIST))
            GameSceneManager::Instance()->Pop();
        FriendManager* friendManager = g_pFriendManager;
        friendManager->mReturnScene = 27;
        friendManager->mPreviousRankedMode = 0;
        GameSceneManager::Instance()->Push((SceneList)52, SCREEN_FORWARD, true);
        return;
    }
    for (int i = 0; i < 4; ++i)
    {
        TLComponentInstance* cursor = GetPointerInstance(i);
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
        g_pPadManager->GetPad(i)->GetButtonIndex(30, true);
        event.mPressed = g_pFEInput->JustPressed((eFEINPUT_PAD)i, 30, true, 0);
        for (int j = 0; j < 24; ++j)
            mOptionButtons[j].HandlePointerEvent(&event);
        for (int j = 0; j < 3; ++j)
            mCheatButtons[j].HandlePointerEvent(&event);
        mPageControls->Update(event, dt);
        if (mPageControls->IsButtonPressed(0) || mPageControls->IsButtonPressed(1))
        {
            FEAudio::PlayAnimAudioEvent(0x375C885A, 0, 0, 1);
            FEAudio::PlayAnimAudioEvent(0xEA7AD449, 0, 0, 1);
            mFlowState = 4;
            mPresentation->SetActiveSlide("OPTIONS_OUT", true);
            mPresentation->Update(0.0f);
            SHNavigation* scene = GetNavigationScene();
            if (scene != 0)
            {
                if (mPageControls->IsButtonPressed(0))
                    scene->SetButtons(9, false);
                else
                    scene->SetButtons(10, false);
            }
            return;
        }
        if (mNavigation.UpdateBackButton(event, dt))
        {
            mFlowState = 3;
            SHNavigation* scene = GetNavigationScene();
            if (scene != 0)
                scene->HideButtons();
            mPresentation->SetActiveSlide("OPTIONS_OUT", true);
            return;
        }
        mDoneButton.HandlePointerEvent(&event);
    }
}

void SHGameplayOptions::ToggleOptionsView()
{
    SHNavigation* scene = GetNavigationScene();
    if (mViewIndex == 1)
    {
        mPresentation->SetActiveSlide("OPTIONS", true);
        mPresentation->Update(0.0f);
        mViewIndex = 0;
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
        mViewIndex = 1;
        mPageControls->SetButtonState(0, true, true);
        mPageControls->SetButtonState(1, false, false);
        mPageControls->ClearButtonHighlight(0);
        SetBreadcrumbs(2, 1);
        UpdateCheatText();
        if (scene != 0)
            scene->SetButtonVisibility(1, false);
    }
    bool options = mViewIndex == 0;
    bool cheats = mViewIndex == 1;
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
    if (mViewIndex == 0)
        UpdateLimitView(mGoalLimitSelected);
}

void SHGameplayOptions::UpdateLimitView(bool value)
{
    mGoalLimitSelected = value;
    bool other = !mGoalLimitSelected;
    mGoalsSection->m_bVisible = value;
    mMinutesSection->m_bVisible = other;
    for (int i = 12; i < 20; ++i)
    {
        if (mGoalLimitSelected)
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

static inline void UpdateSkillLevelSetting(SHGameplayOptions* scene, int skill)
{
    TLComponentInstance* instance = FEFinder<TLComponentInstance, 4>::Find(scene->mPresentation,
        "OPTIONS", "Layer", "SKILL LEVEL SETTINGS", 0UL, 0UL, 0UL);
    switch (skill)
    {
    case 1: instance->SetActiveSlide("ROOKIE", true, false); break;
    case 2: instance->SetActiveSlide("PROFESSIONAL", true, false); break;
    case 3: instance->SetActiveSlide("SUPERSTAR", true, false); break;
    case 4: instance->SetActiveSlide("LEGEND", true, false); break;
    case 5: instance->SetActiveSlide("MEGASTRIKER", true, false); break;
    }
}

static inline void UpdateSeriesSetting(SHGameplayOptions* scene, int series)
{
    TLTextInstance* text = FEFinder<TLTextInstance, 3>::Find(scene->mPresentation,
        "OPTIONS", "Layer", "SERIES SETTING", 0UL, 0UL, 0UL);
    unsigned short number[4];
    nlSNPrintf(number, 4, (const unsigned short*)L"%d", series);
    WideString string = Format(WideString(LookupLocString("OPTIONS_BEST_OF")), number);
    memcpy(scene->mSeriesText, string.c_str(), sizeof(scene->mSeriesText));
    text->SetString(scene->mSeriesText);
}

void SHGameplayOptions::InitializeSelections()
{
    int skill = mSettings.SkillLevel;
    int series = mSettings.NumGames;
    int time = mSettings.GameTime;
    int goals = mSettings.GoalLimit;
    int type = mSettings.GameLimitType;
    int value = type == 1 ? goals : time / 60;
    UpdateSkillLevelSetting(this, skill);
    UpdateSeriesSetting(this, series);
    mGoalLimitSelected = type == 1;
    int selected = 11;
    if (type == 1)
        selected = 10;
    mSelectedLimitType = selected;
    mOptionInstances[selected]->SetActiveSlide("down", true, false);
    FEPointerButton* button = &mOptionButtons[mSelectedLimitType];
    for (int j = 0; j < 4; ++j)
        button->SetPointerState(2, j);
    UpdateLimitText(type, value);
    for (int i = 0; i < 5; ++i)
    {
        if (skill == sSkillLevelOptions[i])
        {
            mSelectedSkill = i;
            mOptionInstances[i]->SetActiveSlide("down", true, false);
            FEPointerButton* button = &mOptionButtons[mSelectedSkill];
            for (int j = 0; j < 4; ++j)
                button->SetPointerState(2, j);
            break;
        }
    }
    for (int i = 0; i < 5; ++i)
    {
        if (series == sNumGamesOptions[i])
        {
            mSelectedSeries = i + 5;
            mOptionInstances[mSelectedSeries]->SetActiveSlide("down", true, false);
            FEPointerButton* button = &mOptionButtons[mSelectedSeries];
            for (int j = 0; j < 4; ++j)
                button->SetPointerState(2, j);
            break;
        }
    }
    for (int i = 0; i < 8; ++i)
    {
        if (goals == sGoalLimitOptions[i])
        {
            mSelectedGoals = i + 12;
            mOptionInstances[mSelectedGoals]->SetActiveSlide("down", true, false);
            FEPointerButton* button = &mOptionButtons[mSelectedGoals];
            for (int j = 0; j < 4; ++j)
                button->SetPointerState(2, j);
            break;
        }
    }
    for (int i = 0; i < 4; ++i)
    {
        if (time == sGameTimeOptions[i])
        {
            mSelectedTime = i + 20;
            mOptionInstances[mSelectedTime]->SetActiveSlide("down", true, false);
            FEPointerButton* button = &mOptionButtons[mSelectedTime];
            for (int j = 0; j < 4; ++j)
                button->SetPointerState(2, j);
            break;
        }
    }
}

void SHGameplayOptions::ApplyOptionSelection(int item)
{
    if (item >= 0 && item < 5)
    {
        mSettings.SkillLevel = (GameplaySettings::eSkillLevel)sSkillLevelOptions[item];
        UpdateSkillLevelSetting(this, mSettings.SkillLevel);
    }
    else if (item >= 5 && item < 10)
    {
        mSettings.NumGames = sNumGamesOptions[item - 5];
        UpdateSeriesSetting(this, mSettings.NumGames);
    }
    else if (item == 10)
    {
        mSettings.GameLimitType = 1;
        UpdateLimitText(1, mSettings.GoalLimit);
    }
    else if (item == 11)
    {
        mSettings.GameLimitType = 0;
        UpdateLimitText(0, mSettings.GameTime / 60);
    }
    else if (item >= 12 && item < 20)
    {
        mSettings.GoalLimit = sGoalLimitOptions[item - 12];
        UpdateLimitText(1, mSettings.GoalLimit);
    }
    else if (item >= 20 && item < 24)
    {
        mSettings.GameTime = sGameTimeOptions[item - 20];
        UpdateLimitText(0, mSettings.GameTime / 60);
    }
}

void SHGameplayOptions::UpdateLimitText(int type, int value)
{
    TLComponentInstance* instance = FEFinder<TLComponentInstance, 4>::Find<>(mPresentation,
        nlStringLowerHash("OPTIONS"), nlStringLowerHash("Layer"), nlStringLowerHash("GAMEPLAYOPTIONS SETTING"), 0, 0, 0);
    TLTextInstance* text = FEFinder<TLTextInstance, 3>::Find<>(instance->GetActiveSlide(), "GAMEPLAY OPTIONS SETTING");
    unsigned short number[4];
    nlSNPrintf(number, 4, (const unsigned short*)L"%d", value);
    const char* id = "X_GOALS";
    if (type == 0)
        id = "X_MINUTES";
    WideString string = Format(WideString(LookupLocString(id)), number);
    memcpy(mLimitText, string.c_str(), sizeof(mLimitText));
    text->SetString(mLimitText);
}

void SHGameplayOptions::InitializePointerButtons()
{
    typedef Detail::MemFunImpl<void, void (SHGameplayOptions::*)(unsigned int, void*)> PointerMethod;
    typedef BindExp3<void, PointerMethod, SHGameplayOptions*, Placeholder<0>, Placeholder<1> > PointerBinding;

    FEPointerListener::Callback over(PointerBinding(MemFun(&SHGameplayOptions::OnOptionPointerEnter), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback off(PointerBinding(MemFun(&SHGameplayOptions::OnOptionPointerLeave), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback down(PointerBinding(MemFun(&SHGameplayOptions::OnOptionPointerPress), this, Placeholder<0>(), Placeholder<1>()));
    feVector3 skill = mSkillSection->GetAssetPosition();
    feVector3 series = mSeriesSection->GetAssetPosition();
    feVector3 minutes = mMinutesSection->GetAssetPosition();
    feVector3 goals = mGoalsSection->GetAssetPosition();
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
    FEPointerListener::Callback cheatOver(PointerBinding(MemFun(&SHGameplayOptions::OnCheatPointerEnter), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback cheatOff(PointerBinding(MemFun(&SHGameplayOptions::OnCheatPointerLeave), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback cheatDown(PointerBinding(MemFun(&SHGameplayOptions::OnCheatPointerPress), this, Placeholder<0>(), Placeholder<1>()));
    for (int i = 0; i < 3; ++i)
    {
        TLInstance* instance = FEFinder<TLInstance, 2>::Find(mCheatInstances[i],
            "off", "CHALLENGE_0", "list_back_480x70 ");
        feVector3 position = mCheatInstances[i]->GetAssetPosition();
        mCheatButtons[i].SetInstanceBounds(instance, true, position.f.x, position.f.y, 0.95f, 0.75f);
        mCheatButtons[i].SetPointerEnterCallback(cheatOver);
        mCheatButtons[i].SetPointerLeaveCallback(cheatOff);
        mCheatButtons[i].SetPointerPressCallback(cheatDown);
    }
    FEPointerListener::Callback nextOver(PointerBinding(MemFun(&SHGameplayOptions::OnDonePointerEnter), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback nextOff(PointerBinding(MemFun(&SHGameplayOptions::OnDonePointerLeave), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback nextDown(PointerBinding(MemFun(&SHGameplayOptions::OnDonePointerPress), this, Placeholder<0>(), Placeholder<1>()));
    SetDoneButtonBounds(&mDoneButton, mDoneButtonInstance, true);
    mDoneButton.SetPointerEnterCallback(nextOver);
    mDoneButton.SetPointerLeaveCallback(nextOff);
    mDoneButton.SetPointerPressCallback(nextDown);
}

void SHGameplayOptions::OnOptionPointerEnter(unsigned int index, void* context)
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

void SHGameplayOptions::OnOptionPointerLeave(unsigned int index, void* context)
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

void SHGameplayOptions::OnOptionPointerPress(unsigned int index, void* context)
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
        previous = mSelectedSkill;
        mSelectedSkill = item;
        FEAudio::PlayAnimAudioEvent(0xF0AFD586, 0, 0, 1);
    }
    else if (item >= 5 && item < 10)
    {
        previous = mSelectedSeries;
        mSelectedSeries = item;
        FEAudio::PlayAnimAudioEvent(0xF0AFD586, 0, 0, 1);
    }
    else if (item == 10 || item == 11)
    {
        UpdateLimitView(item == 10);
        previous = mSelectedLimitType;
        mSelectedLimitType = item;
        FEAudio::PlayAnimAudioEvent(0x362F2841, 0, 0, 1);
    }
    else if (item >= 12 && item < 20)
    {
        previous = mSelectedGoals;
        mSelectedGoals = item;
        FEAudio::PlayAnimAudioEvent(0xF0AFD586, 0, 0, 1);
    }
    else if (item >= 20 && item < 24)
    {
        previous = mSelectedTime;
        mSelectedTime = item;
        FEAudio::PlayAnimAudioEvent(0xF0AFD586, 0, 0, 1);
    }
    ApplyOptionSelection(item);
    mOptionInstances[previous]->SetActiveSlide("off", true, false);
    for (int j = 0; j < 4; ++j)
        mOptionButtons[previous].SetPointerState(0, j);
}

void SHGameplayOptions::OnCheatPointerEnter(unsigned int index, void* context)
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

void SHGameplayOptions::OnCheatPointerLeave(unsigned int index, void* context)
{
    unsigned int item = (unsigned int)context;
    --mPointerInsideCounts[index];
    if (!mCheatButtons[item].HasOtherPointerState(1, index))
    {
        mCheatInstances[item]->SetActiveSlide("off", true, false);
        mCheatButtons[item].SetPointerState(1, index);
    }
}

void SHGameplayOptions::OnCheatPointerPress(unsigned int index, void* context)
{
    SHOptionsCheatsList* scene = (SHOptionsCheatsList*)GameSceneManager::Instance()->Push(SCENE_OPTIONS_CHEATS_LIST, SCREEN_NOTHING, false);
    scene->mCheatCategory = (int)context;
    scene->mSettings = &mPowerupSettings;
    FEAudio::PlayAnimAudioEvent(0xF0AFD586, 0, 0, 1);
    FEAudio::PlayAnimAudioEvent(0xBB142B94, 0, 0, 1);
}

void SHGameplayOptions::OnDonePointerEnter(unsigned int index, void* context)
{
    ++mPointerInsideCounts[index];
    mDoneButton.SetPointerState(1, index);
    if (!mDoneButton.HasOtherPointerState(1, index))
    {
        mDoneButtonInstance->SetActiveSlide("over", true, false);
        FEAudio::PlayAnimAudioEvent(0xAA73EF33, 0, 0, 1);
    }
}

void SHGameplayOptions::OnDonePointerLeave(unsigned int index, void* context)
{
    --mPointerInsideCounts[index];
    mDoneButton.SetPointerState(0, index);
    if (!mDoneButton.HasOtherPointerState(1, index))
        mDoneButtonInstance->SetActiveSlide("off", true, false);
}

void SHGameplayOptions::OnDonePointerPress(unsigned int index, void* context)
{
    mFlowState = 2;
    mPresentation->SetActiveSlide("OPTIONS_OUT", true);
    SHNavigation* scene = GetNavigationScene();
    if (scene != 0)
        scene->SetButtons(64, true);
    mDoneButtonInstance->SetActiveSlide("down", true, false);
    FEAudio::PlayAnimAudioEvent(0xF0AFD586, 0, 0, 1);
    GameInfoManager* gameInfo = GameInfoManager::Instance();
    if (gameInfo->UseAltRules())
        FEAudio::PlayAnimAudioEvent(0x64B85E8D, 0, 0, 1);
    else
        FEAudio::PlayAnimAudioEvent(0x5BCD337B, 0, 0, 1);
}

void SHGameplayOptions::CommitSettings()
{
    GameInfoManager* gameInfo = GameInfoManager::Instance();
    if (gameInfo->UseAltRules())
    {
        gameInfo->mUserInfo.mAltGameplayOptions = mSettings;
        GameInfoManager::Instance()->mNoCheatSettings = mSettings;
        GameInfoManager::Instance()->mUserInfo.mAltCheatOptions = mPowerupSettings;
        GameInfoManager::Instance()->mRulesA = mPowerupSettings;
        GameSceneManager::Instance()->Push((SceneList)5, SCREEN_FORWARD, true);
    }
    else
    {
        gameInfo->mUserInfo.mGameplayOptions = mSettings;
        GameInfoManager::Instance()->mUserInfo.mCheatOptions = mPowerupSettings;
        GameSceneManager::Instance()->Push((SceneList)2, SCREEN_FORWARD, true);
    }
}

void SHGameplayOptions::UpdateCheatText()
{
    const char* strings[3][2];
    strings[0][0] = GetLOCEnvironmentCheatName(mPowerupSettings.mEnvironmentCheat);
    strings[0][1] = GetLOCEnvironmentCheatDescription(mPowerupSettings.mEnvironmentCheat);
    strings[1][0] = GetLOCPowerupCheatName(mPowerupSettings.mCustomPowerups);
    strings[1][1] = GetLOCPowerupCheatDescription(mPowerupSettings.mCustomPowerups);
    strings[2][0] = GetLOCPlayerCheatName(mPowerupSettings.mPlayerCheat);
    strings[2][1] = GetLOCPlayerCheatDescription(mPowerupSettings.mPlayerCheat);
    const char* const slides[3] = { "off", "over", "down" };
    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            TLTextInstance* text0 = FEFinder<TLTextInstance, 3>::Find(mCheatInstances[i],
                slides[j], "CHALLENGE_0", "stat_0");
            TLTextInstance* text1 = FEFinder<TLTextInstance, 3>::Find(mCheatInstances[i],
                slides[j], "CHALLENGE_0", "stat_1");
            text0->SetStringId(strings[i][0]);
            text1->SetStringId(strings[i][1]);
        }
    }
}
