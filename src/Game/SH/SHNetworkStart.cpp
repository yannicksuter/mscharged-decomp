#include "Game/SH/SHNetworkStart.h"

#include "Game/FE/feFinder.h"
#include "Game/FE/fePresentation.inl"
#include "Game/FE/feInlineHasher.h"
#include "Game/FE/feFinder_impl.h"
#include "Game/NetworkSession.h"
#include "Game/NetworkStatsManager.h"
#include "Game/GameSceneManager.h"
#include "Game/GameInfo.h"
#include "Game/FE/feInput.h"
#include "Game/FE/fePopupMenu.h"
#include "Game/FE/fePackage.h"
#include "Game/FE/feScene.h"
#include "Game/FE/feMusic.h"
#include "Game/FE/tlTextInstance.h"
#include "Game/Sys/debug.h"
#include "NL/nlBind.h"
#include "NL/nlPrint.h"
#include "Game/FE/tlComponentInstance.h"
#include "NL/nlString.h"
#include "NL/nlFunction.inl"

bool gNetworkStartWaitingForDialog;
bool gNetworkStartResetRequested;

void ResetNetworkStart()
{
    gNetworkStartWaitingForDialog = false;
    gNetworkStartResetRequested = true;
}

void ResumeNetworkStart()
{
    gNetworkStartWaitingForDialog = false;
}

NetworkStartScene::NetworkStartScene()
    : mState(STATE_ONLINE_OPTIONS)
    , mShowPlayer2Controls(false)
{
    gNetworkStartWaitingForDialog = false;
    gNetworkStartResetRequested = false;
    for (int i = 0; i < 7; ++i)
    {
        mPlayerText[i] = 0;
        mPlayerNames[i][0] = 0;
    }
}

NetworkStartScene::~NetworkStartScene()
{
    LANLobby* lobby = g_pNetworkSessionBase->GetTransport();
    if (lobby != 0)
    {
        lobby->SetLobbyListener(0);
    }
}

void NetworkStartScene::SetActionButtons(int buttonState)
{
    TLSlide* activeSlide = mPresentation->m_currentSlide;
    TLComponentInstance* buttons = FEFinder<TLComponentInstance, 2>::Find<>(activeSlide, "Layer", "BUTTONS");

    bool visible = true;
    if (buttonState == ACTION_BUTTONS_A_AND_B)
    {
        buttons->SetActiveSlide("A AND B", true, false);
    }
    else if (buttonState == ACTION_BUTTONS_A)
    {
        buttons->SetActiveSlide("A", true, false);
    }
    else if (buttonState == ACTION_BUTTONS_B)
    {
        buttons->SetActiveSlide("B", true, false);
    }
    else if (buttonState == ACTION_BUTTONS_HIDDEN)
    {
        visible = false;
    }
    buttons->m_bVisible = visible;
}

void NetworkStartScene::EnterState(int state)
{
    mMenuItems.SetItem(0);
    int buttons = ACTION_BUTTONS_A_AND_B;
    switch (state)
    {
    case STATE_ONLINE_OPTIONS:
        mPresentation->SetActiveSlide("ONLINE OPTIONS", true);
        buttons = ACTION_BUTTONS_A_AND_B;
        break;
    case STATE_CREATE_GAME:
        mPresentation->SetActiveSlide("CREATE", true);
        buttons = ACTION_BUTTONS_B;
        break;
    case STATE_JOIN_GAME:
        mPresentation->SetActiveSlide("JOIN", true);
        buttons = ACTION_BUTTONS_B;
        break;
    case STATE_WAIT_FOR_START:
        mPresentation->SetActiveSlide("waiting for start", true);
        buttons = ACTION_BUTTONS_HIDDEN;
        break;
    }
    SetActionButtons(buttons);
    mPresentation->Update(0.0f);
    mState = state;
    TLSlide* slide = mPresentation->GetActiveSlide();
    for (int i = 0; i < 7; ++i)
    {
        char name[100];
        nlSNPrintf(name, sizeof(name), "Player%dTxt", i + 1);
        mPlayerText[i] = FEFinder<TLTextInstance, 3>::Find<>(slide, "Layer", name);
        if (mPlayerText[i] != 0)
            mPlayerText[i]->SetString(mPlayerNames[i]);
    }
}

