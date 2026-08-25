#include <math.h>
#include <revolution/mtx.h>

extern f64 tan(f64);

void C_MTXFrustum(Mtx44 m, f32 t, f32 b, f32 l, f32 r, f32 n, f32 f)
{
    f32 tmp;

    tmp = 1.0f / (r - l);
    m[0][0] = 2.0f * n * tmp;
    m[0][1] = 0.0f;
    m[0][2] = tmp * (r + l);
    m[0][3] = 0.0f;
    tmp = 1.0f / (t - b);
    m[1][0] = 0.0f;
    m[1][1] = 2.0f * n * tmp;
    m[1][2] = tmp * (t + b);
    m[1][3] = 0.0f;
    m[2][0] = 0.0f;
    m[2][1] = 0.0f;
    tmp = 1.0f / (f - n);
    m[2][2] = -n * tmp;
    m[2][3] = tmp * -(f * n);
    m[3][0] = 0.0f;
    m[3][1] = 0.0f;
    m[3][2] = -1.0f;
    m[3][3] = 0.0f;
}

void C_MTXPerspective(Mtx44 m, f32 fovY, f32 aspect, f32 n, f32 f)
{
    f32 angle;
    f32 cot;
    f32 tmp;

    angle = fovY * 0.5f;
    angle = MTXDegToRad(angle);
    cot = tan(angle);
    cot = 1.0f / cot;

    m[0][0] = cot / aspect;
    m[0][1] = 0.0f;
    m[0][2] = 0.0f;
    m[0][3] = 0.0f;
    m[1][0] = 0.0f;
    m[1][1] = cot;
    m[1][2] = 0.0f;
    m[1][3] = 0.0f;
    m[2][0] = 0.0f;
    m[2][1] = 0.0f;
    tmp = 1.0f / (f - n);
    m[2][2] = -n * tmp;
    m[2][3] = tmp * -(f * n);
    m[3][0] = 0.0f;
    m[3][1] = 0.0f;
    m[3][2] = -1.0f;
    m[3][3] = 0.0f;
}

void C_MTXOrtho(Mtx44 m, f32 t, f32 b, f32 l, f32 r, f32 n, f32 f)
{
    f32 tmp;

    tmp = 1.0f / (r - l);
    m[0][0] = 2.0f * tmp;
    m[0][1] = 0.0f;
    m[0][2] = 0.0f;
    m[0][3] = tmp * -(r + l);
    tmp = 1.0f / (t - b);
    m[1][0] = 0.0f;
    m[1][1] = 2.0f * tmp;
    m[1][2] = 0.0f;
    m[1][3] = tmp * -(t + b);
    tmp = 1.0f / (f - n);
    m[2][0] = 0.0f;
    m[2][1] = 0.0f;
    m[2][2] = -1.0f * tmp;
    m[2][3] = -f * tmp;
    m[3][0] = 0.0f;
    m[3][1] = 0.0f;
    m[3][2] = 0.0f;
    m[3][3] = 1.0f;
}

// clang-format off
void PSMTX44Identity(register Mtx44 m)
{
    register f32 c1 = 1.0f;
    register f32 c0 = 0.0f;

    asm {
        stfs c1, 0(m)
        psq_st c0, 4(m), 0, 0
        psq_st c0, 12(m), 0, 0
        stfs c1, 20(m)
        psq_st c0, 24(m), 0, 0
        psq_st c0, 32(m), 0, 0
        stfs c1, 40(m)
        psq_st c0, 44(m), 0, 0
        psq_st c0, 52(m), 0, 0
        stfs c1, 60(m)
    }
}
// clang-format on

// clang-format off
asm void PSMTX44Copy(const register Mtx44 src, register Mtx44 dst)
{
    nofralloc
    psq_l f1, 0(src), 0, 0
    psq_st f1, 0(dst), 0, 0
    psq_l f1, 8(src), 0, 0
    psq_st f1, 8(dst), 0, 0
    psq_l f1, 16(src), 0, 0
    psq_st f1, 16(dst), 0, 0
    psq_l f1, 24(src), 0, 0
    psq_st f1, 24(dst), 0, 0
    psq_l f1, 32(src), 0, 0
    psq_st f1, 32(dst), 0, 0
    psq_l f1, 40(src), 0, 0
    psq_st f1, 40(dst), 0, 0
    psq_l f1, 48(src), 0, 0
    psq_st f1, 48(dst), 0, 0
    psq_l f1, 56(src), 0, 0
    psq_st f1, 56(dst), 0, 0
    blr
}
// clang-format on

