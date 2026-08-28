#include <private/nhttp.h>

void* NHTTPi_memcpy(void* destination, const void* source, u32 size);
s32 NHTTPi_SocRecv(NHTTPRequestInfo* request, s32 socket, void* buffer,
    s32 length, s32 flags);

s32 NHTTPi_findNextLineHdrRecvBuf(NHTTPHdrRecvBuf* recvBuf, s32 start,
    s32 end, s32* separator, s32* lineBreakLength)
{
    NHTTPRecvBufBlock* block;
    s32 blockOffset;
    s32 i;
    s32 result;
    BOOL foundCR;
    s8 c;

    if (separator != NULL)
    {
        *separator = -1;
    }
    if (start < end)
    {
        result = -1;
        foundCR = FALSE;

        if (start < 0x400)
        {
            blockOffset = start;
            block = NULL;
        }
        else
        {
            block = recvBuf->blocks;
            i = (start - 0x400) >> 9;
            while (i-- != 0)
            {
                block = block->next;
            }
            blockOffset = (start - 0x400) & 0x1FF;
        }

        for (i = start; i < end; i++)
        {
            if (block == NULL)
            {
                if (blockOffset < 0x400)
                {
                    c = recvBuf->data[blockOffset++];
                }
                else
                {
                    block = recvBuf->blocks;
                    blockOffset = 0;
                    c = block->data[blockOffset++];
                }
            }
            else
            {
                if (blockOffset == 0x200)
                {
                    blockOffset = 0;
                    block = block->next;
                }
                c = block->data[blockOffset++];
            }

            if (c == ':' && separator != NULL && *separator < 0)
            {
                *separator = i;
            }

            if (foundCR)
            {
                if (c == '\n')
                {
                    result = i == end - 1 ? 0 : i + 1;
                    if (lineBreakLength != NULL)
                    {
                        *lineBreakLength = 2;
                    }
                }
                return result;
            }

            if (c == '\r')
            {
                result = i == end - 1 ? 0 : i + 1;
                foundCR = TRUE;
                if (lineBreakLength != NULL)
                {
                    *lineBreakLength = 1;
                }
            }

            if (c == '\n')
            {
                result = i == end - 1 ? 0 : i + 1;
                if (lineBreakLength != NULL)
                {
                    *lineBreakLength = 1;
                }
                return result;
            }
        }
    }

    return -1;
}

s32 NHTTPi_skipSpaceHdrRecvBuf(NHTTPHdrRecvBuf* recvBuf, s32 start, s32 end)
{
    NHTTPRecvBufBlock* block;
    s32 blockOffset;
    s32 i;
    s8 c;

    if (start < end)
    {
        if (start < 0x400)
        {
            blockOffset = start;
            block = NULL;
        }
        else
        {
            block = recvBuf->blocks;
            i = (start - 0x400) >> 9;
            while (i-- != 0)
            {
                block = block->next;
            }
            blockOffset = (start - 0x400) & 0x1FF;
        }

        for (i = start; i < end; i++)
        {
            if (block == NULL)
            {
                if (blockOffset < 0x400)
                {
                    c = recvBuf->data[blockOffset++];
                }
                else
                {
                    block = recvBuf->blocks;
                    blockOffset = 0;
                    c = block->data[blockOffset++];
                }
            }
            else
            {
                if (blockOffset == 0x200)
                {
                    blockOffset = 0;
                    block = block->next;
                }
                c = block->data[blockOffset++];
            }

            if (c != ' ')
            {
                return i;
            }
        }
    }

    return -1;
}

