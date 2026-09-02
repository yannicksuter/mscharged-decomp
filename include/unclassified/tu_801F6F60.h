#ifndef UNCLASSIFIED_TU_801F6F60_H
#define UNCLASSIFIED_TU_801F6F60_H

#include "Game/FE/BaseOverlayHandler.h"

class TU801F6F60Overlay : public BaseOverlayHandler
{
public:
    TU801F6F60Overlay();
    virtual ~TU801F6F60Overlay();
    virtual void Update(float fParam1);
    virtual void SceneCreated();

    /* 0x25 */ bool mUnidentified25;
    /* 0x28 */ float mUnidentified28;
}; // size 0x2C

#endif // UNCLASSIFIED_TU_801F6F60_H
