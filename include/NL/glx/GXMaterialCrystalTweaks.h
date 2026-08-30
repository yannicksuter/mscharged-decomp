#ifndef NL_GLX_GX_MATERIAL_CRYSTAL_TWEAKS_H
#define NL_GLX_GX_MATERIAL_CRYSTAL_TWEAKS_H

#include "Game/TweakValue.h"
#include "NL/nlMemory.h"

extern const char* lbl_806E1E90;

class GXMaterialFloatTweak_804F4190 : public TweakValueBase_8052BF70
{
public:
    GXMaterialFloatTweak_804F4190(
        const char* name, const char* category, float initialValue = 1.0f)
        : value(initialValue)
    {
        mName = name;
        mUnidentified009 = true;

        if (fn_802C0F04() == 0)
        {
            void* entry = nlMalloc(0x18, 8, true);
            if (entry != 0)
                fn_802C2DF4((TweakPendingValue*)entry, this, category);
        }
        else
        {
            TweakEntry_8052BF00* config = fn_802C0E30();
            TweakEntry_8052BF00* entry = fn_802C4504(config, category, 0);
            if (entry != 0)
                fn_802C5780(entry, this);
        }

        lbl_806E1E90 = category;
    }
    GXMaterialFloatTweak_804F4190(const char* name, float initialValue)
        : value(initialValue)
    {
        mName = name;
    }
    virtual ~GXMaterialFloatTweak_804F4190();
    virtual int UnidentifiedVirtual0C();
    virtual int UnidentifiedVirtual10();
    virtual void UnidentifiedVirtual14(float*, float*, float*);
    virtual void UnidentifiedVirtual18();
    virtual void* UnidentifiedVirtual20();
    virtual void UnidentifiedVirtual24(char*, unsigned long);
    virtual void UnidentifiedVirtual28(const char*);
    virtual void UnidentifiedVirtual2C(TweakValueBase_8052BF70*);

    static void operator delete(void* pointer)
    {
        lbl_806E1E58->m_Pool1.Free(pointer);
    }

    /* 0x0C */ float value;
}; // size: 0x10

extern GXMaterialFloatTweak_804F4190 sCrystalConfigTweak_8057B028;
extern GXMaterialFloatTweak_804F4190 sCrystalConfigTweak_8057B048;

#endif // NL_GLX_GX_MATERIAL_CRYSTAL_TWEAKS_H
