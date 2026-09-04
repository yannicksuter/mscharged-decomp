#ifndef GAME_OBJECT_BLUR_H
#define GAME_OBJECT_BLUR_H

#include "NL/nlMath.h"

struct BlurPointEntry
{
    nlVector3 v3Top;
    nlVector3 v3Bottom;
};

class BlurHandler
{
public:
    void Die(float timeToDie);
    void AddViewOrientedPoint(const nlVector3& position, const nlVector3& forwardVector);
    bool ConstructViewOrientedPoints(nlVector3& topPoint, nlVector3& bottomPoint, nlVector3 position, const nlVector3& forwardVector);

    /* 0x00 */ BlurHandler* m_next;
    /* 0x04 */ BlurHandler* m_prev;
    /* 0x08 */ bool m_bAdditive;
    /* 0x09 */ u8 m_pad09[3];
    /* 0x0C */ unsigned long m_uTexHashID;
    /* 0x10 */ float m_fLineWidth;
    /* 0x14 */ BlurPointEntry m_pointFinal;
    /* 0x2C */ BlurPointEntry* m_pLastPoint;
    /* 0x30 */ bool m_bDying;
    /* 0x31 */ u8 m_pad31[3];
    /* 0x34 */ float m_fDyingTimer;
    /* 0x38 */ float m_fTimeToDie;
    /* 0x3C */ int m_maxPositionEntries;
    /* 0x40 */ BlurPointEntry* m_pointRingBuffer;
    /* 0x44 */ int m_nInsertIndex;
    /* 0x48 */ int m_nTrailEndPointer;
};

class BlurManager
{
public:
    static void Update(float fDeltaT);
    static void DestroyHandler(
        BlurHandler* handler, float timeToDie);
    static BlurHandler* GetNewHandler(
        const char* szTextureName, float fLineWidth, int maxPositionEntries, bool bAdditive);
};

#endif // GAME_OBJECT_BLUR_H
