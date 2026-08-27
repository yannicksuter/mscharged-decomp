#include "NL/nlBundleFile.h"
#include "NL/nlFile.h"
#include "NL/nlFileGC.h"

#include <string.h>

char lbl_8052BA40[] = "ERROR: Failed to find file with hash ID: %d\n";

struct AsyncReadCallbackData
{
    FileReadAsyncCallback callback;
    unsigned long userParam;
};

static void fn_802BDBEC(nlFile*, void* buffer, unsigned int size, unsigned long userParam);
static void fn_802BDC38(nlFile*, void* buffer, unsigned int size, unsigned long userParam);

static void fn_802BDB70(nlFile*, void*, unsigned int, unsigned long userParam)
{
    BundleFile* bundle = (BundleFile*)userParam;
    nlSeek(bundle->m_pFile, bundle->nDirectoryOffsetInSectors * bundle->nSectorSize, 0);
    bundle->m_pDirectory = (BundleFileDirectoryEntry*)nlMalloc(bundle->nNumFiles * sizeof(BundleFileDirectoryEntry), 0x20, true);
    nlReadAsync(bundle->m_pFile, bundle->m_pDirectory, bundle->nNumFiles * sizeof(BundleFileDirectoryEntry), fn_802BDBEC, (unsigned long)bundle, 0);
}

static void fn_802BDBEC(nlFile*, void* buffer, unsigned int size, unsigned long userParam)
{
    BundleFile* bundle = (BundleFile*)userParam;
    bundle->m_pOpenCallback(buffer, size, bundle->m_openUserParam);
    bundle->m_pOpenCallback = 0;
    bundle->m_openUserParam = 0;
}

static void fn_802BDC38(nlFile*, void* buffer, unsigned int size, unsigned long userParam)
{
    AsyncReadCallbackData* data = (AsyncReadCallbackData*)userParam;
    data->callback(buffer, size, data->userParam);
    delete data;
}

BundleFile::BundleFile()
{
    m_pFile = 0;
    m_pOpenCallback = 0;
    m_openUserParam = 0;
    m_pFilename = 0;
    m_pDirectory = 0;
}

BundleFile::~BundleFile()
{
    if (m_pFile != 0)
    {
        nlClose(m_pFile);
        m_pFile = 0;
    }
    if (m_pDirectory != 0)
    {
        delete[] m_pDirectory;
        m_pDirectory = 0;
    }
    if (m_pFilename != 0)
    {
        delete[] m_pFilename;
        m_pFilename = 0;
    }
}

bool BundleFile::Open(const char* filename, bool keepFilename)
{
    m_pFile = nlOpen(filename);
    if (m_pFile == 0)
    {
        return false;
    }
    if (keepFilename)
    {
        unsigned long length = nlStrLen<char>(filename);
        char* copy = (char*)nlMalloc(length + 1, 8, false);
        m_pFilename = copy;
        char* p;
        unsigned long c;
        unsigned long n;
        n = 0;
        p = copy;
        goto copy_test;
    copy_loop:
        n++;
        p++;
        filename++;
    copy_test:
        if (length-- == 0)
            goto copy_done;
        c = *(const unsigned char*)filename;
        *p = c;
        if ((char)c)
            goto copy_loop;
    copy_done:
        copy[n] = 0;
    }

    BundleFileHeader* header = (BundleFileHeader*)nlMalloc(sizeof(BundleFileHeader), 0x20, true);
    nlRead(m_pFile, header, sizeof(BundleFileHeader), 0);
    memcpy(this, header, sizeof(BundleFileHeader));
    delete header;

    if (nNumFiles == 0)
    {
        m_pDirectory = 0;
    }
    else
    {
        nlSeek(m_pFile, nDirectoryOffsetInSectors * nSectorSize, 0);
        m_pDirectory = (BundleFileDirectoryEntry*)nlMalloc(nNumFiles * sizeof(BundleFileDirectoryEntry), 0x20, true);
        nlRead(m_pFile, m_pDirectory, nNumFiles * sizeof(BundleFileDirectoryEntry), 0);
    }
    return true;
}

