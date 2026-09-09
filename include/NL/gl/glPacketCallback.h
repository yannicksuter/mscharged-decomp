#ifndef NL_GL_GLPACKETCALLBACK_H
#define NL_GL_GLPACKETCALLBACK_H

#include "NL/gl/glView.h"

class PacketCallbackManager
{
public:
    PacketCallbackManager(GLView* view, GLViewPacketCallback cb);
    void DoCallback(const glModelPacket* p, unsigned long count);

    GLView* m_View;
    GLViewPacketCallback m_Cb;
    unsigned long m_LastRaster;
    unsigned long m_LastMatrix;
    void* m_LastState;
};

#endif // NL_GL_GLPACKETCALLBACK_H
