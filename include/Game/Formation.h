#ifndef _FORMATION_H_
#define _FORMATION_H_

#include "Game/FormationDefines.h"
#include "NL/nlMath.h"
#include "NL/nlTimer.h"

class cFielder;
class cPlayer;
class cTeam;
class FormationManager;

enum eFormationType
{
    FTYPE_DEFENSIVE = 0,
    FTYPE_OFFENSIVE = 1,
    FTYPE_BALLPOSITION = 2,
    NUM_FORMATION_TYPES = 3,
};

enum eFormationSet
{
    FSET_NONE = -1,
    FSET_OFFENSIVE_DEFENSIVE = 0,
    FSET_BALL_PASSIVE = 1,
    FSET_BALL_MODERATE = 2,
    FSET_BALL_AGGRESIVE = 3,
    NUM_FORMATION_SETS = 4,
};

enum eFormation
{
    FORMATION_NONE = -1,
    FORMATION_OFF_DEF_KICKOFF_NEUTRAL = 0,
    FORMATION_OFF_DEF_KICKOFF_ADVANTAGE = 1,
    FORMATION_OFF_DEF_31 = 2,
    FORMATION_OFF_DEF_121_DIAMOND = 3,
    FORMATION_OFF_DEF_22_SQUARE = 4,
    FORMATION_OFF_DEF_22_TRAPEZE = 5,
    FORMATION_BAL_PAS_OFFENSIVE_RIGHT = 6,
    FORMATION_BAL_PAS_OFFENSIVE_LEFT = 7,
    FORMATION_BAL_PAS_OFFENSIVE_CENTER = 8,
    FORMATION_BAL_PAS_DEFENSIVE_RIGHT = 9,
    FORMATION_BAL_PAS_DEFENSIVE_CENTER = 10,
    FORMATION_BAL_PAS_DEFENSIVE_LEFT = 11,
    FORMATION_BAL_PAS_CENTER_RIGHT = 12,
    FORMATION_BAL_PAS_CENTER_LEFT = 13,
    FORMATION_BAL_PAS_MIDOFFENSIVE_RIGHT = 14,
    FORMATION_BAL_PAS_MIDOFFENSIVE_LEFT = 15,
    FORMATION_BAL_PAS_MIDOFFENSIVE_CENTRE = 16,
    FORMATION_BAL_MOD_OFFENSIVE_RIGHT = 17,
    FORMATION_BAL_MOD_OFFENSIVE_LEFT = 18,
    FORMATION_BAL_MOD_OFFENSIVE_CENTER = 19,
    FORMATION_BAL_MOD_DEFENSIVE_RIGHT = 20,
    FORMATION_BAL_MOD_DEFENSIVE_CENTER = 21,
    FORMATION_BAL_MOD_DEFENSIVE_LEFT = 22,
    FORMATION_BAL_MOD_CENTER_LEFT = 23,
    FORMATION_BAL_MOD_CENTER_RIGHT = 24,
    FORMATION_BAL_MOD_MIDOFFENSIVE_RIGHT = 25,
    FORMATION_BAL_MOD_MIDOFFENSIVE_LEFT = 26,
    FORMATION_BAL_MOD_MIDOFFENSIVE_CENTRE = 27,
    FORMATION_BAL_MOD_CENTER = 28,
    FORMATION_BAL_AGG_OFFENSIVE_RIGHT = 29,
    FORMATION_BAL_AGG_OFFENSIVE_LEFT = 30,
    FORMATION_BAL_AGG_OFFENSIVE_CENTER = 31,
    FORMATION_BAL_AGG_DEFENSIVE_RIGHT = 32,
    FORMATION_BAL_AGG_DEFENSIVE_CENTER = 33,
    FORMATION_BAL_AGG_DEFENSIVE_LEFT = 34,
    FORMATION_BAL_AGG_CENTER_LEFT = 35,
    FORMATION_BAL_AGG_CENTER_RIGHT = 36,
    FORMATION_BAL_AGG_MIDOFFENSIVE_RIGHT = 37,
    FORMATION_BAL_AGG_MIDOFFENSIVE_LEFT = 38,
    FORMATION_BAL_AGG_MIDOFFENSIVE_CENTRE = 39,
    FORMATION_BAL_AGG_MIDDEFENSIVE_CENTRE = 40,
    NUM_FORMATIONS = 41,
};

class FormationEval
{
public:
    FormationEval();
    FormationEval(FormationManager* pMgr, eFormationType type, const FormationSpec* spec)
    {
        m_SortTimer.m_unk0 = m_SortTimer.m_uPackedTime != 0;
        m_SortTimer.m_uPackedTime = 0;
        m_pFormationManager = pMgr;
        m_pKeyPlayer = 0;
        m_pFormationSpec = spec;
        m_eFormationType = type;
        m_iFielderFormationPos[0] = 0;
        m_iFielderFormationPos[1] = 1;
        m_iFielderFormationPos[2] = 2;
        m_iFielderFormationPos[3] = 3;
    }
    static FormationEval* Create(FormationManager* pManager, eFormationType formType,
        eFormationSet formSetID, eFormation formID);
    virtual ~FormationEval() { }
    virtual void CalculateDesiredLocation(nlVector3& destPosition, cFielder* pFielder, bool bExtrapolate);
    virtual cPlayer* GetKeyPlayer();
    virtual void fn_80052978();
    virtual void GetKeyPositions(cFielder* pFielder, nlVector3& v3KeyAIPosition, nlVector3* pKeyFormationAIPosition, bool bExtrapolate);
    virtual float GetWeight(const nlVector2* v2AIBallLoc);
    virtual bool fn_80054A20();
    virtual float IsFielderInPosition(cFielder* pFielder, nlVector3 v3Pos, bool bExtended);
    virtual void SortPlayers(const nlVector2* v2Center);
    virtual void Update(float fDeltaT);

