#include "Game/Formation.h"

#include "Game/AI/AiUtil.h"
#include "Game/AI/Fielder.h"
#include "Game/AI/FuzzyVariant.h"
#include "Game/Ball.h"
#include "Game/Field.h"
#include "Game/GameTweaks.h"
#include "Game/InterpreterCore.h"
#include "Game/MathHelpers.h"
#include "Game/Player.h"
#include "Game/Team.h"
#include "NL/nlList.h"
#include "NL/nlMemory.h"
#include "NL/nlString.h"

enum eGameState
{
    GS_GAMEPLAY = 5,
    GS_OVERTIME = 6,
};

struct cGame
{
    /* 0x00 */ u8 field_0x00[0x18];
    /* 0x18 */ eGameState m_eGameState;

    bool IsGameplayOrOvertime()
    {
        return m_eGameState == GS_GAMEPLAY || m_eGameState == GS_OVERTIME;
    }
};

extern cGame* lbl_806E0C94;
#define g_pGame lbl_806E0C94

extern "C" bool fn_800344B0(cFielder*);
extern "C" unsigned int fn_800387CC(cFielder*);
extern "C" float fn_8002E1B0(cFielder*);
extern "C" float fn_800DEFD4(cFielder*);
extern "C" InterpreterCore* fn_800A695C(cTeam*);
extern "C" cPlayer* fn_800DF790(cTeam*);
extern "C" cFielder* fn_800A8800(cTeam*);
extern "C" cFielder* fn_800A8808(cTeam*);
extern "C" cFielder* fn_800A8884(cTeam*);
extern "C" void fn_800180F4(cBall*, nlVector3*, float);
extern "C" float fn_800DFF1C();
extern "C" void fn_8004F594(int, const char*, ...);
extern "C" void* memset(void*, int, unsigned long);
extern "C" UnidentifiedVariant_80054AB8* fn_80312360(InterpreterCore*, void*, int, cTeam*, int);
int nlPrintf(const char*, ...);

static const nlVector3 v3Zero = { 0.0f, 0.0f, 0.0f };
static const float lbl_804DBF28[4][4] = {
    { 5.0f, 3.0f, 1.0f, 1.0f },
    { 1.8f, 3.0f, 1.2f, 1.0f },
    { 1.0f, 1.2f, 2.5f, 1.2f },
    { 1.0f, 1.0f, 1.2f, 2.5f },
};
static const float lbl_804DBF68[4] = { 0.6f, 0.2f, 0.1f, 0.1f };
static const int lbl_804DBF78[4] = { 0, 1, 2, 3 };
static const int lbl_804DBF88[4] = { 3, 2, 1, 0 };

struct FormationPositionThresholds
{
    float fInRadius;
    float fOutUpField;
    float fOutDownField;
    float fOutLateral;
};

static const FormationPositionThresholds g_aDefensiveFormationThresholds[4] = {
    { 0.6f, 1.0f, 4.0f, 2.0f },
    { 0.6f, 1.0f, 4.0f, 2.0f },
    { 0.6f, 3.0f, 1.0f, 2.0f },
    { 0.6f, 3.0f, 1.0f, 2.0f },
};

bool lbl_806DBA20 = true;

int FormationManager::m_NumFormationSets = 0;
FormationSet* FormationManager::m_FormationSetArray = 0;

FormationManager::FormationManager(cTeam* pTeam)
    : m_pTeam(pTeam)
{
    m_pFormations[0] = 0;
    m_pFormations[1] = 0;
    m_pFormations[2] = 0;
    m_CachedPositions[0].bCacheIsValid = false;
    m_CachedPositions[1].bCacheIsValid = false;
    m_CachedPositions[2].bCacheIsValid = false;
    m_CachedPositions[3].bCacheIsValid = false;
    m_v2AIFielderCenter.x = 0.0f;
    m_v2AIFielderCenter.y = 0.0f;
    field_0x18.x = 0.0f;
    field_0x18.y = 0.0f;
    m_tSelectFormationsTimer.m_unk0 = m_tSelectFormationsTimer.m_uPackedTime != 0;
    m_tSelectFormationsTimer.m_uPackedTime = 0;
}

FormationManager::~FormationManager()
{
    s32 j;
    FormationEval** pp = (FormationEval**)this;
    for (j = 0; j < 3; j++)
    {
        FormationEval* pEval = pp[1];
        delete pEval;
        *++pp = 0;
    }
}

static inline void ClearFormationEvaluators()
{
    if (g_pTeams[0] != 0 && g_pTeams[1] != 0)
    {
        FormationEval** pp;
        cTeam** ppTeam = g_pTeams;
        s32 i;
        s32 j;
        for (i = 0; i < 2; i++)
        {
            pp = (FormationEval**)(*ppTeam)->m_pFormationManager;
            for (j = 0; j < 3; j++)
            {
                FormationEval* pEval = pp[1];
                delete pEval;
                *++pp = 0;
            }
            ppTeam++;
        }
    }
}

void FormationManager::LoadFormationSets()
{
    int numSets = 0;
    int numTries = 0;
    FormationSet* pFormationSets = 0;

    while (numSets == 0 && numTries < 5)
    {
        pFormationSets = FormationSet::LoadFormationSets("/ini/FormationSets.ini", numSets);

        if (numSets == 0)
        {
            if (m_FormationSetArray != 0)
            {
                fn_8004F594(4, "Reload of FormationSets.ini failed, but we've got some loaded already.\n");
                break;
            }

            numTries++;
            fn_8004F594(4, "WEIRD ASS BUG! Failed to reload FormationSets.ini, trying again %d\n", numTries);
        }
    }

    if (numSets == 0 && m_FormationSetArray == 0)
    {
        return;
    }

    if (g_pGame != 0)
    {
        ClearFormationEvaluators();
    }

    delete[] m_FormationSetArray;
    m_FormationSetArray = pFormationSets;
    m_NumFormationSets = numSets;

    if (g_pGame != 0 && g_pGame->IsGameplayOrOvertime())
    {
        if (g_pTeams[0] != 0 && g_pTeams[1] != 0)
        {
            s32 i_team;
            cTeam** ppTeam = g_pTeams;
            for (i_team = 0; i_team < 2; i_team++)
            {
                (*ppTeam)->m_pFormationManager->ChooseNewFormations();
                ppTeam++;
            }
        }
    }
}

static inline void ClearFormationEvals(FormationEval** pp)
{
    for (s32 j = 0; j < 3; j++)
    {
        FormationEval* pEval = pp[1];
        delete pEval;
        *++pp = 0;
    }
}

void FormationManager::UnloadFormationSets()
{
    s32 i;

    if (g_pGame != 0)
    {
        if (g_pTeams[0] != 0 && g_pTeams[1] != 0)
        {
            for (i = 0; i < 2; i++)
            {
                ClearFormationEvals(
                    (FormationEval**)g_pTeams[i]->m_pFormationManager);
            }
        }
    }

    delete[] m_FormationSetArray;
    m_FormationSetArray = 0;
    m_NumFormationSets = 0;
}

FormationSpec* FormationManager::GetFormationSpec(eFormation specType)
{
    FormationSpec* pSpec = 0;
    int i = 0;

    while (i < m_NumFormationSets)
    {
        pSpec = m_FormationSetArray[i].GetFormationSpecFromID(specType);
        if (pSpec != 0)
        {
            break;
        }
        i++;
    }

    return pSpec;
}

