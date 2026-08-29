#ifndef NL_GLX_GX_MATERIAL_CRYSTAL_TWEAKS_H
#define NL_GLX_GX_MATERIAL_CRYSTAL_TWEAKS_H

#include "Game/TweakValue.h"

class GXMaterialFloatTweak_804F4190 : public TweakValueBase_8052BF70
{
public:
    GXMaterialFloatTweak_804F4190(
        const char* name, const char* category);
    virtual ~GXMaterialFloatTweak_804F4190();
    virtual int UnidentifiedVirtual0C();
    virtual int UnidentifiedVirtual10();
    virtual void UnidentifiedVirtual14(float*, float*, float*);
    virtual void UnidentifiedVirtual18();
    virtual void UnidentifiedVirtual1C();
    virtual void* UnidentifiedVirtual20();
    virtual int UnidentifiedVirtual24(char*, unsigned long);
    virtual void UnidentifiedVirtual28(const char*);
    virtual void UnidentifiedVirtual2C(TweakValueBase_8052BF70*);

    /* 0x0C */ float value;
}; // size: 0x10

extern GXMaterialFloatTweak_804F4190 sCrystalConfigTweak_8057B028;
extern GXMaterialFloatTweak_804F4190 sCrystalConfigTweak_8057B048;

#endif // NL_GLX_GX_MATERIAL_CRYSTAL_TWEAKS_H
