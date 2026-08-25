#ifndef RVL_SDK_MTX_QUAT_H
#define RVL_SDK_MTX_QUAT_H
#include <revolution/types.h>

#include <revolution/mtx/mtxtypes.h>
#ifdef __cplusplus
extern "C" {
#endif

void PSQUATMultiply(const Quaternion* a, const Quaternion* b, Quaternion* prod);
void PSQUATScale(const Quaternion* q, Quaternion* r, f32 scale);
f32 PSQUATDotProduct(const Quaternion* p, const Quaternion* q);
void PSQUATNormalize(const Quaternion* in, Quaternion* out);
void C_QUATMtx(Quaternion* quat, const Mtx mtx);
void C_QUATSlerp(const Quaternion* a, const Quaternion* b, Quaternion* out,
                 f32 t);

#ifdef __cplusplus
}
#endif
#endif
