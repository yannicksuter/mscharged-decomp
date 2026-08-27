#include "NL/gl/glMatrix.h"

#include "NL/glx/glxMatrix.h"
#include "NL/glx/glxMemory.h"

#include <string.h>

extern "C" void* fn_802CC094();
extern "C" void* fn_802CC0A4(unsigned long size, int memoryType, void* allocator);

static unsigned long gl_IdentityMatrix = 0xFFFFFFFF;

void GLMatrix::Get(nlMatrix4& m) const
{
    memcpy(&m, &matrix, sizeof(nlMatrix4));
}

void GLMatrix::Set(const nlMatrix4& m)
{
    memcpy(&matrix, &m, sizeof(nlMatrix4));
}

void gl_MatrixStartup()
{
    nlMatrix4 m;
    m.SetIdentity();
    gl_IdentityMatrix = (unsigned long)fn_802CC0A4(sizeof(nlMatrix4), GLM_Matrix, fn_802CC094());
    glplatSetMatrix(gl_IdentityMatrix, m);
}

unsigned long glGetIdentityMatrix()
{
    return gl_IdentityMatrix;
}

u32 glAllocMatrix()
{
    u32 p = (u32)glplatFrameAlloc(sizeof(nlMatrix4), GLM_Matrix);
    if (p == 0U)
    {
        p = -1U;
    }
    return p;
}

void glGetMatrix(unsigned long matrix, nlMatrix4& m)
{
    glplatGetMatrix(matrix, m);
}

void glSetMatrix(unsigned long matrix, const nlMatrix4& m)
{
    glplatSetMatrix(matrix, m);
}

void glMatrixOrthographic(nlMatrix4& m, float width, float height)
{
    glplatMatrixOrthographic(m, width, height);
}

void glMatrixOrthographicCentered(nlMatrix4& m, float width, float height, float nearPlane, float farPlane)
{
    glplatMatrixOrthographicCentered(m, width, height, nearPlane, farPlane);
}

void glMatrixPerspective(nlMatrix4& m, float fovRad, float aspect, float nearPlane, float farPlane)
{
    glplatMatrixPerspective(m, fovRad, aspect, nearPlane, farPlane);
}

void glMatrixLookAt(nlMatrix4& m, const nlVector3& peye, const nlVector3& pat, const nlVector3& vup)
{
    glplatMatrixLookAt(m, peye, pat, vup);
}
