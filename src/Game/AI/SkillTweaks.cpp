#include <stdlib.h>
#include <string.h>

#include "Game/AI/SkillTweaks.h"
#include "Game/AI/AiUtil.h"
#include "Game/AI/Fielder.h"
#include "Game/AI/Powerups.h"
#include "Game/CharacterTweaks.h"
#include "Game/Player.h"
#include "Game/Sys/simpleparser.h"
#include "Game/Team.h"
#include "NL/nlConfig.h"
#include "NL/nlList.h"
#include "NL/nlString.h"
#include "NL/nlstring_tmpl.h"
#include "NL/nlPrint.h"

struct SkillTweakModifier
{
    SkillTweakModifier(unsigned long modifier, const char* name)
        : mModifier(modifier)
        , mHash(nlStringLowerHash(name))
    {
    }

    unsigned long mModifier;
    unsigned long mHash;
    SkillTweakModifier* next;
};

static bool sUseCharacterDifficulty = true;
static float sMinimumCharacterDifficulty = 0.05f;
static float sMaximumCharacterDifficulty = 0.3f;
static nlList<SkillTweakModifier> sSkillTweakModifiers(0, 0);
static nlAVLTreeSlotPool<unsigned long, SkillTweak*, DefaultKeyCompare<unsigned long> > sDefaultSkillTweaks(16, 16);
static bool sSkillTweaksLoaded;

static void InitializeSkillTweakModifiers();

class SkillTweakLoader
{
public:
    SkillTweakLoader(Config* curves, Config* modifiers);
    void Load(const unsigned long& key, SkillTweak** value);

    Config* mCurves;
    Config* mModifiers;
};

class SkillTweakCopier
{
public:
    SkillTweakCopier(SkillTweaks* tweaks, Config* config);
    void Copy(const unsigned long& key, SkillTweak** value);

    SkillTweaks* mTweaks;
    Config* mConfig;
};

void SkillTweak::ParseCurve(const char* text, int length)
{
    const char* rangeToken = "Range";
    const char* curveToken = "Curve";
    int count = 0;
    float values[16];
    char* buffer = (char*)nlMalloc(length, 8, false);
    memcpy(buffer, text, length);
    SimpleParser parser;
    parser.StartParsing(buffer, length, ",: ");
    char* token = parser.NextToken(false);
    bool first = true;
    while (token != 0)
    {
        if (nlStrICmp(token, curveToken) == 0)
        {
            token = parser.NextToken(false);
            count = 0;
            while (count < 16 && token != 0)
            {
                values[count] = (float)atof(token);
                token = parser.NextToken(false);
                ++count;
            }
            if (token != 0)
                break;
        }
        else if (first)
        {
            values[0] = 0.0f;
            count = 3;
            values[1] = (float)atof(token);
            values[2] = 1.0f;
        }
        else if (nlStrICmp(token, rangeToken) == 0)
        {
            token = parser.NextToken(false);
            if (token != 0)
            {
                values[0] = (float)atof(token);
                token = parser.NextToken(false);
                if (token != 0)
                {
                    values[2] = (float)atof(token);
                    break;
                }
            }
        }
        first = false;
        token = parser.NextToken(false);
    }
    nlFree(buffer);
    mOwnsCurve = true;
    mpCurvePoints = new (8, false) nlVector2[count];
    for (int i = 0; i < count; ++i)
    {
        mpCurvePoints[i].x = (float)i / ((float)count - 1.0f);
        mpCurvePoints[i].y = values[i];
    }
    mCurve.mData = mpCurvePoints;
    mCurve.mCount = count;
}

