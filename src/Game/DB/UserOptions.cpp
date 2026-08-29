#include "Game/DB/UserOptions.h"

#include "Game/Audio/AudioLoadMode_806E201C.h"

#include <string.h>

static const float VOLUME_TABLE[] = {
    -96.0f,
    -36.0f,
    -18.0f,
    -15.0f,
    -12.0f,
    -9.0f,
    -6.0f,
    -4.5f,
    -3.0f,
    -1.5f,
    0.0f,
};

AudioSettings::AudioSettings()
{
    memset(this, 0, sizeof(AudioSettings));
    MusicVolume = 10;
    SFXVolume = 10;
    VoiceVolume = 10;
    DefaultMusicVolume = 10;
    DefaultSFXVolume = 10;
    DefaultVoiceVolume = 10;
}

void AudioSettings::ApplySettings()
{
    MusicVolume = MusicVolume < 0 ? 0 : MusicVolume;
    MusicVolume = MusicVolume > 10 ? 10 : MusicVolume;
    AudioValues_801098E4* pValues = lbl_806E201C->m_unkCC->m_unk10->m_unk08;
    float volume = VOLUME_TABLE[MusicVolume];
    if (volume < pValues->m_unk68)
    {
        pValues->m_unk5C = pValues->m_unk68;
    }
    else if (volume > pValues->m_unk6C)
    {
        pValues->m_unk5C = pValues->m_unk6C;
    }
    else
    {
        pValues->m_unk5C = volume;
    }
    pValues->m_unk60 = 0.0f;

    SFXVolume = SFXVolume < 0 ? 0 : SFXVolume;
    SFXVolume = SFXVolume > 10 ? 10 : SFXVolume;
    pValues = lbl_806E201C->m_unkCC->m_unk10->m_unk08;
    volume = VOLUME_TABLE[SFXVolume];
    if (volume < pValues->m_unkB8)
    {
        pValues->m_unkAC = pValues->m_unkB8;
    }
    else if (volume > pValues->m_unkBC)
    {
        pValues->m_unkAC = pValues->m_unkBC;
    }
    else
    {
        pValues->m_unkAC = volume;
    }
    pValues->m_unkB0 = 0.0f;

    VoiceVolume = VoiceVolume < 0 ? 0 : VoiceVolume;
    VoiceVolume = VoiceVolume > 10 ? 10 : VoiceVolume;
    pValues = lbl_806E201C->m_unkCC->m_unk10->m_unk08;
    volume = VOLUME_TABLE[VoiceVolume];
    if (volume < pValues->m_unk90)
    {
        pValues->m_unk84 = pValues->m_unk90;
    }
    else if (volume > pValues->m_unk94)
    {
        pValues->m_unk84 = pValues->m_unk94;
    }
    else
    {
        pValues->m_unk84 = volume;
    }
    pValues->m_unk88 = 0.0f;
}

void AudioSettings::fn_80109A50()
{
    MusicVolume = MusicVolume < 0 ? 0 : MusicVolume;
    MusicVolume = MusicVolume > 10 ? 10 : MusicVolume;
    AudioValues_801098E4* pValues = lbl_806E201C->m_unkCC->m_unk10->m_unk08;
    float volume = VOLUME_TABLE[MusicVolume];
    if (volume < pValues->m_unk68)
    {
        pValues->m_unk5C = pValues->m_unk68;
    }
    else if (volume > pValues->m_unk6C)
    {
        pValues->m_unk5C = pValues->m_unk6C;
    }
    else
    {
        pValues->m_unk5C = volume;
    }
    pValues->m_unk60 = 0.0f;
}

void AudioSettings::fn_80109ACC()
{
    SFXVolume = SFXVolume < 0 ? 0 : SFXVolume;
    SFXVolume = SFXVolume > 10 ? 10 : SFXVolume;
    AudioValues_801098E4* pValues = lbl_806E201C->m_unkCC->m_unk10->m_unk08;
    float volume = VOLUME_TABLE[SFXVolume];
    if (volume < pValues->m_unkB8)
    {
        pValues->m_unkAC = pValues->m_unkB8;
    }
    else if (volume > pValues->m_unkBC)
    {
        pValues->m_unkAC = pValues->m_unkBC;
    }
    else
    {
        pValues->m_unkAC = volume;
    }
    pValues->m_unkB0 = 0.0f;
}

void AudioSettings::fn_80109B48()
{
    VoiceVolume = VoiceVolume < 0 ? 0 : VoiceVolume;
    VoiceVolume = VoiceVolume > 10 ? 10 : VoiceVolume;
    AudioValues_801098E4* pValues = lbl_806E201C->m_unkCC->m_unk10->m_unk08;
    float volume = VOLUME_TABLE[VoiceVolume];
    if (volume < pValues->m_unk90)
    {
        pValues->m_unk84 = pValues->m_unk90;
    }
    else if (volume > pValues->m_unk94)
    {
        pValues->m_unk84 = pValues->m_unk94;
    }
    else
    {
        pValues->m_unk84 = volume;
    }
    pValues->m_unk88 = 0.0f;
}

GameplaySettings::GameplaySettings()
{
    memset(this, 0, sizeof(GameplaySettings));
    SkillLevel = ROOKIE;
    m_unk10 = 3;
    m_unk04 = 0;
    m_unk0C = 5;
    GameTime = 180;
    PowerUps = true;
    Shoot2Score = true;
    BowserAttackEnabled = true;
    RumbleEnabled = true;
    m_unk18 = true;
    m_unk19 = true;
    m_unk1A = true;
}

void GameplaySettings::InitializeDefaults()
{
}

void GameplaySettings::OnSettingsUpdated() const
{
}

PowerupSettings::PowerupSettings()
{
    memset(this, 0, sizeof(PowerupSettings));
    m_unk0 = 0;
    m_unk4 = 0;
    m_unk8 = 0;
}

void PowerupSettings::InitializeDefaults()
{
    m_unk0 = 0;
    m_unk4 = 0;
    m_unk8 = 0;
}

void PowerupSettings::OnSettingsUpdated() const
{
}

VisualSettings::VisualSettings()
{
    memset(this, 0, sizeof(VisualSettings));
    mIsAutoZoomCamera = true;
    mCameraZoomLevel = 0.5f;
}
