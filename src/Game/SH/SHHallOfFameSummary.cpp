#include <string.h>

#include "Game/SH/SHHallOfFameSummary.h"
#include "Game/DB/GameProgress.h"
#include "Game/FE/FEAudio.h"
#include "Game/FE/feFinder.h"
#include "Game/FE/feInput.h"
#include "Game/FE/fePackage.h"
#include "Game/FE/feScene.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlTextInstance.h"
#include "Game/FE/feDPD.h"
#include "Game/FE/fePageControls.h"
#include "Game/SH/SHHallOfFame.h"
#include "Game/SH/SHNavigation.h"
#include "NL/nlAlgorithm.h"
#include "NL/nlBasicString.h"
#include "NL/nlFormat.h"
#include "NL/nlLocalization.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"

typedef BasicString<unsigned short, Detail::TempStringAllocator> WideBasicString;

struct HallOfFameTrophyEntry
{
    const char* mStringId;
    unsigned int mUnlockFlag;
};

static HallOfFameTrophyEntry sTrophyEntries[] = {
    { "SUMMARY_FIRE_CUP", 0x1 },
    { "SUMMARY_FIRE_WALL", 0x8 },
    { "SUMMARY_FIRE_BOOT", 0x10 },
    { "SUMMARY_CRYSTAL_CUP", 0x2 },
    { "SUMMARY_CRYSTAL_WALL", 0x20 },
    { "SUMMARY_CRYSTAL_BOOT", 0x40 },
    { "SUMMARY_STRIKER_CUP", 0x4 },
    { "SUMMARY_STRIKER_WALL", 0x80 },
    { "SUMMARY_STRIKER_BOOT", 0x100 },
};

struct HallOfFameUnlockEntry
{
    const char* mStringId;
    bool (*mIsUnlocked)();
};

static HallOfFameUnlockEntry sUnlockEntries[] = {
    { "STADIUM_TICKER_WASTELANDS", IsWastelandsUnlocked },
    { "STADIUM_TICKER_LAVAPIT", IsLavaPitUnlocked },
    { "STADIUM_TICKER_DUMP", IsDumpUnlocked },
    { "STADIUM_TICKER_CRYSTALCANYON", IsCrystalCanyonUnlocked },
    { "STADIUM_TICKER_GALACTIC", IsGalacticStadiumUnlocked },
    { "STADIUM_TICKER_STORMSHIP", IsStormshipUnlocked },
    { "NAME_BOWSERJR", IsBowserJrUnlocked },
    { "NAME_DIDDYKONG", IsDiddyKongUnlocked },
    { "NAME_PETEY", IsPeteyUnlocked },
};

struct HallOfFameChallengeEntry
{
    const char* mStringId;
    int mChallenge;
};

static HallOfFameChallengeEntry sChallengeEntries[] = {
    { "NAME_MARIO", 10 },
    { "NAME_LUIGI", 11 },
    { "NAME_DK", 12 },
    { "NAME_PEACH", 13 },
    { "NAME_DAISY", 14 },
    { "NAME_WARIO", 15 },
    { "NAME_WALUIGI", 16 },
    { "NAME_YOSHI", 17 },
    { "NAME_BOWSER", 18 },
    { "NAME_PETEY", 19 },
    { "NAME_BOWSERJR", 20 },
    { "NAME_DIDDYKONG", 21 },
};


static inline const unsigned short* LookupLocString(const char* id)
{
    nlLocalization* localization = g_pLocalization;
    unsigned long hash = nlStringLowerHash(id);
    if (localization->m_LookupTable == 0)
    {
        return LocalizationTableNotFound;
    }

    nlLocalization::StringLookup* lookup
        = nlBSearch<nlLocalization::StringLookup, unsigned long>(
            hash, localization->m_LookupTable,
            (int)localization->m_pFile->StringCount);
    if (lookup != 0)
    {
        return localization->m_FirstString + lookup->StringOffset;
    }

    return MissingLocString;
}

SHHallOfFameSummary::SHHallOfFameSummary(int mode)
    : mMode(mode)
    , mBackButton()
    , mScrollBar()
{
    mFirstVisibleItem = 0;
    mInitialized = false;
    mNextPageRequested = false;
    mPreviousPageRequested = false;
    mState = 0;
    mPointerInsideCount[0] = 0;
    mPointerInsideCount[1] = 0;
    mPointerInsideCount[2] = 0;
    mPointerInsideCount[3] = 0;

    switch (mMode)
    {
    case 14:
        mItemCount = 9;
        break;
    case 15:
        mItemCount = 9;
        break;
    case 16:
        mItemCount = 12;
        break;
    }
}