void FormationManager::Update(float dt)
{
    if (!g_pGame->IsGameplayOrOvertime())
    {
        return;
    }

    m_CachedPositions[0].bCacheIsValid = false;
    m_CachedPositions[1].bCacheIsValid = false;
    m_CachedPositions[2].bCacheIsValid = false;
    m_CachedPositions[3].bCacheIsValid = false;

    m_tSelectFormationsTimer.Countdown(dt, 0.0f);

    if (m_tSelectFormationsTimer.m_uPackedTime == 0 && g_pBall->GetOwnerGoalie() == 0)
    {
        ChooseNewFormations();
        m_tSelectFormationsTimer.SetSeconds(2.0f + nlRandomf(0.2f));
    }

    fn_80051F00(&m_v2AIFielderCenter);

    field_0x18.x = 0.0f;
    field_0x18.y = 0.0f;

    cFielder* pFielder;
    int i;
    const float* pWeight = lbl_804DBF68;
    for (i = 0; i < 4; ++i)
    {
        pFielder = m_pTeam->mUnidentified0D8[i];
        bool bIgnoreFielder = fn_800344B0(pFielder) || fn_800387CC(pFielder);
        if (!bIgnoreFielder)
        {
            float newY = field_0x18.y + *pWeight * pFielder->m_v3Position.y;
            float newX = field_0x18.x + *pWeight * pFielder->m_v3Position.x;
            field_0x18.y = newY;
            field_0x18.x = newX;
            pWeight++;
        }
    }

    if (m_pTeam->m_nSide == 1)
    {
        field_0x18.x = -field_0x18.x;
        field_0x18.y = -field_0x18.y;
    }

    for (int i = 0; i < 3; i++)
    {
        if (m_pFormations[i] != 0)
        {
            m_pFormations[i]->Update(dt);
        }
    }
}

void FormationManager::fn_80050D24()
{
    s32 j;
    FormationEval** pp = (FormationEval**)this;
    for (j = 0; j < 3; j++)
    {
        FormationEval* pEval = pp[1];
        delete pEval;
        *++pp = 0;
    }

    m_tSelectFormationsTimer.m_unk0 = m_tSelectFormationsTimer.m_uPackedTime != 0;
    m_tSelectFormationsTimer.m_uPackedTime = 0;
    m_CachedPositions[0].bCacheIsValid = false;
    m_CachedPositions[1].bCacheIsValid = false;
    m_CachedPositions[2].bCacheIsValid = false;
    m_CachedPositions[3].bCacheIsValid = false;
    m_v2AIFielderCenter.x = 0.0f;
    m_v2AIFielderCenter.y = 0.0f;
    field_0x18.x = 0.0f;
    field_0x18.y = 0.0f;
}

void FormationManager::ChooseNewFormations()
{
    int defensiveFormation;
    int offensiveFormation;
    unsigned int ballFormationSet;

    InterpreterCore* context = fn_800A695C(m_pTeam);
    UnidentifiedVariant_80054AB8 result = fn_80054AB8(context, "BestDefensiveFormation", m_pTeam);
    defensiveFormation = result.GetInt();

    result = fn_80054AB8(context, "BestOffensiveFormation", m_pTeam);
    offensiveFormation = result.GetInt();

    result = fn_80054AB8(context, "BestBallFormationSet", m_pTeam);
    ballFormationSet = result.GetInt();

    if (defensiveFormation == 0)
    {
        delete m_pFormations[FTYPE_DEFENSIVE];
        m_pFormations[FTYPE_DEFENSIVE] = 0;
    }
    else if (m_pFormations[FTYPE_DEFENSIVE] == 0
        || (unsigned int)defensiveFormation
            != m_pFormations[FTYPE_DEFENSIVE]->m_pFormationSpec->m_ID)
    {
        delete m_pFormations[FTYPE_DEFENSIVE];
        m_pFormations[FTYPE_DEFENSIVE] = FormationEval::Create(
            this, FTYPE_DEFENSIVE, FSET_NONE, (eFormation)defensiveFormation);
    }

    if (offensiveFormation == 0)
    {
        delete m_pFormations[FTYPE_OFFENSIVE];
        m_pFormations[FTYPE_OFFENSIVE] = 0;
    }
    else if (m_pFormations[FTYPE_OFFENSIVE] == 0
        || (unsigned int)offensiveFormation
            != m_pFormations[FTYPE_OFFENSIVE]->m_pFormationSpec->m_ID)
    {
        delete m_pFormations[FTYPE_OFFENSIVE];
        m_pFormations[FTYPE_OFFENSIVE] = FormationEval::Create(
            this, FTYPE_OFFENSIVE, FSET_NONE, (eFormation)offensiveFormation);
    }

    if (ballFormationSet == (unsigned int)FSET_NONE || ballFormationSet == 0)
    {
        delete m_pFormations[FTYPE_BALLPOSITION];
        m_pFormations[FTYPE_BALLPOSITION] = 0;
    }
    else if (m_pFormations[FTYPE_BALLPOSITION] == 0
        || ballFormationSet
            != (unsigned int)((FormationBallPosition*)m_pFormations[FTYPE_BALLPOSITION])
                   ->m_pFormationSet->m_ID)
    {
        delete m_pFormations[FTYPE_BALLPOSITION];
        m_pFormations[FTYPE_BALLPOSITION] = FormationEval::Create(
            this, FTYPE_BALLPOSITION, (eFormationSet)ballFormationSet, FORMATION_NONE);
    }
}

void FormationManager::fn_80051F00(nlVector2* pCenter)
{
    pCenter->x = 0.0f;
    pCenter->y = 0.0f;

    int numFielders = 0;
    for (int i = 0; i < 4; i++)
    {
        cFielder* pFielder = m_pTeam->GetFielder(i);
        bool bIgnoreFielder = fn_800344B0(pFielder) || fn_800387CC(pFielder);
        if (!bIgnoreFielder)
        {
            numFielders++;
            float newY = pCenter->y + pFielder->m_v3Position.y;
            float newX = pCenter->x + pFielder->m_v3Position.x;
            pCenter->y = newY;
            pCenter->x = newX;
        }
    }

    if (numFielders != 0)
    {
        float newY = (1.0f / numFielders) * pCenter->y;
        float newX = (1.0f / numFielders) * pCenter->x;
        pCenter->y = newY;
        pCenter->x = newX;
    }

    if (m_pTeam->m_nSide == 1)
    {
        pCenter->x = -pCenter->x;
        pCenter->y = -pCenter->y;
    }
}

unsigned int* FormationManager::fn_80052034()
{
    float fBestWeight = 0.0f;
    unsigned int* pFielderFormationPos = 0;
    float fBallFormationWeight = 0.0f;

    if (m_pFormations[FTYPE_BALLPOSITION] != 0
        && m_pFormations[FTYPE_BALLPOSITION]->fn_80054A20())
    {
        fBallFormationWeight = m_pFormations[FTYPE_BALLPOSITION]->GetWeight(0);
    }

    for (int i = 0; i < NUM_FORMATION_TYPES; i++)
    {
        FormationEval* pFormation = m_pFormations[i];
        if (pFormation != 0 && pFormation->fn_80054A20())
        {
            float fWeight;
            if (pFormation->m_eFormationType == FTYPE_BALLPOSITION)
            {
                fWeight = fBallFormationWeight;
            }
            else
            {
                fWeight = (1.0f - fBallFormationWeight) * pFormation->GetWeight(0);
            }

            if (fWeight >= fBestWeight)
            {
                fBestWeight = fWeight;
                pFielderFormationPos = pFormation->m_iFielderFormationPos;
            }
        }
    }

    return pFielderFormationPos;
}

