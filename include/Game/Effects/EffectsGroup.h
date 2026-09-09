#ifndef GAME_EFFECTS_EFFECTS_GROUP_H
#define GAME_EFFECTS_EFFECTS_GROUP_H

#include "Game/Effects/EffectsTemplate.h"
#include "NL/nlMath.h"

enum eFXBinding
{
    FXBind_Emitter = 0,
    FXBind_Joint = 1,
    FXBind_Object = 2,
};

enum eJointBinding
{
    JB_Normal = 0,
    JB_Ascend = 1,
    JB_Num = 2
};

struct EffectsSpec
{
    /* 0x00 */ unsigned long m_uHashID;
    union
    {
        /* 0x04 */ EffectsTemplate* m_pTemplate;
        /* 0x04 */ unsigned long m_uTemplateIndex;
    };
    /* 0x08 */ eFXBinding m_eAttach;
    /* 0x0C */ unsigned long m_uJointID;
    /* 0x10 */ float m_fDelay;
    /* 0x14 */ eJointBinding m_eJointBinding;
    /* 0x18 */ float m_fJointVelocity;
    /* 0x1C */ unsigned long m_bInFront;
    /* 0x20 */ unsigned long m_bGround;
    /* 0x24 */ unsigned long m_bLight;
    /* 0x28 */ float m_fOffset;
    /* 0x2C */ nlVector3 m_vLocalOffset;
    /* 0x38 */ unsigned long mUnidentified038;
    /* 0x3C */ float m_fLingerStart;
    /* 0x40 */ float m_fLingerEnd;
    /* 0x44 */ unsigned long m_uLayer;
    /* 0x48 */ int mUnidentified048;
    /* 0x4C */ unsigned char mPadding04C[0x0C];
}; // size: 0x58

class nlChunk;
class GLView;

struct UserEffectInfo
{
    nlVector3* pv3Position;
    nlVector3* pv3Direction;
};

class UserEffectSpec
{
public:
    virtual ~UserEffectSpec();
    virtual void Update(float dt, UserEffectInfo* info);
    virtual void Render(UserEffectInfo* info, GLView* view);
    virtual bool IsFinished();
    virtual UserEffectSpec* Clone();
};

struct UserEffectSource
{
    /* 0x00 */ unsigned long mSize;
    /* 0x04 */ char* mData;
};

class EffectsGroup
{
public:
    bool IsPersistent() const;
    unsigned long GetHashID() const { return m_hashID; }
    UserEffectSpec** GetUserSpecs() const { return m_userSpecsPtr; }
    static EffectsGroup* LoadFromChunk(nlChunk* chunk);
    void ParseUserSpecs();
    void DestroyUserSpecs();
    void ResolveTemplates(EffectsTemplate** table);

    /* 0x00 */ unsigned long m_hashID;
    /* 0x04 */ EffectsSpec* m_specs;
    /* 0x08 */ unsigned long m_numSpecs;
    /* 0x0C */ unsigned long mUnidentified0C;
    /* 0x10 */ UserEffectSpec** m_userSpecsPtr;
    /* 0x14 */ unsigned long m_userSpecs;
    /* 0x18 */ UserEffectSource* mUserSpecSources;
};
void SetEffectsGroupFountainLife(EffectsGroup* group, float life);

#endif // GAME_EFFECTS_EFFECTS_GROUP_H
