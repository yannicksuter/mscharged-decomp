#ifndef UNCLASSIFIED_TU_801F877C_H
#define UNCLASSIFIED_TU_801F877C_H

#include "Game/FE/BaseOverlayHandler.h"
#include "Game/FE/feAsyncImage.h"

class TLComponentInstance;

class TU801F877COverlay : public BaseOverlayHandler
{
public:
    TU801F877COverlay();
    virtual ~TU801F877COverlay();
    virtual void Update(float fParam1);
    virtual void SceneCreated();
    virtual void SetVisible(bool bParam1);

    void fn_801F8B3C();

    /* 0x028 */ TLComponentInstance* mUnidentified028;
    /* 0x02C */ AsyncImage mUnidentified02C;
    /* 0x0CC */ bool mUnidentified0CC;
    /* 0x0CD */ u8 mUnidentified0CD[3];
    /* 0x0D0 */ float mUnidentified0D0;
}; // size 0xD4

#endif // UNCLASSIFIED_TU_801F877C_H
