#include "Game/SH/SHChallengeSelect.h"
#include "Game/FE/fePresentation.inl"
#include "Game/FE/feHelpFuncs_decl.h"
#include "Game/FE/FEAudio.h"

#include "Game/BaseSceneHandler.h"
#include "Game/GameSceneManager.h"
#include "Game/DB/CharacterInfo.h"
#include "Game/DB/GameProgress.h"
#include "Game/FE/feFinder.inl"
#include "Game/FE/feInput.h"
#include "NL/globalpad.h"
#include "Game/FE/feMusic.h"
#include "Game/FE/fePresentation.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlImageInstance.h"
#include "Game/FE/tlTextInstance.h"
#include "Game/Render/FrontEndPresentation.h"
#include "Game/SH/SHStrikerTimesBase.h"
#include "NL/nlBind.h"
#include "NL/nlFunction.inl"
#include "NL/nlFormat.h"
#include "NL/nlLocalization.h"
#include "NL/nlMemory.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"
#include "Game/FE/fePointerButton.h"
#include "Game/FE/feDPD.h"
#include "Game/FE/feBackButton.h"
#include "Game/FE/feScrollBar.h"
#include "Game/SH/SHNavigation.h"

ChallengeSelectScene::~ChallengeSelectScene()
{
}

ChallengeSelectScene::ChallengeSelectScene(bool tutorial)
    : mChallengeOffset(0)
    , mPointerInsideCount(0)
    , mButtonsInitialized(false)
    , mTutorial(tutorial)
{
    mState = 0;
    mChallengeButtons[0].mContext = 0;
    mChallengeButtons[0].mSpeakerEnabled = false;
    mChallengeButtons[1].mContext = (void*)1;
    mChallengeButtons[1].mSpeakerEnabled = false;
    mChallengeButtons[2].mContext = (void*)2;
    mChallengeButtons[2].mSpeakerEnabled = false;
    int challengeCount = mTutorial ? 10 : 12;
    mMaxScrollOffset = challengeCount - 3;
    nlSNPrintf(mChallengeCountText, 8, (const unsigned short*)L"%d", challengeCount);
    mBackButton.SetPopScene(false);
    g_pStrikerChallenge->mHeadlineVariant = -1;
}

void ChallengeSelectScene::SceneCreated()
{
    for (int i = 0; i < 3; ++i)
    {
        char challengeName[16];
        nlSNPrintf(challengeName, sizeof(challengeName), "CHALLENGE_%d", i);

        TLComponentInstance* challenge = FEFinder<TLComponentInstance, 4>::FindOrDefault(
            mPresentation->GetActiveSlide(), "Layer", "challenge_screen",
            challengeName);
        mChallengeSlides[i] = challenge;
    }
    TLComponentInstance* scrollbar = FEFinder<TLComponentInstance, 4>::FindOrDefault(
        mPresentation->GetActiveSlide(), "Layer", "challenge_screen",
        "scrollbar");
    mScrollBar.SetComponent(scrollbar);
    mScrollBar.SetRange(mMaxScrollOffset);
    mScrollBar.SetValue(mChallengeOffset);

    UpdateRows();

    for (int i = 0; i < 4; ++i)
    {
        GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
    }

    TLComponentInstance* menuComponent = 0;
    SHNavigation* menu = GetNavigationScene();
    if (menu != 0)
    {
        menu->HideButtons();
        menuComponent = menu->GetButton(4);
    }
    mBackButton.SetButtonInstance(menuComponent);
    mBackButton.SetPushBackScene(false);

    if (mTutorial)
    {
        FEMusic::StartStreamIfDifferent(11);
    }
    else
    {
        FEMusic::StartStreamIfDifferent(10);
    }
    TLComponentInstance* title = FEFinder<TLComponentInstance, 4>::FindOrDefault(
        mPresentation->GetActiveSlide(), "Layer", "challenge_screen",
        "TITLE");
    if (mTutorial)
    {
        title->SetActiveSlide("tutorials", true, false);
    }
    else
    {
        title->SetActiveSlide("challenges", true, false);
    }

    FEAudio::PlayAnimAudioEvent(0xBB142B94, 0, 0, 1);
}

