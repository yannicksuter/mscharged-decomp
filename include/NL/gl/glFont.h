#ifndef NL_GL_GLFONT_H
#define NL_GL_GLFONT_H

#include "NL/gl/gl.h"
#include "NL/nlColour.h"

class GLView;

void gl_FontStartup();
void glFontBegin(bool drop);
void glFontEnd();
void glFontVirtualPosToScreenCoordPos(float x, float y, float& outX, float& outY);
bool glFontVirtualCoordinates(bool virtualCoordinates);
int glFontSetFont(int font);
void fn_802C9A0C(int x, int y, char character, unsigned short* image,
    int imageWidth, int font);
int fn_802C9CC8(GLView* renderView);

int glFontPrint(void* renderView, eGLView view, int x, int y, const nlColour& colour, const char* str);
int glFontPrint(void* renderView, eGLView view, int x, int y, const char* str);
int glFontPrintf(void* renderView, int x, int y, const char* format, ...);
int glFontPrintf(void* renderView, int x, int y, const nlColour& colour, const char* format, ...);
int glFontPrintf(void* renderView, eGLView view, int x, int y, const nlColour& colour, const char* format, ...);

#endif // NL_GL_GLFONT_H
