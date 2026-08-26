#ifndef NL_GL_GLMATRIX_H
#define NL_GL_GLMATRIX_H

#include "NL/nlMath.h"

void glSetMatrix(unsigned long matrix, const nlMatrix4& m);
void glGetMatrix(unsigned long matrix, nlMatrix4& m);
u32 glAllocMatrix();

#endif // NL_GL_GLMATRIX_H