SHHallOfFameSummary::~SHHallOfFameSummary()
{
}

void SHHallOfFameSummary::SceneCreated()
{
    SHNavigation* scene = GetNavigationScene();
    TLComponentInstance* screen = 0;
    TLComponentInstance* breadcrumbs = 0;
    if (scene != 0)
    {
        scene->SetButtons(15, false);
        screen = scene->GetButton(4);
        breadcrumbs = scene->GetButton(8);
        mPageControls = &scene->mPageControls;
        mPageControls->SetButtonState(1, true, true);
        mPageControls->SetButtonState(0, true, true);
    }

    mBackButton.SetButtonInstance(screen);
    SetHallOfFameBreadcrumbs(mMode, breadcrumbs);

    FEPresentation* presentation = mFEScene->m_pFEPackage->GetPresentation();
    TLComponentInstance* scrollbar = FEFinder<TLComponentInstance, 4>::Find(presentation->m_currentSlide,
            nlStringLowerHash("Layer"), nlStringLowerHash("summary"),
            nlStringLowerHash("scrollbar"), 0, 0, 0);
    mScrollBar.SetComponent(scrollbar);
    mScrollBar.SetRange(mItemCount - 7);
    mScrollBar.SetValue(mFirstVisibleItem);

    for (int i = 0; i < 4; i++)
    {
        GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
    }

    UpdateTitle();

    switch (mMode)
    {
    case 14:
        for (int i = 0; i < 7; i++)
        {
            UpdateRow( i, sTrophyEntries[i + mFirstVisibleItem].mStringId,
                IsUnlockFlagSet(sTrophyEntries[i + mFirstVisibleItem].mUnlockFlag));
        }
        break;
    case 15:
        for (int i = 0; i < 7; i++)
        {
            UpdateRow( i, sUnlockEntries[i + mFirstVisibleItem].mStringId,
                sUnlockEntries[i + mFirstVisibleItem].mIsUnlocked());
        }
        break;
    case 16:
        for (int i = 0; i < 7; i++)
        {
            UpdateRow( i, sChallengeEntries[i + mFirstVisibleItem].mStringId,
                g_pStrikerChallenge->IsUnlocked(sChallengeEntries[i + mFirstVisibleItem].mChallenge));
        }
        break;
    }
}

