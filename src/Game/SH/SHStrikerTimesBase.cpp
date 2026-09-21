#include "Game/SH/SHNavigation.h"
#include "NL/nlFunction.inl"
#include "Game/FE/feHelpFuncs_decl.h"
#include "Game/SH/SHStrikerTimesBase.h"
#include "Game/FE/tlImageInstance.h"
#include "Game/FE/FEAudio.h"

#include "Game/DB/CharacterInfo.h"
#include "Game/DB/GameProgress.h"
#include "Game/DB/SaveLoad.h"
#include "Game/FE/feFinder.h"
#include "Game/FE/feFinder.inl"
#include "Game/FE/feInput.h"
#include "Game/FE/fePackage.h"
#include "Game/FE/fePresentation.h"
#include "Game/FE/feScene.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlImageInstance.h"
#include "Game/FE/tlSlide.h"
#include "Game/FE/tlTextInstance.h"
#include "Game/GameInfo.h"
#include "NL/globalpad.h"
#include "NL/nlMath.h"
#include "NL/nlBind.h"
#include "NL/nlPrint.h"
#include "NL/nlLocalization.h"
#include "NL/nlString.h"
#include "Game/FE/feDPD.h"
#include "Game/FE/FEAudio.h"
#include "Game/SH/SHNavigation.h"

static inline void ShowDoneButtons(TLSlide* first)
{
    TLSlide* slide = first;
    FEAudio::PlayAnimAudioEvent(0x2AB04562, 0, 0, 1);
    do
    {
        TLComponentInstance* done = FEFinder<TLComponentInstance, TLAT_COMPONENT>::FindOrDefault<>(slide, "Layer", "done");
        done->m_bVisible = true;
        done->SetActiveSlide("in", true, false);
        slide = slide->m_next;
    } while (slide != first);
}

SHStrikerTimesBase::SHStrikerTimesBase()
    : BaseOverlayHandler(1, POSITION_ALL)
    , mDisplayMode(-1)
    , mPage(-1)
    , mDonePressed(false)
    , mUseCustomText(false)
    , mState(0)
    , mControlsInitialized(false)
    , mDoneVisible(false)
    , mCanShowDone(true)
    , mContentInitialized(false)
    , mIntroAudioPlayed(false)
    , mLogoReady(false)
    , mHeadlineScroller(0)
    , mStoryHeadlineScroller(0)
    , mStoryImage("art/fe/StrikerTimesUI.res", 0)
    , mHeadlineImage("art/fe/StrikerTimesUI.res", 0)
    , mLogoImage("art/fe/StrikerTimesUI.res", 0)
{
    mDoneButton.mIgnoreInputLock = true;
    mDoneButton.mDisabled = true;
    FEPointerEvent event;
    mDoneButton.mPreviousEvents[0] = event;
    mDoneButton.mPreviousEvents[1] = event;
    mDoneButton.mPreviousEvents[2] = event;
    mDoneButton.mPreviousEvents[3] = event;
}

SHStrikerTimesBase::~SHStrikerTimesBase()
{
}

