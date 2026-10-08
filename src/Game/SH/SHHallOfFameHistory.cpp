#include "Game/SH/SHHallOfFame.h"
#include "Game/SH/SHNavigation.h"
#include "Game/Render/RLViewLayers.h"
#include "Game/FE/feCupFlow.h"
#include "Game/FE/feHelpFuncs_decl.h"
#include "Game/FE/FEAudio.h"
#include "Game/SH/SHHallOfFameHistory.h"
#include "Game/SH/SHHallOfFamePlayerCard.h"

#include "Game/GameSceneManager.h"
#include "Game/DB/GameProgress.h"
#include <string.h>
#include "Game/BaseGameSceneManager.h"
#include "Game/DB/CharacterInfo.h"
#include "Game/DB/GameProgress.h"
#include "Game/FE/feCupFlow.h"
#include "Game/FE/feAsyncImage.h"
#include "Game/FE/feInlineHasher.h"
#include "Game/FE/feFinder_impl.h"
#include "Game/FE/feInput.h"
#include "Game/FE/fePackage.h"
#include "Game/FE/feScene.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlImageInstance.h"
#include "Game/FE/tlTextInstance.h"
#include "Game/Render/FrontEndPresentation.h"
#include "NL/MemAlloc.h"
#include "NL/gl/glState.h"
#include "NL/nlAlgorithm.h"
#include "NL/nlBasicString.h"
#include "NL/nlFormat.h"
#include "NL/nlLocalization.h"
#include "NL/nlLocalizationLookup.h"
#include "NL/nlMemory.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"
#include "Game/FE/feDPD.h"
#include "Game/FE/feBackButton.h"
#include "Game/FE/FEAudio.h"
#include "Game/Render/RLViewLayers.h"
#include "Game/SH/SHNavigation.h"

#include <string.h>

char sHallOfFameResourcePath[] = "art/fe/HallOfFameUI.res";
const char* sHallOfFameResource = sHallOfFameResourcePath;

typedef BasicString<unsigned short, Detail::TempStringAllocator> WideBasicString;

SHHallOfFameHistory::SHHallOfFameHistory(int mode)
    : mMode(mode)
{
    int modeIndex;

    mSelectedHistoryIndex = 0;
    mHistoryCount = 0;
    mUnidentified324 = false;
    mState = 0;
    mImages[0] = 0;
    mImages[1] = 0;
    mImages[2] = 0;
    mImages[3] = 0;
    mImages[4] = 0;
    mHoverCounts[0] = 0;
    mHoverCounts[1] = 0;
    mHoverCounts[2] = 0;
    mHoverCounts[3] = 0;

    mImages[0] = new (0x20, true) AsyncImage(sHallOfFameResource, 0);
    mImageReady[0] = false;
    mImages[1] = new (0x20, true) AsyncImage(sHallOfFameResource, 0);
    mImageReady[1] = false;
    mImages[2] = new (0x20, true) AsyncImage(sHallOfFameResource, 0);
    mImageReady[2] = false;
    mImages[3] = new (0x20, true) AsyncImage(sHallOfFameResource, 0);
    mImageReady[3] = false;
    mImages[4] = new (0x20, true) AsyncImage(sHallOfFameResource, 0);
    mImageReady[4] = false;

    modeIndex = mode - 4;
    memset(mHistory, 0, sizeof(mHistory));

    int historyIndex;
    CupProgressRecord& cupRecord = CupManager::s_pInstance->mCupRecord;
    historyIndex = 11;
    if (cupRecord.mHistory.mWriteIndex[modeIndex] != 0)
    {
        historyIndex = cupRecord.mHistory.mWriteIndex[modeIndex] - 1;
    }

    while (mHistoryCount < 12)
    {
        if (cupRecord.mHistory.mRecords[modeIndex][historyIndex].IsEmpty())
        {
            break;
        }

        mHistory[mHistoryCount]
            = cupRecord.mHistory.mRecords[modeIndex][historyIndex];
        ++mHistoryCount;
        historyIndex = historyIndex > 0 ? historyIndex - 1 : 11;
    }

}

SHHallOfFameHistory::~SHHallOfFameHistory()
{
    if (mImages[0] != 0)
        delete mImages[0];
    if (mImages[1] != 0)
        delete mImages[1];
    if (mImages[2] != 0)
        delete mImages[2];
    if (mImages[3] != 0)
        delete mImages[3];
    if (mImages[4] != 0)
        delete mImages[4];
}

