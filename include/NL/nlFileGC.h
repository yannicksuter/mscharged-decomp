#ifndef NL_FILE_GC_H
#define NL_FILE_GC_H

struct nlFile;
struct AsyncEntry;

void nlInitFileSystem(void);
void nlCancelPendingAsyncReads(nlFile* pFile, void (*callback)(nlFile*, void*, unsigned int, unsigned long, void (*)(nlFile*, void*, unsigned int, unsigned long)));
bool nlAsyncReadsPending(nlFile* file);
void nlSeek(nlFile* file, unsigned int offset, unsigned long origin);
void nlServiceFileSystem(void);

extern "C" bool fn_80367B70(AsyncEntry* entry);
extern "C" bool fn_80367DAC(AsyncEntry* entry,
    void (*callback)(nlFile*, void*, unsigned int, unsigned long, void (*)(nlFile*, void*, unsigned int, unsigned long)));

#endif // NL_FILE_GC_H