float SkillTweak::GetValue()
{
    if (mpCurvePoints == 0)
        return mOverride;
    float result;
    mCurve.Evaluate(0.5f, result);
    if (mOverride != -9999.9f)
    {
        result = mOverride;
    }
    else if (mpSkillTweaks != 0 && mModifier != 0)
    {
        float skill = -9999.9f;
        float weightedSkill = 0.0f;
        float totalWeight = 0.0f;
        switch (mModifier)
        {
        case 1:
            skill = mpSkillTweaks->mDifficulty[0];
            break;
        case 2:
            skill = mpSkillTweaks->mDifficulty[1];
            break;
        case 3:
            skill = mpSkillTweaks->mDifficulty[2];
            break;
        case 4:
            skill = mpSkillTweaks->mDifficulty[3];
            break;
        case 5:
            skill = mpSkillTweaks->mDifficulty[4];
            break;
        case 6:
            skill = mpSkillTweaks->mDifficulty[5];
            break;
        case 7:
            skill = mpSkillTweaks->mDifficulty[6];
            break;
        case 8:
            skill = mpSkillTweaks->mDifficulty[7];
            break;
        case 9:
            skill = mpSkillTweaks->mDifficulty[8];
            break;
        case 10:
            skill = mpSkillTweaks->mDifficulty[9];
            break;
        case 11:
            skill = mpSkillTweaks->mDifficulty[10];
            break;
        case 12:
            skill = mpSkillTweaks->mDifficulty[11];
            break;
        case 13:
            skill = mpSkillTweaks->mDifficulty[12];
            break;
        case 14:
            skill = mpSkillTweaks->mDifficulty[13];
            break;
        }
        weightedSkill += skill * (1.0f - mpSkillTweaks->mCharacterWeight);
        totalWeight += 1.0f - mpSkillTweaks->mCharacterWeight;
        if (skill == 0.0f)
        {
            result = skill;
        }
        else
        {
            if (mpSkillTweaks->mpCurrentPlayer != 0 && sUseCharacterDifficulty && mModifier < 10)
            {
                float characterSkill = ((cFielder*)mpSkillTweaks->mpCurrentPlayer)->GetTweaks()->GetSkillRating(mModifier);
                weightedSkill += characterSkill * mpSkillTweaks->mCharacterWeight;
                totalWeight += mpSkillTweaks->mCharacterWeight;
            }
            float x = 0.0f;
            if (totalWeight > 0.0f)
                x = weightedSkill / totalWeight;
            x = x >= 0.0f ? x : 0.0f;
            x = x <= 1.0f ? x : 1.0f;
            mCurve.Evaluate(x, result);
        }
    }
    return result;
}

static const char* g_sDifficultyFileNames[] = {
    "DifficultyBraindead.ini",
    "Difficulty0.ini",
    "Difficulty1.ini",
    "Difficulty2.ini",
    "Difficulty3.ini",
    "Difficulty4.ini",
    "DifficultySuperhuman.ini",
    "DifficultyHuman.ini",
    "DifficultyBaseline.ini",
};

SkillTweaks::SkillTweaks(const char* category)
    : mCategory(category)
    , mSkillTweaksList(16, 16)
    , mpCurrentPlayer(0)
{
    float defaultValue = 1.234f;
    fShotValue1.BindWithDefault("Shoot/Shot Value 1", defaultValue, mCategory, false, 0.0f, 1.0f, 0.05f);
    fShotValue2.BindWithDefault("Shoot/Shot Value 2", defaultValue, mCategory, false, 0.0f, 1.0f, 0.05f);
    fShotValue3.BindWithDefault("Shoot/Shot Value 3", defaultValue, mCategory, false, 0.0f, 1.0f, 0.05f);
    fShotChance0.BindWithDefault("Shoot/Shot Chance 0", defaultValue, mCategory, false, 0.0f, 1.0f, 0.05f);
    fShotChance1.BindWithDefault("Shoot/Shot Chance 1", defaultValue, mCategory, false, 0.0f, 1.0f, 0.05f);
    fShotChance2.BindWithDefault("Shoot/Shot Chance 2", defaultValue, mCategory, false, 0.0f, 1.0f, 0.05f);
    fShotChance3.BindWithDefault("Shoot/Shot Chance 3", defaultValue, mCategory, false, 0.0f, 1.0f, 0.05f);
    fShotChance4.BindWithDefault("Shoot/Shot Chance 4", defaultValue, mCategory, false, 0.0f, 1.0f, 0.05f);
    fAttackCarrierDistance.BindWithDefault("Goalie/Attack Carrier Range", defaultValue, mCategory, false, 0.0f, 1.0f, 0.05f);
    fLooseBallChaseDistance.BindWithDefault("Goalie/Loose Ball Range", defaultValue, mCategory, false, 0.0f, 1.0f, 0.05f);
    fGoalieCanInterceptPass.BindWithDefault("Goalie/Goalie Intercept Pass", defaultValue, mCategory, false, 0.0f, 1.0f, 0.05f);
    fGoalieDekeChance.BindWithDefault("Goalie/Goalie Deke Chance", defaultValue, mCategory, false, 0.0f, 1.0f, 0.05f);
    fGoalieDekeSpeed.BindWithDefault("Goalie/Goalie Deke Speed", defaultValue, mCategory, false, 0.0f, 1.0f, 0.05f);
}

