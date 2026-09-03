#include "Game/GL/ShaderSkinMesh.h"

#include "Game/PoseAccumulator.h"
#include "NL/nlMemory.h"

extern "C"
{
    extern int lbl_806DF408;
    extern float lbl_806DF40C;
    extern unsigned char lbl_806E1F40;
}

GLSkinMesh::~GLSkinMesh()
{
    if (m_Unknown10 != 0)
    {
        delete[] m_Unknown10;
    }
}

void GLSkinMesh::fn_802D407C(unsigned long count)
{
    if (numMorphs != count || m_Unknown10 == 0)
    {
        numMorphs = count;
        if (m_Unknown10 != 0)
        {
            delete[] m_Unknown10;
        }
        m_Unknown10 = (UnidentifiedGLSkinMeshEntry*)nlMalloc(
            count * sizeof(UnidentifiedGLSkinMeshEntry), 8, false);
    }
}

void GLSkinMesh::fn_802D40F4(unsigned long index, unsigned long id)
{
    m_Unknown10[index].morphID = id;
}

void GLSkinMesh::fn_802D4104(cPoseAccumulator* pPoseAccumulator)
{
    bool changed = false;

    if (!lbl_806E1F40)
    {
        for (unsigned long i = 0; i < numMorphs; ++i)
        {
            UnidentifiedGLSkinMeshEntry& morph = m_Unknown10[i];
            float morphWeight =
                pPoseAccumulator->m_MorphWeights.mData[morph.morphID];
            if (morphWeight != morph.morphWeight)
            {
                morph.morphWeight = morphWeight;
                changed = true;
            }
        }
    }

    m_Unknown1C = changed;
    fn_802D41D4();

    unsigned long count = 0;
    for (unsigned long i = 0; i < numMorphs; ++i)
    {
        if (m_Unknown10[i].morphWeight > 0.0f)
        {
            ++count;
        }
    }
    m_Unknown18 = count;
}

void GLSkinMesh::fn_802D41D4()
{
    bool changed = false;

    if (!lbl_806E1F40)
    {
        return;
    }

    for (unsigned long i = 0; i < numMorphs; ++i)
    {
        if (lbl_806DF408 != -1
            && (unsigned long)lbl_806DF408 == m_Unknown10[i].morphID)
        {
            if (lbl_806DF40C != m_Unknown10[i].morphWeight)
            {
                m_Unknown10[i].morphWeight = lbl_806DF40C;
                changed = true;
            }
        }
        else if (0.0f != m_Unknown10[i].morphWeight)
        {
            m_Unknown10[i].morphWeight = 0.0f;
            changed = true;
        }
    }

    m_Unknown1C = changed;
}

void GLSkinMesh::fn_802D4268(glModel* model)
{
    pModel = model;
}
