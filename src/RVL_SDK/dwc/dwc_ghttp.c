#include <dwc/dwc_ghttp.h>

#include <dwc/dwc_memfunc.h>
#include <dwc/dwc_report.h>
#include <dwc/dwci_error.h>
#include <gamespy/ghttp/ghttp.h>
#include <string.h>

typedef struct DWCGHTTPParamEntry {
  DWCGHTTPParam param;
  char* buf;
  int req;
  struct DWCGHTTPParamEntry* nextentry;
} DWCGHTTPParamEntry;

static DWCGHTTPParamEntry* paramhead = NULL;
static int ghttpinitcount = 0;

static DWCGHTTPResult DWCi_HandleGHTTPError(DWCGHTTPResult result);
static DWCGHTTPParamEntry* DWCi_AppendDWCGHTTPParam(
    const DWCGHTTPParam* param);
static void DWCi_RemoveDWCGHTTPParamEntry(DWCGHTTPParamEntry* entry);
static void DWCi_RemoveAllDWCGHTTPParamEntry(void);
static DWCGHTTPParamEntry* DWCi_FindDWCGHTTPParamEntryByReq(int req);

BOOL DWC_InitGHTTP(const char* gamename) {
  DWC_Printf(4, "DWC_InitGHTTP\n");
  ghttpStartup();
  ghttpinitcount++;
  return TRUE;
}