void SHHallOfFameHistory::SceneCreated()
{
    StopHallOfFameTrophyEffects();

    SHNavigation* object = GetNavigationScene();
    TLComponentInstance* screen = 0;
    if (object != 0)
    {
        object->HideButtons();
        screen = object->GetButton(4);
    }
    mNavigation.SetButtonInstance(screen);

    FEPresentation* presentation = mFEScene->m_pFEPackage->GetPresentation();
    TLComponentInstance* scrollbar = FEFinder<TLComponentInstance, 4>::Find(presentation->m_currentSlide,
            nlStringLowerHash("Layer"), nlStringLowerHash("scrollbar"), 0, 0, 0, 0);
    mScrollWidget.SetComponent(scrollbar);
    mScrollWidget.SetRange(mHistoryCount - 1);
    mScrollWidget.SetValue(mSelectedHistoryIndex);

    UpdateTitle();
    switch (mMode)
    {
    case 4:
    case 5:
    case 6:
        UpdateCupRecordText();
        break;
    case 7:
    case 8:
    case 9:
    case 10:
    case 11:
    case 12:
        UpdateGoalsRecordText();
        break;
    }
    UpdateDateText();
    UpdateTeamDisplay();
}

void SHHallOfFameHistory::Update(float fDeltaT)
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
            GetNavigationScene()->SetButtons(4, true);
            mState = 1;
            if (!mScrollWidget.mInitialized)
            {
                TLComponentInstance* scrollbar = FEFinder<TLComponentInstance, 4>::Find(
                        mPresentation->m_currentSlide, nlStringLowerHash("Layer"),
                        nlStringLowerHash("scrollbar"), 0, 0, 0, 0);
                mScrollWidget.SetComponent(scrollbar);
                mScrollWidget.Initialize();
            }
        }
        else if (state == 2)
        {
            return;
        }
        else if (state == 3)
        {
            LeaveHallOfFamePage(mMode);
            return;
        }
    }

    if (mImages[0]->Update(true))
        mImageReady[0] = true;
    if (mImages[1]->Update(true))
        mImageReady[1] = true;
    if (mImages[2]->Update(true))
        mImageReady[2] = true;
    if (mImages[3]->Update(true))
        mImageReady[3] = true;
    if (mImages[4]->Update(true))
        mImageReady[4] = true;

    if (mImageReady[0] && mImageReady[1] && mImageReady[2]
        && mImageReady[3] && mImageReady[4])
    {
        mImages[0]->mImageInstance->m_bVisible = true;
        mImages[4]->mImageInstance->m_bVisible = true;
        mImages[1]->mImageInstance->m_bVisible = true;
        mImages[2]->mImageInstance->m_bVisible = true;
        mImages[3]->mImageInstance->m_bVisible = true;
    }
    else
    {
        return;
    }

    for (unsigned int pad = 0; pad < 4; ++pad)
    {
        TLComponentInstance* controller = GetPointerInstance(pad);
        bool processInput;
        if (g_pFEInput->m_InputLockDepth == 0)
        {
            if (pad != gFEControllerIndex)
            {
                controller->SetActiveSlide("waiting", true, false);
                processInput = false;
                goto checkInput;
            }
            else if (mHoverCounts[pad] > 0)
            {
                controller->SetActiveSlide("A", true, false);
            }
            else
            {
                controller->SetActiveSlide("cursor", true, false);
            }
        }

        processInput = true;
    checkInput:
        if (processInput)
        {
            unsigned char valid = 1;
            FEPointerEvent event;
            event.mIndex = pad;
            event.mPosition = GetPointerPosition(pad, &valid);
            event.mPressed
                = g_pFEInput->JustPressed((eFEINPUT_PAD)pad, 0x1E, true, 0);
            event.mReleased
                = g_pFEInput->JustReleased((eFEINPUT_PAD)pad, 0x1E, true, 0);
            mScrollWidget.Update(event, fDeltaT);

            if (mNavigation.UpdateBackButton(event, fDeltaT))
            {
                FEAudio::PlayAnimAudioEvent(0x80BA0C86, 0, 0, 1);
                mState = 3;
                SHNavigation* object = GetNavigationScene();
                if (object != 0)
                {
                    object->HideButtons();
                }
                mPresentation->SetActiveSlide("OUT", true);
                return;
            }
        }
    }

    if (mScrollWidget.IsScrolling(1, 1))
    {
        ++mSelectedHistoryIndex;
        switch (mMode)
        {
        case 4:
        case 5:
        case 6:
            UpdateCupRecordText();
            break;
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
            UpdateGoalsRecordText();
            break;
        }
        UpdateDateText();
        UpdateTeamDisplay();
    }
    else if (mScrollWidget.IsScrolling(0, 1))
    {
        --mSelectedHistoryIndex;
        switch (mMode)
        {
        case 4:
        case 5:
        case 6:
            UpdateCupRecordText();
            break;
        case 7:
        case 8:
        case 9:
        case 10:
        case 11:
        case 12:
            UpdateGoalsRecordText();
            break;
        }
        UpdateDateText();
        UpdateTeamDisplay();
    }
}