SkillTweaks::~SkillTweaks()
{
    mSkillTweaksList.DeleteValues();
    mSkillTweaksList.GetAllocator()->FreeBlocks();
    if (sSkillTweaksLoaded)
    {
        sDefaultSkillTweaks.DeleteValues();
        sDefaultSkillTweaks.GetAllocator()->FreeBlocks();
        nlDeleteList(sSkillTweakModifiers);
        sSkillTweaksLoaded = false;
    }
}

void SkillTweaks::CreateTweaks(bool reload)
{
    if (reload && sSkillTweaksLoaded)
    {
        sDefaultSkillTweaks.DeleteValues();
        sDefaultSkillTweaks.GetAllocator()->FreeBlocks();
        nlDeleteList(sSkillTweakModifiers);
        sSkillTweaksLoaded = false;
    }
    if (!sSkillTweaksLoaded)
        InitializeSkillTweakModifiers();
    mSkillTweaksList.DeleteValues();
    mSkillTweaksList.GetAllocator()->FreeBlocks();
    Decision_Choice[0] = AddTweak("Decision/Choice 1st");
    Decision_Choice[1] = AddTweak("Decision/Choice 2nd");
    Decision_Choice[2] = AddTweak("Decision/Choice 3rd");
    Decision_Choice[3] = AddTweak("Decision/Choice 4th");
    AddTweak("Teamplays/Protect Captain");
    AddTweak("Teamplays/Charge Ball");
    Off_Avoidance = AddTweak("Off/Avoidance Effectiveness");
    Off_Reaction = AddTweak("Off/Reaction");
    AddTweak("Off/Deke");
    AddTweak("Off/Ground Pass");
    AddTweak("Off/Volley Pass");
    AddTweak("Off/Shooting");
    AddTweak("Off/Chip Shot");
    AddTweak("Off/Ground One Timer");
    AddTweak("Off/Volley One Timer");
    AddTweak("Off/One Touch Ground Pass");
    AddTweak("Off/One Touch Volley Pass");
    AddTweak("Off/Pass Receive Hit");
    AddTweak("Off/Pass Receive Slide");
    AddTweak("Off/Pass Receive Powerup");
    AddTweak("Off/Cut And Break");
    AddTweak("Off/Turbo With Ball");
    AddTweak("Windup/Deke");
    AddTweak("Windup/Ground Pass");
    AddTweak("Windup/Volley Pass");
    AddTweak("Windup/Powerup");
    AddTweak("Windup/Random Charge");
    AddTweak("Windup/Skillshot");
    AddTweak("Windup/Megastrike");
    for (int i = 3; i <= 6; ++i)
    {
        char sTweakName[0x40];
        nlSNPrintf(sTweakName, 0x3F, "Windup/Mega %d Goal Chance", i);
        MegaGoalChance[i - 3] = AddTweak(sTweakName);
        nlSNPrintf(sTweakName, 0x3F, "Windup/Mega %d Goal Accuracy", i);
        MegaGoalAccuracy[i - 3] = AddTweak(sTweakName);
    }
    AddTweak("Def/Block Pass");
    AddTweak("Def/Block Shot");
    Def_SlideAttackChance = AddTweak("Def/Slide Attack");
    AddTweak("Def/Heavy Attack");
    AddTweak("Def/Volley Pass Defend");
    Def_Marking = AddTweak("Def/Marking Effectiveness");
    Def_Reaction = AddTweak("Def/Reaction");
    AddTweak("Loose/Heavy Attack");
    AddTweak("Loose/Slide Attack");
    AddTweak("Loose/Shot");
    AddTweak("Loose/Chip Shot");
    AddTweak("Loose/Ground Pass");
    AddTweak("Loose/Volley Pass");
    Loose_Reaction = AddTweak("Loose/Reaction");
    for (int i_sit = 0; i_sit < 3; ++i_sit)
    {
        const char* sSituationName;
        switch (i_sit)
        {
        case 1:
            sSituationName = "Def";
            break;
        case 0:
            sSituationName = "Off";
            break;
        case 2:
            sSituationName = "Loose";
            break;
        }
        for (int i_powerup = 0; i_powerup < 10; ++i_powerup)
        {
            char sTweakName[0x40];
            nlSNPrintf(sTweakName, 0x3F, "%s/Powerup/%s", sSituationName, GetPowerupName(i_powerup, false));
            PowerupUsageChance[i_sit][i_powerup] = AddTweak(sTweakName);
        }
    }
    if (!sSkillTweaksLoaded)
    {
        Config curves(Config::ALLOCATE_HIGH, 0x1400, 0x400);
        Config modifiers(Config::ALLOCATE_HIGH, 0x1400, 0x400);
        curves.LoadFromFile("ini/DifficultyLevels/ChanceCurves.ini");
        modifiers.LoadFromFile("ini/DifficultyLevels/MetaMap.ini");
        SkillTweakLoader loader(&curves, &modifiers);
        sDefaultSkillTweaks.Walk(&loader, &SkillTweakLoader::Load);
        sMinimumCharacterDifficulty = curves.Get<float>("Decision/Minimum CharDiff", -9999.9f);
        sMaximumCharacterDifficulty = curves.Get<float>("Decision/Maximum CharDiff", -9999.9f);
    }
    sSkillTweaksLoaded = true;
}

