#ifndef NL_GLX_GX_MATERIAL_SHADOW_TWEAKS_H
#define NL_GLX_GX_MATERIAL_SHADOW_TWEAKS_H

#include "Game/TweakValue.h"

class GXMaterialColourTweak_804FC520 : public TweakValueBase_8052BF70
{
public:
    GXMaterialColourTweak_804FC520(
        const char* name, const char* category);
    virtual ~GXMaterialColourTweak_804FC520();
    virtual int UnidentifiedVirtual0C();
    virtual int UnidentifiedVirtual10();
    virtual void UnidentifiedVirtual14(float*, float*, float*);
    virtual void UnidentifiedVirtual18();
    virtual void UnidentifiedVirtual1C();
    virtual void* UnidentifiedVirtual20();
    virtual int UnidentifiedVirtual24(char*, unsigned long);
    virtual void UnidentifiedVirtual28(const char*);
    virtual void UnidentifiedVirtual2C(TweakValueBase_8052BF70*);

    /* 0x0C */ int value;
}; // size: 0x10

extern GXMaterialColourTweak_804FC520 sShadowVolumeRed;
extern GXMaterialColourTweak_804FC520 sShadowVolumeGreen;
extern GXMaterialColourTweak_804FC520 sShadowVolumeBlue;
extern GXMaterialColourTweak_804FC520 sShadowVolumeAlpha;

#endif // NL_GLX_GX_MATERIAL_SHADOW_TWEAKS_H
