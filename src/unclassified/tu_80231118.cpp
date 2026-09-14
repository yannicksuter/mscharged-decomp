#include "unclassified/tu_80231118.h"
#include "Game/FE/fePresentation.inl"
#include "Game/FE/feHelpFuncs_decl.h"
#include "Game/FE/FEAudio.h"

#include "Game/BaseSceneHandler.h"
#include "Game/GameSceneManager.h"
#include "Game/DB/CharacterInfo.h"
#include "Game/DB/GameProgress.h"
#include "Game/FE/feFinder.h"
#include "Game/FE/feInput.h"
#include "Game/FE/feMusic.h"
#include "Game/FE/fePresentation.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlImageInstance.h"
#include "Game/FE/tlTextInstance.h"
#include "Game/Render/Presentation.h"
#include "Game/SH/SHStrikerTimesBase.h"
#include "NL/nlBind.h"
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


extern "C" TLComponentInstance* fn_801F9FEC(TLInstance* pTopLevel, InlineHasher Level1,
    InlineHasher Level2 = InlineHasher(0UL), InlineHasher Level3 = InlineHasher(0UL),
    InlineHasher Level4 = InlineHasher(0UL), InlineHasher Level5 = InlineHasher(0UL),
    InlineHasher Level6 = InlineHasher(0UL));



extern char lbl_8051D6D0[] = "challenge_screen";
extern char lbl_8051D6E4[] = "CHALLENGE_%d";
extern char lbl_8051D6F4[] = "scrollbar";
extern char lbl_8051D700[] = "tutorials";
extern char lbl_8051D70C[] = "challenges";
extern char lbl_8051D718[] = "CHALLENGE_0";
extern char lbl_8051D724[] = "captain_0_kritter_S";
extern char lbl_8051D738[] = "hammerbros";
extern char lbl_8051D744[] = "sidekick_5_hammer_S";
extern char lbl_8051D758[] = "sidekick_3_boo_S";
extern char lbl_8051D76C[] = "captain_%s_s";
extern char lbl_8051D77C[] = "00_dummy_texture";
extern char lbl_8051D790[] = "COMPLETED";
extern char lbl_8051D79C[] = "list_back_480x70 ";
extern char lbl_8051D7B0[] = "CHALLENGE_TITLE";
extern char lbl_8051D7C0[] = "DIFFICULTY";
extern char lbl_8051D7CC[] = "DIFFICULTY_TITLE";
extern char lbl_8051D7E0[] = "X_SLASH_X";
extern char lbl_8051D7EC[] = "TransitionChallengesToMainMenu";
extern unsigned short lbl_806DE2B0[] = { '%', 'd', 0 };
extern char lbl_806DE2B8[] = "Layer";
extern char lbl_806DE2C0[] = "waiting";
extern char lbl_806DE2C8[] = "TITLE";
extern char lbl_806DE2D0[] = "off";
extern char lbl_806DE2D4[] = "over";
extern char lbl_806DE2E0[] = "kritter";
extern char lbl_806DE2E8[] = "boo";
extern char lbl_806DE2EC[] = "%s";
extern char lbl_806DE2F0[] = "art";
extern char lbl_806DE2F4[] = "locked";
extern char lbl_806DE2FC[] = "NAMES";
extern char lbl_806DE304[] = "number";
extern char lbl_806DE30C[] = "A";
extern char lbl_806DE310[] = "cursor";
extern char lbl_806DE318[] = "out";

TU80231118Scene::~TU80231118Scene()
{
}

void TU80231118Scene::fn_80231198(int, void* context)
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
    mPresentation->SetActiveSlide(lbl_806DE318, true);
}

void TU80231118Scene::fn_80231224(int index, void* context)
{
    int challenge = (int)context;
    ++mActiveCount;
    mChallengeSlides[challenge]->SetActiveSlide(lbl_806DE2D4, true, false);
    mChallengeComponents[challenge].SetPointerState(1, index);
    FEAudio::PlayAnimAudioEvent(0xF6EB899E, 0, 0, 1);
    mChallengeComponents[challenge].PlayHoverFeedback(index);
}

void TU80231118Scene::fn_802312CC(int index, void* context)
{
    int challenge = (int)context;
    --mActiveCount;
    mChallengeSlides[challenge]->SetActiveSlide(lbl_806DE2D0, true, false);
    mChallengeComponents[challenge].SetPointerState(0, index);
}

