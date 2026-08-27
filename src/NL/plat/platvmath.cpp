#include "NL/platvmath.h"

#define qr0 0

#define RAD_TO_FIXED16 10430.378f

extern "C"
{
    void PSMTX44Identity(float m[4][4]);
    void PSMTX44Concat(const float a[4][4], const float b[4][4], float out[4][4]);
    void PSMTX44Transpose(const float src[4][4], float out[4][4]);
    u32 C_MTX44Inverse(const float src[4][4], float out[4][4]);
    void PSMTX44Scale(float m[4][4], float sx, float sy, float sz);
}

void nlMatrix4::SetIdentity()
{
    PSMTX44Identity(e2);
}

nlMatrix4& nlMultMatrices(nlMatrix4& out, const nlMatrix4& a, const nlMatrix4& b)
{
    PSMTX44Concat(a.e2, b.e2, out.e2);
    return out;
}

nlMatrix4& nlTransposeMatrix(nlMatrix4& out, const nlMatrix4& in)
{
    PSMTX44Transpose(in.e2, out.e2);
    return out;
}

nlMatrix4& nlInvertMatrix(nlMatrix4& out, const nlMatrix4& in)
{
    C_MTX44Inverse(in.e2, out.e2);
    return out;
}

void nlMultVectorMatrix(nlVector2& out, const nlVector2& in, const nlMatrix3& m)
{
    out.x = m.e[0] * in.x + m.e[3] * in.y + m.e[6];
    out.y = m.e[1] * in.x + m.e[4] * in.y + m.e[7];
}

// clang-format off
void nlMultPosVectorMatrix(register nlVector3& result, register const nlVector3& pos, register const nlMatrix4& transformMatrix)
{
    asm {
        psq_l f2, 0x0(transformMatrix), 0, qr0
        psq_l f0, 0x0(pos), 0, qr0
        psq_l f3, 0x8(transformMatrix), 0, qr0
        ps_muls0 f10, f2, f0
        psq_l f4, 0x10(transformMatrix), 0, qr0
        ps_muls0 f11, f3, f0
        psq_l f5, 0x18(transformMatrix), 0, qr0
        psq_l f6, 0x20(transformMatrix), 0, qr0
        ps_madds1 f10, f4, f0, f10
        psq_l f1, 0x8(pos), 1, qr0
        ps_madds1 f11, f5, f0, f11
        psq_l f7, 0x28(transformMatrix), 0, qr0
        ps_madds0 f10, f6, f1, f10
        psq_l f8, 0x30(transformMatrix), 0, qr0
        ps_madds0 f11, f7, f1, f11
        psq_l f9, 0x38(transformMatrix), 0, qr0
        ps_add f10, f8, f10
        ps_add f11, f9, f11
        psq_st f10, 0x0(result), 0, qr0
        psq_st f11, 0x8(result), 1, qr0
    }
}
// clang-format on

void nlMultVectorMatrix(nlVector4& out, const nlVector4& in, const nlMatrix4& m)
{
    out.x = m.e2[0][0] * in.x + m.e2[1][0] * in.y + m.e2[2][0] * in.z + m.e2[3][0] * in.w;
    out.y = m.e2[0][1] * in.x + m.e2[1][1] * in.y + m.e2[2][1] * in.z + m.e2[3][1] * in.w;
    out.z = m.e2[0][2] * in.x + m.e2[1][2] * in.y + m.e2[2][2] * in.z + m.e2[3][2] * in.w;
    out.w = m.e2[0][3] * in.x + m.e2[1][3] * in.y + m.e2[2][3] * in.z + m.e2[3][3] * in.w;
}

// clang-format off
void nlMultDirVectorMatrix(register nlVector3& result, register const nlVector3& direction, register const nlMatrix4& transformMatrix)
{
    asm {
        psq_l f2, 0x0(transformMatrix), 0, qr0
        psq_l f0, 0x0(direction), 0, qr0
        psq_l f3, 0x8(transformMatrix), 0, qr0
        ps_muls0 f10, f2, f0
        psq_l f4, 0x10(transformMatrix), 0, qr0
        ps_muls0 f11, f3, f0
        psq_l f5, 0x18(transformMatrix), 0, qr0
        psq_l f6, 0x20(transformMatrix), 0, qr0
        ps_madds1 f10, f4, f0, f10
        psq_l f1, 0x8(direction), 1, qr0
        ps_madds1 f11, f5, f0, f11
        psq_l f7, 0x28(transformMatrix), 0, qr0
        ps_madds0 f10, f6, f1, f10
        ps_madds0 f11, f7, f1, f11
        psq_st f10, 0x0(result), 0, qr0
        psq_st f11, 0x8(result), 1, qr0
    }
}
// clang-format on

