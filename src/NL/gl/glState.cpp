#include "NL/gl/glState.h"

extern "C" void* memcpy(void* dest, const void* src, unsigned long size);

u32 fn_802C8098(const char* name);
u32 fn_802CBE70();

class glRasterState
{
public:
    /* 0x00 */ unsigned long m_State;
}; // total size: 0x4

class glTextureState
{
public:
    /* 0x00 */ unsigned long long m_State;
}; // total size: 0x8

static glStateBundle _bundle;

static glRasterState _state;
static glTextureState _textureState;

static unsigned long defaultRasterState = 0;
static unsigned long long defaultTextureState = 0;

static gl_StateBitfield packed_raster[GLS_Num + 1] = { 0, 1, 0, 1, 0, 2, 0, 1, 0, 8, 0, 3, 0, 2, 0, 2, 0, 1, 0, 1,
                                                       0, 1, 0, 2, 0, 2, 0, 2, 0, 1, 0, 2, 0, 0 };
static gl_StateBitfield packed_texture[GLTS_Num + 1] = { 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 2, 0, 1, 0, 1, 0, 1, 0,
                                                         1, 0, 1, 0, 1, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 6, 0, 0 };

static inline unsigned long GetTextureStateImpl(unsigned long long* pTexture, eGLTextureState texturestate)
{
    gl_StateBitfield* p = &packed_texture[texturestate];
    unsigned long out = 0;

    for (s32 i = 0; i < p->numBits; i++)
    {
        if (*pTexture & (1ull << (i + p->startBit)))
        {
            out |= (1u << i);
        }
    }

    return out;
}

static inline unsigned long glExtractRasterBits(unsigned long raster, gl_StateBitfield* p, s32* pn)
{
    unsigned long out = 0;
    for (s32 i = 0; i < *pn; i++)
    {
        if (raster & (1u << (i + p->startBit)))
        {
            out |= (1u << i);
        }
    }
    return out;
}

static inline void glPackStateBits(gl_StateBitfield* p)
{
    s32 bit = 0;
    while (p->numBits != 0)
    {
        p->startBit = bit;
        bit += p->numBits;
        p++;
    }
}

void gl_StateStartup()
{
    _bundle.texconfig = 0;

    glPackStateBits(packed_raster);
    glPackStateBits(packed_texture);

    glSetRasterState(GLS_DepthTest, 0);
    glSetRasterState(GLS_DepthWrite, 0);
    glSetRasterState(GLS_DepthFunc, 1);
    glSetRasterState(GLS_AlphaTest, 0);
    glSetRasterState(GLS_AlphaTestRef, 0);
    glSetRasterState(GLS_AlphaBlend, 0);
    glSetRasterState(GLS_Culling, 1);
    glSetRasterState(GLS_ColourWrite, 3);
    glSetRasterState(GLS_SolidOffset, 0);
    glSetRasterState(GLS_FillMode, 0);
    glSetRasterState(GLS_State10, 0);
    glSetRasterState(GLS_State11, 0);
    glSetRasterState(GLS_State12, 0);
    glSetRasterState(GLS_State13, 0);
    glSetRasterState(GLS_State14, 0);
    glSetRasterState(GLS_State15, 0);

    defaultRasterState = glHandleizeRasterState();

    glSetTextureState(GLTS_DiffuseWrap, 0);
    glSetTextureState(GLTS_DetailWrap, 0);
    glSetTextureState(GLTS_ShadowWrap, 0);
    glSetTextureState(GLTS_SelfIllumWrap, 0);
    glSetTextureState(GLTS_GlossWrap, 0);
    glSetTextureState(GLTS_BumpLocalWrap, 0);
    glSetTextureState(GLTS_DiffuseFilter, 0);
    glSetTextureState(GLTS_DetailFilter, 0);
    glSetTextureState(GLTS_ShadowFilter, 0);
    glSetTextureState(GLTS_SelfIllumFilter, 0);
    glSetTextureState(GLTS_GlossFilter, 0);
    glSetTextureState(GLTS_BumpLocalFilter, 0);
    glSetTextureState(GLTS_DiffuseLevel, (u8)glPackTextureLevel(1.0f));
    glSetTextureState(GLTS_DetailLevel, (u8)glPackTextureLevel(0.0f));
    glSetTextureState(GLTS_ShadowLevel, (u8)glPackTextureLevel(1.0f));
    glSetTextureState(GLTS_SelfIllumLevel, (u8)glPackTextureLevel(1.0f));
    glSetTextureState(GLTS_GlossLevel, (u8)glPackTextureLevel(1.0f));
    glSetTextureState(GLTS_BumpLocalLevel, (u8)glPackTextureLevel(1.0f));

    defaultTextureState = glHandleizeTextureState();
}

glStateBundle* gl_GetCurrentStateBundle()
{
    return &_bundle;
}

void glStateSave(glStateBundle& state)
{
    memcpy(&state, &_bundle, sizeof(glStateBundle));
}

void glStateRestore(const glStateBundle& state)
{
    memcpy(&_bundle, &state, sizeof(glStateBundle));
}

