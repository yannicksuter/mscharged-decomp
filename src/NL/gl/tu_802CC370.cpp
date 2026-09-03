#include <revolution/types.h>

#include "NL/glx/GXMaterialProgram.h"
#include "NL/gl/glState.h"
#include "NL/gl/glTexture.h"

#include <string.h>

namespace
{
typedef GXMaterialProgramImpl<GXMaterialProgram_802A6B6C> MaterialProgram;

inline const GXMaterialParameter* FindParameter(
    const glModelPacket* packet, unsigned long hash)
{
    MaterialProgram* program = (MaterialProgram*)packet->unknown10;
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
    return (unsigned char*)packet->unknown20 + parameter->offset;
}
} // namespace

extern "C" const GXMaterialParameter* fn_802CC370(
    const glModelPacket* packet, unsigned long index)
{
    MaterialProgram* program = (MaterialProgram*)packet->unknown10;
    if (index < program->parameterCount)
    {
        return program->GetParameters() + index;
    }
    return 0;
}

extern "C" void fn_802CC3C8(glModelPacket* packet, unsigned long hash,
    const void* value, unsigned long count)
{
    const GXMaterialParameter* parameter = FindParameter(packet, hash);
    memcpy(GetParameterData(packet, parameter), value,
        count * sizeof(unsigned long));
}

extern "C" void fn_802CC458(
    glModelPacket* packet, unsigned long hash, unsigned long texture)
{
    const GXMaterialParameter* parameter = FindParameter(packet, hash);
    unsigned char* data = GetParameterData(packet, parameter);
    *(unsigned long*)data = texture;
    *(unsigned short*)(data + 4) = 0xFFFF;
}

extern "C" void fn_802CC4FC(glModelPacket* packet, unsigned long hash,
    const unsigned long* textureIndex)
{
    const GXMaterialParameter* parameter = FindParameter(packet, hash);
    unsigned char* data = GetParameterData(packet, parameter);
    *(unsigned short*)(data + 4) = *textureIndex;
}

extern "C" void fn_802CC59C(glModelPacket* packet, unsigned long hash,
    unsigned long first, unsigned long second)
{
    const GXMaterialParameter* parameter = FindParameter(packet, hash);
    unsigned long* data =
        (unsigned long*)GetParameterData(packet, parameter);
    data[0] = first;
    data[1] = second;
}

extern "C" void fn_802CC628(
    glModelPacket* packet, unsigned long hash, float value)
{
    const GXMaterialParameter* parameter = FindParameter(packet, hash);
    *(float*)GetParameterData(packet, parameter) = value;
}

extern "C" void fn_802CC6C0(
    glModelPacket* packet, unsigned long hash, unsigned long value)
{
    const GXMaterialParameter* parameter = FindParameter(packet, hash);
    *(unsigned long*)GetParameterData(packet, parameter) = value;
}

extern "C" float fn_802CC758(
    const glModelPacket* packet, unsigned long hash)
{
    const GXMaterialParameter* parameter = FindParameter(packet, hash);
    return *(float*)GetParameterData(packet, parameter);
}

extern "C" unsigned long fn_802CC7E4(
    const glModelPacket* packet, unsigned long hash)
{
    const GXMaterialParameter* parameter = FindParameter(packet, hash);
    return *(unsigned long*)GetParameterData(packet, parameter);
}

extern "C" void* fn_802CC870(
    const glModelPacket* packet, unsigned long hash)
{
    const GXMaterialParameter* parameter = FindParameter(packet, hash);
    return GetParameterData(packet, parameter);
}

extern "C" bool fn_802CC8FC(
    const glModelPacket* packet, unsigned long hash)
{
    return FindParameter(packet, hash) != 0;
}

extern "C" void fn_802CC978(
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
