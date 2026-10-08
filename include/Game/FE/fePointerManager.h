#ifndef GAME_FE_FEPOINTERMANAGER_H
#define GAME_FE_FEPOINTERMANAGER_H

#include "NL/nlList.h"
#include "types.h"

class FEPointerListener;

class FEPointerManager
{
public:
    FEPointerManager();
    ~FEPointerManager();

    void RegisterListener(FEPointerListener* listener);
    void UnregisterListener(FEPointerListener* listener);

    /* 0x00 */ nlListContainer<FEPointerListener*> mListeners;
    /* 0x0C */ u32 m_pad00C[4];
    /* 0x1C */ u32 m_pad01C;
    /* 0x20 */ float m_pad020[4][2];
    /* 0x40 */ u16 mUnidentified040[4];
    /* 0x48 */ bool mUnidentified048[4];
    /* 0x4C */ bool mUnidentified04C[4];
    /* 0x50 */ int mListenerCount;
    /* 0x54 */ int m_pad054;
}; // size 0x58

extern FEPointerManager* g_pFEPointerManager;

#endif // GAME_FE_FEPOINTERMANAGER_H
