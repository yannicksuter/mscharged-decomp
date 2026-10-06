#ifndef GAME_GAME_H
#define GAME_GAME_H

#include "Game/GameEventQueue.h"
#include "Game/NetworkMessage.h"
#include "types.h"
#include "NL/nlMath.h"
#include "NL/CircularQueue.h"

class Clock;
class FuzzyTweaks;
class DebugWriteCache;
class WeatherManager;
class RunningChecksum;
class nlPolygonRegion;
class Terrain;
class CrowdRiot;
class AvoidablePolygon;
class AIContext;
struct DetermDataEvent;
struct CharacterImpactEvent;
struct GoalieSaveData;
struct PlayerAttackData;
struct CollisionPlayerWallData;
struct PeachPhotoData;
class cFielder;
class cPlayer;

void DestroyPowerups();
void DestroyGame();
void FreeCollisionPlayerWallData(CollisionPlayerWallData* node);
extern "C" void fn_8005B330(nlVector3*, float, float);

void SetFieldTilt(int relative, float xTilt, float yTilt);

class cGame;
void FinishMegaStrike(cGame* pGame);

class cGame : public NetworkMessageReceiver
{
    friend void SetFieldTilt(int relative, float xTilt, float yTilt);
    friend void FinishMegaStrike(cGame* pGame);

public:
    void QueueCharacterElectrocuted(CollisionPlayerWallData* data);

    virtual int ProcessMessage(NetworkMessage* message);
    virtual ~cGame();

    cGame(void* terrainIndex, int weatherType, bool startCrowdRiot);
    void ResetMegaStrikeMeterQueues();
    void SendMegaStrikeMeter(bool value);
    void SendRemainingMegaStrikeMeter();
    void InitMegaStrikeGameplay();
    void CleanupMegaStrikeGameplay();
    void ClearMegaStrikeCleanupPending();
    void ResetGameFields();
    void RegisterEventListeners();
    void BeginGame(bool bRematch, bool bStraightToKickoff);
    void CheckForGoal();
    void ReceiveCustomDetermData(DetermDataEvent* data);
    void OnSuddenDeath();
    void OnGameOver();
    void SendPauseGameEvent();
    void SendResumingGameEvent();
    void SetMegaStrikeGameplay(bool enabled, int defendingSide, int numShots);
    void StartSlowDown(float timeScale, float transitionTime);
    float GetNormalizedGameTime();
    float GetGameTime();
    float GetGameDuration() const { return m_fGameDuration; }
    void fn_800586C0();
    void fn_80058704();
    void ResetForKickOff();
    void fn_80058A78(float seconds);
    void BlowUpPowerups(
        const nlPolygonRegion& region,
        float fExplosionRadius);
    void ResetPowerups(bool clearPowerUps);
    void ResetCachedPlayerDistances();
    void CopyPlayerDistanceSnapshot(void* snapshotData);
    void SendNISLoadedCustomDeterm(u8 machineBits);
    void SendMegaStrike(int side, int playerId, float numBalls, float accuracy);
    void SendMegaStrikePlayerReady();
    void SendMegaStrikeKillCursor();
    void SendMegaStrikeGoalie(unsigned int side, unsigned int target, float score);
    void SendSlowDownEnd();
    void PreUpdate(float deltaTime);
    void RandomizePlayerUpdateOrder();
    void ResetCharacters();
    void SendPlayerVisibility();
    void UpdateCachedGameData(float fDeltaT);
    float fn_8005B748(int firstIndex, int secondIndex);
    cPlayer* GetClosestPlayer(int playerIndex, int side, int rank);
    void SetPotentialScorer(cPlayer* pPlayer);
    void ChecksumState(RunningChecksum* runningChecksum);
    static void UpdatePowerUpObjects(float fDeltaT);
    void Update(float fDeltaT);
    void SyncLog(void* checksum, DebugWriteCache* cache);
    void ChangeGameState(int state);
    void InitGameState(int state);
    void LoadTerrain(int terrain);
    void SetDifficulty(int diff0, int diff1, int diff2, bool param4);
    void SetMegaStrikeShotResult(int shotIndex, bool scored);
    void ResumeAfterPresentation();
    void QueueNIS(NISData* pData);

