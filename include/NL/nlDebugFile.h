#ifndef NL_DEBUG_FILE_H
#define NL_DEBUG_FILE_H

void nlCloseFileDebug(void* fp);
void nlFlushFileDebug(void* fp);
void nlWriteLineDebug(void* file, const char* buffer, bool flush);
void nlWriteBuffer(void* file, const char* buffer, int size);
void* nlOpenFileDebug(const char* fileName, bool bBinary, bool bAppend);
bool nlDebugFileIsValid(void* fp);

#endif // NL_DEBUG_FILE_H
