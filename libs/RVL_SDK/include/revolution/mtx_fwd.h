#ifndef REVOLUTION_MTX_FWD_H
#define REVOLUTION_MTX_FWD_H

#ifdef __cplusplus
extern "C" {
#endif

void PSMTXIdentity(float matrix[3][4]);
void C_MTXPerspective(float matrix[4][4], float fovY, float aspect, float near, float far);
void C_MTXOrtho(float matrix[4][4], float top, float bottom, float left, float right, float near, float far);

#ifdef __cplusplus
}
#endif

#endif // REVOLUTION_MTX_FWD_H
