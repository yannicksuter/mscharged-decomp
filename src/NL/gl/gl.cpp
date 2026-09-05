#include "NL/gl/gl.h"
#include "NL/gl/glFont.h"

#include "NL/gl/glMatrix.h"
#include "NL/gl/glMemory.h"
#include "NL/gl/glPlat.h"
#include "NL/gl/glStat.h"
#include "NL/gl/glState.h"
#include "NL/gl/glStruct.h"
#include "NL/gl/glTarget.h"
#include "NL/gl/glView.h"
#include "NL/glx/GXMaterialProgramRegistry_802A0A14.h"
#include "NL/glx/glxLoadModel.h"
#include "NL/glx/glxTexture.h"
#include "NL/nlString.h"

static int gl_frameCounter;
static int gl_nDiscard;
static int gl_state;

bool fn_802C7FD0(void (*startupCallback)())
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
    gl_ViewStartup();

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

bool fn_802C80FC()
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

    gl_ViewReset();
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

void fn_802C8180()
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

void* fn_802C81FC(
    void* data, unsigned long size, unsigned long* pNumModels, void* context)
{
    return glplatEndLoadModel(data, size, pNumModels, context);
}

bool fn_802C8200(const char* filename,
    void (*callback)(void*, unsigned long, void*), void* userData,
    void* context)
{
    return glplatBeginLoadModel(filename, callback, userData);
}

bool fn_802C8204(const char* filename,
    void (*callback)(void*, unsigned long, void*), void* param,
    void* context)
{
    return glplatBeginLoadTextureBundle(filename, callback, param);
}

void* fn_802C8208(
    const char* filename, unsigned long* pNumModels, void* context)
{
    return glplatLoadModel(filename, pNumModels, context);
}

bool fn_802C820C(
    const char* filename, MemoryAllocator* allocator)
{
    return glplatLoadTextureBundle(filename, allocator);
}

float glGetOrthographicWidth()
{
    return fn_80369D5C();
}

float glGetOrthographicHeight()
{
    return fn_80369D64();
}

void fn_802C8280(const char*)
{
}

void fn_802C8284(unsigned long)
{
}

void fn_802C8288()
{
}

void fn_802C828C(const char* name, void* allocator)
{
}
