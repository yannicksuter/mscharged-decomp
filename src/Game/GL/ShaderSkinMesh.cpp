#include "Game/GL/ShaderSkinMesh.h"

#include "Game/SHierarchy.h"
#include "NL/nlMemory.h"

void ShaderSkinMesh::fn_8036F768(unsigned long count)
{
    m_Unknown38 = count;
    m_Unknown3C = nlMalloc(count * m_Unknown14 * 8, 8, false);
}

void ShaderSkinMesh::fn_8036F7B0(unsigned long firstIndex,
    unsigned long secondIndex, unsigned long count, const void* data)
{
    unsigned long* entry = m_Unknown3C == 0
                             ? 0
                             : (unsigned long*)m_Unknown3C
                                   + (firstIndex * m_Unknown14 + secondIndex) * 2;

    entry[0] = count;
    entry[1] = (unsigned long)data;
}

void ShaderSkinMesh::fn_8036FB74(cSHierarchy* hierarchy)
{
    unsigned long hashID = hierarchy->GetHashID();
    int numNodes = hierarchy->GetNumNodes();

    m_Unknown08 = hashID;
    m_Unknown34 = numNodes;

    m_Unknown30 = (nlMatrix4*)nlMalloc(
        numNodes * sizeof(nlMatrix4), 8, false);
    for (int i = 0; i < hierarchy->GetNumNodes(); i++)
    {
        m_Unknown30[i].SetIdentity();
    }

    m_Unknown2C = (nlMatrix4*)nlMalloc(
        m_Unknown34 * sizeof(nlMatrix4), 8, false);
    for (int i = 0; i < hierarchy->GetNumNodes(); i++)
    {
        m_Unknown2C[i].SetIdentity();
    }
}

void ShaderSkinMesh::fn_8036FC4C(int nodeIndex, const nlMatrix4* matrix)
{
    m_Unknown2C[nodeIndex] = *matrix;
}

UnidentifiedShaderSkinData_80370808::~UnidentifiedShaderSkinData_80370808()
{
    delete[] m_Unknown08;
}

UnidentifiedShaderSkinEntry_80370868::~UnidentifiedShaderSkinEntry_80370868()
{
    delete[] m_Unknown04;
}

UnidentifiedShaderSkinData_80370808::UnidentifiedShaderSkinData_80370808()
    : m_Unknown00(0)
    , m_Unknown04(0)
    , m_Unknown08(0)
{
}

UnidentifiedShaderSkinEntry_80370868::UnidentifiedShaderSkinEntry_80370868()
    : m_Unknown00(0)
    , m_Unknown04(0)
{
}

glModel* ShaderSkinMesh::GetModel()
{
    bool unknown = false;
    if (!m_Unknown45 && m_Unknown0C == 0)
    {
        unknown = true;
    }
    if (unknown)
    {
        return (glModel*)m_Unknown28;
    }
    return (glModel*)m_pMaterialList;
}
