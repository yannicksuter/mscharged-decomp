#include "Game/AI/FuzzyVariant.h"

#include "Game/DB/CharacterInfo.h"
#include "Game/Player.h"
#include "Game/Team.h"
#include "NL/nlFormat.h"

FuzzyVariant fvNotSet;

unsigned long FuzzyVariant::GetHash() const
{
    unsigned long hash = 0;

    if (mType != FT_UNSPECIFIED && mType >= NUM_V_TYPES)
    {
        switch (GetType())
        {
        case FT_PLAYER:
            hash = (unsigned long)mData.pointer;
            break;
        case FT_TEAM:
            hash = mData.u;
            break;
        case FT_GAME:
            hash = (unsigned long)mData.pointer;
            break;
        case FT_BALL:
            hash = mData.u;
            break;
        }
    }

    return hash;
}

NLString FuzzyVariant::ToString() const
{
    NLString toString;

    if (mType != FT_UNSPECIFIED && mType >= NUM_V_TYPES)
    {
        NLString dataString = "???";

        switch (GetType())
        {
        case FT_PLAYER:
        {
            cPlayer* pPlayer = mData.pPlayer;
            if (pPlayer != 0)
            {
                NLString formatString("{0} {1}");
                int playerID;
                const char* playerName;
                playerName = GetCharacterInfo(pPlayer->m_eCharacterClass).mName;
                playerID = pPlayer->m_ID;
                dataString = Format(formatString, playerID, playerName);
            }
            break;
        }

        case FT_TEAM:
            if (mData.pTeam != 0)
            {
                dataString = Format(NLString("Team={0}"),
                    mData.pTeam->m_nSide == 0 ? "Home" : "Away");
            }
            break;

        case FT_GAME:
            if (mData.pointer != 0)
            {
                dataString = "Game";
            }
            break;

        case FT_BALL:
            if (mData.pointer != 0)
            {
                dataString = "Ball";
            }
            break;
        }

        toString = dataString;
    }
    else
    {
        toString = Variant::ToString();
    }

    return toString;
}
