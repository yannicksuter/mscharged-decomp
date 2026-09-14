#include "Game/SH/SHNetworkStart.h"

#include "Game/FE/feFinder.h"
#include "Game/FE/fePresentation.inl"
#include "Game/FE/feFinder.inl"
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
    : mState(0)
    , mUnidentified238(false)
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

void NetworkStartScene::SetActionButtons(int state)
{
    TLSlide* activeSlide = mPresentation->m_currentSlide;
    TLComponentInstance* buttons = FEFinder<TLComponentInstance, 2>::Find<>(activeSlide, "Layer", "BUTTONS");

    bool visible = true;
    if (state == 0)
    {
        buttons->SetActiveSlide("A AND B", true, false);
    }
    else if (state == 1)
    {
        buttons->SetActiveSlide("A", true, false);
    }
    else if (state == 2)
    {
        buttons->SetActiveSlide("B", true, false);
    }
    else if (state == -1)
    {
        visible = false;
    }
    buttons->m_bVisible = visible;
}

void NetworkStartScene::fn_801FC7E4(int state)
{
    mMenuItems.SetItem(0);
    int buttons = 0;
    switch (state)
    {
    case 0:
        mPresentation->SetActiveSlide("ONLINE OPTIONS", true);
        buttons = 0;
        break;
    case 1:
        mPresentation->SetActiveSlide("CREATE", true);
        buttons = 2;
        break;
    case 2:
        mPresentation->SetActiveSlide("JOIN", true);
        buttons = 2;
        break;
    case 3:
        mPresentation->SetActiveSlide("waiting for start", true);
        buttons = -1;
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

void NetworkStartScene::fn_801FCA60(TLComponentInstance* component, int state)
{
    struct MatchData
    {
        int rank;
        unsigned short wins;
        unsigned short losses;
    };
    switch (state)
    {
    case 0:
        fn_801FC7E4(0);
        break;
    case 1:
    {
        fn_801FC7E4(1);
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
            if (GameSceneManager::Instance()->GetSceneType(GameSceneManager::Instance()->GetCurrentScene()) != (SceneList)10)
            {
                FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push((SceneList)10, SCREEN_NOTHING, false);
                popup->Create((ePopupMenu)0x5c, Function<FnVoidVoid>(ResetNetworkStart));
                gNetworkStartWaitingForDialog = true;
            }
            break;
        default:
            if (GameSceneManager::Instance()->GetSceneType(GameSceneManager::Instance()->GetCurrentScene()) != (SceneList)10)
            {
                FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push((SceneList)10, SCREEN_NOTHING, false);
                popup->Create((ePopupMenu)0x58, Function<FnVoidVoid>(ResetNetworkStart));
                gNetworkStartWaitingForDialog = true;
            }
            break;
        }
        break;
    }
    case 2:
    {
        fn_801FC7E4(2);
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
            if (GameSceneManager::Instance()->GetSceneType(GameSceneManager::Instance()->GetCurrentScene()) != (SceneList)10)
            {
                FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push((SceneList)10, SCREEN_NOTHING, false);
                popup->Create((ePopupMenu)0x5c, Function<FnVoidVoid>(ResetNetworkStart));
                gNetworkStartWaitingForDialog = true;
            }
            break;
        case 4:
            if (GameSceneManager::Instance()->GetSceneType(GameSceneManager::Instance()->GetCurrentScene()) != (SceneList)10)
            {
                FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push((SceneList)10, SCREEN_NOTHING, false);
                popup->Create((ePopupMenu)0x5d, Function<FnVoidVoid>(ResetNetworkStart));
                gNetworkStartWaitingForDialog = true;
            }
            break;
        case 5:
            if (GameSceneManager::Instance()->GetSceneType(GameSceneManager::Instance()->GetCurrentScene()) != (SceneList)10)
            {
                FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push((SceneList)10, SCREEN_NOTHING, false);
                popup->Create((ePopupMenu)0x5e, Function<FnVoidVoid>(ResetNetworkStart));
                gNetworkStartWaitingForDialog = true;
            }
            break;
        default:
            if (GameSceneManager::Instance()->GetSceneType(GameSceneManager::Instance()->GetCurrentScene()) != (SceneList)10)
            {
                FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push((SceneList)10, SCREEN_NOTHING, false);
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
    int menuStates[] = { 1, 2 };
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
            MenuItem<TLComponentInstance>::Callback callback(Bind<void>(MemFun(&NetworkStartScene::fn_801FCA60), this, placeholder0, menuStates[i]));
            item->SetCallback(ON_APPLY, callback);
        }
        item->SetLockedFlag(false);
        DeselectMenuItem(component);
    }
    presentation->SetActiveSlide("online options", true);
    UpdatePlayer2Controls(this);
    mMenuItems.SetFlag(1);
    fn_801FC7E4(mState);
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
            visible = !scene->mUnidentified238;
        FEFinder<TLTextInstance, 3>::Find(scene->mPresentation, slides[i], "Layer", "P2ReadyText")->m_bVisible = scene->mUnidentified238;
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
        fn_801FC7E4(0);
        gNetworkStartResetRequested = false;
    }
    switch (mState)
    {
    case 0:
        fn_801FD834();
        break;
    case 1:
        fn_801FDD08();
        break;
    case 2:
        if (g_pFEInput->JustPressed(FE_ALL_PADS, 31, true, 0))
            fn_801FC7E4(0);
        break;
    case 3:
        break;
    }
}

void NetworkStartScene::fn_801FD834()
{
    eFEINPUT_PAD pad = FE_ALL_PADS;
    if (g_pFEInput->IsAutoPressed(FE_PAD2_ID, 32, true, 0))
    {
        mUnidentified238 = true;
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
            GameSceneManager::Instance()->Push((SceneList)40, SCREEN_BACK, true);
            g_pNetworkSessionBase->SetSessionState(0);
        }
    }
}