SkillTweakLoader::SkillTweakLoader(Config* curves, Config* modifiers)
    : mCurves(curves)
    , mModifiers(modifiers)
{
}

void SkillTweakLoader::Load(const unsigned long& key, SkillTweak** value)
{
    Config* modifiers = mModifiers;
    Config* curves = mCurves;
    SkillTweak* tweak = *value;
    tweak->mOverride = -9999.9f;
    Config::String text = curves->Get<Config::String>(tweak->mNameInFile, Config::String("0.12345"));
    tweak->ParseCurve(text.c_str(), text.size());
    if (modifiers->Exists(tweak->mNameInFile))
    {
        const char* name = modifiers->Get<const char*>(tweak->mNameInFile, "None");
        unsigned long hash = nlStringLowerHash(name);
        SkillTweakModifier* modifier = sSkillTweakModifiers.m_pStart;
        unsigned long id = 0;
        while (modifier != 0)
        {
            if (hash == modifier->mHash)
            {
                id = modifier->mModifier;
                break;
            }
            modifier = modifier->next;
        }
        tweak->mModifier = id;
    }
}

void SkillTweaks::Init(int difficulty, bool blend, bool reload)
{
    CreateTweaks(reload);
    Config cfg(Config::ALLOCATE_HIGH, 0x1400, 0x400);
    nlSNPrintf(mszFileName, 0x3F, "ini/DifficultyLevels/%s", g_sDifficultyFileNames[difficulty]);
    cfg.LoadFromFile(mszFileName);
    mDifficulty[0] = cfg.Get<float>("Difficulty/Speed", -9999.9f);
    mDifficulty[1] = cfg.Get<float>("Difficulty/Slide Tackle", -9999.9f);
    mDifficulty[2] = cfg.Get<float>("Difficulty/Hitting", -9999.9f);
    mDifficulty[3] = cfg.Get<float>("Difficulty/Shooting", -9999.9f);
    mDifficulty[4] = cfg.Get<float>("Difficulty/Passing", -9999.9f);
    mDifficulty[5] = cfg.Get<float>("Difficulty/Defensive", -9999.9f);
    mDifficulty[6] = cfg.Get<float>("Difficulty/Offensive", -9999.9f);
    mDifficulty[7] = cfg.Get<float>("Difficulty/Playmaker", -9999.9f);
    mDifficulty[8] = cfg.Get<float>("Difficulty/Powerplayer", -9999.9f);
    mDifficulty[9] = cfg.Get<float>("Difficulty/ProjectilePowerup", -9999.9f);
    mDifficulty[10] = cfg.Get<float>("Difficulty/CatchupPowerup", -9999.9f);
    mDifficulty[11] = cfg.Get<float>("Difficulty/ModifierPowerup", -9999.9f);
    mDifficulty[12] = cfg.Get<float>("Difficulty/SpecialPowerup", -9999.9f);
    mDifficulty[13] = cfg.Get<float>("Difficulty/NoCharacterDiff", -9999.9f);
    SkillTweakCopier copier(this, &cfg);
    sDefaultSkillTweaks.Walk(&copier, &SkillTweakCopier::Copy);
    fShotValue1 = cfg.Get<float>("Shoot/Shot Value 1", -9999.9f);
    fShotValue2 = cfg.Get<float>("Shoot/Shot Value 2", -9999.9f);
    fShotValue3 = cfg.Get<float>("Shoot/Shot Value 3", -9999.9f);
    fShotChance0 = cfg.Get<float>("Shoot/Shot Chance 0", -9999.9f);
    fShotChance1 = cfg.Get<float>("Shoot/Shot Chance 1", -9999.9f);
    fShotChance2 = cfg.Get<float>("Shoot/Shot Chance 2", -9999.9f);
    fShotChance3 = cfg.Get<float>("Shoot/Shot Chance 3", -9999.9f);
    fShotChance4 = cfg.Get<float>("Shoot/Shot Chance 4", -9999.9f);
    fAttackCarrierDistance = cfg.Get<float>("Goalie/Attack Carrier Range", -9999.9f);
    fLooseBallChaseDistance = cfg.Get<float>("Goalie/Loose Ball Range", -9999.9f);
    fGoalieCanInterceptPass = cfg.Get<float>("Goalie/Goalie Intercept Pass", -9999.9f);
    fGoalieDekeChance = cfg.Get<float>("Goalie/Goalie Deke Chance", -9999.9f);
    fGoalieDekeSpeed = cfg.Get<float>("Goalie/Goalie Deke Speed", -9999.9f);
    mDecisionWeights[0] = -1.0f;
    mCharacterWeight = Interpolate(sMinimumCharacterDifficulty, sMaximumCharacterDifficulty, (float)(nlMin(difficulty, 5) - 1) * 0.25f);
}

