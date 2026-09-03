#include <revolution/so.h>

#include <revolution/ipc.h>

#include <stdarg.h>
#include <stdio.h>
#include <string.h>

const char* __SOCKET_VERSION = "<< RVL_SDK - SOCKET \trelease build: Dec 10 "
                               "2007 10:02:35 (0x4199_60831) >>";

// static int SO_Initialized = 0;

enum
{
    NET_SO_ACCEPT = 1,
    NET_SO_BIND = 2,
    NET_SO_CLOSE = 3,
    NET_SO_CONNECT = 4,
    NET_SO_FCNTL = 5,
    NET_SO_GETSOCKNAME = 7,
    NET_SO_LISTEN = 10,
    NET_SO_POLL = 11,
    NET_SO_RECVFROM = 12,
    NET_SO_SENDTO = 13,
    NET_SO_SHUTDOWN = 14,
    NET_SO_INETATON = 21,
};

typedef struct NETSoSocketAddr
{
    int s;
    BOOL hasAddr;
    u8 addr[28];
} NETSoSocketAddr;

typedef struct NETSoSendToArgs
{
    int s;
    int flags;
    BOOL hasAddr;
    u8 addr[28];
} NETSoSendToArgs;

static char addressBuffer[40];

static int RecvFrom(const char* funcName, int s, void* buf, int len, int flags,
    void* sockFrom);
static int SendTo(const char* funcName, int s, const void* buf, int len,
    int flags, const void* sockTo);

int SOSocket2(int pf, int type, int protocol)
{
    int result;
    s32 rmId;

    if ((result = SOiPrepare(0, &rmId)) == SO_SUCCESS)
    {
        if (pf == SO_PF_INET6)
        {
            result = SO_EAFNOSUPPORT;
        }
        else
        {
            NETSoSocket* soc = (NETSoSocket*)SOiAlloc(0xc, 0x20);
            if (soc == NULL)
            {
                result = SO_ENOMEM;
            }
            else
            {
                soc->af = pf;
                soc->type = type;
                soc->protocol = protocol;
                result = (int)IOS_Ioctl(rmId, NET_SO_SOCKET, (void*)soc, sizeof(NETSoSocket), NULL, 0);
                SOiFree(0xc, soc, 0x20);
            }
        }
        result = SOiConclude(0, result);
    }
    return result;
}

int SOClose(int s)
{
    int result;
    s32 rmId;

    result = SOiPrepare(NULL, &rmId);
    if (result == SO_SUCCESS)
    {
        int* args = SOiAlloc(12, 32);
        if (args == NULL)
        {
            result = SO_ENOMEM;
        }
        else
        {
            args[0] = s;
            result = IOS_Ioctl(rmId, NET_SO_CLOSE, args, 4, NULL, 0);
            SOiFree(12, args, 32);
        }
        result = SOiConclude(NULL, result);
    }
    return result;
}

int SOListen(int s, int backlog)
{
    int result;
    s32 rmId;

    result = SOiPrepare(NULL, &rmId);
    if (result == SO_SUCCESS)
    {
        int* args = SOiAlloc(12, 32);
        if (args == NULL)
        {
            result = SO_ENOMEM;
        }
        else
        {
            args[0] = s;
            args[1] = backlog;
            result = IOS_Ioctl(rmId, NET_SO_LISTEN, args, 8, NULL, 0);
            SOiFree(12, args, 32);
        }
        result = SOiConclude(NULL, result);
    }
    return result;
}

int SOAccept(int s, void* sockAddr)
{
    int bufferSize;
    int result;
    s32 rmId;
    int* args;
    SOSockAddr* addrBuffer;
    SOSockAddr* addr = sockAddr;

    result = SOiPrepare(NULL, &rmId);
    if (result == SO_SUCCESS)
    {
        if (addr != NULL && (sizeof(SOSockAddr) < addr->len || addr->len < sizeof(SOSockAddr)))
        {
            result = SO_EINVAL;
            goto conclude;
        }

        bufferSize = ROUND_UP((addr == NULL ? 4 : addr->len + 32), 32);
        args = SOiAlloc(12, bufferSize);
        if (args == NULL)
        {
            result = SO_ENOMEM;
            goto conclude;
        }

        args[0] = s;
        addrBuffer = (SOSockAddr*)((u8*)args + 32);
        if (addr == NULL)
        {
            result = IOS_Ioctl(rmId, NET_SO_ACCEPT, args, 4, NULL, 0);
        }
        else
        {
            memcpy(addrBuffer, addr, addr->len);
            result = IOS_Ioctl(rmId, NET_SO_ACCEPT, args, 4, addrBuffer, addr->len);
            if (result >= 0)
            {
                memcpy(addr, addrBuffer, addrBuffer->len);
            }
        }
        SOiFree(12, args, bufferSize);

    conclude:
        result = SOiConclude(NULL, result);
    }
    return result;
}