u32 glGetCurrentTexture(eGLTextureType type)
{
    return _bundle.texture[type];
}

u32 glSetCurrentTexture(unsigned long texture, eGLTextureType type)
{
    unsigned long prev = _bundle.texture[type];
    unsigned long mask = 1u << type;

    if (texture == 0xFFFFFFFFu)
    {
        _bundle.texconfig = _bundle.texconfig & ~mask;
    }
    else
    {
        _bundle.texconfig = _bundle.texconfig | mask;
    }

    _bundle.texture[type] = texture;
    return prev;
}

u32 glGetCurrentRasterState()
{
    return _bundle.raster;
}

u32 glSetCurrentRasterState(unsigned long raster)
{
    unsigned long prev = _bundle.raster;
    _bundle.raster = raster;
    return prev;
}

unsigned long long glGetCurrentTextureState()
{
    return _bundle.texturestate;
}

unsigned long long glSetCurrentTextureState(unsigned long long state)
{
    unsigned long long prev = _bundle.texturestate;
    _bundle.texturestate = state;
    return prev;
}

u32 glGetCurrentProgram()
{
    return _bundle.program;
}

u32 glSetCurrentProgram(unsigned long program)
{
    unsigned long prev = _bundle.program;
    _bundle.program = program;
    return prev;
}

u32 glGetTexture(const char* textureName)
{
    if (textureName == 0)
    {
        return 0xFFFFFFFFu;
    }

    return fn_802C8098(textureName);
}

u32 glGetRasterState(unsigned long raster, eGLState state)
{
    gl_StateBitfield* p = &packed_raster[state];
    s32 numBits = p->numBits;
    unsigned long out = 0;

    for (s32 i = 0; i < numBits; i++)
    {
        if (raster & (1u << (i + p->startBit)))
        {
            out |= (1u << i);
        }
    }

    return out;
}

u32 glSetRasterState(eGLState state, unsigned long value)
{
    gl_StateBitfield* p = &packed_raster[state];
    s32* pn = &p->numBits;
    unsigned long out = 0;
    s32 i;

    for (i = 0; i < *pn; i++)
    {
        if (_state.m_State & (1u << (i + p->startBit)))
        {
            out |= (1u << i);
        }
    }

    for (i = 0; i < *pn; i++)
    {
        if (value & (1u << i))
        {
            _state.m_State = _state.m_State | (1u << (i + p->startBit));
        }
        else
        {
            _state.m_State = _state.m_State & ~(1u << (i + p->startBit));
        }
    }

    return out;
}

u32 glSetRasterState(u32& raster, eGLState state, unsigned long value)
{
    gl_StateBitfield* p = &packed_raster[state];
    s32* pn = &p->numBits;
    unsigned long out = glExtractRasterBits(raster, p, pn);

    for (s32 i = 0; i < *pn; i++)
    {
        if (value & (1u << i))
        {
            raster |= (1u << (i + p->startBit));
        }
        else
        {
            raster &= ~(1u << (i + p->startBit));
        }
    }

    return out;
}

u32 glGetTextureState(eGLTextureState texturestate)
{
    unsigned long long texture = _textureState.m_State;
    return GetTextureStateImpl(&texture, texturestate);
}

u32 glGetTextureState(unsigned long long texture, eGLTextureState texturestate)
{
    return GetTextureStateImpl(&texture, texturestate);
}

u32 glSetTextureState(eGLTextureState state, unsigned long value)
{
    gl_StateBitfield* p = &packed_texture[state];
    s32 numBits = p->numBits;
    unsigned long out = 0;
    s32 i;

    for (i = 0; i < numBits; i++)
    {
        if (_textureState.m_State & (1ull << (i + p->startBit)))
        {
            out |= (1u << i);
        }
    }

    for (i = 0; i < numBits; i++)
    {
        if (value & (1u << i))
        {
            _textureState.m_State = _textureState.m_State | (1ull << (i + p->startBit));
        }
        else
        {
            _textureState.m_State = _textureState.m_State & ~(1ull << (i + p->startBit));
        }
    }

    return out;
}

void glSetRasterStateDefaults()
{
    _state.m_State = defaultRasterState;
}

void glSetTextureStateDefaults()
{
    _textureState.m_State = defaultTextureState;
}

unsigned long glHandleizeRasterState()
{
    return _state.m_State;
}

unsigned long long glHandleizeTextureState()
{
    return _textureState.m_State;
}

void glSetDefaultState(bool setRasterDefaults)
{
    glSetRasterStateDefaults();
    glSetTextureStateDefaults();

    if (setRasterDefaults)
    {
        glSetRasterState(GLS_DepthTest, 0);
        glSetRasterState(GLS_DepthWrite, 0);
    }

    _bundle.texturestate = _textureState.m_State;
    _bundle.raster = _state.m_State;
    _bundle.program = fn_802CBE70();
    _bundle.texconfig = _bundle.texconfig & 0xC0;

    for (s32 i = 0; i < GLTT_Num; i++)
    {
        _bundle.texture[i] = 0xFFFFFFFFu;
    }
}

u32 glPackTextureLevel(float level)
{
    return (u8)(63.0f * level);
}
