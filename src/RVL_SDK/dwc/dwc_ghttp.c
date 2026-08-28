#include <dwc/dwc_ghttp.h>

#include <dwc/dwc_memfunc.h>
#include <dwc/dwc_report.h>
#include <dwc/dwci_error.h>
#include <gamespy/ghttp/ghttp.h>
#include <string.h>

typedef struct DWCGHTTPParam {
  void* _00;
  DWCGHTTPCompletedCallback _04;
  DWCGHTTPProgressCallback _08;
  BOOL _0C;
  char* _10;
  int _14;
  struct DWCGHTTPParam* _18;
} DWCGHTTPParam;

DWCGHTTPParam* lbl_806E2EC8;
int lbl_806E2ECC;

int DWCi_HandleGHTTPError(int result);
void DWCi_RemoveDWCGHTTPParamEntry(DWCGHTTPParam* paramEntry);

BOOL DWC_InitGHTTP(const char* gamename) {
  DWC_Printf(4, "DWC_InitGHTTP\n");
  ghttpStartup();
  lbl_806E2ECC++;
  return TRUE;
}

BOOL DWC_ShutdownGHTTP(void) {
  DWCGHTTPParam* paramEntry;
  DWCGHTTPParam* current;

  DWC_Printf(4, "DWC_ShutdownGHTTP\n");
  if (lbl_806E2ECC <= 0) {
    return TRUE;
  }

  ghttpCleanup();
  lbl_806E2ECC--;
  if (lbl_806E2ECC == 0) {
    current = lbl_806E2EC8;
    while (current != NULL) {
      paramEntry = current;
      current = current->_18;
      if (paramEntry->_10 != NULL) {
        DWC_Free(DWC_ALLOCTYPE_GHTTP, paramEntry->_10, 0);
      }
      DWC_Free(DWC_ALLOCTYPE_GHTTP, paramEntry, 0);
    }
    lbl_806E2EC8 = NULL;
  }

  return TRUE;
}

BOOL DWC_ProcessGHTTP(void) {
  if (DWCi_IsError()) {
    return FALSE;
  }

  ghttpThink();
  return TRUE;
}

GHTTPBool GHTTPCompletedCallback(GHTTPRequest request, GHTTPResult result,
                                 char* buffer, GHTTPByteCount bufferLen,
                                 void* param) {
  DWCGHTTPParam* paramEntry = param;
  DWCGHTTPCompletedCallback completedCallback = paramEntry->_04;
  BOOL buffer_clear = paramEntry->_0C;

  DWC_Printf(4, "GHTTPCompleteCallback result : %d\n", result);
  if (completedCallback != NULL) {
    if (result == DWC_GHTTP_SUCCESS) {
      completedCallback(buffer, bufferLen, result, paramEntry->_00);
    } else {
      if (bufferLen > 0) {
        char* temp = DWC_Alloc(DWC_ALLOCTYPE_GHTTP, bufferLen + 1);
        memcpy(temp, buffer, bufferLen);
        temp[bufferLen] = '\0';
        DWC_Printf(4, "%s\n", buffer);
        DWC_Free(DWC_ALLOCTYPE_GHTTP, temp, 0);
      }
      DWCi_HandleGHTTPError(result);
      completedCallback(NULL, 0, result, paramEntry->_00);
    }
  } else {
    DWC_Printf(4, "Callback is NULL\n");
  }

  if (result != DWC_GHTTP_SUCCESS || buffer_clear == TRUE) {
    if (paramEntry->_10 != NULL) {
      DWC_Free(DWC_ALLOCTYPE_GHTTP, paramEntry->_10, 0);
    } else {
      buffer_clear = TRUE;
    }
  }

  DWCi_RemoveDWCGHTTPParamEntry(paramEntry);
  return !!buffer_clear;
}

void GHTTPProgressCallback(GHTTPRequest request, GHTTPState state,
                           const char* buffer, GHTTPByteCount bufferLen,
                           GHTTPByteCount bytesReceived,
                           GHTTPByteCount totalSize, void* param) {
  DWCGHTTPParam* paramEntry = param;

  if (paramEntry->_08 != NULL) {
    paramEntry->_08(state, buffer, bufferLen, bytesReceived, totalSize,
                    paramEntry->_00);
  }
}

