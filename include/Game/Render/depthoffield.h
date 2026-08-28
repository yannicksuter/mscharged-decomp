#ifndef GAME_RENDER_DEPTH_OF_FIELD_H
#define GAME_RENDER_DEPTH_OF_FIELD_H

#include "types.h"

class DepthOfFieldManager
{
public:
    static DepthOfFieldManager instance;

    /* 0x00 */ u8 m_bOn;
    /* 0x01 */ u8 m_bDebugView;
    /* 0x04 */ float m_fDistanceFromCamera;
    /* 0x08 */ float m_fIntensity;
}; // total size: 0xC

#endif // GAME_RENDER_DEPTH_OF_FIELD_H
