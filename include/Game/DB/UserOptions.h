#ifndef GAME_DB_USER_OPTIONS_H
#define GAME_DB_USER_OPTIONS_H

#include "types.h"

class VisualSettings
{
public:
    VisualSettings();

    /* 0x0 */ bool mIsAutoZoomCamera;
    /* 0x4 */ float mCameraZoomLevel;
};

enum eEnvironmentCheat
{
    ENV_CHEAT_NONE = 0,
    ENV_CHEAT_SECURE = 1,
    ENV_CHEAT_POWER = 2,
    ENV_CHEAT_VOLTAGE = 3,
    ENV_CHEAT_TILT = 4,
    ENV_CHEAT_WHITE_BALL = 5,
};

enum ePlayerCheat
{
    PLAYER_CHEAT_NONE = 0,
    PLAYER_CHEAT_DEVASTATING = 1,
    PLAYER_CHEAT_SAFE = 2,
    PLAYER_CHEAT_SKILL_SHOT = 3,
    PLAYER_CHEAT_GLASS_JAW = 4,
};

enum ePowerupCheat
{
    POWERUP_CHEAT_NONE = 0,
    POWERUP_CHEAT_EXPLOSIVES = 1,
    POWERUP_CHEAT_FREEZING = 2,
    POWERUP_CHEAT_SHELLS = 3,
    POWERUP_CHEAT_GIANT = 4,
    POWERUP_CHEAT_ACCELERATOR = 5,
    POWERUP_CHEAT_PEELIN_OUT = 6,
    POWERUP_CHEAT_HEAT_SEEKERS = 7,
    POWERUP_CHEAT_BOMBS_AWAY = 8,
    POWERUP_CHEAT_SUPER = 9,
    POWERUP_CHEAT_INFINITE = 10,
    POWERUP_CHEAT_BUTTERFINGERS = 11,
};

class CheatSettings
{
public:
    CheatSettings();
    void InitializeDefaults();
    void OnSettingsUpdated() const;

    int GetEnvironmentCheat() const { return mEnvironmentCheat; }
    int GetCustomPowerups() const { return mCustomPowerups; }
    int GetPlayerCheat() const { return mPlayerCheat; }

    /* 0x0 */ int mCustomPowerups;
    /* 0x4 */ int mEnvironmentCheat;
    /* 0x8 */ int mPlayerCheat;
};

enum eGameLimitType
{
    GAME_LIMIT_TIME = 0,
    GAME_LIMIT_GOALS = 1,
};

class GameplaySettings
{
public:
    enum eSkillLevel
    {
        TRAINING = 0,
        ROOKIE = 1,
        PROFESSIONAL = 2,
        SUPERSTAR = 3,
        LEGEND = 4,
    };

    GameplaySettings();
    void InitializeDefaults();
    void OnSettingsUpdated() const;

    /* 0x00 */ eSkillLevel SkillLevel;
    /* 0x04 */ eGameLimitType GameLimitType;
    /* 0x08 */ int GameTime;
    /* 0x0C */ int GoalLimit;
    /* 0x10 */ int NumGames;
    /* 0x14 */ bool mHomePowerupsEnabled;
    /* 0x15 */ bool mAwayPowerupsEnabled;
    /* 0x16 */ bool mHomeMegastrikeEnabled;
    /* 0x17 */ bool mAwayMegastrikeEnabled;
    /* 0x18 */ bool m_unk18;
    /* 0x19 */ bool m_unk19;
    /* 0x1A */ bool m_unk1A;
};

class AudioSettings
{
public:
    AudioSettings();
    void ApplySettings();
    void ApplyMusicVolume();
    void ApplySFXVolume();
    void ApplyVoiceVolume();

    /* 0x00 */ int MusicVolume;
    /* 0x04 */ int SFXVolume;
    /* 0x08 */ int VoiceVolume;
    /* 0x0C */ int DefaultMusicVolume;
    /* 0x10 */ int DefaultSFXVolume;
    /* 0x14 */ int DefaultVoiceVolume;
};

#endif // GAME_DB_USER_OPTIONS_H
