#include "Game/SH/SHOnlineInvitePreview.h"
#include "NL/nlBasicString.inl"
#include "NL/nlFunction.inl"
#include "Game/SH/SHOnlineInviteStatus.h"
#include "Game/FE/FEAudio.h"
#include "Game/FE/feHelpFuncs_decl.h"

#include "Game/GameSceneManager.h"
#include "Game/DB/StadiumInfo.h"
#include "Game/FE/feFinder.h"
#include "Game/FE/feInput.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlSlide.h"
#include "Game/FE/tlTextInstance.h"
#include "Game/FriendManager.h"
#include "NL/nlPrint.h"
#include "NL/nlBasicString.h"
#include "NL/nlBind.h"
#include "NL/nlFormat.h"
#include "NL/nlFunction.h"
#include "NL/nlLocalization.h"
#include "Game/FE/feDPD.h"
#include "Game/SH/SHNavigation.h"
#include "Game/FE/FEAudio.h"
#include "NL/nlPrint.h"

#include <string.h>

typedef BasicString<unsigned short, Detail::TempStringAllocator> WideBasicString;

SHOnlineInvitePreview::SHOnlineInvitePreview()
    : mContinueButton()
    , mButtonInitialized(false)
    , mPreviewDelayComplete(false)
    , mPreviewDelayTimer(1.0f,
          Function<FETimer*>(Bind<void>(
              MemFun(&SHOnlineInvitePreview::OnPreviewDelayElapsed), this, Placeholder<0>())))
{
    mState = 0;

    SHNavigation* object = GetNavigationScene();
    if (object != 0)
    {
        object->SetButtons(0, true);
    }
}

SHOnlineInvitePreview::~SHOnlineInvitePreview()
{
}

void SHOnlineInvitePreview::OnPreviewDelayElapsed(FETimer* timer)
{
    mPreviewDelayComplete = true;
    mPreviewDelayTimer.SetEnabled(false);
}

void SHOnlineInvitePreview::Update(float fDeltaT)
{
    BaseSceneHandler::Update(fDeltaT);
    mPreviewDelayTimer.Update(fDeltaT);

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
            FEPointerListener::Callback over(Bind<void>(
                MemFun(&SHOnlineInvitePreview::OnContinuePointerEnter), this, Placeholder<0>(), Placeholder<1>()));
            FEPointerListener::Callback off(Bind<void>(
                MemFun(&SHOnlineInvitePreview::OnContinuePointerLeave), this, Placeholder<0>(), Placeholder<1>()));
            FEPointerListener::Callback select(Bind<void>(
                MemFun(&SHOnlineInvitePreview::OnContinuePointerPress), this, Placeholder<0>(), Placeholder<1>()));

            mContinueButton.SetInstanceBounds(
                mContinueButtonInstance, true, 0.0f, 0.0f, 1.0f, 1.0f);
            mContinueButton.SetPointerEnterCallback(over);
            mContinueButton.SetPointerLeaveCallback(off);
            mContinueButton.SetPointerPressCallback(select);
            mButtonInitialized = true;
            mState = 1;
        }
        else if (state == 2)
        {
            GameSceneManager::Instance()->Push(SCENE_ONLINE_INVITE_RESPONSE, SCREEN_FORWARD, true);
            return;
        }
    }

    if (mPreviewDelayTimer.mEnabled)
    {
        return;
    }

    if (!g_pFriendManager->ValidateHostInvitation())
    {
        GameSceneManager::Instance()->Push(SCENE_ONLINE_INVITE_STATUS, SCREEN_FORWARD, true);
        SHOnlineInviteStatus* scene
            = (SHOnlineInviteStatus*)GameSceneManager::Instance()->GetScene(SCENE_ONLINE_INVITE_STATUS);
        scene->mStatus = 2;
        scene->mReturnDelay = 2.0f;
        return;
    }

    for (int pad = 0; pad < 4; ++pad)
    {
        TLComponentInstance* controller = gFEPointerInstances[pad];
        if ((unsigned int)pad != gFEControllerIndex)
        {
            controller->SetActiveSlide("waiting", true, false);
        }
        else
        {
            unsigned char valid = 1;
            FEPointerEvent event;
            event.mIndex = pad;
            event.mPosition = GetPointerPosition(pad, &valid);
            event.mPressed
                = g_pFEInput->JustPressed((eFEINPUT_PAD)pad, 0x1E, true, 0);
            mContinueButton.HandlePointerEvent(&event);
        }
    }
}

