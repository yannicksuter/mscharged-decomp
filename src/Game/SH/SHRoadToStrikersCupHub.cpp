#include "Game/SH/SHRoadToStrikersCupHub.h"
#include "NL/nlFunction.inl"

#include "Game/DB/CharacterInfo.h"
#include "Game/DB/BasicGameInfo.h"
#include "Game/DB/GameProgress.h"
#include "Game/FE/FEAudio.h"
#include "Game/FE/feCupFlow.h"
#include "Game/FE/feDPD.h"
#include "Game/FE/feFinder.inl"
#include "Game/FE/feHelpFuncs_decl.h"
#include "Game/FE/feInput.h"
#include "Game/FE/feMusic.h"
#include "Game/FE/fePackage.h"
#include "Game/FE/fePresentation.inl"
#include "Game/FE/feScene.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlImageInstance.h"
#include "Game/FE/tlSlide.h"
#include "Game/FE/tlTextInstance.h"
#include "Game/GameInfo.h"
#include "Game/Render/FrontEndPresentation.h"
#include "Game/SH/SHNavigation.h"
#include "NL/nlBind.h"
#include "NL/nlFormat.h"
#include "NL/nlLocalizationLookup.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"

#include <string.h>

typedef BasicString<unsigned short, Detail::TempStringAllocator> WideString;

extern "C" void fn_802088B4();
extern "C" void fn_80207060(bool);
extern "C" void fn_80207724(int);

RoadToStrikersCupHubScene::RoadToStrikersCupHubScene()
    : mPointerButtonsInitialized(false)
    , mUpdateDisabled(false)
    , mIntroAudioPlayed(false)
    , mSelectedButton(-1)
    , mBackButton()
    , mPlayButton()
    , mScheduleButton()
    , mCupStatsButton()
    , mRulesButton()
    , mPlayButtonInstance(0)
    , mTransitionState(0)
{
    mPlayButton.mContext = (void*)0;
    mScheduleButton.mContext = (void*)1;
    mCupStatsButton.mContext = (void*)2;
    mRulesButton.mContext = (void*)3;

    mPointerHoverCounts[0] = 0;
    mPointerHoverCounts[1] = 0;
    mPointerHoverCounts[2] = 0;
    mPointerHoverCounts[3] = 0;
}

RoadToStrikersCupHubScene::~RoadToStrikersCupHubScene()
{
}

