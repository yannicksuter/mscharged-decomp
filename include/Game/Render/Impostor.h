#ifndef GAME_RENDER_IMPOSTOR_H
#define GAME_RENDER_IMPOSTOR_H

#include "NL/nlColour.h"
#include "NL/nlMath.h"
#include "types.h"

class ImpostorCharacter;
class ImpostorSprite_802D4290;

class Impostor
{
public:
    Impostor();
    virtual ~Impostor();

    void Reset();
    void Set(ImpostorCharacter* character, const nlVector3& position,
        u16 angle, float width, float height);
    void Release();

    /* 0x04 */ ImpostorCharacter* mpCharacter;
    /* 0x08 */ ImpostorSprite_802D4290* mpSprite;
    /* 0x0C */ nlVector3 mPosition;
    /* 0x18 */ float mWidth;
    /* 0x1C */ float mHeight;
    /* 0x20 */ u16 mAngle;
    /* 0x22 */ nlColour mColour;
    /* 0x26 */ u8 mUnidentified026[2];
    /* 0x28 */ int mSlot;
    /* 0x2C */ bool mUnidentified02C;
}; // size: 0x30

#endif // GAME_RENDER_IMPOSTOR_H
