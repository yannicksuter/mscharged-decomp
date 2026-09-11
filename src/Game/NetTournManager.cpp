#include "Game/NetTournManager.h"
#include "Game/NetworkMessageRegistry.h"
#include "NL/nlFunctionMemory.h"
#include "Game/Sys/debug.h"

#include "Game/Drawable/DrawableObj.h"
#include "Game/GameInfo.h"
#include "Game/NetworkDraft.h"
#include "Game/NetworkSession.h"
#include "Game/Render/Presentation.h"
#include "Game/TweakValue.h"
#include "NL/nlMath.h"
#include "NL/nlMemory.h"

#include <string.h>


static NetTournManager* sNetTournManager;
static bool sCupPersonaOverrideActive;
static int sCupPersonaOverride;

static float s_fDefaultTimeToStartGames = 50.0f;
static int s_nSendGameInProgressUpdateEvery = 1;
static int s_nSendGameInProgressMajorUpdate = 5;
int s_nOverrideCupPersona = 10;

static TweakFloatBinding sDefaultTimeToStartGamesTweak(
    "s_fDefaultTimeToStartGames", "Network/Tournament",
    &s_fDefaultTimeToStartGames);
static TweakIntBinding sSendGameInProgressUpdateEveryTweak(
    "s_nSendGameInProgressUpdateEvery", "Network/Tournament",
    &s_nSendGameInProgressUpdateEvery, true);
static TweakIntBinding sSendGameInProgressMajorUpdateTweak(
    "s_nSendGameInProgressMajorUpdate", "Network/Tournament",
    &s_nSendGameInProgressMajorUpdate, true);
static TweakIntBinding sOverrideCupPersonaTweak(
    "s_nOverrideCupPersona", "Network/Tournament", &s_nOverrideCupPersona,
    true);

void NetMessagePauseRequest::Serialize(
    NetworkMessageSerializer* serializer)
{
    serializer->Transfer(&mMachineIndex, sizeof(mMachineIndex));
    serializer->Transfer(&mPaused, sizeof(mPaused));
}

void NetMessagePauseResponse::Serialize(
    NetworkMessageSerializer* serializer)
{
    serializer->Transfer(&mMachineMask, sizeof(mMachineMask));
}

int NetMessagePauseResponse::GetType()
{
    return 29;
}

int NetMessagePauseRequest::GetType()
{
    return 28;
}

NetMessagePauseRequest::~NetMessagePauseRequest()
{
}

void NetworkMessageType30::Serialize(
    NetworkMessageSerializer* serializer)
{
    serializer->Transfer(&mUnidentified08, sizeof(mUnidentified08));
}

void NetworkMessageType31::Serialize(
    NetworkMessageSerializer* serializer)
{
    serializer->Transfer(&mUnidentified08, sizeof(mUnidentified08));
}

NetworkMessageType30::~NetworkMessageType30()
{
}

NetworkMessageType31::~NetworkMessageType31()
{
}

int NetworkMessageType31::GetType()
{
    return 31;
}

int NetworkMessageType30::GetType()
{
    return 30;
}

void NetworkMessageType34::Serialize(
    NetworkMessageSerializer* serializer)
{
    serializer->Transfer(&mUnidentified08, sizeof(mUnidentified08));
    serializer->Transfer(&mUnidentified0A, sizeof(mUnidentified0A));
    serializer->Transfer(&mUnidentified0C, sizeof(mUnidentified0C));
    serializer->Transfer(&mUnidentified0D, sizeof(mUnidentified0D));
    serializer->Transfer(&mUnidentified0E, sizeof(mUnidentified0E));
}

void NetworkMessageType35::Serialize(
    NetworkMessageSerializer* serializer)
{
    serializer->Transfer(&mCount, sizeof(mCount));

    if (serializer->mDirection == 0)
    {
        u8 values = 0;
        memcpy(&values, serializer->mPosition, sizeof(values));
        serializer->mPosition += sizeof(values);
        for (int i = 0; i < mCount; ++i)
        {
            if ((values & (1 << i)) != 0)
            {
                mValues[i] = true;
            }
            else
            {
                mValues[i] = false;
            }
        }
    }
    else
    {
        u8 values = 0;
        for (int i = 0; i < mCount; ++i)
        {
            if (mValues[i])
            {
                values |= 1 << i;
            }
        }
        memcpy(serializer->mPosition, &values, sizeof(values));
        serializer->mPosition += sizeof(values);
    }
}

