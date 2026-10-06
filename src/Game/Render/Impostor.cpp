#include "NL/nlDLListContainer.inl"
#include "Game/Render/Impostor.h"

#include "Game/Render/ImpostorCharacter.h"
#include "Game/UnidentifiedStaticStorage.h"

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
    mSkipCrowdVisibilityPass = false;
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
    mSkipCrowdVisibilityPass = false;
}

void Impostor::Set(ImpostorCharacter* character, const nlVector3& position,
    float width, float height, u16 angle)
{
    mpCharacter = character;
    mpSprite = 0;
    mWidth = width;
    mHeight = height;
    mPosition = position;
    mAngle = angle;
    character->Acquire(this);
    mSkipCrowdVisibilityPass = false;
    mColour.c[0] = 0xFF;
    mColour.c[1] = 0xFF;
    mColour.c[2] = 0xFF;
    mColour.c[3] = 0xFF;
}

void Impostor::Release()
{
    if (mpSprite != 0)
    {
        mpSprite->QueueSlot(mSlot);
    }
}

Impostor::~Impostor()
{
}
