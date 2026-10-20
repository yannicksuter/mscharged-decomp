#ifndef REVOLUTION_SSL_H
#define REVOLUTION_SSL_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <revolution/types.h>

    typedef int SSLResult;
    typedef int SSLSocket;
    typedef int SSLId;

#define SSL_RESULT_OK      0
#define SSL_RESULT_INVALID -1

#if defined(VERSION_R4QK01)
#define SSL_EWANT_READ   -2
#define SSL_EWANT_WRITE  -3
#define SSL_EZERO_RETURN -6
#endif

#define SSL_MAX 8

#define SSL_CLIENT_CERT_LENGTH 4096
#define SSL_ROOT_CA_LENGTH     4096
#define SSL_PRIVATE_KEY_LENGTH 4096

#define SSL_BUFFER_MAX_LENGTH  32768
#define SSL_SERVER_NAME_LENGTH 256

    SSLId SSLNew(u32 verifyOption, const char* serverName);
    SSLResult SSLConnect(SSLId sslId, SSLSocket socket);
    SSLResult SSLShutdown(SSLId sslId);
    SSLResult SSLDoHandshake(SSLId sslId);
    SSLResult SSLRead(SSLId sslId, char* buf, u32 bufSize);
    SSLResult SSLWrite(SSLId sslId, const char* buf, u32 bufSize);
    SSLResult SSLSetClientCert(SSLId sslId, const char* clientCertData, u32 clientCertSize,
        const char* privateKeyData, u32 privateKeySize);
    SSLResult SSLSetClientCertDefault(SSLId sslId);
    SSLResult SSLSetRootCA(SSLId sslId, const char* rootCAData, u32 rootCASize);
    SSLResult SSLSetRootCADefault(SSLId sslId);
#if defined(VERSION_R4QK01)
    SSLResult SSLSetBuiltinRootCA(SSLId sslId, u32 rootCAId);
    SSLResult SSLSetBuiltinClientCert(SSLId sslId, u32 clientCertId);
#endif

#ifdef __cplusplus
}
#endif

#endif // REVOLUTION_SSL_H
