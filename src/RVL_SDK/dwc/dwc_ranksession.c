#include <dwc/dwc_memfunc.h>
#include <dwc/dwc_encsession.h>
#include <dwc/dwc_ranksession.h>
#include <dwc/dwc_report.h>

#include <string.h>

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

static BOOL DWCi_RankingSessionVaridateResponse(u32* buf, int len);

#define DWCi_RankingByteSwap32(value) \
    ((((u32)(value) << 24) & 0xFF000000) | (((u32)(value) << 8) & 0x00FF0000) \
        | (((u32)(value) >> 8) & 0x0000FF00) | (((u32)(value) >> 24) & 0x000000FF))

void DWCi_RankingSessionInitialize(BOOL releaseserver, const char* initdata)
{
    DWCi_EncSessionInitialize(!releaseserver, initdata);
}

DWCiRankingSessionState DWCi_RankingSessionProcess(void)
{
    return DWCi_EncSessionProcess();
}

void DWCi_RankingSessionShutdown(void)
{
    DWCi_EncSessionShutdown();
}

void* DWCi_RankingSessionGetResponse(u32* size)
{
    return DWCi_EncSessionGetResponse(size);
}

DWCiRankingSessionResult DWCi_RankingSessionPutAsync(u32 category,
    s32 pid,
    DWCRnkRegion region,
    s32 score,
    void* data,
    u32 size)
{
    DWCiRankingPutHeader putheader;
    DWCiRankingSessionResult res;
    u8* tmpbuf;

    putheader.region = DWCi_RankingByteSwap32(region);
    putheader.category = DWCi_RankingByteSwap32(category);
    putheader.score = DWCi_RankingByteSwap32(score);
    putheader.size = DWCi_RankingByteSwap32(size);

    tmpbuf = DWC_Alloc(DWC_ALLOCTYPE_RANKING, size + sizeof(putheader));
    if (tmpbuf == NULL)
    {
        return DWCi_RANKING_SESSION_ERROR_NOMEMORY;
    }

    memcpy(tmpbuf, &putheader, sizeof(putheader));
    memcpy(tmpbuf + sizeof(putheader), data, size);
    res = DWCi_EncSessionGetAsync("/web/client/put2.asp", pid, tmpbuf, size + sizeof(putheader), NULL);
    DWC_Free(DWC_ALLOCTYPE_RANKING, tmpbuf, 0);
    return res;
}

DWCiRankingSessionResult DWCi_RankingSessionGetAsync(DWCRnkGetMode mode,
    u32 category,
    s32 pid,
    DWCRnkRegion region,
    DWCRnkGetParam* param)
{
    DWCiRankingGetHeader getheader;
    DWCiRankingSessionResult res;
    DWCRnkGetParam paramcopy;
    u8* tmpbuf;
    int i;
    u32* ptr;

    paramcopy = *param;
    getheader.region = DWCi_RankingByteSwap32(region);
    getheader.category = DWCi_RankingByteSwap32(category);
    getheader.mode = DWCi_RankingByteSwap32(mode);
    getheader.size = DWCi_RankingByteSwap32(paramcopy.size);

    ptr = &paramcopy.data;
    for (i = 0; i < paramcopy.size / sizeof(u32); i++)
    {
        ptr[i] = DWCi_RankingByteSwap32(ptr[i]);
    }

    tmpbuf = DWC_Alloc(DWC_ALLOCTYPE_RANKING, paramcopy.size + sizeof(getheader));
    if (tmpbuf == NULL)
    {
        return DWCi_RANKING_SESSION_ERROR_NOMEMORY;
    }

    memcpy(tmpbuf, &getheader, sizeof(getheader));
    memcpy(tmpbuf + sizeof(getheader), &paramcopy.data, paramcopy.size);
    res = DWCi_EncSessionGetAsync("/web/client/get2.asp",
        pid,
        tmpbuf,
        paramcopy.size + sizeof(getheader),
        DWCi_RankingSessionVaridateResponse);
    DWC_Free(DWC_ALLOCTYPE_RANKING, tmpbuf, 0);
    return res;
}

static BOOL DWCi_RankingSessionVaridateResponse(u32* buf, int len)
{
    u32* ptr;
    int offset;
    int i;

    if (len > 0)
    {
        ptr = buf;
        ptr[0] = DWCi_RankingByteSwap32(ptr[0]);

        switch (ptr[0])
        {
        case DWC_RNK_GET_MODE_ORDER:
            ptr[1] = DWCi_RankingByteSwap32(ptr[1]);
            ptr[2] = DWCi_RankingByteSwap32(ptr[2]);
            break;

        case DWC_RNK_GET_MODE_TOPLIST:
        case DWC_RNK_GET_MODE_NEAR:
        case DWC_RNK_GET_MODE_FRIENDS:
            ptr[1] = DWCi_RankingByteSwap32(ptr[1]);
            ptr[2] = DWCi_RankingByteSwap32(ptr[2]);
            offset = 3;

            for (i = 0; i < ptr[1]; i++)
            {
                if ((u8*)&ptr[offset + 5] > (u8*)buf + len)
                {
                    DWC_Printf(DWC_REPORTFLAG_ERROR, "memory access over\n");
                    return FALSE;
                }

                ((DWCRnkData*)&ptr[offset])->order =
                    DWCi_RankingByteSwap32(((DWCRnkData*)&ptr[offset])->order);
                ((DWCRnkData*)&ptr[offset])->pid =
                    DWCi_RankingByteSwap32(((DWCRnkData*)&ptr[offset])->pid);
                ((DWCRnkData*)&ptr[offset])->score =
                    DWCi_RankingByteSwap32(((DWCRnkData*)&ptr[offset])->score);
                ((DWCRnkData*)&ptr[offset])->region =
                    DWCi_RankingByteSwap32(((DWCRnkData*)&ptr[offset])->region);
                ((DWCRnkData*)&ptr[offset])->lastupdate =
                    DWCi_RankingByteSwap32(((DWCRnkData*)&ptr[offset])->lastupdate);
                ((DWCRnkData*)&ptr[offset])->size =
                    DWCi_RankingByteSwap32(((DWCRnkData*)&ptr[offset])->size);

                offset += 6 + ((DWCRnkData*)&ptr[offset])->size / sizeof(u32);
            }
            break;

        default:
            DWC_Printf(DWC_REPORTFLAG_ERROR, "Invalid RNK_GET mode : %d\n", ptr[0]);
            return FALSE;
        }
    }

    return TRUE;
}
