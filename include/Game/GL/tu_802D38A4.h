#ifndef GAME_GL_TU_802D38A4_H
#define GAME_GL_TU_802D38A4_H

struct glModel;
struct glModelPacket;
struct glModelStream;

void fn_802D38A4(glModel* pModel, int numVerts, int prim, void* pResource,
    int numStreams, unsigned long programHash);
void fn_802D39CC(
    glModelStream* pStream, int stream, void* address, int stride, int type);
void fn_802D39E8(
    glModel* pModel, glModelPacket* pPackets, unsigned long numPackets);
void fn_802D39F4(
    glModelPacket* pPacket, glModelStream* pStreams, int numStreams);
void fn_802D3A00(glModelStream* pStream, void* address);

#endif // GAME_GL_TU_802D38A4_H
