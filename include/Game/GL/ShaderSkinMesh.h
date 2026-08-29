#ifndef GAME_GL_SHADER_SKIN_MESH_H
#define GAME_GL_SHADER_SKIN_MESH_H

#include "NL/nlMath.h"
#include "types.h"

class cSHierarchy;
class GLMaterialList;

class GLSkinMesh
{
public:
    GLSkinMesh()
        : m_pMaterialList(0)
        , m_Unknown08(0)
        , m_Unknown0C(0)
        , m_Unknown10(0)
        , m_Unknown14(0)
        , m_Unknown18(0)
        , m_Unknown1C(true)
    {
    }

    virtual ~GLSkinMesh();
    virtual void SetMaterialList(GLMaterialList* materialList);
    virtual void fn_Unknown2() = 0;
    virtual void fn_Unknown3() = 0;
    virtual void fn_Unknown4() = 0;
    virtual void fn_Unknown5() = 0;

    /* 0x04 */ GLMaterialList* m_pMaterialList;
    /* 0x08 */ unsigned long m_Unknown08;
    /* 0x0C */ void* m_Unknown0C;
    /* 0x10 */ void* m_Unknown10;
    /* 0x14 */ unsigned long m_Unknown14;
    /* 0x18 */ unsigned long m_Unknown18;
    /* 0x1C */ bool m_Unknown1C;
};

struct BoneMapList
{
    BoneMapList()
        : m_next(0)
        , m_pBoneIndices(0)
        , m_pMatrices(0)
    {
    }

    /* 0x00 */ BoneMapList* m_next;
    /* 0x04 */ unsigned long m_nBones;
    /* 0x08 */ int* m_pBoneIndices;
    /* 0x0C */ nlMatrix4* m_pMatrices;
};

class ShaderSkinMesh : public GLSkinMesh
{
public:
    ShaderSkinMesh()
        : boneMaps(0)
        , m_Unknown24(0)
        , m_Unknown28(0)
        , m_Unknown38(0)
        , m_Unknown3C(0)
        , m_Unknown45(false)
    {
    }

    virtual ~ShaderSkinMesh();
    virtual void fn_Unknown2();
    virtual void fn_Unknown3();
    virtual void fn_Unknown4();
    virtual void fn_Unknown5();

    void fn_802D407C(unsigned long count);
    void fn_802D40F4(unsigned long index, unsigned long id);
    void fn_8036F768(unsigned long count);
    void fn_8036F7B0(unsigned long firstIndex, unsigned long secondIndex,
        unsigned long count, const void* data);
    void fn_8036F7E4();
    void fn_8036FB74(cSHierarchy* hierarchy);
    void fn_8036FC4C(int nodeIndex, const nlMatrix4* matrix);

    /* 0x20 */ BoneMapList* boneMaps;
    /* 0x24 */ void* m_Unknown24;
    /* 0x28 */ void* m_Unknown28;
    /* 0x2C */ nlMatrix4* m_Unknown2C;
    /* 0x30 */ nlMatrix4* m_Unknown30;
    /* 0x34 */ unsigned long m_Unknown34;
    /* 0x38 */ unsigned long m_Unknown38;
    /* 0x3C */ void* m_Unknown3C;
    /* 0x40 */ void* m_Unknown40;
    /* 0x44 */ unsigned char m_Unknown44;
    /* 0x45 */ bool m_Unknown45;
};

#endif // GAME_GL_SHADER_SKIN_MESH_H