int SOBind(int s, const void* sockAddr)
{
    const SOSockAddr* addr = sockAddr;
    int result;
    s32 rmId;

    result = SOiPrepare(NULL, &rmId);
    if (result == SO_SUCCESS)
    {
        NETSoSocketAddr* args;

        if (addr == NULL || sizeof(SOSockAddr) < addr->len || addr->len < sizeof(SOSockAddr))
        {
            result = SO_EINVAL;
            goto conclude;
        }

        args = SOiAlloc(12, 64);
        if (args == NULL)
        {
            result = SO_ENOMEM;
            goto conclude;
        }

        args->s = s;
        args->hasAddr = TRUE;
        memcpy(args->addr, addr, addr->len);
        result = IOS_Ioctl(rmId, NET_SO_BIND, args, 36, NULL, 0);
        SOiFree(12, args, 64);

    conclude:
        result = SOiConclude(NULL, result);
    }
    return result;
}

int SOConnect(int s, const void* sockAddr)
{
    const SOSockAddr* addr = sockAddr;
    int result;
    s32 rmId;

    result = SOiPrepare(NULL, &rmId);
    if (result == SO_SUCCESS)
    {
        NETSoSocketAddr* args;

        if (addr == NULL || sizeof(SOSockAddr) < addr->len || addr->len < sizeof(SOSockAddr))
        {
            result = SO_EINVAL;
            goto conclude;
        }

        args = SOiAlloc(12, 64);
        if (args == NULL)
        {
            result = SO_ENOMEM;
            goto conclude;
        }

        args->s = s;
        args->hasAddr = TRUE;
        memcpy(args->addr, addr, addr->len);
        result = IOS_Ioctl(rmId, NET_SO_CONNECT, args, 36, NULL, 0);
        SOiFree(12, args, 64);

    conclude:
        result = SOiConclude(NULL, result);
    }
    return result;
}

int SOGetSockName(int s, void* sockAddr)
{
    int bufferSize;
    int result;
    s32 rmId;
    int* args;
    SOSockAddr* addrBuffer;
    SOSockAddr* addr = sockAddr;

    result = SOiPrepare(NULL, &rmId);
    if (result == SO_SUCCESS)
    {
        if (addr == NULL || sizeof(SOSockAddr) < addr->len || addr->len < sizeof(SOSockAddr))
        {
            result = SO_EINVAL;
            goto conclude;
        }

        bufferSize = ROUND_UP(addr->len + 32, 32);
        args = SOiAlloc(12, bufferSize);
        if (args == NULL)
        {
            result = SO_ENOMEM;
            goto conclude;
        }

        args[0] = s;
        addrBuffer = (SOSockAddr*)((u8*)args + 32);
        memcpy(addrBuffer, addr, addr->len);
        result = IOS_Ioctl(rmId, NET_SO_GETSOCKNAME, args, 4, addrBuffer, addr->len);
        if (result >= 0)
        {
            memcpy(addr, addrBuffer, addrBuffer->len);
        }
        SOiFree(12, args, bufferSize);

    conclude:
        result = SOiConclude(NULL, result);
    }
    return result;
}

int SORecvFrom(int s, void* buf, int len, int flags, void* sockFrom)
{
    return RecvFrom(NULL, s, buf, len, flags, sockFrom);
}

int SORecv(int s, void* buf, int len, int flags)
{
    return RecvFrom(NULL, s, buf, len, flags, NULL);
}

int SOSendTo(int s, const void* buf, int len, int flags,
    const void* sockTo)
{
    return SendTo(NULL, s, buf, len, flags, sockTo);
}

int SOSend(int s, const void* buf, int len, int flags)
{
    return SendTo(NULL, s, buf, len, flags, NULL);
}

int SOFcntl(int s, int cmd, ...)
{
    va_list marker;
    int arg;
    int result;
    s32 rmId;

    va_start(marker, cmd);
    arg = va_arg(marker, int);
    va_end(marker);

    result = SOiPrepare(NULL, &rmId);
    if (result == SO_SUCCESS)
    {
        int* args = SOiAlloc(12, 32);
        if (args == NULL)
        {
            result = SO_ENOMEM;
        }
        else
        {
            args[0] = s;
            args[1] = cmd;
            args[2] = arg;
            result = IOS_Ioctl(rmId, NET_SO_FCNTL, args, 12, NULL, 0);
            SOiFree(12, args, 32);
        }
        result = SOiConclude(NULL, result);
    }
    return result;
}

