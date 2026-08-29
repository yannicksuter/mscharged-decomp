#ifndef GAME_FE_TEXTURE_RESOURCE_H
#define GAME_FE_TEXTURE_RESOURCE_H

#include "Game/FE/feResourceManager.h"

class FETextureResource : public FEResourceHandle
{
public:
    FETextureResource();

    unsigned long GetTextureHandle() const;

    /* 0x18 */ unsigned long m_glTextureHandle;
    /* 0x1C */ signed short m_width;
    /* 0x1E */ signed short m_height;
}; // size 0x20

#endif // GAME_FE_TEXTURE_RESOURCE_H
