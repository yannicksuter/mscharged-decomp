#include "Game/DB/UserOptions.h"

#include "Game/Audio/AudioSystem.h"
#include "Game/Audio/UnidentifiedRegistryPools.h"

#include <string.h>

// Audio category names hashed with nlStringLowerHash, kept in hash order, and
// the index of each category's volume block in AudioValues.
struct UnidentifiedAudioCategoryLookup
{
    UnidentifiedAudioCategoryLookup(u32 hash, s16 index) : mHash(hash), mIndex(index) {}

    u32 mHash;
    s16 mIndex;
};

static UnidentifiedAudioCategoryLookup sUnidentifiedAudioCategoryLookup[] = {
    UnidentifiedAudioCategoryLookup(0x00016A70, 4), // "sfx"
    UnidentifiedAudioCategoryLookup(0x05A165C0, 2), // "music"
    UnidentifiedAudioCategoryLookup(0x1883E244, 1), // "default"
    UnidentifiedAudioCategoryLookup(0x52030129, 3), // "dialogue"
    UnidentifiedAudioCategoryLookup(0xAB29FE50, 0), // "global"
};

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
    AudioValues* pValues = g_pAudioSystem->m_unkCC->m_unk10->m_unk08;
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
    pValues = g_pAudioSystem->m_unkCC->m_unk10->m_unk08;
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
    pValues = g_pAudioSystem->m_unkCC->m_unk10->m_unk08;
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

void AudioSettings::ApplyMusicVolume()
{
    MusicVolume = MusicVolume < 0 ? 0 : MusicVolume;
    MusicVolume = MusicVolume > 10 ? 10 : MusicVolume;
    AudioValues* pValues = g_pAudioSystem->m_unkCC->m_unk10->m_unk08;
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

void AudioSettings::ApplySFXVolume()
{
    SFXVolume = SFXVolume < 0 ? 0 : SFXVolume;
    SFXVolume = SFXVolume > 10 ? 10 : SFXVolume;
    AudioValues* pValues = g_pAudioSystem->m_unkCC->m_unk10->m_unk08;
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

void AudioSettings::ApplyVoiceVolume()
{
    VoiceVolume = VoiceVolume < 0 ? 0 : VoiceVolume;
    VoiceVolume = VoiceVolume > 10 ? 10 : VoiceVolume;
    AudioValues* pValues = g_pAudioSystem->m_unkCC->m_unk10->m_unk08;
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
    NumGames = 3;
    GameLimitType = 0;
    GoalLimit = 5;
    GameTime = 180;
    mHomePowerupsEnabled = true;
    mAwayPowerupsEnabled = true;
    mHomeMegastrikeEnabled = true;
    mAwayMegastrikeEnabled = true;
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

CheatSettings::CheatSettings()
{
    memset(this, 0, sizeof(CheatSettings));
    mCustomPowerups = 0;
    mEnvironmentCheat = 0;
    mPlayerCheat = 0;
}

void CheatSettings::InitializeDefaults()
{
    mCustomPowerups = 0;
    mEnvironmentCheat = 0;
    mPlayerCheat = 0;
}

void CheatSettings::OnSettingsUpdated() const
{
}

VisualSettings::VisualSettings()
{
    memset(this, 0, sizeof(VisualSettings));
    mIsAutoZoomCamera = true;
    mCameraZoomLevel = 0.5f;
}
