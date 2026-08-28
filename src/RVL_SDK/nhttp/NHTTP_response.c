#include <private/nhttp.h>

void NHTTPi_free(void* ptr);
NHTTPConnectionInfo* NHTTPi_Response2Connection(void* systemInfo,
    NHTTPResponseInfo* response);
s32 NHTTPi_findNextLineHdrRecvBuf(NHTTPHdrRecvBuf* recvBuf, s32 start,
    s32 end, s32* separator, s32* lineBreakLength);
s32 NHTTPi_compareTokenN_HdrRecvBuf(NHTTPHdrRecvBuf* recvBuf, s32 start,
    s32 end, const char* token, s8 terminal);
s32 NHTTPi_skipSpaceHdrRecvBuf(NHTTPHdrRecvBuf* recvBuf, s32 start, s32 end);

void NHTTP_DestroyResponse(void* systemInfo, NHTTPResponseInfo* response)
{
    NHTTPRecvBufBlock* next;
    NHTTPConnectionInfo* connection;

    while (response->recvBuf.blocks != NULL)
    {
        next = response->recvBuf.blocks->next;
        NHTTPi_free(response->recvBuf.blocks);
        response->recvBuf.blocks = next;
    }

    if (response->recvBuf._unk20 != NULL)
    {
        NHTTPi_free(response->recvBuf._unk20);
    }
    if (response->recvBuf._unk24 != NULL)
    {
        NHTTPi_free(response->recvBuf._unk24);
    }
    if (response->recvBuf.cleanup != NULL)
    {
        response->recvBuf.cleanup(response->recvBuf.buffer, NHTTPi_free, response->userParam);
        response->recvBuf.buffer = NULL;
        response->recvBuf.bufferSize = 0;
    }

    connection = NHTTPi_Response2Connection(systemInfo, response);
    if (connection != NULL)
    {
        connection->response = NULL;
    }
    NHTTPi_free(response);
}

s32 NHTTPi_getHeaderValue(NHTTPHdrRecvBuf* recvBuf, const char* name,
    s32* valueOffset)
{
    s32 separator;
    s32 lineBreakLength = 0;
    s32 current;
    s32 next;
    s32 end;
    s32 valueStart;

    current = NHTTPi_findNextLineHdrRecvBuf(recvBuf, 12, recvBuf->length, &separator, &lineBreakLength);

    while (current > 0)
    {
        next = NHTTPi_findNextLineHdrRecvBuf(recvBuf, current, recvBuf->length, &separator, &lineBreakLength);
        if (separator > 0)
        {
            if (NHTTPi_compareTokenN_HdrRecvBuf(recvBuf, current, separator, name, 0)
                == 0)
            {
                if (separator + 1 < recvBuf->length)
                {
                    end = NHTTPi_findNextLineHdrRecvBuf(recvBuf, separator + 1, recvBuf->length, NULL, &lineBreakLength);
                    if (end <= 0)
                    {
                        end = recvBuf->length;
                    }
                    else
                    {
                        if (end < lineBreakLength)
                        {
                            return -1;
                        }
                        end -= lineBreakLength;
                    }

                    valueStart = NHTTPi_skipSpaceHdrRecvBuf(recvBuf,
                        separator + 1,
                        end);
                    if (valueStart < 0)
                    {
                        valueStart = end;
                    }
                    *valueOffset = valueStart;
                    return end - valueStart;
                }
                return 0;
            }
        }

        current = next;
    }

    return -1;
}
