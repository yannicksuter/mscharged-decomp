#ifndef NL_FILE_GC_H
#define NL_FILE_GC_H

struct nlFile;

void nlInitFileSystem(void);
void nlCancelPendingAsyncReads(nlFile* pFile, void (*callback)(nlFile*, void*, unsigned int, unsigned long, void (*)(nlFile*, void*, unsigned int, unsigned long)));
bool nlAsyncReadsPending(nlFile* file);
void nlSeek(nlFile* file, unsigned int offset, unsigned long origin);
void nlServiceFileSystem(void);

#endif // NL_FILE_GC_H