int NetworkMessageType35::GetType()
{
    return 35;
}

int NetworkMessageType34::GetType()
{
    return 34;
}

void NetMessageTournamentStart::Serialize(
    NetworkMessageSerializer* serializer)
{
    serializer->Transfer(&mMachineIndex, sizeof(mMachineIndex));
    serializer->Transfer(&mMachineCount, sizeof(mMachineCount));
    serializer->Transfer(&mCupPersona, sizeof(mCupPersona));
    serializer->Transfer(&mFirstStadium, sizeof(mFirstStadium));
    serializer->Transfer(&mSecondStadium, sizeof(mSecondStadium));
    serializer->Transfer(mSeedings, sizeof(mSeedings));
}

void NetMessageTournamentGameUpdate::Serialize(
    NetworkMessageSerializer* serializer)
{
    serializer->Transfer(&mUpdateType, sizeof(mUpdateType));
    serializer->Transfer(&mGameIndex, sizeof(mGameIndex));
    serializer->Transfer(&mIsHomeMachine, sizeof(mIsHomeMachine));
    serializer->Transfer(&mGameStatus, sizeof(mGameStatus));
    serializer->Transfer(&mGameTimeDelta, sizeof(mGameTimeDelta));
    serializer->Transfer(&mHasGameInfo, sizeof(mHasGameInfo));
    if (mHasGameInfo != 0)
    {
        serializer->Transfer(&mGameInfo, sizeof(mGameInfo));
    }
}

void NetMessageTournamentLoadingState::Serialize(
    NetworkMessageSerializer* serializer)
{
    serializer->Transfer(&mMachineIndex, sizeof(mMachineIndex));
    serializer->Transfer(
        &mFinishedLoadingToKnockout, sizeof(mFinishedLoadingToKnockout));
}

NetMessageTournamentGameUpdate::~NetMessageTournamentGameUpdate()
{
}

NetMessageTournamentLoadingState::~NetMessageTournamentLoadingState()
{
}

int NetMessageTournamentLoadingState::GetType()
{
    return 33;
}

int NetMessageTournamentGameUpdate::GetType()
{
    return 32;
}

int NetMessageTournamentStart::GetType()
{
    return 20;
}

void NetTournManager::CreateInstance()
{
    sNetTournManager = new ((u8*)nlMalloc(sizeof(NetTournManager), 8, false)) NetTournManager();
}

NetTournManager* NetTournManager::Instance()
{
    return sNetTournManager;
}

void NetTournManager::Reset(bool)
{
    mState = 0;
    mMachineCount = 0;
    mLocalMachineIndex = -1;
    mLargeBracket = false;
    mCupPersona = 10;
    mFirstStadium = -1;
    mSecondStadium = -1;
    mSeedings[0] = 0;
    mSeedings[1] = 1;
    mSeedings[2] = 2;
    mSeedings[3] = 3;
    mSeedings[4] = 4;
    mSeedings[5] = 5;
    mSeedings[6] = 6;
    mSeedings[7] = 7;
    mCurrentRound = 0;
    mTimeToStartGames = s_fDefaultTimeToStartGames;
    mWaitingToStartGames = false;

    for (int i = 0; i < 7; ++i)
    {
        NetworkTournamentGame& game = mGames[i];
        game.mState = NET_TOURN_GAME_EMPTY;
        game.mHomeMachine = -1;
        game.mAwayMachine = -1;
        game.mBracketIndex = i;
        game.mHomeUpdate = 0;
        game.mAwayUpdate = 0;
        game.mGameInfo.Reset(true);
    }

    mWinningMachine = -1;
    mLoadedToKnockout[0] = false;
    mLoadedToGame[0] = false;
    mLoadedToKnockout[1] = false;
    mLoadedToGame[1] = false;
    mLoadedToKnockout[2] = false;
    mLoadedToGame[2] = false;
    mLoadedToKnockout[3] = false;
    mLoadedToGame[3] = false;
    mLoadedToKnockout[4] = false;
    mLoadedToGame[4] = false;
    mLoadedToKnockout[5] = false;
    mLoadedToGame[5] = false;
    mLoadedToKnockout[6] = false;
    mLoadedToGame[6] = false;
    mLoadedToKnockout[7] = false;
    mLoadedToGame[7] = false;
    mLocalMachineEliminated = false;
    mTournamentMachineMappingActive = false;
    mCurrentGameIndex = -1;
    mTournamentToMachine[0] = -1;
    mTournamentToMachine[1] = -1;
    mFirstGameInRound = 0;
    mLastGameInRound = -1;
    mLastGameProgressUpdate = -1;
    mGameProgressUpdateCount = 0;
}

