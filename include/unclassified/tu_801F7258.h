#ifndef UNCLASSIFIED_TU_801F7258_H
#define UNCLASSIFIED_TU_801F7258_H

#include "Game/FE/BaseOverlayHandler.h"
#include "Game/FE/feCaptainComponent.h"

class AsyncImage;
class TLComponentInstance;

class TU801F7258Overlay : public BaseOverlayHandler
{
public:
    TU801F7258Overlay();
    virtual ~TU801F7258Overlay();
    virtual void Update(float fParam1);
    virtual void SceneCreated();

    void fn_801F79B0();
    void fn_801F8414(int iParam1, int iParam2);

    /* 0x028 */ TLComponentInstance* mUnidentified028;
    /* 0x02C */ UnidentifiedTextFader mUnidentified02C;
    /* 0x06C */ u32 mUnidentified06C[4];
    /* 0x07C */ bool mUnidentified07C;
    /* 0x07D */ u8 mUnidentified07D[3];
    /* 0x080 */ u32 mUnidentified080[7];
    /* 0x09C */ bool mUnidentified09C;
    /* 0x09D */ u8 mUnidentified09D[3];
    /* 0x0A0 */ int mUnidentified0A0;
    /* 0x0A4 */ AsyncImage* mUnidentified0A4;
    /* 0x0A8 */ bool mUnidentified0A8;
    /* 0x0A9 */ u8 mUnidentified0A9[3];
    /* 0x0AC */ float mUnidentified0AC;
}; // size 0xB0

#endif // UNCLASSIFIED_TU_801F7258_H
