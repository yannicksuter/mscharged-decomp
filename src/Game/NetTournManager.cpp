#include "Game/NetTournManager.h"

#include "Game/Drawable/DrawableObj.h"
#include "Game/GameInfo.h"
#include "Game/NetworkSession.h"
#include "Game/Render/Presentation.h"
#include "Game/TweakValue.h"
#include "NL/nlMath.h"
#include "NL/nlMemory.h"

#include <string.h>

extern "C" int fn_8004F594(int channel, const char* format, ...);
extern "C" u32 fn_8032C830(void* codec, void* message, void* buffer, int size);
extern "C" void fn_8032CA1C(
    void* codec, int type, UnidentifiedNetworkMessageReceiver* receiver);
extern "C" void fn_8032CA2C(void* codec, int type);
extern "C" void* fn_802B1C4C(unsigned long size);
extern "C" void fn_802B1D4C(void* p, unsigned long size);

extern void* lbl_806E2100;

static NetTournManager* sNetTournManager;
static bool sCupPersonaOverrideActive;
static int sCupPersonaOverride;

static float s_fDefaultTimeToStartGames = 50.0f;
static int s_nSendGameInProgressUpdateEvery = 1;
static int s_nSendGameInProgressMajorUpdate = 5;
static int s_nOverrideCupPersona = 10;

static TweakValueImpl_804F4DC8 sDefaultTimeToStartGamesTweak(
    "s_fDefaultTimeToStartGames", "Network/Tournament",
    &s_fDefaultTimeToStartGames);
static TweakValueIntImpl_804FD898 sSendGameInProgressUpdateEveryTweak(
    "s_nSendGameInProgressUpdateEvery", "Network/Tournament",
    &s_nSendGameInProgressUpdateEvery);
static TweakValueIntImpl_804FD898 sSendGameInProgressMajorUpdateTweak(
    "s_nSendGameInProgressMajorUpdate", "Network/Tournament",
    &s_nSendGameInProgressMajorUpdate);
static TweakValueIntImpl_804FD898 sOverrideCupPersonaTweak(
    "s_nOverrideCupPersona", "Network/Tournament", &s_nOverrideCupPersona);

void NetMessagePauseRequest_8050AD7C::Serialize(
    UnidentifiedMessageSerializer* serializer)
{
    serializer->Transfer(&mMachineIndex, sizeof(mMachineIndex));
    serializer->Transfer(&mPaused, sizeof(mPaused));
}

void NetMessagePauseResponse_8050AD68::Serialize(
    UnidentifiedMessageSerializer* serializer)
{
    serializer->Transfer(&mMachineMask, sizeof(mMachineMask));
}

int NetMessagePauseResponse_8050AD68::GetType()
{
    return 29;
}

int NetMessagePauseRequest_8050AD7C::GetType()
{
    return 28;
}

NetMessagePauseRequest_8050AD7C::~NetMessagePauseRequest_8050AD7C()
{
}

void NetworkMessageType30_8050ADA4::Serialize(
    UnidentifiedMessageSerializer* serializer)
{
    serializer->Transfer(&mUnidentified08, sizeof(mUnidentified08));
}

void NetworkMessageType31_8050AD90::Serialize(
    UnidentifiedMessageSerializer* serializer)
{
    serializer->Transfer(&mUnidentified08, sizeof(mUnidentified08));
}

NetworkMessageType30_8050ADA4::~NetworkMessageType30_8050ADA4()
{
}

NetworkMessageType31_8050AD90::~NetworkMessageType31_8050AD90()
{
}

int NetworkMessageType31_8050AD90::GetType()
{
    return 31;
}

int NetworkMessageType30_8050ADA4::GetType()
{
    return 30;
}

void NetworkMessageType34_8050ADCC::Serialize(
    UnidentifiedMessageSerializer* serializer)
{
    serializer->Transfer(&mUnidentified08, sizeof(mUnidentified08));
    serializer->Transfer(&mUnidentified0A, sizeof(mUnidentified0A));
    serializer->Transfer(&mUnidentified0C, sizeof(mUnidentified0C));
    serializer->Transfer(&mUnidentified0D, sizeof(mUnidentified0D));
    serializer->Transfer(&mUnidentified0E, sizeof(mUnidentified0E));
}

void UnidentifiedNetworkMessage_80126D84::Serialize(
    UnidentifiedMessageSerializer* serializer)
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

int UnidentifiedNetworkMessage_80126D84::GetType()
{
    return 35;
}

int NetworkMessageType34_8050ADCC::GetType()
{
    return 34;
}

NetworkMessageType34_8050ADCC::~NetworkMessageType34_8050ADCC()
{
}

