#ifndef NL_GLX_GLXSEND_H
#define NL_GLX_GLXSEND_H

class GLView;
struct glModelPacket;

void glx_SendFrame_cb(
    GLView* view, unsigned long flags, const glModelPacket* p);
void glx_SendEnd();

#endif // NL_GLX_GLXSEND_H
