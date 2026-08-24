#ifndef _GLSTATE_H_
#define _GLSTATE_H_

#include "NL/gl/glStateBundle.h"
#include "types.h"

enum eGLState
{
    GLS_DepthTest = 0,
    GLS_DepthWrite = 1,
    GLS_DepthFunc = 2,
    GLS_AlphaTest = 3,
    GLS_AlphaTestRef = 4,
    GLS_AlphaBlend = 5,
    GLS_Culling = 6,
    GLS_ColourWrite = 7,
    GLS_SolidOffset = 8,
    GLS_FillMode = 9,
    // Six Wii-only raster states. The stripped DOL preserves their packed
    // widths but not their names, so they stay index-named.
    GLS_State10 = 10,
    GLS_State11 = 11,
    GLS_State12 = 12,
    GLS_State13 = 13,
    GLS_State14 = 14,
    GLS_State15 = 15,
    GLS_Num = 16,
};

enum eGLTextureType
{
    GLTT_Diffuse = 0,
    GLTT_Detail = 1,
    GLTT_Shadow = 2,
    GLTT_SelfIllum = 3,
    GLTT_Gloss = 4,
    GLTT_BumpLocal = 5,
    GLTT_Num = 6,
};

enum eGLTextureState
{
    GLTS_DiffuseWrap = 0,
    GLTS_DetailWrap = 1,
    GLTS_ShadowWrap = 2,
    GLTS_SelfIllumWrap = 3,
    GLTS_GlossWrap = 4,
    GLTS_BumpLocalWrap = 5,
    GLTS_DiffuseFilter = 6,
    GLTS_DetailFilter = 7,
    GLTS_ShadowFilter = 8,
    GLTS_SelfIllumFilter = 9,
    GLTS_GlossFilter = 10,
    GLTS_BumpLocalFilter = 11,
    GLTS_DiffuseLevel = 12,
    GLTS_DetailLevel = 13,
    GLTS_ShadowLevel = 14,
    GLTS_SelfIllumLevel = 15,
    GLTS_GlossLevel = 16,
    GLTS_BumpLocalLevel = 17,
    GLTS_Num = 18,
};

void gl_StateStartup();
glStateBundle* gl_GetCurrentStateBundle();
void glStateSave(glStateBundle& state);
void glStateRestore(const glStateBundle& state);

u32 glGetCurrentTexture(eGLTextureType type);
u32 glGetTexture(const char* textureName);
u32 glPackTextureLevel(float level);
u32 glSetCurrentTexture(unsigned long texture, eGLTextureType type);
u32 glGetCurrentRasterState();
u32 glSetCurrentRasterState(unsigned long raster);
unsigned long long glGetCurrentTextureState();
unsigned long long glSetCurrentTextureState(unsigned long long state);
u32 glGetCurrentProgram();
u32 glSetCurrentProgram(unsigned long program);

u32 glGetRasterState(unsigned long raster, eGLState state);
u32 glSetRasterState(u32& raster, eGLState state, unsigned long value);
u32 glSetRasterState(eGLState state, unsigned long value);
u32 glGetTextureState(unsigned long long texture, eGLTextureState texturestate);
u32 glGetTextureState(eGLTextureState texturestate);
u32 glSetTextureState(eGLTextureState state, unsigned long value);

void glSetRasterStateDefaults();
void glSetTextureStateDefaults();
unsigned long glHandleizeRasterState();
unsigned long long glHandleizeTextureState();
void glSetDefaultState(bool setRasterDefaults);

#endif // _GLSTATE_H_
