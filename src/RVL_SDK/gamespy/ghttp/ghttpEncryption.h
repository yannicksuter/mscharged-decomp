///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#pragma once

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//#include "ghttpCommon.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Encryption method
typedef enum {
  GHIEncryptionMethod_None,
  GHIEncryptionMethod_Encrypt, // encrypt raw data written to buffer
  GHIEncryptionMethod_Decrypt  // decrypt raw data written to buffer
} GHIEncryptionMethod;

// Encryption results
typedef enum {
  GHIEncryptionResult_None,
  GHIEncryptionResult_Success,        // successfully encrypted/decrypted
  GHIEncryptionResult_BufferTooSmall, // buffer was too small to hold converted
                                      // data
  GHIEncryptionResult_Error           // some other kind of error
} GHIEncryptionResult;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
struct GHIEncryptor; // forward declare for callbacks
struct GHIConnection;

// Called to init the encryption engine
typedef GHIEncryptionResult (*GHTTPEncryptorInitFunc)(
    struct GHIConnection* theConnection, struct GHIEncryptor* theEncryptor);

// Called to connect the socket (some engines do this internally)
typedef GHIEncryptionResult (*GHTTPEncryptorConnectFunc)(
    struct GHIConnection* theConnection, struct GHIEncryptor* theEncryptor);

// Called to start the handshake process engine
typedef GHIEncryptionResult (*GHTTPEncryptorStartFunc)(
    struct GHIConnection* theConnection, struct GHIEncryptor* theEncryptor);

// Called to destroy the encryption engine
typedef GHIEncryptionResult (*GHTTPEncryptorCleanupFunc)(
    struct GHIConnection* theConnection, struct GHIEncryptor* theEncryptor);

// Called when data needs to be encrypted
//    - entire plain text buffer will be encrypted
typedef GHIEncryptionResult (*GHTTPEncryptorEncryptFunc)(
    struct GHIConnection* theConnection, struct GHIEncryptor* theEncryptor,
    const char* thePlainTextBuffer,
    int thePlainTextLength, // [in]
    char* theEncryptedBuffer,
    int* theEncryptedLength); // [in/out]

// Called when data needs to be decrypted
//    - encrypted data may be left in the buffer
//    - decrypted buffer is appended to, not overwritten
typedef GHIEncryptionResult (*GHTTPEncryptorDecryptFunc)(
    struct GHIConnection* theConnection, struct GHIEncryptor* theEncryptor,
    const char* theEncryptedBuffer,
    int* theEncryptedLength, // [in/out]
    char* theDecryptedBuffer,
    int* theDecryptedLength); // [in/out]

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
typedef struct GHIEncryptor {
  void* mInterface; // only SSL is currently supported
  GHTTPEncryptionEngine mEngine;
  GHTTPBool mInitialized;
#if defined(VERSION_R4QK01)
  GHTTPBool mSessionStarted; // handshake started?
#endif
  GHTTPBool mSessionEstablished; // handshake completed?
#if defined(VERSION_R4QK01)
  GHTTPBool mEncryptOnBuffer; // encrypt when writing to a buffer
  GHTTPBool mEncryptOnSend; // encrypt when sending over the socket
  GHTTPBool mLibSendsHandshakeMessages;
#endif

  // Functions for engine use
  GHTTPEncryptorInitFunc mInitFunc;
  GHTTPEncryptorCleanupFunc mCleanupFunc;
#if defined(VERSION_R4QK01)
  GHTTPEncryptorStartFunc mStartFunc;
#endif
  GHTTPEncryptorEncryptFunc mEncryptFunc;
  GHTTPEncryptorDecryptFunc mDecryptFunc;
} GHIEncryptor;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// ssl encryption
GHIEncryptionResult ghiEncryptorSslInitFunc(struct GHIConnection* connection,
                                            struct GHIEncryptor* theEncryptor);
GHIEncryptionResult
ghiEncryptorSslCleanupFunc(struct GHIConnection* connection,
                           struct GHIEncryptor* theEncryptor);

GHIEncryptionResult ghiEncryptorSslStartFunc(struct GHIConnection* connection,
                                             struct GHIEncryptor* theEncryptor);

GHIEncryptionResult ghiEncryptorSslEncryptFunc(
    struct GHIConnection* connection, struct GHIEncryptor* theEncryptor,
    const char* thePlainTextBuffer, int thePlainTextLength,
    char* theEncryptedBuffer, int* theEncryptedLength);
GHIEncryptionResult ghiEncryptorSslDecryptFunc(
    struct GHIConnection* connection, struct GHIEncryptor* theEncryptor,
    const char* theEncryptedBuffer, int* theEncryptedLength,
    char* theDecryptedBuffer, int* theDecryptedLength);

#if defined(VERSION_R4QK01)
GHIEncryptionResult ghiEncryptorSslEncryptSend(
    struct GHIConnection* connection, struct GHIEncryptor* theEncryptor,
    const char* thePlainTextBuffer, int thePlainTextLength, int* theBytesSent);
GHIEncryptionResult ghiEncryptorSslDecryptRecv(
    struct GHIConnection* connection, struct GHIEncryptor* theEncryptor,
    char* theDecryptedBuffer, int* theDecryptedLength);
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