    inline bool IsGameplayOrOvertime()
    {
        return (m_eGameState == 5 || m_eGameState == 6);
    }

    inline int GetGameState() const { return m_eGameState; }
    inline bool IsLastTeamToScore(int side) const { return m_nLastTeamToScore == side; }
    inline bool IsCaptainShotToScoreOn() const { return mbCaptainShotToScoreOn; }
    inline u32 GetMegaStrikeGoalMask() const { return m_uMegastrikeResults; }
    float GetXAxisTilt() const { return mfXTilt; }
    float GetYAxisTilt() const { return mfYTilt; }
    const nlVector3& GetTiltDirection() const { return mTiltDirection; }

    /* 0x04 */ FuzzyTweaks* m_pFuzzyTweaks;
    /* 0x08 */ Clock* m_pGameClock;
    /* 0x0C */ Clock* m_pPostResetClock;
    /* 0x10 */ Clock* m_pPostGameDoneClock;

private:
    inline void RegisterDetermGameFields(DebugWriteCache* cache);
    static void PlayEndGamePresentation();

    /* 0x14 */ AIContext* mpAIContext;

public:
    /* 0x18 */ int m_eGameState;
    /* 0x1C */ float m_fGameDuration;
    /* 0x20 */ bool m_bBallInNet;

private:
    /* 0x21 */ u8 mPad21[0x03];

public:
    /* 0x24 */ int m_nLastTeamToScore;

    /* 0x28 */ u32 m_uMegastrikeNumShots;
    /* 0x2C */ u32 m_uMegastrikeCurShot;

    /* 0x30 */ u32 m_uMegastrikeGoals;

private:
    /* 0x34 */ int m_nMegastrikeDefendingTeam;

public:
    /* 0x38 */ u32 m_uMegastrikeResults;

    /* 0x3C */ cFielder* mpMegaStrikeShooter;

public:
    /* 0x40 */ bool mbCaptainShotToScoreOn;
    /* 0x41 */ bool mbMegaStrikePositiveNet;
    /* 0x42 */ bool mbMegaStrikePlayerReady;

private:
    /* 0x43 */ u8 mPad43;

public:
    /* 0x44 */ cPlayer* m_pScorer;
    /* 0x48 */ cPlayer* m_pAssister;
    /* 0x4C */ cPlayer* m_pTeamTouch[2];
    /* 0x54 */ cPlayer* m_pRandomPlayersArray[10];

private:
    /* 0x7C */ float mfTiltLevelTimer;

public:
    /* 0x80 */ float mfXTilt;
    /* 0x84 */ float mfYTilt;

private:
    /* 0x88 */ float mfXTiltMin;
    /* 0x8C */ float mfXTiltMax;
    /* 0x90 */ float mfYTiltMin;
    /* 0x94 */ float mfYTiltMax;
    /* 0x98 */ float mfTiltSpeed;
    /* 0x9C */ float mfDesiredTiltSpeed;

public:
    /* 0xA0 */ float mUnidentified0A0;

private:
    /* 0xA4 */ u16 maTiltDir;
    /* 0xA6 */ u16 maDesiredTiltDir;
    /* 0xA8 */ u32 muTiltFrames;
    /* 0xAC */ nlVector3 mTiltDirection;
    /* 0xB8 */ u32 muCountdownBeepsRemaining;

public:
    /* 0xBC */ bool mbMegaStrikePlayerReadySent;
    /* 0xBD */ bool mbMegaStrikeCleanupPending;

private:
    /* 0xBE */ u8 mPadBE[0x02];

public:
    /* 0xC0 */ CircularQueueBase<bool> mReceivedMegaStrikeMeter;

private:
    /* 0xD0 */ u8 mReceivedMegaStrikeMeterStorage[0x64];

public:
    /* 0x134 */ CircularQueueBase<bool> mPendingMegaStrikeMeter;

private:
    /* 0x144 */ u8 mPendingMegaStrikeMeterStorage[0x10];

public:
    /* 0x154 */ cPlayer* m_nClosestPlayers[10][2][5];
    /* 0x2E4 */ float m_fCachedPlayerDistances[10][10];
    /* 0x474 */ float m_fCachedBallPlayerDistances[10];