void NetMessageTournamentStart::Serialize(
    UnidentifiedMessageSerializer* serializer)
{
    serializer->Transfer(&mMachineIndex, sizeof(mMachineIndex));
    serializer->Transfer(&mMachineCount, sizeof(mMachineCount));
    serializer->Transfer(&mStadium, sizeof(mStadium));
    serializer->Transfer(&mUnidentified0B, sizeof(mUnidentified0B));
    serializer->Transfer(&mUnidentified0C, sizeof(mUnidentified0C));
    serializer->Transfer(mSeedings, sizeof(mSeedings));
}

void NetMessageTournamentGameUpdate::Serialize(
    UnidentifiedMessageSerializer* serializer)
{
    serializer->Transfer(&mUpdateType, sizeof(mUpdateType));
    serializer->Transfer(&mGameIndex, sizeof(mGameIndex));
    serializer->Transfer(&mIsHomeMachine, sizeof(mIsHomeMachine));
    serializer->Transfer(&mGameStatus, sizeof(mGameStatus));
    serializer->Transfer(&mGameTimeDelta, sizeof(mGameTimeDelta));
    serializer->Transfer(&mHasGameInfo, sizeof(mHasGameInfo));
    if (mHasGameInfo != 0)
    {
        serializer->Transfer(mGameInfo, sizeof(mGameInfo));
    }
}

void NetMessageTournamentLoadingState::Serialize(
    UnidentifiedMessageSerializer* serializer)
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
    void* storage = nlMalloc(sizeof(NetTournManager), 8, false);
    NetTournManager* manager = 0;
    if (storage != 0)
    {
        manager = new (storage) NetTournManager;
    }
    sNetTournManager = manager;
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
    mStadium = 10;
    mHomeTeam = -1;
    mAwayTeam = -1;
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
    UnidentifiedNetworkMessageReceiver* receiver = this;
    fn_8032CA1C(lbl_806E2100, 32, receiver);
    fn_8032CA1C(lbl_806E2100, 33, receiver);

    mMachineCount = (s8)message->mMachineCount;
    mLocalMachineIndex = (s8)message->mMachineIndex;
    mLargeBracket = mMachineCount > 4;
    mStadium = message->mStadium;
    mHomeTeam = (s8)message->mUnidentified0B;
    mAwayTeam = (s8)message->mUnidentified0C;
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

    fn_8004F594(16,
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
        game.mGameInfo.mStadiumIndex = mStadium;
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
        fn_8004F594(16,
            "NetTournManager discarded NetworkStartGame Msg because not a tournament game\n");
        return;
    }

    mTournamentMachineMappingActive = true;
    mCurrentGameIndex = (s8)message->mUnidentified1C[0];
    mTournamentToMachine[0] = (s8)message->mUnidentified1C[1];
    mTournamentToMachine[1] = (s8)message->mUnidentified1C[2];

    NetMessageTournamentLoadingState loading;
    loading.mMachineIndex = mLocalMachineIndex;
    loading.mFinishedLoadingToKnockout = false;
    u8 buffer[0xFF];
    int size = fn_8032C830(lbl_806E2100, &loading, buffer, sizeof(buffer));
    SendToAllTournamentMachines(buffer, size);
}

int NetTournManager::MachineIdxToTournamentIdx(int machine) const
{
    return mTournamentToMachine[machine];
}

