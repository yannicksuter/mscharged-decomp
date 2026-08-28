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

BOOL NHTTP_AddPostDataAscii(NHTTPRequestInfo* request, NHTTPBgnEndInfo* info,
    char* name, char* value)
{
    BOOL result = FALSE;
    s32 valueLength = 0;
    BOOL boundaryFound;
    const char* initial;
    s32 index;
    char initialChar;
    char nextChar;

    if (request->state != 0)
    {
        return FALSE;
    }
    if (request->_unk10 != NULL)
    {
        return FALSE;
    }

    if (value != NULL)
    {
        valueLength = NHTTPi_strlen(value);
    }

    if (NHTTPi_memfind(value, valueLength, request->multipartBoundary + 2, 18) < 0)
    {
        boundaryFound = TRUE;
    }
    else
    {
        initial = NHTTPi_strMultipartBound + 19;
        index = 19;

        do
        {
            initialChar = *initial;
            nextChar = request->multipartBoundary[index];
            nextChar++;

            if ((u8)nextChar == '{')
            {
                nextChar = '0';
            }
            else if ((u8)nextChar == '[')
            {
                nextChar = 'a';
            }
            else if ((u8)nextChar == ':')
            {
                nextChar = 'A';
            }

            request->multipartBoundary[index] = nextChar;
            if (nextChar != initialChar)
            {
                if (NHTTPi_memfind(value, valueLength, request->multipartBoundary + 2, 18)
                    < 0)
                {
                    boundaryFound = TRUE;
                    break;
                }
            }
            else
            {
                index--;
                initial--;
            }
        } while (index >= 2);

        if (index < 2)
        {
            boundaryFound = FALSE;
        }
    }

    if (boundaryFound)
    {
        result = addHdrList(&request->postData, info, name, value);
        if (result)
        {
            request->postData->next->length = valueLength;
        }
    }

    return result;
}