bool FormationManager::CalculateFielderPosition(nlVector3& v3DestPosition,
    cFielder* pFielder, bool bInPosition, float fBallPosFormationWeight)
{
    float fFielderInPosition;
    float fWeights[3];
    nlVector3 v3FutureDesiredPosition;
    nlVector3 v3FormationPosition[2][3];

    int id = pFielder->m_ID;
    if (m_CachedPositions[id].bCacheIsValid)
    {
        v3DestPosition = m_CachedPositions[id].vPosition;
        return m_CachedPositions[id].bInPosition;
    }

    v3FutureDesiredPosition = v3Zero;
    fFielderInPosition = 0.0f;

    for (int i = 0; i < 3; i++)
    {
        FormationEval* pFormation = m_pFormations[i];
        if (pFormation != 0 && pFormation->fn_80054A20())
        {
            fWeights[i] = pFormation->GetWeight(0);
            pFormation->CalculateDesiredLocation(v3FormationPosition[1][i], pFielder, true);
            pFormation->CalculateDesiredLocation(v3FormationPosition[0][i], pFielder, false);

            if (v3FormationPosition[0][i].GetLengthSq3D() == 0.0f
                && fWeights[i] > 0.0f)
            {
                fn_8004F594(4, "Formation returned 0,0 as position!\n");
            }
        }
        else
        {
            const nlVector3& v3ZeroPosition = v3Zero;
            v3FormationPosition[0][i] = v3ZeroPosition;
            fWeights[i] = 0.0f;
            v3FormationPosition[1][i] = v3ZeroPosition;
        }
    }

    if (fWeights[2] >= 0.25f)
    {
        fWeights[2] = 1.0f;
    }

    float remainingWeight = 1.0f - fWeights[2];
    fWeights[0] *= remainingWeight;
    fWeights[1] *= remainingWeight;

    for (int i = 0; i < 3; i++)
    {
        FormationEval* pFormation = m_pFormations[i];
        if (pFormation != 0 && pFormation->fn_80054A20())
        {
            nlVector3 pos = v3FormationPosition[0][i];
            float weight = fWeights[i];
            float z = v3FutureDesiredPosition.z + weight * v3FormationPosition[1][i].z;
            float y = v3FutureDesiredPosition.y + weight * v3FormationPosition[1][i].y;
            float x = v3FutureDesiredPosition.x + weight * v3FormationPosition[1][i].x;
            v3FutureDesiredPosition.z = z;
            v3FutureDesiredPosition.y = y;
            v3FutureDesiredPosition.x = x;
            fFielderInPosition += fWeights[i] * pFormation->IsFielderInPosition(pFielder, pos, bInPosition);
        }
    }

    v3DestPosition = v3FutureDesiredPosition;
    bInPosition = fFielderInPosition >= 0.3f;

    if (v3DestPosition.GetLengthSq3D() == 0.0f)
    {
        fn_8004F594(4, "Formation returned 0,0 as position!\n");
    }

    m_CachedPositions[id].vPosition = v3DestPosition;
    m_CachedPositions[id].bInPosition = bInPosition;
    m_CachedPositions[id].bCacheIsValid = true;

    return bInPosition != false;
}

static inline const FormationSpec* GetFormationSpecInline(int id)
{
    const FormationSpec* spec = 0;
    int i;
    for (i = 0; i < FormationManager::m_NumFormationSets; i++)
    {
        spec = FormationManager::m_FormationSetArray[i].GetFormationSpecFromID(id);
        if (spec != 0)
        {
            break;
        }
    }
    return spec;
}

static inline FormationSet* UnidentifiedFormationSetLookup(eFormationSet id)
{
    int i;
    for (i = 0; i < FormationManager::m_NumFormationSets; i++)
    {
        if ((unsigned int)id
            == (unsigned int)FormationManager::m_FormationSetArray[i].m_ID)
        {
            return &FormationManager::m_FormationSetArray[i];
        }
    }
    return 0;
}

FormationEval* FormationEval::Create(FormationManager* pManager, eFormationType formType,
    eFormationSet formSetID, eFormation formID)
{
    FormationEval* pNewFormation = 0;
    FormationSet* formSet = 0;
    const FormationSpec* formDef = 0;

    if ((unsigned int)formSetID != (unsigned int)-1)
    {
        formSet = UnidentifiedFormationSetLookup(formSetID);
    }

    if ((unsigned int)formType != (unsigned int)-1)
    {
        formDef = GetFormationSpecInline(formID);
    }

    if (formSet != 0 || formDef != 0)
    {
        switch (formType)
        {
        case FTYPE_DEFENSIVE:
            pNewFormation = new (nlMalloc(sizeof(FormationDefensive), 8, false))
                FormationDefensive(pManager, formType, formDef);
            break;
        case FTYPE_OFFENSIVE:
            pNewFormation = new (nlMalloc(sizeof(FormationOffensive), 8, false))
                FormationOffensive(pManager, formType, formDef);
            break;
        case FTYPE_BALLPOSITION:
            pNewFormation = new (nlMalloc(sizeof(FormationBallPosition), 8, false))
                FormationBallPosition(pManager, formType, formSet);
            break;
        }
    }
    else
    {
        nlPrintf("FormationEval::Create - error! Could not find requested formation set or spec.\n");
    }

    return pNewFormation;
}

void FormationEval::Update(float fDeltaT)
{
    fn_80052978();

    if (m_SortTimer.Countdown(fDeltaT, 0.0f))
    {
        SortPlayers(0);
        float randomTime = nlRandomf(0.2f);
        m_SortTimer.SetSeconds(0.4f + randomTime);
    }
}

float FormationEval::GetWeight(const nlVector2* v2AIBallLoc)
{
    float fWeight = 1.0f;

    if (!fn_80054A20())
    {
        fWeight = 0.0f;
    }
    else if (m_pFormationSpec != 0 && m_pFormationSpec->m_iKeyIndex > -1 && v2AIBallLoc != 0)
    {
        float fInDist = m_pFormationSpec->m_InRadius;
        float fOutDist = m_pFormationSpec->m_OutRadius;
        nlVector2& keyLoc = m_pFormationSpec->GetKeyLocation();
        nlVector2 offset;
        float dx, dy;
        dy = v2AIBallLoc->y - keyLoc.y;
        dx = v2AIBallLoc->x - keyLoc.x;
        offset.x = dx;
        offset.y = dy;
        float dist = nlSqrt(offset.x * offset.x + offset.y * offset.y, true);

        if (dist < fInDist)
        {
            fWeight = InterpolateRangeClamped(0.5f, 1.0f, fInDist, 0.0f, dist);
        }
        else
        {
            fWeight = InterpolateRangeClamped(0.0f, 0.5f, fOutDist, fInDist, dist);
        }
    }

    return fWeight;
}