void SHHallOfFameSummary::Update(float fDeltaT)
{
    BaseSceneHandler::Update(fDeltaT);

    int state = mState;
    if (state == 0 || (unsigned int)(state - 2) <= 1)
    {
        TLSlide* slide = mPresentation->m_currentSlide;
        if (slide->GetCurrentTime() < slide->m_duration + slide->m_start)
        {
            for (int pad = 0; pad < 4; pad++)
            {
                GetPointerInstance(pad)->SetActiveSlide("waiting", true, false);
            }
            return;
        }

        if (state == 0)
        {
            mState = 1;
        }
        else if (state == 2)
        {
            if (mPreviousPageRequested)
            {
                CycleHallOfFameDetailPage(mMode, false);
                return;
            }
            if (mNextPageRequested)
            {
                CycleHallOfFameDetailPage(mMode, true);
            }
            return;
        }
        else if (state == 3)
        {
            LeaveHallOfFamePage(mMode);
            return;
        }
    }

    if (!mInitialized)
    {
        mInitialized = true;
    }

    if (!mScrollBar.mInitialized)
    {
        mScrollBar.Initialize();
    }

    for (unsigned int pad = 0; pad < 4; pad++)
    {
        TLComponentInstance* controller = GetPointerInstance(pad);
        bool processInput = true;
        if (g_pFEInput->m_InputLockDepth == 0)
        {
            if (pad != gFEControllerIndex)
            {
                controller->SetActiveSlide("waiting", true, false);
                processInput = false;
            }
            else if (mPointerInsideCount[pad] > 0)
            {
                controller->SetActiveSlide("A", true, false);
            }
            else
            {
                controller->SetActiveSlide("cursor", true, false);
            }
        }

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

            mPageControls->Update(event, fDeltaT);
            mScrollBar.Update(event, fDeltaT);

            if (mBackButton.UpdateBackButton(event, fDeltaT))
            {
                mState = 3;
                SHNavigation* scene = GetNavigationScene();
                if (scene != 0)
                {
                    scene->HideButtons();
                }
                mPresentation->SetActiveSlide("OUT", true);
                return;
            }

            if ((mPageControls->mPointerPressed[1]
                    || mPageControls->mPadPressed[1])
                || (mPageControls->mPointerPressed[0]
                    || mPageControls->mPadPressed[0]))
            {
                FEAudio::PlayAnimAudioEvent(0x375D885A, 0, 0, 1);
                FEAudio::PlayAnimAudioEvent(0x3050DD1E, 0, 0, 1);
                mState = 2;
                mPresentation->SetActiveSlide("out", true);

                if (mPageControls->mPointerPressed[1]
                    || mPageControls->mPadPressed[1])
                {
                    mPreviousPageRequested = true;
                }
                else if (mPageControls->mPointerPressed[0]
                    || mPageControls->mPadPressed[0])
                {
                    mNextPageRequested = true;
                }
                return;
            }
        }
    }

    if (mScrollBar.IsScrolling(1, 1))
    {
        ++mFirstVisibleItem;
        switch (mMode)
        {
        case 14:
            for (int i = 0; i < 7; i++)
            {
                UpdateRow( i, sTrophyEntries[i + mFirstVisibleItem].mStringId,
                    IsUnlockFlagSet(sTrophyEntries[i + mFirstVisibleItem].mUnlockFlag));
            }
            break;
        case 15:
            for (int i = 0; i < 7; i++)
            {
                UpdateRow( i, sUnlockEntries[i + mFirstVisibleItem].mStringId,
                    sUnlockEntries[i + mFirstVisibleItem].mIsUnlocked());
            }
            break;
        case 16:
            for (int i = 0; i < 7; i++)
            {
                UpdateRow( i, sChallengeEntries[i + mFirstVisibleItem].mStringId,
                    g_pStrikerChallenge->IsUnlocked(
                        sChallengeEntries[i + mFirstVisibleItem].mChallenge));
            }
            break;
        }
    }
    else if (mScrollBar.IsScrolling(0, 1))
    {
        --mFirstVisibleItem;
        switch (mMode)
        {
        case 14:
            for (int i = 0; i < 7; i++)
            {
                UpdateRow( i, sTrophyEntries[i + mFirstVisibleItem].mStringId,
                    IsUnlockFlagSet(sTrophyEntries[i + mFirstVisibleItem].mUnlockFlag));
            }
            break;
        case 15:
            for (int i = 0; i < 7; i++)
            {
                UpdateRow( i, sUnlockEntries[i + mFirstVisibleItem].mStringId,
                    sUnlockEntries[i + mFirstVisibleItem].mIsUnlocked());
            }
            break;
        case 16:
            for (int i = 0; i < 7; i++)
            {
                UpdateRow( i, sChallengeEntries[i + mFirstVisibleItem].mStringId,
                    g_pStrikerChallenge->IsUnlocked(
                        sChallengeEntries[i + mFirstVisibleItem].mChallenge));
            }
            break;
        }
    }
}

void SHHallOfFameSummary::UpdateTitle()
{
    WideBasicString title;

    FEPresentation* presentation = this->mFEScene->m_pFEPackage->GetPresentation();
    unsigned long subtitleComponentHash = nlStringLowerHash("SUBtitle");
    unsigned long subtitleHash = nlStringLowerHash("subtitle");
    unsigned long summaryHash = nlStringLowerHash("summary");
    TLTextInstance* titleText = FEFinder<TLTextInstance, 3>::Find(presentation->m_currentSlide,
            nlStringLowerHash("Layer"), summaryHash, subtitleHash,
            subtitleComponentHash, 0, 0);
    if (titleText == 0)
    {
        titleText = &UnidentifiedTLTextDefault::sInstance;
    }

    switch (this->mMode)
    {
    case 14:
        title = WideBasicString(LookupLocString("TITLE_SUMMARY_TROPHIES"));
        break;
    case 15:
        title = WideBasicString(LookupLocString("TITLE_SUMMARY_STADIUMS_CHARACTERS"));
        break;
    case 16:
        title = WideBasicString(LookupLocString("TITLE_SUMMARY_STRIKER_CHALLENGES"));
        break;
    }

    memcpy(this->mTitleBuffer, title.c_str(), sizeof(this->mTitleBuffer));
    titleText->SetString(this->mTitleBuffer);
}