    void AssignPositionsToFielders(
        unsigned int* pFielderPosAssignments, float (*fFielderToPositionDistance)[4]);
    static void AILocToFieldLoc(
        nlVector3& dest, const nlVector3& ai_location, int nTeamID);
    static void FieldLocToAILoc(
        nlVector3& dest, const nlVector3& field_location, int nTeamID)
    {
        if (nTeamID == 0)
        {
            dest = field_location;
            return;
        }
        nlVec3Set(dest, -field_location.x, -field_location.y, 0.0f);
    }

    /* 0x04 */ FormationEval* next;
    /* 0x08 */ eFormationType m_eFormationType;
    /* 0x0C */ const FormationSpec* m_pFormationSpec;
    /* 0x10 */ cPlayer* m_pKeyPlayer;
    /* 0x14 */ FormationManager* m_pFormationManager;
    /* 0x18 */ Timer m_SortTimer;
    /* 0x20 */ unsigned int m_iFielderFormationPos[4];
};

class FormationBallPosition : public FormationEval
{
public:
    FormationBallPosition(FormationManager* pMgr, eFormationType type, const FormationSet* set)
        : FormationEval(pMgr, type, 0)
    {
        field_0x38 = 0;
        field_0x34 = 0;
        field_0x3C = 0;
        m_pFormationSet = set;
    }
    virtual ~FormationBallPosition();
    virtual void CalculateDesiredLocation(nlVector3& destPosition, cFielder* pFielder, bool bExtrapolate);
    virtual void fn_80052978();
    virtual float GetWeight(const nlVector2* v2AIBallLoc);
    virtual bool fn_80054A20();
    virtual void Update(float fDeltaT);

    bool SelectClosestBallFormations(const nlVector2& v2AIBallLoc);

    /* 0x30 */ const FormationSet* m_pFormationSet;
    /* 0x34 */ FormationEval* field_0x34;
    /* 0x38 */ FormationEval* field_0x38;
    /* 0x3C */ int field_0x3C;
};

class FormationOffensive : public FormationEval
{
public:
    FormationOffensive(FormationManager* pMgr, eFormationType type, const FormationSpec* spec)
        : FormationEval(pMgr, type, spec)
    {
    }
    virtual float GetWeight(const nlVector2* v2AIBallLoc);
    virtual float IsFielderInPosition(cFielder* pFielder, nlVector3 v3Pos, bool bExtended);
};

class FormationDefensive : public FormationEval
{
public:
    FormationDefensive(FormationManager* pMgr, eFormationType type, const FormationSpec* spec)
        : FormationEval(pMgr, type, spec)
    {
    }
    virtual void GetKeyPositions(cFielder* pFielder, nlVector3& v3KeyAIPosition,
        nlVector3* pKeyFormationAIPosition, bool bExtrapolate);
    virtual float GetWeight(const nlVector2* v2AIBallLoc);
    virtual float IsFielderInPosition(cFielder* pFielder, nlVector3 v3Pos, bool bExtended);
    virtual void Update(float fDeltaT);
};

struct CachedPosition
{
    /* 0x00 */ bool bCacheIsValid;
    /* 0x01 */ bool bInPosition;
    /* 0x02 */ u8 field_0x02[2];
    /* 0x04 */ nlVector3 vPosition;
};

class FormationManager
{
public:
    FormationManager(cTeam* pTeam);
    ~FormationManager();
    void Update(float dt);
    void ChooseNewFormations();
    void fn_80050D24();
    void fn_80051F00(nlVector2* pCenter);
    unsigned int* fn_80052034();
    bool CalculateFielderPosition(nlVector3& v3DestPosition, cFielder* pFielder,
        bool bInPosition, float fBallPosFormationWeight);
    static void LoadFormationSets();
    static void UnloadFormationSets();
    static FormationSpec* GetFormationSpec(eFormation specType);

    /* 0x00 */ cTeam* m_pTeam;
    /* 0x04 */ FormationEval* m_pFormations[3];
    /* 0x10 */ nlVector2 m_v2AIFielderCenter;
    /* 0x18 */ nlVector2 field_0x18;
    /* 0x20 */ Timer m_tSelectFormationsTimer;
    /* 0x28 */ CachedPosition m_CachedPositions[4];

    static FormationSet* m_FormationSetArray;
    static int m_NumFormationSets;
};

#endif // _FORMATION_H_
