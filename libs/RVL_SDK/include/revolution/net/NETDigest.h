#ifndef REVOLUTION_NET_DIGEST_H
#define REVOLUTION_NET_DIGEST_H

#include <revolution/types.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct NETMD5Context
    {
        union
        {
            struct
            {
                u32 a;
                u32 b;
                u32 c;
                u32 d;
            };
            u32 state[4];
        };
        u64 length;
        union
        {
            u32 buffer32[16];
            u8 buffer8[64];
        };
    } NETMD5Context;

#define NET_MD5_DIGEST_SIZE 16

    typedef u8 NETMD5Sum[NET_MD5_DIGEST_SIZE];

    typedef void (*NETHashInitFunc)(void* context);
    typedef void (*NETHashUpdateFunc)(void* context, const void* input, u32 length);
    typedef void (*NETHashGetDigestFunc)(void* context, void* digest);

    typedef struct NETHashInterface
    {
        u32 digestSize;
        u32 blockSize;
        u32 contextSize;
        u32 reserved;
        NETHashInitFunc init;
        NETHashUpdateFunc update;
        NETHashGetDigestFunc getDigest;
    } NETHashInterface;

    typedef struct NETHMACContext
    {
        NETHashInterface interface;
        u32 keyLength;
        u8 hashContext[0x60];
        u8 key[0x40];
    } NETHMACContext;

    void NETMD5Init(NETMD5Context* context);
    void NETMD5Update(NETMD5Context* context, const void* input, u32 length);
    void NETMD5GetDigest(NETMD5Context* context, void* digest);
    const NETHashInterface* NETGetMD5Interface(void);

    void NETHMACInit(NETHMACContext* context, const NETHashInterface* interface,
        const void* key, u32 keyLength);
    void NETHMACUpdate(NETHMACContext* context, const void* input, u32 length);
    void NETHMACGetDigest(NETHMACContext* context, void* digest);

#ifdef __cplusplus
}
#endif

#endif