void SHStrikerTimesBase::SceneCreated()
{
    mScrollBar.SetComponent(FEFinder<TLComponentInstance, TLAT_COMPONENT>::FindOrDefault(mPresentation->m_currentSlide, "Layer", "scrollbar"));
    if ((unsigned int)(mDisplayMode - 0xB) <= 2)
    {
        mScrollBar.SetRange(0);
        mLogoReady = true;
    }
    else if (mDisplayMode == 0xA)
    {
        mScrollBar.SetRange(2);
    }
    else
    {
        mScrollBar.SetRange(1);
    }
    mScrollBar.SetValue(0);

    TLSlide* first = mPresentation->m_currentSlide;
    TLSlide* slide = first;
    do
    {
        FEFinder<TLInstance, 2>::Find<>(slide, "Layer", "TimerText")->m_bVisible = false;
        FEFinder<TLInstance, 2>::Find<>(slide, "Layer", "NetworkWait")->m_bVisible = false;
        FEFinder<TLComponentInstance, TLAT_COMPONENT>::FindOrDefault<>(slide, "Layer", "done")->m_bVisible = false;
        slide = slide->m_next;
    } while (slide != first);

    if (mDisplayMode != 0xA)
    {
        TLInstance* more = FEFinder<TLComponentInstance, 4>::FindOrDefault(mPresentation, "story", "Layer", "more");
        more->m_bVisible = false;
    }

    if (mDisplayMode != 8 && mDisplayMode != 0xD)
    {
        SHNavigation* scene = GetNavigationScene();
        if (scene != 0)
            scene->SetButtons(0, true);
    }

    char buffer[0x40];
    switch (g_pLocalization->m_CurrentLanguage)
    {
    case nlLocalization::LangFrench:
    case nlLocalization::LangNAFrench:
        nlSNPrintf(buffer, 0x40, "fe/striker_times_textures/st_logo_french");
        break;
    case nlLocalization::LangGerman:
        nlSNPrintf(buffer, 0x40, "fe/striker_times_textures/st_logo_german");
        break;
    case nlLocalization::LangSpanish:
    case nlLocalization::LangNASpanish:
        nlSNPrintf(buffer, 0x40, "fe/striker_times_textures/st_logo_spanish");
        break;
    case nlLocalization::LangItalian:
        nlSNPrintf(buffer, 0x40, "fe/striker_times_textures/st_logo_italian");
        break;
    default:
        mLogoReady = true;
        break;
    }

    if (!mLogoReady)
    {
        TLInstance* logo = FEFinder<TLImageInstance, 2>::Find<>(mPresentation, "logo", "Layer", "logo", "st_logo");
        mLogoImage.mImageInstance = (TLImageInstance*)logo;
        mLogoImage.QueueLoad(buffer, false);
    }

    mPresentation->SetActiveSlide("in", true);
    mPresentation->Update(0.0f);
}

void SHStrikerTimesBase::Update(float dt)
{
    TLSlide* first;
    if (SaveEnabled && InOperation)
        return;
    if (!mLogoReady)
    {
        mLogoReady = mLogoImage.Update(true);
        return;
    }
    BaseSceneHandler::Update(dt);
    if (mDisplayMode != 0xD && mDisplayMode != 0xC && mDisplayMode != 0xB)
    {
        if (!mContentInitialized)
            InitializeContent();
        mStoryImage.Update(true);
        mHeadlineImage.Update(true);
    }
    if (!mIntroAudioPlayed)
    {
        mIntroAudioPlayed = true;
        FEAudio::PlayAnimAudioEvent(0xAFE4352B, 0, 0, 1);
    }
    int state = mState;
    if (state == 0 || (unsigned int)(state - 2) <= 1)
    {
        FEPresentation* presentation = mPresentation;
        TLSlide* slide = presentation->m_currentSlide;
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
            if (!mControlsInitialized)
            {
                InitializeControls();
                mControlsInitialized = true;
            }
            mState = 1;
            if ((unsigned int)(mDisplayMode - 0xB) <= 2)
            {
                mPresentation->SetActiveSlide("game summary", true);
                mPresentation->Update(0.0f);
            }
            else if (mDisplayMode == 0xA)
            {
                mPresentation->SetActiveSlide("logo", true);
                mPresentation->Update(0.0f);
            }
            else
            {
                mPresentation->SetActiveSlide("logo", true);
                mPresentation->Update(0.0f);
            }
            return;
        }
        if (state == 2)
        {
            OnDoneTransitionComplete();
            return;
        }
        if (state == 3)
        {
            OnBackTransitionComplete();
            return;
        }
    }
    if (mPage == 0)
    {
        TLSlide* slide = mPresentation->m_currentSlide;
        if (slide->GetCurrentTime() >= slide->GetStartTime() + slide->GetDuration())
        {
            mPage = 1;
            mPresentation->SetActiveSlide("headline pic", true);
            mPresentation->Update(0.0f);
        }
        else
        {
            return;
        }
    }
    if ((unsigned int)(mPage - 1) <= 1)
    {
        mHeadlineScroller.Update(dt);
        mStoryHeadlineScroller.m_scrollOffset = mHeadlineScroller.m_scrollOffset;
        mStoryHeadlineScroller.Update(0.0f);
    }
    if (mCanShowDone && !mDoneVisible)
    {
        FEPresentation* presentation = mPresentation;
        first = presentation->m_currentSlide;
        ShowDoneButtons(first);
        mDoneButton.mDisabled = false;
        mDoneVisible = true;
    }
    for (int pad = 0; pad < 4; ++pad)
    {
        TLComponentInstance* instance = GetPointerInstance(pad);
        if (mDisplayMode != 0xC && pad != gFEControllerIndex)
        {
            instance->SetActiveSlide("waiting", true, false);
        }
        else
        {
            instance->SetActiveSlide("cursor", true, false);
            u8 valid = 1;
            FEPointerEvent event;
            event.mIndex = pad;
            event.mPosition = GetPointerPosition(pad, &valid);
            g_pPadManager->GetPad(pad)->GetButtonIndex(0x1E, true);
            event.mPressed = g_pFEInput->JustPressed((eFEINPUT_PAD)pad, 0x1E, true, 0);
            event.mReleased = g_pFEInput->JustReleased((eFEINPUT_PAD)pad, 0x1E, true, 0);
            mDoneButton.HandlePointerEvent(&event);
            if (mDonePressed)
                return;
            mScrollBar.Update(event, dt);
            if (mScrollBar.IsScrolling(1, 1))
            {
                ShowNextPage();
            }
            else if (mScrollBar.IsScrolling(0, 1))
            {
                ShowPreviousPage();
            }
        }
    }
}

