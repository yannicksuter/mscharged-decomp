#ifndef NL_GLX_GLXMATRIX_H
#define NL_GLX_GLXMATRIX_H

#include "NL/nlMath.h"

void glplatMatrixLookAt(nlMatrix4& m, const nlVector3& eye, const nlVector3& at, const nlVector3& up);
void glplatMatrixPerspective(nlMatrix4& matrix, float fovY, float aspect, float nearPlane, float farPlane);
void glplatMatrixOrthographicCentered(nlMatrix4& matrix, float width, float height, float nearPlane, float farPlane);
void glplatMatrixOrthographic(nlMatrix4& matrix, float width, float height);

#endif // NL_GLX_GLXMATRIX_H
