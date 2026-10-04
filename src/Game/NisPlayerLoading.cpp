namespace
{
static void* sTriggerByteCode;
}

const char* NisPlayer::GetTargetFilter(NisTarget target, NisWinnerType winnerType) const
{
    if (target == NIS_TARGET_STADIUM)
    {
        int stadium = GameInfoManager::Instance()->GetStadium();
        const char* stadiumName = GetStadiumName(stadium);
        return stadiumName;
    }

    if (target == NIS_TARGET_HOME_CAPTAIN)
    {
        return GetTeamName((eTeamID)GameInfoManager::Instance()->GetTeam(0));
    }

    if (target == NIS_TARGET_AWAY_CAPTAIN)
    {
        return GetTeamName((eTeamID)GameInfoManager::Instance()->GetTeam(1));
    }

    if (target == NIS_TARGET_HOME_SIDEKICK)
    {
        return GetSidekickName((eSidekickID)GameInfoManager::Instance()->GetSidekick(0, 0));
    }

    if (target == NIS_TARGET_HOME_SIDEKICK_1)
    {
        return GetSidekickName((eSidekickID)GameInfoManager::Instance()->GetSidekick(0, 0));
    }

    if (target == NIS_TARGET_HOME_SIDEKICK_2)
    {
        return GetSidekickName((eSidekickID)GameInfoManager::Instance()->GetSidekick(0, 1));
    }

    if (target == NIS_TARGET_HOME_SIDEKICK_3)
    {
        return GetSidekickName((eSidekickID)GameInfoManager::Instance()->GetSidekick(0, 2));
    }

    if (target == NIS_TARGET_AWAY_SIDEKICK)
    {
        return GetSidekickName((eSidekickID)GameInfoManager::Instance()->GetSidekick(1, 0));
    }

    if (target == NIS_TARGET_AWAY_SIDEKICK_1)
    {
        return GetSidekickName((eSidekickID)GameInfoManager::Instance()->GetSidekick(1, 0));
    }

    if (target == NIS_TARGET_AWAY_SIDEKICK_2)
    {
        return GetSidekickName((eSidekickID)GameInfoManager::Instance()->GetSidekick(1, 1));
    }

    if (target == NIS_TARGET_AWAY_SIDEKICK_3)
    {
        return GetSidekickName((eSidekickID)GameInfoManager::Instance()->GetSidekick(1, 2));
    }

    if (target == NIS_TARGET_SCORER)
    {
        return g_pCharacters[mGoalScorerCharIndex]->mUnidentified11C->mName;
    }

    if (target == NIS_TARGET_WINNER_SIDEKICK)
    {
        return GetSidekickName((eSidekickID)GameInfoManager::Instance()->GetSidekick((short)GetWinnerSide(winnerType), 0));
    }

    if (target == NIS_TARGET_LOSER_SIDEKICK)
    {
        int side = (GetWinnerSide(winnerType) + 1) % 2;
        return GetSidekickName((eSidekickID)GameInfoManager::Instance()->GetSidekick((short)side, 0));
    }

    if (target == NIS_TARGET_WINNER_CAPTAIN)
    {
        return GetTeamName((eTeamID)GameInfoManager::Instance()->GetTeam((short)GetWinnerSide(winnerType)));
    }

    if (target == NIS_TARGET_LOSER_CAPTAIN)
    {
        int side = (GetWinnerSide(winnerType) + 1) % 2;
        return GetTeamName((eTeamID)GameInfoManager::Instance()->GetTeam((short)side));
    }

    if (target == NIS_TARGET_MEGASTRIKE_CAPTAIN)
    {
        return GetTeamName((eTeamID)GameInfoManager::Instance()->GetTeam((short)mMegaStrikeSide));
    }

    if (target == NIS_TARGET_HOME_GOALIE || target == NIS_TARGET_AWAY_GOALIE || target == NIS_TARGET_WINNER_GOALIE || target == NIS_TARGET_LOSER_GOALIE || target == NIS_TARGET_MEGASTRIKE_DEFENDING_GOALIE)
    {
        return "goalie";
    }

    if (target == NIS_TARGET_AWAY_SIDEKICK)
    {
        return GetSidekickName((eSidekickID)GameInfoManager::Instance()->GetSidekick(1, 0));
    }

    return "";
}

static inline void FormatNisName(char* fullName, const char* filter, const char* nisType, NisUseFilter useFilter, const char* extraNameFilter)
{
    char prefix[64];
    if (nlStrCmp(filter, "") != 0)
    {
        nlSNPrintf(prefix, sizeof(prefix), "%s_", filter);
    }
    else
    {
        prefix[0] = '\0';
    }

    char extra[64];
    if (useFilter != NIS_NO_FILTER)
    {
        nlSNPrintf(extra, sizeof(extra), "_%s", extraNameFilter);
    }
    else
    {
        extra[0] = '\0';
    }

    nlSNPrintf(fullName, 64, "%s%s%s", prefix, nisType, extra);
}

