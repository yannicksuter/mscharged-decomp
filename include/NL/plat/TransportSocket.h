#ifndef NL_PLAT_TRANSPORT_SOCKET_H
#define NL_PLAT_TRANSPORT_SOCKET_H

#include "types.h"

struct TransportSocket
{
    int socket;
};

void TransportSocketInitialize(TransportSocket* transport);
bool TransportSocketOpen(TransportSocket* transport, bool stream);
bool TransportSocketBind(TransportSocket* transport, u16 port);
void TransportSocketClose(TransportSocket* transport);
bool TransportSocketIsOpen(TransportSocket* transport);
// The Wii implementation ignores the second argument.
void TransportSocketSetNonBlocking(TransportSocket* transport, bool);
int TransportSocketConnect(TransportSocket* transport, const u8* host, u16 port);
int TransportSocketSend(TransportSocket* transport, const void* data, int size);
int TransportSocketBroadcast(TransportSocket* transport, const void* data, int size, u16 port);
int TransportSocketSendTo(TransportSocket* transport, const void* data, int size, const u8* host, u16 port);
int TransportSocketReceiveFrom(TransportSocket* transport, void* data, int size, unsigned int* host, u16* port);

#endif // NL_PLAT_TRANSPORT_SOCKET_H