int SOShutdown(int s, int how)
{
    int result;
    s32 rmId;

    result = SOiPrepare(NULL, &rmId);
    if (result == SO_SUCCESS)
    {
        int* args = SOiAlloc(12, 32);
        if (args == NULL)
        {
            result = SO_ENOMEM;
        }
        else
        {
            args[0] = s;
            args[1] = how;
            result = IOS_Ioctl(rmId, NET_SO_SHUTDOWN, args, 8, NULL, 0);
            SOiFree(12, args, 32);
        }
        result = SOiConclude(NULL, result);
    }
    return result;
}

int SOPoll(SOPollFD fds[], unsigned nfds, OSTime timeout)
{
    int bufferSize;
    int fdsSize;
    int result;
    s32 rmId;
    OSTime* args;
    SOPollFD* fdBuffer;

    result = SOiPrepare(NULL, &rmId);
    if (result == SO_SUCCESS)
    {
        if (fds == NULL)
        {
            result = SO_EINVAL;
            goto conclude;
        }

        fdsSize = nfds * sizeof(SOPollFD);
        bufferSize = ROUND_UP(fdsSize + 32, 32);
        args = SOiAlloc(12, bufferSize);
        if (args == NULL)
        {
            result = SO_ENOMEM;
            goto conclude;
        }

        fdBuffer = (SOPollFD*)((u8*)args + 32);
        if (timeout <= -1)
        {
            memcpy(args, &timeout, sizeof(timeout));
        }
        else
        {
            *args = OSTicksToMilliseconds(timeout);
        }
        memcpy(fdBuffer, fds, fdsSize);
        result = IOS_Ioctl(rmId, NET_SO_POLL, args, 8, fdBuffer, fdsSize);
        if (result >= 0)
        {
            memcpy(fds, fdBuffer, fdsSize);
        }
        SOiFree(12, args, bufferSize);

    conclude:
        result = SOiConclude(NULL, result);
    }
    return result;
}

int SOInetAtoN(const char* cp, SOInAddr* inp)
{
    int bufferSize;
    int result;
    s32 rmId;
    int isTempRm;
    u8* args;
    char* stringBuffer;

    result = SOiPrepareTempRm(NULL, &rmId, &isTempRm);
    if (result == SO_SUCCESS)
    {
        if (cp == NULL)
        {
            result = SO_EINVAL;
            goto conclude;
        }

        bufferSize = ROUND_UP(strlen(cp) + 33, 32);
        args = SOiAlloc(12, bufferSize);
        if (args == NULL)
        {
            result = SO_ENOMEM;
            goto conclude;
        }

        stringBuffer = (char*)args + 32;
        if (cp != NULL)
        {
            strcpy(stringBuffer, cp);
        }
        result = IOS_Ioctl(rmId, NET_SO_INETATON, stringBuffer, strlen(cp), args, sizeof(SOInAddr));
        if (result >= 0 && inp != NULL)
        {
            memcpy(inp, args, sizeof(SOInAddr));
        }
        SOiFree(12, args, bufferSize);

    conclude:
        result = SOiConcludeTempRm(NULL, result, isTempRm);
    }
    return result;
}

char* SOInetNtoA(SOInAddr in)
{
    u8* addr = (u8*)&in;
    sprintf(addressBuffer, "%d.%d.%d.%d", addr[0], addr[1], addr[2], addr[3]);
    return addressBuffer;
}

u32 SONtoHl(u32 netlong)
{
    return netlong;
}

u16 SONtoHs(u16 netshort)
{
    return netshort;
}

u32 SOHtoNl(u32 hostlong)
{
    return hostlong;
}

u16 SOHtoNs(u16 hostshort)
{
    return hostshort;
}