void NetworkStartScene::OnMenuItemApply(TLComponentInstance* component, int state)
{
    struct MatchData
    {
        int rank;
        unsigned short wins;
        unsigned short losses;
    };
    switch (state)
    {
    case STATE_ONLINE_OPTIONS:
        EnterState(STATE_ONLINE_OPTIONS);
        break;
    case STATE_CREATE_GAME:
    {
        EnterState(STATE_CREATE_GAME);
        LANLobby* lobby = g_pNetworkSessionBase->GetTransport();
        MatchData data;
        NetworkStatsManager* stats = NetworkStatsManager::Instance();
        data.rank = stats->mHasLocalStats[0] ? stats->mLocalStats[0].mDisplayRank : 0;
        NetworkRankingMeta* record = NetworkStatsManager::Instance()->GetLocalStats(0);
        if (record != 0)
        {
            data.wins = record->mWins;
            data.losses = record->mLosses;
        }
        else
        {
            data.wins = 0;
            data.losses = 0;
        }
        lobby->SetUserMatchData(sizeof(data), &data);
        int result = lobby->CreateGame(g_pNetworkSession->mCupMode);
        switch (result)
        {
        case 0:
            break;
        case 1:
            if (GameSceneManager::Instance()->GetSceneType(GameSceneManager::Instance()->GetCurrentScene()) != SCENE_POPUP_MENU)
            {
                FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push(SCENE_POPUP_MENU, SCREEN_NOTHING, false);
                popup->Create((ePopupMenu)0x5c, Function<FnVoidVoid>(ResetNetworkStart));
                gNetworkStartWaitingForDialog = true;
            }
            break;
        default:
            if (GameSceneManager::Instance()->GetSceneType(GameSceneManager::Instance()->GetCurrentScene()) != SCENE_POPUP_MENU)
            {
                FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push(SCENE_POPUP_MENU, SCREEN_NOTHING, false);
                popup->Create((ePopupMenu)0x58, Function<FnVoidVoid>(ResetNetworkStart));
                gNetworkStartWaitingForDialog = true;
            }
            break;
        }
        break;
    }
    case STATE_JOIN_GAME:
    {
        EnterState(STATE_JOIN_GAME);
        LANLobby* lobby = g_pNetworkSessionBase->GetTransport();
        MatchData data;
        NetworkStatsManager* stats = NetworkStatsManager::Instance();
        data.rank = stats->mHasLocalStats[0] ? stats->mLocalStats[0].mDisplayRank : 0;
        NetworkRankingMeta* record = NetworkStatsManager::Instance()->GetLocalStats(0);
        if (record != 0)
        {
            data.wins = record->mWins;
            data.losses = record->mLosses;
        }
        else
        {
            data.wins = 0;
            data.losses = 0;
        }
        lobby->SetUserMatchData(sizeof(data), &data);
        int result = lobby->JoinGame(0, g_pNetworkSession->mCupMode);
        switch (result)
        {
        case 0:
            break;
        case 1:
            if (GameSceneManager::Instance()->GetSceneType(GameSceneManager::Instance()->GetCurrentScene()) != SCENE_POPUP_MENU)
            {
                FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push(SCENE_POPUP_MENU, SCREEN_NOTHING, false);
                popup->Create((ePopupMenu)0x5c, Function<FnVoidVoid>(ResetNetworkStart));
                gNetworkStartWaitingForDialog = true;
            }
            break;
        case 4:
            if (GameSceneManager::Instance()->GetSceneType(GameSceneManager::Instance()->GetCurrentScene()) != SCENE_POPUP_MENU)
            {
                FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push(SCENE_POPUP_MENU, SCREEN_NOTHING, false);
                popup->Create((ePopupMenu)0x5d, Function<FnVoidVoid>(ResetNetworkStart));
                gNetworkStartWaitingForDialog = true;
            }
            break;
        case 5:
            if (GameSceneManager::Instance()->GetSceneType(GameSceneManager::Instance()->GetCurrentScene()) != SCENE_POPUP_MENU)
            {
                FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push(SCENE_POPUP_MENU, SCREEN_NOTHING, false);
                popup->Create((ePopupMenu)0x5e, Function<FnVoidVoid>(ResetNetworkStart));
                gNetworkStartWaitingForDialog = true;
            }
            break;
        default:
            if (GameSceneManager::Instance()->GetSceneType(GameSceneManager::Instance()->GetCurrentScene()) != SCENE_POPUP_MENU)
            {
                FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push(SCENE_POPUP_MENU, SCREEN_NOTHING, false);
                popup->Create((ePopupMenu)0x59, Function<FnVoidVoid>(ResetNetworkStart));
                gNetworkStartWaitingForDialog = true;
            }
            break;
        }
        break;
    }
    }
}

