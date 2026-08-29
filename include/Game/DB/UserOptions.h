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

class PowerupSettings
{
public:
    PowerupSettings();
    void InitializeDefaults();
    void OnSettingsUpdated() const;

    /* 0x0 */ int m_unk0;
    /* 0x4 */ int m_unk4;
    /* 0x8 */ int m_unk8;
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
    /* 0x04 */ int m_unk04;
    /* 0x08 */ int GameTime;
    /* 0x0C */ int m_unk0C;
    /* 0x10 */ int m_unk10;
    /* 0x14 */ bool PowerUps;
    /* 0x15 */ bool Shoot2Score;
    /* 0x16 */ bool BowserAttackEnabled;
    /* 0x17 */ bool RumbleEnabled;
    /* 0x18 */ bool m_unk18;
    /* 0x19 */ bool m_unk19;
    /* 0x1A */ bool m_unk1A;
};

class AudioSettings
{
public:
    AudioSettings();
    void ApplySettings();
    void fn_80109A50();
    void fn_80109ACC();
    void fn_80109B48();

    /* 0x00 */ int MusicVolume;
    /* 0x04 */ int SFXVolume;
    /* 0x08 */ int VoiceVolume;
    /* 0x0C */ int DefaultMusicVolume;
    /* 0x10 */ int DefaultSFXVolume;
    /* 0x14 */ int DefaultVoiceVolume;
};

#endif // GAME_DB_USER_OPTIONS_H
