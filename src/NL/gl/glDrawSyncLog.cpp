#include "NL/gl/glDrawSyncLog.h"

#include "Game/UnidentifiedStaticStorage.h"

extern "C" GLDrawSyncLog* glGetDrawSyncLog()
{
    static GLDrawSyncLog instance;
    return &instance;
}

GLDrawSyncLog::~GLDrawSyncLog()
{
}

void GLDrawSyncLog::EndFrame()
{
}

void GLDrawSyncLog::AddMarker(const char*)
{
}

void GLDrawSyncLog::SetCurrentView(const char*)
{
}

const char* GLDrawSyncLog::GetViewName(unsigned short)
{
    return "";
}

const char* GLDrawSyncLog::GetMarkerName(unsigned short)
{
    return "";
}
