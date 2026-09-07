#ifndef NL_DEBUG_STRING_H
#define NL_DEBUG_STRING_H

extern void* g_pDebugStringTable;
const char* nlLookupDebugString(void* table, unsigned long value);

#endif // NL_DEBUG_STRING_H
