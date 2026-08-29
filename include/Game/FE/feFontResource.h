#ifndef GAME_FE_FONT_RESOURCE_H
#define GAME_FE_FONT_RESOURCE_H

#include "Game/FE/feResourceManager.h"

class nlFont;

class FEFontResource : public FEResourceHandle
{
public:
    void SetFontReference(nlFont* pFontReference);

    /* 0x18 */ nlFont* m_pFontReference;
}; // size 0x1C

#endif // GAME_FE_FONT_RESOURCE_H
