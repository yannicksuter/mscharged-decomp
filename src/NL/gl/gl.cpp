#include "NL/gl/gl.h"
#include "NL/gl/glFont.h"

#include "NL/gl/glMatrix.h"
#include "NL/gl/glPlat.h"
#include "NL/gl/glStat.h"
#include "NL/gl/glState.h"
#include "NL/gl/glStruct.h"
#include "NL/gl/glTarget.h"
#include "NL/gl/glView.h"
#include "NL/glx/glxLoadModel.h"
#include "NL/nlString.h"

extern "C" void fn_802A0A14();
extern "C" void fn_802CBEC8();
extern "C" void fn_802CEC68();
extern "C" void fn_802CEF18();
extern "C" unsigned long fn_80369D5C();
extern "C" unsigned long fn_80369D64();

extern "C" bool fn_8036B518(const char*, void (*)(void*, unsigned long, void*), void*);
extern "C" void* fn_8036B558(const char*, void*);

static int gl_frameCounter;
static int gl_nDiscard;
static int gl_state;

extern "C" bool fn_802C7FD0(void (*startupCallback)())
{
    gl_frameCounter = 0;
    gl_nDiscard = 0;
    gl_state = 0;

    fn_802CBEC8();
    startupCallback();
    fn_802A0A14();

    if (!glplatStartup(glGetScreenInfo()))
        return false;

    gl_StatStartup();
    gl_StateStartup();
    for (int i = 0; i < GLTT_Num; ++i)
        glSetCurrentTexture(-1, (eGLTextureType)i);

    glSetRasterStateDefaults();
    glSetCurrentRasterState(glHandleizeRasterState());
    glSetTextureStateDefaults();
    glSetCurrentTextureState(glHandleizeTextureState());
    gl_MatrixStartup();
    gl_TargetStartup();
    fn_802CEF18();

    if (!glplatPostStartup())
        return false;

    gl_FontStartup();
    return true;
}

unsigned long glHash(const char* string)
{
    return nlStringHash(string);
}

int glGetCurrentFrame()
{
    return gl_frameCounter;
}

bool glHasQuads()
{
    return true;
}

void glBeginFrame()
{
    glplatBeginFrame();
    gl_state = 1;
}

void glEndFrame()
{
    glplatEndFrame();
    gl_state = 2;
}

extern "C" bool fn_802C80FC()
{
    return gl_state == 1;
}

void glSendFrame()
{
    if (gl_nDiscard > 0)
    {
        glplatAbortFrame();
        gl_nDiscard -= 1;
    }
    else
    {
        glplatSendFrame();
    }

    fn_802CEC68();
    gl_state = 0;
    gl_frameCounter += 1;
}

void glDiscardFrame(int nFrames)
{
    if (nFrames > gl_nDiscard)
        gl_nDiscard = nFrames;
}

void glFinish()
{
    glplatFinish();
}

extern "C" void fn_802C8180()
{
    glViewCompact();
}

unsigned long glGetNumTriangles(eGLPrimitive primitive, unsigned long count)
{
    switch (primitive)
    {
    case GLP_TriList:
        return count / 3;
    case GLP_TriStrip:
        return count - 2;
    case GLP_TriFan:
        return count - 2;
    case GLP_QuadList:
        return count / 2;
    case GLP_LineList:
        return count / 2;
    case GLP_LineStrip:
        return count - 1;
    default:
        return 0;
    }
}

extern "C" void* fn_802C81FC(
    void* data, unsigned long size, unsigned long* pNumModels, void* context)
{
    return glplatEndLoadModel(data, size, pNumModels, context);
}

extern "C" bool fn_802C8200(const char* filename, void (*callback)(void*, unsigned long, void*), void* userData)
{
    return glplatBeginLoadModel(filename, callback, userData);
}

extern "C" bool fn_802C8204(const char* filename, void (*callback)(void*, unsigned long, void*), void* param)
{
    return fn_8036B518(filename, callback, param);
}

extern "C" void* fn_802C8208(
    const char* filename, unsigned long* pNumModels, void* context)
{
    return glplatLoadModel(filename, pNumModels, context);
}

extern "C" void* fn_802C820C(const char* filename, void* arg1)
{
    return fn_8036B558(filename, arg1);
}

float glGetOrthographicWidth()
{
    return fn_80369D5C();
}

float glGetOrthographicHeight()
{
    return fn_80369D64();
}

extern "C" void fn_802C8280(const char*)
{
}

extern "C" void fn_802C8284()
{
}

extern "C" void fn_802C8288()
{
}

extern "C" void fn_802C828C()
{
}