bool BundleFile::OpenAsync(const char* filename, FileOpenAsyncCallback callback, unsigned long userParam, bool keepFilename)
{
    m_pOpenCallback = callback;
    m_openUserParam = userParam;
    m_pFile = nlOpen(filename);
    if (m_pFile == 0)
    {
        return false;
    }
    if (keepFilename)
    {
        unsigned long length = nlStrLen<char>(filename);
        char* copy = (char*)nlMalloc(length + 1, 8, false);
        m_pFilename = copy;
        char* p;
        unsigned long c;
        unsigned long n;
        n = 0;
        p = copy;
        goto copy_test;
    copy_loop:
        n++;
        p++;
        filename++;
    copy_test:
        if (length-- == 0)
            goto copy_done;
        c = *(const unsigned char*)filename;
        *p = c;
        if ((char)c)
            goto copy_loop;
    copy_done:
        copy[n] = 0;
    }
    nlReadAsync(m_pFile, this, sizeof(BundleFileHeader), fn_802BDB70, (unsigned long)this, 0);
    return true;
}

void BundleFile::Close()
{
    if (m_pFile != 0)
    {
        nlClose(m_pFile);
        m_pFile = 0;
    }
    if (m_pDirectory != 0)
    {
        delete[] m_pDirectory;
        m_pDirectory = 0;
    }
    if (m_pFilename != 0)
    {
        delete[] m_pFilename;
        m_pFilename = 0;
    }
}

bool BundleFile::GetFileInfo(const char* filename, BundleFileDirectoryEntry* entry, bool printError)
{
    u32 index = FindHashIndex(HashFilename(filename), printError);
    if (index == -1U && !printError)
    {
        return false;
    }
    if (index < nNumFiles)
    {
        memcpy(entry, &m_pDirectory[index], sizeof(BundleFileDirectoryEntry));
        return true;
    }
    return false;
}

bool BundleFile::GetFileInfo(unsigned long hash, BundleFileDirectoryEntry* entry, bool printError)
{
    u32 index = FindHashIndex(hash, printError);
    if (index == -1U && !printError)
    {
        return false;
    }
    if (index < nNumFiles)
    {
        memcpy(entry, &m_pDirectory[index], sizeof(BundleFileDirectoryEntry));
        return true;
    }
    return false;
}

bool BundleFile::GetFileInfoByIndex(unsigned long index, BundleFileDirectoryEntry* entry)
{
    if (index < nNumFiles)
    {
        memcpy(entry, &m_pDirectory[index], sizeof(BundleFileDirectoryEntry));
        return true;
    }
    return false;
}

void BundleFile::ReadFileByIndex(unsigned long index, void* buffer, unsigned long)
{
    BundleFileDirectoryEntry* entry = &m_pDirectory[index];
    nlSeek(m_pFile, entry->m_blockNumber * nSectorSize, 0);
    nlRead(m_pFile, buffer, entry->m_length, 0);
}

void BundleFile::ReadFileAsync(const char* filename, void* buffer, unsigned long size, FileReadAsyncCallback callback, unsigned long userParam)
{
    u32 index = FindHashIndex(HashFilename(filename));
    AsyncReadCallbackData* data = (AsyncReadCallbackData*)nlMalloc(sizeof(AsyncReadCallbackData), 8, true);
    data->callback = callback;
    data->userParam = userParam;
    BundleFileDirectoryEntry* entry = &m_pDirectory[index];
    nlSeek(m_pFile, entry->m_blockNumber * nSectorSize, 0);
    nlReadAsync(m_pFile, buffer, size, fn_802BDC38, (unsigned long)data, 0);
}

void BundleFile::ReadFileAsync(unsigned long hash, void* buffer, unsigned long size, FileReadAsyncCallback callback, unsigned long userParam)
{
    u32 index = FindHashIndex(hash);
    AsyncReadCallbackData* data = (AsyncReadCallbackData*)nlMalloc(sizeof(AsyncReadCallbackData), 8, true);
    data->callback = callback;
    data->userParam = userParam;
    BundleFileDirectoryEntry* entry = &m_pDirectory[index];
    nlSeek(m_pFile, entry->m_blockNumber * nSectorSize, 0);
    nlReadAsync(m_pFile, buffer, size, fn_802BDC38, (unsigned long)data, 0);
}

void BundleFile::ReadFileAsyncByIndex(unsigned long index, void* buffer, unsigned long size, FileReadAsyncCallback callback, unsigned long userParam)
{
    AsyncReadCallbackData* data = (AsyncReadCallbackData*)nlMalloc(sizeof(AsyncReadCallbackData), 8, true);
    data->callback = callback;
    data->userParam = userParam;
    BundleFileDirectoryEntry* entry = &m_pDirectory[index];
    nlSeek(m_pFile, entry->m_blockNumber * nSectorSize, 0);
    nlReadAsync(m_pFile, buffer, size, fn_802BDC38, (unsigned long)data, 0);
}
