#ifndef NL_GL_MODEL_PARAMETERS_H
#define NL_GL_MODEL_PARAMETERS_H

#include "NL/gl/glModel.h"

struct GXMaterialParameter;

const GXMaterialParameter* glGetMaterialParameterInfo(
    const glModelPacket* packet, unsigned long index);
void glSetMaterialParameterArray(glModelPacket* packet, unsigned long hash,
    const void* value, unsigned long count);
void glSetMaterialTextureParameter(
    glModelPacket* packet, unsigned long hash, unsigned long texture);
void glSetMaterialTextureIndexParameter(glModelPacket* packet, unsigned long hash,
    const unsigned long* textureIndex);
void glSetMaterialBufferParameter(glModelPacket* packet, unsigned long hash,
    unsigned long buffer, unsigned long size);
void glSetMaterialFloatParameter(glModelPacket* packet, unsigned long hash, float value);
void glSetMaterialUnsignedParameter(
    glModelPacket* packet, unsigned long hash, unsigned long value);
float glGetMaterialFloatParameter(const glModelPacket* packet, unsigned long hash);
unsigned long glGetMaterialUnsignedParameter(const glModelPacket* packet, unsigned long hash);
void* glGetMaterialParameterData(const glModelPacket* packet, unsigned long hash);
bool glHasMaterialParameter(const glModelPacket* packet, unsigned long hash);
void glSetMaterialTextureAlphaState(
    void* program, glModelPacket* packet, unsigned long texture);

// An override of 1.0f selects the value stored in the packet.
template <class Parameters>
inline float glGetMaterialFloatParameterWithOverride(const glModelPacket* packet,
    float Parameters::*member, const float& overrideValue)
{
    if (1.0f != overrideValue)
        return overrideValue;
    return static_cast<const Parameters*>(packet->materialParameters)->*member;
}

#endif // NL_GL_MODEL_PARAMETERS_H