void FormationEval::fn_80052978()
{
    m_pKeyPlayer = 0;
}

void FormationEval::AssignPositionsToFielders(
    unsigned int* pFielderPosAssignments, float (*fFielderToPositionDistance)[4])
{
    float fBestDistance = 1.0e35f;
    cPlayer* pKeyPlayer = GetKeyPlayer();
    cTeam* pTeam = m_pFormationManager->m_pTeam;
    int aiAssignedPos[4];

    for (aiAssignedPos[0] = 0; aiAssignedPos[0] < 4; aiAssignedPos[0]++)
    {
        float fDistance = fFielderToPositionDistance[0][aiAssignedPos[0]];

        for (aiAssignedPos[1] = 0; aiAssignedPos[1] < 4; aiAssignedPos[1]++)
        {
            if (aiAssignedPos[1] == aiAssignedPos[0])
            {
                continue;
            }

            float fDistancePos1 = fDistance;
            fDistance += fFielderToPositionDistance[1][aiAssignedPos[1]];

            for (aiAssignedPos[2] = 0; aiAssignedPos[2] < 4; aiAssignedPos[2]++)
            {
                if (aiAssignedPos[2] == aiAssignedPos[1])
                {
                    continue;
                }

                if (aiAssignedPos[2] == aiAssignedPos[0])
                {
                    continue;
                }

                float fDistancePos2 = fDistance;
                fDistance += fFielderToPositionDistance[2][aiAssignedPos[2]];

                for (aiAssignedPos[3] = 0; aiAssignedPos[3] < 4; aiAssignedPos[3]++)
                {
                    if (aiAssignedPos[3] == aiAssignedPos[2])
                    {
                        continue;
                    }

                    if (aiAssignedPos[3] == aiAssignedPos[1])
                    {
                        continue;
                    }

                    if (aiAssignedPos[3] == aiAssignedPos[0])
                    {
                        continue;
                    }

                    float fDistancePos3 = fDistance;
                    fDistance += fFielderToPositionDistance[3][aiAssignedPos[3]];

                    float fCaptainPosScore = 0.0f;
                    float fCaptainPosCount = fCaptainPosScore;
                    int* piAssignedPos = aiAssignedPos;
                    const float* pCaptainPosBonus = lbl_804DBF28[0];

                    for (int iFielder = 0; iFielder < 4; iFielder++)
                    {
                        cFielder* pFielder = pTeam->GetFielder(iFielder);
                        if (iFielder == pKeyPlayer->m_ID
                            && *piAssignedPos == m_pFormationSpec->m_iKeyIndex)
                        {
                            fCaptainPosScore += 25.0f;
                            fCaptainPosCount += 5.0f;
                        }
                        else if (lbl_806DBA20)
                        {
                            float fCaptainPosBonus = pCaptainPosBonus[*piAssignedPos];
                            if (fCaptainPosBonus > 0.0f)
                            {
                                fCaptainPosScore += fCaptainPosBonus;
                                fCaptainPosCount += 1.0f;
                            }
                        }

                        piAssignedPos++;
                        pCaptainPosBonus += 4;
                    }

                    float fCaptainPosMult;
                    if (fCaptainPosCount > 0.0f)
                    {
                        fCaptainPosMult = fCaptainPosScore / fCaptainPosCount;
                    }
                    else
                    {
                        fCaptainPosMult = 0.0f;
                    }

                    if (fCaptainPosMult > 0.0f)
                    {
                        fDistance /= fCaptainPosMult;
                    }

                    if (fDistance < fBestDistance)
                    {
                        fBestDistance = fDistance;
                        pFielderPosAssignments[0] = aiAssignedPos[0];
                        pFielderPosAssignments[1] = aiAssignedPos[1];
                        pFielderPosAssignments[2] = aiAssignedPos[2];
                        pFielderPosAssignments[3] = aiAssignedPos[3];
                    }

                    fDistance = fDistancePos3;
                }

                fDistance = fDistancePos2;
            }

            fDistance = fDistancePos1;
        }
    }
}

inline void FormationEval::AILocToFieldLoc(
    nlVector3& dest, const nlVector3& ai_location, int nTeamID)
{
    if (nTeamID == 0)
    {
        dest = ai_location;
        return;
    }
    nlVec3Set(dest, -ai_location.x, -ai_location.y, 0.0f);
}

static inline void SubtractVector(nlVector2& result, const nlVector2& value)
{
    result.y -= value.y;
    result.x -= value.x;
}

static inline cTeam* GetFormationTeam(FormationManager* manager)
{
    return manager->m_pTeam;
}

void FormationEval::SortPlayers(const nlVector2* v2Center)
{
    float fFielderToPositionDistance[4][4];
    nlVector2 av2FormationPositions[4];
    nlVector3 av3FielderAILocs[4];
    int i;
    nlVector2 v2CenterOfPlayers;
    int i_fielder;
    int i_pos;

    if (!fn_80054A20())
    {
        return;
    }

    cTeam* team = GetFormationTeam(m_pFormationManager);
    memset(fFielderToPositionDistance, 0, sizeof(fFielderToPositionDistance));

    for (i = 0; i < 4; i++)
    {
        cFielder* pFielder = team->GetFielder(i);
        do
        {
            if (fn_800DEFD4(pFielder))
            {
                bool bStolen = false;
                bool bAirborne = g_pBall->meBallState == 5
                    || g_pBall->meBallState == 3;
                if (bAirborne && g_pBall->m_pPassTarget != 0)
                {
                    bStolen = true;
                }
                if (bStolen)
                {
                    av3FielderAILocs[i] = g_pBall->m_v3PassIntercept;
                    break;
                }
            }

            bool bHasGlobalPad = pFielder->GetGlobalPad() != 0;
            if (bHasGlobalPad)
            {
                nlVec3ScaleAdd(av3FielderAILocs[i], 0.1f,
                    pFielder->m_v3Velocity, pFielder->m_v3Position);
            }
            else
            {
                nlVec3ScaleAdd(av3FielderAILocs[i], 0.1f,
                    pFielder->m_v3Velocity, pFielder->m_v3Position);
            }
        } while (false);

        FieldLocToAILoc(av3FielderAILocs[i], av3FielderAILocs[i], team->m_nSide);
    }

    if (v2Center != 0)
    {
        v2CenterOfPlayers = *v2Center;
    }
    else
    {
        v2CenterOfPlayers = m_pFormationManager->m_v2AIFielderCenter;
        SubtractVector(v2CenterOfPlayers, m_pFormationSpec->m_v2Center);
    }

    for (i = 0; i < 4; i++)
    {
        nlVec2Set(av2FormationPositions[i],
            m_pFormationSpec->m_Positions[i].m_Location.x + v2CenterOfPlayers.x,
            m_pFormationSpec->m_Positions[i].m_Location.y + v2CenterOfPlayers.y);
    }

    const int* pFielderOrder = lbl_804DBF78;
    if (m_pFormationManager->m_pTeam->field_0x18 == 0)
    {
        pFielderOrder = lbl_804DBF88;
    }

    for (i_fielder = 0; i_fielder < 4; i_fielder++)
    {
        cFielder* pFielder = team->GetFielder(i_fielder);
        bool bApplyFielderOrder = fn_800344B0(pFielder) || fn_800387CC(pFielder);

        for (i_pos = 0; i_pos < 4; i_pos++)
        {
            nlVector2 offset;
            offset.y = av2FormationPositions[i_pos].y - av3FielderAILocs[i_fielder].y;
            offset.x = av2FormationPositions[i_pos].x - av3FielderAILocs[i_fielder].x;
            fFielderToPositionDistance[i_fielder][i_pos]
                = nlSqrt(offset.x * offset.x + offset.y * offset.y, true);
            fFielderToPositionDistance[i_fielder][i_pos] /= fn_8002E1B0(pFielder);

            if (bApplyFielderOrder)
            {
                fFielderToPositionDistance[i_fielder][i_pos]
                    += 10.0f * (float)pFielderOrder[i_pos];
            }
        }
    }

    AssignPositionsToFielders(m_iFielderFormationPos, fFielderToPositionDistance);
}

