#ifndef GAME_CHARACTER_H
#define GAME_CHARACTER_H

#include "Game/CharacterEffects.h"
#include "NL/nlMath.h"
#include "NL/nlAVLTree.h"
#include "NL/nlTimer.h"
#include "types.h"

class Blinker;
class BlurHandler;
class cAnimInventory;
class cHeadTrack;
class EffectsTexturing;
class EffectsGroup;
class EmissionController;
class GLSkinMesh;
class PhysicsCharacter;
class cPN_SAnimController;
class cAnimInventory;
class cPoseAccumulator;
class cPoseNode;
class glModel;
struct CharacterInfo;
class RunningChecksum;
class DebugWriteCache;
class cSHierarchy;
class CharacterPhysicsData;
class AnimRetargetList;
struct UnidentifiedCharacterObject_8001C158;

enum eCharacterClass
{
    CHARACTER_CLASS_INVALID = -1,
    BIRDO = 0,
    DAISY = 1,
    DONKEYKONG = 2,
    HAMMERBROS = 3,
    KOOPA = 4,
    LUIGI = 5,
    MARIO = 6,
    PEACH = 7,
    TOAD = 8,
    WALUIGI = 9,
    WARIO = 10,
    YOSHI = 11,
    MYSTERY = 12,
};

enum eMovementState
{
    MOVEMENT_COAST = 0,
    MOVEMENT_DECELERATE_EXPONENTIAL = 1,
    MOVEMENT_FROM_ANIM = 2,
    MOVEMENT_FROM_ANIM_SEEK = 3,
    MOVEMENT_NONE = 4,
    MOVEMENT_RUNNING = 5,
    MOVEMENT_RUNNING_NO_TURN = 6,
    MOVEMENT_STRAFING = 7,
    MOVEMENT_UNUSED = 8,
};

enum eClassTypes
{
    CHARACTER = 0,
    PLAYER = 1,
    FIELDER = 2,
    GOALIE = 3,
    NUM_CLASSES = 4,
};

enum eCharacterModelType
{
    CharModel_Rigid = 0,
    CharModel_Blend = 1,
    CharModel_Num = 2,
};

struct UnidentifiedCharacterState_024
{
    UnidentifiedCharacterState_024()
    {
        UnidentifiedReset();
    }

    void UnidentifiedReset()
    {
        m_eMovementState = MOVEMENT_NONE;
        m_aDesiredFacingDirection = 0;
        m_aActualFacingDirection = 0;
        m_aPrevFacingDirection = 0;
        m_aDesiredMovementDirection = 0;
        m_aActualMovementDirection = 0;
        m_bFromAnimBlended = false;
        m_bOnScreen = false;
        m_fAnimAdjustBeginTime = 0.0f;
        m_fAnimAdjustEndTime = 0.0f;
        m_fDirectionSeekSpeed = 0.0f;
        m_fDirectionSeekFalloff = 0.0f;
        m_fAccel = 0.0f;
        m_fDecel = 0.0f;
        m_fDesiredSpeed = 0.0f;
        m_fActualSpeed = 0.0f;
        m_fLeanAmount = 0.0f;
        m_nAnimTurnAdjust = 0;
        m_fPlayerScale = 1.0f;
        m_fMovementScale = 1.0f;
        m_fDesiredPlayerScale = 1.0f;
        m_fDesiredMovementScale = 1.0f;
        nlVec3Set(m_v3Position, 0.0f, 0.0f, 0.0f);
        nlVec3Set(m_v3PrevPosition, 0.0f, 0.0f, 0.0f);
        nlVec3Set(m_v3Velocity, 0.0f, 0.0f, 0.0f);
        nlVec3Set(m_v3PrevVelocity, 0.0f, 0.0f, 0.0f);
        nlVec3Set(m_v3AnimMoveAdjust, 0.0f, 0.0f, 0.0f);
        m_tScaleTimer.UnidentifiedClear();
    }

