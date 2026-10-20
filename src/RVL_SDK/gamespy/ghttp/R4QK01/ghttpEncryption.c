/*
 * GameSpy GHTTP SDK - RevoEx SSL backend.
 * Copyright (c) 2011, IGN Entertainment, Inc.
 * See ../../LICENSE for redistribution terms.
 */

#include "ghttpCommon.h"
#include <revolution/ssl.h>

GHTTPBool ghttpSetRequestEncryptionEngine(GHTTPRequest request,
                                         GHTTPEncryptionEngine engine) {
  GHIConnection* connection = ghiRequestToConnection(request);
  if (!connection)
    return GHTTPFalse;

  if (engine == GHTTPEncryptionEngine_Default)
    engine = GHTTPEncryptionEngine_RevoEx;

  if (connection->encryptor.mEngine == engine)
    return GHTTPTrue;

  if (connection->encryptor.mInterface != NULL &&
      connection->encryptor.mEngine != engine)
    return GHTTPFalse;

  if (engine == GHTTPEncryptionEngine_None &&
      strncmp(connection->URL, "https://", 8) == 0)
    return GHTTPFalse;

  connection->encryptor.mEngine = engine;
  if (engine == GHTTPEncryptionEngine_None) {
    connection->encryptor.mInterface = NULL;
    return GHTTPTrue;
  } else {
    connection->encryptor.mInterface = NULL;
    connection->encryptor.mInitFunc = ghiEncryptorSslInitFunc;
    connection->encryptor.mStartFunc = ghiEncryptorSslStartFunc;
    connection->encryptor.mCleanupFunc = ghiEncryptorSslCleanupFunc;
    connection->encryptor.mEncryptFunc = ghiEncryptorSslEncryptFunc;
    connection->encryptor.mDecryptFunc = ghiEncryptorSslDecryptFunc;
    connection->encryptor.mInitialized = GHTTPFalse;
    connection->encryptor.mSessionStarted = GHTTPFalse;
    connection->encryptor.mSessionEstablished = GHTTPFalse;
    connection->encryptor.mEncryptOnBuffer = GHTTPTrue;
    connection->encryptor.mEncryptOnSend = GHTTPFalse;
    connection->encryptor.mLibSendsHandshakeMessages = GHTTPFalse;
    return GHTTPTrue;
  }
}

typedef struct gsRevoExInterface {
  SSLId mId;
  int mClientCertId;
  int mRootCAId;
  GHTTPBool mConnected;
} gsRevoExInterface;

GHIEncryptionResult ghiEncryptorSslInitFunc(GHIConnection* connection,
                                           GHIEncryptor* theEncryptor) {
  gsRevoExInterface* sslInterface = NULL;

  theEncryptor->mInterface = gsimalloc(sizeof(gsRevoExInterface));
  if (theEncryptor->mInterface == NULL)
    return GHIEncryptionResult_Error;

  memset(theEncryptor->mInterface, 0, sizeof(gsRevoExInterface));
  sslInterface = (gsRevoExInterface*)theEncryptor->mInterface;

  sslInterface->mId = SSLNew(0xB, connection->serverAddress);
  if (SSLSetBuiltinRootCA(sslInterface->mId, 1) != SSL_RESULT_OK) {
    SSLShutdown(sslInterface->mId);
    return GHIEncryptionResult_Error;
  }
  if (SSLSetBuiltinClientCert(sslInterface->mId, 0) != SSL_RESULT_OK) {
    SSLShutdown(sslInterface->mId);
    return GHIEncryptionResult_Error;
  }

  theEncryptor->mInitialized = GHTTPTrue;
  theEncryptor->mSessionStarted = GHTTPFalse;
  theEncryptor->mSessionEstablished = GHTTPFalse;
  theEncryptor->mEncryptOnBuffer = GHTTPFalse;
  theEncryptor->mEncryptOnSend = GHTTPTrue;
  theEncryptor->mLibSendsHandshakeMessages = GHTTPTrue;
  return GHIEncryptionResult_Success;
}

