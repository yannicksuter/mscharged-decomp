#include "NL/glx/glxMatrix.h"

#include <stddef.h>

extern "C"
{
    void* memcpy(void* dest, const void* src, size_t num);
    void C_MTXPerspective(float m[4][4], float fovY, float aspect, float near, float far);
    void C_MTXOrtho(float m[4][4], float top, float bottom, float left, float right, float near, float far);
    void fn_802B5D74(nlVector3& out, const nlVector3& point, const nlVector4& plane);
}

void glplatMatrixOrthographic(nlMatrix4& matrix, float width, float height)
{
    static float fNear = 0.0f;
    static float fFar = 16777215.0f;
    C_MTXOrtho(matrix.e2, 0.0f, height, 0.0f, width, fNear, fFar);
}

void glplatMatrixOrthographicCentered(nlMatrix4& matrix, float width, float height, float near, float far)
{
    float half = 0.5f;
    C_MTXOrtho(matrix.e2, height * half, -height * half, -width * half, width * half, near, far);
}

void glplatMatrixPerspective(nlMatrix4& matrix, float fovY, float aspect, float near, float far)
{
    C_MTXPerspective(matrix.e2, (180.0f * fovY) / 3.1415927f, aspect, near, far);
}

void glplatMatrixLookAt(nlMatrix4& m, const nlVector3& eye, const nlVector3& at, const nlVector3& up)
{
    nlVector3 view;
    nlVector3 side;
    nlVector3 cameraUp;
    nlVector3 cameraEye;
    float y;
    float x;
    float z;
    y = eye.y - at.y;
    x = eye.x - at.x;
    z = eye.z - at.z;
    cameraEye = eye;
    nlVec3Set(view, x, y, z);
    float length = nlSqrt(nlVec3LengthSquared(view), true);
    nlVec3Scale(view, view, 1.0f / length);
    nlVec3CrossProduct(side, up, view);
    nlVec3Scale(side, side, nlRecipSqrt(nlVec3LengthSquared(side), true));
    nlVec3CrossProduct(cameraUp, view, side);

    if (nlAbs(up.z) > 0.5f && (nlAbs(up.x) > 0.001f || nlAbs(up.y) > 0.001f))
    {
        nlVector4 sidePlane;
        nlVec4Set(sidePlane, side.x, side.y, side.z, 0.0f);

        nlVector3 projectedUp;
        fn_802B5D74(projectedUp, up, sidePlane);
        if (nlAbs(projectedUp.x) > 0.001f || nlAbs(projectedUp.y) > 0.001f)
        {
            nlQuaternion rotation;
            const nlVector3 fallbackAxis = { 0.0f, 0.0f, 1.0f };
            GetRotationBetweenVectors(rotation, fallbackAxis, projectedUp);
            RotateVector(cameraUp, cameraUp, rotation);
            nlVec3CrossProduct(view, side, cameraUp);
            nlVec3ScaleAdd(cameraEye, length, view, at);
        }
    }

    m.e2[0][0] = side.x;
    m.e2[1][0] = side.y;
    m.e2[2][0] = side.z;
    m.e2[3][0] = -nlVec3DotProduct(side, cameraEye);
    m.e2[0][1] = cameraUp.x;
    m.e2[1][1] = cameraUp.y;
    m.e2[2][1] = cameraUp.z;
    m.e2[3][1] = -nlVec3DotProduct(cameraUp, cameraEye);
    m.e2[0][2] = view.x;
    m.e2[1][2] = view.y;
    m.e2[2][2] = view.z;
    m.e2[3][2] = -nlVec3DotProduct(view, cameraEye);
    m.e2[0][3] = 0.0f;
    m.e2[1][3] = 0.0f;
    m.e2[2][3] = 0.0f;
    m.e2[3][3] = 1.0f;
}

void glxCopyMatrix(float (&mo)[4][4], const nlMatrix4& mi)
{
    memcpy(mo, mi.e2, sizeof(mi.e2));
}

void glxCopyMatrix(float (&mo)[3][4], const nlMatrix4& mi)
{
    mo[0][0] = mi.e2[0][0];
    mo[0][1] = mi.e2[1][0];
    mo[0][2] = mi.e2[2][0];
    mo[0][3] = mi.e2[3][0];
    mo[1][0] = mi.e2[0][1];
    mo[1][1] = mi.e2[1][1];
    mo[1][2] = mi.e2[2][1];
    mo[1][3] = mi.e2[3][1];
    mo[2][0] = mi.e2[0][2];
    mo[2][1] = mi.e2[1][2];
    mo[2][2] = mi.e2[2][2];
    mo[2][3] = mi.e2[3][2];
}

void glxCopyMatrix(nlMatrix4& mo, const float (&mi)[3][4])
{
    mo.e2[0][0] = mi[0][0];
    mo.e2[1][0] = mi[0][1];
    mo.e2[2][0] = mi[0][2];
    mo.e2[3][0] = mi[0][3];
    mo.e2[0][1] = mi[1][0];
    mo.e2[1][1] = mi[1][1];
    mo.e2[2][1] = mi[1][2];
    mo.e2[3][1] = mi[1][3];
    mo.e2[0][2] = mi[2][0];
    mo.e2[1][2] = mi[2][1];
    mo.e2[2][2] = mi[2][2];
    mo.e2[3][2] = mi[2][3];
    mo.e2[0][3] = 0.0f;
    mo.e2[1][3] = 0.0f;
    mo.e2[2][3] = 0.0f;
    mo.e2[3][3] = 1.0f;
}
