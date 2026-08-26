#ifndef GAME_NET_H
#define GAME_NET_H

#include "NL/nlMath.h"

class cNet
{
public:
    void GetPostLocation(nlVector3& v3PostPosition,
        unsigned int uPostNum, float fYAdjust) const;

    static float GetNetHeight();
    static float GetNetWidth();
    static float GetPostRadius()
    {
        return m_fNetPostRadius;
    }

    /* 0x00 */ int m_nIndex;
    /* 0x04 */ float m_fDirection;

public:
    static float m_fNetHeight;
    static float m_fNetWidth;
    static float m_fNetDepth;
    static float m_fNetPostRadius;

    /* 0x08 */ nlVector3 m_v3NetLocation;
};

#endif // GAME_NET_H
