#ifndef _FEIMAGE_H_
#define _FEIMAGE_H_

#include "Game/FE/feLibObject.h"

class FETextureResource;

class FEImage : public FELibObject
{
public:
    FEImage();

    /* 0x78 */ FETextureResource* m_pFeTextureResource;
};

#endif
