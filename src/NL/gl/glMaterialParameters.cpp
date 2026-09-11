#include <revolution/types.h>

#include "NL/gl/glMaterialParameters.h"
#include "NL/gl/glMaterialProgram.h"
#include "NL/gl/glModel.h"
#include "NL/gl/glState.h"
#include "NL/gl/glTexture.h"

#include <string.h>

namespace
{
inline const GXMaterialParameter* FindParameter(
    const glModelPacket* packet, unsigned long hash)
{
    GLMaterialProgram* program = (GLMaterialProgram*)packet->materialProgram;
    unsigned long count = program->parameterCount;
    const GXMaterialParameter* parameter = program->GetParameters();

    for (unsigned long i = 0; i < count; ++i, ++parameter)
    {
        if (hash == parameter->hash)
        {
            return parameter;
        }
    }

    return 0;
}

inline unsigned char* GetParameterData(
    const glModelPacket* packet, const GXMaterialParameter* parameter)
{
    return (unsigned char*)packet->materialParameters + parameter->offset;
}
} // namespace

const GXMaterialParameter* glGetMaterialParameterInfo(
    const glModelPacket* packet, unsigned long index)
{
    GLMaterialProgram* program = (GLMaterialProgram*)packet->materialProgram;
    if (index < program->parameterCount)
    {
        return program->GetParameters() + index;
    }
    return 0;
}

void glSetMaterialParameterArray(glModelPacket* packet, unsigned long hash,
    const void* value, unsigned long count)
{
    const GXMaterialParameter* parameter = FindParameter(packet, hash);
    memcpy(GetParameterData(packet, parameter), value,
        count * sizeof(unsigned long));
}

void glSetMaterialTextureParameter(
    glModelPacket* packet, unsigned long hash, unsigned long texture)
{
    const GXMaterialParameter* parameter = FindParameter(packet, hash);
    unsigned char* data = GetParameterData(packet, parameter);
    *(unsigned long*)data = texture;
    *(unsigned short*)(data + 4) = 0xFFFF;
}

void glSetMaterialTextureIndexParameter(glModelPacket* packet, unsigned long hash,
    const unsigned long* textureIndex)
{
    const GXMaterialParameter* parameter = FindParameter(packet, hash);
    unsigned char* data = GetParameterData(packet, parameter);
    *(unsigned short*)(data + 4) = *textureIndex;
}

void glSetMaterialBufferParameter(glModelPacket* packet, unsigned long hash,
    unsigned long buffer, unsigned long size)
{
    const GXMaterialParameter* parameter = FindParameter(packet, hash);
    unsigned long* data =
        (unsigned long*)GetParameterData(packet, parameter);
    data[0] = buffer;
    data[1] = size;
}

void glSetMaterialFloatParameter(
    glModelPacket* packet, unsigned long hash, float value)
{
    const GXMaterialParameter* parameter = FindParameter(packet, hash);
    *(float*)GetParameterData(packet, parameter) = value;
}

void glSetMaterialUnsignedParameter(
    glModelPacket* packet, unsigned long hash, unsigned long value)
{
    const GXMaterialParameter* parameter = FindParameter(packet, hash);
    *(unsigned long*)GetParameterData(packet, parameter) = value;
}

float glGetMaterialFloatParameter(
    const glModelPacket* packet, unsigned long hash)
{
    const GXMaterialParameter* parameter = FindParameter(packet, hash);
    return *(float*)GetParameterData(packet, parameter);
}

unsigned long glGetMaterialUnsignedParameter(
    const glModelPacket* packet, unsigned long hash)
{
    const GXMaterialParameter* parameter = FindParameter(packet, hash);
    return *(unsigned long*)GetParameterData(packet, parameter);
}

void* glGetMaterialParameterData(
    const glModelPacket* packet, unsigned long hash)
{
    const GXMaterialParameter* parameter = FindParameter(packet, hash);
    return GetParameterData(packet, parameter);
}

bool glHasMaterialParameter(
    const glModelPacket* packet, unsigned long hash)
{
    return FindParameter(packet, hash) != 0;
}

void glSetMaterialTextureAlphaState(
    void*, const glModelPacket* packet, unsigned long texture)
{
    if (texture == 0xFFFFFFFF)
    {
        return;
    }
    if (!glTextureLoad(texture))
    {
        return;
    }

    switch (glTextureGetNumBits(3))
    {
    case 0:
        break;
    case 1:
    {
        unsigned int* rasterState = (unsigned int*)&packet->rasterState;
        glSetRasterState(*rasterState, GLS_AlphaTest, 1);
        glSetRasterState(*rasterState, GLS_AlphaTestRef, 0x80);
        break;
    }
    default:
    {
        unsigned int* rasterState = (unsigned int*)&packet->rasterState;
        glSetRasterState(*rasterState, GLS_AlphaTest, 1);
        glSetRasterState(*rasterState, GLS_AlphaTestRef, 0);
        glSetRasterState(*rasterState, GLS_AlphaBlend, 1);
        glSetRasterState(*rasterState, GLS_DepthWrite, 0);
        glSetRasterState(*rasterState, GLS_Culling, 0);
        break;
    }
    }
}
