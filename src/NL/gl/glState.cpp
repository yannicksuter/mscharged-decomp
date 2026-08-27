#include "NL/gl/glState.h"

#include "NL/gl/glMatrix.h"

extern "C" void* memcpy(void* dest, const void* src, unsigned long size);

extern "C" u32 fn_802C8098(const char* name);

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

static inline unsigned long glSetCurrentTextureInline(unsigned long texture, eGLTextureType type)
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

static inline bool glTextureBitIsSet(const unsigned long long* texture, s32 bit)
{
    const u32* words = (const u32*)texture;
    if (bit < 32)
    {
        return (words[0] & (1u << bit)) != 0;
    }
    return (words[1] & (1u << (bit - 32))) != 0;
}

static inline unsigned long GetTextureStateImpl(unsigned long long* pTexture, eGLTextureState texturestate)
{
    gl_StateBitfield* pInfo = &packed_texture[texturestate];
    s32 numBits = pInfo->numBits;
    unsigned long long texture = *pTexture;
    s32 cnt = 0;
    unsigned long out = 0;
    cnt = (s32)out;
    unsigned long one = 1;

    for (; cnt < numBits; cnt++)
    {
        if (glTextureBitIsSet(&texture, cnt + pInfo->startBit))
        {
            out |= (one << cnt);
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

static inline unsigned long glSetRasterStateRefImpl(
    u32& raster,
    gl_StateBitfield* p,
    unsigned long value,
    unsigned long one,
    s32 startBit,
    unsigned long oldRaster)
{
    s32 i;
    unsigned long out = 0;
    s32 numBits = p->numBits;
    i = 0;
    for (; i < numBits; i++)
    {
        if (oldRaster & (one << (i + startBit)))
        {
            out |= (one << i);
        }
    }

    for (i = 0; i < p->numBits; i++)
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

static inline void glPackTextureStateBits(s32 bit, gl_StateBitfield* p)
{
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
    glPackTextureStateBits(0, packed_texture);

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

u32 glGetCurrentMatrix()
{
    return _bundle.matrix;
}

u32 glSetCurrentMatrix(unsigned long matrix)
{
    unsigned long prev = _bundle.matrix;
    _bundle.matrix = matrix;
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
    return glSetRasterStateRefImpl(raster, p, value, 1, p->startBit, raster);
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

// clang-format off
asm u32 glSetTextureState(eGLTextureState state, unsigned long value)
{
    nofralloc
    lis r5, packed_texture@ha
    stwu r1, -0x10(r1)
    slwi r11, r3, 3
    li r3, 0
    addi r5, r5, packed_texture@l
    lwz r8, _textureState(r13)
    add r5, r5, r11
    lwz r9, _textureState+4(r13)
    lwz r0, 4(r5)
    li r10, 0
    stw r8, 8(r1)
    cmpwi r0, 0
    stw r9, 0xc(r1)
    ble @texture_read_done
    lwz r7, 0(r5)
    li r6, 1
    mtctr r0
    ble @texture_read_done
@texture_read_loop:
    add r5, r10, r7
    cmpwi r5, 0x20
    bge @texture_read_low
    slw r0, r6, r5
    and r5, r8, r0
    neg r0, r5
    or r0, r0, r5
    srwi r0, r0, 31
    b @texture_read_test
@texture_read_low:
    subi r0, r5, 0x20
    slw r0, r6, r0
    and r5, r9, r0
    neg r0, r5
    or r0, r0, r5
    srwi r0, r0, 31
@texture_read_test:
    cmpwi r0, 0
    beq @texture_read_next
    slw r0, r6, r10
    or r3, r3, r0
@texture_read_next:
    addi r10, r10, 1
    bdnz @texture_read_loop
@texture_read_done:
    lis r6, packed_texture@ha
    li r10, 0
    addi r6, r6, packed_texture@l
    la r5, _textureState(r13)
    add r7, r6, r11
    li r9, 1
    lwz r0, 4(r7)
    la r6, _textureState(r13)
    mtctr r0
    cmpwi r0, 0
    ble @texture_write_done
@texture_write_loop:
    slw r0, r9, r10
    and. r0, r4, r0
    beq @texture_clear
    lwz r0, 0(r7)
    add r11, r10, r0
    cmpwi r11, 0x20
    bge @texture_set_low
    lwz r8, 0(r5)
    slw r0, r9, r11
    or r0, r8, r0
    stw r0, 0(r5)
    b @texture_write_next
@texture_set_low:
    subi r0, r11, 0x20
    lwz r8, 4(r5)
    slw r0, r9, r0
    or r0, r8, r0
    stw r0, 4(r5)
    b @texture_write_next
@texture_clear:
    lwz r0, 0(r7)
    add r11, r10, r0
    cmpwi r11, 0x20
    bge @texture_clear_low
    lwz r8, 0(r6)
    slw r0, r9, r11
    andc r0, r8, r0
    stw r0, 0(r6)
    b @texture_write_next
@texture_clear_low:
    subi r0, r11, 0x20
    lwz r8, 4(r6)
    slw r0, r9, r0
    andc r0, r8, r0
    stw r0, 4(r6)
@texture_write_next:
    addi r10, r10, 1
    bdnz @texture_write_loop
@texture_write_done:
    addi r1, r1, 0x10
    blr
}
// clang-format on

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
    _state.m_State = defaultRasterState;
    _textureState.m_State = defaultTextureState;

    if (setRasterDefaults)
    {
        s32 i;
        s32 depthTestBit = packed_raster[0].startBit;
        s32 depthTestNumBits = packed_raster[0].numBits;

        for (i = depthTestNumBits; i > 0; i--)
        {
        }

        for (i = 0; i < depthTestNumBits; i++)
        {
            if ((1u << i) & 1u)
            {
                _state.m_State = _state.m_State | (1u << depthTestBit);
            }
            else
            {
                _state.m_State = _state.m_State & ~(1u << depthTestBit);
            }
            depthTestBit++;
        }

        {
            s32 depthWriteBit = packed_raster[1].startBit;
            s32 depthWriteNumBits = packed_raster[1].numBits;

            for (i = depthWriteNumBits; i > 0; i--)
            {
            }

            for (i = 0; i < depthWriteNumBits; i++)
            {
                if ((1u << i) & 1u)
                {
                    _state.m_State = _state.m_State | (1u << depthWriteBit);
                }
                else
                {
                    _state.m_State = _state.m_State & ~(1u << depthWriteBit);
                }
                depthWriteBit++;
            }
        }
    }

    _bundle.raster = _state.m_State;
    _bundle.texturestate = _textureState.m_State;
    _bundle.matrix = glGetIdentityMatrix();

    glSetCurrentTextureInline((u32)-1, GLTT_Diffuse);
    glSetCurrentTextureInline((u32)-1, GLTT_Detail);
    glSetCurrentTextureInline((u32)-1, GLTT_Shadow);
    glSetCurrentTextureInline((u32)-1, GLTT_SelfIllum);
    glSetCurrentTextureInline((u32)-1, GLTT_Gloss);
    glSetCurrentTextureInline((u32)-1, GLTT_BumpLocal);
}

u32 glPackTextureLevel(float level)
{
    return (u8)(63.0f * level);
}
