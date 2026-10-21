#include <dwc/dwc_ghttp.h>
#include <dwc/dwc_memfunc.h>
#include <dwc/dwc_encsession.h>
#include <dwc/dwc_report.h>
#include <dwc/dwci_memfunc.h>
#if defined(VERSION_R4QK01)
#include <dwc/dwc_error.h>
#include <dwc/dwci_error.h>
#endif
#include <gamespy/common/gsPlatformUtil.h>
#include <nitro/math/dgt.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <extras.h>

const char* DWCi_ENC_URL_BASE = NULL;
static int s_local_seed;

static struct
{
    DWCiRankingSessionState state;
#if defined(VERSION_R4QK01)
    BOOL initialized;
#endif
    int reqid;
    BOOL validated;
    char gamename[32];
    char secretkey[20];
    u32 key1;
    u32 key2;
    u32 key3;
    u32 key4;
    char* request;
    char* hash;
    char* data;
    void* resbuf;
    u32 resbuflen;
    void* decodedbuf;
    u32 decodedbuflen;
    BOOL (*callback)(u32* data, int size);
#if defined(VERSION_R4QK01)
    char cachedHash[41];
#endif
} g_session = { DWCi_ENC_SESSION_STATE_INITIAL };

#define DWCi_RankingByteSwap32(value) \
    ((((u32)(value) << 24) & 0xFF000000) | (((u32)(value) << 8) & 0x00FF0000) \
        | (((u32)(value) >> 8) & 0x0000FF00) | (((u32)(value) >> 24) & 0x000000FF))

#if defined(VERSION_R4QK01)
#define DWCi_ENC_HASH_LENGTH (sizeof(g_session.cachedHash) - 1)
#else
#define DWCi_ENC_HASH_LENGTH strlen("0000000000000000000000000000000000000000")
#endif

static void DWCi_EncSessionSetError(void)
{
#if defined(VERSION_R4QK01)
    if (!DWCi_IsError())
    {
        DWCi_SetError(DWC_ERROR_NETWORK, DWC_ECODE_SEQ_ETC + DWC_ECODE_TYPE_DATA);
    }
#endif
    g_session.state = DWCi_ENC_SESSION_STATE_ERROR;
}

static u32 b64size(u32 len)
{
    u32 i;

    i = (len % 3 != 0) ? 1 : 0;
    return ((len / 3) + i) * 4;
}

static u32 b64bufsize(u32 len)
{
    return b64size(len) + 1;
}

static void my_randinit(u32 seed)
{
    s_local_seed = seed | (seed << 16);
}

static u8 my_rand(void)
{
    s_local_seed = (g_session.key1 * s_local_seed + g_session.key2) % g_session.key3;
    return (s_local_seed >> 16) & 0xFF;
}

static BOOL DWCi_EncValidateKey(const char* gamename,
    const char* secretkey,
    u32 key1,
    u32 key2,
    u32 key3,
    u32 key4)
{
    int i;
    int bitcount;

    g_session.validated = FALSE;

    if (strlen(gamename) >= sizeof(g_session.gamename))
    {
        return FALSE;
    }

    if (strlen(secretkey) != sizeof(g_session.secretkey))
    {
        return FALSE;
    }

    if ((key1 - 5) % 8 != 0)
    {
        return FALSE;
    }

    if ((key2 & 1) == 0)
    {
        return FALSE;
    }

    bitcount = 0;
    for (i = 0; i < 32; i++)
    {
        if (((key3 >> i) & 1) == 1)
        {
            bitcount++;
        }
    }

    if (bitcount != 1)
    {
        return FALSE;
    }

#if defined(VERSION_R4QK01)
    strncpy(g_session.gamename, gamename, sizeof(g_session.gamename));
#else
    memcpy(g_session.gamename, gamename, sizeof(g_session.gamename));
#endif
    memcpy(g_session.secretkey, secretkey, sizeof(g_session.secretkey));
    g_session.key1 = key1;
    g_session.key2 = key2;
    g_session.key3 = key3;
    g_session.key4 = key4;
    g_session.validated = TRUE;

    return TRUE;
}