void NetTournManager::TransitionOnlineMenuToTournament(
    NetMessageTournamentStart* message)
{
    NetworkMessageReceiver* receiver = this;
    gNetworkMessageRegistry->RegisterReceiver(32, receiver);
    gNetworkMessageRegistry->RegisterReceiver(33, receiver);

    mMachineCount = (s8)message->mMachineCount;
    mLocalMachineIndex = (s8)message->mMachineIndex;
    mLargeBracket = mMachineCount > 4;
    mCupPersona = message->mCupPersona;
    mFirstStadium = (s8)message->mFirstStadium;
    mSecondStadium = (s8)message->mSecondStadium;
    for (int i = 0; i < 8; ++i)
    {
        mSeedings[i] = message->mSeedings[i];
    }

    mState = 1;
    mCurrentRound = 0;
    mTimeToStartGames = s_fDefaultTimeToStartGames;
    mWaitingToStartGames = true;
    mLocalMachineEliminated = false;

    for (int i = 0; i < 7; ++i)
    {
        NetworkTournamentGame& game = mGames[i];
        game.mState = NET_TOURN_GAME_EMPTY;
        game.mHomeMachine = -1;
        game.mAwayMachine = -1;
        game.mBracketIndex = i;
        game.mHomeUpdate = 0;
        game.mAwayUpdate = 0;
        game.mGameInfo.Reset(true);
    }

    mWinningMachine = -1;
    BuildInitialBracket();
    Presentation::Instance().Call("TransitionOnlineMenuToTournament");
}

void NetTournManager::GenerateFirstRoundSeedings(
    int machineCount, u8* seedings)
{
    static const u8 largeBracketOrder[8] = { 0, 4, 2, 6, 1, 5, 3, 7 };
    static const u8 smallBracketOrder[8] = { 0, 2, 1, 3, 4, 5, 6, 7 };
    const u8* order = machineCount > 4 ? largeBracketOrder : smallBracketOrder;
    bool used[8] = { false, false, false, false, false, false, false, false };

    int i = 0;
    for (; i < machineCount; ++i)
    {
        int candidate = (int)nlRandomf((float)machineCount);
        for (int tries = 0; tries < machineCount; ++tries)
        {
            if (candidate >= machineCount)
            {
                candidate = 0;
            }
            if (!used[candidate])
            {
                used[candidate] = true;
                break;
            }
            ++candidate;
        }
        seedings[order[i]] = candidate;
    }

    for (; i < 8; ++i)
    {
        seedings[order[i]] = i;
    }

    tDebugPrintManager::Print(DC_NETWORK,
        "Generated 1st Rnd Seedings: %d %d %d %d %d %d %d %d\n",
        seedings[0], seedings[1], seedings[2], seedings[3], seedings[4],
        seedings[5], seedings[6], seedings[7]);
}

void NetTournManager::BuildInitialBracket()
{
    UpdateRoundGameRange();
    for (int gameIndex = mFirstGameInRound; gameIndex <= mLastGameInRound;
         ++gameIndex)
    {
        NetworkTournamentGame& game = mGames[gameIndex];
        game.mState = NET_TOURN_GAME_READY;
        game.mHomeMachine = mSeedings[gameIndex * 2];
        game.mAwayMachine = mSeedings[gameIndex * 2 + 1];
        game.mBracketIndex = gameIndex;
        game.mHomeUpdate = 0;
        game.mAwayUpdate = 0;
        game.mGameInfo.Reset(true);
        if (mCurrentRound == 1)
        {
            game.mGameInfo.mStadiumIndex = mSecondStadium;
        }
        else
        {
            game.mGameInfo.mStadiumIndex = mFirstStadium;
        }
    }
}