#pragma dont_inline on
void RoadToStrikersCupHubScene::SceneCreated()
{
    if (!FEMusic::IsPlayingCupResultStream())
        FEMusic::StartStreamIfDifferent(9);

    int userTeam = CupManager::Instance()->GetUserSelectedCupTeam();
    FEPresentation* presentation = mFEScene->GetPackage()->GetPresentation();

    TLComponentInstance* rulesButton = FEFinder<TLComponentInstance, TLAT_COMPONENT>::FindOrDefault(
        presentation->GetActiveSlide(), "Layer", "RULES_BUTTON");
    if (GameInfoManager::Instance()->GetUserInfo().IsWidescreen())
        rulesButton->SetActiveSlide("16:9", true, false);
    else
        rulesButton->SetActiveSlide("4:3", true, false);

    mScheduleButtonInstance = FEFinder<TLComponentInstance, TLAT_COMPONENT>::FindOrDefault(
        presentation->GetActiveSlide(), "Layer", "schedule");
    mCupStatsButtonInstance = FEFinder<TLComponentInstance, TLAT_COMPONENT>::FindOrDefault(
        presentation->GetActiveSlide(), "Layer", "CUP STATS");
    mRulesButtonInstance = FEFinder<TLComponentInstance, TLAT_COMPONENT>::FindOrDefault(
        rulesButton->GetActiveSlide(), "RULES");

    TLComponentInstance* summary = FEFinder<TLComponentInstance, TLAT_COMPONENT>::FindOrDefault(
        presentation->GetActiveSlide(), "Layer", "summary");
    TLImageInstance* bottomTeamLogo1 = FEFinder<TLImageInstance, TLAT_IMAGE>::FindOrDefault(
        summary->GetActiveSlide(), "bottom_team_logo_1");
    TLImageInstance* bottomTeamLogo2 = FEFinder<TLImageInstance, TLAT_IMAGE>::FindOrDefault(
        summary->GetActiveSlide(), "bottom_team_logo_2");

    int team1;
    int team2;
    if (CupManager::Instance()->GetCurrentRoundNumber() != -5)
    {
        BasicGameInfo* game = CupManager::Instance()->GetCurrentGameInfo();
        team1 = game->mTeamIndex[0];
        team2 = game->mTeamIndex[1];
        UpdateCupStatus();
    }
    else
    {
        team1 = CupManager::Instance()->GetPreviousGameTeam(0);
        team2 = CupManager::Instance()->GetPreviousGameTeam(1);
        UpdateRoundMessage();
        rulesButton->SetVisible(false);
        mRulesButton.Disable();
    }

    SetTeamLogo(bottomTeamLogo1, team1);
    SetTeamLogo(bottomTeamLogo2, team2);

    TLImageInstance* topTeamLogo = FEFinder<TLImageInstance, TLAT_IMAGE>::FindOrDefault(
        summary->GetActiveSlide(), "top_team_logo");
    SetTeamLogo(topTeamLogo, userTeam);

    TLTextInstance* record = FEFinder<TLTextInstance, TLAT_TEXT>::FindOrDefault(
        summary->GetActiveSlide(), "therecord");
    UpdateCupRecordText(record);
    UpdateCupHeading();

    if (CupManager::Instance()->ShouldShowCupPhasePopup() == true)
    {
        if (CupManager::Instance()->GetCurrentRoundType() != 0)
            ShowCupRulesPopup();
        CupManager::Instance()->SetShowCupPhasePopup(false);
    }

    for (int i = 0; i < 4; ++i)
        GetPointerInstance(i)->SetActiveSlide("waiting", true, false);

    SHNavigation* navigation = GetNavigationScene();
    TLComponentInstance* backButton = 0;
    if (navigation != 0)
    {
        navigation->HideButtons();
        backButton = navigation->GetButton(4);
        mPlayButtonInstance = navigation->GetButton(16);
    }
    mBackButton.SetButtonInstance(backButton);
    fn_80207724(0);

    FEFinder<TLImageInstance, TLAT_IMAGE>::FindOrDefault(
        presentation->GetActiveSlide(), "Layer", "summary", "TROPHY WIN")
        ->SetVisible(false);
}
#pragma dont_inline reset