void SHStrikerTimesBase::InitializeContent()
{
    FEPresentation* presentation = mFEScene->m_pFEPackage->GetPresentation();

    TLTextInstance* headlineText = FEFinder<TLTextInstance, 3>::FindOrDefault(presentation, "headline pic", "Layer", "HEADLINE");

    TLTextInstance* descriptionText = FEFinder<TLTextInstance, 3>::FindOrDefault(presentation, "story", "Layer", "Description_clip");

    mHeadlineScroller.ApplyNewTextInstancePointer(headlineText, -1, -1, -300);
    if (mUseCustomText)
        mHeadlineScroller.SetDisplayMessage(mHeadlineText);
    else
        mHeadlineScroller.SetDisplayMessage(mHeadlineStringID);
    mHeadlineScroller.SetClippingTextInstance(descriptionText);
    descriptionText->m_bVisible = false;

    TLTextInstance* storyHeadline = FEFinder<TLTextInstance, 3>::FindOrDefault(presentation, "story", "Layer", "HEADLINE");
    mStoryHeadlineScroller.ApplyNewTextInstancePointer(storyHeadline, -1, -1, -300);
    if (mUseCustomText)
        mStoryHeadlineScroller.SetDisplayMessage(mHeadlineText);
    else
        mStoryHeadlineScroller.SetDisplayMessage(mHeadlineStringID);
    mStoryHeadlineScroller.SetClippingTextInstance(descriptionText);

    TLTextInstance* bodyText = FEFinder<TLTextInstance, 3>::FindOrDefault(presentation, "story", "Layer", "BODY");
    if (mUseCustomText)
        bodyText->SetString(mStoryText.c_str());
    else
        bodyText->SetStringId(mStoryStringID);

    mHeadlineScroller.m_scrollSpeed = 150.0f;
    mStoryHeadlineScroller.m_scrollSpeed = 150.0f;
    mHeadlineScroller.m_nextDeltaT = 2.0f;
    mStoryHeadlineScroller.m_nextDeltaT = 2.0f;

    TLInstance* storyTexture = FEFinder<TLInstance, 2>::FindOrDefault(mPresentation, "story", "Layer", "00_dummy_texture");
    mStoryImage.mImageInstance = (TLImageInstance*)storyTexture;

    TLInstance* headlineTexture = FEFinder<TLInstance, 2>::FindOrDefault(mPresentation, "headline pic", "Layer", "00_dummy_texture");
    mHeadlineImage.mImageInstance = (TLImageInstance*)headlineTexture;

    mHeadlineImage.QueueLoad(mArticleImageName, false);
    mStoryImage.QueueLoad(mArticleImageName, false);
    mContentInitialized = true;
}

