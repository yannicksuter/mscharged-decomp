#include <private/nhttp.h>

#include <revolution/so.h>
#include <revolution/ssl.h>

void NHTTPi_lockReqList(void* mutexInfo);
void NHTTPi_unlockReqList(void* mutexInfo);
NHTTPConnectionInfo* NHTTPi_Request2Connection(void* mutexInfo,
    NHTTPRequestInfo* request);
void NHTTPi_SetSSLError(NHTTPBgnEndInfo* info, s32 error);
void* NHTTPi_memcpy(void* destination, const void* source, u32 size);
void* NHTTPi_memclr(void* destination, u32 size);

s32 NHTTPi_SocSSLConnect(NHTTPBgnEndInfo* info, void* mutexInfo,
    NHTTPRequestInfo* request, s32 socket);

s32 NHTTPi_SocOpen(NHTTPRequestInfo* request)
{
    s32 socket;
    s32 recvBufferSize;

    socket = SOSocket2(SO_PF_INET, SO_SOCK_STREAM, 0);
    recvBufferSize = 0;
    if (request != NULL)
    {
        recvBufferSize = request->recvBufferSize;
    }
    if (socket >= 0 && recvBufferSize != 0)
    {
        SOSetSockOpt(socket, SO_SOL_SOCKET, SO_SO_RCVBUF, &recvBufferSize, sizeof(recvBufferSize));
    }
    return socket;
}

s32 NHTTPi_SocClose(void* mutexInfo, NHTTPRequestInfo* request, s32 socket)
{
    NHTTPi_lockReqList(mutexInfo);
    if (request->sslId > 0)
    {
        SSLShutdown(request->sslId);
        request->sslId = -1;
    }
    NHTTPi_unlockReqList(mutexInfo);
    return SOClose(socket);
}

s32 NHTTPi_SocConnect(NHTTPBgnEndInfo* info, void* mutexInfo,
    NHTTPRequestInfo* request, s32 socket, u32 address, u32 port)
{
    SOSockAddrIn sockAddr;
    s32 result;

    sockAddr.len = sizeof(sockAddr);
    sockAddr.family = SO_PF_INET;
    sockAddr.port = SOHtoNs((u16)port);
    sockAddr.addr.addr = address;
    result = SOConnect(socket, &sockAddr);
    if (result < 0)
    {
        result = -1001;
        if (request->cancel != FALSE)
        {
            result = -1002;
        }
    }
    else if (request->secure != FALSE && request->proxyEnabled == FALSE)
    {
        result = NHTTPi_SocSSLConnect(info, mutexInfo, request, socket);
    }
    else
    {
        result = 0;
    }
    return result;
}

s32 NHTTPi_SocSSLConnect(NHTTPBgnEndInfo* info, void* mutexInfo,
    NHTTPRequestInfo* request, s32 socket)
{
    BOOL complete = FALSE;
    NHTTPConnectionInfo* connection;
    s32 result;

    request->sslId = SSLNew(request->verifyOption, request->host);
    if (request->clientCertDefault == TRUE)
    {
        SSLSetClientCertDefault(request->sslId);
    }
    else if (request->clientCertData != NULL && request->privateKeyData != NULL)
    {
        SSLSetClientCert(request->sslId, request->clientCertData, request->clientCertSize, request->privateKeyData, request->privateKeySize);
    }
    if (request->rootCAData != NULL)
    {
        SSLSetRootCA(request->sslId, request->rootCAData, request->rootCASize);
    }
    else
    {
        SSLSetRootCADefault(request->sslId);
    }
    result = SSLConnect(request->sslId, socket);
    if (result < -1)
    {
        return -1001;
    }

    while (!complete)
    {
        connection = NHTTPi_Request2Connection(mutexInfo, request);
        result = SSLDoHandshake(request->sslId);
        NHTTPi_SetSSLError(info, result);
        if (connection != NULL)
        {
            connection->sslError = result;
        }

        switch (result)
        {
        case 0:
            complete = TRUE;
            break;
        case -7:
        case -3:
        case -2:
            break;
        default:
            return -1001;
        }
    }
    return 0;
}

s32 NHTTPi_SocRecv_sub(s32 socket, char* o_buf, u32 i_bufSize, s32 flags)
{
    char tmp[32] ALIGN(32);
    u32 align;
    u32 front_bytes;
    s32 rest;
    int total_read;
    s32 ret;

    align = (u32)o_buf & 31;
    front_bytes = align != 0 ? (32 - align) : 0;
    rest = i_bufSize;
    total_read = 0;
    NHTTPi_memclr(tmp, sizeof(tmp));
    if (front_bytes != 0)
    {
        if (front_bytes > i_bufSize)
        {
            front_bytes = i_bufSize;
        }
        ret = SORecv(socket, tmp, front_bytes, flags);
        if (ret > 0)
        {
            total_read += ret;
            NHTTPi_memcpy(o_buf, tmp, ret);
            if (ret < front_bytes)
            {
                return total_read;
            }
            else
            {
                o_buf += ret;
                rest -= ret;
            }
        }
        else
        {
            return ret;
        }
    }

    if (rest > 0)
    {
        u32 body_bytes = rest & ~31;
        if (body_bytes != 0)
        {
            ret = SORecv(socket, o_buf, body_bytes, flags);
            if (ret > 0)
            {
                total_read += ret;
                if (ret < body_bytes)
                {
                    return total_read;
                }
                else
                {
                    o_buf += ret;
                    rest -= ret;
                }
            }
            else
            {
                if (total_read > 0)
                {
                    return total_read;
                }
                else
                {
                    return ret;
                }
            }
        }
    }
    if (rest > 0)
    {
        u32 back_bytes = rest & 31;
        if (back_bytes != 0)
        {
            NHTTPi_memclr(tmp, sizeof(tmp));
            ret = SORecv(socket, tmp, back_bytes, flags);
            if (ret > 0)
            {
                total_read += ret;
                NHTTPi_memcpy(o_buf, tmp, ret);
            }
            else
            {
                if (total_read > 0)
                {
                    return total_read;
                }
                else
                {
                    return ret;
                }
            }
        }
    }
    return total_read;
}