BOOL DWC_ShutdownGHTTP(void) {
  DWC_Printf(4, "DWC_ShutdownGHTTP\n");
  if (ghttpinitcount <= 0) {
    return TRUE;
  }

  ghttpCleanup();
  ghttpinitcount--;
  if (ghttpinitcount == 0) {
    DWCi_RemoveAllDWCGHTTPParamEntry();
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

static GHTTPBool GHTTPCompletedCallback(GHTTPRequest request,
                                        GHTTPResult result, char* buffer,
                                        GHTTPByteCount bufferLen, void* param) {
  DWCGHTTPParamEntry* entry = (DWCGHTTPParamEntry*)param;
  DWCGHTTPParam* parameter = &entry->param;
  DWCGHTTPCompletedCallback callback = parameter->completedCallback;
  BOOL buffer_clear = parameter->buffer_clear;

  DWC_Printf(4, "GHTTPCompleteCallback result : %d\n", result);
  if (callback != NULL) {
    if (result == DWC_GHTTP_SUCCESS) {
      callback(buffer, bufferLen, result, parameter->param);
    } else {
      if (bufferLen > 0) {
        char* temp = DWC_Alloc(DWC_ALLOCTYPE_GHTTP, bufferLen + 1);
        memcpy(temp, buffer, bufferLen);
        temp[bufferLen] = '\0';
        DWC_Printf(4, "%s\n", buffer);
        DWC_Free(DWC_ALLOCTYPE_GHTTP, temp, 0);
      }
      DWCi_HandleGHTTPError(result);
      callback(NULL, 0, result, parameter->param);
    }
  } else {
    DWC_Printf(4, "Callback is NULL\n");
  }

  if (result != DWC_GHTTP_SUCCESS || buffer_clear == TRUE) {
    if (entry->buf != NULL) {
      DWC_Free(DWC_ALLOCTYPE_GHTTP, entry->buf, 0);
    } else {
      buffer_clear = TRUE;
    }
  }

  DWCi_RemoveDWCGHTTPParamEntry(entry);
  return !!buffer_clear;
}

static void GHTTPProgressCallback(GHTTPRequest request, GHTTPState state,
                                  const char* buffer,
                                  GHTTPByteCount bufferLen,
                                  GHTTPByteCount bytesReceived,
                                  GHTTPByteCount totalSize, void* param) {
  DWCGHTTPParam* parameter = &((DWCGHTTPParamEntry*)param)->param;
  DWCGHTTPProgressCallback callback = parameter->progressCallback;

  if (callback != NULL) {
    callback(state, buffer, bufferLen, bytesReceived, totalSize,
             parameter->param);
  }
}

void DWC_GHTTPNewPost(DWCGHTTPPost* post) {
  DWC_Printf(4, "DWC_GHTTPNewPost\n");

  *post = ghttpNewPost();

  if (post == NULL) {
    DWCi_HandleGHTTPError(DWC_GHTTP_INSUFFICIENT_MEMORY);
    DWC_Printf(4, "DWC_Alloc Error\n");
  }
}

BOOL DWC_GHTTPPostAddString(DWCGHTTPPost* post, const char* key,
                            const char* value) {
  if (!ghttpPostAddString(*post, key, value)) {
    DWC_Printf(2, "DWC_GHTTPPostAddString FALSE\n");
    return FALSE;
  } else {
    return TRUE;
  }
}

int DWC_PostGHTTPData(const char* url, GHTTPPost* post,
                      DWCGHTTPCompletedCallback completedCallback,
                      void* param) {
  GHTTPRequest req;
  DWCGHTTPParamEntry* entry = NULL;
  DWCGHTTPParam parameter_instance;

  DWC_Printf(4, "DWC_PostGHTTPData\n");

  if (DWCi_IsError()) {
    return DWC_GHTTP_IN_ERROR;
  }

  parameter_instance.param = param;
  parameter_instance.completedCallback = completedCallback;
  parameter_instance.progressCallback = NULL;
  parameter_instance.buffer_clear = TRUE;

  entry = DWCi_AppendDWCGHTTPParam(&parameter_instance);

  if (entry == NULL) {
    DWCi_HandleGHTTPError(DWC_GHTTP_INSUFFICIENT_MEMORY);
    DWC_Printf(4, "DWC_Alloc Error\n");
    completedCallback(NULL, 0, DWC_GHTTP_INSUFFICIENT_MEMORY, param);
    return DWC_GHTTP_INSUFFICIENT_MEMORY;
  }

  req = ghttpPost(url, *post, GHTTPFalse, GHTTPCompletedCallback, entry);

  if (req < 0) {
    DWCi_HandleGHTTPError(req);
    completedCallback(NULL, 0, req, param);
    DWCi_RemoveDWCGHTTPParamEntry(entry);
  }

  entry->req = req;
  ghttpSetMaxRecvTime(req, 1);
  return req;
}

int DWC_GetGHTTPData(const char* url,
                     DWCGHTTPCompletedCallback completedCallback,
                     void* param) {
  GHTTPRequest req;
  DWCGHTTPParamEntry* entry = NULL;
  DWCGHTTPParam parameter_instance;

  DWC_Printf(4, "DWC_GetGHTTPData\n");

  if (DWCi_IsError()) {
    return DWC_GHTTP_IN_ERROR;
  }

  parameter_instance.param = param;
  parameter_instance.completedCallback = completedCallback;
  parameter_instance.progressCallback = NULL;
  parameter_instance.buffer_clear = TRUE;

  entry = DWCi_AppendDWCGHTTPParam(&parameter_instance);

  if (entry == NULL) {
    DWCi_HandleGHTTPError(DWC_GHTTP_INSUFFICIENT_MEMORY);
    DWC_Printf(4, "DWC_Alloc Error\n");
    completedCallback(NULL, 0, DWC_GHTTP_INSUFFICIENT_MEMORY, param);
    return DWC_GHTTP_INSUFFICIENT_MEMORY;
  }

  req = ghttpGet(url, GHTTPFalse, GHTTPCompletedCallback, entry);

  if (req < 0) {
    DWCi_HandleGHTTPError(req);
    completedCallback(NULL, 0, req, param);
    DWCi_RemoveDWCGHTTPParamEntry(entry);
  }

  entry->req = req;
  ghttpSetMaxRecvTime(req, 1);
  return req;
}

int DWC_GetGHTTPDataEx(const char* url, int bufferlen, BOOL buffer_clear,
                       DWCGHTTPProgressCallback progressCallback,
                       DWCGHTTPCompletedCallback completedCallback,
                       void* param) {
  char* buffer = NULL;
  GHTTPRequest req;
  DWCGHTTPParamEntry* entry = NULL;
  DWCGHTTPParam parameter_instance;

  DWC_Printf(4, "DWC_GetGHTTPDataEx\n");
  if (DWCi_IsError()) {
    return DWC_GHTTP_IN_ERROR;
  }

  parameter_instance.param = param;
  parameter_instance.completedCallback = completedCallback;
  parameter_instance.progressCallback = progressCallback;
  parameter_instance.buffer_clear = buffer_clear;

  entry = DWCi_AppendDWCGHTTPParam(&parameter_instance);

  if (entry == NULL) {
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
      DWCi_RemoveDWCGHTTPParamEntry(entry);
      return DWC_GHTTP_INSUFFICIENT_MEMORY;
    }
    entry->buf = buffer;
  }

  if (progressCallback != NULL) {
    req = ghttpGetEx(url, NULL, buffer, bufferlen, NULL, GHTTPFalse, GHTTPFalse,
                     GHTTPProgressCallback, GHTTPCompletedCallback, entry);
  } else {
    req = ghttpGetEx(url, NULL, buffer, bufferlen, NULL, GHTTPFalse, GHTTPFalse,
                     NULL, GHTTPCompletedCallback, entry);
  }

  if (req < 0) {
    DWCi_HandleGHTTPError(req);
    completedCallback(NULL, 0, req, param);
    if (entry->buf != NULL) {
      DWC_Free(DWC_ALLOCTYPE_GHTTP, entry->buf, 0);
    }
    DWCi_RemoveDWCGHTTPParamEntry(entry);
  }

  entry->req = req;
  ghttpSetMaxRecvTime(req, 1);
  return req;
}

void DWC_CancelGHTTPRequest(int req) {
  DWCGHTTPParamEntry* entry;

  ghttpCancelRequest(req);

  entry = DWCi_FindDWCGHTTPParamEntryByReq(req);
  if (entry == NULL) {
    return;
  }
  if (entry->buf != NULL) {
    DWC_Free(DWC_ALLOCTYPE_GHTTP, entry->buf, 0);
  }
  DWCi_RemoveDWCGHTTPParamEntry(entry);
}

DWCGHTTPState DWC_GetGHTTPState(int req) {
  if (req < 0) {
    return DWC_GHTTP_FALSE;
  } else {
    return ghttpGetState(req);
  }
}

static DWCGHTTPResult DWCi_HandleGHTTPError(DWCGHTTPResult result) {
  int errorCode = -98000;
  DWCErrorType dwcError = DWC_ERROR_TYPE_7;

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
      dwcError = DWC_ERROR_FATAL;
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

  DWCi_SetError(dwcError, errorCode);
  return result;
}

static DWCGHTTPParamEntry* DWCi_AppendDWCGHTTPParam(
    const DWCGHTTPParam* param) {
  DWCGHTTPParamEntry* entry;

  entry = DWC_Alloc(DWC_ALLOCTYPE_GHTTP, sizeof(DWCGHTTPParamEntry));
  if (entry == NULL) {
    return NULL;
  }

  entry->param = *param;
  entry->nextentry = NULL;
  entry->buf = NULL;

  if (paramhead == NULL) {
    paramhead = entry;
    return entry;
  }

  entry->nextentry = paramhead;
  paramhead = entry;
  return entry;
}

static void DWCi_RemoveDWCGHTTPParamEntry(DWCGHTTPParamEntry* entry) {
  DWCGHTTPParamEntry* cursor;
  DWCGHTTPParamEntry* target;

  if (paramhead == NULL) {
    return;
  }

  if (paramhead == entry) {
    cursor = paramhead->nextentry;
    DWC_Free(DWC_ALLOCTYPE_GHTTP, paramhead, 0);
    paramhead = cursor;
    return;
  }

  cursor = paramhead;
  while (cursor->nextentry != NULL) {
    if (cursor->nextentry != entry) {
      cursor = cursor->nextentry;
      continue;
    }

    target = cursor->nextentry;
    cursor->nextentry = cursor->nextentry->nextentry;
    DWC_Free(DWC_ALLOCTYPE_GHTTP, target, 0);
    return;
  }
}

static DWCGHTTPParamEntry* DWCi_FindDWCGHTTPParamEntryByReq(int req) {
  DWCGHTTPParamEntry* cursor;

  cursor = paramhead;
  while (cursor != NULL && cursor->req != req) {
    cursor = cursor->nextentry;
  }

  return cursor;
}

static void DWCi_RemoveAllDWCGHTTPParamEntry(void) {
  DWCGHTTPParamEntry* cursor;
  DWCGHTTPParamEntry* target;

  cursor = paramhead;
  while (cursor != NULL) {
    target = cursor;
    cursor = cursor->nextentry;

    if (target->buf != NULL) {
      DWC_Free(DWC_ALLOCTYPE_GHTTP, target->buf, 0);
    }
    DWC_Free(DWC_ALLOCTYPE_GHTTP, target, 0);
  }

  paramhead = NULL;
}
