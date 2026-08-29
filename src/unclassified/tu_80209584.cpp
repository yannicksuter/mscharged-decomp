#include "unclassified/tu_80209584.h"

#include "Game/FE/feFinder.h"
#include "Game/FE/fePresentation.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlImageInstance.h"
#include "Game/FE/tlTextInstance.h"
#include "NL/nlFormat.h"
#include "NL/nlLocalization.h"
#include "NL/nlString.h"

extern "C" void* memcpy(void* dest, const void* src, unsigned long count);

typedef BasicString<unsigned short, Detail::TempStringAllocator> WideBasicString;

extern "C" TLInstance* fn_8030677C(FEPresentation* pPresentation, unsigned long Level1, unsigned long Level2,
    unsigned long Level3, unsigned long Level4, unsigned long Level5, unsigned long Level6);

static const unsigned short* LookupSummaryLocString(const char* id)
{
    nlLocalization* localization = g_pLocalization;
    unsigned long hash = nlStringLowerHash(id);
    if (localization->m_LookupTable == 0)
    {
        return LocalizationTableNotFound;
    }
    nlLocalization::StringLookup* lookup = nlBSearch<nlLocalization::StringLookup, unsigned long>(hash, localization->m_LookupTable, localization->m_pFile->StringCount);
    if (lookup != 0)
    {
        return localization->m_FirstString + lookup->StringOffset;
    }
    return MissingLocString;
}

TU80209584Summary::TU80209584Summary()
{
    mPresentation = 0;
}

TU80209584Summary::~TU80209584Summary()
{
}

void TU80209584Summary::fn_802095D0(const TU80209584StatsSource* home, const TU80209584StatsSource* away, FEPresentation* presentation)
{
    mStatsA = home->mStats;
    mPresentation = presentation;
    mTeamA = home->mTeam;
    mTeamB = away->mTeam;
    mStatsB = away->mStats;

    TLInstance* pSummary = fn_8030677C(mPresentation, nlStringLowerHash("game summary"), nlStringLowerHash("Layer"), nlStringLowerHash("game summary"), 0, 0, 0);

    unsigned long subHash = 0;
    const TU80209584Stats* stats[2] = { &mStatsA, &mStatsB };
    int percents[2] = { 0, 0 };

    unsigned int total = mStatsA.mValue3C + mStatsB.mValue3C;
    if ((float)total != 0.0f)
    {
        int percent = (int)(100.0f * ((float)mStatsA.mValue3C / (float)total));
        percents[0] = percent;
        percents[1] = 100 - percent;
    }

    static const char* STAT_INSTANCES[7][2] = {
        { "STAT_1_LEFT", "STAT_1_RIGHT" },
        { "STAT_2_LEFT", "STAT_2_RIGHT" },
        { "STAT_3_LEFT", "STAT_3_RIGHT" },
        { "STAT_4_LEFT", "STAT_4_RIGHT" },
        { "STAT_5_LEFT", "STAT_5_RIGHT" },
        { "STAT_6_LEFT", "STAT_6_RIGHT" },
        { "score_left", "score_right" },
    };

    for (int side = 0; side < 2; side++)
    {
        TLInstance* rows[7];
        int value = 0;
        int extra = 0;

        for (int row = 0; row < 7; row++)
        {
            switch (row)
            {
            case 0:
                value = stats[side]->mValue06;
                break;
            case 1:
                value = stats[side]->mValue14;
                break;
            case 2:
                value = stats[side]->mValue32;
                break;
            case 3:
                value = stats[side]->mValue36;
                break;
            case 4:
                value = stats[side]->mValue18;
                extra = stats[side]->mValue16;
                break;
            case 5:
                value = percents[side];
                break;
            case 6:
                value = stats[side]->mValue10;
                break;
            default:
                break;
            }

            TLInstance* pInstance = FEFinder<TLTextInstance, 3>::_Find(pSummary, nlStringLowerHash(STAT_INSTANCES[row][side]), subHash, 0, 0, 0, 0);
            rows[row] = pInstance;
            fn_8020A014(side, row, value, extra, rows[row]);
        }
    }

    TLComponentInstance* pIconComponent = (TLComponentInstance*)FEFinder<TLComponentInstance, 4>::_Find(pSummary, nlStringLowerHash("team_icon_left"), 0, 0, 0, 0, 0);
    TLInstance* pIcon = FEFinder<TLInstance, 3>::_Find(pIconComponent->GetActiveSlide(), nlStringLowerHash("team_icon"), subHash, 0, 0, 0, 0);

    const char* CAPTAIN_TEXTURES[12] = {
        "captain_mario_s",
        "captain_bowser_s",
        "captain_daisy_s",
        "captain_donkeykong_s",
        "captain_luigi_s",
        "captain_peach_s",
        "captain_waluigi_s",
        "captain_wario_s",
        "captain_yoshi_s",
        "captain_bowserjr_s",
        "captain_diddykong_s",
        "captain_petey_s",
    };

    TLImageInstance* pTexture = (TLImageInstance*)fn_8030677C(mPresentation, nlStringLowerHash("art"), nlStringLowerHash("Layer"), nlStringLowerHash(CAPTAIN_TEXTURES[mTeamA]), 0, 0, 0);
    if (pTexture->m_pTextureResource != 0)
    {
        ((TLImageInstance*)pIcon)->m_pTextureResource = pTexture->m_pTextureResource;
    }

    pIconComponent = (TLComponentInstance*)FEFinder<TLComponentInstance, 4>::_Find(pSummary, nlStringLowerHash("team_icon_right"), 0, 0, 0, 0, 0);
    pIcon = FEFinder<TLInstance, 3>::_Find(pIconComponent->GetActiveSlide(), nlStringLowerHash("team_icon"), subHash, 0, 0, 0, 0);

    pTexture = (TLImageInstance*)fn_8030677C(mPresentation, nlStringLowerHash("art"), nlStringLowerHash("Layer"), nlStringLowerHash(CAPTAIN_TEXTURES[mTeamB]), 0, 0, 0);
    if (pTexture->m_pTextureResource != 0)
    {
        ((TLImageInstance*)pIcon)->m_pTextureResource = pTexture->m_pTextureResource;
    }
}

void TU80209584Summary::fn_8020A014(int side, int row, int value, int extra, TLInstance* instance)
{
    static const char* STAT_VALUE_IDS[7] = {
        "GAME_RESULTS_STAT_1_VALUE",
        "GAME_RESULTS_STAT_2_VALUE",
        "GAME_RESULTS_STAT_3_VALUE",
        "GAME_RESULTS_STAT_4_VALUE",
        "GAME_RESULTS_STAT_5_VALUE",
        "GAME_RESULTS_STAT_6_VALUE",
        "SCORE",
    };

    unsigned short wideValue[4];
    unsigned short wideExtra[4];
    WideBasicString unformatted;
    WideBasicString formatted;
    char valueBuffer[4];
    char extraBuffer[4];

    nlSNPrintf(valueBuffer, 4, "%d", value);
    nlStrToWcs(valueBuffer, wideValue, 4);
    nlSNPrintf(extraBuffer, 4, "%d", extra);
    nlStrToWcs(extraBuffer, wideExtra, 4);

    unformatted = WideBasicString(LookupSummaryLocString(STAT_VALUE_IDS[row]));
    formatted = Format(unformatted, wideValue, wideExtra);

    const unsigned short* string = formatted.c_str();

    memcpy(&mBuffersColBySide[side][row][0], string, 0x40);
    ((TLTextInstance*)instance)->SetString(&mBuffersColBySide[side][row][0]);
}
