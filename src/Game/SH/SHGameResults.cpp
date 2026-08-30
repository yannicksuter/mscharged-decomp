#include "Game/SH/SHGameResults.h"

#include "Game/BaseGameSceneManager.h"
#include "Game/FE/feInput.h"
#include "Game/FE/fePackage.h"
#include "Game/FE/feScene.h"
#include "Game/FE/tlTextInstance.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"

extern "C" TLInstance* fn_8030677C(FEPresentation* pPresentation, unsigned long Level1, unsigned long Level2,
    unsigned long Level3, unsigned long Level4, unsigned long Level5, unsigned long Level6);
extern "C" bool fn_80253E18();
extern "C" void fn_802533F0();
extern "C" void fn_80253474();

extern BaseGameSceneManager* lbl_806E1838;

static inline TLTextInstance* AsTextInstance(TLInstance* instance)
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
    if (fn_80253E18())
        fn_802533F0();
}

void GameResultsScene::fn_8020A494(UnidentifiedGameResultsData* data, UnidentifiedResultsListener* listener, UnidentifiedGameClock* clock)
{
    mUnidentifiedA10 = data;
    mUnidentifiedA14 = listener;
    mUnidentifiedA18 = clock;
}

void GameResultsScene::SHSceneVirtual30()
{
    UnidentifiedSHSceneBase::SHSceneVirtual30();
    mUnidentifiedA14->Virtual28(1);
    lbl_806E1838->Pop();
}

void GameResultsScene::SceneCreated()
{
    UnidentifiedSHSceneBase::SceneCreated();
    unsigned long titleHash;
    unsigned long summaryHash;
    unsigned long layerHash;
    FEPresentation* presentation;
    presentation = mFEScene->m_pFEPackage->GetPresentation();
    titleHash = nlStringLowerHash("title");
    summaryHash = nlStringLowerHash("game summary");
    layerHash = nlStringLowerHash("Layer");
    TLTextInstance* text = AsTextInstance(fn_8030677C(presentation, nlStringLowerHash("game summary"), layerHash, summaryHash, titleHash, 0, 0));
    if (text == 0)
        text = &UnidentifiedFallbackTextInstance;
    mTitleText = text;
    mTitleText->SetStringId("CUP_GAME_RESULTS");
    if (fn_80253E18())
        fn_80253474();
}

void GameResultsScene::Update(float dt)
{
    UnidentifiedSHSceneBase::Update(dt);
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