cPlayer* FormationEval::GetKeyPlayer()
{
    if (m_pKeyPlayer != 0)
    {
        return m_pKeyPlayer;
    }

    cTeam* team = m_pFormationManager->m_pTeam;
    cPlayer* otherCarrier;
    cPlayer* pKeyPlayer;

    fn_800A695C(team);
    cPlayer* strategicBallCarrier = fn_800DF790(team);
    pKeyPlayer = strategicBallCarrier;

    if (pKeyPlayer == 0)
    {
        pKeyPlayer = g_pBall->GetOwnerGoalie();
    }

    if (pKeyPlayer == 0)
    {
        otherCarrier = fn_800DF790(team->GetOtherTeam());

        if (otherCarrier != 0)
        {
            for (int i_fielder = 0; i_fielder < 4; i_fielder++)
            {
                cFielder* pFielder = team->GetFielder(i_fielder);
                if (pFielder->GetMark() == (cFielder*)otherCarrier)
                {
                    pKeyPlayer = pFielder;
                    break;
                }
            }
        }
        else
        {
            pKeyPlayer = team->mpBestBallInterceptor;
        }
    }

    if (pKeyPlayer == 0)
    {
        for (int i_fielder = 0; i_fielder < 4; i_fielder++)
        {
            bool bHasGlobalPad = team->GetFielder(i_fielder)->GetGlobalPad() != 0;
            if (bHasGlobalPad)
            {
                pKeyPlayer = team->GetFielder(i_fielder);
                break;
            }
        }
    }

    if (pKeyPlayer == 0)
    {
        if (m_eFormationType == FTYPE_DEFENSIVE)
        {
            pKeyPlayer = fn_800A8884(team);
        }
        else if (m_eFormationType == FTYPE_OFFENSIVE)
        {
            pKeyPlayer = fn_800A8808(team);
        }
        else
        {
            pKeyPlayer = fn_800A8800(team);
        }
    }

    m_pKeyPlayer = pKeyPlayer;
    return pKeyPlayer;
}

void FormationEval::GetKeyPositions(cFielder* pFielder, nlVector3& v3KeyAIPosition,
    nlVector3* pKeyFormationAIPosition, bool bExtrapolate)
{
    cPlayer* keyPlayer = GetKeyPlayer();

    if (g_pBall->m_pPassTarget == keyPlayer)
    {
        v3KeyAIPosition = g_pBall->m_v3PassIntercept;

        if (pKeyFormationAIPosition != 0 && keyPlayer->m_eClassType != GOALIE)
        {
            u32 posIndex = m_iFielderFormationPos[keyPlayer->m_ID];
            const FormationPos* pPos = &m_pFormationSpec->m_Positions[posIndex];
            f32 px;
            f32 py;
            py = pPos->m_Location.y;
            px = pPos->m_Location.x;
            pKeyFormationAIPosition->x = px;
            pKeyFormationAIPosition->y = py;
            pKeyFormationAIPosition->z = 0.0f;
        }
    }
    else if (keyPlayer == pFielder && m_eFormationType != FTYPE_BALLPOSITION)
    {
        FormationManager* mgr = m_pFormationManager;
        f32 x;
        f32 y;
        y = mgr->m_v2AIFielderCenter.y;
        x = mgr->m_v2AIFielderCenter.x;
        v3KeyAIPosition.x = x;
        v3KeyAIPosition.y = y;
        v3KeyAIPosition.z = 0.0f;

        if (pKeyFormationAIPosition != 0)
        {
            const FormationSpec* spec = m_pFormationSpec;
            f32 cx;
            f32 cy;
            cy = spec->m_v2Center.y;
            cx = spec->m_v2Center.x;
            pKeyFormationAIPosition->x = cx;
            pKeyFormationAIPosition->y = cy;
            pKeyFormationAIPosition->z = 0.0f;
        }
    }
    else
    {
        if (bExtrapolate)
        {
            f32 t = 0.1f;
            f32 rz = keyPlayer->m_v3Position.z + t * keyPlayer->m_v3Velocity.z;
            f32 rx;
            f32 ry;
            ry = keyPlayer->m_v3Position.y + t * keyPlayer->m_v3Velocity.y;
            rx = keyPlayer->m_v3Position.x + t * keyPlayer->m_v3Velocity.x;
            v3KeyAIPosition.x = rx;
            v3KeyAIPosition.y = ry;
            v3KeyAIPosition.z = rz;
        }
        else
        {
            v3KeyAIPosition = keyPlayer->m_v3Position;
        }

        FieldLocToAILoc(v3KeyAIPosition, v3KeyAIPosition, pFielder->m_pTeam->m_nSide);

        if (pKeyFormationAIPosition != 0)
        {
            if (m_pFormationSpec->m_iKeyIndex > -1)
            {
                nlVector2& keyLoc = m_pFormationSpec->GetKeyLocation();
                f32 lx;
                f32 ly;
                ly = keyLoc.y;
                lx = keyLoc.x;
                pKeyFormationAIPosition->x = lx;
                pKeyFormationAIPosition->y = ly;
                pKeyFormationAIPosition->z = 0.0f;
            }
            else
            {
                pKeyFormationAIPosition->x = 2.0f;
                pKeyFormationAIPosition->y = 0.0f;
                pKeyFormationAIPosition->z = 0.0f;

                if (keyPlayer->m_eClassType != GOALIE)
                {
                    u32 posIndex = m_iFielderFormationPos[keyPlayer->m_ID];
                    const FormationPos* pPos = &m_pFormationSpec->m_Positions[posIndex];
                    f32 px;
                    f32 py;
                    py = pPos->m_Location.y;
                    px = pPos->m_Location.x;
                    pKeyFormationAIPosition->x = px;
                    pKeyFormationAIPosition->y = py;
                    pKeyFormationAIPosition->z = 0.0f;
                }
            }
        }
    }
}

