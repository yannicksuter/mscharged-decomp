#include <dwc/dwc_error.h>

//! @brief The last error code encountered.
//!
int stDwcErrorCode;

//! @brief The last error type encountered.
//!
int stDwcLastError;

int DWC_GetLastError(int* errorCode) {
  if (errorCode)
    *errorCode = stDwcErrorCode;

  return stDwcLastError;
}

s32 DWC_GetLastErrorEx(s32* errorCode, DWCErrorType* errorType) {
  if (errorCode)
    *errorCode = stDwcErrorCode;
  if (errorType) {
#if defined(VERSION_R4QK01)
    switch (stDwcLastError) {
    case DWC_ERROR_AUTH_ANY:
    case DWC_ERROR_AUTH_OUT_OF_SERVICE:
    case DWC_ERROR_AUTH_STOP_SERVICE:
    case DWC_ERROR_AC_ANY:
    case DWC_ERROR_DISCONNECTED:
    case 14:
    case 16:
    case 18:
    case 21:
      *errorType = 6;
      break;
    case DWC_ERROR_NETWORK:
      *errorType = 3;
      break;
    case DWC_ERROR_GHTTP_ANY:
      *errorType = 4;
      break;
    case 13:
      *errorType = 5;
      break;
    case 22:
      *errorType = 8;
      break;
    case DWC_ERROR_FRIENDS_SHORTAGE:
    case DWC_ERROR_NOT_FRIEND_SERVER:
    case DWC_ERROR_SERVER_FULL:
    case DWC_ERROR_MATCH_LOCKED:
      *errorType = 1;
      break;
    case 15:
    case 17:
    case 19:
    case 20:
    case 24:
      *errorType = 2;
      break;
    case DWC_ERROR_DS_MEMORY_ANY:
    case DWC_ERROR_FATAL:
      *errorType = 7;
      break;
    default:
      *errorType = 0;
      break;
    }
#else
    switch (stDwcLastError) {
    case 2:
    case 3:
    case 4:
    case 5:
    case 8:
      *errorType = 6;
      break;
    case 6:
      *errorType = 3;
      break;
    case 7:
      *errorType = 4;
      break;
    case 10:
    case 11:
    case 12:
    case 13:
      *errorType = 1;
      break;
    case 1:
    case DWC_ERROR_FATAL:
      *errorType = 7;
      break;

    case 14:
      *errorType = 5;
      break;
    case 15:
    case 17:
      *errorType = 6;
      break;
    case 16:
      *errorType = 2;
      break;
    default:
      *errorType = 0;
      break;
    }
#endif
  }
  return stDwcLastError;
}

void DWC_ClearError() {
  if (stDwcLastError != DWC_ERROR_FATAL) {
    stDwcLastError = DWC_ERROR_NONE;
    stDwcErrorCode = 0;
  }
}

int DWCi_IsError() { return stDwcLastError != DWC_ERROR_NONE; }

void DWCi_SetError(int lastError, int errorCode) {
  if (stDwcLastError != DWC_ERROR_FATAL) {
    stDwcLastError = lastError;
    stDwcErrorCode = errorCode;
  }
#ifdef DEBUG
  if (stDwcLastError == DWC_ERROR_FATAL)
    DWC_Printf(-1, "FATALERROR_SET\n");
#endif
}
