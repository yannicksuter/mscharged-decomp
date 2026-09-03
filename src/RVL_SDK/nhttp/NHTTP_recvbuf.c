#include <private/nhttp.h>

void* NHTTPi_memcpy(void* destination, const void* source, u32 size);
s32 NHTTPi_SocRecv(NHTTPRequestInfo* request, s32 socket, void* buffer,
    s32 length, s32 flags);

s32 NHTTPi_findNextLineHdrRecvBuf(NHTTPResponseInfo* recvBuf, s32 start,
    s32 end, s32* separator, s32* lineBreakLength)
{
    s32 c;
    NHTTPi_HDRBUFLIST* block;
    s32 blockOffset;
    s32 i;
    s32 result;
    BOOL foundCR;

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
            block = recvBuf->hdrBufBlock_p;
            i = (start - 0x400) >> 9;
            while (i-- != 0)
            {
                block = block->next_p;
            }
            blockOffset = (start - 0x400) & 0x1FF;
        }

        for (i = start; i < end; i++)
        {
            if (block == NULL)
            {
                if (blockOffset < 0x400)
                {
                    c = (s8)recvBuf->hdrBufFirst[blockOffset++];
                    goto check_character;
                }

                block = recvBuf->hdrBufBlock_p;
                blockOffset = 0;
            }
            else if (blockOffset == 0x200)
            {
                blockOffset = 0;
                block = block->next_p;
            }
            c = block->block[blockOffset++];

        check_character:
            if ((s8)c == ':' && separator != NULL && *separator < 0)
            {
                *separator = i;
            }

            if (foundCR)
            {
                if ((s8)c == '\n')
                {
                    result = i == end - 1 ? 0 : i + 1;
                    if (lineBreakLength != NULL)
                    {
                        *lineBreakLength = 2;
                    }
                }
                return result;
            }

            if ((s8)c == '\r')
            {
                result = i == end - 1 ? 0 : i + 1;
                foundCR = TRUE;
                if (lineBreakLength != NULL)
                {
                    *lineBreakLength = 1;
                }
            }

            if ((s8)c == '\n')
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

s32 NHTTPi_skipSpaceHdrRecvBuf(
    NHTTPResponseInfo* recvBuf, s32 start, s32 end)
{
    s32 blockOffset;
    NHTTPi_HDRBUFLIST* block;
    s32 i;
    s32 c;

    if (start < end)
    {
        if (start < 0x400)
        {
            blockOffset = start;
            block = NULL;
        }
        else
        {
            block = recvBuf->hdrBufBlock_p;
            i = (start - 0x400) >> 9;
            while (i-- != 0)
            {
                block = block->next_p;
            }
            blockOffset = (start - 0x400) & 0x1FF;
        }

        for (i = start; i < end; i++)
        {
            if (block == NULL)
            {
                if (blockOffset < 0x400)
                {
                    c = (s8)recvBuf->hdrBufFirst[blockOffset++];
                    goto check_space;
                }

                block = recvBuf->hdrBufBlock_p;
                blockOffset = 0;
            }
            else if (blockOffset == 0x200)
            {
                blockOffset = 0;
                block = block->next_p;
            }
            c = block->block[blockOffset++];

        check_space:
            if ((s8)c != ' ')
            {
                return i;
            }
        }
    }

    return -1;
}

s32 NHTTPi_compareTokenN_HdrRecvBuf(NHTTPResponseInfo* recvBuf, s32 start,
    s32 end, const char* token, s8 terminal)
{
    NHTTPi_HDRBUFLIST* block;
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
            block = recvBuf->hdrBufBlock_p;
            i = (start - 0x400) >> 9;
            while (i-- != 0)
            {
                block = block->next_p;
            }
            blockOffset = (start - 0x400) & 0x1FF;
        }

        if (block == NULL)
        {
            if (blockOffset < 0x400)
            {
                recvChar = (s8)recvBuf->hdrBufFirst[blockOffset++];
                goto compare_characters;
            }

            block = recvBuf->hdrBufBlock_p;
            blockOffset = 0;
        }
        else if (blockOffset == 0x200)
        {
            blockOffset = 0;
            block = block->next_p;
        }
        recvChar = block->block[blockOffset++];

    compare_characters:
        i = start;
        while (((((s8)recvChar >= 'A') & ((s8)recvChar <= 'Z'))
                       ? (s8)recvChar + ('a' - 'A')
                       : (s8)recvChar)
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
                    recvChar = (s8)recvBuf->hdrBufFirst[blockOffset++];
                    goto advance_token;
                }

                block = recvBuf->hdrBufBlock_p;
                blockOffset = 0;
            }
            else if (blockOffset == 0x200)
            {
                blockOffset = 0;
                block = block->next_p;
            }
            recvChar = block->block[blockOffset++];

        advance_token:
            i++;
            token++;
        }
    }

    return -1;
}

BOOL NHTTPi_loadFromHdrRecvBuf(NHTTPResponseInfo* recvBuf, u8* destination,
    s32 offset, s32 length)
{
    s32 copyLength;
    NHTTPi_HDRBUFLIST* block;
    s32 blockOffset;
    s32 blockCount;

    if (offset + length <= recvBuf->headerLen)
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
                NHTTPi_memcpy(
                    destination, &recvBuf->hdrBufFirst[offset], copyLength);
                offset += copyLength;
                length -= copyLength;
                destination += copyLength;
            }

            if (length != 0)
            {
                blockOffset = offset - 0x400;
                block = recvBuf->hdrBufBlock_p;
                blockCount = blockOffset >> 9;
                blockOffset &= 0x1FF;
                while (blockCount-- != 0)
                {
                    block = block->next_p;
                }

                while (length != 0)
                {
                    copyLength = length;
                    if (copyLength > 0x200 - blockOffset)
                    {
                        copyLength = 0x200 - blockOffset;
                    }
                    NHTTPi_memcpy(
                        destination, &block->block[blockOffset], copyLength);
                    blockOffset += copyLength;
                    block = block->next_p;
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
    return response->recvBufLen <= received;
}

s32 NHTTPi_RecvBuf(NHTTPRequestInfo* request, s32 socket, s32 offset,
    s32 flags)
{
    NHTTPResponseInfo* response = request->response;
    return NHTTPi_SocRecv(request, socket, (u8*)response->recvBuf_p + offset,
        response->recvBufLen - offset, flags);
}

s32 NHTTPi_RecvBufN(NHTTPRequestInfo* request, s32 socket, u32 offset,
    s32 length, s32 flags)
{
    s32 remaining;

    if (NHTTPi_isRecvBufFull(request->response, offset))
    {
        return -1003;
    }

    remaining = request->response->recvBufLen - offset;
    return NHTTPi_SocRecv(request, socket,
        (u8*)request->response->recvBuf_p + offset,
        length > remaining ? remaining : length, flags);
}
