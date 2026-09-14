#include "NL/nlBasicString.inl"
#include "Game/SH/SHNavigation.h"
#include "Game/SH/SHCupHub.h"
#include "Game/FE/FEAudio.h"

#include "Game/DB/GameProgress.h"
#include "Game/DB/CharacterInfo.h"
#include "Game/DB/BasicGameInfo.h"
#include "Game/FE/fePresentation.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/feDPD.h"
#include "Game/FE/feFinder.inl"
#include "Game/FE/feScene.h"
#include "Game/FE/fePackage.h"
#include "Game/FE/feInput.h"
#include "Game/FE/feCupFlow.h"
#include "Game/FE/feHelpFuncs_decl.h"
#include "Game/FE/fePopupMenu.h"
#include "Game/GameSceneManager.h"
#include "Game/SH/SHGameResults.h"
#include "NL/nlBind.h"
#include "NL/nlFunction.inl"
#include "NL/nlPrint.h"
#include "NL/nlFormat.h"
#include "NL/nlLocalizationLookup.h"

class SHNavigation;

extern "C" void fn_80208950(TLComponentInstance*, unsigned short*, unsigned long);
extern "C" void fn_80207724(bool);
extern "C" void fn_802088B4();
extern "C" void fn_80207060(bool);

typedef BasicString<unsigned short, Detail::TempStringAllocator> WideString;

static const char* lbl_8051B3E0[] = {
    "matchups", "matchups2", "matchups3", "matchups4"
};

CupHubScene::CupHubScene()
    : mUnidentified300(false)
    , mUnidentified304(0)
    , mUnidentified67C(false)
    , mUnidentified67D(true)
    , mUnidentified67E(false)
    , mUnidentified67F(false)
    , mNavigationComponent()
    , mRulesButton(0)
    , mUnidentified890(false)
    , mUnidentified894(0)
{
    mRulesComponent.mContext = 0;
    mUnidentified680[0] = 0;
    mUnidentified680[1] = 0;
    mUnidentified680[2] = 0;
    mUnidentified680[3] = 0;

    for (int i = 0; i < 54; ++i)
    {
        mMatchupStates[i][0] = -1;
        mMatchupStates[i][1] = -1;
    }

    for (int i = 0; i < 4; ++i)
    {
        mMatchupComponents[i].mSpeakerEnabled = false;
    }

    fn_80203B54();
    if (g_pCupManager->mState == 0x10
        || g_pCupManager->GetCurrentRoundType() == 0)
    {
        mUnidentified67D = false;
    }
}

CupHubScene::~CupHubScene()
{
}

inline void CupHubScene::UpdateRows()
{
    for (int i = 0; i < 4; ++i)
        fn_80201BE0(i);
}

