#ifndef UNCLASSIFIED_TU_801FADB4_H
#define UNCLASSIFIED_TU_801FADB4_H

#include "Game/FE/BaseOverlayHandler.h"

class TU801FADB4Overlay : public BaseOverlayHandler
{
public:
    TU801FADB4Overlay();
    virtual ~TU801FADB4Overlay();
    virtual void Update(float fDeltaT);
    virtual void SceneCreated();

    void fn_801FB12C();

    /* 0x25 */ bool mUnidentified25;
    /* 0x26 */ bool mUnidentified26;
    /* 0x27 */ u8 mPadding27;
    /* 0x28 */ int mUnidentified28;
    /* 0x2C */ int mUnidentified2C;
    /* 0x30 */ float mUnidentified30;
    /* 0x34 */ float mUnidentified34;
    /* 0x38 */ void* mUnidentified38;
    /* 0x3C */ int mUnidentified3C;
}; // size 0x40

#endif // UNCLASSIFIED_TU_801FADB4_H