void RoadToStrikersCupHubScene::Update(float fDeltaT)
{
    if (g_pFEInput->m_InputLockDepth != 0)
        return;

    BaseSceneHandler::Update(fDeltaT);
    if (mUpdateDisabled)
        return;
    if (FrontEndPresentation::GetInstance()->IsActive())
        return;

    if (!mIntroAudioPlayed)
    {
        FEAudio::PlayAnimAudioEvent(0x37A9934D, 0, 0, true);
        mIntroAudioPlayed = true;
    }

    if (mTransitionState == 0 || mTransitionState == 2
        || mTransitionState == 3)
    {
        TLSlide* slide = mPresentation->m_currentSlide;
        if (slide->GetCurrentTime()
            < slide->GetStartTime() + slide->GetDuration())
        {
            for (int i = 0; i < 4; ++i)
                GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
            return;
        }

        if (mTransitionState == 0)
        {
            SHNavigation* navigation = GetNavigationScene();
            navigation->SetButtons(0x14, true);
            fn_802088B4();
            mTransitionState = 1;
        }
        else if (mTransitionState == 2)
        {
            if (mSelectedButton == 0)
                fn_80207060(false);
            else if (mSelectedButton == 1)
                ShowCurrentCupRoundPage();
            else if (mSelectedButton == 2)
                ShowFirstCupPage();
            return;
        }
        else if (mTransitionState == 3)
        {
            return;
        }
    }

    if (!mPointerButtonsInitialized)
    {
        InitializePointerButtons();
        mPointerButtonsInitialized = true;
    }

    for (int i = 0; i < 4; ++i)
    {
        TLComponentInstance* pointer = GetPointerInstance(i);
        if (g_pFEInput->m_InputLockDepth == 0)
        {
            if ((unsigned int)i != gFEControllerIndex)
            {
                pointer->SetActiveSlide("waiting", true, false);
                continue;
            }
            if (mPointerHoverCounts[i] > 0 || mBackButton.mPointerInside[i])
                pointer->SetActiveSlide("A", true, false);
            else
                pointer->SetActiveSlide("cursor", true, false);
        }

        u8 valid = 1;
        FEPointerEvent event;
        event.mIndex = i;
        event.mPosition = GetPointerPosition(i, &valid);
        event.mPressed = g_pFEInput->JustPressed((eFEINPUT_PAD)i, 30, true, 0);
        event.mReleased = g_pFEInput->JustReleased((eFEINPUT_PAD)i, 30, true, 0);
        mPlayButton.HandlePointerEvent(&event);
        mScheduleButton.HandlePointerEvent(&event);
        mCupStatsButton.HandlePointerEvent(&event);
        mRulesButton.HandlePointerEvent(&event);
        if (mBackButton.UpdateBackButton(event, fDeltaT))
        {
            HandleCupBack(0);
            return;
        }
    }
}

void RoadToStrikersCupHubScene::SetTeamLogo(
    TLImageInstance* image, int captain)
{
    char imageName[24];
    nlSNPrintf(imageName, sizeof(imageName), "logos_TEAM_%s", GetCharacterInfo(GetCharacterIndexFromCaptain(captain)).mName);
    TLImageInstance* source = FEFinder<TLImageInstance, TLAT_IMAGE>::FindOrDefault(
        mPresentation, "art", "Layer", imageName);
    if (source->m_pTextureResource != 0)
        image->m_pTextureResource = source->m_pTextureResource;
}

void RoadToStrikersCupHubScene::UpdateCupRecordText(TLTextInstance* text)
{
    char winsString[4];
    char lossesString[4];
    char cupsString[4];
    unsigned short winsWideString[4];
    unsigned short lossesWideString[4];
    unsigned short cupsWideString[4];
    WideString unformatted;
    WideString formatted;

    CupRecord_8010C5C0& record = g_pCupManager->mCupRecord.mUnidentified86A0;
    int wins = record.mValues[0];
    int losses = record.mValues[1];
    int cups = record.mValues[2];

    nlSNPrintf(winsString, 4, "%d", wins);
    nlStrToWcs(winsString, winsWideString, 4);
    nlSNPrintf(lossesString, 4, "%d", losses);
    nlStrToWcs(lossesString, lossesWideString, 4);
    nlSNPrintf(cupsString, 4, "%d", cups);
    nlStrToWcs(cupsString, cupsWideString, 4);

    unformatted = WideString(LookupLocString("ROAD_HUB_TEAM_RECORD_STATS"));
    formatted = Format(
        unformatted, winsWideString, lossesWideString, cupsWideString);

    memcpy(mCupRecordText, formatted.c_str(), sizeof(mCupRecordText));
    text->SetString(mCupRecordText);
}