void SHHallOfFameHistory::UpdateTitle()
{
    WideBasicString title;

    FEPresentation* presentation = mFEScene->m_pFEPackage->GetPresentation();
    unsigned long titlesHash = nlStringLowerHash("TITLES");
    unsigned long titleHash = nlStringLowerHash("TITLE");
    unsigned long historyHash = nlStringLowerHash("HISTORY");
    TLTextInstance* titleText = FEFinder<TLTextInstance, 3>::Find(presentation->m_currentSlide,
            nlStringLowerHash("Layer"), historyHash, titleHash, titlesHash, 0, 0);
    if (titleText == 0)
    {
        titleText = &TLTextDefault::sInstance;
    }

    switch (mMode)
    {
    case 4:
        title = WideBasicString(LookupLocString("HOF_HISTORY_TITLE_FIRE_CUP"));
        break;
    case 6:
        title = WideBasicString(LookupLocString("HOF_HISTORY_TITLE_STRIKER_CUP"));
        break;
    case 5:
        title = WideBasicString(LookupLocString("HOF_HISTORY_TITLE_CRYSTAL_CUP"));
        break;
    case 7:
        title = WideBasicString(LookupLocString("HOF_HISTORY_TITLE_FIRE_BRICK_WALL"));
        break;
    case 8:
        title = WideBasicString(LookupLocString("HOF_HISTORY_TITLE_FIRE_GOLDEN_BOOT"));
        break;
    case 11:
        title = WideBasicString(LookupLocString("HOF_HISTORY_TITLE_STRIKER_BRICK_WALL"));
        break;
    case 12:
        title = WideBasicString(LookupLocString("HOF_HISTORY_TITLE_STRIKER_GOLDEN_BOOT"));
        break;
    case 9:
        title = WideBasicString(LookupLocString("HOF_HISTORY_TITLE_CRYSTAL_BRICK_WALL"));
        break;
    case 10:
        title = WideBasicString(LookupLocString("HOF_HISTORY_TITLE_CRYSTAL_GOLDEN_BOOT"));
        break;
    }

    memcpy(mTitleText, title.c_str(), sizeof(mTitleText));
    titleText->SetString(mTitleText);
}

void SHHallOfFameHistory::UpdateCupRecordText()
{
    FEPresentation* presentation = mFEScene->m_pFEPackage->GetPresentation();
    unsigned long recordHash = nlStringLowerHash("RECORD");
    unsigned long historyHash = nlStringLowerHash("HISTORY");
    TLTextInstance* recordText = FEFinder<TLTextInstance, 3>::Find(presentation->m_currentSlide,
            nlStringLowerHash("Layer"), historyHash, recordHash, 0, 0, 0);
    if (recordText == 0)
    {
        recordText = &TLTextDefault::sInstance;
    }

    char winsString[4];
    char lossesString[4];
    char overtimeLossesString[4];
    unsigned short winsWideString[4];
    unsigned short lossesWideString[4];
    unsigned short overtimeLossesWideString[4];
    WideBasicString unformatted;
    WideBasicString formatted;

    CupHistoryRecord& record = mHistory[mSelectedHistoryIndex];
    int wins = record.mWins;
    int losses = record.mLosses;
    int overtimeLosses = record.mOvertimeLosses;

    nlSNPrintf(winsString, 4, "%d", wins);
    nlStrToWcs(winsString, winsWideString, 4);
    nlSNPrintf(lossesString, 4, "%d", losses);
    nlStrToWcs(lossesString, lossesWideString, 4);
    nlSNPrintf(overtimeLossesString, 4, "%d", overtimeLosses);
    nlStrToWcs(overtimeLossesString, overtimeLossesWideString, 4);

    unformatted = WideBasicString(LookupLocString("ROAD_HUB_TEAM_RECORD_STATS"));
    formatted = Format(unformatted, winsWideString, lossesWideString, overtimeLossesWideString);

    memcpy(mRecordText, formatted.c_str(), sizeof(mRecordText));
    recordText->SetString(mRecordText);
}