int DWC_GetGHTTPDataEx(const char* url, int bufferlen, BOOL buffer_clear,
                       DWCGHTTPProgressCallback progressCallback,
                       DWCGHTTPCompletedCallback completedCallback,
                       void* param) {
  char* buffer = NULL;
  DWCGHTTPParam* paramEntry;
  int request;

  DWC_Printf(4, "DWC_GetGHTTPDataEx\n");
  if (DWCi_IsError()) {
    return DWC_GHTTP_IN_ERROR;
  }

  paramEntry = DWC_Alloc(DWC_ALLOCTYPE_GHTTP, sizeof(DWCGHTTPParam));
  if (paramEntry != NULL) {
    paramEntry->_00 = param;
    paramEntry->_04 = completedCallback;
    paramEntry->_08 = progressCallback;
    paramEntry->_0C = buffer_clear;
    paramEntry->_18 = NULL;
    paramEntry->_10 = NULL;
    if (lbl_806E2EC8 == NULL) {
      lbl_806E2EC8 = paramEntry;
    } else {
      paramEntry->_18 = lbl_806E2EC8;
      lbl_806E2EC8 = paramEntry;
    }
  }

  if (paramEntry == NULL) {
    DWCi_HandleGHTTPError(DWC_GHTTP_INSUFFICIENT_MEMORY);
    DWC_Printf(4, "DWC_Alloc Error\n");
    completedCallback(NULL, 0, DWC_GHTTP_INSUFFICIENT_MEMORY, param);
    return DWC_GHTTP_INSUFFICIENT_MEMORY;
  }

  if (bufferlen > 0) {
    buffer = DWC_Alloc(DWC_ALLOCTYPE_GHTTP, bufferlen);
    if (buffer == NULL) {
      DWCi_HandleGHTTPError(DWC_GHTTP_INSUFFICIENT_MEMORY);
      DWC_Printf(4, "DWC_Alloc Error\n");
      completedCallback(NULL, 0, DWC_GHTTP_INSUFFICIENT_MEMORY, param);
      DWCi_RemoveDWCGHTTPParamEntry(paramEntry);
      return DWC_GHTTP_INSUFFICIENT_MEMORY;
    }
    paramEntry->_10 = buffer;
  }

  if (progressCallback != NULL) {
    request = ghttpGetEx(url, NULL, buffer, bufferlen, NULL, GHTTPFalse,
                         GHTTPFalse, GHTTPProgressCallback,
                         GHTTPCompletedCallback, paramEntry);
  } else {
    request = ghttpGetEx(url, NULL, buffer, bufferlen, NULL, GHTTPFalse,
                         GHTTPFalse, NULL, GHTTPCompletedCallback, paramEntry);
  }

  if (request < 0) {
    DWCi_HandleGHTTPError(request);
    completedCallback(NULL, 0, request, param);
    if (paramEntry->_10 != NULL) {
      DWC_Free(DWC_ALLOCTYPE_GHTTP, paramEntry->_10, 0);
    }
    DWCi_RemoveDWCGHTTPParamEntry(paramEntry);
  }

  paramEntry->_14 = request;
  ghttpSetMaxRecvTime(request, 1);
  return request;
}

int DWCi_HandleGHTTPError(int result) {
  int errorCode = -98000;
  int errorType = DWC_ERROR_TYPE_7;

  if (result == DWC_GHTTP_SUCCESS) {
    return DWC_GHTTP_SUCCESS;
  }

  DWC_Printf(2, "Main, DWCGHTTP error %d\n", result);
  switch (result) {
    case DWC_GHTTP_FAILED_TO_OPEN_FILE:
      errorCode -= 800;
      break;
    case DWC_GHTTP_INVALID_POST:
      errorCode -= 810;
      break;
    case DWC_GHTTP_INSUFFICIENT_MEMORY:
      errorCode -= 840;
      break;
    case DWC_GHTTP_INVALID_FILE_NAME:
    case DWC_GHTTP_INVALID_BUFFER_SIZE:
    case DWC_GHTTP_INVALID_URL:
      errorCode -= 820;
      break;
    case DWC_GHTTP_UNSPECIFIED_ERROR:
      errorCode -= 830;
      break;
    case DWC_GHTTP_OUT_OF_MEMORY:
    case 20:
      errorType = DWC_ERROR_FATAL;
      errorCode -= 1;
      break;
    case DWC_GHTTP_BUFFER_OVERFLOW:
      errorCode -= 840;
      break;
    case DWC_GHTTP_PARSE_URL_FAILED:
      errorCode -= 850;
      break;
    case DWC_GHTTP_HOST_LOOKUP_FAILED:
      errorCode -= 30;
      break;
    case DWC_GHTTP_SOCKET_FAILED:
      errorCode -= 50;
      break;
    case DWC_GHTTP_CONNECT_FAILED:
    case DWC_GHTTP_FILE_NOT_FOUND:
    case DWC_GHTTP_SERVER_ERROR:
      errorCode -= 20;
      break;
    case DWC_GHTTP_BAD_RESPONSE:
      errorCode -= 860;
      break;
    case DWC_GHTTP_REQUEST_REJECTED:
    case DWC_GHTTP_UNAUTHORIZED:
    case DWC_GHTTP_FORBIDDEN:
      errorCode -= 870;
      break;
    case DWC_GHTTP_FILE_WRITE_FAILED:
    case DWC_GHTTP_FILE_READ_FAILED:
      errorCode -= 880;
      break;
    case DWC_GHTTP_FILE_INCOMPLETE:
      errorCode -= 890;
      break;
    case DWC_GHTTP_FILE_TOO_BIG:
      errorCode -= 900;
      break;
    case DWC_GHTTP_ENCRYPTION_ERROR:
      errorCode -= 910;
      break;
  }

  DWCi_SetError(errorType, errorCode);
  return result;
}

void DWCi_RemoveDWCGHTTPParamEntry(DWCGHTTPParam* paramEntry) {
  DWCGHTTPParam* current = lbl_806E2EC8;

  if (current == NULL) {
    return;
  }

  if (current == paramEntry) {
    DWCGHTTPParam* next = current->_18;
    DWC_Free(DWC_ALLOCTYPE_GHTTP, current, 0);
    lbl_806E2EC8 = next;
  } else {
    DWCGHTTPParam* previous = current;
    while ((current = previous->_18) != NULL) {
      if (current != paramEntry) {
        previous = current;
      } else {
        previous->_18 = current->_18;
        DWC_Free(DWC_ALLOCTYPE_GHTTP, current, 0);
        break;
      }
    }
  }
}