void RoadToStrikersCupHubScene::UpdateCupStatus()
{
    WideString formatted;
    int roundType = g_pCupManager->GetCurrentRoundType();
    FEPresentation* presentation = mFEScene->m_pFEPackage->GetPresentation();
    TLTextInstance* currentRound = FEFinder<TLTextInstance, TLAT_TEXT>::FindOrDefault(
        presentation->m_currentSlide, "Layer", "summary", "current round");

    if (roundType == 0)
    {
        int numRounds = g_pCupManager->GetNumRegularRounds();
        int round = g_pCupManager->GetCurrentRoundNumber() + 1;
        char total[4];
        char current[4];
        unsigned short totalWide[4];
        unsigned short currentWide[4];
        nlSNPrintf(total, 4, "%d", numRounds);
        nlSNPrintf(current, 4, "%d", round);
        nlStrToWcs(total, totalWide, 4);
        nlStrToWcs(current, currentWide, 4);
        formatted = Format(
            WideString(LookupLocString("CUP_STATUS_QUALIFY")),
            currentWide,
            totalWide);
    }
    else if (roundType == 1)
    {
        int numRounds = g_pCupManager->GetNumPlayoffRounds();
        int round = g_pCupManager->GetCurrentRoundNumber();
        if (round == numRounds - 3)
            formatted = WideString(LookupLocString("CUP_STATUS_QUARTER"));
        else if (round == numRounds - 2)
            formatted = WideString(LookupLocString("CUP_STATUS_SEMI"));
        else
            formatted = WideString(LookupLocString("CUP_STATUS_FINAL"));
    }
    else if (roundType == 2)
    {
        int wins = 0;
        int losses = 0;
        int round = g_pCupManager->GetCurrentRoundNumber();
        for (int i = 0; i < round; ++i)
        {
            BasicGameInfo* game = g_pCupManager->GetMatchupInfo(2, (short)i, 0);
            short captainSide = i == 1;
            short userSide = i != 1;
            short captainScore = game->GetFinalScore(captainSide);
            short userScore = game->GetFinalScore(userSide);
            if (captainScore > userScore)
                ++losses;
            else if (captainScore < userScore)
                ++wins;
        }

        char winsString[4];
        char lossesString[4];
        unsigned short winsWide[4];
        unsigned short lossesWide[4];
        nlSNPrintf(winsString, 4, "%d", wins);
        nlSNPrintf(lossesString, 4, "%d", losses);
        nlStrToWcs(winsString, winsWide, 4);
        nlStrToWcs(lossesString, lossesWide, 4);
        formatted = Format(WideString(LookupLocString("CUP_STATUS_BOSS")),
            winsWide,
            lossesWide);
    }

    memcpy(mCupStatusText, formatted.c_str(), sizeof(mCupStatusText));
    currentRound->SetString(mCupStatusText);
}

void RoadToStrikersCupHubScene::UpdateRoundMessage()
{
    FEPresentation* presentation = mPresentation;
    TLTextInstance* currentRound = FEFinder<TLTextInstance, TLAT_TEXT>::FindOrDefault(
        presentation->m_currentSlide,
        nlStringLowerHash("Layer"),
        nlStringLowerHash("summary"),
        nlStringLowerHash("current round"),
        0,
        0,
        0);
    if (g_pCupManager->mState == 4)
        currentRound->SetStringId("HUB_CONGRATS");
    else
        currentRound->SetStringId("HUB_ELIMINATED");

    TLTextInstance* nextMatch = FEFinder<TLTextInstance, TLAT_TEXT>::FindOrDefault(
        presentation->m_currentSlide,
        nlStringLowerHash("Layer"),
        nlStringLowerHash("summary"),
        nlStringLowerHash("NEXT MATCH"),
        0,
        0,
        0);
    nextMatch->SetStringId("STRIKERSCUP_LAST_MATCH");
}