void SHHallOfFameHistory::UpdateGoalsRecordText()
{
    char goalsString[4];
    unsigned short goalsWideString[4];
    WideBasicString unformatted;
    WideBasicString formatted;

    CupHistoryRecord& record = mHistory[mSelectedHistoryIndex];
    int goals = record.mGoals;
    nlSNPrintf(goalsString, 4, "%d", goals);
    nlStrToWcs(goalsString, goalsWideString, 4);

    FEPresentation* presentation = mFEScene->m_pFEPackage->GetPresentation();
    TLTextInstance* goalsText = 0;
    switch (mMode)
    {
    case 7:
    case 9:
    case 11:
    {
        unformatted = WideBasicString(LookupLocString("HOF_GOALS_AGAINST"));

        goalsText = FEFinder<TLTextInstance, 3>::FindOrDefault(presentation->m_currentSlide,
                "Layer", "HISTORY", "GOALS FOR");
        goalsText->m_bVisible = false;

        unsigned long goalsAgainstHash = nlStringLowerHash("GOALS AGAINST");
        unsigned long historyHash = nlStringLowerHash("HISTORY");
        goalsText = FEFinder<TLTextInstance, 3>::Find(presentation->m_currentSlide,
                nlStringLowerHash("Layer"), historyHash, goalsAgainstHash, 0, 0, 0);
        if (goalsText == 0)
        {
            goalsText = &TLTextDefault::sInstance;
        }
        break;
    }
    case 8:
    case 10:
    case 12:
    {
        unformatted = WideBasicString(LookupLocString("HOF_GOALS_FOR"));

        unsigned long goalsAgainstHash = nlStringLowerHash("GOALS AGAINST");
        unsigned long historyHash = nlStringLowerHash("HISTORY");
        goalsText = FEFinder<TLTextInstance, 3>::FindOrDefault(presentation->m_currentSlide,
                nlStringLowerHash("Layer"), historyHash, goalsAgainstHash, 0, 0, 0);
        goalsText->m_bVisible = false;

        unsigned long goalsForHash = nlStringLowerHash("GOALS FOR");
        historyHash = nlStringLowerHash("HISTORY");
        goalsText = FEFinder<TLTextInstance, 3>::Find(presentation->m_currentSlide,
                nlStringLowerHash("Layer"), historyHash, goalsForHash, 0, 0, 0);
        if (goalsText == 0)
        {
            goalsText = &TLTextDefault::sInstance;
        }
        break;
    }
    }

    formatted = Format(unformatted, goalsWideString);
    memcpy(mRecordText, formatted.c_str(), sizeof(mRecordText));
    goalsText->SetString(mRecordText);
}

void SHHallOfFameHistory::UpdateDateText()
{
    char yearString[5];
    char monthString[5];
    char dayString[5];
    unsigned short yearWideString[5];
    unsigned short monthWideString[5];
    unsigned short dayWideString[5];

    FEPresentation* presentation = mFEScene->m_pFEPackage->GetPresentation();
    unsigned long dateHash = nlStringLowerHash("DATE");
    unsigned long historyHash = nlStringLowerHash("HISTORY");
    TLTextInstance* dateText = FEFinder<TLTextInstance, 3>::Find(presentation->m_currentSlide,
            nlStringLowerHash("Layer"), historyHash, dateHash, 0, 0, 0);
    if (dateText == 0)
    {
        dateText = &TLTextDefault::sInstance;
    }

    WideBasicString unformatted;
    WideBasicString formatted;

    CupHistoryRecord& record = mHistory[mSelectedHistoryIndex];
    int month = record.mMonth + 1;
    int day = record.mDay + 1;
    int year = record.mYearOffset + 2000;

    nlSNPrintf(yearString, 5, "%.4d", year);
    nlStrToWcs(yearString, yearWideString, 5);
    nlSNPrintf(monthString, 5, "%.2d", month);
    nlStrToWcs(monthString, monthWideString, 5);
    nlSNPrintf(dayString, 5, "%.2d", day);
    nlStrToWcs(dayString, dayWideString, 5);

    unformatted = WideBasicString(LookupLocString("DATE_STAMP"));
    formatted
        = Format(unformatted, dayWideString, monthWideString, yearWideString);
    memcpy(mDateText, formatted.c_str(), sizeof(mDateText));
    dateText->SetString(mDateText);
}