void ChallengeSelectScene::UpdateRow(int index)
{
    int challenge = mTutorial ? index + mChallengeOffset : index + mChallengeOffset + 10;
    const CharacterInfo& character = GetCharacterInfo(
        GetCharacterIndexFromCaptain(fn_801CA670()->GetCaptain(challenge)));
    const char* config = fn_801CA670()->GetConfigPath(challenge);
    TLInstance* off = FEFinder<TLInstance, 5>::FindOrDefault(
        mChallengeSlides[index], "off", "CHALLENGE_0");
    TLInstance* over = FEFinder<TLInstance, 5>::FindOrDefault(
        mChallengeSlides[index], "over", "CHALLENGE_0");

    char name[24];
    char textureName[24];
    if (challenge == 2)
    {
        nlSNPrintf(name, sizeof(name), "kritter");
        nlSNPrintf(textureName, sizeof(textureName), "captain_0_kritter_S");
    }
    else if (challenge == 4)
    {
        nlSNPrintf(name, sizeof(name), "hammerbros");
        nlSNPrintf(textureName, sizeof(textureName), "sidekick_5_hammer_S");
    }
    else if (challenge == 5)
    {
        nlSNPrintf(name, sizeof(name), "boo");
        nlSNPrintf(textureName, sizeof(textureName), "sidekick_3_boo_S");
    }
    else
    {
        nlSNPrintf(name, sizeof(name), "%s", character.GetName());
        nlSNPrintf(textureName, sizeof(textureName), "captain_%s_s", name);
    }

    FETextureResource* texture = FEFinder<TLImageInstance, 2>::FindOrDefault(GetPresentation(),
        "art", "Layer", textureName)->GetTextureResource();
    FEFinder<TLImageInstance, 2>::FindOrDefault(off, "00_dummy_texture")->SetTextureResource(texture);
    FEFinder<TLImageInstance, 2>::FindOrDefault(over, "00_dummy_texture")->SetTextureResource(texture);

    TLComponentInstance* completedOff = static_cast<TLComponentInstance*>(FEFinder<TLInstance, 4>::FindOrDefault(off, "COMPLETED"));
    TLComponentInstance* completedOver = static_cast<TLComponentInstance*>(FEFinder<TLInstance, 4>::FindOrDefault(over, "COMPLETED"));
    TLImageInstance* backgroundOff = FEFinder<TLImageInstance, 2>::FindOrDefault(off, "list_back_480x70 ");
    TLImageInstance* backgroundOver = FEFinder<TLImageInstance, 2>::FindOrDefault(over, "list_back_480x70 ");
    TLComponentInstance* lockedOff = static_cast<TLComponentInstance*>(FEFinder<TLInstance, 4>::FindOrDefault(off, "locked"));
    TLComponentInstance* lockedOver = static_cast<TLComponentInstance*>(FEFinder<TLInstance, 4>::FindOrDefault(over, "locked"));
    if (IsChallengeAvailable(index + mChallengeOffset))
    {
        backgroundOff->SetVisible(false);
        backgroundOver->SetVisible(false);
        completedOff->SetVisible(fn_801CA670()->IsUnlocked(challenge));
        completedOver->SetVisible(fn_801CA670()->IsUnlocked(challenge));
        lockedOff->SetVisible(false);
        lockedOver->SetVisible(false);
    }
    else
    {
        completedOff->SetVisible(false);
        completedOver->SetVisible(false);
        backgroundOff->SetVisible(true);
        backgroundOver->SetVisible(true);
        lockedOff->SetVisible(true);
        lockedOver->SetVisible(true);
    }

    static_cast<TLComponentInstance*>(FEFinder<TLInstance, 4>::FindOrDefault(off, "NAMES"))->SetActiveSlide(name, true, false);
    static_cast<TLComponentInstance*>(FEFinder<TLInstance, 4>::FindOrDefault(over, "NAMES"))->SetActiveSlide(name, true, false);

    const char* difficulty = fn_801CA670()->GetDifficulty(challenge);
    TLComponentInstance* title = static_cast<TLComponentInstance*>(FEFinder<TLInstance, 4>::FindOrDefault(off, "CHALLENGE_TITLE"));
    FEFinder<TLTextInstance, 3>::FindOrDefault(title->GetActiveSlide(), "CHALLENGE_TITLE")->SetStringId(difficulty);
    title = static_cast<TLComponentInstance*>(FEFinder<TLInstance, 4>::FindOrDefault(over, "CHALLENGE_TITLE"));
    FEFinder<TLTextInstance, 3>::FindOrDefault(title->GetActiveSlide(), "CHALLENGE_TITLE")->SetStringId(difficulty);

    TLComponentInstance* difficultyOff = static_cast<TLComponentInstance*>(FEFinder<TLInstance, 4>::FindOrDefault(off, "DIFFICULTY"));
    TLComponentInstance* difficultyOver = static_cast<TLComponentInstance*>(FEFinder<TLInstance, 4>::FindOrDefault(over, "DIFFICULTY"));
    if (mTutorial)
    {
        difficultyOff->SetVisible(false);
        difficultyOver->SetVisible(false);
        FEFinder<TLTextInstance, 3>::FindOrDefault(off, "DIFFICULTY_TITLE")->SetVisible(false);
        FEFinder<TLTextInstance, 3>::FindOrDefault(over, "DIFFICULTY_TITLE")->SetVisible(false);
    }
    else
    {
        difficultyOff->SetActiveSlide(config, true, false);
        difficultyOver->SetActiveSlide(config, true, false);
    }

    unsigned short number[4];
    nlSNPrintf(number, 4, (const unsigned short*)L"%d", index + mChallengeOffset + 1);
    WideBasicString text = Format(
        WideBasicString(g_pLocalization->GetString("X_SLASH_X")), number, mChallengeCountText);
    nlStrNCpy<unsigned short>(mChallengeNumbers[index], text.c_str(), 8);
    TLTextInstance* numberOff = FEFinder<TLTextInstance, 3>::FindOrDefault(off, "number");
    numberOff->SetString(mChallengeNumbers[index]);
    TLTextInstance* numberOver = FEFinder<TLTextInstance, 3>::FindOrDefault(over, "number");
    numberOver->SetString(mChallengeNumbers[index]);
}