void RoadToStrikersCupHubScene::UpdateCupHeading()
{
    WideString formatted;
    CupManager* cupManager = g_pCupManager;
    int captain;
    if (cupManager->GetCurrentRoundNumber() == -5
        && cupManager->mState == 4)
    {
        captain = cupManager->GetUserSelectedCupTeam();
    }
    else
    {
        captain = cupManager->fn_8010AFA4();
    }

    const CharacterInfo& character = GetCharacterInfo(GetCharacterIndexFromCaptain(captain));
    TLSlide* slide = mPresentation->GetActiveSlide();
    TLComponentInstance* cupHeading = FEFinder<TLComponentInstance, TLAT_COMPONENT>::FindOrDefault(
        slide, "Layer", "SUBHEADING");
    TLTextInstance* championText = FEFinder<TLTextInstance, TLAT_TEXT>::FindOrDefault(
        slide, "Layer", "subheading2");

    switch (cupManager->GetCurrentMode())
    {
    case 0:
        cupHeading->SetActiveSlide("FIRE CUP", true, false);
        break;
    case 1:
        cupHeading->SetActiveSlide("CRYSTAL CUP", true, false);
        break;
    case 2:
        cupHeading->SetActiveSlide("Slide1", true, false);
        break;
    }

    {
        WideString unformatted(
            LookupLocString("RTSC_HUB_CURRENT_CHAMP"));
        const unsigned short* characterName = LookupLocString(character.GetDisplayNameKey());
        formatted = Format(unformatted, characterName);
    }
    memcpy(mCupHeadingText, formatted.c_str(), sizeof(mCupHeadingText));
    championText->SetString(mCupHeadingText);
}

