#ifndef METROTRK_MUTEX_TRK_H
#define METROTRK_MUTEX_TRK_H

#include <MetroTRK/dserror.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef unsigned int DSMutex;

    DSError TRKReleaseMutex(void* p1);
    DSError TRKAcquireMutex(void* p1);
    DSError TRKInitializeMutex(void* p1);

#ifdef __cplusplus
}
#endif

#endif // METROTRK_MUTEX_TRK_H