int NetTournManager::TournamentIdxToMachineIdx(int machine) const
{
    if (machine == mTournamentToMachine[0])
    {
        return 0;
    }
    if (machine == mTournamentToMachine[1])
    {
        return 1;
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
    if (lbl_806E20D8 == 0)
    {
        fn_8004F594(16,
            "No lobby found, cannot send message size %d to all machines in tournament\n",
            size);
        return;
    }

    for (int machine = 0; machine < mMachineCount; ++machine)
    {
        lbl_806E20D8->Send((s8)machine, data, size, true);
    }
}

void NetTournManager::UpdateRoundGameRange()
{
    mFirstGameInRound = 0;
    mLastGameInRound = -1;
    if (mLargeBracket)
    {
        if (mCurrentRound == 0)
        {
            mFirstGameInRound = 0;
            mLastGameInRound = 3;
        }
        else if (mCurrentRound == 1)
        {
            mFirstGameInRound = 4;
            mLastGameInRound = 5;
        }
        else if (mCurrentRound == 2)
        {
            mFirstGameInRound = 6;
            mLastGameInRound = 6;
        }
    }
    else if (mCurrentRound == 0)
    {
        mFirstGameInRound = 0;
        mLastGameInRound = 1;
    }
    else if (mCurrentRound == 1)
    {
        mFirstGameInRound = 2;
        mLastGameInRound = 2;
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
        if (!mGames[gameIndex].GetWinnerAndLoser(&winner, 0))
        {
            return false;
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
        if (mCurrentRound >= GetNumRounds())
        {
            int winnerSide = -1;
            mGames[mFirstGameInRound].GetWinnerAndLoser(
                &winnerSide, &mWinningMachine);
            fn_8032CA2C(lbl_806E2100, 32);
            fn_8032CA2C(lbl_806E2100, 33);
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
    NetMessageTournamentLoadingState message;
    message.mMachineIndex = mLocalMachineIndex;
    message.mFinishedLoadingToKnockout = true;
    u8 buffer[0xFF];
    int size = fn_8032C830(lbl_806E2100, &message, buffer, sizeof(buffer));
    SendToAllTournamentMachines(buffer, size);
}

void NetTournManager::NotifyOverlayPopped(int)
{
    NetMessageTournamentGameUpdate message;
    message.mUpdateType = 2;
    message.mGameIndex = mCurrentGameIndex;
    message.mIsHomeMachine
        = mLocalMachineIndex == mTournamentToMachine[0];
    message.mGameStatus = 0;
    message.mGameTimeDelta = 0;
    message.mHasGameInfo = false;
    message.mUnidentified0F = 0;
    u8 buffer[0xFF];
    int size = fn_8032C830(lbl_806E2100, &message, buffer, sizeof(buffer));
    SendToAllTournamentMachines(buffer, size);
}

void NetTournManager::NotifyGameOver()
{
    NetMessageTournamentGameUpdate message;
    message.mUpdateType = 0;
    message.mGameIndex = mCurrentGameIndex;
    message.mIsHomeMachine
        = mLocalMachineIndex == mTournamentToMachine[0];
    message.mGameStatus = 1;
    message.mGameTimeDelta = 0;
    message.mHasGameInfo = true;
    message.mUnidentified0F = 0;
    memcpy(message.mGameInfo, GameInfoManager::Instance()->GetCurrentGameInfo(),
        sizeof(message.mGameInfo));
    u8 buffer[0xFF];
    int size = fn_8032C830(lbl_806E2100, &message, buffer, sizeof(buffer));
    SendToAllTournamentMachines(buffer, size);
}

void NetTournManager::ResetGameProgressUpdateTimer(int)
{
    mLastGameProgressUpdate = -1;
    mGameProgressUpdateCount = 0;
}

int NetTournManager::ReceiverVirtual00(UnidentifiedNetworkMessage* message)
{
    switch (message->GetType())
    {
    case 32:
        HandleTournamentGameUpdate(
            static_cast<NetMessageTournamentGameUpdate*>(message));
        break;
    case 33:
    {
        NetMessageTournamentLoadingState* loading
            = static_cast<NetMessageTournamentLoadingState*>(message);
        int machine = (s8)loading->mMachineIndex;
        if (machine >= 0 && machine < mMachineCount)
        {
            if (loading->mFinishedLoadingToKnockout)
            {
                mLoadedToKnockout[machine] = true;
            }
            else
            {
                mLoadedToGame[machine] = true;
            }
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
        fn_8004F594(16,
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
        memcpy(&game.mGameInfo, message->mGameInfo, sizeof(game.mGameInfo));
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

int NetTournManager::GetRoundMask(int, int round) const
{
    int mask = 0;
    if (round == GetNumRounds() - 1)
    {
        mask = 1;
    }
    else if (round == GetNumRounds() - 2)
    {
        mask = 2;
    }
    else if (round == GetNumRounds() - 3)
    {
        mask = 4;
    }
    return mask;
}

int NetTournManager::GetNumTournamentGames() const
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
    static const char* paths[10] = {
        "art/characters/npcs/trophymushroom/trophymushroom",
        "art/characters/npcs/trophyflower/trophyflower",
        "art/characters/npcs/trophystar/trophystar",
        "art/characters/npcs/trophysunshine/trophysunshine",
        "art/characters/npcs/trophybanana/trophybanana",
        "art/characters/npcs/trophynextlevelcup/trophynextlevelcup",
        "art/characters/npcs/trophykonga/trophykonga",
        "art/characters/npcs/trophysand/trophysand",
        "art/characters/npcs/trophylava/trophylava",
        "art/characters/npcs/trophynintendo/trophynintendo",
    };
    int stadium = GetStadium();
    if (sCupPersonaOverrideActive)
    {
        stadium = sCupPersonaOverride;
    }
    if (stadium < 0 || stadium >= 10)
    {
        stadium = 0;
    }
    return paths[stadium];
}

int NetTournManager::GetNumRounds() const
{
    return mLargeBracket ? 3 : 2;
}

bool NetTournManager::IsFinalRound() const
{
    return mCurrentRound == GetNumRounds() - 1;
}

s16 NetTournManager::GetCurrentRoundNumber() const
{
    return mCurrentRound;
}

int NetTournManager::IsTournamentMode() const
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
    void* operator new(unsigned long size) { return fn_802B1C4C(size); }
    void operator delete(void* p)
    {
        fn_802B1D4C(p, sizeof(NetTournManagerDelegate));
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
