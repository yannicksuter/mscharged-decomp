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
    /* 0x04 */ int GameLimitType;
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