static void UpdatePlayer2Controls(NetworkStartScene* scene);

void NetworkStartScene::SceneCreated()
{
    FEPresentation* presentation = mFEScene->m_pFEPackage->GetPresentation();
    const char* menuNames[] = { "CREATE GAME", "JOIN GAME" };
    int menuStates[] = { STATE_CREATE_GAME, STATE_JOIN_GAME };
    presentation->SetActiveSlide("online options", true);
    for (int i = 0; i < 2; ++i)
    {
        TLComponentInstance* component = FEFinder<TLComponentInstance, 2>::Find<>(presentation->m_currentSlide, "Layer", menuNames[i]);
        MenuItem<TLComponentInstance>* item = mMenuItems.AddItem(component);
        {
            MenuItem<TLComponentInstance>::Callback callback(Bind<void>(MemFun(&NetworkStartScene::SelectMenuItem), this, placeholder0));
            item->SetCallback(ON_HIGHLIGHT, callback);
        }
        {
            MenuItem<TLComponentInstance>::Callback callback(Bind<void>(MemFun(&NetworkStartScene::DeselectMenuItem), this, placeholder0));
            item->SetCallback(ON_UNHIGHLIGHT, callback);
        }
        {
            MenuItem<TLComponentInstance>::Callback callback(Bind<void>(MemFun(&NetworkStartScene::OnMenuItemApply), this, placeholder0, menuStates[i]));
            item->SetCallback(ON_APPLY, callback);
        }
        item->SetLockedFlag(false);
        DeselectMenuItem(component);
    }
    presentation->SetActiveSlide("online options", true);
    UpdatePlayer2Controls(this);
    mMenuItems.SetFlag(1);
    EnterState(mState);
    g_pNetworkSessionBase->SetSessionState(2);
    g_pNetworkSessionBase->GetTransport()->SetLobbyListener(this);
    FEMusic::StartStreamIfDifferent(1);
}

static void UpdatePlayer2Controls(NetworkStartScene* scene)
{
    const char* slides[] = { "ONLINE OPTIONS", "CREATE", "JOIN", "waiting for start" };
    for (unsigned int i = 0; i < 4; ++i)
    {
        bool visible = false;
        if (i == 0)
            visible = !scene->mShowPlayer2Controls;
        FEFinder<TLTextInstance, 3>::Find(scene->mPresentation, slides[i], "Layer", "P2ReadyText")->m_bVisible = scene->mShowPlayer2Controls;
        FEFinder<TLComponentInstance, 2>::Find(scene->mPresentation, slides[i], "Layer", "P2StartComponent")->m_bVisible = visible;
    }
}