void SHHallOfFameHistory::UpdateTeamDisplay()
{
    int value0 = mHistory[mSelectedHistoryIndex].mCaptain;
    int value1 = mHistory[mSelectedHistoryIndex].mSidekick1;
    int value2 = mHistory[mSelectedHistoryIndex].mSidekick2;
    int value3 = mHistory[mSelectedHistoryIndex].mSidekick3;
    WideBasicString string;
    switch (value0)
    {
    case 0:
        string = g_pLocalization->GetString("NAME_MARIO");
        break;
    case 1:
        string = g_pLocalization->GetString("NAME_BOWSER");
        break;
    case 2:
        string = g_pLocalization->GetString("NAME_DAISY");
        break;
    case 3:
        string = g_pLocalization->GetString("NAME_DK");
        break;
    case 4:
        string = g_pLocalization->GetString("NAME_LUIGI");
        break;
    case 5:
        string = g_pLocalization->GetString("NAME_PEACH");
        break;
    case 6:
        string = g_pLocalization->GetString("NAME_WALUIGI");
        break;
    case 7:
        string = g_pLocalization->GetString("NAME_WARIO");
        break;
    case 8:
        string = g_pLocalization->GetString("NAME_YOSHI");
        break;
    case 9:
        string = g_pLocalization->GetString("NAME_BOWSERJR");
        break;
    case 10:
        string = g_pLocalization->GetString("NAME_DIDDYKONG");
        break;
    case 11:
        string = g_pLocalization->GetString("NAME_PETEY");
        break;
    }

    FEPresentation* presentation = mFEScene->GetPackage()->GetPresentation();
    TLTextInstance* text = FEFinder<TLTextInstance, 3>::FindOrDefault(presentation->GetActiveSlide(),
        "Layer", "HISTORY", "NAMES", "NAMES");
    memcpy(mTeamNameText, string.c_str(), sizeof(mTeamNameText));
    text->SetString(mTeamNameText);

    const CharacterInfo& captain = GetCharacterInfo(GetCharacterIndexFromCaptain(value0));
    TLImageInstance* image;
    char path[0x80];

    image = FEFinder<TLImageInstance, 2>::FindOrDefault<TLSlide>(presentation->GetActiveSlide(), "Layer", "HISTORY", "CAPTAIN");
    mImages[0]->SetImageInstance(image);
    image->SetVisible(false);
    nlSNPrintf(path, sizeof(path), "fe/hof_history_images/hof_history_%s", captain.GetName());
    mImages[0]->QueueLoad(path, false);

    image = FEFinder<TLImageInstance, 2>::FindOrDefault<TLSlide>(presentation->GetActiveSlide(), "Layer", "HISTORY", "KRITTER");
    mImages[4]->SetImageInstance(image);
    image->SetVisible(false);
    nlSNPrintf(path, sizeof(path), "fe/hof_history_images/hof_history_kritter_%s", captain.GetName());
    mImages[4]->QueueLoad(path, false);

    const CharacterInfo* sidekick = &GetCharacterInfo(GetCharacterIndexFromSidekick(value1));
    image = FEFinder<TLImageInstance, 2>::FindOrDefault<TLSlide>(presentation->GetActiveSlide(), "Layer", "HISTORY", "SK1");
    mImages[1]->SetImageInstance(image);
    image->SetVisible(false);
    nlSNPrintf(path, sizeof(path), "fe/hof_history_images/hof_history_sidekicks_%s_%s",
        sidekick->GetName(), captain.GetName());
    mImages[1]->QueueLoad(path, false);

    sidekick = &GetCharacterInfo(GetCharacterIndexFromSidekick(value2));
    image = FEFinder<TLImageInstance, 2>::FindOrDefault<TLSlide>(presentation->GetActiveSlide(), "Layer", "HISTORY", "SK2");
    mImages[2]->SetImageInstance(image);
    image->SetVisible(false);
    nlSNPrintf(path, sizeof(path), "fe/hof_history_images/hof_history_sidekicks_%s_%s",
        sidekick->GetName(), captain.GetName());
    mImages[2]->QueueLoad(path, false);

    sidekick = &GetCharacterInfo(GetCharacterIndexFromSidekick(value3));
    image = FEFinder<TLImageInstance, 2>::FindOrDefault<TLSlide>(presentation->GetActiveSlide(), "Layer", "HISTORY", "SK3");
    mImages[3]->SetImageInstance(image);
    image->SetVisible(false);
    nlSNPrintf(path, sizeof(path), "fe/hof_history_images/hof_history_sidekicks_%s_%s",
        sidekick->GetName(), captain.GetName());
    mImages[3]->QueueLoad(path, false);

    mImageReady[0] = false;
    mImageReady[1] = false;
    mImageReady[2] = false;
    mImageReady[3] = false;
    mImageReady[4] = false;
}

