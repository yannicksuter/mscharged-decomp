#ifndef NL_GL_GL_DRAW_SYNC_LOG_H
#define NL_GL_GL_DRAW_SYNC_LOG_H

class GLDrawSyncLog
{
public:
    GLDrawSyncLog()
        : m_UnknownStateA(0)
        , m_UnknownStateB(0)
        , m_UnknownStateC(0)
    {
    }

    virtual ~GLDrawSyncLog();

    void EndFrame();
    void AddMarker(const char*);
    void SetCurrentView(const char*);
    const char* GetViewName(unsigned short);
    const char* GetMarkerName(unsigned short);

    unsigned char m_UnknownStorage[8];
    unsigned int m_UnknownStateA;
    unsigned int m_UnknownStateB;
    unsigned int m_UnknownStateC;
};

extern "C" GLDrawSyncLog* glGetDrawSyncLog();

#endif // NL_GL_GL_DRAW_SYNC_LOG_H
