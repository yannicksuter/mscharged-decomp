#ifndef GAME_EVENT_DATA_TYPES_H
#define GAME_EVENT_DATA_TYPES_H

#include "NL/nlMath.h"

class cPlayer;
class CrowdRiot;
class PhysicsObject;
class PhysicsSphere_80175F8C;

struct UnidentifiedEventData_800673FC
{
    /* 0x00 */ cPlayer* mUnidentified00;
}; // total size: 0x4

struct UnidentifiedEventData_80066B08
{
    /* 0x00 */ nlVector3 mUnidentified00;
    /* 0x0C */ PhysicsObject* mUnidentified0C;
    /* 0x10 */ CrowdRiot* mUnidentified10;
}; // total size: 0x14

struct UnidentifiedEventData_80066A04
{
    /* 0x00 */ cPlayer* mUnidentified00;
    /* 0x04 */ int mUnidentified04;
    /* 0x08 */ void* mUnidentified08;
    /* 0x0C */ int mUnidentified0C;
}; // total size: 0x10

struct UnidentifiedEventData00;
struct UnidentifiedEventData01;
struct UnidentifiedEventData02;
struct UnidentifiedEventData03;
struct UnidentifiedEventData04;
struct UnidentifiedEventData05;
struct UnidentifiedEventData06;
struct UnidentifiedEventData07;
struct UnidentifiedEventData08;
struct UnidentifiedEventData09;
struct UnidentifiedEventData10;
struct UnidentifiedEventData11;
struct UnidentifiedEventData12;
struct UnidentifiedEventData13;
struct UnidentifiedEventData14;
struct UnidentifiedEventData15;
struct UnidentifiedEventData16;
struct UnidentifiedEventData17;
struct UnidentifiedEventData18;
struct UnidentifiedEventData19;
struct UnidentifiedEventData20;
struct UnidentifiedEventData21;
struct UnidentifiedEventData22;
struct UnidentifiedEventData23;
struct UnidentifiedEventData24;
struct UnidentifiedEventData25;
struct UnidentifiedEventData26;
struct UnidentifiedEventData27;
struct UnidentifiedEventData28;
struct UnidentifiedEventData29;
struct UnidentifiedEventData30;
struct UnidentifiedEventData31;
struct UnidentifiedEventData32
{
    void* source;
    void* sourceValue;
    void* target;
};
struct UnidentifiedEventData33;
struct UnidentifiedEventData34;
struct UnidentifiedEventData35;
struct UnidentifiedEventData36;
struct UnidentifiedEventData37;

struct UnidentifiedEventData38
{
    /* 0x00 */ PhysicsSphere_80175F8C* mUnidentified00;
    /* 0x04 */ PhysicsObject* mUnidentified04;
    /* 0x08 */ unsigned int mUnidentified08;
}; // total size: 0xC

struct CollisionPlayerWallData
{
    /* 0x00 */ cPlayer* pPlayer;
    /* 0x04 */ nlVector3 contactPoint;
    /* 0x10 */ nlVector3 wallNormal;
}; // total size: 0x1C

#endif // GAME_EVENT_DATA_TYPES_H