void FormationEval::CalculateDesiredLocation(
    nlVector3& destPosition, cFielder* pFielder, bool bExtrapolate)
{
    nlVector3 v3KeyAIPosition;
    nlVector3 v3KeyFormationAIPosition;
    nlVector2 v2FormationMin;
    nlVector2 v2FormationMax;

    GetKeyPositions(
        pFielder, v3KeyAIPosition, &v3KeyFormationAIPosition, bExtrapolate);

    nlVector2 offset;
    float dy;
    float dx;
    dy = v3KeyAIPosition.y - v3KeyFormationAIPosition.y;
    dx = v3KeyAIPosition.x - v3KeyFormationAIPosition.x;
    offset.y = dy;
    offset.x = dx;

    u32 posIndex = m_iFielderFormationPos[pFielder->m_ID];

    if (m_eFormationType == FTYPE_BALLPOSITION)
    {
        nlVector3 v3Offset;
        v3Offset.x = offset.x;
        v3Offset.y = offset.y;
        v3Offset.z = v3KeyAIPosition.z - v3KeyFormationAIPosition.z;

        float fScale = nlSqrt(v3Offset.GetLengthSq3D(), true);
        float fOutRadius = m_pFormationSpec->m_OutRadius;
        float fInRadius = m_pFormationSpec->m_InRadius;
        fScale = NormalizeVal(fScale, fInRadius, fOutRadius);
        float scaledY;
        float scaledX;
        scaledY = fScale * offset.y;
        scaledX = fScale * offset.x;
        offset.x = scaledX;
        offset.y = scaledY;

        nlVec2Set(v2FormationMin, -18.539999f, -11.25f);
        nlVec2Set(v2FormationMax, 18.539999f, 11.25f);
    }
    else
    {
        m_pFormationSpec->CalculateExtents(v2FormationMin, v2FormationMax,
            *(nlVector2*)&v3KeyFormationAIPosition);
    }

    const FormationPos* pPos = &m_pFormationSpec->m_Positions[posIndex];
    float clampX
        = nlMinEquals(nlMaxEquals(offset.x, v2FormationMin.x), v2FormationMax.x);
    destPosition.x = pPos->m_Location.x + clampX;

    float clampY
        = nlMinEquals(nlMaxEquals(offset.y, v2FormationMin.y), v2FormationMax.y);
    destPosition.y = pPos->m_Location.y + clampY;
    destPosition.z = 0.0f;

    FieldLocToAILoc(destPosition, destPosition, pFielder->m_pTeam->m_nSide);
}

float FormationEval::IsFielderInPosition(
    cFielder* pFielder, nlVector3 v3Pos, bool bExtended)
{
    if (!fn_80054A20())
    {
        return 0.0f;
    }

    nlVector3 v3DesiredPos;
    GetKeyPositions(pFielder, v3DesiredPos, 0, false);
    FieldLocToAILoc(v3DesiredPos, v3DesiredPos, pFielder->m_pTeam->m_nSide);

    nlVector2 offset;
    float dy;
    float dx;
    dx = pFielder->m_v3Position.x - v3DesiredPos.x;
    dy = pFielder->m_v3Position.y - v3DesiredPos.y;
    offset.x = dx;
    offset.y = dy;
    float distToDesiredSquared
        = offset.x * offset.x + offset.y * offset.y;
    float distToDesired = nlSqrt(distToDesiredSquared, true);

    nlVector2 offset2;
    float dx2;
    float dy2;
    dx2 = pFielder->m_v3Position.x;
    dx2 -= v3Pos.x;
    dy2 = pFielder->m_v3Position.y - v3Pos.y;
    offset2.x = dx2;
    offset2.y = dy2;
    float distToTarget = nlVec2Length(offset2);

    float normalizedDist = NormalizeVal(distToDesired,
        lbl_8056CF08.m_pGameTweaks->vGetInPositionKeyFielderDist.x,
        lbl_8056CF08.m_pGameTweaks->vGetInPositionKeyFielderDist.y);

    float inDist = Interpolate(
        lbl_8056CF08.m_pGameTweaks->vGetInPositionInRadius.x,
        lbl_8056CF08.m_pGameTweaks->vGetInPositionInRadius.y,
        normalizedDist);

    float outDist = Interpolate(
        lbl_8056CF08.m_pGameTweaks->vGetInPositionOutRadius.x,
        lbl_8056CF08.m_pGameTweaks->vGetInPositionOutRadius.y,
        normalizedDist);

    float result = 0.0f;
    if (distToTarget <= inDist)
    {
        result = 1.0f;
    }
    else if (bExtended && distToTarget <= outDist)
    {
        result = nlMinEquals(
            nlMaxEquals(1.0f - distToTarget / outDist, 0.0f), 1.0f);
    }

    return result;
}

void FormationDefensive::Update(float fDeltaT)
{
    nlVector3 v3AIBallLoc;

    fn_80052978();

    if (m_SortTimer.Countdown(fDeltaT, 0.0f))
    {
        v3AIBallLoc = v3Zero;

        if (g_pBall->GetOwnerGoalie() == 0)
        {
            *(nlVector2*)&v3AIBallLoc = m_pFormationManager->m_pTeam
                                            ->GetOtherTeam()
                                            ->m_pFormationManager
                                            ->field_0x18;
            v3AIBallLoc.y = 0.0f;
            v3AIBallLoc.z = 0.0f;

            AILocToFieldLoc(v3AIBallLoc, v3AIBallLoc,
                m_pFormationManager->m_pTeam->GetOtherTeam()->m_nSide);
            FieldLocToAILoc(v3AIBallLoc, v3AIBallLoc,
                m_pFormationManager->m_pTeam->m_nSide);
        }

        SortPlayers((const nlVector2*)&v3AIBallLoc);

        float randomTime = nlRandomf(0.2f);
        m_SortTimer.SetSeconds(0.4f + randomTime);
    }
}

static inline float CalcScore(
    float fUpScore, float fDownScore, float fLateralScore)
{
    float fUpPos = fUpScore >= 0.0f ? fUpScore : 0.0f;
    float fDownPos = fDownScore >= 0.0f ? fDownScore : 0.0f;
    float fLateralPos = fLateralScore >= 0.0f ? fLateralScore : 0.0f;
    float fScore = (fUpPos + fDownPos + fLateralPos) * 0.5f;
    fScore = fScore >= 0.0f ? fScore : 0.0f;
    fScore = fScore <= 1.0f ? fScore : 1.0f;
    return 1.0f - fScore;
}

