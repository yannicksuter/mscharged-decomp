#include <private/nhttp.h>

extern const char NHTTPi_strMultipartBound[];

s32 NHTTPi_compareToken(const char* lhs, const char* rhs);
void* NHTTPi_alloc(u32 size, int align);
void NHTTPi_SetError(NHTTPBgnEndInfo* info, NHTTPErr error);
s32 NHTTPi_strlen(const char* string);
s32 NHTTPi_memfind(const void* haystack, s32 haystackLength,
    const void* needle, s32 needleLength);

static BOOL addHdrList(NHTTPHeader** list, NHTTPBgnEndInfo* info, char* name,
    char* value)
{
    BOOL found = FALSE;
    NHTTPHeader* header = *list;

    if (header != NULL)
    {
        if (NHTTPi_compareToken(name, header->name) != 0)
        {
            header = header->prev;
            while (header != *list)
            {
                if (NHTTPi_compareToken(name, header->name) == 0)
                {
                    found = TRUE;
                    break;
                }
                header = header->prev;
            }
        }
        else
        {
            found = TRUE;
        }
    }

    if (found)
    {
        header->value = value;
    }
    else
    {
        header = NHTTPi_alloc(sizeof(NHTTPHeader), 4);
        if (header == NULL)
        {
            NHTTPi_SetError(info, NHTTP_ERROR_ALLOC);
            return FALSE;
        }

        header->name = name;
        header->value = value;
        header->length = 0;
        header->_unk14 = 0;

        if (*list != NULL)
        {
            header->next = (*list)->next;
            header->prev = *list;
            (*list)->next->prev = header;
            (*list)->next = header;
        }
        else
        {
            header->prev = header;
            header->next = header;
            *list = header;
        }
    }

    return TRUE;
}

static char incAscii(u8 c)
{
    c++;
    if (c == 'z' + 1)
    {
        c = '0';
    }
    else if (c == 'Z' + 1)
    {
        c = 'a';
    }
    else if (c == '9' + 1)
    {
        c = 'A';
    }

    return (char)c;
}

static BOOL checkTagPost(NHTTPRequestInfo* req_p, const char* value_p, u32 length)
{
    int n;
    char c;

    if (NHTTPi_memfind(value_p, (int)length,
            &req_p->multipartBoundary[2], LEN_POSTBOUND)
        < 0)
    {
        return TRUE;
    }

    for (n = 2 + LEN_POSTBOUND - 1; n >= 2; n--)
    {
        for (c = req_p->multipartBoundary[n];;)
        {
            c = incAscii((u8)c);
            req_p->multipartBoundary[n] = c;

            if (c == NHTTPi_strMultipartBound[n])
            {
                break;
            }
            if (NHTTPi_memfind(value_p, (int)length,
                    &req_p->multipartBoundary[2], LEN_POSTBOUND)
                < 0)
            {
                return TRUE;
            }
        }
    }

    return FALSE;
}

NHTTPHeader* NHTTPi_getHdrFromList(NHTTPHeader** list)
{
    NHTTPHeader* header = *list;

    if (header != NULL)
    {
        if (header != header->next)
        {
            header->next->prev = header->prev;
            header->prev->next = header->next;
            *list = header->prev;
        }
        else
        {
            *list = NULL;
        }
    }

    return header;
}

BOOL NHTTP_AddHeaderField(NHTTPRequestInfo* request, NHTTPBgnEndInfo* info,
    char* name, char* value)
{
    if (request->state != 0)
    {
        return FALSE;
    }
    return addHdrList(&request->headers, info, name, value);
}

BOOL NHTTP_AddPostDataAscii(NHTTPRequestInfo* req_p,
    NHTTPBgnEndInfo* bgnEndInfo_p, char* label_p, char* value_p)
{
    int rc = FALSE;
    u32 length = 0;

    if (req_p->state != 0)
    {
        return FALSE;
    }
    if (req_p->isRawData)
    {
        return FALSE;
    }

    if (value_p)
    {
        length = (u32)NHTTPi_strlen(value_p);
    }
    if (checkTagPost(req_p, value_p, length))
    {
        rc = addHdrList(&req_p->postData, bgnEndInfo_p, label_p, value_p);
        if (rc)
        {
            req_p->postData->next->length = length;
        }
    }

    return rc;
}