s32 NHTTPi_compareTokenN_HdrRecvBuf(NHTTPHdrRecvBuf* recvBuf, s32 start,
    s32 end, const char* token, s8 terminal)
{
    NHTTPRecvBufBlock* block;
    s32 blockOffset;
    s32 i;
    s32 tokenChar;
    s32 recvChar;

    if (start < end)
    {
        if (start < 0x400)
        {
            blockOffset = start;
            block = NULL;
        }
        else
        {
            block = recvBuf->blocks;
            i = (start - 0x400) >> 9;
            while (i-- != 0)
            {
                block = block->next;
            }
            blockOffset = (start - 0x400) & 0x1FF;
        }

        if (block == NULL)
        {
            if (blockOffset < 0x400)
            {
                recvChar = recvBuf->data[blockOffset++];
            }
            else
            {
                block = recvBuf->blocks;
                blockOffset = 0;
                recvChar = block->data[blockOffset++];
            }
        }
        else
        {
            if (blockOffset == 0x200)
            {
                blockOffset = 0;
                block = block->next;
            }
            recvChar = block->data[blockOffset++];
        }

        i = start;
        while ((((recvChar >= 'A') & (recvChar <= 'Z'))
                       ? recvChar + ('a' - 'A')
                       : recvChar)
               == (((((s8)*token >= 'A') & ((s8)*token <= 'Z'))
                        ? (s8)*token + ('a' - 'A')
                        : (s8)*token)))
        {
            tokenChar = (s8)*token;
            if (tokenChar == '\0' || tokenChar == ' ' || tokenChar == terminal
                || i == end - 1)
            {
                return 0;
            }

            if (block == NULL)
            {
                if (blockOffset < 0x400)
                {
                    recvChar = recvBuf->data[blockOffset++];
                }
                else
                {
                    block = recvBuf->blocks;
                    blockOffset = 0;
                    recvChar = block->data[blockOffset++];
                }
            }
            else
            {
                if (blockOffset == 0x200)
                {
                    blockOffset = 0;
                    block = block->next;
                }
                recvChar = block->data[blockOffset++];
            }

            i++;
            token++;
        }
    }

    return -1;
}

BOOL NHTTPi_loadFromHdrRecvBuf(NHTTPHdrRecvBuf* recvBuf, u8* destination,
    s32 offset, s32 length)
{
    NHTTPRecvBufBlock* block;
    s32 blockOffset;
    s32 blockCount;
    s32 copyLength;

    if (offset + length <= recvBuf->length)
    {
        if (length != 0)
        {
            if (offset < 0x400)
            {
                copyLength = length;
                if (copyLength > 0x400 - offset)
                {
                    copyLength = 0x400 - offset;
                }
                NHTTPi_memcpy(destination, &recvBuf->data[offset], copyLength);
                offset += copyLength;
                length -= copyLength;
                destination += copyLength;
            }

            if (length != 0)
            {
                blockOffset = offset - 0x400;
                block = recvBuf->blocks;
                blockCount = blockOffset >> 9;
                blockOffset &= 0x1FF;
                while (blockCount-- != 0)
                {
                    block = block->next;
                }

                while (length != 0)
                {
                    copyLength = length;
                    if (copyLength > 0x200 - blockOffset)
                    {
                        copyLength = 0x200 - blockOffset;
                    }
                    NHTTPi_memcpy(destination, &block->data[blockOffset], copyLength);
                    blockOffset += copyLength;
                    block = block->next;
                    blockOffset &= 0x1FF;
                    length -= copyLength;
                    destination += copyLength;
                }
            }
        }
        return TRUE;
    }

    return FALSE;
}

BOOL NHTTPi_isRecvBufFull(NHTTPResponseInfo* response, u32 received)
{
    return response->recvBuf.bufferSize <= received;
}

s32 NHTTPi_RecvBuf(NHTTPRequestInfo* request, s32 socket, s32 offset,
    s32 flags)
{
    NHTTPResponseInfo* response = request->response;
    return NHTTPi_SocRecv(request, socket, (u8*)response->recvBuf.buffer + offset, response->recvBuf.bufferSize - offset, flags);
}

s32 NHTTPi_RecvBufN(NHTTPRequestInfo* request, s32 socket, u32 offset,
    s32 length, s32 flags)
{
    s32 remaining;

    if (NHTTPi_isRecvBufFull(request->response, offset))
    {
        return -1003;
    }

    remaining = request->response->recvBuf.bufferSize - offset;
    if (length > remaining)
    {
        length = remaining;
    }
    return NHTTPi_SocRecv(request, socket, (u8*)request->response->recvBuf.buffer + offset, length, flags);
}