SkillTweakCopier::SkillTweakCopier(SkillTweaks* tweaks, Config* config)
    : mTweaks(tweaks)
    , mConfig(config)
{
}

void SkillTweakCopier::Copy(const unsigned long& key, SkillTweak** value)
{
    SkillTweak* tweak;
    SkillTweak** foundValue;
    bool found = mTweaks->mSkillTweaksList.FindGet((*value)->mHash, &foundValue);
    if (found)
        tweak = *foundValue;
    if (found)
    {
        *tweak = **value;
        tweak->mpSkillTweaks = mTweaks;
        tweak->mOwnsCurve = false;
        {
            char name[0x40];
            Config* cfg = mConfig;
            nlSNPrintf(name, 0x3F, "Override/%s", tweak->mNameInFile);
            tweak->mOverride = -9999.9f;
            if (cfg->Exists(name))
                tweak->mOverride = cfg->Get<float>(name, -9999.9f);
        }
    }
}

SkillTweak* SkillTweaks::AddTweak(const char* name)
{
    SkillTweak* tweak = 0;
    SkillTweak* defaultTweak = 0;
    if (!sSkillTweaksLoaded)
    {
        defaultTweak = new (8, false) SkillTweak(name);
        sDefaultSkillTweaks.Add(defaultTweak->mHash, defaultTweak);
    }
    else
    {
        SkillTweak** found;
        if (sDefaultSkillTweaks.FindGet(nlStringLowerHash(name), &found))
            defaultTweak = *found;
    }
    if (defaultTweak != 0)
    {
        tweak = new (8, false) SkillTweak(*defaultTweak);
        mSkillTweaksList.Add(tweak->mHash, tweak);
    }
    return tweak;
}

