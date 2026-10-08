#ifndef NL_GLX_GLXMODEL_H
#define NL_GLX_GLXMODEL_H

class nlMatrix4;
struct glModelPacket;

void glplatSetMatrix(unsigned long matrix, const nlMatrix4& m);
void glplatGetMatrix(unsigned long matrix, nlMatrix4& m);
void glplatFinalizePacket(glModelPacket* packet, bool permanent, void* allocator);

// Platform hook applied after a packet and its material data have been cloned.
void glplatOnPacketCloned(glModelPacket* packet, void* allocator);

#endif // NL_GLX_GLXMODEL_H