GHIEncryptionResult ghiEncryptorSslCleanupFunc(GHIConnection* connection,
                                              GHIEncryptor* theEncryptor) {
  if (theEncryptor != NULL) {
    gsRevoExInterface* sslInterface =
        (gsRevoExInterface*)theEncryptor->mInterface;
    if (sslInterface != NULL) {
      SSLShutdown(sslInterface->mId);
      gsifree(sslInterface);
      theEncryptor->mInterface = NULL;
    }
    theEncryptor->mInitialized = GHTTPFalse;
    theEncryptor->mSessionStarted = GHTTPFalse;
    theEncryptor->mSessionEstablished = GHTTPFalse;
  }
  return GHIEncryptionResult_Success;
}

GHIEncryptionResult ghiEncryptorSslStartFunc(GHIConnection* connection,
                                            GHIEncryptor* theEncryptor) {
  gsRevoExInterface* sslInterface =
      (gsRevoExInterface*)theEncryptor->mInterface;
  int result = 0;

  if (!sslInterface->mConnected) {
    result = SSLConnect(sslInterface->mId, connection->socket);
    if (result != SSL_RESULT_OK)
      return GHIEncryptionResult_Error;
    sslInterface->mConnected = GHTTPTrue;
  }

  result = SSLDoHandshake(sslInterface->mId);
  if (result != SSL_RESULT_OK) {
    if (result == SSL_EWANT_READ || result == SSL_EWANT_WRITE)
      return GHIEncryptionResult_Success;
    return GHIEncryptionResult_Error;
  }

  theEncryptor->mSessionStarted = GHTTPTrue;
  theEncryptor->mSessionEstablished = GHTTPTrue;
  return GHIEncryptionResult_Success;
}

GHIEncryptionResult ghiEncryptorSslEncryptSend(
    GHIConnection* connection, GHIEncryptor* theEncryptor,
    const char* thePlainTextBuffer, int thePlainTextLength,
    int* theBytesSent) {
  gsRevoExInterface* sslInterface =
      (gsRevoExInterface*)theEncryptor->mInterface;
  int result = SSLWrite(sslInterface->mId, thePlainTextBuffer,
                        thePlainTextLength);
  if (result == SSL_EZERO_RETURN)
    *theBytesSent = 0;
  else if (result == SSL_EWANT_WRITE)
    *theBytesSent = -1;
  else if (result < 0)
    return GHIEncryptionResult_Error;
  else
    *theBytesSent = result;
  return GHIEncryptionResult_Success;
}

GHIEncryptionResult ghiEncryptorSslDecryptRecv(
    GHIConnection* connection, GHIEncryptor* theEncryptor,
    char* theDecryptedBuffer, int* theDecryptedLength) {
  gsRevoExInterface* sslInterface =
      (gsRevoExInterface*)theEncryptor->mInterface;
  int result = SSLRead(sslInterface->mId, theDecryptedBuffer,
                       *theDecryptedLength);
  if (result == SSL_EZERO_RETURN)
    *theDecryptedLength = 0;
  else if (result == SSL_EWANT_READ)
    *theDecryptedLength = -1;
  else if (result < 0)
    return GHIEncryptionResult_Error;
  else
    *theDecryptedLength = result;
  return GHIEncryptionResult_Success;
}

// RevoEx encrypts and decrypts through socket I/O, not standalone buffers.
GHIEncryptionResult ghiEncryptorSslEncryptFunc(
    GHIConnection* connection, GHIEncryptor* theEncryptor,
    const char* thePlainTextBuffer, int thePlainTextLength,
    char* theEncryptedBuffer, int* theEncryptedLength) {
  return GHIEncryptionResult_Error;
}

GHIEncryptionResult ghiEncryptorSslDecryptFunc(
    GHIConnection* connection, GHIEncryptor* theEncryptor,
    const char* theEncryptedBuffer, int* theEncryptedLength,
    char* theDecryptedBuffer, int* theDecryptedLength) {
  return GHIEncryptionResult_Error;
}
