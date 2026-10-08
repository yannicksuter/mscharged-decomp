#include "NL/nlDLListContainer.inl"
#include "Game/DB/UserOptions.h"

#include "Game/Audio/AudioSystem.h"
#include "Game/Audio/AudioCalculation.h"
#include "Game/Audio/RegistryPools.h"

#include <string.h>

// Audio category names hashed with nlStringLowerHash, kept in hash order, and
// the index of each category's calculation slider.
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
    AudioCalculationSlider* sliders = ((AudioCalculationTable*)
        g_pAudioSystem->GetBundleManager()->GetCalculationTable())->sliders;
    float volume = VOLUME_TABLE[MusicVolume];
    if (volume < sliders[2].minimum)
    {
        sliders[2].target = sliders[2].minimum;
    }
    else if (volume > sliders[2].maximum)
    {
        sliders[2].target = sliders[2].maximum;
    }
    else
    {
        sliders[2].target = volume;
    }
    sliders[2].remainingTime = 0.0f;

    SFXVolume = SFXVolume < 0 ? 0 : SFXVolume;
    SFXVolume = SFXVolume > 10 ? 10 : SFXVolume;
    sliders = ((AudioCalculationTable*)
        g_pAudioSystem->GetBundleManager()->GetCalculationTable())->sliders;
    volume = VOLUME_TABLE[SFXVolume];
    if (volume < sliders[4].minimum)
    {
        sliders[4].target = sliders[4].minimum;
    }
    else if (volume > sliders[4].maximum)
    {
        sliders[4].target = sliders[4].maximum;
    }
    else
    {
        sliders[4].target = volume;
    }
    sliders[4].remainingTime = 0.0f;

    VoiceVolume = VoiceVolume < 0 ? 0 : VoiceVolume;
    VoiceVolume = VoiceVolume > 10 ? 10 : VoiceVolume;
    sliders = ((AudioCalculationTable*)
        g_pAudioSystem->GetBundleManager()->GetCalculationTable())->sliders;
    volume = VOLUME_TABLE[VoiceVolume];
    if (volume < sliders[3].minimum)
    {
        sliders[3].target = sliders[3].minimum;
    }
    else if (volume > sliders[3].maximum)
    {
        sliders[3].target = sliders[3].maximum;
    }
    else
    {
        sliders[3].target = volume;
    }
    sliders[3].remainingTime = 0.0f;
}

void AudioSettings::ApplyMusicVolume()
{
    MusicVolume = MusicVolume < 0 ? 0 : MusicVolume;
    MusicVolume = MusicVolume > 10 ? 10 : MusicVolume;
    AudioCalculationSlider* sliders = ((AudioCalculationTable*)
        g_pAudioSystem->GetBundleManager()->GetCalculationTable())->sliders;
    float volume = VOLUME_TABLE[MusicVolume];
    if (volume < sliders[2].minimum)
    {
        sliders[2].target = sliders[2].minimum;
    }
    else if (volume > sliders[2].maximum)
    {
        sliders[2].target = sliders[2].maximum;
    }
    else
    {
        sliders[2].target = volume;
    }
    sliders[2].remainingTime = 0.0f;
}

void AudioSettings::ApplySFXVolume()
{
    SFXVolume = SFXVolume < 0 ? 0 : SFXVolume;
    SFXVolume = SFXVolume > 10 ? 10 : SFXVolume;
    AudioCalculationSlider* sliders = ((AudioCalculationTable*)
        g_pAudioSystem->GetBundleManager()->GetCalculationTable())->sliders;
    float volume = VOLUME_TABLE[SFXVolume];
    if (volume < sliders[4].minimum)
    {
        sliders[4].target = sliders[4].minimum;
    }
    else if (volume > sliders[4].maximum)
    {
        sliders[4].target = sliders[4].maximum;
    }
    else
    {
        sliders[4].target = volume;
    }
    sliders[4].remainingTime = 0.0f;
}

void AudioSettings::ApplyVoiceVolume()
{
    VoiceVolume = VoiceVolume < 0 ? 0 : VoiceVolume;
    VoiceVolume = VoiceVolume > 10 ? 10 : VoiceVolume;
    AudioCalculationSlider* sliders = ((AudioCalculationTable*)
        g_pAudioSystem->GetBundleManager()->GetCalculationTable())->sliders;
    float volume = VOLUME_TABLE[VoiceVolume];
    if (volume < sliders[3].minimum)
    {
        sliders[3].target = sliders[3].minimum;
    }
    else if (volume > sliders[3].maximum)
    {
        sliders[3].target = sliders[3].maximum;
    }
    else
    {
        sliders[3].target = volume;
    }
    sliders[3].remainingTime = 0.0f;
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
