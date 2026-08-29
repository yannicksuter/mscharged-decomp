#include "Game/FormationDefines.h"

#include "Game/AI/AiUtil.h"
#include "NL/nlConfig.h"
#include "NL/nlMemory.h"
#include "NL/nlPrint.h"
#include "NL/nlString.h"
#include "math.h"

static const nlVector3 v3Zero = { 0.0f, 0.0f, 0.0f };

void FieldLocToAILoc(nlVector2& dest, const nlVector2& field_location, eTeamSide nTeamSide)
{
    float fMinFromX, fMaxFromX, fMinFromY, fMaxFromY, fMinToX, fMaxToX, fMinToY, fMaxToY;
    fMinFromX = -20.6f;
    fMaxFromX = 20.6f;
    fMinFromY = -12.5f;
    fMaxFromY = 12.5f;
    fMinToX = 0.0f;
    fMaxToX = 4.0f;
    fMinToY = 1.0f;
    fMaxToY = -1.0f;

    if (nTeamSide == AWAY)
    {
        fMinFromX = -fMinFromX;
        fMaxFromX = -fMaxFromX;
        fMinFromY = -fMinFromY;
        fMaxFromY = -fMaxFromY;
    }

    float xPercent = (field_location.x - fMinFromX) / (fMaxFromX - fMinFromX);
    if (xPercent > 1.0f)
        xPercent = 1.0f;
    if (xPercent < 0.0f)
        xPercent = 0.0f;
    dest.x = fMinToX + xPercent * (fMaxToX - fMinToX);

    float yPercent = (field_location.y - fMinFromY) / (fMaxFromY - fMinFromY);
    if (yPercent > 1.0f)
        yPercent = 1.0f;
    if (yPercent < 0.0f)
        yPercent = 0.0f;
    dest.y = fMinToY + yPercent * (fMaxToY - fMinToY);
}

void AILocToFieldLoc(nlVector2& dest, const nlVector2& ai_location, eTeamSide nTeamSide)
{
    float fMinFromX, fMaxFromX, fMinFromY, fMaxFromY, fMinToX, fMaxToX, fMinToY, fMaxToY;
    fMinFromX = 0.0f;
    fMaxFromX = 4.0f;
    fMinFromY = 1.0f;
    fMaxFromY = -1.0f;
    fMinToX = -20.6f;
    fMaxToX = 20.6f;
    fMinToY = -12.5f;
    fMaxToY = 12.5f;

    if (nTeamSide == AWAY)
    {
        fMinFromX = fMaxFromX;
        fMaxFromX = 0.0f;
        fMinFromY = fMaxFromY;
        fMaxFromY = 1.0f;
    }

    float xPercent = (ai_location.x - fMinFromX) / (fMaxFromX - fMinFromX);
    if (xPercent > 1.0f)
        xPercent = 1.0f;
    if (xPercent < 0.0f)
        xPercent = 0.0f;
    dest.x = fMinToX + xPercent * (fMaxToX - fMinToX);

    float yPercent = (ai_location.y - fMinFromY) / (fMaxFromY - fMinFromY);
    if (yPercent > 1.0f)
        yPercent = 1.0f;
    if (yPercent < 0.0f)
        yPercent = 0.0f;
    dest.y = fMinToY + yPercent * (fMaxToY - fMinToY);
}

void FieldLocToAILoc(nlVector3& dest, const nlVector3& field_location, eTeamSide nTeamSide)
{
    FieldLocToAILoc((nlVector2&)dest, (const nlVector2&)field_location, nTeamSide);
    dest.z = 0.0f;
}

void FormationPos::GetLocationForTeam(nlVector2& dest, int teamId) const
{
    if (teamId == 0)
    {
        dest = m_Location;
        return;
    }
    nlVec2Set(dest, -m_Location.x, -m_Location.y);
}

nlVector2& FormationSpec::GetKeyLocation() const
{
    if (m_iKeyIndex >= 0 && m_iKeyIndex < 4)
    {
        return const_cast<nlVector2&>(m_Positions[m_iKeyIndex].m_Location);
    }
    return *(nlVector2*)&const_cast<nlVector3&>(v3Zero);
}

void FormationSpec::CalculateExtents(nlVector2& minOut, nlVector2& maxOut, const nlVector2& input) const
{
    const float fieldHalfWidth = 18.539999f;
    const float fieldHalfHeight = 11.25f;

    minOut.x = -fieldHalfWidth + (input.x - m_v2Min.x);
    maxOut.x = fieldHalfWidth + (input.x - m_v2Max.x);
    minOut.y = -fieldHalfHeight + (input.y - m_v2Min.y);
    maxOut.y = fieldHalfHeight + (input.y - m_v2Max.y);
}

static inline float FormationMin(float current, float value)
{
    if (current <= value)
        return current;
    return value;
}

static inline float FormationMax(float current, float value)
{
    if (current >= value)
        return current;
    return value;
}