nlMatrix4& nlMakeRotationMatrixX(nlMatrix4& out, float theta)
{
    f32 sn;
    f32 cs;

    nlSinCos(&sn, &cs, (short)(RAD_TO_FIXED16 * theta));
    PSMTX44Identity(out.e2);
    out.e2[1][1] = cs;
    out.e2[1][2] = sn;
    out.e2[2][1] = -out.e2[1][2];
    out.e2[2][2] = out.e2[1][1];
    return out;
}

nlMatrix4& nlMakeRotationMatrixY(nlMatrix4& out, float theta)
{
    f32 sn;
    f32 cs;

    nlSinCos(&sn, &cs, (short)(RAD_TO_FIXED16 * theta));
    PSMTX44Identity(out.e2);
    out.e2[0][0] = cs;
    out.e2[0][2] = -sn;
    out.e2[2][0] = -out.e2[0][2];
    out.e2[2][2] = out.e2[0][0];
    return out;
}

nlMatrix4& nlMakeRotationMatrixZ(nlMatrix4& out, float theta)
{
    f32 sn;
    f32 cs;

    nlSinCos(&sn, &cs, (short)(RAD_TO_FIXED16 * theta));
    PSMTX44Identity(out.e2);
    out.e2[0][0] = cs;
    out.e2[0][1] = sn;
    out.e2[1][0] = -out.e2[0][1];
    out.e2[1][1] = out.e2[0][0];
    return out;
}

nlMatrix3& nlMakeRotationMatrixZ(nlMatrix3& out, float theta)
{
    f32 sn;
    f32 cs;

    nlSinCos(&sn, &cs, (short)(RAD_TO_FIXED16 * theta));
    out.SetIdentity();
    out.e[0] = cs;
    out.e[1] = sn;
    out.e[3] = -out.e[1];
    out.e[4] = out.e[0];
    return out;
}

nlMatrix4& nlMakeRotationMatrixEulerAngles(nlMatrix4& m, float pitch, float yaw, float roll)
{
    f32 sinRoll;
    f32 cosRoll;
    f32 sinYaw;
    f32 cosYaw;
    f32 sinPitch;
    f32 cosPitch;

    nlSinCos(&sinRoll, &cosRoll, (short)(RAD_TO_FIXED16 * roll));
    nlSinCos(&sinYaw, &cosYaw, (short)(RAD_TO_FIXED16 * yaw));
    nlSinCos(&sinPitch, &cosPitch, (short)(RAD_TO_FIXED16 * pitch));

    m.e2[2][3] = 0.0f;
    m.e2[1][3] = 0.0f;
    m.e2[0][3] = 0.0f;
    m.e2[3][2] = 0.0f;
    m.e2[3][1] = 0.0f;
    m.e2[3][0] = 0.0f;
    m.e2[3][3] = 1.0f;
    m.e2[0][0] = cosYaw * cosRoll;
    m.e2[0][1] = cosYaw * sinRoll;
    m.e2[0][2] = -sinYaw;
    m.e2[1][0] = (cosRoll * (sinPitch * sinYaw)) - (cosPitch * sinRoll);
    m.e2[1][1] = (sinRoll * (sinPitch * sinYaw)) + (cosPitch * cosRoll);
    m.e2[1][2] = cosYaw * sinPitch;
    m.e2[2][0] = (cosRoll * (cosPitch * sinYaw)) + (sinPitch * sinRoll);
    m.e2[2][1] = (sinRoll * (cosPitch * sinYaw)) - (sinPitch * cosRoll);
    m.e2[2][2] = cosYaw * cosPitch;
    return m;
}

nlMatrix4& nlMakeScaleMatrix(nlMatrix4& m, float sx, float sy, float sz)
{
    PSMTX44Scale(m.e2, sx, sy, sz);
    return m;
}

nlMatrix4& nlMakeTranslationMatrix(nlMatrix4& m, float x, float y, float z)
{
    m.SetIdentity();
    m.e2[3][0] = x;
    m.e2[3][1] = y;
    m.e2[3][2] = z;
    return m;
}