float FormationDefensive::IsFielderInPosition(
    cFielder* pFielder, nlVector3 v3DesiredPosition, bool bInPosition)
{
    nlVector3 v3KeyPosition;
    GetKeyPositions(pFielder, v3KeyPosition, 0, false);
    FieldLocToAILoc(v3KeyPosition, v3KeyPosition, pFielder->m_pTeam->m_nSide);

    nlVector2 keyOffset;
    float dy;
    float dx;
    dx = pFielder->m_v3Position.x - v3KeyPosition.x;
    dy = pFielder->m_v3Position.y - v3KeyPosition.y;
    keyOffset.x = dx;
    keyOffset.y = dy;
    float fPercent
        = nlSqrt(keyOffset.x * keyOffset.x + keyOffset.y * keyOffset.y, true);

    fPercent = NormalizeVal(fPercent,
        lbl_8056CF08.m_pGameTweaks->vGetInPositionKeyFielderDist.x,
        lbl_8056CF08.m_pGameTweaks->vGetInPositionKeyFielderDist.y);

    nlVector3 v3FielderPos = pFielder->m_v3Position;
    nlVector3 v3NetLocation = pFielder->GetAIOffNetLocation(0);
    const FormationPositionThresholds* pPositionThresholds
        = &g_aDefensiveFormationThresholds[pFielder->m_eRole];

    nlVector2 offset;
    float distY;
    float distX;
    distX = v3FielderPos.x - v3DesiredPosition.x;
    distY = v3FielderPos.y - v3DesiredPosition.y;
    offset.x = distX;
    offset.y = distY;
    float dist = nlSqrt(offset.x * offset.x + offset.y * offset.y, true);

    float fInPosition = 0.0f;

    if (dist <= pPositionThresholds->fInRadius)
    {
        fInPosition = 1.0f;
    }
    else
    {
        bool fNearZero = (float)__fabs(fPercent - fInPosition) <= 0.0001f;
        if (fNearZero)
        {
            if (pFielder == GetKeyPlayer())
            {
                fInPosition = 1.0f;
            }
            else
            {
                fInPosition = 0.0f;
            }
        }
        else if (bInPosition)
        {
            float fUpScore;
            float fLateralScore;
            float fDownScore;
            fUpScore = ((v3DesiredPosition.x - v3FielderPos.x)
                           * AIsgn(v3NetLocation.x))
                / (fPercent * pPositionThresholds->fOutUpField);

            fDownScore = ((v3FielderPos.x - v3DesiredPosition.x)
                             * AIsgn(v3NetLocation.x))
                / (fPercent * pPositionThresholds->fOutDownField);

            float fLateralDistance
                = v3FielderPos.y - v3DesiredPosition.y;
            double fAbsLateralDistance = __fabs(fLateralDistance);
            float fAbsLateralDistanceFloat
                = (float)fAbsLateralDistance;
            fLateralScore
                = fAbsLateralDistanceFloat
                / (fPercent * pPositionThresholds->fOutLateral);

            fInPosition = CalcScore(fUpScore, fDownScore, fLateralScore);
        }
    }

    return fInPosition;
}

float FormationDefensive::GetWeight(const nlVector2* v2AIBallLoc)
{
    return 1.0f;
}

void FormationDefensive::GetKeyPositions(cFielder* pFielder,
    nlVector3& v3KeyAIPosition, nlVector3* pKeyFormationAIPosition,
    bool bExtrapolate)
{
    cTeam* pOtherTeam = pFielder->m_pTeam->GetOtherTeam();

    if (g_pBall->GetOwnerGoalie() != 0)
    {
        nlVec3Set(v3KeyAIPosition, -5.0f, 0.0f, 0.0f);

        if (!pFielder->IsOnSameTeam(g_pBall->GetOwnerGoalie()))
        {
            v3KeyAIPosition.x *= -1.0f;
        }
    }
    else
    {
        v3KeyAIPosition.x = pOtherTeam->m_pFormationManager->field_0x18.x;
        v3KeyAIPosition.y = 0.0f;
        v3KeyAIPosition.z = 0.0f;

        AILocToFieldLoc(
            v3KeyAIPosition, v3KeyAIPosition, pOtherTeam->m_nSide);
        FieldLocToAILoc(v3KeyAIPosition, v3KeyAIPosition,
            pFielder->m_pTeam->m_nSide);
    }

    v3KeyAIPosition.x
        = nlMaxEquals(-12.0f, v3KeyAIPosition.x - 4.0f);

    if (pKeyFormationAIPosition != 0)
    {
        nlVec3Set(*pKeyFormationAIPosition, m_pFormationSpec->m_v2Center.x,
            m_pFormationSpec->m_v2Center.y, 0.0f);
    }
}

float FormationOffensive::IsFielderInPosition(
    cFielder* pFielder, nlVector3 v3DesiredPosition, bool bInPosition)
{
    nlVector3 v3KeyPosition;
    GetKeyPositions(pFielder, v3KeyPosition, 0, false);
    FieldLocToAILoc(v3KeyPosition, v3KeyPosition, pFielder->m_pTeam->m_nSide);

    nlVector2 keyOffset;
    float dy;
    float dx;
    dx = pFielder->m_v3Position.x - v3KeyPosition.x;
    dy = pFielder->m_v3Position.y - v3KeyPosition.y;
    keyOffset.x = dx;
    keyOffset.y = dy;
    float fPercent
        = nlSqrt(keyOffset.x * keyOffset.x + keyOffset.y * keyOffset.y, true);

    fPercent = NormalizeVal(fPercent,
        lbl_8056CF08.m_pGameTweaks->vGetInPositionKeyFielderDist.x,
        lbl_8056CF08.m_pGameTweaks->vGetInPositionKeyFielderDist.y);

    nlVector3 v3FielderPos = pFielder->m_v3Position;
    nlVector3 v3NetLocation = pFielder->GetAIOffNetLocation(0);
    const FormationPositionThresholds* pPositionThresholds
        = &g_aDefensiveFormationThresholds[pFielder->m_eRole];

    nlVector2 offset;
    float distY;
    float distX;
    distX = v3FielderPos.x - v3DesiredPosition.x;
    distY = v3FielderPos.y - v3DesiredPosition.y;
    offset.x = distX;
    offset.y = distY;
    float dist = nlSqrt(offset.x * offset.x + offset.y * offset.y, true);

    float fInPosition = 0.0f;

    if (dist <= pPositionThresholds->fInRadius)
    {
        fInPosition = 1.0f;
    }
    else
    {
        bool fNearZero = (float)__fabs(fPercent - fInPosition) <= 0.0001f;
        if (fNearZero)
        {
            if (pFielder == GetKeyPlayer())
            {
                fInPosition = 1.0f;
            }
            else
            {
                fInPosition = 0.0f;
            }
        }
        else if (bInPosition)
        {
            float fUpScore;
            float fLateralScore;
            float fDownScore;
            fUpScore = ((v3DesiredPosition.x - v3FielderPos.x)
                           * AIsgn(v3NetLocation.x))
                / (fPercent * pPositionThresholds->fOutUpField);

            fDownScore = ((v3FielderPos.x - v3DesiredPosition.x)
                             * AIsgn(v3NetLocation.x))
                / (fPercent * pPositionThresholds->fOutDownField);

            float fLateralDistance
                = v3FielderPos.y - v3DesiredPosition.y;
            double fAbsLateralDistance = __fabs(fLateralDistance);
            float fAbsLateralDistanceFloat
                = (float)fAbsLateralDistance;
            fLateralScore
                = fAbsLateralDistanceFloat
                / (fPercent * pPositionThresholds->fOutLateral);

            fInPosition = CalcScore(fUpScore, fDownScore, fLateralScore);
        }
    }

    return fInPosition;
}

float FormationOffensive::GetWeight(const nlVector2* v2AIBallLoc)
{
    return NormalizeVal(
        m_pFormationManager->m_v2AIFielderCenter.x, -12.360001f, 12.360001f);
}

FormationBallPosition::~FormationBallPosition()
{
    while (field_0x34 != 0)
    {
        FormationEval* pEval = nlListRemoveStart(&field_0x34, &field_0x38);
        delete pEval;
    }
}

void FormationBallPosition::fn_80052978()
{
    m_pKeyPlayer = 0;

    FormationEval* pEval = field_0x34;
    while (pEval != 0)
    {
        pEval->fn_80052978();
        pEval = pEval->next;
    }
}