s32 NHTTPi_SocRecv(NHTTPRequestInfo* request, s32 socket, char* buffer,
    u32 bufferSize, s32 flags)
{
    s32 result;

    if (request->sslId > 0)
    {
        result = SSLRead(request->sslId, buffer, bufferSize);
    }
    else
    {
        result = NHTTPi_SocRecv_sub(socket, buffer, bufferSize, flags);
    }
    if (result < 0)
    {
        if (request->cancel != FALSE)
        {
            return -1002;
        }
        if (request->sslId > 0)
        {
            if (result == -7 || result == -6)
            {
                return 0;
            }
        }
        else if (result == SO_ENOTCONN)
        {
            return 0;
        }
        return -1001;
    }
    return result;
}

s32 NHTTPi_SocSend_sub(s32 socket, const char* i_buf, u32 i_bufSize,
    s32 flags)
{
    s32 ret;
    char tmp[32] ALIGN(32);
    u32 align;
    u32 front_bytes;
    s32 rest;
    int total_write;

    align = (u32)i_buf & 31;
    front_bytes = align != 0 ? (32 - align) : 0;
    rest = i_bufSize;
    total_write = 0;
    NHTTPi_memclr(tmp, sizeof(tmp));
    if (front_bytes != 0)
    {
        if (front_bytes > i_bufSize)
        {
            front_bytes = i_bufSize;
        }
        NHTTPi_memcpy(tmp, i_buf, front_bytes);
        ret = SOSend(socket, tmp, front_bytes, flags);
        if (ret > 0)
        {
            total_write += ret;
            if (ret < front_bytes)
            {
                return ret;
            }
            else
            {
                i_buf += ret;
                rest -= ret;
            }
        }
        else
        {
            return ret;
        }
    }
    if (rest > 0)
    {
        u32 body_bytes = rest & ~31;
        if (body_bytes != 0)
        {
            ret = SOSend(socket, i_buf, body_bytes, flags);
            if (ret > 0)
            {
                total_write += ret;
                if (ret < body_bytes)
                {
                    return total_write;
                }
                else
                {
                    i_buf += ret;
                    rest -= ret;
                }
            }
            else
            {
                if (total_write > 0)
                {
                    return total_write;
                }
                else
                {
                    return ret;
                }
            }
        }
    }
    if (rest > 0)
    {
        u32 back_bytes = rest & 31;
        if (back_bytes != 0)
        {
            NHTTPi_memclr(tmp, sizeof(tmp));
            NHTTPi_memcpy(tmp, i_buf, back_bytes);
            ret = SOSend(socket, tmp, back_bytes, flags);
            if (ret > 0)
            {
                total_write += ret;
            }
            else
            {
                if (total_write > 0)
                {
                    return total_write;
                }
                else
                {
                    return ret;
                }
            }
        }
    }
    return total_write;
}

s32 NHTTPi_SocSend(NHTTPRequestInfo* request, s32 socket,
    const char* buffer, u32 bufferSize, s32 flags)
{
    s32 result;

    if (request->sslId > 0)
    {
        result = SSLWrite(request->sslId, buffer, bufferSize);
    }
    else
    {
        result = NHTTPi_SocSend_sub(socket, buffer, bufferSize, flags);
    }
    if (result < 0)
    {
        if (request->cancel != FALSE)
        {
            return -1002;
        }
        if (request->sslId > 0)
        {
            if (result == -7 || result == -6)
            {
                return 0;
            }
        }
        else if (result == SO_ENOTCONN)
        {
            return 0;
        }
        return -1001;
    }
    return result;
}

void NHTTPi_SocCancel(void* mutexInfo, NHTTPRequestInfo* request, s32 socket)
{
    NHTTPi_lockReqList(mutexInfo);
    if (socket >= 0)
    {
        SOShutdown(socket, 2);
    }
    NHTTPi_unlockReqList(mutexInfo);
}

u32 NHTTPi_resolveHostname(NHTTPRequestInfo* request, const char* hostname)
{
    u32 address;
    SOAddrInfo* result;

    address = 0;
    if (SOGetAddrInfo(hostname, NULL, NULL, &result) == 0)
    {
        NHTTPi_memcpy(&address, (u8*)result->addr + 4, sizeof(address));
        SOFreeAddrInfo(result);
    }
    else
    {
        address = 0;
    }
    return address;
}