void NetTournManager::AdvanceBracket()
{
    int previousFirst = mFirstGameInRound;
    UpdateRoundGameRange();
    for (int gameIndex = mFirstGameInRound; gameIndex <= mLastGameInRound;
         ++gameIndex)
    {
        NetworkTournamentGame& game = mGames[gameIndex];
        game.mState = NET_TOURN_GAME_READY;
        game.mHomeMachine = -1;
        game.mAwayMachine = -1;
        game.mBracketIndex = gameIndex;
        game.mHomeUpdate = 0;
        game.mAwayUpdate = 0;
        game.mGameInfo.Reset(true);

        int winnerSide = -1;
        int winnerMachine = -1;
        if (mGames[previousFirst++].GetWinnerAndLoser(
                &winnerSide, &winnerMachine))
        {
            game.mHomeMachine = winnerMachine;
        }
        if (mGames[previousFirst++].GetWinnerAndLoser(
                &winnerSide, &winnerMachine))
        {
            game.mAwayMachine = winnerMachine;
        }
    }
}

void NetTournManager::OnTournamentGameStart(NetMessageGameStart* message)
{
    if (message->mUnidentified1B == 0)
    {
        tDebugPrintManager::Print(DC_NETWORK,
            "NetTournManager discarded NetworkStartGame Msg because not a tournament game\n");
        return;
    }

    mTournamentMachineMappingActive = true;
    mCurrentGameIndex = (s8)message->mUnidentified1C[0];
    mTournamentToMachine[0] = (s8)message->mUnidentified1C[1];
    mTournamentToMachine[1] = (s8)message->mUnidentified1C[2];

    u8 gameBuffer[0xFF];
    u8 buffer[0xFF];
    NetMessageTournamentLoadingState loading(mLocalMachineIndex, false);
    tDebugPrintManager::Print(DC_NETWORK, "NotifyLoadingToGame called on machine %d\n",
        mLocalMachineIndex);
    int size = gNetworkMessageRegistry->Serialize(&loading, buffer, sizeof(buffer));
    SendToAllTournamentMachines(buffer, size);

    bool isHomeMachine = false;
    if (mLocalMachineIndex == mTournamentToMachine[0])
    {
        isHomeMachine = true;
    }
    if (isHomeMachine)
    {
        NetMessageTournamentGameUpdate gameUpdate(
            1, mCurrentGameIndex, isHomeMachine, 1, 0, false);
        int gameSize = gNetworkMessageRegistry->Serialize(&gameUpdate, gameBuffer, sizeof(gameBuffer));
        SendToAllTournamentMachines(gameBuffer, gameSize);
    }
}

int NetTournManager::MachineIdxToTournamentIdx(int machine) const
{
    return mTournamentToMachine[machine];
}

int NetTournManager::TournamentIdxToMachineIdx(int machine) const
{
    for (int i = 0; i < 2; ++i)
    {
        if (machine == mTournamentToMachine[i])
        {
            return i;
        }
    }
    return -1;
}

bool NetTournManager::SendTournamentGameStart(NetworkTournamentGame* game)
{
    if (game == 0 || game->mHomeMachine == -1 || game->mAwayMachine == -1)
    {
        return false;
    }

    game->mState = NET_TOURN_GAME_IN_PROGRESS;
    return true;
}

void NetTournManager::SendToAllTournamentMachines(void* data, int size)
{
    NetworkMachineRoster* roster = g_pNetworkSessionBase->GetMachineRoster();
    if (roster == 0)
    {
        tDebugPrintManager::Print(DC_NETWORK,
            "No lobby found, cannot send message of size %d to all machines in tournament\n",
            size);
        return;
    }

    for (int machine = 0; machine < mMachineCount; ++machine)
    {
        u32 aid = roster->GetMachineAid(machine);
        if (aid == 0xFFFFFFFF)
        {
            g_pNetworkSessionBase->GetDirectSocket()->Receive(data, size);
        }
        else if (aid == 0)
        {
            tDebugPrintManager::Print(DC_NETWORK,
                "Warning: Cannot send message to tournament midx %d of size %d - no connection\n",
                machine, size);
        }
        else
        {
            g_pNetworkSessionBase->GetDirectSocket()->Send(aid, data, size, true);
        }
    }
}

