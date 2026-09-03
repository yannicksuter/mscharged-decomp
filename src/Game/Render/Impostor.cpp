#include "Game/Render/Impostor.h"

#include "Game/Render/ImpostorCharacter.h"
#include "Game/UnidentifiedStaticStorage.h"

extern "C" void fn_802D7664(ImpostorCharacter*, Impostor*);
Impostor::Impostor()
{
    mpCharacter = 0;
    mpSprite = 0;
    mWidth = 0.0f;
    mHeight = 0.0f;
    mPosition.x = 0.0f;
    mPosition.y = 0.0f;
    mPosition.z = 0.0f;
    mAngle = 0;
    mSlot = -1;
    mColour.c[0] = 0xFF;
    mColour.c[1] = 0xFF;
    mColour.c[2] = 0xFF;
    mColour.c[3] = 0xFF;
    mUnidentified02C = false;
}

Impostor::~Impostor()
{
}

void Impostor::Reset()
{
    mpCharacter = 0;
    mpSprite = 0;
    mWidth = 0.0f;
    mHeight = 0.0f;
    mPosition.x = 0.0f;
    mPosition.y = 0.0f;
    mPosition.z = 0.0f;
    mAngle = 0;
    mSlot = -1;
    mColour.c[0] = 0xFF;
    mColour.c[1] = 0xFF;
    mColour.c[2] = 0xFF;
    mColour.c[3] = 0xFF;
    mUnidentified02C = false;
}

void Impostor::Set(ImpostorCharacter* character, const nlVector3& position,
    u16 angle, float width, float height)
{
    mpCharacter = character;
    mpSprite = 0;
    mWidth = width;
    mHeight = height;
    mPosition = position;
    mAngle = angle;
    fn_802D7664(character, this);
    mUnidentified02C = false;
    mColour.c[0] = 0xFF;
    mColour.c[1] = 0xFF;
    mColour.c[2] = 0xFF;
    mColour.c[3] = 0xFF;
}

void Impostor::Release()
{
    if (mpSprite != 0)
    {
        fn_802D5078(mpSprite, mSlot);
    }
}

template struct UnidentifiedStaticStorage<UnidentifiedStaticTag>;
