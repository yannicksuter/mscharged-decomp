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
void nlMultVectorMatrix(nlVector4& out, const nlVector4& in, const nlMatrix4& m);
void nlMultPosVectorMatrix(nlVector3& result, const nlVector3& pos, const nlMatrix4& transformMatrix);
void nlMultVectorMatrix(nlVector2& v_out, const nlVector2& v_in, const nlMatrix3& m);
nlMatrix4& nlInvertMatrix(nlMatrix4& out, const nlMatrix4& in);
nlMatrix4& nlTransposeMatrix(nlMatrix4& out, const nlMatrix4& in);
nlMatrix4& nlMultMatrices(nlMatrix4& out, const nlMatrix4& a, const nlMatrix4& b);

#endif // _PLATVMATH_H_
