#ifndef GAME_RENDER_CROWD_MANAGER_H
#define GAME_RENDER_CROWD_MANAGER_H

#include "types.h"

struct LoadFrame;
struct SaveFrame;

// Retail keeps the predecessor's CrowdManager object with its replay and
// update entry points; the layout follows the predecessor header.
class CrowdManager
{
public:
    void Uninitialize();
    void Replay(LoadFrame& frame);
    void Replay(SaveFrame& frame);
    void Update(float deltaTime);

    /* 0x00 */ int m_State;
    /* 0x04 */ float m_fTime;
    /* 0x08 */ float m_fAnimScale;
    /* 0x0C */ s32 m_nCurrentFrame;
    /* 0x10 */ u32 m_CurrentTexture;
    /* 0x14 */ int m_nNumFrames;
    /* 0x18 */ u32 m_TextureHandle;
    /* 0x1C */ char m_szTexture[64];
    /* 0x5C */ char m_szStadium[64];
    /* 0x9C */ u32 m_BundleLoadBase;

    static CrowdManager instance;
}; // total size: 0xA0

#endif // GAME_RENDER_CROWD_MANAGER_H