    void QueueChainNisEnd(ShotAtGoalData* data);
    void fn_80060BFC(CollisionBulletBillData& data);

    /* 0x49C */ GameEventQueue mUnidentified49C;

    /* 0x10D8 */ Terrain* mpTerrain;
    /* 0x10DC */ WeatherManager* mpWeatherManager;
    /* 0x10E0 */ CrowdRiot* mpCrowdRiot;
    /* 0x10E4 */ AvoidablePolygon* mpBoundaryAvoidables[4];
};

extern cGame* g_pGame;

extern "C" void fn_8006040C(cGame*, cFielder*);
extern "C" void fn_80060804(cGame*, cFielder*);

extern "C" void fn_8005D210(cGame*, LightningStrikeData*);
void DeliverGoalieSaveEvent(cGame* pGame, const GoalieSaveData* pData);
void DeliverGoalieKickEvent(cGame* pGame, const GoalieSaveData* pData);
void DeliverGoalieExertEvent(cGame* pGame, const GoalieSaveData* pData);
void DeliverGoalieDekeAttackAttemptEvent(cGame* pGame, const PlayerAttackData* pData);
void DeliverGoalieSlamAttackAttemptEvent(cGame* pGame, const PlayerAttackData* pData);
void DeliverGoalieDekeAttackSuccessEvent(cGame* pGame, const PlayerAttackData* pData);
void DeliverGoalieSlamAttackSuccessEvent(cGame* pGame, const PlayerAttackData* pData);

extern "C" void fn_8005D74C(cGame* game, const GoalieSaveData* pSaveData);

void DeliverMegaStrikeMeterEndEvent(cGame* pGame);
void DeliverPeachFlashEvent(cGame* pGame, PeachPhotoData* pEventData);
void DeliverPeachCameraFlashEvent(cGame* pGame, PeachPhotoData* pEventData);
void DeliverPeachCamerasDownEvent(cGame* pGame, PeachPhotoData* pEventData);
void DeliverPeachCamerasAwayEvent(cGame* pGame, PeachPhotoData* pEventData);
extern "C" void fn_80060A00(cGame* pGame, cFielder* pFielder);
extern "C" void fn_80060FF4(cGame* pGame, const CharacterImpactEvent* pEventData);

extern "C" void fn_800611F0(cGame* pGame, const void* pEventData);

void DeliverShotPresentationEvent(cGame* pGame);
void DeliverShotPresentationEndEvent(cGame* pGame);
void DeliverCaptainClashPresentationEvent(cGame* pGame);
void DeliverWindupPresentationEvent(cGame* pGame);
void DeliverWindupPresentationEndEvent(cGame* pGame);
void QueueCollisionCrowdEvent(cGame* pGame, CollisionCrowdData* pData);
void QueueAttackAttemptEvent(cGame* pGame, PlayerAttackData* pData);
void DeliverWindupShotEvent(cGame* pGame, ShotAtGoalData* pData);
void DeliverMegaStrikeMeterStartEvent(cGame* pGame, MegaStrikeMeterData* pData);
void DeliverMegaStrikeMeterFirstEvent(cGame* pGame, MegaStrikeMeterData* pData);
void DeliverMegaStrikeMeterSecondEvent(cGame* pGame, MegaStrikeMeterData* pData);
void DeliverMegaStrikeIntroEvent(cGame* pGame, cFielder* pFielder);
void DeliverMontyReappearEvent(cGame* pGame, const CharacterImpactEvent* pEventData);

#endif // GAME_GAME_H
