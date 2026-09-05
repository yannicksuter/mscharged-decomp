#ifndef NL_GLX_GLXMEMORY_H
#define NL_GLX_GLXMEMORY_H

#include "NL/nlMath.h"

enum eGLMemory
{
    GLM_Header = 0,
    GLM_Matrix = 1,
    GLM_IndexData = 2,
    GLM_VertexData = 3,
    GLM_TextureData = 4,
    GLM_Target = 5,
    GLM_Num = 6,
};

void glplatSetMatrix(unsigned long matrix, const nlMatrix4& m);
void glplatGetMatrix(unsigned long matrix, nlMatrix4& m);
bool glxInitMemory(
    unsigned long frameMemSize1, unsigned long frameMemSize2);
void glplatFrameAllocNextFrame();
void* glplatResourceAlloc(
    unsigned long size, eGLMemory memType, void* resource);
void* glplatFrameAlloc(unsigned long size, eGLMemory memType);
unsigned long glx_GetFreeMemory();

struct UnidentifiedMemoryRequirement_80376664
{
    eGLMemory m_00;
    unsigned long m_04;
};

class ResourceInterface_802CC094;

ResourceInterface_802CC094* fn_80376664(
    const UnidentifiedMemoryRequirement_80376664* requirements,
    unsigned long count, const char* name);

#endif // NL_GLX_GLXMEMORY_H
