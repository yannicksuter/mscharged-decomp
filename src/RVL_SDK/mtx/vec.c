#include <revolution/mtx.h>

// clang-format off
void PSVECNormalize(const register Vec* src, register Vec* unit)
{
    register f32 c_half = 0.5f;
    register f32 c_three = 3.0f;
    register f32 v1_xy;
    register f32 v1_z;
    register f32 xx_zz;
    register f32 xx_yy;
    register f32 sqsum;
    register f32 rsqrt;
    register f32 nwork0;
    register f32 nwork1;

    asm {
        psq_l v1_xy, Vec.x(src), 0, 0
        ps_mul xx_yy, v1_xy, v1_xy
        psq_l v1_z, Vec.z(src), 1, 0
        ps_madd xx_zz, v1_z, v1_z, xx_yy
        ps_sum0 sqsum, xx_zz, v1_z, xx_yy
        frsqrte rsqrt, sqsum
        fmuls nwork0, rsqrt, rsqrt
        fmuls nwork1, rsqrt, c_half
        fnmsubs nwork0, nwork0, sqsum, c_three
        fmuls rsqrt, nwork0, nwork1
        ps_muls0 v1_xy, v1_xy, rsqrt
        psq_st v1_xy, Vec.x(unit), 0, 0
        ps_muls0 v1_z, v1_z, rsqrt
        psq_st v1_z, Vec.z(unit), 1, 0
    }
}
// clang-format on

// clang-format off
asm void PSVECCrossProduct(const register Vec* a, const register Vec* b, register Vec* axb)
{
    nofralloc
    psq_l f1, Vec.x(b), 0, 0
    lfs f2, Vec.z(a)
    psq_l f0, Vec.x(a), 0, 0
    ps_merge10 f6, f1, f1
    lfs f3, Vec.z(b)
    ps_mul f4, f1, f2
    ps_muls0 f7, f1, f0
    ps_msub f5, f0, f3, f4
    ps_msub f8, f0, f6, f7
    ps_merge11 f9, f5, f5
    ps_merge01 f10, f5, f8
    psq_st f9, Vec.x(axb), 1, 0
    ps_neg f10, f10
    psq_st f10, Vec.y(axb), 0, 0
    blr
}
// clang-format on
