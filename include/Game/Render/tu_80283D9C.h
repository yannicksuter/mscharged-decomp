#ifndef GAME_RENDER_TU_80283D9C_H
#define GAME_RENDER_TU_80283D9C_H

#include "types.h"

class NisPlayer;

/**
 * Base of the five screen overlays NisPlayer allocates and owns. Each one
 * renders a 2D quad over the world view while a NIS plays; Update() returns the
 * overlay state the player should continue with. The empty Reset() body is
 * defined in Game/NisPlayer.cpp.
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

class NisPlayerOverlay_80523808 : public NisPlayerOverlay
{
public:
    NisPlayerOverlay_80523808(NisPlayer* player, float duration);
    virtual ~NisPlayerOverlay_80523808();
    virtual void Reset();
    virtual int Update(float dt);
    virtual void Render();
    virtual int GetOverlayType();

    /* 0x08 */ float mTime;
    /* 0x0C */ float mDuration;
}; // size 0x10

class NisPlayerOverlay_80523824 : public NisPlayerOverlay
{
public:
    NisPlayerOverlay_80523824(NisPlayer* player);
    virtual ~NisPlayerOverlay_80523824();
    virtual int Update(float dt);
    virtual void Render();
    virtual int GetOverlayType();
}; // size 0x08

class NisPlayerOverlay_80523840 : public NisPlayerOverlay
{
public:
    NisPlayerOverlay_80523840(NisPlayer* player);
    virtual ~NisPlayerOverlay_80523840();
    virtual int Update(float dt);
    virtual void Render();
    virtual int GetOverlayType();
}; // size 0x08

class NisPlayerOverlay_8052385C : public NisPlayerOverlay
{
public:
    NisPlayerOverlay_8052385C(NisPlayer* player);
    virtual ~NisPlayerOverlay_8052385C();
    virtual int Update(float dt);
    virtual void Render();
    virtual int GetOverlayType();
}; // size 0x08

class NisPlayerOverlay_80523878 : public NisPlayerOverlay
{
public:
    NisPlayerOverlay_80523878(NisPlayer* player);
    virtual ~NisPlayerOverlay_80523878();
    virtual int Update(float dt);
    virtual void Render();
    virtual int GetOverlayType();
}; // size 0x08

void fn_8028468C(float x, float y, float z);
void fn_8028469C(bool value);

#endif // GAME_RENDER_TU_80283D9C_H
