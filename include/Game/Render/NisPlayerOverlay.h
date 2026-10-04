#ifndef GAME_RENDER_NIS_PLAYER_OVERLAY_H
#define GAME_RENDER_NIS_PLAYER_OVERLAY_H

#include "types.h"

class NisPlayer;

/**
 * Base of the five overlay states NisPlayer allocates and owns. Update()
 * selects the next state; Render() draws the active screen or world overlay.
 * The default Reset() does nothing.
 */
class NisPlayerOverlay
{
public:
    virtual ~NisPlayerOverlay() { }
    virtual void Reset();
    virtual int Update(float dt) = 0;
    virtual void Render() = 0;
    virtual int GetOverlayType() = 0;

    /* 0x04 */ NisPlayer* mPlayer;
}; // size 0x08

class NisPlayerNoOverlay : public NisPlayerOverlay
{
public:
    NisPlayerNoOverlay(NisPlayer* player);
    virtual ~NisPlayerNoOverlay();
    virtual int Update(float dt);
    virtual void Render();
    virtual int GetOverlayType();
}; // size 0x08

class NisPlayerPIPOverlay : public NisPlayerOverlay
{
public:
    NisPlayerPIPOverlay(NisPlayer* player);
    virtual ~NisPlayerPIPOverlay();
    virtual int Update(float dt);
    virtual void Render();
    virtual int GetOverlayType();
}; // size 0x08

class NisPlayerHolotronOverlay : public NisPlayerOverlay
{
public:
    NisPlayerHolotronOverlay(NisPlayer* player);
    virtual ~NisPlayerHolotronOverlay();
    virtual int Update(float dt);
    virtual void Render();
    virtual int GetOverlayType();
}; // size 0x08

class NisPlayerCameraSwapOverlay : public NisPlayerOverlay
{
public:
    NisPlayerCameraSwapOverlay(NisPlayer* player);
    virtual ~NisPlayerCameraSwapOverlay();
    virtual int Update(float dt);
    virtual void Render();
    virtual int GetOverlayType();
}; // size 0x08

class NisPlayerPIPExpandOverlay : public NisPlayerOverlay
{
public:
    NisPlayerPIPExpandOverlay(NisPlayer* player, float duration);
    virtual ~NisPlayerPIPExpandOverlay();
    virtual void Reset();
    virtual int Update(float dt);
    virtual void Render();
    virtual int GetOverlayType();

    /* 0x08 */ float mTime;
    /* 0x0C */ float mDuration;
}; // size 0x10

void SetHolotronDimensions(float verticalOffset, float width, float height);
void SetHolotronCameraTrackingEnabled(bool enabled);

#endif // GAME_RENDER_NIS_PLAYER_OVERLAY_H
