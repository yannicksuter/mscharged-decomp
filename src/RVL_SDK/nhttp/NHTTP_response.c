#include <private/nhttp.h>

void NHTTPi_free(void* ptr);
NHTTPConnectionInfo* NHTTPi_Response2Connection(void* systemInfo,
    NHTTPResponseInfo* response);
s32 NHTTPi_findNextLineHdrRecvBuf(NHTTPResponseInfo* recvBuf, s32 start,
    s32 end, s32* separator, s32* lineBreakLength);
s32 NHTTPi_compareTokenN_HdrRecvBuf(NHTTPResponseInfo* recvBuf, s32 start,
    s32 end, const char* token, s8 terminal);
s32 NHTTPi_skipSpaceHdrRecvBuf(
    NHTTPResponseInfo* recvBuf, s32 start, s32 end);

void NHTTP_DestroyResponse(void* systemInfo, NHTTPResponseInfo* response)
{
    NHTTPi_HDRBUFLIST* next_p;
    NHTTPConnectionInfo* connection;

    while (response->hdrBufBlock_p != NULL)
    {
        next_p = response->hdrBufBlock_p->next_p;
        NHTTPi_free(response->hdrBufBlock_p);
        response->hdrBufBlock_p = next_p;
    }

    if (response->allHeader_p != NULL)
    {
        NHTTPi_free(response->allHeader_p);
    }
    if (response->foundHeader_p != NULL)
    {
        NHTTPi_free(response->foundHeader_p);
    }
    if (response->freeBuf != NULL)
    {
        response->freeBuf(response->recvBuf_p, NHTTPi_free, response->param_p);
        response->recvBuf_p = NULL;
        response->recvBufLen = 0;
    }

    connection = NHTTPi_Response2Connection(systemInfo, response);
    if (connection != NULL)
    {
        connection->response = NULL;
    }
    NHTTPi_free(response);
}

s32 NHTTPi_getHeaderValue(NHTTPResponseInfo* recvBuf, const char* name,
    s32* valueOffset)
{
    s32 separator;
    s32 lineBreakLength = 0;
    s32 current;
    s32 next;
    s32 end;
    s32 valueStart;

    current = NHTTPi_findNextLineHdrRecvBuf(
        recvBuf, 12, recvBuf->headerLen, &separator, &lineBreakLength);

    while (current > 0)
    {
        next = NHTTPi_findNextLineHdrRecvBuf(recvBuf, current,
            recvBuf->headerLen, &separator, &lineBreakLength);
        if (separator > 0)
        {
            if (NHTTPi_compareTokenN_HdrRecvBuf(recvBuf, current, separator, name, 0)
                == 0)
            {
                if (separator + 1 < recvBuf->headerLen)
                {
                    end = NHTTPi_findNextLineHdrRecvBuf(recvBuf,
                        separator + 1, recvBuf->headerLen, NULL,
                        &lineBreakLength);
                    if (end <= 0)
                    {
                        end = recvBuf->headerLen;
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
