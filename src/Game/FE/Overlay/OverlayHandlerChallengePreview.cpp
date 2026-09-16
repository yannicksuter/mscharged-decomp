#include "Game/FE/feFinder.h"
#include "Game/FE/Overlay/OverlayHandlerChallengePreview.h"
#include "Game/DB/GameProgress.h"
#include "Game/BaseGameSceneManager.h"
#include "Game/FE/FEAudio.h"
#include "Game/FE/feDPD.h"
#include "Game/FE/feInput.h"
#include "Game/FE/feManager.h"
#include "Game/FE/fePresentation.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlSlide.h"
#include "Game/FE/tlTextInstance.h"
#include "Game/HBMManager.h"
#include "Game/SH/SHNavigation.h"
#include "NL/nlBasicString.h"
#include "NL/nlBind.h"
#include "NL/nlLocalization.h"
#include "NL/nlPrint.h"

#include <string.h>

typedef BasicString<unsigned short, Detail::TempStringAllocator> WideBasicString;

template <typename StringType, typename T0>
StringType Format(const StringType& string, const T0& t0);

template <typename StringType, typename T0, typename T1>
StringType Format(const StringType& string, const T0& t0, const T1& t1);

ChallengePreviewOverlay::ChallengePreviewOverlay(ScreenMovement movement)
    : BaseOverlayHandler(1, POSITION_ALL)
    , mContinueButton()
    , mMovement(movement)
    , mButtonInitialized(false)
    , mContinuePressed(false)
    , mState(0)
{
    SHNavigation* object = GetNavigationScene();
    if (object != 0)
    {
        object->SetButtons(0, true);
    }
}

ChallengePreviewOverlay::~ChallengePreviewOverlay()
{
}

