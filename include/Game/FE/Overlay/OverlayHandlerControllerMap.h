#ifndef GAME_FE_OVERLAY_HANDLER_CONTROLLER_MAP_H
#define GAME_FE_OVERLAY_HANDLER_CONTROLLER_MAP_H

#include "Game/FE/BaseOverlayHandler.h"
#include "Game/FE/feAsyncImage.h"
#include "Game/FE/fePointerButton.h"

class TLComponentInstance;
class TLImageInstance;

class ControllerMapOverlay : public BaseOverlayHandler
{
public:
    ControllerMapOverlay();
    virtual ~ControllerMapOverlay();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();
    inline void InitializeDoneButton();

    void OnDonePointerEnter(int index, void* context);
    void OnDonePointerLeave(int index, void* context);
    void OnDonePointerPress(int index, void* context);

    /* 0x028 */ FEPointerButton mDoneButton;
    /* 0x0DC */ TLComponentInstance* mDoneButtonInstance;
    /* 0x0E0 */ TLImageInstance* mMapImage;
    /* 0x0E4 */ bool mButtonInitialized;
    /* 0x0E5 */ bool mDonePressed;
    /* 0x0E6 */ bool mImageLoaded;
    /* 0x0E7 */ u8 mPadding0E7;
    /* 0x0E8 */ AsyncImage mAsyncImage;
}; // size 0x188

#endif // GAME_FE_OVERLAY_HANDLER_CONTROLLER_MAP_H
