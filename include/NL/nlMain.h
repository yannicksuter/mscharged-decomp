#ifndef NL_NLMAIN_H
#define NL_NLMAIN_H

#include "types.h"

u32 nlChecksum32(const void* data, unsigned long size);

class RunningChecksum
{
public:
    void ChecksumData(const void* pData, unsigned long nDataLen);
    RunningChecksum();

    /* 0x00 */ u32 m_nChecksum;
};

#endif // NL_NLMAIN_H
