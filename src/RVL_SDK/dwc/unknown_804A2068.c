#include <dwc/dwc_memfunc.h>
#include <dwc/dwc_ranksession.h>
#include <dwc/dwc_report.h>

#include <string.h>

typedef BOOL (*DWCiRankingResponseCallback)(char* data, int size);

typedef struct
{
    u32 region;
    u32 category;
    s32 score;
    u32 size;
} DWCiRankingPutHeader;

typedef struct
{
    u32 region;
    u32 category;
    DWCRnkGetMode mode;
    u32 size;
} DWCiRankingGetHeader;

void fn_804A1AE8(int server, const char* initdata);
DWCiRankingSessionState fn_804A1C54(void);
void fn_804A1D74(void);
void* fn_804A1DF4(u32* size);
DWCiRankingSessionResult fn_804A1E0C(const char* path,
    s32 pid,
    const void* payload,
    u32 size,
    DWCiRankingResponseCallback response_callback);

static BOOL fn_804A2310(char* data, int size);

static u32 DWCi_RankingByteSwap32(u32 value)
{
    return ((value >> 24) & 0x000000FF) | ((value >> 8) & 0x0000FF00)
         | ((value << 8) & 0x00FF0000) | ((value << 24) & 0xFF000000);
}

void DWCi_RankingSessionInitialize(BOOL releaseserver, const char* initdata)
{
    fn_804A1AE8(!releaseserver, initdata);
}

DWCiRankingSessionState DWCi_RankingSessionProcess(void)
{
    return fn_804A1C54();
}

void DWCi_RankingSessionShutdown(void)
{
    fn_804A1D74();
}

void* DWCi_RankingSessionGetResponse(u32* size)
{
    return fn_804A1DF4(size);
}

DWCiRankingSessionResult DWCi_RankingSessionPutAsync(u32 category,
    s32 pid,
    DWCRnkRegion region,
    s32 score,
    void* data,
    u32 size)
{
    DWCiRankingPutHeader putheader;
    u8* buf;
    DWCiRankingSessionResult res;

    putheader.region = DWCi_RankingByteSwap32(region);
    putheader.category = DWCi_RankingByteSwap32(category);
    putheader.score = DWCi_RankingByteSwap32(score);
    putheader.size = DWCi_RankingByteSwap32(size);

    buf = DWC_Alloc(DWC_ALLOCTYPE_RANKING, size + sizeof(putheader));
    if (buf == NULL)
    {
        return DWCi_RANKING_SESSION_ERROR_NOMEMORY;
    }

    memcpy(buf, &putheader, sizeof(putheader));
    memcpy(buf + sizeof(putheader), data, size);
    res = fn_804A1E0C("/web/client/put2.asp", pid, buf, size + sizeof(putheader), NULL);
    DWC_Free(DWC_ALLOCTYPE_RANKING, buf, 0);
    return res;
}

DWCiRankingSessionResult DWCi_RankingSessionGetAsync(DWCRnkGetMode mode,
    u32 category,
    s32 pid,
    DWCRnkRegion region,
    DWCRnkGetParam* param)
{
    DWCiRankingGetHeader getheader;
    DWCRnkGetParam localparam;
    u8* buf;
    u32 i;
    DWCiRankingSessionResult res;

    localparam = *param;
    getheader.region = DWCi_RankingByteSwap32(region);
    getheader.category = DWCi_RankingByteSwap32(category);
    getheader.mode = DWCi_RankingByteSwap32(mode);
    getheader.size = DWCi_RankingByteSwap32(localparam.size);

    for (i = 0; i < localparam.size / sizeof(u32); i++)
    {
        ((u32*)&localparam.data)[i] = DWCi_RankingByteSwap32(((u32*)&localparam.data)[i]);
    }

    buf = DWC_Alloc(DWC_ALLOCTYPE_RANKING, localparam.size + sizeof(getheader));
    if (buf == NULL)
    {
        return DWCi_RANKING_SESSION_ERROR_NOMEMORY;
    }

    memcpy(buf, &getheader, sizeof(getheader));
    memcpy(buf + sizeof(getheader), &localparam.data, localparam.size);
    res = fn_804A1E0C("/web/client/get2.asp",
        pid,
        buf,
        localparam.size + sizeof(getheader),
        fn_804A2310);
    DWC_Free(DWC_ALLOCTYPE_RANKING, buf, 0);
    return res;
}

static BOOL fn_804A2310(char* data, int size)
{
    u32* words;
    u8* end;
    u32 offset;
    u32 i;
    u32 j;

    if (size <= 0)
    {
        return TRUE;
    }

    words = (u32*)data;
    words[0] = DWCi_RankingByteSwap32(words[0]);

    switch (words[0])
    {
    case DWC_RNK_GET_MODE_ORDER:
        words[1] = DWCi_RankingByteSwap32(words[1]);
        words[2] = DWCi_RankingByteSwap32(words[2]);
        break;

    case DWC_RNK_GET_MODE_TOPLIST:
    case DWC_RNK_GET_MODE_NEAR:
    case DWC_RNK_GET_MODE_FRIENDS:
        end = (u8*)data + size;
        words[1] = DWCi_RankingByteSwap32(words[1]);
        words[2] = DWCi_RankingByteSwap32(words[2]);
        offset = 3;

        for (i = 0; i < words[1]; i++)
        {
            if ((u8*)&words[offset + 5] > end)
            {
                DWC_Printf(DWC_REPORTFLAG_ERROR, "memory access over\n");
                return FALSE;
            }

            for (j = 0; j < 6; j++)
            {
                words[offset + j] = DWCi_RankingByteSwap32(words[offset + j]);
            }

            offset += 6 + words[offset + 5] / sizeof(u32);
        }
        break;

    default:
        DWC_Printf(DWC_REPORTFLAG_ERROR, "Invalid RNK_GET mode : %d\n", words[0]);
        return FALSE;
    }

    return TRUE;
}
