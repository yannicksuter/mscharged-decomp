#include <revolution/types.h>

#include <string.h>

static char sBase64Alphabet[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

void* NHTTPi_memcpy(void* destination, const void* source, u32 size)
{
    return memcpy(destination, source, size);
}

s32 NHTTPi_strlen(const char* string)
{
    return strlen(string);
}

s32 NHTTPi_strcmp(const char* left, const char* right)
{
    return strcmp(left, right);
}

void* NHTTPi_memclr(void* destination, u32 size)
{
    return memset(destination, 0, size);
}

s32 NHTTPi_strnicmp(const char* left, const char* right, s32 size)
{
    char leftChar;
    char rightChar;

    while (size > 0)
    {
        leftChar = *left++;
        rightChar = *right++;

        if (leftChar == '\0' || rightChar == '\0')
        {
            if (leftChar == '\0' && rightChar == '\0')
            {
                size = 0;
                break;
            }
        }

        if ((((leftChar >= 'A') & (leftChar <= 'Z'))
                    ? leftChar + ('a' - 'A')
                    : leftChar)
            != (((rightChar >= 'A') & (rightChar <= 'Z'))
                    ? rightChar + ('a' - 'A')
                    : rightChar))
        {
            break;
        }
        size--;
    }

    return size;
}

s32 NHTTPi_getUrlEncodedSize(const char* string)
{
    s32 result = 0;
    s32 c = *string++;

    while (c != '\0')
    {
        if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z')
            || (c >= 'a' && c <= 'z') || c == ' ')
        {
            result++;
        }
        else
        {
            result += 3;
        }
        c = *string++;
    }

    return result;
}

s32 NHTTPi_getUrlEncodedSize2(const char* string, s32 length)
{
    s32 result = 0;
    s32 c = *string++;

    while (length-- > 0)
    {
        if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z')
            || (c >= 'a' && c <= 'z') || c == ' ')
        {
            result++;
        }
        else
        {
            result += 3;
        }
        c = *string++;
    }

    return result;
}

s32 NHTTPi_encodeUrlChar(char* destination, char c)
{
    s32 high;
    s32 low;

    if (c == ' ')
    {
        destination[0] = '+';
        return 1;
    }

    if ((c >= '0' && c <= '9') || (c >= 'A' && c <= 'Z')
        || (c >= 'a' && c <= 'z'))
    {
        destination[0] = c;
        return 1;
    }

    high = ((u8)c >> 4) & 0xF;
    low = (u8)c & 0xF;
    destination[0] = '%';
    if (high < 10)
    {
        high += '0';
    }
    else
    {
        high += 'A' - 10;
    }
    destination[1] = high;
    if (low < 10)
    {
        low += '0';
    }
    else
    {
        low += 'A' - 10;
    }
    destination[2] = low;
    return 3;
}

s32 NHTTPi_strToHex(const char* string, s32 length)
{
    s32 result;
    BOOL foundDigit;
    char c;

    if (length > 8)
    {
        return -1;
    }
    if ((length == 8) & (string[0] > '7'))
    {
        return -1;
    }

    result = 0;
    foundDigit = FALSE;
    while (length-- > 0)
    {
        c = ((*string >= 'A') & (*string <= 'Z'))
            ? *string + ('a' - 'A')
            : *string;

        if (c >= '0' && c <= '9')
        {
            result = result * 16 + c - '0';
            foundDigit = TRUE;
        }
        else if (c >= 'a' && c <= 'f')
        {
            result = result * 16 + c - 'a' + 10;
            foundDigit = TRUE;
        }
        else if (foundDigit && (c == ' ' || c == '\0'))
        {
            break;
        }
        else if (!foundDigit && c == ' ')
        {
        }
        else
        {
            return -1;
        }
        string++;
    }

    return result;
}

