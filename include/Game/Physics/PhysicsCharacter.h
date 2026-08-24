#ifndef GAME_PHYSICS_PHYSICS_CHARACTER_H
#define GAME_PHYSICS_PHYSICS_CHARACTER_H

#include "Game/Physics/PhysicsCharacterBase.h"

class PhysicsCharacter : public PhysicsCharacterBase
{
public:
    void SetCharacterVelocityXY(const nlVector3& vel);
    void GetCharacterVelocityXY(nlVector3* vel);
    void SetCharacterPositionXY(const nlVector3& pos);
    void GetCharacterPositionXY(nlVector3* pos);
};

#endif // GAME_PHYSICS_PHYSICS_CHARACTER_H
