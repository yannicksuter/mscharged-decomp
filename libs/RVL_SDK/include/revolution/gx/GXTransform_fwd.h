#ifndef RVL_SDK_GX_TRANSFORM_FWD_H
#define RVL_SDK_GX_TRANSFORM_FWD_H

#include <revolution/gx/GXProjectionTypes.h>

#ifdef __cplusplus
extern "C" {
#endif

void GXSetProjection(const float projection[4][4], GXProjectionType type);
void GXSetProjectionv(const float projection[7]);
void GXGetProjectionv(float projection[7]);
void GXGetViewportv(float viewport[6]);
void GXProject(float x, float y, float z, const float matrix[3][4],
    const float projection[7], const float viewport[6],
    float* screenX, float* screenY, float* screenZ);

#ifdef __cplusplus
}
#endif

#endif // RVL_SDK_GX_TRANSFORM_FWD_H