    /* 0x00 */ eCharacterClass m_eCharacterClass;
    /* 0x04 */ eMovementState m_eMovementState;
    /* 0x08 */ bool m_bFromAnimBlended;
    /* 0x09 */ bool m_bOnScreen;
    /* 0x0A */ u8 mPadding0A[2];
    /* 0x0C */ nlVector3 m_v3Position;
    /* 0x18 */ nlVector3 m_v3PrevPosition;
    /* 0x24 */ nlVector3 m_v3Velocity;
    /* 0x30 */ nlVector3 m_v3PrevVelocity;
    /* 0x3C */ u16 m_aDesiredFacingDirection;
    /* 0x3E */ u16 m_aActualFacingDirection;
    /* 0x40 */ u16 m_aPrevFacingDirection;
    /* 0x42 */ u16 m_aDesiredMovementDirection;
    /* 0x44 */ u16 m_aActualMovementDirection;
    /* 0x46 */ u8 mPadding46[2];
    /* 0x48 */ float m_fAnimAdjustBeginTime;
    /* 0x4C */ float m_fAnimAdjustEndTime;
    /* 0x50 */ float m_fDirectionSeekSpeed;
    /* 0x54 */ float m_fDirectionSeekFalloff;
    /* 0x58 */ float m_fAccel;
    /* 0x5C */ float m_fDecel;
    /* 0x60 */ float m_fDesiredSpeed;
    /* 0x64 */ float m_fActualSpeed;
    /* 0x68 */ float m_fLeanAmount;
    /* 0x6C */ s16 m_nAnimTurnAdjust;
    /* 0x6E */ u8 mPadding6E[2];
    /* 0x70 */ nlVector3 m_v3AnimMoveAdjust;
    /* 0x7C */ float m_fPlayerScale;
    /* 0x80 */ float m_fMovementScale;
    /* 0x84 */ float m_fDesiredMovementScale;
    /* 0x88 */ float m_fDesiredPlayerScale;
    /* 0x8C */ Timer m_tScaleTimer;
};

class cCharacter
{
public:
    virtual ~cCharacter();
    cCharacter(eCharacterClass cc, const int* nModelID,
        cSHierarchy* pHierarchy, cAnimInventory* pAnimInventory,
        const CharacterPhysicsData* pPhysicsData,
        float fPhysicsCapsuleHeight, float fPhysicsCapsuleWidth,
        AnimRetargetList* pAnimRetargetList, int nIndex,
        eClassTypes eNewClassType);
    virtual void PostPhysicsUpdate();
    virtual void PrePhysicsUpdate();
    virtual void Unknown7(float dt);
    virtual void PreUpdate(float dt);
    virtual void UnidentifiedVirtual1C();
    virtual void ResetEffects();
    virtual void SetAnimID(int animID);
    virtual void Unknown8(unsigned short aDirection, bool bParam);
    virtual void SetPosition(const nlVector3& position);
    virtual void Update(float fDeltaT);
    virtual void Unknown10(
        const nlVector3& v3Position, unsigned short aDirection);
    virtual void Unknown11(void* context, DebugWriteCache* cache);
    virtual void Unknown12(RunningChecksum* pChecksum);

    void SetAnimState(int animID, bool useBlendTime, float nonDefaultBlendTime,
        bool restartCyclic, bool forceMirrorSwap);
    cPN_SAnimController* NewAnimController(int animID, bool bRestartCyclic,
        bool bForceMirrorSwap,
        void (*funcPlaybackSpeedCallback)(
            unsigned int, cPN_SAnimController*),
        unsigned int nPlaybackSpeedCallbackParam);
    bool ShouldStartCrossBlend(int animID);
    static void MatchAnimSpeedToCharacterSpeed(unsigned int nParam,
        cPN_SAnimController* pController);

    void UpdateMovementState(float fDeltaT);
    void UnidentifiedSetScale(float unidentifiedScale);
    void fn_80022D58(float fDeltaT);