// clang-format off
asm void PSMTX44Concat(const register Mtx44 a, const register Mtx44 b, register Mtx44 ab)
{
    nofralloc
    psq_l f0, 0(a), 0, 0
    psq_l f2, 0(b), 0, 0
    ps_muls0 f6, f2, f0
    psq_l f3, 16(b), 0, 0
    psq_l f4, 32(b), 0, 0
    ps_madds1 f6, f3, f0, f6
    psq_l f1, 8(a), 0, 0
    psq_l f5, 48(b), 0, 0
    ps_madds0 f6, f4, f1, f6
    psq_l f0, 16(a), 0, 0
    ps_madds1 f6, f5, f1, f6
    psq_l f1, 24(a), 0, 0
    ps_muls0 f8, f2, f0
    ps_madds1 f8, f3, f0, f8
    psq_l f0, 32(a), 0, 0
    ps_madds0 f8, f4, f1, f8
    ps_madds1 f8, f5, f1, f8
    psq_l f1, 40(a), 0, 0
    ps_muls0 f10, f2, f0
    ps_madds1 f10, f3, f0, f10
    psq_l f0, 48(a), 0, 0
    ps_madds0 f10, f4, f1, f10
    ps_madds1 f10, f5, f1, f10
    psq_l f1, 56(a), 0, 0
    ps_muls0 f12, f2, f0
    psq_l f2, 8(b), 0, 0
    ps_madds1 f12, f3, f0, f12
    psq_l f0, 0(a), 0, 0
    ps_madds0 f12, f4, f1, f12
    psq_l f3, 24(b), 0, 0
    ps_madds1 f12, f5, f1, f12
    psq_l f1, 8(a), 0, 0
    ps_muls0 f7, f2, f0
    psq_l f4, 40(b), 0, 0
    ps_madds1 f7, f3, f0, f7
    psq_l f5, 56(b), 0, 0
    ps_madds0 f7, f4, f1, f7
    psq_l f0, 16(a), 0, 0
    ps_madds1 f7, f5, f1, f7
    psq_l f1, 24(a), 0, 0
    ps_muls0 f9, f2, f0
    psq_st f6, 0(ab), 0, 0
    ps_madds1 f9, f3, f0, f9
    psq_l f0, 32(a), 0, 0
    ps_madds0 f9, f4, f1, f9
    psq_st f8, 16(ab), 0, 0
    ps_madds1 f9, f5, f1, f9
    psq_l f1, 40(a), 0, 0
    ps_muls0 f11, f2, f0
    psq_st f10, 32(ab), 0, 0
    ps_madds1 f11, f3, f0, f11
    psq_l f0, 48(a), 0, 0
    ps_madds0 f11, f4, f1, f11
    psq_st f12, 48(ab), 0, 0
    ps_madds1 f11, f5, f1, f11
    psq_l f1, 56(a), 0, 0
    ps_muls0 f13, f2, f0
    psq_st f7, 8(ab), 0, 0
    ps_madds1 f13, f3, f0, f13
    psq_st f9, 24(ab), 0, 0
    ps_madds0 f13, f4, f1, f13
    psq_st f11, 40(ab), 0, 0
    ps_madds1 f13, f5, f1, f13
    psq_st f13, 56(ab), 0, 0
    blr
}
// clang-format on

// clang-format off
asm void PSMTX44Transpose(const register Mtx44 src, register Mtx44 xPose)
{
    nofralloc
    psq_l f0, 0(src), 0, 0
    psq_l f1, 16(src), 0, 0
    ps_merge00 f4, f0, f1
    psq_l f2, 8(src), 0, 0
    psq_st f4, 0(xPose), 0, 0
    ps_merge11 f5, f0, f1
    psq_l f3, 24(src), 0, 0
    psq_st f5, 16(xPose), 0, 0
    ps_merge00 f4, f2, f3
    psq_l f0, 32(src), 0, 0
    psq_st f4, 32(xPose), 0, 0
    ps_merge11 f5, f2, f3
    psq_l f1, 48(src), 0, 0
    psq_st f5, 48(xPose), 0, 0
    ps_merge00 f4, f0, f1
    psq_l f2, 40(src), 0, 0
    psq_st f4, 8(xPose), 0, 0
    ps_merge11 f5, f0, f1
    psq_l f3, 56(src), 0, 0
    psq_st f5, 24(xPose), 0, 0
    ps_merge00 f4, f2, f3
    psq_st f4, 40(xPose), 0, 0
    ps_merge11 f5, f2, f3
    psq_st f5, 56(xPose), 0, 0
    blr
}
// clang-format on

#define SWAP(a, b) \
    {              \
        f32 tmp;   \
        tmp = a;   \
        a = b;     \
        b = tmp;   \
    }

u32 C_MTX44Inverse(const Mtx44 src, Mtx44 inv)
{
    Mtx44 gjm;
    s32 i;
    s32 j;
    s32 k;
    f32 w;
    f32 max;
    s32 swp;
    f32 ftmp;

    PSMTX44Copy(src, gjm);
    PSMTX44Identity(inv);

    for (i = 0; i < 4; i++)
    {
        max = 0.0f;
        swp = i;

        for (k = i; k < 4; k++)
        {
            ftmp = fabsf(gjm[k][i]);
            if (ftmp > max)
            {
                max = ftmp;
                swp = k;
            }
        }

        if (max == 0.0f)
        {
            return 0;
        }

        if (swp != i)
        {
            for (k = 0; k < 4; k++)
            {
                SWAP(gjm[i][k], gjm[swp][k]);
                SWAP(inv[i][k], inv[swp][k]);
            }
        }

        w = 1.0f / gjm[i][i];
        for (j = 0; j < 4; j++)
        {
            gjm[i][j] *= w;
            inv[i][j] *= w;
        }

        for (k = 0; k < 4; k++)
        {
            if (k != i)
            {
                w = gjm[k][i];
                for (j = 0; j < 4; j++)
                {
                    gjm[k][j] -= gjm[i][j] * w;
                    inv[k][j] -= inv[i][j] * w;
                }
            }
        }
    }

    return 1;
}

// clang-format off
void PSMTX44Scale(register Mtx44 m, register f32 xS, register f32 yS, register f32 zS)
{
    register f32 c_zero = 0.0f;
    register f32 c_one = 1.0f;

    asm {
        stfs xS, 0(m)
        psq_st c_zero, 4(m), 0, 0
        psq_st c_zero, 12(m), 0, 0
        stfs yS, 20(m)
        psq_st c_zero, 24(m), 0, 0
        psq_st c_zero, 32(m), 0, 0
        stfs zS, 40(m)
        psq_st c_zero, 44(m), 0, 0
        psq_st c_zero, 52(m), 0, 0
        stfs c_one, 60(m)
    }
}
// clang-format on