void TU80231118Scene::fn_8023134C(int index, void* context)
{
    int challenge = (int)context;
    if (mChallengeComponents[challenge].GetPointerState(index) == 0)
    {
        ++mActiveCount;
        mChallengeSlides[challenge]->SetActiveSlide(lbl_806DE2D4, true, false);
        mChallengeComponents[challenge].SetPointerState(1, index);
        FEAudio::PlayAnimAudioEvent(0xF6EB899E, 0, 0, 1);
        mChallengeComponents[challenge].PlayHoverFeedback(index);
    }
}

bool TU80231118Scene::fn_80231404(int)
{
    return true;
}

TU80231118Scene::TU80231118Scene(bool tutorial)
    : mChallengeOffset(0)
    , mActiveCount(0)
    , mPrepared(false)
    , mTutorial(tutorial)
{
    mState = 0;
    mChallengeComponents[0].mContext = 0;
    mChallengeComponents[0].mSpeakerEnabled = false;
    mChallengeComponents[1].mContext = (void*)1;
    mChallengeComponents[1].mSpeakerEnabled = false;
    mChallengeComponents[2].mContext = (void*)2;
    mChallengeComponents[2].mSpeakerEnabled = false;
    int challengeCount = mTutorial ? 10 : 12;
    mChallengeCount = challengeCount - 3;
    nlSNPrintf(mLayerName, 8, lbl_806DE2B0, challengeCount);
    mNavigation.SetPopScene(false);
    g_pStrikerChallenge->mHeadlineVariant = -1;
}

void TU80231118Scene::SceneCreated()
{
    for (int i = 0; i < 3; ++i)
    {
        char challengeName[16];
        nlSNPrintf(challengeName, sizeof(challengeName), lbl_8051D6E4, i);

        TLComponentInstance* challenge = FEFinder<TLComponentInstance, 3>::Find(
            mPresentation->GetActiveSlide(), nlStringLowerHash(lbl_806DE2B8), nlStringLowerHash(lbl_8051D6D0),
            nlStringLowerHash(challengeName), 0, 0, 0);
        if (challenge == 0)
        {
            challenge = &UnidentifiedTLComponentDefault::sInstance;
        }
        mChallengeSlides[i] = challenge;
    }
    TLComponentInstance* scrollbar = FEFinder<TLComponentInstance, 3>::Find(
        mPresentation->GetActiveSlide(), nlStringLowerHash(lbl_806DE2B8), nlStringLowerHash(lbl_8051D6D0),
        nlStringLowerHash(lbl_8051D6F4), 0, 0, 0);
    mScrollWidget.SetComponent(scrollbar != 0 ? scrollbar : &UnidentifiedTLComponentDefault::sInstance);
    mScrollWidget.SetRange(mChallengeCount);
    mScrollWidget.SetValue(mChallengeOffset);

    for (int i = 0; i < 3; ++i)
    {
        fn_80231958(i);
    }

    for (int i = 0; i < 4; ++i)
    {
        gFEPointerInstances[i]->SetActiveSlide(lbl_806DE2C0, true, false);
    }

    TLComponentInstance* menuComponent = 0;
    SHNavigation* menu = GetNavigationScene();
    if (menu != 0)
    {
        menu->HideButtons();
        menuComponent = menu->GetButton(4);
    }
    mNavigation.SetButtonInstance(menuComponent);
    mNavigation.SetPushBackScene(false);

    if (mTutorial)
    {
        FEMusic::StartStreamIfDifferent(11);
    }
    else
    {
        FEMusic::StartStreamIfDifferent(10);
    }
    TLComponentInstance* title = FEFinder<TLComponentInstance, 3>::Find(
        mPresentation->GetActiveSlide(), nlStringLowerHash(lbl_806DE2B8), nlStringLowerHash(lbl_8051D6D0),
        nlStringLowerHash(lbl_806DE2C8), 0, 0, 0);
    if (title == 0)
    {
        title = &UnidentifiedTLComponentDefault::sInstance;
    }
    if (mTutorial)
    {
        title->SetActiveSlide(lbl_8051D700, true, false);
    }
    else
    {
        title->SetActiveSlide(lbl_8051D70C, true, false);
    }

    FEAudio::PlayAnimAudioEvent(0xBB142B94, 0, 0, 1);
}