void NetTournManager::UpdateRoundGameRange()
{
    mFirstGameInRound = 0;
    mLastGameInRound = -1;
    if (mLargeBracket)
    {
        switch (mCurrentRound)
        {
        case 0:
            mFirstGameInRound = 0;
            mLastGameInRound = 3;
            break;
        case 1:
            mFirstGameInRound = 4;
            mLastGameInRound = 5;
            break;
        case 2:
            mFirstGameInRound = 6;
            mLastGameInRound = 6;
            break;
        }
    }
    else
    {
        switch (mCurrentRound)
        {
        case 0:
            mFirstGameInRound = 0;
            mLastGameInRound = 1;
            break;
        case 1:
            mFirstGameInRound = 2;
            mLastGameInRound = 2;
            break;
        }
    }
}

void NetTournManager::StartReadyGames()
{
    for (int i = 0; i < 8; ++i)
    {
        mLoadedToGame[i] = false;
        mLoadedToKnockout[i] = false;
    }

    for (int gameIndex = mFirstGameInRound; gameIndex <= mLastGameInRound;
         ++gameIndex)
    {
        NetworkTournamentGame& game = mGames[gameIndex];
        if (game.mHomeMachine == -1 && game.mAwayMachine == -1)
        {
            game.mState = NET_TOURN_GAME_NO_PLAYERS;
        }
        else if (game.mHomeMachine == -1)
        {
            game.mState = NET_TOURN_GAME_AWAY_ADVANCES;
        }
        else if (game.mAwayMachine == -1)
        {
            game.mState = NET_TOURN_GAME_HOME_ADVANCES;
        }
        else if (game.mHomeMachine == mLocalMachineIndex)
        {
            SendTournamentGameStart(&game);
        }
    }
}

bool NetworkTournamentGame::IsFinished() const
{
    switch (mState)
    {
    case NET_TOURN_GAME_IN_PROGRESS:
    case NET_TOURN_GAME_STATE_3:
    case NET_TOURN_GAME_STATE_4:
    case NET_TOURN_GAME_OVER:
        return true;
    case NET_TOURN_GAME_NO_CONTEST:
    case NET_TOURN_GAME_NO_PLAYERS:
    case NET_TOURN_GAME_HOME_ADVANCES:
    case NET_TOURN_GAME_AWAY_ADVANCES:
    case NET_TOURN_GAME_STATE_10:
    case NET_TOURN_GAME_STATE_11:
    default:
        return false;
    }
}

bool NetworkTournamentGame::GetWinnerAndLoser(
    int* winnerSide, int* winningMachine) const
{
    switch (mState)
    {
    case NET_TOURN_GAME_OVER:
    {
        int winner = -1;
        if (mGameInfo.mFinalScore[0] != 0 || mGameInfo.mFinalScore[1] != 0)
        {
            winner = mGameInfo.mFinalScore[0] > mGameInfo.mFinalScore[1] ? 0 : 1;
        }
        if (winner >= 0 && winner <= 1)
        {
            if (winnerSide != 0)
            {
                *winnerSide = winner;
            }
            if (winningMachine != 0)
            {
                *winningMachine = winner == 0 ? mHomeMachine : mAwayMachine;
            }
        }
        else
        {
            if (winnerSide != 0)
            {
                *winnerSide = -1;
            }
            if (winningMachine != 0)
            {
                *winningMachine = -1;
            }
        }
        return true;
    }
    case NET_TOURN_GAME_NO_CONTEST:
    case NET_TOURN_GAME_NO_PLAYERS:
    case NET_TOURN_GAME_STATE_10:
    case NET_TOURN_GAME_STATE_11:
        if (winnerSide != 0)
        {
            *winnerSide = -1;
        }
        if (winningMachine != 0)
        {
            *winningMachine = -1;
        }
        return true;
    case NET_TOURN_GAME_HOME_ADVANCES:
        if (winnerSide != 0)
        {
            *winnerSide = 0;
        }
        if (winningMachine != 0)
        {
            *winningMachine = mHomeMachine;
        }
        return true;
    case NET_TOURN_GAME_AWAY_ADVANCES:
        if (winnerSide != 0)
        {
            *winnerSide = 1;
        }
        if (winningMachine != 0)
        {
            *winningMachine = mAwayMachine;
        }
        return true;
    default:
        return false;
    }
}

