#ifndef NL_GLX_GLXSEND_H
#define NL_GLX_GLXSEND_H

class GLView;
struct glModelPacket;

void glx_SendFrame_cb(
    GLView* view, unsigned long flags, const glModelPacket* p);
void glx_SendEnd();
void glx_Fog(bool enable);
void glx_SetFogClipPlanes(float nearPlane, float farPlane);
float glx_GetFogStart();
void glx_SetFogStart(float value);
float glx_GetFogEnd();
void glx_SetFogEnd(float value);

#endif // NL_GLX_GLXSEND_H
