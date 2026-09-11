#include "Game/GL/ShaderSkinMesh.h"

#include "Game/PoseAccumulator.h"
#include "NL/nlMemory.h"
#include "Game/UnidentifiedStaticStorage.h"

GLSkinMesh::~GLSkinMesh()
{
    if (morphWeights != 0)
    {
        delete[] morphWeights;
    }
}

void GLSkinMesh::SetNumMorphs(unsigned long count)
{
    if (numMorphs != count || morphWeights == 0)
    {
        numMorphs = count;
        if (morphWeights != 0)
        {
            delete[] morphWeights;
        }
        morphWeights = (MorphWeight*)nlMalloc(
            count * sizeof(MorphWeight), 8, false);
    }
}

void GLSkinMesh::SetMorphID(unsigned long index, unsigned long id)
{
    morphWeights[index].morphID = id;
}

void GLSkinMesh::UpdateMorphWeights(cPoseAccumulator* pPoseAccumulator)
{
    MorphWeight* morph;
    bool changed = false;

    if (!gMorphOverrideEnabled)
    {
        for (unsigned long i = 0; i < numMorphs; ++i)
        {
            morph = &morphWeights[i];
            float morphWeight =
                pPoseAccumulator->m_MorphWeights.mData[morph->morphID];
            if (morphWeight != morph->morphWeight)
            {
                morph->morphWeight = morphWeight;
                changed = true;
            }
        }
    }

    morphWeightsChanged = changed;
    ApplyMorphOverride();

    unsigned long count = 0;
    for (unsigned long i = 0; i < numMorphs; ++i)
    {
        if (morphWeights[i].morphWeight > 0.0f)
        {
            ++count;
        }
    }
    numActiveMorphs = count;
}

void GLSkinMesh::ApplyMorphOverride()
{
    bool changed = false;

    if (!gMorphOverrideEnabled)
    {
        return;
    }

    for (unsigned long i = 0; i < numMorphs; ++i)
    {
        if (gMorphOverrideID != -1
            && (unsigned long)gMorphOverrideID == morphWeights[i].morphID)
        {
            if (gMorphOverrideWeight != morphWeights[i].morphWeight)
            {
                morphWeights[i].morphWeight = gMorphOverrideWeight;
                changed = true;
            }
        }
        else if (0.0f != morphWeights[i].morphWeight)
        {
            morphWeights[i].morphWeight = 0.0f;
            changed = true;
        }
    }

    morphWeightsChanged = changed;
}

void GLSkinMesh::SetModel(glModel* model)
{
    pModel = model;
}