void NetTournManager::MarkDisconnectedMachine(int machine)
{
    for (int gameIndex = mFirstGameInRound; gameIndex <= mLastGameInRound;
         ++gameIndex)
    {
        NetworkTournamentGame& game = mGames[gameIndex];
        if (game.mHomeMachine == machine || game.mAwayMachine == machine)
        {
            switch (game.mState)
            {
            case NET_TOURN_GAME_EMPTY:
            case NET_TOURN_GAME_READY:
                game.mState = NET_TOURN_GAME_STATE_11;
                break;
            case NET_TOURN_GAME_IN_PROGRESS:
                game.mState = NET_TOURN_GAME_STATE_10;
                break;
            case NET_TOURN_GAME_STATE_3:
            case NET_TOURN_GAME_STATE_4:
                game.mState = NET_TOURN_GAME_OVER;
                break;
            }
        }
    }
}

bool NetTournManager::AreRoundGamesFinished()
{
    for (int gameIndex = mFirstGameInRound; gameIndex <= mLastGameInRound;
         ++gameIndex)
    {
        int winner = -1;
        NetworkTournamentGame& game = mGames[gameIndex];
        if (!game.GetWinnerAndLoser(&winner, 0))
        {
            return false;
        }
        if (winner != -1)
        {
            int homeMachine = game.mHomeMachine;
            int awayMachine = game.mAwayMachine;
            if (homeMachine != -1 && awayMachine != -1)
            {
                if (!NetworkDraft::Instance()->FindDraftTeamByPeerIndex(homeMachine)->mPlayers[0].mDisconnected
                    && mLoadedToGame[homeMachine] && !mLoadedToKnockout[homeMachine])
                {
                    return false;
                }
                if (!NetworkDraft::Instance()->FindDraftTeamByPeerIndex(awayMachine)->mPlayers[0].mDisconnected
                    && mLoadedToGame[awayMachine] && !mLoadedToKnockout[awayMachine])
                {
                    return false;
                }
            }
        }
    }
    return true;
}

void NetTournManager::Update(float dt)
{
    if (mState != 1)
    {
        return;
    }

    if (mWaitingToStartGames)
    {
        if (mTimeToStartGames > 0.0f)
        {
            mTimeToStartGames -= dt;
        }
        if (mTimeToStartGames <= 0.0f)
        {
            StartReadyGames();
            mWaitingToStartGames = false;
        }
    }
    else if (AreRoundGamesFinished())
    {
        ++mCurrentRound;
        if (mCurrentRound >= GetNumPlayoffRounds())
        {
            int winnerSide = -1;
            mGames[mFirstGameInRound].GetWinnerAndLoser(
                &winnerSide, &mWinningMachine);
            gNetworkMessageRegistry->UnregisterReceiver(32);
            gNetworkMessageRegistry->UnregisterReceiver(33);
            mState = 2;
        }
        else
        {
            mTimeToStartGames = s_fDefaultTimeToStartGames;
            mWaitingToStartGames = true;
            AdvanceBracket();
        }
    }
}

void NetTournManager::NotifyGameStarted()
{
    // TODO: register NotifyGameOver and broadcast the initial progress update.
    mLastGameProgressUpdate = 0;
    mGameProgressUpdateCount = 0;
}

void NetTournManager::NotifyFinishedLoadingToKnockout()
{
    u8 buffer[0xFF];
    NetMessageTournamentLoadingState message(mLocalMachineIndex, true);
    tDebugPrintManager::Print(DC_NETWORK,
        "NotifyFinishedLoadingToKnockout called on machine %d\n",
        mLocalMachineIndex);
    int size = gNetworkMessageRegistry->Serialize(&message, buffer, sizeof(buffer));
    SendToAllTournamentMachines(buffer, size);
}

void NetTournManager::NotifyOverlayPopped(int)
{
    bool isHomeMachine = false;
    if (mLocalMachineIndex == mTournamentToMachine[0])
    {
        isHomeMachine = true;
    }
    NetMessageTournamentGameUpdate message(2, mCurrentGameIndex,
        isHomeMachine, 0, 0, false);
    u8 buffer[0xFF];
    int size = gNetworkMessageRegistry->Serialize(&message, buffer, sizeof(buffer));
    SendToAllTournamentMachines(buffer, size);
}