void ChallengeSelectScene::Update(float dt)
{
    BaseSceneHandler::Update(dt);
    if (mState == 0 || mState == 2 || mState == 3)
    {
        TLSlide* slide = mPresentation->GetActiveSlide();
        if (slide->GetCurrentTime() < slide->GetStartTime() + slide->GetDuration())
        {
            for (int i = 0; i < 4; ++i)
            {
                GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
            }
            return;
        }

        if (mState == 0)
        {
            if (!mButtonsInitialized)
            {
                GetNavigationScene()->SetButtons(4, true);
                InitializeButtons();
                mButtonsInitialized = true;
            }
            mState = 1;
        }
        else if (mState == 2)
        {
            SHStrikerTimesBase* scene = static_cast<SHStrikerTimesBase*>(
                GameSceneManager::Instance()->Push((SceneList)0x4D, SCREEN_NOTHING, true));
            g_pStrikerChallenge->mUnidentified6C = mChallengeOffset;
            if (scene != 0)
            {
                scene->SetDisplayMode(8);
            }
            return;
        }
        else if (mState == 3)
        {
            FEAudio::PlayAnimAudioEvent(0x4430B152, 0, 0, 1);
            FrontEndPresentation::GetInstance()->Call("TransitionChallengesToMainMenu");
            GameSceneManager::Instance()->Pop();
            return;
        }
    }

    for (int i = 0; i < 4; ++i)
    {
        TLComponentInstance* cursor = GetPointerInstance(i);
        if ((unsigned int)i != gFEControllerIndex)
        {
            cursor->SetActiveSlide("waiting", true, false);
            continue;
        }
        if (mPointerInsideCount > 0 || mBackButton.mPointerInside[i])
        {
            cursor->SetActiveSlide("A", true, false);
        }
        else
        {
            cursor->SetActiveSlide("cursor", true, false);
        }

        u8 valid = true;
        FEPointerEvent event;
        event.mIndex = i;
        event.mPosition = GetPointerPosition(i, &valid);
        g_pPadManager->GetPad(i)->GetButtonIndex(0x1E, true);
        event.mPressed = g_pFEInput->JustPressed((eFEINPUT_PAD)i, 0x1E, true, 0);
        event.mReleased = g_pFEInput->JustReleased((eFEINPUT_PAD)i, 0x1E, true, 0);
        mScrollBar.Update(event, dt);
        for (int j = 0; j < 3; ++j)
        {
            mChallengeButtons[j].HandlePointerEvent(&event);
        }
        if (mBackButton.UpdateBackButton(event, dt))
        {
            mState = 3;
            GetNavigationScene()->HideButtons();
            mPresentation->SetActiveSlide("out", true);
            return;
        }
    }

    if (mScrollBar.IsScrolling(1, 1))
    {
        ++mChallengeOffset;
        UpdateRows();
    }
    else if (mScrollBar.IsScrolling(0, 1))
    {
        --mChallengeOffset;
        UpdateRows();
    }
}

