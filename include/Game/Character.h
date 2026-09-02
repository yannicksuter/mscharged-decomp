#ifndef GAME_CHARACTER_H
#define GAME_CHARACTER_H

#include "NL/nlMath.h"
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

class cCharacter
{
public:
    virtual ~cCharacter();
    virtual void PostPhysicsUpdate();
    virtual void PrePhysicsUpdate();
    virtual void Unknown7();
    virtual void PreUpdate(float dt);
    virtual void UnidentifiedVirtual1C();
    virtual void ResetEffects();
    virtual void SetAnimID(int animID);
    virtual void Unknown8(unsigned short aDirection, bool bParam);
    virtual void SetPosition(const nlVector3& position);
    virtual void Update(float fDeltaT);
    virtual void Unknown10();
    virtual void Unknown11();
    virtual void Unknown12();

    void SetAnimState(int animID, bool useBlendTime, float nonDefaultBlendTime,
        bool restartCyclic, bool forceMirrorSwap);
    cPN_SAnimController* NewAnimController(int animID, bool bRestartCyclic,
        bool bForceMirrorSwap,
        void (*funcPlaybackSpeedCallback)(
            unsigned int, cPN_SAnimController*),
        unsigned int nPlaybackSpeedCallbackParam);
    bool ShouldStartCrossBlend(int animID);

    void SetElectrocutionTextureEnabled(bool isEnabled);
    bool IsPlayingEffect(const EffectsGroup* effectGroup) const;
    void EndEffect(const EffectsGroup* effectGroup);
    void KillEffect(const EffectsGroup* effectGroup);
    void PerformBlinking(GLSkinMesh* skinMesh, glModel* model) const;
    void UpdateBlinking(float fDeltaT);
    void SetVelocity(const nlVector3& velocity);
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
    void AttachEffect(EmissionController* pEmissionController);
    GLSkinMesh* GetSkinMesh(int modelType) const;
    cAnimInventory* GetAnimInventory() const
    {
        return m_pAnimInventory;
    }
    int GetHeadJointIndex() const
    {
        return m_nHeadJointIndex;
    }
    /* 0x004 */ u8 unknown_0x004[0x04];
    /* 0x008 */ GLSkinMesh* m_pSkinMesh[4];
    /* 0x018 */ bool unknown_0x018[4];
    /* 0x01C */ int m_ModelType;
    /* 0x020 */ PhysicsCharacter* m_pPhysicsCharacter;
    /* 0x024 */ eCharacterClass m_eCharacterClass;
    /* 0x028 */ eMovementState m_eMovementState;
    /* 0x02C */ bool m_bFromAnimBlended;
    /* 0x02D */ u8 unknown_0x02D[0x03];
    /* 0x030 */ nlVector3 m_v3Position;
    /* 0x03C */ nlVector3 m_v3PrevPosition;
    /* 0x048 */ nlVector3 m_v3Velocity;
    /* 0x054 */ nlVector3 mUnidentified054;
    /* 0x060 */ u16 m_aDesiredFacingDirection;
    /* 0x062 */ u16 m_aActualFacingDirection;
    /* 0x064 */ u16 m_aPrevFacingDirection;
    /* 0x066 */ u16 m_aDesiredMovementDirection;
    /* 0x068 */ u16 m_aActualMovementDirection;
    /* 0x06A */ u8 unknown_0x06A[0x02];
    /* 0x06C */ float m_fAnimAdjustBeginTime;
    /* 0x070 */ float m_fAnimAdjustEndTime;
    /* 0x074 */ float m_fDirectionSeekSpeed;
    /* 0x078 */ float m_fDirectionSeekFalloff;
    /* 0x07C */ float m_fAccel;
    /* 0x080 */ float m_fDecel;
    /* 0x084 */ float m_fDesiredSpeed;
    /* 0x088 */ float m_fActualSpeed;
    /* 0x08C */ float m_fLeanAmount;
    /* 0x090 */ s16 m_nAnimTurnAdjust;
    /* 0x092 */ u8 unknown_0x092[0x02];
    /* 0x094 */ nlVector3 m_v3AnimMoveAdjust;
    /* 0x0A0 */ float mUnidentified0A0;
    /* 0x0A4 */ u8 unknown_0x0A4[0x14];
    /* 0x0B8 */ cAnimInventory* m_pAnimInventory;
    /* 0x0BC */ cPoseAccumulator* m_pPoseAccumulator;
    /* 0x0C0 */ cPoseNode* m_pPoseTree;
    /* 0x0C4 */ cPoseNode** m_pAILayer;
    /* 0x0C8 */ cPN_SAnimController* m_pCurrentAnimController;
    /* 0x0CC */ int m_eAnimID;
    /* 0x0D0 */ u8 unknown_0x0D0[0x04];
    /* 0x0D4 */ cHeadTrack* m_pHeadTrack;
    /* 0x0D8 */ int m_nHeadJointIndex;
    /* 0x0DC */ int m_nBip01JointIndex_0xA4;
    /* 0x0E0 */ u8 unknown_0x0E0[0x10];
    /* 0x0F0 */ eClassTypes m_eClassType;
    /* 0x0F4 */ bool m_bIsUsingElectrocutionTexture;
    /* 0x0F5 */ u8 unknown_0x0F5[0x27];
    /* 0x11C */ const CharacterInfo* mUnidentified11C;
    /* 0x120 */ int mUnidentified120;
    /* 0x124 */ nlMatrix4 m_m4WorldMatrix;
    /* 0x164 */ u8 unknown_0x164[0x0C];
    /* 0x170 */ BlurHandler* m_pBlurHandler;
    /* 0x174 */ Blinker* m_pBlinker;
    /* 0x178 */ float mUnidentified178;
    /* 0x17C */ bool mUnidentified17C;
    /* 0x17D */ u8 unknown_0x17D[0x33];
    /* 0x1B0 */ EffectsTexturing* m_pEffectsTexturing;
    /* 0x1B4 */ u8 unknown_0x1B4[0x30];
}; // total size: 0x1E4

#endif // GAME_CHARACTER_H