static void InitializeSkillTweakModifiers()
{
    nlListAddEnd(&sSkillTweakModifiers.m_pStart, &sSkillTweakModifiers.m_pEnd, new (8, false) SkillTweakModifier(0, "None"));
    nlListAddEnd(&sSkillTweakModifiers.m_pStart, &sSkillTweakModifiers.m_pEnd, new (8, false) SkillTweakModifier(1, "Speed"));
    nlListAddEnd(&sSkillTweakModifiers.m_pStart, &sSkillTweakModifiers.m_pEnd, new (8, false) SkillTweakModifier(2, "Slide Tackle"));
    nlListAddEnd(&sSkillTweakModifiers.m_pStart, &sSkillTweakModifiers.m_pEnd, new (8, false) SkillTweakModifier(3, "Hitting"));
    nlListAddEnd(&sSkillTweakModifiers.m_pStart, &sSkillTweakModifiers.m_pEnd, new (8, false) SkillTweakModifier(4, "Shooting"));
    nlListAddEnd(&sSkillTweakModifiers.m_pStart, &sSkillTweakModifiers.m_pEnd, new (8, false) SkillTweakModifier(5, "Passing"));
    nlListAddEnd(&sSkillTweakModifiers.m_pStart, &sSkillTweakModifiers.m_pEnd, new (8, false) SkillTweakModifier(6, "Defensive"));
    nlListAddEnd(&sSkillTweakModifiers.m_pStart, &sSkillTweakModifiers.m_pEnd, new (8, false) SkillTweakModifier(7, "Offensive"));
    nlListAddEnd(&sSkillTweakModifiers.m_pStart, &sSkillTweakModifiers.m_pEnd, new (8, false) SkillTweakModifier(8, "Playmaker"));
    nlListAddEnd(&sSkillTweakModifiers.m_pStart, &sSkillTweakModifiers.m_pEnd, new (8, false) SkillTweakModifier(9, "Powerplayer"));
    nlListAddEnd(&sSkillTweakModifiers.m_pStart, &sSkillTweakModifiers.m_pEnd, new (8, false) SkillTweakModifier(10, "ProjectilePowerup"));
    nlListAddEnd(&sSkillTweakModifiers.m_pStart, &sSkillTweakModifiers.m_pEnd, new (8, false) SkillTweakModifier(11, "CatchupPowerup"));
    nlListAddEnd(&sSkillTweakModifiers.m_pStart, &sSkillTweakModifiers.m_pEnd, new (8, false) SkillTweakModifier(12, "ModifierPowerup"));
    nlListAddEnd(&sSkillTweakModifiers.m_pStart, &sSkillTweakModifiers.m_pEnd, new (8, false) SkillTweakModifier(13, "SpecialPowerup"));
    nlListAddEnd(&sSkillTweakModifiers.m_pStart, &sSkillTweakModifiers.m_pEnd, new (8, false) SkillTweakModifier(14, "NoCharacterDiff"));
}

/**
 * Offset/Address/Size: 0x2ABC | 0x800A327C | size: 0xD4
 */
float SkillTweaks::GetSkillValue(unsigned long param1, cPlayer* param2)
{
    float result = -9999.9f;
    GetSkillValue(param1, &result, param2);
    return result;
}

/**
 * Offset/Address/Size: 0x2B90 | 0x800A3350 | size: 0xB4
 */
bool SkillTweaks::GetSkillValue(
    unsigned long param1, float* param2, cPlayer* param3)
{
    cPlayer* pPreviousPlayer = mpCurrentPlayer;
    if (param3 != NULL)
    {
        mpCurrentPlayer = param3;
    }

    bool found = false;
    SkillTweak* pSkillTweak;
    SkillTweak** ppSkillTweak;
    bool tweakFound = mSkillTweaksList.FindGet(param1, &ppSkillTweak);
    if (tweakFound)
    {
        pSkillTweak = *ppSkillTweak;
    }
    if (tweakFound)
    {
        *param2 = pSkillTweak->GetValue();
        found = true;
    }

    if (param3 != NULL)
    {
        mpCurrentPlayer = pPreviousPlayer;
    }
    return found;
}

/**
 * Offset/Address/Size: 0x2C44 | 0x800A3404 | size: 0x70
 */
float* SkillTweaks::GetDecisionWeights()
{
    if (mDecisionWeights[0] == -1.0f)
    {
        for (int i = 0; i < 4; ++i)
        {
            mDecisionWeights[i] = Decision_Choice[i]->GetValue();
        }
    }
    return mDecisionWeights;
}

/**
 * Offset/Address/Size: 0x2CB4 | 0x800A3474 | size: 0x60
 */
float SkillTweaks::GetReaction(cPlayer* param1)
{
    if (param1 == NULL)
    {
        param1 = mpCurrentPlayer;
    }

    float result = 0.0f;
    if (param1 == NULL)
    {
        return result;
    }

    switch (param1->m_pTeam->mpCurrentSituation)
    {
    case 1:
        return Def_Reaction->GetValue();
    case 0:
        return Off_Reaction->GetValue();
    case 2:
        return Loose_Reaction->GetValue();
    }
    return result;
}
