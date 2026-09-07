#include <revolution/so.h>
#include "Game/Sys/debug.h"

#include "NL/plat/TransportSocket.h"


extern "C" void TransportSocketInitialize(TransportSocket* transport)
{
    transport->socket = -1;
}

extern "C" bool TransportSocketOpen(TransportSocket* transport, bool stream)
{
    transport->socket = SOSocket2(
        SO_PF_INET, stream ? SO_SOCK_STREAM : SO_SOCK_DGRAM, 0);
    if (transport->socket < 0)
    {
        tDebugPrintManager::Print(DC_NETWORK, "Failed to open TransportSocket error %d\n",
            transport->socket);
        transport->socket = -1;
        return false;
    }
    return true;
}

extern "C" bool TransportSocketBind(TransportSocket* transport, u16 port)
{
    if (transport->socket == -1)
    {
        return false;
    }

    SOInAddr host = { 0 };
    SOSockAddrIn address = { sizeof(address), SO_PF_INET, 0, { 0 } };
    address.port = SOHtoNs(port);
    address.addr = host;

    int result = SOBind(transport->socket, &address);
    if (result == 0)
    {
        return true;
    }

    tDebugPrintManager::Print(DC_NETWORK, "Failed to Bind TransportSocket result %d\n", result);
    if (transport->socket != -1)
    {
        SOClose(transport->socket);
        transport->socket = -1;
    }
    return false;
}

extern "C" void TransportSocketClose(TransportSocket* transport)
{
    if (transport->socket != -1)
    {
        SOClose(transport->socket);
        transport->socket = -1;
    }
}

extern "C" bool TransportSocketIsOpen(TransportSocket* transport)
{
    return transport->socket != -1;
}

extern "C" void TransportSocketSetNonBlocking(TransportSocket* transport, bool)
{
    if (transport->socket != -1)
    {
        int flags = SOFcntl(transport->socket, SO_F_GETFL, 0);
        int result = SOFcntl(
            transport->socket, SO_F_SETFL, flags | SO_O_NONBLOCK);
        if (result < 0)
        {
            tDebugPrintManager::Print(DC_NETWORK,
                "Failed to set socket to nonblocking mode error %d", result);
        }
    }
}

extern "C" int TransportSocketConnect(
    TransportSocket* transport, const u8* host, u16 port)
{
    if (transport->socket == -1)
    {
        return -1;
    }

    u32 hostAddress = *reinterpret_cast<const u32*>(host);
    SOSockAddrIn address = { sizeof(address), SO_PF_INET, 0, { 0 } };
    address.port = SOHtoNs(port);
    address.addr.addr = hostAddress;
    return SOConnect(transport->socket, &address);
}

extern "C" int TransportSocketSend(
    TransportSocket* transport, const void* data, int size)
{
    if (transport->socket == -1)
    {
        return -1;
    }
    return SOSend(transport->socket, data, size, 0);
}

extern "C" int TransportSocketBroadcast(
    TransportSocket* transport, const void* data, int size, u16 port)
{
    if (transport->socket == -1)
    {
        return -1;
    }

    SOSockAddrIn address = {
        sizeof(address), SO_PF_INET, 0, { 0xffffffff }
    };
    address.port = SOHtoNs(port);
    return SOSendTo(transport->socket, data, size, 0, &address);
}

extern "C" int TransportSocketSendTo(TransportSocket* transport, const void* data,
    int size, const u8* host, u16 port)
{
    if (transport->socket == -1)
    {
        return -1;
    }

    u32 hostAddress = *reinterpret_cast<const u32*>(host);
    SOSockAddrIn address = { sizeof(address), SO_PF_INET, 0, { 0 } };
    address.port = SOHtoNs(port);
    address.addr.addr = hostAddress;
    return SOSendTo(transport->socket, data, size, 0, &address);
}

extern "C" int TransportSocketReceiveFrom(TransportSocket* transport, void* data, int size,
    u32* host, u16* port)
{
    if (transport->socket == -1)
    {
        return -1;
    }

    SOSockAddrIn address = { sizeof(address), SO_PF_INET, 0, { 0 } };
    int result = SORecvFrom(transport->socket, data, size, 0, &address);
    if (host != 0)
    {
        *host = address.addr.addr;
    }
    if (port != 0)
    {
        *port = SONtoHs(address.port);
    }
    return result;
}
