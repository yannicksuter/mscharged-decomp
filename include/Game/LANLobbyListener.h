#ifndef GAME_LAN_LOBBY_LISTENER_H
#define GAME_LAN_LOBBY_LISTENER_H

struct LANGameInfo;

enum eLANLobbyResult
{
    LAN_RESULT_OK = 0,
    LAN_RESULT_NO_LOCAL_ADDRESS = 1,
    LAN_RESULT_INVALID_STATE = 2,
    LAN_RESULT_NOT_HOST = 3,
    LAN_RESULT_CONNECTION_FAILED = 4,
    LAN_RESULT_NO_GAME_FOUND = 5,
    LAN_RESULT_JOIN_REFUSED = 6,
    LAN_RESULT_CONNECTION_LOST = 7,
    LAN_RESULT_CANCELED = 8,
    LAN_RESULT_CONFIRM_TIMEOUT = 9,
};

class LANLobbyListener
{
public:
    virtual void OnGameCreated(int result) = 0;
    virtual void OnGameJoined(int result) = 0;
    virtual void OnGameLaunched(int result) = 0;
    virtual void OnGameFound(LANGameInfo* game) = 0;
    virtual void OnReservedLobbyEvent() = 0;
    virtual void OnGameExpired(LANGameInfo* game) = 0;
    virtual void OnLobbyShutdown() = 0;
};

class LANLobbyPlayerListener
{
public:
    virtual void OnPlayerListChanged() = 0;
};

#endif // GAME_LAN_LOBBY_LISTENER_H