static inline int RandomNisIndex(int count, unsigned int* seed)
{
    float value = (count - 1) * nlRandomf(1.0f, seed);
    value += value < 0.0f ? -0.5f : 0.5f;
    return (int)value;
}

static inline void PlayNisCue(NisPlayer* player, const char* nisName)
{
    char cueName[128];
    nlStrNCpy(cueName, nisName, sizeof(cueName));
    unsigned long length = nlStrLen(cueName);
    if (GetStadiumUnknown0x10(GameInfoManager::Instance()->GetStadium()))
    {
        cueName[length - 4] = '\0';
    }
    else
    {
        nlStrNCpy(cueName + length - 4, "_nocrowd", sizeof(cueName) - length - 4);
    }
    player->PrepareNisCue(nlStringLowerHash(cueName));
}

void NisPlayer::Load(const char* nisType, NisTarget target, NisUseStadiumOffset useStadiumOffset, NisUseFilter useFilter, NisWinnerType winnerType, int renderMode, int variantIndex)
{
    char fullName[64];
    mActive = true;

    const char* filter = GetTargetFilter(target, winnerType);
    FormatNisName(fullName, filter, nisType, useFilter, mExtraNameFilter);

    int numAvailableNis = 0;
    NisHeader* availableNis[10] = { 0 };
    int dictionaryIndex;
    for (dictionaryIndex = 0; dictionaryIndex < mDictSize && numAvailableNis < 10; dictionaryIndex++)
    {
        if (nlStrNICmp(mDict[dictionaryIndex].name, fullName, nlStrLen(fullName)) != 0)
        {
            continue;
        }
        NisHeader* candidate = &mDict[dictionaryIndex];
        if (strstr(candidate->name, "_same") != NULL)
        {
            continue;
        }
        if (strstr(candidate->name, "_other") != NULL)
        {
            continue;
        }
        availableNis[numAvailableNis++] = candidate;
    }

    if (numAvailableNis == 0)
    {
        return;
    }

    int index;
    if (variantIndex >= 0)
    {
        index = variantIndex % numAvailableNis;
    }
    else
    {
        index = RandomNisIndex(numAvailableNis, &GetPresentation()->mRandomSeed);
        if (DuringGoalCelebration(GetPresentation()) && renderMode == 0)
        {
            if (numAvailableNis > 1 && mLastCelebrationIndex == index && nlStrCmp(mLastCelebrationFilter, mExtraNameFilter) == 0)
            {
                while (index == mLastCelebrationIndex)
                {
                    index = RandomNisIndex(numAvailableNis, &GetPresentation()->mRandomSeed);
                }
            }
            mLastCelebrationIndex = index;
            nlStrNCpy(mLastCelebrationFilter, mExtraNameFilter, sizeof(mLastCelebrationFilter));
        }
    }

    NisHeader& nisHeader = *availableNis[index];
    QueueNisLoad(nisHeader, target, useStadiumOffset, winnerType, renderMode, false);

    NisTarget sameTarget = NIS_TARGET_NONE;
    NisTarget otherTarget = NIS_TARGET_NONE;
    switch (target)
    {
    case NIS_TARGET_HOME_CAPTAIN:
    case NIS_TARGET_HOME_GOALIE:
        sameTarget = NIS_TARGET_HOME_SIDEKICK;
        otherTarget = NIS_TARGET_AWAY_SIDEKICK;
        break;
    case NIS_TARGET_AWAY_CAPTAIN:
    case NIS_TARGET_AWAY_GOALIE:
        sameTarget = NIS_TARGET_AWAY_SIDEKICK;
        otherTarget = NIS_TARGET_HOME_SIDEKICK;
        break;
    case NIS_TARGET_LOSER_CAPTAIN:
    case NIS_TARGET_LOSER_GOALIE:
        sameTarget = NIS_TARGET_LOSER_SIDEKICK;
        otherTarget = NIS_TARGET_WINNER_SIDEKICK;
        break;
    case NIS_TARGET_WINNER_CAPTAIN:
    case NIS_TARGET_WINNER_GOALIE:
        sameTarget = NIS_TARGET_WINNER_SIDEKICK;
        otherTarget = NIS_TARGET_LOSER_SIDEKICK;
        break;
    case NIS_TARGET_SCORER:
    {
        cCharacter* character = g_pCharacters[mGoalScorerCharIndex];
        if (character != NULL && character->IsCaptain())
        {
            if (((cPlayer*)character)->m_pTeam->m_nSide == 0)
            {
                sameTarget = NIS_TARGET_HOME_SIDEKICK;
                otherTarget = NIS_TARGET_AWAY_SIDEKICK;
            }
            else
            {
                sameTarget = NIS_TARGET_AWAY_SIDEKICK;
                otherTarget = NIS_TARGET_HOME_SIDEKICK;
            }
        }
        break;
    }
    case NIS_TARGET_MEGASTRIKE_DEFENDING_GOALIE:
    {
        cCharacter* character = g_pCharacters[mGoalScorerCharIndex];
        if (character != NULL && character->IsCaptain())
        {
            if (((cPlayer*)character)->m_pTeam->m_nSide == 0)
            {
                sameTarget = NIS_TARGET_AWAY_SIDEKICK;
                otherTarget = NIS_TARGET_HOME_SIDEKICK;
            }
            else
            {
                sameTarget = NIS_TARGET_HOME_SIDEKICK;
                otherTarget = NIS_TARGET_AWAY_SIDEKICK;
            }
        }
        break;
    }
    case NIS_TARGET_MEGASTRIKE_CAPTAIN:
    {
        cCharacter* character = g_pCharacters[mGoalScorerCharIndex];
        if (character != NULL && character->IsCaptain())
        {
            if (((cPlayer*)character)->m_pTeam->m_nSide == 0)
            {
                sameTarget = NIS_TARGET_HOME_SIDEKICK;
                otherTarget = NIS_TARGET_AWAY_SIDEKICK;
            }
            else
            {
                sameTarget = NIS_TARGET_AWAY_SIDEKICK;
                otherTarget = NIS_TARGET_HOME_SIDEKICK;
            }
        }
        break;
    }
    }

    if (sameTarget != NIS_TARGET_NONE)
    {
        LoadRelatedNis(nisHeader.name, "same", sameTarget, useStadiumOffset, winnerType, nisHeader.mirrored, renderMode);
    }
    if (otherTarget != NIS_TARGET_NONE)
    {
        LoadRelatedNis(nisHeader.name, "other", otherTarget, useStadiumOffset, winnerType, nisHeader.mirrored, renderMode);
    }

    if (renderMode != 1 && mPreparedNisCue == 0)
    {
        PlayNisCue(this, nisHeader.name);
    }
}