void SHStrikerTimesBase::SetArticleImageName(int captain, int mood, int special)
{
    int variant = nlRandom(3, &nlDefaultSeed);
    if (mDisplayMode == 8)
    {
        s8 stored = g_pStrikerChallenge->mHeadlineVariant;
        if (stored != -1)
            variant = stored;
        else
            g_pStrikerChallenge->mHeadlineVariant = variant;
    }
    const CharacterInfo& info = GetCharacterInfo(GetCharacterIndexFromCaptain(captain));
    char name[0x10];
    if (special == 4)
    {
        nlSNPrintf(name, 0x10, "boo");
        variant = 0;
    }
    else if (special == 2)
    {
        nlSNPrintf(name, 0x10, "hammerbro");
        variant = 0;
    }
    else if (special == 8)
    {
        nlSNPrintf(name, 0x10, "kritter");
        variant = 0;
    }
    else if (captain == 3)
    {
        nlSNPrintf(name, 0x10, "dk");
    }
    else if (captain == 0xA)
    {
        nlSNPrintf(name, 0x10, "diddy");
    }
    else
    {
        nlSNPrintf(name, 0x10, "%s", info.mName);
    }
    switch (mood)
    {
    case 0:
        nlSNPrintf(mArticleImageName, 0x40, "fe/striker_times_textures/%s_positive_0%d", name, variant);
        break;
    case 1:
        nlSNPrintf(mArticleImageName, 0x40, "fe/striker_times_textures/%s_neutral_0%d", name, variant);
        break;
    case 2:
        nlSNPrintf(mArticleImageName, 0x40, "fe/striker_times_textures/%s_negative_0%d", name, variant);
        break;
    case 3:
        nlSNPrintf(mArticleImageName, 0x40, "fe/striker_times_textures/%s_positive_0%d", name, variant);
        break;
    }
}

void SHStrikerTimesBase::SetDisplayMode(unsigned int transition)
{
    mDisplayMode = transition;
    if (transition - 0xB <= 2)
    {
        mCanShowDone = true;
        mPage = 3;
    }
    else
    {
        mCanShowDone = false;
        mPage = 0;
    }
}

void SHStrikerTimesBase::ShowPreviousPage()
{
    FEPresentation* presentation = mPresentation;
    if (mDisplayMode == 0xD)
        return;
    if (mDisplayMode == 0xC)
        return;
    if (mDisplayMode == 0xB)
        return;
    if (mDisplayMode == 0xA)
    {
        if (mPage == 2)
        {
            mPage = 1;
            presentation->SetActiveSlide("headline pic", true);
            presentation->Update(presentation->m_currentSlide->GetStartTime() + presentation->m_currentSlide->GetDuration());
        }
        else if (mPage == 3)
        {
            mPage = 2;
            presentation->SetActiveSlide("story", true);
            presentation->Update(0.0f);
        }
    }
    else if (mPage == 2)
    {
        mPage = 1;
        presentation->SetActiveSlide("headline pic", true);
        presentation->Update(presentation->m_currentSlide->GetStartTime() + presentation->m_currentSlide->GetDuration());
    }
}