void CupHubScene::SceneCreated()
{
    g_pCupManager->GetNumGames(0);
    FEPresentation* presentation = mFEScene->m_pFEPackage->GetPresentation();
    TLComponentInstance* title = FEFinder<TLComponentInstance, 4>::FindOrDefault(presentation->m_currentSlide, "Layer", "schedule screen", "TITLE2");
    fn_80208950(title, mUnidentified5BC, 64);

    for (int i = 0; i < 4; ++i)
    {
        mUnidentified20[i] = FEFinder<TLComponentInstance, 4>::FindOrDefault(presentation->m_currentSlide, "Layer", "schedule screen", lbl_8051B3E0[i]);
        TLComponentInstance* highlight = FEFinder<TLComponentInstance, 4>::FindOrDefault(mUnidentified20[i]->GetActiveSlide(), "highlite");
        TLImageInstance* image = FEFinder<TLImageInstance, 2>::FindOrDefault(highlight, "on", "RTSC_highlightbar");
        nlColour colour;
        nlColourSet(colour, 255, 192, 0, 90);
        image->SetAssetColour(colour);
        highlight->SetActiveSlide("off", true, false);
    }

    mUnidentified1C = FEFinder<TLComponentInstance, 4>::Find<>(presentation->m_currentSlide, "Layer", "schedule screen", "MOVING_HIGHLIGHT");
    mUnidentified1C->m_bVisible = false;
    TLComponentInstance* scrollbar = FEFinder<TLComponentInstance, 4>::Find<>(presentation->m_currentSlide, "Layer", "schedule screen", "scrollbar");

    SHNavigation* navigation = GetNavigationScene();
    TLComponentInstance* backButton = 0;
    if (navigation != 0)
    {
        backButton = navigation->GetButton(4);
        mRulesButton = navigation->GetButton(16);
        mUnidentified768 = navigation->GetPageControls();
    }
    mNavigationComponent.SetButtonInstance(backButton);
    if (mUnidentified67D)
    {
        mUnidentified768->SetButtonState(1, true, true);
        mUnidentified768->SetButtonState(0, true, true);
    }
    else
    {
        mUnidentified768->SetButtonState(1, true, false);
        mUnidentified768->SetButtonState(0, true, false);
    }
    fn_80207724(true);
    mScrollWidget.SetComponent(scrollbar);
    mScrollWidget.SetRange(mUnidentified890 - 4);
    mScrollWidget.SetValue(mUnidentified304);
    UpdateRows();
    for (int i = 0; i < 4; ++i)
    {
        GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
    }
}

void CupHubScene::Update(float fDeltaT)
{
    BaseSceneHandler::Update(fDeltaT);
    if (!mScrollWidget.mInitialized)
    {
        mScrollWidget.Initialize();
    }
    g_pCupManager->GetNumGames(0);
    if (mUnidentified894 == 0 || mUnidentified894 == 2 || mUnidentified894 == 3)
    {
        TLSlide* slide = mPresentation->m_currentSlide;
        if (slide->GetCurrentTime() < slide->GetStartTime() + slide->GetDuration())
        {
            for (int i = 0; i < 4; ++i)
            {
                GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
            }
            return;
        }
        if (mUnidentified894 == 0)
        {
            if (!mUnidentified300)
            {
                SHNavigation* navigation = GetNavigationScene();
                if (mUnidentified67D)
                    navigation->SetButtons(0x1F, false);
                else
                    navigation->SetButtons(0x14, true);
                fn_802088B4();
                fn_80202C34();
                mUnidentified300 = true;
            }
            mUnidentified894 = 1;
        }
        else if (mUnidentified894 == 2)
        {
            if (mUnidentified67E)
            {
                CycleCupPage(1, false);
                return;
            }
            if (mUnidentified67F)
            {
                CycleCupPage(1, true);
                return;
            }
            fn_80207060(false);
            return;
        }
        else if (mUnidentified894 == 3)
        {
            HandleCupBack(true);
            return;
        }
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
            if (mUnidentified680[i] > 0 || mNavigationComponent.mPointerInside[i]
                || mUnidentified768->mPointerInside[0] || mUnidentified768->mPointerInside[1])
                pointer->SetActiveSlide("A", true, false);
            else
                pointer->SetActiveSlide("cursor", true, false);
            if (mUnidentified680[i] < 1)
                mUnidentified1C->m_bVisible = false;
        }

        u8 valid = 1;
        FEPointerEvent event;
        event.mIndex = i;
        event.mPosition = GetPointerPosition(i, &valid);
        event.mPressed = g_pFEInput->JustPressed((eFEINPUT_PAD)i, 30, true, 0);
        event.mReleased = g_pFEInput->JustReleased((eFEINPUT_PAD)i, 30, true, 0);
        for (int j = 0; j < 4; ++j)
        {
            mMatchupComponents[j].HandlePointerEvent(&event);
        }
        if (mUnidentified67D)
            mUnidentified768->Update(event, fDeltaT);
        if (!mUnidentified67C)
            mRulesComponent.HandlePointerEvent(&event);

        bool back = !mUnidentified67C && mNavigationComponent.UpdateBackButton(event, fDeltaT);
        if (back)
        {
            for (int j = 0; j < 4; ++j)
                mMatchupComponents[j].Disable();
            mUnidentified894 = 3;
            SHNavigation* navigation = GetNavigationScene();
            if (navigation != 0)
                navigation->HideButtons();
            mPresentation->SetActiveSlide("out", true);
            return;
        }
        if (!mUnidentified67C)
            mScrollWidget.Update(event, fDeltaT);

        if (g_pFEInput->m_InputLockDepth == 0)
        {
            if (mUnidentified768->IsButtonPressed(1) || mUnidentified768->IsButtonPressed(0))
            {
                FEAudio::PlayAnimAudioEvent(0x304FDD1E, 0, 0, 1);
                FEAudio::PlayAnimAudioEvent(0x375C885A, 0, 0, 1);
                mUnidentified894 = 2;
                mPresentation->SetActiveSlide("out", true);
                if (mUnidentified768->IsButtonPressed(1))
                    mUnidentified67E = true;
                else if (mUnidentified768->IsButtonPressed(0))
                    mUnidentified67F = true;
                return;
            }
        }
    }
    if (mScrollWidget.IsScrolling(1, true))
    {
        ++mUnidentified304;
        UpdateRows();
    }
    else if (mScrollWidget.IsScrolling(0, true))
    {
        --mUnidentified304;
        UpdateRows();
    }
    mUnidentified67C = false;
}

