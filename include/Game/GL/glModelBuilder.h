#ifndef GAME_GL_MODEL_BUILDER_H
#define GAME_GL_MODEL_BUILDER_H

struct glModel;
struct glModelPacket;
struct glModelStream;

void glCreateModel(glModel* pModel, int numVerts, int prim, void* pResource,
    int numStreams, unsigned long programHash);
void glSetModelStream(
    glModelStream* pStream, int stream, void* address, int stride, int type);
void glSetModelPackets(
    glModel* pModel, glModelPacket* pPackets, unsigned long numPackets);
void glSetPacketStreams(
    glModelPacket* pPacket, glModelStream* pStreams, int numStreams);
void glSetStreamAddress(glModelStream* pStream, void* address);

#endif // GAME_GL_MODEL_BUILDER_H
