#ifndef NL_GL_GLMATRIX_H
#define NL_GL_GLMATRIX_H

#include "NL/nlMath.h"

void glMatrixLookAt(nlMatrix4& m, const nlVector3& peye, const nlVector3& pat, const nlVector3& vup);
void glMatrixPerspective(nlMatrix4& m, float fovRad, float aspect, float nearPlane, float farPlane);
void glMatrixOrthographicCentered(nlMatrix4& m, float width, float height, float nearPlane, float farPlane);
void glMatrixOrthographic(nlMatrix4& m, float width, float height);
void glSetMatrix(unsigned long matrix, const nlMatrix4& m);
void glGetMatrix(unsigned long matrix, nlMatrix4& m);
u32 glAllocMatrix();
unsigned long glGetIdentityMatrix();
void gl_MatrixStartup();

class GLMatrix
{
public:
    void Set(const nlMatrix4& m);
    void Get(nlMatrix4& m) const;

    nlMatrix4 matrix;
};

#endif // NL_GL_GLMATRIX_H