void CupHubScene::fn_802017C4(int index, int value)
{
    mUnidentified20[index]->SetActiveSlide("ROUND", true, false);
    TLTextInstance* text = FEFinder<TLTextInstance, 3>::FindOrDefault(mUnidentified20[index]->GetActiveSlide(), "BIG_ROUND");
    char buffer[4];
    unsigned short wideBuffer[4];
    WideString format;
    WideString result;
    nlSNPrintf(buffer, sizeof(buffer), "%d", value + 1);
    nlStrToWcs(buffer, wideBuffer, 4);
    format = g_pLocalization->GetString("ROUND");
    result = Format(format, wideBuffer);
    memcpy(mTextBuffers[index], result.c_str(), sizeof(mTextBuffers[index]));
    text->SetString(mTextBuffers[index]);
}

void CupHubScene::fn_80201BE0(int index)
{
    if (mMatchupComponents[index].HasOtherPointerState(1, -1))
        mUnidentified20[index]->SetActiveSlide("over", true, false);
    else
        mUnidentified20[index]->SetActiveSlide("off", true, false);

    CupManager* cupManager = g_pCupManager;
    cupManager->GetNumGamesPerRound(0, 0);
    cupManager->GetCurrentRoundNumber();
    cupManager->GetNumGamesPerRound(0, 0);
    int round = mMatchupStates[mUnidentified304 + index][0];
    int matchup = mMatchupStates[mUnidentified304 + index][1];
    if (matchup == -1)
    {
        fn_802017C4(index, round);
        return;
    }

    BasicGameInfo* game = cupManager->GetMatchupInfo(0, round, (u16)matchup);
    int home = game->mTeamIndex[0];
    int away = game->mTeamIndex[1];
    int userTeam = cupManager->GetUserSelectedCupTeam();
    TLComponentInstance* highlight = FEFinder<TLComponentInstance, 4>::FindOrDefault(mUnidentified20[index]->GetActiveSlide(), "matchup_content", "highlite2");
    if (home == userTeam || away == userTeam)
        highlight->SetActiveSlide("on", true, false);
    else
        highlight->SetActiveSlide("off", true, false);

    TLImageInstance* leftCaptain = FEFinder<TLImageInstance, 2>::FindOrDefault(mUnidentified20[index]->GetActiveSlide(), "matchup_content", "left", "left_captain");
    TLImageInstance* rightCaptain = FEFinder<TLImageInstance, 2>::FindOrDefault(mUnidentified20[index]->GetActiveSlide(), "matchup_content", "right", "right_captain");
    char leftName[24];
    nlSNPrintf(leftName, sizeof(leftName), "captain_%s_s", GetCharacterInfo(GetCharacterIndexFromCaptain(home)).mName);
    FEPresentation* leftPresentation = mFEScene->m_pFEPackage->GetPresentation();
    TLImageInstance* leftImage = FEFinder<TLImageInstance, 2>::FindOrDefault(leftPresentation, "Slide2", "Layer", leftName);
    if (leftImage->m_pTextureResource != 0)
        leftCaptain->m_pTextureResource = leftImage->m_pTextureResource;
    char rightName[24];
    nlSNPrintf(rightName, sizeof(rightName), "captain_%s_s", GetCharacterInfo(GetCharacterIndexFromCaptain(away)).mName);
    FEPresentation* rightPresentation = mFEScene->m_pFEPackage->GetPresentation();
    TLImageInstance* rightImage = FEFinder<TLImageInstance, 2>::FindOrDefault(rightPresentation, "Slide2", "Layer", rightName);
    if (rightImage->m_pTextureResource != 0)
        rightCaptain->m_pTextureResource = rightImage->m_pTextureResource;

    TLTextInstance* leftScore = FEFinder<TLTextInstance, 3>::FindOrDefault(mUnidentified20[index]->GetActiveSlide(), "matchup_content", "left", "score_left");
    TLTextInstance* rightScore = FEFinder<TLTextInstance, 3>::FindOrDefault(mUnidentified20[index]->GetActiveSlide(), "matchup_content", "right", "score_right");
    if (game->mFinalScore[0] == 0 && game->mFinalScore[1] == 0)
    {
        leftScore->m_bVisible = false;
        rightScore->m_bVisible = false;
    }
    else
    {
        char leftScoreBuffer[4];
        nlSNPrintf(leftScoreBuffer, sizeof(leftScoreBuffer), "%d", game->mFinalScore[0]);
        nlStrToWcs(leftScoreBuffer, mUnidentified63C[index][0], 4);
        leftScore->SetString(mUnidentified63C[index][0]);
        char rightScoreBuffer[4];
        nlSNPrintf(rightScoreBuffer, sizeof(rightScoreBuffer), "%d", game->mFinalScore[1]);
        nlStrToWcs(rightScoreBuffer, mUnidentified63C[index][1], 4);
        rightScore->SetString(mUnidentified63C[index][1]);
        leftScore->m_bVisible = true;
        rightScore->m_bVisible = true;
    }

    TLTextInstance* roundText = FEFinder<TLTextInstance, 3>::FindOrDefault(mUnidentified20[index]->GetActiveSlide(), "matchup_content", "ROUND");
    TLTextInstance* gameText = FEFinder<TLTextInstance, 3>::FindOrDefault(mUnidentified20[index]->GetActiveSlide(), "matchup_content", "GAME");
    FEFinder<TLImageInstance, 2>::Find<>(mUnidentified20[index]->GetActiveSlide(), "matchup_content", "round_bar");
    fn_802025F4(roundText, round + 1, gameText, matchup + 1, index);
}