void NetworkStartScene::Update(float dt)
{
    BaseSceneHandler::Update(dt);
    if (gNetworkStartWaitingForDialog && !g_pFEInput->HasInputLock(this))
        return;
    if (gNetworkStartResetRequested)
    {
        EnterState(STATE_ONLINE_OPTIONS);
        gNetworkStartResetRequested = false;
    }
    switch (mState)
    {
    case STATE_ONLINE_OPTIONS:
        UpdateMenuInput();
        break;
    case STATE_CREATE_GAME:
        UpdateLobby();
        break;
    case STATE_JOIN_GAME:
        if (g_pFEInput->JustPressed(FE_ALL_PADS, 31, true, 0))
            EnterState(STATE_ONLINE_OPTIONS);
        break;
    case STATE_WAIT_FOR_START:
        break;
    }
}

void NetworkStartScene::UpdateMenuInput()
{
    eFEINPUT_PAD pad = FE_ALL_PADS;
    if (g_pFEInput->IsAutoPressed(FE_PAD2_ID, 32, true, 0))
    {
        mShowPlayer2Controls = true;
        UpdatePlayer2Controls(this);
    }
    TLSlide* slide = mPresentation->GetActiveSlide();
    bool slideFinished = slide->GetCurrentTime() >= slide->GetStartTime() + slide->GetDuration();
    if (slideFinished)
    {
        if (g_pFEInput->IsAutoPressed(FE_ALL_PADS, 14, true, 0))
        {
            mMenuItems.PreviousItem();
        }
        else if (g_pFEInput->IsAutoPressed(FE_ALL_PADS, 13, true, 0))
        {
            mMenuItems.NextItem();
        }
        else if (g_pFEInput->JustPressed(FE_ALL_PADS, 30, true, &pad))
        {
            if (mMenuItems.RunCallbackOnCurrent(ON_APPLY) == RES_OK)
                GameInfoManager::Instance()->mMainUserPadNumber = pad;
        }
        else if (g_pFEInput->JustPressed(FE_ALL_PADS, 31, true, 0))
        {
            GameSceneManager::Instance()->Push(SCENE_ONLINE_MENU, SCREEN_BACK, true);
            g_pNetworkSessionBase->SetSessionState(0);
        }
    }
}

void NetworkStartScene::UpdateLobby()
{
    int playerCount = 0;
    for (int i = 0; i < 7; ++i)
        mPlayerNames[i][0] = 0;

    LANLobby* lobby = g_pNetworkSessionBase->GetTransport();
    if (lobby != 0)
    {
        playerCount = lobby->GetPlayerCount();
        for (int i = 1; i < playerCount; ++i)
            nlStrToWcs(lobby->GetPlayerInfo(i)->mName, mPlayerNames[i - 1], 11);
    }
    for (int i = 0; i < 7; ++i)
    {
        if (mPlayerText[i] != 0)
            mPlayerText[i]->SetString(mPlayerNames[i]);
    }
    if (playerCount >= 2)
        SetActionButtons(ACTION_BUTTONS_A_AND_B);
    else
        SetActionButtons(ACTION_BUTTONS_B);

    if (g_pFEInput->JustPressed(FE_ALL_PADS, 31, true, 0))
    {
        int result = lobby->AbortCreateGame();
        tDebugPrintManager::Print(DC_NETWORK, "Abort create game returned status %d\n", result);
        EnterState(STATE_ONLINE_OPTIONS);
    }
    else if (g_pFEInput->JustPressed(FE_ALL_PADS, 30, true, 0) && playerCount >= 2)
    {
        lobby->StartGame();
    }
}

void NetworkStartScene::SelectMenuItem(TLComponentInstance* component)
{
    component->SetActiveSlide("on", true, false);
    component->Update(0.0f);
}

void NetworkStartScene::DeselectMenuItem(TLComponentInstance* component)
{
    component->SetActiveSlide("off", true, false);
    component->Update(0.0f);
}

