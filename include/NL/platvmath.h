#ifndef _PLATVMATH_H_
#define _PLATVMATH_H_

#include "NL/nlMath.h"

nlMatrix4& nlMakeScaleMatrix(nlMatrix4& m, float sx, float sy, float sz);
nlMatrix4& nlMakeTranslationMatrix(nlMatrix4& m, float x, float y, float z);
nlMatrix4& nlMakeRotationMatrixEulerAngles(nlMatrix4& m, float pitch, float yaw, float roll);
nlMatrix3& nlMakeRotationMatrixZ(nlMatrix3& out, float theta);
nlMatrix4& nlMakeRotationMatrixZ(nlMatrix4& out, float theta);
nlMatrix4& nlMakeRotationMatrixY(nlMatrix4& out, float theta);
nlMatrix4& nlMakeRotationMatrixX(nlMatrix4& out, float theta);
void nlMultDirVectorMatrix(nlVector3& result, const nlVector3& direction, const nlMatrix4& transformMatrix);
inline void nlMultDirVectorMatrix(nlVector3& direction, const nlMatrix4& transformMatrix)
{
    nlVector3 result;
    nlMultDirVectorMatrix(result, direction, transformMatrix);
    direction = result;
}
void nlMultVectorMatrix(nlVector4& out, const nlVector4& in, const nlMatrix4& m);
inline void nlMultVectorMatrix(nlVector4& v, const nlMatrix4& m)
{
    nlVector4 result;
    nlMultVectorMatrix(result, v, m);
    v = result;
}
void nlMultPosVectorMatrix(nlVector3& result, const nlVector3& pos, const nlMatrix4& transformMatrix);
inline void nlMultPosVectorMatrix(nlVector3& pos, const nlMatrix4& transformMatrix)
{
    nlVector3 result;
    nlMultPosVectorMatrix(result, pos, transformMatrix);
    pos = result;
}
void nlMultVectorMatrix(nlVector2& v_out, const nlVector2& v_in, const nlMatrix3& m);
nlMatrix4& nlInvertMatrix(nlMatrix4& out, const nlMatrix4& in);
nlMatrix4& nlTransposeMatrix(nlMatrix4& out, const nlMatrix4& in);
nlMatrix4& nlMultMatrices(nlMatrix4& out, const nlMatrix4& a, const nlMatrix4& b);
inline void nlMultMatrices(nlMatrix4& a, const nlMatrix4& b)
{
    nlMatrix4 result;
    nlMultMatrices(result, a, b);
    a = result;
}

inline void nlMultVectorMatrix(nlVector3& v_out, const nlVector3& v_in, const nlMatrix3& m)
{
    nlVector3 t;
    t.x = m.e2[0][0] * v_in.x + m.e2[1][0] * v_in.y + m.e2[2][0] * v_in.z;
    t.y = m.e2[0][1] * v_in.x + m.e2[1][1] * v_in.y + m.e2[2][1] * v_in.z;
    t.z = m.e2[0][2] * v_in.x + m.e2[1][2] * v_in.y + m.e2[2][2] * v_in.z;
    v_out = t;
}

#endif // _PLATVMATH_H_