void NetworkStartScene::fn_801FDD08()
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
        SetActionButtons(0);
    else
        SetActionButtons(2);

    if (g_pFEInput->JustPressed(FE_ALL_PADS, 31, true, 0))
    {
        int result = lobby->AbortCreateGame();
        tDebugPrintManager::Print(DC_NETWORK, "Abort create game returned status %d\n", result);
        fn_801FC7E4(0);
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
        if (GameSceneManager::Instance()->GetSceneType(GameSceneManager::Instance()->GetCurrentScene()) != (SceneList)10)
        {
            FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push((SceneList)10, SCREEN_NOTHING, false);
            popup->Create((ePopupMenu)0x60, Function<FnVoidVoid>(ResetNetworkStart));
            gNetworkStartWaitingForDialog = true;
        }
        break;
    default:
        if (GameSceneManager::Instance()->GetSceneType(GameSceneManager::Instance()->GetCurrentScene()) != (SceneList)10)
        {
            FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push((SceneList)10, SCREEN_NOTHING, false);
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
        fn_801FC7E4(3);
        break;
    case 4:
        if (GameSceneManager::Instance()->GetSceneType(GameSceneManager::Instance()->GetCurrentScene()) != (SceneList)10)
        {
            FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push((SceneList)10, SCREEN_NOTHING, false);
            popup->Create((ePopupMenu)0x5d, Function<FnVoidVoid>(ResetNetworkStart));
            gNetworkStartWaitingForDialog = true;
        }
        break;
    case 6:
        if (GameSceneManager::Instance()->GetSceneType(GameSceneManager::Instance()->GetCurrentScene()) != (SceneList)10)
        {
            FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push((SceneList)10, SCREEN_NOTHING, false);
            popup->Create((ePopupMenu)0x5f, Function<FnVoidVoid>(ResetNetworkStart));
            gNetworkStartWaitingForDialog = true;
        }
        break;
    case 7:
        if (GameSceneManager::Instance()->GetSceneType(GameSceneManager::Instance()->GetCurrentScene()) != (SceneList)10)
        {
            FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push((SceneList)10, SCREEN_NOTHING, false);
            popup->Create((ePopupMenu)0x60, Function<FnVoidVoid>(ResetNetworkStart));
            gNetworkStartWaitingForDialog = true;
        }
        break;
    default:
        if (GameSceneManager::Instance()->GetSceneType(GameSceneManager::Instance()->GetCurrentScene()) != (SceneList)10)
        {
            FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push((SceneList)10, SCREEN_NOTHING, false);
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
        if (GameSceneManager::Instance()->GetSceneType(GameSceneManager::Instance()->GetCurrentScene()) != (SceneList)10)
        {
            FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push((SceneList)10, SCREEN_NOTHING, false);
            popup->Create((ePopupMenu)0x61, Function<FnVoidVoid>(ResumeNetworkStart));
            gNetworkStartWaitingForDialog = true;
        }
        break;
    default:
        if (GameSceneManager::Instance()->GetSceneType(GameSceneManager::Instance()->GetCurrentScene()) != (SceneList)10)
        {
            FEPopupMenu* popup = (FEPopupMenu*)GameSceneManager::Instance()->Push((SceneList)10, SCREEN_NOTHING, false);
            popup->Create((ePopupMenu)0x62, Function<FnVoidVoid>(ResumeNetworkStart));
            gNetworkStartWaitingForDialog = true;
        }
        break;
    }
}
