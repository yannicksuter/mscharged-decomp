#include "NL/plat/TransportSocket.h"

void TransportSocketInitialize(TransportSocket* transport)
{
    transport->socket = -1;
}

bool TransportSocketOpen(TransportSocket*, bool)
{
    return false;
}

bool TransportSocketBind(TransportSocket*, u16)
{
    return false;
}

void TransportSocketClose(TransportSocket*)
{
}

bool TransportSocketIsOpen(TransportSocket*)
{
    return false;
}

void TransportSocketSetNonBlocking(TransportSocket*, bool)
{
}

int TransportSocketConnect(TransportSocket*, const u8*, u16)
{
    return -1;
}

int TransportSocketSend(TransportSocket*, const void*, int)
{
    return -1;
}

int TransportSocketBroadcast(TransportSocket*, const void*, int, u16)
{
    return -1;
}

int TransportSocketSendTo(TransportSocket*, const void*, int, const u8*, u16)
{
    return -1;
}

int TransportSocketReceiveFrom(TransportSocket*, void*, int, unsigned int*, u16*)
{
    return -1;
}
