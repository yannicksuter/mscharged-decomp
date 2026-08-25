#ifndef GAME_OBJECT_BLUR_H
#define GAME_OBJECT_BLUR_H

#include "NL/nlMath.h"

class BlurHandler
{
public:
    void Die(float timeToDie);
    void AddViewOrientedPoint(const nlVector3& position, const nlVector3& forwardVector);

private:
    /* 0x00 */ u8 mUnidentified00[0x10];

public:
    /* 0x10 */ float m_fLineWidth;
};

class BlurManager
{
public:
    static BlurHandler* GetNewHandler(
        const char* szTextureName, float fLineWidth, int maxPositionEntries, bool bAdditive);
};

#endif // GAME_OBJECT_BLUR_H