inline void ChallengePreviewOverlay::InitializeContinueButton()
{
    typedef Detail::MemFunImpl<void, void (ChallengePreviewOverlay::*)(int, void*)> PointerMethod;
    typedef BindExp3<void, PointerMethod, ChallengePreviewOverlay*, Placeholder<0>, Placeholder<1> > PointerBinding;
    FEPointerListener::Callback over(PointerBinding(
        MemFun(&ChallengePreviewOverlay::OnContinuePointerEnter), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback off(PointerBinding(
        MemFun(&ChallengePreviewOverlay::OnContinuePointerLeave), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback down(PointerBinding(
        MemFun(&ChallengePreviewOverlay::OnContinuePointerPress), this, Placeholder<0>(), Placeholder<1>()));

    mContinueButton.SetInstanceBounds(
        mContinueButtonInstance, true, 0.0f, 0.0f, 1.0f, 1.0f);
    mContinueButton.SetPointerEnterCallback(over);
    mContinueButton.SetPointerLeaveCallback(off);
    mContinueButton.SetPointerPressCallback(down);
}

void ChallengePreviewOverlay::Update(float fDeltaT)
{
    BaseSceneHandler::Update(fDeltaT);

    int state = mState;
    if (state == 0 || (unsigned int)(state - 2) <= 1)
    {
        TLSlide* slide = mPresentation->m_currentSlide;
        if (slide->GetCurrentTime() < slide->GetStartTime() + slide->GetDuration())
        {
            for (int pad = 0; pad < 4; ++pad)
            {
                GetPointerInstance(pad)->SetActiveSlide("waiting", true, false);
            }
            return;
        }

        if (state == 0)
        {
            InitializeContinueButton();
            mButtonInitialized = true;
            mState = 1;
        }
        else if (state == 2)
        {
            if (mMovement == SCREEN_FORWARD)
            {
                FrontEnd::ExitMenuState();
            }
            else
            {
                g_pOverlayManager->Push((SceneList)80, SCREEN_BACK, true);
            }
        }
    }

    for (int pad = 0; pad < 4; ++pad)
    {
        GetPointerInstance(pad)->SetActiveSlide("cursor", true, false);
        unsigned char valid = true;
        FEPointerEvent event;
        event.mIndex = pad;
        event.mPosition = GetPointerPosition(pad, &valid);
        event.mPressed = g_pFEInput->JustPressed((eFEINPUT_PAD)pad, 0x1E, true, 0);
        mContinueButton.HandlePointerEvent(&event);
        if (mContinuePressed)
        {
            break;
        }
    }
}

void ChallengePreviewOverlay::SceneCreated()
{
    BaseSceneHandler::SceneCreated();

    StrikerChallenge* challenge = fn_801CA670();
    int currentChallenge = challenge->GetCurrentChallenge();
    TLSlide* slide = GetPresentation()->GetActiveSlide();
    TLInstance* preview = FEFinder<TLInstance, 5>::FindOrDefault<TLSlide>(slide, "Layer", "PREVIEW");
    mContinueButtonInstance = (TLComponentInstance*)
        FEFinder<TLInstance, 4>::FindOrDefault<TLInstance>(preview, "BTN_1");

    if (mMovement == SCREEN_FORWARD)
    {
        TLTextInstance* text = FEFinder<TLTextInstance, 3>::FindOrDefault(
            mContinueButtonInstance, "OFF", "ACCEPT");
        text->SetStringId("PLAY_NOW");
        text = FEFinder<TLTextInstance, 3>::FindOrDefault(
            mContinueButtonInstance, "OVER", "ACCEPT");
        text->SetStringId("PLAY_NOW");
        text = FEFinder<TLTextInstance, 3>::FindOrDefault(
            mContinueButtonInstance, "DOWN", "ACCEPT");
        text->SetStringId("PLAY_NOW");
    }

    TLTextInstance* text
        = FEFinder<TLTextInstance, 3>::FindOrDefault<TLInstance>(preview, "TITLE");
    text->SetStringId(challenge->GetDifficulty());

    TLInstance* options
        = FEFinder<TLInstance, 5>::FindOrDefault<TLInstance>(preview, "OPTIONS");
    char objective[64];
    if (currentChallenge < 10)
    {
        nlSNPrintf(objective, sizeof(objective), "tutorial_objective_%s", challenge->GetTitle());
    }
    else
    {
        nlSNPrintf(objective, sizeof(objective), "objective_%s", challenge->GetTitle());
    }

    text = FEFinder<TLTextInstance, 3>::Find<TLInstance>(options, "OPTION_0");
    WideBasicString string = Format(
        WideBasicString(g_pLocalization->GetString("CHALLENGE_PREVIEW_OBJECTIVE")),
        g_pLocalization->GetString(objective));
    memcpy(mTextBuffers[0], string.c_str(), sizeof(mTextBuffers[0]));
    text->SetString(mTextBuffers[0]);

    int remainingTime = challenge->GetRemainingTime();
    if (remainingTime == 0)
    {
        string = Format(
            WideBasicString(g_pLocalization->GetString("CHALLENGE_PREVIEW_TIME")),
            g_pLocalization->GetString("SUDDEN_DEATH"));
    }
    else
    {
        unsigned short minutes[4];
        unsigned short seconds[4];
        nlSNPrintf(minutes, 4, (const unsigned short*)L"%d", remainingTime / 60);
        nlSNPrintf(seconds, 4, (const unsigned short*)L"%.2d", remainingTime % 60);
        WideBasicString clock = Format(
            WideBasicString(g_pLocalization->GetString("CLOCK")), minutes, seconds);
        string = Format(
            WideBasicString(g_pLocalization->GetString("CHALLENGE_PREVIEW_TIME")),
            clock.c_str());
    }

    text = FEFinder<TLTextInstance, 3>::Find<TLInstance>(options, "OPTION_2");
    memcpy(mTextBuffers[1], string.c_str(), sizeof(mTextBuffers[1]));
    text->SetString(mTextBuffers[1]);

    unsigned short homeScore[4];
    unsigned short awayScore[4];
    nlSNPrintf(homeScore, 4, (const unsigned short*)L"%d", challenge->GetScore(0));
    nlSNPrintf(awayScore, 4, (const unsigned short*)L"%d", challenge->GetScore(1));
    string = Format(
        WideBasicString(g_pLocalization->GetString("CHALLENGE_PREVIEW_SCORE")),
        homeScore, awayScore);

    text = FEFinder<TLTextInstance, 3>::Find<TLInstance>(options, "OPTION_4");
    memcpy(mTextBuffers[2], string.c_str(), sizeof(mTextBuffers[2]));
    text->SetString(mTextBuffers[2]);

    fn_801FA19C()->Unblock();
    FEAudio::PlayAnimAudioEvent(0xBB142B94, 0, 0, 1);
}
void ChallengePreviewOverlay::OnContinuePointerEnter(int index, void*)
{
    if (!mContinueButton.HasOtherPointerState(1, index))
    {
        mContinueButtonInstance->SetActiveSlide("over", true, false);
        FEAudio::PlayAnimAudioEvent(0xAA73EF33, 0, 0, 1);
    }

    mContinueButton.SetPointerState(1, index);
}

void ChallengePreviewOverlay::OnContinuePointerLeave(int index, void*)
{
    if (!mContinueButton.HasOtherPointerState(1, index))
    {
        mContinueButtonInstance->SetActiveSlide("off", true, false);
    }

    mContinueButton.SetPointerState(0, index);
}

void ChallengePreviewOverlay::OnContinuePointerPress(int index, void*)
{
    mContinueButtonInstance->SetActiveSlide("down", true, false);
    mContinueButton.SetPointerState(2, index);
    mContinuePressed = true;

    for (int i = 0; i < 4; ++i)
    {
        GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
    }

    mState = 2;
    mPresentation->SetActiveSlide("out", true);
    mPresentation->Update(0.0f);
    FEAudio::PlayAnimAudioEvent(0x9F9BF00F, 0, 0, 1);
}

#include "NL/nlFunction.inl"
#include "Game/FE/feFinder_impl.h"
#include "Game/DB/GameProgress.inl"
#include "Game/HBMManager.inl"