static BOOL DWCi_EncSessionValidateResponse(const char* buf, int len)
{
    int i;
    u8* tmpbuf;
    char hashhex[41];
    u8 hash[20];
    char hextbl[] = "0123456789abcdef";

    if (len <= 40)
    {
        DWC_Printf(DWC_REPORTFLAG_WARNING, "response too short\n");
        return FALSE;
    }

    tmpbuf = DWC_Alloc(DWC_ALLOCTYPE_ENC,
        b64bufsize(len - DWCi_ENC_HASH_LENGTH) + 40);
    if (tmpbuf == NULL)
    {
        DWC_Printf(DWC_REPORTFLAG_WARNING, "no enough memory\n");
        return FALSE;
    }

    memcpy(tmpbuf, g_session.secretkey, sizeof(g_session.secretkey));
    B64Encode(buf,
        (char*)tmpbuf + sizeof(g_session.secretkey),
        len - DWCi_ENC_HASH_LENGTH,
        2);
    memcpy(tmpbuf + sizeof(g_session.secretkey)
               + b64size(len - DWCi_ENC_HASH_LENGTH),
        g_session.secretkey,
        sizeof(g_session.secretkey));

    MATH_CalcSHA1(hash,
        tmpbuf,
        b64size(len - DWCi_ENC_HASH_LENGTH)
            + sizeof(g_session.secretkey) * 2);
    DWC_Free(DWC_ALLOCTYPE_ENC, tmpbuf, 0);

    for (i = 0; i < 20; i++)
    {
        hashhex[i * 2] = hextbl[hash[i] >> 4];
        hashhex[i * 2 + 1] = hextbl[hash[i] & 0xF];
    }
    hashhex[40] = '\0';

    if (strncmp(buf + (len - 40), hashhex, 40) != 0)
    {
        DWC_Printf(DWC_REPORTFLAG_WARNING, "invalid HMAC\n");
        return FALSE;
    }

    g_session.decodedbuf = (void*)buf;
#if defined(VERSION_R4QK01)
    g_session.decodedbuflen = len - DWCi_ENC_HASH_LENGTH;
#else
    g_session.decodedbuflen = len - 40;
#endif
    return TRUE;
}

static void DWCi_EncSessionProgressCallback(DWCGHTTPState state,
    const char* buf,
    int len,
    int bytesReceived,
    int totalSize,
    void* param)
{
}

