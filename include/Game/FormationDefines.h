#ifndef _FORMATIONDEFINES_H_
#define _FORMATIONDEFINES_H_

#include "Game/Team.h"
#include "NL/nlMath.h"

void AILocToFieldLoc(nlVector3& result, const nlVector3& input, eTeamSide side);
void FieldLocToAILoc(nlVector3& dest, const nlVector3& field_location, eTeamSide nTeamSide);

class FormationPos
{
public:
    FormationPos()
    {
        m_Location.x = 0.0f;
        m_Location.y = 0.0f;
        m_CaptainPreference = 0.0f;
    }

    void GetLocationForTeam(nlVector2& dest, int teamId) const;

    /* 0x00 */ nlVector2 m_Location;
    /* 0x08 */ float m_CaptainPreference;
};

class FormationSpec
{
public:
    FormationSpec();

    void Init(int id, int iKeyIndex, const char* name, bool field_0x04);
    void SetName(const char* name);
    nlVector2& GetKeyLocation() const;
    void CalculateExtents(nlVector2& minOut, nlVector2& maxOut, const nlVector2& input) const;

    /* 0x00 */ u32 m_ID;
    /* 0x04 */ bool field_0x04;
    /* 0x08 */ s32 m_iKeyIndex;
    /* 0x0C */ f32 m_InRadius;
    /* 0x10 */ f32 m_OutRadius;
    /* 0x14 */ FormationPos m_Positions[4];
    /* 0x44 */ nlVector2 m_v2Min;
    /* 0x4C */ nlVector2 m_v2Max;
    /* 0x54 */ nlVector2 m_v2Center;
    /* 0x5C */ char m_Name[32];
};

class FormationSet
{
public:
    FormationSet();

    ~FormationSet();

    void Init(int id, FormationSpec* formationArray, int numFormations, bool bCreateCopy, const char* name);
    FormationSpec* GetFormationSpec(int index) const;
    FormationSpec* GetFormationSpecFromID(int formationID) const;
    static FormationSet* LoadFormationSets(const char* filename, int& out_numsets);

    /* 0x00 */ bool m_AutoDelete;
    /* 0x01 */ u8 field_0x01[3];
    /* 0x04 */ int m_ID;
    /* 0x08 */ int m_NumFormationDefs;
    /* 0x0C */ FormationSpec* m_FormationDefArray;
    /* 0x10 */ char field_0x10[0x20];
};

#endif // _FORMATIONDEFINES_H_
