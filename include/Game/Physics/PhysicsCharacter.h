#ifndef GAME_PHYSICS_PHYSICS_CHARACTER_H
#define GAME_PHYSICS_PHYSICS_CHARACTER_H

#include "Game/Physics/PhysicsCharacterBase.h"

class PhysicsColumn;
class cCharacter;

class PhysicsCharacter : public PhysicsCharacterBase
{
public:
    PhysicsCharacter(float radius, float heightScale);
    virtual ~PhysicsCharacter() { }

    virtual void Unknown0();
    virtual int GetObjectType() const { return 8; }
    virtual bool SetContactInfo(dContact*, PhysicsObject*, bool);
    virtual void PostUpdate();
    virtual void PreCollide();
    virtual ContactType Contact(
        PhysicsObject*, dContact*, int, PhysicsObject*);
    virtual PhysicsBoneID ResolvePhysicsBoneIDFromName(const char*);

    void DisablePhysicsColumn();
    void EnablePhysicsColumn();
    void GetRadius(float* radius);
    void SetCharacterVelocityXY(const nlVector3& vel);
    void GetCharacterVelocityXY(nlVector3* vel);
    void SetCharacterPositionXY(const nlVector3& pos);
    void GetCharacterPositionXY(nlVector3* pos);

    /* 0x98 */ unsigned int m_CanCollideWithWall : 1;
    /* 0x98 */ unsigned int m_CanCollideWithBall : 1;
    /* 0x98 */ unsigned int m_CanCollideWithCharacters : 1;
    /* 0x98 */ unsigned int m_HasCollidedWithBall : 1;
    /* 0x98 */ unsigned int m_CanCollideWithGoalLine : 1;
    /* 0x98 */ unsigned int mUnidentified098_5 : 27;
    /* 0x9C */ int m_nDKBallStuckHackCounter;
    /* 0xA0 */ bool m_bSupportingBallThisFrame;
    /* 0xA1 */ bool m_bInsideNet;
    /* 0xA2 */ bool m_bWasInsideNet;
    /* 0xA3 */ unsigned char mPaddingA3;
    /* 0xA4 */ cCharacter* m_pAICharacter;
    /* 0xA8 */ PhysicsColumn* m_pPlayerPlayerColumn;
}; // size: 0xAC

#endif // GAME_PHYSICS_PHYSICS_CHARACTER_H