    void SetElectrocutionTextureEnabled(bool isEnabled);
    void fn_8001F1D8();
    bool IsPlayingEffect(const EffectsGroup* effectGroup) const;
    bool fn_8001E2C0(const EffectsGroup* effectGroup) const;
    void EndEffect(const EffectsGroup* effectGroup);
    void KillEffect(const EffectsGroup* effectGroup);
    void PerformBlinking(GLSkinMesh* skinMesh, glModel* model) const;
    void UpdateBlinking(float fDeltaT);
    void StopPlayingAllTrackedSFX();
    void SetVelocity(const nlVector3& velocity);
    void CreateWorldMatrix();
    void SetFacingDirection(
        unsigned short dir, bool bSetMovementDirection);
    static float SeekSpeedExponential(float currentValue,
        float targetValue, float responsiveness, float deltaTime);
    void InitMovementStrafing(float fDirectionSeekSpeed,
        float fDirectionSeekFalloff, float fAccel, float fDecel);
    void InitMovementRunningNoTurn(float fAccel, float fDecel);
    void InitMovementRunning(float fDirectionSeekSpeed,
        float fDirectionSeekFalloff, float fAccel, float fDecel);
    void InitMovementNone(
        float fDirectionSeekSpeed, float fDirectionSeekFalloff);
    void InitMovementFromAnimSeek(
        float fDirectionSeekSpeed, float fDirectionSeekFalloff);
    void InitMovementFromAnim(short fDirectionSeekSpeed,
        const nlVector3& v3AnimMoveAdjust, float fAdjustEndTime,
        bool bBlended);
    void InitMovementDecelerateExponential(float fDecel);
    void InitMovementCoast();
    void EndBlur();
    nlVector3& GetPrevJointPosition(int jointIndex);
    void GetJointPositionFuture(nlVector3* v3Out, int nAnimIndex,
        int nJointIndex, float fTime, bool bAddRootTrans,
        bool bAddRootRot, bool bUsePrevPosition, bool bParam4);
    void GetCurrentAnimFuture(int nJointIndex, float fTime,
        nlVector3& v3Out, nlVector3& v3FutureRoot,
        unsigned short& outFacing);
    nlVector3& GetJointPosition(int jointIndex) const;
    s16 GetFacingDeltaToPosition(const nlVector3& position);
    s16 CalcAnimTurnAdjust(unsigned short aFacingDirection, unsigned short aDesiredFacingDirection, int nAnimID, float fParam);
    void AttachEffect(EmissionController* pEmissionController);
    GLSkinMesh* GetSkinMesh(int modelType) const;
    void PoseSkinMesh(cPoseAccumulator* pPoseAccumulator, int modelType);
    void fn_8001C510(int modelType);
    bool fn_8001C534(int modelType);
    void fn_8001C574();
    void fn_80022DAC(unsigned long uTextureID);
    void fn_80022DE8(unsigned long uTextureID);
    void fn_80022E24(unsigned long uTextureID);
    void fn_80022E60();
    void fn_80022D3C(float fParam0, float fParam1);
    void fn_8001EF78(float fParam);
    void AddRandomDirt();
    void fn_8001F1C0(int nParam);
    void fn_8001EE74(float fParam0, float fParam1, float fParam2);
    void fn_8001E304(float fSpeed, float fDeltaT);
    void fn_8001EF6C(float movementScale);
    void fn_8001DCE0(unsigned short aDirection);
    bool fn_8001E160();
    bool fn_8001E168() const;
    bool fn_8001E184();
    cAnimInventory* GetAnimInventory() const
    {
        return m_pAnimInventory;
    }
    int GetHeadJointIndex() const
    {
        return m_nHeadJointIndex;
    }
    /* 0x004 */ const CharacterPhysicsData* m_pPhysicsData;
    /* 0x008 */ GLSkinMesh* m_pSkinMesh[4];
    /* 0x018 */ bool unknown_0x018[4];
    /* 0x01C */ int m_ModelType;
    /* 0x020 */ PhysicsCharacter* m_pPhysicsCharacter;
    /* 0x024 */ UnidentifiedCharacterState_024 mUnidentified024;
    /* 0x0B8 */ cAnimInventory* m_pAnimInventory;
    /* 0x0BC */ cPoseAccumulator* m_pPoseAccumulator;
    /* 0x0C0 */ cPoseNode* m_pPoseTree;
    /* 0x0C4 */ cPoseNode** m_pAILayer;
    /* 0x0C8 */ cPN_SAnimController* m_pCurrentAnimController;
    /* 0x0CC */ int m_eAnimID;
    /* 0x0D0 */ AnimRetargetList* m_pAnimRetargetList;
    /* 0x0D4 */ cHeadTrack* m_pHeadTrack;
    /* 0x0D8 */ int m_nHeadJointIndex;
    /* 0x0DC */ int m_nBip01JointIndex_0xA4;
    /* 0x0E0 */ int m_nSpine1JointIndex;
    /* 0x0E4 */ int mUnidentified0E4;
    /* 0x0E8 */ int mUnidentified0E8;
    /* 0x0EC */ const char* m_szEffectsName;
    /* 0x0F0 */ eClassTypes m_eClassType;
    /* 0x0F4 */ bool m_bIsUsingElectrocutionTexture;
    /* 0x0F5 */ u8 unknown_0x0F5[3];
    /* 0x0F8 */ UnidentifiedCharacterObject_8001C158* mUnidentified0F8;
    /* 0x0FC */ void* mUnidentified0FC;
    /* 0x100 */ u32 mUnidentified100;
    /* 0x104 */ u32 mUnidentified104;
    /* 0x108 */ u32 mUnidentified108;
    /* 0x10C */ ResolvedTexture mUnidentified10C;
    /* 0x110 */ ResolvedTexture mUnidentified110;
    /* 0x114 */ ResolvedTexture mUnidentified114;
    /* 0x118 */ bool mUnidentified118;
    /* 0x119 */ u8 unknown_0x119[3];
    /* 0x11C */ const CharacterInfo* mUnidentified11C;
    /* 0x120 */ int mUnidentified120;
    /* 0x124 */ nlMatrix4 m_m4WorldMatrix;
    /* 0x164 */ float m_Dirt;
    /* 0x168 */ float m_MinDirt;
    /* 0x16C */ u32 mUnidentified16C;
    /* 0x170 */ BlurHandler* m_pBlurHandler;
    /* 0x174 */ Blinker* m_pBlinker;
    /* 0x178 */ float mUnidentified178;
    /* 0x17C */ bool mUnidentified17C;
    /* 0x17D */ bool mUnidentified17D;
    /* 0x17E */ bool mUnidentified17E;
    /* 0x17F */ bool mUnidentified17F;
    /* 0x180 */ bool mUnidentified180;
    /* 0x181 */ bool mUnidentified181;
    /* 0x182 */ bool mUnidentified182;
    /* 0x183 */ u8 unknown_0x183;
    /* 0x184 */ nlQuaternion mUnidentified184;
    /* 0x194 */ nlVector3 mUnidentified194;
    /* 0x1A0 */ float mUnidentified1A0;
    /* 0x1A4 */ float mUnidentified1A4;
    /* 0x1A8 */ float mUnidentified1A8;
    /* 0x1AC */ float mUnidentified1AC;
    /* 0x1B0 */ EffectsTexturing* m_pEffectsTexturing;
    /* 0x1B4 */ nlVector3 m_v3ScreenPosition;
    /* 0x1C0 */ nlAVLTreeSlotPool<unsigned long, nlVector3,
        DefaultKeyCompare<unsigned long> > mUnidentified1C0;
}; // total size: 0x1E4

#endif // GAME_CHARACTER_H