static void DWCi_EncSessionCompleteCallback(const char* buf,
    int len,
    DWCGHTTPResult result,
    void* param)
{
    g_session.reqid = -1;

    if (g_session.state == DWCi_ENC_SESSION_STATE_ERROR)
    {
        return;
    }

    if (result == DWC_GHTTP_SUCCESS)
    {
        switch (g_session.state)
        {
        case DWCi_ENC_SESSION_STATE_GETTING_TOKEN:
            if (len == 32)
            {
                char hextbl[] = "0123456789abcdef";
                char tmpbuf[52];
                u8* hash;
                int i;

                hash = (u8*)g_session.hash + 20;
                memcpy(tmpbuf, g_session.secretkey, sizeof(g_session.secretkey));
                memcpy(tmpbuf + sizeof(g_session.secretkey), buf, 32);
                MATH_CalcSHA1(hash, tmpbuf, sizeof(tmpbuf));

                for (i = 0; i < 20; i++)
                {
                    g_session.hash[i * 2] = hextbl[hash[i] >> 4];
                    g_session.hash[i * 2 + 1] = hextbl[hash[i] & 0xF];
                }

                *(g_session.hash - strlen("&hash=")) = '&';
#if defined(VERSION_R4QK01)
                strncpy(g_session.cachedHash, g_session.hash, sizeof(g_session.cachedHash));
#endif
                g_session.state = DWCi_ENC_SESSION_STATE_GOT_TOKEN;
            }
            else
            {
                DWC_Printf(DWC_REPORTFLAG_WARNING, "invalid response length : %d\n", len);
                DWCi_EncSessionSetError();
            }

            DWCi_GsFree((void*)buf);
            break;

        case DWCi_ENC_SESSION_STATE_SENDING_DATA:
            if (strncmp(buf, "error:", strlen("error:")) == 0)
            {
                DWC_Printf(DWC_REPORTFLAG_WARNING, "invalid response : (%s)\n", buf);
                DWCi_GsFree((void*)buf);
                DWCi_EncSessionSetError();
                return;
            }

            if (g_session.resbuf != NULL)
            {
                DWCi_GsFree(g_session.resbuf);
                g_session.resbuf = NULL;
            }

            if (!DWCi_EncSessionValidateResponse(buf, len))
            {
                DWCi_GsFree((void*)buf);
                DWCi_EncSessionSetError();
                return;
            }

            if (g_session.callback != NULL && !g_session.callback((u32*)buf, len))
            {
                DWCi_GsFree((void*)buf);
                DWCi_EncSessionSetError();
                return;
            }

            g_session.resbuf = (void*)buf;
            g_session.resbuflen = len;
            g_session.state = DWCi_ENC_SESSION_STATE_COMPLETED;
            break;
        }
    }
    else
    {
        DWCi_EncSessionSetError();
    }
}

static DWCiRankingSessionResult DWCi_EncSessionEncrypt(u8* outbuf,
    const u8* header,
    u32 headerlen,
    const u8* srcbuf,
    u32 srclen)
{
    u8* tmpbuf;
    u32 sum = 0;
    int i;

    tmpbuf = DWC_Alloc(DWC_ALLOCTYPE_ENC, srclen + headerlen + 4);
    if (tmpbuf == NULL)
    {
        return DWCi_ENC_SESSION_ERROR_NOMEMORY;
    }

    for (i = 0; i < (int)headerlen; i++)
    {
        tmpbuf[i + 4] = header[i];
    }

    for (i = 0; i < (int)srclen; i++)
    {
        tmpbuf[i + headerlen + 4] = srcbuf[i];
    }

    for (i = 0; i < (int)(srclen + headerlen); i++)
    {
        sum += tmpbuf[i + 4];
    }

    my_randinit(sum);
    for (i = 0; i < (int)(srclen + headerlen); i++)
    {
        tmpbuf[i + 4] ^= my_rand();
    }

    sum ^= g_session.key4;
    tmpbuf[0] = sum >> 24;
    tmpbuf[1] = sum >> 16;
    tmpbuf[2] = sum >> 8;
    tmpbuf[3] = sum;

    B64Encode((const char*)tmpbuf, (char*)outbuf, srclen + headerlen + 4, 2);
    DWC_Free(DWC_ALLOCTYPE_ENC, tmpbuf, 0);
    outbuf[b64bufsize(srclen + headerlen + 4) - 1] = '\0';

    return DWCi_ENC_SESSION_SUCCESS;
}