SHHallOfFamePlayerCard::SHHallOfFamePlayerCard(int mode)
    : mMode(mode)
    , mNavigation()
    , mSlideFinished(false)
    , mFrontImage(sHallOfFameResource, 0)
    , mBackImage(sHallOfFameResource, 0)
{
    mFrontImageReady = false;
    mBackImageReady = false;
    mUnidentified0FC[0] = 0;
    mUnidentified0FC[1] = 0;
    mUnidentified0FC[2] = 0;
    mUnidentified0FC[3] = 0;

    mCardIndex = GetHallOfFamePlayerCardIndex();
    mIsUnlocked = IsUnlockFlagSet(GetHallOfFamePlayerUnlockFlag(mCardIndex));
}

SHHallOfFamePlayerCard::~SHHallOfFamePlayerCard()
{
}

void SHHallOfFamePlayerCard::SceneCreated()
{
    StopHallOfFameTrophyEffects();

    SHNavigation* object = GetNavigationScene();
    TLComponentInstance* screen = 0;
    if (object != 0)
    {
        object->SetButtons(4, true);
        screen = object->GetButton(4);
    }
    mNavigation.SetButtonInstance(screen);

    FEPresentation* presentation = mFEScene->m_pFEPackage->GetPresentation();
    TLComponentInstance* playerCard = FEFinder<TLComponentInstance, 4>::Find(presentation->m_currentSlide,
            nlStringLowerHash("Layer"), nlStringLowerHash("player card"), 0, 0, 0, 0);
    if (playerCard == 0)
    {
        playerCard = &TLComponentDefault::sInstance;
    }

    if (mIsUnlocked)
    {
        playerCard->SetActiveSlide("HAVE", true, false);
    }
    else
    {
        playerCard->SetActiveSlide("NOT HAVE", true, false);
    }

    UpdateText();
    UpdateImages();
}

void SHHallOfFamePlayerCard::Update(float fDeltaT)
{
    if (!mFrontImageReady || !mBackImageReady)
    {
        if (!mFrontImageReady)
        {
            mFrontImageReady = mFrontImage.Update(true);
        }
        if (!mBackImageReady)
        {
            mBackImageReady = mBackImage.Update(true);
        }
        if (mFrontImageReady && mBackImageReady)
        {
            FEAudio::PlayAnimAudioEvent(0x12057B21, 0, 0, 1);
        }
        return;
    }

    BaseSceneHandler::Update(fDeltaT);

    if (!mSlideFinished)
    {
        TLSlide* slide = mPresentation->m_currentSlide;
        if (slide->GetCurrentTime() < slide->GetStartTime() + slide->GetDuration())
        {
            return;
        }
        mSlideFinished = true;
    }

    for (unsigned int pad = 0; pad < 4; ++pad)
    {
        TLComponentInstance* controller = GetPointerInstance(pad);
        bool processInput;
        if (g_pFEInput->m_InputLockDepth == 0)
        {
            if (pad != gFEControllerIndex)
            {
                controller->SetActiveSlide("waiting", true, false);
                processInput = false;
                goto checkInput;
            }

            if (mUnidentified0FC[pad] > 0)
            {
                controller->SetActiveSlide("A", true, false);
            }
            else
            {
                controller->SetActiveSlide("cursor", true, false);
            }
        }
        processInput = true;

    checkInput:
        if (processInput)
        {
            unsigned char valid = 1;
            FEPointerEvent event;
            event.mIndex = pad;
            event.mPosition = GetPointerPosition(pad, &valid);
            event.mPressed
                = g_pFEInput->JustPressed((eFEINPUT_PAD)pad, 0x1E, true, 0);
            event.mReleased
                = g_pFEInput->JustReleased((eFEINPUT_PAD)pad, 0x1E, true, 0);

            if (mNavigation.UpdateBackButton(event, fDeltaT))
            {
                LeaveHallOfFamePage(mMode);
                return;
            }
        }
    }
}

