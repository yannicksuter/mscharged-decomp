#ifndef GAME_FE_TL_IMAGE_INSTANCE_H
#define GAME_FE_TL_IMAGE_INSTANCE_H

#include "Game/FE/tlInstance.h"

class FETextureResource;

class TLImageInstance : public TLInstance
{
public:
    /* 0x90 */ FETextureResource* m_pTextureResource;
    /* 0x94 */ unsigned long field_0x94;
}; // size 0x98

#endif // GAME_FE_TL_IMAGE_INSTANCE_H
