#ifndef GAME_RENDER_DEPTH_OF_FIELD_H
#define GAME_RENDER_DEPTH_OF_FIELD_H

#include "types.h"

class DepthOfFieldManager
{
public:
    static DepthOfFieldManager instance;

    DepthOfFieldManager();

    void Initialize();
    void TurnOn();
    void TurnOff();
    void Update();

    /* 0x00 */ u8 m_bOn;
    /* 0x01 */ u8 m_bDebugView;
    /* 0x04 */ float m_fDistanceFromCamera;
    /* 0x08 */ float m_fIntensity;
    /* 0x0C */ void* mUnidentified0C;
}; // total size: 0x10

#endif // GAME_RENDER_DEPTH_OF_FIELD_H
