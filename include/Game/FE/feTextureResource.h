#ifndef _FETEXTURERESOURCE_H_
#define _FETEXTURERESOURCE_H_

#include "Game/FE/feResourceManager.h"

class FETextureResource : public FEResourceHandle
{
public:
    FETextureResource();

    void SetTextureHandle(const unsigned long& textureReference);
    u32 GetTextureHandle() const;

    /* 0x18 */ u32 m_glTextureHandle;
    /* 0x1C */ u16 m_uWidth;
    /* 0x1E */ u16 m_uHeight;
}; // total size: 0x20

#endif // _FETEXTURERESOURCE_H_
