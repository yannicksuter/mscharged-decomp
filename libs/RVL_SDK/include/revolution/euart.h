#ifndef REVOLUTION_EUART_H
#define REVOLUTION_EUART_H

#include <revolution/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum EUARTError {
    EUART_ERROR_OK,
    EUART_ERROR_FATAL,
    EUART_ERROR_INVALID,
    EUART_ERROR_BUSY,
    EUART_ERROR_4,
    EUART_ERROR_EXI,
} EUARTError;

BOOL EUARTInit(void);
EUARTError InitializeUART(void);
EUARTError WriteUARTN(const char* msg, u32 n);

#ifdef __cplusplus
}
#endif

#endif  // REVOLUTION_EUART_H
