#include "Game/SH/SHNavigation.h"
#include "Game/SH/SHGameResults.h"

#include "Game/GameSceneManager.h"
#include "Game/FE/feFinder.h"
#include "Game/FE/feInput.h"
#include "Game/FE/fePackage.h"
#include "Game/FE/feScene.h"
#include "Game/FE/tlTextInstance.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"
#include "Game/SH/SHNavigation.h"

class SHNavigation;


static inline TLTextInstance* AsTextInstance(void* instance)
{
    if (instance == 0)
        return 0;
    return (TLTextInstance*)instance;
}

static inline void FillSummary(TU80209584Summary* summary, TU80209584StatsSource home, TU80209584StatsSource away,
    FEPresentation* presentation)
{
    summary->fn_802095D0(&home, &away, presentation);
}


GameResultsScene::GameResultsScene()
    : mTitleText(0)
    , mUnidentified5D8(false)
    , mUnidentifiedA10(0)
    , mUnidentifiedA14(0)
    , mUnidentifiedA18(0)
{
    mTitleBuffer[0] = 0;
    g_pFEInput->PushExclusiveInputLock(this, -1);
}

GameResultsScene::~GameResultsScene()
{
    g_pFEInput->PopExclusiveInputLock(this);
    SHNavigation* scene = GetNavigationScene();
    if (scene != 0)
        scene->RestoreButtonVisibility();
}

void GameResultsScene::fn_8020A494(UnidentifiedGameResultsData* data, UnidentifiedResultsListener* listener, UnidentifiedGameClock* clock)
{
    mUnidentifiedA10 = data;
    mUnidentifiedA14 = listener;
    mUnidentifiedA18 = clock;
}

void GameResultsScene::OnDoneTransitionComplete()
{
    SHStrikerTimesBase::OnDoneTransitionComplete();
    mUnidentifiedA14->Virtual28(1);
    GameSceneManager::Instance()->Pop();
}

void GameResultsScene::SceneCreated()
{
    SHStrikerTimesBase::SceneCreated();
    unsigned long titleHash;
    unsigned long summaryHash;
    unsigned long layerHash;
    FEPresentation* presentation;
    presentation = mFEScene->m_pFEPackage->GetPresentation();
    titleHash = nlStringLowerHash("title");
    summaryHash = nlStringLowerHash("game summary");
    layerHash = nlStringLowerHash("Layer");
    TLTextInstance* text = AsTextInstance(FEFindInstance(presentation, nlStringLowerHash("game summary"), layerHash, summaryHash, titleHash, 0, 0));
    if (text == 0)
        text = &UnidentifiedTLTextDefault::sInstance;
    mTitleText = text;
    mTitleText->SetStringId("CUP_GAME_RESULTS");
    SHNavigation* scene = GetNavigationScene();
    if (scene != 0)
        scene->HideButtons();
}

void GameResultsScene::Update(float dt)
{
    SHStrikerTimesBase::Update(dt);
    if (!mUnidentified5D8 && mUnidentifiedA10 != 0)
    {
        FillSummary(&mSummary, mUnidentifiedA10->mHome, mUnidentifiedA10->mAway, mFEScene->m_pFEPackage->GetPresentation());
    }
    UnidentifiedGameClock* clock = mUnidentifiedA18;
    if (clock != 0)
    {
        switch (clock->mUnidentified138)
        {
        case 2:
        {
            char buffer[0x20];
            int seconds = clock->mUnidentified13C;
            int minutes = seconds / 60;
            int remainder = seconds % 60;
            if (remainder < 10)
                nlSNPrintf(buffer, 0x20, "%d:0%d", minutes, remainder);
            else
                nlSNPrintf(buffer, 0x20, "%d:%d", minutes, remainder);
            nlStrToWcs(buffer, mTitleBuffer, 0x20);
            mTitleText->SetString(mTitleBuffer);
            break;
        }
        case 3:
            mTitleText->SetStringId("SUDDEN_DEATH");
            break;
        default:
            mTitleText->SetStringId("CUP_GAME_RESULTS");
            break;
        }
    }
}
