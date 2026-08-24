#ifndef _NLPRINT_H_
#define _NLPRINT_H_

struct __va_list_struct;

int nlVSNPrintf(char* buffer, unsigned long size, const char* format, __va_list_struct* args);
int nlSNPrintf(char* buffer, unsigned long size, const char* format, ...);

#endif // _NLPRINT_H_