void SHHallOfFamePlayerCard::UpdateText()
{
    typedef BasicString<unsigned short, Detail::TempStringAllocator> WideBasicString;

    WideBasicString title;
    WideBasicString description;
    WideBasicString name;

    FEPresentation* presentation = mFEScene->GetPackage()->GetPresentation();
    TLTextInstance* titleText = FEFinder<TLTextInstance, 3>::FindOrDefault(
        presentation->GetActiveSlide(), "Layer", "player card", "title");
    TLTextInstance* nameText = FEFinder<TLTextInstance, 3>::FindOrDefault(
        presentation->GetActiveSlide(), "Layer", "player card", "name");
    TLTextInstance* descriptionText = FEFinder<TLTextInstance, 3>::FindOrDefault(
        presentation->GetActiveSlide(), "Layer", "player card", "description");

    switch (mCardIndex)
    {
    case 0:
        title = g_pLocalization->GetString("PLAYER_CARDS_MARIO_TITLE");
        name = g_pLocalization->GetString("NAME_MARIO");
        if (mIsUnlocked)
            description = g_pLocalization->GetString("PLAYER_CARDS_MARIO_DESCRIPTION");
        else
            description = g_pLocalization->GetString("PLAYER_CARDS_MARIO_LOCKED");
        break;
    case 1:
        title = g_pLocalization->GetString("PLAYER_CARDS_LUIGI_TITLE");
        name = g_pLocalization->GetString("NAME_LUIGI");
        if (mIsUnlocked)
            description = g_pLocalization->GetString("PLAYER_CARDS_LUIGI_DESCRIPTION");
        else
            description = g_pLocalization->GetString("PLAYER_CARDS_LUIGI_LOCKED");
        break;
    case 2:
        title = g_pLocalization->GetString("PLAYER_CARDS_DONKEYKONG_TITLE");
        name = g_pLocalization->GetString("NAME_DK");
        if (mIsUnlocked)
            description
                = g_pLocalization->GetString("PLAYER_CARDS_DONKEYKONG_DESCRIPTION");
        else
            description = g_pLocalization->GetString("PLAYER_CARDS_DONKEYKONG_LOCKED");
        break;
    case 3:
        title = g_pLocalization->GetString("PLAYER_CARDS_PEACH_TITLE");
        name = g_pLocalization->GetString("NAME_PEACH");
        if (mIsUnlocked)
            description = g_pLocalization->GetString("PLAYER_CARDS_PEACH_DESCRIPTION");
        else
            description = g_pLocalization->GetString("PLAYER_CARDS_PEACH_LOCKED");
        break;
    case 4:
        title = g_pLocalization->GetString("PLAYER_CARDS_DAISY_TITLE");
        name = g_pLocalization->GetString("NAME_DAISY");
        if (mIsUnlocked)
            description = g_pLocalization->GetString("PLAYER_CARDS_DAISY_DESCRIPTION");
        else
            description = g_pLocalization->GetString("PLAYER_CARDS_DAISY_LOCKED");
        break;
    case 5:
        title = g_pLocalization->GetString("PLAYER_CARDS_WARIO_TITLE");
        name = g_pLocalization->GetString("NAME_WARIO");
        if (mIsUnlocked)
            description = g_pLocalization->GetString("PLAYER_CARDS_WARIO_DESCRIPTION");
        else
            description = g_pLocalization->GetString("PLAYER_CARDS_WARIO_LOCKED");
        break;
    case 6:
        title = g_pLocalization->GetString("PLAYER_CARDS_WALUIGI_TITLE");
        name = g_pLocalization->GetString("NAME_WALUIGI");
        if (mIsUnlocked)
            description = g_pLocalization->GetString("PLAYER_CARDS_WALUIGI_DESCRIPTION");
        else
            description = g_pLocalization->GetString("PLAYER_CARDS_WALUIGI_LOCKED");
        break;
    case 7:
        title = g_pLocalization->GetString("PLAYER_CARDS_YOSHI_TITLE");
        name = g_pLocalization->GetString("NAME_YOSHI");
        if (mIsUnlocked)
            description = g_pLocalization->GetString("PLAYER_CARDS_YOSHI_DESCRIPTION");
        else
            description = g_pLocalization->GetString("PLAYER_CARDS_YOSHI_LOCKED");
        break;
    case 8:
        title = g_pLocalization->GetString("PLAYER_CARDS_BOWSER_TITLE");
        name = g_pLocalization->GetString("NAME_BOWSER");
        if (mIsUnlocked)
            description = g_pLocalization->GetString("PLAYER_CARDS_BOWSER_DESCRIPTION");
        else
            description = g_pLocalization->GetString("PLAYER_CARDS_BOWSER_LOCKED");
        break;
    case 9:
        title = g_pLocalization->GetString("PLAYER_CARDS_PETEY_TITLE");
        name = g_pLocalization->GetString("NAME_PETEY");
        if (mIsUnlocked)
            description = g_pLocalization->GetString("PLAYER_CARDS_PETEY_DESCRIPTION");
        else
            description = g_pLocalization->GetString("PLAYER_CARDS_PETEY_LOCKED");
        break;
    case 10:
        title = g_pLocalization->GetString("PLAYER_CARDS_BOWSERJR_TITLE");
        name = g_pLocalization->GetString("NAME_BOWSERJR");
        if (mIsUnlocked)
            description = g_pLocalization->GetString("PLAYER_CARDS_BOWSERJR_DESCRIPTION");
        else
            description = g_pLocalization->GetString("PLAYER_CARDS_BOWSERJR_LOCKED");
        break;
    case 11:
        title = g_pLocalization->GetString("PLAYER_CARDS_DIDDYKONG_TITLE");
        name = g_pLocalization->GetString("NAME_DIDDYKONG");
        if (mIsUnlocked)
            description
                = g_pLocalization->GetString("PLAYER_CARDS_DIDDYKONG_DESCRIPTION");
        else
            description = g_pLocalization->GetString("PLAYER_CARDS_DIDDYKONG_LOCKED");
        break;
    }

    memcpy(mTitleText, title.c_str(),
        sizeof(mTitleText));
    titleText->SetString(mTitleText);
    memcpy(mNameText, name.c_str(), sizeof(mNameText));
    nameText->SetString(mNameText);
    memcpy(mDescriptionText, description.c_str(),
        sizeof(mDescriptionText));
    descriptionText->SetString(mDescriptionText);
}