static int RecvFrom(const char* funcName, int s, void* buf, int len, int flags,
    void* sockFrom)
{
    BOOL canUseBuffer;
    BOOL valid;
    BOOL direct;
    SOSockAddr* from = sockFrom;
    void* dataBuffer;
    int bufferSize;
    IPCIOVector* vectors;
    int* args;
    int result;
    SOSockAddr* addrBuffer;
    s32 rmId;

    result = SOiPrepare(funcName, &rmId);
    if (result == SO_SUCCESS)
    {
        if (from != NULL && (sizeof(SOSockAddr) < from->len || from->len < sizeof(SOSockAddr)))
        {
            result = SO_EINVAL;
            goto conclude;
        }
        if (len < 0 || (len > 0 && buf == NULL))
        {
            result = SO_EINVAL;
            goto conclude;
        }

        direct = TRUE;
        if (len != 0)
        {
            BOOL aligned = (canUseBuffer = FALSE);
            if ((u32)buf % 32 == 0 && len % 32 == 0)
            {
                aligned = TRUE;
            }
            if (aligned)
            {
                valid = TRUE;
                if (SOiIsBufferAddrCheck())
                {
                    BOOL inRange = ((u32)buf & 0x1fffffff) >= 0x10000000 && ((u32)buf & 0x1fffffff) < 0x18000000;
                    if (!inRange)
                    {
                        valid = FALSE;
                    }
                }
                if (valid)
                {
                    canUseBuffer = TRUE;
                }
            }
            if (!canUseBuffer)
            {
                direct = FALSE;
            }
        }

        bufferSize = ROUND_UP((from == NULL ? 0 : from->len) + 64, 32);
        vectors = SOiAlloc(12, bufferSize);
        if (direct == FALSE)
        {
            dataBuffer = SOiAlloc(13, ROUND_UP(len, 32));
        }
        else
        {
            dataBuffer = buf;
        }

        if (vectors == NULL || dataBuffer == NULL)
        {
            result = SO_ENOMEM;
            goto freeBuffers;
        }

        args = (int*)((u8*)vectors + 32);
        addrBuffer = (SOSockAddr*)((u8*)args + 32);
        args[0] = s;
        args[1] = flags;
        vectors[0].base = args;
        vectors[0].length = 8;
        vectors[1].base = dataBuffer;
        vectors[1].length = len;
        if (from == NULL)
        {
            vectors[2].base = NULL;
            vectors[2].length = 0;
            result = IOS_Ioctlv(rmId, NET_SO_RECVFROM, 1, 2, vectors);
        }
        else
        {
            memcpy(addrBuffer, from, from->len);
            vectors[2].base = addrBuffer;
            vectors[2].length = from->len;
            result = IOS_Ioctlv(rmId, NET_SO_RECVFROM, 1, 2, vectors);
            if (result >= 0)
            {
                memcpy(from, addrBuffer, from->len > addrBuffer->len ? addrBuffer->len : from->len);
            }
        }
        if (result >= 0 && !direct)
        {
            memcpy(buf, dataBuffer, len);
        }

    freeBuffers:
        if (!direct)
        {
            SOiFree(13, dataBuffer, ROUND_UP(len, 32));
        }
        SOiFree(12, vectors, bufferSize);

    conclude:
        result = SOiConclude(funcName, result);
    }
    return result;
}

static int SendTo(const char* funcName, int s, const void* buf, int len,
    int flags, const void* sockTo)
{
    BOOL canUseBuffer;
    BOOL direct;
    const SOSockAddr* to = sockTo;
    int result;
    s32 rmId;
    BOOL valid;

    result = SOiPrepare(funcName, &rmId);
    if (result == SO_SUCCESS)
    {
        void* dataBuffer;
        IPCIOVector* vectors;
        NETSoSendToArgs* args;

        if (to != NULL && (sizeof(SOSockAddr) < to->len || to->len < sizeof(SOSockAddr)))
        {
            result = SO_EINVAL;
            goto conclude;
        }
        if (len < 0 || (len > 0 && buf == NULL))
        {
            result = SO_EINVAL;
            goto conclude;
        }

        direct = TRUE;
        if (len != 0)
        {
            BOOL aligned = (canUseBuffer = FALSE);
            if ((u32)buf % 32 == 0 && len % 32 == 0)
            {
                aligned = TRUE;
            }
            if (aligned)
            {
                valid = TRUE;
                if (SOiIsBufferAddrCheck())
                {
                    BOOL inRange = ((u32)buf & 0x1fffffff) >= 0x10000000 && ((u32)buf & 0x1fffffff) < 0x18000000;
                    if (!inRange)
                    {
                        valid = FALSE;
                    }
                }
                if (valid)
                {
                    canUseBuffer = TRUE;
                }
            }
            if (!canUseBuffer)
            {
                direct = FALSE;
            }
        }

        vectors = SOiAlloc(12, 96);
        if (direct == FALSE)
        {
            dataBuffer = SOiAlloc(14, ROUND_UP(len, 32));
        }
        else
        {
            dataBuffer = (void*)buf;
        }

        if (vectors == NULL || dataBuffer == NULL)
        {
            result = SO_ENOMEM;
            goto freeBuffers;
        }

        args = (NETSoSendToArgs*)((u8*)vectors + 32);
        args->s = s;
        args->flags = flags;
        if (to == NULL)
        {
            args->hasAddr = FALSE;
        }
        else
        {
            args->hasAddr = TRUE;
            memcpy(args->addr, to, to->len);
        }
        if (!direct)
        {
            memcpy(dataBuffer, buf, len);
        }

        vectors[0].base = dataBuffer;
        vectors[0].length = len;
        vectors[1].base = args;
        vectors[1].length = 40;
        result = IOS_Ioctlv(rmId, NET_SO_SENDTO, 2, 0, vectors);

    freeBuffers:
        if (!direct)
        {
            SOiFree(14, dataBuffer, ROUND_UP(len, 32));
        }
        SOiFree(12, vectors, 96);

    conclude:
        result = SOiConclude(funcName, result);
    }
    return result;
}
