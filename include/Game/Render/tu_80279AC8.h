#ifndef GAME_RENDER_TU_80279AC8_H
#define GAME_RENDER_TU_80279AC8_H

#include "Game/MathHelpers.h"
#include "NL/nlMath.h"
#include "types.h"

class glModel;
class GLView;
class nlMatrix4;

// Stadium world objects. They extend the shared world drawable the
// world-animation unit operates on (fn_80343B14 / fn_80343B34 / fn_80343C14 /
// fn_80343DE4), so only the fields this unit touches are named here; the rest
// of the drawable is padding, as in Game/World/worldanim.cpp.
class StadiumWorldObject_80279AC8
{
public:
    virtual ~StadiumWorldObject_80279AC8() { }
    virtual void V1();
    virtual nlMatrix4* GetWorldMatrix();
    virtual void SetWorldMatrix(const nlMatrix4* transform);
    virtual void V4(void* world);
    virtual void Draw();
    virtual bool V6(const nlVector4* planes);
    virtual void V7(glModel* model);
    virtual void V8(GLView* view);
    virtual void V9(void* context);

    float GetBlend() const { return m_fBlend; }

    /* 0x04 */ unsigned long m_uHashID;
    /* 0x08 */ u8 m_pad08[0x10];
    /* 0x18 */ void* m_pAnimController;
    /* 0x1C */ u8 m_pad1C[0x14];
    /* 0x30 */ nlVector3 m_lightPosition;
    /* 0x3C */ u8 m_pad3C[0x28];
    union
    {
        /* 0x64 */ glModel* m_pModel;
        /* 0x64 */ float m_fLightRange;
    };
    /* 0x68 */ u8 m_pad68[0x08];
    union
    {
        struct
        {
            /* 0x70 */ nlVector3 m_boundsMin;
            /* 0x7C */ nlVector3 m_boundsMax;
        };
        /* 0x70 */ glModel* m_pLayerModels[2];
        struct
        {
            /* 0x70 */ unsigned long m_uCupTrophyKey;
            /* 0x74 */ float m_fCupTrophyOpacity;
        };
    };
    /* 0x88 */ unsigned long m_uFlags;
    /* 0x8C */ float m_fBlend;
};

// The six sibling object types of this unit. Only their destructors are
// recovered as class members; every other override is still an address-named
// entry of the tables the neighbouring units hold.
class StadiumGoalObject_8027A2C8 : public StadiumWorldObject_80279AC8
{
public:
    virtual ~StadiumGoalObject_8027A2C8();

    void SetOpacity(float opacity)
    {
        float clamped = nlMaxEquals(opacity, 0.0f);
        clamped = nlMinEquals(clamped, 1.0f);
        m_fCupTrophyOpacity = clamped;
    }
};

class StadiumLayerObject_8027A130 : public StadiumWorldObject_80279AC8
{
public:
    virtual ~StadiumLayerObject_8027A130();
};

class StadiumIndicatorObject_8027A0C8 : public StadiumWorldObject_80279AC8
{
public:
    virtual ~StadiumIndicatorObject_8027A0C8();
};

class StadiumEffectObject_8027A11C : public StadiumWorldObject_80279AC8
{
public:
    virtual ~StadiumEffectObject_8027A11C();
};

class StadiumCrowdObject_8027A208 : public StadiumWorldObject_80279AC8
{
public:
    virtual ~StadiumCrowdObject_8027A208();
};

class StadiumLightObject_8027A054 : public StadiumWorldObject_80279AC8
{
public:
    virtual ~StadiumLightObject_8027A054();
};

#endif // GAME_RENDER_TU_80279AC8_H
