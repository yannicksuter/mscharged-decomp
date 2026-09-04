#ifndef GAME_RENDER_JUMBOTRON_H
#define GAME_RENDER_JUMBOTRON_H

#include "types.h"

// Retail keeps the predecessor's Jumbotron object and its eight methods as
// empty stubs; the layout follows the predecessor header.
class Jumbotron
{
public:
    void Initialize();
    void Uninitialize();
    void Reset();
    void BeginLoad();
    void WaitForLoad();
    void BeginPlaying();
    void StopPlaying();
    void Update(float dt);

    /* 0x00 */ void* m_BundleLoadBase;
    /* 0x04 */ int m_State;
    /* 0x08 */ int m_AnimationClass;
    /* 0x0C */ float m_fFramerate;
    /* 0x10 */ float m_fTime;
    /* 0x14 */ u32 m_CurrentTexture;
    /* 0x18 */ int m_nCurrentFrame;
    /* 0x1C */ int m_nNumFrames;
    /* 0x20 */ int m_nMaxNumFrames;
    /* 0x24 */ char m_szPrefix[64];
    /* 0x64 */ char m_szTexture[128];

    static Jumbotron instance;
}; // total size: 0xE4

#endif // GAME_RENDER_JUMBOTRON_H