void DWCi_EncSessionInitialize(int server, const char* initdata)
{
    char keybuf[9] = { 0 };
    char secretkey[21];
    u32 key1;
    u32 key2;
    u32 key3;
    u32 key4;

    g_session.reqid = -1;
    g_session.request = NULL;
    g_session.hash = NULL;
    g_session.data = NULL;
    g_session.resbuf = NULL;
    g_session.resbuflen = 0;

    switch (server)
    {
    case 0:
        DWCi_ENC_URL_BASE = "http://gamestats2.gs.nintendowifi.net/";
        break;
    case 1:
        DWCi_ENC_URL_BASE = "http://sdkdev.gamespy.com/games/";
        break;
    case 2:
        DWCi_ENC_URL_BASE = "http://ishikawa.servebeer.com/games/";
        break;
    }

    strncpy(secretkey, initdata, 20);
    secretkey[20] = '\0';

    key1 = strtoul(strncpy(keybuf, initdata + 20, 8), NULL, 16);
    key2 = strtoul(strncpy(keybuf, initdata + 28, 8), NULL, 16);
    key3 = strtoul(strncpy(keybuf, initdata + 36, 8), NULL, 16);
    key4 = strtoul(strncpy(keybuf, initdata + 44, 8), NULL, 16);

    DWCi_EncValidateKey(initdata + 52, secretkey, key1, key2, key3, key4);
    DWC_InitGHTTP(NULL);
    g_session.state = DWCi_ENC_SESSION_STATE_INITIALIZED;
#if defined(VERSION_R4QK01)
    g_session.initialized = TRUE;
#endif
}

DWCiRankingSessionState DWCi_EncSessionProcess(void)
{
    int result;

    switch (g_session.state)
    {
#if defined(VERSION_R4QK01)
    case DWCi_ENC_SESSION_STATE_ERROR:
        DWCi_EncSessionSetError();
        break;
#else
    case DWCi_ENC_SESSION_STATE_CANCELED:
        break;

    case DWCi_ENC_SESSION_STATE_INITIAL:
        break;

    case DWCi_ENC_SESSION_STATE_INITIALIZED:
        break;
#endif

    case DWCi_ENC_SESSION_STATE_REQUEST:
        result = DWC_GetGHTTPDataEx(g_session.request,
            0,
            FALSE,
            DWCi_EncSessionProgressCallback,
            DWCi_EncSessionCompleteCallback,
            NULL);
        g_session.reqid = result;
        if (result >= 0)
        {
            g_session.state = DWCi_ENC_SESSION_STATE_GETTING_TOKEN;
        }
        else
        {
            DWCi_EncSessionSetError();
        }
        break;

    case DWCi_ENC_SESSION_STATE_GETTING_TOKEN:
        if (!DWC_ProcessGHTTP())
        {
            DWCi_EncSessionSetError();
        }
        break;

    case DWCi_ENC_SESSION_STATE_GOT_TOKEN:
        result = DWC_GetGHTTPDataEx(g_session.request,
            0,
            FALSE,
            DWCi_EncSessionProgressCallback,
            DWCi_EncSessionCompleteCallback,
            NULL);
        g_session.reqid = result;
        if (result >= 0)
        {
            g_session.state = DWCi_ENC_SESSION_STATE_SENDING_DATA;
        }
        else
        {
            DWCi_EncSessionSetError();
        }
        break;

    case DWCi_ENC_SESSION_STATE_SENDING_DATA:
        if (!DWC_ProcessGHTTP())
        {
            DWCi_EncSessionSetError();
        }
        break;

#if !defined(VERSION_R4QK01)
    case DWCi_ENC_SESSION_STATE_COMPLETED:
        break;
#endif

    default:
        break;
    }

    return g_session.state;
}

#if defined(VERSION_R4QK01)
void DWCi_EncSessionCancel(void)
{
    if (g_session.reqid >= 0)
    {
        DWC_CancelGHTTPRequest(g_session.reqid);
    }
    g_session.state = DWCi_ENC_SESSION_STATE_CANCELED;
}
#endif

void DWCi_EncSessionShutdown(void)
{
#if defined(VERSION_R4QK01)
    if (!g_session.initialized)
    {
        return;
    }
#endif
    if (g_session.request != NULL)
    {
        DWC_Free(DWC_ALLOCTYPE_ENC, g_session.request, 0);
        g_session.request = NULL;
    }

    if (g_session.resbuf != NULL)
    {
        DWCi_GsFree(g_session.resbuf);
        g_session.resbuflen = 0;
        g_session.resbuf = NULL;
    }

    DWC_ShutdownGHTTP();
    g_session.state = DWCi_ENC_SESSION_STATE_INITIAL;
#if defined(VERSION_R4QK01)
    g_session.initialized = FALSE;
#endif
}