void TU80231118Scene::fn_80231958(int index)
{
    int challenge = mTutorial ? index + mChallengeOffset : index + mChallengeOffset + 10;
    const CharacterInfo& character = GetCharacterInfo(
        GetCharacterIndexFromCaptain(fn_801CA670()->GetCaptain(challenge)));
    const char* config = fn_801CA670()->GetConfigPath(challenge);
    TLInstance* off = FEFinder<TLInstance, 5>::FindOrDefault(
        mChallengeSlides[index], lbl_806DE2D0, lbl_8051D718);
    TLInstance* over = FEFinder<TLInstance, 5>::FindOrDefault(
        mChallengeSlides[index], lbl_806DE2D4, lbl_8051D718);

    char name[24];
    char textureName[24];
    if (challenge == 2)
    {
        nlSNPrintf(name, sizeof(name), lbl_806DE2E0);
        nlSNPrintf(textureName, sizeof(textureName), lbl_8051D724);
    }
    else if (challenge == 4)
    {
        nlSNPrintf(name, sizeof(name), lbl_8051D738);
        nlSNPrintf(textureName, sizeof(textureName), lbl_8051D744);
    }
    else if (challenge == 5)
    {
        nlSNPrintf(name, sizeof(name), lbl_806DE2E8);
        nlSNPrintf(textureName, sizeof(textureName), lbl_8051D758);
    }
    else
    {
        nlSNPrintf(name, sizeof(name), lbl_806DE2EC, character.GetName());
        nlSNPrintf(textureName, sizeof(textureName), lbl_8051D76C, name);
    }

    FETextureResource* texture = FEFinder<TLImageInstance, 2>::FindOrDefault(GetPresentation(),
        lbl_806DE2F0, lbl_806DE2B8, textureName)->fn_802332D0();
    FEFinder<TLImageInstance, 2>::FindOrDefault(off, lbl_8051D77C)->fn_802332D8(texture);
    FEFinder<TLImageInstance, 2>::FindOrDefault(over, lbl_8051D77C)->fn_802332D8(texture);

    TLComponentInstance* completedOff = fn_801F9FEC(off, lbl_8051D790);
    TLComponentInstance* completedOver = fn_801F9FEC(over, lbl_8051D790);
    TLImageInstance* backgroundOff = FEFinder<TLImageInstance, 2>::FindOrDefault(off, lbl_8051D79C);
    TLImageInstance* backgroundOver = FEFinder<TLImageInstance, 2>::FindOrDefault(over, lbl_8051D79C);
    TLComponentInstance* lockedOff = fn_801F9FEC(off, lbl_806DE2F4);
    TLComponentInstance* lockedOver = fn_801F9FEC(over, lbl_806DE2F4);
    if (fn_80231404(index + mChallengeOffset))
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

    fn_801F9FEC(off, lbl_806DE2FC)->SetActiveSlide(name, true, false);
    fn_801F9FEC(over, lbl_806DE2FC)->SetActiveSlide(name, true, false);

    const char* difficulty = fn_801CA670()->GetDifficulty(challenge);
    TLComponentInstance* titleOff = fn_801F9FEC(off, lbl_8051D7B0);
    FEFinder<TLTextInstance, 3>::FindOrDefault(titleOff->GetActiveSlide(), lbl_8051D7B0)->SetStringId(difficulty);
    TLComponentInstance* titleOver = fn_801F9FEC(over, lbl_8051D7B0);
    FEFinder<TLTextInstance, 3>::FindOrDefault(titleOver->GetActiveSlide(), lbl_8051D7B0)->SetStringId(difficulty);

    TLComponentInstance* difficultyOff = fn_801F9FEC(off, lbl_8051D7C0);
    TLComponentInstance* difficultyOver = fn_801F9FEC(over, lbl_8051D7C0);
    if (mTutorial)
    {
        difficultyOff->SetVisible(false);
        difficultyOver->SetVisible(false);
        FEFinder<TLTextInstance, 3>::FindOrDefault(off, lbl_8051D7CC)->SetVisible(false);
        FEFinder<TLTextInstance, 3>::FindOrDefault(over, lbl_8051D7CC)->SetVisible(false);
    }
    else
    {
        difficultyOff->SetActiveSlide(config, true, false);
        difficultyOver->SetActiveSlide(config, true, false);
    }

    unsigned short number[4];
    nlSNPrintf(number, 4, lbl_806DE2B0, index + mChallengeOffset + 1);
    WideBasicString text = Format(
        WideBasicString(g_pLocalization->GetString(lbl_8051D7E0)), number, mLayerName);
    nlStrNCpy<unsigned short>(mChallengeNumbers[index], text.c_str(), 8);
    FEFinder<TLTextInstance, 3>::FindOrDefault(off, lbl_806DE304)->SetString(mChallengeNumbers[index]);
    FEFinder<TLTextInstance, 3>::FindOrDefault(over, lbl_806DE304)->SetString(mChallengeNumbers[index]);
}