void FormationSpec::Init(int id, int iKeyIndex, const char* name, bool field_0x04)
{
    m_ID = id;
    this->field_0x04 = field_0x04;
    m_iKeyIndex = iKeyIndex;
    if (name != 0)
    {
        nlStrNCpy(m_Name, name, 32);
    }
    else
    {
        m_Name[0] = 0;
    }
    m_v2Min.x = 999999.9f;
    m_v2Min.y = 999999.9f;
    m_v2Max.x = -999999.9f;
    m_v2Max.y = -999999.9f;
    m_v2Center.x = 0.0f;
    m_v2Center.y = 0.0f;

    FormationPos* position = m_Positions;
    for (int i_fielder = 0; i_fielder < 4; i_fielder++, position++)
    {
        m_v2Min.x = FormationMin(m_v2Min.x, position->m_Location.x);
        m_v2Min.y = FormationMin(m_v2Min.y, position->m_Location.y);
        m_v2Max.x = FormationMax(m_v2Max.x, position->m_Location.x);
        m_v2Max.y = FormationMax(m_v2Max.y, position->m_Location.y);
        {
            float cy = m_v2Center.y + position->m_Location.y;
            float cx = m_v2Center.x + position->m_Location.x;
            m_v2Center.y = cy;
            m_v2Center.x = cx;
        }
    }

    {
        float cy = m_v2Center.y * 0.25f;
        float cx = m_v2Center.x * 0.25f;
        m_v2Center.y = cy;
        m_v2Center.x = cx;
    }
}

FormationSpec* FormationSet::GetFormationSpec(int index) const
{
    return &m_FormationDefArray[index];
}

FormationSpec* FormationSet::GetFormationSpecFromID(int formationID) const
{
    int i = 0;
    int count = m_NumFormationDefs;
    for (; count > 0; count--)
    {
        if ((unsigned int)formationID == m_FormationDefArray[i].m_ID)
        {
            return &m_FormationDefArray[i];
        }
        i++;
    }
    return 0;
}

void FormationSet::Init(int id, FormationSpec* formationArray, int numFormations, bool bCreateCopy, const char* name)
{
    m_ID = id;
    m_NumFormationDefs = numFormations;

    if (name != 0)
    {
        nlStrNCpy(field_0x10, name, 32);
    }
    else
    {
        field_0x10[0] = 0;
    }

    if (bCreateCopy)
    {
        m_AutoDelete = true;
        m_FormationDefArray = new (8, false) FormationSpec[numFormations];
        for (int i = 0; i < numFormations; i++)
        {
            m_FormationDefArray[i] = formationArray[i];
        }
    }
    else
    {
        m_AutoDelete = false;
        m_FormationDefArray = formationArray;
    }
}

FormationSpec::FormationSpec()
{
    m_ID = -1;
    field_0x04 = true;
}

FormationSet::FormationSet()
{
    m_ID = -1;
    m_NumFormationDefs = 0;
    m_FormationDefArray = 0;
    m_AutoDelete = false;
}

FormationSet* FormationSet::LoadFormationSets(const char* filename, int& out_numsets)
{
    Config config(Config::ALLOCATE_HIGH, 0xC800, 0xC00);
    config.LoadFromFile(filename);

    out_numsets = GetConfigInt(config, "Number Of Formation Sets", 0);
    if (out_numsets == 0)
    {
        return 0;
    }

    FormationSet* setList = new (8, false) FormationSet[out_numsets];
    char section_name[128];
    char var_name[128];
    FormationSpec formationList[42];
    int i_set;
    int i_formation;
    int i_pos;

    for (i_set = 0; i_set < out_numsets; i_set++)
    {
        nlSNPrintf(section_name, 127, "FORMATION_SET%d", i_set);
        nlSNPrintf(var_name, 127, "%s/Name", section_name);
        const char* setName = config.Get<const char*>(var_name, "Unnamed");

        i_formation = 0;
        while (true)
        {
            nlSNPrintf(var_name, 127, "%s/F%d_NAME", section_name, i_formation);
            if (!config.Exists(var_name))
            {
                break;
            }

            FormationSpec& formation = formationList[i_formation];
            Config::String formationName = config.Get<Config::String>(var_name, Config::String("Unnamed"));

            nlSNPrintf(var_name, 127, "%s/F%d_KEY_POS", section_name, i_formation);
            int keyIndex = GetConfigInt(config, var_name, -1);

            nlSNPrintf(var_name, 127, "%s/F%d_ENABLED", section_name, i_formation);
            bool enabled = GetConfigBool(config, var_name, true);

            nlSNPrintf(var_name, 127, "%s/F%d_RADIUS_IN", section_name, i_formation);
            float inRadius = GetConfigFloat(config, var_name, 0.58f);

            nlSNPrintf(var_name, 127, "%s/F%d_RADIUS_OUT", section_name, i_formation);
            float outRadius = GetConfigFloat(config, var_name, 0.91f);

            float aiRadiusRange = (float)fabs(-1.0f - 1.0f);
            inRadius = InterpolateRange(0.0f, 25.0f, 0.0f, aiRadiusRange, inRadius);
            outRadius = InterpolateRange(0.0f, 25.0f, 0.0f, aiRadiusRange, outRadius);
            formation.m_InRadius = inRadius;
            formation.m_OutRadius = outRadius;

            for (i_pos = 0; i_pos < 4; i_pos++)
            {
                FormationPos& position = formation.m_Positions[i_pos];

                nlSNPrintf(var_name, 127, "%s/F%d_P%d_X", section_name, i_formation, i_pos);
                float xVal = GetConfigFloat(config, var_name, -9999.9f);

                nlSNPrintf(var_name, 127, "%s/F%d_P%d_Y", section_name, i_formation, i_pos);
                float yVal = GetConfigFloat(config, var_name, -9999.9f);

                nlVector2 ailocation = { xVal, yVal };
                nlVector2 fieldLocation;
                AILocToFieldLoc(fieldLocation, ailocation, HOME);

                position.m_Location = fieldLocation;
                position.m_CaptainPreference = 0.0f;
            }

            const char* formationNameString = formationName.c_str();
            int formationID = nlStringHash(formationName.c_str());
            formation.Init(formationID, keyIndex, formationNameString, enabled);
            i_formation++;
        }

        setList[i_set].Init(nlStringHash(setName), formationList, i_formation, true, setName);
    }

    return setList;
}
