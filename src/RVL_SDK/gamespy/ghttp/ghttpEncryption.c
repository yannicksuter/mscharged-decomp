///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#include "ghttpCommon.h"

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
GHTTPBool ghttpSetRequestEncryptionEngine(GHTTPRequest request,
                                          GHTTPEncryptionEngine engine) {
  GHIConnection* connection = ghiRequestToConnection(request);
  if (!connection)
    return GHTTPFalse;

  // If the same engine has previously been set then we're done
  if (connection->encryptor.mEngine == engine)
    return GHTTPTrue;

  // If a different engine has previously been set then we're screwed
  if (connection->encryptor.mInterface != NULL &&
      connection->encryptor.mEngine != engine) {
    return GHTTPFalse;
  }

  // If the URL is HTTPS but the engine is specific as NONE then we can't
  // connect
  if ((engine == GHTTPEncryptionEngine_None) &&
      (strncmp(connection->URL, "https://", 8) == 0))
    return GHTTPFalse;

  // Initialize the engine
  connection->encryptor.mEngine = engine;

  if (engine == GHTTPEncryptionEngine_None) {
    connection->encryptor.mInterface = NULL;
    return GHTTPTrue; // this is the default, just return
  } else {
    connection->encryptor.mInterface = NULL;
    connection->encryptor.mInitFunc = ghiEncryptorSslInitFunc;
    connection->encryptor.mCleanupFunc = ghiEncryptorSslCleanupFunc;
    connection->encryptor.mEncryptFunc = ghiEncryptorSslEncryptFunc;
    connection->encryptor.mDecryptFunc = ghiEncryptorSslDecryptFunc;
    connection->encryptor.mInitialized = GHTTPFalse;
    connection->encryptor.mSessionEstablished = GHTTPFalse;
    return GHTTPTrue;
  }
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#include "../common/gsSSL.h"
#include "../common/gsSHA1.h"
#include "../common/gsRC4.h"
#include "../md5.h"

// Processor for SSL state messages (transparent to application)
static GHIEncryptionResult
ghiEncryptorProcessSSLHandshake(struct GHIConnection* connection,
                                struct GHIEncryptor* encryptor,
                                GHIBuffer* data);

// SSL-ASN.1 lengths are variable length NBO integers
//    we use this utility to make data packing easier
//    example: 61 little-endian(intel) = 61 00 00 00
//                big-endian(network)  = 00 00 00 61
static void ghiEncryptorWriteNBOLength(unsigned char* buf, int value,
                                       int size) {
  int NBO = (int)htonl(value);
  unsigned char* NBOData = (unsigned char*)&NBO;

  assert(size <= sizeof(NBO));
  if (size > sizeof(NBO))
    return; // can't write more than 4 bytes!

  // this won't work if NBO ever changes from big-endian
  memcpy(buf, NBOData + (sizeof(int) - size), (size_t)size);
}
static GHTTPBool ghiEncryptorReadNBOLength(GHIBuffer* data, int* value,
                                           int size) {
  assert(size <= sizeof(*value));
  if (size > sizeof(*value))
    return GHTTPFalse;
  if (GHTTPFalse ==
      ghiReadDataFromBufferFixed(data, ((char*)value) + (sizeof(int) - size),
                                 size))
    return GHTTPFalse;

  *value = (int)htonl(*value);
  return GHTTPTrue;
}

static GHTTPBool ghiEncryptorParseASN1Sequence(GHIBuffer* data, int* lenOut) {
  char tempChar = '\0';

  if (GHTTPFalse == ghiReadDataFromBufferFixed(data, &tempChar, 1))
    return GHTTPFalse;
  if (tempChar != 0x30) // sequence start byte
    return GHTTPFalse;

  if (GHTTPFalse == ghiReadDataFromBufferFixed(data, &tempChar, 1))
    return GHTTPFalse;
  if ((tempChar & 0x80) == 0x80) {
    int tempInt = 0;

    // length is stored in next (tempChar^0x80) bytes
    tempChar ^= 0x80;
    if (GHTTPFalse == ghiEncryptorReadNBOLength(data, &tempInt, tempChar))
      return GHTTPFalse;
    if (tempInt > (data->len - data->pos))
      return GHTTPFalse;

    *lenOut = tempInt;
    return GHTTPTrue;
  } else {
    if ((int)tempChar > (data->len - data->pos))
      return GHTTPFalse;

    *lenOut = tempChar;
    return GHTTPTrue;
  }
}

static void ghiEncryptorGenerateEncryptionKeys(gsSSL* sslInterface) {
  // Use the server random, client random and pre master secret
  // to compute the encryption key.

  // SSLv3 style
  //  master_secret = {
  //		MD5(pre_master_secret +
  //SHA1("A"+pre_master_secret+client_random+server_random)) +
  //		MD5(pre_master_secret +
  //SHA1("BB"+pre_master_secret+client_random+server_random)) +
  //		MD5(pre_master_secret +
  //SHA1("CCC"+pre_master_secret+client_random+server_random))
  //  }
  //  key_block = {
  //		MD5(master_secret +
  //SHA1("A"+master_secret+server_random+client_random)) +
  //		MD5(master_secret +
  //SHA1("BB"+master_secret+server_random+client_random)) +
  //		MD5(master_secret +
  //SHA1("CCC"+master_secret+server_random+client_random))

  SHA1Context sha1;
  MD5_CTX md5;
  unsigned char temp[SHA1HashSize];

  unsigned int randomSize = 32;
  unsigned char keyblock[64]; // todo: support different key sizes

  // master_secret "A"
  SHA1Reset(&sha1);
  SHA1Input(&sha1, (const unsigned char*)"A", 1);
  SHA1Input(&sha1, (const unsigned char*)sslInterface->premastersecret,
            GS_SSL_MASTERSECRET_LEN);
  SHA1Input(&sha1, (const unsigned char*)sslInterface->clientRandom,
            randomSize);
  SHA1Input(&sha1, (const unsigned char*)sslInterface->serverRandom,
            randomSize);
  SHA1Result(&sha1, temp);
  MD5Init(&md5);
  MD5Update(&md5, (unsigned char*)sslInterface->premastersecret,
            GS_SSL_MASTERSECRET_LEN);
  MD5Update(&md5, temp, GS_CRYPT_SHA1_HASHSIZE);
  MD5Final((unsigned char*)&sslInterface->mastersecret[0 * GS_CRYPT_MD5_HASHSIZE],
           &md5);

  // master_secret "BB"
  SHA1Reset(&sha1);
  SHA1Input(&sha1, (const unsigned char*)"BB", 2);
  SHA1Input(&sha1, (const unsigned char*)sslInterface->premastersecret,
            GS_SSL_MASTERSECRET_LEN);
  SHA1Input(&sha1, (const unsigned char*)sslInterface->clientRandom,
            randomSize);
  SHA1Input(&sha1, (const unsigned char*)sslInterface->serverRandom,
            randomSize);
  SHA1Result(&sha1, temp);
  MD5Init(&md5);
  MD5Update(&md5, (unsigned char*)sslInterface->premastersecret,
            GS_SSL_MASTERSECRET_LEN);
  MD5Update(&md5, temp, GS_CRYPT_SHA1_HASHSIZE);
  MD5Final((unsigned char*)&sslInterface->mastersecret[1 * GS_CRYPT_MD5_HASHSIZE],
           &md5);

  // master_secret "CCC"
  SHA1Reset(&sha1);
  SHA1Input(&sha1, (const unsigned char*)"CCC", 3);
  SHA1Input(&sha1, (const unsigned char*)sslInterface->premastersecret,
            GS_SSL_MASTERSECRET_LEN);
  SHA1Input(&sha1, (const unsigned char*)sslInterface->clientRandom,
            randomSize);
  SHA1Input(&sha1, (const unsigned char*)sslInterface->serverRandom,
            randomSize);
  SHA1Result(&sha1, temp);
  MD5Init(&md5);
  MD5Update(&md5, (unsigned char*)sslInterface->premastersecret,
            GS_SSL_MASTERSECRET_LEN);
  MD5Update(&md5, temp, GS_CRYPT_SHA1_HASHSIZE);
  MD5Final((unsigned char*)&sslInterface->mastersecret[2 * GS_CRYPT_MD5_HASHSIZE],
           &md5);

  // key_block "A"
  SHA1Reset(&sha1);
  SHA1Input(&sha1, (const unsigned char*)"A", 1);
  SHA1Input(&sha1, (const unsigned char*)sslInterface->mastersecret,
            GS_SSL_MASTERSECRET_LEN);
  SHA1Input(&sha1, (const unsigned char*)sslInterface->serverRandom,
            randomSize);
  SHA1Input(&sha1, (const unsigned char*)sslInterface->clientRandom,
            randomSize);
  SHA1Result(&sha1, temp);
  MD5Init(&md5);
  MD5Update(&md5, (unsigned char*)sslInterface->mastersecret,
            GS_SSL_MASTERSECRET_LEN);
  MD5Update(&md5, temp, GS_CRYPT_SHA1_HASHSIZE);
  MD5Final(&keyblock[0 * GS_CRYPT_MD5_HASHSIZE], &md5);

  // key_block "BB"
  SHA1Reset(&sha1);
  SHA1Input(&sha1, (const unsigned char*)"BB", 2);
  SHA1Input(&sha1, (const unsigned char*)sslInterface->mastersecret,
            GS_SSL_MASTERSECRET_LEN);
  SHA1Input(&sha1, (const unsigned char*)sslInterface->serverRandom,
            randomSize);
  SHA1Input(&sha1, (const unsigned char*)sslInterface->clientRandom,
            randomSize);
  SHA1Result(&sha1, temp);
  MD5Init(&md5);
  MD5Update(&md5, (unsigned char*)sslInterface->mastersecret,
            GS_SSL_MASTERSECRET_LEN);
  MD5Update(&md5, temp, GS_CRYPT_SHA1_HASHSIZE);
  MD5Final(&keyblock[1 * GS_CRYPT_MD5_HASHSIZE], &md5);

  // key_block "CCC"
  SHA1Reset(&sha1);
  SHA1Input(&sha1, (const unsigned char*)"CCC", 3);
  SHA1Input(&sha1, (const unsigned char*)sslInterface->mastersecret,
            GS_SSL_MASTERSECRET_LEN);
  SHA1Input(&sha1, (const unsigned char*)sslInterface->serverRandom,
            randomSize);
  SHA1Input(&sha1, (const unsigned char*)sslInterface->clientRandom,
            randomSize);
  SHA1Result(&sha1, temp);
  MD5Init(&md5);
  MD5Update(&md5, (unsigned char*)sslInterface->mastersecret,
            GS_SSL_MASTERSECRET_LEN);
  MD5Update(&md5, temp, GS_CRYPT_SHA1_HASHSIZE);
  MD5Final(&keyblock[2 * GS_CRYPT_MD5_HASHSIZE], &md5);

  // key_block "DDDD"
  SHA1Reset(&sha1);
  SHA1Input(&sha1, (const unsigned char*)"DDDD", 4);
  SHA1Input(&sha1, (const unsigned char*)sslInterface->mastersecret,
            GS_SSL_MASTERSECRET_LEN);
  SHA1Input(&sha1, (const unsigned char*)sslInterface->serverRandom,
            randomSize);
  SHA1Input(&sha1, (const unsigned char*)sslInterface->clientRandom,
            randomSize);
  SHA1Result(&sha1, temp);
  MD5Init(&md5);
  MD5Update(&md5, (unsigned char*)sslInterface->mastersecret,
            GS_SSL_MASTERSECRET_LEN);
  MD5Update(&md5, temp, GS_CRYPT_SHA1_HASHSIZE);
  MD5Final(&keyblock[3 * GS_CRYPT_MD5_HASHSIZE], &md5);

  // key_block "EEEEE"
  // key_block "FFFFFF"
  // ... continue if more key material is needed

  // todo: support different key sizes
  // KEYBLOCK
  //    writemac[16], readmac[16], writekey[16], readkey[16], writeIV[0],
  //    readIV[0]
  memcpy(sslInterface->clientWriteMACSecret, &keyblock[0], 16);
  memcpy(sslInterface->clientReadMACSecret, &keyblock[16], 16);
  memcpy(sslInterface->clientWriteKey, &keyblock[32], 16);
  memcpy(sslInterface->clientReadKey, &keyblock[48], 16);

  sslInterface->clientWriteMACLen = 16;
  sslInterface->clientReadMACLen = 16;
  sslInterface->clientWriteKeyLen = 16;
  sslInterface->clientReadKeyLen = 16;

  // Init the stream cipher
  RC4Init(&sslInterface->sendRC4,
          (const unsigned char*)sslInterface->clientWriteKey,
          sslInterface->clientWriteKeyLen);
  RC4Init(&sslInterface->recvRC4,
          (const unsigned char*)sslInterface->clientReadKey,
          sslInterface->clientReadKeyLen);
}

// Init the engine
GHIEncryptionResult ghiEncryptorSslInitFunc(struct GHIConnection* connection,
                                            struct GHIEncryptor* theEncryptor) {
  gsSSL* sslInterface = NULL;
  gsSSLClientHelloMsg helloMsg;
  int i = 0;

  // There is only one place where this function should be called,
  //  and it should check if the engine has been initialized
  assert(theEncryptor->mInitialized == GHTTPFalse);
  assert(theEncryptor->mInterface == NULL);

  if ((connection->sendBuffer.size - connection->sendBuffer.len) <
      sizeof(gsSSLClientHelloMsg))
    return GHIEncryptionResult_BufferTooSmall;

  // allocate the interface (need one per connection)
  theEncryptor->mInterface = gsimalloc(sizeof(gsSSL));
  if (theEncryptor->mInterface == NULL) {
    // memory allocation failed
    return GHIEncryptionResult_Error;
  }
  memset(theEncryptor->mInterface, 0, sizeof(gsSSL));
  sslInterface = (gsSSL*)theEncryptor->mInterface;

  theEncryptor->mInitialized = GHTTPTrue;
  theEncryptor->mSessionEstablished = GHTTPFalse;
  MD5Init(&sslInterface->finishHashMD5);
  SHA1Reset(&sslInterface->finishHashSHA1);

  // prepare the client hello
  //    1)
  helloMsg.header.contentType = GS_SSL_CONTENT_HANDSHAKE;
  helloMsg.header.versionMajor = GS_SSL_VERSION_MAJOR;
  helloMsg.header.versionMinor = GS_SSL_VERSION_MINOR;

  // Set the length of the client hello message (2-byte NBO int, not including
  // record header)
  ghiEncryptorWriteNBOLength(helloMsg.header.lengthNBO,
                             sizeof(gsSSLClientHelloMsg) -
                                 sizeof(gsSSLRecordHeaderMsg),
                             2);

  helloMsg.handshakeType = GS_SSL_HANDSHAKE_CLIENTHELLO;
  helloMsg.versionMajor = GS_SSL_VERSION_MAJOR;
  helloMsg.versionMinor = GS_SSL_VERSION_MINOR;

  // Set the length of the client hello data (3 byte NBO int)
  //    This is the total message length MINUS the SSL record header MINUS four
  //    additional header bytes
  ghiEncryptorWriteNBOLength(helloMsg.lengthNBO,
                             sizeof(gsSSLClientHelloMsg) -
                                 sizeof(gsSSLRecordHeaderMsg) - 4,
                             3);
  ghiEncryptorWriteNBOLength(helloMsg.time, 0, 4); // test code: no randomness

  // fill in the [rest of the] random
  //   Security Note: If a hacker is able to discern the current_time() they may
  //   be able to recreate the random bytes and recover the session key.
  Util_RandSeed(current_time());
  for (i = 0; i < 28; i++) {
    helloMsg.random[i] = (unsigned char)Util_RandInt(0, 0xff);
  }

  // store a copy of the random (used later for key generation)
  memcpy(&sslInterface->clientRandom[0], helloMsg.time, 4);
  memcpy(&sslInterface->clientRandom[4], helloMsg.random, 28);

  // todo: session resumption
  helloMsg.sessionIdLen = 0;

  // fill in cipher suite IDs
  helloMsg.cipherSuitesLength =
      htons(sizeof(gsi_u16) * GS_SSL_NUM_CIPHER_SUITES);
  for (i = 0; i < GS_SSL_NUM_CIPHER_SUITES; i++)
    helloMsg.cipherSuites[i] =
        htons((unsigned short)gsSSLCipherSuites[i].mSuiteID);

  // there are no standard SSL compression methods
  helloMsg.compressionMethodLen = 1;
  helloMsg.compressionMethodList = 0;

  // We need to compute a hash of all the handshake messages
  //    Add this message to the hash (both MD5 hash and SHA1 hash)
  MD5Update(&sslInterface->finishHashMD5,
            (unsigned char*)&helloMsg + sizeof(gsSSLRecordHeaderMsg),
            sizeof(gsSSLClientHelloMsg) - sizeof(gsSSLRecordHeaderMsg));
  SHA1Input(&sslInterface->finishHashSHA1,
            (unsigned char*)&helloMsg + sizeof(gsSSLRecordHeaderMsg),
            sizeof(gsSSLClientHelloMsg) - sizeof(gsSSLRecordHeaderMsg));

  // Now send it (we already verified the length, so this should not fail)
  if (GHTTPFalse == ghiAppendDataToBuffer(&connection->sendBuffer,
                                          (const char*)&helloMsg,
                                          sizeof(gsSSLClientHelloMsg))) {
    // assert or just return?
    return GHIEncryptionResult_BufferTooSmall;
  }

  return GHIEncryptionResult_Success;
}

// Destroy the engine
GHIEncryptionResult
ghiEncryptorSslCleanupFunc(struct GHIConnection* connection,
                           struct GHIEncryptor* theEncryptor) {
  if (theEncryptor != NULL) {
    gsSSL* sslInterface = (gsSSL*)theEncryptor->mInterface;
    if (sslInterface != NULL) {
      gsifree(sslInterface);
      theEncryptor->mInterface = NULL;
    }
    theEncryptor->mInitialized = GHTTPFalse;
    theEncryptor->mSessionEstablished = GHTTPFalse;
  }

  GSI_UNUSED(connection);

  return GHIEncryptionResult_Success;
}

// Encrypt some data
//    -  theEncryptedLength is reduced by the length of data written to
//    theEncryptedBuffer
// So if the encrypted buffer is 255 bytes long and we write 50 additional
// bytes, we'll return 205.
GHIEncryptionResult ghiEncryptorSslEncryptFunc(
    struct GHIConnection* connection, struct GHIEncryptor* theEncryptor,
    const char* thePlainTextBuffer, int thePlainTextLength,
    char* theEncryptedBuffer, int* theEncryptedLength) {
  if (theEncryptor != NULL) {
    gsSSL* sslInterface = (gsSSL*)theEncryptor->mInterface;
    if (sslInterface == NULL ||
        theEncryptor->mSessionEstablished == GHTTPFalse) {
      // not secured yet, send as plain text
      if (thePlainTextLength > *theEncryptedLength)
        return GHIEncryptionResult_BufferTooSmall;
      memcpy(theEncryptedBuffer, thePlainTextBuffer,
             (size_t)thePlainTextLength);
      *theEncryptedLength += thePlainTextLength; // number of bytes written
    } else {
      // Create an SSL encrypted record
      //    The order of operations below is very important.
      //    The MAC must be computed before ciphering the plain text because
      //    theEncryptedBuffer may be the same memory location as
      //    thePlainTextBuffer

      gsSSL* sslInterface = (gsSSL*)theEncryptor->mInterface;
      gsSSLRecordHeaderMsg* header = NULL;
      MD5_CTX md5;
      int pos = 0;
      unsigned short lengthNBO = htons((unsigned short)thePlainTextLength);
      unsigned char MAC[GS_CRYPT_MD5_HASHSIZE];

      // The SSL record adds a little overhead
      if (*theEncryptedLength <
          (thePlainTextLength + (int)sizeof(gsSSLRecordHeaderMsg)))
        return GHIEncryptionResult_BufferTooSmall;

      // write the SSL header
      header = (gsSSLRecordHeaderMsg*)theEncryptedBuffer;
      header->contentType = GS_SSL_CONTENT_APPLICATIONDATA;
      header->versionMajor = GS_SSL_VERSION_MAJOR;
      header->versionMinor = GS_SSL_VERSION_MINOR;
      pos += sizeof(gsSSLRecordHeaderMsg);

      // calculate the MAC
      MD5Init(&md5);
      MD5Update(&md5, sslInterface->clientWriteMACSecret,
                (unsigned int)sslInterface->clientWriteMACLen);
      MD5Update(&md5, (unsigned char*)GS_SSL_PAD_ONE, GS_SSL_MD5_PAD_LEN);
      MD5Update(&md5, sslInterface->sendSeqNBO,
                sizeof(sslInterface->sendSeqNBO));
      MD5Update(&md5, (unsigned char*)"\x17", 1); // content type application
                                                  // data
      MD5Update(&md5, (unsigned char*)&lengthNBO, sizeof(lengthNBO));
      MD5Update(&md5, (unsigned char*)thePlainTextBuffer,
                (unsigned int)thePlainTextLength); // **cast-away const**
      MD5Final(MAC, &md5); // first half of MAC

      MD5Init(&md5);
      MD5Update(&md5, sslInterface->clientWriteMACSecret,
                (unsigned int)sslInterface->clientWriteMACLen);
      MD5Update(&md5, (unsigned char*)GS_SSL_PAD_TWO, GS_SSL_MD5_PAD_LEN);
      MD5Update(&md5, MAC, GS_CRYPT_MD5_HASHSIZE);
      MD5Final(MAC, &md5); // complete MAC

      // apply stream cipher to data + MAC
      RC4Encrypt(&sslInterface->sendRC4,
                 (const unsigned char*)thePlainTextBuffer,
                 (unsigned char*)&theEncryptedBuffer[pos], thePlainTextLength);
      pos += thePlainTextLength;
      RC4Encrypt(&sslInterface->sendRC4, MAC,
                 (unsigned char*)&theEncryptedBuffer[pos],
                 GS_CRYPT_MD5_HASHSIZE);
      pos += GS_CRYPT_MD5_HASHSIZE;

      // Now that we know the final length (data+mac+pad), write it into the
      // header
      ghiEncryptorWriteNBOLength(header->lengthNBO,
                                 (int)(pos - sizeof(gsSSLRecordHeaderMsg)), 2);

      // adjust encrypted length
      *theEncryptedLength -= pos;

      // Update the sequence number for the next message (8-byte, NBO)
      pos = 7; // **changing the semantic of variable "pos"
      do {
        if (sslInterface->sendSeqNBO[pos] == 0xFF) // wraparound means carry
        {
          sslInterface->sendSeqNBO[pos] = 0;
          pos -= 1;
        } else {
          sslInterface->sendSeqNBO[pos] += 1;
          pos = 0; // end addition
        }
      } while (pos >= 0);
    }
  }

  GSI_UNUSED(connection);
  return GHIEncryptionResult_Success;
}

// Decrypt some data
//    -  During the handshaking process, this may result in data being appended
//    to the send buffer
//    -  Data may be left in the encrypted buffer
//    -  theEncryptedLength becomes the length of data read from
//    theEncryptedBuffer
//    -  theDecryptedLength becomes the length of data written to
//    theDecryptedBuffer
GHIEncryptionResult ghiEncryptorSslDecryptFunc(
    struct GHIConnection* connection, struct GHIEncryptor* theEncryptor,
    const char* theEncryptedBuffer, int* theEncryptedLength,
    char* theDecryptedBuffer, int* theDecryptedLength) {
  gsSSL* sslInterface = NULL;
  int readPos = 0;
  int writePos = 0;

  // Make sure we have a valid encryptor
  assert(theEncryptor != NULL);
  assert(theEncryptor->mInterface != NULL);
  if (theEncryptor == NULL || theEncryptor->mInterface == NULL) {
    // no encryption set? copy as plain text
    memcpy(theDecryptedBuffer, theEncryptedBuffer,
           (size_t)(*theEncryptedLength));
    *theDecryptedLength = *theEncryptedLength;
    *theEncryptedLength = 0; // no bytes remaining
    return GHIEncryptionResult_Success;
  }

  sslInterface = (gsSSL*)theEncryptor->mInterface;
  if (sslInterface == NULL)
    return GHIEncryptionResult_Error;

  // Read each SSL message from the stream (leave partial messages)
  while (readPos < *theEncryptedLength) {
    gsSSLRecordHeaderMsg* header = NULL;
    unsigned short length = 0;
    GHIEncryptionResult result;

    // make sure we have the complete record header
    if ((*theEncryptedLength - readPos) < sizeof(gsSSLRecordHeaderMsg))
      break;
    header = (gsSSLRecordHeaderMsg*)&theEncryptedBuffer[readPos];

    // make sure we have the complete record data
    length =
        ntohs((unsigned short)(header->lengthNBO[0] | (header->lengthNBO[1] << 8)));
    if (*theEncryptedLength <
        (readPos + length + (int)sizeof(gsSSLRecordHeaderMsg)))
      break; // wait for more data

    // if we have to decrypt, make sure there is room in the decrypt buffer
    if (connection->encryptor.mSessionEstablished) {
      if ((*theDecryptedLength - writePos) < length) {
        *theEncryptedLength = readPos;  // bytes read *NOT* bytes remaining
        *theDecryptedLength = writePos; // bytes written

        if (*theDecryptedLength > 0)
          return GHIEncryptionResult_Success;
        else
          return GHIEncryptionResult_BufferTooSmall;
      }
    }

    // process the record data
    switch (header->contentType) {
    case GS_SSL_CONTENT_HANDSHAKE: {
      GHIBuffer data;

      // Apply stream cipher if the session has been established
      readPos += sizeof(gsSSLRecordHeaderMsg);
      if (connection->encryptor.mSessionEstablished)
        RC4Encrypt(&sslInterface->recvRC4,
                   (const unsigned char*)&theEncryptedBuffer[readPos],
                   (unsigned char*)&theEncryptedBuffer[readPos], length);

      ghiInitReadOnlyBuffer(connection, &data, &theEncryptedBuffer[readPos],
                            length);

      result = ghiEncryptorProcessSSLHandshake(connection, theEncryptor,
                                               &data);
      if (result != GHIEncryptionResult_Success)
        return result; // error!

      break;
    }
    case GS_SSL_CONTENT_APPLICATIONDATA: {
      // Apply stream cipher if the session has been established
      readPos += sizeof(gsSSLRecordHeaderMsg);
      if (connection->encryptor.mSessionEstablished)
        RC4Encrypt(&sslInterface->recvRC4,
                   (const unsigned char*)&theEncryptedBuffer[readPos],
                   (unsigned char*)&theEncryptedBuffer[readPos], length);

      // verify MAC and pad
      // verifyMAC();

      // copy to decrypted buffer so HTTP layer can process
      memcpy(theDecryptedBuffer + writePos, &theEncryptedBuffer[readPos],
             (size_t)(length - GS_CRYPT_MD5_HASHSIZE));
      writePos += length - GS_CRYPT_MD5_HASHSIZE;
      break;
    }

    case GS_SSL_CONTENT_CHANGECIPHERSPEC:
      readPos += sizeof(gsSSLRecordHeaderMsg);
      connection->encryptor.mSessionEstablished = GHTTPTrue;
      break;

    case GS_SSL_CONTENT_ALERT:
      readPos += sizeof(gsSSLRecordHeaderMsg);
      // server alert
      break;

    default:
      readPos += sizeof(gsSSLRecordHeaderMsg);
      return GHIEncryptionResult_Error; // unhandled content type
    };

    readPos += length;
  };

  // remove read bytes from the stream
  *theEncryptedLength = readPos;  // bytes read *NOT* bytes remaining
  *theDecryptedLength = writePos; // bytes written

  if (*theEncryptedLength < 0)
    return GHIEncryptionResult_Error;
  else
    return GHIEncryptionResult_Success;
}

static GHTTPBool ghiCertificateChainIsValid(gsSSL* sslInterface) {
  GSI_UNUSED(sslInterface);

  return GHTTPTrue;
}

#define CHECK(a)                                                               \
  {                                                                            \
    if (GHTTPFalse == a)                                                       \
      return GHIEncryptionResult_Error;                                        \
  }

// Programmer note:
//    The structure of these SSL handshake messages may seem a bit cryptic,
//    due to their variable length data items.  Refer to the ASN1/DER encoding
//    guide for tag specifics.
GHIEncryptionResult
ghiEncryptorProcessSSLHandshake(struct GHIConnection* connection,
                                struct GHIEncryptor* encryptor,
                                GHIBuffer* data) {
  // There may be multiple messages within the handshake message
  //  length must be completely used, otherwise it's a protocol error
  gsSSL* sslInterface = (gsSSL*)encryptor->mInterface;

  while (data->pos < data->len) {
    // Parse each SSL handshake message (there may be multiple)
    int messageStart = data->pos;
    char messageType = 0;
    CHECK(ghiReadDataFromBufferFixed(data, &messageType, 1));

    if (messageType == GS_SSL_HANDSHAKE_SERVERHELLO) {
      int totalMsgLen = 0; // length of header + data
      int msgDataLen = 0;  // length of data
      int tempInt = 0;
      char tempChar = '\0';

      // make sure we don't have a session already (e.g. dupe hello message)
      if (sslInterface->sessionLen != 0)
        return GHIEncryptionResult_Error; // abort connection

      CHECK(ghiEncryptorReadNBOLength(data, &msgDataLen, 3));

      // check reported size against the actual bytes remaining
      if (msgDataLen > (data->len - data->pos))
        return GHIEncryptionResult_Error; // abort connection

      // skip SSL version
      //    (length check not required because we did that above)
      data->pos += 2;

      // store server random (used for key generation)
      CHECK(ghiReadDataFromBufferFixed(
          data, (char*)&sslInterface->serverRandom[0], 32));

      // store session information (length followed by data)
      CHECK(ghiReadDataFromBufferFixed(data, &tempChar, 1));
      CHECK(ghiReadDataFromBufferFixed(data, (char*)sslInterface->sessionData,
                                       tempChar));
      sslInterface->sessionLen = (int)tempChar;

      // store cipher suite
      CHECK(ghiEncryptorReadNBOLength(data, &tempInt, 2));
      sslInterface->cipherSuite = (unsigned short)tempInt;

      // skip compression algorithms (should always be 0x00 since we don't
      // support any!)
      CHECK(ghiReadDataFromBufferFixed(data, &tempChar, 1));
      if (tempChar != 0x00)
        return GHIEncryptionResult_Error;

      // add it to the running handshake hash
      totalMsgLen = data->pos - messageStart;
      MD5Update(&sslInterface->finishHashMD5,
                (unsigned char*)&data->data[messageStart],
                (unsigned int)totalMsgLen);
      SHA1Input(&sslInterface->finishHashSHA1,
                (unsigned char*)&data->data[messageStart],
                (unsigned int)totalMsgLen);
    } else if (messageType == GS_SSL_HANDSHAKE_CERTIFICATE) {
      int msgLength = 0;   // combined length of the message (size in SSL
                           // message header)
      int certListLen = 0; // combined length of all certificates
      int totalMsgLen = 0; // our calculated msg length (for handshake hashing)

      int certCount = 0;
      int certListEndPos = 0;

      CHECK(ghiEncryptorReadNBOLength(data, &msgLength, 3));
      CHECK(ghiEncryptorReadNBOLength(data, &certListLen, 3));
      if (msgLength != certListLen + 3)
        return GHIEncryptionResult_Error;

      // make sure we have enough data to cover the certificate list
      certListEndPos = data->pos + certListLen;
      if (certListLen > (data->len - data->pos))
        return GHIEncryptionResult_Error;

      // read the certificates
      while (data->pos < certListEndPos) {
        int certLength = 0;
        int certStartPos = 0;

        int temp = 0;
        int version = 0;

        // Must start with a 3 byte length
        CHECK(ghiEncryptorReadNBOLength(data, &certLength, 3));

        // Make sure we have enough data to cover this certificate
        if (certLength > (data->len - data->pos))
          return GHIEncryptionResult_Error; // certificate too big

        // 0xFFFF is max message size in SSL v3.0, we don't currently support
        // split messages
        if (certLength > 0xFFFF)
          return GHIEncryptionResult_Error;

        certStartPos = data->pos; // remember this for a shortcut later
        certCount++;

        // The first certificate holds the server's public key
        if (certCount == 1) {
          // X.509 format is rather convoluted.  Since we only support
          // one variation anyways, I'm hardcoding the specific values
          // we require.  Anything else is a protocol error.
          //    0x30 marks the start of a sequence.  next byte is a length
          //    field size 0x82 is a length tag, meaning the next two bytes
          //    contain the length 0x81 is the same thing, only the next one
          //    byte contains the length The other values usually denote
          //    required types

          // Certificate SEQUENCE
          int seqLen = 0;
          CHECK(ghiEncryptorParseASN1Sequence(data, &seqLen));
          // todo: verify reported length of this sequence

          // TBSCertificate SEQUENCE
          CHECK(ghiEncryptorParseASN1Sequence(data, &seqLen));
          // todo: verify reported length of this sequence

          // EXPLICIT Version (must be one of: 0x03,0x02,0x01)
          if (5 > (data->len - data->pos))
            return GHIEncryptionResult_Error;
          if ((unsigned char)data->data[data->pos++] != 0xa0)
            return GHIEncryptionResult_Error;
          if ((unsigned char)data->data[data->pos++] != 0x03)
            return GHIEncryptionResult_Error;
          if ((unsigned char)data->data[data->pos++] != 0x02)
            return GHIEncryptionResult_Error;
          if ((unsigned char)data->data[data->pos++] != 0x01)
            return GHIEncryptionResult_Error;
          version = (unsigned char)data->data[data->pos++];

          // Serial Number (variable length, with 2-byte length field)
          if ((unsigned char)data->data[data->pos++] != 0x02)
            return GHIEncryptionResult_Error;
          temp = (unsigned char)data->data[data->pos++]; // len of serial
                                                         // number
          if (data->pos + temp > certListEndPos)
            return GHIEncryptionResult_Error;
          data->pos += temp; // skip the serial number

          // Signature algorithm identifier SEQUENCE
          CHECK(ghiEncryptorParseASN1Sequence(data, &seqLen));
          data->pos += seqLen; // skip algorithm ID (todo: verify signatures)

          // Issuer Name
          CHECK(ghiEncryptorParseASN1Sequence(data, &seqLen));
          data->pos += seqLen; // skip the issuer name sequence

          // Validity
          CHECK(ghiEncryptorParseASN1Sequence(data, &seqLen));
          data->pos += seqLen; // skip the validity sequence

          // Subject Name
          CHECK(ghiEncryptorParseASN1Sequence(data, &seqLen));
          data->pos += seqLen; // skip the subject name sequence

          // Subject Public Key Info
          CHECK(ghiEncryptorParseASN1Sequence(data, &seqLen));
          //     AlgorithmIdentifier
          CHECK(ghiEncryptorParseASN1Sequence(data, &seqLen));
          if (seqLen != 0x0d)
            return GHIEncryptionResult_Error;
          if ((unsigned char)data->data[data->pos++] != 0x06)
            return GHIEncryptionResult_Error;
          if ((unsigned char)data->data[data->pos++] != 0x09)
            return GHIEncryptionResult_Error;
          if (0 !=
              memcmp(&data->data[data->pos], gsSslRsaOid, sizeof(gsSslRsaOid)))
            return GHIEncryptionResult_Error; // only RSA certs are supported
          data->pos += sizeof(gsSslRsaOid);
          if ((unsigned char)data->data[data->pos++] != 0x05)
            return GHIEncryptionResult_Error;
          if ((unsigned char)data->data[data->pos++] != 0x00)
            return GHIEncryptionResult_Error;

          //     Bitstring (subject public key)
          if (2 > (certListEndPos - data->pos))
            return GHIEncryptionResult_Error;
          if ((unsigned char)data->data[data->pos++] != 0x03)
            return GHIEncryptionResult_Error; // bitstring
          if ((unsigned char)data->data[data->pos++] != 0x81)
            return GHIEncryptionResult_Error; // 1 byte len field
          if (temp > (certListEndPos - data->pos))
            return GHIEncryptionResult_Error;
          temp = (unsigned char)data->data[data->pos++]; // remaining data
                                                         // size (check or
                                                         // ignore)

          if ((unsigned char)data->data[data->pos++] != 0x00)
            return GHIEncryptionResult_Error;

          //     Start of the public key modulus
          CHECK(ghiEncryptorParseASN1Sequence(data, &seqLen));

          // Read out the public key modulus
          if (data->data[data->pos++] != 0x02)
            return GHIEncryptionResult_Error; // integer tag
          if ((data->data[data->pos] & 0x80) == 0x80) // ASN1 variable length
                                                      // field
          {
            int lensize = data->data[data->pos++] & 0x7f;
            if (lensize > 4)
              return GHIEncryptionResult_Error;
            temp = 0;
            while (lensize-- > 0)
              temp = (temp << 8) | (unsigned char)data->data[data->pos++];
          } else {
            temp = (unsigned char)data->data[data->pos++];
          }
          if (data->pos + temp > certListEndPos)
            return GHIEncryptionResult_Error;
          if (data->data[data->pos++] != 0x00)
            return GHIEncryptionResult_Error; // ignore bits must be 0x00
          if (temp - 1 > GS_LARGEINT_BINARY_SIZE / sizeof(char))
            return GHIEncryptionResult_Error;
          sslInterface->serverpub.modulus.mLength =
              (unsigned int)((temp - 1) / (GS_LARGEINT_BYTE_SIZE / 8));
          memcpy(((char*)sslInterface->serverpub.modulus.mData) +
                     (4 - (temp - 1) % 4) % 4,
                 &data->data[data->pos], (size_t)(temp - 1));
          gsLargeIntReverseBytes(&sslInterface->serverpub.modulus);
          data->pos += temp - 1;

          // Read out the public key exponent
          if (data->data[data->pos++] != 0x02)
            return GHIEncryptionResult_Error; // integer
          if ((data->data[data->pos] & 0x80) == 0x80) {
            int lensize = data->data[data->pos++] & 0x7f;
            if (lensize > 4)
              return GHIEncryptionResult_Error;
            temp = 0;
            while (lensize-- > 0)
              temp = (temp << 8) | (unsigned char)data->data[data->pos++];
          } else {
            temp = (unsigned char)data->data[data->pos++];
          }
          if (data->pos + temp > certListEndPos)
            return GHIEncryptionResult_Error;
          if (temp == 0)
            return GHIEncryptionResult_Error; // no exponent?
          if (temp > GS_LARGEINT_BINARY_SIZE / sizeof(char))
            return GHIEncryptionResult_Error;
          sslInterface->serverpub.exponent.mLength =
              (unsigned int)(((temp - 1) / (GS_LARGEINT_BYTE_SIZE / 8)) + 1);
          memcpy(((char*)sslInterface->serverpub.exponent.mData) +
                     (4 - temp % 4) % 4,
                 &data->data[data->pos], (size_t)temp);
          gsLargeIntReverseBytes(&sslInterface->serverpub.exponent);
          data->pos += temp;
        }

        // update the position
        data->pos = certStartPos + certLength;

        GSI_UNUSED(version);
      }
      if (data->pos != certListEndPos)
        return GHIEncryptionResult_Error; // bytes hanging off the end!

      // todo: verify certificate chain
      //       first certificate is the server's, the rest likely belong to CA
      if (GHTTPFalse == ghiCertificateChainIsValid(sslInterface))
        return GHIEncryptionResult_Error;

      // add it to the running handshake hash
      totalMsgLen = data->pos - messageStart;
      MD5Update(&sslInterface->finishHashMD5,
                (unsigned char*)&data->data[messageStart],
                (unsigned int)totalMsgLen);
      SHA1Input(&sslInterface->finishHashSHA1,
                (unsigned char*)&data->data[messageStart],
                (unsigned int)totalMsgLen);
    } else if (messageType == GS_SSL_HANDSHAKE_SERVERHELLODONE) {
      // Process the hello done
      // Respond with 3 messages
      //    ClientKeyExchange
      //    ChangeCipherSpec
      //    Finished (final handshake)
      int i = 0;

      gsSSLClientKeyExchangeMsg* clientKeyExchange = NULL;
      gsSSLRecordHeaderMsg* changeCipherSpec = NULL;
      gsSSLRecordHeaderMsg* finalHandshake = NULL;

      unsigned char temp[7];
      unsigned char hashTempMD5[GS_CRYPT_MD5_HASHSIZE];
      unsigned char hashTempSHA1[GS_CRYPT_SHA1_HASHSIZE];
      int tempInt = 0;

      // ServerHelloDone has a zero length data field
      CHECK(ghiEncryptorReadNBOLength(data, &tempInt, 3));
      if (tempInt != 0x00)
        return GHIEncryptionResult_Error;

      // add it to the running handshake hash
      MD5Update(&sslInterface->finishHashMD5,
                (unsigned char*)&data->data[messageStart],
                (unsigned int)(data->pos - messageStart));
      SHA1Input(&sslInterface->finishHashSHA1,
                (unsigned char*)&data->data[messageStart],
                (unsigned int)(data->pos - messageStart));

      // Make sure there is room in the send buffer for the response messages
      tempInt = (int)(sizeof(gsSSLClientKeyExchangeMsg) +
                      sslInterface->serverpub.modulus.mLength *
                          GS_LARGEINT_DIGIT_SIZE_BYTES);
      while (connection->sendBuffer.size - connection->sendBuffer.len <
             tempInt) {
        // not enough room in send buffer, try to grow it
        if (GHTTPFalse == ghiResizeBuffer(&connection->sendBuffer,
                                          connection->sendBuffer.sizeIncrement))
          return GHIEncryptionResult_Error;
      }

      // 1) Client key exchange,
      //    create the pre-master-secret
      sslInterface->premastersecret[0] = GS_SSL_VERSION_MAJOR;
      sslInterface->premastersecret[1] = GS_SSL_VERSION_MINOR;
      for (i = 2; i < GS_SSL_MASTERSECRET_LEN; i++) {
        Util_RandSeed(current_time());
        sslInterface->premastersecret[i] =
            (unsigned char)(Util_RandInt(0, 0x0100)); // range = [0...FF]
      }

      clientKeyExchange =
          (gsSSLClientKeyExchangeMsg*)&connection->sendBuffer
              .data[connection->sendBuffer.len];
      connection->sendBuffer.len += sizeof(gsSSLClientKeyExchangeMsg);
      clientKeyExchange->header.contentType = GS_SSL_CONTENT_HANDSHAKE;
      clientKeyExchange->header.versionMajor = GS_SSL_VERSION_MAJOR;
      clientKeyExchange->header.versionMinor = GS_SSL_VERSION_MINOR;
      ghiEncryptorWriteNBOLength(clientKeyExchange->header.lengthNBO,
                                 (int)(sslInterface->serverpub.modulus.mLength *
                                           GS_LARGEINT_DIGIT_SIZE_BYTES +
                                       4),
                                 2);
      clientKeyExchange->handshakeType = GS_SSL_HANDSHAKE_CLIENTKEYEXCHANGE;
      ghiEncryptorWriteNBOLength(clientKeyExchange->lengthNBO,
                                 (int)(sslInterface->serverpub.modulus.mLength *
                                       GS_LARGEINT_DIGIT_SIZE_BYTES),
                                 3);
      //    encrypt the preMasterSecret using the server's public key (store
      //    result in sendbuffer)
      gsCryptRSAEncryptBuffer(
          &sslInterface->serverpub, sslInterface->premastersecret,
          GS_SSL_MASTERSECRET_LEN,
          (unsigned char*)&connection->sendBuffer
              .data[connection->sendBuffer.len]);
      connection->sendBuffer.len +=
          sslInterface->serverpub.modulus.mLength * GS_LARGEINT_DIGIT_SIZE_BYTES;

      // add it to the running handshake hash
      MD5Update(&sslInterface->finishHashMD5,
                (unsigned char*)clientKeyExchange +
                    sizeof(gsSSLRecordHeaderMsg),
                sizeof(gsSSLClientKeyExchangeMsg) -
                    sizeof(gsSSLRecordHeaderMsg) +
                    sslInterface->serverpub.modulus.mLength *
                        GS_LARGEINT_DIGIT_SIZE_BYTES);
      SHA1Input(&sslInterface->finishHashSHA1,
                (unsigned char*)clientKeyExchange +
                    sizeof(gsSSLRecordHeaderMsg),
                sizeof(gsSSLClientKeyExchangeMsg) -
                    sizeof(gsSSLRecordHeaderMsg) +
                    sslInterface->serverpub.modulus.mLength *
                        GS_LARGEINT_DIGIT_SIZE_BYTES);

      // 2) change cipher spec
      changeCipherSpec =
          (gsSSLRecordHeaderMsg*)&connection->sendBuffer
              .data[connection->sendBuffer.len];
      changeCipherSpec->contentType = GS_SSL_CONTENT_CHANGECIPHERSPEC;
      changeCipherSpec->versionMajor = GS_SSL_VERSION_MAJOR;
      changeCipherSpec->versionMinor = GS_SSL_VERSION_MINOR;
      changeCipherSpec->lengthNBO[0] = 0;
      changeCipherSpec->lengthNBO[1] = 1; // always one byte length
      connection->sendBuffer.len += sizeof(gsSSLRecordHeaderMsg);
      connection->sendBuffer.data[connection->sendBuffer.len++] =
          0x01; // always set to 0x01
      // DO NOT add it to the running handshake hash (its content is not
      // GS_SSL_CONTENT_HANDSHAKE)

      // Calculate the encryption keys
      ghiEncryptorGenerateEncryptionKeys(sslInterface);

      // 3) final handshake message (encrypted)
      finalHandshake =
          (gsSSLRecordHeaderMsg*)&connection->sendBuffer
              .data[connection->sendBuffer.len];
      finalHandshake->contentType = GS_SSL_CONTENT_HANDSHAKE;
      finalHandshake->versionMajor = GS_SSL_VERSION_MAJOR;
      finalHandshake->versionMinor = GS_SSL_VERSION_MINOR;
      finalHandshake->lengthNBO[0] = 0;
      finalHandshake->lengthNBO[1] =
          56; // handshake type(1)+handshake lenNBO(3)+SHA1(20)+MD5(16)+MAC(16)
      connection->sendBuffer.len += sizeof(gsSSLRecordHeaderMsg);
      connection->sendBuffer.data[connection->sendBuffer.len++] =
          GS_SSL_HANDSHAKE_FINISHED;
      ghiEncryptorWriteNBOLength(
          (unsigned char*)&connection->sendBuffer
              .data[connection->sendBuffer.len],
          36, 3);
      connection->sendBuffer.len += 3;

      // MD5(master_secret + pad2 +
      // MD5(handshake_messages+"CLNT"+master_secret+pad1))
      // SHA1(master_secret + pad2 +
      // SHA1(handshake_messages+"CLNT"+master_secret+pad1))
      // prepare the final hashes (inner hashes)
      MD5Update(&sslInterface->finishHashMD5,
                (unsigned char*)GS_SSL_CLIENT_FINISH_VALUE, 4);
      MD5Update(&sslInterface->finishHashMD5,
                (unsigned char*)sslInterface->mastersecret,
                GS_SSL_MASTERSECRET_LEN);
      MD5Update(&sslInterface->finishHashMD5, (unsigned char*)GS_SSL_PAD_ONE,
                GS_SSL_MD5_PAD_LEN);
      MD5Final(hashTempMD5, &sslInterface->finishHashMD5);

      SHA1Input(&sslInterface->finishHashSHA1,
                (unsigned char*)GS_SSL_CLIENT_FINISH_VALUE, 4);
      SHA1Input(&sslInterface->finishHashSHA1,
                (unsigned char*)sslInterface->mastersecret,
                GS_SSL_MASTERSECRET_LEN);
      SHA1Input(&sslInterface->finishHashSHA1, (unsigned char*)GS_SSL_PAD_ONE,
                GS_SSL_SHA1_PAD_LEN);
      SHA1Result(&sslInterface->finishHashSHA1, hashTempSHA1);

      // prepare the final hashes (outer hashes)
      MD5Init(&sslInterface->finishHashMD5);
      MD5Update(&sslInterface->finishHashMD5,
                (unsigned char*)sslInterface->mastersecret,
                GS_SSL_MASTERSECRET_LEN);
      MD5Update(&sslInterface->finishHashMD5, (unsigned char*)GS_SSL_PAD_TWO,
                GS_SSL_MD5_PAD_LEN);
      MD5Update(&sslInterface->finishHashMD5, hashTempMD5,
                GS_CRYPT_MD5_HASHSIZE);
      MD5Final(hashTempMD5, &sslInterface->finishHashMD5);

      SHA1Reset(&sslInterface->finishHashSHA1);
      SHA1Input(&sslInterface->finishHashSHA1,
                (unsigned char*)sslInterface->mastersecret,
                GS_SSL_MASTERSECRET_LEN);
      SHA1Input(&sslInterface->finishHashSHA1, (unsigned char*)GS_SSL_PAD_TWO,
                GS_SSL_SHA1_PAD_LEN);
      SHA1Input(&sslInterface->finishHashSHA1, hashTempSHA1,
                GS_CRYPT_SHA1_HASHSIZE);
      SHA1Result(&sslInterface->finishHashSHA1, hashTempSHA1);

      // copy results into the sendbuffer
      memcpy(&connection->sendBuffer.data[connection->sendBuffer.len],
             hashTempMD5, GS_CRYPT_MD5_HASHSIZE);
      connection->sendBuffer.len += GS_CRYPT_MD5_HASHSIZE;
      memcpy(&connection->sendBuffer.data[connection->sendBuffer.len],
             hashTempSHA1, GS_CRYPT_SHA1_HASHSIZE);
      connection->sendBuffer.len += GS_CRYPT_SHA1_HASHSIZE;

      // output the message MAC
      // (hash(MAC_write_secret+pad_2+
      // hash(MAC_write_secret+pad_1+seq_num+length+content)));
      // Re-using the finishHashMD5 since it has already been allocated
      MD5Init(&sslInterface->finishHashMD5);
      MD5Update(&sslInterface->finishHashMD5,
                (unsigned char*)sslInterface->clientWriteMACSecret,
                GS_CRYPT_MD5_HASHSIZE);
      MD5Update(&sslInterface->finishHashMD5, (unsigned char*)GS_SSL_PAD_ONE,
                GS_SSL_MD5_PAD_LEN);
      MD5Update(&sslInterface->finishHashMD5,
                (unsigned char*)sslInterface->sendSeqNBO, 8);
      temp[0] = 0x16;
      temp[1] =
          (unsigned char)((GS_CRYPT_MD5_HASHSIZE + GS_CRYPT_SHA1_HASHSIZE + 4) >>
                          8);
      temp[2] =
          (unsigned char)((GS_CRYPT_MD5_HASHSIZE + GS_CRYPT_SHA1_HASHSIZE + 4));
      temp[3] = 0x14; // 20-bytes of data (MD5+SHA1)
      temp[4] = 0x00; // 3-byte length NBO
      temp[5] = 0x00; // ..
      temp[6] = 0x24; // ..
      MD5Update(&sslInterface->finishHashMD5, (unsigned char*)&temp, 7);
      MD5Update(&sslInterface->finishHashMD5, hashTempMD5,
                GS_CRYPT_MD5_HASHSIZE); // content part 1
      MD5Update(&sslInterface->finishHashMD5, hashTempSHA1,
                GS_CRYPT_SHA1_HASHSIZE); // content part 2
      MD5Final(hashTempMD5, &sslInterface->finishHashMD5);
      MD5Init(&sslInterface->finishHashMD5); // reset for outer hash
      MD5Update(&sslInterface->finishHashMD5,
                (unsigned char*)sslInterface->clientWriteMACSecret,
                GS_CRYPT_MD5_HASHSIZE);
      MD5Update(&sslInterface->finishHashMD5, (unsigned char*)GS_SSL_PAD_TWO,
                GS_SSL_MD5_PAD_LEN);
      MD5Update(&sslInterface->finishHashMD5, hashTempMD5,
                GS_CRYPT_MD5_HASHSIZE);
      MD5Final(hashTempMD5, &sslInterface->finishHashMD5);

      memcpy(&connection->sendBuffer.data[connection->sendBuffer.len],
             hashTempMD5, GS_CRYPT_MD5_HASHSIZE);
      connection->sendBuffer.len += GS_CRYPT_MD5_HASHSIZE;

      // increment sequence each time we send a message
      //   ...assume NBO is bigendian for simplicity
      memset(sslInterface->sendSeqNBO, 0, sizeof(sslInterface->sendSeqNBO));
      ghiEncryptorWriteNBOLength(&sslInterface->sendSeqNBO[4], 1, 4);

      // now encrypt the message (not including record header)
      RC4Encrypt(&sslInterface->sendRC4,
                 ((unsigned char*)finalHandshake) +
                     sizeof(gsSSLRecordHeaderMsg),
                 ((unsigned char*)finalHandshake) +
                     sizeof(gsSSLRecordHeaderMsg),
                 56);
    } else if (messageType == GS_SSL_HANDSHAKE_FINISHED) {
      // process server finished and verify hashes
      data->pos = data->len;
    } else {
      return GHIEncryptionResult_Error; // abort connection
    }
  }

  GSI_UNUSED(connection);

  if (data->pos == data->len)
    return GHIEncryptionResult_Success;
  else
    return GHIEncryptionResult_Error; // too many or too few bytes, protocol
                                      // error!
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
