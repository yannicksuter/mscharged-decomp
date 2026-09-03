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

#endif // NL_GLX_GLXMEMORY_H