void RoadToStrikersCupHubScene::InitializePointerButtons()
{
    typedef Detail::MemFunImpl<void,
        void (RoadToStrikersCupHubScene::*)(unsigned int, void*)>
        PointerMethod;
    typedef BindExp3<void, PointerMethod, RoadToStrikersCupHubScene*, Placeholder<0>, Placeholder<1> > PointerBinding;

    FEPointerListener::Callback over(PointerBinding(
        MemFun(&RoadToStrikersCupHubScene::OnButtonPointerEnter), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback off(PointerBinding(
        MemFun(&RoadToStrikersCupHubScene::OnButtonPointerLeave), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback press(PointerBinding(
        MemFun(&RoadToStrikersCupHubScene::OnButtonPointerPress), this, Placeholder<0>(), Placeholder<1>()));

    SetPlayButtonBounds(&mPlayButton, mPlayButtonInstance);
    mPlayButton.SetPointerEnterCallback(over);
    mPlayButton.SetPointerLeaveCallback(off);
    mPlayButton.SetPointerPressCallback(press);

    mScheduleButton.SetInstanceBounds(
        mScheduleButtonInstance, true, 0.0f, 0.0f, 1.0f, 1.0f);
    mScheduleButton.SetPointerEnterCallback(over);
    mScheduleButton.SetPointerLeaveCallback(off);
    mScheduleButton.SetPointerPressCallback(press);

    mCupStatsButton.SetInstanceBounds(
        mCupStatsButtonInstance, true, 0.0f, 0.0f, 1.0f, 1.0f);
    mCupStatsButton.SetPointerEnterCallback(over);
    mCupStatsButton.SetPointerLeaveCallback(off);
    mCupStatsButton.SetPointerPressCallback(press);

    TLImageInstance* rulesOver = FEFinder<TLImageInstance, TLAT_IMAGE>::FindOrDefault(
        mRulesButtonInstance, "OVER", "list_high_250x60");
    feVector3 rulesPosition = mRulesButtonInstance->GetAssetPosition();
    mRulesButton.SetInstanceBounds(rulesOver, true, rulesPosition.f.x, rulesPosition.f.y, 1.0f, 1.0f);
    mRulesButton.SetPointerEnterCallback(over);
    mRulesButton.SetPointerLeaveCallback(off);
    mRulesButton.SetPointerPressCallback(press);
}

void RoadToStrikersCupHubScene::OnButtonPointerEnter(
    unsigned int index, void* context)
{
    ++mPointerHoverCounts[index];

    if (context == (void*)0)
    {
        if (!mPlayButton.HasOtherPointerState(1, index))
        {
            mPlayButtonInstance->SetActiveSlide("over", true, false);
            FEAudio::PlayAnimAudioEvent(0xAA73EF34, 0, 0, 1);
            mPlayButton.SetPointerState(1, index);
        }
    }
    else if (context == (void*)1)
    {
        if (!mScheduleButton.HasOtherPointerState(1, index))
        {
            mScheduleButtonInstance->SetActiveSlide("over", true, false);
            FEAudio::PlayAnimAudioEvent(0x96DEB5C3, 0, 0, 1);
            mScheduleButton.SetPointerState(1, index);
        }
    }
    else if (context == (void*)2)
    {
        if (!mCupStatsButton.HasOtherPointerState(1, index))
        {
            mCupStatsButtonInstance->SetActiveSlide("over", true, false);
            FEAudio::PlayAnimAudioEvent(0x96DEB5C3, 0, 0, 1);
            mCupStatsButton.SetPointerState(1, index);
        }
    }
    else if (context == (void*)3
             && !mRulesButton.HasOtherPointerState(1, index))
    {
        mRulesButtonInstance->SetActiveSlide("over", true, false);
        FEAudio::PlayAnimAudioEvent(0xACCDCA48, 0, 0, 1);
        mRulesButton.SetPointerState(1, index);
    }
}

void RoadToStrikersCupHubScene::OnButtonPointerLeave(
    unsigned int index, void* context)
{
    --mPointerHoverCounts[index];

    if (context == (void*)0)
    {
        if (!mPlayButton.HasOtherPointerState(1, index))
        {
            mPlayButtonInstance->SetActiveSlide("off", true, false);
            mPlayButton.SetPointerState(0, index);
        }
    }
    else if (context == (void*)1)
    {
        if (!mScheduleButton.HasOtherPointerState(1, index))
        {
            mScheduleButtonInstance->SetActiveSlide("off", true, false);
            mScheduleButton.SetPointerState(0, index);
        }
    }
    else if (context == (void*)2)
    {
        if (!mCupStatsButton.HasOtherPointerState(1, index))
        {
            mCupStatsButtonInstance->SetActiveSlide("off", true, false);
            mCupStatsButton.SetPointerState(0, index);
        }
    }
    else if (context == (void*)3
             && !mRulesButton.HasOtherPointerState(1, index))
    {
        mRulesButtonInstance->SetActiveSlide("off", true, false);
        mRulesButton.SetPointerState(0, index);
    }
}

void RoadToStrikersCupHubScene::OnButtonPointerPress(
    unsigned int, void* context)
{
    mButtonPressed = true;
    for (int i = 0; i < 4; ++i)
    {
        GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
    }

    unsigned int button = (unsigned int)context;
    if (button != 0)
    {
        FEAudio::PlayAnimAudioEvent(0xF0AFD586, 0, 0, 1);
    }

    if ((unsigned int)(button - 1) <= 1)
    {
        FEAudio::PlayAnimAudioEvent(0xE4023EE3, 0, 0, 1);
        FEAudio::PlayAnimAudioEvent(0xEA7AD449, 0, 0, 1);
    }

    if (button != 3)
    {
        FEAudio::PlayAnimAudioEvent(0x6E5C794C, 0, 0, 1);
        mTransitionState = 2;

        SHNavigation* navigation = GetNavigationScene();
        if (navigation != 0)
        {
            navigation->HideButtons();
        }

        mPresentation->SetActiveSlide("OUT", true);
        mSelectedButton = button;
    }
    else
    {
        ShowCupRulesPopup();
    }

    if (button == 0)
    {
        FEAudio::PlayAnimAudioEvent(0x4A51F95D, 0, 0, 1);
    }
}

int CupManager::GetPreviousGameTeam(int index) const
{
    return mPreviousGameTeams[index];
}

bool CupManager::ShouldShowCupPhasePopup() const
{
    return mShowCupPhasePopup;
}

void CupManager::SetShowCupPhasePopup(bool value)
{
    mShowCupPhasePopup = value;
}
