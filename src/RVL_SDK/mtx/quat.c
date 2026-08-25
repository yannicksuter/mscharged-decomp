#include <math.h>
#include <revolution/mtx.h>

// clang-format off
void PSQUATScale(const register Quaternion* q, register Quaternion* r, register f32 scale)
{
    register f32 rxy;
    register f32 rzw;

    asm {
        psq_l rxy, Quaternion.x(q), 0, 0
        psq_l rzw, Quaternion.z(q), 0, 0
        ps_muls0 rxy, rxy, scale
        psq_st rxy, Quaternion.x(r), 0, 0
        ps_muls0 rzw, rzw, scale
        psq_st rzw, Quaternion.z(r), 0, 0
    }
}
// clang-format on

// clang-format off
f32 PSQUATDotProduct(const register Quaternion* p, const register Quaternion* q)
{
    register f32 pxy;
    register f32 pzw;
    register f32 qxy;
    register f32 qzw;
    register f32 dp;

    asm {
        psq_l pxy, Quaternion.x(p), 0, 0
        psq_l qxy, Quaternion.x(q), 0, 0
        ps_mul dp, pxy, qxy
        psq_l pzw, Quaternion.z(p), 0, 0
        psq_l qzw, Quaternion.z(q), 0, 0
        ps_madd dp, pzw, qzw, dp
        ps_sum0 dp, dp, dp, dp
    }

    return dp;
}
// clang-format on

#pragma fp_contract off
void C_QUATSlerp(const Quaternion* p, const Quaternion* q, Quaternion* r, f32 t)
{
    f32 theta;
    f32 sin_th;
    f32 cos_th;
    f32 tp;
    f32 tq;

    cos_th = p->x * q->x + p->y * q->y + p->z * q->z + p->w * q->w;
    tq = 1.0f;

    if (cos_th < 0.0f)
    {
        cos_th = -cos_th;
        tq = -tq;
    }

    if (cos_th <= 0.99999f)
    {
        theta = acosf(cos_th);
        sin_th = sinf(theta);
        tp = sinf((1.0f - t) * theta) / sin_th;
        tq *= sinf(t * theta) / sin_th;
    }
    else
    {
        tp = 1.0f - t;
        tq *= t;
    }

    r->x = (tp * p->x) + (tq * q->x);
    r->y = (tp * p->y) + (tq * q->y);
    r->z = (tp * p->z) + (tq * q->z);
    r->w = (tp * p->w) + (tq * q->w);
}
#pragma fp_contract on
