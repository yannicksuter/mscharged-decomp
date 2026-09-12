#include "NL/nlDebugString.h"

void* g_pDebugStringTable;

const char* nlLookupDebugString(void*, unsigned long)
{
    return "unknown";
}