void SHStrikerTimesBase::ShowNextPage()
{
    FEPresentation* presentation = mPresentation;
    if ((unsigned int)(mDisplayMode - 0xB) <= 2)
    {
        mCanShowDone = true;
    }
    else if (mDisplayMode == 0xA)
    {
        if (mPage == 2)
        {
            mCanShowDone = GameInfoManager::Instance()->mIsOnlineMode == 0;
            mPage = 3;
            presentation->SetActiveSlide("game summary", true);
        }
        else if (mPage == 1)
        {
            mPage = 2;
            presentation->SetActiveSlide("story", true);
            presentation->Update(0.0f);
        }
    }
    else if (mPage == 1)
    {
        mCanShowDone = true;
        mPage = 2;
        presentation->SetActiveSlide("story", true);
        presentation->Update(0.0f);
    }
}

void SHStrikerTimesBase::OnDoneTransitionComplete()
{
}

void SHStrikerTimesBase::InitializeControls()
{
    typedef Detail::MemFunImpl<void, void (SHStrikerTimesBase::*)(int, void*)> PointerMethod;
    typedef BindExp3<void, PointerMethod, SHStrikerTimesBase*, Placeholder<0>, Placeholder<1> > PointerBinding;

    if (!mScrollBar.mInitialized)
    {
        mScrollBar.SetComponent(FEFinder<TLComponentInstance, TLAT_COMPONENT>::FindOrDefault(mPresentation->m_currentSlide, "Layer", "scrollbar"));
        mScrollBar.Initialize();
    }
    FEPointerListener::Callback callback(
        PointerBinding(MemFun(&SHStrikerTimesBase::OnDonePointerEnter), this, Placeholder<0>(), Placeholder<1>()));
    mDoneButton.SetPointerEnterCallback(callback);
    callback = FEPointerListener::Callback(
        PointerBinding(MemFun(&SHStrikerTimesBase::OnDonePointerLeave), this, Placeholder<0>(), Placeholder<1>()));
    mDoneButton.SetPointerLeaveCallback(callback);
    FEPointerListener::Callback callback2(
        PointerBinding(MemFun(&SHStrikerTimesBase::OnDonePointerPress), this, Placeholder<0>(), Placeholder<1>()));
    mDoneButton.SetPointerPressCallback(callback2);
    SetDoneButtonBounds(&mDoneButton, 0, 0);
}

void SHStrikerTimesBase::OnDonePointerEnter(int index, void* context)
{
    mDoneButton.SetPointerState(1, index);
    TLComponentInstance* done = FEFinder<TLComponentInstance, TLAT_COMPONENT>::FindOrDefault(mPresentation->m_currentSlide, "Layer", "done");
    if (!mDoneButton.HasOtherPointerState(1, index))
    {
        done->SetActiveSlide("over", true, false);
        FEAudio::PlayAnimAudioEvent(0xAA73EF33, 0, 0, 1);
    }
}

void SHStrikerTimesBase::OnDonePointerLeave(int index, void* context)
{
    mDoneButton.SetPointerState(0, index);
    TLComponentInstance* done = FEFinder<TLComponentInstance, TLAT_COMPONENT>::FindOrDefault(mPresentation->m_currentSlide, "Layer", "done");
    if (!mDoneButton.HasOtherPointerState(1, index))
    {
        done->SetActiveSlide("off", true, false);
    }
}

void SHStrikerTimesBase::OnDonePointerPress(int index, void* context)
{
    FEAudio::PlayAnimAudioEvent(0xF0AFD586, 0, 0, 1);
    mDonePressed = true;
    mState = 2;
    mPresentation->SetActiveSlide("out", true);
    mPresentation->Update(0.0f);
    if (mDisplayMode != 0xD)
    {
        GetNavigationScene()->HideButtons();
    }
    if (mDisplayMode == 8)
    {
        FEAudio::PlayAnimAudioEvent(0x4861E03D, 0, 0, 1);
    }
    FEFinder<TLComponentInstance, TLAT_COMPONENT>::FindOrDefault(mPresentation->m_currentSlide, "Layer", "done")->SetActiveSlide("down", true, false);
}
