#ifndef UNCLASSIFIED_TU_801FA28C_H
#define UNCLASSIFIED_TU_801FA28C_H

#include "Game/FE/BaseOverlayHandler.h"
#include "Game/FE/feAsyncImage.h"
#include "unclassified/tu_80219248.h"

class TLComponentInstance;
class TLImageInstance;

class TU801FA324Overlay : public BaseOverlayHandler
{
public:
    TU801FA324Overlay();
    virtual ~TU801FA324Overlay();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();

    void fn_801FAB00(int index, void* context);
    void fn_801FAB88(int index, void* context);
    void fn_801FABF8(int index, void* context);

    /* 0x028 */ TU80219248Component mControllerComponent;
    /* 0x0DC */ TLComponentInstance* mUnidentified0DC;
    /* 0x0E0 */ TLImageInstance* mUnidentified0E0;
    /* 0x0E4 */ bool mUnidentified0E4;
    /* 0x0E5 */ bool mUnidentified0E5;
    /* 0x0E6 */ bool mUnidentified0E6;
    /* 0x0E7 */ u8 mPadding0E7;
    /* 0x0E8 */ AsyncImage mUnidentified0E8;
}; // size 0x188

#endif // UNCLASSIFIED_TU_801FA28C_H
