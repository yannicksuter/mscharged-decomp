#include <revolution/net/NETDigest.h>

#include <revolution/os.h>

#include <string.h>

void NETHMACInit(NETHMACContext* context, const NETHashInterface* interface,
    const void* key, u32 keyLength)
{
    u8 innerPad[64];
    u32 i;
    u32 actualKeyLength;
    void* hashContext = context->hashContext;

    context->interface = *interface;

    if (interface->contextSize > sizeof(context->hashContext) || interface->blockSize > sizeof(context->key))
    {
        OSReport("%s(%d):[warning in %s]", "hmac.c", 100, "NETHMACInit");
        OSReport("specified interface needs too large work memory.");
        OSReport("\n");
        return;
    }

    if (keyLength <= interface->blockSize)
    {
        memcpy(context->key, key, keyLength);
        context->keyLength = keyLength;
    }
    else
    {
        context->interface.init(hashContext);
        context->interface.update(hashContext, key, keyLength);
        context->interface.getDigest(hashContext, context->key);
        context->keyLength = context->interface.digestSize;
    }

    actualKeyLength = context->keyLength;
    for (i = 0; i < actualKeyLength; i++)
    {
        innerPad[i] = context->key[i] ^ 0x36;
    }
    memset(&innerPad[actualKeyLength], 0x36, context->interface.blockSize - actualKeyLength);

    context->interface.init(hashContext);
    context->interface.update(hashContext, innerPad, context->interface.blockSize);
}

void NETHMACUpdate(NETHMACContext* context, const void* input, u32 length)
{
    context->interface.update(context->hashContext, input, length);
}

void NETHMACGetDigest(NETHMACContext* context, void* digest)
{
    u8 innerDigest[64];
    u8 outerPad[64];
    u32 i;
    u32 actualKeyLength;
    void* hashContext = context->hashContext;

    context->interface.getDigest(hashContext, innerDigest);

    actualKeyLength = context->keyLength;
    for (i = 0; i < actualKeyLength; i++)
    {
        outerPad[i] = context->key[i] ^ 0x5C;
    }
    memset(&outerPad[actualKeyLength], 0x5C, context->interface.blockSize - actualKeyLength);

    context->interface.init(hashContext);
    context->interface.update(hashContext, outerPad, context->interface.blockSize);
    context->interface.update(hashContext, innerDigest, context->interface.digestSize);
    context->interface.getDigest(hashContext, digest);
}
