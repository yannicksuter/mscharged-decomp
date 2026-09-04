#ifndef NL_BUNDLE_FILE_H
#define NL_BUNDLE_FILE_H

#include "NL/nlMemory.h"
#include "NL/nlString.h"
#include "types.h"

struct nlFile;

typedef void (*FileReadAsyncCallback)(void*, unsigned long, unsigned long);
typedef void (*FileOpenAsyncCallback)(void*, unsigned long, unsigned long);

extern char lbl_8052BA40[];

struct BundleFileHeader
{
    u32 nSectorSize;
    u32 nNumFiles;
    u32 nDirectoryOffsetInSectors;
    u32 nDataOffsetInSectors;
};

struct BundleFileDirectoryEntry
{
    unsigned long m_hash;
    unsigned long m_blockNumber;
    unsigned long m_length;
};

class BundleFile
{
public:
    BundleFile();
    ~BundleFile();

    bool Open(const char* filename, bool keepFilename);
    bool OpenAsync(const char* filename, FileOpenAsyncCallback callback, unsigned long userParam, bool keepFilename);
    void Close();

    bool GetFileInfo(const char* filename, BundleFileDirectoryEntry* entry, bool printError);
    bool GetFileInfo(unsigned long hash, BundleFileDirectoryEntry* entry, bool printError);
    bool GetFileInfoByIndex(unsigned long index, BundleFileDirectoryEntry* entry);

    void ReadFileByIndex(unsigned long index, void* buffer, unsigned long size);
    void ReadFileAsync(const char* filename, void* buffer, unsigned long size, FileReadAsyncCallback callback, unsigned long userParam);
    void ReadFileAsync(unsigned long hash, void* buffer, unsigned long size, FileReadAsyncCallback callback, unsigned long userParam);
    void ReadFileAsyncByIndex(unsigned long index, void* buffer, unsigned long size, FileReadAsyncCallback callback, unsigned long userParam);

    static inline u32 HashFilename(const char* filename)
    {
        char fixedName[256];
        unsigned long index;
        unsigned long n;
        int c;
        index = 0;
        goto test;
    loop:
        c = (char)(unsigned char)filename[index];
        if ((c >= 0x41) && (c <= 0x5A))
        {
            c = c | 0x20;
        }
        fixedName[index] = c;
        if (*(char*)&filename[index] == 0x5C)
        {
            fixedName[index] = '/';
        }
        index++;
    test:
        n = 0;
        if (filename)
        {
            while (filename[n])
                n++;
        }
        if (index < n)
            goto loop;
        fixedName[index] = 0;
        return nlStringHash(fixedName);
    }

    inline u32 FindHashIndex(u32 hash) const
    {
        for (u32 i = 0; i < nNumFiles; i++)
        {
            if (hash == m_pDirectory[i].m_hash)
            {
                return i;
            }
        }
        extern int nlPrintf(const char*, ...);
        nlPrintf(lbl_8052BA40, hash);
        return -1U;
    }

    inline u32 FindHashIndex(u32 hash, bool printError) const
    {
        for (u32 i = 0; i < nNumFiles; i++)
        {
            if (hash == m_pDirectory[i].m_hash)
            {
                return i;
            }
        }
        if (printError)
        {
            extern int nlPrintf(const char*, ...);
            nlPrintf(lbl_8052BA40, hash);
        }
        return -1U;
    }

    u32 nSectorSize;
    u32 nNumFiles;
    u32 nDirectoryOffsetInSectors;
    u32 nDataOffsetInSectors;
    nlFile* m_pFile;
    FileOpenAsyncCallback m_pOpenCallback;
    unsigned long m_openUserParam;
    char* m_pFilename;
    BundleFileDirectoryEntry* m_pDirectory;
};

#endif // NL_BUNDLE_FILE_H
