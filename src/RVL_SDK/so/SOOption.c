#include <revolution/so.h>

#include <revolution/ipc.h>

#include <string.h>

typedef struct SOGetSockOptArgs
{
    s32 socket;
    s32 level;
    s32 option;
    s32 length;
    u8 value[8];
} SOGetSockOptArgs;

typedef struct SOSetSockOptArgs
{
    s32 socket;
    s32 level;
    s32 option;
    s32 length;
    u8 value[20];
} SOSetSockOptArgs;

int SOGetSockOpt(int s, int level, int optname, void* optval, int* optlen)
{
    int result;
    s32 rmId;
    SOGetSockOptArgs* args;

    result = SOiPrepare(NULL, &rmId);
    if (result == SO_SUCCESS)
    {
        args = SOiAlloc(12, 32);
        if (args == NULL)
        {
            result = SO_ENOMEM;
        }
        else
        {
            args->socket = s;
            args->level = level;
            args->option = optname;
            result = IOS_Ioctl(rmId, 8, NULL, 0, args, 24);
            if (result >= 0 && optlen != NULL)
            {
                if (*optlen >= args->length)
                {
                    if (optval != NULL)
                    {
                        memcpy(optval, args->value, args->length);
                    }
                    *optlen = args->length;
                }
                else
                {
                    *optlen = args->length;
                    result = SO_EINVAL;
                }
            }
            SOiFree(12, args, 32);
        }
        result = SOiConclude(NULL, result);
    }
    return result;
}

int SOSetSockOpt(int s, int level, int optname, const void* optval, int optlen)
{
    int result;
    s32 rmId;
    SOSetSockOptArgs* args;

    result = SOiPrepare(NULL, &rmId);
    if (result == SO_SUCCESS)
    {
        if (optlen < 0 || optlen > 20)
        {
            result = SO_EINVAL;
        }
        else
        {
            args = SOiAlloc(12, 64);
            if (args == NULL)
            {
                result = SO_ENOMEM;
            }
            else
            {
                args->socket = s;
                args->level = level;
                args->option = optname;
                args->length = optlen;
                if (optval != NULL)
                {
                    memcpy(args->value, optval, optlen);
                }
                else
                {
                    memset(args->value, 0, optlen);
                }
                result = IOS_Ioctl(rmId, 9, args, 36, NULL, 0);
                SOiFree(12, args, 64);
            }
        }
        result = SOiConclude(NULL, result);
    }
    return result;
}

int SOGetInterfaceOpt(IPInterface* unk, int level, int optname,
    void* optval, int* optlen)
{
    int result;
    s32 rmId;
    int isTempRm;
    int length;
    int bufferSize;
    IPCIOVector* vectors;
    s32* option;
    s32* returnedLength;
    u8* value;

    (void)unk;

    result = SOiPrepareTempRm(NULL, &rmId, &isTempRm);
    if (result == SO_SUCCESS)
    {
        if (optname == SO_CONFIG_FILTER_INPUT || optname == SO_CONFIG_FILTER_OUTPUT)
        {
            result = SO_EINVAL;
        }
        else
        {
            length = optlen == NULL || *optlen < 0 ? 0 : *optlen;
            bufferSize = OSRoundUp32B(length + 96);
            vectors = SOiAlloc(12, bufferSize);
            if (vectors == NULL)
            {
                result = SO_ENOMEM;
            }
            else
            {
                option = (s32*)((u8*)vectors + 32);
                returnedLength = (s32*)((u8*)option + 32);
                value = (u8*)returnedLength + 32;

                option[0] = level;
                option[1] = optname;
                *returnedLength = optlen == NULL || *optlen < 0 ? 0 : *optlen;

                vectors[0].base = option;
                vectors[0].length = 8;
                vectors[1].base = value;
                vectors[1].length = optlen == NULL || *optlen < 0 ? 0 : *optlen;
                vectors[2].base = returnedLength;
                vectors[2].length = 4;

                result = IOS_Ioctlv(rmId, 28, 1, 2, vectors);
                if (result >= 0 && optlen != NULL)
                {
                    if (*optlen >= *returnedLength)
                    {
                        if (optval != NULL)
                        {
                            memcpy(optval, value, *returnedLength);
                        }
                        *optlen = *returnedLength;
                    }
                    else
                    {
                        *optlen = *returnedLength;
                        result = SO_EINVAL;
                    }
                }
                SOiFree(12, vectors, bufferSize);
            }
        }
        result = SOiConcludeTempRm(NULL, result, isTempRm);
    }
    return result;
}
