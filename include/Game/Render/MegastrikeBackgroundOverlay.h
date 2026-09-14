#ifndef GAME_RENDER_MEGASTRIKE_BACKGROUND_OVERLAY_H
#define GAME_RENDER_MEGASTRIKE_BACKGROUND_OVERLAY_H

class MegastrikeBackgroundOverlay
{
public:
    MegastrikeBackgroundOverlay()
        : mFadeRate(0.0f)
        , mAlpha(0.0f)
        , mTargetAlpha(0.0f)
        , mTeamIndex(-1)
        , mActive(false)
    {
    }

    void Start(float rate, float target, int mode);
    void UpdateAndRender(float deltaTime);

    /* 0x00 */ float mFadeRate;
    /* 0x04 */ float mAlpha;
    /* 0x08 */ float mTargetAlpha;
    /* 0x0C */ int mTeamIndex;
    /* 0x10 */ bool mActive;
}; // size: 0x14

extern MegastrikeBackgroundOverlay gMegastrikeBackgroundOverlay;

#endif // GAME_RENDER_MEGASTRIKE_BACKGROUND_OVERLAY_H
