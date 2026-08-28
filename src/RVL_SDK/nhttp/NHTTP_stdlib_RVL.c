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
    s32 leftChar;
    s32 rightChar;

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

        if ((rightChar >= 'A') & (rightChar <= 'Z'))
        {
            rightChar += 'a' - 'A';
        }
        if ((leftChar >= 'A') & (leftChar <= 'Z'))
        {
            leftChar += 'a' - 'A';
        }

        if (leftChar != rightChar)
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
    s32 c;
    s32 result;
    BOOL foundDigit;
    s32 parsedChar;

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
        c = *string;
        if ((c >= 'A') & (c <= 'Z'))
        {
            c += 'a' - 'A';
        }
        parsedChar = (char)c;

        if (parsedChar >= '0' && parsedChar <= '9')
        {
            result = result * 16 + parsedChar - '0';
            foundDigit = TRUE;
        }
        else if (parsedChar >= 'a' && parsedChar <= 'f')
        {
            result = result * 16 + parsedChar - 'a' + 10;
            foundDigit = TRUE;
        }
        else if (foundDigit && (parsedChar == ' ' || parsedChar == '\0'))
        {
            break;
        }
        else if (!foundDigit && parsedChar == ' ')
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
    char* current = destination;
    s32 length = 0;
    BOOL started = FALSE;
    s32 i;
    u32 digit;

    for (i = 0; i < 9; i++)
    {
        if (value >= powers[i])
        {
            digit = value / powers[i];
            started = TRUE;
            length++;
            *current = digit + '0';
            current++;
            value -= digit * powers[i];
        }
        else if (started)
        {
            *current = '0';
            length++;
            current++;
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
    const char* current;
    s32 positionCount;
    s32 position;

    if (bufferLength < patternLength)
    {
        return -1;
    }

    positionCount = bufferLength - patternLength + 1;
    current = buffer;
    for (position = 0; position < positionCount; position++)
    {
        if (pattern[0] == current[0])
        {
            const char* bufferCursor = &buffer[position + 1];
            const char* patternCursor = &pattern[1];
            s32 matched = 1;

            while (matched < patternLength)
            {
                if (*bufferCursor != *patternCursor)
                {
                    break;
                }
                matched++;
                patternCursor++;
                bufferCursor++;
            }
            if (matched == patternLength)
            {
                return 0;
            }
        }
        current++;
    }

    return -1;
}

s32 NHTTPi_Base64Encode(char* destination, const char* source)
{
    char* alphabet = sBase64Alphabet;
    char* current = destination;
    s32 length = strlen(source);
    u32 groupCount = length + 2;
    s32 encodedLength;
    s32 i;

    groupCount /= 3;
    encodedLength = groupCount * 3;
    for (i = 0; i < groupCount; i++)
    {
        current[0] = alphabet[source[0] >> 2];
        current[1] = alphabet[((source[0] & 3) << 4) + (source[1] >> 4)];
        current[2] = alphabet[((source[1] & 0xF) << 2) + (source[2] >> 6)];
        current[3] = alphabet[source[2] & 0x3F];
        source += 3;
        current += 4;
    }

    if (encodedLength == length + 1)
    {
        current[-1] = '=';
    }
    else if (encodedLength == length + 2)
    {
        current[-2] = '=';
        current[-1] = '=';
    }
    *current = '\0';
    return strlen(destination);
}