void* DWCi_EncSessionGetResponse(u32* size)
{
    *size = g_session.decodedbuflen;
    return g_session.decodedbuf;
}

#if defined(VERSION_R4QK01)
static DWCiRankingSessionResult DWCi_EncSessionGetAsyncEx(const char* url,
    s32 pid,
    void* data,
    u32 size,
    BOOL (*cb)(u32* data, int size),
    BOOL reuseHash)
#else
DWCiRankingSessionResult DWCi_EncSessionGetAsync(const char* url,
    s32 pid,
    void* data,
    u32 size,
    BOOL (*cb)(u32* data, int size))
#endif
{
    struct
    {
        s32 pid;
        u32 size;
    } putheader;
    char buf[16];
    DWCiRankingSessionResult res;

    if (g_session.validated != TRUE)
    {
        return DWCi_ENC_SESSION_ERROR_INVALID_KEY;
    }

    if (g_session.request != NULL)
    {
        DWC_Free(DWC_ALLOCTYPE_ENC, g_session.request, 0);
        g_session.request = NULL;
    }

    g_session.request = DWC_Alloc(DWC_ALLOCTYPE_ENC,
        strlen(DWCi_ENC_URL_BASE)
            + strlen(g_session.gamename)
            + strlen(url)
            + strlen("?pid=")
#if defined(VERSION_R4QK01)
            + snprintf(buf, sizeof(buf), "%d", pid)
#else
            + strlen(itoa(pid, buf, 10))
#endif
            + strlen("&hash=")
            + DWCi_ENC_HASH_LENGTH
            + strlen("&data=")
            + b64size(size + sizeof(putheader) + 4)
            + 1);
    if (g_session.request == NULL)
    {
        return DWCi_ENC_SESSION_ERROR_NOMEMORY;
    }

    sprintf(g_session.request,
        "%s%s%s?pid=%d&hash=%s&data=",
        DWCi_ENC_URL_BASE,
        g_session.gamename,
        url,
        pid,
        "0000000000000000000000000000000000000000",
        "");

    g_session.data = g_session.request + strlen(g_session.request);
    g_session.hash = g_session.data
                   - strlen("&data=")
                   - DWCi_ENC_HASH_LENGTH;

    putheader.pid = DWCi_RankingByteSwap32(pid);
    putheader.size = DWCi_RankingByteSwap32(size);
    res = DWCi_EncSessionEncrypt((u8*)g_session.data,
        (const u8*)&putheader,
        sizeof(putheader),
        data,
        size);

    if (res == DWCi_ENC_SESSION_ERROR_NOMEMORY)
    {
        DWC_Free(DWC_ALLOCTYPE_ENC, g_session.request, 0);
        g_session.request = NULL;
        return DWCi_ENC_SESSION_ERROR_NOMEMORY;
    }

#if defined(VERSION_R4QK01)
    if (reuseHash)
    {
        memcpy(g_session.hash, g_session.cachedHash, DWCi_ENC_HASH_LENGTH);
        g_session.state = DWCi_ENC_SESSION_STATE_GOT_TOKEN;
    }
    else
#endif
    {
        *(g_session.hash - strlen("&hash=")) = '\0';
        g_session.state = DWCi_ENC_SESSION_STATE_REQUEST;
    }
    g_session.callback = cb;
    return DWCi_ENC_SESSION_SUCCESS;
}

#if defined(VERSION_R4QK01)
DWCiRankingSessionResult DWCi_EncSessionGetAsync(const char* url,
    s32 pid,
    void* data,
    u32 size,
    BOOL (*cb)(u32* data, int size))
{
    return DWCi_EncSessionGetAsyncEx(url, pid, data, size, cb, FALSE);
}
#endif
