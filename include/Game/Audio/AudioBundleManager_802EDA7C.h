#ifndef GAME_AUDIO_AUDIO_BUNDLE_MANAGER_802EDA7C_H
#define GAME_AUDIO_AUDIO_BUNDLE_MANAGER_802EDA7C_H

#include "types.h"

class nlChunk;
class AudioBackend_8035B8E8;

class AudioResourceRuntime_802F479C
{
public:
    AudioResourceRuntime_802F479C();

    virtual void fn_802F479C_0();
    virtual bool fn_802F479C_1(void* data, unsigned int size, bool unknown);
    virtual void fn_802F479C_2();
    virtual void* fn_802F479C_3();

    void fn_802F48FC(void* data, unsigned int size);

private:
    friend class AudioBundleManager_802EDA7C;

    u8 m_Unknown04[0x1C];
    void* m_Unknown20;
    void* m_Unknown24;
    void* m_Unknown28;
};

class AudioBundleManager_802EDA7C
{
public:
    AudioBundleManager_802EDA7C();

    virtual bool fn_802EE170() { return m_Loaded; }
    virtual bool fn_800ED8B8();
    virtual void fn_802EDEE4(const char* path);
    virtual bool fn_802EDB00();
    virtual void fn_802EDA7C_4() = 0;
    virtual void fn_802EDA7C_5(float deltaTime) = 0;
    virtual void fn_802EE16C() { }
    virtual void fn_802EDC34(nlChunk* chunk);
    virtual void fn_802EE160() { m_Loaded = true; }

    void* GetSoundMap() const { return m_Chunk13500; }

protected:
    static void fn_802EDB14(
        void* data, unsigned long size, void* manager);

    AudioBackend_8035B8E8* m_Backend;
    bool m_Initialized;
    u8 m_Unknown09[3];
    void* m_Chunk13100;
    void* m_Chunk13400;
    void* m_Chunk13500;
    AudioResourceRuntime_802F479C m_Runtime;
    bool m_Loaded;
    u8 m_Unknown45[3];
    void* m_LoadedData;
};

#endif // GAME_AUDIO_AUDIO_BUNDLE_MANAGER_802EDA7C_H
