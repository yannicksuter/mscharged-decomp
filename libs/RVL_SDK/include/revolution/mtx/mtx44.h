#ifndef RVL_SDK_MTX_MTX44_H
#define RVL_SDK_MTX_MTX44_H
#include <revolution/types.h>

#include <revolution/mtx/mtxtypes.h>
#ifdef __cplusplus
extern "C" {
#endif

void C_MTXFrustum(Mtx44 mtx, f32 t, f32 b, f32 l, f32 r, f32 n, f32 f);
void C_MTXPerspective(Mtx44 mtx, f32 fovy, f32 aspect, f32 n, f32 f);
void C_MTXOrtho(Mtx44 mtx, f32 t, f32 b, f32 l, f32 r, f32 n, f32 f);
void PSMTX44Identity(Mtx44 mtx);
void PSMTX44Copy(const Mtx44 src, Mtx44 dst);
void PSMTX44Concat(const Mtx44 a, const Mtx44 b, Mtx44 ab);
void PSMTX44Transpose(const Mtx44 src, Mtx44 xPose);
u32 C_MTX44Inverse(const Mtx44 src, Mtx44 inv);
void PSMTX44Scale(Mtx44 m, f32 xS, f32 yS, f32 zS);

#ifdef __cplusplus
}
#endif
#endif
