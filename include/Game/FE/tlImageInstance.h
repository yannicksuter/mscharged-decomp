#ifndef GAME_FE_TL_IMAGE_INSTANCE_H
#define GAME_FE_TL_IMAGE_INSTANCE_H

#include "Game/FE/tlInstance.h"

class FETextureResource;

class TLImageInstance : public TLInstance
{
public:
    TLImageInstance(FELibObject* component)
        : TLInstance(component)
    {
        m_pTextureResource = 0;
        m_type = TLAT_IMAGE;
    }

    FETextureResource*& fn_802332D0();
    void fn_802332D8(FETextureResource* resource);

    /* 0x90 */ FETextureResource* m_pTextureResource;
    /* 0x94 */ unsigned long field_0x94;
}; // size 0x98


#endif // GAME_FE_TL_IMAGE_INSTANCE_H
