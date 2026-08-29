#ifndef GAME_FE_TEXT_H
#define GAME_FE_TEXT_H

#include "Game/FE/feFontResource.h"
#include "Game/FE/feLibObject.h"
#include "Game/FE/tlTextInstance.h"

class FEText : public FELibObject
{
public:
    /* 0x78 */ const FEFontResource* m_pFeFontResource;
    /* 0x7C */ FETextLibObjectAttributes m_TextAttributes;
}; // size 0x88

#endif // GAME_FE_TEXT_H
