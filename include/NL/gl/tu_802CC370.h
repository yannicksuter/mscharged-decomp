#ifndef NL_GL_TU_802CC370_H
#define NL_GL_TU_802CC370_H

struct glModelPacket;
struct GXMaterialParameter;

const GXMaterialParameter* fn_802CC370(
    const glModelPacket* packet, unsigned long index);
void fn_802CC3C8(glModelPacket* packet, unsigned long hash,
    const void* value, unsigned long count);
void fn_802CC458(
    glModelPacket* packet, unsigned long hash, unsigned long texture);
void fn_802CC4FC(glModelPacket* packet, unsigned long hash,
    const unsigned long* textureIndex);
void fn_802CC59C(glModelPacket* packet, unsigned long hash,
    unsigned long first, unsigned long second);
void fn_802CC628(glModelPacket* packet, unsigned long hash, float value);
void fn_802CC6C0(
    glModelPacket* packet, unsigned long hash, unsigned long value);
float fn_802CC758(const glModelPacket* packet, unsigned long hash);
unsigned long fn_802CC7E4(const glModelPacket* packet, unsigned long hash);
void* fn_802CC870(const glModelPacket* packet, unsigned long hash);
bool fn_802CC8FC(const glModelPacket* packet, unsigned long hash);
void fn_802CC978(
    void* program, const glModelPacket* packet, unsigned long texture);

#endif // NL_GL_TU_802CC370_H