inline void FormationBallPosition::CalcBallPosition(nlVector2& v2DestAIBallPos)
{
    nlVector3 v3AIBallLoc;

    if (g_pBall->m_pOwner != 0)
    {
        nlVec3ScaleAdd(v3AIBallLoc, 0.1f,
            g_pBall->m_pOwner->m_v3Velocity,
            g_pBall->m_pOwner->m_v3Position);
    }
    else if (g_pBall->m_pPassTarget != 0)
    {
        v3AIBallLoc = g_pBall->m_v3PassIntercept;
    }
    else
    {
        fn_800180F4(g_pBall, &v3AIBallLoc, 0.1f);
    }

    FieldLocToAILoc(v3AIBallLoc, v3AIBallLoc,
        m_pFormationManager->m_pTeam->m_nSide);

    v2DestAIBallPos = *(const nlVector2*)&v3AIBallLoc;
}

void FormationBallPosition::Update(float fDeltaT)
{
    nlVector2 vAIBallLoc;

    fn_80052978();

    CalcBallPosition(vAIBallLoc);

    if (cField::IsOnField(vAIBallLoc))
    {
        SelectClosestBallFormations(vAIBallLoc);

        if (m_SortTimer.Countdown(fDeltaT, 0.0f))
        {
            FormationEval* pEval = field_0x34;
            while (pEval != 0)
            {
                pEval->SortPlayers((const nlVector2*)&v3Zero);
                pEval = pEval->next;
            }

            if (fn_800DFF1C())
            {
                m_SortTimer = g_pBall->m_tPassTargetTimer;
            }
            else
            {
                m_SortTimer.SetSeconds(0.4f + nlRandomf(0.2f));
            }
        }
    }
}

bool FormationBallPosition::SelectClosestBallFormations(const nlVector2& v2AIBallLoc)
{
    FormationSpec* pClosest[4];
    FormationSpec* pSpec;
    FormationSpec** ppClosest = pClosest;
    FormationEval* pEval = field_0x34;
    bool bChanged = false;
    int numClosest = 0;

    while (pEval != 0)
    {
        pSpec = (FormationSpec*)pEval->m_pFormationSpec;
        FormationEval* pNextEval = pEval->next;
        nlVector2& keyLoc = pSpec->GetKeyLocation();
        nlVector2 offset;
        float dx, dy;
        dy = v2AIBallLoc.y - keyLoc.y;
        dx = v2AIBallLoc.x - keyLoc.x;
        offset.x = dx;
        offset.y = dy;
        float dist = nlSqrt(offset.x * offset.x + offset.y * offset.y, true);

        if (dist > pSpec->m_OutRadius)
        {
            field_0x3C--;
            nlListRemoveElement(&field_0x34, pEval, &field_0x38);
            delete pEval;
            bChanged = true;
        }
        else
        {
            *ppClosest++ = pSpec;
            numClosest++;
        }

        pEval = pNextEval;
    }

    for (int i = 0; i < m_pFormationSet->m_NumFormationDefs && field_0x3C < 4; i++)
    {
        FormationSpec* pSpec = m_pFormationSet->GetFormationSpec(i);
        if (!pSpec->field_0x04)
        {
            continue;
        }

        bool bAlreadySelected = false;
        for (int j = 0; j < numClosest; j++)
        {
            if (pClosest[j] == pSpec)
            {
                bAlreadySelected = true;
                break;
            }
        }

        if (bAlreadySelected)
        {
            continue;
        }

        nlVector2& keyLoc = pSpec->GetKeyLocation();
        nlVector2 offset;
        float dx, dy;
        dy = v2AIBallLoc.y - keyLoc.y;
        dx = v2AIBallLoc.x - keyLoc.x;
        offset.x = dx;
        offset.y = dy;
        float dist = nlSqrt(offset.x * offset.x + offset.y * offset.y, true);

        if (dist < pSpec->m_OutRadius)
        {
            field_0x3C++;
            FormationEval* pNewEval = new (nlMalloc(sizeof(FormationEval), 8, false))
                FormationEval(m_pFormationManager, FTYPE_BALLPOSITION, pSpec);
            pNewEval->SortPlayers((const nlVector2*)&v3Zero);
            nlListAddEnd(&field_0x34, &field_0x38, pNewEval);
            bChanged = true;
        }
    }

    return bChanged;
}

void FormationBallPosition::CalculateDesiredLocation(
    nlVector3& destPosition, cFielder* pFielder, bool bExtrapolate)
{
    if (!fn_80054A20())
    {
        destPosition = pFielder->m_v3Position;
        return;
    }

    nlVector3 v3Position;
    nlVector2 vAIBallPos;

    CalcBallPosition(vAIBallPos);

    nlVector3 v3DesiredPosition = v3Zero;
    float fTotalWeight = 0.0f;

    FormationEval* pEval = field_0x34;
    while (pEval != 0)
    {
        pEval->CalculateDesiredLocation(
            v3Position, pFielder, bExtrapolate);
        float fWeight = pEval->GetWeight(&vAIBallPos);
        nlVec3ScaleAdd(
            v3DesiredPosition, fWeight, v3Position, v3DesiredPosition);
        fTotalWeight += pEval->GetWeight(&vAIBallPos);
        pEval = pEval->next;
    }

    if (fTotalWeight > 0.0f)
    {
        nlVec3Scale(destPosition, v3DesiredPosition, 1.0f / fTotalWeight);
    }
    else
    {
        destPosition = v3Zero;
    }
}

float FormationBallPosition::GetWeight(const nlVector2* v2AIBallLoc)
{
    float fWeight = 0.0f;
    nlVector2 vAIBallPos;

    CalcBallPosition(vAIBallPos);

    FormationEval* pEval = field_0x34;
    while (pEval != 0)
    {
        fWeight = nlMaxEquals(fWeight, pEval->GetWeight(&vAIBallPos));
        pEval = pEval->next;
    }

    return fWeight;
}

bool FormationEval::fn_80054A20()
{
    return m_pFormationSpec != 0;
}

bool FormationBallPosition::fn_80054A20()
{
    return field_0x3C > 0;
}

FormationSet::~FormationSet()
{
    if (m_AutoDelete)
    {
        delete[] m_FormationDefArray;
    }
}

extern "C" UnidentifiedVariant_80054AB8 fn_80054AC8(InterpreterCore*, cTeam*, const char*);
extern "C" UnidentifiedVariant_80054AB8 fn_80054B28(InterpreterCore*, const unsigned int&, cTeam*);

extern "C" UnidentifiedVariant_80054AB8 fn_80054AB8(
    InterpreterCore* context, const char* name, cTeam* team)
{
    return fn_80054AC8(context, team, name);
}

extern "C" UnidentifiedVariant_80054AB8 fn_80054AC8(
    InterpreterCore* context, cTeam* team, const char* name)
{
    unsigned int hash = nlStringHash(name);
    return fn_80054B28(context, hash, team);
}

extern "C" UnidentifiedVariant_80054AB8 fn_80054B28(
    InterpreterCore* context, const unsigned int& hash, cTeam* team)
{
    unsigned int localHash = hash;
    return UnidentifiedVariant_80054AB8(fn_80312360(
        context, fn_802DF3E4(context, &localHash), 1, team, 0));
}
