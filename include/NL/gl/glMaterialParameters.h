#ifndef NL_GL_MODEL_PARAMETERS_H
#define NL_GL_MODEL_PARAMETERS_H

struct glModelPacket;
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
    void* program, const glModelPacket* packet, unsigned long texture);

#endif // NL_GL_MODEL_PARAMETERS_H