void SHOnlineInvitePreview::SceneCreated()
{
    FriendStatusPayload* payload = GetFriendManager()->GetFriendStatusPayload(
        GetFriendManager()->GetHostInvitationIndex());
    const CheatSettings& settings = payload->mPowerupSettings;
    TLSlide* slide = GetPresentation()->GetActiveSlide();
    mContinueButtonInstance = FEFinder<TLComponentInstance, 4>::FindOrDefault<TLSlide>(slide, "Layer", "PREVIEW", "BTN_1");
    for (int i = 0; i < 4; ++i)
    {
        GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
    }

    TLTextInstance* text = FEFindTextInstance(slide, "Layer", "PREVIEW", "OPTIONS", "OPTION_0");
    unsigned short value0[4];
    unsigned short value1[4];
    nlSNPrintf(value0, 4, (const unsigned short*)L"%d", payload->mGameplaySettings.NumGames);
    WideBasicString string = Format(WideBasicString(g_pLocalization->GetString("ONLINE_PREVIEW_OPTION_0")), value0);
    memcpy(mBestSeriesText, string.c_str(), sizeof(mBestSeriesText));
    text->SetString(mBestSeriesText);

    text = FEFindTextInstance(slide, "Layer", "PREVIEW", "OPTIONS", "OPTION_1");
    const char* id;
    int value;
    if (payload->mGameplaySettings.GameLimitType == 0)
    {
        id = "X_MINUTES";
        value = payload->mGameplaySettings.GameTime / 60;
    }
    else
    {
        id = "X_GOALS";
        value = payload->mGameplaySettings.GoalLimit;
    }
    nlSNPrintf(value1, 4, (const unsigned short*)L"%d", value);
    const unsigned short* format = g_pLocalization->GetString("ONLINE_PREVIEW_OPTION_1");
    string = Format(WideBasicString(g_pLocalization->GetString(id)), value1);
    string = Format(WideBasicString(format), string.c_str());
    memcpy(mGameLimitText, string.c_str(), sizeof(mGameLimitText));
    text->SetString(mGameLimitText);

    text = FEFindTextInstance(slide, "Layer", "PREVIEW", "OPTIONS", "OPTION_2");
    int stadium = payload->mStadium;
    string = Format(WideBasicString(g_pLocalization->GetString("ONLINE_PREVIEW_OPTION_2")),
        g_pLocalization->GetString(GetStadiumTickerStringID(stadium)));
    memcpy(mStadiumText, string.c_str(), sizeof(mStadiumText));
    text->SetString(mStadiumText);

    text = FEFindTextInstance(slide, "Layer", "PREVIEW", "OPTIONS", "OPTION_3");
    id = GetLOCEnvironmentCheatName(settings.GetEnvironmentCheat());
    string = Format(WideBasicString(g_pLocalization->GetString("ONLINE_PREVIEW_OPTION_3")),
        g_pLocalization->GetString(id));
    memcpy(mEnvironmentCheatText, string.c_str(), sizeof(mEnvironmentCheatText));
    text->SetString(mEnvironmentCheatText);

    text = FEFindTextInstance(slide, "Layer", "PREVIEW", "OPTIONS", "OPTION_4");
    id = GetLOCPowerupCheatName(settings.GetCustomPowerups());
    string = Format(WideBasicString(g_pLocalization->GetString("ONLINE_PREVIEW_OPTION_4")),
        g_pLocalization->GetString(id));
    memcpy(mPowerupCheatText, string.c_str(), sizeof(mPowerupCheatText));
    text->SetString(mPowerupCheatText);

    text = FEFindTextInstance(slide, "Layer", "PREVIEW", "OPTIONS", "OPTION_5");
    id = GetLOCPlayerCheatName(settings.GetPlayerCheat());
    string = Format(WideBasicString(g_pLocalization->GetString("ONLINE_PREVIEW_OPTION_5")),
        g_pLocalization->GetString(id));
    memcpy(mPlayerCheatText, string.c_str(), sizeof(mPlayerCheatText));
    text->SetString(mPlayerCheatText);

    FEAudio::PlayAnimAudioEvent(0xBB142B94, 0, 0, 1);
    mPreviewDelayTimer.SetEnabled(true);
}

void SHOnlineInvitePreview::OnContinuePointerEnter(int index, void*)
{
    mContinueButtonInstance->SetActiveSlide("over", true, false);
    mContinueButton.SetPointerState(1, index);
    gFEPointerInstances[index]->SetActiveSlide("A", true, false);
    FEAudio::PlayAnimAudioEvent(0xDE912775, 0, 0, 1);
}

void SHOnlineInvitePreview::OnContinuePointerLeave(int index, void*)
{
    mContinueButtonInstance->SetActiveSlide("off", true, false);
    mContinueButton.SetPointerState(0, index);
    gFEPointerInstances[index]->SetActiveSlide("cursor", true, false);
}

void SHOnlineInvitePreview::OnContinuePointerPress(int index, void*)
{
    mContinueButtonInstance->SetActiveSlide("down", true, false);
    mContinueButton.SetPointerState(2, index);
    FEAudio::PlayAnimAudioEvent(0xF0AFD586, 0, 0, 1);
    mState = 2;
    mPresentation->SetActiveSlide("out", true);
    mPresentation->Update(0.0f);
}

#include "Game/FE/feFinder_impl.h"
