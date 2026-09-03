#include <revolution/types.h>

#define SO_PF_INET 2
#define SO_SOCK_STREAM 1
#define SO_SOCK_DGRAM 2
#define SO_F_GETFL 3
#define SO_F_SETFL 4
#define SO_O_NONBLOCK 4

struct SOInAddr
{
    u32 addr;
};

struct SOSockAddrIn
{
    u8 len;
    u8 family;
    u16 port;
    SOInAddr addr;
};

extern "C"
{
    int SOSocket2(int pf, int type, int protocol);
    int SOClose(int socket);
    int SOBind(int socket, const void* address);
    int SOConnect(int socket, const void* address);
    int SORecvFrom(
        int socket, void* data, int size, int flags, void* address);
    int SOSendTo(int socket, const void* data, int size, int flags,
        const void* address);
    int SOSend(int socket, const void* data, int size, int flags);
    int SOFcntl(int socket, int command, ...);
    u16 SONtoHs(u16 value);
    u16 SOHtoNs(u16 value);
}

struct TransportSocket
{
    int socket;
};

extern "C" void fn_8004F594(int category, const char* format, ...);

extern "C" void fn_80374D68(TransportSocket* transport)
{
    transport->socket = -1;
}

extern "C" bool fn_80374D74(TransportSocket* transport, bool stream)
{
    transport->socket = SOSocket2(
        SO_PF_INET, stream ? SO_SOCK_STREAM : SO_SOCK_DGRAM, 0);
    if (transport->socket < 0)
    {
        fn_8004F594(0x10, "Failed to open TransportSocket error %d\n",
            transport->socket);
        transport->socket = -1;
        return false;
    }
    return true;
}

extern "C" bool fn_80374DF0(TransportSocket* transport, u16 port)
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

    fn_8004F594(
        0x10, "Failed to Bind TransportSocket result %d\n", result);
    if (transport->socket != -1)
    {
        SOClose(transport->socket);
        transport->socket = -1;
    }
    return false;
}

extern "C" void fn_80374EA8(TransportSocket* transport)
{
    if (transport->socket != -1)
    {
        SOClose(transport->socket);
        transport->socket = -1;
    }
}

extern "C" bool fn_80374EEC(TransportSocket* transport)
{
    return transport->socket != -1;
}

extern "C" void fn_80374F04(TransportSocket* transport)
{
    if (transport->socket != -1)
    {
        int flags = SOFcntl(transport->socket, SO_F_GETFL, 0);
        int result = SOFcntl(
            transport->socket, SO_F_SETFL, flags | SO_O_NONBLOCK);
        if (result < 0)
        {
            fn_8004F594(0x10,
                "Failed to set socket to nonblocking mode error %d", result);
        }
    }
}

extern "C" int fn_80374F84(
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

extern "C" int fn_80374FF8(
    TransportSocket* transport, const void* data, int size)
{
    if (transport->socket == -1)
    {
        return -1;
    }
    return SOSend(transport->socket, data, size, 0);
}

extern "C" int fn_80375018(
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

extern "C" int fn_803750A0(TransportSocket* transport, const void* data,
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

extern "C" int fn_80375138(TransportSocket* transport, void* data, int size,
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