void SHHallOfFamePlayerCard::UpdateImages()
{
    FEPresentation* presentation;
    TLImageInstance* image0;
    TLImageInstance* image1;
    char path0[64];
    char path1[64];

    presentation = mFEScene->m_pFEPackage->GetPresentation();
    {
        unsigned long imageHash;
        unsigned long playerCardHash;
        imageHash = nlStringLowerHash("00_dummy_texture");
        playerCardHash = nlStringLowerHash("player card");
        image0 = FEFinder<TLImageInstance, 2>::Find(presentation->m_currentSlide,
                nlStringLowerHash("Layer"), playerCardHash, imageHash, 0, 0, 0);
    }
    if (image0 == 0)
    {
        image0 = &TLImageDefault::sInstance;
    }

    {
        unsigned long imageHash = nlStringLowerHash("00_dummy_texture_positions");
        unsigned long playerCardHash = nlStringLowerHash("player card");
        image1 = FEFinder<TLImageInstance, 2>::Find(presentation->m_currentSlide,
                nlStringLowerHash("Layer"), playerCardHash, imageHash, 0, 0, 0);
    }
    if (image1 == 0)
    {
        image1 = &TLImageDefault::sInstance;
    }

    const char* name0 = 0;
    const char* name1 = 0;
    switch (mCardIndex)
    {
    case 0:
        name0 = "PLAYER_CARDS_MARIO";
        name1 = "PLAYER_CARDS_BACK_MARIO";
        break;
    case 1:
        name0 = "PLAYER_CARDS_LUIGI";
        name1 = "PLAYER_CARDS_BACK_LUIGI";
        break;
    case 2:
        name0 = "PLAYER_CARDS_DONKEYKONG";
        name1 = "PLAYER_CARDS_BACK_DONKEYKONG";
        break;
    case 3:
        name0 = "PLAYER_CARDS_PEACH";
        name1 = "PLAYER_CARDS_BACK_PEACH";
        break;
    case 4:
        name0 = "PLAYER_CARDS_DAISY";
        name1 = "PLAYER_CARDS_BACK_DAISY";
        break;
    case 5:
        name0 = "PLAYER_CARDS_WARIO";
        name1 = "PLAYER_CARDS_BACK_WARIO";
        break;
    case 6:
        name0 = "PLAYER_CARDS_WALUIGI";
        name1 = "PLAYER_CARDS_BACK_WALUIGI";
        break;
    case 7:
        name0 = "PLAYER_CARDS_YOSHI";
        name1 = "PLAYER_CARDS_BACK_YOSHI";
        break;
    case 8:
        name0 = "PLAYER_CARDS_BOWSER";
        name1 = "PLAYER_CARDS_BACK_BOWSER";
        break;
    case 9:
        name0 = "PLAYER_CARDS_PETEY";
        name1 = "PLAYER_CARDS_BACK_PETEY";
        break;
    case 10:
        name0 = "PLAYER_CARDS_BOWSERJR";
        name1 = "PLAYER_CARDS_BACK_BOWSERJR";
        break;
    case 11:
        name0 = "PLAYER_CARDS_DIDDYKONG";
        name1 = "PLAYER_CARDS_BACK_DIDDYKONG";
        break;
    }

    nlSNPrintf(path0, sizeof(path0), "fe/screens/images/%s", name0);
    nlSNPrintf(path1, sizeof(path1), "fe/screens/images/%s", name1);

    mFrontImage.mImageInstance = image0;
    mBackImage.mImageInstance = image1;
    mFrontImage.QueueLoad(path0, false);
    mBackImage.QueueLoad(path1, false);
}
