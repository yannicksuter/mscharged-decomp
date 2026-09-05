///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
#include "gsPlatformUtil.h"
#include "gsCommon.h"

// Include platform separated functions
#include "revolution/gsUtilRevolution.c"

#if defined(__cplusplus)
extern "C" {
#endif

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// ********** ASYNC DNS ********** //

// struct is used in both threaded and non-threaded versions
typedef struct GSIResolveHostnameInfo {
  char* hostname;
  unsigned int ip;
  int finishedResolving;
  GSIThreadID threadID;
} GSIResolveHostnameInfo;

///////////////////////////////////////////////////////////////////////////////
static void* gsiResolveHostnameThread(void* arg) {
  static GSICriticalSection aHostnameCrit;
  static int aInitialized = 0;
  // SOAddrInfo *aHostAddr;
  HOSTENT* aHostAddr;
  // int retval;
  GSIResolveHostnameHandle handle = (GSIResolveHostnameHandle)arg;

  if (!aInitialized) {
    gsiInitializeCriticalSection(&aHostnameCrit);
    aInitialized = 1;
  }
  gsiEnterCriticalSection(&aHostnameCrit);

  // retval = getaddrinfo(handle->hostname, NULL, NULL, &aHostAddr);
  aHostAddr = gethostbyname(handle->hostname);
  if (aHostAddr != 0) {
    char* ip;
    // first convert to character string for debug output
    ip = inet_ntoa(*(in_addr*)aHostAddr->addrList[0]);

    // gsDebugFormat(GSIDebugCat_HTTP, GSIDebugType_State,
    // GSIDebugLevel_Comment,
    //              "Resolved host '%s' to ip '%s'\n", handle->hostname, ip);

    handle->ip = inet_addr(ip);
    // freeaddrinfo(aHostAddr);
  } else {
    // couldnt reach host - debug output is printed later
    handle->ip = GSI_ERROR_RESOLVING_HOSTNAME;
  }

  // finished resolving
  handle->finishedResolving = 1;

  OSUnlockMutex(&aHostnameCrit);
}
////////////////////////////////////////////////////////////////////////////////

int gsiStartResolvingHostname(const char* hostname,
                              GSIResolveHostnameHandle* handle) {
  GSIResolveHostnameInfo* info;

  // allocate a handle
  info = (GSIResolveHostnameInfo*)gsimalloc(sizeof(GSIResolveHostnameInfo));
  if (!info)
    return -1;

  // make a copy of the hostname so the thread has access to it
  info->hostname = goastrdup(hostname);
  if (!info->hostname) {
    gsifree(info);
    return -1;
  }

  // not resolved yet
  info->finishedResolving = 0;

  // start the thread
  if (gsiStartThread(gsiResolveHostnameThread, (0x1000), info,
                     &info->threadID) == -1) {
    gsifree(info->hostname);
    info->hostname = NULL;
    gsifree(info);
    info = NULL;
    return -1;
  }

  // set the handle to the info
  *handle = info;

  return 0;
}

void gsiCancelResolvingHostname(GSIResolveHostnameHandle handle) {
  // cancel the thread
  gsiCancelThread(handle->threadID);

  gsifree(handle->hostname);
  handle->hostname = NULL;
  gsifree(handle);
  handle = NULL;
}

// PAL: 0x800f2300
unsigned int gsiGetResolvedIP(GSIResolveHostnameHandle handle) {
  unsigned int ip;

  // check if we haven't finished
  if (!handle->finishedResolving)
    return GSI_STILL_RESOLVING_HOSTNAME;

  // save the ip
  ip = handle->ip;

  // free resources
  gsiCleanupThread(handle->threadID);
  gsifree(handle->hostname);
  gsifree(handle);
  handle = NULL;

  return ip;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
char* goastrdup(const char* src) {
  char* res;
  if (src == NULL) // PANTS|02.11.00|check for NULL before strlen
    return NULL;
  res = (char*)gsimalloc(strlen(src) + 1);
  if (res != NULL) // PANTS|02.02.00|check for NULL before strcpy
    strcpy(res, src);
  return res;
}

/*
unsigned short* goawstrdup(const unsigned short* src) {
  unsigned short* res;
  if (src == NULL)
    return NULL;
  res = (unsigned short*)gsimalloc((wcslen((wchar_t*)src) + 1) *
                                   sizeof(unsigned short));
  if (res != NULL)
    wcscpy((wchar_t*)res, (const wchar_t*)src);
  return res;
}
*/

char* _strlwr(char* string) {
  char* hold = string;
  while (*string) {
    *string = (char)tolower(*string);
    string++;
  }

  return hold;
}

/*
char* _strupr(char* string) {
  char* hold = string;
  while (*string) {
    *string = (char)toupper(*string);
    string++;
  }

  return hold;
}
*/

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// PAL: 0x800f24c0
void SocketStartUp() {}
// PAL: 0x800f24c4
void SocketShutDown() {}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// PAL: 0x800f24c8
gsi_time current_time() // returns current time in milliseconds
{
  OSTime aTimeNow = OSGetTime();
  gsi_time aMilliseconds = (gsi_time)OSTicksToMilliseconds(aTimeNow);
  return aMilliseconds;
}

gsi_time current_time_hires() // returns current time in microseconds
{
#ifdef _WIN32
#if (!defined(_M_IX86) ||                                                     \
     (defined(_INTEGRAL_MAX_BITS) && _INTEGRAL_MAX_BITS >= 64))
  static LARGE_INTEGER counterFrequency;
  static BOOL haveCounterFrequency = FALSE;
  static BOOL haveCounter = FALSE;
  LARGE_INTEGER count;

  if (!haveCounterFrequency) {
    haveCounter = QueryPerformanceFrequency(&counterFrequency);
    haveCounterFrequency = TRUE;
  }

  if (haveCounter) {
    if (QueryPerformanceCounter(&count)) {
      return (gsi_time)(count.QuadPart * 1000000 / counterFrequency.QuadPart);
    }
  }
#endif

  return (current_time() / 1000);
#endif

#ifdef _PS2
  unsigned int ticks;
  static unsigned int msec = 0;
  static unsigned int lastticks = 0;
  sceCdCLOCK lasttimecalled; /* defined in libcdvd.h */

  if (!msec) {
    sceCdReadClock(&lasttimecalled); /* libcdvd.a */
    msec = (unsigned int)(DEC(lasttimecalled.day) * 86400000) +
           (unsigned int)(DEC(lasttimecalled.hour) * 3600000) +
           (unsigned int)(DEC(lasttimecalled.minute) * 60000) +
           (unsigned int)(DEC(lasttimecalled.second) * 1000);
    msec *= 1000;
  }

  ticks = (unsigned int)GetTicks();
  if (lastticks > ticks)
    msec += ((sizeof(unsigned int) - lastticks) + ticks) / 300;
  else
    msec += (unsigned int)(ticks - lastticks) / 300;
  lastticks = ticks;

  return msec;
#endif

#ifdef _PSP
  struct SceRtcTick ticks;
  int result = 0;

  result = sceRtcGetCurrentTick(&ticks);
  if (result < 0) {
    ScePspDateTime time;
    result = sceRtcGetCurrentClock(&time, 0);
    if (result < 0)
      return 0; // um...error handling? //Nope, should return zero since time
                // cannot be zero
    result = sceRtcGetTick(&time, &ticks);
    if (result < 0)
      return 0; // Nope, should return zero since time cannot be zero
  }

  return (gsi_time)(ticks.tick);
#endif

#ifdef _UNIX
  struct timeval time;

  gettimeofday(&time, NULL);
  return (time.tv_sec * 1000000 + time.tv_usec);
#endif

#ifdef _NITRO
  assert(OS_IsTickAvailable() == TRUE);
  return (gsi_time)OS_TicksToMicroSeconds(OS_GetTick());
#endif

#ifdef _PS3
  return (gsi_time)sys_time_get_system_time();
#endif
}

// PAL: 0x800f2510
void msleep(gsi_time msec) {
  OSSleepTicks((((s64)msec) * ((OS_BUS_CLOCK / 4) / 1000)));
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Cross platform random number generator
#define RANa 16807               // multiplier
#define LONGRAND_MAX 2147483647L // 2**31 - 1

static long randomnum = 1;

static long nextlongrand(long seed) {
  unsigned

      long lo,
      hi;
  lo = RANa * (unsigned long)(seed & 0xFFFF);
  hi = RANa * ((unsigned long)seed >> 16);
  lo += (hi & 0x7FFF) << 16;

  if (lo > LONGRAND_MAX) {
    lo &= LONGRAND_MAX;
    ++lo;
  }
  lo += hi >> 15;

  if (lo > LONGRAND_MAX) {
    lo &= LONGRAND_MAX;
    ++lo;
  }

  return (long)lo;
}

// return next random long
static long longrand(void) {
  randomnum = nextlongrand(randomnum);
  return randomnum;
}

// to seed it
// PAL: 0x800f2ec8
void Util_RandSeed(unsigned long seed) {
  // nonzero seed
  randomnum = seed ? (long)(seed & LONGRAND_MAX) : 1;
}

// PAL: 0x800f2ee0
int Util_RandInt(int low, int high) {
  unsigned int range = (unsigned int)high - low;
  int num;

  if (range == 0)
    return (low); // Prevent divide by zero

  num = (int)(longrand() % range);

  return (num + low);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

static void QuartToTrip(char* quart, char* trip, int inlen) {
  if (inlen >= 2)
    trip[0] = (char)(quart[0] << 2 | quart[1] >> 4);
  if (inlen >= 3)
    trip[1] = (char)((quart[1] & 0x0F) << 4 | quart[2] >> 2);
  if (inlen >= 4)
    trip[2] = (char)((quart[2] & 0x3) << 6 | quart[3]);
}

static void TripToQuart(const char* trip, char* quart, int inlen) {
  unsigned char triptemp[3];
  int i;
  for (i = 0; i < inlen; i++) {
    triptemp[i] = (unsigned char)trip[i];
  }
  while (i < 3) // fill the rest with 0
  {
    triptemp[i] = 0;
    i++;
  }
  quart[0] = (char)(triptemp[0] >> 2);
  quart[1] = (char)(((triptemp[0] & 3) << 4) | (triptemp[1] >> 4));
  quart[2] = (char)((triptemp[1] & 0x0F) << 2 | (triptemp[2] >> 6));
  quart[3] = (char)(triptemp[2] & 0x3F);
}

const char defaultEncoding[] = {'+', '/', '='};
const char alternateEncoding[] = {'[', ']', '_'};
const char urlSafeEncodeing[] = {'-', '_', '='};

void B64Decode(const char* input, char* output, int inlen, int* outlen,
               int encodingType) {
  const char* encoding = NULL;
  const char* holdin = input;
  int readpos = 0;
  int writepos = 0;
  char block[4];

  // int outlen = -1;
  // int inlen = (int)strlen(input);

  // 10-31-2004 : Added by Saad Nader
  // now supports URL safe encoding
  ////////////////////////////////////////////////
  switch (encodingType) {
  case 1:
    encoding = alternateEncoding;
    break;
  case 2:
    encoding = urlSafeEncodeing;
    break;
  default:
    encoding = defaultEncoding;
  }

  GS_ASSERT(inlen >= 0);
  if (inlen <= 0) {
    if (outlen)
      *outlen = 0;
    output[0] = '\0';
    return;
  }

  // Break at end of string or padding character
  while (readpos < inlen && input[readpos] != encoding[2]) {
    //    'A'-'Z' maps to 0-25
    //    'a'-'z' maps to 26-51
    //    '0'-'9' maps to 52-61
    //    62 maps to encoding[0]
    //    63 maps to encoding[1]
    if (input[readpos] >= '0' && input[readpos] <= '9')
      block[readpos % 4] = (char)(input[readpos] - 48 + 52);
    else if (input[readpos] >= 'a' && input[readpos] <= 'z')
      block[readpos % 4] = (char)(input[readpos] - 71);
    else if (input[readpos] >= 'A' && input[readpos] <= 'Z')
      block[readpos % 4] = (char)(input[readpos] - 65);
    else if (input[readpos] == encoding[0])
      block[readpos % 4] = 62;
    else if (input[readpos] == encoding[1])
      block[readpos % 4] = 63;

    // padding or '\0' characters also mark end of input
    else if (input[readpos] == encoding[2])
      break;
    else if (input[readpos] == '\0')
      break;
    else {
      //	(assert(0)); //bad input data
      if (outlen)
        *outlen = 0;
      output[0] = '\0';
      return; // invaid data
    }

    // every 4 bytes, convert QuartToTrip into destination
    if (readpos % 4 == 3) // zero based, so (3%4) means four bytes, 0-1-2-3
    {
      QuartToTrip(block, &output[writepos], 4);
      writepos += 3;
    }
    readpos++;
  }

  // Convert any leftover characters in block
  if ((readpos != 0) && (readpos % 4 != 0)) {
    // fill block with pad (required for QuartToTrip)
    memset(&block[readpos % 4], encoding[2], (unsigned int)4 - (readpos % 4));
    QuartToTrip(block, &output[writepos], readpos % 4);

    // output bytes depend on the number of non-pad input bytes
    if (readpos % 4 == 3)
      writepos += 2;
    else
      writepos += 1;
  }

  if (outlen)
    *outlen = writepos;

  GSI_UNUSED(holdin);
}

void B64Encode(const char* input, char* output, int inlen, int encodingType) {
  const char* encoding;
  char* holdout = output;
  char* lastchar;
  int todo = inlen;

  // 10-31-2004 : Added by Saad Nader
  // now supports URL safe encoding
  ////////////////////////////////////////////////
  switch (encodingType) {
  case 1:
    encoding = alternateEncoding;
    break;
  case 2:
    encoding = urlSafeEncodeing;
    break;
  default:
    encoding = defaultEncoding;
  }

  // assume interval of 3
  while (todo > 0) {
    TripToQuart(input, output, min(todo, 3));
    output += 4;
    input += 3;
    todo -= 3;
  }
  lastchar = output;
  if (inlen % 3 == 1)
    lastchar -= 2;
  else if (inlen % 3 == 2)
    lastchar -= 1;
  *output = 0; // null terminate!
  while (output > holdout) {
    output--;
    if (output >= lastchar) // pad the end
      *output = encoding[2];
    else if (*output <= 25)
      *output = (char)(*output + 65);
    else if (*output <= 51)
      *output = (char)(*output + 71);
    else if (*output <= 61)
      *output = (char)(*output + 48 - 52);
    else if (*output == 62)
      *output = encoding[0];
    else if (*output == 63)
      *output = encoding[1];
  }
}

// PAL: 0x800f3484
int B64DecodeLen(const char* input, int encodingType) {
  const char* encoding;
  const char* holdin = input;

  switch (encodingType) {
  case 1:
    encoding = alternateEncoding;
    break;
  case 2:
    encoding = urlSafeEncodeing;
    break;
  default:
    encoding = defaultEncoding;
  }

  while (*input) {
    if (*input == encoding[2])
      return (input - holdin) / 4 * 3 + (input - holdin - 1) % 4;
    input++;
  }

  return (input - holdin) / 4 * 3;
}

// PAL: 0x800f3528
void B64InitEncodeStream(B64StreamData* data, const char* input, int len,
                         int encodingType) {
  data->input = input;
  data->len = len;
  data->encodingType = encodingType;
}

gsi_bool B64EncodeStream(B64StreamData* data, char output[4]) {
  const char* encoding;
  char* c;
  int i;

  if (data->len <= 0)
    return gsi_false;

  // 10-31-2004 : Added by Saad Nader
  // now supports URL safe encoding
  ////////////////////////////////////////////////
  switch (data->encodingType) {
  case 1:
    encoding = alternateEncoding;
    break;
  case 2:
    encoding = urlSafeEncodeing;
    break;
  default:
    encoding = defaultEncoding;
  }

  TripToQuart(data->input, output, min(data->len, 3));
  data->input += 3;
  data->len -= 3;

  for (i = 0; i < 4; i++) {
    c = &output[i];
    if (*c <= 25)
      *c = (char)(*c + 65);
    else if (*c <= 51)
      *c = (char)(*c + 71);
    else if (*c <= 61)
      *c = (char)(*c + 48 - 52);
    else if (*c == 62)
      *c = encoding[0];
    else if (*c == 63)
      *c = encoding[1];
  }

  if (data->len < 0) {
    output[3] = encoding[2];
    if (data->len == -2)
      output[2] = encoding[2];
  }

  return gsi_true;
}

/*
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
void gsiPadRight(char* cArray, char padChar, int cLength);
char* gsiXxteaAlg(const char* sIn, int nIn, char key[XXTEA_KEY_SIZE], int bEnc,
                  int* nOut);

void gsiPadRight(char* cArray, char padChar, int cLength) {
  int diff;
  int length = (int)strlen(cArray);

  diff = cLength - length;
  memset(&cArray[length], padChar, (size_t)diff);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// The heart of the XXTEA encryption/decryption algorithm.
//
// sIn:  Input stream.
// nIn:  Input length (bytes).
// key:  Key (only first 128 bits are significant).
// bEnc: Encrypt (else decrypt)?
char* gsiXxteaAlg(const char* sIn, int nIn, char key[XXTEA_KEY_SIZE], int bEnc,
                  int* nOut) {
  int i, p, n1;
  unsigned int *k, *v, z, y;
  char *oStr = NULL, *pStr = NULL;
  char* sIn2 = NULL;
  /////////////////////////////////
  // ERROR CHECK!
  if (!sIn || !key[0] || nIn == 0)
    return NULL;

  // Convert stream length to a round number of 32-bit words
  // Convert byte	count to 32-bit	word count
  if (nIn % 4 == 0) // Fix for null terminated strings divisible by 4
    nIn = (nIn / 4) + 1;
  else
    nIn = (nIn + 3) / 4;

  if (nIn <= 1) // XXTEA requires at least 64 bits
    nIn = 2;

  // Load	and	zero-pad first 16 characters (128 bits)	of key
  gsiPadRight(key, '\0', XXTEA_KEY_SIZE);
  k = (unsigned int*)key;

  // Load and zero-pad entire input stream as 32-bit words
  sIn2 = (char*)gsimalloc((size_t)(4 * nIn));
  strcpy(sIn2, sIn);
  gsiPadRight(sIn2, '\0', 4 * nIn);
  v = (unsigned int*)sIn2;

  // Prepare to encrypt or decrypt
  n1 = nIn - 1;
  z = v[n1];
  y = v[0];
  i = (int)(6 + 52 / nIn);

  if (bEnc == 1) // Encrypt
  {
    unsigned int sum = 0;
    while (i-- != 0) {
      int e;
      sum += 0x9E3779B9;
      e = (int)(sum >> 2);
      for (p = -1; ++p < nIn;) {
        y = v[(p < n1) ? p + 1 : 0];
        z = (v[p] += (((z >> 5) ^ (y << 2)) + ((y >> 3) ^ (z << 4))) ^
                     ((sum ^ y) + (k[(p ^ e) & 3] ^ z)));
      }
    }
  } else if (bEnc == 0) // Decrypt
  {
    unsigned int sum = (unsigned int)i * 0x9E3779B9;
    while (sum != 0) {
      int e = (int)(sum >> 2);
      for (p = nIn; p-- != 0;) {
        z = v[(p != 0) ? p - 1 : n1];
        y = (v[p] -= (((z >> 5) ^ (y << 2)) + ((y >> 3) ^ (z << 4))) ^
                     ((sum ^ y) + (k[(p ^ e) & 3] ^ z)));
      }
      sum -= 0x9E3779B9;
    }
  } else
    return NULL;
  // Convert result from 32-bit words to a byte stream

  oStr = (char*)gsimalloc((size_t)(4 * nIn + 1));
  pStr = oStr;
  *nOut = 4 * nIn;
  for (i = -1; ++i < nIn;) {
    unsigned int q = v[i];

    *pStr++ = (char)(q & 0xFF);
    *pStr++ = (char)((q >> 8) & 0xFF);
    *pStr++ = (char)((q >> 16) & 0xFF);
    *pStr++ = (char)((q >> 24) & 0xFF);
  }
  *pStr = '\0';
  gsifree(sIn2);

  return oStr;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// XXTEA Encrpyt
// params
// iStr    : the input string to be encrypted
// iLength : the length of the input string
// key     : the key used to encrypt
char* gsXxteaEncrypt(const char* iStr, int iLength, char key[XXTEA_KEY_SIZE],
                     int* oLength) {
  return gsiXxteaAlg(iStr, iLength, key, 1, oLength);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// XXTEA Decrypt
// params
// iStr    : the input string to be decrypted
// iLength : the length of the input string
// key     : the key used to decrypt
char* gsXxteaDecrypt(const char* iStr, int iLength, char key[XXTEA_KEY_SIZE],
                     int* oLength) {
  return gsiXxteaAlg(iStr, iLength, key, 0, oLength);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
*/

#if (!defined(_PS2) && !defined(_PS3) && !defined(_XBOX) &&                 \
     !defined(_PSP)) ||                                                     \
    defined(UNIQUEID)
GetUniqueIDFunction GOAGetUniqueID = GOAGetUniqueID_Internal;
#endif

#if defined(__cplusplus)
}
#endif