void NetTournManager::NotifyGameOver()
{
    bool isHomeMachine = false;
    if (mLocalMachineIndex == mTournamentToMachine[0])
    {
        isHomeMachine = true;
    }
    NetMessageTournamentGameUpdate message(0, mCurrentGameIndex,
        isHomeMachine, 1, 0, true);
    message.mGameInfo = *GameInfoManager::Instance()->GetCurrentGameInfo();
    u8 buffer[0xFF];
    int size = gNetworkMessageRegistry->Serialize(&message, buffer, sizeof(buffer));
    SendToAllTournamentMachines(buffer, size);
}

void NetTournManager::ResetGameProgressUpdateTimer(int)
{
    mLastGameProgressUpdate = -1;
    mGameProgressUpdateCount = 0;
}

int NetTournManager::ProcessMessage(NetworkMessage* message)
{
    NetworkMachineRoster* roster = g_pNetworkSessionBase->GetMachineRoster();
    s8 machine = roster->MachineIdxFromConnection(message->mSource);
    if (machine < 0 || machine >= roster->GetMachineCount())
    {
        tDebugPrintManager::Print(DC_NETWORK,
            "Discarded message type %d because from unknown connection %x\n",
            (u8)message->GetType(), message->mSource);
        return 1;
    }

    switch ((u8)message->GetType())
    {
    case 32:
        HandleTournamentGameUpdate(
            static_cast<NetMessageTournamentGameUpdate*>(message));
        break;
    case 33:
    {
        NetMessageTournamentLoadingState* loading
            = static_cast<NetMessageTournamentLoadingState*>(message);
        const char* destination = "Game";
        if (loading->mFinishedLoadingToKnockout)
        {
            destination = "Knockout";
        }
        tDebugPrintManager::Print(DC_NETWORK, "Received Tournament Loaded to %s from %d\n",
            destination, loading->mMachineIndex);
        if (loading->mMachineIndex >= 0 && loading->mMachineIndex < mMachineCount)
        {
            if (loading->mFinishedLoadingToKnockout)
            {
                mLoadedToKnockout[loading->mMachineIndex] = true;
            }
            else
            {
                mLoadedToGame[loading->mMachineIndex] = true;
            }
        }
        else
        {
            tDebugPrintManager::Print(DC_NETWORK, "Loaded from machine %d out of range [0,%d)\n",
                loading->mMachineIndex, mMachineCount);
        }
        break;
    }
    }
    return 1;
}

void NetTournManager::HandleTournamentGameUpdate(
    NetMessageTournamentGameUpdate* message)
{
    if (message->mGameIndex >= 7)
    {
        return;
    }

    NetworkTournamentGame& game = mGames[message->mGameIndex];
    switch (message->mUpdateType)
    {
    case 0:
        game.mState = message->mGameStatus;
        break;
    case 1:
        game.mState = NET_TOURN_GAME_IN_PROGRESS;
        break;
    case 2:
        game.mState = NET_TOURN_GAME_OVER;
        break;
    case 3:
        game.mState = NET_TOURN_GAME_NO_CONTEST;
        break;
    default:
        tDebugPrintManager::Print(DC_NETWORK,
            "Ignoring unknown NetworkTournamentGameUpdate type %d\n",
            message->mUpdateType);
        return;
    }

    if (message->mIsHomeMachine)
    {
        game.mHomeUpdate = message->mGameTimeDelta;
    }
    else
    {
        game.mAwayUpdate = message->mGameTimeDelta;
    }
    if (message->mHasGameInfo)
    {
        memcpy(
            &game.mGameInfo, &message->mGameInfo, sizeof(game.mGameInfo));
    }
}

BasicGameInfo* NetTournManager::GetGameInfo(int, int matchup)
{
    return &mGames[matchup].mGameInfo;
}

NetworkTournamentGame* NetTournManager::GetTournamentGame(int, int matchup)
{
    return &mGames[matchup];
}

bool NetTournManager::HasGameBeenPlayed(int, int matchup)
{
    return mGames[matchup].IsFinished();
}

