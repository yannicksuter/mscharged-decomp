#ifndef _FESCENE_H_
#define _FESCENE_H_

#include "Game/FE/feResourceManager.h"
#include "Game/FE/feSceneResource.h"
#include "NL/nlMath.h"
#include "types.h"

class FEPackage;
class MemoryAllocator;
struct FE_FILE_HEADER;

class FEScene
{
public:
    FEScene();
    ~FEScene();

    bool LoadPackage(const char* szPackageFileName, MemoryAllocator* pAllocator);
    void UnloadPackage();
    void Update(float dt);
    void AllResourcesLoadedCallback();

    /* 0x00 */ FEPackage* m_pFEPackage;
    /* 0x04 */ unsigned long m_uHashID;
    /* 0x08 */ nlMatrix4 m_matView;
    /* 0x48 */ unsigned long m_uRenderView;
    /* 0x4C */ FE_FILE_HEADER* field_0x4C;
    /* 0x50 */ unsigned long* field_0x50;
    /* 0x54 */ FESceneResource m_feSceneResourceHandle;
    /* 0x74 */ int mState;
    /* 0x78 */ FEResourceHandle* field_0x78;
    /* 0x7C */ MemoryAllocator* m_pAllocator;
}; // size 0x80

#endif // _FESCENE_H_
