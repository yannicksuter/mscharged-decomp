#ifndef GAME_PLAYER_H
#define GAME_PLAYER_H

#include "Game/Character.h"

class cPlayer : public cCharacter
{
public:
    virtual ~cPlayer();
    virtual void PostPhysicsUpdate();
    virtual void PrePhysicsUpdate(float dt);
    virtual void PreUpdate(float dt);

protected:
    /* 0x1E4 */ u8 mUnknown1E4[0x144];
}; // total size: 0x328

#endif // GAME_PLAYER_H
