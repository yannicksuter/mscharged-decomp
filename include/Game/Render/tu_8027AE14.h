#ifndef GAME_RENDER_TU_8027AE14_H
#define GAME_RENDER_TU_8027AE14_H

#include "NL/nlMath.h"
#include "unclassified/tu_80188884.h"

class UnidentifiedObject_8027AE14 : public UnidentifiedObject_80188884
{
public:
    UnidentifiedObject_8027AE14(const nlVector3& param1);
    virtual ~UnidentifiedObject_8027AE14();
    virtual void UnidentifiedVirtual0C(float param1);

    /* 0x10 */ nlVector3 mUnidentified010;
    /* 0x1C */ float mUnidentified01C;
    /* 0x20 */ bool mUnidentified020;
}; // size: 0x24

class StadiumDrawable_8027ADC0
{
public:
    virtual ~StadiumDrawable_8027ADC0();
    virtual void V1();
    virtual nlMatrix4* GetWorldMatrix();
    virtual void SetWorldMatrix(const nlMatrix4* transform);
    virtual void V4(void* world);
    virtual void Draw();
    virtual bool V6(const nlVector4* planes);
    virtual void V7(void* model);
    virtual void V8(void* view);
    virtual void V9(void* context);

    /* 0x04 */ u8 m_Unknown04[0x1C];
    /* 0x20 */ nlMatrix4 m_WorldMatrix;
    /* 0x60 */ u8 m_Unknown60[0x10];
    /* 0x70 */ unsigned long m_Unknown70;
};

extern bool lbl_806E19B8;
extern StadiumDrawable_8027ADC0* lbl_806E19BC;


#endif // GAME_RENDER_TU_8027AE14_H
