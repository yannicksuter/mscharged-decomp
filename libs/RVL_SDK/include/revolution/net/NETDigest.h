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

    static inline void NETWriteSwappedBytes32(u32* dst, u32 val)
    {
        __stwbrx(val, dst, 0);
    }

    static inline u32 NETSwapBytes32(u32 val)
    {
        return (val >> 24) | ((val >> 8) & 0xFF00) | ((val << 8) & 0xFF0000) |
               (val << 24);
    }

    static inline u32 NETReadSwappedBytes32(const u32* src)
    {
        return __lwbrx((void*)src, 0);
    }

    typedef void (*NETHashInitFunc)(void* context);
    typedef void (*NETHashUpdateFunc)(void* context, void* input, u32 length);
    typedef void (*NETHashGetDigestFunc)(void* context, void* digest);

    typedef struct NETHashInterface
    {
        u32 hashLength;
        u32 blockLength;
        u32 workLength;
        void* context;
        NETHashInitFunc Init;
        NETHashUpdateFunc Update;
        NETHashGetDigestFunc GetDigest;
    } NETHashInterface;

    typedef struct NETSHA1Context
    {
        u32 h[5];
        u8 block[64];
        u32 pool;
        u32 blocks_low;
        u32 blocks_high;
    } NETSHA1Context;

    typedef struct NETHMACContext
    {
        NETHashInterface hashInterface[1];
        u32 keyLength;
        union
        {
            NETMD5Context md5;
            NETSHA1Context sha1;
            u8 buffer[1];
        } hashContext;
        union
        {
            u8 md5[64];
            u8 sha1[64];
            u8 buffer[1];
        } key;
        u8 padding[8];
    } NETHMACContext;

    void NETMD5Init(NETMD5Context* context);
    void NETMD5Update(NETMD5Context* context, void* input, u32 length);
    void NETMD5GetDigest(NETMD5Context* context, void* digest);
    const NETHashInterface* NETGetMD5Interface(void);

    void NETHMACInit(NETHMACContext* context, const NETHashInterface* hashInterface,
        void* key, u32 keyLength);
    void NETHMACUpdate(NETHMACContext* context, void* message, u32 length);
    void NETHMACGetDigest(NETHMACContext* context, void* digest);

#ifdef __cplusplus
}
#endif

#endif