void ChallengeSelectScene::InitializeButtons()
{
    typedef Detail::MemFunImpl<void, void (ChallengeSelectScene::*)(int, void*)> PointerMethod;
    typedef BindExp3<void, PointerMethod, ChallengeSelectScene*, Placeholder<0>, Placeholder<1> > PointerBinding;

    FEPointerListener::Callback onEnter(
        PointerBinding(MemFun(&ChallengeSelectScene::OnChallengeEnter), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback onLeave(
        PointerBinding(MemFun(&ChallengeSelectScene::OnChallengeLeave), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback onInside(
        PointerBinding(MemFun(&ChallengeSelectScene::OnChallengeInside), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback onPressed(
        PointerBinding(MemFun(&ChallengeSelectScene::OnChallengePressed), this, Placeholder<0>(), Placeholder<1>()));

    for (int i = 0; i < 3; ++i)
    {
        TLImageInstance* image = FEFinder<TLImageInstance, 2>::FindOrDefault(mChallengeSlides[i],
            "off", "CHALLENGE_0",
            "list_back_480x90");
        feVector3 position = mChallengeSlides[i]->GetAssetPosition();
        mChallengeButtons[i].SetInstanceBounds(image, true, position.f.x, position.f.y, 0.95f, 0.75f);
        mChallengeButtons[i].SetPointerEnterCallback(onEnter);
        mChallengeButtons[i].SetPointerLeaveCallback(onLeave);
        mChallengeButtons[i].SetPointerInsideCallback(onInside);
        mChallengeButtons[i].SetPointerPressCallback(onPressed);
    }
    if (!mScrollBar.mInitialized)
    {
        mScrollBar.Initialize();
    }
}

void ChallengeSelectScene::OnChallengePressed(int, void* context)
{
    StrikerChallenge* challengeData = g_pStrikerChallenge;
    int challenge;
    if (mTutorial)
    {
        challenge = mChallengeOffset + (int)context;
    }
    else
    {
        challenge = mChallengeOffset + (int)context + 10;
    }
    challengeData->SetCurrentChallenge(challenge);

    FEAudio::PlayAnimAudioEvent(0xF0AFD586, 0, 0, 1);
    mState = 2;
    GetNavigationScene()->HideButtons();
    mPresentation->SetActiveSlide("out", true);
}

void ChallengeSelectScene::OnChallengeEnter(int pointerIndex, void* context)
{
    int row = (int)context;
    ++mPointerInsideCount;
    mChallengeSlides[row]->SetActiveSlide("over", true, false);
    mChallengeButtons[row].SetPointerState(1, pointerIndex);
    FEAudio::PlayAnimAudioEvent(0xF6EB899E, 0, 0, 1);
    mChallengeButtons[row].PlayHoverFeedback(pointerIndex);
}

void ChallengeSelectScene::OnChallengeLeave(int pointerIndex, void* context)
{
    int row = (int)context;
    --mPointerInsideCount;
    mChallengeSlides[row]->SetActiveSlide("off", true, false);
    mChallengeButtons[row].SetPointerState(0, pointerIndex);
}

void ChallengeSelectScene::OnChallengeInside(int pointerIndex, void* context)
{
    int row = (int)context;
    if (mChallengeButtons[row].GetPointerState(pointerIndex) == 0)
    {
        ++mPointerInsideCount;
        mChallengeSlides[row]->SetActiveSlide("over", true, false);
        mChallengeButtons[row].SetPointerState(1, pointerIndex);
        FEAudio::PlayAnimAudioEvent(0xF6EB899E, 0, 0, 1);
        mChallengeButtons[row].PlayHoverFeedback(pointerIndex);
    }
}

bool ChallengeSelectScene::IsChallengeAvailable(int)
{
    return true;
}

#include "NL/nlstring_tmpl.h"
