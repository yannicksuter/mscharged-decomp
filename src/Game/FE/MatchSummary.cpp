#include "Game/FE/MatchSummary.h"

#include "Game/FE/feFinder.inl"
#include "Game/FE/fePresentation.h"
#include "Game/FE/tlComponentInstance.h"
#include "Game/FE/tlImageInstance.h"
#include "Game/FE/tlTextInstance.h"
#include "NL/nlFormat.h"
#include "NL/nlLocalizationLookup.h"
#include "NL/nlString.h"
#include "NL/nlPrint.h"

#include <string.h>

typedef BasicString<unsigned short, Detail::TempStringAllocator> WideBasicString;

static const char* STAT_INSTANCES[7][2] = {
    { "STAT_1_LEFT", "STAT_1_RIGHT" },
    { "STAT_2_LEFT", "STAT_2_RIGHT" },
    { "STAT_3_LEFT", "STAT_3_RIGHT" },
    { "STAT_4_LEFT", "STAT_4_RIGHT" },
    { "STAT_5_LEFT", "STAT_5_RIGHT" },
    { "STAT_6_LEFT", "STAT_6_RIGHT" },
    { "score_left", "score_right" },
};

static const char* STAT_VALUE_IDS[7] = {
    "GAME_RESULTS_STAT_1_VALUE",
    "GAME_RESULTS_STAT_2_VALUE",
    "GAME_RESULTS_STAT_3_VALUE",
    "GAME_RESULTS_STAT_4_VALUE",
    "GAME_RESULTS_STAT_5_VALUE",
    "GAME_RESULTS_STAT_6_VALUE",
    "SCORE",
};

MatchSummary::MatchSummary()
{
    mPresentation = 0;
}

MatchSummary::~MatchSummary()
{
}

void MatchSummary::DisplayMatchSummary(TeamStats home, TeamStats away, FEPresentation* presentation)
{
    mPresentation = presentation;
    mTeamIDs[0] = home.mTeamIndex;
    mTeamIDs[1] = away.mTeamIndex;
    mPlayerStats[0] = home.mPlayerTotalStats;
    mPlayerStats[1] = away.mPlayerTotalStats;

    TLInstance* pSummary = FEFinder<TLInstance, TLAT_UNKNOWN>::Find<>(mPresentation, "game summary", "Layer", "game summary");

    PlayerStats* displayedStats[2] = { &mPlayerStats[0], &mPlayerStats[1] };
    TLTextInstance* pTextInstanceRows[7] = { 0 };
    int percents[2];

    float total = (float)(mPlayerStats[0].unknown_0x3C + mPlayerStats[1].unknown_0x3C);
    if (0.0 == total)
    {
        percents[0] = 0;
        percents[1] = 0;
    }
    else
    {
        int percent = (int)(100.0f * ((float)mPlayerStats[0].unknown_0x3C / total));
        percents[0] = percent;
        percents[1] = 100 - percent;
    }

    for (int side = 0; side < 2; side++)
    {
        int value = 0;
        int extra = 0;

        for (int row = 0; row < 7; row++)
        {
            switch (row)
            {
            case 0:
                value = displayedStats[side]->mNumShotsOnGoal;
                break;
            case 1:
                value = displayedStats[side]->unknown_0x14;
                break;
            case 2:
                value = displayedStats[side]->mNumHitsMade;
                break;
            case 3:
                value = displayedStats[side]->mNumSteals;
                break;
            case 4:
                value = displayedStats[side]->unknown_0x18;
                extra = displayedStats[side]->unknown_0x16;
                break;
            case 5:
                value = percents[side];
                break;
            case 6:
                value = displayedStats[side]->mNumGoalsFor;
                break;
            default:
                break;
            }

            TLTextInstance* pInstance = FEFinder<TLTextInstance, 3>::Find<>(pSummary, STAT_INSTANCES[row][side]);
            pTextInstanceRows[row] = pInstance;
            SetStatValue(side, row, value, extra, pTextInstanceRows[row]);
        }
    }

    TLComponentInstance* pIconComponent = FEFinder<TLComponentInstance, 4>::Find<>(pSummary, "team_icon_left");
    TLImageInstance* pIcon = FEFinder<TLImageInstance, 2>::Find<>(pIconComponent->GetActiveSlide(), "team_icon");

    TLImageInstance* pTexture = FEFinder<TLImageInstance, 2>::Find<>(mPresentation, "art", "Layer", GetCaptainTextureName(mTeamIDs[0]));
    pIcon->SetTextureResource(pTexture->GetTextureResource());

    pIconComponent = FEFinder<TLComponentInstance, 4>::Find<>(pSummary, "team_icon_right");
    pIcon = FEFinder<TLImageInstance, 2>::Find<>(pIconComponent->GetActiveSlide(), "team_icon");

    pTexture = FEFinder<TLImageInstance, 2>::Find<>(mPresentation, "art", "Layer", GetCaptainTextureName(mTeamIDs[1]));
    pIcon->SetTextureResource(pTexture->GetTextureResource());
}

void MatchSummary::SetStatValue(int side, int row, int value, int extra, TLTextInstance* instance)
{
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

    unformatted = WideBasicString(LookupLocString(STAT_VALUE_IDS[row]));
    formatted = Format(unformatted, wideValue, wideExtra);

    const unsigned short* string = formatted.c_str();

    memcpy(&mBuffersColBySide[side][row][0], string, 0x40);
    instance->SetString(&mBuffersColBySide[side][row][0]);
}

const char* MatchSummary::GetCaptainTextureName(int team)
{
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
    return CAPTAIN_TEXTURES[team];
}
