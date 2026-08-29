#ifndef _FESCENERESOURCE_H_
#define _FESCENERESOURCE_H_

#include "Game/FE/feResourceManager.h"

class FEScene;

class FESceneResource : public FEResourceHandle
{
public:
    FESceneResource();

    /* 0x18 */ FEScene* m_pFESceneContext;
    /* 0x1C */ unsigned long m_glResourceMarker;
}; // total size: 0x20

#endif // _FESCENERESOURCE_H_