void NisPlayer::LoadRelatedNis(const char* nisName, const char* relation, NisTarget target, NisUseStadiumOffset useStadiumOffset, NisWinnerType winnerType, bool mirrored, int renderMode)
{
    char baseName[64];
    int length = nlStrChr(nisName, '.') - nisName + 1;
    nlStrNCpy(baseName, nisName, nlMin((int)sizeof(baseName), length));

    const char* filter = GetTargetFilter(target, winnerType);
    char fullName[64];
    nlSNPrintf(fullName, sizeof(fullName), "%s_%s_%s.nis", baseName, filter, relation);

    NisHeader* nisHeader = NULL;
    for (int dictionaryIndex = 0; dictionaryIndex < mDictSize; dictionaryIndex++)
    {
        if (nlStrCmp(mDict[dictionaryIndex].name, fullName) == 0)
        {
            mDict[dictionaryIndex].mirrored = mirrored;
            nisHeader = &mDict[dictionaryIndex];
            break;
        }
    }

    if (nisHeader != NULL)
    {
        QueueNisLoad(*nisHeader, target, useStadiumOffset, winnerType, renderMode, true);
    }
}

void NisPlayer::QueueNisLoad(NisHeader& nisHeader, NisTarget target, NisUseStadiumOffset useStadiumOffset, NisWinnerType winnerType, int renderMode, bool preserveMirroring)
{
    nisHeader.target = target;
    nisHeader.winnerType = winnerType;
    nisHeader.mTime = 0.0f;
    nisHeader.renderMode = renderMode;
    if (!preserveMirroring)
    {
        nisHeader.mirrored = IsMirrored(target, nisHeader.name, winnerType);
    }
    if (useStadiumOffset == NIS_NO_STADIUM_OFFSET)
    {
        nisHeader.stadiumOffset.x = 0.0f;
        nisHeader.stadiumOffset.y = 0.0f;
        nisHeader.stadiumOffset.z = 0.0f;
    }
    else
    {
        float scale = (useStadiumOffset == NIS_AWAY_STADIUM_OFFSET) ? -1.0f : 1.0f;
        char offsetConfigName[64];
        nlSNPrintf(offsetConfigName, 64, "nisHeader/%s_offset", GetStadiumName(GameInfoManager::Instance()->GetStadium()));
        float offset = scale * GetConfigFloat(Config::Global(), offsetConfigName, 0.0f);
        nisHeader.stadiumOffset.x = 0.0f;
        nisHeader.stadiumOffset.y = offset;
        nisHeader.stadiumOffset.z = 0.0f;
    }
    for (int i = 0; i < nisHeader.numAnimations; i++)
    {
        mBeginPositions[i] = nisHeader.beginPositions[i];
        if (nisHeader.mirrored)
        {
            mBeginPositions[i].x *= -1.0f;
        }
    }
    if (nisHeader.buffer != NULL)
    {
        Load(nisHeader.buffer, nisHeader.bufferSize, nisHeader);
    }
    else
    {
        for (int i = 0; i < 8; i++)
        {
            if (mLoadQueue[i] == NULL)
            {
                mLoadQueue[i] = &nisHeader;
                break;
            }
        }
    }
}