void TU80231118Scene::Update(float dt)
{
    BaseSceneHandler::Update(dt);
    if (mState == 0 || mState == 2 || mState == 3)
    {
        TLSlide* slide = mPresentation->GetActiveSlide();
        if (slide->m_time < slide->m_start + slide->m_duration)
        {
            for (int i = 0; i < 4; ++i)
            {
                gFEPointerInstances[i]->SetActiveSlide(lbl_806DE2C0, true, false);
            }
            return;
        }

        if (mState == 0)
        {
            if (!mPrepared)
            {
                GetNavigationScene()->SetButtons(4, true);
                fn_80232E08();
                mPrepared = true;
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
            Presentation::GetInstance()->Call(lbl_8051D7EC);
            GameSceneManager::Instance()->Pop();
            return;
        }
    }

    for (int i = 0; i < 4; ++i)
    {
        TLComponentInstance* cursor = gFEPointerInstances[i];
        if ((unsigned int)i != gFEControllerIndex)
        {
            cursor->SetActiveSlide(lbl_806DE2C0, true, false);
            continue;
        }
        if (mActiveCount > 0 || mNavigation.mPointerInside[i])
        {
            cursor->SetActiveSlide(lbl_806DE30C, true, false);
        }
        else
        {
            cursor->SetActiveSlide(lbl_806DE310, true, false);
        }

        u8 valid = true;
        FEPointerEvent event;
        event.mIndex = i;
        event.mPosition = GetPointerPosition(i, &valid);
        event.mPressed = g_pFEInput->JustPressed((eFEINPUT_PAD)i, 0x1E, true, 0);
        event.mReleased = g_pFEInput->JustReleased((eFEINPUT_PAD)i, 0x1E, true, 0);
        mScrollWidget.Update(event, dt);
        for (int j = 0; j < 3; ++j)
        {
            mChallengeComponents[j].HandlePointerEvent(&event);
        }
        if (mNavigation.UpdateBackButton(event, dt))
        {
            mState = 3;
            GetNavigationScene()->HideButtons();
            mPresentation->SetActiveSlide(lbl_806DE318, true);
            return;
        }
    }

    if (mScrollWidget.IsScrolling(1, 1))
    {
        ++mChallengeOffset;
        for (int i = 0; i < 3; ++i)
        {
            fn_80231958(i);
        }
    }
    else if (mScrollWidget.IsScrolling(0, 1))
    {
        --mChallengeOffset;
        for (int i = 0; i < 3; ++i)
        {
            fn_80231958(i);
        }
    }
}

void TU80231118Scene::fn_80232E08()
{
    typedef Detail::MemFunImpl<void, void (TU80231118Scene::*)(int, void*)> PointerMethod;
    typedef BindExp3<void, PointerMethod, TU80231118Scene*, Placeholder<0>, Placeholder<1> > PointerBinding;

    FEPointerListener::Callback callback0(
        PointerBinding(MemFun(&TU80231118Scene::fn_80231224), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback callback1(
        PointerBinding(MemFun(&TU80231118Scene::fn_802312CC), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback callback2(
        PointerBinding(MemFun(&TU80231118Scene::fn_8023134C), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback callback3(
        PointerBinding(MemFun(&TU80231118Scene::fn_80231198), this, Placeholder<0>(), Placeholder<1>()));

    for (int i = 0; i < 3; ++i)
    {
        TLImageInstance* image = FEFinder<TLImageInstance, 2>::Find(mChallengeSlides[i],
            nlStringLowerHash(lbl_806DE2D0), nlStringLowerHash(lbl_8051D718),
            nlStringLowerHash("list_back_480x90"), 0, 0, 0);
        if (image == 0)
        {
            image = &UnidentifiedTLImageDefault::sInstance;
        }
        feVector3 position = mChallengeSlides[i]->GetAssetPosition();
        mChallengeComponents[i].SetInstanceBounds(image, true, position.f.x, position.f.y, 0.95f, 0.75f);
        mChallengeComponents[i].SetPointerEnterCallback(callback0);
        mChallengeComponents[i].SetPointerLeaveCallback(callback1);
        mChallengeComponents[i].SetPointerInsideCallback(callback2);
        mChallengeComponents[i].SetPointerPressCallback(callback3);
    }
    if (!mScrollWidget.mInitialized)
    {
        mScrollWidget.Initialize();
    }
}

FETextureResource*& TLImageInstance::fn_802332D0()
{
    return m_pTextureResource;
}

void TLImageInstance::fn_802332D8(FETextureResource* resource)
{
    if (resource != 0)
    {
        m_pTextureResource = resource;
    }
}

#include "Game/FE/feFinder_impl.h"

#include "NL/nlstring_tmpl.h"