BasicGameInfo* NetTournManager::GetCurrentGameInfo()
{
    return 0;
}

u16 NetTournManager::GetNumGamesPerRound(int, int round) const
{
    u16 numRounds = GetNumPlayoffRounds();
    u16 numGames = 0;
    if (round == numRounds - 1)
    {
        numGames = 1;
    }
    else if (round == numRounds - 2)
    {
        numGames = 2;
    }
    else if (round == numRounds - 3)
    {
        numGames = 4;
    }
    return numGames;
}

u16 NetTournManager::GetNumGames(int) const
{
    return mLargeBracket ? 7 : 3;
}

void NetTournManager::AttachTournamentTrophy(void* presentation)
{
    mTrophyPresentation = presentation;
    mTrophyResource = (void*)GetTournamentTrophyResource();
}

void NetTournManager::DetachTournamentTrophy()
{
    mTrophyPresentation = 0;
}

void NetTournManager::DestroyTournamentTrophy()
{
    mTrophyPresentation = 0;
    mTrophyResource = 0;
}

const char* NetTournManager::GetTournamentTrophyResource() const
{
    int cupPersona = GetCupPersona();
    if (sCupPersonaOverrideActive)
    {
        cupPersona = sCupPersonaOverride;
    }
    switch (cupPersona)
    {
    case 0:
        return "art/characters/npcs/trophymushroom/trophymushroom";
    case 1:
        return "art/characters/npcs/trophyflower/trophyflower";
    case 2:
        return "art/characters/npcs/trophystar/trophystar";
    case 3:
        return "art/characters/npcs/trophysunshine/trophysunshine";
    case 4:
        return "art/characters/npcs/trophybanana/trophybanana";
    case 5:
        return "art/characters/npcs/trophynextlevelcup/trophynextlevelcup";
    case 6:
        return "art/characters/npcs/trophykonga/trophykonga";
    case 7:
        return "art/characters/npcs/trophysand/trophysand";
    case 8:
        return "art/characters/npcs/trophylava/trophylava";
    case 9:
        return "art/characters/npcs/trophynintendo/trophynintendo";
    default:
        return "art/characters/npcs/trophymushroom/trophymushroom";
    }
}

u16 NetTournManager::GetNumPlayoffRounds() const
{
    return mLargeBracket ? 3 : 2;
}

bool NetTournManager::IsCupWinningGame(int) const
{
    return GetCurrentRoundNumber() == GetNumPlayoffRounds() - 1;
}

s16 NetTournManager::GetCurrentRoundNumber() const
{
    return mCurrentRound;
}

int NetTournManager::GetCurrentRoundType() const
{
    return 1;
}

int NetTournManager::GetCurrentMode() const
{
    return 3;
}

typedef void (NetTournManager::*NetTournManagerCallback)();

struct NetTournManagerCallbackBinding
{
    NetTournManagerCallback mCallback;
    NetTournManager* mTarget;
};

class NetTournManagerDelegate
{
public:
    void* operator new(unsigned long size) { return AllocateFunctionMemory(size); }
    void operator delete(void* p)
    {
        FreeFunctionMemory(p, sizeof(NetTournManagerDelegate));
    }

    NetTournManagerDelegate(const NetTournManagerCallbackBinding& binding)
        : mCallback(binding.mCallback)
        , mTarget(binding.mTarget)
    {
    }

    virtual ~NetTournManagerDelegate();
    virtual void Execute();
    virtual NetTournManagerDelegate* Clone();

    /* 0x04 */ NetTournManagerCallback mCallback;
    /* 0x10 */ NetTournManager* mTarget;
}; // size: 0x14

void NetTournManagerDelegate::Execute()
{
    (mTarget->*mCallback)();
}

NetTournManagerDelegate* NetTournManagerDelegate::Clone()
{
    return new NetTournManagerDelegate(*this);
}

NetTournManagerDelegate::~NetTournManagerDelegate()
{
}

nlMatrix4& DrawableObject::GetWorldMatrix()
{
    return mWorldMatrix;
}

void DrawableObject::SetWorldMatrix(const nlMatrix4* matrix)
{
    mWorldMatrix = *matrix;
}

void DrawableObject::V1()
{
}

DrawableObject::~DrawableObject()
{
}