s32 NHTTPi_strToInt(const char* string, s32 length)
{
    s32 result;
    BOOL foundDigit;
    s32 previous;
    char c;

    if (length > 10)
    {
        return -1;
    }

    result = 0;
    foundDigit = FALSE;
    while (length-- > 0)
    {
        c = *string;
        if (foundDigit && (c == ' ' || c == '\0'))
        {
            break;
        }
        if (!foundDigit && c == ' ')
        {
        }
        else if (c < '0' || c > '9')
        {
            return -1;
        }
        else
        {
            previous = result;
            foundDigit = TRUE;
            result = result * 10 + c - '0';
            if (previous > result)
            {
                return -1;
            }
        }
        string++;
    }

    return result;
}

s32 NHTTPi_intToStr(char* destination, u32 value)
{
    u32 powers[9] = {
        1000000000,
        100000000,
        10000000,
        1000000,
        100000,
        10000,
        1000,
        100,
        10,
    };
    s32 length = 0;
    BOOL started = FALSE;
    s32 i;
    u32 digit;

    for (i = 0; i < 9; i++)
    {
        if (value >= powers[i])
        {
            digit = value / powers[i];
            value -= digit * powers[i];
            started = TRUE;
            destination[length] = digit + '0';
            length++;
        }
        else if (started)
        {
            destination[length] = '0';
            length++;
        }
    }
    destination[length] = value + '0';
    return length + 1;
}

s32 NHTTPi_compareToken(const char* string, const char* token)
{
    while ((((*string >= 'A') & (*string <= 'Z'))
                   ? *string + ('a' - 'A')
                   : *string)
           == (((*token >= 'A') & (*token <= 'Z'))
                   ? *token + ('a' - 'A')
                   : *token))
    {
        if (*string == '\0' || *string == ' ')
        {
            return 0;
        }
        string++;
        token++;
    }

    return -1;
}

s32 NHTTPi_strtonum(const char* string, s32 length)
{
    char c;
    s32 result;
    s32 digitCount;

    digitCount = 0;
    result = 0;

    while (length-- != 0)
    {
        c = *string;
        if (c != ' ')
        {
            if ((c >= '0') & (c <= '9'))
            {
                result = result * 10 + c - '0';
                digitCount++;
                if (digitCount > 9)
                {
                    return -1;
                }
            }
        }
        string++;
    }

    return digitCount == 0 ? -1 : result;
}

s32 NHTTPi_memfind(const char* buffer, s32 bufferLength,
    const char* pattern, s32 patternLength)
{
    const char* bufferCursor;
    s32 positionCount;
    s32 position;
    s32 matched;

    if (bufferLength < patternLength)
    {
        return -1;
    }

    positionCount = bufferLength - patternLength + 1;
    for (position = 0; position < positionCount; position++)
    {
        if (pattern[0] == buffer[position])
        {
            bufferCursor = &buffer[position + 1];
            matched = 1;

            while (matched < patternLength)
            {
                if (*bufferCursor != pattern[matched])
                {
                    break;
                }
                matched++;
                bufferCursor++;
            }
            if (matched == patternLength)
            {
                return 0;
            }
        }
    }

    return -1;
}

s32 NHTTPi_Base64Encode(char* destination, const char* source)
{
    char* alphabet = sBase64Alphabet;
    char* current = destination;
    s32 length = strlen(source);
    s32 i;

    for (i = 0; i < length; i += 3)
    {
        current[0] = alphabet[source[0] >> 2];
        current[1] = alphabet[((source[0] & 3) << 4) + (source[1] >> 4)];
        current[2] = alphabet[((source[1] & 0xF) << 2) + (source[2] >> 6)];
        current[3] = alphabet[source[2] & 0x3F];
        source += 3;
        current += 4;
    }

    if (i == length + 1)
    {
        current[-1] = '=';
    }
    else if (i == length + 2)
    {
        current[-2] = '=';
        current[-1] = '=';
    }
    *current = '\0';
    return strlen(destination);
}
