#ifndef GAME_LAN_LOBBY_LISTENER_H
#define GAME_LAN_LOBBY_LISTENER_H

struct LANGameInfo;

class LANLobbyListener
{
public:
    virtual void OnGameCreated(int result) = 0;
    virtual void OnGameJoined(int result) = 0;
    virtual void OnGameLaunched(int result) = 0;
    virtual void OnGameFound(LANGameInfo* game) = 0;
    virtual void UnidentifiedVirtual10() = 0;
    virtual void OnGameExpired(LANGameInfo* game) = 0;
    virtual void OnLobbyShutdown() = 0;
};

class LANLobbyPlayerListener
{
public:
    virtual void OnPlayerListChanged() = 0;
};

#endif // GAME_LAN_LOBBY_LISTENER_H