void NetworkStartScene::OnGameCreated(int result)
{
    tDebugPrintManager::Print(DC_NETWORK, "SHNetworkStart screen received game created callback status %d!\n", result);
    switch (result)
    {
    case 0:
        break;
    case 8:
        break;
    case 7:
        if (GameSceneManager::Instance()->GetSceneType(GameSceneManager::Instance()->GetCurrentScene()) != SCENE_POPUP_MENU)
        {
            FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push(SCENE_POPUP_MENU, SCREEN_NOTHING, false);
            popup->Create((ePopupMenu)0x60, Function<FnVoidVoid>(ResetNetworkStart));
            gNetworkStartWaitingForDialog = true;
        }
        break;
    default:
        if (GameSceneManager::Instance()->GetSceneType(GameSceneManager::Instance()->GetCurrentScene()) != SCENE_POPUP_MENU)
        {
            FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push(SCENE_POPUP_MENU, SCREEN_NOTHING, false);
            popup->Create((ePopupMenu)0x58, Function<FnVoidVoid>(ResetNetworkStart));
            gNetworkStartWaitingForDialog = true;
        }
        break;
    }
}

void NetworkStartScene::OnGameJoined(int result)
{
    tDebugPrintManager::Print(DC_NETWORK, "SHNetworkStart screen received game joined callback status %d!\n", result);
    switch (result)
    {
    case 0:
        EnterState(STATE_WAIT_FOR_START);
        break;
    case 4:
        if (GameSceneManager::Instance()->GetSceneType(GameSceneManager::Instance()->GetCurrentScene()) != SCENE_POPUP_MENU)
        {
            FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push(SCENE_POPUP_MENU, SCREEN_NOTHING, false);
            popup->Create((ePopupMenu)0x5d, Function<FnVoidVoid>(ResetNetworkStart));
            gNetworkStartWaitingForDialog = true;
        }
        break;
    case 6:
        if (GameSceneManager::Instance()->GetSceneType(GameSceneManager::Instance()->GetCurrentScene()) != SCENE_POPUP_MENU)
        {
            FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push(SCENE_POPUP_MENU, SCREEN_NOTHING, false);
            popup->Create((ePopupMenu)0x5f, Function<FnVoidVoid>(ResetNetworkStart));
            gNetworkStartWaitingForDialog = true;
        }
        break;
    case 7:
        if (GameSceneManager::Instance()->GetSceneType(GameSceneManager::Instance()->GetCurrentScene()) != SCENE_POPUP_MENU)
        {
            FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push(SCENE_POPUP_MENU, SCREEN_NOTHING, false);
            popup->Create((ePopupMenu)0x60, Function<FnVoidVoid>(ResetNetworkStart));
            gNetworkStartWaitingForDialog = true;
        }
        break;
    default:
        if (GameSceneManager::Instance()->GetSceneType(GameSceneManager::Instance()->GetCurrentScene()) != SCENE_POPUP_MENU)
        {
            FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push(SCENE_POPUP_MENU, SCREEN_NOTHING, false);
            popup->Create((ePopupMenu)0x59, Function<FnVoidVoid>(ResetNetworkStart));
            gNetworkStartWaitingForDialog = true;
        }
        break;
    }
}

void NetworkStartScene::OnGameLaunched(int result)
{
    tDebugPrintManager::Print(DC_NETWORK, "SHNetworkStart screen received game launched callback status %d!\n", result);
    switch (result)
    {
    case 0:
        break;
    case 9:
        if (GameSceneManager::Instance()->GetSceneType(GameSceneManager::Instance()->GetCurrentScene()) != SCENE_POPUP_MENU)
        {
            FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push(SCENE_POPUP_MENU, SCREEN_NOTHING, false);
            popup->Create((ePopupMenu)0x61, Function<FnVoidVoid>(ResumeNetworkStart));
            gNetworkStartWaitingForDialog = true;
        }
        break;
    default:
        if (GameSceneManager::Instance()->GetSceneType(GameSceneManager::Instance()->GetCurrentScene()) != SCENE_POPUP_MENU)
        {
            FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push(SCENE_POPUP_MENU, SCREEN_NOTHING, false);
            popup->Create((ePopupMenu)0x62, Function<FnVoidVoid>(ResumeNetworkStart));
            gNetworkStartWaitingForDialog = true;
        }
        break;
    }
}