void SHHallOfFameSummary::UpdateRow(int index, const char* stringId, bool unlocked)
{
    WideBasicString itemText;
    if (this->mMode == 16)
    {
        WideBasicString itemName(LookupLocString(stringId));
        WideBasicString format(LookupLocString("SUMMARY_CHALLENGES"));
        itemText = Format(format, itemName);
    }
    else
    {
        itemText = WideBasicString(LookupLocString(stringId));
    }

    char itemComponentName[8];
    nlSNPrintf(itemComponentName, sizeof(itemComponentName), "ITEM_%d", index);

    FEPresentation* presentation = this->mFEScene->m_pFEPackage->GetPresentation();
    unsigned long stat0Hash = nlStringLowerHash("stat_0");
    unsigned long challenge0Hash = nlStringLowerHash("CHALLENGE_0");
    unsigned long itemHash = nlStringLowerHash(itemComponentName);
    unsigned long summaryHash = nlStringLowerHash("summary");
    TLTextInstance* rowText = FEFinder<TLTextInstance, 3>::Find(presentation->m_currentSlide,
            nlStringLowerHash("Layer"), summaryHash, itemHash, challenge0Hash,
            stat0Hash, 0);
    if (rowText == 0)
    {
        rowText = &UnidentifiedTLTextDefault::sInstance;
    }

    unsigned long lockedUnlockedHash = nlStringLowerHash("lockedunlocked");
    challenge0Hash = nlStringLowerHash("CHALLENGE_0");
    itemHash = nlStringLowerHash(itemComponentName);
    summaryHash = nlStringLowerHash("summary");
    TLComponentInstance* lockState = FEFinder<TLComponentInstance, 4>::Find(presentation->m_currentSlide,
            nlStringLowerHash("Layer"), summaryHash, itemHash, challenge0Hash,
            lockedUnlockedHash, 0);
    if (lockState == 0)
    {
        lockState = &UnidentifiedTLComponentDefault::sInstance;
    }

    if (unlocked)
    {
        if (this->mMode == 14)
        {
            lockState->SetActiveSlide("claimed", true, false);
        }
        else if (this->mMode == 15)
        {
            lockState->SetActiveSlide("unlocked", true, false);
        }
        else
        {
            lockState->SetActiveSlide("completed", true, false);
        }
    }
    else
    {
        if (this->mMode == 14)
        {
            lockState->SetActiveSlide("unclaimed", true, false);
        }
        else if (this->mMode == 15)
        {
            lockState->SetActiveSlide("locked", true, false);
        }
        else
        {
            lockState->SetActiveSlide("completed", true, false);
        }
    }

    if (this->mMode == 16)
    {
        unsigned long groupHash = nlStringLowerHash("stat_1");
        lockedUnlockedHash = nlStringLowerHash("lockedunlocked");
        challenge0Hash = nlStringLowerHash("CHALLENGE_0");
        itemHash = nlStringLowerHash(itemComponentName);
        summaryHash = nlStringLowerHash("summary");
        TLTextInstance* statusText = FEFinder<TLTextInstance, 3>::Find(presentation->m_currentSlide,
                nlStringLowerHash("Layer"), summaryHash, itemHash,
                challenge0Hash, lockedUnlockedHash, groupHash);
        if (statusText == 0)
        {
            statusText = &UnidentifiedTLTextDefault::sInstance;
        }

        if (unlocked)
        {
            statusText->SetStringId("SUMMARY_COMPLETED");
        }
        else
        {
            statusText->SetStringId("SUMMARY_INCOMPLETE");
        }
    }

    memcpy(this->mItemTextBuffers[index], itemText.c_str(),
        sizeof(this->mItemTextBuffers[index]));
    rowText->SetString(this->mItemTextBuffers[index]);

    char tournamentName[16];
    nlSNPrintf(tournamentName, sizeof(tournamentName), "TOURNAMENT_%d",
        index + this->mFirstVisibleItem + 1);

    unsigned long numberHash = nlStringLowerHash("number");
    challenge0Hash = nlStringLowerHash("CHALLENGE_0");
    itemHash = nlStringLowerHash(itemComponentName);
    summaryHash = nlStringLowerHash("summary");
    TLTextInstance* tournamentText = FEFinder<TLTextInstance, 3>::Find(presentation->m_currentSlide,
            nlStringLowerHash("Layer"), summaryHash, itemHash, challenge0Hash,
            numberHash, 0);
    TLTextInstance* displayedTournamentText = tournamentText == 0
        ? &UnidentifiedTLTextDefault::sInstance
        : tournamentText;
    displayedTournamentText->SetStringId(tournamentName);
}