void CupHubScene::fn_802025F4(TLTextInstance* roundText, int round, TLTextInstance* gameText, int game, int index)
{
    char roundBuffer[4];
    char gameBuffer[4];
    unsigned short roundWide[4];
    unsigned short gameWide[4];
    WideString roundFormat;
    WideString roundResult;
    WideString gameFormat;
    WideString gameResult;
    nlSNPrintf(roundBuffer, sizeof(roundBuffer), "%d", round);
    nlStrToWcs(roundBuffer, roundWide, 4);
    nlSNPrintf(gameBuffer, sizeof(gameBuffer), "%d", game);
    nlStrToWcs(gameBuffer, gameWide, 4);
    roundFormat = g_pLocalization->GetString("ROUND");
    roundResult = Format(roundFormat, roundWide);
    gameFormat = g_pLocalization->GetString("GAME");
    gameResult = Format(gameFormat, gameWide);
    memcpy(mTextBuffers[index], roundResult.c_str(), sizeof(mTextBuffers[index]));
    roundText->SetString(mTextBuffers[index]);
    memcpy(mUnidentified53C[index], gameResult.c_str(), sizeof(mUnidentified53C[index]));
    gameText->SetString(mUnidentified53C[index]);
}

void CupHubScene::fn_80202C34()
{
    typedef Detail::MemFunImpl<void, void (CupHubScene::*)(unsigned int, void*)> PointerMethod;
    typedef BindExp3<void, PointerMethod, CupHubScene*, Placeholder<0>, Placeholder<1> > PointerBinding;

    FEPointerListener::Callback enter(PointerBinding(MemFun(&CupHubScene::fn_80203498), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback leave(PointerBinding(MemFun(&CupHubScene::fn_80203674), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback inside(PointerBinding(MemFun(&CupHubScene::fn_802037E8), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback press(PointerBinding(MemFun(&CupHubScene::fn_80203320), this, Placeholder<0>(), Placeholder<1>()));
    for (int i = 0; i < 4; ++i)
    {
        mMatchupComponents[i].mContext = (void*)i;
        mMatchupComponents[i].SetInstanceBounds(mUnidentified20[i], true, 0.0f, 0.0f, 0.85f, 0.6f);
        mMatchupComponents[i].SetPointerEnterCallback(enter);
        mMatchupComponents[i].SetPointerLeaveCallback(leave);
        mMatchupComponents[i].SetPointerInsideCallback(inside);
        mMatchupComponents[i].SetPointerPressCallback(press);
    }
    FEPointerListener::Callback rulesEnter(PointerBinding(MemFun(&CupHubScene::fn_80203980), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback rulesLeave(PointerBinding(MemFun(&CupHubScene::fn_80203A10), this, Placeholder<0>(), Placeholder<1>()));
    FEPointerListener::Callback rulesPress(PointerBinding(MemFun(&CupHubScene::fn_80203A88), this, Placeholder<0>(), Placeholder<1>()));
    SetPlayButtonBounds(&mRulesComponent, mRulesButton);
    mRulesComponent.SetPointerEnterCallback(rulesEnter);
    mRulesComponent.SetPointerLeaveCallback(rulesLeave);
    mRulesComponent.SetPointerPressCallback(rulesPress);
}

void CupHubScene::fn_80203320(unsigned int index, void* context)
{
    int row = (int)context;
    int round = mMatchupStates[mUnidentified304 + row][0];
    int matchup = mMatchupStates[mUnidentified304 + row][1];
    if (matchup != -1)
    {
        FEAudio::PlayAnimAudioEvent(0x970D6164, 0, 0, 1);
        mUnidentified67C = true;
        BasicGameInfo* game = g_pCupManager->GetMatchupInfo(0, round, (u16)matchup);
        if (game->mFinalScore[0] != 0 || game->mFinalScore[1] != 0)
        {
            GameResultsScene* results = (GameResultsScene*)GameSceneManager::Instance()->Push((SceneList)0x21, (ScreenMovement)0, false);
            results->fn_8020A494(game, this, 0);
            results->SetDisplayMode(0xD);
        }
        else
        {
            FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push((SceneList)0xA, (ScreenMovement)0, false);
            popup->Create((ePopupMenu)0x36, FEPopupMenu::Nothing);
        }
    }
}

void CupHubScene::fn_80203498(unsigned int index, void* context)
{
    int row = (int)context;
    if (mMatchupStates[mUnidentified304 + row][1] == -1)
    {
        mUnidentified1C->m_bVisible = false;
        return;
    }

    ++mUnidentified680[index];
    mMatchupComponents[row].PlayHoverFeedback(index);
    FEFinder<TLComponentInstance, 4>::FindOrDefault(mUnidentified20[row]->GetActiveSlide(), "highlite")->SetActiveSlide("on", true, false);
    mMatchupComponents[row].SetPointerState(1, index);
    mUnidentified20[row]->SetActiveSlide("over", true, false);
    FEAudio::PlayAnimAudioEvent(0xF6EB899E, 0, 0, 1);
    fn_80201BE0(row);

    char buffer[4];
    nlSNPrintf(buffer, sizeof(buffer), "%d", row + 1);
    mUnidentified1C->m_bVisible = true;
    mUnidentified1C->SetActiveSlide(buffer, true, false);
}

void CupHubScene::fn_80203674(unsigned int index, void* context)
{
    int row = (int)context;
    if (mMatchupStates[mUnidentified304 + row][1] != -1)
    {
        --mUnidentified680[index];
        FEFinder<TLComponentInstance, 4>::FindOrDefault(mUnidentified20[row]->GetActiveSlide(), "highlite")->SetActiveSlide("off", true, false);
        mMatchupComponents[row].SetPointerState(0, index);
        mUnidentified20[row]->SetActiveSlide("off", true, false);
        fn_80201BE0(row);
    }
}

void CupHubScene::fn_802037E8(unsigned int index, void* context)
{
    int matchup = mMatchupStates[mUnidentified304 + (int)context][1];
    int state = mMatchupComponents[(int)context].GetPointerState(index);
    if (state == 1 && matchup == -1)
    {
        --mUnidentified680[index];
        FEFinder<TLComponentInstance, 4>::FindOrDefault(mUnidentified20[(int)context]->GetActiveSlide(), "highlite")->SetActiveSlide("off", true, false);
        mMatchupComponents[(int)context].SetPointerState(0, index);
        mUnidentified20[(int)context]->SetActiveSlide("off", true, false);
        fn_80201BE0((int)context);
    }
    else if (state == 0 && matchup != -1)
    {
        fn_80203498(index, context);
    }
}

void CupHubScene::fn_80203980(unsigned int index, void* context)
{
    if (context == 0 && !mRulesComponent.HasOtherPointerState(1, index))
    {
        mRulesButton->SetActiveSlide("over", true, false);
        FEAudio::PlayAnimAudioEvent(0xAA73EF34, 0, 0, 1);
        mRulesComponent.SetPointerState(1, index);
    }
}

void CupHubScene::fn_80203A10(unsigned int index, void* context)
{
    if (context == 0 && !mRulesComponent.HasOtherPointerState(1, index))
    {
        mRulesButton->SetActiveSlide("off", true, false);
        mRulesComponent.SetPointerState(0, index);
    }
}

void CupHubScene::fn_80203A88(unsigned int, void* context)
{
    mUnidentified67C = true;
    for (int i = 0; i < 4; ++i)
    {
        GetPointerInstance(i)->SetActiveSlide("waiting", true, false);
    }

    switch ((int)context)
    {
    case 0:
        FEAudio::PlayAnimAudioEvent(0x6E5C794C, 0, 0, 1);
        FEAudio::PlayAnimAudioEvent(0x2ECB0035, 0, 0, 1);
        mUnidentified894 = 2;

        SHNavigation* object = GetNavigationScene();
        if (object != 0)
        {
            object->HideButtons();
        }

        mPresentation->SetActiveSlide("out", true);
        break;
    }
}

void CupHubScene::fn_80203B54()
{
    CupManager* cupManager = g_pCupManager;
    int entry = -1;
    int round = cupManager->GetCurrentRoundNumber();
    for (int i = 0; i < g_pCupManager->GetNumRegularRounds(); ++i)
    {
        ++entry;
        mMatchupStates[entry][0] = i;
        mMatchupStates[entry][1] = -1;
        if (cupManager->GetCurrentRoundType() == 0 && i != -5 && i == round)
        {
            mUnidentified304 = entry;
        }
        for (int j = 0; j < g_pCupManager->GetNumGamesPerRound(0, i); ++j)
        {
            ++entry;
            mMatchupStates[entry][0] = i;
            mMatchupStates[entry][1] = j;
        }
    }
    mUnidentified890 = entry + 1;
    if (mUnidentified304 + 4 > mUnidentified890)
    {
        mUnidentified304 += mUnidentified890 - mUnidentified304 - 4;
    }
}
